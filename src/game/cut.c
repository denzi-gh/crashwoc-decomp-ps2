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
