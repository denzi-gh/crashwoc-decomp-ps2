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

typedef unsigned int u32;
typedef unsigned long u64;

typedef struct nuvec_s {
    float x;
    float y;
    float z;
} nuvec_s;

typedef struct nulight_s {
    char unknown_00[0x88];
    int index;
    int used;
} nulight_s;

typedef struct nulgtspot_s {
    nuvec_s pos;
    float size;
    int colour;
    int mode;
    float brightness;
    char xspan;
    char fade;
    char pad[2];
} nulgtspot_s;

typedef struct nulgtarclaser_s {
    nuvec_s start;
    nuvec_s end;
    nuvec_s mid;
    float sizew;
    float sizel;
    float sizewob;
    float wobspeed;
    char type;
    char pad[3];
    int colour;
    int seed;
} nulgtarclaser_s;

typedef struct nulgtpolyhaze_s {
    nuvec_s pos;
    float size;
    float rot;
} nulgtpolyhaze_s;

extern int NuRndrFogClear;
extern nulight_s D_006AD100[];
extern nuvec_s D_006AD5A8[];
extern nulight_s *D_006AD5D0[];
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
extern int NuPower2(int value);

extern int D_00633088;
extern int D_006AD5E0[];
extern nuvec_s D_006AD5F0;
extern u32 D_006AD600[];
extern int NuLightOldSpotCnt;
extern int NuLgtArcLaserOldCnt;
extern void *NuLgtFxSetCall;
extern void *NuLgtFxFogCop;
extern void *NuLgtFxFogClr;

#define nulights D_006AD100
#define nulight_dirs D_006AD5A8
#define nulight_current D_006AD5D0
#define nulight_ambient D_006AD5F0
#define nulgt_packet_buffer D_006AD600
#define nulight_stamp D_00633088
#define nulight_stamps D_006AD5E0


void NuLightInit(void) {
    int i;
    void *packet;

    for (i = 0; i < 3; i++) {
        nulight_dirs[i].x = nulight_dirs[i].y = nulight_dirs[i].z = 0.0f;
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


void NuLightClose(void) {
}


nulight_s *NuLightCreate(void) {
    int i;

    for (i = 0; i < 8; i++) {
        if (!nulights[i].used) {
            nulights[i].used = 1;
            return &nulights[i];
        }
    }
    return 0;
}


void NuLightDestroy(nulight_s *light) {
    int i;

    light->used = 0;
    for (i = 0; i < 3; i++) {
        if (nulight_current[i] == light) {
            nulight_current[i] = 0;
            nulight_dirs[i].x = nulight_dirs[i].y = nulight_dirs[i].z = 0.0f;
        }
    }
}


nulight_s *NuLightGetLight(int index) {
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


void NuLightAddSpot(nuvec_s *pos, int colour, int mode, float size) {
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


void NuLightAddSpotXSpanFade(nuvec_s *pos, int colour, int mode, int xspan,
                             int fade, float size, float brightness) {
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


void NuLgtArcLaser(int type, nuvec_s *start, nuvec_s *end, nuvec_s *mid,
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


void NuLightAddHazePoly(nuvec_s *pos, float size, float rot) {
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
