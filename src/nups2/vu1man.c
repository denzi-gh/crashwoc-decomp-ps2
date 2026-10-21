/*
 * Unit: nups2/vu1man
 *
 * Functions:
 *   0x0016b7a0 NuPs2InitVU
 *   0x0016b7f8 VU1_DisplayFRegs
 *   0x0016b8c8 VU1_DisplaySRegs
 *   0x0016b948 VU1_LoadXD
 */

#include "nu_types.h"

#include "nucore/nuerror.h"

extern int FlushCache(int operation);
extern void VU1_DmaWrite(void *tag, int wait);

extern char D_00281530[];
extern char D_002834E0[];

#define fpatch_microcode    D_00281530
#define rndr3d_l_microcode  D_002834E0

#define VU1_VF_BASE 0x400
#define VU1_VI_BASE 0x420


inline void VU1_DisplayFRegs(void)
{
    int i;
    f32 vf[4] __attribute__((aligned(16)));

    __asm__ volatile ("ctc2 %0, $vi1" : : "r"(VU1_VF_BASE));
    for (i = 0; i < 32; i++) {
        __asm__ volatile ("vlqi.xyzw $vf2, ($vi1++)");
        __asm__ volatile ("sqc2 $vf2, %0" : "=m"(vf));
        NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\vu1man.c", 0x12)("vf%02d:%f %f %f %f", i, vf[0], vf[1], vf[2], vf[3]);
    }
}


inline void VU1_DisplaySRegs(void)
{
    int i;
    u32 vi[4] __attribute__((aligned(16)));

    __asm__ volatile ("ctc2 %0, $vi1" : : "r"(VU1_VI_BASE));
    for (i = 0; i < 16; i++) {
        __asm__ volatile ("vlqi.xyzw $vf2, ($vi1++)");
        __asm__ volatile ("sqc2 $vf2, %0" : "=m"(vi));
        NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\vu1man.c", 0x25)("vs%02d : %08x %08x %08x %08x", i, vi[0], vi[1], vi[2], vi[3]);
    }
}


inline void VU1_LoadXD(void)
{
    FlushCache(0);
    VU1_DmaWrite(fpatch_microcode, 1);
    VU1_DmaWrite(rndr3d_l_microcode, 1);
    NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\vu1man.c", 0x38)("Loaded default mpg");
}


void NuPs2InitVU(void)
{
    VU1_LoadXD();
}
