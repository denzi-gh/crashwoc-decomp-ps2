#ifndef GAME_SFX_H
#define GAME_SFX_H

#include "nu_types.h"

struct pSFX;

void GameMusic(s32 sfx, s32 i);
void InitLocalSfx(struct pSFX *sfxTab, s32 totalCount);
void ResetGameSfx(void);

#endif /* GAME_SFX_H */
