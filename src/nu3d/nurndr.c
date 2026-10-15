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

#include "nu3d/nucamera.h"
#include "nu3d/nugobj.h"
#include "nu3d/nugscn.h"
#include "nu3d/numtl.h"
#include "nu3d/nustream.h"
#include "nu3d/nutex.h"
#include "nu3d/nuvport.h"
#include "nucore/nuerror.h"
#include "numath/numathtypes.h"
#include "numath/numtx.h"
#include "numath/nutrig.h"
#include "numath/nuvec.h"
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
    char unk00[0x32];
    char active;
    char unk33[5];
} nurndrfoot_s;

extern nurndrfoot_s NuRndrFootData[];

extern char D_00614770[];
#define nurndr_file D_00614770
extern char D_00614790[];

extern int NuHGobjRndrMtxDwa(void *hobj, struct numtx_s *mtx, int nlayers, short *layer, struct numtx_s *dwa, int blend);
extern void NuHGobjEval(void *hobj, int njoints, void *joints, struct numtx_s *mtx);
extern struct numtx_s D_0067DA40[];
#define hgobj_joint_mtx D_0067DA40
extern int NuRndrStreamTri3dClip(void *verts, int num, void *mtx, void *strm);
extern int NuRndrStreamTriStrip3dClip(void *verts, int num, void *mtx, void *strm);
extern void NuRndrOTFlush(void);
extern void NuFntInit(void);
extern void NuRndrDeformerWeightsArrayInit(void);
extern void NuRndrOTInit(void);
extern u32 *NuRndrCylMakeData(void);
extern void NuRndrShadowDirCol(struct nuvec_s *dir, u32 col);
extern u32 *NRCylCapData;
extern u32 *NRCylData;
extern int NuRndrShadowCnt;
extern struct numtx_s D_00689A40;
#define rndr_identity_mtx D_00689A40
extern struct nuvec_s D_00614760;
#define default_light_dir D_00614760

extern void NuLgtArcLaserDraw(int mode);
extern void NuRndrShadowStencil(int mode, void *fxdata);
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
} nurndrotitem_s;

typedef struct nurndrotbin_s {
    char pad00[0xC];
    nurndrotitem_s *head;
    char pad10[0xC];
} nurndrotbin_s;

typedef struct ottri_s {
    struct ottri_s *next;
    void *tri;
} ottri_s;

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
extern int NRFanData;
extern int NuRndrShadowLightFan(void);

extern struct nugspline_s *NuRndrShadMasks[];
extern unsigned char D_0062ECA8[];

extern unsigned int D_00689A80[];
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

/* Eight entries per channel; names of the sprite parameters remain unknown. */
struct FxSprite2 {
    float parameter_00;
    int parameter_04;
    int count;
    float parameter_0c;
    float parameter_10;
    float parameter_14;
    float parameter_18;
    int parameter_1c[8];
    int colour[8];
    float channel[24][8];
};

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
extern void NuRndrOTInsertTriBatch(struct numtx_s *mtx, int clip, struct numtl_s *mtl, void *tris, int count);
extern u32 *vpDmaTag_Call(u32 *packet, void *addr);
extern struct NuPs2GsReg D_002EA1A0;
#define NuPs2ZBufReg0 D_002EA1A0
extern u32 *vpDmaTag_Cnt(u32 *packet);

struct nuvtx2d_s {
    float x, y, z;
    float rhw;
    union {
        int colour;
        unsigned char rgba[4];
    };
    float u, v;
};

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
                        rndrstream_3d = &mtl->stream;
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
                    mtl = mtl->display_next;
                } while (mtl);
                } while ((geom = geom->next));
            }
            for (fgeom = gobj->faceon_geom; fgeom; fgeom = fgeom->next) {
                mtl = fgeom->mtl;
                if (!mtl) {
                    NuDebugMsgProlog(nurndr_file, 0x666)(err_fgeom_no_mtl);
                }
                if (mtl != lastmtl) {
                    rndrstream_3d = &mtl->stream;
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
                        rndrstream_3d = &mtl->stream;
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
                    mtl = mtl->display_next;
                } while (mtl);
            }
            for (fgeom = gobj->faceon_geom; fgeom; fgeom = fgeom->next) {
                if (!fgeom->mtl) {
                    NuDebugMsgProlog(nurndr_file, 0x707)(err_fgeom_no_mtl);
                }
                faceonmtl = fgeom->mtl;
                if (faceonmtl != lastmtl) {
                    rndrstream_3d = &faceonmtl->stream;
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


struct FxSprite {
    float parameter_00;
    int parameter_04;
    int count;
    float parameter_0c;
    float parameter_10;
    float parameter_14;
    float parameter_18;
    int parameter_1c[16];
    int colour[16];
    float channel[12][16];
};

u32 *NuRndrFxSpriteSet(u32 code, struct FxSprite *fx) {
    u32 tag[8];
    u32 *base;
    u32 *packet;
    int i;

    base = NuRndrStreamAlloc(fx->count * 5 + 8);
    packet = vpDmaTag_RetEx(base, 0x13000000,
        ((fx->count * 5 + 4) << 16) | 0x6c008000);
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
    *(float *)packet++ = fx->parameter_00;
    *packet++ = 0;
    *packet++ = fx->parameter_04;
    *packet++ = fx->count;
    *(float *)packet++ = fx->parameter_14;
    *(float *)packet++ = fx->parameter_18;
    *(float *)packet++ = fx->parameter_0c;
    *(float *)packet++ = fx->parameter_10;

    for (i = 0; i < fx->count; i++) {
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = fx->parameter_1c[i];
        *packet++ = fx->colour[i];
        *packet++ = fx->colour[i] >> 8;
        *packet++ = (short)(fx->colour[i] >> 16);
        *packet++ = (signed char)(fx->colour[i] >> 24);
        *(float *)packet++ = fx->channel[2][i];
        *(float *)packet++ = fx->channel[3][i];
        *(float *)packet++ = fx->channel[0][i];
        *(float *)packet++ = fx->channel[1][i];
        *(float *)packet++ = fx->channel[6][i];
        *(float *)packet++ = fx->channel[7][i];
        *(float *)packet++ = fx->channel[4][i];
        *(float *)packet++ = fx->channel[5][i];
        *(float *)packet++ = fx->channel[10][i];
        *(float *)packet++ = fx->channel[11][i];
        *(float *)packet++ = fx->channel[8][i];
        *(float *)packet++ = fx->channel[9][i];
    }

    *packet++ = code | 0x14000000;
    *packet++ = 0x13000000;
    *packet++ = 0;
    *packet++ = 0;
    vpDmaTag_Close(packet);
    return base;
}


u32 *NuRndrFxSprite2Set(u32 code, struct FxSprite2 *fx) {
    /* The target reserves two quadwords for its GIF tag scratch. */
    u32 tag[8];
    u32 *base;
    u32 *packet;
    int i;

    base = NuRndrStreamAlloc(fx->count * 8 + 8);
    packet = vpDmaTag_RetEx(base, 0x13000000,
        ((fx->count * 8 + 4) << 16) | 0x6c008000);
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
    *packet++ = (u32)fx->parameter_00;
    *packet++ = 0;
    *packet++ = fx->parameter_04;
    *packet++ = fx->count;
    *(float *)packet++ = fx->parameter_14;
    *(float *)packet++ = fx->parameter_18;
    *(float *)packet++ = fx->parameter_0c;
    *(float *)packet++ = fx->parameter_10;

    for (i = 0; i < fx->count; i++) {
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = fx->parameter_1c[i];
        *packet++ = fx->colour[i];
        *packet++ = fx->colour[i] >> 8;
        *packet++ = (short)(fx->colour[i] >> 16);
        *packet++ = (signed char)(fx->colour[i] >> 24);
        *(float *)packet++ = fx->channel[2][i];
        *(float *)packet++ = fx->channel[3][i];
        *(float *)packet++ = fx->channel[0][i];
        *(float *)packet++ = fx->channel[1][i];
        *(float *)packet++ = fx->channel[6][i];
        *(float *)packet++ = fx->channel[7][i];
        *(float *)packet++ = fx->channel[4][i];
        *(float *)packet++ = fx->channel[5][i];
        *(float *)packet++ = fx->channel[10][i];
        *(float *)packet++ = fx->channel[11][i];
        *(float *)packet++ = fx->channel[8][i];
        *(float *)packet++ = fx->channel[9][i];
        *(float *)packet++ = fx->channel[14][i];
        *(float *)packet++ = fx->channel[15][i];
        *(float *)packet++ = fx->channel[12][i];
        *(float *)packet++ = fx->channel[13][i];
        *(float *)packet++ = fx->channel[18][i];
        *(float *)packet++ = fx->channel[19][i];
        *(float *)packet++ = fx->channel[16][i];
        *(float *)packet++ = fx->channel[17][i];
        *(float *)packet++ = fx->channel[22][i];
        *(float *)packet++ = fx->channel[23][i];
        *(float *)packet++ = fx->channel[20][i];
        *(float *)packet++ = fx->channel[21][i];
    }

    *packet++ = code | 0x14000000;
    *packet++ = 0x13000000;
    *packet++ = 0;
    *packet++ = 0;
    vpDmaTag_Close(packet);
    return base;
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

u32 *NuRndrFxBlueAndGreenData(u32 code) {
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

/* Parameters of the single-channel colour filter; names remain unknown. */
struct FxBlueOrGreen {
    float parameter_00;
    float parameter_04;
    float parameter_08;
    float parameter_0c;
    int parameter_10;
};

u32 *NuRndrFxBlueOrGreenData(u32 code, struct FxBlueOrGreen *fx) {
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

void NuRndrLine3dDbg(u32 colour, float x0, float y0, float z0, float x1, float y1, float z1) {
}


void NuRndrLine3dDbgFlush(void) {
}


int NuRndrTri3dClip(void *verts, int num, void *mtx, struct numtl_s *mtl) {
    struct numtl_s *m;

    m = mtl ? mtl : numtl_defaultmtl3d;
    return NuRndrStreamTri3dClip(verts, num, mtx, &m->stream);
}


int NuRndrTriStrip3dClip(void *verts, int num, void *mtx, struct numtl_s *mtl) {
    struct numtl_s *m;

    m = mtl ? mtl : numtl_defaultmtl3d;
    return NuRndrStreamTriStrip3dClip(verts, num, mtx, &m->stream);
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
        NuRndrFootData[i].active = 0;
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

/* Near-match (stays asm): retail loads stream->cur.w before materialising the
   ot_close_packet / bin addresses, so the quadword copy schedules differently. */
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
    *((u128 *)stream->cur.w)++ = *(u128 *)ot_close_packet;
    bin = &ot_bins[0xFFF];
    do {
        for (item = bin->head; item != 0; item = item->next) {
            if (item->mtl != mtl) {
                mtl = item->mtl;
                NuRndrStreamAddMtl(stream, mtl);
                if (mtl->stream.mpgtag != mpgtag) {
                    mpgtag = mtl->stream.mpgtag;
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

void NuRndrOTInsertItem(nurndrotitem_s *item) {
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


void OTTriInit(void) {
    memset(ot_tri, 0, 0x4000);
    ot_highest = 0;
}


int OTTriInsert(void *tri, float depth) {
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


float GlobalCam3DNClip(struct nuvec_s *p0, struct nuvec_s *p1, struct nuvec_s *p2) {
    struct nuvec_s normal;

    NuVecCrossRel(&normal, p0, p1, p2);
    return (p0->x - global_camera.mtx._30) * normal.x
        + (p0->y - global_camera.mtx._31) * normal.y
        + (p0->z - global_camera.mtx._32) * normal.z;
}


void RotateCyl(struct nuvec_s *v, float xang, float yang) {
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


void DeRotateVectorAngles(struct nuvec_s *dir, float *xang, float *yang) {
    *yang = NuAtan2D(dir->x, dir->z);
    *xang = NuAtan2D(-dir->y, NuFsqrt(dir->x * dir->x + dir->z * dir->z));
}


void NuRndrTest(void) {
}


void NuRndrAnglesZX(struct nuvec_s *dir, struct nuvec_s *angles) {
    struct nuvec_s rotated;

    angles->x = NuAtan2D(dir->z, dir->y);
    NuVecRotateX(&rotated, dir, (int)-angles->x);
    angles->z = -NuAtan2D(rotated.x, rotated.y);
}
