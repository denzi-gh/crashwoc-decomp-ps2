#ifndef GAME_CREATURE_H
#define GAME_CREATURE_H

#include "nu_types.h"
#include "numath/numathtypes.h"
#include "nu3d/nugscn.h"
#include "nu3d/nuanim.h"
#include "game/gametypes.h"

struct NUHGOBJ_s;
struct NUPOINTOFINTEREST_s;
struct nupad_s;
struct shaddata_s;
struct shadentry_s;

struct MoveInfo {
    float IDLESPEED;                    /* 0x00 */
    float TIPTOESPEED;                  /* 0x04 */
    float WALKSPEED;                    /* 0x08 */
    float RUNSPEED;                     /* 0x0C */
    float SPRINTSPEED;                  /* 0x10 */
    float SLIDESPEED;                   /* 0x14 */
    float CRAWLSPEED;                   /* 0x18 */
    float DANGLESPEED;                  /* 0x1C */
    float WADESPEED;                    /* 0x20 */
    float JUMPHEIGHT;                   /* 0x24 */
    float DANGLEGAP;                    /* 0x28 */
    short JUMPFRAMES0;                  /* 0x2C */
    short JUMPFRAMES1;                  /* 0x2E */
    short JUMPFRAMES2;                  /* 0x30 */
    short STARJUMPFRAMES;               /* 0x32 */
    short SOMERSAULTFRAMES;             /* 0x34 */
    short SPINFRAMES;                   /* 0x36 */
    short SPINRESETFRAMES;              /* 0x38 */
    short SUPERSPINFRAMES;              /* 0x3A */
    short SUPERSPINWAITFRAMES;          /* 0x3C */
    short SLAMWAITFRAMES;               /* 0x3E */
    short SLIDEFRAMES;                  /* 0x40 */
    short CROUCHINGFRAMES;              /* 0x42 */
    short JUMPLANDFRAMES;               /* 0x44 */
    short spad;                         /* 0x46 */
}; /* 0x48 */

/* Entry of the per-character animation tables (CrashAnim, ...). */
struct animlist {
    char *file;                         /* 0x00 */
    short action;                       /* 0x04 */
    u8 blend_in_frames;                 /* 0x06 */
    u8 blend_out_frames;                /* 0x07 */
    float speed;                        /* 0x08 */
    u16 flags;                          /* 0x0C */
    char pad1;                          /* 0x0E */
    char pad2;                          /* 0x0F */
    u64 levbits;                        /* 0x10 */
}; /* 0x18 */

/* One character of a hub/space cutscene and its animations (SpaceGameCutTab). */
struct space_s {
    s32 character;                      /* 0x00 */
    s32 unk_0x04;                       /* 0x04 */
    struct animlist anim[5];            /* 0x08 */
}; /* 0x80 */

/* Entry of the static character table CData. */
struct CharacterData {
    char *path;                         /* 0x00 */
    char *file;                         /* 0x04 */
    char *name;                         /* 0x08 */
    struct animlist *anim;              /* 0x0C */
    float radius;                       /* 0x10 */
    struct nuvec_s min;                 /* 0x14 */
    struct nuvec_s max;                 /* 0x20 */
    float scale;                        /* 0x2C */
    float shadow_scale;                 /* 0x30 */
}; /* 0x34 */

/* A loaded character model with its animation banks. The shadow fields are
 * PS2 only: shadhdr is the InstShadDataLoad result, shaddata[action] its
 * ShadFindData entry per animation, and shaddatarest the default entry. */
struct CharacterModel {
    struct NUHGOBJ_s *obj;              /* 0x000 */
    struct nuanimdata_s *anmdata[118];  /* 0x004 */
    struct animlist *animlist[118];     /* 0x1DC */
    struct nuanimdata_s *fanmdata[118]; /* 0x3B4 */
    struct animlist *fanimlist[118];    /* 0x58C */
    struct shadentry_s *shaddata[118];  /* 0x764 */
    struct shaddata_s *shadhdr;         /* 0x93C */
    struct shadentry_s *shaddatarest;   /* 0x940 */
    short character;                    /* 0x944 */
    char pad1;                          /* 0x946 */
    char pad2;                          /* 0x947 */
    struct NUPOINTOFINTEREST_s *pLOCATOR[16]; /* 0x948 */
}; /* 0x988 */

struct obj_s {
    void *parent;                       /* 0x000 */
    struct CharacterModel *model;       /* 0x004 */
    struct mask_s *mask;                /* 0x008 */
    void *contact;                      /* 0x00C */
    struct numtx_s *pLOCATOR;           /* 0x010 */
    struct anim_s anim;                 /* 0x014 */
    short character;                    /* 0x030 */
    short vehicle;                      /* 0x032 */
    u32 flags;                          /* 0x034 */
    u32 frame;                          /* 0x038 */
    u32 draw_frame;                     /* 0x03C */
    u32 vehicle_frame;                  /* 0x040 */
    struct RPos_s RPos;                 /* 0x044 */
    struct nuvec_s pos;                 /* 0x068 */
    struct nuvec_s mom;                 /* 0x074 */
    struct nuvec_s oldpos;              /* 0x080 */
    struct nuvec_s startpos;            /* 0x08C */
    struct nuvec_s vSN;                 /* 0x098 */
    struct nuvec_s vLN;                 /* 0x0A4 */
    struct nuvec_s vRN;                 /* 0x0B0 */
    float shadow;                       /* 0x0BC */
    float layer_shadow;                 /* 0x0C0 */
    float roof_y;                       /* 0x0C4 */
    float clearance;                    /* 0x0C8 */
    float forward;                      /* 0x0CC */
    float abs_forward;                  /* 0x0D0 */
    float side;                         /* 0x0D4 */
    float abs_side;                     /* 0x0D8 */
    float xyz_distance;                 /* 0x0DC */
    float xz_distance;                  /* 0x0E0 */
    float radius;                       /* 0x0E4 */
    struct nuvec_s min;                 /* 0x0E8 */
    struct nuvec_s max;                 /* 0x0F4 */
    float scale;                        /* 0x100 */
    float SCALE;                        /* 0x104 */
    float RADIUS;                       /* 0x108 */
    float old_SCALE;                    /* 0x10C */
    float objbot;                       /* 0x110 */
    float objtop;                       /* 0x114 */
    float bot;                          /* 0x118 */
    float top;                          /* 0x11C */
    float oldobjbot;                    /* 0x120 */
    float oldobjtop;                    /* 0x124 */
    float die_time;                     /* 0x128 */
    float die_duration;                 /* 0x12C */
    float reflect_y;                    /* 0x130 */
    float idle_gametime;                /* 0x134 */
    float pad_speed;                    /* 0x138 */
    float pad_dx;                       /* 0x13C */
    float pad_dz;                       /* 0x140 */
    signed char i;                      /* 0x144 */
    signed char dead;                   /* 0x145 */
    u16 pad_angle;                      /* 0x146 */
    u16 attack;                         /* 0x148 */
    u16 vulnerable;                     /* 0x14A */
    short die_action;                   /* 0x14C */
    signed char old_ground;             /* 0x14E */
    signed char finished;               /* 0x14F */
    u16 xrot;                           /* 0x150 */
    u16 yrot;                           /* 0x152 */
    u16 zrot;                           /* 0x154 */
    u16 surface_xrot;                   /* 0x156 */
    u16 surface_zrot;                   /* 0x158 */
    u16 layer_xrot;                     /* 0x15A */
    u16 layer_zrot;                     /* 0x15C */
    u16 roof_xrot;                      /* 0x15E */
    u16 roof_zrot;                      /* 0x160 */
    short target_xrot;                  /* 0x162 */
    short target_yrot;                  /* 0x164 */
    short dyrot;                        /* 0x166 */
    union {
        u8 chrs[2];
        u16 all;
    } gndflags;                         /* 0x168 */
    u16 hdg;                            /* 0x16A */
    u16 thdg;                           /* 0x16C */
    signed char ground;                 /* 0x16E */
    signed char surface_type;           /* 0x16F */
    signed char layer_type;             /* 0x170 */
    signed char roof_type;              /* 0x171 */
    signed char invisible;              /* 0x172 */
    u8 submerged;                       /* 0x173 */
    signed char transporting;           /* 0x174 */
    signed char got_shadow;             /* 0x175 */
    u8 boing;                           /* 0x176 */
    u8 contact_type;                    /* 0x177 */
    signed char die_model[2];           /* 0x178 */
    u8 invincible;                      /* 0x17A */
    signed char pos_adjusted;           /* 0x17B */
    signed char wade;                   /* 0x17C */
    signed char dangle;                 /* 0x17D */
    char ddsand;                        /* 0x17E */
    char ddsnow;                        /* 0x17F */
    char ddwater;                       /* 0x180 */
    char ddr;                           /* 0x181 */
    char ddg;                           /* 0x182 */
    char ddb;                           /* 0x183 */
    signed char last_ground;            /* 0x184 */
    signed char direction;              /* 0x185 */
    signed char kill_contact;           /* 0x186 */
    u8 touch;                           /* 0x187 */
}; /* 0x188 */

struct AI_s {
    struct nuvec_s oldpos;              /* 0x00 */
    struct nuvec_s newpos;              /* 0x0C */
    struct nuvec_s wobblepos;           /* 0x18 */
    float time;                         /* 0x24 */
    float duration;                     /* 0x28 */
    float spins;                        /* 0x2C */
    float speed;                        /* 0x30 */
    float radius;                       /* 0x34 */
    float distance;                     /* 0x38 */
    float height;                       /* 0x3C */
    float lateral;                      /* 0x40 */
    float scale;                        /* 0x44 */
    float wobble;                       /* 0x48 */
    u16 oldangle;                       /* 0x4C */
    u16 angle;                          /* 0x4E */
    u16 old_yrot;                       /* 0x50 */
    u16 movflags;                       /* 0x52 */
    u16 terflags;                       /* 0x54 */
    u16 rotflags;                       /* 0x56 */
    u16 rotspeed;                       /* 0x58 */
    u16 rotadjust;                      /* 0x5A */
    u16 wobble_ang;                     /* 0x5C */
    u16 wobble_speed;                   /* 0x5E */
    signed char direction;              /* 0x60 */
    signed char i0;                     /* 0x61 */
    signed char i1;                     /* 0x62 */
    signed char idle_update;            /* 0x63 */
    void *event_function[2];            /* 0x64 */
    float event_distance[2];            /* 0x6C */
    short event_branch[2];              /* 0x74 */
    signed char event[2];               /* 0x78 */
    signed char event_index[2];         /* 0x7A */
    u8 event_flags[2];                  /* 0x7C */
    u8 event_occured[2];                /* 0x7E */
    u16 attack_locator_bits;            /* 0x80 */
    char pad1;                          /* 0x82 */
    char pad2;                          /* 0x83 */
    float attack_locator_radius;        /* 0x84 */
    struct nuhspecial_s obj;            /* 0x88 */
    signed char kill;                   /* 0x90 */
    signed char locator_kill;           /* 0x91 */
    short force_action;                 /* 0x92 */
    short look_creature;                /* 0x94 */
    u8 count;                           /* 0x96 */
    u8 hits;                            /* 0x97 */
}; /* 0x98 */

struct creatcmd_s {
    s32 cmd;                            /* 0x0 */
    nusintptr i;                        /* 0x4 */
    float f;                            /* 0x8 */
}; /* 0xC */

struct rumble_s {
    u8 buzz;                            /* 0x0 */
    u8 power;                           /* 0x1 */
    u8 frame;                           /* 0x2 */
    u8 frames;                          /* 0x3 */
}; /* 0x4 */

/* One creature or player slot (Character[9]). */
struct creature_s {
    signed char used;                   /* 0x000 */
    signed char on;                     /* 0x001 */
    signed char off_wait;               /* 0x002 */
    signed char i_aitab;                /* 0x003 */
    struct obj_s obj;                   /* 0x004 */
    struct AI_s ai;                     /* 0x18C */
    struct NEWBUGGY *Buggy;             /* 0x224 */
    struct creatcmd_s *cmdtable;        /* 0x228 */
    struct creatcmd_s *cmdcurr;         /* 0x22C */
    struct MoveInfo *minfo;             /* 0x230 */
    struct numtx_s m;                   /* 0x234 */
    struct numtx_s mtxLOCATOR[2][16];   /* 0x274  [model][locator]; DrawCreatures steps 0x400 per model */
    struct nuvec_s momLOCATOR[2][16];   /* 0xA74 */
    struct Nearest_Light_s lights;      /* 0xBF4 */
    struct rumble_s rumble;             /* 0xCA4 */
    float idle_time;                    /* 0xCA8 */
    float idle_wait;                    /* 0xCAC */
    short idle_action;                  /* 0xCB0 */
    short old_idle_action;              /* 0xCB2 */
    signed char idle_mode;              /* 0xCB4 */
    signed char idle_repeat;            /* 0xCB5 */
    signed char jump;                   /* 0xCB6 */
    signed char jump_type;              /* 0xCB7 */
    signed char jump_subtype;           /* 0xCB8 */
    signed char ok_slam;                /* 0xCB9 */
    signed char slam;                   /* 0xCBA */
    signed char spin;                   /* 0xCBB */
    signed char crawl;                  /* 0xCBC */
    signed char crawl_lock;             /* 0xCBD */
    signed char tiptoe;                 /* 0xCBE */
    signed char sprint;                 /* 0xCBF */
    u8 somersault;                      /* 0xCC0 */
    u8 land;                            /* 0xCC1 */
    signed char pad_type;               /* 0xCC2 */
    signed char jump_hack;              /* 0xCC3 */
    u8 jump_hold;                       /* 0xCC4 */
    u8 allow_jump;                      /* 0xCC5 */
    short jump_frames;                  /* 0xCC6 */
    short jump_frame;                   /* 0xCC8 */
    short slam_wait;                    /* 0xCCA */
    short spin_frames;                  /* 0xCCC */
    short spin_frame;                   /* 0xCCE */
    short spin_wait;                    /* 0xCD0 */
    short slide;                        /* 0xCD2 */
    short crouch_pos;                   /* 0xCD4 */
    u16 slam_frame;                     /* 0xCD6 */
    short fire_action;                  /* 0xCD8 */
    u8 fire;                            /* 0xCDA */
    u8 tap;                             /* 0xCDB */
    signed char target;                 /* 0xCDC */
    signed char target_wait;            /* 0xCDD */
    signed char fire_lock;              /* 0xCDE */
    signed char idle_sigh;              /* 0xCDF */
    u8 hit_type;                        /* 0xCE0 */
    u8 freeze;                          /* 0xCE1 */
    signed char anim_processed;         /* 0xCE2 */
    char pad1;                          /* 0xCE3 */
}; /* 0xCE4 */

extern struct creature_s Character[9];
extern struct creature_s *player;
extern struct creature_s OppTubCreature;
extern struct CharacterData CData[191];
extern struct CharacterModel CModel[48];
extern signed char CRemap[191];

void ResetPlayer(s32 warp);
void ManageCreatures(void);
void LoadCharacterModels(void);
void ChangeCharacter(struct creature_s *c, s32 character);
void PlayerStartPos(struct creature_s *c, struct nuvec_s *pos);
s32 AddCreature(s32 character, s32 index, s32 i_aitab);
s32 NewCharacterIdle(struct creature_s *c, struct CharacterModel *model);
void UpdateCharacterIdle(struct creature_s *c, s32 character);
void MovePlayer(struct creature_s *c, struct nupad_s *pad);
void ProcessCreatures(void);
void EvalModelAnim(struct CharacterModel *model, struct anim_s *anim, struct numtx_s *m, struct numtx_s *tmtx, float ***dwa, struct numtx_s *mLOCATOR);
s32 DrawCharacterModel(struct CharacterModel *model, struct anim_s *anim, struct numtx_s *mC, struct numtx_s *mS, s32 render, struct numtx_s *mR, struct numtx_s *loc_mtx, struct nuvec_s *loc_mom, struct obj_s *obj);
void DrawCreatures(struct creature_s *c, s32 count, s32 render, s32 shadow);
void UpdateAnimPacket(struct CharacterModel *mod, struct anim_s *anim, float dt, float xz_distance);
float ModelAnimDuration(u32 character, u32 action, float start, float end);
void TerrainFailsafe(struct obj_s *obj);
void ResetPlayerMoves(struct creature_s *c);
void RemoveCreature(struct creature_s *c);
void CloseCreatures(void);
void ResetAnimPacket(struct anim_s *anim, s32 action);
void UpdateRumble(struct rumble_s *rumble);
void NewRumble(struct rumble_s *rumble, s32 power);
void NewBuzz(struct rumble_s *rumble, s32 frames);
void StoreLocatorMatrices(struct CharacterModel *model, struct numtx_s *mC, struct numtx_s *tmtx, struct numtx_s *mtx, struct nuvec_s *mom);

#endif
