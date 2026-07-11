/*
 * Unit: game/vehicle
 *
 * Functions:
 *   0x00201a38 StillLockedOnTarget
 *   0x00201b70 DrawGlider
 *   0x00201d88 GliderFire
 *   0x00202818 DrawWeatherBossTarget
 *   0x00202980 ControlGliderWeatherBoss
 *   0x00202dc0 ControlGlider
 *   0x00203860 ProcessGliderMovement
 *   0x00203f00 GliderSmoke
 *   0x002041c8 DeadGliderCoco
 *   0x002043b8 ProcessGliderMovementWB
 *   0x00204588 MoveGlider
 *   0x00205268 InitZoffa
 *   0x00205440 DrawZoffaUFOs
 *   0x00205548 FindZoffaRestartPoint
 *   0x002056d8 ZoffaSmoke
 *   0x00205978 TeleportManager
 *   0x00205be8 MoveZoffaUFO
 *   0x00206f80 DrawZoffaDebugStuff
 *   0x002070d0 InitHova
 *   0x002072f0 InitialiseHovaBlimps
 *   0x00207488 ProcessHovaBlimp
 *   0x00207858 ProcessAllHovaBlimps
 *   0x00207998 DrawAllHovaBlimps
 *   0x00207ca8 ProcessSatellite
 *   0x00207e60 InitSpaceStation
 *   0x002081a0 InitSpaceStations
 *   0x002082b8 DrawSpaceStation
 *   0x002084a0 ProcessSpaceStation
 *   0x00208a28 AddGliderBullet
 *   0x00208bf0 CollideGliderBullets
 *   0x002091b0 GliderBulletsHitThings
 *   0x00209528 GliderBombsHitThings
 *   0x00209750 DrawGliderBullets
 *   0x00209990 ProcessGliderBullets
 *   0x00209c38 MovePlane
 *   0x00209dd0 FarmReset
 *   0x0020a118 ProcessFarmLevel
 *   0x0020a2f8 GliderCamWeatherBoss
 *   0x0020a658 GliderCam
 *   0x0020ae70 InitAsteroid
 *   0x0020b198 DrawAsteroid
 *   0x0020b2a0 ProcessAsteroid
 *   0x0020b8a0 CheckForPotentialMidAirCollisions
 *   0x0020c498 SafeFromCollisions
 *   0x0020c5b0 LinesIntersectEllipse
 *   0x0020c7b8 CheckGliderCollisions
 *   0x0020d730 DrawGliderBombs
 *   0x0020d898 ProcessGliderBombs
 *   0x0020dcc0 CollideGliderBombs
 *   0x0020ded8 InitGunBoats
 *   0x0020e1d8 ProcessGunBoat
 *   0x0020e770 InitBigGuns
 *   0x0020f640 GetBigGunBestTarget
 *   0x0020f810 ProcessBigGun
 *   0x0020fe00 CollideWithBattleShip
 *   0x0020ff90 InitBattleShip
 *   0x00210150 InitBattleShips
 *   0x00210290 ProcessBattleShip
 *   0x002107a0 InitWeatherBoss_a
 *   0x00210960 DrawWeatherBoss_a
 *   0x00210b28 ProcessWeatherBoss_a
 *   0x00211d38 DrawGliderTarget
 *   0x00211eb8 DrawTorpedoTarget
 *   0x00212618 FireFlyReset
 *   0x00212918 DrawFireFlyLevelExtra
 *   0x00212b08 ProcessFireFlyIntro
 *   0x00213110 ProcessFireFlyLevel
 *   0x00213308 WeatherResearchReset
 *   0x00213598 SpaceArenaReset
 *   0x00213880 DrawSpaceArenaLevelExtra
 *   0x00213a70 ProcessSpaceArenaLevel
 *   0x00213d10 CollidePlayerPoint
 *   0x00213ed0 ProcessLighteningHail
 *   0x00214030 UnleashLighteningHail
 *   0x00214220 DrawWBBolts
 *   0x00214400 ProcessWBBolts
 *   0x00214588 WeatherBossReset
 *   0x00214980 DrawWeatherBossLevelExtra
 *   0x00214ac8 ProcessWeatherBossLevel
 *   0x00214c68 ProcessWBIntro
 *   0x00214de8 ProcessVehicleLevel
 *   0x00214f40 DrawExtraCreatures
 *   0x00215140 GetCurrentLevelObjectives
 *   0x002152f8 ControlAtlas
 *   0x002154e0 ProcessMovementAtlas
 *   0x00215750 AdjustAtlasRotations
 *   0x002159f0 ProcessPlatformsAtlas
 *   0x00215df0 TerrainAtlas
 *   0x00216598 CheckAtlasGround
 *   0x00216848 ResetAtlas
 *   0x00216ba8 ProcessAtlas
 *   0x00216f48 MoveAtlas
 *   0x002171a8 InitEarthBoss
 *   0x00217420 JonnyParticles
 *   0x00217680 DrawEarthBoss
 *   0x00217890 PrintEarthBossAction
 *   0x00217ab8 ProcessEarthBossActions
 *   0x00218d00 ProcessRumblePanel
 *   0x002190e8 RumbleHeadUpDisplay
 *   0x002195a8 ProcessEarthBossVortex
 *   0x002196e8 ProcessEarthBossLevel
 *   0x00219bd0 ProcessAtlasAtlasCollisions_a
 *   0x00219f90 TrailPointInPoly
 *   0x0021a138 SetTrailPos
 *   0x0021a498 ProcessAtlasTrail
 *   0x0021a940 AddRockVel
 *   0x0021adc0 ShootRoksSkyward
 *   0x0021ae90 AllRoksSkyward
 *   0x0021af28 DrawJeepRocks
 *   0x0021b088 ProcessJeepRock
 *   0x0021b410 ProcessJeepRocks
 *   0x0021b598 ProcessRockRockCollisions
 *   0x0021b718 RumbleCam
 *   0x0021ba08 ProcessVehMasks
 *   0x0021bc88 DrawVehMasks
 *   0x0021be58 MoveVehicle
 *   0x0021bed8 GetRumbleCrunchRoks
 *   0x0021bf50 GetRumblePlayerRoks
 *   0x0021bfc8 GetRumbleTotalRoks
 *   0x0021c018 AddGliderHitPoints
 *   0x0021c048 ResetVehicleLevel
 *   0x0021c1b0 DrawVehicleTrail
 *   0x0021c1e8 DrawAtlas
 *   0x0021c270 VehicleSetup
 *   0x0021c278 SetWeatherStartPos
 *   0x0021c2c8 GetRumblePlayerHealthPercentage
 *   0x0021c2d8 GetGliderHealthPercentage
 *   0x0021c2f8 GetMaxLevelObjectives
 *   0x0021c370 LoadVehicleStuff
 *   0x0021c3b8 ObjectToAtlas
 *   0x0021c428 BossBar
 *   0x0021c558 InitVehMasks
 *   0x0021c580 InitVehMask
 *   0x0021c5f0 SetNewMaskStuff
 *   0x0021c718 CollideWithBattleShips
 *   0x0021c798 ProcessCrashteroidsIntro
 *   0x0021c7b8 GetWeatherBossPos
 *   0x0021c7c8 GetWeatherBossSpline
 *   0x0021c7d8 GetBattleShipBestTarget
 *   0x0021c938 GetGunBoatBestTarget
 *   0x0021caa0 GetZoffaBestTarget
 *   0x0021cc00 PickGliderTarget
 *   0x0021ccd0 AddGliderBomb
 *   0x0021ce80 GliderWeatherBossRailStuff
 *   0x0021ceb0 InitGlider
 *   0x0021cfd8 InitWeatherBossTarget
 *   0x0021d040 ProcessWeatherBossTarget
 *   0x0021d0f8 ExplodeGlider
 *   0x0021d1d8 DeadGliderWB
 *   0x0021d2c8 DrawGliderDebugStuff
 *   0x0021d300 DrawZoffa
 *   0x0021d3b8 FindFreeHova
 *   0x0021d3f0 InitSatellite
 *   0x0021d4e8 DrawSatellite
 *   0x0021d610 ProcessSatellites
 *   0x0021d668 DrawSatellites
 *   0x0021d7d8 ProcessSpaceStations
 *   0x0021d848 DrawSpaceStations
 *   0x0021d8b0 GrabGliderBullet
 *   0x0021d958 FreeGliderBullet
 *   0x0021da20 InitPlane
 *   0x0021db00 DrawPlane
 *   0x0021db68 DrawFarmLevelExtra
 *   0x0021dbf0 FindFreeAsteroid
 *   0x0021dc90 GetRandomGliderLevelEdgePoint
 *   0x0021dd48 CountAsteroids
 *   0x0021dda0 RespawnAsteroids
 *   0x0021de38 InitAsteroids
 *   0x0021de80 ProcessAsteroids
 *   0x0021df38 DrawAsteroids
 *   0x0021df90 PossIntersect
 *   0x0021e128 GetCurrentFarmObjectives
 *   0x0021e158 FindSurfaceType
 *   0x0021e210 InitGunBoat
 *   0x0021e330 DrawGunBoat
 *   0x0021e3f0 ProcessGunBoats
 *   0x0021e448 InitBigGun
 *   0x0021e618 DrawBigGun
 *   0x0021e6c8 ProcessBigGuns
 *   0x0021e720 DrawBigGuns
 *   0x0021e818 DrawBattleShip
 *   0x0021e8c0 ProcessBattleShips
 *   0x0021e918 PrintWeatherBossAction
 *   0x0021ea20 InitWeatherBoss
 *   0x0021ea40 ProcessBazookaToken
 *   0x0021eb08 DrawBazookaToken
 *   0x0021eb58 FireWBBolt
 *   0x0021ed08 WeatherBossNextAction
 *   0x0021ed80 GetCurrentWeatherBossObjectives
 *   0x0021ede0 ProcessWeatherBoss
 *   0x0021ee10 DrawWeatherBoss
 *   0x0021ee40 GetCurrentFireFlyObjectives
 *   0x0021ee88 DrawWeatherResearchLevelExtra
 *   0x0021ef90 ProcessWeatherResearchLevel
 *   0x0021eff8 GetCurrentWeatherResearchObjectives
 *   0x0021f040 GetCurrentSpaceArenaObjectives
 *   0x0021f078 InitLighteningHail
 *   0x0021f0d8 DrawLighteningHail
 *   0x0021f1b0 FindFreeWBBoltOfType
 *   0x0021f1f8 FindFreeWBBolt
 *   0x0021f230 InitWBBolts
 *   0x0021f2b0 InitWBIntro
 *   0x0021f380 DrawEarthBossLevelExtra
 *   0x0021f3b0 EarthBossReset
 *   0x0021f410 GetCurrentRumbleObjectives
 *   0x0021f420 InitJeepRocks
 *   0x0021f448 KillAtlasphere
 *   0x0021f4b0 InitTrail
 *   0x0021f4e8 UnembedRayCastAtlas
 *   0x0021f578 UnembedRayCastAtlasSimple
 *   0x0021f5c8 InitAtlas
 *   0x0021f7d8 DrawJonny
 *   0x0021f8d8 UpdateRumbleCamTween
 *   0x0021f9a0 InitRumblePanel
 *   0x0021f9a8 ProcessAtlasAtlasCollisions
 *   0x0021f9e0 CheckAtlasVortex
 *   0x0021fa98 FadeOutLastTrail
 *   0x0021fb30 PointLineSide
 *   0x0021fb68 PointsSame
 *   0x0021fbb8 FindTrailAng
 *   0x0021fbf0 FindJeepRock
 *   0x0021fc30 KeepHoldOnRock
 *   0x0021fcb8 AddRock
 *   0x0021fce0 DrawJeepRock
 *   0x0021fdc0 SmashRockIntoTwo
 *   0x0021feb8 CheckAgainstRocks
 */

#include "creature.h"

extern s32 Level;
extern struct numtx_s mTEMP;

/* gp-relative angle scale constants used to convert the glider's float roll/
 * pitch/yaw into NuMtx integer rotation units. */
extern float D_0062D91C;
extern float D_0062D920;
extern float D_0062D924;
extern struct nuvec_s D_006B75A0;   /* fixed glider translation (level 0xD) */

/* per-level glider model indices into CModel (-1 == none).  These live far
 * from $gp (retail uses %hi/%lo absolute loads); declare as incomplete arrays
 * so the compiler does not treat them as gp-relative small data. */
extern s8 D_0056236E[];
extern s8 D_005623B9[];
extern s8 D_005623C3[];

extern s32 AtlasFrame;
extern s32 tttt;
extern s8 D_0056238B[];   /* per-level atlasphere model index (far from $gp) */

extern s32 temp_xzmomset;
extern float D_00560D44[]; /* ball vertical lift offset (far from $gp) */
extern float D_0062DE9C;   /* ball momentum -> velocity divisor */
extern float D_0062DEA0;   /* ball 'boing' vertical momentum */

void NuMtxSetRotationX(struct numtx_s *m, s32 r);
void NuMtxSetRotationZ(struct numtx_s *m, s32 r);
void NuMtxRotateX(struct numtx_s *m, s32 r);
void NuMtxRotateY(struct numtx_s *m, s32 r);
void NuMtxTranslate(struct numtx_s *m, struct nuvec_s *v);
void NuQuatToMtx(struct nuquat_s *q, struct numtx_s *m);
void NuHGobjRndr(struct NUHGOBJ_s *hobj, struct numtx_s *m, s32 nlayers,
                 short *layer);

/* ---- Zoffa UFO teleport (glider levels) ------------------------------- */

typedef struct ZOFFASTART {
    float x;
    float y;
    float z;
    float Angle;
} ZOFFASTART;                       /* 0x10 */

typedef struct ZOFFASTRUCT {
    unsigned char MainDraw[0xE0];   /* 0x000  draw/creature block */
    s32 ActiveMode;                 /* 0x0E0 */
    float RespawnTimer;             /* 0x0E4 */
    struct nuvec_s Position;        /* 0x0E8 */
    struct nuvec_s Velocity;        /* 0x0F4 */
    struct nuvec_s Resolved;        /* 0x100 */
    float TiltX;                    /* 0x10C */
    float TiltZ;                    /* 0x110 */
    float DestTiltX;                /* 0x114 */
    float DestTiltZ;                /* 0x118 */
    float DestAngleY;               /* 0x11C */
    float AngleY;                   /* 0x120 */
    s32 NoFireSound;                /* 0x124 */
    float Temp[6];                  /* 0x128 */
    float VisibleTimer;             /* 0x140 */
    float NewDirectionTimer;        /* 0x144 */
    float NewAltitudeTimer;         /* 0x148 */
    float NewAltitudeTarget;        /* 0x14C */
    s32 TerminalDive;               /* 0x150 */
    s32 Explode;                    /* 0x154 */
    s32 HitPoints;                  /* 0x158 */
    s32 FireFrame;                  /* 0x15C */
    s32 FireNow;                    /* 0x160 */
    float FireTimer;                /* 0x164 */
    s32 Seen;                       /* 0x168 */
    s32 SmkTimer;                   /* 0x16C */
    struct numtx_s Locators[16];    /* 0x170 */
    s32 SmokeCounter;               /* 0x570 */
    float AggressionTimer;          /* 0x574 */
    s32 PlayerCloseAndVisable;      /* 0x578 */
    float NotSeenTimer;             /* 0x57C */
    float AggressionPoints;         /* 0x580 */
    s32 Teleport;                   /* 0x584 */
    float NoTeleportTimer;          /* 0x588 */
    float Speed;                    /* 0x58C */
    s32 InFront;                    /* 0x590 */
    s32 FacingTarget;               /* 0x594 */
    float Dist;                     /* 0x598 */
    s32 FireSide;                   /* 0x59C */
    float NotInFrontTimer;          /* 0x5A0 */
    float LockedOnTimer;            /* 0x5A4 */
    float KeepSameVelocityTimer;    /* 0x5A8 */
    float FireBurstTimer;           /* 0x5AC */
} ZOFFASTRUCT;                      /* 0x5B0 */

typedef struct GLIDERSTRUCT {
    struct creature_s *Cre;         /* 0x00 */
    struct nuvec_s vel;             /* 0x04 */
    s32 Dead;                       /* 0x10 */
    s32 CocoDead;                   /* 0x14 */
    float CocoDeadTimer;            /* 0x18 */
    float CocoDeathSpinX;           /* 0x1C */
    float CocoDeathSpinZ;           /* 0x20 */
    float NextEngRum;               /* 0x24 */
    float FixVelTimer;              /* 0x28 */
    float ImmuneAsteroidsTimer;     /* 0x2C */
    struct nuvec_s Position;        /* 0x30 */
    struct nuvec_s OldPosition;     /* 0x3C */
    struct nuvec_s Velocity;        /* 0x48 */
    struct nuvec_s Resolved;        /* 0x54 */
    struct nuvec_s RailPoint;       /* 0x60 */
    float RailAngle;                /* 0x6C */
    float TiltX;                    /* 0x70 */
    float TiltZ;                    /* 0x74 */
    float DestTiltX;                /* 0x78 */
    float DestTiltZ;                /* 0x7C */
    float AngleY;                   /* 0x80 */
    /* ... remaining glider fields not needed here ... */
} GLIDERSTRUCT;

extern ZOFFASTRUCT EnemyZoffa[4];
extern struct nuvec_s TeleportPos[4];
extern struct nuvec_s TeleportVel[4];
extern GLIDERSTRUCT PlayerGlider;
extern struct numtx_s GameCam;      /* world camera matrix (== GameCam[0].m) */
extern float Level_GliderFloor;
extern float D_0062DA14;            /* == 0.8f (KeepSameVelocityTimer seed) */
extern s32 D_006332A4;              /* Zoffa teleport slot cursor */
#define ZoffaTeleportIndx D_006332A4

void NuVecMtxTransform(struct nuvec_s *dst, struct nuvec_s *src,
                       struct numtx_s *m);
void NuVecMtxRotate(struct nuvec_s *dst, struct nuvec_s *src,
                    struct numtx_s *m);
s32 SafeFromCollisions(struct nuvec_s *pos);
void InitZoffa(ZOFFASTRUCT *z, ZOFFASTART *start);

/* Faithful full-C near-match (state=asm): structure/frame/reg-mask exact, but
 * ee-gcc rematerializes %hi(mTEMP) in each of the Level==0x1A/else blocks and
 * fills the `bne` delay slot with a speculative scale load, where retail shares
 * one `lui $s1,%hi(mTEMP)` in that delay slot (+4 bytes). See docs/notes.md
 * ("DrawGlider") and the recorded no_progress blocker. */
void DrawGlider(struct creature_s *c)
{
    struct NEWBUGGY *b;
    float lift;
    float scale;
    s8 idx;

    b = c->Buggy;
    lift = ((Level != 0xD) && (Level != 0x1A)) ? 15.0f : 0.0f;

    if (b->enable && (Level == 0xD)) {
        NuMtxSetRotationX(&mTEMP, lift * D_0062D91C);
        NuMtxRotateY(&mTEMP, 0);
        NuMtxTranslate(&mTEMP, &D_006B75A0);
    } else if (Level == 0x1A) {
        scale = D_0062D920;
        NuMtxSetRotationZ(&mTEMP, b->roll * scale);
        NuMtxRotateX(&mTEMP, (b->pitch + b->pitch) * scale);
        NuMtxRotateY(&mTEMP, b->yaw * scale);
        NuMtxTranslate(&mTEMP, &b->pos);
    } else {
        scale = D_0062D924;
        NuMtxSetRotationZ(&mTEMP, b->roll * scale);
        NuMtxRotateX(&mTEMP, (b->pitch + lift) * scale);
        NuMtxRotateY(&mTEMP, b->yaw * scale);
        NuMtxTranslate(&mTEMP, &b->pos);
    }

    if (Level == 0x12) {
        idx = D_005623C3[0];
    } else if (Level == 0x1A) {
        idx = D_005623B9[0];
    } else {
        idx = D_0056236E[0];
    }
    if (idx != -1) {
        DrawCharacterModel(&CModel[idx], &c->obj.anim, &mTEMP, 0, 1, 0,
                           &c->mtxLOCATOR[1][0], &c->momLOCATOR[1][0], &c->obj);
    }
}

/* Respawns each active teleporting Zoffa UFO at one of four camera-relative
 * teleport points (cursor D_006332A4, advanced on success), clamped above the
 * glider floor and re-aimed 90/135 degrees off the player's heading, then seeds
 * its velocity by rotating TeleportVel through the camera matrix. */
void TeleportManager(void)
{
    struct nuvec_s Point;
    s32 Indx;
    s32 i;
    s32 j;
    ZOFFASTART Start;

    for (i = 0; i < 4; i++) {
        if ((EnemyZoffa[i].ActiveMode != 0) && (EnemyZoffa[i].Teleport != 0)) {
            for (j = 0; j < 2; j++) {
                Indx = (j + ZoffaTeleportIndx) & 3;
                NuVecMtxTransform(&Point, &TeleportPos[Indx], &GameCam);
                if (Point.y < Level_GliderFloor) {
                    Point.y = Level_GliderFloor;
                }
                if (SafeFromCollisions(&Point) != 0) {
                    Start.x = Point.x;
                    Start.y = Point.y;
                    Start.z = Point.z;
                    switch (Indx) {
                    case 0:
                        Start.Angle = PlayerGlider.AngleY - 135.0f;
                        break;
                    case 1:
                        Start.Angle = PlayerGlider.AngleY + 135.0f;
                        break;
                    case 2:
                        Start.Angle = PlayerGlider.AngleY - 90.0f;
                        break;
                    case 3:
                        Start.Angle = PlayerGlider.AngleY + 90.0f;
                        break;
                    }
                    InitZoffa(&EnemyZoffa[i], &Start);
                    EnemyZoffa[i].NoTeleportTimer = 3.0f;
                    EnemyZoffa[i].KeepSameVelocityTimer = D_0062DA14;
                    EnemyZoffa[i].Speed = 14.0f;
                    NuVecMtxRotate(&EnemyZoffa[i].Velocity,
                                   &TeleportVel[(j + ZoffaTeleportIndx) & 3],
                                   &GameCam);
                    ZoffaTeleportIndx = (ZoffaTeleportIndx + 1) & 3;
                    break;
                }
            }
        }
    }
}

void DrawAtlas(struct creature_s *c)
{
    struct numtx_s m;
    struct NEWBUGGY *b;
    s8 idx;

    b = c->Buggy;
    if (AtlasFrame == 0x705) {
        tttt++;
    }
    NuQuatToMtx(&b->rotquat, &m);
    NuMtxTranslate(&m, &b->ball_pos);
    idx = D_0056238B[0];
    if (idx != -1) {
        NuHGobjRndr(CModel[idx].hobj, &m, 1, 0);
    }
}

/* Seeds the ball (atlasphere) NEWBUGGY transform from a character's object:
 * ball_pos <- obj->pos (+ vertical lift), ball_vel <- obj->mom / divisor.
 * Faithful near-match (state=asm): first half byte-exact; the mom/divisor FP
 * load order and the `boing` beql delay-slot differ (scheduling). */
void ObjectToAtlas(struct obj_s *obj, struct creature_s *c)
{
    struct NEWBUGGY *b = c->Buggy;

    b->ball_pos.x = obj->pos.x;
    b->ball_pos.y = obj->pos.y + D_00560D44[0];
    b->ball_pos.z = obj->pos.z;
    if (temp_xzmomset != 0) {
        b->ball_vel.x = obj->mom.x / D_0062DE9C;
        b->ball_vel.z = obj->mom.z / D_0062DE9C;
    }
    if (obj->boing != 0) {
        obj->mom.y = D_0062DEA0;
    }
    b->ball_vel.y = obj->mom.y;
}
