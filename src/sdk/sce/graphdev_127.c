/*
 * Unit: sdk/sce/graphdev_127
 *
 * Functions:
 *   0x002611a8 sceGsResetGraph
 *   0x00261338 sceGsGetGParam
 */

#include "nu_types.h"

typedef struct {
    short sceGsInterMode;
    short sceGsOutMode;
    short sceGsFFMode;
    unsigned short sceGsVersion;
    int (*sceGsVSCfunc)(int);
    int sceGsVSCid;
} sceGsGParam;

#define GS_CSR (*(volatile u64 *)0x12001000)

#define SCE_GS_RESET 0
#define SCE_GS_FLUSH 1
#define SCE_GS_NOINTERLACE_RESET 5

#define INTC_VBLANK_S 2

extern sceGsGParam D_00610850;
#define gs_gparam D_00610850

sceGsGParam *sceGsGetGParam(void);
u64 GsPutIMR(u64 imr);
void SetGsCrt(short interlace, short omode, short ffmode);
int DisableIntc(int cause);
int RemoveIntcHandler(int cause, int handler_id);


void sceGsResetGraph(short mode, short inter, short omode, short ffmode)
{
    sceGsGParam *gp;

    switch (mode) {
    case SCE_GS_RESET:
        gp = sceGsGetGParam();
        GS_CSR = 0x200;
        gp->sceGsInterMode = inter;
        gp->sceGsOutMode = omode;
        gp->sceGsVersion = (GS_CSR >> 16) & 0xFF;
        GsPutIMR(0xFF00);
        gp->sceGsFFMode = ffmode != 0;
        if (gp->sceGsVSCfunc != 0) {
            DisableIntc(INTC_VBLANK_S);
            RemoveIntcHandler(INTC_VBLANK_S, gp->sceGsVSCid);
            gp->sceGsVSCfunc = 0;
            gp->sceGsVSCid = 0;
        }
        SetGsCrt(inter & 1, omode & 0xFF, ffmode & 1);
        break;
    case SCE_GS_FLUSH:
        GS_CSR = 0x100;
        break;
    case SCE_GS_NOINTERLACE_RESET:
        gp = sceGsGetGParam();
        gp->sceGsFFMode = ffmode != 0;
        gp->sceGsInterMode = inter;
        gp->sceGsOutMode = omode;
        gp->sceGsVersion = (GS_CSR >> 16) & 0xFF;
        SetGsCrt(inter & 1, omode & 0xFF, ffmode & 1);
        break;
    }
}


sceGsGParam *sceGsGetGParam(void)
{
    return &gs_gparam;
}
