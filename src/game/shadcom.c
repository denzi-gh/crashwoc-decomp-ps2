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

#include "nu_libc.h"

#include "nu3d/numtl.h"
#include "nu3d/nustream.h"
#include "nu3d/nutex.h"
#include "nu3d/nugobj.h"
#include "nucore/nucoretypes.h"

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

extern struct numtl_s *numtl_defaultmtl3d;
extern u8 *rndrstream_free;

static s32 palids[4];

extern s32 NuFileSize(char *name);
extern s32 NuFileLoadBuffer(char *name, void *buf, s32 max);
extern void *NuMemAllocFn(s32 size, char *file, s32 line);
extern void NuMemFreeFn(void *ptr, char *file, s32 line);
extern void NuRndrStreamLink(struct rndrstream_s *rs);
extern s32 NuRndrStreamTriStrip3dClip(struct nuvtx_tc1_s *vtx, s32 num, struct numtx_s *mtx,
                                      struct rndrstream_s *rs);
extern u8 *vpDmaTag_CntEx(u8 *cur, s32 qwc, u32 tag);
extern u8 *vpDmaTag_Next(u8 *cur, s32 mode);

u32 shadcol = 0x7FFFFFFF;

static u32 pal0[16] = {
    0x00000000, 0x20FFFFFF, 0x00000000, 0x20FFFFFF,
    0x00000000, 0x20FFFFFF, 0x00000000, 0x20FFFFFF,
    0x00000000, 0x20FFFFFF, 0x00000000, 0x20FFFFFF,
    0x00000000, 0x20FFFFFF, 0x00000000, 0x20FFFFFF,
};
static u32 pal1[16] = {
    0x00000000, 0x00000000, 0x20FFFFFF, 0x20FFFFFF,
    0x00000000, 0x00000000, 0x20FFFFFF, 0x20FFFFFF,
    0x00000000, 0x00000000, 0x20FFFFFF, 0x20FFFFFF,
    0x00000000, 0x00000000, 0x20FFFFFF, 0x20FFFFFF,
};
static u32 pal2[16] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x20FFFFFF, 0x20FFFFFF, 0x20FFFFFF, 0x20FFFFFF,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x20FFFFFF, 0x20FFFFFF, 0x20FFFFFF, 0x20FFFFFF,
};
static u32 pal3[16] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x20FFFFFF, 0x20FFFFFF, 0x20FFFFFF, 0x20FFFFFF,
    0x20FFFFFF, 0x20FFFFFF, 0x20FFFFFF, 0x20FFFFFF,
};
static u32 *pals[4] = { pal0, pal1, pal2, pal3 };


void ShadRndr(struct numtx_s *mtx, struct shadentry_s *shad, f32 frame, f32 scale) {
    struct numtx_s m;
    struct numtl_s *mtl;
    struct rndrstream_s *rs;
    s32 tid;
    s32 last;
    s32 n;
    s32 pal;
    s32 col;
    s32 row;
    f32 du;
    f32 dv;
    u32 colour;
    static struct nuvtx_tc1_s vtx[4] = {
        { { -1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0x7FFFFFFF }, { 0.0f, 0.0f } },
        { { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0x7FFFFFFF }, { 1.0f, 0.0f } },
        { { -1.0f, 0.0f, -1.0f }, { 0.0f, 0.0f, 0.0f }, { 0x7FFFFFFF }, { 0.0f, 1.0f } },
        { { 1.0f, 0.0f, -1.0f }, { 0.0f, 0.0f, 0.0f }, { 0x7FFFFFFF }, { 1.0f, 1.0f } },
    };

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
    vtx[3].diffuse = colour;
    vtx[2].diffuse = colour;
    vtx[1].diffuse = colour;
    vtx[0].diffuse = colour;
    vtx[0].tc[0] = col * du;
    vtx[0].tc[1] = row * dv;
    vtx[1].tc[0] = (col + 1) * du;
    vtx[1].tc[1] = row * dv;
    vtx[2].tc[0] = col * du;
    vtx[2].tc[1] = (row + 1) * dv;
    vtx[3].tc[0] = (col + 1) * du;
    vtx[3].tc[1] = (row + 1) * dv;
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
    tid = palids[pal];
    if (tid != 0) {
        NuRndrStreamLink(rs);
        rs->mtl = 0;
        rs->tid = 0;
        NuRndrStreamAddMtl(rs, mtl);
        NuTexAccomodateRS(rs, tid);
        rs->ptr.u8 = NuTexSetPal(rs->ptr.u8, mtl->tid, tid, 0);
        rndrstream_free = vpDmaTag_Next(rs->ptr.u8, 0);
    }
    NuRndrStreamTriStrip3dClip(vtx, 4, &m, rs);
    NuRndrStreamLink(rs);
    rs->ptr.u8 = vpDmaTag_CntEx(rs->ptr.u8, 0, 0x13000000);
    rndrstream_free = vpDmaTag_Next(rs->ptr.u8, 0);
}


struct shaddata_s *ShadDataLoad(char *file, union variptr_u *buffer) {
    struct shaddata_s *shad;
    struct shaddata_s *data;
    struct shadentry_s *entry;
    struct numtl_s *mtls;
    union variptr_u local;
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
            local.voidptr = NuMemAllocFn(size + 0x100, ".\\shadcom.c", 0x12B);
        }
        buffer->intaddr = (buffer->intaddr + 0xF) & 0xFFFFFFF0;
        size = NuFileLoadBuffer(file, buffer->voidptr, 0x7FFFFFFF);
        data = buffer->voidptr;
        buffer->u8 = buffer->u8 + size;
        shad = data;
        data->external = external;
        if (external != 0) {
            buffer->intaddr = (buffer->intaddr + 0xF) & 0xFFFFFFF0;
            mtls = buffer->voidptr;
            memset(mtls, 0, data->count * sizeof(struct numtl_s));
            buffer->u8 = buffer->u8 + data->count * sizeof(struct numtl_s);
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
                entry->mtl->sort_pri = 100;
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
        NuMemFreeFn(data, ".\\shadcom.c", 0x102);
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
        tex.pal = pals[i];
        tex.height = 1;
        tex.bits = 0;
        tex.type = 4;
        tex.mmcnt = 0;
        palids[i] = NuTexCreate(&tex);
    }
}
