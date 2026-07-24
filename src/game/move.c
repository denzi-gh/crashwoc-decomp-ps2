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
extern s32 Level;
extern s32 SmokeyCountDownValue;
extern struct RPos_s *best_cRPos;
extern f32 D_0062E844;

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

s32 RotDiff(u16 a, u16 b);


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

void AnimateDIVE(struct creature_s *plr, f32 ratio) {
    if (ratio < 0.333f) {
        plr->obj.anim.newaction = 0x2c;
        return;
    }
    plr->obj.anim.newaction = 0x44;
}
