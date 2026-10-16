/*
 * Unit: sdk/sce/graphdev_128
 *
 * Functions:
 *   0x00261348 sceGsSyncV
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

#define SCE_GS_INTERLACE 1

sceGsGParam *sceGsGetGParam(void);
void VSync(void);
s64 VSync2(void);


int sceGsSyncV(int mode)
{
    sceGsGParam *gp = sceGsGetGParam();
    s64 field;

    if (gp->sceGsVSCfunc == 0) {
        VSync();
        if (gp->sceGsInterMode == SCE_GS_INTERLACE) {
            return (GS_CSR >> 13) & 1;
        }
    } else {
        field = (VSync2() >> 13) & 1;
        if (gp->sceGsInterMode == SCE_GS_INTERLACE) {
            return field;
        }
    }
}
