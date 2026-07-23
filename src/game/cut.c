/*
 * Unit: game/cut
 *
 * Functions:
 *   0x0025b850 AppCutSceneCharacterRender
 *   0x0025ba88 LoadCutComponents
 *   0x0025bd98 LoadCutMovie
 *   0x0025c050 SetCutMovieRate
 *   0x0025c0e8 CloseCutMovie
 *   0x0025c210 StartCutMovie
 *   0x0025c368 PlayCutMovie
 *   0x0025c8c0 InitGameCut
 *   0x0025ca10 UpdateGameCut
 *   0x0025ce20 DrawGameCut
 *   0x0025d0d0 GetSpaceCut
 *   0x0025d4e0 SetCutSceneLights
 *   0x0025e598 InitCutScenes
 *   0x0025e5d0 UpdateCutMovie
 *   0x0025e650 DrawCutMovie
 *   0x0025e670 NewCut
 *   0x0025e698 LoadCutSceneCharacters
 *   0x0025e798 FindCutChar
 *   0x0025e808 AppCutSceneFindCharacters
 *   0x0025e948 locatorfn_fadeout
 *   0x0025ea08 CutSceneEndFunction
 *   0x0025ea60 CutSceneNextFunction
 *   0x0025eb48 CutSceneNextLogoFunction
 *   0x0025eb98 UpdateCutMovieCamera
 *   0x0025ecd0 NewGameCutAnim
 *   0x0025ed88 MakeDirLightColour
 *   0x0025edb8 MakeLightDirection
 */

#include "creature.h"

struct game_s {
    u8 unk_0x00[0x408];
    s32 cutbits;          /* 0x408 */
};

extern struct game_s Game;
extern s32 gamecut;
extern struct nucolour3_s cutdircol[];
extern struct nuvec_s cutdir[];

extern void NuGCutSceneSysRender(void);
extern void NuVecNorm(struct nuvec_s *out, struct nuvec_s *in);

extern s32 cutmovie;
extern s32 cut_on;
extern s32 logos_played;
extern char Cursor[];
extern s32 D_00633388;                 /* cutworldix */

struct csc_s {
    void *obj;            /* 0x0 */
    char *path;           /* 0x4 */
    char *name;           /* 0x8 */
};                        /* 0xC */

extern struct csc_s *CutChar;

struct csccache_s {
    char *path;          /* 0x0 */
    void *obj;           /* 0x4 */
};
extern struct csccache_s D_006FF880[];     /* cut char cache */
extern s32 D_0063338C;                     /* cut char cache count */
extern void *superbuffer_ptr;
extern void *NuGHGRead(void *buf, char *path);

extern void NewMenu(void *cursor, s32 a, s32 b, s32 c);
extern void GameMusic(s32 sfx, s32 i);
extern s32 strcasecmp(char *a, char *b);

extern s32 cutmovie_hold;
extern s32 Paused;
extern u8 D_0060B398[];                 /* cutscene_locatorfns */

struct cutinst_s {
    u8 unk_0x00[0x74];
    f32 rate;             /* 0x74 */
};
extern struct cutinst_s *D_006FF700[];  /* CutInst */
extern s32 D_006FF780[];                /* CutAudio */
#define CutInst D_006FF700
#define CutAudio D_006FF780
#define cutworldix D_00633388

static void AppCutSceneFindCharacters();
extern void AppCutSceneCharacterRender(void);
extern void NuGCutSceneSysInit(void *fns);
extern void NuSetCutSceneFindCharactersFn(void *fn);
extern void NuSetCutSceneCharacterRenderFn(void *fn);
extern s32 NuSoundKeyStatus(s32 chan);
extern void SetCutMovieRate(void);
extern void NuGCutSceneSysUpdate(s32 paused);

extern f32 D_0062E888;
extern f32 D_0062E88C;
extern f32 D_00632BD8;

struct pcutanm_s {
    s16 character;    /* 0x0 */
    s16 action;       /* 0x2 */
    s16 sfx;          /* 0x4 */
    s8 i;             /* 0x6 */
    s8 pad1;          /* 0x7 */
};
extern struct pcutanm_s *pCutAnim;
extern s32 gamecut_hold;
extern s32 gamecut_sfx;
extern s32 gamesfx_channel;
extern s32 Level;
extern u8 D_0058B117[];
extern void NuSoundStopStream(s32 chan);
extern void GameSfx(s32 sfx, s32 a);

struct nugcutlocator_s {
    u8 pad0[0x50];
    void *anim;                      /* 0x50 */
    u8 pad1[0x64 - 0x54];
};                                   /* 0x64 */
struct nugcutlocatorsys_s {
    struct nugcutlocator_s *locators; /* 0x0 */
};
struct nugcutchar_s {
    u8 mtx[0x40];                    /* 0x0 */
    char *name;                      /* 0x40 */
    u8 pad1[0x50 - 0x44];
    void *character;                 /* 0x50 */
    struct nugcutlocator_s *locators; /* 0x54 */
    u8 pad2[0x5E - 0x58];
    u8 nlocators;                    /* 0x5E */
    u8 pad3;                         /* 0x5F */
    u8 first_locator;                /* 0x60 */
    u8 pad4[0x64 - 0x61];
};                                   /* 0x64 */
struct nugcutcharsys_s {
    struct nugcutchar_s *chars;      /* 0x0 */
    u16 nchars;                      /* 0x4 */
};
struct nugcutscene_s {
    u8 pad0[0x8];
    f32 nframes;                     /* 0x8 */
    u8 pad1[0x18 - 0xC];
    struct nugcutcharsys_s *chars;   /* 0x18 */
    struct nugcutlocatorsys_s *locators; /* 0x1C */
};
struct instnugcutscene_s {
    u8 pad0[0x58];
    struct nugcutscene_s *cutscene;  /* 0x58 */
    u8 pad1[0x70 - 0x5C];
    f32 cframe;                      /* 0x70 */
};
struct instnugcutlocator_s {
    f32 timer;                       /* 0x0 */
    void *data;                      /* 0x4 */
};
struct nuanimtime_s {
    f32 time;                        /* 0x0 */
    f32 time_offset;                 /* 0x4 */
    s32 chunk;                       /* 0x8 */
    u32 time_mask;                   /* 0xC */
    u32 time_byte;                   /* 0x10 */
};

extern s32 fade_rate;
extern s32 fadeval;
extern void NuSoundUpdate(void);
extern void sceGsSyncV(s32 mode);
extern void *world_scene[];
extern void instNuGCutSceneDestroy(void *inst);
extern void NuHGobjDestroy(void *obj);
extern void NuSoundFlushLoops(void);
extern void NuTexInit(void);
extern void NuMtlRelease(void);
extern void NuGobjInit(void);
extern s32 set_cutscenecammtx;
extern struct numtx_s cutscenecammtx;
extern struct numtx_s *pNuCam;
extern void *memcpy(void *dst, void *src, s32 n);
extern void NuCameraSet(void *cam);

extern struct nuvec_s cutpos_CRASH;
extern struct nuvec_s cutpos_FRONTEND[];
extern struct nuvec_s cutpos_SPACE[];
extern struct nuvec_s campos_SPACE;
extern u16 cutang_FRONTEND[2];
extern u16 cutang_SPACE[3];
extern void *SpaceGameCutTab[][2];
extern u8 CutAnim[];
extern u8 CutVortexAnim[];
extern s32 gamecut_finished;
extern s32 NuAtan2D(f32 x, f32 z);
extern void NuAnimData2CalcTime(void *anim, f32 currf, struct nuanimtime_s *atime);
extern s32 NuGCutLocatorIsVisble(void *locator, f32 currf, struct nuanimtime_s *atime,
                                 void *null);


void SetCutMovieRate(void) {
    CutInst[cutworldix]->rate = 0.5f;
    if (cutmovie == 3 || cutmovie == 4) {
        if (cutworldix == 0) {
            CutInst[cutworldix]->rate *= D_0062E888;
        }
    }
    CutInst[cutworldix]->rate *= D_0062E88C;
    if (D_00632BD8 != 0.0f) {
        CutInst[cutworldix]->rate = D_00632BD8;
    }
}


void CloseCutMovie(s32 all) {
    s32 i;

    for (i = 0; i < 0x20; i++) {
        if (CutInst[i] != 0) {
            instNuGCutSceneDestroy(CutInst[i]);
            CutInst[i] = 0;
        }
    }
    if (D_0063338C != 0) {
        for (i = 0; i < D_0063338C; i++) {
            NuHGobjDestroy(D_006FF880[i].obj);
        }
    }
    D_0063338C = 0;
    for (i = 0; i < 0x20; i++) {
        world_scene[i] = 0;
    }
    NuSoundUpdate();
    NuSoundStopStream(0);
    NuSoundStopStream(1);
    NuSoundStopStream(2);
    NuSoundStopStream(3);
    NuSoundStopStream(4);
    NuSoundUpdate();
    NuSoundFlushLoops();
    if (all != 0) {
        NuTexInit();
        NuMtlRelease();
        NuGobjInit();
    }
}

void InitGameCut(void) {
    cutang_FRONTEND[0] = (u16)NuAtan2D(cutpos_CRASH.x - cutpos_FRONTEND[0].x,
                                       cutpos_CRASH.z - cutpos_FRONTEND[0].z);
    cutang_FRONTEND[1] = (u16)NuAtan2D(cutpos_CRASH.x - cutpos_FRONTEND[1].x,
                                       cutpos_CRASH.z - cutpos_FRONTEND[1].z);
    cutang_SPACE[0] = (u16)NuAtan2D(campos_SPACE.x - cutpos_SPACE[0].x,
                                    campos_SPACE.z - cutpos_SPACE[0].z);
    cutang_SPACE[1] = (u16)NuAtan2D(campos_SPACE.x - cutpos_SPACE[1].x,
                                    campos_SPACE.z - cutpos_SPACE[1].z);
    cutang_SPACE[2] = (u16)NuAtan2D(campos_SPACE.x - cutpos_SPACE[2].x,
                                    campos_SPACE.z - cutpos_SPACE[2].z);
    pCutAnim = (struct pcutanm_s *)SpaceGameCutTab[gamecut][1];
    ResetAnimPacket(CutAnim, pCutAnim->action);
    gamecut_hold = 1;
    gamecut_sfx = -1;
    gamecut_finished = 0;
    ResetAnimPacket(CutVortexAnim, 0x22);
}

void InitCutScenes(void) {
    NuGCutSceneSysInit(D_0060B398);
    NuSetCutSceneFindCharactersFn((void *)AppCutSceneFindCharacters);
    NuSetCutSceneCharacterRenderFn((void *)AppCutSceneCharacterRender);
}

void UpdateCutMovie(void) {
    if (cutmovie_hold != 0 && CutAudio[cutworldix] != -1 &&
        NuSoundKeyStatus(4) != 1) {
        CutInst[cutworldix]->rate = 0.0f;
    } else {
        cutmovie_hold = 0;
        SetCutMovieRate();
    }
    NuGCutSceneSysUpdate(Paused);
}


void locatorfn_fadeout(struct instnugcutscene_s *icutscene, void *locatorsys,
                       struct instnugcutlocator_s *ilocator,
                       struct nugcutlocator_s *locator, f32 currf, void *wm) {
    struct nuanimtime_s atime;
    f32 remaining;

    if (locator->anim != 0) {
        NuAnimData2CalcTime(locator->anim, currf, &atime);
        if (NuGCutLocatorIsVisble(locator, currf, &atime, 0) != 0 &&
            ilocator->data == 0) {
            ilocator->data = (void *)0x1;
            remaining = 255.0f / (icutscene->cutscene->nframes - icutscene->cframe);
            fade_rate = remaining < 1.0f ? 1 : (s32)remaining;
        }
    }
}

void CutSceneEndFunction(void *icutscene) {
    if (cutmovie == 0) {
        NewMenu(Cursor, 0, 0, -1);
        if (logos_played == 0) {
            GameMusic(0xaa, 0);
        }
    } else {
        cut_on = 0;
    }
}

void CutSceneNextFunction(void *icutscene) {
    s32 sfx;

    cutworldix++;
    if (CutAudio[cutworldix] != -1) {
        NuSoundStopStream(4);
        do {
            NuSoundUpdate();
            if (NuSoundKeyStatus(4) == 1) {
                sceGsSyncV(0);
            }
        } while (NuSoundKeyStatus(4) == 1);
        sfx = CutAudio[cutworldix];
        if (cutmovie != 0) {
            sfx += D_0058B117[0] * 4;
        }
        gamesfx_channel = 4;
        GameSfx(sfx, 0);
        cutmovie_hold = 1;
    }
    if (fadeval != 0) {
        fade_rate = -10;
    }
}

void CutSceneNextLogoFunction(void *icutscene) {
    D_00633388++;
    NewMenu(Cursor, 0, 0, -1);
    if (logos_played == 0) {
        GameMusic(0xaa, 0);
    }
}

void LoadCutSceneCharacters(struct csc_s *name) {
    s32 i;
    void *obj;
    void *tmp;
    char *path;

    while (name->path != 0) {
        path = name->path;
        for (i = 0; i < D_0063338C; i++) {
            if (strcasecmp(path, D_006FF880[i].path) == 0) {
                obj = D_006FF880[i].obj;
                goto found;
            }
        }
        D_006FF880[i].path = path;
        tmp = NuGHGRead(&superbuffer_ptr, path);
        D_006FF880[i].obj = tmp;
        obj = tmp;
        D_0063338C++;
    found:
        name->obj = obj;
        name++;
    }
}

inline void *FindCutChar(char *name) {
    struct csc_s *chr;

    chr = CutChar;
    if (CutChar != 0) {
        for (; chr->path != 0; chr++) {
            if (strcasecmp(name, chr->name) == 0) {
                return chr->obj;
            }
        }
    }
    return 0;
}

typedef void (*NuDebugMsgFn)();
extern NuDebugMsgFn NuDebugMsgProlog(char *file, s32 line);
extern char D_00632BC0[];
extern char D_00629350[];

static void AppCutSceneFindCharacters(struct nugcutscene_s *cutscene) {
    struct nugcutcharsys_s *charSys;
    struct nugcutchar_s *cutchar;
    struct nugcutlocatorsys_s *locator;
    s32 i;

    charSys = cutscene->chars;
    locator = cutscene->locators;
    for (i = 0; i < charSys->nchars; i++) {
        cutchar = &charSys->chars[i];
        cutchar->character = FindCutChar(cutchar->name);
        if (cutchar->character == 0) {
            NuDebugMsgProlog(D_00632BC0, 0x2a1)(D_00629350, cutchar->name);
        }
        if (cutchar->nlocators != 0 && (s32)cutchar->locators < 0xff) {
            cutchar->first_locator = (u8)(s32)cutchar->locators;
            cutchar->locators = &locator->locators[(s32)cutchar->locators];
        } else {
            cutchar->first_locator = -1;
        }
    }
}


void DrawCutMovie(void) {
    NuGCutSceneSysRender();
}

void NewCut(s32 i) {
    gamecut = i;
    Game.cutbits |= 1 << i;
}

void UpdateCutMovieCamera(struct numtx_s *cam) {
    if (set_cutscenecammtx != 0) {
        *cam = cutscenecammtx;
        set_cutscenecammtx = 0;
        *pNuCam = *cam;
        NuCameraSet(pNuCam);
    }
}

void NewGameCutAnim(void) {
    s32 sfx;

    gamecut_hold = 1;
    gamecut_sfx = pCutAnim->sfx;
    if (gamecut_sfx != -1) {
        NuSoundStopStream(4);
        gamesfx_channel = 4;
        if (Level == 0x25) {
            GameSfx(gamecut_sfx + D_0058B117[0] * 12, 0);
        } else {
            GameSfx(gamecut_sfx, 0);
        }
    }
    if (pCutAnim->action == 0xc) {
        sfx = 0x1e;
    } else if (pCutAnim->action == 0xd) {
        sfx = 0x21;
    } else {
        sfx = -1;
    }
    if (sfx != -1) {
        GameSfx(sfx, 0);
    }
}

void MakeDirLightColour(s32 ix, f32 r, f32 g, f32 b) {
    cutdircol[ix].r = r;
    cutdircol[ix].g = g;
    cutdircol[ix].b = b;
}

void MakeLightDirection(s32 ix, f32 x, f32 y, f32 z) {
    cutdir[ix].x = x;
    cutdir[ix].y = y;
    cutdir[ix].z = z;
    NuVecNorm(&cutdir[ix], &cutdir[ix]);
}
