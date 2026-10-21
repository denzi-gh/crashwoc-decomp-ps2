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

/* Sixteen-entry sprite table for NuRndrFxSpriteSet. Field names from the
   DWARF of FxSpriteSet in another game on this engine. */
struct FxSpriteSet {
    float zval;                         /* 0x000 */
    int oiterations;                    /* 0x004 */
    int tablecount;                     /* 0x008 */
    float oxstep;                       /* 0x00C */
    float oystep;                       /* 0x010 */
    float oustep;                       /* 0x014 */
    float ovstep;                       /* 0x018 */
    int iterations[16];                 /* 0x01C */
    int rgba[16];                       /* 0x05C */
    float xstart[16];                   /* 0x09C */
    float ystart[16];                   /* 0x0DC */
    float ustart[16];                   /* 0x11C */
    float vstart[16];                   /* 0x15C */
    float xw[16];                       /* 0x19C */
    float yh[16];                       /* 0x1DC */
    float uw[16];                       /* 0x21C */
    float vh[16];                       /* 0x25C */
    float xstep[16];                    /* 0x29C */
    float ystep[16];                    /* 0x2DC */
    float ustep[16];                    /* 0x31C */
    float vstep[16];                    /* 0x35C */
}; /* 0x39C */

/* Eight-entry, two-set sprite table for NuRndrFxSprite2Set. Field names from
   the DWARF of FxSprite2Set in another game on this engine. */
struct FxSprite2Set {
    float zval;                         /* 0x000 */
    int oiterations;                    /* 0x004 */
    int tablecount;                     /* 0x008 */
    float oxstep;                       /* 0x00C */
    float oystep;                       /* 0x010 */
    float oustep;                       /* 0x014 */
    float ovstep;                       /* 0x018 */
    int iterations[8];                  /* 0x01C */
    int rgba[8];                        /* 0x03C */
    float xstart[8];                    /* 0x05C */
    float ystart[8];                    /* 0x07C */
    float ustart[8];                    /* 0x09C */
    float vstart[8];                    /* 0x0BC */
    float xw[8];                        /* 0x0DC */
    float yh[8];                        /* 0x0FC */
    float uw[8];                        /* 0x11C */
    float vh[8];                        /* 0x13C */
    float xstep[8];                     /* 0x15C */
    float ystep[8];                     /* 0x17C */
    float ustep[8];                     /* 0x19C */
    float vstep[8];                     /* 0x1BC */
    float xstart2[8];                   /* 0x1DC */
    float ystart2[8];                   /* 0x1FC */
    float ustart2[8];                   /* 0x21C */
    float vstart2[8];                   /* 0x23C */
    float xw2[8];                       /* 0x25C */
    float yh2[8];                       /* 0x27C */
    float uw2[8];                       /* 0x29C */
    float vh2[8];                       /* 0x2BC */
    float xstep2[8];                    /* 0x2DC */
    float ystep2[8];                    /* 0x2FC */
    float ustep2[8];                    /* 0x31C */
    float vstep2[8];                    /* 0x33C */
}; /* 0x35C */

extern struct nurndrscreenvtx_s watertab[33][21];
extern struct nurndrscreenvtx_s Hazetab[33][21];

#ifdef NW_HOST
/* Retail calls these without a prototype in scope (game/main.c); only the host declares them. */
struct numtl_s;
void NuRndrEndSceneEx(int flags);
void NuRndrFootPrints(struct numtl_s *mtl, float *u, float *v);
void NuRndrFx(int mode, void *fxdata);
void NuRndrInitEx(int streamsize);
void NuRndrShadPolys(struct numtl_s *mtl);
void NuRndrShadowOnOff(int on);
void NuRndrWaterRip(struct numtl_s *mtl);
void NuRndrWaterRippleUpdate(int frames);
#endif

#endif
