#ifndef NU3D_NURNDR_H
#define NU3D_NURNDR_H

#include "numath/numathtypes.h"

struct nuvtx_tc1_s;
struct numtl_s;

int NuRndrLine3d(struct nuvtx_tc1_s *vtx, struct numtl_s *mtl,
                 struct numtx_s *wm);

struct faceon_s {
    struct nuvec_s pos;                 /* 0x00 */
    float w;                            /* 0x0C */
    float tc[4];                        /* 0x10 */
    float width;                        /* 0x20 */
    float height;                       /* 0x24 */
    float sinang;                       /* 0x28 */
    float cosang;                       /* 0x2C */
    int colour[4];                      /* 0x30 */
}; /* 0x40 */

struct nuvtx2d_s {
    float x;                            /* 0x00 */
    float y;                            /* 0x04 */
    float z;                            /* 0x08 */
    float rhw;                          /* 0x0C */
    union {
        int colour;
        unsigned char rgba[4];
    };                                  /* 0x10 */
    float u;                            /* 0x14 */
    float v;                            /* 0x18 */
}; /* 0x1C */

struct nurndrscreenvtx_s {
    short x;                            /* 0x00 */
    short y;                            /* 0x02 */
    short u;                            /* 0x04 */
    short v;                            /* 0x06 */
}; /* 0x08 */

/* Eight entries per channel; names of the sprite parameters remain unknown. */
struct FxSprite2 {
    float parameter_00;                 /* 0x00 */
    int parameter_04;                   /* 0x04 */
    int count;                          /* 0x08 */
    float parameter_0c;                 /* 0x0C */
    float parameter_10;                 /* 0x10 */
    float parameter_14;                 /* 0x14 */
    float parameter_18;                 /* 0x18 */
    int parameter_1c[8];                /* 0x1C */
    int colour[8];                      /* 0x3C */
    float channel[24][8];               /* 0x5C */
}; /* 0x35C */

extern struct nurndrscreenvtx_s watertab[33][21];
extern struct nurndrscreenvtx_s Hazetab[33][21];

#endif
