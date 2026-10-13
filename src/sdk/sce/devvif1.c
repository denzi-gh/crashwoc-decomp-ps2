/*
 * Unit: sdk/sce/devvif1
 *
 * Functions:
 *   0x002641c0 sceDevVif1Reset
 *   0x002641e8 sceDevVif1Pause
 *   0x00264250 sceDevVif1Continue
 *   0x00264290 sceDevVif1PutErr
 *   0x002642b0 sceDevVif1GetErr
 *   0x002642c0 sceDevVif1Sync
 *   0x00264310 sceDevVif1GetCnd
 *   0x002644e8 sceDevVif1PutFifo
 *   0x00264548 sceDevVif1GetFifo
 */

typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;
typedef unsigned int u_long128 __attribute__((mode(TI)));

typedef struct {
    u_int row[4];
    u_int col[4];
    u_int mask;
    u_int code;
    u_int stat;
    u_short itop;
    u_short itops;
    u_short base;
    u_short offset;
    u_short top;
    u_short tops;
    u_short mark;
    u_short num;
    u_char error;
    u_char cl;
    u_char wl;
    u_char cmod;
} sceDevVif1Cnd;

#define VIF1_STAT   (*(volatile u_int *)0x10003C00)
#define VIF1_FBRST  (*(volatile u_int *)0x10003C10)
#define VIF1_ERR    (*(volatile u_int *)0x10003C20)
#define VIF1_MARK   (*(volatile u_int *)0x10003C30)
#define VIF1_CYCLE  (*(volatile u_int *)0x10003C40)
#define VIF1_MODE   (*(volatile u_int *)0x10003C50)
#define VIF1_NUM    (*(volatile u_int *)0x10003C60)
#define VIF1_MASK   (*(volatile u_int *)0x10003C70)
#define VIF1_CODE   (*(volatile u_int *)0x10003C80)
#define VIF1_ITOPS  (*(volatile u_int *)0x10003C90)
#define VIF1_BASE   (*(volatile u_int *)0x10003CA0)
#define VIF1_OFST   (*(volatile u_int *)0x10003CB0)
#define VIF1_TOPS   (*(volatile u_int *)0x10003CC0)
#define VIF1_ITOP   (*(volatile u_int *)0x10003CD0)
#define VIF1_TOP    (*(volatile u_int *)0x10003CE0)
#define VIF1_R0     (*(volatile u_int *)0x10003D00)
#define VIF1_R1     (*(volatile u_int *)0x10003D10)
#define VIF1_R2     (*(volatile u_int *)0x10003D20)
#define VIF1_R3     (*(volatile u_int *)0x10003D30)
#define VIF1_C0     (*(volatile u_int *)0x10003D40)
#define VIF1_C1     (*(volatile u_int *)0x10003D50)
#define VIF1_C2     (*(volatile u_int *)0x10003D60)
#define VIF1_C3     (*(volatile u_int *)0x10003D70)
#define VIF1_FIFO   (*(volatile u_long128 *)0x10005000)
#define VIF1_FIFO_IN (*(u_long128 *)0x10005000)

int sceDevVif1Sync(void);


void sceDevVif1Reset(void)
{
    VIF1_FBRST = 1;
    VIF1_ERR = 6;
}


int sceDevVif1Pause(int mode)
{
    int status = sceDevVif1Sync();

    if (status == 0) {
        return 0;
    }
    if (status == 1) {
        return 0;
    }
    if (mode == 0) {
        VIF1_FBRST = 2;
    } else if (mode == 1) {
        VIF1_FBRST = 4;
    }
    return 1;
}


int sceDevVif1Continue(void)
{
    if (sceDevVif1Sync() != 1) {
        return 0;
    }
    VIF1_FBRST = 8;
    return 1;
}


u_int sceDevVif1PutErr(int interrupt, int miss1, int miss2)
{
    u_int err = interrupt | (miss1 << 1) | (miss2 << 2);

    VIF1_ERR = err;
    return err;
}


u_int sceDevVif1GetErr(void)
{
    return VIF1_ERR;
}


int sceDevVif1Sync(void)
{
    u_int stat = VIF1_STAT;

    if (!(stat & 0x3)) {
        return 0;
    }
    if (stat & 0x700) {
        return 1;
    }
    if (stat & 0xC) {
        return 2;
    }
    if (stat & 0x3000) {
        return 3;
    }
    return 4;
}


int sceDevVif1GetCnd(sceDevVif1Cnd *cnd)
{
    u_int cycle = VIF1_CYCLE;
    u_int status = sceDevVif1Sync();

    if (status > 1) {
        return 0;
    }
    cnd->row[0] = VIF1_R0;
    cnd->row[1] = VIF1_R1;
    cnd->row[2] = VIF1_R2;
    cnd->row[3] = VIF1_R3;
    cnd->col[0] = VIF1_C0;
    cnd->col[1] = VIF1_C1;
    cnd->col[2] = VIF1_C2;
    cnd->col[3] = VIF1_C3;
    cnd->mask = VIF1_MASK;
    cnd->code = VIF1_CODE;
    cnd->stat = VIF1_STAT;
    cnd->itop = VIF1_ITOP;
    cnd->itops = VIF1_ITOPS;
    cnd->base = VIF1_BASE;
    cnd->offset = VIF1_OFST;
    cnd->top = VIF1_TOP;
    cnd->tops = VIF1_TOPS;
    cnd->mark = VIF1_MARK;
    cnd->num = VIF1_NUM;
    cnd->error = VIF1_ERR;
    cnd->cl = cycle;
    cnd->wl = cycle >> 8;
    cnd->cmod = VIF1_MODE;
    return 1;
}


int sceDevVif1PutFifo(u_long128 *addr, int n)
{
    int count = 0;

    while (n--) {
        if (VIF1_STAT & 0x10000000) {
            return count;
        }
        VIF1_FIFO_IN = *addr++;
        count++;
    }
    return count;
}


int sceDevVif1GetFifo(u_long128 *addr, int n)
{
    int count = 0;

    while (n--) {
        if (!(VIF1_STAT & 0x1F000000)) {
            return count;
        }
        *addr++ = VIF1_FIFO;
        count++;
    }
    return count;
}
