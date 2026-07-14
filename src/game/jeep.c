/*
 * Unit: game/jeep
 *
 * Functions:
 *   0x00223ae0 SpinWheelsJeep
 *   0x00223c68 GenerateJeepTargets
 *   0x00223de8 TiltJeep
 *   0x00223f90 MovePlayerJeep
 *   0x00224990 DriveJeep
 *   0x00224fc0 ProcessJeepMovement
 *   0x002256d0 SetUpJeepWheelPositions
 *   0x00225908 ResetJeep
 *   0x00225b80 DrawJeep
 *   0x00225d98 DrawJeepDebugStuff
 *   0x00225ec8 JeepCamFollowAng
 *   0x00226958 JeepCamIntro
 *   0x00226c98 JeepCam
 *   0x002273c0 ProcessEnemyJeep
 *   0x00227550 InitEnemyJeeps
 *   0x002277a0 DrawEnemyJeep
 *   0x00227c38 PlayerHitBuggies
 *   0x00227ec8 LoadWesternArenaData
 *   0x00228070 CurrentWesternPosition
 *   0x002281c8 NewSetTrailPos
 *   0x00228370 ProcessJeepTrail
 *   0x00228900 ProcessJeepTrails
 *   0x002289b0 InitFireBoss
 *   0x00228b10 DrawFireBoss
 *   0x00228d70 ProcessFireBoss
 *   0x0022a5a8 DrawFireBossDebugStuff
 *   0x0022a8e0 DrawJeepBalloon
 *   0x0022a9a8 ProcessJeepBalloon
 *   0x0022aaf8 FireBossWaterFire
 *   0x0022adf8 DrawPlayerJeep
 *   0x0022ae40 NewGenerateJeepMatrix
 *   0x0022aef8 GenerateJeepWheelPoint
 *   0x0022af30 DrawJeepTrails
 *   0x0022afd8 WesternArenaReset
 *   0x0022b0b0 DrawWesternArenaLevelExtra
 *   0x0022b108 ProcessWesternArenaLevel
 *   0x0022b1f0 FireBossReset
 *   0x0022b248 DrawFireBossLevelExtra
 *   0x0022b278 ProcessFireBossLevel
 *   0x0022b340 DrawWesternArenaTemp
 *   0x0022b370 GetTotalFireBossObjectives
 *   0x0022b378 GetCurrentFireBossObjectives
 *   0x0022b388 ProcessGenericTrail
 *   0x0022b408 FindTerrainType
 *   0x0022b498 NewInitTrail
 *   0x0022b508 SteeringUpdate
 *   0x0022b598 TiltSeek
 *   0x0022b698 LimitSpeedZbyXZ
 *   0x0022b718 AddBalloon
 *   0x0022b810 CheckAgainstFireBoss
 *   0x0022b950 PackBuggyData
 *   0x0022ba80 LimitCam
 *   0x0022bb50 DoCamMtx
 *   0x0022bca8 BlendNUVECs
 *   0x0022bd00 InitEnemyJeep
 *   0x0022bdd8 AnimateForLightsEnemyJeep
 *   0x0022bdf8 DrawEnemyJeeps
 *   0x0022be50 ProcessEnemyJeeps
 *   0x0022bea8 AnimateForLightsEnemyJeeps
 *   0x0022bf10 LoadBuggyData
 *   0x0022c008 WesternRaceManager
 *   0x0022c010 EmptyTrail
 *   0x0022c068 NewFadeOutLastTrail
 *   0x0022c0e8 NewFindTrailAng
 *   0x0022c120 JonnySteam
 *   0x0022c198 JonnyBossSmashEffect
 *   0x0022c200 FireBossActionName
 *   0x0022c280 InitJeepBalloons
 *   0x0022c2a8 ProcessJeepBalloons
 *   0x0022c310 DrawJeepBalloons
 *   0x0022c378 FindJeepBalloon
 *   0x0022c3b8 BalloonHitFireBoss
 */

#include "creature.h"

extern struct nuvec_s BaseWheelPosition[];

struct fireboss_s {
    u8 unk_0x00[0x408];
    s32 HitPoints;        /* 0x408 */
};
extern struct fireboss_s FireBoss;

extern void NuMtxSetRotationX(struct numtx_s *m, short ang);
extern void NuMtxRotateZ(struct numtx_s *m, short ang);
extern void NuMtxRotateY(struct numtx_s *m, short ang);
extern void NuMtxRotateX(struct numtx_s *m, short ang);
extern void NuMtxTranslate(struct numtx_s *m, struct nuvec_s *pos);
extern void NuVecScale(f32 scale, struct nuvec_s *dest, struct nuvec_s *src);
extern void NuVecScaleAccum(f32 scale, struct nuvec_s *dest, struct nuvec_s *src);
extern void DrawFireBoss(struct fireboss_s *fb);
extern void DrawJeepRocks(void);
extern void DrawVehMasks(void);
extern void *app_fnt;
extern void NuFntSet(void *fnt);
extern void NuFntSetPen(u32 colour);
extern f32 NuFsqrt(f32 x);
extern f32 fsign(f32 x);
extern double pow(double, double);
extern f32 NewShadowMaskPlat(struct nuvec_s *pos, f32 y0, s32 flag);
extern s32 ShadowInfo(void);

struct JEEPSTRUCT {
    u8 _pad_000[0x7F4];
    f32 TiltSeekTime;               /* 0x7F4 */
    u16 aTiltX;                     /* 0x7F8 */
    u16 aTiltZ;                     /* 0x7FA */
    u16 aDestTiltX;                 /* 0x7FC */
    u16 aDestTiltZ;                 /* 0x7FE */
    u8 _pad_800[0x84C - 0x800];
    f32 FloorHeight;                /* 0x84C */
    s32 TerrainType;                /* 0x850 */
    u8 _pad_854[0x8F8 - 0x854];
    struct nuvec_s ActualPosition;  /* 0x8F8 */
};

struct GENERICTRAIL {
    struct nuvec_s Position;        /* 0x00 */
    struct nuvec_s OldPosition;     /* 0x0C */
    f32 TrailWidth;                 /* 0x18 */
    f32 Radius;                     /* 0x1C */
    s32 Platform;                   /* 0x20 */
    s32 pad_24;                     /* 0x24 */
};
extern struct GENERICTRAIL GenericTrail[];
extern void ProcessJeepTrail(struct GENERICTRAIL *t, s32 i);

struct JEEPTRAILPT {
    struct nuvec_s pos1;            /* 0x00 */
    u8 pad_0C[0x20 - 0x0C];
};
extern struct JEEPTRAILPT JeepTrail[][0x20];
extern s32 TrailPntr[];
extern s32 TrailAir[];

struct spline_s {
    s16 len;                        /* 0x0 */
    s16 ptsize;                     /* 0x2 */
    u8 pad4[0x4];                   /* 0x4 */
    u8 *pts;                        /* 0x8 */
};
struct spltab_s {
    struct spline_s *spl;           /* 0x0 */
    u8 pad[0x14];
};
extern struct spltab_s SplTab[];
extern void PointAlongSpline(struct spline_s *spl, f32 ratio,
                             struct nuvec_s *out, void *a3, void *a4);

struct SPLINEFOLLOW {
    struct spline_s *Spline;        /* 0x00 */
    f32 Cur;                        /* 0x04 */
    f32 Nex;                        /* 0x08 */
    f32 Act;                        /* 0x0C */
    f32 Inc;                        /* 0x10 */
    struct nuvec_s CurPos;          /* 0x14 */
    struct nuvec_s NexPos;          /* 0x20 */
};
extern struct SPLINEFOLLOW JeepFollowSpline;

extern void InitEnemyJeeps(void);
extern s32 D_005BBCC4[];              /* PlayerJeep.Finished (far .data -> absolute) */
extern struct spline_s *D_00586394[]; /* Rail[0].pCAM (far .data -> absolute) */
extern f32 WesternCountdown;
extern s32 SmokeyFinished;
extern f32 WesternTime;
extern s32 SmokeyCountDownValue;
extern struct spline_s *JeepIntroLookSpline;
extern struct spline_s *JeepIntroCamSpline;
extern s32 SmokeyCam;

extern void NuVecSub(struct nuvec_s *dest, struct nuvec_s *a, struct nuvec_s *b);
extern s32 NuAtan2D(f32 x, f32 z);

extern s32 FireBossFinished;
extern s32 FireBossWon;
extern s32 ChrisBigBossDead;
extern s32 VEHICLECONTROL;
extern s32 WaterTimer;
extern s32 FireBossHoldPlayer;
extern s32 FireBossHealth;
extern s32 SMASHRUMPOWER;
extern void InitVehMasks(void);
extern void InitVehMask(s32 a, s32 b);
extern void InitFireBoss(struct fireboss_s *fb);
extern void InitJeepRocks(void);
extern void ProcessFireBoss(struct fireboss_s *fb);
extern void ProcessJeepRocks(void);
extern void ProcessVehMasks(void);
extern void ProcessRockRockCollisions(void);
extern s32 CheckAgainstRocks(struct nuvec_s *pos, struct nuvec_s *vel);
extern void KillPlayer(struct obj_s *obj, s32 type);


void NewGenerateJeepMatrix(struct numtx_s *Mat, short YAng, short SurfaceX,
                           short SurfaceZ, short TiltX, short TiltZ,
                           struct nuvec_s *Pos) {
    NuMtxSetRotationX(Mat, TiltX);
    NuMtxRotateZ(Mat, TiltZ);
    NuMtxRotateY(Mat, YAng);
    NuMtxRotateZ(Mat, SurfaceZ);
    NuMtxRotateX(Mat, SurfaceX);
    if (Pos != 0) {
        NuMtxTranslate(Mat, Pos);
    }
}

struct nuvec_s GenerateJeepWheelPoint(s32 WheelId) {
    return BaseWheelPosition[WheelId];
}

void WesternArenaReset(s32 PlayerDead) {
    struct nuvec_s Temp;

    D_005BBCC4[0] = 0;
    SmokeyFinished = 0;
    WesternTime = 0;
    WesternCountdown = 5.999f;
    SmokeyCountDownValue = 0;
    InitEnemyJeeps();
    JeepIntroCamSpline = SplTab[68].spl;
    JeepIntroLookSpline = SplTab[69].spl;
    if (SplTab[68].spl != 0 && SplTab[69].spl != 0) {
        SmokeyCam = 0x15;
    } else {
        SmokeyCam = 0x14;
    }
    JeepFollowSpline.Spline = D_00586394[0];
    if (D_00586394[0] != 0) {
        JeepFollowSpline.Cur = 0.0f;
        JeepFollowSpline.Inc = 0.0005f;
        JeepFollowSpline.Nex = 0.0f;
        JeepFollowSpline.Act = 0.0f;
        PointAlongSpline(D_00586394[0], 0.0f, &Temp, 0, 0);
        JeepFollowSpline.CurPos = Temp;
        JeepFollowSpline.NexPos = Temp;
    }
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

void ProcessFireBossLevel(void) {
    struct nuvec_s Rel;

    ProcessFireBoss(&FireBoss);
    ProcessJeepRocks();
    ProcessVehMasks();
    ProcessRockRockCollisions();
    NuVecSub(&Rel, &player->obj.pos, &player->obj.oldpos);
    if (CheckAgainstRocks(&player->obj.pos, &Rel) != 0) {
        NewRumble(&player->rumble, SMASHRUMPOWER);
        NewBuzz(&player->rumble, 5);
        if (FireBossHoldPlayer == 0 && FireBoss.HitPoints > 0) {
            KillPlayer(&player->obj, 0xD);
        }
    }
    if (FireBossHealth < 1) {
        FireBossFinished = 1;
        FireBossWon = 1;
    }
}

void DrawFireBossLevelExtra(void) {
    DrawFireBoss(&FireBoss);
    DrawJeepRocks();
    DrawVehMasks();
}

void DrawWesternArenaTemp(void) {
    NuFntSet(app_fnt);
    NuFntSetPen(0x7F7F7F7F);
}

s32 GetTotalFireBossObjectives(void) {
    return 3;
}

s32 GetCurrentFireBossObjectives(void) {
    return FireBoss.HitPoints;
}

void ProcessGenericTrail(s32 id, struct nuvec_s *pos, f32 Radius, f32 width) {
    GenericTrail[id].OldPosition = GenericTrail[id].Position;
    GenericTrail[id].Radius = Radius;
    GenericTrail[id].Position = *pos;
    GenericTrail[id].TrailWidth = width;
    GenericTrail[id].Platform = -1;
    ProcessJeepTrail(GenericTrail + id, id);
}

u16 NewFindTrailAng(struct nuvec_s *A, struct nuvec_s *B) {
    struct nuvec_s Line;

    NuVecSub(&Line, B, A);
    return (u16)(NuAtan2D(Line.x, Line.z) - 0x2000);
}

void FindTerrainType(struct JEEPSTRUCT *Jeep) {
    struct nuvec_s Pos;
    f32 FloorY;

    Pos = Jeep->ActualPosition;
    Pos.y += 1.0f;
    FloorY = Jeep->ActualPosition.y - NewShadowMaskPlat(&Pos, 0.0f, -1);
    Jeep->FloorHeight = FloorY;
    if (FloorY < 0.1f) {
        Jeep->TerrainType = ShadowInfo();
    } else {
        Jeep->TerrainType = -1;
    }
}

void SteeringUpdate(f32 *val, f32 target) {
    f32 step;

    step = fsign(target - *val) * 0.025f;
    if (target < *val) {
        *val = *val + step;
        if (*val < target) {
            *val = target;
        }
    } else if (*val < target) {
        *val = *val + step;
        if (target < *val) {
            *val = target;
        }
    }
}

void TiltSeek(struct JEEPSTRUCT *Jeep, f32 DeltaTime) {
    f32 rate;
    f32 TempX;
    f32 TempZ;

    if (Jeep->TiltSeekTime == 0.0f) {
        rate = 1.0f;
    } else {
        rate = (f32)(1.0 - 1.0 / pow(2.0, DeltaTime / Jeep->TiltSeekTime));
    }
    TempX = (f32)(s16)(Jeep->aDestTiltX - Jeep->aTiltX) * rate;
    TempZ = (f32)(s16)(Jeep->aDestTiltZ - Jeep->aTiltZ) * rate;
    Jeep->aTiltX += (short)TempX;
    Jeep->aTiltZ += (short)TempZ;
}

void LimitSpeedZbyXZ(struct nuvec_s *Vec, f32 LimitSpeed) {
    f32 MaxZ;

    if (Vec->x > LimitSpeed) {
        Vec->x = LimitSpeed;
    } else {
        if (Vec->x < -LimitSpeed) {
            Vec->x = -LimitSpeed;
        }
    }
    MaxZ = NuFsqrt(LimitSpeed * LimitSpeed - Vec->x * Vec->x) * 1.1f;
    if (Vec->z > MaxZ) {
        Vec->z = MaxZ;
    }
}

void BlendNUVECs(struct nuvec_s *Dest, struct nuvec_s *A, struct nuvec_s *B,
                 f32 Blend) {
    NuVecScale(1.0f - Blend, Dest, A);
    NuVecScaleAccum(Blend, Dest, B);
}

void WesternRaceManager(void) {
}

void EmptyTrail(s32 i) {
    s32 j;

    for (j = 0; j < 0x20; j++) {
        JeepTrail[i][j].pos1.x = -10000.0f;
    }
    TrailPntr[i] = 0;
    TrailAir[i] = 0;
}
