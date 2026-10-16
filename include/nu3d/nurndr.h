#ifndef NU3D_NURNDR_H
#define NU3D_NURNDR_H

#include "numath/numathtypes.h"

struct faceon_s {
    struct nuvec_s pos;                 /* 0x00 */
    float w;                            /* 0x0C */
    float tc[4];                        /* 0x10 */
    float width;                        /* 0x20 */
    float height;                       /* 0x24 */
    float sinang;                       /* 0x28 */
    float cosang;                       /* 0x2C */
    int colour[4];                      /* 0x30 */
}; /* 0x40 */

#endif
