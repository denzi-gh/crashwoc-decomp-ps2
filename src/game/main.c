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

#include "nu_libc.h"
#include "nu_types.h"
#include "nu_port.h"

#include "nu3d/nuhgobj.h"
#include "nu3d/numtl.h"
#include "nu3d/nustream.h"
#include "nu3d/nutex.h"
#include "nu3d/nutexanm.h"
#include "nucore/nuerror.h"
#include "numath/numathtypes.h"
#include "numath/numtx.h"
#include "numath/nutrig.h"
#include "numath/nuvec.h"
#include "numath/nufloat.h"
#include "nu3d/nugscn.h"
#include "nu3d/nulight.h"
#include "nu3d/nuvport.h"
#include "nu3d/nucamera.h"
#include "nu3d/nuanim.h"
#include "game/gametypes.h"
#include "gamelib/terrain.h"
#include "game/creature.h"
#include "game/visidat.h"
#include "nucore/nucoretypes.h"
#include "nups2/ps2ctl.h"
#include "game/main.h"
#include "game/sfx.h"

#ifdef NW_HOST
#include "edtools/edmain.h"
#include "game/ai.h"
#include "game/bgload.h"
#include "game/bug.h"
#include "game/camera.h"
#include "game/chase.h"
#include "game/cloudfx.h"
#include "game/crate.h"
#include "game/cut.h"
#include "game/deb3.h"
#include "game/font3d.h"
#include "game/game.h"
#include "game/game_obj.h"
#include "game/glass.h"
#include "game/inst.h"
#include "game/lights.h"
#include "game/move.h"
#include "game/panel.h"
#include "game/particle.h"
#include "game/shadcom.h"
#include "game/vehicle.h"
#include "game/visi.h"
#include "gamelib/debris.h"
#include "nu3d/nufont.h"
#include "nu3d/nugobj.h"
#include "nu3d/nurndr.h"
#include "nucore/nufile.h"
#include "nucore/numem.h"
#include "numath/nurand.h"
#include "nups2/ps2video.h"
#include "nups2/vu1man.h"
#include "nusound/nusound.h"
/* gnu89 inline globals stay out of module headers. */
struct nufont_s;
struct nuvec_s;
struct obj_s;
void DrawCutMovie(void);
void GameVP(void);
s32 KillPlayer(struct obj_s *obj, s32 anim);
void MaxVP(void);
void NewCut(s32 i);
void NuFntDestroy(struct nufont_s *font);
void ResetTimeTrial(void);
void RotateDirectionalLight(struct nuvec_s *v, s32 xrot, s32 yrot);
void UpdateCutMovie(void);
#endif

extern s32 Level;
extern s32 level_part_2;
extern float AtmosphericPressureHackedZ;
extern struct nugscn_s *world_scene[32];
extern struct nucamera_s *pNuCam;
extern struct cammtx_s *pCam;
extern struct numtl_s *DebMat[8];

extern s32 fadeval;
extern s32 fade_rate;
extern s32 fadehack;
extern s32 SWIDTH;
extern s32 SHEIGHT;
extern s32 D_00633264;              /* fade colour */
extern struct numtl_s *D_00633260; /* fade material */

extern void NuCameraSet(struct nucamera_s *cam);
extern void NuGScnRndr3(struct nugscn_s *scene);
extern void NuBridgeDraw(struct nugscn_s *scene, struct numtl_s *mtl);
extern void NuWindDraw(struct nugscn_s *scene);
extern void edobjRenderObjects(struct nugscn_s *scene);
extern void NuRndrRect2di(s32 x, s32 y, s32 w, s32 h, s32 colour,
                          struct numtl_s *mtl);

struct hazecol_s {
    u8 c[8];
};

extern struct ldata_s LData[];
extern struct ldata_s *LDATA;
extern u64 LBIT;
extern float AIVISRANGE;
extern struct numtl_s *CrateMat;
extern struct nugscn_s *wumpa_scene;
extern struct nugscn_s *font3d_scene;
extern volatile s32 font3d_initialised;
extern s32 GAMEOBJECTCOUNT;
extern s32 i_cratetypedata;
extern s32 PLAYERCOUNT;
extern union variptr_u superbuffer_ptr;
extern union variptr_u superbuffer_end;

extern char load_txt[];
extern u8 texanimbuff[];
extern struct texanimref_s D_004C1300[];
#define texanimscripts D_004C1300
extern struct hazecol_s D_0062FAC8[];
extern struct hazecol_s D_0062FAD0[];
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
#ifndef NW_HOST /* the host takes the prototypes of nucore/nufile.h */
extern void *NuDatOpen(char *name, s32 a, s32 b);
extern void NuDatSet(void *dat);
extern void NuDatClose(void *dat);
#endif
extern void InitFont3D(struct nugscn_s *scene);
extern void LoadScreen(char *name);
extern void ClearGameObjects(void);
extern void ResetCheckpoint(s32 a, s32 b, s32 c, float d);
extern void InitWorld(void);
extern s32 ReadCrateData(void);
extern void ReadInCrateData(void);
extern void LoadLights(void);
extern void InitAI(void);
extern void InitChases(void);
extern void ResetChases(void);
extern s32 MtlReadEventHandler(struct numtl_s *mtl);

extern char LevelFileName[];
extern char PadRecordPath[];
extern char tbuf[];
extern struct visidata_s *world_vd;
extern s32 WUMPACOUNT;
extern s32 SFXCOUNT_ALL;
extern struct nugscn_s *crate_scene;
extern struct nugscn_s *pause_scene;
extern struct numtl_s *ShadowMat;
extern GameSound SfxTabGLOBAL[];
extern u8 D_0057EEA4[];
extern s32 edcrt_used;
extern s32 edwmp_used;
extern s32 edai_used;

extern char D_0062FA80[];
extern char D_0062FA88[];
extern char D_0062FA90[];
extern char D_0062FA98[];
extern char D_0062FAA0[];
extern char D_0062FAA8[];
extern char D_0062FAB0[];
extern char D_0062FAB8[];
extern char D_0062FAC0[];
extern char D_0061AED0[];
extern char D_0061AEE8[];
extern char D_0061AF00[];
extern char D_0061AF20[];
extern char D_0061AF40[];
extern char D_0061AF58[];
extern char D_0061AF78[];
extern char D_0061AF90[];
extern char D_0061AFB0[];
extern char D_0061AFC8[];
extern char D_0061AFE8[];
extern char D_0061B000[];
extern char D_0061B020[];
extern char D_0061B038[];
extern char D_0061B058[];
extern char D_0061B070[];
extern char D_0061B088[];
extern char D_0061B0A0[];
extern char D_0061B0B0[];
extern char D_0061B0C0[];
extern char D_0061B0D8[];
extern char D_0061B0F0[];
extern char D_0061B108[];
extern char D_0061B118[];
extern char D_0061B130[];
extern char D_0061B150[];
extern char D_0061B168[];
extern char D_0061B180[];
extern char D_0061B1A0[];
extern char D_0061B1B8[];
extern char D_0061B1D0[];
extern char D_0061B1E8[];
extern char D_0061B200[];
extern char D_0061B218[];
extern char D_0061B228[];
extern char D_0061B248[];
extern char D_0061B260[];
extern char D_0061B270[];
extern char D_0061B280[];
extern char D_0061B290[];
extern char D_0061B2A0[];
extern char D_0061B2B0[];
extern char D_0061B2C0[];
extern char D_0061B2D8[];

extern void NuStrCat(char *dst, char *src);
extern void NuRndrInitWorld(void);
extern void BuildVisiTable(struct nugscn_s *gsc);
extern void edobjRegisterBaseScene(struct nugscn_s *scene);
extern void edbitsRegisterBaseScene(struct nugscn_s *scene);
extern void NuBridgeRegisterBaseScene(struct nugscn_s *scene);
extern void edanimRegisterBaseScene(struct nugscn_s *scene);
extern void edbitsRegisterSfx(void *tab, void *sfx, s32 count, s32 total);
extern void NuWaterInit(struct nugscn_s *scene);
extern void NuBridgeInit(void);
extern void NuWindInit(void);
extern s32 saveloadLoadIcon(char *name, void *buf);
extern void edppDestroyAllParticles(void);
extern void edppDestroyAllEffects(void);
extern s32 NuFileSize(char *name);
extern int edppLoadEffects(char *name, s32 a);
extern int edppMergeEffects(char *name, s32 a);
extern void InitGameDebris(void);
extern void edppRestartAllEffectsInLevel(void);
extern void edobjObjectReset(void);
extern void edobjFileLoadObjects(char *name);
extern void edanimParamReset(void);
extern int edanimFileLoad(char *name);
extern void edgraClumpsReset(void);
extern int edgraFileLoad(char *name);
extern void edgraInitAllClumps(void);
extern void LoadVehicleStuff(void);
extern s32 LoadCutMovie(s32 movie);
extern void StartCutMovie(void);
extern void NuGScnUpdate(struct nugscn_s *scene, float t);
extern void edobjUpdateObjects(float t);
extern void edanimUpdateObjects(float t);
extern void NuBridgeUpdate(void *a);
extern void NuWindUpdate(void *a);
extern struct numtl_s *CreateAlphaBlendTexture64(char *name, s32 a, s32 b,
                                                 s32 c);
extern void InitSplineTable(void);
extern void InitObjectTable(void);
extern void InitRails(void);
extern void InitWumpa(void);
extern void ResetMaskFeathers(void);
extern void InitCrates(void);
extern void InitVehicleToggles(void);
extern void InitLevel(void);
extern s32 qrand(void);
extern void NuRndrShadowInit(void *buf);
extern void NuLightMatInit(void);

/* Keep the message argument alive while the VBlank callback is disabled. */
static inline void LOAD_MESSAGE(char *name)
{
    NuDisableVBlank();
    NuStrCpy(load_txt, name);
    NuEnableVBlank();
}

void InitWorld(void)
{
    s32 i;
    s32 n;
    char buf[0x800];

    NuStrCpy(LevelFileName, D_0062FA80);
    NuStrCat(LevelFileName, LDATA->path);
    NuStrCpy(PadRecordPath, LevelFileName);
    NuStrCat(PadRecordPath, D_0062FA88);

    for (i = 0; i < 32; i++) {
        world_scene[i] = 0;
    }
    NuRndrInitWorld();

    if (LDATA->flags & 0x4) {
        LOAD_MESSAGE(D_0061AED0);
        NuStrCpy(tbuf, LevelFileName);
        NuStrCat(tbuf, D_0062FA90);

        if (Level != 0x27 && Level != 0x29) {
            world_scene[0] = NuGScnRead(&superbuffer_ptr.charptr, superbuffer_end.charptr, tbuf);
            NuStrCpy(tbuf, LevelFileName);
            NuStrCat(tbuf, D_0062FA98);
            world_vd = visiLoadData(tbuf, world_scene[0], &superbuffer_ptr);
            if (world_scene[0] != 0 && world_vd == 0) {
                BuildVisiTable(world_scene[0]);
            }
        }

        LOAD_MESSAGE(D_0061AEE8);

        if (Level == 0xE) {
            world_scene[1] =
                NuGScnRead(&superbuffer_ptr.charptr, superbuffer_end.charptr, D_0061AF00);
        } else if (Level == 0x1F) {
            world_scene[1] =
                NuGScnRead(&superbuffer_ptr.charptr, superbuffer_end.charptr, D_0061AF20);
        } else if (Level == 0x27 || Level == 0x29) {
            LOAD_MESSAGE(D_0061AF40);
            world_scene[0] =
                NuGScnRead(&superbuffer_ptr.charptr, superbuffer_end.charptr, D_0061AF58);
            LOAD_MESSAGE(D_0061AF78);
            world_scene[1] =
                NuGScnRead(&superbuffer_ptr.charptr, superbuffer_end.charptr, D_0061AF90);
            LOAD_MESSAGE(D_0061AFB0);
            world_scene[2] =
                NuGScnRead(&superbuffer_ptr.charptr, superbuffer_end.charptr, D_0061AFC8);
            LOAD_MESSAGE(D_0061AFE8);
            world_scene[3] =
                NuGScnRead(&superbuffer_ptr.charptr, superbuffer_end.charptr, D_0061B000);
            LOAD_MESSAGE(D_0061B020);
            world_scene[4] =
                NuGScnRead(&superbuffer_ptr.charptr, superbuffer_end.charptr, D_0061B038);
        }

        if (LBIT & 0x400000) {
            world_scene[2] =
                NuGScnRead(&superbuffer_ptr.charptr, superbuffer_end.charptr, D_0061B058);
        } else if (LBIT & 0x5A031894DULL) {
            world_scene[2] =
                NuGScnRead(&superbuffer_ptr.charptr, superbuffer_end.charptr, D_0061B070);
        }
    }

    edobjRegisterBaseScene(world_scene[0]);
    edbitsRegisterBaseScene(world_scene[0]);
    NuBridgeRegisterBaseScene(world_scene[0]);
    edanimRegisterBaseScene(world_scene[0]);
    edbitsRegisterSfx(SfxTabGLOBAL, LDATA->pSFX, 0xC5, SFXCOUNT_ALL);
    NuWaterInit(world_scene[0]);
    NuBridgeInit();
    NuWindInit();
    NuLightFogClear(0);

    if (Level == 0x25) {
        LOAD_MESSAGE(D_0061B088);
        n = saveloadLoadIcon(D_0061B0A0, superbuffer_ptr.voidptr);
        superbuffer_ptr.intaddr = (superbuffer_ptr.intaddr + n + 0xF) & ~(nuintptr)0xF;
    }

    noterraininit();
    if (LDATA->flags & 0x8) {
        LOAD_MESSAGE(D_0061B0B0);
        TerrainSetCur(superbuffer_ptr.voidptr);
        terraininit(Level, &superbuffer_ptr.s16, superbuffer_end.s16, 0, LevelFileName,
                    world_scene[0], 0);
        superbuffer_ptr.intaddr = (superbuffer_ptr.intaddr + 0xF) & ~(nuintptr)0xF;
    }

    WUMPACOUNT = 0;
    crate_scene = 0;
    if (LDATA->flags & 0x40) {
        LOAD_MESSAGE(D_0061B0C0);
        crate_scene = NuGScnRead(&superbuffer_ptr.charptr, superbuffer_end.charptr, D_0061B0D8);
    }

    pause_scene = 0;
    if ((LDATA->flags & 0x1) || Level == 0x23) {
        LOAD_MESSAGE(D_0061B0F0);
        pause_scene = NuGScnRead(&superbuffer_ptr.charptr, superbuffer_end.charptr, D_0061B108);
    }

    edppDestroyAllParticles();
    edppDestroyAllEffects();

    if (LDATA->flags & 0x10) {
        LOAD_MESSAGE(D_0061B118);
        sprintf(tbuf, D_0062FAA0, LevelFileName);
        if (NuFileSize(tbuf) > 0) {
            edppLoadEffects(tbuf, 1);
        }

        LOAD_MESSAGE(D_0061B130);
        sprintf(tbuf, D_0061B150);
        if (NuFileSize(tbuf) > 0) {
            edppMergeEffects(tbuf, 0);
        }

        LOAD_MESSAGE(D_0061B168);
        sprintf(tbuf, D_0062FAA8, LevelFileName);
        if (NuFileSize(tbuf) > 0) {
            edppMergeEffects(tbuf, 5);
        }
    }

    InitGameDebris();
    LOAD_MESSAGE(D_0061B180);
    edppRestartAllEffectsInLevel();

    LOAD_MESSAGE(D_0061B1A0);
    edobjObjectReset();

    LOAD_MESSAGE(D_0061B1B8);
    sprintf(tbuf, D_0062FAB0, LevelFileName);
    if (NuFileSize(tbuf) > 0) {
        edobjFileLoadObjects(tbuf);
    }

    edanimParamReset();
    LOAD_MESSAGE(D_0061B1D0);
    sprintf(tbuf, D_0062FAB8, LevelFileName);
    if (NuFileSize(tbuf) > 0) {
        edanimFileLoad(tbuf);
    }

    edgraClumpsReset();
    LOAD_MESSAGE(D_0061B1E8);
    sprintf(tbuf, D_0062FAC0, LevelFileName);
    if (NuFileSize(tbuf) > 0) {
        edgraFileLoad(tbuf);
    }

    edgraInitAllClumps();
    LOAD_MESSAGE(D_0061B200);
    LoadVehicleStuff();

    LOAD_MESSAGE(D_0061B218);

    if (Level == 0x27) {
        LoadCutMovie(1);
        StartCutMovie();
    } else if (Level == 0x29) {
        LoadCutMovie(3);
        StartCutMovie();
    }

    TerrainPlatformOldUpdate();
    if (world_scene[0] != 0) {
        NuGScnUpdate(world_scene[0], 0.0f);
    }
    edobjUpdateObjects(0.0f);
    edanimUpdateObjects(0.0f);
    NuBridgeUpdate(D_0057EEA4);
    NuWindUpdate(D_0057EEA4);
    TerrainPlatformNewUpdate();

    if ((LDATA->flags & 0x1) && Level != 0x25) {
        LOAD_MESSAGE(D_0061B228);
        ShadowMat = CreateAlphaBlendTexture64(D_0061B248, 0, 1, 0x64);
    }

    ResetCheckpoint(-1, -1, 0, 0.0f);

    LOAD_MESSAGE(D_0061B260);
    InitSplineTable();

    LOAD_MESSAGE(D_0061B270);
    InitObjectTable();

    LOAD_MESSAGE(D_0061B280);
    InitRails();

    LOAD_MESSAGE(D_0061B290);
    InitWumpa();
    ResetMaskFeathers();

    LOAD_MESSAGE(D_0061B2A0);
    InitCrates();
    InitVehicleToggles();

    LOAD_MESSAGE(D_0061B2B0);
    InitLevel();

    for (i = 0; i < 0x800; i++) {
        buf[i] = qrand() >> 8;
    }

    LOAD_MESSAGE(D_0061B2C0);
    NuRndrShadowInit(buf);

    LOAD_MESSAGE(D_0061B2D8);
    NuLightMatInit();

    edcrt_used = 0;
    edwmp_used = 0;
    edai_used = 0;
}

inline void InitTexAnimScripts(void)
{
    struct texanimref_s *list = texanimscripts;
    void *dat;
    union variptr_u animbuf;
    NuErrorFunc dbg;

    NuTexAnimProgSysInit();
    dat = NuDatOpen(D_0062FA48, 0, 0);
    NuDatSet(dat);
    animbuf.voidptr = texanimbuff;
    memset(texanimbuff, 0, 0x8000);

    for (; list->script != 0; list++) {
        if ((list->lbit & LBIT) != 0 &&
            NuTexAnimProgReadScript(&animbuf, list->script) == 0) {
            dbg = NuDebugMsgProlog(D_0061AE18, 0x4FF);
            dbg(D_0061AE28, list->script);
        }
    }

    NuDatSet(0);
    if (dat != 0) {
        NuDatClose(dat);
    }
}


inline void InitCreatureModels(void)
{
    s32 i;

    for (i = 0; i < 9; i++) {
        Character[i].used = 0;
        Character[i].on = 0;
        Character[i].off_wait = 0;
    }
    GAMEOBJECTCOUNT = 0;
    LoadScreen(D_0061AE88);
    LoadCharacterModels();
}


inline void InitCreatures(void)
{
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


inline void LoadScreen(char *name)
{
    NuDisableVBlank();
    NuStrCpy(load_txt, name);
    NuEnableVBlank();
}


inline void LoadFont3D(void)
{
    font3d_initialised = 0;
    font3d_scene = NuGScnRead(&superbuffer_ptr.charptr, superbuffer_end.charptr, D_0061AE58);
    InitFont3D(font3d_scene);
}


void LoadLevel(void)
{
    struct hazecol_s haze0;
    struct hazecol_s haze1;

    if ((LDATA->flags & 0x40) && CrateMat == 0) {
        LoadScreen(D_0061B2E8);
        CrateMat = CreateAlphaBlendTexture256_32(D_0061B308, 0, 1, 0x100, 0);
    }

    LoadScreen(D_0061B318);
    ParticleReset();
    LoadScreen(D_0061B330);
    InitSpecular();
    LoadScreen(D_0061B340);
    InitEnviro();
    LoadScreen(D_0061B350);
    InitGlass();
    LoadScreen(D_0061B360);
    if (LBIT & 0x200000A1) {
        InitClouds();
    }
    LoadScreen(D_0061B370);

    InitTexAnimScripts();
    NuMtlReadEventSetHandler(MtlReadEventHandler);

    wumpa_scene = 0;
    if (LDATA->flags & 0x100) {
        LoadScreen(D_0061B388);
        wumpa_scene = NuGScnRead(&superbuffer_ptr.charptr, superbuffer_end.charptr, D_0061B3A0);
    }

    LoadScreen(D_0061B3B0);
    if (Level != 0x28) {
        LoadFont3D();
    }

    LoadScreen(D_0061B3C0);
    InitCreatureModels();

    haze0 = D_0062FAC8[0];
    haze1 = D_0062FAD0[0];
    NuLigthSetPolyHazeMat(DebMat[4], (float *)&haze0, (float *)&haze1);

    ClearGameObjects();
    ResetCheckpoint(-1, -1, 0, 0.0f);

    LoadScreen(D_0061B3D8);
    InitWorld();

    i_cratetypedata = 0;
    if (LDATA->flags & 0x40) {
        LoadScreen(D_0061B3E8);
        if (ReadCrateData() != 0) {
            LoadScreen(D_0061B400);
            ReadInCrateData();
        }
    }

    LoadScreen(D_0061B418);
    LoadLights();
    LoadScreen(D_0061B428);
    InitCreatures();
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

void CreateFadeMtl(void)
{
    struct numtl_s *m = NuMtlCreate(1);

    D_00633260 = m;
    D_00633260->attrib.zmode = 3;
    D_00633260->attrib.filter = 1;
    D_00633260->attrib.lighting = 2;
    D_00633260->attrib.utc = 1;
    D_00633260->attrib.vtc = 1;
    D_00633260->attrib.colour = 1;
    D_00633260->attrib.alpha = 3;
    NuMtlUpdate(m);
}

inline void UpdateFade(void)
{
    s32 old = fadeval;

    fadeval = old + fade_rate;
    if (fadeval >= 0x100) {
        fadeval = 0xFF;
    } else if (fadeval < 0) {
        fadeval = 0;
    }

    if (old < 0xFF && fadeval == 0xFF) {
        fadehack = 1;
    } else if (fadehack != 0) {
        fadehack--;
    }

    if (fadeval == 0 || fadeval == 0xFF) {
        fade_rate = 0;
    }

    D_00633264 =
        fadeval | (fadeval << 8) | (fadeval << 16) | 0x80000000;
}

inline void DrawFade(void)
{
    if (fadeval != 0) {
        NuRndrRect2di(0, 0, SWIDTH << 4, SHEIGHT << 3, D_00633264,
                      D_00633260);
    }
}

extern char D_0061B550[];
extern union variptr_u superbuffer_reset_base;
extern volatile s32 bgload_complete;
extern void BGLoadScreenInit(s32 a);
extern void InitLevelSfxTables(void);
extern void InitGlobalSfx(void);

void BGLoadSfxFn(void)
{
    char *msg = D_0061B550;
    s32 done = 1;

    NuDisableVBlank();
    NuStrCpy(load_txt, msg);
    NuEnableVBlank();

    superbuffer_ptr = superbuffer_reset_base;
    font3d_initialised = 0;
    font3d_scene = NuGScnRead(&superbuffer_ptr.charptr, superbuffer_end.charptr, D_0061AE58);
    InitFont3D(font3d_scene);
    font3d_initialised = done;
    BGLoadScreenInit(0);
    InitLevelSfxTables();
    InitGlobalSfx();
    bgload_complete = done;
    for (;;) {
        NW_HOST_IDLE();
    }
}

void SetLevel(void)
{
    LDATA = &LData[Level];
    LBIT = (u64)1 << Level;
    if (pNuCam != 0) {
        if ((u32)LDATA->farclip > 9) {
            pNuCam->farclip = (float)LDATA->farclip;
        } else {
            pNuCam->farclip = 1000.0f;
        }
    }
    AIVISRANGE = 25.0f;
}

extern struct nupad_s *Pad[];
extern struct cursor_s Cursor;
extern struct game_s Game;
extern struct gtimer_s PauseTimer;
extern char PadRecordPath[];

extern s32 pad_record;
extern s32 stick_bits;
extern s32 stick_oldbits;
extern s32 stick_bits_db;
extern s32 new_mode;
extern s32 new_level;
extern s32 editor_active;
extern s32 GameMode;
extern s32 cutmovie;
extern s32 Paused;
extern s32 last_hub;
extern s32 pause_dir;
extern s32 pausestats_frame;
extern s32 pausebuzz;
extern s32 Demo;
extern float tumble_time;
extern float tumble_duration;

extern void NewMenu(struct cursor_s *cursor, s32 menu, s32 y, s32 level);
extern void ResetTimer(struct gtimer_s *t);
extern void PauseGameAudio(s32 a);
extern void GameSfx(s32 id, s32 a);
extern void NuPs2VideoSetPos(s32 x, s32 y);

typedef struct {
    u32 score;
    u32 time;
    u32 flags;
} minigamedata_s;

extern minigamedata_s D_0061B4B0;

void MiniGameReset(void)
{
    minigamedata_s save = D_0061B4B0;
}


void MiniGame(struct nuvec_s *pos)
{
}

void MiniGameRender(void)
{
}


void MiniGameRenderScore(void)
{
    char buf[16];
}

void DoInput(void)
{
    struct nupad_s *pad = Pad[0];

    if (pad == 0) {
        return;
    }

    if (NuPs2ReadPad(pad)) {
        if (pad_record != 0) {
            if ((pad->paddata_db & 0x800) || player->obj.dead != 0) {
                SavePadRecord(PadRecordPath);
                pad_record = 0;
            }
        }
        stick_bits = 0;
        {
            float lim = 85.0f;
            float f = (float)pad->rdata.l_alg_x - 127.5f;
            if (f < -lim) {
                stick_bits = 0x8000;
            } else if (f > lim) {
                stick_bits = 0x2000;
            }
            f = (float)pad->rdata.l_alg_y - 127.5f;
            if (f < -lim) {
                stick_bits |= 0x1000;
            } else if (f > lim) {
                stick_bits |= 0x4000;
            }
        }
        stick_bits_db = stick_bits & (stick_bits ^ stick_oldbits);
        stick_oldbits = stick_bits;

        if (!(pad->paddata_db & 0x800)) {
            return;
        }
        if (!(LDATA->flags & 1)) {
            return;
        }
        if (new_mode != -1) {
            return;
        }
        if (new_level != -1) {
            return;
        }
        if (fadeval != 0) {
            return;
        }
        if (editor_active != 0) {
            return;
        }
        if (GameMode == 1) {
            return;
        }
        if (cutmovie != -1) {
            return;
        }

        if (Paused == 0 && Cursor.menu == -1 &&
            !(PLAYERCOUNT != 0 &&
              (player->obj.finished != 0 ||
               (Level == 0x25 && tumble_time < tumble_duration &&
                last_hub != -1)))) {
            pause_dir = 1;
            NewMenu(&Cursor, 3, 0, -1);
            ResetTimer(&PauseTimer);
            pausestats_frame = 0;
            PauseGameAudio(1);
            GameSfx(0x36, 0);
            GameSfx(0x36, 0);
        } else if (Paused == 25) {
            pause_dir = 2;
            pausebuzz = 0;
            NuPs2VideoSetPos(Game.screen_x << 2, Game.screen_y << 1);
            GameSfx(0x3C, 0);
        }
    } else {
        if (Paused != 0 && pause_dir != 2) {
            return;
        }
        if (Pad[0] == 0) {
            return;
        }
        if (Pad[0]->valid) {
            return;
        }
        if (Demo != 0) {
            return;
        }
        if (Cursor.menu == 0) {
            return;
        }
        if (Cursor.menu == 0x20) {
            return;
        }
        if (GameMode == 1) {
            return;
        }
        if (cutmovie != -1) {
            return;
        }
        if (new_mode != -1) {
            return;
        }
        if (new_level != -1) {
            return;
        }
        if (fadeval != 0) {
            return;
        }
        if (editor_active != 0) {
            return;
        }
        if (Cursor.menu != -1) {
            return;
        }
        if (PLAYERCOUNT != 0) {
            if (player->obj.finished != 0) {
                return;
            }
            if (Level == 0x25 && tumble_time < tumble_duration &&
                last_hub != -1) {
                return;
            }
        }
        pause_dir = 1;
        NewMenu(&Cursor, 3, 0, -1);
        ResetTimer(&PauseTimer);
        pausestats_frame = 0;
        PauseGameAudio(1);
        GameSfx(0x36, 0);
    }
}


extern char D_0061B4D0[];
extern s32 bgloadlastcharacter;
extern char *bgloadcharnames[];
extern char *bgloadanimnames[];
extern struct NUHGOBJ_s *bgloadchr[2];
extern volatile struct nuanimdata_s *bgloadanim[2];

extern void TimeTunnelInit(union variptr_u *buffer, union variptr_u *buffend);

void BGLoadScreenInit(s32 loadchars)
{
    union variptr_u *buffend = &superbuffer_end;
    union variptr_u buffer;
    void *dat;
    s32 c;

    buffer.u8 = superbuffer_end.u8 - 0x100000;
    TimeTunnelInit(&buffer, buffend);
    if (loadchars != 0) {
        dat = NuDatOpen(D_0061B4D0, 0, 0);
        NuDatSet(dat);
        c = bgloadlastcharacter;
        if (c >= 0 && c < 2) {
            bgloadchr[c] = NuGHGRead(&buffer, bgloadcharnames[c]);
            bgloadanim[c] = NuAnimDataLoadBuff(bgloadanimnames[c], &buffer, buffend);
            bgloadlastcharacter = -1;
        }
        if (LDATA->character >= 0 && LDATA->character < 2) {
            bgloadanim[LDATA->character] =
                NuAnimDataLoadBuff(bgloadanimnames[LDATA->character], &superbuffer_ptr, buffend);
        }
        NuDatSet(0);
        if (dat != 0) {
            NuDatClose(dat);
        }
    }
}


extern s32 texanimbits;
extern float WeatherBossSkeletonTimer;
extern s32 WeatherBossDead;

void SetTexAnimSignals(void)
{
    if (player->used != 0 &&
        (player->obj.dead == 3 || player->obj.dead == 8 || player->freeze != 0)) {
        texanimbits |= 2;
    } else {
        texanimbits &= 0xFFFFFFFD;
    }
    if (player->obj.mask != 0 && player->obj.mask->active > 1) {
        texanimbits |= 4;
    } else {
        texanimbits &= 0xFFFFFFFB;
    }
    if (player->used != 0 && player->obj.dead != 0 && player->obj.dead != 0x12) {
        texanimbits |= 0x20;
    } else {
        texanimbits &= 0xFFFFFFDF;
    }
    if (Level == 0x17) {
        texanimbits |= 0x40;
    } else {
        texanimbits &= 0xFFFFFFBF;
    }
    if (Level == 0x18 && WeatherBossSkeletonTimer != 0.0f) {
        texanimbits |= 0x100;
    } else {
        texanimbits &= 0xFFFFFEFF;
    }
    if (Level == 0x18 && WeatherBossDead != 0) {
        texanimbits |= 0x200;
    }
    NuTexAnimSetSignals(texanimbits);
    NuTexAnimProcess();
}


void ResetSuperBuffer(void)
{
    superbuffer_ptr = superbuffer_reset_base;
}


void SuperBufferMakeResident(void)
{
    superbuffer_reset_base = superbuffer_ptr;
}


void PauseGame(void)
{
    pause_dir = 1;
    NewMenu(&Cursor, 3, 0, -1);
    ResetTimer(&PauseTimer);
    pausestats_frame = 0;
    PauseGameAudio(1);
    GameSfx(0x36, 0);
}


extern void ResumeGameAudio(void);

void ResumeGame(void)
{
    ResumeGameAudio();
}



void PauseRumble(void)
{
    if (Pad[0]) {
        NuPs2PadSetMotors(Pad[0], pausebuzz != 0, 0);
    }
    if (Pad[1]) {
        NuPs2PadSetMotors(Pad[1], 0, 0);
    }
}


struct vec2_s {
    float x;
    float y;
};

struct vec3_s {
    float x;
    float y;
    float z;
};

extern struct nulight_s *D_0062FAE8;
#define level_light D_0062FAE8

void InitLights(void)
{
    level_light = NuLightCreate();
    if (level_light != 0) {
        level_light->type = 0;
        level_light->on = 1;
        level_light->ambient.r = 0.0f;
        level_light->ambient.g = 0.0f;
        level_light->ambient.b = 0.0f;
        level_light->diffuse.r = 0.0f;
        level_light->diffuse.g = 0.0f;
        level_light->diffuse.b = 1.0f;
        NuMtxSetRotationX(&level_light->mtx, 0xC000);
        NuLightUpdate(level_light);
    }
}


extern struct numtl_s *pause_src_mtl;
extern struct numtl_s *pause_rndr_mtl;
extern s32 pause_rt;

void ClosePauseRender(void)
{
    if (pause_src_mtl) {
        NuMtlDestroy(pause_src_mtl);
    }
    if (pause_rndr_mtl) {
        NuMtlDestroy(pause_rndr_mtl);
    }
    if (pause_rt) {
        NuTexDestroy(pause_rt);
    }
    pause_src_mtl = 0;
    pause_rndr_mtl = 0;
    pause_rt = 0;
}

/* Texture descriptor handed to NuTexCreate (type 0xB = render target). */
struct nutexdesc_s {
    s32 type;                /* 0x00 */
    s32 width;               /* 0x04 */
    s32 height;              /* 0x08 */
    s32 mmcnt;               /* 0x0C */
    void *bits;              /* 0x10 */
    void *pal;               /* 0x14 */
    s32 decal;               /* 0x18 */
    s32 linear;              /* 0x1C */
}; /* 0x20 */

extern s32 pause_rndr_on;
extern s32 D_0062FB54;                 /* Paused state of the previous frame */
extern s32 PS2_REZ_W;
extern s32 PS2_REZ_H;
extern s32 screendump;
extern s32 NuRndrBeginScene(s32 a);
extern s32 NuRndrBeginSceneEx(s32 target, s32 zbuffer, s32 halfoffset);
extern void NuRndrEndScene(void);
extern void NuRndrClear(s32 mode, s32 col, float z);
extern void NuRndrFlush(s32 flags);
extern void NuRndrRectUV2di(s32 x, s32 y, s32 w, s32 h, float u0, float v0, float u1, float v1,
                            u32 col, struct numtl_s *mtl);

void InitPauseRender(void)
{
    struct nutexdesc_s desc;

    desc.type = 0xB;
    desc.width = 0x100;
    desc.height = 0x100;
    pause_rt = NuTexCreate((struct nutex_s *)&desc);

    pause_rndr_mtl = NuMtlCreate(1);
    pause_rndr_mtl->alpha = 1.0f;
    pause_rndr_mtl->tid = pause_rt;
    pause_rndr_mtl->attrib.zmode = 3;
    pause_rndr_mtl->attrib.utc = 1;
    pause_rndr_mtl->attrib.vtc = 1;
    pause_rndr_mtl->attrib.filter = 1;
    pause_rndr_mtl->attrib.lighting = 2;
    pause_rndr_mtl->attrib.colour = 1;
    pause_rndr_mtl->attrib.alpha = 0;
    pause_rndr_mtl->sort_pri = 0x7FFF;
    NuMtlUpdate(pause_rndr_mtl);

    pause_src_mtl = NuMtlCreate(1);
    pause_src_mtl->alpha = 1.0f;
    pause_src_mtl->tid = -1;
    pause_src_mtl->attrib.zmode = 3;
    pause_src_mtl->attrib.utc = 1;
    pause_src_mtl->attrib.vtc = 1;
    pause_src_mtl->attrib.filter = 1;
    pause_src_mtl->attrib.lighting = 2;
    pause_src_mtl->attrib.colour = 1;
    pause_src_mtl->attrib.alpha = 0;
    pause_src_mtl->sort_pri = 0x7FFE;
    NuMtlUpdate(pause_src_mtl);

    pause_rndr_on = 0;
}

void HandlePauseRender(void)
{
    struct nuviewport_s *vp;
    float u;
    float v;

    if (pause_rndr_on != 0 && pause_rt != 0 && NuRndrBeginScene(-1) != 0) {
        vp = NuVpGetCurrentViewport();
        NuRndrClear(5, 0, 1.0f);
        NuRndrRectUV2di(0, 0, vp->width, vp->height, 0.0f, 0.0f, 1.0f, 1.0f, 0x80808080, pause_rndr_mtl);
        NuRndrEndScene();
    }
    if (Paused != 0 && D_0062FB54 == 0) {
        u = (PS2_REZ_W - 1.0f) / NuPower2(PS2_REZ_W);
        v = (PS2_REZ_H - 1.0f) / NuPower2(PS2_REZ_H);
        if (pause_rt != 0) {
            NuRndrFlush(3);
            if (NuRndrBeginSceneEx(pause_rt, -2, 0) != 0) {
                NuRndrRectUV2di(0, 0, 0xFF0, 0xFF0, 0.0f, 0.0f, u, v, 0x80808080, pause_src_mtl);
                NuRndrEndScene();
            }
        }
        pause_rndr_on = 1;
    }
    if (Paused == 0 && D_0062FB54 != 0) {
        pause_rndr_on = 0;
    }
    D_0062FB54 = Paused;
    if (editor_active != 0) {
        pause_rndr_on = 0;
    }
    if (screendump != 0) {
        pause_rndr_on = 0;
    }
}

extern void NuMemSetExternal(void *addr, s32 *end);

void TextureCreate(char *name, struct numtl_s **mtl, struct nutex_s **tid)
{
    if (NuFileSize(name) < 0) {
        *mtl = 0;
        *tid = 0;
        return;
    }
    *tid = NuTexReadBitmapMM(name, 0);
    *mtl = NuMtlCreate(1);
    (*mtl)->attrib.zmode = 3;
    (*mtl)->attrib.filter = 1;
    (*mtl)->attrib.lighting = 2;
    (*mtl)->attrib.utc = 1;
    (*mtl)->attrib.vtc = 1;
    (*mtl)->attrib.colour = 1;
    (*mtl)->attrib.alpha = 0;
    (*mtl)->tid = (s32)NuTexCreate(*tid);
    NuMtlUpdate(*mtl);
    NuMemSetExternal(0, 0);
}


void TextureCreateEx(char *name, struct numtl_s **mtl, struct nutex_s **tid, void *extmem, s32 *extend,
                     union variptr_u *addr, union variptr_u *addrend)
{
    union variptr_u a;

    if (NuFileSize(name) < 0) {
        *mtl = 0;
        *tid = 0;
        return;
    }
    if (addr != 0) {
        a = *addr;
        *tid = NuTexReadBitmapMMEx(name, 0, &a, addrend);
    } else {
        *tid = NuTexReadBitmapMM(name, 0);
    }
    *mtl = NuMtlCreate(1);
    (*mtl)->attrib.zmode = 3;
    (*mtl)->attrib.filter = 1;
    (*mtl)->attrib.lighting = 2;
    (*mtl)->attrib.utc = 1;
    (*mtl)->attrib.vtc = 1;
    (*mtl)->attrib.colour = 1;
    (*mtl)->attrib.alpha = 0;
    if (extmem != 0) {
        (*mtl)->tid = NuTexCreateEx(*tid, extmem, extend);
    } else {
        (*mtl)->tid = (s32)NuTexCreate(*tid);
    }
    NuMtlUpdate(*mtl);
    if (addr != 0) {
        *tid = 0;
    }
    NuMemSetExternal(0, 0);
}


extern void GlassGivenMaterial(struct numtl_s *mtl);
extern void EnviroGivenMaterial(struct numtl_s *mtl);
extern void SpecularGivenMaterial(struct numtl_s *mtl);
extern void FlickerPalGivenMaterial(struct numtl_s *mtl);

s32 MtlReadEventHandler(struct numtl_s *mtl)
{
    s32 handled = 0;

    if (mtl->fxid != 0 && !(mtl->fxid & 0x80)) {
        if (mtl->fxid & 1) {
            GlassGivenMaterial(mtl);
            handled = 1;
        }
        if (mtl->fxid & 0x10) {
            EnviroGivenMaterial(mtl);
        }
        if (mtl->fxid & 2) {
            SpecularGivenMaterial(mtl);
        }
        if (mtl->fxid & 8) {
            FlickerPalGivenMaterial(mtl);
        }
    }
    return handled;
}


extern void NuMemFreeFn(void *ptr, char *file, s32 line);

void TextureDestroy(struct numtl_s *mtl, struct nutexdesc_s *tex)
{
    if (mtl != 0) {
        NuMtlDestroy(mtl);
        if (mtl->tid != 0) {
            NuTexDestroy(mtl->tid);
        }
    }
    if (tex != 0) {
        if (tex->bits != 0) {
            NuMemFreeFn(tex->bits, D_0061AE18, 0x70A);
        }
        NuMemFreeFn(tex, D_0061AE18, 0x70B);
    }
}


extern struct vec3_s mg_player_actpos;
extern float mg_colldist;

s32 MGCheckRng(struct vec2_s *pos)
{
    float dx = pos->x - mg_player_actpos.x;
    float dy = pos->y - mg_player_actpos.y;
    return dx * dx + dy * dy < mg_colldist;
}


extern char D_0061B4E0[];

void BGLoadLevel(void)
{
    char *msg = D_0061B4E0;

    NuDisableVBlank();
    NuStrCpy(load_txt, msg);
    NuEnableVBlank();

    superbuffer_ptr = superbuffer_reset_base;
    BGLoadScreenInit(1);
    LoadLevel();
    bgload_complete = 1;
    for (;;) {
        NW_HOST_IDLE();
    }
}


extern char D_0061B530[];
extern s32 bg_cutmovie_id;
extern s32 bg_cutloadrv;

void BGLoadCutScene(void)
{
    LoadScreen(D_0061B530);
    BGLoadScreenInit(0);
    bg_cutloadrv = LoadCutMovie(bg_cutmovie_id);
    bgload_complete = 1;
    for (;;) {
        NW_HOST_IDLE();
    }
}


extern struct numtx_s numtx_identity;
extern struct vec3_s bgload_campos;
extern char *tLOADING[];
extern s32 mg_frame;
extern struct gtimer_s GameTimer;

extern int NuRndrSwapScreen(int target);
extern void BGLoadStart(void (*func)(void));
extern void BGLoadEnd(void);
extern void BGLoad(void);
extern void BGLoadSfx(void);
extern void NuPs2FlushVRam(s32 a, s32 b, s32 c);
extern void UpdateGameSfx(void);
extern void NuRndrLine3dDbgFlush(void);
extern void BGSuspendMain(void);
extern void NuSoundUpdate(void);
extern void TimeTunnelRender(void);
extern void TimeTunnelClose(void);
extern void DrawGameMessage(char *text, s32 frame, float y);
extern void UpdateTimer(struct gtimer_s *timer);

s32 BGLoadCutMovie(s32 movie)
{
    s32 done;

    if (NuRndrBeginScene(-1) != 0) {
        NuRndrEndScene();
        NuRndrSwapScreen(-1);
    }
    bg_cutmovie_id = movie;
    BGLoadStart(BGLoadCutScene);
    done = 0;
    fade_rate = -10;
    fadeval = 0xFF;
    NuVpSetSize(PS2_REZ_W << 4, PS2_REZ_H << 4);

    do {
        NuSoundUpdate();
        pNuCam->mtx = numtx_identity;
        NuMtxTranslate(&pNuCam->mtx, (struct nuvec_s *)&bgload_campos);
        NuCameraSet(pNuCam);
        if (NuRndrBeginScene(-1) != 0) {
            NuRndrClear(3, 0, 1.0f);
            TimeTunnelRender();
            if (font3d_scene != 0 && font3d_initialised != 0) {
                pNuCam->mtx = numtx_identity;
                NuCameraSet(pNuCam);
                if (mg_frame < 0) {
                    mg_frame = 0;
                } else {
                    mg_frame++;
                }
                DrawGameMessage(tLOADING[Game.language], mg_frame, 0.8f);
            }
            UpdateFade();
            DrawFade();
            UpdateTimer(&GameTimer);
            NuRndrEndScene();
        }
        BGSuspendMain();
        NuRndrVu1Draw();
        NuRndrSwapStreamBuffers();
        if (bgload_complete == 1) {
            fade_rate = 10;
            bgload_complete++;
        }
        if (bgload_complete != 0 && fadeval == 0xFF) {
            done = 1;
        }
    } while (done == 0);

    BGLoadEnd();
    if (NuRndrBeginScene(-1) != 0) {
        NuRndrEndScene();
        NuRndrSwapScreen(-1);
    }
    if (NuRndrBeginScene(-1) != 0) {
        NuRndrEndScene();
        NuRndrSwapScreen(-1);
    }
    TimeTunnelClose();
    fadeval = 0xFF;
    return bg_cutloadrv;
}

extern union variptr_u superbuffer_base;
extern struct nuvec_s ldir[1];
extern struct cammtx_s GameCam[];
extern int no_default_font;
extern int MAXVPSIZEX;
extern int MINVPSIZEX;
extern int MAXVPSIZEY;
extern int MINVPSIZEY;
extern void *app_tbset;
extern int last_level;
extern int highallocaddr;
extern int pad_play;
extern int logos_played;
extern s8 memdump_enabled;
#define fade_mtl D_00633260
extern int ForcePlayRecord;
extern int musicvol_mul;
extern int USELIGHTS;
extern int Bonus;
extern struct RPos_s* best_cRPos;
extern int D_0062FC6C;
#define dumpidx D_0062FC6C
extern float D_0062FA70;
#define gamma_val D_0062FA70
extern int ps2video_novsync;
extern int ps2video_ignore_frameout;
extern int D_0062FA2C;
#define fmvdump_enabled D_0062FA2C
extern int D_0062FC68;
#define odevframe_193 D_0062FC68
extern int plr_died;
extern void *app_fnt;
extern int last_character;
extern int last_used_character;
extern int D_0062FA28;
#define number_of_times_played D_0062FA28
extern int peak_poly_count;
extern int poly_count;
extern int nurndr_tritot_this_frame;
extern float D_0062D06C;
extern float POWERTEXTY;
extern int DRAWDEBRISHACK;
extern int DRAWWORLDHACK;
extern int iVIS;
extern struct nugspline_s *pVIS;
extern int save_paused;
extern int cut_on;
extern int DRAWCREATUREHACK;
extern float vtog_time;
extern float vtog_duration;
extern int in_finish_range;
extern s64 frameout_count;
extern volatile int nuvideo_global_vbcnt; //check
extern int frameout;
extern int FRAMES;
extern int FRAME;
extern int FixFrameRate;
extern int COMPLEXPLAYERSHADOW;
extern struct gtimer_s GlobalTimer;
extern int mg_wumpatot;
extern struct panelcount_s plr_wumpas;
extern int mask;
extern int mask_crates;
extern int crate_wumpa;
extern int NODEBRIS;
extern int GLASSPLAYER;
extern float plr_invisibility_time;
#define fadecol D_00633264
extern int VEHICLECONTROL;
extern int BLUR;
extern int LostLife;
extern int game_music;
extern int qseed;
extern struct rail_s Rail[8];
extern struct nulight_s* D_0062FAE8;
#define main_light D_0062FAE8
extern struct gtimer_s CutTimer;
extern struct nupadrec_s PadData;
extern float foot_u[];
extern float foot_v[];
extern int tempanim_waitaudio;
extern int cortex_gameover_i;
extern int gamesfx_channel;
extern int tempanim_nextaction;
extern struct nucamera_s global_camera;
extern char D_0062FC50[];
extern char D_0062FC58[];
extern char D_0062FC60[];
#define frmtmr_str D_0062FC58
#define Candle_str D_0062FC60
extern int gamecut;
extern int Hub;

extern struct nupad_s* NuPs2OpenPad(int port, int slot);
extern struct nulight_s* NuLightCreate();
extern void* NuMemAllocFn(int size, char* file, int line);
extern void DrawCreatures(struct creature_s * c, int count, int render, int shadow);
extern void NuLgtSetArcMat(void *material, float u0, float v0, float u1, float v1);
extern void ResetVehicleControl(int iRAIL, int iALONG, float fALONG);
extern void ResetTempCharacter(int character, int action);
extern void NuLightAddHazePoly(struct nuvec_s *pos, float size, float rot);
extern void InitCutScenes();
extern void NuWaterSpeed(float);
extern void NuLightFog(float near, float far, int colour, int blur, int haze_colour);
extern void NuMtlAnimate(float);
extern s32 edmainProcess(struct nupad_s *pad, f32 dt);
extern void CalculateGamePercentage(struct game_s*);
extern struct nucamera_s* NuCameraCreate(void);
extern void DrawPanel();
extern void ProcessCreatures();
extern void UpdateScreenWumpas();
extern void ProcDeb3();
extern void DrawParallax();
extern void GetSpaceCut(int, int);
extern void DoGlass(int);
extern void NuWaterRender(struct nugscn_s*);
extern void MoveGameCamera(struct cammtx_s *GameCamera, struct obj_s *obj);
extern void NuPs2VideoScreenDump(char * name, int lace, float gamma, int idx);
extern void Debris(int);
extern void ProcessVehicleLevel(struct nupad_s*);
extern void NuPs2InitVideo(int w, int h, int mode, int zmode, int fmt);
extern int DebrisCollisionCheck(struct nuvec_s *centre, float radius);
extern void DebrisRegisterCutoffCameraVec(struct nuvec_s *cutoff);
extern int HubFromLevel(int level);
extern void ResetGameCameras(struct cammtx_s *cameras, int count);
extern void ProcMenu(struct cursor_s *cursor, struct nupad_s *pad);
extern s32 DebrisFindAllOfType(s32 type, struct nuvec_s **list, s32 max, s32 skip, s32 activeonly);
extern int LookupDebrisEffect(char* name);

extern void AddBugLight();
extern void ApplyVisiTable(struct nugscn_s *sc, struct nuvec_s *pos);
extern void BGLoadInit(void);
extern void CloseCrates(void);
extern void CloseCutMovie(s32 all);
extern void CloseParticles(void);
extern void CloudFxInit(void);
extern void DebrisDraw();
extern void DebrisSetup(void);
extern void DefaultGame(void);
extern void DoClouds(s32 paused);
extern void DrawAwards(void);
extern void DrawBug(void);
extern void DrawCarpet(void);
extern void DrawChases(s32 render);
extern void DrawCrateExplosions(void);
extern void DrawCrates(void);
extern void DrawCutMovie(void);
extern void DrawDeb3(void);
extern void DrawExtraCreatures(void);
extern void DrawGameCut(void);
extern void DrawKabooms(void);
extern void DrawLevel(void);
extern void DrawMask(struct mask_s *mask);
extern void DrawMaskFeathers(void);
extern void DrawNextVehicle(s32 render);
extern void DrawProjectiles(void);
extern void DrawRings(void);
extern void DrawTarget(void);
extern void DrawTempCharacter(s32 render);
extern void DrawTempCharacter2(s32 render);
extern void DrawTransporters(void);
extern void DrawVehicleTrail(void);
extern void DrawWumpa(void);
extern void GameTiming(void);
extern void GameVP(void);
extern void InitCameraTargetMaterial(void);
extern void InitCrateExplosions(void);
extern void InitDeb3(void);
extern void InitGameMode();
extern void InitPlayerEvents(void);
extern void InstClose(void);
extern void InstInit(void);
extern s32 KillPlayer(struct obj_s *obj, s32 anim);
extern void LightEverythingInEditor(void);
extern void MaxVP(void);
extern void NewCut(s32 i);
extern void NewGame(void);
extern void NuFileInitEx(int deviceid, int rebootiop);
extern void NuFntClose(void);
struct nufont_s;
extern void NuFntDestroy(struct nufont_s *font);
extern void NuFntInit(void);
extern void NuFntScale(s32 xscale, s32 yscale);
extern void NuGobjInit(void);
extern void NuMemDumpFn(int min_size);
extern void NuPs2Init(void);
extern void NuPs2InitIOP(void);
extern void NuPs2InitVU(void);
extern void NuRandSeed(unsigned int seed);
extern void NuRndrEndSceneEx(int flags);
extern void NuRndrFootPrints(struct numtl_s *mtl, float *u, float *v);
extern void NuRndrFx(int mode, void *fxdata);
extern void NuRndrInitEx(int streamsize);
extern void NuRndrShadPolys(struct numtl_s *mtl);
extern void NuRndrShadowOnOff(int on);
extern void NuRndrWaterRip(struct numtl_s *mtl);
extern void NuRndrWaterRippleUpdate(int frames);
extern void NuSoundFlushLoops(void);
extern int NuSoundKeyStatus(unsigned int channel);
extern void NuSoundKillAllAudio(void);
extern int NuSoundStopStream(int stream);
extern void PlayCutMovie(s32 movie);
extern void ProcessGlass(s32 paused);
extern void ReleaseVisiTable(void);
extern void Reset3DFontObjects(void);
extern void ResetAI(void);
extern void ResetAwards(void);
extern void ResetBonus(void);
extern void ResetBug(void);
extern void ResetCrates(void);
extern void ResetDeath(void);
extern void ResetGame(void);
extern void ResetGates(void);
extern void ResetGemPath(void);
extern void ResetLevel(void);
extern void ResetPanelDebris(void);
extern void ResetPlayerEvents(void);
extern void ResetProjectiles(void);
extern void ResetRings(void);
extern void ResetTimeTrial(void);
extern void ResetWumpa(void);
extern void RotateDirectionalLight(struct nuvec_s *v, s32 xrot, s32 yrot);
extern void SetLevelLights(void);
extern void ShadComInit(void);
extern void UpdateAwards(void);
extern void UpdateBugLight(struct creature_s *c);
extern void UpdateChases(void);
extern void UpdateCrateExplosions(void);
extern void UpdateCrates(void);
extern void UpdateCutMovie(void);
extern void UpdateGameCut(void);
extern void UpdateKabooms(void);
extern void UpdateLevel(void);
extern void UpdateMaskFeathers(void);
extern void UpdatePanelDebris(void);
extern void UpdatePlayerStats(struct creature_s *plr);
extern void UpdateProjectiles(void);
extern void UpdateTempCharacter(void);
extern void UpdateWumpa(void);
extern void VehicleSetup(void);
extern void edmainRender(void);
extern struct TbSet *tbsetCreate(int *colours);
extern void tbslotBeginExFn(struct TbSet *set, int slot, char *name);
extern void tbslotBeginFn(struct TbSet *set, int slot);
extern int tbslotEndFn(void *tbset, int slot);
extern int tbslotLastValue(struct TbSet *set, int slot);
extern void tbslotReset(struct TbSet *set, int slot);

int main(int argc, char** argv) {
    int render_frame;
    struct creature_s* plr;
    int effect_count;
    int effects_remaining;
    int phase_value;
    s64 frame_delta;
    int timing_or_collision_result;
    int debris_type;
    struct nuvec_s **effect_cursor;
    int effect_offset;
    int previous_fade;
    float death_progress;
    float death_duration;
    float half;
    struct nuvec_s pos;
    struct nuvec_s* flamepos[32];
    int scene_started;
    int render_paused;
    int player_dying;
    int skipped_frame_ticks;
    int frameout_streak;
    int pause_frameout_grace;
    int peak_polygons;

    frameout_streak = 0;
    no_default_font = 0;
    superbuffer_base.voidptr = NuMemAllocFn(0x1200000, ".\\main.c", 0x579);
    superbuffer_end.intaddr = (superbuffer_base.intaddr + 0x1200000);
    superbuffer_reset_base.voidptr = superbuffer_base.voidptr;
    if (superbuffer_base.voidptr == 0) {
        NuErrorProlog(".\\main.c", 0x57c)("unable to allocate super buffer");
    }
    DefaultGame();
    ResetGame();
    NuPs2Init();
    NuFileInitEx(1, 1);
    Hub = -1;
    Level = -1;
    NuPs2InitIOP();
    InitCutScenes();
    SWIDTH = 0x280;
    SHEIGHT = 0x200;
    NuPs2InitVideo(0x280, 0x100, 0, 0x31, 3);
    NuPs2VideoSetPos(0, 0);
    MAXVPSIZEX = SWIDTH;
    MAXVPSIZEY = SHEIGHT;
    MINVPSIZEX = (SWIDTH * 0x18c) / 0x280;
    MINVPSIZEY = SHEIGHT / 2;
    NuPs2InitVU();
    NuTrigInit();
    NuVpInit();
    NuTexInit();
    NuGobjInit();
    NuMtlInit();
    NuRndrInitEx(0x200000);
    NuFntClose();
    NuLightInit();
    main_light = NuLightCreate();
    if (main_light != 0) {
        main_light->on = 1;
        main_light->diffuse.b = 1.0f;
        main_light->type = 0;
        main_light->ambient.r = 0;
        main_light->ambient.g = 0;
        main_light->ambient.b = 0;
        main_light->diffuse.r = 0;
        main_light->diffuse.g = 0;
        NuMtxSetRotationX(&main_light->mtx, 0xc000);
        NuLightUpdate(main_light);
    }
    pNuCam = NuCameraCreate();
    CloudFxInit();
    BGLoadInit();
    BGLoadSfx();
    Pad[0] = NuPs2OpenPad(0, 0);
    Pad[1] = NuPs2OpenPad(1, 0);
    app_tbset = tbsetCreate(0);
    if (Level != -1) {
        NewGame();
        CalculateGamePercentage(&Game);
        Hub = HubFromLevel(Level);
    } else {
        Level = 0x23;
    }
    last_level = Level;
    while (1) {
        srand(0);
        qseed = 0x3039;
        NuRandSeed(0);
        if (Level == 0x23) {
            NewGame();
            Hub = -1;
            NewMenu(&Cursor, -1, -1, -1);
            cutmovie = 0;
            Level = 0x25;
            Demo = 0;
        }
        LBIT = 1L << Level;
        LDATA = &LData[Level];
        if (pNuCam != 0) {
            if (LDATA->farclip >= 10) {
                pNuCam->farclip = LDATA->farclip;
            } else {
                pNuCam->farclip = 1000.0f;
            }
        }
        highallocaddr = 0;
        AIVISRANGE = 25.0f;
        NuTexInit();
        NuMtlRelease();
        NuGobjInit();
        InstInit();
        while (cutmovie != -1) {
            pad_play = 0;
            NuSoundKillAllAudio();
            InitPadPlayRecord(PadRecordPath, 0, 30000, &PadData);
            NuDebugMsgProlog(".\\main.c", 0x11f1)("Entering PlayCutMovie");
            PlayCutMovie(cutmovie);
            NuDebugMsgProlog(".\\main.c", 0x11f3)("Leaving PlayCutMovie");
            NewMenu(&Cursor, -1, -1, -1);
            cutmovie = -1;
            logos_played = 1;
        }
        NuSoundKillAllAudio();
        LBIT = 1L << Level;
        LDATA = &LData[Level];
        if (pNuCam != 0) {
            if (LDATA->farclip >= 10) {
                pNuCam->farclip = LDATA->farclip;
            } else {
                pNuCam->farclip = 1000.0f;
            }
        }
        AIVISRANGE = 25.0f;
        NuFntInit();
        InitCameraTargetMaterial();
        ShadComInit();
        if (memdump_enabled != '\0') {
            NuMemDumpFn(0);
        }
        DebrisSetup();
        VehicleSetup();
        DebrisRegisterCutoffCameraVec((struct nuvec_s *)&global_camera.mtx._30);
        fade_mtl = (struct numtl_s*)NuMtlCreate(1);
        fade_mtl->attrib.zmode = 3;
        fade_mtl->attrib.filter = 1;
        fade_mtl->attrib.lighting = 2;
        fade_mtl->attrib.utc = 1;
        fade_mtl->attrib.vtc = 1;
        fade_mtl->attrib.colour = 1;
        fade_mtl->attrib.alpha = 3;
        NuMtlUpdate(fade_mtl);
        NuDebugMsgProlog(".\\main.c", 0x120e)("Entering InitLocalSfx");
        InitLocalSfx(LDATA->pSFX, LDATA->nSFX);
        NuDebugMsgProlog(".\\main.c", 0x1210)("Leaving InitLocalSfx");
        ResetTimer(&GameTimer);
        MaxVP();
        NuDebugMsgProlog(".\\main.c", 0x1218)("Entering BGLOAD");
        NuSoundKillAllAudio();
        BGLoad();
        NuSoundKillAllAudio();
        NuDebugMsgProlog(".\\main.c", 0x1220)("Leaving BGLOAD");
        InitPauseRender();
        NuLgtSetArcMat(DebMat[5],0.5019531f, 0.74804688f, 0.56054688f, 0.99804688f);
        Reset3DFontObjects();
        InitPlayerEvents();
        if (Level == 0x28) {
            GameMode = 1;
        } else if (Level == 0x26) {
            ResetTempCharacter(2,0x22);
            tempanim_nextaction = 0x73;
            gamesfx_channel = 4;
            GameSfx(0xc6,0);
            cortex_gameover_i = -1;
            tempanim_waitaudio = 1;
        } else if (Level == 0x25) {
            if ((Game.cutbits & 1U) == 0) {
                NewCut(0);
                GameMode = 1;
            } else {
                GameMode = 0;
            }
        } else {
            GameMode = 0;
        }
        if (GameMode == 1) {
            if (Level == 0x25) {
                Hub = -1;
            }
            ResetTimer(&CutTimer);
        }
        if (ForcePlayRecord == 0) {
            pad_play = (Demo != 0 && pad_record == 0) ? 1 : 0;
        }

        if (pad_record != 0) {
            InitPadPlayRecord(PadRecordPath, 1, 30000, &PadData);
        } else if (pad_play != 0) {
            InitPadPlayRecord(PadRecordPath, 2, 30000, &PadData);
        } else {
            InitPadPlayRecord(PadRecordPath, 0, 30000, &PadData);
        }
        ResetBonus();
        ResetDeath();
        ResetGemPath();
        LostLife = 0;
        game_music = -1;
        if ((Cursor.menu != 0x11) && ((GameMode != 1 || (gamecut != 0)))) {
            GameMusic(LDATA->music[0], 0);
        }
        musicvol_mul = 0;
        USELIGHTS = 1;
        srand(0);
        qseed = 0x3039;
        NuRandSeed(0);
        NuDisableVBlank();
        NuStrCpy(load_txt, D_0062FC50);
        NuEnableVBlank();
next_game_mode:
        NuSoundUpdate();
        if (((Cursor.menu != 0x11) && (game_music != LDATA->music[0])) && ((GameMode != 1 || (gamecut != 0)))) {
            GameMusic(LDATA->music[0], 0);
        }
        InitCrateExplosions();
        if (Bonus != 4) {
            ResetBonus();
        }
        ResetDeath();
        ResetCrates();
        ResetGemPath();
        ResetWumpa();
        NuSoundUpdate();
        ResetChases();
        ResetPlayerEvents();
        ResetGates();
        ResetRings();
        ResetMaskFeathers();
        ResetTimeTrial();
        ResetAI();
        NuSoundUpdate();
        InitGameMode();
        ResetBug();
        ResetLevel();
        ResetVehicleControl(player->obj.RPos.iRAIL, player->obj.RPos.iALONG, player->obj.RPos.fALONG);
        InitDeb3();
        NuSoundUpdate();
        ResetProjectiles();
        pause_dir = 0;
        Paused = 0;
        ResetPanelDebris();
        if (Pad[0] != 0) {
            NuPs2PadSetMotors(Pad[0], 0, 0);
        }
        if (Pad[1] != 0) {
            NuPs2PadSetMotors(Pad[1], 0, 0);
        }
        ResetGameSfx();
        player_dying = 0;
        vtog_time = vtog_duration = 0;
        ResetGameCameras(GameCam, 1);
        in_finish_range = 0;
        ResetAwards();
        NuSoundUpdate();
        NuRndrSwapStreamBuffers();
        (void)nuvideo_global_vbcnt;
        fade_rate = -10;
        frameout = 0;
        frameout_count = nuvideo_global_vbcnt;
        do {
            NW_HOST_IDLE();
        } while (frameout_count == nuvideo_global_vbcnt);
        frameout_count = nuvideo_global_vbcnt;
        half = 0.5f;
        pause_frameout_grace = 0;
        frameout = 0;
        while (((new_mode == -1) && (new_level == -1)) || fadeval < 0xff || (fadehack != 0)) {
            tbslotBeginFn(app_tbset, 0);
            FRAMES = frameout + 1;
            if (((FixFrameRate != 0) || (pad_record != 0)) || (pad_play != 0)) {
                FRAMES = 1;
            }
            plr = player;
            for (FRAME = 0; FRAME < FRAMES; FRAME++) {
                if (FRAME < FRAMES - 1) {
                    tbslotReset(0, 0xf);
                    tbslotBeginExFn(0, 0xf, frmtmr_str);
                }

                if (FRAME == 0) {
                    tbslotBeginFn(app_tbset, 1);
                }

                ProcessGlass(Paused);
                NuRndrShadowOnOff(COMPLEXPLAYERSHADOW);

                if (149U < GameTimer.frame) {
                    if (((GlobalTimer.frame % 50 == 25) && (fadeval == 0))
                        && ((NuSoundKeyStatus(0) != 1) || (NuSoundKeyStatus(1) != 1))
                        && (Bonus != 1 && (Bonus != 3)))
                    {
                        GameMusic(game_music, 0);
                    }
                }

                if (mg_wumpatot != 0) {
                    mg_wumpatot--;
                    plr_wumpas.count = plr_wumpas.count + 1;
                }

                DoInput();
                if ((Demo != 0) && ((pad_play != 0) && (NuPs2PadDemoEnd() != 0) || (60.0f <= GameTimer.ftime) || ((Level == 12) && (GameTimer.frame >= 2100U)))) {
                    new_level = 0x23;
                }

                UpdateGameSfx();
                NuSoundUpdate();

                if (editor_active != 0) {
                    if (edmainProcess(Pad[0], 0.016666668f) == 1) {
                        editor_active = 0;
                        Pad[0]->paddata_db = Pad[0]->paddata = 0;
                    }
                    if ((LDATA->flags & 8) != 0) {
                        TerrainPlatformOldUpdate();
                    }
                    NuGScnUpdate(world_scene[0], 1.0f);
                    edobjUpdateObjects(1.0f);
                    edanimUpdateObjects(1.0f);
                    NuBridgeUpdate(&Character[0].obj.pos.x);
                    NuWindUpdate(&Character[0].obj.pos.x);

                    if (NODEBRIS == 0) {
                        Debris(0);
                    }

                    if ((LDATA->flags & 8) != 0) {
                        TerrainPlatformNewUpdate();
                    }
                    LightEverythingInEditor();
                } else {
                    if (Paused != 0) {
                        if (Pad[0] != 0) {
                            NuPs2PadSetMotors(Pad[0], pausebuzz != 0, 0);
                        }
                        if (Pad[1] != 0) {
                            NuPs2PadSetMotors(Pad[1], 0, 0);
                        }
                        pNuCam->mtx = GameCam[0].m;
                        NuCameraSet(pNuCam);
                        if ((Pad[0] != 0) && ((Pad[0]->paddata_db & 0xa0) != 0)) {
                            pausestats_frame = (pausestats_frame + 150) - (pausestats_frame % 150U);
                            GameSfx(0x36, 0);
                        }
                    } else {
                        mask_crates = 0;
                        crate_wumpa = 0;
                        RotateDirectionalLight(ldir, -0x2000, ((int)GameTimer.frame % 400U << 0x10) / 400U);

                        for (phase_value = 0; phase_value <= 8; phase_value++) {
                            Character[phase_value].anim_processed = 0;
                        }

                        SetLevelLights();
                        SetTexAnimSignals();
                        if ((LDATA->flags & 8) != 0) {
                            TerrainPlatformOldUpdate();
                        }
                        if (world_scene[0] != 0) {
                            NuGScnUpdate(world_scene[0], 1.0f);
                        }
                        edobjUpdateObjects(1.0f);
                        edanimUpdateObjects(1.0f);
                        NuRndrWaterRippleUpdate(1);
                        NuBridgeUpdate(&Character[0].obj.pos.x);
                        NuWindUpdate(&Character[0].obj.pos.x);
                        if ((LDATA->flags & 8) != 0) {
                            TerrainPlatformNewUpdate();
                        }
                        if ((LDATA->flags & 1) != 0) {
                            if (FRAME == 0) {
                                tbslotBeginFn(app_tbset, 5);
                            }
                            ManageCreatures();
                            ProcessCreatures();
                            if (FRAME == 0) {
                                tbslotEndFn(app_tbset, 5);
                            }
                        }
                        if (Cursor.menu == 0x11) {
                            UpdateCutMovie();
                        }
                        if (level_part_2 == 0) {
                            UpdateLevel();
                            UpdateKabooms();
                            UpdateWumpa();
                            UpdateCrates();
                            UpdateMaskFeathers();
                            UpdateCrateExplosions();
                            UpdateChases();
                            UpdateProjectiles();
                            UpdateBugLight(plr);
                            UpdateGameCut();
                        }
                        if (FRAME == 0) {
                            tbslotBeginFn(app_tbset, 2);
                        }
                        ProcessVehicleLevel(Pad[0]);
                        if (FRAME == 0) {
                            tbslotEndFn(app_tbset, 2);
                        }
                        ProcDeb3();

                        if ((NODEBRIS == 0)) {
                            Debris(0);
                            if ((GLASSPLAYER == 0 || !(plr_invisibility_time < 5.0f))) {
                                pos.x = (player->obj).pos.x;
                                pos.y = (player->obj).pos.y + ((player->obj).bot + (player->obj).top) * (player->obj).SCALE * half;
                                pos.z = (player->obj).pos.z;
                                timing_or_collision_result = DebrisCollisionCheck(&pos, player->obj.RADIUS);
                                if (timing_or_collision_result != -1) {
                                    KillPlayer(&player->obj, 0x14);
                                }
                            }
                        }
                        NuMtlAnimate(0.02f);
                        UpdateTempCharacter();
                        UpdateAwards();
                        MoveGameCamera(GameCam, &plr->obj);
                        if (PLAYERCOUNT != 0) {
                            NuRndrSetSpecularLight(&((player->lights).pDir1st)->Direction, 0);
                        } else {
                            NuRndrSetSpecularLight(ldir, 0);
                        }
                        UpdateScreenWumpas();
                        if (PLAYERCOUNT != 0) {
                            UpdatePlayerStats(plr);
                        }
                        UpdatePanelDebris();
                    }
                }
                if ((FRAME == FRAMES - 1) && (pause_rndr_on == 0)) {
                    AddBugLight(plr);
                }
                GameTiming();
                ProcMenu(&Cursor, Pad[0]);
                previous_fade = fadeval;
                fadeval += fade_rate;
                if (fadeval >= 0x100) {
                    fadeval = 0xff;
                } else if (fadeval < 0) {
                    fadeval = 0;
                }

                if ((previous_fade < 0xff) && (fadeval == 0xff)) {
                    fadehack = 1;
                } else if (fadehack != 0) {
                    fadehack--;
                }

                if ((fadeval == 0) || (fadeval == 0xff)) {
                    fade_rate = 0;
                }

                fadecol = fadeval | fadeval << 8 | fadeval << 0x10 | 0x80000000;
                if ((Level == 2) && (VEHICLECONTROL != 1)) {
                    NuLightFog(4.0f, 16.0f, 0x40000000, BLUR, 0);
                } else {
                    NuLightFog(
                        LDATA->fognear, LDATA->fogfar,
                        LDATA->foga << 0x18 | LDATA->fogb << 0x10 | LDATA->fogg << 8 | LDATA->fogr, BLUR,
                        LDATA->hazea << 0x18 | LDATA->hazeb << 0x10 | LDATA->hazeg << 8 | LDATA->hazer
                    );
                }
                if (FRAME == 0) {
                    tbslotEndFn(app_tbset, 1);
                }

                if (pause_rndr_on == 0) {
                    if (FRAME == FRAMES - 1) {
                        tbslotBeginFn(app_tbset, 9);
                        pCam = GameCam;
                        if ((scene_started = NuRndrBeginScene(-1)) != 0) {
                            GameVP();
                            NuRndrClear(7, 0, 1.0f);
                            if (level_part_2 == 0) {
                                DrawParallax();
                            }
                        }
                    }

                    player_dying = plr->obj.dead == '\x02';
                    render_frame = FRAME == FRAMES - 1;
                    if (FRAME == FRAMES - 1) {
                        tbslotBeginFn(app_tbset, 0xc);
                    }
                    if ((LDATA->flags & 1) != 0 && DRAWCREATUREHACK != 0 && cut_on == 0) {
                        if ((player_dying == 0)
                            && ((GLASSPLAYER == 0) || !(plr_invisibility_time < 5.0f) || (Level == 0x17)))
                        {
                            DrawCreatures(&Character[0], 1, render_frame, 1);
                        }
                        if ((level_part_2 == 0) && (DrawCreatures(&Character[1], 8, render_frame, 1), Level == 0x1c)) {
                            DrawCreatures(&OppTubCreature, 1, render_frame, 0);
                        }
                        DrawNextVehicle(render_frame);
                        if (FRAME == FRAMES - 1) {
                            DrawExtraCreatures();
                        }
                    }

                    if (FRAME == FRAMES - 1) {
                        tbslotEndFn(app_tbset, 0xc);
                    }

                    if (cut_on == 0) {
                        DrawChases(render_frame);
                        DrawTempCharacter(render_frame);
                        DrawTempCharacter2(render_frame);
                    }
                }
                if (FRAME != FRAMES - 1) {
                    skipped_frame_ticks = tbslotEndFn(0, 0xf);
                }
            }

            if (FRAMES == 1) {
                skipped_frame_ticks = 0;
            }

            if (screendump != 0) {
                render_paused = save_paused;
            } else {
                render_paused = Paused;
            }

            if (pause_rndr_on == 0) {
                if (scene_started != 0) {
                    NuWaterSpeed((render_paused) ? 0 : 1.2f);
                    if ((LDATA->flags & 4) != 0) {
                        tbslotBeginFn(app_tbset, 10);
                        if (world_vd != 0) {
                            if (pVIS != 0) {
                                visiSetSplineKnot(world_vd, pVIS - world_scene[0]->splines, iVIS);
                            }
                        } else {
                            ApplyVisiTable(world_scene[0], &pCam->pos);
                        }
                        if (DRAWWORLDHACK != 0) {
                            DrawWorld();
                        }
                        tbslotEndFn(app_tbset, 10);
                    }
                    if (cut_on == 0) {
                        DrawVehicleTrail();
                    }
                    if ((LDATA->flags & 0x10) != 0) {
                        effect_offset = 0;
                        debris_type = LookupDebrisEffect(Candle_str);
                        effect_count = DebrisFindAllOfType(debris_type, flamepos, 0x20, effect_offset, 1);
                        while (effect_count != 0) {
                            if (effect_count > 0) {
                                effects_remaining = effect_count;
                                effect_cursor = flamepos;
                                do {
                                    effects_remaining--;
                                    NuLightAddHazePoly(*effect_cursor++, 0.6f, 1.2f);
                                } while (effects_remaining != 0);
                            }
                            effect_offset += effect_count;
                            effect_count = DebrisFindAllOfType(debris_type, flamepos, 0x20, effect_offset, 1);
                        }
                    }
                    if (cut_on == 0) {
                        DrawDeb3();
                        DrawProjectiles();
                    }
                    if ((level_part_2 == 0) && (cut_on == 0)) {
                        tbslotBeginFn(app_tbset, 0xd);
                        DrawCrates();
                        DrawCrateExplosions();
                        tbslotEndFn(app_tbset, 0xd);
                        tbslotBeginFn(app_tbset, 0xe);
                        DrawWumpa();
                        tbslotEndFn(app_tbset, 0xe);
                        if ((LDATA->flags & 1) != 0) {
                            if (plr->used != 0 && plr->obj.mask != 0 &&
                                plr->obj.mask->active != 0 && (LDATA->flags & 0xe00) == 0 &&
                                (VEHICLECONTROL != 1 || (LBIT & 0x1005042000UL) == 0)) {
                                DrawMask(plr->obj.mask);
                            }
                            DrawMaskFeathers();
                        }
                        DrawKabooms();
                        DrawTransporters();
                        DrawAwards();
                        DrawBug();
                        DrawRings();
                        DrawTarget();
                        DrawLevel();
                        DrawGameCut();
                        if (Level == 0x1d) {
                            DrawCarpet();
                        }
                        NuRndrShadPolys(ShadowMat);
                        NuRndrWaterRip(DebMat[4]);
                    }
                    if (Cursor.menu == 0x11) {
                        DrawCutMovie();
                    }
                    if (cut_on == 0) {
                        NuRndrFootPrints(DebMat[4], foot_u, foot_v);
                    }
                    if (editor_active != 0) {
                        NuFntScale(0xc, 0xc);
                        edmainRender();
                    }
                    tbslotEndFn(app_tbset, 9);
                    NuRndrLine3dDbgFlush();
                    NuRndrEndScene();
                    tbslotBeginFn(app_tbset, 0xf);
                    DoGlass(editor_active);
                    if ((editor_active == 0) && (NuWaterRender(world_scene[0]), editor_active == 0)) {
                        NuRndrFx((Cursor.menu == 0xA) ? 0 : render_paused, &Character[0].obj.pos);
                    }
                    tbslotEndFn(app_tbset, 0xf);
                    NuRndrBeginScene(-1);
                    if (((LBIT & 0x200000a1) != 0) && (cut_on == 0)) {
                        DoClouds(Paused);
                    }
                    if (((LDATA->flags & 0x10) != 0) && (NODEBRIS == 0) && (DRAWDEBRISHACK != 0)) {
                        if (editor_active != 0) {
                            DebrisDraw(0);
                        } else {
                            DebrisDraw(render_paused);
                        }
                    }
                    NuRndrEndSceneEx(0);
                }
                tbslotEndFn(app_tbset, 0);
            }

            HandlePauseRender();
            if (NuRndrBeginScene(-1) != 0) {
                if ((player_dying != 0) && (PLAYERCOUNT != 0)) {
                    death_duration = plr->obj.die_duration;
                    death_progress = plr->obj.die_time * 3.0f;
                    if (death_duration < death_progress) {
                        death_progress = death_duration;
                    }
                    phase_value = ((death_progress / death_duration) * 255.0f);
                    NuRndrRect2di(
                        0, 0, SWIDTH << 4, SHEIGHT << 3, phase_value | phase_value << 8 | phase_value << 0x10 | 0x80000000,
                        fade_mtl
                    );

                } else {
                    if (Cursor.menu == '\x1f') {
                        int panel_y;
                        int end_y;
                        panel_y = ((1.0f - (POWERTEXTY + D_0062D06C + 1.0f) * half) * (float)(SHEIGHT << 3));
                        end_y = ((1.0f - ((POWERTEXTY - 0.75f) + 1.0f) * half) * (float)(SHEIGHT << 3));
                        NuRndrRect2di(
                            0,
                            panel_y,
                            SWIDTH << 4,
                            end_y - panel_y,
                            0x18ffffff,
                            fade_mtl
                        );
                    }
                }
                NuRndrClear(6, 0, 1.0f);
                if (player_dying != 0) {
                    DrawCreatures(&Character[0], 1, 1, 0);
                }
                DrawPanel();
                if ((new_mode != -1) || (new_level != -1)) {
                    fade_rate = 10;
                }
                if (fadeval != 0) {
                    NuRndrRect2di(0, 0, SWIDTH << 4, SHEIGHT << 3, fadecol, fade_mtl);
                }
                NuRndrEndScene();
            }
            timing_or_collision_result = (int)frameout_count;
            frameout_count = nuvideo_global_vbcnt;
            frame_delta = frameout_count - timing_or_collision_result - 1;
            if (frame_delta < 0) {
                frame_delta = 0;
            }
            frameout = frame_delta;
            if (pause_frameout_grace != 0) {
                pause_frameout_grace = pause_frameout_grace - 1;
                frameout = 0;
            }
            if (pause_rndr_on != 0) {
                pause_frameout_grace = 2;
            }
            if (frameout != 0) {
                frameout_streak++;
                if (frameout_streak > 5) {
                    phase_value = tbslotLastValue(0, 0);
                    timing_or_collision_result = tbslotLastValue(0, 1);
                    if ((timing_or_collision_result < 285) && (phase_value - skipped_frame_ticks < 285)) {
                        frameout = 0;
                    }
                    frameout_streak = 6;
                }
            } else {
                frameout_streak = 0;
            }
            if (frameout > 2) {
                frameout = 2;
            }
            peak_polygons = peak_poly_count;
            poly_count = nurndr_tritot_this_frame;
            if (peak_polygons < poly_count) {
                peak_polygons = poly_count;
            }
            peak_poly_count = peak_polygons;
            NuRndrSwapScreen(-1);
            if (fmvdump_enabled == 0) {
                continue;
            }

            ps2video_ignore_frameout = 1;
            odevframe_193 = 1 - odevframe_193;
            ps2video_novsync = 0;
            if (odevframe_193 == 0) {
                frameout = 0;
                continue;
            }
            NuPs2VideoScreenDump("fmvdump\\frm", 1, gamma_val, dumpidx);
            dumpidx++;

            if (NuRndrBeginScene(-1) != 0) {
                NuRndrEndScene();
                NuRndrSwapScreen(-1);
            }

            if (NuRndrBeginScene(-1) != 0) {
                NuRndrEndScene();
                NuRndrSwapScreen(-1);
            }

            if (NuRndrBeginScene(-1) == 0) {
                frameout = 0;
                continue;
            }
            NuRndrEndScene();
            NuRndrSwapScreen(-1);
            frameout = 0;
        }
        pause_dir = 0;
        Paused = 0;
        if ((plr_died != 0) && (best_cRPos != 0) && (Rail[best_cRPos->iRAIL].type == '\0')) {
            LostLife = 1;
        }

        plr_died = 0;
        for (phase_value = 2; phase_value > 0; phase_value--) {

            if (NuRndrBeginScene(-1) != 0) {
                NuRndrEndScene();
            }
            NuRndrSwapScreen(-1);
        }

        if (new_mode != -1) {
            NuSoundStopStream(4);
            NuSoundUpdate();
            GameMode = new_mode;
            goto next_game_mode;
        }
        PauseGameAudio(1);
        NuSoundFlushLoops();
        NuSoundKillAllAudio();
        NuSoundUpdate();
        if (pause_src_mtl != 0) {
            NuMtlDestroy(pause_src_mtl);
        }
        if (pause_rndr_mtl != 0) {
            NuMtlDestroy(pause_rndr_mtl);
        }
        if (pause_rt != 0) {
            NuTexDestroy(pause_rt);
        }
        pause_src_mtl = 0;
        pause_rndr_mtl = 0;
        pause_rt = 0;
        if (Pad[0] != 0) {
            NuPs2PadSetMotors(Pad[0], 0, 0);
        }
        if (Pad[1] != 0) {
            NuPs2PadSetMotors(Pad[1], 0, 0);
        }
        ReleaseVisiTable();
        CloseCutMovie(0);
        CloseCreatures();
        CloseParticles();
        CloseCrates();
        InstClose();
        if (app_fnt != 0) {
            NuFntDestroy(app_fnt);
        }
        app_fnt = 0;
        NuFntClose();
        if (PLAYERCOUNT != 0) {
            last_character = plr->obj.character;
        } else {
            last_character = -1;
        }

        if (last_character != -1) {
            last_used_character = last_character;
        }
        last_level = Level;
        last_hub = HubFromLevel(Level);
        GetSpaceCut(last_level, new_level);
        Level = new_level;
        number_of_times_played++;
    }
}


extern struct nuvec_s v000;
extern float NuTrigTable[];
extern struct nuvec_s nuvec_zero;
extern struct nuvec_s mg_player_pos;
extern struct nuvec_s bgload_objpos[2];
extern struct CharacterModel *bgload_model[2];
extern float bgload_animtime[2];
extern s32 mg_player_tilt;
extern s32 mg_player_spin;
extern float mg_scoreoff;
extern float DIVPANEL3D;
extern float PANEL3DMULX;
extern float PANEL3DMULY;
extern float bgload_objstart;
extern float bgload_objtarg;
extern float bgload_bumppos;
extern float bgload_objspd;
extern u16 bobrate;
extern float bobscale;
extern float fallrate;
extern float maxfallspd;
extern s32 camtiltrate;
extern s32 camrotrate;
extern float camtiltscale;
extern float camrotscale;
extern float bump_scale;
extern s32 bgloadgibberscalecnt;
extern s32 bgloadgibberscalecntrate;
extern s32 bgloadgibbercntx;
extern s32 bgloadgibberratex;
extern s32 bgloadgibbercnty;
extern s32 bgloadgibberratey;
extern s32 gamesfx_effect_volume;

extern s32 D_0062FC40[];

extern void GameSfxLoop(s32 sfx, struct nuvec_s *pos);
extern void NuHGobjEvalAnim(struct NUHGOBJ_s *hobj, struct nuanimdata_s *data,
                          float time, s32 nJ, struct NUJOINTANIM_s *pJ,
                          struct numtx_s *tmtx);
extern s32 NuHGobjRndrMtxDwa(struct NUHGOBJ_s *hobj, struct numtx_s *m,
                           s32 nlayers, short *layers, struct numtx_s *tmtx,
                           float **dwa);


void BGLoad(void)
{
    struct numtx_s m[2];
    struct numtx_s tmtx[256];
    short layers[5] = {0, 1, 2, 3, 4};
    float fall_speed[2];
    struct nucolour3_s ambient = {0.4f, 0.4f, 0.4f};
    struct nuvec_s light_dir[2];
    struct nuvec_s down = {0.0f, -1.0f, 0.0f};
    struct nucolour3_s light_colour[2];
    s32 light_angle[2];
    struct nuvec_s bump;
    s32 phase;
    s32 done;
    s32 render_characters;
    s32 bob_angle;
    s32 camera_tilt;
    s32 camera_rotation;
    s32 bumped;
    s32 bump_angle;
    s32 bump_rate;
    s32 character;
    s32 old_character;
    s32 previous_fade;
    float light_intensity;
    float score_scale;
    float anim_step;
    float gibber_scale;
    float gibber_x;
    float gibber_y;

    camera_tilt = 0;
    memset(light_colour, 0, sizeof(light_colour));
    camera_rotation = 0;
    memcpy(light_angle, D_0062FC40, sizeof(light_angle));
    bump_rate = bump_angle = bumped = 0;
    score_scale = 0.4f;
    memset(&bump, 0, sizeof(bump));
    mg_player_pos = v000;
    mg_player_tilt = 0;
    mg_player_spin = 0;
    PANEL3DMULX = (float)SWIDTH / (DIVPANEL3D * ((float)SWIDTH / 640.0f));
    PANEL3DMULY = (float)SHEIGHT / (DIVPANEL3D * ((float)SHEIGHT / 448.0f));
    font3d_scene = 0;
    if (LDATA->flags & 0x40) {
        LOAD_MESSAGE(D_0061B2E8);
        CrateMat = CreateAlphaBlendTexture256_32(D_0061B308, 0, 1, 0x100, 0);
    }

    mg_frame = -1;
    bgload_model[1] = 0;
    bgload_animtime[0] = 1.0f;
    bgload_model[0] = 0;
    old_character = -1;
    bgloadchr[1] = 0;
    bgloadchr[0] = 0;
    phase = 1;
    bgloadanim[1] = 0;
    bgloadanim[0] = 0;
    bgload_animtime[1] = 1.0f;
    character = LDATA->character;
    bgload_objpos[0].y = bgload_objpos[1].y = bgload_objstart;
    mg_scoreoff = 1.0f;
    MiniGameReset();
    if (character != last_used_character && last_used_character != -1 &&
        character != -1) {
        old_character = last_used_character;
        phase = 0;
        bgloadlastcharacter = old_character;
    }
    if (NuRndrBeginScene(-1) != 0) {
        NuRndrEndScene();
        NuRndrSwapScreen(-1);
    }
    BGLoadStart(BGLoadLevel);
    anim_step = 0.48f;
    fall_speed[0] = fall_speed[1] = 0.0f;
    fade_rate = -10;
    fadeval = 0xff;
    bob_angle = 0;
    done = 0;
    NuVpSetSize(PS2_REZ_W << 4, PS2_REZ_H << 4);
    render_characters = (u16)LDATA->character > 1 ? 0 : 2;

    while (1) {
        UpdateGameSfx();
        NuSoundUpdate();
        if (NuRndrBeginScene(-1) != 0) {
            NuRndrClear(3, 0, 1.0f);
            if (render_characters != 0) {
                light_intensity = NuTrigTable[light_angle[0] & 0xffff];
                NuVecRotateX(&light_dir[0], &down, light_angle[0]);
                NuVecRotateY(&light_dir[0], &light_dir[0], 0x2000);
                light_angle[0] = (light_angle[0] + 0x71c) & 0x7fff;
                light_colour[0].r = light_intensity;
                light_intensity = NuTrigTable[light_angle[1] & 0xffff];
                NuVecRotateX(&light_dir[1], &down, light_angle[1]);
                NuVecRotateY(&light_dir[1], &light_dir[1], -0x2000);
                light_angle[1] = (light_angle[1] + 0x71c) & 0x7fff;
                light_colour[1].b = light_intensity;
                NuRndrSetDirectionalLights(&light_dir[0], &light_colour[0],
                    &light_dir[1], &light_colour[1], &light_dir[0],
                    (struct nucolour3_s *)&nuvec_zero);
                NuRndrSetAmbientLight(&ambient);
                pNuCam->mtx = numtx_identity;
                bgload_campos.y = NuTrigTable[camera_tilt & 0xffff] * camtiltscale;
                bgload_campos.x = NuTrigTable[camera_rotation & 0xffff] * camrotscale;
                camera_tilt += camtiltrate;
                camera_rotation += camrotrate;
                NuMtxTranslate(&pNuCam->mtx, (struct nuvec_s *)&bgload_campos);
                NuCameraSet(pNuCam);
                GameCam[0].vX = *(struct nuvec_s *)&pNuCam->mtx._00;

                if (render_characters == 2 && (u32)character < 2) {
                    if (bgload_model[character] != 0) {
                        bgloadchr[character] = bgload_model[character]->obj;
                    }
                    switch (phase) {
                    case 0:
                        if (bgloadchr[old_character] != 0 &&
                            bgloadanim[old_character] != 0) {
                            NuMtxSetTranslation(&m[old_character], &bgload_objpos[old_character]);
                            MiniGame(0);
                            NuMtxPreRotateZ(&m[old_character], mg_player_tilt);
                            NuMtxTranslate(&m[old_character], &mg_player_pos);
                            bgload_animtime[old_character] += anim_step;
                            if (bgload_animtime[old_character] > bgloadanim[old_character]->time) {
                                bgload_animtime[old_character] -= bgloadanim[old_character]->time - 1.0f;
                            }
                            if (bgload_objpos[old_character].y > bgload_objtarg) {
                                bgload_objpos[old_character].y -=
                                    bgload_objspd < NuFabs(bgload_objpos[old_character].y - bgload_objtarg) * 0.3f
                                    ? bgload_objspd
                                    : NuFabs(bgload_objpos[old_character].y - bgload_objtarg) * 0.3f;
                            }
                            if (NuFabs(bgload_objpos[old_character].y - bgload_objtarg) < 0.01f) {
                                phase = 1;
                            }
                        }
                        break;
                    case 1:
                        if (bgloadchr[character] != 0 && bgloadanim[character] != 0) {
                            NuMtxSetTranslation(&m[character], &bgload_objpos[character]);
                            NuMtxTranslate(&m[character], &bump);
                            MiniGame(0);
                            NuMtxPreRotateZ(&m[character], mg_player_tilt);
                            NuMtxTranslate(&m[character], &mg_player_pos);
                            bgload_animtime[character] += anim_step;
                            if (bgload_animtime[character] > bgloadanim[character]->time) {
                                bgload_animtime[character] -= bgloadanim[character]->time - 1.0f;
                            }
                            if (bgload_objpos[character].y > bgload_objtarg) {
                                bgload_objpos[character].y -=
                                    bgload_objspd < NuFabs(bgload_objpos[character].y - bgload_objtarg) * 0.3f
                                    ? bgload_objspd
                                    : NuFabs(bgload_objpos[character].y - bgload_objtarg) * 0.3f;
                            }
                            if (bgload_objpos[character].y < bgload_bumppos &&
                                bumped == 0 && old_character != -1) {
                                gamesfx_effect_volume = 0x2aaa;
                                bumped = 1;
                                bump_rate = 0x444;
                                GameSfx(2, 0);
                            }
                            if (bump_rate != 0) {
                                bump_angle += bump_rate;
                                if (bump_angle > 0x7fff) {
                                    bump_rate = 0;
                                }
                                bump.y = NuTrigTable[bump_angle & 0xffff] * bump_scale;
                            }
                            if (NuFabs(bgload_objpos[character].y - bgload_objtarg) < 0.01f) {
                                bob_angle = 0;
                                phase = 2;
                            }
                        }
                        /* The departing character is animated during both phases. */
                        if (old_character != -1 && bgloadchr[old_character] != 0 &&
                            bgloadanim[old_character] != 0) {
                            if (bgload_objpos[character].y < bgload_bumppos) {
                                if (bgload_objpos[old_character].y > -5.0f) {
                                    bgload_objpos[old_character].y -= fall_speed[old_character];
                                    fall_speed[old_character] =
                                        fall_speed[old_character] + fallrate < maxfallspd
                                        ? fall_speed[old_character] + fallrate : maxfallspd;
                                } else {
                                    NuHGobjDestroy(bgloadchr[old_character]);
                                    bgloadchr[old_character] = 0;
                                    bgloadanim[old_character] = 0;
                                    goto bgload_render_characters;
                                }
                            } else {
                                bgload_objpos[old_character].y = bgload_objtarg +
                                    NuTrigTable[bob_angle] * bobscale;
                                bob_angle = (bob_angle + bobrate) & 0xffff;
                            }
                            bgload_animtime[old_character] += anim_step;
                            if (bgload_animtime[old_character] > bgloadanim[old_character]->time) {
                                bgload_animtime[old_character] -= bgloadanim[old_character]->time - 1.0f;
                            }
                            NuMtxSetTranslation(&m[old_character], &bgload_objpos[old_character]);
                            NuMtxPreRotateZ(&m[old_character], mg_player_tilt);
                            NuMtxTranslate(&m[old_character], &mg_player_pos);
                        }
                        break;
                    case 2:
                        NuMtxSetTranslation(&m[character], &bgload_objpos[character]);
                        NuMtxPreRotateY(&m[character], mg_player_spin);
                        NuMtxPreRotateZ(&m[character], mg_player_tilt);
                        NuMtxTranslate(&m[character], &bump);
                        MiniGame((struct nuvec_s *)&m[character]._30);
                        NuMtxTranslate(&m[character], &mg_player_pos);
                        bgload_objpos[character].y = bgload_objtarg + NuTrigTable[bob_angle] * bobscale;
                        bob_angle = (bob_angle + bobrate) & 0xffff;
                        if (bump_rate != 0) {
                            bump_angle += bump_rate;
                            if (bump_angle > 0x7fff) {
                                bump_rate = 0;
                            }
                            bump.y = NuTrigTable[bump_angle & 0xffff] * bump_scale;
                        }
                        bgload_animtime[character] += anim_step;
                        if (bgload_animtime[character] > bgloadanim[character]->time) {
                            bgload_animtime[character] -= bgloadanim[character]->time - 1.0f;
                        }
                        if (bgload_complete != 0) {
                            phase = 3;
                            fade_rate = 10;
                        }
                        if (old_character != -1 && bgloadchr[old_character] != 0 &&
                            bgloadanim[old_character] != 0) {
                            if (bgload_objpos[character].y < bgload_bumppos) {
                                if (!(bgload_objpos[old_character].y > -5.0f)) {
                                    NuHGobjDestroy(bgloadchr[old_character]);
                                    bgloadchr[old_character] = 0;
                                    bgloadanim[old_character] = 0;
                                    goto bgload_render_characters;
                                }
                                bgload_objpos[old_character].y -= fall_speed[old_character];
                                fall_speed[old_character] =
                                    fall_speed[old_character] + fallrate < maxfallspd
                                    ? fall_speed[old_character] + fallrate : maxfallspd;
                            } else {
                                bgload_objpos[old_character].y = bgload_objtarg +
                                    NuTrigTable[bob_angle] * bobscale;
                                bob_angle = (bob_angle + bobrate) & 0xffff;
                            }
                            bgload_animtime[old_character] += anim_step;
                            if (bgload_animtime[old_character] > bgloadanim[old_character]->time) {
                                bgload_animtime[old_character] -= bgloadanim[old_character]->time - 1.0f;
                            }
                            NuMtxSetTranslation(&m[old_character], &bgload_objpos[old_character]);
                            NuMtxPreRotateZ(&m[old_character], mg_player_tilt);
                            NuMtxTranslate(&m[old_character], &mg_player_pos);
                        }
                        break;
                    case 3:
                        NuMtxSetTranslation(&m[character], &bgload_objpos[character]);
                        NuMtxPreRotateZ(&m[character], mg_player_tilt);
                        NuMtxTranslate(&m[character], &mg_player_pos);
                        bgload_objpos[character].y -= fall_speed[character];
                        fall_speed[character] = fall_speed[character] + fallrate < maxfallspd
                            ? fall_speed[character] + fallrate : maxfallspd;
                        if (fadeval == 0xff) {
                            done = phase != 0;
                        }
                        break;
                    }
bgload_render_characters:
                    GameSfxLoop(0x8a, (struct nuvec_s *)&m[character]._30);
                    if (bgloadchr[character] != 0 && bgloadanim[character] != 0) {
                        NuHGobjEvalAnim(bgloadchr[character], (struct nuanimdata_s *)bgloadanim[character],
                            bgload_animtime[character], 0, 0, tmtx);
                        NuHGobjRndrMtxDwa(bgloadchr[character], &m[character], 2, layers, tmtx, 0);
                    }
                    if (old_character != -1 && bgloadchr[old_character] != 0 &&
                        bgloadanim[old_character] != 0) {
                        NuHGobjEvalAnim(bgloadchr[old_character], (struct nuanimdata_s *)bgloadanim[old_character],
                            bgload_animtime[old_character], 0, 0, tmtx);
                        NuHGobjRndrMtxDwa(bgloadchr[old_character], &m[old_character], 2, layers, tmtx, 0);
                    }
                    MiniGameRender();
                } else {
                    done = bgload_complete;
                }
                TimeTunnelRender();
                pNuCam->mtx = numtx_identity;
                NuCameraSet(pNuCam);
                if (font3d_scene != 0 && font3d_initialised != 0) {
                    MiniGameRenderScore();
                    if (mg_frame < 0) {
                        mg_frame = 0;
                    } else {
                        mg_frame++;
                    }
                    DrawGameMessage(tLOADING[Game.language], mg_frame, 0.8f);
                }
                gibber_scale = NuFabs(NuTrigTable[bgloadgibberscalecnt & 0xffff]);
                gibber_x = NuTrigTable[bgloadgibbercntx & 0xffff] * gibber_scale;
                gibber_y = NuTrigTable[bgloadgibbercnty & 0xffff] * gibber_scale;
                bgloadgibberscalecnt += bgloadgibberscalecntrate;
                bgloadgibbercntx += bgloadgibberratex;
                bgloadgibbercnty += bgloadgibberratey;
                if (font3d_scene != 0 && score_scale > 0.0f) {
                    score_scale *= 1.08f;
                }
            } else {
                done = bgload_complete;
            }

            if ((LDATA->flags & 0x40) != 0 && CrateMat == 0) {
                fadecol = 0x80ffffff;
            } else {
                previous_fade = fadeval;
                fadeval += fade_rate;
                if (fadeval > 0xff) {
                    fadeval = 0xff;
                } else if (fadeval < 0) {
                    fadeval = 0;
                }
                if (previous_fade < 0xff && fadeval == 0xff) {
                    fadehack = 1;
                } else if (fadehack != 0) {
                    fadehack--;
                }
                if (fadeval == 0 || fadeval == 0xff) {
                    fade_rate = 0;
                }
                fadecol = fadeval | (fadeval << 8) | (fadeval << 16) | 0x80000000;
            }
            if (fadeval != 0) {
                NuRndrRect2di(0, 0, SWIDTH << 4, SHEIGHT << 3, fadecol, fade_mtl);
            }
            UpdateTimer(&GameTimer);
            NuRndrLine3dDbgFlush();
            NuRndrEndScene();
        }
        BGSuspendMain();
        NuRndrVu1Draw();
        NuRndrSwapStreamBuffers();
        if (done != 0) {
            break;
        }
    }
    BGLoadEnd();
    if (NuRndrBeginScene(-1) != 0) {
        NuRndrEndScene();
        NuRndrSwapScreen(-1);
    }
    if (NuRndrBeginScene(-1) != 0) {
        NuRndrEndScene();
        NuRndrSwapScreen(-1);
    }
    TimeTunnelClose();
    fadeval = 0xff;
}

void BGLoadSfx(void)
{
    s32 done;

    fade_mtl = NuMtlCreate(1);
    fade_mtl->attrib.zmode = 3;
    fade_mtl->attrib.filter = 1;
    fade_mtl->attrib.lighting = 2;
    fade_mtl->attrib.utc = 1;
    fade_mtl->attrib.vtc = 1;
    fade_mtl->attrib.colour = 1;
    fade_mtl->attrib.alpha = 3;
    NuMtlUpdate(fade_mtl);

    if (NuRndrBeginScene(-1) != 0) {
        NuRndrEndScene();
        NuRndrSwapScreen(-1);
    }
    NuPs2FlushVRam(0, 0, 0);
    done = 0;
    font3d_scene = 0;
    PANEL3DMULX = (float)SWIDTH / (DIVPANEL3D * ((float)SWIDTH / 640.0f));
    PANEL3DMULY = (float)SHEIGHT / (DIVPANEL3D * ((float)SHEIGHT / 448.0f));
    BGLoadStart(BGLoadSfxFn);
    fade_rate = -10;
    fadeval = 0xff;
    mg_frame = -1;
    NuVpSetSize(PS2_REZ_W << 4, PS2_REZ_H << 4);

    do {
        if (bgload_complete != 0 && fadeval == 0xff) {
            done = 1;
        }
        pNuCam->mtx = numtx_identity;
        NuMtxTranslate(&pNuCam->mtx, (struct nuvec_s *)&bgload_campos);
        NuCameraSet(pNuCam);
        if (NuRndrBeginScene(-1) != 0) {
            NuRndrClear(3, 0, 1.0f);
            UpdateFade();
            if (font3d_scene != 0 && font3d_initialised != 0) {
                pNuCam->mtx = numtx_identity;
                NuCameraSet(pNuCam);
                if (mg_frame < 0) {
                    mg_frame = 0;
                } else {
                    mg_frame++;
                }
                DrawGameMessage(tLOADING[Game.language], mg_frame, 0.0f);
            }
            DrawFade();
            UpdateTimer(&GameTimer);
            NuRndrEndScene();
        }
        BGSuspendMain();
        NuRndrVu1Draw();
        NuRndrSwapStreamBuffers();
        if (bgload_complete == 1) {
            fade_rate = 10;
            bgload_complete++;
        }
    } while (done == 0);

    BGLoadEnd();
    if (NuRndrBeginScene(-1) != 0) {
        NuRndrEndScene();
        NuRndrSwapScreen(-1);
    }
    if (NuRndrBeginScene(-1) != 0) {
        NuRndrEndScene();
        NuRndrSwapScreen(-1);
    }
    TimeTunnelClose();
    fadeval = 0xff;
}
