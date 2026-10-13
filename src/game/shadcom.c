/*
 * Unit: game/shadcom
 *
 * Functions:
 *   0x0024c980 ShadRndr
 *   0x0024cc20 ShadDataLoad
 *   0x0024cee0 ShadDataDestroy
 *   0x0024cf78 ShadFindData
 *   0x0024d020 ShadComInit
 */

#include "creature.h"

struct numtlattrib_s {
    u64 unk0 : 32;
    u64 alpha : 4;                /* bits 32-35 */
    u64 filter : 2;               /* bits 36-37 */
    u64 unk38 : 6;
    u64 cull : 2;                 /* bits 44-45 */
    u64 zmode : 2;                /* bits 46-47 */
    u64 unk48 : 16;
};

struct nurndrstream_s {
    u8 *start;                    /* 0x00 */
    u8 *cur;                      /* 0x04 */
    u8 pad08[0xC];
    struct numtl_s *mtl;          /* 0x14 */
    u8 pad18[0x1C];
    s32 mtl_tid;                  /* 0x34 */
};

struct numtl_s {
    u8 pad000[0x124];
    struct nurndrstream_s rs;     /* 0x124 */
    u8 pad15C[0xC];
    struct numtlattrib_s attrib;  /* 0x168 */
    u8 pad170[0x10];
    struct nucolour3_s diffuse;   /* 0x180 */
    u8 pad18C[0x14];
    f32 alpha;                    /* 0x1A0 */
    s32 tid;                      /* 0x1A4 */
    s32 alpha_sort;               /* 0x1A8 */
    u8 pad1AC[0x24];
}; /* 0x1D0 */

struct nutex_s {
    s32 type;                     /* 0x00 */
    s32 width;                    /* 0x04 */
    s32 height;                   /* 0x08 */
    s32 mmcnt;                    /* 0x0C */
    void *bits;                   /* 0x10 */
    void *pal;                    /* 0x14 */
    s32 decal;                    /* 0x18 */
    s32 linear;                   /* 0x1C */
}; /* 0x20 */

struct nuvtx_tc1_s {
    struct nuvec_s pnt;           /* 0x00 */
    struct nuvec_s nrm;           /* 0x0C */
    u32 diffuse;                  /* 0x18 */
    f32 tc[2];                    /* 0x1C */
}; /* 0x24 */

struct shadentry_s {
    s16 xframes;                  /* 0x00 */
    s16 yframes;                  /* 0x02 */
    s32 pad04;
    struct numtl_s *mtl;          /* 0x08 */
    s32 tid;                      /* 0x0C */
    u8 native[4];                 /* 0x10 */
};

union shadslot_u {
    s32 offset;
    struct shadentry_s *entry;
    char *name;
};

struct shaddata_s {
    s16 count;                    /* 0x00 */
    s16 external;                 /* 0x02 */
    union shadslot_u table[1];    /* 0x04: entries[count], then names[count] */
};

extern u32 shadcol;
extern struct numtl_s *numtl_defaultmtl3d;
extern u8 *rndrstream_free;

extern u32 *D_005D98C0[4];
#define shadpals D_005D98C0
extern struct nuvtx_tc1_s D_005D98D0[4];
#define shadvtx D_005D98D0
extern s32 D_006B7650[4];
#define shadtids D_006B7650
extern char D_006251F8[];
#define shadcom_file D_006251F8

extern s32 NuFileSize(char *name);
extern s32 NuFileLoadBuffer(char *name, void *buf, s32 max);
extern void *NuMemAllocFn(s32 size, char *file, s32 line);
extern void NuMemFreeFn(void *ptr, char *file, s32 line);
extern void *memset(void *dst, s32 c, s32 n);
extern s32 strcasecmp(char *a, char *b);
extern s32 NuTexCreate(struct nutex_s *tex);
extern s32 NuTexCreateNative(void *native);
extern void NuTexDestroy(s32 tid);
extern void NuTexAccomodateRS(struct nurndrstream_s *rs, s32 tid);
extern u8 *NuTexSetPal(u8 *cur, s32 tid, s32 paltid, s32 mode);
extern struct numtl_s *NuMtlCreate(s32 n);
extern void NuMtlAdd(struct numtl_s *mtl);
extern void NuMtlUpdate(struct numtl_s *mtl);
extern void NuMtlDestroy(struct numtl_s *mtl);
extern void NuRndrStreamLink(struct nurndrstream_s *rs);
extern void NuRndrStreamAddMtl(struct nurndrstream_s *rs, struct numtl_s *mtl);
extern s32 NuRndrStreamTriStrip3dClip(struct nuvtx_tc1_s *vtx, s32 num, struct numtx_s *mtx,
                                      struct nurndrstream_s *rs);
extern u8 *vpDmaTag_CntEx(u8 *cur, s32 qwc, u32 tag);
extern u8 *vpDmaTag_Next(u8 *cur, s32 mode);


void ShadRndr(struct numtx_s *mtx, struct shadentry_s *shad, f32 frame, f32 scale) {
    struct numtx_s m;
    struct numtl_s *mtl;
    struct nurndrstream_s *rs;
    s32 tid;
    s32 last;
    s32 n;
    s32 pal;
    s32 col;
    s32 row;
    f32 du;
    f32 dv;
    u32 colour;

    if (shad == 0 || shad->mtl == 0) {
        return;
    }
    last = (s32)frame - 1;
    n = shad->xframes * shad->yframes * 4;
    du = 1.0f / shad->xframes;
    dv = 1.0f / shad->yframes;
    if (last < n) {
        n = last;
    }
    if (n < 0) {
        n = 0;
    }
    pal = n & 3;
    n >>= 2;
    col = n % shad->xframes;
    row = n / shad->xframes;
    colour = shadcol;
    shadvtx[3].diffuse = colour;
    shadvtx[2].diffuse = colour;
    shadvtx[1].diffuse = colour;
    shadvtx[0].diffuse = colour;
    shadvtx[0].tc[0] = col * du;
    shadvtx[0].tc[1] = row * dv;
    shadvtx[1].tc[0] = (col + 1) * du;
    shadvtx[1].tc[1] = row * dv;
    shadvtx[2].tc[0] = col * du;
    shadvtx[2].tc[1] = (row + 1) * dv;
    shadvtx[3].tc[0] = (col + 1) * du;
    shadvtx[3].tc[1] = (row + 1) * dv;
    m._00 = mtx->_00 * scale;
    m._01 = mtx->_01 * scale;
    m._02 = mtx->_02 * scale;
    m._03 = mtx->_03;
    m._10 = mtx->_10 * scale;
    m._11 = mtx->_11 * scale;
    m._12 = mtx->_12 * scale;
    m._13 = mtx->_13;
    m._20 = mtx->_20 * scale;
    m._21 = mtx->_21 * scale;
    m._22 = mtx->_22 * scale;
    m._23 = mtx->_23;
    m._30 = mtx->_30;
    m._31 = mtx->_31;
    m._32 = mtx->_32;
    m._33 = mtx->_33;
    mtl = numtl_defaultmtl3d;
    if (shad->mtl != 0) {
        mtl = shad->mtl;
    }
    rs = &mtl->rs;
    tid = shadtids[pal];
    if (tid != 0) {
        NuRndrStreamLink(rs);
        rs->mtl = 0;
        rs->mtl_tid = 0;
        NuRndrStreamAddMtl(rs, mtl);
        NuTexAccomodateRS(rs, tid);
        rs->cur = NuTexSetPal(rs->cur, mtl->tid, tid, 0);
        rndrstream_free = vpDmaTag_Next(rs->cur, 0);
    }
    NuRndrStreamTriStrip3dClip(shadvtx, 4, &m, rs);
    NuRndrStreamLink(rs);
    rs->cur = vpDmaTag_CntEx(rs->cur, 0, 0x13000000);
    rndrstream_free = vpDmaTag_Next(rs->cur, 0);
}


struct shaddata_s *ShadDataLoad(char *file, void **buffer) {
    struct shaddata_s *shad;
    struct shaddata_s *data;
    struct shadentry_s *entry;
    struct numtl_s *mtls;
    void *local;
    s32 external;
    s32 size;
    s32 i;

    shad = 0;
    external = 1;
    mtls = 0;
    size = NuFileSize(file);
    if (size > 0) {
        if (buffer == 0) {
            buffer = &local;
            external = 0;
            local = NuMemAllocFn(size + 0x100, shadcom_file, 0x12B);
        }
        *buffer = (void *)(((u32)*buffer + 0xF) & 0xFFFFFFF0);
        size = NuFileLoadBuffer(file, *buffer, 0x7FFFFFFF);
        data = *buffer;
        *buffer = (u8 *)*buffer + size;
        shad = data;
        data->external = external;
        if (external != 0) {
            *buffer = (void *)(((u32)*buffer + 0xF) & 0xFFFFFFF0);
            mtls = *buffer;
            memset(mtls, 0, data->count * sizeof(struct numtl_s));
            *buffer = (u8 *)*buffer + data->count * sizeof(struct numtl_s);
        }
        for (i = 0; i < data->count; i++) {
            ((s32 *)data)[i + 1] += (s32)data;
            ((s32 *)data)[data->count + i + 1] += (s32)data;
            entry = data->table[i].entry;
            entry->tid = 0;
            entry->mtl = 0;
            entry->tid = NuTexCreateNative(entry->native);
            if (entry->tid != 0) {
                if (mtls != 0) {
                    entry->mtl = NuMtlCreate(1);
                } else {
                    entry->mtl = &mtls[i];
                    NuMtlAdd(entry->mtl);
                }
                entry->mtl->diffuse.r = 1.0f;
                entry->mtl->diffuse.g = 1.0f;
                entry->mtl->diffuse.b = 1.0f;
                entry->mtl->attrib.cull = 2;
                entry->mtl->attrib.zmode = 0;
                entry->mtl->alpha = 0.999f;
                entry->mtl->attrib.alpha = 3;
                entry->mtl->attrib.filter = 1;
                entry->mtl->tid = entry->tid;
                entry->mtl->alpha_sort = 100;
                NuMtlUpdate(entry->mtl);
            }
        }
    }
    return shad;
}


void ShadDataDestroy(struct shaddata_s *data) {
    s32 i;

    for (i = 0; i < data->count; i++) {
        NuTexDestroy(data->table[i].entry->mtl->tid);
        NuMtlDestroy(data->table[i].entry->mtl);
    }
    if (data->external == 0) {
        NuMemFreeFn(data, shadcom_file, 0x102);
    }
}


struct shadentry_s *ShadFindData(struct shaddata_s *data, char *name) {
    union shadslot_u *names;
    s32 i;

    if (name == 0) {
        return data->table[0].entry;
    }
    names = &data->table[data->count];
    for (i = 0; i < data->count; i++) {
        if (strcasecmp(names[i].name, name) == 0) {
            return data->table[i].entry;
        }
    }
    return data->table[0].entry;
}


void ShadComInit(void) {
    struct nutex_s tex;
    s32 i;

    for (i = 0; i < 4; i++) {
        tex.width = 1;
        tex.pal = shadpals[i];
        tex.height = 1;
        tex.bits = 0;
        tex.type = 4;
        tex.mmcnt = 0;
        shadtids[i] = NuTexCreate(&tex);
    }
}
