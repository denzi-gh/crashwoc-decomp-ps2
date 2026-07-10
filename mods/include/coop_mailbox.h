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
#define COOP_MAILBOX_VERSION 2u

#define COOP_F_PRESENT 1u /* writer is in a playable level, state valid */
#define COOP_F_DEAD 2u    /* writer's player is in a death state */

/* CoopMailbox.ctl bits (PC writes, game reads). */
#define COOP_CTL_GHOST 1u /* self-test: mirror the local player into the
                           * remote snapshot at pos.x + 2.0 -- a ghost
                           * puppet shadows the player with no bridge */

typedef struct CoopSlot {              /* 0x60 bytes */
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
    /* --- v2: sub-states DrawCreatures needs to reproduce spin + bazooka --- */
    short target_xrot;                 /* 0x44 obj.target_xrot, aim pitch */
    short target_yrot;                 /* 0x46 obj.target_yrot, aim yaw */
    short spin_frame;                  /* 0x48 spin_frame (spin progress) */
    short spin_frames;                 /* 0x4A spin_frames (spin length) */
    signed char spin;                  /* 0x4C spin != 0 -> spinning */
    signed char dangle;                /* 0x4D obj.dangle (spin variant) */
    signed char target;                /* 0x4E target != 0 -> aiming bazooka */
    unsigned char fire;                /* 0x4F fire countdown */
    unsigned char freeze;              /* 0x50 freeze (aim-lock facing) */
    unsigned char pad[3];              /* 0x51 alignment */
    unsigned int reserved[2];          /* 0x54 */
    unsigned int seq_close;            /* 0x5C seq-lock bracket */
} CoopSlot;

typedef struct CoopMailbox {           /* overlays ModsdkMailbox.payload */
    unsigned int magic;                /* +0x00 (abs 0x706A60) COOP_MAILBOX_MAGIC */
    unsigned int version;              /* +0x04 COOP_MAILBOX_VERSION */
    unsigned int ctl;                  /* +0x08 COOP_CTL_* (PC writes) */
    unsigned int diag;                 /* +0x0C game writes: bit 0 = puppet
                                        * shown, bits 8+ = puppet re-inits */
    CoopSlot local;                    /* +0x10 (abs 0x706A70) game writes, bridge reads */
    CoopSlot remote;                   /* +0x70 (abs 0x706AD0) bridge writes, game reads */
} CoopMailbox;                         /* 0xD0 */

#endif
