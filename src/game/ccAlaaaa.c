/*
 * Unit: game/ccAlaaaa
 *
 * Data only (no functions): the preprocessed temp file of vu/vu.c's build
 * (same .text address as game/vu), which owns sceGsResetPath's two VIF1
 * reset quadwords. game/vu reads them as vif1_reset_cycle_mask and
 * vif1_reset_base_itop; the retail names are the compiler's function-static
 * labels.
 */

#include "nu_types.h"

/* STCYCL 4,4; STMASK; mask 0; STMOD 0 */
static u32 q1[4] __asm__("q1.3") = {
    0x01000404, 0x20000000, 0x00000000, 0x05000000
};

/* MSKPATH3 0; BASE 0; OFFSET 0; ITOP 0 */
static u32 q2[4] __asm__("q2.4") = {
    0x06000000, 0x03000000, 0x02000000, 0x04000000
};
