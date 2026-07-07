/* .\creature.c -- creatures, the player, character models (unit 91).
 *
 * PS2 PAL v1.03 reconstruction.  The GameCube decompilation
 * (denzi-gh/crashwoc-decomp-gc, src/gamecode/creature.c) is the semantic
 * reference; every function here is matched against the retail PS2 bytes
 * (expected/pal103/091_creature.c.o).  This file contains only the
 * decompiled functions; the rest of the unit is spliced from retail
 * assembly by tools/gen_hybrid.py, driven by
 * config/pal103/status/game/creature.toml.
 */

#include "creature.h"

/* --- externals ---------------------------------------------------- */

/* AI table entry (AITab): PS2 stride 0x80; only ai_type (+0x0) and the
 * start positions (+0x14) are used here -- the rest is opaque. */
struct aitab_s {
    u8 ai_type;              /* 0x00 */
    u8 unk_0x01[0x13];       /* 0x01 (unverified) */
    struct nuvec_s pos[9];   /* 0x14 (9 = (0x80-0x14)/0xC) */
}; /* 0x80 */

/* AI type entry (AIType): PS2 stride 0x1C; character (+0x0) and the
 * command table pointer (+0x4) verified in AddCreature. */
struct aitype_s {
    short character;         /* 0x00 */
    short unk_0x02;          /* 0x02 */
    struct creatcmd_s *cmd;  /* 0x04 */
    u8 unk_0x08[0x14];       /* 0x08 (unverified) */
}; /* 0x1C */

/* Panel counter (plr_lives 0x632038, plr_wumpas 0x632028,
 * plr_crates 0x632030): count +0x0, draw +0x2, frame +0x4. */
struct panelcount_s {
    short count;
    short draw;
    signed char frame;
    u8 pad[3];
};

/* Only the fields creature.c touches are typed; sizes unverified. */
struct game_s {
    u8 unk_0x000[0x3FC];     /* 0x000 (opaque) */
    u8 lives;                /* 0x3FC */
    u8 wumpas;               /* 0x3FD */
};

struct ldata_s {
    u8 unk_0x00[0x24];       /* 0x00 (opaque) */
    unsigned short flags;    /* 0x24 */
    u8 unk_0x26[2];          /* 0x26 */
    unsigned short vehicle;  /* 0x28 */
};

struct leveldata_s {
    u8 unk_0x00[0x23];       /* 0x00 (opaque) */
    signed char hub;         /* 0x23 */
    u8 unk_0x24[0x30];       /* 0x24 (opaque) */
}; /* 0x54 */

struct gamecam_s {
    u8 unk_0x000[0x114];     /* 0x000 (opaque) */
    signed char mode;        /* 0x114 */
};

struct pad_s {
    u8 unk_0x000[0x564];     /* 0x000 (opaque) */
    unsigned int buttons;    /* 0x564 */
};

extern float IDLEWAIT;
extern float SAFEY;
extern s32 Level;
extern s32 GameMode;
extern s32 Adventure;
extern s32 FRAME;
extern s32 PLAYERCOUNT;
extern s32 USELIGHTS;
extern s32 LIGHTCREATURES;
extern s32 level_part_2;
extern s32 cRPosCOUNT;
extern s32 TimeTrial;
extern s32 VEHICLECONTROL;
extern s32 Hub;
extern s32 i_ring;
extern s32 gamecut_hack;
extern s32 last_hub;
extern s32 last_level;
extern s32 tumble_action;
extern float tumble_duration;
extern float tumble_time;
extern struct nuvec_s *pos_START;
extern struct nuvec_s v000;
extern struct nuvec_s cutpos_CRASH;
extern unsigned short cutang_CRASH;
extern struct nuvec_s D_0061D318;     /* TimeTrial restart position (.rodata) */

extern struct MoveInfo CrashMoveInfo;
extern struct MoveInfo CocoMoveInfo;
extern struct MoveInfo DefaultMoveInfo;
extern CharacterData CData[191];
extern struct aitab_s AITab[];
extern struct aitype_s AIType[];
extern struct game_s Game;
extern struct mask_s Mask;
extern struct ldata_s *LDATA;
extern struct leveldata_s LData[];
extern struct gamecam_s GameCam;
extern struct pad_s *Pad[];
extern struct panelcount_s plr_lives;
extern struct panelcount_s plr_wumpas;
extern void *app_tbset;

void *memset(void *s, int c, unsigned int n);
void RemoveGameObject(struct obj_s *obj);
s32 AddGameObject(struct obj_s *obj, struct creature_s *c);
void NuHGobjDestroy(struct NUHGOBJ_s *hobj);
void NuHGobjPOIMtx(struct NUHGOBJ_s *hobj, u8 i, struct numtx_s *mC,
                   struct numtx_s *tmtx, struct numtx_s *m);
void ResetLights(struct Nearest_Light_s *lights);
void GetLights(struct nuvec_s *pos, struct Nearest_Light_s *lights, s32 mode);
void GetTopBot(struct creature_s *c);
void NewTopBot(struct obj_s *obj);
void HubStart(struct obj_s *obj, s32 hub, s32 level, struct nuvec_s *pos);
void MoveCreature(struct creature_s *c);
void ComplexRailPosition(struct nuvec_s *pos, s32 iRAIL, s32 iALONG,
                         struct RPos_s *rpos, s32 mode);
void BlendGameCamera(struct gamecam_s *cam, float t);
void UpdateMask(struct mask_s *mask, struct obj_s *obj);
void HubSelect(struct creature_s *c);
void HubLevelSelect(struct obj_s *obj, s32 hub);
void HubMoveVR(void);
void CheckPlayerEvents(struct obj_s *obj);
void CheckFinish(struct obj_s *obj);
void CheckGates(struct obj_s *obj);
void CheckRings(struct obj_s *obj, s32 *ring);
void tbslotBeginFn(void *tbset, s32 slot);
void tbslotEndFn(void *tbset, s32 slot);


/* Still held out of this file: ManageCreatures and MovePlayer (switch jump
 * tables -> .rodata) and EvalModelAnim / DrawCharacterModel / DrawCreatures
 * (initialized local aggregates such as `short layertab[2] = {0, 1}`
 * -> .sdata). Float literal pools (.lit4) are supported: gen_hybrid maps
 * pool-bound li.s constants onto the retail .lit4 slots the function's own
 * retail slice references. */


float ModelAnimDuration(u32 character, u32 action, float start, float end)
{
    f32 t;
    struct CharacterModel *model;
    s32 index;

    if ((character > 0xBE) || (action > 0x75)) {
        return 1.0f;
    }
    index = CRemap[character];
    if (index == -1) {
        return 1.0f;
    }
    model = &CModel[index];
    if (model->anmdata[action] == 0) {
        return 1.0f;
    }
    t = model->anmdata[action]->time;
    if ((start >= 1.0f) && (start < t)) {
        if ((end >= 1.0f) && (end < t) && (start < end)) {
            t = end - start;
        } else {
            t -= start;
        }
    } else if ((end >= 1.0f) && (end < t)) {
        if (start < end) {
            t = end - 1.0f;
        }
    }
    return t * (1.0f / model->animlist[action]->speed) * 0.033333335f;
}


void TerrainFailsafe(struct obj_s *obj)
{
    if (obj->shadow != 2000000.0f) {
        return;
    }
    obj->shadow = SAFEY;
    if (obj->pos.y + obj->bot * obj->SCALE < SAFEY) {
        obj->pos.y = SAFEY - obj->min.y * obj->SCALE;
    }
}


void ChangeCharacter(struct creature_s *c, s32 character)
{
    CharacterData *cdata;

    if (((u32)character < 0xBF) && (CRemap[character] != -1)) {
        c->obj.character = character;
        cdata = &CData[character];
        if ((s16)character == 0) {
            c->OnFootMoveInfo = &CrashMoveInfo;
        } else if ((s16)character == 1) {
            c->OnFootMoveInfo = &CocoMoveInfo;
        } else {
            c->OnFootMoveInfo = &DefaultMoveInfo;
        }
        c->obj.model = &CModel[CRemap[character]];
        c->obj.radius = c->obj.RADIUS = cdata->radius;
        c->obj.min = cdata->min;
        c->obj.max = cdata->max;
        c->obj.bot = c->obj.min.y;
        c->obj.top = c->obj.max.y;
        if (c == player) {
            /* ResetPlayerMoves' body, inlined by hand in the retail
             * source (there is no call here in the PS2 binary). */
            c->jump = 0;
            c->slam = 0;
            c->spin = 0;
            c->crawl = 0;
            c->tiptoe = 0;
            c->sprint = 0;
            c->somersault = 0;
            c->land = 0;
            c->idle_mode = 0;
            c->idle_sigh = 0;
            c->crawl_lock = 1;
            c->crouch_pos = 0;
            c->slam_wait = 0;
            c->spin_wait = 0;
            c->slide = 0;
            c->idle_action = 0x22;
            c->idle_wait = IDLEWAIT * 30.0f;
            c->obj.idle_gametime = 0.0f;
            c->idle_time = 0.0f;
            c->target = 0;
            c->fire = 0;
            c->tap = 0;
            c->freeze = 0;
            c->obj.transporting = 0;
            ResetAnimPacket(&c->obj.anim, 0x22);
            c->obj.frame = 0;
            c->obj.attack = 0;
            c->obj.dyrot = 0;
            c->obj.boing = 0;
            c->obj.dangle = 0;
            c->obj.old_ground = 3;
            c->obj.submerged = 0;
            c->obj.SCALE = 1.0f;
            c->obj.scale = 1.0f;
            c->obj.RADIUS = c->obj.radius;
            c->obj.ground = 3;
        } else {
            ResetAnimPacket(&c->obj.anim, 0x22);
        }
    }
}


void PlayerStartPos(struct creature_s *c, struct nuvec_s *pos)
{
    if (Level == 0x25) {
        if (gamecut_hack == 1) {
            gamecut_hack = 0;
            last_level = 0x15;
            last_hub = 0;
        }
        if (GameMode == 1) {
            *pos = cutpos_CRASH;
            c->obj.hdg = cutang_CRASH;
            tumble_action = -1;
            tumble_duration = 0.0f;
            tumble_time = 0.0f;
            last_level = -1;
            last_hub = -1;
            return;
        }
        if ((last_hub != -1) && (last_level != -1) &&
            (LData[last_level].hub != -1)) {
            HubStart(&c->obj, last_hub, last_level, pos);
            return;
        }
    }
    if (pos_START != 0) {
        *pos = *pos_START;
        if (Level == 0x25) {
            c->obj.hdg = 0x8000;
            tumble_action = -1;
            tumble_duration = 0.0f;
            tumble_time = 0.0f;
            last_level = -1;
            last_hub = -1;
        }
    } else {
        *pos = v000;
    }
}


s32 AddCreature(s32 character, s32 index, s32 i_aitab)
{
    s32 i;
    struct creature_s *c;
    struct creatcmd_s *commands;
    struct nuvec_s startpos;
    CharacterData *cdata;

    if (((u32)index < 9) && ((u32)character < 0xBF) &&
        ((i_aitab == -1) ||
         (character == AIType[AITab[i_aitab].ai_type].character)) &&
        (CRemap[character] != -1)) {
        c = &Character[index];
        if (c->used != 0) {
            return 0;
        }
        memset(c, 0, 0xCE4);
        if (AddGameObject(&c->obj, c) == 0) {
            return 0;
        }
        c->obj.pLOCATOR = &c->mtxLOCATOR[0][0];
        c->used = 1;
        c->on = 1;
        c->off_wait = 2;
        cdata = &CData[character];
        if (i_aitab == -1) {
            commands = 0;
            PlayerStartPos(c, &startpos);
        } else {
            commands = AIType[AITab[i_aitab].ai_type].cmd;
            startpos = AITab[i_aitab].pos[commands->i];
        }
        c->i_aitab = i_aitab;
        c->cmdtable = commands;
        c->cmdcurr = commands;
        c->obj.character = character;
        if ((s16)character == 0) {
            c->OnFootMoveInfo = &CrashMoveInfo;
        } else if ((s16)character == 1) {
            c->OnFootMoveInfo = &CocoMoveInfo;
        } else {
            c->OnFootMoveInfo = &DefaultMoveInfo;
        }
        c->obj.radius = c->obj.RADIUS = cdata->radius;
        c->obj.model = &CModel[CRemap[character]];
        c->obj.min = cdata->min;
        c->obj.max = cdata->max;
        c->obj.bot = c->obj.min.y;
        c->obj.top = c->obj.max.y;
        c->obj.startpos = startpos;
        c->obj.oldpos = c->obj.startpos;
        c->obj.pos = c->obj.oldpos;
        c->obj.flags = 2;
        if ((Adventure != 0) && (c == player)) {
            c->obj.vehicle = LDATA->vehicle;
            c->obj.flags = 3;
            plr_lives.count = Game.lives;
            plr_wumpas.count = Game.wumpas;
            c->obj.mask = &Mask;
            c->pad_type = 1;
            plr_lives.draw = Game.lives;
            plr_wumpas.draw = Game.wumpas;
        } else {
            c->obj.vehicle = -1;
            c->obj.flags = c->obj.flags | 4;
            c->pad_type = 0;
            c->obj.mask = 0;
        }
        c->obj.die_duration = 0.5f;
        c->ai.oldpos = c->obj.pos;
        c->obj.anim.action = 0x22;
        if (c->obj.model != 0) {
            if (c->obj.model->anmdata[0x22] == 0) {
                if (c->obj.model->anmdata[0] != 0) {
                    i = 0;
                } else {
                    for (i = 1; i < 0x76; i++) {
                        if (c->obj.model->anmdata[i] != 0) {
                            break;
                        }
                    }
                }
                if (i < 0x76) {
                    c->obj.anim.action = i;
                }
            }
        }
        c->obj.anim.oldaction = c->obj.anim.action;
        c->obj.anim.newaction = c->obj.anim.action;
        ResetLights(&c->lights);
        return 1;
    }
    return 0;
}


void ProcessCreatures(void)
{
    struct creature_s *c;
    s32 i;
    struct nuvec_s pos;

    if (FRAME == 0) {
        tbslotBeginFn(app_tbset, 5);
    }
    c = &Character[0];
    GetTopBot(c);
    NewTopBot(&c->obj);
    c++;
    for (i = 0; i < 8; i++, c++) {
        if (c->on != 0) {
            c->obj.pos_adjusted = 0;
            c->obj.got_shadow = 0;
            c->obj.old_SCALE = c->obj.SCALE;
            MoveCreature(c);
            if ((USELIGHTS != 0) && (LIGHTCREATURES != 0)) {
                pos.x = c->obj.pos.x;
                pos.y = (c->obj.bot + c->obj.top) * c->obj.SCALE * 0.5f
                        + c->obj.pos.y;
                pos.z = c->obj.pos.z;
                GetLights(&pos, &c->lights, 1);
            }
            if (c->obj.dead == 1) {
                c->obj.scale =
                    (1.0f - c->obj.die_time / c->obj.die_duration) * c->ai.scale;
            } else {
                c->obj.scale = c->ai.scale;
            }
            c->hit_type = 0;
            c->obj.SCALE = c->obj.scale * CData[c->obj.character].scale;
            c->obj.RADIUS = c->obj.radius * c->obj.SCALE;
        }
    }
    if (FRAME == 0) {
        tbslotEndFn(app_tbset, 5);
        if (FRAME == 0) {
            tbslotBeginFn(app_tbset, 6);
        }
    }
    c = &Character[0];
    if ((PLAYERCOUNT != 0) && (c->on != 0)) {
        c->obj.pos_adjusted = 0;
        c->obj.got_shadow = 0;
        c->obj.old_SCALE = c->obj.SCALE;
        MovePlayer(c, Pad[0]);
        if ((USELIGHTS != 0) && (LIGHTCREATURES != 0)) {
            pos.x = c->obj.pos.x;
            pos.y = (c->obj.bot + c->obj.top) * c->obj.SCALE * 0.5f
                    + c->obj.pos.y;
            pos.z = c->obj.pos.z;
            GetLights(&pos, &c->lights, 1);
        }
        if (level_part_2 == 0) {
            i = cRPosCOUNT;
            ComplexRailPosition(&c->obj.pos, c->obj.RPos.iRAIL,
                                c->obj.RPos.iALONG, &c->obj.RPos, 1);
            if ((cRPosCOUNT != i) && (GameCam.mode != -1) &&
                (c->obj.transporting == 0)) {
                BlendGameCamera(&GameCam, 0.5f);
            }
        }
        if ((c->obj.mask != 0) && (c->obj.mask->active != 0) &&
            ((LDATA->flags & 0xE00) == 0)) {
            UpdateMask(c->obj.mask, &c->obj);
        }
        if (Level == 0x25) {
            HubSelect(c);
            HubLevelSelect(&c->obj, Hub);
            HubMoveVR();
        } else if (Level < 0x23) {
            CheckPlayerEvents(&c->obj);
            CheckFinish(&c->obj);
            CheckGates(&c->obj);
            CheckRings(&c->obj, &i_ring);
        }
        if (c->obj.dead == 1) {
            c->obj.scale = 1.0f - c->obj.die_time / c->obj.die_duration;
        } else {
            c->obj.scale = 1.0f;
        }
        c->obj.SCALE = c->obj.scale * CData[c->obj.character].scale;
        if (VEHICLECONTROL == 2) {
            c->obj.RADIUS = CData[115].radius;
        } else if ((VEHICLECONTROL == 1) && (c->obj.vehicle != -1)) {
            c->obj.RADIUS = CData[c->obj.vehicle].radius;
        } else {
            c->obj.RADIUS = c->obj.radius;
        }
        c->obj.RADIUS *= c->obj.SCALE;
        /* PS2-only: on level 0x11 a time trial can be restarted with the
         * R1-family button (Pad[0]->buttons & 0x100). Not present on GC. */
        if (Level == 0x11) {
            if ((TimeTrial != 0) && (c->obj.dead == 0) && (Pad[0] != 0) &&
                ((Pad[0]->buttons & 0x100) != 0)) {
                c->obj.pos = D_0061D318;
                c->obj.mom = v000;
                GameCam.mode = -1;
            }
        }
    }
    if (FRAME == 0) {
        tbslotEndFn(app_tbset, 6);
    }
}


void UpdateAnimPacket(struct CharacterModel *mod, struct anim_s *anim,
                      float dt, float xz_distance)
{
    float t;

    if ((mod == 0) || (anim == 0)) {
        return;
    }
    if (anim->blend != 0) {
        anim->blend_frame++;
        if (anim->blend_frame == anim->blend_frames) {
            anim->blend = 0;
            anim->action = anim->blend_dst_action;
            anim->anim_time = anim->blend_dst_time;
        }
    } else if (anim->newaction != anim->oldaction) {
        if ((anim->oldaction != -1) && (anim->newaction != -1) &&
            (mod->anmdata[anim->oldaction] != 0) &&
            (mod->anmdata[anim->newaction] != 0) &&
            (mod->animlist[anim->oldaction]->blend_out_frames > 1) &&
            (mod->animlist[anim->newaction]->blend_in_frames > 1)) {
            anim->blend = 1;
            anim->blend_src_action = anim->oldaction;
            anim->blend_dst_action = anim->newaction;
            anim->blend_src_time = anim->anim_time;
            if (((mod->animlist[anim->oldaction]->flags & 1) != 0) &&
                ((mod->animlist[anim->newaction]->flags & 1) != 0) &&
                (mod->animlist[anim->oldaction]->speed ==
                 mod->animlist[anim->newaction]->speed) &&
                (mod->anmdata[anim->oldaction]->time ==
                 mod->anmdata[anim->newaction]->time)) {
                anim->blend_dst_time = anim->anim_time;
            } else {
                anim->blend_dst_time = 1.0f;
            }
            if ((mod->character == 0) && (PLAYERCOUNT != 0) &&
                (player->used != 0)) {
                if (anim->blend_dst_action == 3) {
                    anim->blend_dst_time = ((float)player->crouch_pos *
                        (mod->anmdata[anim->blend_dst_action]->time - 1.0f)) /
                        (float)player->OnFootMoveInfo->CROUCHINGFRAMES;
                } else if (anim->blend_dst_action == 5) {
                    anim->blend_dst_time =
                        ((float)(player->OnFootMoveInfo->CROUCHINGFRAMES -
                                 player->crouch_pos) *
                         (mod->anmdata[anim->blend_dst_action]->time - 1.0f)) /
                        (float)player->OnFootMoveInfo->CROUCHINGFRAMES;
                }
            }
            anim->blend_frame = 0;
            anim->blend_frames = mod->animlist[anim->newaction]->blend_in_frames;
            if (mod->animlist[anim->oldaction]->blend_out_frames <
                anim->blend_frames) {
                anim->blend_frames =
                    mod->animlist[anim->oldaction]->blend_out_frames;
            }
        } else {
            anim->action = anim->newaction;
            anim->anim_time = 1.0f;
            anim->blend = 0;
        }
    } else {
        anim->action = anim->newaction;
        anim->blend = 0;
    }
    anim->flags = 0;
    if (anim->blend != 0) {
        if ((mod->anmdata[anim->blend_src_action] == 0) ||
            (mod->anmdata[anim->blend_dst_action] == 0)) {
            return;
        }
        t = dt * mod->animlist[anim->blend_src_action]->speed;
        if ((mod->animlist[anim->blend_src_action]->flags & 0x10) != 0) {
            t *= xz_distance * 10.0f;
        }
        anim->blend_src_time += t;
        t = mod->anmdata[anim->blend_src_action]->time;
        if (anim->blend_src_time > t) {
            if ((mod->animlist[anim->blend_src_action]->flags & 1) != 0) {
                anim->blend_src_time -= t - 1.0f;
            } else {
                anim->blend_src_time = t;
            }
        }
        t = dt * mod->animlist[anim->blend_dst_action]->speed;
        if ((mod->animlist[anim->blend_dst_action]->flags & 0x10) != 0) {
            t *= xz_distance * 10.0f;
        }
        anim->blend_dst_time += t;
        t = mod->anmdata[anim->blend_dst_action]->time;
        if (anim->blend_dst_time > t) {
            if ((mod->animlist[anim->blend_dst_action]->flags & 1) != 0) {
                anim->flags = anim->flags | 2;
                anim->blend_dst_time -= t - 1.0f;
                return;
            }
            anim->blend_dst_time = t;
            anim->flags = anim->flags | 1;
        }
    } else {
        if (mod->anmdata[anim->action] == 0) {
            return;
        }
        t = dt * mod->animlist[anim->action]->speed;
        if ((mod->animlist[anim->action]->flags & 0x10) != 0) {
            t *= xz_distance * 10.0f;
        }
        anim->anim_time += t;
        t = mod->anmdata[anim->action]->time;
        if (anim->anim_time > t) {
            if ((mod->animlist[anim->action]->flags & 1) != 0) {
                anim->flags = anim->flags | 2;
                anim->anim_time -= t - 1.0f;
                return;
            }
            anim->anim_time = t;
            anim->flags = anim->flags | 1;
        }
    }
}


void StoreLocatorMatrices(struct CharacterModel *model, struct numtx_s *mC,
                          struct numtx_s *tmtx, struct numtx_s *mtx,
                          struct nuvec_s *mom)
{
    struct nuvec_s oldpos;
    s32 i;
    struct numtx_s m;

    if (mtx != 0) {
        for (i = 0; i < 0x10; i++) {
            if (model->pLOCATOR[i] != 0) {
                oldpos.x = mtx[i]._30;
                oldpos.y = mtx[i]._31;
                oldpos.z = mtx[i]._32;
                NuHGobjPOIMtx(model->hobj, i, mC, tmtx, &m);
                mtx[i] = m;
                if (mom != 0) {
                    mom[i].x = mtx[i]._30 - oldpos.x;
                    mom[i].y = mtx[i]._31 - oldpos.y;
                    mom[i].z = mtx[i]._32 - oldpos.z;
                }
            }
        }
    }
}


void ResetPlayerMoves(struct creature_s *c)
{
    c->jump = 0;
    c->slam = 0;
    c->spin = 0;
    c->crawl = 0;
    c->tiptoe = 0;
    c->sprint = 0;
    c->somersault = 0;
    c->land = 0;
    c->idle_mode = 0;
    c->idle_sigh = 0;
    c->crawl_lock = 1;
    c->crouch_pos = 0;
    c->slam_wait = 0;
    c->spin_wait = 0;
    c->slide = 0;
    c->idle_action = 0x22;
    c->idle_wait = IDLEWAIT * 30.0f;
    c->obj.idle_gametime = 0.0f;
    c->idle_time = 0.0f;
    c->target = 0;
    c->fire = 0;
    c->tap = 0;
    c->freeze = 0;
    c->obj.transporting = 0;

    ResetAnimPacket(&c->obj.anim, 0x22);

    c->obj.frame = 0;
    c->obj.attack = 0;
    c->obj.dyrot = 0;
    c->obj.boing = 0;
    c->obj.dangle = 0;
    c->obj.old_ground = 3;
    c->obj.submerged = 0;
    c->obj.SCALE = 1.0f;
    c->obj.scale = 1.0f;
    c->obj.RADIUS = c->obj.radius;
    c->obj.ground = 3;
}


void RemoveCreature(struct creature_s *c)
{
    RemoveGameObject(&c->obj);
    c->used = 0;
}


void CloseCreatures(void)
{
    s32 i;

    for (i = 0; i < 9; i++) {
        if (Character[i].used != 0) {
            RemoveGameObject(&Character[i].obj);
            Character[i].used = 0;
        }
    }
    for (i = 0; i < 48; i++) {
        if (CModel[i].hobj != 0) {
            NuHGobjDestroy(CModel[i].hobj);
        }
    }
}


void ResetAnimPacket(struct anim_s *anim, s32 action)
{
    if (anim == 0) {
        return;
    }
    anim->newaction = action;
    anim->oldaction = action;
    anim->action = action;
    anim->anim_time = 1.0f;
    anim->blend = 0;
    anim->flags = 0;
}


void UpdateRumble(struct rumble_s *rumble)
{
    if (rumble->buzz != 0) {
        rumble->buzz--;
    }
    if (rumble->frame != 0) {
        rumble->frame--;
    }
}


void NewRumble(struct rumble_s *rumble, s32 power)
{
    if ((rumble->frame != 0) &&
        (power <= (rumble->power * rumble->frame) / rumble->frames)) {
        return;
    }
    rumble->power = power;
    rumble->frames = (power * 0x32) >> 8;
    rumble->frame = (power * 0x32) >> 8;
}


void NewBuzz(struct rumble_s *rumble, s32 frames)
{
    if (frames > rumble->buzz) {
        rumble->buzz = frames;
    }
}
