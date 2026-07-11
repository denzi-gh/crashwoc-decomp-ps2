/*
 * Unit: game/game_deb
 *
 * Functions:
 *   0x0025fb08 AddAnimDebris
 *   0x00260ae8 InitGameDebris
 *   0x00260b60 AddGameDebris
 *   0x00260bb8 AddGameDebrisRot
 *   0x00260c20 AddGameDebrisMtx
 *   0x00260cd8 AddWarpDebris
 *   0x00260d50 AddMechanicalDebris
 */

#include "creature.h"

extern void *D_0060E1D0[];

extern void AddFiniteShotDebrisEffect(s32 *key, void *effect,
                                      struct nuvec_s *pos, s32 enabled);

void AddWarpDebris(struct obj_s *obj)
{
    struct nuvec_s pos;
    s32 key;

    pos.x = obj->pos.x;
    pos.y = obj->pos.y + (obj->bot + obj->top) * obj->SCALE * 0.5f;
    pos.z = obj->pos.z;
    key = -1;
    AddFiniteShotDebrisEffect(&key, D_0060E1D0[0], &pos, 1);
}
