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

#include "game/cloudfx.h"
#include "nu3d/numtl.h"
#include "nu3d/nustream.h"
#include "nu3d/nutex.h"
#include "numath/nurand.h"
#include "numath/nuvec.h"
#include "nucore/nucoretypes.h"

struct nuivec_s {
    s32 x;
    s32 y;
    s32 z;
};

extern u8 vu_load_cloud_mpg[];
extern struct nuvec_s D_002D3EF0;
#define camera_trans D_002D3EF0
extern f32 NuTrigTable[];
extern s32 PS2_REZ_H;
extern u8 *rndrstream_free;
extern union variptr_u superbuffer_ptr;

f32 CLOUDSIZE = 512.0f;
f32 MAXCLOUDS = 512.0f;
f32 CLOUDRNG = 15.0f;
struct numtl_s *cloudmtl = 0;
s32 cloudreset = 0;
struct rndrstream_s *clouds_rs = 0;
f32 cloud_density = 64.0f;
static s32 disable_clouds = 0;
static f32 TTSIZE = 1024.0f;
static s32 TTMAX = 256;
static f32 TTRNG = 15.0f;
static f32 TTSPREAD = 768.0f;
struct numtl_s *ttmtl = 0;
static f32 TTCSIZE = 32768.0f;
struct nutex_s *cloudtex = 0;
s32 cloud_drawn = 0;
struct nutex_s *tttex = 0;
s32 ttmtlcnt = 0;

/* Cloud layer drift: per-axis wobble amplitude and constant velocity. */
struct nuvec_s grscale = { 0.002f, 0.002f, 0.002f };
struct nuvec_s grvel = { 0.0f, 0.005f, 0.0f };

/* Time tunnel: five particle rings and two colour layers. */
struct nuvec_s ttvel[5] = {
    { 0.0f, -0.02f, 0.0f },
    { 0.0f, -0.03f, 0.0f },
    { 0.0f, -0.01f, 0.0f },
    { 0.0f, -0.02f, 0.0f },
    { 0.0f, -0.05f, 0.0f },
};
s32 ttgmul[5] = { 8, 8, 4, 2, 8 };
struct nuvec_s tt_colours[5] = {
    { 1.0f, 0.0f, 1.0f },
    { 1.0f, 0.0f, 0.0f },
    { 0.0f, 0.5f, 0.0f },
    { 0.5f, 0.0f, 0.5f },
    { 0.0f, 0.0f, 1.0f },
};
u16 tt_density[5] = { 0x0000, 0x4000, 0x8000, 0x1000, 0xBC40 };
u16 ttdenrate[5] = { 182, 2000, 546, 182, 400 };
struct nuvec_s ttcvel[2] = {
    { 0.0f, -0.01f, 0.0f },
    { 0.0f, -0.012f, 0.0f },
};
struct nuvec_s ttccol[2] = {
    { 0.5f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.5f },
};
u16 tt_cdensity[] = { 0x0000, 0x4000 };
u16 ttcdenrate[] = { 182, 320 };

struct cloud_s clouds[4096] __attribute__((aligned(128))) = { 0 };
struct nuvec_s groff[10] = { 0 };
struct nuivec_s grphase[10] = { 0 };
struct nuivec_s grphaserate[10] = { 0 };
/* [8][count] trail particles, stride 6 */
struct cloud_s ttptls[4096] __attribute__((aligned(128))) = { 0 };
struct nuvec_s ttoff[5] = { 0 };
struct nuvec_s ttcoff[5] = { 0 };

extern void cloudInit();
extern void TextureCreate(void *desc, void *mtl, void *tex);
extern void TextureCreateEx(void *desc, void *mtl, void *tex, void *sbptr,
                            void *buf, void *buf2, void *buffend);
extern void NuMemFreeFn(void *ptr, void *src, s32 id);
extern void NuRndrStreamLink(struct rndrstream_s *rs);
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
    TextureCreate("stuff\\cloudslo.bmp", &cloudmtl, &cloudtex);
    if (cloudmtl != 0) {
        cloudmtl->attrib.alpha = 2;
        cloudmtl->attrib.zmode = 1;
        cloudmtl->sort_pri = 0x7D00;
        NuMtlUpdateEx(cloudmtl, 1);
        cloudmtl->rs.mpg = vu_load_cloud_mpg;
    }
    if (cloudtex != 0) {
        if (cloudtex->bits != 0) {
            NuMemFreeFn(cloudtex->bits, ".\\cloudfx.c", 0x6C);
        }
        NuMemFreeFn(cloudtex, ".\\cloudfx.c", 0x6D);
    }
}


void cloudRender(struct nuvec_s *pos, struct nuvec_s *off, f32 size, f32 rng, s32 density,
                 s32 maxc, struct cloud_s *cl, struct numtl_s *mtl, struct nuvec_s *col) {
    union {
        u64 dw[2];
        u32 w[4];
    } gif;
    struct nuvec_s scroll;
    struct rndrstream_s *rs;
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
    rs->ptr.u8 = vpDmaTag_CntEx(rs->ptr.u8, 0, 0x6C068002);
    *rs->ptr.f32++ = pos->x;
    *rs->ptr.f32++ = pos->y;
    *rs->ptr.f32++ = pos->z;
    *rs->ptr.f32++ = rng;
    NuVecScale(&scroll, off, rng);
    wrap = rng + rng;
    x = pos->x + scroll.x;
    *rs->ptr.f32++ = x - wrap * (s32)(x / wrap);
    y = pos->y + scroll.y;
    *rs->ptr.f32++ = y - wrap * (s32)(y / wrap);
    z = pos->z + scroll.z;
    *rs->ptr.f32++ = z - wrap * (s32)(z / wrap);
    *rs->ptr.s32++ = 0;
    *rs->ptr.f32++ = size;
    *rs->ptr.f32++ = size * 0.5f;
    *rs->ptr.f32++ = density;
    *rs->ptr.s32++ = 0;
    *rs->ptr.s32++ = 0;
    *rs->ptr.s32++ = 0;
    *rs->ptr.s32++ = 0;
    *rs->ptr.s32++ = 0;
    *rs->ptr.f32++ = col->x;
    *rs->ptr.f32++ = col->y;
    *rs->ptr.f32++ = col->z;
    *rs->ptr.s32++ = 0;
    *rs->ptr.f32++ = (2048.0f + 320.0f) * 16.0f;
    *rs->ptr.f32++ = ((PS2_REZ_H >> 1) + 2048.0f) * 16.0f;
    *rs->ptr.f32++ = (2048.0f - 320.0f) * 16.0f;
    *rs->ptr.f32++ = (2048.0f - (PS2_REZ_H >> 1)) * 16.0f;
    *rs->ptr.u32++ = 0x14000300;
    *rs->ptr.u32++ = 0x17000000;
    rs->ptr.u8 = vpDmaTag_Close(rs->ptr.u8);
    gif.dw[0] = 0x5003400000008000ULL;
    gif.dw[1] = 0x52521;
    while (maxc > 0) {
        n = (maxc > 0x20) ? 0x20 : maxc;
        maxc -= 0x20;
        rs->ptr.u8 = vpDmaTag_CntEx(rs->ptr.u8, 0, 0x6D018002);
        *((u16 *)rs->ptr.u8)++ = n;
        *((u16 *)rs->ptr.u8)++ = 0;
        *((u16 *)rs->ptr.u8)++ = 0;
        *((u16 *)rs->ptr.u8)++ = 0;
        *rs->ptr.u32++ = 0x6C0180D2;
        *rs->ptr.u32++ = gif.w[0];
        *rs->ptr.u32++ = gif.w[1];
        *rs->ptr.u32++ = gif.w[2];
        *rs->ptr.u32++ = gif.w[3];
        *rs->ptr.u32++ = 0;
        *rs->ptr.u32++ = 0;
        *rs->ptr.u32++ = 0;
        rs->ptr.u8 = vpDmaTag_Close(rs->ptr.u8);
        rs->ptr.u8 = vpDmaTag_RefEx(rs->ptr.u8, (u32)cl, 0, (n << 16) | 0x69008003);
        cl += n;
        ((u16 *)rs->ptr.u8)[-8] = (n * 6) >> 4;
        rs->ptr.u8 = vpDmaTag_CntEx(rs->ptr.u8, 0, 0x17000000);
    }
    rs->ptr.u8 = vpDmaTag_CntEx(rs->ptr.u8, 0, 0x10000000);
    rndrstream_free = vpDmaTag_Next(rs->ptr.u8, 0);
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
    struct nuvec_s col = { 1.0f, 1.0f, 1.0f };
    s32 i;

    if (disable_clouds != 0) {
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
    for (i = 0; i < TTMAX; i++) {
        v.x = 0.0f;
        r = NuRandFloat();
        v.y = ((r * TTRNG + r * TTRNG) - TTRNG) * 16.0f;
        r = NuRandFloat();
        v.z = ((r * 0.75f + 0.25f) * TTRNG) * 16.0f;
        NuVecRotateY(&v, &v, rand() << 1);
        ttptls[i].x = (s16)v.x;
        ttptls[i].y = (s16)v.y;
        ttptls[i].z = (s16)v.z;
        for (j = 0; j < 7; j++) {
            f32 ratio = TTSIZE / TTSPREAD;
            ttptls[(j + 1) * TTMAX + i] = ttptls[j * TTMAX + i];
            ttptls[(j + 1) * TTMAX + i].y -= ratio;
        }
    }
    for (i = 0; i < 5; i++) {
        ttoff[i].x = ttoff[i].z = 0.0f;
        ttoff[i].y = NuRandFloat();
    }
    superbuffer_ptr.intaddr = (superbuffer_ptr.intaddr + 0x7F) & ~0x7F;
    ttmtl = 0;
    tttex = 0;
    TextureCreateEx("stuff\\ttunnel.bmp", &ttmtl, &tttex, &superbuffer_ptr, buffer, buffer,
                    buffend);
    superbuffer_ptr.intaddr = (superbuffer_ptr.intaddr + 0x7F) & ~0x7F;
    if (ttmtl != 0) {
        ttmtl->attrib.alpha = 2;
        ttmtl->attrib.zmode = 1;
        ttmtl->sort_pri = 0x7FFF;
        NuMtlUpdateEx(ttmtl, 1);
        ttmtl->rs.mpg = vu_load_cloud_mpg;
    }
}


void TimeTunnelRender(void) {
    struct nuvec_s col = { 1.0f, 1.0f, 1.0f };
    s32 i;
    s32 j;
    f32 density;

    if (disable_clouds != 0 || ttmtl == 0) {
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
        cloudRender(&camera_trans, &ttoff[i], TTSIZE * 0.5f, TTRNG, (s32)density,
                    TTMAX, ttptls, ttmtl, &col);
        for (j = 0; j < ttgmul[i]; j++) {
            cloudRender(&camera_trans, &ttoff[i], TTSIZE / ((f32)j * 0.1f + 1.0f),
                        TTRNG, (s32)density, TTMAX, &ttptls[TTMAX * j], ttmtl,
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
        cloudRender(&camera_trans, &ttcoff[i], TTCSIZE, TTRNG, (s32)density,
                    TTMAX, ttptls, ttmtl, &ttccol[i]);
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
        clouds_rs = NuRndrStreamAllocStream(0x10000000, vu_load_cloud_mpg, 0x3C);
    }
}
