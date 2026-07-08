/* .\creature.c -- creatures, the player, character models (unit 91).
 */

#include "creature.h"

/* --- externals ---------------------------------------------------- */

/* AI table entry (AITab): PS2 stride 0x80; only ai_type (+0x0) and the
 * start positions (+0x14) are used here -- the rest is opaque. */
struct aitab_s {
    u8 ai_type;              /* 0x00 */
    s8 status;               /* 0x01 */
    u8 unk_0x02[0x0A];       /* 0x02 (unverified) */
    f32 time;                /* 0x0C */
    f32 delay;               /* 0x10 */
    struct nuvec_s pos[8];   /* 0x14 */
    struct nuvec_s origin;   /* 0x74 (== pos[8]) */
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

/* Per-level record inside Game (stride 0x1C, flags at +0x0). */
struct gamelevel_s {
    u16 flags;               /* 0x00 */
    u8 unk_0x02[0x1A];       /* 0x02 */
}; /* 0x1C */

/* Only the fields creature.c touches are typed; sizes unverified. */
struct game_s {
    u8 unk_0x000[0x28];      /* 0x000 (opaque) */
    struct gamelevel_s level[35]; /* 0x028 */
    u8 lives;                /* 0x3FC */
    u8 wumpas;               /* 0x3FD */
    u8 unk_0x3FE[8];         /* 0x3FE */
    u8 powerbits;            /* 0x406 */
};

struct ldata_s {
    u8 unk_0x00[0x04];       /* 0x00 (opaque) */
    u8 *clist;               /* 0x04 (character-id load list) */
    u8 unk_0x08[0x1C];       /* 0x08 (opaque) */
    unsigned short flags;    /* 0x24 */
    u8 unk_0x26[2];          /* 0x26 */
    unsigned short vehicle;  /* 0x28 */
    unsigned short farclip;  /* 0x2A */
    u8 unk_0x2C[0xC];        /* 0x2C */
    struct nuvec_s vBONUS;   /* 0x38 (bonus-round restart position) */
};

struct leveldata_s {
    u8 unk_0x00[0x23];       /* 0x00 (opaque) */
    signed char hub;         /* 0x23 */
    u8 unk_0x24[0x30];       /* 0x24 (opaque) */
}; /* 0x54 */

struct gamecam_s {
    u8 unk_0x000[0x10C];     /* 0x000 (opaque) */
    unsigned short hdg_to_player; /* 0x10C */
    u8 unk_0x10E[6];         /* 0x10E */
    signed char mode;        /* 0x114 */
};

struct pad_s {
    u8 unk_0x000[0x564];     /* 0x000 (opaque) */
    unsigned int buttons;    /* 0x564 */
};

extern float IDLEWAIT;
extern float SAFEY;
extern u64 LBIT;
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
extern struct panelcount_s plr_bonus_wumpas;
extern void *app_tbset;

/* Globals ManageCreatures reads/writes (PS2 v1.03 addresses in symbol_addrs). */
extern f32 AIVISRANGE;
extern s32 Demo;
extern s32 Bonus;
extern s32 GemPath;
extern s32 Death;
extern s32 new_mode;
extern s32 new_level;
extern s32 bonus_restart;
extern s32 bonus_lives;
extern s32 bonus_finish_frame;
extern s32 save_bonus_crates_destroyed;
extern s32 LivesLost;
extern s32 plr_died;
extern s32 clock_ok;
extern s32 c_slot;
extern s32 LEVELAICOUNT;
extern s32 bonusgem_ok;
extern s32 boss_dead;
extern u16 plr_items;
extern u8 bonus_wumpa_delay;
extern u8 bonus_life_delay;
extern f32 bonus_wumpa_wait;
extern f32 bonus_crates_wait;
extern f32 bonus_lives_wait;

s32 NuSoundStopStream(s32 channel);
f32 NuVecDistSqr(struct nuvec_s *a, struct nuvec_s *b, struct nuvec_s *d);
s32 qrand(void);
s32 RotDiff(u16 a, u16 b);
void GameSfx(s32 sfx, struct nuvec_s *pos);
s32 abs(s32 x);

/* NuDebugMsgProlog(file, line) returns the printf-style logger it prologues. */
typedef void (*debugmsg_fn)(char *fmt, ...);
debugmsg_fn NuDebugMsgProlog(char *file, s32 line);

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

/* One joint-animation override entry passed to the NuHGobj eval routines.
 * Only the fields DrawCharacterModel writes are typed (stride 0x34). */
struct NUJOINTANIM_s {
    float rx, ry, rz;        /* 0x00 */
    float tx, ty, tz;        /* 0x0C */
    float sx, sy, sz;        /* 0x18 */
    u8 unk_0x24[0xC];        /* 0x24 */
    u8 joint_id;             /* 0x30 */
    u8 flags;                /* 0x31 */
    u8 pad[2];               /* 0x32 */
}; /* 0x34 */

/* Global frame counter (size 0x18, GameTimer .. PauseTimer); only
 * .frame (0x0, u32) is touched here. Sized >0x8 so it is not gp-relative. */
struct gametimer_s {
    u32 frame;               /* 0x00 */
    u8 unk_0x04[0x14];       /* 0x04 */
}; /* 0x18 */

extern s32 jeep_draw;
extern s32 glass_draw;
extern s32 plr_render;
extern s32 Paused;
extern s32 ChrisJointOveride;
extern s32 ChrisNumJoints;
extern struct NUJOINTANIM_s *ChrisJointList;
extern struct gametimer_s GameTimer;

float **NuHGobjEvalDwaBlend(s32 nlayers, short *layer,
                            struct nuanimdata_s *src, float src_time,
                            struct nuanimdata_s *dst, float dst_time,
                            float blend);
float **NuHGobjEvalDwa(s32 nlayers, short *layer, struct nuanimdata_s *data,
                       float time);
void NuHGobjEvalAnimBlend(struct NUHGOBJ_s *hobj, struct nuanimdata_s *src,
                          float src_time, struct nuanimdata_s *dst,
                          float dst_time, float blend, s32 nJ,
                          struct NUJOINTANIM_s *pJ, struct numtx_s *tmtx);
void NuHGobjEvalAnim(struct NUHGOBJ_s *hobj, struct nuanimdata_s *data,
                     float time, s32 nJ, struct NUJOINTANIM_s *pJ,
                     struct numtx_s *tmtx);
void NuHGobjEval(struct NUHGOBJ_s *hobj, s32 nJ, struct NUJOINTANIM_s *pJ,
                 struct numtx_s *tmtx);
s32 NuHGobjRndrMtxDwa(struct NUHGOBJ_s *hobj, struct numtx_s *mC, s32 nlayers,
                      short *layer, struct numtx_s *tmtx, float **dwa);
void AddAnimDebris(struct CharacterModel *model, struct numtx_s *mtx,
                   s32 action, float time, struct obj_s *obj);
void DrawProbeFX(struct obj_s *obj);
void ShadRndr(struct numtx_s *mS, void *data, float time, float scale);


inline float ModelAnimDuration(u32 character, u32 action, float start, float end)
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


/* Checkpoint / restart globals (PS2 v1.03 addresses in symbol_addrs). */
extern s32 cp_iALONG;
extern s32 cp_iRAIL;
extern s32 cp_goto;
extern struct nuvec_s cpGOTO;
extern struct nuvec_s cpPOS;
extern struct RPos_s *best_cRPos;
extern s32 plr_rebound;
extern f32 ATLASCAMHEIGHT;
extern struct panelcount_s plr_crates;
extern f32 D_0062D070;      /* "no shadow" sentinel 2000000.0f (.sdata) */
extern s32 force_panel_items_update;
extern s32 force_panel_wumpa_update;
extern s32 force_panel_crate_update;
extern s32 force_panel_lives_update;
extern f32 point_time;
extern f32 point_duration;
extern f32 check_time;
extern f32 check_duration;

void edobjResetAnimsToZero(void);
void OldTopBot(struct obj_s *obj);
f32 NewShadowPlat(struct nuvec_s *pos, f32 y);
void GetSurfaceInfo(struct obj_s *obj, s32 mode, f32 y);
void ResetAtlas(struct creature_s *c);
void SetWeatherStartPos(struct creature_s *c);
void ResetJeep(struct creature_s *c);
void NewMask(struct mask_s *mask, struct nuvec_s *pos);


void ResetPlayer(s32 set)
{
    struct creature_s *c;
    s32 start;
    s32 water;
    f32 y;

    if (PLAYERCOUNT != 0) {
        if (set != 0) {
            if ((Level != 0x11) || (cp_iALONG < 0x6B)) {
                edobjResetAnimsToZero();
            }
            c = player;
            PlayerStartPos(player, &player->obj.startpos);
            start = 0;
            if ((bonus_restart != 0) &&
                ((LDATA->vBONUS.x != 0.0f) || (LDATA->vBONUS.y != 0.0f) ||
                 (LDATA->vBONUS.z != 0.0f))) {
                c->obj.pos = LDATA->vBONUS;
            } else {
                if (cp_goto != -1) {
                    c->obj.pos = cpGOTO;
                } else if (cp_iRAIL != -1) {
                    c->obj.pos = cpPOS;
                } else {
                    start = 1;
                    c->obj.pos = c->obj.startpos;
                }
            }
            if (bonus_restart != 0) {
                bonus_restart = 0;
            }
            water = 0;
            if ((LBIT & 0x0000000400000040ULL) ||
                ((Level == 2) && start)) {
                water = 1;
            }

            /* ResetPlayerMoves(c)'s body, inlined by hand in the retail
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

            GetTopBot(c);
            NewTopBot(&c->obj);
            OldTopBot(&c->obj);
            y = NewShadowPlat(&c->obj.pos, 0.0f);
            GetSurfaceInfo(&c->obj, 1, y);
            if (cp_goto != -1) {
                cp_goto = -1;
            } else if (water != 0) {
                if (cp_iRAIL != -1) {
                    c->obj.pos.y -= c->obj.bot * c->obj.SCALE;
                }
            } else if ((Level != 0x1D) && (y != D_0062D070)) {
                c->obj.pos.y = y - c->obj.bot * c->obj.SCALE;
            }
            c->obj.oldpos = c->obj.pos;
            c->obj.mom = v000;
            ComplexRailPosition(&c->obj.pos, c->obj.RPos.iRAIL,
                                c->obj.RPos.iALONG, &c->obj.RPos, 1);
            if ((VEHICLECONTROL == 2) ||
                ((VEHICLECONTROL == 1) && (c->obj.vehicle == 0x20))) {
                c->obj.thdg = c->obj.hdg = 0;
            } else if (Level == 0x19) {
                c->obj.hdg = 0x4000;
            } else if (best_cRPos != 0) {
                c->obj.thdg = c->obj.hdg = best_cRPos->angle;
            }
            switch (c->obj.vehicle) {
            case 0x53:
                ResetAtlas(c);
                break;
            case 0x36:
                if (Level == 0x18) {
                    SetWeatherStartPos(c);
                }
                break;
            case 0x63:
                ResetJeep(c);
                break;
            }
            plr_rebound = 0;
            ATLASCAMHEIGHT = 2.5f;
            ResetLights(&c->lights);
            if ((c->obj.mask != 0) && (c->obj.mask->active != 0)) {
                if (c->obj.mask->active > 2) {
                    c->obj.mask->active = 2;
                }
                ResetLights(&c->obj.mask->lights);
            }
        }
        plr_crates.count = plr_crates.draw = 0;
        plr_crates.frame = 0;
        player->obj.scale = 1.0f;
        player->obj.dead = 0;
        player->obj.finished = 0;
        if ((player->obj.mask != 0) && (player->obj.mask->active == 0) &&
            (LivesLost > 4)) {
            NewMask(player->obj.mask, &player->obj.pos);
        }
    }
    if ((Demo == 0) && (GameMode != 1)) {
        force_panel_items_update = 0x32;
        force_panel_wumpa_update = 0x32;
        force_panel_crate_update = 0x32;
        force_panel_lives_update = 0x32;
    }
    boss_dead = 0;
    point_time = 0.0f;
    check_duration = 0.0f;
    check_time = 0.0f;
    point_duration = 0.0f;
}


void ManageCreatures(void)
{
    struct nuvec_s *p0;
    f32 d;
    f32 dist;
    f32 range2;
    s32 i;
    s32 j;
    s32 free_slot;
    s32 old_index;
    s32 index;
    struct aitab_s *pAI;

    if ((Level == 0x25) || ((LDATA->flags & 0x202) != 0)) {
        d = (f32)LDATA->farclip;
    } else {
        d = AIVISRANGE;
    }
    range2 = d;
    if ((f32)LDATA->farclip < range2) {
        range2 = (f32)LDATA->farclip;
    }
    range2 = range2 * range2;

    if ((player != 0) && (player->used != 0) && (player->obj.dead != 0)) {
        player->obj.die_time += 0.02f;
        if (player->obj.die_time >= player->obj.die_duration) {
            player->obj.die_time = player->obj.die_duration;
            if ((new_mode == -1) && (new_level == -1)) {
                if (Demo != 0) {
                    new_level = 0x23;
                } else if (Bonus == 2) {
                    if ((plr_bonus_wumpas.count == 0) &&
                        (bonus_wumpa_delay == 0) &&
                        !(bonus_wumpa_wait > 0.0f) &&
                        (bonus_finish_frame >=
                         save_bonus_crates_destroyed * 5 + 5) &&
                        !(bonus_crates_wait > 0.0f) && (bonus_lives == 0) &&
                        (bonus_life_delay == 0) && !(bonus_lives_wait > 0.0f)) {
                        NuSoundStopStream(0);
                        NuSoundStopStream(1);
                        bonus_restart = 1;
                        new_mode = GameMode;
                    }
                } else if (TimeTrial != 0) {
                    new_mode = GameMode;
                } else {
                    if (plr_lives.count != 0) {
                        plr_lives.count = plr_lives.count - 1;
                        if (Adventure != 0) {
                            LivesLost = LivesLost + 1;
                            Game.lives = (u8)plr_lives.count;
                        }
                        new_mode = GameMode;
                    } else {
                        new_level = 0x26;
                    }
                    plr_died = 1;
                }
            }
        }
    }

    free_slot = -1;
    for (i = 1; i < 9; i++) {
        if (Character[i].used != 0) {
            if (Character[i].on == 0) {
                if (Character[i].off_wait != 0) {
                    Character[i].off_wait = Character[i].off_wait - 1;
                    if (Character[i].off_wait == 0) {
                        RemoveGameObject(&Character[i].obj);
                        Character[i].used = 0;
                    }
                }
            } else if (Character[i].obj.dead != 0) {
                Character[i].obj.die_time += 0.02f;
                if (Character[i].obj.die_time >=
                    Character[i].obj.die_duration) {
                    Character[i].obj.die_time = Character[i].obj.die_duration;
                    Character[i].on = 0;
                    Character[i].off_wait = 2;
                    if ((Character[i].i_aitab != -1) &&
                        (AITab[Character[i].i_aitab].delay <= 0.0f)) {
                        AITab[Character[i].i_aitab].status = 0;
                    }
                }
            } else {
                if ((NuVecDistSqr(&AITab[Character[i].i_aitab].origin,
                                  &player->obj.pos, 0) > range2) ||
                    ((level_part_2 != 0) &&
                     (AITab[Character[i].i_aitab].ai_type != 0x4F))) {
                    Character[i].on = 0;
                    Character[i].off_wait = 2;
                    if (Character[i].obj.character == 0x76) {
                        clock_ok = 0;
                    }
                }
            }
        } else if (free_slot == -1) {
            free_slot = i;
        }
    }

    if (free_slot == -1) {
        c_slot = c_slot + 1;
        if (c_slot == 9) {
            c_slot = 1;
        }
    } else {
        c_slot = free_slot;
    }

    p0 = &player->obj.pos;
    index = -1;
    if (Character[c_slot].used != 0) {
        index = Character[c_slot].i_aitab;
        if (index != -1) {
            dist = NuVecDistSqr(p0, AITab[Character[c_slot].i_aitab].pos, 0);
        }
    }

    old_index = index;
    pAI = AITab;
    for (i = 0; i < LEVELAICOUNT; i++, pAI++) {
        if ((pAI->delay > 0.0f) && (pAI->time > 0.0f)) {
            pAI->time -= 0.02f;
            if (pAI->time < 0.0f) {
                pAI->time = 0.0f;
            }
        }
        for (j = 1; j < 9; j++) {
            if ((Character[j].used != 0) && (Character[j].i_aitab == i) &&
                (!(pAI->delay > 0.0f) || (pAI->time != 0.0f))) {
                goto next;
            }
        }
        if (((level_part_2 == 0) || (pAI->ai_type == 0x4F)) &&
            (pAI->status != 0) &&
            (CRemap[AIType[pAI->ai_type].character] != -1)) {
            switch (pAI->ai_type) {
            case 0x4C:
            case 0x4D:
                if (((Game.level[Level].flags & 8) == 0) &&
                    ((plr_items & 1) == 0) &&
                    ((LDATA->flags & 0x200) == 0)) {
                    goto add;
                }
                break;
            case 0x4E:
            case 0x4F:
                if ((Demo == 0) &&
                    ((Hub == 5) || ((Game.level[Level].flags & 8) != 0)) &&
                    (TimeTrial == 0) && (clock_ok != 0)) {
                    goto add;
                }
                break;
            case 0x50:
            case 0x51:
                if (((Game.level[Level].flags & 0x10) == 0) &&
                    ((plr_items & 2) == 0) && (TimeTrial == 0) &&
                    ((LDATA->flags & 0x200) == 0)) {
                    goto add;
                }
                break;
            case 0x52:
            case 0x53:
                if (((Game.level[Level].flags & 0x20) == 0) &&
                    ((plr_items & 4) == 0) && (TimeTrial == 0) &&
                    ((LDATA->flags & 0x200) == 0) && (bonusgem_ok != 0)) {
                    goto add;
                }
                break;
            case 0x54:
                if (((Game.level[Level].flags & 0x40) == 0) &&
                    ((plr_items & 8) == 0) && (TimeTrial == 0) &&
                    ((LDATA->flags & 0x200) == 0)) {
                    goto add;
                }
                break;
            case 0x55:
                if (((Game.level[Level].flags & 0x80) == 0) &&
                    ((plr_items & 0x20) == 0) && (TimeTrial == 0) &&
                    ((LDATA->flags & 0x200) == 0)) {
                    goto add;
                }
                break;
            case 0x56:
                if (((Game.level[Level].flags & 0x100) == 0) &&
                    ((plr_items & 0x10) == 0) && (TimeTrial == 0) &&
                    ((LDATA->flags & 0x200) == 0)) {
                    goto add;
                }
                break;
            case 0x57:
                if (((Game.level[Level].flags & 0x200) == 0) &&
                    ((plr_items & 0x40) == 0) && (TimeTrial == 0) &&
                    ((LDATA->flags & 0x200) == 0)) {
                    goto add;
                }
                break;
            case 0x58:
                if (((Game.level[Level].flags & 0x400) == 0) &&
                    ((plr_items & 0x80) == 0) && (TimeTrial == 0) &&
                    ((LDATA->flags & 0x200) == 0)) {
                    goto add;
                }
                break;
            case 0x5A:
                if ((Game.powerbits & 0x20) == 0) {
                    goto add;
                }
                break;
            case 0x59:
            case 0x5B:
            case 0x5C:
            case 0x5D:
            case 0x5E:
                if ((boss_dead == 1) && ((LBIT & 0x3E00000) != 0)) {
                    goto add;
                }
                break;
            default:
            add:
                d = NuVecDistSqr(p0, pAI->pos, 0);
                if ((index == -1) || (d < dist)) {
                    index = i;
                    dist = d;
                }
                break;
            }
        }
    next:;
    }

    if ((index != -1) && (index != old_index)) {
        if (!(AITab[index].delay > 0.0f) || !(AITab[index].time > 0.0f)) {
            if (Character[c_slot].used != 0) {
                Character[c_slot].on = 0;
                Character[c_slot].off_wait = 2;
                if (Character[c_slot].obj.character == 0x76) {
                    clock_ok = 0;
                }
            } else if (NuVecDistSqr(&AITab[index].origin,
                                    &player->obj.pos, 0) < range2) {
                i = AIType[AITab[index].ai_type].character;
                NuDebugMsgProlog(".\\creature.c", 0x135C)(
                    "ManageCreatures - new character = %s", CData[i].name);
                AddCreature(i, c_slot, index);
                if (AITab[index].delay > 0.0f) {
                    AITab[index].time = AITab[index].delay;
                }
            }
        }
    }
}


/* One SpaceGameCut load entry (Level 0x28 only, stride 0x80): the character
 * id at +0x0 and an inline animation-descriptor table at +0x8. */
struct space_s {
    s32 character;            /* 0x00 */
    s32 unk_0x04;             /* 0x04 */
    struct animlist anim[5];  /* 0x08 */
}; /* 0x80 */

/* 3D-font glyph table entry (stride 0xC); only the id and flag byte are used. */
struct font3dobj_s {
    short id;                 /* 0x00 */
    u8 flags;                 /* 0x02 */
    u8 unk_0x03[9];           /* 0x03 */
}; /* 0x0C */

extern s8 CLetter[191];
extern s32 gamecut;
extern struct space_s *SpaceGameCutTab[][2];
extern struct CharacterModel *bgload_model[2];
extern void *superbuffer_ptr;
extern char tbuf[];
extern struct font3dobj_s Font3DObjTab[];
extern struct MoveInfo MineCartMoveInfo;

/* Retail-owned filename fragments and per-variant animation tables. */
extern char D_0061D270[];   /* .dat path passed to NuDatOpen */
extern char D_0061D280[];   /* sprintf format for the load screen */
extern char D_00630980[];
extern char D_00630988[];
extern char D_00630990[];
extern char D_00630998[];
extern char D_006309A0[];
extern char D_006309A8[];
extern struct animlist D_0055C100[];
extern struct animlist D_0055C178[];
extern struct animlist D_0055C1C0[];
extern struct animlist D_0055C2C8[];
extern struct animlist D_0055C370[];
extern struct animlist D_0055C3E8[];
extern struct animlist D_0055CCA0[];

void *NuDatOpen(char *name, s32 mode, s32 arg);
void NuDatSet(void *dat);
void NuDatClose(void *dat);
void LoadScreen(char *name);
char *NuStrCpy(char *dst, char *src);
char *NuStrCat(char *dst, char *src);
struct NUHGOBJ_s *NuGHGRead(void **buf, char *name);
struct instSHADHDR_s *InstShadDataLoad(char *name);
void *ShadFindData(struct instSHADHDR_s *hdr, char *name);
struct NUPOINTOFINTEREST_s *NuHGobjGetPOI(struct NUHGOBJ_s *hobj, u8 i);
struct nuanimdata_s *InstAnimDataLoad(char *name);
int sprintf(char *buf, const char *fmt, ...);


void LoadCharacterModels(void)
{
    s32 i;
    s32 j;
    s32 character;
    s32 cmodel_index;
    struct space_s *space;
    struct CharacterModel *model;
    struct animlist *anim;
    CharacterData *cdata;
    void *dat;
    char charsdat_filename[64];

    for (i = 0; i < 0xBF; i++) {
        CRemap[i] = -1;
        CLetter[i] = 0x3F;
    }
    for (i = 0x2F; i >= 0; i--) {
        CModel[i].hobj = 0;
    }

    if (Level == 0x28) {
        space = SpaceGameCutTab[gamecut][0];
    } else {
        space = 0;
    }

    dat = NuDatOpen(D_0061D270, 0, 0);
    NuDatSet(dat);

    if (LDATA->clist != 0) {
        cmodel_index = 0;
        model = CModel;
        for (i = 0; ; i++) {
            if (space != 0) {
                character = ((s32 *)space)[i << 5];
            } else {
                character = LDATA->clist[i];
            }
            if (character == 0xFF) {
                break;
            }
            if (cmodel_index >= 0x30) {
                break;
            }

            sprintf(tbuf, D_0061D280, CData[character].name);
            LoadScreen(tbuf);
            memset(model, 0, 0x988);
            if (character < 2) {
                bgload_model[character] = model;
            }
            cdata = &CData[character];
            NuStrCpy(charsdat_filename, D_00630980);
            NuStrCat(charsdat_filename, cdata->path);
            NuStrCat(charsdat_filename, D_00630988);
            NuStrCpy(tbuf, charsdat_filename);
            NuStrCat(tbuf, cdata->file);
            NuStrCat(tbuf, D_00630990);
            model->hobj = NuGHGRead(&superbuffer_ptr, tbuf);
            if (model->hobj != 0) {
                NuStrCpy(tbuf, charsdat_filename);
                if (character == 0xB2) {
                    NuStrCat(tbuf, CData[0x44].file);
                } else {
                    NuStrCat(tbuf, cdata->file);
                }
                NuStrCat(tbuf, D_00630998);
                model->shadhdr = InstShadDataLoad(tbuf);
                if (model->shadhdr != 0) {
                    model->shaddata = ShadFindData(model->shadhdr, 0);
                }
                for (j = 0; j < 0x10; j++) {
                    model->pLOCATOR[j] = NuHGobjGetPOI(model->hobj, (u8)j);
                }
                if ((character == 0x54) || (character == 0x9F)) {
                    NuStrCpy(charsdat_filename, D_00630980);
                    NuStrCat(charsdat_filename, CData[0].path);
                    NuStrCat(charsdat_filename, D_00630988);
                }
                if (Level == 0x28) {
                    anim = (struct animlist *)((char *)space + (i << 7) + 8);
                } else {
                    anim = cdata->anim;
                    if (character == 0) {
                        if ((LBIT & 0x1001002000ULL) && (Level != 0x1E)) {
                            anim = D_0055C100;
                        } else if (LBIT & 0x40000) {
                            anim = D_0055C178;
                        } else if (LBIT & 0x100210801ULL) {
                            anim = D_0055C1C0;
                        } else if (Level == 0x1D) {
                            anim = D_0055C2C8;
                        } else if (Level == 0x1C) {
                            anim = D_0055C370;
                        } else if (Level == 0x2B) {
                            anim = D_0055C3E8;
                        }
                    } else if (character == 1) {
                        if (LBIT & 0x4000000) {
                            anim = D_0055CCA0;
                        }
                    }
                }
                while ((anim != 0) && (anim->file != 0) &&
                       (anim->action >= 0) && (anim->action < 0x76)) {
                    if (anim->levbits & LBIT) {
                        if (anim->flags & 2) {
                            if (model->anmdata[anim->action] == 0) {
                                NuStrCpy(tbuf, charsdat_filename);
                                NuStrCat(tbuf, anim->file);
                                NuStrCat(tbuf, D_006309A0);
                                model->anmdata[anim->action] =
                                    InstAnimDataLoad(tbuf);
                                if (model->anmdata[anim->action] != 0) {
                                    model->animlist[anim->action] = anim;
                                    if (model->shadhdr != 0) {
                                        model->sanmdata[anim->action] =
                                            ShadFindData(model->shadhdr,
                                                         anim->file);
                                    }
                                }
                            }
                        }
                        if (anim->flags & 4) {
                            if (model->fanmdata[anim->action] == 0) {
                                NuStrCpy(tbuf, charsdat_filename);
                                NuStrCat(tbuf, anim->file);
                                NuStrCat(tbuf, D_006309A8);
                                model->fanmdata[anim->action] =
                                    InstAnimDataLoad(tbuf);
                                if (model->fanmdata[anim->action] != 0) {
                                    model->fanimlist[anim->action] = anim;
                                }
                            }
                        }
                    }
                    anim++;
                }
                model->character = character;
                CRemap[character] = cmodel_index;
                model++;
                cmodel_index++;
            }
            for (j = 0; j < 0x1A; j++) {
                if ((Font3DObjTab[j].flags & 1) &&
                    (Font3DObjTab[j].id == character)) {
                    CLetter[character] = j + 0x61;
                    break;
                }
            }
        }
    }

    NuDatSet(0);
    if (dat != 0) {
        NuDatClose(dat);
    }

    CrashMoveInfo.JUMPLANDFRAMES   = ModelAnimDuration(0, 0x30, 0.0f, 0.0f) * 50.0f;
    CrashMoveInfo.SLAMWAITFRAMES   = ModelAnimDuration(0, 0x1F, 0.0f, 0.0f) * 50.0f;
    CrashMoveInfo.SOMERSAULTFRAMES = ModelAnimDuration(0, 0x44, 0.0f, 0.0f) * 50.0f;
    CrashMoveInfo.SLIDEFRAMES      = ModelAnimDuration(0, 0x43, 0.0f, 0.0f) * 50.0f;
    CrashMoveInfo.CROUCHINGFRAMES  = ModelAnimDuration(0, 3,    0.0f, 0.0f) * 50.0f;
    CocoMoveInfo.JUMPLANDFRAMES    = ModelAnimDuration(1, 0x30, 0.0f, 0.0f) * 50.0f;
    CocoMoveInfo.SLAMWAITFRAMES    = ModelAnimDuration(1, 0x1F, 0.0f, 0.0f) * 50.0f;
    CocoMoveInfo.SOMERSAULTFRAMES  = ModelAnimDuration(1, 0x44, 0.0f, 0.0f) * 50.0f;
    CocoMoveInfo.SPINFRAMES        = ModelAnimDuration(1, 0x46, 0.0f, 0.0f) * 50.0f;
    CocoMoveInfo.SLIDEFRAMES       = ModelAnimDuration(1, 0x43, 0.0f, 0.0f) * 50.0f;
    MineCartMoveInfo.JUMPFRAMES0   = ModelAnimDuration(0x89, 99, 0.0f, 0.0f) * 50.0f;
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


s32 NewCharacterIdle(struct creature_s *c, struct CharacterModel *model)
{
    s32 i;
    s32 ok;
    s32 count;
    s32 list[118];
    s32 sfx;

    if (GameMode != 1) {
        if ((c->idle_mode == 0) && (c->idle_sigh == 0)) {
            c->idle_sigh = 1;
            if (model->anmdata[0x3D] != 0) {
                c->idle_repeat = 1;
                c->idle_action = 0x3D;
                sfx = 0x22;
                goto New;
            }
        }

        count = 0;
        for (i = 0; i < 0x76; i++) {
            if ((model->anmdata[i] != 0) &&
                ((model->animlist[i]->flags & 8) != 0)) {
                ok = 1;
                if ((c == player) && (c->obj.character == 0)) {
                    if ((GameMode == 1) &&
                        ((i == 0x25) || (i == 0x26) || (i == 0x27))) {
                        ok = 0;
                    } else if ((i == 0x27) &&
                               (abs(RotDiff(GameCam.hdg_to_player,
                                            c->obj.hdg)) < 0x6000)) {
                        ok = 0;
                    } else if ((i == 0x29) &&
                               ((GemPath == 1) || (GemPath == 3) ||
                                (Death == 1) || (Death == 3) ||
                                (Bonus == 1) || (Bonus == 3))) {
                        ok = 0;
                    }
                }
                if (ok) {
                    list[count++] = i;
                }
            }
        }

        if (count < 1) {
            return 0;
        }

    Retry:
        sfx = -1;
        i = (count <= 1) ? 0 : qrand() / (0xFFFF / count + 1);
        c->idle_repeat = 1;
        c->idle_action = list[i];
        if (c->obj.character == 0) {
            switch (c->idle_action) {
            case 0x29:
                c->idle_repeat = qrand() / 0x4000 + 2;
                break;
            case 0x28:
                c->idle_repeat = qrand() / 0x2000 + 8;
                sfx = 0x10;
                break;
            case 0x3D:
                sfx = 0x22;
                break;
            case 0x27:
                break;
            }
        }
        if ((count >= 2) && (c->idle_action == c->old_idle_action)) {
            goto Retry;
        }

    New:
        c->idle_mode = 1;
        c->old_idle_action = c->idle_action;
        if ((1 < c->idle_repeat) &&
            ((model->animlist[c->idle_action]->flags & 1) == 0)) {
            c->idle_repeat = 1;
        }
        c->idle_time = 0.0f;
        c->idle_wait =
            (model->anmdata[c->idle_action]->time - 1.0f) * c->idle_repeat;
        i = model->animlist[c->idle_action]->blend_out_frames;
        if (i != 0) {
            c->idle_wait -= i * 0.5999999642f;
            if (c->idle_wait < 1.0f) {
                c->idle_wait = 1.0f;
            }
        }
        if (sfx != -1) {
            GameSfx(sfx, &c->obj.pos);
        }
        return 1;
    }
    return 0;
}


void UpdateCharacterIdle(struct creature_s *c, s32 character)
{
    struct CharacterModel *model;
    float t;
    s32 i;

    i = CRemap[character];
    if (i != -1) {
        model = &CModel[i];
        if ((c->obj.anim.newaction == 0x22) &&
            ((c->spin == 0) ||
             (c->spin_frame >= c->spin_frames - c->OnFootMoveInfo->SPINRESETFRAMES))) {
            c->obj.idle_gametime += 0.02f;
            /* retail .lit4 slot D_0062D0B4 = 0x3F199999 (0.6 truncated,
             * not the round-to-nearest 0x3F19999A of a plain 0.6f). */
            t = 0.5999999642f;
            if (model->anmdata[c->idle_action] != 0) {
                t *= model->animlist[c->idle_action]->speed;
            }
            c->idle_time += t;
            switch (c->idle_mode) {
            case 0:
                if (c->idle_time > c->idle_wait) {
                    i = NewCharacterIdle(c, model);
                    if (i == 0) {
                        goto StartIdle;
                    }
                }
                break;
            case 1:
                if (c->idle_time > c->idle_wait) {
                    if (((LBIT & 0x200000A1) != 0) && (c->obj.character == 0)) {
                        i = NewCharacterIdle(c, model);
                        if (i == 0) {
                            goto StartIdle;
                        }
                    } else {
                        c->idle_mode = 0;
                        c->idle_action = 0x22;
                        c->idle_time = 0.0f;
                        c->idle_wait = IDLEWAIT * 30.0f;
                        break;
                    }
                }
                break;
            }
            c->obj.anim.newaction = c->idle_action;
            return;
        }
    }
StartIdle:
    c->idle_mode = 0;
    c->idle_sigh = 0;
    c->idle_action = 0x22;
    c->old_idle_action = -1;
    c->obj.idle_gametime = 0.0f;
    c->idle_time = 0.0f;
    c->idle_wait = IDLEWAIT * 30.0f;
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



extern s32 temp_action;
extern f32 temp_time;


void EvalModelAnim(struct CharacterModel *model, struct anim_s *anim,
                   struct numtx_s *m, struct numtx_s *tmtx, float ***dwa,
                   struct numtx_s *mLOCATOR)
{
    short layertab[2] = { 0, 1 };
    short *layer = layertab;
    s32 nlayers;
    s32 i;

    nlayers = 1;
    if (model->character == 0) {
        nlayers = 2;
    }

    if ((anim->blend != 0)
        && (((u16)anim->blend_src_action <= 0x75)
            && (model->fanmdata[anim->blend_src_action] != 0))
        && (((u16)anim->blend_dst_action <= 0x75)
            && (model->fanmdata[anim->blend_dst_action] != 0))) {
        *dwa = NuHGobjEvalDwaBlend(
            nlayers, layer,
            model->fanmdata[anim->blend_src_action], anim->blend_src_time,
            model->fanmdata[anim->blend_dst_action], anim->blend_dst_time,
            (float)anim->blend_frame / (float)anim->blend_frames);
    } else if ((anim->blend == 0)
               && ((u16)anim->action <= 0x75)
               && (model->fanmdata[anim->action] != 0)) {
        *dwa = NuHGobjEvalDwa(nlayers, layer,
                              model->fanmdata[anim->action],
                              anim->anim_time);
    } else {
        *dwa = 0;
    }

    if ((anim->blend != 0)
        && (((u16)anim->blend_src_action < 0x76)
            && (model->anmdata[anim->blend_src_action] != 0))
        && (((u16)anim->blend_dst_action < 0x76)
            && (model->anmdata[anim->blend_dst_action] != 0))) {
        NuHGobjEvalAnimBlend(
            model->hobj,
            model->anmdata[anim->blend_src_action], anim->blend_src_time,
            model->anmdata[anim->blend_dst_action], anim->blend_dst_time,
            (float)anim->blend_frame / (float)anim->blend_frames,
            0, 0, tmtx);
        temp_action = anim->blend_dst_action;
        temp_time = anim->blend_dst_time;
    } else if ((anim->blend == 0)
               && ((u16)anim->action < 0x76)
               && (model->anmdata[anim->action] != 0)) {
        NuHGobjEvalAnim(model->hobj, model->anmdata[anim->action],
                        anim->anim_time, 0, 0, tmtx);
        temp_action = anim->action;
        temp_time = anim->anim_time;
    } else {
        NuHGobjEval(model->hobj, 0, 0, tmtx);
        temp_action = -1;
    }

    if (mLOCATOR != 0) {
        for (i = 0; i < 0x10; i++) {
            if (model->pLOCATOR[i] != 0) {
                NuHGobjPOIMtx(model->hobj, (u8)i, m, tmtx, &mLOCATOR[i]);
            }
        }
    }
}


s32 DrawCharacterModel(struct CharacterModel *model, struct anim_s *anim,
                       struct numtx_s *mC, struct numtx_s *mS, s32 render,
                       struct numtx_s *mR, struct numtx_s *loc_mtx,
                       struct nuvec_s *loc_mom, struct obj_s *obj)
{
    struct numtx_s tmtx[256];
    struct NUJOINTANIM_s joint[4];
    struct NUJOINTANIM_s *pJ;
    float **dwa;
    struct CharacterModel *model2;
    short layertab[2] = { 0, 1 };
    short *layer = layertab;
    struct nuvec_s oldpos;
    struct numtx_s m;
    float time;
    float scale;
    s32 action;
    s32 nJ;
    s32 nlayers;
    s32 Drawn;
    s32 i;

    if (jeep_draw != 0) {
        for (i = 0; i < 4; i++) {
            joint[i].rx = ((GameTimer.frame % 0x32) * 0x10000) / 0x32
                          * 9.58738e-05f;
            joint[i].ry = 0;
            joint[i].rz = 0;
            joint[i].tx = joint[i].ty = joint[i].tz = 0;
            joint[i].sx = joint[i].sy = joint[i].sz = 1;
            joint[i].joint_id = i;
        }
    } else {
        joint->rx = (f32)(u16)-(u16)player->obj.target_xrot * 9.58738e-05f;
        joint->ry = (f32)(u16)-(u16)player->obj.target_yrot * 9.58738e-05f;
        joint->rz = 0.0f;
        joint->tx = joint->ty = joint->tz = 0.0f;
        joint->sx = joint->sy = joint->sz = 1.0f;
        joint->joint_id = 0;
        joint->flags = 1;
    }

    if (mC == 0) {
        Drawn = 0;
        goto Exit;
    }

    nlayers = 1;
    if (model->character == 0) {
        nlayers = 2;
    }

    if (anim != 0) {
        if ((anim->blend != 0)
            && (((u16)anim->blend_src_action <= 0x75)
                && (model->fanmdata[anim->blend_src_action] != 0))
            && (((u16)anim->blend_dst_action <= 0x75)
                && (model->fanmdata[anim->blend_dst_action] != 0))) {
            dwa = NuHGobjEvalDwaBlend(
                nlayers, layer,
                model->fanmdata[anim->blend_src_action], anim->blend_src_time,
                model->fanmdata[anim->blend_dst_action], anim->blend_dst_time,
                (float)anim->blend_frame / (float)anim->blend_frames);
        } else if ((anim->blend == 0)
                   && ((u16)anim->action <= 0x75)
                   && (model->fanmdata[anim->action] != 0)) {
            dwa = NuHGobjEvalDwa(nlayers, layer,
                                 model->fanmdata[anim->action],
                                 anim->anim_time);
        } else {
            dwa = 0;
        }
    } else {
        dwa = 0;
    }

    model2 = model;
    if (model->character == 0x54) {
        if (LBIT & 0x0000000400000040ULL) {
            if (CRemap[115] != -1) {
                model2 = &CModel[CRemap[115]];
            }
        } else if (CRemap[0] != -1) {
            model2 = &CModel[CRemap[0]];
        }
    } else if (model->character == 0x9f) {
        if (CRemap[8] != -1) {
            model2 = &CModel[CRemap[8]];
        }
    }

    pJ = 0;
    nJ = 0;
    if (((jeep_draw == 0) && (plr_render != 0))
        && ((player->target != 0)
            && ((VEHICLECONTROL != 1) || (player->obj.vehicle == -1)))
        && ((model2->character == 0) || (model2->character == 0x54)
            || (model2->character == 0x8c))) {
        pJ = joint;
        nJ = 1;
    }
    if (ChrisJointOveride != 0) {
        pJ = ChrisJointList;
        nJ = ChrisNumJoints;
    }

    if (anim != 0) {
        if (anim->blend != 0) {
            if ((((u16)anim->blend_src_action < 0x76)
                 && (model2->anmdata[anim->blend_src_action] != 0))
                && (((u16)anim->blend_dst_action < 0x76)
                    && (model2->anmdata[anim->blend_dst_action] != 0))) {
                NuHGobjEvalAnimBlend(
                    model2->hobj,
                    model2->anmdata[anim->blend_src_action],
                    anim->blend_src_time,
                    model2->anmdata[anim->blend_dst_action],
                    anim->blend_dst_time,
                    (float)anim->blend_frame / (float)anim->blend_frames,
                    nJ, pJ, tmtx);
                action = anim->blend_dst_action;
                time = anim->blend_dst_time;
                goto AfterEval;
            }
            goto NoModelAnim;
        }
        if (((u16)anim->action >= 0x76)
            || (model2->anmdata[anim->action] == 0)) {
            goto NoModelAnim;
        }
        NuHGobjEvalAnim(model2->hobj, model2->anmdata[anim->action],
                        anim->anim_time, nJ, pJ, tmtx);
        action = anim->action;
        time = anim->anim_time;
    } else {
    NoModelAnim:
        NuHGobjEval(model2->hobj, nJ, pJ, tmtx);
        action = -1;
    }

AfterEval:
    if ((glass_draw == 0) && (loc_mtx != 0)) {
        /* StoreLocatorMatrices(model2, mC, tmtx, loc_mtx, loc_mom) inlined */
        for (i = 0; i < 0x10; i++) {
            if (model2->pLOCATOR[i] != 0) {
                oldpos.x = loc_mtx[i]._30;
                oldpos.y = loc_mtx[i]._31;
                oldpos.z = loc_mtx[i]._32;
                NuHGobjPOIMtx(model2->hobj, i, mC, tmtx, &m);
                loc_mtx[i] = m;
                if (loc_mom != 0) {
                    loc_mom[i].x = loc_mtx[i]._30 - oldpos.x;
                    loc_mom[i].y = loc_mtx[i]._31 - oldpos.y;
                    loc_mom[i].z = loc_mtx[i]._32 - oldpos.z;
                }
            }
        }
    }

    if ((action != -1) && (loc_mtx != 0) && (Paused == 0)
        && (glass_draw == 0)) {
        AddAnimDebris(model, loc_mtx, action, time, obj);
    }

    if (render != 0) {
        if ((plr_render != 0) && (model->character == 0)
            && (player->obj.dead == 0x12)) {
            nlayers = 1;
        }
        Drawn = NuHGobjRndrMtxDwa(model->hobj, mC, nlayers, layer, tmtx,
                                 dwa);
        if ((Drawn != 0) && (obj != 0) && (obj->character == 0xb1)) {
            DrawProbeFX(obj);
        }
        if (mR != 0) {
            NuHGobjRndrMtxDwa(model->hobj, mR, nlayers, layer, tmtx, dwa);
        }
        if (mS != 0) {
            if (model->shadhdr != 0) {
                scale = CData[model->character].shadow_scale;
                if (model->character == 0x99) {
                    ShadRndr(mS, model->sanmdata[98], 1.0f, scale);
                } else if (anim->blend != 0) {
                    if (((u16)anim->blend_dst_action < 0x76)
                        && (model->anmdata[anim->blend_dst_action] != 0)) {
                        ShadRndr(mS, model->sanmdata[anim->blend_dst_action],
                                 anim->blend_dst_time, scale);
                    } else {
                        ShadRndr(mS, model->shaddata, 1.0f, scale);
                    }
                } else if (((u16)anim->action < 0x76)
                           && (model->anmdata[anim->action] != 0)) {
                    ShadRndr(mS, model->sanmdata[anim->action],
                             anim->anim_time, scale);
                } else {
                    ShadRndr(mS, model->shaddata, 1.0f, scale);
                }
            }
        }
    }

Exit:
    plr_render = 0;
    jeep_draw = 0;
    return Drawn;
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
            anim->action = anim->blend_dst_action;
            anim->anim_time = anim->blend_dst_time;
            anim->blend = 0;
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
                        (mod->anmdata[3]->time - 1.0f)) /
                        (float)player->OnFootMoveInfo->CROUCHINGFRAMES;
                } else if (anim->blend_dst_action == 5) {
                    anim->blend_dst_time =
                        ((float)(player->OnFootMoveInfo->CROUCHINGFRAMES -
                                 player->crouch_pos) *
                         (mod->anmdata[5]->time - 1.0f)) /
                        (float)player->OnFootMoveInfo->CROUCHINGFRAMES;
                }
            }
            anim->blend_frame = 0;
            anim->blend_frames =
                (u16)mod->animlist[anim->newaction]->blend_in_frames;
            if (mod->animlist[anim->oldaction]->blend_out_frames <
                anim->blend_frames) {
                anim->blend_frames =
                    (u16)mod->animlist[anim->oldaction]->blend_out_frames;
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
            t *= xz_distance * 10.0;
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
            t *= xz_distance * 10.0;
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
            t *= xz_distance * 10.0;
        }
        anim->anim_time += t;
        t = mod->anmdata[anim->action]->time;
        if (anim->anim_time > t) {
            if ((mod->animlist[anim->action]->flags & 1) != 0) {
                anim->flags = 2;
                anim->anim_time -= t - 1.0f;
                return;
            }
            anim->anim_time = t;
            anim->flags = 1;
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
