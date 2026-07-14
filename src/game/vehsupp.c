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
extern double pow(double, double);
extern double fmod(double, double);
extern f32 NuTrigTable[];
extern s32 ChrisInTheHouse;
extern struct nuvec_s D_006B75C0[];
extern void NuRndrLine3dDbg(s32 colour, f32 x0, f32 y0, f32 z0, f32 x1, f32 y1,
                            f32 z1);

struct MYDRAW {
    struct anim_s Anim;   /* 0x00 */
};


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

f32 Rationalise360f(f32 a) {
    a = (f32)(fmod(a + 180.0f, 360.0) - 180.0);
    if (a < -180.0f) {
        a += 360.0f;
    }
    return a;
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

void DrawCross(struct nuvec_s *pos, s32 colour, f32 size) {
    if (ChrisInTheHouse) {
        NuRndrLine3dDbg(colour, pos->x - size, pos->y, pos->z, pos->x + size,
                        pos->y, pos->z);
        NuRndrLine3dDbg(colour, pos->x, pos->y - size, pos->z, pos->x,
                        pos->y + size, pos->z);
        NuRndrLine3dDbg(colour, pos->x, pos->y, pos->z - size, pos->x, pos->y,
                        pos->z + size);
    }
}

void DrawLine(struct nuvec_s *p0, struct nuvec_s *p1, s32 colour) {
    if (ChrisInTheHouse) {
        NuRndrLine3dDbg(colour, p0->x, p0->y, p0->z, p1->x, p1->y, p1->z);
    }
}

void DrawLineRel(struct nuvec_s *p0, struct nuvec_s *rel, s32 colour) {
    if (ChrisInTheHouse) {
        NuRndrLine3dDbg(colour, p0->x, p0->y, p0->z, p0->x + rel->x,
                        p0->y + rel->y, p0->z + rel->z);
    }
}

void SeekHalfLife(f32 *dest, f32 target, f32 halflife, f32 dt) {
    f32 rate;

    if (halflife == 0.0f) {
        rate = 1.0f;
    } else {
        rate = (f32)(1.0 - 1.0 / pow(2.0, dt / halflife));
    }
    *dest = *dest + (target - *dest) * rate;
}

void SeekHalfLifeLim(f32 *dest, f32 target, f32 limit, f32 halflife, f32 dt) {
    f32 rate;
    f32 change;
    f32 lim;
    f32 cur;

    if (halflife == 0.0f) {
        rate = 1.0f;
    } else {
        rate = (f32)(1.0 - 1.0 / pow(2.0, dt / halflife));
    }

    cur = *dest;
    lim = limit * dt;
    change = (target - cur) * rate;

    if (change > lim) {
        change = lim;
    } else if (change < -lim) {
        change = -lim;
    }

    *dest = cur + change;
}

void SeekHalfLifeNUVEC(struct nuvec_s *src, struct nuvec_s *target, f32 halflife,
                       f32 dt) {
    f32 rate;
    struct nuvec_s diff;

    if (halflife == 0.0f) {
        rate = 1.0f;
    } else {
        rate = (f32)(1.0 - 1.0 / pow(2.0, dt / halflife));
    }

    diff.x = target->x - src->x;
    diff.y = target->y - src->y;
    diff.z = target->z - src->z;
    src->x = diff.x * rate + src->x;
    src->y = diff.y * rate + src->y;
    src->z = diff.z * rate + src->z;
}

void SeekAngHalfLife360f(f32 *dest, f32 target, f32 halflife, f32 dt) {
    f32 rate;
    f32 diff;

    if (halflife == 0.0f) {
        rate = 1.0f;
    } else {
        rate = (f32)(1.0 - 1.0 / pow(2.0, dt / halflife));
    }

    diff = Rationalise360f(target - *dest);
    *dest = rate * diff + *dest;
}

void SeekAngLimHalfLife360f(f32 *dest, f32 target, f32 max_speed, f32 halflife,
                            f32 dt) {
    f32 rate;
    f32 change;
    f32 lim;

    lim = max_speed * dt;

    if (halflife == 0.0f) {
        rate = 1.0f;
    } else {
        rate = (f32)(1.0 - 1.0 / pow(2.0, dt / halflife));
    }

    change = rate * Rationalise360f(target - *dest);

    if (change > lim) {
        change = lim;
    }
    if (change < -lim) {
        change = -lim;
    }

    *dest += change;
}

void SeekAngHalfLife(u16 *dest, u16 target, f32 halflife, f32 dt) {
    f32 rate;
    f32 diff;
    f32 cur;
    f32 result;

    if (halflife == 0.0f) {
        rate = 1.0f;
    } else {
        rate = (f32)(1.0 - 1.0 / pow(2.0, dt / halflife));
    }

    diff = (f32)(s16)(target - *dest);
    cur = (f32)(s16)*dest;
    result = rate * diff + cur;
    *dest = (u16)(s32)result;
}

s32 LimitAng360f(f32 *dest, f32 min, f32 max) {
    f32 a;
    s32 result = 0;

    a = (f32)(fmod(*dest + 180.0f, 360.0) - 180.0);
    if (a < -180.0f) {
        a += 360.0f;
    }

    if (a < min) {
        a = min;
    } else if (a > max) {
        a = max;
    } else {
        result = 1;
    }

    *dest = a;
    return result;
}

f32 Sin360f(f32 a) {
    return NuTrigTable[(u16)(s32)(a * 182.0444489f)];
}

f32 Cos360f(f32 a) {
    return NuTrigTable[(u16)((s32)(a * 182.0444489f) + 0x4000)];
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

void MyResetAnimPacket(struct MYDRAW *Draw, s32 Action) {
    ResetAnimPacket(&Draw->Anim, Action);
    Draw->Anim.flags = 0;
}

void MyChangeAnim(struct MYDRAW *Draw, s32 Action) {
    Draw->Anim.flags = 0;
    Draw->Anim.oldaction = Draw->Anim.action;
    Draw->Anim.newaction = (s16)Action;
}

struct nuvec_s SetNuVec(f32 x, f32 y, f32 z) {
    struct nuvec_s v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

struct nuvec_s *SetNuVecPntr(f32 x, f32 y, f32 z) {
    D_006B75C0[0].x = x;
    D_006B75C0[0].y = y;
    D_006B75C0[0].z = z;
    return &D_006B75C0[0];
}

struct nuquat_s SetNuQuat(f32 x, f32 y, f32 z, f32 w) {
    struct nuquat_s q;
    q.x = x;
    q.y = y;
    q.z = z;
    q.w = w;
    return q;
}
