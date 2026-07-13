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
#define COOP_MAILBOX_VERSION 9u

/* Sizes shared with the v8 progression fields below. */
#define COOP_LEVEL_COUNT 35   /* Game.level[] entries */
#define COOP_HUB_COUNT 6      /* Game.hub[] entries */
#define COOP_CRATE_WORDS 8    /* 256 Crate[] slots / 32 */
#define COOP_ITEM_WORDS 2     /* 64 pObj[] slots / 32 */

#define COOP_F_PRESENT 1u /* writer is in a playable level, state valid */
#define COOP_F_DEAD 2u    /* writer's player is in a death state */

/* CoopMailbox.ctl bits (PC writes, game reads). Adding BITS is not a
 * layout change -- no version bump; an old mod ignores unknown bits. */
#define COOP_CTL_GHOST 1u /* self-test: mirror the local player into the
                           * remote snapshot at pos.x + 2.0 -- a ghost
                           * puppet shadows the player with no bridge */
#define COOP_CTL_VS 2u    /* session is VS mode: the mod attributes pickups
                           * (mine vs peer) and tints the crystal + HUD
                           * carving in the owner's player colour */
#define COOP_CTL_P2 4u    /* this instance is player 2 (red); unset = player
                           * 1 (blue). Identity comes from which bridge
                           * endpoint (--p1/--p2-endpoint) we are. */

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
    /* --- v6: teleporter-appears-on-zone-entry timing --- */
    int in_finish_range;               /* 0x88 remote's in_finish_range: ramps
                                        * 1..0x32 the instant it enters a hub
                                        * node's teleport zone (JonProbe's gate),
                                        * BEFORE warp_level commits. 0 = not in a
                                        * zone. Drives the puppet teleporter's
                                        * appearance at the right moment. */
    /* --- v7: teleporter position = the node, not the puppet's entry point --- */
    float in_finish_pos[3];            /* 0x8C remote's in_finish_pos: the exact
                                        * teleport NODE world position (the spline
                                        * point HubLevelSelect slides Crash to),
                                        * set every frame in the zone. The puppet's
                                        * teleporter is placed here so it lands on
                                        * the pad instead of where the puppet first
                                        * crossed the zone edge. Valid only while
                                        * in_finish_range > 0. */
    /* --- v8: shared collectibles & progression (Stage 3a). All progression
     * state is ABSOLUTE (bitmaps / OR-able flag words), never deltas: the
     * transport has no reliable delivery, so latest-wins + OR-merge is the
     * whole consistency model. The reader ORs the remote's words into its
     * own game state and re-derives every tally with
     * CalculateGamePercentage; re-applying an already-applied bit is a
     * no-op by construction. */
    unsigned char bonus;               /* 0x98 Bonus != 0: writer is in a bonus
                                        * round -- freezes item/crate capture
                                        * and apply in BOTH directions (bonus
                                        * progress stays personal). */
    unsigned char pad1;                /* 0x99 */
    unsigned short items;              /* 0x9A plr_items: current-level pickup
                                        * bits (crystal 1, crate gem 2, bonus
                                        * gems...). OR-merged into the peer's
                                        * plr_items while both are in the same
                                        * level and neither is in a bonus. */
    unsigned short level_flags[COOP_LEVEL_COUNT]; /* 0x9C Game.level[i].flags:
                                        * committed per-level progression
                                        * (crystal 8, gems 0x10..0x400, relics
                                        * 1..7, opened 0x800). OR-merged ALWAYS
                                        * (cross-level: a crystal earned by the
                                        * remote shows in the local hub UI via
                                        * the CalculateGamePercentage recompute
                                        * even while the players are apart). */
    unsigned char powers;              /* 0xE2 Game.powerbits (+0x406): boss
                                        * power-ups. OR-merged always. */
    unsigned char hub_flags[COOP_HUB_COUNT]; /* 0xE3 Game.hub[i].flags:
                                        * hub-unlock / boss-progress bits --
                                        * the one commit product NOT derivable
                                        * from level_flags. OR-merged always. */
    unsigned char pad2[3];             /* 0xE9 */
    unsigned int crate_bits[COOP_CRATE_WORDS]; /* 0xEC broken-crate bitmap by
                                        * flat Crate[] slot index (bit i = slot
                                        * i broken by the writer this level).
                                        * Reserved by v8; the crate capture /
                                        * apply hooks land in Stage 3b so one
                                        * version bump covers both. */
    unsigned int item_bits[COOP_ITEM_WORDS]; /* 0x10C RESERVED (always 0).
                                        * Was a taken-item bitmap by pObj[]
                                        * slot index; abandoned 2026-07-12 --
                                        * pObj fills as objects stream in, so
                                        * slot order diverges between
                                        * instances and the replay hit the
                                        * wrong object. Item replay is now
                                        * identity-based off the items/powers
                                        * words (each reward bit maps to one
                                        * object character). */
    /* --- v9: hub award timing. pending_flags = the writer's end-of-level
     * award bits that are pending AND not yet popped: new_lev_flags (set at
     * level finish, only XOR-cleared when the award flight LANDS) masked by
     * ~temp_lev_flags (the groups the hub tumble already popped off Crash --
     * OR-ed at the exact AddAward frame). A falling edge here is therefore
     * the frame-accurate pop cue for the peer to spawn the same award flight
     * on the puppet -- both new_lev_flags alone (falls at landing) and the
     * committed level_flags bit (v8's only signal) are ~2 s late.
     * pending_level = last_level, the level those bits belong to. */
    unsigned short pending_flags;      /* 0x114 new_lev_flags & ~temp_lev_flags */
    short pending_level;               /* 0x116 writer's last_level; -1 = none */
    unsigned int seq_close;            /* 0x118 seq-lock bracket */
} CoopSlot;                            /* 0x11C */

typedef struct CoopMailbox {           /* overlays ModsdkMailbox.payload */
    unsigned int magic;                /* +0x00 (abs 0x706A60) COOP_MAILBOX_MAGIC */
    unsigned int version;              /* +0x04 COOP_MAILBOX_VERSION */
    unsigned int ctl;                  /* +0x08 COOP_CTL_* (PC writes) */
    unsigned int diag;                 /* +0x0C game writes: bit 0 = puppet
                                        * shown, bit 1 = VS glow tint
                                        * active, bit 2 = VS tint wanted but
                                        * the glow packets have no V4-8
                                        * colour block, bit 3 = VS on but the
                                        * ObjTab[0x84] gobj chain is broken
                                        * (scene not streamed / no crystal
                                        * model), bit 4 = VS body diffuse
                                        * tint active, bit 5 = VS hub
                                        * stone-HUD tint active,
                                        * bits 8+ = re-inits */
    CoopSlot local;                    /* +0x10 (abs 0x706A70) game writes, bridge reads */
    CoopSlot remote;                   /* +0x12C (abs 0x706B8C) bridge writes, game reads */
} CoopMailbox;                         /* 0x248 */

#endif
