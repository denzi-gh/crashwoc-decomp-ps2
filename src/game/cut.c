/*
 * Unit: game/cut
 *
 * Functions:
 *   0x0025b850 AppCutSceneCharacterRender
 *   0x0025ba88 LoadCutComponents
 *   0x0025bd98 LoadCutMovie
 *   0x0025c050 SetCutMovieRate
 *   0x0025c0e8 CloseCutMovie
 *   0x0025c210 StartCutMovie
 *   0x0025c368 PlayCutMovie
 *   0x0025c8c0 InitGameCut
 *   0x0025ca10 UpdateGameCut
 *   0x0025ce20 DrawGameCut
 *   0x0025d0d0 GetSpaceCut
 *   0x0025d4e0 SetCutSceneLights
 *   0x0025e598 InitCutScenes
 *   0x0025e5d0 UpdateCutMovie
 *   0x0025e650 DrawCutMovie
 *   0x0025e670 NewCut
 *   0x0025e698 LoadCutSceneCharacters
 *   0x0025e798 FindCutChar
 *   0x0025e808 AppCutSceneFindCharacters
 *   0x0025e948 locatorfn_fadeout
 *   0x0025ea08 CutSceneEndFunction
 *   0x0025ea60 CutSceneNextFunction
 *   0x0025eb48 CutSceneNextLogoFunction
 *   0x0025eb98 UpdateCutMovieCamera
 *   0x0025ecd0 NewGameCutAnim
 *   0x0025ed88 MakeDirLightColour
 *   0x0025edb8 MakeLightDirection
 */

#include "nu_libc.h"

#include "nu3d/nugobj.h"
#include "nu3d/nugscn.h"
#include "nu3d/nuhgobj.h"
#include "nu3d/numtl.h"
#include "nu3d/nutex.h"
#include "nu3d/nutexanm.h"
#include "nucore/nuerror.h"
#include "numath/numtx.h"
#include "numath/nutrig.h"
#include "numath/nuvec.h"
#include "nu3d/nuanim.h"
#include "game/gametypes.h"
#include "gamelib/gcutscn.h"
#include "game/creature.h"
#include "nucore/nucoretypes.h"
#include "nups2/ps2ctl.h"

extern struct game_s Game;
extern s32 gamecut;
extern struct nucolour3_s cutdircol[];
extern struct nuvec_s cutdir[];

extern s32 cutmovie;
extern s32 cut_on;
extern s32 logos_played;
extern struct cursor_s Cursor;
extern s32 D_00633388;
#define cutworldix D_00633388

struct csc_s {
    void *obj;            /* 0x0 */
    char *path;           /* 0x4 */
    char *name;           /* 0x8 */
};                        /* 0xC */

extern struct csc_s *CutChar;

struct csccache_s {
    char *path;          /* 0x0 */
    void *obj;           /* 0x4 */
};
extern struct csccache_s D_006FF880[];
extern s32 D_0063338C;
#define cutcharcache D_006FF880            /* loaded character models, by path */
#define ncutcharcache D_0063338C
extern union variptr_u superbuffer_ptr;

extern void NewMenu(void *cursor, s32 a, s32 b, s32 c);
extern void GameMusic(s32 sfx, s32 i);

extern s32 cutmovie_hold;
extern s32 Paused;
extern struct NUGCUTLOCFNDATA_s D_0060B398[];
#define cutscene_locatorfns D_0060B398

extern struct NUGCUTSCENE_s *D_006FF680[];
extern struct instNUGCUTSCENE_s *D_006FF700[];
extern s32 D_006FF780[];
#define CutScene D_006FF680
#define CutInst D_006FF700
#define CutAudio D_006FF780

extern u32 texanimbits;
extern f32 TITLESTARTTIME;
extern f32 TITLEFPS;

static void AppCutSceneFindCharacters(struct NUGCUTSCENE_s *cutscene);
extern s32 NuSoundKeyStatus(s32 chan);
extern void SetCutMovieRate(void);

extern f32 D_0062E888;
extern f32 D_0062E88C;
extern f32 D_00632BD8;

struct pcutanm_s {
    s16 character;    /* 0x0 */
    s16 action;       /* 0x2 */
    s16 sfx;          /* 0x4 */
    s8 i;             /* 0x6 */
    s8 pad1;          /* 0x7 */
};
extern struct pcutanm_s *pCutAnim;
extern s32 gamecut_hold;
extern s32 gamecut_sfx;
extern s32 gamesfx_channel;
extern s32 Level;
extern s32 NuSoundStopStream(s32 chan);
extern void GameSfx(s32 sfx, s32 a);

extern s32 fade_rate;
extern s32 fadeval;
extern void NuSoundUpdate(void);
extern void sceGsSyncV(s32 mode);
extern void *world_scene[];
extern void NuSoundFlushLoops(void);
extern s32 set_cutscenecammtx;
extern struct numtx_s cutscenecammtx;
extern struct numtx_s *pNuCam;
extern void NuCameraSet(void *cam);

extern struct nuvec_s cutpos_CRASH;
extern struct nuvec_s cutpos_FRONTEND[];
extern struct nuvec_s cutpos_SPACE[];
extern struct nuvec_s campos_SPACE;
extern u16 cutang_FRONTEND[2];
extern u16 cutang_SPACE[3];
extern void *SpaceGameCutTab[][2];
extern struct anim_s CutAnim;
extern struct anim_s CutVortexAnim;
extern s32 gamecut_finished;

extern s32 NuHGobjRndrMtxDwa(struct NUHGOBJ_s *hobj, struct numtx_s *m,
                             s32 nlayers, short *layer, struct numtx_s *tmtx,
                             float **dwa);

extern void NuHGobjEvalAnim2(struct NUHGOBJ_s *obj, void *joint_anim, f32 frame,
                             s32 njoints, void *joints, struct numtx_s *tmtx);
extern void NuHGobjEval(struct NUHGOBJ_s *obj, s32 njoints, void *joints,
                        struct numtx_s *tmtx);
extern float **NuHGobjEvalDwa2(s32 nlayers, short *layers, void *face_anim, f32 frame);
extern void NuMtxMulVU0(struct numtx_s *dst, struct numtx_s *a, struct numtx_s *b);

/* Cutscene character render callback: animate and draw one character, then
 * update the locators attached to it (on a joint, or to the character). */
void AppCutSceneCharacterRender(struct instNUGCUTSCENE_s *icutscene, struct NUGCUTSCENE_s *cutscene,
                                struct instNUGCUTCHAR_s *icutchar, struct NUGCUTCHAR_s *cutchar,
                                f32 current_frame) {
    struct numtx_s mtx;
    struct numtx_s tmtx[256];
    short layertab[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    struct numtx_s locatormtx;
    s32 is_visible;
    struct NUHGOBJ_s *obj;
    float **dwa;
    struct NUGCUTLOCATORSYS_s *locatorsys;
    struct instNUGCUTLOCATORSYS_s *ilocatorsys;
    struct NUGCUTLOCATOR_s *locator;
    s32 locix;
    s32 j;

    obj = icutchar->data;
    NuGCutCharAnimProcess(cutchar, current_frame, &mtx, &is_visible, 0, 0, 0);
    if (icutscene->flags & NUGCUTSCENE_MATRIXSET) {
        NuMtxMul(&mtx, &mtx, &icutscene->mtx);
    }
    if (is_visible == 0) {
        return;
    }

    if (cutchar->joint_anim != 0) {
        NuHGobjEvalAnim2(obj, cutchar->joint_anim, current_frame, 0, 0, tmtx);
    } else {
        NuHGobjEval(obj, 0, 0, tmtx);
    }
    if (cutchar->face_anim != 0) {
        dwa = NuHGobjEvalDwa2(obj->num_layers, layertab, cutchar->face_anim, current_frame);
    } else {
        dwa = 0;
    }
    NuHGobjRndrMtxDwa(obj, &mtx, obj->num_layers, layertab, tmtx, dwa);

    if (cutchar->first_locator == 0xFF) {
        return;
    }
    locatorsys = cutscene->locatordef;
    ilocatorsys = icutscene->locatorsys;
    for (j = 0; j < cutchar->nlocators; j++) {
        locix = cutchar->first_locator + j;
        locator = &locatorsys->locators[locix];
        if (locator->joint_ix == 0xFF) {
            instNuGCutLocatorUpdate(icutscene, locatorsys, &ilocatorsys->locators[locix],
                                    locator, current_frame, &mtx);
        } else {
            NuMtxMulVU0(&locatormtx, &tmtx[locator->joint_ix], &mtx);
            instNuGCutLocatorUpdate(icutscene, locatorsys, &ilocatorsys->locators[locix],
                                    &locatorsys->locators[locix], current_frame, &locatormtx);
        }
    }
}

struct cutscenedesc_s {
    char *gscname;        /* 0x0 scene (.gsc) */
    char *cutname;        /* 0x4 cutscene (.cut) */
    s32 sfxid;            /* 0x8 audio stream */
    s32 debgroup;         /* 0xC */
};                        /* 0x10 */

/* Cutscene tables (scenes/cuts, characters); unnamed in the symbol map, named
 * here after the files they load. */
extern struct cutscenedesc_s D_0060AFE0[]; /* levels\\b\\logos\\logos.gsc, ... */
extern struct csc_s D_0060AFB8[];        /* levels\\b\\logos\\ttchar.ghg, ... */
extern struct cutscenedesc_s D_0060B160[]; /* levels\\b\\intro\\space.gsc, ... */
extern struct csc_s D_0060B010[];        /* levels\\b\\intro\\cortex.ghg, ... */
extern struct cutscenedesc_s D_0060B280[]; /* levels\\b\\outro\\station.gsc, ... */
extern struct csc_s D_0060B220[];        /* levels\\b\\outro\\crunch.ghg, ... */
extern struct cutscenedesc_s D_0060B358[]; /* levels\\b\\outro\\station.gsc, outro2 */
extern struct csc_s D_0060B2A0[];        /* levels\\b\\outro\\Crunch.ghg, ... */
#define csd_logos D_0060AFE0
#define csc_logos D_0060AFB8
#define csd_intro D_0060B160
#define csc_intro D_0060B010
#define csd_outro D_0060B280
#define csc_outro D_0060B220
#define csd_outro2 D_0060B358
#define csc_outro2 D_0060B2A0

/* Retail-owned strings (no Data-from-C yet). */
extern char D_00632BD0[];                /* "ats.dat" */
extern char D_006293A0[];                /* "ats\\levels\\pal\\title.ats" */
extern char D_006293C0[];                /* "ats\\splash\\ripples.ats" */
extern char D_006293D8[];                /* "ats\\elecy\\elecy2.ats" */
#define ats_dat D_00632BD0
#define title_ats D_006293A0
#define ripples_ats D_006293C0
#define elecy2_ats D_006293D8

extern s32 D_00633390;
extern u8 texanimbuff[];
extern void LoadFont3D(void);
extern void *NuDatOpen(char *name, char **mem, s32 *used);
struct nudathdr_s;
extern void NuDatSet(struct nudathdr_s *ndh);
extern void NuDatClose(struct nudathdr_s *ndh);
struct nutexanimprog_s;
void CutSceneNextLogoFunction(struct instNUGCUTSCENE_s *icutscene);

struct scenecache_s {
    char *path;           /* 0x0 */
    void *scene;          /* 0x4 */
};
extern struct scenecache_s D_006FF980[];
extern s32 D_006FF800[];
#define cutscncache D_006FF980             /* loaded scenes, by path */
#define ncutscncache D_00633390
#define cutdebgroups D_006FF800
extern union variptr_u superbuffer_end;
extern void InitSpecular(void);
extern void InitEnviro(void);
void CutSceneEndFunction(struct instNUGCUTSCENE_s *icutscene);
void CutSceneNextFunction(struct instNUGCUTSCENE_s *icutscene);

/* Load each character model once, sharing already-loaded ones by path.
 * Inline: LoadCutComponents carries a copy of it in retail. */
inline void LoadCutSceneCharacters(struct csc_s *name) {
    s32 i;
    void *obj;
    void *tmp;
    char *path;

    while (name->path != 0) {
        path = name->path;
        for (i = 0; i < ncutcharcache; i++) {
            if (strcasecmp(path, cutcharcache[i].path) == 0) {
                obj = cutcharcache[i].obj;
                goto found;
            }
        }
        cutcharcache[i].path = path;
        tmp = NuGHGRead(&superbuffer_ptr, path);
        cutcharcache[i].obj = tmp;
        obj = tmp;
        ncutcharcache++;
    found:
        name->obj = obj;
        name++;
    }
}

/* Load the characters and every scene/cutscene of a movie, sharing models and
 * scenes already loaded, and chain the cutscenes so each plays the next.
 * Returns the number of cutscenes. */
/* Near-match (state=asm): control flow, frame size (0xB0) and the inlined
 * character cache match retail; the remaining diff is how gcc keeps the two
 * counters (retail: one in $fp plus `ix + 1` spilled at 0($sp)). Tried: for
 * with/without init, while, a single counter, ret/ix declaration order. */
s32 LoadCutComponents(struct cutscenedesc_s *csd, struct csc_s *chars) {
    s32 ret = 0;
    s32 ix = 0;
    s32 i;
    char *path;
    void *scene;
    char *end;

    InitSpecular();
    InitEnviro();
    CutChar = chars;
    LoadCutSceneCharacters(chars);

    for (; csd[ix].gscname != 0; ix++, ret++) {
        cutdebgroups[ix] = csd[ix].debgroup;

        path = csd[ix].gscname;
        end = superbuffer_end.charptr;
        for (i = 0; i < ncutscncache; i++) {
            if (strcasecmp(path, cutscncache[i].path) == 0) {
                scene = cutscncache[i].scene;
                goto found;
            }
        }
        cutscncache[i].path = path;
        scene = NuGScnRead(&superbuffer_ptr.charptr, end, path);
        cutscncache[i].scene = scene;
        ncutscncache++;
    found:
        world_scene[ix] = scene;

        CutScene[ix] = NuGCutSceneLoad(csd[ix].cutname, &superbuffer_ptr, &superbuffer_end);
        if (CutScene[ix] != 0) {
            NuGCutSceneFixUp(CutScene[ix], world_scene[ix], 0);
            /* The instance is named after the last 16 characters of the path. */
            path = csd[ix].cutname;
            if (strlen(path) != 0x10) {
                path -= 0x10;
                path += strlen(csd[ix].cutname);
            }
            CutInst[ix] = instNuGCutSceneCreate(CutScene[ix], 0, 0, path, &superbuffer_ptr);
            if (CutInst[ix] != 0) {
                CutInst[ix]->speed = 0.6f;
                CutAudio[ix] = csd[ix].sfxid;
                instNuGCutSceneSetEndCallback(CutInst[ix], CutSceneEndFunction);
                if (ret != 0 && CutInst[ix - 1] != 0) {
                    CutInst[ix - 1]->chain = CutInst[ix];
                    instNuGCutSceneSetEndCallback(CutInst[ix - 1], CutSceneNextFunction);
                }
            }
        }
    }
    return ret;
}

/* Reset the cutscene state and load movie `movie`: 0 = logos/title,
 * 1 = intro, 3 = outro, 4 = outro2. Returns whether a cutscene was loaded. */
/* Near-match (state=asm), 652/696 bytes: same instructions; retail allocates
 * $v0 where this build picks $v1 for the switch's `1` and for &texanimbuff in
 * every case. No `case 2` (adding one emits a jump table retail lacks). */
s32 LoadCutMovie(s32 movie) {
    union variptr_u buf;
    void *dat;
    s32 count;
    s32 i;

    LoadFont3D();
    ncutscncache = 0;
    for (i = 0x1F; i >= 0; i--) {
        world_scene[i] = 0;
    }
    ncutcharcache = 0;
    for (i = 0; i < 0x20; i++) {
        CutScene[i] = 0;
        CutInst[i] = 0;
        CutAudio[i] = -1;
    }
    CutChar = 0;
    cutworldix = 0;
    NuTexAnimProgSysInit();

    switch (movie) {
    case 0:
        dat = NuDatOpen(ats_dat, 0, 0);
        NuDatSet(dat);
        buf.voidptr = texanimbuff;
        NuTexAnimProgReadScript(&buf, title_ats);
        NuDatSet(0);
        if (dat != 0) {
            NuDatClose(dat);
        }
        count = LoadCutComponents(csd_logos, csc_logos);
        instNuGCutSceneSetEndCallback(CutInst[0], CutSceneNextLogoFunction);
        CutInst[count - 1]->flags |= NUGCUTSCENE_LOOP;
        instNuGCutSceneSetEndCallback(CutInst[count - 1], 0);
        break;
    case 1:
        dat = NuDatOpen(ats_dat, 0, 0);
        NuDatSet(dat);
        buf.voidptr = texanimbuff;
        NuTexAnimProgReadScript(&buf, ripples_ats);
        NuDatSet(0);
        if (dat != 0) {
            NuDatClose(dat);
        }
        LoadCutComponents(csd_intro, csc_intro);
        break;
    case 3:
    case 4:
        dat = NuDatOpen(ats_dat, 0, 0);
        NuDatSet(dat);
        buf.voidptr = texanimbuff;
        NuTexAnimProgReadScript(&buf, elecy2_ats);
        if (movie == 4) {
            NuTexAnimProgReadScript(&buf, ripples_ats);
        }
        NuDatSet(0);
        if (dat != 0) {
            NuDatClose(dat);
        }
        if (movie == 3) {
            LoadCutComponents(csd_outro, csc_outro);
        } else {
            LoadCutComponents(csd_outro2, csc_outro2);
        }
        break;
    }
    return CutInst[0] != 0;
}

void SetCutMovieRate(void) {
    CutInst[cutworldix]->speed = 0.5f;
    if (cutmovie == 3 || cutmovie == 4) {
        if (cutworldix == 0) {
            CutInst[cutworldix]->speed *= D_0062E888;
        }
    }
    CutInst[cutworldix]->speed *= D_0062E88C;
    if (D_00632BD8 != 0.0f) {
        CutInst[cutworldix]->speed = D_00632BD8;
    }
}


void CloseCutMovie(s32 all) {
    s32 i;

    for (i = 0; i < 0x20; i++) {
        if (CutInst[i] != 0) {
            instNuGCutSceneDestroy(CutInst[i]);
            CutInst[i] = 0;
        }
    }
    if (ncutcharcache != 0) {
        for (i = 0; i < ncutcharcache; i++) {
            NuHGobjDestroy(cutcharcache[i].obj);
        }
    }
    ncutcharcache = 0;
    for (i = 0; i < 0x20; i++) {
        world_scene[i] = 0;
    }
    NuSoundUpdate();
    NuSoundStopStream(0);
    NuSoundStopStream(1);
    NuSoundStopStream(2);
    NuSoundStopStream(3);
    NuSoundStopStream(4);
    NuSoundUpdate();
    NuSoundFlushLoops();
    if (all != 0) {
        NuTexInit();
        NuMtlRelease();
        NuGobjInit();
    }
}

/* Start the loaded cutscene. On the title screen (logos already played) it
 * loops from TITLESTARTTIME seconds before the end with title music; otherwise
 * its audio stream is started, offset by the selected language for movies. */
void StartCutMovie(void) {
    s32 sfx;

    cutmovie_hold = 0;
    if (CutScene[0] != 0 && CutInst[0] != 0) {
        instNuGCutSceneStart(CutInst[0]);
        if (cutmovie == 0 && logos_played != 0) {
            GameMusic(0xaa, 0);
            sfx = -1;
            CutInst[cutworldix]->time =
                (CutScene[cutworldix]->duration - TITLESTARTTIME * TITLEFPS) + 0.0f;
            texanimbits = (texanimbits & ~2) | 4;
        } else {
            sfx = CutAudio[cutworldix];
            texanimbits = (texanimbits | 2) & ~4;
        }
        if (sfx != -1 && (cutmovie != 0 || logos_played == 0)) {
            NuSoundStopStream(4);
            if (cutmovie != 0) {
                sfx += Game.language * 4;
            }
            gamesfx_channel = 4;
            GameSfx(sfx, 0);
            cutmovie_hold = 1;
        }
    }
}

void InitGameCut(void) {
    cutang_FRONTEND[0] = (u16)NuAtan2D(cutpos_CRASH.x - cutpos_FRONTEND[0].x,
                                       cutpos_CRASH.z - cutpos_FRONTEND[0].z);
    cutang_FRONTEND[1] = (u16)NuAtan2D(cutpos_CRASH.x - cutpos_FRONTEND[1].x,
                                       cutpos_CRASH.z - cutpos_FRONTEND[1].z);
    cutang_SPACE[0] = (u16)NuAtan2D(campos_SPACE.x - cutpos_SPACE[0].x,
                                    campos_SPACE.z - cutpos_SPACE[0].z);
    cutang_SPACE[1] = (u16)NuAtan2D(campos_SPACE.x - cutpos_SPACE[1].x,
                                    campos_SPACE.z - cutpos_SPACE[1].z);
    cutang_SPACE[2] = (u16)NuAtan2D(campos_SPACE.x - cutpos_SPACE[2].x,
                                    campos_SPACE.z - cutpos_SPACE[2].z);
    pCutAnim = (struct pcutanm_s *)SpaceGameCutTab[gamecut][1];
    ResetAnimPacket(&CutAnim, pCutAnim->action);
    gamecut_hold = 1;
    gamecut_sfx = -1;
    gamecut_finished = 0;
    ResetAnimPacket(&CutVortexAnim, 0x22);
}

extern s32 GameMode;
extern f32 HUBHOLOSCALE;
extern f32 SPACEMASKADJUSTY;
extern f32 FRONTENDCORTEXADJUSTY;

extern struct gtimer_s CutTimer;
extern struct nupad_s *Pad[];
extern struct nuvec_s v000;
extern s32 gamecut_start;
extern s32 gamecut_newlevel;
extern s32 new_level;
extern s32 new_mode;
extern s32 gamecut_hack;
extern s32 last_hub;
extern f32 tumble_time;
extern f32 tumble_duration;
extern void UpdateTimer(struct gtimer_s *timer);
extern void GameSfxLoop(s32 sfx, struct nuvec_s *pos);

/* Start the current cutscene animation step: its voice line (localised on the
 * hub, Level 0x25) and the sound for actions 0xC/0xD. Inline: UpdateGameCut
 * carries two copies of it in retail. */
inline void NewGameCutAnim(void) {
    s32 sfx;

    gamecut_hold = 1;
    gamecut_sfx = pCutAnim->sfx;
    if (gamecut_sfx != -1) {
        NuSoundStopStream(4);
        gamesfx_channel = 4;
        if (Level == 0x25) {
            GameSfx(gamecut_sfx + Game.language * 12, 0);
        } else {
            GameSfx(gamecut_sfx, 0);
        }
    }
    if (pCutAnim->action == 0xc) {
        sfx = 0x1e;
    } else if (pCutAnim->action == 0xd) {
        sfx = 0x21;
    } else {
        sfx = -1;
    }
    if (sfx != -1) {
        GameSfx(sfx, 0);
    }
}

/* Advance the hub/space cutscene: start it when entering the level, step
 * through pCutAnim (skippable with the pad), then return to the hub or go on
 * to the level the cutscene was routing to. */
/* Near-match (state=asm), 1020/1040 bytes: only register choice differs (the
 * previous character after pCutAnim++, and CRemap[183]; retail uses $a1).
 * Tried: GC-style `i` for CRemap[183], pCutAnim[-1].character, s16/decl order. */
void UpdateGameCut(void) {
    struct CharacterModel *model;
    f32 dt;
    s32 character;

    if (GameMode != 1 && gamecut_start != 0 && (Level == 0x25 || Level == 0x28)) {
        new_mode = 1;
        gamecut_hold = 1;
        gamecut_newlevel = -1;
        gamecut_start = 0;
        gamecut_finished = 0;
        gamecut = 0;
    } else if (GameMode == 1) {
        if (CutTimer.frame == 0) {
            InitGameCut();
            NewGameCutAnim();
        }
        if (gamecut_finished == 0) {
            model = 0;
            if (pCutAnim->character >= 0 && pCutAnim->character < 0xBF &&
                CRemap[pCutAnim->character] != -1) {
                model = &CModel[CRemap[pCutAnim->character]];
            }
            CutAnim.oldaction = CutAnim.action;
            CutAnim.newaction = pCutAnim->action;
            if (model != 0) {
                if (gamecut_hold != 0 && gamecut_sfx != -1 && NuSoundKeyStatus(4) != 1) {
                    dt = 0.0f;
                } else {
                    dt = 0.59999996f;
                    gamecut_hold = 0;
                }
                UpdateAnimPacket(model, &CutAnim, dt, 0.0f);
            }
            UpdateTimer(&CutTimer);
            if (fadeval == 0 && Pad[0] != 0 && (Pad[0]->paddata_db & 0x800)) {
                if (Level == 0x28) {
                    new_level = gamecut_newlevel;
                } else {
                    new_mode = 0;
                }
            }
            if (model == 0 || (CutAnim.flags & 1)) {
                character = pCutAnim->character;
                pCutAnim++;
                if (pCutAnim->character != -1) {
                    if (pCutAnim->character != character) {
                        ResetAnimPacket(&CutAnim, pCutAnim->action);
                    }
                    NewGameCutAnim();
                } else {
                    gamecut_finished = 1;
                }
            }
        }
        if (gamecut_finished != 0) {
            if (Level == 0x28) {
                if (gamecut_newlevel != -1) {
                    new_level = gamecut_newlevel;
                } else {
                    new_level = 0x25;
                }
            } else {
                new_mode = 0;
            }
        }
        if (Level == 0x28) {
            if (CRemap[183] != -1) {
                model = &CModel[CRemap[183]];
                CutVortexAnim.oldaction = CutVortexAnim.action;
                CutVortexAnim.newaction = 0x22;
                UpdateAnimPacket(model, &CutVortexAnim, 0.59999996f, 0.0f);
            }
            GameSfxLoop(0xE1, &v000);
        }
    }
    if (gamecut == 5) {
        gamecut_hack = 1;
    }
    if (new_mode == 0) {
        last_hub = -1;
        tumble_time = tumble_duration = 0.0f;
    }
}

/* Draw the character of the current hub/space cutscene, plus the vortex in
 * space (Level 0x28). Unlike the GameCube build, the bone matrices live in a
 * stack array here rather than in a global. */
/* Near-match (state=asm): instruction-for-instruction except register choice
 * inside the Level 0x28 / frontend branches (retail reloads pCutAnim per arm,
 * into $a1/$a0; here gcc loads it once). Tried: statement order in both arms,
 * direct pCutAnim->i indexing, single-expression yrot, local decl order. */
void DrawGameCut(void) {
    struct numtx_s m;
    struct numtx_s tmtx[256];
    struct nuvec_s scale;
    struct nuvec_s pos;
    float **dwa;
    struct CharacterModel *model;
    u16 yrot;
    s32 i;

    if (Level == 0x28 && CRemap[183] != -1) {
        model = &CModel[CRemap[183]];
        NuMtxSetTranslation(&m, &campos_SPACE);
        EvalModelAnim(model, &CutVortexAnim, &m, tmtx, &dwa, 0);
        NuHGobjRndrMtxDwa(model->obj, &m, 1, 0, tmtx, dwa);
    }
    if (GameMode != 1 || gamecut_finished != 0) {
        return;
    }
    if (CRemap[pCutAnim->character] != -1) {
        model = &CModel[CRemap[pCutAnim->character]];
    } else {
        model = 0;
    }
    if (model == 0) {
        return;
    }

    scale.x = (model->character == 0xB3 || model->character == 0xB4) ? 0.77f : 1.0f;
    if (Level == 0x25) {
        scale.x *= HUBHOLOSCALE;
    }
    scale.y = scale.z = scale.x;
    NuMtxSetScale(&m, &scale);

    if (Level == 0x28) {
        i = pCutAnim->i;
        pos = cutpos_SPACE[i];
        yrot = cutang_SPACE[i];
        yrot -= 0x8000;
        if (model->character == 0x55 || model->character == 0x57 ||
            model->character == 0x56 || model->character == 0x58 ||
            model->character == 2) {
            pos.y += SPACEMASKADJUSTY * scale.y;
        }
    } else {
        i = pCutAnim->i;
        yrot = cutang_FRONTEND[i];
        yrot -= 0x8000;
        pos = cutpos_FRONTEND[i];
        if (model->character == 0xB5) {
            pos.y += FRONTENDCORTEXADJUSTY * scale.y;
        }
    }
    NuMtxRotateY(&m, yrot);
    NuMtxTranslate(&m, &pos);
    EvalModelAnim(model, &CutAnim, &m, tmtx, &dwa, 0);
    NuHGobjRndrMtxDwa(model->obj, &m, 1, 0, tmtx, dwa);
}

extern s32 Demo;
extern u16 new_lev_flags;
extern s32 HubFromLevel(s32 level);

/* Queue cutscene `i` and mark it as seen. Inline: GetSpaceCut carries copies
 * of it in retail. */
inline void NewCut(s32 i) {
    gamecut = i;
    Game.cutbits |= 1 << i;
}

/* Choose the hub cutscene to play when going from level `last` to `next`:
 * a hub's intro, its 2- and 4-crystal scenes, then its boss door. If one is
 * picked, route through the space level (0x28) first. */
void GetSpaceCut(s32 last, s32 next) {
    s32 hub;

    gamecut = -1;
    if (Demo != 0) {
        return;
    }
    if (next == 0x25) {
        if (last == 0x15 && (new_lev_flags & 0x800) && !(Game.cutbits & 0x20)) {
            NewCut(5);
        }
    } else {
        hub = HubFromLevel(next);
        switch (hub) {
        case 0:
            if (!(Game.cutbits & 0x2)) {
                NewCut(1);
            } else if (Game.hub[0].crystals == 2 && !(Game.cutbits & 0x4)) {
                NewCut(2);
            } else if (Game.hub[0].crystals == 4 && !(Game.cutbits & 0x8)) {
                NewCut(3);
            } else if (next == 0x15 && !(Game.cutbits & 0x10)) {
                NewCut(4);
            }
            break;
        case 1:
            if (!(Game.cutbits & 0x80)) {
                NewCut(7);
            } else if (Game.hub[1].crystals == 2 && !(Game.cutbits & 0x100)) {
                NewCut(8);
            } else if (Game.hub[1].crystals == 4 && !(Game.cutbits & 0x200)) {
                NewCut(9);
            } else if (next == 0x17 && !(Game.cutbits & 0x400)) {
                NewCut(0xA);
            }
            break;
        case 2:
            if (!(Game.cutbits & 0x1000)) {
                NewCut(0xC);
            } else if (Game.hub[2].crystals == 2 && !(Game.cutbits & 0x2000)) {
                NewCut(0xD);
            } else if (Game.hub[2].crystals == 4 && !(Game.cutbits & 0x4000)) {
                NewCut(0xE);
            } else if (next == 0x16 && !(Game.cutbits & 0x8000)) {
                NewCut(0xF);
            }
            break;
        case 3:
            if (!(Game.cutbits & 0x20000)) {
                NewCut(0x11);
            } else if (Game.hub[3].crystals == 2 && !(Game.cutbits & 0x40000)) {
                NewCut(0x12);
            } else if (Game.hub[3].crystals == 4 && !(Game.cutbits & 0x80000)) {
                NewCut(0x13);
            } else if (next == 0x18 && !(Game.cutbits & 0x100000)) {
                NewCut(0x14);
            }
            break;
        case 4:
            if (!(Game.cutbits & 0x400000)) {
                NewCut(0x16);
            } else if (Game.hub[4].crystals == 2 && !(Game.cutbits & 0x800000)) {
                NewCut(0x17);
            } else if (Game.hub[4].crystals == 4 && !(Game.cutbits & 0x1000000)) {
                NewCut(0x18);
            } else if (next == 0x19 && !(Game.cutbits & 0x2000000)) {
                NewCut(0x19);
            }
            break;
        }
    }
    if (gamecut != -1) {
        gamecut_newlevel = next;
        new_level = 0x28;
    }
}

void InitCutScenes(void) {
    NuGCutSceneSysInit(cutscene_locatorfns);
    NuSetCutSceneFindCharactersFn(AppCutSceneFindCharacters);
    NuSetCutSceneCharacterRenderFn(AppCutSceneCharacterRender);
}

void UpdateCutMovie(void) {
    if (cutmovie_hold != 0 && CutAudio[cutworldix] != -1 &&
        NuSoundKeyStatus(4) != 1) {
        CutInst[cutworldix]->speed = 0.0f;
    } else {
        cutmovie_hold = 0;
        SetCutMovieRate();
    }
    NuGCutSceneSysUpdate(Paused);
}


void locatorfn_fadeout(struct instNUGCUTSCENE_s *icutscene, struct NUGCUTLOCATORSYS_s *locatorsys,
                       struct instNUGCUTLOCATOR_s *ilocator, struct NUGCUTLOCATOR_s *locator, f32 currf,
                       struct numtx_s *wm) {
    struct nuanimtime_s atime;
    f32 remaining;

    if (locator->anim != 0) {
        NuAnimData2CalcTime(locator->anim, currf, &atime);
        if (NuGCutLocatorIsVisble(locator, currf, &atime, 0) != 0 &&
            ilocator->flags == 0) {
            ilocator->flags = 1;
            remaining = 255.0f / (icutscene->def->duration - icutscene->time);
            fade_rate = remaining < 1.0f ? 1 : (s32)remaining;
        }
    }
}

void CutSceneEndFunction(struct instNUGCUTSCENE_s *icutscene) {
    if (cutmovie == 0) {
        NewMenu(&Cursor, 0, 0, -1);
        if (logos_played == 0) {
            GameMusic(0xaa, 0);
        }
    } else {
        cut_on = 0;
    }
}

void CutSceneNextFunction(struct instNUGCUTSCENE_s *icutscene) {
    s32 sfx;

    cutworldix++;
    if (CutAudio[cutworldix] != -1) {
        NuSoundStopStream(4);
        do {
            NuSoundUpdate();
            if (NuSoundKeyStatus(4) == 1) {
                sceGsSyncV(0);
            }
        } while (NuSoundKeyStatus(4) == 1);
        sfx = CutAudio[cutworldix];
        if (cutmovie != 0) {
            sfx += Game.language * 4;
        }
        gamesfx_channel = 4;
        GameSfx(sfx, 0);
        cutmovie_hold = 1;
    }
    if (fadeval != 0) {
        fade_rate = -10;
    }
}

void CutSceneNextLogoFunction(struct instNUGCUTSCENE_s *icutscene) {
    cutworldix++;
    NewMenu(&Cursor, 0, 0, -1);
    if (logos_played == 0) {
        GameMusic(0xaa, 0);
    }
}


inline void *FindCutChar(char *name) {
    struct csc_s *chr;

    chr = CutChar;
    if (CutChar != 0) {
        for (; chr->path != 0; chr++) {
            if (strcasecmp(name, chr->name) == 0) {
                return chr->obj;
            }
        }
    }
    return 0;
}

extern char D_00632BC0[];                /* ".\\cut.c" */
extern char D_00629350[];                /* "AppCutSceneFindCharacters: cannot fixup ..." */
#define cut_srcfile D_00632BC0
#define nocutchar_msg D_00629350

static void AppCutSceneFindCharacters(struct NUGCUTSCENE_s *cutscene) {
    struct NUGCUTCHARSYS_s *charSys;
    struct NUGCUTCHAR_s *cutchar;
    struct NUGCUTLOCATORSYS_s *locator;
    s32 i;

    charSys = cutscene->chardef;
    locator = cutscene->locatordef;
    for (i = 0; i < charSys->count; i++) {
        cutchar = &charSys->chars[i];
        cutchar->character = FindCutChar(cutchar->name);
        if (cutchar->character == 0) {
            NuDebugMsgProlog(cut_srcfile, 0x2a1)(nocutchar_msg, cutchar->name);
        }
        if (cutchar->nlocators != 0 && (s32)cutchar->locators < 0xff) {
            cutchar->first_locator = (u8)(s32)cutchar->locators;
            cutchar->locators = &locator->locators[(s32)cutchar->locators];
        } else {
            cutchar->first_locator = -1;
        }
    }
}


void DrawCutMovie(void) {
    NuGCutSceneSysRender();
}


void UpdateCutMovieCamera(struct numtx_s *cam) {
    if (set_cutscenecammtx != 0) {
        *cam = cutscenecammtx;
        set_cutscenecammtx = 0;
        *pNuCam = *cam;
        NuCameraSet(pNuCam);
    }
}


void MakeDirLightColour(s32 ix, f32 r, f32 g, f32 b) {
    cutdircol[ix].r = r;
    cutdircol[ix].g = g;
    cutdircol[ix].b = b;
}

void MakeLightDirection(s32 ix, f32 x, f32 y, f32 z) {
    cutdir[ix].x = x;
    cutdir[ix].y = y;
    cutdir[ix].z = z;
    NuVecNorm(&cutdir[ix], &cutdir[ix]);
}
