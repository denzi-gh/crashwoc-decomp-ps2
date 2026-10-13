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

#include "creature.h"

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

struct tubinfo_s {
    struct creature_s *c;   /* 0x00 */
    s8 track;               /* 0x04 */
    s8 count;               /* 0x05 */
    u8 wait;                /* 0x06 */
    s8 laps;                /* 0x07 */
    f32 time;               /* 0x08 */
    f32 duration;           /* 0x0C */
    f32 lap_position;       /* 0x10 */
    s8 place;               /* 0x14 */
    u8 finished;            /* 0x15 */
    u8 finishframes;        /* 0x16 */
    u8 stall;               /* 0x17 */
    u16 old_xrot;           /* 0x18 */
    u16 old_yrot;           /* 0x1A */
    u16 old_zrot;           /* 0x1C */
    u8 boost;               /* 0x1E */
    s8 spark;               /* 0x1F */
};

struct spline_s {
    s16 len;                /* 0x0 */
    s16 ptsize;             /* 0x2 */
    char *name;             /* 0x4 */
    u8 *pts;                /* 0x8 */
};

struct spltab_s {
    struct spline_s *spl;   /* 0x00 */
    u8 pad[0x14];           /* 0x04 */
};

struct plrevent_s {
    struct spline_s *spl;           /* 0x00 */
    struct nuhspecial_s obj[16];    /* 0x04 */
    struct nuvec_s pos;             /* 0x84 */
    u8 triggered;                   /* 0x90 */
    s8 iRAIL;                       /* 0x91 */
    s16 iALONG;                     /* 0x92 */
    f32 fALONG;                     /* 0x94 */
};

struct chaseevent_s {
    struct spline_s *spl;           /* 0x00 */
    struct nuhspecial_s obj[24];    /* 0x04 */
};

struct chase_s {
    struct spline_s *spl_START;             /* 0x0000 */
    f32 time;                               /* 0x0004 */
    f32 duration;                           /* 0x0008 */
    struct spline_s *spl_CHASER[6];         /* 0x000C */
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
    struct spline_s *spl_MISC[6][4];        /* 0x7098 */
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
};

struct tersurface_s {
    f32 friction;           /* 0x0 */
    s16 pad;                /* 0x4 */
    u16 flags;              /* 0x6 */
};

struct objtab_s {
    struct nuhspecial_s obj; /* 0x00 */
    u8 pad[0x18];            /* 0x08 */
};

struct gametimer_s {
    u32 frame;              /* 0x00 */
    u8 pad[0x14];           /* 0x04 */
};

struct cammtx_s {
    struct numtx_s m;       /* 0x00 */
    struct numtx_s m0;      /* 0x40 */
    struct nuvec_s vX;      /* 0x80 */
    struct nuvec_s vY;      /* 0x8C */
    struct nuvec_s vZ;      /* 0x98 */
    struct nuvec_s pos;     /* 0xA4 */
    struct nuvec_s oldpos;  /* 0xB0 */
    struct nuvec_s newpos;  /* 0xBC */
    struct nuvec_s pos2;    /* 0xC8 */
    struct nuvec_s seek;    /* 0xD4 */
    f32 distance;           /* 0xE0 */
    f32 ahead;              /* 0xE4 */
    f32 judder;             /* 0xE8 */
    f32 blend_time;         /* 0xEC */
    f32 blend_duration;     /* 0xF0 */
    s32 xrot;               /* 0xF4 */
    s32 yrot;               /* 0xF8 */
    s32 zrot;               /* 0xFC */
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
extern CharacterData CData[];
extern struct CharacterModel *carpet_model;
extern s32 carpet_character;
extern u16 carpet_xrot;
extern u16 carpet_yrot;
extern struct nuvec_s carpet_pos;
extern struct anim_s carpet_anim;
extern struct Nearest_Light_s carpet_lights;
extern struct objtab_s ObjTab[];
extern struct gametimer_s GameTimer;
extern struct cammtx_s GameCam;
extern s32 GATECOUNT;
extern s32 plr_gates;
extern u64 gate_bits;
extern s32 bonusgem_ok;
extern s32 temp_iRAIL;
extern s32 temp_iALONG;
extern f32 temp_fALONG;
extern u16 arrow_xrot;
extern u16 arrow_yrot;

s32 RotDiff(u16 a, u16 b);
void AddGameDebrisRot(s32 i, struct nuvec_s *pos, s32 n, u16 xrot, u16 yrot);
void GetALONG(struct nuvec_s *pos, struct RPos_s *rpos, s32 iRAIL, s32 iALONG, s32 dir);
s32 AheadOfCheckpoint(s32 iRAIL, s32 iALONG, f32 fALONG);
void NuVecSub(struct nuvec_s *d, struct nuvec_s *a, struct nuvec_s *b);
f32 NuFsqrt(f32 x);
s32 NuAtan2D(f32 x, f32 z);
u16 SeekRot(u16 a, u16 target, s32 rate);
s32 qrand(void);
s32 abs(s32 x);
s32 ResetHGobjAnim(struct nuhspecial_s *obj);
void SetNearestLights(struct Nearest_Light_s *l);
void SetLevelLights(void);
void NuMtxSetScale(struct numtx_s *m, struct nuvec_s *s);
void NuMtxRotateX(struct numtx_s *m, s32 r);
void NuMtxRotateY(struct numtx_s *m, s32 r);
void NuMtxTranslate(struct numtx_s *m, struct nuvec_s *v);
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
               (plr->crouch_pos < plr->OnFootMoveInfo->CROUCHINGFRAMES) &&
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
                  plr->OnFootMoveInfo->IDLESPEED * plr->OnFootMoveInfo->IDLESPEED))) {
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
        (plr->spin_frame < plr->spin_frames - plr->OnFootMoveInfo->SPINRESETFRAMES)) {
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
                plr->spin_frames - plr->OnFootMoveInfo->SPINRESETFRAMES)) {
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
                  plr->OnFootMoveInfo->IDLESPEED * plr->OnFootMoveInfo->IDLESPEED))) {
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
         plr->spin_frames - plr->OnFootMoveInfo->SPINRESETFRAMES)) {
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
        NuHGobjRndrMtxDwa(carpet_model->hobj, &m, 1, 0, mtx, dwa);
        SetLevelLights();
    }
}

void AnimateSWIMMING(struct creature_s *plr) {
    struct MoveInfo *info = &SwimmingMoveInfo;
    u64 d;

    if ((u32)plr->obj.dead >= 2) {
        plr->obj.anim.newaction = plr->obj.die_action;
    } else if ((plr->spin != 0) &&
               (plr->spin_frame <
                plr->spin_frames - plr->OnFootMoveInfo->SPINRESETFRAMES)) {
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
    struct spline_s *spl = SplTab[28].spl;
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

void ResetTub(struct tubinfo_s *tub, struct creature_s *c) {
    tub->c = c;
    tub->track = -1;
    tub->count = -1;
    tub->wait = 0;
    tub->laps = -1;
    tub->finished = 0;
    tub->stall = 0;
    tub->boost = 0;
}
