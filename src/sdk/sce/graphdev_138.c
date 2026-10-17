/*
 * Unit: sdk/sce/graphdev_138
 *
 * Functions:
 *   0x002623d8 sceGszbufaddr
 */


typedef struct {
    short sceGsInterMode;
    short sceGsOutMode;
    short sceGsFFMode;
    unsigned short sceGsVersion;
    int (*sceGsVSCfunc)(int);
    int sceGsVSCid;
} sceGsGParam __attribute__((aligned(16)));

#define SCE_GS_INTERLACE 1
#define SCE_GS_FIELD 0

sceGsGParam *sceGsGetGParam(void);


short sceGszbufaddr(short psm, short w, short h)
{
    sceGsGParam *gp;
    short width;
    short height;

    gp = sceGsGetGParam();
    width = (w + 63) / 64;
    if (psm & 2) {
        height = (h + 63) / 64;
    } else {
        height = (h + 31) / 32;
    }
    if (gp->sceGsInterMode == SCE_GS_INTERLACE && gp->sceGsFFMode == SCE_GS_FIELD) {
        return width * height;
    }
    return width * height * 2;
}
