/*
 * Unit: nups2/ptl
 *
 * Functions:
 *   0x0016e890 CreateDmaPartEffectList
 *   0x0016eac0 CreateDmaParticleSet
 *   0x0016ebb0 LinkDmaParticalSets
 *   0x0016ec48 PartDebugHeader
 */

#include "nu_types.h"

#include "nups2/nups2types.h"

typedef union PartLane {
    f32 f;
    u8 b;
} PartLane;

typedef struct PartEffectVert {
    PartLane pos[4];
    f32 pos2[4];
} PartEffectVert;

typedef struct PartEffectList {
    struct DmaTag tag;
    u32 params[4];
    struct DmaTag datatag;
    int corners[4][2];
    u32 pad[4];
    PartEffectVert verts[64];
    u32 mscal;
} PartEffectList;

typedef struct PartSetVert {
    f32 pos[4];
    f32 col[4];
} PartSetVert;

typedef union GifHeader {
    u64 dword[4];
    u32 word[8];
} GifHeader;

#define DMA_ID_MASK     0x8FFFFFFF
#define DMA_ID_NEXT     0x20000000
#define DMA_ID_RET      0x60000000
#define DMA_ADDR_MASK   0x7FFFFFFF
#define DMA_SPR         0x80000000

#define VIF_FLUSH       0x11000000
#define VIF_MSCAL       0x14000000
#define VIF_MSCNT       0x17000000

extern f32 NuTrigTable[];

extern u32 *vpDmaTag_RetEx(void *tag, u32 vif0, u32 vif1);
extern u32 vpDmaTag_Close(void *end);


PartEffectList *CreateDmaPartEffectList(PartEffectList *list, int *size) {
    PartEffectVert *v;
    u32 *p;
    int i;
    f32 a, b;

    p = (u32 *)&list->datatag;
    vpDmaTag_RetEx(list, VIF_FLUSH, 0x6C01003C);
    vpDmaTag_Close(p);
    vpDmaTag_RetEx(p, VIF_FLUSH, 0x6C83003D);
    list->params[1] = 0;
    list->corners[0][0] = 0x38;
    list->corners[0][1] = 0x48;
    list->corners[1][0] = 0xC8;
    list->corners[1][1] = 0x48;
    list->corners[2][0] = 0x38;
    list->corners[2][1] = 0xB8;
    list->corners[3][0] = 0xC8;
    list->corners[3][1] = 0xB8;
    v = list->verts;
    for (i = 0; i < 64; v++, i++) {
        a = 0x200 + i * 8;
        b = -0x200 - i * 8;
        v->pos[0].f = (b * NuTrigTable[(u16)(i * 0x200 + 0x4000)] - a * NuTrigTable[(u16)(i * 0x200)])
            * 640.0f * 0.00390625f;
        v->pos[1].f = b * NuTrigTable[(u16)(i * 0x200)] + a * NuTrigTable[(u16)(i * 0x200 + 0x4000)];
        v->pos2[0] = (a * NuTrigTable[(u16)(i * 0x200 + 0x4000)] - a * NuTrigTable[(u16)(i * 0x200)])
            * 640.0f * 0.00390625f;
        v->pos2[1] = a * NuTrigTable[(u16)(i * 0x200)] + a * NuTrigTable[(u16)(i * 0x200 + 0x4000)];
        v->pos[2].f = (f32)(0x400 + i * 16) * NuTrigTable[(u16)(i * 0x200)] * 640.0f * 0.00390625f;
        v->pos[3].f = (f32)(-0x400 - i * 16) * NuTrigTable[(u16)(i * 0x200 + 0x4000)];
        v->pos[0].b = -0x80 - i;
        v->pos[1].b = 0x50 - i;
        v->pos[2].b = 0;
        if (i <= 0x30) {
            v->pos[3].b = 0x10;
        } else {
            v->pos[3].b = 0x40 - i;
        }
    }
    p = (u32 *)v;
    *p++ = VIF_MSCAL | 0x302;
    *size = vpDmaTag_Close(p) - (u32)list;
    return list;
}


struct DmaTag *CreateDmaParticleSet(struct DmaTag *set, int *size) {
    GifHeader gif;
    u32 *p;
    PartSetVert *v;
    int i;

    p = vpDmaTag_RetEx(set, 0, 0x6C418000);
    gif.dword[0] = 0x9002400000008020;
    *p++ = gif.word[0];
    *p++ = gif.word[1];
    *p++ = 0x35353531;
    *p++ = 5;
    v = (PartSetVert *)p;
    for (i = 0; i < 32; i++) {
        v->pos[0] = v->pos[1] = v->pos[2] = 0.0f;
        v->col[0] = v->col[1] = v->col[2] = 0.0f;
        v->pos[3] = -1.0f;
        v->col[3] = 128.0f;
        v++;
    }
    p = (u32 *)v;
    *p++ = VIF_MSCNT;
    *size = vpDmaTag_Close(p) - (u32)set;
    return set;
}


void LinkDmaParticalSets(struct DmaTag **sets, int count) {
    struct DmaTag *tag;
    int i;

    tag = sets[count - 1];
    tag->ctrl = (tag->ctrl & DMA_ID_MASK) + DMA_ID_RET;
    tag->addr = 0;
    for (i = count - 2; i >= 0; i--) {
        tag = sets[i];
        tag->ctrl = (tag->ctrl & DMA_ID_MASK) + DMA_ID_NEXT;
        tag->addr = (tag->addr & DMA_SPR) + ((u32)sets[i + 1] & DMA_ADDR_MASK);
    }
}


void PartDebugHeader(PartEffectList *list, int mode) {
    switch (mode) {
    case 0:
        list->mscal = VIF_MSCAL | 0x302;
        break;
    case 1:
        list->mscal = VIF_MSCAL | 0x300;
        break;
    case 2:
        list->mscal = VIF_MSCAL | 0x304;
        break;
    }
}
