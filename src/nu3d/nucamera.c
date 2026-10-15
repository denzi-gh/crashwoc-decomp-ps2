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


extern struct nucamera_s global_camera;

extern struct nuvec_s D_002D3EB0;
#define camera_axes D_002D3EB0

extern struct nuvec_s D_002D3EF0;
#define camera_trans D_002D3EF0

extern struct numtx_s D_0067A700;
#define view_mtx D_0067A700

extern struct numtx_s D_0067A740;
#define projection_mtx D_0067A740

extern struct numtx_s D_0067A780;
#define scaling_mtx D_0067A780

extern struct numtx_s D_0067A7C0;
#define vport_clip_mtx D_0067A7C0

extern struct numtx_s D_0067A800;
#define scissor_clip_mtx D_0067A800

extern struct numtx_s D_0067A840;
#define vpc_mtx D_0067A840

extern struct numtx_s D_0067A880;
#define pc_mtx D_0067A880

extern struct numtx_s D_0067A8C0;
#define vpcs_mtx D_0067A8C0

extern struct numtx_s D_0067A900;
#define pcs_mtx D_0067A900

extern int D_0062EB3C;
#define state_buffer_count D_0062EB3C

extern NuCameraState D_0067A940[16];
#define state_buffer D_0067A940

extern int D_0062EB38;
#define camera_reflect D_0062EB38

extern float D_00633048;
#define clip_ratio_x D_00633048

extern float D_0063304C;
#define clip_ratio_y D_0063304C

extern float D_00633050;
#define clip_ratio_z D_00633050

extern float D_00633054;
#define clip_ratio_w D_00633054

extern struct numtx_s D_0067A630;
#define scissor_proj_mtx D_0067A630

extern NuCameraReflect D_0067A670;
#define camera_reflection D_0067A670

extern char D_00614170[];
#define nucamera_file D_00614170

extern char D_0062EB58[];
#define assert_msg D_0062EB58

extern float nucamera_farclip_hack;
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
        NuErrorProlog(nucamera_file, 0xA0)(assert_msg);
    }
    angle = (int)(fov * 0.5f * 10430.378f);
    sine = &NuTrigTable[angle & 0xFFFF];
    if (!(fabs(*sine) > 0.01f)) {
        NuErrorProlog(nucamera_file, 0xA1)(assert_msg);
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
        memset(&pc_mtx, 0, sizeof(struct numtx_s));
        pc_mtx._00 = xscale;
        pc_mtx._11 = yscale;
        pc_mtx._22 = (farclip + nearclip) / (farclip - nearclip);
        pc_mtx._23 = 1.0f;
        pc_mtx._32 = farclip * -2.0f * nearclip / (farclip - nearclip);
    }
    NuMtxMulVU0(dst, view, &pc_mtx);
}


inline void NuCameraSetScissorClipMtx(struct numtx_s *dst, struct numtx_s *view, struct nucamera_s *cam, int keep) {
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
        xscale = vp->guard_scale_x * cam->aspect * cot;
        yscale = vp->guard_scale_y * cot;
        memset(&scissor_proj_mtx, 0, sizeof(struct numtx_s));
        scissor_proj_mtx._00 = xscale;
        scissor_proj_mtx._11 = yscale;
        scissor_proj_mtx._22 = (farclip + nearclip) / (farclip - nearclip);
        scissor_proj_mtx._23 = 1.0f;
        scissor_proj_mtx._32 = farclip * -2.0f * nearclip / (farclip - nearclip);
    }
    NuMtxMulVU0(dst, view, &scissor_proj_mtx);
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
    if (!camera_reflect) {
        NuMtxInvVU0(&view_mtx, &global_camera.mtx);
        NuMtxScaleVU0(&view_mtx, &global_camera.scale);
    } else {
        flip = numtx_identity;
        flip._11 = -1.0f;
        NuMtxInv(&caminv, &global_camera.mtx);
        NuMtxInv(&reflectinv, &camera_reflection.mtx);
        view_mtx = reflectinv;
        if (camera_reflect == 1) {
            NuMtxMul(&view_mtx, &view_mtx, &flip);
        }
        NuMtxMul(&view_mtx, &view_mtx, &camera_reflection.mtx);
        NuMtxMul(&view_mtx, &view_mtx, &caminv);
    }
    if (!keep) {
        NuCameraSetProjectionMtx(&projection_mtx, global_camera.fov, global_camera.aspect,
                                 global_camera.nearclip, global_camera.farclip);
    }
    NuCameraSetVPortClipMtx(&vpc_mtx, &view_mtx, &global_camera, keep);
    NuCameraSetScissorClipMtx(&scissor_clip_mtx, &view_mtx, &global_camera, keep);
    NuVpGetScalingMtx(&scaling_mtx);
    NuMtxMulVU0(&vport_clip_mtx, &view_mtx, &projection_mtx);
    NuMtxMulVU0(&vpcs_mtx, &vport_clip_mtx, &scaling_mtx);
    NuMtxMulVU0(&pcs_mtx, &projection_mtx, &scaling_mtx);
    camera_reflection.clip = vpc_mtx;
    if (!keep) {
        angle = (int)(global_camera.fov * 0.5f * 10430.378f);
        tanfov = NuTrigTable[angle & 0xFFFF] / NuTrigTable[(angle + 0x4000) & 0xFFFF];
        xratio = tanfov / global_camera.aspect;
        clip_ratio_x = xratio;
        clip_ratio_y = tanfov;
        clip_ratio_z = xratio * 5120.0f;
        clip_ratio_w = tanfov * 1792.0f;
    }
    NuRndrSetViewMtx(&vpcs_mtx, &vpc_mtx, &scissor_clip_mtx);
    NuRndrSetSpecularLight(0, 0);
}


int NuCameraSaveState(void) {
    int index = -1;
    NuCameraState *state;

    if (state_buffer_count < 16) {
        index = state_buffer_count++;
        state = &state_buffer[index];
        state->camera = global_camera;
        state->view = view_mtx;
        state->projection = projection_mtx;
        state->scaling = scaling_mtx;
        state->vport_clip = vport_clip_mtx;
        state->scissor_clip = scissor_clip_mtx;
        state->vpc = vpc_mtx;
        state->pc = pc_mtx;
        state->vpcs = vpcs_mtx;
        state->pcs = pcs_mtx;
        state->reflect = camera_reflect;
        state->clip_x = clip_ratio_x;
        state->clip_y = clip_ratio_y;
        state->clip_z = clip_ratio_z;
        state->clip_w = clip_ratio_w;
    }
    return index + 1;
}


void NuCameraRestoreState(int handle) {
    int index = handle - 1;
    NuCameraState *state;

    if (index < state_buffer_count) {
        state = &state_buffer[index];
        global_camera = state->camera;
        view_mtx = state->view;
        projection_mtx = state->projection;
        scaling_mtx = state->scaling;
        vport_clip_mtx = state->vport_clip;
        scissor_clip_mtx = state->scissor_clip;
        vpc_mtx = state->vpc;
        pc_mtx = state->pc;
        vpcs_mtx = state->vpcs;
        pcs_mtx = state->pcs;
        camera_reflect = state->reflect;
        clip_ratio_x = state->clip_x;
        clip_ratio_y = state->clip_y;
        clip_ratio_z = state->clip_z;
        clip_ratio_w = state->clip_w;
        FaceYDirStream(NuAtan2D(-global_camera.mtx._20,
                              -global_camera.mtx._22));
        NuRndrSetViewMtx(&vpcs_mtx, &vpc_mtx, &scissor_clip_mtx);
    }
}


struct nucamera_s *NuCameraCreate(void) {
    struct nucamera_s *cam = NuMemAllocFn(sizeof(struct nucamera_s), nucamera_file, 0x3A);

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
        NuMemFreeFn(cam, nucamera_file, 0x4A);
    }
}


inline void NuCameraSet(struct nucamera_s *cam) {
    while (cam == (struct nucamera_s *)1) {
    }
    NuCameraSetEx(cam, 0);
}


void NuCameraClearStateBuffer(void) {
    state_buffer_count = 0;
}


void NuCameraSetReflect(struct nucamera_s *cam, struct NuCameraReflect *reflect) {
    camera_reflection = *reflect;
    NuCameraSet(cam);
    *reflect = camera_reflection;
}


void NuCameraSetAxes(struct nuvec_s *axes) {
    camera_axes = *axes;
}


void NuCameraGetAxes(struct nuvec_s *axes) {
    *axes = camera_axes;
}


struct nucamera_s *NuCameraGetCam(void) {
    return &global_camera;
}


struct numtx_s *NuCameraGetMtx(void) {
    return &global_camera.mtx;
}


struct numtx_s *NuCameraGetViewMtx(void) {
    return &view_mtx;
}


struct numtx_s *NuCameraGetProjectionMtx(void) {
    return &projection_mtx;
}


struct numtx_s *NuCameraGetScalingMtx(void) {
    return &scaling_mtx;
}


struct numtx_s *NuCameraGetVPCSMtx(void) {
    return &vpcs_mtx;
}


struct numtx_s *NuCameraGetPCMtx(void) {
    return &pc_mtx;
}


struct numtx_s *NuCameraGetPCSMtx(void) {
    return &pcs_mtx;
}


struct numtx_s *NuCameraGetVPCMtx(void) {
    return &vpc_mtx;
}


void NuCameraGetClipMtx(struct numtx_s *vpc, struct numtx_s *scissor) {
    if (vpc) {
        *vpc = vpc_mtx;
    }
    if (scissor) {
        *scissor = scissor_clip_mtx;
    }
}


/* Transform `n` points into view space. `w`, if given, is the world matrix to apply first. */
void NuCameraTransformView(struct nuvec_s *dst, struct nuvec_s *src, int n, struct numtx_s *w) {
    struct nuvec_s *end;
    struct numtx_s m;

    end = &src[n];
    if (w != 0) {
        NuMtxMul(&m, w, &view_mtx);
    } else {
        m = view_mtx;
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
        NuMtxMulH(&m, w, &vport_clip_mtx);
    } else {
        m = vport_clip_mtx;
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
        NuMtxMulH(&m, w, &vpc_mtx);
    } else {
        m = vpc_mtx;
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
        NuMtxMulH(&m, w, &vpcs_mtx);
    } else {
        m = vpcs_mtx;
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
        NuMtxMulVU0(&m, w, &vpcs_mtx);
    } else {
        m = vpcs_mtx;
    }
    for (; src < end; src++, dst++) {
        NuVec4MtxTransformHVU0(dst, src, &m);
    }
}

int NuCameraClipTestExtents(struct nuvec_s *min, struct nuvec_s *max, struct numtx_s *wm) {
    int result;

    NuCameraInitClipTestVU0(&vpc_mtx, &scissor_clip_mtx);
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
        NuMtxMulH(&m, wm, &view_mtx);
    } else {
        m = view_mtx;
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
        if (v.x < -v.z * clip_ratio_x) {
            c |= 0x1;
        }
        if (v.x > v.z * clip_ratio_x) {
            c |= 0x2;
        }
        if (v.y < -v.z * clip_ratio_y) {
            c |= 0x8;
        }
        if (v.y > v.z * clip_ratio_y) {
            c |= 0x4;
        }
        out &= c;
        pnts++;
    }
    return out;
}

float NuCameraDistSqr(struct nuvec_s *pos) {
    return NuVecDistSqr(pos, &camera_trans, 0);
}


float NuCameraDist(struct nuvec_s *pos) {
    return NuVecDist(pos, &camera_trans, 0);
}


void NuCameraGetTrans(struct nuvec_s *trans) {
    *trans = camera_trans;
}


void NuCameraGetClippingRatios(float *xratio, float *yratio) {
    *xratio = clip_ratio_x;
    *yratio = clip_ratio_y;
}


int NuCameraClipTestPointVport(struct nuvec_s *pos) {
    return !NuVecClipTestPointVU0(pos, &vpc_mtx);
}


int NuCameraClipTestPointScissor(struct nuvec_s *pos) {
    return !NuVecClipTestPointVU0(pos, &scissor_clip_mtx);
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
        NuMtxMulH(&m, w, &scissor_clip_mtx);
    } else {
        m = scissor_clip_mtx;
    }
    for (; src < end; src++, dst++) {
        NuVecMtxTransformH(dst, src, &m);
    }
}
