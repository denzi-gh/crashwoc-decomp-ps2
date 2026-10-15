/*
 * Unit: game/move
 *
 * Functions:
 *   0x00250588 InitPlayerEvents
 *   0x002507d0 CheckPlayerEvents
 *   0x00250908 UpdateChaseRunAnim
 *   0x00250a98 MoveCRASH
 *   0x00251c98 AnimateCRASH
 *   0x00252248 MoveSWIMMING
 *   0x00252478 MoveCOCO
 *   0x00252d18 AnimateCOCO
 *   0x00252fb0 MoveSCOOTER
 *   0x00253278 MoveSNOWBOARD
 *   0x00253590 AnimateSNOWBOARD
 *   0x00253720 MoveOFFROADER
 *   0x00253cd8 MoveMECH
 *   0x00254338 AnimateMECH
 *   0x002544d0 MoveFIREENGINE
 *   0x002547c0 AnimateFIREENGINE
 *   0x002548a8 MoveGYRO
 *   0x002552f8 AnimateGYRO
 *   0x002553f0 DrawCarpet
 *   0x002554e0 MoveSUBMARINE
 *   0x002556c8 MoveMINECART
 *   0x00255ca8 ResetTubs
 *   0x00255ec8 MoveTub
 *   0x00256a18 MoveMINETUB
 *   0x00257270 AnimateMINETUB
 *   0x00257570 CheckGates
 *   0x00257700 ResetRings
 *   0x00257880 CheckRings
 *   0x00257d70 DrawRings
 *   0x00257e80 AnimateSWIMMING
 *   0x00257f88 AnimateSCOOTER
 *   0x00258020 AnimateOFFROADER
 *   0x002580f8 AnimateSUBMARINE
 *   0x00258158 AnimateMINECART
 *   0x002581b0 AnimateGLIDER
 *   0x00258270 AnimateDROPSHIP
 *   0x002582c0 AnimateATLASPHERE
 *   0x00258348 AnimateJEEP
 *   0x00258380 AnimateMOSQUITO
 *   0x002583d0 ResetGates
 *   0x002584b8 ResetPlayerEvents
 *   0x00258568 UpdateArrow
 *   0x00258618 DrawPanel3DArrow
 *   0x002586a8 AnimateDIVE
 *   0x002586d0 ResetTub
 */

#include "nu_libc.h"

#include "numath/numtx.h"
#include "numath/nutrig.h"
#include "numath/nuvec.h"
#include "nu3d/nugscn.h"
#include "game/gametypes.h"
#include "game/creature.h"

extern s32 FlyingLevelVictoryDance;
extern struct MoveInfo CrashMoveInfo;
extern struct MoveInfo SwimmingMoveInfo;
extern struct MoveInfo CocoMoveInfo;
extern s32 Level;
extern s32 SmokeyCountDownValue;
extern struct RPos_s *best_cRPos;
extern f32 D_0062E844;
extern s32 FireBossHoldPlayer;
extern f32 SCOOTERJUMPPAUSEFRAME;
extern f32 FIREENGINERUNNINGJUMPPAUSEFRAME;
extern f32 FIREENGINESTANDINGJUMPPAUSEFRAME;

u64 fptodp(f32 value);
s32 dpcmp(u64 a, u64 b);
u64 dpsub(u64 a, u64 b);

struct pad_s {
    u8 unk_0x000[0x55C];     /* 0x000 (opaque) */
    unsigned int paddata;    /* 0x55C (held buttons) */
    u8 unk_0x560[4];         /* 0x560 */
    unsigned int buttons;    /* 0x564 (debounced buttons) */
    u8 unk_0x568[0x16];      /* 0x568 */
    u8 l_alg_x;              /* 0x57E */
    u8 l_alg_y;              /* 0x57F */
};
extern struct pad_s *Pad[];

struct plrevent_s {
    struct nugspline_s *spl;        /* 0x00 */
    struct nuhspecial_s obj[16];    /* 0x04 */
    struct nuvec_s pos;             /* 0x84 */
    s8 triggered;                   /* 0x90 */
    s8 iRAIL;                       /* 0x91 */
    s16 iALONG;                     /* 0x92 */
    f32 fALONG;                     /* 0x94 */
};

extern struct spltab_s SplTab[];
extern struct tersurface_s TerSurface[];
extern s32 GameMode;
extern s32 last_hub;
extern s32 tumble_action;
extern f32 tumble_time;
extern f32 tumble_duration;
extern s32 plr_tumblehack;
extern s32 plr_rebound;
extern f32 FALLONCRATEBREAKSPEED;
extern struct chase_s Chase[3];
extern s32 plr_tocam_wait;
extern s32 plr_tocam_turn;
extern struct plrevent_s PlrEvent[8];
extern struct nugscn_s *world_scene[];
extern struct CharacterModel *carpet_model;
extern s32 carpet_character;
extern u16 carpet_xrot;
extern u16 carpet_yrot;
extern struct nuvec_s carpet_pos;
extern struct anim_s carpet_anim;
extern struct Nearest_Light_s carpet_lights;
extern struct objtab_s ObjTab[];
extern struct gtimer_s GameTimer;
extern struct cammtx_s GameCam;
extern s32 GATECOUNT;
extern s32 plr_gates;
extern u64 gate_bits;
extern s32 bonusgem_ok;
extern s32 LineCrossed(f32 xold, f32 zold, f32 xnew, f32 znew,
                       f32 x0, f32 z0, f32 x1, f32 z1);
extern s32 StartHGobjAnim(struct nuhspecial_s *obj);
extern void JudderGameCamera(struct cammtx_s *cam, f32 time, struct nuvec_s *pos);
extern void GameSfx(s32 sfx, struct nuvec_s *pos);
extern s32 temp_iRAIL;
extern s32 temp_iALONG;
extern f32 temp_fALONG;
extern u16 arrow_xrot;
extern u16 arrow_yrot;

s32 RotDiff(u16 a, u16 b);
void AddGameDebrisRot(s32 i, struct nuvec_s *pos, s32 n, u16 xrot, u16 yrot);
void GetALONG(struct nuvec_s *pos, struct RPos_s *rpos, s32 iRAIL, s32 iALONG, s32 dir);
s32 AheadOfCheckpoint(s32 iRAIL, s32 iALONG, f32 fALONG);
f32 NuFsqrt(f32 x);
u16 SeekRot(u16 a, u16 target, s32 rate);
s32 qrand(void);
s32 ResetHGobjAnim(struct nuhspecial_s *obj);
void SetNearestLights(struct Nearest_Light_s *l);
void SetLevelLights(void);
s32 NuHGobjRndrMtxDwa(struct NUHGOBJ_s *hobj, struct numtx_s *m, s32 nlayers,
                      s16 *layers, struct numtx_s *mtx_array, f32 **dwa);
s32 DrawPanel3DObject(s32 object, f32 x, f32 y, f32 z, f32 scalex, f32 scaley,
                      f32 scalez, u16 xrot, u16 yrot, u16 zrot,
                      struct nugscn_s *scn, struct nuspecial_s *obj, s32 rot);


void UpdateChaseRunAnim(struct creature_s *plr) {
    if ((plr->obj.anim.newaction == 0x3A) &&
        ((Chase[0].status == 2) || (Chase[1].status == 2) ||
         (Chase[2].status == 2)) &&
        (best_cRPos != 0) &&
        (abs(RotDiff(plr->obj.hdg, best_cRPos->angle)) < 0x2000)) {
        if (plr_tocam_wait != 0) {
            plr_tocam_wait--;
            if (plr_tocam_wait == 0) {
                plr_tocam_wait = ((qrand() * 50) >> 16) + 50;
                if (plr_tocam_turn == -1) {
                    plr_tocam_turn = qrand() / 0x8000;
                } else if (plr_tocam_turn == 1) {
                    plr_tocam_turn = -qrand() / 0x8000;
                } else {
                    plr_tocam_turn = (qrand() < 0x8000) ? -1 : 1;
                }
            }
        }
    } else {
        plr_tocam_turn = 0;
        plr_tocam_wait = ((qrand() * 50) >> 16) + 50;
    }
    if (plr->obj.anim.newaction == 0x3A) {
        if (plr_tocam_turn < 0) {
            plr->obj.anim.newaction = 0x32;
        } else if (plr_tocam_turn > 0) {
            plr->obj.anim.newaction = 0x39;
        } else {
            plr->obj.anim.newaction = 0x3A;
        }
    }
}

void AnimateCRASH(struct creature_s *plr) {
    struct MoveInfo *info = &CrashMoveInfo;
    struct nuvec_s pos;
    s16 action;

    if ((u32)plr->obj.dead >= 2) {
        plr->obj.anim.newaction = plr->obj.die_action;
    } else if (GameMode == 1) {
        plr->obj.anim.newaction = 0x22;
    } else if ((Level == 0x25) && (tumble_time < tumble_duration) &&
               (last_hub != -1)) {
        plr->obj.anim.newaction = tumble_action;
    } else if (plr_tumblehack != 0) {
        plr->obj.anim.newaction = 0x22;
    } else if (plr_rebound != 0) {
        plr->obj.anim.newaction = 0x19;
    } else if (plr->freeze != 0) {
        plr->obj.anim.newaction = 0x1E;
    } else if (plr->target != 0) {
        if (plr->fire != 0) {
            plr->obj.anim.newaction = 0x4F;
        } else if (plr->obj.dyrot != 0) {
            plr->obj.anim.newaction = 0x50;
        } else {
            plr->obj.anim.newaction = 0x4E;
        }
    } else if (plr->obj.dangle != 0) {
        if (plr->obj.dangle == 2) {
            plr->obj.anim.newaction = 0x36;
        } else if (plr->obj.pad_speed != 0.0f) {
            plr->obj.anim.newaction = 0x4D;
        } else {
            plr->obj.anim.newaction = 0x20;
        }
    } else if (plr->slam_wait != 0) {
        plr->obj.anim.newaction = 0x1F;
    } else if (plr->slam == 2) {
        plr->obj.anim.newaction = 0x4B;
    } else if (plr->slam == 1) {
        plr->obj.anim.newaction = 0x41;
    } else if (plr->slide != 0) {
        plr->obj.anim.newaction = 0x43;
    } else if ((plr->jump != 0) &&
               ((plr->obj.ground == 0) || (plr->jump_frame < plr->jump_frames))) {
        if (plr->somersault != 0) {
            plr->obj.anim.newaction = 0x44;
        } else if (plr->jump_type == 4) {
            plr->obj.anim.newaction = 0x19;
        } else if (plr->jump_type == 0) {
            plr->obj.anim.newaction = (plr->jump_subtype == 1) ? 0x2D : 0x2E;
        } else if (plr->jump == 5) {
            plr->obj.anim.newaction = 0x44;
        } else {
            plr->obj.anim.newaction = 0x2C;
        }
    } else if ((plr->land != 0) && (plr->obj.pad_speed == 0.0f)) {
        plr->obj.anim.newaction = 0x30;
    } else if ((plr->crouch_pos > 0) &&
               (plr->crouch_pos < plr->minfo->CROUCHINGFRAMES) &&
               (plr->obj.pad_speed == 0.0f)) {
        plr->obj.anim.newaction = (plr->crawl != 0) ? 3 : 5;
    } else if ((plr->crawl != 0) && ((plr->jump == 0) || (plr->obj.ground != 0))) {
        if (plr->obj.pad_speed == 0.0f) {
            plr->obj.anim.newaction = 4;
        } else {
            plr->obj.anim.newaction = 2;
        }
    } else if ((plr->obj.wade != 0) && (plr->obj.ground != 0) &&
               (plr->obj.pad_speed != 0.0f)) {
        plr->obj.anim.newaction = 0x72;
    } else if (((plr->obj.ground != 0) || (plr->allow_jump != 0)) &&
               (TerSurface[plr->obj.surface_type].flags & 0x20) &&
               ((plr->obj.pad_speed != 0.0f) ||
                (plr->obj.xz_distance >= info->IDLESPEED))) {
        if (plr->obj.pad_speed != 0.0f) {
            plr->obj.anim.newaction = 0x3F;
        } else {
            plr->obj.anim.newaction = 0x40;
        }
    } else if ((plr->obj.ground == 0) && (plr->allow_jump == 0) &&
               (plr->obj.mom.y < -FALLONCRATEBREAKSPEED)) {
        plr->obj.anim.newaction = 0x19;
    } else if ((plr->obj.pad_speed == 0.0f) &&
               !((plr->pad_type == 1) &&
                 (plr->obj.mom.x * plr->obj.mom.x + plr->obj.mom.z * plr->obj.mom.z >=
                  plr->minfo->IDLESPEED * plr->minfo->IDLESPEED))) {
        if (((plr->obj.ground == 1) ||
             ((plr->obj.last_ground == 1) && (plr->allow_jump != 0))) &&
            ((plr->obj.pos.y + plr->obj.bot * plr->obj.SCALE) - plr->obj.shadow > 0.25f)) {
            plr->obj.anim.newaction = 0x51;
        } else {
            plr->obj.anim.newaction = 0x22;
        }
    } else if (plr->tiptoe != 0) {
        plr->obj.anim.newaction = 0x53;
    } else if (plr->sprint != 0) {
        plr->obj.anim.newaction = 0x48;
    } else if (plr->obj.pad_speed == info->WALKSPEED) {
        plr->obj.anim.newaction = 0x74;
    } else {
        plr->obj.anim.newaction = 0x3A;
    }

    UpdateChaseRunAnim(plr);

    action = plr->obj.anim.newaction;
    if ((action == 2) || (action == 0x43) || (action == 4)) {
        plr->crouch_pos = info->CROUCHINGFRAMES;
    } else if ((plr->obj.pad_speed != 0.0f) || ((action != 3) && (action != 5))) {
        plr->crouch_pos = 0;
    }
    UpdateCharacterIdle(plr, 0);

    if ((plr->obj.dead == 0) && (plr->spin != 0) &&
        (plr->spin_frame < plr->spin_frames - plr->minfo->SPINRESETFRAMES)) {
        pos.x = plr->obj.pos.x;
        pos.y = plr->obj.pos.y + (plr->obj.bot + plr->obj.top) * plr->obj.SCALE * 0.5f +
                (plr->obj.top - plr->obj.bot) * plr->obj.SCALE *
                    (qrand() * (1.0f / 65535.0f) - 0.5f);
        pos.z = plr->obj.pos.z;
        AddGameDebrisRot(0xB, &pos, 1, 0x4000, qrand());
        AddGameDebrisRot(0xC, &plr->obj.pos, 1, 0x2000, qrand());
    }
}

void AnimateCOCO(struct creature_s *plr) {
    struct MoveInfo *info = &CocoMoveInfo;

    if ((u32)plr->obj.dead >= 2) {
        plr->obj.anim.newaction = plr->obj.die_action;
    } else if ((Level == 0x25) && (tumble_time < tumble_duration) &&
               (last_hub != -1)) {
        plr->obj.anim.newaction = tumble_action;
    } else if ((plr->spin != 0) &&
               (plr->spin_frame <
                plr->spin_frames - plr->minfo->SPINRESETFRAMES)) {
        plr->obj.anim.newaction = 0x46;
    } else if (plr->slam_wait != 0) {
        plr->obj.anim.newaction = 0x1F;
    } else if (plr->slam == 1) {
        plr->obj.anim.newaction = 0x41;
    } else if (plr->slide != 0) {
        plr->obj.anim.newaction = 0x43;
    } else if ((plr->jump != 0) &&
               ((plr->obj.ground == 0) || (plr->jump_frame < plr->jump_frames))) {
        if (plr->somersault != 0) {
            plr->obj.anim.newaction = 0x44;
        } else if (plr->jump_type == 4) {
            plr->obj.anim.newaction = 0x19;
        } else if (plr->jump_type == 2) {
            plr->obj.anim.newaction = 0x2D;
        } else {
            plr->obj.anim.newaction = 0x2C;
        }
    } else if ((plr->land != 0) && (plr->obj.pad_speed == 0.0f)) {
        if (plr->jump_type == 2) {
            plr->obj.anim.newaction = 0x31;
        } else {
            plr->obj.anim.newaction = 0x30;
        }
    } else if (((plr->obj.ground != 0) || (plr->allow_jump != 0)) &&
               (TerSurface[plr->obj.surface_type].flags & 0x20) &&
               ((plr->obj.pad_speed != 0.0f) ||
                (plr->obj.xz_distance >= info->IDLESPEED))) {
        plr->obj.anim.newaction = 0x40;
    } else if ((plr->obj.ground == 0) && (plr->allow_jump == 0) &&
               (plr->obj.mom.y < -FALLONCRATEBREAKSPEED)) {
        plr->obj.anim.newaction = 0x19;
    } else if ((plr->obj.pad_speed == 0.0f) &&
               !((plr->pad_type == 1) &&
                 (plr->obj.mom.x * plr->obj.mom.x + plr->obj.mom.z * plr->obj.mom.z >=
                  plr->minfo->IDLESPEED * plr->minfo->IDLESPEED))) {
        plr->obj.anim.newaction = 0x22;
    } else if (plr->sprint != 0) {
        plr->obj.anim.newaction = 0x48;
    } else if (plr->obj.pad_speed == info->WALKSPEED) {
        plr->obj.anim.newaction = 0x74;
    } else {
        plr->obj.anim.newaction = 0x3A;
    }
    UpdateCharacterIdle(plr, 1);
}

void AnimateFIREENGINE(struct creature_s *plr) {
    if ((u32)plr->obj.dead >= 2) {
        plr->obj.anim.newaction = plr->obj.die_action;
    } else if ((Level == 0x16) && (FireBossHoldPlayer != 0)) {
        plr->obj.anim.newaction = 0x62;
    } else if ((plr->jump != 0) &&
               ((plr->obj.ground == 0) || (plr->jump_frame < plr->jump_frames))) {
        if (plr->jump_type == 2) {
            if (plr->obj.anim.anim_time >= FIREENGINESTANDINGJUMPPAUSEFRAME) {
                plr->obj.anim.anim_time = FIREENGINESTANDINGJUMPPAUSEFRAME;
            }
            plr->obj.anim.newaction = 0x64;
        } else {
            if (plr->obj.anim.anim_time >= FIREENGINERUNNINGJUMPPAUSEFRAME) {
                plr->obj.anim.anim_time = FIREENGINERUNNINGJUMPPAUSEFRAME;
            }
            plr->obj.anim.newaction = 0x63;
        }
    } else if (plr->land != 0) {
        if (plr->jump_type == 2) {
            plr->obj.anim.newaction = 0x64;
        } else {
            plr->obj.anim.newaction = 0x63;
        }
    } else {
        plr->obj.anim.newaction = 0x68;
    }
    UpdateCharacterIdle(plr, 0);
}

void AnimateGYRO(struct creature_s *plr, struct pad_s *pad) {
    s32 held = pad->paddata & 0x60;

    if ((plr->spin != 0) &&
        (plr->spin_frame <
         plr->spin_frames - plr->minfo->SPINRESETFRAMES)) {
        plr->obj.anim.newaction = 0x69;
    } else if (plr->tap != 0) {
        plr->obj.anim.newaction = 0x67;
    } else if ((held == 0x40) || (held == 0x20)) {
        plr->obj.anim.newaction = 0x70;
    } else if (plr->obj.pad_speed > 0.0f) {
        if (0xC000u < (u16)(plr->obj.pad_angle - 0x2000)) {
            plr->obj.anim.newaction = 0x5A;
        } else if ((u16)(plr->obj.pad_angle - 0x6001) < 0x3FFFu) {
            plr->obj.anim.newaction = 0x70;
        } else if ((s16)plr->obj.pad_angle >= 0) {
            plr->obj.anim.newaction = (plr->obj.direction == 0) ? 0x67 : 0x65;
        } else {
            plr->obj.anim.newaction = (plr->obj.direction == 0) ? 0x65 : 0x67;
        }
    } else {
        plr->obj.anim.newaction = 0x62;
    }
    UpdateCharacterIdle(plr, 0);
}

void DrawCarpet(void) {
    struct numtx_s m;
    struct numtx_s mtx[256];
    struct nuvec_s scale;
    f32 **dwa;

    if (carpet_model != 0) {
        SetNearestLights(&carpet_lights);
        scale.x = scale.y = scale.z = CData[carpet_character].scale * 0.5f;
        NuMtxSetScale(&m, &scale);
        NuMtxRotateX(&m, carpet_xrot);
        NuMtxRotateY(&m, carpet_yrot + 0x8000);
        NuMtxTranslate(&m, &carpet_pos);
        EvalModelAnim(carpet_model, &carpet_anim, &m, mtx, &dwa, 0);
        NuHGobjRndrMtxDwa(carpet_model->obj, &m, 1, 0, mtx, dwa);
        SetLevelLights();
    }
}

extern s32 RINGCOUNT;
extern s32 i_ring;
extern u16 ring_xrot[];
extern u16 ring_yrot[];
extern s32 Draw3DObject(s32 object, struct nuvec_s *pos, u16 xrot, u16 yrot, u16 zrot,
                        f32 scalex, f32 scaley, f32 scalez,
                        struct nugscn_s *scn, struct nuspecial_s *obj, s32 rot);

extern struct game_s Game;
extern s32 TimeTrial;
extern void PickupBonusGem(s32 mask);

/* state=asm, but byte-exact (compile-diff 400/400) since game_s uses the full
 * game.c layout: retail hoists &Game.level[5] (Game+0x8C) and reads flags at
 * +0x28. Not promoted yet. */
void CheckGates(struct obj_s *obj) {
    struct nugspline_s *spl;
    struct nuvec_s *p0;
    struct nuvec_s *p1;
    s32 i;
    s32 cross;
    s32 sfx;

    if (TimeTrial == 0 && SplTab[28].spl != 0) {
        spl = SplTab[28].spl;
        for (i = 0; i < GATECOUNT; i++) {
            if ((gate_bits & (1 << i)) == 0) {
                p1 = (struct nuvec_s *)(spl->pts + i * 2 * spl->ptsize);
                p0 = (struct nuvec_s *)(spl->pts + (i * 2 + 1) * spl->ptsize);
                cross = LineCrossed(obj->oldpos.x, obj->oldpos.z, obj->pos.x, obj->pos.z,
                                    p1->x, p1->z, p0->x, p0->z);
                if (cross != 0) {
                    NewBuzz(&player->rumble, 5);
                }
                if (cross == 2) {
                    sfx = 0x36;
                    gate_bits |= 1 << i;
                    plr_gates++;
                    if (plr_gates == GATECOUNT) {
                        if (Level == 5 && (Game.level[Level].flags & 0x20) == 0) {
                            PickupBonusGem(4);
                        }
                    }
                } else if (cross == 1) {
                    sfx = 0x3C;
                } else {
                    sfx = -1;
                }
                if (sfx != -1) {
                    GameSfx(sfx, 0);
                }
            }
        }
    }
}

extern struct nuvec_s ring_pos;

void ResetRings(void) {
    struct nuvec_s *pos;
    struct nuvec_s v;
    s32 i;

    RINGCOUNT = 0;
    if (SplTab[64].spl != 0 && ObjTab[0x67].obj.special != 0 && ObjTab[0x68].obj.special != 0) {
        RINGCOUNT = SplTab[64].spl->len / 2;
        if (RINGCOUNT > 0x30) {
            RINGCOUNT = 0x30;
        }
        pos = (struct nuvec_s *)SplTab[64].spl->pts;
        GetALONG(pos, 0, -1, -1, 1);
        if (AheadOfCheckpoint(temp_iRAIL, temp_iALONG, temp_fALONG) != 0) {
            i_ring = 0;
        }
        for (i = 0; i < RINGCOUNT; i++, pos += 2) {
            NuVecSub(&v, pos + 1, pos);
            ring_yrot[i] = NuAtan2D(v.x, v.z);
            ring_xrot[i] = -NuAtan2D(v.y, NuFsqrt(v.x * v.x + v.z * v.z));
            if (i == i_ring) {
                ring_pos = *pos;
            }
        }
    }
}

void DrawRings(void) {
    struct nugspline_s *spl;
    struct nuvec_s *pos;
    s32 obj;
    s32 i;

    if (RINGCOUNT != 0) {
        spl = SplTab[64].spl;
        for (i = 0; i < RINGCOUNT; i++) {
            pos = (struct nuvec_s *)(spl->pts + i * 2 * spl->ptsize);
            if (i == RINGCOUNT - 1 && i_ring == RINGCOUNT) {
                obj = 0x69;
            } else if (i == i_ring) {
                if (i == 0 || i == RINGCOUNT - 1) {
                    obj = 0x69;
                } else {
                    obj = 0x68;
                }
            } else {
                obj = 0x67;
            }
            /* (ObjTab + obj), not ObjTab[obj]: the index form changes codegen. */
            Draw3DObject(obj, pos, ring_xrot[i], ring_yrot[i], 0, 1.0f, 1.0f, 1.0f,
                         (ObjTab + obj)->obj.scene, (ObjTab + obj)->obj.special, 0);
        }
    }
}

extern struct MoveInfo MechMoveInfo;
extern f32 MECHSTANDINGJUMPPAUSEFRAME;
extern f32 MECHRUNNINGJUMPPAUSEFRAME;

void AnimateMECH(struct creature_s *plr) {
    struct MoveInfo *minfo;

    minfo = &MechMoveInfo;
    if ((u32)plr->obj.dead > 1) {
        plr->obj.anim.newaction = plr->obj.die_action;
    } else if (plr->fire != 0) {
        plr->obj.anim.newaction = plr->fire_action;
    } else if (plr->target != 0) {
        if (plr->fire_action == 0x5D) {
            plr->obj.anim.newaction = 0x6C;
        } else if (plr->fire_action == 0x5F) {
            plr->obj.anim.newaction = 0x6E;
        } else if (plr->fire_action == 0x60) {
            plr->obj.anim.newaction = 0x6F;
        } else {
            plr->obj.anim.newaction = 0x6B;
        }
    } else if (plr->jump != 0 && (plr->obj.ground == 0 || plr->jump_frame < plr->jump_frames)) {
        if (plr->jump_type == 2) {
            if (plr->obj.anim.anim_time >= MECHSTANDINGJUMPPAUSEFRAME) {
                plr->obj.anim.anim_time = MECHSTANDINGJUMPPAUSEFRAME;
            }
            plr->obj.anim.newaction = 100;
        } else {
            if (plr->obj.anim.anim_time >= MECHRUNNINGJUMPPAUSEFRAME) {
                plr->obj.anim.anim_time = MECHRUNNINGJUMPPAUSEFRAME;
            }
            plr->obj.anim.newaction = 99;
        }
    } else if (plr->land != 0) {
        if (plr->jump_type == 2) {
            plr->obj.anim.newaction = 100;
        } else {
            plr->obj.anim.newaction = 99;
        }
    } else if (plr->obj.pad_speed == 0.0f
               && (plr->pad_type != 1
                   || !(plr->obj.mom.x * plr->obj.mom.x + plr->obj.mom.z * plr->obj.mom.z
                        >= plr->minfo->IDLESPEED * plr->minfo->IDLESPEED))) {
        plr->obj.anim.newaction = 0x62;
    } else if (plr->obj.pad_speed == minfo->WALKSPEED) {
        plr->obj.anim.newaction = 0x71;
    } else {
        plr->obj.anim.newaction = 0x68;
    }
    UpdateCharacterIdle(plr, 0);
}

extern f32 SNOWBOARDJUMPPAUSEFRAME;

void AnimateSNOWBOARD(struct creature_s *plr) {
    s32 d;

    if (best_cRPos != 0) {
        d = RotDiff(best_cRPos->angle, plr->obj.hdg);
    } else {
        d = 0;
    }
    if ((u32)plr->obj.dead > 1) {
        plr->obj.anim.newaction = plr->obj.die_action;
    } else if (plr->jump != 0 && (plr->obj.ground == 0 || plr->jump_frame < plr->jump_frames)) {
        if (plr->obj.anim.anim_time >= SNOWBOARDJUMPPAUSEFRAME) {
            plr->obj.anim.anim_time = SNOWBOARDJUMPPAUSEFRAME;
        }
        plr->obj.anim.newaction = 0x63;
    } else if (plr->land != 0) {
        plr->obj.anim.newaction = 0x63;
    } else if (plr->sprint != 0) {
        plr->obj.anim.newaction = 0x6A;
    } else if (d > 0x1000) {
        plr->obj.anim.newaction = 0x65;
    } else if (d < -0x1000) {
        plr->obj.anim.newaction = 0x67;
    } else {
        plr->obj.anim.newaction = 0x68;
    }
    UpdateCharacterIdle(plr, 1);
    /* Snow spray from the board's locators. */
    if (plr->obj.anim.newaction == 0x68 || plr->obj.anim.newaction == 0x6A || plr->land != 0) {
        AddGameDebrisRot(0x53, (struct nuvec_s *)&plr->mtxLOCATOR[1][0]._30, 1, 0, 0);
        AddGameDebrisRot(0x53, (struct nuvec_s *)&plr->mtxLOCATOR[1][1]._30, 1, 0, 0);
        AddGameDebrisRot(0x54, (struct nuvec_s *)&plr->mtxLOCATOR[1][2]._30, 2, 0, 0);
    } else if (plr->obj.anim.newaction == 0x65) {
        AddGameDebrisRot(0x55, (struct nuvec_s *)&plr->mtxLOCATOR[1][4]._30, 3, 0, 0);
    } else if (plr->obj.anim.newaction == 0x67) {
        AddGameDebrisRot(0x55, (struct nuvec_s *)&plr->mtxLOCATOR[1][3]._30, 3, 0, 0);
    }
}

void AnimateSWIMMING(struct creature_s *plr) {
    struct MoveInfo *info = &SwimmingMoveInfo;
    u64 d;

    if ((u32)plr->obj.dead >= 2) {
        plr->obj.anim.newaction = plr->obj.die_action;
    } else if ((plr->spin != 0) &&
               (plr->spin_frame <
                plr->spin_frames - plr->minfo->SPINRESETFRAMES)) {
        plr->obj.anim.newaction = 0x46;
    } else if (plr->obj.pad_speed > 0.0f) {
        plr->obj.anim.newaction = 0x4C;
    } else if (plr->tap != 0) {
        plr->obj.anim.newaction = 0x4C;
    } else {
        d = fptodp(plr->obj.mom.z);
        if (dpcmp(d, 0) < 0) {
            d = dpsub(0, d);
        }
        if (dpcmp(d, fptodp(info->WALKSPEED)) > 0) {
            plr->obj.anim.newaction = 0x4C;
        } else {
            plr->obj.anim.newaction = 0x22;
        }
    }
    UpdateCharacterIdle(plr, 0x73);
}

void AnimateSCOOTER(struct creature_s *plr) {
    if ((u32)plr->obj.dead >= 2) {
        plr->obj.anim.newaction = plr->obj.die_action;
    } else if ((plr->jump != 0) &&
               ((plr->obj.ground == 0) || (plr->jump_frame < plr->jump_frames))) {
        if (plr->obj.anim.anim_time >= SCOOTERJUMPPAUSEFRAME) {
            plr->obj.anim.anim_time = SCOOTERJUMPPAUSEFRAME;
        }
        plr->obj.anim.newaction = 0x63;
    } else if (plr->land != 0) {
        plr->obj.anim.newaction = 0x63;
    } else if (plr->sprint != 0) {
        plr->obj.anim.newaction = 0x6A;
    } else {
        plr->obj.anim.newaction = 0x68;
    }
    UpdateCharacterIdle(plr, 1);
}

void AnimateOFFROADER(struct creature_s *plr) {
    s32 d;

    if (best_cRPos != 0) {
        d = RotDiff(best_cRPos->angle, plr->obj.hdg);
    } else {
        d = 0;
    }
    if ((u32)plr->obj.dead >= 2) {
        plr->obj.anim.newaction = plr->obj.die_action;
    } else {
        if ((Level == 3) &&
            ((Pad[0] == 0) || ((Pad[0]->paddata & 0x60) == 0) ||
             (SmokeyCountDownValue > 0)) &&
            (plr->obj.xz_distance < D_0062E844)) {
            plr->obj.anim.newaction = 0x62;
        } else if (d >= 0x801) {
            plr->obj.anim.newaction = 0x67;
        } else if (d < -0x800) {
            plr->obj.anim.newaction = 0x65;
        } else {
            plr->obj.anim.newaction = 0x68;
        }
    }
    UpdateCharacterIdle(plr, 0);
}

void AnimateSUBMARINE(struct creature_s *plr) {
    plr->obj.anim.newaction = 0x62;
    UpdateCharacterIdle(plr, 0);
    AddGameDebrisRot(0x56, (struct nuvec_s *)&plr->mtxLOCATOR[2][0]._30, 1, 0, 0);
    AddGameDebrisRot(0x56, (struct nuvec_s *)&plr->mtxLOCATOR[3][0]._30, 1, 0, 0);
}

void AnimateMINECART(struct creature_s *plr) {
    if ((u32)plr->obj.dead > 1) {
        plr->obj.anim.newaction = plr->obj.die_action;
    } else {
        if (plr->jump != 0) {
            if (plr->jump_frame >= plr->jump_frames) {
                plr->obj.anim.newaction = 0x68;
            } else {
                plr->obj.anim.newaction = 99;
            }
        } else {
            plr->obj.anim.newaction = 0x68;
        }
    }
    UpdateCharacterIdle(plr, 0);
}

void AnimateGLIDER(struct creature_s *plr) {
    if (FlyingLevelVictoryDance != 0) {
        if (plr->obj.anim.newaction != 0x75) {
            plr->obj.anim.oldaction = plr->obj.anim.action;
        }
        plr->obj.anim.newaction = 0x75;
    } else {
        if ((plr->obj.pad_speed > 0.0f) && (plr->obj.pad_angle > 0x9555) &&
            (plr->obj.pad_angle < 0xeaab)) {
            plr->obj.anim.newaction = 0x65;
        } else {
            if ((plr->obj.pad_speed > 0.0f) && (plr->obj.pad_angle > 0x1555)) {
                if (plr->obj.pad_angle > 0x6aaa) {
                    plr->obj.anim.newaction = 0x62;
                } else {
                    plr->obj.anim.newaction = 0x67;
                }
            } else {
                plr->obj.anim.newaction = 0x62;
            }
        }
    }
    UpdateCharacterIdle(plr, 0);
}

void AnimateDROPSHIP(struct creature_s *plr) {
    if (FlyingLevelVictoryDance != 0) {
        if (plr->obj.anim.newaction != 0x75) {
            plr->obj.anim.oldaction = plr->obj.anim.action;
        }
        plr->obj.anim.newaction = 0x75;
    } else {
        plr->obj.anim.newaction = 0x62;
    }
    UpdateCharacterIdle(plr, 1);
}

void AnimateATLASPHERE(struct creature_s *plr) {
    if ((u32)plr->obj.dead > 1) {
        plr->obj.anim.newaction = plr->obj.die_action;
    } else {
        if ((plr->obj.pad_speed == 0.0f) &&
            (plr->obj.xz_distance < 0.005f)) {
            plr->obj.anim.newaction = 0x22;
        } else {
            if (plr->obj.xz_distance < CrashMoveInfo.WALKSPEED) {
                plr->obj.anim.newaction = 0x71;
            } else {
                plr->obj.anim.newaction = 0x68;
            }
        }
    }
    UpdateCharacterIdle(plr, 0);
}

void AnimateJEEP(struct creature_s *plr) {
    if ((u32)plr->obj.dead < 2) {
        plr->obj.anim.newaction = 0x68;
    } else {
        plr->obj.anim.newaction = plr->obj.die_action;
    }
    UpdateCharacterIdle(plr, 0);
}

void AnimateMOSQUITO(struct creature_s *plr) {
    if (FlyingLevelVictoryDance != 0) {
        if (plr->obj.anim.newaction != 0x75) {
            plr->obj.anim.oldaction = plr->obj.anim.action;
        }
        plr->obj.anim.newaction = 0x75;
    } else {
        plr->obj.anim.newaction = 0x62;
    }
    UpdateCharacterIdle(plr, 0);
}

void ResetGates(void) {
    struct nugspline_s *spl = SplTab[28].spl;
    struct nuvec_s *first;
    struct nuvec_s *second;
    struct nuvec_s pos;

    GATECOUNT = 0;
    if (spl != 0) {
        GATECOUNT = spl->len / 2;
        if (GATECOUNT > 0x40) {
            GATECOUNT = 0x40;
        }
        first = (struct nuvec_s *)spl->pts;
        second = (struct nuvec_s *)(spl->pts + spl->ptsize);
        pos.x = (first->x + second->x) * 0.5f;
        pos.y = (first->y + second->y) * 0.5f;
        pos.z = (first->z + second->z) * 0.5f;
        GetALONG(&pos, 0, -1, -1, 1);
        if (AheadOfCheckpoint(temp_iRAIL, temp_iALONG, temp_fALONG) != 0) {
            plr_gates = 0;
            gate_bits = 0;
            if (Level == 5) {
                bonusgem_ok = 0;
            }
        }
    }
}

void CheckPlayerEvents(struct obj_s *obj) {
    struct plrevent_s *ev;
    struct nuvec_s *p0;
    struct nuvec_s *p1;
    s32 i;
    s32 j;
    s32 started;

    if (world_scene[0] != 0) {
        ev = PlrEvent;
        for (i = 0; i < 8; i++, ev++) {
            if (ev->spl != 0) {
                p0 = (struct nuvec_s *)ev->spl->pts;
                p1 = (struct nuvec_s *)(ev->spl->pts + ev->spl->ptsize);
                if (ev->triggered == 0 &&
                    LineCrossed(obj->oldpos.x, obj->oldpos.z, obj->pos.x, obj->pos.z,
                                p0->x, p0->z, p1->x, p1->z) == 2) {
                    ev->triggered = 1;
                    started = 0;
                    for (j = 0; j < 16; j++) {
                        if (StartHGobjAnim(&ev->obj[j]) != 0) {
                            started++;
                        }
                    }
                    if (started != 0) {
                        JudderGameCamera(&GameCam, 0.5f, 0);
                        GameSfx(0x3B, 0);
                    }
                }
            }
        }
    }
}

void ResetPlayerEvents(void) {
    struct plrevent_s *ev;
    s32 i;
    s32 j;

    if (world_scene[0] != 0) {
        ev = PlrEvent;
        for (i = 0; i < 8; i++, ev++) {
            if ((ev->spl != 0) &&
                (AheadOfCheckpoint(ev->iRAIL, ev->iALONG, ev->fALONG) != 0)) {
                for (j = 0; j < 16; j++) {
                    ResetHGobjAnim(&ev->obj[j]);
                    ev->triggered = 0;
                }
            }
        }
    }
}

void UpdateArrow(struct nuvec_s *target, struct nuvec_s *origin) {
    struct nuvec_s d;
    u16 xrot;
    u16 yrot;

    NuVecSub(&d, target, origin);
    xrot = -NuAtan2D(d.y, NuFsqrt(d.x * d.x + d.z * d.z));
    yrot = NuAtan2D(d.x, d.z);
    if (GameTimer.frame != 0) {
        arrow_xrot = SeekRot(arrow_xrot, xrot, 2);
        arrow_yrot = SeekRot(arrow_yrot, yrot, 2);
    } else {
        arrow_xrot = xrot;
        arrow_yrot = yrot;
    }
}

void DrawPanel3DArrow(void) {
    u16 xrot;
    u16 yrot;

    if (ObjTab[0x6C].obj.special != 0) {
        xrot = arrow_xrot;
        yrot = RotDiff(GameCam.yrot, arrow_yrot);
        DrawPanel3DObject(0x6C, 0.0f, 0.5f, 1.0f, 0.125f, 0.125f, 0.125f, xrot,
                          yrot, 0, ObjTab[0x6C].obj.scene,
                          ObjTab[0x6C].obj.special, 0);
    }
}

void AnimateDIVE(struct creature_s *plr, f32 ratio) {
    if (ratio < 0.333f) {
        plr->obj.anim.newaction = 0x2c;
        return;
    }
    plr->obj.anim.newaction = 0x44;
}

inline void ResetTub(struct tubinfo_s *tub, struct creature_s *c) {
    tub->c = c;
    tub->track = -1;
    tub->count = -1;
    tub->wait = 0;
    tub->laps = -1;
    tub->finished = 0;
    tub->stall = 0;
    tub->boost = 0;
}

extern struct tubinfo_s PlrTub;
extern struct tubinfo_s OppTub;
extern struct creature_s OppTubCreature;
extern void ResetLights(struct Nearest_Light_s *lights);
extern s32 opptub_action;
extern s32 opptub_wait;

/* Puts the player and the rival tub at the start, the rival halfway
   between the two start splines. */
void ResetTubs(void) {
    struct CharacterData *cdata;
    struct nuvec_s *p0;
    struct nuvec_s *p1;
    struct creature_s *c;
    s32 i;

    ResetTub(&PlrTub, player);
    c = &OppTubCreature;
    ResetTub(&OppTub, c);
    memset(c, 0, sizeof(struct creature_s));
    ResetAnimPacket(&c->obj.anim, 0x62);
    c->obj.character = 0x7F;
    c->obj.vehicle = (CRemap[0xA1] != -1) ? 0xA1 : -1;
    c->obj.reflect_y = 2000000.0f;
    c->obj.model = (CRemap[0x7F] != -1) ? &CModel[CRemap[0x7F]] : 0;
    cdata = &CData[c->obj.character];
    c->obj.RADIUS = cdata->radius;
    c->obj.radius = cdata->radius;
    c->obj.min = cdata->min;
    c->obj.max = cdata->max;
    c->obj.SCALE = 0.77f;
    c->obj.scale = 0.77f;
    c->obj.bot = c->obj.min.y;
    c->obj.top = c->obj.max.y;
    c->used = 1;
    c->on = 1;
    ResetLights(&c->lights);
    OppTub.c = c;
    if (SplTab[30].spl != 0 && SplTab[46].spl != 0) {
        p0 = (struct nuvec_s *)SplTab[30].spl->pts;
        p1 = (struct nuvec_s *)SplTab[46].spl->pts;
        c->obj.pos.x = (p0->x + p1->x) * 0.5f;
        c->obj.pos.y = (p0->y + p1->y) * 0.5f;
        c->obj.pos.z = (p0->z + p1->z) * 0.5f;
    }
    opptub_action = 0x62;
    i = qrand() * 50;
    opptub_wait = i / 0x10000 + 50;
}
