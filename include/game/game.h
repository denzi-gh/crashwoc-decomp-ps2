#ifndef GAME_GAME_H
#define GAME_GAME_H

#include "nu_types.h"
#include "game/gametypes.h"

typedef struct {
    struct numtx_s mM;                  /* 0x00 */
    struct numtx_s mS;                  /* 0x40 */
    float time;                         /* 0x80 */
    float duration;                     /* 0x84 */
    float shadow;                       /* 0x88 */
    char pad1;                          /* 0x8C */
    char pad2;                          /* 0x8D */
    char pad3;                          /* 0x8E */
    char pad4;                          /* 0x8F */
} MaskFeathersInfo; /* 0x90 */


void DefaultGame(void);
void DrawAwards(void);
void DrawKabooms(void);
void DrawLevel(void);
void DrawMask(MaskInfo *mask);
void DrawMaskFeathers(void);
void DrawNextVehicle(s32 render);
void DrawTarget(void);
void DrawTempCharacter(s32 render);
void DrawTempCharacter2(s32 render);
void DrawTransporters(void);
void DrawWumpa(void);
void GameTiming(void);
/* main.c calls it without the argument, which it never reads (as retail does). */
void InitGameMode();
void NewGame(void);
void ResetAwards(void);
void ResetBonus(void);
void ResetDeath(void);
void ResetGame(void);
void ResetGemPath(void);
void ResetLevel(void);
void ResetWumpa(void);
void UpdateAwards(void);
void UpdateKabooms(void);
void UpdateLevel(void);
void UpdateMaskFeathers(void);
void UpdateTempCharacter(void);
void UpdateWumpa(void);

#endif /* GAME_GAME_H */
