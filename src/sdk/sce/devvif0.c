/*
 * Unit: sdk/sce/devvif0
 *
 * Functions:
 *   0x00263e90 sceDevVif0Reset
 *   0x00263eb8 sceDevVif0Pause
 *   0x00263f20 sceDevVif0Continue
 *   0x00263f60 sceDevVif0PutErr
 *   0x00263f80 sceDevVif0GetErr
 *   0x00263f90 sceDevVif0Sync
 *   0x00263fe0 sceDevVif0GetCnd
 *   0x00264160 sceDevVif0PutFifo
 */

typedef unsigned int u_int;
typedef unsigned short u_short;
typedef unsigned char u_char;
typedef unsigned int u_long128 __attribute__((mode(TI)));

typedef struct {
    u_int row[4];
    u_int col[4];
    u_int mask;
    u_int code;
    u_int stat;
    u_short itop;
    u_short itops;
    u_short mark;
    u_short num;
    u_char err;
    u_char cl;
    u_char wl;
    u_char cmod;
} sceDevVif0Cnd;

#define VIF0_STAT  (*(volatile u_int *)0x10003800)
#define VIF0_FBRST (*(volatile u_int *)0x10003810)
#define VIF0_ERR   (*(volatile u_int *)0x10003820)
#define VIF0_MARK  (*(volatile u_int *)0x10003830)
#define VIF0_CYCLE (*(volatile u_int *)0x10003840)
#define VIF0_MODE  (*(volatile u_int *)0x10003850)
#define VIF0_NUM   (*(volatile u_int *)0x10003860)
#define VIF0_MASK  (*(volatile u_int *)0x10003870)
#define VIF0_CODE  (*(volatile u_int *)0x10003880)
#define VIF0_ITOPS (*(volatile u_int *)0x10003890)
#define VIF0_ITOP  (*(volatile u_int *)0x100038d0)
#define VIF0_R0    (*(volatile u_int *)0x10003900)
#define VIF0_R1    (*(volatile u_int *)0x10003910)
#define VIF0_R2    (*(volatile u_int *)0x10003920)
#define VIF0_R3    (*(volatile u_int *)0x10003930)
#define VIF0_C0    (*(volatile u_int *)0x10003940)
#define VIF0_C1    (*(volatile u_int *)0x10003950)
#define VIF0_C2    (*(volatile u_int *)0x10003960)
#define VIF0_C3    (*(volatile u_int *)0x10003970)
#define VIF0_FIFO  (*(volatile u_long128 *)0x10004000)

#define VIF_FBRST_RST 0x1
#define VIF_FBRST_FBK 0x2
#define VIF_FBRST_STP 0x4
#define VIF_FBRST_STC 0x8

#define VIF_STAT_FIFO_BUSY 0x10000000

u_int sceDevVif0Sync(void);


void sceDevVif0Reset(void)
{
    VIF0_FBRST = VIF_FBRST_RST;
    VIF0_ERR = 6;
}


int sceDevVif0Pause(int mode)
{
    u_int status = sceDevVif0Sync();

    if (status == 0) {
        return 0;
    }
    if (status == 1) {
        return 0;
    }
    if (mode == 0) {
        VIF0_FBRST = VIF_FBRST_FBK;
    } else if (mode == 1) {
        VIF0_FBRST = VIF_FBRST_STP;
    }
    return 1;
}


int sceDevVif0Continue(void)
{
    if (sceDevVif0Sync() != 1) {
        return 0;
    }
    VIF0_FBRST = VIF_FBRST_STC;
    return 1;
}


u_int sceDevVif0PutErr(int interrupt, int miss1, int miss2)
{
    u_int err = interrupt | (miss1 << 1) | (miss2 << 2);

    VIF0_ERR = err;
    return err;
}


u_int sceDevVif0GetErr(void)
{
    return VIF0_ERR;
}


u_int sceDevVif0Sync(void)
{
    u_int stat = VIF0_STAT;

    if (!(stat & 0x3)) {
        return 0;
    }
    if (stat & 0x700) {
        return 1;
    }
    if (stat & 0xc) {
        return 2;
    }
    if (stat & 0x3000) {
        return 3;
    }
    return 4;
}


int sceDevVif0GetCnd(sceDevVif0Cnd *cnd)
{
    u_int cycle = VIF0_CYCLE;

    if (sceDevVif0Sync() > 1) {
        return 0;
    }
    cnd->row[0] = VIF0_R0;
    cnd->row[1] = VIF0_R1;
    cnd->row[2] = VIF0_R2;
    cnd->row[3] = VIF0_R3;
    cnd->col[0] = VIF0_C0;
    cnd->col[1] = VIF0_C1;
    cnd->col[2] = VIF0_C2;
    cnd->col[3] = VIF0_C3;
    cnd->mask = VIF0_MASK;
    cnd->code = VIF0_CODE;
    cnd->stat = VIF0_STAT;
    cnd->itop = VIF0_ITOP;
    cnd->itops = VIF0_ITOPS;
    cnd->mark = VIF0_MARK;
    cnd->num = VIF0_NUM;
    cnd->err = VIF0_ERR;
    cnd->cl = cycle;
    cnd->wl = cycle >> 8;
    cnd->cmod = VIF0_MODE;
    return 1;
}


int sceDevVif0PutFifo(u_long128 *data, int count)
{
    int sent = 0;

    while (count--) {
        if (VIF0_STAT & VIF_STAT_FIFO_BUSY) {
            return sent;
        }
        VIF0_FIFO = *data++;
        sent++;
    }
    return sent;
}
