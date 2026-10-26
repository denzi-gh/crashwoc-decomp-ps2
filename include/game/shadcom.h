#ifndef GAME_SHADCOM_H
#define GAME_SHADCOM_H

#include "nu_types.h"

struct numtl_s;

struct shadentry_s {
    s16 xframes;                  /* 0x00 */
    s16 yframes;                  /* 0x02 */
    s32 pad04;
    struct numtl_s *mtl;          /* 0x08 */
    s32 tid;                      /* 0x0C */
    u8 native[4];                 /* 0x10 */
};

union shadslot_u {
    s32 offset;
    struct shadentry_s *entry;
    char *name;
};

struct shaddata_s {
    s16 count;                    /* 0x00 */
    s16 external;                 /* 0x02 */
    union shadslot_u table[1];    /* 0x04: entries[count], then names[count] */
};

void ShadComInit(void);

#endif /* GAME_SHADCOM_H */
