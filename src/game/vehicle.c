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
#include "nu_port.h"

#include "numath/numtx.h"
#include "numath/nuquat.h"
#include "numath/nutrig.h"
#include "numath/nuvec.h"
#include "nu3d/nugscn.h"
#include "game/gametypes.h"
#include "gamelib/terrain.h"
#include "game/creature.h"
#include "nups2/ps2ctl.h"
#include "nucore/nuerror.h"
#include "game/vehicle.h"

#ifdef NW_HOST
/* gnu89 inline globals stay out of module headers. */
struct obj_s;
s32 KillPlayer(struct obj_s *obj, s32 anim);
#endif

extern s32 Level;
extern struct numtx_s mTEMP;

/* paddata d-pad bits (libpad SCE_PADLup/right/down/left). */
#define PAD_UP    0x1000
#define PAD_RIGHT 0x2000
#define PAD_DOWN  0x4000
#define PAD_LEFT  0x8000

struct zoffastart_s {
    f32 x, y, z;
    f32 angle;
};

struct nuvec_s ExhaustPosA = {0.325f, 0.11f, 0.66f};
struct nuvec_s ExhaustPosB = {-0.325f, 0.11f, 0.66f};
struct nuvec_s SpaceFirePosA = {0.75f, -0.1f, 0.0f};
struct nuvec_s SpaceFirePosB = {-0.75f, -0.1f, 0.0f};
static struct zoffastart_s ZoffaStartPoints[16] = {
    {0.0f, 28.0f, 100.0f, 0.0f},
    {-100.0f, 24.0f, 0.0f, 90.0f},
    {0.0f, 32.0f, -100.0f, 180.0f},
    {100.0f, 34.0f, 0.0f, 270.0f},
    {-25.0f, 26.0f, 75.0f, 0.0f},
    {-25.0f, 22.0f, 25.0f, 90.0f},
    {-75.0f, 18.0f, 25.0f, 180.0f},
    {25.0f, 38.0f, 75.0f, 0.0f},
    {25.0f, 14.0f, 25.0f, 90.0f},
    {75.0f, 22.0f, 25.0f, 180.0f},
    {-25.0f, 18.0f, -75.0f, 0.0f},
    {-25.0f, 24.0f, -25.0f, 90.0f},
    {-75.0f, 22.0f, -25.0f, 180.0f},
    {25.0f, 28.0f, -75.0f, 0.0f},
    {25.0f, 34.0f, -25.0f, 90.0f},
    {75.0f, 16.0f, -25.0f, 180.0f}
};
struct nuvec_s TeleportPos[4] = {
    {-40.0f, 5.0f, 60.0f},
    {40.0f, 5.0f, 60.0f},
    {-40.0f, -5.0f, 60.0f},
    {40.0f, -5.0f, 60.0f}
};
struct nuvec_s TeleportVel[4] = {
    {14.0f, 0.0f, 0.0f},
    {-14.0f, 0.0f, 0.0f},
    {14.0f, 0.0f, 0.0f},
    {-14.0f, 0.0f, 0.0f}
};
struct nuvec_s ZoffaFirePosA = {0.3f, -0.4f, -0.75f};
struct nuvec_s ZoffaFirePosB = {-0.3f, -0.4f, -0.75f};
struct nuvec_s SPACETRAN = {0.0f, -2.5f, 0.0f};

extern s32 MyDrawModelNew(struct MYDRAW *Draw, struct numtx_s *mC,
                          struct numtx_s *loc_mtx);

/* Defined here: retail emits its Scale between SPACETRAN and ZepplinGliderOffset. */
inline void DrawPlane(struct plane_s *plane)
{
    static struct nuvec_s Scale = {1.0f, 1.0f, 1.0f};

    if (plane->active != 0) {
        NuMtxSetScale(&mTEMP, &Scale);
        NuMtxTranslate(&mTEMP, &plane->pos);
        MyDrawModelNew(&plane->draw, &mTEMP, 0);
    }
}

struct nuvec_s ZepplinGliderOffset = {0.0f, 0.0f, 0.0f};
struct nuvec_s FarmCameraStart = {50.0f, 75.0f, -140.0f};
struct nuvec_s FarmZepplinStart = {-50.0f, 50.0f, -44.0f};
struct nuvec_s WBCamI = {0.0f, 0.0f, 5.0f};
struct nuvec_s ExtroIdealPos = {0.0f, 0.0f, 7.0f};
struct nuvec_s TORNADOCAMIDEAL = {0.0f, 1.0f, 2.0f};
struct nuvec_s GliderCollPoints[5] = {
    {1.6f, 0.3f, -0.3f},
    {0.0f, 0.3f, -0.9f},
    {-1.6f, 0.3f, -0.3f},
    {-0.5f, 0.8f, 1.8f},
    {0.5f, 0.8f, 1.8f}
};

f32 WBIntroTweenTime = 1.0f;
s32 GUNBOATVOLUME = 32767;
s32 BATTLESHIPAVOLUME = 32767;
s32 BATTLESHIPBVOLUME = 32767;
f32 SeekGliderCamTime = 0.1f;
f32 SeekTiltZOveride = 0.05f;
f32 SeekTiltZZero = 0.05f;
f32 SeekTiltZNonZero = 0.1f;
f32 RumbleCamVal = 0.0f;
f32 BumpSpeed = 0.5f;
f32 BumpAmount = 0.1f;
s32 ChrisInTheHouse = 0;
s32 JamesInTheHouse = 0;
s32 CRASHTEROIDSENGINEVOL = 4096;
s32 CRASHTEROIDSAMBIENTVOL = 2048;
s32 CRASHTEROIDSZOFFAVOL = 8704;
s32 CRASHTEROIDSSATELLITEVOL = 7168;
struct creature_s *DebugCreature = 0;
s32 jonemit = 0;
f32 FIXVELFADETIME = 2.0f;
f32 FIXVELTIME = 2.0f;
f32 FIXVELBUMPAMOUNT = 0.5f;
f32 WBBULLETSPEED = 200.0f;
f32 WBBULLETFIRERATE = 6.0f;
f32 TornFireY = -6.0f;
s32 TORPTARGBLEEPTIME = 2;
s32 TORPTARGFAILBLEEPTIME = 12;
s32 TORPTARGAQUIREBLEEPTIME = 3;
s32 BEENHITBUZZTIME = 3;
s32 BEENHITBIGBUZZTIME = 12;
s32 BEENHITBIGRUMTIME = 255;
f32 YAdj = -4.0f;
f32 XAdj = -15.0f;
f32 TorpedoTime = 1.0f;
f32 TARGETBLEEPTIME = 0.1f;
s32 TORPRUMBLEVAL = 127;
s32 TORPBUZZVAL = 4;
f32 CRASHTEROIDSSeekTiltXTimeZero = 1.0f;
f32 CRASHTEROIDSSeekTiltXTimeNonZero = 0.5f;
f32 TORNSeekTiltXTimeZero = 1.0f;
f32 TORNSeekTiltXTimeNonZero = 0.5f;
f32 WBSeekTiltZTimeZero = 1.0f;
f32 WBSeekTiltZTimeNonZero = 0.5f;
f32 WBSeekTiltXTimeZero = 1.0f;
f32 WBSeekTiltXTimeNonZero = 0.5f;
f32 WeatherBossTargetAppearTime = 1.0f;
f32 WBTARGSPEED = 0.015f;
f32 WBTARGSEEKSPEEDZERO = 0.5f;
f32 WBTARGSEEKSPEEDNONZERO = 0.15f;
f32 WBTARGLIMX = 4.0f;
f32 WBTARGLIMY = 3.0f;
f32 WBTARGSCALE = 0.8f;
f32 WBLIMX = 6.0f;
f32 WBMINY = 37.0f;
f32 WBMAXY = 42.0f;
f32 WBTILTXSCALE = 0.2f;
f32 WBTILTZSCALE = -0.1f;
f32 WBCamDist = 8.0f;
f32 FIXX = 0.0f;
f32 FIXY = 40.0f;
f32 WBTARGHOMESPEED = 5.0f;
f32 WBIntroDestTiltX = 0.0f;
f32 WBIntroDestTiltZ = 0.0f;
f32 WBITILTXSCALE = 1.5f;
f32 WBITILTZSCALE = -0.8f;
s32 BEENHITTORNRUMTIME = 255;
s32 BEENHITTORNBUZZTIME = 12;
f32 WBClimbRate = 0.25f;
f32 WBFRICX = 0.0f;
f32 WBFRICY = 0.0f;
f32 WBBARRELSEEKSPEEDZERO = 0.1f;
f32 WBBARRELSEEKSPEEDNONZERO = 0.1f;
f32 WBBARRELSPEED = 8.0f;
f32 GLIDERHITSOUNDFREQUENCY = 6.0f;
s32 GLIDERHITSOUNDOTHERID = 66;
s32 SINKENGINEVOL = 4096;
s32 MOSENGPITCH = 2048;
f32 MOSENGPITCHSLOPE = -6.5f;
s32 OIDENGPITCH = 512;
f32 OIDENGPITCHSLOPE = -6.5f;
s32 OIDZOFPITCH = 512;
f32 OIDZOFPITCHSLOPE = -6.5f;
s32 OIDENGRUMBLE = 80;
f32 OIDENGRUMBLESLOPE = 4.0f;
s32 GLIDENGRUMBLE = 80;
f32 GLIDENGRUMBLESLOPE = 4.0f;
s32 MOSENGRUMBLE = 80;
f32 MOSENGRUMBLESLOPE = 4.0f;
s32 GLIDENGPITCH = 1024;
f32 GLIDENGPITCHSLOPE = -8.5f;
s32 GLIDENGINEVOL = 8191;
f32 GLIDRUMTIM = 0.5f;
s32 GLIDRUMVOL = 120;
s32 FarmTestIndx = 0;
s32 FarmFrame = 0;
s32 NumZoffasFiring = 0;
s32 LastNumZoffasFiring = 0;
f32 ZoffaAlter = 3.0f;
s32 UFOEXPLODEID = 208;
f32 HovaRad = 2.0f;
f32 HovaY = 3.8f;
f32 SatelliteRad = 2.0f;
f32 SatelliteY = 3.8f;
f32 PropRad = 8.0f;
f32 PropY = 0.1f;
f32 PropYOff = 4.0f;
f32 BigRad = 18.0f;
s32 jonfirst = 0;
s32 ACTIVEBLIMPCOUNT = 6;
f32 jtemp1 = -25.0f;

static s32 FarmResetTimer;
static s32 ZoffaCollisionCounter;
static s32 ZoffaTeleportIndx;
static struct zoffa_s *CurrentAggressor;
static s32 LevelResetTimer;
static struct nuvec_s GliderAutoPosition;
static struct nuvec_s WeatherStartPos;

extern s8 D_0056236E[];

extern s32 AtlasFrame;
extern s32 tttt;
extern s8 D_0056238B[];   /* per-level atlasphere model index (far from $gp) */

extern s32 temp_xzmomset;
extern float D_00560D44[]; /* ball vertical lift offset (far from $gp) */

void NuHGobjRndr(struct NUHGOBJ_s *hobj, struct numtx_s *m, s32 nlayers,
                 short *layer);

struct nuvec_s *SetNuVecPntr(f32 x, f32 y, f32 z);
f32 frand(void);
s32 ProcessTimer(f32 *timer);

void DrawGlider(struct creature_s *c)
{
    struct GLIDERSTRUCT *g;
    float lift;
    s32 model;

    g = (struct GLIDERSTRUCT *)c->Buggy;
    if (Level == 0xD) {
        lift = 0.0f;
    } else if (Level == 0x1A) {
        lift = 0.0f;
    } else {
        lift = 15.0f;
    }

    if (g->AutoPilot && (Level == 0xD)) {
        NuMtxSetRotationX(&mTEMP, lift * 182.04445f);
        NuMtxRotateY(&mTEMP, 0);
        NuMtxTranslate(&mTEMP, &GliderAutoPosition);
    } else if (Level == 0x1A) {
        NuMtxSetRotationZ(&mTEMP, g->TiltZ * 182.04445f);
        NuMtxRotateX(&mTEMP, (g->TiltX + g->TiltX) * 182.04445f);
        NuMtxRotateY(&mTEMP, g->AngleY * 182.04445f);
        NuMtxTranslate(&mTEMP, &g->Position);
    } else {
        NuMtxSetRotationZ(&mTEMP, g->TiltZ * 182.04445f);
        NuMtxRotateX(&mTEMP, (g->TiltX + lift) * 182.04445f);
        NuMtxRotateY(&mTEMP, g->AngleY * 182.04445f);
        NuMtxTranslate(&mTEMP, &g->Position);
    }

    switch (Level) {
    case 0x1A:
        model = CRemap[0x81];
        break;
    case 0x12:
        model = CRemap[0x8B];
        break;
    default:
        model = CRemap[0x36];
        break;
    }
    if (model != -1) {
        DrawCharacterModel(&CModel[model], &c->obj.anim, &mTEMP, 0, 1, 0,
                           c->mtxLOCATOR[1], c->momLOCATOR[1], &c->obj);
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

void ObjectToAtlas(struct obj_s *obj, struct creature_s *c)
{
    struct NEWBUGGY *b = c->Buggy;

    b->ball_pos.x = obj->pos.x;
    b->ball_pos.y = obj->pos.y + CData[0x53].radius;
    b->ball_pos.z = obj->pos.z;
    if (temp_xzmomset != 0) {
        b->ball_vel.x = obj->mom.x / NW_HZ(0.02f, 0.016666668f);
        b->ball_vel.z = obj->mom.z / NW_HZ(0.02f, 0.016666668f);
    }
    if (obj->boing != 0) {
        obj->mom.y = 3.3333335f;
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
extern struct GLIDERSTRUCT PlayerGlider;
extern struct nuvec_s v000;

extern void ProcessFireFlyIntro(void);
extern void InitWeatherBoss_a(void);
struct weatherboss_s;
extern void ProcessWeatherBoss_a(struct weatherboss_s *wb);
extern void DrawWeatherBoss_a(struct weatherboss_s *wb);
extern struct jeeprock_s *AddRockVel(struct nuvec_s *pos, struct nuvec_s *vel, f32 radius, s32 owner);
extern RailPosition *best_cRPos;

/* The earth boss is an atlasphere, like PlayerAtlas (far). */
extern struct NEWBUGGY EarthBoss;

extern struct weatherboss_s WeatherBoss;

extern struct vehmask_s VehicleMask[2];

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
extern void ProcessHovaBlimp(struct hovablimp_s *hova);
extern f32 Rationalise360f(f32 a);

/* Spin the tornado and keep it under its blimp, or retire it with it. */
static inline void ProcessTornado(struct tornado_s *tornado, struct hovablimp_s *hova)
{
    tornado->yang += tornado->yanginc * NW_HZ(0.02f, 0.016666668f);
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
            JeepRock[i].flametimer = 10.0f;
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
        if (JeepRock[i].exists != 0 && JeepRock[i].flametimer > 0.0f &&
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
        if (JeepRock[i].exists != 0 && JeepRock[i].flametimer > 0.0f &&
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
    struct GLIDERSTRUCT *g = (struct GLIDERSTRUCT *)player->Buggy;

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
    WeatherStartPos = c->obj.pos;
    PlayerGlider.Position = WeatherStartPos;
}

s32 GetRumblePlayerHealthPercentage(void)
{
    return D_005B7918[0];
}

s32 GetGliderHealthPercentage(struct creature_s *c)
{
    struct GLIDERSTRUCT *g = (struct GLIDERSTRUCT *)c->Buggy;

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
        mask->tweeninc = tweeninc * NW_HZ(0.02f, 0.016666668f);
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
    mask->anginc[0] = anginc * NW_HZ(0.02f, 0.016666668f);
    mask->rad[0] = rad;
    mask->scale[0] = scale;
    mask->tiltx[0] = tiltx;
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

inline void InitWeatherBoss(void)
{
    InitWeatherBoss_a();
}

extern struct MYDRAW IconMainDraw;
extern s32 BazookaIconOn;
extern struct nuvec_s BazookaTokenCurrentPos;

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

inline void DrawFarmLevelExtra(void)
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
        t = WeatherBoss.bonesdraw.Anim.anim_time /
            WeatherBoss.bonesdraw.Model->anmdata[WeatherBoss.bonesdraw.Anim.action]->time;
        NuVecScale(&pos, &BazookaTokenPos, 1.0f - t);
        NuVecScaleAccum(&pos, &PlayerGlider.Position, t);
        pos.y += t * 0.75f;
        BazookaTokenCurrentPos = pos;
        MyAnimateModelNew(&IconMainDraw, NW_HZ(0.59999996f, 0.5f));
    }
}


inline void DrawBazookaToken(void)
{
    if (BazookaIconOn) {
        NuMtxSetTranslation(&mTEMP, &BazookaTokenCurrentPos);
        MyDrawModelNew(&IconMainDraw, &mTEMP, 0);
    }
}

inline s32 WeatherBossNextAction(void)
{
    s32 action;
    s32 next;
    s32 tries;

    if (WeatherBoss.action == 0) {
        next = 0;
        tries = 1;
        action = WeatherBoss.lastnonseekaction;
        do {
            action++;
            if (action > 4) {
                action = 1;
            }
            if (WeatherBoss.hitpoints[action - 1] > 0) {
                next = action;
            }
        } while (tries++ < 4 && next == 0);
        return next;
    }
    WeatherBoss.lastnonseekaction = WeatherBoss.action;
    return 0;
}

extern s32 GetCurrentWeatherBossObjectives(void);
extern s32 WBIntroOn;
extern struct nuvec_s *FireWBBolt(struct nuvec_s *pos, struct nuvec_s *vel, s32 type, f32 life,
                                  s32 owner);
extern void UnleashLighteningHail(struct nuvec_s *pos, s32 attnum);
extern void MyGameSfxLoop(s32 id, struct nuvec_s *pos, s32 vol);
extern void MyResetAnimPacket(struct MYDRAW *draw, s32 action);
extern void MyChangeAnim(struct MYDRAW *draw, s32 action);
extern s32 CollideGliderBullets(struct nuvec_s *pos, f32 radius, s32 enemy, f32 yscale,
                                s32 a, s32 b);
extern void SeekAngHalfLife360f(f32 *dest, f32 target, f32 halflife, f32 dt);
extern void SeekHalfLifeLim(f32 *dest, f32 target, f32 limit, f32 halflife, f32 dt);
extern f32 frandPN(void);
extern struct numtx_s WeatherBossCamMtx;
extern struct game_s Game;
extern s32 Hub;
extern s32 ChrisBigBossDead;
extern s32 FlyingLevelVictoryDance;
extern s32 WeatherBossDead;
extern f32 Level_GliderSpeed;
extern struct nuvec_s ElectricalPosition;
extern f32 WeatherBossSkeletonGlitchTimer;
extern f32 WeatherBossSkeletonTimer;
extern f32 WBSKELTIME;
extern f32 WBSKELTIMERAND;
extern struct nuvec_s WBMASKONHIGH;
extern struct nuvec_s WBMASKLEFT;
extern struct nuvec_s WBMASKRIGHT;
extern struct nuvec_s WBMASKEYES;
extern struct nuvec_s WBMASKCHEST;
extern struct nuvec_s WBAKUMASKONHIGH;
extern f32 WBMASKLEFTRAD;
extern f32 WBMASKRIGHTRAD;
extern f32 WBMASKEYESRAD;
extern f32 WBMASKCHESTRAD;
extern f32 WBLOLOSCALE;
extern f32 WBLOLOTILTX;
extern f32 WBAKUAKUSCALE;
extern f32 WBLeftStartFrame;
extern f32 WBLeftStopFrame;
extern f32 WBLeftStopTurnFrame;
extern f32 WBANGSCALE;
extern f32 WBANGOFF;
extern f32 WBANGSCALE2;
extern f32 WBANGOFF2;
extern f32 WBANGSCALE3;
extern struct nuvec_s WBLeftFirePos;
extern f32 WBSNOWBALLSPEED;
extern f32 WBSNOWCONESPEED;
extern f32 EYESTOPTIME;
extern f32 EYEBOLTFIRESCALEX;
extern f32 EYEBOLTFIRESCALEY;
extern f32 EYEBOLTFIREY;
extern f32 EYEBOLTFIRESPEED;
extern f32 CHESTATTACKSOUNDTIME;
extern f32 WBDISTANCESPEED;
extern f32 WBDISTANCETIME;
extern f32 WBDISTANCEYSPEED;
extern f32 WBDISTANCEYTIME;
extern s32 CHESTATTACKAVOL;
extern s32 CHESTATTACKBVOL;
extern s32 BALLATTACKVOL;
extern s32 BEAMVOL;
extern s32 EYEATTACKVOL;
extern struct nuvec_s *VulnerableA;
extern struct nuvec_s *VulnerableB;
extern f32 VulnerableRad;
extern s32 VulnerableSection;

void ProcessWeatherBoss_a(struct weatherboss_s *wb)
{
    static s32 WasLastOn;
    static s32 LastVulnerableOn;
    static struct nuvec_s VulnerablePos;
    struct nuvec_s vel;
    struct nuvec_s firepos;
    struct nuvec_s firepos2;
    struct nuvec_s gliderpos;
    struct nuvec_s vel2;
    struct nuvec_s conepos;
    struct nuvec_s conetarget;
    struct nuvec_s conevel;
    struct nuvec_s boltpos;
    s32 objectives;
    s32 laston;
    s32 vulnerableon;
    s32 side;

    objectives = GetCurrentWeatherBossObjectives();
    if (--wb->hitsoundframe < 0) {
        wb->hitsoundframe = 0;
    }
    if (wb->chestsoundtimer < 100.0f && ProcessTimer(&wb->chestsoundtimer)) {
        wb->chestsoundtimer = 100000.0f;
        ElectricalPosition = PlayerGlider.Position;
        MyGameSfx(0xC8, &ElectricalPosition, CHESTATTACKBVOL);
    }
    if (objectives < 50 && ProcessTimer(&WeatherBossSkeletonGlitchTimer)) {
        WeatherBossSkeletonTimer = WBSKELTIME + frand() * 0.5f;
        WeatherBossSkeletonGlitchTimer =
            (frand() * 0.5f + 0.15f) + (frand() * 2.0f * (f32)objectives) / 50.0f;
    }
    ProcessTimer(&WeatherBossSkeletonTimer);
    laston = 0;
    MyAnimateModelNew(&wb->maindraw, NW_HZ(0.59999996f, 0.5f));
    MyAnimateModelNew(&wb->bonesdraw, NW_HZ(0.12f, 0.1f));
    if (wb->oldaction != wb->action) {
        switch (wb->action) {
        case 0:
            wb->destyaw = wb->baseyaw;
            if (WBIntroOn == 0) {
                wb->destdist = 96.0f;
            }
            wb->destposy = 37.0f;
            MyChangeAnim(&wb->maindraw, 0x22);
            VulnerableA = 0;
            wb->actiontimer = 3.0f;
            wb->nextaction = WeatherBossNextAction();
            switch (wb->nextaction) {
            case 1:
                SetNewMaskStuff(0, &wb->pos, &WBMASKONHIGH, 0.0f, 360.0f, 0.0f, 0, 1, WBLOLOSCALE,
                                WBLOLOTILTX);
                SetNewMaskStuff(0, (struct nuvec_s *)&wb->locators[0]._30, &WBMASKLEFT, WBMASKLEFTRAD,
                                360.0f, 0.5f, 0, 0, WBLOLOSCALE, WBLOLOTILTX);
                break;
            case 3:
                SetNewMaskStuff(0, &wb->pos, &WBMASKONHIGH, 0.0f, 360.0f, 0.0f, 0, 1, WBLOLOSCALE,
                                WBLOLOTILTX);
                SetNewMaskStuff(0, (struct nuvec_s *)&wb->locators[1]._30, &WBMASKRIGHT, WBMASKRIGHTRAD,
                                360.0f, 0.5f, 0, 0, WBLOLOSCALE, WBLOLOTILTX);
                break;
            case 2:
                SetNewMaskStuff(0, &wb->pos, &WBMASKONHIGH, 0.0f, 360.0f, 0.0f, 0, 1, WBLOLOSCALE,
                                WBLOLOTILTX);
                SetNewMaskStuff(0, (struct nuvec_s *)&wb->locators[2]._30, &WBMASKEYES, WBMASKEYESRAD,
                                360.0f, 0.5f, 0, 0, WBLOLOSCALE, WBLOLOTILTX);
                break;
            case 4:
                SetNewMaskStuff(0, &wb->pos, &WBMASKONHIGH, 0.0f, 360.0f, 0.0f, 0, 1, WBLOLOSCALE,
                                WBLOLOTILTX);
                SetNewMaskStuff(0, (struct nuvec_s *)&wb->locators[4]._30, &WBMASKCHEST, WBMASKCHESTRAD,
                                360.0f, 0.5f, 0, 0, WBLOLOSCALE, WBLOLOTILTX);
                break;
            }
            break;
        case 5:
            MyGameSfx(0xCA, 0, CHESTATTACKAVOL);
            FlyingLevelVictoryDance = 1;
            wb->destdist = 0.0f;
            MyResetAnimPacket(&wb->bonesdraw, 0xE);
            wb->actiontimer = 3.0f;
            VulnerableA = 0;
            BazookaIconOn = !(Game.hub[Hub].flags & 4);
            BazookaTokenPos = wb->pos;
            BazookaTokenCurrentPos = wb->pos;
            MyInitModelNew(&IconMainDraw, 0xA2, 0x22, 0, 0, &BazookaTokenCurrentPos);
            break;
        case 1:
            WasLastOn = 0;
            wb->destdist = 96.0f;
            wb->destposy = 37.0f;
            WasLastOn = 0;
            MyChangeAnim(&wb->maindraw, 0x32);
            break;
        case 3:
            wb->destdist = 96.0f;
            wb->firetimer = 0.0f;
            wb->destposy = 37.0f;
            MyChangeAnim(&wb->maindraw, 0x39);
            break;
        case 2:
            wb->firetimer = 0.0f;
            wb->destdist = 32.0f;
            wb->fireside = 0;
            MyChangeAnim(&wb->maindraw, 0x1B);
            break;
        case 4:
            wb->unleashed = 0;
            wb->destdist = 128.0f;
            wb->destposy = 37.0f;
            MyChangeAnim(&wb->maindraw, 0);
            break;
        }
    }
    wb->oldaction = wb->action;
    switch (wb->action) {
    case 5:
        if (wb->bonesdraw.Anim.flags & 1) {
            ChrisBigBossDead = 1;
            wb->active = 0;
            BazookaIconOn = 0;
            return;
        }
        break;
    case 0:
        if (WBIntroOn == 0 && ProcessTimer(&wb->actiontimer)) {
            wb->action = wb->nextaction;
            SetNewMaskStuff(0, &wb->pos, &WBMASKONHIGH, 2.0f, 360.0f, 4.0f, 0, 1, WBLOLOSCALE,
                            WBLOLOTILTX);
        }
        break;
    case 1:
        if (wb->maindraw.Anim.action == 0x32 && wb->maindraw.Anim.anim_time >= WBLeftStartFrame &&
            wb->maindraw.Anim.anim_time <= WBLeftStopFrame) {
            VulnerableB = VulnerableA = (struct nuvec_s *)&wb->locators[0]._30;
            VulnerableRad = 9.0f;
            VulnerableSection = 0;
        } else {
            VulnerableA = 0;
        }
        if (wb->maindraw.Anim.action == 0x32 && wb->maindraw.Anim.anim_time <= WBLeftStopTurnFrame) {
            wb->destyaw = wb->baseyaw + PlayerGlider.Position.x * WBANGSCALE + WBANGOFF;
        } else {
            wb->destyaw = wb->baseyaw;
        }
        if (wb->maindraw.Anim.action == 0x32 && wb->maindraw.Anim.anim_time >= WBLeftStartFrame &&
            wb->maindraw.Anim.anim_time <= WBLeftStopFrame) {
            NuVecRotateY(&vel, SetNuVecPntr(0.0f, 0.0f, -110.0f), (wb->yaw + 45.0f) * 182.04445f);
            NuVecMtxTransform(&firepos, &WBLeftFirePos, wb->locators);
            vel.z -= Level_GliderSpeed;
            FireWBBolt(&firepos, &vel, 2, 5.0f, wb->action);
            if (WasLastOn == 0) {
                ElectricalPosition = PlayerGlider.Position;
                MyGameSfx(0xC7, &ElectricalPosition, BALLATTACKVOL);
            }
            laston = 1;
        } else if (WasLastOn != 0) {
            gliderpos = PlayerGlider.Position;
            NuVecMtxTransform(&firepos2, &WBLeftFirePos, wb->locators);
            NuVecSub(&vel2, &gliderpos, &firepos2);
            NuVecNorm(&vel2, &vel2);
            NuVecScale(&vel2, &vel2, WBSNOWBALLSPEED);
            vel2.z -= Level_GliderSpeed;
            FireWBBolt(&firepos2, &vel2, 2, 5.0f, wb->action);
        }
        if ((wb->maindraw.Anim.flags & 1) || wb->hitpoints[wb->action - 1] < 1) {
            wb->action = WeatherBossNextAction();
            wb->destyaw = wb->baseyaw;
        }
        break;
    case 3:
        if (wb->maindraw.Anim.action == 0x39 && wb->maindraw.Anim.anim_time <= 246.0f) {
            wb->destyaw = wb->baseyaw + PlayerGlider.Position.x * WBANGSCALE2 + WBANGOFF2;
        } else {
            wb->destyaw = wb->baseyaw;
        }
        if (wb->maindraw.Anim.action != 0x39) {
            VulnerableA = 0;
        } else {
            if (wb->maindraw.Anim.anim_time >= 60.0f && wb->maindraw.Anim.anim_time <= 247.0f) {
                conepos = *(struct nuvec_s *)&wb->locators[1]._30;
                conetarget = PlayerGlider.Position;
                wb->firetimer = 0.25f;
                NuVecSub(&conevel, &conetarget, &conepos);
                NuVecNorm(&conevel, &conevel);
                NuVecScale(&conevel, &conevel, WBSNOWCONESPEED);
                conevel.z -= Level_GliderSpeed;
                FireWBBolt(&conepos, &conevel, 3, 1.5f, wb->action);
                ElectricalPosition = PlayerGlider.Position;
                MyGameSfxLoop(0xCB, &ElectricalPosition, BEAMVOL);
            }
            if (wb->maindraw.Anim.action == 0x39 && wb->maindraw.Anim.anim_time >= 60.0f &&
                wb->maindraw.Anim.anim_time <= 247.0f) {
                VulnerableA = (struct nuvec_s *)&wb->locators[1]._30;
                VulnerableSection = 2;
                VulnerableB = VulnerableA;
                VulnerableRad = 9.0f;
            } else {
                VulnerableA = 0;
            }
        }
        if ((wb->maindraw.Anim.flags & 1) || wb->hitpoints[wb->action - 1] < 1) {
            wb->action = WeatherBossNextAction();
            wb->destyaw = wb->baseyaw;
        }
        break;
    case 2:
        wb->destposy = PlayerGlider.Position.y - 3.5f;
        wb->destyaw = wb->baseyaw + PlayerGlider.Position.x * WBANGSCALE3;
        if (wb->maindraw.Anim.action == 0x1B && wb->maindraw.Anim.anim_time >= 46.0f &&
            wb->maindraw.Anim.anim_time <= EYESTOPTIME) {
            VulnerableA = (struct nuvec_s *)&wb->locators[2]._30;
            VulnerableB = (struct nuvec_s *)&wb->locators[3]._30;
            VulnerableSection = 1;
            VulnerableRad = 8.0f;
            if (ProcessTimer(&wb->firetimer)) {
                wb->firetimer = 0.25f;
                boltpos = *(struct nuvec_s *)&wb->locators[wb->fireside + 2]._30;
                side = (frand() < 0.5f) ? 0 : 1;
                FireWBBolt(&boltpos,
                           SetNuVecPntr(frandPN() * EYEBOLTFIRESCALEX,
                                        EYEBOLTFIREY + frandPN() * EYEBOLTFIRESCALEY,
                                        EYEBOLTFIRESPEED - Level_GliderSpeed),
                           side, 8.0f, wb->action);
                wb->fireside = (wb->fireside + 1) & 1;
                MyGameSfx(0x85, 0, EYEATTACKVOL);
            }
        }
        if ((wb->maindraw.Anim.flags & 1) || wb->hitpoints[wb->action - 1] < 1) {
            wb->action = WeatherBossNextAction();
        }
        break;
    case 4:
        if (wb->unleashed == 0 && wb->maindraw.Anim.action == 0 && wb->maindraw.Anim.anim_time >= 48.0f) {
            boltpos = *(struct nuvec_s *)&wb->locators[4]._30;
            boltpos.z += 10.0f;
            wb->unleashed = 1;
            VulnerableA = VulnerableB = (struct nuvec_s *)&wb->locators[4]._30;
            VulnerableRad = 15.0f;
            VulnerableSection = 3;
            UnleashLighteningHail(&boltpos, frand() * 3.0f);
            MyGameSfx(0xCA, 0, CHESTATTACKAVOL);
            wb->chestsoundtimer = CHESTATTACKSOUNDTIME;
        }
        if ((wb->maindraw.Anim.flags & 1) || wb->hitpoints[wb->action - 1] < 1) {
            wb->action = WeatherBossNextAction();
        }
        break;
    }
    vulnerableon = 0;
    SeekAngHalfLife360f(&wb->yaw, wb->destyaw, 0.5f, NW_HZ(0.02f, 0.016666668f));
    SeekHalfLifeLim(&wb->dist, wb->destdist, WBDISTANCESPEED, WBDISTANCETIME, NW_HZ(0.02f, 0.016666668f));
    SeekHalfLifeLim(&wb->pos.y, wb->destposy, WBDISTANCEYSPEED, WBDISTANCEYTIME, NW_HZ(0.02f, 0.016666668f));
    wb->pos.z = PlayerGlider.Position.z - wb->dist;
    if (VulnerableA != 0 && wb->hitpoints[VulnerableSection] > 0) {
        vulnerableon = 1;
        NuVecScale(&vel, VulnerableA, 0.5f);
        NuVecScaleAccum(&vel, VulnerableB, 0.5f);
        VulnerablePos = vel;
        if (CollideGliderBullets(&vel, VulnerableRad, 0, 1.0f, 0, 0)) {
            if (wb->hitsoundframe == 0) {
                MyGameSfx(0xCC, 0, EYEATTACKVOL);
            }
            if (WeatherBossSkeletonTimer == 0.0f) {
                WeatherBossSkeletonTimer = WBSKELTIME + frand() * WBSKELTIMERAND;
            }
            if (--wb->hitpoints[VulnerableSection] < 0) {
                wb->hitpoints[VulnerableSection] = 0;
            }
        }
    }
    if (vulnerableon) {
        if (LastVulnerableOn == 0) {
            SetNewMaskStuff(1, (struct nuvec_s *)&WeatherBossCamMtx._30, &WBAKUMASKONHIGH, 0.0f,
                            -360.0f, 0.0f, 0, 1, WBLOLOSCALE, 90.0f);
            SetNewMaskStuff(1, &VulnerablePos, SetNuVecPntr(0.0f, 0.0f, 10.0f), VulnerableRad, -360.0f,
                            0.5f, 0, 0, WBAKUAKUSCALE, 90.0f);
        }
    } else if (LastVulnerableOn != 0) {
        SetNewMaskStuff(1, (struct nuvec_s *)&WeatherBossCamMtx._30, &WBAKUMASKONHIGH, 0.0f, -360.0f,
                        0.5f, 0, 1, WBLOLOSCALE, 90.0f);
    }
    LastVulnerableOn = vulnerableon;
    if (wb->dead == 0 &&
        wb->hitpoints[0] + wb->hitpoints[1] + wb->hitpoints[2] + wb->hitpoints[3] < 1) {
        WeatherBossDead = 1;
        wb->dead = 1;
        WeatherBossSkeletonTimer = 0.0f;
        wb->action = 5;
    }
    WasLastOn = laston;
}

inline s32 GetCurrentWeatherBossObjectives(void)
{
    s32 total;
    s32 i;

    if (WeatherBoss.active != 0) {
        total = 0;
        for (i = 0; i < 4; i++) {
            if (WeatherBoss.hitpoints[i] > 0) {
                total += WeatherBoss.hitpoints[i];
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

void ProcessWeatherBossLevel(struct nupad_s *pad)
{
    ProcessWBIntro();
    if (WBIntroOn == 0) {
        AtmosphericPressureHackedZ -= NW_HZ(1.92f, 1.6000001f);
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

inline void UpdateRumbleCamTween(void)
{
    if (RumbleCamTweenDest > RumbleCamTween) {
        RumbleCamTween += NW_HZ(0.02f, 0.016666668f);
        if (RumbleCamTween > RumbleCamTweenDest) {
            RumbleCamTween = RumbleCamTweenDest;
        }
    }
    if (RumbleCamTweenDest < RumbleCamTween) {
        RumbleCamTween -= NW_HZ(0.02f, 0.016666668f);
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
inline void SmashRockIntoTwo(struct jeeprock_s *rock)
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

struct jeeprock_s *AddRock(struct nuvec_s *pos, f32 radius, s32 owner)
{
    return AddRockVel(pos, &v000, radius, owner);
}


extern struct objtab_s ObjTab[];
extern s32 NuRndrGScnObj(void *gobj, struct numtx_s *m);

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
                  struct nuvec_s *targetpoint, struct nuvec_s *targetvel, s32 moving);

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
            if (bombs[i].life == 0) {
                continue;
            }
            bomb = &bombs[i];
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
                bomb->life = 0;
                bomb->pos.y = 0.0f;
                AddGameDebris(0x35, &bomb->pos);
                continue;
            }
            if (enemy == 0 && bomb->sinking == 0 && bomb->pos.y < 0.0f && Level == 0x12) {
                bomb->sinking = 1;
                continue;
            }
            if (enemy == 1) {
                SeekHalfLife(&bomb->vel.x, 0.0f, 3.0f, NW_HZ(0.02f, 0.016666668f));
                SeekHalfLife(&bomb->vel.z, 0.0f, 3.0f, NW_HZ(0.02f, 0.016666668f));
                bomb->vel.y += bomb->gravity;
                continue;
            }
            if (bomb->targetpntr->y > 100000.0f) {
                bomb->target = bomb->pos;
                bomb->target.y = -100.0f;
                bomb->targetpntr = &bomb->target;
            }
            if (ProcessTimer(&bomb->droptimer)) {
                SeekHalfLife(&bomb->gravity, 0.0f, 0.15f, NW_HZ(0.02f, 0.016666668f));
                bomb->vel.y += bomb->gravity;
                speed = NuVecMag(&bomb->vel);
                NuVecScale(&dir, &bomb->vel, 1.0f / speed);
                speed += NW_HZ(0.02f, 0.016666668f);
                if (speed > 6.0f) {
                    speed = 6.0f;
                }
                NuVecSub(&rel, bomb->targetpntr, &bomb->pos);
                dist = NuVecMag(&rel);
                if (speed != 0.0f) {
                    NuVecScaleAccum(&rel, bomb->targetvelpntr, dist * NW_HZ(0.02f, 0.016666668f) / speed);
                }
                NuVecNorm(&rel, &rel);
                SeekHalfLifeNUVEC(&dir, &rel, 0.1f, NW_HZ(0.02f, 0.016666668f));
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

extern s32 CollideGliderBullets(struct nuvec_s *pos, f32 radius, s32 enemy, f32 yscale,
                                s32 a, s32 b);
extern void AddScreenWumpa(f32 x, f32 y, f32 z, s32 n);
extern void ClockOff(void);
extern void SeekAngHalfLife360f(f32 *dest, f32 target, f32 halflife, f32 dt);
extern struct gdeb_s GDeb[];
extern void AddVariableShotDebrisEffect(s32 effect, struct nuvec_s *pos, s32 n,
                                        short xrot, short yrot);
extern void ProcessAtlas(struct NEWBUGGY *atlas);
extern s32 ROCKSMASHVOL;


void ProcessJeepRock(struct jeeprock_s *rock)
{
    struct jeeprock_s *other;
    s32 effect;

    if (Level == 0x15 && rock->owner != 1 && rock->owner != 20) {
        if (ProcessTimer(&rock->flametimer) != 0) {
            rock->owner = 0;
        }
    }
    if (rock->held != 0) {
        rock->held = 0;
        return;
    }
    if (rock->seen == 0 && rock->life < 4.0f) {
        rock->exists = 0;
    }
    if (rock->atlas.ball_beenhit > 2 && rock->owner != 20) {
        other = &JeepRock[rock->atlas.ball_beenhit - 3];
        if (other->owner != 0) {
            if (rock->owner + other->owner == 0) {
                other->owner = 0;
                rock->owner = 0;
                rock->flametimer = other->flametimer = 0.0f;
            } else if (other->owner == -1) {
                rock->owner = other->owner;
                rock->flametimer = other->flametimer = 7.0f;
            }
            other->atlas.ball_beenhit = 0;
        }
        rock->atlas.ball_beenhit = 0;
    }
    if (Level == 0x15) {
        rock->life = 10.0f;
    }
    if (ProcessTimer(&rock->life) != 0) {
        rock->explode = 1;
    } else if (rock->fireblob != 2) {
        if (rock->owner != 20) {
            ProcessAtlas(&rock->atlas);
            rock->pos = rock->atlas.ball_pos;
            if (rock->atlas.embedded != 0 && Level != 0x15) {
                rock->explode = 1;
            }
            if (rock->smashme != 0 && (rock->atlas.onground & 3)) {
                SmashRockIntoTwo(rock);
            }
            if (rock->fireblob == 1 && (rock->atlas.onground & 3)) {
                rock->fireblob = 2;
            }
        } else {
            NuVecScaleAccum(&rock->atlas.ball_pos, &rock->atlas.ball_vel, NW_HZ(0.02f, 0.016666668f));
            rock->pos = rock->atlas.ball_pos;
        }
    }
    effect = GDeb[0x3D].i;
    if (Level == 0x15) {
        switch (rock->owner) {
        case 1:
            effect = GDeb[0x3E].i;
            break;
        case 20:
            effect = GDeb[0x40].i;
            break;
        case -1:
            break;
        default:
            effect = GDeb[0x3F].i;
            break;
        }
    }
    if (effect != -1) {
        AddVariableShotDebrisEffect(effect, &rock->pos, 1, 0, 0);
    }
    if (rock->explode != 0) {
        MyGameSfx(0x56, &rock->pos, rock->atlas.ball_radius * ROCKSMASHVOL);
        AddGameDebris(0x42, &rock->pos);
        rock->exists = 0;
    }
}

extern void MyResetAnimPacket(struct MYDRAW *draw, s32 action);
extern s32 jonframe1;

extern struct gunboat_s GunBoatList[];

extern s32 AddGliderBullet(struct numtx_s *m, struct nuvec_s *pos, struct nuvec_s *vel,
                           s32 enemy);

void ProcessGunBoat(struct gunboat_s *boat)
{
    struct nuvec_s pos;
    struct nuvec_s diff;
    struct numtx_s m;
    struct nuvec_s rel;
    struct numtx_s firemtx;
    struct nuvec_s vel;
    f32 dist;
    f32 targetyaw;
    f32 delta;
    f32 rate;

    MyAnimateModelNew(&boat->draw, NW_HZ(0.59999996f, 0.5f));
    if (boat->action != 2) {
        if (CollideGliderBombs(&boat->pos, SetNuVecPntr(1.0f, 1.0f, 1.0f), 0, 0.0f)) {
            AddGameDebris(0x43, &GliderBombCollisionPos);
            boat->action = 2;
            AddScreenWumpa(boat->pos.x, boat->pos.y, boat->pos.z, 1);
            MyGameSfx(0xC9, &boat->pos, GUNBOATVOLUME);
            ClockOff();
        }
    }
    if (boat->lastaction != boat->action) {
        switch (boat->action) {
        case 0:
            break;
        case 1:
            boat->bursttimer = 0.0f;
            break;
        case 2:
            boat->sunktimer = 40.0f;
            break;
        }
    }
    boat->lastaction = boat->action;
    dist = NuVecDist(&PlayerGlider.ApparentPosition, &boat->pos, &diff);
    if (PlayerGlider.AutoPilot != 0) {
        diff.y -= 4.0f;
    }
    NuVecScaleAccum(&diff, &PlayerGlider.ApparentVelocity, dist / 30.0f);
    switch (boat->action) {
    case 0:
        if (dist <= 70.0f) {
            boat->action = 1;
        }
        break;
    case 1:
        if (80.0f < dist) {
            boat->action = 0;
            break;
        }
        NuMtxSetRotationY(&m, -boat->yaw * 182.04445f);
        NuVecMtxRotate(&rel, &diff, &m);
        targetyaw = NuAtan2D(rel.x * -100.0f, rel.z * -100.0f) / 182.04445f;
        NuVecRotateY(&rel, &rel, -targetyaw * 182.04445f);
        delta = Rationalise360f(NuAtan2D(rel.y * 100.0f, rel.z * -100.0f) / 182.04445f - boat->firepitch);
        if (5.0f < delta) {
            rate = 90.0f;
        } else if (delta < -5.0f) {
            rate = -90.0f;
        } else {
            rate = delta * 90.0f / 5.0f;
        }
        boat->firepitchmom = rate;
        boat->firepitch += rate * NW_HZ(0.02f, 0.016666668f);
        delta = Rationalise360f(targetyaw - boat->fireyaw);
        if (10.0f < delta) {
            rate = 180.0f;
        } else if (delta < -10.0f) {
            rate = -180.0f;
        } else {
            rate = delta * 180.0f / 10.0f;
        }
        boat->fireyawmom = rate;
        boat->fireyaw += rate * NW_HZ(0.02f, 0.016666668f);
        if (ProcessTimer(&boat->bursttimer)) {
            boat->fireframe = 0;
            boat->bursttimer = 3.0f;
        }
        if (1.2f <= boat->bursttimer) {
            if (--boat->fireframe < 0) {
                NuMtxSetRotationX(&firemtx, (boat->firepitch + (frand() - 0.5f) * 3.0f) * 182.04445f);
                NuMtxRotateY(&firemtx,
                             (boat->yaw + boat->fireyaw + (frand() - 0.5f) * 3.0f) * 182.04445f);
                vel = SetNuVec(0.0f, 0.0f, -30.0f);
                NuVecMtxRotate(&vel, &vel, &firemtx);
                if (AddGliderBullet(&firemtx, &boat->pos, &vel, 1)) {
                    boat->fireframe = 6;
                    MyGameSfx(0x60, &boat->pos, 0x3FFF);
                }
            }
        }
        break;
    case 2:
        if (ProcessTimer(&boat->sunktimer)) {
            boat->pos.y += NW_HZ(0.01f, 0.008333334f);
            if (0.0f < boat->pos.y) {
                boat->pos.y = 0.0f;
                boat->action = 0;
            }
        } else {
            boat->pos.y -= NW_HZ(0.01f, 0.008333334f);
        }
        if ((jonframe1 & 3) == 0) {
            pos.x = boat->pos.x;
            pos.y = 0.5f;
            pos.z = boat->pos.z;
            AddVariableShotDebrisEffect(GDeb[133].i, &pos, 1, 0, 0);
        }
        break;
    }
}

extern struct biggun_s BigGunList[];

extern s32 BigGunIndx;

inline void InitBigGun(struct nuvec_s *pos, f32 angle, s32 type, f32 pitchmin, f32 pitchmax,
                       f32 yawmain, f32 yawdeviation)
{
    struct biggun_s *gun;
    struct nuvec_s dir;

    gun = &BigGunList[BigGunIndx++];
    if (gun->active != 0) {
        return;
    }
    memset(gun, 0, sizeof(struct biggun_s));
    gun->fireyawmain = yawmain;
    gun->firepitchmin = pitchmin;
    gun->firepitchmax = pitchmax;
    gun->fireyawdeviation = yawdeviation;
    gun->pos = *pos;
    gun->action = 1;
    gun->lastaction = -1;
    gun->yaw = angle;
    gun->firepitch = 0.0f;
    gun->fireyaw = 0.0f;
    gun->yaw = gun->fireyawmain;
    gun->type = type;
    NuVecSub(&dir, &gun->pos, SetNuVecPntr(0.0f, 7.75f, 11.17f));
    NuVecNorm(&dir, &dir);
    NuVecScale(&dir, &dir, 1.5f);
    NuVecAdd(&gun->terrpos, &gun->pos, &dir);
    switch (type) {
    case 0:
        gun->hitpoints = 6;
        gun->explosioneffect = 0x36;
        break;
    case 1:
        gun->hitpoints = 2;
        gun->explosioneffect = 0x37;
        break;
    case 2:
        gun->hitpoints = 2;
        gun->explosioneffect = 0x38;
        break;
    case 3:
        gun->hitpoints = 6;
        gun->explosioneffect = 0x39;
        break;
    case 4:
        gun->hitpoints = 4;
        gun->explosioneffect = 0x3A;
        break;
    case 5:
        gun->hitpoints = 4;
        gun->explosioneffect = 0x3B;
        break;
    }
    if (ChrisInTheHouse != 0) {
        gun->explosioneffect = 0x18;
    }
    gun->active = 1;
}

void InitBigGuns(void)
{
    memset(BigGunList, 0, sizeof(struct biggun_s) * 12);
    BigGunIndx = 0;
    InitBigGun(SetNuVecPntr(0.0f, 18.86f, 30.58f), 0.0f, 0, -65.0f, -10.0f, 0.0f, 60.0f);
    InitBigGun(SetNuVecPntr(0.0f, -1.68f, 16.72f), 0.0f, 4, 10.0f, 80.0f, 0.0f, 60.0f);
    InitBigGun(SetNuVecPntr(-10.83f, 6.32f, 34.15f), 0.0f, 5, -65.0f, -10.0f, -90.0f, 60.0f);
    InitBigGun(SetNuVecPntr(10.83f, 6.32f, 34.15f), 0.0f, 5, -65.0f, -10.0f, 90.0f, 60.0f);
    InitBigGun(SetNuVecPntr(0.03f, 23.71f, 2.48f), 0.0f, 0, -65.0f, -10.0f, 180.0f, 60.0f);
    InitBigGun(SetNuVecPntr(0.0f, -25.5f, -5.97f), 0.0f, 3, -30.0f, 30.0f, 180.0f, 60.0f);
    InitBigGun(SetNuVecPntr(10.0f, 9.89f, 14.46f), 0.0f, 1, -30.0f, 30.0f, 90.0f, 30.0f);
    InitBigGun(SetNuVecPntr(9.5f, 9.89f, 11.25f), 0.0f, 1, -30.0f, 30.0f, 90.0f, 30.0f);
    InitBigGun(SetNuVecPntr(9.0f, 9.89f, 7.84f), 0.0f, 1, -30.0f, 30.0f, 90.0f, 30.0f);
    InitBigGun(SetNuVecPntr(-10.0f, 9.89f, 14.46f), 0.0f, 2, -30.0f, 30.0f, -90.0f, 30.0f);
    InitBigGun(SetNuVecPntr(-9.5f, 9.89f, 11.25f), 0.0f, 2, -30.0f, 30.0f, -90.0f, 30.0f);
    InitBigGun(SetNuVecPntr(-9.0f, 9.89f, 7.84f), 0.0f, 2, -30.0f, 30.0f, -90.0f, 30.0f);
}

f32 GetBigGunBestTarget(f32 bestdot, struct nuvec_s **pos, struct nuvec_s **vel, s32 *type)
{
    struct nuvec_s camdir;
    struct nuvec_s rel;
    struct nuvec_s shootdir;
    s32 i;
    s32 clear;
    f32 dot;
    f32 mag;

    NuVecMtxRotate(&camdir, SetNuVecPntr(0.0f, 0.0f, 1.0f), &GameCam[0].m);
    for (i = 0; i < 12; i++) {
        if (BigGunList[i].active != 0) {
            if (BigGunList[i].hitpoints > 0) {
                NuVecSub(&rel, &BigGunList[i].pos, &GameCam[0].pos);
                mag = NuVecMag(&rel);
                if (10.0f < mag) {
                    NuVecScale(&rel, &rel, 1.0f / mag);
                    dot = DotProduct(&camdir, &rel);
                    if (bestdot < dot) {
                        NuVecSub(&shootdir, &PlayerGlider.Position, &BigGunList[i].terrpos);
                        NuVecNorm(&shootdir, &shootdir);
                        NuVecScale(&shootdir, &shootdir, 3.0f);
                        clear = NewRayCast(&BigGunList[i].terrpos, &shootdir, 0.0f) == 0;
                        if (clear) {
                            *pos = &BigGunList[i].terrpos;
                            bestdot = dot;
                            *vel = &v000;
                            *type = 0;
                        }
                    }
                }
            }
        }
    }
    return bestdot;
}


f32 GetBattleShipBestTarget(f32 bestdot, struct nuvec_s **pos, struct nuvec_s **vel, s32 *type)
{
    struct nuvec_s camdir;
    struct nuvec_s rel;
    s32 i;
    f32 dot;
    f32 mag;

    NuVecMtxRotate(&camdir, SetNuVecPntr(0.0f, 0.0f, 1.0f), &GameCam[0].m);
    for (i = 0; i < 6; i++) {
        if (BattleShipList[i].active != 0) {
            if (BattleShipList[i].hitpoints > 0) {
                NuVecSub(&rel, &BattleShipList[i].pos, &GameCam[0].pos);
                mag = NuVecMag(&rel);
                if (10.0f < mag) {
                    NuVecScale(&rel, &rel, 1.0f / mag);
                    dot = DotProduct(&camdir, &rel);
                    if (bestdot < dot) {
                        *pos = &BattleShipList[i].pos;
                        bestdot = dot;
                        *vel = &v000;
                        *type = 0;
                    }
                }
            }
        }
    }
    return bestdot;
}


f32 GetGunBoatBestTarget(f32 bestdot, struct nuvec_s **pos, struct nuvec_s **vel, s32 *type)
{
    struct nuvec_s camdir;
    struct nuvec_s rel;
    s32 i;
    f32 dot;
    f32 mag;

    NuVecMtxRotate(&camdir, SetNuVecPntr(0.0f, 0.0f, 1.0f), &GameCam[0].m);
    for (i = 0; i < 4; i++) {
        if (GunBoatList[i].active != 0) {
            if (GunBoatList[i].action != 2) {
                NuVecSub(&rel, &GunBoatList[i].pos, &GameCam[0].pos);
                mag = NuVecMag(&rel);
                if (10.0f < mag) {
                    NuVecScale(&rel, &rel, 1.0f / mag);
                    dot = DotProduct(&camdir, &rel);
                    if (bestdot < dot) {
                        *pos = &GunBoatList[i].pos;
                        bestdot = dot;
                        *vel = &v000;
                        *type = 0;
                    }
                }
            }
        }
    }
    return bestdot;
}

extern void DrawCross(struct nuvec_s *pos, s32 colour, f32 size);
extern s32 BigGunDraw;
extern s32 TimeTrial;
extern f32 TimeTrialWait;
extern s32 LimitAng360f(f32 *dest, f32 min, f32 max);
extern void DrawLine(struct nuvec_s *p0, struct nuvec_s *p1, s32 colour);
extern void AddGameDebrisRot(s32 i, struct nuvec_s *pos, s32 n, u16 xrot, u16 yrot);

void ProcessBigGun(struct biggun_s *gun)
{
    static struct nuvec_s Val = {0.0f, 0.0f, 5.0f};
    struct nuvec_s diff;
    f32 dist;

    if (gun == &BigGunList[BigGunDraw]) {
        DrawCross(&gun->pos, 0xFFFFFF, 5.0f);
    }
    if (CollideGliderBombs(&gun->pos, SetNuVecPntr(1.0f, 1.0f, 1.0f), 0, 0.0f)) {
        AddGameDebris(0x1A, &gun->pos);
        if (gun->hitpoints > 0) {
            gun->action = 2;
        }
    }
    if (gun->lastaction != gun->action) {
        switch (gun->action) {
        case 0:
            break;
        case 1:
            gun->bursttimer = 0.0f;
            break;
        case 2:
            gun->hitpoints = 0;
            if (TimeTrial != 0) {
                TimeTrialWait += 2.0f;
            }
            break;
        }
    }
    gun->lastaction = gun->action;
    dist = NuVecDist(&PlayerGlider.ApparentPosition, &gun->pos, &diff);
    NuVecScaleAccum(&diff, &PlayerGlider.ApparentVelocity, dist / 30.0f);
    switch (gun->action) {
    case 0:
        gun->action = 1;
        break;
    case 1: {
        f32 targetyaw;
        f32 targetpitch;
        f32 delta;
        f32 rate;
        s32 inrange;

        {
            struct numtx_s m;
            struct nuvec_s rel;

            NuMtxSetRotationY(&m, -gun->yaw * 182.04445f);
            NuVecMtxRotate(&rel, &diff, &m);
            targetyaw = NuAtan2D(rel.x * 100.0f, rel.z * 100.0f) / 182.04445f;
            inrange = LimitAng360f(&targetyaw, -gun->fireyawdeviation, gun->fireyawdeviation);
            NuVecRotateY(&rel, &rel, -targetyaw * 182.04445f);
            targetpitch = -(f32)NuAtan2D(rel.y * 100.0f, rel.z * 100.0f) / 182.04445f;
            inrange &= LimitAng360f(&targetpitch, gun->firepitchmin, gun->firepitchmax);
            delta = Rationalise360f(targetpitch - gun->firepitch);
            if (5.0f < delta) {
                rate = 90.0f;
            } else if (delta < -5.0f) {
                rate = -90.0f;
            } else {
                rate = delta * 90.0f / 5.0f;
            }
            gun->firepitchmom = rate;
            gun->firepitch += rate * NW_HZ(0.02f, 0.016666668f);
            delta = Rationalise360f(targetyaw - gun->fireyaw);
            if (10.0f < delta) {
                rate = 180.0f;
            } else if (delta < -10.0f) {
                rate = -180.0f;
            } else {
                rate = delta * 180.0f / 10.0f;
            }
            gun->fireyawmom = rate;
            gun->fireyaw += rate * NW_HZ(0.02f, 0.016666668f);
            if (ProcessTimer(&gun->bursttimer) && inrange) {
                gun->fireframe = 0;
                gun->bursttimer = 10.0f;
            }
            if (6.0f <= gun->bursttimer) {
                if (--gun->fireframe < 0) {
                    struct numtx_s firemtx;
                    struct nuvec_s vel;
                    struct nuvec_s start;

                    start = gun->pos;
                    NuMtxSetRotationX(&firemtx,
                                      (gun->firepitch + 180.0f + (frand() - 0.5f) * 3.0f) * 182.04445f);
                    NuMtxRotateY(&firemtx, (gun->yaw + gun->fireyaw + (frand() - 0.5f) * 3.0f) * 182.04445f);
                    vel = SetNuVec(0.0f, 0.0f, -30.0f);
                    NuVecMtxRotate(&vel, &vel, &firemtx);
                    NuVecScaleAccum(&start, &vel, 0.1f);
                    if (AddGliderBullet(&firemtx, &start, &vel, 1)) {
                        gun->fireframe = 5;
                        MyGameSfx(0x60, &start, 0x3FFF);
                    }
                }
            }
            if (ChrisInTheHouse != 0) {
                struct nuvec_s dir;
                struct nuvec_s end;

                NuVecRotateX(&dir, &Val, gun->firepitch * 182.04445f);
                NuVecRotateY(&end, &dir, (gun->fireyaw + gun->yaw) * 182.04445f);
                NuVecAdd(&end, &end, &gun->pos);
                if (inrange) {
                    DrawLine(&gun->pos, &end, 0xFF);
                } else {
                    DrawLine(&gun->pos, &end, 0xFF0000);
                }
            }
        }
        break;
    }
    case 2: {
        struct nuvec_s pos;

        pos = gun->pos;
        AddGameDebrisRot(gun->explosioneffect, &pos, 1, 0, 0);
        break;
    }
    }
}

static struct nuvec_s BattleShipCollScale;
extern s32 InvincibilityCHEAT;

s32 CollideWithBattleShip(struct nuvec_s *pos, struct battleship_s *ship, f32 radius)
{
    struct nuvec_s rel;
    struct nuvec_s *centre;
    f32 size;
    f32 minsize;

    centre = &ship->pos;
    size = (BattleShipCollScale.x > BattleShipCollScale.y) ? BattleShipCollScale.x : BattleShipCollScale.y;
    if (size < BattleShipCollScale.z) {
        size = BattleShipCollScale.z;
    }
    rel.x = pos->x - centre->x;
    minsize = -size;
    if (rel.x <= size && minsize <= rel.x) {
        rel.y = pos->y - centre->y;
        if (rel.y <= size && minsize <= rel.y) {
            rel.z = pos->z - centre->z;
            if (rel.z <= size && minsize <= rel.z) {
                NuVecRotateY(&rel, &rel, (s32)(-ship->yaw * 182.04445f));
                rel.x /= BattleShipCollScale.x;
                rel.y /= BattleShipCollScale.y;
                rel.z /= BattleShipCollScale.z;
                if (rel.x * rel.x + rel.y * rel.y + rel.z * rel.z <= 1.0f) {
                    if (InvincibilityCHEAT != 0) {
                        ship->rammed = 1;
                    }
                    return 1;
                }
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
    ship->goospeed = (ship->gootimer.x * 0.1f + 0.2f) * NW_HZ(0.02f, 0.016666668f);
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

inline void DrawBigGun(struct biggun_s *gun)
{
    struct nuvec_s scale = {1.5f, 1.5f, 1.5f};

    NuMtxSetScale(&mTEMP, &scale);
    NuMtxRotateX(&mTEMP, gun->firepitch * 182.04445f);
    NuMtxRotateY(&mTEMP, (gun->yaw + gun->fireyaw) * 182.04445f);
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

extern struct spacestation_s SpaceStationList[3];

/* trail array: stride 0x20, 128 entries. */
struct trail_s {
    struct nuvec_s pos[2];     /* 0x00 */
    s32 alpha;                 /* 0x18 */
    s32 basealpha;             /* 0x1C */
};
extern struct trail_s trail[128];
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

extern void *app_fnt;
extern void NuFntSet(void *fnt);
extern void NuFntSetPen(s32 col);
extern void NuFntScale(s32 x, s32 y);
extern void NuFntPrintEx(s32 x, s32 y, s32 flags, char *fmt, ...);
extern s32 SHEIGHT;
extern struct spltab_s SplTab[];
extern void PointAlongSpline(struct nugspline_s *spl, f32 ratio, struct nuvec_s *dst,
                             u16 *angle, u16 *tilt);
extern struct nuvec_s *CamOveride;
extern f32 WBSCALE;
extern s32 LocIndx;

void InitWeatherBoss_a(void)
{
    struct weatherboss_s *wb;
    struct nuvec_s start;
    u16 startangle;

    wb = &WeatherBoss;
    memset(&WeatherBoss, 0, 0x65C);
    if (MyInitModelNew(&wb->maindraw, 0x86, 0x32, 0, 0, &wb->pos) != 0 &&
        MyInitModelNew(&wb->bonesdraw, 0xBE, 0xE, 0, 0, &wb->pos) != 0) {
        BazookaIconOn = 0;
        wb->active = 1;
        wb->hitpoints[0] = 100;
        wb->hitpoints[1] = 100;
        wb->hitpoints[2] = 100;
        wb->hitpoints[3] = 100;
        wb->chestsoundtimer = 100000.0f;
        wb->action = 0;
        wb->oldaction = -1;
        wb->spline = SplTab[25].spl;
        if (SplTab[25].spl != 0) {
            wb->splinecur = 0.0f;
            wb->splineinc = 0.002f;
            wb->splinenex = 0.0f;
            PointAlongSpline(SplTab[25].spl, wb->splinecur, &start, &startangle, 0);
            wb->splinecurpos = start;
            wb->splinenexpos = start;
            wb->splinelookahead = 128.0f;
            wb->yaw = startangle / 182.04445f;
            wb->baseyaw = wb->yaw;
            wb->destyaw = wb->yaw;
            wb->pos = start;
            wb->pos = SetNuVec(0.0f, 37.0f, 0.0f);
            wb->active = 1;
            wb->destposy = wb->pos.y;
            CamOveride = &wb->pos;
            wb->dist = -96.0f;
            wb->destdist = -96.0f;
        }
    }
}

/* Defined here: retail emits them before DrawWeatherBoss_a's strings. */
f32 SPACETORNADOSCALE = 2.0f;
f32 SPACEMINSPEEDIN = 40.0f;
s32 HITSHIELDVOL = 8191;
s32 HITSHIELDVOLA = 16383;
s32 SPACESTATIONEXPLODEVOL = 0x4ffff;
f32 SHIELDSCALEY = 1.1f;
f32 FarmZepplinSpeed = 22.0f;
f32 FarmZepplinTimer = 2.5f;
f32 CameraSeekSpeedAutoPilot = 0.8f;
f32 WeatherCamCentY = 35.0f;
f32 WeatherCamScaleX = 0.8f;
f32 WeatherCamScaleY = 0.9f;
f32 WeatherCamDistY = 2.75f;
f32 WeatherCamDistZ = 8.0f;
f32 WeatherCamIntDistZ = 2000.0f;
f32 WBCamY = 27.0f;
f32 WBCamZ = 2000.0f;
f32 WBXTiltScale = 0.5f;
f32 WBYTiltScale = 1.0f;
f32 WBZTiltScale = 0.5f;
f32 WBTween = 1.0f;
f32 WBTX = 0.5f;
f32 WBTZ = 0.5f;
f32 ExtroTargAng = -170.0f;
f32 TORNADOINTERESTY = 0.2f;
f32 FFTWEENTIME = 0.25f;
f32 ExtroChangeVal = 0.5f;
f32 ExtroStillToEndTime = 1.5f;
f32 ExtroSeekZeroTime = 0.4f;
s32 AsteroidDebug = -1;
s32 SINKTORPVOL = 16383;
s32 SINKTORPPITCH = 512;

inline void PrintWeatherBossAction(struct weatherboss_s *wb)
{
    char *name;

    NuFntSet(app_fnt);
    NuFntSetPen(0x7F7F7F7F);
    NuFntScale(0x10, 0x10);
    switch (wb->action) {
    case 0:
        name = "Seek";
        break;
    case 1:
        name = "AttackLeft";
        break;
    case 3:
        name = "AttackRight";
        break;
    case 2:
        name = "AttackEyes";
        break;
    case 4:
        name = "AttackChest";
        break;
    case 5:
        name = "Dead";
        break;
    default:
        name = "Unknown";
        break;
    }
    NuFntPrintEx(0x80, SHEIGHT * 2, 0, "Action %s %f", name, wb->maindraw.Anim.anim_time);
}

void DrawWeatherBoss_a(struct weatherboss_s *wb)
{
    struct nuvec_s pos;

    pos = wb->pos;
    NuMtxSetScale(&mTEMP, SetNuVecPntr(WBSCALE, WBSCALE, WBSCALE));
    NuMtxRotateY(&mTEMP, wb->yaw * 182.04445f);
    NuMtxTranslate(&mTEMP, &pos);
    if (wb->action == 5) {
        wb->seen = MyDrawModelNew(&wb->bonesdraw, &mTEMP, wb->locators);
    } else {
        wb->seen = MyDrawModelNew(&wb->maindraw, &mTEMP, wb->locators);
    }
    if (ChrisInTheHouse != 0) {
        PrintWeatherBossAction(wb);
    }
    if (LocIndx != -1) {
        DrawCross((struct nuvec_s *)&wb->locators[LocIndx]._30, 0xFF, 10.0f);
    }
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

extern f32 BIGSCALE;
extern f32 SMALLSCALE;
extern f32 Level_TargetTime;
extern s32 GliderFrameCounter;

void DrawTorpedoTarget(void)
{
    struct nuvec_s scaledpos;
    struct nuvec_s projpos[4];
    struct nuvec_s centre;
    struct nuvec_s corner[4];
    struct nuvec_s dir;
    struct nuvec_s delta;
    struct nuvec_s *target;
    struct nuvec_s *vel;
    s32 moving;
    s32 obj;
    s16 angle;
    s32 next;
    s32 i;
    f32 ratio;
    f32 midscale;
    f32 rocketscale;

    if (PlayerGlider.TerminalDive != 0) {
        return;
    }
    if (PlayerGlider.TargetOn == 0) {
        s32 aimobj = 0x5A;

        if (Level == 0x24) {
            aimobj = 0x5E;
        }
        if (!PickGliderTarget(&target, &vel, &moving)) {
            return;
        }
        if (ObjTab[aimobj].obj.special == 0) {
            return;
        }
        NuVecSub(&centre, target, &GameCam[0].pos);
        NuVecScale(&centre, &centre, 10.0f / NuVecMag(&centre));
        NuVecAdd(&centre, &centre, (struct nuvec_s *)&GameCam[0].m._30);
        mTEMP = GameCam[0].m;
        NuMtxPreScale(&mTEMP, SetNuVecPntr(BIGSCALE, BIGSCALE, 0.0f));
        *(struct nuvec_s *)&mTEMP._30 = centre;
        NuRndrGScnObj(ObjTab[aimobj].obj.scene->gobjs[ObjTab[aimobj].obj.special->instance->objid],
                      &mTEMP);
        return;
    }
    NuVecSub(&centre, PlayerGlider.MovingTargetPoint, &GameCam[0].pos);
    NuVecScale(&scaledpos, &centre, 10.0f / NuVecMag(&centre));
    NuVecAdd(&scaledpos, &scaledpos, (struct nuvec_s *)&GameCam[0].m._30);
    NuVecMtxTransform(&corner[0], SetNuVecPntr(NW_WIDE_SCALE(-4.5f, nw_wide.hud), 3.5f, 10.0f), &GameCam[0].m);
    NuVecMtxTransform(&corner[1], SetNuVecPntr(NW_WIDE_SCALE(4.5f, nw_wide.hud), 3.5f, 10.0f), &GameCam[0].m);
    NuVecMtxTransform(&corner[2], SetNuVecPntr(NW_WIDE_SCALE(4.5f, nw_wide.hud), -3.5f, 10.0f), &GameCam[0].m);
    NuVecMtxTransform(&corner[3], SetNuVecPntr(NW_WIDE_SCALE(-4.5f, nw_wide.hud), -3.5f, 10.0f), &GameCam[0].m);
    i = 0;
    do {
        next = (i + 1) & 3;
        NuVecSub(&dir, &corner[next], &corner[i]);
        NuVecSub(&delta, &scaledpos, &corner[i]);
        projpos[i] = corner[i];
        NuVecScaleAccum(&projpos[i], &dir, DotProduct(&dir, &delta) / NuVecMagSqr(&dir));
        i++;
    } while (i < 4);
    ratio = PlayerGlider.TargetTimer / Level_TargetTime;
    midscale = ratio * (BIGSCALE - SMALLSCALE) + SMALLSCALE;
    rocketscale = ratio * (BIGSCALE - SMALLSCALE) * 4.0f + SMALLSCALE;
    if (ratio != 0.0f) {
        obj = 0x5A;
    } else {
        obj = (GliderFrameCounter & 2) ? 0x5A : 0x5B;
    }
    if (Level == 0x24) {
        obj += 4;
    }
    if (ObjTab[obj].obj.special != 0) {
        mTEMP = GameCam[0].m;
        NuMtxPreScale(&mTEMP, SetNuVecPntr(midscale, midscale, 0.0f));
        *(struct nuvec_s *)&mTEMP._30 = scaledpos;
        NuRndrGScnObj(ObjTab[obj].obj.scene->gobjs[ObjTab[obj].obj.special->instance->objid], &mTEMP);
    }
    angle = 0;
    if (ratio != 0.0f) {
        obj = 0x5C;
    } else {
        obj = (GliderFrameCounter & 2) ? 0x5C : 0x5D;
    }
    if (Level == 0x24) {
        obj += 4;
    }
    if (ObjTab[obj].obj.special == 0) {
        return;
    }
    for (i = 0; i < 4; i++) {
        mTEMP = GameCam[0].m;
        NuMtxPreScale(&mTEMP, SetNuVecPntr(rocketscale, rocketscale, 0.0f));
        NuVecScale((struct nuvec_s *)&mTEMP._30, &projpos[i], ratio);
        NuVecScaleAccum((struct nuvec_s *)&mTEMP._30, &scaledpos, 1.0f - ratio);
        NuMtxPreRotateZ(&mTEMP, angle);
        NuRndrGScnObj(ObjTab[obj].obj.scene->gobjs[ObjTab[obj].obj.special->instance->objid], &mTEMP);
        angle -= 0x4000;
    }
}

inline void DrawWeatherResearchLevelExtra(void)
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
            if (BigGunList[i].action != 2) {
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
        trail[i].pos[0].x = -10000.0f;
    }
    trailpt = 0;
    trailair = 0;
}

extern struct nuquat_s SetNuQuat(f32 x, f32 y, f32 z, f32 w);

inline void InitAtlas(struct NEWBUGGY *Atlas, struct nuvec_s *Pos, f32 Radius, s32 Type)
{
    struct creature_s *cre;
    struct CharacterData *cdata;

    cre = Atlas->owner;
    memset(Atlas, 0, sizeof(struct NEWBUGGY));
    Atlas->owner = cre;
    Atlas->type = Type;
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

extern s32 AtlasEmbeddedFrame;
extern f32 AtlasWhackValue;
extern f32 AtlasWhackTimer;
extern s32 ResetAtlasCamera;


void ResetAtlas(struct creature_s *c)
{
    struct NEWBUGGY *atlas;
    struct nuvec_s pos;

    atlas = c->Buggy;
    if (atlas == 0) {
        c->Buggy = &PlayerAtlas;
        atlas = &PlayerAtlas;
        memset(&PlayerAtlas, 0, sizeof(struct NEWBUGGY));
        PlayerAtlas.owner = c;
    }
    AtlasFrame = -1;
    AtlasEmbeddedFrame = 0;
    pos.x = c->obj.pos.x;
    pos.y = c->obj.pos.y + 1.0f;
    pos.z = c->obj.pos.z;
    AtlasWhackValue = 0.0f;
    AtlasWhackTimer = 0.0f;
    if (Level == 0x10 && ChrisInTheHouse != 0) {
        pos = (struct nuvec_s){21.68f, 1.25f, 3.05f};
    }
    if (Level == 0 && ChrisInTheHouse != 0) {
        pos = SetNuVec(27.88f, -16.0f, 23.17f);
    }
    if (Level == 0xB && ChrisInTheHouse != 0) {
        pos = SetNuVec(38.66f, -3.0f, -59.15f);
    }
    if (Level == 0x20 && ChrisInTheHouse != 0) {
        pos = SetNuVec(105.25f, -14.47f, 36.82f);
    }
    if (Level == 0x15) {
        pos = SetNuVec(0.0f, -2.83736f, 0.0f);
    }
    ResetAtlasCamera = 1;
    RumbleCamTween = 1.0f;
    RumbleCamTweenDest = 1.0f;
    RumbleCamVal = 0.0f;
    InitAtlas(atlas, &pos, 0.65f, 1);
    atlas->ball_health = 100;
    atlas->desthitpoints = 100;
}

void InitEarthBoss(void)
{
    struct nuvec_s start;

    start = (struct nuvec_s){0.0f, -2.35f, 7.98f};
    InitAtlas(&EarthBoss, &start, 1.0f, 2);
    EarthBoss.ball_health = 3;
    MyInitModelNew(&EarthBoss.shell, 0xAD, 0x1F, 0, 0, &EarthBoss.ball_pos);
    MyInitModelNew(&EarthBoss.crunch, 0x7F, 0x22, 0, 0, &EarthBoss.ball_pos);
    ShootRockSound = 0;
    EarthBoss.lastaction = -1;
    EarthBoss.ball_action = 0;
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
    if (GameTimer.frame < NW_HZ(50, 60)) {
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


extern s32 TryUnembeddPointDirSimple(struct nuvec_s *Point, struct nuvec_s *Dir, f32 Radius,
                                     s16 *TerrHandle, f32 Scale, s32 MaxDepth);

extern struct nuvec_s ShadNorm;
extern s32 TryUnembeddPointDir(struct nuvec_s *pos, struct nuvec_s *norm, struct nuvec_s *dir,
                               f32 radius, s16 *handle);
extern s32 TryUnembeddPointSafe(struct nuvec_s *pos, struct nuvec_s *safe, s16 *handle,
                                f32 radius);

/* Push the atlasphere out of the terrain along the ground normal, or back to
 * its last safe position if that fails. Both unembed helpers are defined
 * before AdjustAtlasRotations so their {0, 1, 0} constant comes first. */
inline void UnembedRayCastAtlas(struct NEWBUGGY *b, s16 *handle)
{
    struct nuvec_s up = {0.0f, 1.0f, 0.0f};

    if (!TryUnembeddPointDir(&b->ball_pos, &ShadNorm, &up, b->ball_radius, handle)) {
        TryUnembeddPointSafe(&b->ball_pos, &b->ball_oldpos, handle, b->ball_radius);
    }
}

inline s32 UnembedRayCastAtlasSimple(struct NEWBUGGY *b, s16 *handle)
{
    struct nuvec_s dir = {0.0f, 1.0f, 0.0f};

    return TryUnembeddPointDirSimple(&b->ball_pos, &dir, b->ball_radius, handle, 0.03f, 5);
}

void AdjustAtlasRotations(struct NEWBUGGY *Atlas, struct nuvec_s *Velocity, struct nuvec_s *Normal, f32 DeltaTime)
{
    struct nuvec_s vel;
    struct numtx_s mtx;
    struct numtx_s inv;
    struct nuquat_s quat;
    f32 yaw;
    f32 pitch;
    f32 roll;
    f32 spin;
    s32 yawang;
    s32 pitchang;

    vel = *Velocity;
    if (fabs(Normal->x) < 0.01f && fabs(Normal->z) < 0.01f) {
        yaw = 0.0f;
    } else {
        yaw = -(s32)(u16)NuAtan2D(Normal->x, Normal->z) / 182.04445f;
    }
    pitch = -(s32)(u16)NuAtan2D(NuFsqrt(Normal->x * Normal->x + Normal->z * Normal->z), Normal->y) / 182.04445f;
    NuVecScaleAccum(&vel, Normal, -DotProduct(&vel, Normal));
    yawang = yaw * 182.04445f;
    NuVecRotateY(&vel, &vel, yawang);
    pitchang = pitch * 182.04445f;
    NuVecRotateX(&vel, &vel, pitchang);
    spin = NuVecMag(&vel) * 180.0f;
    spin /= DeltaTime * 3.1415927f;
    roll = (u16)NuAtan2D(vel.x, vel.z) / 182.04445f;
    NuMtxSetRotationY(&mtx, yawang);
    NuMtxRotateX(&mtx, pitchang);
    NuMtxRotateY(&mtx, (s32)(-roll * 182.04445f));
    NuMtxInv(&inv, &mtx);
    NuMtxRotateX(&mtx, (s32)(spin * 182.04445f));
    NuMtxMul(&mtx, &mtx, &inv);
    NuMtxToQuat(&mtx, &quat);
    NuQuatMul(&Atlas->rotquat_delta, &quat, &Atlas->rotquat_delta);
}

extern s32 DavesGay;
extern s16 GetVolumeI(f32 vol);


extern void CheckAtlasGround(struct NEWBUGGY *atlas);


extern void ProcessPlatformsAtlas(struct NEWBUGGY *atlas, f32 dt);
extern void TerrainAtlas(struct NEWBUGGY *atlas, f32 dt);


/* Inlined with dt = 0.02f: retail loads the constant afresh for each call. */
static inline void StepAtlas(struct NEWBUGGY *atlas, f32 dt)
{
    ProcessMovementAtlas(atlas, dt);
    NewScanInit();
    atlas->terrhandle = NewScanHandel(&atlas->ball_pos, &atlas->ball_vel, 1, 0, atlas->ball_radius + 0.25f);
    if (Level != 0x15) {
        ProcessPlatformsAtlas(atlas, dt);
    }
    TerrainAtlas(atlas, dt);
}

void ProcessAtlas(struct NEWBUGGY *atlas)
{
    if (AtlasFrame >= 0x1FE) {
        DavesGay++;
    }
    atlas->onground <<= 1;
    atlas->rotquat_delta = SetNuQuat(0.0f, 0.0f, 0.0f, 1.0f);
    atlas->ball_lastpos = atlas->ball_pos;
    ProcessTimer(&atlas->boosttimer);
    StepAtlas(atlas, NW_HZ(0.02f, 0.016666668f));
    if (atlas->type == 1) {
        CheckAtlasGround(atlas);
    } else if (Level == 0x16 && (atlas->onground & 3)) {
        MyGameSfxLoop(0x41, &atlas->ball_pos,
                      GetVolumeI(NuVecDist(&atlas->ball_lastpos, &atlas->ball_pos, 0) * atlas->ball_radius * 10.0f));
    }
    if (atlas->onground & 3) {
        NuQuatMul(&atlas->rotquat, &atlas->rotquat_delta, &atlas->rotquat);
        NuQuatNormalise(&atlas->rotquat, &atlas->rotquat);
        atlas->framequat[3] = atlas->framequat[2];
        atlas->framequat[2] = atlas->framequat[1];
        atlas->framequat[1] = atlas->framequat[0];
        atlas->framequat[0] = atlas->rotquat_delta;
    } else {
        NuQuatMul(&atlas->rotquat, &atlas->framequat[0], &atlas->rotquat);
        NuQuatNormalise(&atlas->rotquat, &atlas->rotquat);
    }
    if (Level == 0x15) {
        struct nuvec_s edge;
        struct nuvec_s dir;
        f32 limit;
        f32 dist;
        f32 dot;

        dist = atlas->ball_pos.x * atlas->ball_pos.x + atlas->ball_pos.z * atlas->ball_pos.z;
        limit = 15.3f - atlas->ball_radius;
        if (limit * limit < dist) {
            f32 MagXZ;

            MagXZ = NuFsqrt(dist);
            dir.x = atlas->ball_pos.x / MagXZ;
            dir.y = 0.0f;
            dir.z = atlas->ball_pos.z / MagXZ;
            edge = dir;
            dot = DotProduct(&atlas->ball_vel, &edge);
            atlas->ball_pos.x = edge.x * limit;
            atlas->ball_pos.z = edge.z * limit;
            if (dot > 0.0f) {
                NuVecScaleAccum(&atlas->ball_vel, &edge, -dot);
            }
        }
        if (atlas->ball_pos.y > 7.0f) {
            atlas->ball_pos.y = 7.0f;
            atlas->ball_vel.y = 0.0f;
        }
    }
    atlas->lastquat = atlas->rotquat;
    atlas->ball_oldvel = atlas->ball_vel;
}

extern s32 Paused;

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
extern s32 qrand(void);


void JonnyParticles(struct nuvec_s *pos)
{
    struct nuvec_s spawn;
    s32 i;

    if (ebgo == 0) {
        return;
    }
    if (ebgo == 1) {
        ebgo = 2;
        ebtime[0] = 0.0f;
        ebpos[0].x = 99999.0f;
        ebtime[1] = 10.0f;
        ebtime[2] = 20.0f;
        ebpos[1].x = 99999.0f;
        ebpos[2].x = 99999.0f;
    }
    for (i = 0; i < 3; i++) {
        ebtime[i] -= 1.0f;
        if (ebtime[i] < 0.0f) {
            if (ebpos[i].x != 99999.0f) {
                ebpos[i].y += 1.0f;
                AddGameDebris(0x17, &ebpos[i]);
            }
            ebtime[i] = 30.0f;
            spawn = *pos;
            spawn.x += (s32)(qrand() - 0x8000) * 0.000061035156f;
            spawn.z += (s32)(qrand() - 0x8000) * 0.000061035156f;
            spawn.y += 12.0f;
            spawn.y = NewShadow(&spawn, 0.0f) + 12.0f;
            ebpos[i] = spawn;
        }
        if (ebpos[i].x != 99999.0f) {
            ebpos[i].y -= 0.4f;
            AddVariableShotDebrisEffect(GDeb[64].i, &ebpos[i], 1, 0, 0);
        }
    }
}


inline void DrawJonny(void)
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

extern void PrintEarthBossAction(struct NEWBUGGY *boss);


void DrawEarthBoss(void)
{
    struct NEWBUGGY *boss;
    struct nuvec_s scale;
    f32 size;

    boss = &EarthBoss;
    if (boss->dead != 0) {
        return;
    }
    DrawJonny();
    if (boss->drawcrunch != 0) {
        NuMtxSetRotationY(&mTEMP, boss->crunchy * 182.04445f);
        NuMtxTranslate(&mTEMP, &boss->ball_pos);
        mTEMP._31 -= boss->ball_radius;
        MyDrawModelNew(&boss->crunch, &mTEMP, 0);
    }
    if (boss->drawshell != 0) {
        size = boss->ball_radius / 0.65f * 2.56;
        scale = SetNuVec(size, size, size);
        NuQuatToMtx(&boss->rotquat, &mTEMP);
        NuMtxScale(&mTEMP, &scale);
        NuMtxTranslate(&mTEMP, &boss->ball_pos);
        MyDrawModelNew(&boss->shell, &mTEMP, 0);
    }
    if (ChrisInTheHouse != 0) {
        PrintEarthBossAction(boss);
    }
}

extern f32 ATLASBOOSTSPEED;
extern s32 ATLASBOOSTVOL;
extern f32 ATLASLOOPVOL;
extern f32 BALLRUMBLESLOPE;


extern f32 BALLBUZZSLOPE;
extern f32 BALLBUZZMIN;
extern f32 BALLBUZZLIMCOS;
extern f32 BALLHITVOLDIV;
extern f32 BALLHITVOLBASE;
extern struct nuvec_s NullNuVec;
extern struct nuvec_s *CrossProduct(struct nuvec_s *dest, struct nuvec_s *a, struct nuvec_s *b);

void TerrainAtlas(struct NEWBUGGY *Atlas, f32 DeltaTime)
{
    struct nuvec_s Stack[6];
    struct nuquat_s QuatStack[6];
    struct nuvec_s Delta;
    struct nuvec_s ToMove;
    s32 StartDepth;
    s32 Hit;
    f32 dot;
    f32 speed;
    f32 scale;
    f32 vol;
    s32 platid;
    s32 unembedded;
    s32 Depth;

    Depth = 6;
    StartDepth = 6;
    /* Keep the debug-counter accesses on their retail execution paths. */
    if (AtlasFrame >= 500) {
        (*(volatile s32 *)&tttt)++;
    }
    ToMove.x = Atlas->ball_target.x - Atlas->ball_pos.x;
    ToMove.y = Atlas->ball_target.y - Atlas->ball_pos.y;
    ToMove.z = Atlas->ball_target.z - Atlas->ball_pos.z;
    do {
        Delta = ToMove;
        Stack[Depth - 1] = Atlas->ball_pos;
        QuatStack[Depth - 1] = Atlas->rotquat_delta;
        Hit = NewRayCastSetHandel(&Atlas->ball_pos, &Delta, Atlas->ball_radius, 0.01f, 0.0f, Atlas->terrhandle);
        if (Hit & 0x10) {
            platid = TerrainPlatId();
            if (platid != -1 && ShadNorm.y >= 0.5f) {
                unembedded = UnembedRayCastAtlasSimple(Atlas, Atlas->terrhandle);
                if (!unembedded) {
                    UnembedRayCastAtlas(Atlas, Atlas->terrhandle);
                }
            } else {
                UnembedRayCastAtlas(Atlas, Atlas->terrhandle);
            }
            Delta = ToMove;
            Hit = NewRayCastSetHandel(&Atlas->ball_pos, &Delta, Atlas->ball_radius, 0.01f, 0.0f, Atlas->terrhandle);
        }
        Atlas->ball_oldpos = Atlas->ball_pos;
        NuVecSub(&ToMove, &ToMove, &Delta);
        NuVecAdd(&Atlas->ball_pos, &Atlas->ball_pos, &Delta);
        AdjustAtlasRotations(Atlas, &Delta, &Atlas->lastnormal, Atlas->ball_radius);
        Atlas->lasthit = Hit;
        if (Hit != 0) {
            struct nuvec_s norm;
            struct nuvec_s probe;
            struct nuvec_s edge;
            struct nuvec_s slide;

            norm = ShadNorm;
            Atlas->onground |= 1;
            if (norm.y * Atlas->ball_vel.y < -7.0f && Atlas->ball_vel.y < 0.0f) {
                Atlas->bigdrop = 1;
            }
            dot = DotProduct(&norm, &Atlas->ball_vel);
            NuVecScaleAccum(&Atlas->ball_vel, &norm, -dot);
            if (Atlas->type != 2 && dot < -1.0f) {
                speed = NuVecMag(&Atlas->ball_vel);
                if (fabs(dot) > BALLBUZZLIMCOS * speed) {
                    scale = Atlas->ball_radius / 0.65f;
                    vol = BALLHITVOLBASE + (1.0f - dot * scale * scale * scale) / BALLHITVOLDIV;
                    MyGameSfx(0x11, &Atlas->ball_pos, GetVolumeI(vol));
                    if (Atlas->type == 1 && vol > BALLBUZZMIN) {
                        vol = (vol - BALLBUZZMIN) / (1.0f - BALLBUZZMIN);
                        NewBuzz(&player->rumble, (s32)(BALLBUZZSLOPE * (1.0f - (1.0f - vol) * (1.0f - vol))) + 2);
                    }
                }
            }
            if (-dot > AtlasWhackValue) {
                AtlasWhackValue = -dot;
                AtlasWhackTimer = 1.0f;
            }
            dot = DotProduct(&norm, &ToMove);
            NuVecScaleAccum(&ToMove, &norm, -dot);
            if (Depth != StartDepth && dot > -0.99f && NuVecMagSqr(&Delta) < 1e-11f) {
                if (DotProduct(&norm, &Atlas->lastnormal) >= 0.0f) {
                    probe = Atlas->ball_pos;
                    NuVecScaleAccum(&probe, &norm, 0.001f);
                    if (!NewRayCastSetHandel(&probe, &NullNuVec, Atlas->ball_radius, 0.0f, 0.0f, Atlas->terrhandle)) {
                        Atlas->ball_pos = probe;
                    } else {
                        probe = Atlas->ball_pos;
                        NuVecScaleAccum(&probe, &norm, 0.0007f);
                        NuVecScaleAccum(&probe, &ShadNorm, 0.0007f);
                        if (!NewRayCastSetHandel(&probe, &NullNuVec, Atlas->ball_radius, 0.0f, 0.0f, Atlas->terrhandle)) {
                            Atlas->ball_pos = probe;
                        }
                    }
                } else {
                    CrossProduct(&edge, &norm, &Atlas->lastnormal);
                    NuVecNorm(&edge, &edge);
                    NuVecScale(&ToMove, &edge, DotProduct(&edge, &ToMove));
                    NuVecScale(&Atlas->ball_vel, &edge, DotProduct(&edge, &Atlas->ball_vel));
                    slide = Atlas->ball_pos;
                    NuVecScaleAccum(&slide, &norm, 0.0007f);
                    NuVecScaleAccum(&slide, &Atlas->lastnormal, 0.0007f);
                    if (!NewRayCastSetHandel(&slide, &NullNuVec, Atlas->ball_radius, 0.0f, 0.0f, Atlas->terrhandle)) {
                        Atlas->ball_pos = slide;
                    }
                }
            }
            Atlas->lastnormal = norm;
        }
        Depth--;
    } while (Hit != 0 && Depth != 0);
    if (Depth == 0) {
        StartDepth = (*(volatile s32 *)&tttt) + 1;
        (*(volatile s32 *)&tttt) = StartDepth;
    }
}

void CheckAtlasGround(struct NEWBUGGY *atlas)
{
    struct nuvec_s rel;
    struct nuvec_s up;
    struct nuvec_s pos;
    f32 volume;
    f32 boost;
    s32 ivolume;

    up = (struct nuvec_s){0.0f, 1.0f, 0.0f};
    pos = atlas->ball_pos;
    NuVecScaleAccum(&pos, &up, 0.01f);
    NuVecScale(&rel, &up, -0.02f);
    NewRayCastSetHandel(&pos, &rel, atlas->ball_radius, 0.0f, 0.0f, atlas->terrhandle);
    atlas->platformid = TerrainPlatId();
    atlas->platformnormal = ShadNorm;
    atlas->shadowy = NewShadowMask(&atlas->ball_pos, atlas->ball_radius, -1);
    if (atlas->shadowy == 2000000.0f || (atlas->onground & 1) == 0 ||
        0.75f <= atlas->ball_pos.y - atlas->shadowy) {
        atlas->surfacetype = 0;
    } else {
        atlas->surfacetype = ShadowInfo();
    }
    if (atlas->surfacetype == 3) {
        KillAtlasphere(atlas);
    }
    if (atlas->surfacetype == 0xB) {
        atlas->boosttimer = 1.5f;
        boost = NuFsqrt(atlas->ball_vel.x * atlas->ball_vel.x + atlas->ball_vel.z * atlas->ball_vel.z);
        if (boost != 0.0f) {
            boost = ATLASBOOSTSPEED / boost;
            MyGameSfx(0xCB, &atlas->ball_pos, ATLASBOOSTVOL);
        }
        if (boost > 1.0f) {
            atlas->ball_vel.x *= boost;
            atlas->ball_vel.z *= boost;
        }
        if (atlas->boosttimer < 0.5f) {
            atlas->boosttimer = 0.5f;
        }
    }
    if (atlas->onground & 3) {
        volume = NuVecDist(&atlas->ball_lastpos, &atlas->ball_pos, 0) * atlas->ball_radius * 10.0f;
        ivolume = GetVolumeI(volume);
        ATLASLOOPVOL = volume;
        MyGameSfxLoop(0x41, &atlas->ball_pos, ivolume);
        NewRumble(&player->rumble, BALLRUMBLESLOPE * (1.0f - (1.0f - volume) * (1.0f - volume)));
    }
}


void ProcessPlatformsAtlas(struct NEWBUGGY *atlas, f32 dt)
{
    struct nuvec_s dir;
    struct nuvec_s delta;
    struct nuvec_s move;
    struct nuvec_s local;
    struct nuvec_s moved;
    struct nuvec_s back;
    struct nuvec_s probe;
    struct nuvec_s ray;
    struct numtx_s *oldmtx;
    struct numtx_s *curmtx;
    f32 shift;
    f32 push;
    f32 scale;
    f32 mag;
    f32 len;
    s32 onplat;

    onplat = 0;
    if (AtlasFrame == 0x11E) {
        tttt++;
    }
    if (atlas->platformid != -1) {
        onplat = 1;
    } else if (NewRayCastSetHandel(&atlas->ball_pos, &v000, atlas->ball_radius, 0.01f, 0.0f, atlas->terrhandle) & 0x10) {
        onplat = 2;
        atlas->platformid = TerrainPlatId();
        atlas->platformnormal = ShadNorm;
        if (atlas->platformid == -1) {
            onplat = 0;
        }
    }
    if (onplat == 0) {
        return;
    }
    TerrainPlatGetMtx(atlas->platformid, &oldmtx, &curmtx);
    NuVecInvMtxTransform(&local, &atlas->ball_pos, oldmtx);
    NuVecMtxTransform(&moved, &local, curmtx);
    NuVecSub(&delta, &moved, &atlas->ball_pos);
    back = SetNuVec(-delta.x, -delta.y, -delta.z);
    AdjustAtlasRotations(atlas, &back, &atlas->platformnormal, atlas->ball_radius);
    shift = DotProduct(&delta, &atlas->platformnormal);
    mag = NuVecMag(&delta);
    if (shift > mag * 0.8f) {
        shift = mag;
        NuVecNorm(&dir, &delta);
    } else {
        dir = atlas->platformnormal;
    }
    if (fabs(dir.y) < 0.1f) {
        dir.y = 0.0f;
        NuVecNorm(&dir, &dir);
    }
    NuVecScale(&move, &dir, shift);
    NuVecAdd(&atlas->ball_target, &atlas->ball_target, &move);
    NuVecAdd(&atlas->ball_pos, &atlas->ball_pos, &move);
    push = NuVecMagSqr(&move);
    len = NuFsqrt(push);
    push -= DotProduct(&move, &atlas->ball_vel) * NW_HZ(0.02f, 0.016666668f) * 0.8f;
    len *= NW_HZ(0.02f, 0.016666668f);
    scale = push / len;
    if (push > 0.0f) {
        NuVecScaleAccum(&atlas->ball_vel, &move, scale);
    }
    if (onplat == 2) {
        NuVecAdd(&probe, &atlas->ball_pos, &dir);
        NuVecScale(&ray, &dir, -2.0f);
        if (NewRayCastPlatForm(&probe, &ray, atlas->ball_radius, 0.01f, atlas->platformid)) {
            NuVecAdd(&atlas->ball_pos, &probe, &ray);
        }
    }
    if (NewRayCastSetHandel(&atlas->ball_pos, &v000, atlas->ball_radius, 0.01f, 0.0f, atlas->terrhandle) & 0x10) {
        if (!UnembedRayCastAtlasSimple(atlas, atlas->terrhandle)) {
            UnembedRayCastAtlas(atlas, atlas->terrhandle);
        }
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

/* Pull the atlasphere towards the vortex when it is close to it.
 * Inline: ProcessEarthBossVortex carries two copies of it in retail. Defined
 * after BossBar so its constant is emitted with ProcessEarthBossVortex. */
inline void CheckAtlasVortex(struct NEWBUGGY *b)
{
    struct nuvec_s vortex = {0.0f, -3.3f, 0.0f};
    struct nuvec_s rel;
    f32 dist;

    if (b->ball_pos.y < -3.0f) {
        vortex.y = b->ball_pos.y - 0.3f;
    }
    NuVecSub(&rel, &vortex, &b->ball_pos);
    dist = NuVecMag(&rel);
    if (dist < 1.0f) {
        /* Retail constant: one ulp below 0.1f. */
        NuVecScaleAccum(&b->ball_vel, &rel, NW_HZ(0.099999994f, 0.083333336f) / dist);
    }
}

extern s32 EarthBossVortexOpen;

void ProcessEarthBossVortex(void)
{
    if (EarthBossVortexOpen != 0) {
        CheckAtlasVortex(&EarthBoss);
        CheckAtlasVortex(&PlayerAtlas);
    }
}

extern f32 BIGIMPACT;
extern f32 MINROKVOL;
extern s32 ROKPITCH;
extern s32 gamesfx_pitch;


void ProcessAtlasAtlasCollisions_a(struct NEWBUGGY *a, struct NEWBUGGY *b)
{
    struct nuvec_s rel;
    struct nuvec_s oldrel;
    struct nuvec_s norm;
    struct NEWBUGGY *rock;
    f32 reach;
    f32 dva;
    f32 dvb;
    f32 dist;
    f32 vb;
    f32 massa;
    f32 massb;
    f32 vol;
    s32 ivol;
    s32 neutral;

    reach = (a->ball_radius + b->ball_radius) * (a->ball_radius + b->ball_radius);
    NuVecSub(&rel, &a->ball_pos, &b->ball_pos);
    dist = NuVecMagSqr(&rel);
    if (dist < reach) {
        NuVecSub(&oldrel, &a->ball_lastpos, &b->ball_lastpos);
        if (dist < NuVecMagSqr(&oldrel)) {
            NuVecNorm(&norm, &rel);
            dist = DotProduct(&a->ball_vel, &norm);
            vb = DotProduct(&b->ball_vel, &norm);
            massa = a->ball_radius * a->ball_radius * a->ball_radius;
            massb = b->ball_radius * b->ball_radius * b->ball_radius;
            dvb = (massa * 2.0f * dist - vb * (massa - massb)) / (massa + massb) - vb;
            dva = (massb * 2.0f * vb + dist * (massa - massb)) / (massa + massb) - dist;
            NuVecScaleAccum(&a->ball_vel, &norm, dva);
            NuVecScaleAccum(&b->ball_vel, &norm, dvb);
            a->ball_beenhit = b->type;
            b->ball_beenhit = a->type;
            vol = fabs(dva) * a->ball_radius + fabs(dvb) * b->ball_radius;
            BIGIMPACT = vol;
            vol /= 10.0f;
            if (vol < MINROKVOL) {
                vol = MINROKVOL;
            }
            if (Level == 0x15) {
                if (a == &PlayerAtlas || b == &PlayerAtlas) {
                    if (a == &EarthBoss || b == &EarthBoss) {
                        ivol = GetVolumeI(vol);
                        gamesfx_pitch = ROKPITCH;
                        MyGameSfx(0xD3, &a->ball_pos, ivol);
                    } else {
                        neutral = 0;
                        if (a->rock != 0) {
                            rock = a;
                        } else if (b->rock != 0) {
                            rock = b;
                        } else {
                            rock = 0;
                        }
                        if (rock != 0) {
                            neutral = JeepRock[rock->rocknum].owner == 0;
                        }
                        if (neutral != 0) {
                            ivol = GetVolumeI(vol);
                            gamesfx_pitch = ROKPITCH;
                            MyGameSfx(0xC7, &a->ball_pos, ivol);
                        } else {
                            ivol = GetVolumeI(vol);
                            gamesfx_pitch = ROKPITCH;
                            MyGameSfx(0xD3, &a->ball_pos, ivol);
                        }
                    }
                } else if (a != &EarthBoss && b != &EarthBoss) {
                    ivol = GetVolumeI(vol);
                    gamesfx_pitch = ROKPITCH;
                    MyGameSfx(0xD3, &a->ball_pos, ivol);
                } else {
                    neutral = 0;
                    if (a->rock != 0) {
                        rock = a;
                    } else if (b->rock != 0) {
                        rock = b;
                    } else {
                        rock = 0;
                    }
                    if (rock != 0) {
                        neutral = JeepRock[rock->rocknum].owner == 0;
                    }
                    if (neutral != 0) {
                        ivol = GetVolumeI(vol);
                        gamesfx_pitch = ROKPITCH;
                        MyGameSfx(0xC7, &a->ball_pos, ivol);
                    } else {
                        ivol = GetVolumeI(vol);
                        gamesfx_pitch = ROKPITCH;
                        MyGameSfx(0xD3, &a->ball_pos, ivol);
                    }
                }
            } else {
                MyGameSfx(0x40, &a->ball_pos, GetVolumeI(vol));
            }
        }
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

    Angle = NW_EE_DIV(-DotProduct(&A, VelA), MagA * MagVelA);
    if (Angle < 0.866f) {
        return 0;
    }

    Time = NW_EE_DIV(DotProduct(&A, &A), DotProduct(&A, &AV));
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

inline void FadeOutLastTrail(s32 last, s32 count)
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
        if (trail[idx].pos[0].x == -10000.0f) {
            return;
        }
        base = trail[idx].basealpha;
        trail[idx].alpha = base * fade;
        fade += step;
    }
}

/* ------------------------------------------------------------------ */

extern struct satellite_s SatelliteList[9];

extern void MyGameSfxLoop(s32 id, struct nuvec_s *pos, s32 vol);

void ProcessSatellite(struct satellite_s *sat)
{
    s32 id;
    short vol;
    struct nuvec_s vec;

    MyAnimateModelNew(&sat->draw, NW_HZ(0.59999996f, 0.5f));
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
    SeekAngHalfLife360f(&sat->tilt[0], sat->tiltdest[0], 1.0f, NW_HZ(0.02f, 0.016666668f));
    SeekAngHalfLife360f(&sat->tilt[1], sat->tiltdest[1], 1.0f, NW_HZ(0.02f, 0.016666668f));
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

extern void NuRndrTrail(s32 pt, struct trail_s *trail, s32 count);
extern void DrawJeepTrails(void);
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
    station->satelliteangle += NW_HZ(0.59999996f, 0.5f);
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

inline void DrawZoffa(struct zoffa_s *ufo)
{
    NuMtxSetRotationZ(&mTEMP, ufo->roll * 182.0444489f);
    NuMtxRotateX(&mTEMP, -ufo->pitch * 182.0444489f);
    NuMtxRotateY(&mTEMP, (ufo->yaw + 180.0f) * 182.0444489f);
    NuMtxTranslate(&mTEMP, &ufo->pos);
    ufo->seen = NW_WIDE_SEEN(MyDrawModelNew(&ufo->draw, &mTEMP, ufo->locators));
}

extern struct zoffa_s EnemyZoffa[4];

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

void DrawZoffaDebugStuff(void)
{
    struct zoffa_s *ufo = &EnemyZoffa[0];

    NuFntSet(app_fnt);
    NuFntSetPen(0x7F7F7F7F);
    NuFntScale(0x10, 0x10);
    NuFntPrintEx(0x80, SHEIGHT * 3 / 2, 0, "Action %d", ufo->active);
    NuFntPrintEx(0x80, SHEIGHT * 2, 0, "Temp %1.3f   %1.3f", ufo->temp[0], ufo->temp[1]);
    NuFntPrintEx(0x80, SHEIGHT * 5 / 2, 0, "Temp %1.3f   %1.3f", ufo->temp[2], ufo->temp[3]);
    NuFntPrintEx(0x80, SHEIGHT * 3, 0, "Temp %1.3f   %1.3f", ufo->temp[4], ufo->temp[5]);
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


f32 GetZoffaBestTarget(f32 bestdot, struct nuvec_s **pos, struct nuvec_s **vel, s32 *type)
{
    struct nuvec_s camdir;
    struct nuvec_s rel;
    s32 i;
    f32 dot;
    f32 mag;

    NuVecMtxRotate(&camdir, SetNuVecPntr(0.0f, 0.0f, 1.0f), &GameCam[0].m);
    for (i = 0; i < 4; i++) {
        if (EnemyZoffa[i].active != 0) {
            NuVecSub(&rel, &EnemyZoffa[i].pos, &GameCam[0].pos);
            mag = NuVecMag(&rel);
            if (10.0f < mag) {
                NuVecScale(&rel, &rel, 1.0f / mag);
                dot = DotProduct(&camdir, &rel);
                if (bestdot < dot) {
                    *pos = &EnemyZoffa[i].pos;
                    bestdot = dot;
                    *vel = &EnemyZoffa[i].vel;
                    *type = 1;
                }
            }
        }
    }
    return bestdot;
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

extern f32 Level_GliderFloor;
extern f32 Level_GliderCeiling;
extern f32 Level_GliderRadius;
extern f32 Level_GliderCentreX;
extern f32 Level_GliderCentreZ;
extern void GameSfx(s32 id, struct nuvec_s *pos);
extern void NuVecMtxRotateVU0(struct nuvec_s *dst, struct nuvec_s *src, struct numtx_s *mtx);

void TeleportManager(void)
{
    struct nuvec_s point;
    struct zoffastart_s start;
    s32 i;
    s32 j;
    s32 index;

    for (i = 0; i < 4; i++) {
        if (EnemyZoffa[i].active != 0 && EnemyZoffa[i].teleport != 0) {
            for (j = 0; j < 2; j++) {
                index = (j + ZoffaTeleportIndx) & 3;
                NuVecMtxTransform(&point, &TeleportPos[index], &GameCam[0].m);
                if (point.y < Level_GliderFloor) {
                    point.y = Level_GliderFloor;
                }
                if (SafeFromCollisions(&point) != 0) {
                    start.x = point.x;
                    start.y = point.y;
                    start.z = point.z;
                    switch (index) {
                    case 0:
                        start.angle = PlayerGlider.AngleY - 135.0f;
                        break;
                    case 1:
                        start.angle = PlayerGlider.AngleY + 135.0f;
                        break;
                    case 2:
                        start.angle = PlayerGlider.AngleY - 90.0f;
                        break;
                    case 3:
                        start.angle = PlayerGlider.AngleY + 90.0f;
                        break;
                    }
                    InitZoffa(&EnemyZoffa[i], &start);
                    EnemyZoffa[i].noteleporttimer = 3.0f;
                    EnemyZoffa[i].keepsamevelocitytimer = 0.8f;
                    EnemyZoffa[i].speed = 14.0f;
                    NuVecMtxRotate(&EnemyZoffa[i].vel, &TeleportVel[(j + ZoffaTeleportIndx) & 3],
                                   &GameCam[0].m);
                    ZoffaTeleportIndx = (ZoffaTeleportIndx + 1) & 3;
                    break;
                }
            }
        }
    }
}


void MoveZoffaUFO(struct zoffa_s *ufo)
{
    struct nuvec_s rel;
    f32 turnspeed = NW_HZ(1.1999999f, 1.0f);
    s32 oldmode;
    s32 close;
    f32 oldyaw;
    f32 delta;
    f32 rollrate;

    oldmode = ufo->active;
    if (ufo->seen) {
        ufo->notseentimer = 0.0f;
    } else {
        ufo->notseentimer += NW_HZ(0.02f, 0.016666668f);
    }
    if (5.0f < ufo->notseentimer) {
        ufo->notseentimer = 5.0f;
    }
    if (ufo->infront) {
        ufo->notinfronttimer = 0.0f;
    } else {
        ufo->notinfronttimer += NW_HZ(0.02f, 0.016666668f);
    }
    if (10.0f < ufo->notinfronttimer) {
        ufo->notinfronttimer = 10.0f;
    }
    if (--ufo->nofiresound < 0) {
        ufo->nofiresound = 0;
    }
    ProcessTimer(&ufo->keepsamevelocitytimer);
    ufo->draw.Anim.oldaction = ufo->draw.Anim.action;
    MyAnimateModelNew(&ufo->draw, NW_HZ(0.59999996f, 0.5f));
    ufo->playercloseandvisible = 0;
    ProcessTimer(&ufo->noteleporttimer);
    if (ufo->fireframe != 0) {
        ufo->fireframe--;
    }
    ufo->firetimer -= NW_HZ(0.02f, 0.016666668f);
    if (ufo->firetimer < 0.0f) {
        ufo->firetimer = 0.0f;
    }
    CollideGliderBullets(&ufo->pos, 1.0f, 1, 1.0f, 0, 0);
    if (Level == 0x12) {
        if (CollideGliderBombs(&ufo->pos, SetNuVecPntr(1.0f, 1.0f, 1.0f), 0, 0.0f)) {
            ufo->explode = 1;
            ufo->respawntimer = 40.0f;
        }
    } else if (CollideGliderBullets(&ufo->pos, 1.0f, 0, 1.0f, 0, 0)) {
        if (Level == 0x1A) {
            MyGameSfx(0xD3, &ufo->pos, 0x3FFF);
        } else {
            GameSfx(0x8C, 0);
        }
        AddScreenWumpa(ufo->pos.x, ufo->pos.y, ufo->pos.z, 1);
        ufo->keepsamevelocitytimer = 0.0f;
        if (--ufo->hitpoints == 0) {
            ufo->terminaldive = 1;
        }
        if (ufo->hitpoints < -5 || (ufo->hitpoints <= 0 && Level == 0x1A)) {
            ufo->explode = 1;
            ufo->respawntimer = (Level == 0x1A) ? 35.0f : 40.0f;
        }
        if (ufo->terminaldive == 0) {
            ufo->aggressiontimer = 20.0f;
            CurrentAggressor = ufo;
            ufo->active = 2;
            ufo->visibletimer = 20.0f;
        }
    }
    if (ufo->explode || (ufo->terminaldive && Level == 0x1A)) {
        AddGameDebris(0x18, &ufo->pos);
        ufo->active = 0;
        if (Level == 0x12) {
            MyGameSfx(0xC7, &ufo->pos, 0x5FFF);
        } else if (Level == 0x1A) {
            MyGameSfx(UFOEXPLODEID, &ufo->pos, 0x7FFF);
        } else {
            MyGameSfx(0x56, &ufo->pos, 0x7FFF);
        }
        ufo->pos.y = 200000.0f;
        return;
    }
    ZoffaSmoke(ufo);
    if (ufo->terminaldive) {
        ufo->destpitch += NW_HZ(0.19999999f, 0.16666667f);
        if (45.0f < ufo->destpitch) {
            ufo->destpitch = 45.0f;
        }
        if (ufo->terminaldive == 2) {
            ufo->terminaldive = (Rationalise360f(ufo->destyaw - ufo->yaw) < 0.0f) ? -1 : 1;
        }
        ufo->destyaw += (f32)ufo->terminaldive * turnspeed * 0.3f;
    } else if (ufo->active == 3) {
        f32 turnleft;

        ufo->visibletimer = 0.0f;
        turnleft = Rationalise360f(ufo->destyaw - ufo->yaw);
        if (-1.0f <= turnleft && turnleft < 1.0f) {
            ufo->active = 1;
        }
    } else {
        if (PlayerGlider.TerminalDive == 0 && PlayerGlider.Dead == 0 && PlayerGlider.AutoPilot == 0 &&
            !(0.0f < ufo->keepsamevelocitytimer)) {
            f32 horiz;
            f32 yawto;
            f32 pitchoff;
            f32 yawoff;
            s32 lenient;

            NuVecSub(&rel, &PlayerGlider.ApparentPosition, &ufo->pos);
            yawto = NuAtan2D(rel.x, rel.z) / 182.04445f;
            horiz = NuFsqrt(rel.x * rel.x + rel.z * rel.z);
            pitchoff = NuAtan2D(-rel.y, horiz) / 182.04445f;
            pitchoff = (f32)fabs(Rationalise360f(pitchoff - ufo->pitch));
            yawoff = (f32)fabs(Rationalise360f(yawto - ufo->yaw));
            lenient = 0;
            if (yawoff < 90.0f && pitchoff < 40.0f) {
                lenient = 1;
            }
            if (Level == 0x1A) {
                lenient = 1;
            }
            close = (horiz < (lenient ? 180.0f : 90.0f));
        } else {
            close = 0;
        }
        if (close) {
            ufo->active = 2;
            ufo->visibletimer = 12.0f;
            CurrentAggressor = ufo;
        } else {
            ufo->visibletimer -= NW_HZ(0.02f, 0.016666668f);
            if (ufo->visibletimer <= 0.0f) {
                ufo->visibletimer = 0.0f;
                ufo->active = 1;
            }
        }
        if (ufo->active == 1) {
            if (oldmode == 2) {
                ufo->newaltitudetimer = frand() * frand() * 4.0f + 1.0f;
                ufo->newaltitudetarget = -100.0f;
            }
            if (ufo->keepsamevelocitytimer == 0.0f) {
                if (ProcessTimer(&ufo->newaltitudetimer)) {
                    ufo->newaltitudetimer = frand() * 8.0f + 6.0f;
                    ufo->newaltitudetarget = Level_GliderFloor + 3.0f +
                                             frand() * (Level_GliderCeiling - Level_GliderFloor - 6.0f);
                }
                if (ufo->newaltitudetarget == -100.0f) {
                    ufo->destpitch = 0.0f;
                } else {
                    ufo->destpitch = -(ufo->newaltitudetarget - ufo->pos.y) * 8.0f;
                }
                if (22.5f < ufo->destpitch) {
                    ufo->destpitch = 22.5f;
                } else if (ufo->destpitch < -22.5f) {
                    ufo->destpitch = -22.5f;
                }
                if (ProcessTimer(&ufo->newdirectiontimer)) {
                    ufo->newdirectiontimer = frand() * 8.0f + 7.0f;
                    if (frand() < 0.5f) {
                        ufo->destyaw += frand() * 130.0f + 45.0f;
                    } else {
                        ufo->destyaw -= frand() * 130.0f + 45.0f;
                    }
                }
            }
            {
                struct nuvec_s *pos = &ufo->pos;
                f32 dx = pos->x - Level_GliderCentreX;
                f32 dz = pos->z - Level_GliderCentreZ;

                if (Level_GliderRadius * Level_GliderRadius * 0.81f < dx * dx + dz * dz) {
                    f32 centreyaw = NuAtan2D(dx, dz) / 182.04445f + 180.0f;
                    f32 offcentre = Rationalise360f(centreyaw - ufo->destyaw);

                    ufo->newdirectiontimer = frand() * 8.0f + 10.0f;
                    if (90.0f < offcentre || offcentre < -90.0f) {
                        ufo->destyaw = centreyaw - 45.0f + frand() * 90.0f;
                    }
                }
            }
        } else {
            struct nuvec_s diff;
            f32 dist;

            rel = PlayerGlider.ApparentPosition;
            dist = NuVecDist(&rel, &ufo->pos, &diff);
            NuVecScaleAccum(&rel, &PlayerGlider.ApparentVelocity, dist / 80.0f);
            ufo->destyaw = NuAtan2D(rel.x - ufo->pos.x, rel.z - ufo->pos.z) / 182.04445f;
            if (close && ufo->facingtarget) {
                f32 horiz = NuFsqrt(diff.x * diff.x + diff.z * diff.z);

                ufo->destpitch = Rationalise360f(NuAtan2D(-diff.y, horiz) / 182.04445f);
                if (ufo->seen) {
                    ufo->firetimer = 1.5f;
                }
            } else {
                ufo->destpitch = -diff.y * 4.0f;
                if (ufo->pos.y < Level_GliderFloor && 0.0f < ufo->destpitch) {
                    ufo->destpitch = 0.0f;
                }
            }
            if (40.0f < ufo->destpitch) {
                ufo->destpitch = 40.0f;
            } else if (ufo->destpitch < -40.0f) {
                ufo->destpitch = -40.0f;
            }
        }
    }
    oldyaw = ufo->yaw;
    delta = Rationalise360f(ufo->destyaw - oldyaw);
    if (turnspeed < delta) {
        ufo->yaw += turnspeed;
    } else if (delta < -turnspeed) {
        ufo->yaw -= turnspeed;
    } else {
        ufo->yaw = ufo->destyaw;
    }
    rollrate = Rationalise360f(ufo->yaw - oldyaw) / NW_HZ(0.02f, 0.016666668f);
    if (rollrate == 0.0f) {
        SeekHalfLife(&ufo->roll, rollrate, 0.6f, NW_HZ(0.02f, 0.016666668f));
    } else {
        SeekHalfLife(&ufo->roll, rollrate, 0.3f, NW_HZ(0.02f, 0.016666668f));
    }
    SeekHalfLife(&ufo->pitch, ufo->destpitch, 0.6f, NW_HZ(0.02f, 0.016666668f));
    if (ufo->infront) {
        if (Level == 0x1A) {
            SeekHalfLife(&ufo->speed, 18.0f, 0.8f, NW_HZ(0.02f, 0.016666668f));
        } else {
            SeekHalfLife(&ufo->speed, 14.0f, 0.8f, NW_HZ(0.02f, 0.016666668f));
        }
    } else {
        SeekHalfLife(&ufo->speed, 10.0f, 0.8f, NW_HZ(0.02f, 0.016666668f));
    }
    {
        {
            struct numtx_s m;

            rel = SetNuVec(0.0f, 0.0f, ufo->speed);
            NuMtxSetRotationZ(&m, ufo->roll * 182.04445f);
            NuMtxRotateX(&m, ufo->pitch * 182.04445f);
            NuMtxRotateY(&m, ufo->yaw * 182.04445f);
            NuVecMtxRotate(&ufo->vel, &rel, &m);
        }
    }
    ufo->pos.x += ufo->vel.x * NW_HZ(0.02f, 0.016666668f);
    ufo->pos.y += ufo->vel.y * NW_HZ(0.02f, 0.016666668f);
    ufo->pos.z += ufo->vel.z * NW_HZ(0.02f, 0.016666668f);
    if (Level_GliderCeiling - 3.0f < ufo->pos.y) {
        ufo->pos.y = Level_GliderCeiling - 3.0f;
    }
    if (ufo->pos.y < Level_GliderFloor + 3.0f && ufo->terminaldive == 0) {
        ufo->pos.y = Level_GliderFloor + 3.0f;
    }
    if (PlayerGlider.AutoPilot || FlyingLevelExtro || PlayerGlider.TerminalDive || PlayerGlider.ForceTurn ||
        0.0f < ufo->keepsamevelocitytimer || ufo->terminaldive) {
        ufo->firenow = 0;
        ufo->firebursttimer = 0.75f;
        ufo->infront = 0;
        ufo->facingtarget = 0;
    } else {
        f32 firerange;
        s32 selfcount;
        f32 pitchoff;
        f32 yawoff;
        s32 facing;
        s32 infront;

        ufo->dist = NuVecDist(&PlayerGlider.ApparentPosition, &ufo->pos, &rel);
        pitchoff = Rationalise360f(ufo->pitch - ufo->destpitch);
        yawoff = Rationalise360f(ufo->yaw - ufo->destyaw);
        facing = 0;
        if (-15.0f < pitchoff && pitchoff < 15.0f && -10.0f < yawoff && yawoff < 10.0f) {
            facing = 1;
        }
        ufo->facingtarget = facing;
        infront = (DotProduct(&PlayerGlider.Velocity, &rel) <= 0.0f) ? 1 : 0;
        selfcount = ufo->firenow - 1;
        firerange = ufo->dist * (f32)(LastNumZoffasFiring - selfcount);
        ufo->infront = infront;
        if (ufo->seen) {
            if (ufo->facingtarget && firerange < 120.0f) {
                ufo->firenow = 1;
            } else {
                ufo->firenow = 0;
            }
        } else if (ufo->facingtarget && infront && firerange < 60.0f) {
            ufo->firenow = 1;
        } else if (!ufo->seen && ufo->facingtarget && ufo->notinfronttimer < 10.0f && firerange < 30.0f) {
            ufo->firenow = 1;
        } else {
            ufo->firenow = 0;
        }
        if (Level == 0xD && ufo->infront == 0) {
            ufo->firenow = 0;
        }
        NumZoffasFiring += ufo->firenow;
        ufo->teleport = (5.0f <= ufo->notseentimer && ufo->firenow == 0 && ufo->noteleporttimer == 0.0f);
    }
    if (ufo->firenow) {
        if (ufo->seen || (Level != 0xD && Level != 0x12)) {
            ufo->lockedontimer += NW_HZ(0.02f, 0.016666668f);
        }
        if (0.25f <= ufo->lockedontimer) {
            ufo->lockedontimer = 0.25f;
        }
    } else {
        ufo->lockedontimer = 0.0f;
    }
    if (Level == 0x1A) {
        if (ufo->firenow) {
            if (ProcessTimer(&ufo->firebursttimer)) {
                ufo->firebursttimer = 0.75f;
            }
        } else {
            ufo->firebursttimer += NW_HZ(0.02f, 0.016666668f);
            if (0.75f < ufo->firebursttimer) {
                ufo->firebursttimer = 0.75f;
            }
        }
        if (ufo->firebursttimer < 0.5f) {
            ufo->firenow = 0;
        }
    }
    if (ufo->firenow && ufo->fireframe == 0) {
        if (Level == 0x1A) {
            struct numtx_s firemtx;
            struct nuvec_s dir;
            struct nuvec_s firepos;

            NuMtxSetRotationZ(&firemtx, ufo->roll * 182.04445f);
            NuMtxRotateX(&firemtx, -ufo->pitch * 182.04445f);
            NuMtxRotateY(&firemtx, (ufo->yaw + 180.0f) * 182.04445f);
            NuVecMtxRotateVU0(&dir, SetNuVecPntr(0.0f, -ZoffaAlter, -80.0f), &firemtx);
            NuVecMtxRotateVU0(&firepos, &ZoffaFirePosA, &firemtx);
            NuVecAdd(&firepos, &firepos, &ufo->pos);
            NuVecScaleAccum(&firepos, &dir, -NW_HZ(0.02f, 0.016666668f));
            AddGliderBullet(&firemtx, &firepos, &dir, 1);
            NuVecMtxRotateVU0(&firepos, &ZoffaFirePosB, &firemtx);
            NuVecAdd(&firepos, &firepos, &ufo->pos);
            NuVecScaleAccum(&firepos, &dir, -NW_HZ(0.02f, 0.016666668f));
            AddGliderBullet(&firemtx, &firepos, &dir, 1);
            ufo->fireframe = 3;
            if (ufo->nofiresound <= 0) {
                ufo->nofiresound = 9;
                MyGameSfx(0xCF, &firepos, 0x3FFF);
            }
        } else {
            struct numtx_s firemtx;
            struct nuvec_s spread;
            struct nuvec_s offset;

            NuMtxSetRotationZ(&firemtx, ufo->roll * 182.04445f);
            NuMtxRotateX(&firemtx, -ufo->pitch * 182.04445f);
            NuMtxRotateY(&firemtx, (ufo->yaw + 180.0f) * 182.04445f);
            spread = SetNuVec((frand() - 0.25f) * 6.0f, (frand() - 0.5f) * 6.0f - ZoffaAlter, -80.0f);
            offset = SetNuVec(0.15f, 0.0f, -0.65f);
            if (ufo->fireside & 1) {
                offset.x = -offset.x;
                spread.x = -spread.x;
            }
            ufo->fireside++;
            NuVecMtxRotateVU0(&spread, &spread, &firemtx);
            NuVecMtxRotateVU0(&offset, &offset, &firemtx);
            NuVecAdd(&offset, &offset, &ufo->pos);
            NuMtxScale(&firemtx, SetNuVecPntr(0.8f, 0.8f, 0.8f));
            if (AddGliderBullet(&firemtx, &offset, &spread, 1)) {
                ufo->fireframe = 5;
                GameSfx(0x60, &offset);
            }
        }
    }
    if (Level == 0x1A) {
        MyGameSfxLoopVolPitch(0xD1, &ufo->pos, (short)CRASHTEROIDSZOFFAVOL,
                              (s16)(OIDZOFPITCH + (s32)(OIDZOFPITCHSLOPE * ufo->pitch)));
        if (Level == 0x1A) {
            AddVariableShotDebrisEffect(GDeb[160].i, (struct nuvec_s *)&ufo->locators[2]._30, 1, 0, 0);
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
    if (jonemit >= NW_HZ(150, 180)) {
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
    MyAnimateModelNew(&hova->draw, NW_HZ(0.59999996f, 0.5f));
    SeekHalfLifeNUVEC(&hova->vel, &hova->targetvel, 1.0f, NW_HZ(0.02f, 0.016666668f));
    newpos = hova->pos;
    maxdist2 = (Level_GliderRadius - 15.0f) * (Level_GliderRadius - 15.0f);
    NuVecScaleAccum(&newpos, &hova->vel, NW_HZ(0.02f, 0.016666668f));
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
        hova->vel.y += NW_HZ(0.06f, 0.050000004f);
    } else {
        hova->vel.y -= NW_HZ(0.12f, 0.10000001f);
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
    hova->pos.y += hova->vel.y * NW_HZ(0.02f, 0.016666668f);
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
            b->life = NW_HZ(100, 120);
        } else if (Level == 0xD) {
            b->life = NW_HZ(75, 90);
        } else if (Level == 0x1A) {
            b->life = NW_HZ(37, 45);
        } else {
            b->life = NW_HZ(150, 180);
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
        b->vel.x = vel->x * NW_HZ(0.02f, 0.016666668f);
        b->vel.y = vel->y * NW_HZ(0.02f, 0.016666668f);
        b->vel.z = vel->z * NW_HZ(0.02f, 0.016666668f);
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

void GliderBulletsHitThings(struct GLIDERSTRUCT *glider)
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

void GliderBombsHitThings(struct GLIDERSTRUCT *glider)
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
    asteroid->spinx = (frand() - 0.5f) * 72.0f * NW_HZ(0.02f, 0.016666668f);
    asteroid->spiny = (frand() - 0.5f) * 72.0f * NW_HZ(0.02f, 0.016666668f);
    asteroid->stealth = stealth;
    asteroid->active = 1;
    asteroid->hitpoints = 3;
    asteroid->scale = SetNuVec(2.0f, 2.0f, 2.0f);
    NuVecScale(&asteroid->vel, &asteroid->vel, NW_HZ(0.02f, 0.016666668f));
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
        struct nuvec_s offset;
        struct nuvec_s dirs[3];

        NuVecScale(&asteroid->scale, &asteroid->scale, 0.5f);
        NuVecSub(&scale, SetNuVecPntr(2.0f, 2.0f, 2.0f), &asteroid->scale);
        NuMtxSetScale(&mTEMP, &scale);
        NuMtxRotateY(&mTEMP, frand() * 360.0f * 182.0444489f);
        NuMtxRotateX(&mTEMP, frand() * 360.0f * 182.0444489f);
        dirs[0] = SetNuVec(0.0f, 0.0f, NW_HZ(0.099999994f, 0.083333336f));
        dirs[1] = SetNuVec(0.0f, NW_HZ(0.099999994f, 0.083333336f), 0.0f);
        dirs[2] = SetNuVec(NW_HZ(0.099999994f, 0.083333336f), 0.0f, 0.0f);
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
            NuVecScaleAccum(&piece->pos, &piece->vel, NW_HZ(25.0f, 29.999998f));
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


extern s32 VehicleLevelImmune;
extern f32 Level_DeadTime;

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
    static struct nuvec_s NullVec = {0.0f, 0.0f, 0.0f};
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

        GetGliderCollPoints(outline);
        points[5] = outline[0];
        for (i = 0; i < 6; i++) {
            if (HovaBlimp[i].active == 0
                || !SphereCollideDiff(&HovaBlimp[i].pos, &PlayerGlider.Position, BigRad, 1.0f, &diff2)) {
                continue;
            }
            if (LinesIntersectEllipse(outline, 5, &HovaBlimp[i].pos, HovaRad + 3.0f, HovaY)) {
                hit = 1;
            } else {
                diff = HovaBlimp[i].pos;
                diff.y += PropYOff;
                hit = LinesIntersectEllipse(outline, 5, &diff, PropRad + 3.0f, PropY) != 0;
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
            && (NewRayCast(&EnemyZoffa[ZoffaCollisionCounter].pos, &NullVec, 0.8f)
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

struct hail_s {
    s32 state;                 /* 0x00 */
    f32 timer;                 /* 0x04 */
    f32 fallx;                 /* 0x08 */
    struct nuvec_s pos;        /* 0x0C */
    struct nuvec_s vel;        /* 0x18 */
};
extern struct hail_s HailList[30];
extern struct numtx_s BoltMtxC;

inline void InitLighteningHail(void)
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
extern s32 SkipLeftA;
extern s32 SkipLeftB;
extern s32 SkipLeftC;
extern s32 SkipNum;

/* Defined here: retail emits them after ProcessBigGun's Val. */
static struct nuvec_s BattleShipCollScale = {2.5f, 6.5f, 6.0f};
s32 BSTab[3] = {5, 2, 10};
struct nuvec_s BossCollScale = {5.0f, 7.0f, 5.0f};
struct nuvec_s WBMASKONHIGH = {0.0f, 0.0f, 0.0f};
struct nuvec_s WBAKUMASKONHIGH = {3.0f, 3.0f, 0.0f};
struct nuvec_s WBMASKLEFT = {0.0f, 0.0f, 10.0f};
struct nuvec_s WBMASKRIGHT = {0.0f, 0.0f, 10.0f};
struct nuvec_s WBMASKEYES = {2.0f, -3.0f, 7.0f};
struct nuvec_s WBMASKCHEST = {0.0f, 0.0f, 10.0f};
struct nuvec_s WBLeftFirePos = {-0.35f, -0.45f, 0.0f};
struct nuvec_s TargetTemp = {-3.5f, 2.5f, 10.0f};
struct nuvec_s FireFlyStart = {20.0f, 18.0f, 20.0f};
struct nuvec_s WRStartPos = {80.0f, 36.0f, 112.0f};
struct nuvec_s CrashteroidsStart = {20.0f, 18.0f, 20.0f};
struct nuvec_s LighteningHailVel = {60.0f, 120.0f, 0.0f};
struct nuvec_s RockPanel = {-0.18f, 0.8f, 1.0f};
struct nuvec_s RockPanelOff = {0.12f, 0.0f, 0.0f};
struct numtx_s GliderInvMtx = {0};
struct nuvec_s GliderIntroInterest = {0.0f, 0.0f, 0.0f};
struct nuvec_s GliderIntroCamPos = {0.0f, 0.0f, 0.0f};
struct nuvec_s RumbleCamPos = {0.0f, 0.0f, 0.0f};
struct splinepath_s WeatherBossCamSpline = {0};
struct splinepath_s WeatherBossIntroSpline = {0};
struct nuvec_s WBIntroGliderPos = {0.0f, 0.0f, 0.0f};
s32 jontorn[20] = {0};
struct nuvec_s GliderBulletReflectPos = {0.0f, 0.0f, 0.0f};
struct nuvec_s GliderTargetPos = {0.0f, 0.0f, 0.0f};
struct nuvec_s GliderBombCollisionPos = {0.0f, 0.0f, 0.0f};
struct nuvec_s WeatherBossFirePoint = {0.0f, 0.0f, 0.0f};
struct gliderbomb_s GliderBombs[10] = {0};
struct GLIDERSTRUCT PlayerGlider = {0};
struct nuvec_s WeatherBossTargetPos = {0.0f, 0.0f, 0.0f};
struct nuvec_s WeatherBossTargetVel = {0.0f, 0.0f, 0.0f};
struct numtx_s WeatherBossCamMtx = {0};
struct nuvec_s WeatherBossMovePoint = {0.0f, 0.0f, 0.0f};
struct zoffa_s EnemyZoffa[4] = {0};
struct hovablimp_s HovaBlimp[6] = {0};
struct tornado_s TornadoList[6] = {0};
struct satellite_s SatelliteList[9] = {0};
struct spacestation_s SpaceStationList[3] = {0};
struct gliderbullet_s GliderBullet[64] __attribute__((aligned(16))) = {0};
struct gliderbullet_s EnemyGliderBullet[100] = {0};
struct gliderbullet_s *FreeGliderBulletList[64] = {0};
struct gliderbullet_s *UsedGliderBulletList[64] = {0};
struct gliderbullet_s *FreeEnemyGliderBulletList[100] = {0};
struct gliderbullet_s *UsedEnemyGliderBulletList[100] = {0};
struct plane_s PlayerPlane = {0};
struct nuvec_s ExtroStillCamPos = {0.0f, 0.0f, 0.0f};
struct nuvec_s ExtroStillInterest = {0.0f, 0.0f, 0.0f};
struct nuvec_s ExtroStillVelocity = {0.0f, 0.0f, 0.0f};
struct asteroid_s AsteroidList[100] = {0};
struct asteroid_s *ActiveAsteroidList[100] = {0};
struct gliderbomb_s EnemyGliderBombs[30] = {0};
struct gunboat_s GunBoatList[4] = {0};
struct biggun_s BigGunList[12] = {0};
struct battleship_s BattleShipList[6] = {0};
struct nuvec_s BazookaTokenPos = {0.0f, 0.0f, 0.0f};
struct nuvec_s BazookaTokenCurrentPos = {0.0f, 0.0f, 0.0f};
struct weatherboss_s WeatherBoss = {0};
struct MYDRAW IconMainDraw = {0};
struct nuvec_s ElectricalPosition = {0.0f, 0.0f, 0.0f};
struct nuvec_s BossHitLoc = {0.0f, 0.0f, 0.0f};
struct hail_s HailList[30] = {0};
struct numtx_s BoltMtxC = {0};
struct numtx_s BoltMtxA = {0};
struct numtx_s BoltMtxB = {0};
struct wbbolt_s BoltList[120] = {0};
struct nuvec_s AmbientPosition = {0.0f, 0.0f, 0.0f};
struct jeeprock_s JeepRock[6] = {0};
struct NEWBUGGY PlayerAtlas = {0};
struct NEWBUGGY EarthBoss = {0};
struct nuvec_s DebugTemp = {0.0f, 0.0f, 0.0f};
struct nuvec_s ebpos[3] = {0};
f32 ebtime[3] = {0};
struct nuvec_s EarthBossAimLocation = {0.0f, 0.0f, 0.0f};
struct nuvec_s EarthBossAimVelocity = {0.0f, 0.0f, 0.0f};
struct nuvec_s RockPanelData[4] = {0};
struct nuvec_s RockPanelDest[4] = {0};
s32 RockPanelObj[4] = {0};
struct trail_s trail[128] = {0};
struct vehmask_s VehicleMask[2] = {0};

/* Fires the 30 hailstones of attack `attnum` in a fan, leaving a gap of
   SkipNum stones after stone SkipLeft[attnum]. */
void UnleashLighteningHail(struct nuvec_s *pos, s32 attnum)
{
    struct hail_s *hail;
    struct nuvec_s vel;
    f32 fallxmin;
    f32 upspeed;
    f32 timer;
    f32 fallx;
    f32 timeinc;
    f32 xinc;
    f32 fallxinc;
    f32 count;
    s32 skipleft;
    s32 i;

    upspeed = HAILFLYUPSPEED * NW_HZ(0.02f, 0.016666668f);
    memset(HailList, 0, sizeof(HailList));
    NuVecScale(&vel, &LighteningHailVel, NW_HZ(0.02f, 0.016666668f));
    timer = 2.0f;
    fallxmin = LighteningFallXMin;
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
    fallxinc = (fallxmin - fallx) / count;
    xinc = -vel.x * 2.0f / count;
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
            fallx += fallxinc;
            timer += timeinc;
            vel.x += xinc;
        } else {
            vel.x += xinc * SkipNum;
            fallx += fallxinc * SkipNum;
            timer += timeinc * SkipNum;
        }
    }
}

extern f32 HAILHIDDENTIME;
extern f32 HailFallSpeed;
extern f32 HAILCOLLRAD;

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
                    hail->vel.y = -HailFallSpeed * NW_HZ(0.02f, 0.016666668f);
                    hail->vel.z = -Level_GliderSpeed * NW_HZ(0.02f, 0.016666668f);
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
                    SeekHalfLifeNUVEC(&bolt->vel, &temp, BOLTHOMESEEKTIME, NW_HZ(0.02f, 0.016666668f));
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

extern f32 RockPanelEdge;
extern f32 RockPanelSpacing;
extern f32 RockPanelSlideSpeed;
extern u16 RockPanelRotX[4];
extern u16 RockPanelRotY[4];
extern u16 RockPanelRotZ[4];
extern u16 RockPanelRotIncX[4];
extern u16 RockPanelRotIncY[4];
extern u16 RockPanelRotIncZ[4];
extern f32 RumbleDisplayY;
extern f32 DISPLAYYSPEED;


void ProcessRumblePanel(void)
{
    s32 playerroks;
    s32 crunchroks;
    s32 freeroks;
    s32 n;
    s32 i;
    s32 axis;
    f32 BlueStart;
    f32 GreyStart;
    f32 RedStart;

    playerroks = GetRumblePlayerRoks();
    crunchroks = GetRumbleCrunchRoks();
    freeroks = GetRumbleTotalRoks() - playerroks - crunchroks;
    n = 0;
    for (axis = 0; axis < 4; axis++) {
        RockPanelRotX[axis] += RockPanelRotIncX[axis];
        RockPanelRotY[axis] += RockPanelRotIncY[axis];
        RockPanelRotZ[axis] += RockPanelRotIncZ[axis];
    }
    if (playerroks > 0) {
        BlueStart = -RockPanelEdge;
        for (i = 0; i < playerroks; i++) {
            RockPanelDest[n].x = BlueStart + i * RockPanelSpacing;
            RockPanelDest[n].y = 0.8f;
            RockPanelDest[n].z = 1.0f;
            RockPanelObj[n] = 0xA9;
            n++;
        }
    }
    if (freeroks > 0) {
        GreyStart = (1.0f - freeroks) * RockPanelSpacing * 0.5f;
        for (i = 0; i < freeroks; i++) {
            RockPanelDest[n].x = GreyStart + i * RockPanelSpacing;
            RockPanelDest[n].y = 0.8f;
            RockPanelDest[n].z = 1.0f;
            RockPanelObj[n] = 0xAA;
            n++;
        }
    }
    if (crunchroks > 0) {
        RedStart = RockPanelEdge - crunchroks * RockPanelSpacing;
        for (i = 0; i < crunchroks; i++) {
            RockPanelDest[n].x = RedStart + i * RockPanelSpacing;
            RockPanelDest[n].y = 0.8f;
            RockPanelDest[n].z = 1.0f;
            RockPanelObj[n] = 0xAB;
            n++;
        }
    }
    NumRockPanel = n;
    for (i = 0; i < NumRockPanel; i++) {
        SeekHalfLife(&RockPanelData[i].x, RockPanelDest[i].x, RockPanelSlideSpeed, NW_HZ(0.02f, 0.016666668f));
        RockPanelData[i].y = RockPanelDest[i].y;
        RockPanelData[i].z = RockPanelDest[i].z;
    }
    if (RumbleDisplayMode == 2) {
        for (i = 0; i < NumRockPanel; i++) {
            RockPanelData[i] = RockPanelDest[i];
        }
        RumbleDisplayMode = 1;
        RumbleDisplayY = 1.2f;
    }
    if (RumbleDisplayMode == 1) {
        RumbleDisplayY -= DISPLAYYSPEED * NW_HZ(0.02f, 0.016666668f);
        if (RumbleDisplayY < 0.8f) {
            RumbleDisplayY = 0.8f;
            RumbleDisplayMode = 0;
        }
    }
    if (RumbleDisplayMode == -1) {
        RumbleDisplayY += DISPLAYYSPEED * NW_HZ(0.02f, 0.016666668f);
        if (RumbleDisplayY > 1.2f) {
            RumbleDisplayY = 1.2f;
            RumbleDisplayMode = 0;
        }
    }
}


extern f32 RockPanelScale;
extern f32 RADARSCALE;
extern f32 RADARSCALEX;
extern f32 RADARBASESCALE;
extern f32 DOTSCALE;
extern f32 RadarX;
extern f32 RadarY;
extern f32 RadarZ;
extern f32 FindNearestCreature(struct nuvec_s *pos, s32 type, struct nuvec_s *out);


void RumbleHeadUpDisplay(void)
{
    struct nuvec_s pos;
    struct nuvec_s nearest;
    f32 scale;
    s16 angle;
    s32 panel;
    s32 obj;
    s32 i;

    for (i = 0; i < NumRockPanel; i++) {
        panel = RockPanelObj[i];
        DrawPanel3DObject(panel, RockPanelData[i].x, RumbleDisplayY, RockPanelData[i].z,
                          RockPanelScale, RockPanelScale, RockPanelScale,
                          RockPanelRotX[i], RockPanelRotY[i], RockPanelRotZ[i],
                          ObjTab[panel].obj.scene, ObjTab[panel].obj.special, 1);
    }
    pos = v000;
    scale = RADARBASESCALE * RADARSCALE;
    angle = -GameCam[0].yrot;
    DrawPanel3DObject(0xA2, pos.x * RADARSCALE + RadarX, pos.z * RADARSCALE + RadarY, RadarZ,
                      scale, scale, scale, 0, 0, 0, ObjTab[0xA2].obj.scene, ObjTab[0xA2].obj.special, 1);
    scale = PlayerAtlas.ball_radius * RADARSCALE * DOTSCALE;
    NuVecRotateY(&pos, &PlayerAtlas.ball_pos, angle);
    DrawPanel3DObject(0xA5, pos.x * RADARSCALEX + RadarX, pos.z * RADARSCALE + RadarY, RadarZ,
                      scale, scale, scale, 0, 0, 0, ObjTab[0xA5].obj.scene, ObjTab[0xA5].obj.special, 1);
    if (EarthBoss.dead == 0) {
        scale = EarthBoss.ball_radius * RADARSCALE * DOTSCALE;
        NuVecRotateY(&pos, &EarthBoss.ball_pos, angle);
        DrawPanel3DObject(0xA3, pos.x * RADARSCALEX + RadarX, pos.z * RADARSCALE + RadarY, RadarZ,
                          scale, scale, scale, 0, 0, 0, ObjTab[0xA3].obj.scene, ObjTab[0xA3].obj.special, 1);
    }
    scale = RADARSCALE * 0.5f * DOTSCALE;
    for (i = 0; i < 6; i++) {
        if (JeepRock[i].exists != 0) {
            NuVecRotateY(&pos, &JeepRock[i].pos, angle);
            switch (JeepRock[i].owner) {
            case 1:
            case 0x14:
                obj = 0xA6;
                break;
            case -1:
                obj = 0xA4;
                break;
            default:
                obj = 0xA7;
                break;
            }
            DrawPanel3DObject(obj, pos.x * RADARSCALEX + RadarX, pos.z * RADARSCALE + RadarY, RadarZ,
                              scale, scale, scale, 0, 0, 0, ObjTab[obj].obj.scene, ObjTab[obj].obj.special, 1);
        }
    }
    if (FindNearestCreature(&player->obj.pos, 0xA7, &nearest) < 1000000.0f) {
        obj = 0xA4;
        scale = (GameTimer.frame % NW_HZ(25, 30) + NW_HZ(25, 30)) * RADARSCALE * DOTSCALE / NW_HZ(50.0f, 60.0f);
        NuVecRotateY(&nearest, &nearest, angle);
        DrawPanel3DObject(obj, nearest.x * RADARSCALEX + RadarX, nearest.z * RADARSCALE + RadarY, RadarZ,
                          scale, scale, scale, 0, 0, 0, ObjTab[obj].obj.scene, ObjTab[obj].obj.special, 1);
    }
}

extern s16 RumbleCamY;
extern struct nuvec_s D_005B7C80;
extern struct nuvec_s D_005B8008;
#define playeratlas_interest D_005B7C80 /* PlayerAtlas interest point (+0x378) */
#define earthboss_interest D_005B8008   /* EarthBoss interest point (+0x378) */


/* Defined here: retail emits them before RumbleCam's AngleY. */
f32 FORMPITCH = 1280.0f;
f32 ROKDEATHTIME = 0.5f;
f32 FORMSOUNDTIME = 1.0f;
f32 RUMZOOM = 0.65f;
s32 ROKBPITCH = 768;
s32 BOSSLANDVOL = 32767;
s32 BIGLANDVOL = 32767;
s32 NumRockPanel = 4;
f32 RockPanelScale = 0.06f;
f32 DISPLAYYSPEED = 0.4f;
f32 RockPanelEdge = 0.5f;
f32 RockPanelSpacing = 0.12f;
f32 RockPanelSlideSpeed = 0.1f;
u16 RockPanelRotIncX[4] = {300, 0, 100, 200};
u16 RockPanelRotIncY[4] = {65386, 450, 0, 0};
u16 RockPanelRotIncZ[4] = {0, 200, 300, 70};
f32 RadarX = 0.75f;
f32 RadarY = 0.75f;
f32 RadarZ = 1.05f;
f32 RADARSCALE = 0.015f;
f32 RADARSCALEX = 0.0105f;
f32 MAXRAD = 10.0f;
f32 RADARBASESCALE = 10.0f;
f32 DOTSCALE = 0.05f;
s32 ROKAMBVOL = 16383;
s32 ROKPITCH = 640;
f32 MINROKVOL = 2.0f;
f32 TRAILFUDGE = 0.075f;
f32 ROCKSCALE = 0.5f;
s32 ROCKSMASHVOL = 32767;

void RumbleCam(struct cammtx_s *cam)
{
    static f32 AngleY = 0.0f;
    struct nuvec_s delta;
    struct nuvec_s campos;
    struct nuvec_s interest;
    f32 angy;
    f32 turn;
    f32 dist;
    f32 limit;

    NuVecScale(&interest, &earthboss_interest, RumbleCamTween);
    NuVecScaleAccum(&interest, &playeratlas_interest, 1.0f - RumbleCamTween);
    interest.y += (1.0f - RumbleCamTweenInterest) * 2.0f;
    angy = NuAtani(-interest.x * 256.0f, -interest.z * 256.0f) / 182.04445f;
    if (ResetAtlasCamera != 0) {
        AngleY = angy;
        ResetAtlasCamera = 0;
    }
    turn = Rationalise360f(angy - AngleY);
    dist = NuFsqrt(interest.x * interest.x + interest.z * interest.z);
    limit = NW_HZ(3.6f, 3.0000002f);
    if (dist < 5.0f) {
        turn *= (dist / 5.0f) * (dist / 5.0f);
    }
    if (turn > limit) {
        turn = limit;
    } else if (turn < -limit) {
        turn = -limit;
    }
    AngleY = Rationalise360f(AngleY + turn);
    NuVecRotateY(&campos, SetNuVecPntr(0.0f, 5.0f, 9.9f), (s32)(AngleY * 182.04445f));
    interest.y += RumbleCamVal;
    NuVecScale(&campos, &campos, 1.0f - RumbleCamTweenInterest);
    NuVecScaleAccum(&campos, &interest, RumbleCamTweenInterest);
    delta.x = interest.x - campos.x;
    delta.y = interest.y - campos.y;
    delta.z = interest.z - campos.z;
    dist = NuFsqrt(delta.x * delta.x + delta.z * delta.z);
    cam->xrot = NuAtani(-delta.y * 256.0f, dist * 256.0f);
    cam->yrot = NuAtani(delta.x * 256.0f, delta.z * 256.0f);
    NuMtxSetRotationX(&cam->m, cam->xrot);
    NuMtxRotateY(&cam->m, cam->yrot);
    NuMtxTranslate(&cam->m, &campos);
    cam->pos = campos;
    RumbleCamPos = campos;
    RumbleCamY = cam->yrot;
}


/* Defined here: retail emits them after RumbleCam's AngleY. */
s32 DebugCam = 0;
s32 VEHICLECONTROL = 0;
f32 GliderCamHighTimer = 0.0f;
f32 Level_DeadTime = 0.0f;
s32 ResetAtlasCamera = 0;
s32 FlyingLevelVictoryDance = 0;
f32 FlyingLevelCompleteTimer = 0.0f;
s32 ChrisBigBossDead = 0;
f32 WBIntroTweenTimer = 0.0f;
f32 FlyingLevelVictoryDanceTimer = 0.0f;
s32 FireFlyIntroOn = 0;
f32 AtlasWhackValue = 0.0f;
f32 AtlasWhackTimer = 0.0f;
s32 WhackSFX = 0;
f32 ATLASLOOPVOL = 0.0f;
s32 EarthBossJustEntered = 0;
s32 EarthBossDeathEffect = 0;
f32 EarthBossDeathTimer = 0.0f;
s16 RumbleCamY = 0;
s32 WBIntroOn = 0;
f32 RumbleCamTween = 0.0f;
f32 RumbleCamTweenDest = 0.0f;
f32 RumbleCamTweenInterest = 0.0f;
f32 RumbleCamTweenInterestDest = 0.0f;
f32 RumbleBumpVal = 0.0f;
s32 FlyingLevelExtro = 0;
s32 VehicleLevelImmune = 0;
struct numtx_s *GliderBulletReflectMtx = 0;
s32 GliderFrameCounter = 0;
f32 FireFlyIntroTween = 0.0f;
f32 Level_GliderSpeed = 0.0f;
f32 Level_GliderFloor = 0.0f;
f32 Level_GliderCeiling = 0.0f;
f32 Level_GliderCurrentCeiling = 0.0f;
f32 Level_GliderRadius = 0.0f;
f32 Level_GliderCentreX = 0.0f;
f32 Level_GliderCentreZ = 0.0f;
f32 Level_TargetTime = 0.0f;
f32 WeatherBossSkeletonGlitchTimer = 0.0f;
f32 WeatherBossSkeletonTimer = 0.0f;
s32 WeatherBossDead = 0;
f32 WeatherBossTargetAppearTimer = 0.0f;
s32 TestIndx = 0;
s32 GliderRumble = 0;
s32 SatelliteCharacterId = 0;
s32 FreeGliderBulletNum = 0;
s32 UsedGliderBulletNum = 0;
s32 FreeEnemyGliderBulletNum = 0;
s32 UsedEnemyGliderBulletNum = 0;
s32 BulletNumFrees = 0;
s32 BulletEnemyNumFrees = 0;
struct nuvec_s *CamOveride = 0;
f32 ExtroAngOff = 0.0f;
s32 ActiveAsteroidListNum = 0;
s32 BigGunIndx = 0;
s32 BigGunDraw = 0;
s32 TestLocatorIndx = 0;
s32 BazookaIconOn = 0;
struct nuvec_s *VulnerableA = 0;
struct nuvec_s *VulnerableB = 0;
f32 VulnerableRad = 0.0f;
s32 VulnerableSection = 0;
s32 FireFlyIntroAction = 0;
s32 FireFlyIntroOldAction = 0;
s32 AtlasEmbedd = 0;
s32 AtlasLastNonEmbeddedFrame = 0;
s32 AtlasFrame = 0;
s32 tttt = 0;
f32 TempAngX = 0.0f;
f32 TempAngY = 0.0f;
s32 DavesGay = 0;
s32 kj = 0;
s32 EarthBossVortexOpen = 0;
s32 RumbleStoreTotalRoks = 0;
s32 RumbleStoreCrunchRoks = 0;
s32 RumbleStoreCrashRoks = 0;
s32 RumbleDisplayMode = 0;
s32 ShootRockSound = 0;
s32 JonnyOn = 0;
f32 HitMag = 0.0f;
s32 AtlasHitsLost = 0;
f32 RumbleDisplayY = 0.0f;
u16 RockPanelRotX[4] = {0};
u16 RockPanelRotY[4] = {0};
u16 RockPanelRotZ[4] = {0};
volatile s32 r1 = 0;
f32 BIGIMPACT = 0.0f;
s32 AtlasTrailType[2] = {0};
s32 trailpt = 0;
s32 trailair = 0;
s32 trailpos = 0;
f32 CurrentIntensity = 0.0f;
s32 IntoTrail = 0;

void ProcessVehMasks(void)
{
    struct vehmask_s *mask;
    struct nuvec_s pos;
    f32 tween;
    f32 newmix;
    f32 oldmix;
    f32 rad;
    f32 tiltx;
    f32 inv;
    s32 i;

    mask = VehicleMask;
    for (i = 0; i < 2; i++, mask++) {
        if (mask->active == 0) {
            continue;
        }
        tween = mask->tween;
        if (tween > 0.5f) {
            inv = 1.0f - tween;
            newmix = 1.0f - (inv + inv) * inv;
        } else {
            newmix = (tween + tween) * tween;
        }
        oldmix = 1.0f - newmix;
        rad = mask->rad[0] * oldmix + mask->rad[1] * newmix;
        tiltx = mask->tiltx[0] * oldmix + mask->tiltx[1] * newmix;
        NuVecRotateY(&pos, SetNuVecPntr(0.0f, 0.0f, rad), (s32)(mask->ang * 182.04445f));
        NuVecRotateX(&pos, &pos, (s32)(tiltx * 182.04445f));
        NuVecScaleAccum(&pos, &mask->offset[0], oldmix);
        NuVecScaleAccum(&pos, mask->point[0], oldmix);
        NuVecScaleAccum(&pos, &mask->offset[1], newmix);
        NuVecScaleAccum(&pos, mask->point[1], newmix);
        mask->drawscale = mask->scale[0] * oldmix + mask->scale[1] * newmix;
        mask->pos = pos;
        mask->ang = Rationalise360f(mask->ang + mask->anginc[0] * oldmix + mask->anginc[0] * newmix);
        mask->tween -= mask->tweeninc;
        if (mask->tween <= 0.0f) {
            if (mask->killatend != 0) {
                mask->active = 0;
            }
            mask->tween = 0.0f;
        }
        if (Level == 0x15) {
            mask->drawangy = RumbleCamY / 182.04445f;
        } else if (Level == 0x18) {
            mask->drawangy = 180.0f;
        } else if (Level == 0x17) {
            mask->drawangy = NuAtan2D(player->obj.pos.x - mask->pos.x, player->obj.pos.z - mask->pos.z) *
                             (3.1415927f / 32768.0f);
        } else {
            mask->drawangy = 0.0f;
        }
        MyAnimateModelNew(&mask->draw, NW_HZ(0.59999996f, 0.5f));
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
        NuVecScale(&bolt->vel, vel, NW_HZ(0.02f, 0.016666668f));
        bolt->life = life;
        bolt->type = type;
        bolt->active = 2;
        bolt->scale = oldscale + NW_HZ(0.02f, 0.016666668f) / WBTYPE2SCALETIME;
        if (bolt->scale >= 1.0f) {
            bolt->scale = 1.0f;
        }
        if (bolt->type == 0) {
            bolt->seekspeed = BOLTHOMESEEKSPEED * NW_HZ(0.02f, 0.016666668f);
        } else {
            bolt->seekspeed = 0.0f;
        }
        return &bolt->pos;
    }
    return 0;
}

inline void InitWBBolts(void)
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

inline s32 PointsSame(struct nuvec_s *a, struct nuvec_s *b)
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


void SetTrailPos(s32 pt, struct NEWBUGGY *atlas, struct nuvec_s *pos, s32 reset, s32 angle, f32 width,
                 s32 surface)
{
    struct trail_s *cur;
    struct trail_s *prev;
    f32 fade;
    f32 intensity;
    s32 base;
    s32 count;
    s32 tries;
    s32 j;
    s32 i;

    fade = 0.0f;
    for (i = 0; i < 8; i++) {
        base = trail[(pt + 1 + i) & 0x7F].basealpha;
        trail[(pt + 1 + i) & 0x7F].alpha = base * fade;
        fade += 0.125f;
    }
    j = pt - 1;
    prev = &trail[j & 0x7F];
    count = 128;
    tries = 4;
    while (--count != -1 && --tries != -1) {
        cur = prev;
        j--;
        prev = &trail[j & 0x7F];
        if (cur->pos[0].x != -10000.0f && prev->pos[0].x != -10000.0f &&
            !TrailPointInPoly(pos, &cur->pos[0], &cur->pos[1], &prev->pos[1], &prev->pos[0])) {
            if (PointsSame(&cur->pos[0], &prev->pos[0]) || PointsSame(&cur->pos[1], &prev->pos[1])) {
                tries = 4;
            }
        } else {
            tries = 0;
        }
    }
    trail[pt].pos[0] = pos[0];
    trail[pt].pos[1] = pos[1];
    if (reset != 0) {
        IntoTrail = 0;
    } else {
        IntoTrail++;
        if (IntoTrail > 8) {
            IntoTrail = 8;
        }
    }
    if (angle & 0x8000) {
        intensity = IntoTrail * -31.9999f * 0.125f;
    } else {
        intensity = IntoTrail * 31.9999f * 0.125f;
    }
    if (reset == 0) {
        if (CurrentIntensity < intensity) {
            CurrentIntensity += 12.8f;
            if (intensity < CurrentIntensity) {
                CurrentIntensity = intensity;
            }
        } else if (intensity < CurrentIntensity) {
            CurrentIntensity -= 12.8f;
            if (CurrentIntensity < intensity) {
                CurrentIntensity = intensity;
            }
        }
    } else {
        CurrentIntensity = intensity;
    }
    trail[pt].basealpha = trail[pt].alpha = (s32)CurrentIntensity << 24;
}

inline u16 FindTrailAng(struct nuvec_s *a, struct nuvec_s *b)
{
    struct nuvec_s d;

    NuVecSub(&d, b, a);
    return (u16)(NuAtan2D(d.x, d.z) - 0x2000);
}

void ProcessAtlasTrail(struct NEWBUGGY *atlas)
{
    f32 dx;
    f32 dz;
    struct trail_s *t;
    struct nuvec_s TrailPos[2];
    s32 TrailType[2];
    s32 TrailOn;
    f32 Scale = 1.0f;

    TrailOn = 0;
    if (atlas->platformid == -1) {
        {
            struct nuvec_s Temp;
            struct nuvec_s Normal;
            struct nuvec_s Line;
            s32 k;

            Temp = (struct nuvec_s){0.0f, 1.0f, 0.0f};
            NuVecSub(&Line, &atlas->ball_pos, &atlas->ball_oldpos);
            NuVecCross(&Normal, &Line, &Temp);
            NuVecNorm(&Normal, &Normal);
            TrailPos[1] = atlas->ball_pos;
            TrailPos[0] = TrailPos[1];
            NuVecScaleAccum(&TrailPos[0], &Normal, 0.25f);
            NuVecScaleAccum(&TrailPos[1], &Normal, -0.25f);
            for (k = 0; k < 2; k++) {
                TrailOn = 0;
                TrailPos[k].y = TRAILFUDGE + NewShadowMask(&TrailPos[k], atlas->ball_radius, -1);
                if (atlas->ball_pos.y - TrailPos[k].y < atlas->ball_radius + 1.0f && TrailPos[k].y != 2000000.0f) {
                    TrailOn = 1;
                }
                if (TrailOn == 0) {
                    break;
                }
                TrailType[k] = ShadowInfo();
            }
        }
    }
    if (TrailOn != 0) {
        if (TrailType[0] == TrailType[1]) {
            if (TrailType[1] != 0) {
                atlas->trailsurfacetype = TrailType[1];
            } else {
                TrailOn = 0;
                atlas->trailsurfacetype = -1;
            }
        } else if (atlas->trailsurfacetype != TrailType[0] && atlas->trailsurfacetype != TrailType[1]) {
            TrailOn = 0;
            atlas->trailsurfacetype = -1;
        }
    } else {
        atlas->trailsurfacetype = -1;
    }
    if (TrailOn != 0) {
        AtlasTrailType[0] = TrailType[0];
        AtlasTrailType[1] = TrailType[1];
    } else {
        AtlasTrailType[1] = AtlasTrailType[0] = -1;
    }
    if (TrailOn != 0) {
        if (trailair != 0) {
            s32 Ang;

            trailpt = (trailpt + 1) & 0x7F;
            trailair = 0;
            Ang = FindTrailAng(&TrailPos[0], &TrailPos[1]);
            SetTrailPos(trailpt, atlas, TrailPos, 1, Ang, Scale, atlas->trailsurfacetype);
            trailpt = (trailpt + 1) & 0x7F;
            SetTrailPos(trailpt, atlas, TrailPos, 1, Ang, Scale, atlas->trailsurfacetype);
        } else {
            s32 Ang;

            t = &trail[(trailpt - 1) & 0x7F];
            dx = (TrailPos[0].x + TrailPos[1].x) * 0.5f - (t->pos[0].x + (t->pos[1].x - t->pos[0].x) * 0.5f);
            dz = (TrailPos[0].z + TrailPos[1].z) * 0.5f - (t->pos[0].z + (t->pos[1].z - t->pos[0].z) * 0.5f);
            {
                s32 OldAng;
                s32 NewAng;
                s32 DeltaAng;

                NewAng = FindTrailAng(&TrailPos[0], &TrailPos[1]);
                OldAng = FindTrailAng(&t->pos[0], &t->pos[1]);
                DeltaAng = (u16)(NewAng - OldAng - 0x8000) - 0x8000;
                if (dx * dx + dz * dz > 0.040000003f || (DeltaAng < 0 ? -DeltaAng : DeltaAng) >= 0x1556) {
                    trailpt = (trailpt + 1) & 0x7F;
                }
                Ang = NewAng;
            }
            SetTrailPos(trailpt, atlas, TrailPos, 0, Ang, Scale, atlas->trailsurfacetype);
        }
    } else {
        if (trailair == 0) {
            FadeOutLastTrail(trailpt, 1);
            trailpt = (trailpt + 1) & 0x7F;
        }
        trailair = 1;
        trail[trailpt].pos[0].x = -10000.0f;
    }
}


inline struct jeeprock_s *FindJeepRock(void)
{
    s32 i;

    for (i = 0; i < 6; i++) {
        if (JeepRock[i].exists == 0) {
            return &JeepRock[i];
        }
    }
    return 0;
}


struct jeeprock_s *AddRockVel(struct nuvec_s *pos, struct nuvec_s *vel, f32 radius, s32 owner)
{
    struct nuquat_s spin;
    struct jeeprock_s *rock;

    rock = FindJeepRock();
    if (rock == 0) {
        return 0;
    }
    memset(rock, 0, sizeof(struct jeeprock_s));
    NuMtxSetRotationY(&mTEMP, frand() * 180.0f * NW_HZ(0.02f, 0.016666668f) * 182.04445f);
    NuMtxRotateX(&mTEMP, frand() * 720.0f * NW_HZ(0.02f, 0.016666668f) * 182.04445f);
    NuMtxToQuat(&mTEMP, &spin);
    NuQuatNormalise(&spin, &spin);
    rock->pos = *pos;
    switch (owner) {
    case 1:
        rock->fireblob = 1;
        break;
    case 2:
        rock->smashme = 1;
        break;
    }
    rock->scale = SetNuVec(radius + radius, radius + radius, radius + radius);
    rock->exists = 1;
    rock->vel = *vel;
    rock->life = 5.5f;
    InitAtlas(&rock->atlas, &rock->pos, radius, rock + 3 - JeepRock);
    rock->atlas.ball_vel = *vel;
    rock->atlas.framequat[0] = spin;
    rock->atlas.framequat[1] = spin;
    rock->atlas.framequat[2] = spin;
    rock->atlas.framequat[3] = spin;
    rock->atlas.rock = 1;
    rock->atlas.rocknum = rock - JeepRock;
    return rock;
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
extern void ProcessAtlasTrail(struct NEWBUGGY *atlas);
extern s32 LIFTPLAYER;
extern u16 SeekRot(u16 a, u16 target, s32 rate);
extern s32 RotDiff(u16 a, u16 b);
extern void PlayerCreatureCollisions(struct obj_s *obj);
extern s32 WumpaCollisions(struct obj_s *obj);


void MoveAtlas(struct creature_s *c, struct nupad_s *pad)
{
    struct NEWBUGGY *atlas;
    u16 oldhdg;
    f32 dx;
    f32 dz;

    atlas = c->Buggy;
    if (ProcessTimer(&AtlasWhackTimer) != 0) {
        AtlasWhackValue = 0.0f;
    }
    if (LIFTPLAYER != 0 && (pad->paddata & 0x10)) {
        atlas->ball_pos.y += 0.1f;
        atlas->ball_vel.y = 0.0f;
    }
    AtlasFrame++;
    if (AtlasFrame > 0x11D) {
        kj++;
    }
    ControlAtlas(atlas, pad, NW_HZ(0.02f, 0.016666668f));
    ProcessAtlas(atlas);
    c->obj.pos.x = atlas->ball_pos.x;
    c->obj.pos.y = atlas->ball_pos.y - D_00560D44[0];
    c->obj.pos.z = atlas->ball_pos.z;
    c->obj.mom = atlas->ball_vel;
    dx = c->obj.pos.x - c->obj.oldpos.x;
    dz = c->obj.pos.z - c->obj.oldpos.z;
    c->obj.xz_distance = NuFsqrt(dx * dx + dz * dz);
    oldhdg = c->obj.hdg;
    if (c->obj.pad_speed > 0.0f || c->obj.xz_distance > NW_HZ(0.01f, 0.008333334f)) {
        c->obj.thdg = NuAtan2D(dx, dz) + 0x8000;
    }
    c->obj.hdg = SeekRot(c->obj.hdg, c->obj.thdg, 4);
    c->obj.dyrot = RotDiff(oldhdg, c->obj.hdg);
    PlayerCreatureCollisions(&c->obj);
    HitItems(&c->obj);
    if (HitCrates(&c->obj, 1) != 0 && (temp_crate_type == 0x10 || temp_crate_type == 9)) {
        KillPlayer(&c->obj, 0xB);
    }
    WumpaCollisions(&c->obj);
    c->obj.attack = 0x430;
    ProcessAtlasTrail(atlas);
    if (Level == 0x15 && atlas->ball_health > atlas->desthitpoints && GDeb[0x3D].i != -1) {
        AddVariableShotDebrisEffect(GDeb[0x3D].i, &atlas->ball_pos, 1, 0, 0);
    }
    atlas->interestpoint = atlas->ball_pos;
}

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



extern void edppRegisterPointerToGameCharLocation(struct nuvec_s *loc);

inline void InitGlider(struct GLIDERSTRUCT *Glider, struct nuvec_s *StartPos, f32 StartAng)
{
    struct CharacterData *cdata;
    struct creature_s *cre;
    s32 i;

    WBIntroOn = 0;
    memset(Glider, 0, sizeof(struct GLIDERSTRUCT));
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

extern f32 WRStartAng;
extern f32 CrashteroidsStartAngle;
extern void ResetGameCameras(struct cammtx_s *cam, s32 n);
extern void InitBigGuns(void);

extern s32 new_mode;
extern s32 new_level;
extern struct nuvec_s D_005864F8;
#define CameraPos D_005864F8
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
        FarmResetTimer = 0;
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

void SpaceArenaReset(s32 PlayerDead)
{
    LevelResetTimer = 0;
    Level_GliderSpeed = 8.0f;
    Level_GliderCeiling = 35.0f;
    Level_GliderFloor = -30.0f;
    Level_GliderCurrentCeiling = 35.0f;
    Level_GliderRadius = 180.0f;
    Level_DeadTime = 8.0f;
    FireFlyIntroOldAction = -1;
    Level_GliderCentreX = 0.0f;
    Level_GliderCentreZ = 0.0f;
    FireFlyIntroAction = 0;
    FireFlyIntroOn = 0;
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

extern f32 FIREFLYVICDANCETIME;
extern f32 FireFlyStartAngle;
extern void InitGunBoats(void);

void FireFlyReset(s32 PlayerDead)
{
    FlyingLevelVictoryDanceTimer = FIREFLYVICDANCETIME;
    FireFlyIntroOldAction = -1;
    FireFlyIntroAction = 0;
    LevelResetTimer = 0;
    Level_GliderSpeed = 8.0f;
    Level_GliderCeiling = 25.0f;
    Level_GliderFloor = 3.0f;
    Level_GliderCurrentCeiling = 25.0f;
    Level_GliderRadius = 90.0f;
    Level_GliderCentreX = 40.32f;
    Level_GliderCentreZ = 52.09f;
    Level_TargetTime = 1.3f;
    Level_DeadTime = 8.0f;
    GliderFrameCounter = 0;
    FireFlyIntroOn = 0;
    InitGlider(&PlayerGlider, &FireFlyStart, FireFlyStartAngle);
    GliderCamHighTimer = 0.0f;
    GameCam[0].m._30 = FireFlyStart.x + 0.0f;
    GameCam[0].m._31 = FireFlyStart.y + 2.0f;
    GameCam[0].m._32 = FireFlyStart.z + 5.0f;
    ResetGameCameras(&GameCam[0], 1);
    InitZoffaUFOs();
    InitGunBoats();
    InitBattleShips();
    InitGliderBullets();
    InitGliderBombs();
}

inline void InitWeatherBossTarget(void)
{
    WeatherBossTargetPos = SetNuVec(0.0f, 0.0f, -15.0f);
    WeatherBossTargetVel = v000;
    WeatherBossTargetAppearTimer = 0.0f;
}

inline void ProcessWeatherBossTarget(void)
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


extern f32 WBBOSSINTRODIST;
extern void InitWBIntro(void);


void WeatherBossReset(s32 playerdead)
{
    struct nuvec_s start;
    struct nuvec_s pos;
    u16 angle;

    WBIntroTweenTimer = 0.0f;
    ChrisBigBossDead = 0;
    WeatherBossSkeletonGlitchTimer = 0.0f;
    Level_GliderFloor = 18.0f;
    start = (struct nuvec_s){0.0f, 38.0f, 0.0f};
    WeatherBossSkeletonTimer = 0.0f;
    WeatherBossDead = 0;
    AtmosphericPressureHackedZ = 0.0f;
    LevelResetTimer = 0;
    Level_GliderSpeed = 0.0f;
    Level_GliderCeiling = 60.0f;
    Level_GliderCentreX = 0.0f;
    Level_GliderCentreZ = 0.0f;
    Level_GliderCurrentCeiling = 60.0f;
    Level_GliderRadius = 100000.0f;
    Level_DeadTime = 8.0f;
    InitGlider(&PlayerGlider, &start, 0.0f);
    GliderCamHighTimer = 0.0f;
    GameCam[0].m._30 = start.x + 0.0f;
    GameCam[0].m._31 = start.y + 2.0f;
    GameCam[0].m._32 = start.z + 5.0f;
    ResetGameCameras(&GameCam[0], 1);
    WeatherBossCamSpline.spl = SplTab[26].spl;
    if (WeatherBossCamSpline.spl != 0) {
        WeatherBossCamSpline.speed = 0.00002f;
        WeatherBossCamSpline.ratio = 0.0f;
        WeatherBossCamSpline.nextratio = 0.0f;
        PointAlongSpline(WeatherBossCamSpline.spl, 0.0f, &pos, &angle, 0);
        WeatherBossCamSpline.pos = pos;
        WeatherBossCamSpline.lastpos = pos;
        InitVehMasks();
        InitVehMask(0, 0x58);
        InitVehMask(1, 3);
        InitWeatherBossTarget();
        InitGliderBullets();
        InitWeatherBoss();
        InitLighteningHail();
        InitWBBolts();
        InitWBIntro();
    }
}


void InitWBIntro(void)
{
    struct splinepath_s *path = &WeatherBossIntroSpline;
    u16 angle;

    WBIntroOn = 0;
    path->spl = SplTab[72].spl;
    if (path->spl != 0) {
        path->ratio = 0.0f;
        path->speed = 0.00002f;
        path->nextratio = 0.0f;
        PointAlongSpline(path->spl, path->ratio, &WBIntroGliderPos, &angle, 0);
        path->pos = WBIntroGliderPos;
        path->lastpos = WBIntroGliderPos;
        WeatherBoss.dist = WBBOSSINTRODIST;
        WeatherBoss.destdist = WBBOSSINTRODIST;
        path->finished = 0;
        WeatherBossTargetAppearTimer = WeatherBossTargetAppearTime;
        WBIntroTweenTimer = WBIntroTweenTime;
        WBIntroOn = 1;
    }
}

extern f32 FindSplineClosestPointAndDist(struct splinepath_s *spline, s32 control, struct nuvec_s *point,
                                         struct nuvec_s *closest, s32 wrap, s32 biglook);
extern f32 WBINTROLOOKAHEAD;


void ProcessWBIntro(void)
{
    struct nuvec_s lookpos;
    struct nuvec_s target;
    struct nuvec_s dir;

    if (WBIntroOn != 0) {
        lookpos = WBIntroGliderPos;
        lookpos.z -= WBINTROLOOKAHEAD;
        FindSplineClosestPointAndDist(&WeatherBossIntroSpline, 0x300, &lookpos, &target, 0, 0);
        NuVecSub(&dir, &target, &WBIntroGliderPos);
        NuVecScale(&dir, &dir, 96.0 / fabs(dir.z));
        NuVecScaleAccum(&WBIntroGliderPos, &dir, NW_HZ(0.02f, 0.016666668f));
        WBIntroDestTiltZ = dir.x * WBITILTZSCALE;
        WBIntroDestTiltX = dir.y * WBITILTXSCALE;
        PlayerGlider.Velocity = dir;
        if (WeatherBossIntroSpline.nextratio >= 0.99f) {
            WBIntroOn = 0;
            WeatherBoss.destdist = 96.0f;
        }
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

void GliderSmoke(struct GLIDERSTRUCT *glider)
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

inline void ExplodeGlider(struct GLIDERSTRUCT *glider)
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
        FarmResetTimer = NW_HZ(50, 60);
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


inline void DeadGliderWB(struct GLIDERSTRUCT *glider)
{
    if (glider->Dead == 0) {
        ExplodeGlider(glider);
    }
}


void DeadGliderCoco(struct GLIDERSTRUCT *glider)
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
    NuVecScaleAccum(&glider->Position, &glider->Velocity, NW_HZ(0.02f, 0.016666668f));
    glider->TiltX += glider->CocoDeathSpinX * NW_HZ(0.02f, 0.016666668f);
    glider->TiltZ += glider->CocoDeathSpinZ * NW_HZ(0.02f, 0.016666668f);
    if (ProcessTimer(&glider->CocoDeadTimer)) {
        glider->CocoDeadTimer = 1000000.0f;
        ExplodeGlider(glider);
    }
}


void ProcessGliderMovementWB(struct GLIDERSTRUCT *glider)
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
    NuVecScaleAccum(&glider->Position, &glider->Velocity, NW_HZ(0.02f, 0.016666668f));
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
        SeekHalfLife(&glider->BarrelSpeedX, dest, WBBARRELSEEKSPEEDZERO, NW_HZ(0.02f, 0.016666668f));
    } else {
        SeekHalfLife(&glider->BarrelSpeedX, dest, WBBARRELSEEKSPEEDNONZERO, NW_HZ(0.02f, 0.016666668f));
    }
    roll = SetNuVec(glider->BarrelSpeedX, 0.0f, 0.0f);
    NuVecRotateY(&roll, &roll, (s32)(glider->AngleY * 182.04445f));
    glider->Position.x += roll.x * NW_HZ(0.02f, 0.016666668f);
    glider->Position.y += roll.y * NW_HZ(0.02f, 0.016666668f);
    glider->Position.z += roll.z * NW_HZ(0.02f, 0.016666668f);
}

extern void AddExtraLife(struct nuvec_s *pos, s32 pdeb);
extern void ControlGlider(struct GLIDERSTRUCT *glider, struct nupad_s *pad);
extern void ControlGliderWeatherBoss(struct GLIDERSTRUCT *glider, struct nupad_s *pad);
extern void ProcessGliderMovement(struct GLIDERSTRUCT *glider, f32 dt);
extern void GliderFire(struct GLIDERSTRUCT *glider, struct nupad_s *pad);

extern void ProcessCrashteroidsIntro(void);

void MoveGlider(struct creature_s *Cre, struct nupad_s *Pad)
{
    struct GLIDERSTRUCT *Glider;
    struct creature_s *c;
    struct nuvec_s v;
    f32 r;
    f32 TiltVal;
    double absval;
    short Rumble;

    Glider = (struct GLIDERSTRUCT *)Cre->Buggy;
    if (Glider == 0) {
        NuDebugMsgProlog(".\\vehicle.c", 0xFB1)("Init Glider Called");
        Glider = &PlayerGlider;
        Cre->Buggy = (struct NEWBUGGY *)Glider;
        PlayerGlider.Cre = Cre;
        return;
    }
    ProcessTimer(&Glider->NextHitSoundTimer);
    ProcessTimer(&Glider->ImmuneAsteroidsTimer);
    Glider->OldPosition = Glider->Position;
    if (Glider->TerminalDive != 0) {
        Glider->HitPoints = 0;
    }
    if (Glider->HitPoints <= 0) {
        if (Level == 0x1A) {
            DeadGliderCoco(Glider);
            return;
        }
        if (Level == 0x18) {
            Glider->HitTimer = Level_DeadTime;
            GliderSmoke(Glider);
            DeadGliderWB(Glider);
            return;
        }
    }
    if (Glider->Dead != 0) {
        Glider->HitTimer = Level_DeadTime;
        GliderSmoke(Glider);
        Glider->Dead--;
        if (Glider->Dead > 0) {
            return;
        }
        Glider->Dead = NW_HZ(0x32, 0x3C);
        AddGameDebris(0x18, &Glider->Position);
        return;
    }
    ProcessTimer(&Glider->HitTimer);
    ControlGlider(Glider, Pad);
    if (Level == 0x18) {
        ProcessGliderMovementWB(Glider);
    } else {
        ProcessGliderMovement(Glider, NW_HZ(0.02f, 0.016666668f));
    }
    if (Level == 0x18) {
        ControlGliderWeatherBoss(Glider, Pad);
    }
    if (CollideGliderBullets(&Glider->Position, 0.8f, 1, 1.0f, 0, 0) != 0) {
        Glider->HitTimer += 0.5f;
        if (InvincibilityCHEAT == 0 && VehicleLevelImmune == 0) {
            Glider->HitPoints--;
        }
        if (Glider->HitPoints < 0) {
            Glider->HitPoints = 0;
        }
        NewBuzz(&player->rumble, BEENHITBUZZTIME);
        if (Glider->NextHitSoundTimer == 0.0f) {
            if (Level == 0x1A) {
                MyGameSfx(0xD9, &Glider->Position, 0x7FFF);
            } else {
                MyGameSfx(0x5A, &Glider->Position, 0x7FFF);
            }
            Glider->NextHitSoundTimer = GLIDERHITSOUNDFREQUENCY;
        } else {
            MyGameSfx(GLIDERHITSOUNDOTHERID, &Glider->Position, 0x4FFF);
        }
    }
    GliderBulletsHitThings(Glider);
    if (Level == 0x12 || Level == 0x24) {
        GliderBombsHitThings(Glider);
    }
    if (Glider->HitPoints <= 0) {
        Glider->HitPoints = 0;
        Glider->TerminalDive = 1;
        Glider->HitTimer = Level_DeadTime;
    }
    if (Level != 0x18 && Glider->Dead == 0 && new_mode == -1 && new_level == -1
        && GameTimer.frame > NW_HZ(0x32, 0x3C)) {
        s32 Explode;

        Explode = (Glider->Position.y < 6.0f && Level == 0xD);
        Explode |= (Glider->Position.y < 0.0f && Level == 0x12);
        if (!Explode) {
            if (Level == 0x12 && Glider->TerminalDive == 0) {
                struct nuvec_s Start;
                struct nuvec_s Rel = {0.0f, -11.0f, 0.0f};

                Start = Glider->Position;
                Start.y += 10.0f;
                if (NewRayCastMask(&Start, &Rel, 0.0f, 0xFF) != 0) {
                    struct nuvec_s Temp;

                    NuVecAdd(&Temp, &Start, &Rel);
                    if (Temp.y > Glider->Position.y) {
                        Glider->Position = Temp;
                    }
                    Glider->PullUpTimer = 0.5f;
                }
            } else {
                struct nuvec_s Move;

                NuVecSub(&Move, &Glider->Position, &Glider->OldPosition);
                if (NewRayCast(&Glider->OldPosition, &Move, 0.0f) != 0) {
                    Explode = 1;
                }
            }
            if (!Explode && Level == 0x12 && CollideWithBattleShips(&PlayerGlider.Position, 1.0f) != 0) {
                Explode = 1;
            }
        }
        if (Explode) {
            ExplodeGlider(Glider);
        }
    }
    GliderSmoke(Glider);
    Cre->obj.pos = Glider->Position;
    if (HitCrates(&Cre->obj, 1) != 0) {
        if (temp_crate_type == 5) {
            if (Level == 0x12 && qrand() < 0xC000) {
                Glider->HitPoints += 0x32;
                if (Glider->HitPoints > 100) {
                    Glider->HitPoints = 100;
                }
            } else {
                AddExtraLife(&temp_pCrate->pos, 2);
            }
        }
    } else if (HitCrateBalloons(&Cre->obj.pos, Cre->obj.RADIUS) != 0) {
        AddGameDebris(0x1A, &Cre->obj.pos);
        AddScreenWumpa(temp_pCrate->pos.x, temp_pCrate->pos.y + CRATEBALLOONOFFSET, temp_pCrate->pos.z, 1);
        temp_pCrate->flags = (temp_pCrate->flags | 0x400) ^ 0x400;
    } else if (HitItems(&Cre->obj) != 0) {
        AddGameDebris(0x1A, &Cre->obj.pos);
    } else {
        c = FindClock();
        if (c != 0) {
            v.x = c->obj.pos.x;
            v.y = c->obj.pos.y + CRATEBALLOONOFFSET;
            v.z = c->obj.pos.z;
            r = Cre->obj.RADIUS + CRATEBALLOONRADIUS;
            if (NuVecDistSqr(&Cre->obj.pos, &v, 0) < r * r) {
                AddGameDebris(0x1A, &Cre->obj.pos);
                StartTimeTrial(&c->obj.pos, 1);
            }
        }
    }
    Glider->LastInTornado = Glider->InTornado;
    Glider->ApparentPosition = Glider->PositionStack[Glider->StackIndx];
    Glider->ApparentVelocity = Glider->VelocityStack[Glider->StackIndx];
    Glider->PositionStack[Glider->StackIndx] = Glider->Position;
    Glider->VelocityStack[Glider->StackIndx] = Glider->Velocity;
    NuVecScaleAccum(&Glider->PositionStack[Glider->StackIndx], &Glider->Velocity, NW_HZ(0.59999996f, 0.5f));
    Glider->StackIndx++;
    if (Glider->StackIndx > 0x1D) {
        Glider->StackIndx = 0;
    }
    ProcessTimer(&Glider->OverideTiltZ);
    GliderFire(Glider, Pad);
    if (Level == 0x12) {
        s32 indx;
        struct nuvec_s Pos;
        struct nuvec_s Dir;
        struct numtx_s Mtx;
        struct nuvec_s Scale;
        struct nuvec_s Temp;

        indx = 0x86;
        NuMtxSetRotationZ(&mTEMP, (s32)(Glider->TiltZ * 182.04445f));
        NuMtxRotateX(&mTEMP, (s32)((Glider->TiltX + 15.0f) * 182.04445f));
        NuMtxRotateY(&mTEMP, (s32)(Glider->AngleY * 182.04445f));
        NuMtxTranslate(&mTEMP, &Glider->Position);
        if (Glider->HitPoints == 0) {
            indx = 0x84;
        }
        NuVecMtxTransform(&Temp, &ExhaustPosA, &mTEMP);
        AddVariableShotDebrisEffect(GDeb[indx].i, &Temp, 1, 0, 0);
        NuVecMtxTransform(&Temp, &ExhaustPosB, &mTEMP);
        AddVariableShotDebrisEffect(GDeb[indx].i, &Temp, 1, 0, 0);
    }
    if (Glider->InTornadoTime == 0.0) {
        if (ProcessTimer(&Glider->CamTornRecoverTimer) != 0) {
            Glider->CamAngleY = Glider->AngleY;
            Glider->CamTiltX = Glider->TiltX;
        } else {
            Glider->CamAngleY += (1.0f - Glider->CamTornRecoverTimer)
                * Rationalise360f(Glider->AngleY - Glider->CamAngleY);
            Glider->CamTiltX += (1.0f - Glider->CamTornRecoverTimer)
                * Rationalise360f(Glider->TiltX - Glider->CamTiltX);
        }
    }
    if (Level == 0x12) {
        ProcessFireFlyIntro();
    }
    if (Level == 0x1A) {
        ProcessCrashteroidsIntro();
    }
    absval = Glider->TiltZ * 0.5f;
    if (absval < 0.0) {
        absval = -absval;
    }
    TiltVal = absval;
    if (Glider->DestTiltX > 0.0f) {
        TiltVal += Glider->DestTiltX * 0.5f;
    } else {
        TiltVal -= Glider->DestTiltX;
    }
    GliderRumble = 0;
    switch (Level) {
    case 0x1A:
        MyGameSfxLoopVolPitch(0xCE, &Glider->Position, (short)CRASHTEROIDSENGINEVOL,
                              (short)(OIDENGPITCH + (s32)(OIDENGPITCHSLOPE * Glider->TiltX)));
        Rumble = OIDENGRUMBLESLOPE * TiltVal;
        if (Rumble > 0xFF) {
            Rumble = 0xFF;
        }
        if (Rumble < 0) {
            Rumble = 0;
        }
        GliderRumble = Rumble;
        break;
    case 0x12:
        MyGameSfxLoopVolPitch(0xCA, &Glider->Position, (short)SINKENGINEVOL,
                              (short)(MOSENGPITCH + (s32)(MOSENGPITCHSLOPE * Glider->TiltX)));
        Rumble = MOSENGRUMBLESLOPE * 1.25f * TiltVal;
        if (Rumble > 0xFF) {
            Rumble = 0xFF;
        }
        if (Rumble < 0) {
            Rumble = 0;
        }
        GliderRumble = Rumble;
        break;
    case 0xD:
    case 0x18:
    case 0x24:
        MyGameSfxLoopVolPitch(0x5B, &Glider->Position,
                              (Level == 0xD) ? (short)(GLIDENGINEVOL * 2) : (short)GLIDENGINEVOL,
                              (short)(GLIDENGPITCH + (s32)(GLIDENGPITCHSLOPE * Glider->TiltX)));
        Rumble = GLIDENGRUMBLESLOPE * TiltVal;
        if (Rumble > 0xFF) {
            Rumble = 0xFF;
        }
        if (Rumble < 0) {
            Rumble = 0;
        }
        GliderRumble = Rumble;
        break;
    }
    NewRumble(&player->rumble, GliderRumble);
    if (ProcessTimer(&Glider->NextEngRum) != 0) {
        frand();
        GliderRumble = GLIDRUMVOL;
        NewRumble(&player->rumble, GLIDRUMVOL);
        Glider->NextEngRum = GLIDRUMTIM;
    }
    if (Level == 0x1A && PlayerGlider.InTornadoTime == 0.0f) {
        AddVariableShotDebrisEffect(GDeb[0x9F].i, (struct nuvec_s *)&Glider->Cre->mtxLOCATOR[1][0]._30, 1, 0, 0);
        AddVariableShotDebrisEffect(GDeb[0x9F].i, (struct nuvec_s *)&Glider->Cre->mtxLOCATOR[1][1]._30, 1, 0, 0);
        AddVariableShotDebrisEffect(GDeb[0x9F].i, (struct nuvec_s *)&Glider->Cre->mtxLOCATOR[1][2]._30, 1, 0, 0);
    }
}

inline void ProcessCrashteroidsIntro(void)
{
    ProcessFireFlyIntro();
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
    boat->firepitch = 45.0f;
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
    struct nuvec_s scale = {1.5f, 1.5f, 1.5f};

    NuMtxSetScale(&mTEMP, &scale);
    NuMtxRotateX(&mTEMP, boat->firepitch * 182.04445f);
    NuMtxRotateY(&mTEMP, (boat->yaw + boat->fireyaw) * 182.04445f);
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
                EnemyZoffa[i].respawntimer -= NW_HZ(0.02f, 0.016666668f);
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

extern f32 MatchTimer;
extern f32 MatchMaxDist;
extern f32 MatchMinDist;
extern f32 PrePanTime;
extern f32 PanSeekSpeed;
extern f32 MinPanSeekSpeed;
extern f32 FFINTROHEIGHT;
extern f32 FFINTROCAMYOFF;
extern struct nuvec_s D_005864F8;
#define gamecam_trans D_005864F8 /* GameCam[0].m._30.._32 */


void ProcessFireFlyIntro(void)
{
    static f32 Timer;
    static f32 CamAngY;
    struct nuvec_s dir;
    struct nuvec_s flatdir;
    struct nuvec_s rel;
    f32 relang;
    f32 pan;
    f32 seek;
    f32 camdiff;
    s32 done;

    flatdir = SetNuVec(1.0f, 0.0f, 1.0f);
    NuVecNorm(&dir, &flatdir);
    if (Level == 0x12) {
        MatchTimer = 0.0f;
        MatchMaxDist = 4.0f;
        MatchMinDist = 4.0f;
        PrePanTime = 2.0f;
        PanSeekSpeed = 2.0f;
        MinPanSeekSpeed = 0.3f;
        FFINTROHEIGHT = 2.5f;
        FFINTROCAMYOFF = 4.0f;
    } else if (Level == 0x1A) {
        MatchMaxDist = 3.0f;
        MatchMinDist = 3.0f;
        MatchTimer = 0.0f;
        PrePanTime = 3.5f;
        PanSeekSpeed = 1.0f;
        MinPanSeekSpeed = 0.3f;
        FFINTROHEIGHT = -2.5f;
        FFINTROCAMYOFF = 1.75f;
    }
    if (FireFlyIntroOn != 0) {
        NuVecScale(&PlayerGlider.Velocity, &flatdir, Level_GliderSpeed);
        NuVecScaleAccum(&PlayerGlider.Position, &PlayerGlider.Velocity, NW_HZ(0.02f, 0.016666668f));
    }
    GliderIntroInterest = PlayerGlider.Position;
    if (FireFlyIntroAction != FireFlyIntroOldAction) {
        switch (FireFlyIntroAction) {
        case 0:
            PlayerGlider.AutoPilot = 1;
            FireFlyIntroOn = 1;
            GliderIntroCamPos = SetNuVec(40.0f, FFINTROCAMYOFF + 18.0f - FFINTROHEIGHT, 40.0f);
            gamecam_trans = GliderIntroCamPos;
            break;
        case 1:
            Timer = MatchTimer;
            break;
        case 2:
            Timer = PrePanTime;
            break;
        case 3:
            CamAngY = GameCam[0].yrot / 182.04445f + 180.0f;
            break;
        case 4:
            FireFlyIntroOn = 0;
            PlayerGlider.AutoPilot = 0;
            break;
        }
    }
    FireFlyIntroOldAction = FireFlyIntroAction;
    switch (FireFlyIntroAction) {
    case 0:
        NuVecSub(&rel, &PlayerGlider.Position, &GliderIntroCamPos);
        rel.y = 0.0f;
        pan = NuVecMag(&rel);
        CamAngY = PlayerGlider.AngleY + 180.0f;
        if (pan < MatchMaxDist) {
            FireFlyIntroAction = 1;
        }
        break;
    case 1:
        GliderIntroCamPos.x = PlayerGlider.Position.x;
        GliderIntroCamPos.z = PlayerGlider.Position.z;
        NuVecScaleAccum(&GliderIntroCamPos, &dir,
                        MatchMinDist + NW_EE_DIV(Timer, MatchTimer) * (MatchMaxDist - MatchMinDist));
        if (ProcessTimer(&Timer) != 0) {
            FireFlyIntroAction = 3;
        }
        break;
    case 2:
        GliderIntroCamPos = PlayerGlider.Position;
        NuVecScaleAccum(&GliderIntroCamPos, &dir, MatchMinDist);
        if (ProcessTimer(&Timer) != 0) {
            FireFlyIntroAction = 3;
        }
        break;
    case 3:
        done = 0;
        relang = Rationalise360f(PlayerGlider.AngleY - CamAngY);
        pan = fabs(relang);
        if (pan > 90.0f) {
            pan = 180.0f - pan;
        }
        seek = PanSeekSpeed * pan / 90.0f;
        if (seek < MinPanSeekSpeed) {
            seek = MinPanSeekSpeed;
        }
        if (fabs(relang) < seek) {
            done = 1;
            relang = 0.0f;
        } else {
            relang -= seek;
        }
        CamAngY = Rationalise360f(PlayerGlider.AngleY - relang);
        if (done != 0) {
            FireFlyIntroAction = 4;
        }
        break;
    case 4:
        FireFlyIntroAction = 5;
        break;
    }
    GliderIntroInterest.y += 1.0f;
    if (FireFlyIntroAction != 0) {
        GliderIntroCamPos = PlayerGlider.Position;
        NuVecScale(&rel, &dir, MatchMinDist);
        NuVecRotateY(&rel, &rel, (s32)((CamAngY - FireFlyStartAngle - 180.0f) * 182.04445f));
        NuVecAdd(&GliderIntroCamPos, &rel, &PlayerGlider.Position);
        camdiff = Rationalise360f(PlayerGlider.AngleY - CamAngY);
        GliderIntroCamPos.y += FFINTROCAMYOFF - fabs(camdiff) * FFINTROHEIGHT / 180.0;
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

extern void FarmReset(s32 playerdead);

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
            NewBuzz(&player->rumble, NW_HZ(5, 6));
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
        NuVecScaleAccum(&plane->pos, &plane->vel, NW_HZ(0.02f, 0.016666668f));
        PlayerGlider.AutoPilot = grabglider;
        if (grabglider != 0) {
            NuVecAdd(&PlayerGlider.Position, &plane->pos, &ZepplinGliderOffset);
            GliderAutoPosition = PlayerGlider.Position;
            PlayerGlider.TiltX = 0.0f;
            PlayerGlider.TiltZ = 0.0f;
            PlayerGlider.Velocity = plane->vel;
        }
        MyAnimateModelNew(&plane->draw, NW_HZ(0.59999996f, 0.5f));
    }
}


void ProcessFarmLevel(struct nupad_s *pad)
{
    FarmFrame++;
    if (GliderCamHighTimer > 0.0f) {
        GliderCamHighTimer -= NW_HZ(0.02f, 0.016666668f);
    } else {
        GliderCamHighTimer = 0.0f;
    }
    if (FarmResetTimer != 0) {
        FarmResetTimer--;
        if (FarmResetTimer == 0) {
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

void GliderCamWeatherBoss(struct cammtx_s *cam)
{
    static f32 VehCamX;
    static f32 VehCamY;
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
    VehCamX = cam->xrot / 182.0444489f;
    VehCamY = cam->yrot / 182.0444489f;
    NuMtxSetRotationX(&cam->m, VehCamX * 182.0444489f);
    NuMtxRotateY(&cam->m, VehCamY * 182.0444489f);
    NuMtxTranslate(&cam->m, &campos);
    cam->pos = campos;
    WeatherBossCamMtx = cam->m;
}


extern void SeekAngLimHalfLife360f(f32 *dest, f32 target, f32 maxspeed, f32 halflife, f32 dt);

void GliderCam(struct cammtx_s *cam)
{
    static f32 VehCamX;
    static f32 VehCamY;
    static struct nuvec_s LastVel;
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
    SeekHalfLifeNUVEC(&introcam, &GliderIntroCamPos, SeekGliderCamTime, NW_HZ(0.02f, 0.016666668f));
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
            SeekAngLimHalfLife360f(&ExtroAngOff, ExtroTargAng, 120.0f, 0.3f, NW_HZ(0.02f, 0.016666668f));
            NuVecScale(&extropos, &offset, 1.0 - fabs(ExtroAngOff) / 180.0);
            NuVecScaleAccum(&extropos, &ExtroIdealPos, fabs(ExtroAngOff) / 180.0);
            if (fabs(Rationalise360f(ExtroAngOff - ExtroTargAng)) < ExtroChangeVal) {
                NuVecScaleAccum(&ExtroStillCamPos, &ExtroStillVelocity, NW_HZ(0.02f, 0.016666668f));
                NuVecScaleAccum(&ExtroStillInterest, &ExtroStillVelocity, NW_HZ(0.02f, 0.016666668f));
                ideal = ExtroStillCamPos;
                FlyingLevelVictoryDance = 1;
                lookpos = ExtroStillInterest;
                SeekHalfLifeNUVEC(&ExtroStillVelocity, &v000, ExtroSeekZeroTime, NW_HZ(0.02f, 0.016666668f));
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
            SeekHalfLifeNUVEC(&LastVel, SetNuVecPntr(0.0f, 0.0f, 0.0f), 0.5f, NW_HZ(0.02f, 0.016666668f));
            NuVecAdd(&campos, &campos, &LastVel);
        } else {
            if (PlayerGlider.AutoPilot != 0) {
                seek = CameraSeekSpeedAutoPilot;
            } else {
                seek = SeekGliderCamTime;
            }
            LastVel = campos;
            SeekHalfLifeNUVEC(&campos, &ideal, seek, NW_HZ(0.02f, 0.016666668f));
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
            NuVecSub(&LastVel, &campos, &LastVel);
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
    VehCamX = cam->xrot / 182.0444489f;
    VehCamY = cam->yrot / 182.0444489f;
    NuMtxSetRotationX(&cam->m, VehCamX * 182.0444489f);
    NuMtxRotateY(&cam->m, VehCamY * 182.0444489f);
    NuMtxTranslate(&cam->m, &campos);
    NuMtxRotateY(&GliderInvMtx, -VehCamY * 182.0444489f);
    NuMtxSetRotationX(&GliderInvMtx, -VehCamX * 182.0444489f);
    NuMtxTranslate(&GliderInvMtx, &campos);
    cam->pos = campos;
}

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
extern s32 FRAME;
extern void *app_tbset;
extern void tbslotBeginFn(void *tbset, s32 slot);
extern s32 tbslotEndFn(void *tbset, s32 slot);
extern void ProcessEarthBossActions(struct NEWBUGGY *boss);


void ProcessEarthBossLevel(struct nupad_s *pad)
{
    struct jeeprock_s *rock;
    f32 mag;
    f32 bump;
    s32 damage;
    s32 hitpower;
    s32 bumppower;
    s32 hit;

    MyGameSfxLoop(0xD0, &v000, ROKAMBVOL);
    if (AtlasFrame > 0x320) {
        r1++;
    }
    if (EarthBoss.dead == 0) {
        ProcessAtlas(&EarthBoss);
    }
    if (FRAME == 0) {
        tbslotBeginFn(app_tbset, 3);
    }
    ProcessJeepRocks();
    if (FRAME == 0) {
        tbslotEndFn(app_tbset, 3);
        if (FRAME == 0) {
            tbslotBeginFn(app_tbset, 4);
        }
    }
    ProcessRockRockCollisions();
    ProcessAtlasAtlasCollisions();
    if (FRAME == 0) {
        tbslotEndFn(app_tbset, 4);
    }
    hit = 0;
    if (PlayerAtlas.ball_beenhit == 2 &&
        (EarthBoss.ball_action == 4 || EarthBoss.ball_action == 6 || EarthBoss.ball_action == 5)) {
        hit = 1;
    }
    if (PlayerAtlas.ball_beenhit > 2) {
        rock = &JeepRock[PlayerAtlas.ball_beenhit - 3];
        if (rock->owner == -1) {
            rock->owner = 0;
            hit = 1;
            rock->flametimer = 0.0f;
        } else if (PlayerAtlas.ball_health > PlayerAtlas.desthitpoints) {
            if (rock->owner == 0) {
                rock->owner = -1;
                rock->flametimer = 7.0f;
            } else {
                PlayerAtlas.desthitpoints = PlayerAtlas.ball_health;
                rock->owner = 0;
                rock->flametimer = 0.0f;
            }
        } else {
            rock->owner = 1;
            rock->flametimer = 8.0f;
        }
    }
    if (hit != 0) {
        mag = NuVecDist(&PlayerAtlas.ball_vel, &PlayerAtlas.ball_oldvel, 0);
        HitMag = mag;
        if (mag > 20.0f) {
            mag = 20.0f;
        }
        mag = mag * 25.0f / 20.0f;
        damage = mag;
        hitpower = mag * 3.0f;
        NewRumble(&player->rumble, hitpower);
        NewBuzz(&player->rumble, hitpower / NW_HZ(6, 5));
        if (InvincibilityCHEAT == 0 && VehicleLevelImmune == 0) {
            PlayerAtlas.desthitpoints = PlayerAtlas.ball_health - damage;
        }
        AtlasHitsLost = damage;
        PlayerAtlas.hitpointcounter = 0;
        PlayerAtlas.inhibitcontroltimer = mag * 1.5f / 25.0f;
        if (PlayerAtlas.desthitpoints <= 0) {
            PlayerAtlas.desthitpoints = 0;
        }
    } else if (PlayerAtlas.ball_beenhit != 0) {
        bump = NuVecDist(&PlayerAtlas.ball_vel, &PlayerAtlas.ball_oldvel, 0);
        HitMag = bump;
        if (bump > 20.0f) {
            bump = 20.0f;
        }
        bump = bump * 25.0f / 20.0f;
        bumppower = bump * 3.0f;
        NewRumble(&player->rumble, bumppower);
        NewBuzz(&player->rumble, bumppower / NW_HZ(6, 5));
    }
    PlayerAtlas.ball_beenhit = 0;
    if (PlayerAtlas.hitpointcounter != 0) {
        PlayerAtlas.hitpointcounter--;
    } else if (PlayerAtlas.desthitpoints < PlayerAtlas.ball_health) {
        PlayerAtlas.ball_health--;
        PlayerAtlas.hitpointcounter = NW_HZ(5, 6);
    }
    if (PlayerAtlas.ball_health <= 0) {
        if (PlayerAtlas.owner->obj.dead == 0) {
            MyGameSfx(0x25, &PlayerGlider.Position, 0x7FFF);
        }
        PlayerAtlas.ball_health = 0;
        if (PlayerAtlas.owner->obj.dead == 0) {
            KillGameObject(&PlayerAtlas.owner->obj, 0xB);
        }
    }
    if ((PlayerAtlas.ball_pos.x * PlayerAtlas.ball_pos.x + PlayerAtlas.ball_pos.z * PlayerAtlas.ball_pos.z > 225.0f &&
         PlayerAtlas.ball_pos.y < -1.0f) ||
        PlayerAtlas.ball_pos.y < -4.0f ||
        PlayerAtlas.ball_pos.x * PlayerAtlas.ball_pos.x + PlayerAtlas.ball_pos.z * PlayerAtlas.ball_pos.z > 1600.0f) {
        if (PlayerAtlas.owner != 0) {
            PlayerAtlas.dead = 1;
            PlayerAtlas.owner->obj.invincible = 0;
            PlayerAtlas.ball_vel = v000;
            if (PlayerAtlas.owner->obj.dead == 0) {
                KillGameObject(&PlayerAtlas.owner->obj, 0xB);
            }
        }
    }
    ProcessEarthBossActions(&EarthBoss);
    ProcessEarthBossVortex();
    ProcessVehMasks();
    ProcessRumblePanel();
}
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

extern void DrawWesternArenaLevelExtra(void);
extern void DrawFireBossLevelExtra(void);


void DrawExtraCreatures(void)
{
    switch (Level) {
    case 0xD:
        DrawFarmLevelExtra();
        break;
    case 0x12:
        DrawFireFlyLevelExtra();
        break;
    case 0x24:
        DrawWeatherResearchLevelExtra();
        break;
    case 0x18:
        DrawWeatherBossLevelExtra();
        break;
    case 0x1A:
        DrawSpaceArenaLevelExtra();
        break;
    case 0x3:
        DrawWesternArenaLevelExtra();
        break;
    case 0x16:
        DrawFireBossLevelExtra();
        break;
    case 0x15:
        DrawEarthBossLevelExtra();
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



extern s32 D_005B7C94[];
#define earthboss_whacko D_005B7C94

/* Defined here: retail emits them before PrintEarthBossAction's strings. */
f32 WBSCALE = 16.0f;
s32 LocIndx = -1;
s32 jonrotx = -16384;
s32 jonroty = 16384;
f32 WBICONSCALE = 1.0f;
f32 WBSMALLMAG = 32.0f;
f32 WBMOVESPEED = 1.0f;
f32 BossYAdj = 30.0f;
f32 WBMASKLEFTRAD = 4.0f;
f32 WBMASKRIGHTRAD = 4.0f;
f32 WBMASKEYESRAD = 2.0f;
f32 WBMASKCHESTRAD = 6.0f;
f32 WBSKELTIME = 0.04f;
f32 WBSKELTIMERAND = 0.1f;
f32 EYEBOLTFIRESPEED = 25.0f;
f32 EYEBOLTFIRESCALEX = 4.0f;
f32 EYEBOLTFIRESCALEY = 4.0f;
f32 EYEBOLTFIREY = -5.0f;
f32 WBANGSCALE = 4.0f;
f32 WBANGOFF = -27.5f;
f32 WBANGSCALE2 = 6.0f;
f32 WBANGOFF2 = -30.0f;
f32 WBANGSCALE3 = 5.0f;
f32 WBSNOWBALLSPEED = 60.0f;
f32 WBSNOWCONESPEED = 80.0f;
f32 WBLeftStartFrame = 35.0f;
f32 WBLeftStopFrame = 146.0f;
f32 WBLeftStopTurnFrame = 147.0f;
f32 WBDISTANCEYSPEED = 2.0f;
f32 WBDISTANCEYTIME = 0.2f;
f32 WBDISTANCESPEED = 32.0f;
f32 WBDISTANCETIME = 0.5f;
f32 WBLOLOSCALE = 5.0f;
f32 WBLOLOTILTX = 90.0f;
f32 WBAKUAKUSCALE = 5.0f;
s32 BALLATTACKVOL = 20479;
s32 BEAMVOL = 12287;
s32 EYEATTACKVOL = 6144;
s32 CHESTATTACKAVOL = 8191;
s32 CHESTATTACKBVOL = 24575;
f32 CHESTATTACKSOUNDTIME = 1.0f;
f32 EYESTOPTIME = 150.0f;
s32 HitVol = 16383;
s32 HitPitch = 512;
s32 HitId = 111;
f32 FireVal = 0.3f;
f32 TargetScale = 1.0f;
f32 BIGSCALE = 1.2f;
f32 SMALLSCALE = 0.8f;
f32 FireFlyStartAngle = 225.0f;
f32 FIREFLYVICDANCETIME = 10.0f;
f32 MatchMaxDist = 3.0f;
f32 MatchMinDist = 3.0f;
f32 MatchTimer = 0.0f;
f32 PrePanTime = 2.0f;
f32 PanSeekSpeed = 2.0f;
f32 MinPanSeekSpeed = 0.3f;
f32 FFINTROHEIGHT = 0.0f;
f32 FFINTROCAMYOFF = 0.75f;
f32 WRStartAng = 140.0f;
f32 CrashteroidsStartAngle = 225.0f;
f32 LighteningFallXMin = -10.0f;
f32 LighteningFallXMax = 10.0f;
f32 HailFallSpeed = 6.0f;
f32 HAILFLYUPSPEED = 120.0f;
f32 HAILTIMERANGE = 0.5f;
f32 HAILHIDDENTIME = 0.5f;
f32 HAILCOLLRAD = 1.0f;
s32 SkipNum = 7;
s32 SkipLeftA = 5;
s32 SkipLeftB = 22;
s32 SkipLeftC = 15;
s32 MyAttNum = 0;
f32 BOLTHOMESEEKTIME = 0.6f;
f32 BOLTHOMESEEKSPEED = 4.5f;
f32 WBTYPE2SCALETIME = 0.125f;
f32 WBBOSSINTRODIST = 48.0f;
s32 AMBIENTVOL = 16383;
f32 WBScrollSpeed = 96.0f;
f32 WBINTROLOOKAHEAD = 4.0f;
s32 AtlasEmbeddedFrame = 0;
f32 BALLBUZZSLOPE = 6.0f;
f32 BALLBUZZMIN = 0.4f;
f32 BALLBUZZLIMCOS = 0.6f;
f32 BALLHITVOLDIV = 1.0f;
f32 BALLHITVOLBASE = 1.0f;
s32 ATLASBOOSTVOL = 16383;
f32 BALLRUMBLESLOPE = 128.0f;
f32 ATLASBOOSTSPEED = 13.0f;
s32 ebgo = 0;

void PrintEarthBossAction(struct NEWBUGGY *boss)
{
    char *name;

    NuFntSet(app_fnt);
    NuFntSetPen(0x7F7F7F7F);
    NuFntScale(0x10, 0x10);
    switch (boss->ball_action) {
    case 0:
        name = "Enter";
        break;
    case 1:
        name = "Form A";
        break;
    case 2:
        name = "Form B";
        break;
    case 3:
        name = "Stand";
        break;
    case 4:
        name = "Roll";
        break;
    case 5:
        name = "WaitRoll";
        break;
    case 6:
        name = "Slow";
        break;
    case 7:
        name = "Been Hit";
        break;
    case 10:
        name = "Wait Camera";
        break;
    case 8:
        name = "Wait Big Jump";
        break;
    case 9:
        name = "Big Jump";
        break;
    case 11:
        name = "Whacko";
        break;
    default:
        name = "";
        break;
    }
    NuFntPrintEx(0x80, SHEIGHT * 2, 0, "Action %s", name);
    NuFntPrintEx(0x80, SHEIGHT * 5 / 2, 0, "Timer %1.3f", boss->actiontimer);
    NuFntPrintEx(0x80, SHEIGHT * 3, 0, "HitMag %1.3f", HitMag);
    NuFntPrintEx(0x80, SHEIGHT * 7 / 2, 0, "Damage %d", AtlasHitsLost);
    if (earthboss_whacko[0] != 0) {
        NuFntPrintEx(0x80, SHEIGHT * 4, 0, "Whacko");
    }
    if (JonnyOn != 0) {
        NuFntPrintEx(0x80, SHEIGHT * 9 / 2, 0, "Jonny On");
    }
}


extern struct nuvec_s *SetNuVecPntrA(f32 x, f32 y, f32 z);

void ProcessEarthBossActions(struct NEWBUGGY *boss)
{
    struct nuvec_s rel;
    f32 diff;

    if (EarthBossDeathEffect != 0 && ProcessTimer(&EarthBossDeathTimer)) {
        boss->dead = 1;
    }
    if (boss->dead != 0) {
        if (PlayerAtlas.inhibitcontroltimer == 0.0f) {
            RumbleCamTweenDest = 0.0f;
        }
        UpdateRumbleCamTween();
        return;
    }
    SeekHalfLife(&RumbleCamVal, 0.0f, BumpSpeed, NW_HZ(0.02f, 0.016666668f));
    NuVecSub(&rel, &RumbleCamPos, &boss->ball_pos);
    boss->destcrunchy = NuAtan2D(-rel.x, -rel.z) / 182.04445f;
    if (GetRumbleTotalRoks() > 0 && GetRumblePlayerRoks() == GetRumbleTotalRoks() && boss->whacko == 0) {
        boss->whacko = 1;
        PlayerAtlas.inhibitcontroltimer = 200.0f;
        ShootRoksSkyward();
        if (boss->ball_action == 5) {
            boss->ball_action = 0xB;
        } else if ((boss->onground & 3) && boss->ball_pos.y < -2.0f) {
            boss->ball_action = 0xB;
        } else {
            boss->ball_action = 6;
        }
        boss->rollacctimer = 0.0f;
        boss->actiontimer = 0.0f;
    }
    if (boss->whacko != 0) {
        PlayerAtlas.ball_vel = v000;
    }
    if (boss->ball_action == 4 || boss->ball_action == 6 || boss->ball_action == 5) {
        if (boss->ball_beenhit >= 3) {
            struct jeeprock_s *rock = &JeepRock[boss->ball_beenhit - 3];

            if (rock->owner == 1) {
                rock->owner = 0;
            } else {
                rock->owner = -1;
                rock->flametimer = 7.0f;
            }
            boss->ball_beenhit = 0;
        }
    }
    if (boss->ball_action != boss->lastaction) {
        switch (boss->ball_action) {
        case 11:
            boss->actiontimer = 1.0f;
            break;
        case 0:
            PlayerAtlas.inhibitcontroltimer = 20.0f;

            JonnyOn = 0;
            EarthBossJustEntered = 1;
            boss->actiontimer = 2.4f;
            boss->actiontimer2 = 1.0f;
            boss->cantbehit = 1;
            boss->drawcrunch = 1;

            boss->rollspeed = 0.0f;
            boss->rollacctimer = 0.0f;
            boss->drawshell = 0;
            RumbleCamTweenDest = 1.0f;
            MyChangeAnim(&boss->crunch, 0x22);
            break;
        case 1:
            boss->cantbehit = 1;
            boss->drawcrunch = 1;
            boss->rollspeed = 0.0f;
            boss->rollacctimer = 0.0f;
            boss->drawshell = 0;
            gamesfx_pitch = FORMPITCH;
            MyGameSfx(0xD4, &rumble_sfx_pos, 0x7FFF);
            MyChangeAnim(&boss->crunch, 0x2B);
            break;
        case 2:
            boss->cantbehit = 1;
            boss->drawcrunch = 1;
            boss->drawshell = 1;
            MyResetAnimPacket(&boss->shell, 0x1F);
            boss->actiontimer = 0.0f;
            break;
        case 10:
            boss->drawcrunch = 0;
            boss->cantbehit = 1;
            boss->drawshell = 1;

            break;
        case 3:
            RumbleCamTweenDest = 1.0f;
            boss->drawcrunch = 1;
            boss->drawshell = 1;
            boss->rotquat
 = SetNuQuat(0.0f, 0.0f, 0.0f, 1.0f);
            MyGameSfx(0x78, &rumble_sfx_pos, 0x7FFF);
            SetNewMaskStuff(1, &rumble_sfx_pos, SetNuVecPntr(0.0f, 12.5f, 0.0f), 0.0f, -540.0f, 0.0f, 1, 0, 2.0f, 0.0f);
            SetNewMaskStuff(1, &rumble_sfx_pos, SetNuVecPntr(0.0f, 1.0f, 0.0f), 1.7f, -540.0f, 1.0f, 1, 0, 2.0f, 0.0f);
            MyChangeAnim(&boss->shell, 0x18);
            MyChangeAnim(&boss->crunch, 0x35);
            boss->cantbehit = 0;
            boss->actiontimer = 4.0f;
            break;
        case 4:
            boss->cantbehit = 1;
            boss->drawcrunch = 0;
            boss->drawshell = 1;
            boss->rollspeed = 10.0f;
            boss->rollacctimer = 0.5f;
            boss->actiontimer = 3.0f;
            EarthBossAimLocation = PlayerAtlas.ball_pos;
            EarthBossAimVelocity = PlayerAtlas.ball_vel;

            break;
        case 5:
            boss->cantbehit = 1;
            boss->actiontimer = 2.5f;
            break;
        case 6:
            boss->cantbehit = 1;
            boss->actiontimer = 3.0f;
            break;
        case 7:
            if (--boss->ball_health > 0) {
                SetNewMaskStuff(1, &rumble_sfx_pos, SetNuVecPntr(0.0f, 12.5f, 0.0f), 3.0f, -540.0f, 0.5f, 1, 1, 2.0f, 0.0f);
            }
            boss->actiontimer2 = 0.5f;
            EarthBossDeathEffect = 0;
            boss->crunchy = boss->destcrunchy;
            boss->cantbehit = 1;
            boss->drawcrunch = 1;

            JonnyOn = 0;
            MyChangeAnim(&boss->crunch, (boss->ball_health > 0) ? 0x19 : 0xE);
            boss->actiontimer = 5.0f;
            break;
        case 8:
            boss->actiontimer = 0.5f;
            break;
        case 9:
            boss->ball_vel = SetNuVec(0.0f, 7.0f, 0.0f);
            boss->onground = 0;
            boss->whacko = 0;
            break;
        }
    }
    boss->lastaction = boss->ball_action;
    if (boss->ball_action != 7 && boss->ball_action != 1 && boss->ball_action != 2) {
        diff = Rationalise360f(boss->destcrunchy - boss->crunchy);
        if (diff >= 0.0f) {
            diff -= NW_HZ(7.2f, 6.0000005f);
            if (diff < 0.0f) {
                diff = 0.0f;
            }
        } else {
            diff += NW_HZ(7.2f, 6.0000005f);
            if (diff > 0.0f) {
                diff = 0.0f;
            }
        }
        boss->crunchy = boss->destcrunchy - diff;
    }
    switch (boss->ball_action) {
    case 8:
        boss->ball_vel = v000;
        if (ProcessTimer(&boss->actiontimer)) {
            boss->ball_action = 9;
        }
        break;
    case 11:
        boss->ball_vel = v000;
        if (AllRoksSkyward()) {
            if (JonnyOn == 0) {
                gamesfx_pitch = ROKBPITCH;
                MyGameSfx(0xCE, &rumble_sfx_pos, 0x7FFF);
            }
            JonnyOn = 1;
            ebgo = 0;
            RumbleCamTweenDest = 1.0f;
            if (RumbleCamTween == 1.0f && ProcessTimer(&boss->actiontimer)) {
                boss->ball_action = 3;
                ebgo = 1;
            }
        }
        break;
    case 0:
        if (ProcessTimer(&boss->actiontimer2)) {
            boss->actiontimer2 = 1000.0f;
            SetNewMaskStuff(0, &rumble_sfx_pos, SetNuVecPntr(0.0f, 12.5f, 0.0f), 0.0f, 360.0f, 0.0f, 1, 0, 2.0f, 0.0f);
            SetNewMaskStuff(0, &rumble_sfx_pos, SetNuVecPntr(0.0f, 1.5f, 0.0f), 1.5f, 360.0f, 1.0f, 1, 0, 2.0f, 0.0f);
        }
        boss->ball_vel = v000;
        if (ProcessTimer(&boss->actiontimer)) {
            boss->ball_action = 1;
        }
        break;
    case 1:
        boss->ball_vel = v000;
        boss->ball_action = 2;
        break;
    case 2:
        boss->ball_vel = v000;
        if (boss->shell.Anim.flags & 1) {
            SetNewMaskStuff(0, &rumble_sfx_pos, SetNuVecPntr(0.0f, 12.5f, 0.0f), 3.0f, 360.0f, 0.5f, 1, 1, 2.0f, 0.0f);
            boss->ball_action = 8;
        }
        break;
    case 10:
        boss->ball_vel = v000;
        if (RumbleCamTweenDest == RumbleCamTween) {
            boss->numattacks = 3;
            boss->ball_action = 4;
        }
        break;
    case 3:
        boss->ball_vel = v000;
        if (boss->crunch.Anim.flags & 1) {
            boss->ball_action = 7;
        }
        break;
    case 4:
        if (!ProcessTimer(&boss->rollacctimer) && boss->ball_beenhit == 0) {
            struct nuvec_s aim;
            f32 speed;

            speed = boss->rollspeed * (0.5f - boss->rollacctimer) * 2.0f;
            if (speed < 0.1f) {
                speed = 0.1f;
            }
            NuVecSub(&aim, &EarthBossAimLocation, &boss->ball_pos);
            NuVecScaleAccum(&aim, &EarthBossAimVelocity, NuVecMag(&aim) / speed);
            NuVecNorm(&aim, &aim);
            NuVecScale(&boss->ball_vel, &aim, speed);
            boss->ball_vel.y = -speed / 3.0f;
        } else if (ProcessTimer(&boss->actiontimer) || boss->ball_beenhit != 0) {
            boss->ball_action = 6;
        }
        break;
    case 5:
        if (EarthBossJustEntered != 0) {
            boss->ball_vel = v000;
        }
        if (ProcessTimer(&boss->actiontimer) && (PlayerAtlas.onground & 3)) {
            boss->ball_action = 4;
            EarthBossJustEntered = 0;
        }
        break;
    case 6:
        if ((boss->onground & 3) && (boss->ball_pos.y < -2.0f || boss->numattacks > 0)) {
            SeekHalfLifeNUVEC(&boss->ball_vel, &v000, 0.2f, NW_HZ(0.02f, 0.016666668f));
            if (NuVecMagSqr(&boss->ball_vel) < 0.5f) {
                if (boss->whacko != 0) {
                    boss->ball_action = 0xB;
                } else if (boss->numattacks > 0) {
                    boss->ball_action = 5;
                } else {
                    boss->ball_action = 3;
                }
            }
        }
        break;
    case 7:
        boss->ball_vel = v000;
        if (boss->ball_health > 0 && ProcessTimer(&boss->actiontimer2)) {
            boss->actiontimer2 = 1000.0f;
            SetNewMaskStuff(0, &rumble_sfx_pos, SetNuVecPntr(0.0f, 12.5f, 0.0f), 0.0f, 360.0f, 0.0f, 1, 0, 2.0f, 0.0f);
            SetNewMaskStuff(0, &rumble_sfx_pos, SetNuVecPntr(0.0f, 1.5f, 0.0f), 1.5f, 360.0f, 1.0f, 1, 0, 2.0f, 0.0f);
        }
        if (boss->crunch.Anim.action == 0x19) {
            f32 time = boss->crunch.Anim.anim_time;
            f32 last = time - boss->crunch.Model->animlist[0x19]->speed * NW_HZ(0.59999996f, 0.5f);

            if (time >= 52.0f && last < 52.0f) {
                MyGameSfx(0x54, &rumble_sfx_pos, BOSSLANDVOL);
            }
        }

        if (boss->crunch.Anim.flags & 1) {
            if (boss->ball_health > 0) {
                boss->ball_action = 1;
            } else {
                if (EarthBossDeathEffect == 0) {
                    struct nuvec_s pos;

                    pos = boss->ball_pos;
                    pos.y -= boss->ball_radius * 0.75f;
                    AddGameDebris(0x9A, &pos);
                    MyGameSfx(0xD5, &boss->ball_pos, 0x7FFF);
                    EarthBossDeathEffect = 1;
                    EarthBossDeathTimer = ROKDEATHTIME;
                    SetNewMaskStuff(1, &rumble_sfx_pos, SetNuVecPntr(0.0f, 12.5f, 0.0f), 3.0f, -540.0f, 0.5f, 1, 1, 2.0f, 0.0f);
                }
                ChrisBigBossDead = 1;
                PlayerAtlas.inhibitcontroltimer = 0.5f;
                boss->whacko = 0;
            }
        }
        break;
    case 9:
        if (boss->onground != 0) {
            struct nuvec_s pos;

            pos = boss->ball_pos;
            NuVecScaleAccum(&pos, &boss->lastnormal, -boss->ball_radius);
            AddGameDebris(0x99, &pos);
            MyGameSfx(4, &boss->ball_pos, BIGLANDVOL);
            RumbleCamTweenInterestDest = 0.0f;
            RumbleCamVal = BumpAmount;
            RumbleCamTweenDest = 0.25f;
            RumbleDisplayMode = 2;
            AddRockVel(SetNuVecPntr(12.0f, 6.0f, 0.0f), SetNuVecPntrA(0.0f, -8.0f, -12.0f), 0.5f, 0);
            AddRockVel(SetNuVecPntr(-12.0f, 6.0f, 0.0f), SetNuVecPntrA(0.0f, -8.0f, 12.0f), 0.5f, 0);
            if (boss->ball_health < 3) {
                AddRockVel(SetNuVecPntr(0.0f, 6.0f, 12.0f), SetNuVecPntrA(-12.0f, -8.0f, 0.0f), 0.5f, 0);
            }
            if (boss->ball_health < 2) {
                AddRockVel(SetNuVecPntr(0.0f, 6.0f, -12.0f), SetNuVecPntrA(12.0f, -8.0f, 0.0f), 0.5f, 0);
            }
            boss->numattacks = 3;
            boss->ball_action = 5;
            PlayerAtlas.inhibitcontroltimer = 0.0f;
            PlayerAtlas.ball_vel.y = 4.0f;
            PlayerAtlas.onground = 0;
        }
        break;
    }
    boss->ball_beenhit = 0;
    UpdateRumbleCamTween();
    if (boss->ball_action != 9) {
        boss->interestpoint = boss->ball_pos;
    }
    MyAnimateModelNew(&boss->shell, NW_HZ(0.59999996f, 0.5f));
    MyAnimateModelNew(&boss->crunch, NW_HZ(0.59999996f, 0.5f));
    if (JonnyOn != 0) {
        JonnyParticles(&rumble_sfx_pos);
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


extern void NuVecMtxRotateVU0(struct nuvec_s *dst, struct nuvec_s *src, struct numtx_s *mtx);

void GliderFire(struct GLIDERSTRUCT *glider, struct nupad_s *pad)
{
    static s32 LastFireButton;
    static f32 FireBleepTimer;
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
        NuVecScaleAccum(&GliderTargetPos, &spacetargetdir, NW_HZ(0.74f, 0.741666675f));
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
        NuVecScaleAccum(&spacepos, &spacevel, -NW_HZ(0.01f, 0.008333334f));
        AddGliderBullet(&mat, &spacepos, &spacevel, 0);
        NuVecMtxRotateVU0(&spacevel, SetNuVecPntr(-SpaceFirePosB.x / 0.75f, 0.0f, -120.0f), &mat);
        NuVecMtxRotateVU0(&spacepos, &SpaceFirePosB, &mat);
        NuVecAdd(&spacepos, &spacepos, &glider->Position);
        NuVecScaleAccum(&spacepos, &spacevel, -NW_HZ(0.01f, 0.008333334f));
        AddGliderBullet(&mat, &spacepos, &spacevel, 0);
        glider->FireTimer = NW_HZ(0.12f, 0.10000001f);
        MyGameSfx(0xCF, &spacepos, 0x1FFF);
        NewBuzz(&player->rumble, 3);
        break;
    case 0xD:
        tornscale = (struct nuvec_s){0.5f, 0.5f, 0.5f};
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
        glider->FireTimer = NW_HZ(0.12f, 0.10000001f);
        GameSfx(0x8B, &tornpos);
        NewBuzz(&player->rumble, 3);
        break;
    case 0x18:
        wbscale = (struct nuvec_s){0.5f, 0.5f, 0.5f};
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
        glider->FireTimer = WBBULLETFIRERATE * NW_HZ(0.02f, 0.016666668f);
        GameSfx(0x55, (struct nuvec_s *)&glider->Cre->mtxLOCATOR[1][0]._30);
        NewBuzz(&player->rumble, 3);
        break;
    case 0x12:
    case 0x24:
        if (glider->TargetOn == 0) {
            glider->TargetTimer = 0.0f;
            glider->TargetedTime = 0.0f;
        } else {
            glider->TargetedTime += NW_HZ(0.02f, 0.016666668f);
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


inline s32 AddGliderBomb(struct nuvec_s *pos, struct nuvec_s *vel, f32 angy, s32 enemy,
                  struct nuvec_s *targetpoint, struct nuvec_s *targetvel, s32 moving)
{
    struct gliderbomb_s *bomb;

    bomb = GrabGliderBomb(enemy);
    if (bomb != 0) {
        bomb->life = NW_HZ(500, 600);
        bomb->pos = *pos;
        bomb->vel = *vel;
        NuVecScale(&bomb->vel, vel, NW_HZ(0.02f, 0.016666668f));
        bomb->angy = angy;
        if (enemy) {
            bomb->gravity = -NW_HZ(0.004f, 0.0033333336f);
        } else {
            bomb->gravity = -NW_HZ(0.004f, 0.0033333336f);
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

void ProcessBattleShip(struct battleship_s *ship)
{
    struct nuvec_s pos;
    struct numtx_s m;
    struct nuvec_s dir;
    struct nuvec_s start;
    f32 angle;
    s32 locator;
    s32 i;

    MyAnimateModelNew(&ship->draw, NW_HZ(0.59999996f, 0.5f));
    if (ship->hitpoints > 0) {
        CollideGliderBullets(&ship->pos, 6.0f, 1, 1.0f, 0, 1);
    }
    if (ship->rammed != 0 || CollideGliderBombs(&ship->pos, &BattleShipCollScale, 0, ship->yaw)) {
        AddGameDebris(0x43, &GliderBombCollisionPos);
        if (ship->rammed != 0) {
            ship->hitpoints = 1;
        }
        ship->rammed = 0;
        ship->hitpoints--;
        switch (ship->hitpoints) {
        case 2:
            ship->destpitch = 6.5f;
            ship->destroll = 5.0f;
            ship->desty = -1.0f;
            ship->seek = 1.0f;
            break;
        case 1:
            ship->destpitch = 13.0f;
            ship->destroll = 10.0f;
            ship->desty = -1.5f;
            ship->seek = 1.5f;
            break;
        default:
            ship->destpitch = 25.0f;
            ship->destroll = 50.0f;
            ship->desty = -5.0f;
            ship->seek = 6.0f;
            break;
        }
        AddScreenWumpa(ship->pos.x, ship->pos.y, ship->pos.z, 1);
        if (ship->hitpoints > 0) {
            MyGameSfx(0xC7, &ship->pos, BATTLESHIPAVOLUME);
        } else {
            MyGameSfx(0xCC, &ship->pos, BATTLESHIPBVOLUME);
        }
        ClockOff();
    }
    SeekAngHalfLife360f(&ship->pitch, ship->destpitch, 1.0f, NW_HZ(0.02f, 0.016666668f));
    SeekAngHalfLife360f(&ship->roll, ship->destroll, 1.0f, NW_HZ(0.02f, 0.016666668f));
    SeekHalfLife(&ship->pos.y, ship->desty, 1.0f, NW_HZ(0.02f, 0.016666668f));
    for (i = 0; i < 3 - ship->hitpoints; i++) {
        if (jonframe1 % 3 == i) {
            locator = BSTab[i];
            if (ship->locators[locator]._31 < 0.0f) {
                pos.x = ship->locators[locator]._30;
                pos.y = 0.5f;
                pos.z = ship->locators[locator]._32;
                AddVariableShotDebrisEffect(GDeb[133].i, &pos, 1, 0, 0);
            } else {
                AddVariableShotDebrisEffect(GDeb[132].i,
                                            (struct nuvec_s *)&ship->locators[locator]._30, 1, 0, 0);
            }
        }
    }
    if (ship->hitpoints > 0) {
        for (i = 0; i < 1; i++) {
            if (ProcessTimer(&ship->firetimer[i])) {
                angle = ship->yaw + frand() * 6.0f;
                NuMtxSetRotationX(&m, ship->pitch * 182.04445f);
                NuMtxRotateZ(&m, ship->roll * 182.04445f);
                NuMtxRotateY(&m, angle * 182.04445f);
                start = ship->pos;
                start = *(struct nuvec_s *)&ship->locators[i]._30;
                dir = SetNuVec(0.0f, 15.0f, -20.0f);
                NuVecMtxRotate(&dir, &dir, &m);
                if (AddGliderBomb(&start, &dir, angle, 1, 0, 0, 0)) {
                    ship->firetimer[i] = frand() * 3.0f + 5.0f;
                    MyResetAnimPacket(&ship->draw, 0x1B);
                    MyGameSfx(0xC8, &start, 0xFFF);
                    AddGameDebris(0x1A, &start);
                }
            }
        }
    }
}

extern f32 fsign(f32 x);

void ControlGliderWeatherBoss(struct GLIDERSTRUCT *glider, struct nupad_s *pad)
{
    f32 OldTiltZ;
    struct nuvec_s *campos;
    struct nuvec_s *bosspos;
    struct nuvec_s offset;

    if (glider->TerminalDive == 0 && WBIntroOn == 0 && FlyingLevelExtro == 0) {
        if ((((volatile struct nupad_s *)pad)->paddata & 0x2) && 
            (glider->BarrelRoll == 0 || glider->BarrelRoll == 1)
            && glider->Position.x > -WBLIMX) {
            glider->BarrelRoll = 3;
            GameSfx(0x5C, &glider->Position);
        } else if ((((pad->paddata & 0x1) != 0) && ((glider->BarrelRoll == -1) || (glider->BarrelRoll == 0))) &&
         (glider->Position.x < WBLIMX)) {
            glider->BarrelRoll = -3;
            GameSfx(0x5C, &glider->Position);
        }
    }
    if (glider->BarrelRoll != 0) {
        OldTiltZ = Rationalise360f(glider->TiltZ);
        if (glider->BarrelRoll == 2 && glider->TiltZ > -45.0f) {
            glider->BarrelRoll = 1;
        }
        if (glider->BarrelRoll == -2 && glider->TiltZ < 45.0f) {
            glider->BarrelRoll = -1;
        }
        if ((glider->BarrelRoll < -1 || glider->BarrelRoll > 1) 
            || glider->TiltZ > 30.0f || glider->TiltZ < -30.0f) {
            SeekHalfLife(&glider->BarrelDelta, NW_HZ(7.2f, 6.0000005f), 0.05f, NW_HZ(0.02f, 0.016666668f));
        } else if (glider->TiltZ > 1.0f || glider->TiltZ < -1.0f) {
            glider->BarrelDelta = (f32)fabs(glider->TiltZ * 20.0f * NW_HZ(0.02f, 0.016666668f) * 0.5f);
        } else {
            glider->BarrelDelta = 1.0f;
        }
        if (glider->BarrelRoll > 0) {
            glider->TiltZ = Rationalise360f(OldTiltZ + glider->BarrelDelta);
            if (OldTiltZ >= 0.0f && glider->TiltZ < 0.0f) {
                glider->BarrelRoll = 2;
            }
        } else {
            glider->TiltZ = Rationalise360f(OldTiltZ - glider->BarrelDelta);
            if (OldTiltZ <= 0.0f && glider->TiltZ > 0.0f) {
                glider->BarrelRoll = -2;
            }
        }
        if ((glider->BarrelRoll == 1 && OldTiltZ < 0.0f && glider->TiltZ >= 0.0f)
            || (glider->BarrelRoll == -1 && OldTiltZ > 0.0f && glider->TiltZ <= 0.0f)) {
            glider->BarrelRoll = 0;
            glider->TiltZ = 0.0f;
        }
    }
    ProcessWeatherBossTarget();
    campos = (struct nuvec_s *)&WeatherBossCamMtx._30;
    bosspos = GetWeatherBossPos();
    NuVecScale(&offset, &WeatherBossTargetPos, (bosspos->z - WeatherBossCamMtx._32) / WeatherBossTargetPos.z);
    NuVecAdd(&WeatherBossFirePoint, &offset, campos);
}

void ControlGlider(struct GLIDERSTRUCT *glider, struct nupad_s *pad)
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
            SeekAngHalfLife360f(&glider->TiltX, glider->DestTiltX, CRASHTEROIDSSeekTiltXTimeZero,
                                NW_HZ(0.02f, 0.016666668f));
        } else {
            SeekAngHalfLife360f(&glider->TiltX, glider->DestTiltX, CRASHTEROIDSSeekTiltXTimeNonZero,
                                NW_HZ(0.02f, 0.016666668f));
        }
    } else if (Level == 0xD) {
        if (glider->DestTiltX == 0.0f) {
            SeekAngHalfLife360f(&glider->TiltX, glider->DestTiltX, TORNSeekTiltXTimeZero, NW_HZ(0.02f, 0.016666668f));
        } else {
            SeekAngHalfLife360f(&glider->TiltX, glider->DestTiltX, TORNSeekTiltXTimeNonZero, NW_HZ(0.02f, 0.016666668f));
        }
    } else {
        SeekAngHalfLife360f(&glider->TiltX, glider->DestTiltX, 0.2f, NW_HZ(0.02f, 0.016666668f));
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
            SeekHalfLife(&glider->BarrelDelta, NW_HZ(7.2f, 6.0000005f), 0.05f, NW_HZ(0.02f, 0.016666668f));
        } else if (glider->TiltZ > 1.0f || glider->TiltZ < -1.0f) {
            absval = glider->TiltZ * 20.0f * NW_HZ(0.02f, 0.016666668f) * 0.5f;
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
        SeekAngHalfLife360f(&glider->TiltZ, glider->DestTiltZ, SeekTiltZOveride, NW_HZ(0.02f, 0.016666668f));
    } else if (glider->DestTiltZ == 0.0f) {
        SeekAngHalfLife360f(&glider->TiltZ, glider->DestTiltZ, SeekTiltZZero, NW_HZ(0.02f, 0.016666668f));
    } else {
        SeekAngHalfLife360f(&glider->TiltZ, glider->DestTiltZ, SeekTiltZNonZero, NW_HZ(0.02f, 0.016666668f));
    }
}


void ProcessGliderMovement(struct GLIDERSTRUCT *glider, f32 dt)
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
        glider->AngleY += glider->TornadoSpin * NW_HZ(0.02f, 0.016666668f) * glider->InTornadoTime;
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
        SeekHalfLife(&glider->Speed, glider->TargetSpeed, 1.0f, NW_HZ(0.02f, 0.016666668f));
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
