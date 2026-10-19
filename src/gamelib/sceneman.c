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
#include "nucore/nufpar.h"
#include "numath/numtx.h"

#define NUSCENEINST_DISABLED 0x2

extern void *CreateSpecialTerrainData(int levelnum, struct nugscn_s *gscene, union variptr_u *heap,
                                      union variptr_u *heapend);

extern void NuGScnUpdate(struct nugscn_s *scene, float time);
extern void NuGScnRndr3(struct nugscn_s *scene);

static struct NuSceneInstance *sceneinstances = 0;
static struct numtx_s *player_mtx = 0;

struct NuSceneCamTarget {
    char *name;
    float frame;
    float nframes;
    signed char camid;
    char pad[3];
};

struct NuSceneCutsceneInst {
    struct numtx_s mtx;
    char *name;
    float maxvisidist_sq;
    float speed;
    int ncamtgts;
    struct NuSceneCamTarget *camtgts;
    int looping : 1;
    int autostart : 1;
    int position : 1;
    int clipbbox : 1;
    int maxvisidist : 1;
};

enum NuSceneCommandType {
    NUSCENECMD_STARTCUTSCENE,
    NUSCENECMD_CHAINCUTSCENE,
    NUSCENECMD_TRIGGERFLAG,
    NUSCENECMD_TRIGGERRESET,
    NUSCENECMD_FINISH,
    NUSCENECMD_SPECIALVISIBLE
};

struct NuSceneChainArgs {
    char *first;
    char *next;
};

struct NuSceneTriggerFlagArgs {
    signed char index;
    signed char set;
    char pad[2];
    char *trigger;
};

struct NuSceneSpecialArgs {
    signed char visible;
    char pad[3];
    char *special;
};

struct NuSceneCommand {
    struct NuSceneCommand *next;
    int type;
    union NuSceneCommandArgs {
        void *data;
        char **name;
        struct NuSceneChainArgs *chain;
        struct NuSceneTriggerFlagArgs *flag;
        struct NuSceneSpecialArgs *special;
    } args;
};


struct NuSceneData *NuSceneDataLoad(char *dir, union variptr_u *heap, union variptr_u *heapend) {
    char path[256];
    struct NuSceneData *result;
    struct NuSceneScript *lastscript;
    struct NuSceneScript *script;
    struct NuSceneData *scene;
    struct nufpar_s *parser;
    char *word;
    char *namebuf;
    int cutscene;

    sprintf(path, "%s\\scene.cfg", dir);
    result = 0;
    lastscript = 0;
    parser = NuFParCreate(path);
    if (parser) {
        heap->intaddr = (heap->intaddr + 15) & ~15;
        scene = heap->voidptr;
        result = scene;
        heap->u8 += sizeof(struct NuSceneData);
        memset(scene, 0, sizeof(struct NuSceneData));
        while (NuFParGetLine(parser)) {
            NuFParGetWord(parser);
            word = parser->wbuff;
            if (strcasecmp(word, "SCENE") == 0) {
                NuFParGetWord(parser);
                heap->intaddr = (heap->intaddr + 15) & ~15;
                scene->name = heap->voidptr;
                namebuf = heap->charptr + 1;
                heap->voidptr = namebuf + strlen(word);
                sprintf(scene->name, word);
                if (scene->gscene == 0) {
                    heap->intaddr = (heap->intaddr + 15) & ~15;
                    sprintf(path, "%s\\%s.gsc", dir, word);
                    scene->gscene = NuGScnRead(&heap->charptr, heapend->charptr, path);
                    if (scene->gscene) {
                        sprintf(path, "%s\\%s", dir, word);
                        scene->terrain = terraininit(0, &heap->s16, heapend->s16, 0, path, scene->gscene, 0);
                        if (scene->terrain) {
                            scene->specialterrain = CreateSpecialTerrainData(0, scene->gscene, heap, heapend);
                        }
                        sprintf(path, "%s\\%s.trg", dir, word);
                        scene->triggers = NuTriggerSysLoad(path, heap, heapend);
                    }
                } else {
                    NuErrorProlog("..\\nu2crash.ps2\\gamelib\\sceneman.c", 0xAF)(
                        "NuSceneDataLoad: scene.cfg in <%s> has more than one SCENE", dir);
                }
            } else if (strcasecmp(word, "NUMCUTSCENES") == 0) {
                if (scene->ncutscenes) {
                    NuErrorProlog("..\\nu2crash.ps2\\gamelib\\sceneman.c", 0xB3)(
                        "NuSceneDataLoad <%s>: Should only have one NUMCUTSCENES", path);
                }
                scene->ncutscenes = NuFParGetInt(parser);
                if (scene->ncutscenes) {
                    heap->intaddr = (heap->intaddr + 15) & ~15;
                    scene->cutscenes = heap->voidptr;
                    heap->u8 += scene->ncutscenes * sizeof(struct NuSceneCutscene);
                    memset(scene->cutscenes, 0, scene->ncutscenes * sizeof(struct NuSceneCutscene));
                    cutscene = 0;
                }
            } else if (strcasecmp(word, "CUTSCENE") == 0) {
                if (scene->ncutscenes == 0) {
                    NuErrorProlog("..\\nu2crash.ps2\\gamelib\\sceneman.c", 0xC0)(
                        "NuSceneDataLoad <%s>: Need NUMCUTSCENES <num> before CUTSCENE", path);
                }
                if (cutscene >= scene->ncutscenes) {
                    NuErrorProlog("..\\nu2crash.ps2\\gamelib\\sceneman.c", 0xC1)(
                        "NuSceneDataLoad <%s>: There are more cutscenes than was defined by NUMCUTSCENES", path);
                }
                LoadCutsceneData(parser, dir, scene, &scene->cutscenes[cutscene], heap, heapend);
                cutscene++;
            } else if (strcasecmp(word, "SCRIPT") == 0) {
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
            NuErrorProlog("..\\nu2crash.ps2\\gamelib\\sceneman.c", 0xD3)(
                "NuSceneDataLoad <%s>: Num cutscenes does not match NUMCUTSCENES <num>", path);
        }
        for (cutscene = 0; cutscene < scene->ncutscenes; cutscene++) {
            NuGCutSceneFixUp(scene->cutscenes[cutscene].def, scene->gscene, scene->triggers);
        }
    }
    return result;
}


struct NuSceneInstance *NuSceneInstanceCreate(struct NuSceneData *data, int duplicate, union variptr_u *heap,
                                              union variptr_u *heapend) {
    struct NuSceneInstance *result;
    struct NuSceneInstance *inst;
    struct NuSceneCutscene *cutscene;
    struct NuSceneCutsceneInst *cinst;
    int i;
    int j;
    int k;
    int t;

    result = 0;
    if (data) {
        heap->intaddr = (heap->intaddr + 15) & ~15;
        inst = heap->voidptr;
        result = inst;
        heap->u8 += sizeof(struct NuSceneInstance);
        memset(inst, 0, sizeof(struct NuSceneInstance));
        result->prev = 0;
        result->next = sceneinstances;
        if (sceneinstances) {
            sceneinstances->prev = result;
        }
        sceneinstances = result;
        result->data = data;
        if (duplicate) {
            result->gscene = NuGScnDuplicate(data->gscene, &heap->charptr, &heapend->charptr);
        } else {
            result->gscene = data->gscene;
        }
        inst->triggers = instNuTriggerSysCreate(data->triggers, inst, heap, heapend);
        inst->rate = 1.0f;
        k = 0;
        if (data->ninstances) {
            heap->intaddr = (heap->intaddr + 15) & ~15;
            inst->cutscenes = heap->voidptr;
            heap->u8 += data->ninstances * sizeof(struct instNUGCUTSCENE_s *);
            for (i = 0; i < data->ncutscenes; i++) {
                cutscene = &data->cutscenes[i];
                for (j = 0; j < cutscene->ninstances; j++) {
                    cinst = (struct NuSceneCutsceneInst *)cutscene->instances + j;
                    inst->cutscenes[k] = instNuGCutSceneCreate(cutscene->def, inst->gscene, inst->triggers,
                                                               cinst->name, heap);
                    if (cinst->position) {
                        instNuGCutSceneSetMtx(inst->cutscenes[k], &cinst->mtx);
                    }
                    inst->cutscenes[k]->maxvisidist_sq = cinst->maxvisidist_sq;
                    inst->cutscenes[k]->speed = cinst->speed;
                    inst->cutscenes[k]->flags =
                        (inst->cutscenes[k]->flags & ~NUGCUTSCENE_LOOP) | ((cinst->looping & 1) << 2);
                    inst->cutscenes[k]->flags =
                        (inst->cutscenes[k]->flags & ~NUGCUTSCENE_AUTOSTART) | ((cinst->autostart & 1) << 3);
                    inst->cutscenes[k]->flags =
                        (inst->cutscenes[k]->flags & ~NUGCUTSCENE_CLIPBOUNDS) | ((cinst->clipbbox & 1) << 5);
                    inst->cutscenes[k]->flags =
                        (inst->cutscenes[k]->flags & ~NUGCUTSCENE_CLIPDIST) | ((cinst->maxvisidist & 1) << 6);
                    if (cinst->ncamtgts) {
                        instNuGCutSceneCreateCamTgtArray(inst->cutscenes[k], cinst->ncamtgts, heap);
                        for (t = 0; t < cinst->ncamtgts; t++) {
                            if (strcasecmp(cinst->camtgts[t].name, "Player") == 0) {
                                instNuGCutSceneAddCamTgt(inst->cutscenes[k], (struct nuvec_s *)&player_mtx->_30,
                                                         cinst->camtgts[t].frame, cinst->camtgts[t].nframes,
                                                         cinst->camtgts[t].camid);
                            }
                        }
                    }
                    k++;
                }
            }
        }
    }
    return result;
}


int NuSceneInstanceRunScript(struct NuSceneInstance *inst, char *name) {
    struct NuSceneScript *script;
    struct NuSceneCommand *cmd;
    struct instNUGCUTSCENE_s *start;
    struct instNUGCUTSCENE_s *cutscene;
    struct instNUGCUTSCENE_s *chain;
    struct NuSceneChainArgs *chainargs;
    struct NuSceneTriggerFlagArgs *flagargs;
    struct NuSceneSpecialArgs *specialargs;
    struct nuhspecial_s special;

    for (script = inst->data->scripts; script; script = script->next) {
        if (strcasecmp(script->name, name) == 0) {
            for (cmd = script->commands; cmd; cmd = cmd->next) {
                switch (cmd->type) {
                case NUSCENECMD_STARTCUTSCENE:
                    start = SceneInstanceFindCutsceneInst(inst, *cmd->args.name);
                    if (start) {
                        instNuGCutSceneStart(start);
                    }
                    break;
                case NUSCENECMD_CHAINCUTSCENE:
                    chainargs = cmd->args.chain;
                    cutscene = SceneInstanceFindCutsceneInst(inst, chainargs->first);
                    chain = SceneInstanceFindCutsceneInst(inst, chainargs->next);
                    if (cutscene && chain) {
                        cutscene->chain = chain;
                    }
                    break;
                case NUSCENECMD_TRIGGERFLAG:
                    flagargs = cmd->args.flag;
                    instNuTriggerSetFlag(inst->triggers, flagargs->trigger, flagargs->index, flagargs->set);
                    break;
                case NUSCENECMD_TRIGGERRESET:
                    instNuTriggerReset(inst->triggers, *cmd->args.name);
                    break;
                case NUSCENECMD_FINISH:
                    inst->flags |= 1;
                    break;
                case NUSCENECMD_SPECIALVISIBLE:
                    specialargs = cmd->args.special;
                    if (NuSpecialFind(inst->gscene, &special, specialargs->special)) {
                        if (specialargs->visible) {
                            special.special->instance->flagword |= 1;
                        } else {
                            special.special->instance->flagword &= ~1;
                        }
                    }
                    break;
                }
            }
            return 1;
        }
    }
    return 0;
}


void LoadCutSceneInstanceData(struct nufpar_s *parser, struct NuSceneCutsceneInst *inst, union variptr_u *heap,
                              union variptr_u *heapend) {
    struct nuvec_s pos;
    char *word;
    char *namebuf;
    char *tgtname;
    int i;

    NuFParGetWord(parser);
    inst->name = heap->charptr;
    namebuf = heap->charptr + 1;
    heap->charptr = namebuf + strlen(parser->wbuff);
    sprintf(inst->name, parser->wbuff);
    inst->speed = NuFParGetFloat(parser);
    NuMtxSetIdentity(&inst->mtx);
    while (NuFParGetLine(parser)) {
        NuFParGetWord(parser);
        word = parser->wbuff;
        if (strcasecmp(word, "LOOPING") == 0) {
            inst->looping = 1;
        } else if (strcasecmp(word, "AUTOSTART") == 0) {
            inst->autostart = 1;
        } else if (strcasecmp(word, "CLIPBBOX") == 0) {
            inst->clipbbox = 1;
        } else if (strcasecmp(word, "MAXVISIDIST") == 0) {
            inst->maxvisidist = 1;
            inst->maxvisidist_sq = NuFParGetFloat(parser);
            inst->maxvisidist_sq *= inst->maxvisidist_sq;
        } else if (strcasecmp(word, "POSITION") == 0) {
            pos.x = NuFParGetFloat(parser);
            pos.y = NuFParGetFloat(parser);
            pos.z = NuFParGetFloat(parser);
            NuMtxTranslate(&inst->mtx, &pos);
            inst->position = 1;
        } else if (strcasecmp(word, "NUMCAMTGTS") == 0) {
            if (inst->ncamtgts) {
                NuErrorProlog("..\\nu2crash.ps2\\gamelib\\sceneman.c", 0x204)(
                    "LoadCutsceneData: defined NUMCAMTGTS more than once");
            }
            inst->ncamtgts = NuFParGetInt(parser);
            heap->intaddr = (heap->intaddr + 15) & ~15;
            inst->camtgts = heap->voidptr;
            heap->u8 += inst->ncamtgts * sizeof(struct NuSceneCamTarget);
            memset(inst->camtgts, 0, inst->ncamtgts * sizeof(struct NuSceneCamTarget));
            for (i = 0; i < inst->ncamtgts; i++) {
                NuFParGetLine(parser);
                NuFParGetWord(parser);
                if (strcasecmp(parser->wbuff, "ADDCAMTGT") != 0) {
                    NuErrorProlog("..\\nu2crash.ps2\\gamelib\\sceneman.c", 0x20E)(
                        "LoadCutsceneData: expecting ADDCAMTGT, found <%s>", parser->wbuff);
                }
                NuFParGetWord(parser);
                inst->camtgts[i].name = heap->charptr;
                tgtname = heap->charptr + 1;
                heap->charptr = tgtname + strlen(parser->wbuff);
                sprintf(inst->camtgts[i].name, parser->wbuff);
                inst->camtgts[i].frame = NuFParGetFloat(parser);
                inst->camtgts[i].nframes = NuFParGetFloat(parser);
                inst->camtgts[i].camid = NuFParGetInt(parser);
            }
        } else if (strcasecmp(word, "ENDINSTANCE") == 0) {
            return;
        }
    }
}


void LoadCutsceneData(struct nufpar_s *parser, char *dir, struct NuSceneData *scene, struct NuSceneCutscene *cutscene,
                      union variptr_u *heap, union variptr_u *heapend) {
    char path[256];
    char *word;
    char *namebuf;
    int i;

    NuFParGetWord(parser);
    sprintf(path, "%s\\%s.cut", dir, parser->wbuff);
    cutscene->def = NuGCutSceneLoad(path, heap, heapend);
    if (cutscene->def) {
        cutscene->name = heap->charptr;
        namebuf = heap->charptr + 1;
        heap->charptr = namebuf + strlen(parser->wbuff);
        sprintf(cutscene->name, parser->wbuff);
        while (NuFParGetLine(parser)) {
            NuFParGetWord(parser);
            word = parser->wbuff;
            if (strcasecmp(word, "ENDCUTSCENE") == 0) {
                break;
            }
            if (strcasecmp(word, "NUMINSTANCES") == 0) {
                if (cutscene->ninstances) {
                    NuErrorProlog("..\\nu2crash.ps2\\gamelib\\sceneman.c", 0x235)(
                        "LoadCutsceneData: defined NUMINSTANCES more than once", cutscene->name);
                }
                cutscene->ninstances = NuFParGetInt(parser);
                if (cutscene->ninstances) {
                    heap->intaddr = (heap->intaddr + 15) & ~15;
                    cutscene->instances = heap->voidptr;
                    heap->u8 += cutscene->ninstances * sizeof(struct NuSceneCutsceneInst);
                    memset(cutscene->instances, 0, cutscene->ninstances * sizeof(struct NuSceneCutsceneInst));
                    i = 0;
                }
            } else if (strcasecmp(word, "INSTANCE") == 0) {
                if (cutscene->ninstances == 0) {
                    NuErrorProlog("..\\nu2crash.ps2\\gamelib\\sceneman.c", 0x242)(
                        "LoadCutsceneData: need to define NUMINSTANCES before INSTANCE's", cutscene->name);
                }
                if (i >= cutscene->ninstances) {
                    NuErrorProlog("..\\nu2crash.ps2\\gamelib\\sceneman.c", 0x243)(
                        "LoadCutsceneData <%s>: There are more cutscenes instances than was defined by NUMINSTANCES", path);
                }
                LoadCutSceneInstanceData(parser, (struct NuSceneCutsceneInst *)cutscene->instances + i, heap, heapend);
                i++;
                scene->ninstances++;
            }
        }
    } else {
        NuErrorProlog("..\\nu2crash.ps2\\gamelib\\sceneman.c", 0x24B)(
            "LoadCutsceneData <%s>: Could not load cutscene <%s>", path);
    }
    if (cutscene->ninstances && cutscene->ninstances != i) {
        NuErrorProlog("..\\nu2crash.ps2\\gamelib\\sceneman.c", 0x250)(
            "LoadCutsceneData: mismatch between number of instance of <%s> declared <NUMINSTANCES %d> and found <%d>", cutscene->name, cutscene->ninstances, i);
    }
}


static inline void NuSceneScriptAddCommand(struct NuSceneScript *script, struct NuSceneCommand *last,
                                           struct NuSceneCommand *cmd, int type) {
    if (last) {
        last->next = cmd;
    } else {
        script->commands = cmd;
    }
    cmd->type = type;
}


static inline void NuSceneScriptReadName(struct nufpar_s *parser, char **name, union variptr_u *heap) {
    char *end;

    NuFParGetWord(parser);
    *name = heap->charptr;
    end = heap->charptr + 1;
    heap->charptr = end + strlen(parser->wbuff);
    sprintf(*name, parser->wbuff);
}


struct NuSceneScript *LoadCutsceneScript(struct nufpar_s *parser, struct NuSceneData *scene, union variptr_u *heap,
                                         union variptr_u *heapend) {
    struct NuSceneScript *script;
    struct NuSceneCommand *last;
    struct NuSceneCommand *cmd;
    struct NuSceneCommand *finish;
    union NuSceneCommandArgs args;
    char *word;
    char *namebuf;

    script = (struct NuSceneScript *)((heap->intaddr + 15) & ~15);
    heap->voidptr = script + 1;
    memset(script, 0, sizeof(struct NuSceneScript));
    NuFParGetWord(parser);
    script->name = heap->charptr;
    namebuf = heap->charptr + 1;
    heap->charptr = namebuf + strlen(parser->wbuff);
    sprintf(script->name, parser->wbuff);
    last = 0;
    while (NuFParGetLine(parser)) {
        NuFParGetWord(parser);
        word = parser->wbuff;
        if (strcasecmp(word, "ENDSCRIPT") == 0) {
            break;
        }
        if (strcasecmp(word, "STARTCUTSCENEINST") == 0) {
            cmd = (struct NuSceneCommand *)((heap->intaddr + 15) & ~15);
            heap->voidptr = cmd + 1;
            NuSceneScriptAddCommand(script, last, cmd, NUSCENECMD_STARTCUTSCENE);
            cmd->args.data = heap->voidptr;
            heap->u8 += sizeof(char *);
            memset(cmd->args.data, 0, sizeof(char *));
            last = cmd;
            args = cmd->args;
            NuSceneScriptReadName(parser, args.name, heap);
        } else if (strcasecmp(word, "CHAINCUTSCENEINST") == 0) {
            cmd = (struct NuSceneCommand *)((heap->intaddr + 15) & ~15);
            heap->voidptr = cmd + 1;
            NuSceneScriptAddCommand(script, last, cmd, NUSCENECMD_CHAINCUTSCENE);
            cmd->args.data = heap->voidptr;
            heap->u8 += sizeof(struct NuSceneChainArgs);
            memset(cmd->args.data, 0, sizeof(struct NuSceneChainArgs));
            last = cmd;
            args = cmd->args;
            NuSceneScriptReadName(parser, &args.chain->first, heap);
            NuSceneScriptReadName(parser, &args.chain->next, heap);
        } else if (strcasecmp(word, "SETTRIGGERFLAG") == 0) {
            cmd = (struct NuSceneCommand *)((heap->intaddr + 15) & ~15);
            heap->voidptr = cmd + 1;
            NuSceneScriptAddCommand(script, last, cmd, NUSCENECMD_TRIGGERFLAG);
            cmd->args.data = heap->voidptr;
            heap->u8 += sizeof(struct NuSceneTriggerFlagArgs);
            memset(cmd->args.data, 0, sizeof(struct NuSceneTriggerFlagArgs));
            last = cmd;
            args = cmd->args;
            args.flag->index = NuFParGetInt(parser);
            NuSceneScriptReadName(parser, &args.flag->trigger, heap);
            NuFParGetWord(parser);
            if (strcasecmp(parser->wbuff, "ON") == 0) {
                args.flag->set = 1;
            } else {
                args.flag->set = 0;
            }
        } else if (strcasecmp(word, "RESETTRIGGER") == 0) {
            cmd = (struct NuSceneCommand *)((heap->intaddr + 15) & ~15);
            heap->voidptr = cmd + 1;
            NuSceneScriptAddCommand(script, last, cmd, NUSCENECMD_TRIGGERRESET);
            cmd->args.data = heap->voidptr;
            heap->u8 += sizeof(char *);
            memset(cmd->args.data, 0, sizeof(char *));
            last = cmd;
            args = cmd->args;
            NuSceneScriptReadName(parser, &args.flag->trigger, heap);
        } else if (strcasecmp(word, "SCENEFINISHED") == 0) {
            finish = (struct NuSceneCommand *)((heap->intaddr + 15) & ~15);
            heap->voidptr = finish + 1;
            NuSceneScriptAddCommand(script, last, finish, NUSCENECMD_FINISH);
            last = finish;
        } else if (strcasecmp(word, "SETVISIBILITY") == 0) {
            cmd = (struct NuSceneCommand *)((heap->intaddr + 15) & ~15);
            heap->voidptr = cmd + 1;
            NuSceneScriptAddCommand(script, last, cmd, NUSCENECMD_SPECIALVISIBLE);
            cmd->args.data = heap->voidptr;
            heap->u8 += sizeof(struct NuSceneSpecialArgs);
            memset(cmd->args.data, 0, sizeof(struct NuSceneSpecialArgs));
            last = cmd;
            args = cmd->args;
            NuSceneScriptReadName(parser, &args.special->special, heap);
            NuFParGetWord(parser);
            if (strcasecmp(parser->wbuff, "ON") == 0) {
                args.special->visible = 1;
            } else {
                args.special->visible = 0;
            }
        }
    }
    return script;
}


void NuSceneManagerSysInit(void) {
    sceneinstances = 0;
}


void NuSceneManagerSysUpdate(void) {
    struct NuSceneInstance *inst;
    struct NuSceneInstance *next;

    for (inst = sceneinstances; inst; inst = next) {
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
        sceneinstances = inst->next;
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

    for (inst = sceneinstances; inst; inst = next) {
        next = inst->next;
        if (!(inst->flags & NUSCENEINST_DISABLED)) {
            NuGScnRndr3(inst->gscene);
        }
    }
}


void NuSceneManagerSetPtrToPlayerMtx(struct numtx_s *mtx) {
    player_mtx = mtx;
}
