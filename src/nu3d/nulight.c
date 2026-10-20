/*
 * Unit: nu3d/nulight
 *
 * Functions:
 *   0x00132090 NuLightMatInit
 *   0x00135160 NuLightInit
 *   0x001357e0 NuLightUpdate
 *   0x00135948 NuLightFxSetup
 *   0x00135cb8 NuLightProcessSpot
 *   0x00139788 NuLightHaze
 *   0x0013af40 NuLightPolyHaze
 *   0x0013cbe8 NuLightFogDraw
 *   0x0013f268 NuLigthSetModeMask
 *   0x0013f408 NuLgtArcLaserDraw
 *   0x0013ff70 NuLightClose
 *   0x0013ff78 NuLightCreate
 *   0x0013ffc0 NuLightDestroy
 *   0x00140018 NuLightGetLight
 *   0x00140030 NuLigthSetPolyHazeMat
 *   0x00140088 NuLightAddSpot
 *   0x00140108 NuLightAddSpotXSpanFade
 *   0x00140190 NuLightFog
 *   0x001401b0 NuLightFogClear
 *   0x001401b8 NuLgtArcLaser
 *   0x001402c8 NuLgtSetArcMat
 *   0x001402e0 NuLightAddHazePoly
 *   0x00140340 NuLightSprDmaOpen
 *   0x001403d0 NuLightSprDmaClose
 *   0x00140400 NuLightSprMake
 *   0x001404e8 NuLgtRand
 */

#include "nu_libc.h"
#include "nu_types.h"

#include "nu3d/nucamera.h"
#include "nu3d/numtl.h"
#include "nu3d/nurndr.h"
#include "nu3d/nustream.h"
#include "nu3d/nutex.h"
#include "nu3d/nuvport.h"
#include "numath/numathtypes.h"
#include "numath/nutrig.h"
#include "numath/nuvec.h"
#include "nu3d/nulight.h"

typedef struct nulgtspot_s {
    struct nuvec_s pos;
    float size;
    u32 colour;
    u32 mode;
    float brightness;
    u8 xspan;
    u8 fade;
    char pad[2];
} nulgtspot_s;

typedef struct nulgtarclaser_s {
    struct nuvec_s start;
    struct nuvec_s end;
    struct nuvec_s mid;
    float sizew;
    float sizel;
    float sizewob;
    float wobspeed;
    char type;
    char pad[3];
    union {
        int colour;
        u8 rgba[4];
    };
    int seed;
} nulgtarclaser_s;

typedef struct nulgtpolyhaze_s {
    struct nuvec_s pos;
    float size;
    float rot;
} nulgtpolyhaze_s;

extern int NuRndrFogClear;
extern struct nulight_s D_006AD100[];
extern struct nuvec_s D_006AD580[];
extern struct nucolour3_s D_006AD5A8[];
extern struct nulight_s *D_006AD5D0[];
extern float NuRndrFogNear;
extern float NuRndrFogFar;
extern int NuRndrFogCol;
extern int NuRndrFogBlur;
extern int HazeColour;
extern void *NuLgtArcMtl;
extern float NuLgtArcU0;
extern float NuLgtArcV0;
extern float NuLgtArcU1;
extern float NuLgtArcV1;
extern int NuLgtSeed;
extern void *NuLightHazePolymtl;
extern float NuLightHazePolyus[4];
extern float NuLightHazePolyvs[4];
extern int NuLightSpotCnt;
extern nulgtspot_s NuLightSpots[];
extern int NuLgtArcLaserCnt;
extern int NuLgtArcLaserFrame;
extern nulgtarclaser_s NuLgtArcLaserData[];
extern int NuLightHazePolyCnt;
extern nulgtpolyhaze_s NuLgtPolyHazeData[];
extern void *NuLgtBuff;
extern int PS2_REZ_W;
extern int PS2_REZ_H;
extern int PS2_VCNTR_X;
extern int PS2_VCNTR_Y;

extern void *vpDmaTag_RetEx(void *packet, u32 vif0, u32 vif1);
extern void *vpDmaTag_Close(void *packet);
extern void *vtBeginDirect(void *packet, u32 irq);
extern void *vtCloseDirect(void *packet);
extern void *vpDmaTag_CntEx(void *packet, u32 vif0, u32 vif1);
extern void *gsBeginGifPacket(void *packet, int eop, int nloop, int pre, int prim, int flg, int nreg, ...);
extern void *gsCloseGifPacket(void *packet);
extern struct nurndrstream_s *rndrstream_3d;
extern struct nurndrstream_s *rndrstream_3d_u;
extern u32 *rndrstream_free;
extern u64 cur_rt;
extern int txupload_on;
extern int NuLgtHSurf[18];
extern struct numtl_s *NuLgtSurf[18];
extern struct numtl_s *NuLightZMat;
extern void *vpDmaTag_Call(void *packet, void *addr);
extern u32 *vpDmaTag_Next(void *packet, u32 addr);
extern void NuRndrStreamLink(struct nurndrstream_s *stream);
extern int NuRndrBeginSceneEx(int target, int zbuffer, int halfoffset);
extern void NuRndrEndSceneEx(int flags);
extern void NuRndrRectUV3di(int x, int y, int width, int height, float u0, float v0, float u1, float v1,
                            int colour, int z);
extern void NuRndrFxCode(int code, void *fx, void *dma, struct numtl_s *mtl);
extern void *NuScratchAlloc32(u32 size);
extern void NuScratchRelease(void);
extern void NuVec4MtxTransformVU0(struct nuvec4_s *dst, struct nuvec4_s *src, struct numtx_s *m);
extern void NuVec4ScaleXYZVU0(struct nuvec4_s *dst, struct nuvec4_s *src, f32 scale);
extern void NuVecConvertToIntVU0(void *dst, struct nuvec_s *src);
extern f32 NuFsqrt(f32 x);
extern f32 NuTrigTable[];

extern int D_00633080;
extern int D_00633084;
extern int D_00633088;
extern int D_006AD5E0[];
extern struct nuvec_s D_006AD5F0;
extern u32 D_006AD600[];
extern int NuLightOldSpotCnt;
extern int NuLgtArcLaserOldCnt;
extern void *NuLgtFxSetCall;
extern void *NuLgtFxFogCop;
extern void *NuLgtFxFogClr;
extern struct numtl_s *NuLgtBackBuff;
extern float Hazeampx;
extern float Hazeampy;
extern int nurndr_pixel_width;
extern int nurndr_pixel_height;

#define nulights D_006AD100
#define nulight_dirs D_006AD580
#define nulight_colours D_006AD5A8
#define nulight_current D_006AD5D0
#define nulight_ambient D_006AD5F0
#define nulgt_packet_buffer D_006AD600
#define nulight_stamp D_00633088
#define nulight_stamps D_006AD5E0
#define fx_ang D_00633080
#define fx_ang2 D_00633084

extern int NuLgtHFogMat[3];
extern struct numtl_s *NuLgtFogMat[3];
extern int NuLgtHLightMat[24];
extern struct numtl_s *NuLgtLightMat[24];
extern struct numtl_s *NuLgtGreyMat;
extern struct numtl_s *NuLgtGreyNZMat;
extern struct numtl_s *NuLightZBufMat;
extern struct numtl_s *NuLightAddMat;
extern struct numtl_s *NuLightAddMat2;
extern struct numtl_s *NuLgtBackAlphaBuff;

/* Retail passes the palette in $a1 although NuTexPalChange takes only the
   texture. */
#define NuTexPalChangePal(tex, pal) ((void (*)(int, int *))NuTexPalChange)(tex, pal)


void NuLightMatInit(void) {
    struct nutex_s tex;
    int lp;
    int pal[256];
    struct numtl_s *mtl;

    tex.type = 0x12;
    tex.width = PS2_REZ_W;
    tex.height = PS2_REZ_H;
    NuLgtHSurf[1] = NuTexCreate(&tex);
    NuLgtSurf[1] = NuMtlCreate(1);
    NuLgtSurf[1]->diffuse.r = 1.0f;
    NuLgtSurf[1]->diffuse.g = 1.0f;
    NuLgtSurf[1]->diffuse.b = 1.0f;
    NuLgtSurf[1]->attrib.cull = 2;
    NuLgtSurf[1]->attrib.zmode = 3;
    NuLgtSurf[1]->attrib.filter = 0;
    NuLgtSurf[1]->attrib.lighting = 2;
    NuLgtSurf[1]->attrib.colour = 1;
    NuLgtSurf[1]->attrib.utc = 1;
    NuLgtSurf[1]->attrib.vtc = 1;
    NuLgtSurf[1]->alpha = 0.999f;
    NuLgtSurf[1]->attrib.alpha = 0;
    NuLgtSurf[1]->attrib2.afail = 1;
    NuLgtSurf[1]->tid = NuLgtHSurf[1];
    NuLgtSurf[1]->alpha_sort = 0x80;
    NuMtlUpdate(NuLgtSurf[1]);

    tex.width = PS2_REZ_W;
    tex.height = PS2_REZ_H;
    tex.type = 0xF;
    NuLgtHSurf[2] = NuTexCreate(&tex);
    NuLgtSurf[2] = NuMtlCreate(1);
    NuLgtSurf[2]->diffuse.r = 1.0f;
    NuLgtSurf[2]->diffuse.g = 1.0f;
    NuLgtSurf[2]->diffuse.b = 1.0f;
    NuLgtSurf[2]->attrib.cull = 2;
    NuLgtSurf[2]->attrib.zmode = 3;
    NuLgtSurf[2]->attrib.filter = 1;
    NuLgtSurf[2]->attrib.lighting = 2;
    NuLgtSurf[2]->attrib.colour = 1;
    NuLgtSurf[2]->attrib.utc = 1;
    NuLgtSurf[2]->attrib.vtc = 1;
    NuLgtSurf[2]->alpha = 0.999f;
    NuLgtSurf[2]->attrib.alpha = 3;
    NuLgtSurf[2]->attrib2.afail = 1;
    NuLgtSurf[2]->tid = NuLgtHSurf[2];
    NuLgtSurf[2]->alpha_sort = 0x80;
    NuMtlUpdate(NuLgtSurf[2]);

    tex.type = 0x11;
    tex.width = PS2_REZ_W;
    tex.height = PS2_REZ_H;
    NuLgtHSurf[3] = NuTexCreate(&tex);
    NuLgtSurf[3] = NuMtlCreate(1);
    NuLgtSurf[3]->diffuse.r = 1.0f;
    NuLgtSurf[3]->diffuse.g = 1.0f;
    NuLgtSurf[3]->diffuse.b = 1.0f;
    NuLgtSurf[3]->attrib.cull = 2;
    NuLgtSurf[3]->attrib.zmode = 3;
    NuLgtSurf[3]->attrib.filter = 0;
    NuLgtSurf[3]->attrib.lighting = 2;
    NuLgtSurf[3]->attrib.colour = 1;
    NuLgtSurf[3]->attrib.utc = 1;
    NuLgtSurf[3]->attrib.vtc = 1;
    NuLgtSurf[3]->alpha = 0.999f;
    NuLgtSurf[3]->attrib.alpha = 0;
    NuLgtSurf[3]->attrib2.afail = 1;
    NuLgtSurf[3]->tid = NuLgtHSurf[3];
    NuLgtSurf[3]->alpha_sort = 0x80;
    NuMtlUpdate(NuLgtSurf[3]);

    tex.type = 0xE;
    tex.width = PS2_REZ_W;
    tex.height = PS2_REZ_H;
    NuLgtHSurf[4] = NuTexCreate(&tex);
    NuLgtSurf[4] = NuMtlCreate(1);
    NuLgtSurf[4]->diffuse.r = 1.0f;
    NuLgtSurf[4]->diffuse.g = 1.0f;
    NuLgtSurf[4]->diffuse.b = 1.0f;
    NuLgtSurf[4]->attrib.cull = 2;
    NuLgtSurf[4]->attrib.zmode = 3;
    NuLgtSurf[4]->attrib.filter = 0;
    NuLgtSurf[4]->attrib.lighting = 2;
    NuLgtSurf[4]->attrib.colour = 1;
    NuLgtSurf[4]->attrib.utc = 1;
    NuLgtSurf[4]->attrib.vtc = 1;
    NuLgtSurf[4]->alpha = 0.999f;
    NuLgtSurf[4]->attrib.alpha = 0;
    NuLgtSurf[4]->attrib2.afail = 1;
    NuLgtSurf[4]->tid = NuLgtHSurf[4];
    NuLgtSurf[4]->alpha_sort = 0x80;
    NuMtlUpdate(NuLgtSurf[4]);

    tex.type = 0xF;
    tex.width = PS2_REZ_W;
    tex.height = PS2_REZ_H;
    NuLgtHSurf[5] = NuTexCreate(&tex);
    NuLgtSurf[5] = NuMtlCreate(1);
    NuLgtSurf[5]->diffuse.r = 1.0f;
    NuLgtSurf[5]->diffuse.g = 1.0f;
    NuLgtSurf[5]->diffuse.b = 1.0f;
    NuLgtSurf[5]->attrib.cull = 2;
    NuLgtSurf[5]->attrib.zmode = 3;
    NuLgtSurf[5]->attrib.filter = 0;
    NuLgtSurf[5]->attrib.lighting = 2;
    NuLgtSurf[5]->attrib.colour = 1;
    NuLgtSurf[5]->attrib.utc = 1;
    NuLgtSurf[5]->attrib.vtc = 1;
    NuLgtSurf[5]->alpha = 0.999f;
    NuLgtSurf[5]->attrib.alpha = 0;
    NuLgtSurf[5]->attrib2.afail = 1;
    NuLgtSurf[5]->tid = NuLgtHSurf[5];
    NuLgtSurf[5]->alpha_sort = 0x80;
    NuMtlUpdate(NuLgtSurf[5]);

    tex.type = 0xA;
    tex.width = PS2_REZ_W;
    tex.height = PS2_REZ_H;
    NuLgtHSurf[6] = NuTexCreate(&tex);
    NuLgtSurf[6] = NuMtlCreate(1);
    NuLgtSurf[6]->diffuse.r = 1.0f;
    NuLgtSurf[6]->diffuse.g = 1.0f;
    NuLgtSurf[6]->diffuse.b = 1.0f;
    NuLgtSurf[6]->attrib.cull = 2;
    NuLgtSurf[6]->attrib.zmode = 1;
    NuLgtSurf[6]->attrib.filter = 0;
    NuLgtSurf[6]->attrib.lighting = 2;
    NuLgtSurf[6]->attrib.colour = 1;
    NuLgtSurf[6]->attrib.utc = 1;
    NuLgtSurf[6]->attrib.vtc = 1;
    NuLgtSurf[6]->alpha = 0.999f;
    NuLgtSurf[6]->attrib.alpha = 2;
    NuLgtSurf[6]->attrib2.afail = 1;
    NuLgtSurf[6]->tid = NuLgtHSurf[6];
    NuLgtSurf[6]->alpha_sort = 0x80;
    NuMtlUpdate(NuLgtSurf[6]);
    for (lp = 0; lp < 256; lp++) {
        pal[lp] = lp + (lp << 8) + (lp << 16) + (lp << 24);
    }
    NuTexPalChangePal(NuLgtSurf[6]->tid, pal);

    tex.type = 0xA;
    tex.width = PS2_REZ_W;
    tex.height = PS2_REZ_H;
    NuLgtHSurf[16] = NuTexCreate(&tex);
    NuLgtSurf[16] = NuMtlCreate(1);
    NuLgtSurf[16]->diffuse.r = 1.0f;
    NuLgtSurf[16]->diffuse.g = 1.0f;
    NuLgtSurf[16]->diffuse.b = 1.0f;
    NuLgtSurf[16]->attrib.cull = 2;
    NuLgtSurf[16]->attrib.zmode = 1;
    NuLgtSurf[16]->attrib.filter = 0;
    NuLgtSurf[16]->attrib.lighting = 2;
    NuLgtSurf[16]->attrib.colour = 1;
    NuLgtSurf[16]->attrib.utc = 1;
    NuLgtSurf[16]->attrib.vtc = 1;
    NuLgtSurf[16]->alpha = 0.999f;
    NuLgtSurf[16]->attrib.alpha = 2;
    NuLgtSurf[16]->attrib2.afail = 1;
    NuLgtSurf[16]->tid = NuLgtHSurf[16];
    NuLgtSurf[16]->alpha_sort = 0x80;
    NuMtlUpdate(NuLgtSurf[16]);
    for (lp = 0; lp < 256; lp++) {
        pal[lp] = lp + (lp << 8) + (lp << 16) + (lp << 24);
    }
    NuTexPalChangePal(NuLgtSurf[16]->tid, pal);

    tex.type = 0xE;
    tex.width = PS2_REZ_W;
    tex.height = PS2_REZ_H;
    NuLgtHSurf[7] = NuTexCreate(&tex);
    NuLgtSurf[7] = NuMtlCreate(1);
    NuLgtSurf[7]->diffuse.r = 1.0f;
    NuLgtSurf[7]->diffuse.g = 1.0f;
    NuLgtSurf[7]->diffuse.b = 1.0f;
    NuLgtSurf[7]->attrib.cull = 2;
    NuLgtSurf[7]->attrib.zmode = 3;
    NuLgtSurf[7]->attrib.filter = 0;
    NuLgtSurf[7]->attrib.lighting = 2;
    NuLgtSurf[7]->attrib.colour = 1;
    NuLgtSurf[7]->attrib.utc = 1;
    NuLgtSurf[7]->attrib.vtc = 1;
    NuLgtSurf[7]->alpha = 0.999f;
    NuLgtSurf[7]->attrib.alpha = 0;
    NuLgtSurf[7]->attrib2.afail = 1;
    NuLgtSurf[7]->tid = NuLgtHSurf[7];
    NuLgtSurf[7]->alpha_sort = 0x80;
    NuMtlUpdate(NuLgtSurf[7]);

    tex.width = PS2_REZ_W;
    tex.height = PS2_REZ_H;
    tex.type = 0xE;
    NuLgtHSurf[8] = NuTexCreate(&tex);
    NuLgtSurf[8] = NuMtlCreate(1);
    NuLgtSurf[8]->diffuse.r = 1.0f;
    NuLgtSurf[8]->diffuse.g = 1.0f;
    NuLgtSurf[8]->diffuse.b = 1.0f;
    NuLgtSurf[8]->attrib.cull = 2;
    NuLgtSurf[8]->attrib.zmode = 3;
    NuLgtSurf[8]->attrib.filter = 0;
    NuLgtSurf[8]->attrib.lighting = 2;
    NuLgtSurf[8]->attrib.colour = 1;
    NuLgtSurf[8]->attrib.utc = 1;
    NuLgtSurf[8]->attrib.vtc = 1;
    NuLgtSurf[8]->alpha = 0.999f;
    NuLgtSurf[8]->attrib.alpha = 2;
    NuLgtSurf[8]->attrib2.afail = 1;
    NuLgtSurf[8]->tid = NuLgtHSurf[8];
    NuLgtSurf[8]->alpha_sort = 0x80;
    NuMtlUpdate(NuLgtSurf[8]);

    tex.type = 0xF;
    tex.width = PS2_REZ_W;
    tex.height = PS2_REZ_H;
    NuLgtHSurf[17] = NuTexCreate(&tex);
    NuLgtSurf[17] = NuMtlCreate(1);
    NuLgtSurf[17]->diffuse.r = 1.0f;
    NuLgtSurf[17]->diffuse.g = 1.0f;
    NuLgtSurf[17]->diffuse.b = 1.0f;
    NuLgtSurf[17]->attrib.cull = 2;
    NuLgtSurf[17]->attrib.zmode = 3;
    NuLgtSurf[17]->attrib.filter = 0;
    NuLgtSurf[17]->attrib.lighting = 2;
    NuLgtSurf[17]->attrib.colour = 1;
    NuLgtSurf[17]->attrib.utc = 1;
    NuLgtSurf[17]->attrib.vtc = 1;
    NuLgtSurf[17]->alpha = 0.999f;
    NuLgtSurf[17]->attrib.alpha = 2;
    NuLgtSurf[17]->attrib2.afail = 1;
    NuLgtSurf[17]->tid = NuLgtHSurf[17];
    NuLgtSurf[17]->alpha_sort = 0x80;
    NuMtlUpdate(NuLgtSurf[17]);

    tex.width = PS2_REZ_W;
    tex.type = 0xE;
    tex.height = PS2_REZ_H;
    NuLgtHSurf[9] = NuTexCreate(&tex);
    NuLgtSurf[9] = NuMtlCreate(1);
    NuLgtSurf[9]->diffuse.r = 1.0f;
    NuLgtSurf[9]->diffuse.g = 1.0f;
    NuLgtSurf[9]->diffuse.b = 1.0f;
    NuLgtSurf[9]->attrib.cull = 2;
    NuLgtSurf[9]->attrib.zmode = 1;
    NuLgtSurf[9]->attrib.filter = 0;
    NuLgtSurf[9]->attrib.lighting = 2;
    NuLgtSurf[9]->attrib.colour = 1;
    NuLgtSurf[9]->attrib.utc = 1;
    NuLgtSurf[9]->attrib.vtc = 1;
    NuLgtSurf[9]->alpha = 0.999f;
    NuLgtSurf[9]->attrib.alpha = 2;
    NuLgtSurf[9]->attrib2.afail = 1;
    NuLgtSurf[9]->tid = NuLgtHSurf[9];
    NuLgtSurf[9]->alpha_sort = 0x80;
    NuMtlUpdate(NuLgtSurf[9]);

    NuLgtSurf[10] = NuMtlCreate(1);
    NuLgtSurf[10]->attrib.zmode = 3;
    NuLgtSurf[10]->attrib.filter = 0;
    NuLgtSurf[10]->attrib.lighting = 2;
    NuLgtSurf[10]->attrib.utc = 1;
    NuLgtSurf[10]->attrib.vtc = 1;
    NuLgtSurf[10]->attrib.colour = 1;
    NuLgtSurf[10]->attrib.utc = 1;
    NuLgtSurf[10]->attrib.vtc = 1;
    NuLgtSurf[10]->attrib.alpha = 0;
    NuLgtSurf[10]->attrib2.afail = 1;
    NuMtlUpdate(NuLgtSurf[10]);

    tex.width = PS2_REZ_W;
    tex.height = PS2_REZ_H;
    tex.type = 0xE;
    NuLgtHSurf[11] = NuTexCreate(&tex);
    NuLgtSurf[11] = NuMtlCreate(1);
    NuLgtSurf[11]->diffuse.r = 1.0f;
    NuLgtSurf[11]->diffuse.g = 1.0f;
    NuLgtSurf[11]->diffuse.b = 1.0f;
    NuLgtSurf[11]->attrib.cull = 2;
    NuLgtSurf[11]->attrib.zmode = 1;
    NuLgtSurf[11]->attrib.filter = 0;
    NuLgtSurf[11]->attrib.lighting = 2;
    NuLgtSurf[11]->attrib.colour = 1;
    NuLgtSurf[11]->attrib.utc = 1;
    NuLgtSurf[11]->attrib.vtc = 1;
    NuLgtSurf[11]->alpha = 0.999f;
    NuLgtSurf[11]->attrib.alpha = 0;
    NuLgtSurf[11]->attrib2.afail = 1;
    NuLgtSurf[11]->tid = NuLgtHSurf[11];
    NuLgtSurf[11]->alpha_sort = 0x80;
    NuMtlUpdate(NuLgtSurf[11]);

    tex.height = PS2_REZ_H << 2;
    tex.type = 9;
    tex.width = PS2_REZ_W;
    NuLgtHSurf[13] = NuTexCreate(&tex);
    NuLgtSurf[13] = NuMtlCreate(1);
    NuLgtSurf[13]->diffuse.r = 1.0f;
    NuLgtSurf[13]->diffuse.g = 1.0f;
    NuLgtSurf[13]->diffuse.b = 1.0f;
    NuLgtSurf[13]->attrib.cull = 2;
    NuLgtSurf[13]->attrib.zmode = 3;
    NuLgtSurf[13]->attrib.filter = 0;
    NuLgtSurf[13]->attrib.lighting = 2;
    NuLgtSurf[13]->attrib.colour = 1;
    NuLgtSurf[13]->attrib.utc = 1;
    NuLgtSurf[13]->attrib.vtc = 1;
    NuLgtSurf[13]->alpha = 0.999f;
    NuLgtSurf[13]->attrib.alpha = 2;
    NuLgtSurf[13]->attrib2.afail = 1;
    NuLgtSurf[13]->tid = NuLgtHSurf[13];
    NuLgtSurf[13]->alpha_sort = 0x80;
    NuMtlUpdate(NuLgtSurf[13]);
    for (lp = 0; lp < 256; lp++) {
        pal[lp] = lp + (lp << 8) + (lp << 16) + (lp << 24);
    }
    NuTexPalChangePal(NuLgtSurf[13]->tid, pal);

    mtl = NuMtlCreate(1);
    NuLightZMat = mtl;
    mtl->attrib2.afail = 1;
    mtl->tid = -2;
    mtl->attrib.cull = 2;
    mtl->attrib.zmode = 3;
    mtl->attrib.filter = 0;
    mtl->attrib.lighting = 2;
    mtl->attrib.colour = 1;
    mtl->attrib.utc = 1;
    mtl->attrib.vtc = 1;
    mtl->attrib.alpha = 0;
    mtl->alpha_sort = 0x80;
    mtl->diffuse.r = 1.0f;
    mtl->diffuse.g = 1.0f;
    mtl->diffuse.b = 1.0f;
    mtl->alpha = 0.999f;
    NuMtlUpdate(mtl);

    tex.width = PS2_REZ_W;
    tex.height = PS2_REZ_H << 2;
    NuLgtSurf[13]->attrib.utc = 1;
    NuLgtSurf[13]->attrib.vtc = 1;
    tex.type = 9;
    NuLgtHSurf[14] = NuTexCreate(&tex);
    NuLgtSurf[14] = NuMtlCreate(1);
    NuLgtSurf[14]->diffuse.r = 1.0f;
    NuLgtSurf[14]->diffuse.g = 1.0f;
    NuLgtSurf[14]->diffuse.b = 1.0f;
    NuLgtSurf[14]->attrib.cull = 2;
    NuLgtSurf[14]->attrib.zmode = 3;
    NuLgtSurf[14]->attrib.filter = 0;
    NuLgtSurf[14]->attrib.lighting = 2;
    NuLgtSurf[14]->attrib.colour = 1;
    NuLgtSurf[14]->attrib.utc = 1;
    NuLgtSurf[14]->attrib.vtc = 1;
    NuLgtSurf[14]->alpha = 0.999f;
    NuLgtSurf[14]->attrib.alpha = 2;
    NuLgtSurf[14]->attrib2.afail = 1;
    NuLgtSurf[14]->tid = NuLgtHSurf[14];
    NuLgtSurf[14]->alpha_sort = 0x80;
    NuMtlUpdate(NuLgtSurf[14]);
    for (lp = 0; lp < 256; lp++) {
        pal[lp] = lp + (lp << 8) + (lp << 16) + 0x80000000;
    }
    NuTexPalChangePal(NuLgtSurf[14]->tid, pal);

    tex.type = 0xE;
    tex.height = PS2_REZ_H;
    tex.width = PS2_REZ_W;
    NuLgtHSurf[12] = NuTexCreate(&tex);
    NuLgtSurf[12] = NuMtlCreate(1);
    NuLgtSurf[12]->diffuse.r = 1.0f;
    NuLgtSurf[12]->diffuse.g = 1.0f;
    NuLgtSurf[12]->diffuse.b = 1.0f;
    NuLgtSurf[12]->attrib.cull = 2;
    NuLgtSurf[12]->attrib.zmode = 1;
    NuLgtSurf[12]->attrib.filter = 0;
    NuLgtSurf[12]->attrib.lighting = 2;
    NuLgtSurf[12]->attrib.colour = 1;
    NuLgtSurf[12]->attrib.utc = 1;
    NuLgtSurf[12]->attrib.vtc = 1;
    NuLgtSurf[12]->alpha = 0.999f;
    NuLgtSurf[12]->attrib.alpha = 1;
    NuLgtSurf[12]->attrib2.afail = 1;
    NuLgtSurf[12]->tid = NuLgtHSurf[12];
    NuLgtSurf[12]->alpha_sort = 0x80;
    NuMtlUpdate(NuLgtSurf[12]);

    tex.type = 0xE;
    tex.width = PS2_REZ_W;
    tex.height = PS2_REZ_H;
    NuLgtHSurf[15] = NuTexCreate(&tex);
    NuLgtSurf[15] = NuMtlCreate(1);
    NuLgtSurf[15]->diffuse.r = 1.0f;
    NuLgtSurf[15]->diffuse.g = 1.0f;
    NuLgtSurf[15]->diffuse.b = 1.0f;
    NuLgtSurf[15]->attrib.cull = 2;
    NuLgtSurf[15]->attrib.zmode = 1;
    NuLgtSurf[15]->attrib.filter = 0;
    NuLgtSurf[15]->attrib.lighting = 2;
    NuLgtSurf[15]->attrib.colour = 1;
    NuLgtSurf[15]->attrib.utc = 1;
    NuLgtSurf[15]->attrib.vtc = 1;
    NuLgtSurf[15]->alpha = 0.999f;
    NuLgtSurf[15]->attrib.alpha = 0;
    NuLgtSurf[15]->attrib2.afail = 1;
    NuLgtSurf[15]->tid = NuLgtHSurf[15];
    NuLgtSurf[15]->alpha_sort = 0x80;
    NuMtlUpdate(NuLgtSurf[15]);
    for (lp = 0; lp < 256; lp++) {
        pal[lp] = lp + (lp << 8) + (lp << 16) + (lp << 24);
    }

    tex.height = PS2_REZ_H << 2;
    tex.type = 9;
    tex.width = PS2_REZ_W;
    NuLgtHFogMat[1] = NuTexCreate(&tex);
    NuLgtFogMat[1] = NuMtlCreate(1);
    NuLgtFogMat[1]->diffuse.r = 1.0f;
    NuLgtFogMat[1]->diffuse.g = 1.0f;
    NuLgtFogMat[1]->diffuse.b = 1.0f;
    NuLgtFogMat[1]->attrib.cull = 2;
    NuLgtFogMat[1]->attrib.zmode = 3;
    NuLgtFogMat[1]->attrib.filter = 0;
    NuLgtFogMat[1]->attrib.lighting = 2;
    NuLgtFogMat[1]->attrib.colour = 1;
    NuLgtFogMat[1]->attrib.utc = 1;
    NuLgtFogMat[1]->attrib.vtc = 1;
    NuLgtFogMat[1]->alpha = 0.999f;
    NuLgtFogMat[1]->attrib.alpha = 0;
    NuLgtFogMat[1]->attrib2.afail = 1;
    NuLgtFogMat[1]->tid = NuLgtHFogMat[1];
    NuLgtFogMat[1]->alpha_sort = 0x80;
    NuMtlUpdate(NuLgtFogMat[1]);

    tex.height = PS2_REZ_H << 2;
    tex.type = 9;
    tex.width = PS2_REZ_W;
    NuLgtHFogMat[0] = NuTexCreate(&tex);
    NuLgtFogMat[0] = NuMtlCreate(1);
    NuLgtFogMat[0]->diffuse.r = 1.0f;
    NuLgtFogMat[0]->diffuse.g = 1.0f;
    NuLgtFogMat[0]->diffuse.b = 1.0f;
    NuLgtFogMat[0]->attrib.cull = 2;
    NuLgtFogMat[0]->attrib.zmode = 3;
    NuLgtFogMat[0]->attrib.filter = 0;
    NuLgtFogMat[0]->attrib.lighting = 2;
    NuLgtFogMat[0]->attrib.colour = 1;
    NuLgtFogMat[0]->attrib.utc = 1;
    NuLgtFogMat[0]->attrib.vtc = 1;
    NuLgtFogMat[0]->alpha = 0.999f;
    NuLgtFogMat[0]->attrib.alpha = 2;
    NuLgtFogMat[0]->attrib2.afail = 1;
    NuLgtFogMat[0]->tid = NuLgtHFogMat[0];
    NuLgtFogMat[0]->alpha_sort = 0x80;
    NuMtlUpdate(NuLgtFogMat[0]);

    tex.height = PS2_REZ_H << 2;
    tex.type = 9;
    tex.width = PS2_REZ_W;
    NuLgtHFogMat[2] = NuTexCreate(&tex);
    NuLgtFogMat[2] = NuMtlCreate(1);
    NuLgtFogMat[2]->diffuse.r = 1.0f;
    NuLgtFogMat[2]->diffuse.g = 1.0f;
    NuLgtFogMat[2]->diffuse.b = 1.0f;
    NuLgtFogMat[2]->attrib.cull = 2;
    NuLgtFogMat[2]->attrib.zmode = 3;
    NuLgtFogMat[2]->attrib.filter = 0;
    NuLgtFogMat[2]->attrib.lighting = 2;
    NuLgtFogMat[2]->attrib.colour = 1;
    NuLgtFogMat[2]->attrib.utc = 1;
    NuLgtFogMat[2]->attrib.vtc = 1;
    NuLgtFogMat[2]->alpha = 0.999f;
    NuLgtFogMat[2]->attrib.alpha = 2;
    NuLgtFogMat[2]->attrib2.afail = 1;
    NuLgtFogMat[2]->tid = NuLgtHFogMat[2];
    NuLgtFogMat[2]->alpha_sort = 0x80;
    NuMtlUpdate(NuLgtFogMat[2]);

    for (lp = 0; lp < 8; lp++) {
        tex.height = PS2_REZ_H << 2;
        tex.type = 9;
        tex.width = PS2_REZ_W;
        NuLgtHLightMat[lp] = NuTexCreate(&tex);
        NuLgtLightMat[lp] = NuMtlCreate(1);
        NuLgtLightMat[lp]->diffuse.r = 1.0f;
        NuLgtLightMat[lp]->diffuse.g = 1.0f;
        NuLgtLightMat[lp]->diffuse.b = 1.0f;
        NuLgtLightMat[lp]->attrib.cull = 2;
        NuLgtLightMat[lp]->attrib.zmode = 3;
        NuLgtLightMat[lp]->attrib.filter = 0;
        NuLgtLightMat[lp]->attrib.lighting = 2;
        NuLgtLightMat[lp]->attrib.colour = 1;
        NuLgtLightMat[lp]->attrib.utc = 1;
        NuLgtLightMat[lp]->attrib.vtc = 1;
        NuLgtLightMat[lp]->alpha = 0.999f;
        NuLgtLightMat[lp]->attrib.alpha = 2;
        NuLgtLightMat[lp]->attrib2.afail = 1;
        NuLgtLightMat[lp]->tid = NuLgtHLightMat[lp];
        NuLgtLightMat[lp]->alpha_sort = 0x80;
        NuMtlUpdate(NuLgtLightMat[lp]);
    }
    for (lp = 8; lp < 16; lp++) {
        tex.height = PS2_REZ_H << 2;
        tex.type = 9;
        tex.width = PS2_REZ_W;
        NuLgtHLightMat[lp] = NuTexCreate(&tex);
        NuLgtLightMat[lp] = NuMtlCreate(1);
        NuLgtLightMat[lp]->diffuse.r = 1.0f;
        NuLgtLightMat[lp]->diffuse.g = 1.0f;
        NuLgtLightMat[lp]->diffuse.b = 1.0f;
        NuLgtLightMat[lp]->attrib.cull = 2;
        NuLgtLightMat[lp]->attrib.zmode = 3;
        NuLgtLightMat[lp]->attrib.filter = 0;
        NuLgtLightMat[lp]->attrib.lighting = 2;
        NuLgtLightMat[lp]->attrib.colour = 1;
        NuLgtLightMat[lp]->attrib.utc = 1;
        NuLgtLightMat[lp]->attrib.vtc = 1;
        NuLgtLightMat[lp]->alpha = 0.999f;
        NuLgtLightMat[lp]->attrib.alpha = 0;
        NuLgtLightMat[lp]->attrib2.afail = 1;
        NuLgtLightMat[lp]->tid = NuLgtHLightMat[lp];
        NuLgtLightMat[lp]->alpha_sort = 0x80;
        NuMtlUpdate(NuLgtLightMat[lp]);
    }
    for (lp = 16; lp < 24; lp++) {
        tex.height = PS2_REZ_H << 2;
        tex.type = 9;
        tex.width = PS2_REZ_W;
        NuLgtHLightMat[lp] = NuTexCreate(&tex);
        NuLgtLightMat[lp] = NuMtlCreate(1);
        NuLgtLightMat[lp]->diffuse.r = 1.0f;
        NuLgtLightMat[lp]->diffuse.g = 1.0f;
        NuLgtLightMat[lp]->diffuse.b = 1.0f;
        NuLgtLightMat[lp]->attrib.cull = 2;
        NuLgtLightMat[lp]->attrib.zmode = 3;
        NuLgtLightMat[lp]->attrib.filter = 0;
        NuLgtLightMat[lp]->attrib.lighting = 2;
        NuLgtLightMat[lp]->attrib.colour = 1;
        NuLgtLightMat[lp]->attrib.utc = 1;
        NuLgtLightMat[lp]->attrib.vtc = 1;
        NuLgtLightMat[lp]->alpha = 0.999f;
        NuLgtLightMat[lp]->attrib.alpha = 2;
        NuLgtLightMat[lp]->attrib2.afail = 1;
        NuLgtLightMat[lp]->tid = NuLgtHLightMat[lp];
        NuLgtLightMat[lp]->alpha_sort = 0x80;
        NuMtlUpdate(NuLgtLightMat[lp]);
    }

    mtl = NuMtlCreate(1);
    NuLgtGreyMat = mtl;
    mtl->attrib2.afail = 1;
    mtl->attrib.zmode = 1;
    mtl->attrib.filter = 0;
    mtl->attrib.lighting = 2;
    mtl->attrib.utc = 1;
    mtl->attrib.vtc = 1;
    mtl->attrib.colour = 1;
    mtl->attrib.utc = 1;
    mtl->attrib.vtc = 1;
    mtl->attrib.alpha = 0;
    NuMtlUpdate(mtl);

    mtl = NuMtlCreate(1);
    NuLgtGreyNZMat = mtl;
    mtl->attrib2.afail = 1;
    mtl->attrib.zmode = 3;
    mtl->attrib.filter = 0;
    mtl->attrib.lighting = 2;
    mtl->attrib.utc = 1;
    mtl->attrib.vtc = 1;
    mtl->attrib.colour = 1;
    mtl->attrib.utc = 1;
    mtl->attrib.vtc = 1;
    mtl->attrib.alpha = 0;
    NuMtlUpdate(mtl);

    mtl = NuMtlCreate(1);
    NuLightZBufMat = mtl;
    mtl->attrib2.afail = 1;
    mtl->tid = -2;
    mtl->attrib.cull = 2;
    mtl->attrib.zmode = 3;
    mtl->diffuse.r = 1.0f;
    mtl->diffuse.g = 1.0f;
    mtl->attrib.filter = 0;
    mtl->attrib.lighting = 2;
    mtl->attrib.colour = 1;
    mtl->attrib.alpha = 0;
    mtl->diffuse.b = 1.0f;
    mtl->alpha = 0.999f;
    mtl->alpha_sort = 0x80;
    NuMtlUpdate(mtl);

    mtl = NuMtlCreate(1);
    NuLightAddMat = mtl;
    mtl->attrib2.afail = 1;
    mtl->attrib.cull = 2;
    mtl->attrib.zmode = 1;
    mtl->attrib.alpha = 2;
    mtl->attrib.utc = 1;
    mtl->attrib.vtc = 1;
    mtl->attrib.lighting = 2;
    mtl->diffuse.r = 1.0f;
    mtl->diffuse.g = 1.0f;
    mtl->diffuse.b = 1.0f;
    mtl->alpha = 0.999f;
    mtl->alpha_sort = 0x100;
    mtl->tid = 0;
    NuMtlUpdate(mtl);

    mtl = NuMtlCreate(1);
    NuLightAddMat2 = mtl;
    mtl->attrib2.afail = 1;
    mtl->attrib2.gouraud = 0;
    mtl->attrib.cull = 2;
    mtl->attrib.zmode = 1;
    mtl->attrib.alpha = 2;
    mtl->attrib.utc = 1;
    mtl->attrib.vtc = 1;
    mtl->attrib.lighting = 2;
    mtl->alpha_sort = 0x100;
    mtl->diffuse.r = 1.0f;
    mtl->diffuse.g = 1.0f;
    mtl->diffuse.b = 1.0f;
    mtl->alpha = 0.999f;
    mtl->tid = 0;
    NuMtlUpdate(mtl);

    mtl = NuMtlCreate(1);
    NuLgtBackBuff = mtl;
    mtl->attrib2.afail = 1;
    mtl->attrib.cull = 2;
    mtl->attrib.zmode = 3;
    mtl->attrib.filter = 1;
    mtl->attrib.lighting = 2;
    mtl->attrib.colour = 1;
    mtl->attrib.alpha = 0;
    mtl->diffuse.r = 1.0f;
    mtl->diffuse.g = 1.0f;
    mtl->diffuse.b = 1.0f;
    mtl->alpha = 0.999f;
    mtl->tid = -1;
    mtl->alpha_sort = 0x80;
    NuMtlUpdate(mtl);

    mtl = NuMtlCreate(1);
    NuLgtBackAlphaBuff = mtl;
    mtl->attrib2.afail = 1;
    mtl->attrib.cull = 2;
    mtl->attrib.zmode = 1;
    mtl->attrib.filter = 1;
    mtl->attrib.lighting = 2;
    mtl->attrib.colour = 1;
    mtl->attrib.alpha = 1;
    mtl->diffuse.b = 1.0f;
    mtl->alpha = 0.999f;
    mtl->tid = -1;
    mtl->alpha_sort = 0x80;
    mtl->diffuse.r = 1.0f;
    mtl->diffuse.g = 1.0f;
    NuMtlUpdate(mtl);
}


void NuLightInit(void) {
    int i;
    void *packet;

    for (i = 0; i < 3; i++) {
        nulight_colours[i].r = nulight_colours[i].g = nulight_colours[i].b = 0.0f;
        nulight_current[i] = 0;
        nulight_stamps[i] = 0;
    }
    nulight_ambient.x = 0.15f;
    nulight_ambient.y = 0.15f;
    nulight_ambient.z = 0.15f;
    for (i = 0; i < 8; i++) {
        nulights[i].index = i;
        nulights[i].used = 0;
    }

    nulight_stamp = 0;
    NuLightSpotCnt = 0;
    NuLightOldSpotCnt = 0;
    NuLightHazePolyCnt = 0;
    NuLgtArcLaserCnt = 0;
    NuLgtArcLaserOldCnt = 0;
    NuLightHazePolymtl = 0;
    packet = nulgt_packet_buffer;
    NuLgtBuff = packet;
    NuLgtBuff = (u32 *)vpDmaTag_RetEx(packet, 0x10000000, 0x50000000) - 1;
    NuLgtBuff = vtBeginDirect(NuLgtBuff, 0);
    *((u64 *)NuLgtBuff)++ = (u64)(10 | 0x8000) | ((u64)0x64034000 << 32);
    *((u64 *)NuLgtBuff)++ = 0x525210;
    NuLgtFxSetCall = packet;
    for (i = 0; i < 10; i++) {
        int x0 = i * 409.6f;
        int x1 = (i + 1) * 409.6f;
        float u0 = (i * 64.0f + 0.5f) / 1024.0f;
        float u1 = (i * 64.0f + 63.5f) / 1024.0f;
        int height = PS2_REZ_H;
        int x = x0 - 8;
        int width = x1 - x0;
        int colour = 0x80808080;
        float v1 = (float)PS2_REZ_H / NuPower2(PS2_REZ_H);
        int y;

        height = (height << 4) - 8;
        x += (PS2_VCNTR_X - (PS2_REZ_W >> 1)) << 4;
        y = (PS2_VCNTR_Y - (PS2_REZ_H >> 1)) << 4;
        *((u64 *)NuLgtBuff)++ = 6;
        *((int *)NuLgtBuff)++ = colour;
        *((float *)NuLgtBuff)++ = 1.0f;
        *((float *)NuLgtBuff)++ = u0;
        *((float *)NuLgtBuff)++ = 0.0f;
        *((short *)NuLgtBuff)++ = x;
        *((short *)NuLgtBuff)++ = y;
        *((int *)NuLgtBuff)++ = 0;
        *((float *)NuLgtBuff)++ = u1;
        *((float *)NuLgtBuff)++ = v1;
        *((short *)NuLgtBuff)++ = x + width;
        *((short *)NuLgtBuff)++ = y + height;
        *((int *)NuLgtBuff)++ = 0;
    }
    NuLgtBuff = vtCloseDirect(NuLgtBuff);
    NuLgtBuff = vpDmaTag_Close(NuLgtBuff);

    packet = NuLgtBuff;
    NuLgtBuff = (u32 *)vpDmaTag_RetEx(packet, 0x10000000, 0x50000000) - 1;
    NuLgtBuff = vtBeginDirect(NuLgtBuff, 0);
    *((u64 *)NuLgtBuff)++ = (u64)(4 | 0x8000) | ((u64)0x64034000 << 32);
    *((u64 *)NuLgtBuff)++ = 0x525210;
    NuLgtFxFogCop = packet;
    for (i = 0; i < 4; i++) {
        float u0 = i * 64.0f / 1024.0f;
        float u1 = (i * 64.0f + 64.0f) / 1024.0f;
        int left = i * 0x400 + 0x17FC;
        int y = -4;
        int height = PS2_REZ_H;
        int colour = 0x808080;
        int z = 0x1FFFFFF;
        int x;
        float v1 = (float)PS2_REZ_H / NuPower2(PS2_REZ_H);

        height = height << 4;
        x = left + ((PS2_VCNTR_X - (PS2_REZ_W >> 1)) << 4);
        y += (PS2_VCNTR_Y - (PS2_REZ_H >> 1)) << 4;
        *((u64 *)NuLgtBuff)++ = 6;
        *((int *)NuLgtBuff)++ = colour;
        *((float *)NuLgtBuff)++ = 1.0f;
        *((float *)NuLgtBuff)++ = u0;
        *((float *)NuLgtBuff)++ = 0.0f;
        *((short *)NuLgtBuff)++ = x;
        *((short *)NuLgtBuff)++ = y;
        *((int *)NuLgtBuff)++ = z;
        *((float *)NuLgtBuff)++ = u1;
        *((float *)NuLgtBuff)++ = v1;
        *((short *)NuLgtBuff)++ = x + 0x400;
        *((short *)NuLgtBuff)++ = y + height;
        *((int *)NuLgtBuff)++ = z;
    }
    NuLgtBuff = vtCloseDirect(NuLgtBuff);
    NuLgtBuff = vpDmaTag_Close(NuLgtBuff);

    packet = NuLgtBuff;
    NuLgtBuff = (u32 *)vpDmaTag_RetEx(packet, 0x10000000, 0x50000000) - 1;
    NuLgtBuff = vtBeginDirect(NuLgtBuff, 0);
    *((u64 *)NuLgtBuff)++ = (u64)(4 | 0x8000) | ((u64)0x64034000 << 32);
    *((u64 *)NuLgtBuff)++ = 0x525210;
    NuLgtFxFogClr = packet;
    for (i = 0; i < 4; i++) {
        int left = i * 0x400 + 0x17FC;
        int y = -4;
        int height = PS2_REZ_H << 4;
        int colour = 0;
        int z = -1;
        int x;

        x = left + ((PS2_VCNTR_X - (PS2_REZ_W >> 1)) << 4);
        y += (PS2_VCNTR_Y - (PS2_REZ_H >> 1)) << 4;
        *((u64 *)NuLgtBuff)++ = 6;
        *((int *)NuLgtBuff)++ = colour;
        *((float *)NuLgtBuff)++ = 1.0f;
        *((float *)NuLgtBuff)++ = 0.0f;
        *((float *)NuLgtBuff)++ = 0.0f;
        *((short *)NuLgtBuff)++ = x;
        *((short *)NuLgtBuff)++ = y;
        *((int *)NuLgtBuff)++ = z;
        *((float *)NuLgtBuff)++ = 0.0f;
        *((float *)NuLgtBuff)++ = 0.0f;
        *((short *)NuLgtBuff)++ = x + 0x400;
        *((short *)NuLgtBuff)++ = y + height;
        *((int *)NuLgtBuff)++ = z;
    }
    NuLgtBuff = vtCloseDirect(NuLgtBuff);
    NuLgtBuff = vpDmaTag_Close(NuLgtBuff);
}


void NuLigthSetModeMask(int tex, int zpsm, int fbmsk, int zmsk) {
    int zbp;

    rndrstream_3d->cur.w = (u32 *)vpDmaTag_CntEx(rndrstream_3d->cur.w, 0x11000000, 0x50000000) - 1;
    rndrstream_3d->cur.w = vtBeginDirect(rndrstream_3d->cur.w, 0);
    rndrstream_3d->cur.w = gsBeginGifPacket(rndrstream_3d->cur.w, 0, 1, 0, 0, 0, 1, 0xE);
    zbp = NuTexGetTextAddr(tex);
    if (zbp) {
        *((u64 *)rndrstream_3d->cur.w)++ = (zbp >> 5) | (u64)zpsm << 24 | (u64)zmsk << 32;
        *((u64 *)rndrstream_3d->cur.w)++ = 0x4E;
    }
    *((u64 *)rndrstream_3d->cur.w)++ = (int)(cur_rt & 0x1FF)
                                     | (int)((cur_rt >> 16) & 0x3F) << 16
                                     | (int)((cur_rt >> 24) & 0x3F) << 24
                                     | (u64)fbmsk << 32;
    *((u64 *)rndrstream_3d->cur.w)++ = 0x4C;
    rndrstream_3d->cur.w = gsCloseGifPacket(rndrstream_3d->cur.w);
    rndrstream_3d->cur.w = vtCloseDirect(rndrstream_3d->cur.w);
    rndrstream_3d->cur.w = vpDmaTag_Close(rndrstream_3d->cur.w);
}

void NuLgtArcLaserDraw(int paused) {
    struct nuvec4_s *pnts;
    struct numtx_s *vpcsmtx;
    struct nuvec_s norm;
    struct nuvec_s norm2;
    float uv[2][4];
    float step;
    float lp;
    float next;
    float scale;
    float ry;
    float rx;
    float u;
    int id;
    int seed;
    int k;
    u8 rnd;
    u8 *rgba;

    uv[1][0] = NuLgtArcV0;
    uv[1][2] = NuLgtArcV1;
    uv[1][1] = NuLgtArcV0;
    uv[1][3] = NuLgtArcV1;
    if (paused) {
        NuLgtArcLaserCnt = NuLgtArcLaserOldCnt;
    }
    if (!NuLgtArcMtl || !NuLgtArcLaserCnt) {
        NuLgtArcLaserOldCnt = 0;
        return;
    }
    pnts = NuScratchAlloc32(0xC0);
    vpcsmtx = NuCameraGetVPCSMtx();
    if (NuRndrBeginSceneEx(-1, -2, 0)) {
        rndrstream_3d = rndrstream_3d_u;
        rndrstream_3d->mtl = 0;
        rndrstream_3d->mtl_tid = 0;
        NuRndrStreamLink(rndrstream_3d);
        NuRndrStreamSetMisc(rndrstream_3d);
        NuRndrStreamAddMtl(rndrstream_3d, NuLgtArcMtl);
        for (id = 0; id < NuLgtArcLaserCnt; id++) {
            pnts[4].x = NuLgtArcLaserData[id].start.x;
            pnts[4].y = NuLgtArcLaserData[id].start.y;
            pnts[4].z = NuLgtArcLaserData[id].start.z;
            pnts[4].w = 1.0f;
            NuVec4MtxTransformVU0(&pnts[4], &pnts[4], vpcsmtx);
            pnts[5].x = NuLgtArcLaserData[id].end.x;
            pnts[5].y = NuLgtArcLaserData[id].end.y;
            pnts[5].z = NuLgtArcLaserData[id].end.z;
            pnts[5].w = 1.0f;
            NuVec4MtxTransformVU0(&pnts[5], &pnts[5], vpcsmtx);
            if (!(0.5f <= pnts[4].w)) {
                if (!(0.5f <= pnts[5].w)) {
                    continue;
                }
            }
            if (pnts[4].w < 0.5f) {
                pnts[6].x = NuLgtArcLaserData[id].end.x + ((NuLgtArcLaserData[id].start.x - NuLgtArcLaserData[id].end.x) * (pnts[5].w - 0.5f)) / (pnts[5].w - pnts[4].w);
                pnts[6].y = NuLgtArcLaserData[id].end.y + ((NuLgtArcLaserData[id].start.y - NuLgtArcLaserData[id].end.y) * (pnts[5].w - 0.5f)) / (pnts[5].w - pnts[4].w);
                pnts[6].z = NuLgtArcLaserData[id].end.z + ((NuLgtArcLaserData[id].start.z - NuLgtArcLaserData[id].end.z) * (pnts[5].w - 0.5f)) / (pnts[5].w - pnts[4].w);
                pnts[6].w = 1.0f;
            } else {
                pnts[6].x = NuLgtArcLaserData[id].start.x;
                pnts[6].y = NuLgtArcLaserData[id].start.y;
                pnts[6].z = NuLgtArcLaserData[id].start.z;
                pnts[6].w = 1.0f;
            }
            step = sqrt((pnts[6].x - NuLgtArcLaserData[id].end.x) * (pnts[6].x - NuLgtArcLaserData[id].end.x)
                       + (pnts[6].y - NuLgtArcLaserData[id].end.y) * (pnts[6].y - NuLgtArcLaserData[id].end.y)
                       + (pnts[6].z - NuLgtArcLaserData[id].end.z) * (pnts[6].z - NuLgtArcLaserData[id].end.z));
            if (pnts[5].w < 0.5f) {
                pnts[7].x = NuLgtArcLaserData[id].start.x + ((NuLgtArcLaserData[id].end.x - NuLgtArcLaserData[id].start.x) * (pnts[4].w - 0.5f)) / (pnts[4].w - pnts[5].w);
                pnts[7].y = NuLgtArcLaserData[id].start.y + ((NuLgtArcLaserData[id].end.y - NuLgtArcLaserData[id].start.y) * (pnts[4].w - 0.5f)) / (pnts[4].w - pnts[5].w);
                pnts[7].z = NuLgtArcLaserData[id].start.z + ((NuLgtArcLaserData[id].end.z - NuLgtArcLaserData[id].start.z) * (pnts[4].w - 0.5f)) / (pnts[4].w - pnts[5].w);
                pnts[7].w = 1.0f;
            } else {
                pnts[7].x = NuLgtArcLaserData[id].end.x;
                pnts[7].y = NuLgtArcLaserData[id].end.y;
                pnts[7].z = NuLgtArcLaserData[id].end.z;
                pnts[7].w = 1.0f;
            }
            NuVec4MtxTransformVU0(&pnts[4], &pnts[6], vpcsmtx);
            NuVec4ScaleXYZVU0(&pnts[4], &pnts[4], 1.0f / pnts[4].w);
            NuVec4MtxTransformVU0(&pnts[5], &pnts[7], vpcsmtx);
            NuVec4ScaleXYZVU0(&pnts[5], &pnts[5], 1.0f / pnts[5].w);
            norm.x = (pnts[5].y - pnts[4].y) * (640.0f / 224.0f);
            norm.y = pnts[4].x - pnts[5].x;
            norm.z = 0.0f;
            NuVecNorm(&norm, &norm);
            norm.x = norm.x * (NuLgtArcLaserData[id].sizew * (640.0f * 2000.0f / 224.0f));
            norm.y = norm.y * (NuLgtArcLaserData[id].sizew * 2000.0f);
            pnts[7].x = pnts[7].x - pnts[6].x;
            pnts[7].y = pnts[7].y - pnts[6].y;
            pnts[7].z = pnts[7].z - pnts[6].z;
            pnts[7].w = pnts[7].w - pnts[6].w;
            step = sqrt(pnts[7].x * pnts[7].x + pnts[7].y * pnts[7].y + pnts[7].z * pnts[7].z);
            if (0.0f < step) {
                pnts[10].x = NuLgtArcLaserData[id].mid.x;
                pnts[10].y = NuLgtArcLaserData[id].mid.y;
                pnts[10].z = NuLgtArcLaserData[id].mid.z;
                scale = pnts[10].x * pnts[10].x + pnts[10].y * pnts[10].y + pnts[10].z * pnts[10].z;
                if (scale > 0.0f) {
                    scale = NuLgtArcLaserData[id].wobspeed / NuFsqrt(scale);
                }
                pnts[10].x = pnts[10].x * scale;
                pnts[10].y = pnts[10].y * scale;
                pnts[10].z = pnts[10].z * scale;
                NuLgtSeed = NuLgtArcLaserData[id].seed;
                step = NuLgtArcLaserData[id].sizel / step;
                if (step > 1.0f) {
                    step = 1.0f;
                }
                uv[0][2] = NuLgtArcU0;
                uv[0][3] = NuLgtArcU1;
                uv[0][0] = NuLgtArcU0;
                uv[0][1] = NuLgtArcU1;
                pnts[4].x = pnts[6].x;
                pnts[4].y = pnts[6].y;
                pnts[4].z = pnts[6].z;
                pnts[4].w = 1.0f;
                NuVec4MtxTransformVU0(&pnts[4], &pnts[4], vpcsmtx);
                scale = 1.0f / pnts[4].w;
                NuVec4ScaleXYZVU0(&pnts[4], &pnts[4], scale);
                norm2.x = norm.x * scale;
                norm2.y = norm.y * scale;
                pnts[1].x = pnts[4].x - norm2.x;
                pnts[1].y = pnts[4].y - norm2.y;
                pnts[3].x = pnts[4].x + norm2.x;
                pnts[3].y = pnts[4].y + norm2.y;
                pnts[1].z = pnts[4].z;
                pnts[3].z = pnts[4].z;
                for (lp = 0.0f; lp < 1.0f; lp += step) {
                    next = lp + step;
                    if (next >= 1.0f) {
                        pnts[5].x = pnts[6].x + pnts[7].x;
                        pnts[5].y = pnts[6].y + pnts[7].y;
                        pnts[5].z = pnts[6].z + pnts[7].z;
                        u = NuLgtArcU1 + ((NuLgtArcU0 - NuLgtArcU1) * (1.0f - lp)) / step;
                        uv[0][2] = u;
                        uv[0][0] = u;
                    } else {
                        pnts[5].x = pnts[6].x + pnts[7].x * next + pnts[10].x * NuTrigTable[(u16)(next * 32768.0f)];
                        pnts[5].y = pnts[6].y + pnts[7].y * next + pnts[10].y * NuTrigTable[(u16)(next * 32768.0f)];
                        pnts[5].z = pnts[6].z + pnts[7].z * next + pnts[10].z * NuTrigTable[(u16)(next * 32768.0f)];
                    }
                    pnts[5].w = 1.0f;
                    NuVec4MtxTransformVU0(&pnts[5], &pnts[5], vpcsmtx);
                    scale = 1.0f / pnts[5].w;
                    NuVec4ScaleXYZVU0(&pnts[5], &pnts[5], scale);
                    if (lp < 1.0f - step * 1.5f && NuLgtSeed) {
                        seed = NuLgtSeed * 0x24CD + 1;
                        NuLgtSeed = seed & 0xFFFF;
                        rnd = seed;
                        ry = rnd - 0x80;
                        rx = norm.x * ry * NuLgtArcLaserData[id].sizewob * scale;
                        ry = norm.y * ry * NuLgtArcLaserData[id].sizewob * scale;
                    } else {
                        ry = 0.0f;
                        rx = ry;
                    }
                    norm2.x = norm.x * scale;
                    norm2.y = norm.y * scale;
                    pnts[0].x = pnts[5].x - norm2.x + rx;
                    pnts[0].y = pnts[5].y - norm2.y + ry;
                    pnts[2].x = pnts[5].x + norm2.x + rx;
                    pnts[2].y = pnts[5].y + norm2.y + ry;
                    pnts[0].z = pnts[5].z;
                    pnts[2].z = pnts[5].z;
                    if ((!(pnts[0].x < 24576.0f) || !(pnts[1].x < 24576.0f))
                        && (!(pnts[0].x > 40960.0f) || !(pnts[1].x > 40960.0f))
                        && (!(pnts[0].y < 30208.0f) || !(pnts[1].y < 30208.0f))
                        && (!(pnts[0].y > 35328.0f) || !(pnts[1].y > 35328.0f))) {
                        rndrstream_3d->cur.w = vpDmaTag_CntEx(rndrstream_3d->cur.w, 0x10000000, 0x5000000D);
                        *((u64 *)rndrstream_3d->cur.w)++ = 0x3002400000008004;
                        *((u64 *)rndrstream_3d->cur.w)++ = 0x512;
                        rgba = NuLgtArcLaserData[id].rgba;
                        for (k = 0; k < 4; k++) {
                            *rndrstream_3d->cur.f++ = uv[0][k];
                            *rndrstream_3d->cur.f++ = uv[1][k];
                            *rndrstream_3d->cur.f++ = 1.0f;
                            *rndrstream_3d->cur.i++ = 0;
                            *rndrstream_3d->cur.i++ = rgba[0];
                            *rndrstream_3d->cur.i++ = rgba[1];
                            *rndrstream_3d->cur.i++ = rgba[2];
                            *rndrstream_3d->cur.i++ = rgba[3];
                            NuVecConvertToIntVU0(rndrstream_3d->cur.w, (struct nuvec_s *)&pnts[k]);
                            rndrstream_3d->cur.b += 0xC;
                            *rndrstream_3d->cur.i++ = 0;
                        }
                        rndrstream_3d->cur.w = vpDmaTag_Close(rndrstream_3d->cur.w);
                    }
                    pnts[1].x = pnts[0].x;
                    pnts[1].y = pnts[0].y;
                    pnts[3].x = pnts[2].x;
                    pnts[3].y = pnts[2].y;
                    pnts[1].z = pnts[0].z;
                    pnts[3].z = pnts[2].z;
                }
            }
        }
        rndrstream_free = vpDmaTag_Next(rndrstream_3d->cur.w, 0);
        NuRndrEndSceneEx(0);
        NuLgtArcLaserOldCnt = NuLgtArcLaserCnt;
        NuLgtArcLaserCnt = 0;
        if (!paused) {
            NuLgtArcLaserFrame++;
        }
    }
    NuScratchRelease();
}


void NuLightClose(void) {
}


struct nulight_s *NuLightCreate(void) {
    int i;

    for (i = 0; i < 8; i++) {
        if (!nulights[i].used) {
            nulights[i].used = 1;
            return &nulights[i];
        }
    }
    return 0;
}


/* Puts the light in one of the three directional slots: its own slot when
   slot 0 already holds it, otherwise the least recently used one. */
void NuLightUpdate(struct nulight_s *light) {
    struct nuvec_s dir;
    struct nulight_s *l;
    int oldest;
    int best;
    int i;

    l = light;
    for (i = 0; i < 3; i++) {
        if (i == 0) {
            oldest = nulight_stamps[0];
            best = 0;
            if (nulight_current[0] == light) {
                break;
            }
        }
        if (nulight_stamps[i] < oldest) {
            oldest = nulight_stamps[i];
            best = i;
        }
    }
    if (i >= 3) {
        i = best;
    }
    dir.x = 0.0f;
    dir.y = 0.0f;
    dir.z = -1.0f;
    NuVecMtxRotate(&nulight_dirs[i], &dir, &light->mtx);
    nulight_colours[i] = l->diffuse;
    nulight_stamps[i] = ++nulight_stamp;
    nulight_current[i] = light;
    NuRndrSetDirectionalLights(&nulight_dirs[0], &nulight_colours[0],
                               &nulight_dirs[1], &nulight_colours[1],
                               &nulight_dirs[2], &nulight_colours[2]);
}


void NuLightFxSetup(void) {
    int upload;
    int i;
    int height;
    float u0;
    float u1;

    NuTexFlushTemp();
    NuTexAssignTempAddr(NuLgtHSurf[1]);
    NuTexAssignTempAddr(NuLgtHSurf[4]);
    NuTexAssignTempAddr(NuLgtHSurf[2]);
    upload = txupload_on;
    txupload_on = 0;
    if (NuRndrBeginSceneEx(NuLgtHSurf[1], -2, 0)) {
        rndrstream_3d = rndrstream_3d_u;
        rndrstream_3d->mtl = 0;
        rndrstream_3d->mtl_tid = 0;
        NuRndrStreamLink(rndrstream_3d);
        NuRndrStreamSetMisc(rndrstream_3d);
        NuRndrStreamAddMtl(rndrstream_3d, NuLightZMat);
        rndrstream_3d->cur.w = vpDmaTag_Call(rndrstream_3d->cur.w, NuLgtFxSetCall);
        rndrstream_free = vpDmaTag_Next(rndrstream_3d->cur.w, 0);
        NuRndrEndSceneEx(0);
    }

    NuTexCloneTempAddr(NuLgtSurf[3]->tid, NuLgtSurf[1]->tid);
    NuTexCloneTempAddr(NuLgtSurf[6]->tid, NuLgtSurf[1]->tid);
    NuTexCloneTempAddr(NuLgtSurf[8]->tid, NuLgtSurf[1]->tid);
    NuTexCloneTempAddr(NuLgtSurf[13]->tid, NuLgtSurf[1]->tid);
    NuTexCloneTempAddr(NuLgtSurf[17]->tid, NuLgtSurf[1]->tid);
    NuTexCloneTempAddr(NuLgtSurf[7]->tid, NuLgtSurf[2]->tid);
    NuTexCloneTempAddr(NuLgtSurf[11]->tid, NuLgtSurf[2]->tid);
    NuTexCloneTempAddr(NuLgtSurf[14]->tid, NuLgtSurf[2]->tid);
    NuTexCloneTempAddr(NuLgtSurf[5]->tid, NuLgtSurf[4]->tid);
    NuTexCloneTempAddr(NuLgtSurf[9]->tid, NuLgtSurf[4]->tid);
    NuTexCloneTempAddr(NuLgtSurf[12]->tid, NuLgtSurf[4]->tid);
    NuTexCloneTempAddr(NuLgtSurf[15]->tid, NuLgtSurf[4]->tid);

    if (NuRndrBeginSceneEx(NuLgtHSurf[8], NuLgtHSurf[8], 0)) {
        rndrstream_3d = rndrstream_3d_u;
        rndrstream_3d->mtl = 0;
        rndrstream_3d->mtl_tid = 0;
        NuRndrStreamLink(rndrstream_3d);
        NuRndrStreamSetMisc(rndrstream_3d);
        NuRndrStreamAddMtl(rndrstream_3d, NuLgtSurf[1]);
        for (i = 0; i < 4; i++) {
            u0 = (i * 64.0f + 0.5f) / 1024.0f;
            u1 = (i * 64.0f + 63.5f) / 1024.0f;
            height = (PS2_REZ_H << 4) - 8;
            NuRndrRectUV3di(i * 0x400 + 0x1400, 0, 0x3F8, height,
                            u0, 0.0f, u1, (float)PS2_REZ_H / NuPower2(PS2_REZ_H), 0x80808080, 0);
        }
        rndrstream_free = vpDmaTag_Next(rndrstream_3d->cur.w, 0);
        NuRndrEndSceneEx(0);
    }

    if (NuRndrBeginSceneEx(NuLgtHSurf[7], NuLgtHSurf[1], 0)) {
        NuRndrFxCode(0x304, 0, 0, NuLgtSurf[13]);
        NuRndrEndSceneEx(0);
    }
    txupload_on = upload;
    fx_ang += 0x6F;
    fx_ang2 += 0x173;
}


void NuLightHaze(int z, int paused) {
    int i;
    int j;
    int xoffset;
    int yoffset;
    int dx0;
    int dy0;
    int dx1;
    int dy1;
    int dx2;
    int dy2;
    int dx3;
    int dy3;
    int x0;
    int y0;
    int i1;
    int j1;
    int colour;
    int x1;
    int y1;
    int x2;
    int y2;
    int x3;
    int y3;

    if (HazeColour >> 24) {
        colour = (HazeColour & 0xFFFFFF) + ((HazeColour >> 1) & 0xFF000000);
        txupload_on = 0;
        if (NuRndrBeginSceneEx(NuLgtHSurf[5], -2, 0)) {
            rndrstream_3d = rndrstream_3d_u;
            rndrstream_3d->mtl = 0;
            rndrstream_3d->mtl_tid = 0;
            NuRndrStreamLink(rndrstream_3d);
            NuRndrStreamSetMisc(rndrstream_3d);
            NuRndrStreamAddMtl(rndrstream_3d, NuLgtBackBuff);
            rndrstream_3d->cur.w = (u32 *)vpDmaTag_CntEx(rndrstream_3d->cur.w, 0x10000000, 0x50000000) - 1;
            rndrstream_3d->cur.w = vtBeginDirect(rndrstream_3d->cur.w, 0);
            rndrstream_3d->cur.w = gsBeginGifPacket(rndrstream_3d->cur.w, 0, 1, 1, 4, 0, 9, 1, 2, 5, 2, 5, 2, 5, 2, 5);
            for (i = 0; i < 10; i++) {
                i1 = i + 1;
                for (j = 0; j < NuPower2(PS2_REZ_H) >> 4; j++) {
                    j1 = j + 1;
                    dx0 = (int)(NuTrigTable[Hazetab[j][i].x & 0xFFFF] * Hazeampx) - 8;
                    dx1 = (int)(NuTrigTable[Hazetab[j][i1].x & 0xFFFF] * Hazeampx) - 8;
                    dx2 = (int)(NuTrigTable[Hazetab[j1][i].x & 0xFFFF] * Hazeampx) - 8;
                    dx3 = (int)(NuTrigTable[Hazetab[j1][i1].x & 0xFFFF] * Hazeampx) - 8;
                    dy0 = (int)(NuTrigTable[(Hazetab[j][i].y + 0x4000) & 0xFFFF] * Hazeampy) - 8;
                    dy1 = (int)(NuTrigTable[(Hazetab[j][i1].y + 0x4000) & 0xFFFF] * Hazeampy) - 8;
                    dy2 = (int)(NuTrigTable[(Hazetab[j1][i].y + 0x4000) & 0xFFFF] * Hazeampy) - 8;
                    dy3 = (int)(NuTrigTable[(Hazetab[j1][i1].y + 0x4000) & 0xFFFF] * Hazeampy) - 8;
                    xoffset = (PS2_VCNTR_X - (nurndr_pixel_width >> 1)) << 4;
                    yoffset = (PS2_VCNTR_Y - (nurndr_pixel_height >> 1)) << 4;
                    x0 = (i << 10) + dx0 + xoffset;
                    x1 = ((i + 1) << 10) + dx1 + xoffset;
                    x2 = (i << 10) + dx2 + xoffset;
                    x3 = ((i + 1) << 10) + dx3 + xoffset;
                    y0 = (j << 8) + dy0 + yoffset;
                    y1 = (j << 8) + dy1 + yoffset;
                    y2 = (j1 << 8) + dy2 + yoffset;
                    y3 = (j1 << 8) + dy3 + yoffset;
                    *rndrstream_3d->cur.w++ = 0x80;
                    *rndrstream_3d->cur.w++ = 0x80;
                    *rndrstream_3d->cur.w++ = 0x80;
                    *rndrstream_3d->cur.w++ = 0x80;
                    *rndrstream_3d->cur.f++ = (float)i * 64.0f / 1024.0f;
                    *rndrstream_3d->cur.f++ = (float)(j << 4) / NuPower2(PS2_REZ_H);
                    *rndrstream_3d->cur.f++ = 1.0f;
                    rndrstream_3d->cur.w++;
                    *rndrstream_3d->cur.w++ = x0;
                    *rndrstream_3d->cur.w++ = y0;
                    *rndrstream_3d->cur.w++ = 0xFFFFFF;
                    *rndrstream_3d->cur.w++ = 0x8000;
                    *rndrstream_3d->cur.f++ = (float)(i + 1) * 64.0f / 1024.0f;
                    *rndrstream_3d->cur.f++ = (float)(j << 4) / NuPower2(PS2_REZ_H);
                    *rndrstream_3d->cur.f++ = 1.0f;
                    rndrstream_3d->cur.w++;
                    *rndrstream_3d->cur.w++ = x1;
                    *rndrstream_3d->cur.w++ = y1;
                    *rndrstream_3d->cur.w++ = 0xFFFFFF;
                    *rndrstream_3d->cur.w++ = 0x8000;
                    *rndrstream_3d->cur.f++ = (float)i * 64.0f / 1024.0f;
                    *rndrstream_3d->cur.f++ = (float)(j1 << 4) / NuPower2(PS2_REZ_H);
                    *rndrstream_3d->cur.f++ = 1.0f;
                    rndrstream_3d->cur.w++;
                    *rndrstream_3d->cur.w++ = x2;
                    *rndrstream_3d->cur.w++ = y2;
                    *rndrstream_3d->cur.w++ = 0xFFFFFF;
                    *rndrstream_3d->cur.w++ = 0;
                    *rndrstream_3d->cur.f++ = (float)(i + 1) * 64.0f / 1024.0f;
                    *rndrstream_3d->cur.f++ = (float)(j1 << 4) / NuPower2(PS2_REZ_H);
                    *rndrstream_3d->cur.f++ = 1.0f;
                    rndrstream_3d->cur.w++;
                    *rndrstream_3d->cur.w++ = x3;
                    *rndrstream_3d->cur.w++ = y3;
                    *rndrstream_3d->cur.w++ = 0xFFFFFF;
                    *rndrstream_3d->cur.w++ = 0;
                }
            }
            x0 = (PS2_VCNTR_X - (nurndr_pixel_width >> 1)) << 4;
            y0 = (PS2_VCNTR_Y - (nurndr_pixel_height >> 1)) << 4;
            *rndrstream_3d->cur.w++ = 0x80;
            *rndrstream_3d->cur.w++ = 0x80;
            *rndrstream_3d->cur.w++ = 0x80;
            *rndrstream_3d->cur.w++ = 0x80;
            *rndrstream_3d->cur.f++ = 0.0f;
            *rndrstream_3d->cur.f++ = 0.0f / NuPower2(PS2_REZ_H);
            *rndrstream_3d->cur.f++ = 1.0f;
            rndrstream_3d->cur.w++;
            *rndrstream_3d->cur.w++ = x0;
            *rndrstream_3d->cur.w++ = y0;
            *rndrstream_3d->cur.w++ = 0xFFFFFF;
            *rndrstream_3d->cur.w++ = 0x8000;
            *rndrstream_3d->cur.f++ = 3.5f / 1024.0f;
            *rndrstream_3d->cur.f++ = 0.0f / NuPower2(PS2_REZ_H);
            *rndrstream_3d->cur.f++ = 1.0f;
            rndrstream_3d->cur.w++;
            *rndrstream_3d->cur.w++ = x0 + 0x40;
            *rndrstream_3d->cur.w++ = y0;
            *rndrstream_3d->cur.w++ = 0xFFFFFF;
            *rndrstream_3d->cur.w++ = 0x8000;
            *rndrstream_3d->cur.f++ = 0.0f;
            *rndrstream_3d->cur.f++ = ((float)PS2_REZ_H - 0.5f) / NuPower2(PS2_REZ_H);
            *rndrstream_3d->cur.f++ = 1.0f;
            rndrstream_3d->cur.w++;
            *rndrstream_3d->cur.w++ = x0;
            *rndrstream_3d->cur.w++ = (PS2_REZ_H << 4) + y0;
            *rndrstream_3d->cur.w++ = 0xFFFFFF;
            *rndrstream_3d->cur.w++ = 0;
            *rndrstream_3d->cur.f++ = 3.5f / 1024.0f;
            *rndrstream_3d->cur.f++ = ((float)PS2_REZ_H - 0.5f) / NuPower2(PS2_REZ_H);
            *rndrstream_3d->cur.f++ = 1.0f;
            rndrstream_3d->cur.w++;
            *rndrstream_3d->cur.w++ = x0 + 0x40;
            *rndrstream_3d->cur.w++ = (PS2_REZ_H << 4) + y0;
            *rndrstream_3d->cur.w++ = 0xFFFFFF;
            *rndrstream_3d->cur.w++ = 0;
            *rndrstream_3d->cur.w++ = 0x80;
            *rndrstream_3d->cur.w++ = 0x80;
            *rndrstream_3d->cur.w++ = 0x80;
            *rndrstream_3d->cur.w++ = 0x80;
            *rndrstream_3d->cur.f++ = 636.0f / 1024.0f;
            *rndrstream_3d->cur.f++ = 0.0f / NuPower2(PS2_REZ_H);
            *rndrstream_3d->cur.f++ = 1.0f;
            rndrstream_3d->cur.w++;
            *rndrstream_3d->cur.w++ = x0 + 0x27C0;
            *rndrstream_3d->cur.w++ = y0;
            *rndrstream_3d->cur.w++ = 0xFFFFFF;
            *rndrstream_3d->cur.w++ = 0x8000;
            *rndrstream_3d->cur.f++ = 639.5f / 1024.0f;
            *rndrstream_3d->cur.f++ = 0.0f / NuPower2(PS2_REZ_H);
            *rndrstream_3d->cur.f++ = 1.0f;
            rndrstream_3d->cur.w++;
            *rndrstream_3d->cur.w++ = x0 + 0x2800;
            *rndrstream_3d->cur.w++ = y0;
            *rndrstream_3d->cur.w++ = 0xFFFFFF;
            *rndrstream_3d->cur.w++ = 0x8000;
            *rndrstream_3d->cur.f++ = 636.0f / 1024.0f;
            *rndrstream_3d->cur.f++ = ((float)PS2_REZ_H - 0.5f) / NuPower2(PS2_REZ_H);
            *rndrstream_3d->cur.f++ = 1.0f;
            rndrstream_3d->cur.w++;
            *rndrstream_3d->cur.w++ = x0 + 0x27C0;
            *rndrstream_3d->cur.w++ = (PS2_REZ_H << 4) + y0;
            *rndrstream_3d->cur.w++ = 0xFFFFFF;
            *rndrstream_3d->cur.w++ = 0;
            *rndrstream_3d->cur.f++ = 639.0f / 1024.0f;
            *rndrstream_3d->cur.f++ = ((float)PS2_REZ_H - 0.5f) / NuPower2(PS2_REZ_H);
            *rndrstream_3d->cur.f++ = 1.0f;
            rndrstream_3d->cur.w++;
            *rndrstream_3d->cur.w++ = x0 + 0x2800;
            *rndrstream_3d->cur.w++ = (PS2_REZ_H << 4) + y0;
            *rndrstream_3d->cur.w++ = 0xFFFFFF;
            *rndrstream_3d->cur.w++ = 0;
            *rndrstream_3d->cur.w++ = 0x80;
            *rndrstream_3d->cur.w++ = 0x80;
            *rndrstream_3d->cur.w++ = 0x80;
            *rndrstream_3d->cur.w++ = 0x80;
            *rndrstream_3d->cur.f++ = 0.0f;
            *rndrstream_3d->cur.f++ = 0.5f / NuPower2(PS2_REZ_H);
            *rndrstream_3d->cur.f++ = 1.0f;
            rndrstream_3d->cur.w++;
            *rndrstream_3d->cur.w++ = x0;
            *rndrstream_3d->cur.w++ = y0;
            *rndrstream_3d->cur.w++ = 0xFFFFFF;
            *rndrstream_3d->cur.w++ = 0x8000;
            *rndrstream_3d->cur.f++ = 639.5f / 1024.0f;
            *rndrstream_3d->cur.f++ = 0.5f / NuPower2(PS2_REZ_H);
            *rndrstream_3d->cur.f++ = 1.0f;
            rndrstream_3d->cur.w++;
            *rndrstream_3d->cur.w++ = x0 + 0x2800;
            *rndrstream_3d->cur.w++ = y0;
            *rndrstream_3d->cur.w++ = 0xFFFFFF;
            *rndrstream_3d->cur.w++ = 0x8000;
            *rndrstream_3d->cur.f++ = 0.0f;
            *rndrstream_3d->cur.f++ = 2.5f / NuPower2(PS2_REZ_H);
            *rndrstream_3d->cur.f++ = 1.0f;
            rndrstream_3d->cur.w++;
            *rndrstream_3d->cur.w++ = x0;
            *rndrstream_3d->cur.w++ = y0 + 0x30;
            *rndrstream_3d->cur.w++ = 0xFFFFFF;
            *rndrstream_3d->cur.w++ = 0;
            *rndrstream_3d->cur.f++ = 639.5f / 1024.0f;
            *rndrstream_3d->cur.f++ = 2.5f / NuPower2(PS2_REZ_H);
            *rndrstream_3d->cur.f++ = 1.0f;
            rndrstream_3d->cur.w++;
            *rndrstream_3d->cur.w++ = x0 + 0x2800;
            *rndrstream_3d->cur.w++ = y0 + 0x30;
            *rndrstream_3d->cur.w++ = 0xFFFFFF;
            *rndrstream_3d->cur.w++ = 0;
            *rndrstream_3d->cur.w++ = 0x80;
            *rndrstream_3d->cur.w++ = 0x80;
            *rndrstream_3d->cur.w++ = 0x80;
            *rndrstream_3d->cur.w++ = 0x80;
            *rndrstream_3d->cur.f++ = 0.0f;
            *rndrstream_3d->cur.f++ = (float)(PS2_REZ_H - 3) / NuPower2(PS2_REZ_H);
            *rndrstream_3d->cur.f++ = 1.0f;
            rndrstream_3d->cur.w++;
            *rndrstream_3d->cur.w++ = x0;
            *rndrstream_3d->cur.w++ = ((PS2_REZ_H - 3) << 4) + y0;
            *rndrstream_3d->cur.w++ = 0xFFFFFF;
            *rndrstream_3d->cur.w++ = 0x8000;
            *rndrstream_3d->cur.f++ = 639.5f / 1024.0f;
            *rndrstream_3d->cur.f++ = (float)(PS2_REZ_H - 3) / NuPower2(PS2_REZ_H);
            *rndrstream_3d->cur.f++ = 1.0f;
            rndrstream_3d->cur.w++;
            *rndrstream_3d->cur.w++ = x0 + 0x2800;
            *rndrstream_3d->cur.w++ = ((PS2_REZ_H - 3) << 4) + y0;
            *rndrstream_3d->cur.w++ = 0xFFFFFF;
            *rndrstream_3d->cur.w++ = 0x8000;
            *rndrstream_3d->cur.f++ = 0.0f;
            *rndrstream_3d->cur.f++ = ((float)PS2_REZ_H - 0.5f) / NuPower2(PS2_REZ_H);
            *rndrstream_3d->cur.f++ = 1.0f;
            rndrstream_3d->cur.w++;
            *rndrstream_3d->cur.w++ = x0;
            *rndrstream_3d->cur.w++ = (PS2_REZ_H << 4) + y0;
            *rndrstream_3d->cur.w++ = 0xFFFFFF;
            *rndrstream_3d->cur.w++ = 0;
            *rndrstream_3d->cur.f++ = 639.5f / 1024.0f;
            *rndrstream_3d->cur.f++ = ((float)PS2_REZ_H - 0.5f) / NuPower2(PS2_REZ_H);
            *rndrstream_3d->cur.f++ = 1.0f;
            rndrstream_3d->cur.w++;
            *rndrstream_3d->cur.w++ = x0 + 0x2800;
            *rndrstream_3d->cur.w++ = (PS2_REZ_H << 4) + y0;
            *rndrstream_3d->cur.w++ = 0xFFFFFF;
            *rndrstream_3d->cur.w++ = 0;
            rndrstream_3d->cur.w = gsCloseGifPacket(rndrstream_3d->cur.w);
            rndrstream_3d->cur.w = vtCloseDirect(rndrstream_3d->cur.w);
            rndrstream_3d->cur.w = vpDmaTag_Close(rndrstream_3d->cur.w);
            rndrstream_free = vpDmaTag_Next(rndrstream_3d->cur.w, 0);
            NuRndrEndSceneEx(0);
        }
        if (!paused) {
            for (i = 0; i < 21; i++) {
                for (j = 0; j < (NuPower2(PS2_REZ_H) >> 4) + 1; j++) {
                    Hazetab[j][i].x += Hazetab[j][i].u;
                    Hazetab[j][i].y += Hazetab[j][i].v;
                }
            }
        }
        if (NuRndrBeginSceneEx(-1, -2, 0)) {
            rndrstream_3d = rndrstream_3d_u;
            rndrstream_3d->mtl = 0;
            rndrstream_3d->mtl_tid = 0;
            NuRndrStreamLink(rndrstream_3d);
            NuRndrStreamSetMisc(rndrstream_3d);
            NuRndrStreamAddMtl(rndrstream_3d, NuLgtSurf[12]);
            rndrstream_3d->cur.w = (u32 *)vpDmaTag_CntEx(rndrstream_3d->cur.w, 0x11000000, 0x50000000) - 1;
            rndrstream_3d->cur.w = vtBeginDirect(rndrstream_3d->cur.w, 0);
            rndrstream_3d->cur.w = gsBeginGifPacket(rndrstream_3d->cur.w, 0, 1, 0, 0, 0, 1, 0xE);
            *((u64 *)rndrstream_3d->cur.w)++ = 0x44;
            *((u64 *)rndrstream_3d->cur.w)++ = 0x42;
            rndrstream_3d->cur.w = gsCloseGifPacket(rndrstream_3d->cur.w);
            rndrstream_3d->cur.w = vtCloseDirect(rndrstream_3d->cur.w);
            rndrstream_3d->cur.w = vpDmaTag_Close(rndrstream_3d->cur.w);
            NuRndrRectUV3di(-8, -8, 0x2800, PS2_REZ_H << 4,
                            0.0f, 0.0f, 639.5f / 1024.0f, ((float)PS2_REZ_H - 0.5f) / NuPower2(PS2_REZ_H), colour, z);
            rndrstream_free = vpDmaTag_Next(rndrstream_3d->cur.w, 0);
            NuRndrEndSceneEx(0);
        }
        txupload_on = 1;
    }
}


void NuLightPolyHaze(int paused) {
    struct nuvec4_s *pnts;
    float ratiox;
    float ratioy;
    float scale;
    int i;
    int j;
    int k;
    int xoffset;
    int yoffset;
    int dx0;
    int dy0;
    int dx1;
    int dy1;
    int dx2;
    int dy2;
    int dx3;
    int dy3;
    int x0;
    int y0;
    int i1;
    int j1;
    struct numtx_s *vpcs;
    struct nuviewport_s *vp;
    int x1;
    int y1;
    int x2;
    int y2;
    int x3;
    int y3;

    if (NuLightHazePolyCnt) {
        pnts = NuScratchAlloc32(0x80);
        txupload_on = 0;
        if (NuRndrBeginSceneEx(NuLgtHSurf[4], -2, 0)) {
            rndrstream_3d = rndrstream_3d_u;
            rndrstream_3d->mtl = 0;
            rndrstream_3d->mtl_tid = 0;
            NuRndrStreamLink(rndrstream_3d);
            NuRndrStreamSetMisc(rndrstream_3d);
            NuRndrStreamAddMtl(rndrstream_3d, NuLgtBackBuff);
            rndrstream_3d->cur.w = (u32 *)vpDmaTag_CntEx(rndrstream_3d->cur.w, 0x10000000, 0x50000000) - 1;
            rndrstream_3d->cur.w = vtBeginDirect(rndrstream_3d->cur.w, 0);
            rndrstream_3d->cur.w = gsBeginGifPacket(rndrstream_3d->cur.w, 0, 1, 1, 4, 0, 9, 1, 2, 5, 2, 5, 2, 5, 2, 5);
            for (i = 0; i < 10; i++) {
                i1 = i + 1;
                for (j = 0; j < NuPower2(PS2_REZ_H) >> 4; j++) {
                    j1 = j + 1;
                    dx0 = (int)(NuTrigTable[Hazetab[j][i].x & 0xFFFF] * Hazeampx) - 8;
                    dx1 = (int)(NuTrigTable[Hazetab[j][i1].x & 0xFFFF] * Hazeampx) - 8;
                    dx2 = (int)(NuTrigTable[Hazetab[j1][i].x & 0xFFFF] * Hazeampx) - 8;
                    dx3 = (int)(NuTrigTable[Hazetab[j1][i1].x & 0xFFFF] * Hazeampx) - 8;
                    dy0 = (int)(NuTrigTable[(Hazetab[j][i].y + 0x4000) & 0xFFFF] * Hazeampy) - 8;
                    dy1 = (int)(NuTrigTable[(Hazetab[j][i1].y + 0x4000) & 0xFFFF] * Hazeampy) - 8;
                    dy2 = (int)(NuTrigTable[(Hazetab[j1][i].y + 0x4000) & 0xFFFF] * Hazeampy) - 8;
                    dy3 = (int)(NuTrigTable[(Hazetab[j1][i1].y + 0x4000) & 0xFFFF] * Hazeampy) - 8;
                    xoffset = (PS2_VCNTR_X - (nurndr_pixel_width >> 1)) << 4;
                    yoffset = (PS2_VCNTR_Y - (nurndr_pixel_height >> 1)) << 4;
                    x0 = (i << 10) + dx0 + xoffset;
                    x1 = ((i + 1) << 10) + dx1 + xoffset;
                    x2 = (i << 10) + dx2 + xoffset;
                    x3 = ((i + 1) << 10) + dx3 + xoffset;
                    y0 = (j << 8) + dy0 + yoffset;
                    y1 = (j << 8) + dy1 + yoffset;
                    y2 = (j1 << 8) + dy2 + yoffset;
                    y3 = (j1 << 8) + dy3 + yoffset;
                    *rndrstream_3d->cur.w++ = 0x80;
                    *rndrstream_3d->cur.w++ = 0x80;
                    *rndrstream_3d->cur.w++ = 0x80;
                    *rndrstream_3d->cur.w++ = 0;
                    *rndrstream_3d->cur.f++ = (float)i * 64.0f / 1024.0f;
                    *rndrstream_3d->cur.f++ = (float)(j << 4) / NuPower2(PS2_REZ_H);
                    *rndrstream_3d->cur.f++ = 1.0f;
                    rndrstream_3d->cur.w++;
                    *rndrstream_3d->cur.w++ = x0;
                    *rndrstream_3d->cur.w++ = y0;
                    *rndrstream_3d->cur.w++ = 0xFFFFFF;
                    *rndrstream_3d->cur.w++ = 0x8000;
                    *rndrstream_3d->cur.f++ = (float)(i + 1) * 64.0f / 1024.0f;
                    *rndrstream_3d->cur.f++ = (float)(j << 4) / NuPower2(PS2_REZ_H);
                    *rndrstream_3d->cur.f++ = 1.0f;
                    rndrstream_3d->cur.w++;
                    *rndrstream_3d->cur.w++ = x1;
                    *rndrstream_3d->cur.w++ = y1;
                    *rndrstream_3d->cur.w++ = 0xFFFFFF;
                    *rndrstream_3d->cur.w++ = 0x8000;
                    *rndrstream_3d->cur.f++ = (float)i * 64.0f / 1024.0f;
                    *rndrstream_3d->cur.f++ = (float)(j1 << 4) / NuPower2(PS2_REZ_H);
                    *rndrstream_3d->cur.f++ = 1.0f;
                    rndrstream_3d->cur.w++;
                    *rndrstream_3d->cur.w++ = x2;
                    *rndrstream_3d->cur.w++ = y2;
                    *rndrstream_3d->cur.w++ = 0xFFFFFF;
                    *rndrstream_3d->cur.w++ = 0;
                    *rndrstream_3d->cur.f++ = (float)(i + 1) * 64.0f / 1024.0f;
                    *rndrstream_3d->cur.f++ = (float)(j1 << 4) / NuPower2(PS2_REZ_H);
                    *rndrstream_3d->cur.f++ = 1.0f;
                    rndrstream_3d->cur.w++;
                    *rndrstream_3d->cur.w++ = x3;
                    *rndrstream_3d->cur.w++ = y3;
                    *rndrstream_3d->cur.w++ = 0xFFFFFF;
                    *rndrstream_3d->cur.w++ = 0;
                }
            }
            x0 = (PS2_VCNTR_X - (nurndr_pixel_width >> 1)) << 4;
            y0 = (PS2_VCNTR_Y - (nurndr_pixel_height >> 1)) << 4;
            *rndrstream_3d->cur.w++ = 0x80;
            *rndrstream_3d->cur.w++ = 0x80;
            *rndrstream_3d->cur.w++ = 0x80;
            *rndrstream_3d->cur.w++ = 0;
            *rndrstream_3d->cur.f++ = 0.0f;
            *rndrstream_3d->cur.f++ = 0.0f / NuPower2(PS2_REZ_H);
            *rndrstream_3d->cur.f++ = 1.0f;
            rndrstream_3d->cur.w++;
            *rndrstream_3d->cur.w++ = x0;
            *rndrstream_3d->cur.w++ = y0;
            *rndrstream_3d->cur.w++ = 0xFFFFFF;
            *rndrstream_3d->cur.w++ = 0x8000;
            *rndrstream_3d->cur.f++ = 3.5f / 1024.0f;
            *rndrstream_3d->cur.f++ = 0.0f / NuPower2(PS2_REZ_H);
            *rndrstream_3d->cur.f++ = 1.0f;
            rndrstream_3d->cur.w++;
            *rndrstream_3d->cur.w++ = x0 + 0x40;
            *rndrstream_3d->cur.w++ = y0;
            *rndrstream_3d->cur.w++ = 0xFFFFFF;
            *rndrstream_3d->cur.w++ = 0x8000;
            *rndrstream_3d->cur.f++ = 0.0f;
            *rndrstream_3d->cur.f++ = ((float)PS2_REZ_H - 0.5f) / NuPower2(PS2_REZ_H);
            *rndrstream_3d->cur.f++ = 1.0f;
            rndrstream_3d->cur.w++;
            *rndrstream_3d->cur.w++ = x0;
            *rndrstream_3d->cur.w++ = (PS2_REZ_H << 4) + y0;
            *rndrstream_3d->cur.w++ = 0xFFFFFF;
            *rndrstream_3d->cur.w++ = 0;
            *rndrstream_3d->cur.f++ = 3.5f / 1024.0f;
            *rndrstream_3d->cur.f++ = ((float)PS2_REZ_H - 0.5f) / NuPower2(PS2_REZ_H);
            *rndrstream_3d->cur.f++ = 1.0f;
            rndrstream_3d->cur.w++;
            *rndrstream_3d->cur.w++ = x0 + 0x40;
            *rndrstream_3d->cur.w++ = (PS2_REZ_H << 4) + y0;
            *rndrstream_3d->cur.w++ = 0xFFFFFF;
            *rndrstream_3d->cur.w++ = 0;
            *rndrstream_3d->cur.w++ = 0x80;
            *rndrstream_3d->cur.w++ = 0x80;
            *rndrstream_3d->cur.w++ = 0x80;
            *rndrstream_3d->cur.w++ = 0;
            *rndrstream_3d->cur.f++ = 636.0f / 1024.0f;
            *rndrstream_3d->cur.f++ = 0.0f / NuPower2(PS2_REZ_H);
            *rndrstream_3d->cur.f++ = 1.0f;
            rndrstream_3d->cur.w++;
            *rndrstream_3d->cur.w++ = x0 + 0x27C0;
            *rndrstream_3d->cur.w++ = y0;
            *rndrstream_3d->cur.w++ = 0xFFFFFF;
            *rndrstream_3d->cur.w++ = 0x8000;
            *rndrstream_3d->cur.f++ = 639.5f / 1024.0f;
            *rndrstream_3d->cur.f++ = 0.0f / NuPower2(PS2_REZ_H);
            *rndrstream_3d->cur.f++ = 1.0f;
            rndrstream_3d->cur.w++;
            *rndrstream_3d->cur.w++ = x0 + 0x2800;
            *rndrstream_3d->cur.w++ = y0;
            *rndrstream_3d->cur.w++ = 0xFFFFFF;
            *rndrstream_3d->cur.w++ = 0x8000;
            *rndrstream_3d->cur.f++ = 636.0f / 1024.0f;
            *rndrstream_3d->cur.f++ = ((float)PS2_REZ_H - 0.5f) / NuPower2(PS2_REZ_H);
            *rndrstream_3d->cur.f++ = 1.0f;
            rndrstream_3d->cur.w++;
            *rndrstream_3d->cur.w++ = x0 + 0x27C0;
            *rndrstream_3d->cur.w++ = (PS2_REZ_H << 4) + y0;
            *rndrstream_3d->cur.w++ = 0xFFFFFF;
            *rndrstream_3d->cur.w++ = 0;
            *rndrstream_3d->cur.f++ = 639.0f / 1024.0f;
            *rndrstream_3d->cur.f++ = ((float)PS2_REZ_H - 0.5f) / NuPower2(PS2_REZ_H);
            *rndrstream_3d->cur.f++ = 1.0f;
            rndrstream_3d->cur.w++;
            *rndrstream_3d->cur.w++ = x0 + 0x2800;
            *rndrstream_3d->cur.w++ = (PS2_REZ_H << 4) + y0;
            *rndrstream_3d->cur.w++ = 0xFFFFFF;
            *rndrstream_3d->cur.w++ = 0;
            *rndrstream_3d->cur.w++ = 0x80;
            *rndrstream_3d->cur.w++ = 0x80;
            *rndrstream_3d->cur.w++ = 0x80;
            *rndrstream_3d->cur.w++ = 0;
            *rndrstream_3d->cur.f++ = 0.0f;
            *rndrstream_3d->cur.f++ = 0.0f / NuPower2(PS2_REZ_H);
            *rndrstream_3d->cur.f++ = 1.0f;
            rndrstream_3d->cur.w++;
            *rndrstream_3d->cur.w++ = x0;
            *rndrstream_3d->cur.w++ = y0;
            *rndrstream_3d->cur.w++ = 0xFFFFFF;
            *rndrstream_3d->cur.w++ = 0x8000;
            *rndrstream_3d->cur.f++ = 639.5f / 1024.0f;
            *rndrstream_3d->cur.f++ = 0.0f / NuPower2(PS2_REZ_H);
            *rndrstream_3d->cur.f++ = 1.0f;
            rndrstream_3d->cur.w++;
            *rndrstream_3d->cur.w++ = x0 + 0x2800;
            *rndrstream_3d->cur.w++ = y0;
            *rndrstream_3d->cur.w++ = 0xFFFFFF;
            *rndrstream_3d->cur.w++ = 0x8000;
            *rndrstream_3d->cur.f++ = 0.0f;
            *rndrstream_3d->cur.f++ = 2.5f / NuPower2(PS2_REZ_H);
            *rndrstream_3d->cur.f++ = 1.0f;
            rndrstream_3d->cur.w++;
            *rndrstream_3d->cur.w++ = x0;
            *rndrstream_3d->cur.w++ = y0 + 0x30;
            *rndrstream_3d->cur.w++ = 0xFFFFFF;
            *rndrstream_3d->cur.w++ = 0;
            *rndrstream_3d->cur.f++ = 639.5f / 1024.0f;
            *rndrstream_3d->cur.f++ = 2.5f / NuPower2(PS2_REZ_H);
            *rndrstream_3d->cur.f++ = 1.0f;
            rndrstream_3d->cur.w++;
            *rndrstream_3d->cur.w++ = x0 + 0x2800;
            *rndrstream_3d->cur.w++ = y0 + 0x30;
            *rndrstream_3d->cur.w++ = 0xFFFFFF;
            *rndrstream_3d->cur.w++ = 0;
            *rndrstream_3d->cur.w++ = 0x80;
            *rndrstream_3d->cur.w++ = 0x80;
            *rndrstream_3d->cur.w++ = 0x80;
            *rndrstream_3d->cur.w++ = 0;
            *rndrstream_3d->cur.f++ = 0.0f;
            *rndrstream_3d->cur.f++ = (float)(PS2_REZ_H - 3) / NuPower2(PS2_REZ_H);
            *rndrstream_3d->cur.f++ = 1.0f;
            rndrstream_3d->cur.w++;
            *rndrstream_3d->cur.w++ = x0;
            *rndrstream_3d->cur.w++ = ((PS2_REZ_H - 3) << 4) + y0;
            *rndrstream_3d->cur.w++ = 0xFFFFFF;
            *rndrstream_3d->cur.w++ = 0x8000;
            *rndrstream_3d->cur.f++ = 639.5f / 1024.0f;
            *rndrstream_3d->cur.f++ = (float)(PS2_REZ_H - 3) / NuPower2(PS2_REZ_H);
            *rndrstream_3d->cur.f++ = 1.0f;
            rndrstream_3d->cur.w++;
            *rndrstream_3d->cur.w++ = x0 + 0x2800;
            *rndrstream_3d->cur.w++ = ((PS2_REZ_H - 3) << 4) + y0;
            *rndrstream_3d->cur.w++ = 0xFFFFFF;
            *rndrstream_3d->cur.w++ = 0x8000;
            *rndrstream_3d->cur.f++ = 0.0f;
            *rndrstream_3d->cur.f++ = ((float)PS2_REZ_H - 0.5f) / NuPower2(PS2_REZ_H);
            *rndrstream_3d->cur.f++ = 1.0f;
            rndrstream_3d->cur.w++;
            *rndrstream_3d->cur.w++ = x0;
            *rndrstream_3d->cur.w++ = (PS2_REZ_H << 4) + y0;
            *rndrstream_3d->cur.w++ = 0xFFFFFF;
            *rndrstream_3d->cur.w++ = 0;
            *rndrstream_3d->cur.f++ = 639.5f / 1024.0f;
            *rndrstream_3d->cur.f++ = ((float)PS2_REZ_H - 0.5f) / NuPower2(PS2_REZ_H);
            *rndrstream_3d->cur.f++ = 1.0f;
            rndrstream_3d->cur.w++;
            *rndrstream_3d->cur.w++ = x0 + 0x2800;
            *rndrstream_3d->cur.w++ = (PS2_REZ_H << 4) + y0;
            *rndrstream_3d->cur.w++ = 0xFFFFFF;
            *rndrstream_3d->cur.w++ = 0;
            rndrstream_3d->cur.w = gsCloseGifPacket(rndrstream_3d->cur.w);
            rndrstream_3d->cur.w = vtCloseDirect(rndrstream_3d->cur.w);
            rndrstream_3d->cur.w = vpDmaTag_Close(rndrstream_3d->cur.w);
            txupload_on = 1;
            NuRndrStreamAddMtl(rndrstream_3d, NuLightHazePolymtl);
            txupload_on = 0;
            rndrstream_3d->cur.w = (u32 *)vpDmaTag_CntEx(rndrstream_3d->cur.w, 0x10000000, 0x50000000) - 1;
            rndrstream_3d->cur.w = vtBeginDirect(rndrstream_3d->cur.w, 0);
            rndrstream_3d->cur.w = gsBeginGifPacket(rndrstream_3d->cur.w, 0, 1, 0, 0, 0, 1, 0xE);
            *((u64 *)rndrstream_3d->cur.w)++ = 0x48;
            *((u64 *)rndrstream_3d->cur.w)++ = 0x42;
            rndrstream_3d->cur.w = gsCloseGifPacket(rndrstream_3d->cur.w);
            rndrstream_3d->cur.w = vtCloseDirect(rndrstream_3d->cur.w);
            rndrstream_3d->cur.w = vpDmaTag_Close(rndrstream_3d->cur.w);
            rndrstream_3d->cur.w = (u32 *)vpDmaTag_CntEx(rndrstream_3d->cur.w, 0x10000000, 0x50000000) - 1;
            rndrstream_3d->cur.w = vtBeginDirect(rndrstream_3d->cur.w, 0);
            rndrstream_3d->cur.w = gsBeginGifPacket(rndrstream_3d->cur.w, 0, 1, 1, 4, 0, 9, 1, 2, 5, 2, 5, 2, 5, 2, 5);
            vpcs = NuCameraGetVPCSMtx();
            NuCameraGetClippingRatios(&ratiox, &ratioy);
            vp = NuVpGetCurrentViewport();
            ratiox *= 2.0f;
            ratioy *= 2.0f;
            for (i = 0; i < NuLightHazePolyCnt; i++) {
                pnts[4].x = NuLgtPolyHazeData[i].pos.x;
                pnts[4].y = NuLgtPolyHazeData[i].pos.y + NuLgtPolyHazeData[i].rot;
                pnts[4].z = NuLgtPolyHazeData[i].pos.z;
                pnts[4].w = 1.0f;
                NuVec4MtxTransformVU0(&pnts[4], &pnts[4], vpcs);
                pnts[5].x = NuLgtPolyHazeData[i].pos.x;
                pnts[5].y = NuLgtPolyHazeData[i].pos.y;
                pnts[5].z = NuLgtPolyHazeData[i].pos.z;
                pnts[5].w = 1.0f;
                NuVec4MtxTransformVU0(&pnts[5], &pnts[5], vpcs);
                if (pnts[4].w > 0.2f && pnts[5].w > 0.2f) {
                    NuVec4ScaleXYZVU0(&pnts[4], &pnts[4], 1.0f / pnts[4].w);
                    NuVec4ScaleXYZVU0(&pnts[5], &pnts[5], 1.0f / pnts[5].w);
                    scale = (float)vp->width / (pnts[4].w * ratiox);
                    pnts[0].x = pnts[4].x - NuLgtPolyHazeData[i].size * scale;
                    pnts[0].y = pnts[4].y;
                    pnts[0].z = pnts[4].z;
                    pnts[1].x = pnts[4].x + NuLgtPolyHazeData[i].size * scale;
                    pnts[1].y = pnts[4].y;
                    pnts[1].z = pnts[4].z;
                    pnts[2].x = pnts[5].x - NuLgtPolyHazeData[i].size * scale;
                    pnts[2].y = pnts[5].y;
                    pnts[2].z = pnts[5].z;
                    pnts[3].x = pnts[5].x + NuLgtPolyHazeData[i].size * scale;
                    pnts[3].y = pnts[5].y;
                    pnts[3].z = pnts[5].z;
                    if (pnts[0].x > 0.0f && pnts[1].x < 65535.0f && pnts[0].y > 0.0f && pnts[2].y < 65535.0f) {
                        *rndrstream_3d->cur.w++ = 0;
                        *rndrstream_3d->cur.w++ = 0;
                        *rndrstream_3d->cur.w++ = 0;
                        *rndrstream_3d->cur.w++ = 0xA0;
                        for (k = 0; k < 4; k++) {
                            *rndrstream_3d->cur.f++ = NuLightHazePolyus[k];
                            *rndrstream_3d->cur.f++ = NuLightHazePolyvs[k];
                            *rndrstream_3d->cur.f++ = 1.0f;
                            *rndrstream_3d->cur.i++ = 0;
                            NuVecConvertToIntVU0(rndrstream_3d->cur.w, (struct nuvec_s *)&pnts[k]);
                            rndrstream_3d->cur.b += 0xC;
                            if (k < 2) {
                                *rndrstream_3d->cur.w++ = 0x8000;
                            } else {
                                *rndrstream_3d->cur.w++ = 0;
                            }
                        }
                    }
                }
            }
            rndrstream_3d->cur.w = gsCloseGifPacket(rndrstream_3d->cur.w);
            rndrstream_3d->cur.w = vtCloseDirect(rndrstream_3d->cur.w);
            rndrstream_3d->cur.w = vpDmaTag_Close(rndrstream_3d->cur.w);
            rndrstream_free = vpDmaTag_Next(rndrstream_3d->cur.w, 0);
            NuRndrEndSceneEx(0);
        }
        if (!paused) {
            for (i = 0; i < 21; i++) {
                for (j = 0; j < (NuPower2(PS2_REZ_H) >> 4) + 1; j++) {
                    Hazetab[j][i].x += Hazetab[j][i].u;
                    Hazetab[j][i].y += Hazetab[j][i].v;
                }
            }
        }
        if (NuRndrBeginSceneEx(-1, -2, 0)) {
            rndrstream_3d = rndrstream_3d_u;
            rndrstream_3d->mtl = 0;
            rndrstream_3d->mtl_tid = 0;
            NuRndrStreamLink(rndrstream_3d);
            NuRndrStreamSetMisc(rndrstream_3d);
            NuRndrStreamAddMtl(rndrstream_3d, NuLgtSurf[12]);
            rndrstream_3d->cur.w = (u32 *)vpDmaTag_CntEx(rndrstream_3d->cur.w, 0x11000000, 0x50000000) - 1;
            rndrstream_3d->cur.w = vtBeginDirect(rndrstream_3d->cur.w, 0);
            rndrstream_3d->cur.w = gsBeginGifPacket(rndrstream_3d->cur.w, 0, 1, 0, 0, 0, 1, 0xE);
            *((u64 *)rndrstream_3d->cur.w)++ = 0x44;
            *((u64 *)rndrstream_3d->cur.w)++ = 0x42;
            rndrstream_3d->cur.w = gsCloseGifPacket(rndrstream_3d->cur.w);
            rndrstream_3d->cur.w = vtCloseDirect(rndrstream_3d->cur.w);
            rndrstream_3d->cur.w = vpDmaTag_Close(rndrstream_3d->cur.w);
            NuRndrRectUV3di(-8, -8, 0x2800, PS2_REZ_H << 4,
                            0.0f, 0.0f, 639.5f / 1024.0f, ((float)PS2_REZ_H - 0.5f) / NuPower2(PS2_REZ_H),
                            0x80808080, 0x1FFFFFF);
            rndrstream_free = vpDmaTag_Next(rndrstream_3d->cur.w, 0);
            NuRndrEndSceneEx(0);
        }
        if (!paused) {
            NuLightHazePolyCnt = 0;
        }
        txupload_on = 1;
        NuScratchRelease();
    }
}

void NuLightDestroy(struct nulight_s *light) {
    int i;

    light->used = 0;
    for (i = 0; i < 3; i++) {
        if (nulight_current[i] == light) {
            nulight_current[i] = 0;
            nulight_colours[i].r = nulight_colours[i].g = nulight_colours[i].b = 0.0f;
        }
    }
}


struct nulight_s *NuLightGetLight(int index) {
    return &nulights[index];
}


void NuLigthSetPolyHazeMat(void *mtl, float *u, float *v) {
    NuLightHazePolymtl = mtl;
    NuLightHazePolyus[0] = u[0];
    NuLightHazePolyus[1] = u[1];
    NuLightHazePolyus[2] = u[0];
    NuLightHazePolyus[3] = u[1];
    NuLightHazePolyvs[0] = v[0];
    NuLightHazePolyvs[1] = v[0];
    NuLightHazePolyvs[2] = v[1];
    NuLightHazePolyvs[3] = v[1];
}


void NuLightAddSpot(struct nuvec_s *pos, float size, unsigned int colour, int mode) {
    if (NuLightSpotCnt < 8) {
        NuLightSpots[NuLightSpotCnt].pos = *pos;
        NuLightSpots[NuLightSpotCnt].size = size;
        NuLightSpots[NuLightSpotCnt].colour = colour;
        NuLightSpots[NuLightSpotCnt].mode = mode;
        NuLightSpots[NuLightSpotCnt].brightness = 1.0f;
        NuLightSpots[NuLightSpotCnt].xspan = 0;
        NuLightSpots[NuLightSpotCnt].fade = 0;
        NuLightSpotCnt++;
    }
}


void NuLightAddSpotXSpanFade(struct nuvec_s *pos, float size, float brightness, int colour,
                             int mode, int xspan, int fade) {
    if (xspan < 0x80 && NuLightSpotCnt < 8) {
        NuLightSpots[NuLightSpotCnt].pos = *pos;
        NuLightSpots[NuLightSpotCnt].size = size;
        NuLightSpots[NuLightSpotCnt].colour = colour;
        NuLightSpots[NuLightSpotCnt].mode = mode;
        NuLightSpots[NuLightSpotCnt].brightness = brightness;
        NuLightSpots[NuLightSpotCnt].xspan = xspan;
        NuLightSpots[NuLightSpotCnt].fade = fade;
        NuLightSpotCnt++;
    }
}


void NuLightFog(float near, float far, int colour, int blur, int haze_colour) {
    NuRndrFogNear = near;
    NuRndrFogFar = far;
    NuRndrFogCol = colour;
    NuRndrFogBlur = blur << 24;
    HazeColour = haze_colour;
}


void NuLightFogClear(int clear) {
    NuRndrFogClear = clear;
}


void NuLgtArcLaser(int type, struct nuvec_s *start, struct nuvec_s *end, struct nuvec_s *mid,
                   float sizew, float sizel, float sizewob, float wobspeed,
                   int colour) {
    int i;

    if (NuLgtArcLaserCnt < 16) {
        NuLgtArcLaserData[NuLgtArcLaserCnt].type = type;
        NuLgtArcLaserData[NuLgtArcLaserCnt].start = *start;
        NuLgtArcLaserData[NuLgtArcLaserCnt].end = *end;
        NuLgtArcLaserData[NuLgtArcLaserCnt].mid = *mid;
        NuLgtArcLaserData[NuLgtArcLaserCnt].sizew = sizew;
        NuLgtArcLaserData[NuLgtArcLaserCnt].sizel = sizel;
        NuLgtArcLaserData[NuLgtArcLaserCnt].sizewob = sizewob;
        NuLgtArcLaserData[NuLgtArcLaserCnt].wobspeed = wobspeed;
        NuLgtArcLaserData[NuLgtArcLaserCnt].colour = colour;
        if (!(NuLgtArcLaserFrame & 1) || !NuLgtArcLaserData[NuLgtArcLaserCnt].seed) {
            NuLgtSeed = (NuLgtSeed * 0x24CD + 1) & 0xFFFF;
            NuLgtArcLaserData[NuLgtArcLaserCnt].seed = NuLgtSeed;
        }
        for (i = 0; i < 6; i++) {
            NuLgtSeed = (NuLgtSeed * 0x24CD + 1) & 0xFFFF;
        }
        NuLgtArcLaserCnt++;
    }
}


void NuLgtSetArcMat(void *material, float u0, float v0, float u1, float v1) {
    NuLgtArcMtl = material;
    NuLgtArcU0 = u0;
    NuLgtArcV0 = v0;
    NuLgtArcU1 = u1;
    NuLgtArcV1 = v1;
}


void NuLightAddHazePoly(struct nuvec_s *pos, float size, float rot) {
    if (NuLightHazePolyCnt < 32) {
        NuLgtPolyHazeData[NuLightHazePolyCnt].pos = *pos;
        NuLgtPolyHazeData[NuLightHazePolyCnt].size = size;
        NuLgtPolyHazeData[NuLightHazePolyCnt].rot = rot;
        NuLightHazePolyCnt++;
    }
}


void *NuLightSprDmaOpen(int count) {
    void *start = NuLgtBuff;

    NuLgtBuff = (u32 *)vpDmaTag_RetEx(start, 0x10000000, 0x50000000) - 1;
    NuLgtBuff = vtBeginDirect(NuLgtBuff, 0);
    *((u64 *)NuLgtBuff)++ = (u64)(count | 0x8000) | ((u64)0x64034000 << 32);
    *((u64 *)NuLgtBuff)++ = 0x525210;
    return start;
}


void NuLightSprDmaClose(void) {
    NuLgtBuff = vtCloseDirect(NuLgtBuff);
    NuLgtBuff = vpDmaTag_Close(NuLgtBuff);
}


void NuLightSprMake(int x, int y, int width, int height, int colour, int z,
                    float u0, float v0, float u1, float v1) {
    x += (PS2_VCNTR_X - (PS2_REZ_W >> 1)) << 4;
    y += (PS2_VCNTR_Y - (PS2_REZ_H >> 1)) << 4;
    *((u64 *)NuLgtBuff)++ = 6;
    *((int *)NuLgtBuff)++ = colour;
    *((float *)NuLgtBuff)++ = 1.0f;
    *((float *)NuLgtBuff)++ = u0;
    *((float *)NuLgtBuff)++ = v0;
    *((short *)NuLgtBuff)++ = x;
    *((short *)NuLgtBuff)++ = y;
    *((int *)NuLgtBuff)++ = z;
    *((float *)NuLgtBuff)++ = u1;
    *((float *)NuLgtBuff)++ = v1;
    *((short *)NuLgtBuff)++ = x + width;
    *((short *)NuLgtBuff)++ = y + height;
    *((int *)NuLgtBuff)++ = z;
}


int NuLgtRand(void) {
    int seed = (NuLgtSeed * 0x24CD + 1) & 0xFFFF;

    NuLgtSeed = seed;
    return seed;
}
