/*
 * Unit: sdk/sce/libdma
 *
 * Functions:
 *   0x00262a58 memclr
 *   0x00262a90 sceDmaGetChan
 *   0x00262ab8 sceDmaReset
 *   0x00262b98 sceDmaDebug
 *   0x00262ba8 sceDmaPutEnv
 *   0x00262d80 sceDmaGetEnv
 *   0x00262db8 sceDmaPutStallAddr
 *   0x00262de0 sceDmaSend
 *   0x00262eb8 sceDmaSendN
 *   0x00262fa0 sceDmaSendI
 *   0x00263088 sceDmaRecv
 *   0x00263150 sceDmaRecvN
 *   0x00263240 sceDmaRecvI
 *   0x00263330 sceDmaSync
 *   0x002633f8 sceDmaWatch
 *   0x002634c8 sceDmaPause
 *   0x00263510 sceDmaRestart
 */

typedef unsigned int u_int;
typedef unsigned short u_short;
typedef unsigned char u_char;
typedef volatile u_int vu_int;

typedef struct {
    u_int dir : 1;
    u_int : 1;
    u_int mod : 2;
    u_int asp : 2;
    u_int tte : 1;
    u_int tie : 1;
    u_int str : 1;
    u_int : 7;
    u_int tag : 16;
} DmaChcr;

typedef union {
    vu_int word;
    DmaChcr bits;
} DmaChcrReg;

typedef struct {
    DmaChcrReg chcr;
    u_int pad0[3];
    void * volatile madr;
    u_int pad1[3];
    vu_int qwc;
    u_int pad2[3];
    void * volatile tadr;
    u_int pad3[3];
    void * volatile as0;
    u_int pad4[3];
    void * volatile as1;
    u_int pad5[3];
    u_int pad6[8];
    void * volatile sadr;
    u_int pad7[3];
} sceDmaChan;

typedef struct {
    u_char sts;
    u_char std;
    u_char mfd;
    u_char rcycle;
    u_short express;
    u_short notify;
    u_short sqwc;
    u_short tqwc;
    void *rbadr;
    u_int rbmsk;
} sceDmaEnv;

#define DMA_CTRL    (*(vu_int *)0x1000e000)
#define DMA_STAT    (*(vu_int *)0x1000e010)
#define DMA_PCR     (*(vu_int *)0x1000e020)
#define DMA_SQWC    (*(vu_int *)0x1000e030)
#define DMA_RBSR    (*(vu_int *)0x1000e040)
#define DMA_RBOR    (*(vu_int *)0x1000e050)
#define DMA_STADR   (*(vu_int *)0x1000e060)
#define DMA_ENABLER (*(vu_int *)0x1000f520)
#define DMA_ENABLEW (*(vu_int *)0x1000f590)

#define DMA_CTRL_DMAE    0x1
#define DMA_CTRL_RELE    0x2
#define DMA_CTRL_MFD_M   0xc
#define DMA_CTRL_STS_M   0x30
#define DMA_CTRL_STD_M   0xc0
#define DMA_CTRL_RCYC_M  0x300
#define DMA_STAT_CIS_M   0xff1f
#define DMA_STAT_CIM_M   0xff1f0000
#define DMA_ENABLEW_PAUSE 0x1000
#define DMA_CHCR_STR     0x100

#define DMA_CHAN_COUNT   10
#define DMA_SYNC_TIMEOUT 0x1000000

extern sceDmaChan *dch[];
extern int sceDmaDebugMode[];
extern int D_006108B0[];
extern u_char D_006108D8[];
extern u_char D_006108E8[];
extern u_char D_006108F8[];
extern sceDmaEnv sceDmaCurrentEnv;

#define dma_debug_mode sceDmaDebugMode[0]
#define chan_resettable D_006108B0
#define sts_select D_006108D8
#define std_select D_006108E8
#define mfd_select D_006108F8

extern int printf(const char *fmt, ...);

int sceDmaPutEnv(sceDmaEnv *env);


static void memclr(void *buf, int size)
{
    u_char *p = buf;

    while (size--) {
        *p++ = 0;
    }
}


sceDmaChan *sceDmaGetChan(int id)
{
    if ((u_int)id < DMA_CHAN_COUNT) {
        return dch[id];
    }
    return 0;
}


int sceDmaReset(int mode)
{
    sceDmaEnv env;
    int was_enabled = DMA_CTRL & DMA_CTRL_DMAE;
    int i;

    for (i = 0; i < DMA_CHAN_COUNT; i++) {
        if (chan_resettable[i]) {
            sceDmaChan *d = dch[i];

            d->sadr = 0;
            d->chcr.word = 0;
            d->tadr = 0;
            d->madr = 0;
            d->as1 = 0;
            d->as0 = 0;
        }
    }
    DMA_STAT = DMA_STAT_CIS_M;
    DMA_STAT = DMA_STAT & DMA_STAT_CIM_M;
    memclr(&env, sizeof(env));
    sceDmaPutEnv(&env);
    if (mode == 1) {
        DMA_CTRL = DMA_CTRL | DMA_CTRL_DMAE;
    }
    return was_enabled;
}


int sceDmaDebug(int mode)
{
    int old = dma_debug_mode;

    dma_debug_mode = mode;
    return old;
}


int sceDmaPutEnv(sceDmaEnv *env)
{
    u_int ctrl = DMA_CTRL;
    u_int pcr = DMA_PCR;
    u_int sqwc = DMA_SQWC;
    u_int rbor = DMA_RBOR;
    u_int rbsr = DMA_RBSR;

    if (env->sts >= 10) {
        return -1;
    }
    if (env->std >= 10) {
        return -2;
    }
    if (env->mfd >= 10) {
        return -3;
    }
    if (env->rcycle >= 7) {
        return -4;
    }
    ctrl = (ctrl & ~DMA_CTRL_STS_M) | (sts_select[env->sts] << 4);
    ctrl = (ctrl & ~DMA_CTRL_STD_M) | (std_select[env->std] << 6);
    ctrl = (ctrl & ~DMA_CTRL_MFD_M) | (mfd_select[env->mfd] << 2);
    if (env->rcycle) {
        ctrl |= DMA_CTRL_RELE;
        ctrl = (ctrl & ~DMA_CTRL_RCYC_M) | ((env->rcycle - 1) << 8);
    } else {
        ctrl &= ~DMA_CTRL_RELE;
    }
    pcr = (env->express << 16) | env->notify;
    sqwc = (env->tqwc << 16) | env->sqwc;
    rbor = (u_int)env->rbadr;
    rbsr = env->rbmsk;
    DMA_CTRL = ctrl;
    DMA_PCR = pcr;
    DMA_SQWC = sqwc;
    DMA_RBOR = rbor;
    DMA_RBSR = rbsr;
    sceDmaCurrentEnv = *env;
    return 0;
}


sceDmaEnv *sceDmaGetEnv(sceDmaEnv *env)
{
    *env = sceDmaCurrentEnv;
    return env;
}


void *sceDmaPutStallAddr(void *addr)
{
    void *old = (void *)DMA_STADR;

    if (addr != (void *)-1) {
        DMA_STADR = (u_int)addr;
    }
    return old;
}


static inline void dma_wait(sceDmaChan *d, int timeout)
{
    DmaChcr chcr;

    while (d->chcr.word & DMA_CHCR_STR) {
        if (--timeout < 0) {
            printf("libdma: sync timeout\n");
            chcr = d->chcr.bits;
            while (chcr.str) {
                chcr.str = 0;
                d->chcr.bits = chcr;
            }
        }
    }
}


void sceDmaSend(sceDmaChan *d, void *tag)
{
    DmaChcr chcr;

    dma_wait(d, DMA_SYNC_TIMEOUT);
    if (d->tadr != (void *)-1) {
        d->tadr = tag;
    }
    chcr = d->chcr.bits;
    d->qwc = 0;
    chcr.mod = 1;
    chcr.dir = 1;
    chcr.str = 1;
    d->chcr.bits = chcr;
}


void sceDmaRecv(sceDmaChan *d)
{
    DmaChcr chcr;

    dma_wait(d, DMA_SYNC_TIMEOUT);
    chcr = d->chcr.bits;
    d->qwc = 0;
    chcr.mod = 1;
    chcr.dir = 0;
    chcr.str = 1;
    d->chcr.bits = chcr;
}


int sceDmaSync(sceDmaChan *d, int mode, int timeout)
{
    DmaChcr chcr;

    if (mode == 1) {
        return d->chcr.bits.str;
    }
    if (timeout == 0) {
        timeout = DMA_SYNC_TIMEOUT;
    }
    while (d->chcr.word & DMA_CHCR_STR) {
        if (--timeout < 0) {
            printf("libdma: sync timeout\n");
            chcr = d->chcr.bits;
            while (chcr.str) {
                chcr.str = 0;
                d->chcr.bits = chcr;
            }
        }
    }
    return 0;
}


u_int sceDmaPause(sceDmaChan *d)
{
    u_int chcr;

    if (DMA_ENABLER == 0) {
        DMA_ENABLEW = DMA_ENABLEW_PAUSE;
    }
    chcr = d->chcr.word;
    d->chcr.word = chcr & ~DMA_CHCR_STR;
    DMA_ENABLEW = 0;
    return chcr;
}


int sceDmaRestart(sceDmaChan *d, u_int chcr)
{
    DmaChcr old = d->chcr.bits;

    d->chcr.word = chcr;
    return old.str;
}
