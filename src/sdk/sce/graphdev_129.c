/*
 * Unit: sdk/sce/graphdev_129
 *
 * Functions:
 *   0x002613e0 sceGsSetDefStoreImage
 */

typedef unsigned int u_int;
typedef unsigned long u_long;
typedef unsigned int u_long128 __attribute__((mode(TI)));

typedef struct {
    u_long NLOOP : 15;
    u_long EOP : 1;
    u_long pad16 : 16;
    u_long id : 14;
    u_long PRE : 1;
    u_long PRIM : 11;
    u_long FLG : 2;
    u_long NREG : 4;
    u_long REGS0 : 4;
    u_long REGS1 : 4;
    u_long REGS2 : 4;
    u_long REGS3 : 4;
    u_long REGS4 : 4;
    u_long REGS5 : 4;
    u_long REGS6 : 4;
    u_long REGS7 : 4;
    u_long REGS8 : 4;
    u_long REGS9 : 4;
    u_long REGS10 : 4;
    u_long REGS11 : 4;
    u_long REGS12 : 4;
    u_long REGS13 : 4;
    u_long REGS14 : 4;
    u_long REGS15 : 4;
} sceGifTag;

typedef struct {
    u_int vifcode[4];
    sceGifTag giftag;
    u_long bitbltbuf;
    long bitbltbufaddr;
    u_long trxpos;
    long trxposaddr;
    u_long trxreg;
    long trxregaddr;
    u_long finish;
    long finishaddr;
    u_long trxdir;
    long trxdiraddr;
} sceGsStoreImage;

#define SCE_VIF1_SET_NOP(irq) ((u_int)(irq) << 31)
#define SCE_VIF1_SET_MSKPATH3(msk, irq) ((u_int)(msk) | ((u_int)0x06 << 24) | ((u_int)(irq) << 31))
#define SCE_VIF1_SET_FLUSHA(irq) (((u_int)0x13 << 24) | ((u_int)(irq) << 31))
#define SCE_VIF1_SET_DIRECT(count, irq) ((u_int)(count) | ((u_int)0x50 << 24) | ((u_int)(irq) << 31))

#define SCE_GIF_CLEAR_TAG(tp) (*(u_long128 *)(tp) = (u_long128)0)
#define SCE_GIF_PACKED_AD 0xe

#define SCE_GS_SET_BITBLTBUF(sbp, sbw, spsm, dbp, dbw, dpsm) \
    ((u_long)(sbp) | ((u_long)(sbw) << 16) | ((u_long)(spsm) << 24) | \
    ((u_long)(dbp) << 32) | ((u_long)(dbw) << 48) | ((u_long)(dpsm) << 56))
#define SCE_GS_SET_TRXPOS(ssax, ssay, dsax, dsay, dir) \
    ((u_long)(ssax) | ((u_long)(ssay) << 16) | ((u_long)(dsax) << 32) | \
    ((u_long)(dsay) << 48) | ((u_long)(dir) << 59))
#define SCE_GS_SET_TRXREG(rrw, rrh) ((u_long)(rrw) | ((u_long)(rrh) << 32))
#define SCE_GS_SET_TRXDIR(xdr) ((u_long)(xdr))

#define SCE_GS_BITBLTBUF 0x50
#define SCE_GS_TRXPOS 0x51
#define SCE_GS_TRXREG 0x52
#define SCE_GS_TRXDIR 0x53
#define SCE_GS_FINISH 0x61


int sceGsSetDefStoreImage(sceGsStoreImage *sp, short sbp, short sbw, short spsm,
                          short x, short y, short w, short h)
{
    sp->vifcode[0] = SCE_VIF1_SET_NOP(0);
    sp->vifcode[1] = SCE_VIF1_SET_MSKPATH3(0x8000, 0);
    sp->vifcode[2] = SCE_VIF1_SET_FLUSHA(0);
    sp->vifcode[3] = SCE_VIF1_SET_DIRECT(6, 0);

    SCE_GIF_CLEAR_TAG(&sp->giftag);
    sp->giftag.NLOOP = 5;
    sp->giftag.EOP = 1;
    sp->giftag.NREG = 1;
    sp->giftag.REGS0 = SCE_GIF_PACKED_AD;

    sp->bitbltbuf = SCE_GS_SET_BITBLTBUF(sbp, sbw, spsm, 0, 0, 0);
    sp->bitbltbufaddr = SCE_GS_BITBLTBUF;
    sp->trxpos = SCE_GS_SET_TRXPOS(x, y, 0, 0, 0);
    sp->trxposaddr = SCE_GS_TRXPOS;
    sp->trxreg = SCE_GS_SET_TRXREG(w, h);
    sp->trxregaddr = SCE_GS_TRXREG;
    sp->finish = 0;
    sp->finishaddr = SCE_GS_FINISH;
    sp->trxdir = SCE_GS_SET_TRXDIR(1);
    sp->trxdiraddr = SCE_GS_TRXDIR;

    __asm__ volatile ("sync");
    return sizeof(sceGsStoreImage) / 16;
}
