#ifndef GAME_SFX_H
#define GAME_SFX_H

#include "nu_types.h"
#include "numath/numathtypes.h"

/* GameSound entry (offsets verified in GameMusic/GameSfxLoop), stride 0x30. */
typedef struct {
    char name[16];
    u16 pitch;        /* 0x10 */
    u16 volume;       /* 0x12 */
    u8 buzz;          /* 0x14 */
    u8 rumble;        /* 0x15 */
    u8 delay;         /* 0x16 */
    u8 wait;          /* 0x17 */
    char *path;       /* 0x18 verified in InitGlobalSfx (lw 0x18) */
    s16 frequency;    /* 0x1C verified in InitGlobalSfx (lh 0x1C) */
    u16 stream;       /* 0x1E verified in InitGlobalSfx (lhu 0x1E) */
    s8 type;          /* 0x20 */
    u8 pad1;          /* 0x21 */
    u16 id;           /* 0x22 verified in InitGlobalSfx (lhu 0x22) */
    struct nuvec_s Pos;   /* 0x24 */
} GameSound;              /* 0x30 */

void GameMusic(s32 sfx, s32 i);
void InitLocalSfx(GameSound *sfxTab, s32 totalCount);
void ResetGameSfx(void);

#endif /* GAME_SFX_H */
