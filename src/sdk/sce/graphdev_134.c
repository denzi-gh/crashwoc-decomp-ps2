/*
 * Unit: sdk/sce/graphdev_134
 *
 * Functions:
 *   0x00261d60 sceGsSetDefDBuffDc
 */

typedef unsigned char u_char;
typedef unsigned int u_int;
typedef unsigned long u_long;
typedef unsigned int u_long128 __attribute__((mode(TI)));

typedef struct {
    short sceGsInterMode;
    short sceGsOutMode;
    short sceGsFFMode;
    short sceGsVersion;
    int (*sceGsVSCfunc)(int);
    int sceGsVSCid;
} sceGsGParam __attribute__((aligned(16)));

typedef struct {
    u_long FBP:9;
    u_long FBW:6;
    u_long PSM:5;
    u_long p0:12;
    u_long DBX:11;
    u_long DBY:11;
    u_long p1:10;
} tGS_DISPFB2;

typedef struct {
    u_long pmode;
    u_long smode2;
    tGS_DISPFB2 dispfb;
    u_long display;
    u_long bgcolor;
} sceGsDispEnv;

typedef struct {
    u_long NLOOP:15;
    u_long EOP:1;
    u_long pad16:16;
    u_long id:14;
    u_long PRE:1;
    u_long PRIM:11;
    u_long FLG:2;
    u_long NREG:4;
    u_long REGS0:4;
    u_long REGS1:4;
    u_long REGS2:4;
    u_long REGS3:4;
    u_long REGS4:4;
    u_long REGS5:4;
    u_long REGS6:4;
    u_long REGS7:4;
    u_long REGS8:4;
    u_long REGS9:4;
    u_long REGS10:4;
    u_long REGS11:4;
    u_long REGS12:4;
    u_long REGS13:4;
    u_long REGS14:4;
    u_long REGS15:4;
} sceGifTag __attribute__((aligned(16)));

typedef struct {
    u_long FBP:9;
    u_long pad09:7;
    u_long FBW:6;
    u_long pad22:2;
    u_long PSM:6;
    u_long pad30:2;
    u_long FBMSK:32;
} sceGsFrame;

typedef struct {
    sceGsFrame frame1;
    u_long frame1addr;
    u_long zbuf1;
    u_long zbuf1addr;
    u_long xyoffset1;
    u_long xyoffset1addr;
    u_long scissor1;
    u_long scissor1addr;
    u_long prmodecont;
    u_long prmodecontaddr;
    u_long colclamp;
    u_long colclampaddr;
    u_long dthe;
    u_long dtheaddr;
    u_long test1;
    u_long test1addr;
} sceGsDrawEnv1 __attribute__((aligned(16)));

typedef struct {
    sceGsFrame frame2;
    u_long frame2addr;
    u_long zbuf2;
    u_long zbuf2addr;
    u_long xyoffset2;
    u_long xyoffset2addr;
    u_long scissor2;
    u_long scissor2addr;
    u_long prmodecont;
    u_long prmodecontaddr;
    u_long colclamp;
    u_long colclampaddr;
    u_long dthe;
    u_long dtheaddr;
    u_long test2;
    u_long test2addr;
} sceGsDrawEnv2 __attribute__((aligned(16)));

typedef struct {
    u_long testa;
    u_long testaaddr;
    u_long prim;
    u_long primaddr;
    u_long rgbaq;
    u_long rgbaqaddr;
    u_long xyz2a;
    u_long xyz2aaddr;
    u_long xyz2b;
    u_long xyz2baddr;
    u_long testb;
    u_long testbaddr;
} sceGsClear __attribute__((aligned(16)));

typedef struct {
    sceGsDispEnv disp[2];
    sceGifTag giftag0;
    sceGsDrawEnv1 draw01;
    sceGsDrawEnv2 draw02;
    sceGsClear clear0;
    sceGifTag giftag1;
    sceGsDrawEnv1 draw11;
    sceGsDrawEnv2 draw12;
    sceGsClear clear1;
} sceGsDBuffDc;

#define SCE_GS_INTERLACE 1
#define SCE_GS_NOINTERLACE 0
#define SCE_GS_FRAME 1
#define SCE_GIF_PACKED_AD 0xe
#define SCE_GIF_CLEAR_TAG(tp) (*(u_long128 *)(tp) = (u_long128)0)

sceGsGParam *sceGsGetGParam(void);
void sceGsSetDefDispEnv(sceGsDispEnv *disp, short psm, short w, short h, short dx, short dy);
int sceGsSetDefDrawEnv(sceGsDrawEnv1 *draw, short psm, short w, short h, short ztest, short zpsm);
int sceGsSetDefDrawEnv2(sceGsDrawEnv2 *draw, short psm, short w, short h, short ztest, short zpsm);
void sceGsSetDefClear(sceGsClear *cp, short ztest, short x, short y, short w, short h,
                      u_char r, u_char g, u_char b, u_char a, u_int z);
int sceGszbufaddr(short psm, short w, short h);


void sceGsSetDefDBuffDc(sceGsDBuffDc *db, short psm, short w, short h, short ztest, short zpsm, short clear)
{
    sceGsGParam *gp = sceGsGetGParam();
    int zbp;

    sceGsSetDefDispEnv(&db->disp[0], psm, w, h, 0, 0);
    sceGsSetDefDispEnv(&db->disp[1], psm, w, h, 0, 0);
    sceGsSetDefDrawEnv(&db->draw01, psm, w, h, ztest, zpsm);
    sceGsSetDefDrawEnv2(&db->draw02, psm, w, h, ztest, zpsm);
    sceGsSetDefDrawEnv(&db->draw11, psm, w, h, ztest, zpsm);
    sceGsSetDefDrawEnv2(&db->draw12, psm, w, h, ztest, zpsm);
    if (clear) {
        sceGsSetDefClear(&db->clear0, ztest, 2048 - (w >> 1), 2048 - (h >> 1), w, h, 0, 0, 0, 0, 0);
        sceGsSetDefClear(&db->clear1, ztest, 2048 - (w >> 1), 2048 - (h >> 1), w, h, 0, 0, 0, 0, 0);
    }
    SCE_GIF_CLEAR_TAG(&db->giftag0);
    SCE_GIF_CLEAR_TAG(&db->giftag1);
    db->giftag0.NLOOP = clear ? 22 : 16;
    db->giftag0.EOP = 1;
    db->giftag0.NREG = 1;
    db->giftag0.REGS0 = SCE_GIF_PACKED_AD;
    db->giftag1.NLOOP = clear ? 22 : 16;
    db->giftag1.EOP = 1;
    db->giftag1.NREG = 1;
    db->giftag1.REGS0 = SCE_GIF_PACKED_AD;
    zbp = sceGszbufaddr(psm, w, h);
    if ((gp->sceGsInterMode == SCE_GS_INTERLACE && gp->sceGsFFMode == SCE_GS_FRAME)
        || gp->sceGsInterMode == SCE_GS_NOINTERLACE) {
        short fbp = zbp >> 1;

        db->disp[1].dispfb.FBP = fbp;
        db->draw01.frame1.FBP = zbp >> 1;
        db->draw02.frame2.FBP = zbp >> 1;
    }
}
