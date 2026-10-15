/*
 * Unit: gamelib/glutils
 *
 * Functions:
 *   0x001a0ba0 CreateAlphaBlendTexture256_32
 *   0x001a0d48 CreateAlphaBlendTexture64
 *   0x001a0ec0 CreateSubtractiveTexture
 *   0x001a0fd8 CreateAlphaBlendTexturewh_32
 */

#include "nu3d/numtl.h"
#include "nu3d/nutex.h"

extern char D_00619210[];
#define glutils_file D_00619210

s32 NuFileLoadBuffer(char *name, void *buf, s32 max);
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
        mtl->attrib2.uvmode = 1;
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
        mtl->attrib2.uvmode = 1;
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
        mtl->attrib2.uvmode = 1;
    }
    mtl->tid = NuTexCreate(&tex);
    mtl->alpha_sort = alpha_sort;
    NuMtlUpdate(mtl);
    return mtl;
}
