#ifndef GAMECODE_CREATURE_H
#define GAMECODE_CREATURE_H

/* PS2 PAL v1.03 (SLES_503.86) layout of the creature/player structures for
 * unit 91 (game/creature, .\creature.c).
 *
 * Every offset and size below is verified against the retail PS2 v1.03
 * assembly of unit 91 (build/pal103/expected_s/091_creature.c.s) unless a
 * comment says otherwise. The field names follow the GameCube decompilation
 * (denzi-gh/crashwoc-decomp-gc, src/gamecode/creature.h) and the alpha-NGC
 * DWARF dump shipped with it, whose creature_s layout (size 0xCE4, ai at
 * 0x18C, m at 0x234, mtxLOCATOR at 0x274, lights at 0xBF4, rumble at 0xCA4)
 * matches the PS2 v1.03 access patterns exactly.
 *
 * Key PS2-verified anchors:
 *   sizeof(struct creature_s) = 0xCE4  (CloseCreatures loop stride,
 *                                       Character array: 9 * 0xCE4 = 0x7404)
 *   sizeof(struct obj_s)      = 0x188  (obj at creature+0x4; radius = obj+0xE4,
 *                                       ground = obj+0x16E, touch = obj+0x187)
 *   sizeof(struct CharacterModel) = 0x988 (CloseCreatures CModel loop stride,
 *                                          ModelAnimDuration index scaling)
 */

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef unsigned long long u64;
typedef float f32;

/* ------------------------------------------------------------------ */
/* Shared small math / engine types                                    */
/* ------------------------------------------------------------------ */

struct nuvec_s {
    float x;
    float y;
    float z;
}; /* 0xC */

struct nucolour3_s {
    float r;
    float g;
    float b;
}; /* 0xC */

struct numtx_s {
    float _00, _01, _02, _03;
    float _10, _11, _12, _13;
    float _20, _21, _22, _23;
    float _30, _31, _32, _33;
}; /* 0x40 */

struct nuhspecial_s {
    struct nugscn_s *scene;    /* 0x0 */
    struct nuspecial_s *special; /* 0x4 */
}; /* 0x8 */

/* Animation data blob header.  PS2-verified in ModelAnimDuration
 * (time at 0x0). */
struct nuanimdata_s {
    float time;                        /* 0x0 */
    char *node_name;                   /* 0x4 */
    int nchunks;                       /* 0x8 */
    struct nuanimdatachunk_s **chunks; /* 0xC */
}; /* 0x10 */

/* Blended animation state.  PS2-verified in ResetAnimPacket
 * (anim_time 0x0, action 0xC, oldaction 0xE, newaction 0x10, blend 0x1A,
 * flags 0x1B) and EvalModelAnim (blend_src_* 0x4/0x12, blend_dst_* 0x8/0x14,
 * blend_frame 0x16, blend_frames 0x18). */
struct anim_s {
    float anim_time;        /* 0x00 */
    float blend_src_time;   /* 0x04 */
    float blend_dst_time;   /* 0x08 */
    short action;           /* 0x0C */
    short oldaction;        /* 0x0E */
    short newaction;        /* 0x10 */
    short blend_src_action; /* 0x12 */
    short blend_dst_action; /* 0x14 */
    short blend_frame;      /* 0x16 */
    short blend_frames;     /* 0x18 */
    signed char blend;      /* 0x1A */
    unsigned char flags;    /* 0x1B */
}; /* 0x1C */

/* One directional light. */
struct pdir_s {
    int Index;                   /* 0x00 */
    struct nuvec_s Direction;    /* 0x04 */
    struct nucolour3_s Colour;   /* 0x10 */
    float Distance;              /* 0x1C */
}; /* 0x20 */

/* PS2-verified size 0xB0 (creature lights at 0xBF4, rumble at 0xCA4). */
struct Nearest_Light_s {
    int AmbIndex;              /* 0x00 */
    struct nuvec_s AmbCol;     /* 0x04 */
    float ambientdist;         /* 0x10 */
    int CurLoopIndex;          /* 0x14 */
    struct pdir_s dir1;        /* 0x18 */
    struct pdir_s dir2;        /* 0x38 */
    struct pdir_s dir3;        /* 0x58 */
    struct pdir_s *pDir1st;    /* 0x78 */
    struct pdir_s *pDir2nd;    /* 0x7C */
    struct pdir_s *pDir3rd;    /* 0x80 */
    int glbambindex;           /* 0x84 */
    int negativeindex;         /* 0x88 */
    float negativedist;        /* 0x8C */
    struct pdir_s glbdirectional; /* 0x90 */
}; /* 0xB0 */

/* Rail position.  PS2-verified in ResetPlayer (iRAIL obj+0x44, iALONG
 * obj+0x46, angle obj+0x54). */
struct RPos_s {
    signed char iRAIL;      /* 0x00 */
    signed char vertical;   /* 0x01 */
    short iALONG;           /* 0x02 */
    short i1;               /* 0x04 */
    short i2;               /* 0x06 */
    float fALONG;           /* 0x08 */
    float fACROSS;          /* 0x0C */
    unsigned short angle;   /* 0x10 */
    unsigned short cam_angle; /* 0x12 */
    unsigned char mode;     /* 0x14 */
    char pad1;              /* 0x15 */
    char pad2;              /* 0x16 */
    char pad3;              /* 0x17 */
    struct nuvec_s pos;     /* 0x18 */
}; /* 0x24 */

/* Per-character movement tuning table. */
struct MoveInfo {
    float IDLESPEED;            /* 0x00 */
    float TIPTOESPEED;          /* 0x04 */
    float WALKSPEED;            /* 0x08 */
    float RUNSPEED;             /* 0x0C */
    float SPRINTSPEED;          /* 0x10 */
    float SLIDESPEED;           /* 0x14 */
    float CRAWLSPEED;           /* 0x18 */
    float DANGLESPEED;          /* 0x1C */
    float WADESPEED;            /* 0x20 */
    float JUMPHEIGHT;           /* 0x24 */
    float DANGLEGAP;            /* 0x28 */
    short JUMPFRAMES0;          /* 0x2C */
    short JUMPFRAMES1;          /* 0x2E */
    short JUMPFRAMES2;          /* 0x30 */
    short STARJUMPFRAMES;       /* 0x32 */
    short SOMERSAULTFRAMES;     /* 0x34 */
    short SPINFRAMES;           /* 0x36 */
    short SPINRESETFRAMES;      /* 0x38 */
    short SUPERSPINFRAMES;      /* 0x3A */
    short SUPERSPINWAITFRAMES;  /* 0x3C */
    short SLAMWAITFRAMES;       /* 0x3E */
    short SLIDEFRAMES;          /* 0x40 */
    short CROUCHINGFRAMES;      /* 0x42 */
    short JUMPLANDFRAMES;       /* 0x44 */
    short spad;                 /* 0x46 */
}; /* 0x48 */

/* Entry of the per-character animation list tables (CrashAnim etc).
 * PS2-verified in LoadCharacterModels (file 0x0, action 0x4, flags 0xC,
 * levbits 0x10, stride 0x18) and ModelAnimDuration (speed 0x8). */
struct animlist {
    char *file;                    /* 0x00 */
    short action;                  /* 0x04 */
    unsigned char blend_in_frames; /* 0x06 */
    unsigned char blend_out_frames;/* 0x07 */
    float speed;                   /* 0x08 */
    unsigned short flags;          /* 0x0C */
    char pad1;                     /* 0x0E */
    char pad2;                     /* 0x0F */
    unsigned long long levbits;    /* 0x10 */
}; /* 0x18 */

/* Static per-character data table (CData). */
typedef struct {
    char *path;               /* 0x00 */
    char *file;               /* 0x04 */
    char *name;               /* 0x08 */
    struct animlist *anim;    /* 0x0C */
    float radius;             /* 0x10 */
    struct nuvec_s min;       /* 0x14 */
    struct nuvec_s max;       /* 0x20 */
    float scale;              /* 0x2C */
    float shadow_scale;       /* 0x30 */
} CharacterData; /* 0x34 */

/* Loaded character model + animation banks.
 * PS2 v1.03 size 0x988 (GC dropped the three shadow-model fields and is
 * 0x7AC).  PS2-verified in LoadCharacterModels / EvalModelAnim /
 * ModelAnimDuration: hobj 0x0, anmdata 0x4, animlist 0x1DC, fanmdata 0x3B4,
 * fanimlist 0x58C, sanmdata 0x764, shadhdr 0x93C, shaddata 0x940,
 * character 0x944, pLOCATOR 0x948.
 * "sanmdata"/"shaddata" are our names for the PS2-only shadow-skin data
 * (filled from InstShadDataLoad / ShadFindData); the retail names are
 * unknown. */
struct CharacterModel {
    struct NUHGOBJ_s *hobj;              /* 0x000 */
    struct nuanimdata_s *anmdata[118];   /* 0x004 */
    struct animlist *animlist[118];      /* 0x1DC */
    struct nuanimdata_s *fanmdata[118];  /* 0x3B4 */
    struct animlist *fanimlist[118];     /* 0x58C */
    struct nuanimdata_s *sanmdata[118];  /* 0x764  shadow anim data (PS2 only) */
    struct instSHADHDR_s *shadhdr;       /* 0x93C  shadow header (PS2 only) */
    struct NUHGOBJ_s *shaddata;          /* 0x940  shadow model (PS2 only) */
    short character;                     /* 0x944 */
    char pad1;                           /* 0x946 */
    char pad2;                           /* 0x947 */
    struct NUPOINTOFINTEREST_s *pLOCATOR[16]; /* 0x948 */
}; /* 0x988 */

/* Aku-aku / mask object.  Layout from the alpha-NGC DWARF dump; the PS2
 * accesses seen so far (active 0x16E, lights 0x98 in ResetPlayer) agree. */
struct mask_s {
    struct numtx_s mM;              /* 0x000 */
    struct numtx_s mS;              /* 0x040 */
    struct nuvec_s pos;             /* 0x080 */
    struct nuvec_s newpos;          /* 0x08C */
    struct Nearest_Light_s lights;  /* 0x098 */
    struct anim_s anim;             /* 0x148 */
    float scale;                    /* 0x164 */
    float shadow;                   /* 0x168 */
    short character;                /* 0x16C */
    short active;                   /* 0x16E */
    short sfx;                      /* 0x170 */
    char pad1;                      /* 0x172 */
    char pad2;                      /* 0x173 */
    unsigned short xrot;            /* 0x174 */
    unsigned short yrot;            /* 0x176 */
    unsigned short angle;           /* 0x178 */
    unsigned short surface_xrot;    /* 0x17A */
    unsigned short surface_zrot;    /* 0x17C */
    unsigned short wibble_ang[3];   /* 0x17E */
    float idle_time;                /* 0x184 */
    float idle_duration;            /* 0x188 */
    signed char idle_mode;          /* 0x18C */
    signed char reflect;            /* 0x18D */
    unsigned char offset_frame;     /* 0x18E */
    signed char hold;               /* 0x18F */
}; /* 0x190 */

/* ------------------------------------------------------------------ */
/* obj_s -- one game object                                            */
/* ------------------------------------------------------------------ */

/* PS2-verified size 0x188 (creature_s.obj at +0x4, creature_s.ai at
 * +0x18C).  All offsets below that unit 91 touches were checked against
 * the retail assembly; the rest follow the alpha-NGC DWARF layout, which
 * has agreed with the PS2 image at every verified point. */
struct obj_s {
    struct obj_s *parent;            /* 0x000 */
    struct CharacterModel *model;    /* 0x004 */
    struct mask_s *mask;             /* 0x008 */
    void *contact;                   /* 0x00C */
    struct numtx_s *pLOCATOR;        /* 0x010 */
    struct anim_s anim;              /* 0x014 */
    short character;                 /* 0x030 */
    short vehicle;                   /* 0x032 */
    unsigned int flags;              /* 0x034 */
    unsigned int frame;              /* 0x038 */
    unsigned int draw_frame;         /* 0x03C */
    unsigned int vehicle_frame;      /* 0x040 */
    struct RPos_s RPos;              /* 0x044 */
    struct nuvec_s pos;              /* 0x068 */
    struct nuvec_s mom;              /* 0x074 */
    struct nuvec_s oldpos;           /* 0x080 */
    struct nuvec_s startpos;         /* 0x08C */
    struct nuvec_s vSN;              /* 0x098 */
    struct nuvec_s vLN;              /* 0x0A4 */
    struct nuvec_s vRN;              /* 0x0B0 */
    float shadow;                    /* 0x0BC */
    float layer_shadow;              /* 0x0C0 */
    float roof_y;                    /* 0x0C4 */
    float clearance;                 /* 0x0C8 */
    float forward;                   /* 0x0CC */
    float abs_forward;               /* 0x0D0 */
    float side;                      /* 0x0D4 */
    float abs_side;                  /* 0x0D8 */
    float xyz_distance;              /* 0x0DC */
    float xz_distance;               /* 0x0E0 */
    float radius;                    /* 0x0E4 */
    struct nuvec_s min;              /* 0x0E8 */
    struct nuvec_s max;              /* 0x0F4 */
    float scale;                     /* 0x100 */
    float SCALE;                     /* 0x104 */
    float RADIUS;                    /* 0x108 */
    float old_SCALE;                 /* 0x10C */
    float objbot;                    /* 0x110 */
    float objtop;                    /* 0x114 */
    float bot;                       /* 0x118 */
    float top;                       /* 0x11C */
    float oldobjbot;                 /* 0x120 */
    float oldobjtop;                 /* 0x124 */
    float die_time;                  /* 0x128 */
    float die_duration;              /* 0x12C */
    float reflect_y;                 /* 0x130 */
    float idle_gametime;             /* 0x134 */
    float pad_speed;                 /* 0x138 */
    float pad_dx;                    /* 0x13C */
    float pad_dz;                    /* 0x140 */
    signed char i;                   /* 0x144 */
    signed char dead;                /* 0x145 */
    unsigned short pad_angle;        /* 0x146 */
    unsigned short attack;           /* 0x148 */
    unsigned short vulnerable;       /* 0x14A */
    short die_action;                /* 0x14C */
    signed char old_ground;          /* 0x14E */
    signed char finished;            /* 0x14F */
    unsigned short xrot;             /* 0x150 */
    unsigned short yrot;             /* 0x152 */
    unsigned short zrot;             /* 0x154 */
    unsigned short surface_xrot;     /* 0x156 */
    unsigned short surface_zrot;     /* 0x158 */
    unsigned short layer_xrot;       /* 0x15A */
    unsigned short layer_zrot;       /* 0x15C */
    unsigned short roof_xrot;        /* 0x15E */
    unsigned short roof_zrot;        /* 0x160 */
    short target_xrot;               /* 0x162 */
    short target_yrot;               /* 0x164 */
    short dyrot;                     /* 0x166 */
    union {
        unsigned char chrs[2];
        unsigned short all;
    } gndflags;                      /* 0x168 */
    unsigned short hdg;              /* 0x16A */
    unsigned short thdg;             /* 0x16C */
    signed char ground;              /* 0x16E */
    signed char surface_type;        /* 0x16F */
    signed char layer_type;          /* 0x170 */
    signed char roof_type;           /* 0x171 */
    signed char invisible;           /* 0x172 */
    unsigned char submerged;         /* 0x173 */
    signed char transporting;        /* 0x174 */
    signed char got_shadow;          /* 0x175 */
    unsigned char boing;             /* 0x176 */
    unsigned char contact_type;      /* 0x177 */
    signed char die_model[2];        /* 0x178 */
    unsigned char invincible;        /* 0x17A */
    signed char pos_adjusted;        /* 0x17B */
    signed char wade;                /* 0x17C */
    signed char dangle;              /* 0x17D */
    char ddsand;                     /* 0x17E */
    char ddsnow;                     /* 0x17F */
    char ddwater;                    /* 0x180 */
    char ddr;                        /* 0x181 */
    char ddg;                        /* 0x182 */
    char ddb;                        /* 0x183 */
    signed char last_ground;         /* 0x184 */
    signed char direction;           /* 0x185 */
    signed char kill_contact;        /* 0x186 */
    unsigned char touch;             /* 0x187 */
}; /* 0x188 */

/* ------------------------------------------------------------------ */
/* AI state                                                            */
/* ------------------------------------------------------------------ */

/* Size 0x98 (derived: creature_s.ai 0x18C .. Buggy 0x224; agrees with the
 * alpha-NGC DWARF layout). */
struct AI_s {
    struct nuvec_s oldpos;        /* 0x00 */
    struct nuvec_s newpos;        /* 0x0C */
    struct nuvec_s wobblepos;     /* 0x18 */
    float time;                   /* 0x24 */
    float duration;               /* 0x28 */
    float spins;                  /* 0x2C */
    float speed;                  /* 0x30 */
    float radius;                 /* 0x34 */
    float distance;               /* 0x38 */
    float height;                 /* 0x3C */
    float lateral;                /* 0x40 */
    float scale;                  /* 0x44 */
    float wobble;                 /* 0x48 */
    unsigned short oldangle;      /* 0x4C */
    unsigned short angle;         /* 0x4E */
    unsigned short old_yrot;      /* 0x50 */
    unsigned short movflags;      /* 0x52 */
    unsigned short terflags;      /* 0x54 */
    unsigned short rotflags;      /* 0x56 */
    unsigned short rotspeed;      /* 0x58 */
    unsigned short rotadjust;     /* 0x5A */
    unsigned short wobble_ang;    /* 0x5C */
    unsigned short wobble_speed;  /* 0x5E */
    signed char direction;        /* 0x60 */
    signed char i0;               /* 0x61 */
    signed char i1;               /* 0x62 */
    signed char idle_update;      /* 0x63 */
    void *event_function[2];      /* 0x64 */
    float event_distance[2];      /* 0x6C */
    short event_branch[2];        /* 0x74 */
    signed char event[2];         /* 0x78 */
    signed char event_index[2];   /* 0x7A */
    unsigned char event_flags[2]; /* 0x7C */
    unsigned char event_occured[2]; /* 0x7E */
    unsigned short attack_locator_bits; /* 0x80 */
    char pad1;                    /* 0x82 */
    char pad2;                    /* 0x83 */
    float attack_locator_radius;  /* 0x84 */
    struct nuhspecial_s obj;      /* 0x88 */
    signed char kill;             /* 0x90 */
    signed char locator_kill;     /* 0x91 */
    short force_action;           /* 0x92 */
    short look_creature;          /* 0x94 */
    unsigned char count;          /* 0x96 */
    unsigned char hits;           /* 0x97 */
}; /* 0x98 */

struct creatcmd_s {
    int cmd;   /* 0x0 */
    int i;     /* 0x4 */
    float f;   /* 0x8 */
}; /* 0xC */

/* Pad rumble state.  PS2-verified in UpdateRumble / NewRumble / NewBuzz
 * (buzz 0x0, power 0x1, frame 0x2, frames 0x3). */
struct rumble_s {
    unsigned char buzz;    /* 0x0 */
    unsigned char power;   /* 0x1 */
    unsigned char frame;   /* 0x2 */
    unsigned char frames;  /* 0x3 */
}; /* 0x4 */

/* ------------------------------------------------------------------ */
/* creature_s -- one creature / player slot                            */
/* ------------------------------------------------------------------ */

/* PS2-verified size 0xCE4.  Anchors from retail asm: obj +0x4 (RemoveCreature),
 * lights +0xBF4 (ResetPlayer), idle_time +0xCA8 .. freeze +0xCE1
 * (ResetPlayerMoves), array stride 0xCE4 (CloseCreatures). */
struct creature_s {
    signed char used;          /* 0x000 */
    signed char on;            /* 0x001 */
    signed char off_wait;      /* 0x002 */
    signed char i_aitab;       /* 0x003 */
    struct obj_s obj;          /* 0x004 */
    struct AI_s ai;            /* 0x18C */
    struct NEWBUGGY *Buggy;    /* 0x224 */
    struct creatcmd_s *cmdtable; /* 0x228 */
    struct creatcmd_s *cmdcurr;  /* 0x22C */
    struct MoveInfo *OnFootMoveInfo; /* 0x230 */
    struct numtx_s m;          /* 0x234 */
    struct numtx_s mtxLOCATOR[16][2]; /* 0x274 */
    struct nuvec_s momLOCATOR[16][2]; /* 0xA74 */
    struct Nearest_Light_s lights; /* 0xBF4 */
    struct rumble_s rumble;    /* 0xCA4 */
    float idle_time;           /* 0xCA8 */
    float idle_wait;           /* 0xCAC */
    short idle_action;         /* 0xCB0 */
    short old_idle_action;     /* 0xCB2 */
    signed char idle_mode;     /* 0xCB4 */
    signed char idle_repeat;   /* 0xCB5 */
    signed char jump;          /* 0xCB6 */
    signed char jump_type;     /* 0xCB7 */
    signed char jump_subtype;  /* 0xCB8 */
    signed char ok_slam;       /* 0xCB9 */
    signed char slam;          /* 0xCBA */
    signed char spin;          /* 0xCBB */
    signed char crawl;         /* 0xCBC */
    signed char crawl_lock;    /* 0xCBD */
    signed char tiptoe;        /* 0xCBE */
    signed char sprint;        /* 0xCBF */
    unsigned char somersault;  /* 0xCC0 */
    unsigned char land;        /* 0xCC1 */
    signed char pad_type;      /* 0xCC2 */
    signed char jump_hack;     /* 0xCC3 */
    unsigned char jump_hold;   /* 0xCC4 */
    unsigned char allow_jump;  /* 0xCC5 */
    short jump_frames;         /* 0xCC6 */
    short jump_frame;          /* 0xCC8 */
    short slam_wait;           /* 0xCCA */
    short spin_frames;         /* 0xCCC */
    short spin_frame;          /* 0xCCE */
    short spin_wait;           /* 0xCD0 */
    short slide;               /* 0xCD2 */
    short crouch_pos;          /* 0xCD4 */
    unsigned short slam_frame; /* 0xCD6 */
    short fire_action;         /* 0xCD8 */
    unsigned char fire;        /* 0xCDA */
    unsigned char tap;         /* 0xCDB */
    signed char target;        /* 0xCDC */
    signed char target_wait;   /* 0xCDD */
    signed char fire_lock;     /* 0xCDE */
    signed char idle_sigh;     /* 0xCDF */
    unsigned char hit_type;    /* 0xCE0 */
    unsigned char freeze;      /* 0xCE1 */
    signed char anim_processed;/* 0xCE2 */
    char pad1;                 /* 0xCE3 */
}; /* 0xCE4 */

struct pad_s; /* game pad state; buttons word verified at +0x564 */

/* ------------------------------------------------------------------ */
/* Globals owned by creature.c (PS2 v1.03 addresses in comments)       */
/* ------------------------------------------------------------------ */

extern struct creature_s Character[9];   /* 0x0057EE38 */
extern struct creature_s *player;        /* 0x00630968 */
/* 48 entries on PS2 v1.03 (GC has 49): CloseCreatures' loop bound is
 * 0x1C980 = 48 * 0x988, and CLetter sits at CModel + 48 * 0x988. */
extern struct CharacterModel CModel[48]; /* 0x005623F8 */
extern signed char CRemap[191];          /* 0x00562338 */

/* ------------------------------------------------------------------ */
/* Functions of unit 91 (game/creature)                                */
/* ------------------------------------------------------------------ */

void ResetPlayer(s32 warp);                          /* 0x001CBCE8 */
void ManageCreatures(void);                          /* 0x001CC218 */
void LoadCharacterModels(void);                      /* 0x001CCBF8 */
void ChangeCharacter(struct creature_s *c, s32 character); /* 0x001CD7C8 */
void PlayerStartPos(struct creature_s *c, struct nuvec_s *pos); /* 0x001CD9A8 */
s32 AddCreature(s32 character, s32 index, s32 i_aitab); /* 0x001CDAE8 */
s32 NewCharacterIdle(struct creature_s *c, struct CharacterModel *model); /* 0x001CDEA8 */
void UpdateCharacterIdle(struct creature_s *c, s32 character); /* 0x001CE230 */
void MovePlayer(struct creature_s *c, struct pad_s *pad); /* 0x001CE3E0 */
void ProcessCreatures(void);                         /* 0x001D1FF0 */
void EvalModelAnim(struct CharacterModel *model, struct anim_s *anim,
                   struct numtx_s *m, struct numtx_s *tmtx, float ***dwa,
                   struct numtx_s *mLOCATOR);        /* 0x001D2470 */
/* DrawCharacterModel/DrawCreatures: parameter lists taken from the GC
 * decompilation; not yet verified against the PS2 call sites. */
s32 DrawCharacterModel(struct CharacterModel *model, struct anim_s *anim,
                       struct numtx_s *mC, struct numtx_s *mS, s32 render,
                       struct numtx_s *mR, struct numtx_s *loc_mtx,
                       struct nuvec_s *loc_mom, struct obj_s *obj); /* 0x001D2728 */
void DrawCreatures(struct creature_s *c, s32 count, s32 render, s32 shadow); /* 0x001D2F50 */
void UpdateAnimPacket(struct CharacterModel *mod, struct anim_s *anim,
                      float dt, float xz_distance);  /* 0x001D4FC0 */
float ModelAnimDuration(u32 character, u32 action, float start, float end); /* 0x001D5528 */
void TerrainFailsafe(struct obj_s *obj);             /* 0x001D5648 */
void ResetPlayerMoves(struct creature_s *c);         /* 0x001D56A0 */
void RemoveCreature(struct creature_s *c);           /* 0x001D5780 */
void CloseCreatures(void);                           /* 0x001D57B0 */
void ResetAnimPacket(struct anim_s *anim, s32 action); /* 0x001D5850 */
void UpdateRumble(struct rumble_s *rumble);          /* 0x001D5880 */
void NewRumble(struct rumble_s *rumble, s32 power);  /* 0x001D58A8 */
void NewBuzz(struct rumble_s *rumble, s32 frames);   /* 0x001D5900 */
void StoreLocatorMatrices(struct CharacterModel *model, struct numtx_s *mC,
                          struct numtx_s *tmtx, struct numtx_s *mtx,
                          struct nuvec_s *mom);      /* 0x001D5918 */

#endif /* GAMECODE_CREATURE_H */
