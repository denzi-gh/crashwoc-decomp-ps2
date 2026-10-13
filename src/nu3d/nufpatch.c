/*
 * Unit: nu3d/nufpatch
 *
 * Functions:
 *   0x0015b378 mtlSetDefaults
 *   0x0015b510 NuFPatchMakeMaterial
 *   0x0015b6b8 initCamera
 *   0x0015ba00 NuRndrFPatchBegin
 *   0x0015bca8 NuRndrFPatchInit
 *   0x0015caa8 NuRndrFPatchCameraUpdate
 *   0x0015cc28 NuRndrFPatchHeightField
 *   0x0015d068 NuFPatchHeightFieldIndexSlow
 *   0x0015d510 NuFPatchHeightFieldIndex
 *   0x0015dfd0 NuMtxBilinear
 *   0x0015e2b0 NuFPatchSplit
 *   0x0015e8e8 NuFPatchTesselateGrid
 *   0x0015ec18 NuFPatchTesselateSphereRec
 *   0x0015eef8 NuFPatchUpdateLight
 *   0x0015f0e0 NuFPatchGenerateWorld
 *   0x001606d0 NuFPatchShadowCast
 *   0x00160a20 yy
 *   0x00160bc0 NuFPatchMakeHeightFieldBuffer
 *   0x00162258 NuFPatchHeightFieldIndexFast
 *   0x00162510 NuFPatchGetStream
 *   0x00162518 NuRndrFPatchEnd
 *   0x00162598 NuRndrFPatch
 *   0x001625b8 NuRndrFPatchN
 *   0x00162628 NuFPatchAnimateWaterTable
 *   0x00162728 NuFPatchTesselateSphere
 *   0x00162750 NuFPatchLightTexture
 *   0x00162968 NuFPatchSetTidsN
 *   0x00162998 NuRndrFPatchHeightFieldIndex
 *   0x00162a08 NuFPatchLightPoint
 *   0x00162ae0 testShadowCast
 */

typedef unsigned char u8;
typedef int s32;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef float f32;

struct nuvec_s {
    f32 x;
    f32 y;
    f32 z;
};

struct nuvec4_s {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
};

struct nucolour3_s {
    f32 r;
    f32 g;
    f32 b;
};

struct numtlattrib_s {
    u32 alpha : 4;
    u32 filter : 2;
    u32 fx : 2;
    u32 utc : 2;
    u32 vtc : 2;
    u32 cull : 2;
    u32 zmode : 2;
    u32 lighting : 2;
    u32 colour : 1;
    u32 fill : 1;
    u32 atst : 3;
    u32 aref : 8;
    u32 afail : 2;
    u32 uvmode : 1;
};

struct numtl_s {
    u64 pad000[0x2D];
    u32 pad168;
    struct numtlattrib_s attrib;
    struct nucolour3_s ambient;
    struct nucolour3_s diffuse;
    u8 pad18C[0x14];
    f32 alpha;
    u8 pad1A4[0x20];
    u8 fx_flags;
};

struct nurndrstream_s {
    u8 *start;
    u8 *cur;
};

struct numtx_s {
    f32 _00, _01, _02, _03;
    f32 _10, _11, _12, _13;
    f32 _20, _21, _22, _23;
    f32 _30, _31, _32, _33;
};

struct nuhfvtx_s {
    struct nuvec4_s pnt;
    struct nuvec4_s nrm;
    u8 pad20[0xC];
    f32 light;
};

struct nufpatch_s {
    struct numtx_s uv;
    struct nuvec4_s pnt[4];
    struct numtx_s col;
    struct nuvec4_s nrm[4];
    s32 tid[2];
    struct numtl_s *mtl[2];
};

#define FPATCH_STREAMS 1

extern struct nurndrstream_s *D_0062EFD0[FPATCH_STREAMS];
#define fpatch_stream D_0062EFD0

extern struct nuvec4_s *D_0062EFC4;
#define water_wave_src D_0062EFC4

extern s32 D_0062EFC8;
#define water_table_ready D_0062EFC8

extern struct nuvec4_s D_006AFF68[0x100];
#define water_table D_006AFF68

extern struct nuhfvtx_s D_006B0F80[];
#define hf_vtx D_006B0F80

extern char D_00281530[];
#define fpatch_microcode D_00281530

extern u8 *rndrstream_free;
extern s32 fast_index;

extern void NuRndrStreamLink(struct nurndrstream_s *rs);
extern u8 *vpDmaTag_Call(u8 *cur, void *microcode);
extern u8 *vpDmaTag_Next(u8 *cur, s32 mode);
extern void NuTexUnReserve(void);
extern void NuVecNorm(struct nuvec_s *out, struct nuvec_s *in);
extern f32 NuVecDot(struct nuvec_s *a, struct nuvec_s *b);
extern double sin(double x);

extern void NuRndrFPatchHeightField(struct nufpatch_s *patch, s32 lit, s32 flags);
extern void NuFPatchHeightFieldIndexSlow(struct nufpatch_s *src, f32 *heights,
                                         s32 index, struct nufpatch_s *dst);
extern void NuFPatchTesselateSphereRec(s32 depth, struct nufpatch_s *patch,
                                       s32 flags, f32 radius, f32 scale);


void mtlSetDefaults(struct numtl_s *mtl)
{
    mtl->attrib.utc = 1;
    mtl->attrib.vtc = 1;
    mtl->attrib.cull = 2;
    mtl->attrib.zmode = 0;
    mtl->attrib.lighting = 2;
    mtl->attrib.alpha = 0;
    mtl->attrib.colour = 0;
    mtl->attrib.filter = 2;
    mtl->attrib.atst = 0;
    mtl->ambient.r = 0.0f;
    mtl->attrib.uvmode = 0;
    mtl->attrib.afail = 3;
    mtl->attrib.aref = 0;
    mtl->attrib.fill = 1;
    mtl->fx_flags = 0;
    mtl->diffuse.r = 1.0f;
    mtl->diffuse.g = 1.0f;
    mtl->diffuse.b = 1.0f;
    mtl->ambient.g = 0.0f;
    mtl->ambient.b = 0.0f;
    mtl->alpha = 1.0f;
}


void NuFPatchHeightFieldIndexFast(struct nufpatch_s *src, f32 *heights, s32 index,
                                  struct nufpatch_s *dst)
{
    s32 vtx;

    vtx = index / 8 + index;
    dst->tid[0] = src->tid[0];
    dst->tid[1] = src->tid[1];
    dst->mtl[0] = src->mtl[0];
    dst->mtl[1] = src->mtl[1];
    dst->col = src->col;
    dst->pnt[0] = hf_vtx[vtx].pnt;
    dst->pnt[1] = hf_vtx[vtx + 1].pnt;
    dst->pnt[2] = hf_vtx[vtx + 9].pnt;
    dst->pnt[3] = hf_vtx[vtx + 10].pnt;
    dst->nrm[0] = hf_vtx[vtx].nrm;
    dst->nrm[1] = hf_vtx[vtx + 1].nrm;
    dst->nrm[2] = hf_vtx[vtx + 9].nrm;
    dst->nrm[3] = hf_vtx[vtx + 10].nrm;
    dst->col._03 = hf_vtx[vtx].light;
    dst->col._13 = hf_vtx[vtx + 1].light;
    dst->col._23 = hf_vtx[vtx + 9].light;
    dst->col._33 = hf_vtx[vtx + 10].light;
    dst->uv._01 = heights[vtx];
    dst->uv._03 = heights[vtx + 1];
    dst->uv._00 = heights[vtx + 81];
    dst->uv._02 = heights[vtx + 82];
    dst->uv._10 = 0.0f;
    dst->uv._11 = heights[vtx + 162];
    dst->uv._12 = 0.0f;
    dst->uv._13 = heights[vtx + 163];
    dst->uv._20 = 0.0f;
    dst->uv._21 = heights[vtx + 171];
    dst->uv._22 = 0.0f;
    dst->uv._23 = heights[vtx + 172];
    dst->uv._31 = heights[vtx + 9];
    dst->uv._33 = heights[vtx + 10];
    dst->uv._30 = heights[vtx + 90];
    dst->uv._32 = heights[vtx + 91];
}


struct nurndrstream_s *NuFPatchGetStream(void)
{
    return fpatch_stream[0];
}


void NuRndrFPatchEnd(void)
{
    s32 i;

    for (i = 0; i < FPATCH_STREAMS; i++) {
        NuRndrStreamLink(fpatch_stream[i]);
        fpatch_stream[i]->cur = vpDmaTag_Call(fpatch_stream[i]->cur, fpatch_microcode);
        rndrstream_free = vpDmaTag_Next(fpatch_stream[i]->cur, 0);
    }
    NuTexUnReserve();
}


void NuRndrFPatch(struct nufpatch_s *patch, s32 flags)
{
    NuRndrFPatchHeightField(patch, 0, flags);
}


void NuRndrFPatchN(struct nufpatch_s *patch, s32 count, s32 flags)
{
    s32 i;

    i = 0;
    if (count > 0) {
        do {
            NuRndrFPatchHeightField(&patch[i], 0, flags);
            i++;
        } while (i < count);
    }
}


void NuFPatchAnimateWaterTable(f32 time)
{
    s32 i;
    double amplitude;

    for (i = 0; i < 0x100; i++) {
        amplitude = water_wave_src[i].z * 0.5;
        water_table[i].z = amplitude * sin(time * (water_wave_src[i].z * 0.3f + 1.0f)
                                           + water_wave_src[i].z * 3.142f);
    }
    water_table_ready = 1;
}


void NuFPatchTesselateSphere(s32 depth, struct nufpatch_s *patch, s32 flags, f32 radius)
{
    NuFPatchTesselateSphereRec(depth, patch, flags, radius, 1.0f);
}


void NuFPatchSetTidsN(struct nufpatch_s *patch, s32 count, s32 tid0, s32 tid1,
                      struct numtl_s *mtl0, struct numtl_s *mtl1)
{
    s32 i;

    for (i = 0; i < count; i++) {
        patch->tid[0] = tid0;
        patch->tid[1] = tid1;
        patch->mtl[0] = mtl0;
        patch->mtl[1] = mtl1;
        patch++;
    }
}


void NuRndrFPatchHeightFieldIndex(struct nufpatch_s *src, f32 *heights, s32 index,
                                  s32 tid0, s32 tid1, s32 flags)
{
    struct nufpatch_s patch;

    if (fast_index) {
        NuFPatchHeightFieldIndexFast(src, heights, index, &patch);
    } else {
        NuFPatchHeightFieldIndexSlow(src, heights, index, &patch);
    }
    patch.tid[0] = tid0;
    patch.tid[1] = tid1;
    NuRndrFPatchHeightField(&patch, 0, flags);
}


f32 NuFPatchLightPoint(struct nuvec_s *normal, f32 scale, f32 bias, f32 dx, f32 dy)
{
    struct nuvec_s dir;
    f32 dot;
    f32 light;

    dir.x = -dx;
    dir.y = -dy;
    dir.z = 1.0f;
    NuVecNorm(&dir, &dir);
    dot = NuVecDot(&dir, normal);
    if (dot < 0.0f) {
        dot = 0.0f;
    }
    light = dot * scale + bias;
    if (light > 255.0) {
        light = 255.0f;
    }
    if (light < 0.0) {
        light = 0.0f;
    }
    return light;
}
