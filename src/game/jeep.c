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

#include "creature.h"

extern struct nuvec_s BaseWheelPosition[];
extern f32 DotProduct(struct nuvec_s *a, struct nuvec_s *b);
extern void ApplyFriction(f32 *val, f32 rate, f32 dt);
extern struct nuvec_s WorldAxis[3];
extern f32 JeepCamTween;
extern f32 JeepRailCamTween;
extern f32 JeepRailCamTween2;
extern s32 ResetJeepCamera;
extern struct nuvec_s WesternPlayerStart;
extern void NewInitTrail(void);
extern f32 ASin360f(f32 x);
extern s32 MyDrawModelNew(struct MYDRAW *Draw, struct numtx_s *mC,
                          struct numtx_s *loc_mtx);
extern void GetLocatorMtx(struct CharacterModel *model, struct numtx_s *dest,
                          f32 angle);

struct fireboss_s;
extern struct fireboss_s FireBoss;

extern s32 DrawFireBoss(struct fireboss_s *fb);
extern void AddVariableShotDebrisEffect(s32 effect, struct nuvec_s *pos, s32 n,
                                        s32 xrot, s32 yrot);
extern s32 TrailActive[];
extern char D_006319D8[];
#define ActionIntroText D_006319D8
extern char D_006319E0[];
#define ActionTiredText D_006319E0
extern char D_0061FB40[];
#define ActionFrontHurlText D_0061FB40
extern char D_006319E8[];
#define ActionRunawayText D_006319E8
extern char D_0061FB50[];
#define ActionGetAngryText D_0061FB50
extern char D_006319F0[];
#define ActionDeadText D_006319F0
extern char D_006319F8[];
#define ActionNoneText D_006319F8
extern void DrawJeepRocks(void);
extern void DrawVehMasks(void);
extern void *app_fnt;
extern void NuFntSet(void *fnt);
extern void NuFntSetPen(u32 colour);
extern f32 NuFsqrt(f32 x);
extern f32 NuTrigTable[];
extern void SeekHalfLife(f32 *dest, f32 target, f32 halflife, f32 dt);
extern f32 fsign(f32 x);
extern f32 NewShadowMaskPlat(struct nuvec_s *pos, f32 y0, s32 flag);
extern s32 ShadowInfo(void);

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
    f32 LookaheadDist;              /* 0x2C */
}; /* 0x30 */
extern struct SPLINEFOLLOW JeepFollowSpline;

extern void FindSplineTargetPoint(struct SPLINEFOLLOW *Spline, s32 Control,
                                  struct nuvec_s *Point,
                                  struct nuvec_s *Direction, s32 Wrap,
                                  s32 BigLook);
extern f32 FindSplineClosestPointAndDist(struct SPLINEFOLLOW *Spline,
                                         s32 Control, struct nuvec_s *Pos,
                                         struct nuvec_s *Closest, s32 Wrap,
                                         s32 BigLook);

/* One simulated wheel of the vehicle trail system. */
struct SIMWHEEL {
    struct nuvec_s Position;        /* 0x00 */
    struct nuvec_s OldPosition;     /* 0x0C */
    f32 TrailWidth;                 /* 0x18 */
    f32 Radius;                     /* 0x1C */
    s32 Platform;                   /* 0x20 */
    s32 SurfaceType;                /* 0x24 */
}; /* 0x28 */

/* A restart position plus a facing angle. */
struct POINTANG {
    f32 x;                          /* 0x00 */
    f32 y;                          /* 0x04 */
    f32 z;                          /* 0x08 */
    s32 Ang;                        /* 0x0C */
}; /* 0x10 */

/* Shared vehicle physics state (jeep / atlasphere / ...). */
struct VEHICLE {
    struct nuvec_s ActualWheelPosition[4]; /* 0x00 */
    struct nuvec_s OldWheelPosition[4];    /* 0x30 */
    s32 BigSpin[4];                        /* 0x60 */
    struct nuvec_s ActualPosition;         /* 0x70 */
    struct nuvec_s Resolved;               /* 0x7C */
    struct nuvec_s Velocity;               /* 0x88 */
    struct nuvec_s WheelAxis[3];           /* 0x94 */
    f32 FrontWheelSpeedAdj;                /* 0xB8 */
    short aTargetAngle;                    /* 0xBC */
    short aTarSurfRotX;                    /* 0xBE */
    short aTarSurfRotZ;                    /* 0xC0 */
    u16 aActualAngle;                      /* 0xC2 */
    short aActSurfRotX;                    /* 0xC4 */
    short aActSurfRotZ;                    /* 0xC6 */
    short ActFrontRotX;                    /* 0xC8 */
    short ActRearRotX;                     /* 0xCA */
    short TarFrontRotX;                    /* 0xCC */
    short TarRearRotX;                     /* 0xCE */
    s32 AnyOnGroundBits;                   /* 0xD0 */
    s32 AllOnGroundBits;                   /* 0xD4 */
    s32 AllTouchingGroundBits;             /* 0xD8 */
    s32 AnyTouchingGroundBits;             /* 0xDC */
    struct nuvec_s AirNormal;              /* 0xE0 */
    struct nuvec_s SurfaceNormal;          /* 0xEC */
    short *TerrHandle;                     /* 0xF8 */
    s32 FrontWheelGroundBits;              /* 0xFC */
}; /* 0x100 */

struct MYDRAW {
    struct anim_s Anim;             /* 0x00 */
    struct CharacterModel *model;   /* 0x1C */
    s32 index;                      /* 0x20 */
    s32 numjoints;                  /* 0x24 */
    void *jointlist;                /* 0x28 */
    s32 field2C;                    /* 0x2C */
    struct Nearest_Light_s lights;  /* 0x30 */
}; /* 0xE0 */

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
    u8 pad_688[0x8];                /* 0x688 */
}; /* 0x690 */
extern f32 FIREBOSSSTART;

struct jointctrl_s {
    f32 rx, ry, rz;                 /* 0x00 */
    f32 tx, ty, tz;                 /* 0x0C */
    f32 sx, sy, sz;                 /* 0x18 */
    u8 unk_0x24[0xC];               /* 0x24 */
    s8 joint;                       /* 0x30 */
    s8 mode;                        /* 0x31 */
    u8 pad_32[2];
}; /* 0x34 */

struct JEEPSTRUCT {
    struct creature_s *Cre;              /* 0x000 */
    struct MYDRAW ChassisDraw;           /* 0x004 */
    struct numtx_s ChassisLocators[16];  /* 0x0E4 */
    struct numtx_s DrawMtx;              /* 0x4E4 */
    struct jointctrl_s Joints[8];        /* 0x524 */
    struct SIMWHEEL TrailWheel[4];       /* 0x6C4 */
    struct POINTANG RestartPoint;        /* 0x764 */
    f32 DownHoleTimer;                   /* 0x774 */
    s32 DownHole;                        /* 0x778 */
    s32 Dropped;                         /* 0x77C */
    struct nuvec_s RestartCamPos;        /* 0x780 */
    struct nuvec_s RestartCamObj;        /* 0x78C */
    f32 FireBossTurnTimer;               /* 0x798 */
    f32 WheelHeight[4];                  /* 0x79C */
    f32 TimeLine;                        /* 0x7AC */
    s32 FireBossDir;                     /* 0x7B0 */
    s32 CantMove;                        /* 0x7B4 */
    s32 Quick;                           /* 0x7B8 */
    f32 MaxSpeed;                        /* 0x7BC */
    f32 MySpeed;                         /* 0x7C0 */
    f32 DefaultSpeed;                    /* 0x7C4 */
    f32 StartSpeed;                      /* 0x7C8 */
    f32 StartSpeedTimer;                 /* 0x7CC */
    s32 Active;                          /* 0x7D0 */
    short aWRot[4];                      /* 0x7D4 */
    u16 aFrontWheelAng;                  /* 0x7DC */
    u16 aOldFrontWheelAng;               /* 0x7DE */
    struct nuvec_s Pos;                  /* 0x7E0 */
    short aAngleY;                       /* 0x7EC */
    short aMovementAng;                  /* 0x7EE */
    short aSurfRotX;                     /* 0x7F0 */
    short aSurfRotZ;                     /* 0x7F2 */
    f32 TiltSeekTime;                    /* 0x7F4 */
    short aTiltX;                        /* 0x7F8 */
    short aTiltZ;                        /* 0x7FA */
    short aDestTiltX;                    /* 0x7FC */
    short aDestTiltZ;                    /* 0x7FE */
    short aInputAng;                     /* 0x800 */
    f32 InputSpeed;                      /* 0x804 */
    s32 WheelSpin;                       /* 0x808 */
    short aDeltaAng;                     /* 0x80C */
    short aLastDeltaAng;                 /* 0x80E */
    short aLastDeltaAngA;                /* 0x810 */
    u16 aOldChassisAngleY;               /* 0x812 */
    u16 aChassisAngleY;                  /* 0x814 */
    u16 aChassisTargetAngleY;            /* 0x816 */
    s32 aChassisAngMom;                  /* 0x818 */
    f32 Accelerator;                     /* 0x81C */
    f32 AccelerationForce;               /* 0x820 */
    f32 CentrefugalForce;                /* 0x824 */
    f32 Traction;                        /* 0x828 */
    f32 GroundTractionAcc;               /* 0x82C */
    f32 TurnSin;                         /* 0x830 */
    short aBaseMoveAng;                  /* 0x834 */
    f32 CentRailDist;                    /* 0x838 */
    f32 BoostOnTimer;                    /* 0x83C */
    f32 BoostTimer;                      /* 0x840 */
    s32 Finished;                        /* 0x844 */
    f32 CarryOnRecordTime;               /* 0x848 */
    f32 FloorHeight;                     /* 0x84C */
    s32 TerrainType;                     /* 0x850 */
    struct SPLINEFOLLOW Spline;          /* 0x854 */
    f32 FireTimer;                       /* 0x884 */
    struct VEHICLE Move;                 /* 0x888 */
}; /* 0x988 */
extern struct JEEPSTRUCT PlayerJeep;

extern struct SIMWHEEL GenericTrail[];
extern void ProcessJeepTrail(struct SIMWHEEL *Wheel, s32 TrailIndx);
extern f32 NewShadowMask(struct nuvec_s *pos, f32 size, s32 extramask);

struct jeeptrail_s {
    struct nuvec_s pos1;            /* 0x00 */
    struct nuvec_s pos2;            /* 0x0C */
    s32 intensity;                  /* 0x18 */
    s32 RealIntensity;              /* 0x1C */
};
extern struct jeeptrail_s JeepTrail[][0x20];
extern s32 TrailPntr[];
extern s32 TrailAir[];
extern void NuRndrTrail(s32 pntr, struct jeeptrail_s *trail, s32 count);
extern s32 NewIntoTrail[];
extern f32 NewCurrentIntensity[];

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
extern struct JEEPBALLOON JeepBalloon[];
extern void ProcessJeepBalloon(struct JEEPBALLOON *b);
extern void DrawJeepBalloon(struct JEEPBALLOON *b);

struct objtab_s {
    struct nugscn_s *scene;         /* 0x00 */
    struct nuspecial_s *special;    /* 0x04 */
    u8 pad_08[0x18];
}; /* 0x20 */
extern struct objtab_s ObjTab[];
extern struct numtx_s mTEMP;
extern struct nuvec_s FIREBOSSSCALE;
extern s32 WallOfFireOn;
extern struct nuvec_s WallOfFirePosition;
extern f32 WallOfFireAngleY;
extern s32 Paused;

struct gdeb_s {
    s32 i;                          /* 0x00 */
    char *name;                     /* 0x04 */
    u64 levbits;                    /* 0x08 */
}; /* 0x10 */
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
extern struct enemyjeep_s EnemyJeep[];
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
extern struct spline_s *D_00586394[]; /* Rail[0].pCAM (far .data -> absolute) */
extern f32 WesternCountdown;
extern s32 SmokeyFinished;
extern f32 WesternTime;
extern s32 SmokeyCountDownValue;
extern struct spline_s *JeepIntroLookSpline;
extern struct spline_s *JeepIntroCamSpline;
extern s32 SmokeyCam;
extern s32 SmokeyPosition;
extern s32 CurrentWesternPosition(void);
extern void ProcessJeepTrails(void);

extern char LevelFileName[];
extern struct BUGGYDATA *BuggyData[];
extern void *superbuffer_ptr;
extern void *superbuffer_end;
extern struct nugscn_s *world_scene[];
extern void LoadScreen(char *name);
extern void TerrainSetCur(void *buf);
extern short *terraininit(s32 level, void **pptr, void *end, s32 a, char *name,
                        struct nugscn_s *scene, s32 b);
extern char D_0061FAC0[];
#define LoadingAiPathsText D_0061FAC0
extern s32 BuggyDataMax[];

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
extern short D_006332E0;
#define WaterCurrentAngleY D_006332E0
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




struct pad_s {
    u8 unk_0x000[0x55C];            /* 0x000 (opaque) */
    unsigned int buttons;           /* 0x55C (held buttons) */
    u8 unk_0x560[0x1E];             /* 0x560 */
    u8 l_alg_x;                     /* 0x57E */
};

extern s32 ProcessTimer(f32 *Timer);
extern struct nuvec_s *SetNuVecPntr(f32 x, f32 y, f32 z);
extern struct nuvec_s SetNuVec(f32 x, f32 y, f32 z);
extern s32 NewRayCast(struct nuvec_s *pos, struct nuvec_s *dir, f32 size);
extern void AddGameDebris(s32 type, struct nuvec_s *pos);
extern s32 NewRayCastSetHandel(struct nuvec_s *vpos, struct nuvec_s *vvel,
                               f32 size, f32 timeadj, f32 impactadj,
                               short *Handel);
extern void DriveJeep(struct JEEPSTRUCT *Jeep, struct pad_s *Pad);
extern void ProcessJeepMovement(struct JEEPSTRUCT *Jeep, f32 DeltaTime);
extern struct nuvec_s TerrainVehicleSoft(struct VEHICLE *Vehicle,
                                         struct nuvec_s *Vel);
extern void FindSurfaceNormalAndUnembedd(struct VEHICLE *Vehicle,
                                         f32 DeltaTime);
extern void SeekAngHalfLife(u16 *dest, short target, f32 halflife, f32 dt);
extern void FindTerrainType(struct JEEPSTRUCT *Jeep);
extern struct RPos_s *best_cRPos;
extern short JeepBaseAng;
extern f32 D_006332E8;
#define JeepCamYaw D_006332E8
extern f32 D_006332E4;
#define JeepCamPitch D_006332E4
extern short D_005865C0[];
#define FireBossRailAng D_005865C0
extern s32 RotDiff(u16 a, u16 b);
extern void PlayerCreatureCollisions(struct obj_s *obj);
extern s32 HitItems(struct obj_s *obj);
extern s32 HitCrates(struct obj_s *obj, s32 destroy);
extern s32 WumpaCollisions(struct obj_s *obj);
extern void AddGameDebrisRot(s32 i, struct nuvec_s *pos, s32 n, u16 xrot,
                             u16 yrot);
struct cammtx_s {
    struct numtx_s m;               /* 0x00 */
    struct numtx_s m0;              /* 0x40 */
    struct nuvec_s vX;              /* 0x80 */
    struct nuvec_s vY;              /* 0x8C */
    struct nuvec_s vZ;              /* 0x98 */
    struct nuvec_s pos;             /* 0xA4 */
    struct nuvec_s oldpos;          /* 0xB0 */
    struct nuvec_s newpos;          /* 0xBC */
    struct nuvec_s pos2;            /* 0xC8 */
    struct nuvec_s seek;            /* 0xD4 */
    f32 distance;                   /* 0xE0 */
    f32 ahead;                      /* 0xE4 */
    f32 judder;                     /* 0xE8 */
    f32 blend_time;                 /* 0xEC */
    f32 blend_duration;             /* 0xF0 */
    s32 xrot;                       /* 0xF4 */
    s32 yrot;                       /* 0xF8 */
    s32 zrot;                       /* 0xFC */
    u16 old_xrot;                   /* 0x100 */
    u16 new_xrot;                   /* 0x102 */
    u16 old_yrot;                   /* 0x104 */
    u16 new_yrot;                   /* 0x106 */
    u16 old_zrot;                   /* 0x108 */
    u16 new_zrot;                   /* 0x10A */
    u16 hdg_to_player;              /* 0x10C */
    s16 route;                      /* 0x10E */
    s16 i0;                         /* 0x110 */
    s16 i1;                         /* 0x112 */
    s8 mode;                        /* 0x114 */
    s8 iRAIL;                       /* 0x115 */
    s16 iALONG;                     /* 0x116 */
    u16 angle;                      /* 0x118 */
    s8 vertical;                    /* 0x11A */
    u8 unk_0x11B[0x1];
}; /* 0x11C */
struct rail_s {
    struct spline_s *pINPLAT;       /* 0x00 */
    struct spline_s *pINCAM;        /* 0x04 */
    struct spline_s *pLEFT;         /* 0x08 */
    struct spline_s *pCAM;          /* 0x0C */
    struct spline_s *pRIGHT;        /* 0x10 */
    struct spline_s *pOUTCAM;       /* 0x14 */
    struct spline_s *pOUTPLAT;      /* 0x18 */
    f32 in_distance;                /* 0x1C */
    f32 out_distance;               /* 0x20 */
    s16 edges;                      /* 0x24 */
    s8 type;                        /* 0x26 */
    s8 circuit;                     /* 0x27 */
}; /* 0x28 */
extern struct rail_s Rail[];
extern struct spline_s *pVIS;
extern s32 iVIS;
extern struct nuvec_s JeepvPos;
extern struct nuvec_s JeepvObj;
extern struct nuvec_s D_006B75E0;
#define JeepCamVel D_006B75E0
extern struct nuvec_s D_006B75F0;
#define JeepCamObjTween D_006B75F0
extern struct nuvec_s D_006B7600;
#define JeepCamDropPos D_006B7600
extern short D_006332B4;
#define JeepCamAngY D_006332B4
extern short D_006332B6;
#define JeepCamAngX D_006332B6
extern f32 D_006332B8;
#define JeepCamDist D_006332B8
extern f32 D_006332BC;
#define JeepCamYawVel D_006332BC
extern s32 D_006332C0;
#define JeepCamVelValid D_006332C0
extern f32 D_006332C4;
#define JeepCamDropPitch D_006332C4
extern f32 D_006332C8;
#define JeepCamDropYaw D_006332C8
extern s32 JeepCamSnapShot;
extern struct nuvec_s JeepCamPosTween;
extern struct nuvec_s JeepCamPos2;
extern struct nuvec_s v000;
extern void SeekHalfLifeNUVEC(struct nuvec_s *src, struct nuvec_s *target,
                              f32 halflife, f32 dt);
extern void SeekAngHalfLife360f(f32 *dest, f32 target, f32 halflife, f32 dt);
extern f32 Rationalise360f(f32 a);
extern void JeepCamFollowAng(struct cammtx_s *cam, s32 blend);
extern s32 AddBalloon(struct nuvec_s *Pos, struct nuvec_s *Vel);
extern void DrawCross(struct nuvec_s *pos, f32 size, s32 colour);
extern void DrawLine(struct nuvec_s *p0, struct nuvec_s *p1, s32 colour);
extern void MoveRailPosition(struct nuvec_s *dst, struct RPos_s *rpos,
                             f32 distance, s32 direction);
extern struct RPos_s cRPos[];
extern s32 cRPosCOUNT;
extern f32 JCDist;
extern s32 ChrisInTheHouse;
extern s32 HighCam;
extern struct nuvec_s FireBossPosition;

extern s32 JeepFrame;
extern s32 JeepInControl;
extern s32 gggg;
extern s32 JamesInTheHouse;
extern s32 CurrentDebugJeep;
extern s32 D_006332B0;              /* debug-jeep button edge latch */
extern s32 temp_crate_type;
extern s32 Level;
extern f32 DropAdj;
extern struct nuvec_s ExhaustPos[2];
extern struct POINTANG WesternRestartPoints[];
extern struct nuvec_s IdealCamPos;
extern struct nuvec_s IdealObjPos;
extern f32 D_005B9548[4];           /* MinHeight[4] (retail-owned .data) */
#define MinHeight D_005B9548
extern s32 SHEIGHT;
extern void NuFntScale(s32 x, s32 y);
extern void NuFntPrintEx(s32 x, s32 y, s32 colour, char *fmt, ...);
extern char D_0061FA70[];
#define DebugIdFmt D_0061FA70
extern char D_00631960[];
#define AngFmt D_00631960
extern char D_0061FA80[];
#define ChasAngFmt D_0061FA80
extern char D_0061FA90[];
#define AngMomFmt D_0061FA90
extern char D_0061FAA0[];
#define JeepFrameFmt D_0061FAA0
extern void GameSfx(s32 id, struct nuvec_s *pos);
extern s32 D_006332CC;
#define LastWesternPosition D_006332CC
extern struct nuvec_s RelTemp;
extern s32 WallOfFireAttatched;
extern char D_0061FB60[];
#define BossActionFmt D_0061FB60
extern char D_0061FB88[];
#define BossPosFmt D_0061FB88
extern char D_0061FBA8[];
#define BossRelFmt D_0061FBA8
extern char D_0061FBD0[];
#define WallAttachedText D_0061FBD0
extern char D_0061FBE8[];
#define WallFreeText D_0061FBE8
extern char D_0061FBF8[];
#define HoldPlayerText D_0061FBF8
extern char D_0061FC08[];
#define BossFrameFmt D_0061FC08
extern char D_0061FC28[];
#define BossCoordFmt D_0061FC28

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
    f32 Temp;
    short aDelta;
    f32 TurnSin;

    aDelta = Jeep->aInputAng - Jeep->aChassisAngleY;
    Temp = (Jeep->MySpeed * 4.0f - Jeep->Move.Resolved.z) /
           (Jeep->MySpeed * 8.0f) * aDelta;
    aDelta = Temp;
    TurnSin = NuTrigTable[(u16)aDelta];
    if (aDelta > 0x4000) {
        TurnSin = 1.0f;
    } else if (aDelta < -0x4000) {
        TurnSin = -1.0f;
    }
    Jeep->TurnSin = TurnSin;
    Jeep->aChassisAngMom += (s32)(TurnSin * 4369.066895f);
    Temp = Jeep->aChassisAngMom;
    Jeep->aChassisTargetAngleY =
        Temp * DeltaTime + (short)Jeep->aChassisAngleY;
    if (abs(Jeep->aChassisAngMom) < 0x1C73 && abs(aDelta) < 0x38F) {
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


void MovePlayerJeep(struct creature_s *Cre, struct pad_s *Pad) {
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
        s32 press = Pad->buttons & 0x20;
        if (press != 0 && D_006332B0 == 0) {
            CurrentDebugJeep++;
            if (CurrentDebugJeep > 4) {
                CurrentDebugJeep = -1;
            }
        }
        D_006332B0 = press;
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
        (Pad->buttons & 0x40) != 0) {
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
    ang = PlayerJeep.aChassisAngleY;
    old_hdg = Cre->obj.hdg;
    Cre->obj.hdg = ang;
    Cre->obj.thdg = ang;
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
        u16 YAng = Jeep->aChassisAngleY + 0x8000;

        NewGenerateJeepMatrix(&Jeep->DrawMtx, YAng, Jeep->aSurfRotX,
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
                Jeep->CantMove = 1;
                Jeep->DownHole = 1;
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


void DriveJeep(struct JEEPSTRUCT *Jeep, struct pad_s *Pad) {
    short aRailAng = 0;
    f32 Steer;
    f32 Speed2;
    f32 OldWheelSpin;
    short aDelta;
    struct nuvec_s *Vel;
    struct nuvec_s Move;

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
            Steer = Pad->l_alg_x - 0x7F;
            if (Pad->buttons & 0x40) {
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
                        JeepCamYaw * 182.0444489f + Steer * 10923.0f;
                }
            } else if (Level == 0x16 && Jeep->FireBossDir == 0) {
                Jeep->aInputAng = aRailAng - 0x8000;
            } else {
                Jeep->aInputAng = Jeep->aChassisAngleY;
            }
            aDelta = Jeep->aInputAng - Jeep->aBaseMoveAng;
            if (aDelta > 0x4000 || aDelta < -0x4000) {
                Jeep->aInputAng = Jeep->aChassisAngleY;
                Jeep->InputSpeed = 0.0f;
            } else if (aDelta > 0x2000) {
                Jeep->aInputAng = Jeep->aBaseMoveAng + 0x2000;
            } else if (aDelta < -0x2000) {
                Jeep->aInputAng = Jeep->aBaseMoveAng - 0x2000;
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
    aDelta = Jeep->aInputAng - Jeep->aChassisAngleY;
    Jeep->aDeltaAng = aDelta;
    OldWheelSpin = Jeep->WheelSpin;
    if ((Jeep->WheelSpin != 0 && aDelta >= 0xE3A) || aDelta >= 0x471D) {
        Jeep->WheelSpin = 1;
    } else if (Speed2 < 4.0f && aDelta > 0x2000) {
        Jeep->WheelSpin = 1;
    } else {
        Jeep->WheelSpin = 0;
    }
    if (Jeep->WheelSpin != 0) {
        Jeep->aLastDeltaAng = aDelta;
    } else if (OldWheelSpin != 0.0f) {
        Jeep->aLastDeltaAngA = aDelta;
    }
    Jeep->Accelerator = Jeep->InputSpeed;
    if (Jeep->WheelSpin != 0 && Jeep->InputSpeed < 0.5f) {
        Jeep->Accelerator = 0.5f;
    }
    GenerateJeepTargets(Jeep, 0.01999999955f);
    Jeep->aAngleY = Jeep->aChassisTargetAngleY + ((short)Jeep->aFrontWheelAng >> 3);
    ProcessJeepMovement(Jeep, 0.01999999955f);
    Jeep->Move.aTargetAngle = Jeep->aChassisTargetAngleY;
    JeepBaseAng = Jeep->aBaseMoveAng;
    if (Jeep->Quick != 0) {
        NuVecScaleAccum(&Jeep->Move.ActualPosition, Vel, 0.01999999955f);
        Jeep->Move.aActSurfRotX = 0;
        Jeep->Move.aActualAngle = Jeep->aChassisTargetAngleY;
        Jeep->aChassisAngleY = Jeep->aChassisTargetAngleY;
        Jeep->aSurfRotX = 0;
        Jeep->Move.aActSurfRotZ = 0;
        Jeep->aSurfRotZ = 0;
    } else {
        NuVecScale(&Move, Vel, 0.01999999955f);
        CheckAgainstRocks(&Jeep->Move.ActualPosition, &Move);
        TerrainVehicleSoft(&Jeep->Move, &Move);
        Jeep->aChassisAngleY = Jeep->Move.aActualAngle;
        Jeep->aSurfRotX = Jeep->Move.aActSurfRotX;
        Jeep->aSurfRotZ = Jeep->Move.aActSurfRotZ;
        TiltJeep(Jeep, 0.01999999955f);
        FindSurfaceNormalAndUnembedd(&Jeep->Move, 0.01999999955f);
    }
    Jeep->Pos = Jeep->Move.ActualPosition;
    aDelta = Jeep->aInputAng - Jeep->aChassisAngleY;
    if (aDelta >= 0x2000) {
        aDelta = 0x2000;
    } else if (aDelta <= -0x2000) {
        aDelta = -0x2000;
    }
    SeekAngHalfLife(&Jeep->aFrontWheelAng, aDelta, 0.1f, 0.01999999955f);
    SpinWheelsJeep(Jeep, 0.01999999955f);
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

    GetLocatorMtx(Jeep->ChassisDraw.model, Locators, 180.0f);
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
    for (i = 0; i < PlayerJeep.ChassisDraw.numjoints; i++) {
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
        NuFntPrintEx(0x80, SHEIGHT, 0, DebugIdFmt, CurrentDebugJeep);
        NuFntPrintEx(0x80, SHEIGHT * 3 / 2, 0, AngFmt,
                     (short)PlayerJeep.Move.aActualAngle);
        NuFntPrintEx(0x80, SHEIGHT * 2, 0, ChasAngFmt,
                     (short)PlayerJeep.aChassisAngleY);
        NuFntPrintEx(0x80, SHEIGHT * 5 / 2, 0, AngMomFmt,
                     PlayerJeep.aChassisAngMom);
    } else {
        NuFntSet(app_fnt);
        NuFntSetPen(0x7F7F7F7F);
        NuFntScale(16, 16);
        NuFntPrintEx(0x80, SHEIGHT, 0, JeepFrameFmt, JeepFrame);
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
    JeepCamYaw = NuAtani(Dir.x * 256.0f, Dir.z * 256.0f) / 182.0444489f;
    JeepCamPitch = NuAtani(-Dir.y * 256.0f, Dist * 256.0f) / 182.0444489f;
    NuMtxSetRotationX(&Cam->m, JeepCamPitch * 182.0444489f);
    NuMtxRotateY(&Cam->m, JeepCamYaw * 182.0444489f);
    NuMtxTranslate(&Cam->m, Pos);
    Cam->pos = *Pos;
    Cam->xrot = JeepCamPitch * 182.0444489f;
    Cam->yrot = JeepCamYaw * 182.0444489f;
}


void JeepCamFollowAng(struct cammtx_s *Cam, s32 Snap) {
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
        if (JeepCamVelValid != 0) {
            JeepCamVelValid = 0;
            JeepCamVel.y = 0.0f;
            JeepCamDropPos = *(struct nuvec_s *)&Cam->m._30;
            JeepCamDropPitch = JeepCamPitch;
            JeepCamDropYaw = JeepCamYaw;
        }
        InvBlend = 1.0f - Blend;
        NuVecScaleAccum(&JeepCamDropPos, &JeepCamVel, InvBlend);
        NuMtxSetRotationX(&mTEMP, JeepCamDropPitch * 182.0444489f);
        NuMtxRotateY(&mTEMP, JeepCamDropYaw * 182.0444489f);
        NuMtxTranslate(&mTEMP, &JeepCamDropPos);
        NuVecMtxTransform(&Ahead, SetNuVecPntr(0.0f, 0.0f, 5.0f), &mTEMP);
        BlendNUVECs(&Pos, &JeepCamDropPos, &Jeep->RestartCamPos, Blend);
        BlendNUVECs(&Obj, &Ahead, &Jeep->RestartCamObj, Blend);
        DoCamMtx(Cam, &Obj, &Pos);
    } else if (Jeep->Finished != 0 && Snap == 0) {
        SeekAngHalfLife360f(&JeepCamYawVel, 0.0f, 0.5f, 0.01999999955f);
        SeekHalfLifeNUVEC(&JeepCamVel, &v000, 0.5f, 0.01999999955f);
        Pos.x = Cam->m._30;
        Pos.y = Cam->m._31;
        Pos.z = Cam->m._32;
        NuVecAdd(&Pos, &Pos, &JeepCamVel);
        if (JeepFollowSpline.Spline != 0) {
            LimitCam(&JeepFollowSpline, &Pos, 5.0f, -1.25f);
        }
        JeepCamYaw = Rationalise360f(JeepCamYaw + JeepCamYawVel);
        NuMtxSetRotationX(&Cam->m, JeepCamPitch * 182.0444489f);
        NuMtxRotateY(&Cam->m, JeepCamYaw * 182.0444489f);
        NuMtxTranslate(&Cam->m, &Pos);
        Cam->xrot = JeepCamPitch * 182.0444489f;
        Cam->yrot = JeepCamYaw * 182.0444489f;
        Cam->pos = Pos;
    } else {
        if (JeepCamSnapShot != 0 && Snap == 0) {
            JeepCamSnapShot = 0;
            JeepCamPosTween = *(struct nuvec_s *)&Cam->m._30;
            NuVecMtxTransform(&JeepCamObjTween, SetNuVecPntr(0.0f, 0.0f, 20.0f),
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
                JeepCamAngX = aPitch;
                JeepCamAngY = Jeep->aAngleY;
                JeepCamDist = 5.0f;
            } else {
                JeepCamAngX = aPitch;
                JeepCamAngY = Jeep->aAngleY;
                JeepCamDist = 5.0f;
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
            JeepCamAngY = NuAtani(Dir.x * 256.0f, Dir.z * 256.0f);
            JeepCamAngX = NuAtani(-Dir.y * 256.0f, Dist * 256.0f);
            JeepCamDist = NuVecMag(&Dir);
            if (Jeep->Finished != 0) {
                aYaw = 0;
            } else {
                aYaw = Jeep->aAngleY;
            }
            if (Level == 0x16) {
                SeekAngHalfLife((u16 *)&JeepCamAngY, aYaw, 0.5f, 0.01999999955f);
                SeekAngHalfLife((u16 *)&JeepCamAngX, aPitch, 0.009999999776f,
                                0.01999999955f);
                SeekHalfLife(&JeepCamDist, 5.0f, 0.1f, 0.01999999955f);
            } else {
                SeekAngHalfLife((u16 *)&JeepCamAngY, aYaw, 1.0f, 0.01999999955f);
                SeekAngHalfLife((u16 *)&JeepCamAngX, aPitch, 0.009999999776f,
                                0.01999999955f);
                SeekHalfLife(&JeepCamDist, 5.0f, 0.1f, 0.01999999955f);
            }
        }
        Pos = SetNuVec(0.0f, 0.0f, -JeepCamDist);
        NuVecRotateX(&Pos, &Pos, JeepCamAngX);
        NuVecRotateY(&Pos, &Pos, JeepCamAngY);
        NuVecAdd(&Pos, &Pos, &Obj);
        if (Snap == 0) {
            struct nuvec_s Delta;

            NuVecSub(&Delta, &JeepCamObjTween, &Obj);
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
        OldYaw = JeepCamYaw;
        DoCamMtx(Cam, &Obj, &Pos);
        NuVecSub(&JeepCamVel, &Pos, &OldPos);
        JeepCamYawVel = Rationalise360f(JeepCamYaw - OldYaw);
        JeepCamVelValid = 1;
        JeepCamYaw = JeepCamAngY / 182.0444489f;
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
    JeepCamYaw = NuAtani(Dir.x * 256.0f, Dir.z * 256.0f) / 182.0444489f;
    JeepCamPitch = NuAtani(-Dir.y * 256.0f, Dist * 256.0f) / 182.0444489f;
    NuMtxSetRotationX(&Cam->m, JeepCamPitch * 182.0444489f);
    NuMtxRotateY(&Cam->m, JeepCamYaw * 182.0444489f);
    NuMtxTranslate(&Cam->m, &JeepvPos);
    Cam->pos = JeepvPos;
    Cam->xrot = JeepCamPitch * 182.0444489f;
    Cam->yrot = JeepCamYaw * 182.0444489f;
    if (WesternCountdown <= 0.1f) {
        SmokeyCam = 0x14;
    }
    if (best_cRPos != 0) {
        pVIS = Rail[best_cRPos->iRAIL].pCAM;
        iVIS = best_cRPos->iALONG;
    }
}


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
    for (i = 0; i < Jeep->Draw.numjoints; i++) {
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
    struct nuquat_s Quat;
    struct nuvec_s Pos;
    struct nuvec_s Delta;
    struct nuvec_s Pos0;
    struct nuvec_s Pos1;
    f32 Dist;
    f32 Frame;
    f32 Blend;
    s32 Index;
    s32 Frame0;
    s32 Frame1;
    s32 i;

    Dist = Jeep->Position.z - player->obj.pos.z;
    if (Dist > 100.0f || Dist < -10.0f) {
        Jeep->DrawOn = 0;
        Jeep->TrailOn = 0;
    } else if (Dist > 20.0f) {
        Jeep->TrailOn = 0;
        Jeep->DrawOn = 1;
    } else {
        Jeep->DrawOn = 1;
        Jeep->TrailOn = 1;
    }
    Frame = Jeep->PlayBackTime * 60.0f;
    Index = Jeep - EnemyJeep;
    Frame0 = Frame;
    Blend = Frame - Frame0;
    Frame1 = Frame0 + 1;
    if (Frame0 < 0) {
        Frame1 = 0;
        Blend = 0.0f;
        Frame0 = 0;
    } else if (Frame1 >= BuggyDataMax[Index]) {
        Frame1 = BuggyDataMax[Index] - 1;
        Blend = 0.0f;
        Frame0 = Frame1;
    }
    Pos0.x = BuggyData[Index][Frame0].PosX * 0.0078125f;
    Pos0.y = BuggyData[Index][Frame0].PosY * 6.103515625e-05f;
    Pos0.z = BuggyData[Index][Frame0].PosZ / 80.0f + 380.0f;
    Pos1.x = BuggyData[Index][Frame1].PosX * 0.0078125f;
    Pos1.y = BuggyData[Index][Frame1].PosY * 6.103515625e-05f;
    Pos1.z = BuggyData[Index][Frame1].PosZ / 80.0f + 380.0f;
    NuVecSub(&Delta, &Pos1, &Pos0);
    NuVecScale(&Delta, &Delta, Blend);
    NuVecAdd(&Pos, &Pos0, &Delta);
    Jeep->Position = Pos;
    if (Jeep->DrawOn != 0) {
        short aWheel;
        short aDiff;
        short aDelta;
        short aRot;
        u16 aSteer;

        NuQuatSlerp(&Quat, &BuggyData[Index][Frame0].Quat,
                    &BuggyData[Index][Frame1].Quat, Blend);
        NuQuatToMtx(&Quat, &mTEMP);
        {
            struct nuvec_s Scale = { 0.75f, 0.75f, 0.75f };

            NuMtxScale(&mTEMP, &Scale);
        }
        NuMtxTranslate(&mTEMP, &Pos);
        aDiff = BuggyData[Index][Frame1].aWheelAng -
                BuggyData[Index][Frame0].aWheelAng;
        aDelta = aDiff * Blend;
        aWheel = BuggyData[Index][Frame0].aWheelAng + aDelta;
        aRot = Jeep->PlayBackTime * 196608.0f;
        aSteer = aWheel * 9 / 8;
        Jeep->Joints[0].rx = (u16)aRot * 9.587380191e-05f;
        Jeep->Joints[1].rx = (u16)-aRot * 9.587380191e-05f;
        Jeep->Joints[2].rx = (u16)aRot * 9.587380191e-05f;
        Jeep->Joints[3].rx = (u16)-aRot * 9.587380191e-05f;
        Jeep->Joints[0].rz = (u16)-aSteer * 9.587380191e-05f;
        Jeep->Joints[1].rz = aSteer * 9.587380191e-05f;
        for (i = 0; i < 4; i++) {
            f32 Height0 = BuggyData[Index][Frame0].WheelHeight[i] * 0.00390625f;
            f32 Height1 = BuggyData[Index][Frame1].WheelHeight[i] * 0.00390625f;

            Jeep->Joints[i].tx = Height0 + Blend * (Height1 - Height0);
        }
        MyDrawModelNew(&Jeep->Draw, &mTEMP, Jeep->Locators);
        Jeep->LocatorValidFrame = JeepFrame;
    }
}


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

    LoadScreen(LoadingAiPathsText);
    TerrainSetCur(superbuffer_ptr);
    terraininit(Level, &superbuffer_ptr, superbuffer_end, 0, LevelFileName,
                world_scene[0], 0);
    superbuffer_ptr = (void *)(((u32)superbuffer_ptr + 0xF) & 0xFFFFFFF0);
    for (i = 0; i < 4; i++) {
        BuggyData[i] = superbuffer_ptr;
        superbuffer_ptr = LoadBuggyData(superbuffer_ptr, i);
    }
    superbuffer_ptr = (void *)(((u32)superbuffer_ptr + 0xF) & 0xFFFFFFF0);
}


s32 CurrentWesternPosition(void) {
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
        return LastWesternPosition;
    }
    LastWesternPosition = Position;
    return Position;
}


void NewSetTrailPos(struct jeeptrail_s *Trail, s32 TrailId, s32 Pntr,
                    struct SIMWHEEL *Wheel, struct nuvec_s *Pos, s32 Air,
                    s32 Flags, s32 SurfaceType, f32 Scale) {
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
        return ActionIntroText;
    case 1:
        return ActionTiredText;
    case 2:
        return ActionFrontHurlText;
    case 3:
        return ActionRunawayText;
    case 4:
        return ActionGetAngryText;
    case 5:
        return ActionDeadText;
    default:
        return ActionNoneText;
    }
}


void DrawFireBossDebugStuff(void) {
    NuFntSet(app_fnt);
    NuFntSetPen(0x7F7F7F7F);
    NuFntScale(16, 16);
    NuFntPrintEx(0x80, SHEIGHT * 3 / 2, 0, BossActionFmt, FireBossActionName(),
                 FireBoss.ActionTimer, FireBoss.Speed);
    NuFntPrintEx(0x80, SHEIGHT * 2, 0, BossPosFmt, FireBoss.MainSpline.Act,
                 FireBoss.MainSpline.Inc);
    NuFntPrintEx(0x80, SHEIGHT * 5 / 2, 0, BossRelFmt,
                 FireBoss.Pos.z - player->obj.pos.z, RelTemp.x, RelTemp.y,
                 RelTemp.z);
    if (WallOfFireOn != 0) {
        if (WallOfFireAttatched != 0) {
            NuFntPrintEx(0x80, SHEIGHT * 3, 0, WallAttachedText);
        } else {
            NuFntPrintEx(0x80, SHEIGHT * 3, 0, WallFreeText);
        }
    }
    if (FireBossHoldPlayer != 0) {
        NuFntPrintEx(0x80, SHEIGHT * 7 / 2, 0, HoldPlayerText);
    }
    NuFntPrintEx(0x80, SHEIGHT * 4, 0, BossFrameFmt,
                 FireBoss.MainDraw.Anim.anim_time, FireBoss.MainDraw.Anim.action,
                 FireBoss.AngleY);
    NuFntPrintEx(0x80, SHEIGHT * 7, 0, BossCoordFmt, FireBoss.Pos.x,
                 FireBoss.Pos.y, FireBoss.Pos.z);
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
    if (ObjTab[89].special != 0) {
        struct nuspecial_s *special = ObjTab[89].special;

        Balloon->Seen = NuRndrGScnObj(
            ObjTab[89].scene->gobjs[special->instance->objid], &mTEMP);
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


void FireBossWaterFire(s32 WaterOn) {
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

    j = 0;
    fade = 0.0f;
    step = 1.0f / (f32)(count + 1);
    fade += step;
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
    struct nuvec_s TrailPos[2];
    s32 TrailType[2];
    struct jeeptrail_s *Trail;
    s32 TrailOn;
    f32 Depth;

    TrailActive[TrailIndx] = 3;
    Trail = JeepTrail[TrailIndx];
    TrailOn = 0;
    if (Wheel->Platform == -1) {
        struct nuvec_s Up = { 0.0f, 1.0f, 0.0f };
        struct nuvec_s Normal;
        struct nuvec_s Line;
        s32 k;

        NuVecSub(&Line, &Wheel->Position, &Wheel->OldPosition);
        NuVecCross(&Normal, &Line, &Up);
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
        s32 Ang;

        if (TrailAir[TrailIndx] != 0) {
            TrailAir[TrailIndx] = 0;
            TrailPntr[TrailIndx] = (TrailPntr[TrailIndx] + 1) & 0x1F;
            Ang = NewFindTrailAng(&TrailPos[0], &TrailPos[1]);
            NewSetTrailPos(Trail, TrailIndx, TrailPntr[TrailIndx], Wheel, TrailPos,
                           1, Ang, Wheel->SurfaceType, 1.0f);
            TrailPntr[TrailIndx] = (TrailPntr[TrailIndx] + 1) & 0x1F;
            NewSetTrailPos(Trail, TrailIndx, TrailPntr[TrailIndx], Wheel, TrailPos,
                           1, Ang, Wheel->SurfaceType, 1.0f);
        } else {
            struct jeeptrail_s *Last;
            f32 dx;
            f32 dz;

            Last = &Trail[(TrailPntr[TrailIndx] - 1) & 0x1F];
            dx = (TrailPos[0].x + TrailPos[1].x) * 0.5f -
                 (Last->pos1.x + (Last->pos2.x - Last->pos1.x) * 0.5f);
            dz = (TrailPos[0].z + TrailPos[1].z) * 0.5f -
                 (Last->pos1.z + (Last->pos2.z - Last->pos1.z) * 0.5f);
            Ang = NewFindTrailAng(&TrailPos[0], &TrailPos[1]);
            {
                s32 OldAng = NewFindTrailAng(&Last->pos1, &Last->pos2);
                s32 DeltaAng;

                DeltaAng = (u16)(Ang - OldAng + 0x8000) - 0x8000;

                if (dx * dx + dz * dz > 0.04000000283f || abs(DeltaAng) > 0x1555) {
                    TrailPntr[TrailIndx] = (TrailPntr[TrailIndx] + 1) & 0x1F;
                }
            }
            NewSetTrailPos(Trail, TrailIndx, TrailPntr[TrailIndx], Wheel, TrailPos,
                           0, Ang, Wheel->SurfaceType, 1.0f);
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


void JonnySteam(struct fireboss_s *fb) {
    struct nuvec_s pos;

    if (fb->BeenHitThisPassTimer < 3.0f) {
        pos = fb->Pos;
        pos.y += 1.5f;
        AddVariableShotDebrisEffect(GDeb[162].i, &pos, 1, 0, 0);
    }
}


void JonnyBossSmashEffect(struct fireboss_s *fb) {
    u8 *p;
    s32 i;

    DrawCross(&fb->Pos, 1.0f, 0xFF00);
    p = (u8 *)fb + 0xB0;
    for (i = 0; i < 10; i++) {
        DrawCross((struct nuvec_s *)p, 1.0f, 0xFF);
        p += 0x40;
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
