#ifndef NU3D_NULIGHT_H
#define NU3D_NULIGHT_H

#include "numath/numathtypes.h"

struct nulight_s {
    int type;                           /* 0x00 */
    int on;                             /* 0x04 */
    struct nucolour3_s ambient;         /* 0x08 */
    struct nucolour3_s diffuse;         /* 0x14 */
    char pad20[0x20];                   /* 0x20 */
    struct numtx_s mtx;                 /* 0x40 */
    char pad80[0x08];                   /* 0x80 */
    int index;                          /* 0x88 */
    int used;                           /* 0x8C */
}; /* 0x90 */

void NuLightInit(void);
void NuLightClose(void);
struct nulight_s *NuLightCreate(void);
void NuLightUpdate(struct nulight_s *light);
void NuLightDestroy(struct nulight_s *light);
struct nulight_s *NuLightGetLight(int index);
void NuLigthSetPolyHazeMat(void *mtl, float *u, float *v);
void NuLightAddSpot(struct nuvec_s *pos, float size, int colour, int mode);
void NuLightAddSpotXSpanFade(struct nuvec_s *pos, float size, float brightness, int colour, int mode, int xspan, int fade);
void NuLightFog(float near, float far, int colour, int blur, int haze_colour);
void NuLightFogClear(int clear);
void NuLgtArcLaser(int type, struct nuvec_s *start, struct nuvec_s *end, struct nuvec_s *mid, float sizew, float sizel, float sizewob, float wobspeed, int colour);
void NuLgtSetArcMat(void *material, float u0, float v0, float u1, float v1);
void NuLightAddHazePoly(struct nuvec_s *pos, float size, float rot);
void *NuLightSprDmaOpen(int count);
void NuLightSprDmaClose(void);
void NuLightSprMake(int x, int y, int width, int height, int colour, int z, float u0, float v0, float u1, float v1);
int NuLgtRand(void);

#endif /* NU3D_NULIGHT_H */
