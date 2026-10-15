#ifndef NU3D_NUVPORT_H
#define NU3D_NUVPORT_H

#include "numath/numathtypes.h"

struct nuviewport_s {
    int x;                              /* 0x00 */
    int y;                              /* 0x04 */
    int width;                          /* 0x08 */
    int height;                         /* 0x0C */
    float zmin;                         /* 0x10 */
    float zmax;                         /* 0x14 */
    float centre_x;                     /* 0x18 */
    float centre_y;                     /* 0x1C */
    float clip_left;                    /* 0x20 */
    float clip_top;                     /* 0x24 */
    float clip_right;                   /* 0x28 */
    float clip_bottom;                  /* 0x2C */
    float clip_scale_x;                 /* 0x30 */
    float clip_scale_y;                 /* 0x34 */
    float guard_scale_x;                /* 0x38 */
    float guard_scale_y;                /* 0x3C */
}; /* 0x40 */

void NuVpUpdate(void);
void NuVpSetScalingMtx(void);
void NuVpInit(void);
void NuVpRestore(void);
void NuVpSetPosition(float x, float y);
void NuVpSetSize(float width, float height);
void NuVpSetZRange(float zmin, float zmax);
void NuVpSetCentre(float x, float y);
void NuVpSetClipping(float left, float top, float right, float bottom);
void NuVpGetScalingMtx(struct numtx_s *matrix);
float NuVpVirtualWidth(float width);
float NuVpVirtualHeight(float height);
float NuVpPixelWidth(float width);
float NuVpPixelHeight(float height);
struct nuviewport_s *NuVpGetCurrentViewport(void);

#endif /* NU3D_NUVPORT_H */
