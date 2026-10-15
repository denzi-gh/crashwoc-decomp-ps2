#ifndef GAMELIB_SCENEMAN_H
#define GAMELIB_SCENEMAN_H

#include "numath/numathtypes.h"

struct nugscn_s;
struct NUGCUTSCENE_s;
struct instNUGCUTSCENE_s;
struct NUTRIGGERSYS_s;
struct instNUTRIGGERSYS_s;
struct NuFPar;
union variptr_u;

struct NuSceneCutscene {
    char *name;                         /* 0x00 */
    struct NUGCUTSCENE_s *def;          /* 0x04 */
    int ninstances;                     /* 0x08 */
    void *instances;                    /* 0x0C */
}; /* 0x10 */

struct NuSceneScript {
    struct NuSceneScript *next;         /* 0x00 */
    char *name;                         /* 0x04 */
    void *commands;                     /* 0x08 */
}; /* 0x0C */

struct NuSceneData {
    char *name;                         /* 0x00 */
    struct nugscn_s *gscene;            /* 0x04 */
    short *terrain;                     /* 0x08 */
    void *specialterrain;               /* 0x0C */
    struct NUTRIGGERSYS_s *triggers;    /* 0x10 */
    int ncutscenes;                     /* 0x14 */
    struct NuSceneCutscene *cutscenes;  /* 0x18 */
    int ninstances;                     /* 0x1C */
    struct NuSceneScript *scripts;      /* 0x20 */
}; /* 0x24 */

struct NuSceneInstance {
    struct NuSceneInstance *next;       /* 0x00 */
    struct NuSceneInstance *prev;       /* 0x04 */
    int reserved;                       /* 0x08 */
    struct NuSceneData *data;           /* 0x0C */
    struct nugscn_s *gscene;            /* 0x10 */
    struct instNUTRIGGERSYS_s *triggers; /* 0x14 */
    float rate;                         /* 0x18 */
    int flags;                          /* 0x1C */
    struct instNUGCUTSCENE_s **cutscenes; /* 0x20 */
}; /* 0x24 */

struct NuSceneData *NuSceneDataLoad(char *dir, union variptr_u *heap, union variptr_u *heapend);
int NuSceneInstanceRunScript(struct NuSceneInstance *inst, char *name);
void LoadCutsceneData(struct NuFPar *parser, char *dir, struct NuSceneData *scene, struct NuSceneCutscene *cutscene,
                      union variptr_u *heap, union variptr_u *heapend);
struct NuSceneScript *LoadCutsceneScript(struct NuFPar *parser, struct NuSceneData *scene, union variptr_u *heap,
                                         union variptr_u *heapend);
void NuSceneManagerSysInit(void);
void NuSceneManagerSysUpdate(void);
void NuSceneInstanceDestroy(struct NuSceneInstance *inst);
void NuSceneInstanceTranslate(struct NuSceneInstance *inst, struct nuvec_s *offset);
void NuSceneInstanceRotateY(struct NuSceneInstance *inst, int angle);
void NuSceneInstanceEnable(struct NuSceneInstance *inst);
void NuSceneInstanceDisable(struct NuSceneInstance *inst);
void NuSceneInstanceSetRate(struct NuSceneInstance *inst, float rate);
struct instNUGCUTSCENE_s *SceneInstanceFindCutsceneInst(struct NuSceneInstance *inst, char *name);
void NuSceneManagerSysRender(void);
void NuSceneManagerSetPtrToPlayerMtx(struct numtx_s *mtx);

#endif
