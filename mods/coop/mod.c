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

/* orig_ thunks the SDK provides for the replace hooks. */
void orig_DrawCreatures(struct creature_s *c, s32 count, s32 render,
                        s32 shadow);
void orig_DrawMenu(void *cursor, s32 paused);

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

static int local_valid(void)
{
    return LEVEL_PLAYABLE(Level) && PLAYERCOUNT != 0 && player != 0 &&
           player->used != 0;
}

static void publish_local(void)
{
    volatile CoopSlot *s = &COOP->local;

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
        for (i = 0; i < 16; i++) {
            tmp.name[i] = r->name[i];
        }
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

/* The show rule IS the level-independence requirement: the puppet exists
 * only while both sides report the same room; nothing else is synced. */
static void update_puppet(void)
{
    int show;

    show = local_valid() &&
           (remote_snap.flags & COOP_F_PRESENT) != 0 &&
           (remote_snap.flags & COOP_F_DEAD) == 0 &&
           remote_snap.level == Level &&
           stale_frames < STALE_LIMIT;
    if (show) {
        if (!g_puppet_active || g_puppet_char != remote_snap.character ||
            g_puppet.obj.model != puppet_model(remote_snap.character)) {
            puppet_init(remote_snap.character);
        }
        puppet_update();
        g_puppet_level = Level;
    }
    g_puppet_active = show;
    COOP->diag = (g_puppet_inits << 8) | (show ? 1u : 0u);
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
#define COOP_LABEL_UP 2.0f       /* world units above the puppet origin */
#define COOP_LABEL_SCALE 1.0f    /* base Text3D glyph scale */
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
        txt[0] = 'P';
        txt[1] = 'A';
        txt[2] = 'U';
        txt[3] = 'S';
        txt[4] = 'E';
        txt[5] = 'D';
        txt[6] = 0;
        Text3D(txt, 8, 4, x, y, 1.0f, s, s, s);
    } else if (remote_snap.name[0] != 0) {
        upcase_copy(txt, remote_snap.name, 16);
        Text3D(txt, 8, 4, x, y, 1.0f, s, s, s);
    }
}

/* Replace hook on DrawCreatures (0x1D2F50): pass everything through, and
 * whenever the engine draws the player pass (Character, count 1 -- the
 * main render and the hub reflection pass; NPCs go through &Character[1])
 * draw the puppet with the same render/shadow arguments. */
void coop_draw_creatures(struct creature_s *c, s32 count, s32 render,
                         s32 shadow)
{
    short save_xrot;
    short save_yrot;

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
        orig_DrawCreatures(&g_puppet, 1, render, shadow);
        player->obj.target_xrot = save_xrot;
        player->obj.target_yrot = save_yrot;
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
    update_puppet();
}
