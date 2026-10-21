#ifndef GAME_GAME_OBJ_H
#define GAME_GAME_OBJ_H

#include "nu_types.h"

struct obj_s;

void DrawProjectiles(void);
void ResetProjectiles(void);
void UpdateProjectiles(void);
s32 WumpaCollisions(struct obj_s *obj);

#endif /* GAME_GAME_OBJ_H */
