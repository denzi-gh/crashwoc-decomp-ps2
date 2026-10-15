#ifndef GAME_GAMETYPES_H
#define GAME_GAMETYPES_H

#include "nu_types.h"
#include "numath/numathtypes.h"
#include "nu3d/nugscn.h"

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

struct edmenu_s {
    u8 pad00[0x8];                      /* 0x00 */
    void *sel_item;                     /* 0x08 */
    u8 pad0C[0x4];                      /* 0x0C */
    s32 x;                              /* 0x10 */
    s32 y;                              /* 0x14 */
    u8 pad18[0xC];                      /* 0x18 */
    void (*cb)(struct edmenu_s *menu, s32 arg); /* 0x24 */
    u8 pad28[0x8];                      /* 0x28 */
    s32 arg;                            /* 0x30 */
}; /* 0x34 */

#endif
