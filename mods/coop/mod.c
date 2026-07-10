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

/* orig_ thunk the SDK provides for the replace hook. */
void orig_DrawCreatures(struct creature_s *c, s32 count, s32 render,
                        s32 shadow);

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
    } else {
        s->level = -1;
        s->flags = 0;
    }
    s->seq_close = local_gen;
}

static void consume_remote(void)
{
    volatile CoopSlot *r = &COOP->remote;
    unsigned int c;
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
    g_puppet.obj.flags = 2;
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
    /* What ProcessCreatures does for real creatures: sample the level
     * lighting at mid-body every frame, else the model renders black. */
    if (USELIGHTS != 0 && LIGHTCREATURES != 0) {
        lpos.x = g_puppet.obj.pos.x;
        lpos.y = (g_puppet.obj.bot + g_puppet.obj.top) *
                 g_puppet.obj.SCALE * 0.5f + g_puppet.obj.pos.y;
        lpos.z = g_puppet.obj.pos.z;
        GetLights(&lpos, &g_puppet.lights, 1);
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

/* Replace hook on DrawCreatures (0x1D2F50): pass everything through, and
 * whenever the engine draws the player pass (Character, count 1 -- the
 * main render and the hub reflection pass; NPCs go through &Character[1])
 * draw the puppet with the same render/shadow arguments. */
void coop_draw_creatures(struct creature_s *c, s32 count, s32 render,
                         s32 shadow)
{
    orig_DrawCreatures(c, count, render, shadow);
    if (g_puppet_active != 0 && c == Character && count == 1 &&
        Level == g_puppet_level && g_puppet.obj.model != 0) {
        orig_DrawCreatures(&g_puppet, 1, render, shadow);
    }
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
    publish_local();
    consume_remote();
    if ((COOP->ctl & COOP_CTL_GHOST) != 0) {
        synth_ghost();
    }
    update_puppet();
}
