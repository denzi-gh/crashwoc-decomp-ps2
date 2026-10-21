#ifndef GAME_CRATE_H
#define GAME_CRATE_H

#include "nu_types.h"

struct obj_s;

void CloseCrates(void);
void DrawCrateExplosions(void);
void DrawCrates(void);
s32 HitCrates(struct obj_s *obj, s32 destroy);
void InitCrateExplosions(void);
void ResetCrates(void);
void UpdateCrateExplosions(void);
void UpdateCrates(void);

#endif /* GAME_CRATE_H */
