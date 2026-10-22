#ifndef GAME_FONT3D_H
#define GAME_FONT3D_H

#include "nu_types.h"
#include "nu3d/nugscn.h"

/* Font3DAccentTab entry: obj{scene@0,special@4}, name@8; stride 0xC.
 * Offsets verified in InitFont3D. */
struct font3djchar_s {
    struct nuhspecial_s obj; /* 0x0 scene, 0x4 special */
    char *name;             /* 0x8 */
};                          /* 0xC */

void Reset3DFontObjects(void);

#endif /* GAME_FONT3D_H */
