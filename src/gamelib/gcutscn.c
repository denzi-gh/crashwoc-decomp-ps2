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

#define NUGCUTSCENE_RUNNING  0x002
#define NUGCUTSCENE_DISABLED 0x100

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


void NuGCutSceneSysInit(NuGCutLocatorFn *fns) {
    locatorfns = fns;
    cutscenelist = 0;
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


int instNuGCutSceneIsFinished(NuGCutScene *scene) {
    return !(scene->flags & NUGCUTSCENE_RUNNING);
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
