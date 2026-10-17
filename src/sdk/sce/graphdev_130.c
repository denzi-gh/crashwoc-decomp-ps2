/*
 * Unit: sdk/sce/graphdev_130
 *
 * Functions:
 *   0x00261520 sceGsExecStoreImage
 */


typedef unsigned char u_char;
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
    u_long SBP : 14;
    u_long pad14 : 2;
    u_long SBW : 6;
    u_long pad22 : 2;
    u_long SPSM : 6;
    u_long pad30 : 2;
    u_long DBP : 14;
    u_long pad46 : 2;
    u_long DBW : 6;
    u_long pad54 : 2;
    u_long DPSM : 6;
    u_long pad62 : 2;
} tGS_BITBLTBUF;

typedef struct {
    u_long RRW : 12;
    u_long pad12 : 20;
    u_long RRH : 12;
    u_long pad44 : 20;
} tGS_TRXREG;

typedef struct {
    u_int vifcode[4];
    sceGifTag giftag;
    tGS_BITBLTBUF bitbltbuf;
    long bitbltbufaddr;
    u_long trxpos;
    long trxposaddr;
    tGS_TRXREG trxreg;
    long trxregaddr;
    u_long finish;
    long finishaddr;
    u_long trxdir;
    long trxdiraddr;
} sceGsStoreImage;

#define SCE_GS_PSMCT32 0
#define SCE_GS_PSMCT24 1
#define SCE_GS_PSMCT16 2
#define SCE_GS_PSMCT16S 10
#define SCE_GS_PSMT8 19
#define SCE_GS_PSMT4 20
#define SCE_GS_PSMT8H 27
#define SCE_GS_PSMT4HL 36
#define SCE_GS_PSMT4HH 44
#define SCE_GS_PSMZ32 48
#define SCE_GS_PSMZ24 49
#define SCE_GS_PSMZ16 50
#define SCE_GS_PSMZ16S 58

#define SCE_GS_SET_TRXREG(rrw, rrh) ((u_long)(rrw) | ((u_long)(rrh) << 32))

#define D1_CHCR (*(volatile u_int *)0x10009000)
#define D1_MADR (*(volatile u_int *)0x10009010)
#define D1_QWC (*(volatile u_int *)0x10009020)
#define GIF_CTRL (*(volatile u_int *)0x10003000)
#define VIF1_STAT (*(volatile u_int *)0x10003C00)
#define VIF1_FBRST (*(volatile u_int *)0x10003C10)
#define VIF1_FIFO (*(volatile u_long128 *)0x10005000)
#define GS_CSR (*(volatile u_long *)0x12001000)
#define GS_BUSDIR (*(volatile u_long *)0x12001040)

#define D_CHCR_STR 0x100
#define VIF1_STAT_FDR 0x800000
#define VIF1_STAT_FQC 0x1F000000
#define GS_CSR_FINISH 2
#define TIMEOUT 0x1000000

#define UNCACHED(p) ((u_int)(p) | 0x20000000)
#define IS_SPR(p) (((u_int)(p) & 0x70000000) == 0x70000000)
#define PHYS_ADDR(p) ((u_int)(p) & 0x0FFFFFFF)
#define SPR_DMA_FLAG 0x80000000

u_long GsGetIMR(void);
u_long GsPutIMR(u_long imr);
int printf(const char *format, ...);


int sceGsExecStoreImage(sceGsStoreImage *sp, u_long128 *dstaddr)
{
    static u_int init_mp3[4] __attribute__((aligned(16))) = { 0x06000000, 0, 0, 0 };
    int w, h, rrh;
    int bytes, pixels;
    int fifoqwc, dmaqwc, rest, drain;
    int i;
    u_int count;
    u_long oldimr;
    union {
        u_long128 q;
        u_char b[16];
    } tmp;
    u_long128 mskpath3;

    drain = 0;
    rest = 0;
    dmaqwc = 0;
    fifoqwc = 0;
    count = 0;
    w = sp->trxreg.RRW;
    h = sp->trxreg.RRH;
    rrh = 0;

    switch (sp->bitbltbuf.SPSM) {
    case SCE_GS_PSMCT32:
    case SCE_GS_PSMZ32:
        bytes = w * h * 4;
        rest = bytes & 15;
        fifoqwc = (bytes >> 4) & 7;
        dmaqwc = (bytes >> 4) & ~7U;
        if (!rest) {
            rrh = h;
            drain = 0;
        } else {
            rrh = (h + 3) & ~3U;
            pixels = w * rrh;
            drain = (pixels >> 2) - dmaqwc - fifoqwc - 1;
        }
        break;
    case SCE_GS_PSMCT24:
    case SCE_GS_PSMZ24:
        bytes = w * h * 3;
        rest = bytes & 15;
        fifoqwc = (bytes >> 4) & 7;
        dmaqwc = (bytes >> 4) & ~7U;
        if (!rest) {
            rrh = h;
            drain = 0;
        } else {
            rrh = (h + 15) & ~15U;
            pixels = w * rrh;
            pixels = ((pixels * 3) >> 4) - dmaqwc;
            drain = pixels - fifoqwc - 1;
        }
        break;
    case SCE_GS_PSMCT16:
    case SCE_GS_PSMCT16S:
    case SCE_GS_PSMZ16:
    case SCE_GS_PSMZ16S:
        bytes = w * h * 2;
        rest = bytes & 15;
        fifoqwc = (bytes >> 4) & 7;
        dmaqwc = (bytes >> 4) & ~7U;
        if (!rest) {
            rrh = h;
            drain = 0;
        } else {
            rrh = (h + 7) & ~7U;
            pixels = w * rrh;
            drain = (pixels >> 3) - dmaqwc - fifoqwc - 1;
        }
        break;
    case SCE_GS_PSMT8:
    case SCE_GS_PSMT8H:
        bytes = w * h;
        rest = bytes & 15;
        fifoqwc = (bytes >> 4) & 7;
        dmaqwc = (bytes >> 4) & ~7U;
        if (!rest) {
            rrh = h;
            drain = 0;
        } else {
            rrh = (h + 7) & ~7U;
            pixels = w * rrh;
            drain = (pixels >> 4) - dmaqwc - fifoqwc - 1;
        }
        break;
    case SCE_GS_PSMT4:
    case SCE_GS_PSMT4HL:
    case SCE_GS_PSMT4HH:
        bytes = (w * h) >> 1;
        rest = bytes & 15;
        fifoqwc = (bytes >> 4) & 7;
        dmaqwc = (bytes >> 4) & ~7U;
        if (!rest) {
            rrh = h;
            drain = 0;
        } else {
            rrh = (h + 7) & ~7U;
            pixels = w * rrh;
            drain = (pixels >> 5) - dmaqwc - fifoqwc - 1;
        }
        break;
    }

    if (rest) {
        *(u_long *)UNCACHED(&sp->trxreg) = SCE_GS_SET_TRXREG((int)sp->trxreg.RRW, rrh);
    }

    while (D1_CHCR & D_CHCR_STR) {
        if (count++ > TIMEOUT) {
            printf("sceGsExecStoreImage: DMA Ch.1 does not terminate\r\n");
            return -1;
        }
    }

    oldimr = GsPutIMR(GsGetIMR() | 0x200);
    GS_CSR = GS_CSR_FINISH;
    D1_QWC = 7;
    if (IS_SPR(sp)) {
        D1_MADR = PHYS_ADDR(sp) | SPR_DMA_FLAG;
    } else {
        D1_MADR = PHYS_ADDR(sp);
    }
    D1_CHCR = 0x101;

    while (D1_CHCR & D_CHCR_STR) {
        if (count++ > TIMEOUT) {
            printf("sceGsExecStoreImage: DMA Ch.1 does not terminate\r\n");
            return -1;
        }
    }

    while (!(GS_CSR & GS_CSR_FINISH)) {
        if (count++ > TIMEOUT) {
            printf("sceGsExecStoreImage: GS does not terminate\r\n");
            VIF1_FIFO = *(u_long128 *)init_mp3;
            return -1;
        }
    }

    VIF1_STAT = VIF1_STAT_FDR;
    GS_BUSDIR = 1;

    if (dmaqwc) {
        D1_QWC = dmaqwc;
        if (IS_SPR(dstaddr)) {
            D1_MADR = PHYS_ADDR(dstaddr) | SPR_DMA_FLAG;
        } else {
            D1_MADR = PHYS_ADDR(dstaddr);
        }
        D1_CHCR = 0x100;
        while (D1_CHCR & D_CHCR_STR) {
            if (count++ > TIMEOUT) {
                printf("sceGsExecStoreImage: DMA Ch.1 (GS->MEM) does not terminate\r\n");
                GS_CSR = 0x100;
                GS_BUSDIR = 0;
                GIF_CTRL = 1;
                VIF1_FBRST = 1;
                return -1;
            }
        }
    }

    for (i = 0; i < fifoqwc; i++) {
        while (!(VIF1_STAT & VIF1_STAT_FQC)) {
            if (count++ > TIMEOUT) {
                printf("sceGsExecStoreImage: Enough data does not reach VIF1\n");
                GS_CSR = 0x100;
                GS_BUSDIR = 0;
                GIF_CTRL = 1;
                VIF1_FBRST = 1;
                return -1;
            }
        }
        dstaddr[dmaqwc + i] = VIF1_FIFO;
    }

    if (rest) {
        while (!(VIF1_STAT & VIF1_STAT_FQC)) {
            if (count++ > TIMEOUT) {
                printf("sceGsExecStoreImage: Enough data does not reach VIF1\n");
                GS_CSR = 0x100;
                GS_BUSDIR = 0;
                GIF_CTRL = 1;
                VIF1_FBRST = 1;
                return -1;
            }
        }
        tmp.q = VIF1_FIFO;
        for (i = 0; i < rest; i++) {
            ((u_char *)&dstaddr[dmaqwc + fifoqwc])[i] = tmp.b[i];
        }
        for (i = 0; i < drain; i++) {
            while (!(VIF1_STAT & VIF1_STAT_FQC)) {
                if (count++ > TIMEOUT) {
                    printf("sceGsExecStoreImage: Enough data does not reach VIF1\n");
                    GS_CSR = 0x100;
                    GS_BUSDIR = 0;
                    GIF_CTRL = 1;
                    VIF1_FBRST = 1;
                    return -1;
                }
            }
            tmp.q = VIF1_FIFO;
        }
    }

    VIF1_STAT = 0;
    GS_BUSDIR = 0;
    GsPutIMR(oldimr);
    mskpath3 = *(u_long128 *)init_mp3;
    GS_CSR = GS_CSR_FINISH;
    VIF1_FIFO = mskpath3;
    return 0;
}
