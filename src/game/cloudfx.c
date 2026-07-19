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

#include "creature.h"

extern s32 cloudreset;
extern void *ttmtl;
extern void *tttex;
extern void *clouds_rs;
extern u8 D_00293090[];

struct nuivec_s {
    s32 x;
    s32 y;
    s32 z;
};

extern struct nuvec_s groff[];
extern struct nuivec_s grphase[];
extern struct nuivec_s grphaserate[];

extern void cloudInit();
extern void NuMtlDestroy(void *mtl);
extern void *NuRndrStreamAllocStream(void *a0, void *a1, s32 a2);
extern f32 NuRandFloat(void);
extern int rand(void);

/* ---- cloudInit support ---- */

/* PS2 compact cloud vertex table (s16 x/y/z), stride 6.
 * offset 0x0/0x2/0x4 verified in cloudInit stores. */
struct cloud_s {
    s16 x;
    s16 y;
    s16 z;
};

/* Cloud material.  Offsets verified in cloudInit:
 *   attrib   (+0x168)  ld/sd 64-bit render-attrib word
 *   fxdata   (+0x13C)  sw D_00293090
 *   field1A8 (+0x1A8)  sw 0x7D00 */
struct cloudmtl_s {
    char pad_000[0x13C];
    void *fxdata;              /* +0x13C */
    char pad_140[0x168 - 0x140];
    u64 attrib;               /* +0x168 */
    char pad_170[0x1A8 - 0x170];
    s32 field_1A8;            /* +0x1A8 */
};

/* Cloud texture.  bits (+0x10) verified in cloudInit free path. */
struct cloudtex_s {
    char pad_00[0x10];
    void *bits;               /* +0x10 */
};

extern struct cloud_s clouds[];
extern f32 MAXCLOUDS;
extern f32 CLOUDRNG;
extern struct cloudmtl_s *cloudmtl;
extern struct cloudtex_s *cloudtex;
extern u8 D_00625260[];
extern u8 D_00625278[];

extern void TextureCreate(void *desc, void *mtl, void *tex);
extern void NuMtlUpdateEx(struct cloudmtl_s *mtl, s32 flag);
extern void NuMemFreeFn(void *ptr, void *src, s32 id);

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
    TextureCreate(D_00625260, &cloudmtl, &cloudtex);
    if (cloudmtl != 0) {
        cloudmtl->attrib =
            (((cloudmtl->attrib & 0xFFFFFFF0FFFFFFFFULL) | 0x0000000200000000ULL) &
             0xFFFF3FFFFFFFFFFFULL) | 0x0000400000000000ULL;
        cloudmtl->field_1A8 = 0x7D00;
        NuMtlUpdateEx(cloudmtl, 1);
        cloudmtl->fxdata = D_00293090;
    }
    if (cloudtex != 0) {
        if (cloudtex->bits != 0) {
            NuMemFreeFn(cloudtex->bits, D_00625278, 0x6C);
        }
        NuMemFreeFn(cloudtex, D_00625278, 0x6D);
    }
}

/* ---- DoClouds support ---- */

extern s32 D_00632904;                 /* cloud disable flag (gp-rel) */
extern struct nuvec_s D_00625288;      /* cloud render colour */
extern struct nuvec_s grscale;
extern struct nuvec_s grvel;
extern f32 NuTrigTable[];
extern f32 cloud_density;
extern f32 CLOUDSIZE;
extern u8 D_002D3EF0[];                 /* cloud render matrix */

void cloudRender(void *m, struct nuvec_s *off, f32 size, f32 rng, s32 density,
                 s32 maxc, struct cloud_s *cl, struct cloudmtl_s *mtl,
                 struct nuvec_s *col);

/* ---- cloudRender support ---- */

struct rndrstream_s {
    u8 *pad0;     /* 0x0 */
    u8 *cur;      /* 0x4  DMA write cursor */
};

extern s32 PS2_REZ_H;
extern u8 *rndrstream_free;

extern void NuRndrStreamLink(struct rndrstream_s *rs);
extern void NuRndrStreamSetMisc(struct rndrstream_s *rs);
extern void NuRndrStreamAddMtl(struct rndrstream_s *rs, struct cloudmtl_s *mtl);
extern void NuRndrStreamSetViewMtx(struct rndrstream_s *rs);
extern void NuVecScale(struct nuvec_s *dst, struct nuvec_s *src, f32 scale);
extern u8 *vpDmaTag_CntEx(u8 *cur, s32 qwc, u32 tag);
extern u8 *vpDmaTag_Close(u8 *cur);
extern u8 *vpDmaTag_RefEx(u8 *cur, u32 addr, s32 a2, u32 tag);
extern u8 *vpDmaTag_Next(u8 *cur);

/* ---- TimeTunnelInit support ---- */

extern s32 D_0063290C;                 /* tunnel particle count */
extern f32 D_00632908;
extern f32 D_00632910;
extern f32 D_00632914;
extern s32 ttmtlcnt;
extern struct cloud_s ttptls[];        /* [8][count] trail particles, stride 6 */
extern struct nuvec_s ttoff[];
extern s32 superbuffer_ptr;
extern u8 D_00625298[];

extern void NuVecRotateY(struct nuvec_s *dst, struct nuvec_s *src, s32 angle);
extern void TextureCreateEx(void *desc, void *mtl, void *tex, void *sbptr,
                            void *buf, void *buf2, void *buffend);

void DoClouds(s32 paused) {
    struct nuvec_s col;
    s32 i;

    col = D_00625288;
    if (D_00632904 != 0) {
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
        cloudRender(D_002D3EF0, &groff[i], CLOUDSIZE, CLOUDRNG, (s32)cloud_density,
                    (s32)MAXCLOUDS, clouds, cloudmtl, &col);
    }
}

void cloudRender(void *m, struct nuvec_s *off, f32 size, f32 rng, s32 density,
                 s32 maxc, struct cloud_s *cl, struct cloudmtl_s *mtl,
                 struct nuvec_s *col) {
    struct nuvec_s scaled;
    u64 gt[2];
    struct rndrstream_s *rs;
    struct nuvec_s *target;
    struct cloud_s *clptr;
    u8 *p;
    u8 *q;
    u8 *r2;
    f32 twoRng;
    f32 cx, cy, cz;
    s32 n;
    s32 nb;

    target = (struct nuvec_s *)m;
    rs = clouds_rs;
    NuRndrStreamLink(rs);
    NuRndrStreamSetMisc(rs);
    NuRndrStreamAddMtl(rs, mtl);
    NuRndrStreamSetViewMtx(rs);

    p = vpDmaTag_CntEx(rs->cur, 0, 0x6C068002);
    rs->cur = p;
    ((f32 *)p)[0] = target->x;  rs->cur = p + 0x4;
    ((f32 *)p)[1] = target->y;  rs->cur = p + 0x8;
    ((f32 *)p)[2] = target->z;  rs->cur = p + 0xC;
    ((f32 *)p)[3] = rng;        rs->cur = p + 0x10;
    NuVecScale(&scaled, off, rng);

    twoRng = rng + rng;
    p = rs->cur;
    cx = target->x + scaled.x;
    cx = cx - twoRng * (f32)(s32)(cx / twoRng);
    ((f32 *)p)[0] = cx;         rs->cur = p + 0x4;
    cy = target->y + scaled.y;
    cy = cy - twoRng * (f32)(s32)(cy / twoRng);
    ((f32 *)p)[1] = cy;         rs->cur = p + 0x8;
    cz = target->z + scaled.z;
    cz = cz - twoRng * (f32)(s32)(cz / twoRng);
    ((f32 *)p)[2] = cz;         rs->cur = p + 0xC;
    ((s32 *)p)[3] = 0;          rs->cur = p + 0x10;
    ((f32 *)p)[4] = size;       rs->cur = p + 0x14;
    ((f32 *)p)[5] = size * 0.5f; rs->cur = p + 0x18;
    ((f32 *)p)[6] = (f32)density; rs->cur = p + 0x1C;
    ((s32 *)p)[7] = 0;          rs->cur = p + 0x20;
    ((s32 *)p)[8] = 0;          rs->cur = p + 0x24;
    ((s32 *)p)[9] = 0;          rs->cur = p + 0x28;
    ((s32 *)p)[10] = 0;         rs->cur = p + 0x2C;
    ((s32 *)p)[11] = 0;         rs->cur = p + 0x30;
    ((f32 *)p)[12] = col->x;    rs->cur = p + 0x34;
    ((f32 *)p)[13] = col->y;    rs->cur = p + 0x38;
    ((f32 *)p)[14] = col->z;    rs->cur = p + 0x3C;
    ((s32 *)p)[15] = 0;         rs->cur = p + 0x40;
    ((f32 *)p)[16] = 38912.0f;  rs->cur = p + 0x44;
    ((f32 *)p)[17] = ((f32)(PS2_REZ_H >> 1) + 2048.0f) * 16.0f; rs->cur = p + 0x48;
    ((f32 *)p)[18] = 27648.0f;  rs->cur = p + 0x4C;
    ((f32 *)p)[19] = (2048.0f - (f32)(PS2_REZ_H >> 1)) * 16.0f; rs->cur = p + 0x50;
    ((s32 *)p)[20] = 0x14000300; rs->cur = p + 0x54;
    ((s32 *)p)[21] = 0x17000000; rs->cur = p + 0x58;
    rs->cur = vpDmaTag_Close(rs->cur);

    gt[0] = 0x5003400000008000ULL;
    gt[1] = 0x0000000000052521ULL;
    clptr = cl;
    while (maxc > 0) {
        n = 0x20;
        if (maxc < 0x21) {
            n = maxc;
        }
        maxc -= 0x20;
        q = vpDmaTag_CntEx(rs->cur, 0, 0x6D018002);
        rs->cur = q;
        ((u16 *)q)[0] = (u16)n;     rs->cur = q + 0x2;
        ((u16 *)q)[1] = 0;          rs->cur = q + 0x4;
        ((u16 *)q)[2] = 0;          rs->cur = q + 0x6;
        ((u16 *)q)[3] = 0;          rs->cur = q + 0x8;
        ((u32 *)q)[2] = 0x6C0180D2; rs->cur = q + 0xC;
        ((u32 *)q)[3] = ((u32 *)gt)[0]; rs->cur = q + 0x10;
        ((u32 *)q)[4] = ((u32 *)gt)[1]; rs->cur = q + 0x14;
        ((u32 *)q)[5] = ((u32 *)gt)[2]; rs->cur = q + 0x18;
        ((u32 *)q)[6] = ((u32 *)gt)[3]; rs->cur = q + 0x1C;
        ((u32 *)q)[7] = 0;          rs->cur = q + 0x20;
        ((u32 *)q)[8] = 0;          rs->cur = q + 0x24;
        ((u32 *)q)[9] = 0;          rs->cur = q + 0x28;
        rs->cur = vpDmaTag_Close(rs->cur);
        r2 = vpDmaTag_RefEx(rs->cur, (u32)clptr, 0, (n << 16) | 0x69008003);
        rs->cur = r2;
        nb = n * 6;
        clptr = (struct cloud_s *)((u8 *)clptr + nb);
        *(u16 *)(r2 - 0x10) = (u16)(nb >> 4);
        rs->cur = vpDmaTag_CntEx(rs->cur, 0, 0x17000000);
    }
    rs->cur = vpDmaTag_CntEx(rs->cur, 0, 0x10000000);
    rndrstream_free = vpDmaTag_Next(rs->cur);
}

void TimeTunnelInit(void *buffer, void *buffend) {
    struct nuvec_s v;
    struct nuvec_s *p;
    struct cloud_s *tp;
    s32 i, j, k;
    f32 r;
    struct cloudmtl_s *mtl;

    ttmtlcnt = 0;
    tp = ttptls;
    for (i = 0; i < D_0063290C; i++, tp++) {
        v.x = 0.0f;
        r = NuRandFloat();
        v.y = ((r * D_00632910 + r * D_00632910) - D_00632910) * 16.0f;
        r = NuRandFloat();
        v.z = ((r * 0.75f + 0.25f) * D_00632910) * 16.0f;
        NuVecRotateY(&v, &v, rand() << 1);
        tp->x = (s16)v.x;
        tp->y = (s16)v.y;
        tp->z = (s16)v.z;
        j = 0;
        do {
            f32 ratio = D_00632908 / D_00632914;
            k = j + 1;
            ttptls[k * D_0063290C + i] = ttptls[j * D_0063290C + i];
            ttptls[k * D_0063290C + i].y =
                (s16)((f32)ttptls[k * D_0063290C + i].y - ratio);
            j = k;
        } while (j < 7);
    }
    p = ttoff;
    do {
        p->x = 0.0f;
        p->y = NuRandFloat();
        p->z = 0.0f;
        p++;
    } while (p < ttoff + 5);
    superbuffer_ptr = (superbuffer_ptr + 0x7F) & ~0x7F;
    ttmtl = 0;
    tttex = 0;
    TextureCreateEx(D_00625298, &ttmtl, &tttex, &superbuffer_ptr, buffer, buffer, buffend);
    superbuffer_ptr = (superbuffer_ptr + 0x7F) & ~0x7F;
    mtl = (struct cloudmtl_s *)ttmtl;
    if (mtl != 0) {
        mtl->attrib =
            (((mtl->attrib & 0xFFFFFFF0FFFFFFFFULL) | 0x0000000200000000ULL) &
             0xFFFF3FFFFFFFFFFFULL) | 0x0000400000000000ULL;
        mtl->field_1A8 = 0x7FFF;
        NuMtlUpdateEx(mtl, 1);
        mtl->fxdata = D_00293090;
    }
}


void InitClouds(void *buffer, void *buffend) {
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
        clouds_rs = NuRndrStreamAllocStream((void *)0x10000000, D_00293090, 0x3C);
    }
}
