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

#include "nu_libc.h"
#include "nu_types.h"

#include "numath/numathtypes.h"
#include "numath/numtx.h"
#include "numath/nurand.h"
#include "numath/nuvec.h"
#include "nu3d/numtl.h"
#include "nu3d/nustream.h"
#include "nu3d/nutex.h"
#include "nu3d/nucamera.h"

struct nuhfvtx_s {
    struct nuvec4_s pnt;
    struct nuvec4_s nrm;
    struct nuvec4_s col;
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

extern f32 D_006331A8;
#define fpatch_height_base D_006331A8

extern struct nuvec4_s D_002E96C0[12];
#define submtx2 D_002E96C0

extern s32 D_002E9780[16][4];
#define stitchtable D_002E9780

extern u32 D_002E9880[3][4];
#define heightTable D_002E9880

extern u8 *rndrstream_free;
extern s32 fast_index;

extern void *NuMemAllocFn(s32 size, char *file, s32 line);
extern void NuFPatchLightTexture(struct nutex_s *tex, s32 size, struct nuvec_s *lightdir,
                                 f32 range, f32 scale, f32 bias);
extern u8 *NuPs2TxGenUpload(u8 *cur, void *ps2tex, s32 addr);
extern u8 *vpDmaTag_Cnt(u8 *cur);
extern u8 *vpDmaTag_Close(u8 *cur);
extern u8 *vpDmaTag_CntEx(u8 *cur, u32 vif0, u32 vif1);
extern u8 *vpDmaTag_RefEx(u8 *cur, void *addr, u32 vif0, u32 vif1);
extern void NuMtxBilinear(struct nuvec4_s *corner, struct nuvec4_s *quad0, struct nuvec4_s *quad1,
                          struct nuvec4_s *quad2, struct nuvec4_s *quad3);
extern void NuVec4LerpVU0(struct nuvec4_s *out, struct nuvec4_s *a, struct nuvec4_s *b, f32 *t);
extern void NuRndrStreamLink(struct nurndrstream_s *rs);
extern u8 *vpDmaTag_Call(u8 *cur, void *microcode);
extern u8 *vpDmaTag_Next(u8 *cur, s32 mode);

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
    mtl->attrib2.uvmode = 0;
    mtl->attrib2.afail = 3;
    mtl->attrib.aref = 0;
    mtl->attrib.fill = 1;
    mtl->ctxt = 0;
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
    mtl->K = -175;
    mtl->L = 1;
    mtl->ctxt = 0;
    mtl2 = NuMtlCreate(1);
    mtlSetDefaults(mtl2);
    mtl2->tid = light_tid;
    mtl->K = -175;
    mtl->L = 1;
    mtl2->ctxt = 1;
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
        fpatch_stream[i]->cur.b = NuPs2TxGenUpload(fpatch_stream[i]->cur.b, ps2tex, fpatch_texaddr);
        rndrstream_free = vpDmaTag_Next(fpatch_stream[i]->cur.b, 0);
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


void NuRndrFPatchInit(u32 levels, s32 detail, f32 lodscale, f32 heightscale, f32 lodrange,
                      f32 heightmax, f32 heightmin, f32 nearclip_override)
{
    struct numtx_s frustum;
    struct numtx_s inv_clip;
    struct numtx_s vpc;
    f32 nearclip;
    f32 farclip;
    f32 one;
    f32 falloff;
    f32 dist;
    f32 base;
    s32 i;
    u32 j;

    fpatch_height_base = heightmin;
    initCamera(&frustum, &inv_clip, &vpc, &nearclip, &farclip);
    if (nearclip_override != 0.0f) {
        nearclip = nearclip_override;
    }
    one = 1.0f;
    for (i = 0; i < FPATCH_STREAMS; i++) {
        NuRndrStreamLink(fpatch_stream[i]);
        fpatch_stream[i]->cur.b = vpDmaTag_RefEx(fpatch_stream[i]->cur.b, water_wave_src, 0x10000000,
                                                 0x6C000300);
        *(u16 *)(fpatch_stream[i]->cur.b - 0x10) = 0x100;
        fpatch_stream[i]->cur.b = vpDmaTag_Cnt(fpatch_stream[i]->cur.b);
        *fpatch_stream[i]->cur.w++ = 0x10000000;
        *fpatch_stream[i]->cur.w++ = 0x6C840200;
        for (j = 0; j < 0x84; j++) {
            *fpatch_stream[i]->cur.w++ = 0x80;
            *fpatch_stream[i]->cur.w++ = 0x80;
            *fpatch_stream[i]->cur.w++ = 0x80;
            *fpatch_stream[i]->cur.w++ = 0x80;
        }
        *fpatch_stream[i]->cur.w++ = 0x10000000;
        *fpatch_stream[i]->cur.w++ = 0x6C4202BE;
        for (j = 0; j < levels; j++) {
            heightscale *= 0.5f;
        }
        dist = 0.125f;
        *fpatch_stream[i]->cur.f++ = one;
        *fpatch_stream[i]->cur.f++ = dist;
        *fpatch_stream[i]->cur.f++ = heightscale * 1.732f;
        *fpatch_stream[i]->cur.w++ = 0;
        *((u64 *)fpatch_stream[i]->cur.b)++ = 0x3002C0000000000AULL;
        *((u64 *)fpatch_stream[i]->cur.b)++ = 0x512;
        *((struct nuvec4_s *)fpatch_stream[i]->cur.b)++ = submtx2[0];
        *((struct nuvec4_s *)fpatch_stream[i]->cur.b)++ = submtx2[1];
        *((struct nuvec4_s *)fpatch_stream[i]->cur.b)++ = submtx2[2];
        *((struct nuvec4_s *)fpatch_stream[i]->cur.b)++ = submtx2[3];
        *((struct nuvec4_s *)fpatch_stream[i]->cur.b)++ = submtx2[4];
        *((struct nuvec4_s *)fpatch_stream[i]->cur.b)++ = submtx2[5];
        *((struct nuvec4_s *)fpatch_stream[i]->cur.b)++ = submtx2[6];
        *((struct nuvec4_s *)fpatch_stream[i]->cur.b)++ = submtx2[7];
        *((struct nuvec4_s *)fpatch_stream[i]->cur.b)++ = submtx2[8];
        *((struct nuvec4_s *)fpatch_stream[i]->cur.b)++ = submtx2[9];
        *((struct nuvec4_s *)fpatch_stream[i]->cur.b)++ = submtx2[10];
        *((struct nuvec4_s *)fpatch_stream[i]->cur.b)++ = submtx2[11];
        *((struct numtx_s *)fpatch_stream[i]->cur.b)++ = vpc;
        for (j = 0; j < 16; j++) {
            *fpatch_stream[i]->cur.i++ = stitchtable[j][0] + 4;
            *fpatch_stream[i]->cur.i++ = stitchtable[j][1] + 4;
            *fpatch_stream[i]->cur.i++ = stitchtable[j][2] + 4;
            *fpatch_stream[i]->cur.i++ = stitchtable[j][3] + 4;
        }
        *((struct numtx_s *)fpatch_stream[i]->cur.b)++ = frustum;
        *((struct numtx_s *)fpatch_stream[i]->cur.b)++ = inv_clip;
        *fpatch_stream[i]->cur.f++ = nearclip;
        *fpatch_stream[i]->cur.f++ = farclip;
        *fpatch_stream[i]->cur.f++ = lodscale;
        *fpatch_stream[i]->cur.f++ = lodscale;
        lodscale += lodscale;
        *fpatch_stream[i]->cur.f++ = one;
        *fpatch_stream[i]->cur.f++ = one;
        *fpatch_stream[i]->cur.f++ = one;
        *fpatch_stream[i]->cur.f++ = one;
        *((u64 *)fpatch_stream[i]->cur.b)++ = 0x3002C0000000800AULL;
        *((u64 *)fpatch_stream[i]->cur.b)++ = 0x512;
        falloff = 0.47f;
        dist = 0.25f;
        if (water_table_ready) {
            dist = 0.375f;
        }
        for (j = 0; j < levels; j++) {
            lodscale *= 0.5f;
            falloff *= 0.47f;
            dist += dist;
        }
        base = dist;
        for (j = 0; j < 16; j++) {
            *fpatch_stream[i]->cur.f++ = lodscale;
            *fpatch_stream[i]->cur.f++ = -1.0f / (lodscale * lodrange);
            *fpatch_stream[i]->cur.f++ = falloff;
            if (j < 3 && detail) {
                *fpatch_stream[i]->cur.f++ = base * 4.0f;
            } else {
                *fpatch_stream[i]->cur.f++ = dist;
            }
            lodscale *= 0.5f;
            falloff *= 0.47f;
            dist += dist;
        }
        *((u64 *)fpatch_stream[i]->cur.b)++ = 0x0002C00000008000ULL;
        *((u64 *)fpatch_stream[i]->cur.b)++ = 0;
        if (heightmin == 0.0f && heightmax == 0.0f) {
            heightmin = 10000.0f;
            heightmax = 9999.0f;
        }
        *fpatch_stream[i]->cur.f++ = heightmin;
        *fpatch_stream[i]->cur.f++ = 128.0f / (heightmax - heightmin);
        *fpatch_stream[i]->cur.w++ = 0;
        *fpatch_stream[i]->cur.w++ = 0;
        for (j = 0; j < 3; j++) {
            *fpatch_stream[i]->cur.w++ = heightTable[j][0];
            *fpatch_stream[i]->cur.w++ = heightTable[j][1];
            *fpatch_stream[i]->cur.w++ = heightTable[j][2];
            *fpatch_stream[i]->cur.w++ = heightTable[j][3];
        }
        *fpatch_stream[i]->cur.w++ = 0x10000000;
        *fpatch_stream[i]->cur.w++ = 0x6C030200;
        *((u64 *)fpatch_stream[i]->cur.b)++ = 0x1000000000000001ULL;
        *((u64 *)fpatch_stream[i]->cur.b)++ = 0xE;
        *((u64 *)fpatch_stream[i]->cur.b)++ = 0x58;
        *((u64 *)fpatch_stream[i]->cur.b)++ = 0x1B;
        *((u64 *)fpatch_stream[i]->cur.b)++ = 0x3002C0000000000AULL;
        *((u64 *)fpatch_stream[i]->cur.b)++ = 0x512;
        *fpatch_stream[i]->cur.w++ = 0x10000000;
        *fpatch_stream[i]->cur.w++ = 0x6C030221;
        *((u64 *)fpatch_stream[i]->cur.b)++ = 0x1000000000000001ULL;
        *((u64 *)fpatch_stream[i]->cur.b)++ = 0xE;
        *((u64 *)fpatch_stream[i]->cur.b)++ = 0x258;
        *((u64 *)fpatch_stream[i]->cur.b)++ = 0x1B;
        *((u64 *)fpatch_stream[i]->cur.b)++ = 0x3002C0000000800AULL;
        *((u64 *)fpatch_stream[i]->cur.b)++ = 0x512;
        *fpatch_stream[i]->cur.w++ = 0x10000000;
        *fpatch_stream[i]->cur.w++ = 0x6C030242;
        *((u64 *)fpatch_stream[i]->cur.b)++ = 0x1000000000000001ULL;
        *((u64 *)fpatch_stream[i]->cur.b)++ = 0xE;
        *((u64 *)fpatch_stream[i]->cur.b)++ = 0x58;
        *((u64 *)fpatch_stream[i]->cur.b)++ = 0x1B;
        *((u64 *)fpatch_stream[i]->cur.b)++ = 0x3002C0000000000AULL;
        *((u64 *)fpatch_stream[i]->cur.b)++ = 0x512;
        *fpatch_stream[i]->cur.w++ = 0x10000000;
        *fpatch_stream[i]->cur.w++ = 0x6C030263;
        *((u64 *)fpatch_stream[i]->cur.b)++ = 0x1000000000000001ULL;
        *((u64 *)fpatch_stream[i]->cur.b)++ = 0xE;
        *((u64 *)fpatch_stream[i]->cur.b)++ = 0x258;
        *((u64 *)fpatch_stream[i]->cur.b)++ = 0x1B;
        *((u64 *)fpatch_stream[i]->cur.b)++ = 0x3002C0000000800AULL;
        *((u64 *)fpatch_stream[i]->cur.b)++ = 0x512;
        fpatch_stream[i]->cur.b = vpDmaTag_Close(fpatch_stream[i]->cur.b);
        rndrstream_free = vpDmaTag_Next(fpatch_stream[i]->cur.b, 0);
    }
}


void NuRndrFPatchCameraUpdate(void)
{
    u32 i;

    for (i = 0; i < FPATCH_STREAMS; i++) {
        NuRndrStreamLink(fpatch_stream[i]);
        fpatch_stream[i]->cur.b = vpDmaTag_Cnt(fpatch_stream[i]->cur.b);
        *((u32 *)fpatch_stream[i]->cur.b)++ = 0;
        *((u32 *)fpatch_stream[i]->cur.b)++ = 0;
        *((u32 *)fpatch_stream[i]->cur.b)++ = 0x10000000;
        *((u32 *)fpatch_stream[i]->cur.b)++ = 0x6C0402CC;
        *((struct numtx_s *)fpatch_stream[i]->cur.b)++ = *NuCameraGetVPCSMtx();
        fpatch_stream[i]->cur.b = vpDmaTag_Close(fpatch_stream[i]->cur.b);
        rndrstream_free = vpDmaTag_Next(fpatch_stream[i]->cur.b, 0);
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
    fpatch_stream[0]->cur.b = vpDmaTag_Cnt(fpatch_stream[0]->cur.b);
    *((u32 *)fpatch_stream[0]->cur.b)++ = 0x10000000;
    *((u32 *)fpatch_stream[0]->cur.b)++ = 0x6C100001;
    *((struct numtx_s *)fpatch_stream[0]->cur.b)++ = patch->uv;
    *((struct nuvec4_s *)fpatch_stream[0]->cur.b)++ = patch->nrm[0];
    ((u32 *)fpatch_stream[0]->cur.b)[-1] = (heightbuf != 0);
    *((struct nuvec4_s *)fpatch_stream[0]->cur.b)++ = patch->nrm[1];
    ((u32 *)fpatch_stream[0]->cur.b)[-1] = flags & 1;
    *((struct nuvec4_s *)fpatch_stream[0]->cur.b)++ = patch->nrm[2];
    ((u32 *)fpatch_stream[0]->cur.b)[-1] = flags & 8;
    *((struct nuvec4_s *)fpatch_stream[0]->cur.b)++ = patch->nrm[3];
    *((struct numtx_s *)fpatch_stream[0]->cur.b)++ = patch->col;
    *((struct numtx_s *)fpatch_stream[0]->cur.b)++ = *(struct numtx_s *)patch->pnt;
    if (heightbuf != 0) {
        fpatch_stream[0]->cur.b = vpDmaTag_Close(fpatch_stream[0]->cur.b);
        fpatch_stream[0]->cur.b = vpDmaTag_CntEx(fpatch_stream[0]->cur.b, 0x20000000, 0x3F);
        fpatch_stream[0]->cur.b = vpDmaTag_RefEx(fpatch_stream[0]->cur.b, heightbuf, 0x10000000,
                                               0x70000300);
        *(u16 *)(fpatch_stream[0]->cur.b - 0x10) = 0x40;
        fpatch_stream[0]->cur.b = vpDmaTag_Cnt(fpatch_stream[0]->cur.b);
    }
    *((u32 *)fpatch_stream[0]->cur.b)++ = 0;
    *((u32 *)fpatch_stream[0]->cur.b)++ = 0x14000000;
    fpatch_stream[0]->cur.b = vpDmaTag_Close(fpatch_stream[0]->cur.b);
    rndrstream_free = vpDmaTag_Next(fpatch_stream[0]->cur.b, 0);
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


void NuFPatchHeightFieldIndex(struct nufpatch_s *src, f32 *heights, s32 index,
                              struct nufpatch_s *dst)
{
    struct nufpatch_s patch;
    struct nuvec4_s pnt[4];
    struct nuvec4_s pnt01;
    struct nuvec4_s pnt02;
    struct nuvec4_s pntmid;
    struct nuvec4_s pnt13;
    struct nuvec4_s pnt23;
    struct nuvec4_s nrm[4];
    struct nuvec4_s nrm01;
    struct nuvec4_s nrm02;
    struct nuvec4_s nrmmid;
    struct nuvec4_s nrm13;
    struct nuvec4_s nrm23;
    f32 w0;
    f32 w1;
    f32 w2;
    f32 w3;
    f32 w01;
    f32 w02;
    f32 w13;
    f32 w23;
    f32 wmid;
    s32 x;
    s32 y;
    u32 xbit;
    u32 ybit;
    s32 i;
    f32 *row;

    patch = *src;
    pnt[0] = src->pnt[0];
    pnt[1] = src->pnt[1];
    pnt[2] = src->pnt[2];
    pnt[3] = src->pnt[3];
    nrm[0] = src->nrm[0];
    nrm[1] = src->nrm[1];
    nrm[2] = src->nrm[2];
    nrm[3] = src->nrm[3];
    w0 = src->col._03;
    row = &src->col._10;
    w1 = row[3];
    row = &src->col._20;
    w2 = row[3];
    row = &src->col._30;
    w3 = row[3];
    x = 0;
    y = 0;
    for (i = 0; i < 3; i++) {
        x <<= 1;
        y <<= 1;
        NuVec4LerpVU0(&pnt01, &pnt[0], &pnt[1], &fpatch_half);
        NuVec4LerpVU0(&pnt02, &pnt[0], &pnt[2], &fpatch_half);
        NuVec4LerpVU0(&pnt13, &pnt[1], &pnt[3], &fpatch_half);
        NuVec4LerpVU0(&pnt23, &pnt[2], &pnt[3], &fpatch_half);
        NuVec4LerpVU0(&pntmid, &pnt01, &pnt23, &fpatch_half);
        NuVec4LerpVU0(&nrm01, &nrm[0], &nrm[1], &fpatch_half);
        NuVec4LerpVU0(&nrm02, &nrm[0], &nrm[2], &fpatch_half);
        NuVec4LerpVU0(&nrm13, &nrm[1], &nrm[3], &fpatch_half);
        NuVec4LerpVU0(&nrm23, &nrm[2], &nrm[3], &fpatch_half);
        NuVec4LerpVU0(&nrmmid, &nrm01, &nrm23, &fpatch_half);
        w01 = (w0 + w1) * 0.5f;
        w23 = (w2 + w3) * 0.5f;
        w02 = (w0 + w2) * 0.5f;
        w13 = (w1 + w3) * 0.5f;
        wmid = (w01 + w23) * 0.5f;
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
            pnt[1] = pnt01;
            pnt[2] = pnt02;
            pnt[3] = pntmid;
            nrm[1] = nrm01;
            nrm[2] = nrm02;
            nrm[3] = nrmmid;
            w1 = w01;
            w2 = w02;
            w3 = wmid;
            break;
        case 1:
            pnt[0] = pnt01;
            pnt[2] = pntmid;
            pnt[3] = pnt13;
            nrm[0] = nrm01;
            nrm[2] = nrmmid;
            nrm[3] = nrm13;
            w0 = w01;
            w2 = wmid;
            w3 = w13;
            break;
        case 2:
            pnt[0] = pnt02;
            pnt[1] = pntmid;
            pnt[3] = pnt23;
            nrm[0] = nrm02;
            nrm[1] = nrmmid;
            nrm[3] = nrm23;
            w0 = w02;
            w1 = wmid;
            w3 = w23;
            break;
        case 3:
            pnt[0] = pntmid;
            pnt[1] = pnt13;
            pnt[2] = pnt23;
            nrm[0] = nrmmid;
            nrm[1] = nrm13;
            nrm[2] = nrm23;
            w0 = wmid;
            w1 = w13;
            w2 = w23;
            break;
        }
    }
    patch.pnt[0] = pnt[0];
    patch.pnt[1] = pnt[1];
    patch.pnt[2] = pnt[2];
    patch.pnt[3] = pnt[3];
    patch.nrm[0] = nrm[0];
    patch.nrm[1] = nrm[1];
    patch.nrm[2] = nrm[2];
    patch.nrm[3] = nrm[3];
    patch.col._03 = w0;
    row = &patch.col._10;
    row[3] = w1;
    row = &patch.col._20;
    row[3] = w2;
    row = &patch.col._30;
    row[3] = w3;
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


void NuFPatchUpdateLight(struct nuvec_s *lightdir, f32 scale, f32 bias)
{
    struct nuvec_s dir;
    struct nutexslot_s *entry;
    struct nups2tex_s *ps2tex;
    u8 *texel;
    s32 x;
    s32 y;
    f32 dot;
    f32 light;
    u8 shade;
    f32 range;

    range = 4.0f;
    NuTexGetPointers(fpatch_lighttid, &entry, &ps2tex);
    texel = ps2tex->mipdata[0] + 0x70;
    for (y = 0; y < 32; y++) {
        for (x = 0; x < 32; x++) {
            dir.x = -(x * 8.0f / 31.0f - range);
            dir.y = -(y * 8.0f / 31.0f - range);
            dir.z = 1.0f;
            NuVecNorm(&dir, &dir);
            dot = NuVecDot(&dir, lightdir);
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
            shade = light;
            *texel++ = shade;
            *texel++ = shade;
            *texel++ = shade;
            *texel++ = shade;
        }
    }
}


inline void NuFPatchTesselateSphere(s32 depth, struct nufpatch_s *patch, f32 radius,
                                    struct nufpatch_s **out)
{
    NuFPatchTesselateSphereRec(depth, patch, radius, 1.0f, out);
}


struct nufpatch_s *NuFPatchGenerateWorld(s32 depth, s32 *count, s32 radius)
{
    struct numtx_s uv = {0};
    struct numtx_s front_pnt = {
        -0.57735f, -0.57735f, 0.57735f, 1.0f,
        0.57735f, -0.57735f, 0.57735f, 1.0f,
        -0.57735f, 0.57735f, 0.57735f, 1.0f,
        0.57735f, 0.57735f, 0.57735f, 1.0f
    };
    struct numtx_s back_pnt = {
        -0.57735f, -0.57735f, -0.57735f, 1.0f,
        0.57735f, -0.57735f, -0.57735f, 1.0f,
        -0.57735f, 0.57735f, -0.57735f, 1.0f,
        0.57735f, 0.57735f, -0.57735f, 1.0f
    };
    struct numtx_s right_pnt = {
        0.57735f, -0.57735f, -0.57735f, 1.0f,
        0.57735f, 0.57735f, -0.57735f, 1.0f,
        0.57735f, -0.57735f, 0.57735f, 1.0f,
        0.57735f, 0.57735f, 0.57735f, 1.0f
    };
    struct numtx_s left_pnt = {
        -0.57735f, -0.57735f, -0.57735f, 1.0f,
        -0.57735f, 0.57735f, -0.57735f, 1.0f,
        -0.57735f, -0.57735f, 0.57735f, 1.0f,
        -0.57735f, 0.57735f, 0.57735f, 1.0f
    };
    struct numtx_s top_pnt = {
        -0.57735f, 0.57735f, -0.57735f, 1.0f,
        0.57735f, 0.57735f, -0.57735f, 1.0f,
        -0.57735f, 0.57735f, 0.57735f, 1.0f,
        0.57735f, 0.57735f, 0.57735f, 1.0f
    };
    struct numtx_s bottom_pnt = {
        -0.57735f, -0.57735f, -0.57735f, 1.0f,
        0.57735f, -0.57735f, -0.57735f, 1.0f,
        -0.57735f, -0.57735f, 0.57735f, 1.0f,
        0.57735f, -0.57735f, 0.57735f, 1.0f
    };
    struct numtx_s front_nrm = {
        -0.57735f, -0.57735f, 0.57735f, 0.0f,
        0.57735f, -0.57735f, 0.57735f, 0.0f,
        -0.57735f, 0.57735f, 0.57735f, 0.0f,
        0.57735f, 0.57735f, 0.57735f, 0.0f
    };
    struct numtx_s back_nrm = {
        -0.57735f, -0.57735f, -0.57735f, 0.0f,
        0.57735f, -0.57735f, -0.57735f, 0.0f,
        -0.57735f, 0.57735f, -0.57735f, 0.0f,
        0.57735f, 0.57735f, -0.57735f, 0.0f
    };
    struct numtx_s right_nrm = {
        0.57735f, -0.57735f, -0.57735f, 0.0f,
        0.57735f, 0.57735f, -0.57735f, 0.0f,
        0.57735f, -0.57735f, 0.57735f, 0.0f,
        0.57735f, 0.57735f, 0.57735f, 0.0f
    };
    struct numtx_s left_nrm = {
        -0.57735f, -0.57735f, -0.57735f, 0.0f,
        -0.57735f, 0.57735f, -0.57735f, 0.0f,
        -0.57735f, -0.57735f, 0.57735f, 0.0f,
        -0.57735f, 0.57735f, 0.57735f, 0.0f
    };
    struct numtx_s top_nrm = {
        -0.57735f, 0.57735f, -0.57735f, 0.0f,
        0.57735f, 0.57735f, -0.57735f, 0.0f,
        -0.57735f, 0.57735f, 0.57735f, 0.0f,
        0.57735f, 0.57735f, 0.57735f, 0.0f
    };
    struct numtx_s bottom_nrm = {
        -0.57735f, -0.57735f, -0.57735f, 0.0f,
        0.57735f, -0.57735f, -0.57735f, 0.0f,
        -0.57735f, -0.57735f, 0.57735f, 0.0f,
        0.57735f, -0.57735f, 0.57735f, 0.0f
    };
    f32 light[6][4] = {
        {0.053098477f, 0.19876964f, 0.20987545f, 0.3389355f},
        {0.6198752f, 0.7198163f, 0.48765674f, 0.55558765f},
        {0.7198163f, 0.55558765f, 0.19876964f, 0.3389355f},
        {0.6198752f, 0.48765674f, 0.053098477f, 0.20987545f},
        {0.48765674f, 0.55558765f, 0.20987545f, 0.3389355f},
        {0.6198752f, 0.7198163f, 0.053098477f, 0.19876964f}
    };
    struct numtx_s col = {
        0.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 0.0f
    };
    struct nufpatch_s patch;
    struct nufpatch_s *out;
    struct nufpatch_s *patches;
    s32 i;

    if (depth == 1) {
        *count = 1;
    } else {
        *count = 4;
        for (i = 1; i < depth - 1; i++) {
            *count *= 4;
        }
    }
    *count *= 6;
    patches = NuMemAllocFn(*count * sizeof(struct nufpatch_s), fpatch_file, 0x734);
    out = patches;
    col._03 = light[0][0];
    col._13 = light[0][1];
    col._23 = light[0][2];
    col._33 = light[0][3];
    *(struct numtx_s *)patch.pnt = front_pnt;
    *(struct numtx_s *)patch.nrm = front_nrm;
    patch.col = col;
    patch.uv = uv;
    NuFPatchTesselateSphere(depth, &patch, radius, &out);
    col._03 = light[1][0];
    col._13 = light[1][1];
    col._23 = light[1][2];
    col._33 = light[1][3];
    *(struct numtx_s *)patch.pnt = back_pnt;
    *(struct numtx_s *)patch.nrm = back_nrm;
    patch.col = col;
    patch.uv = uv;
    NuFPatchTesselateSphere(depth, &patch, radius, &out);
    col._03 = light[2][0];
    col._13 = light[2][1];
    col._23 = light[2][2];
    col._33 = light[2][3];
    *(struct numtx_s *)patch.pnt = right_pnt;
    *(struct numtx_s *)patch.nrm = right_nrm;
    patch.col = col;
    patch.uv = uv;
    NuFPatchTesselateSphere(depth, &patch, radius, &out);
    col._03 = light[3][0];
    col._13 = light[3][1];
    col._23 = light[3][2];
    col._33 = light[3][3];
    *(struct numtx_s *)patch.pnt = left_pnt;
    *(struct numtx_s *)patch.nrm = left_nrm;
    patch.col = col;
    patch.uv = uv;
    NuFPatchTesselateSphere(depth, &patch, radius, &out);
    col._03 = light[4][0];
    col._13 = light[4][1];
    col._23 = light[4][2];
    col._33 = light[4][3];
    *(struct numtx_s *)patch.pnt = top_pnt;
    *(struct numtx_s *)patch.nrm = top_nrm;
    patch.col = col;
    patch.uv = uv;
    NuFPatchTesselateSphere(depth, &patch, radius, &out);
    col._03 = light[5][0];
    col._13 = light[5][1];
    col._23 = light[5][2];
    col._33 = light[5][3];
    *(struct numtx_s *)patch.pnt = bottom_pnt;
    *(struct numtx_s *)patch.nrm = bottom_nrm;
    patch.col = col;
    patch.uv = uv;
    NuFPatchTesselateSphere(depth, &patch, radius, &out);
    return patches;
}


static inline void fpatchOffsetCorner(struct nuvec_s *out, struct nuvec_s *nrm, struct nuvec_s *pnt)
{
    NuVecScale(out, nrm, 1.0f);
    NuVecAdd(out, out, pnt);
}


f32 NuFPatchShadowCast(struct nufpatch_s *patch, struct nuvec_s *lightpos)
{
    struct nuvec_s pos;
    struct nuvec_s normal;
    struct nuvec_s twist;
    struct nuvec_s error;
    struct nuvec_s corner0;
    struct nuvec_s corner1;
    struct nuvec_s corner2;
    struct nuvec_s corner3;
    struct nuvec_s du;
    struct nuvec_s dv;
    struct nuvec_s top;
    struct nuvec_s bottom;
    struct nuvec_s unitnormal;
    struct nuvec_s tolight;
    struct nuvec_s *pnt0;
    struct nuvec_s *pnt1;
    struct nuvec_s *pnt2;
    struct nuvec_s *pnt3;
    struct nuvec_s *nrm0;
    struct nuvec_s *nrm1;
    struct nuvec_s *nrm2;
    struct nuvec_s *nrm3;
    f32 u;
    f32 v;
    struct nuvec_s *corner0p;
    struct nuvec_s *corner1p;
    struct nuvec_s *corner2p;
    struct nuvec_s *corner3p;
    f32 rate;
    f32 one;
    s32 i;

    u = 0.5f;
    v = 0.5f;
    rate = 0.9f;
    pnt0 = (struct nuvec_s *)&patch->pnt[0];
    pnt1 = (struct nuvec_s *)&patch->pnt[1];
    pnt2 = (struct nuvec_s *)&patch->pnt[2];
    pnt3 = (struct nuvec_s *)&patch->pnt[3];
    nrm0 = (struct nuvec_s *)&patch->nrm[0];
    nrm1 = (struct nuvec_s *)&patch->nrm[1];
    nrm2 = (struct nuvec_s *)&patch->nrm[2];
    nrm3 = (struct nuvec_s *)&patch->nrm[3];
    one = 1.0f;
    for (i = 0; i < 10; i++) {
        NuVecLerp(&top, pnt0, pnt1, one - u);
        NuVecLerp(&bottom, pnt2, pnt3, one - u);
        NuVecLerp(&pos, &top, &bottom, one - v);
        NuVecLerp(&top, nrm0, nrm1, one - u);
        NuVecLerp(&bottom, nrm2, nrm3, one - u);
        NuVecLerp(&normal, &top, &bottom, one - v);
        NuVecNorm(&unitnormal, &normal);
        NuVecSub(&tolight, lightpos, &pos);
        NuVecNorm(&tolight, &tolight);
        NuVecSub(&error, &tolight, &unitnormal);
        corner0p = &corner0;
        corner1p = &corner1;
        corner2p = &corner2;
        corner3p = &corner3;
        fpatchOffsetCorner(corner0p, nrm0, pnt0);
        fpatchOffsetCorner(corner1p, nrm1, pnt1);
        fpatchOffsetCorner(corner2p, nrm2, pnt2);
        fpatchOffsetCorner(corner3p, nrm3, pnt3);
        NuVecSub(&twist, corner3p, corner2p);
        NuVecSub(&twist, &twist, corner1p);
        NuVecAdd(&twist, &twist, corner0p);
        NuVecScale(&du, &twist, v);
        NuVecAdd(&du, &du, corner2p);
        NuVecSub(&du, &du, corner0p);
        NuVecScale(&dv, &twist, u);
        NuVecAdd(&dv, &dv, corner1p);
        NuVecSub(&dv, &dv, corner0p);
        u += rate * NuVecDot(&error, &du);
        v += rate * NuVecDot(&error, &dv);
    }
    return 0.0f;
}


static inline void fpatchLerpQuad(struct nuvec4_s *mid, struct nuvec4_s *a, struct nuvec4_s *b,
                                  struct nuvec4_s *c, struct nuvec4_s *d)
{
    struct nuvec4_s cd;

    NuVec4LerpVU0(mid, a, b, &fpatch_half);
    NuVec4LerpVU0(&cd, c, d, &fpatch_half);
    NuVec4LerpVU0(mid, &cd, mid, &fpatch_half);
}


void yy(s32 a, s32 b, s32 c, s32 d, s32 mid)
{
    fpatchLerpQuad(&hf_vtx[mid].pnt, &hf_vtx[a].pnt, &hf_vtx[b].pnt, &hf_vtx[c].pnt,
                   &hf_vtx[d].pnt);
    fpatchLerpQuad(&hf_vtx[mid].nrm, &hf_vtx[a].nrm, &hf_vtx[b].nrm, &hf_vtx[c].nrm,
                   &hf_vtx[d].nrm);
    fpatchLerpQuad(&hf_vtx[mid].col, &hf_vtx[a].col, &hf_vtx[b].col, &hf_vtx[c].col,
                   &hf_vtx[d].col);
}


static inline void fpatchLerpEdge(s32 a, s32 b, s32 mid)
{
    NuVec4LerpVU0(&hf_vtx[mid].pnt, &hf_vtx[a].pnt, &hf_vtx[b].pnt, &fpatch_half);
    NuVec4LerpVU0(&hf_vtx[mid].nrm, &hf_vtx[a].nrm, &hf_vtx[b].nrm, &fpatch_half);
    NuVec4LerpVU0(&hf_vtx[mid].col, &hf_vtx[a].col, &hf_vtx[b].col, &fpatch_half);
}

void NuFPatchMakeHeightFieldBuffer(struct nufpatch_s *patch)
{
    hf_vtx[0].pnt = patch->pnt[0];
    hf_vtx[8].pnt = patch->pnt[1];
    hf_vtx[72].pnt = patch->pnt[2];
    hf_vtx[80].pnt = patch->pnt[3];
    hf_vtx[0].nrm = patch->nrm[0];
    hf_vtx[8].nrm = patch->nrm[1];
    hf_vtx[72].nrm = patch->nrm[2];
    hf_vtx[80].nrm = patch->nrm[3];
    hf_vtx[0].col = *(struct nuvec4_s *)&patch->col._00;
    hf_vtx[8].col = *(struct nuvec4_s *)&patch->col._10;
    hf_vtx[72].col = *(struct nuvec4_s *)&patch->col._20;
    hf_vtx[80].col = *(struct nuvec4_s *)&patch->col._30;
    fpatchLerpEdge(0, 8, 4);
    fpatchLerpEdge(72, 80, 76);
    fpatchLerpEdge(0, 72, 36);
    fpatchLerpEdge(8, 80, 44);
    yy(0, 8, 72, 80, 40);
    fpatchLerpEdge(0, 4, 2);
    fpatchLerpEdge(36, 40, 38);
    fpatchLerpEdge(0, 36, 18);
    fpatchLerpEdge(4, 40, 22);
    yy(0, 4, 36, 40, 20);
    fpatchLerpEdge(4, 8, 6);
    fpatchLerpEdge(40, 44, 42);
    fpatchLerpEdge(8, 44, 26);
    yy(4, 8, 40, 44, 24);
    fpatchLerpEdge(72, 76, 74);
    fpatchLerpEdge(36, 72, 54);
    fpatchLerpEdge(40, 76, 58);
    yy(36, 40, 72, 76, 56);
    fpatchLerpEdge(76, 80, 78);
    fpatchLerpEdge(44, 80, 62);
    yy(40, 44, 76, 80, 60);
    yy(0, 2, 18, 20, 10);
    fpatchLerpEdge(0, 2, 1);
    fpatchLerpEdge(18, 20, 19);
    fpatchLerpEdge(0, 18, 9);
    fpatchLerpEdge(2, 20, 11);
    yy(2, 4, 20, 22, 12);
    fpatchLerpEdge(2, 4, 3);
    fpatchLerpEdge(20, 22, 21);
    fpatchLerpEdge(4, 22, 13);
    yy(18, 20, 36, 38, 28);
    fpatchLerpEdge(36, 38, 37);
    fpatchLerpEdge(18, 36, 27);
    fpatchLerpEdge(20, 38, 29);
    yy(20, 22, 38, 40, 30);
    fpatchLerpEdge(38, 40, 39);
    fpatchLerpEdge(22, 40, 31);
    yy(4, 6, 22, 24, 14);
    fpatchLerpEdge(4, 6, 5);
    fpatchLerpEdge(22, 24, 23);
    fpatchLerpEdge(6, 24, 15);
    yy(6, 8, 24, 26, 16);
    fpatchLerpEdge(6, 8, 7);
    fpatchLerpEdge(24, 26, 25);
    fpatchLerpEdge(8, 26, 17);
    yy(22, 24, 40, 42, 32);
    fpatchLerpEdge(40, 42, 41);
    fpatchLerpEdge(24, 42, 33);
    yy(24, 26, 42, 44, 34);
    fpatchLerpEdge(42, 44, 43);
    fpatchLerpEdge(26, 44, 35);
    yy(36, 38, 54, 56, 46);
    fpatchLerpEdge(54, 56, 55);
    fpatchLerpEdge(36, 54, 45);
    fpatchLerpEdge(38, 56, 47);
    yy(38, 40, 56, 58, 48);
    fpatchLerpEdge(56, 58, 57);
    fpatchLerpEdge(40, 58, 49);
    yy(54, 56, 72, 74, 64);
    fpatchLerpEdge(72, 74, 73);
    fpatchLerpEdge(54, 72, 63);
    fpatchLerpEdge(56, 74, 65);
    yy(56, 58, 74, 76, 66);
    fpatchLerpEdge(74, 76, 75);
    fpatchLerpEdge(58, 76, 67);
    yy(40, 42, 58, 60, 50);
    fpatchLerpEdge(58, 60, 59);
    fpatchLerpEdge(42, 60, 51);
    yy(42, 44, 60, 62, 52);
    fpatchLerpEdge(60, 62, 61);
    fpatchLerpEdge(44, 62, 53);
    yy(58, 60, 76, 78, 68);
    fpatchLerpEdge(76, 78, 77);
    fpatchLerpEdge(60, 78, 69);
    yy(60, 62, 78, 80, 70);
    fpatchLerpEdge(78, 80, 79);
    fpatchLerpEdge(62, 80, 71);
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
    dst->col._03 = hf_vtx[vtx].col.w;
    dst->col._13 = hf_vtx[vtx + 1].col.w;
    dst->col._23 = hf_vtx[vtx + 9].col.w;
    dst->col._33 = hf_vtx[vtx + 10].col.w;
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
        fpatch_stream[i]->cur.b = vpDmaTag_Call(fpatch_stream[i]->cur.b, fpatch_microcode);
        rndrstream_free = vpDmaTag_Next(fpatch_stream[i]->cur.b, 0);
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


void NuFPatchLightTexture(struct nutex_s *tex, s32 size, struct nuvec_s *lightdir, f32 range,
                          f32 scale, f32 bias)
{
    struct nuvec_s dir;
    u8 *texel;
    s32 x;
    s32 y;
    f32 dot;
    f32 light;
    u8 shade;

    tex->type = 3;
    tex->width = size;
    tex->height = size;
    tex->mmcnt = 0;
    tex->bits = NuMemAllocFn(size * size * 4, fpatch_file, 0x6D3);
    texel = tex->bits;
    for (y = 0; y < size; y++) {
        for (x = 0; x < size; x++) {
            dir.x = -(((range + range) * x) / (size - 1) - range);
            dir.y = -(((range + range) * y) / (size - 1) - range);
            dir.z = 1.0f;
            NuVecNorm(&dir, &dir);
            dot = NuVecDot(&dir, lightdir);
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
            shade = light;
            *texel++ = shade;
            *texel++ = shade;
            *texel++ = shade;
            *texel++ = shade;
        }
    }
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
