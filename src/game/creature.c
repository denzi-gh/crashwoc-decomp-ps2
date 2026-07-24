/* .\creature.c -- creatures, the player, character models
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
    u8 unk_0x000[0x9];       /* 0x000 (opaque) */
    u8 vibration;            /* 0x009 */
    u8 unk_0x00A[0x1E];      /* 0x00A */
    struct gamelevel_s level[35]; /* 0x028 */
    u8 lives;                /* 0x3FC */
    u8 wumpas;               /* 0x3FD */
    u8 mask;                 /* 0x3FE */
    u8 unk_0x3FF[7];         /* 0x3FF */
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
    u8 unk_0x000[0xA4];      /* 0x000 (opaque) */
    struct nuvec_s pos;      /* 0x0A4 */
    u8 unk_0x0B0[0x48];      /* 0x0B0 (opaque) */
    s32 yrot;                /* 0x0F8 */
    u8 unk_0x0FC[0x10];      /* 0x0FC (opaque) */
    unsigned short hdg_to_player; /* 0x10C */
    u8 unk_0x10E[6];         /* 0x10E */
    signed char mode;        /* 0x114 */
};

struct pad_s {
    u8 unk_0x000[0x55C];     /* 0x000 (opaque) */
    unsigned int paddata;    /* 0x55C (held buttons) */
    u8 unk_0x560[4];         /* 0x560 */
    unsigned int buttons;    /* 0x564 (debounced buttons) */
    u8 unk_0x568[0x16];      /* 0x568 */
    u8 l_alg_x;              /* 0x57E */
    u8 l_alg_y;              /* 0x57F */
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


inline void TerrainFailsafe(struct obj_s *obj)
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
int *InstShadDataLoad(char *name);
struct NUSHADOWDATA_s *ShadFindData(int *hdr, char *name);
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
                model->shadhdr = *(int *)InstShadDataLoad(tbuf);
                if (model->shadhdr != 0) {
                    model->shaddata[anim->action] = ShadFindData(&model->shadhdr, 0);
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
                                        model->shaddata[anim->action] =
                                            ShadFindData((int *)model->shadhdr,
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


/* --- MovePlayer externals ------------------------------------------ */

/* Menu cursor (0x591F70); only menu (+0x6E) and wait (+0x71) are typed. */
struct cursor_s {
    u8 unk_0x00[0x6E];       /* 0x00 (opaque) */
    s8 menu;                 /* 0x6E */
    u8 unk_0x6F[2];          /* 0x6F */
    u8 wait;                 /* 0x71 */
};

/* Terrain surface table entry (stride 8): friction +0, flags +6. */
struct tersurface_s {
    f32 friction;            /* 0x0 */
    short unk_0x04;          /* 0x4 */
    unsigned short flags;    /* 0x6 */
};

/* Debris table entry (stride 0x10); only the effect id (+0) is used. */
struct gdeb_s {
    s32 i;                   /* 0x0 */
    u8 unk_0x04[0xC];        /* 0x4 */
};

struct nuinstance_s {
    struct numtx_s matrix;   /* 0x00 */
    s32 objid;               /* 0x40 */
};

struct nugscn_s {
    short *tids;             /* 0x00 */
    s32 numtid;              /* 0x04 */
    void *mtls;              /* 0x08 */
    s32 nummtl;              /* 0x0C */
    s32 numgobj;             /* 0x10 */
    void **gobjs;            /* 0x14 */
};

struct nuspecial_s {
    struct numtx_s mtx;             /* 0x00 */
    struct nuinstance_s *instance;  /* 0x40 */
    char *name;                     /* 0x44 */
};

/* Level 3D-object table entry (stride 0x20). */
struct objtab_s {
    struct nugscn_s *scene;      /* 0x00 */
    struct nuspecial_s *special; /* 0x04 */
    u8 pad[24];                  /* 0x08 */
}; /* 0x20 */

extern struct MoveInfo ScooterMoveInfo;
extern struct MoveInfo SnowBoardMoveInfo;
extern struct MoveInfo MechMoveInfo;
extern struct MoveInfo FireEngineMoveInfo;
extern struct MoveInfo GyroMoveInfo;
extern struct MoveInfo SubmarineMoveInfo;
extern struct MoveInfo MineTubMoveInfo;
extern struct MoveInfo OffRoaderMoveInfo;
extern struct MoveInfo SwimmingMoveInfo;

extern struct cursor_s Cursor;
extern struct tersurface_s TerSurface[];
extern struct gdeb_s GDeb[];
extern struct objtab_s ObjTab[];

extern f32 vtog_time;
extern f32 vtog_duration;
extern s32 vtog_blend;
extern struct nuvec_s vtog_oldpos;
extern struct nuvec_s vtog_newpos;
extern u16 vtog_angle;
extern s32 gamesfx_effect_volume;
extern f32 plr_vehicle_speedmul;
extern f32 plr_vehicle_time;
extern f32 VEHICLETIME;
extern f32 tumble_item_starttime;
extern f32 tumble_cycleduration;
extern f32 tumble_moveduration;
extern struct nuvec_s tumble_newpos;
extern struct nuvec_s tumble_oldpos;
extern u16 tumble_hdg;
extern u16 new_lev_flags;
extern u16 temp_lev_flags;
extern s32 warp_level;
extern s32 in_finish_range;
extern s32 FireBossHoldPlayer;
extern s32 fadeval;
extern s32 SmokeyCountDownValue;
extern f32 SMOKEYSPEED;
extern f32 SMOKEYBOOSTSPEED;
extern f32 offroader_speedtime;
extern f32 OFFROADERSEEK;
extern f32 TERMINALVELOCITY;
extern f32 GRAVITY;
extern u16 best_railangle;
extern s32 plr_target_found;
extern s32 plr_target_frame;
extern struct nuvec_s plr_target_pos[2];
extern struct nuvec_s plr_target_dir;
extern struct nuvec_s plr_target_firepos;
extern struct nuvec_s plr_target_sightpos;
extern struct numtx_s plr_target_mtx;
extern struct nuvec_s v001;
extern f32 MECHTARGETHACK;
extern f32 BAZOOKATARGETHACK;
extern s32 ExtraMoves;
extern f32 in_speed;
extern f32 in_s_friction;
extern f32 in_f_friction;
extern s32 LIFTPLAYER;
extern s32 temp_crate_y_ceiling_adjust;
extern s32 temp_crate_y_floor_adjust;
extern s32 temp_crate_xz_adjust;
extern s32 NOTERRAINSTOP;
extern s32 plr_terrain_ok;
extern s32 jonframe1;
extern s32 plr_allow_jump;
extern s32 InvincibilityCHEAT;
extern s32 loadsave_frame;
extern struct nuvec_s loadsavepos;
extern f32 NuTrigTable[];

void NuPs2PadSetMotors(struct pad_s *pad, s32 small, s32 big);
void ToggleVehicle(struct creature_s *c);
void ResetTubs(void);
u16 SeekRot(u16 a, u16 target, s32 rate);
f32 NewShadowMaskPlat(struct nuvec_s *pos, f32 y, s32 layer);
f32 NewShadowMaskPlatRot(struct nuvec_s *pos, f32 y, s32 layer);
void ObjectRotation(struct obj_s *obj, s32 mode, s32 vehicle);
s32 AddAward(s32 hub, s32 level, s32 bits);
void AddGameDebris(s32 type, struct nuvec_s *pos);
f32 CrateTopBelow(struct nuvec_s *pos);
f32 NuVecMag(struct nuvec_s *v);
s32 NuAtan2D(f32 x, f32 z);
void NuVecRotateY(struct nuvec_s *dst, struct nuvec_s *src, s32 angle);
void NuVecRotateX(struct nuvec_s *dst, struct nuvec_s *src, s32 angle);
void MoveVehicle(struct creature_s *c, struct pad_s *pad);
f32 NuFsqrt(f32 x);
f32 NuFabs(f32 x);
void GameRayCast(struct nuvec_s *pos, struct nuvec_s *dir, f32 dist,
                 struct nuvec_s *hit);
void NuMtxSetRotationX(struct numtx_s *m, s32 r);
void NuMtxRotateY(struct numtx_s *m, s32 r);
void NuVecSub(struct nuvec_s *d, struct nuvec_s *a, struct nuvec_s *b);
void NuVecNorm(struct nuvec_s *d, struct nuvec_s *a);
f32 NuVecDot(struct nuvec_s *a, struct nuvec_s *b);
void NuVecAdd(struct nuvec_s *d, struct nuvec_s *a, struct nuvec_s *b);
void MoveLoopXZ(struct obj_s *obj, u16 *hdg);
void NewTerrainScaleY(struct nuvec_s *pos, struct nuvec_s *mom, u8 *chrs,
                      s32 index, f32 adjust, f32 radius, f32 ratio);
s32 PlatformCrush(void);
s32 GetDieAnim(struct obj_s *obj, s32 anim);
void KillPlayer(struct obj_s *obj, s32 anim);
void BonusTransporter(struct creature_s *c);
void DeathTransporter(struct creature_s *c);
void GemPathTransporter(struct creature_s *c);
void PlayerCreatureCollisions(struct obj_s *obj);
void HitItems(struct obj_s *obj);
s32 PlayerObjectAnimCollision(struct obj_s *obj, struct nuspecial_s *special,
                              f32 range);
void CrateCollisions(struct obj_s *obj);
void NuRndrAddWaterRipple(struct nuvec_s *pos, f32 size, f32 grow, s32 frames,
                          u32 colour);
void AddVariableShotDebrisEffect(s32 type, struct nuvec_s *pos, s32 count,
                                 s32 a, s32 b);
void LoseMask(struct obj_s *obj);
void KillGameObject(struct obj_s *obj, s32 anim);
void AddMaskFeathers(struct mask_s *mask);
void WumpaCollisions(struct obj_s *obj);
void BonusTiming(struct creature_s *c);
void NewMenu(struct cursor_s *cursor, s32 menu, s32 mode, s32 item);
void MoveSUBMARINE(struct creature_s *c, struct pad_s *pad);
void MoveSCOOTER(struct creature_s *c, struct pad_s *pad);
void MoveSNOWBOARD(struct creature_s *c, struct pad_s *pad);
void MoveMECH(struct creature_s *c, struct pad_s *pad);
void MoveFIREENGINE(struct creature_s *c, struct pad_s *pad);
void MoveGYRO(struct creature_s *c, struct pad_s *pad);
void MoveMINECART(struct creature_s *c, struct pad_s *pad);
void MoveMINETUB(struct creature_s *c, struct pad_s *pad);
void MoveOFFROADER(struct creature_s *c, struct pad_s *pad);
void MoveCOCO(struct creature_s *c, struct pad_s *pad);
void MoveSWIMMING(struct creature_s *c, struct pad_s *pad);
void MoveCRASH(struct creature_s *c, struct pad_s *pad);
void AnimateDIVE(struct creature_s *c, f32 t);
void AnimateATLASPHERE(struct creature_s *c);
void AnimateSUBMARINE(struct creature_s *c);
void AnimateSCOOTER(struct creature_s *c);
void AnimateSNOWBOARD(struct creature_s *c);
void AnimateGLIDER(struct creature_s *c);
void AnimateDROPSHIP(struct creature_s *c);
void AnimateGYRO(struct creature_s *c, struct pad_s *pad);
void AnimateMECH(struct creature_s *c);
void AnimateFIREENGINE(struct creature_s *c);
void AnimateJEEP(struct creature_s *c);
void AnimateMINECART(struct creature_s *c);
void AnimateMINETUB(struct creature_s *c);
void AnimateMOSQUITO(struct creature_s *c);
void AnimateOFFROADER(struct creature_s *c);
void AnimateSWIMMING(struct creature_s *c);
void AnimateCOCO(struct creature_s *c);
void AnimateCRASH(struct creature_s *c);


inline void UpdateRumble(struct rumble_s *rumble)
{
    if (rumble->buzz != 0) {
        rumble->buzz--;
    }
    if (rumble->frame != 0) {
        rumble->frame--;
    }
}


inline void NewRumble(struct rumble_s *rumble, s32 power)
{
    if ((rumble->frame != 0) &&
        (power <= (rumble->power * rumble->frame) / rumble->frames)) {
        return;
    }
    rumble->power = power;
    rumble->frames = (power * 0x32) >> 8;
    rumble->frame = (power * 0x32) >> 8;
}


inline void NewBuzz(struct rumble_s *rumble, s32 frames)
{
    if (frames > rumble->buzz) {
        rumble->buzz = frames;
    }
}


void MovePlayer(struct creature_s *c, struct pad_s *pad)
{
    u16 gotlist[9] = {0x8, 0x7, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400};
    struct nuvec_s in;
    struct nuvec_s in2;
    struct nuvec_s d0;
    struct nuvec_s c0;
    struct nuvec_s b0;
    struct nuvec_s av;
    struct nuvec_s tv;
    u16 hold;
    s32 old_dangle;
    s32 old_found;
    s32 old_layer;
    struct MoveInfo *minfo;
    s32 veh;
    s32 oldvc;
    s32 i;
    f32 spd;

    UpdateRumble(&c->rumble);
    if (pad != 0) {
        if (Game.vibration != 0 && Demo == 0) {
            s32 v;
            if (c->rumble.frame != 0) {
                v = (c->rumble.power * c->rumble.frame) / c->rumble.frames;
            } else {
                v = 0;
            }
            NuPs2PadSetMotors(pad, c->rumble.buzz != 0, v);
        } else {
            NuPs2PadSetMotors(pad, 0, 0);
        }
    }
    oldvc = VEHICLECONTROL;
    if (c->obj.vehicle != -1) {
        if ((LBIT & 0x1105252801ULL) != 0 || Level == 3 || Level == 0x1D ||
            Level == 0x1C) {
            VEHICLECONTROL = 1;
        } else if ((LBIT & 0x400000040ULL) != 0 && oldvc == 0) {
            VEHICLECONTROL = 1;
        }
    } else {
        VEHICLECONTROL = 0;
    }
    if (VEHICLECONTROL == 1 && vtog_time == vtog_duration) {
        c->obj.vehicle_frame++;
    } else {
        c->obj.vehicle_frame = 0;
        plr_vehicle_speedmul = 1.0f;
        plr_vehicle_time = 0.0f;
    }
    ToggleVehicle(c);
    veh = -1;
    if (VEHICLECONTROL == 1 && c->obj.vehicle != -1) {
        veh = c->obj.vehicle;
    }
    if (veh == 0x6B) {
        minfo = &ScooterMoveInfo;
    } else if (veh == 0xA0) {
        minfo = &SnowBoardMoveInfo;
    } else if (veh == 0x44) {
        minfo = &MechMoveInfo;
    } else if (veh == 0xB2) {
        minfo = &FireEngineMoveInfo;
    } else if (veh == 0x3B) {
        minfo = &GyroMoveInfo;
    } else if (veh == 0x20) {
        minfo = &SubmarineMoveInfo;
    } else if (veh == 0x89) {
        minfo = &MineCartMoveInfo;
    } else if (veh == 0xA1) {
        minfo = &MineTubMoveInfo;
    } else if (veh == 0x99) {
        minfo = &OffRoaderMoveInfo;
    } else if (VEHICLECONTROL == 2) {
        minfo = &SwimmingMoveInfo;
    } else {
        minfo = c->OnFootMoveInfo;
    }
    if (VEHICLECONTROL == 1 && oldvc != 1) {
        plr_vehicle_time = 0.0f;
        if (veh == 0x89) {
            VEHICLETIME = 60.0f;
        } else if (veh == 0x63) {
            ResetJeep(c);
        }
        ResetTubs();
    }
    if (c->obj.invincible != 0) {
        c->obj.invincible--;
    }
    if (c->freeze != 0) {
        c->freeze--;
    }
    c->obj.oldpos = c->obj.pos;
    c->obj.old_ground = c->obj.ground;
    old_dangle = c->obj.dangle;
    GetTopBot(c);
    OldTopBot(&c->obj);
    if (GameMode != 1) {
        if (vtog_time < vtog_duration) {
            vtog_time += 0.02f;
            if (vtog_time >= vtog_duration * 0.333f &&
                vtog_time < vtog_duration * 0.333f) {
                gamesfx_effect_volume = 0x2FFF;
                GameSfx(0x2F, &c->obj.pos);
            }
            if (vtog_time >= vtog_duration) {
                vtog_time = vtog_duration;
                if (VEHICLECONTROL == 1) {
                    i = (veh == 0xA0) ? 0x54 : 0x48;
                    if (i != -1) {
                        GameSfx(i, &c->obj.pos);
                    }
                    NewRumble(&c->rumble, 0xBF);
                    NewBuzz(&c->rumble, 0xA);
                }
            }
            if (vtog_blend != 0) {
                f32 t = vtog_time / vtog_duration;
                c->obj.pos.x = (vtog_newpos.x - vtog_oldpos.x) * t + vtog_oldpos.x;
                c->obj.pos.y = (vtog_newpos.y - vtog_oldpos.y) * t + vtog_oldpos.y;
                c->obj.pos.z = (vtog_newpos.z - vtog_oldpos.z) * t + vtog_oldpos.z;
                c->obj.mom = v000;
                c->obj.thdg = vtog_angle;
                c->obj.hdg = SeekRot(c->obj.hdg, vtog_angle, 3);
                c->obj.shadow = NewShadowMaskPlat(&c->obj.pos, 0.0f, -1);
                if (c->obj.shadow != 2000000.0f) {
                    GetSurfaceInfo(&c->obj, 1, c->obj.shadow);
                }
                if ((LBIT & 0x400000040ULL) == 0) {
                    c->obj.pos.y += NuTrigTable[(s32)(t * 32768.0f) & 0xFFFF] +
                                    NuTrigTable[(s32)(t * 32768.0f) & 0xFFFF];
                }
                ObjectRotation(&c->obj, 2, 1);
                if (Level != 0x16 || vtog_time != vtog_duration) {
                    goto post_move;
                }
            }
        }
        if (Level == 0x25) {
            if (tumble_time < tumble_duration) {
                tumble_time += 0.02f;
                if (tumble_action == 0x56 && new_lev_flags != 0) {
                    if (c->obj.anim.anim_time -
                            c->obj.model->animlist[0x56]->speed * 0.59999996f <
                            tumble_item_starttime + 1.0f &&
                        tumble_item_starttime + 1.0f <= c->obj.anim.anim_time) {
                        for (i = 0; i < 9; i++) {
                            u16 bits = new_lev_flags & gotlist[i];
                            if (bits != 0 && (temp_lev_flags & bits) == 0) {
                                temp_lev_flags |= bits;
                                if (AddAward(Hub, last_level, bits) == 0) {
                                    new_lev_flags = bits ^ (bits | new_lev_flags);
                                    Game.level[last_level].flags |= bits;
                                }
                                i = 9;
                            }
                        }
                    }
                }
                if (tumble_time >= tumble_duration) {
                    if ((new_lev_flags | temp_lev_flags) != temp_lev_flags) {
                        tumble_time = tumble_cycleduration;
                        c->obj.anim.anim_time = tumble_item_starttime;
                    } else {
                        tumble_time = tumble_duration;
                        c->jump = 1;
                        if (tumble_action == 0x56) {
                            c->jump_type = 0;
                            c->jump_subtype = qrand() / 32768;
                        } else {
                            c->jump_type = 1;
                        }
                        if (c->jump_type == 0) {
                            c->jump_frames = minfo->STARJUMPFRAMES;
                        } else {
                            c->jump_frames = minfo->JUMPFRAMES0;
                        }
                        c->jump_frame = 0;
                        c->somersault = 0;
                        c->land = 0;
                        c->obj.ground = 0;
                        c->obj.anim.anim_time = 1.0f;
                        c->jump_hack = 1;
                        AddGameDebris(0x10, &c->obj.pos);
                        AddGameDebris(0x11, &c->obj.pos);
                        GameSfx(0x2F, &c->obj.pos);
                    }
                }
            }
            if (tumble_time < tumble_duration && last_hub != -1) {
                if (tumble_time >= tumble_moveduration) {
                    c->obj.pos.x = tumble_newpos.x;
                    c->obj.pos.y = tumble_newpos.y;
                    c->obj.pos.z = tumble_newpos.z;
                } else {
                    f32 t = tumble_time / tumble_moveduration;
                    c->obj.pos.x = (tumble_newpos.x - tumble_oldpos.x) * t +
                                   tumble_oldpos.x;
                    c->obj.pos.y = (tumble_newpos.y - tumble_oldpos.y) * t +
                                   tumble_oldpos.y;
                    c->obj.pos.z = (tumble_newpos.z - tumble_oldpos.z) * t +
                                   tumble_oldpos.z;
                    c->obj.shadow = NewShadowMaskPlat(&c->obj.pos, 0.0f, -1);
                    if (c->obj.shadow != 2000000.0f) {
                        GetSurfaceInfo(&c->obj, 1, c->obj.shadow);
                    }
                }
                c->obj.ground = 3;
                c->obj.thdg = tumble_hdg;
                c->obj.hdg = tumble_hdg;
                c->obj.old_ground = 3;
                goto terrain;
            } else if (Level == 0x25) {
                if (warp_level != -1) {
                    goto post_move;
                }
            }
        }
        if ((c->obj.dead != 0 && Level != 0x1D) ||
            (Cursor.menu != -1 && Cursor.menu != 0x1F && Cursor.menu != 0x21 &&
             Cursor.menu != 0x10 && (Cursor.menu != 0xE || Level != 0x1D)) ||
            Cursor.wait != 0 ||
            (c->obj.finished != 0 && veh != 0x63 && veh != 0xA1 &&
             veh != 0x36 && veh != 0x8B && veh != 0x81 && veh != 0x3B)) {
            if (c->obj.dead == 0xA) {
                c->obj.pos.y += (c->obj.layer_shadow - c->obj.pos.y) * 0.04f;
            } else if (veh != 1 &&
                       (c->obj.dead == 6 || c->obj.dead == 0x10 ||
                        c->obj.dead == 0x12 || c->obj.dead == 0x13 ||
                        c->obj.dead == 0xD || veh == 0x6B || veh == 0xA0 ||
                        veh == 0x99)) {
                c->obj.shadow = NewShadowMaskPlat(&c->obj.pos, 0.0f, -1);
                if (c->obj.shadow != 2000000.0f) {
                    f32 crate;
                    tv.x = c->obj.pos.x;
                    tv.y = (c->obj.bot + c->obj.top) * c->obj.SCALE * 0.5f +
                           c->obj.pos.y;
                    tv.z = c->obj.pos.z;
                    b0 = tv;
                    crate = CrateTopBelow(&b0);
                    if (crate != 2000000.0f && c->obj.shadow < crate) {
                        c->obj.shadow = crate;
                    }
                    if (c->obj.dead != 0x13) {
                        f32 m = c->obj.mom.y + GRAVITY;
                        f32 ny = c->obj.pos.y + m;
                        f32 b = c->obj.bot * c->obj.SCALE;
                        c->obj.mom.y = m;
                        c->obj.pos.y = ny;
                        if (ny + b < c->obj.shadow) {
                            c->obj.pos.y = c->obj.shadow - b;
                        }
                    } else {
                        c->obj.pos.y = c->obj.shadow -
                                       c->obj.bot * c->obj.SCALE;
                    }
                }
            }
            goto post_move;
        }
        if (Level == 0x1D) {
            if (c->obj.dead != 0 ||
                (c->obj.finished != 0 && in_finish_range == 0x32)) {
                goto move_dispatch;
            }
        }
        in.y = 0.0f;
        if (GameTimer.frame < 0x32 || c->obj.finished != 0 || c->freeze != 0 ||
            (Level == 0x16 && FireBossHoldPlayer != 0) || Cursor.menu == 0x1F ||
            Cursor.menu == 0x21 || fadeval > 0) {
            c->pad_type = 1;
            in.x = 0.0f;
            in.z = 0.0f;
            spd = in.x;
            c->obj.pad_angle = 0;
            c->obj.pad_speed = spd;
            c->obj.pad_dx = spd;
            c->obj.pad_dz = spd;
        } else {
            f32 fx;
            f32 fz;
            f32 m;
            if ((pad->paddata & 0xF000) != 0) {
                if ((pad->paddata & 0x8000) != 0) {
                    fx = -127.5f;
                } else {
                    fx = in.y;
                    if ((pad->paddata & 0x2000) != 0) {
                        fx = 127.5f;
                    }
                }
                if ((pad->paddata & 0x4000) != 0) {
                    fz = -127.5f;
                } else {
                    fz = 0.0f;
                    if ((pad->paddata & 0x1000) != 0) {
                        fz = 127.5f;
                    }
                }
                if (fx != 0.0f || fz != 0.0f) {
                    c->pad_type = 1;
                }
            } else {
                fz = -((f32)pad->l_alg_y - 127.5f);
                fx = (f32)pad->l_alg_x - 127.5f;
                if (fx * fx + fz * fz < 1806.25f) {
                    double d;
                    d = fx;
                    if (d < 0.0) {
                        d = 0.0 - d;
                    }
                    if (d < 42.5) {
                        fx = 0.0f;
                    }
                    d = fz;
                    if (d < 0.0) {
                        d = 0.0 - d;
                    }
                    if (d < 42.5) {
                        fz = 0.0f;
                    }
                }
                if (fx != 0.0f || fz != 0.0f) {
                    c->pad_type = 2;
                }
            }
            in.z = fz * 0.00784313772f;
            in.x = fx * 0.00784313772f;
            NuVecMag(&in);
            i = NuAtan2D(in.x, in.z);
            in2.x = 0.0f;
            in2.y = 0.0f;
            {
                double ax;
                double az;
                double am;
                f32 big;
                ax = in.x;
                if (ax < 0.0) {
                    ax = 0.0 - ax;
                }
                az = in.z;
                if (az < 0.0) {
                    az = 0.0 - az;
                }
                if (ax > az) {
                    big = in.x;
                } else {
                    big = in.z;
                }
                am = big;
                if (am < 0.0) {
                    am = 0.0 - am;
                }
                in2.z = am;
            }
            NuVecRotateY(&in, &in2, i);
            m = NuVecMag(&in);
            if (m < 0.2f) {
                spd = 0.0f;
            } else if (m < 0.6f) {
                spd = minfo->WALKSPEED;
            } else {
                spd = minfo->RUNSPEED;
            }
            c->obj.pad_speed = spd;
            c->obj.pad_dx = in.x;
            c->obj.pad_dz = in.z;
            c->obj.pad_angle = NuAtan2D(in.x, in.z);
        }
        if (VEHICLECONTROL == 1 && c->obj.vehicle != 0x3B &&
            c->obj.vehicle != 0x20 && c->obj.vehicle != 0x6B &&
            c->obj.vehicle != 0xA0 && c->obj.vehicle != 0x44 &&
            c->obj.vehicle != 0xB2 && c->obj.vehicle != 0x89 &&
            c->obj.vehicle != 0xA1 && c->obj.vehicle != 0x99) {
            f32 dx;
            f32 dy;
            f32 xx;
            f32 zz;
            c->obj.boing = 0;
            MoveVehicle(c, pad);
            dx = c->obj.pos.x - c->obj.oldpos.x;
            xx = dx * dx;
            dx = c->obj.pos.z - c->obj.oldpos.z;
            zz = dx * dx;
            dy = c->obj.pos.y - c->obj.oldpos.y;
            c->obj.xz_distance = NuFsqrt(xx + zz);
            c->obj.xyz_distance = NuFsqrt(dy * dy + xx + zz);
            goto post_move;
        }
        if (veh == 0x6B || veh == 0xA0 || veh == 0x99 || veh == 0xA1 ||
            veh == 0xB2) {
            s32 frozen;
            s32 onice;
            s32 rate;
            u16 oldhdg;
            frozen = 0;
            if (Level == 3) {
                frozen = SmokeyCountDownValue > 0;
            }
            onice = 0;
            if (c->obj.ground != 0) {
                onice = c->obj.surface_type == 10;
            }
            if (Level == 3) {
                spd = ((c->fire != 0) ? SMOKEYBOOSTSPEED : SMOKEYSPEED) *
                      0.02f * offroader_speedtime;
                if (onice) {
                    spd = spd * 0.25;
                }
            } else if (Level == 0x16) {
                c->sprint = 0;
                if (FireBossHoldPlayer != 0) {
                    spd = 0.0f;
                } else if ((pad->paddata & 0x80) != 0) {
                    spd = minfo->SPRINTSPEED;
                    c->sprint = 1;
                } else {
                    spd = minfo->RUNSPEED;
                }
            } else {
                if ((pad->paddata & 0x28) != 0 ||
                    (veh == 0x99 && (pad->paddata & 0x40) != 0)) {
                    spd = minfo->SPRINTSPEED;
                    c->sprint = 1;
                } else {
                    spd = minfo->RUNSPEED;
                    c->sprint = 0;
                }
            }
            {
                u16 t = (best_cRPos != 0) ? best_cRPos->angle : c->obj.thdg;
                c->obj.thdg = t;
                hold = t;
            }
            rate = 3;
            if (best_cRPos != 0 && ((best_cRPos->mode & 3) != 0 || Level == 9)) {
                s32 amt;
                short d16;
                if (Level == 3) {
                    amt = 0x2AAB;
                    rate = 5;
                } else if (veh == 0xA0) {
                    amt = 0x2000;
                } else if (veh == 0x6B || veh == 0xB2) {
                    amt = 0x1555;
                } else {
                    amt = 0x1000;
                }
                d16 = (s32)((f32)amt * c->obj.pad_dx);
                if (frozen == 0) {
                    if ((best_cRPos->mode & 1) != 0 && Level != 9) {
                        c->obj.thdg += d16;
                    } else {
                        c->obj.thdg -= d16;
                    }
                }
            }
            oldhdg = c->obj.hdg;
            if (frozen == 0 && rate != 0) {
                c->obj.hdg = SeekRot(oldhdg, c->obj.thdg, rate);
            }
            c->obj.dyrot = RotDiff(oldhdg, c->obj.hdg);
            if (frozen == 0) {
                c0.x = NuTrigTable[c->obj.hdg] * spd;
                c0.z = NuTrigTable[(u16)(c->obj.hdg + 0x4000)] * spd;
            }
            c->obj.dangle = 0;
            if (frozen == 0) {
                f32 k;
                if (Level == 3) {
                    f32 seekmax = OFFROADERSEEK * 1.2f;
                    k = (seekmax - 0.3f) * (c->obj.xz_distance / 0.06f) + 0.3f;
                    if (k < seekmax) {
                        k = seekmax;
                    }
                } else {
                    k = 0.3f;
                }
                c->obj.mom.x += (c0.x - c->obj.mom.x) * k;
                c->obj.mom.z += (c0.z - c->obj.mom.z) * k;
            } else {
                c->obj.mom.x = c->obj.mom.z = 0.0f;
            }
            if (c->obj.mom.y < -TERMINALVELOCITY) {
                c->obj.mom.y = -TERMINALVELOCITY;
            } else if (c->obj.mom.y > TERMINALVELOCITY) {
                c->obj.mom.y = TERMINALVELOCITY;
            }
            if (best_cRPos != 0) {
                s32 d = RotDiff(best_cRPos->angle, c->obj.hdg);
                if (d < 0) {
                    d = -d;
                }
                if (d < 0x2AAB) {
                    c->obj.direction = 0;
                } else if (d < 0x5555) {
                    c->obj.direction = 2;
                } else {
                    c->obj.direction = 1;
                }
            } else {
                c->obj.direction = 0;
            }
        } else if (veh == 0x3B && best_cRPos != 0) {
            f32 t;
            if (Level == 0x1D && GameTimer.frame < 0x96) {
                b0 = v000;
                t = 0.333f;
                c->obj.direction = 0;
            } else {
                if (c->obj.pad_speed > 0.0f && c->tap == 0) {
                    b0.x = c->obj.pad_dx * minfo->WALKSPEED;
                    b0.z = 0.0f;
                    b0.y = -c->obj.pad_dz * minfo->WALKSPEED;
                    NuVecRotateY(&b0, &b0, best_railangle);
                    t = 1.0f;
                } else {
                    t = 0.333f;
                    b0 = v000;
                }
                if ((pad->buttons & 0x60) != 0) {
                    NewRumble(&player->rumble, 0x9F);
                } else if ((pad->paddata & 0x60) != 0 && qrand() < 0x4000) {
                    s32 r = qrand();
                    NewRumble(&player->rumble, r / 512);
                }
                {
                    u32 bt = pad->paddata & 0x60;
                    if (c->tap == 0) {
                        if (bt == 0x40) {
                            if (c->obj.direction != 0) {
                                c->obj.direction = 0;
                                c->tap = 0x19;
                            } else {
                                b0.x += NuTrigTable[best_railangle] *
                                        minfo->RUNSPEED;
                                t = 1.0f;
                                b0.z += NuTrigTable[(u16)(best_railangle + 0x4000)] *
                                        minfo->RUNSPEED;
                            }
                        } else if (bt == 0x20) {
                            if (c->obj.direction != 1) {
                                c->obj.direction = 1;
                                c->tap = 0x19;
                            } else {
                                b0.x -= NuTrigTable[best_railangle] *
                                        minfo->RUNSPEED;
                                t = 1.0f;
                                b0.z -= NuTrigTable[(u16)(best_railangle + 0x4000)] *
                                        minfo->RUNSPEED;
                            }
                        }
                    }
                }
            }
            {
                f32 k = t * 0.02f;
                s32 d;
                c->obj.thdg = best_railangle;
                c->obj.mom.x += (b0.x - c->obj.mom.x) * k;
                c->obj.mom.y += (b0.y - c->obj.mom.y) * k;
                c->obj.mom.z += (b0.z - c->obj.mom.z) * k;
                if (c->obj.direction == 1) {
                    c->obj.thdg = best_railangle - 0x8000;
                }
                d = RotDiff(c->obj.hdg, c->obj.thdg);
                if (c->tap != 0 && d < 0) {
                    d += 0x10000;
                }
                c->obj.hdg += (short)(d >> 4);
            }
        } else {
            if (c->target == 0) {
                NuVecRotateY(&in, &in, GameCam.yrot);
            }
            c->fire_lock = 0;
            hold = c->obj.hdg;
            old_found = plr_target_found;
            plr_target_found = 0;
            if (c->target == 0) {
                plr_target_found = c->target;
            }
            if (c->target != 0) {
                if (c->fire == 0) {
                    f32 hack;
                    if (veh == 0x44) {
                        s32 tx;
                        s32 ty;
                        if (c->obj.pad_speed > 0.0f) {
                            tx = (s32)(-c->obj.pad_dz * 5461.0f);
                            ty = (s32)(c->obj.pad_dx * 10923.0f);
                        } else {
                            ty = c->obj.target_yrot;
                            tx = c->obj.target_xrot;
                        }
                        c->obj.target_xrot += (short)(tx - c->obj.target_xrot >> 5);
                        c->obj.target_yrot += (short)(ty - c->obj.target_yrot >> 5);
                        plr_target_firepos =
                            *(struct nuvec_s *)&c->mtxLOCATOR[1][0]._30;
                        plr_target_pos[0] = plr_target_firepos;
                        NuVecRotateX(&plr_target_dir, &v001,
                                     -c->obj.target_xrot & 0xFFFF);
                        NuVecRotateY(&plr_target_dir, &plr_target_dir,
                                     c->obj.hdg + c->obj.target_yrot);
                        hack = MECHTARGETHACK;
                    } else {
                        s32 r;
                        r = c->obj.target_yrot +
                            (s32)(c->obj.pad_dx * 16384.0f * 0.02f);
                        if (r < -0x3555) {
                            s32 d = -0x3555 - r;
                            r = -0x3555;
                            c->obj.hdg -= d / 2;
                        } else if (r > 0x3555) {
                            s32 d = 0x3555 - r;
                            r = 0x3555;
                            c->obj.hdg -= d / 2;
                        }
                        c->obj.target_yrot = r;
                        r = c->obj.target_xrot -
                            (s32)(c->obj.pad_dz * 16384.0f * 0.02f);
                        if (r < -0x1555) {
                            r = -0x1555;
                        } else if (r > 0x2AAB) {
                            r = 0x2AAB;
                        }
                        c->obj.target_xrot = r;
                        plr_target_firepos =
                            *(struct nuvec_s *)&c->mtxLOCATOR[1][0]._30;
                        plr_target_pos[0] = plr_target_firepos;
                        NuVecRotateX(&plr_target_dir, &v001,
                                     -c->obj.target_xrot & 0xFFFF);
                        NuVecRotateY(&plr_target_dir, &plr_target_dir,
                                     c->obj.hdg + c->obj.target_yrot);
                        hack = BAZOOKATARGETHACK;
                    }
                    plr_target_pos[0].x -= plr_target_dir.x * hack;
                    plr_target_pos[0].y -= plr_target_dir.y * hack;
                    plr_target_pos[0].z -= plr_target_dir.z * hack;
                    plr_target_sightpos =
                        *(struct nuvec_s *)&c->mtxLOCATOR[1][1]._30;
                }
                GameRayCast(&plr_target_pos[0], &plr_target_dir, 10.0f,
                            &plr_target_pos[1]);
                NuMtxSetRotationX(&plr_target_mtx, -c->obj.target_xrot & 0xFFFF);
                NuMtxRotateY(&plr_target_mtx, c->obj.hdg + c->obj.target_yrot);
                NuVecSub(&in2, &plr_target_pos[1], &plr_target_firepos);
                NuVecNorm(&in2, &in2);
                if (NuVecDot(&in2, &plr_target_dir) <= 0.0f) {
                    c->fire_lock = 1;
                }
                if (c->fire_lock == 0 && old_found == 0 &&
                    plr_target_found != 0) {
                    GameSfx(5, &c->obj.pos);
                } else if (qrand() < 0x400) {
                    GameSfx(3, &c->obj.pos);
                }
                plr_target_frame++;
            } else {
                if (veh == 0x20) {
                    if (c->obj.pad_speed > 0.0f && c->obj.hdg == c->obj.thdg) {
                        hold = (s32)(16384.0f -
                                     NuTrigTable[c->obj.pad_angle] * 16384.0f);
                        if (c->obj.hdg == 0) {
                            if (hold > 0x4AAB) {
                                c->obj.thdg = 0x4000;
                            }
                        } else if (c->obj.hdg == 0x8000) {
                            if (hold < 0x3555) {
                                c->obj.thdg = 0x4000;
                            }
                        } else if (hold < 0xAAB) {
                            c->obj.thdg = 0;
                        } else if (hold > 0x7555) {
                            c->obj.thdg = 0x8000;
                        }
                    }
                    {
                        u16 h = c->obj.hdg;
                        u16 t = c->obj.thdg;
                        if (h < t) {
                            if (t - h < 0x369) {
                                c->obj.hdg = t;
                            } else {
                                c->obj.hdg = h + 0x369;
                            }
                        } else if (t < h) {
                            if (h - t < 0x369) {
                                c->obj.hdg = t;
                            } else {
                                c->obj.hdg = h - 0x369;
                            }
                        }
                    }
                } else if (VEHICLECONTROL == 2) {
                    s32 d;
                    if (c->obj.pad_speed > 0.0f) {
                        u32 pa = c->obj.pad_angle;
                        if (pa - 0x1555 < 0x5557) {
                            c->obj.thdg = 0;
                        } else if (((pa + 0x6AAB) & 0xFFFF) < 0x5557) {
                            c->obj.thdg = 0x8000;
                        }
                    }
                    d = RotDiff(c->obj.hdg, c->obj.thdg);
                    if (d > 0) {
                        d -= 0x10000;
                    }
                    if (d < -0x369) {
                        c->obj.hdg -= 0x369;
                    } else {
                        c->obj.hdg = c->obj.thdg;
                    }
                } else if (plr_rebound != 0) {
                    c->obj.hdg = c->obj.thdg;
                    c->obj.pad_speed = 0.0f;
                } else {
                    if (c->slide != 0) {
                        c->obj.hdg = SeekRot(c->obj.hdg, c->obj.thdg, 2);
                    } else if ((ExtraMoves != 0 ||
                                (Game.powerbits & 0x20) != 0) &&
                               c->slam != 0 && c->slam < 3 &&
                               c->obj.ground == 0) {
                        c->obj.hdg -= 0xCCC;
                    } else if (Cursor.menu == 0x21) {
                        u16 t = NuAtan2D(GameCam.pos.x - c->obj.pos.x,
                                         GameCam.pos.z - c->obj.pos.z);
                        c->obj.thdg = t;
                        c->obj.hdg = SeekRot(c->obj.hdg, t, 3);
                    } else if (c->slam_wait == 0 && c->obj.pad_speed > 0.0f &&
                               c->obj.dangle != 2 &&
                               (c->obj.dangle == 0 || c->spin == 0 ||
                                c->spin_frame < c->spin_frames -
                                    c->OnFootMoveInfo->SPINRESETFRAMES) &&
                               (c->jump == 0 || c->jump_hold == 0) &&
                               (veh != 0x44 ||
                                (*(u32 *)&c->fire_action & 0xFFFF0000) == 0)) {
                        s32 rate;
                        if (NuFabs(in.x) > 0.0f || NuFabs(in.z) > 0.0f) {
                            c->obj.thdg = NuAtan2D(in.x, in.z);
                        }
                        if ((c->slam != 0 && c->slam < 3 &&
                             c->obj.ground == 0) ||
                            ((ExtraMoves != 0 || (Game.powerbits & 4) != 0) &&
                             c->spin != 0 &&
                             c->spin_frame < c->spin_frames -
                                 c->OnFootMoveInfo->SPINRESETFRAMES) ||
                            c->target != 0) {
                            rate = 5;
                        } else if (c->crawl != 0 ||
                                   ((veh == 0x44 || veh == 0xB2) &&
                                    c->jump != 0 && c->jump_type == 2 &&
                                    c->jump_hold == 0)) {
                            rate = 4;
                        } else if (c->tiptoe != 0 || veh == 0x44 ||
                                   veh == 0xB2) {
                            rate = 3;
                        } else {
                            rate = 2;
                        }
                        c->obj.hdg = SeekRot(c->obj.hdg, c->obj.thdg, rate);
                    }
                }
            }
            c->obj.dyrot = RotDiff(hold, c->obj.hdg);
            if (veh != 0x20) {
                if (VEHICLECONTROL == 2 && c->spin != 0 &&
                    c->spin_frame < c->spin_frames -
                        c->OnFootMoveInfo->SPINRESETFRAMES) {
                    spd = minfo->SPRINTSPEED;
                } else if (plr_rebound != 0) {
                    spd = minfo->WALKSPEED;
                } else if (c->slam_wait == 0 && c->target == 0 &&
                           (c->jump == 0 || c->jump_hold == 0) &&
                           (veh != 0x44 ||
                            (*(u32 *)&c->fire_action & 0xFFFF0000) == 0)) {
                    if (c->slam != 0 && c->slam < 3 && c->obj.ground == 0) {
                        spd = spd * 0.1f;
                    } else if (c->obj.dangle != 0) {
                        if (c->obj.pad_speed == 0.0f ||
                            (c->spin != 0 &&
                             c->spin_frame < c->spin_frames -
                                 c->OnFootMoveInfo->SPINRESETFRAMES) ||
                            c->obj.dangle == 2) {
                            spd = 0.0f;
                        } else {
                            spd = minfo->DANGLESPEED;
                        }
                    } else if (c->slide != 0) {
                        spd = 0.0f;
                        if (c->obj.character != 1) {
                            spd = minfo->SLIDESPEED;
                        }
                    } else if (c->crawl != 0) {
                        if (c->obj.pad_speed > 0.0f) {
                            spd = minfo->CRAWLSPEED;
                        } else {
                            spd = 0.0f;
                        }
                    } else if (c->tiptoe != 0) {
                        if (c->obj.pad_speed > 0.0f) {
                            spd = minfo->TIPTOESPEED;
                        } else {
                            spd = 0.0f;
                        }
                    } else if (c->obj.wade != 0) {
                        if (c->obj.pad_speed > 0.0f) {
                            spd = minfo->WADESPEED;
                        } else {
                            spd = 0.0f;
                        }
                    } else if (c->sprint != 0) {
                        if (c->obj.pad_speed > 0.0f) {
                            spd = minfo->SPRINTSPEED;
                        } else {
                            spd = 0.0f;
                        }
                    }
                } else {
                    spd = 0.0f;
                }
            }
            in_speed = spd;
            in_s_friction = 0.007200000341981649f;
            in_f_friction = 0.007200000341981649f;
            if (veh == 0x20) {
                in_s_friction = 0.001440000138245523f;
                NuVecRotateX(&in2, &v001, -c->obj.xrot);
                if (c->obj.pad_speed > 0.0f) {
                    double d;
                    d = c->obj.pad_dx;
                    if (d < 0.0) {
                        d = 0.0 - d;
                    }
                    if (d < 0.33333334f) {
                        in2.z = 0.0f;
                    } else {
                        in2.z *= c->obj.pad_speed;
                    }
                    if (c->obj.pad_angle > 0x8000) {
                        in2.z = -in2.z;
                    }
                    in2.y *= c->obj.pad_speed;
                } else {
                    in2.z = 0.0f;
                    in2.y = 0.0f;
                }
                if (c->obj.mom.z > in2.z) {
                    c->obj.mom.z -= in_s_friction;
                    if (c->obj.mom.z < in2.z) {
                        c->obj.mom.z = in2.z;
                    }
                } else if (c->obj.mom.z < in2.z) {
                    c->obj.mom.z += in_s_friction;
                    if (c->obj.mom.z > in2.z) {
                        c->obj.mom.z = in2.z;
                    }
                }
                if (c->obj.mom.y > in2.y) {
                    c->obj.mom.y -= in_s_friction;
                    if (c->obj.mom.y < in2.y) {
                        c->obj.mom.y = in2.y;
                    }
                } else if (c->obj.mom.y < in2.y) {
                    c->obj.mom.y += in_s_friction;
                    if (c->obj.mom.y > in2.y) {
                        c->obj.mom.y = in2.y;
                    }
                }
                if (pos_START != 0) {
                    c->obj.mom.x = pos_START->x - c->obj.pos.x;
                } else {
                    c->obj.mom.x = 0.0f;
                }
            } else if (VEHICLECONTROL == 2) {
                if ((c->spin == 0 || c->spin_frame >= c->spin_frames) &&
                    c->tap == 0) {
                    in_s_friction = 0.0007200000691227615f;
                }
                NuVecRotateX(&in2, &v001, -c->obj.xrot);
                if (c->obj.thdg == 0x8000) {
                    in2.z = -in2.z;
                }
                if ((c->spin != 0 &&
                     c->spin_frame < c->spin_frames -
                         c->OnFootMoveInfo->SPINRESETFRAMES) || c->tap >= 5) {
                    in2.z *= minfo->SPRINTSPEED;
                    in2.y *= minfo->SPRINTSPEED;
                } else if ((c->spin != 0 &&
                            c->spin_frame >= c->spin_frames -
                                c->OnFootMoveInfo->SPINRESETFRAMES) ||
                           (u8)(c->tap - 1) < 4) {
                    in2.z *= minfo->RUNSPEED;
                    in2.y *= minfo->RUNSPEED;
                } else if (c->obj.pad_speed > 0.0f) {
                    double d;
                    d = c->obj.pad_dx;
                    if (d < 0.0) {
                        d = 0.0 - d;
                    }
                    if (d < 0.33333334f) {
                        in2.z = 0.0f;
                    } else {
                        in2.z *= c->obj.pad_speed;
                    }
                    in2.y *= c->obj.pad_speed;
                } else {
                    in2.z = 0.0f;
                    in2.y = -0.005f;
                }
                if (c->obj.mom.z > in2.z) {
                    c->obj.mom.z -= in_s_friction;
                    if (c->obj.mom.z < in2.z) {
                        c->obj.mom.z = in2.z;
                    }
                } else if (c->obj.mom.z < in2.z) {
                    c->obj.mom.z += in_s_friction;
                    if (c->obj.mom.z > in2.z) {
                        c->obj.mom.z = in2.z;
                    }
                }
                if (c->obj.mom.y > in2.y) {
                    c->obj.mom.y -= in_s_friction;
                    if (c->obj.mom.y < in2.y) {
                        c->obj.mom.y = in2.y;
                    }
                } else if (c->obj.mom.y < in2.y) {
                    c->obj.mom.y += in_s_friction;
                    if (c->obj.mom.y > in2.y) {
                        c->obj.mom.y = in2.y;
                    }
                }
                if (pos_START != 0) {
                    c->obj.mom.x = pos_START->x - c->obj.pos.x;
                } else {
                    c->obj.mom.x = 0.0f;
                }
            } else {
                if (c->slide != 0) {
                    in_f_friction = 0.0216f;
                    in_s_friction = 0.0216f;
                } else if (c->freeze != 0) {
                    f32 k;
                    if (c->obj.ground == 0) {
                        k = 0.5f;
                    } else {
                        k = 0.15f;
                    }
                    in_f_friction = k * 0.007200000341981649f;
                    in_s_friction = k * 0.007200000341981649f;
                } else if (c->obj.ground != 0) {
                    in_s_friction = TerSurface[c->obj.surface_type].friction *
                                    0.007200000341981649f;
                    in_f_friction = TerSurface[c->obj.surface_type].friction *
                                    0.007200000341981649f;
                } else if ((ExtraMoves != 0 || (Game.powerbits & 4) != 0) &&
                           c->spin != 0 &&
                           c->spin_frame < c->spin_frames -
                               c->OnFootMoveInfo->SPINRESETFRAMES) {
                    in_f_friction = 0.0024f;
                    in_s_friction = 0.0024f;
                } else if (c->jump != 0) {
                    if (c->jump_type == 0) {
                        in_s_friction = in_s_friction * 0.5f;
                        in_f_friction = in_f_friction * 0.5f;
                    } else if ((veh == 0x44 || veh == 0xB2) &&
                               c->jump_type == 2 && c->jump_hold == 0) {
                        in_s_friction = in_s_friction * 0.25f;
                        in_f_friction = in_f_friction * 0.25f;
                    }
                }
                if (veh == 0x3B && best_cRPos != 0) {
                    in_f_friction = in_f_friction * 0.1f;
                    in_s_friction = in_s_friction * 0.1f;
                    hold = NuAtan2D(in.x, in.z);
                    MoveLoopXZ(&c->obj, &hold);
                } else {
                    MoveLoopXZ(&c->obj, &c->obj.thdg);
                }
                if (c->obj.mom.y < -TERMINALVELOCITY) {
                    c->obj.mom.y = -TERMINALVELOCITY;
                } else if (c->obj.mom.y > TERMINALVELOCITY) {
                    c->obj.mom.y = TERMINALVELOCITY;
                }
            }
        }
        if (LIFTPLAYER != 0 && (pad->paddata & 0x10) != 0 &&
            c->obj.transporting == 0) {
            c->obj.gndflags.all = 0;
            c->obj.pos.y += 0.1f;
            c->obj.mom.y = 0.0f;
            c->obj.ground = 0;
        }
        if (FRAME == 0) {
            tbslotBeginFn(app_tbset, 7);
        }
        if (c->obj.transporting == 0) {
            f32 k;
            d0.x = c->obj.pos.x;
            d0.y = c->obj.bot * c->obj.SCALE + c->obj.pos.y;
            d0.z = c->obj.pos.z;
            if (c->obj.ground == 1 && c->obj.mom.x == 0.0f &&
                c->obj.mom.z == 0.0f && d0.y - c->obj.shadow > 0.1f) {
                c->obj.gndflags.all = 0;
            }
            k = 0.0036000001709908247f;
            if (c->obj.pad_speed > 0.0f) {
                k = 0.0f;
            }
            NewTerrainScaleY(&d0, &c->obj.mom, c->obj.gndflags.chrs,
                             c - Character, k, c->obj.RADIUS,
                             (c->obj.top - c->obj.bot) * c->obj.SCALE /
                                 (c->obj.RADIUS + c->obj.RADIUS));
            c->obj.pos.x = d0.x;
            c->obj.pos.y = d0.y - c->obj.bot * c->obj.SCALE;
            c->obj.pos.z = d0.z;
        } else {
            NuVecAdd(&c->obj.pos, &c->obj.pos, &c->obj.mom);
        }
        if (FRAME == 0) {
            tbslotEndFn(app_tbset, 7);
        }
        i = PlatformCrush();
        if (i != 0) {
            switch (i) {
            case 6:
                i = 0x12;
                break;
            case 7:
                i = 0x13;
                break;
            case 9:
                i = 0x9;
                break;
            case 10:
                i = 0x11;
                break;
            case 11:
            default:
                i = GetDieAnim(&c->obj, -1);
                break;
            }
            KillPlayer(&c->obj, i);
        }
        if (c->obj.mask != 0 && c->obj.mask->active > 2) {
            c->obj.mask->active--;
        }
        c->obj.transporting = 0;
        BonusTransporter(c);
        DeathTransporter(c);
        GemPathTransporter(c);
        if (c->obj.dead == 0) {
            if (c->obj.transporting == 0) {
                GetTopBot(c);
                if (veh != 0x89 && veh != 0xA1) {
                    PlayerCreatureCollisions(&c->obj);
                    if (c->obj.dangle != 0) {
                        c->slam = 3;
                    }
                    HitItems(&c->obj);
                }
            }
            if (c->obj.dead == 0) {
                if (c->obj.transporting == 0) {
                    GetTopBot(c);
                    NewTopBot(&c->obj);
                    if (Level == 1) {
                        if (PlayerObjectAnimCollision(&c->obj,
                                ObjTab[58].special, 0.4f) != 0 ||
                            PlayerObjectAnimCollision(&c->obj,
                                ObjTab[59].special, 0.4f) != 0) {
                            KillPlayer(&c->obj, 3);
                        }
                    }
                }
                if (c->obj.dead == 0 && c->obj.transporting == 0) {
                    GetTopBot(c);
                    if (veh != 0x89 && veh != 0xA1) {
                        CrateCollisions(&c->obj);
                    }
                }
            }
        }
        if (c->obj.boing != 0) {
            f32 h;
            c->jump = 1;
            c->ok_slam = 1;
            c->somersault = 0;
            c->land = 0;
            c->jump_type = (c->obj.boing & 2) ? 3 : 1;
            c->jump_frames = minfo->JUMPFRAMES1 + 1;
            c->jump_frame = 0;
            h = minfo->JUMPHEIGHT;
            if (c->jump_type == 3) {
                h *= 1.5f;
            }
            c->obj.mom.y = h / (f32)minfo->JUMPFRAMES2;
            c->obj.anim.anim_time = 1.0f;
            c->obj.ground = 0;
        } else if (temp_crate_y_ceiling_adjust != 0 && c->jump != 0) {
            c->jump = 6;
            c->jump_frame = c->jump_frames;
            c->jump_type = 4;
        }
        if (temp_crate_y_floor_adjust != 0 || temp_crate_y_ceiling_adjust != 0 ||
            temp_crate_xz_adjust != 0) {
            c->obj.pos_adjusted = 1;
        }
terrain:
        {
            f32 f = NewShadowMaskPlatRot(&c->obj.pos, 0.0f, -1);
            if (f == 2000000.0f && c->obj.transporting == 0) {
                plr_terrain_ok = 0;
                if (NOTERRAINSTOP != 0) {
                    c->obj.pos.x = c->obj.oldpos.x;
                    c->obj.pos.z = c->obj.oldpos.z;
                    c->obj.mom.z = 0.0f;
                    c->obj.dangle = 0;
                    c->obj.mom.x = 0.0f;
                    goto skip_surface;
                }
            } else {
                plr_terrain_ok = 1;
            }
            old_layer = c->obj.layer_type;
            if (c->obj.got_shadow == 0 ||
                (c->obj.transporting == 0 && f != 2000000.0f &&
                 f > c->obj.shadow)) {
                c->obj.got_shadow = 0;
                c->obj.shadow = f;
                GetSurfaceInfo(&c->obj, 2, f);
            } else {
                GetSurfaceInfo(&c->obj, 0, f);
            }
        }
skip_surface:
        c->obj.wade = 0;
        if (c->obj.layer_type != -1) {
            s32 di;
            s32 dt;
            if (c->obj.pos.y + c->obj.top * c->obj.SCALE <
                c->obj.layer_shadow) {
                if (c->obj.submerged < 0x32) {
                    c->obj.submerged++;
                } else if (Level != 0x25 && veh != 0x20 &&
                           VEHICLECONTROL != 2) {
                    KillPlayer(&c->obj, 10);
                }
            } else {
                c->obj.submerged = 0;
            }
            if ((c->obj.bot + c->obj.top) * c->obj.SCALE * 0.5f +
                    c->obj.pos.y < c->obj.layer_shadow) {
                c->obj.wade = 1;
            }
            di = 1;
            in2.x = c->obj.pos.x;
            dt = -1;
            in2.y = c->obj.layer_shadow;
            in2.z = c->obj.pos.z;
            switch (c->obj.layer_type) {
            case 1:
                av.x = (f32)qrand() * 1.5258789289873675e-06f + in2.x;
                av.y = (f32)qrand() * 1.5258789289873675e-06f + in2.y;
                av.z = (f32)qrand() * 1.5258789289873675e-06f + in2.z;
                c->obj.ddr = 0x40;
                c->obj.ddwater = 0x78;
                c->obj.ddg = 0x78;
                c->obj.ddb = -0x80;
                if ((c->obj.idle_gametime != 0.0f) ? qrand() < 0x1000
                                                   : qrand() <= 0x7FFF) {
                    NuRndrAddWaterRipple(&av, 0.2f, 0.4f, 0x20, 0x60706050);
                }
                break;
            case 2:
            case 3:
                dt = 1;
                break;
            case 4:
                if (c->obj.idle_gametime == 0.0f) {
                    dt = 1;
                    di = 2;
                    in2.y += 0.03f;
                }
                break;
            }
            if (Paused == 0 && dt >= 0 && jonframe1 % di == 0) {
                AddVariableShotDebrisEffect(GDeb[dt].i, &in2, 1, 0, 0);
            }
        } else {
            c->obj.submerged = 0;
        }
        if (c->obj.surface_type > 0) {
            s32 di;
            s32 dt;
            di = 1;
            in2.x = c->obj.pos.x;
            dt = -1;
            in2.y = c->obj.pos.y;
            in2.z = c->obj.pos.z;
            switch (c->obj.surface_type) {
            case 1:
            case 12:
            case 13:
                if (c->obj.idle_gametime == 0.0f) {
                    dt = 2;
                    di = 2;
                }
                break;
            case 2:
                if (c->obj.idle_gametime == 0.0f) {
                    dt = 1;
                    di = 2;
                    in2.y += 0.03f;
                }
                break;
            case 8:
                if (c->obj.idle_gametime == 0.0f && Level != 3) {
                    dt = 4;
                    di = 4;
                    in2.y += 0.03f;
                }
                break;
            }
            if (Paused == 0 && dt >= 0 && c->obj.ground != 0 &&
                jonframe1 % di == 0) {
                AddVariableShotDebrisEffect(GDeb[dt].i, &in2, 1, 0, 0);
            }
        }
        if (c->obj.roof_type != -1 &&
            (TerSurface[c->obj.roof_type].flags & 0x10) != 0 &&
            VEHICLECONTROL != 1) {
            f32 gap = c->obj.roof_y - minfo->DANGLEGAP;
            if (c->obj.pos.y + c->obj.top * c->obj.SCALE >= gap ||
                old_dangle != 0) {
                c->obj.dangle = 1;
                c->obj.pos.y = gap - c->obj.max.y * c->obj.SCALE;
                if (old_dangle == 0) {
                    GameSfx(0x1B, &c->obj.pos);
                }
            }
        } else {
            c->obj.dangle = 0;
            if (old_dangle != 0) {
                c->jump = 6;
                c->obj.mom.y = 0.0f;
                c->jump_frame = c->jump_frames;
                c->jump_type = 4;
                GameSfx(0x1C, &c->obj.pos);
            }
        }
        if (c->obj.layer_type == 1 && old_layer == -1) {
            GameSfx(0x47, &c->obj.pos);
        } else if (c->obj.layer_type == -1 && old_layer == 1) {
            GameSfx(0x47, &c->obj.pos);
        }
        TerrainFailsafe(&c->obj);
        {
            s32 onground;
            if (c->obj.got_shadow != 0) {
                onground = c->obj.pos.y + c->obj.bot * c->obj.SCALE <=
                           c->obj.shadow;
            } else {
                onground = (c->obj.pos.y + c->obj.bot * c->obj.SCALE) -
                               c->obj.shadow < 0.025f;
            }
            c->obj.ground = 0;
            if (c->obj.gndflags.chrs[1] != 0) {
                c->obj.ground = 1;
            }
            if (onground) {
                c->obj.ground |= 2;
            }
        }
        if (c->obj.ground != 0) {
            c->obj.last_ground = c->obj.ground;
        }
        if ((c->obj.ground & 2) != 0 && c->obj.dead == 0) {
            if ((TerSurface[c->obj.surface_type].flags & 1) != 0 &&
                c->obj.invincible == 0 && veh != 0x20 && VEHICLECONTROL != 2) {
                s32 anim;
                if (VEHICLECONTROL == 1 && c->obj.vehicle != -1) {
                    anim = 0xB;
                } else {
                    if (c->obj.character == 0 && c->obj.layer_type != -1 &&
                        CRemap[79] != -1 && Level == 7) {
                        anim = 8;
                    } else if (c->obj.character == 0 &&
                               c->obj.surface_type == 7 && CRemap[151] != -1) {
                        anim = 0xD;
                    } else {
                        anim = 5;
                        if (c->obj.layer_type != -1 && c->obj.submerged != 0) {
                            anim = 0xA;
                        }
                    }
                }
                if (c->obj.layer_type != -1 && c->obj.submerged == 0 &&
                    Level == 2 && Bonus == 0) {
                    if (c->obj.mask != 0 && c->obj.mask->active != 0 &&
                        (LDATA->flags & 0xE00) == 0) {
                        if (c->obj.mask->active < 3 &&
                            c->obj.invincible == 0) {
                            LoseMask(&c->obj);
                        }
                    } else {
                        KillGameObject(&c->obj, anim);
                    }
                } else {
                    c->obj.invincible = 0;
                    KillGameObject(&c->obj, anim);
                    if (c->obj.mask != 0 && c->obj.mask->active != 0 &&
                        (LDATA->flags & 0xE00) == 0) {
                        AddMaskFeathers(c->obj.mask);
                        c->obj.mask->active = 0;
                    }
                }
            } else if ((TerSurface[c->obj.surface_type].flags & 2) != 0) {
                c->obj.boing |= 2;
                GameSfx(2, &c->obj.pos);
                NewRumble(&player->rumble, 0x7F);
                NewBuzz(&player->rumble, 0xA);
            }
        }
        ObjectRotation(&c->obj, 2, veh == -1);
        if (c->obj.transporting == 0 && veh != 0x89 && veh != 0xA1) {
            WumpaCollisions(&c->obj);
        }
        plr_allow_jump = c->allow_jump;
        if (c->obj.ground != 0) {
            c->allow_jump = 0xA;
        } else if (c->allow_jump != 0) {
            c->allow_jump--;
        }
        if (c->obj.boing != 0) {
            f32 h;
            c->ok_slam = 1;
            c->jump = 1;
            c->somersault = 0;
            c->land = 0;
            if ((c->obj.boing & 2) != 0) {
                c->jump_type = 3;
            } else {
                c->jump_type = 1;
            }
            c->jump_frame = 0;
            c->jump_frames = minfo->JUMPFRAMES1 + 1;
            h = minfo->JUMPHEIGHT;
            if (c->jump_type == 3) {
                h *= 1.5f;
            }
            c->obj.mom.y = h / (f32)minfo->JUMPFRAMES2;
            c->slam = 3;
            c->slam_wait = 0;
            c->obj.anim.anim_time = 1.0f;
        }
        if (c->fire != 0) {
            c->fire--;
        }
        if (c->tap != 0) {
            c->tap--;
        }
move_dispatch:
        if (veh == 0x20) {
            MoveSUBMARINE(c, pad);
        } else if (veh == 0x6B) {
            MoveSCOOTER(c, pad);
        } else if (veh == 0xA0) {
            MoveSNOWBOARD(c, pad);
        } else if (veh == 0x44) {
            MoveMECH(c, pad);
        } else if (veh == 0xB2) {
            MoveFIREENGINE(c, pad);
        } else if (veh == 0x3B) {
            MoveGYRO(c, pad);
        } else if (veh == 0x89) {
            MoveMINECART(c, pad);
        } else if (veh == 0xA1) {
            MoveMINETUB(c, pad);
        } else if (veh == 0x99) {
            MoveOFFROADER(c, pad);
        } else if (c->obj.character == 1) {
            MoveCOCO(c, pad);
        } else if (VEHICLECONTROL == 2) {
            MoveSWIMMING(c, pad);
        } else {
            MoveCRASH(c, pad);
        }
        c->obj.boing = 0;
    }
post_move:
    if (c->obj.dead != 0 && InvincibilityCHEAT != 0) {
        c->obj.dead = 0;
        plr_lives.count = Game.lives;
    }
    if (Adventure != 0) {
        Game.lives = (u8)plr_lives.count;
        Game.wumpas = (u8)plr_wumpas.count;
        if (c->obj.mask != 0) {
            Game.mask = (c->obj.mask->active < 3) ? (u8)c->obj.mask->active : 2;
        }
    }
    BonusTiming(c);
    {
        f32 dx;
        f32 dy;
        f32 xx;
        f32 zz;
        dx = c->obj.pos.x - c->obj.oldpos.x;
        xx = dx * dx;
        dx = c->obj.pos.z - c->obj.oldpos.z;
        zz = dx * dx;
        dy = c->obj.pos.y - c->obj.oldpos.y;
        c->obj.xz_distance = NuFsqrt(xx + zz);
        c->obj.xyz_distance = NuFsqrt(dy * dy + xx + zz);
    }
    c->obj.anim.oldaction = c->obj.anim.action;
    if (vtog_time < vtog_duration && vtog_blend != 0) {
        AnimateDIVE(c, vtog_time / vtog_duration);
    } else if (VEHICLECONTROL == 1 && c->obj.vehicle == 0x53) {
        AnimateATLASPHERE(c);
    } else if (VEHICLECONTROL == 1 && c->obj.vehicle == 0x20) {
        AnimateSUBMARINE(c);
    } else if (VEHICLECONTROL == 1 && c->obj.vehicle == 0x6B) {
        AnimateSCOOTER(c);
    } else if (VEHICLECONTROL == 1 && c->obj.vehicle == 0xA0) {
        AnimateSNOWBOARD(c);
    } else if (VEHICLECONTROL == 1 && c->obj.vehicle == 0x36) {
        AnimateGLIDER(c);
    } else if (VEHICLECONTROL == 1 && c->obj.vehicle == 0x81) {
        AnimateDROPSHIP(c);
    } else if (VEHICLECONTROL == 1 && c->obj.vehicle == 0x3B) {
        AnimateGYRO(c, pad);
    } else if (VEHICLECONTROL == 1 && c->obj.vehicle == 0x44) {
        AnimateMECH(c);
    } else if (VEHICLECONTROL == 1 && c->obj.vehicle == 0xB2) {
        AnimateFIREENGINE(c);
    } else if (VEHICLECONTROL == 1 && c->obj.vehicle == 0x63) {
        AnimateJEEP(c);
    } else if (VEHICLECONTROL == 1 && c->obj.vehicle == 0x89) {
        AnimateMINECART(c);
    } else if (VEHICLECONTROL == 1 && c->obj.vehicle == 0xA1) {
        AnimateMINETUB(c);
    } else if (VEHICLECONTROL == 1 && c->obj.vehicle == 0x8B) {
        AnimateMOSQUITO(c);
    } else if (VEHICLECONTROL == 1 && c->obj.vehicle == 0x99) {
        AnimateOFFROADER(c);
    } else if (VEHICLECONTROL == 2) {
        AnimateSWIMMING(c);
    } else if (c->obj.character == 1) {
        AnimateCOCO(c);
    } else {
        AnimateCRASH(c);
    }
    {
        struct CharacterModel *model;
        if (c->obj.dead != 0) {
            model = &CModel[c->obj.die_model[0]];
        } else if (VEHICLECONTROL == 2 && CRemap[115] != -1) {
            model = &CModel[CRemap[115]];
        } else {
            model = c->obj.model;
        }
        if (c->obj.dead != 0x16 && c->obj.dead != 4) {
            UpdateAnimPacket(model, &c->obj.anim, 0.59999996f,
                             c->obj.xz_distance);
        }
    }
    c->obj.frame++;
    if (Cursor.menu == -1 && Level == 0x25) {
        if (c->obj.ground != 0 && c->obj.idle_gametime > 0.0f &&
            NuVecDistSqr(&c->obj.pos, &loadsavepos, 0) < 1.0f) {
            if (pad != 0 && (pad->buttons & 0x40) != 0) {
                loadsave_frame = 0x32;
            } else {
                loadsave_frame++;
            }
            if (loadsave_frame > 0x32) {
                loadsave_frame = 0x33;
            }
        } else {
            loadsave_frame = 0;
        }
        if (loadsave_frame == 0x32) {
            NewMenu(&Cursor, 0x13, 3, -1);
            loadsave_frame = 0x33;
            GameSfx(0x36, 0);
        }
    }
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
                    ShadRndr(mS, model->shaddata[98], 1.0f, scale);
                } else if (anim->blend != 0) {
                    if (((u16)anim->blend_dst_action < 0x76)
                        && (model->anmdata[anim->blend_dst_action] != 0)) {
                        ShadRndr(mS, model->shaddata[anim->blend_dst_action],
                                 anim->blend_dst_time, scale);
                    } else {
                        ShadRndr(mS, model->shaddata, 1.0f, scale);
                    }
                } else if (((u16)anim->action < 0x76)
                           && (model->anmdata[anim->action] != 0)) {
                    ShadRndr(mS, model->shaddata[anim->action],
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


/* --- DrawCreatures externals ------------------------------------- */

/* (tersurface_s / objtab_s / nuspecial_s are defined above MovePlayer.) */

extern s32 DRAWCREATURESHADOWS;
extern s32 editor_active;
extern s32 in_finish_range;
extern s32 warp_level;
extern s32 glass_phase;
extern f32 glass_mix;
extern s32 SKELETALCRASH;
extern f32 HUBREFLECTY;
extern f32 ATLASPLAYERLIFT;
extern f32 vtog_time;
extern f32 vtog_duration;
extern unsigned short temp_surface_xrot;
extern unsigned short temp_surface_yrot;
extern unsigned short temp_surface_zrot;
extern struct gamecam_s *pCam;
extern struct numtx_s mTEMP;
extern f32 NuTrigTable[];
extern struct MoveInfo GyroMoveInfo;
extern struct tersurface_s TerSurface[];
extern struct objtab_s ObjTab[201];

void SetCreatureLights(struct creature_s *c);
void SetLevelLights(void);
void DrawGlider(struct creature_s *c);
void DrawAtlas(struct creature_s *c);
struct numtx_s *DrawPlayerJeep(struct creature_s *c);
s32 Draw3DObject(s32 object, struct nuvec_s *pos, u16 xrot, u16 yrot, u16 zrot,
                 float scalex, float scaley, float scalez,
                 struct nugscn_s *scn, struct nuspecial_s *obj, s32 rot);
void Draw3DCrateCount(struct nuvec_s *pos, u16 angle);
void ScaleFlatShadow(struct nuvec_s *s, f32 y, f32 shadow, f32 scale);
void NuMtxSetScale(struct numtx_s *m, struct nuvec_s *s);
void NuMtxSetRotationY(struct numtx_s *m, s32 r);
void NuMtxRotateX(struct numtx_s *m, s32 r);
void NuMtxRotateY(struct numtx_s *m, s32 r);
void NuMtxRotateZ(struct numtx_s *m, s32 r);
void NuMtxTranslate(struct numtx_s *m, struct nuvec_s *v);
void NuMtxPreScale(struct numtx_s *m, struct nuvec_s *s);
s32 NuAtan2D(f32 x, f32 z);
s32 NuRndrGScnObj(void *gobj, struct numtx_s *m);
void NuHGobjRndrMtx(struct NUHGOBJ_s *hobj, struct numtx_s *m, s32 nlayers,
                    short *layer, struct numtx_s *tmtx);
void NuHGobjRndr(struct NUHGOBJ_s *hobj, struct numtx_s *m, s32 nlayers,
                 short *layer);


/* GNU89 `inline`: inlined into DrawCreatures; the standalone copy is
 * emitted deferred at the end of the unit (retail 0x001D5918). */
inline void StoreLocatorMatrices(struct CharacterModel *model,
                                 struct numtx_s *mC, struct numtx_s *tmtx,
                                 struct numtx_s *mtx, struct nuvec_s *mom)
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


void DrawCreatures(struct creature_s *c, s32 count, s32 render, s32 shadow)
{
    struct nuvec_s s;
    struct numtx_s mV;
    struct numtx_s mC;
    struct numtx_s mS;
    struct numtx_s mR;
    struct CharacterModel *model[2];
    struct numtx_s tmtx[256];
    struct numtx_s *pm;
    struct numtx_s *jm;
    s32 i;
    s32 j;
    s32 vflag;
    s32 shflag;
    s32 reflect;
    s32 old_frame;
    s32 VEHICLE;
    s32 o;
    s32 bVar9;
    u32 anim_action;
    f32 dx;
    f32 r2;
    f32 y;
    u16 yrot;
    u16 a;

    if ((DRAWCREATURESHADOWS == 0) || (Level == 0x1D) || (Level == 0x24) ||
        ((Level == 0x1E) && (level_part_2 != 0)) || (Level == 0x1A) ||
        ((LDATA->flags & 0x1000) != 0) || (VEHICLECONTROL == 2) ||
        ((VEHICLECONTROL == 1) && (player->obj.vehicle == 0x20))) {
        shadow = 0;
    }

    if (((LDATA->flags & 0x202) != 0) || (Level == 0x1C)) {
        r2 = (s32)LDATA->farclip;
    } else {
        r2 = AIVISRANGE;
    }
    if ((f32)LDATA->farclip < r2) {
        r2 = (f32)LDATA->farclip;
    }
    r2 = r2 * r2;

    for (i = 0; i < count; i++, c++) {
        vflag = c->obj.flags & 1;
        if (vflag == 0) {
            goto novehicle;
        }
        if (in_finish_range == 0x32) {
            goto next;
        }
        if (c->obj.finished != 0) {
            goto next;
        }
        if ((Level == 0x25) && (warp_level != -1)) {
            goto next;
        }
        if ((VEHICLECONTROL == 1) && (c->obj.vehicle != -1)) {
            VEHICLE = c->obj.vehicle;
        } else {
        novehicle:
            VEHICLE = -1;
        }

        old_frame = c->obj.draw_frame;
        c->obj.draw_frame = 0;
        if (c->used == 0) {
            goto next;
        }
        if (c->on == 0) {
            goto next;
        }
        if (c->obj.model == 0) {
            goto next;
        }
        if (c->obj.dead == 0x16) {
            goto next;
        }
        if (c->obj.dead == 4) {
            goto next;
        }
        if (c->obj.dead == 7) {
            goto next;
        }
        if (Level == 0x17) {
            if (glass_phase == 0) {
                if (c->obj.character == 0x7F) {
                    goto next;
                }
            } else {
                if (c->obj.character != 0x7F) {
                    goto next;
                }
            }
        }
        if (c->obj.invisible != 0) {
            if (c->obj.character != 0x77) {
                goto next;
            }
        }
        if (c->obj.invincible != 0) {
            if ((c->obj.invincible & 3) < 2) {
                goto next;
            }
        }

        bVar9 = 0;
        if (editor_active == 0) {
            dx = (pCam->pos.x - c->obj.pos.x) * (pCam->pos.x - c->obj.pos.x) +
                 (pCam->pos.z - c->obj.pos.z) * (pCam->pos.z - c->obj.pos.z);
            if ((LDATA->flags & 0x200) == 0) {
                if (r2 < dx) {
                    goto next;
                }
            }
        }

        shflag = 0;
        reflect = 0;
        if ((VEHICLE == 0x63) || (VEHICLE == 0x36) || (VEHICLE == 0x81) ||
            (VEHICLE == 0x53) || (VEHICLE == 0x8B)) {
            if (render == 0) {
                goto next;
            }
            SetCreatureLights(c);
            switch (VEHICLE) {
            case 0x81:
            case 0x8B:
            case 0x36:
                DrawGlider(c);
                c->obj.draw_frame = old_frame + 1;
                mV = mTEMP;
                bVar9 = 1;
                break;
            case 0x53:
                DrawAtlas(c);
                bVar9 = 1;
                c->obj.draw_frame = old_frame + 1;
                NuMtxSetRotationY(&mV, c->obj.hdg);
                NuMtxTranslate(&mV, &c->obj.pos);
                mV._31 = mV._31 + ATLASPLAYERLIFT;
                break;
            case 0x63:
                jm = DrawPlayerJeep(c);
                c->obj.draw_frame = old_frame + 1;
                if (jm == 0) {
                    goto next;
                }
                mV = *jm;
                bVar9 = 1;
                break;
            }
        }

        if ((vflag != 0) && (VEHICLECONTROL == 2) && (c->obj.dead == 0) &&
            (c->spin != 0) &&
            (c->spin_frame <
             c->spin_frames - c->OnFootMoveInfo->SPINRESETFRAMES) &&
            (bVar9 == 0)) {
            if (CRemap[116] != -1) {
                yrot = c->obj.hdg - 0x8000;
                s.x = s.y = s.z = c->obj.SCALE;
                pm = &mC;
                NuMtxSetScale(&mC, &s);
                NuMtxRotateZ(&mC, c->spin_frame * 0x1999);
                NuMtxRotateX(&mC, c->obj.xrot);
                NuMtxRotateY(&mC, yrot);
                NuMtxTranslate(&mC, &c->obj.pos);
                model[0] = &CModel[CRemap[116]];
                if (model[0]->anmdata[0x46] != 0) {
                    NuHGobjEvalAnim(model[0]->hobj, model[0]->anmdata[0x46],
                                    (model[0]->anmdata[0x46]->time - 1.0f) *
                                    ((f32)c->spin_frame /
                                     (f32)(c->OnFootMoveInfo->SPINFRAMES +
                                           c->OnFootMoveInfo->SUPERSPINFRAMES * 3))
                                    + 1.0f,
                                    0, 0, tmtx);
                } else {
                    NuHGobjEval(model[0]->hobj, 0, 0, tmtx);
                }
                if (glass_draw == 0) {
                    StoreLocatorMatrices(model[0], &mC, tmtx,
                                         &c->mtxLOCATOR[0][0],
                                         &c->momLOCATOR[0][0]);
                }
                if (render == 0) {
                    goto next;
                }
                SetCreatureLights(c);
                NuHGobjRndrMtx(model[0]->hobj, pm, 1, 0, tmtx);
                if ((c->obj.reflect_y != 2000000.0f) &&
                    (glass_mix == 0.0f) && (glass_draw == 0)) {
                    mR = mC;
                    mR._01 = -mR._01;
                    mR._11 = -mR._11;
                    mR._21 = -mR._21;
                    mR._31 = c->obj.reflect_y -
                             (mR._31 - c->obj.reflect_y);
                    NuHGobjRndrMtx(model[0]->hobj, &mR, 1, 0, tmtx);
                }
                if ((shadow != 0) && (c->obj.shadow != 2000000.0f) &&
                    (dx < r2) &&
                    ((TerSurface[c->obj.surface_type].flags & 1) == 0) &&
                    (SKELETALCRASH == 0) && (c->freeze == 0) &&
                    (glass_draw == 0)) {
                    ScaleFlatShadow(&s, c->obj.pos.y, c->obj.shadow,
                                    c->obj.SCALE);
                    NuMtxSetScale(&mS, &s);
                    NuMtxRotateY(&mS, yrot);
                    NuMtxRotateZ(&mS, c->obj.surface_zrot);
                    NuMtxRotateX(&mS, c->obj.surface_xrot);
                    mS._30 = c->obj.pos.x;
                    mS._31 = c->obj.shadow + 0.025f;
                    mS._32 = c->obj.pos.z;
                    if (model[0]->shaddata != 0) {
                        ShadRndr(&mS, model[0]->shaddata, 1.0f,
                                 CData[model[0]->character].shadow_scale);
                    }
                }
            }
            c->obj.draw_frame = old_frame + 1;
            goto next;
        } else if ((vflag != 0) && (c->obj.character == 0) &&
                   (c->obj.dead == 0) && (c->spin != 0) &&
                   (c->spin_frame <
                    c->spin_frames - c->OnFootMoveInfo->SPINRESETFRAMES) &&
                   (bVar9 == 0) && (VEHICLE == -1) && (c->freeze == 0)) {
            if (render != 0) {
                SetCreatureLights(c);
            }
            if (CRemap[8] != -1) {
                yrot = -(c->spin_frame * 0x1999);
                s.x = s.y = s.z = c->obj.SCALE;
                pm = &mC;
                NuMtxSetScale(&mC, &s);
                NuMtxRotateY(&mC, yrot);
                NuMtxRotateZ(&mC, c->obj.zrot);
                NuMtxRotateX(&mC, c->obj.xrot);
                NuMtxTranslate(&mC, &c->obj.pos);
                if ((SKELETALCRASH != 0) && (CRemap[159] != -1)) {
                    model[0] = &CModel[CRemap[159]];
                } else {
                    model[0] = &CModel[CRemap[8]];
                }
                if (c->obj.dangle != 0) {
                    anim_action = 0x47;
                } else {
                    anim_action = 0x46;
                }
                if ((anim_action < 0x76) &&
                    (model[0]->anmdata[anim_action] != 0)) {
                    NuHGobjEvalAnim(model[0]->hobj,
                                    model[0]->anmdata[anim_action],
                                    (model[0]->anmdata[anim_action]->time - 1.0f) *
                                    ((f32)c->spin_frame /
                                     (f32)(c->OnFootMoveInfo->SPINFRAMES +
                                           c->OnFootMoveInfo->SUPERSPINFRAMES * 3))
                                    + 1.0f,
                                    0, 0, tmtx);
                } else {
                    NuHGobjEval(model[0]->hobj, 0, 0, tmtx);
                }
                if (glass_draw == 0) {
                    StoreLocatorMatrices(model[0], &mC, tmtx,
                                         &c->mtxLOCATOR[0][0],
                                         &c->momLOCATOR[0][0]);
                }
                if (render == 0) {
                    goto next;
                }
                NuHGobjRndrMtx(model[0]->hobj, pm, 1, 0, tmtx);
                if ((c->obj.reflect_y != 2000000.0f) &&
                    (glass_mix == 0.0f) && (glass_draw == 0)) {
                    mR = mC;
                    mR._01 = -mR._01;
                    mR._11 = -mR._11;
                    mR._21 = -mR._21;
                    if (Level != 0x25) {
                        y = c->obj.reflect_y;
                    } else {
                        y = HUBREFLECTY;
                    }
                    mR._31 = y - (mR._31 - y);
                    NuHGobjRndrMtx(model[0]->hobj, &mR, 1, 0, tmtx);
                }
                if ((shadow != 0) && (c->obj.shadow != 2000000.0f) &&
                    (dx < r2) &&
                    ((TerSurface[c->obj.surface_type].flags & 1) == 0) &&
                    (SKELETALCRASH == 0) && (c->freeze == 0) &&
                    (glass_draw == 0)) {
                    ScaleFlatShadow(&s, c->obj.pos.y, c->obj.shadow,
                                    c->obj.SCALE);
                    NuMtxSetScale(&mS, &s);
                    NuMtxRotateY(&mS, yrot);
                    NuMtxRotateZ(&mS, c->obj.surface_zrot);
                    NuMtxRotateX(&mS, c->obj.surface_xrot);
                    mS._30 = c->obj.pos.x;
                    mS._31 = c->obj.shadow + 0.025f;
                    mS._32 = c->obj.pos.z;
                    if (model[0]->shaddata != 0) {
                        ShadRndr(&mS, model[0]->shaddata, 1.0f,
                                 CData[model[0]->character].shadow_scale);
                    }
                }
            }
            if (render == 0) {
                goto next;
            }
            if ((c->obj.dangle != 0) && (CRemap[9] != -1)) {
                yrot = c->obj.hdg - 0x8000;
                s.x = s.y = s.z = c->obj.SCALE;
                NuMtxSetScale(&mC, &s);
                NuMtxRotateY(&mC, yrot);
                NuMtxRotateZ(&mC, c->obj.zrot);
                NuMtxRotateX(&mC, c->obj.xrot);
                NuMtxTranslate(&mC, &c->obj.pos);
                model[1] = &CModel[CRemap[9]];
                NuHGobjRndr(model[1]->hobj, &mC, 1, 0);
                if ((c->obj.reflect_y != 2000000.0f) &&
                    (glass_mix == 0.0f) && (glass_draw == 0)) {
                    mR = mC;
                    mR._01 = -mR._01;
                    mR._11 = -mR._11;
                    mR._21 = -mR._21;
                    mR._31 = c->obj.reflect_y -
                             (mR._31 - c->obj.reflect_y);
                    NuHGobjRndr(model[1]->hobj, &mR, 1, 0);
                }
            }
            c->obj.draw_frame = old_frame + 1;
            goto next;
        }

        if ((c->obj.character == 0x77) && (c->obj.invisible != 0)) {
            if (render != 0) {
                Draw3DCrateCount(&c->obj.pos, c->obj.hdg);
                c->obj.draw_frame = old_frame + 1;
            }
            goto next;
        }

        if (render != 0) {
            if (c->obj.character == 0x75) {
                o = 0x84;
            } else if (c->obj.character == 0x77) {
                o = 0x88;
            } else if (c->obj.character == 0x78) {
                o = 0x89;
            } else if (c->obj.character == 0x79) {
                o = 0x8A;
            } else if (c->obj.character == 0x7A) {
                o = 0x8B;
            } else if (c->obj.character == 0x7B) {
                o = 0x8C;
            } else if (c->obj.character == 0x7C) {
                o = 0x8D;
            } else {
                o = (c->obj.character == 0x7D) ? 0x8E : -1;
            }
            if (o != -1) {
                Draw3DObject(o, &c->obj.pos, 0,
                             NuAtan2D(c->obj.pos.x - GameCam.pos.x,
                                      c->obj.pos.z - GameCam.pos.z),
                             0, 1.0f, 1.0f, 1.0f,
                             ObjTab[o].scene, ObjTab[o].special, 0);
            }
        }

        yrot = c->obj.hdg + 0x8000;
        if (vflag != 0) {
            if ((c->obj.dead == 3) || (c->obj.dead == 8)) {
                yrot += 0x8000;
            } else if (c->freeze != 0) {
                yrot = GameCam.hdg_to_player;
            } else {
                if ((c->spin != 0) &&
                    (c->spin_frame <
                     c->spin_frames - c->OnFootMoveInfo->SPINRESETFRAMES) &&
                    (VEHICLE == 0x3B) && (c->obj.anim.newaction == 0x69) &&
                    (c->obj.model->anmdata[0x69] != 0)) {
                    yrot -= (c->spin_frame << 16) / GyroMoveInfo.SPINFRAMES;
                }
            }
        }

        if (bVar9 != 0) {
            mC = mV;
            c->m = mC;
        } else if (((vflag == 0) && (c->obj.vehicle == 0xA1)) ||
                   (VEHICLECONTROL == 2) || (VEHICLE == 0x20) ||
                   (VEHICLE == 0x89) || (VEHICLE == 0xA1)) {
            s.x = s.y = s.z = c->obj.SCALE;
            pm = &mC;
            NuMtxSetScale(&mC, &s);
            NuMtxRotateZ(&mC, c->obj.zrot);
            if (VEHICLE == 0x20) {
                o = RotDiff(0, c->obj.xrot) / 4;
            } else {
                o = c->obj.xrot;
            }
            NuMtxRotateX(&mC, o);
            NuMtxRotateY(&mC, yrot);
            NuMtxTranslate(&mC, &c->obj.pos);
            c->m = mC;
        } else {
            if ((Level == 0x17) && (c->obj.character == 0x7F)) {
                a = ((GameTimer.frame % 0x1E) * 0x10000) / 0x1E;
                s.x = NuTrigTable[a] * 0.05f + 1.0f;
                s.y = NuTrigTable[(u16)(a + 0x4000)] * 0.05f + 1.0f;
                s.z = NuTrigTable[a] * 0.05f + 1.0f;
                s.x *= c->obj.SCALE;
                if (s.x < 0.0f) {
                    s.x = 0.0f;
                }
                s.y *= c->obj.SCALE;
                if (s.y < 0.0f) {
                    s.y = 0.0f;
                }
                s.z *= c->obj.SCALE;
                if (s.z < 0.0f) {
                    s.z = 0.0f;
                }
            } else {
                s.x = s.y = s.z = c->obj.SCALE;
            }
            if ((c->obj.flags & 0x10000) != 0) {
                s.y = s.y * -1.0;
            }
            NuMtxSetScale(&mC, &s);
            NuMtxRotateY(&mC, yrot);
            NuMtxRotateZ(&mC, c->obj.zrot);
            NuMtxRotateX(&mC, c->obj.xrot);
            NuMtxTranslate(&mC, &c->obj.pos);
            c->m = mC;
        }

        if (render != 0) {
            if ((c->obj.reflect_y != 2000000.0f) &&
                ((c != player) || (c->obj.dead != 2)) &&
                (glass_mix == 0.0f) && (glass_draw == 0)) {
                mR = mC;
                mR._01 = -mR._01;
                mR._11 = -mR._11;
                mR._21 = -mR._21;
                if (Level != 0x25) {
                    y = c->obj.reflect_y;
                } else {
                    y = HUBREFLECTY;
                }
                reflect = 1;
                mR._31 = y - (mR._31 - y);
            }
            if ((render != 0) && (shadow != 0) &&
                (c->obj.shadow != 2000000.0f) && (dx < r2) &&
                ((TerSurface[c->obj.surface_type].flags & 1) == 0) &&
                (c->freeze == 0) && (glass_draw == 0) &&
                ((c->obj.flags & 0x4000) == 0) && (bVar9 == 0) &&
                (c->obj.dead != 8) &&
                ((vflag == 0) || (SKELETALCRASH == 0)) &&
                (VEHICLE != 0xA1) && (VEHICLE != 0x89) && (VEHICLE != 0x63) &&
                ((vflag != 0) || (c->obj.vehicle != 0xA1))) {
                ScaleFlatShadow(&s, c->obj.pos.y, c->obj.shadow,
                                c->obj.SCALE);
                NuMtxSetScale(&mS, &s);
                NuMtxRotateY(&mS, yrot);
                NuMtxRotateZ(&mS, c->obj.surface_zrot);
                NuMtxRotateX(&mS, c->obj.surface_xrot);
                mS._30 = c->obj.pos.x;
                if (c->obj.dead == 1) {
                    mS._31 = c->obj.pos.y +
                             (c->obj.shadow - c->obj.oldpos.y);
                }
                mS._31 = c->obj.shadow + 0.025f;
                mS._32 = c->obj.pos.z;
                if (VEHICLE == 0x99) {
                    if (Level == 3) {
                        mS._31 = mS._31 + 0.05f;
                    } else {
                        mS._31 = mS._31 + 0.025f;
                    }
                }
                shflag = 1;
                temp_surface_xrot = c->obj.surface_xrot;
                temp_surface_yrot = c->obj.hdg;
                temp_surface_zrot = c->obj.surface_zrot;
            }
        }

        model[0] = c->obj.model;
        if ((c->obj.dead != 0) && (c->obj.die_model[0] != -1) &&
            (c->obj.die_model[0] != CRemap[c->obj.character])) {
            model[0] = &CModel[c->obj.die_model[0]];
        } else if ((vflag != 0) && (VEHICLECONTROL == 2)) {
            model[0] = &CModel[CRemap[115]];
        }

        if ((vflag != 0) &&
            ((model[0]->character == 0) || (model[0]->character == 0x73)) &&
            (CRemap[84] != -1) &&
            ((SKELETALCRASH != 0) ||
             ((c->obj.dead == 0x11) && (GameTimer.frame % 0xA < 5)))) {
            model[0] = &CModel[CRemap[84]];
        }

        model[1] = 0;
        if ((c->obj.dead != 0) && (c->obj.die_model[1] != -1) &&
            (c->obj.die_model[1] != CRemap[c->obj.character]) &&
            (c->obj.die_model[1] != c->obj.die_model[0])) {
            model[1] = &CModel[c->obj.die_model[1]];
        } else {
            o = -1;
            if (c->obj.character == 0x11) {
                o = 0x12;
            } else if ((c->obj.character == 0x24) &&
                       (c->obj.anim.newaction == 0)) {
                o = 0x87;
            } else if (c->obj.character == 0x6D) {
                o = 0x3D;
            } else if ((c->obj.dead == 0) && (c->obj.character == 0) &&
                       ((c->obj.anim.newaction == 0x25) ||
                        (c->obj.anim.newaction == 0x26))) {
                o = 0x45;
            } else if ((c->freeze != 0) && (c->obj.dead == 0)) {
                o = 0x4F;
            } else if ((vflag != 0) && (c->obj.dead == 0) &&
                       (c->target != 0) && (c->obj.character == 0) &&
                       (VEHICLE == -1)) {
                o = 0x8C;
            } else if ((VEHICLE != -1) && (vtog_time == vtog_duration)) {
                if (VEHICLE == 0x6B) {
                    o = 0x6B;
                } else if (VEHICLE == 0xA0) {
                    o = 0xA0;
                } else if (VEHICLE == 0x44) {
                    o = 0x44;
                } else if (VEHICLE == 0xB2) {
                    o = 0xB2;
                } else if (VEHICLE == 0x3B) {
                    o = 0x3B;
                } else if (VEHICLE == 0x20) {
                    o = 0x20;
                } else if (VEHICLE == 0x89) {
                    o = 0x89;
                } else if (VEHICLE == 0xA1) {
                    o = 0xA1;
                } else if (VEHICLE == 0x99) {
                    o = 0x99;
                }
            } else {
                if ((vflag == 0) && (c->obj.vehicle == 0xA1)) {
                    o = 0xA1;
                }
            }
            if ((o != -1) && (CRemap[o] != -1)) {
                model[1] = &CModel[CRemap[o]];
            }
        }

        if (render != 0) {
            SetCreatureLights(c);
        }

        for (j = 0; j < 2; j++) {
            if (model[j] == 0) {
                continue;
            }
            if ((c->obj.anim.blend == 0) ||
                ((model[j]->character != 0x45) &&
                 (model[j]->character != 0x8C) &&
                 (model[j]->character != 0xA0) &&
                 (model[j]->character != 0x6B)) ||
                ((model[j]->anmdata[c->obj.anim.blend_src_action] != 0) &&
                 (model[j]->anmdata[c->obj.anim.blend_dst_action] != 0))) {
                pm = &mC;
                if ((Level == 0x1C) && (j == 1) &&
                    (model[0]->character == 0x7F)) {
                    s.x = s.y = s.z = 1.2987013f;
                    NuMtxPreScale(&mC, &s);
                    if (shflag != 0) {
                        NuMtxPreScale(&mS, &s);
                    }
                    if (reflect != 0) {
                        NuMtxPreScale(&mR, &s);
                    }
                }
                if (vflag != 0) {
                    plr_render = 1;
                }
                if ((render != 0) && ((model[j]->character == 0xAF) ||
                                      (model[j]->character == 0xB0))) {
                    SetLevelLights();
                }
                if (model[j]->character == 0x99) {
                    jeep_draw = 1;
                }
                DrawCharacterModel(model[j], &c->obj.anim, pm,
                                   ((shflag != 0) &&
                                    ((vflag == 0) || (j != 0) ||
                                     (model[1] == 0) ||
                                     ((model[1]->character != 0x44) &&
                                      (model[1]->character != 0xB2) &&
                                      (model[1]->character != 0x99) &&
                                      (model[1]->character != 0x63))))
                                       ? &mS : 0,
                                   render,
                                   (reflect != 0) ? &mR : 0,
                                   &c->mtxLOCATOR[j][0],
                                   &c->momLOCATOR[j][0], &c->obj);
            }
        }

        if ((render != 0) && (c->obj.character == 0x76) &&
            (((LDATA->flags & 0x200) != 0) || (Level == 0x1D)) &&
            (ObjTab[66].special != 0)) {
            NuRndrGScnObj(
                ObjTab[66].scene->gobjs[ObjTab[66].special->instance->objid],
                &mC);
        }
        c->obj.draw_frame = old_frame + 1;

    next:
        c->anim_processed = 1;
    }

    glass_phase = 0;
    glass_draw = 0;
    if (render != 0) {
        SetLevelLights();
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


/* UpdateRumble / NewRumble / NewBuzz are GNU89 inline (definitions live
 * before MovePlayer, which inlines them); the deferred standalone copies
 * are emitted at the end of the unit, like StoreLocatorMatrices. */
