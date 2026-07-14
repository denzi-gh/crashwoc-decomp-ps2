/*
 * Unit: game/deb3
 *
 * Functions:
 *   0x00247aa0 PlayRandSFX
 *   0x00247bc8 RBodyInitClasses
 *   0x00247e18 RBodyMove
 *   0x00248118 RBodyImpact
 *   0x00248310 InitDeb3
 *   0x002484d8 AddDeb3Ang
 *   0x00248ae0 AddDeb3
 *   0x00249108 JonExtraDraw
 *   0x002495a8 LaunchObjects
 *   0x0024bcc0 ProcDeb3
 *   0x0024c5b0 DrawDeb3
 *   0x0024c778 CoalSpark
 *   0x0024c7e0 CoalBreak
 *   0x0024c830 RockSpark
 *   0x0024c8a8 RockBreak
 *   0x0024c908 DynaBreak
 *   0x0024c950 InitRandSFX
 *   0x0024c960 JonMaskFPS
 */

#include "creature.h"

/* deb3_s: mtx@0x0 (translation row _30/_31/_32), timer@0xD2, data@0xDC. */
struct deb3_s {
    struct numtx_s mtx;   /* 0x00 */
    u8 unk_0x40[0x92];
    s16 timer;            /* 0xD2 */
    u8 unk_0xD4[0x8];
    s32 data;             /* 0xDC */
};

struct gdeb_s {
    s32 i;                /* 0x0 */
    char *name;           /* 0x4 */
    u64 levbits;          /* 0x8 */
};

extern struct gdeb_s GDeb[170];
extern s32 rsfxcount;
extern s32 *rsfxpt;

extern void AddGameDebris(s32 id, struct nuvec_s *pos);
extern void GameSfx(s32 id, struct nuvec_s *pos);
extern void AddFiniteShotDebrisEffect(s32 *key, s32 effect, struct nuvec_s *pos,
                                      s32 n);


void InitRandSFX(void) {
    rsfxcount = 0;
    rsfxpt = 0;
}

s32 JonMaskFPS(s32 val, s32 add) {
    return add * 0x3c + val <= 0x320000 ? val + add * 0x3c
                                        : (add * 0x3c + val) - 0x320000;
}

void DynaBreak(struct deb3_s *deb) {
    struct nuvec_s pos;

    pos.x = deb->mtx._30;
    pos.y = deb->mtx._31;
    pos.z = deb->mtx._32;
    AddGameDebris(0x44, &pos);
    GameSfx(0x3b, &pos);
}

void RockBreak(struct deb3_s *deb) {
    s32 key;
    struct nuvec_s vec;

    vec.x = deb->mtx._30;
    vec.y = deb->mtx._31;
    vec.z = deb->mtx._32;
    key = -1;
    AddFiniteShotDebrisEffect(&key, GDeb[33].i, &vec, 1);
    GameSfx(0x5f, &vec);
}

void CoalBreak(struct deb3_s *deb) {
    s32 key;
    struct nuvec_s vec;

    vec.x = deb->mtx._30;
    vec.y = deb->mtx._31;
    vec.z = deb->mtx._32;
    key = -1;
    AddFiniteShotDebrisEffect(&key, GDeb[36].i, &vec, 1);
}

void RockSpark(struct deb3_s *deb) {
    s32 key;
    struct nuvec_s vec;

    vec.x = deb->mtx._30;
    vec.y = deb->mtx._31;
    vec.z = deb->mtx._32;
    key = -1;
    deb->data--;
    if (deb->data < 0) {
        deb->timer = 1;
    } else {
        AddFiniteShotDebrisEffect(&key, GDeb[33].i, &vec, 1);
        GameSfx(0x5f, &vec);
    }
}

void CoalSpark(struct deb3_s *deb) {
    s32 key;
    struct nuvec_s vec;

    vec.x = deb->mtx._30;
    vec.y = deb->mtx._31;
    vec.z = deb->mtx._32;
    key = -1;
    deb->data--;
    if (deb->data < 0) {
        deb->timer = 1;
    } else {
        AddFiniteShotDebrisEffect(&key, GDeb[35].i, &vec, 1);
    }
}
