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

struct numtx_s {
    float m[4][4]; /* 0x40 bytes; _30/_31/_32 are m[3][0..2] at +0x30 */
};


/* offsets verified in NewMask; `active` is signed (retail tests it with
 * lh + slti, not lhu + sltiu). */
struct mask_s {
    struct numtx_s mM; /* 0x0 (AddMaskFeathers) */
    struct numtx_s mS; /* 0x40 */
    struct nuvec_s pos; /* 0x80 */
    char pad_8c[0x98 - 0x8C];
    u8 lights[0x164 - 0x98]; /* 0x98 */
    float scale;  /* 0x164 */
    float shadow; /* 0x168 (AddMaskFeathers) */
    short character; /* 0x16C */
    short active;    /* 0x16E == D_0058A00E */
    char pad_170[0x174 - 0x170];
    u16 xrot;         /* 0x174 (MakeMaskMatrix) */
    u16 yrot;         /* 0x176 */
    char pad_178[0x17A - 0x178];
    u16 surface_xrot; /* 0x17A */
    u16 surface_zrot; /* 0x17C */
    char pad_end[0x190 - 0x17E];
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
    char pad_10[0x18 - 0x10];
    float anim_time;      /* 0x18 */
    char pad_1c[0x36 - 0x1C];
    short vehicle;        /* 0x36 */
    char pad_38[0x6C - 0x38];
    struct nuvec_s pos;   /* 0x6C */
    char pad_78[0x108 - 0x78];
    float SCALE;          /* 0x108 */
    char pad_10c[0x120 - 0x10C];
    float top;            /* 0x120 */
    char pad_124[0x149 - 0x124];
    signed char dead;     /* 0x149 */
    char pad_14a[0x153 - 0x14A];
    signed char finished; /* 0x153 */
    char pad_154[0x16E - 0x154];
    u16 hdg;              /* 0x16E */
    char pad_170[0x274 - 0x170];
    struct numtx_s mtxLOCATOR[2][16]; /* 0x274 */
    char pad_a74[0xCA4 - 0xA74];
    u8 rumble[4]; /* 0xCA4 */
};

struct player_s {
    struct pobj_s obj;
};

struct plr_lives_s {
    short count; /* 0x0 */
    short draw;  /* 0x2 */
};

struct anmdata_s {
    float time; /* 0x0 (AddMaskFeathers) */
};

/* stride 0x988; hobj/anmdata/shadow fields verified in DrawMaskFeathers */
struct cmodel_s {
    void *hobj;          /* 0x0 */
    struct anmdata_s *anmdata[0x10]; /* 0x4 */
    char pad_44[0x79C - 0x44];
    void *shadow_model; /* 0x79C */
    char pad_7a0[0x93C - 0x7A0];
    void *shadow; /* 0x93C */
    char pad_940[0x944 - 0x940];
    short character; /* 0x944 (also the D_0055FC88 shadow index) */
    char pad_946[0x988 - 0x946];
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
    char pad_b0[0xF8 - 0xB0];
    u16 yrot; /* 0xF8 */
    char pad_fa[0x114 - 0xFA];
    signed char mode; /* 0x114 */
};
extern struct gamecam_s GameCam;
extern s32 gamesfx_volume;
extern s32 LostLife;
extern s32 bonus_restart;
/* MaskFeathers entry: 4 x 0x90 = 0x240 (memset len in ResetMaskFeathers);
 * time (+0x80) / duration (+0x84) verified in UpdateMaskFeathers. */
struct mfeathers_s {
    struct numtx_s mM; /* 0x0 (AddMaskFeathers) */
    struct numtx_s mS; /* 0x40 (also the ShadRndr arg in DrawMaskFeathers) */
    float time;        /* 0x80 */
    float duration;    /* 0x84 */
    float shadow;      /* 0x88 */
    char pad_8c[0x90 - 0x8C];
};
extern struct mfeathers_s MaskFeathers[];
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

/* hgobj anim packet: mtx (+0x0) verified in PlayerObjectAnimCollision,
 * ltime (+0x4C) / playing (+0x50) in ResetHGobjAnim / StartHGobjAnim. */
struct hganim_s {
    struct numtx_s mtx; /* 0x0 */
    char pad_40[0x4C - 0x40];
    float ltime; /* 0x4C */
    /* signed: retail clears bit 0 with `addiu -2; and`, which gcc only emits
     * for a signed constant (u32 forces a 2-word lui+ori). */
    s32 playing; /* 0x50 */
};

struct nuinstance_s {
    struct numtx_s mtx; /* 0x0 (verified in PlayerObjectAnimCollision) */
    s32 objid;          /* 0x40 (verified in DrawParallax) */
    struct {
        u32 visible : 1;
        u32 rest : 31;
    } flags;               /* 0x44 */
    struct hganim_s *anim; /* 0x48 */
};

/* scene: gobjs (+0x14) verified in DrawParallax */
struct nugscn_s {
    char pad_14[0x14];
    void **gobjs; /* 0x14 */
};

struct objspecial_s {
    char pad_40[0x40];
    struct nuinstance_s *instance; /* 0x40 */
};

struct objinfo_s {
    struct nugscn_s *scene;       /* 0x0 (verified in DrawParallax) */
    struct objspecial_s *special; /* 0x4 */
};

/* entry stride 0x20; scene/visible/font3d_letter/name/levbits verified in
 * InitObjectTable */
struct objtab_s {
    struct objinfo_s obj;    /* 0x0 (scene +0x0, special +0x4) */
    struct nugscn_s **scene; /* 0x8 */
    signed char visible;     /* 0xC */
    char font3d_letter;      /* 0xD */
    char pad_e[0x10 - 0xE];
    char *name; /* 0x10 */
    char pad_14[0x18 - 0x14];
    u64 levbits; /* 0x18 */
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
extern u16 plr_items;
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
inline s32 qrand(void) {
    qseed = qseed * 0x24CD + 1;
    qseed = qseed & 0xFFFF;
    return qseed;
}

inline s32 HubFromLevel(s32 level) {
    s32 j;
    s32 i;

    if (level == -1) {
        return -1;
    }
    for (i = 0; i < 6; i++) {
        for (j = 0; j < 6; j++) {
            if (HData[i].level[j] == level) {
                temp_hub = i;
                temp_hublevel = j;
                return i;
            }
        }
    }
    return -1;
}

void inline InitProbe(void) {
    probeon = 0;
    probey = 0;
    probetime = 0;
    proberot.x = 0;
    proberot.y = 0;
    proberot.z = 0;
    probecol = 0;
}

inline void ResetTempCharacter(s32 character, s32 action) {
    temp_character = character;
    temp_character_action = action;
    ResetAnimPacket(TempAnim, action);
    ResetLights(TempLights);
}

inline void ResetTempCharacter2(s32 character, s32 action) {
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
                    if (D_0056233A[0] == -1 || CModel[D_0056233A[0]].anmdata[0] == NULL) {
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
                    if (D_0056233A[0] == -1 || CModel[D_0056233A[0]].anmdata[0] == NULL) {
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
    struct nuvec_s pos0; /* 0x0 (file-loaded spawn point; see LoadWumpa) */
    struct nuvec_s pos1; /* 0xC (verified in AddFlyingWumpa) */
    struct nuvec_s pos;   /* 0x18 */
    struct nuvec_s mom;   /* 0x24 */
    float shadow;         /* 0x30 (AddFlyingWumpa) */
    char pad_34[0x38 - 0x34];
    s32 field_38;         /* 0x38 */
    float field_3C;       /* 0x3C */
    signed char field_40; /* 0x40 */
    signed char field_41; /* 0x41 */
    short field_42;       /* 0x42 */
    float field_44;       /* 0x44 (fALONG; verified in WipeWumpa) */
    signed char field_48; /* 0x48 */
    signed char field_49; /* 0x49 */
    char pad_4a;
    signed char surface_type; /* 0x4B (AddFlyingWumpa) */
};

struct winfo_s {
    char pad_0[0x40];
    float field_40;   /* 0x40 */
    char pad_44[0x4]; /* 0x44 */
    short field_48;   /* 0x48 */
    short field_4A;   /* 0x4A */
    char pad_4c[0x4]; /* 0x4C, stride 0x50 */
};

extern struct wumpa_s Wumpa[320];
extern struct winfo_s WInfo[8];
extern s32 WUMPACOUNT;
/* takes no argument: retail LoadWumpa reaches Wumpa via %hi/%lo, and the a0
 * set-up an argument would need is exactly InitWumpa's one divergent word. */
void LoadWumpa(void);

void InitWumpa(void) {
    s32 i;

    memset(Wumpa, 0, sizeof(Wumpa));
    WUMPACOUNT = 0;
    if (LDATA->flags & 0x80) {
        LoadWumpa();
    }
    for (i = 0; i < 320; i++) {
        Wumpa[i].field_41 = -1;
        Wumpa[i].field_42 = -1;
    }
    for (i = 0; i < 8; i++) {
        WInfo[i].field_48 = 0;
        WInfo[i].field_4A = 0x7AE;
        WInfo[i].field_40 = 0.75f;
    }
    ResetWumpa();
}

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
    if (n < 1) {
        n = 1;
    }
    NewWumpa[i_newwumpa].x = x;
    NewWumpa[i_newwumpa].y = y;
    NewWumpa[i_newwumpa].z = z;
    NewWumpa[i_newwumpa].count = n;
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

extern void ResetVehicleLevel(s32 a);

struct gtimer_s {
    s32 frame;   /* 0x0 */
    s32 itime;   /* 0x4 */
    /* unsigned: UpdateTimer's wrap test is `sltiu`, not `slti`. */
    u32 isec;    /* 0x8 */
    float ftime; /* 0xC */
    float fsec;  /* 0x10 */
};

/* gnu89 plain inline: emits the standalone AND inlines into ResetTimeTrial,
 * which is how retail has it (no jal there). */
inline void ResetTimer(void *timer) {
    struct gtimer_s *t = timer;
    t->frame = 0;
    t->itime = 0;
    t->isec = 0;
    t->ftime = 0.0f;
    t->fsec = 0.0f;
}

void ResetBonus(void) {
    Bonus = 0;
}

void ResetLevel(void) {
    ResetVehicleLevel(1);
}

void ResetMaskFeathers(void) {
    memset(MaskFeathers, 0, 0x240);
}

/* stride 0x30; fields verified in AddAward */
struct award_s {
    float time;        /* 0x0 */
    u16 yrot;          /* 0x4 */
    u16 got;           /* 0x6 */
    signed char level; /* 0x8 */
    signed char wait; /* 0x9 */
    char pad_a[0xC - 0xA];
    struct nuvec_s oldpos0; /* 0xC  (UpdateAwards) */
    struct nuvec_s oldpos1; /* 0x18 (UpdateAwards) */
    struct nuvec_s newpos;  /* 0x24 */
};
extern struct award_s Award[];

void ResetAwards(void) {
    s32 i;
    for (i = 0; i < 3; i++) {
        Award[i].time = 1.0f;
    }
}

/* kaboom_s stride 0x24 (i_kaboom*0x24 at 0x001DF320; ResetKabooms memsets
 * 0xA20 = 0x48 * 0x24, matching the i_kaboom==0x48 wrap). */
struct kaboom_s {
    u16 type; /* 0x0 */
    short crate; /* 0x2 */
    struct nuvec_s pos; /* 0x4 (ldl/ldr + lw 12-byte copy) */
    float t0; /* 0x10 */
    float t1; /* 0x14 */
    float time; /* 0x18 */
    float duration; /* 0x1C */
    short group; /* 0x20 */
    char pad_22[2];
};

extern struct kaboom_s Kaboom[];
extern s32 i_kaboom;

void ResetKabooms(void) {
    memset(Kaboom, 0, 0xA20);
    i_kaboom = 0;
}

/* gnu89 plain inline: standalone AND inlined into TurnRot (retail has no jal there). */
inline s32 RotDiff(u16 a, u16 b) {
    s32 diff = b - a;
    if (0x8000 < diff) {
        diff -= 0x10000;
    } else if (diff < -0x8000) {
        diff += 0x10000;
    }
    return diff;
}

extern void NewBuzz(void *p, s32 n);

void TransporterGo(void) {
    GameSfx(0x82, &player->obj.pos);
    NewBuzz((char *)player + 0xCA4, 5);
}

u16 SeekRot(u16 a, u16 b, s32 shift) {
    s32 diff = b - a;
    if (0x8000 < diff) {
        diff -= 0x10000;
    } else if (diff < -0x8000) {
        diff += 0x10000;
    }
    return a + (diff >> shift);
}

s32 StartHGobjAnim(struct objinfo_s *obj) {
    if (obj->special != NULL && obj->special->instance != NULL &&
        obj->special->instance->anim != NULL) {
        obj->special->instance->anim->playing |= 1;
        return 1;
    }
    return 0;
}

s32 StopHGobjAnim(struct objinfo_s *obj) {
    struct nuinstance_s *instance;

    if (obj->special != NULL) {
        instance = obj->special->instance;
        if (instance != NULL && instance->anim != NULL) {
            instance->anim->playing &= ~1;
            return 1;
        }
    }
    return 0;
}

s32 ResetHGobjAnim(struct objinfo_s *obj) {
    if (obj->special != NULL && obj->special->instance != NULL &&
        obj->special->instance->anim != NULL) {
        obj->special->instance->anim->playing &= ~1;
        obj->special->instance->anim->ltime = 1.0f;
        return 1;
    }
    return 0;
}

extern float D_0062D6FC; /* mask-feather advance rate (PAL) */

void UpdateMaskFeathers(void) {
    struct mfeathers_s *f = MaskFeathers;
    s32 i;

    for (i = 0; i < 4; i++) {
        if (f->time < f->duration) {
            f->time += D_0062D6FC;
            if (f->time > f->duration) {
                f->time = f->duration;
            }
        }
        f++;
    }
}

extern struct gtimer_s TimeTrialTimer;
extern s32 TimeTrial;
extern float TimeTrialWait;
extern u32 timetrial_frame;
extern s32 clock_ok;
extern s32 PLAYERCOUNT;

/* gnu89 plain inline: standalone AND inlined into InitGameMode. */
inline void ResetTimeTrial(void) {
    s32 tmp;

    TimeTrial = 0;
    ResetTimer(&TimeTrialTimer);
    tmp = 0;
    TimeTrialWait = 0;
    /* timetrial_frame last: it fills the beqz delay slot in retail. */
    timetrial_frame = 0;
    if (PLAYERCOUNT != 0) {
        tmp = !plr_items;
    }
    clock_ok = tmp;
}

u16 TurnRot(u16 a, u16 b, s32 rate) {
    s32 d;

    if (b == a) {
        return b;
    }
    d = RotDiff(a, b);
    if (d > 0) {
        if (d <= rate) {
            return b;
        }
        return (a + rate) & 0xFFFF;
    }
    if (d >= -rate) {
        return b;
    }
    return (a - rate) & 0xFFFF;
}

inline void UpdateTimer(void *timer) {
    struct gtimer_s *t = timer;

    t->frame++;
    /* PAL: 300/50 = 6 per frame (GC 60Hz used 5). */
    t->itime += 6;
    t->isec += 6;
    if (t->isec > 299) {
        t->isec -= 0x12C;
    }
    t->ftime += 0.02f;
    t->fsec += 0.02f;
    if (t->fsec >= 1.0f) {
        t->fsec -= 1.0f;
    }
}

extern void KillItem(void *p);

/* gnu89 plain inline: standalone AND inlined into StartTimeTrial. */
inline void ClockOff(void) {
    struct creatobj_s *base = Character;
    struct creatobj_s *end = base + 9;
    struct creatobj_s *c;
    char *o;

    /* pointer do-while + (s32) cast: gives retail's signed `slt`, no loop-entry
     * guard, and two independent 0xCE4-stride IVs (creature base + &obj).
     * `base` is used at three distinct offsets so gcc keeps the bare symbol
     * address in a temp and derives s0/s1/s2 from it, as retail does. */
    c = base + 1;
    o = (char *)base + 0xCE8; /* &Character[1].obj */
    do {
        if (c->on != 0 && c->character == 0x76) {
            KillItem(o);
        }
        c++;
        o += 0xCE4;
    } while ((s32)c < (s32)end);
    clock_ok = 0;
}

/* signed: MakeTimeI clamps with `slt $v0,-1,$a0` + movn */
extern void MakeTimeI(s32 itime, s32 hours, char *buf);
extern char D_00630D00[]; /* "%s  %s" */
extern char D_00630D08[]; /* "%s %s"  */

void MakeLevelTimeString(struct time_s *time, char *txt) {
    char *fmt;
    char time_string[64];

    MakeTimeI(time->itime, 0, time_string);
    if (Game.language == 'c') {
        fmt = D_00630D00;
    } else {
        fmt = D_00630D08;
    }
    sprintf(txt, fmt, time, time_string);
}


void MakeTimeF(float time, s32 hours, char *txt) {
    MakeTimeI(time * 300.0f, hours, txt);
}

struct animpkt_s {
    float time; /* 0x0 */
    char pad_4[0xC - 0x4];
    u16 action;    /* 0xC */
    u16 oldaction; /* 0xE */
    u16 newaction; /* 0x10 */
    char pad_12[0x1B - 0x12];
    u8 flags; /* 0x1B */
};

/* transporter object: offsets verified in FinishTransporter / StartTransporter */
struct tobj_s {
    char pad_8[0x8];
    struct mask_s *mask; /* 0x8 (LoseMask) */
    char pad_c[0x14 - 0xC];
    struct animpkt_s anim; /* 0x14 (ToggleVehicle) */
    char pad_30[0x32 - 0x30];
    short vehicle; /* 0x32 (ToggleVehicle) */
    u32 flags; /* 0x34 (UpdateKabooms) */
    char pad_38[0x68 - 0x38];
    struct nuvec_s pos; /* 0x68 */
    struct nuvec_s mom;    /* 0x74 */
    struct nuvec_s oldpos; /* 0x80 */
    char pad_8c[0xE4 - 0x8C];
    float RADIUS; /* 0xE4 */
    char pad_e8[0x100 - 0xE8];
    float old_SCALE; /* 0x100 */
    float SCALE;     /* 0x104 */
    float radius; /* 0x108 */
    float scale;  /* 0x10C */
    /* objbot/objtop/dead verified in PlayerObjectAnimCollision */
    float objbot; /* 0x110 */
    float objtop; /* 0x114 */
    float bot;    /* 0x118 */
    float top;    /* 0x11C */
    char pad_120[0x145 - 0x120];
    signed char dead; /* 0x145 */
    char pad_146[0x14A - 0x146];
    u16 kaboom_mask; /* 0x14A (UpdateKabooms) */
    char pad_14c[0x14F - 0x14C];
    signed char finished; /* 0x14F (CheckFinish) */
    char pad_150[0x166 - 0x150];
    short dyrot; /* 0x166 */
    char pad_168[0x16A - 0x168];
    short hdg; /* 0x16A */
    char pad_16c[0x172 - 0x16C];
    signed char invincible; /* 0x172 (UpdateKabooms) */
    char pad_173[0x17A - 0x173];
    u8 f17A; /* 0x17A (LoseMask; unsigned -- retail li 0x96, not -0x6A) */
    char pad_17b[0x185 - 0x17B];
    u8 f185;     /* 0x185 (ToggleVehicle) */
    u8 exploded; /* 0x186 (UpdateKabooms) */
    char pad_187[0x230 - 0x187];
    struct numtx_s mtx; /* 0x230 (ToggleVehicle) */
};

void FinishTransporter(struct tobj_s *cyl, struct tobj_s *obj) {
    cyl->oldpos = cyl->pos;
    cyl->mom = v000;
    cyl->dyrot = 0;
    obj->pos.y = obj->oldpos.y =
        (cyl->pos.y + cyl->top * cyl->SCALE) - obj->bot * obj->SCALE;
}

/* gnu89 plain inline: standalone AND inlined into ResetDeath (retail has no
 * jal there -- the whole field-init sequence is expanded in place). */
inline void StartTransporter(struct tobj_s *cyl, struct nuvec_s *pos) {
    cyl->oldpos = *pos;
    cyl->pos = cyl->oldpos;
    cyl->mom = v000;
    /* the two (0.5f,1.0f) groups emit swapped -- pre-swap them in source */
    cyl->radius = 0.5f;
    cyl->scale = 1.0f;
    cyl->SCALE = 1.0f;
    cyl->RADIUS = 0.5f;
    cyl->old_SCALE = 1.0f;
    cyl->top = 0.0f;
    cyl->bot = -1.0f;
    cyl->hdg = 0;
}

/* Rail entry stride 0x28; type (+0x26) verified in AheadOfCheckpoint,
 * spline (+0x0) in ResetDeath (D_00586478 == &Rail[6]) */
struct rail_s {
    struct nugspline_s *spline; /* 0x0 */
    char pad_4[0x26 - 0x4];
    signed char type; /* 0x26 */
    char pad_27[0x28 - 0x27];
};
extern struct rail_s Rail[];
extern s32 cp_iRAIL;
extern s32 cp_iALONG;
extern float cp_fALONG;

s32 AheadOfCheckpoint(s32 iRAIL, s32 iALONG, float fALONG) {
    if (cp_iRAIL == -1) {
        return 1;
    }
    if (cp_iALONG == -1) {
        return 1;
    }
    if (iRAIL == -1) {
        return 1;
    }
    if (iALONG == -1) {
        return 1;
    }
    /* (Rail+i)->type, not Rail[i].type: steers the addu accumulator to $v1 */
    if ((Rail + iRAIL)->type != 0) {
        return 1;
    }
    if (cp_iRAIL < iRAIL) {
        return 1;
    }
    if (iRAIL < cp_iRAIL) {
        return 0;
    }
    if (cp_iALONG < iALONG) {
        return 1;
    }
    if (iALONG < cp_iALONG) {
        return 0;
    }
    if (cp_fALONG < fALONG) {
        return 1;
    }
    return 0;
}

extern float NuVecMag(struct nuvec_s *v);
extern s32 NewRayCast(struct nuvec_s *pos, struct nuvec_s *ray, float f);

/* gnu89 plain inline: standalone AND inlined into AddFlyingWumpa. */
inline void WumpaHitTerrain(struct wumpa_s *wmp) {
    float f;
    struct nuvec_s ray;

    /* PAL: 50.0f (GC 60Hz used 60.0f) */
    f = wmp->field_3C * 50.0f;
    ray.x = wmp->mom.x * f;
    ray.y = wmp->mom.y * f;
    ray.z = wmp->mom.z * f;
    f = NuVecMag(&ray);
    if (NewRayCast(&wmp->pos, &ray, 0.0f) != 0) {
        wmp->field_3C *= (NuVecMag(&ray) / f);
    }
}

void ResetGemPath(void) {
    u8 bits;
    s32 open;

    /* GemPath first: gempath_begun is then the last store before the switch, so
     * it fills the range-check's beqz delay slot (GC has these reversed). */
    GemPath = 0;
    gempath_begun = 0;
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
        /* `^ 1` not `1 - x`: open comes from an sltu so gcc knows it is 0/1 and
         * folds the bitfield's `& 1` away, giving retail's single xori. */
        ObjTab[101].obj.special->instance->flags.visible = gempath_open ^ 1;
    }
}

/* nugspline_s: len (+0x0) / ptsize (+0x2) / pts (+0x8) verified in
 * SplineDistance / NearestSplinePoint / SplinePointAngle */
struct nugspline_s {
    short len;    /* 0x0 */
    short ptsize; /* 0x2 */
    char pad_4[4];
    char *pts; /* 0x8 */
};

extern void NuVecSub(struct nuvec_s *d, struct nuvec_s *a, struct nuvec_s *b);
extern float NuVecDistSqr(struct nuvec_s *a, struct nuvec_s *b, struct nuvec_s *c);
extern s32 NuAtan2D(float x, float z);

s32 NearestSplinePoint(struct nuvec_s *pos, struct nugspline_s *spl) {
    s32 index;
    s32 i;
    s32 d;
    s32 d0;

    index = -1;
    if (spl != NULL) {
        for (i = 0; i < spl->len; i++) {
            d = NuVecDistSqr(pos, (struct nuvec_s *)(spl->pts + i * spl->ptsize),
                             NULL);
            if (index == -1 || d < d0) {
                index = i;
                d0 = d;
            }
        }
    }
    return index;
}

float SplineDistance(struct nugspline_s *spl) {
    struct nuvec_s *p0;
    struct nuvec_s *p1;
    struct nuvec_s v;
    s32 i;
    float d;

    if (spl == NULL || spl->len < 2) {
        return 0.0f;
    }
    p0 = (struct nuvec_s *)spl->pts;
    d = 0.0f;
    for (i = 0; i < spl->len - 1; i++) {
        p1 = (struct nuvec_s *)(spl->pts + (i + 1) * spl->ptsize);
        NuVecSub(&v, p1, p0);
        d += NuVecMag(&v);
        p0 = p1;
    }
    return d;
}

u16 SplinePointAngle(struct nugspline_s *spl, s32 i) {
    struct nuvec_s *p0;
    struct nuvec_s *p1;
    float dx;
    float dz;

    p0 = (struct nuvec_s *)(spl->pts + i * spl->ptsize);
    dx = dz = 0.0f;
    if (i > 0) {
        p1 = (struct nuvec_s *)(spl->pts + (i - 1) * spl->ptsize);
        /* the explicit `+ 0.0f` is real: retail adds the zeroed dx/dz here */
        dx = (p0->x - p1->x) + 0.0f;
        dz = (p0->z - p1->z) + 0.0f;
    }
    if (i < spl->len - 1) {
        p1 = (struct nuvec_s *)(spl->pts + (i + 1) * spl->ptsize);
        dx = dx + (p1->x - p0->x);
        dz = dz + (p1->z - p0->z);
    }
    return NuAtan2D(dx, dz);
}

s32 LineCrossed(float xold, float zold, float xnew, float znew, float x0,
                float z0, float x1, float z1) {
    s32 crossed;

    if (!((xnew - x0) * (z1 - z0) + (znew - z0) * (x0 - x1) >= 0.0f) &&
        (xold - x0) * (z1 - z0) + (zold - z0) * (x0 - x1) >= 0.0f) {
        crossed = 1;
        if ((xnew - xold) * (z0 - zold) + (znew - zold) * (xold - x0) >= 0.0f &&
            (xnew - x1) * (zold - z1) + (znew - z1) * (x1 - xold) >= 0.0f) {
            crossed = 2;
        }
    } else {
        crossed = 0;
    }
    return crossed;
}

extern float NuFsqrt(float x);

void AnglesBetweenPoints(struct nuvec_s *from, struct nuvec_s *to, u16 *xrot,
                         u16 *yrot) {
    float dx;
    float dy;
    float dz;

    dx = to->x - from->x;
    dy = to->y - from->y;
    dz = to->z - from->z;
    *xrot = NuAtan2D(dy, NuFsqrt(dx * dx + dz * dz));
    *yrot = NuAtan2D(dx, dz);
}


u16 SplinePointTilt(struct nugspline_s *spl, s32 i) {
    struct nuvec_s *p0;
    struct nuvec_s *p1;
    float dx;
    float dz;
    float dy;
    float d;

    dx = 0.0f;
    dy = 0.0f;
    dz = 0.0f;
    p0 = (struct nuvec_s *)(spl->pts + (i * spl->ptsize));
    if (i > 0) {
        p1 = (struct nuvec_s *)(spl->pts + ((i - 1) * spl->ptsize));
        dx = (p0->x - p1->x) + 0.0f;
        dy = (p0->y - p1->y) + 0.0f;
        dz = (p0->z - p1->z) + 0.0f;
    }
    if (i < spl->len - 1) {
        p1 = (struct nuvec_s *)(spl->pts + ((i + 1) * spl->ptsize));
        dx += p1->x - p0->x;
        dy += p1->y - p0->y;
        dz += p1->z - p0->z;
    }
    d = NuFsqrt(dx * dx + dz * dz);
    return NuAtan2D(dy, d);
}

/* gnu89 plain inline: standalone AND inlined twice into RatioBetweenEdges. */
inline float DistanceToLine(struct nuvec_s *pos, struct nuvec_s *p0,
                            struct nuvec_s *p1) {
    u16 ang;
    double d;

    /* bind (-a & 0xFFFF) to a u16 local: retail masks first and reuses it for
     * both table indices. Inlining it lets gcc fold the inner mask away and
     * emit the +0x4000 before the andi. */
    ang = -NuAtan2D(p1->x - p0->x, p1->z - p0->z);
    /* soft-double: the float expression is promoted, then |d| is taken with
     * dpcmp/dpsub (retail inlines NuFabs's double form). */
    d = (pos->x - p0->x) * NuTrigTable[(u16)(ang + 0x4000)] +
        (pos->z - p0->z) * NuTrigTable[ang];
    if (d < 0.0) {
        d = 0.0 - d;
    }
    return d;
}

extern signed char CRemap[];
extern float D_0062D710; /* new-mask scale (PAL; GC used 0.8f) */

void NewMask(struct mask_s *mask, struct nuvec_s *pos) {
    if (CRemap[3] != -1 && (LBIT & 0x3E00000) == 0) {
        if (mask->active < 2) {
            if (mask->active == 0) {
                if (pos != NULL) {
                    mask->pos = *pos;
                }
                /* Faithful near-match (96.3%), kept as state=asm -- see
                 * blocker: gcc duplicates this D_0062D710 load into the
                 * `pos == NULL` branch delay slot (retail leaves a nop). */
                mask->character = 3;
                mask->scale = D_0062D710;
                ResetLights(mask->lights);
            }
            mask->active++;
        } else {
            /* PAL: 0x228 (GC 0x296); and a channel-forced GameSfx rather than
             * GC's GameMusic(0xa2, 0). */
            mask->active = 0x228;
            gamesfx_channel = 4;
            GameSfx(0xA2, 0);
        }
        GameSfx(0x3D, pos);
    }
}

extern char D_00630BE8[];
#define wumpa_file_fmt D_00630BE8
extern char LevelFileName[];
extern u8 Chase[];
extern s32 NuFileLoadBuffer(char *name, void *buf, s32 max);
extern s32 NuMemFileOpen(void *buf, s32 size, s32 mode);
extern s32 NuFileReadInt(s32 fh);
extern float NuFileReadFloat(s32 fh);
extern void NuFileClose(s32 fh);

void LoadWumpa(void) {
    s32 fh;
    s32 i;
    void *fbuff;
    s32 fsize;

    sprintf(tbuf, wumpa_file_fmt, LevelFileName);
    /* PS2 drops GC's `if (NuFileExists(tbuf))` guard */
    fbuff = Chase;
    fsize = NuFileLoadBuffer(tbuf, fbuff, 0x7FFFFFFF);
    fh = NuMemFileOpen(fbuff, fsize, 0);
    if (fh != 0) {
        WUMPACOUNT = NuFileReadInt(fh);
        if (0x100 < WUMPACOUNT) {
            WUMPACOUNT = 0x100;
        }
        for (i = 0; i < WUMPACOUNT; i++) {
            Wumpa[i].pos0.x = NuFileReadFloat(fh);
            Wumpa[i].pos0.y = NuFileReadFloat(fh);
            Wumpa[i].pos0.z = NuFileReadFloat(fh);
        }
        NuFileClose(fh);
    }
}

extern s32 NuFileOpen(char *name, s32 mode);
extern s32 NuFileWriteInt(s32 fh, s32 v);
extern s32 NuFileWriteFloat(s32 fh, float v);

void SaveWumpa(void) {
    s32 fh;
    s32 i;

    sprintf(tbuf, wumpa_file_fmt, LevelFileName);
    fh = NuFileOpen(tbuf, 1);
    if (fh != 0) {
        NuFileWriteInt(fh, WUMPACOUNT);
        for (i = 0; i < WUMPACOUNT; i++) {
            NuFileWriteFloat(fh, Wumpa[i].pos0.x);
            NuFileWriteFloat(fh, Wumpa[i].pos0.y);
            NuFileWriteFloat(fh, Wumpa[i].pos0.z);
        }
        NuFileClose(fh);
    }
}

struct gcam_s {
    struct numtx_s m; /* 0x0 */
};

extern struct gcam_s *pCam;
extern float D_0062D6D0; /* parallax farclip scale (GC 0.1f) */
extern void NuMtxSetScale(struct numtx_s *m, struct nuvec_s *s);
extern s32 NuRndrGScnObj(void *gobj, struct numtx_s *m);
extern void NuRndrClear(s32 mode, s32 col, float z);

void DrawParallax(void) {
    struct numtx_s m;
    struct nuvec_s s;
    s32 n;

    n = 0;
    s.x = s.y = s.z = pNuCam->farclip * D_0062D6D0 * 0.5f;
    NuMtxSetScale(&m, &s);
    m.m[3][0] = pCam->m.m[3][0];
    m.m[3][1] = pCam->m.m[3][1];
    m.m[3][2] = pCam->m.m[3][2];
    if (ObjTab[9].obj.special != NULL) {
        NuRndrGScnObj(
            ObjTab[9].obj.scene->gobjs[ObjTab[9].obj.special->instance->objid],
            &m);
        n++;
    }
    if (ObjTab[10].obj.special != NULL) {
        NuRndrGScnObj(
            ObjTab[10].obj.scene->gobjs[ObjTab[10].obj.special->instance->objid],
            &m);
        n++;
    }
    /* not in the GC reference: PS2 clears after drawing the parallax layers */
    if (n != 0) {
        NuRndrClear(2, 0, 1.0f);
    }
}

float RatioAlongLine(struct nuvec_s *pos, struct nuvec_s *p0,
                     struct nuvec_s *p1) {
    float z;
    float z1;
    float dx;
    float dz;
    float siny;
    float cosy;
    u16 yrot;

    dx = p1->x - p0->x;
    dz = p1->z - p0->z;
    yrot = -NuAtan2D(dx, dz);
    siny = NuTrigTable[yrot];
    cosy = NuTrigTable[(u16)(yrot + 0x4000)];
    z = -((pos->x - p0->x) * siny) + ((pos->z - p0->z) * cosy);
    if (z <= 0.0f) {
        return 0.0f;
    }
    z1 = -(dx * siny) + (dz * cosy);
    if (z >= z1) {
        return 1.0f;
    }
    return z / z1;
}

extern struct tobj_s death_obj;
extern s32 Death;
extern s32 death_begun;

/* &Rail[6]. Retail addresses this rail through its own symbol (%hi/%lo of
 * Rail+0xF0); indexing Rail[6] instead makes gcc CSE &Rail and reach the
 * fields at +0x116/+0xF0. */
extern struct rail_s D_00586478;
#define death_rail D_00586478

void ResetDeath(void) {
    Death = 0;
    death_begun = 0;
    if (death_rail.type != -1 && Level == 0x25 && Hub == 5 &&
        last_level != 0x26 && last_level != -1) {
        StartTransporter(&death_obj,
                         (struct nuvec_s *)(death_rail.spline->pts +
                                            (death_rail.spline->len - 1) *
                                                death_rail.spline->ptsize));
        Death = 2;
        death_begun = 1;
    }
}

/* SplTab entry stride 0x18; fields verified in InitSplineTable */
struct spltab_s {
    struct nugspline_s *spl; /* 0x0 */
    char *name;              /* 0x4 */
    short min;               /* 0x8 */
    short max;               /* 0xA */
    char pad_c[0x10 - 0xC];
    u64 levbits; /* 0x10 */
};
extern struct spltab_s SplTab[0x49];
/* array-form: a bare pointer scalar goes gp-rel under -G8 and overflows
 * GPREL16 (world_scene is far from $gp); retail uses absolute %hi/%lo. */
extern struct nugscn_s *world_scene[];
extern struct nugspline_s *NuSplineFind(struct nugscn_s *scn, char *name);

void InitSplineTable(void) {
    s32 i;

    if (world_scene[0] != NULL) {
        for (i = 0; i < 0x49; i++) {
            SplTab[i].spl = NULL;
            if (((SplTab[i].levbits >> Level) & 1) != 0) {
                SplTab[i].spl = NuSplineFind(world_scene[0], SplTab[i].name);
                if (SplTab[i].spl != NULL) {
                    if (SplTab[i].min > 0 && SplTab[i].spl->len < SplTab[i].min) {
                        SplTab[i].spl = NULL;
                    } else {
                        if (SplTab[i].max > 0 && SplTab[i].max >= SplTab[i].min &&
                            SplTab[i].spl->len > SplTab[i].max) {
                            SplTab[i].spl = NULL;
                        }
                    }
                }
            }
        }
    } else {
        for (i = 0; i < 0x49; i++) {
            SplTab[i].spl = NULL;
        }
    }
}

s32 PlayerObjectAnimCollision(struct tobj_s *obj, struct objspecial_s *special,
                              float radius) {
    float r;
    float dx;
    float dz;
    struct nuvec_s pos;

    if (obj != NULL && special != NULL && special->instance != NULL &&
        special->instance->flags.visible && obj->dead == 0) {
        if (special->instance->anim != NULL) {
            pos.x = special->instance->anim->mtx.m[3][0];
            pos.y = special->instance->anim->mtx.m[3][1];
            pos.z = special->instance->anim->mtx.m[3][2];
        } else {
            pos.x = special->instance->mtx.m[3][0];
            pos.y = special->instance->mtx.m[3][1];
            pos.z = special->instance->mtx.m[3][2];
        }
        if (!(obj->objbot > pos.y + radius) && !(obj->objtop < pos.y - radius)) {
            dx = pos.x - obj->pos.x;
            dz = pos.z - obj->pos.z;
            r = obj->radius + radius;
            if (dx * dx + dz * dz > r * r) {
                return 0;
            }
            return 1;
        }
    }
    return 0;
}

extern float SAFEY;
extern struct nuvec_s start_pos;

void InitPositions(void) {
    struct nuvec_s *p0;
    struct nuvec_s *p1;

    pos_START = NULL;
    SAFEY = -1000.0f;
    if (SplTab[0].spl != NULL && 0 < SplTab[0].spl->len) {
        pos_START = (struct nuvec_s *)SplTab[0].spl->pts;
    }
    if (Level == 0x1c && SplTab[29].spl != NULL && SplTab[45].spl != NULL) {
        p0 = (struct nuvec_s *)SplTab[29].spl->pts;
        p1 = (struct nuvec_s *)SplTab[45].spl->pts;
        start_pos.x = (p0->x + p1->x) * 0.5f;
        start_pos.y = (p0->y + p1->y) * 0.5f;
        start_pos.z = (p0->z + p1->z) * 0.5f;
        pos_START = &start_pos;
        return;
    }
    if (LDATA->vSTART.x != 0.0f || LDATA->vSTART.y != 0.0f ||
        LDATA->vSTART.z != 0.0f) {
        pos_START = &LDATA->vSTART;
    }
}

/* rpos_s: iRAIL (+0x0) / iALONG (+0x2) / fALONG (+0x8) verified in WipeWumpa */
struct rpos_s {
    signed char iRAIL; /* 0x0 */
    char pad_1;
    short iALONG; /* 0x2 */
    char pad_4[4];
    float fALONG; /* 0x8 */
    float fACROSS; /* 0xC */
    u16 angle; /* 0x10 */
};

extern s32 FurtherALONG(s32 iRAIL0, s32 iALONG0, float fALONG0, s32 iRAIL1,
                        s32 iALONG1, float fALONG1);
extern float D_0062D340; /* wipe-fly speed (FlyWumpa itself uses D_0062D700) */

s32 WipeWumpa(struct rpos_s *rpos) {
    s32 i;
    struct wumpa_s *wumpa;

    if (TimeTrial != 0) {
        return 0;
    }
    wumpa = Wumpa;
    for (i = 0; i < 0x140; i++) {
        if (((u8)wumpa->field_40 == 1 || (u8)wumpa->field_40 == 2) &&
            FurtherALONG(rpos->iRAIL, rpos->iALONG, rpos->fALONG,
                         wumpa->field_41, wumpa->field_42,
                         wumpa->field_44) != 0) {
            /* FlyWumpa's body, copied in source (retail reads D_0062D340 here,
             * not FlyWumpa's D_0062D700, so it cannot be that function inlined) */
            wumpa->mom.x = 0.0f;
            wumpa->mom.y = 0.0f;
            wumpa->mom.z = D_0062D340;
            NuVecRotateX(&wumpa->mom, &wumpa->mom, -0x400);
            NuVecRotateY(&wumpa->mom, &wumpa->mom, qrand());
            wumpa->field_38 = 0;
            wumpa->field_40 = 3;
            wumpa->field_3C = 2.0f;
            wumpa->field_48 = 0;
            wumpa->field_49 = 0;
            GameSfx(0x2A, &wumpa->pos);
            return 1;
        }
        wumpa++;
    }
    return 0;
}

/* Font3DObjTab entry stride 0xC; i (+0x0) / flags (+0x2) verified in
 * InitObjectTable */
struct font3dobj_s {
    short i;  /* 0x0 */
    u8 flags; /* 0x2 */
    char pad_3[0xC - 0x3];
};
extern struct font3dobj_s Font3DObjTab[0x1a];
extern s32 NuSpecialFind(struct nugscn_s *scn, struct objinfo_s *obj,
                         char *name);

void InitObjectTable(void) {
    s32 i;
    s32 j;

    for (i = 0; i < 0xc9; i++) {
        ObjTab[i].obj.special = NULL;
        if (((ObjTab[i].levbits >> Level) & 1) != 0 &&
            *ObjTab[i].scene != NULL) {
            if (NuSpecialFind(*ObjTab[i].scene, &ObjTab[i].obj,
                              ObjTab[i].name) != 0) {
                ObjTab[i].obj.special->instance->flags.visible =
                    ObjTab[i].visible;
                for (j = 0; j < 0x1a; j++) {
                    if ((Font3DObjTab[j].flags & 2) != 0 &&
                        Font3DObjTab[j].i == i) {
                        ObjTab[i].font3d_letter = (char)j + 0x61;
                        j = 0x1a;
                    }
                }
            }
        }
    }
}

/* stride 0x34; the leading float is the shadow radius (DrawMaskFeathers) */
struct shadinfo_s {
    float f0; /* 0x0 */
    char pad_4[0x34 - 0x4];
};
extern struct shadinfo_s D_0055FC88[];
extern float D_0062D3B8;
extern void NuHGobjEvalAnim(void *hobj, void *anm, float t, s32 a, void *b,
                            struct numtx_s *mtx);
extern void NuHGobjRndrMtx(void *hobj, void *m, s32 a, void *b,
                           struct numtx_s *mtx);
extern void ShadRndr(void *a, void *b, float t, float r);

void DrawMaskFeathers(void) {
    struct cmodel_s *model;
    struct mfeathers_s *feathers;
    s32 i;
    float dead;
    struct numtx_s tmtx[0x100];

    i = CRemap[11];
    if (i != -1) {
        model = &CModel[i];
        if (model->anmdata[0xe] != NULL) {
            /* explicit loop-invariant local: retail keeps this in the
             * callee-saved $f20 across the two calls (freg_mask 0x00100000). */
            dead = D_0062D3B8;
            feathers = &MaskFeathers[0];
            for (i = 0; i < 4; i++) {
                if (feathers->time < feathers->duration) {
                    NuHGobjEvalAnim(model->hobj, model->anmdata[0xe],
                                    feathers->time, 0, NULL, tmtx);
                    NuHGobjRndrMtx(model->hobj, feathers, 1, NULL, tmtx);
                    /* not in the GC reference: PS2 also renders the shadow */
                    if (feathers->shadow != dead && model->shadow != NULL) {
                        ShadRndr(&feathers->mS, model->shadow_model,
                                 feathers->time,
                                 D_0055FC88[model->character].f0);
                    }
                }
                feathers++;
            }
        }
    }
}

extern s32 temp_hours;
extern s32 temp_minutes;
extern s32 temp_seconds;
extern s32 temp_hundredths;
extern s32 temp_tenths;
extern char D_0061DFD0[]; /* "%i:%c%c.%c%c" */

void MakeTimeI(s32 time, s32 hours, char *txt) {
    if (time < 0) {
        time = 0;
    }
    time /= 3;
    if (hours != 0) {
        temp_hours = time / 360000;
        temp_minutes = (time / 6000) % 60;
    } else {
        temp_minutes = time / 6000;
    }
    temp_seconds = (time / 100) % 60;
    temp_hundredths = time % 100;
    temp_tenths = temp_hundredths / 10;
    if (txt != NULL) {
        sprintf(txt, D_0061DFD0, temp_minutes, temp_seconds / 10 + 0x30,
                temp_seconds % 10 + 0x30, temp_tenths + 0x30,
                temp_hundredths % 10 + 0x30);
        if (Game.language == 'c') {
            AddSpacesIntoText(txt, 1);
        }
    }
}

extern char *PlaceName3[3][6]; /* [place][language]; row stride 0x18 */

void DefaultTimeTrialNames(s32 all) {
    char *txt;
    s32 i;
    s32 j;

    for (i = 0; i < 0x23; i++) {
        for (j = 0; j < 3; j++) {
            txt = Game.level[i].time[j].name;
            if (all != 0) {
                NuStrCpy(txt, PlaceName3[j][Game.language]);
            } else if (*txt == '1') {
                NuStrCpy(txt, PlaceName3[0][Game.language]);
            } else if (*txt == '2') {
                NuStrCpy(txt, PlaceName3[1][Game.language]);
            } else if (*txt == '3') {
                NuStrCpy(txt, PlaceName3[2][Game.language]);
            }
        }
    }
}

extern float DISCOXOFFSET;
extern void NuMtxSetIdentity(struct numtx_s *m);
extern void NuMtxRotateY(struct numtx_s *m, s32 ang);
extern void EvalModelAnim(struct cmodel_s *model, void *anm, struct numtx_s *m,
                          struct numtx_s *tmtx, float ***dwa,
                          struct numtx_s *loc);
extern void NuHGobjRndrMtxDwa(void *hobj, struct numtx_s *m, s32 nlayers,
                              short *layer, struct numtx_s *tmtx, float **dwa);

void DrawTempCharacter2(s32 render) {
    struct cmodel_s *model;
    struct numtx_s m;
    struct numtx_s tmtx[0x100];
    struct numtx_s mtxLOCATOR[16];
    short layertab[2] = {0, 1};
    float **dwa;
    short *layer;
    s32 nlayers;
    s32 i;

    layer = layertab;
    if (temp_character2 != -1) {
        i = CRemap[temp_character2];
        if (i != -1) {
            nlayers = 1;
            model = &CModel[i];
            if (model->character == 0) {
                nlayers = 2;
            }
            NuMtxSetIdentity(&m);
            if (Level == 0x2b) {
                if (model->character == 0xbb) {
                    NuMtxRotateY(&m, 0x2000);
                    m.m[3][0] = DISCOXOFFSET;
                } else if (model->character == 0) {
                    NuMtxRotateY(&m, -0x2000);
                    m.m[3][0] = -DISCOXOFFSET;
                }
            }
            EvalModelAnim(model, TempAnim2, &m, tmtx, &dwa, mtxLOCATOR);
            if (render != 0) {
                NuHGobjRndrMtxDwa(model->hobj, &m, nlayers, layer, tmtx, dwa);
            }
        }
    }
}

/* Case order below follows jtbl_0061D900's block layout (address order), not
 * numeric order -- gcc emits the blocks in source order. Menu->block mapping
 * decoded from asm/data/rodata.rodata.s:19394. The GC reference is unusable
 * here: it calls ParseNintendoErrorCode (absent on PS2) and several y_max
 * values differ. */
void GetMenuInfo(struct cursor_s *cur) {
    cur->x_min = 0;
    cur->y_min = 0;
    cur->x_max = 0;
    cur->y_max = 0;
    switch (cur->menu) {
    case 0:
        cur->y_max = (LANGUAGEOPTION != 0) ? 2 : 1;
        return;
    case 3:
        cur->y_max = (TimeTrial != 0) ? 3 : 2;
        return;
    case 4:
        cur->y_max = 3;
        if (LANGUAGEOPTION != 0) {
            cur->y_max = 4;
        }
        return;
    case 5:
    case 19:
        cur->y_max = 3;
        return;
    case 6:
        cur->y_max = 2;
        return;
    case 7:
        cur->y_max = 5;
        return;
    case 9:
        cur->y_max = 0xA;
        return;
    case 13:
        cur->y_max = 4;
        return;
    case 8:
    case 14:
    case 16:
    case 18:
        cur->y_max = 1;
        return;
    case 10:
        cur->y_max = 0xA;
        return;
    case 12:
        cur->y_max = 5;
        cur->x_max = 5;
        return;
    case 2:
        cur->x_max = 6;
        cur->y_max = 5;
        if (Game.language == 0x63) {
            cur->y_max = 8;
        }
        return;
    case 15:
        cur->x_max = 6;
        cur->y_max = 4;
        if (Game.language == 0x63) {
            cur->y_max = 7;
        }
        return;
    case 20:
    case 23:
    case 26:
        cur->x_max = 1;
        cur->y_max = 2;
        return;
    case 21:
    case 24:
    case 27:
    case 29:
        cur->x_max = 0;
        cur->y_max = 1;
        return;
    case 22:
    case 25:
    case 28:
    case 30:
        cur->y_max = 0;
        cur->x_max = 0;
        return;
    }
}

extern s32 i_tempwumpa;
extern float D_0062D704; /* flying-wumpa shadow y (GC 2000000.0f) */
extern float D_0062D708; /* flying-wumpa mom scale (GC 0.1666667f) */
extern void NuVecScale(float s, struct nuvec_s *d, struct nuvec_s *v);

void AddFlyingWumpa(struct nuvec_s *src, struct nuvec_s *dir,
                    struct nuvec_s *dst, s32 destroy) {
    struct wumpa_s *wumpa;

    /* &Wumpa[i_tempwumpa + 0x100]: single reference, so gcc folds the +0x4C00
     * into the reloc (retail's D_00590170 == &Wumpa[0x100]). */
    wumpa = &Wumpa[i_tempwumpa + 0x100];
    wumpa->pos1 = *src;
    wumpa->pos0 = wumpa->pos1;
    wumpa->pos = wumpa->pos0;
    wumpa->shadow = D_0062D704;
    wumpa->field_40 = 3;
    wumpa->field_38 = 0;
    wumpa->field_3C = 2.0f;
    wumpa->surface_type = -1;
    NuVecScale(D_0062D708, &wumpa->mom, dir);
    WumpaHitTerrain(wumpa);
    if (destroy == 3) {
        destroy = 2;
        wumpa->field_49 = 2;
    } else {
        wumpa->field_49 = 1;
    }
    wumpa->field_48 = destroy;
    i_tempwumpa++;
    if (i_tempwumpa == 0x40) {
        i_tempwumpa = 0;
    }
}

extern struct nuvec_s TTScrPos;
extern float TEXTZMUL;
extern float D_0062D5C8;
extern float D_0062D5CC;
extern float D_0062D5D0;
extern float D_0062D5D4;
extern float tt_sx;
extern float tt_sy;
extern void NuCameraTransformScreenClip(struct nuvec_s *dst, struct nuvec_s *src,
                                        s32 n, s32 flag);

void StartTimeTrial(struct nuvec_s *pos, s32 clock) {
    TimeTrial = 1;
    ResetCheckpoint(-1, -1, 0.0f, NULL);
    GameSfx(0x4C, NULL);
    if (clock != 0) {
        ClockOff();
    }
    NuCameraTransformScreenClip(&TTScrPos, pos, 1, 0);
    TTScrPos.z = (1.0f - TTScrPos.z) * (TEXTZMUL / D_0062D5C8);
    if (ObjTab[101].obj.special != NULL) {
        ObjTab[101].obj.special->instance->flags.visible = 1;
    }
    if (Level == 0x1D) {
        tt_sx = D_0062D5CC;
    } else if (LDATA->flags & 0x200) {
        tt_sx = 0.0f;
    } else if (Level == 0x1C) {
        tt_sx = 0.0f;
    } else {
        tt_sx = D_0062D5D0;
    }
    tt_sy = D_0062D5D4;
}

float RatioBetweenEdges(struct nuvec_s *pos, struct nuvec_s *p0,
                        struct nuvec_s *p1, struct nuvec_s *p2,
                        struct nuvec_s *p3) {
    float d1;
    float d2;

    d1 = DistanceToLine(pos, p0, p1);
    d2 = DistanceToLine(pos, p2, p3);
    return d1 / (d1 + d2);
}

extern void NuMtxTranslate(struct numtx_s *m, struct nuvec_s *v);
/* retail-owned const vectors: {5,5,5} scale and {0,11,6.25} translate.
 * Copied into ONE stack slot -- two initialised locals would take two. */
extern struct nuvec_s D_0061D468;
extern struct nuvec_s D_0061D478;

void DrawTempCharacter(s32 render) {
    struct cmodel_s *model;
    struct numtx_s m;
    struct numtx_s tmtx[0x100];
    struct numtx_s mtxLOCATOR[16];
    struct nuvec_s v;
    float **dwa;
    s32 i;

    if (temp_character == -1 || temp_character == 0x62 ||
        temp_character == 0xB9 || temp_character == 0xB8 ||
        temp_character == 0xBA || temp_character == 0xBC) {
        return;
    }
    i = CRemap[temp_character];
    if (i == -1) {
        return;
    }
    model = &CModel[i];
    if (Level == 0x25) {
        v = D_0061D468;
        NuMtxSetScale(&m, &v);
        v = D_0061D478;
        NuMtxTranslate(&m, &v);
    } else {
        NuMtxSetIdentity(&m);
    }
    if (Level == 0x2b) {
        if (model->character == 0xbb) {
            NuMtxRotateY(&m, 0x2000);
            m.m[3][0] = DISCOXOFFSET;
        } else if (model->character == 0) {
            NuMtxRotateY(&m, -0x2000);
            m.m[3][0] = -DISCOXOFFSET;
        }
    }
    EvalModelAnim(model, TempAnim, &m, tmtx, &dwa, mtxLOCATOR);
    if (render != 0) {
        NuHGobjRndrMtxDwa(model->hobj, &m, 1, NULL, tmtx, dwa);
    }
}

extern s32 i_maskfeathers;

/* gnu89 plain inline: standalone AND inlined into LoseMask. */
inline void AddMaskFeathers(struct mask_s *mask) {
    struct mfeathers_s *feathers;
    s32 i;

    feathers = &MaskFeathers[i_maskfeathers];
    feathers->mM = mask->mM;
    feathers->mS = mask->mS;
    i = CRemap[11];
    feathers->time = 0.0f;
    feathers->shadow = mask->shadow;
    if (i != -1 && CModel[i].anmdata[0xe] != NULL) {
        feathers->duration = CModel[i].anmdata[0xe]->time - 1.0f;
    } else {
        feathers->duration = 30.0f;
    }
    i_maskfeathers++;
    if (i_maskfeathers != 4) {
        return;
    }
    i_maskfeathers = 0;
}

extern s32 i_award;

s32 AddAward(s32 hub, s32 level, u16 got) {
    struct award_s *award;
    struct nugspline_s *spl;
    struct nuvec_s *p0;
    struct nuvec_s *p1;
    s32 i;
    s32 ang;

    if (hub == -1 || level == -1 || HData[hub].i_spl[1] == -1) {
        return 0;
    }
    spl = SplTab[HData[hub].i_spl[1]].spl;
    if (spl == NULL) {
        return 0;
    }
    HubFromLevel(level);
    i = temp_hublevel;
    if (i == -1) {
        return 0;
    }
    if ((got & 7) != 0) {
        ang = 2;
    } else if ((got & 8) != 0) {
        ang = 1;
    } else if ((got & 0x10) != 0) {
        ang = 0;
    } else {
        ang = 3;
    }
    award = &Award[i_award];
    i = i * 8 + ang * 2;
    p0 = (struct nuvec_s *)&spl->pts[i * spl->ptsize];
    p1 = (struct nuvec_s *)&spl->pts[(i + 1) * spl->ptsize];
    award->time = 0.0f;
    award->yrot = NuAtan2D(p1->x - p0->x, p1->z - p0->z);
    award->got = got;
    award->level = level;
    award->newpos = *p0;
    award->wait = 1;
    i_award++;
    if (i_award == 3) {
        i_award = 0;
    }
    return 1;
}

extern s32 GDeb[];
extern void AddFiniteShotDebrisEffect(s32 *key, s32 i, struct nuvec_s *pos,
                                      s32 n);

s32 HitWumpa(struct tobj_s *obj, s32 destroy) {
    float objtop;
    float objbot;
    float dx;
    float dz;
    float r2;
    struct wumpa_s *wumpa;
    s32 i;
    s32 key;

    if (TimeTrial == 0) {
        objtop = obj->pos.y + obj->top * obj->SCALE;
        objbot = obj->pos.y + obj->bot * obj->SCALE;
        wumpa = Wumpa;
        r2 = obj->radius + 0.1f;
        r2 *= r2;
        for (i = 0; i < 0x140; i++, wumpa++) {
            if ((u8)wumpa->field_40 == 1 || (u8)wumpa->field_40 == 2) {
                if (objtop < wumpa->pos.y - 0.1f ||
                    objbot > wumpa->pos.y + 0.1f) {
                    continue;
                }
                dx = (wumpa->pos.x - obj->pos.x) * (wumpa->pos.x - obj->pos.x);
                dz = (wumpa->pos.z - obj->pos.z) * (wumpa->pos.z - obj->pos.z);
                if (dx + dz < r2) {
                    if (destroy != 0) {
                        key = -1;
                        AddFiniteShotDebrisEffect(&key, GDeb[520], &wumpa->pos,
                                                  1);
                        wumpa->field_40 = 0;
                    } else {
                        wumpa->mom.x = 0.0f;
                        wumpa->mom.y = 0.0f;
                        wumpa->mom.z = 0.19999999f;
                        NuVecRotateX(&wumpa->mom, &wumpa->mom, -0x400);
                        NuVecRotateY(&wumpa->mom, &wumpa->mom, qrand());
                        wumpa->field_38 = 0;
                        wumpa->field_40 = 3;
                        wumpa->field_3C = 2.0f;
                        wumpa->field_48 = 0;
                        wumpa->field_49 = 0;
                        GameSfx(0x2A, &wumpa->pos);
                    }
                    return 1;
                }
            }
        }
    }
    return 0;
}

extern struct gtimer_s GameTimer;
extern struct gtimer_s PauseTimer;
extern u32 finish_frame;
extern s32 pausestats_frame;
extern float start_time;
extern struct nupad_s *Pad[];
extern struct plr_lives_s plr_wumpas;
extern s32 force_panel_wumpa_update;
extern s32 force_panel_crate_update;
extern s32 force_panel_lives_update;
extern s32 force_panel_items_update;
extern struct plr_lives_s plr_crates;
extern float check_time;
extern float check_duration;
extern float point_time;
extern float point_duration;
extern s32 FlyingLevelExtro;
extern s32 bonus_frame;
extern u16 PANELWUMPAYROT;
extern u16 PANELCRATEYROT;
extern u16 panel_head_xrot;
extern u16 panel_head_yrot;
extern s32 tt_flash;
extern void UpdatePanelItem(struct plr_lives_s *item, s32 reset, s32 snap_allowed);
extern void Update3DFontObjects(void);
extern void GameAudioUpdate(void);

void GameTiming(void) {
    s32 sfx;
    s32 flash;

    if (pause_dir == 1) {
        Paused++;
        if (Paused == 0x19) {
            pause_dir = 0;
        }
    } else if (pause_dir == 2) {
        Paused--;
        if (Paused == 0) {
            pause_dir = 0;
            NewMenu((struct cursor_s *)Cursor, -1, -1, -1);
            ResumeGame();
        }
    }
    if (Paused != 0) {
        if (Paused == 0x19) {
            UpdateTimer(&PauseTimer);
            pausestats_frame++;
        }
    } else {
        UpdateTimer(&GameTimer);
        if (force_panel_wumpa_update != 0) {
            force_panel_wumpa_update--;
        }
        if (force_panel_crate_update != 0) {
            force_panel_crate_update--;
        }
        if (force_panel_lives_update != 0) {
            force_panel_lives_update--;
        }
        if (force_panel_items_update != 0) {
            force_panel_items_update--;
        }
        if (Pad[0] != NULL && (Pad[0]->paddata & 0x10) != 0 && GameMode != 1) {
            force_panel_wumpa_update = 50;
            if (TimeTrial == 0 && LDATA->hub != -1) {
                force_panel_crate_update = 50;
            }
            force_panel_lives_update = 50;
            force_panel_items_update = 50;
        }
        UpdatePanelItem(&plr_wumpas, force_panel_wumpa_update, 1);
        UpdatePanelItem(&plr_crates, force_panel_crate_update, 0);
        UpdatePanelItem(&plr_lives, force_panel_lives_update, 1);
        UpdatePanelItem(&plr_crystal, force_panel_items_update, 1);
        UpdatePanelItem(&plr_crategem, force_panel_items_update, 1);
        UpdatePanelItem(&plr_bonusgem, force_panel_items_update, 1);
        if (check_time < check_duration) {
            check_time += 0.02f;
            if (check_time >= check_duration) {
                check_time = check_duration;
                point_time = 0.0f;
            }
        }
        if (point_time < point_duration) {
            point_time += 0.02f;
            if (point_time >= point_duration) {
                point_time = point_duration;
            }
        }
        if (start_time > 0.0f) {
            start_time -= 0.02f;
            if (start_time < 0.0f) {
                start_time = 0.0f;
            }
        }
        if (TimeTrial != 0) {
            if (TimeTrialWait > 0.0f) {
                if (timetrial_frame % 12 == 0) {
                    GameSfx(0x4E, NULL);
                }
                TimeTrialWait -= 0.02f;
                if (TimeTrialWait < 0.0f) {
                    TimeTrialWait = 0.0f;
                }
            } else if (player->obj.finished == 0 && FlyingLevelExtro == 0) {
                UpdateTimer(&TimeTrialTimer);
            }
            timetrial_frame++;
        }
        if (Bonus == 2) {
            bonus_frame++;
        }
        PANELWUMPAYROT -= 0x7C1;
        PANELCRATEYROT -= 0x28F;
        panel_head_xrot = 0;
        panel_head_yrot = 0;
        if (Level != 0x1A ? GameTimer.frame == 0x96 : GameTimer.frame == 0x19) {
            sfx = -1;
            if (Level == 0xD || Level == 0x1A) {
                sfx = 0xC7;
            }
            if (sfx != -1) {
                gamesfx_channel = 4;
                GameSfx(sfx + Game.language, NULL);
            }
        }
        flash = 0;
        if (TimeTrial != 0 && player->obj.finished == 1) {
            flash = (finish_frame % 50 > 36) ? 0 : player->obj.finished;
        }
        tt_flash = flash;
    }
    UpdateTimer(MenuTimer);
    UpdateTimer(GlobalTimer);
    Update3DFontObjects();
    GameAudioUpdate();
}

extern char D_00630D10[]; /* "___" */

void NewLevelTime(s32 t) {
    s32 i;
    s32 j;
    u32 *p;

    /* walking u32* over the .itime fields: retail's IV loads at +0 and steps
     * +8 (a struct time_s* would load at +4). */
    i = 0;
    p = &Game.level[Level].time[0].itime;
    while (*p != 0 && *p < t) {
        i++;
        if (i >= 3) {
            newleveltime_slot = i;
            return;
        }
        p += 2;
    }
    for (j = 2; j > i; j--) {
        Game.level[Level].time[j] = Game.level[Level].time[j - 1];
    }
    Game.level[Level].time[i].itime = t;
    NuStrCpy(Game.level[Level].time[i].name, D_00630D10);
    if (i < 2 && Game.level[Level].time[2].itime == 0) {
        NuStrCpy(Game.level[Level].time[2].name, PlaceName3[2][Game.language]);
    }
    if (i < 1 && Game.level[Level].time[1].itime == 0) {
        NuStrCpy(Game.level[Level].time[1].name, PlaceName3[1][Game.language]);
    }
    newleveltime_slot = i;
}

extern void ResetTubs(void);
extern void InitGameCut(void);
extern void ResetDRAINDAMAGE(void);
extern void ResetCRUNCHTIME(void);

void InitGameMode(s32 mode) {
    ResetTimer(&GameTimer);
    finish_frame = 0;
    ResetTimer(&PauseTimer);
    pausestats_frame = 0;
    ResetTimer(MenuTimer);
    ResetTimeTrial();
    if (force_menu != -1) {
        NewMenu((struct cursor_s *)Cursor, force_menu, -1, -1);
        force_menu = -1;
    } else {
        if ((LDATA->flags & 1) != 0) {
            start_time = 3.0f;
            NewMenu((struct cursor_s *)Cursor, -1, -1, -1);
        } else {
            start_time = 0.0f;
        }
    }
    /* both pairs pre-swapped: adjacent global stores emit reversed */
    new_level = -1;
    new_mode = -1;
    ResetPlayer(1);
    ResetTubs();
    plr_lives.draw = Game.lives;
    plr_lives.count = plr_lives.draw;
    switch (Level) {
    case 0x25:
        if (GameMode == 1) {
            InitGameCut();
            return;
        }
        return;
    case 0x17:
        ResetDRAINDAMAGE();
        return;
    case 0x19:
        ResetCRUNCHTIME();
        return;
    case 0x28:
        InitGameCut();
        break;
    }
}

/* animpacket: action +0xC / oldaction +0xE / newaction +0x10 / flags +0x1B
 * verified in UpdateTempCharacter */

extern s32 TESTCORTEXVOICES;
extern struct nuvec_s in_finish_pos;
extern struct nuvec_s probepos;
extern struct nuvec_s probedpos;
extern struct nuvec_s probespk;
extern struct nuvec_s probepos2;
extern s32 gamesfx_pitch;
extern float NuFabs(float x);
extern float NuVecDist(struct nuvec_s *a, struct nuvec_s *b, struct nuvec_s *d);
extern void NuLgtArcLaser(s32 type, struct nuvec_s *start, struct nuvec_s *end,
                          struct nuvec_s *mid, float a, float b, float c, float d,
                          u32 col);
extern void AddVariableShotDebrisEffect(s32 type, struct nuvec_s *pos, s32 count,
                                        s32 a, s32 b);
void Draw3DCharacter(struct nuvec_s *pos, u16 xrot, u16 yrot, u16 zrot,
                     struct cmodel_s *model, float scale, s32 action,
                     float anim_time, s32 last);

void JonProbe(void) {
    struct nuvec_s pos;
    struct nuvec_s end;
    struct nuvec_s off;
    struct nuvec_s oldpos;
    s32 charging;
    float x;
    float z;
    float dist;
    s32 col;
    s32 ang;
    s32 vol;
    s32 i;

    if (Hub != -1 && in_finish_range > 0) {
        if (probeon == 0) {
            probepos = in_finish_pos;
            probepos.y += 4.5f;
            probeon = 1;
            GameSfx(0x82, &probepos);
            NewRumble(player->obj.rumble, 0x7F);
            NewBuzz(player->obj.rumble, 5);
        }
        probetime = 0x5DC;
        probedpos = in_finish_pos;
    }
    oldpos = probepos;
    if (probeon == 0) {
        return;
    }
    if (Paused == 0) {
        if (probetime != 0) {
            probetime -= 0x3C;
            if (probetime < 0) {
                probetime = 0;
            }
            probey += 0x5A00;
            if (probey > 0xC8000) {
                probey = 0xC8000;
            }
        } else {
            probey -= 0x5A00;
            if (probey < 0) {
                probey = 0;
                probeon = 0;
                GameSfx(0x82, &probepos);
                NewRumble(player->obj.rumble, 0x7F);
                NewBuzz(player->obj.rumble, 5);
            }
        }
        pos = probedpos;
        pos.y += 4.5f - NuTrigTable[(probey / 50) & 0xFFFF] * 2.5f;
        x = (probepos.x - pos.x) * 0.0625f;
        if (x > 0.1f) {
            x = 0.1f;
        }
        if (x < -0.1f) {
            x = -0.1f;
        }
        probepos.x -= x;
        z = (probepos.z - pos.z) * 0.0625f;
        if (z > 0.1f) {
            z = 0.1f;
        }
        if (z < -0.1f) {
            z = -0.1f;
        }
        probepos.y -= (probepos.y - pos.y) * 0.5f;
        probepos.z -= z;
        proberot.y = (proberot.y + 0xA3) & 0xFFFF;
        if (NuFabs(x) < 0.005f && NuFabs(z) < 0.005f && probey == 0xC8000) {
            if (probecol == 0) {
                gamesfx_effect_volume = 0xBFFD;
                GameSfx(0x4B, &probepos);
            }
            probespk = probepos;
            pos = probepos;
            AddVariableShotDebrisEffect(GDeb[304], &pos, 1, -0x8000, 0);
            probecol += 0xF0;
            if (probecol > 0x1900) {
                probecol = 0x1900;
            }
            charging = 0;
        } else {
            in_finish_range = 1;
            probecol -= 0xF0;
            if (probecol < 0) {
                probecol = 0;
            }
            charging = 1;
        }
    }
    if (probecol != 0) {
        ang = proberot.y + 0xBB8;
        col = (probecol / 50) << 24;
        for (i = 0; i < 3; i++) {
            pos = probespk;
            end.y = pos.y;
            pos.y -= 0.05;
            end.y -= 1.8f;
            if (charging != 0) {
                pos.y -= (float)(0x80 - probecol / 50) * 0.015625f;
                x = (float)(probecol / 50) / 426.66666f;
                z = (float)(probecol / 50) / 266.66669f;
            } else {
                x = 0.3f;
                z = 0.48f;
            }
            off.x = NuTrigTable[ang & 0xFFFF] * z;
            off.z = NuTrigTable[(ang + 0x4000) & 0xFFFF] * z;
            off.y = 0.0f;
            pos.x += off.x;
            pos.z += off.z;
            end.x = pos.x + off.x;
            end.z = pos.z + off.z;
            if (Paused == 0) {
                NuLgtArcLaser(0, &end, &pos, &off, 0.1f, 0.1f, 0.01f, x,
                              col + 0xFF7F3F);
                NuLgtArcLaser(0, &end, &pos, &off, 0.4f, 0.3f, 0.001f, x,
                              col + 0x800040);
                AddVariableShotDebrisEffect(GDeb[312], &end, 1, 0, 0);
                AddVariableShotDebrisEffect(GDeb[312], &pos, 1, -0x8000, 0);
            }
            ang += 0x5555;
        }
    }
    if (CRemap[0xB1] != -1) {
        Draw3DCharacter(&probepos, proberot.x, proberot.y, proberot.z,
                        &CModel[CRemap[0xB1]], 1.0f, -1, 1.0f, 0);
    }
    vol = 16383.0f - NuVecDist(&probepos, &oldpos, 0) * 16383.0f / 0.06f;
    if (vol > 0) {
        gamesfx_effect_volume = vol;
        gamesfx_pitch = 0x75A;
        GameSfxLoop(0x61, &probepos);
    }
    probepos2 = probepos;
    dist = NuVecDist(&probepos, &oldpos, 0);
    vol = dist * 16383.0f / 0.06f;
    if (vol >= 0x4000) {
        vol = 0x3FFF;
    }
    if (vol > 0) {
        gamesfx_effect_volume = vol;
        gamesfx_pitch = dist * 4700.0005f + 470.0f;
        if (gamesfx_pitch > 0x3AD) {
            gamesfx_pitch = 0x3AD;
        }
        GameSfxLoop(0x3A, &probepos2);
    }
}

extern float D_0062D274;
extern float D_0062D278;
extern void UpdateAnimPacket(struct cmodel_s *m, struct animpkt_s *a, float r,
                             float b);
extern s32 NuSoundKeyStatus(s32 ch);

void UpdateTempCharacter(void) {
    struct cmodel_s *model;
    s32 i;

    if (temp_character2 != -1) {
        i = CRemap[temp_character2];
        if (i == -1) {
            return;
        }
        ((struct animpkt_s *)TempAnim2)->oldaction =
            ((struct animpkt_s *)TempAnim2)->action;
        /* hoisted: retail computes &CModel[i] before the action checks */
        model = &CModel[i];
        if (((struct animpkt_s *)TempAnim2)->action < 0x76 &&
            ((struct animpkt_s *)TempAnim2)->newaction < 0x76) {
            UpdateAnimPacket(model, (struct animpkt_s *)TempAnim2, D_0062D274,
                             0.0f);
        }
    }
    if (temp_character != -1) {
        i = CRemap[temp_character];
        if (i == -1) {
            return;
        }
        ((struct animpkt_s *)TempAnim)->oldaction =
            ((struct animpkt_s *)TempAnim)->action;
        if (temp_character == 2 &&
            ((((struct animpkt_s *)TempAnim)->flags & 1) != 0 ||
             (Pad[0] != NULL && (Pad[0]->paddata & 0x800) != 0))) {
            if (Level == 0x26) {
                Game.lives = 4;
                if (cortex_gameover_i >= 0) {
                    if (cortex_gameover_i < 2) {
                        if (TESTCORTEXVOICES != 0) {
                            new_level = Level;
                        } else {
                            new_level = 0x25;
                        }
                    } else if (cortex_gameover_i < 7) {
                        if (TESTCORTEXVOICES != 0) {
                            new_level = Level;
                        } else {
                            new_level = 0x23;
                        }
                    }
                }
            }
            if ((((struct animpkt_s *)TempAnim)->flags & 1) != 0) {
                ((struct animpkt_s *)TempAnim)->flags = 0;
                ((struct animpkt_s *)TempAnim)->newaction = 0x22;
            }
        }
        if (tempanim_waitaudio != 0 && NuSoundKeyStatus(4) == 1) {
            ((struct animpkt_s *)TempAnim)->newaction = tempanim_nextaction;
            tempanim_waitaudio = 0;
        }
        model = &CModel[i];
        if (((struct animpkt_s *)TempAnim)->action < 0x76 &&
            ((struct animpkt_s *)TempAnim)->newaction < 0x76) {
            UpdateAnimPacket(model, (struct animpkt_s *)TempAnim, D_0062D278,
                             0.0f);
        }
    }
}

extern s32 tumble_character;
extern float tumble_item_addtime;
extern void NuVecAdd(struct nuvec_s *d, struct nuvec_s *a, struct nuvec_s *b);
extern void AddGameDebris(s32 i, struct nuvec_s *pos);

void UpdateAwards(void) {
    struct award_s *award;
    float time;
    float y;
    s32 i;

    award = Award;
    for (i = 0; i < 3; i++, award++) {
        time = award->time;
        if (time < 1.0f) {
            if (award->wait != 0) {
                if (tumble_character == 0) {
                    NuVecAdd(&award->oldpos0,
                             (struct nuvec_s *)player->obj.mtxLOCATOR[0][7].m[3],
                             (struct nuvec_s *)player->obj.mtxLOCATOR[0][8].m[3]);
                    NuVecScale(0.5f, &award->oldpos0, &award->oldpos0);
                } else {
                    award->oldpos0 =
                        *(struct nuvec_s *)player->obj.mtxLOCATOR[0][3].m[3];
                }
                award->oldpos1.x = award->oldpos0.x;
                y = award->oldpos0.y;
                if (tumble_character == 1) {
                    y += 1.0f;
                } else {
                    y += 0.5f;
                }
                award->oldpos1.y = y;
                award->oldpos1.z = award->oldpos0.z;
                if (player->obj.anim_time >= tumble_item_addtime) {
                    award->wait = 0;
                    GameSfx(0x26, NULL);
                    AddGameDebris(0xA1, &award->oldpos1);
                }
            } else {
                time += 0.02f;
                award->time = time;
                if (time >= 1.0f) {
                    award->time = 1.0f;
                    new_lev_flags |= award->got;
                    new_lev_flags ^= award->got;
                    Game.level[award->level].flags |= award->got;
                    CalculateGamePercentage(&Game);
                    AddGameDebris(0xA1, &award->newpos);
                } else if (time < 0.666f) {
                    award->oldpos0.y +=
                        (award->oldpos1.y - award->oldpos0.y) * 0.1f;
                }
            }
        }
    }
}

extern float tumble_item_starttime;
s32 Draw3DObject(s32 object, struct nuvec_s *pos, u16 xrot, u16 yrot, u16 zrot,
                 float scalex, float scaley, float scalez,
                 struct nugscn_s *scn, struct objspecial_s *obj, s32 rot);

void DrawAwards(void) {
    struct award_s *award;
    struct nuvec_s pos;
    float scale;
    float t;
    s32 object;
    s32 character;
    s32 model;
    s32 i;

    award = Award;
    for (i = 0; i < 3; i++, award++) {
        if (award->time < 1.0f) {
            if (award->wait != 0) {
                pos = award->oldpos0;
                t = tumble_item_starttime + 1.0f;
                scale = (player->obj.anim_time - t) / (tumble_item_addtime - t);
                if (scale > 1.0f) {
                    scale = 1.0f;
                }
                if (scale < 0.333f) {
                    scale = 0.0f;
                } else {
                    scale = (scale - 0.333f) / 0.667f;
                }
            } else if (award->time < 0.666f) {
                scale = 1.0f;
                pos = award->oldpos0;
            } else {
                t = (award->time - 0.666f) / 0.334f;
                pos.x = award->oldpos0.x + (award->newpos.x - award->oldpos0.x) * t;
                pos.y = award->oldpos0.y + (award->newpos.y - award->oldpos0.y) * t;
                pos.z = award->oldpos0.z + (award->newpos.z - award->oldpos0.z) * t;
                scale = 1.0f;
            }
            object = -1;
            character = -1;
            if ((award->got & 4) != 0) {
                object = 1;
            } else if ((award->got & 2) != 0) {
                object = 2;
            } else if ((award->got & 1) != 0) {
                object = 3;
            } else if (award->got == 8) {
                object = 0xC1;
            } else if (award->got == 0x10) {
                object = 0xC2;
            } else if (award->got == 0x20) {
                object = 0xC3;
            } else if (award->got == 0x40) {
                object = 0xC4;
            } else if (award->got == 0x80) {
                object = 0xC5;
            } else if (award->got == 0x100) {
                object = 0xC6;
            } else if (award->got == 0x200) {
                object = 0xC8;
            } else if (award->got == 0x400) {
                object = 0xC7;
            }
            if (object != -1) {
                if (ObjTab[object].obj.special != NULL) {
                    Draw3DObject(object, &pos, 0, award->yrot, 0, scale * 0.77f,
                                 scale * 0.77f, scale * 0.77f,
                                 ObjTab[object].obj.scene,
                                 ObjTab[object].obj.special, 0);
                }
            } else if (character != -1) {
                model = CRemap[character];
                if (model != -1) {
                    Draw3DCharacter(&pos, 0, award->yrot, 0, &CModel[model],
                                    scale * 0.77f, -1, 1.0f, 0);
                }
            }
        }
    }
}

extern u16 temp_lev_flags;
extern u16 tumble_hdg;
extern struct nuvec_s tumble_oldpos;
extern struct nuvec_s tumble_newpos;
extern float tumble_cycleduration;
extern float D_0062D29C;
extern float NewShadowMaskPlat(struct nuvec_s *pos, float y, s32 layer);
extern float ModelAnimDuration(u32 character, u32 action, float start, float end);

void HubStart(struct tobj_s *obj, s32 hub, s32 level, struct nuvec_s *pos) {
    struct nugspline_s *spl;
    struct nuvec_s *p0;
    struct nuvec_s *p1;
    float y;
    float end;
    s32 i_spl;
    s32 h;
    s32 l;

    if (level != -1 && (new_lev_flags & 0x800) != 0) {
        new_lev_flags ^= 0x800;
        Game.level[level].flags |= 0x800;
        CalculateGamePercentage(&Game);
    }
    tumble_action = -1;
    tumble_time = tumble_duration = 0.0f;
    temp_lev_flags = 0;
    i_spl = HData[hub].i_spl[0];
    if (i_spl == -1 || SplTab[i_spl].spl == NULL) {
        *pos = v000;
        return;
    }
    if (level != -1) {
        for (h = 0; h < 6; h++) {
            for (l = 0; l < 6; l++) {
                if (HData[h].level[l] == level) {
                    temp_hub = h;
                    temp_hublevel = l;
                    goto found;
                }
            }
        }
    }
found:
    spl = SplTab[i_spl].spl;
    p0 = (struct nuvec_s *)(spl->pts + (temp_hublevel * 2 + 2) * spl->ptsize);
    tumble_oldpos = *p0;
    p1 = (struct nuvec_s *)(spl->pts + (temp_hublevel * 2 + 3) * spl->ptsize);
    pos->x = p0->x + (p1->x - p0->x) * 2.0f;
    pos->y = p0->y;
    pos->z = p0->z + (p1->z - p0->z) * 2.0f;
    y = NewShadowMaskPlat(pos, 0.0f, -1);
    if (y != D_0062D29C) {
        pos->y = y;
    }
    tumble_newpos.x = pos->x;
    tumble_newpos.y = pos->y - obj->bot * obj->SCALE;
    tumble_newpos.z = pos->z;
    *pos = tumble_newpos;
    tumble_hdg = NuAtan2D(p1->x - p0->x, p1->z - p0->z);
    tumble_action = new_lev_flags ? 0x56 : 0x55;
    tumble_time = 0.0f;
    tumble_duration = ModelAnimDuration(tumble_character, tumble_action, 0.0f, 0.0f);
    if (tumble_character == 1) {
        tumble_item_starttime = 46.0f;
        tumble_item_addtime = 90.0f;
        end = 25.0f;
    } else {
        end = 25.0f;
        if (tumble_action == 0x56) {
            end = 19.0f;
        }
        tumble_item_starttime = 32.0f;
        tumble_item_addtime = 50.0f;
    }
    tumble_moveduration = ModelAnimDuration(tumble_character, tumble_action, 0.0f, end);
    tumble_cycleduration = ModelAnimDuration(tumble_character, tumble_action, 0.0f,
                                             tumble_item_starttime);
}

extern s32 HubLevel;
extern s32 HubLevel_available;
extern u16 HubLevelAngle;
extern s32 hubleveltext_i;
extern s32 hubleveltext_open;
extern s32 hub_vr_image;
extern u32 finish_frames;
extern u8 hublevelopen[6][6];
extern float FINISHRADIUSHUB;
extern struct nuvec_s hubwarp_oldpos;
extern struct nuvec_s hubwarp_newpos;
extern void AddWarpDebris(struct tobj_s *obj, struct nuvec_s *pos);

void HubLevelSelect(struct tobj_s *obj, s32 hub) {
    struct nuvec_s pos;
    struct nuvec_s d;
    struct nuvec_s *p0;
    struct nuvec_s *p1;
    float range;
    float dx;
    float dz;
    float dist;
    float best;
    double dy;
    u16 ang;
    s32 i_spl;
    s32 level;
    s32 i;

    if (warp_level != -1) {
        finish_frame++;
        if (finish_frame == finish_frames) {
            new_level = HData[hub].level[HubLevel];
        }
        if (hubleveltext_pos > 0.333f) {
            hubleveltext_pos -= 0.02f;
            if (hubleveltext_pos < 0.333f) {
                hubleveltext_pos = 0.333f;
            }
        } else if (hubleveltext_pos < 0.333f) {
            hubleveltext_pos += 0.02f;
            if (hubleveltext_pos > 0.333f) {
                hubleveltext_pos = 0.333f;
            }
        }
        return;
    }
    HubLevel = -1;
    finish_frame = 0;
    HubLevel_available = 0;
    level = -1;
    if (new_mode != -1 || new_level != -1 || hub == -1) {
        goto text;
    }
    i_spl = HData[hub].i_spl[0];
    if (i_spl == -1 || SplTab[i_spl].spl == NULL || Game.hub[hub].flags == 0) {
        goto text;
    }
    range = 4.0f;
    if (hub == 5) {
        range = 16.0f;
    }
    for (i = 0; i < 6; i++) {
        if (hub == 5 && i == hub) {
            continue;
        }
        p0 = (struct nuvec_s *)(SplTab[i_spl].spl->pts +
                                (i * 2 + 2) * SplTab[i_spl].spl->ptsize);
        dx = p0->x - obj->pos.x;
        dz = p0->z - obj->pos.z;
        dist = dx * dx + dz * dz;
        if (dist < range && (HubLevel == -1 || dist < best)) {
            HubLevel = i;
            best = dist;
            hub_vr_level = i;
        }
    }
    if (HubLevel == -1 || hublevelopen[hub][HubLevel] == 0) {
        in_finish_range = 0;
    } else {
        HubLevel_available = 1;
    }
    level = HData[hub].level[HubLevel];
    if (HubLevel != -1) {
        if (Level == 0x25 && tumble_time < tumble_duration && last_hub != -1) {
            in_finish_range = 0;
        } else {
            p0 = (struct nuvec_s *)(SplTab[i_spl].spl->pts +
                                  (HubLevel * 2 + 2) * SplTab[i_spl].spl->ptsize);
            p1 = (struct nuvec_s *)(SplTab[i_spl].spl->pts +
                                  (HubLevel * 2 + 3) * SplTab[i_spl].spl->ptsize);
            ang = NuAtan2D(obj->pos.x - p0->x, obj->pos.z - p0->z);
            pos.x = p0->x;
            pos.y = p0->y + 1.0f;
            pos.z = p0->z;
            d.x = obj->pos.x - pos.x;
            d.y = obj->pos.y + (obj->bot + obj->top) * obj->SCALE * 0.5f - pos.y;
            d.z = obj->pos.z - pos.z;
            if (d.x * d.x + d.z * d.z < FINISHRADIUSHUB * FINISHRADIUSHUB) {
                dy = d.y;
                if (dy < 0.0) {
                    dy = 0.0 - dy;
                }
                if (dy < 5.0) {
                    in_finish_pos = *p0;
                    if (HubLevel_available != 0) {
                        in_finish_range++;
                        if (in_finish_range < 50) {
                            goto text;
                        }
                    }
                    if (level == -1) {
                        HubLevel = -1;
                    } else if (HubLevel_available != 0) {
                        if (HubLevel < 5) {
                            hub_vr_image = level;
                        }
                        p0 = (struct nuvec_s *)(SplTab[i_spl].spl->pts +
                                              (HubLevel * 2 + 2) * SplTab[i_spl].spl->ptsize);
                        p1 = (struct nuvec_s *)(SplTab[i_spl].spl->pts +
                                              (HubLevel * 2 + 3) * SplTab[i_spl].spl->ptsize);
                        level = HData[hub].level[HubLevel];
                        HubLevelAngle = NuAtan2D(p1->x - p0->x, p1->z - p0->z);
                        warp_level = HData[hub].level[HubLevel];
                        AddWarpDebris(obj, &pos);
                        finish_frames = 50;
                        hubwarp_oldpos = obj->pos;
                        hubwarp_newpos = *p0;
                        gamesfx_effect_volume = 0x7FFE;
                        GameSfx(0x1E, &obj->pos);
                        goto text;
                    }
                    if (Level == 0x25 && tumble_time < tumble_duration && last_hub != -1) {
                        goto text;
                    }
                    if (HubLevel == 5) {
                        obj->mom.x = NuTrigTable[ang] * 0.06f;
                        obj->mom.z = NuTrigTable[(u16)(ang + 0x4000)] * 0.06f;
                    } else {
                        ang = NuAtan2D(p1->x - p0->x, p1->z - p0->z);
                        obj->mom.x = NuTrigTable[ang] * 0.06f;
                        obj->mom.z = NuTrigTable[(u16)(ang + 0x4000)] * 0.06f;
                    }
                } else {
                    in_finish_range = 0;
                }
            } else {
                in_finish_range = 0;
            }
        }
    } else {
        in_finish_range = 0;
    }
text:
    best = 0.0f;
    if (level != -1) {
        if (hubleveltext_pos == 0.0f) {
            hubleveltext_i = HubLevel;
            hubleveltext_open = HubLevel_available;
            hubleveltext_level = level;
        }
        if (hubleveltext_level != -1 && level == hubleveltext_level &&
            tumble_time == tumble_duration) {
            best = 0.333f;
        }
    }
    if (hubleveltext_pos > best) {
        hubleveltext_pos -= 0.02f;
        if (hubleveltext_pos < best) {
            hubleveltext_pos = best;
        }
    } else if (hubleveltext_pos < best) {
        hubleveltext_pos += 0.02f;
        if (hubleveltext_pos > best) {
            hubleveltext_pos = best;
        }
    }
    if (hubleveltext_pos == 0.0f) {
        hubleveltext_level = -1;
    }
}

extern struct rpos_s *best_cRPos;
extern float hub_vr_scale;
extern float hub_vr_newscale;
extern struct nuvec_s hub_vr_pos;
extern u16 hub_vr_xrot;
extern u16 hub_vr_yrot;

void HubMoveVR(void) {
    struct nuvec_s pos;
    struct nugspline_s *spl;
    struct nuvec_s *p0;
    struct nuvec_s *p1;
    float t;
    float scale;
    float dx;
    float dy;
    float dz;
    u32 across;
    u32 i;
    s32 i_spl;
    u16 xrot;
    u16 yrot;

    if (hub_vr_level == -1) {
        hub_vr_scale = hub_vr_newscale = 0.0f;
    } else if (hub_vr_scale > hub_vr_newscale) {
        hub_vr_scale -= 0.04f;
        if (hub_vr_scale < hub_vr_newscale) {
            hub_vr_scale = hub_vr_newscale;
        }
    } else if (hub_vr_scale < hub_vr_newscale) {
        hub_vr_scale += 0.04f;
        if (hub_vr_scale > hub_vr_newscale) {
            hub_vr_scale = hub_vr_newscale;
        }
    }
    if (Hub == -1) {
        return;
    }
    i_spl = HData[Hub].i_spl[0];
    if (i_spl == -1 || best_cRPos == NULL) {
        return;
    }
    spl = SplTab[i_spl].spl;
    if (spl == NULL) {
        return;
    }
    across = (s32)(best_cRPos->fACROSS * 262144.0f);
    i = across >> 16;
    p0 = (struct nuvec_s *)(spl->pts + (i + 14) * spl->ptsize);
    pos.x = p0->x;
    pos.y = p0->y;
    pos.z = p0->z;
    if (i < 4) {
        across &= 0xFFFF;
        if (across != 0) {
            p1 = (struct nuvec_s *)(spl->pts + (i + 15) * spl->ptsize);
            t = across * (1.0f / 65536.0f);
            pos.x += (p1->x - p0->x) * t;
            pos.y += (p1->y - p0->y) * t;
            pos.z += (p1->z - p0->z) * t;
        }
    }
    hub_vr_pos.x += (pos.x - hub_vr_pos.x) * 0.1f;
    hub_vr_pos.y += (pos.y - hub_vr_pos.y) * 0.1f;
    hub_vr_pos.z += (pos.z - hub_vr_pos.z) * 0.1f;
    scale = 0.0f;
    if (HubLevel_available != 0 && HubLevel >= 0 && HubLevel < 5) {
        scale = 1.0f;
    }
    hub_vr_newscale = scale;
    dx = player->obj.pos.x - hub_vr_pos.x;
    dy = player->obj.pos.y + player->obj.top * player->obj.SCALE - hub_vr_pos.y;
    dz = player->obj.pos.z - hub_vr_pos.z;
    xrot = panel_head_yrot - NuAtan2D(dy, NuFsqrt(dx * dx + dz * dz));
    yrot = panel_head_xrot + NuAtan2D(dx, dz);
    hub_vr_xrot = SeekRot(hub_vr_xrot, xrot, 2);
    hub_vr_yrot = SeekRot(hub_vr_yrot, yrot, 2);
}


void HubDrawItems(void) {
    struct objspecial_s *special;
    struct nugspline_s *spl;
    struct nuvec_s *p0;
    struct nuvec_s *p1;
    s32 level;
    s32 language;
    s32 object;
    s32 character;
    s32 i;
    s32 n;
    s32 k;

    language = 0;
    if (Game.language != 'c') {
        language = Game.language;
    }
    for (i = 0; i < 6; i++) {
        if (ObjTab[0x73 + i].obj.special != NULL) {
            if (GameMode != 1 && i == language) {
                ObjTab[0x73 + i].obj.special->instance->flags.visible = 1;
            } else {
                ObjTab[0x73 + i].obj.special->instance->flags.visible = 0;
            }
        }
    }
    special = ObjTab[0x79].obj.special;
    if (special != NULL) {
        if (GameMode == 1) {
            special->instance->flags.visible = 1;
        } else {
            special->instance->flags.visible = 0;
        }
    }
    if (hub_vr_scale > 0.0f) {
        n = 0x66;
        if (hubleveltext_level != -1 && hubleveltext_pos == 0.333f) {
            n = hubleveltext_level + 0x17;
        }
        Draw3DObject(n, &hub_vr_pos, hub_vr_xrot, hub_vr_yrot, 0, hub_vr_scale,
                     hub_vr_scale, hub_vr_scale, ObjTab[n].obj.scene,
                     ObjTab[n].obj.special, 0);
        Draw3DObject(0x16, &hub_vr_pos, hub_vr_xrot, hub_vr_yrot, 0, hub_vr_scale,
                     hub_vr_scale, hub_vr_scale, ObjTab[0x16].obj.scene,
                     ObjTab[0x16].obj.special, 0);
    }
    for (i = 0; i < 6; i++) {
        n = HData[i].barrier;
        if (n != 0xFF && ObjTab[n].obj.special != NULL) {
            ObjTab[n].obj.special->instance->flags.visible = Game.hub[i].flags ^ 1;
        }
        n = HData[i].i_spl[1];
        if (n == -1) {
            continue;
        }
        spl = SplTab[n].spl;
        if (spl == NULL) {
            continue;
        }
        for (n = 0; n < 6; n++) {
            if (i == 5 && n == i) {
                continue;
            }
            if (n == 5) {
                continue;
            }
            for (k = 0; k < 4; k++) {
                level = HData[i].level[n];
                if (level == -1) {
                    continue;
                }
                p0 = (struct nuvec_s *)(spl->pts + (n * 8 + k * 2) * spl->ptsize);
                p1 = (struct nuvec_s *)(spl->pts + (n * 8 + k * 2 + 1) * spl->ptsize);
                object = -1;
                character = -1;
                switch (k) {
                case 0:
                    if ((Game.level[level].flags & 0x10) != 0) {
                        object = 0xC2;
                    }
                    break;
                case 1:
                    if ((Game.level[level].flags & 8) != 0) {
                        object = 0xC1;
                    }
                    break;
                case 2:
                    if ((Game.level[level].flags & 4) != 0) {
                        object = 1;
                    } else if ((Game.level[level].flags & 2) != 0) {
                        object = 2;
                    } else if ((Game.level[level].flags & 1) != 0) {
                        object = 3;
                    }
                    break;
                case 3:
                    if ((Game.level[level].flags & 0x20) != 0) {
                        object = 0xC3;
                    } else if ((Game.level[level].flags & 0x40) != 0) {
                        object = 0xC4;
                    } else if ((Game.level[level].flags & 0x80) != 0) {
                        object = 0xC5;
                    } else if ((Game.level[level].flags & 0x100) != 0) {
                        object = 0xC6;
                    } else if ((Game.level[level].flags & 0x200) != 0) {
                        object = 0xC8;
                    } else if ((Game.level[level].flags & 0x400) != 0) {
                        object = 0xC7;
                    }
                    break;
                }
                if (object != -1) {
                    if (ObjTab[object].obj.special != NULL) {
                        Draw3DObject(object, p0, 0, NuAtan2D(p1->x - p0->x, p1->z - p0->z), 0,
                                     0.77f, 0.77f, 0.77f, ObjTab[object].obj.scene,
                                     ObjTab[object].obj.special, 0);
                    }
                } else if (character != -1) {
                    if (CRemap[character] != -1) {
                        Draw3DCharacter(p0, 0, NuAtan2D(p1->x - p0->x, p1->z - p0->z), 0,
                                        &CModel[CRemap[character]], 0.77f, -1, 1.0f, 0);
                    }
                }
            }
        }
    }
    JonProbe();
}

extern void NuMtxRotateX(struct numtx_s *m, s32 a);
extern void NuMtxRotateZ(struct numtx_s *m, s32 a);

/* The GC draft is WRONG here (same trap as DrawPanel3DObject, see
 * docs/decomp_agent.md): it drops the 1/2/3 -> 0x85/0x86/0x87 dispatch and
 * nests the render inside the Level check. Retail guards only the recursion;
 * the render always runs. */
s32 Draw3DObject(s32 object, struct nuvec_s *pos, u16 xrot, u16 yrot, u16 zrot,
                 float scalex, float scaley, float scalez,
                 struct nugscn_s *scn, struct objspecial_s *obj, s32 rot) {
    struct numtx_s m;
    struct nuvec_s s;
    s32 o;

    if (scn != NULL && obj != NULL &&
        (scalex != 0.0f || scaley != 0.0f || scalez != 0.0f)) {
        if (Level != 0x25) {
            if (object == 1) {
                o = 0x85;
            } else if (object == 2) {
                o = 0x86;
            } else {
                o = (object == 3) ? 0x87 : -1;
            }
            if (o != -1) {
                Draw3DObject(o, pos, xrot, yrot, zrot, scalex, scaley, scalez,
                             ObjTab[o].obj.scene, ObjTab[o].obj.special, rot);
            }
        }
        s.x = scalex;
        s.y = scaley;
        s.z = scalez;
        NuMtxSetScale(&m, &s);
        switch (rot) {
        case 0:
            if (xrot != 0) {
                NuMtxRotateX(&m, xrot);
            }
            if (yrot != 0) {
                NuMtxRotateY(&m, yrot);
            }
            if (zrot != 0) {
                NuMtxRotateZ(&m, zrot);
            }
            break;
        case 1:
            if (yrot != 0) {
                NuMtxRotateY(&m, yrot);
            }
            if (xrot != 0) {
                NuMtxRotateX(&m, xrot);
            }
            if (zrot != 0) {
                NuMtxRotateZ(&m, zrot);
            }
            break;
        }
        NuMtxTranslate(&m, pos);
        return NuRndrGScnObj(scn->gobjs[obj->instance->objid], &m);
    }
    return 0;
}

extern s32 bonus_finish_frame;
extern u8 bonus_wumpa_delay;
extern u8 bonus_life_delay;
extern s32 bonus_lives;
extern float bonus_wumpa_wait;
extern float bonus_lives_wait;
extern struct plr_lives_s plr_bonus_wumpas;
extern float BONUSWUMPAOBJSX;
extern float BONUSLIVESOBJSX;
extern float BONUSPANELSY;
extern float BONUSLIFESCALE;
extern float D_0062D5F0; /* wumpa wait step (PAL; GC 1/60) */
extern float D_0062D5F4; /* lives wait step (PAL; GC 1/60) */
extern void AddPanelDebris(float x, float y, s32 i, float scale, s32 n);

/* creature wrapper: obj sits at +0x4, so obj.dead lands at 0x149 */
struct bcreature_s {
    char pad_4[4];
    struct tobj_s obj; /* 0x4 */
};

void BonusTiming(struct bcreature_s *plr) {
    s32 dead;

    dead = 0;
    if (Bonus == 2 && plr->obj.dead != 0) {
        dead = 1;
    }
    if ((Bonus != 3 && Bonus != 4) && !dead) {
        return;
    }
    bonus_finish_frame++;
    if (bonus_wumpa_delay != 0) {
        bonus_wumpa_delay--;
        if (plr_bonus_wumpas.count == 0) {
            bonus_wumpa_wait = 0.5f;
        }
    } else {
        if (plr_bonus_wumpas.count != 0) {
            bonus_wumpa_delay = 5; /* PAL (GC 6) */
            if (dead == 0) {
                plr_wumpas.count++;
                AddPanelDebris(BONUSWUMPAOBJSX, BONUSPANELSY, 0, 1.0f, 1);
            } else {
                AddPanelDebris(BONUSWUMPAOBJSX, BONUSPANELSY, 1, 1.0f, 1);
                GameSfx(0x19, NULL);
            }
            plr_bonus_wumpas.count--;
            if (plr_bonus_wumpas.count == 0) {
                bonus_wumpa_wait = 0.5f;
            }
        } else if (bonus_wumpa_wait > 0.0f) {
            bonus_wumpa_wait -= D_0062D5F0;
            if (bonus_wumpa_wait < 0.0f) {
                bonus_wumpa_wait = 0.0f;
            }
        }
    }
    plr_bonus_wumpas.draw = plr_bonus_wumpas.count;
    if (bonus_life_delay != 0) {
        bonus_life_delay--;
        if (bonus_lives == 0) {
            bonus_lives_wait = 0.5f;
        }
    } else if (bonus_lives != 0) {
        bonus_life_delay = 0x4B; /* PAL: 90 * 50/60 (GC 90) */
        if (!dead) {
            AddPanelDebris(BONUSLIVESOBJSX, BONUSPANELSY, 4, BONUSLIFESCALE, 1);
        }
        bonus_lives--;
        if (bonus_lives == 0) {
            bonus_lives_wait = 0.5f;
        }
    } else if (bonus_lives_wait > 0.0f) {
        bonus_lives_wait -= D_0062D5F4;
        if (bonus_lives_wait < 0.0f) {
            bonus_lives_wait = 0.0f;
        }
    }
}

extern float D_0062D35C; /* mask "no shadow" sentinel (GC 2000000.0f) */
extern float D_0062D360; /* shadow y bias (GC 0.025f) */
extern void NuMtxMulR(struct numtx_s *d, struct numtx_s *a, struct numtx_s *b);
extern void ScaleFlatShadow(struct nuvec_s *s, float y, float shadow, float f);

void MakeMaskMatrix(struct mask_s *mask, struct numtx_s *mM, struct numtx_s *mS,
                    struct numtx_s *mLOCATOR, float scale) {
    struct nuvec_s s;
    u16 yrot;

    if (mM != NULL) {
        s.x = s.y = s.z = scale * mask->scale;
        NuMtxSetScale(mM, &s);
        yrot = mask->yrot - 0x8000;
        if (mask->active > 2) {
            if (mLOCATOR != NULL) {
                NuMtxMulR(mM, mM, mLOCATOR);
                mask->pos.x = mLOCATOR->m[3][0];
                mask->pos.y = mLOCATOR->m[3][1];
                mask->pos.z = mLOCATOR->m[3][2];
            } else {
                NuMtxRotateX(
                    mM, (s32)(NuTrigTable[(GameTimer.frame & 0xf) * 0x1000] *
                              8192.0f) &
                            0xffff);
                NuMtxRotateZ(
                    mM,
                    (s32)(NuTrigTable[(u16)((GameTimer.frame & 0xf) * 0x1000 +
                                            0x4000)] *
                          8192.0f) &
                        0xffff);
                NuMtxRotateY(mM, yrot);
            }
        } else {
            NuMtxRotateX(mM, mask->xrot);
            NuMtxRotateY(mM, yrot);
        }
        NuMtxTranslate(mM, &mask->pos);
        if (mS != NULL && mask->shadow != D_0062D35C) {
            ScaleFlatShadow(&s, mask->pos.y, mask->shadow, 1.0f);
            NuMtxSetScale(mS, &s);
            NuMtxRotateY(mS, yrot);
            NuMtxRotateZ(mS, mask->surface_zrot);
            NuMtxRotateX(mS, mask->surface_xrot);
            mS->m[3][0] = mask->pos.x;
            mS->m[3][1] = mask->shadow + D_0062D360;
            mS->m[3][2] = mask->pos.z;
        }
    }
}

extern void NewRumble(void *p, s32 n);

void LoseMask(struct tobj_s *plr) {
    GameSfx(1, &plr->pos);
    AddMaskFeathers(plr->mask);
    plr->mask->active--;
    plr->f17A = 0x96;
    plr->mom.x = plr->mom.z = 0.0f;
    NewBuzz(player->obj.rumble, 0x19);
    NewRumble(player->obj.rumble, 0x7F);
}

/* vtog_s: stride 0x84 (VTog[i] -> +0x84 in ResetVehicleControl's loop).
 * Fields verified in InitVehicleToggles; iRAIL/iALONG/fALONG are a packed copy
 * of TempRPos at +0x75/+0x76/+0x78 (NOT an rpos_s). */
struct vtog_s {
    struct nugspline_s *spl;  /* 0x0 */
    struct nugspline_s *cam;  /* 0x4 */
    struct nugspline_s *look; /* 0x8 */
    struct numtx_s mtx;       /* 0xC */
    struct nuvec_s pos;      /* 0x4C */
    struct animpkt_s anim;   /* 0x58 */
    signed char type;  /* 0x74 */
    signed char iRAIL; /* 0x75 */
    short iALONG;      /* 0x76 */
    float fALONG;      /* 0x78 */
    u16 xrot;          /* 0x7C */
    u16 angle;         /* 0x7E */
    u16 zrot;          /* 0x80 */
    char pad_82[0x84 - 0x82];
};

extern struct vtog_s VTog[];
extern s32 vtog_action;
extern struct rpos_s TempRPos;
extern struct nuvec_s ShadNorm;
extern u16 temp_xrot;
extern u16 temp_zrot;
extern float D_0062D2E8;
extern char D_0061D7F8[]; /* "vehicle_trigger_%.2i_in" */
extern char D_0061D810[]; /* "vehicle_trigger_%.2i_out" */
extern char D_0061D830[]; /* "vehicle_cam_%.2i" */
extern char D_0061D848[]; /* "vehicle_look_%.2i" */
extern void GetALONG(struct nuvec_s *pos, struct rpos_s *rpos, s32 iRAIL, s32 iALONG,
                     s32 dir);
extern float NewShadowMask(struct nuvec_s *pos, float y, s32 flag);
extern void FindAnglesZX(struct nuvec_s *norm);

void InitVehicleToggles(void) {
    struct vtog_s *vt;
    float y;
    s32 vehicle;
    s32 i;

    vehicle = LDATA->vehicle;
    if (vehicle == 0x44 || vehicle == 0xB2 || vehicle == 0x3B || vehicle == 0x20) {
        vtog_action = 0x62;
    } else if (vehicle == -1) {
        vtog_action = -1;
    } else {
        vtog_action = 0x68;
    }
    vt = VTog;
    for (i = 0; i < 6; i++, vt++) {
        vt->spl = NULL;
        if (world_scene[0] == NULL) {
            continue;
        }
        sprintf(tbuf, D_0061D7F8, i);
        vt->spl = NuSplineFind(world_scene[0], tbuf);
        if (vt->spl != NULL) {
            vt->type = 0;
        } else {
            sprintf(tbuf, D_0061D810, i);
            vt->spl = NuSplineFind(world_scene[0], tbuf);
            if (vt->spl == NULL) {
                continue;
            }
            vt->type = 1;
        }
        if (vt->spl->len != 3) {
            vt->spl = NULL;
            continue;
        }
        vt->pos = *(struct nuvec_s *)(vt->spl->pts + vt->spl->ptsize * 2);
        GetALONG(&vt->pos, NULL, -1, -1, 2);
        vt->iRAIL = TempRPos.iRAIL;
        vt->iALONG = TempRPos.iALONG;
        vt->fALONG = TempRPos.fALONG;
        vt->angle = TempRPos.angle;
        if (vt->type != 0 || (LBIT & 0x10080010) == 0) {
            y = NewShadowMask(&vt->pos, 0.0f, -1);
            if (y != D_0062D2E8) {
                vt->pos.y = y;
                FindAnglesZX(&ShadNorm);
                vt->xrot = temp_xrot;
                vt->zrot = temp_zrot;
            }
        }
        sprintf(tbuf, D_0061D830, i);
        vt->cam = NuSplineFind(world_scene[0], tbuf);
        sprintf(tbuf, D_0061D848, i);
        vt->look = NuSplineFind(world_scene[0], tbuf);
    }
}

extern s32 FurtherBEHIND(s32 iRAIL0, s32 iALONG0, float fALONG0, s32 iRAIL1,
                         s32 iALONG1, float fALONG1);
extern struct vtog_s D_0058AF24[];
extern float vtog_time;
extern float vtog_duration;

void ResetVehicleControl(s32 iRAIL, s32 iALONG, float fALONG) {
    struct vtog_s *vt;
    s32 i;
    s32 hit;
    s32 done;

    hit = Level != 0xF;
    done = 0;
    for (i = 0, vt = VTog; i < 6; i++, vt++) {
        if (vt->spl != NULL &&
            FurtherALONG(vt->iRAIL, vt->iALONG, vt->fALONG, iRAIL, iALONG,
                         fALONG)) {
            if (Level == 0xF && i >= 2) {
                if (!hit) {
                    if (vt->type == 1) {
                        SKELETALCRASH = 1;
                    } else {
                        SKELETALCRASH = 0;
                    }
                    hit = 1;
                }
            } else {
                if (!done) {
                    if (vt->type == 1) {
                        VEHICLECONTROL = 1;
                    } else if ((LBIT & 0x400000040ULL) != 0) {
                        VEHICLECONTROL = 2;
                    } else {
                        VEHICLECONTROL = 0;
                    }
                }
                done = 1;
            }
        }
        if (hit && done) {
            goto end;
        }
    }

    for (i = 5, vt = D_0058AF24; i >= 0; i--, vt--) {
        if (vt->spl != NULL &&
            FurtherBEHIND(vt->iRAIL, vt->iALONG, vt->fALONG, iRAIL, iALONG,
                          fALONG)) {
            if (Level == 0xF && i >= 2) {
                if (!hit) {
                    if (vt->type == 1) {
                        SKELETALCRASH = 0;
                    } else {
                        SKELETALCRASH = 1;
                    }
                    hit = 1;
                }
            } else {
                if (!done) {
                    if (vt->type == 1) {
                        if ((LBIT & 0x400000040ULL) != 0) {
                            VEHICLECONTROL = 2;
                        } else {
                            VEHICLECONTROL = 0;
                        }
                    } else {
                        VEHICLECONTROL = 1;
                    }
                }
                done = 1;
            }
        }
        if (hit && done) {
            goto end;
        }
    }

end:
    vtog_time = 0.0f;
    vtog_duration = 0.0f;
}

/* CData entry, stride 0x34 */
struct chardata_s {
    char pad_0[0x14];
    struct nuvec_s min; /* 0x14 */
    struct nuvec_s max; /* 0x20 */
    float scale;        /* 0x2C */
    float shadow_scale; /* 0x30 */
};

extern struct chardata_s CData[];
extern struct numtx_s mTEMP;
extern struct nuvec_s AshesMechInPos;
extern struct vtog_s *pVTog;
extern s32 vtog_blend;
extern u16 vtog_angle;
extern float D_0062D2FC;
extern float NewShadow(struct nuvec_s *pos, float y);
extern void NuMtxSetRotationY(struct numtx_s *m, s32 ang);
extern s32 DrawCharacterModel(struct cmodel_s *model, struct animpkt_s *anim,
                              struct numtx_s *mC, struct numtx_s *mS, s32 render,
                              struct numtx_s *mR, struct numtx_s *loc_mtx,
                              struct nuvec_s *loc_mom, void *obj);

extern struct nuvec_s vtog_oldpos;
extern struct nuvec_s vtog_newpos;
extern struct nuvec_s AshesMechOutPos;
extern float AshesMechInZ;
extern float AshesMechOutZ;
extern float D_0062D2EC;
extern float D_0062D2F0;
extern float D_0062D2F4;
extern float D_0062D2F8;
extern void ResetPlayerMoves(struct bcreature_s *c);
extern void AddMechanicalDebris(struct nuvec_s *pos, s32 vehicle);
extern void BlendGameCamera(struct gamecam_s *cam, float t);
extern void JudderGameCamera(struct gamecam_s *cam, struct nuvec_s *pos, float amt);

void ToggleVehicle(struct bcreature_s *c) {
    struct nuvec_s p0;
    struct nuvec_s p1;
    struct nuvec_s oldpos;
    struct nuvec_s newpos;
    struct nuvec_s pos;
    struct vtog_s *vt;
    struct nugspline_s *spl;
    struct nuvec_s *pt0;
    struct nuvec_s *pt1;
    float y;
    s32 mirror;
    s32 oldside;
    s32 newside;
    s32 i;

    if (c->obj.dead != 0) {
        return;
    }
    mirror = 0;
    if (Level == 6 || Level == 0x22) {
        mirror = 1;
    }
    if (mirror) {
        oldpos.x = -c->obj.oldpos.y;
        oldpos.y = c->obj.oldpos.x;
        oldpos.z = c->obj.oldpos.z;
        newpos.x = -c->obj.pos.y;
        newpos.y = c->obj.pos.x;
        newpos.z = c->obj.pos.z;
    } else {
        oldpos = c->obj.oldpos;
        newpos = c->obj.pos;
    }
    if (Level == 0x16) {
        if (VEHICLECONTROL == 0 && oldpos.z >= AshesMechInZ && newpos.z < AshesMechInZ) {
            VEHICLECONTROL = 1;
            vtog_oldpos = c->obj.pos;
            ResetPlayerMoves(c);
            vtog_time = 0.0f;
            vtog_duration = 1.0f;
            vtog_newpos = AshesMechInPos;
            y = NewShadow(&vtog_newpos, vtog_time);
            if (y != D_0062D2EC) {
                vtog_newpos.y = y - c->obj.bot * c->obj.SCALE;
            }
            vtog_blend = 1;
            vtog_angle = 0;
            pVTog = NULL;
            AddGameDebris(0x10, &c->obj.pos);
            AddGameDebris(0x11, &c->obj.pos);
            GameSfx(0x2F, &c->obj.pos);
            return;
        }
        if (VEHICLECONTROL == 1 && oldpos.z < AshesMechOutZ && AshesMechOutZ <= newpos.z) {
            VEHICLECONTROL = 0;
            vtog_oldpos = c->obj.pos;
            ResetPlayerMoves(c);
            vtog_time = 0.0f;
            vtog_duration = 1.0f;
            vtog_newpos = AshesMechOutPos;
            y = NewShadow(&vtog_newpos, vtog_time);
            if (y != D_0062D2F0) {
                vtog_newpos.y = y - c->obj.bot * c->obj.SCALE;
            }
            vtog_blend = 1;
            vtog_angle = 0;
            pVTog = NULL;
            AddGameDebris(0x10, &c->obj.pos);
            AddGameDebris(0x11, &c->obj.pos);
            GameSfx(0x2F, &c->obj.pos);
            pos.x = c->obj.pos.x;
            pos.y = c->obj.pos.y + (c->obj.bot + c->obj.top) * c->obj.SCALE * 0.5f;
            pos.z = c->obj.pos.z;
            AddMechanicalDebris(&pos, 0xB2);
            GameSfx(0x3B, &c->obj.pos);
            JudderGameCamera(&GameCam, NULL, D_0062D2F4);
            return;
        }
    }
    for (i = 0, vt = VTog; i < 6; i++, vt++) {
        spl = vt->spl;
        if (spl == NULL) {
            continue;
        }
        if (Level != 0xF || i < 2) {
            if (vt->type == 0 && VEHICLECONTROL == 1) {
                continue;
            }
            if (vt->type == 1 && VEHICLECONTROL != 1) {
                continue;
            }
        }
        pt0 = (struct nuvec_s *)spl->pts;
        pt1 = (struct nuvec_s *)(spl->pts + spl->ptsize);
        if (mirror) {
            p0.x = -pt0->y;
            p0.y = pt0->x;
            p0.z = pt0->z;
            p1.x = -pt1->y;
            p1.y = pt1->x;
            p1.z = pt1->z;
        } else {
            p0 = *pt0;
            p1 = *pt1;
        }
        oldside = (oldpos.x - p0.x) * (p1.z - p0.z) + (oldpos.z - p0.z) * (p0.x - p1.x) >= 0.0f;
        newside = (newpos.x - p0.x) * (p1.z - p0.z) + (newpos.z - p0.z) * (p0.x - p1.x) >= 0.0f;
        if (oldside && !newside &&
            (newpos.x - oldpos.x) * (p0.z - oldpos.z) + (newpos.z - oldpos.z) * (oldpos.x - p0.x) >= 0.0f &&
            (newpos.x - p1.x) * (oldpos.z - p1.z) + (newpos.z - p1.z) * (p1.x - oldpos.x) >= 0.0f) {
            if (Level == 0xF && i >= 2) {
                if (vt->type == 0) {
                    SKELETALCRASH = 1;
                    GameSfx(0x1E, NULL);
                } else {
                    SKELETALCRASH = 0;
                    GameSfx(0x21, NULL);
                }
                return;
            }
            vtog_oldpos = c->obj.pos;
            if (vt->type == 0) {
                VEHICLECONTROL = 1;
            } else if ((LBIT & 0x400000040ULL) != 0) {
                VEHICLECONTROL = 2;
            } else {
                VEHICLECONTROL = 0;
            }
            ResetPlayerMoves(c);
            vtog_newpos.x = vt->pos.x;
            vtog_newpos.y = vt->pos.y - c->obj.bot * c->obj.SCALE;
            vtog_duration = 1.0f;
            vtog_newpos.z = vt->pos.z;
            vtog_time = 0.0f;
            if (mirror && pos_START != NULL) {
                vtog_newpos.x = pos_START->x;
            }
            vtog_blend = 1;
            vt->mtx = c->obj.mtx;
            if (vt->type == 0) {
                ResetAnimPacket(&vt->anim, vtog_action);
            } else {
                vt->anim = c->obj.anim;
            }
            if (c->obj.vehicle == 0xA1) {
                BlendGameCamera(&GameCam, 1.0f);
            } else if (vt->cam != NULL && vtog_blend != 0) {
                GameCam.mode = -1;
            }
            if (vt->type == 0) {
                AddGameDebris(0x10, &c->obj.pos);
                AddGameDebris(0x11, &c->obj.pos);
                GameSfx(0x2F, &c->obj.pos);
                if (Level == 4) {
                    c->obj.f185 = 0;
                }
            } else {
                AddGameDebris(0x10, &c->obj.pos);
                AddGameDebris(0x11, &c->obj.pos);
                GameSfx(0x2F, &c->obj.pos);
                pos.x = c->obj.pos.x;
                pos.y = c->obj.pos.y + (c->obj.bot + c->obj.top) * c->obj.SCALE * 0.5f;
                pos.z = c->obj.pos.z;
                switch (c->obj.vehicle) {
                case 0x3B:
                case 0x44:
                case 0x63:
                case 0x99:
                case 0xB2:
                    GameSfx(0x69, &pos);
                    AddMechanicalDebris(&pos, c->obj.vehicle);
                    break;
                case 0x20:
                    AddMechanicalDebris(&pos, 0x20);
                    break;
                }
                GameSfx(0x3B, &c->obj.pos);
                JudderGameCamera(&GameCam, NULL, D_0062D2F8);
            }
            pVTog = vt;
            vtog_angle = vt->angle;
            return;
        }
        if (!oldside && newside &&
            (newpos.x - oldpos.x) * (p1.z - oldpos.z) + (newpos.z - oldpos.z) * (oldpos.x - p1.x) >= 0.0f &&
            (newpos.x - p0.x) * (oldpos.z - p0.z) + (newpos.z - p0.z) * (p0.x - oldpos.x) >= 0.0f) {
            if (Level == 0xF && i >= 2) {
                if (vt->type == 0) {
                    SKELETALCRASH = 0;
                    GameSfx(0x21, NULL);
                } else {
                    SKELETALCRASH = 1;
                    GameSfx(0x1E, NULL);
                }
            }
            return;
        }
    }
}

void DrawNextVehicle(s32 render) {
    struct nuvec_s pos;
    struct animpkt_s anim;
    struct cmodel_s *model;
    struct vtog_s *vt;
    float y;
    s32 vehicle;
    s32 i;

    vehicle = player->obj.vehicle;
    if (vehicle == -1) {
        return;
    }
    i = CRemap[vehicle];
    if (i == -1) {
        return;
    }
    model = &CModel[i];
    if (Level == 0x16 && (VEHICLECONTROL == 0 || vtog_time < vtog_duration)) {
        pos = AshesMechInPos;
        y = NewShadow(&pos, 0.0f);
        if (y != D_0062D2FC) {
            pos.y = y;
            FindAnglesZX(&ShadNorm);
        } else {
            temp_zrot = 0;
            temp_xrot = 0;
        }
        NuMtxSetRotationY(&mTEMP, vtog_angle + 0x8000);
        NuMtxRotateZ(&mTEMP, temp_zrot);
        NuMtxRotateX(&mTEMP, temp_xrot);
        NuMtxTranslate(&mTEMP, &pos);
        ResetAnimPacket(&anim, 0x62);
        DrawCharacterModel(model, &anim, &mTEMP, NULL, render, NULL, NULL, NULL, NULL);
    }
    for (i = 0, vt = VTog; i < 6; i++, vt++) {
        if (Level == 0xF && i >= 2) {
            continue;
        }
        if ((vtog_time < vtog_duration && vtog_blend != 0 && vt == pVTog) ||
            (best_cRPos != NULL && VEHICLECONTROL != 1 && vt->spl != NULL &&
             FurtherALONG(vt->iRAIL, vt->iALONG, vt->fALONG, best_cRPos->iRAIL,
                          best_cRPos->iALONG, best_cRPos->fALONG))) {
            if (vt->type != 0) {
                return;
            }
            NuMtxSetIdentity(&mTEMP);
            NuMtxRotateY(&mTEMP, vt->angle + 0x8000);
            NuMtxRotateZ(&mTEMP, vt->zrot);
            NuMtxRotateX(&mTEMP, vt->xrot);
            pos.x = vt->pos.x;
            pos.y = vt->pos.y - CData[vehicle].min.y * CData[vehicle].scale;
            pos.z = vt->pos.z;
            NuMtxTranslate(&mTEMP, &vt->pos);
            ResetAnimPacket(&vt->anim, vtog_action);
            DrawCharacterModel(model, &vt->anim, &mTEMP, NULL, render, NULL, NULL, NULL,
                               NULL);
            return;
        }
    }
}

extern void JudderGameCamera(struct gamecam_s *cam, struct nuvec_s *pos, float amt);

/* Only the fields AddKaboom reads are known (lhu 0x5A / lhu 0x14). */
/* CrateCube subset, stride 0x90 */
struct cratecube_s {
    char pad_0[0x10];
    struct nuvec_s pos;   /* 0x10 */
    char pad_1c[0x30 - 0x1C];
    signed char on;       /* 0x30 */
    char pad_31[0x3C - 0x31];
    signed char type3;    /* 0x3C */
    char pad_3d;
    signed char newtype;  /* 0x3E */
    char pad_3f[0x42 - 0x3F];
    signed char appeared; /* 0x42 */
    char pad_43[0x56 - 0x43];
    short trigger;        /* 0x56 */
    char pad_58[0x5A - 0x58];
    u16 index;            /* 0x5A */
    char pad_5c[0x76 - 0x5C];
    short action;         /* 0x76 */
    char pad_78[0x90 - 0x78];
};

/* CrateCubeGroup, stride 0x30 */
struct crategroup_s {
    struct nuvec_s origin;  /* 0x0 */
    float radius;           /* 0xC */
    short iCrate;           /* 0x10 */
    short nCrates;          /* 0x12 */
    u16 angle;              /* 0x14 */
    char pad_16[2];
    struct nuvec_s minclip; /* 0x18 */
    struct nuvec_s maxclip; /* 0x24 */
};

extern struct cratecube_s *temp_pCrate;
extern struct crategroup_s *temp_pGroup;

void AddKaboom(s32 type, struct nuvec_s *pos, float t) {
    struct kaboom_s *k;
    s32 sfx;
    float rate;

    k = &Kaboom[i_kaboom];
    sfx = -1;
    k->time = 0.0f;
    k->type = type;
    k->pos = *pos;

    switch (type) {
    case 1:
    case 2:
        k->t0 = 0.0f;
        k->t1 = 1.25f;
        rate = 3.0f;
        break;
    case 4:
        k->t1 = 1.25f;
        k->t0 = 0.0f;
        rate = 3.0f;
        JudderGameCamera(&GameCam, 0, 0.2f);
        break;
    case 8:
        k->t1 = 1.75f;
        k->t0 = 0.0f;
        rate = 4.2f;
        JudderGameCamera(&GameCam, 0, 0.4f);
        break;
    case 0x10:
        k->t1 = 3.0f;
        k->t0 = 0.0f;
        rate = 7.5f;
        JudderGameCamera(&GameCam, &k->pos, 0.3f);
        break;
    case 0x80:
    case 0x100:
        if (type == 0x80) {
            sfx = 0x3B;
        }
        k->t1 = 1.0f;
        k->t0 = 0.0f;
        rate = 3.0f;
        JudderGameCamera(&GameCam, &k->pos, 0.3f);
        break;
    case 0x20:
    case 0x40:
        k->t0 = 0.0f;
        k->t1 = 50.0f;
        rate = 15.0f;
        k->crate = temp_pCrate->index;
        k->group = temp_pGroup->angle;
        break;
    }

    if (0.0f < t) {
        k->t1 = t;
    }
    k->duration = (k->t1 - k->t0) / rate;
    i_kaboom++;
    if (i_kaboom == 0x48) {
        i_kaboom = 0;
    }
    if (sfx != -1) {
        GameSfx(sfx, &k->pos);
    }
}

extern struct crategroup_s CrateGroup[];
extern struct cratecube_s Crate[];
extern s32 CRATEGROUPCOUNT;
extern float CRATEHOPSPEED;
extern struct tobj_s *pObj[64];
extern float D_0062D314; /* kaboom time step */
extern s32 GetCrateType(struct cratecube_s *crate, s32 flags);
extern s32 NewCrateAnimation(struct cratecube_s *crate, s32 character, s32 action, s32 flag);
extern s32 CrateOff(struct crategroup_s *group, struct cratecube_s *crate, s32 type, s32 b);
extern void StartExclamationCrateSequence(struct crategroup_s *group, struct cratecube_s *crate);
extern void DestroyAllNitroCrates(struct crategroup_s *group, struct cratecube_s *crate);
extern void HopCratesAbove(float speed, struct crategroup_s *group, struct cratecube_s *crate);
extern s32 KillPlayer(struct tobj_s *obj, s32 anim);
extern void PickupItem(struct tobj_s *obj);
extern void FlyGameObject(struct tobj_s *obj, u16 yrot);
extern s32 KillGameObject(struct tobj_s *obj, s32 anim);

void UpdateKabooms(void) {
    struct nuvec_s pos;
    struct nuvec_s d;
    s32 key;
    struct kaboom_s *k;
    struct crategroup_s *group;
    struct cratecube_s *crate;
    struct tobj_s *obj;
    struct wumpa_s *wumpa;
    float r;
    float r2;
    s32 type;
    s32 i;
    s32 j;
    s32 n;

    k = Kaboom;
    for (i = 0; i < 0x48; i++, k++) {
        if (!(k->time < k->duration)) {
            continue;
        }
        if ((k->type & 0x40) != 0) {
            continue;
        }
        r = k->t0 + (k->t1 - k->t0) * (k->time / k->duration);
        r2 = r * r;
        group = CrateGroup;
        for (j = 0; j < CRATEGROUPCOUNT; j++, group++) {
            if (k->pos.x < group->minclip.x - r || group->maxclip.x + r < k->pos.x ||
                k->pos.z < group->minclip.z - r || group->maxclip.z + r < k->pos.z ||
                k->pos.y < group->minclip.y - r || group->maxclip.y + r < k->pos.y) {
                continue;
            }
            crate = &Crate[group->iCrate];
            for (n = 0; n < group->nCrates; n++, crate++) {
                if (crate->on == 0) {
                    continue;
                }
                type = GetCrateType(crate, 0);
                if (type == 0x13 && (k->type & 0xC) == 0) {
                    NewCrateAnimation(crate, 0x13, 0x58, 0);
                    GameSfx(0x39, &crate->pos);
                    continue;
                }
                if (!((k->type == 0x20 && type == 0 && crate->type3 != -1 && k->crate != -1 &&
                       crate->trigger == k->crate) ||
                      (k->type != 0x20 && type != 0))) {
                    continue;
                }
                pos.x = crate->pos.x;
                pos.y = crate->pos.y + 0.25f;
                pos.z = crate->pos.z;
                NuVecSub(&d, &k->pos, &pos);
                if (!(d.x * d.x + d.y * d.y + d.z * d.z < r2)) {
                    continue;
                }
                if (k->type == 0x20) {
                    crate->newtype = crate->type3;
                    if (crate->type3 == 3) {
                        NewCrateAnimation(crate, 3, 0x22, 1);
                    }
                    if (crate->newtype != 0) {
                        crate->appeared = 1;
                    }
                } else if (type == 0xE) {
                    if (crate->action == -1 && NewCrateAnimation(crate, 0xE, 0x35, 0) == 0) {
                        StartExclamationCrateSequence(group, crate);
                    }
                } else if (type == 0x11) {
                    if (crate->action == -1 && NewCrateAnimation(crate, 0x11, 0x35, 0) == 0) {
                        DestroyAllNitroCrates(group, crate);
                    }
                } else if (CrateOff(group, crate, k->type, 0) != 0) {
                    HopCratesAbove(CRATEHOPSPEED, group, crate);
                }
            }
        }
        if (k->type != 0x20) {
            for (j = 0; j < 64; j++) {
                obj = pObj[j];
                if (obj == NULL || obj->dead != 0 || obj->invincible != 0) {
                    continue;
                }
                if ((obj->flags & 1) != 0 && (k->type & 0x18C) != 0) {
                    continue;
                }
                if ((obj->flags & 4) != 0) {
                    if ((k->type & 0x10) != 0) {
                        continue;
                    }
                    if (k->type == 4 && (obj->kaboom_mask & 4) == 0) {
                        continue;
                    }
                    if (k->type == 8 && (obj->kaboom_mask & 8) == 0) {
                        continue;
                    }
                }
                if ((obj->flags & 0x10) != 0 && (k->type & 0xC) == 0) {
                    continue;
                }
                if ((obj->flags & 8) != 0) {
                    continue;
                }
                pos.x = obj->pos.x;
                pos.y = obj->pos.y + (obj->bot + obj->top) * obj->SCALE * 0.5f;
                pos.z = obj->pos.z;
                NuVecSub(&d, &pos, &k->pos);
                if (!(d.x * d.x + d.y * d.y + d.z * d.z < r2)) {
                    continue;
                }
                if ((obj->flags & 1) != 0) {
                    KillPlayer(obj, 6);
                } else if ((obj->flags & 0x10) != 0) {
                    PickupItem(obj);
                } else if ((obj->flags & 0x40004) == 0x40004) {
                    obj->exploded = 1;
                } else {
                    FlyGameObject(obj, NuAtan2D(d.x, d.z));
                    KillGameObject(obj, 1);
                }
            }
            if (k->type != 0x20 && TimeTrial == 0) {
                wumpa = Wumpa;
                for (j = 0; j < 320; j++, wumpa++) {
                    if (wumpa->field_40 != 2 &&
                        (wumpa->field_40 != 1 || (k->type & 0xC) != 0)) {
                        continue;
                    }
                    NuVecSub(&d, &k->pos, &wumpa->pos);
                    if (!(d.x * d.x + d.y * d.y + d.z * d.z < r2)) {
                        continue;
                    }
                    if ((k->type & 0xC) != 0) {
                        if (player->obj.dead == 0) {
                            AddScreenWumpa(wumpa->pos.x, wumpa->pos.y, wumpa->pos.z, 1);
                        }
                        wumpa->field_40 = 0;
                        key = -1;
                        AddFiniteShotDebrisEffect(&key, GDeb[520], &wumpa->pos, 1);
                    } else {
                        FlyWumpa(wumpa);
                    }
                }
            }
        }
        k->time += D_0062D314;
        if (k->time > k->duration) {
            k->time = k->duration;
        }
    }
}

void DrawKabooms(void) {
    struct nuvec_s scale;
    struct kaboom_s *k;
    float r;
    u16 ang;
    s32 nextang;
    s32 object;
    s32 i;
    s32 j;

    k = Kaboom;
    for (i = 0; i < 0x48; i++, k++) {
        if (!(k->time < k->duration)) {
            continue;
        }
        if ((k->type & 0x181) != 0) {
            object = 0xB;
        } else if ((k->type & 2) != 0) {
            object = 0xC;
        } else if ((k->type & 0x1C) != 0) {
            object = 0xD;
        } else if ((k->type & 0x60) != 0) {
            object = 0xF;
        } else {
            continue;
        }
        if (ObjTab[object].obj.special == NULL) {
            continue;
        }
        if (object == 0xF) {
            scale.x = scale.y = scale.z = (k->duration - k->time) / k->duration;
            r = k->t0 + (k->t1 - k->t0) * (k->time / k->duration);
            ang = k->group;
            for (j = 0; j < 4; j++) {
                NuMtxSetScale(&mTEMP, &scale);
                nextang = ang + 0x4000;
                NuMtxRotateY(&mTEMP, ang);
                mTEMP.m[3][0] = k->pos.x + NuTrigTable[ang] * r;
                mTEMP.m[3][1] = k->pos.y;
                mTEMP.m[3][2] = k->pos.z + NuTrigTable[nextang & 0xFFFF] * r;
                NuRndrGScnObj(ObjTab[object].obj.scene->gobjs[ObjTab[object].obj.special->instance->objid],
                              &mTEMP);
                ang = nextang;
            }
        } else {
            scale.x = scale.y = scale.z =
                k->t0 + (k->t1 - k->t0) * (k->time / k->duration);
            NuMtxSetScale(&mTEMP, &scale);
            NuMtxTranslate(&mTEMP, &k->pos);
            NuRndrGScnObj(ObjTab[object].obj.scene->gobjs[ObjTab[object].obj.special->instance->objid],
                          &mTEMP);
        }
    }
}

/* race opponent state (OppTub/PlrTub) */
struct tubinfo_s {
    void *c;            /* 0x0 */
    s8 track;           /* 0x4 */
    s8 count;           /* 0x5 */
    u8 wait;            /* 0x6 */
    s8 laps;            /* 0x7 */
    float time;         /* 0x8 */
    float duration;     /* 0xC */
    float lap_position; /* 0x10 */
    s8 place;           /* 0x14 */
};

extern struct tubinfo_s PlrTub;
extern struct nuvec_s finish_newpos;
extern struct nuvec_s finish_oldpos;
extern s32 finish_type;
extern float FINISHRADIUSLEVEL;
extern float FlyingLevelCompleteTimer;
extern s32 ChrisBigBossDead;
extern s32 SmokeyFinished;
extern s32 SmokeyPosition;
extern s32 MAXLAPS;
extern s32 i_ring;
extern s32 RINGCOUNT;
extern s32 carpet_place;
extern s32 DESTRUCTIBLECRATECOUNT;
extern float TT_RELICX;
extern float TT_RELICY;
extern float D_0062D318;
extern float FindNearestCreature(struct nuvec_s *pos, s32 type, struct nuvec_s *out);
extern s32 GetCurrentLevelObjectives(void);
extern void PickupCrateGem(void);
extern void PickupCrystal(void);
extern void PickupBonusGem(s32 mask);
extern void PickupPower(s32 type);

void CheckFinish(struct tobj_s *obj) {
    struct nuvec_s pos;
    struct nuvec_s nearest;
    float dist;
    u16 lev_flags;
    s32 hub_flags;

    if (Level == 0x25 || obj->dead != 0 || Hub == -1) {
        return;
    }
    pos.x = obj->pos.x;
    pos.y = obj->pos.y + (obj->bot + obj->top) * obj->SCALE * 0.5f;
    pos.z = obj->pos.z;
    if (obj->finished == 0) {
        hub_flags = Game.hub[Hub].flags;
        lev_flags = Game.level[Level].flags;
        new_hub_flags = 0;
        new_lev_flags = 0;
        finish_type = 0;
        switch (Level) {
        default:
            if (Level == 0x24) {
                if (GetCurrentLevelObjectives() == 0) {
                    VEHICLECONTROL = 0;
                    ResetPlayer(1);
                    level_part_2 = 0;
                }
                return;
            }
            pos.y += 0.5f;
            dist = FindNearestCreature(&pos, 0xB1, &nearest);
            if (FINISHRADIUSLEVEL * FINISHRADIUSLEVEL < dist) {
                in_finish_range = 0;
                return;
            }
            in_finish_pos = nearest;
            finish_newpos = nearest;
            in_finish_range = 50;
            finish_oldpos = obj->pos;
            finish_type = 1;
            finish_frames = 50;
            break;

        case 0xD:
        case 0x12:
        case 0x1A:
            if (TimeTrial == 0 && (Game.level[Level].flags & 0x10) == 0 &&
                (plr_items & 2) == 0 && plr_crates.count >= DESTRUCTIBLECRATECOUNT) {
                PickupCrateGem();
            }
            if (FlyingLevelCompleteTimer != 0.0f) {
                return;
            }
            finish_newpos = obj->pos;
            if ((Game.level[Level].flags & 8) == 0) {
                PickupCrystal();
            }
            finish_frames = 50;
            break;
        case 0x15:
        case 0x16:
        case 0x17:
        case 0x18:
        case 0x19:
            if ((Level == 0x18 || Level == 0x16 || Level == 0x15) && ChrisBigBossDead == 0) {
            } else if (Level != 0x19 && Level != 0x17 && GetCurrentLevelObjectives() == 0) {
                if ((Game.hub[Hub].flags & 4) == 0) {
                    if (boss_dead <= 0) {
                        boss_dead = 1;
                        if (Level == 0x18) {
                            PickupPower(0xA2);
                        }
                    }
                } else {
                    boss_dead = 2;
                }
            }
            if (boss_dead < 2) {
                return;
            }
            if (((struct cursor_s *)Cursor)->menu != -1) {
                return;
            }
            in_finish_range++;
            if (in_finish_range < 50) {
                return;
            }
            finish_newpos = obj->pos;
            finish_frames = 50;
            if ((Game.hub[Hub].flags & 6) == 2) {
                Game.hub[Hub].flags |= 4;
                Game.hub[Hub + 1].flags |= 1;
            }
            break;
        case 0x24:
            if (TimeTrial == 0 && (Game.level[Level].flags & 0x10) == 0 &&
                (plr_items & 2) == 0 && plr_crates.count >= DESTRUCTIBLECRATECOUNT) {
                PickupCrateGem();
            }
            return;
        case 0x3:
            if (TimeTrial == 0 && (Game.level[Level].flags & 0x10) == 0 &&
                (plr_items & 2) == 0 && plr_crates.count >= DESTRUCTIBLECRATECOUNT) {
                PickupCrateGem();
            }
            if (SmokeyFinished == 0) {
                return;
            }
            if (in_finish_range == 0) {
                GameSfx(0x69, &player->obj.pos);
            }
            if (TimeTrial == 0 && SmokeyPosition == 1 && (Game.level[Level].flags & 8) == 0 &&
                (plr_items & 1) == 0) {
                PickupCrystal();
            }
            in_finish_range++;
            if (in_finish_range < 50) {
                return;
            }
            finish_newpos = obj->pos;
            finish_frames = 50;
            break;
        case 0x1C:
            if (TimeTrial == 0 && (Game.level[Level].flags & 0x10) == 0 &&
                (plr_items & 2) == 0 && plr_crates.count >= DESTRUCTIBLECRATECOUNT) {
                PickupCrateGem();
            }
            if (PlrTub.laps < MAXLAPS) {
                return;
            }
            in_finish_range++;
            if (in_finish_range < 50) {
                return;
            }
            finish_newpos = obj->pos;
            if (TimeTrial == 0 && PlrTub.place == 1 && (Game.level[Level].flags & 0x20) == 0) {
                PickupBonusGem(4);
            }
            finish_frames = 50;
            break;
        case 0x1D:
            if (TimeTrial == 0 && (Game.level[Level].flags & 0x10) == 0 &&
                (plr_items & 2) == 0 && plr_crates.count >= DESTRUCTIBLECRATECOUNT) {
                PickupCrateGem();
            }
            if (i_ring < RINGCOUNT) {
                return;
            }
            in_finish_range++;
            if (in_finish_range < 50) {
                return;
            }
            finish_newpos = obj->pos;
            if (TimeTrial == 0 && carpet_place == 2 && (Game.level[Level].flags & 0x20) == 0) {
                PickupBonusGem(4);
            }
            finish_frames = 50;
            break;
        }
        if ((Level == 3 && SmokeyPosition >= 2) || (Level == 0x1D && carpet_place == 1) ||
            (Level == 0x1C && PlrTub.place >= 2)) {
            obj->finished = 2;
        } else {
            obj->finished = 1;
        }
        AddWarpDebris(obj, NULL);
        gamesfx_effect_volume = 0x7FFE;
        GameSfx(0x1E, &obj->pos);
        if ((plr_items & 1) != 0) {
            Game.level[Level].flags |= 8;
            if (Game.hub[Hub].crystals < 5) {
                Game.hub[Hub].crystals++;
                if (Game.hub[Hub].crystals == 5) {
                    Game.hub[Hub].flags |= 2;
                }
            }
        }
        if ((LBIT & 0x3E00000) != 0) {
            Game.level[Level].flags |= 0x800;
        } else if (TimeTrial != 0) {
            if (obj->finished == 1) {
                NewLevelTime(TimeTrialTimer.itime);
                if (TimeTrialTimer.itime < LDATA->time[2]) {
                    Game.level[Level].flags |= 7;
                } else if (TimeTrialTimer.itime < LDATA->time[1]) {
                    Game.level[Level].flags |= 3;
                } else if (TimeTrialTimer.itime < LDATA->time[0]) {
                    Game.level[Level].flags |= 1;
                }
                if (finish_frames < 0x96) {
                    finish_frames = 0x96;
                }
            } else {
                finish_frames = 50;
            }
        } else {
            if ((plr_items & 2) != 0) {
                Game.level[Level].flags |= 0x10;
            }
            if ((plr_items & 4) != 0) {
                Game.level[Level].flags |= 0x20;
            }
            if ((plr_items & 8) != 0) {
                Game.level[Level].flags |= 0x40;
            }
            if ((plr_items & 0x20) != 0) {
                Game.level[Level].flags |= 0x80;
            }
            if ((plr_items & 0x10) != 0) {
                Game.level[Level].flags |= 0x100;
            }
            if ((plr_items & 0x40) != 0) {
                Game.level[Level].flags |= 0x200;
            }
            if ((plr_items & 0x80) != 0) {
                Game.level[Level].flags |= 0x400;
            }
        }
        new_hub_flags = Game.hub[Hub].flags ^ hub_flags;
        new_lev_flags = Game.level[Level].flags ^ lev_flags;
        Game.level[Level].flags ^= new_lev_flags;
        if (obj->finished == 0) {
            return;
        }
    }
    finish_frame++;
    if (finish_frame != finish_frames) {
        return;
    }
    if (obj->finished == 1) {
        if ((LBIT & 0x3E00000) != 0 && (new_lev_flags & 0x800) != 0) {
            new_lev_flags ^= 0x800;
            Game.level[Level].flags |= 0x800;
        }
        if (TimeTrial != 0) {
            if (newleveltime_slot < 3) {
                if ((new_lev_flags & 7) != 0) {
                    GameSfx(0x26, NULL);
                    AddPanelDebris(TT_RELICX, TT_RELICY, 6, 0.125f, 0x10);
                }
                NewMenu((struct cursor_s *)Cursor, 0xF, 0, -1);
            } else {
                NewMenu((struct cursor_s *)Cursor, 0xE, 0, -1);
            }
            JudderGameCamera(&GameCam, NULL, D_0062D318);
            GameSfx(0x3C, NULL);
            return;
        }
        if (Level == 0x19) {
            CalculateGamePercentage(&Game);
            if (Game.gems >= 0x2E) {
                cutmovie = 4;
                Game.cutbits |= 0x10000000;
            } else {
                cutmovie = 3;
                Game.cutbits |= 0x8000000;
            }
            new_level = 0x2B;
        } else {
            new_level = 0x25;
        }
        return;
    }
    if (TimeTrial != 0) {
        NewMenu((struct cursor_s *)Cursor, 0xE, 0, -1);
        return;
    }
    if ((Level == 0x1C || Level == 0x1D || Level == 3) && new_lev_flags == 0) {
        NewMenu((struct cursor_s *)Cursor, 0x10, 0, -1);
        return;
    }
    new_level = 0x25;
}

/* Font3DTab entry, stride 0x10 */
struct font3d_s {
    char pad_0[0x8];
    struct objspecial_s *special; /* 0x8 */
    char pad_c[0x10 - 0xC];
};

extern struct font3d_s Font3DTab[];
extern s8 Font3DRemap[];
extern struct nugscn_s *font3d_scene;
extern struct nuvec_s D_0061D8E8; /* crate count icon scale {1.5, 1.5, 1.5} */
extern char D_00630BC0[];         /* "%i/%i" */
extern float CRATECOUNT3DSEPERATION;
extern float CRATECOUNT3DSCALE;
extern s32 DESTRUCTIBLECRATECOUNT;
extern s32 strlen(const char *s);

void Draw3DCrateCount(struct nuvec_s *pos, u16 angle) {
    struct nuvec_s v;
    float dx;
    float dz;
    char *c;
    s32 len;
    s32 i;
    u16 ang;

    v = D_0061D8E8;
    NuMtxSetScale(&mTEMP, &v);
    NuMtxRotateY(&mTEMP, angle);
    NuMtxTranslate(&mTEMP, pos);
    NuRndrGScnObj(ObjTab[0x10].obj.scene->gobjs[ObjTab[0x10].obj.special->instance->objid],
                  &mTEMP);
    if (font3d_scene == NULL) {
        return;
    }
    ang = GameCam.yrot;
    v = *pos;
    v.x -= NuTrigTable[ang] * CRATECOUNT3DSEPERATION;
    v.z -= NuTrigTable[(u16)(ang + 0x4000)] * CRATECOUNT3DSEPERATION;
    dx = NuTrigTable[(u16)(ang + 0x4000)] * CRATECOUNT3DSCALE;
    dz = NuTrigTable[(u16)(ang - 0x8000)] * CRATECOUNT3DSCALE;
    sprintf(tbuf, D_00630BC0, plr_crates.draw, DESTRUCTIBLECRATECOUNT);
    len = strlen(tbuf);
    v.x -= dx * (len - 1) * 0.5f;
    v.z -= dz * (len - 1) * 0.5f;
    for (i = 0; i < len; i++) {
        c = &tbuf[i];
        if (Font3DRemap[*c] != -1) {
            Draw3DObject(-1, &v, 0, ang, 0, CRATECOUNT3DSCALE, CRATECOUNT3DSCALE,
                         CRATECOUNT3DSCALE, font3d_scene, Font3DTab[Font3DRemap[*c]].special,
                         0);
        }
        v.x += dx;
        v.z += dz;
    }
}
