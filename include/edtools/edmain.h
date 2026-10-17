#ifndef EDTOOLS_EDMAIN_H
#define EDTOOLS_EDMAIN_H

#include "nu_types.h"

struct nupad_s;

/* Four-character block tag, first character in the low byte. */
#define EDMAIN_BLOCKID(a, b, c, d) \
    ((a) | ((b) << 8) | ((c) << 16) | ((d) << 24))

/* An editor module's descriptor, linked into the edmain registry by
 * edmainRegister. blockid is the four-character tag of the module's block
 * in the editor data file. */
struct edmainreg_s {
    struct edmainreg_s *next; /* 0x00 */
    struct edmainreg_s *prev; /* 0x04 */
    char *name; /* 0x08 */
    void (*init)(void); /* 0x0C */
    void (*close)(void); /* 0x10 */
    void (*select)(void); /* 0x14 */
    char pad_0x18[0xC]; /* 0x18 */
    void (*readblock)(s32 fh); /* 0x24 */
    s32 blockid; /* 0x28 */
    s32 (*process)(struct nupad_s *pad, f32 dt); /* 0x2C */
    void (*render)(void); /* 0x30 */
}; /* 0x34 */

#endif
