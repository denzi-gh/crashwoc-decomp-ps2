#ifndef GAMELIB_DEBRIS_H
#define GAMELIB_DEBRIS_H

#include "nu_types.h"
#include "numath/numathtypes.h"

struct debinfo_s;

/* One particle as the ptl program reads it. */
struct debpart_s {
    struct nuvec_s pos;                     /* 0x00 */
    f32 time;                               /* 0x0C */
    struct nuvec_s mom;                     /* 0x10 */
    f32 rate;                               /* 0x1C */
}; /* 0x20 */

/* A DMA particle set (CreateDmaParticleSet): its tags, then 32 particles. */
struct debchunk_s {
    u8 pad_00[0x20];                        /* 0x00 */
    struct debpart_s parts[32];             /* 0x20 */
}; /* 0x420 */

/* One collision sphere of an emitter. */
struct debrissphere_s {
    struct nuvec_s emit;                    /* 0x00 */
    f32 t;                                  /* 0x0C */
}; /* 0x10 */

/* One debris emitter (debkeydata[]). */
struct debkeydatatype_s {
    struct debchunk_s *chunks[256];         /* 0x000 */
    struct numtx_s rotmtx;                  /* 0x400 */
    struct numtx_s emitrotmtx;              /* 0x440 */
    s16 count;                              /* 0x480 */
    s16 type;                               /* 0x482 */
    s16 active;                             /* 0x484 */
    s16 debcount;                           /* 0x486 */
    s16 disposed;                           /* 0x488 */
    s16 pointer;                            /* 0x48A */
    s16 reqcount;                           /* 0x48C */
    s16 reqdebcount;                        /* 0x48E */
    struct nuvec_s emitpos;                 /* 0x490 */
    struct nuvec_s emitoffset;              /* 0x49C */
    struct debpart_s *(*genptr)();          /* 0x4A8 */
    void (*gencode)(struct debkeydatatype_s *deb, struct debinfo_s *info, struct debpart_s *part); /* 0x4AC */
    struct debkeydatatype_s *next;          /* 0x4B0 */
    struct debkeydatatype_s *prev;          /* 0x4B4 */
    s16 rotory;                             /* 0x4B8 */
    s16 rotorz;                             /* 0x4BA */
    s32 instances;                          /* 0x4BC */
    s16 oncount;                            /* 0x4C0 */
    s16 delay;                              /* 0x4C2 */
    struct debrissphere_s spheres[8];       /* 0x4C4 */
    s16 sphere_next;                        /* 0x544 */
    s16 sphere_next_emit;                   /* 0x546 */
    s32 trigger_type;                       /* 0x548 */
    s32 trigger_id;                         /* 0x54C */
    f32 trigger_var;                        /* 0x550 */
    s16 refrotz;                            /* 0x554 */
    s16 refroty;                            /* 0x556 */
    f32 refoff;                             /* 0x558 */
    f32 refbounce;                          /* 0x55C */
    s16 sound_next[4];                      /* 0x560 */
    s16 group_id;                           /* 0x568 */
}; /* 0x56C */

struct debsound_s {
    s32 id;                                 /* 0x00 */
    s32 type;                               /* 0x04 */
    s32 delay;                              /* 0x08 */
}; /* 0x0C */

/* Envelope key (alpha, size, rotation, jitter, collision radius). */
struct debkey_s {
    f32 t;                                  /* 0x00 */
    f32 value;                              /* 0x04 */
}; /* 0x08 */

struct debcolkey_s {
    f32 t;                                  /* 0x00 */
    f32 r;                                  /* 0x04 */
    f32 g;                                  /* 0x08 */
    f32 b;                                  /* 0x0C */
}; /* 0x10 */

union debchannel_u {
    f32 coord;
    u8 colour;
    u32 bits;
};

struct debtri_s {
    union debchannel_u v0x;                 /* 0x00 */
    union debchannel_u v0y;                 /* 0x04 */
    union debchannel_u v1x;                 /* 0x08 */
    union debchannel_u v1y;                 /* 0x0C */
    f32 v2x;                                /* 0x10 */
    f32 v2y;                                /* 0x14 */
    u8 pad_18[8];                           /* 0x18 */
}; /* 0x20 */

/* VU1 packet header of a debris type. */
struct debdma_s {
    u8 pad_00[0x14];                        /* 0x00 */
    f32 grav;                               /* 0x14 */
    u8 pad_18[0x18];                        /* 0x18 */
    s32 u0;                                 /* 0x30 */
    s32 v0;                                 /* 0x34 */
    s32 u1;                                 /* 0x38 */
    s32 v1;                                 /* 0x3C */
    s32 u2;                                 /* 0x40 */
    s32 v2;                                 /* 0x44 */
    s32 u3;                                 /* 0x48 */
    s32 v3;                                 /* 0x4C */
    u8 pad_50[0x10];                        /* 0x50 */
    struct debtri_s tri[64];                /* 0x60 */
}; /* 0x860 */

/* A debris type definition (debtab[]). */
struct debinfo_s {
    char id[0x10];                          /* 0x000 */
    s8 listid;                              /* 0x010 */
    s16 generate;                           /* 0x012 */
    s16 debnum;                             /* 0x014 */
    s16 ival_offset;                        /* 0x016 */
    s16 ival_on;                            /* 0x018 */
    s16 ival_on_ran;                        /* 0x01A */
    s16 ival_off;                           /* 0x01C */
    s16 ival_off_ran;                       /* 0x01E */
    s8 gensort;                             /* 0x020 */
    s8 gencode;                             /* 0x021 */
    s8 mat;                                 /* 0x022 */
    s8 efftab;                              /* 0x023 */
    f32 radius;                             /* 0x024 */
    f32 cuton;                              /* 0x028 */
    f32 cutoff;                             /* 0x02C */
    f32 drawcutoff;                         /* 0x030 */
    f32 emitmag;                            /* 0x034 */
    struct nuvec_s variable_emit;           /* 0x038 */
    struct nuvec_s variable_emit_ranscale;  /* 0x044 */
    struct nuvec_s variable_start;          /* 0x050 */
    struct nuvec_s variable_start_ranscale; /* 0x05C */
    struct nuvec_s pos_range;               /* 0x068 */
    struct nuvec_s pos_base;                /* 0x074 */
    struct nuvec_s mom_range;               /* 0x080 */
    struct nuvec_s mom_base;                /* 0x08C */
    f32 grav;                               /* 0x098 */
    f32 etime;                              /* 0x09C */
    s16 uw;                                 /* 0x0A0 */
    s8 frames;                              /* 0x0A2 */
    s8 rate;                                /* 0x0A3 */
    f32 uvanim;                             /* 0x0A4 */
    f32 jibxfreq;                           /* 0x0A8 */
    f32 jibxamp;                            /* 0x0AC */
    f32 jibyfreq;                           /* 0x0B0 */
    f32 jibyamp;                            /* 0x0B4 */
    struct debcolkey_s colslot[8];          /* 0x0B8 */
    struct debkey_s alpslot[8];             /* 0x138 */
    f32 minsize;                            /* 0x178 */
    f32 maxsize;                            /* 0x17C */
    struct debkey_s wslot[8];               /* 0x180 */
    struct debkey_s hslot[8];               /* 0x1C0 */
    f32 minrot;                             /* 0x200 */
    f32 maxrot;                             /* 0x204 */
    struct debkey_s rotslot[8];             /* 0x208 */
    struct debkey_s jibxslot[8];            /* 0x248 */
    struct debkey_s jibyslot[8];            /* 0x288 */
    f32 u1;                                 /* 0x2C8 */
    f32 v1;                                 /* 0x2CC */
    f32 u2;                                 /* 0x2D0 */
    f32 v2;                                 /* 0x2D4 */
    struct debdma_s *DmaDebTypePointer;     /* 0x2D8 */
    struct debkey_s sphereslot[8];          /* 0x2DC */
    s8 numspheres;                          /* 0x31C */
    s32 variable_key;                       /* 0x320 */
    struct debsound_s sounds[4];            /* 0x324 */
}; /* 0x354 */

/* One queued chunk render (ParticleChunkRenderStack[]). */
struct debrender_s {
    struct debchunk_s *chunk;               /* 0x00 */
    struct debinfo_s *debinfo;              /* 0x04 */
    struct debkeydatatype_s *debdata;       /* 0x08 */
    struct numtx_s rotmtx;                  /* 0x0C */
    struct nuvec_s pos;                     /* 0x4C */
}; /* 0x58 */

extern struct debkeydatatype_s debkeydata[256];
extern struct debinfo_s *debtab[128];
extern struct debrender_s ParticleChunkRenderStack[256];

#ifdef NW_HOST
/* Retail calls these without a prototype in scope (game/main.c); only the host declares them. */
/* Callers pass a flag it ignores (as retail does). */
void DebrisDraw();
void DebrisSetup(void);
#endif

#endif /* GAMELIB_DEBRIS_H */
