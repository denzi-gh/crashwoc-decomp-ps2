/*
 * Unit: sdk/sce/devvu1
 *
 * Functions:
 *   0x00263938 sceVuCheckBusy
 *   0x00263968 sceVu0MemWriteQ
 *   0x002639a8 sceVu0MemReadQ
 *   0x002639f0 sceDevVu1Reset
 *   0x00263a08 sceDevVu1Pause
 *   0x00263a68 sceDevVu1Continue
 *   0x00263ab8 sceDevVu1PutDBit
 *   0x00263ae8 sceDevVu1PutTBit
 *   0x00263b18 sceDevVu1GetDBit
 *   0x00263b28 sceDevVu1GetTBit
 *   0x00263b38 sceDevVu1Sync
 *   0x00263b80 sceDevVu1Exec
 *   0x00263b90 sceDevVu1GetTpc
 *   0x00263bb8 sceDevVu1GetCnd
 *   0x00263d48 sceDevVu1PutCnd
 */

typedef unsigned int u_int;
typedef unsigned short u_short;
typedef unsigned long u_long;
typedef unsigned int u_long128 __attribute__((mode(TI)));

typedef struct {
    u_long128 vf[32];
    u_int status;
    u_int mac;
    u_int clipping;
    u_int r;
    u_int i;
    u_int q;
    u_int p;
    u_short vi[16];
} sceDevVu1Cnd;

typedef union {
    u_long128 qword;
    u_long dword[2];
    u_int word[4];
    u_short hword[8];
} Vu1RegData;

int sceVuCheckBusy(void);
void sceVu0MemWriteQ(u_short addr, Vu1RegData data);
Vu1RegData sceVu0MemReadQ(u_short addr);
int sceDevVu1Sync(void);
void sceDevVu1Exec(u_short addr);
u_short sceDevVu1GetTpc(void);

#define CFC2(value, reg) __asm__ volatile ("cfc2 %0, " reg : "=r" (value))
#define CTC2(value, reg) __asm__ volatile ("ctc2 %0, " reg : : "r" (value))

#define VU_FBRST "$vi28"
#define VU_VPU_STAT "$vi29"
#define VU_CMSAR1 "$vi31"

#define VU1_VF_ADDR       0x400 /* VU1 VF00..VF31 */
#define VU1_VI_ADDR       0x420 /* VU1 VI00..VI15 */
#define VU1_STATUS_ADDR   0x430
#define VU1_MAC_ADDR      0x431
#define VU1_CLIPPING_ADDR 0x432
#define VU1_R_ADDR        0x434
#define VU1_I_ADDR        0x435
#define VU1_Q_ADDR        0x436
#define VU1_P_ADDR        0x437
#define VU1_TPC_ADDR      0x43A /* VU1 TPC, mapped into VU0 register space */



int sceVuCheckBusy(void)
{
    u_int stat;

    CFC2(stat, VU_VPU_STAT);
    if ((stat & 0xF) == 1) {
        return 1;
    }
    if ((stat & 0xF00) == 0x100) {
        return 1;
    }
    return 0;
}


void sceVu0MemWriteQ(u_short addr, Vu1RegData data)
{
    u_int saved_vi1;
    u_long128 saved_vf1;

    __asm__ ("cfc2 %0, $vi1" : "=r" (saved_vi1));
    __asm__ ("qmfc2 %0, $vf1" : "=r" (saved_vf1));
    __asm__ volatile ("qmtc2 %0, $vf1" : : "r" (data.qword));
    CTC2(addr, "$vi1");
    __asm__ volatile ("vsqi.xyzw $vf1, ($vi1++)");
    CTC2(saved_vi1, "$vi1");
    __asm__ volatile ("qmtc2 %0, $vf1" : : "r" (saved_vf1));
}


Vu1RegData sceVu0MemReadQ(u_short addr)
{
    Vu1RegData data;
    u_int saved_vi1;
    u_long128 saved_vf1;
    u_long128 value;

    __asm__ ("cfc2 %0, $vi1" : "=r" (saved_vi1));
    __asm__ ("qmfc2 %0, $vf1" : "=r" (saved_vf1));
    CTC2(addr, "$vi1");
    __asm__ volatile ("vlqi.xyzw $vf1, ($vi1++)");
    __asm__ volatile ("qmfc2 %0, $vf1" : "=r" (value));
    CTC2(saved_vi1, "$vi1");
    __asm__ volatile ("qmtc2 %0, $vf1" : : "r" (saved_vf1));
    data.qword = value;
    return data;
}


void sceDevVu1Reset(void)
{
    u_int fbrst;

    CFC2(fbrst, VU_FBRST);
    CTC2(fbrst | 0x200, VU_FBRST);
}


int sceDevVu1Pause(void)
{
    int paused = 0;
    u_int fbrst;

    switch (sceDevVu1Sync()) {
    case 4:
        CFC2(fbrst, VU_FBRST);
        CTC2(fbrst | 0x100, VU_FBRST);
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

int sceDevVu1Continue(void)
{
    int resumed = 0;

    switch (sceDevVu1Sync()) {
    case 1:
    case 2:
    case 3:
        resumed = 1;
        sceDevVu1Exec(sceDevVu1GetTpc());
        break;
    }
    return resumed;
}



void sceDevVu1PutDBit(int bit)
{
    u_int fbrst;

    CFC2(fbrst, VU_FBRST);
    if (bit == 1) {
        fbrst |= 0x400;
    } else {
        fbrst &= ~0x400;
    }
    CTC2(fbrst, VU_FBRST);
}


void sceDevVu1PutTBit(int bit)
{
    u_int fbrst;

    CFC2(fbrst, VU_FBRST);
    if (bit == 1) {
        fbrst |= 0x800;
    } else {
        fbrst &= ~0x800;
    }
    CTC2(fbrst, VU_FBRST);
}


int sceDevVu1GetDBit(void)
{
    u_int fbrst;

    CFC2(fbrst, VU_FBRST);
    return (fbrst >> 10) & 1;
}


int sceDevVu1GetTBit(void)
{
    u_int fbrst;

    CFC2(fbrst, VU_FBRST);
    return (fbrst >> 11) & 1;
}


int sceDevVu1Sync(void)
{
    u_int stat;
    int state;

    CFC2(stat, VU_VPU_STAT);
    if (stat & 0x200) {
        state = 1;
    } else if (stat & 0x400) {
        state = 2;
    } else if (stat & 0x800) {
        state = 3;
    } else if (stat & 0x100) {
        state = 4;
    } else {
        state = 0;
    }
    return state;
}


void sceDevVu1Exec(u_short addr)
{
    CTC2(addr, VU_CMSAR1);
}

u_short sceDevVu1GetTpc(void)
{
    return sceVu0MemReadQ(VU1_TPC_ADDR).hword[0];
}


int sceDevVu1GetCnd(sceDevVu1Cnd *cnd)
{
    u_int i;

    if (sceVuCheckBusy()) {
        return 0;
    }
    for (i = 0; i < 32; i++) {
        cnd->vf[i] = sceVu0MemReadQ(VU1_VF_ADDR + i).qword;
    }
    cnd->status = sceVu0MemReadQ(VU1_STATUS_ADDR).word[0];
    cnd->mac = sceVu0MemReadQ(VU1_MAC_ADDR).word[0];
    cnd->clipping = sceVu0MemReadQ(VU1_CLIPPING_ADDR).word[0];
    cnd->r = sceVu0MemReadQ(VU1_R_ADDR).word[0];
    cnd->i = sceVu0MemReadQ(VU1_I_ADDR).word[0];
    cnd->q = sceVu0MemReadQ(VU1_Q_ADDR).word[0];
    cnd->p = sceVu0MemReadQ(VU1_P_ADDR).word[0];
    for (i = 0; i < 16; i++) {
        cnd->vi[i] = sceVu0MemReadQ(VU1_VI_ADDR + i).word[0];
    }
    return 1;
}


int sceDevVu1PutCnd(sceDevVu1Cnd *cnd)
{
    Vu1RegData data;
    u_int i;

    if (sceVuCheckBusy()) {
        return 0;
    }
    for (i = 0; i < 32; i++) {
        data.qword = cnd->vf[i];
        sceVu0MemWriteQ(VU1_VF_ADDR + i, data);
    }
    data.qword = 0;
    data.dword[0] = cnd->status;
    sceVu0MemWriteQ(VU1_STATUS_ADDR, data);
    data.dword[0] = cnd->mac;
    sceVu0MemWriteQ(VU1_MAC_ADDR, data);
    data.dword[0] = cnd->clipping;
    sceVu0MemWriteQ(VU1_CLIPPING_ADDR, data);
    data.dword[0] = cnd->r;
    sceVu0MemWriteQ(VU1_R_ADDR, data);
    data.dword[0] = cnd->i;
    sceVu0MemWriteQ(VU1_I_ADDR, data);
    data.dword[0] = cnd->q;
    sceVu0MemWriteQ(VU1_Q_ADDR, data);
    data.dword[0] = cnd->p;
    sceVu0MemWriteQ(VU1_P_ADDR, data);
    for (i = 0; i < 16; i++) {
        data.dword[0] = cnd->vi[i];
        sceVu0MemWriteQ(VU1_VI_ADDR + i, data);
    }
    return 1;
}
