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
typedef short s16;
typedef unsigned short u16;
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
    s32 tid;
    s32 pad1A8;
    s16 alpha_sort;
    u8 alpha_layer;
    u8 pad1AF[0x15];
    u8 fx_flags;
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

struct nurndrstream_s {
    u8 *start;
    u8 *cur;
    u8 *txtr_start;
    u8 *txtr_end;
    void *zbuf;
    struct numtl_s *mtl;
};

struct numtx_s {
    f32 _00, _01, _02, _03;
    f32 _10, _11, _12, _13;
    f32 _20, _21, _22, _23;
    f32 _30, _31, _32, _33;
};

struct nucamera_s {
    struct numtx_s mtx;
    f32 fov;
    f32 aspect;
    f32 nearclip;
    f32 farclip;
};

struct nuhfvtx_s {
    struct nuvec4_s pnt;
    struct nuvec4_s nrm;
    u8 pad20[0xC];
    f32 light;
};

struct fpatchstat_s {
    f32 total;
    f32 peak;
    f32 average;
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

extern char D_00615BD8[];
#define fpatch_file D_00615BD8

extern char D_0028AD70[];
#define fpatch_mpgtag D_0028AD70

extern struct nutex_s D_006B0F68;
#define fpatch_lighttex D_006B0F68

extern s32 D_006331A0;
#define fpatch_lighttid D_006331A0

extern s32 D_006331A4;
#define fpatch_texaddr D_006331A4

extern f32 D_0062CBAC;
#define fpatch_stat_blend D_0062CBAC

extern f32 D_0062CBB0;
#define water_wave_height D_0062CBB0

extern struct fpatchstat_s D_002E9598[3];
#define fpatch_stats D_002E9598

extern s32 nutex_force_overbrighten;

extern f32 D_0062CBC8;
#define water_split_height D_0062CBC8

extern struct numtx_s D_002E95C0;
#define fpatch_split_left D_002E95C0

extern struct numtx_s D_002E9600;
#define fpatch_split_right D_002E9600

extern struct numtx_s D_002E9640;
#define fpatch_split_top D_002E9640

extern struct numtx_s D_002E9680;
#define fpatch_split_bottom D_002E9680

extern f32 D_0062EFDC;
#define fpatch_half D_0062EFDC

extern u8 *rndrstream_free;
extern s32 fast_index;

extern void *NuMemAllocFn(s32 size, char *file, s32 line);
extern s32 NuTexCreate(struct nutex_s *tex);
extern struct numtl_s *NuMtlCreate(s32 count);
extern void NuMtlUpdate(struct numtl_s *mtl);
extern struct nurndrstream_s *NuRndrStreamAllocStream(u32 sortkey, void *mpgtag, u32 flags);
extern void NuFPatchLightTexture(struct nutex_s *tex, s32 size, struct nuvec_s *lightdir,
                                 f32 range, f32 scale, f32 bias);
extern void NuRandSeed(s32 seed);
extern f32 NuRandFloat(void);
extern s32 NuTexReserve(s32 size);
extern s32 NuTexGetPointers(s32 tid, void **entry, void **ps2tex);
extern u8 *NuPs2TxGenUpload(u8 *cur, void *ps2tex, s32 addr);
extern u8 *vpDmaTag_Cnt(u8 *cur);
extern u8 *vpDmaTag_Close(u8 *cur);
extern u8 *vpDmaTag_CntEx(u8 *cur, u32 vif0, u32 vif1);
extern u8 *vpDmaTag_RefEx(u8 *cur, void *addr, u32 vif0, u32 vif1);
extern s32 NuRndrStreamAddMtl(struct nurndrstream_s *rs, struct numtl_s *mtl);
extern struct numtx_s *NuCameraGetVPCSMtx(void);
extern void NuMtxBilinear(struct nuvec4_s *corner, struct nuvec4_s *quad0, struct nuvec4_s *quad1,
                          struct nuvec4_s *quad2, struct nuvec4_s *quad3);
extern void NuVec4LerpVU0(struct nuvec4_s *out, struct nuvec4_s *a, struct nuvec4_s *b, f32 *t);
extern void NuRndrStreamLink(struct nurndrstream_s *rs);
extern u8 *vpDmaTag_Call(u8 *cur, void *microcode);
extern u8 *vpDmaTag_Next(u8 *cur, s32 mode);
extern void NuTexUnReserve(void);
extern void NuVecNorm(struct nuvec_s *out, struct nuvec_s *in);
extern f32 NuVecDot(struct nuvec_s *a, struct nuvec_s *b);
extern void NuVecScale(struct nuvec_s *out, struct nuvec_s *in, f32 scale);
extern double sin(double x);
extern double cos(double x);
extern double tan(double x);
extern struct nucamera_s *NuCameraGetCam(void);
extern struct numtx_s *NuCameraGetViewMtx(void);
extern struct numtx_s *NuCameraGetScalingMtx(void);
extern struct numtx_s *NuCameraGetProjectionMtx(void);
extern void NuMtxMulH(struct numtx_s *dst, struct numtx_s *a, struct numtx_s *b);
extern void NuMtxInvH(struct numtx_s *inv, struct numtx_s *mtx);

extern void NuRndrFPatchHeightField(struct nufpatch_s *patch, void *heightbuf, s32 flags);
extern void NuFPatchHeightFieldIndexSlow(struct nufpatch_s *src, f32 *heights,
                                         s32 index, struct nufpatch_s *dst);
extern void NuFPatchSplit(struct nufpatch_s *patch, struct nufpatch_s *children, f32 amount);
extern void NuFPatchTesselateSphereRec(s32 depth, struct nufpatch_s *patch, f32 radius,
                                       f32 scale, struct nufpatch_s **out);


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


void NuFPatchMakeMaterial(struct numtl_s **height_mtl, struct numtl_s **light_mtl)
{
    struct nutex_s tex;
    s32 height_tid;
    s32 light_tid;
    struct numtl_s *mtl;
    struct numtl_s *mtl2;

    tex.type = 2;
    tex.width = 0x40;
    tex.height = 0x40;
    tex.mmcnt = 7;
    tex.bits = NuMemAllocFn(0x8000, fpatch_file, 0x124);
    tex.pal = NuMemAllocFn(0x400, fpatch_file, 0x125);
    height_tid = NuTexCreate(&tex);
    tex.type = 4;
    tex.width = 0x100;
    tex.height = 0x100;
    tex.mmcnt = 0;
    tex.bits = NuMemAllocFn(0x80000, fpatch_file, 0x124);
    tex.pal = NuMemAllocFn(0x400, fpatch_file, 0x125);
    light_tid = NuTexCreate(&tex);
    mtl = NuMtlCreate(1);
    mtlSetDefaults(mtl);
    mtl->attrib.alpha = 0;
    mtl->tid = height_tid;
    mtl->alpha_sort = -175;
    mtl->alpha_layer = 1;
    mtl->fx_flags = 0;
    mtl2 = NuMtlCreate(1);
    mtlSetDefaults(mtl2);
    mtl2->tid = light_tid;
    mtl->alpha_sort = -175;
    mtl->alpha_layer = 1;
    mtl2->fx_flags = 1;
    mtl2->attrib.alpha = 6;
    NuMtlUpdate(mtl);
    NuMtlUpdate(mtl2);
    *height_mtl = mtl;
    *light_mtl = mtl2;
}


void initCamera(struct numtx_s *frustum, struct numtx_s *inv_clip, struct numtx_s *vpc,
                f32 *nearclip, f32 *farclip)
{
    struct nuvec_s left;
    struct nuvec_s right;
    struct nuvec_s top;
    struct nuvec_s bottom;
    struct numtx_s clip;
    struct numtx_s scale;
    struct nucamera_s *cam;
    struct numtx_s *view;
    struct numtx_s *scaling;
    struct numtx_s *proj;
    f32 fov;
    f32 half;
    f32 slope;
    f32 vslope;

    half = 0.5f;
    cam = NuCameraGetCam();
    view = NuCameraGetViewMtx();
    scaling = NuCameraGetScalingMtx();
    proj = NuCameraGetProjectionMtx();
    scale = *scaling;
    scale._00 = 5120.0f;
    scale._30 = 32768.0f;
    scale._11 = -1792.0f;
    scale._31 = 32768.0f;
    NuMtxMulH(&clip, proj, &scale);
    NuMtxMulH(vpc, view, &clip);
    NuMtxInvH(inv_clip, &clip);
    fov = cam->fov;
    half = fov * half;
    slope = (f32)tan(half) / cam->aspect;
    vslope = tan(half);
    right.x = -1.0f;
    right.y = 0.0f;
    right.z = slope;
    NuVecNorm(&right, &right);
    left.x = 1.0f;
    left.y = 0.0f;
    left.z = slope;
    NuVecNorm(&left, &left);
    top.x = 0.0f;
    top.y = cos(fov * 0.5);
    top.z = sin(fov * 0.5);
    bottom.x = 0.0f;
    bottom.y = -cos(fov * 0.5);
    bottom.z = sin(fov * 0.5);
    frustum->_00 = right.x;
    frustum->_10 = right.y;
    frustum->_20 = right.z;
    frustum->_01 = left.x;
    frustum->_11 = left.y;
    frustum->_21 = left.z;
    frustum->_02 = top.x;
    frustum->_12 = top.y;
    frustum->_22 = top.z;
    frustum->_03 = bottom.x;
    frustum->_13 = bottom.y;
    frustum->_23 = bottom.z;
    frustum->_30 = 1.0f;
    frustum->_31 = 1.0f;
    frustum->_32 = 1.0f;
    frustum->_33 = 1.0f;
    *nearclip = cam->nearclip;
    *farclip = cam->farclip;
}


void NuRndrFPatchBegin(struct nuvec_s *lightdir, f32 scale, f32 bias)
{
    s32 size;
    f32 blend;
    f32 height;
    struct nuvec4_s *waves;
    struct nuvec4_s *wave;
    void *entry;
    void *ps2tex;
    s32 i;

    size = 32;
    blend = fpatch_stat_blend;
    if (fpatch_stream[0] == 0) {
        fpatch_stream[0] = NuRndrStreamAllocStream(0x7FFFFFFD, fpatch_mpgtag, 0xFFFFFFFF);
        NuFPatchLightTexture(&fpatch_lighttex, size, lightdir, 4.0f, scale, bias);
        nutex_force_overbrighten = 1;
        fpatch_lighttid = NuTexCreate(&fpatch_lighttex);
        nutex_force_overbrighten = 0;
        if (water_wave_src == 0) {
            waves = NuMemAllocFn(0x1000, fpatch_file, 0xEA);
            NuRandSeed(0x11);
            height = water_wave_height;
            wave = waves;
            for (i = 0; i < 0x100; i++) {
                wave->x = (NuRandFloat() - 0.5f) * 0.0f;
                wave->y = (NuRandFloat() - 0.5f) * 0.0f;
                wave->z = (NuRandFloat() - 0.5f) * height;
                wave->w = (NuRandFloat() - 0.5f) * 0.0f;
                wave->y = wave->z;
                wave++;
            }
            water_wave_src = waves;
        }
    }
    fpatch_texaddr = NuTexReserve(size * size * 4);
    for (i = 0; i < FPATCH_STREAMS; i++) {
        NuRndrStreamLink(fpatch_stream[i]);
        NuTexGetPointers(fpatch_lighttid, &entry, &ps2tex);
        fpatch_stream[i]->cur = NuPs2TxGenUpload(fpatch_stream[i]->cur, ps2tex, fpatch_texaddr);
        rndrstream_free = vpDmaTag_Next(fpatch_stream[i]->cur, 0);
    }
    if (fpatch_stats[0].total > fpatch_stats[0].peak) {
        fpatch_stats[0].peak = fpatch_stats[0].total;
    }
    if (fpatch_stats[1].total > fpatch_stats[1].peak) {
        fpatch_stats[1].peak = fpatch_stats[1].total;
    }
    if (fpatch_stats[2].total > fpatch_stats[2].peak) {
        fpatch_stats[2].peak = fpatch_stats[2].total;
    }
    fpatch_stats[0].average = fpatch_stats[0].average * blend
                              + fpatch_stats[0].total * (1.0f - blend);
    fpatch_stats[1].average = fpatch_stats[1].average * blend
                              + fpatch_stats[1].total * (1.0f - blend);
    fpatch_stats[2].average = fpatch_stats[2].average * blend
                              + fpatch_stats[2].total * (1.0f - blend);
    fpatch_stats[0].total = 0.0f;
    fpatch_stats[2].total = 0.0f;
    fpatch_stats[1].total = 0.0f;
}


void NuRndrFPatchCameraUpdate(void)
{
    u32 i;

    for (i = 0; i < FPATCH_STREAMS; i++) {
        NuRndrStreamLink(fpatch_stream[i]);
        fpatch_stream[i]->cur = vpDmaTag_Cnt(fpatch_stream[i]->cur);
        *((u32 *)fpatch_stream[i]->cur)++ = 0;
        *((u32 *)fpatch_stream[i]->cur)++ = 0;
        *((u32 *)fpatch_stream[i]->cur)++ = 0x10000000;
        *((u32 *)fpatch_stream[i]->cur)++ = 0x6C0402CC;
        *((struct numtx_s *)fpatch_stream[i]->cur)++ = *NuCameraGetVPCSMtx();
        fpatch_stream[i]->cur = vpDmaTag_Close(fpatch_stream[i]->cur);
        rndrstream_free = vpDmaTag_Next(fpatch_stream[i]->cur, 0);
    }
}

void NuRndrFPatchHeightField(struct nufpatch_s *patch, void *heightbuf, s32 flags)
{
    NuRndrStreamLink(fpatch_stream[0]);
    fpatch_stream[0]->mtl = 0;
    patch->mtl[0]->tid = patch->tid[0];
    NuRndrStreamAddMtl(fpatch_stream[0], patch->mtl[0]);
    if (flags & 8) {
        patch->mtl[1]->tid = patch->tid[1];
        NuRndrStreamAddMtl(fpatch_stream[0], patch->mtl[1]);
    }
    fpatch_stream[0]->cur = vpDmaTag_Cnt(fpatch_stream[0]->cur);
    *((u32 *)fpatch_stream[0]->cur)++ = 0x10000000;
    *((u32 *)fpatch_stream[0]->cur)++ = 0x6C100001;
    *((struct numtx_s *)fpatch_stream[0]->cur)++ = patch->uv;
    *((struct nuvec4_s *)fpatch_stream[0]->cur)++ = patch->nrm[0];
    ((u32 *)fpatch_stream[0]->cur)[-1] = (heightbuf != 0);
    *((struct nuvec4_s *)fpatch_stream[0]->cur)++ = patch->nrm[1];
    ((u32 *)fpatch_stream[0]->cur)[-1] = flags & 1;
    *((struct nuvec4_s *)fpatch_stream[0]->cur)++ = patch->nrm[2];
    ((u32 *)fpatch_stream[0]->cur)[-1] = flags & 8;
    *((struct nuvec4_s *)fpatch_stream[0]->cur)++ = patch->nrm[3];
    *((struct numtx_s *)fpatch_stream[0]->cur)++ = patch->col;
    *((struct numtx_s *)fpatch_stream[0]->cur)++ = *(struct numtx_s *)patch->pnt;
    if (heightbuf != 0) {
        fpatch_stream[0]->cur = vpDmaTag_Close(fpatch_stream[0]->cur);
        fpatch_stream[0]->cur = vpDmaTag_CntEx(fpatch_stream[0]->cur, 0x20000000, 0x3F);
        fpatch_stream[0]->cur = vpDmaTag_RefEx(fpatch_stream[0]->cur, heightbuf, 0x10000000,
                                               0x70000300);
        *(u16 *)(fpatch_stream[0]->cur - 0x10) = 0x40;
        fpatch_stream[0]->cur = vpDmaTag_Cnt(fpatch_stream[0]->cur);
    }
    *((u32 *)fpatch_stream[0]->cur)++ = 0;
    *((u32 *)fpatch_stream[0]->cur)++ = 0x14000000;
    fpatch_stream[0]->cur = vpDmaTag_Close(fpatch_stream[0]->cur);
    rndrstream_free = vpDmaTag_Next(fpatch_stream[0]->cur, 0);
}

void NuFPatchHeightFieldIndexSlow(struct nufpatch_s *src, f32 *heights, s32 index,
                                  struct nufpatch_s *dst)
{
    struct nufpatch_s children[4];
    struct nufpatch_s patch;
    s32 x;
    s32 y;
    u32 xbit;
    u32 ybit;
    s32 i;

    patch = *src;
    x = 0;
    y = 0;
    for (i = 0; i < 3; i++) {
        NuFPatchSplit(&patch, children, 1.0f);
        x <<= 1;
        y <<= 1;
        if ((index >> (2 - i)) & 1) {
            xbit = 1;
        } else {
            xbit = 0;
        }
        if ((index >> (5 - i)) & 1) {
            ybit = 1;
        } else {
            ybit = 0;
        }
        x |= xbit;
        y |= ybit;
        switch (ybit * 2 + xbit) {
        case 0:
            patch = children[0];
            break;
        case 1:
            patch = children[2];
            break;
        case 2:
            patch = children[1];
            break;
        case 3:
            patch = children[3];
            break;
        }
    }
    x += y * 9;
    patch.uv._01 = heights[x];
    patch.uv._03 = heights[x + 1];
    patch.uv._00 = heights[x + 81];
    patch.uv._02 = heights[x + 82];
    patch.uv._10 = 0.0f;
    patch.uv._11 = heights[x + 162];
    patch.uv._12 = 0.0f;
    patch.uv._13 = heights[x + 163];
    patch.uv._20 = 0.0f;
    patch.uv._21 = heights[x + 171];
    patch.uv._22 = 0.0f;
    patch.uv._23 = heights[x + 172];
    patch.uv._31 = heights[x + 9];
    patch.uv._33 = heights[x + 10];
    patch.uv._30 = heights[x + 90];
    patch.uv._32 = heights[x + 91];
    *dst = patch;
    dst->mtl[0] = src->mtl[0];
    dst->mtl[1] = src->mtl[1];
    dst->tid[0] = src->tid[0];
    dst->tid[1] = src->tid[1];
}


void NuMtxBilinear(struct nuvec4_s *corner, struct nuvec4_s *quad0, struct nuvec4_s *quad1,
                   struct nuvec4_s *quad2, struct nuvec4_s *quad3)
{
    struct nuvec4_s edge01;
    struct nuvec4_s edge02;
    struct nuvec4_s centre;
    struct nuvec4_s edge13;
    struct nuvec4_s edge23;

    NuVec4LerpVU0(&edge01, &corner[0], &corner[1], &fpatch_half);
    NuVec4LerpVU0(&edge02, &corner[0], &corner[2], &fpatch_half);
    NuVec4LerpVU0(&edge13, &corner[1], &corner[3], &fpatch_half);
    NuVec4LerpVU0(&edge23, &corner[2], &corner[3], &fpatch_half);
    NuVec4LerpVU0(&centre, &edge01, &edge23, &fpatch_half);
    quad0[0] = corner[0];
    quad0[1] = edge01;
    quad0[2] = edge02;
    quad0[3] = centre;
    quad1[0] = edge02;
    quad1[1] = centre;
    quad1[2] = corner[2];
    quad1[3] = edge23;
    quad2[0] = edge01;
    quad2[1] = corner[1];
    quad2[2] = centre;
    quad2[3] = edge13;
    quad3[0] = centre;
    quad3[1] = edge13;
    quad3[2] = edge23;
    quad3[3] = corner[3];
}

static inline void fpatchDisplace(f32 *x, f32 *height, f32 *y, struct nuvec4_s *wave, f32 amount)
{
    *x += amount * wave->x;
    *height += amount * wave->z;
    *y += amount * wave->y;
}


void NuFPatchSplit(struct nufpatch_s *patch, struct nufpatch_s *children, f32 amount)
{
    struct numtx_s left;
    struct numtx_s right;
    s32 wave[9];
    f32 height;
    struct nuvec4_s *waves;
    struct nuvec4_s *w;
    struct nuvec4_s *table;
    struct nuvec4_s *ripple;
    s32 i;

    if (water_wave_src == 0) {
        waves = NuMemAllocFn(0x1000, fpatch_file, 0xEA);
        NuRandSeed(0x11);
        height = water_split_height;
        w = waves;
        for (i = 0; i < 0x100; i++) {
            w->x = (NuRandFloat() - 0.5f) * 0.0f;
            w->y = (NuRandFloat() - 0.5f) * 0.0f;
            w->z = (NuRandFloat() - 0.5f) * height;
            w->w = (NuRandFloat() - 0.5f) * 0.0f;
            w->y = w->z;
            w++;
        }
        water_wave_src = waves;
    }
    NuMtxMulH(&left, &patch->uv, &fpatch_split_left);
    NuMtxMulH(&right, &patch->uv, &fpatch_split_right);
    NuMtxMulH(&children[0].uv, &fpatch_split_top, &left);
    NuMtxMulH(&children[1].uv, &fpatch_split_bottom, &left);
    NuMtxMulH(&children[2].uv, &fpatch_split_top, &right);
    NuMtxMulH(&children[3].uv, &fpatch_split_bottom, &right);
    NuMtxBilinear(patch->pnt, children[0].pnt, children[1].pnt, children[2].pnt, children[3].pnt);
    NuMtxBilinear(patch->nrm, children[0].nrm, children[1].nrm, children[2].nrm, children[3].nrm);
    NuMtxBilinear((struct nuvec4_s *)&patch->col, (struct nuvec4_s *)&children[0].col,
                  (struct nuvec4_s *)&children[1].col, (struct nuvec4_s *)&children[2].col,
                  (struct nuvec4_s *)&children[3].col);
    wave[0] = *(u8 *)&children[0].col._03;
    wave[1] = *(u8 *)&children[0].col._13;
    wave[2] = *(u8 *)&children[2].col._13;
    wave[3] = *(u8 *)&children[0].col._23;
    wave[4] = *(u8 *)&children[0].col._33;
    wave[5] = *(u8 *)&children[2].col._33;
    wave[6] = *(u8 *)&children[1].col._23;
    wave[7] = *(u8 *)&children[1].col._33;
    wave[8] = *(u8 *)&children[3].col._33;
    table = water_wave_src;
    ripple = &table[wave[0]];
    fpatchDisplace(&children[0].uv._00, &children[0].uv._01, &children[0].uv._11,
                   ripple, amount);
    ripple = &table[wave[1]];
    fpatchDisplace(&children[0].uv._02, &children[0].uv._03, &children[0].uv._13,
                   ripple, amount);
    fpatchDisplace(&children[2].uv._00, &children[2].uv._01, &children[2].uv._11,
                   ripple, amount);
    ripple = &table[wave[2]];
    fpatchDisplace(&children[2].uv._02, &children[2].uv._03, &children[2].uv._13,
                   ripple, amount);
    ripple = &table[wave[3]];
    fpatchDisplace(&children[0].uv._30, &children[0].uv._31, &children[0].uv._21,
                   ripple, amount);
    fpatchDisplace(&children[1].uv._00, &children[1].uv._01, &children[1].uv._11,
                   ripple, amount);
    ripple = &table[wave[4]];
    fpatchDisplace(&children[0].uv._32, &children[0].uv._33, &children[0].uv._23,
                   ripple, amount);
    fpatchDisplace(&children[1].uv._02, &children[1].uv._03, &children[1].uv._13,
                   ripple, amount);
    fpatchDisplace(&children[2].uv._30, &children[2].uv._31, &children[2].uv._21,
                   ripple, amount);
    fpatchDisplace(&children[3].uv._00, &children[3].uv._01, &children[3].uv._11,
                   ripple, amount);
    ripple = &table[wave[5]];
    fpatchDisplace(&children[2].uv._32, &children[2].uv._33, &children[2].uv._23,
                   ripple, amount);
    fpatchDisplace(&children[3].uv._02, &children[3].uv._03, &children[3].uv._13,
                   ripple, amount);
    ripple = &table[wave[6]];
    fpatchDisplace(&children[1].uv._30, &children[1].uv._31, &children[1].uv._21,
                   ripple, amount);
    ripple = &table[wave[7]];
    fpatchDisplace(&children[1].uv._32, &children[1].uv._33, &children[1].uv._23,
                   ripple, amount);
    fpatchDisplace(&children[3].uv._30, &children[3].uv._31, &children[3].uv._21,
                   ripple, amount);
    ripple = &table[wave[8]];
    fpatchDisplace(&children[3].uv._32, &children[3].uv._33, &children[3].uv._23,
                   ripple, amount);
}


static inline void fpatchScaleMtx(struct numtx_s *mtx, f32 scale)
{
    mtx->_00 *= scale;
    mtx->_01 *= scale;
    mtx->_02 *= scale;
    mtx->_03 *= scale;
    mtx->_10 *= scale;
    mtx->_11 *= scale;
    mtx->_12 *= scale;
    mtx->_13 *= scale;
    mtx->_20 *= scale;
    mtx->_21 *= scale;
    mtx->_22 *= scale;
    mtx->_23 *= scale;
    mtx->_30 *= scale;
    mtx->_31 *= scale;
    mtx->_32 *= scale;
    mtx->_33 *= scale;
}


void NuFPatchTesselateGrid(s32 depth, struct nufpatch_s *patch, f32 scale, f32 height,
                           struct nufpatch_s **out, f32 falloff)
{
    struct nufpatch_s children[4];

    if (depth == 1) {
        fpatchScaleMtx((struct numtx_s *)patch->pnt, scale);
        fpatchScaleMtx((struct numtx_s *)patch->nrm, scale);
        **out = *patch;
        (*out)++;
    } else {
        height *= falloff;
        NuFPatchSplit(patch, children, height);
        depth--;
        NuFPatchTesselateGrid(depth, &children[0], scale, height, out, 0.5f);
        NuFPatchTesselateGrid(depth, &children[1], scale, height, out, 0.5f);
        NuFPatchTesselateGrid(depth, &children[2], scale, height, out, 0.5f);
        NuFPatchTesselateGrid(depth, &children[3], scale, height, out, 0.5f);
    }
}


void NuFPatchTesselateSphereRec(s32 depth, struct nufpatch_s *patch, f32 radius, f32 scale,
                                struct nufpatch_s **out)
{
    struct nufpatch_s children[4];
    struct nuvec_s *first;
    struct nuvec_s *v;
    struct nuvec4_s *n;
    f32 nrmscale;

    if (depth == 1) {
        nrmscale = radius * scale;
        first = (struct nuvec_s *)&patch->pnt[0];
        NuVecNorm(first, first);
        NuVecScale(first, first, radius);
        v = (struct nuvec_s *)&patch->pnt[1];
        NuVecNorm(v, v);
        NuVecScale(v, v, radius);
        v = (struct nuvec_s *)&patch->pnt[2];
        NuVecNorm(v, v);
        NuVecScale(v, v, radius);
        v = (struct nuvec_s *)&patch->pnt[3];
        NuVecNorm(v, v);
        NuVecScale(v, v, radius);
        n = patch->nrm;
        NuVecNorm((struct nuvec_s *)n, (struct nuvec_s *)n);
        NuVecScale((struct nuvec_s *)n, (struct nuvec_s *)n, nrmscale);
        n++;
        NuVecNorm((struct nuvec_s *)n, (struct nuvec_s *)n);
        NuVecScale((struct nuvec_s *)n, (struct nuvec_s *)n, nrmscale);
        n++;
        NuVecNorm((struct nuvec_s *)n, (struct nuvec_s *)n);
        NuVecScale((struct nuvec_s *)n, (struct nuvec_s *)n, nrmscale);
        n++;
        NuVecNorm((struct nuvec_s *)n, (struct nuvec_s *)n);
        NuVecScale((struct nuvec_s *)n, (struct nuvec_s *)n, nrmscale);
        n++;
        patch->col._00 = 0.0f;
        patch->col._01 = 0.0f;
        patch->col._10 = 1.0f;
        patch->col._11 = 0.0f;
        patch->col._20 = 0.0f;
        patch->col._21 = 1.0f;
        patch->col._30 = 1.0f;
        patch->col._31 = 1.0f;
        **out = *patch;
        (*out)++;
    } else {
        NuFPatchSplit(patch, children, 0.0f);
        depth--;
        scale *= 0.5f;
        NuFPatchTesselateSphereRec(depth, &children[0], radius, scale, out);
        NuFPatchTesselateSphereRec(depth, &children[1], radius, scale, out);
        NuFPatchTesselateSphereRec(depth, &children[2], radius, scale, out);
        NuFPatchTesselateSphereRec(depth, &children[3], radius, scale, out);
    }
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


void NuFPatchTesselateSphere(s32 depth, struct nufpatch_s *patch, f32 radius,
                             struct nufpatch_s **out)
{
    NuFPatchTesselateSphereRec(depth, patch, radius, 1.0f, out);
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
