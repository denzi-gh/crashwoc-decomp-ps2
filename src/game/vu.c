/*
 * Unit: game/vu
 *
 * Functions:
 *   0x00261078 sceGsResetPath
 *   0x00261118 sceGsSyncPath
 */

#include "nu_types.h"

#define GIF_CTRL    (*(volatile u32 *)0x10003000)
#define GIF_STAT    (*(volatile u32 *)0x10003020)
#define VIF1_STAT   (*(volatile u32 *)0x10003C00)
#define VIF1_FBRST  (*(volatile u32 *)0x10003C10)
#define VIF1_ERR    (*(volatile u32 *)0x10003C20)
#define VIF1_FIFO   (*(volatile u128 *)0x10005000)
#define D1_CHCR     (*(volatile u32 *)0x10009000)
#define D2_CHCR     (*(volatile u32 *)0x1000A000)

extern u128 D_00610820;
extern u128 D_00610830;

#define vif1_reset_cycle_mask D_00610820
#define vif1_reset_base_itop  D_00610830


void sceGsResetPath(void)
{
    VIF1_FBRST = 1;
    VIF1_ERR = 2;
    __asm__ volatile (
        "daddu $4, $0, $0\n\t"
        "sync.l\n\t"
        "cfc2 $4, $vi28\n\t"
        "nop\n\t"
        "ori $4, $4, 0x200\n\t"
        "ctc2 $4, $vi28\n\t"
        "sync.p\n"
        "1:\n\t"
        "cfc2 $4, $vi29\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "andi $4, $4, 0x100\n\t"
        "bnez $4, 1b\n\t"
        "li $4, 0x404\n\t"
        "ctc2 $4, $vi28\n\t"
        "nop"
        : : : "$4");
    VIF1_FIFO = vif1_reset_cycle_mask;
    VIF1_FIFO = vif1_reset_base_itop;
    GIF_CTRL = 1;
}


int sceGsSyncPath(int mode, u16 timeout)
{
    int busy;
    u32 vu1_stat;

    do {
        if (D1_CHCR & 0x100) {
            busy = 1;
        } else {
            busy = 0;
        }
        if (D2_CHCR & 0x100) {
            busy |= 2;
        }
        if (VIF1_STAT & 3) {
            busy |= 4;
        }
        __asm__ volatile ("cfc2 %0, $vi29\n\tnop" : "=r"(vu1_stat));
        if (vu1_stat & 0x100) {
            busy |= 8;
        }
        if (GIF_STAT & 0xC00) {
            busy |= 0x10;
        }
    } while (busy);
    return 0;
}
