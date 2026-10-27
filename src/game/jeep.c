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

#include "nu_libc.h"

#include "numath/numtx.h"
#include "numath/nuquat.h"
#include "numath/nutrig.h"
#include "numath/nuvec.h"
#include "nu3d/nugscn.h"
#include "game/gametypes.h"
#include "gamelib/terrain.h"
#include "game/creature.h"
#include "nucore/nucoretypes.h"
#include "nups2/ps2ctl.h"
#include "game/jeep.h"

extern struct nuvec_s BaseWheelPosition[];
extern f32 DotProduct(struct nuvec_s *a, struct nuvec_s *b);
extern void ApplyFriction(f32 *val, f32 rate, f32 dt);
extern struct nuvec_s WorldAxis[3];
extern f32 JeepCamTween;
extern f32 JeepRailCamTween;
extern f32 JeepRailCamTween2;
extern s32 ResetJeepCamera;
extern void NewInitTrail(void);
extern f32 ASin360f(f32 x);
extern s32 MyDrawModelNew(struct MYDRAW *Draw, struct numtx_s *mC,
                          struct numtx_s *loc_mtx);
extern void GetLocatorMtx(struct CharacterModel *model, struct numtx_s *dest,
                          f32 angle);

struct fireboss_s;

extern s32 DrawFireBoss(struct fireboss_s *fb);
extern void AddVariableShotDebrisEffect(s32 effect, struct nuvec_s *pos, s32 n,
                                        s32 xrot, s32 yrot);
extern void DrawJeepRocks(void);
extern void DrawVehMasks(void);
extern void *app_fnt;
extern void NuFntSet(void *fnt);
extern void NuFntSetPen(u32 colour);
extern f32 NuFsqrt(f32 x);
extern f32 NuTrigTable[];
extern void SeekHalfLife(f32 *dest, f32 target, f32 halflife, f32 dt);
extern f32 fsign(f32 x);

extern struct spltab_s SplTab[];
extern void PointAlongSpline(struct nugspline_s *spl, f32 ratio,
                             struct nuvec_s *out, void *a3, void *a4);

extern void FindSplineTargetPoint(struct SPLINEFOLLOW *Spline, s32 Control,
                                  struct nuvec_s *Point,
                                  struct nuvec_s *Direction, s32 Wrap,
                                  s32 BigLook);
extern f32 FindSplineClosestPointAndDist(struct SPLINEFOLLOW *Spline,
                                         s32 Control, struct nuvec_s *Pos,
                                         struct nuvec_s *Closest, s32 Wrap,
                                         s32 BigLook);

struct fireboss_s {
    struct numtx_s Locator[16];     /* 0x000 */
    s32 DropSide;                   /* 0x400 */
    f32 FootTime;                   /* 0x404 */
    s32 HitPoints;                  /* 0x408 */
    s32 Pass;                       /* 0x40C */
    s32 Active;                     /* 0x410 */
    f32 AngleY;                     /* 0x414 */
    struct nuvec_s Pos;             /* 0x418 */
    struct MYDRAW MainDraw;         /* 0x424 */
    struct MYDRAW ExplodeDraw;      /* 0x504 */
    struct SPLINEFOLLOW MainSpline; /* 0x5E4 */
    s32 Seen;                       /* 0x614 */
    f32 Speed;                      /* 0x618 */
    s32 Action;                     /* 0x61C */
    s32 LastAction;                 /* 0x620 */
    f32 ActionTimer;                /* 0x624 */
    f32 ActionTimer2;               /* 0x628 */
    struct nuvec_s Vel;             /* 0x62C */
    s32 Dead;                       /* 0x638 */
    struct numtx_s DrawMtx;         /* 0x63C */
    s32 BeenHitThisPass;            /* 0x67C */
    f32 BeenHitThisPassTimer;       /* 0x680 */
    void *Rock;                     /* 0x684 */
} __attribute__((aligned(16))); /* 0x690 */
extern f32 FIREBOSSSTART;

extern struct JEEPSTRUCT PlayerJeep;

extern void ProcessJeepTrail(struct SIMWHEEL *Wheel, s32 TrailIndx);

struct jeeptrail_s {
    struct nuvec_s pos1;            /* 0x00 */
    struct nuvec_s pos2;            /* 0x0C */
    s32 intensity;                  /* 0x18 */
    s32 RealIntensity;              /* 0x1C */
};
extern void NuRndrTrail(s32 pntr, struct jeeptrail_s *trail, s32 count);

struct JEEPBALLOON {
    struct nuvec_s Pos;             /* 0x00 */
    struct nuvec_s Vel;             /* 0x0C */
    s32 Active;                     /* 0x18 */
    s32 Seen;                       /* 0x1C */
    s32 Explode;                    /* 0x20 */
    s32 SmallDamage;                /* 0x24 */
    f32 Life;                       /* 0x28 */
    s16 AngY;                       /* 0x2C */
    s16 unk;                        /* 0x2E */
};
extern void ProcessJeepBalloon(struct JEEPBALLOON *b);
extern void DrawJeepBalloon(struct JEEPBALLOON *b);

extern struct objtab_s ObjTab[];
extern struct numtx_s mTEMP;
extern s32 WallOfFireOn;
extern f32 WallOfFireAngleY;
extern s32 Paused;

extern struct gdeb_s GDeb[];
extern s32 NuRndrGScnObj(void *gobj, struct numtx_s *m);

struct enemyjeep_s {
    struct nuvec_s Position;        /* 0x000 */
    struct MYDRAW Draw;             /* 0x00C */
    f32 PlayBackTime;               /* 0x0EC */
    s32 PlayBackMax;                /* 0x0F0 */
    struct jointctrl_s Joints[8];   /* 0x0F4 */
    struct numtx_s Locators[16];    /* 0x294 */
    s32 LocatorValidFrame;          /* 0x694 */
    struct SIMWHEEL TrailWheel[4];  /* 0x698 */
    f32 Scale;                      /* 0x738 */
    s8 Active;                      /* 0x73C */
    s8 DrawOn;                      /* 0x73D */
    s8 TrailOn;                     /* 0x73E */
    s8 Pad;                         /* 0x73F */
};
extern void DrawEnemyJeep(struct enemyjeep_s *j);
extern void ProcessEnemyJeep(struct enemyjeep_s *j);
extern void MyAnimateModelNew(void *draw, f32 dt);
extern s32 MyInitModelNew(struct MYDRAW *draw, s32 index, s32 action,
                          s32 numjoints, void *jointlist,
                          struct nuvec_s *pos);

extern void InitEnemyJeeps(void);
extern s32 qrand(void);
extern s32 D_005BBCC4[];              /* PlayerJeep.Finished (far .data -> absolute) */
#define PlayerJeepFinished D_005BBCC4
extern struct nugspline_s *D_00586394[]; /* Rail[0].pCAM (far .data -> absolute) */
extern f32 WesternCountdown;
extern s32 SmokeyFinished;
extern f32 WesternTime;
extern s32 SmokeyCountDownValue;
extern struct nugspline_s *JeepIntroLookSpline;
extern struct nugspline_s *JeepIntroCamSpline;
extern s32 SmokeyCam;
extern s32 SmokeyPosition;
extern s32 CurrentWesternPosition(void);
extern void ProcessJeepTrails(void);

extern char LevelFileName[];
extern union variptr_u superbuffer_ptr;
extern union variptr_u superbuffer_end;
extern struct nugscn_s *world_scene[];
extern void LoadScreen(char *name);

struct BUGGYDATA {
    struct nuquat_s Quat;           /* 0x00 */
    short PosX;                     /* 0x10 */
    short PosY;                     /* 0x12 */
    short PosZ;                     /* 0x14 */
    u16 aWheelAng;                  /* 0x16 */
    s8 WheelHeight[4];              /* 0x18 */
}; /* 0x1C */
extern void NuStrCpy(char *dst, char *src);
extern void NuStrCat(char *dst, char *src);
extern s32 NuFileSize(char *name);
extern s32 NuFileOpen(char *name, s32 mode);
extern s32 NuFileRead(s32 fh, void *buf, s32 size);
extern void NuFileClose(s32 fh);
extern s32 FireBossFinished;
extern s32 FireBossWon;
extern s32 ChrisBigBossDead;
extern s32 VEHICLECONTROL;
extern f32 WaterTimer;
extern f32 FireBossWaterSoundTimer;
extern f32 FIREWATERSOUNDTIME;
extern s32 WATERFIRESOUNDVOL;
extern short SEEKANGSPEED;
extern f32 MAXWATERTIME;
extern f32 WaterLength;
extern f32 WaterWidth;
extern s32 FBSCREAMVOL;
extern f32 AshesMechOutZ;
extern void MyGameSfx(s32 id, struct nuvec_s *pos, s32 vol);
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
extern s32 KillPlayer(struct obj_s *obj, s32 type);




extern s32 ProcessTimer(f32 *Timer);
extern struct nuvec_s *SetNuVecPntr(f32 x, f32 y, f32 z);
extern struct nuvec_s SetNuVec(f32 x, f32 y, f32 z);
extern void AddGameDebris(s32 type, struct nuvec_s *pos);
extern void DriveJeep(struct JEEPSTRUCT *Jeep, struct nupad_s *Pad);
extern void ProcessJeepMovement(struct JEEPSTRUCT *Jeep, f32 DeltaTime);
extern struct nuvec_s TerrainVehicleSoft(struct VEHICLE *Vehicle,
                                         struct nuvec_s *Vel);
extern void FindSurfaceNormalAndUnembedd(struct VEHICLE *Vehicle,
                                         f32 DeltaTime);
extern void SeekAngHalfLife(u16 *dest, short target, f32 halflife, f32 dt);
extern void FindTerrainType(struct JEEPSTRUCT *Jeep);
extern RailPosition *best_cRPos;
extern short JeepBaseAng;
static f32 JeepCamX;
static f32 JeepCamY;
extern short D_005865C0[];
#define FireBossRailAng D_005865C0
extern s32 RotDiff(u16 a, u16 b);
extern void PlayerCreatureCollisions(struct obj_s *obj);
extern s32 HitItems(struct obj_s *obj);
extern s32 HitCrates(struct obj_s *obj, s32 destroy);
extern s32 WumpaCollisions(struct obj_s *obj);
extern void AddGameDebrisRot(s32 i, struct nuvec_s *pos, s32 n, u16 xrot,
                             u16 yrot);
extern struct rail_s Rail[];
extern struct nugspline_s *pVIS;
extern s32 iVIS;
extern s32 JeepCamSnapShot;
extern struct nuvec_s v000;
extern void SeekHalfLifeNUVEC(struct nuvec_s *src, struct nuvec_s *target,
                              f32 halflife, f32 dt);
extern void SeekAngHalfLife360f(f32 *dest, f32 target, f32 halflife, f32 dt);
extern f32 Rationalise360f(f32 a);
extern void JeepCamFollowAng(struct cammtx_s *cam, s32 blend);
extern s32 AddBalloon(struct nuvec_s *Pos, struct nuvec_s *Vel);
extern void DrawCross(struct nuvec_s *pos, f32 size, s32 colour);
extern void DrawLine(struct nuvec_s *p0, struct nuvec_s *p1, s32 colour);
extern void MoveRailPosition(struct nuvec_s *dst, RailPosition *rpos,
                             f32 distance, s32 direction);
extern RailPosition cRPos[];
extern s32 cRPosCOUNT;
extern s32 ChrisInTheHouse;
extern s32 HighCam;

extern s32 JeepFrame;
extern s32 JeepInControl;
extern s32 gggg;
extern s32 JamesInTheHouse;
extern s32 temp_crate_type;
extern s32 Level;
f32 PYROTILTX = 20.0f;
f32 AKUAKUTILTX = -37.5f;
s32 CurrentDebugJeep = -1;
f32 DropAdj = 2.0f;
s32 Replay = 0;
struct nuvec_s WesternEnemyStarts[4] = {
    { -0.9f, 0.0f, 0.0f },
    { 1.8f, 0.0f, 0.0f },
    { 0.9f, 0.0f, -1.5f },
    { -1.8f, 0.0f, -1.5f },
};
struct nuvec_s WesternPlayerStart = { -0.45f, 0.0f, -3.0f };
struct POINTANG WesternRestartPoints[11] = {
    { 1.22f, -0.5f, 145.26f, -3289 },
    { -25.66f, -0.5f, 189.08f, -2860 },
    { -26.71f, -0.15f, 268.25f, 10157 },
    { -8.07f, 0.0f, 291.44f, 1716 },
    { -42.61f, 0.0f, 408.52f, -5589 },
    { -73.24f, -0.5f, 486.65f, 12168 },
    { -2.03f, 0.0f, 519.73f, -1197 },
    { -7.46f, 0.0f, 559.07f, 1257 },
    { -27.59f, 0.0f, 671.38f, 9698 },
    { -9.8f, 0.0f, 691.61f, 1000 },
    { 0.0f, 0.0f, 1000000.0f, 0 },
};
struct nuvec_s ExhaustPos[2] = {
    { 0.055f, 0.38f, 0.6f },
    { -0.055f, 0.38f, 0.6f },
};
extern struct nuvec_s IdealCamPos;
extern struct nuvec_s IdealObjPos;
extern s32 SHEIGHT;
extern void NuFntScale(s32 x, s32 y);
extern void NuFntPrintEx(s32 x, s32 y, s32 colour, char *fmt, ...);
extern void GameSfx(s32 id, struct nuvec_s *pos);
extern s32 WallOfFireAttatched;

struct numtx_s *DrawJeep(struct JEEPSTRUCT *Jeep);

struct numtx_s *DrawPlayerJeep(struct creature_s *Cre) {
    struct NEWBUGGY *Buggy = Cre->Buggy;

    if (Buggy == 0 || VEHICLECONTROL == 0) {
        return 0;
    }
    if (Cre->obj.vehicle == 0x63) {
        return DrawJeep((struct JEEPSTRUCT *)Buggy);
    }
    return 0;
}


inline void NewGenerateJeepMatrix(struct numtx_s *Mat, short YAng,
                                 short SurfaceX, short SurfaceZ, short TiltX,
                                 short TiltZ, struct nuvec_s *Pos) {
    NuMtxSetRotationX(Mat, TiltX);
    NuMtxRotateZ(Mat, TiltZ);
    NuMtxRotateY(Mat, YAng);
    NuMtxRotateZ(Mat, SurfaceZ);
    NuMtxRotateX(Mat, SurfaceX);
    if (Pos != 0) {
        NuMtxTranslate(Mat, Pos);
    }
}

void SpinWheelsJeep(struct JEEPSTRUCT *Jeep, f32 DeltaTime) {
    s32 i;
    f32 Turn;
    short aSteer;
    short aRoll;
    short aBigSpin;
    f32 Spin;

    aSteer = Jeep->aChassisAngleY - Jeep->aOldChassisAngleY +
             (((short)Jeep->aFrontWheelAng - (short)Jeep->aOldFrontWheelAng) >> 2);
    Turn = (short)Jeep->aChassisAngleY - (short)Jeep->aOldChassisAngleY;
    aRoll = Jeep->Move.Resolved.z * DeltaTime * 65536.0f * 2.0f / 10.0f;
    Spin = Jeep->Accelerator * 2.0f * 2.0f * 65536.0f * DeltaTime /
           (Jeep->Traction + 1.0f);
    if (Spin < aRoll) {
        Spin = aRoll;
    }
    Jeep->aWRot[0] -= aRoll + aSteer;
    Jeep->aWRot[1] -= aRoll - aSteer;
    Jeep->aWRot[2] = Jeep->aWRot[2] - (Spin + Turn);
    Jeep->aWRot[3] = Jeep->aWRot[3] - (Spin - Turn);
    aBigSpin = DeltaTime * 393216.0f;
    for (i = 0; i < 4; i++) {
        if (Jeep->Move.BigSpin[i] != 0) {
            Jeep->aWRot[i] += aBigSpin;
        }
    }
}


void GenerateJeepTargets(struct JEEPSTRUCT *Jeep, f32 DeltaTime) {
    f32 AngMomScale = 4369.067f;
    f32 ModifiedSin;
    f32 Temp;
    f32 MomScale;
    short DeltaAng;
    s32 TempI;

    DeltaAng = Jeep->aInputAng - Jeep->aChassisAngleY;
    MomScale = (Jeep->MySpeed * 4.0f - Jeep->Move.Resolved.z) /
               (Jeep->MySpeed * 8.0f);
    Temp = MomScale * DeltaAng;
    TempI = Temp;
    DeltaAng = TempI;
    ModifiedSin = NuTrigTable[(u16)DeltaAng];
    if (DeltaAng > 0x4000) {
        ModifiedSin = 1.0f;
    } else if (DeltaAng < -0x4000) {
        ModifiedSin = -1.0f;
    }
    Jeep->aChassisAngMom = Jeep->aChassisAngMom + (s32)(ModifiedSin * AngMomScale);
    Jeep->TurnSin = ModifiedSin;
    Temp = Jeep->aChassisAngMom;
    TempI = Temp * DeltaTime + (short)Jeep->aChassisAngleY;
    Jeep->aChassisTargetAngleY = TempI;
    if (abs(Jeep->aChassisAngMom) < 0x1C73 && abs(DeltaAng) < 0x38F) {
        SeekHalfLife(&Temp, 0.0f, 0.05f, DeltaTime);
    } else {
        SeekHalfLife(&Temp, 0.0f, 0.25f, DeltaTime);
    }
    Jeep->aChassisAngMom = Temp;
}


inline void TiltSeek(struct JEEPSTRUCT *Jeep, f32 DeltaTime) {
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

void TiltJeep(struct JEEPSTRUCT *Jeep, f32 DeltaTime) {
    f32 Tilt;
    short aTilt;

    Tilt = (f32)fabs(Jeep->CentrefugalForce) * 2.0f;
    if (Tilt >= 1.0f) {
        Tilt = 1.0f;
    }
    Tilt = 1.0f - Tilt;
    Tilt *= Jeep->AccelerationForce;
    Tilt *= 3641.0f;
    aTilt = Tilt;
    if (aTilt > 0x1555) {
        aTilt = 0x1555;
    } else if (aTilt < -0x1555) {
        aTilt = -0x1555;
    }
    Jeep->aDestTiltX = aTilt;
    if (Level == 0x16) {
        Jeep->aDestTiltZ = Jeep->CentrefugalForce * 5461.0f;
    } else {
        Jeep->aDestTiltZ = Jeep->CentrefugalForce * 5461.0f;
    }
    Jeep->TiltSeekTime = 0.1f;
    TiltSeek(Jeep, 0.02f);
}


void MovePlayerJeep(struct creature_s *Cre, struct nupad_s *Pad) {
    static s32 LastChangePad;
    static f32 MinHeight[4] = { -0.085f, -0.085f, -0.07f, -0.07f };
    struct JEEPSTRUCT *Jeep = &PlayerJeep;
    struct nuvec_s FirePos;
    struct nuvec_s FireVel;
    u16 old_hdg;
    u16 ang;

    if (JeepFrame > 0x514) {
        gggg++;
    }
    Cre->Buggy = (struct NEWBUGGY *)Jeep;
    PlayerJeep.Cre = Cre;
    if (JamesInTheHouse != 0) {
        s32 press = Pad->paddata & 0x20;
        if (press != 0 && LastChangePad == 0) {
            CurrentDebugJeep++;
            if (CurrentDebugJeep > 4) {
                CurrentDebugJeep = -1;
            }
        }
        LastChangePad = press;
    }
    ProcessTimer(&Jeep->BoostOnTimer);
    if (ProcessTimer(&Jeep->BoostTimer) != 0) {
        if (Jeep->TerrainType == 0xB && Jeep->Move.ActualPosition.z > 10.0f) {
            Jeep->BoostOnTimer = 1.0f;
            Jeep->BoostTimer = 2.0f;
        }
    }
    JeepFrame++;
    JeepInControl = 1;
    Jeep->aOldChassisAngleY = Jeep->aChassisAngleY;
    Jeep->aOldFrontWheelAng = Jeep->aFrontWheelAng;
    Jeep->MySpeed = 13.0f;
    if (ProcessTimer(&Jeep->FireTimer) != 0 && Jeep->CantMove == 0 &&
        (Pad->paddata & 0x40) != 0) {
        FirePos = Jeep->Move.ActualPosition;
        FirePos.y += 0.5f;
        NuVecMtxRotate(&FireVel, SetNuVecPntr(0.0f, 5.0f, -10.0f),
                       &Jeep->DrawMtx);
        NuVecAdd(&FireVel, &FireVel, &Jeep->Move.Velocity);
        if (AddBalloon(&FirePos, &FireVel) != 0) {
            Jeep->FireTimer = 1.200000048f;
        }
    }
    DriveJeep(Jeep, Pad);
    Cre->obj.pos = Jeep->Move.ActualPosition;
    old_hdg = Cre->obj.hdg;
    Cre->obj.thdg = Cre->obj.hdg = PlayerJeep.aChassisAngleY;
    Cre->obj.dyrot = RotDiff(old_hdg, Cre->obj.hdg);
    PlayerCreatureCollisions(&Cre->obj);
    HitItems(&Cre->obj);
    if (HitCrates(&Cre->obj, 1) != 0) {
        if (temp_crate_type == 0x10 || temp_crate_type == 9) {
            KillPlayer(&Cre->obj, 0xB);
        }
    }
    WumpaCollisions(&Cre->obj);
    {
        s32 i;
        struct nuvec_s WheelPos[4];
        struct nuvec_s TempA;

        NewGenerateJeepMatrix(&Jeep->DrawMtx, (short)Jeep->aChassisAngleY + 0x8000,
                              Jeep->aSurfRotX,
                              Jeep->aSurfRotZ, Jeep->aTiltX, Jeep->aTiltZ,
                              &Jeep->Pos);
        for (i = 0; i < 4; i++) {
            TempA.x = -BaseWheelPosition[i].x;
            TempA.y = BaseWheelPosition[i].y;
            TempA.z = -BaseWheelPosition[i].z;
            NuVecMtxTransform(&WheelPos[i], &TempA, &Jeep->DrawMtx);
        }
        {
            s32 Hit[4];
            f32 Tempf;
            struct nuvec_s Temp2 = { 0.0f, -0.375f, 0.0f };
            struct nuvec_s TestVec;
            struct nuvec_s Temp;

            for (i = 0; i < 4; i++) {
                Tempf = Jeep->WheelHeight[i] + 0.009999999776f;
                if (Tempf > 0.125f) {
                    Tempf = 0.125f;
                }
                TestVec = WheelPos[i];
                TestVec.y += 0.25f;
                Temp = Temp2;
                Temp.y = -0.25f - Tempf;
                Hit[i] = NewRayCastSetHandel(&TestVec, &Temp, 0.2366899997f,
                                             0.009999999776f, 0.0f,
                                             Jeep->Move.TerrHandle);
                Tempf = -0.25f - Temp.y;
                if (Tempf < MinHeight[i]) {
                    Jeep->WheelHeight[i] = MinHeight[i];
                } else {
                    Jeep->WheelHeight[i] = Tempf;
                }
            }
        }
        for (i = 0; i < 4; i++) {
            Jeep->TrailWheel[i].OldPosition = Jeep->TrailWheel[i].Position;
            Jeep->TrailWheel[i].Radius = 0.400000006f;
            Jeep->TrailWheel[i].Position = WheelPos[i];
            if (i < 2) {
                Jeep->TrailWheel[i].TrailWidth = 0.1000000015f;
            } else {
                Jeep->TrailWheel[i].TrailWidth = 0.09000000358f;
            }
            Jeep->TrailWheel[i].Platform = -1;
            ProcessJeepTrail(&Jeep->TrailWheel[i], i);
        }
    }
    if (Level == 0x16) {
        if (Jeep->FireBossDir != 0) {
            PlayerJeep.Spline.Inc = -0.004999999888f;
        } else {
            PlayerJeep.Spline.Inc = 0.004999999888f;
        }
        FindSplineTargetPoint(&Jeep->Spline, 0x303, &Jeep->Pos, 0, 1, 0);
    }
    {
        struct nuvec_s Temp;
        struct nuvec_s Movement;
        s32 i;
        s32 j;

        NuVecScale(&Movement, &Jeep->Move.Velocity, 0.009999999776f);
        for (i = 0; i < 2; i++) {
            NuVecMtxTransform(&Temp, &ExhaustPos[i], &Jeep->DrawMtx);
            for (j = 0; j < 2; j++) {
                AddGameDebrisRot(0x5B, &Temp, 1, 0, 0);
                NuVecAdd(&Temp, &Temp, &Movement);
            }
        }
    }
    {
        struct nuvec_s Temp;
        struct nuvec_s Movement;
        s32 i;
        s32 j;

        NuVecScale(&Movement, &Jeep->Move.Velocity, 0.009999999776f);
        for (i = 0; i < 4; i++) {
            Temp = *(struct nuvec_s *)&Jeep->ChassisLocators[i]._30;
            for (j = 0; j < 2; j++) {
                AddGameDebrisRot(0x5A, &Temp, 1, 0, 0);
                NuVecAdd(&Temp, &Temp, &Movement);
            }
        }
    }
    if (Level == 3) {
        if (Jeep->DownHole == 0) {
            if (Jeep->Pos.y < -0.8000000119f) {
                struct POINTANG *RestartPoint;
                struct POINTANG *BestRestartPoint;
                f32 BestDist2 = 1000000.0f;
                f32 Dist2;
                struct nuvec_s Start;
                short OldTiltX;
                short OldAngleY;

                for (RestartPoint = WesternRestartPoints;
                     RestartPoint->z < 100000.0f; RestartPoint++) {
                    Dist2 = NuVecDistSqr((struct nuvec_s *)RestartPoint,
                                         &Jeep->Pos, 0);
                    if (Dist2 <= BestDist2) {
                        BestRestartPoint = RestartPoint;
                        BestDist2 = Dist2;
                    }
                }
                Jeep->RestartPoint = *BestRestartPoint;
                Jeep->DownHole = 1;
                Jeep->CantMove = 1;
                Jeep->DownHoleTimer = 1.0f;
                Start = Jeep->Move.ActualPosition;
                OldTiltX = Jeep->aTiltX;
                OldAngleY = Jeep->aAngleY;
                Jeep->aAngleY = Jeep->RestartPoint.Ang;
                Jeep->aTiltX = 0;
                Jeep->Move.ActualPosition =
                    *(struct nuvec_s *)&Jeep->RestartPoint;
                JeepCamFollowAng(0, 1);
                Jeep->RestartCamPos = IdealCamPos;
                Jeep->RestartCamObj = IdealObjPos;
                Jeep->aTiltX = OldTiltX;
                Jeep->aAngleY = OldAngleY;
                Jeep->Move.ActualPosition = Start;
            }
        } else if (ProcessTimer(&Jeep->DownHoleTimer) != 0) {
            struct nuvec_s Start;
            s32 i;

            Start = *(struct nuvec_s *)&Jeep->RestartPoint;
            memset(&Jeep->Move, 0, 0x100);
            Jeep->Dropped = 1;
            Start.y += DropAdj;
            Jeep->DownHole = 0;
            PlayerJeep.Move.ActualPosition = Start;
            PlayerJeep.Pos = PlayerJeep.Move.ActualPosition;
            Jeep->aChassisAngMom = 0;
            PlayerJeep.Move.aActualAngle = Jeep->RestartPoint.Ang;
            PlayerJeep.aChassisTargetAngleY = Jeep->RestartPoint.Ang;
            PlayerJeep.aChassisAngleY = Jeep->RestartPoint.Ang;
            Jeep->Move.Velocity = SetNuVec(0.0f, -1.0f, 0.0f);
            Start.y += 0.5f;
            for (i = 0; i < 4; i++) {
                Jeep->Move.ActualWheelPosition[i] = Start;
                Jeep->Move.OldWheelPosition[i] = Start;
            }
        }
    }
    if (Jeep->Dropped != 0 && Jeep->Move.AnyOnGroundBits != 0) {
        Jeep->Dropped = 0;
        Jeep->CantMove = 0;
    }
    if (Jeep->DownHole != 0) {
        DrawCross((struct nuvec_s *)&Jeep->RestartPoint, 1.0f, 0xFFFFFF);
    }
}

struct nuvec_s CamVec = { 2.0f, 0.3f, -2.0f };
struct nuvec_s FIREBOSSSCALE = { 1.0f, 1.0f, 1.0f };
f32 ThrowVal[10] = { 0.22f, 0.5f };
s32 ThrowList[9] = { 0x3B, 0x1B, 0x3B, 0x1B, 0x1B, 0x3B, 0x1B, 0x1B, 0x1B };
f32 InMechZ[4] = { 18.5f, -17.0f, -17.0f, -17.0f };
f32 OutMechZ[4] = { 47.0f, 47.0f, 47.0f, 47.0f };
struct nuvec_s InMechPos[4] = {
    { 0.0f, 0.0f, 17.0f },
    { 0.0f, 0.0f, -19.0f },
    { 0.0f, 0.0f, -19.0f },
    { 0.0f, 0.0f, -19.0f },
};
struct nuvec_s OutMechPos[4] = {
    { 0.0f, 0.0f, 50.0f },
    { 0.0f, 0.0f, 50.0f },
    { 0.0f, 0.0f, 50.0f },
    { 0.0f, 0.0f, 50.0f },
};
f32 EndChase[4] = { 0.68f, 0.45f, 0.45f, 0.45f };
struct nuvec_s AshesMechInPos = { 0.0f, 0.0f, 0.0f };
struct nuvec_s AshesMechOutPos = { 0.0f, 0.0f, 0.0f };
struct nuvec_s JeepvObj = { 0.0f, 0.0f, 0.0f };
struct nuvec_s JeepvPos = { 0.0f, 0.0f, 0.0f };
struct nuvec_s IdealCamPos = { 0.0f, 0.0f, 0.0f };
struct nuvec_s IdealObjPos = { 0.0f, 0.0f, 0.0f };
struct SPLINEFOLLOW JeepFollowSpline = { 0 };
struct nuvec_s JeepCamPos2 = { 0.0f, 0.0f, 0.0f };
struct nuvec_s JeepCamPosTween = { 0.0f, 0.0f, 0.0f };
struct nuvec_s FireBossPosition = { 0.0f, 0.0f, 0.0f };
struct BUGGYDATA *BuggyData[4] = { 0 };
s32 BuggyDataMax[4] = { 0 };
struct enemyjeep_s EnemyJeep[4] = { 0 };
struct nuvec_s BaseWheelPosition[6] = { 0 };
struct JEEPSTRUCT PlayerJeep = { 0 };
struct jeeptrail_s JeepTrail[20][0x20] = { 0 };
s32 TrailPntr[20] = { 0 };
s32 TrailAir[20] = { 0 };
s32 TrailPos[20] = { 0 };
f32 NewCurrentIntensity[20] = { 0 };
s32 NewIntoTrail[20] = { 0 };
s32 TrailActive[20] = { 0 };
struct nuvec_s WallOfFirePosition = { 0.0f, 0.0f, 0.0f };
struct nuvec_s RelTemp = { 0.0f, 0.0f, 0.0f };
struct fireboss_s FireBoss = { 0 };
struct JEEPBALLOON JeepBalloon[6] = { 0 };
struct SIMWHEEL GenericTrail[1] = { 0 };

void DriveJeep(struct JEEPSTRUCT *Jeep, struct nupad_s *Pad) {
    short aRailAng = 0;
    f32 Steer;
    f32 Speed2;
    f32 LastWheelSpin;
    short DeltaAng;
    struct nuvec_s *Vel;
    struct nuvec_s Move;
    f32 DeltaTime = 0.01999999955f;

    if (Level == 0x16 && FireBossHealth <= 0) {
        Jeep->Finished = 1;
    }
    if (Jeep->Finished != 0) {
        Jeep->InputSpeed = 0.0f;
        if (Jeep->Cre != 0) {
            Jeep->aInputAng = 0x6000;
        } else {
            Jeep->aInputAng = Jeep->aChassisAngleY;
        }
    } else if (Pad != 0) {
        if (Jeep->CantMove == 0 || Level == 0x16) {
            ProcessTimer(&Jeep->FireBossTurnTimer);
            if (Level == 0x16 && Jeep->FireBossDir == 0) {
                aRailAng = FireBossRailAng[0];
            }
            if (best_cRPos != 0) {
                if (Level == 0x16 && Jeep->FireBossDir != 0) {
                    Jeep->aBaseMoveAng = best_cRPos->angle - 0x8000;
                } else {
                    Jeep->aBaseMoveAng = best_cRPos->angle;
                }
            }
            Steer = Pad->rdata.l_alg_x - 0x7F;
            if (Pad->paddata & 0x40) {
                Jeep->InputSpeed = 1.0f;
            } else {
                Jeep->InputSpeed = 0.0f;
            }
            if (Steer > 32.0f) {
                Steer -= 32.0f;
            } else if (Steer < -32.0f) {
                Steer += 32.0f;
            } else {
                Steer = 0.0f;
            }
            if (Level == 0x16 && Jeep->FireBossDir != 0 &&
                Jeep->FireBossTurnTimer > 0.0f) {
                Jeep->aInputAng = Jeep->aBaseMoveAng;
            } else if (Level == 0x16 && Jeep->CantMove != 0) {
                Jeep->aInputAng = Jeep->aBaseMoveAng;
            } else if (Steer != 0.0f) {
                Steer /= 95.0f;
                if (Steer > 1.0f) {
                    Steer = 1.0f;
                }
                if (Steer < -1.0f) {
                    Steer = -1.0f;
                }
                if (Level == 0x16 && Jeep->FireBossDir == 0) {
                    Jeep->aInputAng =
                        (f32)(aRailAng + 0x8000) - Steer * 5461.0f;
                } else {
                    Jeep->aInputAng =
                        JeepCamY * 182.0444489f + Steer * 10923.0f;
                }
            } else if (Level == 0x16 && Jeep->FireBossDir == 0) {
                Jeep->aInputAng = aRailAng - 0x8000;
            } else {
                Jeep->aInputAng = Jeep->aChassisAngleY;
            }
            {
                short aDiff = Jeep->aInputAng - Jeep->aBaseMoveAng;

                if (aDiff > 0x4000 || aDiff < -0x4000) {
                    Jeep->aInputAng = Jeep->aChassisAngleY;
                    Jeep->InputSpeed = 0.0f;
                } else if (aDiff > 0x2000) {
                    Jeep->aInputAng = Jeep->aBaseMoveAng + 0x2000;
                } else if (aDiff < -0x2000) {
                    Jeep->aInputAng = Jeep->aBaseMoveAng - 0x2000;
                }
            }
        }
    }
    if (Level == 3 && Jeep->Pos.z <= 1.0f) {
        Jeep->InputSpeed *= 0.5f;
    }
    if (Level == 0x16) {
        if (FireBossHealth > 0 && Jeep->CantMove == 0) {
            Jeep->InputSpeed = 1.0f;
        } else {
            Jeep->InputSpeed = 0.0f;
        }
    }
    if (Jeep->BoostOnTimer > 0.0f) {
        Jeep->MaxSpeed = Jeep->MySpeed * 1.5f;
    } else if (Jeep->MaxSpeed > Jeep->MySpeed) {
        SeekHalfLife(&Jeep->MaxSpeed, Jeep->MySpeed, 0.5f, 0.01999999955f);
    } else {
        Jeep->MaxSpeed = Jeep->MySpeed;
    }
    Vel = &Jeep->Move.Velocity;
    Speed2 = Vel->x * Vel->x + Vel->z * Vel->z;
    DeltaAng = Jeep->aInputAng - Jeep->aChassisAngleY;
    Jeep->aDeltaAng = DeltaAng;
    LastWheelSpin = Jeep->WheelSpin;
    if ((Jeep->WheelSpin != 0 && DeltaAng >= 0xE3A) || DeltaAng >= 0x471D) {
        Jeep->WheelSpin = 1;
    } else if (Speed2 < 4.0f && DeltaAng > 0x2000) {
        Jeep->WheelSpin = 1;
    } else {
        Jeep->WheelSpin = 0;
    }
    if (Jeep->WheelSpin != 0) {
        Jeep->aLastDeltaAng = DeltaAng;
    } else if (LastWheelSpin != 0.0f) {
        Jeep->aLastDeltaAngA = DeltaAng;
    }
    Jeep->Accelerator = Jeep->InputSpeed;
    if (Jeep->WheelSpin != 0 && Jeep->InputSpeed < 0.5f) {
        Jeep->Accelerator = 0.5f;
    }
    GenerateJeepTargets(Jeep, DeltaTime);
    Jeep->aAngleY =
        Jeep->aChassisTargetAngleY + ((short)Jeep->aFrontWheelAng >> 3);
    ProcessJeepMovement(Jeep, DeltaTime);
    Jeep->Move.aTargetAngle = Jeep->aChassisTargetAngleY;
    JeepBaseAng = Jeep->aBaseMoveAng;
    Vel = &Jeep->Move.Velocity;
    if (Jeep->Quick != 0) {
        NuVecScaleAccum(&Jeep->Move.ActualPosition, Vel, DeltaTime);
        Jeep->Move.aActSurfRotX = 0;
        Jeep->Move.aActualAngle = Jeep->aChassisTargetAngleY;
        Jeep->aChassisAngleY = Jeep->aChassisTargetAngleY;
        Jeep->aSurfRotX = 0;
        Jeep->Move.aActSurfRotZ = 0;
        Jeep->aSurfRotZ = 0;
    } else {
        NuVecScale(&Move, Vel, DeltaTime);
        CheckAgainstRocks(&Jeep->Move.ActualPosition, &Move);
        TerrainVehicleSoft(&Jeep->Move, &Move);
        Jeep->aChassisAngleY = Jeep->Move.aActualAngle;
        Jeep->aSurfRotX = Jeep->Move.aActSurfRotX;
        Jeep->aSurfRotZ = Jeep->Move.aActSurfRotZ;
        TiltJeep(Jeep, DeltaTime);
        FindSurfaceNormalAndUnembedd(&Jeep->Move, DeltaTime);
    }
    Jeep->Pos = Jeep->Move.ActualPosition;
    {
        short WheelAngTarget = Jeep->aInputAng - Jeep->aChassisAngleY;

        if (WheelAngTarget >= 0x2000) {
            WheelAngTarget = 0x2000;
        } else if (WheelAngTarget <= -0x2000) {
            WheelAngTarget = -0x2000;
        }
        SeekAngHalfLife(&Jeep->aFrontWheelAng, WheelAngTarget, 0.1f, DeltaTime);
    }
    SpinWheelsJeep(Jeep, DeltaTime);
    Jeep->aMovementAng = Jeep->aChassisAngleY;
    Jeep->aAngleY = Jeep->aChassisAngleY + Jeep->aFrontWheelAng;
    FindTerrainType(Jeep);
}


inline void LimitSpeedZbyXZ(struct nuvec_s *Vec, f32 LimitSpeed) {
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


void ProcessJeepMovement(struct JEEPSTRUCT *Jeep, f32 DeltaTime) {
    struct numtx_s Mat;
    struct numtx_s InvMat;
    struct nuvec_s Vel;
    f32 Accel;
    f32 Side;
    f32 SideRate;
    f32 Target;
    f32 Dot;
    f32 Slope;
    f32 Grip;
    struct nuvec_s *Velocity;
    f32 Dead;


    if (Jeep->Quick != 0) {
        NuMtxSetRotationY(&Mat, Jeep->aAngleY);
        NuMtxSetRotationY(&InvMat, -Jeep->aAngleY);
        Slope = 1.0f;
        Grip = Slope;
    } else {
        NuMtxSetRotationY(&Mat, Jeep->aAngleY);
        NuMtxRotateZ(&Mat, Jeep->Move.aActSurfRotZ);
        NuMtxRotateX(&Mat, Jeep->Move.aActSurfRotX);
        NuMtxSetRotationX(&InvMat, -Jeep->Move.aTarSurfRotX);
        NuMtxRotateZ(&InvMat, -Jeep->Move.aTarSurfRotZ);
        NuMtxRotateY(&InvMat, -Jeep->aAngleY);
        if (Jeep->Move.AnyOnGroundBits & 1) {
            Dot = DotProduct(&Jeep->Move.WheelAxis[1], &WorldAxis[1]);
            if (Dot > 0.8659999967f) {
                Slope = 0.0f;
            } else if (Dot < 0.7070000172f) {
                Slope = 1.0f;
            } else {
                Slope = (0.8659999967f - Dot) / 0.1589999795f;
            }
            Dot = DotProduct(&Jeep->Move.WheelAxis[2], &WorldAxis[1]);
            if (Dot > 0.7070000172f) {
                Grip = 0.0f;
            } else if (Dot < 0.5f) {
                Grip = 1.0f;
            } else {
                Grip = (0.7070000172f - Dot) / 0.2070000172f;
            }
            Grip = Grip * (2.0f - Slope) * 0.5f;
        } else {
            Slope = 1.0f;
            Grip = Slope;
        }
    }
    Velocity = &Jeep->Move.Velocity;
    if ((Jeep->Move.AnyOnGroundBits & 1) || Jeep->Quick != 0) {
        NuVecMtxRotate(&Vel, Velocity, &InvMat);
        SideRate = 0.5f;
        Side = Vel.x;
        if (Level == 0x16) {
            ApplyFriction(&Vel.x, 11.40000057f, DeltaTime);
        } else {
            ApplyFriction(&Vel.x, 9.5f, DeltaTime);
        }
        Side -= Vel.x;
        Side /= DeltaTime;
        Side /= 9.5f;
        if (Jeep->WheelSpin != 0) {
            Side = -Jeep->TurnSin;
        }
        Dead = -0.1f;
        if (Side < Dead && Jeep->CentrefugalForce > 0.1f) {
            Jeep->CentrefugalForce = 0.0f;
        } else if (Side > 0.1f && Jeep->CentrefugalForce < Dead) {
            Jeep->CentrefugalForce = 0.0f;
        } else if (Jeep->WheelSpin != 0) {
            SideRate = 0.02999999933f;
        } else if (fabs(Jeep->CentrefugalForce) > fabs(Side)) {
            SideRate = 0.1f;
        }
        SeekHalfLife(&Jeep->CentrefugalForce, Side, SideRate, DeltaTime);
        if (Jeep->Finished != 0) {
            ApplyFriction(&Vel.z, 6.0f, DeltaTime);
        } else {
            ApplyFriction(&Vel.z, 2.0f, DeltaTime);
        }
        Accel = -Vel.z;
        if (Jeep->WheelSpin != 0) {
            SeekHalfLife(&Jeep->Traction, 0.0f, 0.05000000075f, DeltaTime);
        } else {
            SeekHalfLife(&Jeep->Traction, 1.0f, 0.3000000119f, DeltaTime);
        }
        Target = Jeep->MaxSpeed * Jeep->Accelerator * Jeep->Traction * Grip;
        if (Jeep->TerrainType == 10 && Level != 0x16) {
            Target *= 0.5f;
        }
        if (Vel.z < Target) {
            SeekHalfLife(&Vel.z, Target, 0.200000003f, DeltaTime);
        }
        Accel = (Accel + Vel.z) / DeltaTime / Jeep->MySpeed;
        SeekHalfLife(&Accel, 0.0f, 3.0f, fabs(Vel.x));
        if (Jeep->WheelSpin != 0 && Accel < 1.0f) {
            Accel = (Accel + 1.0f) * 0.5f;
        }
        SeekHalfLife(&Jeep->AccelerationForce, Accel, 0.1f, DeltaTime);
        LimitSpeedZbyXZ(&Vel, Jeep->MaxSpeed);
        if ((Jeep->Move.AllOnGroundBits & 1) || Jeep->Quick != 0) {
            Jeep->GroundTractionAcc -= Vel.y;
            if (Vel.y < 0.0f) {
                Vel.y = 0.0f;
            }
        } else {
            Jeep->GroundTractionAcc = 0.0f;
        }
        SeekHalfLife(&Jeep->GroundTractionAcc, 0.0f, 0.400000006f, DeltaTime);
        Jeep->Move.Resolved = Vel;
        NuVecMtxRotate(Velocity, &Vel, &Mat);
    } else {
        Jeep->CentrefugalForce = 0.0f;
        ApplyFriction(&Velocity->x, 0.200000003f, DeltaTime);
        ApplyFriction(&Velocity->z, 0.200000003f, DeltaTime);
        NuVecMtxRotate(&Jeep->Move.Resolved, Velocity, &InvMat);
    }
    if (!(Jeep->Move.AllTouchingGroundBits & 1) && Jeep->Quick == 0) {
        if ((Jeep->Move.AnyOnGroundBits & 1) && Slope < 1.0f) {
            Slope = 1.0f;
        }
        Jeep->Move.Velocity.x -= WorldAxis[1].x * 0.5f * Slope;
        Jeep->Move.Velocity.y -= WorldAxis[1].y * 0.5f * Slope;
        Jeep->Move.Velocity.z -= WorldAxis[1].z * 0.5f * Slope;
    }
}


void SetUpJeepWheelPositions(struct JEEPSTRUCT *Jeep) {
    struct numtx_s Locators[16];
    s32 i;

    GetLocatorMtx(Jeep->ChassisDraw.Model, Locators, 180.0f);
    for (i = 0; i < 4; i++) {
        BaseWheelPosition[i] = *(struct nuvec_s *)&Locators[i]._30;
    }
    BaseWheelPosition[0] = SetNuVec(-0.349999994f, 0.2366899997f, 0.5289999843f);
    BaseWheelPosition[1] = SetNuVec(0.349999994f, 0.2366899997f, 0.5289999843f);
    BaseWheelPosition[2] = SetNuVec(-0.349999994f, 0.2366899997f, -0.3100000024f);
    BaseWheelPosition[3] = SetNuVec(0.349999994f, 0.2366899997f, -0.3100000024f);
    BaseWheelPosition[4].x = (BaseWheelPosition[0].x + BaseWheelPosition[1].x) * 0.5f;
    BaseWheelPosition[4].y = (BaseWheelPosition[0].y + BaseWheelPosition[1].y) * 0.5f;
    BaseWheelPosition[4].z = (BaseWheelPosition[0].z + BaseWheelPosition[1].z) * 0.5f;
    NuVecAdd(&BaseWheelPosition[5], &BaseWheelPosition[0], &BaseWheelPosition[1]);
    NuVecAdd(&BaseWheelPosition[5], &BaseWheelPosition[5], &BaseWheelPosition[2]);
    NuVecAdd(&BaseWheelPosition[5], &BaseWheelPosition[5], &BaseWheelPosition[3]);
    NuVecScale(&BaseWheelPosition[5], &BaseWheelPosition[5], 0.25f);
}


void ResetJeep(struct creature_s *Cre) {
    struct JEEPSTRUCT *Jeep = &PlayerJeep;
    struct nuvec_s Pos;
    u16 Ang;
    struct jointctrl_s *joint;
    s32 i;

    memset(Jeep, 0, sizeof(struct JEEPSTRUCT));
    Jeep->Active = 1;
    Cre->Buggy = (struct NEWBUGGY *)Jeep;
    PlayerJeep.Cre = Cre;
    JeepCamTween = 0.0f;
    JeepRailCamTween = 0.0f;
    JeepRailCamTween2 = 0.0f;
    NewInitTrail();
    switch (Level) {
    case 3:
        Jeep->CantMove = 1;
        Pos = WesternPlayerStart;
        Ang = 0;
        break;
    case 0x16:
        ResetJeepCamera = 1;
        Jeep->CantMove = 1;
        Jeep->FireBossDir = 0;
        Jeep->Spline.Spline = SplTab[27].spl;
        if (Jeep->Spline.Spline != 0) {
            Jeep->Spline.Cur = 0.1f;
            Jeep->Spline.Nex = 0.1f;
            Jeep->Spline.Act = 0.1f;
            Jeep->Spline.Inc = 0.004999999888f;
            PointAlongSpline(Jeep->Spline.Spline, 0.1f, &Pos, &Ang, 0);
            Jeep->Spline.CurPos = Pos;
            Jeep->Spline.NexPos = Pos;
            Jeep->Spline.LookaheadDist = 8.0f;
        }
        break;
    default:
        Pos = Cre->obj.pos;
        Ang = Cre->obj.hdg;
        break;
    }
    PlayerJeep.Move.ActualPosition = Pos;
    PlayerJeep.Pos = PlayerJeep.Move.ActualPosition;
    PlayerJeep.Move.aActualAngle = Ang;
    PlayerJeep.aChassisTargetAngleY = Ang;
    PlayerJeep.aChassisAngleY = Ang;
    JeepFrame = 0;
    MyInitModelNew(&PlayerJeep.ChassisDraw, 0x63, 0x1D, 8, PlayerJeep.Joints,
                   &PlayerJeep.Pos);
    joint = PlayerJeep.Joints;
    for (i = 0; i < PlayerJeep.ChassisDraw.NumJoints; i++) {
        if (i < 4) {
            joint->mode = 3;
        } else {
            joint->mode = 1;
        }
        joint->joint = i;
        joint++;
    }
    SetUpJeepWheelPositions(&PlayerJeep);
    {
        struct nuvec_s Start;
        s32 j;

        Start = PlayerJeep.Pos;
        Start.y += 0.5f;
        for (j = 0; j < 4; j++) {
            PlayerJeep.Move.ActualWheelPosition[j] = Start;
        }
    }
}


struct numtx_s *DrawJeep(struct JEEPSTRUCT *Jeep) {
    u16 aSteer;
    s32 i;

    mTEMP = Jeep->DrawMtx;
    aSteer = (short)Jeep->aFrontWheelAng * 9 / 8;
    Jeep->Joints[0].rx = (u16)Jeep->aWRot[0] * 9.587380191e-05f;
    Jeep->Joints[0].ry = aSteer * 9.587380191e-05f;
    Jeep->Joints[1].ry = (u16)-aSteer * 9.587380191e-05f;
    Jeep->Joints[1].rx = (u16)Jeep->aWRot[1] * 9.587380191e-05f;
    Jeep->Joints[2].rx = (u16)Jeep->aWRot[2] * 9.587380191e-05f;
    Jeep->Joints[3].rx = (u16)Jeep->aWRot[3] * 9.587380191e-05f;
    for (i = 0; i < 4; i++) {
        Jeep->Joints[i + 4].rz =
            (short)(ASin360f(Jeep->WheelHeight[i] / 0.349999994f) * 182.0444489f) *
            9.587380191e-05f;
    }
    if (MyDrawModelNew(&Jeep->ChassisDraw, &mTEMP, Jeep->ChassisLocators) != 0) {
        return &mTEMP;
    }
    return 0;
}


void DrawJeepDebugStuff(void) {
    if (JamesInTheHouse != 0) {
        NuFntSet(app_fnt);
        NuFntSetPen(0x7F7F7F7F);
        NuFntScale(16, 16);
        NuFntPrintEx(0x80, SHEIGHT, 0, "Debug Id %d", CurrentDebugJeep);
        NuFntPrintEx(0x80, SHEIGHT * 3 / 2, 0, "Ang %d",
                     (short)PlayerJeep.Move.aActualAngle);
        NuFntPrintEx(0x80, SHEIGHT * 2, 0, "Chas Ang %d",
                     (short)PlayerJeep.aChassisAngleY);
        NuFntPrintEx(0x80, SHEIGHT * 5 / 2, 0, "Ang Mom %d",
                     PlayerJeep.aChassisAngMom);
    } else {
        NuFntSet(app_fnt);
        NuFntSetPen(0x7F7F7F7F);
        NuFntScale(16, 16);
        NuFntPrintEx(0x80, SHEIGHT, 0, "JeepFrame %d", JeepFrame);
    }
}


inline void LimitCam(struct SPLINEFOLLOW *Spline, struct nuvec_s *Pos, f32 MaxDist,
              f32 Height) {
    struct nuvec_s Closest;
    struct nuvec_s Dir;
    f32 Dist;
    f32 MinY;

    Dist = FindSplineClosestPointAndDist(Spline, 0x303, Pos, &Closest, 0, 0);
    MinY = Closest.y + Height;
    if (Pos->y < MinY) {
        Pos->y = MinY;
    }
    if (MaxDist < Dist) {
        Closest.y = Pos->y;
        NuVecSub(&Dir, Pos, &Closest);
        NuVecScaleAccum(&Closest, &Dir, MaxDist / NuVecMag(&Dir));
        *Pos = Closest;
    }
}


inline void BlendNUVECs(struct nuvec_s *Dest, struct nuvec_s *A, struct nuvec_s *B,
                 f32 Blend) {
    NuVecScale(Dest, A, 1.0f - Blend);
    NuVecScaleAccum(Dest, B, Blend);
}


inline void DoCamMtx(struct cammtx_s *Cam, struct nuvec_s *Obj,
                     struct nuvec_s *Pos) {
    struct nuvec_s Dir;
    f32 Dist;

    NuVecSub(&Dir, Obj, Pos);
    Dist = NuFsqrt(Dir.x * Dir.x + Dir.z * Dir.z);
    JeepCamY = NuAtani(Dir.x * 256.0f, Dir.z * 256.0f) / 182.0444489f;
    JeepCamX = NuAtani(-Dir.y * 256.0f, Dist * 256.0f) / 182.0444489f;
    NuMtxSetRotationX(&Cam->m, JeepCamX * 182.0444489f);
    NuMtxRotateY(&Cam->m, JeepCamY * 182.0444489f);
    NuMtxTranslate(&Cam->m, Pos);
    Cam->pos = *Pos;
    Cam->xrot = JeepCamX * 182.0444489f;
    Cam->yrot = JeepCamY * 182.0444489f;
}


void JeepCamFollowAng(struct cammtx_s *Cam, s32 Snap) {
    static short JCamAngY;
    static short JCamAngX;
    static f32 JCamMag;
    static struct nuvec_s Vel;
    static f32 AngVelY;
    static s32 VelSet;
    static struct nuvec_s TweenTargPos;
    static struct nuvec_s BackCamPos;
    static f32 BackJeepCamX;
    static f32 BackJeepCamY;
    struct JEEPSTRUCT *Jeep = &PlayerJeep;
    struct nuvec_s OldPos;
    struct nuvec_s Dir;
    struct nuvec_s Pos;
    struct nuvec_s Obj;
    f32 Blend;
    f32 InvBlend;
    f32 Dist;
    f32 OldYaw;
    short aPitch;
    short aYaw;

    if ((Jeep->Dropped != 0 || Jeep->DownHole != 0) && Snap == 0) {
        struct nuvec_s Ahead;

        Blend = 1.0f - Jeep->DownHoleTimer * Jeep->DownHoleTimer;
        if (VelSet != 0) {
            VelSet = 0;
            Vel.y = 0.0f;
            BackCamPos = *(struct nuvec_s *)&Cam->m._30;
            BackJeepCamX = JeepCamX;
            BackJeepCamY = JeepCamY;
        }
        InvBlend = 1.0f - Blend;
        NuVecScaleAccum(&BackCamPos, &Vel, InvBlend);
        NuMtxSetRotationX(&mTEMP, BackJeepCamX * 182.0444489f);
        NuMtxRotateY(&mTEMP, BackJeepCamY * 182.0444489f);
        NuMtxTranslate(&mTEMP, &BackCamPos);
        NuVecMtxTransform(&Ahead, SetNuVecPntr(0.0f, 0.0f, 5.0f), &mTEMP);
        BlendNUVECs(&Pos, &BackCamPos, &Jeep->RestartCamPos, Blend);
        BlendNUVECs(&Obj, &Ahead, &Jeep->RestartCamObj, Blend);
        DoCamMtx(Cam, &Obj, &Pos);
    } else if (Jeep->Finished != 0 && Snap == 0) {
        SeekAngHalfLife360f(&AngVelY, 0.0f, 0.5f, 0.01999999955f);
        SeekHalfLifeNUVEC(&Vel, &v000, 0.5f, 0.01999999955f);
        Pos.x = Cam->m._30;
        Pos.y = Cam->m._31;
        Pos.z = Cam->m._32;
        NuVecAdd(&Pos, &Pos, &Vel);
        if (JeepFollowSpline.Spline != 0) {
            LimitCam(&JeepFollowSpline, &Pos, 5.0f, -1.25f);
        }
        JeepCamY = Rationalise360f(JeepCamY + AngVelY);
        NuMtxSetRotationX(&Cam->m, JeepCamX * 182.0444489f);
        NuMtxRotateY(&Cam->m, JeepCamY * 182.0444489f);
        NuMtxTranslate(&Cam->m, &Pos);
        Cam->xrot = JeepCamX * 182.0444489f;
        Cam->yrot = JeepCamY * 182.0444489f;
        Cam->pos = Pos;
    } else {
        if (JeepCamSnapShot != 0 && Snap == 0) {
            JeepCamSnapShot = 0;
            JeepCamPosTween = *(struct nuvec_s *)&Cam->m._30;
            NuVecMtxTransform(&TweenTargPos, SetNuVecPntr(0.0f, 0.0f, 20.0f),
                              &Cam->m);
        }
        Obj = Jeep->Move.ActualPosition;
        Obj.y += 1.0f;
        aPitch = 3641.0f - Jeep->aTiltX * 0.5f;
        if (ResetJeepCamera != 0 || Snap != 0) {
            if (Snap == 0) {
                ResetJeepCamera = 0;
            }
            if (Level == 0x16) {
                JCamAngX = aPitch;
                JCamAngY = Jeep->aAngleY;
                JCamMag = 5.0f;
            } else {
                JCamAngX = aPitch;
                JCamAngY = Jeep->aAngleY;
                JCamMag = 5.0f;
            }
        } else {
            Pos.x = Cam->m._30;
            Pos.y = Cam->m._31;
            Pos.z = Cam->m._32;
            OldPos = Pos;
            Dir.x = Obj.x - Pos.x;
            Dir.y = Obj.y - Pos.y;
            Dir.z = Obj.z - Pos.z;
            Dist = NuFsqrt(Dir.x * Dir.x + Dir.z * Dir.z);
            JCamAngY = NuAtani(Dir.x * 256.0f, Dir.z * 256.0f);
            JCamAngX = NuAtani(-Dir.y * 256.0f, Dist * 256.0f);
            JCamMag = NuVecMag(&Dir);
            if (Jeep->Finished != 0) {
                aYaw = 0;
            } else {
                aYaw = Jeep->aAngleY;
            }
            if (Level == 0x16) {
                SeekAngHalfLife((u16 *)&JCamAngY, aYaw, 0.5f, 0.01999999955f);
                SeekAngHalfLife((u16 *)&JCamAngX, aPitch, 0.009999999776f,
                                0.01999999955f);
                SeekHalfLife(&JCamMag, 5.0f, 0.1f, 0.01999999955f);
            } else {
                SeekAngHalfLife((u16 *)&JCamAngY, aYaw, 1.0f, 0.01999999955f);
                SeekAngHalfLife((u16 *)&JCamAngX, aPitch, 0.009999999776f,
                                0.01999999955f);
                SeekHalfLife(&JCamMag, 5.0f, 0.1f, 0.01999999955f);
            }
        }
        Pos = SetNuVec(0.0f, 0.0f, -JCamMag);
        NuVecRotateX(&Pos, &Pos, JCamAngX);
        NuVecRotateY(&Pos, &Pos, JCamAngY);
        NuVecAdd(&Pos, &Pos, &Obj);
        if (Snap == 0) {
            struct nuvec_s Delta;

            NuVecSub(&Delta, &TweenTargPos, &Obj);
            NuVecScaleAccum(&Obj, &Delta, JeepCamTween);
            NuVecSub(&Delta, &JeepCamPosTween, &Pos);
            NuVecScaleAccum(&Pos, &Delta, JeepCamTween);
        }
        if (JeepFollowSpline.Spline != 0 && Snap == 0) {
            LimitCam(&JeepFollowSpline, &Pos, 5.0f, -1.25f);
        }
        if (Snap != 0) {
            IdealCamPos = Pos;
            IdealObjPos = Obj;
            return;
        }
        OldYaw = JeepCamY;
        DoCamMtx(Cam, &Obj, &Pos);
        NuVecSub(&Vel, &Pos, &OldPos);
        AngVelY = Rationalise360f(JeepCamY - OldYaw);
        VelSet = 1;
        JeepCamY = JCamAngY / 182.0444489f;
    }
    JeepRailCamTween2 = 1.0f;
    JeepCamPos2 = Pos;
    if (best_cRPos != 0) {
        pVIS = Rail[best_cRPos->iRAIL].pCAM;
        iVIS = best_cRPos->iALONG;
    }
}


void JeepCamIntro(struct cammtx_s *Cam) {
    f32 Ratio;
    f32 Blend;
    f32 Dist;
    struct nuvec_s Dir;

    if (WesternCountdown < 1.0f) {
        Ratio = 1.0f;
    } else if (WesternCountdown > 6.0f) {
        Ratio = 0.0f;
    } else {
        Ratio = (5.0f - (WesternCountdown - 1.0f)) / 5.0f;
    }
    if (Ratio > 0.9499999881f) {
        Ratio = 0.9499999881f;
    }
    if (WesternCountdown < 1.0f) {
        Blend = 1.0f;
    } else if (WesternCountdown > 6.0f) {
        Blend = 0.0f;
    } else {
        Blend = (5.0f - (WesternCountdown - 1.0f)) / 5.0f;
    }
    PointAlongSpline(JeepIntroCamSpline, Ratio, &JeepvPos, 0, 0);
    PointAlongSpline(JeepIntroLookSpline, Ratio, &JeepvObj, 0, 0);
    if (Blend != 0.0f) {
        JeepCamFollowAng(0, 1);
        Blend = Blend * Blend;
        NuVecScale(&JeepvPos, &JeepvPos, 1.0f - Blend);
        NuVecScale(&JeepvObj, &JeepvObj, 1.0f - Blend);
        NuVecScaleAccum(&JeepvPos, &IdealCamPos, Blend);
        NuVecScaleAccum(&JeepvObj, &IdealObjPos, Blend);
    }
    NuVecSub(&Dir, &JeepvObj, &JeepvPos);
    Dist = NuFsqrt(Dir.x * Dir.x + Dir.z * Dir.z);
    JeepCamY = NuAtani(Dir.x * 256.0f, Dir.z * 256.0f) / 182.0444489f;
    JeepCamX = NuAtani(-Dir.y * 256.0f, Dist * 256.0f) / 182.0444489f;
    NuMtxSetRotationX(&Cam->m, JeepCamX * 182.0444489f);
    NuMtxRotateY(&Cam->m, JeepCamY * 182.0444489f);
    NuMtxTranslate(&Cam->m, &JeepvPos);
    Cam->pos = JeepvPos;
    Cam->xrot = JeepCamX * 182.0444489f;
    Cam->yrot = JeepCamY * 182.0444489f;
    if (WesternCountdown <= 0.1f) {
        SmokeyCam = 0x14;
    }
    if (best_cRPos != 0) {
        pVIS = Rail[best_cRPos->iRAIL].pCAM;
        iVIS = best_cRPos->iALONG;
    }
}


f32 JCDist = 12.0f;

void JeepCam(struct cammtx_s *Cam) {
    struct obj_s *obj = &PlayerJeep.Cre->obj;
    struct nuvec_s Pos;
    struct nuvec_s Obj;
    struct nuvec_s RailPoint;
    struct nuvec_s RailFar;
    struct nuvec_s RailNear;
    struct nuvec_s Target;
    struct nuvec_s RailSide;
    struct nuvec_s RailCentre;
    struct nuvec_s ToCentre;
    f32 Dist;
    s32 i;
    s32 Sum;
    u16 Angle;

    if (ResetJeepCamera != 0) {
        ResetJeepCamera = 0;
    }
    Dist = JCDist;
    if (best_cRPos != 0 && cRPosCOUNT != 0) {
        RailFar = v000;
        RailNear = v000;
        RailCentre = v000;
        for (i = 0; i < cRPosCOUNT; i++) {
            MoveRailPosition(&RailPoint, &cRPos[i], 0.0f, 1);
            NuVecAdd(&RailCentre, &RailCentre, &RailPoint);
            MoveRailPosition(&RailPoint, &cRPos[i], 6.0f, 1);
            NuVecAdd(&RailFar, &RailFar, &RailPoint);
            MoveRailPosition(&RailPoint, &cRPos[i], 5.5f, 1);
            NuVecAdd(&RailNear, &RailNear, &RailPoint);
        }
        NuVecSub(&Target, &RailFar, &RailNear);
        PlayerJeep.aBaseMoveAng = NuAtan2D(Target.x, Target.z);
        NuVecScale(&RailCentre, &RailCentre, 1.0f / cRPosCOUNT);
        NuVecSub(&RailSide, &RailNear, &RailCentre);
        NuVecSub(&ToCentre, &RailCentre, &PlayerJeep.Move.ActualPosition);
        PlayerJeep.CentRailDist =
            NuFsqrt(ToCentre.x * ToCentre.x + ToCentre.z * ToCentre.z) *
            fsign(ToCentre.x * RailSide.z - ToCentre.z * RailSide.x);
        if (ChrisInTheHouse != 0) {
            NuVecScale(&RailFar, &RailFar, 1.0f / cRPosCOUNT);
            NuVecScale(&RailNear, &RailNear, 1.0f / cRPosCOUNT);
            DrawCross(&RailFar, 0.1f, 0xFF00);
            DrawCross(&RailNear, 0.1f, 0xFF0000);
            DrawLine(&RailFar, &RailNear, 0xFFFFFF);
        }
    }
    if (best_cRPos != 0 && cRPosCOUNT != 0) {
        Obj = v000;
        for (i = 0; i < cRPosCOUNT; i++) {
            MoveRailPosition(&RailPoint, &cRPos[i], Dist, 0);
            NuVecAdd(&Obj, &Obj, &RailPoint);
        }
        NuVecScale(&Obj, &Obj, 1.0f / cRPosCOUNT);
        Cam->vertical = 0;
        Pos = v000;
        for (i = 0; i < cRPosCOUNT; i++) {
            MoveRailPosition(&RailPoint, &cRPos[i], Dist, 0);
            NuVecAdd(&Pos, &Pos, &RailPoint);
        }
        NuVecScale(&Pos, &Pos, 1.0f / cRPosCOUNT);
        Angle = cRPos[0].cam_angle;
        if (cRPosCOUNT > 1) {
            Sum = 0;
            for (i = 1; i < cRPosCOUNT; i++) {
                Sum += RotDiff(Angle, cRPos[i].cam_angle);
            }
            Angle += Sum / (cRPosCOUNT - 1);
        }
    } else {
        Pos = Cam->pos;
    }
    if (HighCam != 0) {
        Pos.y += 5.0f;
    }
    Obj.x = (obj->pos.x + Obj.x * 3.0f) * 0.25f;
    Obj.y = (obj->pos.y + (obj->bot + obj->top) * obj->SCALE * 0.5f + Obj.y) *
            0.5f;
    Obj.z = (obj->pos.z + Obj.z * 3.0f) * 0.25f;
    if (JeepRailCamTween != 0.0f && Level == 0x16) {
        NuVecSub(&RailFar, &FireBossPosition, &Obj);
        NuVecScaleAccum(&Obj, &RailFar, JeepRailCamTween);
    }
    Obj.x = obj->pos.x;
    Obj.y = obj->pos.y + (obj->bot + obj->top) * obj->SCALE * 0.5f;
    Obj.z = obj->pos.z;
    if (HighCam != 0) {
        Obj.y += 4.0f;
    }
    if (JeepRailCamTween2 != 0.0f && Level == 0x16) {
        JeepRailCamTween2 -= 0.015f;
        if (JeepRailCamTween2 < 0.0f) {
            JeepRailCamTween2 = 0.0f;
        }
        NuVecSub(&RailNear, &JeepCamPos2, &Pos);
        NuVecScaleAccum(&Pos, &RailNear, JeepRailCamTween2);
        Target = PlayerJeep.Move.ActualPosition;
        Target.y += 1.0f;
        NuVecSub(&RailNear, &Target, &Obj);
        NuVecScaleAccum(&Obj, &RailNear, JeepRailCamTween2);
    }
    JeepvObj = Obj;
    JeepvPos = Pos;
    if (best_cRPos != 0) {
        pVIS = Rail[best_cRPos->iRAIL].pCAM;
        iVIS = best_cRPos->iALONG;
    }
}


void ProcessEnemyJeep(struct enemyjeep_s *Jeep) {
    s32 index = Jeep - EnemyJeep;
    s32 i;

    if (Jeep == EnemyJeep) {
        Jeep->PlayBackTime += 0.01960000023f;
    } else {
        Jeep->PlayBackTime += 0.01999999955f;
    }
    MyAnimateModelNew(&Jeep->Draw, 0.5999999642f);
    if (Jeep->TrailOn != 0 && Jeep->LocatorValidFrame == JeepFrame - 1) {
        for (i = 0; i < 4; i++) {
            Jeep->TrailWheel[i].OldPosition = Jeep->TrailWheel[i].Position;
            Jeep->TrailWheel[i].Radius = 0.400000006f;
            Jeep->TrailWheel[i].Position =
                *(struct nuvec_s *)&Jeep->Locators[i]._30;
            if (i < 2) {
                Jeep->TrailWheel[i].TrailWidth = 0.1000000015f;
            } else {
                Jeep->TrailWheel[i].TrailWidth = 0.09000000358f;
            }
            Jeep->TrailWheel[i].Platform = -1;
            ProcessJeepTrail(&Jeep->TrailWheel[i],
                             i + 4 + index * 4);
        }
    }
}


inline void InitEnemyJeep(struct enemyjeep_s *Jeep, s32 Character) {
    s32 index = Jeep - EnemyJeep;
    s32 active = 0;
    struct jointctrl_s *joint;
    s32 i;

    memset(Jeep, 0, sizeof(struct enemyjeep_s));
    switch (index) {
    case 0:
        active = 0;
        Jeep->Scale = 1.0f;
        break;
    case 1:
        active = 1;
        Jeep->Scale = 1.0f;
        break;
    case 2:
        active = 2;
        Jeep->Scale = 1.0f;
        break;
    default:
        Jeep->Scale = 1.0f;
        break;
    }
    Jeep->Active = active;
    Jeep->Active = 1;
    MyInitModelNew(&Jeep->Draw, Character, 0x1D, 4, Jeep->Joints,
                   &Jeep->Position);
    joint = Jeep->Joints;
    for (i = 0; i < Jeep->Draw.NumJoints; i++) {
        joint->joint = i;
        joint->mode = 3;
        joint++;
    }
}


void InitEnemyJeeps(void) {
    memset(EnemyJeep, 0, 4 * sizeof(struct enemyjeep_s));
    InitEnemyJeep(&EnemyJeep[0], 0x90);
    InitEnemyJeep(&EnemyJeep[1], 0x8F);
    InitEnemyJeep(&EnemyJeep[2], 0x8E);
    InitEnemyJeep(&EnemyJeep[3], 0x91);
}


void DrawEnemyJeep(struct enemyjeep_s *Jeep) {
    s32 Indx;
    f32 Element_f;
    s32 ElementA;
    s32 ElementB;
    f32 Tween;
    f32 RelZ;

    RelZ = Jeep->Position.z - player->obj.pos.z;
    if (RelZ > 100.0f || RelZ < -10.0f) {
        Jeep->DrawOn = 0;
        Jeep->TrailOn = 0;
    } else if (RelZ > 20.0f) {
        Jeep->TrailOn = 0;
        Jeep->DrawOn = 1;
    } else {
        Jeep->DrawOn = 1;
        Jeep->TrailOn = 1;
    }
    Element_f = Jeep->PlayBackTime * 60.0f;
    Indx = Jeep - EnemyJeep;
    ElementA = Element_f;
    Tween = Element_f - ElementA;
    ElementB = ElementA + 1;
    if (ElementA < 0) {
        ElementB = 0;
        Tween = 0.0f;
        ElementA = 0;
    } else if (ElementB >= BuggyDataMax[Indx]) {
        ElementB = BuggyDataMax[Indx] - 1;
        Tween = 0.0f;
        ElementA = ElementB;
    }
    {
        struct nuquat_s Quat;
        struct nuvec_s Pos;
        struct nuvec_s Rel;
        f32 TempAf;
        s32 Temp;
        short aWheel;
        u16 aDiff;
        short aDelta;
        short aRot;
        u16 aSteer;
        struct nuvec_s PosA;
        struct nuvec_s PosB;

        PosA.x = BuggyData[Indx][ElementA].PosX * 0.0078125f;
        PosA.y = BuggyData[Indx][ElementA].PosY * 6.103515625e-05f;
        PosA.z = BuggyData[Indx][ElementA].PosZ / 80.0f + 380.0f;
        PosB.x = BuggyData[Indx][ElementB].PosX * 0.0078125f;
        PosB.y = BuggyData[Indx][ElementB].PosY * 6.103515625e-05f;
        PosB.z = BuggyData[Indx][ElementB].PosZ / 80.0f + 380.0f;
        NuVecSub(&Rel, &PosB, &PosA);
        NuVecScale(&Rel, &Rel, Tween);
        NuVecAdd(&Pos, &PosA, &Rel);
        Jeep->Position = Pos;
        if (Jeep->DrawOn != 0) {
            NuQuatSlerp(&Quat, &BuggyData[Indx][ElementA].Quat,
                        &BuggyData[Indx][ElementB].Quat, Tween);
            NuQuatToMtx(&Quat, &mTEMP);
            {
                struct nuvec_s Scale = { 0.75f, 0.75f, 0.75f };

                NuMtxScale(&mTEMP, &Scale);
            }
            NuMtxTranslate(&mTEMP, &Pos);
            aDiff = BuggyData[Indx][ElementB].aWheelAng;
            Temp = BuggyData[Indx][ElementA].aWheelAng;
            aDiff -= Temp;
            TempAf = (short)aDiff;
            TempAf *= Tween;
            aDelta = TempAf;
            aWheel = aDelta + Temp;
            TempAf = Jeep->PlayBackTime * 196608.0f;
            aRot = TempAf;
            aSteer = aWheel * 9 / 8;
            Jeep->Joints[0].rz = (u16)-aSteer * 9.587380191e-05f;
            Jeep->Joints[1].rz = aSteer * 9.587380191e-05f;
            Jeep->Joints[2].rx = Jeep->Joints[0].rx = (u16)aRot * 9.587380191e-05f;
            Jeep->Joints[3].rx = Jeep->Joints[1].rx = (u16)-aRot * 9.587380191e-05f;
            {
                s32 i;
                f32 TempA;
                f32 TempB;

                for (i = 0; i < 4; i++) {
                    TempA = BuggyData[Indx][ElementA].WheelHeight[i] * 0.00390625f;
                    TempB = BuggyData[Indx][ElementB].WheelHeight[i] * 0.00390625f;
                    Jeep->Joints[i].tx = TempA + Tween * (TempB - TempA);
                }
            }
            MyDrawModelNew(&Jeep->Draw, &mTEMP, Jeep->Locators);
            Jeep->LocatorValidFrame = JeepFrame;
        }
    }
}


s32 buggy_contact = 0;

void PlayerHitBuggies(struct creature_s *Cre) {
    struct nuvec_s Pos;
    s32 i;

    for (i = 0; i < 4; i++) {
        if (EnemyJeep[i].Active != 0 &&
            NuVecDistSqr(&Cre->obj.pos, &EnemyJeep[i].Position, 0) < 2.25f) {
            u16 Ang = NuAtan2D(Cre->obj.pos.x - EnemyJeep[i].Position.x,
                               Cre->obj.pos.z - EnemyJeep[i].Position.z);

            Pos.x = EnemyJeep[i].Position.x + NuTrigTable[Ang] * 1.5f;
            Pos.z = EnemyJeep[i].Position.z +
                    NuTrigTable[(u16)(Ang + 0x4000)] * 1.5f;
            Cre->obj.pos.x += (Pos.x - Cre->obj.pos.x) * 0.3330000043f * 1.200000048f;
            Cre->obj.pos.z += (Pos.z - Cre->obj.pos.z) * 0.3330000043f * 1.200000048f;
            Cre->obj.mom.x += (NuTrigTable[Ang] * 0.05999999866f - Cre->obj.mom.x) *
                              0.1110000014f * 1.200000048f;
            Cre->obj.mom.z += (NuTrigTable[(u16)(Ang + 0x4000)] * 0.05999999866f -
                               Cre->obj.mom.z) * 0.1110000014f * 1.200000048f;
            Pos.x = (EnemyJeep[i].Position.x + Cre->obj.pos.x) * 0.5f;
            Pos.y = (EnemyJeep[i].Position.y +
                     (player->obj.top - player->obj.bot) * player->obj.SCALE * 0.5f) *
                    0.5f;
            Pos.z = (EnemyJeep[i].Position.z + Cre->obj.pos.z) * 0.5f;
            AddGameDebrisRot(0x88, &Pos, 1, 0, 0);
            NewBuzz(&player->rumble, 2);
            GameSfx(qrand() < 0x8000 ? 0x39 : 0x42, &Cre->obj.pos);
        }
    }
}


inline u8 *LoadBuggyData(u8 *Dest, s32 Index) {
    char Name[256];
    char Ext[5] = ".Ai0";
    s32 Size;
    s32 fh;

    Ext[3] += Index;
    BuggyDataMax[Index] = 0;
    NuStrCpy(Name, LevelFileName);
    NuStrCat(Name, Ext);
    Size = NuFileSize(Name);
    if (Size > 0) {
        fh = NuFileOpen(Name, 0);
        if (fh != 0) {
            NuFileRead(fh, Dest, Size);
            Dest += Size;
            NuFileClose(fh);
            BuggyDataMax[Index] = (u32)Size / 0x1C;
        }
    }
    return Dest;
}


void LoadWesternArenaData(void) {
    s32 i;

    LoadScreen("loading Ai Paths");
    TerrainSetCur(superbuffer_ptr.voidptr);
    terraininit(Level, &superbuffer_ptr.s16, superbuffer_end.s16, 0, LevelFileName, world_scene[0], 0);
    superbuffer_ptr.intaddr = (superbuffer_ptr.intaddr + 0xF) & ~(nuintptr)0xF;
    for (i = 0; i < 4; i++) {
        BuggyData[i] = superbuffer_ptr.voidptr;
        superbuffer_ptr.u8 = LoadBuggyData(superbuffer_ptr.u8, i);
    }
    superbuffer_ptr.intaddr = (superbuffer_ptr.intaddr + 0xF) & ~(nuintptr)0xF;
}


s32 CurrentWesternPosition(void) {
    static s32 FinalPosition;
    s32 Position = 1;
    s32 OldCountDown = SmokeyCountDownValue;
    s32 i;

    SmokeyCountDownValue = 0;
    if (ProcessTimer(&WesternCountdown) == 0) {
        s32 Count = (s32)WesternCountdown + 1;

        if (Count < 1) {
            Count = 1;
        }
        if (Count > 5) {
            Count = 5;
        }
        SmokeyCountDownValue = Count;
    } else if (WesternTime <= 1.0f) {
        PlayerJeep.CantMove = 0;
        SmokeyCountDownValue = -1;
    }
    if (SmokeyCountDownValue != OldCountDown && SmokeyCountDownValue != 0) {
        GameSfx(SmokeyCountDownValue == -1 ? 0x4C : 0x4F, 0);
    }
    if (player->obj.pos.z >= 751.5f) {
        PlayerJeep.Finished = 1;
        SmokeyFinished = 1;
    }
    for (i = 0; i < 4; i++) {
        if (EnemyJeep[i].Active != 0 &&
            EnemyJeep[i].Position.z > player->obj.pos.z) {
            Position++;
        }
    }
    if (PlayerJeep.Finished != 0) {
        return FinalPosition;
    }
    FinalPosition = Position;
    return Position;
}


void NewSetTrailPos(struct jeeptrail_s *Trail, s32 TrailId, s32 Pntr,
                    struct SIMWHEEL *Wheel, struct nuvec_s *Pos, s32 Air,
                    s32 Flags, f32 Scale, s32 SurfaceType) {
    s32 i;
    f32 fade;
    f32 target;
    f32 cur;

    fade = 0.0f;
    for (i = 0; i < 8; i++) {
        s32 idx = (Pntr + 1 + i) & 0x1F;

        Trail[idx].intensity = Trail[idx].RealIntensity * fade;
        fade += 0.125f;
    }
    Trail[Pntr].pos1 = Pos[0];
    Trail[Pntr].pos2 = Pos[1];
    if (Air != 0) {
        NewIntoTrail[TrailId] = 0;
    } else {
        NewIntoTrail[TrailId]++;
        if (NewIntoTrail[TrailId] > 8) {
            NewIntoTrail[TrailId] = 8;
        }
    }
    if (Flags & 0x8000) {
        target = NewIntoTrail[TrailId] * -31.9999f / 8.0f;
    } else {
        target = NewIntoTrail[TrailId] * 31.9999f / 8.0f;
    }
    if (Air != 0) {
        NewCurrentIntensity[TrailId] = target;
    } else {
        cur = NewCurrentIntensity[TrailId];
        if (cur < target) {
            NewCurrentIntensity[TrailId] = cur + 12.8f;
            if (target < NewCurrentIntensity[TrailId]) {
                NewCurrentIntensity[TrailId] = target;
            }
        } else if (target < cur) {
            NewCurrentIntensity[TrailId] = cur - 12.8f;
            if (NewCurrentIntensity[TrailId] < target) {
                NewCurrentIntensity[TrailId] = target;
            }
        }
    }
    Trail[Pntr].intensity = (s32)NewCurrentIntensity[TrailId] << 24;
    Trail[Pntr].RealIntensity = Trail[Pntr].intensity;
}


inline char *FireBossActionName(void) {
    switch (FireBoss.Action) {
    case 0:
        return "Intro";
    case 2:
        return "Tired";
    case 1:
        return "FrontHurl";
    case 3:
        return "Runaway";
    case 4:
        return "GetAngry";
    case 5:
        return "Dead";
    default:
        return "";
    }
}


void DrawFireBossDebugStuff(void) {
    NuFntSet(app_fnt);
    NuFntSetPen(0x7F7F7F7F);
    NuFntScale(16, 16);
    NuFntPrintEx(0x80, SHEIGHT * 3 / 2, 0, "Action %s Timer %1.3f  Speed %1.3f",
                 FireBossActionName(), FireBoss.ActionTimer, FireBoss.Speed);
    NuFntPrintEx(0x80, SHEIGHT * 2, 0, "Boss Pos %1.3f,  Inc %1.3f",
                 FireBoss.MainSpline.Act, FireBoss.MainSpline.Inc);
    NuFntPrintEx(0x80, SHEIGHT * 5 / 2, 0,
                 "RelZ  %1.3f   **** %1.3f, %1.3f, %1.3f",
                 FireBoss.Pos.z - player->obj.pos.z, RelTemp.x, RelTemp.y,
                 RelTemp.z);
    if (WallOfFireOn != 0) {
        if (WallOfFireAttatched != 0) {
            NuFntPrintEx(0x80, SHEIGHT * 3, 0, "WallOfFire Attatched");
        } else {
            NuFntPrintEx(0x80, SHEIGHT * 3, 0, "WallOfFire Free");
        }
    }
    if (FireBossHoldPlayer != 0) {
        NuFntPrintEx(0x80, SHEIGHT * 7 / 2, 0, "Hold Player");
    }
    NuFntPrintEx(0x80, SHEIGHT * 4, 0, "Frame %1.3f   %d   %1.3f",
                 FireBoss.MainDraw.Anim.anim_time, FireBoss.MainDraw.Anim.action,
                 FireBoss.AngleY);
    NuFntPrintEx(0x80, SHEIGHT * 7, 0, "Pos %1.3f  %1.3f    %1.3f",
                 FireBoss.Pos.x, FireBoss.Pos.y, FireBoss.Pos.z);
}


void DrawJeepBalloon(struct JEEPBALLOON *Balloon) {
    f32 speed;
    short pitch;

    speed = NuFsqrt(Balloon->Vel.x * Balloon->Vel.x +
                    Balloon->Vel.z * Balloon->Vel.z);
    pitch = NuAtan2D(Balloon->Vel.y * 100.0f, speed * 100.0f);
    NuMtxSetRotationX(&mTEMP, pitch);
    NuMtxRotateY(&mTEMP, Balloon->AngY);
    NuMtxTranslate(&mTEMP, &Balloon->Pos);
    Balloon->Seen = 0;
    if (ObjTab[89].obj.special != 0) {
        struct nuspecial_s *special = ObjTab[89].obj.special;

        Balloon->Seen = NuRndrGScnObj(
            ObjTab[89].obj.scene->gobjs[special->instance->objid], &mTEMP);
    }
}


inline s32 BalloonHitFireBoss(struct nuvec_s *pos) {
    f32 dx;
    f32 dz;
    f32 top;

    top = FireBoss.Pos.y + 3.4f;
    dx = FireBoss.Pos.x - pos->x;
    dz = FireBoss.Pos.z - pos->z;
    if (dx * dx + dz * dz < 1.689999819f) {
        if (FireBoss.Pos.y <= pos->y) {
            if (pos->y <= top) {
                return 1;
            }
        }
    }
    return 0;
}


void ProcessJeepBalloon(struct JEEPBALLOON *Balloon) {
    struct nuvec_s Step;
    s32 Hit;

    if (ProcessTimer(&Balloon->Life) != 0) {
        Balloon->Explode = 1;
    } else {
        NuVecScale(&Step, &Balloon->Vel, 0.01999999955f);
        Hit = NewRayCast(&Balloon->Pos, &Step, 0.1f);
        NuVecAdd(&Balloon->Pos, &Balloon->Pos, &Step);
        Balloon->Vel.y -= 0.1999999881f;
        if (Hit != 0) {
            Balloon->Explode = 1;
        }
        if (BalloonHitFireBoss(&Balloon->Pos) != 0) {
            Balloon->Explode = 1;
            FireBoss.HitPoints--;
        }
    }
    if (Balloon->Explode != 0) {
        Balloon->Active = 0;
        AddGameDebris(0x41, &Balloon->Pos);
    }
    DrawCross(&Balloon->Pos, 0.5f, 0xFFFFFF);
}


inline s32 CheckAgainstFireBoss(struct nuvec_s *Pos, struct nuvec_s *Vel, f32 Radius) {
    struct nuvec_s Dir;
    struct nuvec_s NewPos;
    f32 Radius2 = Radius * Radius;
    f32 Dist2;
    s32 Hit = 0;

    if (Level != 0x16) {
        return 0;
    }
    if (Vel != 0) {
        NuVecAdd(&NewPos, Pos, Vel);
    } else {
        NewPos = *Pos;
    }
    NuVecSub(&Dir, &NewPos, &FireBoss.Pos);
    Dir.y = 0.0f;
    Dist2 = NuVecMagSqr(&Dir);
    if (Dist2 < Radius2) {
        if (Vel != 0) {
            DotProduct(&Dir, Vel);
            NuVecScale(&Dir, &Dir, Radius / NuFsqrt(Dist2));
            NuVecAdd(&NewPos, &FireBoss.Pos, &Dir);
            NuVecSub(Vel, &NewPos, Pos);
        }
        Hit = 1;
    }
    return Hit;
}


s32 SMASHRUMPOWER = 200;
s32 FBSCREAMVOL = 0x1FFFF;
f32 FIREWATERSOUNDTIME = 1.0f;
s32 WATERFIRESOUNDVOL = 0xFFF;
f32 WaterLength = 4.0f;
f32 WaterWidth = 1.5f;
short SEEKANGSPEED = 0x100;
f32 MAXWATERTIME = 1.0f;
s32 FireBossHoldPlayer = 0;
s32 SmokeyCam = 0;
s32 FireBossCam = 0;
s32 SmokeyFinished = 0;
s32 SmokeyPosition = 0;
s32 SmokeyCountDownValue = 0;
s32 FireBossFinished = 0;
s32 FireBossWon = 0;
f32 AshesMechInZ = 0.0f;
f32 AshesMechOutZ = 0.0f;
s32 JeepFrame = 0;
s32 JeepInControl = 0;
f32 WesternCountdown = 0.0f;
s32 JeepCamSnapShot = 0;
short JeepBaseAng = 0;
f32 JeepCamTween = 0.0f;
f32 JeepRailCamTween = 0.0f;
f32 JeepRailCamTween2 = 0.0f;
s32 HoldFireBoss = 0;
f32 WaterTimer = 0.0f;
struct nugspline_s *JeepIntroLookSpline = 0;
struct nugspline_s *JeepIntroCamSpline = 0;
s32 HighCam = 0;
f32 FireBossWaterSoundTimer = 0.0f;
s32 RaceOn = 0;
s32 FireBossHealth = 0;
s32 DebugTest = 0;
s32 ResetJeepCamera = 0;
s32 gggg = 0;
s32 tttt2 = 0;
s32 WID = 0;
f32 WesternTime = 0.0f;
s32 WallOfFireOn = 0;
s32 WallOfFireAttatched = 0;
f32 WallOfFireAngleY = 0.0f;
f32 WallOfFireHurtTimer = 0.0f;

void FireBossWaterFire(s32 WaterOn) {
    static short WaterCurrentAngleY;
    struct nuvec_s Start;
    struct nuvec_s Dir;
    struct nuvec_s End;
    struct nuvec_s Step;
    f32 Width;
    f32 Ratio;
    short Rel;
    s32 i;

    if (WaterOn != 0) {
        Rel = player->obj.hdg - WaterCurrentAngleY;
        if (FireBossWaterSoundTimer == 0.0f) {
            MyGameSfx(0xCB, &player->obj.pos, WATERFIRESOUNDVOL);
            FireBossWaterSoundTimer = FIREWATERSOUNDTIME;
        }
        if ((Rel >= 0 ? Rel : -Rel) <= SEEKANGSPEED) {
            WaterCurrentAngleY = player->obj.hdg;
        } else if (Rel >= 0) {
            WaterCurrentAngleY += SEEKANGSPEED;
        } else {
            WaterCurrentAngleY -= SEEKANGSPEED;
        }
        WaterTimer += 0.01999999955f;
        if (WaterTimer > MAXWATERTIME) {
            WaterTimer = MAXWATERTIME;
        }
    } else if (ProcessTimer(&WaterTimer) != 0) {
        WaterCurrentAngleY = player->obj.hdg;
    }
    Start = *(struct nuvec_s *)&player->mtxLOCATOR[1][0]._30;
    Width = WaterWidth * WaterTimer / MAXWATERTIME;
    NuVecRotateY(&Dir,
                 SetNuVecPntr(0.0f, 0.0f, WaterLength * WaterTimer / MAXWATERTIME),
                 WaterCurrentAngleY);
    if (FireBoss.BeenHitThisPass == 0 && FireBoss.Action != 4 &&
        WaterTimer != 0.0f) {
        for (i = 0; i < 3; i++) {
            Ratio = i / 3.0f;
            NuVecScale(&Step, &Dir, Ratio);
            NuVecAdd(&End, &Start, &Step);
            if (CheckAgainstFireBoss(&End, 0, Width * Ratio + 2.0f) != 0) {
                FireBoss.HitPoints--;
                FireBoss.BeenHitThisPass = 1;
                MyGameSfx(0xD4, &FireBoss.Pos, FBSCREAMVOL);
                if (FireBoss.HitPoints <= 0) {
                    AshesMechOutZ = 200.0f;
                }
                i = 3;
            }
        }
    }
}


struct nuvec_s GenerateJeepWheelPoint(s32 WheelId) {
    return BaseWheelPosition[WheelId];
}

void DrawJeepTrails(void) {
    s32 i;

    if (Level == 3 || Level == 0x16 || Level == 9 || Level == 5) {
        for (i = 0; i < 20; i++) {
            if (TrailActive[i] != 0) {
                NuRndrTrail(TrailPntr[i], JeepTrail[i], 0x20);
            }
        }
    }
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

void DrawWesternArenaLevelExtra(void) {
    s32 i;

    for (i = 0; i < 4; i++) {
        if (EnemyJeep[i].Active != 0) {
            DrawEnemyJeep(&EnemyJeep[i]);
        }
    }
}


void ProcessWesternArenaLevel(void) {
    s32 i;

    if (WesternCountdown == 0.0f) {
        if (PlayerJeepFinished[0] == 0) {
            WesternTime += 0.02f;
        }
        for (i = 0; i < 4; i++) {
            if (EnemyJeep[i].Active != 0) {
                ProcessEnemyJeep(&EnemyJeep[i]);
            }
        }
    } else {
        for (i = 0; i < 4; i++) {
            if (EnemyJeep[i].Active != 0) {
                struct enemyjeep_s *jeep = &EnemyJeep[i];

                MyAnimateModelNew(&jeep->Draw, 0.5999999642f);
            }
        }
    }
    ProcessJeepTrails();
    SmokeyPosition = CurrentWesternPosition();
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


void FindTerrainType(struct JEEPSTRUCT *Jeep) {
    struct nuvec_s Pos;
    f32 FloorY;

    Pos = Jeep->Move.ActualPosition;
    Pos.y += 1.0f;
    FloorY = Jeep->Move.ActualPosition.y - NewShadowMaskPlat(&Pos, 0.0f, -1);
    Jeep->FloorHeight = FloorY;
    if (FloorY < 0.1f) {
        Jeep->TerrainType = ShadowInfo();
    } else {
        Jeep->TerrainType = -1;
    }
}

void SteeringUpdate(f32 *val, f32 target) {
    f32 rate = 0.025f;
    f32 step;

    step = fsign(target - *val) * rate;
    if (target < *val) {
        *val += step;
        if (*val < target) {
            *val = target;
        }
    } else if (*val < target) {
        *val += step;
        if (target < *val) {
            *val = target;
        }
    }
}

void PackBuggyData(struct BUGGYDATA *Data, struct JEEPSTRUCT *Jeep) {
    s32 Value;
    s32 i;

    Value = (short)Jeep->aFrontWheelAng * 9 / 8;
    NuMtxToQuat(&mTEMP, &Data->Quat);
    Data->PosX = Jeep->Pos.x * 128.0f;
    Data->PosY = Jeep->Pos.y * 16384.0f;
    Data->PosZ = (Jeep->Pos.z - 380.0f) * 80.0f;
    Data->aWheelAng = Value;
    for (i = 0; i < 4; i++) {
        Value = -Jeep->WheelHeight[i] * 256.0f;
        if (Value > 127) {
            Value = 127;
        }
        if (Value <= -128) {
            Value = -127;
        }
        Data->WheelHeight[i] = Value;
    }
}


void WesternRaceManager(void) {
}

inline void EmptyTrail(s32 i) {
    s32 j;

    for (j = 0; j < 0x20; j++) {
        JeepTrail[i][j].pos1.x = -10000.0f;
    }
    TrailPntr[i] = 0;
    TrailAir[i] = 0;
}

void NewInitTrail(void) {
    s32 i;

    for (i = 0; i < 0x14; i++) {
        EmptyTrail(i);
    }
}


inline u16 NewFindTrailAng(struct nuvec_s *A, struct nuvec_s *B) {
    struct nuvec_s Line;

    NuVecSub(&Line, B, A);
    return (u16)(NuAtan2D(Line.x, Line.z) - 0x2000);
}


inline void NewFadeOutLastTrail(struct jeeptrail_s *trail, s32 start, s32 count) {
    s32 j;
    f32 step;
    f32 fade;

    fade = 0.0f;
    step = 1.0f / (f32)(count + 1);
    fade += step;
    j = 0;
    if (count <= 0) {
        return;
    }
    do {
        s32 idx = (start - j) & 0x1F;
        if (trail[idx].pos1.x == -10000.0f) {
            break;
        }
        j++;
        trail[idx].intensity = (s32)((f32)trail[idx].RealIntensity * fade);
        fade += step;
    } while (j < count);
}


void ProcessJeepTrail(struct SIMWHEEL *Wheel, s32 TrailIndx) {
    f32 dx;
    f32 dz;
    struct jeeptrail_s *t;
    struct nuvec_s TrailPos[2];
    s32 TrailType[2];
    f32 Depth;
    s32 TrailOn;
    struct jeeptrail_s *Trail;
    f32 Scale = 1.0f;

    TrailActive[TrailIndx] = 3;
    Trail = JeepTrail[TrailIndx];
    TrailOn = 0;
    if (Wheel->Platform == -1) {
        {
            struct nuvec_s Temp = {0.0f, 1.0f, 0.0f};
            struct nuvec_s Normal;
            struct nuvec_s Line;
            s32 k;

            NuVecSub(&Line, &Wheel->Position, &Wheel->OldPosition);
            NuVecCross(&Normal, &Line, &Temp);
            NuVecNorm(&Normal, &Normal);
            TrailPos[1] = Wheel->Position;
            TrailPos[0] = TrailPos[1];
            NuVecScaleAccum(&TrailPos[0], &Normal, Wheel->TrailWidth);
            NuVecScaleAccum(&TrailPos[1], &Normal, -Wheel->TrailWidth);
            for (k = 0; k < 2; k++) {
                TrailOn = 0;
                Depth = NewShadowMask(&TrailPos[k], Wheel->Radius, -1);
                TrailPos[k].y = Depth;
                if (Wheel->Position.y - Depth < Wheel->Radius + 0.200000003f &&
                    Depth != 2000000.0f) {
                    TrailOn = 1;
                }
                TrailPos[k].y += 0.05000000075f;
                if (TrailOn == 0) {
                    break;
                }
                TrailType[k] = ShadowInfo();
            }
        }
    }
    if (TrailOn != 0) {
        if (TrailType[0] == TrailType[1]) {
            if (TrailType[0] == 0) {
                Wheel->SurfaceType = -1;
            } else {
                Wheel->SurfaceType = TrailType[0];
            }
        } else if (Wheel->SurfaceType != TrailType[0] &&
                   Wheel->SurfaceType != TrailType[1]) {
            TrailOn = 0;
            Wheel->SurfaceType = -1;
        }
    } else {
        Wheel->SurfaceType = -1;
    }
    if (TrailOn != 0 &&
        fabs(Wheel->Position.y - (TrailPos[0].y + TrailPos[1].y) * 0.5f) > 0.4f) {
        TrailOn = 0;
    }
    if (TrailOn != 0) {
        if (TrailAir[TrailIndx] != 0) {
            s32 Ang;

            TrailAir[TrailIndx] = 0;
            TrailPntr[TrailIndx] = (TrailPntr[TrailIndx] + 1) & 0x1F;
            Ang = NewFindTrailAng(&TrailPos[0], &TrailPos[1]);
            NewSetTrailPos(Trail, TrailIndx, TrailPntr[TrailIndx], Wheel, TrailPos,
                           1, Ang, Scale, Wheel->SurfaceType);
            TrailPntr[TrailIndx] = (TrailPntr[TrailIndx] + 1) & 0x1F;
            NewSetTrailPos(Trail, TrailIndx, TrailPntr[TrailIndx], Wheel, TrailPos,
                           1, Ang, Scale, Wheel->SurfaceType);
        } else {
            s32 Ang;

            t = &Trail[(TrailPntr[TrailIndx] - 1) & 0x1F];
            dx = (TrailPos[0].x + TrailPos[1].x) * 0.5f -
                 (t->pos1.x + (t->pos2.x - t->pos1.x) * 0.5f);
            dz = (TrailPos[0].z + TrailPos[1].z) * 0.5f -
                 (t->pos1.z + (t->pos2.z - t->pos1.z) * 0.5f);
            {
                s32 OldAng;
                s32 NewAng;
                s32 DeltaAng;

                NewAng = NewFindTrailAng(&TrailPos[0], &TrailPos[1]);
                OldAng = NewFindTrailAng(&t->pos1, &t->pos2);
                DeltaAng = (u16)(NewAng - OldAng + 0x8000) - 0x8000;
                if (dx * dx + dz * dz > 0.04000000283f || abs(DeltaAng) > 0x1555) {
                    TrailPntr[TrailIndx] = (TrailPntr[TrailIndx] + 1) & 0x1F;
                }
                Ang = NewAng;
            }
            NewSetTrailPos(Trail, TrailIndx, TrailPntr[TrailIndx], Wheel, TrailPos,
                           0, Ang, Scale, Wheel->SurfaceType);
        }
    } else {
        if (TrailAir[TrailIndx] == 0) {
            NewFadeOutLastTrail(Trail, TrailPntr[TrailIndx], 1);
            TrailPntr[TrailIndx] = (TrailPntr[TrailIndx] + 1) & 0x1F;
        }
        TrailAir[TrailIndx] = 1;
        Trail[TrailPntr[TrailIndx]].pos1.x = -10000.0f;
    }
}


void ProcessJeepTrails(void) {
    s32 i;

    if (Level == 3 || Level == 0x16) {
        for (i = 0; i < 20; i++) {
            if (TrailActive[i] != 0) {
                TrailActive[i]--;
                if (TrailActive[i] == 0) {
                    EmptyTrail(i);
                }
            }
        }
    }
}


void InitFireBoss(struct fireboss_s *Boss) {
    struct nuvec_s Start;
    short StartAngle;

    memset(Boss, 0, sizeof(struct fireboss_s));
    if (MyInitModelNew(&Boss->MainDraw, 0x9C, 0x3A, 0, 0, &Boss->Pos) != 0) {
        if (MyInitModelNew(&Boss->ExplodeDraw, 0xBD, 0xE, 0, 0, &Boss->Pos) != 0) {
            Boss->Active = 1;
            Boss->Pass = 4;
            Boss->HitPoints = 3;
            FireBossHealth = 3;
            Boss->MainSpline.Spline = SplTab[27].spl;
            if (Boss->MainSpline.Spline != 0) {
                Boss->MainSpline.Cur = FIREBOSSSTART;
                Boss->MainSpline.Inc = -0.004999999888f;
                Boss->MainSpline.Nex = FIREBOSSSTART;
                Boss->MainSpline.Act = FIREBOSSSTART;
                PointAlongSpline(Boss->MainSpline.Spline, FIREBOSSSTART, &Start,
                                 &StartAngle, 0);
                Boss->MainSpline.CurPos = Start;
                Boss->MainSpline.NexPos = Start;
                Boss->MainSpline.LookaheadDist = 3.0f;
                Boss->AngleY = StartAngle / 182.0444489f + 180.0f;
                Boss->Pos = Start;
                Boss->LastAction = -1;
                Boss->Active = 1;
                Boss->Action = 0;
            }
        }
    }
}


s32 DrawFireBoss(struct fireboss_s *Boss) {
    struct nuvec_s P0;
    struct nuvec_s P1;
    struct nuvec_s Rel;
    struct nuvec_s P2;
    struct nuvec_s P3;

    NuMtxSetScale(&mTEMP, &FIREBOSSSCALE);
    NuMtxRotateY(&mTEMP, (Boss->AngleY + 180.0f) * 182.0444489f);
    NuMtxTranslate(&mTEMP, &Boss->Pos);
    Boss->DrawMtx = mTEMP;
    if (Boss->Action == 5) {
        Boss->Seen = MyDrawModelNew(&Boss->ExplodeDraw, &mTEMP, Boss->Locator);
    } else {
        Boss->Seen = MyDrawModelNew(&Boss->MainDraw, &mTEMP, Boss->Locator);
    }
    if (WallOfFireOn != 0 && FireBoss.HitPoints > 0) {
        if (Paused == 0) {
            P0 = WallOfFirePosition;
            P0.y += 0.5f;
            AddVariableShotDebrisEffect(GDeb[0x95].i, &P0, 4, 0,
                                        (short)WallOfFireAngleY);
        }
        NuVecRotateY(&P0, SetNuVecPntr(4.0f, 0.0f, 0.0f),
                     WallOfFireAngleY * 182.0444489f);
        NuVecRotateY(&P1, SetNuVecPntr(-4.0f, 0.0f, 0.0f),
                     WallOfFireAngleY * 182.0444489f);
        NuVecAdd(&P0, &WallOfFirePosition, &P0);
        NuVecAdd(&P1, &WallOfFirePosition, &P1);
        NuVecSub(&Rel, &P1, &P0);
    }
}


inline void JonnySteam(struct fireboss_s *fb) {
    struct nuvec_s pos;

    if (fb->BeenHitThisPassTimer < 3.0f) {
        pos = fb->Pos;
        pos.y += 1.5f;
        AddVariableShotDebrisEffect(GDeb[162].i, &pos, 1, 0, 0);
    }
}


inline void JonnyBossSmashEffect(struct fireboss_s *fb) {
    s32 i;

    DrawCross(&fb->Pos, 1.0f, 0xFF00);
    for (i = 0; i < 10; i++) {
        DrawCross((struct nuvec_s *)&fb->Locator[i + 2]._30, 1.0f, 0xFF);
    }
}


extern f32 vtog_time;
extern f32 vtog_duration;
extern struct ldata_s *LDATA;
extern struct cammtx_s GameCam[];
extern void SetNewMaskStuff(s32 index, struct nuvec_s *centre,
                            struct nuvec_s *offset, f32 rad, f32 anginc,
                            f32 tweeninc, s32 fixed, s32 killatend, f32 scale,
                            f32 tiltx);
extern void MyResetAnimPacket(struct MYDRAW *draw, s32 action);
extern void MyChangeAnim(struct MYDRAW *draw, s32 action);
extern void MyGameSfxLoop(s32 id, struct nuvec_s *pos, s32 vol);
extern f32 frand(void);
extern f32 frandPN(void);
struct jeeprock_s;
extern struct jeeprock_s *AddRock(struct nuvec_s *pos, f32 radius, s32 owner);
extern void KeepHoldOnRock(struct jeeprock_s *rock, struct nuvec_s *pos,
                           struct nuvec_s *vel);
extern void GetLocatorMtxMyDraw(struct MYDRAW *Draw, struct numtx_s *dest,
                                struct numtx_s *baseMtx);
extern s32 KillGameObject(struct obj_s *obj, s32 anim);
extern void AddMaskFeathers(MaskInfo *mask);
extern void JudderGameCamera(struct cammtx_s *cam, f32 time,
                             struct nuvec_s *pos);
f32 FIREBOSSTHROWSPEED = 10.0f;
f32 FIREBOSSCHASESPEED = 3.6f;
f32 FIREBOSSSCAREDSPEED = 4.65f;
f32 FIREBOSSENDRUNAWAY = 0.95f;
f32 FIREBOSSSTART = 0.95f;
f32 FIREBOSSHOLDPLAYERTIME = 5.5f;
f32 FIREBOSSHOLDINTROTIME = 3.7f;
f32 FIREBOSSENDCHASE = 0.68f;
f32 FIREBOSSTURNTOMECH = 12.0f;
f32 FIREBOSSTURNFROMMECH = 54.0f;
f32 FIREBOSSWALLFIREDIST = 0.0f;
f32 FOOTJUDDERTIME = 0.35f;
s32 FOOTSTEPVOL = 0x7FFF;
f32 TRANSLATEFIREBOSS = 0.9342f;
f32 FIREBOSSRUNAHEADTIMER = 0.5f;
s32 FOOTRUMBLEPOWER = 0x7F;
s32 FIREVOL = 0x7FFF;
s32 BREAKVOL = 0x3FFF;
s32 BOSSBANGVOL = 0x17FFF;
s32 BOSSANGRYVOL = 0x17FFF;
s32 BOSSBREATHVOL = 0x4FFF;
f32 BOSSBREATHFRAME = 20.0f;

void ProcessFireBoss(struct fireboss_s *Boss) {
    static f32 LastBossAct;
    static s32 FireIndx = 0;
    static s32 DontDropRocks;
    static s32 JustFromIntro;
    static s32 LastVehicleControl;
    struct nuvec_s Target;
    struct nuvec_s DestVel;

    ProcessTimer(&FireBossWaterSoundTimer);
    if (vtog_time >= vtog_duration) {
        if (LastVehicleControl != VEHICLECONTROL) {
            FireBossHoldPlayer = 1;
            FireBossWaterSoundTimer = 0.0f;
            if (Boss->Action == 1 || Boss->Action == 2) {
                SetNewMaskStuff(1, &player->obj.pos,
                                SetNuVecPntr(0.0f, 12.5f, 0.0f), 0.0f,
                                -380.0f, 0.0f, 0, 0, 1.0f, AKUAKUTILTX);
                SetNewMaskStuff(1, &player->obj.pos,
                                SetNuVecPntr(0.0f, 1.0f, 0.0f), 1.0f, -380.0f,
                                1.0f, 0, 0, 1.0f, AKUAKUTILTX);
            }
            if (Boss->Action == 3 || Boss->Action == 4) {
                MyGameSfx(0xD0, &player->obj.pos, BREAKVOL);
            }
        }
        LastVehicleControl = VEHICLECONTROL;
        if (VEHICLECONTROL == 0 && (Boss->Action == 3 || Boss->Action == 4)) {
            FireBossHoldPlayer = 1;
        }
    }
    if (Boss->HitPoints <= 0) {
        FireBossHoldPlayer = 0;
        Boss->Action = 5;
    }
    AshesMechInZ = InMechZ[4 - Boss->Pass];
    AshesMechInPos = InMechPos[4 - Boss->Pass];
    AshesMechOutPos = OutMechPos[4 - Boss->Pass];
    AshesMechOutZ = OutMechZ[4 - Boss->Pass];
    FIREBOSSENDCHASE = EndChase[4 - Boss->Pass];
    FIREBOSSTURNTOMECH = AshesMechInZ;
    FIREBOSSTURNFROMMECH = AshesMechOutZ;

    if (Boss->Action != Boss->LastAction) {
        switch (Boss->Action) {
        case 0:
            MyResetAnimPacket(&Boss->MainDraw, 0x22);
            SetNewMaskStuff(0, &Boss->Pos, SetNuVecPntr(0.0f, 12.5f, 0.0f),
                            0.0f, 360.0f, 0.0f, 0, 0, 2.0f, PYROTILTX);
            SetNewMaskStuff(0, &Boss->Pos, SetNuVecPntr(0.0f, 1.0f, 0.0f), 2.7f,
                            360.0f, 1.0f, 0, 0, 2.0f, PYROTILTX);
            Boss->Speed = 0.0f;
            Boss->ActionTimer = FIREBOSSHOLDINTROTIME;
            Boss->ActionTimer2 = FIREBOSSHOLDPLAYERTIME;
            FireBossHoldPlayer = 1;
            LastVehicleControl = 0;
            WallOfFireOn = 1;
            WallOfFireAttatched = 1;
            JustFromIntro = 1;
            break;
        case 1:
            MyGameSfx(0xD5, &Boss->Pos, BOSSANGRYVOL);
            SetNewMaskStuff(0, &Boss->Pos, SetNuVecPntr(0.0f, 12.5f, 0.0f), 2.7f,
                            360.0f, 1.0f, 0, 1, 2.0f, PYROTILTX);
            FireIndx = -1;
            break;
        case 2:
            Boss->BeenHitThisPass = 0;
            Boss->ActionTimer = 100.0f;
            WallOfFireAttatched = 0;
            break;
        case 3:
            SetNewMaskStuff(1, &player->obj.pos, SetNuVecPntr(0.0f, 12.5f, 0.0f),
                            0.0f, -380.0f, 1.0f, 0, 1, 1.0f, AKUAKUTILTX);
            DontDropRocks = 3;
            Boss->ActionTimer = FIREBOSSRUNAHEADTIMER;
            Boss->Speed = FIREBOSSSCAREDSPEED;
            WallOfFireOn = 1;
            WallOfFireAttatched = 1;
            break;
        case 4:
            Boss->BeenHitThisPass = 0;
            SetNewMaskStuff(0, &Boss->Pos, SetNuVecPntr(0.0f, 12.5f, 0.0f),
                            0.0f, 360.0f, 0.0f, 0, 0, 2.0f, PYROTILTX);
            SetNewMaskStuff(0, &Boss->Pos, SetNuVecPntr(0.0f, 1.0f, 0.0f), 2.7f,
                            360.0f, 1.0f, 0, 0, 2.0f, PYROTILTX);
            Boss->Pass = Boss->HitPoints + 1;
            Boss->ActionTimer = 3.0f;
            PlayerJeep.FireBossDir = 0;
            break;
        case 5:
            Boss->Speed = 0.0f;
            JonnyBossSmashEffect(Boss);
            MyGameSfx(0xCF, &Boss->Pos, BOSSBANGVOL);
            MyResetAnimPacket(&Boss->ExplodeDraw, 0xE);
            break;
        }
    }
    Boss->LastAction = Boss->Action;

    if (Boss->MainSpline.Inc > 0.0f && Boss->MainDraw.Anim.action == 0x3B) {
        if (Boss->Speed > FIREBOSSSCAREDSPEED) {
            MyAnimateModelNew(&Boss->MainDraw,
                              (Boss->Speed * 0.5999999642f) / FIREBOSSCHASESPEED);
        } else {
            MyAnimateModelNew(&Boss->MainDraw,
                              (FIREBOSSSCAREDSPEED * 0.5999999642f) /
                                  FIREBOSSCHASESPEED);
        }
    } else {
        MyAnimateModelNew(&Boss->MainDraw, 0.5999999642f);
    }
    MyAnimateModelNew(&Boss->ExplodeDraw, 0.4499999881f);

    switch (Boss->Action) {
    case 0:
        FireBossHoldPlayer = !ProcessTimer(&Boss->ActionTimer2);
        if (ProcessTimer(&Boss->ActionTimer)) {
            Boss->Action = 1;
            WallOfFireOn = 1;
            WallOfFireAttatched = 1;
            FireBossHoldPlayer = 0;
        }
        break;
    case 1: {
        s32 ThrowNow = 0;

        Boss->Speed = FIREBOSSCHASESPEED;
        if ((Boss->MainDraw.Anim.flags & 1) || FireIndx == -1) {
            s32 Act;

            FireIndx++;
            if (FireIndx > 8) {
                FireIndx = 0;
            }
            Act = ThrowList[FireIndx];
            if (VEHICLECONTROL != 0) {
                Act = 0x3A;
            }
            if (JustFromIntro == 0) {
                MyResetAnimPacket(&Boss->MainDraw, Act);
            } else {
                MyChangeAnim(&Boss->MainDraw, Act);
            }
            JustFromIntro = 0;
        }
        if (Boss->MainDraw.Anim.action == 0x1B) {
            f32 ThisTime = Boss->MainDraw.Anim.anim_time;
            f32 LastTime = ThisTime - Boss->MainDraw.Model->animlist[0x1B]->speed *
                                          0.5999999642f;

            if (ThisTime >= 13.0f && LastTime < 13.0f) {
                ThrowNow = 1;
            }
        }
        if (ThrowNow) {
            if (Boss->Pass < 2 || (Boss->Pass == 2 && frand() < 0.5f)) {
                Boss->Rock = AddRock(&v000, 0.4f, 2);
            } else {
                Boss->Rock = AddRock(&v000, 0.4f, 0);
            }
            if (Boss->Rock != 0) {
                Boss->ActionTimer2 = 1.0f;
            }
        }
        if (Boss->MainSpline.Act <= FIREBOSSENDCHASE) {
            Boss->Action = 2;
        }
        break;
    }
    case 2:
        if ((Boss->MainDraw.Anim.flags & 1) ||
            Boss->MainDraw.Anim.action == 0x3A ||
            Boss->MainDraw.Anim.action == 0x1B) {
            Boss->MainDraw.Anim.flags &= ~1;
            if (Boss->MainDraw.Anim.action == 0x46) {
                struct nuvec_s Temp;

                MyResetAnimPacket(&Boss->MainDraw, 0x3A);
                Boss->AngleY = Rationalise360f(Boss->AngleY + 180.0f);
                NuVecRotateY(&Temp, SetNuVecPntr(0.0f, 0.0f, TRANSLATEFIREBOSS),
                             Boss->AngleY * 182.0444489f);
                NuVecAdd(&Boss->Pos, &Boss->Pos, &Temp);
                Boss->Action = 3;
                Boss->MainSpline.Inc = 0.004999999888f;
            } else if (Boss->MainDraw.Anim.action == 0x3A ||
                       Boss->MainDraw.Anim.action == 0x1B) {
                if (Boss->MainDraw.Anim.newaction != 0x22) {
                    MyGameSfx(0xD6, &Boss->Pos, BOSSBREATHVOL);
                }
                MyChangeAnim(&Boss->MainDraw, 0x22);
                Boss->ActionTimer = 1.0f;
            }
        }
        if (Boss->MainDraw.Anim.action == 0x22 && FireBossHoldPlayer != 0 &&
            ProcessTimer(&Boss->ActionTimer)) {
            MyChangeAnim(&Boss->MainDraw, 0x46);
        }
        Boss->Speed = 0.0f;
        PlayerJeep.CantMove = 1;
        break;
    case 3: {
        s32 ThrowNow = 0;

        if (ProcessTimer(&Boss->ActionTimer)) {
            FireBossHoldPlayer = 0;
        }
        JeepCamTween -= 0.07999999821f;
        if (JeepCamTween < 0.0f) {
            JeepCamTween = 0.0f;
        }
        if (Boss->BeenHitThisPass != 0) {
            if (Boss->Speed > FIREBOSSSCAREDSPEED + 3.0f) {
                Boss->Speed -= 0.03999999911f;
                if (Boss->Speed < FIREBOSSSCAREDSPEED + 3.0f) {
                    Boss->Speed = FIREBOSSSCAREDSPEED + 3.0f;
                }
            } else if (Boss->Speed < FIREBOSSSCAREDSPEED + 3.0f) {
                Boss->Speed += 0.03999999911f;
                if (Boss->Speed > FIREBOSSSCAREDSPEED + 3.0f) {
                    Boss->Speed = FIREBOSSSCAREDSPEED + 3.0f;
                }
            }
        } else if (Boss->Pos.z - player->obj.pos.z < 5.0f) {
            if (Boss->Speed > FIREBOSSSCAREDSPEED + 1.0f) {
                Boss->Speed -= 0.03999999911f;
                if (Boss->Speed < FIREBOSSSCAREDSPEED + 1.0f) {
                    Boss->Speed = FIREBOSSSCAREDSPEED + 1.0f;
                }
            } else if (Boss->Speed < FIREBOSSSCAREDSPEED + 1.0f) {
                Boss->Speed += 0.03999999911f;
                if (Boss->Speed > FIREBOSSSCAREDSPEED + 1.0f) {
                    Boss->Speed = FIREBOSSSCAREDSPEED + 1.0f;
                }
            }
        } else if (Boss->Speed > FIREBOSSSCAREDSPEED) {
            Boss->Speed -= 0.03999999911f;
            if (Boss->Speed < FIREBOSSSCAREDSPEED) {
                Boss->Speed = FIREBOSSSCAREDSPEED;
            }
        } else if (Boss->Speed < FIREBOSSSCAREDSPEED) {
            Boss->Speed += 0.03999999911f;
            if (Boss->Speed > FIREBOSSSCAREDSPEED) {
                Boss->Speed = FIREBOSSSCAREDSPEED;
            }
        }
        if (Boss->MainSpline.Act >= FIREBOSSENDRUNAWAY) {
            Boss->Action = 4;
        }
        if (Boss->Pass < 4) {
            if (Boss->MainDraw.Anim.action == 0x3A) {
                f32 ThisTime = Boss->MainDraw.Anim.anim_time;
                f32 LastTime = ThisTime -
                               Boss->MainDraw.Model->animlist[0x3A]->speed *
                                   0.5999999642f;

                if (ThisTime >= 20.0f && LastTime < 20.0f) {
                    Boss->DropSide = 0;
                    ThrowNow = 1;
                }
                if (ThisTime >= 50.0f && LastTime < 50.0f) {
                    ThrowNow = 1;
                    Boss->DropSide = 1;
                }
            }
            if (ThrowNow) {
                if (DontDropRocks > 0) {
                    DontDropRocks--;
                } else {
                    DontDropRocks = 2;
                    if (Boss->Pass < 3) {
                        Boss->Rock = AddRock(&v000, 0.2f, 0);
                    } else {
                        Boss->Rock = AddRock(&v000, 0.2f, 1);
                    }
                    if (Boss->Rock != 0) {
                        Boss->ActionTimer2 = 1.0f;
                    }
                }
            }
        }
        break;
    }
    case 4:
        if ((Boss->MainDraw.Anim.flags & 1) ||
            Boss->MainDraw.Anim.action == 0x3A) {
            if (Boss->MainDraw.Anim.action == 0x47) {
                struct nuvec_s Temp;

                MyResetAnimPacket(&Boss->MainDraw, 0x3A);
                Boss->AngleY = Rationalise360f(Boss->AngleY + 180.0f);
                NuVecRotateY(&Temp, SetNuVecPntr(0.0f, 0.0f, 1.346f),
                             Boss->AngleY * 182.0444489f);
                NuVecAdd(&Boss->Pos, &Boss->Pos, &Temp);
                Boss->Action = 1;
                Boss->MainSpline.Inc = -0.004999999888f;
                FireBossHoldPlayer = 0;
            } else if (Boss->MainDraw.Anim.action == 0x3A) {
                MyChangeAnim(&Boss->MainDraw, 0x22);
                Boss->Speed = 0.0f;
            }
        }
        if (Boss->MainDraw.Anim.action == 0x22) {
            f32 ThisTime = Boss->MainDraw.Anim.anim_time;
            f32 LastTime = ThisTime - Boss->MainDraw.Model->animlist[0x22]->speed *
                                          0.5999999642f;

            if (ThisTime >= BOSSBREATHFRAME && LastTime < BOSSBREATHFRAME) {
                MyGameSfx(0xD6, &Boss->Pos, BOSSBREATHVOL);
            }
        }
        if (FireBossHoldPlayer != 0 && Boss->MainDraw.Anim.action == 0x22 &&
            ProcessTimer(&Boss->ActionTimer)) {
            MyChangeAnim(&Boss->MainDraw, 0x47);
            Boss->ActionTimer = 100.0f;
        }
        break;
    case 5:
        if (Boss->ExplodeDraw.Anim.flags & 1) {
            ChrisBigBossDead = 1;
        }
        break;
    }

    LastBossAct = FireBoss.MainSpline.Act;
    FindSplineTargetPoint(&Boss->MainSpline, 0x300, &Boss->Pos, &Target, 0, 0);
    {
        struct nuvec_s Temp;
        struct nuvec_s Target2;

        Target2 = Target;
        Target2.x = player->obj.pos.x * 0.5f;
        NuVecSub(&Temp, &Target2, &Boss->Pos);
        NuVecNorm(&DestVel, &Temp);
        NuVecScale(&DestVel, &DestVel, Boss->Speed);
        NuVecScaleAccum(&Boss->Pos, &DestVel, 0.01999999955f);
        {
            struct nuvec_s Ray = {0.0f, -6.0f, 0.0f};

            Boss->Pos.y += 3.0f;
            NewRayCast(&Boss->Pos, &Ray, 0.3f);
            NuVecAdd(&Boss->Pos, &Boss->Pos, &Ray);
            Boss->Pos.y -= 0.3f;
        }
        if (Boss->Action != 0 && Boss->Action != 5 && Boss->Speed > 1.5f &&
            NuVecMagSqr(&Temp) > 0.003599999705f) {
            f32 TempY = NuAtan2D(DestVel.x * 100.0f, DestVel.z * 100.0f) /
                        182.0444489f;

            if (fabs(Rationalise360f(TempY - Boss->AngleY)) < 45.0) {
                Boss->AngleY = TempY;
            }
        }
    }
    Boss->Vel = DestVel;

    if (WallOfFireOn != 0) {
        struct nuvec_s Temp;
        struct nuvec_s Ray;

        if (WallOfFireAttatched != 0) {
            if (Boss->Action == 3 || Boss->Action == 0) {
                WallOfFirePosition = SetNuVec(0.230876f, -0.0738214f, 53.0f);
                WallOfFireAngleY = 182.796f;
            } else {
                NuVecRotateY(&Temp, SetNuVecPntr(0.0f, 0.0f, FIREBOSSWALLFIREDIST),
                             WallOfFireAngleY * 182.0444489f);
                NuVecAdd(&Temp, &Target, &Temp);
                if (Temp.z < WallOfFirePosition.z) {
                    WallOfFirePosition = Temp;
                }
                WallOfFireAngleY = Boss->AngleY;
            }
        } else {
            NuVecRotateY(&Temp,
                         SetNuVecPntr(0.0f, 0.0f,
                                      FIREBOSSCHASESPEED * 0.01999999955f),
                         WallOfFireAngleY * 182.0444489f);
            NuVecAdd(&WallOfFirePosition, &WallOfFirePosition, &Temp);
        }
        if (Boss->Action != 3 && Boss->Action != 0 && VEHICLECONTROL == 0 &&
            WallOfFirePosition.z < player->obj.pos.z) {
            WallOfFirePosition.z = player->obj.pos.z;
        }
        Ray = (struct nuvec_s){0.0f, -6.0f, 0.0f};
        WallOfFirePosition.y += 3.0f;
        NewRayCast(&WallOfFirePosition, &Ray, 0.3f);
        NuVecAdd(&WallOfFirePosition, &WallOfFirePosition, &Ray);
        WallOfFirePosition.y -= 0.3f;
        if (VEHICLECONTROL == 0) {
            if (ProcessTimer(&WallOfFireHurtTimer) && FireBoss.HitPoints > 0) {
                struct nuvec_s Rel;

                NuVecSub(&Rel, &player->obj.pos, &WallOfFirePosition);
                NuVecRotateY(&Rel, &Rel, -WallOfFireAngleY * 182.0444489f);
                RelTemp = Rel;
                if (Rel.z < 0.5f && Rel.z > -0.5f) {
                    WallOfFireHurtTimer = 2.0f;
                    if (player->obj.dead == 0) {
                        player->obj.invincible = 0;
                        KillGameObject(&player->obj, 0xD);
                        if (player->obj.mask != 0 &&
                            player->obj.mask->active != 0 &&
                            (LDATA->flags & 0xE00) == 0) {
                            AddMaskFeathers(player->obj.mask);
                            player->obj.mask->active = 0;
                        }
                    }
                }
            }
        } else {
            WallOfFireHurtTimer = 0.0f;
        }
        MyGameSfxLoop(0xCD, &WallOfFirePosition, FIREVOL);
    } else {
        WallOfFireHurtTimer = 0.0f;
    }
    FireBossHealth = Boss->HitPoints;
    FireBossPosition = Boss->Pos;

    if (Boss->Action != 5) {
        f32 ThisTime = Boss->MainDraw.Anim.anim_time;
        f32 LastTime = ThisTime - Boss->MainDraw.Model->animlist[0x1B]->speed *
                                      0.5999999642f;
        s32 Footstep = 0;

        if (ProcessTimer(&Boss->FootTime)) {
            switch (Boss->MainDraw.Anim.action) {
            case 0x1B:
            case 0x3A:
            case 0x3B:
                if (ThisTime >= 1.0f && LastTime < 1.0f) {
                    Footstep = 0xD;
                }
                if (ThisTime >= 30.0f && LastTime < 30.0f) {
                    Footstep = 0xC;
                }
                break;
            }
        }
        if (Footstep != 0) {
            MyGameSfx(0xCE, &Boss->Pos, FOOTSTEPVOL);
            JudderGameCamera(GameCam, FOOTJUDDERTIME, &Boss->Pos);
            NewRumble(&player->rumble, FOOTRUMBLEPOWER);
            Boss->FootTime = 0.3f;
            if (Boss->Action == 3) {
                struct nuvec_s DustPos;

                DustPos = *(struct nuvec_s *)&Boss->Locator[Footstep]._30;
                AddGameDebris(0x99, &DustPos);
            }
        }
    }

    {
        struct nuvec_s RockPos;
        struct nuvec_s RockVel;

        if (Boss->Action == 3 || Boss->Action == 4) {
            f32 Start;
            f32 Stop;

            if (Boss->DropSide != 0) {
                Start = 50.0f;
                Stop = 60.0f;
            } else {
                Start = 20.0f;
                Stop = 30.0f;
            }
            if (Boss->Rock != 0 && Boss->MainDraw.Anim.action == 0x3A &&
                Boss->MainDraw.Anim.anim_time >= Start) {
                if (Boss->MainDraw.Anim.anim_time <= Stop) {
                    NuMtxSetScale(&mTEMP, &FIREBOSSSCALE);
                    NuMtxRotateY(&mTEMP, (Boss->AngleY + 180.0f) * 182.0444489f);
                    NuMtxTranslate(&mTEMP, &Boss->Pos);
                    if (Boss->Pass < 3) {
                        RockVel = SetNuVec(frandPN() * 3.0f, -1.0f, -2.0f);
                    } else {
                        RockVel = SetNuVec(0.0f, -0.5f, 0.0f);
                    }
                    GetLocatorMtxMyDraw(&Boss->MainDraw, Boss->Locator, &mTEMP);
                    RockPos = *(struct nuvec_s *)&Boss->Locator[Boss->DropSide]._30;
                    KeepHoldOnRock(Boss->Rock, &RockPos, &RockVel);
                    goto done;
                }
            }
        } else if (Boss->Rock != 0 && Boss->MainDraw.Anim.action == 0x1B &&
                   Boss->MainDraw.Anim.anim_time < 29.0f) {
            struct nuvec_s ThrowPos;
            struct nuvec_s ThrowVel;

            NuMtxSetScale(&mTEMP, &FIREBOSSSCALE);
            NuMtxRotateY(&mTEMP, (Boss->AngleY + 180.0f) * 182.0444489f);
            NuMtxTranslate(&mTEMP, &Boss->Pos);
            NuVecMtxRotate(&ThrowVel,
                           SetNuVecPntr(frandPN() * 0.4f, -0.3f, -1.0f), &mTEMP);
            NuVecNorm(&ThrowVel, &ThrowVel);
            NuVecScale(&ThrowVel, &ThrowVel, FIREBOSSTHROWSPEED);
            GetLocatorMtxMyDraw(&Boss->MainDraw, Boss->Locator, &mTEMP);
            ThrowPos.x = (Boss->Locator[0]._30 + Boss->Locator[1]._30) * 0.5f;
            ThrowPos.y = (Boss->Locator[0]._31 + Boss->Locator[1]._31) * 0.5f;
            ThrowPos.z = (Boss->Locator[0]._32 + Boss->Locator[1]._32) * 0.5f;
            KeepHoldOnRock(Boss->Rock, &ThrowPos, &ThrowVel);
            goto done;
        }
        Boss->Rock = 0;
    }
done:
    if (FireBoss.BeenHitThisPass != 0) {
        JonnySteam(Boss);
        FireBoss.BeenHitThisPassTimer += 0.01999999955f;
    } else {
        FireBoss.BeenHitThisPassTimer = 0.0f;
    }
}


void InitJeepBalloons(void) {
    s32 i;

    for (i = 0; i < 6; i++) {
        JeepBalloon[i].Active = 0;
    }
}

void ProcessJeepBalloons(void) {
    s32 i;

    for (i = 0; i < 6; i++) {
        if (JeepBalloon[i].Active != 0) {
            ProcessJeepBalloon(&JeepBalloon[i]);
        }
    }
}


void DrawJeepBalloons(void) {
    s32 i;

    for (i = 0; i < 6; i++) {
        if (JeepBalloon[i].Active != 0) {
            DrawJeepBalloon(&JeepBalloon[i]);
        }
    }
}


inline struct JEEPBALLOON *FindJeepBalloon(void) {
    s32 i;

    for (i = 0; i < 6; i++) {
        if (JeepBalloon[i].Active == 0) {
            return &JeepBalloon[i];
        }
    }
    return 0;
}


s32 AddBalloon(struct nuvec_s *Pos, struct nuvec_s *Vel) {
    struct JEEPBALLOON *Balloon;
    s32 i;

    Balloon = FindJeepBalloon();
    if (Balloon != 0) {
        memset(Balloon, 0, 0x30);
        Balloon->Active = 1;
        Balloon->Vel = *Vel;
        Balloon->Pos = *Pos;
        Balloon->Life = 5.0f;
        Balloon->AngY = (short)NuAtan2D(Vel->x * 100.0f, Vel->z * 100.0f);
        i = 1;
    } else {
        i = 0;
    }
    return i;
}

void DrawEnemyJeeps(void) {
    s32 i;

    for (i = 0; i < 4; i++) {
        if (EnemyJeep[i].Active != 0) {
            DrawEnemyJeep(&EnemyJeep[i]);
        }
    }
}

void ProcessEnemyJeeps(void) {
    s32 i;

    for (i = 0; i < 4; i++) {
        if (EnemyJeep[i].Active != 0) {
            ProcessEnemyJeep(&EnemyJeep[i]);
        }
    }
}

inline void AnimateForLightsEnemyJeep(struct enemyjeep_s *j) {
    MyAnimateModelNew(&j->Draw, 0.5999999642f);
}

void AnimateForLightsEnemyJeeps(void) {
    s32 i;

    for (i = 0; i < 4; i++) {
        if (EnemyJeep[i].Active != 0) {
            AnimateForLightsEnemyJeep(&EnemyJeep[i]);
        }
    }
}
