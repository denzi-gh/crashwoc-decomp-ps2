#ifndef NUPS2_PS2CTL_H
#define NUPS2_PS2CTL_H

#include "nu_types.h"

/* One controller read as scePadRead returns it. */
struct nupadframe_s {
    u8 ok;                    /* 0x00 */
    u8 read_id;               /* 0x01 */
    u8 buttons_hi;            /* 0x02 */
    u8 buttons_lo;            /* 0x03 */
    u8 r_alg_x;               /* 0x04 */
    u8 r_alg_y;               /* 0x05 */
    u8 l_alg_x;               /* 0x06 */
    u8 l_alg_y;               /* 0x07 */
    u8 l_algpad_r;            /* 0x08 */
    u8 l_algpad_l;            /* 0x09 */
    u8 l_algpad_u;            /* 0x0A */
    u8 l_algpad_d;            /* 0x0B */
    u8 r_algpad_u;            /* 0x0C */
    u8 r_algpad_r;            /* 0x0D */
    u8 r_algpad_d;            /* 0x0E */
    u8 r_algpad_l;            /* 0x0F */
    u8 l1_alg;                /* 0x10 */
    u8 r1_alg;                /* 0x11 */
    u8 l2_alg;                /* 0x12 */
    u8 r2_alg;                /* 0x13 */
}; /* 0x14 */

/* One controller port. The u128 DMA buffer makes the struct 16-aligned, so
 * the flag bit-fields are accessed as a doubleword. */
struct nupad_s {
    u128 dmabuf[0x50];        /* 0x000 */
    s32 port;                 /* 0x500 */
    s32 slot;                 /* 0x504 */
    s32 stage;                /* 0x508 */
    s32 id;                   /* 0x50C */
    s32 ldx;                  /* 0x510 */
    s32 ldy;                  /* 0x514 */
    s32 rdx;                  /* 0x518 */
    s32 rdy;                  /* 0x51C */
    f32 l_nx;                 /* 0x520 */
    f32 l_ny;                 /* 0x524 */
    f32 l_rx;                 /* 0x528 */
    f32 l_ry;                 /* 0x52C */
    f32 l_dx;                 /* 0x530 */
    f32 l_dy;                 /* 0x534 */
    f32 r_nx;                 /* 0x538 */
    f32 r_ny;                 /* 0x53C */
    f32 r_rx;                 /* 0x540 */
    f32 r_ry;                 /* 0x544 */
    f32 r_dx;                 /* 0x548 */
    f32 r_dy;                 /* 0x54C */
    u8 actdirect[6];          /* 0x550 */
    u8 actalign[6];           /* 0x556 */
    u32 paddata;              /* 0x55C */
    u32 oldpaddata;           /* 0x560 */
    u32 paddata_db;           /* 0x564 */
    int analogue_sticks : 1;  /* 0x568 */
    int analogue_buttons : 1;
    int valid : 1;
    int deadzone : 1;
    int : 28;
    u8 pad56c[0xC];           /* 0x56C */
    struct nupadframe_s rdata; /* 0x578 */
    u8 pad58c[0x14];          /* 0x58C */
}; /* 0x5A0 */

/* Pad recording / demo playback buffer. */
struct nupadrec_s {
    s32 frame;                /* 0x00 */
    s32 mode;                 /* 0x04 */
    s32 nframes;              /* 0x08 */
    s32 maxframes;            /* 0x0C */
    struct nupadframe_s data[1]; /* 0x10 */
}; /* 0x24 */

s32 DeadZoneValue(s32 value);
s32 NuPs2ReadPad(struct nupad_s *pad);
void NuPs2ClosePad(struct nupad_s *pad);
struct nupad_s *NuPs2OpenPad(s32 port, s32 slot);
s32 NuPs2ApplyDeadZone(s32 value, s32 deadzone);
void InitPadPlayRecord(char *file, s32 mode, s32 count, struct nupadrec_s *buffer);
void SavePadRecord(char *file);
s32 PadRecPtr(void);
s32 NuPs2PadDemoEnd(void);
void NuPs2PadSetMotors(struct nupad_s *pad, s32 small, s32 big);
void PadPlayOff(void);

#endif /* NUPS2_PS2CTL_H */
