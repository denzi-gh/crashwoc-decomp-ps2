#ifndef GAME_VEHICLE_H
#define GAME_VEHICLE_H

#include "nu_types.h"
#include "game/gametypes.h"

/* Player glider state: GC DWARF names, with PS2-verified member offsets. */
struct GLIDERSTRUCT {
    struct creature_s *Cre;             /* 0x000 */
    struct nuvec_s Vel;                 /* 0x004 */
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

/* WeatherBoss state block (far). */
struct weatherboss_s {
    s32 active;                /* 0x000 */
    s32 unleashed;             /* 0x004 */
    f32 firetimer;             /* 0x008 */
    s32 fireside;              /* 0x00C */
    struct MYDRAW maindraw;    /* 0x010 */
    struct MYDRAW bonesdraw;   /* 0x0F0 */
    struct numtx_s locators[16]; /* 0x1D0 */
    struct nuvec_s pos;        /* 0x5D0 */
    f32 destposy;              /* 0x5DC */
    f32 yaw;                   /* 0x5E0 */
    f32 baseyaw;               /* 0x5E4 */
    f32 destyaw;               /* 0x5E8 */
    s32 hitpoints[4];          /* 0x5EC */
    s32 seen;                  /* 0x5FC */
    f32 dist;                  /* 0x600 */
    f32 destdist;              /* 0x604 */
    s32 action;                /* 0x608 */
    s32 oldaction;             /* 0x60C */
    s32 nextaction;            /* 0x610 */
    f32 actiontimer;           /* 0x614 */
    s32 lastnonseekaction;     /* 0x618 */
    s32 dead;                  /* 0x61C */
    struct nugspline_s *spline; /* 0x620 */
    f32 splinecur;             /* 0x624 */
    f32 splinenex;             /* 0x628 */
    f32 splineact;             /* 0x62C */
    f32 splineinc;             /* 0x630 */
    struct nuvec_s splinecurpos; /* 0x634 */
    struct nuvec_s splinenexpos; /* 0x640 */
    f32 splinelookahead;       /* 0x64C */
    f32 holdframetimer;        /* 0x650 */
    f32 chestsoundtimer;       /* 0x654 */
    s32 hitsoundframe;         /* 0x658 */
};

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

/* JeepRock array, stride 0x3D4, 6 entries. */
struct jeeprock_s {
    struct nuvec_s pos;        /* 0x000 */
    struct nuvec_s vel;        /* 0x00C */
    s32 exists;                /* 0x018 */
    s32 seen;                  /* 0x01C */
    char pad_020[0x024 - 0x020];
    s32 explode;               /* 0x024 set when something hits the rock */
    char pad_028[0x02C - 0x028];
    f32 life;                  /* 0x02C */
    struct NEWBUGGY atlas;     /* 0x030 the rock rolls as an atlasphere */
    s32 held;                  /* 0x3B4 */
    s32 owner;                 /* 0x3B8 */
    f32 flametimer;            /* 0x3BC */
    struct nuvec_s scale;      /* 0x3C0 */
    s32 fireblob;              /* 0x3CC */
    s32 smashme;               /* 0x3D0 */
};

struct plane_s {
    struct nuvec_s pos;        /* 0x00 */
    struct nuvec_s vel;        /* 0x0C */
    f32 actiontimer;           /* 0x18 */
    s32 actionstatus;          /* 0x1C */
    s32 active;                /* 0x20 */
    struct MYDRAW draw;        /* 0x24 */
};

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
    s32 rammed;                /* 0x550 */
};

/* GunBoatList: stride 0x134, 4 entries. */
struct gunboat_s {
    s32 active;                /* 0x000 */
    s32 character;             /* 0x004 -1 if the model failed to load */
    struct MYDRAW draw;        /* 0x008 */
    struct nuvec_s pos;        /* 0x0E8 */
    f32 yaw;                   /* 0x0F4 */
    f32 firepitch;             /* 0x0F8 */
    f32 fireyaw;               /* 0x0FC */
    f32 tiltx;                 /* 0x100 */
    f32 tiltz;                 /* 0x104 */
    f32 destyaw;               /* 0x108 */
    f32 desttiltx;             /* 0x10C */
    f32 desttiltz;             /* 0x110 */
    s32 fireframe;             /* 0x114 */
    s32 seen;                  /* 0x118 */
    s32 action;                /* 0x11C */
    s32 lastaction;            /* 0x120 */
    f32 firepitchmom;          /* 0x124 */
    f32 fireyawmom;            /* 0x128 */
    f32 bursttimer;            /* 0x12C */
    f32 sunktimer;             /* 0x130 */
};

/* BigGunList: stride 0x154, 12 entries. */
struct biggun_s {
    s32 active;                /* 0x000 */
    struct MYDRAW draw;        /* 0x004 */
    struct nuvec_s pos;        /* 0x0E4 */
    s32 type;                  /* 0x0F0 */
    s32 hitpoints;             /* 0x0F4 */
    f32 yaw;                   /* 0x0F8 */
    f32 firepitch;             /* 0x0FC */
    f32 fireyaw;               /* 0x100 */
    f32 tiltx;                 /* 0x104 */
    f32 tiltz;                 /* 0x108 */
    f32 destyaw;               /* 0x10C */
    f32 desttiltx;             /* 0x110 */
    f32 desttiltz;             /* 0x114 */
    s32 fireframe;             /* 0x118 */
    s32 seen;                  /* 0x11C */
    s32 action;                /* 0x120 */
    s32 lastaction;            /* 0x124 */
    f32 firepitchmom;          /* 0x128 */
    f32 fireyawmom;            /* 0x12C */
    f32 bursttimer;            /* 0x130 */
    s32 explosioneffect;       /* 0x134 */
    f32 firepitchmin;          /* 0x138 */
    f32 firepitchmax;          /* 0x13C */
    f32 fireyawmain;           /* 0x140 */
    f32 fireyawdeviation;      /* 0x144 */
    struct nuvec_s terrpos;    /* 0x148 */
};

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

struct splinepath_s {
    struct nugspline_s *spl;   /* 0x00 */
    f32 ratio;                 /* 0x04 */
    f32 nextratio;             /* 0x08 */
    char pad_0C[0x10 - 0x0C];
    f32 speed;                 /* 0x10 */
    struct nuvec_s pos;        /* 0x14 */
    struct nuvec_s lastpos;    /* 0x20 */
    s32 finished;              /* 0x2C */
};

void DrawExtraCreatures(void);
void DrawVehicleTrail(void);
void VehicleSetup(void);

#endif /* GAME_VEHICLE_H */
