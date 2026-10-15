#ifndef NU3D_NUCAMERA_H
#define NU3D_NUCAMERA_H

#include "numath/numathtypes.h"

struct nucamera_s {
    struct numtx_s mtx;                 /* 0x00 */
    float fov;                          /* 0x40 */
    float aspect;                       /* 0x44 */
    float nearclip;                     /* 0x48 */
    float farclip;                      /* 0x4C */
    float portalnearclip;               /* 0x50 */
    struct nuvec_s scale;               /* 0x54 */
}; /* 0x60 */

struct NuCameraReflect;

void NuCameraSetProjectionMtx(struct numtx_s *m, float fov, float aspect, float nearclip, float farclip);
void NuCameraSetEx(struct nucamera_s *cam, int keep);
int NuCameraSaveState(void);
void NuCameraRestoreState(int handle);
struct nucamera_s *NuCameraCreate(void);
void NuCameraDestroy(struct nucamera_s *cam);
void NuCameraClearStateBuffer(void);
void NuCameraSetReflect(struct nucamera_s *cam, struct NuCameraReflect *reflect);
void NuCameraSetAxes(struct nuvec_s *axes);
void NuCameraGetAxes(struct nuvec_s *axes);
struct nucamera_s *NuCameraGetCam(void);
struct numtx_s *NuCameraGetMtx(void);
struct numtx_s *NuCameraGetViewMtx(void);
struct numtx_s *NuCameraGetProjectionMtx(void);
struct numtx_s *NuCameraGetScalingMtx(void);
struct numtx_s *NuCameraGetVPCSMtx(void);
struct numtx_s *NuCameraGetPCMtx(void);
struct numtx_s *NuCameraGetPCSMtx(void);
struct numtx_s *NuCameraGetVPCMtx(void);
void NuCameraGetClipMtx(struct numtx_s *vpc, struct numtx_s *scissor);
void NuCameraTransformView(struct nuvec_s *dst, struct nuvec_s *src, int n, struct numtx_s *w);
void NuCameraTransformPerspective(struct nuvec_s *dst, struct nuvec_s *src, int n, struct numtx_s *w);
void NuCameraTransformScreenClip(struct nuvec_s *dst, struct nuvec_s *src, int n, struct numtx_s *w);
void NuCameraTransformScreen(struct nuvec_s *dst, struct nuvec_s *src, int n, struct numtx_s *w);
void NuCameraTransformScreenVU0(struct nuvec4_s *dst, struct nuvec4_s *src, int n, struct numtx_s *w);
int NuCameraClipTestExtents(struct nuvec_s *min, struct nuvec_s *max, struct numtx_s *wm);
int NuCameraClipTestPoints(struct nuvec_s *pnts, int cnt, struct numtx_s *wm);
float NuCameraDistSqr(struct nuvec_s *pos);
float NuCameraDist(struct nuvec_s *pos);
void NuCameraGetTrans(struct nuvec_s *trans);
void NuCameraGetClippingRatios(float *xratio, float *yratio);
int NuCameraClipTestPointVport(struct nuvec_s *pos);
int NuCameraClipTestPointScissor(struct nuvec_s *pos);
void NuCameraGet(struct nucamera_s *cam);
void NuCameraTransformScissorClip(struct nuvec_s *dst, struct nuvec_s *src, int n, struct numtx_s *w);

#endif /* NU3D_NUCAMERA_H */
