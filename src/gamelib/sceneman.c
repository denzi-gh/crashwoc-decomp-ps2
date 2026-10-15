/*
 * Unit: gamelib/sceneman
 *
 * Functions:
 *   0x001b0fe8 NuSceneDataLoad
 *   0x001b1410 NuSceneInstanceCreate
 *   0x001b1790 NuSceneInstanceRunScript
 *   0x001b1940 LoadCutSceneInstanceData
 *   0x001b1ca8 LoadCutsceneData
 *   0x001b1f60 LoadCutsceneScript
 *   0x001b2448 NuSceneManagerSysInit
 *   0x001b2450 NuSceneManagerSysUpdate
 *   0x001b24a0 NuSceneInstanceDestroy
 *   0x001b2558 NuSceneInstanceTranslate
 *   0x001b25d0 NuSceneInstanceRotateY
 *   0x001b2648 NuSceneInstanceEnable
 *   0x001b26e0 NuSceneInstanceDisable
 *   0x001b2778 NuSceneInstanceSetRate
 *   0x001b2780 SceneInstanceFindCutsceneInst
 *   0x001b2820 NuSceneManagerSysRender
 *   0x001b2868 NuSceneManagerSetPtrToPlayerMtx
 */

#include "nu_libc.h"

#include "nucore/nuerror.h"
#include "numath/numathtypes.h"
#include "nu3d/nugscn.h"
#include "gamelib/gcutscn.h"
#include "gamelib/sceneman.h"
#include "gamelib/terrain.h"
#include "gamelib/trigger.h"
#include "nucore/nucoretypes.h"

typedef struct NuFPar {
    int handle;
    char buffer[0x1000];
    char line[0x101];
    char word[0x101];
    int line_num;
} NuFPar;

#define NUSCENEINST_DISABLED 0x2

extern NuFPar *NuFParCreate(char *name);
extern void NuFParDestroy(NuFPar *parser);
extern int NuFParGetLine(NuFPar *parser);
extern int NuFParGetWord(NuFPar *parser);
extern int NuFParGetInt(NuFPar *parser);

extern void *CreateSpecialTerrainData(int levelnum, struct nugscn_s *gscene, union variptr_u *heap,
                                      union variptr_u *heapend);

extern void NuGScnUpdate(struct nugscn_s *scene, float time);
extern void NuGScnRndr3(struct nugscn_s *scene);

extern char D_0061A090[];
extern char D_0061A0A0[];
extern char D_0061A0B0[];
extern char D_0061A0C0[];
extern char D_0061A0E8[];
extern char D_0061A128[];
extern char D_0061A138[];
extern char D_0061A170[];
extern char D_0061A180[];
extern char D_0061A1C0[];
extern char D_0061A210[];
extern char D_0062F810[];
extern char D_0062F818[];
extern char D_0062F820[];

#define scenecfg_fmt D_0061A090
#define gscfile_fmt D_0061A0A0
#define trgfile_fmt D_0061A0B0
#define terfile_fmt D_0062F818
#define sceneman_srcfile D_0061A0C0
#define kw_scene D_0062F810
#define kw_numcutscenes D_0061A128
#define kw_cutscene D_0061A170
#define kw_script D_0062F820
#define err_multiple_scene D_0061A0E8
#define err_multiple_numcutscenes D_0061A138
#define err_missing_numcutscenes D_0061A180
#define err_too_many_cutscenes D_0061A1C0
#define err_cutscene_count D_0061A210

extern struct NuSceneInstance *D_0062F808;
extern struct numtx_s *D_0062F80C;
#define sceneinst_list D_0062F808
#define player_mtx D_0062F80C


struct NuSceneData *NuSceneDataLoad(char *dir, union variptr_u *heap, union variptr_u *heapend) {
    char path[256];
    struct NuSceneData *result;
    struct NuSceneScript *lastscript;
    struct NuSceneScript *script;
    struct NuSceneData *scene;
    NuFPar *parser;
    char *word;
    char *namebuf;
    int cutscene;

    result = 0;
    sprintf(path, scenecfg_fmt, dir);
    lastscript = 0;
    parser = NuFParCreate(path);
    if (parser) {
        scene = (struct NuSceneData *)((heap->intaddr + 15) & ~15);
        heap->voidptr = (char *)scene + sizeof(struct NuSceneData);
        result = scene;
        memset(scene, 0, sizeof(struct NuSceneData));
        while (NuFParGetLine(parser)) {
            NuFParGetWord(parser);
            word = parser->word;
            if (strcasecmp(word, kw_scene) == 0) {
                NuFParGetWord(parser);
                heap->intaddr = (heap->intaddr + 15) & ~15;
                scene->name = heap->voidptr;
                namebuf = heap->charptr + 1;
                heap->voidptr = namebuf + strlen(word);
                sprintf(scene->name, word);
                if (scene->gscene == 0) {
                    heap->intaddr = (heap->intaddr + 15) & ~15;
                    sprintf(path, gscfile_fmt, dir, word);
                    scene->gscene = NuGScnRead(&heap->charptr, heapend->charptr, path);
                    if (scene->gscene) {
                        sprintf(path, terfile_fmt, dir, word);
                        scene->terrain = terraininit(0, &heap->s16, heapend->s16, 0, path, scene->gscene, 0);
                        if (scene->terrain) {
                            scene->specialterrain = CreateSpecialTerrainData(0, scene->gscene, heap, heapend);
                        }
                        sprintf(path, trgfile_fmt, dir, word);
                        scene->triggers = NuTriggerSysLoad(path, heap, heapend);
                    }
                } else {
                    NuErrorProlog(sceneman_srcfile, 0xAF)(err_multiple_scene, dir);
                }
            } else if (strcasecmp(word, kw_numcutscenes) == 0) {
                if (scene->ncutscenes) {
                    NuErrorProlog(sceneman_srcfile, 0xB3)(err_multiple_numcutscenes, path);
                }
                scene->ncutscenes = NuFParGetInt(parser);
                if (scene->ncutscenes) {
                    heap->intaddr = (heap->intaddr + 15) & ~15;
                    scene->cutscenes = heap->voidptr;
                    heap->u8 += scene->ncutscenes * sizeof(struct NuSceneCutscene);
                    memset(scene->cutscenes, 0, scene->ncutscenes * sizeof(struct NuSceneCutscene));
                    cutscene = 0;
                }
            } else if (strcasecmp(word, kw_cutscene) == 0) {
                if (scene->ncutscenes == 0) {
                    NuErrorProlog(sceneman_srcfile, 0xC0)(err_missing_numcutscenes, path);
                }
                if (cutscene >= scene->ncutscenes) {
                    NuErrorProlog(sceneman_srcfile, 0xC1)(err_too_many_cutscenes, path);
                }
                LoadCutsceneData(parser, dir, scene, &scene->cutscenes[cutscene], heap, heapend);
                cutscene++;
            } else if (strcasecmp(word, kw_script) == 0) {
                script = LoadCutsceneScript(parser, scene, heap, heapend);
                if (script) {
                    if (lastscript) {
                        lastscript->next = script;
                    } else {
                        scene->scripts = script;
                    }
                    lastscript = script;
                }
            }
        }
        NuFParDestroy(parser);
        if (cutscene < scene->ncutscenes) {
            NuErrorProlog(sceneman_srcfile, 0xD3)(err_cutscene_count, path);
        }
        for (cutscene = 0; cutscene < scene->ncutscenes; cutscene++) {
            NuGCutSceneFixUp(scene->cutscenes[cutscene].def, scene->gscene, scene->triggers);
        }
    }
    return result;
}


void NuSceneManagerSysInit(void) {
    sceneinst_list = 0;
}


void NuSceneManagerSysUpdate(void) {
    struct NuSceneInstance *inst;
    struct NuSceneInstance *next;

    for (inst = sceneinst_list; inst; inst = next) {
        next = inst->next;
        if (!(inst->flags & NUSCENEINST_DISABLED)) {
            NuGScnUpdate(inst->gscene, inst->rate);
        }
    }
}


void NuSceneInstanceDestroy(struct NuSceneInstance *inst) {
    int i;

    if (inst->triggers) {
        instNuTriggerSysDestroy(inst->triggers);
    }
    for (i = 0; i < inst->data->ninstances; i++) {
        if (inst->cutscenes[i]) {
            instNuGCutSceneDestroy(inst->cutscenes[i]);
        }
    }
    if (inst->next) {
        inst->next->prev = inst->prev;
    }
    if (inst->prev) {
        inst->prev->next = inst->next;
    } else {
        sceneinst_list = inst->next;
    }
}


void NuSceneInstanceTranslate(struct NuSceneInstance *inst, struct nuvec_s *offset) {
    int i;

    for (i = 0; i < inst->data->ninstances; i++) {
        instNuGCutSceneTranslate(inst->cutscenes[i], offset);
    }
}


void NuSceneInstanceRotateY(struct NuSceneInstance *inst, int angle) {
    int i;

    for (i = 0; i < inst->data->ninstances; i++) {
        instNuGCutSceneRotateY(inst->cutscenes[i], angle);
    }
}


void NuSceneInstanceEnable(struct NuSceneInstance *inst) {
    int i;

    if (inst->flags & NUSCENEINST_DISABLED) {
        for (i = 0; i < inst->data->ninstances; i++) {
            instNuGCutSceneEnable(inst->cutscenes[i]);
        }
        if (inst->triggers) {
            instNuTriggerEnable(inst->triggers);
        }
        inst->flags &= ~NUSCENEINST_DISABLED;
    }
}


void NuSceneInstanceDisable(struct NuSceneInstance *inst) {
    int i;

    if (!(inst->flags & NUSCENEINST_DISABLED)) {
        for (i = 0; i < inst->data->ninstances; i++) {
            instNuGCutSceneDisable(inst->cutscenes[i]);
        }
        if (inst->triggers) {
            instNuTriggerDisable(inst->triggers);
        }
        inst->flags |= NUSCENEINST_DISABLED;
    }
}


void NuSceneInstanceSetRate(struct NuSceneInstance *inst, float rate) {
    inst->rate = rate;
}


struct instNUGCUTSCENE_s *SceneInstanceFindCutsceneInst(struct NuSceneInstance *inst, char *name) {
    int i;

    for (i = 0; i < inst->data->ninstances; i++) {
        if (strcasecmp(inst->cutscenes[i]->name, name) == 0) {
            return inst->cutscenes[i];
        }
    }
    return 0;
}


void NuSceneManagerSysRender(void) {
    struct NuSceneInstance *inst;
    struct NuSceneInstance *next;

    for (inst = sceneinst_list; inst; inst = next) {
        next = inst->next;
        if (!(inst->flags & NUSCENEINST_DISABLED)) {
            NuGScnRndr3(inst->gscene);
        }
    }
}


void NuSceneManagerSetPtrToPlayerMtx(struct numtx_s *mtx) {
    player_mtx = mtx;
}
