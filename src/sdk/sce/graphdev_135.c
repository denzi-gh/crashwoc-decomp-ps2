/*
 * Unit: sdk/sce/graphdev_135
 *
 * Functions:
 *   0x00262048 sceGsSwapDBuffDc
 */

typedef unsigned long u_long;
typedef unsigned int u_long128 __attribute__((mode(TI)));

typedef struct {
    u_long pmode;
    u_long smode2;
    u_long dispfb;
    u_long display;
    u_long bgcolor;
} sceGsDispEnv;

typedef struct {
    u_long128 tag;
} sceGifTag;

typedef struct {
    u_long frame1;
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
} sceGsDrawEnv1;

typedef struct {
    u_long frame2;
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
} sceGsDrawEnv2;

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
} sceGsClear;

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

void sceGsPutDispEnv(sceGsDispEnv *disp);
int sceGsPutDrawEnv(sceGifTag *giftag);


int sceGsSwapDBuffDc(sceGsDBuffDc *db, int id)
{
    int ret;

    sceGsPutDispEnv(&db->disp[id & 1]);
    if (id & 1) {
        ret = sceGsPutDrawEnv(&db->giftag1);
    } else {
        ret = sceGsPutDrawEnv(&db->giftag0);
    }
    return ret;
}
