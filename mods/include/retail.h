/* Retail symbols mod code uses; each is bound to its fixed address at
 * link time (gen_symbols.py). Grow as needed -- src/ is the ground truth
 * for signatures. Plain extern only: mod code is compiled -G0.
 *
 * Verify a global's SEMANTICS from the consuming asm before writing it,
 * not from its name: WUMPACOUNT is the live count of Wumpa[] entities
 * (blindly incrementing it made the engine walk past the array end and
 * corrupt memory at Crash's death); the HUD collected counter is
 * plr_wumpas, whose >=100 handling is a tolerant while-loop. */
#ifndef MODSDK_RETAIL_H
#define MODSDK_RETAIL_H

struct nuvec_s; /* defined in creature.h; only used by pointer below */
struct obj_s;   /* defined in creature.h; only used by pointer below */

extern int Level;
extern int PLAYERCOUNT;             /* 0/1 flag: player exists this level */
extern unsigned short plr_wumpas;   /* HUD wumpa counter, player 0 */
extern int Paused;                  /* 0 = running; ramps 1..0x19 while in-game paused */
extern int VEHICLECONTROL;          /* 0 on foot / 1 riding vehicle / 2 swimming */
extern float vtog_time;             /* vehicle mount-transition timer (== vtog_duration when complete) */
extern float vtog_duration;         /* mount-transition length */
extern int warp_level;              /* hub (Level 0x25) teleport target; -1 = not
                                     * warping. Set when Crash steps on a hub
                                     * teleporter, before the level loads. */
extern int in_finish_range;         /* ramps 1..0x32 while Crash stands in a
                                     * teleport zone (hub node / level exit);
                                     * 0 = not in a zone. Goes >0 at zone entry,
                                     * before warp_level commits. */
extern float in_finish_pos[3];      /* world position of the teleport node Crash
                                     * is in range of; set while in_finish_range>0.
                                     * The teleporter (JonProbe) is drawn here. */

/* Spawn the finite warp-debris effect at an object's mid-body (the "Crash
 * dissolves into sparks" teleport-out look). Fire-and-forget: the effect
 * self-animates in the game's debris pass. 0x00260CD8 */
void AddWarpDebris(struct obj_s *obj);

/* Play a game sound effect. pos != NULL routes through NuSoundPlay3d, which
 * spatialises + attenuates it by distance from the listener (so it is only
 * audible when in range); pos == NULL plays it non-positional. gamesfx_effect_volume
 * is the base volume (-1 = use the sfx's default); set it before the call, the game
 * resets it to -1 afterwards. SFX 0x1E is the warp/dissolve sound. 0x0024FA90 */
void GameSfx(int id, struct nuvec_s *pos);
extern int gamesfx_effect_volume;   /* 0x00632960 */

/* --- Teleporter prop (the spinning hub/level-exit ring) -----------------
 * The teleporter you see is NOT a placed object: JonProbe (0x1DB150) draws it
 * every frame from global probe state via Draw3DCharacter, using the model for
 * character 0xB1 (CModel[CRemap[0xB1]]). We reuse that same draw primitive to
 * render a teleporter at the puppet's position, with our own isolated state, so
 * it never touches the local player's singleton probe globals. CModel/CRemap
 * are declared in creature.h. */
#define COOP_TELEPORTER_CHAR 0xB1

/* Draw one character model at a world pos with Euler rotation. scale must be
 * != 0 (0 early-returns). action = -1 => static pose (no anim eval); rotorder
 * 0 => X,Y,Z. EABI: pos=a0, rot=a1..a3, scale=f12, anim_time=f13, model=t0,
 * action=t1, rotorder=t2. 0x001EE410 */
void Draw3DCharacter(struct nuvec_s *pos, int rotx, int roty, int rotz,
                     float scale, float anim_time, void *model,
                     int action, int rotorder);

void UpdateLevel(void);   /* per-frame, gameplay levels only */
void DoInput(void);       /* per-frame, menus included */

/* --- Progression state (Stage 3a: shared collectibles) -------------------
 * Layouts mirror src/game/game.c (the matched CalculateGamePercentage's own
 * TU) -- Game sits at 0x0058B108, hub[] at +0x10 (stride 4), level[] at
 * +0x28 (stride 0x1C), powerbits at +0x406. */
struct coop_hub_s {
    unsigned char flags;             /* +0 hub-unlock / boss-progress bits */
    unsigned char crystals;          /* +1 derived (CalculateGamePercentage) */
    char pad1;
    char pad2;
};                                   /* 4 */

struct coop_time_s {
    char name[4];
    unsigned int itime;
};                                   /* 8 */

struct coop_level_s {
    unsigned short flags;            /* +0 relics 1..7, crystal 8, gems
                                      * 0x10..0x400, opened 0x800 */
    char pad1;
    char pad2;
    struct coop_time_s time[3];
};                                   /* 0x1C */

struct coop_game_s {
    char name[9];
    unsigned char vibration;
    unsigned char surround;
    unsigned char sfx_volume;
    unsigned char music_volume;
    char screen_x;
    char screen_y;
    unsigned char language;
    struct coop_hub_s hub[6];        /* 0x10 */
    struct coop_level_s level[35];   /* 0x28 */
    unsigned char lives;             /* 0x3FC */
    unsigned char wumpas;
    unsigned char mask;
    unsigned char percent;           /* 0x3FF derived */
    unsigned char crystals;          /* 0x400 derived */
    unsigned char relics;            /* derived */
    unsigned char crate_gems;        /* derived */
    unsigned char bonus_gems;        /* derived */
    unsigned char gems;              /* derived */
    unsigned char gembits;           /* derived */
    unsigned char powerbits;         /* 0x406 boss power-ups */
    unsigned char empty;
    unsigned int cutbits;            /* 0x408; struct ends 0x40C */
};

extern struct coop_game_s Game;     /* 0x0058B108 the live profile */
extern int TimeTrial;               /* 0x00630D18 nonzero = time-trial run */
extern int Bonus;                   /* 0x00630D30 nonzero = in a bonus round */
extern unsigned short plr_items;    /* 0x00631168 current-level pickup bits
                                     * (crystal 1, crate gem 2, bonus gems...) */
extern struct obj_s *pObj[64];      /* 0x005A4FB8 placed-object slots; slot
                                     * order is deterministic per level load.
                                     * obj.dead (+0x145) nonzero = collected. */

/* Apply one placed item's pickup to the local game: full type dispatch
 * (crystal/gems/powers/relic/...) + KillItem despawn. Idempotent via
 * obj->dead. 0x001FCE88 */
void PickupItem(struct obj_s *obj);

/* Recompute EVERY derived tally -- percent, crystals (total and per hub),
 * relics, crate/bonus/colour gems, gembits -- purely from level[].flags.
 * Call after OR-merging remote flag words so a crystal earned by the peer
 * (in any level) shows up in the local hub UI and save. 0x001EADD8 */
void CalculateGamePercentage(struct coop_game_s *game);

/* --- Placed-object model table (VS crystal tint) --------------------------
 * ObjTab (0x00588580, creature.c objtab_s, stride 0x20) backs both the
 * in-world item draw (DrawCreatures -> Draw3DObject) and the pause-panel
 * carving (DrawPanel3DCharacter): character 0x75 -> entry 0x84 (crystal),
 * 0x77 -> 0x88 (crate gem), 0x78..0x7D -> 0x89..0x8E (coloured gems).
 * Both paths resolve the SAME graphics object:
 *   inst  = *(void **)(special + 0x40);
 *   gobj  = ((void **)*(void **)(scene + 0x14))[*(int *)(inst + 0x40)]
 * (Draw3DObject 0x1F0B40 / DrawPanel3DCharacter 0x23A130 -- byte-identical
 * chains), so one vertex tint recolours the level crystal AND its carving. */
struct coop_objtab_s {
    void *scene;                     /* +0x00 loaded GScene the model lives in */
    void *special;                   /* +0x04 per-object instance record */
    unsigned char pad08[0x18];       /* stride 0x20 */
};
extern struct coop_objtab_s ObjTab[]; /* 0x00588580, 201 entries
                                       * NOTE: this gobj is only the item's
                                       * GLOW; the crystal BODY is the
                                       * creature model CModel[CRemap[id]]
                                       * drawn by the regular DrawCreatures
                                       * path -- its colour is the material
                                       * DIFFUSE baked as VU1 float
                                       * constants in the state packets at
                                       * hobj+0x08 (see mod.c VS section). */

/* --- Crates (Stage 3b: per-crate destroyed-state sync) -------------------
 * Layouts mirror src/game/crate.c (matched BreakCrate's TU). Crate identity
 * across instances = the flat Crate[] slot index (fill order is
 * deterministic per level load; region 0x9000 => at most 256 slots).
 * Retail's own "destroyed" predicate (UpdatePlayerStats, panel.c):
 * (flags & 0x10) && (on == 0 || (newtype == 0xF && metal_count != 0));
 * plr_crates is re-derived from it every frame -- never sync counters. */
struct coop_crate_s {
    unsigned char pad00[0x30];
    signed char on;                  /* 0x30 0 = broken or inactive slot */
    unsigned char pad31[7];
    unsigned short flags;            /* 0x38 0x10 counts, 0x40 bonus-round */
    unsigned char pad3a[4];
    unsigned char newtype;           /* 0x3E 0xF = exploded */
    unsigned char pad3f[2];
    unsigned char metal_count;       /* 0x41 nonzero = destroyed by blast */
    unsigned char pad42[0x34];
    short armed;                     /* 0x76 -1 = idle; TNT/nitro countdown */
    unsigned char pad78[0x18];
};                                   /* 0x90 */

struct coop_crategroup_s {
    unsigned char pad00[0x10];
    short first;                     /* 0x10 index of first Crate[] slot */
    short count;                     /* 0x12 slots in this group */
    unsigned char pad14[0x1C];
};                                   /* 0x30 */

extern struct coop_crate_s Crate[];          /* 0x00592E18 */
extern struct coop_crategroup_s CrateGroup[];/* 0x0059BE18 */
extern int CRATEGROUPCOUNT;                  /* 0x00631218 */
extern int CRATECOUNT;                       /* 0x0063121C */

/* Break one crate through the full retail path: dispatches by type (TNT
 * 0x13 arms the countdown, nitro/nitro-switch chain via direct CrateOff
 * calls, default -> CrateOff), stack hops, sfx, debris. group must be the
 * crate's own CrateGroup entry (chains walk it). flags bit 9 is the only
 * bit CrateOff sees ((flags >> 9) & 1); 0 = plain break. 0x001F6FC0 */
void BreakCrate(struct coop_crategroup_s *group, struct coop_crate_s *crate,
                int type, int flags);

/* The single point every crate actually dies through (BreakCrate's default
 * path, nitro-switch chains, TNT explosions, stack chains). Returns nonzero
 * if the crate really went off -- the capture hook keys on that. Grants the
 * rewards (deferred screen-wumpa entities, ?-life panel debris). 0x001F3178 */
int CrateOff(struct coop_crategroup_s *group, struct coop_crate_s *crate,
             int a, int b);

/* Effective type of a crate slot (applies newtype/extra/time-trial
 * remaps; flags=0 for the plain query). 0x001F2DC8 */
int GetCrateType(struct coop_crate_s *crate, int flags);

/* Move the respawn point (called when a checkpoint crate opens, and with
 * (-1,-1,0,NULL) on level init). CrateOff's call site derives EVERY
 * argument from the crate (+0x31, +0x32, +0x34, &pos), so replaying a
 * remote checkpoint-crate break yields the identical checkpoint on both
 * sides -- checkpoints are shared by construction. 0x001E9E48 */
void ResetCheckpoint(int a, int b, float c, void *d);

/* Death respawn: restores the CrateTypeData snapshot + resets crates to the
 * last checkpoint. The coop hook lets it run, then arms the drop-intact
 * pass (resurrected crates leave the published bitmap -- the falling edge
 * the peer's shared death reset triggers on). 0x001F7CC8 */
int GotoCheckpoint(struct nuvec_s *pos, int dir);

/* The crate-state pair of retail's death block (0x1E4C4C..0x1E4C58) and
 * of the coop shared death reset: RestoreCrateTypeData un-does the
 * type-byte changes logged since the last checkpoint baseline and
 * self-clears the log (double reset = no-op); ResetCrates rebuilds every
 * crate's runtime state from the type bytes. The rest of the death block
 * (ResetWumpa/Chases/AI/PlayerEvents/...) is personal world state and is
 * NOT mirrored on the living player. */
void RestoreCrateTypeData(void);    /* 0x001F9010 */
void ResetCrates(void);             /* 0x001F4EE0 */

/* --- Hub award flight (the "crystal appears and flies to the pad" show) --
 * Retail's end-of-level celebration: back in the hub, the tumble animation
 * (action 0x56) spawns one Award per earned flag group via AddAward -- the
 * item rides above the player (stage 1), pops off (GameSfx 0x26 + debris
 * 0xA1), flies to the finished level's spline pad, and ON ARRIVAL
 * UpdateAwards commits Game.level[].flags |= bits + recomputes the tallies
 * + fires the arrival sparkle. The flying stage (stage 0) is fully
 * self-contained, so the coop mod replays the show from the PUPPET by
 * spawning with AddAward and flipping the slot straight to stage 0 with the
 * source at the puppet. Layout verified against AddAward/UpdateAwards asm
 * (0x001DBDC0 / 0x001DBF68). */
struct coop_award_s {
    float progress;          /* +0x00 flight progress 0..1 */
    short heading;           /* +0x04 flight yaw (NuAtan2D src->dest) */
    unsigned short bits;     /* +0x06 level-flag bits this award commits */
    signed char level;       /* +0x08 level the bits belong to */
    signed char stage;       /* +0x09 1 = held above player, 0 = flying */
    unsigned char pad[2];
    float src[3];            /* +0x0C flight source anchor */
    float fx[3];             /* +0x18 rising fx pos (y = src.y + 1.0) */
    float dest[3];           /* +0x24 the level pad spline point */
};                           /* 0x30 */

extern struct coop_award_s Award[3];  /* 0x0058B008; i_award cycles 0..2 */
extern int i_award;                   /* 0x00630B64 next Award slot */
extern int Hub;                       /* 0x00630B44 current hub, -1 = none */

struct coop_hdata_s {
    signed char level[6];    /* +0 the hub's level ids, -1 = empty slot */
    signed char i_spl[2];    /* +6 spline-set indices */
    unsigned char barrier;
    unsigned char i_gdeb;
    short sfx;
};                           /* 0xC */

extern struct coop_hdata_s HData[6];  /* 0x00586680 */

/* The LOCAL player's own pending-award bits (CheckFinish stashes its flag
 * delta here). NOT cleared at the tumble pop: the pop frame only ORs the
 * group into temp_lev_flags (creature.c AddAward call); new_lev_flags keeps
 * the bit until UpdateAwards XOR-clears it when the flight LANDS on the pad.
 * If a merged remote bit is already in here, the local player earned the
 * same thing simultaneously and their own award will commit it. 0x006310D2 */
extern unsigned short new_lev_flags;
/* Groups already popped off Crash this hub visit (OR-ed at the exact
 * AddAward frame of the tumble, zeroed by HubStart). new_lev_flags &
 * ~temp_lev_flags = "pending and not yet popped" -- the v9 pending_flags
 * payload, whose falling edge is the peer's frame-accurate pop cue.
 * 0x006310BE */
extern unsigned short temp_lev_flags;
extern int last_level;              /* 0x00630B98 level the pending award bits
                                     * belong to (the hub tumble's source) */

/* Queue the award flight for one flag GROUP (crystal 8, relics 7, gems
 * 0x10..0x400 -- one gotlist group per call, never mixed). hub picks the
 * spline set (must be the CURRENT hub and contain the level, or the pad
 * lookup goes stale). Returns 1 = queued at Award[old i_award], 0 = no
 * spline / bad args. Spawns at stage 1 (held above the LOCAL player) -- the
 * coop replay rewrites the slot to stage 0. 0x001DBDC0 */
int AddAward(int hub, int level, int bits);

/* Spawn a finite game-debris effect at pos; type 0xA1 is the award
 * pop/arrival sparkle UpdateAwards uses. 0x00260B60 */
void AddGameDebris(int type, struct nuvec_s *pos);

/* 2D text in normalized screen space (game HUD/menu font); align 0=left,
 * 8=centre, 2=right. x,y ~ -1..1, z = 1.0, sx/sy/sz = scale. 0x00238280 */
void Text3D(char *text, int align, int colour, float x, float y, float z,
            float sx, float sy, float sz);
/* Project count world points (nuvec, stride 12) to screen space through the
 * global screen matrix when mtx is NULL. 0x00114048 */
void NuCameraTransformScreen(struct nuvec_s *dest, struct nuvec_s *src,
                             int count, void *mtx);
/* Transform count world points (nuvec, stride 12) into CAMERA/view space
 * through the global view matrix when mtx is NULL (affine, no perspective
 * divide). dest.z is the signed depth along the camera axis: its sign is the
 * front/back test and its magnitude is the true perspective distance. 0x00113D18 */
void NuCameraTransformView(struct nuvec_s *dest, struct nuvec_s *src,
                           int count, void *mtx);

#endif
