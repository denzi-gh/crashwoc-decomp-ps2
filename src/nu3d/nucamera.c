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

typedef struct NuVec3 {
    float x;
    float y;
    float z;
} NuVec3;

typedef struct NuMtx {
    float m[4][4];
} NuMtx;

typedef struct NuCamera {
    NuMtx mtx;
    float fov;
    float aspect;
    float nearclip;
    float farclip;
    unsigned char unknown_50[0x10];
} NuCamera;

/* The state-buffer stride is 0x2b4; the camera occupies its first 0x60 bytes. */
typedef struct NuCameraState {
    NuCamera camera;
    NuMtx view;
    NuMtx projection;
    NuMtx scaling;
    NuMtx vport_clip;
    NuMtx scissor_clip;
    NuMtx vpc;
    NuMtx pc;
    NuMtx vpcs;
    NuMtx pcs;
    int reflect;
    float clip_x;
    float clip_y;
    float clip_z;
    float clip_w;
} NuCameraState;


extern NuCamera global_camera;

extern NuVec3 D_002D3EB0;
#define camera_axes D_002D3EB0

extern NuVec3 D_002D3EF0;
#define camera_trans D_002D3EF0

extern NuMtx D_0067A700;
#define view_mtx D_0067A700

extern NuMtx D_0067A740;
#define projection_mtx D_0067A740

extern NuMtx D_0067A780;
#define scaling_mtx D_0067A780

extern NuMtx D_0067A7C0;
#define vport_clip_mtx D_0067A7C0

extern NuMtx D_0067A800;
#define scissor_clip_mtx D_0067A800

extern NuMtx D_0067A840;
#define vpc_mtx D_0067A840

extern NuMtx D_0067A880;
#define pc_mtx D_0067A880

extern NuMtx D_0067A8C0;
#define vpcs_mtx D_0067A8C0

extern NuMtx D_0067A900;
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

extern char D_00614170[];
#define nucamera_file D_00614170

extern void NuMemFreeFn(void *ptr, char *file, int line);
extern float NuVecDistSqr(NuVec3 *a, NuVec3 *b, NuVec3 *diff);
extern int NuVecClipTestPointVU0(NuVec3 *pos, NuMtx *clipmtx);
extern int NuAtan2D(float x, float z);
extern void FaceYDirStream(int angle);
extern void NuRndrSetViewMtx(NuMtx *vpcs, NuMtx *vpc, NuMtx *scissor);


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
        FaceYDirStream(NuAtan2D(-global_camera.mtx.m[2][0],
                              -global_camera.mtx.m[2][2]));
        NuRndrSetViewMtx(&vpcs_mtx, &vpc_mtx, &scissor_clip_mtx);
    }
}


void NuCameraDestroy(NuCamera *cam) {
    if (cam) {
        NuMemFreeFn(cam, nucamera_file, 0x4A);
    }
}


void NuCameraClearStateBuffer(void) {
    state_buffer_count = 0;
}


void NuCameraSetAxes(NuVec3 *axes) {
    camera_axes = *axes;
}


void NuCameraGetAxes(NuVec3 *axes) {
    *axes = camera_axes;
}


NuCamera *NuCameraGetCam(void) {
    return &global_camera;
}


NuMtx *NuCameraGetMtx(void) {
    return &global_camera.mtx;
}


NuMtx *NuCameraGetViewMtx(void) {
    return &view_mtx;
}


NuMtx *NuCameraGetProjectionMtx(void) {
    return &projection_mtx;
}


NuMtx *NuCameraGetScalingMtx(void) {
    return &scaling_mtx;
}


NuMtx *NuCameraGetVPCSMtx(void) {
    return &vpcs_mtx;
}


NuMtx *NuCameraGetPCMtx(void) {
    return &pc_mtx;
}


NuMtx *NuCameraGetPCSMtx(void) {
    return &pcs_mtx;
}


NuMtx *NuCameraGetVPCMtx(void) {
    return &vpc_mtx;
}


float NuCameraDistSqr(NuVec3 *pos) {
    return NuVecDistSqr(pos, &camera_trans, 0);
}


void NuCameraGetTrans(NuVec3 *trans) {
    *trans = camera_trans;
}


void NuCameraGetClippingRatios(float *xratio, float *yratio) {
    *xratio = clip_ratio_x;
    *yratio = clip_ratio_y;
}


int NuCameraClipTestPointVport(NuVec3 *pos) {
    return !NuVecClipTestPointVU0(pos, &vpc_mtx);
}
