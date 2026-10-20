/*
 * Unit: sdk/sce/devvu0
 *
 * Functions:
 *   0x00263528 sceDevVu0CheckBusy
 *   0x00263540 sceDevVu0Reset
 *   0x00263558 sceDevVu0Pause
 *   0x002635b8 sceDevVu0Continue
 *   0x00263608 sceDevVu0PutDBit
 *   0x00263638 sceDevVu0PutTBit
 *   0x00263668 sceDevVu0GetDBit
 *   0x00263678 sceDevVu0GetTBit
 *   0x00263688 sceDevVu0Sync
 *   0x002636c8 sceDevVu0Exec
 *   0x002636d8 sceDevVu0GetTpc
 *   0x002636e8 sceDevVu0GetCnd
 *   0x002637c8 sceDevVu0PutCnd
 */


#define CFC2(dst, reg) __asm__ ("cfc2 %0, $" #reg : "=r" (dst))
#define CTC2(src, reg) __asm__ volatile ("ctc2 %0, $" #reg : : "r" (src))
#define SQC2(reg, offset, base) __asm__ volatile ("sqc2 $" #reg ", " #offset "(%0)" : : "r" (base))

#define FBRST_FORCE_BREAK 0x1
#define FBRST_RESET 0x2
#define FBRST_DBIT 0x4
#define FBRST_TBIT 0x8

#define STAT_BUSY 0x1
#define STAT_DBIT_STOP 0x2
#define STAT_TBIT_STOP 0x4
#define STAT_FORCE_BREAK 0x8

typedef struct {
    unsigned int vf[32][4];
    int status;
    int mac;
    int clipping;
    int r;
    int i;
    int q;
    unsigned short vi[16];
} sceDevVu0Cnd;

int sceDevVu0Sync(void);
void sceDevVu0Exec(unsigned short addr);
unsigned short sceDevVu0GetTpc(void);


int sceDevVu0CheckBusy(void)
{
    int stat;

    CFC2(stat, vi29);
    return (stat & 0xf) == STAT_BUSY;
}


void sceDevVu0Reset(void)
{
    int fbrst;

    CFC2(fbrst, vi28);
    CTC2(fbrst | FBRST_RESET, vi28);
}


int sceDevVu0Pause(void)
{
    int paused = 0;
    int fbrst;

    switch (sceDevVu0Sync()) {
    case 4:
        CFC2(fbrst, vi28);
        CTC2(fbrst | FBRST_FORCE_BREAK, vi28);
    case 0:
    case 3:
        paused = 1;
        break;
    case 1:
    case 2:
        break;
    }
    return paused;
}


int sceDevVu0Continue(void)
{
    int resumed = 0;

    switch (sceDevVu0Sync()) {
    case 1:
    case 2:
    case 3:
        resumed = 1;
        sceDevVu0Exec(sceDevVu0GetTpc());
        break;
    }
    return resumed;
}


void sceDevVu0PutDBit(int enable)
{
    unsigned int fbrst;

    CFC2(fbrst, vi28);
    if (enable == 1) {
        fbrst |= FBRST_DBIT;
    } else {
        fbrst &= ~FBRST_DBIT;
    }
    CTC2(fbrst, vi28);
}


void sceDevVu0PutTBit(int enable)
{
    unsigned int fbrst;

    CFC2(fbrst, vi28);
    if (enable == 1) {
        fbrst |= FBRST_TBIT;
    } else {
        fbrst &= ~FBRST_TBIT;
    }
    CTC2(fbrst, vi28);
}


unsigned int sceDevVu0GetDBit(void)
{
    unsigned int fbrst;

    CFC2(fbrst, vi28);
    return (fbrst >> 2) & 1;
}


unsigned int sceDevVu0GetTBit(void)
{
    unsigned int fbrst;

    CFC2(fbrst, vi28);
    return (fbrst >> 3) & 1;
}


int sceDevVu0Sync(void)
{
    int stat;

    CFC2(stat, vi29);
    if (stat & STAT_DBIT_STOP) {
        return 1;
    }
    if (stat & STAT_TBIT_STOP) {
        return 2;
    }
    if (stat & STAT_FORCE_BREAK) {
        return 3;
    }
    return (stat & STAT_BUSY) << 2;
}


void sceDevVu0Exec(unsigned short addr)
{
    CTC2(addr, vi27);
    __asm__ volatile ("vcallmsr $vi27");
}


unsigned short sceDevVu0GetTpc(void)
{
    int tpc;

    CFC2(tpc, vi26);
    return tpc;
}


int sceDevVu0GetCnd(sceDevVu0Cnd *cnd)
{
    int reg;

    if (sceDevVu0CheckBusy()) {
        return 0;
    }
    CFC2(reg, vi16);
    cnd->status = reg;
    CFC2(reg, vi17);
    cnd->mac = reg;
    CFC2(reg, vi18);
    cnd->clipping = reg;
    CFC2(reg, vi20);
    cnd->r = reg;
    CFC2(reg, vi21);
    cnd->i = reg;
    CFC2(reg, vi22);
    cnd->q = reg;
    CFC2(reg, vi0);
    cnd->vi[0] = reg;
    CFC2(reg, vi1);
    cnd->vi[1] = reg;
    CFC2(reg, vi2);
    cnd->vi[2] = reg;
    CFC2(reg, vi3);
    cnd->vi[3] = reg;
    CFC2(reg, vi4);
    cnd->vi[4] = reg;
    CFC2(reg, vi5);
    cnd->vi[5] = reg;
    CFC2(reg, vi6);
    cnd->vi[6] = reg;
    CFC2(reg, vi7);
    cnd->vi[7] = reg;
    CFC2(reg, vi8);
    cnd->vi[8] = reg;
    CFC2(reg, vi9);
    cnd->vi[9] = reg;
    CFC2(reg, vi10);
    cnd->vi[10] = reg;
    CFC2(reg, vi11);
    cnd->vi[11] = reg;
    CFC2(reg, vi12);
    cnd->vi[12] = reg;
    CFC2(reg, vi13);
    cnd->vi[13] = reg;
    CFC2(reg, vi14);
    cnd->vi[14] = reg;
    CFC2(reg, vi15);
    cnd->vi[15] = reg;
    return 1;
}


int sceDevVu0PutCnd(sceDevVu0Cnd *cnd)
{
    unsigned int (*vf)[4] = cnd->vf;
    int reg;

    if (sceDevVu0CheckBusy()) {
        return 0;
    }
    SQC2(vf0, 0x0, vf);
    SQC2(vf1, 0x10, vf);
    SQC2(vf2, 0x20, vf);
    SQC2(vf3, 0x30, vf);
    SQC2(vf4, 0x40, vf);
    SQC2(vf5, 0x50, vf);
    SQC2(vf6, 0x60, vf);
    SQC2(vf7, 0x70, vf);
    SQC2(vf8, 0x80, vf);
    SQC2(vf9, 0x90, vf);
    SQC2(vf10, 0xa0, vf);
    SQC2(vf11, 0xb0, vf);
    SQC2(vf12, 0xc0, vf);
    SQC2(vf13, 0xd0, vf);
    SQC2(vf14, 0xe0, vf);
    SQC2(vf15, 0xf0, vf);
    SQC2(vf16, 0x100, vf);
    SQC2(vf17, 0x110, vf);
    SQC2(vf18, 0x120, vf);
    SQC2(vf19, 0x130, vf);
    SQC2(vf20, 0x140, vf);
    SQC2(vf21, 0x150, vf);
    SQC2(vf22, 0x160, vf);
    SQC2(vf23, 0x170, vf);
    SQC2(vf24, 0x180, vf);
    SQC2(vf25, 0x190, vf);
    SQC2(vf26, 0x1a0, vf);
    SQC2(vf27, 0x1b0, vf);
    SQC2(vf28, 0x1c0, vf);
    SQC2(vf29, 0x1d0, vf);
    SQC2(vf30, 0x1e0, vf);
    SQC2(vf31, 0x1f0, vf);
    reg = cnd->status;
    CTC2(reg, vi16);
    reg = cnd->mac;
    CTC2(reg, vi17);
    reg = cnd->clipping;
    CTC2(reg, vi18);
    reg = cnd->r;
    CTC2(reg, vi20);
    reg = cnd->i;
    CTC2(reg, vi21);
    reg = cnd->q;
    CTC2(reg, vi22);
    reg = cnd->vi[0];
    CTC2(reg, vi0);
    reg = cnd->vi[1];
    CTC2(reg, vi1);
    reg = cnd->vi[2];
    CTC2(reg, vi2);
    reg = cnd->vi[3];
    CTC2(reg, vi3);
    reg = cnd->vi[4];
    CTC2(reg, vi4);
    reg = cnd->vi[5];
    CTC2(reg, vi5);
    reg = cnd->vi[6];
    CTC2(reg, vi6);
    reg = cnd->vi[7];
    CTC2(reg, vi7);
    reg = cnd->vi[8];
    CTC2(reg, vi8);
    reg = cnd->vi[9];
    CTC2(reg, vi9);
    reg = cnd->vi[10];
    CTC2(reg, vi10);
    reg = cnd->vi[11];
    CTC2(reg, vi11);
    reg = cnd->vi[12];
    CTC2(reg, vi12);
    reg = cnd->vi[13];
    CTC2(reg, vi13);
    reg = cnd->vi[14];
    CTC2(reg, vi14);
    reg = cnd->vi[15];
    CTC2(reg, vi15);
    return 1;
}
