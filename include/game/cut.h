#ifndef GAME_CUT_H
#define GAME_CUT_H

#include "nu_types.h"

struct csc_s {
    void *obj;            /* 0x0 */
    char *path;           /* 0x4 */
    char *name;           /* 0x8 */
};                        /* 0xC */

struct csccache_s {
    char *path;          /* 0x0 */
    void *obj;           /* 0x4 */
};

struct cutscenedesc_s {
    char *gscname;        /* 0x0 scene (.gsc) */
    char *cutname;        /* 0x4 cutscene (.cut) */
    s32 sfxid;            /* 0x8 audio stream */
    s32 debgroup;         /* 0xC */
};                        /* 0x10 */

struct scenecache_s {
    char *path;           /* 0x0 */
    void *scene;          /* 0x4 */
};

void CloseCutMovie(s32 all);
void DrawGameCut(void);
void PlayCutMovie(s32 movie);
void UpdateGameCut(void);

#endif /* GAME_CUT_H */
