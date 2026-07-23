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
extern struct nuvec_s D_006B75D0[];
extern void NuRndrLine3dDbg(s32 colour, f32 x0, f32 y0, f32 z0, f32 x1, f32 y1,
                            f32 z1);
/* Earlier in this TU, still asm. */
extern f32 ASin360f(f32 x);

struct MYDRAW {
    struct anim_s Anim;               /* 0x00 */
    struct CharacterModel *model;     /* 0x1C */
    s32 index;                        /* 0x20 */
    s32 numjoints;                    /* 0x24 */
    void *jointlist;                  /* 0x28 */
    s32 field2C;                      /* 0x2C */
    struct Nearest_Light_s lights;    /* 0x30 */
};

extern s32 ChrisJointOveride;
extern s32 ChrisNumJoints;
extern void *ChrisJointList;

extern void ResetLights(struct Nearest_Light_s *nl);
extern void SetNearestLights(struct Nearest_Light_s *l);
extern void GetLights(struct nuvec_s *pos, struct Nearest_Light_s *lights,
                      s32 mode);
extern void UpdateAnimPacket(struct CharacterModel *mod, struct anim_s *anim,
                             f32 dt, f32 xz_distance);


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

/* Retail .rodata doubles (far from gp): 360.0 and 180.0, loaded via ld. */
extern double D_0061FA38[]; /* 360.0 */
extern double D_0061FA40[]; /* 180.0 */

f32 Rationalise360f(f32 a) {
    a = (f32)(fmod(a + 180.0f, D_0061FA38[0]) - D_0061FA40[0]);
    if (a < -180.0f) {
        a += 360.0f;
    }
    return a;
}

f32 ASin360f(f32 val) {
    f32 sign;
    s32 hi, lo, mid, step;
    f32 t_lo, t_hi, range, frac;
    f32 lo_f, hi_f;

    if (val < 0.0f) {
        val = -val;
        sign = -1.0f;
    } else {
        sign = 1.0f;
    }

    if (val >= 1.0f) {
        return sign * 90.0f;
    }

    hi = 0x4000;
    lo = 0;
    step = 0x2000;

    while (step != 0) {
        mid = lo + step;
        if (NuTrigTable[mid] >= val) {
            hi = mid;
        } else {
            lo = mid;
        }
        step >>= 1;
    }

    t_hi = NuTrigTable[hi];
    t_lo = NuTrigTable[lo];
    lo_f = (f32)lo;
    frac = val - t_lo;
    range = t_hi - t_lo;
    hi_f = (f32)hi;

    if (range > 0.0f) {
        lo_f += (hi_f - lo_f) * frac / range;
    }

    return sign * lo_f / 182.0444489f;
}

s16 ASin(f32 val) {
    s32 positive;
    s32 lo, mid, step;

    positive = (val >= 0.0f);
    if (!positive) {
        val = -val;
    }

    if (val >= 1.0f) {
        return positive ? 0x4000 : -0x4000;
    }

    lo = 0;
    step = 0x2000;
    while (step != 0) {
        mid = lo + step;
        if (NuTrigTable[mid] < val) {
            lo = mid;
        }
        step >>= 1;
    }

    if (positive) {
        return lo;
    }
    return -lo;
}

s16 ACos(f32 val) {
    s32 positive;
    s32 lo, mid, step;
    s32 idx;

    positive = (val >= 0.0f);
    if (!positive) {
        val = -val;
    }

    if (val >= 1.0f) {
        idx = positive ? 0x4000 : -0x4000;
    } else {
        lo = 0;
        step = 0x2000;
        while (step != 0) {
            mid = lo + step;
            if (NuTrigTable[mid] < val) {
                lo = mid;
            }
            step >>= 1;
        }
        idx = positive ? lo : -lo;
    }

    return 0x4000 - idx;
}

extern s32 SHEIGHT;
extern u64 fptodp(f32 value);
extern char D_0061FA18[];
extern void NuFntPrintEx(s32 x, s32 y, s32 colour, char *fmt, ...);

void PrintVec(s32 line, struct nuvec_s *v) {
    NuFntPrintEx(100, SHEIGHT * line / 2, 0, D_0061FA18, fptodp(v->x),
                 fptodp(v->y), fptodp(v->z));
}

f32 DotProduct(struct nuvec_s *A, struct nuvec_s *B) {
    return A->x * B->x + A->y * B->y + A->z * B->z;
}

struct nuvec_s *CrossProduct(struct nuvec_s *dest, struct nuvec_s *A,
                             struct nuvec_s *B) {
    struct nuvec_s result;
    result.x = A->y * B->z - A->z * B->y;
    result.y = A->z * B->x - A->x * B->z;
    result.z = A->x * B->y - A->y * B->x;
    *dest = result;
    return dest;
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

extern void SphereDraw(struct nuvec_s *pos, f32 radius);

void DrawSphere(struct nuvec_s *pos, f32 radius, f32 factor) {
    struct nuvec_s v;
    if (ChrisInTheHouse) {
        v = *pos;
        v.y -= radius * factor;
        SphereDraw(&v, radius);
    }
}

extern f32 D_0062DF34;
extern void NuVecRotateY(struct nuvec_s *dst, struct nuvec_s *src, s32 angle);

void DrawDeformedCross(struct nuvec_s *center, struct nuvec_s *size, s32 colour,
                       f32 angle) {
    s32 i;

    if (ChrisInTheHouse) {
        s32 a = (s32)(angle * D_0062DF34);
        struct nuvec_s p[3] = {
            {size->x, 0.0f, 0.0f},
            {0.0f, size->y, 0.0f},
            {0.0f, 0.0f, size->z},
        };

        for (i = 0; i < 3; i++) {
            NuVecRotateY(&p[i], &p[i], a);
            NuRndrLine3dDbg(colour, center->x - p[i].x, center->y - p[i].y,
                            center->z - p[i].z, center->x + p[i].x,
                            center->y + p[i].y, center->z + p[i].z);
        }
    }
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

    if (halflife == 0.0f) {
        rate = 1.0f;
    } else {
        rate = (f32)(1.0 - 1.0 / pow(2.0, dt / halflife));
    }

    limit = limit * dt;
    change = (target - *dest) * rate;

    if (change > limit) {
        change = limit;
    } else if (change < -limit) {
        change = -limit;
    }

    *dest = *dest + change;
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
    src->x = src->x + diff.x * rate;
    src->y = src->y + diff.y * rate;
    src->z = src->z + diff.z * rate;
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
    rate = rate * diff;
    *dest = *dest + rate;
}

void SeekAngLimHalfLife360f(f32 *dest, f32 target, f32 max_speed, f32 halflife,
                            f32 dt) {
    f32 rate;
    f32 lim;

    lim = max_speed * dt;

    if (halflife == 0.0f) {
        rate = 1.0f;
    } else {
        rate = (f32)(1.0 - 1.0 / pow(2.0, dt / halflife));
    }

    rate = rate * Rationalise360f(target - *dest);

    if (rate > lim) {
        rate = lim;
    }
    if (rate < -lim) {
        rate = -lim;
    }

    *dest = *dest + rate;
}

void SeekAngHalfLife(u16 *dest, s16 target, f32 halflife, f32 dt) {
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
    rate = rate * diff;
    rate = rate + cur;
    *dest = (u16)(s32)rate;
}

/* Retail .rodata doubles (far from gp): 360.0 and 180.0, loaded via ld. */
extern double D_0061FA48[]; /* 360.0 */
extern double D_0061FA50[]; /* 180.0 */

s32 LimitAng360f(f32 *dest, f32 min, f32 max) {
    f32 a;
    s32 result = 0;

    a = (f32)(fmod(*dest + 180.0f, D_0061FA48[0]) - D_0061FA50[0]);
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
    s32 m = rand() % 16384;
    f32 f = (f32)m;
    return f * (1.0f / 16384.0f);
}

f32 frandPN(void) {
    s32 m = rand() % 16384;
    f32 f = (f32)m;
    return f * (1.0f / 8192.0f) - 1.0f;
}

f32 fsign(f32 x) {
    if (x >= 0.0f) {
        return 1.0f;
    }
    return -1.0f;
}

extern f32 D_0062DF40;
extern void NuMtxSetRotationY(struct numtx_s *m, s32 r);
extern void NuHGobjEval(struct NUHGOBJ_s *hobj, s32 nJ, struct NUJOINTANIM_s *pJ,
                        struct numtx_s *tmtx);
extern void NuHGobjEvalAnim(struct NUHGOBJ_s *hobj, struct nuanimdata_s *data,
                            f32 time, s32 nJ, struct NUJOINTANIM_s *pJ,
                            struct numtx_s *tmtx);
extern void NuHGobjPOIMtx(struct NUHGOBJ_s *hobj, u8 i, struct numtx_s *mC,
                          struct numtx_s *tmtx, struct numtx_s *m);

void GetLocatorMtx(struct CharacterModel *model, struct numtx_s *dest,
                   f32 angle) {
    struct numtx_s BaseMat;
    struct numtx_s tmtx[256];
    s32 i;
    struct numtx_s *d;

    if (model == 0) {
        return;
    }

    NuMtxSetRotationY(&BaseMat, (s32)(angle * D_0062DF40));
    NuHGobjEval(model->hobj, 0, 0, tmtx);

    d = dest;
    for (i = 0; i <= 15; i++) {
        if (model->pLOCATOR[i] != 0) {
            NuHGobjPOIMtx(model->hobj, (u8)i, &BaseMat, tmtx, d);
        }
        d++;
    }
}

void GetLocatorMtxMyDraw(struct MYDRAW *Draw, struct numtx_s *dest,
                         struct numtx_s *baseMtx) {
    struct numtx_s tmtx[256];
    struct CharacterModel *model;
    s32 i;
    struct numtx_s *d;

    model = Draw->model;
    if (model == 0) {
        return;
    }

    NuHGobjEvalAnim(model->hobj, model->anmdata[Draw->Anim.action],
                    Draw->Anim.anim_time, 0, 0, tmtx);

    d = dest;
    for (i = 0; i <= 15; i++) {
        if (Draw->model->pLOCATOR[i] != 0) {
            NuHGobjPOIMtx(Draw->model->hobj, (u8)i, baseMtx, tmtx, d);
        }
        d++;
    }
}

s32 MyInitModelNew(struct MYDRAW *Draw, s32 index, s32 action, s32 numjoints,
                   void *jointlist, s32 arg6) {
    s32 remap = CRemap[index];

    if (remap == -1) {
        return 0;
    }

    Draw->model = &CModel[remap];
    Draw->index = index;
    Draw->numjoints = numjoints;
    Draw->jointlist = jointlist;
    Draw->field2C = arg6;
    ResetAnimPacket(&Draw->Anim, action);
    ResetLights(&Draw->lights);
    return 1;
}

s32 MyDrawModelNew(struct MYDRAW *Draw, struct numtx_s *mC,
                   struct numtx_s *loc_mtx) {
    s32 result;

    if (Draw->model == 0) {
        return 0;
    }

    if (Draw->numjoints != 0) {
        ChrisJointOveride = 1;
        ChrisNumJoints = Draw->numjoints;
        ChrisJointList = Draw->jointlist;
    }

    SetNearestLights(&Draw->lights);
    result = DrawCharacterModel(Draw->model, &Draw->Anim, mC, 0, 1, 0, loc_mtx,
                                0, 0);
    ChrisJointOveride = 0;
    return result;
}

void MyAnimateModelNew(struct MYDRAW *Draw, f32 dt) {
    Draw->Anim.oldaction = Draw->Anim.action;
    UpdateAnimPacket(Draw->model, &Draw->Anim, dt, 0.0f);
    GetLights((struct nuvec_s *)Draw->field2C, &Draw->lights, 1);
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

f32 ACos360f(f32 x) {
    return 90.0f - ASin360f(x);
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

struct nuvec_s *SetNuVecPntrA(f32 x, f32 y, f32 z) {
    D_006B75D0[0].x = x;
    D_006B75D0[0].y = y;
    D_006B75D0[0].z = z;
    return &D_006B75D0[0];
}

struct nuquat_s SetNuQuat(f32 x, f32 y, f32 z, f32 w) {
    struct nuquat_s q;
    q.x = x;
    q.y = y;
    q.z = z;
    q.w = w;
    return q;
}

extern void NuVecSub(struct nuvec_s *d, struct nuvec_s *a, struct nuvec_s *b);
extern f32 NuFsqrt(f32 x);

f32 ControlledDist(struct nuvec_s *A, struct nuvec_s *B, s32 control) {
    struct nuvec_s diff;
    f32 dist = 0.0f;
    f32 *p;
    s32 i;

    p = &diff.x;
    NuVecSub(&diff, A, B);

    for (i = 0; i < 3; i++) {
        f32 v = *p;
        if (v > 0.0f) {
            if (control & 1) {
                dist += v * v;
            }
        } else if (v < 0.0f) {
            if (control & 2) {
                dist += v * v;
            }
        }
        p++;
        control >>= 4;
    }

    return NuFsqrt(dist);
}

struct MYSPLINE {
    struct spline_s *Spline;   /* 0x00 verified in FindSplineClosestPointAndDist (lw 0(s2)->PointAlongSpline) */
    f32 Cur;                   /* 0x04 verified (swc1/lwc1 0x4) */
    f32 Nex;                   /* 0x08 verified (lwc1/swc1 0x8) */
    f32 Act;                   /* 0x0C */
    f32 Inc;                   /* 0x10 verified (lwc1 0x10) */
    struct nuvec_s CurPos;     /* 0x14 verified (addiu s2,0x14; sdl/sdr copy) */
    struct nuvec_s NexPos;     /* 0x20 verified (addiu s2,0x20) */
    f32 LookaheadDist;         /* 0x2C */
};

extern void PointAlongSpline(struct spline_s *spl, f32 ratio,
                             struct nuvec_s *out, void *a3, void *a4);

static inline f32 SplineControlledDist(struct nuvec_s *A, struct nuvec_s *B,
                                       s32 control) {
    struct nuvec_s diff;
    f32 dist = 0.0f;
    f32 *p;
    s32 i;

    p = &diff.x;
    NuVecSub(&diff, A, B);

    for (i = 0; i < 3; i++) {
        f32 v = *p;
        if (v > 0.0f) {
            if (control & 1) {
                dist += v * v;
            }
        } else if (v < 0.0f) {
            if (control & 2) {
                dist += v * v;
            }
        }
        p++;
        control >>= 4;
    }

    return NuFsqrt(dist);
}

f32 FindSplineClosestPointAndDist(struct MYSPLINE *Spline, s32 Control,
                                  struct nuvec_s *Point,
                                  struct nuvec_s *TargetPoint, s32 Wrap,
                                  s32 BigLook) {
    f32 dist_cur, dist_nex;

    dist_cur = SplineControlledDist(&Spline->CurPos, Point, Control);
    dist_nex = SplineControlledDist(&Spline->NexPos, Point, Control);

    while (1) {
        if (dist_nex < dist_cur || Spline->Cur == Spline->Nex) {
            if (Spline->Nex == 1.0f && Wrap == 0) {
                break;
            }
        } else {
            break;
        }

        dist_cur = dist_nex;
        Spline->CurPos = Spline->NexPos;
        Spline->Cur = Spline->Nex;
        Spline->Nex = Spline->Nex + Spline->Inc;

        if (Spline->Nex > 1.0f) {
            if (Wrap) {
                Spline->Nex -= 1.0f;
            } else {
                Spline->Nex = 1.0f;
            }
        } else if (Spline->Nex < 0.0f) {
            if (Wrap) {
                Spline->Nex += 1.0f;
            } else {
                Spline->Nex = 0.0f;
            }
        }

        PointAlongSpline(Spline->Spline, Spline->Nex, &Spline->NexPos, 0, 0);
        dist_nex = SplineControlledDist(&Spline->NexPos, Point, Control);
    }

    if (TargetPoint != 0) {
        *TargetPoint = Spline->CurPos;
    }

    return dist_cur;
}
