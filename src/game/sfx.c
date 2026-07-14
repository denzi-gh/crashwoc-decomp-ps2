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

/* pSFX entry (offsets verified in GameMusic/GameSfxLoop), stride 0x30. */
struct pSFX {
    u8 pad0[0x10];
    u16 pitch;        /* 0x10 */
    u16 volume;       /* 0x12 */
    u8 buzz;          /* 0x14 */
    u8 rumble;        /* 0x15 */
    u8 delay;         /* 0x16 */
    u8 wait;          /* 0x17 */
    u8 pad1[0x8];
    s8 type;          /* 0x20 */
    u8 pad2[3];       /* 0x21 */
    struct nuvec_s Pos;   /* 0x24 */
};                    /* 0x30 */

extern struct pSFX SfxTabGLOBAL[];
extern struct pSFX *CurSfxTabLocal;
extern s32 SFXCOUNT_ALL;

struct game_s {
    u8 unk_0x00[0xB];
    u8 sfx_volume;        /* 0x0B */
    u8 music_volume;      /* 0x0C */
    u8 unk_0x0D[3];
};                        /* 0x10 */

extern struct game_s Game;
extern s32 game_music;
extern s32 gamesfx_volume;
extern s32 gamesfx_effect_volume;
extern s32 gamesfx_pitch;
extern s32 gamesfx_channel;
extern s32 gamesfx_edbits;
extern s32 PLAYERCOUNT;
extern char GameCam[];

extern f32 NuVecDist(struct nuvec_s *a, struct nuvec_s *b, void *c);
extern void NuSoundPlay3d(struct nuvec_s *pos, s32 sfx, s32 volL, s32 volR,
                          s32 pitch);
extern void NuSoundPlayChan(s32 track, s32 volL, s32 volR, s32 pitch, s32 channel);
extern void NuSoundPlay(s32 sfx, s32 volL, s32 volR, s32 pitch);
extern void NuSoundSetChannelPitch(s32 chan, s32 pitch, s32 a2);
extern void NuSoundUpdate(void);
extern void SOUND_StopSound(s32 chan);
extern s32 NuSoundStopStream(s32 channel);
extern void NuSoundPlayStereo(s32 track, s32 sfx2, s32 volL, s32 volR, s32 pitch);
extern void NuSoundPlayStereo2(s32 track, s32 sfx2, s32 volL, s32 volR, s32 pitch);
extern void NuSoundPlay3dLoopSfx(struct nuvec_s *pos, s32 sfx, s32 volL, s32 volR,
                                 s32 pitch);


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

void GameSfx(s32 sfx, struct nuvec_s *pos) {
    struct pSFX *info;
    struct nuvec_s camPos;
    s32 vol;
    s32 pitch;
    s32 proximity;
    s32 type;

    camPos = *(struct nuvec_s *)(GameCam + 0x30);
    proximity = 0;
    if (sfx < 0) {
        goto cleanup;
    }
    if (sfx >= SFXCOUNT_ALL) {
        goto cleanup;
    }
    if (sfx < 0xC5) {
        info = &SfxTabGLOBAL[sfx];
    } else {
        info = &CurSfxTabLocal[sfx - 0xC5];
    }
    if (sfx == 0x81 || sfx == 0x53) {
        if (pos != 0) {
            f32 dist1 = NuVecDist(&camPos, &info->Pos, 0);
            f32 dist2 = NuVecDist(&camPos, pos, 0);
            if (dist2 + 2.0f < dist1) {
                proximity = 1;
            }
        }
    }
    if (info->wait != 0) {
        if (proximity == 0) {
            goto cleanup;
        }
    }
    type = info->type;
    if (type == 1) {
        vol = info->volume * Game.music_volume / 100;
    } else {
        if (gamesfx_volume == -1) {
            vol = Game.sfx_volume;
        } else {
            vol = gamesfx_volume;
        }
        if (gamesfx_effect_volume == -1) {
            vol = info->volume * vol / 100;
        } else {
            vol = gamesfx_effect_volume * vol / 100;
        }
    }
    if (gamesfx_pitch == -1) {
        pitch = info->pitch;
    } else {
        pitch = gamesfx_pitch;
    }
    if (pos != 0) {
        NuSoundPlay3d(pos, sfx, vol, vol, pitch);
    } else if (gamesfx_channel != -1) {
        NuSoundPlayChan(sfx, vol, vol, pitch, gamesfx_channel);
    } else {
        NuSoundPlay(sfx, vol, vol, pitch);
    }
    if (PLAYERCOUNT != 0 && gamesfx_edbits == 0) {
        if (info->buzz != 0) {
            NewBuzz((struct rumble_s *)((char *)player + 0xCA4), info->buzz);
        }
        if (info->rumble != 0) {
            NewRumble((struct rumble_s *)((char *)player + 0xCA4), info->rumble);
        }
    }
    info->wait = info->delay;
    if (pos != 0) {
        info->Pos = *pos;
    } else {
        info->Pos = camPos;
    }
cleanup:
    gamesfx_edbits = 0;
    gamesfx_effect_volume = -1;
    gamesfx_channel = -1;
    gamesfx_volume = -1;
    gamesfx_pitch = -1;
}

void GameSfxLoop(s32 sfx, struct nuvec_s *pos) {
    struct pSFX *info;
    s32 vol;
    s32 pitch;

    if (sfx < 0) {
        goto cleanup;
    }
    if (sfx >= SFXCOUNT_ALL) {
        goto cleanup;
    }
    if (sfx < 0xC5) {
        info = &SfxTabGLOBAL[sfx];
    } else {
        info = &CurSfxTabLocal[sfx - 0xC5];
    }
    if (info->wait != 0) {
        goto cleanup;
    }
    if (gamesfx_volume == -1) {
        vol = Game.sfx_volume;
    } else {
        vol = gamesfx_volume;
    }
    if (gamesfx_effect_volume == -1) {
        vol = info->volume * vol / 100;
    } else {
        vol = gamesfx_effect_volume * vol / 100;
    }
    if (gamesfx_pitch == -1) {
        pitch = info->pitch;
    } else {
        pitch = gamesfx_pitch;
    }
    if (pos != 0) {
        NuSoundPlay3dLoopSfx(pos, sfx, vol, vol, pitch);
    }
    if (PLAYERCOUNT != 0 && gamesfx_edbits == 0) {
        if (info->buzz != 0) {
            NewBuzz((struct rumble_s *)((char *)player + 0xCA4), info->buzz);
        }
        if (info->rumble != 0) {
            NewRumble((struct rumble_s *)((char *)player + 0xCA4), info->rumble);
        }
    }
    info->wait = info->delay;
cleanup:
    gamesfx_edbits = 0;
    gamesfx_effect_volume = -1;
    gamesfx_channel = -1;
    gamesfx_volume = -1;
    gamesfx_pitch = -1;
}

void GameMusic(s32 sfx, s32 i) {
    struct pSFX *info;
    s32 vol;

    if (sfx < 0) {
        return;
    }
    if (sfx >= SFXCOUNT_ALL - 1) {
        return;
    }
    if (sfx < 0xC5) {
        info = &SfxTabGLOBAL[sfx];
    } else {
        info = &CurSfxTabLocal[sfx - 0xC5];
    }
    if (info->type == 1) {
        vol = Game.music_volume * 0x1BFF / 100;
    } else {
        vol = info->volume * Game.sfx_volume / 100;
    }
    if (i == 0) {
        NuSoundStopStream(0);
        NuSoundStopStream(1);
        NuSoundPlayStereo(sfx, sfx + 1, vol, vol, info->pitch);
    } else {
        NuSoundStopStream(2);
        NuSoundStopStream(3);
        NuSoundPlayStereo2(sfx, sfx + 1, vol, vol, info->pitch);
    }
    NuSoundUpdate();
    game_music = sfx;
}
