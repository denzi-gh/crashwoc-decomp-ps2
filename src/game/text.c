/*
 * Unit: game/text
 *
 * Functions:
 *   0x002409e8 NewLanguage
 *   0x00240b88 AddSpacesIntoText
 *   0x00240c48 JStrLen
 */

#include "creature.h"

struct game_s {
    u8 pad_00[0xF];
    u8 language;            /* 0xF */
};

struct powerdata_s {
    u8 pad_00[4];
    char *name;             /* 0x4 */
    char **text;            /* 0x8 */
};                          /* 0xC */

extern struct game_s Game;
extern struct powerdata_s PData[6];

extern char *tSNEAKSHOES[6];
extern char *tDOUBLEJUMP[6];
extern char *tDEATHTORNADOSPIN[6];
extern char *tFRUITBAZOOKA[6];
extern char *tSPEEDSHOES[6];
extern char *tSUPERBELLYFLOP[6];
extern char *tSNEAKSHOESTEXT[6][7];
extern char *tDOUBLEJUMPTEXT[6][7];
extern char *tDEATHTORNADOSPINTEXT[6][7];
extern char *tFRUITBAZOOKATEXT[6][7];
extern char *tSPEEDSHOESTEXT[6][7];
extern char *tSUPERBELLYFLOPTEXT[6][7];

extern void NuStrCpy(char *dst, char *src);
extern void DefaultTimeTrialNames(s32 all);
extern s32 CombinationCharacterBD(char c0, char c1);
extern s32 CombinationCharacterBC(char c0, char c1);


void NewLanguage(s32 lang) {
    Game.language = lang;
    PData[0].name = tSNEAKSHOES[Game.language];
    PData[0].text = tSNEAKSHOESTEXT[Game.language];
    PData[1].name = tDOUBLEJUMP[Game.language];
    PData[1].text = tDOUBLEJUMPTEXT[Game.language];
    PData[2].name = tDEATHTORNADOSPIN[Game.language];
    PData[2].text = tDEATHTORNADOSPINTEXT[Game.language];
    PData[3].name = tFRUITBAZOOKA[Game.language];
    PData[3].text = tFRUITBAZOOKATEXT[Game.language];
    PData[4].name = tSPEEDSHOES[Game.language];
    PData[4].text = tSPEEDSHOESTEXT[Game.language];
    PData[5].name = tSUPERBELLYFLOP[Game.language];
    PData[5].text = tSUPERBELLYFLOPTEXT[Game.language];
    DefaultTimeTrialNames(0);
}


void AddSpacesIntoText(char *text, u32 flags) {
    char buf[256];
    s32 i;
    s32 len;

    if (Game.language == 'c') {
        len = 0;
        for (i = 0; text[i] != 0; i++) {
            buf[len++] = text[i];
            if ((flags & 1) || ((flags & 2) && text[i] >= '0' && text[i] <= '9')
                || ((flags & 4) && text[i] == '-')) {
                buf[len++] = ' ';
            }
        }
        buf[len] = 0;
        NuStrCpy(text, buf);
    }
}


s32 JStrLen(char *text) {
    s32 len = 0;

    while (text[0] != 0 && text[1] != 0) {
        if (text[2] == 'B'
            && ((text[3] == 'D' && CombinationCharacterBD(text[0], text[1]))
                || (text[3] == 'C' && CombinationCharacterBC(text[0], text[1])))) {
            text += 4;
        } else {
            text += 2;
        }
        len++;
    }
    return len;
}
