#ifndef GAMELIB_GCUTSCN_H
#define GAMELIB_GCUTSCN_H

#include "numath/numathtypes.h"
#include "nu3d/nugscn.h"

struct nuanimdata2_s;
struct NUTRIGGERSYS_s;
struct instNUTRIGGERSYS_s;
struct instNUGCUTSCENE_s;
union variptr_u;

typedef void (*NuGCutSceneEndFn)(struct instNUGCUTSCENE_s *scene);

struct NUSTATEANIM_s {
    int count;                          /* 0x00 */
    float *times;                       /* 0x04 */
    unsigned char *values;              /* 0x08 */
}; /* 0x0C */

struct NUGCUTCAMTGT_s {
    struct nuvec_s *object;             /* 0x00 */
    float frame;                        /* 0x04 */
    float nframes;                      /* 0x08 negative = blend back out */
    signed char camid;                  /* 0x0C */
    char pad[3];                        /* 0x0D */
}; /* 0x10 */

struct instNUGCUTCAM_s {
    unsigned char flags;                /* 0x00 */
    unsigned char tgt_ix;               /* 0x01 */
    char pad[2];                        /* 0x02 */
}; /* 0x04 */

struct NUGCUTCAM_s {
    struct numtx_s mtx;                 /* 0x00 */
    unsigned char flags;                /* 0x40 */
    char pad41;                         /* 0x41 */
    unsigned char anim_ix;              /* 0x42 */
    char pad43[0x50 - 0x43];            /* 0x43 */
}; /* 0x50 */

struct NUGCUTCAMSYS_s {
    unsigned int count;                 /* 0x00 */
    struct NUGCUTCAM_s *cams;           /* 0x04 */
    struct nuanimdata2_s *anim;         /* 0x08 */
    struct NUSTATEANIM_s *stateanim;    /* 0x0C camera switches */
    unsigned char first_camera;         /* 0x10 */
    signed char remap[1];               /* 0x11 camera id -> cams index */
}; /* 0x14 */

struct instNUGCUTCAMSYS_s {
    struct NUGCUTCAMTGT_s *targets;     /* 0x00 */
    struct instNUGCUTCAM_s *cams;       /* 0x04 */
    unsigned char next_switch;          /* 0x08 */
    signed char current;                /* 0x09 */
    unsigned char next_tgt;             /* 0x0A */
    unsigned char max;                  /* 0x0B */
    unsigned char count;                /* 0x0C */
}; /* 0x10 */

/* One locator of a cutscene definition. */
struct NUGCUTLOCATOR_s {
    char pad[0x4C];                     /* 0x00 */
    float value;                        /* 0x4C */
    struct nuanimdata2_s *anim;         /* 0x50 */
    char pad54[4];                      /* 0x54 */
    unsigned char flags;                /* 0x58 */
    unsigned char type;                 /* 0x59 index into NUGCUTLOCATORSYS_s.types */
    unsigned char joint_ix;             /* 0x5A 0xFF = not on a joint */
    char pad5B[0x64 - 0x5B];            /* 0x5B */
}; /* 0x64 */

struct NUGCUTLOCATORTYPE_s {
    char *name;                         /* 0x00 */
    unsigned char flags;                /* 0x04 */
    char pad5;                          /* 0x05 */
    unsigned short fn;                  /* 0x06 */
    char pad8[4];                       /* 0x08 */
}; /* 0x0C */

struct NUGCUTLOCATORSYS_s {
    struct NUGCUTLOCATOR_s *locators;   /* 0x00 */
    struct NUGCUTLOCATORTYPE_s *types;  /* 0x04 */
    unsigned char count;                /* 0x08 */
    unsigned char ntypes;               /* 0x09 */
}; /* 0x0C */

/* Per-instance state of one locator (stride 8). */
struct instNUGCUTLOCATOR_s {
    float timer;                        /* 0x00 debris emission accumulator */
    int flags;                          /* 0x04 owned by the locator function */
}; /* 0x08 */

struct instNUGCUTLOCATORSYS_s {
    struct instNUGCUTLOCATOR_s *locators; /* 0x00 */
}; /* 0x04 */

struct NUGCUTRIGID_s {
    struct numtx_s mtx;                 /* 0x00 */
    char *name;                         /* 0x40 */
    struct nuhspecial_s obj;            /* 0x44 */
    struct nuanimdata2_s *anim;         /* 0x4C */
    struct NUSTATEANIM_s *stateanim;    /* 0x50 */
    struct NUGCUTLOCATOR_s *locators;   /* 0x54 index on disk until fixed up */
    unsigned char flags;                /* 0x58 */
    unsigned char nlocators;            /* 0x59 */
    unsigned char first_locator;        /* 0x5A 0xFF = none */
    char pad5B;                         /* 0x5B */
}; /* 0x5C */

struct NUGCUTRIGIDSYS_s {
    struct NUGCUTRIGID_s *rigids;       /* 0x00 */
    unsigned short count;               /* 0x04 */
}; /* 0x08 */

struct instNUGCUTRIGID_s {
    struct nuhspecial_s obj;            /* 0x00 */
    unsigned char state;                /* 0x08 */
    char pad[3];                        /* 0x09 */
}; /* 0x0C */

struct instNUGCUTRIGIDSYS_s {
    struct instNUGCUTRIGID_s *rigids;   /* 0x00 */
}; /* 0x04 */

struct NUGCUTTRIGGER_s {
    short target;                       /* 0x00 */
    char pad2[2];                       /* 0x02 */
    char *name;                         /* 0x04 */
    struct NUSTATEANIM_s *anim;         /* 0x08 */
}; /* 0x0C */

struct NUGCUTTRIGGERSYS_s {
    int count;                          /* 0x00 */
    struct NUGCUTTRIGGER_s *triggers;   /* 0x04 */
}; /* 0x08 */

struct instNUGCUTTRIGGER_s {
    unsigned char state;                /* 0x00 */
    char pad[3];                        /* 0x01 */
}; /* 0x04 */

struct instNUGCUTTRIGGERSYS_s {
    struct instNUTRIGGERSYS_s *list;    /* 0x00 */
    struct instNUGCUTTRIGGER_s *triggers; /* 0x04 */
}; /* 0x08 */

struct NUGCUTBBOX_s {
    struct nuvec_s min;                 /* 0x00 */
    struct nuvec_s max;                 /* 0x0C */
}; /* 0x18 */

/* One animated character of a cutscene definition. */
struct NUGCUTCHAR_s {
    struct numtx_s mtx;                 /* 0x00 start matrix */
    char *name;                         /* 0x40 */
    void *char_anim;                    /* 0x44 */
    void *joint_anim;                   /* 0x48 */
    void *face_anim;                    /* 0x4C */
    void *character;                    /* 0x50 model, bound by the find-characters callback */
    struct NUGCUTLOCATOR_s *locators;   /* 0x54 index on disk until fixed up */
    char pad58[4];                      /* 0x58 */
    unsigned char flags;                /* 0x5C 0x2 = per-instance data (CharacterCreateData) */
    char pad5D;                         /* 0x5D */
    unsigned char nlocators;            /* 0x5E */
    char pad5F;                         /* 0x5F */
    unsigned char first_locator;        /* 0x60 0xFF = none */
    char pad61[0x64 - 0x61];            /* 0x61 */
}; /* 0x64 */

struct NUGCUTCHARSYS_s {
    struct NUGCUTCHAR_s *chars;         /* 0x00 */
    unsigned short count;               /* 0x04 */
}; /* 0x08 */

/* Per-instance state of one character (stride 0x18). */
struct instNUGCUTCHAR_s {
    void *data;                         /* 0x00 NUGCUTCHAR_s.character, or the
                                         * NuCutSceneCharacterCreateData result */
    char pad4[0x15 - 0x4];              /* 0x04 */
    unsigned char prev_animix;          /* 0x15 */
    unsigned char current_animix;       /* 0x16 */
    char pad17;                         /* 0x17 */
}; /* 0x18 */

struct instNUGCUTCHARSYS_s {
    struct instNUGCUTCHAR_s *chars;     /* 0x00 */
}; /* 0x04 */

/* A cutscene as loaded from its .cut file. */
struct NUGCUTSCENE_s {
    int version;                        /* 0x00 must be 1 */
    int base;                           /* 0x04 load address; relocation delta once loaded */
    float duration;                     /* 0x08 last frame */
    char *names;                        /* 0x0C string table */
    struct NUGCUTCAMSYS_s *camdef;      /* 0x10 */
    struct NUGCUTRIGIDSYS_s *rigiddef;  /* 0x14 */
    struct NUGCUTCHARSYS_s *chardef;    /* 0x18 */
    struct NUGCUTLOCATORSYS_s *locatordef; /* 0x1C */
    struct NUGCUTBBOX_s *bounds;        /* 0x20 */
    struct NUGCUTTRIGGERSYS_s *triggerdef; /* 0x24 */
}; /* 0x28 */

/* A running cutscene. */
struct instNUGCUTSCENE_s {
    struct instNUGCUTSCENE_s *next;     /* 0x00 */
    struct instNUGCUTSCENE_s *prev;     /* 0x04 */
    char name[16];                      /* 0x08 */
    struct numtx_s mtx;                 /* 0x18 valid if NUGCUTSCENE_MATRIXSET */
    struct NUGCUTSCENE_s *def;          /* 0x58 */
    struct nuvec_s centre;              /* 0x5C */
    float maxvisidist_sq;               /* 0x68 */
    int flags;                          /* 0x6C NUGCUTSCENE_* */
    float time;                         /* 0x70 current frame */
    float speed;                        /* 0x74 frames per update */
    struct instNUGCUTCAMSYS_s *camsys;  /* 0x78 */
    struct instNUGCUTRIGIDSYS_s *rigidsys; /* 0x7C */
    struct instNUGCUTCHARSYS_s *charsys; /* 0x80 */
    struct instNUGCUTLOCATORSYS_s *locatorsys; /* 0x84 */
    struct instNUGCUTTRIGGERSYS_s *triggersys; /* 0x88 */
    struct instNUGCUTSCENE_s *chain;    /* 0x8C started when this one ends */
    NuGCutSceneEndFn endcallback;       /* 0x90 */
}; /* 0x94 */

#define NUGCUTSCENE_STARTED   0x001
#define NUGCUTSCENE_RUNNING   0x002
#define NUGCUTSCENE_LOOP      0x004 /* instNuGCutSceneUpdate: restart at the end */
#define NUGCUTSCENE_AUTOSTART 0x008
#define NUGCUTSCENE_MATRIXSET 0x010
#define NUGCUTSCENE_CLIPBOUNDS 0x020
#define NUGCUTSCENE_CLIPDIST  0x040
#define NUGCUTSCENE_VISIBLE   0x080
#define NUGCUTSCENE_DISABLED  0x100
#define NUGCUTSCENE_UPDATED   0x200

/* Entry of the application's locator function table (NuGCutSceneSysInit). */
typedef void (*NuGCutLocatorUpdateFn)(struct instNUGCUTSCENE_s *scene, struct NUGCUTLOCATORSYS_s *def,
                                      struct instNUGCUTLOCATOR_s *locator,
                                      struct NUGCUTLOCATOR_s *data, float time, struct numtx_s *mtx);

struct NUGCUTLOCFNDATA_s {
    const char *name;                   /* 0x00 */
    NuGCutLocatorUpdateFn func;         /* 0x04 */
}; /* 0x08 */

typedef void (*NuCutSceneCharacterRenderFn)(struct instNUGCUTSCENE_s *scene, struct NUGCUTSCENE_s *def,
                                            struct instNUGCUTCHAR_s *character,
                                            struct NUGCUTCHAR_s *desc, float time);
typedef void (*NuCutSceneCharacterReleaseFn)(struct instNUGCUTCHAR_s *character);
typedef void (*NuCutSceneFindCharactersFn)(struct NUGCUTSCENE_s *def);
typedef void (*NuCutSceneCharacterCreateDataFn)(struct NUGCUTCHAR_s *desc,
                                                struct instNUGCUTCHAR_s *character, union variptr_u *heap);
typedef void (*NuCutSceneRigidCollisionCheckFn)(struct instNUGCUTRIGID_s *rigid, struct numtx_s *mtx);

struct nuanimtime_s;
struct nugscn_s;

void NuGCutSceneSysInit(struct NUGCUTLOCFNDATA_s *fns);
void NuGCutSceneSysUpdate(int paused);
void NuGCutSceneSysRender(void);
void NuSetCutSceneCharacterRenderFn(NuCutSceneCharacterRenderFn fn);
void NuSetCutSceneCharacterReleaseFn(NuCutSceneCharacterReleaseFn fn);
void NuSetCutSceneFindCharactersFn(NuCutSceneFindCharactersFn fn);
void NuSetCutSceneCharacterCreateDataFn(NuCutSceneCharacterCreateDataFn fn);
void NuSetCutSceneRigidCollisionCheckFn(NuCutSceneRigidCollisionCheckFn fn);
int instNuGCutSceneCharGetStartMtx(struct instNUGCUTSCENE_s *scene, char *name, struct numtx_s *mtx);
void NuGCutSceneFixUp(struct NUGCUTSCENE_s *def, void *gscene, struct NUTRIGGERSYS_s *triggers);
struct instNUGCUTSCENE_s *instNuGCutSceneFind(const char *name);
void instNuGCutSceneDestroy(struct instNUGCUTSCENE_s *scene);
int instNuGCutSceneIsFinished(struct instNUGCUTSCENE_s *scene);
void instNuGCutSceneUpdate(struct instNUGCUTSCENE_s *scene, int paused);
void instNuGCutSceneReset(struct instNUGCUTSCENE_s *scene);
void instNuGCutSceneCreateCamTgtArray(struct instNUGCUTSCENE_s *scene, int count, union variptr_u *heap);
int instNuGCutSceneAddCamTgt(struct instNUGCUTSCENE_s *scene, struct nuvec_s *object, float frame, float nframes, char camid);
void instNuGCutSceneSetPos(struct instNUGCUTSCENE_s *scene, struct nuvec4_s *pos);
void instNuGCutSceneTranslate(struct instNUGCUTSCENE_s *scene, struct nuvec_s *offset);
void instNuGCutSceneRotateY(struct instNUGCUTSCENE_s *scene, int angle);
void instNuGCutSceneSetMtx(struct instNUGCUTSCENE_s *scene, struct numtx_s *mtx);
void instNuGCutSceneChain(struct instNUGCUTSCENE_s *scene, struct instNUGCUTSCENE_s *chain);
void instNuGCutSceneSetEndCallback(struct instNUGCUTSCENE_s *scene, NuGCutSceneEndFn callback);
void instNuGCutSceneEnable(struct instNUGCUTSCENE_s *scene);
void instNuGCutSceneDisable(struct instNUGCUTSCENE_s *scene);
void DebugRndrEmitter(struct nuvec_s *pos, short rotz, short roty);
void NuGCutLocatorCalcMtx(struct NUGCUTLOCATOR_s *data, float time, struct numtx_s *mtx, struct nuanimtime_s *atime);
void instNuGCutDebrisLocatorUpdate(struct NUGCUTLOCATORSYS_s *def, struct instNUGCUTLOCATOR_s *locator, struct NUGCUTLOCATOR_s *data, float time, struct numtx_s *wm);
void instNuGCutLocatorSysUpdate(struct instNUGCUTSCENE_s *scene, float time);
void NuGCutCamsSysFixPtrs(struct NUGCUTSCENE_s *def, int base);
void instNuGCutSceneCalculateCentre(struct instNUGCUTSCENE_s *scene, struct numtx_s *mtx);
struct instNUGCUTCAMSYS_s *instNuCGutCamSysCreate(struct NUGCUTCAMSYS_s *def, union variptr_u *heap);
void instNuGCutCamSysStart(struct instNUGCUTCAMSYS_s *camsys, struct NUGCUTCAMSYS_s *def);
struct instNUGCUTLOCATORSYS_s *NuCGutLocatorSysCreateInst(struct NUGCUTLOCATORSYS_s *def, union variptr_u *heap);
void instNuGCutLocatorSysEnd(struct instNUGCUTLOCATORSYS_s *sys, struct NUGCUTLOCATORSYS_s *def, float time);
void instNuGCutLocatorSysStart(struct instNUGCUTLOCATORSYS_s *sys, struct NUGCUTLOCATORSYS_s *def);
void NuGCutRigidSysFixPtrs(struct NUGCUTSCENE_s *def, int base);
void NuGCutLocatorSysFixPtrs(struct NUGCUTSCENE_s *def, int base);
void NuGCutLocatorSysFixUp(struct NUGCUTLOCATORSYS_s *def);
void NuGCutRigidSysFixUp(struct NUGCUTSCENE_s *def, void *scene);
void NuGCutTriggerSysFixUp(struct NUGCUTSCENE_s *def, struct NUTRIGGERSYS_s *triggersys);
void NuGCutCharSysFixPtrs(struct NUGCUTSCENE_s *def, int base);
void instNuGCutRigidSysReset(struct instNUGCUTRIGIDSYS_s *sys, struct NUGCUTRIGIDSYS_s *def);
void instNuGCutRigidSysStart(struct instNUGCUTRIGIDSYS_s *sys, struct NUGCUTRIGIDSYS_s *def);
void NuGCutCharSysFixUp(struct NUGCUTSCENE_s *def);
void NuGCutRigidCalcMtx(struct NUGCUTRIGID_s *rigid, float time, struct numtx_s *mtx);
void instNuGCutRigidSysUpdate(struct instNUGCUTSCENE_s *scene, float time);
void instNuGCutRigidSysRender(struct instNUGCUTSCENE_s *scene, float time);
void NuGCutCharAnimProcess(struct NUGCUTCHAR_s *desc, float time, struct numtx_s *mtx, int *is_visible, unsigned int *animix, float *animrate, float *blendframes);
void instNuGCutRigidSysEnd(struct instNUGCUTSCENE_s *scene, float time);
void instNuGCutCharSysEnd(struct instNUGCUTSCENE_s *scene, float time);
void NuGCutTriggerSysFixPtrs(struct NUGCUTSCENE_s *def, int base);
struct instNUGCUTTRIGGERSYS_s *instNuCGutTriggerSysCreate(struct NUGCUTSCENE_s *def, struct instNUTRIGGERSYS_s *list, union variptr_u *heap);
struct instNUGCUTRIGIDSYS_s *instNuCGutRigidSysCreate(struct NUGCUTSCENE_s *def, struct nugscn_s *gscene, union variptr_u *heap);
struct instNUGCUTCHARSYS_s *instNuCGutCharSysCreate(struct NUGCUTSCENE_s *def, union variptr_u *heap);
struct instNUGCUTSCENE_s *instNuGCutSceneCreate(struct NUGCUTSCENE_s *def, void *gscene, struct instNUTRIGGERSYS_s *triggers, char *name, union variptr_u *heap);
void instNuGCutTriggerSysUpdate(struct instNUGCUTSCENE_s *scene, float time);
void instNuGCutTriggerSysStart(struct instNUGCUTSCENE_s *scene);
void instNuGCutSceneRender(struct instNUGCUTSCENE_s *scene);
int StateAnimEvaluate(struct NUSTATEANIM_s *anim, unsigned char *key, unsigned char *value, float time);
struct NUSTATEANIM_s *StateAnimFixPtrs(struct NUSTATEANIM_s *anim, int base);
void instNuGCutCharSysRender(struct instNUGCUTSCENE_s *scene, float time);
void instNuGCutCamSysUpdate(struct instNUGCUTSCENE_s *scene, float time);
struct NUGCUTSCENE_s *NuGCutSceneLoad(char *name, union variptr_u *heap, union variptr_u *heapend);
void instNuGCutSceneStart(struct instNUGCUTSCENE_s *scene);
void instNuGCutSceneEnd(struct instNUGCUTSCENE_s *scene);
int NuGCutLocatorIsVisble(struct NUGCUTLOCATOR_s *data, float time, struct nuanimtime_s *atime, float *value);
void instNuGCutLocatorUpdate(struct instNUGCUTSCENE_s *scene, struct NUGCUTLOCATORSYS_s *def, struct instNUGCUTLOCATOR_s *locator, struct NUGCUTLOCATOR_s *data, float time, struct numtx_s *mtx);

#endif /* GAMELIB_GCUTSCN_H */
