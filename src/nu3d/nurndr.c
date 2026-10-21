/*
 * Unit: nu3d/nurndr
 *
 * Functions:
 *   0x0011f030 NuRndrSetZBuffer
 *   0x0011f1b8 NuRndrSetRTarget
 *   0x0011f3d0 NuRndrClear
 *   0x0011f870 NuRndrGradClear
 *   0x0011fe28 NuRndrGScnObj
 *   0x00120240 NuRndrGobj
 *   0x00120590 NuRndrFxBlueAndGreenData
 *   0x001207d0 NuRndrFxSpriteSet
 *   0x00120a78 NuRndrFxSprite2Set
 *   0x00120e60 NuRndrFxCode
 *   0x00121220 NuRndrGobjSkin
 *   0x00121498 NuRndrShadowInit
 *   0x00121628 NuRndrStreamQuad3dClip
 *   0x00121988 NuRndrOTInit
 *   0x00121a98 NuRndrOTInsertTriBatch
 *   0x00121ee0 NuRndrOTFlush
 *   0x00122068 NuRndrStreamTriStrip3dClip
 *   0x00122458 NuRndrStreamTri3dClip
 *   0x00122850 NuRndrCylCapMakeData
 *   0x00122b88 NuRndrCylMakeData
 *   0x00122da8 NuRndrShadowCylinderCap
 *   0x00123210 NuRndrShadowEllipse
 *   0x00123660 NuRndrShadowPols
 *   0x001241b8 NuRndrShadowCylinderCap2
 *   0x00124f40 NuRndrShadowStack
 *   0x00125180 NuHGobjRndrMtxDwa
 *   0x00125608 NuHGobjEval
 *   0x00125918 NuHGobjEvalAnim
 *   0x00125c10 NuHGobjEvalAnim2
 *   0x00125f30 NuHGobjRndrAnim
 *   0x00126558 NuHGobjEvalDwa
 *   0x00126758 NuHGobjEvalDwa2
 *   0x00126900 NuHGobjEvalDwaBlend
 *   0x00126c28 NuHGobjEvalDwaBlend2
 *   0x00126ee0 NuHGobjEvalAnimBlend
 *   0x001271e8 NuHGobjEvalAnimBlend2
 *   0x00127530 NuHGobjRndrAnimBlend
 *   0x001279b0 NuRndrLine2d
 *   0x00127c08 NuRndrLineStrip2d
 *   0x00127e70 ClipLinePoint
 *   0x00128138 NuRndrLine3d
 *   0x00128578 NuRndrTri2d
 *   0x001287d0 NuRndrTriStrip2d
 *   0x00128a38 NuRndrTriStrip3d
 *   0x00128c48 NuRndrRect2di
 *   0x00128f40 NuRndrGradRect2di
 *   0x001293c0 NuRndrRectUV2di
 *   0x001296e8 NuRndrLine2di
 *   0x00129940 NuRndrLineRect2di
 *   0x00129ca0 NuRndrTri2di
 *   0x00129f68 NuRndrTriStrip2di
 *   0x0012a218 NuRndrLineStrip2di
 *   0x0012a4c8 NuRndrQuad2d
 *   0x0012a730 NuRndrTriStripGif
 *   0x0012a9c0 NuRndrOldFaceOn
 *   0x0012ae38 NuRndrFaceOn
 *   0x0012b270 NuRndrFaceY
 *   0x0012b610 NuRndrRectUV3di
 *   0x0012b870 NuRndrRectUVStream
 *   0x0012bac0 NuRndrWater
 *   0x0012cbf8 NuRndrShadow
 *   0x0012d238 NuRndrShadowLightFan
 *   0x0012d5b0 NuRndrShadowStencil
 *   0x0012db68 NuRndrTrail
 *   0x0012e200 NuRndrAddFootPrint
 *   0x0012e560 NuRndrFootPrints
 *   0x0012e810 NuRndrStreamQuadShad
 *   0x0012ee58 NuRndrWaterRippleUpdate
 *   0x0012ef88 NuRndrStreamQuadWRip
 *   0x0012f640 NuHGobjRndr
 *   0x0012f6c0 NuHGobjRndrMtx
 *   0x0012f6e0 NuRndrInitEx
 *   0x0012f770 NuRndrInit
 *   0x0012f800 NuRndrClose
 *   0x0012f808 NuRndrBeginScene
 *   0x0012f8c8 NuRndrBeginSceneEx
 *   0x0012f990 NuRndrEndScene
 *   0x0012f9c0 NuRndrEndSceneEx
 *   0x0012fa10 NuRndrSwapScreen
 *   0x0012fa98 NuRndrGlobalMtl
 *   0x0012faa0 NuRndrFogColour
 *   0x0012faa8 NuRndrFogDistance
 *   0x0012fab0 NuRndrFogMode
 *   0x0012fab8 NuRndrParticleGroup
 *   0x0012fc68 NuRndrStarfield
 *   0x0012fc98 NuRndrRect
 *   0x0012fdf0 NuRndrLine3dDbg
 *   0x0012fdf8 NuRndrLine3dDbgFlush
 *   0x0012fe00 NuRndrTri3dClip
 *   0x0012fe28 NuRndrTriStrip3dClip
 *   0x0012fe50 NuRndrQuad3d
 *   0x0012fe58 NuRndrFlush
 *   0x0012fea8 NuRndrShadowOnOff
 *   0x0012feb0 NuRndrRectUV3diNS
 *   0x00130000 NuRndrFx
 *   0x00130078 NuRndrShadowDirCol
 *   0x001300b8 NuRndrWaterLevelTint
 *   0x001300c8 NuRndrAddShadow
 *   0x001301a0 NuRndrShadPolys
 *   0x00130230 NuRndrAddWaterRipple
 *   0x001302a8 NuRndrWaterRip
 *   0x001303a0 NuRndrInitWorld
 *   0x001303d0 NuRndrSplineSearch
 *   0x00130418 NuRndrCreateBlendShapeDeformerWeightsArray
 *   0x001304a0 NuRndrDeformerWeightsArrayInit
 *   0x001304e8 NuRndrFxBlueOrGreenData
 *   0x00130638 NuRndrOTInsertItem
 *   0x00130690 OTTriInit
 *   0x001306c0 OTTriInsert
 *   0x00130740 GlobalCam3DNClip
 *   0x001307c0 RotateCyl
 *   0x00130888 DeRotateVectorAngles
 *   0x00130920 NuRndrTest
 *   0x00130928 NuRndrAnglesZX
 */

#include "nu_libc.h"
#include "nu_types.h"

#include "nu3d/nuanim.h"
#include "nu3d/nucamera.h"
#include "nu3d/nugobj.h"
#include "nu3d/nuhgobj.h"
#include "nu3d/nugscn.h"
#include "nu3d/numtl.h"
#include "nu3d/nurndr.h"
#include "nu3d/nustream.h"
#include "nu3d/nutex.h"
#include "nu3d/nuvport.h"
#include "nucore/nuerror.h"
#include "numath/numathtypes.h"
#include "numath/nufloat.h"
#include "numath/numtx.h"
#include "numath/nutrig.h"
#include "numath/nuvec.h"
#include "numath/nuvec4.h"
#include "nups2/ps2txtr.h"
#include "nups2/nups2types.h"

typedef struct GsFrameReg {
    u64 fbp : 9;
    u64 pad09 : 7;
    u64 fbw : 6;
    u64 pad16 : 2;
    u64 psm : 6;
    u64 pad1e : 2;
    u64 fbmsk : 32;
} GsFrameReg;

typedef union GifTagQuad {
    struct GifTag tag;
    u64 dword[2];
    u32 word[4];
} GifTagQuad;

typedef struct GsTestReg {
    u64 ate : 1;
    u64 atst : 3;
    u64 aref : 8;
    u64 afail : 2;
    u64 date : 1;
    u64 datm : 1;
    u64 zte : 1;
    u64 ztst : 2;
    u64 pad13 : 45;
} GsTestReg;

extern struct numtl_s *D_00633078;
#define GlobalMtl D_00633078

extern int nurndr_shadowson;

extern struct numtl_s *numtl_defaultmtl3d;

extern float NuRndrWaterLevel;
extern u32 NuRndrWaterTint;
extern int NuRndrShadMaskCount;

typedef struct nurndrfoot_s {
    struct nuvec_s pnts[4];
    short brightness;
    char timer;
    char gfx;
    u32 colour;
} nurndrfoot_s;

extern int D_002D4110[4][2];
#define oldfaceon_uvindex D_002D4110
extern int D_002D4130[4][2];
#define faceon_uvindex D_002D4130
extern void NuVec4ScaleXYZVU0(struct nuvec4_s *dst, struct nuvec4_s *src, float scale);
extern void NuVecConvertToIntVU0(void *dst, struct nuvec_s *src);
extern int D_002D4150[4][2];
#define facey_uvindex D_002D4150

extern nurndrfoot_s NuRndrFootData[];
extern int NuRndrFootFree;
extern u32 testfootcol;

extern char D_00614770[];
#define nurndr_file D_00614770
extern char D_00614790[];

extern int NuHGobjRndrMtxDwa(struct NUHGOBJ_s *hobj, struct numtx_s *wm, int nlayers, short *layer,
                             struct numtx_s *mtx_array, float **dwa);
extern int D_0062EC78;
#define hgobj_enabled D_0062EC78
extern void NuMtxMulArrayVU0(struct numtx_s *dst, struct numtx_s *a, struct numtx_s *b, int count);
extern struct numtx_s D_00681A40[];
#define hgobj_anim_mtx D_00681A40
extern struct numtx_s D_00685A40[];
#define hgobj_blend_mtx D_00685A40
extern void NuHGobjEval(struct NUHGOBJ_s *hobj, int njanims, struct NUJOINTANIM_s *janim, struct numtx_s *mtx_array);
extern void NuMtxSetRotateXYZVU0(struct numtx_s *m, struct nuangvec_s *a);
extern void NuMtxMulVU0(struct numtx_s *dst, struct numtx_s *a, struct numtx_s *b);
extern u32 *NuRndrCreateBlendShapeDeformerWeightsArray(u32 count);
extern struct numtx_s D_0067DA40[];
#define hgobj_joint_mtx D_0067DA40
extern int NuRndrStreamTri3dClip(struct nuvtx_tc1_s *verts, int count, struct numtx_s *mtx,
                                 struct nurndrstream_s *stream);
extern int NuRndrStreamTriStrip3dClip(struct nuvtx_tc1_s *verts, int count, struct numtx_s *mtx,
                                      struct nurndrstream_s *stream);
extern void NuRndrOTFlush(void);
extern void NuFntInit(void);
extern void NuRndrDeformerWeightsArrayInit(void);
extern void NuRndrOTInit(void);
extern u32 *NuRndrCylMakeData(void);
extern void NuRndrShadowDirCol(struct nuvec_s *dir, u32 col);
extern u32 *NRCylCapData;
extern u32 *NRCylData;

struct nurndrshadcyl_s {
    union {
        struct nuvec_s pos;
        int flags;
    };
    struct nuvec_s edge0;
    struct nuvec_s edge1;
    char pad24[4];
    struct nuvec_s edge2;
    char pad34[0xC];
};

struct nurndrshadface_s {
    struct nuvec_s v;
    int count;
};

extern int testdebug;
extern void NuVec4MtxTransformVU0(struct nuvec4_s *dst, struct nuvec4_s *src, struct numtx_s *m);
extern void *NuScratchAlloc32(u32 size);
extern void NuScratchRelease(void);

struct nurndrshadellipse_s {
    struct nuvec_s pos;
    struct nuvec_s axis0;
    struct nuvec_s axis1;
    struct nuvec_s axis2;
};
extern int NuRndrShadowCnt;
extern struct numtx_s D_00689A40;
#define rndr_identity_mtx D_00689A40
extern struct nuvec_s D_00614760;
#define default_light_dir D_00614760

extern void NuLgtArcLaserDraw(int mode);
extern void NuRndrShadowStencil(int paused, struct nuvec_s *pos);
extern void NuLightFxSetup(void);
extern void NuLightProcessSpot(int mode);
extern void NuLightFogDraw(int mode);
extern void NuRndrShadow(int mode);
extern void NuRndrWater(void);
extern int txupload_on;

extern struct nucamera_s global_camera;
extern float NuFsqrt(float x);

typedef struct nurndrwaterrip_s {
    struct nuvec_s pos;
    float startsize;
    float endsize;
    float size;
    u32 colour;
    u32 curcolour;
    short timer;
    short duration;
} nurndrwaterrip_s;

extern nurndrwaterrip_s NuRndrWaterRipDat[];
extern int NuRndrWaterRipCnt;


typedef struct nurndrotitem_s {
    void *dma;                      /* DMA chain to call */
    struct numtx_s *mtx;
    struct numtl_s *mtl;
    struct nurndrotitem_s *next;
    float depth;
    int mtxmode;
    char pad18[4];
} nurndrotitem_s;

typedef struct nurndrotbin_s {
    char pad00[0xC];
    nurndrotitem_s *head;
    char pad10[0xC];
} nurndrotbin_s;

typedef struct ottri_s {
    struct ottri_s *next;
    struct nurndrottri_s *tri;
} ottri_s;

extern int D_0062EC70;
#define first_time D_0062EC70
extern nurndrotitem_s D_006A5AA0[];
#define ot_item_pool D_006A5AA0

struct nurndrottri_s {
    struct DmaTag tag;
    u128 data[9];
};

extern float XFormVuTriZ(float *depth, struct nurndrottri_s *tri, struct numtx_s *mtx, int count);
extern u32 *vpDmaTag_Ret(u32 *packet);

extern float otscale;
extern nurndrotbin_s D_00689AA0[];
#define ot_bins D_00689AA0

extern ottri_s *ot_tri[];
extern ottri_s ot_tribuff[];
extern ottri_s *ot_free;
extern int ot_highest;
extern float NuTrigTable[];

extern struct nuvec_s curlight;
extern u32 curlightcol;
extern u32 *NRFanData;
extern u32 *NuRndrShadowLightFan(void);
extern int testflipold;
extern float testmultl;
extern GsFrameReg cur_rt;
extern int NuLgtHSurf[18];
extern struct numtl_s *NuLgtSurf[18];
extern struct numtl_s *NuLgtGreyNZMat;
extern struct numtl_s *NuLightAddMat;
extern struct nuvec_s NuRndrShadBox[2];
extern int D_00633070;
#define shadow_wobble_ang D_00633070
extern int D_0063306C;
#define shadow_spin_ang D_0063306C
extern struct nuvec_s lightfan[];
extern u32 *nurndr_lightfan;
extern u32 D_006ACF00[];
#define lightfan_packet D_006ACF00

extern struct nugspline_s *NuRndrShadMasks[];
extern unsigned char D_0062ECA8[];

extern unsigned int D_00689A80[];
#define default_skin_weights D_00689A80
extern u32 *vpDmaTag_RetEx(void *buf, u32 a1, u32 a2);
extern u32 *vpDmaTag_Close(void *p);
extern u32 *vpDmaTag_RefEx(void *packet, void *addr, int qwc, u32 vif1);
extern u32 D_006ACAA0[];
#define ot_open_packet D_006ACAA0
extern u32 D_006ACAE0[];
#define ot_close_packet D_006ACAE0
extern int D_0063307C;
#define ot_batch_free D_0063307C
extern u32 D_006ACB00[];
#define cylcap_packet D_006ACB00
extern u32 D_006ACD00[];
#define cyl_packet D_006ACD00


struct nutribatch_s {
    int count;
    int pad04[3];
    u128 tris[1];
};

extern struct nurndrstream_s *rndrstream_2d;
extern struct nurndrstream_s *rndrstream_3d;
extern int nurndr_pixel_width;
extern int nurndr_pixel_height;
extern u32 *rndrstream_free;
extern struct numtl_s *numtl_defaultmtl2d;
extern int PS2_REZ_W;
extern int PS2_REZ_H;
extern int PS2_VCNTR_X;
extern int PS2_VCNTR_Y;
extern int tri2dz;

extern void NuRndrStreamLink(struct nurndrstream_s *stream);
extern struct nurndrstream_s *rndrstream_system;
extern int nurndr_half_offset;
extern void NuPs2DisableAutoScreenSwap(void);
extern int NuPs2ScreenSwap(int vsync);
extern void NuPs2FlushVRam(int a, int b, int c);
extern int debug_flush_vram;
extern int nutex_total_xfer_this_frame;
extern int nutex_peak_xfer_per_frame;
extern int nurndr_tritot_this_frame;
extern struct nurndrstream_s *rndrstream_fx;
extern struct numtx_s numtx_identity;
extern int NuRndrStreamQuadShad(struct nurndrstream_s *stream);
extern int NuRndrStreamQuadWRip(struct nurndrstream_s *stream, short *visible);
typedef struct nurndrshadpol_s {
    struct nuvec_s pos;
    float size;
    short shade;
    short xrot;
    short yrot;
    short zrot;
} nurndrshadpol_s;

extern nurndrshadpol_s NuRndrShadPolDat[];




extern u32 *vpDmaTag_CntEx(u32 *packet, u32 vif0, u32 vif1);
extern u32 *vpDmaTag_Next(u32 *packet, u32 address);
extern u32 *vtBeginDirect(u32 *packet, u32 irq);
extern u32 *vtCloseDirect(u32 *packet);
extern u32 *gsBeginGifPacket(u32 *packet, int nloop, int eop, int pre,
                             int prim, int flg, int nreg, ...);
extern u32 *gsCloseGifPacket(u32 *packet);
extern u32 *NuRndrFxBlueAndGreenData(u32 code, void *fx);
extern u32 *vpDmaTag_CallEx(u32 *packet, void *addr, u32 vif0, u32 vif1);
extern u32 *vpDmaTag_Copy(u32 *packet, struct DmaTag *tag);
extern u32 *vpDmaTag_RepointCopy(u32 *packet, struct DmaTag *tag, u32 addr);
extern int skinskipix;
extern u32 *D_00633074;
#define skin_mtx_buffer D_00633074
extern char D_00614820[];
#define err_skin_no_mtl D_00614820

extern int shadstorecnt;

struct nurndrshadstore_s {
    struct numtx_s mtx;
    struct nushadowdata_s shadow;
};

extern struct nurndrshadstore_s shadstore[256];
extern int testcntp;
extern int testdone;
extern int testdone2;
extern struct numtl_s *NuLightAddMat2;
extern void NuCameraSet(struct nucamera_s *cam);
extern unsigned char D_002D3F50[];
#define quad_adc_flags D_002D3F50
extern unsigned char D_002D4008[];
#define tri_adc_flags D_002D4008

extern void NuPs2GetCurrentBackBuffer(GsFrameReg *reg);
extern void NuPs2GetCurrentFrontBuffer(GsFrameReg *reg);
extern void NuPs2GetCurrentZBuffer(struct GsZbufReg *reg);
extern u32 *NuPs2CopyCurrentBackBuffer(u32 *packet);
extern u32 *NuPs2CopyCurrentFrontBuffer(u32 *packet);
extern int nurndr_pixel_p2w;
extern int nurndr_pixel_p2h;
extern u32 *NuPs2CopyCurrentZBuffer(u32 *packet);

extern int D_0062EC60;
#define nurndr_gscnobj_enable D_0062EC60
extern int D_0062EC5C;
#define nurndr_debug_geom D_0062EC5C
extern char D_006147A8[];
#define err_geom_no_mtl D_006147A8
extern char D_006147C8[];
#define err_fgeom_no_mtl D_006147C8
extern struct numtl_s *nurndr_forced_mtl;
extern struct numtl_s **nurndr_forced_mtl_table;
extern void NuRndrOTInsertTriBatch(struct numtx_s *mtx, int clip, struct numtl_s *mtl, struct nurndrottri_s *tris,
                                   int count);
extern u32 *vpDmaTag_Call(u32 *packet, void *addr);
extern struct NuPs2GsReg D_002EA1A0;
#define NuPs2ZBufReg0 D_002EA1A0
extern u32 *vpDmaTag_Cnt(u32 *packet);

void NuRndrSetZBuffer(struct nurndrstream_s *stream, int zbuffer) {
    GsFrameReg frame;

    if (zbuffer <= 0) {
        NuRndrStreamClearZBState();
        stream->cur.w = vpDmaTag_CntEx(stream->cur.w, 0x11000000, 0x50000000);
        ((unsigned char *)stream->cur.w)[-14] = 0xB;
        stream->cur.w--;
        stream->cur.w = vtBeginDirect(stream->cur.w, 0);
        stream->cur.w = gsBeginGifPacket(stream->cur.w, 0, 1, 0, 0, 0, 1, 0xE);
        switch (zbuffer) {
        default:
            NuPs2GetCurrentBackBuffer(&frame);
            NuRndrStreamSetZBState(frame.fbp << 5, frame.psm, 0);
            stream->cur.w = NuRndrStreamAddZBState(stream->cur.w, 1);
            break;
        case -3:
            NuPs2GetCurrentFrontBuffer(&frame);
            NuRndrStreamSetZBState(frame.fbp << 5, frame.psm, 0);
            stream->cur.w = NuRndrStreamAddZBState(stream->cur.w, 1);
            break;
        case -2:
            stream->cur.w = NuPs2CopyCurrentZBuffer(stream->cur.w);
            break;
        }
        stream->cur.w = gsCloseGifPacket(stream->cur.w);
        stream->cur.w = vtCloseDirect(stream->cur.w);
        stream->cur.w = vpDmaTag_Close(stream->cur.w);
    } else {
        stream->cur.w = NuTexSetZBuffer(stream->cur.w, zbuffer);
    }
}


void NuRndrSetRTarget(struct nurndrstream_s *stream, int target) {
    GsFrameReg frame;
    struct GsZbufReg zbuf;

    if (target <= 0) {
        stream->cur.w = vpDmaTag_CntEx(stream->cur.w, 0x13000000, 0x50000000);
        ((unsigned char *)stream->cur.w)[-14] = 9;
        stream->cur.w--;
        stream->cur.w = vtBeginDirect(stream->cur.w, 0);
        stream->cur.w = gsBeginGifPacket(stream->cur.w, 0, 1, 0, 0, 0, 1, 0xE);
        switch (target) {
        default:
            stream->cur.w = NuPs2CopyCurrentBackBuffer(stream->cur.w);
            break;
        case -3:
            stream->cur.w = NuPs2CopyCurrentFrontBuffer(stream->cur.w);
            break;
        case -2:
            NuPs2GetCurrentBackBuffer(&frame);
            NuPs2GetCurrentZBuffer(&zbuf);
            frame.fbp = zbuf.zbp;
            frame.psm = zbuf.psm | 0x30;
            *((GsFrameReg *)stream->cur.w)++ = frame;
            *((u64 *)stream->cur.w)++ = 0x4C;
            break;
        case -4:
            NuPs2GetCurrentBackBuffer(&frame);
            NuPs2GetCurrentZBuffer(&zbuf);
            frame.fbp = zbuf.zbp;
            frame.psm = 0x30;
            *((GsFrameReg *)stream->cur.w)++ = frame;
            *((u64 *)stream->cur.w)++ = 0x4C;
            break;
        }
        stream->cur.w = gsCloseGifPacket(stream->cur.w);
        stream->cur.w = vtCloseDirect(stream->cur.w);
        stream->cur.w = vpDmaTag_Close(stream->cur.w);
        nurndr_pixel_width = PS2_REZ_W;
        nurndr_pixel_height = PS2_REZ_H;
        nurndr_pixel_p2w = Ps2TxValToPowerOfTwo(PS2_REZ_W);
        nurndr_pixel_p2h = Ps2TxValToPowerOfTwo(PS2_REZ_H);
    } else {
        stream->cur.w = NuTexSetRenderTarget(stream->cur.w, target);
    }
}


void NuRndrClear(int flags, int colour, float depth) {
    struct nurndrstream_s *stream;
    u16 *scissor;
    int *offset;
    int z;
    int x;
    int xmax;
    int y0;
    int y1;

    if (flags & 4) {
        NuRndrStreamFlush();
    } else {
        NuMtlBuildRenderList();
        NuRndrOTFlush();
        NuRndrStreamFlush();
    }
    stream = rndrstream_system;
    NuRndrStreamLink(stream);
    NuRndrStreamSetMisc(stream);
    scissor = NuRndrStreamGetScissor();
    offset = NuRndrStreamGetOffset();
    stream->cur.w = vpDmaTag_CntEx(stream->cur.w, 0, 0x50000000) - 1;
    stream->cur.w = vtBeginDirect(stream->cur.w, 0);
    stream->cur.w = gsBeginGifPacket(stream->cur.w, 0, 1, 0, 0, 0, 1, 0xE);
    *((u64 *)stream->cur.w)++ = NuPs2ZBufReg0.zbuf.zbp | (NuPs2ZBufReg0.zbuf.psm << 24);
    *((u64 *)stream->cur.w)++ = 0x4E;
    stream->cur.w = gsCloseGifPacket(stream->cur.w);
    stream->cur.w = vtCloseDirect(stream->cur.w);
    stream->cur.w = vpDmaTag_Close(stream->cur.w);
    stream->cur.w = NuRndrStreamAddZBState(stream->cur.w, 1);
    stream->cur.w = vpDmaTag_CntEx(stream->cur.w, 0, 0) - 1;
    stream->cur.w = vtBeginDirect(stream->cur.w, 0);
    stream->cur.w = gsBeginGifPacket(stream->cur.w, 0, 1, 0, 0, 0, 1, 0xE);
    *((u64 *)stream->cur.w)++ = 0;
    *((u64 *)stream->cur.w)++ = 0x1B;
    *(u64 *)stream->cur.w = 0;
    switch (flags & 3) {
    case 1:
        ((GsTestReg *)stream->cur.w)->ate = 1;
        ((GsTestReg *)stream->cur.w)->atst = 0;
        ((GsTestReg *)stream->cur.w)->afail = 1;
        break;
    case 2:
        ((GsTestReg *)stream->cur.w)->ate = 1;
        ((GsTestReg *)stream->cur.w)->atst = 0;
        ((GsTestReg *)stream->cur.w)->afail = 2;
        break;
    case 3:
    default:
        ((GsTestReg *)stream->cur.w)->ate = 0;
        break;
    }
    ((GsTestReg *)stream->cur.w)->date = 0;
    ((GsTestReg *)stream->cur.w)->zte = 1;
    ((GsTestReg *)stream->cur.w)->ztst = 1;
    ((u64 *)stream->cur.w)++;
    *((u64 *)stream->cur.w)++ = 0x47;
    stream->cur.w = gsCloseGifPacket(stream->cur.w);
    z = (1.0f - depth) * 8388607.0f;
    stream->cur.w = gsBeginGifPacket(stream->cur.w, 0, 1, 1, 6, 0, 3, 1, 5, 5);
    x = scissor[0] * 16 + offset[0];
    xmax = scissor[1] * 16 + offset[0] + 16;
    y0 = scissor[2] * 16 + offset[1];
    y1 = scissor[3] * 16 + offset[1] + 16;
    do {
        *stream->cur.w++ = colour & 0xFF;
        *stream->cur.w++ = (colour >> 8) & 0xFF;
        *stream->cur.w++ = (colour >> 16) & 0xFF;
        *stream->cur.w++ = (colour >> 24) & 0xFF;
        *stream->cur.w++ = x;
        *stream->cur.w++ = y0;
        *stream->cur.w++ = z;
        *stream->cur.w++ = 0;
        *stream->cur.w++ = (x + 0x400 < xmax) ? x + 0x400 : xmax;
        *stream->cur.w++ = y1;
        *stream->cur.w++ = z;
        *stream->cur.w++ = 0;
        x += 0x400;
    } while (x < xmax);
    stream->cur.w = gsCloseGifPacket(stream->cur.w);
    stream->cur.w = vtCloseDirect(stream->cur.w);
    stream->cur.w = vpDmaTag_Close(stream->cur.w);
    rndrstream_free = vpDmaTag_Next(stream->cur.w, 0);
}


void NuRndrGradClear(int mode, int topcolour, int bottomcolour, float depth) {
    struct nurndrstream_s *stream;
    int z;

    NuMtlBuildRenderList();
    NuRndrOTFlush();
    NuRndrStreamFlush();
    stream = rndrstream_system;
    NuRndrStreamLink(stream);
    NuRndrStreamSetMisc(stream);
    stream->cur.w = vpDmaTag_CntEx(stream->cur.w, 0, 0x50000000) - 1;
    stream->cur.w = vtBeginDirect(stream->cur.w, 0);
    stream->cur.w = gsBeginGifPacket(stream->cur.w, 0, 1, 0, 0, 0, 1, 0xE);
    *((u64 *)stream->cur.w)++ = NuPs2ZBufReg0.zbuf.zbp | (NuPs2ZBufReg0.zbuf.psm << 24);
    *((u64 *)stream->cur.w)++ = 0x4E;
    stream->cur.w = gsCloseGifPacket(stream->cur.w);
    stream->cur.w = vtCloseDirect(stream->cur.w);
    stream->cur.w = vpDmaTag_Close(stream->cur.w);
    stream->cur.w = NuRndrStreamAddZBState(stream->cur.w, 1);
    stream->cur.w = vpDmaTag_Cnt(stream->cur.w);
    ((unsigned char *)stream->cur.w)[-14] = 0x10;
    *stream->cur.w++ = 0;
    *stream->cur.w++ = 0;
    *stream->cur.w++ = 0x10000000;
    *stream->cur.w++ = 0x5000000C;
    *((u64 *)stream->cur.w)++ = 0x2002000000000001ULL;
    *((u64 *)stream->cur.w)++ = 0xEEE;
    *((u64 *)stream->cur.w)++ = 8;
    *((u64 *)stream->cur.w)++ = 0x1B;
    *(u64 *)stream->cur.w = 0;
    switch (mode) {
    case 1:
        ((GsTestReg *)stream->cur.w)->ate = 1;
        ((GsTestReg *)stream->cur.w)->atst = 0;
        ((GsTestReg *)stream->cur.w)->afail = 1;
        break;
    case 2:
        ((GsTestReg *)stream->cur.w)->ate = 1;
        ((GsTestReg *)stream->cur.w)->atst = 0;
        ((GsTestReg *)stream->cur.w)->afail = 2;
        break;
    case 3:
    default:
        ((GsTestReg *)stream->cur.w)->ate = 0;
        break;
    }
    ((GsTestReg *)stream->cur.w)->date = 0;
    ((GsTestReg *)stream->cur.w)->zte = 1;
    ((GsTestReg *)stream->cur.w)->ztst = 1;
    ((u64 *)stream->cur.w)++;
    *((u64 *)stream->cur.w)++ = 0x47;
    z = (1.0f - depth) * 8388607.0f;
    *((u64 *)stream->cur.w)++ = 0x2002400000008004ULL;
    *((u64 *)stream->cur.w)++ = 0x51;
    *stream->cur.w++ = topcolour & 0xFF;
    *stream->cur.w++ = (topcolour >> 8) & 0xFF;
    *stream->cur.w++ = (topcolour >> 16) & 0xFF;
    *stream->cur.w++ = 0x80;
    *stream->cur.w++ = (int)NuVpPixelWidth(0.0f);
    *stream->cur.w++ = (int)NuVpPixelHeight(0.0f);
    *stream->cur.w++ = z;
    *stream->cur.w++ = 0;
    *stream->cur.w++ = topcolour & 0xFF;
    *stream->cur.w++ = (topcolour >> 8) & 0xFF;
    *stream->cur.w++ = (topcolour >> 16) & 0xFF;
    *stream->cur.w++ = 0x80;
    *stream->cur.w++ = (int)NuVpPixelWidth(1.0f);
    *stream->cur.w++ = (int)NuVpPixelHeight(0.0f);
    *stream->cur.w++ = z;
    *stream->cur.w++ = 0;
    *stream->cur.w++ = bottomcolour & 0xFF;
    *stream->cur.w++ = (bottomcolour >> 8) & 0xFF;
    *stream->cur.w++ = (bottomcolour >> 16) & 0xFF;
    *stream->cur.w++ = 0x80;
    *stream->cur.w++ = (int)NuVpPixelWidth(0.0f);
    *stream->cur.w++ = (int)NuVpPixelHeight(1.0f);
    *stream->cur.w++ = z;
    *stream->cur.w++ = 0;
    *stream->cur.w++ = bottomcolour & 0xFF;
    *stream->cur.w++ = (bottomcolour >> 8) & 0xFF;
    *stream->cur.w++ = (bottomcolour >> 16) & 0xFF;
    *stream->cur.w++ = 0x80;
    *stream->cur.w++ = (int)NuVpPixelWidth(1.0f);
    *stream->cur.w++ = (int)NuVpPixelHeight(1.0f);
    *stream->cur.w++ = z;
    *stream->cur.w++ = 0;
    stream->cur.w = vpDmaTag_Close(stream->cur.w);
    rndrstream_free = vpDmaTag_Next(stream->cur.w, 0);
}


int NuRndrGScnObj(struct nugobj_s *gobj, struct numtx_s *wm) {
    struct numtx_s mtx;
    struct numtx_s offset;
    int clip;
    int instanced;
    int n;
    int vis;
    int geomidx;
    struct nurndrstream_s *stream;
    struct numtl_s *lastmtl;
    struct numtl_s *mtl;
    struct nugeom_s *geom;
    struct nufaceongeom_s *fgeom;

    clip = -1;
    if (!nurndr_gscnobj_enable) {
        return 1;
    }
    n = gobj->ngobjs;
    instanced = n > 1;
    stream = 0;
    lastmtl = 0;
    while (n-- != 0) {
        if (instanced) {
            NuMtxSetIdentity(&offset);
            offset._30 = gobj->origin.x;
            offset._31 = gobj->origin.y;
            offset._32 = gobj->origin.z;
            NuMtxMul(&mtx, &offset, wm);
            vis = NuCameraClipTestExtents(&gobj->min, &gobj->max, &mtx);
        } else {
            vis = NuCameraClipTestExtents(&gobj->min, &gobj->max, wm);
        }
        if (clip == -1) {
            clip = vis;
        } else if ((clip == 1 && vis != 1) || (clip == 0 && vis != 0)) {
            clip = 2;
        }
        if (vis) {
            nurndr_tritot_this_frame += gobj->tricount;
            if (instanced) {
                NuRndrSetLocalMtx(&mtx, vis);
            } else {
                NuRndrSetLocalMtx(wm, vis);
            }
            geom = gobj->geom;
            if (geom) {
                geomidx = -1;
                do {
                geomidx++;
                if (nurndr_debug_geom && geomidx != nurndr_debug_geom) {
                    continue;
                }
                mtl = nurndr_forced_mtl;
                if (!mtl) {
                    mtl = geom->mtl;
                    if (!mtl) {
                        NuDebugMsgProlog(nurndr_file, 0x62B)(err_geom_no_mtl);
                    }
                }
                if (nurndr_forced_mtl_table && mtl->special_id) {
                    if (nurndr_forced_mtl_table[mtl->special_id]) {
                        mtl = nurndr_forced_mtl_table[mtl->special_id];
                    }
                }
                do {
                    if (mtl != lastmtl) {
                        rndrstream_3d = &mtl->rs;
                        lastmtl = mtl;
                        if (stream != rndrstream_3d) {
                            if (stream) {
                                rndrstream_free = vpDmaTag_Next(stream->cur.w, 0);
                            }
                            stream = rndrstream_3d;
                            NuRndrStreamLink(stream);
                            NuRndrStreamSetMisc(rndrstream_3d);
                            NuRndrStreamSetViewMtx(rndrstream_3d);
                            NuRndrStreamSetLocalMtx(rndrstream_3d);
                        }
                    }
                    if (geom->dmastream) {
                        rndrstream_3d->cur.w = vpDmaTag_Call(rndrstream_3d->cur.w, geom->dmastream);
                        ((unsigned char *)rndrstream_3d->cur.w)[-14] = 4;
                    }
                    if (geom->otbatch) {
                        rndrstream_free = vpDmaTag_Next(rndrstream_3d->cur.w, 0);
                        if (instanced) {
                            NuRndrOTInsertTriBatch(&mtx, vis, mtl, geom->otbatch->tris, geom->otbatch->count);
                        } else {
                            NuRndrOTInsertTriBatch(wm, vis, mtl, geom->otbatch->tris, geom->otbatch->count);
                        }
                        NuRndrStreamLink(rndrstream_3d);
                    }
                    mtl = mtl->next;
                } while (mtl);
                } while ((geom = geom->next));
            }
            for (fgeom = gobj->faceon_geom; fgeom; fgeom = fgeom->next) {
                mtl = fgeom->mtl;
                if (!mtl) {
                    NuDebugMsgProlog(nurndr_file, 0x666)(err_fgeom_no_mtl);
                }
                if (mtl != lastmtl) {
                    rndrstream_3d = &mtl->rs;
                    lastmtl = mtl;
                    if (stream != rndrstream_3d) {
                        if (stream) {
                            rndrstream_free = vpDmaTag_Next(stream->cur.w, 0);
                        }
                        stream = rndrstream_3d;
                        NuRndrStreamLink(stream);
                        NuRndrStreamSetMisc(rndrstream_3d);
                        NuRndrStreamSetViewMtx(rndrstream_3d);
                        if (!fgeom->faceon_type) {
                            NuRndrStreamSetLocalMtxEx(rndrstream_3d, 1);
                        } else {
                            NuRndrStreamSetLocalMtxEx(rndrstream_3d, 2);
                        }
                    }
                }
                if (fgeom->dmastream) {
                    rndrstream_3d->cur.w = vpDmaTag_Call(rndrstream_3d->cur.w, fgeom->dmastream);
                    ((unsigned char *)rndrstream_3d->cur.w)[-14] = 4;
                }
            }
            if (stream) {
                rndrstream_free = vpDmaTag_Next(rndrstream_3d->cur.w, 0);
                stream = 0;
                lastmtl = 0;
            }
        }
        gobj++;
    }
    return clip;
}


int NuRndrGobj(struct nugobj_s *gobj, struct numtx_s *wm) {
    struct numtx_s mtx;
    struct numtx_s offset;
    int clip;
    int instanced;
    int vis;
    struct nurndrstream_s *stream;
    struct numtl_s *lastmtl;
    struct numtl_s *mtl;
    struct nugeom_s *geom;
    struct nufaceongeom_s *fgeom;
    struct numtl_s *faceonmtl;

    clip = -1;
    stream = 0;
    lastmtl = 0;
    instanced = gobj->next_gobj != 0;
    for (; gobj; gobj = gobj->next_gobj) {
        if (instanced) {
            NuMtxSetIdentity(&offset);
            offset._30 = gobj->origin.x;
            offset._31 = gobj->origin.y;
            offset._32 = gobj->origin.z;
            NuMtxMul(&mtx, &offset, wm);
            vis = NuCameraClipTestExtents(&gobj->min, &gobj->max, &mtx);
        } else {
            vis = NuCameraClipTestExtents(&gobj->min, &gobj->max, wm);
        }
        if (clip == -1) {
            clip = vis;
        } else if ((clip == 1 && vis != 1) || (clip == 0 && vis != 0)) {
            clip = 2;
        }
        if (vis) {
            if (instanced) {
                NuRndrSetLocalMtx(&mtx, vis);
            } else {
                NuRndrSetLocalMtx(wm, vis);
            }
            for (geom = gobj->geom; geom; geom = geom->next) {
                mtl = geom->mtl;
                if (!mtl) {
                    NuDebugMsgProlog(nurndr_file, 0x6D5)(err_geom_no_mtl);
                } else if (nurndr_forced_mtl) {
                    mtl = nurndr_forced_mtl;
                } else if (nurndr_forced_mtl_table && mtl->special_id) {
                    if (nurndr_forced_mtl_table[mtl->special_id]) {
                        mtl = nurndr_forced_mtl_table[mtl->special_id];
                    }
                }
                do {
                    if (mtl != lastmtl) {
                        rndrstream_3d = &mtl->rs;
                        lastmtl = mtl;
                        if (stream != rndrstream_3d) {
                            if (stream) {
                                rndrstream_free = vpDmaTag_Next(stream->cur.w, 0);
                            }
                            stream = rndrstream_3d;
                            NuRndrStreamLink(stream);
                            NuRndrStreamSetMisc(rndrstream_3d);
                            NuRndrStreamSetViewMtx(rndrstream_3d);
                            NuRndrStreamSetLocalMtx(rndrstream_3d);
                        }
                    }
                    rndrstream_3d->cur.w = vpDmaTag_Call(rndrstream_3d->cur.w, geom->dmastream);
                    ((unsigned char *)rndrstream_3d->cur.w)[-14] = 4;
                    mtl = mtl->next;
                } while (mtl);
            }
            for (fgeom = gobj->faceon_geom; fgeom; fgeom = fgeom->next) {
                if (!fgeom->mtl) {
                    NuDebugMsgProlog(nurndr_file, 0x707)(err_fgeom_no_mtl);
                }
                faceonmtl = fgeom->mtl;
                if (faceonmtl != lastmtl) {
                    rndrstream_3d = &faceonmtl->rs;
                    lastmtl = faceonmtl;
                    if (stream != rndrstream_3d) {
                        if (stream) {
                            rndrstream_free = vpDmaTag_Next(stream->cur.w, 0);
                        }
                        stream = rndrstream_3d;
                        NuRndrStreamLink(stream);
                        NuRndrStreamSetMisc(rndrstream_3d);
                        NuRndrStreamSetViewMtx(rndrstream_3d);
                        if (!fgeom->faceon_type) {
                            NuRndrStreamSetLocalMtxEx(rndrstream_3d, 1);
                        } else {
                            NuRndrStreamSetLocalMtxEx(rndrstream_3d, 2);
                        }
                    }
                }
                rndrstream_3d->cur.w = vpDmaTag_Call(rndrstream_3d->cur.w, fgeom->dmastream);
                ((unsigned char *)rndrstream_3d->cur.w)[-14] = 4;
            }
            if (stream) {
                rndrstream_free = vpDmaTag_Next(rndrstream_3d->cur.w, 0);
                stream = 0;
                lastmtl = 0;
            }
        }
    }
    return clip;
}

u32 *NuRndrFxSpriteSet(u32 code, struct FxSpriteSet *fx) {
    u32 tag[8];
    u32 *base;
    u32 *packet;
    int i;

    base = NuRndrStreamAlloc(fx->tablecount * 5 + 8);
    packet = vpDmaTag_RetEx(base, 0x13000000,
        ((fx->tablecount * 5 + 4) << 16) | 0x6c008000);
    *(unsigned long long *)tag = 0x1003400000008001ULL;
    *packet++ = tag[0];
    *packet++ = tag[1];
    *packet++ = 1;
    *packet++ = 0;
    *(unsigned long long *)tag = 0x4403400000008001ULL;
    *packet++ = tag[0];
    *packet++ = tag[1];
    *packet++ = 0x5252;
    *packet++ = 0;
    *(float *)packet++ = fx->zval;
    *packet++ = 0;
    *packet++ = fx->oiterations;
    *packet++ = fx->tablecount;
    *(float *)packet++ = fx->oustep;
    *(float *)packet++ = fx->ovstep;
    *(float *)packet++ = fx->oxstep;
    *(float *)packet++ = fx->oystep;

    for (i = 0; i < fx->tablecount; i++) {
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = fx->iterations[i];
        *packet++ = fx->rgba[i];
        *packet++ = fx->rgba[i] >> 8;
        *packet++ = fx->rgba[i] >> 16;
        *packet++ = fx->rgba[i] >> 24;
        *(float *)packet++ = fx->ustart[i];
        *(float *)packet++ = fx->vstart[i];
        *(float *)packet++ = fx->xstart[i];
        *(float *)packet++ = fx->ystart[i];
        *(float *)packet++ = fx->uw[i];
        *(float *)packet++ = fx->vh[i];
        *(float *)packet++ = fx->xw[i];
        *(float *)packet++ = fx->yh[i];
        *(float *)packet++ = fx->ustep[i];
        *(float *)packet++ = fx->vstep[i];
        *(float *)packet++ = fx->xstep[i];
        *(float *)packet++ = fx->ystep[i];
    }

    *packet++ = code | 0x14000000;
    *packet++ = 0x13000000;
    *packet++ = 0;
    *packet++ = 0;
    vpDmaTag_Close(packet);
    return base;
}


u32 *NuRndrFxSprite2Set(u32 code, struct FxSprite2Set *fx) {
    /* The target reserves two quadwords for its GIF tag scratch. */
    u32 tag[8];
    u32 *base;
    u32 *packet;
    int i;

    base = NuRndrStreamAlloc(fx->tablecount * 8 + 8);
    packet = vpDmaTag_RetEx(base, 0x13000000,
        ((fx->tablecount * 8 + 4) << 16) | 0x6c008000);
    *(unsigned long long *)tag = 0x1003400000008001ULL;
    *packet++ = tag[0];
    *packet++ = tag[1];
    *packet++ = 1;
    *packet++ = 0;
    *(unsigned long long *)tag = 0x4403400000008001ULL;
    *packet++ = tag[0];
    *packet++ = tag[1];
    *packet++ = 0x5252;
    *packet++ = 0;
    *packet++ = (u32)fx->zval;
    *packet++ = 0;
    *packet++ = fx->oiterations;
    *packet++ = fx->tablecount;
    *(float *)packet++ = fx->oustep;
    *(float *)packet++ = fx->ovstep;
    *(float *)packet++ = fx->oxstep;
    *(float *)packet++ = fx->oystep;

    for (i = 0; i < fx->tablecount; i++) {
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = fx->iterations[i];
        *packet++ = fx->rgba[i];
        *packet++ = fx->rgba[i] >> 8;
        *packet++ = (short)(fx->rgba[i] >> 16);
        *packet++ = (signed char)(fx->rgba[i] >> 24);
        *(float *)packet++ = fx->ustart[i];
        *(float *)packet++ = fx->vstart[i];
        *(float *)packet++ = fx->xstart[i];
        *(float *)packet++ = fx->ystart[i];
        *(float *)packet++ = fx->uw[i];
        *(float *)packet++ = fx->vh[i];
        *(float *)packet++ = fx->xw[i];
        *(float *)packet++ = fx->yh[i];
        *(float *)packet++ = fx->ustep[i];
        *(float *)packet++ = fx->vstep[i];
        *(float *)packet++ = fx->xstep[i];
        *(float *)packet++ = fx->ystep[i];
        *(float *)packet++ = fx->ustart2[i];
        *(float *)packet++ = fx->vstart2[i];
        *(float *)packet++ = fx->xstart2[i];
        *(float *)packet++ = fx->ystart2[i];
        *(float *)packet++ = fx->uw2[i];
        *(float *)packet++ = fx->vh2[i];
        *(float *)packet++ = fx->xw2[i];
        *(float *)packet++ = fx->yh2[i];
        *(float *)packet++ = fx->ustep2[i];
        *(float *)packet++ = fx->vstep2[i];
        *(float *)packet++ = fx->xstep2[i];
        *(float *)packet++ = fx->ystep2[i];
    }

    *packet++ = code | 0x14000000;
    *packet++ = 0x13000000;
    *packet++ = 0;
    *packet++ = 0;
    vpDmaTag_Close(packet);
    return base;
}


/* Parameters of the single-channel colour filter; names remain unknown. */
struct FxBlueOrGreen {
    float parameter_00;
    float parameter_04;
    float parameter_08;
    float parameter_0c;
    int parameter_10;
};

inline u32 *NuRndrFxBlueOrGreenData(u32 code, struct FxBlueOrGreen *fx) {
    /* The target reserves two quadwords for its GIF tag scratch. */
    u32 tag[8];
    u32 *base;
    u32 *packet;

    base = NuRndrStreamAlloc(0x10);
    packet = vpDmaTag_RetEx(base, 0x13000000, 0x6C048000);
    *(unsigned long long *)tag = 0x1003000000008001ULL;
    *packet++ = tag[0];
    *packet++ = tag[1];
    *packet++ = 1;
    *packet++ = 0;
    *(unsigned long long *)tag = 0x4403400000008001ULL;
    *packet++ = tag[0];
    *packet++ = tag[1];
    *packet++ = 0x5252;
    *packet++ = 0;
    *(float *)packet++ = fx->parameter_00;
    *(float *)packet++ = fx->parameter_04;
    *(float *)packet++ = fx->parameter_08;
    *(float *)packet++ = fx->parameter_0c;
    *(float *)packet++ = fx->parameter_10;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = code | 0x14000000;
    *packet++ = 0x13000000;
    vpDmaTag_Close(packet);
    return base;
}


void NuRndrFxCode(int code, void *fx, void *dma, struct numtl_s *mtl) {
    u32 *data;

    switch (code) {
    case 0x300:
    case 0x302:
        data = NuRndrFxBlueOrGreenData(code, fx);
        break;
    case 0x304:
        data = NuRndrFxBlueAndGreenData(code, fx);
        break;
    case 0x306:
        data = NuRndrFxSpriteSet(code, fx);
        break;
    case 0x308:
    case 0x30A:
        data = NuRndrFxSprite2Set(code, fx);
        break;
    default:
        return;
    }
    NuRndrStreamLink(rndrstream_fx);
    NuRndrStreamSetMisc(rndrstream_fx);
    NuRndrStreamAddMtl(rndrstream_fx, mtl);
    rndrstream_fx->cur.w = vpDmaTag_CntEx(rndrstream_fx->cur.w, 0, 0x50000000) - 1;
    rndrstream_fx->cur.w = vtBeginDirect(rndrstream_fx->cur.w, 0);
    rndrstream_fx->cur.w = gsBeginGifPacket(rndrstream_fx->cur.w, 0, 1, 0, 0, 0, 1, 0xE);
    *((u64 *)rndrstream_fx->cur.w)++ = 0;
    *((u64 *)rndrstream_fx->cur.w)++ = 0x18;
    rndrstream_fx->cur.w = gsCloseGifPacket(rndrstream_fx->cur.w);
    rndrstream_fx->cur.w = vtCloseDirect(rndrstream_fx->cur.w);
    rndrstream_fx->cur.w = vpDmaTag_Close(rndrstream_fx->cur.w);
    if (dma) {
        rndrstream_fx->cur.w = vpDmaTag_Call(rndrstream_fx->cur.w, dma);
    }
    rndrstream_fx->cur.w = vpDmaTag_Call(rndrstream_fx->cur.w, data);
    rndrstream_fx->cur.w = vpDmaTag_CntEx(rndrstream_fx->cur.w, 0x13000000, 0x50000000) - 1;
    rndrstream_fx->cur.w = vtBeginDirect(rndrstream_fx->cur.w, 0);
    rndrstream_fx->cur.w = gsBeginGifPacket(rndrstream_fx->cur.w, 0, 1, 0, 0, 0, 1, 0xE);
    *((u64 *)rndrstream_fx->cur.w)++ = (u64)((PS2_VCNTR_X - (nurndr_pixel_width >> 1)) << 4)
        | ((u64)((PS2_VCNTR_Y - (nurndr_pixel_height >> 1)) << 4) << 32);
    *((u64 *)rndrstream_fx->cur.w)++ = 0x18;
    rndrstream_fx->cur.w = gsCloseGifPacket(rndrstream_fx->cur.w);
    rndrstream_fx->cur.w = vtCloseDirect(rndrstream_fx->cur.w);
    rndrstream_fx->cur.w = vpDmaTag_Close(rndrstream_fx->cur.w);
    rndrstream_free = vpDmaTag_Next(rndrstream_fx->cur.w, 0);
}


int NuRndrGobjSkin(struct nugobj_s *gobj, struct numtx_s *wm, int njoints, struct numtx_s *joints,
                   u32 *weights, int vis) {
    struct nurndrstream_s *stream;
    struct numtl_s *lastmtl;
    struct numtl_s *mtl;
    struct nugeom_s *geom;
    struct DmaTag *tag;
    int geomidx;

    stream = 0;
    lastmtl = 0;
    if (vis) {
        NuRndrSetLocalMtx(wm, vis);
        geomidx = 0;
        for (geom = gobj->geom; geom; geom = geom->next, geomidx++) {
            if (skinskipix != geomidx && skinskipix != -1) {
                continue;
            }
            mtl = nurndr_forced_mtl;
            if (!mtl && !(mtl = geom->mtl)) {
                NuDebugMsgProlog(nurndr_file, 0x8E5)(err_skin_no_mtl);
            } else if (nurndr_forced_mtl_table && mtl->special_id) {
                if (nurndr_forced_mtl_table[mtl->special_id]) {
                    mtl = nurndr_forced_mtl_table[mtl->special_id];
                }
            }
            do {
                if (mtl != lastmtl) {
                    rndrstream_3d = &mtl->rs;
                    lastmtl = mtl;
                    if (stream != rndrstream_3d) {
                        if (stream) {
                            rndrstream_free = vpDmaTag_Next(stream->cur.w, 0);
                        }
                        stream = rndrstream_3d;
                        NuRndrStreamLink(stream);
                        NuRndrStreamSetMisc(rndrstream_3d);
                        NuRndrStreamSetViewMtxScissorClip(rndrstream_3d);
                        NuRndrStreamSetLocalMtx(rndrstream_3d);
                        if (weights) {
                            rndrstream_3d->cur.w = vpDmaTag_CallEx(rndrstream_3d->cur.w, weights, 0, 0);
                        } else {
                            rndrstream_3d->cur.w = vpDmaTag_CallEx(rndrstream_3d->cur.w, default_skin_weights, 0, 0);
                        }
                        ((unsigned char *)rndrstream_3d->cur.w)[-14] = 4;
                    }
                }
                tag = (struct DmaTag *)geom->skindma;
                while (tag->id != 0x60) {
                    switch (tag->id) {
                    case 0x10:
                        rndrstream_3d->cur.w = vpDmaTag_Copy(rndrstream_3d->cur.w, tag);
                        tag++;
                        break;
                    case 0x30:
                        rndrstream_3d->cur.w = vpDmaTag_RepointCopy(rndrstream_3d->cur.w, tag,
                            tag->addr + (u32)skin_mtx_buffer);
                        tag++;
                        break;
                    case 0x50:
                    default:
                        *((struct DmaTag *)rndrstream_3d->cur.w)++ = *tag++;
                        break;
                    }
                }
                mtl = mtl->next;
            } while (mtl);
        }
        rndrstream_free = vpDmaTag_Next(rndrstream_3d->cur.w, 0);
    }
    return vis;
}


void NuRndrShadowInit(unsigned char *image) {
    unsigned char *src;
    int i;
    int j;

    src = image + 0x14;
    shadstorecnt = 0;
    for (i = 0; i < 21; i++) {
        for (j = 0; j < (NuPower2(PS2_REZ_H) >> 4) + 1; j++) {
            watertab[j][i].x = 0;
            watertab[j][i].y = 0;
            watertab[j][i].u = *src++ * 5 + 0x100;
            watertab[j][i].v = *src++ * 5 + 0x100;
        }
    }
    for (i = 0; i < 21; i++) {
        for (j = 0; j < (NuPower2(PS2_REZ_H) >> 4) + 1; j++) {
            Hazetab[j][i].x = 0;
            Hazetab[j][i].y = 0;
            Hazetab[j][i].u = *src++ * 8 + 0x80;
            Hazetab[j][i].v = *src++ * 8 + 0x80;
        }
    }
}


int NuRndrStreamQuad3dClip(struct nuvtx_tc1_s *verts, int count, void *mtx, struct nurndrstream_s *stream) {
    GifTagQuad strip;
    GifTagQuad fan;
    int n;
    int v;
    int i;

    strip.dword[0] = 0x3002400000008000ULL;
    strip.dword[1] = 0x512;
    fan.dword[0] = 0x3002C00000008000ULL;
    fan.dword[1] = 0x512;
    stream->cur.w = vpDmaTag_Cnt(stream->cur.w);
    ((unsigned char *)stream->cur.w)[-14] = 0x12;
    *stream->cur.w++ = 0x6C010056;
    *stream->cur.w++ = fan.word[0];
    *stream->cur.w++ = fan.word[1];
    *stream->cur.w++ = fan.word[2];
    *stream->cur.w++ = fan.word[3];
    v = 0;
    while (count) {
        n = (count > 42) ? 42 : count;
        n &= 0xFC;
        strip.tag.nloop = n;
        *stream->cur.w++ = 0x6C0180D2;
        *stream->cur.w++ = strip.word[0];
        *stream->cur.w++ = strip.word[1];
        *stream->cur.w++ = strip.word[2];
        *stream->cur.w++ = strip.word[3];
        *stream->cur.w++ = 0x6D018002;
        *((u16 *)stream->cur.w)++ = n * 3;
        *((u16 *)stream->cur.w)++ = 0;
        *((u16 *)stream->cur.w)++ = 0;
        *((u16 *)stream->cur.w)++ = 0;
        *stream->cur.w++ = 0x01000103;
        *stream->cur.w++ = (n << 16) | 0x6E00C005;
        for (i = 0; i < n; i++) {
            *stream->cur.w++ = verts[v + i].diffuse;
        }
        *stream->cur.w++ = 0x01000203;
        *stream->cur.w++ = (n << 17) | 0x6C008003;
        for (i = 0; i < n; i++) {
            *stream->cur.f++ = verts[v].pnt.x;
            *stream->cur.f++ = verts[v].pnt.y;
            *stream->cur.f++ = verts[v].pnt.z;
            *stream->cur.w++ = quad_adc_flags[i] + 0x7FFF;
            *stream->cur.i++ = (int)(verts[v].tc[0] * 4096.0f);
            *stream->cur.i++ = (int)(verts[v].tc[1] * 4096.0f);
            *stream->cur.w++ = 0;
            *stream->cur.w++ = 0;
            v++;
        }
        *stream->cur.w++ = 0x01000101;
        *stream->cur.w++ = 0x14000306;
        count -= n;
    }
    stream->cur.w = vpDmaTag_Close(stream->cur.w);
    return 1;
}


void NuRndrOTInit(void) {
    u32 *packet;

    packet = ot_open_packet;
    packet = vpDmaTag_RefEx(packet, packet + 4, 0, 0x6C0180D2);
    ((u16 *)packet)[-8] = 2;
    *((u64 *)packet)++ = 0x3002400000008003ULL;
    *((u64 *)packet)++ = 0x512;
    *packet++ = 0x6D018002;
    *((u16 *)packet)++ = 9;
    *((u16 *)packet)++ = 0;
    *((u16 *)packet)++ = 0;
    *((u16 *)packet)++ = 0;
    *packet = 0x1400030A;

    packet = ot_close_packet;
    packet = vpDmaTag_RefEx(packet, packet + 4, 0, 0x6C010056);
    ((u16 *)packet)[-8] = 1;
    *((u64 *)packet)++ = 0x3002C00000008000ULL;
    *((u64 *)packet)++ = 0x512;

    ot_batch_free = 0x400;
    memset(ot_bins, 0, 0x1000 * sizeof(nurndrotbin_s));
}


inline void NuRndrOTInsertItem(nurndrotitem_s *item) {
    nurndrotbin_s *bin;
    int index;

    index = (int)(item->depth * otscale);
    if (index < 0) {
        index = 0;
    }
    if (index > 0xFFF) {
        index = 0xFFF;
    }
    bin = &ot_bins[index];
    item->next = bin->head;
    bin->head = item;
}


inline void OTTriInit(void) {
    memset(ot_tri, 0, 0x4000);
    ot_highest = 0;
}


inline int OTTriInsert(struct nurndrottri_s *tri, float depth) {
    int index;

    if (ot_free > ot_tribuff) {
        index = (int)depth;
        if (index > 0xFFF) {
            index = 0xFFF;
        }
        if (index < 0) {
            index = 0;
        }
        ot_free--;
        ot_highest = (ot_highest > index) ? ot_highest : index;
        ot_free->next = ot_tri[index];
        ot_tri[index] = ot_free;
        ot_free->tri = tri;
        return 1;
    }
    return 0;
}


void NuRndrOTInsertTriBatch(struct numtx_s *mtx, int clip, struct numtl_s *mtl, struct nurndrottri_s *tris,
                            int count) {
    struct numtx_s local;
    struct numtx_s caminv;
    float depth[0x800];
    struct numtx_s *xform;
    struct nurndrottri_s *tri;
    float *z;
    float batchz;
    struct numtx_s *mtxcopy;
    u32 *packet;
    u32 *end;
    nurndrotitem_s *item;
    ottri_s **slot;
    ottri_s *entry;
    int n;
    int vtx;

    if (first_time) {
        first_time = 0;
        OTTriInit();
    }
    if (count > 0x800) {
        return;
    }
    ot_free = &ot_tribuff[0x800];
    NuMtxInv(&caminv, &global_camera.mtx);
    if (mtx) {
        NuMtxMulH(&local, mtx, &caminv);
        xform = &local;
    } else {
        mtx = &numtx_identity;
        xform = &caminv;
    }
    if (!ot_batch_free) {
        return;
    }
    z = &depth[0x7FF];
    tri = &tris[count - 1];
    batchz = XFormVuTriZ(z, tri, xform, count);
    do {
        if (!OTTriInsert(tri, *z)) {
            memset(ot_tri, 0, 0x4000);
            memset(ot_tribuff, 0, 0x4000);
            return;
        }
        tri--;
        z--;
    } while (tri >= tris);

    entry = 0;
    mtxcopy = NuRndrStreamAlloc(4);
    *mtxcopy = *mtx;
    packet = NuRndrStreamAlloc(0);
    ot_batch_free--;
    item = &ot_item_pool[ot_batch_free];
    slot = &ot_tri[ot_highest];
    item->dma = packet;
    item->mtx = mtxcopy;
    item->mtxmode = clip;
    item->mtl = mtl;
    item->depth = batchz;
    while (count) {
        n = (count > 14) ? 14 : count;
        count -= n;
        packet = vpDmaTag_CntEx(packet, 0, 0x6C0180D2);
        ((u16 *)packet)[-8] = 2;
        *((u64 *)packet)++ = (n * 3 | 0x8000) | 0x3002400000000000ULL;
        *((u64 *)packet)++ = 0x512;
        *packet++ = 0x6D018002;
        *((u16 *)packet)++ = n * 9;
        *((u16 *)packet)++ = 1;
        vtx = 3;
        *((u16 *)packet)++ = 0;
        *((u16 *)packet)++ = 0;
        *packet++ = 0x01000101;
        while (n) {
            if (!entry) {
                entry = *slot;
                *slot-- = 0;
                if (!entry) {
                    continue;
                }
            }
            *((struct DmaTag *)packet)++ = entry->tri->tag;
            ((u16 *)packet)[-2] = vtx | 0x8000;
            vtx += 9;
            n--;
            entry = entry->next;
        }
        packet = vpDmaTag_CntEx(packet, 0, 0x14000300);
    }
    ot_highest = 0;
    end = vpDmaTag_Ret(packet);
    NuRndrStreamAlloc(((u32)end - (u32)item->dma) >> 4);
    NuRndrOTInsertItem(item);
}


int NuRndrStreamTriStrip3dClip(struct nuvtx_tc1_s *verts, int count, struct numtx_s *mtx,
                               struct nurndrstream_s *stream) {
    GifTagQuad strip;
    GifTagQuad fan;
    int n;
    int v;
    int i;

    strip.dword[0] = 0x3002400000008000ULL;
    strip.dword[1] = 0x512;
    fan.dword[0] = 0x3002C00000008000ULL;
    fan.dword[1] = 0x512;
    if (!mtx) {
        mtx = &numtx_identity;
    }
    NuRndrSetLocalMtx(mtx, 2);
    NuRndrStreamLink(stream);
    NuRndrStreamSetMisc(stream);
    NuRndrStreamSetViewMtx(stream);
    NuRndrStreamSetLocalMtx(stream);
    stream->cur.w = vpDmaTag_Cnt(stream->cur.w);
    ((unsigned char *)stream->cur.w)[-14] = 0x13;
    *stream->cur.w++ = 0x6C010056;
    *stream->cur.w++ = fan.word[0];
    *stream->cur.w++ = fan.word[1];
    *stream->cur.w++ = fan.word[2];
    *stream->cur.w++ = fan.word[3];
    v = 0;
    while (count) {
        n = (count > 42) ? 42 : count;
        strip.tag.nloop = n;
        *stream->cur.w++ = 0x6C0180D2;
        *stream->cur.w++ = strip.word[0];
        *stream->cur.w++ = strip.word[1];
        *stream->cur.w++ = strip.word[2];
        *stream->cur.w++ = strip.word[3];
        *stream->cur.w++ = 0x6D018002;
        *((u16 *)stream->cur.w)++ = n * 3;
        *((u16 *)stream->cur.w)++ = 0;
        *((u16 *)stream->cur.w)++ = 0;
        *((u16 *)stream->cur.w)++ = 0;
        *stream->cur.w++ = 0x01000103;
        *stream->cur.w++ = (n << 16) | 0x6C00C005;
        for (i = 0; i < n; i++) {
            *stream->cur.w++ = verts[v + i].rgba.r;
            *stream->cur.w++ = verts[v + i].rgba.g;
            *stream->cur.w++ = verts[v + i].rgba.b;
            *stream->cur.w++ = verts[v + i].rgba.a;
        }
        *stream->cur.w++ = 0x01000203;
        *stream->cur.w++ = (n << 17) | 0x6C008003;
        for (i = 0; i < n; i++) {
            *stream->cur.f++ = verts[v].pnt.x;
            *stream->cur.f++ = verts[v].pnt.y;
            *stream->cur.f++ = verts[v].pnt.z;
            *stream->cur.f = verts[v].nrm.x;
            *stream->cur.b = i < 2;
            stream->cur.w++;
            *stream->cur.i++ = (int)(verts[v].tc[0] * 4096.0f);
            *stream->cur.i++ = (int)(verts[v].tc[1] * 4096.0f);
            *stream->cur.i++ = (int)(verts[v].nrm.y * 4096.0f);
            *stream->cur.i++ = (int)(verts[v].nrm.z * 4096.0f);
            v++;
        }
        *stream->cur.w++ = 0x01000101;
        *stream->cur.w++ = 0x14000300;
        count -= n;
    }
    stream->cur.w = vpDmaTag_Close(stream->cur.w);
    rndrstream_free = vpDmaTag_Next(stream->cur.w, 0);
    return 1;
}


int NuRndrStreamTri3dClip(struct nuvtx_tc1_s *verts, int count, struct numtx_s *mtx,
                          struct nurndrstream_s *stream) {
    GifTagQuad strip;
    GifTagQuad fan;
    int n;
    int v;
    int i;

    strip.dword[0] = 0x3002400000008000ULL;
    strip.dword[1] = 0x512;
    fan.dword[0] = 0x3002C00000008000ULL;
    fan.dword[1] = 0x512;
    if (!mtx) {
        mtx = &numtx_identity;
    }
    NuRndrSetLocalMtx(mtx, 2);
    NuRndrStreamLink(stream);
    NuRndrStreamSetMisc(stream);
    NuRndrStreamSetViewMtx(stream);
    NuRndrStreamSetLocalMtx(stream);
    stream->cur.w = vpDmaTag_Cnt(stream->cur.w);
    ((unsigned char *)stream->cur.w)[-14] = 0x11;
    *stream->cur.w++ = 0x6C010056;
    *stream->cur.w++ = fan.word[0];
    *stream->cur.w++ = fan.word[1];
    *stream->cur.w++ = fan.word[2];
    *stream->cur.w++ = fan.word[3];
    v = 0;
    while (count) {
        n = (count > 42) ? 42 : count;
        strip.tag.nloop = n;
        *stream->cur.w++ = 0x6C0180D2;
        *stream->cur.w++ = strip.word[0];
        *stream->cur.w++ = strip.word[1];
        *stream->cur.w++ = strip.word[2];
        *stream->cur.w++ = strip.word[3];
        *stream->cur.w++ = 0x6D018002;
        *((u16 *)stream->cur.w)++ = n * 3;
        *((u16 *)stream->cur.w)++ = 0;
        *((u16 *)stream->cur.w)++ = 0;
        *((u16 *)stream->cur.w)++ = 0;
        *stream->cur.w++ = 0x01000103;
        *stream->cur.w++ = (n << 16) | 0x6C00C005;
        for (i = 0; i < n; i++) {
            *stream->cur.w++ = verts[v + i].rgba.r;
            *stream->cur.w++ = verts[v + i].rgba.g;
            *stream->cur.w++ = verts[v + i].rgba.b;
            *stream->cur.w++ = verts[v + i].rgba.a;
        }
        *stream->cur.w++ = 0x01000203;
        *stream->cur.w++ = (n << 17) | 0x6C008003;
        for (i = 0; i < n; i++) {
            *stream->cur.f++ = verts[v].pnt.x;
            *stream->cur.f++ = verts[v].pnt.y;
            *stream->cur.f++ = verts[v].pnt.z;
            *stream->cur.f = verts[v].nrm.x;
            *stream->cur.b = tri_adc_flags[i];
            stream->cur.w++;
            *stream->cur.i++ = (int)(verts[v].tc[0] * 4096.0f);
            *stream->cur.i++ = (int)(verts[v].tc[1] * 4096.0f);
            *stream->cur.i++ = (int)(verts[v].nrm.y * 4096.0f);
            *stream->cur.i++ = (int)(verts[v].nrm.z * 4096.0f);
            v++;
        }
        *stream->cur.w++ = 0x01000101;
        *stream->cur.w++ = 0x14000300;
        count -= n;
    }
    stream->cur.w = vpDmaTag_Close(stream->cur.w);
    rndrstream_free = vpDmaTag_Next(stream->cur.w, 0);
    return 1;
}


void NuRndrShadowCylinderCap(struct numtx_s *mtx, struct nurndrshadcyl_s *cyl, int count, struct nuvec_s *light) {
    int i;

    for (i = 0; i < count; i++) {
        if (cyl->flags & 1) {
            rndrstream_fx->cur.w = vpDmaTag_Call(rndrstream_fx->cur.w, NRCylCapData);
        } else {
            rndrstream_fx->cur.w = vpDmaTag_Call(rndrstream_fx->cur.w, NRCylData);
        }
        rndrstream_fx->cur.w = vpDmaTag_Cnt(rndrstream_fx->cur.w);
        *rndrstream_fx->cur.w++ = 0x6C0180A5;
        *rndrstream_fx->cur.f++ = global_camera.mtx._30;
        *rndrstream_fx->cur.f++ = global_camera.mtx._31;
        *rndrstream_fx->cur.f++ = global_camera.mtx._32;
        *rndrstream_fx->cur.w++ = 0;
        *rndrstream_fx->cur.w++ = 0x6C0480A6;
        *rndrstream_fx->cur.f++ = mtx->_00;
        *rndrstream_fx->cur.f++ = mtx->_01;
        *rndrstream_fx->cur.f++ = mtx->_02;
        *rndrstream_fx->cur.f++ = mtx->_03;
        *rndrstream_fx->cur.f++ = mtx->_10;
        *rndrstream_fx->cur.f++ = mtx->_11;
        *rndrstream_fx->cur.f++ = mtx->_12;
        *rndrstream_fx->cur.f++ = mtx->_13;
        *rndrstream_fx->cur.f++ = mtx->_20;
        *rndrstream_fx->cur.f++ = mtx->_21;
        *rndrstream_fx->cur.f++ = mtx->_22;
        *rndrstream_fx->cur.f++ = mtx->_23;
        *rndrstream_fx->cur.f++ = mtx->_30;
        *rndrstream_fx->cur.f++ = mtx->_31;
        *rndrstream_fx->cur.f++ = mtx->_32;
        *rndrstream_fx->cur.f++ = mtx->_33;
        *rndrstream_fx->cur.w++ = 0x680580AA;
        *rndrstream_fx->cur.f++ = light->x;
        *rndrstream_fx->cur.f++ = light->y;
        *rndrstream_fx->cur.f++ = light->z;
        *rndrstream_fx->cur.f++ = cyl->pos.x;
        *rndrstream_fx->cur.f++ = cyl->pos.y;
        *rndrstream_fx->cur.f++ = cyl->pos.z;
        *rndrstream_fx->cur.f++ = cyl->edge0.x;
        *rndrstream_fx->cur.f++ = cyl->edge0.y;
        *rndrstream_fx->cur.f++ = cyl->edge0.z;
        *rndrstream_fx->cur.f++ = cyl->edge1.x;
        *rndrstream_fx->cur.f++ = cyl->edge1.y;
        *rndrstream_fx->cur.f++ = cyl->edge1.z;
        *rndrstream_fx->cur.f++ = cyl->edge2.x;
        *rndrstream_fx->cur.f++ = cyl->edge2.y;
        *rndrstream_fx->cur.f++ = cyl->edge2.z;
        cyl++;
        *rndrstream_fx->cur.w++ = 0x14000300;
        rndrstream_fx->cur.w = vpDmaTag_Close(rndrstream_fx->cur.w);
    }
}


void NuRndrShadowEllipse(struct numtx_s *mtx, struct nurndrshadellipse_s *ellipse, int count, struct nuvec_s *light) {
    int i;

    for (i = 0; i < count; i++) {
        rndrstream_fx->cur.w = vpDmaTag_Call(rndrstream_fx->cur.w, NRFanData);
        rndrstream_fx->cur.w = vpDmaTag_Cnt(rndrstream_fx->cur.w);
        *rndrstream_fx->cur.w++ = 0x6C0180A5;
        *rndrstream_fx->cur.f++ = global_camera.mtx._30;
        *rndrstream_fx->cur.f++ = global_camera.mtx._31;
        *rndrstream_fx->cur.f++ = global_camera.mtx._32;
        *rndrstream_fx->cur.w++ = 0;
        *rndrstream_fx->cur.w++ = 0x6C0480A6;
        *rndrstream_fx->cur.f++ = mtx->_00;
        *rndrstream_fx->cur.f++ = mtx->_01;
        *rndrstream_fx->cur.f++ = mtx->_02;
        *rndrstream_fx->cur.f++ = mtx->_03;
        *rndrstream_fx->cur.f++ = mtx->_10;
        *rndrstream_fx->cur.f++ = mtx->_11;
        *rndrstream_fx->cur.f++ = mtx->_12;
        *rndrstream_fx->cur.f++ = mtx->_13;
        *rndrstream_fx->cur.f++ = mtx->_20;
        *rndrstream_fx->cur.f++ = mtx->_21;
        *rndrstream_fx->cur.f++ = mtx->_22;
        *rndrstream_fx->cur.f++ = mtx->_23;
        *rndrstream_fx->cur.f++ = mtx->_30;
        *rndrstream_fx->cur.f++ = mtx->_31;
        *rndrstream_fx->cur.f++ = mtx->_32;
        *rndrstream_fx->cur.f++ = mtx->_33;
        *rndrstream_fx->cur.w++ = 0x680580AA;
        *rndrstream_fx->cur.f++ = light->x;
        *rndrstream_fx->cur.f++ = light->y;
        *rndrstream_fx->cur.f++ = light->z;
        *rndrstream_fx->cur.f++ = ellipse->pos.x;
        *rndrstream_fx->cur.f++ = ellipse->pos.y;
        *rndrstream_fx->cur.f++ = ellipse->pos.z;
        *rndrstream_fx->cur.f++ = ellipse->axis0.x;
        *rndrstream_fx->cur.f++ = ellipse->axis0.y;
        *rndrstream_fx->cur.f++ = ellipse->axis0.z;
        *rndrstream_fx->cur.f++ = ellipse->axis1.x;
        *rndrstream_fx->cur.f++ = ellipse->axis1.y;
        *rndrstream_fx->cur.f++ = ellipse->axis1.z;
        *rndrstream_fx->cur.f++ = ellipse->axis2.x;
        *rndrstream_fx->cur.f++ = ellipse->axis2.y;
        *rndrstream_fx->cur.f++ = ellipse->axis2.z;
        ellipse++;
        *rndrstream_fx->cur.w++ = 0x14000302;
        rndrstream_fx->cur.w = vpDmaTag_Close(rndrstream_fx->cur.w);
    }
}


void NuRndrShadowPols(struct numtx_s *mtx, struct nurndrshadface_s *face, struct nurndrshadface_s *vtx,
                      struct nuvec_s *light) {
    GifTagQuad strip;
    GifTagQuad fan;
    int started;
    int total;
    int addr;
    int i;
    int j;

    strip.dword[0] = 0x3002400000008000ULL;
    strip.dword[1] = 0x512;
    fan.dword[0] = 0x3002C00000008000ULL;
    fan.dword[1] = 0x512;
    rndrstream_fx->cur.w = vpDmaTag_Cnt(rndrstream_fx->cur.w);
    *rndrstream_fx->cur.w++ = 0x6C010056;
    *rndrstream_fx->cur.w++ = fan.word[0];
    *rndrstream_fx->cur.w++ = fan.word[1];
    *rndrstream_fx->cur.w++ = fan.word[2];
    *rndrstream_fx->cur.w++ = fan.word[3];
    strip.tag.nloop = 0;
    *rndrstream_fx->cur.w++ = 0x6C0180D2;
    *rndrstream_fx->cur.w++ = strip.word[0];
    *rndrstream_fx->cur.w++ = strip.word[1];
    *rndrstream_fx->cur.w++ = strip.word[2];
    *rndrstream_fx->cur.w++ = strip.word[3];
    *rndrstream_fx->cur.w++ = 0x6D018002;
    *((u16 *)rndrstream_fx->cur.w)++ = 0;
    *((u16 *)rndrstream_fx->cur.w)++ = 0;
    *((u16 *)rndrstream_fx->cur.w)++ = 0;
    *((u16 *)rndrstream_fx->cur.w)++ = 0;
    *rndrstream_fx->cur.w++ = 0x6C0180BE;
    *rndrstream_fx->cur.f++ = global_camera.mtx._30;
    *rndrstream_fx->cur.f++ = global_camera.mtx._31;
    *rndrstream_fx->cur.f++ = global_camera.mtx._32;
    *rndrstream_fx->cur.w++ = 0;
    *rndrstream_fx->cur.w++ = 0x6C0480BF;
    *rndrstream_fx->cur.f++ = mtx->_00;
    *rndrstream_fx->cur.f++ = mtx->_01;
    *rndrstream_fx->cur.f++ = mtx->_02;
    *rndrstream_fx->cur.f++ = mtx->_03;
    *rndrstream_fx->cur.f++ = mtx->_10;
    *rndrstream_fx->cur.f++ = mtx->_11;
    *rndrstream_fx->cur.f++ = mtx->_12;
    *rndrstream_fx->cur.f++ = mtx->_13;
    *rndrstream_fx->cur.f++ = mtx->_20;
    *rndrstream_fx->cur.f++ = mtx->_21;
    *rndrstream_fx->cur.f++ = mtx->_22;
    *rndrstream_fx->cur.f++ = mtx->_23;
    *rndrstream_fx->cur.f++ = mtx->_30;
    *rndrstream_fx->cur.f++ = mtx->_31;
    *rndrstream_fx->cur.f++ = mtx->_32;
    *rndrstream_fx->cur.f++ = mtx->_33;
    *rndrstream_fx->cur.w++ = 0x6C0180C3;
    *rndrstream_fx->cur.f++ = light->x;
    *rndrstream_fx->cur.f++ = light->y;
    *rndrstream_fx->cur.f++ = light->z;
    *rndrstream_fx->cur.w++ = 0;
    started = 0;
    total = 0;
    addr = 0xA5;
    for (i = vtx->count; i > 0; i--) {
        if (total + face->count + 1 >= 16) {
            *rndrstream_fx->cur.w++ = addr | 0x6C018000;
            *rndrstream_fx->cur.w++ = 0;
            *rndrstream_fx->cur.w++ = 0;
            *rndrstream_fx->cur.w++ = 0;
            *rndrstream_fx->cur.w++ = 0;
            if (testdebug) {
                *rndrstream_fx->cur.w++ = 0x1400030E;
            } else {
                *rndrstream_fx->cur.w++ = 0x14000304;
            }
            if (!started) {
                started = 1;
                *rndrstream_fx->cur.w++ = 0x6C010056;
                *rndrstream_fx->cur.w++ = fan.word[0];
                *rndrstream_fx->cur.w++ = fan.word[1];
                *rndrstream_fx->cur.w++ = fan.word[2];
                *rndrstream_fx->cur.w++ = fan.word[3];
                strip.tag.nloop = 0;
                *rndrstream_fx->cur.w++ = 0x6C0180D2;
                *rndrstream_fx->cur.w++ = strip.word[0];
                *rndrstream_fx->cur.w++ = strip.word[1];
                *rndrstream_fx->cur.w++ = strip.word[2];
                *rndrstream_fx->cur.w++ = strip.word[3];
                *rndrstream_fx->cur.w++ = 0x6D018002;
                *((u16 *)rndrstream_fx->cur.w)++ = 0;
                *((u16 *)rndrstream_fx->cur.w)++ = 0;
                *((u16 *)rndrstream_fx->cur.w)++ = 0;
                *((u16 *)rndrstream_fx->cur.w)++ = 0;
                *rndrstream_fx->cur.w++ = 0x6C0180BE;
                *rndrstream_fx->cur.f++ = global_camera.mtx._30;
                *rndrstream_fx->cur.f++ = global_camera.mtx._31;
                *rndrstream_fx->cur.f++ = global_camera.mtx._32;
                *rndrstream_fx->cur.w++ = 0;
                *rndrstream_fx->cur.w++ = 0x6C0480BF;
                *rndrstream_fx->cur.f++ = mtx->_00;
                *rndrstream_fx->cur.f++ = mtx->_01;
                *rndrstream_fx->cur.f++ = mtx->_02;
                *rndrstream_fx->cur.f++ = mtx->_03;
                *rndrstream_fx->cur.f++ = mtx->_10;
                *rndrstream_fx->cur.f++ = mtx->_11;
                *rndrstream_fx->cur.f++ = mtx->_12;
                *rndrstream_fx->cur.f++ = mtx->_13;
                *rndrstream_fx->cur.f++ = mtx->_20;
                *rndrstream_fx->cur.f++ = mtx->_21;
                *rndrstream_fx->cur.f++ = mtx->_22;
                *rndrstream_fx->cur.f++ = mtx->_23;
                *rndrstream_fx->cur.f++ = mtx->_30;
                *rndrstream_fx->cur.f++ = mtx->_31;
                *rndrstream_fx->cur.f++ = mtx->_32;
                *rndrstream_fx->cur.f++ = mtx->_33;
                *rndrstream_fx->cur.w++ = 0x6C0180C3;
                *rndrstream_fx->cur.f++ = light->x;
                *rndrstream_fx->cur.f++ = light->y;
                *rndrstream_fx->cur.f++ = light->z;
                *rndrstream_fx->cur.w++ = 0;
            }
            total = 0;
            addr = 0xA5;
        }
        *rndrstream_fx->cur.w++ = addr | 0x6C018000;
        *rndrstream_fx->cur.f++ = face->v.x;
        *rndrstream_fx->cur.f++ = face->v.y;
        *rndrstream_fx->cur.f++ = face->v.z;
        *rndrstream_fx->cur.w++ = face->count;
        *rndrstream_fx->cur.w++ = (addr + 1) | ((face->count << 16) + 0x8000) | 0x68000000;
        for (j = face->count; j > 0; j--) {
            *rndrstream_fx->cur.f++ = vtx->v.x;
            *rndrstream_fx->cur.f++ = vtx->v.y;
            *rndrstream_fx->cur.f++ = vtx->v.z;
            vtx++;
        }
        total += face->count + 1;
        addr += face->count + 1;
        face++;
    }
    if (total) {
        *rndrstream_fx->cur.w++ = addr | 0x6C018000;
        *rndrstream_fx->cur.w++ = 0;
        *rndrstream_fx->cur.w++ = 0;
        *rndrstream_fx->cur.w++ = 0;
        *rndrstream_fx->cur.w++ = 0;
        if (testdebug) {
            *rndrstream_fx->cur.w++ = 0x1400030E;
        } else {
            *rndrstream_fx->cur.w++ = 0x14000304;
        }
    }
    rndrstream_fx->cur.w = vpDmaTag_Close(rndrstream_fx->cur.w);
}


inline float GlobalCam3DNClip(struct nuvec_s *p0, struct nuvec_s *p1, struct nuvec_s *p2) {
    struct nuvec_s normal;

    NuVecCrossRel(&normal, p0, p1, p2);
    return (p0->x - global_camera.mtx._30) * normal.x
        + (p0->y - global_camera.mtx._31) * normal.y
        + (p0->z - global_camera.mtx._32) * normal.z;
}


void NuRndrShadowCylinderCap2(struct numtx_s *mtx, struct nurndrshadcyl_s *cyl, int count, struct nuvec_s *light) {
    struct nuvec4_s centre;
    struct nuvec_s axis;
    struct nuvec_s side;
    struct nuvec_s up;
    struct nuvec4_s *pts;
    struct nuvec_s *rel;
    struct nuvtx_tc1_s (*quads)[4];
    struct nuvtx_tc1_s *corner0;
    struct nuvtx_tc1_s *corner1;
    struct nuvtx_tc1_s *corner2;
    struct nuvtx_tc1_s *corner3;
    struct numtx_s *vpcs;
    float sidelen;
    float axislen;
    float dot;
    u32 colour;
    int i;
    int j;

    pts = NuScratchAlloc32(0x200);
    rel = NuScratchAlloc32(0x60);
    quads = NuScratchAlloc32(0x6C0);
    vpcs = NuCameraGetVPCSMtx();
    for (i = 0; i < count; i++, cyl++) {
        if (!(cyl->flags & 1)) {
            continue;
        }
        pts[4].x = cyl->pos.x;
        pts[4].y = cyl->pos.y;
        pts[4].z = cyl->pos.z;
        pts[4].w = 1.0f;
        NuVec4MtxTransformVU0(&centre, &pts[4], mtx);
        pts[0].x = cyl->edge0.x + cyl->pos.x;
        pts[0].y = cyl->edge0.y + cyl->pos.y;
        pts[0].z = cyl->edge0.z + cyl->pos.z;
        pts[0].w = 1.0f;
        NuVec4MtxTransformVU0(&pts[0], &pts[0], mtx);
        rel[0].x = pts[0].x - centre.x;
        rel[0].y = pts[0].y - centre.y;
        rel[0].z = pts[0].z - centre.z;
        pts[1].x = cyl->edge1.x + cyl->pos.x;
        pts[1].y = cyl->edge1.y + cyl->pos.y;
        pts[1].z = cyl->edge1.z + cyl->pos.z;
        pts[1].w = 1.0f;
        NuVec4MtxTransformVU0(&pts[1], &pts[1], mtx);
        rel[1].x = pts[1].x - centre.x;
        rel[1].y = pts[1].y - centre.y;
        rel[1].z = pts[1].z - centre.z;
        pts[2].x = cyl->edge2.x + cyl->pos.x;
        pts[2].y = cyl->edge2.y + cyl->pos.y;
        pts[2].z = cyl->edge2.z + cyl->pos.z;
        pts[2].w = 1.0f;
        NuVec4MtxTransformVU0(&pts[2], &pts[2], mtx);
        rel[2].x = pts[2].x - centre.x;
        rel[2].y = pts[2].y - centre.y;
        rel[2].z = pts[2].z - centre.z;
        axis.x = rel[0].x;
        axis.y = rel[0].y;
        axis.z = rel[0].z;
        NuVecCross(&side, &rel[0], light);
        sidelen = side.x * side.x + side.y * side.y + side.z * side.z;
        if (sidelen > 0.0f) {
            sidelen = 1.0f / NuFsqrt(sidelen);
        }
        axislen = axis.x * axis.x + axis.y * axis.y + axis.z * axis.z;
        side.x *= sidelen;
        side.y *= sidelen;
        side.z *= sidelen;
        if (axislen > 0.0f) {
            axislen = 1.0f / NuFsqrt(axislen);
        } else {
            axislen = 0.0f;
        }
        sidelen = side.x * rel[1].x + side.y * rel[1].y + side.z * rel[1].z;
        dot = side.x * rel[2].x + side.y * rel[2].y + side.z * rel[2].z;
        sidelen = NuFsqrt(sidelen * sidelen + dot * dot);
        axislen *= sidelen;
        side.x *= sidelen;
        side.y *= sidelen;
        side.z *= sidelen;
        up.x = axis.x * axislen;
        up.y = axis.y * axislen;
        up.z = axis.z * axislen;
        for (j = 0; j < 10; j++) {
            switch (j) {
            case 0:
                pts[0].x = axis.x + side.x;
                pts[0].y = axis.y + side.y;
                pts[0].z = axis.z + side.z;
                pts[1].x = -axis.x + side.x;
                pts[1].y = -axis.y + side.y;
                pts[1].z = -axis.z + side.z;
                break;
            case 1:
                pts[0].x = -axis.x + side.x;
                pts[0].y = -axis.y + side.y;
                pts[0].z = -axis.z + side.z;
                pts[1].x = -axis.x + side.x * NuTrigTable[0x6000] - up.x * NuTrigTable[0x2000];
                pts[1].y = -axis.y + side.y * NuTrigTable[0x6000] - up.y * NuTrigTable[0x2000];
                pts[1].z = -axis.z + side.z * NuTrigTable[0x6000] - up.z * NuTrigTable[0x2000];
                break;
            case 2:
                pts[0].x = -axis.x + side.x * NuTrigTable[0x6000] - up.x * NuTrigTable[0x2000];
                pts[0].y = -axis.y + side.y * NuTrigTable[0x6000] - up.y * NuTrigTable[0x2000];
                pts[0].z = -axis.z + side.z * NuTrigTable[0x6000] - up.z * NuTrigTable[0x2000];
                pts[1].x = -axis.x - axis.x * axislen;
                pts[1].y = -axis.y - axis.y * axislen;
                pts[1].z = -axis.z - axis.z * axislen;
                break;
            case 3:
                pts[0].x = -axis.x - axis.x * axislen;
                pts[0].y = -axis.y - axis.y * axislen;
                pts[0].z = -axis.z - axis.z * axislen;
                pts[1].x = -axis.x + side.x * NuTrigTable[0xA000] - up.x * NuTrigTable[0x6000];
                pts[1].y = -axis.y + side.y * NuTrigTable[0xA000] - up.y * NuTrigTable[0x6000];
                pts[1].z = -axis.z + side.z * NuTrigTable[0xA000] - up.z * NuTrigTable[0x6000];
                break;
            case 4:
                pts[0].x = -axis.x + side.x * NuTrigTable[0xA000] - up.x * NuTrigTable[0x6000];
                pts[0].y = -axis.y + side.y * NuTrigTable[0xA000] - up.y * NuTrigTable[0x6000];
                pts[0].z = -axis.z + side.z * NuTrigTable[0xA000] - up.z * NuTrigTable[0x6000];
                pts[1].x = -axis.x - side.x;
                pts[1].y = -axis.y - side.y;
                pts[1].z = -axis.z - side.z;
                break;
            case 5:
                pts[0].x = -axis.x - side.x;
                pts[0].y = -axis.y - side.y;
                pts[0].z = -axis.z - side.z;
                pts[1].x = axis.x - side.x;
                pts[1].y = axis.y - side.y;
                pts[1].z = axis.z - side.z;
                break;
            case 6:
                pts[0].x = axis.x - side.x;
                pts[0].y = axis.y - side.y;
                pts[0].z = axis.z - side.z;
                pts[1].x = axis.x - side.x * NuTrigTable[0x6000] + up.x * NuTrigTable[0x2000];
                pts[1].y = axis.y - side.y * NuTrigTable[0x6000] + up.y * NuTrigTable[0x2000];
                pts[1].z = axis.z - side.z * NuTrigTable[0x6000] + up.z * NuTrigTable[0x2000];
                break;
            case 7:
                pts[0].x = axis.x - side.x * NuTrigTable[0x6000] + up.x * NuTrigTable[0x2000];
                pts[0].y = axis.y - side.y * NuTrigTable[0x6000] + up.y * NuTrigTable[0x2000];
                pts[0].z = axis.z - side.z * NuTrigTable[0x6000] + up.z * NuTrigTable[0x2000];
                pts[1].x = axis.x + axis.x * axislen;
                pts[1].y = axis.y + axis.y * axislen;
                pts[1].z = axis.z + axis.z * axislen;
                break;
            case 8:
                pts[0].x = axis.x + axis.x * axislen;
                pts[0].y = axis.y + axis.y * axislen;
                pts[0].z = axis.z + axis.z * axislen;
                pts[1].x = axis.x - side.x * NuTrigTable[0xA000] + up.x * NuTrigTable[0x6000];
                pts[1].y = axis.y - side.y * NuTrigTable[0xA000] + up.y * NuTrigTable[0x6000];
                pts[1].z = axis.z - side.z * NuTrigTable[0xA000] + up.z * NuTrigTable[0x6000];
                break;
            case 9:
                pts[0].x = axis.x - side.x * NuTrigTable[0xA000] + up.x * NuTrigTable[0x6000];
                pts[0].y = axis.y - side.y * NuTrigTable[0xA000] + up.y * NuTrigTable[0x6000];
                pts[0].z = axis.z - side.z * NuTrigTable[0xA000] + up.z * NuTrigTable[0x6000];
                pts[1].x = axis.x + side.x;
                pts[1].y = axis.y + side.y;
                pts[1].z = axis.z + side.z;
                break;
            }
            pts[0].x += centre.x;
            pts[0].y += centre.y;
            pts[0].z += centre.z;
            pts[1].x += centre.x;
            pts[1].y += centre.y;
            pts[1].z += centre.z;
            pts[2].x = pts[0].x + light->x;
            pts[2].y = pts[0].y + light->y;
            pts[2].z = pts[0].z + light->z;
            pts[3].x = pts[1].x + light->x;
            pts[3].y = pts[1].y + light->y;
            pts[3].z = pts[1].z + light->z;
            corner0 = &quads[j][0];
            corner1 = &quads[j][1];
            corner2 = &quads[j][2];
            corner3 = &quads[j][3];
            corner0->pnt.x = pts[0].x;
            corner0->pnt.y = pts[0].y;
            corner0->pnt.z = pts[0].z;
            corner1->pnt.x = pts[1].x;
            corner1->pnt.y = pts[1].y;
            corner1->pnt.z = pts[1].z;
            corner2->pnt.x = pts[2].x;
            corner2->pnt.y = pts[2].y;
            corner2->pnt.z = pts[2].z;
            corner3->pnt.x = pts[3].x;
            corner3->pnt.y = pts[3].y;
            corner3->pnt.z = pts[3].z;
            if (GlobalCam3DNClip(&corner0->pnt, &corner2->pnt, &corner3->pnt) > 0.0f) {
                colour = 0x80010101;
            } else {
                colour = 0x80FFFFFF;
            }
            corner0->diffuse = colour;
            corner1->diffuse = colour;
            corner2->diffuse = colour;
            corner3->diffuse = colour;
            pts[0].w = 1.0f;
            pts[1].w = 1.0f;
            pts[2].w = 1.0f;
            pts[3].w = 1.0f;
            NuVec4MtxTransformVU0(&pts[0], &pts[0], vpcs);
            NuVec4MtxTransformVU0(&pts[1], &pts[1], vpcs);
            NuVec4MtxTransformVU0(&pts[2], &pts[2], vpcs);
            NuVec4MtxTransformVU0(&pts[3], &pts[3], vpcs);
        }
        pts[18] = pts[1];
    }
    NuScratchRelease();
    NuScratchRelease();
    NuScratchRelease();
}


u32 *NuRndrCylCapMakeData(void) {
    GifTagQuad strip;
    GifTagQuad fan;
    u32 *base;
    u32 *packet;

    strip.dword[0] = 0x3002400000008000ULL;
    strip.dword[1] = 0x512;
    fan.dword[0] = 0x3002C00000008000ULL;
    fan.dword[1] = 0x512;
    packet = cylcap_packet;
    base = packet;
    packet = vpDmaTag_RetEx(base, 0, 0x6C010056);
    *packet++ = fan.word[0];
    *packet++ = fan.word[1];
    *packet++ = fan.word[2];
    *packet++ = fan.word[3];
    strip.tag.nloop = 22;
    *packet++ = 0x6C0180D2;
    *packet++ = strip.word[0];
    *packet++ = strip.word[1];
    *packet++ = strip.word[2];
    *packet++ = strip.word[3];
    *packet++ = 0x6D018002;
    *((u16 *)packet)++ = 0x42;
    *((u16 *)packet)++ = 0;
    *((u16 *)packet)++ = 0;
    *((u16 *)packet)++ = 0;
    *packet++ = 0x6C0C80AF;
    *packet++ = 11;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *((float *)packet)++ = 1.0f;
    *((float *)packet)++ = 0.0f;
    *((float *)packet)++ = 1.0f;
    *((float *)packet)++ = 0.0f;
    *((float *)packet)++ = -1.0f;
    *((float *)packet)++ = 0.0f;
    *((float *)packet)++ = 1.0f;
    *((float *)packet)++ = 0.0f;
    *((float *)packet)++ = -1.0f;
    *((float *)packet)++ = -NuTrigTable[0x2000];
    *((float *)packet)++ = 0.0f;
    *((float *)packet)++ = NuTrigTable[0x6000];
    *((float *)packet)++ = -1.0f;
    *((float *)packet)++ = -1.0f;
    *((float *)packet)++ = 0.0f;
    *((float *)packet)++ = 0.0f;
    *((float *)packet)++ = -1.0f;
    *((float *)packet)++ = -NuTrigTable[0x6000];
    *((float *)packet)++ = 0.0f;
    *((float *)packet)++ = NuTrigTable[0xA000];
    *((float *)packet)++ = -1.0f;
    *((float *)packet)++ = 0.0f;
    *((float *)packet)++ = -1.0f;
    *((float *)packet)++ = 0.0f;
    *((float *)packet)++ = 1.0f;
    *((float *)packet)++ = 0.0f;
    *((float *)packet)++ = -1.0f;
    *((float *)packet)++ = 0.0f;
    *((float *)packet)++ = 1.0f;
    *((float *)packet)++ = NuTrigTable[0x2000];
    *((float *)packet)++ = 0.0f;
    *((float *)packet)++ = -NuTrigTable[0x6000];
    *((float *)packet)++ = 1.0f;
    *((float *)packet)++ = 1.0f;
    *((float *)packet)++ = 0.0f;
    *((float *)packet)++ = 0.0f;
    *((float *)packet)++ = 1.0f;
    *((float *)packet)++ = NuTrigTable[0x6000];
    *((float *)packet)++ = 0.0f;
    *((float *)packet)++ = -NuTrigTable[0xA000];
    *((float *)packet)++ = 1.0f;
    *((float *)packet)++ = 0.0f;
    *((float *)packet)++ = 1.0f;
    *((float *)packet)++ = 0.0f;
    vpDmaTag_Close(packet);
    return base;
}


u32 *NuRndrCylMakeData(void) {
    GifTagQuad strip;
    GifTagQuad fan;
    u32 *base;
    u32 *packet;

    strip.dword[0] = 0x3002400000008000ULL;
    strip.dword[1] = 0x512;
    fan.dword[0] = 0x3002C00000008000ULL;
    fan.dword[1] = 0x512;
    packet = cyl_packet;
    base = packet;
    packet = vpDmaTag_RetEx(base, 0, 0x6C010056);
    *packet++ = fan.word[0];
    *packet++ = fan.word[1];
    *packet++ = fan.word[2];
    *packet++ = fan.word[3];
    strip.tag.nloop = 10;
    *packet++ = 0x6C0180D2;
    *packet++ = strip.word[0];
    *packet++ = strip.word[1];
    *packet++ = strip.word[2];
    *packet++ = strip.word[3];
    *packet++ = 0x6D018002;
    *((u16 *)packet)++ = 0x1E;
    *((u16 *)packet)++ = 0;
    *((u16 *)packet)++ = 0;
    *((u16 *)packet)++ = 0;
    *packet++ = 0x6C0680AF;
    *packet++ = 5;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *((float *)packet)++ = 1.0f;
    *((float *)packet)++ = 0.0f;
    *((float *)packet)++ = 1.0f;
    *((float *)packet)++ = 0.0f;
    *((float *)packet)++ = -1.0f;
    *((float *)packet)++ = 0.0f;
    *((float *)packet)++ = 1.0f;
    *((float *)packet)++ = 0.0f;
    *((float *)packet)++ = -1.0f;
    *((float *)packet)++ = 0.0f;
    *((float *)packet)++ = -1.0f;
    *((float *)packet)++ = 0.0f;
    *((float *)packet)++ = 1.0f;
    *((float *)packet)++ = 0.0f;
    *((float *)packet)++ = -1.0f;
    *((float *)packet)++ = 0.0f;
    *((float *)packet)++ = 1.0f;
    *((float *)packet)++ = 0.0f;
    *((float *)packet)++ = 1.0f;
    *((float *)packet)++ = 0.0f;
    vpDmaTag_Close(packet);
    return base;
}


void NuRndrShadowStack(struct nuvec_s *light) {
    struct nurndrshadstore_s *store;
    struct nushadowmesh_s *mesh;
    int i;

    NuVpSetSize(4096.0f, PS2_REZ_H << 4);
    NuCameraSet(&global_camera);
    testcntp = 0;
    NuRndrStreamLink(rndrstream_fx);
    NuRndrStreamSetMisc(rndrstream_fx);
    NuRndrStreamAddMtl(rndrstream_fx, NuLightAddMat2);
    rndrstream_fx->cur.w = vpDmaTag_CntEx(rndrstream_fx->cur.w, 0x11000000, 0x50000000) - 1;
    rndrstream_fx->cur.w = vtBeginDirect(rndrstream_fx->cur.w, 0);
    rndrstream_fx->cur.w = gsBeginGifPacket(rndrstream_fx->cur.w, 0, 1, 0, 0, 0, 1, 0xE);
    *((u64 *)rndrstream_fx->cur.w)++ = 0;
    *((u64 *)rndrstream_fx->cur.w)++ = 0x46;
    rndrstream_fx->cur.w = gsCloseGifPacket(rndrstream_fx->cur.w);
    rndrstream_fx->cur.w = vtCloseDirect(rndrstream_fx->cur.w);
    rndrstream_fx->cur.w = vpDmaTag_Close(rndrstream_fx->cur.w);
    NuRndrSetLocalMtx(&numtx_identity, 2);
    NuRndrStreamSetViewMtxScissorClip(rndrstream_fx);
    NuRndrStreamSetLocalMtx(rndrstream_fx);
    NuRndrStreamFxMPG(1);
    testdone2 = 0;
    testdone = 0;
    for (i = 0; i < shadstorecnt; i++) {
        store = &shadstore[i];
        if (store->shadow.numcylinders != 0) {
            NuRndrShadowCylinderCap(&store->mtx, store->shadow.cylinders, store->shadow.numcylinders, light);
        }
        if (store->shadow.numellipsoids != 0) {
            NuRndrShadowEllipse(&store->mtx, store->shadow.ellipsoids, store->shadow.numellipsoids, light);
        }
        mesh = store->shadow.meshes;
        if (mesh != 0) {
            NuRndrShadowPols(&store->mtx, (struct nurndrshadface_s *)mesh->first,
                             (struct nurndrshadface_s *)mesh->second, light);
        }
    }
    NuRndrStreamFxMPG(0);
    rndrstream_free = vpDmaTag_Next(rndrstream_fx->cur.w, 0);
    NuVpSetSize(PS2_REZ_W << 4, PS2_REZ_H << 4);
    NuCameraSet(&global_camera);
    shadstorecnt = 0;
}


int NuHGobjRndrMtxDwa(struct NUHGOBJ_s *hobj, struct numtx_s *wm, int nlayers, short *layer,
                      struct numtx_s *mtx_array, float **dwa) {
    struct numtx_s mtx;
    struct nushadowdata_s *shadow;
    float *weights;
    int vis;
    int rv;
    int ix;
    int i;
    int j;

    rv = 0;
    if (hgobj_enabled == 0) {
        return 1;
    }
    vis = NuCameraClipTestExtents(&hobj->bbox_min, &hobj->bbox_max, wm);
    if (vis == 0) {
        return 0;
    }
    if (hobj->shadow != 0 && (u8)hobj->pad81[0] == 0) {
        for (shadow = hobj->shadow; shadow->last != 0xff; shadow++) {
            if (shadstorecnt < 256) {
                NuMtxMulVU0(&mtx, &mtx_array[shadow->last], wm);
                shadstore[shadstorecnt].mtx = mtx;
                shadstore[shadstorecnt].shadow.ellipsoids = shadow->ellipsoids;
                shadstore[shadstorecnt].shadow.cylinders = shadow->cylinders;
                if (shadow->nummeshes != 0) {
                    shadstore[shadstorecnt].shadow.meshes = shadow->meshes;
                } else {
                    shadstore[shadstorecnt].shadow.meshes = 0;
                }
                shadstore[shadstorecnt].shadow.numellipsoids = shadow->numellipsoids;
                shadstore[shadstorecnt].shadow.numcylinders = shadow->numcylinders;
                shadstore[shadstorecnt].shadow.nummeshes = shadow->nummeshes;
                shadstorecnt++;
            }
        }
    }
    for (i = 0; i < nlayers; i++) {
        if (layer != 0) {
            ix = layer[i];
            if (ix < 0) {
                continue;
            }
            if (ix >= hobj->num_layers) {
                continue;
            }
        } else {
            nlayers = 1;
            ix = 0;
        }
        if (dwa != 0) {
            weights = dwa[ix];
        } else {
            weights = 0;
        }
        if (hobj->layers[ix].gobjs != 0) {
            for (j = 0; j < hobj->num_joints; j++) {
                if (hobj->layers[ix].gobjs[j] != 0) {
                    NuMtxMulVU0(&mtx, &mtx_array[j], wm);
                    rv |= NuRndrGobj(hobj->layers[ix].gobjs[j], &mtx);
                }
            }
        }
        if (hobj->layers[ix].blend_gobjs != 0) {
            for (j = 0; j < hobj->num_joints; j++) {
                if (hobj->layers[ix].blend_gobjs[j] != 0) {
                    NuMtxMulVU0(&mtx, &mtx_array[j], wm);
                    rv |= NuRndrGobj(hobj->layers[ix].blend_gobjs[j], &mtx);
                }
            }
        }
        if (hobj->layers[ix].skin_object != 0 || hobj->layers[ix].blend_skin_object != 0) {
            skin_mtx_buffer = NuRndrStreamAlloc(hobj->num_joints * 4);
            NuMtxMulArrayVU0((struct numtx_s *)skin_mtx_buffer, hobj->INV_WT, mtx_array, hobj->num_joints);
            if (hobj->layers[ix].skin_object != 0) {
                rv |= NuRndrGobjSkin(hobj->layers[ix].skin_object, wm, hobj->num_joints, mtx_array, (u32 *)weights,
                                     vis);
            }
            if (hobj->layers[ix].blend_skin_object != 0) {
                rv |= NuRndrGobjSkin(hobj->layers[ix].blend_skin_object, wm, hobj->num_joints, mtx_array,
                                     (u32 *)weights, vis);
            }
        }
    }
    return rv;
}


void NuHGobjEval(struct NUHGOBJ_s *hobj, int njanims, struct NUJOINTANIM_s *janim, struct numtx_s *mtx_array) {
    struct nuvec_s scale[256];
    struct NUJOINTANIM_s *janim_array[256];
    struct nuangvec_s rot;
    struct numtx_s local;
    struct numtx_s offsetmtx;
    struct NUJOINTANIM_s *offset;
    struct numtx_s *m;
    struct numtx_s *wt;
    u8 i;
    u8 joint_ix;

    memset(janim_array, 0, sizeof(janim_array));
    scale[255].x = 1.0f;
    scale[255].y = 1.0f;
    scale[255].z = 1.0f;
    if (njanims != 0) {
        for (i = 0; i < njanims; i++) {
            if (janim[i].joint_id < hobj->num_joint_ixs) {
                joint_ix = hobj->joint_ixs[janim[i].joint_id];
                if (joint_ix != 0xff) {
                    janim_array[joint_ix] = &janim[i];
                }
            }
        }
    }

    for (i = 0; i < hobj->num_joints; i++) {
        wt = &mtx_array[i];
        if (njanims != 0) {
            offset = janim_array[i];
        } else {
            offset = 0;
        }
        if (offset != 0) {
            m = &local;
            local = hobj->T[i];
            rot.x = (int)(offset->rx * 10430.378f);
            rot.y = (int)(offset->ry * 10430.378f);
            rot.z = (int)(offset->rz * 10430.378f);
            NuMtxSetRotateXYZVU0(&offsetmtx, &rot);
            NuMtxTranslate(&offsetmtx, (struct nuvec_s *)&offset->tx);
            NuMtxMulVU0(m, &offsetmtx, m);
        } else {
            m = &hobj->T[i];
        }
        if ((u8)hobj->joints[i].parent_ix == 0xff) {
            *wt = *m;
        } else {
            NuMtxMulVU0(wt, m, &mtx_array[(u8)hobj->joints[i].parent_ix]);
        }
    }
}


void NuHGobjEvalAnim(struct NUHGOBJ_s *hobj, struct nuanimdata_s *animdata, float time, int njanims,
                     struct NUJOINTANIM_s *janim, struct numtx_s *mtx_array) {
    struct numtx_s local;
    struct nuvec_s scale[256];
    struct nuanimtime_s atime;
    struct nuanimdatachunk_s *chunk;
    struct NUJOINTANIM_s *janim_array[256];
    struct nuanimcurveset_s *curveset;
    struct NUJOINTANIM_s *offset;
    struct numtx_s *m;
    struct numtx_s *wt;
    u8 i;
    u8 joint_ix;

    memset(janim_array, 0, sizeof(janim_array));
    scale[255].x = 1.0f;
    scale[255].y = 1.0f;
    scale[255].z = 1.0f;
    NuAnimDataCalcTime(animdata, time, &atime);
    chunk = animdata->chunks[atime.chunk];
    if (njanims != 0) {
        for (i = 0; i < njanims; i++) {
            if (janim[i].joint_id < hobj->num_joint_ixs) {
                joint_ix = hobj->joint_ixs[janim[i].joint_id];
                if (joint_ix != 0xff) {
                    janim_array[joint_ix] = &janim[i];
                }
            }
        }
    }

    for (i = 0; i < hobj->num_joints; i++) {
        wt = &mtx_array[i];
        if (njanims != 0) {
            offset = janim_array[i];
        } else {
            offset = 0;
        }
        curveset = chunk->animcurvesets[i];
        if (curveset != 0) {
            if ((curveset->flags & 0x1a) != 0 || (hobj->joints[i].flags & 8) != 0) {
                NuAnimCurveSetApplyToJoint2(curveset, &atime, &hobj->joints[i], &scale[i],
                                            &scale[(u8)hobj->joints[i].parent_ix], &local, offset);
            } else {
                NuAnimCurveSetApplyToJointBasic(curveset, &atime, &hobj->joints[i], &scale[i],
                                                &scale[(u8)hobj->joints[i].parent_ix], &local, offset);
            }
            m = &local;
        } else {
            m = &hobj->T[i];
        }
        if ((u8)hobj->joints[i].parent_ix == 0xff) {
            *wt = *m;
        } else {
            NuMtxMulVU0(wt, m, &mtx_array[(u8)hobj->joints[i].parent_ix]);
        }
    }
}


void NuHGobjEvalAnim2(struct NUHGOBJ_s *hobj, struct nuanimdata2_s *animdata, float time, int njanims,
                      struct NUJOINTANIM_s *janim, struct numtx_s *mtx_array) {
    struct numtx_s local;
    struct nuvec_s scale[256];
    struct nuanimtime_s atime;
    struct NUJOINTANIM_s *janim_array[256];
    union nuanimcurve2_u *curves;
    signed char *curveflags;
    int curvesetflags;
    struct NUJOINTANIM_s *offset;
    struct numtx_s *m;
    struct numtx_s *wt;
    u8 i;
    u8 joint_ix;

    memset(janim_array, 0, sizeof(janim_array));
    scale[255].x = 1.0f;
    scale[255].y = 1.0f;
    scale[255].z = 1.0f;
    NuAnimData2CalcTime(animdata, time, &atime);
    if (njanims != 0) {
        for (i = 0; i < njanims; i++) {
            if (janim[i].joint_id < hobj->num_joint_ixs) {
                joint_ix = hobj->joint_ixs[janim[i].joint_id];
                if (joint_ix != 0xff) {
                    janim_array[joint_ix] = &janim[i];
                }
            }
        }
    }

    for (i = 0; i < hobj->num_joints; i++) {
        wt = &mtx_array[i];
        if (njanims != 0) {
            offset = janim_array[i];
        } else {
            offset = 0;
        }
        curvesetflags = animdata->curvesetflags[i];
        curves = &animdata->curves[animdata->ncurves * i];
        curveflags = &animdata->curveflags[animdata->ncurves * i];
        if ((curvesetflags & 0x1a) != 0 || (hobj->joints[i].flags & 8) != 0) {
            NuAnimCurve2SetApplyToJoint(curves, curveflags, curvesetflags, &atime, &hobj->joints[i], &scale[i],
                                        &scale[(u8)hobj->joints[i].parent_ix], &local, offset);
        } else {
            NuAnimCurve2SetApplyToJointBasic(curves, curveflags, curvesetflags, &atime, &hobj->joints[i], &scale[i],
                                             &scale[(u8)hobj->joints[i].parent_ix], &local, offset);
        }
        m = &local;
        if ((u8)hobj->joints[i].parent_ix == 0xff) {
            if ((curvesetflags & 0x40) != 0) {
                (scale + i)->x = 1.0f;
                (scale + i)->y = 1.0f;
                (scale + i)->z = 1.0f;
            }
            *wt = *m;
        } else {
            NuMtxMulVU0(wt, m, &mtx_array[(u8)hobj->joints[i].parent_ix]);
        }
    }
}


int NuHGobjRndrAnim(struct NUHGOBJ_s *hobj, struct numtx_s *wm, int layer, struct nuanimdata_s *animdata, float time,
                    struct nuanimdata_s *faceanim, float facetime) {
    struct numtx_s local;
    struct numtx_s mtx;
    struct nuvec_s scale[256];
    struct nuanimtime_s atime;
    struct nuanimtime_s faceatime;
    struct nuanimdatachunk_s *chunk;
    struct nuanimcurveset_s *curveset;
    struct nuanimcurveset_s *faceset;
    struct numtx_s *m;
    struct numtx_s *wt;
    float *weights;
    int ncurves;
    int vis;
    int rv;
    int i;

    vis = NuCameraClipTestExtents(&hobj->bbox_min, &hobj->bbox_max, wm);
    weights = 0;
    if (vis == 0) {
        return 0;
    }
    scale[255].x = 1.0f;
    scale[255].y = 1.0f;
    scale[255].z = 1.0f;
    if (animdata != 0) {
        NuAnimDataCalcTime(animdata, time, &atime);
        chunk = animdata->chunks[atime.chunk];
        for (i = 0; i < hobj->num_joints; i++) {
            wt = &hgobj_anim_mtx[i];
            curveset = chunk->animcurvesets[i];
            if (curveset != 0) {
                if ((curveset->flags & 0x1a) != 0 || (hobj->joints[i].flags & 8) != 0) {
                    NuAnimCurveSetApplyToJoint2(curveset, &atime, &hobj->joints[i], &scale[i],
                                                &scale[(u8)hobj->joints[i].parent_ix], &local, 0);
                } else {
                    NuAnimCurveSetApplyToJointBasic(curveset, &atime, &hobj->joints[i], &scale[i],
                                                    &scale[(u8)hobj->joints[i].parent_ix], &local, 0);
                }
                m = &local;
            } else {
                m = &hobj->T[i];
            }
            if ((u8)hobj->joints[i].parent_ix == 0xff) {
                *wt = *m;
            } else {
                NuMtxMulVU0(wt, m, &hgobj_anim_mtx[(u8)hobj->joints[i].parent_ix]);
            }
        }
    } else {
        for (i = 0; i < hobj->num_joints; i++) {
            wt = &hgobj_anim_mtx[i];
            m = &hobj->T[i];
            if ((u8)hobj->joints[i].parent_ix == 0xff) {
                NuMtxMul(wt, m, wm);
            } else {
                NuMtxMul(wt, m, &hgobj_anim_mtx[(u8)hobj->joints[i].parent_ix]);
            }
        }
    }
    if (faceanim != 0) {
        NuAnimDataCalcTime(faceanim, facetime, &faceatime);
        weights = (float *)NuRndrCreateBlendShapeDeformerWeightsArray(
            faceanim->chunks[faceatime.chunk]->animcurvesets[layer]->ncurves);
        if (weights != 0) {
            ncurves = faceanim->chunks[faceatime.chunk]->animcurvesets[layer]->ncurves;
            for (i = 0; i < ncurves; i++) {
                faceset = faceanim->chunks[faceatime.chunk]->animcurvesets[layer];
                if (faceset->set[i] != 0) {
                    weights[i + 6] = NuAnimCurveCalcVal2(faceset->set[i], &faceatime);
                } else {
                    weights[i + 6] = faceset->constants[i];
                }
            }
        }
    }
    rv = 0;
    if (hobj->layers[layer].gobjs != 0) {
        for (i = 0; i < hobj->num_joints; i++) {
            if (hobj->layers[layer].gobjs[i] != 0) {
                NuMtxMulVU0(&mtx, &hgobj_anim_mtx[i], wm);
                rv |= NuRndrGobj(hobj->layers[layer].gobjs[i], &mtx);
            }
        }
    }
    if (hobj->layers[layer].blend_gobjs != 0) {
        for (i = 0; i < hobj->num_joints; i++) {
            if (hobj->layers[layer].blend_gobjs[i] != 0) {
                NuMtxMulVU0(&mtx, &hgobj_anim_mtx[i], wm);
                rv |= NuRndrGobj(hobj->layers[layer].blend_gobjs[i], &mtx);
            }
        }
    }
    if (hobj->layers[layer].skin_object != 0 || hobj->layers[layer].blend_skin_object != 0) {
        skin_mtx_buffer = NuRndrStreamAlloc(hobj->num_joints * 4);
        NuMtxMulArrayVU0((struct numtx_s *)skin_mtx_buffer, hobj->INV_WT, hgobj_anim_mtx, hobj->num_joints);
        if (hobj->layers[layer].skin_object != 0) {
            rv |= NuRndrGobjSkin(hobj->layers[layer].skin_object, wm, hobj->num_joints, hgobj_anim_mtx, 0, vis);
        }
        if (hobj->layers[layer].blend_skin_object != 0) {
            rv |= NuRndrGobjSkin(hobj->layers[layer].blend_skin_object, wm, hobj->num_joints, hgobj_anim_mtx,
                                 (u32 *)weights, vis);
        }
    }
    return rv;
}


float **NuHGobjEvalDwa(int nlayers, short *layer, struct nuanimdata_s *animdata, float time) {
    struct nuanimtime_s atime;
    struct nuanimcurveset_s *curveset;
    float **dwa;
    float *weights;
    u32 qwc;
    int ncurves;
    int ix;
    int i;
    int j;

    dwa = 0;
    if (animdata != 0 && nlayers != 0) {
        NuAnimDataCalcTime(animdata, time, &atime);
        if (layer == 0) {
            nlayers = 1;
        }
        qwc = (u32)(nlayers + 3) >> 2;
        dwa = NuRndrStreamAlloc(qwc);
        memset(dwa, 0, qwc);
        if (dwa != 0) {
            for (i = 0; i < nlayers; i++) {
                if (layer != 0) {
                    ix = layer[i];
                    if (ix < 0) {
                        continue;
                    }
                    if (ix >= animdata->chunks[atime.chunk]->numnodes) {
                        dwa[i] = (float *)NuRndrCreateBlendShapeDeformerWeightsArray(0);
                        continue;
                    }
                } else {
                    ix = 0;
                }
                if (animdata->chunks[atime.chunk]->animcurvesets[ix] != 0) {
                    ncurves = animdata->chunks[atime.chunk]->animcurvesets[ix]->ncurves;
                    weights = (float *)NuRndrCreateBlendShapeDeformerWeightsArray(ncurves);
                    dwa[i] = weights;
                    if (weights != 0) {
                        for (j = 0; j < ncurves; j++) {
                            curveset = animdata->chunks[atime.chunk]->animcurvesets[ix];
                            if (curveset->set[j] != 0) {
                                weights[j + 6] = NuAnimCurveCalcVal2(curveset->set[j], &atime);
                            } else {
                                weights[j + 6] = curveset->constants[j];
                            }
                        }
                    }
                } else {
                    dwa[i] = (float *)NuRndrCreateBlendShapeDeformerWeightsArray(0);
                }
            }
        }
    }
    return dwa;
}


float **NuHGobjEvalDwa2(int nlayers, short *layer, struct nuanimdata2_s *animdata, float time) {
    struct nuanimtime_s atime;
    union nuanimcurve2_u *curves;
    signed char *curveflags;
    float **dwa;
    float *weights;
    u32 qwc;
    int ncurves;
    int ix;
    int i;
    int j;

    dwa = 0;
    if (animdata != 0 && nlayers != 0) {
        NuAnimData2CalcTime(animdata, time, &atime);
        if (layer == 0) {
            nlayers = 1;
        }
        qwc = (u32)(nlayers + 3) >> 2;
        dwa = NuRndrStreamAlloc(qwc);
        memset(dwa, 0, qwc);
        if (dwa != 0) {
            for (i = 0; i < nlayers; i++) {
                if (layer != 0) {
                    ix = layer[i];
                    if (ix < 0) {
                        continue;
                    }
                    if (ix >= animdata->nnodes) {
                        dwa[i] = (float *)NuRndrCreateBlendShapeDeformerWeightsArray(0);
                        continue;
                    }
                } else {
                    ix = 0;
                }
                ncurves = animdata->ncurves;
                weights = (float *)NuRndrCreateBlendShapeDeformerWeightsArray(ncurves);
                dwa[i] = weights;
                curveflags = &animdata->curveflags[animdata->ncurves * ix];
                curves = &animdata->curves[animdata->ncurves * ix];
                if (weights != 0) {
                    for (j = 0; j < ncurves; j++) {
                        weights[j + 6] = NuAnimCurve2CalcVal(&curves[j], &atime, curveflags[j]);
                    }
                }
            }
        }
    }
    return dwa;
}


float **NuHGobjEvalDwaBlend(int nlayers, short *layer, struct nuanimdata_s *animdata1, float time1,
                            struct nuanimdata_s *animdata2, float time2, float blend) {
    struct nuanimtime_s atime1;
    struct nuanimtime_s atime2;
    struct nuanimdatachunk_s *chunk;
    struct nuanimcurveset_s *curveset;
    float **dwa;
    float *weights;
    float value1;
    float value2;
    u32 qwc;
    int ncurves1;
    int ncurves2;
    int ncurves;
    int ix;
    int i;
    int j;

    dwa = 0;
    if ((animdata1 != 0 || animdata2 != 0) && nlayers != 0) {
        if (animdata1 != 0) {
            NuAnimDataCalcTime(animdata1, time1, &atime1);
        }
        if (animdata2 != 0) {
            NuAnimDataCalcTime(animdata2, time2, &atime2);
        }
        if (layer == 0) {
            nlayers = 1;
        }
        qwc = (u32)(nlayers + 3) >> 2;
        dwa = NuRndrStreamAlloc(qwc);
        memset(dwa, 0, qwc);
        if (dwa != 0) {
            for (i = 0; i < nlayers; i++) {
                if (layer != 0) {
                    ix = layer[i];
                    if (ix < 0) {
                        continue;
                    }
                } else {
                    ix = 0;
                }
                ncurves1 = 0;
                if (animdata1 != 0) {
                    chunk = animdata1->chunks[atime1.chunk];
                    if (ix < chunk->numnodes && chunk->animcurvesets[ix] != 0) {
                        ncurves1 = chunk->animcurvesets[ix]->ncurves;
                    }
                }
                ncurves2 = 0;
                if (animdata2 != 0) {
                    chunk = animdata2->chunks[atime2.chunk];
                    if (ix < chunk->numnodes && chunk->animcurvesets[ix] != 0) {
                        ncurves2 = chunk->animcurvesets[ix]->ncurves;
                    }
                }
                ncurves = ncurves2 < ncurves1 ? ncurves1 : ncurves2;
                weights = (float *)NuRndrCreateBlendShapeDeformerWeightsArray(ncurves);
                dwa[i] = weights;
                if (weights != 0) {
                    for (j = 0; j < ncurves; j++) {
                        if (j < ncurves1) {
                            curveset = animdata1->chunks[atime1.chunk]->animcurvesets[ix];
                            if (curveset->set[j] != 0) {
                                value1 = NuAnimCurveCalcVal2(curveset->set[j], &atime1);
                            } else {
                                value1 = curveset->constants[j];
                            }
                        } else {
                            value1 = 0.0f;
                        }
                        if (j < ncurves2) {
                            curveset = animdata2->chunks[atime2.chunk]->animcurvesets[ix];
                            if (curveset->set[j] != 0) {
                                value2 = NuAnimCurveCalcVal2(curveset->set[j], &atime2);
                            } else {
                                value2 = curveset->constants[j];
                            }
                        } else {
                            value2 = 0.0f;
                        }
                        weights[j + 6] = value2 * blend + value1 * (1.0f - blend);
                    }
                }
            }
        }
    }
    return dwa;
}


float **NuHGobjEvalDwaBlend2(int nlayers, short *layer, struct nuanimdata2_s *animdata1, float time1,
                             struct nuanimdata2_s *animdata2, float time2, float blend) {
    struct nuanimtime_s atime1;
    struct nuanimtime_s atime2;
    union nuanimcurve2_u *curves1;
    union nuanimcurve2_u *curves2;
    signed char *curveflags1;
    signed char *curveflags2;
    float **dwa;
    float *weights;
    float value1;
    float value2;
    u32 qwc;
    int ncurves1;
    int ncurves2;
    int ncurves;
    int ix;
    int i;
    int j;

    dwa = 0;
    if ((animdata1 != 0 || animdata2 != 0) && nlayers != 0) {
        if (animdata1 != 0) {
            NuAnimData2CalcTime(animdata1, time1, &atime1);
        }
        if (animdata2 != 0) {
            NuAnimData2CalcTime(animdata2, time2, &atime2);
        }
        if (layer == 0) {
            nlayers = 1;
        }
        qwc = (u32)(nlayers + 3) >> 2;
        dwa = NuRndrStreamAlloc(qwc);
        memset(dwa, 0, qwc);
        if (dwa != 0) {
            for (i = 0; i < nlayers; i++) {
                if (layer != 0) {
                    ix = layer[i];
                    if (ix < 0) {
                        continue;
                    }
                } else {
                    ix = 0;
                }
                ncurves1 = 0;
                if (animdata1 != 0 && ix < animdata1->nnodes) {
                    ncurves1 = animdata1->ncurves;
                    curveflags1 = &animdata1->curveflags[ncurves1 * ix];
                    curves1 = &animdata1->curves[ncurves1 * ix];
                }
                ncurves2 = 0;
                if (animdata2 != 0 && ix < animdata2->nnodes) {
                    ncurves2 = animdata2->ncurves;
                    curveflags2 = &animdata2->curveflags[ncurves2 * ix];
                    curves2 = &animdata2->curves[ncurves2 * ix];
                }
                ncurves = ncurves2 < ncurves1 ? ncurves1 : ncurves2;
                weights = (float *)NuRndrCreateBlendShapeDeformerWeightsArray(ncurves);
                dwa[i] = weights;
                if (weights != 0) {
                    for (j = 0; j < ncurves; j++) {
                        if (j < ncurves1) {
                            value1 = NuAnimCurve2CalcVal(&curves1[j], &atime1, curveflags1[j]);
                        } else {
                            value1 = 0.0f;
                        }
                        if (j < ncurves2) {
                            value2 = NuAnimCurve2CalcVal(&curves2[j], &atime2, curveflags2[j]);
                        } else {
                            value2 = 0.0f;
                        }
                        weights[j + 6] = value2 * blend + value1 * (1.0f - blend);
                    }
                }
            }
        }
    }
    return dwa;
}


void NuHGobjEvalAnimBlend(struct NUHGOBJ_s *hobj, struct nuanimdata_s *animdata1, float time1,
                          struct nuanimdata_s *animdata2, float time2, float blend, int njanims,
                          struct NUJOINTANIM_s *janim, struct numtx_s *mtx_array) {
    struct numtx_s local;
    struct nuvec_s scale[256];
    struct nuanimtime_s atime1;
    struct nuanimtime_s atime2;
    struct NUJOINTANIM_s *janim_array[256];
    struct nuanimdatachunk_s *chunk1;
    struct nuanimdatachunk_s *chunk2;
    struct NUJOINTANIM_s *offset;
    struct numtx_s *m;
    struct numtx_s *wt;
    u8 i;
    u8 joint_ix;

    memset(janim_array, 0, sizeof(janim_array));
    scale[255].x = 1.0f;
    scale[255].y = 1.0f;
    scale[255].z = 1.0f;
    NuAnimDataCalcTime(animdata1, time1, &atime1);
    chunk1 = animdata1->chunks[atime1.chunk];
    NuAnimDataCalcTime(animdata2, time2, &atime2);
    chunk2 = animdata2->chunks[atime2.chunk];
    if (njanims != 0) {
        for (i = 0; i < njanims; i++) {
            if (janim[i].joint_id < hobj->num_joint_ixs) {
                joint_ix = hobj->joint_ixs[janim[i].joint_id];
                if (joint_ix != 0xff) {
                    janim_array[joint_ix] = &janim[i];
                }
            }
        }
    }

    for (i = 0; i < hobj->num_joints; i++) {
        wt = &mtx_array[i];
        if (njanims != 0) {
            offset = janim_array[i];
        } else {
            offset = 0;
        }
        if (chunk1->animcurvesets[i] != 0 && chunk2->animcurvesets[i] != 0) {
            NuAnimCurveSetApplyBlendToJoint2(chunk1->animcurvesets[i], &atime1, chunk2->animcurvesets[i], &atime2,
                                             blend, &hobj->joints[i], &scale[i],
                                             &scale[(u8)hobj->joints[i].parent_ix], &local, offset);
            m = &local;
        } else {
            m = &hobj->T[i];
        }
        if ((u8)hobj->joints[i].parent_ix == 0xff) {
            *wt = *m;
        } else {
            NuMtxMulVU0(wt, m, &mtx_array[(u8)hobj->joints[i].parent_ix]);
        }
    }
}


void NuHGobjEvalAnimBlend2(struct NUHGOBJ_s *hobj, struct nuanimdata2_s *animdata1, float time1,
                           struct nuanimdata2_s *animdata2, float time2, float blend, int njanims,
                           struct NUJOINTANIM_s *janim, struct numtx_s *mtx_array) {
    struct numtx_s local;
    struct nuvec_s scale[512];
    struct nuanimtime_s atime1;
    struct nuanimtime_s atime2;
    struct NUJOINTANIM_s *janim_array[256];
    int curvesetflags1;
    int curvesetflags2;
    struct NUJOINTANIM_s *offset;
    struct numtx_s *m;
    struct numtx_s *wt;
    u8 i;
    u8 joint_ix;

    memset(janim_array, 0, sizeof(janim_array));
    scale[255].x = 1.0f;
    scale[255].y = 1.0f;
    scale[255].z = 1.0f;
    scale[511].x = 1.0f;
    scale[511].y = 1.0f;
    scale[511].z = 1.0f;
    NuAnimData2CalcTime(animdata1, time1, &atime1);
    NuAnimData2CalcTime(animdata2, time2, &atime2);
    if (njanims != 0) {
        for (i = 0; i < njanims; i++) {
            if (janim[i].joint_id < hobj->num_joint_ixs) {
                joint_ix = hobj->joint_ixs[janim[i].joint_id];
                if (joint_ix != 0xff) {
                    janim_array[joint_ix] = &janim[i];
                }
            }
        }
    }

    for (i = 0; i < hobj->num_joints; i++) {
        wt = &mtx_array[i];
        if (njanims != 0) {
            offset = janim_array[i];
        } else {
            offset = 0;
        }
        m = &local;
        curvesetflags1 = animdata1->curvesetflags[i];
        curvesetflags2 = animdata2->curvesetflags[i];
        NuAnimCurve2SetApplyBlendToJoint2(&animdata1->curves[animdata1->ncurves * i],
                                          &animdata1->curveflags[animdata1->ncurves * i], curvesetflags1, &atime1,
                                          &animdata2->curves[animdata2->ncurves * i],
                                          &animdata2->curveflags[animdata2->ncurves * i], curvesetflags2, &atime2,
                                          blend, &hobj->joints[i], &scale[i],
                                          &scale[(u8)hobj->joints[i].parent_ix], m, offset);
        if ((u8)hobj->joints[i].parent_ix == 0xff) {
            if ((curvesetflags1 & 0x40) != 0) {
                (scale + i)->x = 1.0f;
                (scale + i)->y = 1.0f;
                (scale + i)->z = 1.0f;
            }
            *wt = *m;
        } else {
            NuMtxMulVU0(wt, m, &mtx_array[(u8)hobj->joints[i].parent_ix]);
        }
    }
}


int NuHGobjRndrAnimBlend(struct NUHGOBJ_s *hobj, struct numtx_s *wm, int layer, struct nuanimdata_s *animdata1,
                         struct nuanimdata_s *animdata2, float time1, float time2, float blend) {
    struct numtx_s local;
    struct numtx_s mtx;
    struct nuvec_s scale[256];
    struct nuanimtime_s atime1;
    struct nuanimtime_s atime2;
    struct nuanimdatachunk_s *chunk1;
    struct nuanimdatachunk_s *chunk2;
    struct numtx_s *m;
    struct numtx_s *wt;
    int vis;
    int rv;
    int i;

    vis = NuCameraClipTestExtents(&hobj->bbox_min, &hobj->bbox_max, wm);
    if (vis == 0) {
        return 0;
    }
    scale[255].x = 1.0f;
    scale[255].y = 1.0f;
    scale[255].z = 1.0f;
    NuAnimDataCalcTime(animdata1, time1, &atime1);
    chunk1 = animdata1->chunks[atime1.chunk];
    NuAnimDataCalcTime(animdata2, time2, &atime2);
    chunk2 = animdata2->chunks[atime2.chunk];
    for (i = 0; i < hobj->num_joints; i++) {
        wt = &hgobj_blend_mtx[i];
        if (chunk1->animcurvesets[i] != 0 && chunk2->animcurvesets[i] != 0) {
            NuAnimCurveSetApplyBlendToJoint2(chunk1->animcurvesets[i], &atime1, chunk2->animcurvesets[i], &atime2,
                                             blend, &hobj->joints[i], &scale[i],
                                             &scale[(u8)hobj->joints[i].parent_ix], &local, 0);
            m = &local;
        } else {
            m = &hobj->T[i];
        }
        if ((u8)hobj->joints[i].parent_ix == 0xff) {
            *wt = *m;
        } else {
            NuMtxMulVU0(wt, m, &hgobj_blend_mtx[(u8)hobj->joints[i].parent_ix]);
        }
    }
    rv = 0;
    if (hobj->layers[layer].gobjs != 0) {
        for (i = 0; i < hobj->num_joints; i++) {
            if (hobj->layers[layer].gobjs[i] != 0) {
                NuMtxMulVU0(&mtx, &hgobj_blend_mtx[i], wm);
                rv |= NuRndrGobj(hobj->layers[layer].gobjs[i], &mtx);
            }
        }
    }
    if (hobj->layers[layer].blend_gobjs != 0) {
        for (i = 0; i < hobj->num_joints; i++) {
            if (hobj->layers[layer].blend_gobjs[i] != 0) {
                NuMtxMulVU0(&mtx, &hgobj_blend_mtx[i], wm);
                rv |= NuRndrGobj(hobj->layers[layer].blend_gobjs[i], &mtx);
            }
        }
    }
    if (hobj->layers[layer].skin_object != 0 || hobj->layers[layer].blend_skin_object != 0) {
        skin_mtx_buffer = NuRndrStreamAlloc(hobj->num_joints * 4);
        NuMtxMulArrayVU0((struct numtx_s *)skin_mtx_buffer, hobj->INV_WT, hgobj_blend_mtx, hobj->num_joints);
        if (hobj->layers[layer].skin_object != 0) {
            rv |= NuRndrGobjSkin(hobj->layers[layer].skin_object, wm, hobj->num_joints, hgobj_blend_mtx, 0, vis);
        }
        if (hobj->layers[layer].blend_skin_object != 0) {
            rv |= NuRndrGobjSkin(hobj->layers[layer].blend_skin_object, wm, hobj->num_joints, hgobj_blend_mtx, 0,
                                 vis);
        }
    }
    return rv;
}


int NuRndrLine2d(struct nuvtx2d_s *vtx, struct numtl_s *mtl) {
    int i;

    if (!mtl) {
        mtl = numtl_defaultmtl2d;
    }
    NuRndrStreamLink(rndrstream_2d);
    NuRndrStreamSetMisc(rndrstream_2d);
    NuRndrStreamAddMtl(rndrstream_2d, mtl);
    rndrstream_2d->cur.w = vpDmaTag_Cnt(rndrstream_2d->cur.w);
    *rndrstream_2d->cur.w++ = 0;
    *rndrstream_2d->cur.w++ = 0;
    *rndrstream_2d->cur.w++ = 0x11000000;
    *rndrstream_2d->cur.w++ = 0x50000007;
    *((u64 *)rndrstream_2d->cur.w)++ = 0x3000C00000008002ULL;
    *((u64 *)rndrstream_2d->cur.w)++ = 0x512;
    for (i = 0; i < 2; i++) {
        *((float *)rndrstream_2d->cur.w)++ = vtx->u;
        *((float *)rndrstream_2d->cur.w)++ = vtx->v;
        *((float *)rndrstream_2d->cur.w)++ = vtx->rhw;
        rndrstream_2d->cur.w++;
        *rndrstream_2d->cur.w++ = vtx->rgba[0];
        *rndrstream_2d->cur.w++ = vtx->rgba[1];
        *rndrstream_2d->cur.w++ = vtx->rgba[2];
        *rndrstream_2d->cur.w++ = vtx->rgba[3];
        *rndrstream_2d->cur.w++ = (int)NuVpPixelWidth(vtx->x);
        *rndrstream_2d->cur.w++ = (int)NuVpPixelHeight(vtx->y);
        *rndrstream_2d->cur.w++ = (int)vtx->z;
        *rndrstream_2d->cur.w++ = 0;
        vtx++;
    }
    rndrstream_2d->cur.w = vpDmaTag_Close(rndrstream_2d->cur.w);
    rndrstream_free = vpDmaTag_Next(rndrstream_2d->cur.w, 0);
    return 1;
}


int NuRndrLineStrip2d(struct nuvtx2d_s *vtx, struct numtl_s *mtl, int count) {
    int i;

    if (!mtl) {
        mtl = numtl_defaultmtl2d;
    }
    NuRndrStreamLink(rndrstream_2d);
    NuRndrStreamSetMisc(rndrstream_2d);
    NuRndrStreamAddMtl(rndrstream_2d, mtl);
    rndrstream_2d->cur.w = vpDmaTag_Cnt(rndrstream_2d->cur.w);
    *rndrstream_2d->cur.w++ = 0;
    *rndrstream_2d->cur.w++ = 0;
    *rndrstream_2d->cur.w++ = 0x10000000;
    *rndrstream_2d->cur.w++ = (count * 3 + 1) | 0x50000000;
    *((u64 *)rndrstream_2d->cur.w)++ = (count | 0x8000) | 0x3000C00000000000ULL;
    *((u64 *)rndrstream_2d->cur.w)++ = 0x512;
    for (i = 0; i < count; i++) {
        *((float *)rndrstream_2d->cur.w)++ = vtx->u;
        *((float *)rndrstream_2d->cur.w)++ = vtx->v;
        *((float *)rndrstream_2d->cur.w)++ = vtx->rhw;
        rndrstream_2d->cur.w++;
        *rndrstream_2d->cur.w++ = vtx->rgba[0];
        *rndrstream_2d->cur.w++ = vtx->rgba[1];
        *rndrstream_2d->cur.w++ = vtx->rgba[2];
        *rndrstream_2d->cur.w++ = vtx->rgba[3];
        *rndrstream_2d->cur.w++ = (int)NuVpPixelWidth(vtx->x);
        *rndrstream_2d->cur.w++ = (int)NuVpPixelHeight(vtx->y);
        *rndrstream_2d->cur.w++ = (int)vtx->z;
        *rndrstream_2d->cur.w++ = 0;
        vtx++;
    }
    rndrstream_2d->cur.w = vpDmaTag_Close(rndrstream_2d->cur.w);
    rndrstream_free = vpDmaTag_Next(rndrstream_2d->cur.w, 0);
    return 1;
}


void ClipLinePoint(struct nuvec4_s *scr0, struct nuvec4_s *scr1, struct nuvec4_s *p0, struct nuvec4_s *p1) {
    float w;
    float t;

    w = NuFabs(p0->w);
    if (w < p0->x) {
        t = (p0->x - p0->w) / (p1->w - p0->w - p1->x + p0->x);
        NuVec4Lerp(scr0, scr1, scr0, t);
        NuVec4Lerp(p0, p1, p0, t);
        w = NuFabs(p0->w);
    }
    if (p0->x < -w) {
        t = (-p0->x - p0->w) / (p1->x - p0->x + p1->w - p0->w);
        NuVec4Lerp(scr0, scr1, scr0, t);
        NuVec4Lerp(p0, p1, p0, t);
        w = NuFabs(p0->w);
    }
    if (w < p0->y) {
        t = (p0->y - p0->w) / (p1->w - p0->w - p1->y + p0->y);
        NuVec4Lerp(scr0, scr1, scr0, t);
        NuVec4Lerp(p0, p1, p0, t);
        w = NuFabs(p0->w);
    }
    if (p0->y < -w) {
        t = (-p0->y - p0->w) / (p1->y - p0->y + p1->w - p0->w);
        NuVec4Lerp(scr0, scr1, scr0, t);
        NuVec4Lerp(p0, p1, p0, t);
        w = NuFabs(p0->w);
    }
    if (w < p0->z) {
        t = (p0->w - p0->z) / (p1->z - p0->z - p1->w + p0->w);
        NuVec4Lerp(scr0, scr1, scr0, t);
        NuVec4Lerp(p0, p1, p0, t);
        w = NuFabs(p0->w);
    }
    if (p0->z < -w) {
        t = (-p0->z - p0->w) / (p1->z - p0->z + p1->w - p0->w);
        NuVec4Lerp(scr0, scr1, scr0, t);
        NuVec4Lerp(p0, p1, p0, t);
        NuFabs(p0->w);
    }
}


int NuRndrLine3d(struct nuvtx_tc1_s *vtx, struct numtl_s *mtl, struct numtx_s *wm) {
    struct numtx_s vpcs_local;
    struct numtx_s vpc_local;
    struct nuvec4_s pts[2];
    struct nuvec4_s clippts[2];
    int clip[2];
    struct nucolour_s colours[2];
    struct numtx_s *vpcs;
    struct numtx_s *vpc;
    float rhw;
    int i;

    colours[0] = vtx[0].rgba;
    colours[1] = vtx[1].rgba;
    vpcs = NuCameraGetVPCSMtx();
    vpc = NuCameraGetVPCMtx();
    if (wm) {
        NuMtxMulH(&vpcs_local, wm, vpcs);
        vpcs = &vpcs_local;
        NuMtxMulH(&vpc_local, wm, vpc);
        vpc = &vpc_local;
    }
    clip[0] = NuCameraClipTestPoints(&vtx[0].pnt, 1, wm);
    clip[1] = NuCameraClipTestPoints(&vtx[1].pnt, 1, wm);
    if (clip[0] & clip[1]) {
        return 0;
    }

    pts[0].x = vtx[0].pnt.x;
    pts[0].y = vtx[0].pnt.y;
    pts[0].z = vtx[0].pnt.z;
    pts[0].w = 1.0f;
    pts[1].x = vtx[1].pnt.x;
    pts[1].y = vtx[1].pnt.y;
    pts[1].z = vtx[1].pnt.z;
    pts[1].w = 1.0f;
    if (clip[0] | clip[1]) {
        NuVec4MtxTransformH(&clippts[0], &pts[0], vpc);
        NuVec4MtxTransformH(&clippts[1], &pts[1], vpc);
        NuVec4MtxTransformH(&pts[0], &pts[0], vpcs);
        NuVec4MtxTransformH(&pts[1], &pts[1], vpcs);
        if (clip[0]) {
            ClipLinePoint(&pts[0], &pts[1], &clippts[0], &clippts[1]);
        }
        if (clip[1]) {
            ClipLinePoint(&pts[1], &pts[0], &clippts[1], &clippts[0]);
        }
    } else {
        NuVec4MtxTransformH(&pts[0], &pts[0], vpcs);
        NuVec4MtxTransformH(&pts[1], &pts[1], vpcs);
    }
    if (!mtl) {
        mtl = numtl_defaultmtl3d;
    }

    NuRndrStreamLink(rndrstream_2d);
    NuRndrStreamSetMisc(rndrstream_2d);
    NuRndrStreamAddMtl(rndrstream_2d, mtl);
    rndrstream_2d->cur.w = vpDmaTag_Cnt(rndrstream_2d->cur.w);
    *rndrstream_2d->cur.w++ = 0;
    *rndrstream_2d->cur.w++ = 0;
    *rndrstream_2d->cur.w++ = 0x10000000;
    *rndrstream_2d->cur.w++ = 0x50000007;
    *((u64 *)rndrstream_2d->cur.w)++ = 0x3000C00000008002ULL;
    *((u64 *)rndrstream_2d->cur.w)++ = 0x512;
    for (i = 0; i < 2; i++) {
        rhw = 1.0f / pts[i].w;
        *rndrstream_2d->cur.w++ = 0;
        *rndrstream_2d->cur.w++ = 0;
        *(float *)rndrstream_2d->cur.w++ = 1.0f;
        rndrstream_2d->cur.w++;
        *rndrstream_2d->cur.w++ = colours[i].r;
        *rndrstream_2d->cur.w++ = colours[i].g;
        *rndrstream_2d->cur.w++ = colours[i].b;
        *rndrstream_2d->cur.w++ = colours[i].a;
        *rndrstream_2d->cur.w++ = (int)(pts[i].x * rhw);
        *rndrstream_2d->cur.w++ = (int)(pts[i].y * rhw);
        *rndrstream_2d->cur.w++ = (int)(pts[i].z * rhw);
        *rndrstream_2d->cur.w++ = 0;
    }
    rndrstream_2d->cur.w = vpDmaTag_Close(rndrstream_2d->cur.w);
    rndrstream_free = vpDmaTag_Next(rndrstream_2d->cur.w, 0);
    return 1;
}


int NuRndrTri2d(struct nuvtx2d_s *vtx, struct numtl_s *mtl) {
    int i;

    if (!mtl) {
        mtl = numtl_defaultmtl2d;
    }
    NuRndrStreamLink(rndrstream_2d);
    NuRndrStreamSetMisc(rndrstream_2d);
    NuRndrStreamAddMtl(rndrstream_2d, mtl);
    rndrstream_2d->cur.w = vpDmaTag_Cnt(rndrstream_2d->cur.w);
    *rndrstream_2d->cur.w++ = 0;
    *rndrstream_2d->cur.w++ = 0;
    *rndrstream_2d->cur.w++ = 0x10000000;
    *rndrstream_2d->cur.w++ = 0x5000000A;
    *((u64 *)rndrstream_2d->cur.w)++ = 0x3001C00000008003ULL;
    *((u64 *)rndrstream_2d->cur.w)++ = 0x512;
    for (i = 0; i < 3; i++) {
        *((float *)rndrstream_2d->cur.w)++ = vtx->u;
        *((float *)rndrstream_2d->cur.w)++ = vtx->v;
        *((float *)rndrstream_2d->cur.w)++ = vtx->rhw;
        rndrstream_2d->cur.w++;
        *rndrstream_2d->cur.w++ = vtx->rgba[0];
        *rndrstream_2d->cur.w++ = vtx->rgba[1];
        *rndrstream_2d->cur.w++ = vtx->rgba[2];
        *rndrstream_2d->cur.w++ = vtx->rgba[3];
        *rndrstream_2d->cur.w++ = (int)NuVpPixelWidth(vtx->x);
        *rndrstream_2d->cur.w++ = (int)NuVpPixelHeight(vtx->y);
        *rndrstream_2d->cur.w++ = (int)vtx->z;
        *rndrstream_2d->cur.w++ = 0;
        vtx++;
    }
    rndrstream_2d->cur.w = vpDmaTag_Close(rndrstream_2d->cur.w);
    rndrstream_free = vpDmaTag_Next(rndrstream_2d->cur.w, 0);
    return 1;
}


int NuRndrTriStrip2d(struct nuvtx2d_s *vtx, struct numtl_s *mtl, int count) {
    int i;

    if (!mtl) {
        mtl = numtl_defaultmtl2d;
    }
    NuRndrStreamLink(rndrstream_2d);
    NuRndrStreamSetMisc(rndrstream_2d);
    NuRndrStreamAddMtl(rndrstream_2d, mtl);
    rndrstream_2d->cur.w = vpDmaTag_Cnt(rndrstream_2d->cur.w);
    *rndrstream_2d->cur.w++ = 0;
    *rndrstream_2d->cur.w++ = 0;
    *rndrstream_2d->cur.w++ = 0x10000000;
    *rndrstream_2d->cur.w++ = (count * 3 + 1) | 0x50000000;
    *((u64 *)rndrstream_2d->cur.w)++ = (count | 0x8000) | 0x3002400000000000ULL;
    *((u64 *)rndrstream_2d->cur.w)++ = 0x512;
    for (i = 0; i < count; i++) {
        *((float *)rndrstream_2d->cur.w)++ = vtx->u;
        *((float *)rndrstream_2d->cur.w)++ = vtx->v;
        *((float *)rndrstream_2d->cur.w)++ = vtx->rhw;
        rndrstream_2d->cur.w++;
        *rndrstream_2d->cur.w++ = vtx->rgba[0];
        *rndrstream_2d->cur.w++ = vtx->rgba[1];
        *rndrstream_2d->cur.w++ = vtx->rgba[2];
        *rndrstream_2d->cur.w++ = vtx->rgba[3];
        *rndrstream_2d->cur.w++ = (int)NuVpPixelWidth(vtx->x);
        *rndrstream_2d->cur.w++ = (int)NuVpPixelHeight(vtx->y);
        *rndrstream_2d->cur.w++ = (int)vtx->z;
        *rndrstream_2d->cur.w++ = 0;
        vtx++;
    }
    rndrstream_2d->cur.w = vpDmaTag_Close(rndrstream_2d->cur.w);
    rndrstream_free = vpDmaTag_Next(rndrstream_2d->cur.w, 0);
    return 1;
}


int NuRndrTriStrip3d(struct nuvtx_tc1_s *vtx, int count, struct numtl_s *mtl, struct numtx_s *wm) {
    struct numtx_s local;
    struct nuvec_s pos;
    struct numtx_s *m;
    struct nurndrstream_s *stream;

    m = NuCameraGetVPCSMtx();
    if (wm) {
        NuMtxMulH(&local, wm, m);
        m = &local;
    }
    if (!mtl) {
        mtl = numtl_defaultmtl3d;
    }

    NuRndrStreamLink(rndrstream_2d);
    NuRndrStreamSetMisc(rndrstream_2d);
    NuRndrStreamAddMtl(rndrstream_2d, mtl);
    stream = rndrstream_2d;
    stream->cur.w = vpDmaTag_CntEx(stream->cur.w, 0x11000000, 0x50000000) - 1;
    stream->cur.w = vtBeginDirect(stream->cur.w, 0);
    stream->cur.w = gsBeginGifPacket(stream->cur.w, 0, 1, 1, 4, 0, 3, 1, 2, 5);

    while (count != 0) {
        count--;
        NuVecMtxTransformH(&pos, &vtx->pnt, m);
        *stream->cur.w++ = vtx->rgba.r;
        *stream->cur.w++ = vtx->rgba.g;
        *stream->cur.w++ = vtx->rgba.b;
        *stream->cur.w++ = vtx->rgba.a;
        *(float *)stream->cur.w++ = vtx->tc[0];
        *(float *)stream->cur.w++ = vtx->tc[1];
        *(float *)stream->cur.w++ = 1.0f;
        stream->cur.w++;
        *stream->cur.w++ = (int)pos.x;
        *stream->cur.w++ = (int)pos.y;
        *stream->cur.w++ = (int)pos.z;
        *stream->cur.w++ = 0;
        vtx++;
    }

    stream->cur.w = gsCloseGifPacket(stream->cur.w);
    stream->cur.w = vtCloseDirect(stream->cur.w);
    stream->cur.w = vpDmaTag_Close(stream->cur.w);
    rndrstream_free = vpDmaTag_Next(rndrstream_2d->cur.w, 0);
    return 1;
}


void NuRndrRect2di(int x, int y, int width, int height, int colour, struct numtl_s *mtl) {
    x += (PS2_VCNTR_X - (PS2_REZ_W >> 1)) << 4;
    y += (PS2_VCNTR_Y - (PS2_REZ_H >> 1)) << 4;
    if (!mtl) {
        mtl = numtl_defaultmtl2d;
    }

    NuRndrStreamLink(rndrstream_2d);
    NuRndrStreamSetMisc(rndrstream_2d);
    NuRndrStreamAddMtl(rndrstream_2d, mtl);
    rndrstream_2d->cur.w = vpDmaTag_CntEx(rndrstream_2d->cur.w,
                                      0x10000000, 0x50000000) - 1;
    rndrstream_2d->cur.w = vtBeginDirect(rndrstream_2d->cur.w, 0);
    rndrstream_2d->cur.w = gsBeginGifPacket(rndrstream_2d->cur.w,
        0, 1, 1, 6, 0, 5, 1, 2, 5, 2, 5);

    *rndrstream_2d->cur.w++ = colour & 0xff;
    *rndrstream_2d->cur.w++ = (colour >> 8) & 0xff;
    *rndrstream_2d->cur.w++ = (colour >> 16) & 0xff;
    *rndrstream_2d->cur.w++ = (colour >> 24) & 0xff;

    *rndrstream_2d->cur.w++ = 0;
    *rndrstream_2d->cur.w++ = 0;
    *(float *)rndrstream_2d->cur.w++ = 1.0f;
    rndrstream_2d->cur.w++;
    *rndrstream_2d->cur.w++ = x;
    *rndrstream_2d->cur.w++ = y;
    *rndrstream_2d->cur.w++ = 0;
    *rndrstream_2d->cur.w++ = 0;

    *(float *)rndrstream_2d->cur.w++ = 1.0f;
    *(float *)rndrstream_2d->cur.w++ = 1.0f;
    *(float *)rndrstream_2d->cur.w++ = 1.0f;
    rndrstream_2d->cur.w++;
    *rndrstream_2d->cur.w++ = x + width;
    *rndrstream_2d->cur.w++ = y + height;
    *rndrstream_2d->cur.w++ = 0;
    *rndrstream_2d->cur.w++ = 0;

    rndrstream_2d->cur.w = gsCloseGifPacket(rndrstream_2d->cur.w);
    rndrstream_2d->cur.w = vtCloseDirect(rndrstream_2d->cur.w);
    rndrstream_2d->cur.w = vpDmaTag_Close(rndrstream_2d->cur.w);
    rndrstream_free = vpDmaTag_Next(rndrstream_2d->cur.w, 0);
}


void NuRndrGradRect2di(int x, int y, int width, int height,
                      unsigned char *colours, struct numtl_s *mtl) {
    x += (PS2_VCNTR_X - (PS2_REZ_W >> 1)) << 4;
    y += (PS2_VCNTR_Y - (PS2_REZ_H >> 1)) << 4;
    if (!mtl) {
        mtl = numtl_defaultmtl2d;
    }

    NuRndrStreamLink(rndrstream_2d);
    NuRndrStreamSetMisc(rndrstream_2d);
    NuRndrStreamAddMtl(rndrstream_2d, mtl);
    rndrstream_2d->cur.w = vpDmaTag_CntEx(rndrstream_2d->cur.w,
                                      0x10000000, 0x50000000) - 1;
    rndrstream_2d->cur.w = vtBeginDirect(rndrstream_2d->cur.w, 0);
    rndrstream_2d->cur.w = gsBeginGifPacket(rndrstream_2d->cur.w,
        0, 1, 1, 4, 0, 9, 1, 2, 5, 1, 5, 1, 5, 1, 5);

    *rndrstream_2d->cur.w++ = colours[0];
    *rndrstream_2d->cur.w++ = colours[1];
    *rndrstream_2d->cur.w++ = colours[2];
    *rndrstream_2d->cur.w++ = colours[3];
    *rndrstream_2d->cur.w++ = 0;
    *rndrstream_2d->cur.w++ = 0;
    *(float *)rndrstream_2d->cur.w++ = 1.0f;
    rndrstream_2d->cur.w++;
    *rndrstream_2d->cur.w++ = x;
    *rndrstream_2d->cur.w++ = y;
    *rndrstream_2d->cur.w++ = 0;
    *rndrstream_2d->cur.w++ = 0;

    *rndrstream_2d->cur.w++ = colours[4];
    *rndrstream_2d->cur.w++ = colours[5];
    *rndrstream_2d->cur.w++ = colours[6];
    *rndrstream_2d->cur.w++ = colours[7];
    *rndrstream_2d->cur.w++ = x + width;
    *rndrstream_2d->cur.w++ = y;
    *rndrstream_2d->cur.w++ = 0;
    *rndrstream_2d->cur.w++ = 0;

    *rndrstream_2d->cur.w++ = colours[8];
    *rndrstream_2d->cur.w++ = colours[9];
    *rndrstream_2d->cur.w++ = colours[10];
    *rndrstream_2d->cur.w++ = colours[11];
    *rndrstream_2d->cur.w++ = x;
    *rndrstream_2d->cur.w++ = y + height;
    *rndrstream_2d->cur.w++ = 0;
    *rndrstream_2d->cur.w++ = 0;

    *rndrstream_2d->cur.w++ = colours[12];
    *rndrstream_2d->cur.w++ = colours[13];
    *rndrstream_2d->cur.w++ = colours[14];
    *rndrstream_2d->cur.w++ = colours[15];
    *rndrstream_2d->cur.w++ = x + width;
    *rndrstream_2d->cur.w++ = y + height;
    *rndrstream_2d->cur.w++ = 0;
    *rndrstream_2d->cur.w++ = 0;

    rndrstream_2d->cur.w = gsCloseGifPacket(rndrstream_2d->cur.w);
    rndrstream_2d->cur.w = vtCloseDirect(rndrstream_2d->cur.w);
    rndrstream_2d->cur.w = vpDmaTag_Close(rndrstream_2d->cur.w);
    rndrstream_free = vpDmaTag_Next(rndrstream_2d->cur.w, 0);
}


void NuRndrRectUV2di(int x, int y, int width, int height, float u0, float v0, float u1, float v1,
                     int colour, struct numtl_s *mtl) {
    x += (PS2_VCNTR_X - (nurndr_pixel_width >> 1)) << 4;
    y += (PS2_VCNTR_Y - (nurndr_pixel_height >> 1)) << 4;
    if (!mtl) {
        mtl = numtl_defaultmtl2d;
    }

    NuRndrStreamLink(rndrstream_2d);
    NuRndrStreamSetMisc(rndrstream_2d);
    NuRndrStreamAddMtl(rndrstream_2d, mtl);
    rndrstream_2d->cur.w = vpDmaTag_CntEx(rndrstream_2d->cur.w,
                                      0x10000000, 0x50000000) - 1;
    rndrstream_2d->cur.w = vtBeginDirect(rndrstream_2d->cur.w, 0);
    rndrstream_2d->cur.w = gsBeginGifPacket(rndrstream_2d->cur.w,
        0, 1, 1, 6, 0, 5, 1, 2, 5, 2, 5);

    *rndrstream_2d->cur.w++ = colour & 0xff;
    *rndrstream_2d->cur.w++ = (colour >> 8) & 0xff;
    *rndrstream_2d->cur.w++ = (colour >> 16) & 0xff;
    *rndrstream_2d->cur.w++ = (colour >> 24) & 0xff;

    *(float *)rndrstream_2d->cur.w++ = u0;
    *(float *)rndrstream_2d->cur.w++ = v0;
    *(float *)rndrstream_2d->cur.w++ = 1.0f;
    rndrstream_2d->cur.w++;
    *rndrstream_2d->cur.w++ = x;
    *rndrstream_2d->cur.w++ = y;
    *rndrstream_2d->cur.w++ = 0;
    *rndrstream_2d->cur.w++ = 0;

    *(float *)rndrstream_2d->cur.w++ = u1;
    *(float *)rndrstream_2d->cur.w++ = v1;
    *(float *)rndrstream_2d->cur.w++ = 1.0f;
    rndrstream_2d->cur.w++;
    *rndrstream_2d->cur.w++ = x + width;
    *rndrstream_2d->cur.w++ = y + height;
    *rndrstream_2d->cur.w++ = 0;
    *rndrstream_2d->cur.w++ = 0;

    rndrstream_2d->cur.w = gsCloseGifPacket(rndrstream_2d->cur.w);
    rndrstream_2d->cur.w = vtCloseDirect(rndrstream_2d->cur.w);
    rndrstream_2d->cur.w = vpDmaTag_Close(rndrstream_2d->cur.w);
    rndrstream_free = vpDmaTag_Next(rndrstream_2d->cur.w, 0);
}


void NuRndrLine2di(int x0, int y0, int x1, int y1, int colour, struct numtl_s *mtl) {
    int xoffset = (PS2_VCNTR_X - (PS2_REZ_W >> 1)) << 4;
    int yoffset = (PS2_VCNTR_Y - (PS2_REZ_H >> 1)) << 4;

    x0 += xoffset;
    y0 += yoffset;
    x1 += xoffset;
    y1 += yoffset;
    if (!mtl) {
        mtl = numtl_defaultmtl2d;
    }

    NuRndrStreamLink(rndrstream_2d);
    NuRndrStreamSetMisc(rndrstream_2d);
    NuRndrStreamAddMtl(rndrstream_2d, mtl);
    rndrstream_2d->cur.w = vpDmaTag_CntEx(rndrstream_2d->cur.w,
                                      0x10000000, 0x50000000) - 1;
    rndrstream_2d->cur.w = vtBeginDirect(rndrstream_2d->cur.w, 0);
    rndrstream_2d->cur.w = gsBeginGifPacket(rndrstream_2d->cur.w,
        0, 1, 1, 1, 0, 3, 1, 5, 5);

    *rndrstream_2d->cur.w++ = colour & 0xff;
    *rndrstream_2d->cur.w++ = (colour >> 8) & 0xff;
    *rndrstream_2d->cur.w++ = (colour >> 16) & 0xff;
    *rndrstream_2d->cur.w++ = (colour >> 24) & 0xff;
    *rndrstream_2d->cur.w++ = x0;
    *rndrstream_2d->cur.w++ = y0;
    *rndrstream_2d->cur.w++ = 0;
    *rndrstream_2d->cur.w++ = 0;
    *rndrstream_2d->cur.w++ = x1;
    *rndrstream_2d->cur.w++ = y1;
    *rndrstream_2d->cur.w++ = 0;
    *rndrstream_2d->cur.w++ = 0;

    rndrstream_2d->cur.w = gsCloseGifPacket(rndrstream_2d->cur.w);
    rndrstream_2d->cur.w = vtCloseDirect(rndrstream_2d->cur.w);
    rndrstream_2d->cur.w = vpDmaTag_Close(rndrstream_2d->cur.w);
    rndrstream_free = vpDmaTag_Next(rndrstream_2d->cur.w, 0);
}


void NuRndrLineRect2di(int x, int y, int width, int height, int colour, struct numtl_s *mtl) {
    int xoffset = (PS2_VCNTR_X - (PS2_REZ_W >> 1)) << 4;
    int yoffset = (PS2_VCNTR_Y - (PS2_REZ_H >> 1)) << 4;

    x += xoffset;
    y += yoffset;
    if (!mtl) {
        mtl = numtl_defaultmtl2d;
    }

    NuRndrStreamLink(rndrstream_2d);
    height -= 8;
    width -= 16;
    NuRndrStreamSetMisc(rndrstream_2d);
    NuRndrStreamAddMtl(rndrstream_2d, mtl);
    rndrstream_2d->cur.w = vpDmaTag_CntEx(rndrstream_2d->cur.w,
                                      0x10000000, 0x50000000) - 1;
    rndrstream_2d->cur.w = vtBeginDirect(rndrstream_2d->cur.w, 0);
    rndrstream_2d->cur.w = gsBeginGifPacket(rndrstream_2d->cur.w,
        0, 1, 1, 2, 0, 6, 1, 5, 5, 5, 5, 5);

    *rndrstream_2d->cur.w++ = colour & 0xff;
    *rndrstream_2d->cur.w++ = (colour >> 8) & 0xff;
    *rndrstream_2d->cur.w++ = (colour >> 16) & 0xff;
    *rndrstream_2d->cur.w++ = (colour >> 24) & 0xff;
    *rndrstream_2d->cur.w++ = x;
    *rndrstream_2d->cur.w++ = y;
    *rndrstream_2d->cur.w++ = 0;
    *rndrstream_2d->cur.w++ = 0;
    *rndrstream_2d->cur.w++ = x + width;
    *rndrstream_2d->cur.w++ = y;
    *rndrstream_2d->cur.w++ = 0;
    *rndrstream_2d->cur.w++ = 0;
    *rndrstream_2d->cur.w++ = x + width;
    *rndrstream_2d->cur.w++ = y + height;
    *rndrstream_2d->cur.w++ = 0;
    *rndrstream_2d->cur.w++ = 0;
    *rndrstream_2d->cur.w++ = x;
    *rndrstream_2d->cur.w++ = y + height;
    *rndrstream_2d->cur.w++ = 0;
    *rndrstream_2d->cur.w++ = 0;
    *rndrstream_2d->cur.w++ = x;
    *rndrstream_2d->cur.w++ = y;
    *rndrstream_2d->cur.w++ = 0;
    *rndrstream_2d->cur.w++ = 0;

    rndrstream_2d->cur.w = gsCloseGifPacket(rndrstream_2d->cur.w);
    rndrstream_2d->cur.w = vtCloseDirect(rndrstream_2d->cur.w);
    rndrstream_2d->cur.w = vpDmaTag_Close(rndrstream_2d->cur.w);
    rndrstream_free = vpDmaTag_Next(rndrstream_2d->cur.w, 0);
}


void NuRndrTri2di(int *pnts, float *uvs, int count, int colour, struct numtl_s *mtl) {
    int i;

    if (!mtl) {
        mtl = numtl_defaultmtl2d;
    }

    NuRndrStreamLink(rndrstream_2d);
    NuRndrStreamSetMisc(rndrstream_2d);
    NuRndrStreamAddMtl(rndrstream_2d, mtl);
    rndrstream_2d->cur.w = vpDmaTag_CntEx(rndrstream_2d->cur.w,
                                      0x10000000, 0x50000000) - 1;
    rndrstream_2d->cur.w = vtBeginDirect(rndrstream_2d->cur.w, 0);
    rndrstream_2d->cur.w = gsBeginGifPacket(rndrstream_2d->cur.w,
        0, 1, 1, 3, 0, 3, 1, 2, 5);

    while (count != 0) {
        count--;
        for (i = 0; i < 3; i++) {
            *rndrstream_2d->cur.w++ = colour & 0xff;
            *rndrstream_2d->cur.w++ = (colour >> 8) & 0xff;
            *rndrstream_2d->cur.w++ = (colour >> 16) & 0xff;
            *rndrstream_2d->cur.w++ = (colour >> 24) & 0xff;
            if (uvs) {
                *(float *)rndrstream_2d->cur.w++ = *uvs++;
                *(float *)rndrstream_2d->cur.w++ = *uvs++;
            } else {
                *rndrstream_2d->cur.w++ = 0;
                *rndrstream_2d->cur.w++ = 0;
            }
            *(float *)rndrstream_2d->cur.w++ = 1.0f;
            rndrstream_2d->cur.w++;
            *rndrstream_2d->cur.w++ = *pnts++ + ((PS2_VCNTR_X - (nurndr_pixel_width >> 1)) << 4);
            *rndrstream_2d->cur.w++ = *pnts++ + ((PS2_VCNTR_Y - (nurndr_pixel_height >> 1)) << 4);
            *rndrstream_2d->cur.w++ = tri2dz;
            *rndrstream_2d->cur.w++ = 0;
        }
    }

    rndrstream_2d->cur.w = gsCloseGifPacket(rndrstream_2d->cur.w);
    rndrstream_2d->cur.w = vtCloseDirect(rndrstream_2d->cur.w);
    rndrstream_2d->cur.w = vpDmaTag_Close(rndrstream_2d->cur.w);
    rndrstream_free = vpDmaTag_Next(rndrstream_2d->cur.w, 0);
}


void NuRndrTriStrip2di(int *pnts, float *uvs, int count, int colour, struct numtl_s *mtl) {
    if (!mtl) {
        mtl = numtl_defaultmtl2d;
    }

    NuRndrStreamLink(rndrstream_2d);
    NuRndrStreamSetMisc(rndrstream_2d);
    NuRndrStreamAddMtl(rndrstream_2d, mtl);
    rndrstream_2d->cur.w = vpDmaTag_CntEx(rndrstream_2d->cur.w,
                                      0x10000000, 0x50000000) - 1;
    rndrstream_2d->cur.w = vtBeginDirect(rndrstream_2d->cur.w, 0);
    rndrstream_2d->cur.w = gsBeginGifPacket(rndrstream_2d->cur.w,
        0, 1, 1, 4, 0, 3, 1, 2, 5);

    while (count != 0) {
        count--;
        *rndrstream_2d->cur.w++ = colour & 0xff;
        *rndrstream_2d->cur.w++ = (colour >> 8) & 0xff;
        *rndrstream_2d->cur.w++ = (colour >> 16) & 0xff;
        *rndrstream_2d->cur.w++ = (colour >> 24) & 0xff;
        if (uvs) {
            *(float *)rndrstream_2d->cur.w++ = *uvs++;
            *(float *)rndrstream_2d->cur.w++ = *uvs++;
        } else {
            *rndrstream_2d->cur.w++ = 0;
            *rndrstream_2d->cur.w++ = 0;
        }
        *(float *)rndrstream_2d->cur.w++ = 1.0f;
        rndrstream_2d->cur.w++;
        *rndrstream_2d->cur.w++ = *pnts++ + ((PS2_VCNTR_X - (PS2_REZ_W >> 1)) << 4);
        *rndrstream_2d->cur.w++ = *pnts++ + ((PS2_VCNTR_Y - (PS2_REZ_H >> 1)) << 4);
        *rndrstream_2d->cur.w++ = 0;
        *rndrstream_2d->cur.w++ = 0;
    }

    rndrstream_2d->cur.w = gsCloseGifPacket(rndrstream_2d->cur.w);
    rndrstream_2d->cur.w = vtCloseDirect(rndrstream_2d->cur.w);
    rndrstream_2d->cur.w = vpDmaTag_Close(rndrstream_2d->cur.w);
    rndrstream_free = vpDmaTag_Next(rndrstream_2d->cur.w, 0);
}


void NuRndrLineStrip2di(int *pnts, float *uvs, int count, int colour, struct numtl_s *mtl) {
    if (!mtl) {
        mtl = numtl_defaultmtl2d;
    }

    NuRndrStreamLink(rndrstream_2d);
    NuRndrStreamSetMisc(rndrstream_2d);
    NuRndrStreamAddMtl(rndrstream_2d, mtl);
    rndrstream_2d->cur.w = vpDmaTag_CntEx(rndrstream_2d->cur.w,
                                      0x10000000, 0x50000000) - 1;
    rndrstream_2d->cur.w = vtBeginDirect(rndrstream_2d->cur.w, 0);
    rndrstream_2d->cur.w = gsBeginGifPacket(rndrstream_2d->cur.w,
        0, 1, 1, 2, 0, 3, 1, 2, 5);

    while (count != 0) {
        count--;
        *rndrstream_2d->cur.w++ = colour & 0xff;
        *rndrstream_2d->cur.w++ = (colour >> 8) & 0xff;
        *rndrstream_2d->cur.w++ = (colour >> 16) & 0xff;
        *rndrstream_2d->cur.w++ = (colour >> 24) & 0xff;
        if (uvs) {
            *(float *)rndrstream_2d->cur.w++ = *uvs++;
            *(float *)rndrstream_2d->cur.w++ = *uvs++;
        } else {
            *rndrstream_2d->cur.w++ = 0;
            *rndrstream_2d->cur.w++ = 0;
        }
        *(float *)rndrstream_2d->cur.w++ = 1.0f;
        rndrstream_2d->cur.w++;
        *rndrstream_2d->cur.w++ = *pnts++ + ((PS2_VCNTR_X - (PS2_REZ_W >> 1)) << 4);
        *rndrstream_2d->cur.w++ = *pnts++ + ((PS2_VCNTR_Y - (PS2_REZ_H >> 1)) << 4);
        *rndrstream_2d->cur.w++ = 0;
        *rndrstream_2d->cur.w++ = 0;
    }

    rndrstream_2d->cur.w = gsCloseGifPacket(rndrstream_2d->cur.w);
    rndrstream_2d->cur.w = vtCloseDirect(rndrstream_2d->cur.w);
    rndrstream_2d->cur.w = vpDmaTag_Close(rndrstream_2d->cur.w);
    rndrstream_free = vpDmaTag_Next(rndrstream_2d->cur.w, 0);
}


int NuRndrQuad2d(struct nuvtx2d_s *vtx, struct numtl_s *mtl) {
    int i;

    if (!mtl) {
        mtl = numtl_defaultmtl2d;
    }
    NuRndrStreamLink(rndrstream_2d);
    NuRndrStreamSetMisc(rndrstream_2d);
    NuRndrStreamAddMtl(rndrstream_2d, mtl);
    rndrstream_2d->cur.w = vpDmaTag_Cnt(rndrstream_2d->cur.w);
    *rndrstream_2d->cur.w++ = 0;
    *rndrstream_2d->cur.w++ = 0;
    *rndrstream_2d->cur.w++ = 0x10000000;
    *rndrstream_2d->cur.w++ = 0x5000000D;
    *((u64 *)rndrstream_2d->cur.w)++ = 0x3002400000008004ULL;
    *((u64 *)rndrstream_2d->cur.w)++ = 0x512;
    for (i = 0; i < 4; i++) {
        *((float *)rndrstream_2d->cur.w)++ = vtx->u * vtx->rhw;
        *((float *)rndrstream_2d->cur.w)++ = vtx->v * vtx->rhw;
        *((float *)rndrstream_2d->cur.w)++ = vtx->rhw;
        rndrstream_2d->cur.w++;
        *rndrstream_2d->cur.w++ = vtx->rgba[0];
        *rndrstream_2d->cur.w++ = vtx->rgba[1];
        *rndrstream_2d->cur.w++ = vtx->rgba[2];
        *rndrstream_2d->cur.w++ = vtx->rgba[3];
        *rndrstream_2d->cur.w++ = (int)NuVpPixelWidth(vtx->x);
        *rndrstream_2d->cur.w++ = (int)NuVpPixelHeight(vtx->y);
        *rndrstream_2d->cur.w++ = (int)vtx->z;
        *rndrstream_2d->cur.w++ = 0;
        vtx++;
    }
    rndrstream_2d->cur.w = vpDmaTag_Close(rndrstream_2d->cur.w);
    rndrstream_free = vpDmaTag_Next(rndrstream_2d->cur.w, 0);
    return 1;
}


int NuRndrTriStripGif(u32 *data, int count, struct numtl_s *mtl) {
    int i;

    if (!mtl) {
        mtl = numtl_defaultmtl2d;
    }
    NuRndrStreamLink(rndrstream_2d);
    NuRndrStreamSetMisc(rndrstream_2d);
    NuRndrStreamAddMtl(rndrstream_2d, mtl);
    rndrstream_2d->cur.w = vpDmaTag_Cnt(rndrstream_2d->cur.w);
    *rndrstream_2d->cur.w++ = 0;
    *rndrstream_2d->cur.w++ = 0;
    *rndrstream_2d->cur.w++ = 0x10000000;
    *rndrstream_2d->cur.w++ = (count * 3 + 1) | 0x50000000;
    *((u64 *)rndrstream_2d->cur.w)++ = (count | 0x8000) | 0x3002400000000000ULL;
    *((u64 *)rndrstream_2d->cur.w)++ = 0x512;
    for (i = 0; i < count; i++) {
        *((float *)rndrstream_2d->cur.w)++ = *((float *)data)++;
        *((float *)rndrstream_2d->cur.w)++ = *((float *)data)++;
        *((float *)rndrstream_2d->cur.w)++ = *((float *)data)++;
        *((float *)rndrstream_2d->cur.w)++ = *((float *)data)++;
        *rndrstream_2d->cur.w++ = *data++;
        *rndrstream_2d->cur.w++ = *data++;
        *rndrstream_2d->cur.w++ = *data++;
        *rndrstream_2d->cur.w++ = *data++;
        *rndrstream_2d->cur.w++ = *data++;
        *rndrstream_2d->cur.w++ = *data++;
        *rndrstream_2d->cur.w++ = *data++;
        *rndrstream_2d->cur.w++ = *data++;
    }
    rndrstream_2d->cur.w = vpDmaTag_Close(rndrstream_2d->cur.w);
    rndrstream_free = vpDmaTag_Next(rndrstream_2d->cur.w, 0);
    return 1;
}


int NuRndrOldFaceOn(struct faceon_s *faceon, struct numtl_s *mtl, struct numtx_s *wm) {
    struct nuvec4_s pnts[4];
    struct nuvec_s pos;
    struct numtx_s *pcs;
    float wc;
    float ws;
    float hc;
    float hs;
    int i;

    if (!mtl) {
        mtl = numtl_defaultmtl3d;
    }
    if (NuCameraClipTestPoints(&faceon->pos, 1, wm) == 0) {
        NuCameraTransformView(&pos, &faceon->pos, 1, wm);
        if (faceon->sinang == 0.0f && faceon->cosang == 1.0f) {
            pnts[0].x = pos.x - faceon->width;
            pnts[0].y = pos.y - faceon->height;
            pnts[0].z = pos.z;
            pnts[0].w = 1.0f;
            pnts[1].x = pos.x + faceon->width;
            pnts[1].y = pos.y - faceon->height;
            pnts[1].z = pos.z;
            pnts[1].w = 1.0f;
            pnts[2].x = pos.x - faceon->width;
            pnts[2].y = pos.y + faceon->height;
            pnts[2].z = pos.z;
            pnts[2].w = 1.0f;
            pnts[3].x = pos.x + faceon->width;
            pnts[3].y = pos.y + faceon->height;
            pnts[3].z = pos.z;
            pnts[3].w = 1.0f;
        } else {
            wc = faceon->width * faceon->cosang;
            ws = faceon->width * faceon->sinang;
            hc = faceon->height * faceon->cosang;
            hs = faceon->height * faceon->sinang;
            pnts[0].x = pos.x - wc + hs;
            pnts[0].y = pos.y + ws + hc;
            pnts[0].z = pos.z;
            pnts[0].w = 1.0f;
            pnts[1].x = pos.x + wc + hs;
            pnts[1].y = pos.y - ws + hc;
            pnts[1].z = pos.z;
            pnts[1].w = 1.0f;
            pnts[2].x = pos.x - wc - hs;
            pnts[2].y = pos.y + ws - hc;
            pnts[2].z = pos.z;
            pnts[2].w = 1.0f;
            pnts[3].x = pos.x + wc - hs;
            pnts[3].y = pos.y - ws - hc;
            pnts[3].z = pos.z;
            pnts[3].w = 1.0f;
        }
        pcs = NuCameraGetPCSMtx();
        rndrstream_3d = &mtl->rs;
        NuRndrStreamLink(rndrstream_3d);
        NuRndrStreamSetMisc(rndrstream_3d);
        rndrstream_3d->cur.w = vpDmaTag_Cnt(rndrstream_3d->cur.w);
        *rndrstream_3d->cur.w++ = 0;
        *rndrstream_3d->cur.w++ = 0;
        *rndrstream_3d->cur.w++ = 0x10000000;
        *rndrstream_3d->cur.w++ = 0x5000000D;
        *((u64 *)rndrstream_3d->cur.w)++ = 0x3002400000008004ULL;
        *((u64 *)rndrstream_3d->cur.w)++ = 0x512;
        for (i = 0; i < 4; i++) {
            NuVec4MtxTransformH(&pnts[i], &pnts[i], pcs);
            pnts[i].w = 1.0f / pnts[i].w;
            *rndrstream_3d->cur.f++ = faceon->tc[oldfaceon_uvindex[i][0]] * pnts[i].w;
            *rndrstream_3d->cur.f++ = faceon->tc[oldfaceon_uvindex[i][1]] * pnts[i].w;
            *rndrstream_3d->cur.f++ = pnts[i].w;
            rndrstream_3d->cur.w++;
            *rndrstream_3d->cur.w++ = faceon->colour[0];
            *rndrstream_3d->cur.w++ = faceon->colour[1];
            *rndrstream_3d->cur.w++ = faceon->colour[2];
            *rndrstream_3d->cur.w++ = faceon->colour[3];
            *rndrstream_3d->cur.i++ = pnts[i].x * pnts[i].w;
            *rndrstream_3d->cur.i++ = pnts[i].y * pnts[i].w;
            *rndrstream_3d->cur.i++ = pnts[i].z * pnts[i].w;
            *rndrstream_3d->cur.w++ = 0;
        }
        rndrstream_3d->cur.w = vpDmaTag_Close(rndrstream_3d->cur.w);
        rndrstream_free = vpDmaTag_Next(rndrstream_3d->cur.w, 0);
    }
    return 0;
}


int NuRndrFaceOn(struct faceon_s *faceon, struct numtl_s *mtl, struct numtx_s *wm) {
    struct nuvec4_s pnts[4];
    struct nuvec4_s pos;
    float ratiox;
    float ratioy;
    struct numtx_s *vpcs;
    struct nuviewport_s *vp;
    float rw;
    float zoffset;
    float wc;
    float ws;
    float hs;
    float hc;
    int i;

    if (!mtl) {
        mtl = numtl_defaultmtl3d;
    }
    if (NuCameraClipTestPoints(&faceon->pos, 1, wm) == 0) {
        vpcs = NuCameraGetVPCSMtx();
        NuCameraGetClippingRatios(&ratiox, &ratioy);
        vp = NuVpGetCurrentViewport();
        pos.x = faceon->pos.x;
        pos.y = faceon->pos.y;
        pos.z = faceon->pos.z;
        pos.w = 1.0f;
        NuVec4MtxTransformVU0(&pos, &pos, vpcs);
        rw = 1.0f / pos.w;
        NuVec4ScaleXYZVU0(&pos, &pos, rw);
        ratiox = (float)vp->width / (pos.w * ratiox * 2.0f);
        ratioy = (float)vp->height / (pos.w * ratioy * 2.0f);
        if (faceon->sinang == 0.0f && faceon->cosang == 1.0f) {
            faceon->width *= ratiox;
            faceon->height *= ratioy;
            zoffset = rw * rw * 500000.0f;
            pnts[0].x = pos.x - faceon->width;
            pnts[0].y = pos.y - faceon->height;
            pnts[0].z = pos.z + zoffset;
            pnts[1].x = pos.x + faceon->width;
            pnts[1].y = pos.y - faceon->height;
            pnts[1].z = pos.z + zoffset;
            pnts[2].x = pos.x - faceon->width;
            pnts[2].y = pos.y + faceon->height;
            pnts[2].z = pos.z + zoffset;
            pnts[3].x = pos.x + faceon->width;
            pnts[3].y = pos.y + faceon->height;
            pnts[3].z = pos.z + zoffset;
        } else {
            wc = faceon->width * faceon->cosang;
            ws = faceon->width * faceon->sinang;
            hc = faceon->height * faceon->cosang;
            hs = faceon->height * faceon->sinang;
            pnts[0].x = pos.x + ratiox * (-wc + hs);
            pnts[0].y = pos.y + ratioy * (ws + hc);
            pnts[0].z = pos.z;
            pnts[1].x = pos.x + ratiox * (wc + hs);
            pnts[1].y = pos.y + ratioy * (-ws + hc);
            pnts[1].z = pos.z;
            pnts[2].x = pos.x + ratiox * (-wc - hs);
            pnts[2].y = pos.y + ratioy * (ws - hc);
            pnts[2].z = pos.z;
            pnts[3].x = pos.x + ratiox * (wc - hs);
            pnts[3].y = pos.y + ratioy * (-ws - hc);
            pnts[3].z = pos.z;
        }
        rndrstream_3d = &mtl->rs;
        NuRndrStreamLink(rndrstream_3d);
        NuRndrStreamSetMisc(rndrstream_3d);
        rndrstream_3d->cur.w = vpDmaTag_CntEx(rndrstream_3d->cur.w, 0x10000000, 0x5000000D);
        *((u64 *)rndrstream_3d->cur.w)++ = 0x3002400000008004ULL;
        *((u64 *)rndrstream_3d->cur.w)++ = 0x512;
        for (i = 0; i < 4; i++) {
            *rndrstream_3d->cur.f++ = faceon->tc[faceon_uvindex[i][0]];
            *rndrstream_3d->cur.f++ = faceon->tc[faceon_uvindex[i][1]];
            *rndrstream_3d->cur.f++ = 1.0f;
            rndrstream_3d->cur.w++;
            *rndrstream_3d->cur.w++ = faceon->colour[0];
            *rndrstream_3d->cur.w++ = faceon->colour[1];
            *rndrstream_3d->cur.w++ = faceon->colour[2];
            *rndrstream_3d->cur.w++ = faceon->colour[3];
            NuVecConvertToIntVU0(rndrstream_3d->cur.w, (struct nuvec_s *)&pnts[i]);
            rndrstream_3d->cur.w += 3;
            *rndrstream_3d->cur.w++ = 0;
        }
        rndrstream_3d->cur.w = vpDmaTag_Close(rndrstream_3d->cur.w);
        rndrstream_free = vpDmaTag_Next(rndrstream_3d->cur.w, 0);
    }
    return 0;
}


int NuRndrFaceY(struct faceon_s *faceon, struct numtl_s *mtl, struct numtx_s *wm) {
    struct nuvec4_s pnts[4];
    struct nuvec_s view;
    struct numtx_s *pcs;
    int i;

    if (!mtl) {
        mtl = numtl_defaultmtl3d;
    }
    if (NuCameraClipTestPoints(&faceon->pos, 1, wm) == 0) {
        NuCameraTransformView(&view, &faceon->pos, 1, wm);
        pnts[0].x = view.x - faceon->width;
        pnts[0].y = view.y - faceon->height;
        pnts[0].z = view.z;
        pnts[0].w = 1.0f;
        pnts[1].x = view.x + faceon->width;
        pnts[1].y = view.y - faceon->height;
        pnts[1].z = view.z;
        pnts[1].w = 1.0f;
        pnts[2].x = view.x - faceon->width;
        pnts[2].y = view.y + faceon->height;
        pnts[2].z = view.z;
        pnts[2].w = 1.0f;
        pnts[3].x = view.x + faceon->width;
        pnts[3].y = view.y + faceon->height;
        pnts[3].z = view.z;
        pnts[3].w = 1.0f;
        pcs = NuCameraGetPCSMtx();
        rndrstream_3d = &mtl->rs;
        NuRndrStreamLink(rndrstream_3d);
        NuRndrStreamSetMisc(rndrstream_3d);
        rndrstream_3d->cur.w = vpDmaTag_Cnt(rndrstream_3d->cur.w);
        *rndrstream_3d->cur.w++ = 0;
        *rndrstream_3d->cur.w++ = 0;
        *rndrstream_3d->cur.w++ = 0x10000000;
        *rndrstream_3d->cur.w++ = 0x5000000D;
        *((u64 *)rndrstream_3d->cur.w)++ = 0x3002400000008004ULL;
        *((u64 *)rndrstream_3d->cur.w)++ = 0x512;
        for (i = 0; i < 4; i++) {
            NuVec4MtxTransformH(&pnts[i], &pnts[i], pcs);
            pnts[i].w = 1.0f / pnts[i].w;
            *rndrstream_3d->cur.f++ = faceon->tc[facey_uvindex[i][0]] * pnts[i].w;
            *rndrstream_3d->cur.f++ = faceon->tc[facey_uvindex[i][1]] * pnts[i].w;
            *rndrstream_3d->cur.f++ = pnts[i].w;
            rndrstream_3d->cur.w++;
            *rndrstream_3d->cur.w++ = faceon->colour[0];
            *rndrstream_3d->cur.w++ = faceon->colour[1];
            *rndrstream_3d->cur.w++ = faceon->colour[2];
            *rndrstream_3d->cur.w++ = faceon->colour[3];
            *rndrstream_3d->cur.w++ = (int)(pnts[i].x * pnts[i].w);
            *rndrstream_3d->cur.w++ = (int)(pnts[i].y * pnts[i].w);
            *rndrstream_3d->cur.w++ = (int)(pnts[i].z * pnts[i].w);
            *rndrstream_3d->cur.w++ = 0;
        }
        rndrstream_3d->cur.w = vpDmaTag_Close(rndrstream_3d->cur.w);
        rndrstream_free = vpDmaTag_Next(rndrstream_3d->cur.w, 0);
    }
    return 0;
}


void NuRndrRectUV3di(int x, int y, int width, int height, float u0, float v0, float u1, float v1,
                     int colour, int z) {
    x += (PS2_VCNTR_X - (nurndr_pixel_width >> 1)) << 4;
    y += (PS2_VCNTR_Y - (nurndr_pixel_height >> 1)) << 4;

    rndrstream_3d->cur.w = vpDmaTag_CntEx(rndrstream_3d->cur.w,
                                      0x10000000, 0x50000000) - 1;
    rndrstream_3d->cur.w = vtBeginDirect(rndrstream_3d->cur.w, 0);
    *((u64 *)rndrstream_3d->cur.w)++ = 0x6403400000008001ULL;
    *((u64 *)rndrstream_3d->cur.w)++ = 0x525210;
    *((u64 *)rndrstream_3d->cur.w)++ = 6;

    *rndrstream_3d->cur.w++ = colour;
    *rndrstream_3d->cur.f++ = 1.0f;
    *rndrstream_3d->cur.f++ = u0;
    *rndrstream_3d->cur.f++ = v0;
    *((short *)rndrstream_3d->cur.w)++ = x;
    *((short *)rndrstream_3d->cur.w)++ = y;
    *rndrstream_3d->cur.w++ = z;

    *rndrstream_3d->cur.f++ = u1;
    *rndrstream_3d->cur.f++ = v1;
    *((short *)rndrstream_3d->cur.w)++ = x + width;
    *((short *)rndrstream_3d->cur.w)++ = y + height;
    *rndrstream_3d->cur.w++ = z;

    rndrstream_3d->cur.w = vtCloseDirect(rndrstream_3d->cur.w);
    rndrstream_3d->cur.w = vpDmaTag_Close(rndrstream_3d->cur.w);
}


void NuRndrRectUVStream(struct nurndrstream_s *stream, int x, int y, int width, int height,
                        float u0, float v0, float u1, float v1, int colour, int z) {
    int xoffset = (PS2_VCNTR_X - (nurndr_pixel_width >> 1)) << 4;
    int yoffset = (PS2_VCNTR_Y - (nurndr_pixel_height >> 1)) << 4;

    x += xoffset;
    y += yoffset;
    stream->cur.w = vpDmaTag_CntEx(stream->cur.w, 0x10000000, 0x50000000) - 1;
    stream->cur.w = vtBeginDirect(stream->cur.w, 0);
    stream->cur.w = gsBeginGifPacket(stream->cur.w, 0, 1, 1, 6, 0, 5, 1, 2, 5, 2, 5);

    *stream->cur.w++ = colour & 0xff;
    *stream->cur.w++ = (colour >> 8) & 0xff;
    *stream->cur.w++ = (colour >> 16) & 0xff;
    *stream->cur.w++ = (colour >> 24) & 0xff;

    *stream->cur.f++ = u0;
    *stream->cur.f++ = v0;
    *stream->cur.f++ = 1.0f;
    stream->cur.w++;
    *stream->cur.w++ = x;
    *stream->cur.w++ = y;
    *stream->cur.w++ = z;
    *stream->cur.w++ = 0;

    *stream->cur.f++ = u1;
    *stream->cur.f++ = v1;
    *stream->cur.f++ = 1.0f;
    stream->cur.w++;
    *stream->cur.w++ = x + width;
    *stream->cur.w++ = y + height;
    *stream->cur.w++ = z;
    *stream->cur.w++ = 0;

    stream->cur.w = gsCloseGifPacket(stream->cur.w);
    stream->cur.w = vtCloseDirect(stream->cur.w);
    stream->cur.w = vpDmaTag_Close(stream->cur.w);
}


extern void NuVec4ScaleVU0(struct nuvec4_s *dst, struct nuvec4_s *src, float scale);
extern struct nurndrstream_s *rndrstream_3d_u;
extern struct numtl_s *NuLgtBackBuff;
extern float waveampx;
extern float waveampy;

void NuRndrWater(void) {
    struct nuvec4_s nearpnt;
    struct nuvec4_s farpnt;
    struct numtx_s *pcs;
    struct nurndrstream_s *stream;
    struct nurndrstream_s *screen;
    struct nurndrstream_s *display;
    int target;
    int eop;
    int ang;
    int i;
    int j;
    int xoffset;
    int yoffset;
    int x0;
    int y0;
    int x1;
    int y1;
    int x2;
    int y2;
    int x3;
    int y3;
    int dx0;
    int dy0;
    int dx1;
    int dy1;
    int dx2;
    int dy2;
    int dx3;
    int dy3;
    float u0;
    float u1;
    float v0;
    float v1;

    if (NuRndrWaterLevel > -10000.0f && NuRndrWaterLevel > global_camera.mtx._31 - 0.5f) {
        pcs = NuCameraGetPCSMtx();
        ang = NuAtan2D(global_camera.mtx._11, global_camera.mtx._21);
        nearpnt.x = 0.0f;
        nearpnt.y = ((NuRndrWaterLevel - global_camera.mtx._31)
                     - global_camera.nearclip * NuTrigTable[(ang + 0x4000) & 0xFFFF])
            / NuTrigTable[ang & 0xFFFF];
        nearpnt.z = global_camera.nearclip;
        nearpnt.w = 1.0f;
        NuVec4MtxTransformVU0(&nearpnt, &nearpnt, pcs);
        NuVec4ScaleVU0(&nearpnt, &nearpnt, 1.0f / nearpnt.w);
        if (nearpnt.y < 34944.0f) {
            farpnt.x = 0.0f;
            farpnt.y = ((NuRndrWaterLevel - global_camera.mtx._31)
                        - (global_camera.nearclip + 0.1f) * NuTrigTable[(ang + 0x4000) & 0xFFFF])
                / NuTrigTable[ang & 0xFFFF];
            farpnt.z = global_camera.nearclip + 0.1f;
            farpnt.w = 1.0f;
            NuVec4MtxTransformVU0(&farpnt, &farpnt, pcs);
            NuVec4ScaleVU0(&farpnt, &farpnt, 1.0f / farpnt.w);
            if (farpnt.y < nearpnt.y) {
                farpnt.x = nearpnt.y;
                nearpnt.y = farpnt.y;
                farpnt.y = farpnt.x;
            }
            if (farpnt.y > 30720.0f) {
                screen = rndrstream_system;
                eop = 1;
                NuRndrStreamLink(screen);
                NuRndrSetRTarget(screen, -1);
                NuRndrSetZBuffer(screen, -2);
                rndrstream_free = vpDmaTag_Next(screen->cur.w, 0);
                NuRndrSetXYOffset((0x800 - (nurndr_pixel_width >> 1)) << 4,
                                  (0x800 - (nurndr_pixel_height >> 1)) << 4);
                NuRndrSetScissor(0, 0, nurndr_pixel_width - 1, nurndr_pixel_height - 1);
                rndrstream_3d = rndrstream_3d_u;
                rndrstream_3d->mtl = 0;
                rndrstream_3d->mtl_tid = 0;
                NuRndrStreamLink(rndrstream_3d);
                NuRndrStreamSetMisc(rndrstream_3d);
                NuRndrStreamAddMtl(rndrstream_3d, NuLightAddMat);
                rndrstream_3d->cur.w = vpDmaTag_CntEx(rndrstream_3d->cur.w, 0x10000000, 0x50000000) - 1;
                rndrstream_3d->cur.w = vtBeginDirect(rndrstream_3d->cur.w, 0);
                rndrstream_3d->cur.w = gsBeginGifPacket(rndrstream_3d->cur.w, 0, eop, eop, 4, 0, 8, eop, 5, eop, 5, eop, 5, eop, 5);
                if (farpnt.y > nearpnt.y) {
                    *rndrstream_3d->cur.w++ = 0;
                    *rndrstream_3d->cur.w++ = 0;
                    *rndrstream_3d->cur.w++ = 0;
                    *rndrstream_3d->cur.w++ = 0;
                    *rndrstream_3d->cur.w++ = 0x6BF8;
                    *rndrstream_3d->cur.w++ = nearpnt.y;
                    *rndrstream_3d->cur.w++ = 0xFFFFFF;
                    *rndrstream_3d->cur.w++ = 0;
                    *rndrstream_3d->cur.w++ = 0;
                    *rndrstream_3d->cur.w++ = 0;
                    *rndrstream_3d->cur.w++ = 0;
                    *rndrstream_3d->cur.w++ = 0;
                    *rndrstream_3d->cur.w++ = 0x9408;
                    *rndrstream_3d->cur.w++ = nearpnt.y;
                    *rndrstream_3d->cur.w++ = 0xFFFFFF;
                    *rndrstream_3d->cur.w++ = 0;
                    *rndrstream_3d->cur.w++ = 0;
                    *rndrstream_3d->cur.w++ = 0;
                    *rndrstream_3d->cur.w++ = 0;
                    *rndrstream_3d->cur.w++ = 0x80;
                    *rndrstream_3d->cur.w++ = 0x6BF8;
                    *rndrstream_3d->cur.w++ = farpnt.y;
                    *rndrstream_3d->cur.w++ = 0xFFFFFF;
                    *rndrstream_3d->cur.w++ = 0;
                    *rndrstream_3d->cur.w++ = 0;
                    *rndrstream_3d->cur.w++ = 0;
                    *rndrstream_3d->cur.w++ = 0;
                    *rndrstream_3d->cur.w++ = 0x80;
                    *rndrstream_3d->cur.w++ = 0x9408;
                    *rndrstream_3d->cur.w++ = farpnt.y;
                    *rndrstream_3d->cur.w++ = 0xFFFFFF;
                    *rndrstream_3d->cur.w++ = 0;
                }
                *rndrstream_3d->cur.w++ = 0;
                *rndrstream_3d->cur.w++ = 0;
                *rndrstream_3d->cur.w++ = 0;
                *rndrstream_3d->cur.w++ = 0x80;
                *rndrstream_3d->cur.w++ = 0x6BF8;
                *rndrstream_3d->cur.w++ = farpnt.y;
                *rndrstream_3d->cur.w++ = 0xFFFFFF;
                *rndrstream_3d->cur.w++ = 0;
                *rndrstream_3d->cur.w++ = 0;
                *rndrstream_3d->cur.w++ = 0;
                *rndrstream_3d->cur.w++ = 0;
                *rndrstream_3d->cur.w++ = 0x80;
                *rndrstream_3d->cur.w++ = 0x9408;
                *rndrstream_3d->cur.w++ = farpnt.y;
                *rndrstream_3d->cur.w++ = 0xFFFFFF;
                *rndrstream_3d->cur.w++ = 0;
                *rndrstream_3d->cur.w++ = 0;
                *rndrstream_3d->cur.w++ = 0;
                *rndrstream_3d->cur.w++ = 0;
                *rndrstream_3d->cur.w++ = 0x80;
                *rndrstream_3d->cur.w++ = 0x6BF8;
                *rndrstream_3d->cur.w++ = 0x8800;
                *rndrstream_3d->cur.w++ = 0xFFFFFF;
                *rndrstream_3d->cur.w++ = 0;
                *rndrstream_3d->cur.w++ = 0;
                *rndrstream_3d->cur.w++ = 0;
                *rndrstream_3d->cur.w++ = 0;
                *rndrstream_3d->cur.w++ = 0x80;
                *rndrstream_3d->cur.w++ = 0x9408;
                *rndrstream_3d->cur.w++ = 0x8800;
                *rndrstream_3d->cur.w++ = 0xFFFFFF;
                *rndrstream_3d->cur.w++ = 0;
                rndrstream_3d->cur.w = gsCloseGifPacket(rndrstream_3d->cur.w);
                rndrstream_3d->cur.w = vtCloseDirect(rndrstream_3d->cur.w);
                rndrstream_3d->cur.w = vpDmaTag_Close(rndrstream_3d->cur.w);
                rndrstream_free = vpDmaTag_Next(rndrstream_3d->cur.w, 0);
                rndrstream_3d = rndrstream_3d_u;
                rndrstream_3d->mtl = 0;
                rndrstream_3d->mtl_tid = 0;
                NuRndrStreamFlush();
            }
            stream = rndrstream_system;
            target = NuLgtHSurf[4];
            NuRndrStreamLink(stream);
            NuRndrSetRTarget(stream, target);
            NuRndrSetZBuffer(stream, -2);
            rndrstream_free = vpDmaTag_Next(stream->cur.w, 0);
            NuRndrSetXYOffset((0x800 - (nurndr_pixel_width >> 1)) << 4, (0x800 - (nurndr_pixel_height >> 1)) << 4);
            NuRndrSetScissor(0, 0, nurndr_pixel_width - 1, nurndr_pixel_height - 1);
            NuRndrStreamLink(rndrstream_3d);
            NuRndrStreamSetMisc(rndrstream_3d);
            NuRndrStreamAddMtl(rndrstream_3d, NuLgtBackBuff);
            for (i = 0; i < 10; i++) {
                rndrstream_3d->cur.w = vpDmaTag_CntEx(rndrstream_3d->cur.w, 0x10000000, 0x50000000) - 1;
                rndrstream_3d->cur.w = vtBeginDirect(rndrstream_3d->cur.w, 0);
                rndrstream_3d->cur.w = gsBeginGifPacket(rndrstream_3d->cur.w, 0, 1, 1, 4, 0, 9, 1, 2, 5, 2, 5, 2, 5, 2, 5);
                for (j = 0; j < 14; j++) {
                    u0 = (float)i * 64.0f / 1024.0f;
                    v0 = (float)(j << 4) / 256.0f;
                    v1 = (float)((j + 1) << 4) / 256.0f;
                    xoffset = (PS2_VCNTR_X - (nurndr_pixel_width >> 1)) << 4;
                    yoffset = (PS2_VCNTR_Y - (nurndr_pixel_height >> 1)) << 4;
                    dx0 = (int)(NuTrigTable[watertab[j][i].x & 0xFFFF] * waveampx) - 8;
                    x0 = (i << 10) + dx0;
                    dx1 = (int)(NuTrigTable[watertab[j][i + 1].x & 0xFFFF] * waveampx) - 8;
                    x1 = ((i + 1) << 10) + dx1;
                    dy0 = (int)(NuTrigTable[(watertab[j][i].y + 0x4000) & 0xFFFF] * waveampy) - 8;
                    y0 = (j << 8) + dy0;
                    dx2 = (int)(NuTrigTable[watertab[j + 1][i].x & 0xFFFF] * waveampx) - 8;
                    x2 = (i << 10) + dx2;
                    dy1 = (int)(NuTrigTable[(watertab[j][i + 1].y + 0x4000) & 0xFFFF] * waveampy) - 8;
                    y1 = (j << 8) + dy1;
                    dy2 = (int)(NuTrigTable[(watertab[j + 1][i].y + 0x4000) & 0xFFFF] * waveampy) - 8;
                    y2 = ((j + 1) << 8) + dy2;
                    dx3 = (int)(NuTrigTable[watertab[j + 1][i + 1].x & 0xFFFF] * waveampx) - 8;
                    x3 = ((i + 1) << 10) + dx3;
                    dy3 = (int)(NuTrigTable[(watertab[j + 1][i + 1].y + 0x4000) & 0xFFFF] * waveampy) - 8;
                    y3 = ((j + 1) << 8) + dy3;
                    x0 += xoffset;
                    y0 += yoffset;
                    x1 += xoffset;
                    y1 += yoffset;
                    x2 += xoffset;
                    y2 += yoffset;
                    x3 += xoffset;
                    y3 += yoffset;
                    u1 = (float)(i + 1) * 64.0f / 1024.0f;
                    *rndrstream_3d->cur.w++ = 0x80;
                    *rndrstream_3d->cur.w++ = 0x80;
                    *rndrstream_3d->cur.w++ = 0x80;
                    *rndrstream_3d->cur.w++ = 0x80;
                    *rndrstream_3d->cur.f++ = u0;
                    *rndrstream_3d->cur.f++ = v0;
                    *rndrstream_3d->cur.f++ = 1.0f;
                    rndrstream_3d->cur.w++;
                    *rndrstream_3d->cur.w++ = x0;
                    *rndrstream_3d->cur.w++ = y0;
                    *rndrstream_3d->cur.w++ = 0xFFFFFF;
                    *rndrstream_3d->cur.w++ = 0;
                    *rndrstream_3d->cur.f++ = u1;
                    *rndrstream_3d->cur.f++ = v0;
                    *rndrstream_3d->cur.f++ = 1.0f;
                    rndrstream_3d->cur.w++;
                    *rndrstream_3d->cur.w++ = x1;
                    *rndrstream_3d->cur.w++ = y1;
                    *rndrstream_3d->cur.w++ = 0xFFFFFF;
                    *rndrstream_3d->cur.w++ = 0;
                    *rndrstream_3d->cur.f++ = u0;
                    *rndrstream_3d->cur.f++ = v1;
                    *rndrstream_3d->cur.f++ = 1.0f;
                    rndrstream_3d->cur.w++;
                    *rndrstream_3d->cur.w++ = x2;
                    *rndrstream_3d->cur.w++ = y2;
                    *rndrstream_3d->cur.w++ = 0xFFFFFF;
                    *rndrstream_3d->cur.w++ = 0;
                    *rndrstream_3d->cur.f++ = u1;
                    *rndrstream_3d->cur.f++ = v1;
                    *rndrstream_3d->cur.f++ = 1.0f;
                    rndrstream_3d->cur.w++;
                    *rndrstream_3d->cur.w++ = x3;
                    *rndrstream_3d->cur.w++ = y3;
                    *rndrstream_3d->cur.w++ = 0xFFFFFF;
                    *rndrstream_3d->cur.w++ = 0;
                }
                rndrstream_3d->cur.w = gsCloseGifPacket(rndrstream_3d->cur.w);
                rndrstream_3d->cur.w = vtCloseDirect(rndrstream_3d->cur.w);
                rndrstream_3d->cur.w = vpDmaTag_Close(rndrstream_3d->cur.w);
            }
            rndrstream_free = vpDmaTag_Next(rndrstream_3d->cur.w, 0);
            NuRndrStreamFlush();
            for (i = 0; i < 21; i++) {
                for (j = 0; j < 15; j++) {
                    watertab[j][i].x += watertab[j][i].u;
                    watertab[j][i].y += watertab[j][i].v;
                }
            }
            display = rndrstream_system;
            NuRndrStreamLink(display);
            NuRndrSetRTarget(display, -1);
            NuRndrSetZBuffer(display, -2);
            rndrstream_free = vpDmaTag_Next(display->cur.w, 0);
            NuRndrSetXYOffset((0x800 - (nurndr_pixel_width >> 1)) << 4, (0x800 - (nurndr_pixel_height >> 1)) << 4);
            NuRndrSetScissor(0, 0, nurndr_pixel_width - 1, nurndr_pixel_height - 1);
            NuRndrStreamLink(rndrstream_3d);
            NuRndrStreamSetMisc(rndrstream_3d);
            if (farpnt.y > 30720.0f) {
                NuRndrStreamAddMtl(rndrstream_3d, NuLgtSurf[12]);
            } else {
                NuRndrStreamAddMtl(rndrstream_3d, NuLgtSurf[15]);
            }
            NuRndrRectUV3di(-8, -8, 0x2800, nurndr_pixel_height << 4, 0.00390625f, 0.015625f, 0.62109375f,
                            0.859375f, NuRndrWaterTint, 0x1FFFFFF);
            rndrstream_free = vpDmaTag_Next(rndrstream_3d->cur.w, 0);
            NuRndrStreamFlush();
        }
        NuRndrWaterLevel = -10000.0f;
    }
}


void NuRndrShadow(int mode) {
    struct nuvec_s light;
    struct nurndrstream_s *stream;
    struct nurndrstream_s *screen;
    int target;
    int zbuffer;
    int x[2];
    int y0;
    int y1;
    int left;
    int top;
    int width;
    int height;
    float u0;
    float v0;
    float u1;
    float v1;
    int i;

    light = curlight;
    if ((testflipold & 4) || shadstorecnt == 0) {
        return;
    }
    stream = rndrstream_system;
    target = NuLgtHSurf[2];
    zbuffer = NuLgtHSurf[1];
    NuRndrStreamLink(stream);
    NuRndrSetRTarget(stream, target);
    NuRndrSetZBuffer(stream, zbuffer);
    rndrstream_free = vpDmaTag_Next(stream->cur.w, 0);
    NuRndrSetXYOffset((0x800 - (nurndr_pixel_width >> 1)) << 4, (0x800 - (nurndr_pixel_height >> 1)) << 4);
    NuRndrSetScissor(0, 0, nurndr_pixel_width - 1, nurndr_pixel_height - 1);
    txupload_on = 0;
    NuRndrStreamLink(rndrstream_fx);
    NuRndrStreamSetMisc(rndrstream_fx);
    NuRndrStreamAddMtl(rndrstream_fx, NuLgtGreyNZMat);
    for (i = 0; i < 4; i++) {
        NuRndrRectUVStream(rndrstream_fx, i << 10, 0, 0x3F8, (PS2_REZ_H << 4) - 8,
                           0.0f, 0.0f, 0.0f, 0.0f, 0x207F7F7F, 1);
    }
    rndrstream_free = vpDmaTag_Next(rndrstream_fx->cur.w, 0);
    light.x *= testmultl;
    light.y *= testmultl;
    light.z *= testmultl;
    NuRndrShadowStack(&light);

    NuRndrStreamLink(rndrstream_fx);
    NuRndrStreamSetMisc(rndrstream_fx);
    NuRndrStreamAddMtl(rndrstream_fx, NuLightAddMat);
    rndrstream_fx->cur.w = vpDmaTag_CntEx(rndrstream_fx->cur.w, 0x11000000, 0x50000000) - 1;
    rndrstream_fx->cur.w = vtBeginDirect(rndrstream_fx->cur.w, 0);
    rndrstream_fx->cur.w = gsBeginGifPacket(rndrstream_fx->cur.w, 0, 1, 0, 0, 0, 1, 0xE);
    *((u64 *)rndrstream_fx->cur.w)++ = cur_rt.fbp | cur_rt.fbw << 16 | cur_rt.psm << 24
                                     | (u64)0x808080 << 32;
    *((u64 *)rndrstream_fx->cur.w)++ = 0x4C;
    *((u64 *)rndrstream_fx->cur.w)++ = 0;
    *((u64 *)rndrstream_fx->cur.w)++ = 0x1B;
    *((u64 *)rndrstream_fx->cur.w)++ = 1;
    *((u64 *)rndrstream_fx->cur.w)++ = 0x46;
    rndrstream_fx->cur.w = gsCloseGifPacket(rndrstream_fx->cur.w);
    rndrstream_fx->cur.w = vtCloseDirect(rndrstream_fx->cur.w);
    rndrstream_fx->cur.w = vpDmaTag_Close(rndrstream_fx->cur.w);
    for (i = 0; i < 4; i++) {
        NuRndrRectUVStream(rndrstream_fx, (i << 10) - 8, -8, 0x400, PS2_REZ_H << 4,
                           0.0f, 0.0f, 0.0f, 0.0f, 0, 0xFFFFFF);
    }
    rndrstream_free = vpDmaTag_Next(rndrstream_fx->cur.w, 0);
    NuRndrStreamFlush();

    screen = rndrstream_system;
    NuRndrStreamLink(screen);
    NuRndrSetRTarget(screen, -1);
    NuRndrSetZBuffer(screen, -2);
    rndrstream_free = vpDmaTag_Next(screen->cur.w, 0);
    NuRndrSetXYOffset((0x800 - (nurndr_pixel_width >> 1)) << 4, (0x800 - (nurndr_pixel_height >> 1)) << 4);
    NuRndrSetScissor(0, 0, nurndr_pixel_width - 1, nurndr_pixel_height - 1);
    NuRndrStreamLink(rndrstream_3d);
    NuRndrStreamSetMisc(rndrstream_3d);
    NuRndrStreamAddMtl(rndrstream_3d, NuLgtSurf[2]);
    for (i = (int)NuRndrShadBox[0].x >> 10; i < ((int)NuRndrShadBox[1].x + 0x3FF) >> 10; i++) {
        x[0] = i << 10;
        x[1] = (i + 1) << 10;
        if ((float)x[0] < NuRndrShadBox[0].x) {
            x[0] = NuRndrShadBox[0].x;
        }
        if (NuRndrShadBox[1].x < (float)x[1]) {
            x[1] = NuRndrShadBox[1].x;
        }
        y0 = NuRndrShadBox[0].y;
        y1 = NuRndrShadBox[1].y;
        left = x[0] - 8;
        top = y0 - 8;
        width = x[1] - x[0];
        height = y1 - y0;
        u0 = (float)x[0] * 0.0009765625f * 25.6f / NuPower2(PS2_REZ_W);
        v0 = NuRndrShadBox[0].y / (float)(NuPower2(PS2_REZ_H) << 4);
        u1 = (float)x[1] * 0.0009765625f * 25.6f / NuPower2(PS2_REZ_W);
        v1 = NuRndrShadBox[1].y / (float)(NuPower2(PS2_REZ_H) << 4);
        NuRndrRectUVStream(rndrstream_3d, left, top, width, height, u0, v0, u1, v1, curlightcol, 0);
    }
    rndrstream_free = vpDmaTag_Next(rndrstream_3d->cur.w, 0);
    NuRndrStreamFlush();
}


inline void RotateCyl(struct nuvec_s *v, float xang, float yang) {
    float sinx;
    float cosx;
    float siny;
    float cosy;
    float x;
    float z;

    sinx = NuTrigTable[(int)xang & 0xFFFF];
    cosx = NuTrigTable[(int)(xang + 16384.0f) & 0xFFFF];
    siny = NuTrigTable[(int)yang & 0xFFFF];
    cosy = NuTrigTable[(int)(yang + 16384.0f) & 0xFFFF];
    x = v->x;
    z = v->y * sinx + v->z * cosx;
    v->y = v->y * cosx - v->z * sinx;
    v->z = z * cosy - x * siny;
    v->x = z * siny + x * cosy;
}


inline void DeRotateVectorAngles(struct nuvec_s *dir, float *xang, float *yang) {
    *yang = NuAtan2D(dir->x, dir->z);
    *xang = NuAtan2D(-dir->y, NuFsqrt(dir->x * dir->x + dir->z * dir->z));
}


u32 *NuRndrShadowLightFan(void) {
    GifTagQuad strip;
    GifTagQuad fan;
    struct nuvec_s angles;
    u32 *packet;
    u32 *vertices;
    int i;

    DeRotateVectorAngles(&curlight, &angles.x, &angles.y);
    strip.dword[0] = 0x3002400000008000ULL;
    strip.dword[1] = 0x512;
    fan.dword[0] = 0x3002C00000008000ULL;
    fan.dword[1] = 0x512;
    if (nurndr_lightfan == 0) {
        packet = vpDmaTag_RetEx(lightfan_packet, 0, 0x6C010056);
        *packet++ = fan.word[0];
        *packet++ = fan.word[1];
        *packet++ = fan.word[2];
        *packet++ = fan.word[3];
        strip.tag.nloop = 18;
        *packet++ = 0x6C0180D2;
        *packet++ = strip.word[0];
        *packet++ = strip.word[1];
        *packet++ = strip.word[2];
        *packet++ = strip.word[3];
        *packet++ = 0x6D018002;
        *((u16 *)packet)++ = 0x36;
        *((u16 *)packet)++ = 0;
        *((u16 *)packet)++ = 0;
        *((u16 *)packet)++ = 0;
        *packet++ = 0x6C0A80AF;
        *packet++ = 9;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0;
    }
    if (nurndr_lightfan == 0) {
        vertices = packet;
    } else {
        packet = nurndr_lightfan;
    }

    for (i = 0; i < 8; i++) {
        lightfan[i].x = NuTrigTable[(i * 0x2000 + 0x4000) & 0xFFFF];
        lightfan[i].y = -NuTrigTable[(i * 0x2000) & 0xFFFF];
        lightfan[i].z = 0.0f;
        RotateCyl(&lightfan[i], angles.x, angles.y);
        *((float *)packet)++ = lightfan[i].x;
        *((float *)packet)++ = lightfan[i].y;
        *((float *)packet)++ = lightfan[i].z;
        *((float *)packet)++ = 0.0f;
    }
    *((float *)packet)++ = lightfan[0].x;
    *((float *)packet)++ = lightfan[0].y;
    *((float *)packet)++ = lightfan[0].z;
    *((float *)packet)++ = 0.0f;

    if (nurndr_lightfan == 0) {
        vpDmaTag_Close(packet);
        nurndr_lightfan = vertices;
    }
    return lightfan_packet;
}


void NuRndrShadowStencil(int paused, struct nuvec_s *pos) {
    struct nuvec_s dir;
    struct nuvec4_s v;
    struct nuvec_s *pts;
    struct nuvec_s *maskpts;
    struct numtx_s *vpcs;
    float dx;
    float dz;
    float dist;
    float mag;
    float wobble;
    int best;
    int i;

    NuRndrShadBox[1].x = nurndr_pixel_width << 4;
    NuRndrShadBox[0].x = 0.0f;
    NuRndrShadBox[0].y = 0.0f;
    NuRndrShadBox[1].y = nurndr_pixel_height << 4;
    if (NuRndrShadMaskCount == 0 || shadstorecnt == 0) {
        return;
    }
    best = -1;
    mag = 1000000.0f;
    for (i = 0; i < NuRndrShadMaskCount; i++) {
        maskpts = (struct nuvec_s *)NuRndrShadMasks[i]->pts;
        dx = maskpts->x - pos->x;
        dz = maskpts->z - pos->z;
        dist = dx * dx + dz * dz;
        if (dist < mag) {
            mag = dist;
            pts = maskpts;
            best = i;
        }
    }
    if (best < 0) {
        return;
    }
    if (mag > 400.0f) {
        shadstorecnt = 0;
        return;
    }
    if (paused == 0) {
        shadow_wobble_ang += 0xE74;
        shadow_spin_ang += NuTrigTable[shadow_wobble_ang & 0xFFFF] * 1111.0f + 2111.0f;
    }
    dir.x = pts[2].x;
    dir.z = pts[2].z;
    for (i = 3; i < NuRndrShadMasks[best]->len; i++) {
        dir.x += pts[i].x;
        dir.z += pts[i].z;
    }
    dir.x = pts[0].x - dir.x / (float)(NuRndrShadMasks[best]->len - 2);
    dir.z = pts[0].z - dir.z / (float)(NuRndrShadMasks[best]->len - 2);
    mag = 2.0f / NuFsqrt(dir.x * dir.x + dir.z * dir.z);
    dir.x *= mag;
    dir.z *= mag;
    wobble = NuTrigTable[(shadow_wobble_ang + 0x4000) & 0xFFFF] * 0.5f + NuTrigTable[shadow_spin_ang & 0xFFFF];
    dir.x = pos->x - pts[0].x - dir.x - wobble * (pts[0].x - pts[1].x) * 0.15f;
    dir.z = pos->z - pts[0].z - dir.z - wobble * (pts[0].z - pts[1].z) * 0.15f;
    mag = 2.0f / NuFsqrt(dir.x * dir.x + dir.z * dir.z);
    dir.y = -0.2f;
    dir.x *= mag;
    dir.z *= mag;
    vpcs = NuCameraGetVPCSMtx();
    NuRndrShadBox[1].y = 0.0f;
    NuRndrShadBox[1].x = 0.0f;
    NuRndrShadBox[0].x = nurndr_pixel_width << 4;
    NuRndrShadBox[0].y = nurndr_pixel_height << 4;
    for (i = 2; i < NuRndrShadMasks[best]->len; i++) {
        v.x = pts[i].x;
        v.y = pts[i].y;
        v.z = pts[i].z;
        v.w = 1.0f;
        NuVec4MtxTransformVU0(&v, &v, vpcs);
        if (v.w > 0.25f) {
            NuVec4ScaleXYZVU0(&v, &v, 1.0f / v.w);
            v.x -= 0x8000 - ((nurndr_pixel_width >> 1) << 4);
            v.y -= 0x8000 - ((nurndr_pixel_height >> 1) << 4);
            if (v.x < NuRndrShadBox[0].x) {
                NuRndrShadBox[0].x = v.x;
            }
            if (v.x > NuRndrShadBox[1].x) {
                NuRndrShadBox[1].x = v.x;
            }
            if (v.y < NuRndrShadBox[0].y) {
                NuRndrShadBox[0].y = v.y;
            }
            if (v.y > NuRndrShadBox[1].y) {
                NuRndrShadBox[1].y = v.y;
            }
        }
    }
    if (NuRndrShadBox[1].x == 0.0f || NuRndrShadBox[0].x == nurndr_pixel_width * 16.0f
        || NuRndrShadBox[1].y == 0.0f || NuRndrShadBox[0].y == nurndr_pixel_height * 16.0f) {
        shadstorecnt = 0;
        return;
    }
    curlight = dir;
    curlightcol = 0x80202020;
    NRFanData = NuRndrShadowLightFan();
}


struct nurndrtrail_s {
    struct nuvec_s pos1;
    struct nuvec_s pos2;
    int intensity;
    int RealIntensity;
};

void NuRndrTrail(int trailpt, struct nurndrtrail_s *trail, int trailcount) {
    struct nuvtx_tc1_s vtx[2048];
    int loop;
    int cnt;
    int start;
    int lastcol;
    int col;
    int last;
    int begin;
    int pt;

    cnt = 0;
    last = -1;
    start = trailpt + (1 - trailcount);
    for (loop = trailpt; loop > trailpt + (1 - trailcount); loop--) {
        pt = loop & (trailcount - 1);
        if (trail[pt].pos1.x != -10000.0f) {
            if (last != -1) {
                int next = (loop - 1) & (trailcount - 1);
                int next2 = (loop - 2) & (trailcount - 1);
                int intensity = trail[pt].intensity;
                int alpha = intensity >> 24;

                lastcol = col;
                if (trail[next].pos1.x == -10000.0f || loop - 1 <= start) {
                    col = 0;
                } else if (trail[next2].pos1.x == -10000.0f || begin) {
                    col = (alpha / 2) << 24;
                } else {
                    col = intensity;
                }
                begin = 0;
                vtx[cnt].pnt = trail[last].pos1;
                vtx[cnt + 1].pnt = trail[pt].pos1;
                vtx[cnt + 2].pnt.x = (trail[last].pos2.x - trail[last].pos1.x) * 0.25f + trail[last].pos1.x;
                vtx[cnt + 2].pnt.y = (trail[last].pos2.y - trail[last].pos1.y) * 0.25f + trail[last].pos1.y;
                vtx[cnt + 2].pnt.z = (trail[last].pos2.z - trail[last].pos1.z) * 0.25f + trail[last].pos1.z;
                vtx[cnt + 3].pnt.x = (trail[pt].pos2.x - trail[pt].pos1.x) * 0.25f + trail[pt].pos1.x;
                vtx[cnt + 3].pnt.y = (trail[pt].pos2.y - trail[pt].pos1.y) * 0.25f + trail[pt].pos1.y;
                vtx[cnt + 3].pnt.z = (trail[pt].pos2.z - trail[pt].pos1.z) * 0.25f + trail[pt].pos1.z;
                if (lastcol < 0) {
                    vtx[cnt].diffuse = -lastcol;
                    vtx[cnt + 2].diffuse = 0;
                } else {
                    vtx[cnt].diffuse = lastcol + 0xFFFFFF;
                    vtx[cnt + 2].diffuse = 0xFFFFFF;
                }
                if (col < 0) {
                    vtx[cnt + 1].diffuse = -col;
                    vtx[cnt + 3].diffuse = 0;
                } else {
                    vtx[cnt + 1].diffuse = col + 0xFFFFFF;
                    vtx[cnt + 3].diffuse = 0xFFFFFF;
                }
                cnt += 4;
                vtx[cnt].pnt = trail[last].pos2;
                vtx[cnt + 1].pnt = trail[pt].pos2;
                vtx[cnt + 2].pnt.x = (trail[last].pos1.x - trail[last].pos2.x) * 0.25f + trail[last].pos2.x;
                vtx[cnt + 2].pnt.y = (trail[last].pos1.y - trail[last].pos2.y) * 0.25f + trail[last].pos2.y;
                vtx[cnt + 2].pnt.z = (trail[last].pos1.z - trail[last].pos2.z) * 0.25f + trail[last].pos2.z;
                vtx[cnt + 3].pnt.x = (trail[pt].pos1.x - trail[pt].pos2.x) * 0.25f + trail[pt].pos2.x;
                vtx[cnt + 3].pnt.y = (trail[pt].pos1.y - trail[pt].pos2.y) * 0.25f + trail[pt].pos2.y;
                vtx[cnt + 3].pnt.z = (trail[pt].pos1.z - trail[pt].pos2.z) * 0.25f + trail[pt].pos2.z;
                if (lastcol < 0) {
                    vtx[cnt].diffuse = 0xFFFFF - lastcol;
                    vtx[cnt + 2].diffuse = 0xFFFFFF;
                } else {
                    vtx[cnt].diffuse = lastcol;
                    vtx[cnt + 2].diffuse = 0;
                }
                if (col < 0) {
                    vtx[cnt + 1].diffuse = 0xFFFFFF - col;
                    vtx[cnt + 3].diffuse = 0xFFFFFF;
                } else {
                    vtx[cnt + 1].diffuse = col;
                    vtx[cnt + 3].diffuse = 0;
                }
                cnt += 4;
                last = pt;
            } else {
                last = pt;
                col = 0;
                begin = 1;
            }
        } else {
            last = -1;
        }
    }

    if (cnt != 0) {
        NuRndrStreamLink(rndrstream_fx);
        NuRndrStreamSetMisc(rndrstream_fx);
        NuRndrStreamAddMtl(rndrstream_fx, NuLightAddMat);
        rndrstream_fx->cur.w = vpDmaTag_CntEx(rndrstream_fx->cur.w, 0x11000000, 0x50000000) - 1;
        rndrstream_fx->cur.w = vtBeginDirect(rndrstream_fx->cur.w, 0);
        rndrstream_fx->cur.w = gsBeginGifPacket(rndrstream_fx->cur.w, 0, 1, 0, 0, 0, 1, 0xE);
        *((u64 *)rndrstream_fx->cur.w)++ = 0x44;
        *((u64 *)rndrstream_fx->cur.w)++ = 0x42;
        rndrstream_fx->cur.w = gsCloseGifPacket(rndrstream_fx->cur.w);
        rndrstream_fx->cur.w = vtCloseDirect(rndrstream_fx->cur.w);
        rndrstream_fx->cur.w = vpDmaTag_Close(rndrstream_fx->cur.w);
        NuRndrSetLocalMtx(&numtx_identity, 2);
        NuRndrStreamSetViewMtxScissorClip(rndrstream_fx);
        NuRndrStreamSetLocalMtx(rndrstream_fx);
        NuRndrStreamFxMPG(1);
        NuRndrStreamQuad3dClip(vtx, cnt, 0, rndrstream_fx);
        rndrstream_free = vpDmaTag_Next(rndrstream_fx->cur.w, 0);
    }
}


inline void NuRndrAnglesZX(struct nuvec_s *dir, struct nuvec_s *angles) {
    struct nuvec_s rotated;

    angles->x = NuAtan2D(dir->z, dir->y);
    NuVecRotateX(&rotated, dir, (int)-angles->x);
    angles->z = -NuAtan2D(rotated.x, rotated.y);
}


void NuRndrAddFootPrint(int rot, float sizex, float sizez, int brightness, struct nuvec_s *pos,
                        struct nuvec_s *norm, int gfx, u32 colour) {
    struct nuvec4_s tp[4];
    struct nuvec_s terrot;
    struct numtx_s m;
    int i;
    int ix;

    tp[0].y = tp[1].y = tp[2].y = tp[3].y = 0.0f;
    tp[0].w = tp[1].w = tp[2].w = tp[3].w = 0.0f;
    tp[0].x = NuTrigTable[(rot + 0x4000) & 0xFFFF] * sizex + NuTrigTable[rot & 0xFFFF] * sizez;
    tp[0].z = -NuTrigTable[rot & 0xFFFF] * sizex + NuTrigTable[(rot + 0x4000) & 0xFFFF] * sizez;
    tp[1].x = NuTrigTable[(rot + 0x4000) & 0xFFFF] * -sizex + NuTrigTable[rot & 0xFFFF] * sizez;
    tp[1].z = -NuTrigTable[rot & 0xFFFF] * -sizex + NuTrigTable[(rot + 0x4000) & 0xFFFF] * sizez;
    tp[2].x = NuTrigTable[(rot + 0x4000) & 0xFFFF] * sizex + NuTrigTable[rot & 0xFFFF] * -sizez;
    tp[2].z = -NuTrigTable[rot & 0xFFFF] * sizex + NuTrigTable[(rot + 0x4000) & 0xFFFF] * -sizez;
    tp[3].x = NuTrigTable[(rot + 0x4000) & 0xFFFF] * -sizex + NuTrigTable[rot & 0xFFFF] * -sizez;
    tp[3].z = -NuTrigTable[rot & 0xFFFF] * -sizex + NuTrigTable[(rot + 0x4000) & 0xFFFF] * -sizez;

    NuRndrAnglesZX(norm, &terrot);
    NuMtxSetIdentity(&m);
    NuMtxRotateZ(&m, (int)terrot.z);
    NuMtxRotateX(&m, (int)terrot.x);
    NuVec4MtxTransformVU0(&tp[0], &tp[0], &m);
    NuVec4MtxTransformVU0(&tp[1], &tp[1], &m);
    NuVec4MtxTransformVU0(&tp[2], &tp[2], &m);
    NuVec4MtxTransformVU0(&tp[3], &tp[3], &m);

    ix = NuRndrFootFree & 0x3F;
    NuRndrFootData[ix].pnts[0].x = tp[0].x + pos->x;
    NuRndrFootData[ix].pnts[0].y = tp[0].y + pos->y;
    NuRndrFootData[ix].pnts[0].z = tp[0].z + pos->z;
    NuRndrFootData[ix].pnts[1].x = tp[1].x + pos->x;
    NuRndrFootData[ix].pnts[1].y = tp[1].y + pos->y;
    NuRndrFootData[ix].pnts[1].z = tp[1].z + pos->z;
    NuRndrFootData[ix].pnts[2].x = tp[2].x + pos->x;
    NuRndrFootData[ix].pnts[2].y = tp[2].y + pos->y;
    NuRndrFootData[ix].pnts[2].z = tp[2].z + pos->z;
    NuRndrFootData[ix].pnts[3].x = tp[3].x + pos->x;
    NuRndrFootData[ix].pnts[3].y = tp[3].y + pos->y;
    NuRndrFootData[ix].pnts[3].z = tp[3].z + pos->z;
    NuRndrFootData[ix].gfx = gfx << 2;
    NuRndrFootData[ix].timer = 16;
    NuRndrFootData[ix].brightness = brightness;
    NuRndrFootData[ix].colour = colour;
    NuRndrFootFree++;
    for (i = NuRndrFootFree; i < NuRndrFootFree + 8; i++) {
        ix = i & 0x3F;
        if (NuRndrFootData[ix].timer > 15) {
            NuRndrFootData[ix].timer = 15;
        }
    }
}


void NuRndrFootPrints(struct numtl_s *mtl, float *u, float *v) {
    struct nuvtx_tc1_s vtx[2048];
    struct nuvtx_tc1_s *vp;
    nurndrfoot_s *foot;
    int count;
    int gfx;
    int colour;
    int i;

    vp = vtx;
    count = 0;
    for (i = 0; i < 64; i++) {
        foot = &NuRndrFootData[i];
        if (foot->timer > 0) {
            if (testfootcol != 0) {
                foot->colour = testfootcol;
            }
            if (foot->timer < 16) {
                foot->timer--;
            }
            count += 4;
            gfx = foot->gfx;
            vp[0].pnt = foot->pnts[0];
            vp[0].tc[0] = u[gfx];
            vp[0].tc[1] = v[gfx];
            vp[1].pnt = foot->pnts[1];
            vp[1].tc[0] = u[gfx + 1];
            vp[1].tc[1] = v[gfx + 1];
            vp[2].pnt = foot->pnts[2];
            vp[2].tc[0] = u[gfx + 2];
            vp[2].tc[1] = v[gfx + 2];
            vp[3].pnt = foot->pnts[3];
            vp[3].tc[0] = u[gfx + 3];
            vp[3].tc[1] = v[gfx + 3];
            colour = (((foot->brightness * foot->timer) >> 4) << 24) + foot->colour;
            vp[3].diffuse = colour;
            vp[2].diffuse = colour;
            vp[1].diffuse = colour;
            vp[0].diffuse = colour;
            vp += 4;
        }
    }

    if (count != 0) {
        NuRndrStreamLink(rndrstream_fx);
        NuRndrStreamSetMisc(rndrstream_fx);
        NuRndrStreamAddMtl(rndrstream_fx, mtl);
        rndrstream_fx->cur.w = vpDmaTag_CntEx(rndrstream_fx->cur.w, 0x11000000, 0x50000000) - 1;
        rndrstream_fx->cur.w = vtBeginDirect(rndrstream_fx->cur.w, 0);
        rndrstream_fx->cur.w = gsBeginGifPacket(rndrstream_fx->cur.w, 0, 1, 0, 0, 0, 1, 0xE);
        *((u64 *)rndrstream_fx->cur.w)++ = 0x44;
        *((u64 *)rndrstream_fx->cur.w)++ = 0x42;
        rndrstream_fx->cur.w = gsCloseGifPacket(rndrstream_fx->cur.w);
        rndrstream_fx->cur.w = vtCloseDirect(rndrstream_fx->cur.w);
        rndrstream_fx->cur.w = vpDmaTag_Close(rndrstream_fx->cur.w);
        NuRndrSetLocalMtx(&numtx_identity, 2);
        NuRndrStreamSetViewMtxScissorClip(rndrstream_fx);
        NuRndrStreamSetLocalMtx(rndrstream_fx);
        NuRndrStreamFxMPG(1);
        NuRndrStreamQuad3dClip(vtx, count, 0, rndrstream_fx);
        rndrstream_free = vpDmaTag_Next(rndrstream_fx->cur.w, 0);
    }
}


int NuRndrStreamQuadShad(struct nurndrstream_s *stream) {
    GifTagQuad strip;
    GifTagQuad fan;
    struct numtx_s m;
    struct nuvec_s corner[4];
    struct nuvec_s offset;
    int n;
    int start;
    int i;
    int count;

    count = NuRndrShadowCnt * 4;
    strip.dword[0] = 0x3002400000008000ULL;
    strip.dword[1] = 0x512;
    fan.dword[0] = 0x3002C00000008000ULL;
    fan.dword[1] = 0x512;
    start = 0;
    stream->cur.w = vpDmaTag_Cnt(stream->cur.w);
    ((unsigned char *)stream->cur.w)[-14] = 0x12;
    *stream->cur.w++ = 0x6C010056;
    *stream->cur.w++ = fan.word[0];
    *stream->cur.w++ = fan.word[1];
    *stream->cur.w++ = fan.word[2];
    *stream->cur.w++ = fan.word[3];
    while (count) {
        n = (count > 42) ? 42 : count;
        n &= 0xFC;
        strip.tag.nloop = n;
        *stream->cur.w++ = 0x6C0180D2;
        *stream->cur.w++ = strip.word[0];
        *stream->cur.w++ = strip.word[1];
        *stream->cur.w++ = strip.word[2];
        *stream->cur.w++ = strip.word[3];
        *stream->cur.w++ = 0x6D018002;
        *((u16 *)stream->cur.w)++ = n * 3;
        *((u16 *)stream->cur.w)++ = 0;
        *((u16 *)stream->cur.w)++ = 0;
        *((u16 *)stream->cur.w)++ = 0;
        *stream->cur.w++ = 0x01000103;
        *stream->cur.w++ = (n << 16) | 0x6E00C005;
        for (i = 0; i < n; i += 4) {
            *stream->cur.w++ = NuRndrShadPolDat[(start + i) >> 2].shade << 24;
            *stream->cur.w++ = NuRndrShadPolDat[(start + i) >> 2].shade << 24;
            *stream->cur.w++ = NuRndrShadPolDat[(start + i) >> 2].shade << 24;
            *stream->cur.w++ = NuRndrShadPolDat[(start + i) >> 2].shade << 24;
        }
        *stream->cur.w++ = 0x01000203;
        *stream->cur.w++ = (n << 17) | 0x6C008003;
        for (i = 0; i < n; i += 4) {
            NuMtxSetRotationY(&m, NuRndrShadPolDat[(start + i) >> 2].yrot);
            NuMtxRotateZ(&m, NuRndrShadPolDat[(start + i) >> 2].zrot);
            NuMtxRotateX(&m, NuRndrShadPolDat[(start + i) >> 2].xrot);
            offset.y = 0.0f;
            offset.x = -NuRndrShadPolDat[(start + i) >> 2].size;
            offset.z = -NuRndrShadPolDat[(start + i) >> 2].size;
            NuVecMtxRotate(&corner[0], &offset, &m);
            offset.z = -NuRndrShadPolDat[(start + i) >> 2].size;
            offset.x = NuRndrShadPolDat[(start + i) >> 2].size;
            NuVecMtxRotate(&corner[1], &offset, &m);
            offset.x = -NuRndrShadPolDat[(start + i) >> 2].size;
            offset.z = NuRndrShadPolDat[(start + i) >> 2].size;
            NuVecMtxRotate(&corner[2], &offset, &m);
            offset.x = NuRndrShadPolDat[(start + i) >> 2].size;
            offset.z = NuRndrShadPolDat[(start + i) >> 2].size;
            NuVecMtxRotate(&corner[3], &offset, &m);

            *stream->cur.f++ = corner[0].x + NuRndrShadPolDat[(start + i) >> 2].pos.x;
            *stream->cur.f++ = corner[0].y + NuRndrShadPolDat[(start + i) >> 2].pos.y;
            *stream->cur.f++ = corner[0].z + NuRndrShadPolDat[(start + i) >> 2].pos.z;
            *stream->cur.w++ = 0x8000;
            *stream->cur.w++ = 0;
            *stream->cur.w++ = 0;
            stream->cur.w += 2;
            *stream->cur.f++ = corner[1].x + NuRndrShadPolDat[(start + i) >> 2].pos.x;
            *stream->cur.f++ = corner[1].y + NuRndrShadPolDat[(start + i) >> 2].pos.y;
            *stream->cur.f++ = corner[1].z + NuRndrShadPolDat[(start + i) >> 2].pos.z;
            *stream->cur.w++ = 0x8000;
            *stream->cur.w++ = 0x1000;
            *stream->cur.w++ = 0;
            stream->cur.w += 2;
            *stream->cur.f++ = corner[2].x + NuRndrShadPolDat[(start + i) >> 2].pos.x;
            *stream->cur.f++ = corner[2].y + NuRndrShadPolDat[(start + i) >> 2].pos.y;
            *stream->cur.f++ = corner[2].z + NuRndrShadPolDat[(start + i) >> 2].pos.z;
            *stream->cur.w++ = 0;
            *stream->cur.w++ = 0;
            *stream->cur.w++ = 0x1000;
            stream->cur.w += 2;
            *stream->cur.f++ = corner[3].x + NuRndrShadPolDat[(start + i) >> 2].pos.x;
            *stream->cur.f++ = corner[3].y + NuRndrShadPolDat[(start + i) >> 2].pos.y;
            *stream->cur.f++ = corner[3].z + NuRndrShadPolDat[(start + i) >> 2].pos.z;
            *stream->cur.w++ = 0;
            *stream->cur.w++ = 0x1000;
            *stream->cur.w++ = 0x1000;
            stream->cur.w += 2;
        }
        *stream->cur.w++ = 0x01000101;
        *stream->cur.w++ = 0x14000306;
        start += n;
        count -= n;
    }
    stream->cur.w = vpDmaTag_Close(stream->cur.w);
    return 1;
}


void NuRndrWaterRippleUpdate(int frames) {
    float t;
    float fade;
    int i;

    for (i = 0; i < 32; i++) {
        if (NuRndrWaterRipDat[i].timer != 0) {
            NuRndrWaterRipDat[i].timer -= frames;
            if (NuRndrWaterRipDat[i].timer <= 0) {
                NuRndrWaterRipDat[i].timer = 0;
            } else {
                t = (float)NuRndrWaterRipDat[i].timer / (float)NuRndrWaterRipDat[i].duration;
                NuRndrWaterRipDat[i].size = (NuRndrWaterRipDat[i].startsize - NuRndrWaterRipDat[i].endsize) * t
                    + NuRndrWaterRipDat[i].endsize;
                if (t > 0.75f) {
                    fade = (1.0f - t) * 4.0f;
                } else {
                    fade = t * 1.3333334f;
                }
                NuRndrWaterRipDat[i].curcolour =
                    ((int)((float)(NuRndrWaterRipDat[i].colour >> 24) * fade) << 24)
                    + (NuRndrWaterRipDat[i].colour & 0xFFFFFF);
            }
        }
    }
}


int NuRndrStreamQuadWRip(struct nurndrstream_s *stream, short *visible) {
    GifTagQuad strip;
    GifTagQuad fan;
    int n;
    int start;
    int i;
    int count;

    count = NuRndrWaterRipCnt * 4;
    strip.dword[0] = 0x3002400000008000ULL;
    strip.dword[1] = 0x512;
    fan.dword[0] = 0x3002C00000008000ULL;
    fan.dword[1] = 0x512;
    stream->cur.w = vpDmaTag_Cnt(stream->cur.w);
    ((unsigned char *)stream->cur.w)[-14] = 0x12;
    start = 0;
    *stream->cur.w++ = 0x6C010056;
    *stream->cur.w++ = fan.word[0];
    *stream->cur.w++ = fan.word[1];
    *stream->cur.w++ = fan.word[2];
    *stream->cur.w++ = fan.word[3];
    while (count) {
        n = (count > 42) ? 42 : count;
        n &= 0xFC;
        strip.tag.nloop = n;
        *stream->cur.w++ = 0x6C0180D2;
        *stream->cur.w++ = strip.word[0];
        *stream->cur.w++ = strip.word[1];
        *stream->cur.w++ = strip.word[2];
        *stream->cur.w++ = strip.word[3];
        *stream->cur.w++ = 0x6D018002;
        *((u16 *)stream->cur.w)++ = n * 3;
        *((u16 *)stream->cur.w)++ = 0;
        *((u16 *)stream->cur.w)++ = 0;
        *((u16 *)stream->cur.w)++ = 0;
        *stream->cur.w++ = 0x01000103;
        *stream->cur.w++ = (n << 16) | 0x6E00C005;
        for (i = 0; i < n; i += 4) {
            *stream->cur.w++ = NuRndrWaterRipDat[visible[(start + i) >> 2]].curcolour;
            *stream->cur.w++ = NuRndrWaterRipDat[visible[(start + i) >> 2]].curcolour;
            *stream->cur.w++ = NuRndrWaterRipDat[visible[(start + i) >> 2]].curcolour;
            *stream->cur.w++ = NuRndrWaterRipDat[visible[(start + i) >> 2]].curcolour;
        }
        *stream->cur.w++ = 0x01000203;
        *stream->cur.w++ = (n << 17) | 0x6C008003;
        for (i = 0; i < n; i += 4) {
            *stream->cur.f++ = NuRndrWaterRipDat[visible[(start + i) >> 2]].pos.x
                - NuRndrWaterRipDat[visible[(start + i) >> 2]].size;
            *stream->cur.f++ = NuRndrWaterRipDat[visible[(start + i) >> 2]].pos.y;
            *stream->cur.f++ = NuRndrWaterRipDat[visible[(start + i) >> 2]].pos.z
                - NuRndrWaterRipDat[visible[(start + i) >> 2]].size;
            *stream->cur.w++ = 0x8000;
            *stream->cur.w++ = 0xC00;
            *stream->cur.w++ = 0x400;
            stream->cur.w += 2;
            *stream->cur.f++ = NuRndrWaterRipDat[visible[(start + i) >> 2]].pos.x
                + NuRndrWaterRipDat[visible[(start + i) >> 2]].size;
            *stream->cur.f++ = NuRndrWaterRipDat[visible[(start + i) >> 2]].pos.y;
            *stream->cur.f++ = NuRndrWaterRipDat[visible[(start + i) >> 2]].pos.z
                - NuRndrWaterRipDat[visible[(start + i) >> 2]].size;
            *stream->cur.w++ = 0x8000;
            *stream->cur.w++ = 0xFF0;
            *stream->cur.w++ = 0x400;
            stream->cur.w += 2;
            *stream->cur.f++ = NuRndrWaterRipDat[visible[(start + i) >> 2]].pos.x
                - NuRndrWaterRipDat[visible[(start + i) >> 2]].size;
            *stream->cur.f++ = NuRndrWaterRipDat[visible[(start + i) >> 2]].pos.y;
            *stream->cur.f++ = NuRndrWaterRipDat[visible[(start + i) >> 2]].pos.z
                + NuRndrWaterRipDat[visible[(start + i) >> 2]].size;
            *stream->cur.w++ = 0;
            *stream->cur.w++ = 0xC00;
            *stream->cur.w++ = 0x7F0;
            stream->cur.w += 2;
            *stream->cur.f++ = NuRndrWaterRipDat[visible[(start + i) >> 2]].pos.x
                + NuRndrWaterRipDat[visible[(start + i) >> 2]].size;
            *stream->cur.f++ = NuRndrWaterRipDat[visible[(start + i) >> 2]].pos.y;
            *stream->cur.f++ = NuRndrWaterRipDat[visible[(start + i) >> 2]].pos.z
                + NuRndrWaterRipDat[visible[(start + i) >> 2]].size;
            *stream->cur.w++ = 0;
            *stream->cur.w++ = 0xFF0;
            *stream->cur.w++ = 0x7F0;
            stream->cur.w += 2;
        }
        *stream->cur.w++ = 0x01000101;
        *stream->cur.w++ = 0x14000306;
        start += n;
        count -= n;
    }
    stream->cur.w = vpDmaTag_Close(stream->cur.w);
    return 1;
}


void NuHGobjRndr(void *hobj, struct numtx_s *mtx, int nlayers, short *layer) {
    NuHGobjEval(hobj, 0, 0, hgobj_joint_mtx);
    NuHGobjRndrMtxDwa(hobj, mtx, nlayers, layer, hgobj_joint_mtx, 0);
}


int NuHGobjRndrMtx(void *hobj, struct numtx_s *mtx, int nlayers, short *layer, struct numtx_s *dwa) {
    return NuHGobjRndrMtxDwa(hobj, mtx, nlayers, layer, dwa, 0);
}


void NuRndrInitEx(int streamsize) {
    int i;

    NuRndrStreamInit(streamsize);
    NuFntInit();
    NuMtxSetIdentity(&rndr_identity_mtx);
    NuRndrDeformerWeightsArrayInit();
    NuRndrOTInit();
    NRCylCapData = NuRndrCylCapMakeData();
    NRCylData = NuRndrCylMakeData();
    NuRndrShadowDirCol(&default_light_dir, 0x10808080);
    NuRndrShadowCnt = 0;
    for (i = 0; i < 32; i++) {
        NuRndrWaterRipDat[i].timer = 0;
    }
}


void NuRndrInit(void) {
    int i;

    NuRndrStreamInit(0x100000);
    NuFntInit();
    NuMtxSetIdentity(&rndr_identity_mtx);
    NuRndrDeformerWeightsArrayInit();
    NuRndrOTInit();
    NRCylCapData = NuRndrCylCapMakeData();
    NRCylData = NuRndrCylMakeData();
    NuRndrShadowDirCol(&default_light_dir, 0x10808080);
    NuRndrShadowCnt = 0;
    for (i = 0; i < 32; i++) {
        NuRndrWaterRipDat[i].timer = 0;
    }
}


void NuRndrClose(void) {
}


int NuRndrBeginScene(int target) {
    struct nurndrstream_s *stream;
    int onscreen;
    int offset;

    stream = rndrstream_system;
    onscreen = target == -1;
    NuRndrStreamLink(stream);
    NuRndrSetRTarget(stream, target);
    NuRndrSetZBuffer(stream, -2);
    rndrstream_free = vpDmaTag_Next(stream->cur.w, 0);
    offset = onscreen ? nurndr_half_offset : 0;
    NuRndrSetXYOffset((0x800 - (nurndr_pixel_width >> 1)) << 4,
        ((0x800 - (nurndr_pixel_height >> 1)) << 4) + offset);
    NuRndrSetScissor(0, 0, nurndr_pixel_width - 1, nurndr_pixel_height - 1);
    return 1;
}


int NuRndrBeginSceneEx(int target, int zbuffer, int halfoffset) {
    struct nurndrstream_s *stream;
    int offset;

    stream = rndrstream_system;
    NuRndrStreamLink(stream);
    NuRndrSetRTarget(stream, target);
    NuRndrSetZBuffer(stream, zbuffer);
    rndrstream_free = vpDmaTag_Next(stream->cur.w, 0);
    offset = halfoffset ? nurndr_half_offset : 0;
    NuRndrSetXYOffset((0x800 - (nurndr_pixel_width >> 1)) << 4,
        ((0x800 - (nurndr_pixel_height >> 1)) << 4) + offset);
    NuRndrSetScissor(0, 0, nurndr_pixel_width - 1, nurndr_pixel_height - 1);
    return 1;
}


void NuRndrEndScene(void) {
    NuMtlBuildRenderList();
    NuRndrOTFlush();
    NuRndrStreamFlush();
}


void NuRndrEndSceneEx(int flags) {
    if (flags & 1) {
        NuMtlBuildRenderList();
    }
    if (flags & 2) {
        NuRndrOTFlush();
    }
    NuRndrStreamFlush();
}


int NuRndrSwapScreen(int target) {
    int swapped;

    NuPs2DisableAutoScreenSwap();
    if (target == -3) {
        swapped = NuPs2ScreenSwap(0);
    } else {
        swapped = NuPs2ScreenSwap(1);
    }
    NuRndrVu1Draw();
    if (debug_flush_vram) {
        debug_flush_vram = 0;
        NuPs2FlushVRam(0xFF, 0, 0xFF);
    }
    NuRndrSwapStreamBuffers();
    if (nutex_total_xfer_this_frame > nutex_peak_xfer_per_frame) {
        nutex_peak_xfer_per_frame = nutex_total_xfer_this_frame;
    }
    nutex_total_xfer_this_frame = 0;
    nurndr_tritot_this_frame = 0;
    return swapped;
}


void NuRndrGlobalMtl(struct numtl_s *mtl) {
    GlobalMtl = mtl;
}


void NuRndrFogColour(u32 colour) {
}


void NuRndrFogDistance(float near, float far) {
}


void NuRndrFogMode(u32 mode) {
}


struct nurndrptlhead_s {
    u32 tag[4];
    u32 params[3];
    float time;
};

extern struct nurndrstream_s *rndrstream_ptl;
extern struct nurndrptlhead_s *testforcepart;
extern int PartDebugHeader(struct nurndrptlhead_s *list, int mode);

void NuRndrParticleGroup(void *chunk, struct nurndrptlhead_s *list, struct numtl_s *mtl, struct numtx_s *mtx,
                         int mode, float time) {
    struct nurndrptlhead_s *head;

    if (testforcepart) {
        list = testforcepart;
    }
    head = NuRndrStreamAlloc(2);
    list->time = time;
    *head = *list;
    NuRndrStreamLink(rndrstream_ptl);
    NuRndrStreamSetMisc(rndrstream_ptl);
    rndrstream_ptl->cur.w = vpDmaTag_CntEx(rndrstream_ptl->cur.w, 0x13000000, 0);
    rndrstream_free = vpDmaTag_Next(rndrstream_ptl->cur.w, 0);
    NuRndrSetLocalMtx(mtx, 1);
    NuRndrStreamLink(rndrstream_ptl);
    NuRndrStreamSetMisc(rndrstream_ptl);
    rndrstream_ptl->view_mtx_id = -1;
    NuRndrStreamSetViewMtx(rndrstream_ptl);
    NuRndrStreamSetLocalMtx(rndrstream_ptl);
    NuRndrStreamAddMtl(rndrstream_ptl, mtl);
    PartDebugHeader(list, mode);
    rndrstream_ptl->cur.w = vpDmaTag_Call(rndrstream_ptl->cur.w, head);
    rndrstream_ptl->cur.w = vpDmaTag_Call(rndrstream_ptl->cur.w, list + 1);
    rndrstream_ptl->cur.w = vpDmaTag_Call(rndrstream_ptl->cur.w, chunk);
    rndrstream_ptl->cur.w = vpDmaTag_CntEx(rndrstream_ptl->cur.w, 0x13000000, 0);
    rndrstream_free = vpDmaTag_Next(rndrstream_ptl->cur.w, 0);
}


void NuRndrStarfield(void) {
    NuErrorProlog(D_00614770, 0x5D5)(D_00614790);
}


/* Screen-space rectangle (see NuMtlDisplayMtl). */
struct nurndrrect_s {
    float x, y, w, h;
    float z;
    float u0, v0, u1, v1;
    struct numtl_s *mtl;
};

void NuRndrRect(struct nurndrrect_s *rect) {
    struct nuvtx2d_s vtx[4];

    vtx[0].x = rect->x;
    vtx[0].y = rect->y;
    vtx[0].z = 1.0f;
    if (rect->z > 1.0f) {
        vtx[0].rhw = 1.0f / rect->z;
    } else {
        vtx[0].rhw = 1.0f;
    }
    vtx[0].colour = -1;
    vtx[0].u = rect->u0;
    vtx[0].v = rect->v0;
    vtx[1] = vtx[0];
    vtx[1].u = rect->u1;
    vtx[1].x += rect->w;
    vtx[2] = vtx[0];
    vtx[2].v = rect->v1;
    vtx[2].y += rect->h;
    vtx[3] = vtx[2];
    vtx[3].u = rect->u1;
    vtx[3].x = vtx[1].x;
    NuRndrQuad2d(vtx, rect->mtl);
}

u32 *NuRndrFxBlueAndGreenData(u32 code, void *fx) {
    /* The target reserves two quadwords for its GIF tag scratch. */
    u32 tag[8];
    u32 *base;
    u32 *packet;
    int i;

    base = NuRndrStreamAlloc(0x18);
    packet = vpDmaTag_RetEx(base, 0x13000000, 0x6C158000);
    *(unsigned long long *)tag = 0x1003000000008001ULL;
    *packet++ = tag[0];
    *packet++ = tag[1];
    *packet++ = 1;
    *packet++ = 0;
    *(unsigned long long *)tag = 0x1003000000008001ULL;
    *packet++ = tag[0];
    *packet++ = tag[1];
    *packet++ = 0xE;
    *packet++ = 0;
    *(unsigned long long *)packet = 0xA;
    packet += 2;
    *(unsigned long long *)packet = 0x42;
    packet += 2;
    *(unsigned long long *)packet = 0x8000000068ULL;
    packet += 2;
    *(unsigned long long *)packet = 0x42;
    packet += 2;
    *(unsigned long long *)tag = 0x4403400000008001ULL;
    *packet++ = tag[0];
    *packet++ = tag[1];
    *packet++ = 0x5252;
    *packet++ = 0;

    /* Eight 320x32 strips, as GS 12.4 fixed-point coordinates. */
    for (i = 0; i < 8; i++) {
        *packet++ = 0;
        *(float *)packet++ = i * 128.0f * (1.0f / 1024.0f) + 0.0625f;
        *(float *)packet++ = (i & 1) * 320.0f * 16.0f;
        *(float *)packet++ = (i & 0xFE) * 16.0f * 16.0f;
        *(float *)packet++ = 1.0f / 64.0f;
        *(float *)packet++ = i * 128.0f * (1.0f / 1024.0f) + 0.0625f;
        *(float *)packet++ = ((i & 1) * 320.0f + 64.0f) * 16.0f;
        *(float *)packet++ = (i & 0xFE) * 16.0f * 16.0f;
    }

    *packet++ = code | 0x14000000;
    *packet++ = 0x13000000;
    *packet++ = 0;
    *packet++ = 0;
    vpDmaTag_Close(packet);
    return base;
}

void NuRndrLine3dDbg(u32 colour, float x0, float y0, float z0, float x1, float y1, float z1) {
}


void NuRndrLine3dDbgFlush(void) {
}


int NuRndrTri3dClip(void *verts, int num, void *mtx, struct numtl_s *mtl) {
    struct numtl_s *m;

    m = mtl ? mtl : numtl_defaultmtl3d;
    return NuRndrStreamTri3dClip(verts, num, mtx, &m->rs);
}


int NuRndrTriStrip3dClip(void *verts, int num, void *mtx, struct numtl_s *mtl) {
    struct numtl_s *m;

    m = mtl ? mtl : numtl_defaultmtl3d;
    return NuRndrStreamTriStrip3dClip(verts, num, mtx, &m->rs);
}


int NuRndrQuad3d(void *verts, struct numtl_s *mtl) {
    return 1;
}


void NuRndrFlush(int flags) {
    if (flags & 1) {
        NuMtlBuildRenderList();
    }
    if (flags & 2) {
        NuRndrOTFlush();
    }
    NuRndrStreamFlush();
}


void NuRndrShadowOnOff(int on) {
    nurndr_shadowson = on;
}


void NuRndrRectUV3diNS(int x, int y, int width, int height, u32 colour, u32 z,
                       float u0, float v0, float u1, float v1) {
    x += (PS2_VCNTR_X - (nurndr_pixel_width >> 1)) << 4;
    y += (PS2_VCNTR_Y - (nurndr_pixel_height >> 1)) << 4;
    *((unsigned long long *)rndrstream_3d->cur.w)++ = 6;
    *rndrstream_3d->cur.w++ = colour;
    *((float *)rndrstream_3d->cur.w)++ = 1.0f;
    *((float *)rndrstream_3d->cur.w)++ = u0;
    *((float *)rndrstream_3d->cur.w)++ = v0;
    *((short *)rndrstream_3d->cur.w)++ = x;
    *((short *)rndrstream_3d->cur.w)++ = y;
    *rndrstream_3d->cur.w++ = z;
    *((float *)rndrstream_3d->cur.w)++ = u1;
    *((float *)rndrstream_3d->cur.w)++ = v1;
    *((short *)rndrstream_3d->cur.w)++ = x + width;
    *((short *)rndrstream_3d->cur.w)++ = y + height;
    *rndrstream_3d->cur.w++ = z;
}


void NuRndrFx(int mode, void *fxdata) {
    NuLgtArcLaserDraw(mode);
    NuRndrShadowStencil(mode, fxdata);
    NuLightFxSetup();
    NuLightProcessSpot(mode);
    NuLightFogDraw(mode);
    if (nurndr_shadowson) {
        NuRndrShadow(mode);
    }
    NuRndrWater();
    txupload_on = 1;
}


void NuRndrShadowDirCol(struct nuvec_s *dir, u32 col) {
    curlight = *dir;
    curlightcol = col;
    NRFanData = NuRndrShadowLightFan();
}


void NuRndrWaterLevelTint(float level, u32 tint) {
    NuRndrWaterLevel = level;
    NuRndrWaterTint = tint;
}


void NuRndrAddShadow(struct nuvec_s *pos, float size, int shade, int xrot, int yrot, int zrot) {
    nurndrshadpol_s *shadow;

    if (NuCameraClipTestPoints(pos, 1, 0) == 0 && NuRndrShadowCnt < 128) {
        pos->y += 0.01f;
        shadow = &NuRndrShadPolDat[NuRndrShadowCnt];
        shadow->pos = *pos;
        NuRndrShadPolDat[NuRndrShadowCnt].size = size;
        shadow->shade = shade;
        shadow->xrot = xrot;
        shadow->yrot = yrot;
        shadow->zrot = zrot;
        NuRndrShadowCnt++;
    }
}


void NuRndrShadPolys(struct numtl_s *mtl) {
    if (NuRndrShadowCnt) {
        NuRndrStreamLink(rndrstream_fx);
        NuRndrStreamSetMisc(rndrstream_fx);
        NuRndrStreamAddMtl(rndrstream_fx, mtl);
        NuRndrSetLocalMtx(&numtx_identity, 2);
        NuRndrStreamSetViewMtxScissorClip(rndrstream_fx);
        NuRndrStreamSetLocalMtx(rndrstream_fx);
        NuRndrStreamFxMPG(1);
        NuRndrStreamQuadShad(rndrstream_fx);
        rndrstream_free = vpDmaTag_Next(rndrstream_fx->cur.w, 0);
    }
    NuRndrShadowCnt = 0;
}


void NuRndrAddWaterRipple(struct nuvec_s *pos, int duration, u32 colour, float startsize, float endsize) {
    nurndrwaterrip_s *rip;
    int i;

    for (i = 0; i < 32; i++) {
        rip = &NuRndrWaterRipDat[i];
        if (rip->timer == 0) {
            pos->y += 0.01f;
            rip->pos = *pos;
            rip->startsize = startsize;
            rip->size = startsize;
            rip->endsize = endsize;
            rip->colour = colour;
            rip->curcolour = 0;
            rip->timer = duration;
            rip->duration = duration;
            return;
        }
    }
}


void NuRndrWaterRip(struct numtl_s *mtl) {
    short visible[32];
    nurndrwaterrip_s *rip;
    int i;

    NuRndrWaterRipCnt = 0;
    for (i = 0; i < 32; i++) {
        rip = &NuRndrWaterRipDat[i];
        if (rip->timer != 0 && NuCameraClipTestPoints(&rip->pos, 1, 0) == 0) {
            visible[NuRndrWaterRipCnt] = i;
            NuRndrWaterRipCnt++;
        }
    }
    if (NuRndrWaterRipCnt) {
        NuRndrStreamLink(rndrstream_fx);
        NuRndrStreamSetMisc(rndrstream_fx);
        NuRndrStreamAddMtl(rndrstream_fx, mtl);
        NuRndrSetLocalMtx(&numtx_identity, 2);
        NuRndrStreamSetViewMtxScissorClip(rndrstream_fx);
        NuRndrStreamSetLocalMtx(rndrstream_fx);
        NuRndrStreamFxMPG(1);
        NuRndrStreamQuadWRip(rndrstream_fx, visible);
        rndrstream_free = vpDmaTag_Next(rndrstream_fx->cur.w, 0);
    }
}


void NuRndrInitWorld(void) {
    int i;

    NuRndrShadMaskCount = 0;
    for (i = 0; i < 64; i++) {
        NuRndrFootData[i].timer = 0;
    }
}


void NuRndrSplineSearch(void *spline) {
    int n;

    n = NuRndrShadMaskCount;
    NuRndrShadMaskCount += NuSplineFindAllSub(spline, D_0062ECA8, &NuRndrShadMasks[n], 32 - n);
}


u32 *NuRndrCreateBlendShapeDeformerWeightsArray(u32 count) {
    u32 *base;
    u32 *packet;

    base = NuRndrStreamAlloc((count + 10) >> 2);
    if (base) {
        packet = vpDmaTag_RetEx(base, 0x13000000, 0x01000101);
        *packet++ = ((count + 1) << 16) | 0x60000063;
        *packet++ = count;
        vpDmaTag_Close(packet + count);
    }
    return base;
}


void NuRndrDeformerWeightsArrayInit(void) {
    u32 *p;

    p = vpDmaTag_RetEx(D_00689A80, 0x13000000, 0x01000101);
    *p++ = 0x60010063;
    *p = 0;
    vpDmaTag_Close(p + 1);
}



extern struct nurndrstream_s *rndrstream_ot_sort;

void NuRndrOTFlush(void) {
    struct nurndrstream_s *stream;
    nurndrotbin_s *bin;
    nurndrotitem_s *item;
    struct numtl_s *mtl;
    void *mpgtag;
    struct numtx_s *mtx;
    int mtxmode;

    mtx = 0;
    mpgtag = 0;
    mtxmode = -1;
    mtl = 0;
    if (ot_batch_free == 0x400) {
        return;
    }
    stream = rndrstream_ot_sort;
    NuRndrStreamLink(stream);
    NuRndrStreamSetMisc(stream);
    NuRndrStreamSetViewMtx(stream);
    *((struct DmaTag *)stream->cur.w)++ = *(struct DmaTag *)ot_close_packet;
    bin = &ot_bins[0xFFF];
    do {
        for (item = bin->head; item != 0; item = item->next) {
            if (item->mtl != mtl) {
                mtl = item->mtl;
                NuRndrStreamAddMtl(stream, mtl);
                if (mtl->rs.mpgtag != mpgtag) {
                    mpgtag = mtl->rs.mpgtag;
                    stream->cur.w = vpDmaTag_Call(stream->cur.w, mpgtag);
                }
            }
            if (item->mtx != mtx || item->mtxmode != mtxmode) {
                mtx = item->mtx;
                mtxmode = item->mtxmode;
                NuRndrSetLocalMtx(mtx, mtxmode);
                NuRndrStreamSetLocalMtx(stream);
            }
            stream->cur.w = vpDmaTag_Call(stream->cur.w, item->dma);
        }
        bin->head = 0;
        bin--;
    } while (bin >= ot_bins);
    rndrstream_free = vpDmaTag_Next(stream->cur.w, 0);
    ot_batch_free = 0x400;
}


void NuRndrTest(void) {
}
