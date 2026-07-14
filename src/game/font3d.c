/*
 * Unit: game/font3d
 *
 * Functions:
 *   0x00237d58 RemapAccentedCharacter
 *   0x00237ec8 Reset3DFontObjects
 *   0x00237ff0 Update3DFontObjects
 *   0x002380c8 InitFont3D
 *   0x00238280 Text3D
 *   0x00238b48 CombinationCharacterBD
 *   0x00238ba0 CombinationCharacterBC
 */

#include "creature.h"

/* Font3DObjTab entry (objtemp_s): i@0, flags@2, action@3, anim_time@4. */
typedef struct {
    short i;          /* 0x0 */
    u8 flags;         /* 0x2 */
    s8 action;        /* 0x3 */
    float anim_time;  /* 0x4 */
    float scale;      /* 0x8 */
} objtemp_s;          /* 0xC */

extern objtemp_s Font3DObjTab[];
extern f32 D_0062E350;
extern char D_005CD7B0[];   /* j_bd */
extern char D_005CD808[];   /* j_bc */

extern s32 qrand(void);


void Reset3DFontObjects(void) {
    objtemp_s *tab;
    struct CharacterModel *model;
    s32 j;
    s32 i;
    f32 one;
    f32 rate;

    one = 1.0f;
    rate = D_0062E350;
    tab = Font3DObjTab;
    for (i = 0; i < 0x1a; i++, tab++) {
        tab->anim_time = one;
        if ((tab->flags & 1) != 0) {
            if (tab->i != -1) {
                if ((u8)tab->action < 0x76) {
                    j = CRemap[tab->i];
                    if (j != -1) {
                        model = &CModel[j];
                        if (model->anmdata[tab->action] != 0) {
                            tab->anim_time = (f32)qrand() * rate *
                                (model->anmdata[tab->action]->time - one) +
                                one;
                        }
                    }
                }
            }
        }
    }
}

s32 CombinationCharacterBD(char c0, char c1) {
    char *p;

    for (p = D_005CD7B0; *p != '\0'; p += 2) {
        if (c0 == *p && c1 == p[1]) {
            return 1;
        }
    }
    return 0;
}

s32 CombinationCharacterBC(char c0, char c1) {
    char *p;

    for (p = D_005CD808; *p != '\0'; p += 2) {
        if (c0 == *p && c1 == p[1]) {
            return 1;
        }
    }
    return 0;
}
