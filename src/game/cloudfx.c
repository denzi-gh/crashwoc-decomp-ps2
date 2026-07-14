/*
 * Unit: game/cloudfx
 *
 * Functions:
 *   0x0024dc30 cloudInit
 *   0x0024ddf0 cloudRender
 *   0x0024e2e0 InitClouds
 *   0x0024e400 DoClouds
 *   0x0024e848 TimeTunnelInit
 *   0x0024eb00 TimeTunnelRender
 *   0x0024f280 cloudProcess
 *   0x0024f2a8 CloseClouds
 *   0x0024f2b0 TimeTunnelClose
 *   0x0024f2e0 CloudFxInit
 */

#include "creature.h"

extern s32 cloudreset;
extern void *ttmtl;
extern void *tttex;
extern void *clouds_rs;
extern u8 D_00293090[];

struct nuivec_s {
    s32 x;
    s32 y;
    s32 z;
};

extern struct nuvec_s groff[];
extern struct nuivec_s grphase[];
extern struct nuivec_s grphaserate[];

extern void cloudInit();
extern void NuMtlDestroy(void *mtl);
extern void *NuRndrStreamAllocStream(void *a0, void *a1, s32 a2);
extern f32 NuRandFloat(void);
extern int rand(void);


void InitClouds(void *buffer, void *buffend) {
    s32 n;

    cloudInit(buffer, buffend);
    for (n = 0; n < 10; n++) {
        groff[n].x = NuRandFloat();
        groff[n].y = NuRandFloat();
        groff[n].z = NuRandFloat();
        grphase[n].x = rand() << 1;
        grphase[n].y = rand() << 1;
        grphase[n].z = rand() << 1;
        grphaserate[n].x = (rand() & 0xff) + 0x80;
        grphaserate[n].y = (rand() & 0xff) + 0x80;
        grphaserate[n].z = (rand() & 0xff) + 0x80;
    }
}

void cloudProcess(void) {
    if (cloudreset != 0) {
        cloudInit();
        cloudreset = 0;
    }
}

void CloseClouds(void) {
}

void TimeTunnelClose(void) {
    if (ttmtl != 0) {
        NuMtlDestroy(ttmtl);
    }
    ttmtl = 0;
    tttex = 0;
}

void CloudFxInit(void) {
    if (clouds_rs == 0) {
        clouds_rs = NuRndrStreamAllocStream((void *)0x10000000, D_00293090, 0x3C);
    }
}
