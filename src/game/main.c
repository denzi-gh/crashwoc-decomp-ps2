/*
 * Unit: game/main
 * Functions:
 *   0x001c5490 SetTexAnimSignals
 *   0x001c5620 TextureCreate
 *   0x001c57c0 InitWorld
 *   0x001c6258 LoadLevel
 *   0x001c6778 DrawWorld
 *   0x001c6908 DoInput
 *   0x001c6cd0 InitPauseRender
 *   0x001c6ed0 HandlePauseRender
 *   0x001c7080 MiniGameReset
 *   0x001c70b0 MiniGame
 *   0x001c70b8 MiniGameRender
 *   0x001c70c0 MiniGameRenderScore
 *   0x001c70d0 BGLoadScreenInit
 *   0x001c7210 BGLoad
 *   0x001c83b0 BGLoadCutMovie
 *   0x001c87a8 BGLoadSfx
 *   0x001c8ce0 main
 *   0x001cab58 ResetSuperBuffer
 *   0x001cab68 SuperBufferMakeResident
 *   0x001cab78 PauseGame
 *   0x001cabd0 ResumeGame
 *   0x001cabf0 LoadScreen
 *   0x001cac30 TextureCreateEx
 *   0x001cae50 InitTexAnimScripts
 *   0x001caf48 LoadFont3D
 *   0x001caf88 InitCreatureModels
 *   0x001cafe0 InitCreatures
 *   0x001cb060 MtlReadEventHandler
 *   0x001cb100 TextureDestroy
 *   0x001cb180 CreateFadeMtl
 *   0x001cb280 UpdateFade
 *   0x001cb318 DrawFade
 *   0x001cb358 PauseRumble
 *   0x001cb3a8 InitLights
 *   0x001cb410 ClosePauseRender
 *   0x001cb468 MGCheckRng
 *   0x001cb4b8 BGLoadLevel
 *   0x001cb510 BGLoadCutScene
 *   0x001cb568 BGLoadSfxFn
 *   0x001cb600 SetLevel
 */

typedef signed int s32;

struct numtx_s {
    float _00, _01, _02, _03;
    float _10, _11, _12, _13;
    float _20, _21, _22, _23;
    float _30, _31, _32, _33;
}; /* 0x40 */

struct nugscn_s;
struct numtl_s;

/* NuCamera: view matrix at 0x00 (_32 = 0x38 is the atmospheric-pressure Z). */
struct nucam_s {
    struct numtx_s mtx;
};

/* GameCam: the world camera matrix lives at 0x00. */
struct gamecam_s {
    struct numtx_s m;
};

extern s32 Level;
extern s32 level_part_2;
extern float AtmosphericPressureHackedZ;
extern struct nugscn_s *world_scene[32];
extern struct nucam_s *pNuCam;
extern struct gamecam_s *pCam;
extern struct numtl_s *DebMat[8];

extern s32 fadeval;
extern s32 SWIDTH;
extern s32 SHEIGHT;
extern s32 D_00633264;              /* fade colour */
extern struct numtl_s *D_00633260; /* fade material */

extern void NuCameraSet(struct nucam_s *cam);
extern void NuGScnRndr3(struct nugscn_s *scene);
extern void NuBridgeDraw(struct nugscn_s *scene, struct numtl_s *mtl);
extern void NuWindDraw(struct nugscn_s *scene);
extern void edobjRenderObjects(struct nugscn_s *scene);
extern void NuRndrRect2di(s32 x, s32 y, s32 w, s32 h, s32 colour,
                          struct numtl_s *mtl);

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

struct ldata_s {
    u8 unk_0x00[0x24];
    u16 flags;       /* 0x24 */
    short character; /* 0x26 */
};

struct texanimref_s {
    void *script;    /* 0x00 */
    u64 lbit;        /* 0x08 */
}; /* 0x10 */

struct hazecol_s {
    u8 c[8];
};

struct character_s {
    u8 f0;             /* 0x00 */
    u8 f1;             /* 0x01 */
    u8 f2;             /* 0x02 */
    u8 unk_0x03[0xCE1];
}; /* 0xCE4 */

typedef void (*msgfn_t)();

extern struct ldata_s *LDATA;
extern u64 LBIT;
extern struct numtl_s *CrateMat;
extern struct nugscn_s *wumpa_scene;
extern struct nugscn_s *font3d_scene;
extern s32 font3d_initialised;
extern s32 GAMEOBJECTCOUNT;
extern s32 i_cratetypedata;
extern s32 PLAYERCOUNT;
extern void *superbuffer_ptr;
extern void *superbuffer_end;

extern char load_txt[];
extern u8 texanimbuff[];
extern struct texanimref_s D_004C1300[];
extern struct hazecol_s D_0062FAC8[];
extern struct hazecol_s D_0062FAD0[];
extern struct character_s Character[];
extern char D_0062FA48[];
extern char D_0061B2E8[];
extern char D_0061B308[];
extern char D_0061B318[];
extern char D_0061B330[];
extern char D_0061B340[];
extern char D_0061B350[];
extern char D_0061B360[];
extern char D_0061B370[];
extern char D_0061B388[];
extern char D_0061B3A0[];
extern char D_0061B3B0[];
extern char D_0061B3C0[];
extern char D_0061B3D8[];
extern char D_0061B3E8[];
extern char D_0061B400[];
extern char D_0061B418[];
extern char D_0061B428[];
extern char D_0061AE18[];
extern char D_0061AE28[];
extern char D_0061AE58[];
extern char D_0061AE88[];
extern char D_0061AEA0[];
extern char D_0061AEB0[];
extern char D_0061AEC0[];

extern void NuDisableVBlank(void);
extern void NuEnableVBlank(void);
extern void NuStrCpy(char *dst, char *src);
extern struct numtl_s *CreateAlphaBlendTexture256_32(char *name, s32 a, s32 b,
                                                     s32 c, s32 d);
extern void ParticleReset(void);
extern void InitSpecular(void);
extern void InitEnviro(void);
extern void InitGlass(void);
extern void InitClouds(void);
extern void NuTexAnimProgSysInit(void);
extern void *NuDatOpen(char *name, s32 a, s32 b);
extern void NuDatSet(void *dat);
extern void *memset(void *dst, s32 c, u32 n);
extern s32 NuTexAnimProgReadScript(void **buf, void *script);
extern msgfn_t NuDebugMsgProlog(char *file, s32 line);
extern void NuDatClose(void *dat);
extern void NuMtlReadEventSetHandler(void (*handler)(void));
extern struct nugscn_s *NuGScnRead(void **pptr, void *end, char *name);
extern void InitFont3D(struct nugscn_s *scene);
extern void LoadScreen(char *name);
extern void LoadCharacterModels(void);
extern void NuLigthSetPolyHazeMat(struct numtl_s *mtl, struct hazecol_s *a,
                                  struct hazecol_s *b);
extern void ClearGameObjects(void);
extern void ResetCheckpoint(s32 a, s32 b, s32 c, float d);
extern void InitWorld(void);
extern s32 ReadCrateData(void);
extern void ReadInCrateData(void);
extern void LoadLights(void);
extern void InitAI(void);
extern void InitChases(void);
extern void ResetChases(void);
extern void AddCreature(s32 character, s32 a, s32 b);
extern void MtlReadEventHandler(void);

void LoadLevel(void)
{
    struct texanimref_s *e;
    struct character_s *p;
    struct hazecol_s haze0;
    struct hazecol_s haze1;
    void *dat;
    void *animbuf;
    msgfn_t dbg;

    if ((LDATA->flags & 0x40) && CrateMat == 0) {
        NuDisableVBlank();
        NuStrCpy(load_txt, D_0061B2E8);
        NuEnableVBlank();
        CrateMat = CreateAlphaBlendTexture256_32(D_0061B308, 0, 1, 0x100, 0);
    }

    NuDisableVBlank();
    NuStrCpy(load_txt, D_0061B318);
    NuEnableVBlank();
    ParticleReset();

    NuDisableVBlank();
    NuStrCpy(load_txt, D_0061B330);
    NuEnableVBlank();
    InitSpecular();

    NuDisableVBlank();
    NuStrCpy(load_txt, D_0061B340);
    NuEnableVBlank();
    InitEnviro();

    NuDisableVBlank();
    NuStrCpy(load_txt, D_0061B350);
    NuEnableVBlank();
    InitGlass();

    NuDisableVBlank();
    NuStrCpy(load_txt, D_0061B360);
    NuEnableVBlank();

    if (LBIT & 0x200000A1) {
        InitClouds();
    }

    NuDisableVBlank();
    NuStrCpy(load_txt, D_0061B370);
    NuEnableVBlank();

    NuTexAnimProgSysInit();
    dat = NuDatOpen(D_0062FA48, 0, 0);
    NuDatSet(dat);
    animbuf = texanimbuff;
    memset(texanimbuff, 0, 0x8000);

    for (e = D_004C1300; e->script != 0; e++) {
        if ((e->lbit & LBIT) != 0 &&
            NuTexAnimProgReadScript(&animbuf, e->script) == 0) {
            dbg = NuDebugMsgProlog(D_0061AE18, 0x4FF);
            dbg(D_0061AE28, e->script);
        }
    }

    NuDatSet(0);
    if (dat != 0) {
        NuDatClose(dat);
    }
    NuMtlReadEventSetHandler(MtlReadEventHandler);

    wumpa_scene = 0;
    if (LDATA->flags & 0x100) {
        NuDisableVBlank();
        NuStrCpy(load_txt, D_0061B388);
        NuEnableVBlank();
        wumpa_scene = NuGScnRead(&superbuffer_ptr, superbuffer_end, D_0061B3A0);
    }

    NuDisableVBlank();
    NuStrCpy(load_txt, D_0061B3B0);
    NuEnableVBlank();

    if (Level != 0x28) {
        font3d_initialised = 0;
        font3d_scene = NuGScnRead(&superbuffer_ptr, superbuffer_end, D_0061AE58);
        InitFont3D(font3d_scene);
    }

    NuDisableVBlank();
    NuStrCpy(load_txt, D_0061B3C0);
    NuEnableVBlank();

    p = Character;
    do {
        p->f0 = 0;
        p->f1 = 0;
        p->f2 = 0;
        p++;
    } while (p < &Character[9]);
    GAMEOBJECTCOUNT = 0;
    LoadScreen(D_0061AE88);
    LoadCharacterModels();

    haze0 = D_0062FAC8[0];
    haze1 = D_0062FAD0[0];
    NuLigthSetPolyHazeMat(DebMat[4], &haze0, &haze1);

    ClearGameObjects();
    ResetCheckpoint(-1, -1, 0, 0.0f);

    NuDisableVBlank();
    NuStrCpy(load_txt, D_0061B3D8);
    NuEnableVBlank();
    InitWorld();

    i_cratetypedata = 0;
    if (LDATA->flags & 0x40) {
        NuDisableVBlank();
        NuStrCpy(load_txt, D_0061B3E8);
        NuEnableVBlank();
        if (ReadCrateData() != 0) {
            NuDisableVBlank();
            NuStrCpy(load_txt, D_0061B400);
            NuEnableVBlank();
            ReadInCrateData();
        }
    }

    NuDisableVBlank();
    NuStrCpy(load_txt, D_0061B418);
    NuEnableVBlank();
    LoadLights();

    NuDisableVBlank();
    NuStrCpy(load_txt, D_0061B428);
    NuEnableVBlank();

    LoadScreen(D_0061AEA0);
    InitAI();
    LoadScreen(D_0061AEB0);
    InitChases();
    LoadScreen(D_0061AEC0);
    ResetChases();

    if (LDATA->flags & 0x1) {
        AddCreature(LDATA->character, 0, -1);
        PLAYERCOUNT = 1;
    } else {
        PLAYERCOUNT = 0;
    }
}

void DrawWorld(void)
{
    if (Level == 0x18) {
        if (world_scene[0] != 0) {
            pNuCam->mtx._32 = AtmosphericPressureHackedZ;
            NuCameraSet(pNuCam);
            NuGScnRndr3(world_scene[0]);
            NuBridgeDraw(world_scene[0], DebMat[6]);
            NuWindDraw(world_scene[0]);
            edobjRenderObjects(world_scene[0]);
            pNuCam->mtx = pCam->m;
            NuCameraSet(pNuCam);
        }
    } else if (level_part_2 != 0) {
        if (world_scene[1] != 0) {
            NuGScnRndr3(world_scene[1]);
            NuBridgeDraw(world_scene[1], DebMat[6]);
            NuWindDraw(world_scene[1]);
            edobjRenderObjects(world_scene[1]);
        }
    } else {
        if (world_scene[0] != 0) {
            NuGScnRndr3(world_scene[0]);
            NuBridgeDraw(world_scene[0], DebMat[6]);
            NuWindDraw(world_scene[0]);
            edobjRenderObjects(world_scene[0]);
        }
    }
}

void DrawFade(void)
{
    if (fadeval != 0) {
        NuRndrRect2di(0, 0, SWIDTH << 4, SHEIGHT << 3, D_00633264,
                      D_00633260);
    }
}


