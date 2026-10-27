#ifndef GAME_BUG_H
#define GAME_BUG_H

#include "nu_types.h"

struct creature_s;

/* main.c passes the player, which it ignores (as retail does). */
void AddBugLight();
void DrawBug(void);
void ResetBug(void);
void UpdateBugLight(struct creature_s *c);

#endif /* GAME_BUG_H */
