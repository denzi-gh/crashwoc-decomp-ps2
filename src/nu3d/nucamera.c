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

extern NuMtx D_0067A700;
#define view_mtx D_0067A700

extern int D_0062EB3C;
#define state_buffer_count D_0062EB3C

extern float D_00633048;
#define clip_ratio_x D_00633048

extern float D_0063304C;
#define clip_ratio_y D_0063304C


void NuCameraClearStateBuffer(void) {
    state_buffer_count = 0;
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


void NuCameraGetClippingRatios(float *xratio, float *yratio) {
    *xratio = clip_ratio_x;
    *yratio = clip_ratio_y;
}
