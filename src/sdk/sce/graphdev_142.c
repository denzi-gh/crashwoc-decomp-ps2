/*
 * Unit: sdk/sce/graphdev_142
 *
 * Functions:
 *   0x00262878 sceGsSetDefDrawEnv2
 */

typedef unsigned long u_long;

typedef struct {
    u_long AC : 1;
    u_long pad01 : 63;
} sceGsPrmodecont;

typedef struct {
    u_long CLAMP : 1;
    u_long pad01 : 63;
} sceGsColclamp;

typedef struct {
    u_long DTHE : 1;
    u_long pad01 : 63;
} sceGsDthe;

typedef struct {
    u_long frame2;
    long frame2addr;
    u_long zbuf2;
    long zbuf2addr;
    u_long xyoffset2;
    long xyoffset2addr;
    u_long scissor2;
    long scissor2addr;
    sceGsPrmodecont prmodecont;
    long prmodecontaddr;
    sceGsColclamp colclamp;
    long colclampaddr;
    sceGsDthe dthe;
    long dtheaddr;
    u_long test2;
    long test2addr;
} sceGsDrawEnv2;

#define SCE_GS_SET_FRAME(fbp, fbw, psm, fbmsk) \
    ((u_long)(fbp) | ((u_long)(fbw) << 16) | ((u_long)(psm) << 24) | ((u_long)(fbmsk) << 32))
#define SCE_GS_SET_ZBUF(zbp, psm, zmsk) \
    ((u_long)(zbp) | ((u_long)(psm) << 24) | ((u_long)(zmsk) << 32))
#define SCE_GS_SET_XYOFFSET(ofx, ofy) ((u_long)(ofx) | ((u_long)(ofy) << 32))
#define SCE_GS_SET_SCISSOR(scax0, scax1, scay0, scay1) \
    ((u_long)(scax0) | ((u_long)(scax1) << 16) | ((u_long)(scay0) << 32) | ((u_long)(scay1) << 48))
#define SCE_GS_SET_TEST(ate, atst, aref, afail, date, datm, zte, ztst) \
    ((u_long)(ate) | ((u_long)(atst) << 1) | ((u_long)(aref) << 4) | \
    ((u_long)(afail) << 12) | ((u_long)(date) << 14) | ((u_long)(datm) << 15) | \
    ((u_long)(zte) << 16) | ((u_long)(ztst) << 17))

#define SCE_GS_XYOFFSET_2 0x19
#define SCE_GS_PRMODECONT 0x1a
#define SCE_GS_SCISSOR_2 0x41
#define SCE_GS_DTHE 0x45
#define SCE_GS_COLCLAMP 0x46
#define SCE_GS_TEST_2 0x48
#define SCE_GS_FRAME_2 0x4d
#define SCE_GS_ZBUF_2 0x4f

#define SCE_GS_ZALWAYS 1
#define GS_COORD_CENTER 2048L

short sceGszbufaddr(short psm, short w, short h);


int sceGsSetDefDrawEnv2(sceGsDrawEnv2 *draw, short psm, short w, short h, short ztest, short zpsm)
{
    draw->frame2addr = SCE_GS_FRAME_2;
    draw->frame2 = SCE_GS_SET_FRAME(0, (w >> 6) & 0x3f, psm & 0xf, 0);
    draw->zbuf2addr = SCE_GS_ZBUF_2;
    if (ztest == 0) {
        draw->zbuf2 = SCE_GS_SET_ZBUF(sceGszbufaddr(psm, w, h), zpsm & 0xf, 1);
    } else {
        draw->zbuf2 = SCE_GS_SET_ZBUF(sceGszbufaddr(psm, w, h), zpsm & 0xf, 0);
    }
    draw->xyoffset2addr = SCE_GS_XYOFFSET_2;
    draw->xyoffset2 = SCE_GS_SET_XYOFFSET((GS_COORD_CENTER - (w >> 1)) << 4,
                                          (GS_COORD_CENTER - (h >> 1)) << 4);
    draw->scissor2addr = SCE_GS_SCISSOR_2;
    draw->scissor2 = SCE_GS_SET_SCISSOR(0, w - 1, 0, h - 1);
    draw->prmodecontaddr = SCE_GS_PRMODECONT;
    draw->prmodecont.AC = 1;
    draw->colclampaddr = SCE_GS_COLCLAMP;
    draw->colclamp.CLAMP = 1;
    draw->dtheaddr = SCE_GS_DTHE;
    if (psm & 2) {
        draw->dthe.DTHE = 1;
    } else {
        draw->dthe.DTHE = 0;
    }
    draw->test2addr = SCE_GS_TEST_2;
    if (ztest) {
        draw->test2 = SCE_GS_SET_TEST(0, 0, 0, 0, 0, 0, 1, ztest & 3);
    } else {
        draw->test2 = SCE_GS_SET_TEST(0, 0, 0, 0, 0, 0, 1, SCE_GS_ZALWAYS);
    }
    __asm__ volatile ("sync");
    return sizeof(sceGsDrawEnv2) / 16;
}
