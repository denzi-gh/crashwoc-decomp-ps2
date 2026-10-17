/*
 * Unit: game/cloudfx
 *
 * Functions:
 *   0x0024dc30 cloudInit
 *   0x0024ddf0 cloudRender
 *   0x0024e2e0 InitClouds
 *   0x0024e400 DoClouds
 *   0x0024e848 TimeTunnelInit
 *   0x0024eb00 TimeTunnelRender
 *   0x0024f280 cloudProcess
 *   0x0024f2a8 CloseClouds
 *   0x0024f2b0 TimeTunnelClose
 *   0x0024f2e0 CloudFxInit
 */

#include "nu_libc.h"

#include "nu3d/numtl.h"
#include "nu3d/nustream.h"
#include "nu3d/nutex.h"
#include "numath/nurand.h"
#include "numath/nuvec.h"
#include "nucore/nucoretypes.h"

/* PS2 compact cloud particle (s16 x/y/z), stride 6. */
struct cloud_s {
    s16 x;
    s16 y;
    s16 z;
};

struct nuivec_s {
    s32 x;
    s32 y;
    s32 z;
};

extern s32 cloudreset;
extern struct nurndrstream_s *clouds_rs;
extern struct numtl_s *cloudmtl;
extern struct nutex_s *cloudtex;
extern struct numtl_s *ttmtl;
extern struct nutex_s *tttex;
extern u8 D_00293090[];
#define cloud_mpg D_00293090
extern u8 D_00625260[];
#define cloud_texname D_00625260
extern u8 D_00625278[];
#define cloudfx_file D_00625278
extern u8 D_00625298[];
#define tt_texname D_00625298

extern struct cloud_s clouds[];
extern f32 MAXCLOUDS;
extern f32 CLOUDRNG;
extern f32 CLOUDSIZE;
extern f32 cloud_density;
extern struct nuvec_s groff[];
extern struct nuivec_s grphase[];
extern struct nuivec_s grphaserate[];
extern struct nuvec_s grscale;
extern struct nuvec_s grvel;

extern s32 D_00632904;
#define clouds_off D_00632904
extern struct nuvec_s D_00625288;
#define cloud_colour D_00625288
extern struct nuvec_s D_002D3EF0;
#define camera_trans D_002D3EF0
extern f32 NuTrigTable[];
extern s32 PS2_REZ_H;
extern u8 *rndrstream_free;
extern union variptr_u superbuffer_ptr;

extern s32 D_0063290C;
#define ttcount D_0063290C
extern f32 D_00632908;
#define ttsize D_00632908
extern f32 D_00632910;
#define ttrange D_00632910
extern f32 D_00632914;
#define ttspan D_00632914
extern f32 D_0063291C;
#define ttcsize D_0063291C
extern s32 ttmtlcnt;
extern struct cloud_s ttptls[];        /* [8][count] trail particles, stride 6 */
extern struct nuvec_s ttoff[];
extern struct nuvec_s ttvel[];
extern s32 ttgmul[];
extern struct nuvec_s tt_colours[];
extern u16 tt_density[];
extern u16 ttdenrate[];
extern struct nuvec_s ttcoff[];
extern struct nuvec_s ttcvel[];
extern struct nuvec_s ttccol[];
extern u16 tt_cdensity[];
extern u16 ttcdenrate[];

extern void cloudInit();
extern void TextureCreate(void *desc, void *mtl, void *tex);
extern void TextureCreateEx(void *desc, void *mtl, void *tex, void *sbptr,
                            void *buf, void *buf2, void *buffend);
extern void NuMemFreeFn(void *ptr, void *src, s32 id);
extern void NuRndrStreamLink(struct nurndrstream_s *rs);
extern u8 *vpDmaTag_CntEx(u8 *cur, s32 qwc, u32 tag);
extern u8 *vpDmaTag_Close(u8 *cur);
extern u8 *vpDmaTag_RefEx(u8 *cur, u32 addr, s32 qwc, u32 tag);
extern u8 *vpDmaTag_Next(u8 *cur, s32 mode);


void cloudInit() {
    s32 i;
    f32 r;

    for (i = 0; i < MAXCLOUDS; i++) {
        r = NuRandFloat();
        clouds[i].x = (s16)(((r * CLOUDRNG + r * CLOUDRNG) - CLOUDRNG) * 16.0f);
        r = NuRandFloat();
        clouds[i].y = (s16)(((r * CLOUDRNG + r * CLOUDRNG) - CLOUDRNG) * 16.0f);
        r = NuRandFloat();
        clouds[i].z = (s16)(((r * CLOUDRNG + r * CLOUDRNG) - CLOUDRNG) * 16.0f);
    }
    TextureCreate(cloud_texname, &cloudmtl, &cloudtex);
    if (cloudmtl != 0) {
        cloudmtl->attrib.alpha = 2;
        cloudmtl->attrib.zmode = 1;
        cloudmtl->alpha_sort = 0x7D00;
        NuMtlUpdateEx(cloudmtl, 1);
        cloudmtl->stream.mpgtag = cloud_mpg;
    }
    if (cloudtex != 0) {
        if (cloudtex->bits != 0) {
            NuMemFreeFn(cloudtex->bits, cloudfx_file, 0x6C);
        }
        NuMemFreeFn(cloudtex, cloudfx_file, 0x6D);
    }
}


void cloudRender(struct nuvec_s *pos, struct nuvec_s *off, f32 size, f32 rng, s32 density,
                 s32 maxc, struct cloud_s *cl, struct numtl_s *mtl, struct nuvec_s *col) {
    union {
        u64 dw[2];
        u32 w[4];
    } gif;
    struct nuvec_s scroll;
    struct nurndrstream_s *rs;
    f32 wrap;
    f32 x;
    f32 y;
    f32 z;
    s32 n;

    rs = clouds_rs;
    NuRndrStreamLink(rs);
    NuRndrStreamSetMisc(rs);
    NuRndrStreamAddMtl(rs, mtl);
    NuRndrStreamSetViewMtx(rs);
    rs->cur.b = vpDmaTag_CntEx(rs->cur.b, 0, 0x6C068002);
    *rs->cur.f++ = pos->x;
    *rs->cur.f++ = pos->y;
    *rs->cur.f++ = pos->z;
    *rs->cur.f++ = rng;
    NuVecScale(&scroll, off, rng);
    wrap = rng + rng;
    x = pos->x + scroll.x;
    *rs->cur.f++ = x - wrap * (s32)(x / wrap);
    y = pos->y + scroll.y;
    *rs->cur.f++ = y - wrap * (s32)(y / wrap);
    z = pos->z + scroll.z;
    *rs->cur.f++ = z - wrap * (s32)(z / wrap);
    *rs->cur.i++ = 0;
    *rs->cur.f++ = size;
    *rs->cur.f++ = size * 0.5f;
    *rs->cur.f++ = density;
    *rs->cur.i++ = 0;
    *rs->cur.i++ = 0;
    *rs->cur.i++ = 0;
    *rs->cur.i++ = 0;
    *rs->cur.i++ = 0;
    *rs->cur.f++ = col->x;
    *rs->cur.f++ = col->y;
    *rs->cur.f++ = col->z;
    *rs->cur.i++ = 0;
    *rs->cur.f++ = (2048.0f + 320.0f) * 16.0f;
    *rs->cur.f++ = ((PS2_REZ_H >> 1) + 2048.0f) * 16.0f;
    *rs->cur.f++ = (2048.0f - 320.0f) * 16.0f;
    *rs->cur.f++ = (2048.0f - (PS2_REZ_H >> 1)) * 16.0f;
    *rs->cur.w++ = 0x14000300;
    *rs->cur.w++ = 0x17000000;
    rs->cur.b = vpDmaTag_Close(rs->cur.b);
    gif.dw[0] = 0x5003400000008000ULL;
    gif.dw[1] = 0x52521;
    while (maxc > 0) {
        n = (maxc > 0x20) ? 0x20 : maxc;
        maxc -= 0x20;
        rs->cur.b = vpDmaTag_CntEx(rs->cur.b, 0, 0x6D018002);
        *((u16 *)rs->cur.b)++ = n;
        *((u16 *)rs->cur.b)++ = 0;
        *((u16 *)rs->cur.b)++ = 0;
        *((u16 *)rs->cur.b)++ = 0;
        *rs->cur.w++ = 0x6C0180D2;
        *rs->cur.w++ = gif.w[0];
        *rs->cur.w++ = gif.w[1];
        *rs->cur.w++ = gif.w[2];
        *rs->cur.w++ = gif.w[3];
        *rs->cur.w++ = 0;
        *rs->cur.w++ = 0;
        *rs->cur.w++ = 0;
        rs->cur.b = vpDmaTag_Close(rs->cur.b);
        rs->cur.b = vpDmaTag_RefEx(rs->cur.b, (u32)cl, 0, (n << 16) | 0x69008003);
        cl += n;
        ((u16 *)rs->cur.b)[-8] = (n * 6) >> 4;
        rs->cur.b = vpDmaTag_CntEx(rs->cur.b, 0, 0x17000000);
    }
    rs->cur.b = vpDmaTag_CntEx(rs->cur.b, 0, 0x10000000);
    rndrstream_free = vpDmaTag_Next(rs->cur.b, 0);
}


void InitClouds(union variptr_u *buffer, union variptr_u *buffend) {
    s32 n;

    cloudInit(buffer, buffend);
    for (n = 0; n < 10; n++) {
        groff[n].x = NuRandFloat();
        groff[n].y = NuRandFloat();
        groff[n].z = NuRandFloat();
        grphase[n].x = rand() << 1;
        grphase[n].y = rand() << 1;
        grphase[n].z = rand() << 1;
        grphaserate[n].x = (rand() & 0xff) + 0x80;
        grphaserate[n].y = (rand() & 0xff) + 0x80;
        grphaserate[n].z = (rand() & 0xff) + 0x80;
    }
}


void DoClouds(s32 paused) {
    struct nuvec_s col;
    s32 i;

    col = cloud_colour;
    if (clouds_off != 0) {
        return;
    }
    if (cloudreset != 0) {
        cloudInit();
        cloudreset = 0;
    }
    for (i = 0; i < 10; i++) {
        if (paused == 0) {
            grphase[i].x += grphaserate[i].x;
            grphase[i].y += grphaserate[i].y;
            grphase[i].z += grphaserate[i].z;
            groff[i].x += NuTrigTable[grphase[i].x & 0xFFFF] * grscale.x + grvel.x;
            groff[i].y += NuTrigTable[grphase[i].y & 0xFFFF] * grscale.y + grvel.y;
            groff[i].z += NuTrigTable[grphase[i].z & 0xFFFF] * grscale.z + grvel.z;
        }
        while (1.0f < groff[i].x) {
            groff[i].x = groff[i].x - 2.0f;
        }
        while (1.0f < groff[i].y) {
            groff[i].y = groff[i].y - 2.0f;
        }
        while (1.0f < groff[i].z) {
            groff[i].z = groff[i].z - 2.0f;
        }
        while (groff[i].x < -1.0f) {
            groff[i].x = groff[i].x + 2.0f;
        }
        while (groff[i].y < -1.0f) {
            groff[i].y = groff[i].y + 2.0f;
        }
        while (groff[i].z < -1.0f) {
            groff[i].z = groff[i].z + 2.0f;
        }
        cloudRender(&camera_trans, &groff[i], CLOUDSIZE, CLOUDRNG, (s32)cloud_density,
                    (s32)MAXCLOUDS, clouds, cloudmtl, &col);
    }
}


void TimeTunnelInit(union variptr_u *buffer, union variptr_u *buffend) {
    struct nuvec_s v;
    s32 i, j;
    f32 r;

    ttmtlcnt = 0;
    for (i = 0; i < ttcount; i++) {
        v.x = 0.0f;
        r = NuRandFloat();
        v.y = ((r * ttrange + r * ttrange) - ttrange) * 16.0f;
        r = NuRandFloat();
        v.z = ((r * 0.75f + 0.25f) * ttrange) * 16.0f;
        NuVecRotateY(&v, &v, rand() << 1);
        ttptls[i].x = (s16)v.x;
        ttptls[i].y = (s16)v.y;
        ttptls[i].z = (s16)v.z;
        for (j = 0; j < 7; j++) {
            f32 ratio = ttsize / ttspan;
            ttptls[(j + 1) * ttcount + i] = ttptls[j * ttcount + i];
            ttptls[(j + 1) * ttcount + i].y -= ratio;
        }
    }
    for (i = 0; i < 5; i++) {
        ttoff[i].x = ttoff[i].z = 0.0f;
        ttoff[i].y = NuRandFloat();
    }
    superbuffer_ptr.intaddr = (superbuffer_ptr.intaddr + 0x7F) & ~0x7F;
    ttmtl = 0;
    tttex = 0;
    TextureCreateEx(tt_texname, &ttmtl, &tttex, &superbuffer_ptr, buffer, buffer, buffend);
    superbuffer_ptr.intaddr = (superbuffer_ptr.intaddr + 0x7F) & ~0x7F;
    if (ttmtl != 0) {
        ttmtl->attrib.alpha = 2;
        ttmtl->attrib.zmode = 1;
        ttmtl->alpha_sort = 0x7FFF;
        NuMtlUpdateEx(ttmtl, 1);
        ttmtl->stream.mpgtag = cloud_mpg;
    }
}


void TimeTunnelRender(void) {
    struct nuvec_s col;
    s32 i;
    s32 j;
    f32 density;

    col = cloud_colour;
    if (clouds_off != 0 || ttmtl == 0) {
        return;
    }
    if (++ttmtlcnt < 4) {
        return;
    }
    for (i = 0; i < 5; i++) {
        ttoff[i].x = 0.0f;
        ttoff[i].y += ttvel[i].y;
        ttoff[i].z += ttoff[i].x;
        while (1.0f < ttoff[i].x) {
            ttoff[i].x = ttoff[i].x - 2.0f;
        }
        while (1.0f < ttoff[i].y) {
            ttoff[i].y = ttoff[i].y - 2.0f;
        }
        while (1.0f < ttoff[i].z) {
            ttoff[i].z = ttoff[i].z - 2.0f;
        }
        while (ttoff[i].x < -1.0f) {
            ttoff[i].x = ttoff[i].x + 2.0f;
        }
        while (ttoff[i].y < -1.0f) {
            ttoff[i].y = ttoff[i].y + 2.0f;
        }
        while (ttoff[i].z < -1.0f) {
            ttoff[i].z = ttoff[i].z + 2.0f;
        }
        density = NuTrigTable[tt_density[i]] * 16.0f + 48.0f;
        tt_density[i] += ttdenrate[i];
        cloudRender(&camera_trans, &ttoff[i], ttsize * 0.5f, ttrange, (s32)density,
                    ttcount, ttptls, ttmtl, &col);
        for (j = 0; j < ttgmul[i]; j++) {
            cloudRender(&camera_trans, &ttoff[i], ttsize / ((f32)j * 0.1f + 1.0f),
                        ttrange, (s32)density, ttcount, &ttptls[ttcount * j], ttmtl,
                        &tt_colours[i]);
        }
    }
    for (i = 0; i < 2; i++) {
        ttcoff[i].x = 0.0f;
        ttcoff[i].y += ttcvel[i].y;
        ttcoff[i].z += 0.0f;
        while (1.0f < ttcoff[i].x) {
            ttcoff[i].x = ttcoff[i].x - 2.0f;
        }
        while (1.0f < ttcoff[i].y) {
            ttcoff[i].y = ttcoff[i].y - 2.0f;
        }
        while (1.0f < ttcoff[i].z) {
            ttcoff[i].z = ttcoff[i].z - 2.0f;
        }
        while (ttcoff[i].x < -1.0f) {
            ttcoff[i].x = ttcoff[i].x + 2.0f;
        }
        while (ttcoff[i].y < -1.0f) {
            ttcoff[i].y = ttcoff[i].y + 2.0f;
        }
        while (ttcoff[i].z < -1.0f) {
            ttcoff[i].z = ttcoff[i].z + 2.0f;
        }
        density = NuTrigTable[tt_cdensity[i]] * 16.0f + 32.0f;
        tt_cdensity[i] += ttcdenrate[i];
        cloudRender(&camera_trans, &ttcoff[i], ttcsize, ttrange, (s32)density,
                    ttcount, ttptls, ttmtl, &ttccol[i]);
    }
}


void cloudProcess(void) {
    if (cloudreset != 0) {
        cloudInit();
        cloudreset = 0;
    }
}


void CloseClouds(void) {
}


void TimeTunnelClose(void) {
    if (ttmtl != 0) {
        NuMtlDestroy(ttmtl);
    }
    ttmtl = 0;
    tttex = 0;
}


void CloudFxInit(void) {
    if (clouds_rs == 0) {
        clouds_rs = NuRndrStreamAllocStream(0x10000000, cloud_mpg, 0x3C);
    }
}
