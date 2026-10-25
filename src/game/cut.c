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
#include "nu3d/nucamera.h"
#include "game/gametypes.h"
#include "gamelib/gcutscn.h"
#include "game/creature.h"
#include "nucore/nucoretypes.h"
#include "nups2/ps2ctl.h"
#include "game/cut.h"
#include "nucore/nufile.h"

extern struct game_s Game;

extern s32 cut_on;
extern struct cursor_s Cursor;

static s32 cutworldix;
static s32 ghg_inst_count;
static s32 scene_inst_count;
static struct NUGCUTSCENE_s *CutScene[32];
static struct instNUGCUTSCENE_s *CutInst[32];
static s32 CutAudio[32];
static s32 cutdebgroups[32];
static struct csccache_s ghg_insts[32];     /* loaded character models, by path */
static struct scenecache_s scene_insts[32]; /* loaded scenes, by path */

extern union variptr_u superbuffer_ptr;

extern void NewMenu(void *cursor, s32 a, s32 b, s32 c);
extern void GameMusic(s32 sfx, s32 i);

extern s32 Paused;

extern u32 texanimbits;

extern s32 NuSoundKeyStatus(s32 chan);
extern void SetCutMovieRate(void);

struct pcutanm_s {
    s16 character;    /* 0x0 */
    s16 action;       /* 0x2 */
    s16 sfx;          /* 0x4 */
    s8 i;             /* 0x6 */
    s8 pad1;          /* 0x7 */
};

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

/* Defined after GetSpaceCut, where retail emits them. */
extern void *SpaceGameCutTab[][2];
extern AnimPacket CutAnim;
extern struct gtimer_s CutTimer;
extern AnimPacket CutVortexAnim;
extern s32 next_cut_movie;
extern u16 cutang_FRONTEND[2];
extern u16 cutang_SPACE[3];
extern s32 gamecut_hold;

extern s32 NuHGobjRndrMtxDwa(struct NUHGOBJ_s *hobj, struct numtx_s *m,
                             s32 nlayers, short *layer, struct numtx_s *tmtx,
                             float **dwa);

extern void NuHGobjEvalAnim2(struct NUHGOBJ_s *obj, void *joint_anim, f32 frame,
                             s32 njoints, void *joints, struct numtx_s *tmtx);
extern void NuHGobjEval(struct NUHGOBJ_s *obj, s32 njoints, void *joints,
                        struct numtx_s *tmtx);
extern float **NuHGobjEvalDwa2(s32 nlayers, short *layers, void *face_anim, f32 frame);
extern void NuMtxMulVU0(struct numtx_s *dst, struct numtx_s *a, struct numtx_s *b);

struct nuvec_s cutdir[3] = {0};
struct nucolour3_s cutdircol[3] = {0};
struct nuvec_s cutambcol = {0};
s32 logos_played = 0;
f32 TITLESTARTTIME = 6.0f;
f32 TITLEFPS = 30.0f;
s32 cutmovie_hold = 0;

/* The characters (models) and the scenes/cuts of each movie. */
static struct csc_s csc_titles[3] = {
    { 0, "levels\\b\\logos\\ttchar.ghg", "ttchar" },
    { 0, "levels\\b\\logos\\title.ghg", "title" },
    {0}
};

static struct cutscenedesc_s csd_titles[3] = {
    { "levels\\b\\logos\\logos.gsc", "levels\\b\\logos\\logos1.cut", 172, -1 },
    { "levels\\b\\logos\\logos.gsc", "levels\\b\\logos\\logos2.cut", -1, -1 },
    { 0, 0, -1, -1 }
};

static struct csc_s csc_intro1and2[28] = {
    { 0, "levels\\b\\intro\\cortex.ghg", "CS_Cortex_group" },
    { 0, "levels\\b\\intro\\csdingo.ghg", "CS_Dingo_group" },
    { 0, "levels\\b\\intro\\csngin.ghg", "CS_NGin_group" },
    { 0, "levels\\b\\intro\\csntro.ghg", "CS_Ntro_group" },
    { 0, "levels\\b\\intro\\cstiny.ghg", "CS_Tiny_group" },
    { 0, "levels\\b\\intro\\csuka.ghg", "CS_Uka_group" },
    { 0, "levels\\b\\intro\\graph.ghg", "CS_Graph_group" },
    { 0, "levels\\b\\intro\\hcrash.ghg", "CS_HoloCrash_group" },
    { 0, "levels\\b\\intro2\\cscrash.ghg", "cscrash_group" },
    { 0, "levels\\b\\intro2\\cscoco.ghg", "cscoco_group" },
    { 0, "levels\\b\\intro2\\board.ghg", "cssurfboard_group" },
    { 0, "levels\\b\\intro2\\csscoota.ghg", "csscoota_group" },
    { 0, "levels\\b\\intro2\\purra.ghg", "purra_group" },
    { 0, "levels\\b\\intro\\csaku.ghg", "csaku_group" },
    { 0, "levels\\b\\intro2\\tsunami.ghg", "tsunami_group" },
    { 0, "levels\\b\\intro\\firemask.ghg", "firemask_group" },
    { 0, "levels\\b\\intro\\windmask.ghg", "wind_group" },
    { 0, "levels\\b\\intro\\watermsk.ghg", "water_group" },
    { 0, "levels\\b\\intro\\earthmsk.ghg", "earth_group" },
    { 0, "levels\\b\\intro\\csuka.ghg", "csuka_group" },
    { 0, "levels\\b\\intro2\\bigpurra.ghg", "bigpurra_group" },
    { 0, "levels\\b\\intro2\\towel.ghg", "towel_group" },
    { 0, "levels\\b\\intro2\\door.ghg", "door_group" },
    { 0, "levels\\b\\intro2\\laptop.ghg", "laptop_group" },
    { 0, "levels\\b\\intro2\\contdoor.ghg", "contdoor_group" },
    { 0, "levels\\b\\intro2\\vchair.ghg", "vchair_group" },
    { 0, "levels\\b\\intro2\\volcano.ghg", "volcano_group" },
    {0}
};

static struct cutscenedesc_s csd_intro1and2[12] = {
    { "levels\\b\\intro\\space.gsc", "levels\\b\\intro2\\station.cut", 173, 1 },
    { "levels\\b\\intro\\corridor.gsc", "levels\\b\\intro2\\corridor.cut", -1, 2 },
    { "levels\\b\\intro\\chamber.gsc", "levels\\b\\intro\\inchambr.cut", -1, 3 },
    { "levels\\b\\intro\\volcano.gsc", "levels\\b\\intro2\\surf.cut", 174, 4 },
    { "levels\\b\\intro\\volcano.gsc", "levels\\b\\intro2\\sunny.cut", -1, 5 },
    { "levels\\b\\intro\\volcano.gsc", "levels\\b\\intro2\\storm.cut", -1, 6 },
    { "levels\\b\\intro2\\blackbox.gsc", "levels\\b\\intro2\\black.cut", -1, 7 },
    { "levels\\b\\intro\\volcano.gsc", "levels\\b\\intro2\\washedup.cut", -1, 8 },
    { "levels\\b\\intro\\elemen.gsc", "levels\\b\\intro2\\mystic.cut", -1, 9 },
    { "levels\\b\\intro\\house.gsc", "levels\\b\\intro2\\house.cut", -1, 10 },
    { "levels\\b\\intro\\control.gsc", "levels\\b\\intro2\\control.cut", -1, 11 },
    { 0, 0, -1, -1 }
};

static struct csc_s csc_outro[8] = {
    { 0, "levels\\b\\outro\\crunch.ghg", "CS_Crunch_Group" },
    { 0, "levels\\b\\outro\\Crash.ghg", "CS_Crash_Group" },
    { 0, "levels\\b\\outro\\Cortex.ghg", "CS_Cortex_Group" },
    { 0, "levels\\b\\outro\\UkaUka.ghg", "CS_Uka_Group" },
    { 0, "levels\\b\\outro\\AkuAku.ghg", "CS_Aku_Group" },
    { 0, "levels\\b\\outro\\Crash.ghg", "CS_Crash_Group1" },
    { 0, "levels\\b\\outro\\Crunch.ghg", "CS_Crunch2_Group" },
    {0}
};

static struct cutscenedesc_s csd_outro[2] = {
    { "levels\\b\\outro\\station.gsc", "levels\\b\\outro\\outrosml.cut", 175, 12 },
    { 0, 0, -1, -1 }
};

static struct csc_s csc_outro2[15] = {
    { 0, "levels\\b\\outro\\Crunch.ghg", "CS_Crunch_Group" },
    { 0, "levels\\b\\outro\\Crash.ghg", "CS_Crash_Group" },
    { 0, "levels\\b\\outro\\Cortex.ghg", "CS_Cortex_Group" },
    { 0, "levels\\b\\outro\\UkaUka.ghg", "CS_Uka_Group" },
    { 0, "levels\\b\\outro\\AkuAku.ghg", "CS_Aku_Group" },
    { 0, "levels\\b\\outro\\Crunch.ghg", "CS_Crunch2_Group" },
    { 0, "levels\\b\\outro\\Crash.ghg", "CS_Crash_Group1" },
    { 0, "levels\\b\\outro2\\Crash.ghg", "CSCrash_group" },
    { 0, "levels\\b\\outro2\\Purra.ghg", "BigPurra_group" },
    { 0, "levels\\b\\outro2\\AkuAku.ghg", "CS_AkuAku_Group" },
    { 0, "levels\\b\\outro2\\Coco.ghg", "CS_Coco_Group" },
    { 0, "levels\\b\\outro2\\Crunch.ghg", "CS_Crunch3_Group" },
    { 0, "levels\\b\\outro2\\Cortex.ghg", "CS_Cortex_Group" },
    { 0, "levels\\b\\outro2\\UkaUka.ghg", "CS_UkaUka_Group" },
    {0}
};

static struct cutscenedesc_s csd_outro2[4] = {
    { "levels\\b\\outro\\station.gsc", "levels\\b\\outro\\outro.cut", 176, 13 },
    { "levels\\b\\outro2\\house.gsc", "levels\\b\\outro2\\outhouse.cut", -1, 14 },
    { "levels\\b\\outro2\\iceberg.gsc", "levels\\b\\outro2\\iceberg.cut", -1, 15 },
    { 0, 0, -1, -1 }
};

struct csc_s *CutChar = 0;

/* Load each character model once, sharing already-loaded ones by path.
 * Inline: LoadCutComponents carries a copy of it in retail. */
inline void LoadCutSceneCharacters(struct csc_s *name) {
    s32 i;
    void *obj;
    void *tmp;
    char *path;

    while (name->path != 0) {
        path = name->path;
        for (i = 0; i < ghg_inst_count; i++) {
            if (strcasecmp(path, ghg_insts[i].path) == 0) {
                obj = ghg_insts[i].obj;
                goto found;
            }
        }
        ghg_insts[i].path = path;
        tmp = NuGHGRead(&superbuffer_ptr, path);
        ghg_insts[i].obj = tmp;
        obj = tmp;
        ghg_inst_count++;
    found:
        name->obj = obj;
        name++;
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

/* Inline so that its code is deferred to the end of the unit, after its
 * strings were emitted here, as in retail. */
static inline void AppCutSceneFindCharacters(struct NUGCUTSCENE_s *cutscene) {
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
            NuDebugMsgProlog(".\\cut.c", 0x2a1)("AppCutSceneFindCharacters: cannot fixup cutscene character <%s>",
                                                cutchar->name);
        }
        if (cutchar->nlocators != 0 && (nusintptr)cutchar->locators < 0xff) {
            cutchar->first_locator = (u8)(nusintptr)cutchar->locators;
            cutchar->locators = &locator->locators[(nusintptr)cutchar->locators];
        } else {
            cutchar->first_locator = -1;
        }
    }
}

void locatorfn_fadeout(struct instNUGCUTSCENE_s *icutscene, struct NUGCUTLOCATORSYS_s *locatorsys,
                       struct instNUGCUTLOCATOR_s *ilocator, struct NUGCUTLOCATOR_s *locator, f32 currf,
                       struct numtx_s *wm);

static struct NUGCUTLOCFNDATA_s cutscene_locatorfns[2] = {
    { "FadeOut", locatorfn_fadeout },
    { 0, 0 }
};

struct nucolour3_s DirCol[3] = {0};

struct nuvec_s DirVec[3] = {
    { 1.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f }
};

struct nucolour3_s AmbCol = {0};
struct nuvec_s cutcampos_FRONTEND = { -2.5f, 8.72f, 0.68f };
struct nuvec_s cutcamlook_FRONTEND = { 0.0f, 9.88f, 4.58f };
struct nuvec_s cutpos_CRASH = { 0.0f, 7.9f, 2.31f };

struct nuvec_s cutpos_FRONTEND[2] = {
    { -0.72f, 9.88f, 4.58f },
    { 0.72f, 9.88f, 4.58f }
};

struct nuvec_s cutpos_SPACE[3] = {
    { -0.2f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f },
    { 0.2f, 0.0f, 0.0f }
};

struct nuvec_s campos_SPACE = { 0.0f, 0.0f, -1.15f };

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

extern u8 texanimbuff[];
extern void LoadFont3D(void);
#ifndef NW_HOST /* the host takes the prototypes of nucore/nufile.h */
extern void *NuDatOpen(char *name, char **mem, s32 *used);
extern void NuDatSet(struct nudathdr_s *ndh);
extern void NuDatClose(struct nudathdr_s *ndh);
#endif
struct nutexanimprog_s;
void CutSceneNextLogoFunction(struct instNUGCUTSCENE_s *icutscene);

extern union variptr_u superbuffer_end;
extern void InitSpecular(void);
extern void InitEnviro(void);
void CutSceneEndFunction(struct instNUGCUTSCENE_s *icutscene);
void CutSceneNextFunction(struct instNUGCUTSCENE_s *icutscene);

/* Load the characters and every scene/cutscene of a movie, sharing models and
 * scenes already loaded, and chain the cutscenes so each plays the next.
 * Returns the number of cutscenes. */
s32 LoadCutComponents(struct cutscenedesc_s *csd, struct csc_s *chars) {
    s32 ix = 0;
    s32 i;
    char *scnpath;
    char *path;
    void *scene;
    char *end;
    char *instname;
    void *tmp;
    struct NUGCUTSCENE_s **cutscn;

    InitSpecular();
    InitEnviro();
    CutChar = chars;
    LoadCutSceneCharacters(chars);

    for (; csd[ix].gscname != 0; ix++) {
        cutdebgroups[ix] = csd[ix].debgroup;

        scnpath = csd[ix].gscname;
        end = superbuffer_end.charptr;
        for (i = 0; i < scene_inst_count; i++) {
            if (strcasecmp(scnpath, scene_insts[i].path) == 0) {
                scene = scene_insts[i].scene;
                goto found;
            }
        }
        scene_insts[i].path = scnpath;
        tmp = NuGScnRead(&superbuffer_ptr.charptr, end, scnpath);
        scene_insts[i].scene = tmp;
        scene = tmp;
        scene_inst_count++;
    found:
        world_scene[ix] = scene;

        CutScene[ix] = NuGCutSceneLoad(csd[ix].cutname, &superbuffer_ptr, &superbuffer_end);
        cutscn = &CutScene[ix];
        if (*cutscn != 0) {
            NuGCutSceneFixUp(*cutscn, world_scene[ix], 0);
            /* The instance is named after the last 16 characters of the path. */
            path = csd[ix].cutname;
            if (strlen(csd[ix].cutname) != 0x10) {
                path -= 0x10;
                instname = path + strlen(csd[ix].cutname);
            } else {
                instname = path;
            }
            CutInst[ix] = instNuGCutSceneCreate(*cutscn, 0, 0, instname, &superbuffer_ptr);
            if (CutInst[ix] != 0) {
                CutInst[ix]->speed = 0.6f;
                CutAudio[ix] = csd[ix].sfxid;
                instNuGCutSceneSetEndCallback(CutInst[ix], CutSceneEndFunction);
                if (ix != 0 && CutInst[ix - 1] != 0) {
                    CutInst[ix - 1]->chain = CutInst[ix];
                    instNuGCutSceneSetEndCallback(CutInst[ix - 1], CutSceneNextFunction);
                }
            }
        }
    }
    return ix;
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
    scene_inst_count = 0;
    for (i = 0x1F; i >= 0; i--) {
        world_scene[i] = 0;
    }
    ghg_inst_count = 0;
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
        dat = NuDatOpen("ats.dat", 0, 0);
        NuDatSet(dat);
        buf.voidptr = texanimbuff;
        NuTexAnimProgReadScript(&buf, "ats\\levels\\pal\\title.ats");
        NuDatSet(0);
        if (dat != 0) {
            NuDatClose(dat);
        }
        count = LoadCutComponents(csd_titles, csc_titles);
        instNuGCutSceneSetEndCallback(CutInst[0], CutSceneNextLogoFunction);
        CutInst[count - 1]->flags |= NUGCUTSCENE_LOOP;
        instNuGCutSceneSetEndCallback(CutInst[count - 1], 0);
        break;
    case 1:
        dat = NuDatOpen("ats.dat", 0, 0);
        NuDatSet(dat);
        buf.voidptr = texanimbuff;
        NuTexAnimProgReadScript(&buf, "ats\\splash\\ripples.ats");
        NuDatSet(0);
        if (dat != 0) {
            NuDatClose(dat);
        }
        LoadCutComponents(csd_intro1and2, csc_intro1and2);
        break;
    case 3:
    case 4:
        dat = NuDatOpen("ats.dat", 0, 0);
        NuDatSet(dat);
        buf.voidptr = texanimbuff;
        NuTexAnimProgReadScript(&buf, "ats\\elecy\\elecy2.ats");
        if (movie == 4) {
            NuTexAnimProgReadScript(&buf, "ats\\splash\\ripples.ats");
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

static f32 cutratefrig = 0.0f;
s32 cutmovie = -1;
u16 cutang_CRASH = 0;
s32 gamecut = -1;
s32 gamecut_start = 0;
s32 gamecut_finished = 0;
s32 gamecut_newlevel = -1;
s32 gamecut_sfx = -1;
struct pcutanm_s *pCutAnim = 0;
s32 gamecut_hack = 0;
f32 SPACEMASKADJUSTY = -0.175f;
f32 FRONTENDCORTEXADJUSTY = -0.15f;
f32 HUBHOLOSCALE = 3.0f;

void SetCutMovieRate(void) {
    CutInst[cutworldix]->speed = 0.5f;
    if (cutmovie == 3 || cutmovie == 4) {
        if (cutworldix == 0) {
            CutInst[cutworldix]->speed *= 0.8f;
        }
    }
    CutInst[cutworldix]->speed *= 1.2f;
    if (cutratefrig != 0.0f) {
        CutInst[cutworldix]->speed = cutratefrig;
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
    if (ghg_inst_count != 0) {
        for (i = 0; i < ghg_inst_count; i++) {
            NuHGobjDestroy(ghg_insts[i].obj);
        }
    }
    ghg_inst_count = 0;
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

extern s32 SWIDTH;
extern s32 SHEIGHT;
extern f32 DIVPANEL3D;
extern f32 PANEL3DMULX;
extern f32 PANEL3DMULY;
extern s32 mg_frame;
extern s32 fadehack;
extern struct nugscn_s *font3d_scene;
extern struct nucamera_s global_camera;
extern struct cammtx_s GameCam[];
extern struct cammtx_s *pCam;
extern struct gtimer_s GameTimer;
extern struct nupad_s *Pad[];

extern void ResetSuperBuffer(void);
extern void DebrisSetup(void);
extern void DebrisRegisterCutoffCameraVec(struct nuvec_s *cutoff);
extern void DebrisSetRenderGroup(s32 group);
extern void Debris(int);
extern void ParticleReset(void);
extern void edppDestroyAllParticles(void);
extern void edppDestroyAllEffects(void);
extern int edppLoadEffects(char *filename, signed char list);
extern int edppMergeEffects(char *filename, signed char list);
extern void edppRestartAllEffectsInLevel(void);
extern int NuFileSize(char *name);
extern void CreateFadeMtl(void);
extern s32 BGLoadCutMovie(s32 movie);
extern void ResetTimer(void *timer);
extern void MaxVP(void);
extern void DoInput(void);
extern void UpdateGameSfx(void);
extern void NuGScnUpdate(struct nugscn_s *scene, float time);
extern void NuGScnRndr3(struct nugscn_s *scene);
extern void GameTiming(void);
extern void ProcMenu(struct cursor_s *cursor, struct nupad_s *pad);
extern void UpdateFade(void);
extern void DrawFade(void);
extern void DrawPanel();
extern void DebrisDraw();
extern s32 NuRndrBeginScene(s32 a);
extern void NuRndrClear(s32 mode, s32 col, float z);
extern void NuRndrEndScene(void);
extern int NuRndrSwapScreen(int target);
extern void NuMtlAnimate(float dt);
extern void SetCutSceneLights(void);

inline void UpdateCutMovie(void) {
    if (cutmovie_hold != 0 && CutAudio[cutworldix] != -1 &&
        NuSoundKeyStatus(4) != 1) {
        CutInst[cutworldix]->speed = 0.0f;
    } else {
        cutmovie_hold = 0;
        SetCutMovieRate();
    }
    NuGCutSceneSysUpdate(Paused);
}


inline void DrawCutMovie(void) {
    NuGCutSceneSysRender();
}


inline void UpdateCutMovieCamera(struct numtx_s *cam) {
    if (set_cutscenecammtx != 0) {
        *cam = cutscenecammtx;
        set_cutscenecammtx = 0;
        *pNuCam = *cam;
        NuCameraSet(pNuCam);
    }
}


void PlayCutMovie(s32 movie) {
    s32 quit;

    PANEL3DMULX = (float)SWIDTH / (DIVPANEL3D * ((float)SWIDTH / 640.0f));
    PANEL3DMULY = (float)SHEIGHT / (DIVPANEL3D * ((float)SHEIGHT / 448.0f));
    do {
        next_cut_movie = -1;
        ResetSuperBuffer();
        DebrisSetup();
        DebrisRegisterCutoffCameraVec((struct nuvec_s *)&global_camera.mtx._30);
        ParticleReset();
        edppDestroyAllParticles();
        edppDestroyAllEffects();
        if (NuFileSize("levels\\b\\intro\\intro.ptl") > 0) {
            edppLoadEffects("levels\\b\\intro\\intro.ptl", 1);
        }
        if (NuFileSize("stuff\\general.ptl") > 0) {
            edppMergeEffects("stuff\\general.ptl", 0);
        }
        edppRestartAllEffectsInLevel();
        CreateFadeMtl();
        texanimbits = 0;
        font3d_scene = 0;
        mg_frame = -1;
        if (BGLoadCutMovie(movie) != 0) {
            StartCutMovie();
            quit = 0;
            ResetTimer(&GameTimer);
            Paused = 0;
            MaxVP();
            cut_on = 1;
            fadeval = 0xFF;
            fade_rate = -10;
            do {
                DebrisSetRenderGroup(cutdebgroups[cutworldix]);
                DoInput();
                UpdateGameSfx();
                NuSoundUpdate();
                UpdateCutMovie();
                if (cut_on != 0) {
                    UpdateCutMovieCamera(&GameCam[0].m);
                    Debris(0);
                    if (world_scene[cutworldix] != 0) {
                        NuGScnUpdate(world_scene[cutworldix], 1.0f);
                    }
                    GameTiming();
                    if (cutmovie == 0) {
                        ProcMenu(&Cursor, Pad[0]);
                    }
                    UpdateFade();
                    if (cutmovie != 0 && Pad[0] != 0 && (Pad[0]->paddata_db & 0x800)) {
                        next_cut_movie = -1;
                        quit = 1;
                    }
                    NuMtlAnimate(0.02f);
                    NuTexAnimSetSignals(texanimbits);
                    NuTexAnimProcess();
                    pCam = GameCam;
                    if (NuRndrBeginScene(-1) != 0) {
                        NuRndrClear(3, 0, 1.0f);
                        if (quit == 0) {
                            SetCutSceneLights();
                            DrawCutMovie();
                            if (world_scene[cutworldix] != 0) {
                                NuGScnRndr3(world_scene[cutworldix]);
                            }
                            DebrisDraw(0);
                            if (cutmovie == 0) {
                                DrawPanel();
                            }
                            DrawFade();
                        }
                        NuRndrEndScene();
                    }
                    NuRndrSwapScreen(-1);
                }
                if (cutmovie != 0) {
                    if (cut_on == 0 || quit != 0) {
                        break;
                    }
                } else if (fadeval >= 0xFF && fadehack == 0) {
                    break;
                }
            } while (1);
        }
        NuSoundUpdate();
        NuSoundStopStream(0);
        NuSoundStopStream(1);
        NuSoundStopStream(2);
        NuSoundStopStream(3);
        NuSoundStopStream(4);
        NuSoundUpdate();
        NuSoundFlushLoops();
        if (NuRndrBeginScene(-1) != 0) {
            NuRndrClear(3, 0, 1.0f);
            NuRndrEndScene();
            NuRndrSwapScreen(-1);
        }
        CloseCutMovie(1);
        movie = cutmovie = next_cut_movie;
    } while (cutmovie >= 0);
    DebrisSetRenderGroup(0);
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

extern struct nuvec_s v000;
extern s32 new_level;
extern s32 new_mode;
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
            if (pCutAnim->character >= 0 && pCutAnim->character < 0xBF) {
                character = CRemap[pCutAnim->character];
                if (character != -1) {
                    model = &CModel[character];
                }
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
            character = CRemap[183];
            if (character != -1) {
                model = &CModel[character];
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
void DrawGameCut(void) {
    struct numtx_s m;
    struct numtx_s tmtx[256];
    struct nuvec_s scale;
    struct nuvec_s pos;
    float **dwa;
    struct CharacterModel *model;
    u16 yrot;

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
        yrot = cutang_SPACE[pCutAnim->i];
        yrot -= 0x8000;
        pos = cutpos_SPACE[pCutAnim->i];
        if (model->character == 0x55 || model->character == 0x57 ||
            model->character == 0x56 || model->character == 0x58 ||
            model->character == 2) {
            pos.y += SPACEMASKADJUSTY * scale.y;
        }
    } else {
        yrot = cutang_FRONTEND[pCutAnim->i];
        yrot -= 0x8000;
        pos = cutpos_FRONTEND[pCutAnim->i];
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

/* Hub/space cutscenes: who plays which animation and voice line (GameCut_*),
 * the space level's character animations (Space_*_PlayList), by gamecut. */
struct pcutanm_s GameCut_EARTH_CORTEX[10] = {
    { 181, 12, -1, 0, -1 },
    { 181, 6, 197, 0, -1 },
    { 181, 13, -1, 0, -1 },
    { 180, 12, -1, 1, -1 },
    { 180, 6, 203, 1, -1 },
    { 180, 13, -1, 1, -1 },
    { 181, 12, -1, 0, -1 },
    { 181, 7, 198, 0, -1 },
    { 181, 13, -1, 0, -1 },
    { -1, 0, 0, 0, 0 }
};

SPACEANIM Space_EARTH_1STLEVEL_PlayList[3] = {
    { 85, {
        { "userth1", 6, 10, 10, 0.8f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "hubin", 12, 10, 0, 0.8f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "hubout", 13, 0, 10, 0.8f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 183, {
        { "swirl", 34, 0, 0, 0.2f, 3, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 255 }
};

struct pcutanm_s GameCut_EARTH_1STLEVEL[4] = {
    { 85, 12, -1, 1, -1 },
    { 85, 6, 213, 1, -1 },
    { 85, 13, -1, 1, -1 },
    { -1, 0, 0, 0, 0 }
};

SPACEANIM Space_EARTH_2CRYSTALS_PlayList[3] = {
    { 85, {
        { "userth2", 6, 10, 10, 0.8f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "hubin", 12, 10, 0, 0.8f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "hubout", 13, 0, 10, 0.8f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 183, {
        { "swirl", 34, 0, 0, 0.2f, 3, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 255 }
};

struct pcutanm_s GameCut_EARTH_2CRYSTALS[4] = {
    { 85, 12, -1, 1, -1 },
    { 85, 6, 214, 1, -1 },
    { 85, 13, -1, 1, -1 },
    { -1, 0, 0, 0, 0 }
};

SPACEANIM Space_EARTH_4CRYSTALS_PlayList[3] = {
    { 85, {
        { "userth5", 6, 10, 10, 0.8f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "hubin", 12, 10, 0, 0.8f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "hubout", 13, 0, 10, 0.8f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 183, {
        { "swirl", 34, 0, 0, 0.2f, 3, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 255 }
};

struct pcutanm_s GameCut_EARTH_4CRYSTALS[4] = {
    { 85, 12, -1, 1, -1 },
    { 85, 6, 215, 1, -1 },
    { 85, 13, -1, 1, -1 },
    { -1, 0, 0, 0, 0 }
};

SPACEANIM Space_EARTH_BOSS_PlayList[4] = {
    { 85, {
        { "userth7", 6, 10, 10, 0.8f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "hubin", 12, 10, 0, 0.8f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "hubout", 13, 0, 10, 0.8f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 179, {
        { "b\\uscrn02", 6, 10, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "b\\hubin", 12, 10, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "b\\hubout", 13, 10, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 183, {
        { "swirl", 34, 0, 0, 0.2f, 3, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 255 }
};

struct pcutanm_s GameCut_EARTH_BOSS[7] = {
    { 85, 12, -1, 0, -1 },
    { 85, 6, 216, 0, -1 },
    { 85, 13, -1, 0, -1 },
    { 179, 12, -1, 2, -1 },
    { 179, 6, 206, 2, -1 },
    { 179, 13, -1, 2, -1 },
    { -1, 0, 0, 0, 0 }
};

SPACEANIM Space_EARTH_AKUAKU_PlayList[3] = {
    { 3, {
        { "usaku24", 6, 10, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "hubin", 12, 10, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "hubout", 13, 10, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 183, {
        { "swirl", 34, 0, 0, 0.2f, 3, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 255 }
};

struct pcutanm_s GameCut_EARTH_AKUAKU[4] = {
    { 3, 12, -1, 1, -1 },
    { 3, 6, 201, 1, -1 },
    { 3, 13, -1, 1, -1 },
    { -1, 0, 0, 0, 0 }
};

struct pcutanm_s GameCut_WATER_CORTEX[10] = {
    { 182, 12, -1, 0, -1 },
    { 182, 6, 207, 0, -1 },
    { 182, 13, -1, 0, -1 },
    { 181, 12, -1, 1, -1 },
    { 181, 8, 199, 1, -1 },
    { 181, 13, -1, 1, -1 },
    { 180, 12, -1, 0, -1 },
    { 180, 7, 204, 0, -1 },
    { 180, 13, -1, 0, -1 },
    { -1, 0, 0, 0, 0 }
};

SPACEANIM Space_WATER_1STLEVEL_PlayList[3] = {
    { 87, {
        { "uswa1", 6, 10, 10, 0.8f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "hubin", 12, 10, 0, 0.8f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "hubout", 13, 0, 10, 0.8f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 183, {
        { "swirl", 34, 0, 0, 0.2f, 3, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 255 }
};

struct pcutanm_s GameCut_WATER_1STLEVEL[4] = {
    { 87, 12, -1, 1, -1 },
    { 87, 6, 221, 1, -1 },
    { 87, 13, -1, 1, -1 },
    { -1, 0, 0, 0, 0 }
};

SPACEANIM Space_WATER_2CRYSTALS_PlayList[3] = {
    { 87, {
        { "uswa2", 6, 10, 10, 0.8f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "hubin", 12, 10, 0, 0.8f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "hubout", 13, 0, 10, 0.8f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 183, {
        { "swirl", 34, 0, 0, 0.2f, 3, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 255 }
};

struct pcutanm_s GameCut_WATER_2CRYSTALS[4] = {
    { 87, 12, -1, 1, -1 },
    { 87, 6, 222, 1, -1 },
    { 87, 13, -1, 1, -1 },
    { -1, 0, 0, 0, 0 }
};

SPACEANIM Space_WATER_4CRYSTALS_PlayList[3] = {
    { 87, {
        { "uswa4", 6, 10, 10, 0.8f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "hubin", 12, 10, 0, 0.8f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "hubout", 13, 0, 10, 0.8f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 183, {
        { "swirl", 34, 0, 0, 0.2f, 3, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 255 }
};

struct pcutanm_s GameCut_WATER_4CRYSTALS[4] = {
    { 87, 12, -1, 1, -1 },
    { 87, 6, 223, 1, -1 },
    { 87, 13, -1, 1, -1 },
    { -1, 0, 0, 0, 0 }
};

SPACEANIM Space_WATER_BOSS_PlayList[4] = {
    { 87, {
        { "uswa8", 6, 10, 10, 0.8f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "hubin", 12, 10, 0, 0.8f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "hubout", 13, 0, 10, 0.8f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 179, {
        { "c\\uscrn07", 6, 10, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "b\\hubin", 12, 10, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "b\\hubout", 13, 10, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 183, {
        { "swirl", 34, 0, 0, 0.2f, 3, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 255 }
};

struct pcutanm_s GameCut_WATER_BOSS[7] = {
    { 87, 12, -1, 0, -1 },
    { 87, 6, 224, 0, -1 },
    { 87, 13, -1, 0, -1 },
    { 179, 12, -1, 2, -1 },
    { 179, 6, 207, 2, -1 },
    { 179, 13, -1, 2, -1 },
    { -1, 0, 0, 0, 0 }
};

struct pcutanm_s GameCut_FIRE_CORTEX[10] = {
    { 181, 12, -1, 0, -1 },
    { 181, 10, 201, 0, -1 },
    { 181, 13, -1, 0, -1 },
    { 180, 12, -1, 1, -1 },
    { 180, 9, 206, 1, -1 },
    { 180, 13, -1, 1, -1 },
    { 181, 12, -1, 0, -1 },
    { 181, 11, 202, 0, -1 },
    { 181, 13, -1, 0, -1 },
    { -1, 0, 0, 0, 0 }
};

SPACEANIM Space_FIRE_1STLEVEL_PlayList[3] = {
    { 86, {
        { "usfire2", 6, 10, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "hubin", 12, 10, 0, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "hubout", 13, 0, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 183, {
        { "swirl", 34, 0, 0, 0.2f, 3, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 255 }
};

struct pcutanm_s GameCut_FIRE_1STLEVEL[4] = {
    { 86, 12, -1, 1, -1 },
    { 86, 6, 217, 1, -1 },
    { 86, 13, -1, 1, -1 },
    { -1, 0, 0, 0, 0 }
};

SPACEANIM Space_FIRE_2CRYSTALS_PlayList[3] = {
    { 86, {
        { "usfire5", 6, 10, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "hubin", 12, 10, 0, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "hubout", 13, 0, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 183, {
        { "swirl", 34, 0, 0, 0.2f, 3, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 255 }
};

struct pcutanm_s GameCut_FIRE_2CRYSTALS[4] = {
    { 86, 12, -1, 1, -1 },
    { 86, 6, 218, 1, -1 },
    { 86, 13, -1, 1, -1 },
    { -1, 0, 0, 0, 0 }
};

SPACEANIM Space_FIRE_4CRYSTALS_PlayList[3] = {
    { 86, {
        { "usfire6", 6, 10, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "hubin", 12, 10, 0, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "hubout", 13, 0, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 183, {
        { "swirl", 34, 0, 0, 0.2f, 3, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 255 }
};

struct pcutanm_s GameCut_FIRE_4CRYSTALS[4] = {
    { 86, 12, -1, 1, -1 },
    { 86, 6, 219, 1, -1 },
    { 86, 13, -1, 1, -1 },
    { -1, 0, 0, 0, 0 }
};

SPACEANIM Space_FIRE_BOSS_PlayList[4] = {
    { 86, {
        { "usfire7", 6, 10, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "hubin", 12, 10, 0, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "hubout", 13, 0, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 179, {
        { "c\\uscrn10", 6, 10, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "b\\hubin", 12, 10, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "b\\hubout", 13, 10, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 183, {
        { "swirl", 34, 0, 0, 0.2f, 3, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 255 }
};

struct pcutanm_s GameCut_FIRE_BOSS[7] = {
    { 86, 12, -1, 0, -1 },
    { 86, 6, 220, 0, -1 },
    { 86, 13, -1, 0, -1 },
    { 179, 12, -1, 2, -1 },
    { 179, 6, 209, 2, -1 },
    { 179, 13, -1, 2, -1 },
    { -1, 0, 0, 0, 0 }
};

struct pcutanm_s GameCut_WEATHER_CORTEX[10] = {
    { 182, 12, -1, 0, -1 },
    { 182, 7, 208, 0, -1 },
    { 182, 13, -1, 0, -1 },
    { 180, 12, -1, 0, -1 },
    { 180, 8, 205, 0, -1 },
    { 180, 13, -1, 0, -1 },
    { 181, 12, -1, 1, -1 },
    { 181, 9, 200, 1, -1 },
    { 181, 13, -1, 1, -1 },
    { -1, 0, 0, 0, 0 }
};

SPACEANIM Space_WEATHER_1STLEVEL_PlayList[3] = {
    { 88, {
        { "usair01", 6, 10, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "hubin", 12, 10, 0, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "hubout", 13, 0, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 183, {
        { "swirl", 34, 0, 0, 0.2f, 3, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 255 }
};

struct pcutanm_s GameCut_WEATHER_1STLEVEL[4] = {
    { 88, 12, -1, 1, -1 },
    { 88, 6, 197, 1, -1 },
    { 88, 13, -1, 1, -1 },
    { -1, 0, 0, 0, 0 }
};

SPACEANIM Space_WEATHER_2CRYSTALS_PlayList[3] = {
    { 88, {
        { "usair02", 6, 10, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "hubin", 12, 10, 0, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "hubout", 13, 0, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 183, {
        { "swirl", 34, 0, 0, 0.2f, 3, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 255 }
};

struct pcutanm_s GameCut_WEATHER_2CRYSTALS[4] = {
    { 88, 12, -1, 1, -1 },
    { 88, 6, 198, 1, -1 },
    { 88, 13, -1, 1, -1 },
    { -1, 0, 0, 0, 0 }
};

SPACEANIM Space_WEATHER_4CRYSTALS_PlayList[3] = {
    { 88, {
        { "usair04", 6, 10, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "hubin", 12, 10, 0, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "hubout", 13, 0, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 183, {
        { "swirl", 34, 0, 0, 0.2f, 3, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 255 }
};

struct pcutanm_s GameCut_WEATHER_4CRYSTALS[4] = {
    { 88, 12, -1, 1, -1 },
    { 88, 6, 199, 1, -1 },
    { 88, 13, -1, 1, -1 },
    { -1, 0, 0, 0, 0 }
};

SPACEANIM Space_WEATHER_BOSS_PlayList[4] = {
    { 88, {
        { "usair06", 6, 10, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "hubin", 12, 10, 0, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "hubout", 13, 0, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 179, {
        { "c\\uscrn19", 6, 10, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "b\\hubin", 12, 10, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "b\\hubout", 13, 10, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 183, {
        { "swirl", 34, 0, 0, 0.2f, 3, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 255 }
};

struct pcutanm_s GameCut_WEATHER_BOSS[7] = {
    { 88, 12, -1, 0, -1 },
    { 88, 6, 200, 0, -1 },
    { 88, 13, -1, 0, -1 },
    { 179, 12, -1, 2, -1 },
    { 179, 6, 211, 2, -1 },
    { 179, 13, -1, 2, -1 },
    { -1, 0, 0, 0, 0 }
};

SPACEANIM Space_SPACE_1STLEVEL_PlayList[4] = {
    { 2, {
        { "b\\uscor56", 6, 10, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "b\\hubin", 12, 10, 0, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "b\\hubout", 13, 0, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 179, {
        { "b\\uscrn21", 6, 10, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "b\\hubin", 12, 10, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "b\\hubout", 13, 10, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 183, {
        { "swirl", 34, 0, 0, 0.2f, 3, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 255 }
};

struct pcutanm_s GameCut_SPACE_1STLEVEL[7] = {
    { 2, 12, -1, 0, -1 },
    { 2, 6, 205, 0, -1 },
    { 2, 13, -1, 0, -1 },
    { 179, 12, -1, 2, -1 },
    { 179, 6, 212, 2, -1 },
    { 179, 13, -1, 2, -1 },
    { -1, 0, 0, 0, 0 }
};

SPACEANIM Space_SPACE_2CRYSTALS_PlayList[3] = {
    { 2, {
        { "b\\uscor44", 6, 10, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "b\\hubin", 12, 10, 0, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "b\\hubout", 13, 0, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 183, {
        { "swirl", 34, 0, 0, 0.2f, 3, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 255 }
};

struct pcutanm_s GameCut_SPACE_2CRYSTALS[4] = {
    { 2, 12, -1, 1, -1 },
    { 2, 6, 202, 1, -1 },
    { 2, 13, -1, 1, -1 },
    { -1, 0, 0, 0, 0 }
};

SPACEANIM Space_SPACE_4CRYSTALS_PlayList[3] = {
    { 2, {
        { "b\\uscor54", 6, 10, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "b\\hubin", 12, 10, 0, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "b\\hubout", 13, 0, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 183, {
        { "swirl", 34, 0, 0, 0.2f, 3, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 255 }
};

struct pcutanm_s GameCut_SPACE_4CRYSTALS[4] = {
    { 2, 12, -1, 1, -1 },
    { 2, 6, 204, 1, -1 },
    { 2, 13, -1, 1, -1 },
    { -1, 0, 0, 0, 0 }
};

SPACEANIM Space_SPACE_BOSS_PlayList[4] = {
    { 2, {
        { "b\\uscor52", 6, 10, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "b\\hubin", 12, 10, 0, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "b\\hubout", 13, 0, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 179, {
        { "b\\uscrn12", 6, 10, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "b\\hubin", 12, 10, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL },
        { "b\\hubout", 13, 10, 10, 1.0f, 6, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 183, {
        { "swirl", 34, 0, 0, 0.2f, 3, -1, -1, 0xFFFFFFFFFFFFFFFFULL }
    } },
    { 255 }
};

struct pcutanm_s GameCut_SPACE_BOSS[7] = {
    { 2, 12, -1, 0, -1 },
    { 2, 6, 203, 0, -1 },
    { 2, 13, -1, 0, -1 },
    { 179, 12, -1, 2, -1 },
    { 179, 6, 210, 2, -1 },
    { 179, 13, -1, 2, -1 },
    { -1, 0, 0, 0, 0 }
};

void *SpaceGameCutTab[26][2] = {
    { 0, GameCut_EARTH_CORTEX },
    { Space_EARTH_1STLEVEL_PlayList, GameCut_EARTH_1STLEVEL },
    { Space_EARTH_2CRYSTALS_PlayList, GameCut_EARTH_2CRYSTALS },
    { Space_EARTH_4CRYSTALS_PlayList, GameCut_EARTH_4CRYSTALS },
    { Space_EARTH_BOSS_PlayList, GameCut_EARTH_BOSS },
    { Space_EARTH_AKUAKU_PlayList, GameCut_EARTH_AKUAKU },
    { 0, GameCut_WATER_CORTEX },
    { Space_WATER_1STLEVEL_PlayList, GameCut_WATER_1STLEVEL },
    { Space_WATER_2CRYSTALS_PlayList, GameCut_WATER_2CRYSTALS },
    { Space_WATER_4CRYSTALS_PlayList, GameCut_WATER_4CRYSTALS },
    { Space_WATER_BOSS_PlayList, GameCut_WATER_BOSS },
    { 0, GameCut_FIRE_CORTEX },
    { Space_FIRE_1STLEVEL_PlayList, GameCut_FIRE_1STLEVEL },
    { Space_FIRE_2CRYSTALS_PlayList, GameCut_FIRE_2CRYSTALS },
    { Space_FIRE_4CRYSTALS_PlayList, GameCut_FIRE_4CRYSTALS },
    { Space_FIRE_BOSS_PlayList, GameCut_FIRE_BOSS },
    { 0, GameCut_WEATHER_CORTEX },
    { Space_WEATHER_1STLEVEL_PlayList, GameCut_WEATHER_1STLEVEL },
    { Space_WEATHER_2CRYSTALS_PlayList, GameCut_WEATHER_2CRYSTALS },
    { Space_WEATHER_4CRYSTALS_PlayList, GameCut_WEATHER_4CRYSTALS },
    { Space_WEATHER_BOSS_PlayList, GameCut_WEATHER_BOSS },
    {0},
    { Space_SPACE_1STLEVEL_PlayList, GameCut_SPACE_1STLEVEL },
    { Space_SPACE_2CRYSTALS_PlayList, GameCut_SPACE_2CRYSTALS },
    { Space_SPACE_4CRYSTALS_PlayList, GameCut_SPACE_4CRYSTALS },
    { Space_SPACE_BOSS_PlayList, GameCut_SPACE_BOSS }
};

AnimPacket CutAnim = {0};
struct gtimer_s CutTimer = {0};

AnimPacket CutVortexAnim = {0};


s32 next_cut_movie = 0;
u16 cutang_FRONTEND[2] = {0};
u16 cutang_SPACE[3] = {0};
s32 gamecut_hold = 0;

extern void SetLights(struct nucolour3_s *col0, struct nuvec_s *dir0,
                      struct nucolour3_s *col1, struct nuvec_s *dir1,
                      struct nucolour3_s *col2, struct nuvec_s *dir2, struct nuvec_s *amb);
extern void SetLevelLights(void);

inline void MakeDirLightColour(s32 ix, f32 r, f32 g, f32 b) {
    cutdircol[ix].r = r;
    cutdircol[ix].g = g;
    cutdircol[ix].b = b;
}


inline void MakeLightDirection(s32 ix, f32 x, f32 y, f32 z) {
    cutdir[ix].x = x;
    cutdir[ix].y = y;
    cutdir[ix].z = z;
    NuVecNorm(&cutdir[ix], &cutdir[ix]);
}


void SetCutSceneLights(void) {
    switch (cutmovie) {
    case 0:
        SetLevelLights();
        return;
    case 1:
        switch (cutworldix) {
        case 2:
            cutambcol.x = 0.4f;
            cutambcol.y = 0.4f;
            cutambcol.z = 0.4f;
            MakeDirLightColour(0, 0.4f, 0.8f, 0.4f);
            MakeDirLightColour(1, 0.4f, 0.8f, 0.4f);
            MakeDirLightColour(2, 0.0f, 0.0f, 0.0f);
            MakeLightDirection(0, -0.5f, -1.0f, -0.5f);
            MakeLightDirection(1, 0.5f, -1.0f, 0.5f);
            MakeLightDirection(2, 0.0f, 0.0f, 0.0f);
            SetLights(&cutdircol[0], &cutdir[0], &cutdircol[1], &cutdir[1], &cutdircol[2], &cutdir[2],
                      &cutambcol);
            return;
        case 3:
            cutambcol.x = 0.75f;
            cutambcol.y = 0.75f;
            cutambcol.z = 0.75f;
            MakeDirLightColour(0, 1.25f, 1.0f, 0.65f);
            MakeDirLightColour(1, 0.5f, 0.5f, 0.8f);
            MakeDirLightColour(2, 0.5f, 0.5f, 0.8f);
            MakeLightDirection(0, -0.5f, 1.0f, -0.5f);
            MakeLightDirection(1, -0.5f, -1.0f, -0.5f);
            MakeLightDirection(2, 0.5f, -1.0f, 0.5f);
            SetLights(&cutdircol[0], &cutdir[0], &cutdircol[1], &cutdir[1], &cutdircol[2], &cutdir[2],
                      &cutambcol);
            return;
        case 4:
            cutambcol.x = 0.25f;
            cutambcol.y = 0.25f;
            cutambcol.z = 0.25f;
            MakeDirLightColour(0, 1.0f, 0.8f, 0.65f);
            MakeDirLightColour(1, 0.75f, 0.75f, 0.75f);
            MakeDirLightColour(2, 1.5f, 1.5f, 1.0f);
            MakeLightDirection(0, -0.5f, 1.0f, 0.5f);
            MakeLightDirection(1, 0.5f, 1.0f, -0.5f);
            MakeLightDirection(2, -1.0f, 0.0f, 0.0f);
            SetLights(&cutdircol[0], &cutdir[0], &cutdircol[1], &cutdir[1], &cutdircol[2], &cutdir[2],
                      &cutambcol);
            return;
        case 5:
            cutambcol.x = 0.5f;
            cutambcol.y = 0.5f;
            cutambcol.z = 0.5f;
            MakeDirLightColour(0, 1.0f, 1.0f, 0.5f);
            MakeDirLightColour(1, 0.35f, 0.35f, 0.6f);
            MakeDirLightColour(2, 0.35f, 0.35f, 0.5f);
            MakeLightDirection(0, -0.5f, 1.0f, -0.5f);
            MakeLightDirection(1, -0.5f, -1.0f, -0.5f);
            MakeLightDirection(2, -1.0f, 0.0f, 0.0f);
            SetLights(&cutdircol[0], &cutdir[0], &cutdircol[1], &cutdir[1], &cutdircol[2], &cutdir[2],
                      &cutambcol);
            return;
        case 6:
            return;
        case 7:
            cutambcol.x = 0.75f;
            cutambcol.y = 0.75f;
            cutambcol.z = 0.75f;
            MakeDirLightColour(0, 0.9f, 0.9f, 0.5f);
            MakeDirLightColour(1, 0.0f, 0.0f, 0.0f);
            MakeDirLightColour(2, 0.0f, 0.0f, 0.0f);
            MakeLightDirection(0, -0.5f, 1.0f, -0.5f);
            MakeLightDirection(1, 0.0f, 0.0f, 0.0f);
            MakeLightDirection(2, 0.0f, 0.0f, 0.0f);
            SetLights(&cutdircol[0], &cutdir[0], &cutdircol[1], &cutdir[1], &cutdircol[2], &cutdir[2],
                      &cutambcol);
            return;
        case 8:
            cutambcol.x = 0.25f;
            cutambcol.y = 0.25f;
            cutambcol.z = 0.25f;
            MakeDirLightColour(0, 1.1f, 1.1f, 0.9f);
            MakeDirLightColour(1, 1.1f, 1.1f, 0.9f);
            MakeDirLightColour(2, 0.5f, 0.5f, 0.5f);
            MakeLightDirection(0, 0.25f, -1.0f, 0.0f);
            MakeLightDirection(1, -0.5f, 1.0f, 0.0f);
            MakeLightDirection(2, 0.25f, 1.0f, 0.0f);
            SetLights(&cutdircol[0], &cutdir[0], &cutdircol[1], &cutdir[1], &cutdircol[2], &cutdir[2],
                      &cutambcol);
            return;
        case 9:
            cutambcol.x = 0.75f;
            cutambcol.y = 0.75f;
            cutambcol.z = 0.75f;
            MakeDirLightColour(0, 0.8f, 0.8f, 0.5f);
            MakeDirLightColour(1, 0.0f, 0.25f, 0.0f);
            MakeDirLightColour(2, 0.0f, 0.0f, 0.0f);
            MakeLightDirection(0, 0.0f, 0.0f, -0.8f);
            MakeLightDirection(1, 0.0f, -1.0f, 1.0f);
            MakeLightDirection(2, 0.0f, 0.0f, 0.0f);
            SetLights(&cutdircol[0], &cutdir[0], &cutdircol[1], &cutdir[1], &cutdircol[2], &cutdir[2],
                      &cutambcol);
            return;
        case 10:
            cutambcol.x = 0.5f;
            cutambcol.y = 0.5f;
            cutambcol.z = 0.5f;
            MakeDirLightColour(0, 1.0f, 1.0f, 0.8f);
            MakeDirLightColour(1, 0.25f, 0.25f, 1.0f);
            MakeDirLightColour(2, 0.0f, 0.0f, 0.0f);
            MakeLightDirection(0, -1.0f, 0.75f, -1.0f);
            MakeLightDirection(1, 1.0f, 0.5f, 1.0f);
            MakeLightDirection(2, 0.0f, 0.0f, 0.0f);
            SetLights(&cutdircol[0], &cutdir[0], &cutdircol[1], &cutdir[1], &cutdircol[2], &cutdir[2],
                      &cutambcol);
            return;
        default:
            SetLevelLights();
            return;
        }
    case 2:
        switch (cutworldix) {
        case 0:
            cutambcol.x = 0.75f;
            cutambcol.y = 0.75f;
            cutambcol.z = 0.75f;
            MakeDirLightColour(0, 1.25f, 1.0f, 0.65f);
            MakeDirLightColour(1, 0.5f, 0.5f, 0.8f);
            MakeDirLightColour(2, 0.5f, 0.5f, 0.8f);
            MakeLightDirection(0, -0.5f, 1.0f, -0.5f);
            MakeLightDirection(1, -0.5f, -1.0f, -0.5f);
            MakeLightDirection(2, 0.5f, -1.0f, 0.5f);
            SetLights(&cutdircol[0], &cutdir[0], &cutdircol[1], &cutdir[1], &cutdircol[2], &cutdir[2],
                      &cutambcol);
            return;
        case 1:
            cutambcol.x = 0.25f;
            cutambcol.y = 0.25f;
            cutambcol.z = 0.25f;
            MakeDirLightColour(0, 1.0f, 0.8f, 0.65f);
            MakeDirLightColour(1, 0.75f, 0.75f, 0.75f);
            MakeDirLightColour(2, 1.5f, 1.5f, 1.0f);
            MakeLightDirection(0, -0.5f, 1.0f, 0.5f);
            MakeLightDirection(1, 0.5f, 1.0f, -0.5f);
            MakeLightDirection(2, -1.0f, 0.0f, 0.0f);
            SetLights(&cutdircol[0], &cutdir[0], &cutdircol[1], &cutdir[1], &cutdircol[2], &cutdir[2],
                      &cutambcol);
            return;
        case 2:
            cutambcol.x = 0.5f;
            cutambcol.y = 0.5f;
            cutambcol.z = 0.5f;
            MakeDirLightColour(0, 1.0f, 1.0f, 0.5f);
            MakeDirLightColour(1, 0.35f, 0.35f, 0.6f);
            MakeDirLightColour(2, 0.35f, 0.35f, 0.5f);
            MakeLightDirection(0, -0.5f, 1.0f, -0.5f);
            MakeLightDirection(1, -0.5f, -1.0f, -0.5f);
            MakeLightDirection(2, -1.0f, 0.0f, 0.0f);
            SetLights(&cutdircol[0], &cutdir[0], &cutdircol[1], &cutdir[1], &cutdircol[2], &cutdir[2],
                      &cutambcol);
            return;
        case 3:
            return;
        case 4:
            cutambcol.x = 0.75f;
            cutambcol.y = 0.75f;
            cutambcol.z = 0.75f;
            MakeDirLightColour(0, 0.9f, 0.9f, 0.5f);
            MakeDirLightColour(1, 0.0f, 0.0f, 0.0f);
            MakeDirLightColour(2, 0.0f, 0.0f, 0.0f);
            MakeLightDirection(0, -0.5f, 1.0f, -0.5f);
            MakeLightDirection(1, 0.0f, 0.0f, 0.0f);
            MakeLightDirection(2, 0.0f, 0.0f, 0.0f);
            SetLights(&cutdircol[0], &cutdir[0], &cutdircol[1], &cutdir[1], &cutdircol[2], &cutdir[2],
                      &cutambcol);
            return;
        case 5:
            cutambcol.x = 0.25f;
            cutambcol.y = 0.25f;
            cutambcol.z = 0.25f;
            MakeDirLightColour(0, 1.1f, 1.1f, 0.9f);
            MakeDirLightColour(1, 1.1f, 1.1f, 0.9f);
            MakeDirLightColour(2, 0.5f, 0.5f, 0.5f);
            MakeLightDirection(0, 0.25f, -1.0f, 0.0f);
            MakeLightDirection(1, -0.5f, 1.0f, 0.0f);
            MakeLightDirection(2, 0.25f, 1.0f, 0.0f);
            SetLights(&cutdircol[0], &cutdir[0], &cutdircol[1], &cutdir[1], &cutdircol[2], &cutdir[2],
                      &cutambcol);
            return;
        case 6:
            cutambcol.x = 0.75f;
            cutambcol.y = 0.75f;
            cutambcol.z = 0.75f;
            MakeDirLightColour(0, 0.8f, 0.8f, 0.5f);
            MakeDirLightColour(1, 0.0f, 0.25f, 0.0f);
            MakeDirLightColour(2, 0.0f, 0.0f, 0.0f);
            MakeLightDirection(0, 0.0f, 0.0f, -0.8f);
            MakeLightDirection(1, 0.0f, -1.0f, 1.0f);
            MakeLightDirection(2, 0.0f, 0.0f, 0.0f);
            SetLights(&cutdircol[0], &cutdir[0], &cutdircol[1], &cutdir[1], &cutdircol[2], &cutdir[2],
                      &cutambcol);
            return;
        case 7:
            cutambcol.x = 0.5f;
            cutambcol.y = 0.5f;
            cutambcol.z = 0.5f;
            MakeDirLightColour(0, 1.0f, 1.0f, 0.8f);
            MakeDirLightColour(1, 0.25f, 0.25f, 1.0f);
            MakeDirLightColour(2, 0.0f, 0.0f, 0.0f);
            MakeLightDirection(0, -1.0f, 0.75f, -1.0f);
            MakeLightDirection(1, 1.0f, 0.5f, 1.0f);
            MakeLightDirection(2, 0.0f, 0.0f, 0.0f);
            SetLights(&cutdircol[0], &cutdir[0], &cutdircol[1], &cutdir[1], &cutdircol[2], &cutdir[2],
                      &cutambcol);
            return;
        default:
            SetLevelLights();
            return;
        }
    case 3:
        if (cutworldix == 0) {
            cutambcol.x = 0.5f;
            cutambcol.y = 0.5f;
            cutambcol.z = 0.5f;
            MakeDirLightColour(0, 0.25f, 0.25f, 1.75f);
            MakeDirLightColour(1, 1.25f, 0.5f, 0.5f);
            MakeDirLightColour(2, 0.5f, 0.75f, 0.5f);
            MakeLightDirection(0, -1.0f, -0.5f, -1.0f);
            MakeLightDirection(1, 1.0f, -0.5f, -1.0f);
            MakeLightDirection(2, -1.0f, 0.5f, 1.0f);
            SetLights(&cutdircol[0], &cutdir[0], &cutdircol[1], &cutdir[1], &cutdircol[2], &cutdir[2],
                      &cutambcol);
            return;
        }
        SetLevelLights();
        return;
    case 4:
        switch (cutworldix) {
        case 0:
            cutambcol.x = 0.5f;
            cutambcol.y = 0.5f;
            cutambcol.z = 0.5f;
            MakeDirLightColour(0, 0.25f, 0.25f, 1.75f);
            MakeDirLightColour(1, 1.25f, 0.5f, 0.5f);
            MakeDirLightColour(2, 0.5f, 0.75f, 0.5f);
            MakeLightDirection(0, -1.0f, -0.5f, -1.0f);
            MakeLightDirection(1, 1.0f, -0.5f, -1.0f);
            MakeLightDirection(2, -1.0f, 0.5f, 1.0f);
            SetLights(&cutdircol[0], &cutdir[0], &cutdircol[1], &cutdir[1], &cutdircol[2], &cutdir[2],
                      &cutambcol);
            return;
        case 1:
            cutambcol.x = 0.5f;
            cutambcol.y = 0.5f;
            cutambcol.z = 0.5f;
            MakeDirLightColour(0, 0.75f, 0.65f, 0.5f);
            MakeDirLightColour(1, 0.0f, 0.15f, 0.0f);
            MakeDirLightColour(2, 0.5f, 0.5f, 0.25f);
            MakeLightDirection(0, 1.0f, -0.25f, 1.0f);
            MakeLightDirection(1, 0.0f, -1.0f, 0.0f);
            MakeLightDirection(2, 1.0f, -0.5f, -1.0f);
            SetLights(&cutdircol[0], &cutdir[0], &cutdircol[1], &cutdir[1], &cutdircol[2], &cutdir[2],
                      &cutambcol);
            return;
        case 2:
            cutambcol.x = 0.5f;
            cutambcol.y = 0.5f;
            cutambcol.z = 0.5f;
            MakeDirLightColour(0, 1.15f, 1.15f, 1.15f);
            MakeDirLightColour(1, 1.0f, 1.0f, 1.0f);
            MakeDirLightColour(2, 1.0f, 1.0f, 1.0f);
            MakeLightDirection(0, 1.0f, -0.25f, 1.0f);
            MakeLightDirection(1, 0.0f, -1.0f, 0.0f);
            MakeLightDirection(2, 1.0f, -0.5f, -1.0f);
            SetLights(&cutdircol[0], &cutdir[0], &cutdircol[1], &cutdir[1], &cutdircol[2], &cutdir[2],
                      &cutambcol);
            return;
        default:
            SetLevelLights();
            return;
        }
    }
    SetLevelLights();
}



void InitCutScenes(void) {
    NuGCutSceneSysInit(cutscene_locatorfns);
    NuSetCutSceneFindCharactersFn(AppCutSceneFindCharacters);
    NuSetCutSceneCharacterRenderFn(AppCutSceneCharacterRender);
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
