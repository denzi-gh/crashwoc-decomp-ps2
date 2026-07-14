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

struct gdeb_s {
    s32 i;         /* +0x0  verified in AddGameDebris/InitGameDebris */
    char *name;    /* +0x4  verified in InitGameDebris (lw 0x4) */
    u64 levbits;   /* +0x8  verified in InitGameDebris (ld 0x8) */
};

extern struct gdeb_s GDeb[170];
extern s32 NODEBRIS;
extern s32 Level;

extern s32 LookupDebrisEffect(char *name);
extern void AddFiniteShotDebrisEffect(s32 *key, s32 effect, struct nuvec_s *pos,
                                      s32 n);
extern void AddVariableShotDebrisEffect(s32 effect, struct nuvec_s *pos, s32 n,
                                        short xrot, short yrot);
extern void AddVariableShotDebrisEffectMtx2(s32 effect, struct nuvec_s *pos,
                                            s32 n, struct numtx_s *m1,
                                            struct numtx_s *m2);
extern void NuMtxSetRotationX(struct numtx_s *m, s32 angle);
extern void NuMtxMulR(struct numtx_s *dst, struct numtx_s *a,
                      struct numtx_s *b);

extern struct numtx_s mTEMP;
extern struct numtx_s numtx_identity;


void InitGameDebris(void) {
    s32 i;

    for (i = 0; i < 170; i++) {
        GDeb[i].i = -1;
        if (((GDeb[i].levbits >> Level) & 1) != 0) {
            GDeb[i].i = LookupDebrisEffect(GDeb[i].name);
        }
    }
}

void AddGameDebris(s32 i, struct nuvec_s *pos) {
    s32 key;

    if (NODEBRIS == 0 && (u32)i < 0xaa && GDeb[i].i != -1) {
        key = -1;
        AddFiniteShotDebrisEffect(&key, GDeb[i].i, pos, 1);
    }
}

void AddGameDebrisRot(s32 i, struct nuvec_s *pos, s32 n, u16 xrot, u16 yrot) {
    if (NODEBRIS == 0 && (u32)i < 0xaa && GDeb[i].i != -1) {
        AddVariableShotDebrisEffect(GDeb[i].i, pos, n, xrot, yrot + 0x4000);
    }
}

void AddGameDebrisMtx(s32 i, struct nuvec_s *pos, s32 n, struct numtx_s *m) {
    if (NODEBRIS == 0 && (u32)i < 0xaa && GDeb[i].i != -1) {
        NuMtxSetRotationX(&mTEMP, 0x4000);
        NuMtxMulR(&mTEMP, &mTEMP, m);
        AddVariableShotDebrisEffectMtx2(GDeb[i].i, pos, n, &mTEMP,
                                        &numtx_identity);
    }
}

void AddWarpDebris(struct obj_s *obj, struct nuvec_s *pos) {
    struct nuvec_s v;
    s32 key;

    /*
     * Faithful near-match (state=asm): all loads/mults/stores and the
     * 120-byte extent are exact; residual is a single f3/f4 allocator swap
     * between obj->SCALE and obj->pos.z (both short-lived FP temps). 83.3%.
     */
    key = -1;
    v.x = obj->pos.x;
    v.z = obj->pos.z;
    v.y = obj->pos.y + (obj->bot + obj->top) * obj->SCALE * 0.5f;
    AddFiniteShotDebrisEffect(&key, GDeb[77].i, &v, 1);
}
