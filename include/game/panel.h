#ifndef GAME_PANEL_H
#define GAME_PANEL_H

#include "nu_types.h"

struct creature_s;

void ResetPanelDebris(void);
void UpdatePanelDebris(void);
void UpdatePlayerStats(struct creature_s *plr);

#endif /* GAME_PANEL_H */
