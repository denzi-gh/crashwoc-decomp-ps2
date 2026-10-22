#ifndef GAME_JEEP_H
#define GAME_JEEP_H

#include "nu_types.h"
#include "game/gametypes.h"

struct SPLINEFOLLOW {
    struct nugspline_s *Spline;     /* 0x00 */
    f32 Cur;                        /* 0x04 */
    f32 Nex;                        /* 0x08 */
    f32 Act;                        /* 0x0C */
    f32 Inc;                        /* 0x10 */
    struct nuvec_s CurPos;          /* 0x14 */
    struct nuvec_s NexPos;          /* 0x20 */
    f32 LookaheadDist;              /* 0x2C */
}; /* 0x30 */

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

#endif /* GAME_JEEP_H */
