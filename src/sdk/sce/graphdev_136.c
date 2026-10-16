/*
 * Unit: sdk/sce/graphdev_136
 *
 * Functions:
 *   0x002620a8 sceGsSetDefDispEnv
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

#define SCE_GS_NOINTERLACE 0
#define SCE_GS_INTERLACE 1
#define SCE_GS_FIELD 0
#define SCE_GS_NTSC 2
#define SCE_GS_PAL 3

#define SCE_GS_SET_PMODE(en1, en2, mmod, amod, slbg, alp) \
    ((u_long)(en1) | ((u_long)(en2) << 1) | ((u_long)1 << 2) | \
    ((u_long)(mmod) << 5) | ((u_long)(amod) << 6) | \
    ((u_long)(slbg) << 7) | ((u_long)(alp) << 8))
#define SCE_GS_SET_SMODE2(intr, ffmd, dpms) \
    ((u_long)(intr) | ((u_long)(ffmd) << 1) | ((u_long)(dpms) << 2))
#define GS_DISPFB(fbw, psm) \
    (((u_long)(psm) << 15) | ((u_long)(fbw) << 9))
#define GS_DISPLAY(dx, dy, magh, dw, dh) \
    (((u_long)(magh) << 23) | ((u_long)(dh) << 44) | (u_long)(dx) | \
    ((u_long)(dy) << 12) | ((u_long)(dw) << 32))

extern char D_0062AE50[];
#define unsupported_mode_msg D_0062AE50

sceGsGParam *sceGsGetGParam(void);
int printf(const char *fmt, ...);


void sceGsSetDefDispEnv(sceGsDispEnv *disp, short psm, short w, short h, short dx, short dy)
{
    sceGsGParam *gp = sceGsGetGParam();

    disp->pmode = SCE_GS_SET_PMODE(0, 1, 1, 1, 0, 0);
    if (gp->sceGsInterMode != SCE_GS_NOINTERLACE) {
        if (gp->sceGsFFMode != SCE_GS_FIELD) {
            disp->smode2 = SCE_GS_SET_SMODE2(1, 1, 0);
        } else {
            disp->smode2 = SCE_GS_SET_SMODE2(1, 0, 0);
        }
    } else {
        disp->smode2 = SCE_GS_SET_SMODE2(0, 1, 0);
    }
    disp->dispfb = GS_DISPFB(((w + 63) >> 6) & 0x3f, psm & 0xf);
    if (gp->sceGsOutMode == SCE_GS_NTSC) {
        if (gp->sceGsInterMode == SCE_GS_INTERLACE) {
            int magh = (w + 2559) / w;

            disp->display = GS_DISPLAY(((u_long)(dx * magh) + 636) & 0xfff, (50 + dy) & 0xfff,
                                       magh - 1, magh * w - 1,
                                       gp->sceGsFFMode ? h * 2 - 1 : h - 1);
        } else {
            int magh = (w + 2559) / w;

            disp->display = GS_DISPLAY(((u_long)(dx * magh) + 636) & 0xfff, (25 + dy) & 0xfff,
                                       magh - 1, magh * w - 1, h - 1);
        }
    } else if (gp->sceGsOutMode == SCE_GS_PAL) {
        if (gp->sceGsInterMode == SCE_GS_INTERLACE) {
            int magh = (w + 2559) / w;

            disp->display = GS_DISPLAY(((u_long)(dx * magh) + 656) & 0xfff, (72 + dy) & 0xfff,
                                       magh - 1, magh * w - 1,
                                       gp->sceGsFFMode ? h * 2 - 1 : h - 1);
        } else {
            int magh = (w + 2559) / w;

            disp->display = GS_DISPLAY(((u_long)(dx * magh) + 656) & 0xfff, (36 + dy) & 0xfff,
                                       magh - 1, magh * w - 1, h - 1);
        }
    } else {
        printf(unsupported_mode_msg, gp->sceGsOutMode);
    }
    disp->bgcolor = 0;
}
