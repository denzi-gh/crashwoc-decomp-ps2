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

#include "numath/nuvec.h"
#include "game/gametypes.h"
#include "game/creature.h"
#include "nusound/nusound.h"

/* pSFX entry (offsets verified in GameMusic/GameSfxLoop), stride 0x30. */
struct pSFX {
    u8 pad0[0x10];
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
    u8 pad2;          /* 0x21 */
    u16 id;           /* 0x22 verified in InitGlobalSfx (lhu 0x22) */
    struct nuvec_s Pos;   /* 0x24 */
};                    /* 0x30 */

extern struct pSFX SfxTabGLOBAL[];
extern struct nusound_filename_info_s SfxInfo[];

extern void NuStrCpy(char *dst, const char *src);
extern void NuStrCat(char *dst, const char *src);
extern void NuSoundSetGlobalDat(void *dat);
extern void NuSoundInit(struct nusound_filename_info_s *info);

extern char D_00632938[];  /* "SFX\" prefix */
extern char D_00632940[];  /* filename suffix */
extern char D_00632948[];  /* terminator name */
extern u8 D_00632950[];    /* NuSoundSetGlobalDat argument */
extern struct pSFX *CurSfxTabLocal;
extern s32 SFXCOUNT_ALL;

extern struct game_s Game;
extern s32 game_music;
extern s32 gamesfx_volume;
extern s32 gamesfx_effect_volume;
extern s32 gamesfx_pitch;
extern s32 gamesfx_channel;
extern s32 gamesfx_edbits;
extern s32 PLAYERCOUNT;
extern struct cammtx_s GameCam[];

extern void NuSoundPlay3d(struct nuvec_s *pos, s32 sfx, s32 volL, s32 volR,
                          s32 pitch);
extern void NuSoundPlayChan(s32 track, s32 volL, s32 volR, s32 pitch, s32 channel);
extern void NuSoundPlay(s32 sfx, s32 volL, s32 volR, s32 pitch);
extern int NuSoundSetChannelPitch(s32 chan, s32 pitch, s32 stereo);
extern void NuSoundUpdate(void);
extern int SOUND_StopSound(s32 chan);
extern s32 NuSoundStopStream(s32 channel);
extern void NuSoundPlayStereo(s32 track, s32 sfx2, s32 volL, s32 volR, s32 pitch);
extern void NuSoundPlayStereo2(s32 track, s32 sfx2, s32 volL, s32 volR, s32 pitch);
extern void NuSoundPlay3dLoopSfx(struct nuvec_s *pos, s32 sfx, s32 volL, s32 volR,
                                 s32 pitch);
extern void InitLocalSfx(struct pSFX *sfxTab, s32 totalCount);
extern void NuSoundKillAllAudio(void);

extern struct ldata_s LData[];

/* Level SFX tables (retail-owned data). */
extern struct pSFX D_005E81F0[];
extern struct pSFX D_005E8340[];
extern struct pSFX D_005E85B0[];
extern struct pSFX D_005E8700[];
extern struct pSFX D_005E87C0[];
extern struct pSFX D_005E88B0[];
extern struct pSFX D_005E8A60[];
extern struct pSFX D_005E8B80[];
extern struct pSFX D_005E8CD0[];
extern struct pSFX D_005E8E20[];
extern struct pSFX D_005E8F70[];
extern struct pSFX D_005E9030[];
extern struct pSFX D_005E9120[];   /* nSFX 0xCE */
extern struct pSFX D_005E92D0[];
extern struct pSFX D_005E9450[];
extern struct pSFX D_005E9750[];
extern struct pSFX D_005E9930[];
extern struct pSFX D_005E99C0[];   /* nSFX 0xD0 */
extern struct pSFX D_005E9BD0[];
extern struct pSFX D_005E9FC0[];   /* nSFX 0xD2 */
extern struct pSFX D_005EA230[];
extern struct pSFX D_005EA320[];
extern struct pSFX D_005EA650[];
extern struct pSFX D_005EA9B0[];
extern struct pSFX D_005EABC0[];
extern struct pSFX D_005EAD40[];
extern struct pSFX D_005EAE90[];
extern struct pSFX D_005EB2B0[];
extern struct pSFX D_005EB340[];
extern struct pSFX D_005EB3D0[];
extern struct pSFX D_005EB4F0[];
extern struct pSFX D_005EB550[];
extern struct pSFX D_005EB670[];
extern struct pSFX D_005EB700[];
extern struct pSFX D_005EB790[];
extern struct pSFX SfxTabFRONTEND[];
extern struct pSFX D_005EC780[];
extern struct pSFX D_005ED1A0[];
extern struct pSFX D_005EF240[];

extern s32 soundtestcount;

extern struct cursor_s Cursor;
extern s32 Paused;
extern s32 GameMode;
extern s32 new_level;
extern s32 cutmovie;
extern f32 musicvol_mul;
extern u8 temp_music_volume;
extern u8 temp_surround;
extern s32 Bonus;
extern f32 bonus_duration;
extern f32 bonus_time;
extern s32 nusound_dpl_enabled;
extern f32 nusound_fade_end;
extern s32 Level;
extern u64 LBIT;
extern s32 VEHICLECONTROL;
extern s32 testsfx;
extern s32 testglobalsfx;
extern s32 last_testglobalsfx;

extern s32 ChaseActive(void);
extern int SOUND_SetChannelVolume(unsigned int channel, int left, int right);
extern void edbitsSetSoundFxVolume(int volume);


void InitLevelSfxTables(void) {
    s32 i;

    for (i = 0; i < 44; i++) {
        LData[i].pSFX = 0;
        LData[i].nSFX = 0;
    }

    LData[0].pSFX = D_005E81F0;   LData[0].nSFX = 0xCC;
    LData[1].pSFX = D_005E8340;   LData[1].nSFX = 0xD2;
    LData[2].pSFX = D_005E85B0;   LData[2].nSFX = 0xCC;
    LData[3].pSFX = D_005E8700;   LData[3].nSFX = 0xC9;
    LData[4].pSFX = D_005E87C0;   LData[4].nSFX = 0xCA;
    LData[5].pSFX = D_005E88B0;   LData[5].nSFX = 0xCE;
    LData[6].pSFX = D_005E8A60;   LData[6].nSFX = 0xCB;
    LData[7].pSFX = D_005E8B80;   LData[7].nSFX = 0xCC;
    LData[8].pSFX = D_005E8CD0;   LData[8].nSFX = 0xCC;
    LData[9].pSFX = D_005E8E20;   LData[9].nSFX = 0xCC;
    LData[10].pSFX = D_005E8F70;   LData[10].nSFX = 0xC9;
    LData[11].pSFX = D_005E9030;   LData[11].nSFX = 0xCA;
    LData[12].pSFX = D_005E9120;   LData[12].nSFX = 0xCE;
    LData[13].pSFX = D_005E92D0;   LData[13].nSFX = 0xCD;
    LData[14].pSFX = D_005E9450;   LData[14].nSFX = 0xD5;
    LData[15].pSFX = D_005E9750;   LData[15].nSFX = 0xCF;
    LData[16].pSFX = D_005E9930;   LData[16].nSFX = 0xC8;
    LData[17].pSFX = D_005E99C0;   LData[17].nSFX = 0xD0;
    LData[18].pSFX = D_005E9BD0;   LData[18].nSFX = 0xDA;
    LData[19].pSFX = D_005E9FC0;   LData[19].nSFX = 0xD2;
    LData[20].pSFX = D_005EA230;   LData[20].nSFX = 0xCD;
    LData[21].pSFX = D_005EA320;   LData[21].nSFX = 0xD6;
    LData[22].pSFX = D_005EA650;   LData[22].nSFX = 0xD7;
    LData[23].pSFX = D_005EA9B0;   LData[23].nSFX = 0xD0;
    LData[24].pSFX = D_005EABC0;   LData[24].nSFX = 0xCD;
    LData[25].pSFX = D_005EAD40;   LData[25].nSFX = 0xCC;
    LData[26].pSFX = D_005EAE90;   LData[26].nSFX = 0xDB;
    LData[27].pSFX = D_005EB2B0;   LData[27].nSFX = 0xC8;
    LData[28].pSFX = D_005EB340;   LData[28].nSFX = 0xC8;
    LData[29].pSFX = D_005EB3D0;   LData[29].nSFX = 0xCB;
    LData[30].pSFX = D_005EB4F0;   LData[30].nSFX = 0xDB;
    LData[31].pSFX = D_005EB550;   LData[31].nSFX = 0xCB;
    LData[32].pSFX = D_005EB670;   LData[32].nSFX = 0xC8;
    LData[33].pSFX = D_005EB700;   LData[33].nSFX = 0xC8;
    LData[34].pSFX = D_005EB790;   LData[34].nSFX = 0xCE;
    LData[36].pSFX = D_005E8700;   LData[36].nSFX = 0xC9;
    LData[37].pSFX = SfxTabFRONTEND;   LData[37].nSFX = 0x115;
    LData[38].pSFX = D_005EC780;   LData[38].nSFX = 0xCE;
    LData[40].pSFX = D_005ED1A0;   LData[40].nSFX = 0xE2;
    LData[43].pSFX = D_005EF240;   LData[43].nSFX = 0xE2;
}

void InitGlobalSfx(void) {
    struct pSFX *src;
    struct nusound_filename_info_s *base;
    struct pSFX *end;
    struct nusound_filename_info_s *dst;

    src = SfxTabGLOBAL;
    base = SfxInfo;
    end = &SfxTabGLOBAL[0xC5];
    dst = base;
    do {
        NuStrCpy(dst->Filename, D_00632938);
        NuStrCat(dst->Filename, src->path);
        NuStrCat(dst->Filename, D_00632940);
        dst->Pitch = src->frequency;
        dst->LoopInfo = src->stream;
        dst->ID = src->id;
        src++;
        dst++;
    } while (src < end);

    NuStrCpy(base[0xC5].Filename, D_00632948);
    base[0xC5].Pitch = 0;
    base[0xC5].LoopInfo = 1;
    base[0xC5].ID = -1;
    NuSoundSetGlobalDat(D_00632950);
    NuSoundInit(base);
    SFXCOUNT_ALL = 0xC5;
}

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

    camPos = *(struct nuvec_s *)&GameCam[0].m._30;
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
            NewBuzz(&player->rumble, info->buzz);
        }
        if (info->rumble != 0) {
            NewRumble(&player->rumble, info->rumble);
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
            NewBuzz(&player->rumble, info->buzz);
        }
        if (info->rumble != 0) {
            NewRumble(&player->rumble, info->rumble);
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

void GameAudioUpdate(void) {
    s32 testMode;
    s32 vol;
    f32 volScale;
    f32 fadeTarget;
    double dist;

    if (Cursor.menu == 5) {
        testMode = (Cursor.y == 2) ? 2 : 1;
    } else {
        testMode = 0;
    }

    if ((Paused == 0 && (player->obj.mask == 0 || player->obj.mask->active < 3) &&
         GameMode != 1 && new_level == -1 && Cursor.menu != 0x21) ||
        (Paused != 0 && testMode == 2) || cutmovie == 0) {
        volScale = 1.0f;
    } else {
        volScale = 0.0f;
    }

    if (musicvol_mul > volScale) {
        musicvol_mul -= 0.02f;
        if (musicvol_mul < volScale) {
            musicvol_mul = volScale;
        }
    } else if (musicvol_mul < volScale) {
        musicvol_mul += 0.02f;
        if (musicvol_mul > volScale) {
            musicvol_mul = volScale;
        }
    }

    if (game_music != -1) {
        vol = (testMode != 0) ? temp_music_volume : Game.music_volume;
        vol = (f32)(vol * 0x1BFF / 100) * musicvol_mul;
        if (Bonus == 1 || Bonus == 3) {
            dist = bonus_duration * 0.5f - bonus_time;
            if (dist < 0.0) {
                dist = 0.0 - dist;
            }
            volScale = dist;
            if (volScale < 1.0f) {
                volScale *= (f32)(25 - Paused) / 25.0f;
                vol = (f32)vol * volScale;
            }
        }
        SOUND_SetChannelVolume(0, vol, 0);
        SOUND_SetChannelVolume(1, 0, vol);
    }

    if (Cursor.menu == 5) {
        nusound_dpl_enabled = temp_surround;
    } else {
        nusound_dpl_enabled = Game.surround;
    }

    fadeTarget = 12.5f;
    if (ChaseActive() != -1 || Level == 0x17) {
        fadeTarget = 50.0f;
    } else if (LBIT & 0x3E00000) {
        fadeTarget = 25.0f;
    } else if (Level == 9 || Level == 3) {
        fadeTarget = 50.0f;
    } else if (VEHICLECONTROL == 2) {
        fadeTarget = 25.0f;
    } else if (VEHICLECONTROL == 1) {
        s16 vehicle = player->obj.vehicle;

        if (vehicle == 0x53 || vehicle == 0x20) {
            fadeTarget = 25.0f;
        } else if (vehicle == 0x36 || vehicle == 0x81 || vehicle == 0x8B || vehicle == 0x99) {
            fadeTarget = 100.0f;
        }
    }

    if (nusound_fade_end > fadeTarget) {
        nusound_fade_end -= 0.5f;
        if (nusound_fade_end < fadeTarget) {
            nusound_fade_end = fadeTarget;
        }
    } else if (nusound_fade_end < fadeTarget) {
        nusound_fade_end += 0.5f;
        if (nusound_fade_end > fadeTarget) {
            nusound_fade_end = fadeTarget;
        }
    }

    if (testsfx != -1) {
        GameSfx(testsfx + 0xC7, 0);
        testsfx = -1;
    }
    if (testglobalsfx != -1) {
        last_testglobalsfx = testglobalsfx;
        GameSfx(testglobalsfx, 0);
        testglobalsfx = -1;
    }

    edbitsSetSoundFxVolume(Game.sfx_volume);
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

void TestLocalSfx(void) {
    for (;;) {
        InitLocalSfx(D_005E99C0, 0xD0);
        NuSoundKillAllAudio();
        InitLocalSfx(D_005E9FC0, 0xD2);
        NuSoundKillAllAudio();
        InitLocalSfx(D_005E9120, 0xCE);
        NuSoundKillAllAudio();
        soundtestcount++;
    }
}
