/*
 * Unit: sdk/sce/iopreset
 *
 * Functions:
 *   0x002808a0 sceSifResetIop
 *   0x002809e0 sceSifIsAliveIop
 *   0x00280a08 sceSifSyncIop
 *   0x00280a40 sceSifRebootIop
 */

#define RESET_ARG_MAX 80
#define UDNL_PREFIX   "rom0:UDNL "

typedef struct SifCmdHeader {
    unsigned int psize : 8;
    unsigned int dsize : 24;
    unsigned int dest;
    int cid;
    unsigned int opt;
} SifCmdHeader;

typedef struct SifCmdResetData {
    SifCmdHeader header;
    int arglen;
    int mode;
    char arg[RESET_ARG_MAX];
} SifCmdResetData;

typedef struct sceSifDmaData {
    unsigned int data;
    unsigned int addr;
    unsigned int size;
    unsigned int mode;
} sceSifDmaData;

static SifCmdResetData rdata __attribute__((aligned(64)));

#define SIF_CMD_RESET_CMD    0x80000003
#define SIF_SYSREG_SUBADDR   0x80000000
#define SIF_SYSREG_RPCINIT   0x80000002
#define SIF_REG_SMFLAG       4
#define SIF_STAT_SIFINIT     0x10000
#define SIF_STAT_CMDINIT     0x20000
#define SIF_STAT_BOOTEND     0x40000
#define SIF_DMA_INT_O        0x4
#define SIF_DMA_ERT          0x40

extern void sceSifStopDma(void);
extern unsigned int sceSifGetReg(unsigned int reg);
extern int sceSifSetReg(unsigned int reg, unsigned int value);
extern unsigned int sceSifSetDma(sceSifDmaData *sdd, int len);
extern void sceSifWriteBackDCache(void *ptr, int size);
extern void sceResetttyinit(void);
extern int sceSifInitRpc(unsigned int mode);
extern void sceSifExitRpc(void);
extern int printf(const char *fmt, ...);


int sceSifResetIop(const char *arg, int mode)
{
    sceSifDmaData dma;
    unsigned int iop_addr;
    int i;

    sceSifStopDma();
    iop_addr = sceSifGetReg(SIF_SYSREG_SUBADDR);

    rdata.mode = mode;
    for (i = 0; arg[i] != 0; i++) {
        rdata.arg[i] = arg[i];
    }
    rdata.header.dest = 0;
    rdata.arglen = i;
    rdata.header.cid = SIF_CMD_RESET_CMD;
    rdata.header.dsize = 0;
    rdata.header.psize = sizeof(SifCmdResetData);

    dma.data = (unsigned int)&rdata;
    dma.addr = iop_addr;
    dma.size = sizeof(SifCmdResetData);
    dma.mode = SIF_DMA_ERT | SIF_DMA_INT_O;

    sceSifWriteBackDCache(&rdata, sizeof(SifCmdResetData));
    sceSifSetReg(SIF_REG_SMFLAG, SIF_STAT_BOOTEND);

    if (sceSifSetDma(&dma, 1) != 0) {
        sceSifSetReg(SIF_REG_SMFLAG, SIF_STAT_SIFINIT);
        sceSifSetReg(SIF_REG_SMFLAG, SIF_STAT_CMDINIT);
        sceSifSetReg(SIF_SYSREG_RPCINIT, 0);
        sceSifSetReg(SIF_SYSREG_SUBADDR, 0);
        return 1;
    }
    return 0;
}


int sceSifIsAliveIop(void)
{
    if (sceSifGetReg(SIF_REG_SMFLAG) & SIF_STAT_SIFINIT) {
        return 1;
    }
    return 0;
}


int sceSifSyncIop(void)
{
    if (sceSifGetReg(SIF_REG_SMFLAG) & SIF_STAT_BOOTEND) {
        sceResetttyinit();
        return 1;
    }
    return 0;
}


int sceSifRebootIop(const char *img)
{
    char param[RESET_ARG_MAX];
    const char *prefix = UDNL_PREFIX;
    const char *end;
    char *dst;

    for (end = img; *end != 0; end++) {
    }
    if ((unsigned int)(sizeof(UDNL_PREFIX) + end - img) > RESET_ARG_MAX) {
        printf("too long parameter '%s'\n", img);
        return 0;
    }

    sceSifInitRpc(0);
    sceSifExitRpc();

    dst = param;
    while (*prefix != 0) {
        *dst++ = *prefix++;
    }
    while (*img != 0) {
        *dst++ = *img++;
    }
    *dst = 0;

    return sceSifResetIop(param, 0);
}
