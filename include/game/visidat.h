#ifndef GAME_VISIDAT_H
#define GAME_VISIDAT_H

#include "nu_types.h"

struct nugscn_s;
union variptr_u;

struct visidata_s {
    struct nugscn_s *sc; /* 0x00 */
    u64 ***vspline;      /* 0x04 */
    int binfosize;      /* 0x08 */
    int curspline;      /* 0x0C */
    int curknot;        /* 0x10 */
}; /* 0x14 */

void visiSetSplineKnot(struct visidata_s *vd, int spix, int knotix);
struct visidata_s *visiLoadData(char *fname, struct nugscn_s *sc,
                              union variptr_u *buff);
void visiReset(struct visidata_s *vd);

#endif /* GAME_VISIDAT_H */
