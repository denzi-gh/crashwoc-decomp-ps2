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

typedef struct NuMtx {
    float m[4][4];
} NuMtx;

typedef struct NuVec {
    float x;
    float y;
    float z;
} NuVec;

typedef struct NuGScn NuGScn;
typedef struct NuTriggerSys NuTriggerSys;
typedef struct NuGCutSceneDef NuGCutSceneDef;

typedef struct NuFPar {
    int handle;
    char buffer[0x1000];
    char line[0x101];
    char word[0x101];
    int line_num;
} NuFPar;

typedef void (*NuErrorFunc)(const char *fmt, ...);

typedef struct NuGCutScene {
    struct NuGCutScene *next;       /* 0x00 */
    struct NuGCutScene *prev;       /* 0x04 */
    char name[16];                  /* 0x08 */
} NuGCutScene;

typedef struct NuSceneCutscene {
    char *name;                     /* 0x00 */
    NuGCutSceneDef *def;            /* 0x04 */
    int ninstances;                 /* 0x08 */
    void *instances;                /* 0x0C */
} NuSceneCutscene;

typedef struct NuSceneScript {
    struct NuSceneScript *next;     /* 0x00 */
    char *name;                     /* 0x04 */
    void *commands;                 /* 0x08 */
} NuSceneScript;

typedef struct NuSceneData {
    char *name;                     /* 0x00 */
    NuGScn *gscene;                 /* 0x04 */
    short *terrain;                 /* 0x08 */
    void *specialterrain;           /* 0x0C */
    void *triggers;                 /* 0x10 */
    int ncutscenes;                 /* 0x14 */
    NuSceneCutscene *cutscenes;     /* 0x18 */
    int ninstances;                 /* 0x1C */
    NuSceneScript *scripts;         /* 0x20 */
} NuSceneData;

typedef struct NuSceneInstance {
    struct NuSceneInstance *next;   /* 0x00 */
    struct NuSceneInstance *prev;   /* 0x04 */
    int reserved;                   /* 0x08 */
    NuSceneData *data;              /* 0x0C */
    NuGScn *gscene;                 /* 0x10 */
    NuTriggerSys *triggers;         /* 0x14 */
    float rate;                     /* 0x18 */
    int flags;                      /* 0x1C */
    NuGCutScene **cutscenes;        /* 0x20 */
} NuSceneInstance;

#define NUSCENEINST_DISABLED 0x2

extern int sprintf(char *buf, const char *fmt, ...);
extern int strcasecmp(const char *a, const char *b);
extern unsigned int strlen(const char *s);
extern void *memset(void *dst, int value, unsigned int size);

extern NuErrorFunc NuErrorProlog(const char *file, int line);

extern NuFPar *NuFParCreate(char *name);
extern void NuFParDestroy(NuFPar *parser);
extern int NuFParGetLine(NuFPar *parser);
extern int NuFParGetWord(NuFPar *parser);
extern int NuFParGetInt(NuFPar *parser);

extern NuGScn *NuGScnRead(char **heap, char *heapend, char *name);
extern short *terraininit(int levelnum, char **heap, char *heapend, int opt, char *name,
                          NuGScn *gscene, int gsid);
extern void *CreateSpecialTerrainData(int levelnum, NuGScn *gscene, char **heap, char **heapend);
extern void *NuTriggerSysLoad(char *name, char **heap, char **heapend);
extern void NuGCutSceneFixUp(NuGCutSceneDef *def, NuGScn *gscene, void *triggers);

extern void NuGScnUpdate(NuGScn *scene, float time);
extern void NuGScnRndr3(NuGScn *scene);

extern void instNuTriggerSysDestroy(NuTriggerSys *sys);
extern void instNuTriggerEnable(NuTriggerSys *sys);
extern void instNuTriggerDisable(NuTriggerSys *sys);

extern void instNuGCutSceneDestroy(NuGCutScene *scene);
extern void instNuGCutSceneTranslate(NuGCutScene *scene, NuVec *offset);
extern void instNuGCutSceneRotateY(NuGCutScene *scene, int angle);
extern void instNuGCutSceneEnable(NuGCutScene *scene);
extern void instNuGCutSceneDisable(NuGCutScene *scene);

extern void LoadCutsceneData(NuFPar *parser, char *dir, NuSceneData *scene, NuSceneCutscene *cutscene,
                             char **heap, char **heapend);
extern NuSceneScript *LoadCutsceneScript(NuFPar *parser, NuSceneData *scene, char **heap,
                                         char **heapend);

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

extern NuSceneInstance *D_0062F808;
extern NuMtx *D_0062F80C;
#define sceneinst_list D_0062F808
#define player_mtx D_0062F80C


NuSceneData *NuSceneDataLoad(char *dir, char **heap, char **heapend) {
    char path[256];
    NuSceneData *result;
    NuSceneScript *lastscript;
    NuSceneScript *script;
    NuSceneData *scene;
    NuFPar *parser;
    char *word;
    char *namebuf;
    int cutscene;

    result = 0;
    sprintf(path, scenecfg_fmt, dir);
    lastscript = 0;
    parser = NuFParCreate(path);
    if (parser) {
        scene = (NuSceneData *)(((unsigned int)*heap + 15) & ~15);
        *heap = (char *)scene + sizeof(NuSceneData);
        result = scene;
        memset(scene, 0, sizeof(NuSceneData));
        while (NuFParGetLine(parser)) {
            NuFParGetWord(parser);
            word = parser->word;
            if (strcasecmp(word, kw_scene) == 0) {
                NuFParGetWord(parser);
                *heap = (char *)(((unsigned int)*heap + 15) & ~15);
                scene->name = *heap;
                namebuf = *heap + 1;
                *heap = namebuf + strlen(word);
                sprintf(scene->name, word);
                if (scene->gscene == 0) {
                    *heap = (char *)(((unsigned int)*heap + 15) & ~15);
                    sprintf(path, gscfile_fmt, dir, word);
                    scene->gscene = NuGScnRead(heap, *heapend, path);
                    if (scene->gscene) {
                        sprintf(path, terfile_fmt, dir, word);
                        scene->terrain = terraininit(0, heap, *heapend, 0, path, scene->gscene, 0);
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
                    *heap = (char *)(((unsigned int)*heap + 15) & ~15);
                    scene->cutscenes = (NuSceneCutscene *)*heap;
                    *heap += scene->ncutscenes * sizeof(NuSceneCutscene);
                    memset(scene->cutscenes, 0, scene->ncutscenes * sizeof(NuSceneCutscene));
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
    NuSceneInstance *inst;
    NuSceneInstance *next;

    for (inst = sceneinst_list; inst; inst = next) {
        next = inst->next;
        if (!(inst->flags & NUSCENEINST_DISABLED)) {
            NuGScnUpdate(inst->gscene, inst->rate);
        }
    }
}


void NuSceneInstanceDestroy(NuSceneInstance *inst) {
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


void NuSceneInstanceTranslate(NuSceneInstance *inst, NuVec *offset) {
    int i;

    for (i = 0; i < inst->data->ninstances; i++) {
        instNuGCutSceneTranslate(inst->cutscenes[i], offset);
    }
}


void NuSceneInstanceRotateY(NuSceneInstance *inst, int angle) {
    int i;

    for (i = 0; i < inst->data->ninstances; i++) {
        instNuGCutSceneRotateY(inst->cutscenes[i], angle);
    }
}


void NuSceneInstanceEnable(NuSceneInstance *inst) {
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


void NuSceneInstanceDisable(NuSceneInstance *inst) {
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


void NuSceneInstanceSetRate(NuSceneInstance *inst, float rate) {
    inst->rate = rate;
}


NuGCutScene *SceneInstanceFindCutsceneInst(NuSceneInstance *inst, char *name) {
    int i;

    for (i = 0; i < inst->data->ninstances; i++) {
        if (strcasecmp(inst->cutscenes[i]->name, name) == 0) {
            return inst->cutscenes[i];
        }
    }
    return 0;
}


void NuSceneManagerSysRender(void) {
    NuSceneInstance *inst;
    NuSceneInstance *next;

    for (inst = sceneinst_list; inst; inst = next) {
        next = inst->next;
        if (!(inst->flags & NUSCENEINST_DISABLED)) {
            NuGScnRndr3(inst->gscene);
        }
    }
}


void NuSceneManagerSetPtrToPlayerMtx(NuMtx *mtx) {
    player_mtx = mtx;
}
