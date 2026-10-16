/*
 * Unit: sdk/sce/graphdev_137
 *
 * Functions:
 *   0x00262318 sceGsPutDispEnv
 */

typedef unsigned long u_long;

typedef struct {
    short sceGsInterMode;
    short sceGsOutMode;
    short sceGsFFMode;
    short sceGsVersion;
    int (*sceGsVSCfunc)(int);
    int sceGsVSCid;
} sceGsGParam __attribute__((aligned(16)));

typedef struct {
    u_long pmode;
    u_long smode2;
    u_long dispfb;
    u_long display;
    u_long bgcolor;
} sceGsDispEnv;

#define GS_PMODE    (*(volatile u_long *)0x12000000)
#define GS_SMODE2   (*(volatile u_long *)0x12000020)
#define GS_DISPFB1  (*(volatile u_long *)0x12000070)
#define GS_DISPLAY1 (*(volatile u_long *)0x12000080)
#define GS_DISPFB2  (*(volatile u_long *)0x12000090)
#define GS_DISPLAY2 (*(volatile u_long *)0x120000a0)
#define GS_EXTDATA  (*(volatile u_long *)0x120000c0)
#define GS_BGCOLOR  (*(volatile u_long *)0x120000e0)

sceGsGParam *sceGsGetGParam(void);


void sceGsPutDispEnv(sceGsDispEnv *disp)
{
    if (sceGsGetGParam()->sceGsVersion == 1) {
        GS_PMODE = disp->pmode;
        GS_DISPFB1 = disp->dispfb;
        GS_DISPLAY1 = disp->display;
        GS_EXTDATA = disp->bgcolor;
    } else {
        GS_PMODE = disp->pmode;
        GS_SMODE2 = disp->smode2;
        GS_DISPFB2 = disp->dispfb;
        GS_DISPLAY2 = disp->display;
        GS_BGCOLOR = disp->bgcolor;
    }
}
