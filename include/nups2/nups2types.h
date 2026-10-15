#ifndef NUPS2_NUPS2TYPES_H
#define NUPS2_NUPS2TYPES_H

#include "nu_types.h"

/* DMA source chain tag, with the two VIF codes that travel in its upper half. */
struct DmaTag {
    union {
        struct {
            u16 qwc;          /* 0x00 */
            u8 pad;           /* 0x02 */
            u8 id;            /* 0x03 */
        };
        u32 ctrl;             /* 0x00 */
    };
    u32 addr;                 /* 0x04 */
    u32 vifcode[2];           /* 0x08 */
} __attribute__((aligned(16))); /* 0x10 */

struct GifTag {
    u64 nloop : 15;
    u64 eop : 1;
    u64 pad16 : 30;
    u64 pre : 1;
    u64 prim : 11;
    u64 flg : 2;
    u64 nreg : 4;
    u64 regs;                 /* 0x08 */
}; /* 0x10 */

struct GsZbufReg {
    u64 zbp : 9;
    u64 pad09 : 15;
    u64 psm : 4;
    u64 pad1c : 4;
    u64 zmsk : 1;
    u64 pad21 : 31;
}; /* 0x08 */

/* GIF A+D pair: a GS register value followed by its register address. */
struct NuPs2GsReg {
    union {
        u64 data;             /* 0x00 */
        struct GsZbufReg zbuf; /* 0x00 */
    };
    u64 addr;                 /* 0x08 */
}; /* 0x10 */

/* Memory card directory entry as sceMcGetDir fills it (libmc sceMcTblGetDir). */
struct mcdirent_s {
    u8 pad00[0x20];           /* 0x00 */
    char name[32];            /* 0x20 */
}; /* 0x40 */

#endif /* NUPS2_NUPS2TYPES_H */
