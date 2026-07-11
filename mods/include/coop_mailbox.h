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
#define COOP_MAILBOX_VERSION 5u

#define COOP_F_PRESENT 1u /* writer is in a playable level, state valid */
#define COOP_F_DEAD 2u    /* writer's player is in a death state */

/* CoopMailbox.ctl bits (PC writes, game reads). */
#define COOP_CTL_GHOST 1u /* self-test: mirror the local player into the
                           * remote snapshot at pos.x + 2.0 -- a ghost
                           * puppet shadows the player with no bridge */

typedef struct CoopSlot {              /* 0x70 bytes */
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
    /* --- v3: presence polish (names, paused, vehicle, mask) --- */
    unsigned char paused;              /* 0x51 Paused != 0 -> in-game paused */
    unsigned char mask_active;         /* 0x52 Mask.active: 0/1/2, >2 = invincible */
    unsigned char vehiclecontrol;      /* 0x53 VEHICLECONTROL: 0 foot/1 vehicle/2 swim */
    short vehicle;                     /* 0x54 obj.vehicle model id, -1 = none */
    unsigned char pad0[2];             /* 0x56 alignment */
    char name[16];                     /* 0x58 remote player name, NUL-terminated
                                        * (PC-supplied; the game never writes it) */
    /* --- v4: special-vehicle transform so the puppet's glider banks / ball
     * spins like the remote's, instead of borrowing the local player's Buggy.
     * Packed per mode (glider and atlas are mutually exclusive):
     *   glider: [0]=pitch [1]=roll [2]=yaw  [3..5]=Buggy.pos  [6] unused
     *   atlas : [0..2]=ball_pos             [3..6]=rotquat (x,y,z,w)          */
    float vehicle_xf[7];               /* 0x68 NEWBUGGY transform (see above) */
    /* --- v5: hub-teleport warp-out effect on the puppet --- */
    int warp_level;                    /* 0x84 remote's warp_level (hub teleport
                                        * target); -1 = not warping. Lets the
                                        * puppet play AddWarpDebris as it warps
                                        * out of the hub instead of vanishing. */
    unsigned int reserved2;            /* 0x88 */
    unsigned int seq_close;            /* 0x8C seq-lock bracket */
} CoopSlot;                            /* 0x90 */

typedef struct CoopMailbox {           /* overlays ModsdkMailbox.payload */
    unsigned int magic;                /* +0x00 (abs 0x706A60) COOP_MAILBOX_MAGIC */
    unsigned int version;              /* +0x04 COOP_MAILBOX_VERSION */
    unsigned int ctl;                  /* +0x08 COOP_CTL_* (PC writes) */
    unsigned int diag;                 /* +0x0C game writes: bit 0 = puppet
                                        * shown, bits 8+ = puppet re-inits */
    CoopSlot local;                    /* +0x10 (abs 0x706A70) game writes, bridge reads */
    CoopSlot remote;                   /* +0xA0 (abs 0x706B00) bridge writes, game reads */
} CoopMailbox;                         /* 0x130 */

#endif
