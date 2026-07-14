/*
 * Unit: game/sfx
 *
 * Functions:
 *   0x0024f318 InitLevelSfxTables
 *   0x0024f680 InitGlobalSfx
 *   0x0024f778 InitLocalSfx
 *   0x0024fa08 UpdateGameSfx
 *   0x0024fa90 GameSfx
 *   0x0024fd30 GameSfxLoop
 *   0x0024fe88 GameAudioUpdate
 *   0x00250298 ResetGameSfx
 *   0x00250310 GameMusic
 *   0x00250458 PauseGameAudio
 *   0x002504f0 ResumeGameAudio
 *   0x00250518 TestLocalSfx
 */

#include "creature.h"

/* pSFX entry: wait@0x17, stride 0x30 (verified in UpdateGameSfx/ResetGameSfx). */
struct pSFX {
    u8 pad0[0x17];
    u8 wait;          /* 0x17 */
    u8 pad1[0x18];
};                    /* 0x30 */

extern struct pSFX SfxTabGLOBAL[];
extern struct pSFX *CurSfxTabLocal;
extern s32 SFXCOUNT_ALL;

extern void NuSoundSetChannelPitch(s32 chan, s32 pitch, s32 a2);
extern void NuSoundUpdate(void);
extern void SOUND_StopSound(s32 chan);


void UpdateGameSfx(void) {
    struct pSFX *p;
    s32 i;

    p = SfxTabGLOBAL;
    do {
        if (p->wait != 0) {
            p->wait--;
        }
        p++;
    } while ((s32)p < (s32)&SfxTabGLOBAL[0xC5]);

    if (CurSfxTabLocal == 0) {
        return;
    }
    if (SFXCOUNT_ALL <= 0xC5) {
        return;
    }
    for (i = 0; i < SFXCOUNT_ALL - 0xC5; i++) {
        if (CurSfxTabLocal[i].wait != 0) {
            CurSfxTabLocal[i].wait--;
        }
    }
}

void ResetGameSfx(void) {
    s32 i;

    for (i = 0xC4; i >= 0; i--) {
        SfxTabGLOBAL[i].wait = 0;
    }

    if (CurSfxTabLocal == 0) {
        return;
    }
    if (SFXCOUNT_ALL <= 0xC5) {
        return;
    }
    for (i = 0; i < SFXCOUNT_ALL - 0xC5; i++) {
        CurSfxTabLocal[i].wait = 0;
    }
}

void PauseGameAudio(s32 pause) {
    s32 i;

    if (pause != 0) {
        NuSoundSetChannelPitch(4, 0, 0);
    }
    for (i = 6; i < 0x18; i++) {
        if ((i & 7) == 0) {
            NuSoundUpdate();
        }
        SOUND_StopSound(i);
    }
    for (i = 0x18; i < 0x30; i++) {
        if ((i & 7) == 0) {
            NuSoundUpdate();
        }
        NuSoundSetChannelPitch(i, 0, 0);
    }
}

void ResumeGameAudio(void) {
    NuSoundSetChannelPitch(4, 0x75A, 0);
}
