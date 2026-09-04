/*
 * Unit: gamelib/gcutscn
 *
 * Functions:
 *   0x001aae88 NuGCutSceneLoad
 *   0x001aaff8 instNuGCutSceneUpdate
 *   0x001ab538 instNuGCutCamSysUpdate
 *   0x001abaa8 DebugRndrEmitter
 *   0x001abe00 NuGCutLocatorCalcMtx
 *   0x001ac018 instNuGCutDebrisLocatorUpdate
 *   0x001ac380 instNuCGutRigidSysCreate
 *   0x001ac530 NuGCutRigidCalcMtx
 *   0x001ac7a0 instNuGCutRigidSysUpdate
 *   0x001acaa0 instNuGCutRigidSysRender
 *   0x001acd30 NuGCutCharAnimProcess
 *   0x001ad0b8 NuGCutSceneSysInit
 *   0x001ad0c8 NuGCutSceneSysUpdate
 *   0x001ad130 NuGCutSceneSysRender
 *   0x001ad188 NuSetCutSceneCharacterRenderFn
 *   0x001ad190 NuSetCutSceneCharacterReleaseFn
 *   0x001ad198 NuSetCutSceneFindCharactersFn
 *   0x001ad1a0 NuSetCutSceneCharacterCreateDataFn
 *   0x001ad1a8 NuSetCutSceneRigidCollisionCheckFn
 *   0x001ad1b0 instNuGCutSceneCharGetStartMtx
 *   0x001ad2d0 NuGCutSceneFixUp
 *   0x001ad358 instNuGCutSceneCreate
 *   0x001ad4c0 instNuGCutSceneFind
 *   0x001ad518 instNuGCutSceneDestroy
 *   0x001ad580 instNuGCutSceneIsFinished
 *   0x001ad598 instNuGCutSceneStart
 *   0x001ad630 instNuGCutSceneEnd
 *   0x001ad6c8 instNuGCutSceneReset
 *   0x001ad720 instNuGCutSceneCreateCamTgtArray
 *   0x001ad788 instNuGCutSceneAddCamTgt
 *   0x001ad7e0 instNuGCutSceneSetPos
 *   0x001ad830 instNuGCutSceneTranslate
 *   0x001ad8a0 instNuGCutSceneRotateY
 *   0x001ad8f0 instNuGCutSceneSetMtx
 *   0x001ad9a0 instNuGCutSceneChain
 *   0x001ad9a8 instNuGCutSceneSetEndCallback
 *   0x001ad9b0 instNuGCutSceneEnable
 *   0x001ad9c8 instNuGCutSceneDisable
 *   0x001ad9d8 NuGCutLocatorIsVisble
 *   0x001ada90 CalculateLocatorDirection
 *   0x001adb98 instNuGCutLocatorUpdate
 *   0x001adc20 NuGCutCamsSysFixPtrs
 *   0x001adc90 instNuGCutSceneCalculateCentre
 *   0x001add20 instNuCGutCamSysCreate
 *   0x001adde0 instNuGCutCamSysStart
 *   0x001ade40 NuGCutLocatorSysFixPtrs
 *   0x001adf50 NuGCutLocatorSysFixUp
 *   0x001ae098 NuCGutLocatorSysCreateInst
 *   0x001ae138 instNuGCutLocatorSysEnd
 *   0x001ae170 instNuGCutLocatorSysStart
 *   0x001ae1a8 NuGCutRigidSysFixPtrs
 *   0x001ae290 NuGCutRigidSysFixUp
 *   0x001ae3b0 instNuGCutRigidSysEnd
 *   0x001ae510 instNuGCutRigidSysReset
 *   0x001ae630 instNuGCutRigidSysStart
 *   0x001ae668 NuGCutCharSysFixPtrs
 *   0x001ae770 NuGCutCharSysFixUp
 *   0x001ae798 instNuCGutCharSysCreate
 *   0x001ae8f0 instNuGCutCharSysEnd
 *   0x001ae9e8 NuGCutTriggerSysFixPtrs
 *   0x001aeaa0 NuGCutTriggerSysFixUp
 *   0x001aebb0 instNuCGutTriggerSysCreate
 *   0x001aec50 instNuGCutTriggerSysUpdate
 *   0x001aed38 instNuGCutTriggerSysStart
 *   0x001aed80 instNuGCutSceneRender
 *   0x001aee28 StateAnimEvaluate
 *   0x001aeef8 StateAnimFixPtrs
 *   0x001aef38 instNuGCutLocatorSysUpdate
 *   0x001af030 instNuGCutCharSysRender
 */

typedef struct NuMtx {
    float m[4][4];
} NuMtx;

typedef struct NuVec4 {
    float x;
    float y;
    float z;
    float w;
} NuVec4;

typedef struct NuGCutScene NuGCutScene;
typedef struct NuGCutSceneDef NuGCutSceneDef;
typedef struct NuGCutCharDesc NuGCutCharDesc;
typedef struct NuGCutChar NuGCutChar;
typedef struct NuGCutRigid NuGCutRigid;
typedef struct NuGCutCamSys NuGCutCamSys;
typedef struct NuGCutRigidSys NuGCutRigidSys;
typedef struct NuGCutCharSys NuGCutCharSys;
typedef struct NuGCutLocatorSys NuGCutLocatorSys;
typedef struct NuGCutTriggerSys NuGCutTriggerSys;

typedef void (*NuGCutSceneEndFn)(NuGCutScene *scene);

struct NuGCutScene {
    NuGCutScene *next;              /* 0x00 */
    NuGCutScene *prev;              /* 0x04 */
    char name[16];                  /* 0x08 */
    NuMtx mtx;                      /* 0x18 */
    NuGCutSceneDef *def;            /* 0x58 */
    NuVec4 centre;                  /* 0x5C */
    int flags;                      /* 0x6C */
    float time;                     /* 0x70 */
    float speed;                    /* 0x74 */
    NuGCutCamSys *camsys;           /* 0x78 */
    NuGCutRigidSys *rigidsys;       /* 0x7C */
    NuGCutCharSys *charsys;         /* 0x80 */
    NuGCutLocatorSys *locatorsys;   /* 0x84 */
    NuGCutTriggerSys *triggersys;   /* 0x88 */
    NuGCutScene *chain;             /* 0x8C */
    NuGCutSceneEndFn endcallback;   /* 0x90 */
};                                  /* 0x94 */

#define NUGCUTSCENE_RUNNING   0x002
#define NUGCUTSCENE_MATRIXSET 0x010
#define NUGCUTSCENE_DISABLED  0x100
#define NUGCUTSCENE_UPDATED   0x200

typedef struct NuGCutLocatorFn {
    const char *name;
    void (*func)();
} NuGCutLocatorFn;

typedef void (*NuCutSceneCharacterRenderFn)(NuGCutScene *scene, NuGCutSceneDef *def,
                                            NuGCutChar *character, NuGCutCharDesc *desc,
                                            float time);
typedef void (*NuCutSceneCharacterReleaseFn)(NuGCutChar *character);
typedef void (*NuCutSceneFindCharactersFn)(void);
typedef void (*NuCutSceneCharacterCreateDataFn)(NuGCutCharDesc *desc, NuGCutChar *character,
                                                void *heap);
typedef void (*NuCutSceneRigidCollisionCheckFn)(NuGCutRigid *rigid, NuMtx *mtx);

extern NuCutSceneCharacterRenderFn NuCutSceneCharacterRender;
extern NuCutSceneCharacterReleaseFn NuCutSceneCharacterRelease;
extern NuCutSceneFindCharactersFn NuCutSceneFindCharacters;
extern NuCutSceneCharacterCreateDataFn NuCutSceneCharacterCreateData;
extern NuCutSceneRigidCollisionCheckFn NuCutSceneRigidCollisionCheck;
extern NuGCutLocatorFn *locatorfns;

extern NuGCutScene *D_0063322C;
#define cutscenelist D_0063322C

typedef struct NuGCutLocator {
    void *object;                 /* 0x00 */
    int flags;                    /* 0x04 */
} NuGCutLocator;

struct NuGCutRigid {
    void *object;                 /* 0x00 */
    int flags;                    /* 0x04 */
    unsigned char state;          /* 0x08 */
    char pad[3];                  /* 0x09 */
};

typedef struct NuGCutTrigger {
    unsigned char state;          /* 0x00 */
    char pad[3];                  /* 0x01 */
} NuGCutTrigger;

typedef struct NuGCutLocatorDef {
    char pad[8];                  /* 0x00 */
    unsigned char count;          /* 0x08 */
} NuGCutLocatorDef;

typedef struct NuGCutRigidDef {
    char pad[4];                  /* 0x00 */
    unsigned short count;         /* 0x04 */
} NuGCutRigidDef;

typedef struct NuGCutTriggerDef {
    int count;                    /* 0x00 */
} NuGCutTriggerDef;

struct NuGCutLocatorSys {
    NuGCutLocator *locators;      /* 0x00 */
};

struct NuGCutRigidSys {
    NuGCutRigid *rigids;          /* 0x00 */
};

struct NuGCutTriggerSys {
    char pad[4];                  /* 0x00 */
    NuGCutTrigger *triggers;      /* 0x04 */
};

struct NuGCutSceneDef {
    char pad0[0x14];              /* 0x00 */
    NuGCutRigidDef *rigiddef;     /* 0x14 */
    char pad18[0x24 - 0x18];      /* 0x18 */
    NuGCutTriggerDef *triggerdef; /* 0x24 */
};

typedef struct NuGCutCamTgt {
    void *object;                 /* 0x00 */
    float weight;                 /* 0x04 */
    float time;                   /* 0x08 */
    char flags;                   /* 0x0C */
    char pad[3];                  /* 0x0D */
} NuGCutCamTgt;

typedef struct NuGCutCam {
    unsigned char flags;          /* 0x00 */
    unsigned char state;          /* 0x01 */
    char pad[2];                  /* 0x02 */
} NuGCutCam;

typedef struct NuGCutCamDef {
    int count;                    /* 0x00 */
    char pad[0x10 - 0x4];         /* 0x04 */
    unsigned char field10;        /* 0x10 */
} NuGCutCamDef;

struct NuGCutCamSys {
    NuGCutCamTgt *targets;        /* 0x00 */
    NuGCutCam *cams;              /* 0x04 */
    unsigned char field8;         /* 0x08 */
    unsigned char field9;         /* 0x09 */
    unsigned char fieldA;         /* 0x0A */
    unsigned char max;            /* 0x0B */
    unsigned char count;          /* 0x0C */
};

typedef struct StateAnim {
    int count;                    /* 0x00 */
    float *times;                 /* 0x04 */
    unsigned char *values;        /* 0x08 */
} StateAnim;

#define NuFixPtr(p, base) ((p) ? (void *)((char *)(p) + (base)) : 0)

extern void NuMtxSetTranslation(NuMtx *mtx, NuVec4 *pos);
extern void NuMtxRotateY(NuMtx *mtx, float angle);
extern int strcasecmp(const char *s1, const char *s2);

void instNuGCutSceneCalculateCentre(NuGCutScene *scene, NuMtx *mtx);
void instNuGCutSceneRender(NuGCutScene *scene);
void instNuGCutRigidSysReset(NuGCutRigidSys *sys, NuGCutRigidDef *def);


void NuGCutSceneSysInit(NuGCutLocatorFn *fns) {
    locatorfns = fns;
    cutscenelist = 0;
}


void NuGCutSceneSysRender(void) {
    NuGCutScene *scene;

    for (scene = cutscenelist; scene; scene = scene->next) {
        instNuGCutSceneRender(scene);
        scene->flags &= ~NUGCUTSCENE_UPDATED;
    }
}


void NuSetCutSceneCharacterRenderFn(NuCutSceneCharacterRenderFn fn) {
    NuCutSceneCharacterRender = fn;
}


void NuSetCutSceneCharacterReleaseFn(NuCutSceneCharacterReleaseFn fn) {
    NuCutSceneCharacterRelease = fn;
}


void NuSetCutSceneFindCharactersFn(NuCutSceneFindCharactersFn fn) {
    NuCutSceneFindCharacters = fn;
}


void NuSetCutSceneCharacterCreateDataFn(NuCutSceneCharacterCreateDataFn fn) {
    NuCutSceneCharacterCreateData = fn;
}


void NuSetCutSceneRigidCollisionCheckFn(NuCutSceneRigidCollisionCheckFn fn) {
    NuCutSceneRigidCollisionCheck = fn;
}


NuGCutScene *instNuGCutSceneFind(const char *name) {
    NuGCutScene *scene;

    for (scene = cutscenelist; scene; scene = scene->next) {
        if (strcasecmp(name, scene->name) == 0) {
            return scene;
        }
    }
    return 0;
}


int instNuGCutSceneIsFinished(NuGCutScene *scene) {
    return !(scene->flags & NUGCUTSCENE_RUNNING);
}


void instNuGCutSceneReset(NuGCutScene *scene) {
    scene->flags &= ~NUGCUTSCENE_RUNNING;
    scene->flags &= ~0x1;
    scene->time = 1.0f;
    if (scene->rigidsys) {
        instNuGCutRigidSysReset(scene->rigidsys, scene->def->rigiddef);
    }
}


int instNuGCutSceneAddCamTgt(NuGCutScene *scene, void *object, char flags, float weight, float time) {
    NuGCutCamSys *camsys = scene->camsys;

    if (camsys && camsys->count < camsys->max) {
        NuGCutCamTgt *tgt = &camsys->targets[camsys->count++];
        tgt->object = object;
        tgt->weight = weight;
        tgt->time = time;
        tgt->flags = flags;
        return 1;
    }
    return 0;
}


void instNuGCutSceneSetPos(NuGCutScene *scene, NuVec4 *pos) {
    scene->flags |= NUGCUTSCENE_MATRIXSET;
    NuMtxSetTranslation(&scene->mtx, pos);
    instNuGCutSceneCalculateCentre(scene, &scene->mtx);
}


void instNuGCutSceneRotateY(NuGCutScene *scene, float angle) {
    scene->flags |= NUGCUTSCENE_MATRIXSET;
    NuMtxRotateY(&scene->mtx, angle);
    instNuGCutSceneCalculateCentre(scene, &scene->mtx);
}


void instNuGCutSceneChain(NuGCutScene *scene, NuGCutScene *chain) {
    scene->chain = chain;
}


void instNuGCutSceneSetEndCallback(NuGCutScene *scene, NuGCutSceneEndFn callback) {
    scene->endcallback = callback;
}


void instNuGCutSceneEnable(NuGCutScene *scene) {
    scene->flags &= ~NUGCUTSCENE_DISABLED;
}


void instNuGCutSceneDisable(NuGCutScene *scene) {
    scene->flags |= NUGCUTSCENE_DISABLED;
}


void instNuGCutCamSysStart(NuGCutCamSys *camsys, NuGCutCamDef *def) {
    unsigned int i;

    camsys->field8 = 0;
    camsys->field9 = def->field10;
    camsys->fieldA = 0;
    for (i = 0; i < def->count; i++) {
        NuGCutCam *cam = &camsys->cams[i];
        cam->flags &= ~0x2;
        cam->state = 0;
    }
}


void instNuGCutLocatorSysEnd(NuGCutLocatorSys *sys, NuGCutLocatorDef *def) {
    unsigned int i;

    for (i = 0; i < def->count; i++) {
        NuGCutLocator *loc = &sys->locators[i];
        loc->object = 0;
    }
}


void instNuGCutLocatorSysStart(NuGCutLocatorSys *sys, NuGCutLocatorDef *def) {
    unsigned int i;

    for (i = 0; i < def->count; i++) {
        NuGCutLocator *loc = &sys->locators[i];
        loc->object = 0;
    }
}


void instNuGCutRigidSysStart(NuGCutRigidSys *sys, NuGCutRigidDef *def) {
    unsigned int i;

    for (i = 0; i < def->count; i++) {
        NuGCutRigid *rigid = &sys->rigids[i];
        rigid->state = 0;
    }
}


void NuGCutCharSysFixUp(NuGCutSceneDef *def) {
    if (NuCutSceneFindCharacters) {
        NuCutSceneFindCharacters();
    }
}


void instNuGCutTriggerSysStart(NuGCutScene *scene) {
    NuGCutSceneDef *def = scene->def;
    NuGCutTriggerSys *sys = scene->triggersys;
    NuGCutTriggerDef *tdef = def->triggerdef;
    int i;

    for (i = 0; i < tdef->count; i++) {
        NuGCutTrigger *trigger = &sys->triggers[i];
        trigger->state = 0;
    }
}


StateAnim *StateAnimFixPtrs(StateAnim *anim, int base) {
    anim = NuFixPtr(anim, base);
    if (anim) {
        anim->times = NuFixPtr(anim->times, base);
        anim->values = NuFixPtr(anim->values, base);
    }
    return anim;
}
