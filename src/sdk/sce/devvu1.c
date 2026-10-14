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
} Vu1RegData;

int sceVuCheckBusy(void);
void sceVu0MemWriteQ(u_short addr, void *data);
void sceVu0MemReadQ(void *data, u_short addr);
int sceDevVu1Sync(void);
void sceDevVu1Exec(u_short addr);
u_short sceDevVu1GetTpc(void);

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


u_short sceDevVu1GetTpc(void)
{
    u_short data[8];

    sceVu0MemReadQ(data, VU1_TPC_ADDR);
    return data[0];
}


int sceDevVu1GetCnd(sceDevVu1Cnd *cnd)
{
    Vu1RegData data[8];
    u_int i;

    if (sceVuCheckBusy()) {
        return 0;
    }
    for (i = 0; i < 32; i++) {
        sceVu0MemReadQ(&data[0], VU1_VF_ADDR + i);
        cnd->vf[i] = data[0].qword;
    }
    sceVu0MemReadQ(&data[0], VU1_STATUS_ADDR);
    cnd->status = data[0].word[0];
    sceVu0MemReadQ(&data[1], VU1_MAC_ADDR);
    cnd->mac = data[1].word[0];
    sceVu0MemReadQ(&data[2], VU1_CLIPPING_ADDR);
    cnd->clipping = data[2].word[0];
    sceVu0MemReadQ(&data[3], VU1_R_ADDR);
    cnd->r = data[3].word[0];
    sceVu0MemReadQ(&data[4], VU1_I_ADDR);
    cnd->i = data[4].word[0];
    sceVu0MemReadQ(&data[5], VU1_Q_ADDR);
    cnd->q = data[5].word[0];
    sceVu0MemReadQ(&data[6], VU1_P_ADDR);
    cnd->p = data[6].word[0];
    for (i = 0; i < 16; i++) {
        sceVu0MemReadQ(&data[7], VU1_VI_ADDR + i);
        cnd->vi[i] = data[7].word[0];
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
        sceVu0MemWriteQ(VU1_VF_ADDR + i, &data);
    }
    data.qword = 0;
    data.dword[0] = cnd->status;
    sceVu0MemWriteQ(VU1_STATUS_ADDR, &data);
    data.dword[0] = cnd->mac;
    sceVu0MemWriteQ(VU1_MAC_ADDR, &data);
    data.dword[0] = cnd->clipping;
    sceVu0MemWriteQ(VU1_CLIPPING_ADDR, &data);
    data.dword[0] = cnd->r;
    sceVu0MemWriteQ(VU1_R_ADDR, &data);
    data.dword[0] = cnd->i;
    sceVu0MemWriteQ(VU1_I_ADDR, &data);
    data.dword[0] = cnd->q;
    sceVu0MemWriteQ(VU1_Q_ADDR, &data);
    data.dword[0] = cnd->p;
    sceVu0MemWriteQ(VU1_P_ADDR, &data);
    for (i = 0; i < 16; i++) {
        data.dword[0] = cnd->vi[i];
        sceVu0MemWriteQ(VU1_VI_ADDR + i, &data);
    }
    return 1;
}
