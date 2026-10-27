#ifndef GAME_VISI_H
#define GAME_VISI_H

#include "nu_types.h"

struct nugscn_s;
struct nuvec_s;

void ApplyVisiTable(struct nugscn_s *sc, struct nuvec_s *pos);
void ReleaseVisiTable(void);

#endif /* GAME_VISI_H */
