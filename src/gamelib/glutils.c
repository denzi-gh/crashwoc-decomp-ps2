/*
 * Unit: gamelib/glutils
 *
 * Functions:
 *   0x001a0ba0 CreateAlphaBlendTexture256_32
 *   0x001a0d48 CreateAlphaBlendTexture64
 *   0x001a0ec0 CreateSubtractiveTexture
 *   0x001a0fd8 CreateAlphaBlendTexturewh_32
 */

#include "creature.h"

struct numtlattrib_s {
    u64 unk0 : 32;
    u64 alpha : 4;
    u64 filter : 2;
    u64 unk38 : 2;
    u64 utc : 2;
    u64 vtc : 2;
    u64 cull : 2;
    u64 zmode : 2;
    u64 unk48 : 16;
};

struct numtlattrib2_s {
    u64 fx : 2;
    u64 blend : 1;
    u64 unk3 : 1;
    u64 dither : 1;
    u64 unk5 : 59;
};

struct numtl_s {
    u8 pad000[0x168];
    struct numtlattrib_s attrib;
    struct numtlattrib2_s attrib2;
    u8 pad178[8];
    struct nucolour3_s diffuse;
    u8 pad18C[0x14];
    f32 alpha;
    s32 tid;
    s32 alpha_sort;
};

struct nutex_s {
    s32 type;
    s32 width;
    s32 height;
    s32 mmcnt;
    void *bits;
    void *pal;
    s32 decal;
    s32 linear;
};

extern char D_00619210[];
#define glutils_file D_00619210

s32 NuFileLoadBuffer(char *name, void *buf, s32 max);
struct nutex_s *NuTexReadBitmap(char *name);
s32 NuTexCreate(struct nutex_s *tex);
struct numtl_s *NuMtlCreate(s32 count);
void NuMtlUpdate(struct numtl_s *mtl);
void NuMemFreeFn(void *ptr, char *file, s32 line);


struct numtl_s *CreateAlphaBlendTexture256_32(char *file, s32 blend, s32 alphamode, s32 alpha_sort, s32 zmode) {
    struct nutex_s tex;
    char pixels[0x40000];
    struct numtl_s *mtl;
    s32 i;
    char r, g, b, a;

    NuFileLoadBuffer(file, pixels, 0x40000);
    for (i = 0; i < 0x40000; i += 4) {
        r = pixels[i];
        g = pixels[i + 1];
        b = pixels[i + 2];
        a = pixels[i + 3];
        pixels[i] = a;
        pixels[i + 1] = b;
        pixels[i + 2] = g;
        pixels[i + 3] = r;
    }
    tex.type = 3;
    tex.width = 256;
    tex.height = 256;
    tex.mmcnt = 1;
    tex.bits = pixels;
    mtl = NuMtlCreate(1);
    mtl->diffuse.r = 1.0f;
    mtl->diffuse.g = 1.0f;
    mtl->diffuse.b = 1.0f;
    mtl->alpha = 0.999f;
    mtl->attrib.cull = 2;
    mtl->attrib.zmode = zmode;
    mtl->attrib.alpha = alphamode;
    if (blend != 0) {
        mtl->attrib2.blend = 1;
    }
    mtl->tid = NuTexCreate(&tex);
    mtl->alpha_sort = alpha_sort;
    NuMtlUpdate(mtl);
    return mtl;
}

struct numtl_s *CreateAlphaBlendTexture64(char *file, s32 blend, s32 alphamode, s32 alpha_sort) {
    struct nutex_s tex;
    char pixels[0x4000];
    struct numtl_s *mtl;
    s32 i;
    char r, g, b, a;

    NuFileLoadBuffer(file, pixels, 0x4000);
    for (i = 0; i < 0x4000; i += 4) {
        r = pixels[i];
        g = pixels[i + 1];
        b = pixels[i + 2];
        a = pixels[i + 3];
        pixels[i] = a;
        pixels[i + 1] = b;
        pixels[i + 2] = g;
        pixels[i + 3] = r;
    }
    tex.type = 3;
    tex.width = 64;
    tex.height = 64;
    tex.mmcnt = 0;
    tex.bits = pixels;
    mtl = NuMtlCreate(1);
    mtl->diffuse.r = 1.0f;
    mtl->diffuse.g = 1.0f;
    mtl->diffuse.b = 1.0f;
    mtl->alpha = 0.999f;
    mtl->attrib.cull = 2;
    mtl->attrib.zmode = 1;
    mtl->attrib.alpha = alphamode;
    if (blend != 0) {
        mtl->attrib2.blend = 1;
    }
    mtl->tid = NuTexCreate(&tex);
    mtl->alpha_sort = alpha_sort;
    NuMtlUpdate(mtl);
    return mtl;
}


struct numtl_s *CreateSubtractiveTexture(char *file) {
    struct nutex_s *tex;
    struct numtl_s *mtl;

    tex = NuTexReadBitmap(file);
    mtl = NuMtlCreate(1);
    mtl->diffuse.r = 1.0f;
    mtl->diffuse.g = 1.0f;
    mtl->diffuse.b = 1.0f;
    mtl->alpha = 0.999f;
    mtl->attrib.cull = 2;
    mtl->attrib.zmode = 1;
    mtl->attrib.alpha = 3;
    if (tex != 0) {
        mtl->tid = NuTexCreate(tex);
        NuMemFreeFn(tex->bits, glutils_file, 0x2C);
        NuMemFreeFn(tex, glutils_file, 0x2D);
    }
    NuMtlUpdate(mtl);
    return mtl;
}


struct numtl_s *CreateAlphaBlendTexturewh_32(char *file, s32 blend, s32 alphamode, s32 alpha_sort, s32 zmode,
                                             s32 width, s32 height) {
    struct nutex_s tex;
    char pixels[0x40000];
    struct numtl_s *mtl;
    s32 i;
    char r, g, b, a;

    NuFileLoadBuffer(file, pixels, 0x40000);
    for (i = 0; i < width * height * 4; i += 4) {
        r = pixels[i];
        g = pixels[i + 1];
        b = pixels[i + 2];
        a = pixels[i + 3];
        pixels[i] = a;
        pixels[i + 1] = b;
        pixels[i + 2] = g;
        pixels[i + 3] = r;
    }
    tex.type = 3;
    tex.width = width;
    tex.height = height;
    tex.mmcnt = 1;
    tex.bits = pixels;
    mtl = NuMtlCreate(1);
    mtl->diffuse.r = 1.0f;
    mtl->diffuse.g = 1.0f;
    mtl->diffuse.b = 1.0f;
    mtl->alpha = 0.999f;
    mtl->attrib.cull = 2;
    mtl->attrib.zmode = zmode;
    mtl->attrib.alpha = alphamode;
    if (blend != 0) {
        mtl->attrib2.blend = 1;
    }
    mtl->tid = NuTexCreate(&tex);
    mtl->alpha_sort = alpha_sort;
    NuMtlUpdate(mtl);
    return mtl;
}
