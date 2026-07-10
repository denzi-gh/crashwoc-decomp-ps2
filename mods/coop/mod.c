/* Coop, PR 1: per-frame state exchange through the SDK mailbox.
 * publish: local player state -> CoopMailbox.local (bridge reads it).
 * consume: CoopMailbox.remote (bridge writes it) -> remote_snap.
 * The seq-lock protocol is documented in coop_mailbox.h. */
#include "mailbox.h"
#include "retail.h"
#include "creature.h"
#include "coop_mailbox.h"

#define COOP ((volatile CoopMailbox *)modsdk_mailbox.payload)

/* Level ids that are not playable rooms: menu/save screen, game-over,
 * FMV players. Everything else (hubs included) is a room a puppet can
 * appear in. */
#define LEVEL_PLAYABLE(l) \
    ((l) != 0x25 && (l) != 0x26 && (l) != 0x27 && (l) != 0x29)

static CoopSlot remote_snap;         /* last accepted remote state */
static unsigned int remote_gen;      /* seq of remote_snap */
static unsigned int remote_frame;    /* last seen remote frame tick */
static unsigned int stale_frames;    /* frames since remote_frame advanced */
static unsigned int local_gen;       /* our seq-lock generation */
static unsigned int local_frame;     /* our tick, mirrored into the slot */

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
}
