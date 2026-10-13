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

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned long long u64;

typedef struct { float x, y, z; } NuVec3;

typedef struct { float m[4][4]; } NuMtx;

typedef struct GsFrameReg {
    u64 fbp : 9;
    u64 pad09 : 7;
    u64 fbw : 6;
    u64 pad16 : 2;
    u64 psm : 6;
    u64 pad1e : 2;
    u64 fbmsk : 32;
} GsFrameReg;

typedef struct GifTag {
    u64 nloop : 15;
    u64 eop : 1;
    u64 pad16 : 30;
    u64 pre : 1;
    u64 prim : 11;
    u64 flg : 2;
    u64 nreg : 4;
    u64 regs;
} GifTag;

typedef union GifTagQuad {
    GifTag tag;
    u64 dword[2];
    u32 word[4];
} GifTagQuad;

typedef struct GsZbufReg {
    u64 zbp : 9;
    u64 pad09 : 15;
    u64 psm : 4;
    u64 pad1c : 4;
    u64 zmsk : 1;
    u64 pad21 : 31;
} GsZbufReg;

typedef struct {
    NuMtx mtx;
    float fov;
} NuCamera;

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

typedef void (*NuErrorFunc)(const char *fmt, ...);
extern NuErrorFunc NuErrorProlog(const char *file, int line);
extern char D_00614770[];
extern char D_00614790[];

extern int NuHGobjRndrMtxDwa(void *hobj, NuMtx *mtx, int nlayers, short *layer, NuMtx *dwa, int blend);
extern void NuHGobjEval(void *hobj, int njoints, void *joints, NuMtx *mtx);
extern NuMtx D_0067DA40[];
#define hgobj_joint_mtx D_0067DA40
extern int NuRndrStreamTri3dClip(void *verts, int num, void *mtx, void *strm);
extern int NuRndrStreamTriStrip3dClip(void *verts, int num, void *mtx, void *strm);
extern void NuMtlBuildRenderList(void);
extern void NuRndrOTFlush(void);
extern void NuRndrStreamFlush(void);
extern void NuRndrStreamInit(int size);
extern void NuFntInit(void);
extern void NuMtxSetIdentity(NuMtx *m);
extern void NuRndrDeformerWeightsArrayInit(void);
extern void NuRndrOTInit(void);
extern int NuRndrCylMakeData(void);
extern void NuRndrShadowDirCol(NuVec3 *dir, u32 col);
extern u32 *NRCylCapData;
extern int NRCylData;
extern int NuRndrShadowCnt;
extern NuMtx D_00689A40;
#define rndr_identity_mtx D_00689A40
extern NuVec3 D_00614760;
#define default_light_dir D_00614760

extern void NuLgtArcLaserDraw(int mode);
extern void NuRndrShadowStencil(int mode, void *fxdata);
extern void NuLightFxSetup(void);
extern void NuLightProcessSpot(int mode);
extern void NuLightFogDraw(int mode);
extern void NuRndrShadow(int mode);
extern void NuRndrWater(void);
extern int txupload_on;

extern NuCamera global_camera;
extern void NuVecCrossRel(NuVec3 *dst, NuVec3 *origin, NuVec3 *a, NuVec3 *b);
extern int NuAtan2D(float x, float z);
extern float NuFsqrt(float x);
extern void NuVecRotateX(NuVec3 *dst, NuVec3 *src, int angle);

typedef struct nurndrwaterrip_s {
    NuVec3 pos;
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
    char pad00[0xC];
    struct nurndrotitem_s *next;
    float depth;
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

extern void *memset(void *s, int c, unsigned int n);
extern ottri_s *ot_tri[];
extern ottri_s ot_tribuff[];
extern ottri_s *ot_free;
extern int ot_highest;
extern float NuTrigTable[];

extern NuVec3 curlight;
extern u32 curlightcol;
extern int NRFanData;
extern int NuRndrShadowLightFan(void);

extern int NuRndrShadMasks[];
extern unsigned char D_0062ECA8[];
extern int NuSplineFindAllSub(void *spline, void *buf, int *masks, int maxcount);

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


/* Only the packet cursor at offset 4 is used by the 2D packet writers. */
struct RndrPacketStream {
    void *unknown_00;
    u32 *cur;
};

extern struct RndrPacketStream *rndrstream_2d;
extern struct RndrPacketStream *rndrstream_3d;
extern int nurndr_pixel_width;
extern int nurndr_pixel_height;
extern u32 *rndrstream_free;
extern struct numtl_s *numtl_defaultmtl2d;
extern int PS2_REZ_W;
extern int PS2_REZ_H;
extern int PS2_VCNTR_X;
extern int PS2_VCNTR_Y;

extern void NuRndrStreamLink(struct RndrPacketStream *stream);
extern struct RndrPacketStream *rndrstream_system;
extern int nurndr_half_offset;
extern void NuRndrSetXYOffset(int x, int y);
extern void NuRndrSetScissor(int x0, int y0, int x1, int y1);
extern void NuPs2DisableAutoScreenSwap(void);
extern int NuPs2ScreenSwap(int vsync);
extern void NuRndrVu1Draw(void);
extern void NuPs2FlushVRam(int a, int b, int c);
extern void NuRndrSwapStreamBuffers(void);
extern int debug_flush_vram;
extern int nutex_total_xfer_this_frame;
extern int nutex_peak_xfer_per_frame;
extern int nurndr_tritot_this_frame;
extern struct RndrPacketStream *rndrstream_fx;
extern NuMtx numtx_identity;
extern void NuRndrSetLocalMtx(NuMtx *mtx, int mode);
extern void NuRndrStreamSetViewMtxScissorClip(struct RndrPacketStream *stream);
extern void NuRndrStreamSetLocalMtx(struct RndrPacketStream *stream);
extern void NuRndrStreamFxMPG(int microcode);
extern int NuRndrStreamQuadShad(struct RndrPacketStream *stream);
extern int NuRndrStreamQuadWRip(struct RndrPacketStream *stream, short *visible);
typedef struct nurndrshadpol_s {
    NuVec3 pos;
    float size;
    short shade;
    short xrot;
    short yrot;
    short zrot;
} nurndrshadpol_s;

extern nurndrshadpol_s NuRndrShadPolDat[];
extern int NuCameraClipTestPoints(NuVec3 *pos, int count, int mode);




extern void NuRndrStreamSetMisc(struct RndrPacketStream *stream);
extern void NuRndrStreamAddMtl(struct RndrPacketStream *stream, struct numtl_s *mtl);
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

extern void NuRndrStreamClearZBState(void);
extern void NuRndrStreamSetZBState(u32 zbuf, u32 psm, int zmask);
extern u32 *NuRndrStreamAddZBState(u32 *packet, int index);
extern void NuPs2GetCurrentBackBuffer(GsFrameReg *reg);
extern void NuPs2GetCurrentFrontBuffer(GsFrameReg *reg);
extern void NuPs2GetCurrentZBuffer(GsZbufReg *reg);
extern u32 *NuPs2CopyCurrentBackBuffer(u32 *packet);
extern u32 *NuPs2CopyCurrentFrontBuffer(u32 *packet);
extern int Ps2TxValToPowerOfTwo(int value);
extern u32 *NuTexSetRenderTarget(u32 *packet, int tex);
extern int nurndr_pixel_p2w;
extern int nurndr_pixel_p2h;
extern u32 *NuPs2CopyCurrentZBuffer(u32 *packet);
extern u32 *NuTexSetZBuffer(u32 *packet, int tex);

extern u32 *NuRndrStreamAlloc(int count);

void NuRndrSetZBuffer(struct RndrPacketStream *stream, int zbuffer) {
    GsFrameReg frame;

    if (zbuffer <= 0) {
        NuRndrStreamClearZBState();
        stream->cur = vpDmaTag_CntEx(stream->cur, 0x11000000, 0x50000000);
        ((unsigned char *)stream->cur)[-14] = 0xB;
        stream->cur--;
        stream->cur = vtBeginDirect(stream->cur, 0);
        stream->cur = gsBeginGifPacket(stream->cur, 0, 1, 0, 0, 0, 1, 0xE);
        switch (zbuffer) {
        default:
            NuPs2GetCurrentBackBuffer(&frame);
            NuRndrStreamSetZBState(frame.fbp << 5, frame.psm, 0);
            stream->cur = NuRndrStreamAddZBState(stream->cur, 1);
            break;
        case -3:
            NuPs2GetCurrentFrontBuffer(&frame);
            NuRndrStreamSetZBState(frame.fbp << 5, frame.psm, 0);
            stream->cur = NuRndrStreamAddZBState(stream->cur, 1);
            break;
        case -2:
            stream->cur = NuPs2CopyCurrentZBuffer(stream->cur);
            break;
        }
        stream->cur = gsCloseGifPacket(stream->cur);
        stream->cur = vtCloseDirect(stream->cur);
        stream->cur = vpDmaTag_Close(stream->cur);
    } else {
        stream->cur = NuTexSetZBuffer(stream->cur, zbuffer);
    }
}


void NuRndrSetRTarget(struct RndrPacketStream *stream, int target) {
    GsFrameReg frame;
    GsZbufReg zbuf;

    if (target <= 0) {
        stream->cur = vpDmaTag_CntEx(stream->cur, 0x13000000, 0x50000000);
        ((unsigned char *)stream->cur)[-14] = 9;
        stream->cur--;
        stream->cur = vtBeginDirect(stream->cur, 0);
        stream->cur = gsBeginGifPacket(stream->cur, 0, 1, 0, 0, 0, 1, 0xE);
        switch (target) {
        default:
            stream->cur = NuPs2CopyCurrentBackBuffer(stream->cur);
            break;
        case -3:
            stream->cur = NuPs2CopyCurrentFrontBuffer(stream->cur);
            break;
        case -2:
            NuPs2GetCurrentBackBuffer(&frame);
            NuPs2GetCurrentZBuffer(&zbuf);
            frame.fbp = zbuf.zbp;
            frame.psm = zbuf.psm | 0x30;
            *((GsFrameReg *)stream->cur)++ = frame;
            *((u64 *)stream->cur)++ = 0x4C;
            break;
        case -4:
            NuPs2GetCurrentBackBuffer(&frame);
            NuPs2GetCurrentZBuffer(&zbuf);
            frame.fbp = zbuf.zbp;
            frame.psm = 0x30;
            *((GsFrameReg *)stream->cur)++ = frame;
            *((u64 *)stream->cur)++ = 0x4C;
            break;
        }
        stream->cur = gsCloseGifPacket(stream->cur);
        stream->cur = vtCloseDirect(stream->cur);
        stream->cur = vpDmaTag_Close(stream->cur);
        nurndr_pixel_width = PS2_REZ_W;
        nurndr_pixel_height = PS2_REZ_H;
        nurndr_pixel_p2w = Ps2TxValToPowerOfTwo(PS2_REZ_W);
        nurndr_pixel_p2h = Ps2TxValToPowerOfTwo(PS2_REZ_H);
    } else {
        stream->cur = NuTexSetRenderTarget(stream->cur, target);
    }
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
    rndrstream_2d->cur = vpDmaTag_CntEx(rndrstream_2d->cur,
                                      0x10000000, 0x50000000) - 1;
    rndrstream_2d->cur = vtBeginDirect(rndrstream_2d->cur, 0);
    rndrstream_2d->cur = gsBeginGifPacket(rndrstream_2d->cur,
        0, 1, 1, 4, 0, 9, 1, 2, 5, 1, 5, 1, 5, 1, 5);

    *rndrstream_2d->cur++ = colours[0];
    *rndrstream_2d->cur++ = colours[1];
    *rndrstream_2d->cur++ = colours[2];
    *rndrstream_2d->cur++ = colours[3];
    *rndrstream_2d->cur++ = 0;
    *rndrstream_2d->cur++ = 0;
    *(float *)rndrstream_2d->cur++ = 1.0f;
    rndrstream_2d->cur++;
    *rndrstream_2d->cur++ = x;
    *rndrstream_2d->cur++ = y;
    *rndrstream_2d->cur++ = 0;
    *rndrstream_2d->cur++ = 0;

    *rndrstream_2d->cur++ = colours[4];
    *rndrstream_2d->cur++ = colours[5];
    *rndrstream_2d->cur++ = colours[6];
    *rndrstream_2d->cur++ = colours[7];
    *rndrstream_2d->cur++ = x + width;
    *rndrstream_2d->cur++ = y;
    *rndrstream_2d->cur++ = 0;
    *rndrstream_2d->cur++ = 0;

    *rndrstream_2d->cur++ = colours[8];
    *rndrstream_2d->cur++ = colours[9];
    *rndrstream_2d->cur++ = colours[10];
    *rndrstream_2d->cur++ = colours[11];
    *rndrstream_2d->cur++ = x;
    *rndrstream_2d->cur++ = y + height;
    *rndrstream_2d->cur++ = 0;
    *rndrstream_2d->cur++ = 0;

    *rndrstream_2d->cur++ = colours[12];
    *rndrstream_2d->cur++ = colours[13];
    *rndrstream_2d->cur++ = colours[14];
    *rndrstream_2d->cur++ = colours[15];
    *rndrstream_2d->cur++ = x + width;
    *rndrstream_2d->cur++ = y + height;
    *rndrstream_2d->cur++ = 0;
    *rndrstream_2d->cur++ = 0;

    rndrstream_2d->cur = gsCloseGifPacket(rndrstream_2d->cur);
    rndrstream_2d->cur = vtCloseDirect(rndrstream_2d->cur);
    rndrstream_2d->cur = vpDmaTag_Close(rndrstream_2d->cur);
    rndrstream_free = vpDmaTag_Next(rndrstream_2d->cur, 0);
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


void NuHGobjRndr(void *hobj, NuMtx *mtx, int nlayers, short *layer) {
    NuHGobjEval(hobj, 0, 0, hgobj_joint_mtx);
    NuHGobjRndrMtxDwa(hobj, mtx, nlayers, layer, hgobj_joint_mtx, 0);
}


int NuHGobjRndrMtx(void *hobj, NuMtx *mtx, int nlayers, short *layer, NuMtx *dwa) {
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
    struct RndrPacketStream *stream;
    int onscreen;
    int offset;

    stream = rndrstream_system;
    onscreen = target == -1;
    NuRndrStreamLink(stream);
    NuRndrSetRTarget(stream, target);
    NuRndrSetZBuffer(stream, -2);
    rndrstream_free = vpDmaTag_Next(stream->cur, 0);
    offset = onscreen ? nurndr_half_offset : 0;
    NuRndrSetXYOffset((0x800 - (nurndr_pixel_width >> 1)) << 4,
        ((0x800 - (nurndr_pixel_height >> 1)) << 4) + offset);
    NuRndrSetScissor(0, 0, nurndr_pixel_width - 1, nurndr_pixel_height - 1);
    return 1;
}


int NuRndrBeginSceneEx(int target, int zbuffer, int halfoffset) {
    struct RndrPacketStream *stream;
    int offset;

    stream = rndrstream_system;
    NuRndrStreamLink(stream);
    NuRndrSetRTarget(stream, target);
    NuRndrSetZBuffer(stream, zbuffer);
    rndrstream_free = vpDmaTag_Next(stream->cur, 0);
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


void NuRndrLine3dDbg(u32 colour, float x0, float y0, float z0, float x1, float y1, float z1) {
}


void NuRndrLine3dDbgFlush(void) {
}


int NuRndrTri3dClip(void *verts, int num, void *mtx, struct numtl_s *mtl) {
    struct numtl_s *m;

    m = mtl ? mtl : numtl_defaultmtl3d;
    return NuRndrStreamTri3dClip(verts, num, mtx, (char *)m + 0x124);
}


int NuRndrTriStrip3dClip(void *verts, int num, void *mtx, struct numtl_s *mtl) {
    struct numtl_s *m;

    m = mtl ? mtl : numtl_defaultmtl3d;
    return NuRndrStreamTriStrip3dClip(verts, num, mtx, (char *)m + 0x124);
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
    *((unsigned long long *)rndrstream_3d->cur)++ = 6;
    *rndrstream_3d->cur++ = colour;
    *((float *)rndrstream_3d->cur)++ = 1.0f;
    *((float *)rndrstream_3d->cur)++ = u0;
    *((float *)rndrstream_3d->cur)++ = v0;
    *((short *)rndrstream_3d->cur)++ = x;
    *((short *)rndrstream_3d->cur)++ = y;
    *rndrstream_3d->cur++ = z;
    *((float *)rndrstream_3d->cur)++ = u1;
    *((float *)rndrstream_3d->cur)++ = v1;
    *((short *)rndrstream_3d->cur)++ = x + width;
    *((short *)rndrstream_3d->cur)++ = y + height;
    *rndrstream_3d->cur++ = z;
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


void NuRndrShadowDirCol(NuVec3 *dir, u32 col) {
    curlight = *dir;
    curlightcol = col;
    NRFanData = NuRndrShadowLightFan();
}


void NuRndrWaterLevelTint(float level, u32 tint) {
    NuRndrWaterLevel = level;
    NuRndrWaterTint = tint;
}


void NuRndrAddShadow(NuVec3 *pos, float size, int shade, int xrot, int yrot, int zrot) {
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
        rndrstream_free = vpDmaTag_Next(rndrstream_fx->cur, 0);
    }
    NuRndrShadowCnt = 0;
}


void NuRndrAddWaterRipple(NuVec3 *pos, int duration, u32 colour, float startsize, float endsize) {
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
        rndrstream_free = vpDmaTag_Next(rndrstream_fx->cur, 0);
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


float GlobalCam3DNClip(NuVec3 *p0, NuVec3 *p1, NuVec3 *p2) {
    NuVec3 normal;

    NuVecCrossRel(&normal, p0, p1, p2);
    return (p0->x - global_camera.mtx.m[3][0]) * normal.x
        + (p0->y - global_camera.mtx.m[3][1]) * normal.y
        + (p0->z - global_camera.mtx.m[3][2]) * normal.z;
}


void RotateCyl(NuVec3 *v, float xang, float yang) {
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


void DeRotateVectorAngles(NuVec3 *dir, float *xang, float *yang) {
    *yang = NuAtan2D(dir->x, dir->z);
    *xang = NuAtan2D(-dir->y, NuFsqrt(dir->x * dir->x + dir->z * dir->z));
}


void NuRndrTest(void) {
}


void NuRndrAnglesZX(NuVec3 *dir, NuVec3 *angles) {
    NuVec3 rotated;

    angles->x = NuAtan2D(dir->z, dir->y);
    NuVecRotateX(&rotated, dir, (int)-angles->x);
    angles->z = -NuAtan2D(rotated.x, rotated.y);
}
