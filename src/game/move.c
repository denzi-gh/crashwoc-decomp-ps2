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
