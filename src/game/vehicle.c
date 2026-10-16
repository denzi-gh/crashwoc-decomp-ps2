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

#include "nu_libc.h"

#include "numath/numtx.h"
#include "numath/nuquat.h"
#include "numath/nutrig.h"
#include "numath/nuvec.h"
#include "nu3d/nugscn.h"
#include "game/gametypes.h"
#include "gamelib/terrain.h"
#include "game/creature.h"
#include "nups2/ps2ctl.h"

extern s32 Level;
extern struct numtx_s mTEMP;

/* paddata d-pad bits (libpad SCE_PADLup/right/down/left). */
#define PAD_UP    0x1000
#define PAD_RIGHT 0x2000
#define PAD_DOWN  0x4000
#define PAD_LEFT  0x8000

/* gp-relative angle scale constants used to convert the glider's float roll/
 * pitch/yaw into struct numtx_s integer rotation units. */
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

/* Full view of the player glider: struct glider_s (game/gametypes.h) only
 * names the members other units touch; the flight model needs the rest. */
struct gliderflight_s {
    struct creature_s *Cre;             /* 0x000 */
    struct nuvec_s vel;                 /* 0x004 */
    s32 Dead;                           /* 0x010 */
    s32 CocoDead;                       /* 0x014 */
    f32 CocoDeadTimer;                  /* 0x018 */
    f32 CocoDeathSpinX;                 /* 0x01C */
    f32 CocoDeathSpinZ;                 /* 0x020 */
    f32 NextEngRum;                     /* 0x024 */
    f32 FixVelTimer;                    /* 0x028 */
    f32 ImmuneAsteroidsTimer;           /* 0x02C */
    struct nuvec_s Position;            /* 0x030 */
    struct nuvec_s OldPosition;         /* 0x03C */
    struct nuvec_s Velocity;            /* 0x048 */
    struct nuvec_s Resolved;            /* 0x054 */
    struct nuvec_s RailPoint;           /* 0x060 */
    f32 RailAngle;                      /* 0x06C */
    f32 TiltX;                          /* 0x070 */
    f32 TiltZ;                          /* 0x074 */
    f32 DestTiltX;                      /* 0x078 */
    f32 DestTiltZ;                      /* 0x07C */
    f32 AngleY;                         /* 0x080 */
    f32 CamAngleY;                      /* 0x084 */
    f32 CamTiltX;                       /* 0x088 */
    f32 CamTornRecoverTimer;            /* 0x08C */
    f32 InputX;                         /* 0x090 */
    f32 InputZ;                         /* 0x094 */
    s32 BarrelRoll;                     /* 0x098 */
    f32 BarrelDelta;                    /* 0x09C */
    f32 BarrelSpeedX;                   /* 0x0A0 */
    s32 TerminalDive;                   /* 0x0A4 */
    s32 TerminalDir;                    /* 0x0A8 */
    f32 FireTimer;                      /* 0x0AC */
    f32 HitTimer;                       /* 0x0B0 */
    s32 AutoPilot;                      /* 0x0B4 */
    s32 ForceTurn;                      /* 0x0B8 */
    s32 HitPoints;                      /* 0x0BC */
    char LocatorList[16];               /* 0x0C0 */
    f32 LocatorTime[16];                /* 0x0D0 */
    s32 InTornado;                      /* 0x110 */
    s32 LastInTornado;                  /* 0x114 */
    f32 InTornadoTime;                  /* 0x118 */
    f32 TornadoSpin;                    /* 0x11C */
    f32 InTornadoScale;                 /* 0x120 */
    struct nuvec_s ApparentPosition;    /* 0x124 */
    struct nuvec_s ApparentVelocity;    /* 0x130 */
    struct nuvec_s PositionStack[30];   /* 0x13C */
    struct nuvec_s VelocityStack[30];   /* 0x2A4 */
    s32 StackIndx;                      /* 0x40C */
    f32 Speed;                          /* 0x410 */
    f32 TargetSpeed;                    /* 0x414 */
    f32 NotInFrontTimer;                /* 0x418 */
    f32 PullUpTimer;                    /* 0x41C */
    f32 OverideTiltZ;                   /* 0x420 */
    f32 TargetTimer;                    /* 0x424 */
    struct nuvec_s *MovingTargetPoint;  /* 0x428 */
    struct nuvec_s *MovingTargetVel;    /* 0x42C */
    s32 TargetOn;                       /* 0x430 */
    f32 TargetedTime;                   /* 0x434 */
    s32 TargetMoving;                   /* 0x438 */
    f32 NextHitSoundTimer;              /* 0x43C */
}; /* 0x440 */

void NuHGobjRndr(struct NUHGOBJ_s *hobj, struct numtx_s *m, s32 nlayers,
                 short *layer);

struct nuvec_s *SetNuVecPntr(f32 x, f32 y, f32 z);
f32 frand(void);
s32 ProcessTimer(f32 *timer);

/* Faithful full-C near-match (state=asm): structure/frame/reg-mask exact, but
 * ee-gcc rematerializes %hi(mTEMP) in each of the Level==0x1A/else blocks and
 * fills the `bne` delay slot with a speculative scale load, where retail shares
 * one `lui $s1,%hi(mTEMP)` in that delay slot (+4 bytes). See the recorded
 * no_progress blocker. */
void DrawGlider(struct creature_s *c)
{
    struct glider_s *g;
    float lift;
    float scale;
    s8 idx;

    g = (struct glider_s *)c->Buggy;
    lift = ((Level != 0xD) && (Level != 0x1A)) ? 15.0f : 0.0f;

    if (g->AutoPilot && (Level == 0xD)) {
        NuMtxSetRotationX(&mTEMP, lift * D_0062D91C);
        NuMtxRotateY(&mTEMP, 0);
        NuMtxTranslate(&mTEMP, &D_006B75A0);
    } else if (Level == 0x1A) {
        scale = D_0062D920;
        NuMtxSetRotationZ(&mTEMP, g->TiltZ * scale);
        NuMtxRotateX(&mTEMP, (g->TiltX + g->TiltX) * scale);
        NuMtxRotateY(&mTEMP, g->AngleY * scale);
        NuMtxTranslate(&mTEMP, &g->Position);
    } else {
        scale = D_0062D924;
        NuMtxSetRotationZ(&mTEMP, g->TiltZ * scale);
        NuMtxRotateX(&mTEMP, (g->TiltX + lift) * scale);
        NuMtxRotateY(&mTEMP, g->AngleY * scale);
        NuMtxTranslate(&mTEMP, &g->Position);
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
        NuHGobjRndr(CModel[idx].obj, &m, 1, 0);
    }
}

/* Seeds the atlasphere (struct NEWBUGGY) transform from a character's object:
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

/* ------------------------------------------------------------------ */
/* Small leaf functions (game/vehicle)                                 */
/* ------------------------------------------------------------------ */

extern s32 JeepInControl;
extern s32 NumRockPanel;
extern s32 D_005B7918[];   /* rumble player health % (far) */
extern s32 D_005B7CA0[];   /* current rumble objectives (far) */
extern struct nuvec_s D_005B44F8[]; /* weather boss position (far) */
extern struct nugspline_s D_005B4548; /* weather boss spline (far) */
extern struct nuvec_s D_006B75B0;   /* weather start pos temp (far) */
extern struct gliderflight_s PlayerGlider;
extern struct nuvec_s v000;

extern void ProcessFireFlyIntro(void);
extern void InitWeatherBoss_a(void);
extern void ProcessWeatherBoss_a(struct weatherboss_s *wb);
extern void DrawWeatherBoss_a(struct weatherboss_s *wb);
extern struct jeeprock_s *AddRockVel(struct nuvec_s *pos, struct nuvec_s *vel, f32 radius, s32 owner);
extern struct RPos_s *best_cRPos;

/* The earth boss is an atlasphere, like PlayerAtlas (far). */
extern struct NEWBUGGY EarthBoss;

/* WeatherBoss state block (far). */
struct weatherboss_s {
    s32 active;                /* 0x000 */
    char pad_004[0x0F0 - 0x004];
    struct MYDRAW draw;        /* 0x0F0 */
    char pad_1D0[0x5EC - 0x1D0];
    s32 objectives[4];         /* 0x5EC */
    char pad_5FC[0x600 - 0x5FC];
    f32 dist;                  /* 0x600 */
    f32 lastdist;              /* 0x604 */
    s32 nextaction;            /* 0x608 */
    char pad_60C[0x618 - 0x60C];
    s32 action;                /* 0x618 */
    char pad_61C[0x660 - 0x61C];
};
extern struct weatherboss_s WeatherBoss;

/* VehicleMask: stride 0x174, 2 entries. */
struct vehmask_s {
    s32 active;                /* 0x000 */
    s32 id;                    /* 0x004 */
    s32 killatend;             /* 0x008 */
    struct nuvec_s pos;        /* 0x00C */
    struct MYDRAW draw;        /* 0x018 */
    f32 drawangy;              /* 0x0F8 */
    s32 action;                /* 0x0FC */
    s32 lastaction;            /* 0x100 */
    s32 effecttype;            /* 0x104 */
    f32 tween;                 /* 0x108 */
    f32 tweeninc;              /* 0x10C */
    f32 ang;                   /* 0x110 */
    struct nuvec_s store[2];   /* 0x114 */
    struct nuvec_s *point[2];  /* 0x12C */
    struct nuvec_s offset[2];  /* 0x134 */
    f32 anginc[2];             /* 0x14C */
    f32 rad[2];                /* 0x154 */
    f32 scale[2];              /* 0x15C */
    f32 tiltx[2];              /* 0x164 */
    f32 drawscale;             /* 0x16C */
    s32 seen;                  /* 0x170 */
};
extern struct vehmask_s VehicleMask[2];

/* HovaBlimp array, stride 0x134, 6 entries. */
struct hovablimp_s {
    s32 active;                /* 0x00 */
    struct MYDRAW draw;        /* 0x04 */
    struct nuvec_s pos;        /* 0xE4 */
    struct nuvec_s startpos;   /* 0xF0 */
    struct nuvec_s vel;        /* 0xFC */
    f32 angy;                  /* 0x108 */
    s32 thrust;                /* 0x10C */
    f32 thrustoff;             /* 0x110 */
    s32 terminaldive;          /* 0x114 */
    s32 explode;               /* 0x118 */
    s32 hitpoints;             /* 0x11C */
    f32 staystilltimer;        /* 0x120 */
    f32 movetimer;             /* 0x124 */
    struct nuvec_s targetvel;  /* 0x128 */
}; /* 0x134 */
extern struct hovablimp_s HovaBlimp[6];

/* Tornado under each blimp, stride 0x110, 6 entries. */
struct tornado_s {
    s32 active;                /* 0x000 */
    struct nuvec_s pos;        /* 0x004 */
    struct nuvec_s startpos;   /* 0x010 */
    char pad_01C[0xFC - 0x01C];
    f32 scale;                 /* 0x0FC */
    f32 yang;                  /* 0x100 degrees */
    f32 yanginc;               /* 0x104 degrees per second */
    char pad_108[0x110 - 0x108];
};
extern struct tornado_s TornadoList[6];
extern s32 ACTIVEBLIMPCOUNT;
extern void ProcessHovaBlimp(struct hovablimp_s *hova);
extern f32 Rationalise360f(f32 a);

/* Spin the tornado and keep it under its blimp, or retire it with it. */
static inline void ProcessTornado(struct tornado_s *tornado, struct hovablimp_s *hova)
{
    tornado->yang += tornado->yanginc * 0.02f;
    tornado->yang = Rationalise360f(tornado->yang);
    if (hova->active != 0) {
        tornado->pos = hova->pos;
        tornado->pos.y = 27.0f;
        tornado->startpos = tornado->pos;
    } else {
        tornado->active = 0;
    }
}

/* Returns whether any blimp was active this frame. */
s32 ProcessAllHovaBlimps(void)
{
    s32 i;
    s32 ret;

    ret = 0;
    ACTIVEBLIMPCOUNT = 0;
    for (i = 0; i < 6; i++) {
        if (HovaBlimp[i].active != 0) {
            ret = 1;
            ProcessHovaBlimp(&HovaBlimp[i]);
            if (HovaBlimp[i].active != 0) {
                ACTIVEBLIMPCOUNT++;
            }
        }
    }
    for (i = 0; i < 6; i++) {
        if (TornadoList[i].active != 0) {
            ProcessTornado(&TornadoList[i], &HovaBlimp[i]);
        }
    }
    return ret;
}

extern void InitHova(struct nuvec_s *pos, f32 angle);
extern void DebFree(s32 *key);
extern s32 jontorn[20];
extern s32 jonfirst;

void InitialiseHovaBlimps(void)
{
    s32 i;

    memset(HovaBlimp, 0, sizeof(HovaBlimp));
    memset(TornadoList, 0, sizeof(TornadoList));
    InitHova(SetNuVecPntr(0.0f, 50.0f, 0.0f), 0.0f);
    InitHova(SetNuVecPntr(0.0f, 50.0f, 160.0f), 0.0f);
    InitHova(SetNuVecPntr(152.0f, 50.0f, 49.0f), 0.0f);
    InitHova(SetNuVecPntr(94.0f, 50.0f, -129.0f), 0.0f);
    InitHova(SetNuVecPntr(-94.0f, 50.0f, -129.0f), 0.0f);
    InitHova(SetNuVecPntr(-152.0f, 50.0f, 49.0f), 0.0f);
    for (i = 0; i < 20; i++) {
        if (jontorn[i] != -1 && jonfirst != 0) {
            DebFree(&jontorn[i]);
        }
        jontorn[i] = -1;
    }
    jonfirst = 1;
}

/* JeepRock array, stride 0x3D4, 6 entries. */
struct jeeprock_s {
    struct nuvec_s pos;        /* 0x000 */
    struct nuvec_s vel;        /* 0x00C */
    s32 exists;                /* 0x018 */
    s32 seen;                  /* 0x01C */
    char pad_020[0x024 - 0x020];
    s32 explode;               /* 0x024 set when something hits the rock */
    char pad_028[0x030 - 0x028];
    struct NEWBUGGY atlas;     /* 0x030 the rock rolls as an atlasphere */
    s32 held;                  /* 0x3B4 */
    s32 owner;                 /* 0x3B8 */
    f32 size;                  /* 0x3BC */
    struct nuvec_s scale;      /* 0x3C0 */
    char pad_3CC[0x3D4 - 0x3CC];
};
extern struct jeeprock_s JeepRock[];

extern s32 RumbleDisplayMode;
extern s32 RumbleStoreTotalRoks;
extern s32 RumbleStoreCrunchRoks;
extern s32 RumbleStoreCrashRoks;
extern s32 ShootRockSound;

extern struct nuvec_s D_005B7E9C;
#define rumble_sfx_pos D_005B7E9C
extern void MyGameSfx(s32 id, struct nuvec_s *pos, s32 vol);
extern struct nuvec_s SetNuVec(f32 x, f32 y, f32 z);

void ShootRoksSkyward(void)
{
    s32 i;

    RumbleDisplayMode = -1;
    MyGameSfx(0xCD, &rumble_sfx_pos, 0x7FFF);
    ShootRockSound = 1;
    for (i = 0; i < 6; i++) {
        if (JeepRock[i].exists != 0) {
            JeepRock[i].atlas.ball_vel = SetNuVec(0.0f, 13.0f, 0.0f);
            JeepRock[i].owner = 20;
            JeepRock[i].atlas.ball_beenhit = 0;
            JeepRock[i].size = 10.0f;
        }
    }
}


s32 AllRoksSkyward(void)
{
    s32 done = 1;
    s32 i;

    for (i = 0; i < 6; i++) {
        if (JeepRock[i].exists != 0) {
            if (JeepRock[i].pos.y < 10.0f) {
                if (JeepRock[i].owner != 20 || JeepRock[i].atlas.ball_vel.y < 5.0f) {
                    JeepRock[i].exists = 0;
                }
                done = 0;
            } else {
                JeepRock[i].exists = 0;
            }
        }
    }
    if (done) {
        RumbleDisplayMode = 0;
        ShootRockSound = 0;
    }
    return done;
}

s32 GetRumbleCrunchRoks(void)
{
    s32 count;
    s32 i;

    if (RumbleDisplayMode == -1) {
        return RumbleStoreCrunchRoks;
    }
    count = 0;
    for (i = 0; i < 6; i++) {
        if (JeepRock[i].exists != 0 && JeepRock[i].size > 0.0f &&
            JeepRock[i].owner == -1) {
            count++;
        }
    }
    RumbleStoreCrunchRoks = count;
    return count;
}

s32 GetRumblePlayerRoks(void)
{
    s32 count;
    s32 i;

    if (RumbleDisplayMode == -1) {
        return RumbleStoreCrashRoks;
    }
    count = 0;
    for (i = 0; i < 6; i++) {
        if (JeepRock[i].exists != 0 && JeepRock[i].size > 0.0f &&
            JeepRock[i].owner == 1) {
            count++;
        }
    }
    RumbleStoreCrashRoks = count;
    return count;
}

s32 GetRumbleTotalRoks(void)
{
    s32 count;
    s32 i;

    if (RumbleDisplayMode == -1) {
        return RumbleStoreTotalRoks;
    }
    count = 0;
    for (i = 0; i < 6; i++) {
        if (JeepRock[i].exists != 0) {
            count++;
        }
    }
    RumbleStoreTotalRoks = count;
    return count;
}

void AddGliderHitPoints(void)
{
    struct glider_s *g = (struct glider_s *)player->Buggy;

    g->HitPoints += 25;
    if (g->HitPoints > 100) {
        g->HitPoints = 100;
    }
}

void VehicleSetup(void)
{
    JeepInControl = 0;
}

void SetWeatherStartPos(struct creature_s *c)
{
    D_006B75B0 = c->obj.pos;
    PlayerGlider.Position = D_006B75B0;
}

s32 GetRumblePlayerHealthPercentage(void)
{
    return D_005B7918[0];
}

s32 GetGliderHealthPercentage(struct creature_s *c)
{
    struct glider_s *g = (struct glider_s *)c->Buggy;

    if (g != 0) {
        return g->HitPoints;
    }
    return 100;
}

extern void LoadWesternArenaData(void);

void LoadVehicleStuff(void)
{
    switch (Level) {
    case 3:
        LoadWesternArenaData();
        break;
    case 0xD:
        jonfirst = 0;
        break;
    }
}

void InitVehMasks(void)
{
    memset(VehicleMask, 0, sizeof(VehicleMask));
}

extern s32 MyInitModelNew(struct MYDRAW *draw, s32 id, s32 action, s32 numjoints,
                          void *jointlist, struct nuvec_s *pos);

void InitVehMask(s32 index, s32 id)
{
    struct vehmask_s *mask = &VehicleMask[index];

    memset(mask, 0, sizeof(struct vehmask_s));
    if (MyInitModelNew(&mask->draw, id, 0x22, 0, 0, &mask->pos) != 0) {
        mask->id = id;
    }
}

void SetNewMaskStuff(s32 index, struct nuvec_s *centre, struct nuvec_s *offset,
                     f32 rad, f32 anginc, f32 tweeninc, s32 fixed,
                     s32 killatend, f32 scale, f32 tiltx)
{
    struct vehmask_s *mask = &VehicleMask[index];

    mask->killatend = killatend;
    if (mask->active != 0) {
        mask->tween = 1.0f;
        mask->tweeninc = tweeninc * 0.02f;
        mask->offset[1] = mask->offset[0];
        mask->anginc[1] = mask->anginc[0];
        mask->rad[1] = mask->rad[0];
        mask->scale[1] = mask->scale[0];
        mask->tiltx[1] = mask->tiltx[0];
        if (mask->point[0] == &mask->store[0]) {
            mask->point[1] = &mask->store[1];
            mask->store[1] = *mask->point[0];
        } else {
            mask->point[1] = mask->point[0];
        }
    } else {
        mask->tween = 0.0f;
        mask->active = 1;
        mask->point[1] = &v000;
    }
    if (fixed != 0) {
        mask->store[0] = *centre;
        mask->point[0] = &mask->store[0];
    } else {
        mask->point[0] = centre;
    }
    mask->offset[0] = *offset;
    mask->anginc[0] = anginc * 0.02f;
    mask->rad[0] = rad;
    mask->scale[0] = scale;
    mask->tiltx[0] = tiltx;
}

inline void ProcessCrashteroidsIntro(void)
{
    ProcessFireFlyIntro();
}

struct nuvec_s *GetWeatherBossPos(void)
{
    return D_005B44F8;
}

struct nugspline_s *GetWeatherBossSpline(void)
{
    return &D_005B4548;
}

extern s32 FlyingLevelExtro;
extern f32 GetBigGunBestTarget(f32 bestdot, struct nuvec_s **pos,
                               struct nuvec_s **vel, s32 *type);
extern f32 GetBattleShipBestTarget(f32 bestdot, struct nuvec_s **pos,
                                   struct nuvec_s **vel, s32 *type);
extern f32 GetGunBoatBestTarget(f32 bestdot, struct nuvec_s **pos,
                                struct nuvec_s **vel, s32 *type);
extern f32 GetZoffaBestTarget(f32 bestdot, struct nuvec_s **pos,
                              struct nuvec_s **vel, s32 *type);

inline s32 PickGliderTarget(struct nuvec_s **pos, struct nuvec_s **vel, s32 *type)
{
    f32 best;

    if (PlayerGlider.AutoPilot != 0 || FlyingLevelExtro != 0) {
        return 0;
    }
    if (Level == 0x24) {
        best = GetBigGunBestTarget(0.0f, pos, vel, type);
    } else {
        best = GetBattleShipBestTarget(0.0f, pos, vel, type);
        best = GetGunBoatBestTarget(best, pos, vel, type);
        best = GetZoffaBestTarget(best, pos, vel, type);
    }
    return best > 0.96f;
}

void GliderWeatherBossRailStuff(struct creature_s *c)
{
    if (best_cRPos != 0) {
        c->obj.RPos.pos = best_cRPos->pos;
    }
}

inline s32 GetCurrentFarmObjectives(void)
{
    s32 count = 0;
    s32 i;

    for (i = 0; i < 6; i++) {
        if (HovaBlimp[i].active != 0) {
            count++;
        }
    }
    return count;
}


s32 FindSurfaceType(struct nuvec_s *pos)
{
    struct nuvec_s probe = *pos;

    probe.y += 0.5f;
    if (fabs(pos->y - NewShadowMaskPlat(&probe, 0.0f, -1)) < 0.05f) {
        return ShadowInfo();
    }
    return -1;
}

void InitWeatherBoss(void)
{
    InitWeatherBoss_a();
}

extern struct MYDRAW IconMainDraw;
extern s32 BazookaIconOn;
extern struct nuvec_s BazookaTokenCurrentPos;
extern s32 MyDrawModelNew(struct MYDRAW *Draw, struct numtx_s *mC,
                          struct numtx_s *loc_mtx);

struct plane_s {
    struct nuvec_s pos;        /* 0x00 */
    struct nuvec_s vel;        /* 0x0C */
    f32 actiontimer;           /* 0x18 */
    s32 actionstatus;          /* 0x1C */
    s32 active;                /* 0x20 */
    struct MYDRAW draw;        /* 0x24 */
};

extern struct nuvec_s D_005A6DC0;
#define plane_scale D_005A6DC0

inline void DrawPlane(struct plane_s *plane)
{
    if (plane->active != 0) {
        NuMtxSetScale(&mTEMP, &plane_scale);
        NuMtxTranslate(&mTEMP, &plane->pos);
        MyDrawModelNew(&plane->draw, &mTEMP, 0);
    }
}

extern struct plane_s PlayerPlane;

inline void InitPlane(struct nuvec_s *pos, f32 speed, f32 time)
{
    memset(&PlayerPlane, 0, sizeof(struct plane_s));
    PlayerPlane.active = 1;
    if (MyInitModelNew(&PlayerPlane.draw, 0x51, 0x22, 0, 0, &PlayerPlane.pos) == 0) {
        PlayerPlane.active = 0;
    }
    PlayerPlane.pos = *pos;
    PlayerPlane.vel = SetNuVec(0.0f, 0.0f, -speed);
    PlayerPlane.actionstatus = 0;
    PlayerPlane.actiontimer = time;
}

extern void DrawZoffaUFOs(void);
extern void DrawAllHovaBlimps(void);
extern void DrawGliderBullets(void);
extern void DrawGliderTarget(void);

void DrawFarmLevelExtra(void)
{
    DrawZoffaUFOs();
    DrawPlane(&PlayerPlane);
    DrawAllHovaBlimps();
    DrawGliderBullets();
    DrawGliderTarget();
}

extern struct nuvec_s BazookaTokenPos;
extern void MyAnimateModelNew(struct MYDRAW *draw, f32 dt);

inline void ProcessBazookaToken(void)
{
    struct nuvec_s pos;
    f32 t;

    if (BazookaIconOn) {
        t = WeatherBoss.draw.Anim.anim_time /
            WeatherBoss.draw.Model->anmdata[WeatherBoss.draw.Anim.action]->time;
        NuVecScale(&pos, &BazookaTokenPos, 1.0f - t);
        NuVecScaleAccum(&pos, &PlayerGlider.Position, t);
        pos.y += t * 0.75f;
        BazookaTokenCurrentPos = pos;
        MyAnimateModelNew(&IconMainDraw, 0.59999996f);
    }
}


inline void DrawBazookaToken(void)
{
    if (BazookaIconOn) {
        NuMtxSetTranslation(&mTEMP, &BazookaTokenCurrentPos);
        MyDrawModelNew(&IconMainDraw, &mTEMP, 0);
    }
}

s32 WeatherBossNextAction(void)
{
    s32 action;
    s32 next;
    s32 tries;

    if (WeatherBoss.nextaction == 0) {
        next = 0;
        tries = 1;
        action = WeatherBoss.action;
        do {
            action++;
            if (action > 4) {
                action = 1;
            }
            if (WeatherBoss.objectives[action - 1] > 0) {
                next = action;
            }
        } while (tries++ < 4 && next == 0);
        return next;
    }
    WeatherBoss.action = WeatherBoss.nextaction;
    return 0;
}

inline s32 GetCurrentWeatherBossObjectives(void)
{
    s32 total;
    s32 i;

    if (WeatherBoss.active != 0) {
        total = 0;
        for (i = 0; i < 4; i++) {
            if (WeatherBoss.objectives[i] > 0) {
                total += WeatherBoss.objectives[i];
            }
        }
        return (total + 3) / 4;
    }
    return 0;
}

inline void ProcessWeatherBoss(void)
{
    if (WeatherBoss.active != 0) {
        ProcessWeatherBoss_a(&WeatherBoss);
    }
}

inline void DrawWeatherBoss(void)
{
    if (WeatherBoss.active != 0) {
        DrawWeatherBoss_a(&WeatherBoss);
    }
}

extern void ProcessWBIntro(void);
extern void ProcessGliderBullets(void);
extern void ProcessLighteningHail(void);
extern void ProcessWBBolts(void);
extern void ProcessVehMasks(void);
extern f32 AtmosphericPressureHackedZ;
extern struct nuvec_s WBIntroGliderPos;
extern s32 WBIntroOn;

void ProcessWeatherBossLevel(struct nupad_s *pad)
{
    ProcessWBIntro();
    if (WBIntroOn == 0) {
        AtmosphericPressureHackedZ -= 1.92f;
        if (AtmosphericPressureHackedZ < -1200.0f) {
            AtmosphericPressureHackedZ += 1200.0f;
        }
    } else {
        AtmosphericPressureHackedZ = WBIntroGliderPos.z;
        if (WBIntroGliderPos.z < -1200.0f) {
            AtmosphericPressureHackedZ += 1200.0f;
        }
        if (AtmosphericPressureHackedZ > 0.0f) {
            AtmosphericPressureHackedZ -= 1200.0f;
        }
    }
    ProcessGliderBullets();
    ProcessWeatherBoss();
    ProcessLighteningHail();
    ProcessWBBolts();
    ProcessVehMasks();
    ProcessBazookaToken();
}

s32 GetCurrentRumbleObjectives(void)
{
    return D_005B7CA0[0];
}

void InitJeepRocks(void)
{
    s32 i;

    for (i = 5; i >= 0; i--) {
        JeepRock[i].exists = 0;
    }
}

extern f32 RumbleCamTween;
extern f32 RumbleCamTweenDest;
extern f32 RumbleCamTweenInterest;
extern f32 RUMZOOM;

void UpdateRumbleCamTween(void)
{
    if (RumbleCamTweenDest > RumbleCamTween) {
        RumbleCamTween += 0.02f;
        if (RumbleCamTween > RumbleCamTweenDest) {
            RumbleCamTween = RumbleCamTweenDest;
        }
    }
    if (RumbleCamTweenDest < RumbleCamTween) {
        RumbleCamTween -= 0.02f;
        if (RumbleCamTween < RumbleCamTweenDest) {
            RumbleCamTween = RumbleCamTweenDest;
        }
    }
    if (EarthBoss.ball_action != 7 || EarthBoss.ball_health > 0) {
        RumbleCamTweenInterest = (RumbleCamTween - 0.25f) * RUMZOOM;
    }
    if (RumbleCamTweenInterest < 0.0f) {
        RumbleCamTweenInterest = 0.0f;
    }
}

/* Replace a rock with two smaller ones flying apart. */
void SmashRockIntoTwo(struct jeeprock_s *rock)
{
    struct nuvec_s vel;
    struct nuvec_s pos;
    f32 radius;

    radius = rock->atlas.ball_radius / 1.2599f; /* half the volume each */
    vel = rock->vel;
    pos = rock->pos;
    pos.x -= radius * 0.5f;
    vel.x -= 2.0f;
    AddRockVel(&pos, &vel, radius, 0);
    pos.x += radius * 1.01f;
    vel.x += 4.0f;
    AddRockVel(&pos, &vel, radius, 0);
    rock->explode = 1;
}

void InitRumblePanel(void)
{
    NumRockPanel = 0;
}

void AddRock(struct nuvec_s *pos, f32 radius, s32 owner)
{
    AddRockVel(pos, &v000, radius, owner);
}


extern struct objtab_s ObjTab[];
extern s32 NuRndrGScnObj(void *gobj, struct numtx_s *m);

struct gliderbomb_s {
    struct nuvec_s vel;        /* 0x00 */
    struct nuvec_s pos;        /* 0x0C */
    f32 angy;                  /* 0x18 */
    s32 life;                  /* 0x1C */
    f32 gravity;               /* 0x20 */
    struct nuvec_s target;     /* 0x24 */
    struct nuvec_s *targetpntr;    /* 0x30 */
    struct nuvec_s *targetvelpntr; /* 0x34 */
    s32 targetmoving;          /* 0x38 */
    s32 sinking;               /* 0x3C */
    f32 droptimer;             /* 0x40 */
};
extern struct gliderbomb_s GliderBombs[10];
extern struct gliderbomb_s EnemyGliderBombs[30];

static inline struct gliderbomb_s *GrabGliderBomb(s32 enemy)
{
    struct gliderbomb_s *bombs;
    s32 count;
    s32 i;

    if (enemy) {
        bombs = EnemyGliderBombs;
        count = 30;
    } else {
        bombs = GliderBombs;
        count = 10;
    }
    for (i = 0; i < count; i++) {
        if (bombs[i].life == 0) {
            return &bombs[i];
        }
    }
    return 0;
}

s32 AddGliderBomb(struct nuvec_s *pos, struct nuvec_s *vel, f32 angy, s32 enemy,
                  struct nuvec_s *targetpoint, struct nuvec_s *targetvel, s32 moving)
{
    struct gliderbomb_s *bomb;

    bomb = GrabGliderBomb(enemy);
    if (bomb != 0) {
        bomb->life = 500;
        bomb->pos = *pos;
        bomb->vel = *vel;
        NuVecScale(&bomb->vel, vel, 0.02f);
        bomb->angy = angy;
        if (enemy) {
            bomb->gravity = -0.004f;
        } else {
            bomb->gravity = -0.004f;
            if (Level == 0x24) {
                bomb->targetpntr = targetpoint;
                bomb->targetmoving = enemy;
                bomb->droptimer = 0.0f;
                bomb->targetvelpntr = &v000;
            } else if (moving) {
                bomb->targetmoving = 1;
                bomb->targetpntr = targetpoint;
                bomb->targetvelpntr = targetvel;
                bomb->droptimer = 0.35f;
            } else {
                bomb->droptimer = 0.35f;
                bomb->target = *targetpoint;
                bomb->targetpntr = &bomb->target;
                bomb->targetvelpntr = &v000;
                bomb->targetmoving = moving;
            }
        }
        return 1;
    }
    return 0;
}

extern f32 NuFsqrt(f32 x);

void DrawGliderBombs(void)
{
    struct numtx_s m;
    struct gliderbomb_s *bombs;
    s32 i;
    s32 j;
    s32 count;
    s32 obj;
    f32 horiz;

    bombs = GliderBombs;
    obj = 0x4D;
    count = 10;
    if (ObjTab[0x4D].obj.special == 0) {
        return;
    }
    for (i = 0; i < 2; i++) {
        for (j = 0; j < count; j++) {
            if (bombs[j].life != 0) {
                horiz = NuFsqrt(bombs[j].vel.x * bombs[j].vel.x + bombs[j].vel.z * bombs[j].vel.z);
                NuMtxSetRotationX(&m, NuAtan2D(bombs[j].vel.y * 100.0f, horiz * 100.0f));
                NuMtxRotateY(&m, bombs[j].angy * 182.04445f);
                NuMtxTranslate(&m, &bombs[j].pos);
                /* (ObjTab + obj), not ObjTab[obj]: the index form changes codegen. */
                NuRndrGScnObj((ObjTab + obj)->obj.scene->gobjs[(ObjTab + obj)->obj.special->instance->objid],
                              &m);
            }
        }
        obj = 0x4D; /* retail reloads the (same) object id for the enemy pass */
        bombs = EnemyGliderBombs;
        count = 30;
    }
}

extern void SeekHalfLife(f32 *dest, f32 target, f32 halflife, f32 dt);
extern void SeekHalfLifeNUVEC(struct nuvec_s *cur, struct nuvec_s *target, f32 halflife, f32 dt);
extern void AddGameDebris(s32 type, struct nuvec_s *pos);
extern void AddGameDebrisRot(s32 i, struct nuvec_s *pos, s32 n, u16 xrot, u16 yrot);
extern void MyGameSfxLoopVolPitch(s32 id, struct nuvec_s *pos, s32 vol, s32 pitch);
extern s32 SINKTORPVOL;
extern s32 SINKTORPPITCH;

void ProcessGliderBombs(void)
{
    struct nuvec_s vel;
    struct nuvec_s dir;
    struct nuvec_s rel;
    struct nuvec_s pos;
    struct gliderbomb_s *bombs;
    struct gliderbomb_s *bomb;
    s32 count;
    s32 enemy;
    s32 i;
    s32 sfx;
    f32 floor;
    f32 speed;
    f32 dist;

    bombs = GliderBombs;
    count = 10;
    floor = -30.0f;
    for (enemy = 0; enemy < 2; enemy++) {
        for (i = 0; i < count; i++) {
            bomb = &bombs[i];
            if (bomb->life == 0) {
                continue;
            }
            bomb->life--;
            vel = bomb->vel;
            if (NewRayCast(&bomb->pos, &vel, 0.0f)) {
                bomb->life = 0;
                NuVecAdd(&bomb->pos, &bomb->pos, &bomb->vel);
                AddGameDebris(0x43, &bomb->pos);
                if (Level == 0x12) {
                    sfx = (s32)(frand() * 3.0f);
                    if (sfx >= 3) {
                        sfx = 0;
                    }
                    MyGameSfx(sfx + 0xD7, &bomb->pos, 0x2FFF);
                }
                continue;
            }
            pos = bomb->pos;
            AddGameDebrisRot(0x45, &pos, 1, 0, 0);
            NuVecAdd(&bomb->pos, &bomb->pos, &bomb->vel);
            if (bomb->pos.y < floor && Level == 0x12) {
                bomb->pos.y = 0.0f;
                bomb->life = 0;
                AddGameDebris(0x35, &bomb->pos);
                continue;
            }
            if (enemy == 0 && bomb->sinking == 0 && bomb->pos.y < 0.0f && Level == 0x12) {
                bomb->sinking = 1;
                continue;
            }
            if (enemy == 1) {
                SeekHalfLife(&bomb->vel.x, 0.0f, 3.0f, 0.02f);
                SeekHalfLife(&bomb->vel.z, 0.0f, 3.0f, 0.02f);
                bomb->vel.y += bomb->gravity;
                continue;
            }
            if (bomb->targetpntr->y > 100000.0f) {
                bomb->target = bomb->pos;
                bomb->target.y = -100.0f;
                bomb->targetpntr = &bomb->target;
            }
            if (ProcessTimer(&bomb->droptimer)) {
                SeekHalfLife(&bomb->gravity, 0.0f, 0.15f, 0.02f);
                bomb->vel.y += bomb->gravity;
                speed = NuVecMag(&bomb->vel);
                NuVecScale(&dir, &bomb->vel, 1.0f / speed);
                speed += 0.02f;
                if (speed > 6.0f) {
                    speed = 6.0f;
                }
                NuVecSub(&rel, bomb->targetpntr, &bomb->pos);
                dist = NuVecMag(&rel);
                if (speed != 0.0f) {
                    NuVecScaleAccum(&rel, bomb->targetvelpntr, dist * 0.02f / speed);
                }
                NuVecNorm(&rel, &rel);
                SeekHalfLifeNUVEC(&dir, &rel, 0.1f, 0.02f);
                NuVecNorm(&dir, &dir);
                NuVecScale(&bomb->vel, &dir, speed);
            } else {
                bomb->vel.y += bomb->gravity;
            }
            if (enemy == 0 && Level == 0x12) {
                MyGameSfxLoopVolPitch(0xCA, &bomb->pos, SINKTORPVOL, SINKTORPPITCH);
            }
        }
        bombs = EnemyGliderBombs;
        count = 30;
        floor = 0.0f;
    }
}


extern struct nuvec_s GliderBombCollisionPos;

/* Tests the live bombs against an ellipsoid of half-size Extent around Pos,
   turned by Angle degrees; the first bomb inside is used up. */
s32 CollideGliderBombs(struct nuvec_s *Pos, struct nuvec_s *Extent, s32 Enemy, f32 Angle)
{
    struct nuvec_s delta;
    struct gliderbomb_s *bombs;
    struct gliderbomb_s *bomb;
    s32 numbombs;
    s32 i;
    f32 maxext;
    f32 negmax;

    maxext = (Extent->x > Extent->y) ? Extent->x : Extent->y;
    if (Extent->z > maxext) {
        maxext = Extent->z;
    }
    negmax = -maxext;

    if (Enemy != 0) {
        numbombs = 30;
        bombs = EnemyGliderBombs;
    } else {
        numbombs = 10;
        bombs = GliderBombs;
    }

    for (i = 0; i < numbombs; i++) {
        bomb = &bombs[i];
        if (bomb->life != 0) {
            delta.x = bomb->pos.x - Pos->x;
            if (delta.x <= maxext && delta.x >= negmax) {
                delta.y = bomb->pos.y - Pos->y;
                if (delta.y <= maxext && delta.y >= negmax) {
                    delta.z = bomb->pos.z - Pos->z;
                    if (delta.z <= maxext && delta.z >= negmax) {
                        NuVecRotateY(&delta, &delta, (s32)(-Angle * 182.04445f));
                        delta.x /= Extent->x;
                        delta.y /= Extent->y;
                        delta.z /= Extent->z;
                        if (delta.x * delta.x + delta.y * delta.y + delta.z * delta.z <= 1.0f) {
                            bomb->life = 0;
                            GliderBombCollisionPos = bomb->pos;
                            return 1;
                        }
                    }
                }
            }
        }
    }
    return 0;
}

extern f32 WeatherBossTargetAppearTimer;
extern f32 WeatherBossTargetAppearTime;
extern f32 WBTARGSCALE;
extern struct numtx_s WeatherBossCamMtx;
extern struct nuvec_s WeatherBossTargetPos;

void DrawWeatherBossTarget(void)
{
    s32 obj;
    f32 scale;

    if (WBIntroOn == 0 && FlyingLevelExtro == 0) {
        scale = (1.0f - WeatherBossTargetAppearTimer / WeatherBossTargetAppearTime) * WBTARGSCALE;
        obj = 0x5A;
        /* (ObjTab + obj), not ObjTab[obj]: the index form changes codegen. */
        if ((ObjTab + obj)->obj.special != 0) {
            mTEMP = WeatherBossCamMtx;
            NuVecAdd((struct nuvec_s *)&mTEMP._30, (struct nuvec_s *)&mTEMP._30,
                     &WeatherBossTargetPos);
            NuMtxPreScale(&mTEMP, SetNuVecPntr(scale, scale, 0.05f));
            NuRndrGScnObj((ObjTab + obj)->obj.scene->gobjs[(ObjTab + obj)->obj.special->instance->objid],
                          &mTEMP);
        }
    }
}

inline void DrawJeepRock(struct jeeprock_s *rock)
{
    struct objtab_s *entry;
    struct nuspecial_s *special;
    s32 obj;

    NuQuatToMtx(&rock->atlas.rotquat, &mTEMP);
    NuMtxPreScale(&mTEMP, &rock->scale);
    NuMtxTranslate(&mTEMP, &rock->atlas.ball_pos);
    if (Level == 0x16) {
        obj = 0x58;
    } else if (rock->owner == 20 || rock->owner == 1) {
        obj = 0xA9;
    } else {
        obj = (rock->owner == -1) ? 0xAB : 0xAA;
    }
    rock->seen = 0;
    entry = &ObjTab[obj];
    special = entry->obj.special;
    if (special != 0) {
        rock->seen = NuRndrGScnObj(entry->obj.scene->gobjs[special->instance->objid],
                                   &mTEMP);
    }
}

void DrawJeepRocks(void)
{
    s32 i;

    for (i = 0; i < 6; i++) {
        if (JeepRock[i].exists != 0) {
            DrawJeepRock(&JeepRock[i]);
        }
    }
}

extern void ProcessJeepRock(struct jeeprock_s *rock);

void ProcessJeepRocks(void)
{
    s32 i;
    f32 power;
    f32 dist;

    power = 0.0f;
    for (i = 0; i < 6; i++) {
        if (JeepRock[i].exists != 0) {
            ProcessJeepRock(&JeepRock[i]);
        }
    }
    if (Level == 0x16) {
        for (i = 0; i < 6; i++) {
            if (JeepRock[i].exists != 0 && JeepRock[i].atlas.ball_radius >= 0.3f &&
                (JeepRock[i].atlas.onground & 3) != 0) {
                dist = NuVecDist(&JeepRock[i].atlas.ball_pos, &player->obj.pos, 0) *
                       JeepRock[i].atlas.ball_radius / 0.4f;
                if (dist < 1.0f) {
                    power = 255.0f;
                } else {
                    power += 255.0f / dist;
                }
            }
        }
        if (power > 255.0f) {
            power = 255.0f;
        }
        NewRumble(&player->rumble, power);
    }
}

/* ------------------------------------------------------------------ */

/* BattleShipList: stride 0x554, 6 entries. */
struct battleship_s {
    s32 active;                /* 0x000 */
    struct MYDRAW draw;        /* 0x004 */
    struct numtx_s locators[16]; /* 0x0E4 */
    struct nuvec_s pos;        /* 0x4E4 */
    f32 yaw;                   /* 0x4F0 */
    f32 pitch;                 /* 0x4F4 */
    f32 roll;                  /* 0x4F8 */
    f32 destpitch;             /* 0x4FC */
    f32 destroll;              /* 0x500 */
    f32 desty;                 /* 0x504 */
    f32 seek;                  /* 0x508 */
    s32 seen;                  /* 0x50C */
    s32 hitpoints;             /* 0x510 */
    f32 firetimer[2];          /* 0x514 */
    struct nuvec_s gooscale;   /* 0x51C */
    char pad_528[0x540 - 0x528];
    f32 goospeed;              /* 0x540 */
    struct nuvec_s gootimer;   /* 0x544 */
    char pad_550[0x554 - 0x550];
};
extern struct battleship_s BattleShipList[6];

extern struct cammtx_s GameCam[];

extern f32 DotProduct(struct nuvec_s *a, struct nuvec_s *b);
extern void InitBattleShip(struct nuvec_s *pos, f32 angle, s32 index);

/* Whether `target` is still worth a lock-on: more than 10 units ahead of the
 * camera, within ~25 degrees of its view and with a clear line to the glider.
 * Near-match (state=asm): with NuVecSub prototyped (it used to be implicitly
 * declared here) the instruction stream equals retail's apart from the two
 * SN-as mtc1 hazard nops. */
s32 StillLockedOnTarget(struct nuvec_s *target)
{
    struct nuvec_s *fwd;
    struct nuvec_s view;
    struct nuvec_s rel;
    struct nuvec_s ray;
    f32 dot;
    f32 dist;

    if (FlyingLevelExtro == 0) {
        fwd = SetNuVecPntr(0.0f, 0.0f, 1.0f);
        NuVecMtxRotate(&view, fwd, &GameCam[0].m);
        NuVecSub(&rel, target, &GameCam[0].pos);
        dist = NuVecMag(&rel);
        if (10.0f < dist) {
            NuVecScale(&rel, &rel, 1.0f / dist);
            dot = DotProduct(&view, &rel);
            if (0.9f < dot) {
                NuVecSub(&ray, &PlayerGlider.Position, target);
                NuVecNorm(&ray, &ray);
                NuVecScale(&ray, &ray, 3.0f);
                return NewRayCast(target, &ray, 0.0f) == 0;
            }
        }
    }
    return 0;
}

void InitBattleShip(struct nuvec_s *pos, f32 angle, s32 index)
{
    struct battleship_s *ship;

    ship = &BattleShipList[index];
    if (ship->active != 0) {
        return;
    }
    memset(&BattleShipList[index], 0, sizeof(struct battleship_s));
    if (MyInitModelNew(&ship->draw, 0x65, 0x22, 0, 0, &ship->pos) == 0) {
        return;
    }
    ship->pos = *pos;
    ship->desty = pos->y;
    ship->yaw = angle;
    ship->pitch = 0.0f;
    ship->roll = 0.0f;
    ship->destpitch = 0.0f;
    ship->destroll = 0.0f;
    ship->seek = 1.0f;
    ship->active = 1;
    ship->hitpoints = 3;
    ship->firetimer[0] = frand() * 4.0f;
    ship->firetimer[1] = frand() * 4.0f;
    ship->gooscale = SetNuVec(0.0f, 0.0f, 0.0f);
    ship->gootimer.x = (f32)index / 6.0f;
    ship->gootimer.y = (f32)(index + 1) / 6.0f;
    ship->goospeed = (ship->gootimer.x * 0.1f + 0.2f) * 0.02f;
    if (1.0f <= ship->gootimer.y) {
        ship->gootimer.y -= 1.0f;
    }
    ship->gootimer.z = (f32)(index + 2) / 6.0f;
    if (1.0f <= ship->gootimer.z) {
        ship->gootimer.z -= 1.0f;
    }
}

void InitBattleShips(void)
{
    memset(BattleShipList, 0, sizeof(BattleShipList));
    InitBattleShip(SetNuVecPntr(-7.0f, 0.0f, 15.0f), -150.0f, 0);
    InitBattleShip(SetNuVecPntr(-7.33f, 0.0f, 73.5f), -45.0f, 1);
    InitBattleShip(SetNuVecPntr(31.0f, 0.0f, 109.0f), -35.0f, 2);
    InitBattleShip(SetNuVecPntr(94.0f, 0.0f, 54.0f), 90.0f, 3);
    InitBattleShip(SetNuVecPntr(60.0f, 0.0f, -6.0f), 160.0f, 4);
    InitBattleShip(SetNuVecPntr(20.0f, 0.0f, -3.0f), -150.0f, 5);
}
extern s32 CollideWithBattleShip(struct nuvec_s *pos, struct battleship_s *ship,
                                 f32 radius);

s32 CollideWithBattleShips(struct nuvec_s *pos, f32 radius)
{
    s32 i;

    for (i = 0; i < 6; i++) {
        if (BattleShipList[i].active != 0 &&
            CollideWithBattleShip(pos, &BattleShipList[i], radius)) {
            return 1;
        }
    }
    return 0;
}


inline void DrawBattleShip(struct battleship_s *ship)
{
    NuMtxSetRotationX(&mTEMP, ship->pitch * 182.04445f);
    NuMtxRotateZ(&mTEMP, ship->roll * 182.04445f);
    NuMtxRotateY(&mTEMP, ship->yaw * 182.04445f);
    NuMtxTranslate(&mTEMP, &ship->pos);
    ship->seen = MyDrawModelNew(&ship->draw, &mTEMP, ship->locators);
}

/* BigGunList: stride 0x154, 12 entries. */
struct biggun_s {
    s32 active;                /* 0x000 */
    char pad_004[0x0E4 - 0x004];
    struct nuvec_s pos;        /* 0x0E4 */
    char pad_0F0[0x0F8 - 0x0F0];
    f32 yaw;                   /* 0x0F8 */
    f32 pitch;                 /* 0x0FC */
    f32 yawwobble;             /* 0x100 */
    char pad_104[0x120 - 0x104];
    s32 field120;              /* 0x120 */
    char pad_124[0x154 - 0x124];
};
extern struct biggun_s BigGunList[];

extern struct nuvec_s D_0061F460[];
#define gun_scale D_0061F460 /* {1.5f, 1.5f, 1.5f}: big gun / gun boat model scale */

inline void DrawBigGun(struct biggun_s *gun)
{
    struct nuvec_s scale = gun_scale[0];

    NuMtxSetScale(&mTEMP, &scale);
    NuMtxRotateX(&mTEMP, gun->pitch * 182.04445f);
    NuMtxRotateY(&mTEMP, (gun->yaw + gun->yawwobble) * 182.04445f);
    NuMtxTranslate(&mTEMP, &gun->pos);
}

inline void DrawBigGuns(void)
{
    s32 i;

    for (i = 0; i < 12; i++) {
        if (BigGunList[i].active != 0) {
            DrawBigGun(&BigGunList[i]);
        }
    }
}

/* SpaceStationList: stride 0xD0, 3 entries. */
struct spacestation_s {
    struct numtx_s mtx;
    s32 active;                /* 0x40 */
    struct nuvec_s pos;        /* 0x44 */
    struct nuvec_s vel;        /* 0x50 */
    f32 angle;                 /* 0x5C */
    s32 seen;                  /* 0x60 */
    s32 hitpoints;             /* 0x64 */
    s32 explode;               /* 0x68 */
    f32 satelliteangle;        /* 0x6C */
    struct satellite_s *satellites[3]; /* 0x70 */
    struct numtx_s shieldmtx;  /* 0x7C */
    s32 shielddraw;            /* 0xBC frames left to show the shield */
    s32 shieldgreen;           /* 0xC0 */
    struct nuvec_s appcentre;  /* 0xC4 */
};
extern struct spacestation_s SpaceStationList[3];

/* trail array: stride 0x20, 128 entries. */
struct trail_s {
    f32 field0;                /* 0x00 */
    char pad_04[0x18 - 0x04];
    s32 alpha;                 /* 0x18 */
    s32 basealpha;             /* 0x1C */
};
extern struct trail_s trail[128];
extern f32 D_0062DF00;
extern s32 trailpt;
extern s32 trailair;

extern struct NEWBUGGY PlayerAtlas;   /* far */
extern void ProcessAtlasAtlasCollisions_a(struct NEWBUGGY *a, struct NEWBUGGY *b);
extern void KillGameObject(struct obj_s *obj, s32 type);

inline s32 GetCurrentFireFlyObjectives(void)
{
    s32 count = 0;
    s32 i;

    for (i = 0; i < 6; i++) {
        if (BattleShipList[i].active != 0 && BattleShipList[i].hitpoints > 0) {
            count++;
        }
    }
    return count;
}

extern struct nuvec_s GliderTargetPos;

void DrawGliderTarget(void)
{
    struct nuvec_s centre;
    struct nuvec_s dir;
    s32 obj = 0x72;

    NuVecSub(&dir, &GliderTargetPos, &GameCam[0].pos);
    NuVecScale(&centre, &dir, 5.0f / NuVecMag(&dir));
    NuVecAdd(&centre, &centre, (struct nuvec_s *)&GameCam[0].m._30);
    /* (ObjTab + obj), not ObjTab[obj]: the index form changes codegen. */
    if ((ObjTab + obj)->obj.special != 0) {
        mTEMP = GameCam[0].m;
        NuMtxPreScale(&mTEMP, SetNuVecPntr(0.2f, 0.2f, 0.0f));
        *(struct nuvec_s *)&mTEMP._30 = centre;
        NuRndrGScnObj((ObjTab + obj)->obj.scene->gobjs[(ObjTab + obj)->obj.special->instance->objid],
                      &mTEMP);
    }
}

extern void DrawTorpedoTarget(void);

void DrawWeatherResearchLevelExtra(void)
{
    DrawGliderBullets();
    DrawBigGuns();
    DrawGliderBombs();
    DrawTorpedoTarget();
}

inline s32 GetCurrentWeatherResearchObjectives(void)
{
    s32 count = 0;
    s32 i;

    for (i = 0; i < 12; i++) {
        if (BigGunList[i].active != 0) {
            if (BigGunList[i].field120 != 2) {
                count++;
            }
        }
    }
    return count;
}

inline s32 GetCurrentSpaceArenaObjectives(void)
{
    s32 count = 0;
    s32 i;

    for (i = 0; i < 3; i++) {
        if (SpaceStationList[i].active != 0) {
            count++;
        }
    }
    return count;
}

void KillAtlasphere(struct NEWBUGGY *b)
{
    struct creature_s *c = b->owner;
    struct creature_s *o;

    if (c != 0) {
        b->dead = 1;
        c->obj.invincible = 0;
        b->ball_vel = v000;
        o = b->owner;
        if (o->obj.dead == 0) {
            KillGameObject(&o->obj, 0xB);
        }
    }
}

void InitTrail(void)
{
    s32 i;

    for (i = 127; i >= 0; i--) {
        trail[i].field0 = D_0062DF00;
    }
    trailpt = 0;
    trailair = 0;
}

extern struct nuquat_s SetNuQuat(f32 x, f32 y, f32 z, f32 w);

void InitAtlas(struct NEWBUGGY *Atlas, struct nuvec_s *Pos, f32 Radius, s32 Type)
{
    struct creature_s *cre;
    struct CharacterData *cdata;

    cre = Atlas->owner;
    memset(Atlas, 0, sizeof(struct NEWBUGGY));
    Atlas->type = Type;
    Atlas->owner = cre;
    Atlas->ball_pos = *Pos;
    Atlas->ball_oldpos = Atlas->ball_pos;
    Atlas->ball_radius = Radius;
    Atlas->angley = 90.0f;
    Atlas->rotquat = SetNuQuat(0.0f, 0.0f, 0.0f, 1.0f);
    Atlas->framequat[0] = Atlas->rotquat;
    Atlas->framequat[1] = Atlas->rotquat;
    Atlas->framequat[2] = Atlas->rotquat;
    Atlas->framequat[3] = Atlas->rotquat;
    Atlas->lastquat = Atlas->rotquat;
    Atlas->lastnormal = SetNuVec(0.0f, 1.0f, 0.0f);
    Atlas->platformid = -1;
    if (cre != 0) {
        InitTrail();
        Atlas->owner->obj.scale = Atlas->owner->obj.SCALE = 1.0f;
        cdata = &CData[83];
        Atlas->owner->obj.bot = cdata->min.y;
        Atlas->owner->obj.top = cdata->max.y;
        Atlas->owner->obj.vehicle = 0x53;
    }
}

extern struct gtimer_s GameTimer;

/* Turns the pad into the atlasphere's camera-relative steering input. */
void ControlAtlas(struct NEWBUGGY *Atlas, struct nupad_s *Pad, f32 DeltaTime)
{
    struct nuvec_s ctrl;
    f32 anx;
    f32 anz;
    f32 scale;

    anx = (f32)(Pad->rdata.l_alg_x - 0x7F);
    anz = (f32)(Pad->rdata.l_alg_y - 0x7F);
    if (Pad->paddata & PAD_DOWN) {
        anz = 127.0f;
    }
    if (Pad->paddata & PAD_UP) {
        anz = -127.0f;
    }
    if (Pad->paddata & PAD_RIGHT) {
        anx = 127.0f;
    }
    if (Pad->paddata & PAD_LEFT) {
        anx = -127.0f;
    }
    if (ProcessTimer(&Atlas->inhibitcontroltimer) == 0) {
        if (Atlas->inhibitcontroltimer < 1.0f) {
            scale = 1.0f - Atlas->inhibitcontroltimer;
        } else {
            scale = 0.0f;
        }
        anx *= scale;
        anz *= scale;
    }
    if ((Atlas->onground & 3) == 0 && Level == 0x15) {
        anx = 0.0f;
        anz = 0.0f;
    }
    ctrl.x = anx;
    ctrl.z = -anz;
    ctrl.y = 0.0f;
    if (GameTimer.frame < 50) {
        ctrl.x = ctrl.z = 0.0f;
    }
    Atlas->inputmag = NuVecMag(&ctrl);
    if (Atlas->inputmag >= 40.0f) {
        Atlas->inputang = ((GameCam[0].yrot + NuAtan2D(ctrl.x, ctrl.z)) & 0xFFFF) / 182.04445f;
        Atlas->inputmag = ((Atlas->inputmag - 40.0f) * 1.02f) / 88.0f;
        if (Atlas->inputmag > 1.0f) {
            Atlas->inputmag = 1.0f;
        } else if (Atlas->inputmag < -1.0f) {
            Atlas->inputmag = -1.0f;
        }
    } else {
        Atlas->inputmag = 0.0f;
    }
}

extern void ApplyFriction(f32 *val, f32 rate, f32 dt);

/* One physics step: friction, gravity tilted towards the stick, then the
   new target position and velocity. */
void ProcessMovementAtlas(struct NEWBUGGY *Atlas, f32 DeltaTime)
{
    struct nuvec_s resolved;
    f32 time2;
    f32 scale;
    f32 newmag;
    f32 mag;
    f32 tilt;

    time2 = DeltaTime * DeltaTime;
    mag = NuVecMag(&Atlas->ball_vel);
    if (mag > 0.0f) {
        newmag = mag;
        if (mag < 4.0f) {
            scale = 0.05f;
        } else {
            scale = (mag - 4.0f) * 0.1f;
        }
        ApplyFriction(&newmag, scale, DeltaTime);
        Atlas->ball_vel.x *= newmag / mag;
        Atlas->ball_vel.y *= newmag / mag;
        Atlas->ball_vel.z *= newmag / mag;
    }
    Atlas->ball_force = SetNuVec(0.0f, -10.0f, 0.0f);
    NuVecRotateY(&resolved, &Atlas->ball_vel, (s32)(-Atlas->inputang * 182.04445f));
    if (resolved.z < 0.0f) {
        resolved.z = 0.0f;
    }
    if (Level == 0x15) {
        tilt = -Atlas->inputmag * 89.0f;
    } else {
        tilt = -Atlas->inputmag * 53.0f;
    }
    /* The forward speed goes in as the time step, as in retail (and GC). */
    SeekHalfLife(&tilt, 0.0f, 3.0f, resolved.z);
    NuVecRotateX(&Atlas->ball_force, &Atlas->ball_force, (s32)(tilt * 182.04445f));
    NuVecRotateY(&Atlas->ball_force, &Atlas->ball_force, (s32)(Atlas->inputang * 182.04445f));
    Atlas->ball_target.x = Atlas->ball_pos.x + Atlas->ball_vel.x * DeltaTime + time2 * Atlas->ball_force.x * 0.5f;
    Atlas->ball_target.y = Atlas->ball_pos.y + Atlas->ball_vel.y * DeltaTime + time2 * Atlas->ball_force.y * 0.5f;
    Atlas->ball_target.z = Atlas->ball_pos.z + Atlas->ball_vel.z * DeltaTime + time2 * Atlas->ball_force.z * 0.5f;
    Atlas->ball_vel.x += Atlas->ball_force.x * DeltaTime;
    Atlas->ball_vel.y += Atlas->ball_force.y * DeltaTime;
    Atlas->ball_vel.z += Atlas->ball_force.z * DeltaTime;
}

extern struct nuvec_s D_0061F750[]; /* fixed ray direction, far from $gp */
extern f32 D_0062DF04;
extern s32 TryUnembeddPointDirSimple(struct nuvec_s *pos, struct nuvec_s *dir,
                                     s16 *handle, s32 n, f32 x, f32 y);

extern struct gdeb_s GDeb[];

extern s32 Paused;
extern s32 FlyingLevelVictoryDance;
extern void AddVariableShotDebrisEffect(s32 effect, struct nuvec_s *pos, s32 n,
                                        short xrot, short yrot);

/* On the jeep level, mark every rock within 0.75 of `pos + move` to explode.
 * Returns whether any was hit. */
s32 CheckAgainstRocks(struct nuvec_s *pos, struct nuvec_s *move)
{
    struct nuvec_s rel;
    struct nuvec_s p;
    s32 i;
    s32 hit;

    hit = 0;
    if (Level != 0x16) {
        return 0;
    }
    NuVecAdd(&p, pos, move);
    for (i = 0; i < 6; i++) {
        if (JeepRock[i].exists != 0) {
            NuVecSub(&rel, &p, &JeepRock[i].pos);
            if (NuVecMagSqr(&rel) < 0.5625f) {
                JeepRock[i].explode = 1;
                hit = 1;
            }
        }
    }
    return hit;
}

extern s32 JonnyOn;
extern s32 ebgo;
extern struct nuvec_s ebpos[3];
extern f32 ebtime[3];

void DrawJonny(void)
{
    struct numtx_s m;
    s32 i;

    if (JonnyOn == 1 && ebgo != 0) {
        for (i = 0; i < 3; i++) {
            if (ebpos[i].x != 99999.0f) {
                NuMtxSetRotationX(&m, ebtime[i] * 128.0f);
                NuMtxRotateZ(&m, ebtime[i] * 200.0f);
                NuMtxTranslate(&m, &ebpos[i]);
                NuSpecialDrawAt(&ObjTab[0x58].obj, &m);
            }
        }
    }
}

extern struct nuvec_s ShadNorm;
extern s32 TryUnembeddPointDir(struct nuvec_s *pos, struct nuvec_s *norm, struct nuvec_s *dir,
                               s16 *handle, f32 radius);
extern s32 TryUnembeddPointSafe(struct nuvec_s *pos, struct nuvec_s *safe, s16 *handle,
                                f32 radius);

/* Push the atlasphere out of the terrain along the ground normal, or back to
 * its last safe position if that fails. */
void UnembedRayCastAtlas(struct NEWBUGGY *b, s16 *handle)
{
    struct nuvec_s up = D_0061F750[0];

    if (!TryUnembeddPointDir(&b->ball_pos, &ShadNorm, &up, handle, b->ball_radius)) {
        TryUnembeddPointSafe(&b->ball_pos, &b->ball_oldpos, handle, b->ball_radius);
    }
}

void UnembedRayCastAtlasSimple(struct NEWBUGGY *b, s16 *handle)
{
    struct nuvec_s dir = D_0061F750[0];

    TryUnembeddPointDirSimple(&b->ball_pos, &dir, handle, 5, b->ball_radius,
                              D_0062DF04);
}

extern struct nuvec_s D_0061F8D0[];     /* vortex centre {0, -3.3, 0} */

/* Pull the atlasphere towards the vortex when it is close to it.
 * Inline: ProcessEarthBossVortex carries two copies of it in retail. */
inline void CheckAtlasVortex(struct NEWBUGGY *b)
{
    struct nuvec_s vortex = D_0061F8D0[0];
    struct nuvec_s rel;
    f32 dist;

    if (b->ball_pos.y < -3.0f) {
        vortex.y = b->ball_pos.y - 0.3f;
    }
    NuVecSub(&rel, &vortex, &b->ball_pos);
    dist = NuVecMag(&rel);
    if (dist < 1.0f) {
        /* Retail constant: one ulp below 0.1f. */
        NuVecScaleAccum(&b->ball_vel, &rel, 0.099999994f / dist);
    }
}

extern s32 DrawPanel3DObject(s32 object, f32 x, f32 y, f32 z, f32 scalex, f32 scaley,
                             f32 scalez, u16 xrot, u16 yrot, u16 zrot,
                             struct nugscn_s *scn, struct nuspecial_s *obj, s32 rot);

/* Boss health bar: the frame, then the bar scaled to health / maxhealth. */
void BossBar(f32 x, f32 y, f32 z, f32 xs, f32 ys, s32 health, s32 maxhealth)
{
    f32 scale;

    scale = (f32)health / (f32)maxhealth;
    if (1.0f < scale) {
        scale = 1.0f;
    }
    if (scale < 0.0f) {
        scale = 0.0f;
    }
    DrawPanel3DObject(0xB9, x, y, z, xs, ys, 0.1f, 0, 0, 0,
                      ObjTab[0xB9].obj.scene, ObjTab[0xB9].obj.special, 1);
    DrawPanel3DObject(0xBB, x, y, z, xs * scale, ys, 0.1f, 0, 0, 0,
                      ObjTab[0xBB].obj.scene, ObjTab[0xBB].obj.special, 1);
}

extern s32 EarthBossVortexOpen;

void ProcessEarthBossVortex(void)
{
    if (EarthBossVortexOpen != 0) {
        CheckAtlasVortex(&EarthBoss);
        CheckAtlasVortex(&PlayerAtlas);
    }
}

void ProcessAtlasAtlasCollisions(void)
{
    if (EarthBoss.ball_health > 0) {
        ProcessAtlasAtlasCollisions_a(&PlayerAtlas, &EarthBoss);
    }
}

/* Tests each edge of the closed polygon Points against an ellipsoid around
   Centre (squashed by RadiusY on y) and returns a bit mask of the edges hit. */
s32 LinesIntersectEllipse(struct nuvec_s *Points, s32 NumPoints, struct nuvec_s *Centre,
                          f32 Radius, f32 RadiusY)
{
    struct nuvec_s ToCenter;
    struct nuvec_s ToCenter2;
    struct nuvec_s Edge;
    struct nuvec_s ScaledPoints[16];
    s32 result = 0;
    s32 i;
    f32 dot;
    f32 Radius2;
    f32 perpDist;

    for (i = 0; i < NumPoints; i++) {
        ScaledPoints[i].x = Points[i].x;
        ScaledPoints[i].y = (Points[i].y - Centre->y) / RadiusY + Centre->y;
        ScaledPoints[i].z = Points[i].z;
    }
    ScaledPoints[i] = ScaledPoints[0];

    Radius2 = Radius * Radius;

    for (i = 0; i < NumPoints; i++) {
        NuVecSub(&Edge, &ScaledPoints[i + 1], &ScaledPoints[i]);
        NuVecSub(&ToCenter, Centre, &ScaledPoints[i]);
        dot = DotProduct(&ToCenter, &Edge);
        if (dot < 0.0f) {
            if (NuVecMagSqr(&ToCenter) < Radius2) {
                result |= 1 << i;
            }
        } else {
            NuVecSub(&ToCenter2, Centre, &ScaledPoints[i + 1]);
            if (DotProduct(&ToCenter2, &Edge) > 0.0f) {
                if (NuVecMagSqr(&ToCenter2) < Radius2) {
                    result |= 1 << i;
                }
            } else {
                perpDist = NuVecMagSqr(&ToCenter) - (dot * dot) / NuVecMagSqr(&Edge);
                if (perpDist < Radius2) {
                    result |= 1 << i;
                }
            }
        }
    }
    return result;
}

/* Returns 1 if two moving spheres will meet head-on within MaxTime, -1 if
   they meet while travelling the same way, 0 if they miss. */
inline s32 PossIntersect(struct nuvec_s *PosA, struct nuvec_s *VelA, struct nuvec_s *PosB,
                         struct nuvec_s *VelB, f32 Radius, f32 MaxTime)
{
    struct nuvec_s A;
    struct nuvec_s AV;
    f32 MagA;
    f32 MagVelA;
    f32 Angle;
    f32 Time;

    A.x = PosA->x - PosB->x;
    A.y = PosA->y - PosB->y;
    A.z = PosA->z - PosB->z;
    AV.x = VelB->x - VelA->x;
    AV.y = VelB->y - VelA->y;
    AV.z = VelB->z - VelA->z;

    MagA = NuVecMag(&A);
    if (MagA < Radius) {
        if (DotProduct(VelA, VelB) <= 0.0f) {
            return 1;
        }
        return -1;
    }

    MagVelA = NuVecMag(VelA);
    NuVecMag(&AV); /* result unused, but retail makes the call */

    Angle = -DotProduct(&A, VelA) / (MagA * MagVelA);
    if (Angle < 0.866f) {
        return 0;
    }

    Time = DotProduct(&A, &A) / DotProduct(&A, &AV);
    if (Time > 0.0f && Time < MaxTime) {
        if (DotProduct(VelA, VelB) <= 0.0f) {
            return 1;
        }
        return -1;
    }
    return 0;
}

void ProcessRockRockCollisions(void)
{
    s32 i;
    s32 j;

    for (i = 1; i < 6; i++) {
        if (JeepRock[i].exists != 0 && JeepRock[i].owner != 20) {
            for (j = 0; j < i; j++) {
                if (JeepRock[j].exists != 0) {
                    if (JeepRock[j].owner != 20) {
                        ProcessAtlasAtlasCollisions_a(&JeepRock[i].atlas, &JeepRock[j].atlas);
                    }
                }
            }
        }
    }
    if (Level == 0x15) {
        for (i = 0; i < 6; i++) {
            if (JeepRock[i].exists != 0) {
                if (JeepRock[i].owner != 20) {
                    ProcessAtlasAtlasCollisions_a(&JeepRock[i].atlas, &PlayerAtlas);
                    if (EarthBoss.ball_health > 0) {
                        ProcessAtlasAtlasCollisions_a(&JeepRock[i].atlas, &EarthBoss);
                    }
                }
            }
        }
    }
}

void FadeOutLastTrail(s32 last, s32 count)
{
    f32 step;
    f32 fade;
    f32 base;
    s32 i;
    s32 idx;

    step = 1.0f / (count + 1);
    fade = 0.0f;
    fade += step;
    for (i = 0; i < count; i++) {
        idx = (last - i) & 0x7F;
        if (trail[idx].field0 == -10000.0f) {
            return;
        }
        base = trail[idx].basealpha;
        trail[idx].alpha = base * fade;
        fade += step;
    }
}

/* ------------------------------------------------------------------ */

/* SatelliteList: stride 0x124, 9 entries. */
struct satellite_s {
    s32 active;                /* 0x00 */
    struct MYDRAW draw;
    struct nuvec_s pos;
    struct nuvec_s vel;
    f32 angle;
    f32 tilt[2];
    f32 tiltdest[2];
    s32 seen;                  /* 0x110 */
    s32 hits;
    s32 destroyed;
    struct spacestation_s *station;
    char pad_120[0x124 - 0x120];
};
extern struct satellite_s SatelliteList[9];

extern s32 CollideGliderBullets(struct nuvec_s *pos, f32 radius, s32 enemy, f32 yscale,
                                s32 a, s32 b);
extern void AddScreenWumpa(f32 x, f32 y, f32 z, s32 n);
extern void ClockOff(void);
extern void SeekAngHalfLife360f(f32 *dest, f32 target, f32 halflife, f32 dt);
extern void MyGameSfxLoop(s32 id, struct nuvec_s *pos, s32 vol);
extern short CRASHTEROIDSSATELLITEVOL;

void ProcessSatellite(struct satellite_s *sat)
{
    s32 id;
    short vol;
    struct nuvec_s vec;

    MyAnimateModelNew(&sat->draw, 0.59999996f);
    NuVecRotateY(&vec, SetNuVecPntr(0.0f, 0.0f, 18.0f),
                 (sat->angle + sat->station->satelliteangle) * 182.04445f);
    NuVecMtxTransform(&sat->pos, &vec, &sat->station->mtx);
    CollideGliderBullets(&sat->pos, 3.0f, 1, 0.6666667f, 0, 1);
    if (CollideGliderBullets(&sat->pos, 3.0f, 0, 0.6666667f, 0, 0) != 0) {
        sat->hits--;
        if (sat->hits > 0) {
            MyGameSfx(0xD4, &sat->pos, 0x3FFF);
        }
        AddScreenWumpa(sat->pos.x, sat->pos.y, sat->pos.z, 1);
        ClockOff();
    }
    if (sat->hits < 1 || sat->destroyed != 0) {
        sat->active = 0;
        AddGameDebris(0x17, &sat->pos);
        MyGameSfx(0xCD, &sat->pos, 0x7FFF);
    }
    SeekAngHalfLife360f(&sat->tilt[0], sat->tiltdest[0], 1.0f, 0.02f);
    SeekAngHalfLife360f(&sat->tilt[1], sat->tiltdest[1], 1.0f, 0.02f);
    id = -1;
    vol = 0;
    if (Level == 0x1A) {
        vol = CRASHTEROIDSSATELLITEVOL;
        id = 0xD6;
    }
    if (id != -1) {
        MyGameSfxLoop(id, &sat->pos, vol);
    }
}

inline void DrawSatellite(struct satellite_s *sat)
{
    mTEMP = sat->station->mtx;
    NuMtxPreRotateY(&mTEMP, sat->angle * 182.04445f);
    NuMtxPreScale(&mTEMP, SetNuVecPntr(3.0f, 3.0f, 3.0f));
    *(struct nuvec_s *)&mTEMP._30 = sat->pos;
    sat->seen = MyDrawModelNew(&sat->draw, &mTEMP, 0);
}

inline void DrawSatellites(void)
{
    s32 i;

    for (i = 0; i < 9; i++) {
        if (SatelliteList[i].active != 0) {
            DrawSatellite(&SatelliteList[i]);
        }
    }
}

extern void *app_fnt;

extern void NuRndrTrail(s32 pt, struct trail_s *trail, s32 count);
extern void DrawJeepTrails(void);
extern void NuFntSet(void *fnt);
extern void NuFntSetPen(s32 col);
extern void NuFntScale(s32 x, s32 y);
extern void ProcessSpaceStation(struct spacestation_s *s);

void DrawSpaceStation(struct spacestation_s *station)
{
    s32 obj;

    station->seen = 0;
    obj = 0x6D;
    /* (ObjTab + obj), not ObjTab[obj]: the index form changes codegen. */
    if ((ObjTab + obj)->obj.special != 0) {
        mTEMP = station->mtx;
        station->seen = NuRndrGScnObj((ObjTab + obj)->obj.scene->gobjs[(ObjTab + obj)->obj.special->instance->objid],
                                      &mTEMP);
    }
    if (station->seen != 0) {
        if (station->shielddraw == 0) {
            return;
        }
        obj = 0x71;
        if (station->shieldgreen != 0) {
            obj = 0x70;
        }
        if ((ObjTab + obj)->obj.special != 0) {
            mTEMP = station->shieldmtx;
            NuRndrGScnObj((ObjTab + obj)->obj.scene->gobjs[(ObjTab + obj)->obj.special->instance->objid], &mTEMP);
            NuMtxPreRotateY(&station->shieldmtx, 0x2000);
        }
    }
    if (station->shielddraw != 0) {
        station->shielddraw--;
    }
}

extern struct numtx_s *GliderBulletReflectMtx;
extern s32 HITSHIELDVOL;
extern s32 HITSHIELDVOLA;
extern s32 SPACESTATIONEXPLODEVOL;
extern f32 SHIELDSCALEY;
extern f32 SPACEMINSPEEDIN;
extern f32 SPACETORNADOSCALE;
extern f32 FIXVELBUMPAMOUNT;
extern f32 FIXVELTIME;

void ProcessSpaceStation(struct spacestation_s *station)
{
    struct numtx_s reflect;
    struct nuvec_s delta;
    struct nuvec_s rotated;
    struct nuvec_s hitpos;
    struct nuvec_s unscaled;
    struct nuvec_s offset;
    f32 dist;
    f32 dot;
    f32 speed;
    f32 mag;
    f32 angy;
    f32 angx;
    s32 shielded;
    s32 i;

    shielded = 0;
    for (i = 0; i < 3; i++) {
        if (station->satellites[i] != 0 && station->satellites[i]->active != 0) {
            shielded = 1;
        }
    }
    GliderBulletReflectMtx = 0;
    if (CollideGliderBullets(&station->appcentre, 11.0f, 1, 1.0f, 0, shielded) != 0) {
        station->shieldgreen = 1;
    }
    if (CollideGliderBullets(&station->appcentre, 11.0f, 0, 1.0f, 0, shielded) != 0) {
        station->shieldgreen = 0;
        if (shielded == 0) {
            AddScreenWumpa(station->pos.x, station->pos.y, station->pos.z, 1);
            station->hitpoints--;
            if (station->hitpoints > 0) {
                MyGameSfx(0xD5, &station->pos, 0x3FFF);
            }
        } else {
            MyGameSfx(0xD7, &station->pos, HITSHIELDVOLA);
            ClockOff();
        }
    }
    NuVecSub(&delta, &PlayerGlider.Position, &station->appcentre);
    unscaled = delta;
    delta.y /= SHIELDSCALEY;
    dist = NuVecMag(&delta);
    if (dist < 12.0f) {
        dot = DotProduct(&PlayerGlider.Velocity, &delta);
        if (dot < 0.0f) {
            mag = NuVecMag(&PlayerGlider.Velocity);
            speed = (mag < SPACEMINSPEEDIN) ? SPACEMINSPEEDIN : mag;
            NuVecScaleAccum(&PlayerGlider.Velocity, &delta, -dot / (dist * dist));
            NuVecNorm(&PlayerGlider.Velocity, &PlayerGlider.Velocity);
            NuVecScale(&PlayerGlider.Velocity, &PlayerGlider.Velocity, speed);
            NuVecScaleAccum(&PlayerGlider.Velocity, &delta, FIXVELBUMPAMOUNT / dist);
            PlayerGlider.FixVelTimer = FIXVELTIME;
            PlayerGlider.InTornado = 1;
            PlayerGlider.InTornadoScale = SPACETORNADOSCALE;
            if (Level == 0x1A) {
                MyGameSfx(0xD9, &PlayerGlider.Position, 0x7FFF);
            }
        }
        NuVecScale(&delta, &delta, 12.0f / dist);
        offset = delta;
        offset.y *= SHIELDSCALEY;
        NuVecAdd(&PlayerGlider.Position, &station->appcentre, &offset);
        angy = NuAtan2D(delta.x * -100.0f, delta.z * -100.0f);
        NuVecRotateY(&rotated, &delta, -angy);
        angx = NuAtan2D(rotated.y * 100.0f, rotated.z * -100.0f);
        NuMtxSetRotationX(&reflect, angx);
        NuMtxRotateY(&reflect, angy);
        NuMtxScale(&reflect, SetNuVecPntr(1.0f, SHIELDSCALEY, 1.0f));
        GliderBulletReflectMtx = &reflect;
        NuVecScale(&offset, &offset, 11.0f / 12.0f);
        NuVecAdd(&hitpos, &station->appcentre, &offset);
        AddGameDebris(0x1A, &hitpos);
        MyGameSfx(0xD7, 0, HITSHIELDVOL);
        station->shieldgreen = 0;
    }
    station->satelliteangle += 0.59999996f;
    if (station->hitpoints <= 0 || station->explode != 0) {
        station->active = 0;
        AddGameDebris(0x47, &station->pos);
        MyGameSfx(0xD8, &station->pos, SPACESTATIONEXPLODEVOL);
    }
    if (GliderBulletReflectMtx != 0) {
        mTEMP = *GliderBulletReflectMtx;
        NuMtxPreScale(&mTEMP, SetNuVecPntr(11.0f, 11.0f, 11.0f));
        NuMtxPreRotateX(&mTEMP, -0x4000);
        NuMtxPreRotateY(&mTEMP, rand());
        *(struct nuvec_s *)&mTEMP._30 = station->pos;
        station->shieldmtx = mTEMP;
        station->shielddraw = 2;
    }
}

void DrawVehicleTrail(void)
{
    if (Level == 0) {
        NuRndrTrail(trailpt, trail, 0x80);
    }
    DrawJeepTrails();
}

void DrawGliderDebugStuff(void)
{
    NuFntSet(app_fnt);
    NuFntSetPen(0x7F7F7F7F);
    NuFntScale(0x10, 0x10);
}

struct zoffa_s {
    struct MYDRAW draw;
    s32 active;                /* 0x0E0 */
    f32 respawntimer;          /* 0x0E4 */
    struct nuvec_s pos;        /* 0x0E8 */
    struct nuvec_s vel;        /* 0x0F4 */
    struct nuvec_s resolved;   /* 0x100 */
    f32 pitch;                 /* 0x10C */
    f32 roll;                  /* 0x110 */
    f32 destpitch;             /* 0x114 */
    f32 destroll;              /* 0x118 */
    f32 destyaw;               /* 0x11C */
    f32 yaw;                   /* 0x120 */
    s32 nofiresound;           /* 0x124 */
    f32 temp[6];               /* 0x128 */
    f32 visibletimer;          /* 0x140 */
    f32 newdirectiontimer;     /* 0x144 */
    f32 newaltitudetimer;      /* 0x148 */
    f32 newaltitudetarget;     /* 0x14C */
    s32 terminaldive;          /* 0x150 */
    s32 explode;               /* 0x154 */
    s32 hitpoints;             /* 0x158 */
    s32 fireframe;             /* 0x15C */
    s32 firenow;               /* 0x160 */
    f32 firetimer;             /* 0x164 */
    s32 seen;                  /* 0x168 */
    s32 smoketimer;            /* 0x16C */
    struct numtx_s locators[16];   /* 0x170 */
    s32 smokecounter;          /* 0x570 */
    f32 aggressiontimer;       /* 0x574 */
    s32 playercloseandvisible; /* 0x578 */
    f32 notseentimer;          /* 0x57C */
    f32 aggressionpoints;      /* 0x580 */
    s32 teleport;              /* 0x584 */
    f32 noteleporttimer;       /* 0x588 */
    f32 speed;                 /* 0x58C */
    s32 infront;               /* 0x590 */
    s32 facingtarget;          /* 0x594 */
    f32 dist;                  /* 0x598 */
    s32 fireside;              /* 0x59C */
    f32 notinfronttimer;       /* 0x5A0 */
    f32 lockedontimer;         /* 0x5A4 */
    f32 keepsamevelocitytimer; /* 0x5A8 */
    f32 firebursttimer;        /* 0x5AC */
}; /* 0x5B0 */

inline void DrawZoffa(struct zoffa_s *ufo)
{
    NuMtxSetRotationZ(&mTEMP, ufo->roll * 182.0444489f);
    NuMtxRotateX(&mTEMP, -ufo->pitch * 182.0444489f);
    NuMtxRotateY(&mTEMP, (ufo->yaw + 180.0f) * 182.0444489f);
    NuMtxTranslate(&mTEMP, &ufo->pos);
    ufo->seen = MyDrawModelNew(&ufo->draw, &mTEMP, ufo->locators);
}

extern struct zoffa_s EnemyZoffa[4];

struct zoffastart_s {
    f32 x, y, z;
    f32 angle;
};

static inline struct zoffa_s *FindFreeZoffa(void)
{
    s32 i;

    for (i = 0; i < 4; i++) {
        if (EnemyZoffa[i].active == 0) {
            memset(&EnemyZoffa[i], 0, sizeof(struct zoffa_s));
            return &EnemyZoffa[i];
        }
    }
    return 0;
}


struct zoffa_s *InitZoffa(struct zoffa_s *ufo, struct zoffastart_s *start)
{
    s32 index;
    s32 id;

    if (ufo == 0) {
        ufo = FindFreeZoffa();
    }
    if (ufo != 0) {
        index = ufo - EnemyZoffa;
        memset(ufo, 0, sizeof(struct zoffa_s));
        ufo->pos.x = start->x;
        ufo->pos.y = start->y;
        ufo->pos.z = start->z;
        ufo->yaw = start->angle;
        ufo->destyaw = start->angle;
        ufo->newaltitudetarget = -100.0f;
        ufo->respawntimer = 10.0f;
        ufo->active = 1;
        ufo->hitpoints = 2;
        if (Level == 0xD) {
            switch (index) {
            case 0: id = 0x5B; break;
            case 1: id = 0x5E; break;
            case 2: id = 0x5D; break;
            default: id = 0x5C; break;
            }
        } else if (Level == 0x12) {
            id = 0x5F;
        } else {
            switch (index) {
            case 0: id = 0x82; break;
            case 1: id = 0x83; break;
            case 2: id = 0x84; break;
            default: id = 0x85; break;
            }
        }
        if (MyInitModelNew(&ufo->draw, id, 0x1D, 0, 0, &ufo->pos) == 0) {
            return 0;
        }
        return ufo;
    }
    return 0;
}

extern s32 SHEIGHT;
extern char D_0061F420[]; /* "Action %d" */
extern char D_0061F430[]; /* "Temp %1.3f   %1.3f" */
extern void NuFntPrintEx(s32 x, s32 y, s32 flags, char *fmt, ...);

void DrawZoffaDebugStuff(void)
{
    struct zoffa_s *ufo = &EnemyZoffa[0];

    NuFntSet(app_fnt);
    NuFntSetPen(0x7F7F7F7F);
    NuFntScale(0x10, 0x10);
    NuFntPrintEx(0x80, SHEIGHT * 3 / 2, 0, D_0061F420, ufo->active);
    NuFntPrintEx(0x80, SHEIGHT * 2, 0, D_0061F430, ufo->temp[0], ufo->temp[1]);
    NuFntPrintEx(0x80, SHEIGHT * 5 / 2, 0, D_0061F430, ufo->temp[2], ufo->temp[3]);
    NuFntPrintEx(0x80, SHEIGHT * 3, 0, D_0061F430, ufo->temp[4], ufo->temp[5]);
}

void DrawZoffaUFOs(void)
{
    s32 i;

    for (i = 0; i < 4; i++) {
        if (EnemyZoffa[i].active != 0) {
            DrawZoffa(&EnemyZoffa[i]);
        }
    }
}

/* Whether `a` and `b` are within `radius`, with the y distance scaled. */
static inline s32 CheckSphereCollide(struct nuvec_s *a, struct nuvec_s *b, f32 radius, f32 yscale)
{
    struct nuvec_s rel;

    rel.x = a->x - b->x;
    rel.y = (a->y - b->y) * yscale;
    rel.z = a->z - b->z;
    return rel.x * rel.x + rel.y * rel.y + rel.z * rel.z <= radius * radius;
}

/* Whether `pos` is clear of the player's glider and every live UFO. */
s32 SafeFromCollisions(struct nuvec_s *pos)
{
    s32 i;

    if (CheckSphereCollide(pos, &PlayerGlider.Position, 3.0f, 1.0f)) {
        return 0;
    }
    for (i = 0; i < 4; i++) {
        if (EnemyZoffa[i].active != 0 && CheckSphereCollide(&EnemyZoffa[i].pos, pos, 3.0f, 1.0f)) {
            return 0;
        }
    }
    return 1;
}

extern struct zoffastart_s D_005A6C30[16];
#define ZoffaStartPoints D_005A6C30
extern void DrawCross(struct nuvec_s *pos, s32 colour, f32 size);

struct zoffastart_s *FindZoffaRestartPoint(void)
{
    struct nuvec_s rel;
    struct nuvec_s view;
    s32 i;
    s32 safe;

    i = (s32)(frand() * 16.0f);
    while (i > 16) {
        i -= 4;
    }
    while (i < 0) {
        i += 16;
    }
    NuVecSub(&rel, (struct nuvec_s *)&ZoffaStartPoints[i], (struct nuvec_s *)&GameCam[0].m._30);
    if (NuVecMag(&rel) < 10.0f) {
        safe = 0;
    } else {
        struct nuvec_s fwd = {0.0f, 0.0f, -1.0f};

        NuVecMtxRotate(&fwd, &fwd, &GameCam[0].m);
        if (NuVecDot(&rel, &view) < 0.0f) {
            safe = 0;
        } else {
            safe = SafeFromCollisions((struct nuvec_s *)&ZoffaStartPoints[i]);
        }
    }
    if (safe) {
        DrawCross((struct nuvec_s *)&ZoffaStartPoints[i], 0xFFFFFF, 3.0f);
    } else {
        DrawCross((struct nuvec_s *)&ZoffaStartPoints[i], 0xFF0000, 3.0f);
    }
    if (safe) {
        return &ZoffaStartPoints[i];
    }
    return 0;
}


void ZoffaSmoke(struct zoffa_s *ufo)
{
    struct nuvec_s pos;

    NuMtxSetRotationZ(&mTEMP, ufo->roll * 182.0444489f);
    NuMtxRotateX(&mTEMP, -ufo->pitch * 182.0444489f);
    NuMtxRotateY(&mTEMP, (ufo->yaw + 180.0f) * 182.0444489f);
    NuMtxTranslate(&mTEMP, &ufo->pos);
    if (ufo->terminaldive != 0) {
        if (ufo->smokecounter != 0) {
            NuVecMtxTransform(&pos, SetNuVecPntr(-1.0f, 0.2f, 1.0f), &mTEMP);
            AddGameDebrisRot(0x1D, &pos, 1, 0, 0);
            NuVecMtxTransform(&pos, SetNuVecPntr(0.0f, -0.8f, 1.0f), &mTEMP);
            AddGameDebrisRot(0x1D, &pos, 1, 0, 0);
            ufo->smokecounter = 0;
        } else {
            NuVecMtxTransform(&pos, SetNuVecPntr(1.0f, 0.2f, 1.0f), &mTEMP);
            AddGameDebrisRot(0x1D, &pos, 1, 0, 0);
            NuVecMtxTransform(&pos, SetNuVecPntr(0.0f, 1.2f, 1.0f), &mTEMP);
            AddGameDebrisRot(0x1D, &pos, 1, 0, 0);
            ufo->smokecounter = 1;
        }
        NuVecMtxTransform(&pos, SetNuVecPntr(0.0f, 0.2f, 1.0f), &mTEMP);
        AddGameDebrisRot(0x45, &pos, 1, 0, 0);
    } else if (ufo->hitpoints < 2) {
        ufo->smoketimer--;
        if (ufo->smoketimer < 1 && Level != 0x1A) {
            NuVecMtxTransform(&pos, SetNuVecPntr(0.0f, 0.2f, 1.0f), &mTEMP);
            DrawCross(&pos, 0xFFFFFF, 2.0f);
            AddGameDebrisRot(0x1D, &pos, 1, 0, 0);
            ufo->smoketimer = ufo->hitpoints;
        }
    }
}

inline struct hovablimp_s *FindFreeHova(void)
{
    s32 i;

    for (i = 0; i < 6; i++) {
        if (HovaBlimp[i].active == 0) {
            return &HovaBlimp[i];
        }
    }
    return 0;
}

void InitHova(struct nuvec_s *pos, f32 angle)
{
    struct hovablimp_s *hova;
    struct tornado_s *tornado;
    struct nuvec_s vel;
    s32 index;

    hova = FindFreeHova();
    if (hova == 0) {
        return;
    }
    hova->active = 1;
    hova->pos = *pos;
    hova->startpos = *pos;
    hova->pos.y += frand() - 0.5f;
    vel = SetNuVec(0.0f, (frand() - 0.5) * 5.0, 0.5f);
    index = hova - HovaBlimp;
    hova->vel = vel;
    hova->angy = angle;
    tornado = &TornadoList[index];
    hova->hitpoints = 10;
    hova->thrustoff = 0.1f;
    tornado->active = 1;
    tornado->pos = hova->pos;
    tornado->pos.y = 27.0f;
    tornado->startpos = tornado->pos;
    tornado->yang = frand() * 360.0f;
    tornado->yanginc = frand() * 40.0f + 160.0f;
    tornado->scale = 2.0f;
    MyInitModelNew(&hova->draw, 0x43, 0x46, 0, 0, &hova->pos);
}

extern f32 torndist;
extern s32 jonemit;
extern f32 jtemp1;
extern void AddDebrisEffect(s32 *key, s32 type, f32 x, f32 y, f32 z);

static inline void DrawHovaBlimp(struct hovablimp_s *hova)
{
    struct nuvec_s scale = {6.0f, 6.0f, 6.0f};

    NuMtxSetScale(&mTEMP, &scale);
    NuMtxRotateY(&mTEMP, (hova->angy + 180.0f) * 182.0444489f);
    NuMtxTranslate(&mTEMP, &hova->pos);
    MyDrawModelNew(&hova->draw, &mTEMP, 0);
}

void DrawAllHovaBlimps(void)
{
    struct nuvec_s pos;
    f32 dx;
    f32 dz;
    f32 dist;
    s32 i;

    torndist = 99999.0f;
    for (i = 0; i < 6; i++) {
        if (HovaBlimp[i].active != 0) {
            DrawHovaBlimp(&HovaBlimp[i]);
        }
    }
    jonemit += 60;
    if (jonemit >= 150) {
        jonemit = 0;
    }
    for (i = 0; i < 6; i++) {
        if (TornadoList[i].active != 0) {
            if (jonemit == 0 && Paused == 0) {
                pos = TornadoList[i].pos;
                pos.y -= 20.0f;
                AddVariableShotDebrisEffect(GDeb[0x2D].i, &pos, 1, 0, 0);
                pos.y -= 3.0f;
                AddVariableShotDebrisEffect(GDeb[0x2E].i, &pos, 1, 0, 0);
            }
            pos = TornadoList[i].pos;
            dx = player->obj.pos.x - pos.x;
            dz = player->obj.pos.z - pos.z;
            dist = dx * dx + dz * dz;
            if (dist < torndist) {
                torndist = dist;
            }
            if (jontorn[i * 2] == -1) {
                AddDebrisEffect(&jontorn[i * 2], GDeb[0x6A].i, pos.x, pos.y + jtemp1, pos.z);
            }
            if (jontorn[i * 2 + 1] == -1) {
                AddDebrisEffect(&jontorn[i * 2 + 1], GDeb[0x6B].i, pos.x, pos.y + jtemp1, pos.z);
            }
        } else {
            if (jontorn[i * 2] != -1) {
                DebFree(&jontorn[i * 2]);
                jontorn[i * 2] = -1;
            }
            if (jontorn[i * 2 + 1] != -1) {
                DebFree(&jontorn[i * 2 + 1]);
                jontorn[i * 2 + 1] = -1;
            }
        }
    }
}

extern f32 Level_GliderRadius;
extern f32 HovaRad;
extern f32 HovaY;
extern void AddDeb3(struct nuvec_s *pos, s32 effect, s32 n, struct nuangvec_s *rot);

void ProcessHovaBlimp(struct hovablimp_s *hova)
{
    struct nuvec_s newpos;
    struct nuvec_s rel;
    struct nuvec_s oldrel;
    f32 maxdist2;
    f32 dist;
    f32 olddist;
    s32 i;

    hova->draw.Anim.oldaction = hova->draw.Anim.action;
    MyAnimateModelNew(&hova->draw, 0.59999996f);
    SeekHalfLifeNUVEC(&hova->vel, &hova->targetvel, 1.0f, 0.02f);
    newpos = hova->pos;
    maxdist2 = (Level_GliderRadius - 15.0f) * (Level_GliderRadius - 15.0f);
    NuVecScaleAccum(&newpos, &hova->vel, 0.02f);
    dist = newpos.x * newpos.x + newpos.z * newpos.z;
    olddist = hova->pos.x * hova->pos.x + hova->pos.z * hova->pos.z;
    if (dist > maxdist2 && dist >= olddist) {
        hova->movetimer = 0.0f;
    } else {
        for (i = 0; i < 6; i++) {
            if (HovaBlimp[i].active != 0) {
                NuVecSub(&rel, &newpos, &HovaBlimp[i].pos);
                NuVecSub(&oldrel, &hova->pos, &HovaBlimp[i].pos);
                dist = rel.x * rel.x + rel.z * rel.z;
                olddist = oldrel.x * oldrel.x + oldrel.z * oldrel.z;
                if (dist < 20.0f && dist < olddist) {
                    hova->movetimer = 0.0f;
                    break;
                }
            }
        }
    }
    hova->pos = newpos;
    if (hova->thrust != 0) {
        if (hova->pos.y > hova->startpos.y + hova->thrustoff) {
            hova->thrust = 0;
        }
    } else if (hova->pos.y < hova->startpos.y) {
        hova->thrust = 1;
        hova->thrustoff = frand() * 0.5f;
    }
    if (hova->thrust != 0) {
        hova->vel.y += 0.06f;
    } else {
        hova->vel.y -= 0.12f;
    }
    if (hova->vel.y > 1.0f) {
        hova->vel.y = 1.0f;
    }
    if (hova->vel.y < -2.0f) {
        hova->vel.y = -2.0f;
    }
    if (hova->vel.y < 0.0f) {
        hova->angy += hova->vel.y * 5.0f;
    }
    hova->pos.y += hova->vel.y * 0.02f;
    CollideGliderBullets(&hova->pos, HovaRad, 1, 1.0f / HovaY, 1, 1);
    if (CollideGliderBullets(&hova->pos, HovaRad, 0, 1.0f / HovaY, 1, 0) != 0) {
        hova->hitpoints--;
        if (hova->hitpoints <= 0) {
            hova->explode = 1;
        }
        MyGameSfx(0x8C, &hova->pos, 0x3FFF);
        AddScreenWumpa(hova->pos.x, hova->pos.y, hova->pos.z, 1);
        MyGameSfx(0x8C, &hova->pos, 0x3FFF);
        ClockOff();
    }
    if (hova->explode != 0) {
        MyGameSfx(0x5F, &hova->pos, 0x3FFF);
        AddGameDebris(0x17, &hova->pos);
        AddDeb3(&hova->pos, 6, 4, 0);
        AddDeb3(&hova->pos, 7, 4, 0);
        AddDeb3(&hova->pos, 8, 4, 0);
        hova->active = 0;
    }
}

extern s32 SatelliteCharacterId;

inline struct satellite_s *InitSatellite(struct spacestation_s *station, f32 angle)
{
    struct satellite_s *sat = &SatelliteList[SatelliteCharacterId++];

    if (sat->active != 0) {
        return 0;
    }
    memset(sat, 0, sizeof(struct satellite_s));
    if (MyInitModelNew(&sat->draw, 0x8D, 0x1B, 0, 0, &sat->pos) == 0) {
        return 0;
    }
    sat->station = station;
    sat->pos = station->pos;
    sat->angle = angle;
    sat->tilt[0] = 0.0f;
    sat->tilt[1] = 0.0f;
    sat->tiltdest[0] = 0.0f;
    sat->tiltdest[1] = 0.0f;
    sat->active = 1;
    sat->hits = 16;
    sat->vel = v000;
    return sat;
}

inline void ProcessSatellites(void)
{
    s32 i;

    for (i = 0; i < 9; i++) {
        if (SatelliteList[i].active != 0) {
            ProcessSatellite(&SatelliteList[i]);
        }
    }
}

inline void ProcessSpaceStations(void)
{
    s32 i;

    PlayerGlider.InTornado = 0;
    for (i = 0; i < 3; i++) {
        if (SpaceStationList[i].active != 0) {
            ProcessSpaceStation(&SpaceStationList[i]);
        }
    }
}

inline void DrawSpaceStations(void)
{
    s32 i;

    for (i = 0; i < 3; i++) {
        if (SpaceStationList[i].active != 0) {
            DrawSpaceStation(&SpaceStationList[i]);
        }
    }
}

/* Glider bullet pools: free/used pointer stacks for player and enemy bullets. */
struct gliderbullet_s {
    struct numtx_s mtx;        /* 0x00 */
    struct nuvec_s vel;        /* 0x40 */
    s16 life;                  /* 0x4C */
    s16 lastlife;              /* 0x4E */
};
extern struct gliderbullet_s *FreeGliderBulletList[];
extern struct gliderbullet_s *UsedGliderBulletList[];
extern struct gliderbullet_s *FreeEnemyGliderBulletList[];
extern struct gliderbullet_s *UsedEnemyGliderBulletList[];
extern s32 FreeGliderBulletNum;
extern s32 UsedGliderBulletNum;
extern s32 FreeEnemyGliderBulletNum;
extern s32 UsedEnemyGliderBulletNum;
extern s32 BulletNumFrees;
extern s32 BulletEnemyNumFrees;

inline struct gliderbullet_s *GrabGliderBullet(s32 enemy)
{
    struct gliderbullet_s *b;

    if (enemy) {
        if (FreeEnemyGliderBulletNum != 0) {
            FreeEnemyGliderBulletNum--;
            b = FreeEnemyGliderBulletList[FreeEnemyGliderBulletNum];
            UsedEnemyGliderBulletList[UsedEnemyGliderBulletNum++] = b;
            return b;
        }
        return 0;
    }
    if (FreeGliderBulletNum != 0) {
        FreeGliderBulletNum--;
        b = FreeGliderBulletList[FreeGliderBulletNum];
        UsedGliderBulletList[UsedGliderBulletNum++] = b;
        return b;
    }
    return 0;
}

extern struct nuvec_s SPACETRAN;

void InitSpaceStation(struct nuvec_s *pos, f32 angle, f32 tiltx, f32 tiltz, s32 index)
{
    struct spacestation_s *station;
    s32 i;

    station = &SpaceStationList[index];
    if (index > 2 || station->active != 0) {
        return;
    }
    memset(station, 0, sizeof(struct spacestation_s));
    station->pos = *pos;
    station->active = 1;
    station->angle = angle;
    station->hitpoints = 40;
    station->vel = v000;
    station->satelliteangle = (f32)index * 40.0f;
    NuMtxSetScale(&mTEMP, SetNuVecPntr(1.0f, 1.0f, 1.0f));
    NuMtxRotateX(&mTEMP, tiltx * 182.0444489f);
    NuMtxRotateZ(&mTEMP, tiltz * 182.0444489f);
    NuMtxRotateY(&mTEMP, station->angle * 182.0444489f);
    NuMtxTranslate(&mTEMP, &station->pos);
    station->mtx = mTEMP;
    NuVecMtxTransform(&station->appcentre, &SPACETRAN, &station->mtx);
    for (i = 0; i < 3; i++) {
        station->satellites[i] = InitSatellite(station, (f32)i * 120.0f);
    }
}

void InitSpaceStations(void)
{
    memset(SpaceStationList, 0, sizeof(SpaceStationList));
    memset(SatelliteList, 0, sizeof(SatelliteList));
    SatelliteCharacterId = 0;
    /* -0.0f is retail's literal (a negated zero), not a typo for 0.0f. */
    InitSpaceStation(SetNuVecPntr(0.0f, 0.0f, 80.0f), -0.0f, 15.0f, 20.0f, 0);
    InitSpaceStation(SetNuVecPntr(69.0f, 0.0f, -40.0f), -13.0f, -20.0f, 7.0f, 1);
    InitSpaceStation(SetNuVecPntr(-69.0f, 0.0f, -40.0f), -26.0f, 15.0f, -20.0f, 2);
}

s32 AddGliderBullet(struct numtx_s *m, struct nuvec_s *pos, struct nuvec_s *vel,
                    s32 enemy)
{
    struct gliderbullet_s *b;

    b = GrabGliderBullet(enemy);
    if (b != 0) {
        if (enemy) {
            b->life = 100;
        } else if (Level == 0xD) {
            b->life = 75;
        } else if (Level == 0x1A) {
            b->life = 37;
        } else {
            b->life = 150;
        }
        if (enemy) {
            b->lastlife = b->life - 1;
        } else {
            b->lastlife = b->life + 1;
        }
        b->mtx = *m;
        b->mtx._30 = pos->x;
        b->mtx._31 = pos->y;
        b->mtx._32 = pos->z;
        b->vel.x = vel->x * 0.02f;
        b->vel.y = vel->y * 0.02f;
        b->vel.z = vel->z * 0.02f;
        return 1;
    }
    return 0;
}

inline void FreeGliderBullet(s32 index, s32 enemy)
{
    if (enemy) {
        BulletEnemyNumFrees++;
        FreeEnemyGliderBulletList[FreeEnemyGliderBulletNum++] =
            UsedEnemyGliderBulletList[index];
        UsedEnemyGliderBulletList[index] =
            UsedEnemyGliderBulletList[--UsedEnemyGliderBulletNum];
    } else {
        BulletNumFrees++;
        FreeGliderBulletList[FreeGliderBulletNum++] = UsedGliderBulletList[index];
        UsedGliderBulletList[index] = UsedGliderBulletList[--UsedGliderBulletNum];
    }
}

extern struct nuvec_s GliderBulletReflectPos;
extern s32 qrand(void);
extern f32 NuFsqrt(f32 x);

s32 CollideGliderBullets(struct nuvec_s *pos, f32 radius, s32 enemy, f32 yscale, s32 debris,
                         s32 reflect)
{
    s32 n;
    struct gliderbullet_s **list;
    s32 effect;
    s32 hit;
    struct nuvec_s delta;
    struct nuvec_s rel;
    struct nuvec_s scale;
    struct gliderbullet_s *bullet;
    struct nuvec_s *bulletpos;
    f32 radsq;
    f32 yradius;
    f32 vv;
    f32 b;
    f32 root;
    f32 dot;
    s32 count;
    s32 angy;
    s32 angx;
    s32 i;

    radsq = radius * radius;
    yradius = radius / yscale;
    hit = 0;
    if (enemy) {
        list = UsedEnemyGliderBulletList;
        count = UsedEnemyGliderBulletNum;
        if (Level == 0x1A || Level == 0x12) {
            effect = 0x1A;
        } else if (Level == 0x12) {
            effect = 0x1B;
        } else {
            effect = 0x19;
        }
    } else {
        list = UsedGliderBulletList;
        count = UsedGliderBulletNum;
        effect = 0x1A;
    }
    i = 0;
    for (n = 0; n < count; n++) {
        bullet = list[i++];
        if (bullet->life >= bullet->lastlife) {
            continue;
        }
        delta.x = bullet->mtx._30 - pos->x;
        if (delta.x > radius || delta.x < -radius) {
            continue;
        }
        delta.y = bullet->mtx._31 - pos->y;
        if (delta.y > yradius || delta.y < -yradius) {
            continue;
        }
        delta.z = bullet->mtx._32 - pos->z;
        if (delta.z > radius || delta.z < -radius) {
            continue;
        }
        delta.y *= yscale;
        if (!(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z <= radsq)) {
            continue;
        }
        bulletpos = (struct nuvec_s *)&bullet->mtx._30;
        hit = 1;
        if (reflect) {
            vv = DotProduct(&bullet->vel, &bullet->vel);
            b = DotProduct(&bullet->vel, &delta);
            b = b + b;
            root = NuFsqrt(b * b - vv * 4.0f * (DotProduct(&delta, &delta) - radsq));
            NuVecScaleAccum(&delta, &bullet->vel, -(b + fabs(root)) / (vv + vv));
            NuVecAdd(bulletpos, &delta, pos);
            dot = DotProduct(&bullet->vel, &delta);
            if (dot < 0.0f) {
                NuVecScaleAccum(&bullet->vel, &delta, dot * -2.0f / NuVecMagSqr(&delta));
                GliderBulletReflectPos = *bulletpos;
                angy = (s16)NuAtan2D(delta.x * -100.0f, delta.z * -100.0f);
                NuVecRotateY(&rel, &delta, -angy);
                angx = (s16)NuAtan2D(rel.y * 100.0f, rel.z * -100.0f);
                rel = *bulletpos;
                NuMtxSetRotationX(&bullet->mtx, angx);
                NuMtxRotateY(&bullet->mtx, angy);
                scale.x = 1.0f;
                scale.y = 1.0f / yscale;
                scale.z = 1.0f;
                NuMtxScale(&bullet->mtx, &scale);
                NuMtxTranslate(&bullet->mtx, &rel);
                GliderBulletReflectMtx = &bullet->mtx;
                if (Level == 0x1A) {
                    if (enemy) {
                        AddGameDebris(0x48, bulletpos);
                    } else {
                        AddGameDebris(0x49, bulletpos);
                    }
                } else {
                    AddGameDebris(effect, bulletpos);
                }
            }
        } else {
            AddGameDebris(effect, bulletpos);
            i--;
            FreeGliderBullet(i, enemy);
            if (debris) {
                AddDeb3(pos, qrand() % 3 + 3, 3, 0);
            }
        }
    }
    return hit;
}

extern struct cratecube_s *temp_pCrate;
extern s32 temp_crate_type;
extern f32 CRATEBALLOONOFFSET;
extern f32 CRATEBALLOONRADIUS;
extern s32 HitCrates(struct obj_s *obj, s32 destroy);
extern struct cratecube_s *HitCrateBalloons(struct nuvec_s *pos, f32 radius);
extern s32 HitItems(struct obj_s *obj);
extern struct creature_s *FindClock(void);
extern void StartTimeTrial(struct nuvec_s *pos, s32 clock);

void GliderBulletsHitThings(struct gliderflight_s *glider)
{
    struct obj_s obj;
    struct nuvec_s clockpos;
    struct gliderbullet_s **used;
    struct gliderbullet_s *bullet;
    struct creature_s *clock;
    f32 radius;
    s32 count;
    s32 n;
    s32 i;

    obj.bot = -0.3f;
    obj.SCALE = 1.0f;
    obj.top = 0.3f;
    obj.RADIUS = 0.3f;
    obj.attack = 0;
    obj.pLOCATOR = 0;
    used = UsedGliderBulletList;
    i = 0;
    count = UsedGliderBulletNum;
    for (n = 0; n < count; n++) {
        bullet = used[i++];
        obj.pos.x = bullet->mtx._30;
        obj.pos.y = bullet->mtx._31;
        obj.pos.z = bullet->mtx._32;
        if (HitCrates(&obj, 1)) {
            AddGameDebris(0x1A, &obj.pos);
            i--;
            AddScreenWumpa(temp_pCrate->pos.x, temp_pCrate->pos.y + 0.25f, temp_pCrate->pos.z, 1);
            FreeGliderBullet(i, 0);
            if (temp_crate_type == 5) {
                glider->HitPoints += 50;
                if (glider->HitPoints > 100) {
                    glider->HitPoints = 100;
                }
            }
        } else if (HitCrateBalloons(&obj.pos, obj.RADIUS)) {
            AddGameDebris(0x1A, &obj.pos);
            i--;
            AddScreenWumpa(temp_pCrate->pos.x, temp_pCrate->pos.y + CRATEBALLOONOFFSET, temp_pCrate->pos.z, 1);
            FreeGliderBullet(i, 0);
            temp_pCrate->flags = (temp_pCrate->flags | 0x400) ^ 0x400;
        } else if (HitItems(&obj)) {
            AddGameDebris(0x1A, &obj.pos);
            i--;
            FreeGliderBullet(i, 0);
        } else {
            clock = FindClock();
            if (clock != 0) {
                clockpos.x = clock->obj.pos.x;
                clockpos.y = clock->obj.pos.y + CRATEBALLOONOFFSET;
                clockpos.z = clock->obj.pos.z;
                radius = obj.RADIUS + CRATEBALLOONRADIUS;
                if (NuVecDistSqr(&obj.pos, &clockpos, 0) < radius * radius) {
                    AddGameDebris(0x1A, &obj.pos);
                    i--;
                    FreeGliderBullet(i, 0);
                    StartTimeTrial(&clock->obj.pos, 1);
                }
            }
        }
    }
}

void GliderBombsHitThings(struct gliderflight_s *glider)
{
    struct obj_s obj;
    struct nuvec_s clockpos;
    struct gliderbomb_s *bombs;
    struct gliderbomb_s *bomb;
    struct creature_s *clock;
    f32 radius;
    s32 i;

    obj.bot = -0.3f;
    obj.SCALE = 1.0f;
    obj.top = 0.3f;
    obj.RADIUS = 0.3f;
    obj.attack = 0;
    obj.pLOCATOR = 0;
    bombs = GliderBombs;
    for (i = 0; i < 10; i++) {
        bomb = &bombs[i];
        if (bomb->life == 0) {
            continue;
        }
        obj.pos = bomb->pos;
        if (HitCrates(&obj, 1)) {
            AddGameDebris(0x1A, &obj.pos);
            AddScreenWumpa(temp_pCrate->pos.x, temp_pCrate->pos.y + 0.25f, temp_pCrate->pos.z, 1);
            bomb->life = 0;
            if (temp_crate_type == 5) {
                glider->HitPoints += 50;
                if (glider->HitPoints > 100) {
                    glider->HitPoints = 100;
                }
            }
        } else if (HitCrateBalloons(&obj.pos, obj.RADIUS)) {
            AddGameDebris(0x1A, &obj.pos);
            AddScreenWumpa(temp_pCrate->pos.x, temp_pCrate->pos.y + CRATEBALLOONOFFSET, temp_pCrate->pos.z, 1);
            bomb->life = 0;
            temp_pCrate->flags = (temp_pCrate->flags | 0x400) ^ 0x400;
        } else if (HitItems(&obj)) {
            AddGameDebris(0x1A, &obj.pos);
            bomb->life = 0;
        } else {
            clock = FindClock();
            if (clock != 0) {
                clockpos.x = clock->obj.pos.x;
                clockpos.y = clock->obj.pos.y + CRATEBALLOONOFFSET;
                clockpos.z = clock->obj.pos.z;
                radius = obj.RADIUS + CRATEBALLOONRADIUS;
                if (NuVecDistSqr(&obj.pos, &clockpos, 0) < radius * radius) {
                    AddGameDebris(0x1A, &obj.pos);
                    bomb->life = 0;
                    StartTimeTrial(&clock->obj.pos, 1);
                }
            }
        }
    }
}

void DrawGliderBullets(void)
{
    s32 i;

    if (Level == 0x1A) {
        for (i = 0; i < UsedGliderBulletNum;) {
            NuRndrGScnObj(ObjTab[0x6F].obj.scene->gobjs[ObjTab[0x6F].obj.special->instance->objid],
                          &UsedGliderBulletList[i++]->mtx);
        }
        for (i = 0; i < UsedEnemyGliderBulletNum;) {
            NuRndrGScnObj(ObjTab[0x6E].obj.scene->gobjs[ObjTab[0x6E].obj.special->instance->objid],
                          &UsedEnemyGliderBulletList[i++]->mtx);
        }
    } else {
        for (i = 0; i < UsedGliderBulletNum;) {
            NuRndrGScnObj(ObjTab[0].obj.scene->gobjs[ObjTab[0].obj.special->instance->objid],
                          &UsedGliderBulletList[i++]->mtx);
        }
        if (Level == 0x24) {
            for (i = 0; i < UsedEnemyGliderBulletNum;) {
                NuRndrGScnObj(ObjTab[0xE].obj.scene->gobjs[ObjTab[0xE].obj.special->instance->objid],
                              &UsedEnemyGliderBulletList[i++]->mtx);
            }
        } else {
            for (i = 0; i < UsedEnemyGliderBulletNum;) {
                NuRndrGScnObj(ObjTab[0xE].obj.scene->gobjs[ObjTab[0xE].obj.special->instance->objid],
                              &UsedEnemyGliderBulletList[i++]->mtx);
            }
        }
    }
}


void ProcessGliderBullets(void)
{
    struct nuvec_s pos;
    struct gliderbullet_s **list;
    struct gliderbullet_s *bullet;
    s32 count;
    s32 effect;
    s32 enemy;
    s32 n;
    s32 i;
    s32 j;

    list = UsedGliderBulletList;
    count = UsedGliderBulletNum;
    if (Level == 0x1A) {
        effect = -1;
    } else {
        effect = GDeb[0x2C].i;
    }
    for (enemy = 0; enemy < 2; enemy++) {
        i = 0;
        for (n = 0; n < count; n++) {
            bullet = list[i];
            bullet->mtx._30 += bullet->vel.x;
            bullet->mtx._31 += bullet->vel.y;
            bullet->mtx._32 += bullet->vel.z;
            pos.x = bullet->mtx._30;
            pos.y = bullet->mtx._31;
            pos.z = bullet->mtx._32;
            if (effect != -1 && Paused == 0) {
                for (j = 0; j < 4; j++) {
                    AddVariableShotDebrisEffect(effect, &pos, 1, 0, 0);
                    pos.x -= bullet->vel.x * 0.25f;
                    pos.y -= bullet->vel.y * 0.25f;
                    pos.z -= bullet->vel.z * 0.25f;
                }
            }
            if (--bullet->life == 0) {
                FreeGliderBullet(i, enemy);
            } else {
                i++;
            }
        }
        list = UsedEnemyGliderBulletList;
        effect = -1;
        count = UsedEnemyGliderBulletNum;
    }
}


struct asteroid_s {
    s32 active;                /* 0x00 */
    struct nuvec_s pos;        /* 0x04 */
    struct nuvec_s vel;        /* 0x10 */
    f32 anglex;                /* 0x1C degrees */
    f32 angley;                /* 0x20 degrees */
    f32 spinx;                 /* 0x24 */
    f32 spiny;                 /* 0x28 */
    struct nuvec_s scale;      /* 0x2C */
    s32 inview;                /* 0x38 */
    s32 hitpoints;             /* 0x3C */
    char pad_40[0x44 - 0x40];
    s32 stealth;               /* 0x44 drawn shrunk to nothing */
    s32 explode;               /* 0x48 */
};
extern struct asteroid_s AsteroidList[100];
extern void InitAsteroid(s32 stealth);

void DrawAsteroid(struct asteroid_s *asteroid)
{
    if (asteroid->stealth != 0) {
        NuMtxSetScale(&mTEMP, SetNuVecPntr(0.0f, 0.0f, 0.0f));
        NuMtxTranslate(&mTEMP, &asteroid->pos);
    } else {
        NuMtxSetScale(&mTEMP, &asteroid->scale);
        NuMtxRotateY(&mTEMP, asteroid->angley * 182.04445f);
        NuMtxRotateX(&mTEMP, asteroid->anglex * 182.04445f);
        NuMtxTranslate(&mTEMP, &asteroid->pos);
    }
    asteroid->inview = 0;
    if (ObjTab[0x50].obj.special != 0) {
        asteroid->inview = NuRndrGScnObj(
            ObjTab[0x50].obj.scene->gobjs[ObjTab[0x50].obj.special->instance->objid], &mTEMP);
    }
}

inline struct asteroid_s *FindFreeAsteroid(s32 passes)
{
    s32 i;
    s32 pass;

    for (i = 0; i < 100; i++) {
        if (AsteroidList[i].active == 0) {
            return &AsteroidList[i];
        }
    }
    for (pass = 0; pass < passes; pass++) {
        for (i = 0; i < 100; i++) {
            if (AsteroidList[i].hitpoints <= pass && AsteroidList[i].inview == 0) {
                return &AsteroidList[i];
            }
        }
    }
    return 0;
}

extern f32 Level_GliderFloor;
extern f32 Level_GliderCeiling;

inline struct nuvec_s GetRandomGliderLevelEdgePoint(void)
{
    struct nuvec_s point = {0.0f, 0.0f, Level_GliderRadius};

    NuVecRotateY(&point, &point, frand() * 360.0f * 182.0444489f);
    point.y = Level_GliderFloor + frand() * (Level_GliderCeiling - Level_GliderFloor);
    return point;
}

void InitAsteroid(s32 stealth)
{
    struct asteroid_s *asteroid;

    asteroid = FindFreeAsteroid(3);
    if (asteroid == 0 || asteroid->active != 0) {
        return;
    }
    memset(asteroid, 0, sizeof(struct asteroid_s));
    asteroid->pos = GetRandomGliderLevelEdgePoint();
    {
        struct nuvec_s target = GetRandomGliderLevelEdgePoint();

        NuVecSub(&target, &target, &asteroid->pos);
        NuVecNorm(&target, &target);
        NuVecScale(&asteroid->vel, &target, 10.0f);
    }
    asteroid->anglex = frand() * 360.0f;
    asteroid->angley = frand() * 360.0f;
    asteroid->spinx = (frand() - 0.5f) * 72.0f * 0.02f;
    asteroid->spiny = (frand() - 0.5f) * 72.0f * 0.02f;
    asteroid->stealth = stealth;
    asteroid->active = 1;
    asteroid->hitpoints = 3;
    asteroid->scale = SetNuVec(2.0f, 2.0f, 2.0f);
    NuVecScale(&asteroid->vel, &asteroid->vel, 0.02f);
}


extern struct asteroid_s *ActiveAsteroidList[100];
extern s32 ActiveAsteroidListNum;

void ProcessAsteroid(struct asteroid_s *asteroid)
{
    struct nuvec_s pos;
    struct asteroid_s *piece;
    s32 outside;
    s32 i;
    f32 dot;
    f32 speed;

    ActiveAsteroidList[ActiveAsteroidListNum++] = asteroid;
    if (asteroid->inview == 0) {
        NuVecMagSqr(&asteroid->pos);
        dot = DotProduct(&asteroid->pos, &asteroid->vel);
        outside = 0;
        asteroid->stealth = 0;
        if (asteroid->pos.x * asteroid->pos.x + asteroid->pos.z * asteroid->pos.z
                > Level_GliderRadius * Level_GliderRadius
            || asteroid->pos.y < Level_GliderFloor || asteroid->pos.y > Level_GliderCeiling) {
            outside = 1;
        }
        if (outside && dot >= 0.0f) {
            asteroid->stealth = 1;
            NuVecSub(&asteroid->pos, &v000, &asteroid->pos);
            pos = GetRandomGliderLevelEdgePoint();
            speed = NuVecMag(&asteroid->vel);
            NuVecSub(&pos, &pos, &asteroid->pos);
            NuVecNorm(&pos, &pos);
            NuVecScale(&asteroid->vel, &pos, speed);
        }
    }
    NuVecAdd(&asteroid->pos, &asteroid->pos, &asteroid->vel);
    asteroid->anglex = Rationalise360f(asteroid->anglex + asteroid->spinx);
    asteroid->angley = Rationalise360f(asteroid->angley + asteroid->spiny);
    pos = asteroid->pos;
    if (CollideGliderBullets(&asteroid->pos, asteroid->scale.x + asteroid->scale.x, 1, 1.0f, 0, 0)) {
        asteroid->explode = 1;
    }
    if (CollideGliderBullets(&asteroid->pos, asteroid->scale.x + asteroid->scale.x, 0, 1.0f, 0, 0)) {
        asteroid->explode = 1;
    }
    if (asteroid->explode == 0) {
        return;
    }
    if (asteroid->inview != 0) {
        AddGameDebris(0x18, &asteroid->pos);
        MyGameSfx(0xCD, &pos, 0x7FFF);
    }
    asteroid->explode = 0;
    if (asteroid->hitpoints != 0) {
        struct nuvec_s scale;
        struct nuvec_s dirs[3];

        NuVecScale(&asteroid->scale, &asteroid->scale, 0.5f);
        NuVecSub(&scale, SetNuVecPntr(2.0f, 2.0f, 2.0f), &asteroid->scale);
        NuMtxSetScale(&mTEMP, &scale);
        NuMtxRotateY(&mTEMP, frand() * 360.0f * 182.0444489f);
        NuMtxRotateX(&mTEMP, frand() * 360.0f * 182.0444489f);
        dirs[0] = SetNuVec(0.0f, 0.0f, 0.099999994f);
        dirs[1] = SetNuVec(0.0f, 0.099999994f, 0.0f);
        dirs[2] = SetNuVec(0.099999994f, 0.0f, 0.0f);
        for (i = 0; i < 3; i++) {
            if (i < 2) {
                piece = FindFreeAsteroid(asteroid->hitpoints - 1);
                if (piece == 0) {
                    continue;
                }
                *piece = *asteroid;
            } else {
                piece = asteroid;
            }
            piece->hitpoints--;
            NuVecMtxRotate(&piece->vel, &dirs[i], &mTEMP);
            NuVecScaleAccum(&piece->pos, &piece->vel, 25.0f);
        }
    }
    asteroid->active = 0;
}


void CheckForPotentialMidAirCollisions(void)
{
    struct asteroid_s *asteroid;
    s32 hit;
    s32 i;
    s32 j;

    if (Level == 0x1A) {
        for (i = 0; i < 4; i++) {
            if (EnemyZoffa[i].active == 0 || EnemyZoffa[i].active == 3) {
                continue;
            }
            for (j = 0; j < ActiveAsteroidListNum; j++) {
                asteroid = ActiveAsteroidList[j];
                hit = PossIntersect(&EnemyZoffa[i].pos, &EnemyZoffa[i].vel, &asteroid->pos,
                                    &asteroid->vel, 2.0f, 1.0f);
                if (hit) {
                    EnemyZoffa[i].active = 3;
                    if (i & 1) {
                        EnemyZoffa[i].destyaw = EnemyZoffa[i].yaw + 90.0f;
                    } else {
                        EnemyZoffa[i].destyaw = EnemyZoffa[i].yaw - 90.0f;
                    }
                }
            }
        }
    }
    for (i = 0; i < 4; i++) {
        if (EnemyZoffa[i].active == 0 || EnemyZoffa[i].active == 3) {
            continue;
        }
        for (j = 0; j < 4; j++) {
            if (i == j || EnemyZoffa[j].active == 0) {
                continue;
            }
            hit = PossIntersect(&EnemyZoffa[i].pos, &EnemyZoffa[i].vel, &EnemyZoffa[j].pos,
                                &EnemyZoffa[j].vel, 2.0f, 2.0f);
            if (hit) {
                EnemyZoffa[i].active = 3;
                if (hit == 1 || i < j) {
                    EnemyZoffa[i].destyaw = EnemyZoffa[i].yaw + 90.0f;
                } else {
                    EnemyZoffa[i].destyaw = EnemyZoffa[i].yaw - 90.0f;
                }
            }
        }
    }
    for (i = 0; i < 4; i++) {
        if (EnemyZoffa[i].active == 0 || EnemyZoffa[i].active == 3) {
            continue;
        }
        if (DotProduct(&EnemyZoffa[i].vel, &PlayerGlider.Velocity) >= 0.0f) {
            hit = PossIntersect(&EnemyZoffa[i].pos, &EnemyZoffa[i].vel, &PlayerGlider.Position,
                                &PlayerGlider.Velocity, 3.0f, 2.0f);
            if (hit) {
                EnemyZoffa[i].active = 3;
                if (hit == 1 || i < 2) {
                    EnemyZoffa[i].destyaw = EnemyZoffa[i].yaw + 90.0f;
                } else {
                    EnemyZoffa[i].destyaw = EnemyZoffa[i].yaw - 90.0f;
                }
            }
        }
    }
    if (Level == 0xD) {
        for (i = 0; i < 4; i++) {
            if (EnemyZoffa[i].active == 0 || EnemyZoffa[i].active == 3) {
                continue;
            }
            for (j = 0; j < 6; j++) {
                if (HovaBlimp[j].active == 0) {
                    continue;
                }
                hit = PossIntersect(&EnemyZoffa[i].pos, &EnemyZoffa[i].vel, &HovaBlimp[j].pos,
                                    &HovaBlimp[j].vel, 2.0f, 2.0f);
                if (hit) {
                    EnemyZoffa[i].active = 3;
                    if (hit == 1 || i < j) {
                        EnemyZoffa[i].destyaw = EnemyZoffa[i].yaw + 90.0f;
                    } else {
                        EnemyZoffa[i].destyaw = EnemyZoffa[i].yaw - 90.0f;
                    }
                }
            }
        }
    }
    if (Level == 0x1A) {
        for (i = 0; i < 4; i++) {
            if (EnemyZoffa[i].active == 0 || EnemyZoffa[i].active == 3) {
                continue;
            }
            for (j = 0; j < 3; j++) {
                if (SpaceStationList[j].active == 0) {
                    continue;
                }
                hit = PossIntersect(&EnemyZoffa[i].pos, &EnemyZoffa[i].vel, &SpaceStationList[j].pos,
                                    &SpaceStationList[j].vel, 12.0f, 2.0f);
                if (hit) {
                    EnemyZoffa[i].active = 3;
                    if (hit == 1 || i < j) {
                        EnemyZoffa[i].destyaw = EnemyZoffa[i].yaw + 90.0f;
                    } else {
                        EnemyZoffa[i].destyaw = EnemyZoffa[i].yaw - 90.0f;
                    }
                }
            }
        }
    }
}


extern s32 D_006332A0;
extern s32 D_006332A4;
extern s32 D_006332A8;
#define ZoffaCollisionCounter D_006332A0
#define ZoffaTeleportIndx D_006332A4
#define CurrentAggressor D_006332A8
extern s32 InvincibilityCHEAT;
extern s32 VehicleLevelImmune;
extern s32 BEENHITBIGBUZZTIME;
extern s32 BEENHITBIGRUMTIME;
extern f32 Level_DeadTime;
extern struct nuvec_s GliderCollPoints[5];
extern f32 BigRad;
extern f32 PropRad;
extern f32 PropY;
extern f32 PropYOff;
extern struct nuvec_s D_005A6E70;
#define ZoffaRayDir D_005A6E70

static inline s32 SphereCollideDiff(struct nuvec_s *a, struct nuvec_s *b, f32 radius, f32 yscale,
                                    struct nuvec_s *diff)
{
    diff->x = a->x - b->x;
    diff->y = (a->y - b->y) * yscale;
    diff->z = a->z - b->z;
    return diff->x * diff->x + diff->y * diff->y + diff->z * diff->z <= radius * radius;
}

static inline void GetGliderCollPoints(struct nuvec_s *points)
{
    struct numtx_s m;
    s32 i;

    NuMtxSetRotationZ(&m, PlayerGlider.TiltZ * 182.0444489f);
    NuMtxRotateX(&m, (PlayerGlider.TiltX + 15.0f) * 182.0444489f);
    NuMtxRotateY(&m, PlayerGlider.AngleY * 182.0444489f);
    NuMtxTranslate(&m, &PlayerGlider.Position);
    for (i = 0; i < 5; i++) {
        NuVecMtxTransform(&points[i], &GliderCollPoints[i], &m);
    }
}

void CheckGliderCollisions(void)
{
    struct nuvec_s diff;
    struct nuvec_s diff2;
    struct nuvec_s diff3;
    struct nuvec_s diff4;
    struct nuvec_s diff5;
    struct asteroid_s *asteroid;
    struct zoffa_s *ufo;
    struct satellite_s *sat;
    struct spacestation_s *station;
    s32 damage;
    s32 hits;
    s32 hit;
    s32 i;
    s32 j;
    f32 dot;
    f32 speed;

    if (Level == 0x1A) {
        for (i = 1; i < ActiveAsteroidListNum; i++) {
            for (j = 0; j < i; j++) {
                struct asteroid_s *rock = ActiveAsteroidList[i];
                struct asteroid_s *other = ActiveAsteroidList[j];
                f32 reach;
                f32 range;

                reach = (rock->scale.x + other->scale.x) * 2.0f;
                range = reach * reach;
                NuVecSub(&diff, &rock->pos, &other->pos);
                if (NuVecMagSqr(&diff) < range) {
                    if (rock->scale.x >= other->scale.x) {
                        other->explode = 1;
                    }
                    if (rock->scale.x <= other->scale.x) {
                        rock->explode = 1;
                    }
                }
            }
        }
        for (i = 0; i < 4; i++) {
            ufo = &EnemyZoffa[i];
            if (ufo->active == 0) {
                continue;
            }
            for (j = 0; j < ActiveAsteroidListNum; j++) {
                f32 reach;
                f32 range;

                asteroid = ActiveAsteroidList[j];
                reach = asteroid->scale.x * 2.0f + 1.0f;
                range = reach * reach;
                NuVecSub(&diff2, &asteroid->pos, &ufo->pos);
                if (NuVecMagSqr(&diff2) < range) {
                    asteroid->explode = 1;
                    ufo->explode = 1;
                }
            }
        }
        for (i = 0; i < 9; i++) {
            sat = &SatelliteList[i];
            if (sat->active == 0) {
                continue;
            }
            for (j = 0; j < ActiveAsteroidListNum; j++) {
                f32 reach;
                f32 range;

                asteroid = ActiveAsteroidList[j];
                reach = asteroid->scale.x * 2.0f + 3.0f;
                range = reach * reach;
                NuVecSub(&diff3, &asteroid->pos, &sat->pos);
                if (NuVecMagSqr(&diff3) < range) {
                    asteroid->explode = 1;
                }
            }
        }
        for (i = 0; i < 3; i++) {
            station = &SpaceStationList[i];
            if (station->active == 0) {
                continue;
            }
            for (j = 0; j < ActiveAsteroidListNum; j++) {
                f32 reach;
                f32 range;

                asteroid = ActiveAsteroidList[j];
                reach = asteroid->scale.x * 2.0f + 8.0f;
                range = reach * reach;
                NuVecSub(&diff4, &asteroid->pos, &station->pos);
                if (NuVecMagSqr(&diff4) < range) {
                    asteroid->explode = 1;
                }
            }
        }
        if (PlayerGlider.ImmuneAsteroidsTimer == 0.0f) {
            for (j = 0; j < ActiveAsteroidListNum; j++) {
                struct asteroid_s *rock = ActiveAsteroidList[j];

                if (!SphereCollideDiff(&rock->pos, &PlayerGlider.Position,
                                       rock->scale.x * 2.0f + 0.5f, 1.0f, &diff5)) {
                    continue;
                }
                rock->explode = 1;
                hits = rock->hitpoints;
                damage = (hits * hits + hits) * (hits + 2) / 2;
                if (damage != 0) {
                    PlayerGlider.ImmuneAsteroidsTimer = 1.0f;
                }
                if (InvincibilityCHEAT == 0 && VehicleLevelImmune == 0) {
                    PlayerGlider.HitPoints -= damage;
                }
                if (Level == 0x1A) {
                    MyGameSfx(0xD9, &PlayerGlider.Position, 0x7FFF);
                }
                if (damage != 0 && FlyingLevelExtro == 0) {
                    NewBuzz(&player->rumble, (BEENHITBIGBUZZTIME * 25) / damage);
                    NewRumble(&player->rumble, (BEENHITBIGRUMTIME * 25) / damage);
                }
                if (PlayerGlider.HitPoints <= 0) {
                    PlayerGlider.TerminalDive = 1;
                    PlayerGlider.HitPoints = 0;
                    PlayerGlider.HitTimer = Level_DeadTime;
                }
            }
        }
    }
    for (i = 0; i < 4; i++) {
        if (EnemyZoffa[i].active == 0
            || !SphereCollideDiff(&EnemyZoffa[i].pos, &PlayerGlider.Position, 1.0f, 1.0f, &diff)) {
            continue;
        }
        if (EnemyZoffa[i].terminaldive == 0) {
            if (Level == 0x1A) {
                MyGameSfx(0xD9, &PlayerGlider.Position, 0x7FFF);
            }
            if (InvincibilityCHEAT == 0 && VehicleLevelImmune == 0) {
                PlayerGlider.HitPoints -= 25;
            }
            if (FlyingLevelExtro == 0) {
                NewBuzz(&player->rumble, BEENHITBIGBUZZTIME);
                NewRumble(&player->rumble, BEENHITBIGRUMTIME);
            }
            if (PlayerGlider.HitPoints <= 0) {
                PlayerGlider.TerminalDive = 1;
                PlayerGlider.HitPoints = 0;
                PlayerGlider.HitTimer = Level_DeadTime;
            }
        }
        EnemyZoffa[i].explode = 1;
    }
    if (Level == 0x1A) {
        for (i = 0; i < 9; i++) {
            if (SatelliteList[i].active == 0
                || !SphereCollideDiff(&SatelliteList[i].pos, &PlayerGlider.Position, 3.5f, 0.333f, &diff)) {
                continue;
            }
            SatelliteList[i].destroyed = 1;
            if (InvincibilityCHEAT == 0 && VehicleLevelImmune == 0) {
                PlayerGlider.HitPoints -= 25;
            }
            if (Level == 0x1A) {
                MyGameSfx(0xD9, &PlayerGlider.Position, 0x7FFF);
            }
            NewBuzz(&player->rumble, BEENHITBIGBUZZTIME);
            NewRumble(&player->rumble, BEENHITBIGRUMTIME);
            if (PlayerGlider.HitPoints <= 0) {
                PlayerGlider.HitPoints = 0;
                PlayerGlider.TerminalDive = 1;
                PlayerGlider.HitTimer = Level_DeadTime;
            }
        }
    } else {
        struct nuvec_s outline[6];
        struct nuvec_s *points = outline;

        GetGliderCollPoints(points);
        points[5] = outline[0];
        for (i = 0; i < 6; i++) {
            if (HovaBlimp[i].active == 0
                || !SphereCollideDiff(&HovaBlimp[i].pos, &PlayerGlider.Position, BigRad, 1.0f, &diff2)) {
                continue;
            }
            if (LinesIntersectEllipse(points, 5, &HovaBlimp[i].pos, HovaRad + 3.0f, HovaY)) {
                hit = 1;
            } else {
                diff = HovaBlimp[i].pos;
                diff.y += PropYOff;
                hit = LinesIntersectEllipse(points, 5, &diff, PropRad + 3.0f, PropY) != 0;
            }
            if (hit) {
                NuVecSub(&diff2, &PlayerGlider.Position, &HovaBlimp[i].pos);
                NuVecNorm(&diff2, &diff2);
                dot = DotProduct(&diff2, &PlayerGlider.Velocity);
                if (dot < 0.0f) {
                    NuVecScaleAccum(&PlayerGlider.Velocity, &diff2, 0.2f - dot);
                    speed = NuVecMag(&PlayerGlider.Velocity);
                    if (speed > 14.0f) {
                        NuVecScale(&PlayerGlider.Velocity, &PlayerGlider.Velocity, 14.0f / speed);
                    }
                    if (PlayerGlider.OverideTiltZ == 0.0f) {
                        if (diff2.z * PlayerGlider.Velocity.x + diff2.y * PlayerGlider.Velocity.z > 0.0f) {
                            PlayerGlider.DestTiltZ = -60.0f;
                        } else {
                            PlayerGlider.DestTiltZ = 60.0f;
                        }
                    }
                    PlayerGlider.OverideTiltZ = 0.5f;
                    NewRumble(&player->rumble, BEENHITBIGRUMTIME);
                    NewBuzz(&player->rumble, BEENHITBIGBUZZTIME);
                }
            }
        }
        PlayerGlider.InTornado = 0;
        if (PlayerGlider.TerminalDive == 0) {
            for (i = 0; i < 6; i++) {
                if (TornadoList[i].active != 0
                    && SphereCollideDiff(&TornadoList[i].pos, &PlayerGlider.Position,
                                         TornadoList[i].scale * 5.0f, 1.0f, &diff2)) {
                    PlayerGlider.InTornadoScale = TornadoList[i].scale;
                    PlayerGlider.InTornado = 1;
                }
            }
        }
    }
    for (i = 0; i < 3; i++) {
        if (EnemyZoffa[i].active == 0) {
            continue;
        }
        for (j = i + 1; j < 4; j++) {
            if (EnemyZoffa[j].active == 0
                || !SphereCollideDiff(&EnemyZoffa[i].pos, &EnemyZoffa[j].pos, 1.0f, 1.0f, &diff2)) {
                continue;
            }
            if (frand() < 0.5f) {
                EnemyZoffa[i].terminaldive = 2;
                EnemyZoffa[j].explode = 1;
            } else {
                EnemyZoffa[i].explode = 1;
                EnemyZoffa[j].terminaldive = 2;
            }
        }
    }
    if (Level == 0x1A) {
        for (i = 0; i < 4; i++) {
            if (EnemyZoffa[i].active == 0) {
                continue;
            }
            for (j = 0; j < 9; j++) {
                if (SatelliteList[j].active != 0
                    && SphereCollideDiff(&EnemyZoffa[i].pos, &SatelliteList[j].pos, 3.0f, 0.6666667f, &diff2)) {
                    EnemyZoffa[i].explode = 1;
                }
            }
        }
    } else if (Level == 0xD) {
        for (i = 0; i < 4; i++) {
            if (EnemyZoffa[i].active == 0) {
                continue;
            }
            for (j = 0; j < 6; j++) {
                if (HovaBlimp[j].active != 0
                    && SphereCollideDiff(&EnemyZoffa[i].pos, &HovaBlimp[j].pos, HovaRad + 0.5f,
                                         1.0f / HovaY, &diff2)) {
                    EnemyZoffa[i].explode = 1;
                }
            }
        }
    }
    if (Level == 0xD || Level == 0x12) {
        if (EnemyZoffa[ZoffaCollisionCounter].active != 0
            && (NewRayCast(&EnemyZoffa[ZoffaCollisionCounter].pos, &ZoffaRayDir, 0.8f)
                || EnemyZoffa[ZoffaCollisionCounter].pos.y < 6.0f)) {
            EnemyZoffa[ZoffaCollisionCounter].explode = 1;
        }
        ZoffaCollisionCounter++;
        if (ZoffaCollisionCounter >= 4) {
            ZoffaCollisionCounter = 0;
        }
    }
}


inline s32 CountAsteroids(void)
{
    f32 sum = 0.0f;
    s32 i;

    for (i = 0; i < 100; i++) {
        if (AsteroidList[i].active != 0) {
            sum += AsteroidList[i].scale.x * AsteroidList[i].scale.x * 0.25f;
        }
    }
    return (s32)sum;
}


void RespawnAsteroids(void)
{
    s32 i;

    for (i = CountAsteroids(); i < 30; i++) {
        InitAsteroid(1);
    }
}

inline void InitAsteroids(void)
{
    s32 i;

    memset(AsteroidList, 0, sizeof(AsteroidList));
    for (i = 0x1D; i >= 0; i--) {
        InitAsteroid(0);
    }
}


extern s32 AsteroidDebug;

inline void ProcessAsteroids(void)
{
    s32 i;

    if (AsteroidDebug == -1) {
        for (i = 0; i < 100; i++) {
            if (AsteroidList[i].active != 0) {
                ProcessAsteroid(&AsteroidList[i]);
            }
        }
    } else {
        if (AsteroidList[AsteroidDebug].active != 0) {
            ProcessAsteroid(&AsteroidList[AsteroidDebug]);
        }
        DrawCross(&AsteroidList[AsteroidDebug].pos, 0xFF, 10.0f);
    }
}

struct wbbolt_s {
    s32 active;                /* 0x00 */
    s32 owner;                 /* 0x04 */
    struct nuvec_s pos;        /* 0x08 */
    struct nuvec_s vel;        /* 0x14 */
    f32 life;                  /* 0x20 */
    s32 type;                  /* 0x24 */
    f32 seekspeed;             /* 0x28 */
    f32 scale;                 /* 0x2C */
};
extern struct wbbolt_s BoltList[120];

struct hail_s {
    s32 state;                 /* 0x00 */
    f32 timer;                 /* 0x04 */
    f32 fallx;                 /* 0x08 */
    struct nuvec_s pos;        /* 0x0C */
    struct nuvec_s vel;        /* 0x18 */
};
extern struct hail_s HailList[30];
extern struct numtx_s BoltMtxA;
extern struct numtx_s BoltMtxB;
extern struct numtx_s BoltMtxC;

void InitLighteningHail(void)
{
    memset(HailList, 0, sizeof(HailList));
    NuMtxSetScale(&BoltMtxC, SetNuVecPntr(0.5f, 1.5f, 0.5f));
}

inline void DrawLighteningHail(void)
{
    struct hail_s *hail;
    s32 i;

    if (FlyingLevelVictoryDance != 0) {
        return;
    }
    hail = HailList;
    for (i = 0; i < 30; i++, hail++) {
        if (hail->state != 0) {
            if (hail->state == 1) {
                if (Paused == 0) {
                    AddVariableShotDebrisEffect(GDeb[0x93].i, &hail->pos, 1, 0, 0);
                }
            } else if (hail->state == 3) {
                if (Paused == 0) {
                    AddVariableShotDebrisEffect(GDeb[0x93].i, &hail->pos, 1, 0, 0);
                }
            }
        }
    }
}

extern f32 HAILFLYUPSPEED;
extern f32 HAILTIMERANGE;
extern f32 LighteningFallXMin;
extern f32 LighteningFallXMax;
extern struct nuvec_s LighteningHailVel;
extern s32 SkipLeftA;
extern s32 SkipLeftB;
extern s32 SkipLeftC;
extern s32 SkipNum;
extern f32 Level_GliderSpeed;

/* Fires the 30 hailstones of attack `attnum` in a fan, leaving a gap of
   SkipNum stones after stone SkipLeft[attnum].
   Near-match (stays asm): retail hoists the LighteningFallXMin load above
   the switch and allocates the three step registers differently. */
void UnleashLighteningHail(struct nuvec_s *pos, s32 attnum)
{
    struct hail_s *hail;
    struct nuvec_s vel;
    f32 upspeed;
    f32 timer;
    f32 fallx;
    f32 timeinc;
    f32 xinc;
    f32 fallxinc;
    f32 count;
    s32 skipleft;
    s32 i;

    upspeed = HAILFLYUPSPEED * 0.02f;
    memset(HailList, 0, sizeof(HailList));
    NuVecScale(&vel, &LighteningHailVel, 0.02f);
    timer = 2.0f;
    fallx = LighteningFallXMax;
    switch (attnum) {
    case 0:
        skipleft = SkipLeftA;
        break;
    case 1:
        skipleft = SkipLeftB;
        break;
    default:
        skipleft = SkipLeftC;
        break;
    }
    count = (f32)(SkipNum + 29);
    LighteningHailVel.z = -Level_GliderSpeed;
    timeinc = HAILTIMERANGE / count;
    xinc = -vel.x * 2.0f / count;
    fallxinc = (LighteningFallXMin - LighteningFallXMax) / count;
    hail = HailList;
    for (i = 0; i < 30; i++, hail++) {
        hail->state = 1;
        hail->timer = timer;
        hail->pos = *pos;
        hail->vel = vel;
        NuVecNorm(&hail->vel, &hail->vel);
        NuVecScale(&hail->vel, &hail->vel, upspeed);
        hail->fallx = fallx;
        if (i != skipleft) {
            vel.x += xinc;
            timer += timeinc;
            fallx += fallxinc;
        } else {
            vel.x += xinc * SkipNum;
            timer += timeinc * SkipNum;
            fallx += fallxinc * SkipNum;
        }
    }
}

extern f32 HAILHIDDENTIME;
extern f32 HailFallSpeed;
extern f32 HAILCOLLRAD;
extern f32 GLIDERHITSOUNDFREQUENCY;
extern s32 GLIDERHITSOUNDOTHERID;

/* Damages the player's glider by `hits` if it is within sqrt(rad2) of `pos`;
   returns whether it was hit. */
s32 CollidePlayerPoint(struct nuvec_s *pos, f32 rad2, s32 hits)
{
    s32 oldhits;
    f32 timerinc;

    oldhits = PlayerGlider.HitPoints;
    if (NuVecDistSqr(pos, &PlayerGlider.Position, 0) <= rad2) {
        timerinc = (f32)hits * 0.125f;
        if (InvincibilityCHEAT == 0 && VehicleLevelImmune == 0) {
            PlayerGlider.HitPoints -= hits;
        }
        PlayerGlider.HitTimer += timerinc;
        if (hits != 0) {
            NewBuzz(&player->rumble, (BEENHITBIGBUZZTIME * 25) / hits);
            NewRumble(&player->rumble, (BEENHITBIGRUMTIME * 25) / hits);
        }
        if (4.5f <= PlayerGlider.HitTimer) {
            PlayerGlider.HitTimer = 4.5f;
        }
        if (PlayerGlider.NextHitSoundTimer == 0.0f
            || (PlayerGlider.HitPoints < 1 && oldhits > 0) || hits > 4) {
            MyGameSfx(0x5A, &PlayerGlider.Position, 0x7FFF);
            PlayerGlider.NextHitSoundTimer = GLIDERHITSOUNDFREQUENCY;
        } else {
            MyGameSfx(GLIDERHITSOUNDOTHERID, &PlayerGlider.Position, 0x4FFF);
        }
        if (PlayerGlider.HitPoints < 1) {
            PlayerGlider.HitPoints = 0;
            PlayerGlider.TerminalDive = 1;
            PlayerGlider.HitTimer = Level_DeadTime;
        }
        return 1;
    }
    return 0;
}

void ProcessLighteningHail(void)
{
    struct hail_s *hail;
    s32 i;

    if (FlyingLevelVictoryDance != 0) {
        return;
    }
    hail = HailList;
    for (i = 0; i < 30; i++, hail++) {
        if (hail->state != 0) {
            if (ProcessTimer(&hail->timer) != 0) {
                hail->timer = HAILHIDDENTIME;
                hail->state++;
                if (hail->state == 3) {
                    hail->pos.x = hail->fallx;
                    hail->pos.y = 46.0f;
                    hail->pos.z = PlayerGlider.Position.z;
                    hail->timer = 1.5f;
                    hail->vel.x = 0.0f;
                    hail->vel.y = -HailFallSpeed * 0.02f;
                    hail->vel.z = -Level_GliderSpeed * 0.02f;
                } else if (hail->state == 4) {
                    hail->state = 0;
                }
            } else if (hail->state == 3) {
                if (CollidePlayerPoint(&hail->pos, HAILCOLLRAD, 5) != 0) {
                    hail->state = 0;
                }
            }
            NuVecAdd(&hail->pos, &hail->pos, &hail->vel);
        }
    }
}

extern f32 BOLTHOMESEEKTIME;

void ProcessWBBolts(void)
{
    struct wbbolt_s *bolt;
    struct nuvec_s temp;
    f32 dist;
    s32 i;

    bolt = BoltList;
    for (i = 0; i < 120; i++, bolt++) {
        if (bolt->active != 0) {
            if (ProcessTimer(&bolt->life) != 0) {
                bolt->active = 0;
            }
            if (bolt->seekspeed != 0.0f) {
                NuVecSub(&temp, &PlayerGlider.Position, &bolt->pos);
                temp.z = 0.0f;
                dist = NuVecMag(&temp);
                if (dist <= bolt->seekspeed) {
                    bolt->pos.x = PlayerGlider.Position.x;
                    bolt->pos.y = PlayerGlider.Position.y;
                    bolt->vel.x = 0.0f;
                    bolt->vel.y = 0.0f;
                } else {
                    NuVecScale(&temp, &temp, bolt->seekspeed / dist);
                    temp.z = bolt->vel.z;
                    SeekHalfLifeNUVEC(&bolt->vel, &temp, BOLTHOMESEEKTIME, 0.02f);
                }
            }
            NuVecAdd(&bolt->pos, &bolt->pos, &bolt->vel);
            switch (bolt->type) {
            case 2:
                if (CollidePlayerPoint(&bolt->pos, 9.0f, 0x19) != 0) {
                    bolt->active = 0;
                }
                break;
            default:
                if (bolt->type < 2) {
                    if (CollidePlayerPoint(&bolt->pos, 2.25f, 5) != 0) {
                        bolt->active = 0;
                    }
                } else {
                    if (CollidePlayerPoint(&bolt->pos, 2.25f, 1) != 0) {
                        bolt->active = 0;
                    }
                }
                break;
            }
        }
    }
}

extern void DrawWBBolts(void);
extern void DrawVehMasks(void);

void DrawWeatherBossLevelExtra(void)
{
    DrawGliderBullets();
    DrawWeatherBoss();
    DrawWeatherBossTarget();
    DrawLighteningHail();
    DrawWBBolts();
    DrawVehMasks();
    DrawBazookaToken();
}

/* Type-2 bolts are drawn as growing balls; the others as a debris trail,
   with a model added for the weather boss's own (owner 2) bolts. */
void DrawWBBolts(void)
{
    struct wbbolt_s *bolt;
    s32 obj;
    s32 deb;
    s32 i;
    f32 scale;

    /* (ObjTab + obj), not ObjTab[obj]: the index form changes codegen. */
    obj = 0xBD;
    bolt = BoltList;
    for (i = 0; i < 0x78; i++, bolt++) {
        if (bolt->active == 0) {
            continue;
        }
        if (bolt->type == 2) {
            if (Paused == 0) {
                AddVariableShotDebrisEffect(GDeb[0x91].i, &bolt->pos, 1, 0, 0);
            }
            scale = bolt->scale * 8.0f;
            NuMtxSetScale(&BoltMtxB, SetNuVecPntr(scale, scale, scale));
            if ((ObjTab + obj)->obj.special != 0) {
                *(struct nuvec_s *)&BoltMtxB._30 = bolt->pos;
                NuRndrGScnObj((ObjTab + obj)->obj.scene->gobjs[(ObjTab + obj)->obj.special->instance->objid],
                              &BoltMtxB);
            }
        } else {
            if (Paused == 0) {
                deb = 0x92;
                if (bolt->owner == 2) {
                    deb = 0x94;
                }
                AddVariableShotDebrisEffect(GDeb[deb].i, &bolt->pos, 1, 0, 0);
            }
            if (bolt->owner == 2 && (ObjTab + obj)->obj.special != 0) {
                *(struct nuvec_s *)&BoltMtxA._30 = bolt->pos;
                NuRndrGScnObj((ObjTab + obj)->obj.scene->gobjs[(ObjTab + obj)->obj.special->instance->objid],
                              &BoltMtxA);
            }
        }
    }
}

void DrawVehMasks(void)
{
    struct vehmask_s *mask;
    struct nuvec_s pos;
    s32 deb;
    s32 i;

    mask = VehicleMask;
    for (i = 0; i < 2; i++, mask++) {
        if (mask->active == 0) {
            continue;
        }
        if (Level == 0x17) {
            NuMtxSetRotationY(&mTEMP, NuAtan2D(GameCam[0].pos.x - mask->pos.x,
                                               GameCam[0].pos.z - mask->pos.z) + 0x8000);
        } else {
            NuMtxSetRotationY(&mTEMP, (s32)(mask->drawangy * 182.04445f));
        }
        NuMtxScale(&mTEMP, SetNuVecPntr(mask->drawscale, mask->drawscale, mask->drawscale));
        NuMtxTranslate(&mTEMP, &mask->pos);
        mask->seen = MyDrawModelNew(&mask->draw, &mTEMP, 0);
        pos = mask->pos;
        pos.y += 0.2f;
        deb = 0x7E;
        switch (mask->id) {
        case 0x57:
            deb = 0x7F;
            break;
        case 0x56:
            deb = 0x80;
            break;
        case 0x58:
            deb = 0x81;
            break;
        case 3:
            deb = 0xA3;
            break;
        }
        if (Paused == 0) {
            AddVariableShotDebrisEffect(GDeb[deb].i, &pos, 1, 0, 0);
        }
    }
}

inline struct wbbolt_s *FindFreeWBBoltOfType(s32 type)
{
    struct wbbolt_s *b;
    s32 i;

    b = BoltList;
    for (i = 0; i < 0x78; i++) {
        if (b->active != 0) {
            if (b->type == type) {
                return b;
            }
        }
        b++;
    }
    return 0;
}

inline struct wbbolt_s *FindFreeWBBolt(void)
{
    struct wbbolt_s *b;
    s32 i;

    b = BoltList;
    for (i = 0; i < 0x78; i++) {
        if (b->active == 0) {
            return b;
        }
        b++;
    }
    return 0;
}

extern f32 BOLTHOMESEEKSPEED;
extern f32 WBTYPE2SCALETIME;

struct nuvec_s *FireWBBolt(struct nuvec_s *pos, struct nuvec_s *vel, s32 type, f32 life,
                           s32 owner)
{
    struct wbbolt_s *bolt;
    s32 given;
    f32 oldscale;

    bolt = 0;
    given = 0;
    if (type == 2) {
        bolt = FindFreeWBBoltOfType(type);
    }
    if (bolt != 0) {
        given = 1;
    } else {
        bolt = FindFreeWBBolt();
    }
    if (bolt != 0) {
        if (given) {
            oldscale = bolt->scale;
        } else if (type == 2) {
            oldscale = 0.0f;
        } else {
            oldscale = 1.0f;
        }
        memset(bolt, 0, sizeof(struct wbbolt_s));
        bolt->owner = owner;
        bolt->pos = *pos;
        NuVecScale(&bolt->vel, vel, 0.02f);
        bolt->life = life;
        bolt->type = type;
        bolt->active = 2;
        bolt->scale = oldscale + 0.02f / WBTYPE2SCALETIME;
        if (bolt->scale >= 1.0f) {
            bolt->scale = 1.0f;
        }
        if (bolt->type == 0) {
            bolt->seekspeed = BOLTHOMESEEKSPEED * 0.02f;
        } else {
            bolt->seekspeed = 0.0f;
        }
        return &bolt->pos;
    }
    return 0;
}

void InitWBBolts(void)
{
    memset(BoltList, 0, sizeof(BoltList));
    NuMtxSetScale(&BoltMtxA, SetNuVecPntr(1.5f, 1.5f, 1.5f));
    NuMtxSetScale(&BoltMtxB, SetNuVecPntr(8.0f, 8.0f, 8.0f));
}

extern void DrawEarthBoss(void);

void DrawEarthBossLevelExtra(void)
{
    DrawEarthBoss();
    DrawJeepRocks();
    DrawVehMasks();
}

extern s32 EarthBossJustEntered;
extern s32 EarthBossDeathEffect;
extern f32 EarthBossDeathTimer;
extern s32 ChrisBigBossDead;
extern void InitEarthBoss(void);

void EarthBossReset(s32 playerdead)
{
    EarthBossJustEntered = 0;
    EarthBossDeathEffect = 0;
    EarthBossDeathTimer = 0.0f;
    EarthBossVortexOpen = 0;
    ChrisBigBossDead = 0;
    RumbleDisplayMode = 0;
    InitVehMasks();
    InitVehMask(0, 0x55);
    InitVehMask(1, 3);
    InitEarthBoss();
    InitJeepRocks();
    NumRockPanel = 0;
}

inline f32 PointLineSide(struct nuvec_s *p, struct nuvec_s *a, struct nuvec_s *b)
{
    f32 dx1 = b->x - p->x;
    f32 dz1 = b->z - p->z;
    f32 dx2 = b->x - a->x;
    f32 dz2 = b->z - a->z;

    return dx1 * dz2 - dz1 * dx2;
}

s32 TrailPointInPoly(struct nuvec_s *point, struct nuvec_s *a, struct nuvec_s *b,
                     struct nuvec_s *c, struct nuvec_s *d)
{
    struct nuvec_s *src = a;
    s32 i;

    for (i = 0; i < 2; i++) {
        if (PointLineSide(point, a, b) > 0.0f) {
            if (PointLineSide(point, b, c) > 0.0f &&
                PointLineSide(point, c, d) > 0.0f &&
                PointLineSide(point, d, a) > 0.0f) {
                *point = *src;
                return 1;
            }
        } else if (PointLineSide(point, b, c) <= 0.0f &&
                   PointLineSide(point, c, d) <= 0.0f &&
                   PointLineSide(point, d, a) <= 0.0f) {
            *point = *src;
            return 1;
        }
        src = b;
        point++;
    }
    return 0;
}

s32 PointsSame(struct nuvec_s *a, struct nuvec_s *b)
{
    s32 r = 0;

    if (a->x == b->x) {
        if (a->y == b->y) {
            if (a->z == b->z) {
                r = 1;
            }
        }
    }
    return r;
}

u16 FindTrailAng(struct nuvec_s *a, struct nuvec_s *b)
{
    struct nuvec_s d;

    NuVecSub(&d, b, a);
    return (u16)(NuAtan2D(d.x, d.z) - 0x2000);
}

struct jeeprock_s *FindJeepRock(void)
{
    s32 i;

    for (i = 0; i < 6; i++) {
        if (JeepRock[i].exists == 0) {
            return &JeepRock[i];
        }
    }
    return 0;
}

void KeepHoldOnRock(struct jeeprock_s *rock, struct nuvec_s *pos,
                    struct nuvec_s *vel)
{
    rock->pos = *pos;
    rock->atlas.ball_pos = *pos;
    rock->atlas.ball_oldpos = rock->atlas.ball_pos;
    rock->vel = *vel;
    rock->atlas.ball_vel = *vel;
    rock->held = 1;
}


extern s32 GetTotalFireBossObjectives(void);

s32 GetMaxLevelObjectives(void)
{
    switch (Level) {
    case 0x12:
    case 0xD:
        return 6;
    case 0x18:
        return 100;
    case 0x24:
        return 12;
    case 0x16:
        return GetTotalFireBossObjectives();
    case 0x1A:
    case 0x15:
        return 3;
    default:
        return 1;
    }
}


extern void MoveGlider(struct creature_s *c, struct nupad_s *pad);
extern void MoveAtlas(struct creature_s *c, struct nupad_s *pad);
extern void MovePlayerJeep(struct creature_s *c, struct nupad_s *pad);

void MoveVehicle(struct creature_s *c, struct nupad_s *pad)
{
    switch (c->obj.vehicle) {
    case 0x81:
    case 0x8B:
    case 0x36:
        MoveGlider(c, pad);
        return;
    case 0x53:
        MoveAtlas(c, pad);
        return;
    case 0x63:
        MovePlayerJeep(c, pad);
        return;
    }
}



extern f32 FireFlyIntroTween;
extern void edppRegisterPointerToGameCharLocation(struct nuvec_s *loc);

inline void InitGlider(struct gliderflight_s *Glider, struct nuvec_s *StartPos, f32 StartAng)
{
    struct CharacterData *cdata;
    struct creature_s *cre;
    s32 i;

    WBIntroOn = 0;
    memset(Glider, 0, sizeof(struct gliderflight_s));
    Glider->Cre = player;
    if (StartPos != 0) {
        Glider->Position = *StartPos;
    }
    Glider->DestTiltX = Glider->TiltX = 0.0f;
    Glider->DestTiltZ = Glider->TiltZ = 0.0f;
    Glider->AngleY = StartAng;
    Glider->HitPoints = 100;
    Glider->BarrelRoll = 0;
    Glider->FireTimer = 0.0f;
    Glider->RailAngle = 0.0f;
    for (i = 0; i < 16; i++) {
        Glider->LocatorList[i] = 0x7F;
        Glider->LocatorTime[i] = 0.0f;
    }
    edppRegisterPointerToGameCharLocation(&Glider->Position);
    if (Level == 0x1A) {
        cdata = &CData[0x81];
    } else {
        cdata = &CData[0x36];
    }
    cre = Glider->Cre;
    cre->obj.scale = cre->obj.SCALE = 1.0f;
    Glider->Cre->obj.bot = cdata->min.y;
    Glider->Cre->obj.top = cdata->max.y;
    FireFlyIntroTween = 0.0f;
}

extern struct gliderbullet_s GliderBullet[64];
extern struct gliderbullet_s EnemyGliderBullet[100];

static inline void InitGliderBullets(void)
{
    s32 i;

    for (i = 0; i < 64; i++) {
        FreeGliderBulletList[i] = &GliderBullet[i];
    }
    for (i = 0; i < 100; i++) {
        FreeEnemyGliderBulletList[i] = &EnemyGliderBullet[i];
    }
    FreeGliderBulletNum = 64;
    FreeEnemyGliderBulletNum = 100;
    UsedGliderBulletNum = 0;
    UsedEnemyGliderBulletNum = 0;
}

static inline void InitGliderBombs(void)
{
    memset(GliderBombs, 0, sizeof(GliderBombs));
    memset(EnemyGliderBombs, 0, sizeof(EnemyGliderBombs));
}


static inline void InitZoffaUFOs(void)
{
    s32 i;

    memset(EnemyZoffa, 0, sizeof(EnemyZoffa));
    ZoffaTeleportIndx = 0;
    for (i = 0; i < 4; i++) {
        InitZoffa(0, &ZoffaStartPoints[i]);
    }
    CurrentAggressor = 0;
    ZoffaCollisionCounter = 0;
}

extern s32 D_006332AC;
#define LevelResetTimer D_006332AC
extern f32 Level_GliderCurrentCeiling;
extern f32 Level_GliderCentreX;
extern f32 Level_GliderCentreZ;
extern f32 Level_TargetTime;
extern f32 GliderCamHighTimer;
extern s32 FireFlyIntroOn;
extern s32 FireFlyIntroAction;
extern s32 FireFlyIntroOldAction;
extern struct nuvec_s WRStartPos;
extern f32 WRStartAng;
extern struct nuvec_s CrashteroidsStart;
extern f32 CrashteroidsStartAngle;
extern void ResetGameCameras(struct cammtx_s *cam, s32 n);
extern void InitBigGuns(void);

extern s32 new_mode;
extern s32 new_level;
extern s32 D_0063329C;
#define glider_restart_timer D_0063329C
extern struct nuvec_s D_005864F8;
#define CameraPos D_005864F8
extern struct nuvec_s FarmCameraStart;
extern struct nuvec_s FarmZepplinStart;
extern f32 FarmZepplinSpeed;
extern f32 FarmZepplinTimer;
extern void ResetTimeTrial(void);

void FarmReset(s32 playerdead)
{
    if (new_mode == -1 && new_level == -1) {
        Level_GliderCentreX = 0.0f;
        Level_GliderSpeed = 13.0f;
        CameraPos = FarmCameraStart;
        Level_GliderFloor = 16.0f;
        Level_GliderCeiling = 66.0f;
        Level_GliderRadius = 177.5f;
        Level_DeadTime = 8.0f;
        glider_restart_timer = 0;
        Level_GliderCentreZ = 0.0f;
        InitGlider(&PlayerGlider, 0, 0.0f);
        PlayerGlider.AutoPilot = 1;
        InitZoffaUFOs();
        InitGliderBullets();
        InitialiseHovaBlimps();
        InitPlane(&FarmZepplinStart, FarmZepplinSpeed, FarmZepplinTimer);
        GliderCamHighTimer = 1.0f;
        ResetGameCameras(&GameCam[0], 1);
        ACTIVEBLIMPCOUNT = 6;
        ResetTimeTrial();
    }
}


void WeatherResearchReset(s32 playerdead)
{
    struct nuvec_s start;

    start = WRStartPos;
    LevelResetTimer = 0;
    Level_GliderSpeed = 16.0f;
    Level_GliderCeiling = 60.0f;
    Level_GliderFloor = -60.0f;
    Level_GliderCurrentCeiling = 60.0f;
    Level_GliderRadius = 100.0f;
    Level_GliderCentreX = 0.32f;
    Level_GliderCentreZ = 0.09f;
    Level_DeadTime = 8.0f;
    Level_TargetTime = 0.7f;
    InitGlider(&PlayerGlider, &start, WRStartAng);
    GliderCamHighTimer = 0.0f;
    GameCam[0].m._30 = start.x + 0.0f;
    GameCam[0].m._31 = start.y + 2.0f;
    GameCam[0].m._32 = start.z + 5.0f;
    ResetGameCameras(&GameCam[0], 1);
    InitGliderBullets();
    InitBigGuns();
    InitGliderBombs();
}

/* Near-match (state=asm): only the scheduling of the opening level-limit
   stores differs from retail (44 bytes); no statement order found matches. */
void SpaceArenaReset(s32 PlayerDead)
{
    Level_DeadTime = 8.0f;
    Level_GliderFloor = -30.0f;
    Level_GliderCurrentCeiling = 35.0f;
    Level_GliderRadius = 180.0f;
    Level_GliderCentreX = 0.0f;
    Level_GliderCentreZ = 0.0f;
    FireFlyIntroOn = 0;
    LevelResetTimer = 0;
    FireFlyIntroAction = 0;
    FireFlyIntroOldAction = -1;
    Level_GliderSpeed = 8.0f;
    Level_GliderCeiling = 35.0f;
    InitGlider(&PlayerGlider, &CrashteroidsStart, CrashteroidsStartAngle);
    GliderCamHighTimer = 0.0f;
    GameCam[0].m._30 = CrashteroidsStart.x + 0.0f;
    GameCam[0].m._31 = CrashteroidsStart.y + 2.0f;
    GameCam[0].m._32 = CrashteroidsStart.z + 5.0f;
    ResetGameCameras(&GameCam[0], 1);
    InitZoffaUFOs();
    InitSpaceStations();
    InitAsteroids();
    InitGliderBullets();
}

extern s32 GliderFrameCounter;
extern f32 FlyingLevelVictoryDanceTimer;
extern f32 FIREFLYVICDANCETIME;
extern struct nuvec_s FireFlyStart;
extern f32 FireFlyStartAngle;
extern void InitGunBoats(void);

/* Near-match (state=asm): like SpaceArenaReset, the opening level-limit stores
   are scheduled differently from retail (84 bytes). */
void FireFlyReset(s32 PlayerDead)
{
    Level_DeadTime = 8.0f;
    Level_GliderSpeed = 8.0f;
    Level_GliderFloor = 3.0f;
    Level_GliderCurrentCeiling = 25.0f;
    Level_GliderCeiling = 25.0f;
    Level_GliderRadius = 90.0f;
    Level_GliderCentreX = 40.32f;
    Level_GliderCentreZ = 52.09f;
    Level_TargetTime = 1.3f;
    GliderFrameCounter = 0;
    FireFlyIntroOn = 0;
    FireFlyIntroAction = 0;
    LevelResetTimer = 0;
    FlyingLevelVictoryDanceTimer = FIREFLYVICDANCETIME;
    FireFlyIntroOldAction = -1;
    InitGlider(&PlayerGlider, &FireFlyStart, FireFlyStartAngle);
    GameCam[0].m._30 = FireFlyStart.x + 0.0f;
    GameCam[0].m._31 = FireFlyStart.y + 2.0f;
    GameCam[0].m._32 = FireFlyStart.z + 5.0f;
    GliderCamHighTimer = 0.0f;
    ResetGameCameras(&GameCam[0], 1);
    InitZoffaUFOs();
    InitGunBoats();
    InitBattleShips();
    InitGliderBullets();
    InitGliderBombs();
}

extern struct nuvec_s WeatherBossTargetVel;

void InitWeatherBossTarget(void)
{
    WeatherBossTargetPos = SetNuVec(0.0f, 0.0f, -15.0f);
    WeatherBossTargetVel = v000;
    WeatherBossTargetAppearTimer = 0.0f;
}

extern f32 WBMAXY;
extern f32 WBMINY;
extern f32 WBTARGLIMX;
extern f32 WBTARGLIMY;
extern f32 WBLIMX;

void ProcessWeatherBossTarget(void)
{
    f32 range = WBMAXY - WBMINY;
    f32 mid = (WBMAXY + WBMINY) * 0.5f;

    if (WBIntroOn == 0 && FlyingLevelExtro == 0) {
        ProcessTimer(&WeatherBossTargetAppearTimer);
    }
    WeatherBossTargetPos.x = (PlayerGlider.Position.x * WBTARGLIMX) / WBLIMX;
    WeatherBossTargetPos.y =
        ((PlayerGlider.Position.y - mid) * WBTARGLIMY) / (range * 0.5f) + 1.0f;
}


struct splinepath_s {
    struct nugspline_s *spl;   /* 0x00 */
    f32 ratio;                 /* 0x04 */
    f32 oldratio;              /* 0x08 */
    char pad_0C[0x10 - 0x0C];
    f32 speed;                 /* 0x10 */
    struct nuvec_s pos;        /* 0x14 */
    struct nuvec_s lastpos;    /* 0x20 */
    s32 finished;              /* 0x2C */
};

extern struct spltab_s SplTab[];
extern struct splinepath_s WeatherBossIntroSpline;
extern f32 WBBOSSINTRODIST;
extern f32 WBIntroTweenTime;
extern f32 WBIntroTweenTimer;
extern void PointAlongSpline(struct nugspline_s *spl, f32 ratio, struct nuvec_s *dst,
                             u16 *angle, u16 *tilt);

void InitWBIntro(void)
{
    struct splinepath_s *path = &WeatherBossIntroSpline;
    u16 angle;

    WBIntroOn = 0;
    path->spl = SplTab[72].spl;
    if (path->spl != 0) {
        path->ratio = 0.0f;
        path->speed = 0.00002f;
        path->oldratio = 0.0f;
        PointAlongSpline(path->spl, path->ratio, &WBIntroGliderPos, &angle, 0);
        path->pos = WBIntroGliderPos;
        path->lastpos = WBIntroGliderPos;
        WeatherBoss.dist = WBBOSSINTRODIST;
        WeatherBoss.lastdist = WBBOSSINTRODIST;
        path->finished = 0;
        WeatherBossTargetAppearTimer = WeatherBossTargetAppearTime;
        WBIntroTweenTimer = WBIntroTweenTime;
        WBIntroOn = 1;
    }
}


extern struct panelcount_s plr_lives;
extern struct game_s Game;
extern s32 TimeTrial;
extern s32 Adventure;
extern s32 LivesLost;
extern s32 GameMode;

static inline s8 GrabAnotherLocator(char *list, struct NUPOINTOFINTEREST_s **locators)
{
    s32 tries;
    s32 loc;
    s32 j;

    loc = (s32)(frand() * 16.0f);
    for (tries = 0; tries < 16; tries++, loc++) {
        if (loc > 15) {
            loc -= 16;
        }
        if (locators[loc] != 0) {
            for (j = 0; j < 16; j++) {
                if (list[j] == 0x7F || list[j] == loc) {
                    break;
                }
            }
            if (j == 16 || list[j] == 0x7F) {
                return loc;
            }
        }
    }
    return 0x7F;
}

void GliderSmoke(struct gliderflight_s *glider)
{
    struct CharacterModel *model;
    f32 time;
    f32 val;
    f32 thisval;
    f32 maxtime;
    s32 i;

    val = 0.0f;
    i = D_0056236E[0];
    if (i == -1) {
        return;
    }
    model = &CModel[i];
    i = 0;
    if (glider->TerminalDive != 0) {
        glider->HitTimer = Level_DeadTime;
    }
    time = glider->HitTimer;
    maxtime = Level_DeadTime - 3.0f;
    while (time > 0.0f && val <= maxtime) {
        thisval = time;
        if (time > 3.0f) {
            thisval = 3.0f;
        }
        if (glider->LocatorList[i] == 0x7F) {
            glider->LocatorList[i] = GrabAnotherLocator(glider->LocatorList, model->pLOCATOR);
            if ((s8)glider->LocatorList[i] == 0x7F) {
                goto next;
            }
        }
        glider->LocatorTime[i] += thisval;
        if (glider->LocatorTime[i] >= 3.0f) {
            glider->LocatorTime[i] -= 3.0f;
            AddGameDebrisRot(0x1D, (struct nuvec_s *)&glider->Cre->mtxLOCATOR[1][glider->LocatorList[i]]._30,
                             1, 0x4000, 0);
        }
next:
        time -= 0.5f;
        val += 0.5f;
        i++;
    }
    for (; i < 16; i++) {
        glider->LocatorTime[i] = 0.0f;
        glider->LocatorList[i] = 0x7F;
    }
}

inline void ExplodeGlider(struct gliderflight_s *glider)
{
    NewBuzz(&player->rumble, BEENHITBIGBUZZTIME);
    NewRumble(&player->rumble, BEENHITBIGRUMTIME);
    if (Level == 0x1A) {
        MyGameSfx(0xD0, 0, 0x3FFF);
    } else {
        MyGameSfx(0x56, 0, 0x3FFF);
    }
    if (InvincibilityCHEAT == 0) {
        glider->Dead = 1;
        glider_restart_timer = 50;
        if (TimeTrial != 0) {
            new_mode = GameMode;
        } else if (plr_lives.count != 0) {
            plr_lives.count--;
            if (Adventure != 0) {
                LivesLost++;
                Game.lives = plr_lives.count;
            }
            new_mode = GameMode;
        } else {
            new_level = 0x26;
        }
    }
}


void DeadGliderWB(struct gliderflight_s *glider)
{
    if (glider->Dead == 0) {
        ExplodeGlider(glider);
    }
}


void DeadGliderCoco(struct gliderflight_s *glider)
{
    if (glider->CocoDead == 0) {
        glider->CocoDead = 1;
        glider->CocoDeadTimer = 3.0f;
        glider->Velocity.x = frand() - 0.5f;
        glider->Velocity.y = frand() - 0.5f;
        glider->Velocity.z = frand() - 0.5f;
        glider->CocoDeathSpinX = (frand() - 0.5f) * 360.0f;
        glider->CocoDeathSpinZ = (frand() - 0.5f) * 360.0f;
        NuVecNorm(&glider->Velocity, &glider->Velocity);
        NuVecScale(&glider->Velocity, &glider->Velocity, 2.0f);
        AddGameDebris(0x18, &glider->Position);
    }
    glider->HitPoints = 0;
    NuVecScaleAccum(&glider->Position, &glider->Velocity, 0.02f);
    glider->TiltX += glider->CocoDeathSpinX * 0.02f;
    glider->TiltZ += glider->CocoDeathSpinZ * 0.02f;
    if (ProcessTimer(&glider->CocoDeadTimer)) {
        glider->CocoDeadTimer = 1000000.0f;
        ExplodeGlider(glider);
    }
}


extern f32 WBTILTZSCALE;
extern f32 WBTILTXSCALE;
extern f32 WBBARRELSPEED;
extern f32 WBBARRELSEEKSPEEDZERO;
extern f32 WBBARRELSEEKSPEEDNONZERO;

void ProcessGliderMovementWB(struct gliderflight_s *glider)
{
    f32 dest;
    struct nuvec_s roll;

    if (WBIntroOn != 0) {
        glider->Position = WBIntroGliderPos;
        glider->Position.z = 0.0f;
        return;
    }
    if (glider->BarrelRoll == 0) {
        glider->Velocity.z = -Level_GliderSpeed;
        glider->Velocity.x = glider->TiltZ * WBTILTZSCALE;
        glider->Velocity.y = glider->TiltX * WBTILTXSCALE;
    }
    NuVecScaleAccum(&glider->Position, &glider->Velocity, 0.02f);
    if (glider->Position.x > WBLIMX) {
        glider->Position.x = WBLIMX;
    }
    if (glider->Position.x < -WBLIMX) {
        glider->Position.x = -WBLIMX;
    }
    if (glider->Position.y > WBMAXY) {
        glider->Position.y = WBMAXY;
    }
    if (glider->Position.y < WBMINY) {
        glider->Position.y = WBMINY;
    }
    if (glider->BarrelRoll > 1) {
        dest = -WBBARRELSPEED;
    } else if (glider->BarrelRoll < -1) {
        dest = WBBARRELSPEED;
    } else {
        dest = 0.0f;
    }
    if (dest == 0.0f) {
        SeekHalfLife(&glider->BarrelSpeedX, dest, WBBARRELSEEKSPEEDZERO, 0.02f);
    } else {
        SeekHalfLife(&glider->BarrelSpeedX, dest, WBBARRELSEEKSPEEDNONZERO, 0.02f);
    }
    roll = SetNuVec(glider->BarrelSpeedX, 0.0f, 0.0f);
    NuVecRotateY(&roll, &roll, (s32)(glider->AngleY * 182.04445f));
    glider->Position.x += roll.x * 0.02f;
    glider->Position.y += roll.y * 0.02f;
    glider->Position.z += roll.z * 0.02f;
}


inline void DrawAsteroids(void)
{
    s32 i;

    for (i = 0; i < 100; i++) {
        if (AsteroidList[i].active != 0) {
            DrawAsteroid(&AsteroidList[i]);
        }
    }
}

void DrawSpaceArenaLevelExtra(void)
{
    DrawGliderBullets();
    DrawAsteroids();
    DrawSatellites();
    DrawSpaceStations();
    DrawZoffaUFOs();
    DrawGliderTarget();
}


/* GunBoatList: stride 0x134, 4 entries. */
struct gunboat_s {
    s32 active;                /* 0x000 */
    s32 character;             /* 0x004 -1 if the model failed to load */
    struct MYDRAW draw;        /* 0x008 */
    struct nuvec_s pos;        /* 0x0E8 */
    f32 yaw;                   /* 0x0F4 */
    f32 pitch;                 /* 0x0F8 */
    f32 yawwobble;             /* 0x0FC */
    char pad_100[0x118 - 0x100];
    s32 seen;                  /* 0x118 */
    s32 action;                /* 0x11C */
    s32 lastaction;            /* 0x120 */
    char pad_124[0x134 - 0x124];
};
extern struct gunboat_s GunBoatList[];

inline void InitGunBoat(struct nuvec_s *pos, f32 yaw, s32 character)
{
    struct gunboat_s *boat;
    s32 id;

    boat = &GunBoatList[character];
    if (boat->active != 0) {
        return;
    }
    memset(&GunBoatList[character], 0, sizeof(struct gunboat_s));
    boat->character = character;
    boat->pos = *pos;
    boat->yaw = yaw;
    boat->action = 1;
    boat->lastaction = -1;
    boat->pitch = 45.0f;
    switch (character) {
    case 0:
        id = 0x68;
        break;
    case 1:
        id = 0x69;
        break;
    case 2:
        id = 0x67;
        break;
    default:
        id = 0x66;
        break;
    }
    if (MyInitModelNew(&boat->draw, id, 0x22, 0, 0, &boat->pos) == 0) {
        boat->character = -1;
    }
    boat->active = 1;
}

void InitGunBoats(void)
{
    memset(GunBoatList, 0, sizeof(struct gunboat_s) * 4);
    InitGunBoat(SetNuVecPntr(-24.0f, 0.0f, 50.0f), 0.0f, 0);
    InitGunBoat(SetNuVecPntr(36.0f, 0.0f, -15.0f), 0.0f, 1);
    InitGunBoat(SetNuVecPntr(93.0f, 0.0f, 43.0f), 0.0f, 2);
    InitGunBoat(SetNuVecPntr(21.0f, 0.0f, 106.0f), 0.0f, 3);
}


inline void DrawGunBoat(struct gunboat_s *boat)
{
    struct nuvec_s scale = gun_scale[0];

    NuMtxSetScale(&mTEMP, &scale);
    NuMtxRotateX(&mTEMP, boat->pitch * 182.04445f);
    NuMtxRotateY(&mTEMP, (boat->yaw + boat->yawwobble) * 182.04445f);
    NuMtxTranslate(&mTEMP, &boat->pos);
    boat->seen = MyDrawModelNew(&boat->draw, &mTEMP, 0);
}

void DrawFireFlyLevelExtra(void)
{
    s32 i;

    for (i = 0; i < 4; i++) {
        if (GunBoatList[i].active != 0) {
            DrawGunBoat(&GunBoatList[i]);
        }
    }
    for (i = 0; i < 6; i++) {
        if (BattleShipList[i].active != 0) {
            DrawBattleShip(&BattleShipList[i]);
        }
    }
    DrawGliderBombs();
    DrawGliderBullets();
    DrawZoffaUFOs();
    DrawTorpedoTarget();
}
extern void ProcessGunBoat(struct gunboat_s *boat);

inline void ProcessGunBoats(void)
{
    s32 i;

    for (i = 0; i < 4; i++) {
        if (GunBoatList[i].active != 0) {
            ProcessGunBoat(&GunBoatList[i]);
        }
    }
}


extern void ProcessBigGun(struct biggun_s *gun);

inline void ProcessBigGuns(void)
{
    s32 i;

    for (i = 0; i < 12; i++) {
        if (BigGunList[i].active != 0) {
            ProcessBigGun(&BigGunList[i]);
        }
    }
}


extern void ProcessBattleShip(struct battleship_s *ship);

inline void ProcessBattleShips(void)
{
    s32 i;

    for (i = 0; i < 6; i++) {
        if (BattleShipList[i].active != 0) {
            ProcessBattleShip(&BattleShipList[i]);
        }
    }
}


extern void ProcessGliderBombs(void);
extern void TeleportManager(void);
extern void MoveZoffaUFO(struct zoffa_s *ufo);
extern struct zoffastart_s *FindZoffaRestartPoint(void);
extern void CheckGliderCollisions(void);
extern void CheckForPotentialMidAirCollisions(void);
extern s32 NumZoffasFiring;
extern s32 LastNumZoffasFiring;

/* Flies the live UFOs and runs down the dead ones' respawn timers (paused
 * while the glider is on autopilot). */
static inline void MoveZoffaUFOs(void)
{
    s32 i;

    LastNumZoffasFiring = NumZoffasFiring;
    NumZoffasFiring = 0;
    for (i = 0; i < 4; i++) {
        if (EnemyZoffa[i].active == 0) {
            if (PlayerGlider.AutoPilot == 0) {
                EnemyZoffa[i].respawntimer -= 0.02f;
                if (EnemyZoffa[i].respawntimer < 0.0f) {
                    EnemyZoffa[i].respawntimer = 0.0f;
                }
            }
        } else {
            MoveZoffaUFO(&EnemyZoffa[i]);
        }
    }
}

/* Brings one dead UFO back once its respawn timer has run out. */
static inline void ZoffaUFORespawn(void)
{
    struct zoffastart_s *start;
    s32 active;
    s32 i;

    active = 0;
    for (i = 0; i < 4; i++) {
        if (EnemyZoffa[i].active != 0) {
            active++;
        }
    }
    if (active < 4) {
        for (i = 0; i < 4; i++) {
            if (EnemyZoffa[i].active == 0 && EnemyZoffa[i].respawntimer == 0.0f) {
                start = FindZoffaRestartPoint();
                if (start == 0) {
                    return;
                }
                InitZoffa(&EnemyZoffa[i], start);
                return;
            }
        }
    }
}

void ProcessFireFlyLevel(struct nupad_s *pad)
{
    TeleportManager();
    MoveZoffaUFOs();
    ZoffaUFORespawn();
    CheckGliderCollisions();
    CheckForPotentialMidAirCollisions();
    ProcessGliderBombs();
    ProcessBattleShips();
    ProcessGunBoats();
    ProcessGliderBullets();
    GliderFrameCounter++;
}

extern s32 FarmFrame;
extern void FarmReset(s32 playerdead);

extern f32 Level_GliderCurrentCeiling;
extern struct nuvec_s ZepplinGliderOffset;
extern void GameSfx(s32 id, struct nuvec_s *pos);

void MovePlane(struct plane_s *plane)
{
    s32 action;
    s32 grabglider;

    grabglider = 0;
    if (plane->active != 0) {
        action = plane->actionstatus;
        switch (action) {
        case 0:
            grabglider = 1;
            Level_GliderCurrentCeiling = 36.0f;
            if (ProcessTimer(&plane->actiontimer) != 0) {
                plane->actionstatus = 1;
            }
            break;
        case 1:
            plane->actiontimer = 1.0f;
            plane->actionstatus = 2;
            PlayerGlider.ForceTurn = action;
            GameSfx(0x82, &player->obj.pos);
            NewRumble(&player->rumble, 0x7F);
            NewBuzz(&player->rumble, 5);
            break;
        case 2:
            if (ProcessTimer(&plane->actiontimer) != 0) {
                plane->actionstatus = 3;
            }
            break;
        case 3:
            plane->active = 0;
            break;
        }
        NuVecScaleAccum(&plane->pos, &plane->vel, 0.02f);
        PlayerGlider.AutoPilot = grabglider;
        if (grabglider != 0) {
            NuVecAdd(&PlayerGlider.Position, &plane->pos, &ZepplinGliderOffset);
            D_006B75A0 = PlayerGlider.Position;
            PlayerGlider.TiltX = 0.0f;
            PlayerGlider.TiltZ = 0.0f;
            PlayerGlider.Velocity = plane->vel;
        }
        MyAnimateModelNew(&plane->draw, 0.59999996f);
    }
}


void ProcessFarmLevel(struct nupad_s *pad)
{
    FarmFrame++;
    if (GliderCamHighTimer > 0.0f) {
        GliderCamHighTimer -= 0.02f;
    } else {
        GliderCamHighTimer = 0.0f;
    }
    if (glider_restart_timer != 0) {
        glider_restart_timer--;
        if (glider_restart_timer == 0) {
            FarmReset(1);
        }
    }
    TeleportManager();
    MoveZoffaUFOs();
    ZoffaUFORespawn();
    ProcessAllHovaBlimps();
    CheckGliderCollisions();
    CheckForPotentialMidAirCollisions();
    MovePlane(&PlayerPlane);
    ProcessGliderBullets();
}

struct MYSPLINE;
extern struct MYSPLINE WeatherBossCamSpline;
extern f32 FindSplineClosestPointAndDist(struct MYSPLINE *spline, s32 control, struct nuvec_s *point,
                                         struct nuvec_s *closest, s32 wrap, s32 biglook);
extern f32 WBCamDist;
extern f32 D_0063327C;
extern f32 D_00633280;
#define GliderCamAngleX D_0063327C
#define GliderCamAngleY D_00633280

void GliderCamWeatherBoss(struct cammtx_s *cam)
{
    struct nuvec_s delta;
    struct nuvec_s campos;
    struct nuvec_s lookpos;
    struct nuvec_s target;
    struct nuvec_s look;
    struct nuvec_s mid;
    struct nuvec_s from;
    struct nuvec_s railpos;
    f32 dist;

    target = PlayerGlider.Position;
    target.z += WBCamDist;
    if (WBIntroOn == 0) {
        ProcessTimer(&WBIntroTweenTimer);
    }
    look = PlayerGlider.Position;
    from = PlayerGlider.Position;
    from.z += WBCamDist;
    FindSplineClosestPointAndDist(&WeatherBossCamSpline, 0x300, &target, &railpos, 0, 0);
    mid.x = railpos.x + (PlayerGlider.Position.x - railpos.x) * 0.5f;
    mid.y = railpos.y;
    mid.z = PlayerGlider.Position.z;
    NuVecScale(&lookpos, &look, WBIntroTweenTimer / WBIntroTweenTime);
    NuVecScaleAccum(&lookpos, &mid, 1.0f - WBIntroTweenTimer / WBIntroTweenTime);
    NuVecScale(&campos, &from, WBIntroTweenTimer / WBIntroTweenTime);
    NuVecScaleAccum(&campos, &railpos, 1.0f - WBIntroTweenTimer / WBIntroTweenTime);
    delta.x = lookpos.x - campos.x;
    delta.y = lookpos.y - campos.y;
    delta.z = lookpos.z - campos.z;
    dist = NuFsqrt(delta.x * delta.x + delta.z * delta.z);
    cam->xrot = NuAtani(-delta.y * 256.0f, dist * 256.0f);
    cam->yrot = NuAtani(delta.x * 256.0f, delta.z * 256.0f);
    GliderCamAngleX = cam->xrot / 182.0444489f;
    GliderCamAngleY = cam->yrot / 182.0444489f;
    NuMtxSetRotationX(&cam->m, GliderCamAngleX * 182.0444489f);
    NuMtxRotateY(&cam->m, GliderCamAngleY * 182.0444489f);
    NuMtxTranslate(&cam->m, &campos);
    cam->pos = campos;
    WeatherBossCamMtx = cam->m;
}


extern struct nuvec_s GliderIntroCamPos;
extern struct nuvec_s GliderIntroInterest;
extern f32 FlyingLevelCompleteTimer;
extern f32 SeekGliderCamTime;
extern f32 CameraSeekSpeedAutoPilot;
extern f32 FFTWEENTIME;
extern f32 TORNADOINTERESTY;
extern struct nuvec_s TORNADOCAMIDEAL;
extern f32 ExtroTargAng;
extern f32 ExtroAngOff;
extern f32 ExtroChangeVal;
extern f32 ExtroSeekZeroTime;
extern f32 ExtroStillToEndTime;
extern struct nuvec_s ExtroIdealPos;
extern struct nuvec_s ExtroStillVelocity;
extern struct nuvec_s ExtroStillCamPos;
extern struct nuvec_s ExtroStillInterest;
extern struct numtx_s GliderInvMtx;
extern struct nuvec_s D_006B7580;
#define GliderCamLastVel D_006B7580
extern f32 D_00633284;
extern f32 D_00633288;
#define GliderCamTiltX D_00633284
#define GliderCamTurnY D_00633288
extern void SeekAngLimHalfLife360f(f32 *dest, f32 target, f32 maxspeed, f32 halflife, f32 dt);

void GliderCam(struct cammtx_s *cam)
{
    struct nuvec_s delta;
    struct nuvec_s campos;
    struct nuvec_s lookpos;
    struct nuvec_s ideal;
    struct nuvec_s introlook;
    struct nuvec_s introcam;
    struct nuvec_s offset;
    struct nuvec_s extropos;
    f32 tilt;
    f32 seek;
    f32 tween;
    f32 untween;
    f32 dist;

    if (Level == 0x18) {
        GliderCamWeatherBoss(cam);
        return;
    }
    introlook = GliderIntroInterest;
    campos.x = cam->m._30;
    campos.y = cam->m._31;
    campos.z = cam->m._32;
    introcam = campos;
    SeekHalfLifeNUVEC(&introcam, &GliderIntroCamPos, SeekGliderCamTime, 0.02f);
    if ((Level != 0x12 && Level != 0x1A) || FireFlyIntroOn == 0) {
        tilt = PlayerGlider.CamTiltX;
        lookpos = PlayerGlider.Position;
        lookpos.y += 1.0f;
        if (Level == 0xD) {
            lookpos.y += TORNADOINTERESTY;
            offset = TORNADOCAMIDEAL;
        } else if (Level == 0x1A) {
            offset = SetNuVec(0.0f, 0.75f, 3.0f);
        } else {
            offset = SetNuVec(0.0f, 3.0f, 4.0f);
        }
        NuVecRotateX(&offset, &offset, tilt * 182.0444489f);
        if (FlyingLevelExtro != 0) {
            SeekAngLimHalfLife360f(&ExtroAngOff, ExtroTargAng, 120.0f, 0.3f, 0.02f);
            NuVecScale(&extropos, &offset, 1.0 - fabs(ExtroAngOff) / 180.0);
            NuVecScaleAccum(&extropos, &ExtroIdealPos, fabs(ExtroAngOff) / 180.0);
            if (fabs(Rationalise360f(ExtroAngOff - ExtroTargAng)) < ExtroChangeVal) {
                NuVecScaleAccum(&ExtroStillCamPos, &ExtroStillVelocity, 0.02f);
                NuVecScaleAccum(&ExtroStillInterest, &ExtroStillVelocity, 0.02f);
                ideal = ExtroStillCamPos;
                FlyingLevelVictoryDance = 1;
                lookpos = ExtroStillInterest;
                SeekHalfLifeNUVEC(&ExtroStillVelocity, &v000, ExtroSeekZeroTime, 0.02f);
            } else {
                NuVecRotateY(&ideal, &extropos, (PlayerGlider.CamAngleY + ExtroAngOff) * 182.0444489f);
                NuVecAdd(&ideal, &ideal, &lookpos);
                FlyingLevelCompleteTimer = ExtroStillToEndTime;
                ExtroStillVelocity = PlayerGlider.Velocity;
                ExtroStillCamPos = ideal;
                ExtroStillInterest = lookpos;
            }
        } else {
            NuVecRotateY(&ideal, &offset, PlayerGlider.CamAngleY * 182.0444489f);
            ExtroAngOff = 0.0f;
            NuVecAdd(&ideal, &ideal, &lookpos);
        }
        campos.x = cam->m._30;
        campos.y = cam->m._31;
        campos.z = cam->m._32;
        if (PlayerGlider.TerminalDive != 0 || PlayerGlider.Dead != 0 || PlayerGlider.CocoDead != 0) {
            SeekHalfLifeNUVEC(&GliderCamLastVel, SetNuVecPntr(0.0f, 0.0f, 0.0f), 0.5f, 0.02f);
            NuVecAdd(&campos, &campos, &GliderCamLastVel);
        } else {
            if (PlayerGlider.AutoPilot != 0) {
                seek = CameraSeekSpeedAutoPilot;
            } else {
                seek = SeekGliderCamTime;
            }
            GliderCamLastVel = campos;
            SeekHalfLifeNUVEC(&campos, &ideal, seek, 0.02f);
            if ((Level == 0x12 || Level == 0x1A) && FireFlyIntroTween != 0.0f && FFTWEENTIME != 0.0f) {
                tween = FireFlyIntroTween / FFTWEENTIME;
                untween = 1.0f - tween;
                NuVecScale(&lookpos, &lookpos, untween);
                NuVecScaleAccum(&lookpos, &introlook, tween);
                NuVecScale(&campos, &campos, untween);
                NuVecScaleAccum(&campos, &introcam, tween);
                ProcessTimer(&FireFlyIntroTween);
            }
            if (Level == 0x12 && campos.y >= Level_GliderCeiling - 2.0f) {
                campos.y = Level_GliderCeiling - 2.0f;
            }
            NuVecSub(&GliderCamLastVel, &campos, &GliderCamLastVel);
        }
    } else {
        lookpos = introlook;
        FireFlyIntroTween = FFTWEENTIME;
        campos = introcam;
    }
    delta.x = lookpos.x - campos.x;
    delta.y = lookpos.y - campos.y;
    delta.z = lookpos.z - campos.z;
    dist = NuFsqrt(delta.x * delta.x + delta.z * delta.z);
    cam->xrot = NuAtani(-delta.y * 256.0f, dist * 256.0f);
    cam->yrot = NuAtani(delta.x * 256.0f, delta.z * 256.0f);
    GliderCamTiltX = cam->xrot / 182.0444489f;
    GliderCamTurnY = cam->yrot / 182.0444489f;
    NuMtxSetRotationX(&cam->m, GliderCamTiltX * 182.0444489f);
    NuMtxRotateY(&cam->m, GliderCamTurnY * 182.0444489f);
    NuMtxTranslate(&cam->m, &campos);
    NuMtxRotateY(&GliderInvMtx, -GliderCamTurnY * 182.0444489f);
    NuMtxSetRotationX(&GliderInvMtx, -GliderCamTiltX * 182.0444489f);
    NuMtxTranslate(&GliderInvMtx, &campos);
    cam->pos = campos;
}


extern s32 CRASHTEROIDSAMBIENTVOL;

void ProcessSpaceArenaLevel(struct nupad_s *pad)
{
    if (CRASHTEROIDSAMBIENTVOL != 0) {
        MyGameSfxLoop(0xDA, 0, CRASHTEROIDSAMBIENTVOL);
    }
    ActiveAsteroidListNum = 0;
    TeleportManager();
    MoveZoffaUFOs();
    ZoffaUFORespawn();
    ProcessGliderBullets();
    ProcessAsteroids();
    ProcessSatellites();
    ProcessSpaceStations();
    CheckGliderCollisions();
    CheckForPotentialMidAirCollisions();
}

inline void ProcessWeatherResearchLevel(struct nupad_s *pad)
{
    ProcessGliderBullets();
    ProcessGliderBombs();
    ProcessBigGuns();
}


extern void ProcessWesternArenaLevel(struct nupad_s *pad);
extern void ProcessEarthBossLevel(struct nupad_s *pad);
extern void ProcessFireBossLevel(struct nupad_s *pad);
extern s32 GetCurrentLevelObjectives(void);

void ProcessVehicleLevel(struct nupad_s *pad)
{
    switch (Level) {
    case 0xD:
        ProcessFarmLevel(pad);
        break;
    case 0x12:
        ProcessFireFlyLevel(pad);
        break;
    case 0x24:
        ProcessWeatherResearchLevel(pad);
        break;
    case 0x18:
        ProcessWeatherBossLevel(pad);
        break;
    case 0x1A:
        ProcessSpaceArenaLevel(pad);
        break;
    case 0x3:
        ProcessWesternArenaLevel(pad);
        break;
    case 0x15:
        ProcessEarthBossLevel(pad);
        break;
    case 0x16:
        ProcessFireBossLevel(pad);
        break;
    }
    switch (Level) {
    case 0x18:
    case 0xD:
    case 0x12:
    case 0x1A:
    case 0x24:
        if (GetCurrentLevelObjectives() == 0) {
            VehicleLevelImmune = 1;
            FlyingLevelExtro = 1;
            ProcessTimer(&FlyingLevelCompleteTimer);
        }
        break;
    }
}


extern s32 GetCurrentFireBossObjectives(void);

s32 GetCurrentLevelObjectives(void)
{
    switch (Level) {
    case 0xD:
        return GetCurrentFarmObjectives();
    case 0x12:
        return GetCurrentFireFlyObjectives();
    case 0x1A:
        return GetCurrentSpaceArenaObjectives();
    case 0x18:
        return GetCurrentWeatherBossObjectives();
    case 0x24:
        return GetCurrentWeatherResearchObjectives();
    case 0x16:
        return GetCurrentFireBossObjectives();
    case 0x15:
        return GetCurrentRumbleObjectives();
    default:
        return 0;
    }
}


struct earthbossstate_s {
    struct creature_s *owner;           /* 0x00 */
    s32 whacko;                         /* 0x04 */
    s32 type;                           /* 0x08 */
    s32 dead;                           /* 0x0C */
    s32 hitpoints;                      /* 0x10 */
    s32 desthitpoints;                  /* 0x14 */
    s32 hitpointcounter;                /* 0x18 */
    s32 drawcrunch;                     /* 0x1C */
    s32 drawshell;                      /* 0x20 */
    f32 inhibitcontroltimer;            /* 0x24 */
    s32 numattacks;                     /* 0x28 */
    s32 action;                         /* 0x2C */
    s32 lastaction;                     /* 0x30 */
    f32 actiontimer;                    /* 0x34 */
};

extern f32 HitMag;
extern s32 AtlasHitsLost;
extern s32 D_005B7C94[];
#define earthboss_whacko D_005B7C94
extern char D_006316E0[]; /* "Enter" */
extern char D_006316E8[]; /* "Form A" */
extern char D_006316F0[]; /* "Form B" */
extern char D_006316F8[]; /* "Stand" */
extern char D_00631700[]; /* "Roll" */
extern char D_0061F798[]; /* "WaitRoll" */
extern char D_00631708[]; /* "Slow" */
extern char D_0061F7A8[]; /* "Been Hit" */
extern char D_0061F7B8[]; /* "Wait Camera" */
extern char D_0061F7C8[]; /* "Wait Big Jump" */
extern char D_0061F7D8[]; /* "Big Jump" */
extern char D_00631710[]; /* "Whacko" */
extern char D_00631718[]; /* "" */
extern char D_0061F7E8[]; /* "Action %s" */
extern char D_0061F7F8[]; /* "Timer %1.3f" */
extern char D_0061F808[]; /* "HitMag %1.3f" */
extern char D_0061F818[]; /* "Damage %d" */
extern char D_0061F828[]; /* "Jonny On" */

void PrintEarthBossAction(struct earthbossstate_s *boss)
{
    char *name;

    NuFntSet(app_fnt);
    NuFntSetPen(0x7F7F7F7F);
    NuFntScale(0x10, 0x10);
    switch (boss->action) {
    case 0:
        name = D_006316E0;
        break;
    case 1:
        name = D_006316E8;
        break;
    case 2:
        name = D_006316F0;
        break;
    case 3:
        name = D_006316F8;
        break;
    case 4:
        name = D_00631700;
        break;
    case 5:
        name = D_0061F798;
        break;
    case 6:
        name = D_00631708;
        break;
    case 7:
        name = D_0061F7A8;
        break;
    case 8:
        name = D_0061F7B8;
        break;
    case 9:
        name = D_0061F7C8;
        break;
    case 10:
        name = D_0061F7D8;
        break;
    case 11:
        name = D_00631710;
        break;
    default:
        name = D_00631718;
        break;
    }
    NuFntPrintEx(0x80, SHEIGHT * 2, 0, D_0061F7E8, name);
    NuFntPrintEx(0x80, SHEIGHT * 5 / 2, 0, D_0061F7F8, boss->actiontimer);
    NuFntPrintEx(0x80, SHEIGHT * 3, 0, D_0061F808, HitMag);
    NuFntPrintEx(0x80, SHEIGHT * 7 / 2, 0, D_0061F818, AtlasHitsLost);
    if (earthboss_whacko[0] != 0) {
        NuFntPrintEx(0x80, SHEIGHT * 4, 0, D_00631710);
    }
    if (JonnyOn != 0) {
        NuFntPrintEx(0x80, SHEIGHT * 9 / 2, 0, D_0061F828);
    }
}


extern void WeatherBossReset(s32 playerdead);
extern void WesternArenaReset(s32 playerdead);
extern void FireBossReset(s32 playerdead);

void ResetVehicleLevel(s32 playerdead)
{
    FlyingLevelCompleteTimer = 1.5f;
    FireFlyIntroOldAction = -1;
    VehicleLevelImmune = 0;
    FlyingLevelExtro = 0;
    FlyingLevelVictoryDance = 0;
    FlyingLevelVictoryDanceTimer = 0;
    FireFlyIntroAction = 0;
    FireFlyIntroTween = 0.0f;
    FireFlyIntroOn = 0;
    GliderIntroInterest = v000;
    GliderIntroCamPos = v000;
    switch (Level) {
    case 0xD:
        FarmReset(playerdead);
        break;
    case 0x12:
        FireFlyReset(playerdead);
        break;
    case 0x24:
        WeatherResearchReset(playerdead);
        break;
    case 0x1A:
        SpaceArenaReset(playerdead);
        break;
    case 0x18:
        WeatherBossReset(playerdead);
        break;
    case 0x3:
        WesternArenaReset(playerdead);
        break;
    case 0x16:
        FireBossReset(playerdead);
        break;
    case 0x15:
        EarthBossReset(playerdead);
        break;
    }
    if (Level == 0x12) {
        ProcessFireFlyIntro();
    }
    if (Level == 0x1A) {
        ProcessCrashteroidsIntro();
    }
}


extern struct nuvec_s SpaceFirePosA;
extern struct nuvec_s SpaceFirePosB;
extern f32 TornFireY;
extern struct nuvec_s WeatherBossFirePoint;
extern f32 WBBULLETSPEED;
extern f32 WBBULLETFIRERATE;
extern f32 TorpedoTime;
extern f32 TARGETBLEEPTIME;
extern s32 TORPTARGFAILBLEEPTIME;
extern s32 TORPTARGBLEEPTIME;
extern s32 TORPTARGAQUIREBLEEPTIME;
extern s32 D_00633274;
#define LastFireButton D_00633274
extern f32 D_00633278;
#define FireBleepTimer D_00633278
extern struct nuvec_s D_0061F300;
#define bullet_scale D_0061F300
extern void NuVecMtxRotateVU0(struct nuvec_s *dst, struct nuvec_s *src, struct numtx_s *mtx);

void GliderFire(struct gliderflight_s *glider, struct nupad_s *pad)
{
    s32 firebutton;
    struct numtx_s mat;
    struct nuvec_s spacetargetdir;
    struct nuvec_s torntargetdir;
    struct nuvec_s spacevel;
    struct nuvec_s spacepos;
    struct nuvec_s tornfiredir;
    struct nuvec_s tornscale;
    struct nuvec_s tornpos;
    struct nuvec_s tornrot[2];
    struct nuvec_s wbveldir;
    struct nuvec_s wbscale;
    struct nuvec_s wbfirept;
    struct nuvec_s wbpos;
    struct nuvec_s wbrot[2];
    struct nuvec_s bombpos;

    firebutton = pad->paddata & 0x40;

    switch (Level) {
    case 0x1A:
        NuMtxSetRotationZ(&mat, (s32)(glider->TiltZ * 182.04445f));
        NuMtxRotateX(&mat, (s32)((glider->TiltX * 2.0f - 10.0f) * 182.04445f));
        NuMtxRotateY(&mat, (s32)(glider->AngleY * 182.04445f));
        NuVecMtxRotateVU0(&spacetargetdir, SetNuVecPntr(0.0f, -3.5f, -133.33333f), &mat);
        NuVecMtxRotateVU0(&GliderTargetPos, SetNuVecPntr(0.0f, -0.1f, 0.0f), &mat);
        NuVecAdd(&GliderTargetPos, &GliderTargetPos, &glider->Position);
        NuVecScaleAccum(&GliderTargetPos, &spacetargetdir, 0.74f);
        break;
    case 0xD:
        NuMtxSetRotationZ(&mat, (s32)(glider->TiltZ * 182.04445f));
        NuMtxRotateX(&mat, (s32)(glider->TiltX * 182.04445f));
        NuMtxRotateY(&mat, (s32)(glider->AngleY * 182.04445f));
        GliderTargetPos = *(struct nuvec_s *)&glider->Cre->mtxLOCATOR[1][0]._30;
        NuVecMtxRotateVU0(&torntargetdir, SetNuVecPntr(0.0f, -8.5f, -50.0f), &mat);
        NuVecScaleAccum(&GliderTargetPos, &torntargetdir, 1.5f);
        break;
    case 0x18:
    default:
        NuMtxSetRotationZ(&mat, (s32)(glider->TiltZ * 182.04445f));
        NuMtxRotateX(&mat, (s32)((glider->TiltX + 0.0f) * 182.04445f));
        NuMtxRotateY(&mat, (s32)(glider->AngleY * 182.04445f));
        break;
    }

    if (glider->TerminalDive != 0) {
        goto end;
    }
    ProcessTimer(&glider->FireTimer);
    if (FlyingLevelExtro != 0) {
        goto end;
    }
    if (WBIntroOn != 0) {
        goto end;
    }
    if (FlyingLevelCompleteTimer == 0.0f) {
        goto end;
    }
    if (glider->AutoPilot != 0) {
        goto end;
    }
    if (glider->FireTimer != 0.0f) {
        goto end;
    }
    if (!(pad->paddata & 0x40) && Level != 0x12 && Level != 0x24) {
        goto end;
    }

    switch (Level) {
    case 0x1A:
        NuVecMtxRotateVU0(&spacevel, SetNuVecPntr(-SpaceFirePosA.x / 0.75f, 0.0f, -120.0f), &mat);
        NuVecMtxRotateVU0(&spacepos, &SpaceFirePosA, &mat);
        NuVecAdd(&spacepos, &spacepos, &glider->Position);
        NuVecScaleAccum(&spacepos, &spacevel, -0.01f);
        AddGliderBullet(&mat, &spacepos, &spacevel, 0);
        NuVecMtxRotateVU0(&spacevel, SetNuVecPntr(-SpaceFirePosB.x / 0.75f, 0.0f, -120.0f), &mat);
        NuVecMtxRotateVU0(&spacepos, &SpaceFirePosB, &mat);
        NuVecAdd(&spacepos, &spacepos, &glider->Position);
        NuVecScaleAccum(&spacepos, &spacevel, -0.01f);
        AddGliderBullet(&mat, &spacepos, &spacevel, 0);
        glider->FireTimer = 0.12f;
        MyGameSfx(0xCF, &spacepos, 0x1FFF);
        NewBuzz(&player->rumble, 3);
        break;
    case 0xD:
        tornscale = bullet_scale;
        NuVecMtxRotate(&tornrot[0], SetNuVecPntr(0.0f, 1.0f, 0.0f), &mat);
        NuVecMtxRotate(&tornrot[1], SetNuVecPntr(1.0f, 0.0f, 0.0f), &mat);
        tornfiredir = SetNuVec(0.0f, TornFireY, -50.0f);
        NuVecMtxRotateVU0(&tornfiredir, &tornfiredir, &mat);
        NuMtxScale(&mat, &tornscale);
        tornpos = *(struct nuvec_s *)&glider->Cre->mtxLOCATOR[1][0]._30;
        NuVecScaleAccum(&tornpos, &tornrot[0], (frand() - 0.5f) * 0.5f);
        NuVecScaleAccum(&tornpos, &tornrot[1], (frand() - 0.5f) * 0.5f);
        if (AddGliderBullet(&mat, &tornpos, &tornfiredir, 0) == 0) {
            goto end;
        }
        glider->FireTimer = 0.12f;
        GameSfx(0x8B, &tornpos);
        NewBuzz(&player->rumble, 3);
        break;
    case 0x18:
        wbscale = bullet_scale;
        wbfirept = WeatherBossFirePoint;
        NuVecMtxRotate(&wbrot[0], SetNuVecPntr(0.0f, 1.0f, 0.0f), &mat);
        NuVecMtxRotate(&wbrot[1], SetNuVecPntr(1.0f, 0.0f, 0.0f), &mat);
        wbpos = *(struct nuvec_s *)&glider->Cre->mtxLOCATOR[1][0]._30;
        NuVecScaleAccum(&wbpos, &wbrot[0], (frand() - 0.5f) * 0.5f);
        NuVecScaleAccum(&wbpos, &wbrot[1], (frand() - 0.5f) * 0.5f);
        wbfirept.z -= (wbpos.z - WeatherBossFirePoint.z) / (WBBULLETSPEED - Level_GliderSpeed)
                      * Level_GliderSpeed;
        NuVecSub(&wbveldir, &wbfirept, &wbpos);
        NuVecNorm(&wbveldir, &wbveldir);
        NuVecScale(&wbveldir, &wbveldir, WBBULLETSPEED);
        NuMtxScale(&mat, &wbscale);
        if (AddGliderBullet(&mat, &wbpos, &wbveldir, 0) == 0) {
            goto end;
        }
        glider->FireTimer = WBBULLETFIRERATE * 0.02f;
        GameSfx(0x55, (struct nuvec_s *)&glider->Cre->mtxLOCATOR[1][0]._30);
        NewBuzz(&player->rumble, 3);
        break;
    case 0x12:
    case 0x24:
        if (glider->TargetOn == 0) {
            glider->TargetTimer = 0.0f;
            glider->TargetedTime = 0.0f;
        } else {
            glider->TargetedTime += 0.02f;
        }
        if (glider->TargetOn != 0) {
            if (StillLockedOnTarget(glider->MovingTargetPoint) == 0) {
                MyGameSfx(0x4F, 0, 0x3FFF);
                NewBuzz(&player->rumble, TORPTARGFAILBLEEPTIME);
                glider->TargetOn = 0;
            }
        }
        if (ProcessTimer(&glider->TargetTimer) != 0) {
            if (glider->TargetOn != 0) {
                if (firebutton == 0) {
                    glider->TargetOn = 0;
                    if (Level == 0x24) {
                        NuMtxTranslate(&mat, &glider->Position);
                        NuVecMtxTransform(&bombpos, SetNuVecPntr(-2.0f, 0.0f, 0.0f), &mat);
                        AddGliderBomb(&bombpos, &glider->Velocity, glider->AngleY, 0,
                                      glider->MovingTargetPoint, glider->MovingTargetVel,
                                      glider->TargetMoving);
                        NuVecMtxTransform(&bombpos, SetNuVecPntr(2.0f, 0.0f, 0.0f), &mat);
                        AddGliderBomb(&bombpos, &glider->Velocity, glider->AngleY, 0,
                                      glider->MovingTargetPoint, glider->MovingTargetVel,
                                      glider->TargetMoving);
                        glider->FireTimer = TorpedoTime;
                        NewBuzz(&player->rumble, 6);
                        NewRumble(&player->rumble, 0xB4);
                    } else {
                        if (AddGliderBomb((struct nuvec_s *)&glider->Cre->mtxLOCATOR[1][0]._30,
                                          &glider->Velocity, glider->AngleY, 0,
                                          glider->MovingTargetPoint, glider->MovingTargetVel,
                                          glider->TargetMoving) == 0) {
                            goto end;
                        }
                        glider->FireTimer = TorpedoTime;
                        MyGameSfx(0xCE, &glider->Position, 0x3FFF);
                        NewBuzz(&player->rumble, 6);
                        NewRumble(&player->rumble, 0xB4);
                    }
                } else if (ProcessTimer(&FireBleepTimer) != 0) {
                    FireBleepTimer = TARGETBLEEPTIME;
                    if (Level == 0x12) {
                        MyGameSfx(0xD2, 0, 0x1800);
                        NewBuzz(&player->rumble, TORPTARGBLEEPTIME);
                    }
                }
            } else if (firebutton != 0
                       && PickGliderTarget(&glider->MovingTargetPoint, &glider->MovingTargetVel,
                                           &glider->TargetMoving) != 0) {
                if (Level == 0x12) {
                    MyGameSfx(0xD2, 0, 0x1800);
                }
                NewBuzz(&player->rumble, TORPTARGAQUIREBLEEPTIME);
                glider->TargetOn = 1;
                glider->TargetTimer = Level_TargetTime;
            }
        } else {
            if (firebutton == 0) {
                MyGameSfx(0x4F, 0, 0x3FFF);
                glider->TargetOn = 0;
            }
            FireBleepTimer = TARGETBLEEPTIME;
        }
        break;
    }

end:
    LastFireButton = firebutton;
}


extern f32 WBIntroDestTiltX;
extern f32 WBIntroDestTiltZ;
extern f32 CRASHTEROIDSSeekTiltXTimeZero;
extern f32 CRASHTEROIDSSeekTiltXTimeNonZero;
extern f32 TORNSeekTiltXTimeZero;
extern f32 TORNSeekTiltXTimeNonZero;
extern f32 SeekTiltZOveride;
extern f32 SeekTiltZZero;
extern f32 SeekTiltZNonZero;
extern f32 fsign(f32 x);

void ControlGlider(struct gliderflight_s *glider, struct nupad_s *pad)
{
    f32 anx;
    f32 anz;
    f32 relx;
    f32 relz;
    f32 radius;
    struct nuvec_s ctrl;
    struct nuvec_s input;
    f32 mag;
    f32 oldtiltz;
    f32 angle;
    f32 tilt;
    f32 delta;
    f32 mintiltx;
    f32 maxtiltx;
    f32 mindest;
    f32 maxdest;
    double absval;
    s32 turn;
    s32 ang;
    struct nuvec_s *pos;

    pos = &glider->Position;
    relz = 0.0f;
    relx = relz;
    radius = relz;

    if (Level != 0x18) {
        ProcessTimer(&glider->PullUpTimer);
        relx = glider->Position.x - Level_GliderCentreX;
        relz = pos->z - Level_GliderCentreZ;
        radius = NuFsqrt(relx * relx + relz * relz);
        if (Level != 0x12 && glider->InTornadoTime == 0.0f && glider->AutoPilot == 0
            && radius < Level_GliderRadius && glider->TerminalDive == 0
            && glider->ForceTurn == 0 && FlyingLevelExtro == 0) {
            if ((pad->paddata & 0x2) && (u32)glider->BarrelRoll < 2) {
                glider->BarrelRoll = 3;
                GameSfx(0x5C, pos);
            } else if ((pad->paddata & 0x1) && (u32)(glider->BarrelRoll + 1) < 2) {
                glider->BarrelRoll = -3;
                GameSfx(0x5C, &glider->Position);
            }
        }
    }

    anx = (f32)(pad->rdata.l_alg_x - 0x7F);
    anz = (f32)(pad->rdata.l_alg_y - 0x7F);
    if (pad->paddata & PAD_DOWN) {
        anz = 127.0f;
    }
    if (pad->paddata & PAD_UP) {
        anz = -127.0f;
    }
    if (pad->paddata & PAD_RIGHT) {
        anx = 127.0f;
    }
    if (pad->paddata & PAD_LEFT) {
        anx = -127.0f;
    }
    if (FlyingLevelExtro != 0 || glider->AutoPilot != 0 || glider->InTornadoTime > 0.0f
        || (Level == 0x18 && WBIntroOn != 0)) {
        anz = 0.0f;
        anx = anz;
    }
    ctrl.x = anx;
    ctrl.y = 0.0f;
    ctrl.z = -anz;
    mag = NuVecMag(&ctrl);
    if (mag >= 32.0f) {
        ang = NuAtan2D(ctrl.x, ctrl.z) & 0xFFFF;
        tilt = (mag - 32.0f) * 1.02f / 96.0f;
        absval = tilt;
        if (absval < 0.0) {
            absval = -absval;
        }
        if (absval >= 1.0) {
            input.z = fsign(tilt);
        } else {
            input.z = tilt;
        }
        input.x = input.y = 0.0f;
        NuVecRotateY(&input, &input, ang);
        glider->InputX = input.x;
        glider->InputZ = input.z;
    } else {
        glider->InputX = 0.0f;
        glider->InputZ = 0.0f;
    }

    if (glider->TerminalDive != 0) {
        if (glider->TerminalDir == 0) {
            glider->TerminalDir = ((s32)(frand() * 16.0) & 2) - 1;
        }
        glider->InputZ = 0.8f;
        glider->InputX = (f32)glider->TerminalDir * 0.5f;
    }

    if (Level == 0x18 && WBIntroOn != 0) {
        glider->DestTiltX = WBIntroDestTiltX;
    } else {
        glider->DestTiltX = -glider->InputZ * 40.0f;
    }

    if (glider->TerminalDive == 0 && WBIntroOn == 0) {
        mintiltx = -100.0f;
        maxtiltx = 100.0f;
        if (glider->PullUpTimer != 0.0f) {
            mintiltx = glider->PullUpTimer * 40.0f;
        }
        if (glider->Position.y < Level_GliderFloor + 1.3f) {
            mintiltx = (Level_GliderFloor + 1.3f - glider->Position.y) * 10.0f;
        }
        if (glider->Position.y > Level_GliderCurrentCeiling - 1.3f) {
            maxtiltx = (glider->Position.y - (Level_GliderCurrentCeiling - 1.3f)) * -10.0f;
        } else {
            Level_GliderCurrentCeiling = Level_GliderCeiling;
        }
        if (glider->DestTiltX > maxtiltx) {
            glider->DestTiltX = maxtiltx;
        }
        if (glider->DestTiltX < mintiltx) {
            glider->DestTiltX = mintiltx;
        }
    }

    if (Level == 0x1A) {
        if (glider->DestTiltX == 0.0f) {
            SeekAngHalfLife360f(&glider->TiltX, glider->DestTiltX, CRASHTEROIDSSeekTiltXTimeZero, 0.02f);
        } else {
            SeekAngHalfLife360f(&glider->TiltX, glider->DestTiltX, CRASHTEROIDSSeekTiltXTimeNonZero, 0.02f);
        }
    } else if (Level == 0xD) {
        if (glider->DestTiltX == 0.0f) {
            SeekAngHalfLife360f(&glider->TiltX, glider->DestTiltX, TORNSeekTiltXTimeZero, 0.02f);
        } else {
            SeekAngHalfLife360f(&glider->TiltX, glider->DestTiltX, TORNSeekTiltXTimeNonZero, 0.02f);
        }
    } else {
        SeekAngHalfLife360f(&glider->TiltX, glider->DestTiltX, 0.2f, 0.02f);
    }

    if (glider->BarrelRoll != 0) {
        oldtiltz = Rationalise360f(glider->TiltZ);
        if (glider->BarrelRoll == 2 && glider->TiltZ > -45.0f) {
            glider->BarrelRoll = 1;
        }
        if (glider->BarrelRoll == -2 && glider->TiltZ < 45.0f) {
            glider->BarrelRoll = -1;
        }
        if ((u32)(glider->BarrelRoll + 1) > 2 || glider->TiltZ > 30.0f || glider->TiltZ < -30.0f) {
            SeekHalfLife(&glider->BarrelDelta, 7.2f, 0.05f, 0.02f);
        } else if (glider->TiltZ > 1.0f || glider->TiltZ < -1.0f) {
            absval = glider->TiltZ * 20.0f * 0.02f * 0.5f;
            if (absval < 0.0) {
                absval = -absval;
            }
            glider->BarrelDelta = absval;
        } else {
            glider->BarrelDelta = 1.0f;
        }
        if (glider->BarrelRoll > 0) {
            angle = Rationalise360f(oldtiltz + glider->BarrelDelta);
            glider->TiltZ = angle;
            if (oldtiltz >= 0.0f && angle < 0.0f) {
                glider->BarrelRoll = 2;
            }
        } else {
            angle = Rationalise360f(oldtiltz - glider->BarrelDelta);
            glider->TiltZ = angle;
            if (oldtiltz <= 0.0f && angle > 0.0f) {
                glider->BarrelRoll = -2;
            }
        }
        if ((glider->BarrelRoll == 1 && oldtiltz < 0.0f && glider->TiltZ >= 0.0f)
            || (glider->BarrelRoll == -1 && oldtiltz > 0.0f && glider->TiltZ <= 0.0f)) {
            glider->BarrelRoll = 0;
            glider->TiltZ = 0.0f;
        }
        if (glider->BarrelRoll != 0) {
            return;
        }
    }

    turn = 0;
    if (glider->OverideTiltZ == 0.0f) {
        glider->DestTiltZ = glider->InputX * 60.0f;
    }
    if (Level == 0x18 && WBIntroOn != 0) {
        glider->DestTiltZ = WBIntroDestTiltZ;
    }
    if (Level == 0x18) {
        delta = Rationalise360f(glider->RailAngle - glider->AngleY);
        absval = glider->Position.x;
        if (absval < 0.0) {
            absval = -absval;
        }
        if (absval > 10.0) {
            if (glider->Position.x > 10.0f && delta > 0.0f) {
                turn = 1;
            } else if (glider->Position.x < -10.0f && delta < 0.0f) {
                turn = 1;
            }
        }
    } else if (radius > Level_GliderRadius || glider->ForceTurn != 0) {
        turn = 1;
        angle = (f32)NuAtan2D(relx, relz) / 182.04445f;
        delta = Rationalise360f(angle - glider->AngleY);
    } else {
        delta = 0.0f;
    }

    if (turn != 0) {
        mindest = -1000.0f;
        maxdest = 1000.0f;
        absval = delta;
        if (absval < 0.0) {
            absval = -absval;
        }
        if (absval < 2.0) {
            glider->ForceTurn = 0;
        }
        if (delta > 90.0f) {
            mindest = 60.0f;
        } else if (delta > 0.0f) {
            mindest = delta / 1.5f;
        } else if (delta < -90.0f) {
            maxdest = -60.0f;
        } else {
            maxdest = delta / 1.5f;
        }
        if (glider->DestTiltZ > maxdest) {
            glider->DestTiltZ = maxdest;
        }
        if (glider->DestTiltZ < mindest) {
            glider->DestTiltZ = mindest;
        }
    }

    if (glider->OverideTiltZ != 0.0f) {
        SeekAngHalfLife360f(&glider->TiltZ, glider->DestTiltZ, SeekTiltZOveride, 0.02f);
    } else if (glider->DestTiltZ == 0.0f) {
        SeekAngHalfLife360f(&glider->TiltZ, glider->DestTiltZ, SeekTiltZZero, 0.02f);
    } else {
        SeekAngHalfLife360f(&glider->TiltZ, glider->DestTiltZ, SeekTiltZNonZero, 0.02f);
    }
}


extern s32 BEENHITTORNBUZZTIME;
extern s32 BEENHITTORNRUMTIME;
extern f32 FIXVELFADETIME;
extern f32 WBFRICX;
extern f32 WBFRICY;

void ProcessGliderMovement(struct gliderflight_s *glider, f32 dt)
{
    struct numtx_s in;
    struct numtx_s out;
    struct nuvec_s resolved;
    struct nuvec_s temp;
    struct nuvec_s keepvel;
    struct nuvec_s target;
    f32 scale;
    f32 dest;

    ProcessTimer(&glider->InTornadoTime);

    if (glider->InTornado != 0 && glider->LastInTornado == 0) {
        f32 spin;

        spin = frand() * 360.0f;
        keepvel.x = 0.0f;
        keepvel.y = frand() * 25.0f - 12.5f;
        keepvel.z = frand() * 15.0f + 45.0f;
        temp = keepvel;
        if (Level != 0x1A) {
            NuVecRotateY(&temp, &temp, (s32)(spin * 182.04445f));
            NuVecScale(&temp, &temp, glider->InTornadoScale);
            glider->Velocity = temp;
        }
        glider->TornadoSpin = (frand() * 1200.0f - 600.0f) * glider->InTornadoScale;
        GameSfx(0x5C, 0);
        if (Level != 0x1A && InvincibilityCHEAT == 0 && VehicleLevelImmune == 0) {
            if (glider->InTornadoTime <= 0.0f) {
                glider->HitPoints -= glider->InTornadoScale * 12.5f;
            }
            if (glider->HitPoints < 0) {
                glider->HitPoints = 0;
            }
        }
        NewBuzz(&player->rumble, BEENHITTORNBUZZTIME);
        NewRumble(&player->rumble, BEENHITTORNRUMTIME);
        glider->InTornadoTime = 2.0f;
        glider->CamTornRecoverTimer = 1.0f;
    }

    if (glider->InTornadoTime != 0.0f) {
        glider->AngleY += glider->TornadoSpin * 0.02f * glider->InTornadoTime;
    } else if (glider->BarrelRoll == 0) {
        if (Level == 0x18) {
            glider->AngleY = 0.0f;
        } else {
            glider->AngleY += glider->TiltZ * 2.0f * dt;
        }
    }

    if (Level == 0x1A) {
        NuMtxSetRotationY(&in, (s32)(-glider->AngleY * 182.04445f));
        NuMtxRotateX(&in, (s32)(-glider->TiltX * 2.0f * 182.04445f));
        NuMtxRotateZ(&in, (s32)(-glider->TiltZ * 182.04445f));
        NuMtxSetRotationZ(&out, (s32)(glider->TiltZ * 182.04445f));
        NuMtxRotateX(&out, (s32)(glider->TiltX * 2.0f * 182.04445f));
        NuMtxRotateY(&out, (s32)(glider->AngleY * 182.04445f));
    } else {
        NuMtxSetRotationY(&in, (s32)(-glider->AngleY * 182.04445f));
        NuMtxRotateX(&in, (s32)(-glider->TiltX * 182.04445f));
        NuMtxRotateZ(&in, (s32)(-glider->TiltZ * 182.04445f));
        NuMtxSetRotationZ(&out, (s32)(glider->TiltZ * 182.04445f));
        NuMtxRotateX(&out, (s32)(glider->TiltX * 182.04445f));
        NuMtxRotateY(&out, (s32)(glider->AngleY * 182.04445f));
    }

    ProcessTimer(&glider->FixVelTimer);
    keepvel = glider->Velocity;
    if (glider->FixVelTimer > FIXVELFADETIME) {
        scale = 1.0f;
    } else {
        scale = glider->FixVelTimer / FIXVELFADETIME;
    }
    NuVecMtxRotate(&resolved, &glider->Velocity, &in);
    {
        f32 pitchscale;

        pitchscale = 1.0f - glider->TiltX * 0.01f;
        if (glider->BarrelRoll != 0) {
            glider->TargetSpeed = -Level_GliderSpeed * 0.125f;
        } else {
            glider->TargetSpeed = -Level_GliderSpeed;
        }
        SeekHalfLife(&glider->Speed, glider->TargetSpeed, 1.0f, 0.02f);
        target.z = glider->Speed * pitchscale;
        target.x = 0.0f;
        target.y = 0.0f;
    }
    if (Level == 0x18) {
        SeekHalfLife(&resolved.x, target.x, 0.2f, dt);
        SeekHalfLife(&resolved.y, target.y, 0.1f, dt);
        SeekHalfLife(&resolved.z, target.z, 0.5f, dt);
    } else {
        SeekHalfLife(&resolved.x, target.x, WBFRICX, dt);
        SeekHalfLife(&resolved.y, target.y, WBFRICY, dt);
        SeekHalfLife(&resolved.z, target.z, 0.5f, dt);
    }
    NuVecMtxRotate(&glider->Velocity, &resolved, &out);
    NuVecScale(&glider->Velocity, &glider->Velocity, 1.0f - scale);
    NuVecScaleAccum(&glider->Velocity, &keepvel, scale);

    if (glider->AutoPilot == 0) {
        glider->Position.x += glider->Velocity.x * dt;
        glider->Position.y += glider->Velocity.y * dt;
        glider->Position.z += glider->Velocity.z * dt;
    }

    if (glider->BarrelRoll > 1) {
        dest = -10.0f;
    } else if (glider->BarrelRoll < -1) {
        dest = 10.0f;
    } else {
        dest = 0.0f;
    }
    if (dest == 0.0f) {
        SeekHalfLife(&glider->BarrelSpeedX, dest, 0.5f, dt);
    } else {
        SeekHalfLife(&glider->BarrelSpeedX, dest, 0.3f, dt);
    }
    target = SetNuVec(glider->BarrelSpeedX, 0.0f, 0.0f);
    NuVecRotateY(&target, &target, (s32)(glider->AngleY * 182.04445f));
    glider->Position.x += target.x * dt;
    glider->Position.y += target.y * dt;
    glider->Position.z += target.z * dt;
}
