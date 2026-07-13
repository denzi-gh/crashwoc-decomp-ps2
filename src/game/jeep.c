/*
 * Unit: game/jeep
 *
 * Fire-boss / jeep support ring (PR-S4-D1).  The 6.2KB brain ProcessFireBoss
 * (0x00228D70) and the western-arena / enemy-jeep / trail machinery remain
 * assembly; this file reconstructs the fire-boss support functions and the
 * balloon projectile system, and types the FireBoss / JeepBalloon layouts
 * (see jeep.h).  FireBossActionName (0x0022C200) stays assembly: it compiles a
 * dense switch to a .rodata jump table the hybrid pipeline cannot own.
 *
 * Functions:
 *   0x002289b0 InitFireBoss          -- the FireBoss layout Rosetta stone
 *   0x00228b10 DrawFireBoss
 *   0x0022a8e0 DrawJeepBalloon
 *   0x0022a9a8 ProcessJeepBalloon    -- the damage site (FireBoss.health -= 1)
 *   0x0022aaf8 FireBossWaterFire
 *   0x0022b1f0 FireBossReset
 *   0x0022b248 DrawFireBossLevelExtra
 *   0x0022b278 ProcessFireBossLevel
 *   0x0022b370 GetTotalFireBossObjectives
 *   0x0022b378 GetCurrentFireBossObjectives
 *   0x0022b718 AddBalloon
 *   0x0022b810 CheckAgainstFireBoss
 *   0x0022c200 FireBossActionName    (asm: rodata jump table)
 *   0x0022c280 InitJeepBalloons
 *   0x0022c2a8 ProcessJeepBalloons
 *   0x0022c310 DrawJeepBalloons
 *   0x0022c378 FindJeepBalloon
 *   0x0022c3b8 BalloonHitFireBoss
 */

#include "jeep.h"

/* nu graphics-object handles (see game/panel.c) */
struct nuinstance_s {
    struct numtx_s matrix;
    s32 objid;
};

struct nugscn_s {
    short *tids;
    s32 numtid;
    void *mtls;
    s32 nummtl;
    s32 numgobj;
    void **gobjs;
};

struct nuspecial_s {
    struct numtx_s mtx;
    struct nuinstance_s *instance;
    char *name;
};

struct objtab_s {
    struct nugscn_s *scene;
    struct nuspecial_s *special;
    u8 _pad[24];
};

/* ------------------------------------------------------------------ */
/* Globals referenced from this unit                                   */
/* ------------------------------------------------------------------ */

extern struct numtx_s mTEMP;
extern struct nuvec_s FIREBOSSSCALE;
extern struct nuvec_s WallOfFirePosition;
extern struct nuvec_s D_005C1428;
extern struct objtab_s ObjTab[201];

extern void *D_00588130[]; /* fire-boss path spline (far from $gp) */
extern s32 D_005C1418[];   /* wall-of-fire objective count (far from $gp) */
extern void *D_0060E650[]; /* shot-debris effect owner (far from $gp) */

extern s32 Level, Paused, WallOfFireOn, FireBossHoldPlayer;
extern s32 ChrisBigBossDead, VEHICLECONTROL;
extern s32 SMASHRUMPOWER, WATERFIRESOUNDVOL, FBSCREAMVOL;
extern s16 SEEKANGSPEED;
extern u16 D_006332E0;

extern float FIREBOSSSTART, D_0062E14C, D_0062E150, D_0062E154, D_0062E158;
extern float D_0062E20C, D_0062E210, D_0062E214, D_0062E218, D_0062E21C;
extern float D_0062E220, D_0062E258, D_0062E25C;
extern float WallOfFireAngleY, WaterTimer, MAXWATERTIME;
extern float WaterLength, WaterWidth;
extern float FIREWATERSOUNDTIME, FireBossWaterSoundTimer, AshesMechOutZ;

/* ------------------------------------------------------------------ */
/* External functions                                                  */
/* ------------------------------------------------------------------ */

extern void *memset(void *dst, s32 c, u32 n);
extern s32 MyInitModelNew(struct mymodel_s *m, s32 character, s32 action,
                          s32 jo, s32 nj, void *jl);
extern s32 MyDrawModelNew(struct mymodel_s *m, struct numtx_s *mtx, void *obj);
extern void PointAlongSpline(void *spline, float t, struct nuvec_s *pos,
                             s16 *ang, s32 flag);
extern void NuVecScale(struct nuvec_s *dst, struct nuvec_s *src, float s);
extern void NuVecAdd(struct nuvec_s *dst, struct nuvec_s *a, struct nuvec_s *b);
extern void NuVecSub(struct nuvec_s *dst, struct nuvec_s *a, struct nuvec_s *b);
extern float NuVecMagSqr(struct nuvec_s *v);
extern float NuFsqrt(float x);
extern s32 NuAtan2D(float x, float z);
extern float DotProduct(struct nuvec_s *a, struct nuvec_s *b);
extern void NuMtxSetScale(struct numtx_s *m, struct nuvec_s *v);
extern void NuMtxSetRotationX(struct numtx_s *m, s32 r);
extern void NuMtxRotateY(struct numtx_s *m, s32 r);
extern void NuMtxTranslate(struct numtx_s *m, struct nuvec_s *v);
extern void NuVecRotateY(struct nuvec_s *dst, struct nuvec_s *src, s32 angle);
extern struct nuvec_s *SetNuVecPntr(float x, float y, float z);
extern void AddVariableShotDebrisEffect(void *owner, struct nuvec_s *pos,
                                        s32 a, s32 b, s32 ang);
extern s32 NuRndrGScnObj(void *gobj, struct numtx_s *m);
extern s32 ProcessTimer(float *t);
extern s32 NewRayCast(void *b, struct nuvec_s *dir);
extern void AddGameDebris(s32 id, void *b);
extern void DrawCross(struct nuvec_s *pos, s32 colour, float size);
extern s32 MyGameSfx(s32 id, struct nuvec_s *pos, s32 vol);

extern void ProcessFireBoss(struct fireboss_s *fb);
extern void InitVehMasks(void);
extern void InitVehMask(s32 a, s32 b);
extern void InitJeepRocks(void);
extern void ProcessJeepRocks(void);
extern void ProcessVehMasks(void);
extern void ProcessRockRockCollisions(void);
extern s32 CheckAgainstRocks(struct nuvec_s *pos, struct nuvec_s *mom);
extern void KillPlayer(struct obj_s *obj, s32 how);
extern void DrawJeepRocks(void);
extern void DrawVehMasks(void);

/* ================================================================== */
/* Fire-boss objectives                                                */
/* ================================================================== */

s32 GetTotalFireBossObjectives(void) {
    return 3;
}

s32 GetCurrentFireBossObjectives(void) {
    return D_005C1418[0];
}

/* ================================================================== */
/* Fire-boss init / reset / per-level plumbing                         */
/* ================================================================== */

void InitFireBoss(struct fireboss_s *fb) {
    struct nuvec_s tmp;
    s16 ang;
    void *spline;

    memset(fb, 0, sizeof(*fb));
    if (!MyInitModelNew(&fb->model, 0x9C, 0x3A, 0, 0, &fb->pos)) {
        return;
    }
    if (!MyInitModelNew(&fb->model_hurt, 0xBD, 0xE, 0, 0, &fb->pos)) {
        return;
    }
    fb->active = 1;
    fb->max_objectives = 4;
    fb->health = 3;
    FireBossHealth = 3;

    spline = D_00588130[0];
    if (!spline) {
        return;
    }
    fb->spline = spline;
    fb->spline_t = FIREBOSSSTART;
    fb->f5F4 = D_0062E14C;
    fb->spline_t2 = FIREBOSSSTART;
    fb->spline_t3 = FIREBOSSSTART;
    PointAlongSpline(spline, FIREBOSSSTART, &tmp, &ang, 0);
    fb->spline_pos = tmp;
    fb->f610 = 3.0f;
    fb->heading = (float)ang / D_0062E150 + 180.0f;
    fb->spline_pos2 = tmp;
    fb->pos = tmp;
    fb->i620 = -1;
    fb->active = 1;
    fb->action = 0;
}

void FireBossReset(void) {
    FireBossFinished = 0;
    FireBossWon = 0;
    ChrisBigBossDead = 0;
    InitVehMasks();
    InitVehMask(0, 0x56);
    InitVehMask(1, 3);
    InitFireBoss(&FireBoss);
    InitJeepRocks();
    VEHICLECONTROL = 0;
    WaterTimer = 0;
}

void DrawFireBossLevelExtra(void) {
    DrawFireBoss(&FireBoss);
    DrawJeepRocks();
    DrawVehMasks();
}

void ProcessFireBossLevel(void) {
    struct nuvec_s d;

    ProcessFireBoss(&FireBoss);
    ProcessJeepRocks();
    ProcessVehMasks();
    ProcessRockRockCollisions();

    NuVecSub(&d, &player->obj.pos, &player->obj.oldpos);
    if (CheckAgainstRocks(&player->obj.pos, &d)) {
        NewRumble(&player->rumble, SMASHRUMPOWER);
        NewBuzz(&player->rumble, 5);
        if (!FireBossHoldPlayer && FireBoss.health > 0) {
            KillPlayer(&player->obj, 0xD);
        }
    }
    if (FireBossHealth <= 0) {
        FireBossWon = 1;
        FireBossFinished = 1;
    }
}

s32 CheckAgainstFireBoss(struct nuvec_s *a, struct nuvec_s *b, float radius) {
    struct nuvec_s p;
    struct nuvec_s d;
    float distsq;

    if (Level != 0x16) {
        return 0;
    }
    if (b) {
        NuVecAdd(&p, a, b);
    } else {
        p = *a;
    }
    NuVecSub(&d, &p, &D_005C1428);
    d.y = 0.0f;
    distsq = NuVecMagSqr(&d);
    if (distsq < radius * radius) {
        if (b) {
            DotProduct(&d, b);
            NuVecScale(&d, &d, radius / NuFsqrt(distsq));
            NuVecAdd(&p, &D_005C1428, &d);
            NuVecSub(b, &p, a);
        }
        return 1;
    }
    return 0;
}

/* ================================================================== */
/* Fire-boss render                                                    */
/* ================================================================== */

void DrawFireBoss(struct fireboss_s *fb) {
    struct mymodel_s *model;
    struct nuvec_s a;
    struct nuvec_s b;
    struct nuvec_s c;

    NuMtxSetScale(&mTEMP, &FIREBOSSSCALE);
    NuMtxRotateY(&mTEMP, (s32)((fb->heading + 180.0f) * D_0062E154));
    NuMtxTranslate(&mTEMP, &fb->pos);
    fb->draw_mtx = mTEMP;

    if (fb->action == 5) {
        model = &fb->model_hurt;
    } else {
        model = &fb->model;
    }
    fb->draw_result = MyDrawModelNew(model, &mTEMP, fb);

    if (WallOfFireOn && D_005C1418[0] > 0) {
        if (!Paused) {
            a = WallOfFirePosition;
            a.y += 0.5f;
            AddVariableShotDebrisEffect(D_0060E650[0], &a, 4, 0,
                                        (s16)(s32)WallOfFireAngleY);
        }
        NuVecRotateY(&a, SetNuVecPntr(4.0f, 0.0f, 0.0f),
                     (s32)(WallOfFireAngleY * D_0062E158));
        NuVecRotateY(&b, SetNuVecPntr(-4.0f, 0.0f, 0.0f),
                     (s32)(WallOfFireAngleY * D_0062E158));
        NuVecAdd(&a, &WallOfFirePosition, &a);
        NuVecAdd(&b, &WallOfFirePosition, &b);
        NuVecSub(&c, &b, &a);
    }
}

/* ================================================================== */
/* Balloon projectiles (JeepBalloon[6])                                */
/* ================================================================== */

void InitJeepBalloons(void) {
    s32 i;

    for (i = 5; i >= 0; i--) {
        JeepBalloon[i].active = 0;
    }
}

struct jeepballoon_s *FindJeepBalloon(void) {
    s32 i;

    for (i = 0; i < 6; i++) {
        if (JeepBalloon[i].active == 0) {
            return &JeepBalloon[i];
        }
    }
    return 0;
}

s32 AddBalloon(struct nuvec_s *pos, struct nuvec_s *vel) {
    struct jeepballoon_s *b;
    s32 i;

    b = 0;
    for (i = 0; i < 6; i++) {
        if (JeepBalloon[i].active == 0) {
            b = &JeepBalloon[i];
            break;
        }
    }
    if (!b) {
        return 0;
    }
    memset(b, 0, sizeof(*b));
    b->active = 1;
    b->vel = *vel;
    b->pos = *pos;
    b->timer = 5.0f;
    b->angle = NuAtan2D(vel->x * 100.0f, vel->z * 100.0f);
    return 1;
}

void ProcessJeepBalloon(struct jeepballoon_s *b) {
    struct nuvec_s dir;

    if (ProcessTimer(&b->timer)) {
        b->hit = 1;
    } else {
        s32 rc;
        s32 fbhit;
        float dx, dz;

        NuVecScale(&dir, &b->vel, D_0062E20C);
        rc = NewRayCast(b, &dir);
        NuVecAdd(&b->pos, &b->pos, &dir);
        b->vel.y -= D_0062E214;
        if (rc) {
            b->hit = 1;
        }

        fbhit = 0;
        dx = FireBoss.pos.x - b->pos.x;
        dz = FireBoss.pos.z - b->pos.z;
        if (dx * dx + dz * dz < D_0062E218) {
            if (FireBoss.pos.y <= b->pos.y &&
                b->pos.y <= FireBoss.pos.y + D_0062E21C) {
                fbhit = 1;
            }
        }
        if (fbhit) {
            b->hit = 1;
            FireBoss.health -= 1;
        }
    }

    if (b->hit) {
        b->active = 0;
        AddGameDebris(0x41, b);
    }
    DrawCross(&b->pos, 0xFFFFFF, 0.5f);
}

void ProcessJeepBalloons(void) {
    struct jeepballoon_s *b;
    s32 i;

    b = JeepBalloon;
    for (i = 5; i >= 0; i--) {
        if (b->active != 0) {
            ProcessJeepBalloon(b);
        }
        b++;
    }
}

void DrawJeepBalloon(struct jeepballoon_s *b) {
    struct objtab_s *e;
    struct nuspecial_s *special;
    s32 pitch;

    pitch = NuAtan2D(b->vel.y * 100.0f,
                     NuFsqrt(b->vel.x * b->vel.x + b->vel.z * b->vel.z) *
                         100.0f);
    NuMtxSetRotationX(&mTEMP, (s16)pitch);
    NuMtxRotateY(&mTEMP, b->angle);
    NuMtxTranslate(&mTEMP, &b->pos);

    b->gscn = 0;
    e = &ObjTab[89];
    special = e->special;
    if (special) {
        struct nuinstance_s *inst = special->instance;
        struct nugscn_s *scn = e->scene;
        b->gscn = NuRndrGScnObj(scn->gobjs[inst->objid], &mTEMP);
    }
}

void DrawJeepBalloons(void) {
    struct jeepballoon_s *b;
    s32 i;

    b = JeepBalloon;
    for (i = 5; i >= 0; i--) {
        if (b->active != 0) {
            DrawJeepBalloon(b);
        }
        b++;
    }
}

s32 BalloonHitFireBoss(struct nuvec_s *pos) {
    float dx = FireBoss.pos.x - pos->x;
    float dz = FireBoss.pos.z - pos->z;

    if (dx * dx + dz * dz < D_0062E258) {
        if (FireBoss.pos.y <= pos->y &&
            pos->y <= FireBoss.pos.y + D_0062E25C) {
            return 1;
        }
    }
    return 0;
}
