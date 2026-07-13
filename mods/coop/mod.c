/* Coop: per-frame state exchange through the SDK mailbox plus a
 * remote-player puppet.
 * publish: local player state -> CoopMailbox.local (bridge reads it).
 * consume: CoopMailbox.remote (bridge writes it) -> remote_snap.
 * puppet:  a mod-owned creature record driven from remote_snap and drawn
 *          by a replace hook on DrawCreatures right after the player pass.
 *          It is never registered with the engine (no AddGameObject), so
 *          it has no physics, AI, or collisions -- pure visuals.
 * The seq-lock protocol is documented in coop_mailbox.h. */
#include "mailbox.h"
#include "retail.h"
#include "creature.h"
#include "coop_mailbox.h"

/* Not in the mirrored creature.h (they live outside unit 91). */
void ResetLights(struct Nearest_Light_s *lights);
void GetLights(struct nuvec_s *pos, struct Nearest_Light_s *lights, s32 mode);
extern struct MoveInfo CrashMoveInfo;
extern CharacterData CData[];
extern s32 USELIGHTS;
extern s32 LIGHTCREATURES;
/* Aku Aku on the puppet. NewMask(mask,pos) initialises a mask_s (character 3 /
 * model / anim / lights); DrawMask(mask_s*) renders it at its own matrices (only
 * its ground shadow reads the global player, which we bracket). Both NewMask and
 * UpdateMask are declared in creature.h. */
void DrawMask(struct mask_s *mask);

/* orig_ thunks the SDK provides for the replace hooks. */
void orig_DrawCreatures(struct creature_s *c, s32 count, s32 render,
                        s32 shadow);
void orig_DrawMenu(void *cursor, s32 paused);
int orig_CrateOff(struct coop_crategroup_s *group, struct coop_crate_s *crate,
                  int a, int b);
int orig_GotoCheckpoint(struct nuvec_s *pos, int dir);
void orig_PickupItem(struct obj_s *obj);

#define COOP ((volatile CoopMailbox *)modsdk_mailbox.payload)

/* Level ids that are not playable rooms: game-over and the FMV players.
 * The hub IS Level 0x25 (PINE-verified 2026-07-10; retail agrees --
 * DrawCreatures uses HUBREFLECTY when Level == 0x25), and the title
 * menus are filtered by PLAYERCOUNT == 0, not by a level id. */
#define LEVEL_PLAYABLE(l) \
    ((l) != 0x26 && (l) != 0x27 && (l) != 0x29)

/* Remote peer must have ticked within this many local frames or the
 * puppet hides (bridge gone, peer paused, savestate...). ~0.6 s at 50 Hz. */
#define STALE_LIMIT 30

static CoopSlot remote_snap;         /* last accepted remote state */
static unsigned int remote_gen;      /* seq of remote_snap */
static unsigned int remote_frame;    /* last seen remote frame tick */
static unsigned int stale_frames;    /* frames since remote_frame advanced */
static unsigned int local_gen;       /* our seq-lock generation */
static unsigned int local_frame;     /* our tick, mirrored into the slot */
static unsigned int published_frame; /* SDK frame we last published on */

static struct creature_s g_puppet;   /* the remote player's stand-in */
static int g_puppet_active;          /* show rule result, handler reads it */
static int g_puppet_level;           /* level the puppet was placed in */
static short g_puppet_char;          /* character the record was built for */
static unsigned int g_puppet_inits;  /* diagnostic: re-init count */
static int g_warp_latched;           /* dissolve debris fired for this warp (commit) */
static int g_tp_latched;             /* teleporter armed for this zone entry */
static int g_tp_active;              /* teleporter ring should draw this frame */
static struct nuvec_s g_tp_pos;      /* latched node position (where puppet warps) */
static unsigned short g_tp_rot;      /* teleporter spin angle (u16 anglespace) */
static int g_tp_frame;               /* frames since warp start (bare-ring path) */
static unsigned int g_tp_frame_drawn; /* SDK frame the beam last ran (once/frame guard) */
static int g_tpin_active;            /* warp-IN (arrival) teleporter active */
static struct nuvec_s g_tpin_pos;    /* arrival point (where the puppet materialises) */
static int g_tpin_frame;             /* frames since arrival */
static int g_tpin_hide;              /* this arrival hides the body (materialise) vs
                                      * appears immediately (returning to hub tumble) */
static int g_last_remote_room = -1;  /* last real remote room (level or hub, ignoring
                                      * -1 loading/menu); tells a hub RETURN from a
                                      * fresh hub spawn */
static int g_warp_out_hide;          /* level-exit: hide the body so it despawns with
                                      * the warp debris (hub uses the warp_level bracket) */

/* --- Stage 3a: shared collectibles & progression state ------------------
 * Everything here is per-level and ABSOLUTE (bitmaps), reset on Level
 * change; the persistent progression (Game.level[].flags etc.) needs no
 * bookkeeping because OR-merging into Game is idempotent. */
/* Item identity is the object's CHARACTER code, not a pObj[] slot: pObj
 * fills as objects stream in, so slot order differs between instances
 * whose players explored differently (in-game bug 2026-07-12: a crystal
 * pickup replayed by slot index picked the peer's CRATE GEM). PickupItem's
 * own dispatch maps every character to exactly one reward bit, so
 * remote items/powers words + the live object list identify what to
 * replay unambiguously -- no publish-side capture needed at all. */
static unsigned int g_crate_bits[COOP_CRATE_WORDS];  /* Crate[] slots that went off
                                      * HERE (own breaks + applied echoes --
                                      * the echo makes the bitmaps converge
                                      * from either side) */
static unsigned int g_crate_applied[COOP_CRATE_WORDS]; /* remote bits we already
                                      * broke locally (or found already
                                      * destroyed); a bit is only meaningful
                                      * while the remote still publishes it
                                      * (re-armed on its falling edge) */
static unsigned int g_crate_remote_prev[COOP_CRATE_WORDS]; /* remote crate bits
                                      * last processed tick: bits FALLING out
                                      * = the remote's death respawn just
                                      * resurrected them (shared death reset
                                      * cue) */
static int g_crate_reconcile;        /* ticks left in the post-GotoCheckpoint
                                      * settle window; the drop-intact pass
                                      * runs when it hits 0 */
static int g_sync_level = -1;        /* level the bitmaps belong to */
static int g_remote_prev_room = -1;  /* the remote's PREVIOUS distinct room:
                                      * when its published level changes X->Y
                                      * this holds X. The award celebration only
                                      * fires for bits of the level the remote
                                      * just came from, which is what separates
                                      * a fresh finish (celebrate) from an
                                      * initial profile catch-up (silent). */
static unsigned short g_award_pending[COOP_LEVEL_COUNT]; /* merged bits handed
                                      * to an award flight; committed by
                                      * UpdateAwards on pad arrival (retail's
                                      * own commit), so the level pad/tallies
                                      * update exactly when the crystal lands
                                      * instead of seconds early */
static int g_award_deadline;         /* ticks until pending is force-flushed
                                      * (award lost mid-flight safety net) */
static unsigned short g_prev_pending_flags; /* remote pending_flags last tick --
                                      * a bit FALLING out of it is the exact
                                      * frame the award pops off the remote's
                                      * Crash (v9 edge cue) */
static int g_prev_pending_level = -1; /* remote pending_level last tick */
/* Nonzero while we replay a remote pickup/break through the real game
 * functions. Phase 1 uses it to tell echo captures apart (diagnostics);
 * Stage 3b's ResetCheckpoint suppress-hook and the future asymmetric
 * reward mode key off the same flag. */
static int g_coop_applying;

static int local_valid(void)
{
    return LEVEL_PLAYABLE(Level) && PLAYERCOUNT != 0 && player != 0 &&
           player->used != 0;
}

/* PickupItem's dispatch (game_obj.c), inverted: the plr_items bit an
 * object of this character grants. 0 = not a shareable item; 0x76 (the
 * time-trial clock) must NEVER be replayed -- it starts a time trial. */
static unsigned int coop_item_bit(short character)
{
    switch (character) {
    case 0x75: return 0x1;  /* crystal */
    case 0x77: return 0x2;  /* crate gem */
    case 0x78: return 0x4;  /* coloured gems, per PickupItem's gembit map */
    case 0x79: return 0x8;
    case 0x7A: return 0x20;
    case 0x7B: return 0x10;
    case 0x7C: return 0x40;
    case 0x7D: return 0x80;
    }
    return 0;
}

/* Same inversion for the power items (Game.powerbits bit). */
static unsigned int coop_power_bit(short character)
{
    switch (character) {
    case 0xA7: return 1u << 0;
    case 0xA5: return 1u << 1;
    case 0xA6: return 1u << 2;
    case 0xA2: return 1u << 3;
    case 0xA4: return 1u << 4;
    case 0xA3: return 1u << 5;
    }
    return 0;
}

/* --- VS mode (bridge --vs): pickup attribution + player colours ----------
 * The bridge sets COOP_CTL_VS on both instances and COOP_CTL_P2 on the
 * second endpoint; gameplay stays fully shared (one crystal, the race is
 * who touches it first). Each side derives ownership locally and both
 * agree, because a first-hand pickup marks g_vs_mine synchronously in the
 * PickupItem hook while the peer's claim can only arrive later through the
 * bridge (remote items bit not already ours). A genuine same-frame tie
 * leaves each side believing it won -- cosmetic only; the bridge's stats
 * file serializes the observations and is the authority. */
static int g_vs_mode;                /* COOP->ctl & COOP_CTL_VS, read each tick */
static int g_vs_p2;                  /* COOP->ctl & COOP_CTL_P2: we are player 2 */
static unsigned short g_vs_mine;     /* this level's plr_items bits WE claimed */
static unsigned short g_vs_peer;     /* ... the remote claimed (first) */

/* Owner of a plr_items bit: 0 = player 1, 1 = player 2, -1 = unclaimed. */
static int coop_vs_owner(unsigned int bit)
{
    if ((g_vs_mine & bit) != 0) {
        return g_vs_p2 ? 1 : 0;
    }
    if ((g_vs_peer & bit) != 0) {
        return g_vs_p2 ? 0 : 1;
    }
    return -1;
}

/* Replace hook on PickupItem (0x1FCE88): attribution only -- every pickup
 * passes through here (HitItems touch + our own merge replay), and the
 * g_coop_applying bracket tells a first-hand touch from a replayed echo. */
void coop_pickup_item(struct obj_s *obj)
{
    if (g_vs_mode && g_coop_applying == 0 && Bonus == 0) {
        g_vs_mine |= (unsigned short)coop_item_bit(obj->character);
    }
    orig_PickupItem(obj);
}

/* --- VS crystal tint ------------------------------------------------------
 * The crystal is TWO renders with different colour sources (both probed
 * in-game via PINE, 2026-07-13):
 *
 * 1. The GLOW -- ObjTab[0x84]'s gobj, billboarded at the camera by
 *    Draw3DObject (and drawn again as the pause-panel carving by
 *    DrawPanel3DCharacter via the byte-identical chain, see retail.h).
 *    Unlit; its colours are baked per vertex inside the prebuilt DMA/VIF
 *    render packets as UNPACK V4-8 payloads (R in the low byte -- retail's
 *    purple is 72 00 6D). Retail never builds the crossfade colour-ref
 *    descriptors (the shipped scenes carry prebuilt streams, which skip
 *    the runtime stream builder where nugscn_generate_colourref would
 *    matter), so the mod parses the packets itself: walk the loaded geom
 *    nodes {next, material, packet} at gobj+0xC, scan each packet's VIF
 *    stream, and collect every V4-8 payload word. Originals are kept for
 *    exact restore/retint.
 *
 * 2. The BODY -- the creature model CModel[CRemap[0x75]], drawn by the
 *    normal DrawCreatures model path right after the glow. A LIT gouraud
 *    model: its baked vertex colours are BLACK; the pink is texture x
 *    c->lights. Items are not in Character[], so nothing ever recomputes
 *    their light record -- the DrawCreatures hook overwrites the colour
 *    fields (the puppet's apply_gray_lights trick) with the player
 *    colour before passing through. */
#define COOP_VS_CRYSTAL_OBJ 0x84   /* ObjTab entry of the glow gobj */
#define COOP_VS_CRYSTAL_CHAR 0x75  /* the crystal creature's character id */
#define COOP_TINT_SAVE_MAX 256     /* saved V4-8 colour words (glow uses ~51) */

/* EE main RAM sanity for every pointer we chase out of retail data. */
#define COOP_PTR_OK(p) \
    ((unsigned int)(p) >= 0x00100000u && (unsigned int)(p) < 0x02000000u)

/* Player colours: P1 blue, P2 red (picked 2026-07-12, tune freely).
 * Byte RGB for the glow's vertex colours ... */
static const unsigned char coop_vs_colour[2][3] = {
    { 64, 128, 255 },  /* player 1: blue */
    { 255, 64, 48 },   /* player 2: red */
};
/* ... and float RGB for the body's light tint (flat ambient, directionals
 * zeroed, like the puppet gray). In-game finding 2026-07-13: the body's
 * MAIN surface is texture-only (DECAL -- neither lights nor its all-black
 * V4-8 vertex bytes move it; recolouring it means patching the texture
 * palette, a follow-up), but the alpha REFLECTION overlay is lit and takes
 * this colour -- so the body keeps its pink with clearly coloured facets,
 * while glow + carving carry the strong player colour. */
static const float coop_vs_light[2][3] = {
    { 0.15f, 0.50f, 1.60f },  /* player 1: blue */
    { 1.80f, 0.20f, 0.15f },  /* player 2: red */
};

static int g_vs_want = -1;     /* colour index to show this tick; -1 = none */
static int g_tint_level = -1;  /* level the saved colour words belong to */
static int g_tint_shown = -2;  /* colour index written: -2 = untinted */
static unsigned int g_vs_diag; /* bit 1 = tint active, bit 2 = packets have
                                * no V4-8 colour block, bit 3 = gobj chain
                                * not resolvable (scene still streaming) */
static struct {
    unsigned int *addr;        /* live V4-8 colour word in the packet */
    unsigned int orig;         /* its baked value (restore/retint source) */
} g_tint_save[COOP_TINT_SAVE_MAX];
static int g_tint_save_n;

/* The gobj behind a placed-object ObjTab entry (NULL until the level's
 * scene has streamed in; every hop is range-checked because we walk live
 * retail heap structures). */
static unsigned int *coop_item_gobj(int o)
{
    unsigned char *scene = (unsigned char *)ObjTab[o].scene;
    unsigned char *special = (unsigned char *)ObjTab[o].special;
    unsigned char *inst;
    unsigned char *table;
    unsigned int *gobj;
    int idx;

    if (!COOP_PTR_OK(scene) || !COOP_PTR_OK(special)) {
        return 0;
    }
    inst = *(unsigned char **)(special + 0x40);
    if (!COOP_PTR_OK(inst)) {
        return 0;
    }
    idx = *(int *)(inst + 0x40);
    table = *(unsigned char **)(scene + 0x14);
    if (!COOP_PTR_OK(table) || idx < 0 || idx >= 0x1000) {
        return 0;
    }
    gobj = *(unsigned int **)(table + idx * 4);
    return COOP_PTR_OK(gobj) ? gobj : 0;
}

/* Scan one prebuilt DMA/VIF geometry packet and record every UNPACK V4-8
 * payload word (the per-vertex colours) into g_tint_save. The walk mirrors
 * what the DMAC/VIF1 would do: 16-byte DMA tag (2 VIF words ride in its
 * upper half), then QWC quadwords of VIF codes + data; only cnt-chains
 * continue to a following tag. Unknown VIF codes stop the scan (whatever
 * was collected so far stays valid). */
static void coop_tint_scan_packet(unsigned char *p)
{
    int segs;

    for (segs = 0; segs < 4; segs++) {
        unsigned int tag0 = *(unsigned int *)p;
        unsigned int qwc = tag0 & 0xFFFF;
        unsigned int id = (tag0 >> 28) & 7;
        unsigned char *v = p + 8;
        unsigned char *end;

        if (qwc == 0 || qwc > 0x800) {
            return;
        }
        end = p + 16 + qwc * 16;
        while (v + 4 <= end) {
            unsigned int code = *(unsigned int *)v;
            unsigned int cmd = (code >> 24) & 0x7F;
            unsigned int num = (code >> 16) & 0xFF;

            v += 4;
            if (num == 0) {
                num = 256;
            }
            if (cmd >= 0x60) {                     /* UNPACK */
                unsigned int vn = ((cmd >> 2) & 3) + 1;
                unsigned int bits = 32 >> (cmd & 3);
                unsigned int nbytes = (num * vn * bits + 7) / 8;

                nbytes = (nbytes + 3) & ~3u;
                if ((cmd & 0xF) == 0xE) {          /* V4-8: colours */
                    unsigned int i;

                    for (i = 0; i < num && g_tint_save_n < COOP_TINT_SAVE_MAX;
                         i++) {
                        unsigned int *cw = (unsigned int *)(v + i * 4);

                        g_tint_save[g_tint_save_n].addr = cw;
                        g_tint_save[g_tint_save_n].orig = *cw;
                        g_tint_save_n++;
                    }
                }
                v += nbytes;
            } else if (cmd == 0x20) {              /* STMASK */
                v += 4;
            } else if (cmd == 0x30 || cmd == 0x31) { /* STROW/STCOL */
                v += 16;
            } else if (cmd == 0x4A) {              /* MPG */
                v += num * 8;
            } else if (cmd == 0x50 || cmd == 0x51) { /* DIRECT/HL */
                unsigned int imm = code & 0xFFFF;

                v += (imm == 0 ? 0x10000u : imm) * 16;
            } else if (cmd > 0x17) {               /* not a no-payload code */
                return;
            }
        }
        if (id != 1) {                             /* only cnt continues */
            return;
        }
        p = end;
    }
}

/* (Re)collect the glow's colour words for the current level's scene. */
static void coop_tint_scan(void)
{
    unsigned int *gobj = coop_item_gobj(COOP_VS_CRYSTAL_OBJ);
    unsigned char *node;
    int n;

    g_tint_save_n = 0;
    if (gobj == 0) {
        return;
    }
    /* Loaded (pre-converted) geom node: {next, material, packet}. */
    node = (unsigned char *)gobj[0xC / 4];
    for (n = 0; n < 8 && COOP_PTR_OK(node); n++) {
        unsigned char *pkt = *(unsigned char **)(node + 8);

        if (COOP_PTR_OK(pkt)) {
            coop_tint_scan_packet(pkt);
        }
        node = *(unsigned char **)(node + 0);
    }
}

/* want < 0 restores the baked colours; otherwise every vertex becomes
 * intensity(orig) * player colour (intensity = max component -- luminance
 * would crush the green-poor purple), keeping the baked alpha. */
static void coop_tint_write(int want)
{
    int i;

    for (i = 0; i < g_tint_save_n; i++) {
        unsigned int c = g_tint_save[i].orig;

        if (want < 0) {
            *g_tint_save[i].addr = c;
        } else {
            unsigned int r = c & 0xFF;
            unsigned int g = (c >> 8) & 0xFF;
            unsigned int b = (c >> 16) & 0xFF;
            unsigned int inten = r > g ? r : g;

            if (b > inten) {
                inten = b;
            }
            *g_tint_save[i].addr = (c & 0xFF000000u) |
                (((inten * coop_vs_colour[want][2]) >> 8) << 16) |
                (((inten * coop_vs_colour[want][1]) >> 8) << 8) |
                ((inten * coop_vs_colour[want][0]) >> 8);
        }
    }
}

/* Per-tick tint driver. Pre-claim the crystal shows OUR colour (each player
 * sees their own target); once claimed it re-tints to the WINNER's colour --
 * the world crystal is gone by then, so the retint is what the pause-panel
 * carving renders, the same colour on both screens. Level changes rebuild
 * the scene with freshly baked colours, so old pointers are simply
 * forgotten, never touched. */
static void coop_vs_tint_tick(void)
{
    int owner;

    if (Level != g_tint_level) {
        g_tint_level = -1;
        g_tint_shown = -2;
        g_tint_save_n = 0;
    }
    /* bit 4 is owned by the DrawCreatures hook (set after this tick's diag
     * push): keep last frame's value so it stays visible over PINE. */
    g_vs_diag &= 0x10u;
    if (!g_vs_mode || !local_valid()) {
        g_vs_want = -1;
        if (g_tint_shown != -2) {
            /* VS switched off mid-level: put the baked colours back. */
            coop_tint_write(-1);
            g_tint_shown = -2;
        }
        return;
    }
    owner = coop_vs_owner(0x1);
    g_vs_want = (owner == -1) ? (g_vs_p2 ? 1 : 0) : owner;
    if (g_tint_shown == g_vs_want) {
        g_vs_diag |= 2u;
        return;
    }
    if (g_tint_save_n == 0) {
        coop_tint_scan();
        if (g_tint_save_n == 0) {
            g_vs_diag |= (coop_item_gobj(COOP_VS_CRYSTAL_OBJ) == 0) ? 8u : 4u;
            return;
        }
        g_tint_level = Level;
    }
    coop_tint_write(g_vs_want);
    g_tint_shown = g_vs_want;
    g_vs_diag |= 2u;
}

/* The light tint indexes the live Character[] array, so the mirrored
 * creature_s must match retail's stride and light-record offset exactly
 * (retail anchors: stride 0xCE4 = CloseCreatures, lights +0xBF4 =
 * ResetPlayer, obj +0x4). Build breaks here if a mirror struct drifts. */
typedef char coop_creature_stride_check[
    (sizeof(struct creature_s) == 0xCE4) ? 1 : -1];
typedef char coop_creature_lights_check[
    ((unsigned int)&((struct creature_s *)0)->lights == 0xBF4) ? 1 : -1];
typedef char coop_creature_char_check[
    ((unsigned int)&((struct creature_s *)0)->obj.character == 0x34) ? 1 : -1];

/* Dev-only live calibration channel: the CoopMailbox occupies payload
 * +0x000..0x247 of the 0x400 mailbox; poke magic 'VTNT' + 3 floats at
 * payload+0x248 over PINE and the body tint uses those instead of the
 * table -- colours can be tuned live while the game runs. Zero the magic
 * to fall back. Never written by the game or bridge. */
#define COOP_VS_TUNE_MAGIC 0x544E5456u /* 'VTNT' */
#define COOP_VS_TUNE_BASE \
    ((volatile unsigned char *)modsdk_mailbox.payload + 0x248)

/* Body light tint (see the section comment): flat coloured ambient, no
 * directional contribution. Colour values only -- the light-list pointers
 * the record was initialised with stay untouched. */
static void coop_vs_light_tint(struct Nearest_Light_s *L)
{
    const float *t = coop_vs_light[g_vs_want];

    if (*(volatile unsigned int *)COOP_VS_TUNE_BASE == COOP_VS_TUNE_MAGIC) {
        t = (const float *)(COOP_VS_TUNE_BASE + 4);
    }
    L->AmbCol.x = t[0];
    L->AmbCol.y = t[1];
    L->AmbCol.z = t[2];
    L->dir1.Colour.r = L->dir1.Colour.g = L->dir1.Colour.b = 0.0f;
    L->dir2.Colour.r = L->dir2.Colour.g = L->dir2.Colour.b = 0.0f;
    L->dir3.Colour.r = L->dir3.Colour.g = L->dir3.Colour.b = 0.0f;
    L->glbdirectional.Colour.r = 0.0f;
    L->glbdirectional.Colour.g = 0.0f;
    L->glbdirectional.Colour.b = 0.0f;
}

/* --- Stage 3b: per-crate destroyed-state sync ----------------------------
 * Capture at CrateOff, the single point every crate actually dies through
 * (stack chains, nitro-switch chains and TNT explosions bypass BreakCrate,
 * so hooking BreakCrate would miss them). Apply through the full BreakCrate
 * so remote breaks replay with retail's own dispatch, chains, sfx and
 * rewards (symmetric execution -- the g_coop_applying bracket is the future
 * asymmetric-rewards seam). */
#define COOP_CRATE_APPLY_MAX 8  /* replayed breaks per tick (chains amplify) */
#define COOP_CRATE_SETTLE 50    /* ~1 s apply-pause after a death reset: long
                                 * enough for the peer to see our bits fall,
                                 * mirror the reset and drop ITS bits (incl.
                                 * internet relay lag), so neither side
                                 * re-breaks resurrected crates from the
                                 * other's stale echo */

static void coop_crate_drop_intact(void);

/* Replace hook on CrateOff (0x1F3178): when a crate REALLY went off
 * (nonzero return), record its flat Crate[] slot into the published bitmap.
 * Bonus-round crates stay personal. Applied echoes are captured on purpose:
 * re-publishing them makes the bitmaps converge from either side. */
int coop_crate_off(struct coop_crategroup_s *group, struct coop_crate_s *crate,
                   int a, int b)
{
    int ret = orig_CrateOff(group, crate, a, b);

    if (ret != 0 && Bonus == 0 && LEVEL_PLAYABLE(Level)) {
        int idx = crate - Crate;

        if (idx >= 0 && idx < COOP_CRATE_WORDS * 32) {
            g_crate_bits[idx >> 5] |= 1u << (idx & 31);
        }
    }
    return ret;
}

/* Checkpoints are SHARED (user decision 2026-07-12, replacing the earlier
 * own-checkpoints-only rule): a remote checkpoint-crate break replays
 * through the full CrateOff path, whose ResetCheckpoint call takes every
 * argument from the crate itself (+0x31/+0x32/+0x34/&pos) -- so the replay
 * moves our respawn point to the exact same checkpoint the breaker got,
 * and both CrateTypeData snapshot baselines reset at the same crate. No
 * suppress hook needed; simply not intercepting ResetCheckpoint is the
 * feature. */

/* Replace hook on GotoCheckpoint (0x1F7CC8): the death block runs
 * RestoreCrateTypeData + ResetCrates BEFORE GotoCheckpoint, so when the
 * original returns our post-checkpoint crates are already resurrected --
 * drop them from the published bitmap IMMEDIATELY (the falling edge is the
 * peer's shared-death-reset cue; publishing it before our apply loop can
 * run again is what makes the ordering safe), then pause the apply loop
 * for the settle window so the peer has time to mirror the reset and drop
 * its own bits -- otherwise we would re-break our freshly resurrected
 * crates from its stale echo bitmap (our applied mask never covered the
 * crates we broke first-hand). Event-driven (death block + debug menu),
 * never polled. */
int coop_goto_checkpoint(struct nuvec_s *pos, int dir)
{
    int ret = orig_GotoCheckpoint(pos, dir);

    coop_crate_drop_intact();
    g_crate_reconcile = COOP_CRATE_SETTLE;
    return ret;
}

/* Retail's own "this crate is gone" predicate (UpdatePlayerStats scans it
 * every frame): turned off, or blown up in place (exploded outpost slots
 * keep on != 0). */
static int coop_crate_destroyed(struct coop_crate_s *crate)
{
    return crate->on == 0 ||
           (crate->newtype == 0xF && crate->metal_count != 0);
}

static struct coop_crategroup_s *coop_find_crate_group(int idx)
{
    int g;

    for (g = 0; g < CRATEGROUPCOUNT; g++) {
        int first = CrateGroup[g].first;

        if (idx >= first && idx < first + CrateGroup[g].count) {
            return &CrateGroup[g];
        }
    }
    return 0;
}

/* Drop published bits whose crate is intact again (after a death reset --
 * our own respawn's, or a mirrored remote one): stop claiming them broken,
 * so the peer sees the falling edge / doesn't re-break them. */
static void coop_crate_drop_intact(void)
{
    int i;

    for (i = 0; i < CRATECOUNT && i < COOP_CRATE_WORDS * 32; i++) {
        if ((g_crate_bits[i >> 5] & (1u << (i & 31))) != 0 &&
            !coop_crate_destroyed(&Crate[i])) {
            g_crate_bits[i >> 5] &= ~(1u << (i & 31));
        }
    }
}

/* Replay newly-published remote breaks (called from coop_merge's same-level
 * tier). Budgeted per tick: each BreakCrate can chain (stacks, TNT), so a
 * catch-up burst spreads over a few frames instead of one spike. TNT/nitro
 * mid-countdown (armed != -1) are left pending and retried -- their own
 * explosion flips them to destroyed and the bit is then marked applied. */
static void coop_crate_apply(void)
{
    int budget = COOP_CRATE_APPLY_MAX;
    unsigned int fallen = 0;
    int w;
    int i;

    /* Applied-bits live only as long as the remote publishes the crate:
     * re-arm on the falling edge, so a later re-publish (a genuine
     * re-break after a shared reset) replays again. */
    for (w = 0; w < COOP_CRATE_WORDS; w++) {
        fallen |= g_crate_remote_prev[w] & ~remote_snap.crate_bits[w];
        g_crate_applied[w] &= remote_snap.crate_bits[w];
        g_crate_remote_prev[w] = remote_snap.crate_bits[w];
    }
    /* SHARED DEATH RESET (user decision 2026-07-12): bits falling out of
     * the remote's published set can only mean its death respawn just
     * resurrected its post-checkpoint crates (its GotoCheckpoint drop). Run
     * the crate-state pair of retail's death block here, so the same
     * crates resurrect for the living player and both box counters stay
     * identical. Both CrateTypeData logs converge via the echo capture and
     * share their baseline (the replayed checkpoint), so the restore is
     * symmetric; it also self-clears, so a double reset (both players
     * died) is a no-op. Keyed on the absolute published state rather than
     * the dead flag: a staleness gap can swallow a flag edge, but a
     * missing bit is still missing when the link recovers. Our own apply
     * loop pauses for the settle window too -- the remote may publish a
     * few more stale ticks before its own reset bookkeeping is done. */
    if (fallen != 0) {
        g_coop_applying = 1;
        RestoreCrateTypeData();
        ResetCrates();
        g_coop_applying = 0;
        g_crate_reconcile = COOP_CRATE_SETTLE;
    }
    /* Every processed tick: stop claiming crates that are intact again --
     * covers our GotoCheckpoint respawn, the mirrored reset above, and any
     * other retail path that resurrects crates (menu restarts) within one
     * tick, so the peer's falling edge is never stale. */
    coop_crate_drop_intact();
    if (g_crate_reconcile > 0) {
        g_crate_reconcile--;
        return; /* propagation window: the peer may still echo pre-reset
                 * bits; applying now would re-break resurrected crates */
    }
    for (w = 0; w < COOP_CRATE_WORDS && budget > 0; w++) {
        unsigned int nw = remote_snap.crate_bits[w] & ~g_crate_applied[w] &
                          ~g_crate_bits[w];

        if (nw == 0) {
            continue;
        }
        for (i = 0; i < 32 && budget > 0; i++) {
            unsigned int bit = 1u << i;
            struct coop_crate_s *crate;
            struct coop_crategroup_s *group;
            int idx = (w << 5) | i;

            if ((nw & bit) == 0) {
                continue;
            }
            if (idx >= CRATECOUNT) {
                g_crate_applied[w] |= bit; /* slot divergence guard */
                continue;
            }
            crate = &Crate[idx];
            if (coop_crate_destroyed(crate)) {
                g_crate_applied[w] |= bit; /* already gone: converged */
                continue;
            }
            if (crate->armed != -1) {
                continue; /* counting down: it will destroy itself */
            }
            group = coop_find_crate_group(idx);
            if (group == 0) {
                g_crate_applied[w] |= bit; /* orphan slot: never apply */
                continue;
            }
            g_coop_applying = 1;
            BreakCrate(group, crate, GetCrateType(crate, 0), 0);
            g_coop_applying = 0;
            g_crate_applied[w] |= bit;
            budget--;
        }
    }
}

/* The remote's end-of-level celebration, replayed on the puppet. Retail's
 * award show runs on the REMOTE when it tumbles back into its hub: the
 * crystal/gem appears over Crash (tumble action 0x56 -- which the puppet
 * already mirrors via the action sync), pops off with a chime + sparkle,
 * flies to the finished level's pad, and commits the Game flag bits on
 * arrival. We replay it with the real AddAward, rewriting the slot to the
 * flying stage with the puppet as the source; the flight, pad arrival,
 * sparkles and the COMMIT are all retail's own UpdateAwards. Bits handed to
 * a flight are NOT merged into Game up front, so the pad marker and the
 * tallies appear exactly when the crystal lands, like on the remote's
 * screen (g_award_pending tracks them; a lost flight is force-flushed to a
 * direct merge after COOP_AWARD_FLUSH ticks).
 *
 * Timing: the PRIMARY cue is the v9 pop edge. The remote publishes
 * pending_flags = new_lev_flags & ~temp_lev_flags: new_lev_flags alone is
 * NOT the pop signal (the tumble leaves it set until UpdateAwards XOR-clears
 * it at pad ARRIVAL); the pop frame is where the tumble ORs the group into
 * temp_lev_flags, so the masked value falls at that exact frame
 * (coop_award_pop_edge) and our flight launches in the same frame the
 * remote's does, off a puppet that is in the hold-up pose. The committed
 * level_flags bit (which only appears when the remote's flight LANDS) is
 * kept as a late fallback for missed edges. */
#define COOP_AWARD 1
#ifndef COOP_AWARD_FLUSH
#define COOP_AWARD_FLUSH 750 /* ~15 s: pending-award safety flush */
#endif

#if COOP_AWARD
/* Spawn core: fly award(s) for `bits` of `level` from the puppet. Shared by
 * the frame-accurate pop-edge path and the late merge fallback. Returns the
 * bits actually handed to flights. */
static unsigned int coop_award_spawn(int level, unsigned int bits)
{
    /* creature.c's gotlist: one award per flag group, relic tiers together */
    static const unsigned short gotlist[9] = {
        0x8, 0x7, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400,
    };
    unsigned int spawned_bits = 0;
    int spawned = 0;
    int slot;
    int g;
    int j;

    if (Level != 0x25 || g_puppet_active == 0 || Hub < 0 || Hub >= 6 ||
        remote_snap.level != 0x25 || TimeTrial != 0) {
        return 0;
    }
    /* If the LOCAL player earned the same bits simultaneously, their own
     * award machinery is already handling them -- don't double the show. */
    bits &= (unsigned int)(unsigned short)~new_lev_flags;
    /* AddAward samples the LOCAL hub's spline for the pad position, so the
     * finished level must belong to the hub we are standing in. */
    for (j = 0; j < 6; j++) {
        if (HData[Hub].level[j] == level) {
            break;
        }
    }
    if (j == 6) {
        return 0;
    }
    for (g = 0; g < 9 && spawned < 3; g++) {
        unsigned int b = bits & gotlist[g];

        if (b == 0) {
            continue;
        }
        slot = i_award; /* AddAward fills Award[i_award] then advances it */
        if (AddAward(Hub, level, (int)b) == 0) {
            break;
        }
        {
            struct coop_award_s *aw = &Award[slot];

            /* Skip the held-above-the-LOCAL-player stage: fly immediately,
             * from the puppet's mid-body (the pop-edge timing means the
             * puppet is in the hold-up pose right now, same as the remote's
             * Crash), with the pop chime + sparkle the stage transition
             * would have played. NEVER leave stage 1 set: it reads the
             * global player + tumble state. */
            aw->stage = 0;
            aw->src[0] = g_puppet.obj.pos.x;
            aw->src[1] = g_puppet.obj.pos.y + 0.9f;
            aw->src[2] = g_puppet.obj.pos.z;
            aw->fx[0] = aw->src[0];
            aw->fx[1] = aw->src[1] + 1.0f;
            aw->fx[2] = aw->src[2];
            GameSfx(0x26, 0);
            AddGameDebris(0xA1, (struct nuvec_s *)aw->fx);
            spawned_bits |= b;
            spawned++;
        }
    }
    return spawned_bits;
}

/* v9 pop edge: the remote's pending_flags (new_lev_flags & ~temp_lev_flags)
 * loses a group at the EXACT tumble frame the item pops off its Crash --
 * spawn our flight on that falling edge instead of waiting ~2 s for the
 * committed level_flags bit (the v8-only fallback below, kept for missed
 * edges: stale gaps, or us entering the hub mid-flight). Bits already
 * committed locally or already in flight are filtered so a staleness resync
 * can't double-spawn. */
static void coop_award_pop_edge(void)
{
    int level = remote_snap.pending_level;
    unsigned int fell;
    unsigned int fly;

    if (remote_gen != 0 && level == g_prev_pending_level && level >= 0 &&
        level < COOP_LEVEL_COUNT) {
        fell = g_prev_pending_flags &
               (unsigned short)~remote_snap.pending_flags &
               (unsigned short)~Game.level[level].flags &
               (unsigned short)~g_award_pending[level];
        if (fell != 0) {
            fly = coop_award_spawn(level, fell);
            if (fly != 0) {
                g_award_pending[level] |= (unsigned short)fly;
                g_award_deadline = COOP_AWARD_FLUSH;
            }
        }
    }
    g_prev_pending_flags = remote_snap.pending_flags;
    g_prev_pending_level = remote_snap.pending_level;
}
#endif /* COOP_AWARD */

/* Merge the remote's progression into ours (runs every tick, after
 * consume_remote). Two tiers:
 *
 *  (1) Committed progression -- level_flags / hub_flags / powers -- is
 *      OR-merged ALWAYS (any level, either side, both directions), then any
 *      newly-set level/hub bit triggers CalculateGamePercentage, which
 *      re-derives every tally (percent, crystal counts per hub, gems,
 *      relics) from the flags. That recompute is what makes a crystal the
 *      remote earned show up in the local hub UI and save while the players
 *      are in different places.
 *  (2) Live in-level state -- plr_items + the identity-based item replay
 *      (see coop_item_bit) -- only while both sides are in the SAME level,
 *      neither is in a bonus round, we are not paused and not mid-death.
 *      Replay = the real PickupItem on the live object whose character
 *      grants a collected-but-alive reward bit, so both games run identical
 *      code (symmetric execution; the g_coop_applying bracket is the seam a
 *      later asymmetric-rewards mode hooks into). obj.dead gates
 *      double-application.
 *
 * Time trials are personal by design: no merging at all while TimeTrial.
 * Gating on local_valid keeps us from ever merging into an unloaded
 * profile (menus): Game only holds a real profile while in a level. */
static void coop_merge(void)
{
    unsigned int nw;
    unsigned int npw;
    int changed;
    int w;
    int i;

    if (remote_gen == 0 || TimeTrial != 0 || !local_valid()) {
        return;
    }

#if COOP_AWARD
    /* Pending-award upkeep: bits commit when their flight lands (they show
     * up in Game -- drop them here); a flight lost mid-air (slot overwritten,
     * hub left) is force-flushed to a plain merge after the deadline. */
    if (g_award_deadline > 0 && --g_award_deadline == 0) {
        for (i = 0; i < COOP_LEVEL_COUNT; i++) {
            g_award_pending[i] = 0;
        }
    }
#endif
    changed = 0;
    for (i = 0; i < COOP_LEVEL_COUNT; i++) {
#if COOP_AWARD
        g_award_pending[i] &= (unsigned short)~Game.level[i].flags;
        nw = remote_snap.level_flags[i] &
             (unsigned short)~Game.level[i].flags &
             (unsigned short)~g_award_pending[i];
        if (nw != 0) {
            /* Late fallback for a missed pop edge (we entered the hub with
             * the flight already airborne, or a stale gap ate the edge):
             * only bits of the level the remote just came from celebrate --
             * profile catch-up bursts merge silently. */
            unsigned int fly = (g_remote_prev_room == i)
                                   ? coop_award_spawn(i, nw)
                                   : 0;

            if (fly != 0) {
                g_award_pending[i] |= (unsigned short)fly;
                g_award_deadline = COOP_AWARD_FLUSH;
                nw &= ~fly;
            }
        }
#else
        nw = remote_snap.level_flags[i] & (unsigned short)~Game.level[i].flags;
#endif
        if (nw != 0) {
            Game.level[i].flags |= (unsigned short)nw;
            changed = 1;
        }
    }
    for (i = 0; i < COOP_HUB_COUNT; i++) {
        nw = remote_snap.hub_flags[i] & (unsigned char)~Game.hub[i].flags;
        if (nw != 0) {
            Game.hub[i].flags |= (unsigned char)nw;
            changed = 1;
        }
    }
    /* Powers rising edge BEFORE the OR: tier 2 uses it to replay the
     * power object (despawn + popup) when we share the level. */
    npw = remote_snap.powers & (unsigned int)(unsigned char)~Game.powerbits;
    Game.powerbits |= remote_snap.powers; /* not flag-derived: no recompute */
    if (changed) {
        CalculateGamePercentage(&Game);
    }

    if (remote_snap.level != Level) {
        /* Different room: the remote's crate bitmap belongs to another
         * level (or is the fresh zero after its level change) -- forget
         * the last-seen copy so re-entry never fakes a falling edge. */
        for (w = 0; w < COOP_CRATE_WORDS; w++) {
            g_crate_remote_prev[w] = 0;
        }
        return;
    }
    if (remote_snap.bonus != 0 || Bonus != 0 || Paused != 0 ||
        player->obj.dead != 0) {
        /* prev is KEPT: a falling edge during our pause/death or a bonus
         * round is processed late instead of lost. */
        return;
    }
    /* Identity-based item replay: any LIVE placed item whose reward bit is
     * already collected (by us -- an earlier replay that ran before the
     * object streamed in -- or by the remote) gets the real PickupItem:
     * grants the bit, despawns the object, plays the pickup effects. Our
     * own pickups never appear here (PickupItem sets obj.dead in the same
     * call). Capped per tick against sfx bursts; self-healing -- a still-
     * alive object is retried next tick. */
    {
        unsigned int want = (unsigned int)plr_items | remote_snap.items;
        int replays = 0;

        for (i = 0; i < 64 && replays < 2; i++) {
            struct obj_s *obj = pObj[i];

            if (obj != 0 && obj->dead == 0 &&
                ((coop_item_bit(obj->character) & want) != 0 ||
                 (coop_power_bit(obj->character) & npw) != 0)) {
                g_coop_applying = 1;
                PickupItem(obj);
                g_coop_applying = 0;
                replays++;
            }
        }
    }
    /* VS attribution: a remote items bit we did not claim first-hand was
     * the peer's touch (our own echo is filtered by g_vs_mine, which the
     * PickupItem hook set before the bit could ever round-trip). */
    if (g_vs_mode) {
        g_vs_peer |= (unsigned short)(remote_snap.items & ~g_vs_mine);
    }
    /* Bits with no live object here (not streamed in / already gone):
     * grant the reward directly, the HUD is what matters. Powers were
     * OR-merged in tier 1; their object replay above is edge-only
     * (powerbits persist across levels, so an alive-object rule would
     * auto-collect an owned power when replaying its level). */
    plr_items |= remote_snap.items;
    coop_crate_apply();
}

/* New level (or leaving one): the per-level bitmaps belong to the old room.
 * Pending award bits are dropped too -- their flights died with the room, so
 * the next merge tick re-derives and direct-commits them (self-healing). */
static void coop_progress_reset(void)
{
    int i;

    for (i = 0; i < COOP_CRATE_WORDS; i++) {
        g_crate_bits[i] = 0;
        g_crate_applied[i] = 0;
        g_crate_remote_prev[i] = 0;
    }
    g_crate_reconcile = 0;
    g_vs_mine = 0;
    g_vs_peer = 0;
#if COOP_AWARD
    for (i = 0; i < COOP_LEVEL_COUNT; i++) {
        g_award_pending[i] = 0;
    }
    g_award_deadline = 0;
#endif
}

/* Special vehicles whose transform lives in NEWBUGGY (creature+0x224) rather
 * than obj.pos: glider variants and the atlasphere ("ball"). */
static int coop_is_glider(short v) { return v == 0x81 || v == 0x8B || v == 0x36; }
static int coop_is_atlas(short v) { return v == 0x53; }

/* Pack the local player's NEWBUGGY transform into vehicle_xf[7] (see
 * coop_mailbox.h): glider = pitch/roll/yaw + Buggy.pos; atlas = ball_pos + quat.
 * Only meaningful while VEHICLECONTROL == 1 and Buggy != 0. */
static void pack_vehicle_xf(float *xf)
{
    struct NEWBUGGY *b = player->Buggy;
    short v = player->obj.vehicle;

    if (VEHICLECONTROL != 1 || b == 0) {
        return;
    }
    if (coop_is_glider(v)) {
        xf[0] = b->pitch;
        xf[1] = b->roll;
        xf[2] = b->yaw;
        xf[3] = b->pos.x;
        xf[4] = b->pos.y;
        xf[5] = b->pos.z;
        /* enable gates DrawGlider's level-0xD fixed-vs-positioned branch; sync
         * the remote's real value (0 = flies at Buggy.pos, as in Tornado Valley)
         * rather than forcing it -- forcing 1 pinned the puppet to D_006B75A0. */
        xf[6] = (float)b->enable;
    } else if (coop_is_atlas(v)) {
        xf[0] = b->ball_pos.x;
        xf[1] = b->ball_pos.y;
        xf[2] = b->ball_pos.z;
        xf[3] = b->rotquat.x;
        xf[4] = b->rotquat.y;
        xf[5] = b->rotquat.z;
        xf[6] = b->rotquat.w;
    }
}

static void publish_local(void)
{
    volatile CoopSlot *s = &COOP->local;
    int i;

    local_gen++;
    local_frame++;
    s->seq_open = local_gen;
    s->frame = local_frame;
    if (local_valid()) {
        s->level = Level;
        s->flags = COOP_F_PRESENT |
                   (player->obj.dead != 0 ? COOP_F_DEAD : 0);
        s->pos[0] = player->obj.pos.x;
        s->pos[1] = player->obj.pos.y;
        s->pos[2] = player->obj.pos.z;
        s->mom[0] = player->obj.mom.x;
        s->mom[1] = player->obj.mom.y;
        s->mom[2] = player->obj.mom.z;
        s->hdg = player->obj.hdg;
        s->xrot = player->obj.xrot;
        s->yrot = player->obj.yrot;
        s->zrot = player->obj.zrot;
        s->surface_xrot = player->obj.surface_xrot;
        s->surface_zrot = player->obj.surface_zrot;
        s->action = player->obj.anim.action;
        s->character = player->obj.character;
        s->anim_time = player->obj.anim.anim_time;
        s->scale = player->obj.SCALE;
        s->shadow = player->obj.shadow;
        s->target_xrot = player->obj.target_xrot;
        s->target_yrot = player->obj.target_yrot;
        s->spin_frame = player->spin_frame;
        s->spin_frames = player->spin_frames;
        s->spin = player->spin;
        s->dangle = player->obj.dangle;
        s->target = player->target;
        s->fire = player->fire;
        s->freeze = player->freeze;
        s->paused = (unsigned char)(Paused != 0);
        s->vehicle = player->obj.vehicle;
        s->vehiclecontrol = (unsigned char)VEHICLECONTROL;
        s->mask_active = (player->obj.mask != 0)
                             ? (unsigned char)player->obj.mask->active
                             : 0;
        pack_vehicle_xf((float *)s->vehicle_xf);
        /* Hub teleport-out: warp_level goes != -1 the instant Crash steps on a
         * teleporter, while still standing in the hub, ~1 s before the level
         * loads. The peer uses it to play the warp effect on our puppet at the
         * right moment instead of just popping us out of existence. */
        s->warp_level = warp_level;
        /* in_finish_range goes >0 the instant we enter a teleport zone, ~1 s
         * before warp_level commits -- the peer uses it to make our puppet's
         * teleporter appear as we run into the zone, like real Crash, instead
         * of only at the dissolve. */
        s->in_finish_range = in_finish_range;
        /* in_finish_pos is the teleport NODE (spline point), not our body pos --
         * the peer places the puppet's teleporter here so it lands on the pad. */
        s->in_finish_pos[0] = in_finish_pos[0];
        s->in_finish_pos[1] = in_finish_pos[1];
        s->in_finish_pos[2] = in_finish_pos[2];
        /* Progression (Stage 3a): only published from a valid in-level state,
         * where Game is guaranteed to hold a real loaded profile. */
        s->bonus = (unsigned char)(Bonus != 0);
        s->items = plr_items;
        for (i = 0; i < COOP_LEVEL_COUNT; i++) {
            s->level_flags[i] = Game.level[i].flags;
        }
        s->powers = Game.powerbits;
        for (i = 0; i < COOP_HUB_COUNT; i++) {
            s->hub_flags[i] = Game.hub[i].flags;
        }
        for (i = 0; i < COOP_CRATE_WORDS; i++) {
            s->crate_bits[i] = g_crate_bits[i];
        }
        for (i = 0; i < COOP_ITEM_WORDS; i++) {
            s->item_bits[i] = 0; /* reserved again: identity replay needs
                                  * no capture (items word suffices) */
        }
        /* v9: award bits pending AND not yet popped -- new_lev_flags alone
         * only falls when the flight LANDS (UpdateAwards XOR-clears it at
         * arrival); the pop frame instead ORs the group into temp_lev_flags
         * (creature.c tumble). Masking it out makes the published value fall
         * at the exact pop frame, the peer's celebration cue. */
        s->pending_flags = (unsigned short)(new_lev_flags &
                                            (unsigned short)~temp_lev_flags);
        s->pending_level = (short)last_level;
        /* name is PC-supplied (the bridge writes it into the peer's slot);
         * the game never writes it, so leave the field alone here. */
    } else {
        s->level = -1;
        s->flags = 0;
        s->spin = 0;
        s->target = 0;
        s->paused = 0;
        s->vehicle = -1;
        s->vehiclecontrol = 0;
        s->mask_active = 0;
        s->warp_level = -1;
        s->in_finish_range = 0;
        s->in_finish_pos[0] = 0.0f;
        s->in_finish_pos[1] = 0.0f;
        s->in_finish_pos[2] = 0.0f;
        /* No progression from menus/loading: Game may not hold a loaded
         * profile, and zeroed words are the OR-merge no-op. */
        s->bonus = 0;
        s->items = 0;
        for (i = 0; i < COOP_LEVEL_COUNT; i++) {
            s->level_flags[i] = 0;
        }
        s->powers = 0;
        for (i = 0; i < COOP_HUB_COUNT; i++) {
            s->hub_flags[i] = 0;
        }
        for (i = 0; i < COOP_CRATE_WORDS; i++) {
            s->crate_bits[i] = 0;
        }
        for (i = 0; i < COOP_ITEM_WORDS; i++) {
            s->item_bits[i] = 0;
        }
        s->pending_flags = 0;
        s->pending_level = -1;
    }
    s->seq_close = local_gen;
}

static void consume_remote(void)
{
    volatile CoopSlot *r = &COOP->remote;
    unsigned int c;
    int i;
    CoopSlot tmp;

    c = r->seq_close;
    if (c != 0 && c != remote_gen) {
        tmp.frame = r->frame;
        tmp.level = r->level;
        tmp.flags = r->flags;
        tmp.pos[0] = r->pos[0];
        tmp.pos[1] = r->pos[1];
        tmp.pos[2] = r->pos[2];
        tmp.mom[0] = r->mom[0];
        tmp.mom[1] = r->mom[1];
        tmp.mom[2] = r->mom[2];
        tmp.hdg = r->hdg;
        tmp.xrot = r->xrot;
        tmp.yrot = r->yrot;
        tmp.zrot = r->zrot;
        tmp.surface_xrot = r->surface_xrot;
        tmp.surface_zrot = r->surface_zrot;
        tmp.action = r->action;
        tmp.character = r->character;
        tmp.anim_time = r->anim_time;
        tmp.scale = r->scale;
        tmp.shadow = r->shadow;
        tmp.target_xrot = r->target_xrot;
        tmp.target_yrot = r->target_yrot;
        tmp.spin_frame = r->spin_frame;
        tmp.spin_frames = r->spin_frames;
        tmp.spin = r->spin;
        tmp.dangle = r->dangle;
        tmp.target = r->target;
        tmp.fire = r->fire;
        tmp.freeze = r->freeze;
        tmp.paused = r->paused;
        tmp.vehicle = r->vehicle;
        tmp.vehiclecontrol = r->vehiclecontrol;
        tmp.mask_active = r->mask_active;
        tmp.warp_level = r->warp_level;
        tmp.in_finish_range = r->in_finish_range;
        tmp.in_finish_pos[0] = r->in_finish_pos[0];
        tmp.in_finish_pos[1] = r->in_finish_pos[1];
        tmp.in_finish_pos[2] = r->in_finish_pos[2];
        for (i = 0; i < 16; i++) {
            tmp.name[i] = r->name[i];
        }
        for (i = 0; i < 7; i++) {
            tmp.vehicle_xf[i] = r->vehicle_xf[i];
        }
        tmp.bonus = r->bonus;
        tmp.items = r->items;
        for (i = 0; i < COOP_LEVEL_COUNT; i++) {
            tmp.level_flags[i] = r->level_flags[i];
        }
        tmp.powers = r->powers;
        for (i = 0; i < COOP_HUB_COUNT; i++) {
            tmp.hub_flags[i] = r->hub_flags[i];
        }
        for (i = 0; i < COOP_CRATE_WORDS; i++) {
            tmp.crate_bits[i] = r->crate_bits[i];
        }
        for (i = 0; i < COOP_ITEM_WORDS; i++) {
            tmp.item_bits[i] = r->item_bits[i];
        }
        tmp.pending_flags = r->pending_flags;
        tmp.pending_level = r->pending_level;
        tmp.seq_open = c;
        tmp.seq_close = c;
        if (r->seq_open == c) {
            remote_snap = tmp;
            remote_gen = c;
        }
    }
    if (remote_snap.frame != remote_frame) {
        remote_frame = remote_snap.frame;
        stale_frames = 0;
    } else if (stale_frames < 10000) {
        stale_frames++;
    }
    /* Diagnostics readable over PINE without knowing the coop layout. */
    modsdk_mailbox.reserved[0] = remote_gen;
    modsdk_mailbox.reserved[1] = stale_frames;
}

/* Self-test (COOP_CTL_GHOST over PINE): synthesize the remote snapshot
 * from the local player, offset +2.0 on x, so the puppet path runs with
 * no bridge and no second instance. */
static void synth_ghost(void)
{
    int i;

    if (!local_valid()) {
        remote_snap.level = -1;
        remote_snap.flags = 0;
        return;
    }
    remote_snap.level = Level;
    remote_snap.flags = COOP_F_PRESENT |
                        (player->obj.dead != 0 ? COOP_F_DEAD : 0);
    remote_snap.pos[0] = player->obj.pos.x + 2.0f;
    remote_snap.pos[1] = player->obj.pos.y;
    remote_snap.pos[2] = player->obj.pos.z;
    remote_snap.mom[0] = player->obj.mom.x;
    remote_snap.mom[1] = player->obj.mom.y;
    remote_snap.mom[2] = player->obj.mom.z;
    remote_snap.hdg = player->obj.hdg;
    remote_snap.xrot = player->obj.xrot;
    remote_snap.yrot = player->obj.yrot;
    remote_snap.zrot = player->obj.zrot;
    remote_snap.surface_xrot = player->obj.surface_xrot;
    remote_snap.surface_zrot = player->obj.surface_zrot;
    remote_snap.action = player->obj.anim.action;
    remote_snap.character = player->obj.character;
    remote_snap.anim_time = player->obj.anim.anim_time;
    remote_snap.scale = player->obj.SCALE;
    remote_snap.shadow = player->obj.shadow;
    remote_snap.target_xrot = player->obj.target_xrot;
    remote_snap.target_yrot = player->obj.target_yrot;
    remote_snap.spin_frame = player->spin_frame;
    remote_snap.spin_frames = player->spin_frames;
    remote_snap.spin = player->spin;
    remote_snap.dangle = player->obj.dangle;
    remote_snap.target = player->target;
    remote_snap.fire = player->fire;
    remote_snap.freeze = player->freeze;
    remote_snap.paused = (unsigned char)(Paused != 0);
    remote_snap.vehicle = player->obj.vehicle;
    remote_snap.vehiclecontrol = (unsigned char)VEHICLECONTROL;
    remote_snap.mask_active = (player->obj.mask != 0)
                                  ? (unsigned char)player->obj.mask->active
                                  : 0;
    pack_vehicle_xf(remote_snap.vehicle_xf);
    remote_snap.warp_level = -1; /* the ghost never warps */
    remote_snap.in_finish_range = 0;
    remote_snap.in_finish_pos[0] = 0.0f;
    remote_snap.in_finish_pos[1] = 0.0f;
    remote_snap.in_finish_pos[2] = 0.0f;
    /* Mirror our own progression: coop_merge on a mirror must be a total
     * no-op (every level/hub bit already set, every mirrored items bit's
     * object already dead) -- the in-game idempotency self-test. */
    remote_snap.bonus = (unsigned char)(Bonus != 0);
    remote_snap.items = plr_items;
    for (i = 0; i < COOP_LEVEL_COUNT; i++) {
        remote_snap.level_flags[i] = Game.level[i].flags;
    }
    remote_snap.powers = Game.powerbits;
    for (i = 0; i < COOP_HUB_COUNT; i++) {
        remote_snap.hub_flags[i] = Game.hub[i].flags;
    }
    for (i = 0; i < COOP_CRATE_WORDS; i++) {
        remote_snap.crate_bits[i] = g_crate_bits[i];
    }
    for (i = 0; i < COOP_ITEM_WORDS; i++) {
        remote_snap.item_bits[i] = 0; /* reserved (identity replay) */
    }
    remote_snap.pending_flags = (unsigned short)(new_lev_flags &
                                                 (unsigned short)~temp_lev_flags);
    remote_snap.pending_level = (short)last_level;
    remote_snap.name[0] = 'G';
    remote_snap.name[1] = 'h';
    remote_snap.name[2] = 'o';
    remote_snap.name[3] = 's';
    remote_snap.name[4] = 't';
    remote_snap.name[5] = 0;
    stale_frames = 0;
}

static struct CharacterModel *puppet_model(short character)
{
    if (character >= 0 && character < 191 && CRemap[character] != -1) {
        return &CModel[CRemap[character]];
    }
    /* Remote character's bank not loaded in this level: borrow the local
     * player's model rather than showing nothing. */
    return player != 0 ? player->obj.model : 0;
}

static int anim_ok(struct CharacterModel *model, int action)
{
    return action >= 0 && action < 118 && model->anmdata[action] != 0;
}

/* AddCreature (src/game/creature.c) minus AddGameObject: a record
 * DrawCreatures accepts (used/on set, dead 0, no vehicle, spin 0) with
 * shadow and reflection turned off via their 2000000.0f sentinels. */
static void puppet_init(short character)
{
    unsigned int *w = (unsigned int *)&g_puppet;
    struct CharacterModel *model;
    int i;
    int action;

    for (i = 0; i < (int)(sizeof(struct creature_s) / 4); i++) {
        w[i] = 0;
    }
    model = puppet_model(character);
    g_puppet.used = 1;
    g_puppet.on = 1;
    g_puppet.i_aitab = -1;
    g_puppet.obj.pLOCATOR = &g_puppet.mtxLOCATOR[0][0];
    g_puppet.obj.model = model;
    g_puppet.obj.character = character;
    /* bit0 (vflag) marks a player-controlled creature: DrawCreatures only
     * runs the on-foot spin branch and draws the held-bazooka model[1] for
     * vflag creatures. With spin/target 0 it renders identically to a plain
     * creature, so this only adds the spin+bazooka paths when those states
     * are actually published. */
    g_puppet.obj.flags = 3;
    g_puppet.obj.vehicle = -1;
    g_puppet.obj.scale = 1.0f;
    g_puppet.obj.SCALE = 1.0f;
    g_puppet.obj.shadow = 2000000.0f;
    g_puppet.obj.reflect_y = 2000000.0f;
    g_puppet.OnFootMoveInfo = &CrashMoveInfo;
    if (character >= 0 && character < 0xBF) {
        g_puppet.obj.radius = CData[character].radius;
        g_puppet.obj.RADIUS = CData[character].radius;
        g_puppet.obj.min = CData[character].min;
        g_puppet.obj.max = CData[character].max;
        g_puppet.obj.bot = g_puppet.obj.min.y;
        g_puppet.obj.top = g_puppet.obj.max.y;
    }
    action = 0x22; /* idle, with AddCreature's fallback scan */
    if (model != 0 && !anim_ok(model, action)) {
        if (model->anmdata[0] != 0) {
            action = 0;
        } else {
            for (i = 1; i < 0x76; i++) {
                if (model->anmdata[i] != 0) {
                    break;
                }
            }
            if (i < 0x76) {
                action = i;
            }
        }
    }
    g_puppet.obj.anim.action = action;
    g_puppet.obj.anim.oldaction = action;
    g_puppet.obj.anim.newaction = action;
    ResetLights(&g_puppet.lights);
    g_puppet_char = character;
    g_puppet_inits++;
    g_warp_latched = 0; /* fresh puppet: re-arm the hub warp-out effect */
}

/* Flat cool-gray tint for a paused puppet: mid ambient, no directional
 * contribution. Overwrites colour values only (safe -- never touches the
 * light-list pointers GetLights set up). */
static void apply_gray_lights(struct Nearest_Light_s *L)
{
    L->AmbCol.x = 0.55f;
    L->AmbCol.y = 0.55f;
    L->AmbCol.z = 0.62f;
    L->dir1.Colour.r = L->dir1.Colour.g = L->dir1.Colour.b = 0.0f;
    L->dir2.Colour.r = L->dir2.Colour.g = L->dir2.Colour.b = 0.0f;
    L->dir3.Colour.r = L->dir3.Colour.g = L->dir3.Colour.b = 0.0f;
    L->glbdirectional.Colour.r = 0.0f;
    L->glbdirectional.Colour.g = 0.0f;
    L->glbdirectional.Colour.b = 0.0f;
}

/* Anim MVP: hard-set the remote action and clock every frame, no blend.
 * Unknown/unloaded actions hold the last valid one. */
static void puppet_update(void)
{
    struct CharacterModel *model = g_puppet.obj.model;
    struct nuvec_s lpos;
    int action;

    g_puppet.obj.oldpos = g_puppet.obj.pos;
    g_puppet.obj.pos.x = remote_snap.pos[0];
    g_puppet.obj.pos.y = remote_snap.pos[1];
    g_puppet.obj.pos.z = remote_snap.pos[2];
    g_puppet.obj.mom.x = remote_snap.mom[0];
    g_puppet.obj.mom.y = remote_snap.mom[1];
    g_puppet.obj.mom.z = remote_snap.mom[2];
    g_puppet.obj.hdg = remote_snap.hdg;
    g_puppet.obj.xrot = remote_snap.xrot;
    g_puppet.obj.yrot = remote_snap.yrot;
    g_puppet.obj.zrot = remote_snap.zrot;
    g_puppet.obj.surface_xrot = remote_snap.surface_xrot;
    g_puppet.obj.surface_zrot = remote_snap.surface_zrot;
    if (remote_snap.scale > 0.0f) {
        g_puppet.obj.scale = remote_snap.scale;
        g_puppet.obj.SCALE = remote_snap.scale;
    }
    /* Same level as us (the show rule), so the local player's reflection
     * plane applies; the shadow height is position-dependent and comes
     * from the remote side's own obj.shadow. Both default to their
     * 2000000.0f "off" sentinels from init. */
    g_puppet.obj.reflect_y = player->obj.reflect_y;
    g_puppet.obj.shadow = remote_snap.shadow;
    /* Vehicle mode. Both sides are in the same level (the show rule), so the
     * global VEHICLECONTROL that DrawCreatures reads is already correct for the
     * puppet; feeding it the remote vehicle id makes the same function render the
     * on-foot body, the toggled/rail vehicle (model[1]), the swim body-swap, or
     * the special glider/atlas/jeep draw, exactly as for a local player. The
     * glider/atlas/jeep draw routines read only their creature argument (checked
     * against the retail asm), so passing the puppet is safe. -1 = on foot. */
    g_puppet.obj.vehicle = remote_snap.vehicle;
    /* Sub-states DrawCreatures reads to reproduce the spin whirl (its own
     * branch, driven by spin/spin_frame/dangle, not the anim packet) and the
     * held-bazooka model (drawn when target != 0). target is only tested for
     * nonzero in the draw path, never dereferenced. */
    g_puppet.spin = remote_snap.spin;
    g_puppet.spin_frame = remote_snap.spin_frame;
    g_puppet.spin_frames = remote_snap.spin_frames;
    g_puppet.obj.dangle = remote_snap.dangle;
    g_puppet.target = remote_snap.target;
    g_puppet.fire = remote_snap.fire;
    g_puppet.freeze = remote_snap.freeze;
    g_puppet.obj.target_xrot = remote_snap.target_xrot;
    g_puppet.obj.target_yrot = remote_snap.target_yrot;
    /* What ProcessCreatures does for real creatures: sample the level
     * lighting at mid-body every frame, else the model renders black. */
    if (USELIGHTS != 0 && LIGHTCREATURES != 0) {
        lpos.x = g_puppet.obj.pos.x;
        lpos.y = (g_puppet.obj.bot + g_puppet.obj.top) *
                 g_puppet.obj.SCALE * 0.5f + g_puppet.obj.pos.y;
        lpos.z = g_puppet.obj.pos.z;
        GetLights(&lpos, &g_puppet.lights, 1);
    }
    /* A paused peer freezes on its own (its pos/anim_time stop advancing at the
     * source); tint the lighting flat gray so it reads as "held". Applied after
     * GetLights so the light pointers stay valid -- this only overwrites colour
     * values, never structure. */
    if (remote_snap.paused != 0) {
        apply_gray_lights(&g_puppet.lights);
    }
    action = remote_snap.action;
    if (model == 0 || !anim_ok(model, action)) {
        action = g_puppet.obj.anim.action;
    }
    if (action != g_puppet.obj.anim.action) {
        g_puppet.obj.anim.action = action;
        g_puppet.obj.anim.oldaction = action;
        g_puppet.obj.anim.newaction = action;
        g_puppet.obj.anim.blend = 0;
    }
    g_puppet.obj.anim.anim_time = remote_snap.anim_time;
}

/* Puppet-side teleporter. The teleporter you see in-game is not a placed object:
 * JonProbe (0x1DB150) renders it every frame from the singleton probe globals --
 * the spinning ring model (Draw3DCharacter, character 0xB1) AND the glowing beam
 * (NuLgtArcLaser filaments + rising debris + a probecol intensity ramp). To get
 * the full faithful effect on the puppet we REUSE the real JonProbe, but with an
 * isolated probe CONTEXT: we save the local player's probe globals, load a
 * puppet-owned set, call JonProbe (so it advances + draws the puppet teleporter),
 * save the puppet set back, and restore the local set. The local player's own
 * teleporter/warp is never disturbed -- Hub / in_finish_range are read by the
 * local warp machinery (HubLevelSelect etc.), so they must not leak.
 *
 * The bare-ring fallback (COOP_TP_BEAM 0) draws only the static ring model; keep
 * it in case the JonProbe reuse misbehaves on hardware. */
#define COOP_TELEPORTER 1
#define COOP_TP_BEAM 1       /* 1 = full JonProbe reuse (ring+beam); 0 = bare ring */
#ifndef COOP_TP_SPIN
#define COOP_TP_SPIN 0xA3    /* bare-ring spin/frame (retail JonProbe proberot.y step) */
#endif
#ifndef COOP_TP_UP
#define COOP_TP_UP 4.5f      /* retail JonProbe trigger raises probepos.y by 4.5 so the
                              * ring starts at the top and glides down; we replicate it */
#endif
/* Feature 3 (CUSTOM warp-IN / arrival): no retail reference. */
#define COOP_TPIN 1          /* arrival teleporter + materialise */
#ifndef COOP_TPIN_FRAMES
#define COOP_TPIN_FRAMES 105 /* arrival teleporter total duration (~2.1 s @50 Hz) */
#endif
#ifndef COOP_TPIN_REVEAL
#define COOP_TPIN_REVEAL 40  /* body stays hidden this many frames so the teleporter's
                              * lighting/beam builds up before the puppet materialises */
#endif
#ifndef COOP_TPIN_DEBRIS
#define COOP_TPIN_DEBRIS 32  /* frame the arrival sparkle fires -- just before the body
                              * reveal so the debris blooms as the puppet materialises */
#endif

/* Bare ring model at node, spinning about Y (fallback path). */
static void draw_teleporter(struct nuvec_s *node)
{
    signed char slot = CRemap[COOP_TELEPORTER_CHAR];

    if (slot < 0) {
        return;
    }
    Draw3DCharacter(node, 0, (int)g_tp_rot, 0, 1.0f, 1.0f,
                    &CModel[(int)slot], -1, 0);
}

#if COOP_TP_BEAM
/* Retail probe singleton globals (types per src/game/game.c). vec3s are handled
 * as opaque 12-byte (3-word) blobs -- JonProbe only touches x/y/z at +0/+4/+8. */
extern int Hub;
/* in_finish_range + in_finish_pos are declared in retail.h (publish_local uses
 * them too). in_finish_pos is float[3] there; the probe swap treats every vec3 as
 * an int[3] bit-blob, so it is cast to (int *) below. */
extern int probeon;
extern int probey;
extern int probetime;
extern int probecol;
extern int probepos[3];
extern int probedpos[3];
extern int probepos2[3];
extern int probespk[3];
extern int proberot[3];
void JonProbe(void);

/* Puppet-owned probe state, persisted across frames (the local set is swapped in
 * and out around each JonProbe call). */
struct probe_ctx {
    int Hub, in_finish_range, probeon, probey, probetime, probecol;
    int in_finish_pos[3], probepos[3], probedpos[3], probepos2[3], probespk[3],
        proberot[3];
};
static struct probe_ctx g_pp;

static void v3cpy(int *d, const int *s) { d[0] = s[0]; d[1] = s[1]; d[2] = s[2]; }

static void v3_from_vec(int *d, struct nuvec_s *s)
{
    d[0] = *(int *)&s->x;
    d[1] = *(int *)&s->y;
    d[2] = *(int *)&s->z;
}

/* Arm the puppet probe context at warp start: pre-latched (probeon = 1) so the
 * reused JonProbe skips its trigger block -- no rumble/SFX on the LOCAL pad. But
 * the trigger is also where retail raises probepos.y by 4.5 so the ring starts
 * ABOVE the node and glides down; skipping it left the ring starting AT the node,
 * so it jerked up to the raised glide target before descending. So we seed
 * probepos at node + 4.5 (the ring's start) with probedpos/etc at the node (the
 * glide destination), exactly reproducing the trigger's initial state -- the ring
 * now appears at the top and settles down smoothly. probetime is set by JonProbe
 * itself each frame. */
static void probe_ctx_arm(struct nuvec_s *node, int instant)
{
    struct nuvec_s top;
    struct probe_ctx z = {0};
    g_pp = z;
    g_pp.probeon = 1;
    v3_from_vec(g_pp.probedpos, node);
    v3_from_vec(g_pp.probepos2, node);
    v3_from_vec(g_pp.probespk, node);
    v3_from_vec(g_pp.in_finish_pos, node);
    if (instant) {
        /* Fully-formed teleporter from frame 0: ring already settled at the node
         * (no descend) and beam risen + glowing (skip JonProbe's fade-in ramp), so
         * it is already there when the body appears immediately (hub return). */
        v3_from_vec(g_pp.probepos, node);
        g_pp.probey = 0xC8000;   /* beam fully risen (JonProbe's probey clamp) */
        g_pp.probecol = 0x1900;  /* beam fully glowing (probecol clamp) */
    } else {
        top = *node;
        top.y += COOP_TP_UP;                /* ring starts at the raised height... */
        v3_from_vec(g_pp.probepos, &top);  /* ...and glides down toward the node */
    }
}

/* Swap in the puppet probe context, run the real JonProbe (advances + draws the
 * teleporter ring + beam), swap the puppet state back, restore the local set.
 * MUST run in the draw pass, exactly once per frame. Hub/in_finish_range are
 * forced to a triggering state (Hub = 0 satisfies JonProbe's != -1 gate; it uses
 * Hub for nothing else) and in_finish_pos pinned to the node so the beam stays
 * anchored. */
static void coop_probe_run(struct nuvec_s *node)
{
    struct probe_ctx sav;

    /* save local */
    sav.Hub = Hub;
    sav.in_finish_range = in_finish_range;
    sav.probeon = probeon;
    sav.probey = probey;
    sav.probetime = probetime;
    sav.probecol = probecol;
    v3cpy(sav.in_finish_pos, (int *)in_finish_pos);
    v3cpy(sav.probepos, probepos);
    v3cpy(sav.probedpos, probedpos);
    v3cpy(sav.probepos2, probepos2);
    v3cpy(sav.probespk, probespk);
    v3cpy(sav.proberot, proberot);

    /* load puppet */
    Hub = 0;
    in_finish_range = 0x32;
    probeon = g_pp.probeon;
    probey = g_pp.probey;
    probetime = g_pp.probetime;
    probecol = g_pp.probecol;
    v3_from_vec((int *)in_finish_pos, node);
    v3cpy(probepos, g_pp.probepos);
    v3cpy(probedpos, g_pp.probedpos);
    v3cpy(probepos2, g_pp.probepos2);
    v3cpy(probespk, g_pp.probespk);
    v3cpy(proberot, g_pp.proberot);

    JonProbe();

    /* save puppet back */
    g_pp.probeon = probeon;
    g_pp.probey = probey;
    g_pp.probetime = probetime;
    g_pp.probecol = probecol;
    v3cpy(g_pp.probepos, probepos);
    v3cpy(g_pp.probedpos, probedpos);
    v3cpy(g_pp.probepos2, probepos2);
    v3cpy(g_pp.probespk, probespk);
    v3cpy(g_pp.proberot, proberot);

    /* restore local */
    Hub = sav.Hub;
    in_finish_range = sav.in_finish_range;
    probeon = sav.probeon;
    probey = sav.probey;
    probetime = sav.probetime;
    probecol = sav.probecol;
    v3cpy((int *)in_finish_pos, sav.in_finish_pos);
    v3cpy(probepos, sav.probepos);
    v3cpy(probedpos, sav.probedpos);
    v3cpy(probepos2, sav.probepos2);
    v3cpy(probespk, sav.probespk);
    v3cpy(proberot, sav.proberot);
}
#endif /* COOP_TP_BEAM */

/* The puppet's warp effect: the warp-debris sparkle AND the warp sound (SFX 0x1E,
 * what real Crash plays when it teleports). GameSfx spatialises it at the puppet's
 * position, so it only carries when the local player is in range. Used for BOTH
 * teleport-OUT (every despawn edge: hub warp, level exit, non-teleport leaves like a
 * pause-menu quit) and teleport-IN (materialise), so the puppet never appears or
 * vanishes silently. */
static void coop_warp_fx(void)
{
    AddWarpDebris(&g_puppet.obj);
    gamesfx_effect_volume = 0x7ffe;
    GameSfx(0x1E, &g_puppet.obj.pos);
}

/* Warp-OUT effects (runs in the SIM hook via coop_tick). The HUB and a LEVEL exit
 * are deliberately handled DIFFERENTLY:
 *
 *  - HUB (Feature 1): the hub level-select teleporter is *summoned* by JonProbe
 *    from spline nodes -- there is NO persistent placed pad -- so we draw it for
 *    the puppet (coop_probe_run), appearing on zone entry (in_finish_range > 0) at
 *    the synced node (in_finish_pos). The body-vanish is retail's own warp_level
 *    bracket in the draw hook.
 *  - LEVEL exit (Feature 2): the level already has its OWN placed teleporter pad
 *    (a persistent type-0xB1 creature, always drawn). Drawing a second one would
 *    be wrong. So here we ONLY fire the warp debris and hide the puppet's body, so
 *    it dissolves + despawns *into the existing pad* with the effect, instead of
 *    lingering visible until the level swap. No teleporter of our own.
 *
 * Each one-shot latched so it fires once per warp. */
static void coop_warp_effect(void)
{
    /* (1) HUB teleporter appears on zone entry (Feature 1 only) */
    if (Level == 0x25 && remote_snap.in_finish_range > 0) {
        if (!g_tp_latched) {
            /* the teleport NODE (synced), not the puppet's entry pos */
            g_tp_pos.x = remote_snap.in_finish_pos[0];
            g_tp_pos.y = remote_snap.in_finish_pos[1];
            g_tp_pos.z = remote_snap.in_finish_pos[2];
            g_tp_rot = 0;
            g_tp_frame = 0;
#if COOP_TP_BEAM
            probe_ctx_arm(&g_tp_pos, 0); /* hub warp-out: normal descend + ramp */
#endif
            g_tp_latched = 1;
        }
        g_tp_active = 1;
        g_tp_rot = (unsigned short)(g_tp_rot + COOP_TP_SPIN);
        g_tp_frame++;
    } else {
        g_tp_latched = 0;
        g_tp_active = 0;
    }

    /* (2) dissolve + despawn */
    if (Level == 0x25) {
        /* hub: commit = warp_level; body-vanish is the warp_level draw bracket */
        if (remote_snap.warp_level != -1) {
            if (!g_warp_latched) {
                coop_warp_fx();
                g_warp_latched = 1;
            }
        } else {
            g_warp_latched = 0;
        }
        g_warp_out_hide = 0;
    } else {
        /* level exit: the moment the remote reaches the exit zone
         * (in_finish_range > 0) fire the debris and hide the body, so the puppet
         * despawns WITH the effect (sooner than waiting for the level swap). */
        if (remote_snap.in_finish_range > 0) {
            if (!g_warp_latched) {
                coop_warp_fx();
                g_warp_latched = 1;
            }
            g_warp_out_hide = 1;
        } else {
            g_warp_latched = 0;
            g_warp_out_hide = 0;
        }
    }
}

#if COOP_TPIN
/* Feature 3 (CUSTOM): warp-IN. When the remote arrives in the local player's room
 * the puppet currently just pops into existence. Instead play a teleporter at the
 * arrival point and hold the body hidden for the first COOP_TPIN_REVEAL frames so
 * it materialises out of the teleporter. No retail reference -- our design; reuses
 * the same JonProbe probe primitive (coop_probe_run) as the warp-out. Armed on the
 * puppet's rising show edge; skipped if the remote arrives already in a warp-out
 * zone (that path owns the teleporter). */
static void coop_warp_in_arm(void)
{
    if (remote_snap.in_finish_range > 0) {
        return; /* arriving straight into a warp-out zone: leave it to warp-out */
    }
    g_tpin_pos = g_puppet.obj.pos; /* the point the puppet materialises at */
    g_tpin_frame = 0;
    g_tpin_active = 1;
    /* Hide the body while it materialises -- EXCEPT when returning to the HUB from a
     * level (the remote's last real room was a playable level, not the hub): there
     * Crash tumbles back out at the node, so it should appear immediately. A fresh
     * hub spawn (came from the menu / was already in the hub) still materialises. */
    g_tpin_hide = !(Level == 0x25 && g_last_remote_room >= 0 &&
                    g_last_remote_room != 0x25);
#if COOP_TP_BEAM
    /* When the body appears immediately (hub return, !g_tpin_hide) the teleporter
     * must be fully formed at once so it isn't "late"; when the body materialises
     * (hidden), let it build up normally during the hide. */
    probe_ctx_arm(&g_tpin_pos, !g_tpin_hide);
#endif
    /* the arrival sparkle is deferred to coop_warp_in_tick (near the reveal) so it
     * blooms as the body appears, not while it is still hidden */
}

static void coop_warp_in_tick(void)
{
    if (g_tpin_active) {
        if (g_tpin_frame == COOP_TPIN_DEBRIS) {
            coop_warp_fx(); /* sparkle + warp sound as it materialises in */
        }
        g_tpin_frame++;
        if (g_tpin_frame >= COOP_TPIN_FRAMES) {
            g_tpin_active = 0;
        }
    }
}

#endif /* COOP_TPIN */

/* The puppet body is hidden while it is either materialising out of the arrival
 * teleporter (Feature 3, only when g_tpin_hide) or dissolving into a level-exit pad
 * (Feature 2). g_tpin_hide is decided per-arrival in coop_warp_in_arm: a fresh spawn
 * materialises (hidden), but a puppet RETURNING to the hub from a level tumbles back
 * out at the node and so appears immediately (not hidden). */
static int coop_body_hidden(void)
{
    int hidden = g_warp_out_hide;
#if COOP_TPIN
    hidden = hidden ||
             (g_tpin_active && g_tpin_hide && g_tpin_frame < COOP_TPIN_REVEAL);
#endif
    return hidden;
}

/* The show rule IS the level-independence requirement: the puppet exists
 * only while both sides report the same room; nothing else is synced. */
static void update_puppet(void)
{
    int show;
    int arriving;

    show = local_valid() &&
           (remote_snap.flags & COOP_F_PRESENT) != 0 &&
           (remote_snap.flags & COOP_F_DEAD) == 0 &&
           remote_snap.level == Level &&
           stale_frames < STALE_LIMIT;
    if (show) {
        /* rising show edge = the remote just entered this room (arrival) */
        arriving = !g_puppet_active;
        if (!g_puppet_active || g_puppet_char != remote_snap.character ||
            g_puppet.obj.model != puppet_model(remote_snap.character)) {
            puppet_init(remote_snap.character);
        }
        puppet_update();
#if COOP_TPIN
        if (arriving) {
            coop_warp_in_arm(); /* after puppet_update so obj.pos is the arrival pt */
        }
        coop_warp_in_tick();
#endif
        coop_warp_effect();
        g_puppet_level = Level;
    } else {
        /* Falling show edge: the puppet just despawned. Fire a farewell warp debris
         * at its last position. This covers quit-to-hub from the pause menu, which
         * on the remote STOPS publishing (its coop_tick doesn't run during the quit
         * load), so the local side sees the puppet go stale rather than get a clean
         * "left" update -- hence we do NOT require COOP_F_PRESENT, a fresh level
         * field, or stale_frames < limit. A stale-despawn only happens after
         * STALE_LIMIT frames of silence, i.e. the remote really is gone (quit or
         * disconnect), both worth a farewell. We only exclude: (a) the LOCAL player
         * being the one who left (local_valid -- no level to spawn the effect in);
         * (b) the remote dying (a death, not a warp); (c) a warp-out that already
         * sparkled it (g_warp_latched: hub warp / level-exit pad). */
        if (g_puppet_active && !g_warp_latched && local_valid() &&
            (remote_snap.flags & COOP_F_DEAD) == 0) {
            coop_warp_fx();
        }
#if COOP_TPIN
        g_tpin_active = 0; /* puppet gone: drop any pending arrival fx */
#endif
        g_warp_out_hide = 0;
    }
    g_puppet_active = show;
    COOP->diag = (g_puppet_inits << 8) | (show ? 1u : 0u) | g_vs_diag;
    /* Track the remote's last REAL room (ignore -1 loading/menu) for the next
     * arrival's hide decision -- updated after coop_warp_in_arm has read it.
     * The room BEFORE it feeds the award celebration: bits of the level the
     * remote just came from are a fresh finish, everything else is catch-up. */
    if (remote_snap.level != -1 && remote_snap.level != g_last_remote_room) {
        g_remote_prev_room = g_last_remote_room;
        g_last_remote_room = remote_snap.level;
    }
}

/* Floating name tag + "Paused" label above the remote puppet.
 *
 * Two-phase because the two hooks run under different cameras:
 *  - update_label_pos() runs in the DrawCreatures hook (3D world camera active)
 *    and projects the puppet's head to screen space with NuCameraTransformScreen
 *    (NULL matrix = the world's global screen matrix). It stashes the result.
 *  - draw_puppet_label() runs in the DrawMenu hook (panel camera active, where
 *    Text3D actually composites) and draws the stashed position.
 * Set COOP_LABELS 0 to disable; COOP_LABEL_DEBUG 1 pins a fixed centre position
 * (projection bypassed) for isolating placement issues. */
#define COOP_LABELS 1
#define COOP_LABEL_DEBUG 0
#define COOP_LABEL_UP 2.8f       /* world units above the puppet origin (above the head) */
#define COOP_LABEL_SCALE 1.0f    /* base Text3D glyph scale */
#define COOP_PAUSED_SCALE 0.3f   /* PAUSED is a small status pip, much smaller than a name */
/* NuCameraTransformScreen returns PS2 GS screen coords (12.4 fixed-point). The
 * visible frame is centred on the guard-band origin 2048.0 == 32768 in 12.4,
 * spanning +/- (width/2)<<4 x and +/- (height/2)<<4 y. Map to Text3D's ~-1..1
 * ndc: ndc = (screen - centre) / half_extent. These are the hardware defaults
 * (640 wide, 512 tall PAL); nudge from coop-peek's raw screen.x/screen.y. */
#define COOP_SCR_CX 32768.0f
#define COOP_SCR_CY 32768.0f
#define COOP_SCR_HX 5120.0f
#define COOP_SCR_HY 4096.0f
/* Perspective text scale from the camera-space depth (world units along the
 * camera axis): a full-size label sits at COOP_DIST_REF units, closer grows,
 * farther shrinks. scale = REF/depth, clamped. Tune REF from peek's depth. */
#define COOP_DIST_REF 22.0f
#define COOP_SCALE_MIN 0.5f
#define COOP_SCALE_MAX 1.5f
/* Cull the label when the puppet is at/behind the camera plane: a behind-camera
 * point still projects to a (mirrored) on-screen position, which is what made
 * the tag appear when the camera swung 180 degrees around. view.z is the signed
 * depth; front is view.z > this small positive near margin. */
#define COOP_FRONT_MIN 0.5f

static float g_label_x;          /* stashed ndc position (world camera pass) */
static float g_label_y;
static float g_label_scale;      /* perspective-scaled glyph size */
static int g_label_valid;        /* projected, in front, on-screen this frame */
static unsigned int g_label_frame; /* main-pass latch */

/* Project the puppet head; call from the DrawCreatures hook (world camera).
 * View space gives the signed camera-axis depth (front/back test + true
 * perspective distance); the screen transform gives the 2D position. */
static void update_label_pos(void)
{
    struct nuvec_s world;
    struct nuvec_s screen;
    struct nuvec_s view;
    float depth;

    world.x = g_puppet.obj.pos.x;
    world.y = g_puppet.obj.pos.y + COOP_LABEL_UP;
    world.z = g_puppet.obj.pos.z;
    NuCameraTransformView(&view, &world, 1, 0);
    NuCameraTransformScreen(&screen, &world, 1, 0);

    /* Measurement stash (coop-peek prints these) for calibration. */
    modsdk_mailbox.reserved[2] = *(unsigned int *)&screen.x;
    modsdk_mailbox.reserved[3] = *(unsigned int *)&screen.y;
    modsdk_mailbox.reserved[4] = *(unsigned int *)&view.z;

    depth = view.z;
    if (depth < 0.0f) {
        depth = -depth;
    }
    g_label_scale = (depth > 0.001f) ? (COOP_DIST_REF / depth) : COOP_SCALE_MAX;
    if (g_label_scale < COOP_SCALE_MIN) {
        g_label_scale = COOP_SCALE_MIN;
    } else if (g_label_scale > COOP_SCALE_MAX) {
        g_label_scale = COOP_SCALE_MAX;
    }
#if COOP_LABEL_DEBUG
    g_label_x = 0.0f;
    g_label_y = 0.25f;
    g_label_scale = COOP_LABEL_SCALE;
    g_label_valid = 1;
#else
    g_label_x = (screen.x - COOP_SCR_CX) / COOP_SCR_HX;
    g_label_y = (screen.y - COOP_SCR_CY) / COOP_SCR_HY;
    /* Behind-camera cull + on-screen bound (permissive while calibrating;
     * tighten to ~1.3 once the mapping is dialled in). If view.z's front sign
     * is the opposite convention the tag never shows -- then flip this test. */
    g_label_valid = view.z > COOP_FRONT_MIN &&
                    g_label_x > -2.5f && g_label_x < 2.5f &&
                    g_label_y > -2.5f && g_label_y < 2.5f;
#endif
}

/* Retail Text3D glyph quirk (PAL v1.03): lowercase a-x are NOT letters -- the
 * font remaps them to inline object/icon glyphs, so any readable text must be
 * UPPERCASE. Observed a..x mapping (keep handy; '/' = renders nothing):
 *   a Crystal            b Sapphire Relic     c Gold Relic
 *   d Platinum Relic     e Crate Gem          f Time-Trial Clock
 *   g Sneak power        h Double-Jump power  i Death-Tornado-Spin power
 *   j Bazooka power      k Sprint power       l Super-Charged-Body-Slam power
 *   m /                  n /                  o DualShock Circle button
 *   p green "PlayStation(R)2" text            q /                 r /
 *   s DualShock Square   t DualShock Triangle u /
 *   v /                  w /                  x DualShock Cross button
 * Uppercase A-Z and digits render as normal glyphs. */
static void upcase_copy(char *dst, char *src, int max)
{
    int i;
    char c;

    for (i = 0; i < max - 1 && src[i] != 0; i++) {
        c = src[i];
        if (c >= 'a' && c <= 'z') {
            c = (char)(c - 'a' + 'A');
        }
        dst[i] = c;
    }
    dst[i] = 0;
}

/* Draw the stashed label; call from the DrawMenu hook (Text3D-friendly phase). */
static void draw_puppet_label(void)
{
    float x = g_label_x;
    float y = g_label_y;
    float s = g_label_scale;
    char txt[16];

    if (!g_label_valid) {
        return;
    }
    /* Paused takes over the slot entirely -- the name hides so they don't
     * overlap; otherwise show the name. */
    if (remote_snap.paused != 0) {
        float ps = s * COOP_PAUSED_SCALE; /* much smaller status pip above the head */
        txt[0] = 'P';
        txt[1] = 'A';
        txt[2] = 'U';
        txt[3] = 'S';
        txt[4] = 'E';
        txt[5] = 'D';
        txt[6] = 0;
        Text3D(txt, 8, 4, x, y, 1.0f, ps, ps, ps);
    } else if (remote_snap.name[0] != 0) {
        upcase_copy(txt, remote_snap.name, 16);
        Text3D(txt, 8, 4, x, y, 1.0f, s, s, s);
    }
}

/* Aku Aku mask floating over the puppet. Set COOP_PUPPET_MASK 0 to disable.
 *
 * DrawMask(mask_s*) draws the mask model at the mask's own baked matrices
 * (mM/mS at offset 0); only its ground-shadow pass reads the global player, so
 * we bracket that around the call. We keep a puppet-owned mask so we never
 * disturb the local player's own mask. It is initialised with NewMask (which
 * sets character 3 / model / anim / lights) the first time it activates -- doing
 * it this way, rather than copying the level's global Mask, is what makes it work
 * on BOTH instances: the global Mask only has its anim/model set up on the side
 * whose local player has actually triggered a mask, which is why the copy path
 * rendered the peer's mask on one instance but only its shadow on the other.
 * mask_active 1/2 = shield strength, >2 = invincible; the engine clamps >2 to 2
 * for drawing (creature.c), so pass it straight through. */
#define COOP_PUPPET_MASK 1
#if COOP_PUPPET_MASK
static struct mask_s g_puppet_mask;
static int g_puppet_mask_ready;

static void draw_puppet_mask(void)
{
    struct creature_s *saved;

    if (remote_snap.mask_active == 0) {
        g_puppet_mask.active = 0;
        g_puppet_mask_ready = 0;
        return;
    }
    if (!g_puppet_mask_ready) {
        NewMask(&g_puppet_mask, &g_puppet.obj.pos);
        g_puppet_mask_ready = 1;
    }
    g_puppet_mask.active = remote_snap.mask_active;
    UpdateMask(&g_puppet_mask, &g_puppet.obj);
    /* DrawMask's shadow pass reads the global player; point it at the puppet so
     * the mask shadow lands under the puppet, then restore immediately. */
    saved = player;
    player = &g_puppet;
    DrawMask(&g_puppet_mask);
    player = saved;
}
#endif

/* Glider and Atlas draw entirely from a per-vehicle NEWBUGGY state struct at
 * creature+0x224 (glider position/attitude at Buggy+0x30/0x70.., atlas ball at
 * Buggy+0x20C/quat 0x284), NOT from obj.pos -- so with the puppet's Buggy NULL
 * nothing renders. Now that NEWBUGGY is typed (unit game/vehicle: DrawGlider /
 * DrawAtlas), the puppet owns one and we drive it from the remote player's
 * synced vehicle_xf: the glider banks with the remote's roll/pitch/yaw and the
 * ball sits + spins where the remote's does, instead of copying the local
 * player's vehicle. This replaces the old borrow-the-local-Buggy hack -- no
 * scribbling on the live player's state. Set COOP_SPECIAL_VEH 0 to disable (the
 * puppet just won't render in glider/atlas). The jeep needs none of this:
 * DrawJeep builds its matrix from pos/hdg. DrawCreatures only takes the vehicle
 * path when the global VEHICLECONTROL == 1, so this is a shared-vehicle-level
 * feature (both players in the same vehicle level, as expected for coop). */
#define COOP_SPECIAL_VEH 1
#if COOP_SPECIAL_VEH
static struct NEWBUGGY g_puppet_buggy;  /* puppet-owned vehicle transform */

static void setup_vehicle_buggy(void)
{
    short v = remote_snap.vehicle;
    float *xf = remote_snap.vehicle_xf;

    g_puppet.Buggy = 0;
    if (VEHICLECONTROL != 1) {
        return;
    }
    if (coop_is_glider(v)) {
        g_puppet_buggy.pitch = xf[0];
        g_puppet_buggy.roll = xf[1];
        g_puppet_buggy.yaw = xf[2];
        g_puppet_buggy.pos.x = xf[3];
        g_puppet_buggy.pos.y = xf[4];
        g_puppet_buggy.pos.z = xf[5];
        g_puppet_buggy.mode = 0;
        g_puppet_buggy.enable = (int)xf[6];
        g_puppet.Buggy = &g_puppet_buggy;
    } else if (coop_is_atlas(v)) {
        g_puppet_buggy.ball_pos.x = xf[0];
        g_puppet_buggy.ball_pos.y = xf[1];
        g_puppet_buggy.ball_pos.z = xf[2];
        g_puppet_buggy.rotquat.x = xf[3];
        g_puppet_buggy.rotquat.y = xf[4];
        g_puppet_buggy.rotquat.z = xf[5];
        g_puppet_buggy.rotquat.w = xf[6];
        g_puppet.Buggy = &g_puppet_buggy;
    }
}

static void teardown_vehicle_buggy(void)
{
    g_puppet.Buggy = 0;
}
#endif

/* Replace hook on DrawCreatures (0x1D2F50): pass everything through, and
 * whenever the engine draws the player pass (Character, count 1 -- the
 * main render and the hub reflection pass; NPCs go through &Character[1])
 * draw the puppet with the same render/shadow arguments. */
void coop_draw_creatures(struct creature_s *c, s32 count, s32 render,
                         s32 shadow)
{
    short save_xrot;
    short save_yrot;
    int save_vctl;
    float save_vtog;
    int save_warp;

    /* Publish our fresh, post-simulation state on the frame's first player
     * pass (main render precedes the hub reflection pass). ProcessCreatures
     * has already run, so pos/anim/aim are this frame's -- a frame fresher
     * than the DoInput pre-hook. coop_tick already published + claimed the
     * frame when we are not in a playable state. */
    if (published_frame != modsdk_mailbox.frame && c == Character &&
        count == 1) {
        publish_local();
        published_frame = modsdk_mailbox.frame;
    }

    /* VS: tint every crystal creature's light record before the draw --
     * the body model is lit (its baked vertex colours are black; the pink
     * is texture x c->lights) and items are not in Character[], so nothing
     * recomputes these. One-way: leaving VS mid-level keeps the last tint
     * until the level reloads (items' lights are never refreshed). */
    if (g_vs_want >= 0 && render != 0) {
        s32 i;

        for (i = 0; i < count; i++) {
            if (c[i].on != 0 &&
                c[i].obj.character == COOP_VS_CRYSTAL_CHAR) {
                coop_vs_light_tint(&c[i].lights);
                g_vs_diag |= 0x10u; /* bit 4: body light tint ran this frame */
            }
        }
    }

    orig_DrawCreatures(c, count, render, shadow);
    if (g_puppet_active != 0 && c == Character && count == 1 &&
        Level == g_puppet_level && g_puppet.obj.model != 0) {
        /* DrawCharacterModel poses the aim joint from the GLOBAL player's
         * obj.target_xrot/yrot, not from the creature it is drawing. Point
         * it at the remote aim for the duration of the puppet pass so the
         * puppet's bazooka aims where the remote player aims, then restore
         * so the local player is unaffected. */
        save_xrot = player->obj.target_xrot;
        save_yrot = player->obj.target_yrot;
        player->obj.target_xrot = remote_snap.target_xrot;
        player->obj.target_yrot = remote_snap.target_yrot;
        /* DrawCreatures decides the puppet's vehicle from the GLOBAL
         * VEHICLECONTROL -- the LOCAL player's mount state -- not the puppet's.
         * When the two players are in different states (one on foot, one in the
         * mech), that renders the peer's vehicle from our own mount state. Drive
         * both from the remote's own vehiclecontrol for the puppet pass, and
         * force vtog_time == vtog_duration so a model[1] vehicle (mech, scooter,
         * ...) draws as fully mounted regardless of our transition, then restore
         * so the local player (already drawn above) is untouched. */
        save_vctl = VEHICLECONTROL;
        save_vtog = vtog_time;
        VEHICLECONTROL = remote_snap.vehiclecontrol;
        vtog_time = vtog_duration;
        /* Hub teleport-out: DrawCreatures (creature.c:3584) stops drawing the
         * player body while Level==0x25 && warp_level!=-1 -- retail's own "Crash
         * dissolves into the hub teleporter" vanish. It reads the GLOBAL
         * warp_level (our LOCAL warp state), so drive it from the remote's for
         * the puppet pass: the body vanishes the instant the remote commits to
         * the warp, in sync with the debris (coop_warp_effect) and with the real
         * Crash on the peer -- instead of the frozen body lingering the ~1 s
         * until the remote's level field finally flips. Also stops the puppet
         * wrongly vanishing when the LOCAL player is the one warping. */
        save_warp = warp_level;
        warp_level = remote_snap.warp_level;
#if COOP_SPECIAL_VEH
        setup_vehicle_buggy();
#endif
        /* Hide the body while it materialises out of an arrival teleporter
         * (Feature 3) or dissolves into a level-exit pad (Feature 2). The bracket
         * above is still set/restored so the local player is unaffected. */
        if (!coop_body_hidden())
            orig_DrawCreatures(&g_puppet, 1, render, shadow);
#if COOP_SPECIAL_VEH
        teardown_vehicle_buggy();
#endif
        VEHICLECONTROL = save_vctl;
        vtog_time = save_vtog;
        warp_level = save_warp;
        player->obj.target_xrot = save_xrot;
        player->obj.target_yrot = save_yrot;
#if COOP_PUPPET_MASK
        /* Aku Aku over the puppet, after the body so it composites on top (also
         * hidden while the body is materialising / dissolving). */
        if (!coop_body_hidden())
            draw_puppet_mask();
#endif
#if COOP_TELEPORTER
        /* The teleporter at the puppet's node -- one primitive for BOTH the
         * warp-OUT (Feature 1/2, at in_finish_pos, armed in coop_warp_effect) and
         * the warp-IN (Feature 3, at the arrival point, armed in coop_warp_in_arm).
         * They are mutually exclusive (leaving vs arriving), so at most one is
         * active; pick its position.
         *   COOP_TP_BEAM: reuse the real JonProbe (ring + glowing beam) with an
         *   isolated probe context -- once per frame (it advances state + spawns
         *   debris; the hub player pass runs twice, main + reflection, so guard).
         *   Fallback: the bare spinning ring, drawn every pass. */
        {
            struct nuvec_s *tele_pos = 0;
            if (g_tp_active) {
                tele_pos = &g_tp_pos;
            }
#if COOP_TPIN
            else if (g_tpin_active) {
                tele_pos = &g_tpin_pos;
            }
#endif
            if (tele_pos != 0) {
#if COOP_TP_BEAM
                if (g_tp_frame_drawn != modsdk_mailbox.frame) {
                    g_tp_frame_drawn = modsdk_mailbox.frame;
                    coop_probe_run(tele_pos);
                }
#else
                draw_teleporter(tele_pos);
#endif
            }
        }
#endif
#if COOP_LABELS
        /* Project the label here, under the 3D world camera, once per frame
         * (the player pass runs twice in the hub: main first, then reflection).
         * The DrawMenu hook draws it later using this stashed position. */
        if (g_label_frame != modsdk_mailbox.frame) {
            g_label_frame = modsdk_mailbox.frame;
            update_label_pos();
        }
#endif
    }
}

/* Version banner on the menus so both players can eyeball-confirm the modded
 * build loaded and that their layout versions match before playing (a mismatched
 * COOP_MAILBOX_VERSION means no sync). Shown on the in-game pause menu only
 * (see the gate in coop_draw_menu) -- not on the title/front-end menus, not on
 * the live HUD. This is coop-mod-scoped; a general "mods loaded" overlay would
 * instead live in the SDK. It rides the same Text3D path DrawMenu already uses,
 * so it renders wherever the menu text does. UPPERCASE only (lowercase a-x are
 * icon glyphs); the trailing digit is patched to COOP_MAILBOX_VERSION at init. */
#define COOP_BANNER 1
static char coop_banner[] = "CRASH: TWOC: MULTIPLAYER V0";

static void draw_version_banner(void)
{
    /* Centre-bottom, small. align 8 = centre, colour 4 = visible. */
    Text3D(coop_banner, 8, 4, 0.0f, 0.88f, 1.0f, 0.5f, 0.5f, 0.5f);
}

/* Replace hook on DrawMenu (0x23B... called from DrawPanel): DrawPanel sets up
 * the panel camera/viewport and then repeatedly does Text3D (queue glyphs into
 * font3d_scene) -> NuRndrGScnObj (submit). DrawMenu is called every frame in
 * the middle of that, with the panel camera active and more submit batches
 * still to come -- so text queued right after DrawMenu is rendered in the
 * correct HUD context. (Text queued outside DrawPanel, or after its last
 * submit, is silently dropped -- that was the invisible-text bug.) */
void coop_draw_menu(void *cursor, s32 paused)
{
    orig_DrawMenu(cursor, paused);
#if COOP_LABELS
    if (g_puppet_active != 0) {
        draw_puppet_label();
    }
#endif
#if COOP_BANNER
    /* In-game pause menu only (Paused != 0): never on the title/front-end menus
     * and never on the live HUD. Pause in a level to eyeball-match versions. */
    if (Paused != 0) {
        draw_version_banner();
    }
#endif
}

static void coop_init(void)
{
    volatile unsigned int *w = (volatile unsigned int *)COOP;
    int i;

    for (i = 0; i < (int)(sizeof(CoopMailbox) / 4); i++) {
        w[i] = 0;
    }
    COOP->magic = COOP_MAILBOX_MAGIC;
    COOP->version = COOP_MAILBOX_VERSION;
#if COOP_BANNER
    coop_banner[sizeof(coop_banner) - 2] =
        (char)('0' + (COOP_MAILBOX_VERSION % 10u));
#endif
    modsdk_mailbox.frame = 0;
    modsdk_mailbox.version = MODSDK_MAILBOX_VERSION;
    modsdk_mailbox.magic = MODSDK_MAILBOX_MAGIC;
}

void coop_tick(void)
{
    if (modsdk_mailbox.magic != MODSDK_MAILBOX_MAGIC ||
        COOP->magic != COOP_MAILBOX_MAGIC) {
        coop_init();
    }
    modsdk_mailbox.frame++;
    /* Consume + prepare the puppet before this frame's DrawCreatures. Our
     * own state is published later, from the draw hook, so the peer sees
     * this frame's post-simulation state instead of the previous frame's
     * (one 50 Hz frame less latency). But when we are NOT in a playable
     * state the player pass never runs, so publish the "not present" slot
     * right here to hide the peer's puppet promptly (menus, FMV), and claim
     * this frame so the draw hook does not publish a second time. */
    if (!local_valid() || Paused != 0) {
        publish_local();
        published_frame = modsdk_mailbox.frame;
    }
    consume_remote();
    if ((COOP->ctl & COOP_CTL_GHOST) != 0) {
        synth_ghost();
    }
    g_vs_mode = (COOP->ctl & COOP_CTL_VS) != 0;
    g_vs_p2 = (COOP->ctl & COOP_CTL_P2) != 0;
    /* Stage 3a: per-level bitmaps die with the room they were built in;
     * reset BEFORE merging so a stale bitmap never replays into a new
     * level that reuses the same slot indices. */
    if (Level != g_sync_level) {
        g_sync_level = Level;
        coop_progress_reset();
    }
#if COOP_AWARD
    /* Frame-accurate award-pop cue; must latch every tick (even when a spawn
     * is gated) so a stale edge can never fire late. */
    coop_award_pop_edge();
#endif
    coop_merge();
    coop_vs_tint_tick();
    update_puppet();
}
