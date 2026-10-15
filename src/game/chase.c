/*
 * Unit: game/chase
 *
 * Functions:
 *   0x002586f8 InitChases
 *   0x00258db0 UpdateCrateBallsOfFireDoors
 *   0x00259160 ResetChases
 *   0x002592f8 InitChase
 *   0x00259798 UpdateChase
 *   0x0025a588 DrawChases
 *   0x0025aab8 NuSplineFindPartial
 *   0x0025ab58 ChaseActive
 *   0x0025ab98 UpdateChases
 *   0x0025ac98 NearestChaserDistance
 */

#include "nu_libc.h"

#include "numath/numtx.h"
#include "numath/nuvec.h"
#include "nu3d/nugscn.h"
#include "game/gametypes.h"
#include "gamelib/terrain.h"

#include "creature.h"

struct chase_s {
    struct nugspline_s *spl_START;          /* 0x0000 */
    f32 time;                               /* 0x0004 */
    f32 duration;                           /* 0x0008 */
    struct nugspline_s *spl_CHASER[6];      /* 0x000C */
    struct nuvec_s pos[6];                  /* 0x0024 */
    struct RPos_s RPos[6];                  /* 0x006C */
    struct anim_s anim[6];                  /* 0x0144 */
    struct nuhspecial_s obj[6];             /* 0x01EC */
    s16 character[6];                       /* 0x021C */
    s16 action[6];                          /* 0x0228 */
    u16 xrot[6];                            /* 0x0234 */
    u16 yrot[6];                            /* 0x0240 */
    u16 zrot[6];                            /* 0x024C */
    struct chaseevent_s event[6][24];       /* 0x0258 */
    struct nugspline_s *spl_MISC[6][4];     /* 0x7098 */
    struct Nearest_Light_s lights[6];       /* 0x70F8 */
    f32 scale[6];                           /* 0x7518 */
    u8 misc_phase[6];                       /* 0x7530 */
    s8 ok[6];                               /* 0x7536 */
    u8 cuboid;                              /* 0x753C */
    s8 i;                                   /* 0x753D */
    s8 i_last;                              /* 0x753E */
    s8 i_next;                              /* 0x753F */
    s8 status;                              /* 0x7540 */
    s8 iRAIL;                               /* 0x7541 */
    s16 iALONG;                             /* 0x7542 */
    f32 fALONG;                             /* 0x7544 */
};                                          /* 0x7548 */

struct pchase_s {
    s8 i;                 /* 0x00 */
    s8 i_last;            /* 0x01 */
    s8 i_next;            /* 0x02 */
    u8 cuboid;            /* 0x03 */
    s16 character[6];     /* 0x04 */
    s16 action[6];        /* 0x10 */
    f32 scale[6];         /* 0x1C */
    f32 duration;         /* 0x34 */
};                        /* 0x38 */

extern struct chase_s Chase[3];
extern struct ldata_s *LDATA;
extern struct rail_s Rail[];
extern struct objtab_s ObjTab[201];
extern struct nugscn_s *world_scene[];
extern char tbuf[];
extern s32 temp_iRAIL;
extern s32 temp_iALONG;
extern f32 temp_fALONG;

extern char D_00628738[];
#define fmt_chase_prefix D_00628738
extern char D_00632A80[];
#define str_trigger D_00632A80
extern char D_00632A88[];
#define fmt_name_index D_00632A88
extern char D_00632A90[];
#define fmt_name_index_sep D_00632A90
extern char D_00628748[];
#define str_trigger_event D_00628748
extern char D_00628758[];
#define str_trigger_misc D_00628758
extern char D_00632A98[];
#define str_door1 D_00632A98
extern char D_00632AA0[];
#define str_door2 D_00632AA0
extern char D_00632AA8[];
#define str_door3 D_00632AA8
extern char D_00632AB0[];
#define str_door4 D_00632AB0
extern char D_00632AB8[];
#define str_door25 D_00632AB8
extern char D_00632AC0[];
#define str_door26 D_00632AC0
extern char D_00632AC8[];
#define str_door27 D_00632AC8
extern char D_00632AD0[];
#define str_door28 D_00632AD0
extern char D_00632AD8[];
#define str_door5 D_00632AD8
extern char D_00632AE0[];
#define str_door6 D_00632AE0
extern char D_00632AE8[];
#define str_door7 D_00632AE8
extern char D_00632AF0[];
#define str_door8 D_00632AF0
extern char D_00632AF8[];
#define str_door29 D_00632AF8
extern char D_00632B00[];
#define str_door30 D_00632B00
extern char D_00632B08[];
#define str_door31 D_00632B08
extern char D_00632B10[];
#define str_door32 D_00632B10
extern char D_00628768[];
#define doors_none D_00628768
extern char D_00628778[];
#define doors_first D_00628778
extern char D_00628788[];
#define doors_second D_00628788
extern struct RPos_s *best_cRPos;
extern s32 Level;
extern s32 LivesLost;
extern s32 jonframe1;
extern struct nuvec_s v000;
extern struct nuvec_s chase_midpos[];
extern struct gdeb_s GDeb[];
extern s32 editor_active;
extern s32 Paused;
extern s32 USELIGHTS;
extern s32 LIGHTCHASECHARACTERS;
extern s32 temp_action;
extern f32 temp_time;
extern CharacterData CData[];
extern struct cammtx_s GameCam;
extern u16 temp_xrot;
extern u16 temp_zrot;
extern struct nuvec_s ShadNorm;

extern void NuStrCpy(char *dst, char *src);
extern s32 LineCrossed(f32 xold, f32 zold, f32 xnew, f32 znew,
                       f32 x0, f32 z0, f32 x1, f32 z1);
extern s32 FurtherALONG(s32 iRAIL0, s32 iALONG0, f32 fALONG0, s32 iRAIL1,
                        s32 iALONG1, f32 fALONG1);
extern f32 NuFsqrt(f32 x);
extern void NuStrCat(char *dst, char *src);
extern s32 FurtherBEHIND(s32 iRAIL0, s32 iALONG0, f32 fALONG0, s32 iRAIL1,
                         s32 iALONG1, f32 fALONG1);
extern s32 AheadOfCheckpoint(s32 iRAIL, s32 iALONG, f32 fALONG);
extern void ResetHGobjAnim(struct nuhspecial_s *obj);
extern s32 qrand(void);
extern s32 StartHGobjAnim(struct nuhspecial_s *obj);
extern void PointAlongSpline(struct nugspline_s *spl, f32 ratio, struct nuvec_s *dst,
                             u16 *angle, u16 *tilt);
extern void FindAnglesZX(struct nuvec_s *n);
extern void ResetLights(struct Nearest_Light_s *lights);
extern void JudderGameCamera(struct cammtx_s *cam, f32 time, struct nuvec_s *pos);
extern void GameSfx(s32 id, struct nuvec_s *pos);
extern s32 NuRndrGScnObj(void *gobj, struct numtx_s *m);
extern s32 NuHGobjRndrMtxDwa(struct NUHGOBJ_s *hobj, struct numtx_s *m, s32 nlayers,
                             s16 *layers, struct numtx_s *mtx_array, f32 **dwa);
extern void AddAnimDebris(struct CharacterModel *model, struct numtx_s *mtx, s32 action,
                          f32 time, struct numtx_s *rotmtx);
extern void SetLights(struct nucolour3_s *col0, struct nuvec_s *dir0,
                      struct nucolour3_s *col1, struct nuvec_s *dir1,
                      struct nucolour3_s *col2, struct nuvec_s *dir2, struct nuvec_s *amb);
extern void SetLevelLights(void);
extern void PauseGameAudio(s32 pause);
extern u16 SeekRot(u16 a, u16 target, s32 rate);
extern s32 HitCreatures(struct obj_s *obj, s32 destroy, s32 type);
extern s32 HitCrates(struct obj_s *obj, s32 destroy);
extern s32 HitWumpa(struct obj_s *obj, s32 destroy);
extern void GetLights(struct nuvec_s *pos, struct Nearest_Light_s *lights, s32 glbflag);
extern s32 WipeCreatures(struct RPos_s *rpos);
extern s32 WipeCrates(s32 iRAIL0, s32 iALONG0, f32 fALONG0, s32 iRAIL1, s32 iALONG1,
                      f32 fALONG1, s32 destroy);
extern s32 WipeWumpa(struct RPos_s *rpos);
extern void AddVariableShotDebrisEffect(s32 type, struct nuvec_s *pos, s32 count,
                                        s32 a, s32 b);
extern s32 CylinderCuboidOverlapXZ(struct nuvec_s *pos, struct obj_s *obj,
                                   struct nuvec_s *p2, f32 radius);
extern s32 CylinderCylinderOverlapXZ(struct nuvec_s *p0, f32 r0, struct nuvec_s *p1, f32 r1);
extern void GameSfxLoop(s32 id, struct nuvec_s *pos);
extern s32 GetDieAnim(struct obj_s *obj, s32 anim);
extern s32 KillGameObject(struct obj_s *obj, s32 anim);
extern void LoseMask(struct obj_s *obj);
extern void GetALONG(struct nuvec_s *pos, struct RPos_s *rpos, s32 iRAIL, s32 iALONG, s32 dir);

void UpdateCrateBallsOfFireDoors(void);
void InitChase(struct chase_s *chase);
void UpdateChase(struct chase_s *chase, struct obj_s *obj);


inline struct nugspline_s *NuSplineFindPartial(struct nugscn_s *scene, char *name, char *txt) {
    struct nugspline_s *spl;
    s32 i;

    spl = scene->splines;
    for (i = 0; i < scene->numsplines; i++) {
        if (strncasecmp(name, spl->name, strlen(name)) == 0) {
            NuStrCpy(txt, spl->name);
            return spl;
        }
        spl++;
    }
    return 0;
}


inline f32 NearestChaserDistance(struct chase_s *chase, struct obj_s *obj) {
    s32 j;
    f32 d;
    f32 nearest;

    nearest = 100.0f;
    for (j = 0; j < 6; j++) {
        if (chase->ok[j] != 0) {
            if (best_cRPos != 0
                && FurtherALONG(chase->RPos[j].iRAIL, chase->RPos[j].iALONG, chase->RPos[j].fALONG,
                                best_cRPos->iRAIL, best_cRPos->iALONG, best_cRPos->fALONG) != 0) {
                return 0.0f;
            }
            d = NuVecXZDistSqr(&obj->pos, &chase->pos[j], 0);
            if (d < nearest) {
                nearest = d;
            }
        }
    }
    return NuFsqrt(nearest);
}


void InitChases(void) {
    struct pchase_s *pc;
    struct chase_s *chase;
    char buf[64];
    char name[64];
    char chname[64];
    char txt[256];
    char parsebuf[32];
    struct nuvec_s pos;
    struct nugspline_s *spl;
    struct nuvec_s *p0;
    struct nuvec_s *p1;
    s32 j;
    s32 k;
    s32 l;
    s32 count;
    s32 base_len;
    s32 name_len;
    s32 char_idx;
    char c;

    chase = Chase;
    for (j = 0; j < 3; j++) {
        chase->status = 0;
        chase++;
    }

    pc = LDATA->pChase;
    if (pc == 0 || world_scene[0] == 0) {
        return;
    }

    for (; pc->i != -1; pc++) {
        chase = &Chase[pc->i];
        sprintf(buf, fmt_chase_prefix, pc->i);
        NuStrCpy(chname, buf);
        NuStrCpy(tbuf, chname);
        NuStrCat(tbuf, str_trigger);
        spl = NuSplineFind(world_scene[0], tbuf);
        chase->spl_START = spl;
        if (spl == 0) {
            continue;
        }
        if (spl->len != 2) {
            chase->spl_START = 0;
        }
        if (chase->spl_START == 0) {
            continue;
        }

        count = 0;
        for (j = 0; j < 6; j++) {
            chase->ok[j] = 0;
            chase->character[j] = -1;
            chase->obj[j].special = 0;
            if (pc->character[j] == -1) {
                continue;
            }

            sprintf(tbuf, fmt_name_index, buf, j);
            spl = NuSplineFind(world_scene[0], tbuf);
            chase->spl_CHASER[j] = spl;
            if (spl == 0) {
                continue;
            }

            if (pc->character[j] > 0x3E6) {
                k = pc->character[j] - 0x3E7;
                if ((u32)k > 200) {
                    continue;
                }
                if (ObjTab[k].obj.special == 0) {
                    continue;
                }
                chase->obj[j] = ObjTab[k].obj;
            } else {
                if (CRemap[pc->character[j]] == -1) {
                    continue;
                }
                chase->character[j] = pc->character[j];
                chase->action[j] = pc->action[j];
            }
            chase->scale[j] = pc->scale[j];

            sprintf(name, fmt_name_index_sep, buf, j);
            NuStrCat(name, str_trigger_event);
            for (k = 0; k < 24; k++) {
                sprintf(tbuf, fmt_name_index_sep, name, k);
                base_len = strlen(tbuf);
                spl = NuSplineFindPartial(world_scene[0], tbuf, txt);
                chase->event[j][k].spl = spl;
                if (spl != 0 && spl->len != 2) {
                    chase->event[j][k].spl = 0;
                }
                if (chase->event[j][k].spl == 0) {
                    continue;
                }

                for (l = 0; l < 24; l++) {
                    chase->event[j][k].obj[l].special = 0;
                }

                count = 0;
                name_len = strlen(txt);
                char_idx = 0;
                for (; base_len < name_len; base_len++) {
                    c = txt[base_len];
                    if (c == 'x' || base_len == name_len - 1) {
                        if (base_len == name_len - 1) {
                            parsebuf[char_idx++] = c;
                        }
                        parsebuf[char_idx] = 0;
                        if (NuSpecialFind(world_scene[0], &chase->event[j][k].obj[count], parsebuf) != 0) {
                            count++;
                            if (count == 24) {
                                break;
                            }
                        }
                        char_idx = 0;
                    } else {
                        parsebuf[char_idx++] = c;
                    }
                }
            }

            sprintf(name, fmt_name_index_sep, buf, j);
            NuStrCat(name, str_trigger_misc);
            for (k = 0; k < 4; k++) {
                sprintf(tbuf, fmt_name_index, name, k);
                spl = NuSplineFind(world_scene[0], tbuf);
                chase->spl_MISC[j][k] = spl;
                if (spl != 0 && spl->len != 2) {
                    chase->spl_MISC[j][k] = 0;
                }
            }

            count++;
            chase->ok[j] = 1;
        }

        if (count != 0) {
            p0 = (struct nuvec_s *)chase->spl_START->pts;
            p1 = (struct nuvec_s *)(chase->spl_START->pts + chase->spl_START->ptsize);
            pos.x = (p0->x + p1->x) * 0.5f;
            pos.y = (p0->y + p1->y) * 0.5f;
            pos.z = (p0->z + p1->z) * 0.5f;
            GetALONG(&pos, 0, -1, -1, 1);
            chase->iRAIL = temp_iRAIL;
            chase->iALONG = temp_iALONG;
            chase->fALONG = temp_fALONG;
            chase->duration = pc->duration;
            chase->i = pc->i;
            chase->i_last = pc->i_last;
            chase->i_next = pc->i_next;
            chase->status = 1;
            chase->cuboid = pc->cuboid;
        }

    }
}


void UpdateCrateBallsOfFireDoors(void) {
    struct nuhspecial_s obj[8];
    char txt[16];
    s32 i;

    if (best_cRPos != 0 && Rail[best_cRPos->iRAIL].type == 0) {
        NuSpecialFind(world_scene[0], &obj[0], str_door1);
        NuSpecialFind(world_scene[0], &obj[1], str_door2);
        NuSpecialFind(world_scene[0], &obj[2], str_door3);
        NuSpecialFind(world_scene[0], &obj[3], str_door4);
        NuSpecialFind(world_scene[0], &obj[4], str_door25);
        NuSpecialFind(world_scene[0], &obj[5], str_door26);
        NuSpecialFind(world_scene[0], &obj[6], str_door27);
        NuSpecialFind(world_scene[0], &obj[7], str_door28);
        if (FurtherBEHIND(best_cRPos->iRAIL, best_cRPos->iALONG, best_cRPos->fALONG, 2, 0x14, 0.5f) != 0) {
            NuStrCpy(txt, doors_none);
        } else if (FurtherBEHIND(best_cRPos->iRAIL, best_cRPos->iALONG, best_cRPos->fALONG, 2, 0x21, 0.5f) != 0) {
            NuStrCpy(txt, doors_first);
        } else {
            NuStrCpy(txt, doors_second);
        }
        for (i = 0; i < 8; i++) {
            if (obj[i].special != 0) {
                obj[i].special->instance->flags.visible = txt[i] == '1';
            }
        }

        NuSpecialFind(world_scene[0], &obj[0], str_door5);
        NuSpecialFind(world_scene[0], &obj[1], str_door6);
        NuSpecialFind(world_scene[0], &obj[2], str_door7);
        NuSpecialFind(world_scene[0], &obj[3], str_door8);
        NuSpecialFind(world_scene[0], &obj[4], str_door29);
        NuSpecialFind(world_scene[0], &obj[5], str_door30);
        NuSpecialFind(world_scene[0], &obj[6], str_door31);
        NuSpecialFind(world_scene[0], &obj[7], str_door32);
        if (FurtherBEHIND(best_cRPos->iRAIL, best_cRPos->iALONG, best_cRPos->fALONG, 2, 0x38, 0.5f) != 0) {
            NuStrCpy(txt, doors_none);
        } else if (FurtherBEHIND(best_cRPos->iRAIL, best_cRPos->iALONG, best_cRPos->fALONG, 2, 0x41, 0.5f) != 0) {
            NuStrCpy(txt, doors_first);
        } else {
            NuStrCpy(txt, doors_second);
        }
        for (i = 0; i < 8; i++) {
            if (obj[i].special != 0) {
                obj[i].special->instance->flags.visible = txt[i] == '1';
            }
        }
    }
}


void InitChase(struct chase_s *chase) {
    struct CharacterModel *model;
    f32 shadow;
    u16 action;
    s32 j;

    for (j = 0; j < 6; j++) {
        if (chase->ok[j] == 0) {
            continue;
        }

        if (chase->i_last == -1) {
            goto init_anim;
        }
        if (chase->character[j] == -1) {
            goto start_anim;
        }
        if (chase->character[j] != Chase[chase->i_last].character[j]) {
            goto init_anim;
        }
        chase->anim[j] = Chase[chase->i_last].anim[j];
        chase->lights[j] = Chase[chase->i_last].lights[j];
        goto start_anim;

init_anim:
        if (chase->character[j] == -1) {
            goto start_anim;
        }
        model = &CModel[CRemap[chase->character[j]]];
        if (chase->action[j] != -1 && model->anmdata[chase->action[j]] != 0) {
            chase->anim[j].anim_time = (f32)qrand() * (1.0f / 65535.0f)
                                       * (model->anmdata[chase->action[j]]->time - 1.0f) + 1.0f;
        } else {
            chase->anim[j].anim_time = 1.0f;
        }
        action = chase->action[j];
        chase->anim[j].blend = 0;
        chase->anim[j].newaction = action;
        chase->anim[j].oldaction = action;
        chase->anim[j].action = action;

start_anim:
        if (chase->obj[j].special != 0) {
            StartHGobjAnim(&chase->obj[j]);
        }
        chase->xrot[j] = 0;
        chase->zrot[j] = 0;
        PointAlongSpline(chase->spl_CHASER[j], 0.0f, &chase->pos[j], &chase->yrot[j], 0);
        shadow = NewShadowMask(&chase->pos[j], 0.0f, -1);
        if (shadow != 2000000.0f) {
            FindAnglesZX(&ShadNorm);
            if (Level != 0x11 && Level != 0xE && Level != 0x1F && Level != 5) {
                chase->pos[j].y = shadow;
            }
        } else {
            temp_zrot = 0;
            temp_xrot = 0;
        }
        chase->xrot[j] = temp_xrot;
        chase->zrot[j] = temp_zrot;
        GetALONG(&chase->pos[j], &chase->RPos[j], -1, -1, 2);
        ResetLights(&chase->lights[j]);
    }

    chase->time = 0.0f;
    chase->status = 2;
    JudderGameCamera(&GameCam, 0.5f, 0);
    GameSfx(0x3B, 0);
    NewBuzz(&player->rumble, 0x19);
}


void ResetChases(void) {
    struct chase_s *chase;
    s32 i;
    s32 j;
    s32 k;
    s32 l;

    chase = Chase;
    for (i = 0; i < 3; i++, chase++) {
        if (chase->status != 0
            && AheadOfCheckpoint(chase->iRAIL, chase->iALONG, chase->fALONG) != 0) {
            for (j = 0; j < 6; j++) {
                if (chase->ok[j] != 0) {
                    if (chase->obj[j].special != 0) {
                        ResetHGobjAnim(&chase->obj[j]);
                    }
                    for (k = 0; k < 24; k++) {
                        if (chase->event[j][k].spl != 0) {
                            for (l = 0; l < 24; l++) {
                                ResetHGobjAnim(&chase->event[j][k].obj[l]);
                            }
                        }
                    }
                    chase->misc_phase[j] = 0;
                }
            }
            chase->status = 1;
        }
    }
}


void UpdateChase(struct chase_s *chase, struct obj_s *obj) {
    struct nuvec_s oldpos;
    struct nuvec_s pos;
    struct obj_s c_obj;
    struct RPos_s best_RPos;
    struct nuvec_s *p0;
    struct nuvec_s *p1;
    CharacterData *cdata;
    struct CharacterModel *model;
    s32 holding;
    s32 kill;
    s32 count;
    s32 sfx;
    s32 started;
    s32 j;
    s32 k;
    s32 l;
    f32 d;
    f32 t;
    f32 y;
    f32 ratio;

    d = -1.0f;
    holding = 0;
    if (chase->i_last != -1 && Chase[chase->i_last].status != 3) {
        holding = 1;
    }

    if ((obj->dead == 0 || Level != 0x11) && holding == 0) {
        t = 0.02f;
        if (LivesLost != 0) {
            j = LivesLost;
            if (j > 5) {
                j = 5;
            }
            t = t - (f32)j * t * 0.05f;
        }
        d = NearestChaserDistance(chase, obj);
        if (chase->time < chase->duration && (Level != 0x11 || d != 0.0f)) {
            if (d >= 3.0f) {
                t = t * (d / 3.0f);
            }
            chase->time = chase->time + t;
            if (chase->time >= chase->duration) {
                chase->time = chase->duration;
                chase->status = 3;
                PauseGameAudio(0);
                GameSfx(0x3B, 0);
                JudderGameCamera(&GameCam, 0.5f, 0);
            }
        }
    }

    ratio = chase->time / chase->duration;
    best_RPos.iRAIL = -1;
    chase_midpos[chase->i] = v000;
    kill = 0;
    count = 0;

    for (j = 0; j < 6; j++) {
        if (chase->ok[j] == 0) {
            continue;
        }
        if (chase->status == 2) {
            t = 0.1200000048f;
            oldpos = chase->pos[j];
            PointAlongSpline(chase->spl_CHASER[j], ratio, &pos, &chase->yrot[j], 0);
            chase->pos[j].x = pos.x;
            chase->pos[j].z = pos.z;
            y = NewShadowMaskPlat(&pos, 0.0f, -1);
            if (y != 2000000.0f) {
                FindAnglesZX(&ShadNorm);
                if (Level == 8) {
                    pos.y = y;
                }
            } else {
                temp_zrot = 0;
                temp_xrot = 0;
            }
            if (Level == 8) {
                if (chase->pos[j].y < pos.y) {
                    chase->pos[j].y = pos.y;
                } else {
                    chase->pos[j].y = chase->pos[j].y + (pos.y - chase->pos[j].y) * t;
                }
            } else {
                chase->pos[j].y = pos.y;
            }

            if (Level != 0xE) {
                chase->xrot[j] = SeekRot(chase->xrot[j], temp_xrot, 4);
                chase->zrot[j] = SeekRot(chase->zrot[j], temp_zrot, 4);
            } else {
                chase->zrot[j] = 0;
                chase->xrot[j] = 0;
            }

            count++;
            NuVecAdd(&chase_midpos[chase->i], &chase_midpos[chase->i], &chase->pos[j]);
            GetALONG(&chase->pos[j], &chase->RPos[j], chase->RPos[j].iRAIL, chase->RPos[j].iALONG, 2);

            if (best_RPos.iRAIL == -1
                || FurtherBEHIND(chase->RPos[j].iRAIL, chase->RPos[j].iALONG, chase->RPos[j].fALONG,
                                 best_RPos.iRAIL, best_RPos.iALONG, best_RPos.fALONG) != 0) {
                best_RPos.iRAIL = chase->RPos[j].iRAIL;
                best_RPos.iALONG = chase->RPos[j].iALONG;
                best_RPos.fALONG = chase->RPos[j].fALONG;
            }

            for (k = 0; k < 24; k++) {
                if (chase->event[j][k].spl == 0) {
                    continue;
                }
                p0 = (struct nuvec_s *)chase->event[j][k].spl->pts;
                p1 = (struct nuvec_s *)(chase->event[j][k].spl->pts + chase->event[j][k].spl->ptsize);
                if (LineCrossed(oldpos.x, oldpos.z, chase->pos[j].x, chase->pos[j].z,
                                p0->x, p0->z, p1->x, p1->z) != 2) {
                    continue;
                }
                started = 0;
                for (l = 0; l < 24; l++) {
                    if (StartHGobjAnim(&chase->event[j][k].obj[l]) != 0) {
                        started++;
                    }
                }
                if (started == 0) {
                    continue;
                }
                if (Level == 0x11) {
                    sfx = 0xC8;
                    if (qrand() <= 0x7FFF) {
                        sfx = 0xC7;
                    }
                } else if (Level != 8 || j == 0) {
                    sfx = 0x3B;
                }
                if (sfx != -1) {
                    GameSfx(sfx, &chase->pos[j]);
                }
                JudderGameCamera(&GameCam, 0.3f, 0);
            }

            for (k = 0; k < 4; k++) {
                if (chase->spl_MISC[j][k] == 0) {
                    continue;
                }
                p0 = (struct nuvec_s *)chase->spl_MISC[j][k]->pts;
                p1 = (struct nuvec_s *)(chase->spl_MISC[j][k]->pts + chase->spl_MISC[j][k]->ptsize);
                if (LineCrossed(oldpos.x, oldpos.z, chase->pos[j].x, chase->pos[j].z,
                                p0->x, p0->z, p1->x, p1->z) == 2) {
                    chase->misc_phase[j] = k + 1;
                }
            }

            k = chase->character[j];
            if (k != -1 && k < 0x3E7) {
                cdata = &CData[k];
                c_obj.pos = chase->pos[j];
                c_obj.min = cdata->min;
                c_obj.max = cdata->max;
                c_obj.bot = cdata->min.y;
                c_obj.top = cdata->max.y;
                c_obj.SCALE = cdata->scale * chase->scale[j];
                c_obj.RADIUS = cdata->radius * c_obj.SCALE;
                c_obj.attack = 0x200;
                c_obj.pLOCATOR = 0;
                c_obj.flags = 0;
                HitCreatures(&c_obj, 1, 3);
                HitCrates(&c_obj, 2);
                HitWumpa(&c_obj, 0);
                if (USELIGHTS != 0 && LIGHTCHASECHARACTERS != 0) {
                    pos.x = c_obj.pos.x;
                    pos.y = c_obj.pos.y + (c_obj.bot + c_obj.top) * c_obj.SCALE * 0.5f;
                    pos.z = c_obj.pos.z;
                    GetLights(&pos, &chase->lights[j], 1);
                }
            } else {
                WipeCreatures(&chase->RPos[j]);
                WipeCrates(chase->iRAIL, chase->iALONG, chase->fALONG,
                           chase->RPos[j].iRAIL, chase->RPos[j].iALONG, chase->RPos[j].fALONG, 2);
                WipeWumpa(&chase->RPos[j]);
            }

            if (Level == 8 && (jonframe1 & 3) == 0) {
                AddVariableShotDebrisEffect(GDeb[131].i, &chase->pos[j], 1, 0, 0);
            }
        }

        if (obj->dead == 0 && holding == 0 && kill == 0) {
            k = chase->character[j];
            if (k != -1 && k < 0x3E7) {
                cdata = &CData[k];
                c_obj.pos = chase->pos[j];
                c_obj.min = cdata->min;
                c_obj.max = cdata->max;
                c_obj.bot = cdata->min.y;
                c_obj.top = cdata->max.y;
                c_obj.SCALE = cdata->scale * chase->scale[j];
                c_obj.RADIUS = fabs(cdata->radius) * c_obj.SCALE;
                c_obj.attack = 0x200;
                c_obj.hdg = chase->yrot[j];
                if (d >= 0.0f) {
                    if (((chase->cuboid & (1 << j)) & 0xFF)
                            ? CylinderCuboidOverlapXZ(&obj->pos, &c_obj, &c_obj.pos, obj->RADIUS)
                            : CylinderCylinderOverlapXZ(&obj->pos, obj->RADIUS, &c_obj.pos, c_obj.RADIUS)) {
                        kill = 1;
                    }
                }
            }
        }

        if (chase->character[j] != -1) {
            model = &CModel[CRemap[chase->character[j]]];
            chase->anim[j].oldaction = chase->anim[j].action;
            if (chase->character[j] == 0x42) {
                if (chase->status == 3) {
                    k = 0x33;
                    if (chase->i == 2) {
                        k = 0x75;
                    }
                } else if (obj->dead != 0) {
                    k = 0x75;
                } else if (chase->misc_phase[j] == 1) {
                    k = 0x40;
                } else {
                    k = 0x3A;
                }
            } else {
                k = chase->action[j];
            }
            chase->anim[j].newaction = k;
            UpdateAnimPacket(model, &chase->anim[j], 0.59999996f, 0.0f);
        }
    }

    if (chase->status == 2 && count != 0) {
        sfx = -1;
        switch (Level) {
        case 5:
            sfx = 0xC7;
            break;
        case 8:
            sfx = 0xCA;
            break;
        case 0x1F:
            sfx = 0xCA;
            break;
        case 0xE:
            sfx = 0xD1;
            break;
        }
        if (sfx != -1) {
            GameSfxLoop(sfx, &chase_midpos[chase->i]);
        }
        if (qrand() < 0x800) {
            j = qrand() / 256;
            NewRumble(&player->rumble, j);
            JudderGameCamera(&GameCam, (f32)j / 255.0f * 0.3f, 0);
        }
    }

    if (obj->dead == 0 && holding == 0 && kill == 0 && best_RPos.iRAIL != -1 && best_cRPos != 0) {
        kill = FurtherALONG(best_RPos.iRAIL, best_RPos.iALONG, best_RPos.fALONG,
                            best_cRPos->iRAIL, best_cRPos->iALONG, best_cRPos->fALONG) != 0;
    }
    if (kill != 0) {
        if (KillGameObject(obj, GetDieAnim(obj, -1)) != 0 && Level == 0x11) {
            GameSfx(0xCB, 0);
        }
        if (obj->mask != 0 && obj->mask->active != 0 && (LDATA->flags & 0xE00) == 0) {
            LoseMask(obj);
            obj->mask->active = 0;
        }
        obj->invincible = 0;
    }
}


void DrawChases(s32 render) {
    struct numtx_s m;
    struct numtx_s tmtx[256];
    struct nuvec_s scale;
    struct numtx_s mtxLOCATOR[16];
    f32 **dwa;
    struct chase_s *chase;
    struct nuspecial_s *special;
    struct CharacterModel *model;
    s32 i;
    s32 j;

    chase = Chase;
    for (i = 0; i < 3; i++, chase++) {
        if (!((chase->status == 2 && (chase->i_last == -1 || Chase[chase->i_last].status == 3))
              || (chase->status == 3 && chase->i_next != -1
                  && (chase->i_next > 2 || Chase[chase->i_next].status == 1
                      || Chase[chase->i_next].status == 0)))) {
            continue;
        }
        for (j = 0; j < 6; j++) {
            if (chase->ok[j] == 0) {
                continue;
            }
            if (chase->obj[j].special != 0) {
                scale.x = scale.y = scale.z = 1.0f;
                NuMtxSetScale(&m, &scale);
                NuMtxRotateY(&m, chase->yrot[j] + 0x8000);
                NuMtxRotateZ(&m, chase->zrot[j]);
                NuMtxRotateX(&m, chase->xrot[j]);
                NuMtxTranslate(&m, &chase->pos[j]);
                if (render != 0) {
                    if (editor_active != 0) {
                        m = chase->obj[j].special->mtx;
                    }
                    if (Level != 0x1F) {
                        special = chase->obj[j].special;
                        NuRndrGScnObj(chase->obj[j].scene->gobjs[special->instance->objid], &m);
                    }
                    chase->obj[j].special->instance->mtx = m;
                    if (Level == 0x1F && ObjTab[0x4E].obj.special != 0) {
                        special = ObjTab[0x4E].obj.special;
                        NuRndrGScnObj(ObjTab[0x4E].obj.scene->gobjs[special->instance->objid], &m);
                    }
                }
            } else if (chase->character[j] != -1) {
                model = &CModel[CRemap[chase->character[j]]];
                scale.x = scale.y = scale.z = CData[chase->character[j]].scale * chase->scale[j];
                NuMtxSetScale(&m, &scale);
                NuMtxRotateY(&m, chase->yrot[j] + 0x8000);
                NuMtxRotateZ(&m, chase->zrot[j]);
                NuMtxRotateX(&m, chase->xrot[j]);
                NuMtxTranslate(&m, &chase->pos[j]);
                EvalModelAnim(model, &chase->anim[j], &m, tmtx, &dwa, mtxLOCATOR);
                if (temp_action != -1 && Paused == 0) {
                    AddAnimDebris(model, mtxLOCATOR, temp_action, temp_time, 0);
                }
                if (render != 0) {
                    if (USELIGHTS != 0 && LIGHTCHASECHARACTERS != 0) {
                        SetLights(&chase->lights[j].pDir1st->Colour, &chase->lights[j].pDir1st->Direction,
                                  &chase->lights[j].pDir2nd->Colour, &chase->lights[j].pDir2nd->Direction,
                                  &chase->lights[j].pDir3rd->Colour, &chase->lights[j].pDir3rd->Direction,
                                  &chase->lights[j].AmbCol);
                    }
                    NuHGobjRndrMtxDwa(model->hobj, &m, 1, 0, tmtx, dwa);
                }
            }
        }
    }
    if (render != 0 && USELIGHTS != 0 && LIGHTCHASECHARACTERS != 0) {
        SetLevelLights();
    }
}


s32 ChaseActive(void) {
    s32 i;

    for (i = 0; i < 3; i++) {
        if (Chase[i].status == 2) {
            return i;
        }
    }
    return -1;
}


void UpdateChases(void) {
    struct chase_s *chase;
    struct obj_s *obj;
    struct nuvec_s *p0;
    struct nuvec_s *p1;
    s32 i;

    if (Level == 0x1F) {
        UpdateCrateBallsOfFireDoors();
    }

    chase = Chase;
    obj = &player->obj;
    for (i = 0; i < 3; i++, chase++) {
        if (chase->status == 1) {
            if (player->obj.transporting == 0) {
                p0 = (struct nuvec_s *)chase->spl_START->pts;
                p1 = (struct nuvec_s *)(chase->spl_START->pts + chase->spl_START->ptsize);
                if (LineCrossed(obj->oldpos.x, obj->oldpos.z, obj->pos.x, obj->pos.z,
                                p0->x, p0->z, p1->x, p1->z) == 2) {
                    InitChase(chase);
                }
            }
        } else if (chase->status > 1) {
            if (chase->status < 4) {
                UpdateChase(chase, obj);
            }
        }
    }
}
