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
} NuCamera;


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

extern float D_00633048;
#define clip_ratio_x D_00633048

extern float D_0063304C;
#define clip_ratio_y D_0063304C

extern char D_00614170[];
#define nucamera_file D_00614170

extern void NuMemFreeFn(void *ptr, char *file, int line);
extern float NuVecDistSqr(NuVec3 *a, NuVec3 *b, NuVec3 *diff);
extern int NuVecClipTestPointVU0(NuVec3 *pos, NuMtx *clipmtx);


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


void NuCameraGetClippingRatios(float *xratio, float *yratio) {
    *xratio = clip_ratio_x;
    *yratio = clip_ratio_y;
}


int NuCameraClipTestPointVport(NuVec3 *pos) {
    return !NuVecClipTestPointVU0(pos, &vpc_mtx);
}
