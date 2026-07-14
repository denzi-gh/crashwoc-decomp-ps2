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

typedef int s32;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned long long u64;

struct nuvec_s;

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
