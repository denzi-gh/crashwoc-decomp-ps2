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

extern void AppCutSceneFindCharacters(void *cutscene);
extern void AppCutSceneCharacterRender(void);
extern void NuGCutSceneSysInit(void *fns);
extern void NuSetCutSceneFindCharactersFn(void *fn);
extern void NuSetCutSceneCharacterRenderFn(void *fn);
extern s32 NuSoundKeyStatus(s32 chan);
extern void SetCutMovieRate(void);
extern void NuGCutSceneSysUpdate(s32 paused);


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

void CutSceneNextLogoFunction(void *icutscene) {
    D_00633388++;
    NewMenu(Cursor, 0, 0, -1);
    if (logos_played == 0) {
        GameMusic(0xaa, 0);
    }
}

void *FindCutChar(char *name) {
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


void DrawCutMovie(void) {
    NuGCutSceneSysRender();
}

void NewCut(s32 i) {
    gamecut = i;
    Game.cutbits |= 1 << i;
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
