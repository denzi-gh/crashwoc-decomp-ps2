/*
 * Unit: game/game
 *
 * Functions:
 *   0x001db150 JonProbe
 *   0x001db900 UpdateTempCharacter
 *   0x001dbae8 DrawTempCharacter
 *   0x001dbc80 DrawTempCharacter2
 *   0x001dbdc0 AddAward
 *   0x001dbf68 UpdateAwards
 *   0x001dc150 DrawAwards
 *   0x001dc458 HubStart
 *   0x001dc7c8 HubSelect
 *   0x001dd108 HubLevelSelect
 *   0x001dd7f0 HubMoveVR
 *   0x001ddb20 HubDrawItems
 *   0x001de090 ResetGame
 *   0x001de1d8 InitVehicleToggles
 *   0x001de458 ResetVehicleControl
 *   0x001de678 ToggleVehicle
 *   0x001df008 DrawNextVehicle
 *   0x001df310 AddKaboom
 *   0x001df588 UpdateKabooms
 *   0x001dfc70 DrawKabooms
 *   0x001dfee8 InitPositions
 *   0x001dfff8 CheckFinish
 *   0x001e0c68 Draw3DCrateCount
 *   0x001e0ed0 LoadWumpa
 *   0x001e0fc8 InitWumpa
 *   0x001e1080 ResetWumpa
 *   0x001e1300 UpdateWumpa
 *   0x001e17d8 HitWumpa
 *   0x001e1980 WipeWumpa
 *   0x001e1a98 UpdateScreenWumpas
 *   0x001e1d48 DrawWumpa
 *   0x001e21c0 AddTempWumpa
 *   0x001e23e8 MakeMaskMatrix
 *   0x001e25e0 UpdateMask
 *   0x001e3170 DrawMask
 *   0x001e3458 LoseMask
 *   0x001e3660 DrawMaskFeathers
 *   0x001e3778 MakeEditText
 *   0x001e38c8 DrawNameInputTable
 *   0x001e3cf8 GetMenuInfo
 *   0x001e3e70 NewMenu
 *   0x001e40e0 ProcMenu
 *   0x001e63f8 DrawMenuEntry
 *   0x001e66d8 DrawMenuEntry2
 *   0x001e68d8 DrawMenu
 *   0x001e9e48 ResetCheckpoint
 *   0x001ea0e0 MakeTimeI
 *   0x001ea228 GameTiming
 *   0x001ea858 NewLevelTime
 *   0x001eaa18 StartTimeTrial
 *   0x001eab98 DefaultTimeTrialNames
 *   0x001eacf0 NewGame
 *   0x001eadd8 CalculateGamePercentage
 *   0x001eb0b8 OpenGame
 *   0x001eb328 BonusTransporter
 *   0x001ebc88 BonusTiming
 *   0x001ebe70 ResetDeath
 *   0x001ebf80 DeathTransporter
 *   0x001ec658 GemPathTransporter
 *   0x001ecd10 DrawTransporters
 *   0x001ecf50 InitLevel
 *   0x001ed8e8 InitGameMode
 *   0x001edac0 InitSplineTable
 *   0x001edbc8 PointAlongSpline
 *   0x001ee138 InitObjectTable
 *   0x001ee278 RatioBetweenEdges
 *   0x001ee410 Draw3DCharacter
 *   0x001ee7b8 DrawParallax
 *   0x001ee8b8 RayIntersectSphere
 *   0x001eeb50 WumpaRayCast
 *   0x001eee78 GameRayCast
 *   0x001ef0d8 DrawTarget
 *   0x001ef430 DrawProbeFX
 *   0x001ef658 UpdateLevel
 *   0x001ef760 ResetMaskFeathers
 *   0x001ef788 AddMaskFeathers
 *   0x001ef920 UpdateMaskFeathers
 *   0x001ef970 ResetKabooms
 *   0x001ef9a0 HubFromLevel
 *   0x001efa10 ResetAwards
 *   0x001efa40 ResetTempCharacter
 *   0x001efa78 ResetTimeTrial
 *   0x001efac0 ResetItems
 *   0x001efae0 DefaultGame
 *   0x001efb18 ResetLevel
 *   0x001efb38 ResetBonus
 *   0x001efb40 ResetGemPath
 *   0x001efbe8 qrand
 *   0x001efc08 RotDiff
 *   0x001efc40 SeekRot
 *   0x001efc80 TurnRot
 *   0x001efcf0 ClockOff
 *   0x001efd70 FindClock
 *   0x001efdb8 SaveWumpa
 *   0x001efe80 FlyWumpa
 *   0x001eff20 AddFlyingWumpa
 *   0x001f0098 AddScreenWumpa
 *   0x001f0140 MakeTimeF
 *   0x001f0178 MakeLevelTimeString
 *   0x001f01e8 ResetTimer
 *   0x001f0200 UpdateTimer
 *   0x001f0278 AheadOfCheckpoint
 *   0x001f0310 NewMask
 *   0x001f03e8 NearestSplinePoint
 *   0x001f04a8 SplineDistance
 *   0x001f0558 PlayerObjectAnimCollision
 *   0x001f0658 DistanceToLine
 *   0x001f0730 RatioAlongLine
 *   0x001f0830 AnglesBetweenPoints
 *   0x001f08c8 LineCrossed
 *   0x001f0980 Draw3DObject
 *   0x001f0ba0 StartHGobjAnim
 *   0x001f0be0 StopHGobjAnim
 *   0x001f0c20 ResetHGobjAnim
 *   0x001f0c80 DrawLevel
 *   0x001f0cf0 InitProbe
 *   0x001f0d18 ResetTempCharacter2
 *   0x001f0d50 WumpaHitTerrain
 *   0x001f0de8 InputNewLetter
 *   0x001f0e80 CleanLetters
 *   0x001f0ed8 NextMenuEntry
 *   0x001f0f00 StartTransporter
 *   0x001f0f90 FinishTransporter
 *   0x001f1000 TransporterGo
 *   0x001f1038 SplinePointTilt
 *   0x001f1120 SplinePointAngle
 */

#define NULL 0

typedef signed char s8;
typedef unsigned char u8;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

struct nuvec_s {
    float x, y, z;
};

struct nuangvec_s {
    s32 x, y, z;
};

struct mask_s {
    char pad_16c[0x16C];
    short character; /* 0x16C */
    u16 active;      /* 0x16E == D_0058A00E */
    char pad_end[0x190 - 0x170];
};

struct hub_s {
    u8 flags;
    u8 crystals;
    char pad1;
    char pad2;
};

struct time_s {
    char name[4];
    u32 itime;
};

struct level_s {
    u16 flags;
    char pad1;
    char pad2;
    struct time_s time[3];
};

struct game_s {
    char name[9];
    u8 vibration;
    u8 surround;
    u8 sfx_volume;
    u8 music_volume;
    char screen_x;
    char screen_y;
    u8 language;
    struct hub_s hub[6];
    struct level_s level[35];
    u8 lives;
    u8 wumpas;
    u8 mask;
    u8 percent;
    u8 crystals;
    u8 relics;
    u8 crate_gems;
    u8 bonus_gems;
    u8 gems;
    u8 gembits;
    u8 powerbits;
    u8 empty;
    u32 cutbits; /* 0x408, struct ends at 0x40C */
};

struct ldata_s {
    char *path;
    u8 *clist;
    void *pChase;
    u32 time[3];
    short music[2];
    void *pSFX;
    short nSFX;
    char pad1;
    char hub;
    u16 flags;
    short character;
    short vehicle;
    u16 farclip;
    struct nuvec_s vSTART;
    struct nuvec_s vBONUS;
    float fognear;
    float fogfar;
    u8 fogr;
    u8 fogg;
    u8 fogb;
    u8 foga;
    u8 hazer;
    u8 hazeg;
    u8 hazeb;
    u8 hazea;
};

struct hdata_s {
    s8 level[6];
    s8 i_spl[2];
    u8 barrier;
    u8 i_gdeb;
    short sfx;
};

struct remember_s {
    u8 x;
    u8 y;
};

struct cursor_s {
    struct remember_s remember[48];
    u32 menu_frame;  /* 0x60 */
    u32 item_frame;  /* 0x64 */
    char x;          /* 0x68 */
    char y;          /* 0x69 */
    char x_min;      /* 0x6A */
    char y_min;      /* 0x6B */
    char x_max;      /* 0x6C */
    char y_max;      /* 0x6D */
    char menu;       /* 0x6E */
    char new_menu;   /* 0x6F */
    char new_level;  /* 0x70 */
    u8 wait;         /* 0x71 */
    u8 wait_frames;  /* 0x72 */
    char wait_hack;  /* 0x73 */
    u8 button_lock;  /* 0x74 */
};

extern struct game_s Game;
extern struct ldata_s LData[];
extern struct hdata_s HData[6];
extern s32 platinum_relics;
extern s32 gold_relics;
extern s32 sapphire_relics;
extern s32 temp_hub;
extern s32 temp_hublevel;
extern s32 Hub;
extern char D_0061DFE0[]; /* "MIDGET  " */
extern u16 D_0058A00E[];   /* Mask.active (absolute, far from $gp) */
extern struct mask_s Mask;
extern u8 Cursor[];
extern u8 GlobalTimer[];
extern struct nuangvec_s proberot;
extern s32 probeon;
extern s32 probey;
extern s32 probetime;
extern s32 probecol;
extern float D_00630C04; /* MENUDY */
extern float D_00630C58; /* dme_sy */
extern float D_00630C54; /* dme_sx */
#define MENUDY D_00630C04
#define dme_sy D_00630C58
#define dme_sx D_00630C54
extern float D_0062D408; /* menu entry size scale */
extern float D_0062D40C; /* language-2 x scale */
extern char *tSFXVOLUME[];
extern char *tMUSICVOLUME[];
extern void Text3D(char *, s32, s32, float, float, float, float, float, float);
extern float dme_symul;
extern float dme_yadj;
extern float D_0062D3E4;
extern float D_0062D3E8;
extern float D_0062D3EC;
extern float D_0062D3F0;
extern float D_0062D3F4;
extern float D_0062D3F8;
extern float D_0062D3FC;
extern float D_0062D400;
extern float D_0062D404;
extern char *tPRESSxTOCONTINUE[];
extern char *tADJUSTSCREEN[];
extern char *tRESTARTTRIAL[];
extern char *tNEWGAME[];
extern char *tLOADGAME[];
extern char *tLANGUAGE[];
extern char *tRESUME[];
extern char *tOPTIONS[];
extern char *tQUIT[];
extern char *tWARPROOM[];
extern char *tVIBRATION[];
extern char *tON[];
extern char *tOFF[];
extern char *tSOUNDOPTIONS[];
extern char *tDONE[];
extern char *tSURROUND[];
extern char *tHORIZONTAL[];
extern char *tVERTICAL[];
extern char *tYES[];
extern char *tNO[];
extern char *tCONTINUE[];
extern char *tGAMEOVER[];
extern char *tRESTARTRACE[];
extern char *LanguageName[];
extern float D_0062D410; /* pause slide scale */
extern float D_0062D488; /* restart-time-trial menu top y */
extern float D_0062D48C; /* restart-race menu top y */
extern float PANELMENUX;
extern float GAMENAMEY;
extern char tbuf[];
extern char D_00630C78[]; /* "%s: %s" language format */
extern s32 sprintf(char *, const char *, ...);
extern void AddSpacesIntoText(char *, s32);
extern void DrawCredits(void);
extern char NameInputTable[][7];
extern char edit_txt[];
extern s32 i_nameinput;
extern u8 MenuTimer[];
extern char D_0061D980[]; /* "________" */
extern u8 temp_vibration;
extern u8 temp_surround;
extern u8 temp_sfx_volume;
extern u8 temp_music_volume;
extern char temp_screen_x;
extern char temp_screen_y;
extern s32 memcard_loadattempted;

struct nupad_s {
    char pad_55c[0x55C];
    u32 paddata;   /* 0x55C */
    char pad_560[4];
    u32 oldpaddata; /* 0x564 */
};

struct nucam_s {
    char pad_4c[0x4C];
    float farclip; /* 0x4C */
};

struct pmask_s {
    char pad_163[0x163];
    u8 flags_163;  /* 0x163 */
    char pad_164[0x16E - 0x164];
    short active;  /* 0x16E */
};

struct pobj_s {
    char pad_c[0xC];
    struct pmask_s *mask; /* 0xC */
    char pad_10[0x6C - 0x10];
    struct nuvec_s pos;   /* 0x6C */
    char pad_78[0x16E - 0x78];
    u16 hdg;              /* 0x16E */
};

struct player_s {
    struct pobj_s obj;
};

struct plr_lives_s {
    short count; /* 0x0 */
    short draw;  /* 0x2 */
};

struct cmodel_s {
    char pad_4[0x4];
    void *anmdata_0; /* 0x4 (anmdata[tempanim_nextaction] with idx*4) */
};

extern s32 ForceRestart;
extern s32 fadeval;
extern s32 editor_active;
extern s32 Paused;
extern s32 cutmovie;
extern s32 new_mode;
extern s32 cut_on;
extern s32 new_level;
extern s32 stick_bits;
extern s32 stick_bits_db;
extern s32 Level;
extern s32 i_demolevel;
extern s32 DemoLevel[];
extern s32 Demo;
extern s32 InvincibilityCHEAT;
extern s32 fade_rate;
extern s32 pause_dir;
extern s32 TimeTrial;
extern s32 GameMode;
extern s32 LivesLost;
extern s32 qseed;
extern s32 pausebuzz;
extern s32 LANGUAGEOPTION;
extern u8 D_0058B117[];
extern float NuTrigTable[];
extern float nusound_fade_start;
struct gamecam_s {
    char pad_a4[0xA4];
    float x; /* 0xA4 */
    float y; /* 0xA8 */
    float z; /* 0xAC */
};
extern struct gamecam_s GameCam;
extern s32 gamesfx_volume;
extern s32 LostLife;
extern s32 bonus_restart;
extern u8 MaskFeathers[];
extern struct player_s *player;
extern s32 LIFTPLAYER;
extern s32 ShowPlayerCoordinate;
extern s32 ExtraMoves;
extern s32 Hub;
extern s32 logos_played;
extern struct ldata_s *LDATA;
extern struct nucam_s *pNuCam;
extern s32 saveload_cardtype;
extern s32 saveload_cardformatted;
extern s32 saveload_freespace;
extern char D_0061DA28[]; /* "CRASH   " */
extern s32 force_menu;
extern char D_0061DA38[]; /* "        " */
extern s32 next_cut_movie;
extern s32 input_alphabet;
extern float D_0059202C[];
extern float credit_time;
extern short D_0058B0C8[];
extern s32 cortex_continue_i;
extern s32 cortex_gameover_i;
extern short cortex_gameover_tab[][2];
extern s8 D_0056233A[];
extern s32 tempanim_nextaction;
extern struct cmodel_s CModel[];
extern u8 D_0058B504[];
extern s32 gamesfx_channel;
extern s32 tempanim_waitaudio;
extern s32 cortex_quit_i;
extern char D_0058B134[];
extern s32 newleveltime_slot;
extern char D_00630C48[];
extern u16 new_lev_flags;
extern char D_00630C50[];
extern struct nuvec_s *pos_START;
extern float tumble_time;
extern s32 tumble_action;
extern float tumble_duration;
extern s32 last_level;
extern s32 last_hub;
extern s32 saveload_cardchanged;
extern struct game_s SaveSlot[];
extern struct game_s *game;
extern s32 memcard_loadresult_delay;
extern struct plr_lives_s plr_lives;
extern short D_0063203A;
extern s32 memcard_formatme;
extern s32 memcard_formatting;
extern s32 memcard_formatmessage_delay;
extern s32 memcard_saveneeded;
extern s32 memcard_savestarted;
extern s32 memcard_deleteneeded;
extern s32 memcard_deletestarted;
extern s32 memcard_savemessage_delay;
extern s32 memcard_saveresult_delay;
extern s32 memcard_formatfailed;
extern s32 boss_dead;

/* --- InitLevel support --- */
typedef unsigned long long u64;

struct nuinstance_s {
    char pad_44[0x44];
    struct {
        u32 visible : 1;
        u32 rest : 31;
    } flags; /* 0x44 */
};

struct objspecial_s {
    char pad_40[0x40];
    struct nuinstance_s *instance; /* 0x40 */
};

struct objinfo_s {
    char pad_4[4];
    struct objspecial_s *special; /* 0x4 */
};

struct objtab_s {
    struct objinfo_s obj; /* 0x0 */
    char pad_8[0x18];     /* entry stride 0x20 */
};

extern struct objtab_s ObjTab[];
extern u8 TempAnim[];
extern u8 TempAnim2[];
extern u8 TempLights[];
extern u8 TempLights2[];
extern s32 temp_character;
extern s32 temp_character_action;
extern s32 temp_character2;
extern s32 temp_character2_action;
extern s32 loadsave_frame;
extern s32 warp_level;
extern s32 hub_vr_level;
extern u64 LBIT;
extern s32 VEHICLECONTROL;
extern s32 level_part_2;
extern s32 SKELETALCRASH;
extern u8 new_hub_flags;
extern s32 COMPLEXPLAYERSHADOW;
extern s32 bonusgem_ok;
extern s32 GemPath;
extern s32 gempath_begun;
extern s32 gempath_open;
extern short plr_items;
extern struct plr_lives_s plr_crystal;
extern struct plr_lives_s plr_crategem;
extern struct plr_lives_s plr_bonusgem;
extern s32 hubleveltext_level;
extern float hubleveltext_pos;
extern s32 HubLevelText;
extern float plr_invisibility_time;
extern s32 in_finish_range;
extern float tumble_moveduration;
extern struct nuvec_s v000;
extern struct nuvec_s ai_lookpos;
extern struct nuvec_s lev_ambpos[];
extern s32 jcrunch;
extern s32 gamesfx_effect_volume;

/* creature_s subset for FindClock (full layout in creature.h) */
struct creatobj_s {
    signed char used;    /* 0x00 */
    signed char on;      /* 0x01 */
    char pad_2[0x32];
    short character;     /* 0x34 (obj.character) */
    char pad_36[0xCAE];  /* stride 0xCE4 */
};
extern struct creatobj_s Character[9]; /* 0x0057EE38 */
void UpdateDRAINDAMAGE(void);
s32 UpdateCRUNCHTIME(void);
void DrawDRAINDAMAGE(void);
void DrawCRUNCHTIME(void);
void HubDrawItems(void);
void GameSfxLoop(s32 sfx, struct nuvec_s *pos);

void InitPositions(void);
void ResetAnimPacket(void *pkt, s32 action);
void ResetLights(void *lights);
void NuLightFogClear(s32 mode);
void InitCredits(void);
void InitBugAreas(void);
s32 *NuBridgeCreate(struct nuinstance_s **instance, struct nuinstance_s *ipost,
                    struct nuvec_s *start, struct nuvec_s *end, float width,
                    short yang, float tension, float damp, float gravity,
                    float plrweight, s32 sections, float postw, float posth,
                    s32 postint, s32 colour);

void ResetCheckpoint(s32 a, s32 b, float c, void *d);
void GameSfx(s32 sfx, void *p);
void NuPs2VideoSetPos(s32 x, s32 y);
void ResetItems(void);
void RestoreCrateTypeData(void);
void ResetCrates(void);
void ResetWumpa(void);
void ResetChases(void);
void ResetPlayerEvents(void);
void ResetGates(void);
void ResetRings(void);
void ResetAI(void);
void ResetPlayer(s32 a);
void ResetBug(void);
void ResetLevel(void);
void ResetProjectiles(void);
void ResumeGame(void);
void edobjResetAnimsToZero(void);
s32 GotoCheckpoint(struct nuvec_s *pos, s32 dir);
void loadsaveCallEachFrame(void);
s32 strcmp(const char *a, const char *b);
void UpdateSaveSlots(struct cursor_s *cur);
void NuSoundStopStream(s32 ch);
void ResetMaskFeathers(void);

void *memset(void *s, s32 c, u32 n);
s32 NuStrCpy(char *dst, const char *src);
void NewLanguage(s32 lang);
void DefaultTimeTrialNames(s32 which);
void ResetTimer(void *timer);
void ResetBonus(void);
void ResetDeath(void);
void ResetGemPath(void);
void CalculateGamePercentage(struct game_s *game);
void GetMenuInfo(struct cursor_s *cur);
void CleanLetters(char *txt);

/* This is the RNG function*/
static inline s32 qrand(void) {
    qseed = qseed * 0x24CD + 1;
    qseed = qseed & 0xFFFF;
    return qseed;
}

static inline s32 HubFromLevel(s32 level) {
    s32 j;
    s32 i;

    if (level == -1) {
        return -1;
    }
    for (i = 0; i < 6; i++) {
        for (j = 0; j < 6; j++) {
            if (HData[i].level[j] == level) {
                temp_hublevel = j;
                temp_hub = i;
                return i;
            }
        }
    }
    return -1;
}

static void inline InitProbe(void) {
    probeon = 0;
    probey = 0;
    probetime = 0;
    proberot.x = 0;
    proberot.y = 0;
    proberot.z = 0;
    probecol = 0;
}

static inline void ResetTempCharacter(s32 character, s32 action) {
    temp_character = character;
    temp_character_action = action;
    ResetAnimPacket(TempAnim, action);
    ResetLights(TempLights);
}

static inline void ResetTempCharacter2(s32 character, s32 action) {
    temp_character2 = character;
    temp_character2_action = action;
    ResetAnimPacket(TempAnim2, action);
    ResetLights(TempLights2);
}

void ResetGame(void) {
    s32 i;

    memset(Cursor, 0, 0x78);
    for (i = 0; i < 0x2c; i++) {
        if (0x3de < LData[i].farclip - 10U) {
            LData[i].farclip = 1000;
        }
    }
    for (i = 0; i < 0x23; i++) {
        LData[i].hub = HubFromLevel(i);
    }
    for (i = 0; i < 1; i++) {
        (&Mask)[i].character = 3;
    }
    ResetTimer(GlobalTimer);
    InitProbe();
}

void NewGame(void) {
    s32 save[7];

    save[0] = Game.vibration;
    save[1] = Game.surround;
    save[2] = Game.sfx_volume;
    save[3] = Game.music_volume;
    save[4] = Game.screen_x;
    save[5] = Game.screen_y;
    save[6] = Game.language;
    memset(&Game, 0, 0x40C);
    Game.vibration = save[0];
    Game.surround = save[1];
    Game.sfx_volume = save[2];
    Game.music_volume = save[3];
    Game.screen_x = save[4];
    Game.screen_y = save[5];
    Game.language = save[6];
    NewLanguage(save[6]);
    NuStrCpy(Game.name, D_0061DFE0);
    DefaultTimeTrialNames(1);
    Game.lives = 4;
    Game.hub[0].flags = 1;
    D_0058A00E[0] = (u16)Game.mask;
    Hub = -1;
}

void OpenGame(void) {
    s32 i;
    s32 j;
    s32 k;

    for (k = 0; k < 0x23; k++) {
        Game.level[k].flags = 0;
    }
    for (k = 0; k < 6; k++) {
        Game.hub[k].flags = 0xff;
        Game.hub[k].crystals = 0;
        if (k < 5) {
            for (i = 0; i < 6; i++) {
                j = HData[k].level[i];
                if (j != -1) {
                    if (i < 5) {
                        Game.level[j].flags = 0x1f;
                        Game.hub[k].crystals = Game.hub[k].crystals + 1;
                    } else {
                        Game.level[j].flags = 0x800;
                    }
                }
            }
        } else {
            for (i = 0; i < 5; i++) {
                j = HData[k].level[i];
                if (j != -1) {
                    Game.level[j].flags = 0x17;
                }
            }
        }
    }
    Game.level[1].flags = Game.level[1].flags | 0x40;
    Game.level[17].flags = Game.level[17].flags | 0x80;
    Game.level[7].flags = Game.level[7].flags | 0x100;
    Game.level[19].flags = Game.level[19].flags | 0x200;
    Game.level[10].flags = Game.level[10].flags | 0x400;
    Game.level[4].flags = Game.level[4].flags | 0x20;
    Game.level[12].flags = Game.level[12].flags | 0x20;
    Game.level[14].flags = Game.level[14].flags | 0x20;
    Game.level[5].flags = Game.level[5].flags | 0x20;
    Game.level[20].flags = Game.level[20].flags | 0x20;
    Game.level[33].flags = Game.level[33].flags | 0x20;
    Game.level[27].flags = Game.level[27].flags | 0x20;
    Game.level[28].flags = Game.level[28].flags | 0x20;
    Game.level[29].flags = Game.level[29].flags | 0x20;
    Game.level[32].flags = Game.level[32].flags | 0x20;
    Game.level[9].flags = Game.level[9].flags | 0x20;
    Game.powerbits = 0xff;
    Game.cutbits = (u32)-1;
    CalculateGamePercentage(&Game);
    ResetBonus();
    ResetDeath();
    ResetGemPath();
}

void ResetItems(void) {
    plr_items = 0;
    plr_crystal.draw = 0;
    plr_crystal.count = 0;
    plr_crategem.draw = 0;
    plr_crategem.count = 0;
    plr_bonusgem.draw = 0;
    plr_bonusgem.count = 0;
}

void DefaultGame(void) {
    Game.vibration = 1;
    Game.surround = 0;
    Game.sfx_volume = 100;
    Game.music_volume = 75;
    Game.screen_x = 0;
    Game.screen_y = 0;
    Game.language = 0;
}

void CalculateGamePercentage(struct game_s *game) {
    s32 hub;
    s32 i;

    game->percent = 0;
    game->crystals = 0;
    game->relics = 0;
    game->crate_gems = 0;
    game->bonus_gems = 0;
    game->gems = 0;
    game->gembits = 0;
    sapphire_relics = gold_relics = platinum_relics = 0;
    for (i = 0; i < 6; i++) {
        game->hub[i].crystals = 0;
    }
    for (i = 0; i < 0x23; i++) {
        hub = HubFromLevel(i);
        if ((LData[i].flags & 2) != 0) {
            if ((game->level[i].flags & 0x800) != 0) {
                game->percent++;
            }
        } else {
            if ((game->level[i].flags & 8) != 0) {
                game->percent++;
                game->crystals = game->crystals + 1;
                game->hub[hub].crystals++;
            }
            if ((game->level[i].flags & 7) != 0) {
                game->percent++;
                game->relics = game->relics + 1;
                if ((game->level[i].flags & 4) != 0) {
                    platinum_relics++;
                } else {
                    if ((game->level[i].flags & 2) != 0) {
                        gold_relics++;
                    } else {
                        sapphire_relics++;
                    }
                }
            }
            if ((game->level[i].flags & 0x10) != 0) {
                game->percent++;
                game->crate_gems++;
                game->gems++;
            }
            if ((game->level[i].flags & 0x20) != 0) {
                game->percent++;
                game->bonus_gems++;
                game->gems++;
            } else if ((game->level[i].flags & 0x40) != 0) {
                game->percent++;
                game->gembits |= 1;
                game->gems++;
            } else if ((game->level[i].flags & 0x80) != 0) {
                game->percent++;
                game->gembits |= 2;
                game->gems++;
            } else if ((game->level[i].flags & 0x100) != 0) {
                game->percent++;
                game->gembits |= 4;
                game->gems++;
            } else if ((game->level[i].flags & 0x200) != 0) {
                game->percent++;
                game->gembits |= 8;
                game->gems++;
            } else if ((game->level[i].flags & 0x400) != 0) {
                game->percent++;
                game->gembits |= 0x10;
                game->gems++;
            }
        }
    }
}

char *MakeEditText(char *txt) {
    s32 j;
    s32 k;

    k = 0;
    edit_txt[k++] = '#';
    edit_txt[k++] = 'g';
    j = 0;
    while (txt[j] != '\0') {
        if ((Game.language == 'c' && j == i_nameinput + i_nameinput) ||
            (Game.language != 'c' && j == i_nameinput)) {
            edit_txt[k++] = '#';
            edit_txt[k++] = (*(u32 *)GlobalTimer % 10 < 5) ? 'o' : 'b';
        } else if ((Game.language == 'c' && j == (i_nameinput + 1) * 2) ||
                   (Game.language != 'c' && j == i_nameinput + 1)) {
            edit_txt[k++] = '#';
            edit_txt[k++] = 'g';
        }
        edit_txt[k++] = txt[j++];
    }
    edit_txt[k] = '\0';
    return edit_txt;
}

void NewMenu(struct cursor_s *cur, s32 menu, s32 y, s32 level) {
    s32 lock;

    cur->wait = 0;
    lock = 0;
    if (menu == cur->menu) {
        menu = -1;
    }
    if (menu != cur->new_menu || cur->wait_hack == 0) {
        switch (menu) {
        case -1:
            if (cur->menu == 0x13 || cur->menu == 0x16) {
                cur->wait = 0x32;
            }
            break;
        case 2:
            NuStrCpy(Game.name, D_0061D980);
        case 15:
            i_nameinput = 0;
            break;
        case 4:
            if (cur->menu == 3) {
                temp_vibration = Game.vibration;
                temp_surround = Game.surround;
                temp_sfx_volume = Game.sfx_volume;
                temp_music_volume = Game.music_volume;
                temp_screen_x = Game.screen_x;
                temp_screen_y = Game.screen_y;
            }
            break;
        case 5:
            temp_surround = Game.surround;
            temp_sfx_volume = Game.sfx_volume;
            temp_music_volume = Game.music_volume;
            break;
        case 6:
            temp_screen_x = Game.screen_x;
            temp_screen_y = Game.screen_y;
            break;
        case 19:
            memcard_loadattempted = 0;
            break;
        case 31:
            lock = 0x96;
            break;
        }
    }
    if (cur->wait != 0) {
        cur->new_menu = menu;
        cur->wait_frames = cur->wait;
    } else {
        ResetTimer(MenuTimer);
        cur->new_level = level;
        cur->new_menu = -1;
        cur->button_lock = lock;
        cur->menu = menu;
        cur->wait = 0;
        if (cur->menu != -1) {
            GetMenuInfo(cur);
            cur->x = cur->remember[menu].x;
            if (cur->x < cur->x_min) {
                cur->x = cur->x_min;
            } else if (cur->x > cur->x_max) {
                cur->x = cur->x_max;
            }
            if (y < cur->y_min || y > cur->y_max) {
                cur->y = cur->remember[menu].y;
                if (cur->y < cur->y_min) {
                    cur->y = cur->y_min;
                } else if (cur->y > cur->y_max) {
                    cur->y = cur->y_max;
                }
            } else {
                cur->y = y;
            }
            cur->menu_frame = 0;
            cur->item_frame = 0;
            cur->wait_hack = 0;
        }
    }
}

void ProcMenu(struct cursor_s *cursor, struct nupad_s *pad) {
    s32 old;
    s32 bits;
    s32 bits_db;
    s32 x_count;
    s32 y_count;
    s32 sfx;
    s32 UP;
    s32 DOWN;
    s32 LEFT;
    s32 RIGHT;
    s32 CROSS;
    s32 TRIANGLE;
    s32 FASTLEFT;
    s32 FASTRIGHT;
    s32 uVar16;
    s32 y_cursor_temp;
    s32 x_cursor_temp;
    char *txt;
    s32 nosaves;
    s32 test_sfx[4] = { 0x25, 0x19, 0x13, 0x2c };
    float pos[3];

    if (cursor->wait != 0) {
        cursor->wait--;
        if (cursor->wait == 0) {
            cursor->wait_hack = 1;
            NewMenu(cursor, cursor->new_menu, -1, -1);
        }
    }
    if (ForceRestart == 0) {
        if (cursor->menu == -1) {
            return;
        }
        if (cursor->wait != 0) {
            return;
        }
        if (fadeval > 0) {
            return;
        }
        if (editor_active != 0) {
            return;
        }
        if (Paused - 1U < 0x18) {
            return;
        }
        if (pad == NULL) {
            return;
        }
        if (cutmovie != -1) {
            if (cut_on == 0) {
                return;
            }
        } else if (new_mode != -1) {
            return;
        } else if (new_level != -1) {
            return;
        }
    }
    sfx = -1;
    if (cursor->button_lock != 0) {
        cursor->button_lock--;
    }
    if (pad != NULL && cursor->button_lock == 0) {
        bits = pad->paddata;
        if ((bits & 0xf000) == 0) {
            bits = stick_bits | bits;
        }
        bits_db = pad->oldpaddata;
        if ((bits_db & 0xf000) == 0) {
            bits_db = bits_db | stick_bits_db;
        }
    } else {
        bits_db = 0;
        bits = 0;
    }
    CROSS = bits_db & 0x40;
    TRIANGLE = bits_db & 0x10;
    DOWN = bits_db & 0x1000;
    UP = bits_db & 0x4000;
    RIGHT = bits_db & 0x8000;
    LEFT = bits_db & 0x2000;
    FASTLEFT = bits & 0x8000;
    FASTRIGHT = bits & 0x2000;
    uVar16 = CROSS;
    if (cursor->item_frame == 0) {
        uVar16 = 0;
        TRIANGLE = 0;
        CROSS = 0;
    }
    if (ForceRestart != 0) {
        ForceRestart = 0;
        CROSS = 1;
        cursor->menu = 9;
    }
    old = cursor->y;
    y_count = cursor->y_max - cursor->y_min + 1;
    if (cursor->y < cursor->y_min) {
        cursor->y = cursor->y_min;
    } else if (cursor->y > cursor->y_max) {
        cursor->y = cursor->y_max;
    }
    if (UP != 0) {
        cursor->y++;
        if (cursor->y > cursor->y_max) {
            cursor->y -= y_count;
        }
    } else if (DOWN != 0) {
        cursor->y--;
        if (cursor->y < cursor->y_min) {
            cursor->y += y_count;
        }
    }
    cursor->remember[cursor->menu].y = cursor->y;
    if (cursor->y != old) {
        sfx = 0x18;
        cursor->item_frame = 0;
    }
    old = cursor->x;
    x_count = cursor->x_max - cursor->x_min + 1;
    if (cursor->x < cursor->x_min) {
        cursor->x = cursor->x_min;
    } else if (cursor->x > cursor->x_max) {
        cursor->x = cursor->x_max;
    }
    if (LEFT != 0) {
        cursor->x++;
        if (cursor->x > cursor->x_max) {
            cursor->x -= x_count;
        }
    } else if (RIGHT != 0) {
        cursor->x--;
        if (cursor->x < cursor->x_min) {
            cursor->x += x_count;
        }
    }
    cursor->remember[cursor->menu].x = cursor->x;
    if (cursor->x != old) {
        sfx = 0x18;
        cursor->item_frame = 0;
    }
    if (CROSS != 0 || TRIANGLE != 0) {
        cursor->item_frame = 0;
    }
    cursor->menu_frame++;
    cursor->item_frame++;
    if ((Level == 0x23 || cutmovie == 0) && cursor->menu == 0 && 0x5db < cursor->item_frame) {
        new_level = DemoLevel[i_demolevel];
        Demo = 1;
        i_demolevel++;
        if (i_demolevel == 4) {
            i_demolevel = 0;
        }
        InvincibilityCHEAT = 0;
        if (cutmovie != 0) {
            return;
        }
        Level = new_level;
        fade_rate = 0xA;
        return;
    }

    y_cursor_temp = cursor->y;
    x_cursor_temp = cursor->x;
    switch (cursor->menu) {
    case 3: /* PAUSE MENU */
        if (CROSS != 0) {
            sfx = 0x36;
            switch (y_cursor_temp) {
            case 0:
                pause_dir = 2;
                break;
            case 1:
                NewMenu(cursor, 4, -1, -1);
                break;
            case 2:
                if (Level != 0x25) {
                    new_level = 0x25;
                } else {
                    NewMenu(cursor, 8, 1, -1);
                }
                break;
            case 3:
                if (TimeTrial != 0) {
                    new_mode = GameMode;
                    ResetCheckpoint(-1, -1, 0.0f, NULL);
                    LivesLost = 0;
                    ResetBonus();
                    ResetDeath();
                    ResetGemPath();
                }
                break;
            }
        }
        break;
    case 4: /* OPTIONS MENU */
        if (CROSS != 0) {
            if (y_cursor_temp == 0) {
                temp_vibration = 1 - temp_vibration;
                if (temp_vibration != 0) {
                    pausebuzz = qrand() * 0xA / 0x10000 + 5;
                } else {
                    pausebuzz = 0;
                }
            } else if (y_cursor_temp == 1) {
                NewMenu(cursor, 5, -1, -1);
            } else if (y_cursor_temp == 2) {
                NewMenu(cursor, 6, -1, -1);
            } else if (LANGUAGEOPTION != 0 && y_cursor_temp == 3) {
                NewMenu(cursor, 7, D_0058B117[0], -1);
            } else if (y_cursor_temp == cursor->y_max) {
                Game.vibration = temp_vibration;
                Game.surround = temp_surround;
                Game.sfx_volume = temp_sfx_volume;
                Game.music_volume = temp_music_volume;
                Game.screen_x = temp_screen_x;
                Game.screen_y = temp_screen_y;
                NewMenu(cursor, 3, -1, -1);
            }
            sfx = 0x36;
        } else if (TRIANGLE != 0) {
            NewMenu(cursor, 3, -1, -1);
            sfx = 0x3C;
            pausebuzz = 0;
        }
        if (y_cursor_temp != 0) {
            pausebuzz = 0;
        } else if (pausebuzz != 0) {
            pausebuzz--;
        } else if (temp_vibration != 0) {
            if (qrand() < 0x800) {
                pausebuzz = qrand() * 0xA / 0x10000 + 5;
            }
        }
        break;
    case 5: /* SOUND MENU */
        if (CROSS != 0) {
            if (y_cursor_temp == 0) {
                temp_surround = 1 - temp_surround;
                sfx = 0x36;
            } else if (y_cursor_temp == 3) {
                Game.surround = temp_surround;
                Game.sfx_volume = temp_sfx_volume;
                Game.music_volume = temp_music_volume;
                NewMenu(cursor, 4, -1, -1);
            }
        } else if (TRIANGLE != 0) {
            NewMenu(cursor, 4, -1, -1);
            sfx = 0x3C;
        } else if (y_cursor_temp == 0) {
            if (qrand() < 0x1000) {
                if (temp_surround != 0) {
                    s32 angle = qrand();
                    pos[0] = GameCam.x + NuTrigTable[angle] * nusound_fade_start;
                    pos[1] = GameCam.y;
                    pos[2] = GameCam.z + NuTrigTable[(angle + 0x4000) & 0xFFFF] * nusound_fade_start;
                    gamesfx_volume = temp_sfx_volume;
                    GameSfx(test_sfx[qrand() / 0x4000], pos);
                }
            }
        } else if (y_cursor_temp == 1) {
            if (qrand() < 0x1000) {
                gamesfx_volume = temp_sfx_volume;
                GameSfx(test_sfx[qrand() / 0x4000], NULL);
            }
            if (FASTLEFT != 0 && temp_sfx_volume != 0) {
                temp_sfx_volume--;
            } else if (FASTRIGHT != 0 && temp_sfx_volume < 100) {
                temp_sfx_volume++;
            }
        } else if (y_cursor_temp == 2) {
            if (FASTLEFT != 0 && temp_music_volume != 0) {
                temp_music_volume--;
            } else if (FASTRIGHT != 0 && temp_music_volume < 100) {
                temp_music_volume++;
            }
        }
        break;
    case 6: /* SCREEN MENU */
        if (CROSS != 0) {
            if (y_cursor_temp == 2) {
                Game.screen_y = temp_screen_y;
                Game.screen_x = temp_screen_x;
                NewMenu(cursor, 4, -1, -1);
            }
        } else if (TRIANGLE != 0) {
            NewMenu(cursor, 4, -1, -1);
            sfx = 0x3C;
            NuPs2VideoSetPos(Game.screen_x << 2, Game.screen_y << 1);
        } else {
            if (y_cursor_temp == 0) {
                if (FASTLEFT != 0 && temp_screen_x >= -0x3F) {
                    temp_screen_x--;
                } else if (FASTRIGHT != 0 && temp_screen_x < 0x40) {
                    temp_screen_x++;
                }
            } else if (y_cursor_temp == 1) {
                if (FASTLEFT != 0 && temp_screen_y >= -0x1F) {
                    temp_screen_y--;
                } else if (FASTRIGHT != 0 && temp_screen_y < 0x20) {
                    temp_screen_y++;
                }
            }
            if (temp_screen_x != old || temp_screen_y != (char)cursor->y) {
                NuPs2VideoSetPos(temp_screen_x << 2, temp_screen_y << 1);
            }
        }
        break;
    case 7: /* LANGUAGE MENU */
        if (CROSS != 0) {
            NewLanguage(y_cursor_temp);
            NewMenu(cursor, 4, -1, -1);
            sfx = 0x36;
        } else if (TRIANGLE != 0) {
            NewMenu(cursor, 4, -1, -1);
            sfx = 0x3C;
        }
        break;
    case 8: /* ABANDON MENU */
        if (CROSS != 0) {
            if (y_cursor_temp == 0) {
                new_level = (Level == 0x25) ? 0x23 : 0x25;
                sfx = 0x36;
            } else {
                NewMenu(cursor, 3, -1, -1);
                sfx = 0x3C;
            }
        } else if (TRIANGLE != 0) {
            NewMenu(cursor, 3, -1, -1);
            sfx = 0x3C;
        }
        break;
    case 9: /* DEBUG MENU */
        if (CROSS != 0) {
            switch (y_cursor_temp) {
            case 0:
                new_mode = GameMode;
                ResetCheckpoint(-1, -1, 0.0f, NULL);
                LivesLost = 0;
                LostLife = 0;
                ResetBonus();
                ResetDeath();
                ResetGemPath();
                ResetItems();
                break;
            case 1:
                ResetCheckpoint(-1, -1, 0.0f, NULL);
                LivesLost = 0;
                LostLife = 0;
                ResetBonus();
                bonus_restart = 0;
                ResetDeath();
                ResetGemPath();
                RestoreCrateTypeData();
                ResetCrates();
                ResetWumpa();
                ResetChases();
                ResetPlayerEvents();
                ResetGates();
                ResetRings();
                memset(MaskFeathers, 0, 0x240);
                ResetAI();
                ResetPlayer(0);
                ResetBug();
                ResetLevel();
                ResetProjectiles();
                NewMenu(cursor, -1, -1, -1);
                pause_dir = 0;
                Paused = 0;
                ResumeGame();
                edobjResetAnimsToZero();
                break;
            case 2:
                NewMenu(cursor, 0xC, -1, -1);
                break;
            case 3:
                InvincibilityCHEAT = 1 - InvincibilityCHEAT;
                ResetAI();
                break;
            case 4:
                if (GotoCheckpoint(&player->obj.pos, 0) != 0) {
                    new_mode = GameMode;
                }
                break;
            case 5:
                if (GotoCheckpoint(&player->obj.pos, 1) != 0) {
                    new_mode = GameMode;
                }
                break;
            case 6:
                OpenGame();
                NewMenu(cursor, 3, -1, -1);
                break;
            case 7:
                LIFTPLAYER = 1 - LIFTPLAYER;
                break;
            case 8:
                ShowPlayerCoordinate = 1 - ShowPlayerCoordinate;
                break;
            case 9:
                ExtraMoves = 1 - ExtraMoves;
                break;
            case 10:
                NewGame();
                ResetBonus();
                ResetDeath();
                ResetGemPath();
                CalculateGamePercentage(&Game);
                Hub = HubFromLevel(Level);
                NewMenu(cursor, 3, -1, -1);
                break;
            }
        } else if (TRIANGLE != 0) {
            NewMenu(cursor, 3, -1, -1);
        }
        break;
    case 13: /* DEBUG MOVIE MENU */
        if (CROSS != 0) {
            switch (y_cursor_temp) {
            case 0:
                logos_played = 0;
                cutmovie = 0;
                break;
            case 1:
                cutmovie = 1;
                break;
            case 2:
                cutmovie = 2;
                break;
            case 3:
                cutmovie = 3;
                break;
            case 4:
                cutmovie = 4;
                break;
            }
            NewMenu(cursor, -1, -1, -1);
            new_level = 0x25;
        } else if (TRIANGLE != 0) {
            NewMenu(cursor, 9, -1, -1);
        }
        break;
    case 10: /* DEBUG DRAW MENU */
        if (CROSS != 0) {
            break;
        }
        if (TRIANGLE != 0) {
            NewMenu(cursor, 9, -1, -1);
            break;
        }
        switch (y_cursor_temp) {
        case 0:
            if (FASTLEFT != 0 && LDATA->farclip > 0xa) {
                LDATA->farclip--;
            } else if (FASTRIGHT != 0 && LDATA->farclip < 1000) {
                LDATA->farclip++;
            }
            if ((s32) LDATA->fogfar > LDATA->farclip) {
                LDATA->fogfar = LDATA->farclip;
            }
            if ((s32) LDATA->fognear > LDATA->fogfar) {
                LDATA->fognear = LDATA->fogfar;
            }
            if (pNuCam != NULL) {
                pNuCam->farclip = (float) LDATA->farclip;
            }
            break;
        case 1:
            if (FASTLEFT != 0 && LDATA->fognear > 1.0f) {
                LDATA->fognear -= 1.0f;
            } else if (FASTRIGHT != 0 && (s32) LDATA->fognear < LDATA->farclip && LDATA->fognear < LDATA->fogfar) {
                LDATA->fognear += 1.0f;
            }
            break;
        case 2:
            if (FASTLEFT != 0 && LDATA->fogfar > 1.0f && LDATA->fogfar > LDATA->fognear) {
                LDATA->fogfar -= 1.0f;
            } else if (FASTRIGHT != 0 && (s32) LDATA->fogfar < LDATA->farclip) {
                LDATA->fogfar += 1.0f;
            }
            break;
        case 3:
            if (FASTLEFT != 0 && LDATA->fogr != 0) {
                LDATA->fogr--;
            } else if (FASTRIGHT != 0 && LDATA->fogr < 0xff) {
                LDATA->fogr++;
            }
            break;
        case 4:
            if (FASTLEFT != 0 && LDATA->fogg != 0) {
                LDATA->fogg--;
            } else if (FASTRIGHT != 0 && LDATA->fogg < 0xff) {
                LDATA->fogg++;
            }
            break;
        case 5:
            if (FASTLEFT != 0 && LDATA->fogb != 0) {
                LDATA->fogb--;
            } else if (FASTRIGHT != 0 && LDATA->fogb < 0xff) {
                LDATA->fogb++;
            }
            break;
        case 6:
            if (FASTLEFT != 0 && LDATA->foga != 0) {
                LDATA->foga--;
            } else if (FASTRIGHT != 0 && LDATA->foga < 0x7f) {
                LDATA->foga++;
            }
            break;
        case 7:
            if (FASTLEFT != 0 && LDATA->hazer != 0) {
                LDATA->hazer--;
            } else if (FASTRIGHT != 0 && LDATA->hazer < 0xff) {
                LDATA->hazer++;
            }
            break;
        case 8:
            if (FASTLEFT != 0 && LDATA->hazeg != 0) {
                LDATA->hazeg--;
            } else if (FASTRIGHT != 0 && LDATA->hazeg < 0xff) {
                LDATA->hazeg++;
            }
            break;
        case 9:
            if (FASTLEFT != 0 && LDATA->hazeb != 0) {
                LDATA->hazeb--;
            } else if (FASTRIGHT != 0 && LDATA->hazeb < 0xff) {
                LDATA->hazeb++;
            }
            break;
        case 10:
            if (FASTLEFT != 0 && LDATA->hazea != 0) {
                LDATA->hazea--;
            } else if (FASTRIGHT != 0 && LDATA->hazea < 0xff) {
                LDATA->hazea++;
            }
            break;
        }
        break;
    case 11: /* DEBUG MEMORY MENU */
        if (TRIANGLE != 0) {
            NewMenu(cursor, 9, -1, -1);
        }
        break;
    case 12: /* GOTO LEVEL MENU */
        if (CROSS != 0) {
            if (HData[x_cursor_temp].level[y_cursor_temp] != -1) {
                new_level = HData[x_cursor_temp].level[y_cursor_temp];
                Hub = HubFromLevel(HData[x_cursor_temp].level[y_cursor_temp]);
            }
        } else if (TRIANGLE != 0) {
            NewMenu(cursor, 9, -1, -1);
        }
        break;
    case 0: /* MAIN MENU */
        loadsaveCallEachFrame();
        if (CROSS != 0) {
            if (y_cursor_temp == 0) {
                if (saveload_cardtype != 2) {
                    NewMenu(cursor, 1, 0, -1);
                } else if (saveload_cardformatted == 0) {
                    NewMenu(cursor, 2, 0, -1);
                } else if (saveload_freespace < 0x42) {
                    NewMenu(cursor, 1, 0, -1);
                } else {
                    NewMenu(cursor, 2, 0, -1);
                }
                sfx = 0x36;
            } else if (y_cursor_temp == 1) {
                NuStrCpy(Game.name, D_0061DA28);
                Game.cutbits |= 1;
                new_level = 0x25;
                force_menu = 0x14;
                if (cutmovie == 0) {
                    fade_rate = 0xA;
                }
                sfx = 0x36;
            }
        } else if (y_cursor_temp == 2) {
            old = Game.language;
            if (LEFT != 0) {
                if ((u32) Game.language < 5) {
                    Game.language++;
                }
            } else if (RIGHT != 0 && Game.language != 0) {
                Game.language--;
            }
            if (Game.language != old) {
                NewLanguage(Game.language);
                sfx = 0x36;
            }
        }
        break;
    case 1:
        loadsaveCallEachFrame();
        if (CROSS != 0) {
            NewMenu(cursor, 2, 0, -1);
        } else if (TRIANGLE != 0) {
            NewMenu(cursor, 0, -1, -1);
        }
        break;
    case 2:
        loadsaveCallEachFrame();
        if (CROSS != 0) {
            if (y_cursor_temp == cursor->y_max - 1) {
                CleanLetters(Game.name);
                if (strcmp(Game.name, D_0061DA38) == 0) {
                    NuStrCpy(Game.name, D_0061DA28);
                }
                new_level = 0x25;
                if (cutmovie == 0) {
                    next_cut_movie = 1;
                    NewMenu(cursor, -1, -1, -1);
                    fade_rate = 0xA;
                } else {
                    cutmovie = 1;
                }
                sfx = 0x36;
                break;
            }
            if (y_cursor_temp == cursor->y_max) {
                NewMenu(cursor, 0, -1, -1);
                sfx = 0x36;
                break;
            }
            if (Game.language == 0x63) {
                if (y_cursor_temp == cursor->y_max - 2) {
                    input_alphabet = 2;
                    GetMenuInfo(cursor);
                    sfx = 0x36;
                    break;
                } else if (y_cursor_temp == cursor->y_max - 3) {
                    input_alphabet = 1;
                    GetMenuInfo(cursor);
                    sfx = 0x36;
                    break;
                } else if (y_cursor_temp == cursor->y_max - 4) {
                    input_alphabet = 0;
                    GetMenuInfo(cursor);
                    sfx = 0x36;
                    break;
                } else if (input_alphabet == 2) {
                    if (cursor->y == 0 || cursor->y == 4) {
                        sfx = 0x36;
                        break;
                    }
                }
            }
            if (NameInputTable[cursor->y][cursor->x] == '<') {
                if (i_nameinput > 0) {
                    i_nameinput--;
                    Game.name[i_nameinput] = '_';
                }
                sfx = 0x36;
                break;
            }
            if (i_nameinput >= 7) {
                cursor->y = 4;
            }
            Game.name[i_nameinput] = NameInputTable[cursor->y][cursor->x];
            if (i_nameinput < 8) {
                i_nameinput++;
            }
            sfx = 0x36;
        } else if (TRIANGLE != 0) {
            NewMenu(cursor, 0, -1, -1);
            sfx = 0x36;
        }
        break;
    case 17:
        if ((bits & 0x800) != 0 || cut_on == 0) {
            if (cursor->new_level != -1) {
                new_level = cursor->new_level;
            } else {
                NewMenu(cursor, -1, -1, -1);
            }
            sfx = 0x3C;
        }
        break;
    case 32:
        if ((bits & 0x800) != 0) {
            new_level = 0x25;
        } else if (credit_time <= D_0059202C[0]) {
            new_level = 0x25;
        }
        break;
    case 18:
        if (CROSS != 0) {
            if (D_0058B0C8[0] == 0x22) {
                if (y_cursor_temp == 0) {
                    cortex_gameover_i = cortex_continue_i;
                    cortex_continue_i++;
                    if (cortex_continue_i == 2) {
                        cortex_continue_i = 0;
                    }
                    tempanim_nextaction = cortex_gameover_tab[cortex_gameover_i][0];
                    if (D_0056233A[0] == -1 || CModel[D_0056233A[0]].anmdata_0 == NULL) {
                        D_0058B504[0] = 4;
                        new_level = 0x25;
                    } else {
                        gamesfx_channel = 4;
                        GameSfx(cortex_gameover_tab[cortex_gameover_i][1], NULL);
                        tempanim_waitaudio = 1;
                    }
                    sfx = 0x36;
                } else if (y_cursor_temp == 1) {
                    cortex_gameover_i = cortex_quit_i;
                    cortex_quit_i++;
                    if (cortex_quit_i == 7) {
                        cortex_quit_i = 2;
                    }
                    tempanim_nextaction = cortex_gameover_tab[cortex_gameover_i][0];
                    if (D_0056233A[0] == -1 || CModel[D_0056233A[0]].anmdata_0 == NULL) {
                        new_level = 0x23;
                    } else {
                        gamesfx_channel = 4;
                        GameSfx(cortex_gameover_tab[cortex_gameover_i][1], NULL);
                        tempanim_waitaudio = 1;
                    }
                    sfx = 0x3C;
                }
            }
        }
        break;
    case 14:
    case 16:
        if (CROSS != 0) {
            sfx = 0x36;
            if (y_cursor_temp == 0) {
                new_mode = GameMode;
                ResetCheckpoint(-1, -1, 0.0f, NULL);
                ResetBonus();
                ResetDeath();
                ResetGemPath();
            } else if (y_cursor_temp == 1) {
                new_level = 0x25;
            }
        }
        break;
    case 15:
        if (CROSS != 0) {
            if (y_cursor_temp == cursor->y_max) {
                txt = Game.level[Level].time[newleveltime_slot].name;
                CleanLetters(txt);
                if (strcmp(txt, D_00630C48) == 0) {
                    NuStrCpy(txt, D_00630C50);
                }
                if ((new_lev_flags & 7) != 0) {
                    new_level = 0x25;
                } else {
                    NewMenu(cursor, 0xE, 0, -1);
                }
                sfx = 0x36;
                break;
            }
            if (Game.language == 0x63) {
                if (y_cursor_temp == cursor->y_max - 1) {
                    input_alphabet = 2;
                    GetMenuInfo(cursor);
                    sfx = 0x36;
                    break;
                } else if (y_cursor_temp == cursor->y_max - 2) {
                    input_alphabet = 1;
                    GetMenuInfo(cursor);
                    sfx = 0x36;
                    break;
                } else if (y_cursor_temp == cursor->y_max - 3) {
                    input_alphabet = 0;
                    GetMenuInfo(cursor);
                    sfx = 0x36;
                    break;
                } else if (input_alphabet == 2) {
                    if (y_cursor_temp == 0 || y_cursor_temp == 4) {
                        sfx = 0x36;
                        break;
                    }
                }
            }
            if (NameInputTable[cursor->y][cursor->x] == '<') {
                if (i_nameinput > 0) {
                    i_nameinput--;
                    Game.level[Level].time[newleveltime_slot].name[i_nameinput] = '_';
                }
                sfx = 0x36;
                break;
            }
            if (i_nameinput >= 2) {
                cursor->y = 4;
            }
            Game.level[Level].time[newleveltime_slot].name[i_nameinput] = NameInputTable[cursor->y][cursor->x];
            if (i_nameinput < 3) {
                i_nameinput++;
            }
            sfx = 0x36;
        }
        break;
    case 19:
        loadsaveCallEachFrame();
        if (saveload_cardchanged != 0) {
            NewMenu(cursor, 0x13, -1, -1);
        }
        UpdateSaveSlots(cursor);
        if (CROSS != 0) {
            if (cursor->y < cursor->y_max) {
                game = &SaveSlot[cursor->x * 2 + cursor->y];
                if (game->empty == 0) {
                    NewMenu(cursor, 0x15, 1, -1);
                    sfx = 0x36;
                } else {
                    sfx = 2;
                }
            } else {
                NewMenu(cursor, 0x13, -1, -1);
                sfx = 0x3C;
            }
        }
        break;
    case 20:
        loadsaveCallEachFrame();
        if (saveload_cardchanged != 0) {
            NewMenu(cursor, 0x13, -1, -1);
        }
        if (CROSS != 0) {
            if (cursor->y == 1) {
                NewMenu(cursor, 0x14, -1, -1);
                sfx = 0x3C;
            } else {
                Game = *game;
                NewLanguage(Game.language);
                NewMenu(cursor, 0x16, 0, -1);
                memcard_loadresult_delay = 0x28;
                plr_lives.count = (short) Game.lives;
                D_0063203A = plr_lives.count;
                NuPs2VideoSetPos(Game.screen_x << 2, Game.screen_y << 1);
            }
        }
        break;
    case 21:
        loadsaveCallEachFrame();
        UpdateSaveSlots(cursor);
        if (memcard_loadresult_delay != 0) {
            break;
        }
        NewMenu(cursor, -1, -1, -1);
        tumble_action = -1;
        tumble_duration = 0.0f;
        tumble_time = 0.0f;
        last_level = -1;
        last_hub = -1;
        Hub = -1;
        player->obj.hdg = 0x8000;
        if (pos_START != NULL) {
            player->obj.pos = *pos_START;
        }
        break;
    case 22:
        loadsaveCallEachFrame();
        if (saveload_cardchanged != 0) {
            NewMenu(cursor, 0x13, -1, -1);
        }
        UpdateSaveSlots(cursor);
        if (saveload_cardtype == 2 && saveload_cardformatted == 0 && memcard_formatme == 0 && memcard_formatting == 0) {
            NewMenu(cursor, 0x1D, 1, -1);
        }
        if (CROSS != 0) {
            if (memcard_formatting == 0 && memcard_formatme == 0 && memcard_formatmessage_delay == 0) {
                if (cursor->y < cursor->y_max) {
                    game = &SaveSlot[cursor->x * 2 + cursor->y];
                    NewMenu(cursor, 0x18, 1, -1);
                    sfx = 0x36;
                } else {
                    NewMenu(cursor, 0x13, 1, -1);
                    sfx = 0x3C;
                }
            }
        }
        break;
    case 23:
        loadsaveCallEachFrame();
        if (saveload_cardchanged != 0) {
            NewMenu(cursor, 0x13, -1, -1);
        }
        if (CROSS != 0) {
            if (cursor->y == 1) {
                NewMenu(cursor, 0x17, -1, -1);
                sfx = 0x3C;
            } else if ((game->relics | Game.relics) & 7) {
                /* placeholder */
            }
        }
        break;
    case 24:
        loadsaveCallEachFrame();
        if (saveload_cardchanged != 0) {
            NewMenu(cursor, 0x13, -1, -1);
        }
        if (CROSS != 0) {
            /* save slot logic */
        }
        break;
    case 25:
    case 28:
        loadsaveCallEachFrame();
        UpdateSaveSlots(cursor);
        if (memcard_saveneeded != 0 || memcard_savestarted != 0 || memcard_deleteneeded != 0 ||
            memcard_deletestarted != 0 || memcard_savemessage_delay != 0) {
            memcard_saveresult_delay = 0x32;
        }
        if (memcard_saveresult_delay != 0) {
            break;
        }
        NewMenu(cursor, 0x13, 3, -1);
        break;
    case 30:
        loadsaveCallEachFrame();
        UpdateSaveSlots(cursor);
        if (memcard_formatting != 0 || memcard_formatme != 0 || memcard_formatmessage_delay != 0) {
            memcard_saveresult_delay = 0x32;
        }
        if (memcard_saveresult_delay != 0) {
            break;
        }
        if (memcard_formatfailed != 0) {
            NewMenu(cursor, 0x13, 3, -1);
        } else {
            NewMenu(cursor, 0x17, -1, -1);
        }
        break;
    case 26:
        loadsaveCallEachFrame();
        if (saveload_cardchanged != 0) {
            NewMenu(cursor, 0x13, -1, -1);
        }
        UpdateSaveSlots(cursor);
        if (CROSS != 0) {
            if (cursor->y < cursor->y_max) {
                game = &SaveSlot[cursor->x * 2 + cursor->y];
                if (game->empty == 0) {
                    NewMenu(cursor, 0x1B, 1, -1);
                    sfx = 0x36;
                } else {
                    sfx = 2;
                }
            } else {
                NewMenu(cursor, 0x13, -1, -1);
                sfx = 0x3C;
            }
        }
        break;
    case 27:
        loadsaveCallEachFrame();
        if (saveload_cardchanged != 0) {
            NewMenu(cursor, 0x13, -1, -1);
        }
        if (CROSS != 0) {
            if (cursor->y == 1) {
                NewMenu(cursor, 0x1A, -1, -1);
                sfx = 0x3C;
            } else {
                game->empty = 1;
                nosaves = 1;
                {
                    s32 i;
                    for (i = 0; i < 2; i++) {
                        s32 k;
                        for (k = 1; k >= 0; k--) {
                            if (SaveSlot[i].empty == 0) {
                                nosaves = 0;
                            }
                        }
                    }
                }
                if (nosaves != 0) {
                    memcard_deleteneeded = 1;
                } else {
                    memcard_saveneeded = 1;
                }
                NewMenu(cursor, 0x1C, -1, -1);
                sfx = 0x36;
            }
        }
        break;
    case 29:
        loadsaveCallEachFrame();
        if (saveload_cardchanged != 0) {
            NewMenu(cursor, 0x13, -1, -1);
        }
        if (CROSS != 0) {
            if (cursor->y == 1) {
                memcard_formatme = 1;
                NewMenu(cursor, 0x13, -1, -1);
            } else {
                NewMenu(cursor, 0x1E, -1, -1);
                sfx = 0x36;
            }
        }
        break;
    case 31:
        if (uVar16 != 0) {
            if (Level == 0x16 || Level == 0x18) {
                boss_dead = 2;
            }
            NewMenu(cursor, -1, -1, -1);
            sfx = 0x36;
        }
        break;
    case 33:
        if (uVar16 != 0) {
            sfx = 0x36;
            NuSoundStopStream(4);
            NewMenu(cursor, -1, -1, -1);
        } else if (player->obj.mask != NULL &&
                   ((player->obj.mask->flags_163 & 1) != 0 ||
                    0x5db < cursor->item_frame || player->obj.mask->active >= 3)) {
            NewMenu(cursor, -1, -1, -1);
        }
        break;
    }
    if (sfx != -1) {
        GameSfx(sfx, NULL);
    }
    return;
}

void DrawMenuEntry(struct cursor_s *cursor, char *txt, float *x, float *y,
                   s32 *i) {
    s32 col;
    float size;
    float k;

    if (*i >= cursor->y_min && *i <= cursor->y_max) {
        if (*i == cursor->y) {
            col = (*(u32 *)GlobalTimer % 10 < 5) ? 0 : 3;
        } else {
            col = 2;
        }
        k = D_0062D3E4;
        size = dme_sy * k;
        if (strcmp(txt, tPRESSxTOCONTINUE[Game.language]) == 0) {
            if (Game.language == 1) {
                dme_sx = dme_sx * 0.75;
            } else if (Game.language == 2 || Game.language == 3) {
                dme_sx = dme_sx * D_0062D3E8;
            } else if (Game.language == 5) {
                dme_sx = dme_sx * D_0062D3EC;
            }
        } else if (strcmp(txt, tADJUSTSCREEN[Game.language]) == 0) {
            if (Game.language == 4) {
                dme_sx = dme_sx * D_0062D3F0;
            } else if (Game.language == 5) {
                dme_sx = dme_sx * D_0062D3F4;
            }
        } else if (strcmp(txt, tRESTARTTRIAL[Game.language]) == 0) {
            if (cursor->menu != 0xE) {
                if (Game.language == 1) {
                    dme_sx = dme_sx * D_0062D3F8;
                } else if (Game.language == 2) {
                    dme_sx = dme_sx * D_0062D3FC;
                } else if (Game.language == 3) {
                    dme_sx = dme_sx * k;
                } else if (Game.language == 4) {
                    dme_sx = dme_sx * D_0062D400;
                } else if (Game.language == 5) {
                    dme_sx = dme_sx * D_0062D404;
                }
            }
        }
        Text3D(txt, 0, col, *x, *y + dme_yadj, 1.0f, size * dme_sx,
               size * dme_symul, size);
    }
    *y += MENUDY * dme_sy;
    (*i)++;
    dme_symul = 1.0f;
    dme_sx = 1.0f;
    dme_sy = 1.0f;
    dme_yadj = 0.0f;
}

void DrawMenuEntry2(struct cursor_s *cursor, char *txt0, char *txt1, float *x,
                    float *y, s32 *i) {
    s32 col;
    float size;
    float sx;

    if (*i >= cursor->y_min && *i <= cursor->y_max) {
        if (*i == cursor->y) {
            col = (*(u32 *)GlobalTimer % 10 < 5) ? 0 : 3;
        } else {
            col = 2;
        }
        size = dme_sy * D_0062D408;
        sx = dme_sy;
        if (strcmp(txt0, tSFXVOLUME[Game.language]) == 0 ||
            strcmp(txt0, tMUSICVOLUME[Game.language]) == 0) {
            if (Game.language == 2) {
                sx = sx * D_0062D40C;
            } else if (Game.language == 4) {
                sx = sx * 0.75f;
            }
        }
        Text3D(txt0, 0, col, *x, *y, 1.0f, size * sx, size, size);
        *y = MENUDY * dme_sy + *y;
        Text3D(txt1, 0, col, *x, *y, 1.0f, size * dme_sx, size, size);
    }
    *y += MENUDY * dme_sy;
    (*i)++;
    dme_sy = 1.0f;
    dme_sx = 1.0f;
}

/*
 * DrawMenu: equivalent reconstruction, stays asm. Won't byte-match -- retail
 * emits mtc1->FP hazard nops at 12 sites that ee-gcc-tt does not. Cases follow
 * jump table jtbl_0061DF40 and the PS2 string tables.
 */
void DrawMenu(struct cursor_s *cursor, s32 paused) {
    s32 i;
    float x;
    float y;
    float dy;
    float dvar8;

    if (editor_active != 0) {
        return;
    }
    if (cursor->wait != 0) {
        return;
    }
    if (cursor->menu == -1) {
        return;
    }
    if (GameMode == 1) {
        return;
    }
    x = 0.0f;
    if (pause_dir != 0) {
        x = (0x19 - paused) * D_0062D410;
        if (pause_dir == 1) {
            x = 0.0f - x;
        } else {
            x = x + 0.0f;
        }
    }
    dy = (cursor->y_max - cursor->y_min) * MENUDY;
    dvar8 = dy * 0.5f;
    i = 0;
    switch (cursor->menu) {
    case 0: /* main menu */
        y = -0.75f - dvar8;
        DrawMenuEntry(cursor, tNEWGAME[Game.language], &x, &y, &i);
        DrawMenuEntry(cursor, tLOADGAME[Game.language], &x, &y, &i);
        sprintf(tbuf, D_00630C78, tLANGUAGE[Game.language],
                LanguageName[Game.language]);
        DrawMenuEntry(cursor, tbuf, &x, &y, &i);
        break;
    case 2: /* name entry (new game) */
        DrawNameInputTable(cursor, 0.0f, -0.5f);
        Text3D(MakeEditText(Game.name), 0, 4, 0.0f, GAMENAMEY, 1.0f, 1.0f, 1.0f,
               1.0f);
        return;
    case 3: /* pause menu */
        if (paused < 0x1e) {
            return;
        }
        x = PANELMENUX;
        y = -0.5f - dvar8;
        DrawMenuEntry(cursor, tRESUME[Game.language], &x, &y, &i);
        DrawMenuEntry(cursor, tOPTIONS[Game.language], &x, &y, &i);
        DrawMenuEntry(cursor,
                      (Level == 0x25) ? tQUIT[Game.language]
                                      : tWARPROOM[Game.language],
                      &x, &y, &i);
        if (TimeTrial == 0) {
            return;
        }
        DrawMenuEntry(cursor, tRESTARTTRIAL[Game.language], &x, &y, &i);
        break;
    case 7: /* language select */
        if (paused < 0x1e) {
            return;
        }
        x = PANELMENUX;
        y = -0.5f - dvar8;
        for (i = 0; i < 6; i++) {
            DrawMenuEntry(cursor, LanguageName[i], &x, &y, &i);
        }
        return;
    case 8: /* quit confirm */
        if (paused < 0x1e) {
            return;
        }
        x = PANELMENUX;
        y = -0.5f - dvar8;
        DrawMenuEntry(cursor, tYES[Game.language], &x, &y, &i);
        DrawMenuEntry(cursor, tNO[Game.language], &x, &y, &i);
        break;
    case 14: /* restart time trial */
        y = D_0062D488 - dvar8;
        DrawMenuEntry(cursor, tRESTARTTRIAL[Game.language], &x, &y, &i);
        DrawMenuEntry(cursor, tWARPROOM[Game.language], &x, &y, &i);
        return;
    case 15: /* time-trial name entry */
        DrawNameInputTable(cursor, 0.0f, -0.5f);
        return;
    case 16: /* restart race */
        y = D_0062D48C - dvar8;
        DrawMenuEntry(cursor, tRESTARTRACE[Game.language], &x, &y, &i);
        DrawMenuEntry(cursor, tWARPROOM[Game.language], &x, &y, &i);
        DrawMenuEntry(cursor, tNO[Game.language], &x, &y, &i);
        return;
    case 17: /* empty */
        return;
    case 32: /* credits */
        DrawCredits();
        return;
    /*
     * TODO (not yet reconstructed): case 1 (memory-card), 4/5/6 (options/
     * sound/screen), 9-13 (debug menus), 18 (game-over), 19-31 (memcard state
     * machines + power screen) -- each a distinct PS2 case block.
     */
    }
}

void InputNewLetter(struct cursor_s *cursor, char *name, s32 *i, s32 count) {
    char c0;
    s32 j;

    c0 = NameInputTable[cursor->y][cursor->x];
    if (c0 == '<') {
        if (*i > 0) {
            (*i)--;
            name[*i] = '_';
        }
        return;
    }
    j = count - 1;
    if (*i >= j) {
        cursor->y = 4;
    } else {
        j = *i;
    }
    name[j] = c0;
    if (*i < count) {
        *i = *i + 1;
    }
}

void CleanLetters(char *txt) {
    while (*txt != '\0') {
        if (*txt == '_') {
            *txt = ' ';
        }
        txt++;
        if (Game.language == 'c') {
            txt++;
        }
    }
}

void NextMenuEntry(float *y, s32 *i) {
    *y += MENUDY * dme_sy;
    *i += 1;
}

void InitLevel(void) {
    s32 lp;
    s32 y;
    s32 menu;
    s32 level;
    struct nuvec_s start;
    struct nuvec_s end;
    struct nuinstance_s *ipost;
    struct nuinstance_s *instgrp[24];
    u8 bits;
    s32 open;

    InitPositions();
    ResetTempCharacter(-1, -1);
    menu = -1;
    ResetTempCharacter2(-1, -1);
    y = -1;
    level = -1;
    NuLightFogClear(0);
    tempanim_waitaudio = 1;
    switch (Level) {
    case 0x23:
        NewGame();
        y = 0;
        Hub = -1;
        menu = 0;
        ResetTempCharacter(0x60, 0x2b);
        Demo = 0;
        break;
    case 0x25:
        CalculateGamePercentage(&Game);
        ResetTempCharacter(2, 0x22);
        hub_vr_level = -1;
        loadsave_frame = 0x33;
        warp_level = -1;
        break;
    case 0x27:
        menu = 0x11;
        level = 0x25;
        break;
    case 0x29:
        menu = 0x11;
        level = 0x2b;
        break;
    case 0x26:
        ResetTempCharacter(2, 0x22);
        menu = 0x12;
        tempanim_nextaction = 0x73;
        y = 0;
        gamesfx_channel = 4;
        GameSfx(0xc6, NULL);
        tempanim_waitaudio = 1;
        cortex_gameover_i = -1;
        break;
    case 0x2b:
        InitCredits();
        menu = 0x20;
        ResetTempCharacter(0xbb, 0x22);
        level = 0x2b;
        ResetTempCharacter2(0, 0x22);
        break;
    case 0xd:
        ResetTempCharacter(0x62, 0x22);
        break;
    case 0x1a:
        ResetTempCharacter(0xb8, 0x22);
        break;
    case 0x12:
        ResetTempCharacter(0xb9, 0x22);
        break;
    case 0x1e:
        ResetTempCharacter(0xba, 0x22);
        break;
    case 0x15:
    case 0x16:
    case 0x18:
        ResetTempCharacter(0xbc, 0x22);
        break;
    case 0x13:
        if ((ObjTab[145].obj.special != NULL) &&
            (ObjTab[146].obj.special != NULL)) {
            for (lp = 0; lp < 0x18; lp++) {
                if (lp & 1) {
                    instgrp[lp] = ObjTab[145].obj.special->instance;
                } else {
                    instgrp[lp] = ObjTab[146].obj.special->instance;
                }
            }
            start.x = 36.3721f;
            start.y = -0.1f;
            start.z = 140.7f;
            end.x = 36.49f;
            end.y = -0.1f;
            end.z = 146.84f;
            NuBridgeCreate(instgrp, NULL, &start, &end, 1.2f, -0x4200, 0.3f,
                           0.075f, -0.01f, 4.0f, 10, 1.0f, 0.5f, 3,
                           -0x7f7f7f80);
            start.x = 206.1f;
            start.y = 0.0f;
            start.z = 249.8f;
            end.x = 213.25f;
            end.y = 0.0f;
            end.z = 249.8f;
            NuBridgeCreate(instgrp, NULL, &start, &end, 1.2f, 0, 0.25f, 0.08f,
                           -0.005f, 7.0f, 0xc, 1.0f, 0.5f, 3, -0x7f7f7f80);
        }
        break;
    case 8:
        if (ObjTab[155].obj.special != NULL) {
            if (ObjTab[158].obj.special != NULL) {
                ipost = ObjTab[158].obj.special->instance;
            } else {
                ipost = NULL;
            }
            for (lp = 0; lp < 0x18; lp++) {
                instgrp[lp] = ObjTab[155].obj.special->instance;
            }
            start.x = -79.875f;
            start.y = -14.2f;
            start.z = 22.7f;
            end.x = -79.875f;
            end.y = -14.2f;
            end.z = 26.0f;
            NuBridgeCreate(instgrp, ipost, &start, &end, 1.2f, -0x4000, 0.18f,
                           0.1f, -0.01f, 4.0f, 7, 1.08f, 0.5f, 3, -0x7f7f7f80);
            start.x = 52.5f;
            start.y = -33.7f;
            start.z = -73.45f;
            end.x = 63.9f;
            end.y = -33.7f;
            end.z = -73.14f;
            NuBridgeCreate(instgrp, ipost, &start, &end, 1.2f, 0, 0.4f, 0.05f,
                           -0.004f, 8.0f, 0x11, 1.08f, 0.5f, 3, -0x7f7f7f80);
            start.x = 70.0f;
            start.y = -33.7f;
            start.z = -73.12f;
            end.x = 77.0f;
            end.y = -33.7f;
            end.z = -73.3f;
            NuBridgeCreate(instgrp, ipost, &start, &end, 1.2f, 0, 0.25f, 0.075f,
                           -0.008f, 4.0f, 10, 1.08f, 0.5f, 3, -0x7f7f7f80);
        }
        break;
    case 1:
        if ((ObjTab[159].obj.special != NULL) &&
            (ObjTab[160].obj.special != NULL)) {
            ipost = NULL;
            if (ObjTab[161].obj.special != NULL) {
                ipost = ObjTab[161].obj.special->instance;
            }
            for (lp = 0; lp < 0x18; lp++) {
                if (lp == (lp / 3) * 3) {
                    instgrp[lp] = ObjTab[159].obj.special->instance;
                } else {
                    instgrp[lp] = ObjTab[160].obj.special->instance;
                }
            }
            start.x = -1.31f;
            start.y = 18.6f;
            start.z = 16.23f;
            end.x = -7.14f;
            end.y = 18.6f;
            end.z = 14.8f;
            NuBridgeCreate(instgrp, ipost, &start, &end, 1.2f, 31000, 0.12f,
                           0.1f, -0.01f, 4.0f, 7, 1.12f, 0.9f, 3, -0x7fffcfa8);
            start.x = -8.47f;
            start.y = 18.6f;
            start.z = 14.4f;
            end.x = -13.17f;
            end.y = 18.6f;
            end.z = 10.09f;
            NuBridgeCreate(instgrp, ipost, &start, &end, 1.2f, 25000, 0.12f,
                           0.1f, -0.01f, 4.0f, 7, 1.12f, 0.9f, 3, -0x7fffcfa8);
        }
        break;
    case 0x22:
        NuLightFogClear(1);
        break;
    }
    if ((LBIT & 0x3e00000) != 0) {
        D_0058A00E[0] = 0;
    }
    ai_lookpos = v000;
    ((struct cursor_s *)Cursor)->menu = -1;
    NewMenu((struct cursor_s *)Cursor, menu, y, level);
    VEHICLECONTROL = 0;
    if ((Level == 6) || (Level == 0x1d)) {
        VEHICLECONTROL = 1;
    } else if (Level == 0x22) {
        VEHICLECONTROL = 2;
    } else {
        VEHICLECONTROL = 0;
    }
    level_part_2 = 0;
    SKELETALCRASH = 0;
    if (Level != 0x25) {
        new_hub_flags = 0;
        new_lev_flags = 0;
    }
    /* ResetItems (inlined) */
    plr_bonusgem.count = 0;
    plr_crystal.draw = 0;
    plr_crystal.count = 0;
    plr_crategem.draw = 0;
    plr_crategem.count = 0;
    plr_bonusgem.draw = 0;
    plr_items = 0;
    boss_dead = 0;
    COMPLEXPLAYERSHADOW = (Level == 0xc);
    bonusgem_ok = (Level != 5);
    /* ResetGemPath (inlined) */
    gempath_begun = 0;
    GemPath = 0;
    switch (Level) {
    case 0xc:
        bits = 1;
        break;
    case 0x21:
        bits = 2;
        break;
    case 0xe:
        bits = 4;
        break;
    case 4:
        bits = 8;
        break;
    case 0x14:
        bits = 0x10;
        break;
    default:
        bits = 0;
        break;
    }
    if ((Game.gembits & bits) == 0) {
        open = 0;
    } else {
        open = 1;
    }
    gempath_open = open;
    if (ObjTab[101].obj.special != NULL) {
        ObjTab[101].obj.special->instance->flags.visible = 1 - gempath_open;
    }
    hubleveltext_level = -1;
    hubleveltext_pos = 0.0f;
    tumble_moveduration = 1.0f;
    tumble_duration = 1.0f;
    tumble_time = 1.0f;
    HubLevelText = 0;
    InitBugAreas();
    bonus_restart = 0;
    plr_invisibility_time = 5.0f;
    in_finish_range = 0;
    LivesLost = 0;
    if ((LBIT & 0x3e00000) != 0) {
        D_0058A00E[0] = 0;
    }
}

void UpdateLevel(void) {
    s32 sfx;
    s32 idx;
    s32 i;

    for (i = 0; i < 5; i++) {
        lev_ambpos[i] = player->obj.pos;
    }
    sfx = -1;
    idx = 0;
    switch (Level) {
    case 23:
        UpdateDRAINDAMAGE();
        sfx = 0xCD;
        break;
    case 38:
        sfx = 0xCD;
        break;
    case 25:
        sfx = UpdateCRUNCHTIME();
        idx = jcrunch;
        break;
    case 2:
        sfx = (VEHICLECONTROL == 0) ? 0xC8 : sfx;
        break;
    case 4:
    case 12:
    case 20:
    case 33:
        sfx = 0xC7;
        break;
    case 8:
        sfx = 0xCB;
        break;
    case 14:
        sfx = 0xD4;
        break;
    case 19:
        sfx = 0xD0;
        break;
    }
    if (sfx != -1) {
        gamesfx_effect_volume = 0x5FFE;
        GameSfxLoop(sfx, &lev_ambpos[idx]);
    }
}

struct creatobj_s *FindClock(void) {
    s32 i;

    for (i = 1; i < 9; i++) {
        if (Character[i].on != 0 && Character[i].character == 0x76) {
            return &Character[i];
        }
    }
    return NULL;
}

struct wumpa_s {
    char pad_0[0x18];
    struct nuvec_s pos;   /* 0x18 */
    struct nuvec_s mom;   /* 0x24 */
    char pad_30[0x8];     /* 0x30 */
    s32 field_38;         /* 0x38 */
    float field_3C;       /* 0x3C */
    signed char field_40; /* 0x40 */
    char pad_41[0x7];     /* 0x41 */
    signed char field_48; /* 0x48 */
    signed char field_49; /* 0x49 */
};

void NuVecRotateX(struct nuvec_s *dst, struct nuvec_s *src, s32 angle);
void NuVecRotateY(struct nuvec_s *dst, struct nuvec_s *src, s32 angle);
extern float D_0062D700;

void FlyWumpa(struct wumpa_s *obj) {
    obj->mom.x = 0.0f;
    obj->mom.y = 0.0f;
    obj->mom.z = D_0062D700;
    NuVecRotateX(&obj->mom, &obj->mom, -0x400);
    NuVecRotateY(&obj->mom, &obj->mom, qrand());
    obj->field_38 = 0;
    obj->field_40 = 3;
    obj->field_3C = 2.0f;
    obj->field_48 = 0;
    obj->field_49 = 0;
    GameSfx(0x2A, &obj->pos);
}

struct newwumpa_s {
    float x, y, z;        /* 0x0, 0x4, 0x8 */
    char pad_c[0x10];     /* 0xC */
    signed char count;    /* 0x1C */
    signed char b1d;      /* 0x1D */
    signed char b1e;      /* 0x1E */
    signed char b1f;      /* 0x1F */
    signed char b20;      /* 0x20 */
    char pad_21[0x3];     /* 0x21, stride 0x24 */
};
extern struct newwumpa_s NewWumpa[];
extern s32 i_newwumpa;
extern s32 sw_hack;
extern s32 Bonus;

void AddScreenWumpa(float x, float y, float z, s32 n) {
    NewWumpa[i_newwumpa].x = x;
    NewWumpa[i_newwumpa].y = y;
    NewWumpa[i_newwumpa].z = z;
    NewWumpa[i_newwumpa].count = (n > 0) ? n : 1;
    NewWumpa[i_newwumpa].b1d = 0;
    NewWumpa[i_newwumpa].b1e = 0;
    NewWumpa[i_newwumpa].b1f = (Bonus == 2 || sw_hack != 0);
    NewWumpa[i_newwumpa].b20 = 1;
    i_newwumpa++;
    if (i_newwumpa == 0x20) {
        i_newwumpa = 0;
    }
    sw_hack = 0;
}

void DrawLevel(void) {
    switch (Level) {
    case 37:
        HubDrawItems();
        break;
    case 25:
        DrawCRUNCHTIME();
        break;
    case 23:
        DrawDRAINDAMAGE();
        break;
    }
}
