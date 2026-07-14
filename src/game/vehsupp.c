/*
 * Unit: game/vehsupp
 *
 * Functions:
 *   0x0021ffa8 FindSplineTargetPoint
 *   0x00220410 FindSplineClosestPointAndDist
 *   0x00220710 ASin360f
 *   0x00220820 GetVolumeI
 *   0x00220850 MyGameSfx
 *   0x00220888 MyGameSfxLoop
 *   0x002208c0 MyGameSfxLoopVolPitch
 *   0x002208f8 ApplyFriction
 *   0x00220950 Rationalise360f
 *   0x002209d0 PrintVec
 *   0x00220a70 DotProduct
 *   0x00220aa0 CrossProduct
 *   0x00220b10 ProcessTimer
 *   0x00220b58 DrawSphere
 *   0x00220ba8 DrawDeformedCross
 *   0x00220d00 DrawCross
 *   0x00220db0 DrawLine
 *   0x00220df0 DrawLineRel
 *   0x00220e40 SeekHalfLife
 *   0x00220ee0 SeekHalfLifeLim
 *   0x00220fc8 SeekHalfLifeNUVEC
 *   0x002210b0 SeekAngHalfLife360f
 *   0x00221168 SeekAngLimHalfLife360f
 *   0x00221250 SeekAngHalfLife
 *   0x00221330 LimitAng360f
 *   0x00221418 Sin360f
 *   0x00221448 Cos360f
 *   0x00221480 frand
 *   0x002214d0 frandPN
 *   0x00221530 fsign
 *   0x00221560 GetLocatorMtx
 *   0x00221620 GetLocatorMtxMyDraw
 *   0x002216d8 MyInitModelNew
 *   0x00221760 MyDrawModelNew
 *   0x002217f8 MyAnimateModelNew
 *   0x00221840 MyResetAnimPacket
 *   0x00221868 MyChangeAnim
 *   0x00221880 ACos360f
 *   0x002218a8 ASin
 *   0x00221950 ACos
 *   0x002219f8 SetNuVec
 *   0x00221a30 SetNuVecPntr
 *   0x00221a50 SetNuVecPntrA
 *   0x00221a70 SetNuQuat
 *   0x00221ab0 ControlledDist
 */

#include "creature.h"

extern s32 Level;
extern s32 Paused;
extern s32 gamesfx_effect_volume;
extern s32 gamesfx_pitch;

extern void GameSfx(s32 Id, struct nuvec_s *Pos);
extern void GameSfxLoop(s32 Id, struct nuvec_s *Pos);
extern int rand(void);


s16 GetVolumeI(f32 vol) {
    s32 v = (s32)(vol * 16383.0f);
    if (v > 0x7FFF) {
        v = 0x7FFF;
    }
    return (s16)v;
}

void MyGameSfx(s32 Id, struct nuvec_s *Pos, s32 Vol) {
    if (Level == 0x15 && Pos != 0) {
        Vol = Vol * 5;
    }
    gamesfx_effect_volume = Vol;
    GameSfx(Id, Pos);
}

void MyGameSfxLoop(s32 Id, struct nuvec_s *Pos, s32 Vol) {
    if (Level == 0x15 && Pos != 0) {
        Vol = Vol * 5;
    }
    gamesfx_effect_volume = Vol;
    GameSfxLoop(Id, Pos);
}

void MyGameSfxLoopVolPitch(s32 Id, struct nuvec_s *Pos, s32 Vol, s32 Pitch) {
    if (Level == 0x15 && Pos != 0) {
        Vol = Vol * 5;
    }
    gamesfx_effect_volume = Vol;
    gamesfx_pitch = Pitch;
    GameSfxLoop(Id, Pos);
}

void ApplyFriction(f32 *val, f32 rate, f32 dt) {
    f32 v = *val;
    f32 f = rate * dt;

    if (v > 0.0f) {
        if (f > v) {
            *val = 0.0f;
            return;
        }
        *val = v - f;
    } else {
        if (f > -v) {
            *val = 0.0f;
            return;
        }
        *val = v + f;
    }
}

f32 DotProduct(struct nuvec_s *A, struct nuvec_s *B) {
    return A->x * B->x + A->y * B->y + A->z * B->z;
}

void CrossProduct(struct nuvec_s *dest, struct nuvec_s *A, struct nuvec_s *B) {
    struct nuvec_s result;
    result.x = A->y * B->z - A->z * B->y;
    result.y = A->z * B->x - A->x * B->z;
    result.z = A->x * B->y - A->y * B->x;
    *dest = result;
}

s32 ProcessTimer(f32 *Timer) {
    if (Paused == 0) {
        *Timer -= 1.0f / 50.0f;
    }
    if (*Timer <= 0.0f) {
        *Timer = 0.0f;
        return 1;
    }
    return 0;
}

f32 frand(void) {
    return (f32)(rand() % 16384) * (1.0f / 16384.0f);
}

f32 frandPN(void) {
    return (f32)(rand() % 16384) * (1.0f / 8192.0f) - 1.0f;
}

f32 fsign(f32 x) {
    if (x >= 0.0f) {
        return 1.0f;
    }
    return -1.0f;
}

struct nuvec_s SetNuVec(f32 x, f32 y, f32 z) {
    struct nuvec_s v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

struct nuquat_s SetNuQuat(f32 x, f32 y, f32 z, f32 w) {
    struct nuquat_s q;
    q.x = x;
    q.y = y;
    q.z = z;
    q.w = w;
    return q;
}
