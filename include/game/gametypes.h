#ifndef GAME_GAMETYPES_H
#define GAME_GAMETYPES_H

#include "nu_types.h"
#include "numath/numathtypes.h"
#include "nu3d/nugscn.h"

struct CharacterModel;
struct NUJOINTANIM_s;
struct creatcmd_s;
struct creature_s;
struct pchase_s;

struct hub_s {
    u8 flags;                           /* 0x00 */
    u8 crystals;                        /* 0x01 */
    char pad1;                          /* 0x02 */
    char pad2;                          /* 0x03 */
}; /* 0x04 */

struct time_s {
    char name[4];                       /* 0x00 */
    u32 itime;                          /* 0x04 */
}; /* 0x08 */

struct level_s {
    u16 flags;                          /* 0x00 */
    char pad1;                          /* 0x02 */
    char pad2;                          /* 0x03 */
    struct time_s time[3];              /* 0x04 */
}; /* 0x1C */

struct game_s {
    char name[9];                       /* 0x000 */
    u8 vibration;                       /* 0x009 */
    u8 surround;                        /* 0x00A */
    u8 sfx_volume;                      /* 0x00B */
    u8 music_volume;                    /* 0x00C */
    signed char screen_x;               /* 0x00D */
    signed char screen_y;               /* 0x00E */
    u8 language;                        /* 0x00F */
    struct hub_s hub[6];                /* 0x010 */
    struct level_s level[35];           /* 0x028 */
    u8 lives;                           /* 0x3FC */
    u8 wumpas;                          /* 0x3FD */
    u8 mask;                            /* 0x3FE */
    u8 percent;                         /* 0x3FF */
    u8 crystals;                        /* 0x400 */
    u8 relics;                          /* 0x401 */
    u8 crate_gems;                      /* 0x402 */
    u8 bonus_gems;                      /* 0x403 */
    u8 gems;                            /* 0x404 */
    u8 gembits;                         /* 0x405 */
    u8 powerbits;                       /* 0x406 */
    u8 empty;                           /* 0x407 */
    u32 cutbits;                        /* 0x408 */
}; /* 0x40C */

struct ldata_s {
    char *path;                         /* 0x00 */
    u8 *clist;                          /* 0x04 */
    struct pchase_s *pChase;            /* 0x08 */
    u32 time[3];                        /* 0x0C */
    short music[2];                     /* 0x18 */
    void *pSFX;                         /* 0x1C */
    short nSFX;                         /* 0x20 */
    char pad1;                          /* 0x22 */
    signed char hub;                    /* 0x23 */
    u16 flags;                          /* 0x24 */
    short character;                    /* 0x26 */
    short vehicle;                      /* 0x28 */
    u16 farclip;                        /* 0x2A */
    struct nuvec_s vSTART;              /* 0x2C */
    struct nuvec_s vBONUS;              /* 0x38 */
    float fognear;                      /* 0x44 */
    float fogfar;                       /* 0x48 */
    u8 fogr;                            /* 0x4C */
    u8 fogg;                            /* 0x4D */
    u8 fogb;                            /* 0x4E */
    u8 foga;                            /* 0x4F */
    u8 hazer;                           /* 0x50 */
    u8 hazeg;                           /* 0x51 */
    u8 hazeb;                           /* 0x52 */
    u8 hazea;                           /* 0x53 */
}; /* 0x54 */

struct remember_s {
    signed char x;                      /* 0x00 */
    signed char y;                      /* 0x01 */
}; /* 0x02 */

struct cursor_s {
    struct remember_s remember[48];     /* 0x00 */
    u32 menu_frame;                     /* 0x60 */
    u32 item_frame;                     /* 0x64 */
    signed char x;                      /* 0x68 */
    signed char y;                      /* 0x69 */
    signed char x_min;                  /* 0x6A */
    signed char y_min;                  /* 0x6B */
    signed char x_max;                  /* 0x6C */
    signed char y_max;                  /* 0x6D */
    signed char menu;                   /* 0x6E */
    signed char new_menu;               /* 0x6F */
    signed char new_level;              /* 0x70 */
    u8 wait;                            /* 0x71 */
    u8 wait_frames;                     /* 0x72 */
    signed char wait_hack;              /* 0x73 */
    u8 button_lock;                     /* 0x74 */
    char pad1;                          /* 0x75 */
    char pad2;                          /* 0x76 */
    char pad3;                          /* 0x77 */
}; /* 0x78 */

struct gtimer_s {
    u32 frame;                          /* 0x00 */
    u32 itime;                          /* 0x04 */
    u32 isec;                           /* 0x08 */
    float ftime;                        /* 0x0C */
    float fsec;                         /* 0x10 */
}; /* 0x14 */

struct panelcount_s {
    short count;                        /* 0x00 */
    short draw;                         /* 0x02 */
    signed char frame;                  /* 0x04 */
    signed char wait;                   /* 0x05 */
    u8 delay;                           /* 0x06 */
    u8 item;                            /* 0x07 */
}; /* 0x08 */

struct objtab_s {
    struct nuhspecial_s obj;            /* 0x00 */
    struct nugscn_s **scene;            /* 0x08 */
    signed char visible;                /* 0x0C */
    char font3d_letter;                 /* 0x0D */
    char pad1;                          /* 0x0E */
    char pad2;                          /* 0x0F */
    char *name;                         /* 0x10 */
    u64 levbits;                        /* 0x18 */
}; /* 0x20 */

struct spltab_s {
    struct nugspline_s *spl;            /* 0x00 */
    char *name;                         /* 0x04 */
    short min;                          /* 0x08 */
    short max;                          /* 0x0A */
    u64 levbits;                        /* 0x10 */
}; /* 0x18 */

struct gdeb_s {
    s32 i;                              /* 0x00 */
    char *name;                         /* 0x04 */
    u64 levbits;                        /* 0x08 */
}; /* 0x10 */

struct rail_s {
    struct nugspline_s *pINPLAT;        /* 0x00 */
    struct nugspline_s *pINCAM;         /* 0x04 */
    struct nugspline_s *pLEFT;          /* 0x08 */
    struct nugspline_s *pCAM;           /* 0x0C */
    struct nugspline_s *pRIGHT;         /* 0x10 */
    struct nugspline_s *pOUTCAM;        /* 0x14 */
    struct nugspline_s *pOUTPLAT;       /* 0x18 */
    float in_distance;                  /* 0x1C */
    float out_distance;                 /* 0x20 */
    short edges;                        /* 0x24 */
    signed char type;                   /* 0x26 */
    signed char circuit;                /* 0x27 */
}; /* 0x28 */

struct cammtx_s {
    struct numtx_s m;                   /* 0x000 */
    struct numtx_s m0;                  /* 0x040 */
    struct nuvec_s vX;                  /* 0x080 */
    struct nuvec_s vY;                  /* 0x08C */
    struct nuvec_s vZ;                  /* 0x098 */
    struct nuvec_s pos;                 /* 0x0A4 */
    struct nuvec_s oldpos;              /* 0x0B0 */
    struct nuvec_s newpos;              /* 0x0BC */
    struct nuvec_s pos2;                /* 0x0C8 */
    struct nuvec_s seek;                /* 0x0D4 */
    float distance;                     /* 0x0E0 */
    float ahead;                        /* 0x0E4 */
    float judder;                       /* 0x0E8 */
    float blend_time;                   /* 0x0EC */
    float blend_duration;               /* 0x0F0 */
    s32 xrot;                           /* 0x0F4 */
    s32 yrot;                           /* 0x0F8 */
    s32 zrot;                           /* 0x0FC */
    u16 old_xrot;                       /* 0x100 */
    u16 new_xrot;                       /* 0x102 */
    u16 old_yrot;                       /* 0x104 */
    u16 new_yrot;                       /* 0x106 */
    u16 old_zrot;                       /* 0x108 */
    u16 new_zrot;                       /* 0x10A */
    u16 hdg_to_player;                  /* 0x10C */
    short route;                        /* 0x10E */
    short i0;                           /* 0x110 */
    short i1;                           /* 0x112 */
    signed char mode;                   /* 0x114 */
    signed char iRAIL;                  /* 0x115 */
    short iALONG;                       /* 0x116 */
    u16 angle;                          /* 0x118 */
    signed char vertical;               /* 0x11A */
    char pad1;                          /* 0x11B */
}; /* 0x11C */

struct tersurface_s {
    float friction;                     /* 0x00 */
    short gdeb;                         /* 0x04 */
    u16 flags;                          /* 0x06 */
}; /* 0x08 */

struct aitab_s {
    u8 ai_type;                         /* 0x00 */
    signed char status;                 /* 0x01 */
    char pad1;                          /* 0x02 */
    char pad2;                          /* 0x03 */
    char pad3;                          /* 0x04 */
    signed char iRAIL;                  /* 0x05 */
    short iALONG;                       /* 0x06 */
    float fALONG;                       /* 0x08 */
    float time;                         /* 0x0C */
    float delay;                        /* 0x10 */
    struct nuvec_s pos[8];              /* 0x14 */
    struct nuvec_s origin;              /* 0x74 */
}; /* 0x80 */

struct aitype_s {
    short character;                    /* 0x00 */
    short points;                       /* 0x02 */
    struct creatcmd_s *cmd;             /* 0x04 */
    char name[16];                      /* 0x08 */
    float delay;                        /* 0x18 */
}; /* 0x1C */

struct wumpa_s {
    struct nuvec_s pos0;                /* 0x00 */
    struct nuvec_s pos1;                /* 0x0C */
    struct nuvec_s pos;                 /* 0x18 */
    struct nuvec_s mom;                 /* 0x24 */
    float shadow;                       /* 0x30 */
    u16 surface_xrot;                   /* 0x34 */
    u16 surface_zrot;                   /* 0x36 */
    float time;                         /* 0x38 */
    float duration;                     /* 0x3C */
    signed char active;                 /* 0x40 */
    signed char iRAIL;                  /* 0x41 */
    short iALONG;                       /* 0x42 */
    float fALONG;                       /* 0x44 */
    signed char destroy;                /* 0x48 */
    signed char fired;                  /* 0x49 */
    signed char in_range;               /* 0x4A */
    signed char surface_type;           /* 0x4B */
}; /* 0x4C */

struct winfo_s {
    struct numtx_s m;                   /* 0x00 */
    float scale;                        /* 0x40 */
    float dy;                           /* 0x44 */
    u16 angle;                          /* 0x48 */
    u16 spin;                           /* 0x4A */
    char pad1;                          /* 0x4C */
    char pad2;                          /* 0x4D */
    char pad3;                          /* 0x4E */
    char pad4;                          /* 0x4F */
}; /* 0x50 */

/* A wumpa flying to the on-screen counter. */
struct wscr_s {
    struct nuvec_s pos;                 /* 0x00 */
    float timer;                        /* 0x0C */
    float xs;                           /* 0x10 */
    float ys;                           /* 0x14 */
    float scale;                        /* 0x18 */
    signed char bonus;                  /* 0x1C */
    char pad1;                          /* 0x1D */
    char pad2;                          /* 0x1E */
    char pad3;                          /* 0x1F */
}; /* 0x20 */

struct tubinfo_s {
    struct creature_s *c;               /* 0x00 */
    signed char track;                  /* 0x04 */
    signed char count;                  /* 0x05 */
    u8 wait;                            /* 0x06 */
    signed char laps;                   /* 0x07 */
    float time;                         /* 0x08 */
    float duration;                     /* 0x0C */
    float lap_position;                 /* 0x10 */
    signed char place;                  /* 0x14 */
    u8 finished;                        /* 0x15 */
    u8 finishframes;                    /* 0x16 */
    u8 stall;                           /* 0x17 */
    u16 old_xrot;                       /* 0x18 */
    u16 old_yrot;                       /* 0x1A */
    u16 old_zrot;                       /* 0x1C */
    u8 boost;                           /* 0x1E */
    signed char spark;                  /* 0x1F */
}; /* 0x20 */

struct chaseevent_s {
    struct nugspline_s *spl;            /* 0x00 */
    struct nuhspecial_s obj[24];        /* 0x04 */
}; /* 0xC4 */

struct font3dobj_s {
    short i;                            /* 0x00 */
    u8 flags;                           /* 0x02 */
    signed char action;                 /* 0x03 */
    float anim_time;                    /* 0x04 */
    float scale;                        /* 0x08 */
}; /* 0x0C */

struct font3dtab_s {
    s32 ascii;                          /* 0x00 */
    struct nuhspecial_s obj;            /* 0x04 */
    char *name;                         /* 0x0C */
}; /* 0x10 */

struct crate_s {
    s32 id;                             /* 0x00 */
    signed char type[4];                /* 0x04 */
    struct nuvec_s pos;                 /* 0x08 */
    struct crate_s *linked;             /* 0x14 */
    struct crate_s *trigger;            /* 0x18 */
    u16 orientation;                    /* 0x1C */
    short offx;                         /* 0x1E */
    short offy;                         /* 0x20 */
    short offz;                         /* 0x22 */
    short ccindex;                      /* 0x24 */
    signed char draw;                   /* 0x26 */
    signed char cpad1;                  /* 0x27 */
}; /* 0x28 */

struct crateeditor_s {
    struct nuhspecial_s obj;            /* 0x00 */
    s32 id;                             /* 0x08 */
    char *name;                         /* 0x0C */
    s32 character;                      /* 0x10 */
}; /* 0x14 */

struct cratecube_s {
    struct crate_s *model;              /* 0x00 */
    struct nuvec_s pos0;                /* 0x04 */
    struct nuvec_s pos;                 /* 0x10 */
    float oldy;                         /* 0x1C */
    float shadow;                       /* 0x20 */
    float mom;                          /* 0x24 */
    float timer;                        /* 0x28 */
    float duration;                     /* 0x2C */
    signed char on;                     /* 0x30 */
    signed char iRAIL;                  /* 0x31 */
    short iALONG;                       /* 0x32 */
    float fALONG;                       /* 0x34 */
    u16 flags;                          /* 0x38 */
    signed char type1;                  /* 0x3A */
    signed char type2;                  /* 0x3B */
    signed char type3;                  /* 0x3C */
    signed char type4;                  /* 0x3D */
    signed char newtype;                /* 0x3E */
    signed char subtype;                /* 0x3F */
    signed char i;                      /* 0x40 */
    signed char metal_count;            /* 0x41 */
    signed char appeared;               /* 0x42 */
    signed char in_range;               /* 0x43 */
    short dx;                           /* 0x44 */
    short dy;                           /* 0x46 */
    short dz;                           /* 0x48 */
    short iU;                           /* 0x4A */
    short iD;                           /* 0x4C */
    short iN;                           /* 0x4E */
    short iS;                           /* 0x50 */
    short iE;                           /* 0x52 */
    short iW;                           /* 0x54 */
    short trigger;                      /* 0x56 */
    signed char counter;                /* 0x58 */
    signed char anim_cycle;             /* 0x59 */
    short index;                        /* 0x5A */
    float anim_time;                    /* 0x5C */
    float anim_duration;                /* 0x60 */
    float anim_speed;                   /* 0x64 */
    u16 xrot0;                          /* 0x68 */
    u16 zrot0;                          /* 0x6A */
    u16 xrot;                           /* 0x6C */
    u16 zrot;                           /* 0x6E */
    u16 surface_xrot;                   /* 0x70 */
    u16 surface_zrot;                   /* 0x72 */
    short character;                    /* 0x74 */
    short action;                       /* 0x76 */
    struct nuvec_s colbox[2];           /* 0x78 */
}; /* 0x90 */

struct crategroup_s {
    struct nuvec_s origin;              /* 0x00 */
    float radius;                       /* 0x0C */
    short iCrate;                       /* 0x10 */
    short nCrates;                      /* 0x12 */
    u16 angle;                          /* 0x14 */
    signed char pad1;                   /* 0x16 */
    signed char pad2;                   /* 0x17 */
    struct nuvec_s minclip;             /* 0x18 */
    struct nuvec_s maxclip;             /* 0x24 */
}; /* 0x30 */

struct VEHICLE {
    struct nuvec_s ActualWheelPosition[4]; /* 0x00 */
    struct nuvec_s OldWheelPosition[4]; /* 0x30 */
    s32 BigSpin[4];                     /* 0x60 */
    struct nuvec_s ActualPosition;      /* 0x70 */
    struct nuvec_s Resolved;            /* 0x7C */
    struct nuvec_s Velocity;            /* 0x88 */
    struct nuvec_s WheelAxis[3];        /* 0x94 */
    float FrontWheelSpeedAdj;           /* 0xB8 */
    short aTargetAngle;                 /* 0xBC */
    short aTarSurfRotX;                 /* 0xBE */
    short aTarSurfRotZ;                 /* 0xC0 */
    short aActualAngle;                 /* 0xC2 */
    short aActSurfRotX;                 /* 0xC4 */
    short aActSurfRotZ;                 /* 0xC6 */
    short ActFrontRotX;                 /* 0xC8 */
    short ActRearRotX;                  /* 0xCA */
    short TarFrontRotX;                 /* 0xCC */
    short TarRearRotX;                  /* 0xCE */
    s32 AnyOnGroundBits;                /* 0xD0 */
    s32 AllOnGroundBits;                /* 0xD4 */
    s32 AllTouchingGroundBits;          /* 0xD8 */
    s32 AnyTouchingGroundBits;          /* 0xDC */
    struct nuvec_s AirNormal;           /* 0xE0 */
    struct nuvec_s SurfaceNormal;       /* 0xEC */
    short *TerrHandle;                  /* 0xF8 */
    s32 FrontWheelGroundBits;           /* 0xFC */
}; /* 0x100 */

struct anim_s {
    float anim_time;                    /* 0x00 */
    float blend_src_time;               /* 0x04 */
    float blend_dst_time;               /* 0x08 */
    short action;                       /* 0x0C */
    short oldaction;                    /* 0x0E */
    short newaction;                    /* 0x10 */
    short blend_src_action;             /* 0x12 */
    short blend_dst_action;             /* 0x14 */
    short blend_frame;                  /* 0x16 */
    short blend_frames;                 /* 0x18 */
    signed char blend;                  /* 0x1A */
    u8 flags;                           /* 0x1B */
}; /* 0x1C */

/* Vehicle toggle point: switches the player into/out of a vehicle. */
struct vtog_s {
    struct nugspline_s *pTRIGGER;       /* 0x00 */
    struct nugspline_s *pCAM;           /* 0x04 */
    struct nugspline_s *pLOOK;          /* 0x08 */
    struct numtx_s m;                   /* 0x0C */
    struct nuvec_s pos;                 /* 0x4C */
    struct anim_s anim;                 /* 0x58 */
    signed char type;                   /* 0x74 */
    signed char iRAIL;                  /* 0x75 */
    short iALONG;                       /* 0x76 */
    float fALONG;                       /* 0x78 */
    u16 xrot;                           /* 0x7C */
    u16 yrot;                           /* 0x7E */
    u16 zrot;                           /* 0x80 */
    char pad1;                          /* 0x82 */
    char pad2;                          /* 0x83 */
}; /* 0x84 */

struct pdir_s {
    s32 Index;                          /* 0x00 */
    struct nuvec_s Direction;           /* 0x04 */
    struct nucolour3_s Colour;          /* 0x10 */
    float Distance;                     /* 0x1C */
}; /* 0x20 */

struct glowlight_s {
    struct nucolour3_s HighColour;      /* 0x00 */
    struct nucolour3_s LowColour;       /* 0x0C */
    struct nucolour3_s CurColour;       /* 0x18 */
    struct nucolour3_s ColDiff;         /* 0x24 */
    u16 Step;                           /* 0x30 */
    u16 CurAngle;                       /* 0x32 */
}; /* 0x34 */

/* One placed level light (Lights[]). */
struct lights_s {
    s32 type;                           /* 0x00 */
    struct nuvec_s pos;                 /* 0x04 */
    struct nuvec_s radius_pos;          /* 0x10 */
    float radius;                       /* 0x1C */
    u8 r;                               /* 0x20 */
    u8 g;                               /* 0x21 */
    u8 b;                               /* 0x22 */
    u8 glow;                            /* 0x23 */
    struct nucolour3_s colour;          /* 0x24 */
    struct nuvec_s direction;           /* 0x30 */
    u8 globalflag;                      /* 0x3C */
    u8 blendtype;                       /* 0x3D */
    u8 brightness;                      /* 0x3E */
    struct glowlight_s *glowlight;      /* 0x40 */
    u8 pad2;                            /* 0x44 */
}; /* 0x48 */

struct Nearest_Light_s {
    s32 AmbIndex;                       /* 0x00 */
    struct nuvec_s AmbCol;              /* 0x04 */
    float ambientdist;                  /* 0x10 */
    s32 CurLoopIndex;                   /* 0x14 */
    struct pdir_s dir1;                 /* 0x18 */
    struct pdir_s dir2;                 /* 0x38 */
    struct pdir_s dir3;                 /* 0x58 */
    struct pdir_s *pDir1st;             /* 0x78 */
    struct pdir_s *pDir2nd;             /* 0x7C */
    struct pdir_s *pDir3rd;             /* 0x80 */
    s32 glbambindex;                    /* 0x84 */
    s32 negativeindex;                  /* 0x88 */
    float negativedist;                 /* 0x8C */
    struct pdir_s glbdirectional;       /* 0x90 */
}; /* 0xB0 */

struct RPos_s {
    signed char iRAIL;                  /* 0x00 */
    signed char vertical;               /* 0x01 */
    short iALONG;                       /* 0x02 */
    short i1;                           /* 0x04 */
    short i2;                           /* 0x06 */
    float fALONG;                       /* 0x08 */
    float fACROSS;                      /* 0x0C */
    u16 angle;                          /* 0x10 */
    u16 cam_angle;                      /* 0x12 */
    u8 mode;                            /* 0x14 */
    char pad1;                          /* 0x15 */
    char pad2;                          /* 0x16 */
    char pad3;                          /* 0x17 */
    struct nuvec_s pos;                 /* 0x18 */
}; /* 0x24 */

struct mask_s {
    struct numtx_s mM;                  /* 0x000 */
    struct numtx_s mS;                  /* 0x040 */
    struct nuvec_s pos;                 /* 0x080 */
    struct nuvec_s newpos;              /* 0x08C */
    struct Nearest_Light_s lights;      /* 0x098 */
    struct anim_s anim;                 /* 0x148 */
    float scale;                        /* 0x164 */
    float shadow;                       /* 0x168 */
    short character;                    /* 0x16C */
    short active;                       /* 0x16E */
    short sfx;                          /* 0x170 */
    char pad1;                          /* 0x172 */
    char pad2;                          /* 0x173 */
    u16 xrot;                           /* 0x174 */
    u16 yrot;                           /* 0x176 */
    u16 angle;                          /* 0x178 */
    u16 surface_xrot;                   /* 0x17A */
    u16 surface_zrot;                   /* 0x17C */
    u16 wibble_ang[3];                  /* 0x17E */
    float idle_time;                    /* 0x184 */
    float idle_duration;                /* 0x188 */
    signed char idle_mode;              /* 0x18C */
    signed char reflect;                /* 0x18D */
    u8 offset_frame;                    /* 0x18E */
    signed char hold;                   /* 0x18F */
}; /* 0x190 */

struct chase_s {
    struct nugspline_s *spl_START;      /* 0x0000 */
    float time;                         /* 0x0004 */
    float duration;                     /* 0x0008 */
    struct nugspline_s *spl_CHASER[6];  /* 0x000C */
    struct nuvec_s pos[6];              /* 0x0024 */
    struct RPos_s RPos[6];              /* 0x006C */
    struct anim_s anim[6];              /* 0x0144 */
    struct nuhspecial_s obj[6];         /* 0x01EC */
    short character[6];                 /* 0x021C */
    short action[6];                    /* 0x0228 */
    u16 xrot[6];                        /* 0x0234 */
    u16 yrot[6];                        /* 0x0240 */
    u16 zrot[6];                        /* 0x024C */
    struct chaseevent_s event[6][24];   /* 0x0258 */
    struct nugspline_s *spl_MISC[6][4]; /* 0x7098 */
    struct Nearest_Light_s lights[6];   /* 0x70F8 */
    float scale[6];                     /* 0x7518 */
    u8 misc_phase[6];                   /* 0x7530 */
    signed char ok[6];                  /* 0x7536 */
    u8 cuboid;                          /* 0x753C */
    signed char i;                      /* 0x753D */
    signed char i_last;                 /* 0x753E */
    signed char i_next;                 /* 0x753F */
    signed char status;                 /* 0x7540 */
    signed char iRAIL;                  /* 0x7541 */
    short iALONG;                       /* 0x7542 */
    float fALONG;                       /* 0x7544 */
}; /* 0x7548 */

struct MYDRAW {
    struct anim_s Anim;                 /* 0x00 */
    struct CharacterModel *Model;       /* 0x1C */
    s32 Character;                      /* 0x20 */
    s32 NumJoints;                      /* 0x24 */
    struct NUJOINTANIM_s *JointList;    /* 0x28 */
    struct nuvec_s *Position;           /* 0x2C */
    struct Nearest_Light_s Nearest_Light; /* 0x30 */
}; /* 0xE0 */

/* creature_s.Buggy (creature+0x224) points at the vehicle the creature is
 * driving, and the pointee's type depends on the vehicle (as in the GC
 * source, where Buggy is cast to GLIDERSTRUCT / ATLASSTRUCT / JEEPSTRUCT):
 *   glider (vehicle 0x36/0x81/0x8B)  struct glider_s (GC GLIDERSTRUCT)
 *   atlasphere (vehicle 0x53)        struct NEWBUGGY (GC ATLASSTRUCT)
 *   jeep (vehicle 0x63)              struct JEEPSTRUCT (game/jeep.c)
 * Offsets are PS2-verified in unit game/vehicle; gaps are opaque padding. */

/* The player's glider (PlayerGlider). */
struct glider_s {
    struct creature_s *Cre;             /* 0x000 */
    char pad_04[0x10 - 0x04];
    s32 Dead;                           /* 0x010 */
    char pad_14[0x30 - 0x14];
    struct nuvec_s Position;            /* 0x030 */
    char pad_3C[0x6C - 0x3C];
    float RailAngle;                    /* 0x06C */
    float TiltX;                        /* 0x070  pitch (NuMtxRotateX) */
    float TiltZ;                        /* 0x074  roll (NuMtxSetRotationZ) */
    float DestTiltX;                    /* 0x078 */
    float DestTiltZ;                    /* 0x07C */
    float AngleY;                       /* 0x080  heading (NuMtxRotateY) */
    char pad_84[0x98 - 0x84];
    s32 BarrelRoll;                     /* 0x098 */
    char pad_9C[0xA4 - 0x9C];
    s32 TerminalDive;                   /* 0x0A4 */
    char pad_A8[0xAC - 0xA8];
    float FireTimer;                    /* 0x0AC */
    float HitTimer;                     /* 0x0B0 */
    s32 AutoPilot;                      /* 0x0B4 */
    char pad_B8[0xBC - 0xB8];
    s32 HitPoints;                      /* 0x0BC  0..100 */
    char LocatorList[16];               /* 0x0C0 */
    float LocatorTime[16];              /* 0x0D0 */
    s32 InTornado;                      /* 0x110 */
    char pad_114[0x43C - 0x114];
    float NextHitSoundTimer;            /* 0x43C */
}; /* 0x440 */

/* The atlasphere ("ball"): PlayerAtlas and EarthBoss are one each, and every
 * JeepRock embeds one at +0x30 (JeepRock stride 0x3D4, rock fields resume at
 * +0x3B4), which fixes the size at 0x384 (InitAtlas clears exactly that much). */
struct NEWBUGGY {
    struct creature_s *owner;           /* 0x000  owning creature (KillAtlasphere) */
    s32 whacko;                         /* 0x004 */
    s32 type;                           /* 0x008  atlasphere type (InitAtlas) */
    s32 dead;                           /* 0x00C  kill flag (KillAtlasphere) */
    s32 ball_health;                    /* 0x010  hit points (EarthBoss) */
    s32 desthitpoints;                  /* 0x014 */
    s32 hitpointcounter;                /* 0x018 */
    s32 drawcrunch;                     /* 0x01C */
    s32 drawshell;                      /* 0x020 */
    float inhibitcontroltimer;          /* 0x024  ignores the pad while > 0 */
    s32 numattacks;                     /* 0x028 */
    s32 ball_action;                    /* 0x02C  AI action (EarthBoss) */
    s32 lastaction;                     /* 0x030 */
    float actiontimer;                  /* 0x034 */
    float actiontimer2;                 /* 0x038 */
    s32 ball_beenhit;                   /* 0x03C  hit flag (ShootRoksSkyward) */
    s32 cantbehit;                      /* 0x040 */
    float rollspeed;                    /* 0x044 */
    float rollacctimer;                 /* 0x048 */
    struct MYDRAW shell;                /* 0x04C */
    struct MYDRAW crunch;               /* 0x12C */
    struct nuvec_s ball_pos;            /* 0x20C  position (NuMtxTranslate) */
    struct nuvec_s ball_oldpos;         /* 0x218  last safe position (UnembedRayCastAtlas) */
    struct nuvec_s ball_lastpos;        /* 0x224 */
    struct nuvec_s ball_target;         /* 0x230  position the step moves towards */
    struct nuvec_s ball_vel;            /* 0x23C  velocity (<-> obj->mom) */
    struct nuvec_s ball_oldvel;         /* 0x248 */
    struct nuvec_s ball_resolved;       /* 0x254 */
    struct nuvec_s ball_force;          /* 0x260  gravity tilted by the stick */
    float ball_radius;                  /* 0x26C  radius (InitAtlas, TryUnembeddPoint*) */
    float anglex;                       /* 0x270 */
    float angley;                       /* 0x274  heading */
    float anglez;                       /* 0x278 */
    float inputang;                     /* 0x27C  stick direction, camera relative (degrees) */
    float inputmag;                     /* 0x280  stick deflection 0..1 */
    struct nuquat_s rotquat;            /* 0x284  render orientation (NuQuatToMtx) */
    struct nuquat_s rotquat_delta;      /* 0x294  rotation accumulator (NuQuatMul target) */
    struct nuquat_s framequat[4];       /* 0x2A4  recent orientations */
    struct nuquat_s lastquat;           /* 0x2E4 */
    s32 lasthit;                        /* 0x2F4 */
    s32 axis;                           /* 0x2F8 */
    struct nuvec_s lastnormal;          /* 0x2FC  last ground normal */
    struct nuvec_s storelastnormal;     /* 0x308 */
    char pad_314[0x338 - 0x314];
    s32 onground;                       /* 0x338  ground contact bits */
    s32 platformid;                     /* 0x33C  platform rolled on, -1 for none */
    struct nuvec_s platformnormal;      /* 0x340 */
    float shadowy;                      /* 0x34C */
    s32 surfacetype;                    /* 0x350 */
    s32 trailsurfacetype;               /* 0x354 */
    s32 bigdrop;                        /* 0x358 */
    s32 embedded;                       /* 0x35C */
    s32 rock;                           /* 0x360 */
    s32 rocknum;                        /* 0x364 */
    float crunchy;                      /* 0x368 */
    float destcrunchy;                  /* 0x36C */
    s16 *terrhandle;                    /* 0x370 */
    float boosttimer;                   /* 0x374 */
    struct nuvec_s interestpoint;       /* 0x378 */
}; /* 0x384 */

struct newwumpa_s {
    struct nuvec_s pos;                 /* 0x00 */
    struct nuvec_s screen;              /* 0x0C */
    float scale;                        /* 0x18 */
    u8 count;                           /* 0x1C */
    u8 delay;                           /* 0x1D */
    signed char transformed;            /* 0x1E */
    signed char bonus;                  /* 0x1F */
    signed char active;                 /* 0x20 */
    char pad_21[0x24 - 0x21];
}; /* 0x24 */

struct powerdata_s {
    u8 character;                       /* 0x00 */
    u8 font3d_letter;                   /* 0x01 */
    char pad1;                          /* 0x02 */
    char pad2;                          /* 0x03 */
    char *name;                         /* 0x04 */
    char **text;                        /* 0x08 */
}; /* 0x0C */

#endif
