#ifndef GAMECODE_JEEP_H
#define GAMECODE_JEEP_H

/* A copy for mod builds lives at mods/include/jeep.h -- keep in sync.
 *
 * PS2 PAL v1.03 (SLES_503.86) types for the fire-boss / jeep support ring
 * (unit 101, game/jeep.c).  Struct offsets are anchored in retail asm:
 *   InitFireBoss  0x002289B0  -- the FireBoss layout Rosetta stone
 *   AddBalloon    0x0022B718  -- the JeepBalloon layout
 *   MyInitModelNew 0x002216D8 -- the light-weight model wrapper (0xE0)
 */

#include "creature.h"

/* ------------------------------------------------------------------ */
/* Light-weight animated-model wrapper (MyInitModelNew / MyDrawModelNew) */
/* ------------------------------------------------------------------ */

/* Distinct from CharacterModel (0x988): this is the small render packet the
 * vehicle/boss code embeds directly.  MyInitModelNew writes the CModel pointer
 * at +0x1C, the character id at +0x20 and the joint-override triple at
 * +0x24/+0x28/+0x2C (consumed by MyDrawModelNew as ChrisJointOveride /
 * ChrisNumJoints / ChrisJointList). */
struct mymodel_s {
    struct anim_s anim;             /* 0x00 */
    struct CharacterModel *cmodel;  /* 0x1C */
    s32 character;                  /* 0x20 */
    s32 jointoverride;              /* 0x24 */
    s32 numjoints;                  /* 0x28 */
    void *jointlist;                /* 0x2C */
    struct Nearest_Light_s lights;  /* 0x30 */
}; /* 0xE0 */

/* ------------------------------------------------------------------ */
/* Jeep balloon projectile (JeepBalloon[6], stride 0x30)               */
/* ------------------------------------------------------------------ */

struct jeepballoon_s {
    struct nuvec_s pos;   /* 0x00  world position (AddBalloon arg 0)          */
    struct nuvec_s vel;   /* 0x0C  velocity (AddBalloon arg 1)               */
    s32 active;           /* 0x18  slot in use                               */
    s32 gscn;             /* 0x1C  render object handle (DrawJeepBalloon)    */
    s32 hit;              /* 0x20  set when the balloon should pop this frame */
    s32 unk24;            /* 0x24 */
    float timer;          /* 0x28  lifetime countdown (ProcessTimer, 5.0)    */
    s16 angle;            /* 0x2C  facing yaw (NuAtan2D of vel.x/vel.z)       */
    s16 pad2E;            /* 0x2E */
}; /* 0x30 */

/* ------------------------------------------------------------------ */
/* Fire-boss action ids (FireBossActionName jump table, indices 0..6)  */
/* ------------------------------------------------------------------ */

/* Behaviour-derived names: +0x61C holds the current action.  DrawFireBoss
 * selects the hurt model on action 5; FireBossWaterFire runs on action 4. */
enum fireboss_action {
    FBACT_0 = 0,
    FBACT_1 = 1,
    FBACT_2 = 2,
    FBACT_3 = 3,
    FBACT_WATERFIRE = 4,
    FBACT_HURT = 5,
    FBACT_6 = 6
};

/* ------------------------------------------------------------------ */
/* Fire-boss state block (FireBoss, 0x690 bytes)                       */
/* ------------------------------------------------------------------ */

/* Only the render/health/spline tail (0x408..0x690) is typed here; the brain's
 * working state at 0x000..0x407 is decompiled in PR-S4-D2 (ProcessFireBoss). */
struct fireboss_s {
    char pad_000[0x408];              /* 0x000  brain state (D2)             */
    s32 health;                       /* 0x408  hits remaining (== FireBossHealth) */
    s32 max_objectives;               /* 0x40C  (init 4)                     */
    s32 active;                       /* 0x410  alive flag (init 1)          */
    float heading;                    /* 0x414  render yaw (spline heading)  */
    struct nuvec_s pos;               /* 0x418  position / hit-sphere centre */
    struct mymodel_s model;           /* 0x424  main model                   */
    struct mymodel_s model_hurt;      /* 0x504  hurt model (action 5)        */
    void *spline;                     /* 0x5E4  path spline                  */
    float spline_t;                   /* 0x5E8 */
    float spline_t2;                  /* 0x5EC */
    float spline_t3;                  /* 0x5F0 */
    float f5F4;                       /* 0x5F4 */
    struct nuvec_s spline_pos;        /* 0x5F8  spline start point           */
    struct nuvec_s spline_pos2;       /* 0x604 */
    float f610;                       /* 0x610  (init 3.0)                   */
    s32 draw_result;                  /* 0x614  MyDrawModelNew result        */
    char pad_618[0x61C - 0x618];      /* 0x618 */
    s32 action;                       /* 0x61C  current fireboss_action      */
    s32 i620;                         /* 0x620  (init -1)                    */
    char pad_624[0x63C - 0x624];      /* 0x624 */
    struct numtx_s draw_mtx;          /* 0x63C  cached render transform      */
    s32 water_hit;                    /* 0x67C  water-fire hit latch         */
    char pad_680[0x690 - 0x680];      /* 0x680 */
}; /* 0x690 */

/* ------------------------------------------------------------------ */
/* Globals owned by jeep.c                                             */
/* ------------------------------------------------------------------ */

extern struct fireboss_s FireBoss;        /* 0x005C1010 */
extern struct jeepballoon_s JeepBalloon[6]; /* 0x005C16A0 */

extern s32 FireBossHealth;   /* 0x00631A80 */
extern s32 FireBossWon;      /* 0x00631A38 */
extern s32 FireBossFinished; /* 0x00631A34 */

/* ------------------------------------------------------------------ */
/* Fire-boss / balloon functions (unit 101)                            */
/* ------------------------------------------------------------------ */

void InitFireBoss(struct fireboss_s *fb);            /* 0x002289B0 */
void DrawFireBoss(struct fireboss_s *fb);            /* 0x00228B10 */
void ProcessJeepBalloon(struct jeepballoon_s *b);    /* 0x0022A9A8 */
void DrawJeepBalloon(struct jeepballoon_s *b);       /* 0x0022A8E0 */
void FireBossWaterFire(s32 on);                      /* 0x0022AAF8 */
void FireBossReset(void);                            /* 0x0022B1F0 */
void DrawFireBossLevelExtra(void);                   /* 0x0022B248 */
void ProcessFireBossLevel(void);                     /* 0x0022B278 */
s32 GetTotalFireBossObjectives(void);                /* 0x0022B370 */
s32 GetCurrentFireBossObjectives(void);              /* 0x0022B378 */
s32 AddBalloon(struct nuvec_s *pos, struct nuvec_s *vel); /* 0x0022B718 */
s32 CheckAgainstFireBoss(struct nuvec_s *a, struct nuvec_s *b, float radius); /* 0x0022B810 */
char *FireBossActionName(void);                      /* 0x0022C200 */
void InitJeepBalloons(void);                         /* 0x0022C280 */
void ProcessJeepBalloons(void);                      /* 0x0022C2A8 */
void DrawJeepBalloons(void);                         /* 0x0022C310 */
struct jeepballoon_s *FindJeepBalloon(void);         /* 0x0022C378 */
s32 BalloonHitFireBoss(struct nuvec_s *pos);         /* 0x0022C3B8 */

#endif /* GAMECODE_JEEP_H */
