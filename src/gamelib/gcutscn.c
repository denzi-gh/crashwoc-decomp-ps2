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

#include "nu_libc.h"

#include "nu3d/nucamera.h"
#include "nu3d/nugobj.h"
#include "nu3d/nugscn.h"
#include "nucore/nuerror.h"
#include "numath/nufloat.h"
#include "numath/numathtypes.h"
#include "numath/numtx.h"
#include "numath/nuquat.h"
#include "numath/nutrig.h"
#include "numath/nuvec.h"
#include "nu3d/nuanim.h"
#include "gamelib/gcutscn.h"

#include "creature.h"

#define NUINSTANCE_VISIBLE 0x1

extern NuCutSceneCharacterRenderFn NuCutSceneCharacterRender;
extern NuCutSceneCharacterReleaseFn NuCutSceneCharacterRelease;
extern NuCutSceneFindCharactersFn NuCutSceneFindCharacters;
extern NuCutSceneCharacterCreateDataFn NuCutSceneCharacterCreateData;
extern NuCutSceneRigidCollisionCheckFn NuCutSceneRigidCollisionCheck;
extern struct NUGCUTLOCFNDATA_s *locatorfns;

extern struct instNUGCUTSCENE_s *D_0063322C;
#define cutscenelist D_0063322C

extern int D_0062F7CC;
extern int D_0062F7D0;
extern int D_0062F7D4;
#define cutscene_rndr_rigids   D_0062F7CC
#define cutscene_rndr_chars    D_0062F7D0
#define cutscene_rndr_locators D_0062F7D4

#define NUANIM_CURVESET_ROTATE 0x1
#define NUANIM_CURVESET_SCALE  0x8

#define NUANIM_CURVE_VALUE   6
#define NUANIM_CURVE_VISIBLE 7

#define NUGCUTLOCATOR_DEBRIS 0x1
#define NUGCUTLOCATOR_FUNC   0x2
#define NUGCUTLOCATOR_VISIBLE 0x8
#define NUGCUTLOCATOR_NOFN   0xFFFF

#define NUGCUTRIGID_VISIBLE  0x1
#define NUGCUTRIGID_ANIMATED 0x2
#define NUGCUTRIGID_FOUND    0x4

typedef struct NuGCutTriggerTarget {
    char pad[2];                  /* 0x00 */
    unsigned char flags;          /* 0x02 */
    char pad3;                    /* 0x03 */
} NuGCutTriggerTarget;

struct NuGCutTriggerList {
    char pad[0xC];                /* 0x00 */
    NuGCutTriggerTarget *targets; /* 0x0C */
};

#define NUGCUTCAM_TARGETED 0x2

#define NUGCUTCAM_ANIMATED 0x1

#define NuFixPtr(p, base) ((p) ? (void *)((char *)(p) + (base)) : 0)

/* Round a heap pointer up to the next 16-byte boundary. */
#define ALIGN16(p) ((char *)(((unsigned int)(p) + 15) & ~15))

void instNuGCutSceneEnd(struct instNUGCUTSCENE_s *scene);


void NuGCutSceneSysInit(struct NUGCUTLOCFNDATA_s *fns) {
    locatorfns = fns;
    cutscenelist = 0;
}


void NuGCutSceneSysUpdate(int paused) {
    struct instNUGCUTSCENE_s *scene;

    for (scene = cutscenelist; scene; scene = scene->next) {
        if (!(scene->flags & NUGCUTSCENE_UPDATED)) {
            instNuGCutSceneUpdate(scene, paused);
            scene->flags |= NUGCUTSCENE_UPDATED;
        }
    }
}


void NuGCutSceneSysRender(void) {
    struct instNUGCUTSCENE_s *scene;

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


int instNuGCutSceneCharGetStartMtx(struct instNUGCUTSCENE_s *scene, char *name, struct numtx_s *mtx) {
    struct NUGCUTCHARSYS_s *chardef = scene->def->chardef;
    int i;

    for (i = 0; i < chardef->count; i++) {
        struct NUGCUTCHAR_s *desc = &chardef->chars[i];

        if (strcasecmp(name, desc->name) == 0) {
            *mtx = desc->mtx;
            return 1;
        }
    }
    return 0;
}

void NuGCutSceneFixUp(struct NUGCUTSCENE_s *def, void *gscene, void *triggers) {
    if (def) {
        if (def->rigiddef) {
            NuGCutRigidSysFixUp(def, gscene);
        }
        if (def->chardef) {
            NuGCutCharSysFixUp(def);
        }
        if (def->locatordef) {
            NuGCutLocatorSysFixUp(def->locatordef);
        }
        if (def->triggerdef && triggers) {
            NuGCutTriggerSysFixUp(def, triggers);
        }
    }
}


struct instNUGCUTSCENE_s *instNuGCutSceneFind(const char *name) {
    struct instNUGCUTSCENE_s *scene;

    for (scene = cutscenelist; scene; scene = scene->next) {
        if (strcasecmp(name, scene->name) == 0) {
            return scene;
        }
    }
    return 0;
}


void instNuGCutSceneDestroy(struct instNUGCUTSCENE_s *scene) {
    if (scene->flags & NUGCUTSCENE_RUNNING) {
        instNuGCutSceneEnd(scene);
    }
    if (scene->next) {
        scene->next->prev = scene->prev;
    }
    if (scene->prev) {
        scene->prev->next = scene->next;
    } else {
        cutscenelist = scene->next;
    }
}


int instNuGCutSceneIsFinished(struct instNUGCUTSCENE_s *scene) {
    return !(scene->flags & NUGCUTSCENE_RUNNING);
}


inline void instNuGCutSceneStart(struct instNUGCUTSCENE_s *scene) {
    scene->time = 1.0f;
    scene->flags |= NUGCUTSCENE_RUNNING;
    scene->flags &= ~NUGCUTSCENE_STARTED;
    if (scene->camsys) {
        instNuGCutCamSysStart(scene->camsys, scene->def->camdef);
    }
    if (scene->rigidsys) {
        instNuGCutRigidSysStart(scene->rigidsys, scene->def->rigiddef);
    }
    if (scene->locatorsys) {
        instNuGCutLocatorSysStart(scene->locatorsys, scene->def->locatordef);
    }
    if (scene->triggersys) {
        instNuGCutTriggerSysStart(scene);
    }
}


inline void instNuGCutSceneEnd(struct instNUGCUTSCENE_s *scene) {
    struct NUGCUTSCENE_s *def = scene->def;

    scene->time = def->duration + 1.0f;
    scene->flags &= ~NUGCUTSCENE_RUNNING;
    if (def->rigiddef) {
        instNuGCutRigidSysEnd(scene, scene->time);
    }
    if (scene->charsys) {
        instNuGCutCharSysEnd(scene, scene->time);
    }
    if (scene->locatorsys) {
        instNuGCutLocatorSysEnd(scene->locatorsys, def->locatordef, scene->time);
    }
}


#define NUGCUTSCENE_AUTOSTART 0x008
#define NUGCUTSCENE_CLIPBOUNDS 0x020
#define NUGCUTSCENE_CLIPDIST  0x040

extern int D_0062F7C8;
#define cutscene_rndr_cameras D_0062F7C8

extern struct numtx_s numtx_identity;

static inline void instNuGCutSceneClipTest(struct instNUGCUTSCENE_s *scene) {
    struct NUGCUTSCENE_s *def = scene->def;

    scene->flags |= NUGCUTSCENE_VISIBLE;
    if (def->bounds == 0) {
        return;
    }
    if (scene->flags & NUGCUTSCENE_CLIPDIST) {
        if (NuCameraDistSqr(&scene->centre) > *(float *)scene->pad68) {
            scene->flags &= ~NUGCUTSCENE_VISIBLE;
            return;
        }
    }
    if (!(scene->flags & NUGCUTSCENE_CLIPBOUNDS)) {
        return;
    }
    if (scene->flags & NUGCUTSCENE_MATRIXSET) {
        if (NuCameraClipTestExtents(&def->bounds->min, &def->bounds->max, &scene->mtx) == 0) {
            scene->flags &= ~NUGCUTSCENE_VISIBLE;
        }
    } else if (NuCameraClipTestExtents(&def->bounds->min, &def->bounds->max, &numtx_identity) == 0) {
        scene->flags &= ~NUGCUTSCENE_VISIBLE;
    }
}


void instNuGCutSceneUpdate(struct instNUGCUTSCENE_s *scene, int paused) {
    struct NUGCUTSCENE_s *def = scene->def;
    NuGCutSceneEndFn callback;
    float time;
    float overrun;

    if (scene->flags & NUGCUTSCENE_DISABLED) {
        return;
    }
    if (!(scene->flags & NUGCUTSCENE_STARTED) && (scene->flags & NUGCUTSCENE_AUTOSTART)) {
        instNuGCutSceneStart(scene);
    }
    if (!(scene->flags & NUGCUTSCENE_RUNNING)) {
        return;
    }
    instNuGCutSceneClipTest(scene);
    if (!(scene->flags & NUGCUTSCENE_VISIBLE)) {
        return;
    }
    if (scene->flags & NUGCUTSCENE_STARTED) {
        if (paused == 0) {
            time = scene->time + scene->speed;
            scene->time = time;
            if (time < 1.0f) {
                scene->time = 1.0f;
            } else if (time >= (def->duration + 1.0f) - 1.0f) {
                if (scene->endcallback) {
                    callback = scene->endcallback;
                    scene->endcallback = 0;
                    callback(scene);
                }
                if (scene->chain) {
                    overrun = scene->time - ((def->duration + 1.0f) - 1.0f);
                    if (def->duration < overrun) {
                        overrun = 0.0f;
                    }
                    instNuGCutSceneEnd(scene);
                    instNuGCutSceneStart(scene->chain);
                    scene->chain->time = scene->chain->time + overrun;
                    scene->chain->flags |= NUGCUTSCENE_STARTED;
                    instNuGCutSceneClipTest(scene->chain);
                    scene->chain->flags |= NUGCUTSCENE_UPDATED;
                    scene->chain = 0;
                    return;
                }
                if (scene->flags & NUGCUTSCENE_LOOP) {
                    overrun = scene->time - ((def->duration + 1.0f) - 1.0f);
                    if (def->duration < overrun) {
                        overrun = 0.0f;
                    }
                    instNuGCutSceneStart(scene);
                    scene->time = scene->time + overrun;
                    scene->flags |= NUGCUTSCENE_STARTED;
                } else {
                    instNuGCutSceneEnd(scene);
                }
            }
        }
    } else {
        scene->flags |= NUGCUTSCENE_STARTED;
    }
    if (cutscene_rndr_cameras && scene->camsys) {
        instNuGCutCamSysUpdate(scene, scene->time);
    }
    if (scene->triggersys) {
        instNuGCutTriggerSysUpdate(scene, scene->time);
    }
    if (cutscene_rndr_rigids && scene->rigidsys) {
        instNuGCutRigidSysUpdate(scene, scene->time);
    }
}


void instNuGCutSceneReset(struct instNUGCUTSCENE_s *scene) {
    scene->flags &= ~NUGCUTSCENE_RUNNING;
    scene->flags &= ~NUGCUTSCENE_STARTED;
    scene->time = 1.0f;
    if (scene->rigidsys) {
        instNuGCutRigidSysReset(scene->rigidsys, scene->def->rigiddef);
    }
}


void instNuGCutSceneCreateCamTgtArray(struct instNUGCUTSCENE_s *scene, int count, char **heap) {
    struct instNUGCUTCAMSYS_s *camsys = scene->camsys;

    if (camsys && count) {
        camsys->max = count;
        *heap = ALIGN16(*heap);
        camsys->targets = (struct NUGCUTCAMTGT_s *)*heap;
        *heap += count * sizeof(struct NUGCUTCAMTGT_s);
        memset(camsys->targets, 0, count * sizeof(struct NUGCUTCAMTGT_s));
    }
}


int instNuGCutSceneAddCamTgt(struct instNUGCUTSCENE_s *scene, struct nuvec_s *object, char camid, float frame, float nframes) {
    struct instNUGCUTCAMSYS_s *camsys = scene->camsys;

    if (camsys && camsys->count < camsys->max) {
        struct NUGCUTCAMTGT_s *tgt = &camsys->targets[camsys->count++];
        tgt->object = object;
        tgt->frame = frame;
        tgt->nframes = nframes;
        tgt->camid = camid;
        return 1;
    }
    return 0;
}


void instNuGCutSceneSetPos(struct instNUGCUTSCENE_s *scene, struct nuvec4_s *pos) {
    scene->flags |= NUGCUTSCENE_MATRIXSET;
    NuMtxSetTranslation(&scene->mtx, pos);
    instNuGCutSceneCalculateCentre(scene, &scene->mtx);
}


void instNuGCutSceneTranslate(struct instNUGCUTSCENE_s *scene, struct nuvec_s *offset) {
    if (!(scene->flags & NUGCUTSCENE_MATRIXSET)) {
        scene->flags |= NUGCUTSCENE_MATRIXSET;
        NuMtxSetIdentity(&scene->mtx);
    }
    NuMtxTranslate(&scene->mtx, offset);
    instNuGCutSceneCalculateCentre(scene, &scene->mtx);
}


void instNuGCutSceneRotateY(struct instNUGCUTSCENE_s *scene, int angle) {
    scene->flags |= NUGCUTSCENE_MATRIXSET;
    NuMtxRotateY(&scene->mtx, angle);
    instNuGCutSceneCalculateCentre(scene, &scene->mtx);
}


void instNuGCutSceneSetMtx(struct instNUGCUTSCENE_s *scene, struct numtx_s *mtx) {
    scene->flags |= NUGCUTSCENE_MATRIXSET;
    scene->mtx = *mtx;
    instNuGCutSceneCalculateCentre(scene, &scene->mtx);
}


void instNuGCutSceneChain(struct instNUGCUTSCENE_s *scene, struct instNUGCUTSCENE_s *chain) {
    scene->chain = chain;
}


void instNuGCutSceneSetEndCallback(struct instNUGCUTSCENE_s *scene, NuGCutSceneEndFn callback) {
    scene->endcallback = callback;
}


void instNuGCutSceneEnable(struct instNUGCUTSCENE_s *scene) {
    scene->flags &= ~NUGCUTSCENE_DISABLED;
}


void instNuGCutSceneDisable(struct instNUGCUTSCENE_s *scene) {
    scene->flags |= NUGCUTSCENE_DISABLED;
}


inline int NuGCutLocatorIsVisble(struct NUGCUTLOCATOR_s *data, float time, struct nuanimtime_s *atime, float *value) {
    struct nuanimdata2_s *anim = data->anim;
    int visible;

    if (anim) {
        union nuanimcurve2_u *curves = anim->curves;
        signed char *curveflags = anim->curveflags;

        visible = NuAnimCurve2CalcVal(&curves[NUANIM_CURVE_VISIBLE], atime,
                                      curveflags[NUANIM_CURVE_VISIBLE]);
        if (visible && value) {
            *value = NuAnimCurve2CalcVal(&curves[NUANIM_CURVE_VALUE], atime,
                                         curveflags[NUANIM_CURVE_VALUE]);
        }
    } else {
        visible = data->flags & NUGCUTLOCATOR_VISIBLE;
        if (visible && value) {
            *value = data->value;
        }
    }
    return visible;
}


extern int D_0062F7D8;
#define cutscene_rndr_emitters D_0062F7D8

extern void NuRndrLine3d(struct nuvtx_tc1_s *line, void *mtl, struct numtx_s *mtx);

void DebugRndrEmitter(struct nuvec_s *pos, short rotz, short roty) {
    struct nuvec_s tip = { 0.0f, 2.0f, 0.0f };
    struct nuvec_s left = { 0.2f, 1.8f, 0.0f };
    struct nuvec_s right = { -0.2f, 1.8f, 0.0f };
    struct nuvec_s front = { 0.0f, 1.8f, 0.2f };
    struct nuvec_s back = { 0.0f, 1.8f, -0.2f };
    struct nuvtx_tc1_s line[2];

    if (cutscene_rndr_emitters) {
        line[1].diffuse = -1;
        line[0].diffuse = -1;
        NuVecRotateZ(&tip, &tip, rotz);
        NuVecRotateY(&tip, &tip, roty);
        NuVecAdd(&tip, &tip, pos);
        NuVecRotateZ(&left, &left, rotz);
        NuVecRotateY(&left, &left, roty);
        NuVecAdd(&left, &left, pos);
        NuVecRotateZ(&right, &right, rotz);
        NuVecRotateY(&right, &right, roty);
        NuVecAdd(&right, &right, pos);
        NuVecRotateZ(&front, &front, rotz);
        NuVecRotateY(&front, &front, roty);
        NuVecAdd(&front, &front, pos);
        NuVecRotateZ(&back, &back, rotz);
        NuVecRotateY(&back, &back, roty);
        NuVecAdd(&back, &back, pos);
        line[0].pnt = *pos;
        line[1].pnt = tip;
        NuRndrLine3d(line, 0, 0);
        line[0].pnt = left;
        line[1].pnt = tip;
        NuRndrLine3d(line, 0, 0);
        line[0].pnt = right;
        line[1].pnt = tip;
        NuRndrLine3d(line, 0, 0);
        line[0].pnt = front;
        line[1].pnt = tip;
        NuRndrLine3d(line, 0, 0);
        line[0].pnt = back;
        line[1].pnt = tip;
        NuRndrLine3d(line, 0, 0);
    }
}


void NuGCutLocatorCalcMtx(struct NUGCUTLOCATOR_s *data, float time, struct numtx_s *mtx, struct nuanimtime_s *atime) {
    struct nuanimdata2_s *anim = data->anim;
    struct numtx_s *locmtx = (struct numtx_s *)data->pad;
    struct nuvec_s t;
    struct nuvec_s r;
    struct nuangvec_s ang;

    if (anim) {
        union nuanimcurve2_u *curves = anim->curves;
        signed char *curveflags = anim->curveflags;
        int setflags = anim->curvesetflags[0];

        if (setflags & NUANIM_CURVESET_ROTATE) {
            r.x = NuAnimCurve2CalcVal(&curves[3], atime, curveflags[3]);
            r.y = NuAnimCurve2CalcVal(&curves[4], atime, curveflags[4]);
            r.z = NuAnimCurve2CalcVal(&curves[5], atime, curveflags[5]);
            ang.x = (int)(r.x * 10430.378f);
            ang.y = (int)(r.y * 10430.378f);
            ang.z = (int)(r.z * 10430.378f);
            NuMtxSetRotateXYZ(mtx, &ang);
        } else {
            NuMtxSetIdentity(mtx);
        }
        t.x = NuAnimCurve2CalcVal(&curves[0], atime, curveflags[0]);
        t.y = NuAnimCurve2CalcVal(&curves[1], atime, curveflags[1]);
        t.z = NuAnimCurve2CalcVal(&curves[2], atime, curveflags[2]);
        NuMtxTranslate(mtx, &t);
        mtx->_02 = -mtx->_02;
        mtx->_12 = -mtx->_12;
        mtx->_20 = -mtx->_20;
        mtx->_21 = -mtx->_21;
        mtx->_23 = -mtx->_23;
        mtx->_32 = -mtx->_32;
        NuMtxTranslate(mtx, (struct nuvec_s *)&locmtx->_30);
    } else {
        *mtx = *locmtx;
    }
}


inline void CalculateLocatorDirection(int direction, struct numtx_s *mtx, struct nuvec_s *dir) {
    int rotate = 0;

    switch (direction) {
    case 0:
    case 1:
        dir->x = dir->z = 0.0f;
        dir->y = 1.0f;
        break;
    case 2:
        dir->x = dir->z = 0.0f;
        dir->y = -1.0f;
        break;
    case 3:
        dir->x = -1.0f;
        dir->y = 0.0f;
        dir->z = 0.0f;
        rotate = 1;
        break;
    case 4:
        dir->x = 1.0f;
        dir->y = 0.0f;
        dir->z = 0.0f;
        rotate = 1;
        break;
    case 5:
        dir->x = 0.0f;
        dir->y = -1.0f;
        dir->z = 0.0f;
        rotate = 1;
        break;
    case 6:
        dir->x = 0.0f;
        dir->y = 1.0f;
        dir->z = 0.0f;
        rotate = 1;
        break;
    case 7:
        dir->x = 0.0f;
        dir->y = 0.0f;
        dir->z = 1.0f;
        rotate = 1;
        break;
    case 8:
        dir->x = 0.0f;
        dir->y = 0.0f;
        dir->z = -1.0f;
        rotate = 1;
        break;
    }
    if (rotate) {
        NuVecMtxRotate(dir, dir, mtx);
    }
}


extern float NuFsqrt(float x);
extern void AddFiniteShotDebrisEffect(int *key, int type, struct nuvec_s *pos, int count);
extern void DebrisEmitterOrientation(int key, short rotz, short roty);
extern void AddVariableShotDebrisEffect(int type, struct nuvec_s *pos, int count, short rotz, short roty);

#define NUGCUTLOCATOR_PIVOT  0x4
#define NUGCUTLOCATOR_FINITE 0x10

void instNuGCutDebrisLocatorUpdate(struct NUGCUTLOCATORSYS_s *def, struct instNUGCUTLOCATOR_s *locator,
                                   struct NUGCUTLOCATOR_s *data, float time, struct numtx_s *wm) {
    struct numtx_s mtx;
    struct nuanimtime_s atime;
    struct nuvec_s dir;
    int key;
    float rate;
    int shots;
    unsigned char debtype;
    short roty;
    short rotz;

    debtype = def->types[data->type].fn;
    if (debtype != 0xFF) {
        if (data->anim) {
            NuAnimData2CalcTime(data->anim, time, &atime);
        }
        if (NuGCutLocatorIsVisble(data, time, &atime, &rate)) {
            if (rate > 0.0f) {
                if (rate < 0.0f) {
                    rate = 0.0f;
                }
                shots = 0;
                for (locator->timer += rate; locator->timer > 0.0f; locator->timer -= 1.0f) {
                    shots++;
                }
                if (shots) {
                    NuGCutLocatorCalcMtx(data, time, &mtx, &atime);
                    if (data->flags & NUGCUTLOCATOR_PIVOT) {
                        NuMtxPreTranslate(&mtx, (struct nuvec_s *)&data->pad[0x40]);
                    }
                    if (wm) {
                        NuMtxMul(&mtx, &mtx, wm);
                    }
                    CalculateLocatorDirection(*(int *)data->pad54, &mtx, &dir);
                    roty = NuAngSub(NuAtan2(dir.x, dir.z) * 10430.378f, 0x4000);
                    rotz = NuAtan2(NuFsqrt(dir.x * dir.x + dir.z * dir.z), -dir.y) * 10430.378f;
                    if (data->flags & NUGCUTLOCATOR_FINITE) {
                        key = -1;
                        AddFiniteShotDebrisEffect(&key, debtype, (struct nuvec_s *)&mtx._30, shots);
                        DebrisEmitterOrientation(key, rotz, roty);
                    } else {
                        AddVariableShotDebrisEffect(debtype, (struct nuvec_s *)&mtx._30, shots, rotz, roty);
                    }
                }
            } else {
                locator->timer = 0.0f;
            }
        }
    }
}


void instNuGCutLocatorSysUpdate(struct instNUGCUTSCENE_s *scene, float time) {
    struct NUGCUTLOCATORSYS_s *def = scene->def->locatordef;
    struct instNUGCUTLOCATORSYS_s *sys = scene->locatorsys;
    unsigned int i;

    for (i = 0; i < def->count; i++) {
        struct NUGCUTLOCATOR_s *data = &def->locators[i];

        if (data->flags & 0x1) {
            continue;
        }
        if (data->flags & 0x2) {
            continue;
        }
        if (scene->flags & NUGCUTSCENE_MATRIXSET) {
            instNuGCutLocatorUpdate(scene, def, &sys->locators[i], data, time, &scene->mtx);
        } else {
            instNuGCutLocatorUpdate(scene, def, &sys->locators[i], data, time, 0);
        }
    }
}


typedef void (*NuGCutLocatorFrameFn)(struct instNUGCUTSCENE_s *scene, struct NUGCUTLOCATORSYS_s *def, struct instNUGCUTLOCATOR_s *locator,
                                     struct NUGCUTLOCATOR_s *data, float time, struct numtx_s *mtx);

inline void instNuGCutLocatorUpdate(struct instNUGCUTSCENE_s *scene, struct NUGCUTLOCATORSYS_s *def, struct instNUGCUTLOCATOR_s *locator,
                                    struct NUGCUTLOCATOR_s *data, float time, struct numtx_s *mtx) {
    struct NUGCUTLOCATORTYPE_s *type = &def->types[data->type];

    if (type->flags & NUGCUTLOCATOR_DEBRIS) {
        instNuGCutDebrisLocatorUpdate(def, locator, data, time, mtx);
    } else if ((type->flags & NUGCUTLOCATOR_FUNC) && type->fn != NUGCUTLOCATOR_NOFN) {
        NuGCutLocatorFrameFn update = (NuGCutLocatorFrameFn)locatorfns[type->fn].func;

        update(scene, def, locator, data, time, mtx);
    }
}


void NuGCutCamsSysFixPtrs(struct NUGCUTSCENE_s *def, int base) {
    struct NUGCUTCAMSYS_s *camdef;

    if (def->camdef) {
        camdef = (struct NUGCUTCAMSYS_s *)((char *)def->camdef + base);
        def->camdef = camdef;
        camdef->cams = NuFixPtr(camdef->cams, base);
        if (camdef->anim) {
            camdef->anim = NuAnimData2FixPtrs(camdef->anim, base);
        }
        camdef->stateanim = StateAnimFixPtrs(camdef->stateanim, base);
    }
}


void instNuGCutSceneCalculateCentre(struct instNUGCUTSCENE_s *scene, struct numtx_s *mtx) {
    struct NUGCUTSCENE_s *def = scene->def;

    if (def->bounds) {
        scene->centre.x = (def->bounds->max.x + def->bounds->min.x) * 0.5f;
        scene->centre.y = (def->bounds->max.y + def->bounds->min.y) * 0.5f;
        scene->centre.z = (def->bounds->max.z + def->bounds->min.z) * 0.5f;
    } else {
        scene->centre.x = 0.0f;
        scene->centre.y = 0.0f;
        scene->centre.z = 0.0f;
    }
    if (mtx) {
        NuVecMtxTransform(&scene->centre, &scene->centre, mtx);
    }
}


struct instNUGCUTCAMSYS_s *instNuCGutCamSysCreate(struct NUGCUTCAMSYS_s *def, char **heap) {
    struct instNUGCUTCAMSYS_s *camsys = 0;

    if (def && def->count && heap) {
        camsys = (struct instNUGCUTCAMSYS_s *)ALIGN16(*heap);
        *heap = (char *)camsys + sizeof(struct instNUGCUTCAMSYS_s);
        memset(camsys, 0, sizeof(struct instNUGCUTCAMSYS_s));
        *heap = ALIGN16(*heap);
        camsys->cams = (struct instNUGCUTCAM_s *)*heap;
        *heap += def->count * sizeof(struct instNUGCUTCAM_s);
        memset(camsys->cams, 0, def->count * sizeof(struct instNUGCUTCAM_s));
    }
    return camsys;
}


void instNuGCutCamSysStart(struct instNUGCUTCAMSYS_s *camsys, struct NUGCUTCAMSYS_s *def) {
    unsigned int i;

    camsys->next_switch = 0;
    camsys->current = def->first_camera;
    camsys->next_tgt = 0;
    for (i = 0; i < def->count; i++) {
        struct instNUGCUTCAM_s *cam = &camsys->cams[i];
        cam->flags &= ~NUGCUTCAM_TARGETED;
        cam->tgt_ix = 0;
    }
}


struct instNUGCUTLOCATORSYS_s *NuCGutLocatorSysCreateInst(struct NUGCUTLOCATORSYS_s *def, char **heap) {
    struct instNUGCUTLOCATORSYS_s *sys = 0;

    if (def && def->count && heap) {
        sys = (struct instNUGCUTLOCATORSYS_s *)ALIGN16(*heap);
        *heap = (char *)sys + sizeof(struct instNUGCUTLOCATORSYS_s);
        memset(sys, 0, sizeof(struct instNUGCUTLOCATORSYS_s));
        *heap = ALIGN16(*heap);
        sys->locators = (struct instNUGCUTLOCATOR_s *)*heap;
        *heap += def->count * sizeof(struct instNUGCUTLOCATOR_s);
        memset(sys->locators, 0, def->count * sizeof(struct instNUGCUTLOCATOR_s));
    }
    return sys;
}


void instNuGCutLocatorSysEnd(struct instNUGCUTLOCATORSYS_s *sys, struct NUGCUTLOCATORSYS_s *def, float time) {
    unsigned int i;

    for (i = 0; i < def->count; i++) {
        struct instNUGCUTLOCATOR_s *loc = &sys->locators[i];
        loc->timer = 0.0f;
    }
}


void instNuGCutLocatorSysStart(struct instNUGCUTLOCATORSYS_s *sys, struct NUGCUTLOCATORSYS_s *def) {
    unsigned int i;

    for (i = 0; i < def->count; i++) {
        struct instNUGCUTLOCATOR_s *loc = &sys->locators[i];
        loc->timer = 0.0f;
    }
}


void NuGCutRigidSysFixPtrs(struct NUGCUTSCENE_s *def, int base) {
    struct NUGCUTRIGIDSYS_s *rigiddef;
    int i;

    if (def->rigiddef) {
        rigiddef = (struct NUGCUTRIGIDSYS_s *)((char *)def->rigiddef + base);
        def->rigiddef = rigiddef;
        rigiddef->rigids = NuFixPtr(rigiddef->rigids, base);
        if (rigiddef->rigids) {
            for (i = 0; i < rigiddef->count; i++) {
                struct NUGCUTRIGID_s *rigid = &rigiddef->rigids[i];

                if (rigid->name) {
                    rigid->name = def->names + (int)rigid->name - 1;
                }
                if (rigid->anim) {
                    rigid->anim = NuAnimData2FixPtrs(rigid->anim, base);
                }
                rigid->stateanim = StateAnimFixPtrs(rigid->stateanim, base);
            }
        }
    }
}


void NuGCutLocatorSysFixPtrs(struct NUGCUTSCENE_s *def, int base) {
    struct NUGCUTLOCATORSYS_s *locatordef;
    int i;

    if (def->locatordef) {
        locatordef = (struct NUGCUTLOCATORSYS_s *)((char *)def->locatordef + base);
        def->locatordef = locatordef;
        locatordef->locators = NuFixPtr(locatordef->locators, base);
        if (locatordef->locators) {
            for (i = 0; i < locatordef->count; i++) {
                struct NUGCUTLOCATOR_s *locator = &locatordef->locators[i];

                if (locator->anim) {
                    locator->anim = NuAnimData2FixPtrs(locator->anim, base);
                }
            }
        }
        locatordef->types = NuFixPtr(locatordef->types, base);
        if (locatordef->types) {
            for (i = 0; i < locatordef->ntypes; i++) {
                struct NUGCUTLOCATORTYPE_s *type = &locatordef->types[i];

                type->name = def->names + (int)type->name - 1;
            }
        }
    }
}

extern int LookupDebrisEffect(char *name);

/* Retail-owned (no Data-from-C yet). */
extern char D_00619C50[];               /* source file name */
extern char D_00619D90[];               /* debug message */
#define gcutscn_srcfile D_00619C50
#define nolocatorfn_msg D_00619D90

/* Index of the locator function called `name`, or -1. */
static inline int LookupLocatorFn(char *name) {
    int ix = 0;

    if (locatorfns != 0) {
        while (locatorfns[ix].name) {
            if (strcasecmp(name, locatorfns[ix].name) == 0) {
                return ix;
            }
            ix++;
        }
    }
    return -1;
}

/* Bind each locator type to its debris effect or update function. */
void NuGCutLocatorSysFixUp(struct NUGCUTLOCATORSYS_s *def) {
    unsigned int i;

    for (i = 0; i < def->ntypes; i++) {
        struct NUGCUTLOCATORTYPE_s *type = &def->types[i];

        if (type->flags & NUGCUTLOCATOR_DEBRIS) {
            type->fn = LookupDebrisEffect(type->name);
        } else if (type->flags & NUGCUTLOCATOR_FUNC) {
            type->fn = LookupLocatorFn(type->name);
        }
        if (type->fn == NUGCUTLOCATOR_NOFN) {
            NuDebugMsgProlog(gcutscn_srcfile, 0x4FE)(nolocatorfn_msg, type->name);
        }
    }
}

extern char D_00619DF8[];               /* debug message */
#define norigid_msg D_00619DF8

/* Find each rigid's object in the scene and point it at its locators. */
void NuGCutRigidSysFixUp(struct NUGCUTSCENE_s *def, void *scene) {
    struct NUGCUTRIGIDSYS_s *rigiddef;
    struct NUGCUTLOCATORSYS_s *locatordef;
    int i;

    rigiddef = def->rigiddef;
    locatordef = def->locatordef;
    if (scene != 0) {
        for (i = 0; i < rigiddef->count; i++) {
            struct NUGCUTRIGID_s *rigid = &rigiddef->rigids[i];

            if (NuSpecialFind(scene, &rigid->obj, rigid->name)) {
                rigid->flags |= NUGCUTRIGID_FOUND;
            } else {
                NuDebugMsgProlog(gcutscn_srcfile, 0x5D9)(norigid_msg, rigid->name);
            }
            if (rigid->nlocators != 0 && (int)rigid->locators < 0xFF) {
                rigid->first_locator = (int)rigid->locators;
                rigid->locators = &locatordef->locators[(int)rigid->locators];
            } else {
                rigid->first_locator = 0xFF;
            }
        }
    }
}

/* The scene's trigger definitions (full layout in gamelib/trigger.c). */
typedef struct NuTriggerDef {
    char *name;                   /* 0x00 */
    char pad04[0x34 - 0x04];      /* 0x04 */
} NuTriggerDef; /* 0x34 */

typedef struct NuTriggerDefHdr {
    int version;                  /* 0x00 */
    int reloc;                    /* 0x04 */
    int count;                    /* 0x08 */
    NuTriggerDef *defs;           /* 0x0C */
} NuTriggerDefHdr;

extern char D_00619E90[];               /* error message */
#define notrigger_msg D_00619E90

/* Point each cutscene trigger at the scene trigger with the same name. */
void NuGCutTriggerSysFixUp(struct NUGCUTSCENE_s *def, void *triggers) {
    NuTriggerDefHdr *triggersys = triggers;
    struct NUGCUTTRIGGERSYS_s *triggerdef;
    int i;
    int j;

    triggerdef = def->triggerdef;
    for (i = 0; i < triggerdef->count; i++) {
        struct NUGCUTTRIGGER_s *trigger = &triggerdef->triggers[i];

        for (j = 0; j < triggersys->count; j++) {
            trigger->target = -1;
            if (strcasecmp(trigger->name, triggersys->defs[j].name) == 0) {
                trigger->target = j;
                break;
            }
        }
        if (trigger->target == -1) {
            NuErrorProlog(gcutscn_srcfile, 0x8A8)(notrigger_msg, trigger->name);
        }
    }
}

void NuGCutCharSysFixPtrs(struct NUGCUTSCENE_s *def, int base) {
    struct NUGCUTCHARSYS_s *chardef;
    int i;

    if (def->chardef) {
        chardef = (struct NUGCUTCHARSYS_s *)((char *)def->chardef + base);
        def->chardef = chardef;
        chardef->chars = NuFixPtr(chardef->chars, base);
        if (chardef->chars) {
            for (i = 0; i < chardef->count; i++) {
                struct NUGCUTCHAR_s *desc = &chardef->chars[i];

                if (desc->name) {
                    desc->name = def->names + (int)desc->name - 1;
                }
                if (desc->char_anim) {
                    desc->char_anim = NuAnimData2FixPtrs(desc->char_anim, base);
                }
                if (desc->joint_anim) {
                    desc->joint_anim = NuAnimData2FixPtrs(desc->joint_anim, base);
                }
                if (desc->face_anim) {
                    desc->face_anim = NuAnimData2FixPtrs(desc->face_anim, base);
                }
            }
        }
    }
}

void instNuGCutRigidSysReset(struct instNUGCUTRIGIDSYS_s *sys, struct NUGCUTRIGIDSYS_s *def) {
    unsigned int i;

    for (i = 0; i < def->count; i++) {
        struct NUGCUTRIGID_s *data = &def->rigids[i];
        struct instNUGCUTRIGID_s *rigid = &sys->rigids[i];

        rigid->state = 0;
        if (!(data->flags & NUGCUTRIGID_FOUND)) {
            continue;
        }
        if (data->flags & NUGCUTRIGID_ANIMATED) {
            continue;
        }
        if (data->flags & NUGCUTRIGID_VISIBLE) {
            rigid->obj.special->instance->flagword |= NUINSTANCE_VISIBLE;
        } else {
            rigid->obj.special->instance->flagword &= ~NUINSTANCE_VISIBLE;
        }
        rigid->obj.special->instance->mtx = data->mtx;
    }
}


void instNuGCutRigidSysStart(struct instNUGCUTRIGIDSYS_s *sys, struct NUGCUTRIGIDSYS_s *def) {
    unsigned int i;

    for (i = 0; i < def->count; i++) {
        struct instNUGCUTRIGID_s *rigid = &sys->rigids[i];
        rigid->state = 0;
    }
}


void NuGCutCharSysFixUp(struct NUGCUTSCENE_s *def) {
    if (NuCutSceneFindCharacters) {
        NuCutSceneFindCharacters(def);
    }
}


void NuGCutRigidCalcMtx(struct NUGCUTRIGID_s *rigid, float time, struct numtx_s *mtx) {
    struct nuanimdata2_s *anim = rigid->anim;
    struct nuanimtime_s atime;
    struct nuvec_s t;
    struct nuvec_s r;
    struct nuangvec_s ang;
    struct nuvec_s scale;

    if (anim) {
        union nuanimcurve2_u *curves = anim->curves;
        signed char *curveflags = anim->curveflags;
        int setflags = anim->curvesetflags[0];

        NuAnimData2CalcTime(anim, time, &atime);
        if (setflags & NUANIM_CURVESET_ROTATE) {
            r.x = NuAnimCurve2CalcVal(&curves[3], &atime, curveflags[3]);
            r.y = NuAnimCurve2CalcVal(&curves[4], &atime, curveflags[4]);
            r.z = NuAnimCurve2CalcVal(&curves[5], &atime, curveflags[5]);
            ang.x = (int)(r.x * 10430.378f);
            ang.y = (int)(r.y * 10430.378f);
            ang.z = (int)(r.z * 10430.378f);
            NuMtxSetRotateXYZ(mtx, &ang);
        } else {
            NuMtxSetIdentity(mtx);
        }
        if (setflags & NUANIM_CURVESET_SCALE) {
            scale.x = NuAnimCurve2CalcVal(&curves[6], &atime, curveflags[6]);
            scale.y = NuAnimCurve2CalcVal(&curves[7], &atime, curveflags[7]);
            scale.z = NuAnimCurve2CalcVal(&curves[8], &atime, curveflags[8]);
            NuMtxPreScale(mtx, &scale);
        }
        t.x = NuAnimCurve2CalcVal(&curves[0], &atime, curveflags[0]);
        t.y = NuAnimCurve2CalcVal(&curves[1], &atime, curveflags[1]);
        t.z = NuAnimCurve2CalcVal(&curves[2], &atime, curveflags[2]);
        NuMtxTranslate(mtx, &t);
        mtx->_02 = -mtx->_02;
        mtx->_12 = -mtx->_12;
        mtx->_20 = -mtx->_20;
        mtx->_21 = -mtx->_21;
        mtx->_23 = -mtx->_23;
        mtx->_32 = -mtx->_32;
        NuMtxTranslate(mtx, (struct nuvec_s *)&rigid->mtx._30);
    } else {
        *mtx = rigid->mtx;
    }
}

void instNuGCutRigidSysUpdate(struct instNUGCUTSCENE_s *scene, float time) {
    struct NUGCUTSCENE_s *def;
    struct NUGCUTRIGIDSYS_s *rigiddef;
    struct instNUGCUTRIGIDSYS_s *sys;
    struct numtx_s mtx;
    signed char state;
    int i;
    int j;

    def = scene->def;
    sys = scene->rigidsys;
    rigiddef = def->rigiddef;

    for (i = 0; i < rigiddef->count; i++) {
        struct NUGCUTRIGID_s *data = &rigiddef->rigids[i];
        struct instNUGCUTRIGID_s *rigid = &sys->rigids[i];

        if (data->flags & NUGCUTRIGID_ANIMATED) {
            continue;
        }
        if (!(data->flags & NUGCUTRIGID_FOUND)) {
            continue;
        }
        if (data->stateanim && StateAnimEvaluate(data->stateanim, &rigid->state, (unsigned char *)&state, time)) {
            if (state) {
                rigid->obj.special->instance->flagword |= NUINSTANCE_VISIBLE;
            } else {
                rigid->obj.special->instance->flagword &= ~NUINSTANCE_VISIBLE;
            }
        }
        if (rigid->obj.special->instance->flagword & NUINSTANCE_VISIBLE) {
            NuGCutRigidCalcMtx(data, time, &mtx);
            if (scene->flags & NUGCUTSCENE_MATRIXSET) {
                NuMtxMul(&mtx, &mtx, &scene->mtx);
            }
            rigid->obj.special->instance->mtx = mtx;
            if ((rigid->obj.special->instance->flagword & NUINSTANCE_VISIBLE) && data->first_locator != 0xFF) {
                struct NUGCUTLOCATORSYS_s *locatordef = def->locatordef;
                struct instNUGCUTLOCATORSYS_s *locatorsys = scene->locatorsys;

                for (j = 0; j < data->nlocators; j++) {
                    instNuGCutLocatorUpdate(scene, locatordef, &locatorsys->locators[data->first_locator + j],
                                            &locatordef->locators[data->first_locator + j], time, &mtx);
                }
            }
        }
    }
}


void instNuGCutRigidSysRender(struct instNUGCUTSCENE_s *scene, float time) {
    int draw;
    struct NUGCUTSCENE_s *def;
    struct NUGCUTRIGIDSYS_s *rigiddef;
    struct instNUGCUTRIGIDSYS_s *sys;
    struct numtx_s mtx;
    signed char state;
    int i;
    int j;

    def = scene->def;
    sys = scene->rigidsys;
    rigiddef = def->rigiddef;

    for (i = 0; i < rigiddef->count; i++) {
        struct NUGCUTRIGID_s *data = &rigiddef->rigids[i];
        struct instNUGCUTRIGID_s *rigid = &sys->rigids[i];

        if (!(data->flags & NUGCUTRIGID_ANIMATED)) {
            continue;
        }
        if (!(data->flags & NUGCUTRIGID_FOUND)) {
            continue;
        }
        if (data->stateanim && StateAnimEvaluate(data->stateanim, &rigid->state, (unsigned char *)&state, time)) {
            if (state) {
                rigid->obj.special->instance->flagword |= NUINSTANCE_VISIBLE;
            } else {
                rigid->obj.special->instance->flagword &= ~NUINSTANCE_VISIBLE;
            }
        }
        if (rigid->obj.special->instance->flagword & NUINSTANCE_VISIBLE) {
            NuGCutRigidCalcMtx(data, time, &mtx);
            if (scene->flags & NUGCUTSCENE_MATRIXSET) {
                NuMtxMul(&mtx, &mtx, &scene->mtx);
            }
            if (draw) {
                NuSpecialDrawAt(&rigid->obj, &mtx);
            }
            if (NuCutSceneRigidCollisionCheck) {
                NuCutSceneRigidCollisionCheck((struct instNUGCUTRIGID_s *)data, &mtx);
            }
            if (data->first_locator != 0xFF) {
                struct NUGCUTLOCATORSYS_s *locatordef = def->locatordef;
                struct instNUGCUTLOCATORSYS_s *locatorsys = scene->locatorsys;

                for (j = 0; j < data->nlocators; j++) {
                    instNuGCutLocatorUpdate(scene, locatordef, &locatorsys->locators[data->first_locator + j],
                                            &locatordef->locators[data->first_locator + j], time, &mtx);
                }
            }
        }
    }
}


void NuGCutCharAnimProcess(struct NUGCUTCHAR_s *desc, float time, struct numtx_s *mtx, int *is_visible,
                           unsigned int *animix, float *animrate, float *blendframes) {
    struct nuanimtime_s atime;
    struct nuvec_s t;
    struct nuvec_s r;
    struct nuangvec_s ang;
    float value;
    union nuanimcurve2_u *curves;
    signed char *curveflags;
    int setflags;

    if (desc->char_anim) {
        struct nuanimdata2_s *anim;

        NuAnimData2CalcTime(desc->char_anim, time, &atime);
        anim = desc->char_anim;
        curves = anim->curves;
        curveflags = anim->curveflags;
        setflags = anim->curvesetflags[0];
        if (anim->ncurves >= 7) {
            *is_visible = NuAnimCurve2CalcVal(&curves[6], &atime, curveflags[6]);
        } else {
            *is_visible = desc->flags & 1;
        }
        if (animix) {
            if (((struct nuanimdata2_s *)desc->char_anim)->ncurves >= 8) {
                value = NuAnimCurve2CalcVal(&curves[7], &atime, curveflags[7]);
                if (value < 0.0f) {
                    *animix = 0xFF;
                } else {
                    *animix = value;
                }
            } else {
                *animix = (unsigned char)desc->pad5D;
            }
        }
        if (*is_visible == 0) {
            return;
        }
        if (setflags & NUANIM_CURVESET_ROTATE) {
            r.x = NuAnimCurve2CalcVal(&curves[3], &atime, curveflags[3]);
            r.y = NuAnimCurve2CalcVal(&curves[4], &atime, curveflags[4]);
            r.z = NuAnimCurve2CalcVal(&curves[5], &atime, curveflags[5]);
            ang.x = (int)(r.x * 10430.378f);
            ang.y = (int)(r.y * 10430.378f);
            ang.z = (int)(r.z * 10430.378f);
            NuMtxSetRotateXYZ(mtx, &ang);
        } else {
            NuMtxSetIdentity(mtx);
        }
        t.x = NuAnimCurve2CalcVal(&curves[0], &atime, curveflags[0]);
        t.y = NuAnimCurve2CalcVal(&curves[1], &atime, curveflags[1]);
        t.z = NuAnimCurve2CalcVal(&curves[2], &atime, curveflags[2]);
        NuMtxTranslate(mtx, &t);
        mtx->_02 = -mtx->_02;
        mtx->_12 = -mtx->_12;
        mtx->_20 = -mtx->_20;
        mtx->_21 = -mtx->_21;
        mtx->_23 = -mtx->_23;
        mtx->_32 = -mtx->_32;
        if (animrate) {
            if (((struct nuanimdata2_s *)desc->char_anim)->ncurves >= 10) {
                *animrate = NuAnimCurve2CalcVal(&curves[9], &atime, curveflags[9]);
            } else {
                *animrate = *(float *)desc->pad58;
            }
        }
        if (blendframes) {
            if (((struct nuanimdata2_s *)desc->char_anim)->ncurves >= 9) {
                *blendframes = NuAnimCurve2CalcVal(&curves[8], &atime, curveflags[8]);
            } else {
                *blendframes = (unsigned char)desc->pad61[0];
            }
        }
    } else {
        *is_visible = desc->flags & 1;
        if (animix) {
            *animix = (unsigned char)desc->pad5D;
        }
        if (is_visible == 0) {
            return;
        }
        *mtx = desc->mtx;
        if (animrate) {
            *animrate = *(float *)desc->pad58;
        }
        if (blendframes) {
            *blendframes = (unsigned char)desc->pad61[0];
        }
    }
}

void instNuGCutRigidSysEnd(struct instNUGCUTSCENE_s *scene, float time) {
    struct NUGCUTRIGIDSYS_s *rigiddef = scene->def->rigiddef;
    struct instNUGCUTRIGIDSYS_s *sys = scene->rigidsys;
    struct numtx_s mtx;
    signed char state;
    int i;

    for (i = 0; i < rigiddef->count; i++) {
        struct NUGCUTRIGID_s *data = &rigiddef->rigids[i];
        struct instNUGCUTRIGID_s *rigid = &sys->rigids[i];

        if (!(data->flags & 0x4)) {
            continue;
        }
        if (!(data->flags & 0x2)) {
            if (data->stateanim && StateAnimEvaluate(data->stateanim, &rigid->state, (unsigned char *)&state, time)) {
                if (state) {
                    rigid->obj.special->instance->flagword |= NUINSTANCE_VISIBLE;
                } else {
                    rigid->obj.special->instance->flagword &= ~NUINSTANCE_VISIBLE;
                }
            }
            /* The end matrix is computed but never applied (retail does the
             * same). */
            if (rigid->obj.special->instance->flagword & NUINSTANCE_VISIBLE) {
                NuGCutRigidCalcMtx(data, time, &mtx);
                if (scene->flags & NUGCUTSCENE_MATRIXSET) {
                    NuMtxMul(&mtx, &mtx, &scene->mtx);
                }
            }
        }
    }
}

void instNuGCutCharSysEnd(struct instNUGCUTSCENE_s *scene, float time) {
    struct NUGCUTSCENE_s *def = scene->def;
    struct NUGCUTCHARSYS_s *chardef = def->chardef;
    struct instNUGCUTCHARSYS_s *sys = scene->charsys;
    int i;

    for (i = 0; i < chardef->count; i++) {
        struct NUGCUTCHAR_s *desc = &chardef->chars[i];
        struct instNUGCUTCHAR_s *character = &sys->chars[i];

        if (character->data && !(desc->flags & 0x2)) {
            if (NuCutSceneCharacterRender) {
                NuCutSceneCharacterRender(scene, def, character, desc, time);
            }
            if (NuCutSceneCharacterRelease) {
                NuCutSceneCharacterRelease(character);
            }
        }
    }
}


void NuGCutTriggerSysFixPtrs(struct NUGCUTSCENE_s *def, int base) {
    struct NUGCUTTRIGGERSYS_s *triggerdef;
    int i;

    if (def->triggerdef) {
        triggerdef = (struct NUGCUTTRIGGERSYS_s *)((char *)def->triggerdef + base);
        def->triggerdef = triggerdef;
        triggerdef->triggers = NuFixPtr(triggerdef->triggers, base);
        if (triggerdef->triggers) {
            for (i = 0; i < triggerdef->count; i++) {
                struct NUGCUTTRIGGER_s *trigger = &triggerdef->triggers[i];

                trigger->name = NuFixPtr(trigger->name, base);
                trigger->anim = StateAnimFixPtrs(trigger->anim, base);
            }
        }
    }
}


struct instNUGCUTTRIGGERSYS_s *instNuCGutTriggerSysCreate(struct NUGCUTSCENE_s *def, struct NuGCutTriggerList *list, char **heap) {
    struct instNUGCUTTRIGGERSYS_s *sys;
    int count = def->triggerdef->count;

    sys = (struct instNUGCUTTRIGGERSYS_s *)ALIGN16(*heap);
    *heap = (char *)sys + sizeof(struct instNUGCUTTRIGGERSYS_s);
    memset(sys, 0, sizeof(struct instNUGCUTTRIGGERSYS_s));
    sys->list = list;
    sys->triggers = (struct instNUGCUTTRIGGER_s *)*heap;
    *heap += count * sizeof(struct instNUGCUTTRIGGER_s);
    memset(sys->triggers, 0, count * sizeof(struct instNUGCUTTRIGGER_s));
    return sys;
}


extern char D_00619E38[];
#define nospecial_msg D_00619E38

struct instNUGCUTRIGIDSYS_s *instNuCGutRigidSysCreate(struct NUGCUTSCENE_s *def, struct nugscn_s *gscene, char **heap) {
    struct NUGCUTRIGIDSYS_s *rigiddef = def->rigiddef;
    struct instNUGCUTRIGIDSYS_s *sys = 0;
    int i;

    if (rigiddef && rigiddef->count && heap) {
        sys = (struct instNUGCUTRIGIDSYS_s *)(*heap = ALIGN16(*heap));
        *heap = (char *)sys + sizeof(struct instNUGCUTRIGIDSYS_s);
        sys->rigids = 0;
        sys->rigids = (struct instNUGCUTRIGID_s *)(*heap = ALIGN16(*heap));
        *heap += rigiddef->count * sizeof(struct instNUGCUTRIGID_s);
        memset(sys->rigids, 0, rigiddef->count * sizeof(struct instNUGCUTRIGID_s));
        for (i = 0; i < rigiddef->count; i++) {
            struct NUGCUTRIGID_s *data = &rigiddef->rigids[i];
            struct instNUGCUTRIGID_s *rigid = &sys->rigids[i];

            if (gscene && gscene != data->obj.scene) {
                if (data->obj.special) {
                    rigid->obj.scene = gscene;
                    rigid->obj.special = &gscene->specials[data->obj.special - data->obj.scene->specials];
                } else {
                    NuDebugMsgProlog(gcutscn_srcfile, 0x609)(nospecial_msg, data->name);
                }
            } else {
                rigid->obj = data->obj;
            }
        }
    }
    return sys;
}


struct instNUGCUTCHARSYS_s *instNuCGutCharSysCreate(struct NUGCUTSCENE_s *def, char **heap) {
    struct NUGCUTCHARSYS_s *chardef;
    struct instNUGCUTCHARSYS_s *charsys;
    struct NUGCUTCHAR_s *desc;
    struct instNUGCUTCHAR_s *character;
    int i;

    charsys = 0;
    chardef = def->chardef;
    if (chardef && chardef->count && heap) {
        charsys = (struct instNUGCUTCHARSYS_s *)(*heap = ALIGN16(*heap));
        *heap = (char *)(charsys + 1);
        charsys->chars = 0;
        character = (struct instNUGCUTCHAR_s *)(*heap = ALIGN16(*heap));
        charsys->chars = character;
        *heap += chardef->count * sizeof(struct instNUGCUTCHAR_s);
        memset(charsys->chars, 0, chardef->count * sizeof(struct instNUGCUTCHAR_s));
        for (i = 0; i < chardef->count; i++) {
            desc = &chardef->chars[i];
            character = &charsys->chars[i];
            character->current_animix = -1;
            character->prev_animix = -1;
            if (desc->flags & 0x2) {
                if (NuCutSceneCharacterCreateData) {
                    NuCutSceneCharacterCreateData(desc, character, heap);
                }
            } else {
                character->data = desc->character;
            }
        }
    }
    return charsys;
}

struct instNUGCUTSCENE_s *instNuGCutSceneCreate(struct NUGCUTSCENE_s *def, void *gscene, struct NuGCutTriggerList *triggers,
                                   char *name, char **heap) {
    struct instNUGCUTSCENE_s *scene;

    scene = 0;
    if (def) {
        scene = (struct instNUGCUTSCENE_s *)(*heap = ALIGN16(*heap));
        *heap = (char *)(scene + 1);
        memset(scene, 0, sizeof(struct instNUGCUTSCENE_s));
        scene->next = cutscenelist;
        if (cutscenelist) {
            cutscenelist->prev = scene;
        }
        cutscenelist = scene;
        scene->def = def;
        if (name) {
            sprintf(scene->name, name);
        }
        if (def->camdef) {
            scene->camsys = instNuCGutCamSysCreate(def->camdef, heap);
        }
        if (def->rigiddef) {
            scene->rigidsys = instNuCGutRigidSysCreate(def, gscene, heap);
        }
        if (def->chardef) {
            scene->charsys = instNuCGutCharSysCreate(def, heap);
        }
        if (def->locatordef) {
            scene->locatorsys = NuCGutLocatorSysCreateInst(def->locatordef, heap);
        }
        if (def->triggerdef && triggers) {
            scene->triggersys = instNuCGutTriggerSysCreate(def, triggers, heap);
        }
        instNuGCutSceneCalculateCentre(scene, 0);
        scene->speed = 1.0f;
    }
    return scene;
}


void instNuGCutTriggerSysUpdate(struct instNUGCUTSCENE_s *scene, float time) {
    struct NUGCUTTRIGGERSYS_s *def = scene->def->triggerdef;
    struct instNUGCUTTRIGGERSYS_s *sys = scene->triggersys;
    unsigned char state;
    int i;

    for (i = 0; i < def->count; i++) {
        struct NUGCUTTRIGGER_s *trigger = &def->triggers[i];

        if (trigger->anim && StateAnimEvaluate(trigger->anim, &sys->triggers[i].state, &state, time)) {
            if (state) {
                sys->list->targets[trigger->target].flags |= 0x1;
            } else {
                sys->list->targets[trigger->target].flags &= ~0x1;
            }
        }
    }
}


void instNuGCutTriggerSysStart(struct instNUGCUTSCENE_s *scene) {
    struct NUGCUTSCENE_s *def = scene->def;
    struct instNUGCUTTRIGGERSYS_s *sys = scene->triggersys;
    struct NUGCUTTRIGGERSYS_s *tdef = def->triggerdef;
    int i;

    for (i = 0; i < tdef->count; i++) {
        struct instNUGCUTTRIGGER_s *trigger = &sys->triggers[i];
        trigger->state = 0;
    }
}


void instNuGCutSceneRender(struct instNUGCUTSCENE_s *scene) {
    if (scene->flags & NUGCUTSCENE_DISABLED) {
        return;
    }
    if (!(scene->flags & NUGCUTSCENE_RUNNING)) {
        return;
    }
    if (!(scene->flags & NUGCUTSCENE_VISIBLE)) {
        return;
    }
    if (cutscene_rndr_rigids && scene->rigidsys) {
        instNuGCutRigidSysRender(scene, scene->time);
    }
    if (cutscene_rndr_chars && scene->charsys) {
        instNuGCutCharSysRender(scene, scene->time);
    }
    if (cutscene_rndr_locators && scene->locatorsys) {
        instNuGCutLocatorSysUpdate(scene, scene->time);
    }
}


int StateAnimEvaluate(struct NUSTATEANIM_s *anim, unsigned char *key, unsigned char *value, float time) {
    int changed = 0;

    if (*key < anim->count) {
        do {
            if (!(anim->times[*key] <= time)) {
                break;
            }
            *value = anim->values[*key];
            (*key)++;
            changed = 1;
        } while (*key < anim->count);
    }
    if (!changed) {
        if (*key != 0) {
            do {
                if (!(time < anim->times[*key - 1])) {
                    break;
                }
                (*key)--;
                *value = anim->values[*key];
                changed = 1;
            } while (*key != 0);
        }
    }
    return changed;
}


struct NUSTATEANIM_s *StateAnimFixPtrs(struct NUSTATEANIM_s *anim, int base) {
    anim = NuFixPtr(anim, base);
    if (anim) {
        anim->times = NuFixPtr(anim->times, base);
        anim->values = NuFixPtr(anim->values, base);
    }
    return anim;
}


void instNuGCutCharSysRender(struct instNUGCUTSCENE_s *scene, float time) {
    struct NUGCUTSCENE_s *def = scene->def;
    struct instNUGCUTCHARSYS_s *sys = scene->charsys;
    struct NUGCUTCHARSYS_s *chardef = def->chardef;
    int i;

    if (NuCutSceneCharacterRender) {
        for (i = 0; i < chardef->count; i++) {
            struct NUGCUTCHAR_s *desc = &chardef->chars[i];
            struct instNUGCUTCHAR_s *character = &sys->chars[i];

            if (character->data) {
                NuCutSceneCharacterRender(scene, def, character, desc, time);
            }
        }
    }
}


extern char D_00619C78[];               /* error message */
extern char D_00619CD8[];               /* error message */
#define loadfail_msg D_00619C78
#define badversion_msg D_00619CD8
extern int NuFileLoadBuffer(char *name, void *buf, int size);


extern struct numtx_s cutscenecammtx;
extern int set_cutscenecammtx;
extern char D_00619D18[];
#define badcamera_msg D_00619D18

void instNuGCutCamSysUpdate(struct instNUGCUTSCENE_s *scene, float time) {
    struct NUGCUTCAMSYS_s *camdef = scene->def->camdef;
    struct instNUGCUTCAMSYS_s *camsys = scene->camsys;
    struct nuanimtime_s atime;
    struct numtx_s aim;
    struct nuquat_s qfrom;
    struct nuquat_s qto;
    struct nuquat_s qblend;
    struct nuvec_s campos;
    unsigned char camchange;
    struct NUGCUTCAMTGT_s *tgt;
    struct instNUGCUTCAM_s *cam;
    struct NUGCUTCAM_s *camdata;
    float nframes;
    float lerp;
    int blendout;

    if (camdef->stateanim && StateAnimEvaluate(camdef->stateanim, &camsys->next_switch, &camchange, time)) {
        camsys->current = camchange;
    }
    for (; camsys->next_tgt < camsys->count && time >= camsys->targets[camsys->next_tgt].frame; camsys->next_tgt++) {
        tgt = &camsys->targets[camsys->next_tgt];
        cam = &camsys->cams[camdef->remap[tgt->camid]];
        cam->flags |= NUGCUTCAM_TARGETED;
        cam->tgt_ix = camsys->next_tgt;
    }
    while (camsys->next_tgt != 0 && time < camsys->targets[camsys->next_tgt - 1].frame) {
        camsys->next_tgt--;
        tgt = &camsys->targets[camsys->next_tgt];
        cam = &camsys->cams[camdef->remap[tgt->camid]];
        cam->flags |= NUGCUTCAM_TARGETED;
        cam->tgt_ix = camsys->next_tgt;
    }
    if (camsys->current < 0) {
        return;
    }
    if (camsys->current >= camdef->count) {
        NuErrorProlog(gcutscn_srcfile, 0x37B)(badcamera_msg);
    }
    cam = &camsys->cams[camsys->current];
    camdata = &camdef->cams[camsys->current];
    if ((camdata->flags & NUGCUTCAM_ANIMATED) && camdata->anim_ix < camdef->anim->nnodes) {
        struct nuanimdata2_s *anim;

        NuAnimData2CalcTime(camdef->anim, time, &atime);
        anim = camdef->anim;
        NuAnimCurve2SetApplyToMatrix(&anim->curves[anim->ncurves * camdata->anim_ix],
                                     &anim->curveflags[anim->ncurves * camdata->anim_ix],
                                     anim->curvesetflags[camdata->anim_ix], &atime, &cutscenecammtx);
    } else {
        cutscenecammtx = camdata->mtx;
    }
    if (scene->flags & NUGCUTSCENE_MATRIXSET) {
        NuMtxMul(&cutscenecammtx, &cutscenecammtx, &scene->mtx);
    }
    set_cutscenecammtx = 1;
    if (cam->flags & NUGCUTCAM_TARGETED) {
        blendout = 0;
        tgt = &camsys->targets[cam->tgt_ix];
        nframes = NuFabs(tgt->nframes);
        if (NuFsign(tgt->nframes) < 0.0f) {
            blendout = 1;
        }
        if (blendout && time >= tgt->frame + nframes) {
            cam->flags &= ~NUGCUTCAM_TARGETED;
            return;
        }
        campos = *(struct nuvec_s *)&cutscenecammtx._30;
        aim = cutscenecammtx;
        NuMtxLookAtZ(&aim, tgt->object);
        if (nframes > 0.01f) {
            lerp = (time - tgt->frame) / nframes;
            if (lerp > 1.0f) {
                blendout = 1;
            } else if (lerp < 0.0f) {
                blendout = 0;
            }
        } else {
            lerp = 1.0f;
        }
        if (blendout) {
            lerp = 1.0f - lerp;
        }
        NuMtxToQuat(&aim, &qto);
        NuMtxToQuat(&cutscenecammtx, &qfrom);
        NuQuatSlerp(&qblend, &qfrom, &qto, lerp);
        NuQuatToMtx(&qblend, &cutscenecammtx);
        *(struct nuvec_s *)&cutscenecammtx._30 = campos;
    }
}


struct NUGCUTSCENE_s *NuGCutSceneLoad(char *name, char **heap, char **heapend) {
    struct NUGCUTSCENE_s *def;
    int size;

    *heap = ALIGN16(*heap);
    size = NuFileLoadBuffer(name, *heap, *heapend - *heap);
    if (size == 0) {
        NuErrorProlog(gcutscn_srcfile, 0x14C)(loadfail_msg);
    }
    def = (struct NUGCUTSCENE_s *)*heap;
    *heap = (char *)def + size;
    if (def->version != 1) {
        NuErrorProlog(gcutscn_srcfile, 0x151)(badversion_msg, name, def->version);
    }
    def->base = (int)def - def->base;
    def->names = def->names ? def->names + def->base : 0;
    if (def->camdef) {
        NuGCutCamsSysFixPtrs(def, def->base);
    }
    if (def->locatordef) {
        NuGCutLocatorSysFixPtrs(def, def->base);
    }
    if (def->rigiddef) {
        NuGCutRigidSysFixPtrs(def, def->base);
    }
    if (def->chardef) {
        NuGCutCharSysFixPtrs(def, def->base);
    }
    if (def->triggerdef) {
        NuGCutTriggerSysFixPtrs(def, def->base);
    }
    def->bounds = def->bounds ? (struct NUGCUTBBOX_s *)((char *)def->bounds + def->base) : 0;
    return def;
}
