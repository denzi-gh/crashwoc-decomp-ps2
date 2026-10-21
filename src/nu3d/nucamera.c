/*
 * Unit: nu3d/nucamera
 *
 * Functions:
 *   0x00112118 NuCameraSetProjectionMtx
 *   0x001122f8 NuCameraSetEx
 *   0x001129b0 NuCameraSaveState
 *   0x00112fc8 NuCameraRestoreState
 *   0x00113618 NuCameraCreate
 *   0x00113690 NuCameraDestroy
 *   0x001136b8 NuCameraSetVPortClipMtx
 *   0x00113808 NuCameraSetScissorClipMtx
 *   0x00113958 NuCameraSet
 *   0x00113988 NuCameraClearStateBuffer
 *   0x00113990 NuCameraSetReflect
 *   0x00113b18 NuCameraSetAxes
 *   0x00113b40 NuCameraGetAxes
 *   0x00113b68 NuCameraGetCam
 *   0x00113b78 NuCameraGetMtx
 *   0x00113b88 NuCameraGetViewMtx
 *   0x00113b98 NuCameraGetProjectionMtx
 *   0x00113ba8 NuCameraGetScalingMtx
 *   0x00113bb8 NuCameraGetVPCSMtx
 *   0x00113bc8 NuCameraGetPCMtx
 *   0x00113bd8 NuCameraGetPCSMtx
 *   0x00113be8 NuCameraGetVPCMtx
 *   0x00113bf8 NuCameraGetClipMtx
 *   0x00113d18 NuCameraTransformView
 *   0x00113e28 NuCameraTransformPerspective
 *   0x00113f38 NuCameraTransformScreenClip
 *   0x00114048 NuCameraTransformScreen
 *   0x00114158 NuCameraTransformScreenVU0
 *   0x00114268 NuCameraClipTestExtents
 *   0x001142d8 NuCameraClipTestPoints
 *   0x00114488 NuCameraDistSqr
 *   0x001144b0 NuCameraDist
 *   0x001144d8 NuCameraGetTrans
 *   0x00114500 NuCameraGetClippingRatios
 *   0x00114518 NuCameraClipTestPointVport
 *   0x00114540 NuCameraClipTestPointScissor
 *   0x00114568 NuCameraGet
 *   0x00114608 NuCameraTransformScissorClip
 */

#include "nu_libc.h"

#include "nu3d/nustream.h"
#include "nucore/nuerror.h"
#include "numath/numathtypes.h"
#include "numath/numtx.h"
#include "numath/nutrig.h"
#include "numath/nuvec.h"
#include "nu3d/nucamera.h"
#include "nu3d/nuvport.h"

typedef struct NuCameraReflect {
    int type;
    struct numtx_s mtx;
    struct numtx_s clip;
} NuCameraReflect;

struct NuColour4;

/* The state-buffer stride is 0x2b4; the camera occupies its first 0x60 bytes. */
typedef struct NuCameraState {
    struct nucamera_s camera;
    struct numtx_s view;
    struct numtx_s projection;
    struct numtx_s scaling;
    struct numtx_s vport_clip;
    struct numtx_s scissor_clip;
    struct numtx_s vpc;
    struct numtx_s pc;
    struct numtx_s vpcs;
    struct numtx_s pcs;
    int reflect;
    float clip_x;
    float clip_y;
    float clip_z;
    float clip_w;
} NuCameraState;


static struct nuvec_s cam_axes = { 1.0f, 1.0f, 1.0f };
struct nucamera_s global_camera = { 0 };
static int camfx = 0;
static int cam_state_count = 0;
float vw = 320.0f;
float vh = 112.0f;
float sw = 2047.0f;
float sh = 2047.0f;
float nucamera_farclip_hack = 0.1f;

static float zx;
static float zy;
static float zxs;
static float zys;
static NuCameraReflect global_reflect;
static struct numtx_s vmtx __attribute__((aligned(16)));
static struct numtx_s pmtx;
static struct numtx_s smtx;
static struct numtx_s vpmtx;
static struct numtx_s vpc_sci_mtx;
static struct numtx_s vpc_vport_mtx;
static struct numtx_s pc_vport_mtx;
static struct numtx_s vpsmtx;
static struct numtx_s psmtx;
static NuCameraState cam_state[16];

extern float NuTrigTable[];

extern void NuMemFreeFn(void *ptr, char *file, int line);
extern int NuVecClipTestPointVU0(struct nuvec_s *pos, struct numtx_s *clipmtx);
extern void *NuMemAllocFn(int size, char *file, int line);
extern void NuCameraInitClipTestVU0(struct numtx_s *vpc, struct numtx_s *scissor);
extern int NuCameraClipTestExtentsVU0(struct nuvec_s *min, struct nuvec_s *max);
extern void NuMtxMulVU0(struct numtx_s *dst, struct numtx_s *a, struct numtx_s *b);
extern void NuMtxInvVU0(struct numtx_s *dst, struct numtx_s *src);
extern void NuMtxScaleVU0(struct numtx_s *m, struct nuvec_s *scale);
extern void NuVec4MtxTransformHVU0(struct nuvec4_s *dst, struct nuvec4_s *src, struct numtx_s *m);
extern struct numtx_s numtx_identity;


void NuCameraSetProjectionMtx(struct numtx_s *m, float fov, float aspect, float nearclip, float farclip) {
    float depth = farclip - nearclip;
    int angle;
    float *sine;
    float cot;
    float q;

    if (!(fabs(depth) > 0.01f)) {
        NuErrorProlog("..\\nu2crash.ps2\\nu3d\\nucamera.c", 0xA0)("assert");
    }
    angle = (int)(fov * 0.5f * 10430.378f);
    sine = &NuTrigTable[angle & 0xFFFF];
    if (!(fabs(*sine) > 0.01f)) {
        NuErrorProlog("..\\nu2crash.ps2\\nu3d\\nucamera.c", 0xA1)("assert");
    }
    q = farclip / depth;
    cot = NuTrigTable[(angle + 0x4000) & 0xFFFF] / *sine;
    memset(m, 0, sizeof(struct numtx_s));
    m->_00 = aspect * cot;
    m->_11 = cot;
    m->_22 = q;
    m->_23 = 1.0f;
    m->_32 = -q * nearclip;
}


inline void NuCameraSetVPortClipMtx(struct numtx_s *dst, struct numtx_s *view, struct nucamera_s *cam, int keep) {
    struct nuviewport_s *vp;
    float farclip;
    float nearclip;
    float cot;
    float xscale;
    float yscale;
    int angle;

    if (!keep) {
        vp = NuVpGetCurrentViewport();
        angle = (int)(cam->fov * 0.5f * 10430.378f);
        farclip = cam->farclip - nucamera_farclip_hack;
        nearclip = cam->nearclip;
        cot = NuTrigTable[(angle + 0x4000) & 0xFFFF] / NuTrigTable[angle & 0xFFFF];
        xscale = vp->clip_scale_x * (cam->aspect * cot);
        yscale = vp->clip_scale_y * cot;
        memset(&pc_vport_mtx, 0, sizeof(struct numtx_s));
        pc_vport_mtx._00 = xscale;
        pc_vport_mtx._11 = yscale;
        pc_vport_mtx._22 = (farclip + nearclip) / (farclip - nearclip);
        pc_vport_mtx._23 = 1.0f;
        pc_vport_mtx._32 = farclip * -2.0f * nearclip / (farclip - nearclip);
    }
    NuMtxMulVU0(dst, view, &pc_vport_mtx);
}


inline void NuCameraSetScissorClipMtx(struct numtx_s *dst, struct numtx_s *view, struct nucamera_s *cam, int keep) {
    struct nuviewport_s *vp;
    float farclip;
    float nearclip;
    float cot;
    float xscale;
    float yscale;
    int angle;
    static struct numtx_s cmtx;

    if (!keep) {
        vp = NuVpGetCurrentViewport();
        angle = (int)(cam->fov * 0.5f * 10430.378f);
        farclip = cam->farclip - nucamera_farclip_hack;
        nearclip = cam->nearclip;
        cot = NuTrigTable[(angle + 0x4000) & 0xFFFF] / NuTrigTable[angle & 0xFFFF];
        xscale = vp->guard_scale_x * cam->aspect * cot;
        yscale = vp->guard_scale_y * cot;
        memset(&cmtx, 0, sizeof(struct numtx_s));
        cmtx._00 = xscale;
        cmtx._11 = yscale;
        cmtx._22 = (farclip + nearclip) / (farclip - nearclip);
        cmtx._23 = 1.0f;
        cmtx._32 = farclip * -2.0f * nearclip / (farclip - nearclip);
    }
    NuMtxMulVU0(dst, view, &cmtx);
}


void NuCameraSetEx(struct nucamera_s *cam, int keep) {
    struct numtx_s caminv;
    struct numtx_s reflectinv;
    struct numtx_s flip;
    float tanfov;
    float xratio;
    int angle;

    global_camera = *cam;
    FaceYDirStream(NuAtan2D(-global_camera.mtx._20, -global_camera.mtx._22));
    if (!keep) {
        NuVpUpdate();
    }
    if (!camfx) {
        NuMtxInvVU0(&vmtx, &global_camera.mtx);
        NuMtxScaleVU0(&vmtx, &global_camera.scale);
    } else {
        flip = numtx_identity;
        flip._11 = -1.0f;
        NuMtxInv(&caminv, &global_camera.mtx);
        NuMtxInv(&reflectinv, &global_reflect.mtx);
        vmtx = reflectinv;
        if (camfx == 1) {
            NuMtxMul(&vmtx, &vmtx, &flip);
        }
        NuMtxMul(&vmtx, &vmtx, &global_reflect.mtx);
        NuMtxMul(&vmtx, &vmtx, &caminv);
    }
    if (!keep) {
        NuCameraSetProjectionMtx(&pmtx, global_camera.fov, global_camera.aspect,
                                 global_camera.nearclip, global_camera.farclip);
    }
    NuCameraSetVPortClipMtx(&vpc_vport_mtx, &vmtx, &global_camera, keep);
    NuCameraSetScissorClipMtx(&vpc_sci_mtx, &vmtx, &global_camera, keep);
    NuVpGetScalingMtx(&smtx);
    NuMtxMulVU0(&vpmtx, &vmtx, &pmtx);
    NuMtxMulVU0(&vpsmtx, &vpmtx, &smtx);
    NuMtxMulVU0(&psmtx, &pmtx, &smtx);
    global_reflect.clip = vpc_vport_mtx;
    if (!keep) {
        angle = (int)(global_camera.fov * 0.5f * 10430.378f);
        tanfov = NuTrigTable[angle & 0xFFFF] / NuTrigTable[(angle + 0x4000) & 0xFFFF];
        xratio = tanfov / global_camera.aspect;
        zx = xratio;
        zy = tanfov;
        zxs = xratio * 5120.0f;
        zys = tanfov * 1792.0f;
    }
    NuRndrSetViewMtx(&vpsmtx, &vpc_vport_mtx, &vpc_sci_mtx);
    NuRndrSetSpecularLight(0, 0);
}


int NuCameraSaveState(void) {
    int index = -1;
    NuCameraState *state;

    if (cam_state_count < 16) {
        index = cam_state_count++;
        state = &cam_state[index];
        state->camera = global_camera;
        state->view = vmtx;
        state->projection = pmtx;
        state->scaling = smtx;
        state->vport_clip = vpmtx;
        state->scissor_clip = vpc_sci_mtx;
        state->vpc = vpc_vport_mtx;
        state->pc = pc_vport_mtx;
        state->vpcs = vpsmtx;
        state->pcs = psmtx;
        state->reflect = camfx;
        state->clip_x = zx;
        state->clip_y = zy;
        state->clip_z = zxs;
        state->clip_w = zys;
    }
    return index + 1;
}


void NuCameraRestoreState(int handle) {
    int index = handle - 1;
    NuCameraState *state;

    if (index < cam_state_count) {
        state = &cam_state[index];
        global_camera = state->camera;
        vmtx = state->view;
        pmtx = state->projection;
        smtx = state->scaling;
        vpmtx = state->vport_clip;
        vpc_sci_mtx = state->scissor_clip;
        vpc_vport_mtx = state->vpc;
        pc_vport_mtx = state->pc;
        vpsmtx = state->vpcs;
        psmtx = state->pcs;
        camfx = state->reflect;
        zx = state->clip_x;
        zy = state->clip_y;
        zxs = state->clip_z;
        zys = state->clip_w;
        FaceYDirStream(NuAtan2D(-global_camera.mtx._20,
                              -global_camera.mtx._22));
        NuRndrSetViewMtx(&vpsmtx, &vpc_vport_mtx, &vpc_sci_mtx);
    }
}


struct nucamera_s *NuCameraCreate(void) {
    struct nucamera_s *cam = NuMemAllocFn(sizeof(struct nucamera_s), "..\\nu2crash.ps2\\nu3d\\nucamera.c", 0x3A);

    NuMtxSetIdentity(&cam->mtx);
    cam->nearclip = 0.15f;
    cam->farclip = 10000.0f;
    cam->fov = 0.75f;
    cam->aspect = 0.75f;
    cam->portalnearclip = 0.0f;
    cam->scale.z = 1.0f;
    cam->scale.y = 1.0f;
    cam->scale.x = 1.0f;
    return cam;
}


void NuCameraDestroy(struct nucamera_s *cam) {
    if (cam) {
        NuMemFreeFn(cam, "..\\nu2crash.ps2\\nu3d\\nucamera.c", 0x4A);
    }
}


inline void NuCameraSet(struct nucamera_s *cam) {
    while (cam == (struct nucamera_s *)1) {
    }
    NuCameraSetEx(cam, 0);
}


void NuCameraClearStateBuffer(void) {
    cam_state_count = 0;
}


void NuCameraSetReflect(struct nucamera_s *cam, struct NuCameraReflect *reflect) {
    global_reflect = *reflect;
    NuCameraSet(cam);
    *reflect = global_reflect;
}


void NuCameraSetAxes(struct nuvec_s *axes) {
    cam_axes = *axes;
}


void NuCameraGetAxes(struct nuvec_s *axes) {
    *axes = cam_axes;
}


struct nucamera_s *NuCameraGetCam(void) {
    return &global_camera;
}


struct numtx_s *NuCameraGetMtx(void) {
    return &global_camera.mtx;
}


struct numtx_s *NuCameraGetViewMtx(void) {
    return &vmtx;
}


struct numtx_s *NuCameraGetProjectionMtx(void) {
    return &pmtx;
}


struct numtx_s *NuCameraGetScalingMtx(void) {
    return &smtx;
}


struct numtx_s *NuCameraGetVPCSMtx(void) {
    return &vpsmtx;
}


struct numtx_s *NuCameraGetPCMtx(void) {
    return &pc_vport_mtx;
}


struct numtx_s *NuCameraGetPCSMtx(void) {
    return &psmtx;
}


struct numtx_s *NuCameraGetVPCMtx(void) {
    return &vpc_vport_mtx;
}


void NuCameraGetClipMtx(struct numtx_s *vpc, struct numtx_s *scissor) {
    if (vpc) {
        *vpc = vpc_vport_mtx;
    }
    if (scissor) {
        *scissor = vpc_sci_mtx;
    }
}


/* Transform `n` points into view space. `w`, if given, is the world matrix to apply first. */
void NuCameraTransformView(struct nuvec_s *dst, struct nuvec_s *src, int n, struct numtx_s *w) {
    struct nuvec_s *end;
    struct numtx_s m;

    end = &src[n];
    if (w != 0) {
        NuMtxMul(&m, w, &vmtx);
    } else {
        m = vmtx;
    }
    for (; src < end; src++, dst++) {
        NuVecMtxTransform(dst, src, &m);
    }
}

/* Transform `n` points into clip space. `w`, if given, is the world matrix to apply first. */
void NuCameraTransformPerspective(struct nuvec_s *dst, struct nuvec_s *src, int n, struct numtx_s *w) {
    struct nuvec_s *end;
    struct numtx_s m;

    end = &src[n];
    if (w != 0) {
        NuMtxMulH(&m, w, &vpmtx);
    } else {
        m = vpmtx;
    }
    for (; src < end; src++, dst++) {
        NuVecMtxTransformH(dst, src, &m);
    }
}

/* Transform `n` points into viewport clip space. `w`, if given, is the world matrix to apply first. */
void NuCameraTransformScreenClip(struct nuvec_s *dst, struct nuvec_s *src, int n, struct numtx_s *w) {
    struct nuvec_s *end;
    struct numtx_s m;

    end = &src[n];
    if (w != 0) {
        NuMtxMulH(&m, w, &vpc_vport_mtx);
    } else {
        m = vpc_vport_mtx;
    }
    for (; src < end; src++, dst++) {
        NuVecMtxTransformH(dst, src, &m);
    }
}

/* Transform `n` points onto the screen. `w`, if given, is the world matrix to apply first. */
void NuCameraTransformScreen(struct nuvec_s *dst, struct nuvec_s *src, int n, struct numtx_s *w) {
    struct nuvec_s *end;
    struct numtx_s m;

    end = &src[n];
    if (w != 0) {
        NuMtxMulH(&m, w, &vpsmtx);
    } else {
        m = vpsmtx;
    }
    for (; src < end; src++, dst++) {
        NuVecMtxTransformH(dst, src, &m);
    }
}

/* Transform `n` points onto the screen on VU0. `w`, if given, is the world matrix to apply first. */
void NuCameraTransformScreenVU0(struct nuvec4_s *dst, struct nuvec4_s *src, int n, struct numtx_s *w) {
    struct nuvec4_s *end;
    struct numtx_s m;

    end = &src[n];
    if (w != 0) {
        NuMtxMulVU0(&m, w, &vpsmtx);
    } else {
        m = vpsmtx;
    }
    for (; src < end; src++, dst++) {
        NuVec4MtxTransformHVU0(dst, src, &m);
    }
}

int NuCameraClipTestExtents(struct nuvec_s *min, struct nuvec_s *max, struct numtx_s *wm) {
    int result;

    NuCameraInitClipTestVU0(&vpc_vport_mtx, &vpc_sci_mtx);
    result = NuCameraClipTestExtentsVU0(min, max);
    if (!(result & 0x3F)) {
        return (result & 0x3F00) ? 2 : 1;
    }
    return 0;
}


/* Clip-test `cnt` points (after the world matrix `wm`, if given). Returns the
 * clip flags every point shares: 0 unless all of them are off one side. */
int NuCameraClipTestPoints(struct nuvec_s *pnts, int cnt, struct numtx_s *wm) {
    struct numtx_s m;
    struct nuvec_s v;
    int c;
    int out;
    int i;

    if (wm != 0) {
        NuMtxMulH(&m, wm, &vmtx);
    } else {
        m = vmtx;
    }
    out = -1;
    for (i = 0; i < cnt; i++) {
        NuVecMtxTransform(&v, pnts, &m);
        c = 0;
        if (v.z < global_camera.nearclip) {
            c = 0x20;
        }
        if (v.z > global_camera.farclip) {
            c |= 0x10;
        }
        if (v.x < -v.z * zx) {
            c |= 0x1;
        }
        if (v.x > v.z * zx) {
            c |= 0x2;
        }
        if (v.y < -v.z * zy) {
            c |= 0x8;
        }
        if (v.y > v.z * zy) {
            c |= 0x4;
        }
        out &= c;
        pnts++;
    }
    return out;
}

float NuCameraDistSqr(struct nuvec_s *pos) {
    return NuVecDistSqr(pos, (struct nuvec_s *)&global_camera.mtx._30, 0);
}


float NuCameraDist(struct nuvec_s *pos) {
    return NuVecDist(pos, (struct nuvec_s *)&global_camera.mtx._30, 0);
}


void NuCameraGetTrans(struct nuvec_s *trans) {
    *trans = *(struct nuvec_s *)&global_camera.mtx._30;
}


void NuCameraGetClippingRatios(float *xratio, float *yratio) {
    *xratio = zx;
    *yratio = zy;
}


int NuCameraClipTestPointVport(struct nuvec_s *pos) {
    return !NuVecClipTestPointVU0(pos, &vpc_vport_mtx);
}


int NuCameraClipTestPointScissor(struct nuvec_s *pos) {
    return !NuVecClipTestPointVU0(pos, &vpc_sci_mtx);
}


void NuCameraGet(struct nucamera_s *cam) {
    *cam = global_camera;
}

/* Transform `n` points into scissor clip space. `w`, if given, is the world matrix to apply first. */
void NuCameraTransformScissorClip(struct nuvec_s *dst, struct nuvec_s *src, int n, struct numtx_s *w) {
    struct nuvec_s *end;
    struct numtx_s m;

    end = &src[n];
    if (w != 0) {
        NuMtxMulH(&m, w, &vpc_sci_mtx);
    } else {
        m = vpc_sci_mtx;
    }
    for (; src < end; src++, dst++) {
        NuVecMtxTransformH(dst, src, &m);
    }
}
