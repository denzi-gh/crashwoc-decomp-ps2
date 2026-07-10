/* Coop payload inside the SDK mailbox (overlays ModsdkMailbox.payload,
 * absolute 0x706A60 with the default blob layout). This is the contract
 * the PC bridge (crashwoc-multiplayer repo) codes against -- bump
 * COOP_MAILBOX_VERSION on ANY layout change; the bridge refuses a
 * mismatch. EE ABI: int = 32-bit LE, short = 16, float = IEEE754 LE.
 *
 * Seq-lock, symmetric in both directions: the writer does
 *   gen++; seq_open = gen; <fields>; seq_close = gen;
 * and the reader reads seq_close, copies the fields, reads seq_open,
 * accepting only if both match and are nonzero (else it keeps the
 * previous snapshot). The bridge must write a slot as one ordered PINE
 * batch, seq_open first and seq_close last. */
#ifndef COOP_MAILBOX_H
#define COOP_MAILBOX_H

#define COOP_MAILBOX_MAGIC 0x4F435743u /* "CWCO" */
#define COOP_MAILBOX_VERSION 1u

#define COOP_F_PRESENT 1u /* writer is in a playable level, state valid */
#define COOP_F_DEAD 2u    /* writer's player is in a death state */

typedef struct CoopSlot {              /* 0x50 bytes */
    unsigned int seq_open;             /* 0x00 seq-lock bracket */
    unsigned int frame;                /* 0x04 writer tick, for staleness */
    int level;                         /* 0x08 Level global; -1 = not in level */
    unsigned int flags;                /* 0x0C COOP_F_* */
    float pos[3];                      /* 0x10 obj.pos */
    float mom[3];                      /* 0x1C obj.mom, for extrapolation */
    unsigned short hdg;                /* 0x28 facing yaw, 0..65535 */
    unsigned short xrot;               /* 0x2A */
    unsigned short yrot;               /* 0x2C */
    unsigned short zrot;               /* 0x2E */
    unsigned short surface_xrot;       /* 0x30 for future shadow support */
    unsigned short surface_zrot;       /* 0x32 */
    short action;                      /* 0x34 obj.anim.action */
    short character;                   /* 0x36 0 = Crash, 1 = Coco */
    float anim_time;                   /* 0x38 obj.anim.anim_time */
    float scale;                       /* 0x3C obj.SCALE */
    float shadow;                      /* 0x40 obj.shadow */
    unsigned int reserved[2];          /* 0x44 */
    unsigned int seq_close;            /* 0x4C seq-lock bracket */
} CoopSlot;

typedef struct CoopMailbox {           /* overlays ModsdkMailbox.payload */
    unsigned int magic;                /* +0x00 (abs 0x706A60) COOP_MAILBOX_MAGIC */
    unsigned int version;              /* +0x04 COOP_MAILBOX_VERSION */
    unsigned int reserved[2];          /* +0x08 */
    CoopSlot local;                    /* +0x10 (abs 0x706A70) game writes, bridge reads */
    CoopSlot remote;                   /* +0x60 (abs 0x706AC0) bridge writes, game reads */
} CoopMailbox;                         /* 0xB0 */

#endif
