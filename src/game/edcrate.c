/*
 * Unit: game/edcrate
 *
 * Functions:
 *   0x0022c430 edcrtDrawSpecialCrates
 *   0x0022c6b8 edcrtDrawBoxyThing
 *   0x0022caf0 WriteCrateData
 *   0x0022cd68 edcrtcbMemCardSaveCrates
 *   0x0022d128 edcrtcbMemCardLoadCrates
 *   0x0022d500 edcrtcbMemCardMenu
 *   0x0022d7d0 edcrtcbSpecificTypeMenu
 *   0x0022d978 edcrtcbCrateTypesMenu
 *   0x0022dae0 edcrtcbDisplayTypeMenu
 *   0x0022dcc8 edcrtInit
 *   0x0022df08 edcrtDoInput
 *   0x0022e368 edcrtDrawInfo
 *   0x0022e520 edcrtcbMemCardLoadCratesMenu
 *   0x0022e670 edcrtcbChangeFileNameMenu
 *   0x0022e758 edcrtcbMemCardDeleteCratesMenu
 *   0x0022e8a8 edcrtcbCancelMemCardMenu
 *   0x0022e8d0 edcrtCbSaveCrateData
 *   0x0022e918 edcrtcbSetCrateType
 *   0x0022e978 edcrtcbCancelSpecificTypeMenu
 *   0x0022e9a0 edcrtcbCancelCrateTypesMenu
 *   0x0022e9c8 edcrtcbResetCratesMenu
 *   0x0022eb00 edcrtcbDisplayType
 *   0x0022eb38 edcrtcbCancelDisplayTypeMenu
 *   0x0022eb60 edcrtcbTerrainToggle
 *   0x0022eb70 edcrtcbShowTriggersToggle
 *   0x0022eb80 edcrtCbCancelOptMenu
 *   0x0022eb88 edcrtClose
 *   0x0022eba8 edcrtEnter
 *   0x0022ec60 edcrtProc
 *   0x0022ecc0 edcrtRender
 *   0x0022ed28 edcrtcbMemCardCancelLoadEffectsMenu
 *   0x0022ed50 edcrtcbChangeFileName
 *   0x0022ed78 edcrtcbCancelChangeFileNameMenu
 *   0x0022eda0 edcrtcbMemCardDeleteCrates
 *   0x0022ee60 edcrtcbMemCardCancelDeleteEffectsMenu
 *   0x0022ee88 edcrtcbResetCrates
 *   0x0022eee8 edcrtcbCancelResetCratesMenu
 */

#include "nu_libc.h"
#include "nu_port.h"

#include "numath/nuvec.h"
#include "nu3d/numtl.h"
#include "nu3d/nugobj.h"
#include "nu3d/nurndr.h"
#include "game/gametypes.h"
#include "gamelib/terrain.h"
#include "game/creature.h"
#include "nups2/nups2types.h"
#include "nups2/ps2ctl.h"
#include "edtools/edui.h"
#include "edtools/edmain.h"

extern void *app_fnt;

extern s32 edcrt_used;
extern struct numtl_s *edcrt_mtl_zoff;
extern f32 edcrt_cam_ax;
extern f32 edcrt_cam_ay;
extern struct nuvec_s edcrt_cam_pos;
extern struct mcdirent_s edcrt_mcdir[10];

static s32 edcrt_mctype;
static s32 edcrt_mcfree;
static s32 edcrt_mcformat;
static s32 edcrt_mcresult;

extern struct crate_s *highlighted_crate;
extern struct crate_s *highlight_crate;
extern struct crate_s *triggerorigin_crate;
extern struct crate_s *triggerdest_crate;
extern struct crate_s *locked_crate;
extern struct crate_s *lock_crate;
extern struct crate_s *marker_crate;
extern struct crate_s *flash_crate;
extern struct crateeditor_s crate_list[];
extern struct crategroup_s CrateGroup[];
extern struct cratecube_s Crate[];
extern s32 CRATEGROUPCOUNT;
extern s32 CRATECOUNT;

extern s32 screendump;
extern s32 PANELOFF;
extern s32 PLAYERCOUNT;
extern s32 VEHICLECONTROL;
extern struct nuvec_s *pos_START;
extern struct nuvec_s v000;
extern struct ldata_s *LDATA;
extern char LevelFileName[];
extern char tbuf[];

extern void *eduiMenuCreate(s32 x, s32 y, s32 w, s32 h, void *fnt, void *cancelcb,
                            char *title);
extern void eduiMenuDestroy(void *menu);
extern void eduiMenuDetach(void *menu);
extern s32 eduiMenuAttach(struct eduimenu_s *menu, struct eduimenu_s *child);
extern void *eduiMenuAddItem(void *menu, void *item);
extern s32 eduiMenuProcess(void *menu, void *arg);
extern void *eduiItemSelCreate(s32 value, void *style, s32 on, s32 group, void *cb,
                               char *label);
extern void *eduiItemToggleCreate(s32 value, void *style, s32 on, s32 group,
                                  void *cb, char *label);
extern void *eduiItemCheckCreate(u32 value, void *style, s32 checked, s32 group,
                                 void *cb, char *label);
extern void *eduiItemTextPickCreate(s32 data, void *style, void *cb, char *label);

extern void edcamMove(struct nupad_s *pad);
extern void edcamSet(void);
extern void edcamSetPos(struct nuvec_s *pos);
extern void edcamSetPosAng(struct nuvec_s *pos, s32 ax, s32 ay);
extern void edcamGetPosAng(struct nuvec_s *pos, f32 *ax, f32 *ay);

extern struct crate_s *FindLocalCrate(struct nuvec_s *pos);
extern struct crate_s *FindOverlap(struct crate_s *crate);
extern struct crate_s *AddCrate(s32 type, struct nuvec_s *pos);
extern struct crate_s *NextCrate(struct crate_s *crate);
extern void DestroyCrate(struct crate_s *crate);
extern void DrawCrate(struct crate_s *crate);
extern void ConvertCrateData(void);
extern void ResetCrates(void);
extern void ResetAllCrateTypes(s32 type);
extern void ResetAllCrates(void);
extern void ReadInCrateData(void);
extern void RestoreCrateTypeData(void);

extern void ResetTimeTrial(void);
#ifdef NW_HOST /* retail's prototype passes pos as an int before fALONG; the EE passes them in a2 and $f12 either way */
extern void ResetCheckpoint(s32 iRAIL, s32 iALONG, f32 fALONG, struct nuvec_s *pos);
#define ResetCheckpoint(a, b, c, d) ResetCheckpoint(a, b, d, (struct nuvec_s *)(c))
#else
extern void ResetCheckpoint(s32 a, s32 b, s32 c, f32 d);
#endif
extern void ResetBonus(void);
extern void ResetDeath(void);
extern void ResetGemPath(void);
extern void ResetWumpa(void);
extern void ResetAI(void);
extern void DrawCameraTarget(struct nuvec_s *pos);

extern void NuFntScale(s32 x, s32 y);
extern void NuFntSetPen(s32 colour);
extern void NuFntPrintEx(s32 x, s32 y, s32 colour, char *fmt, ...);

extern s32 NuFileOpen(char *name, s32 mode);
extern void NuFileClose(s32 h);
extern s32 NuFileWriteInt(s32 h, s32 v);
extern s32 NuFileWriteShort(s32 h, s16 v);
extern s32 NuFileWriteFloat(s32 h, f32 v);
extern s32 NuFileWriteChar(s32 h, char v);

extern s32 sceMcGetInfo(s32 port, s32 slot, s32 *type, s32 *free, s32 *format);
extern s32 sceMcFormat(s32 port, s32 slot);
extern s32 sceMcChdir(s32 port, s32 slot, char *path, char *pwd);
extern s32 sceMcMkdir(s32 port, s32 slot, char *name);
extern s32 sceMcGetDir(s32 port, s32 slot, char *name, u32 mode, s32 maxent,
                       struct mcdirent_s *table);
extern s32 sceMcOpen(s32 port, s32 slot, char *name, s32 mode);
extern s32 sceMcClose(s32 fd);
extern s32 sceMcDelete(s32 port, s32 slot, char *name);
extern s32 sceMcSync(s32 mode, s32 *cmd, s32 *result);
extern f32 NuMcReadFloat(s32 fd);
extern s32 NuMcReadInt(s32 fd);
extern s16 NuMcReadShort(s32 fd);
extern char NuMcReadChar(s32 fd);
extern s32 NuMcWriteFloat(s32 fd, f32 value, s32 async);
extern s32 NuMcWriteInt(s32 fd, s32 value, s32 async);
extern s32 NuMcWriteShort(s32 fd, s16 value, s32 async);
extern s32 NuMcWriteChar(s32 fd, char value, s32 async);

extern char *strupr(char *s);

void edcrtDrawSpecialCrates(void);
void edcrtDrawBoxyThing(struct nuvec_s *pos);
void WriteCrateData(void);
void edcrtcbMemCardSaveCrates(struct eduimenu_s *menu);
void edcrtcbMemCardMenu(struct eduimenu_s *menu);
void edcrtcbSpecificTypeMenu(struct eduimenu_s *menu, struct eduiitem_s *item);
void edcrtcbCrateTypesMenu(struct eduimenu_s *menu);
void edcrtcbDisplayTypeMenu(struct eduimenu_s *menu);
void edcrtDoInput(struct nupad_s *pad);
void edcrtDrawInfo(void);
/* The callbacks below are inline: GCC 2.95 then emits their code at the end
 * of the unit (in this declaration order, after edcrtDrawInfo, as retail has
 * it) but their literals where they are defined, which is where retail keeps
 * them. */
static void edcrtcbMemCardLoadCratesMenu(struct eduimenu_s *menu);
static void edcrtcbChangeFileNameMenu(struct eduimenu_s *menu);
static void edcrtcbMemCardDeleteCratesMenu(struct eduimenu_s *menu);
static void edcrtcbCancelMemCardMenu(void);
static void edcrtCbSaveCrateData(void *menu);
static void edcrtcbSetCrateType(struct eduimenu_s *menu, struct eduiitem_s *item);
static void edcrtcbCancelSpecificTypeMenu(void);
static void edcrtcbCancelCrateTypesMenu(void);
static void edcrtcbResetCratesMenu(struct eduimenu_s *menu);
static void edcrtcbDisplayType(struct eduimenu_s *menu, struct eduiitem_s *item);
static void edcrtcbCancelDisplayTypeMenu(void);
static void edcrtcbTerrainToggle(void *menu, struct eduiitem_s *item);
static void edcrtcbShowTriggersToggle(void *menu, struct eduiitem_s *item);
static void edcrtCbCancelOptMenu(void);
static void edcrtClose(void);
static void edcrtEnter(void);
static s32 edcrtProc(struct nupad_s *pad);
static void edcrtRender(void);
static void edcrtcbMemCardCancelLoadEffectsMenu(void);
static void edcrtcbChangeFileName(void *menu, struct eduiitem_s *item);
static void edcrtcbCancelChangeFileNameMenu(void);
static void edcrtcbMemCardDeleteCrates(struct eduimenu_s *menu, struct eduiitem_s *item);
static void edcrtcbMemCardCancelDeleteEffectsMenu(void);
static void edcrtcbResetCrates(struct eduimenu_s *menu, struct eduiitem_s *item);
static void edcrtcbCancelResetCratesMenu(void);
void edcrtInit(void);

static char edcrt_mcfilename[32] = "Save1";

struct eduimenu_s *edcrt_active_menu = 0;
struct eduimenu_s *edcrt_options_menu = 0;
struct eduimenu_s *edcrt_memcard_menu = 0;
struct eduimenu_s *edcrt_filename_menu = 0;
struct eduimenu_s *edcrt_loadeffects_menu = 0;
struct eduimenu_s *edcrt_deleteeffects_menu = 0;
struct eduimenu_s *edcrt_cratetypes_menu = 0;
struct eduimenu_s *edcrt_specifictype_menu = 0;
struct eduimenu_s *edcrt_resetcrates_menu = 0;
struct eduimenu_s *edcrt_displaytype_menu = 0;
static s32 edcrt_count = 0;
static s32 edcrt_terrainlock = 1;
static s32 edcrt_showalltriggers = 0;
s32 edcrtDrawType = 0;
s32 edcrt_currentselcrate = 0;

void edcrtDrawSpecialCrates(void)
{
    struct crate_s *crate;

    if (locked_crate != 0)
        DrawCrate(lock_crate);
    if (triggerorigin_crate != 0 && edcrt_count % 150 < 75) {
        lock_crate->pos = triggerorigin_crate->pos;
        lock_crate->orientation = triggerorigin_crate->orientation;
        DrawCrate(lock_crate);
    }
    if (triggerdest_crate != 0 && edcrt_count % 150 >= 75) {
        lock_crate->pos = triggerdest_crate->pos;
        lock_crate->orientation = triggerdest_crate->orientation;
        DrawCrate(lock_crate);
    }
    if (triggerorigin_crate != 0 && triggerdest_crate != 0) {
        struct nuvtx_tc1_s line[2];

        line[0].pnt.x = triggerorigin_crate->pos.x;
        line[0].pnt.y = triggerorigin_crate->pos.y;
        line[0].pnt.z = triggerorigin_crate->pos.z;
        line[0].diffuse = 0xFF0000FF;
        line[1].pnt.x = triggerdest_crate->pos.x;
        line[1].pnt.y = triggerdest_crate->pos.y;
        line[1].pnt.z = triggerdest_crate->pos.z;
        line[1].diffuse = 0xFF0000FF;
        NuRndrLine3d(line, edcrt_mtl_zoff, 0);
    }
    if (edcrt_showalltriggers) {
        for (crate = NextCrate(0); crate != 0; crate = NextCrate(crate)) {
            if (crate->trigger != 0 && crate != triggerorigin_crate) {
                struct nuvtx_tc1_s line[2];

                line[0].pnt.x = crate->pos.x;
                line[0].pnt.y = crate->pos.y;
                line[0].pnt.z = crate->pos.z;
                line[0].diffuse = 0xFF000080;
                line[1].pnt.x = crate->trigger->pos.x;
                line[1].pnt.y = crate->trigger->pos.y;
                line[1].pnt.z = crate->trigger->pos.z;
                line[1].diffuse = 0xFF000080;
                NuRndrLine3d(line, edcrt_mtl_zoff, 0);
            }
        }
    }
    if (highlighted_crate != 0) {
        highlight_crate->pos = highlighted_crate->pos;
        highlight_crate->orientation = highlighted_crate->orientation;
        DrawCrate(highlight_crate);
    } else {
        DrawCrate(marker_crate);
        if (FindOverlap(marker_crate) == 0 && triggerorigin_crate == 0 &&
            edcrt_count % 150 < 75)
            DrawCrate(flash_crate);
    }
}

void edcrtDrawBoxyThing(struct nuvec_s *pos)
{
    struct nuvec_s snap;
    struct nuvec_s v;
    f32 shadow;
    f32 y;
    s32 grid;

    if (locked_crate != 0) {
        v.x = pos->x - locked_crate->pos.x;
        v.y = pos->y - locked_crate->pos.y;
        v.z = pos->z - locked_crate->pos.z;
        NuVecRotateY(&v, &v, -locked_crate->orientation);
        v.x = v.x + 0.25;
        v.y = v.y + 0.25;
        v.z = v.z + 0.25;
        grid = v.x * 2.0;
        if (v.x < 0.0f)
            grid--;
        v.x = grid * 0.5;
        marker_crate->offx = grid;
        grid = v.y * 2.0;
        if (v.y < 0.0f)
            grid--;
        v.y = grid * 0.5;
        marker_crate->offy = grid;
        grid = v.z * 2.0;
        if (v.z < 0.0f)
            grid--;
        v.z = grid * 0.5;
        marker_crate->offz = grid;
        NuVecRotateY(&v, &v, locked_crate->orientation);
        snap.x = locked_crate->pos.x + v.x;
        snap.y = locked_crate->pos.y + v.y;
        snap.z = locked_crate->pos.z + v.z;
        marker_crate->pos.x = snap.x;
        marker_crate->pos.y = snap.y;
        marker_crate->pos.z = snap.z;
        flash_crate->pos.x = snap.x;
        flash_crate->pos.y = snap.y;
        flash_crate->pos.z = snap.z;
    } else {
        if (edcrt_terrainlock) {
            shadow = NewShadowMask(pos, 0.0f, -1);
            if (shadow == 2000000.0f)
                shadow = pos->y - 0.25;
            y = pos->y - shadow;
            grid = y * 2.0;
            if (y < 0.0f)
                grid--;
            y = shadow + grid * 0.5;
        } else {
            y = pos->y - 0.25;
        }
        snap.x = pos->x;
        snap.y = y;
        snap.z = pos->z;
        marker_crate->offx = 0;
        marker_crate->offy = 0;
        marker_crate->offz = 0;
        marker_crate->pos.x = snap.x;
        marker_crate->pos.y = snap.y + 0.25;
        marker_crate->pos.z = snap.z;
        flash_crate->pos.x = snap.x;
        flash_crate->pos.y = snap.y + 0.25;
        flash_crate->pos.z = snap.z;
    }
    edcrtDrawSpecialCrates();
}

void WriteCrateData(void)
{
    s32 handle;
    s32 i;
    s32 j;

    sprintf(tbuf, "%s.crt", LevelFileName);
    handle = NuFileOpen(tbuf, 1);
    if (handle == 0)
        return;
    NuFileWriteInt(handle, 4);
    NuFileWriteShort(handle, CRATEGROUPCOUNT);
    for (i = 0; i < CRATEGROUPCOUNT; i++) {
        NuFileWriteFloat(handle, CrateGroup[i].origin.x);
        NuFileWriteFloat(handle, CrateGroup[i].origin.y);
        NuFileWriteFloat(handle, CrateGroup[i].origin.z);
        NuFileWriteShort(handle, CrateGroup[i].iCrate);
        NuFileWriteShort(handle, CrateGroup[i].nCrates);
        NuFileWriteShort(handle, CrateGroup[i].angle);
        for (j = CrateGroup[i].iCrate;
             j < CrateGroup[i].iCrate + CrateGroup[i].nCrates; j++) {
            NuFileWriteFloat(handle, Crate[j].pos0.x);
            NuFileWriteFloat(handle, Crate[j].pos0.y);
            NuFileWriteFloat(handle, Crate[j].pos0.z);
            NuFileWriteFloat(handle, Crate[j].shadow);
            NuFileWriteShort(handle, Crate[j].dx);
            NuFileWriteShort(handle, Crate[j].dy);
            NuFileWriteShort(handle, Crate[j].dz);
            NuFileWriteChar(handle, Crate[j].type1);
            NuFileWriteChar(handle, Crate[j].type2);
            NuFileWriteChar(handle, Crate[j].type3);
            NuFileWriteChar(handle, Crate[j].type4);
            NuFileWriteShort(handle, Crate[j].iU);
            NuFileWriteShort(handle, Crate[j].iD);
            NuFileWriteShort(handle, Crate[j].iN);
            NuFileWriteShort(handle, Crate[j].iS);
            NuFileWriteShort(handle, Crate[j].iE);
            NuFileWriteShort(handle, Crate[j].iW);
            NuFileWriteShort(handle, Crate[j].trigger);
        }
    }
    NuFileClose(handle);
}

void edcrtcbMemCardSaveCrates(struct eduimenu_s *menu)
{
    char name[32];
    s32 fd;
    s32 i;
    s32 j;

    sceMcGetDir(0, 0, "*.crt", 0, 10, edcrt_mcdir);
    sceMcSync(0, 0, &edcrt_mcresult);
    if (edcrt_mcresult >= 8) {
        eduiCreateMessageMenu(menu, "Too many Crate Saves", 0);
        return;
    }
    sprintf(name, "%s.crt", edcrt_mcfilename);
    sceMcOpen(0, 0, name, 0x200);
    sceMcSync(0, 0, &edcrt_mcresult);
    fd = edcrt_mcresult;
    if (fd < 0) {
        eduiCreateMessageMenu(menu, "File Save Error", 0);
        return;
    }
    ConvertCrateData();
    NuMcWriteInt(fd, 4, 0);
    NuMcWriteShort(fd, CRATEGROUPCOUNT, 0);
    for (i = 0; i < CRATEGROUPCOUNT; i++) {
        NuMcWriteFloat(fd, CrateGroup[i].origin.x, 0);
        NuMcWriteFloat(fd, CrateGroup[i].origin.y, 0);
        NuMcWriteFloat(fd, CrateGroup[i].origin.z, 0);
        NuMcWriteShort(fd, CrateGroup[i].iCrate, 0);
        NuMcWriteShort(fd, CrateGroup[i].nCrates, 0);
        NuMcWriteShort(fd, CrateGroup[i].angle, 0);
        for (j = CrateGroup[i].iCrate;
             j < CrateGroup[i].iCrate + CrateGroup[i].nCrates; j++) {
            NuMcWriteFloat(fd, Crate[j].pos0.x, 0);
            NuMcWriteFloat(fd, Crate[j].pos0.y, 0);
            NuMcWriteFloat(fd, Crate[j].pos0.z, 0);
            NuMcWriteFloat(fd, Crate[j].shadow, 0);
            NuMcWriteShort(fd, Crate[j].dx, 0);
            NuMcWriteShort(fd, Crate[j].dy, 0);
            NuMcWriteShort(fd, Crate[j].dz, 0);
            NuMcWriteChar(fd, Crate[j].type1, 0);
            NuMcWriteChar(fd, Crate[j].type2, 0);
            NuMcWriteChar(fd, Crate[j].type3, 0);
            NuMcWriteChar(fd, Crate[j].type4, 0);
            NuMcWriteShort(fd, Crate[j].iU, 0);
            NuMcWriteShort(fd, Crate[j].iD, 0);
            NuMcWriteShort(fd, Crate[j].iN, 0);
            NuMcWriteShort(fd, Crate[j].iS, 0);
            NuMcWriteShort(fd, Crate[j].iE, 0);
            NuMcWriteShort(fd, Crate[j].iW, 0);
            NuMcWriteShort(fd, Crate[j].trigger, 0);
        }
    }
    sceMcClose(fd);
    sceMcSync(0, 0, 0);
    if (edcrt_mcresult < 0) {
        eduiCreateMessageMenu(menu, "File Save Error", 0);
        ResetCrates();
    } else {
        ResetCrates();
        eduiCreateMessageMenu(menu, "Saved OK", 1);
    }
}

void edcrtcbMemCardLoadCrates(struct eduimenu_s *menu, struct eduiitem_s *item)
{
    s32 fd;
    s32 version;
    s32 latest;
    s32 i;
    s32 j;

    sceMcOpen(0, 0, edcrt_mcdir[item->value].name, 1);
    sceMcSync(0, 0, &edcrt_mcresult);
    fd = edcrt_mcresult;
    if (fd < 0) {
        eduiCreateMessageMenu(menu, "File Load Error", 0);
        return;
    }
    ResetAllCrates();
    version = NuMcReadInt(fd);
    latest = 4;
    if (version > latest) {
        sceMcClose(fd);
        sceMcSync(0, 0, 0);
        eduiCreateMessageMenu(menu, "File Load Error", 0);
        return;
    }
    CRATEGROUPCOUNT = NuMcReadShort(fd);
    for (i = 0; i < CRATEGROUPCOUNT; i++) {
        CrateGroup[i].origin.x = NuMcReadFloat(fd);
        CrateGroup[i].origin.y = NuMcReadFloat(fd);
        CrateGroup[i].origin.z = NuMcReadFloat(fd);
        CrateGroup[i].iCrate = NuMcReadShort(fd);
        CrateGroup[i].nCrates = NuMcReadShort(fd);
        CrateGroup[i].angle = NuMcReadShort(fd);
        for (j = CrateGroup[i].iCrate;
             j < CrateGroup[i].iCrate + CrateGroup[i].nCrates; j++) {
            Crate[j].pos0.x = NuMcReadFloat(fd);
            Crate[j].pos0.y = NuMcReadFloat(fd);
            Crate[j].pos0.z = NuMcReadFloat(fd);
            Crate[j].shadow = NuMcReadFloat(fd);
            if (version == 1) {
                Crate[j].dx = NuMcReadChar(fd);
                Crate[j].dy = NuMcReadChar(fd);
                Crate[j].dz = NuMcReadChar(fd);
            } else {
                Crate[j].dx = NuMcReadShort(fd);
                Crate[j].dy = NuMcReadShort(fd);
                Crate[j].dz = NuMcReadShort(fd);
            }
            Crate[j].type1 = NuMcReadChar(fd);
            if (version >= 3) {
                Crate[j].type2 = NuMcReadChar(fd);
                Crate[j].type3 = NuMcReadChar(fd);
                Crate[j].type4 = NuMcReadChar(fd);
            } else {
                Crate[j].type2 = -1;
                Crate[j].type3 = -1;
                Crate[j].type4 = -1;
            }
            if (version == 1) {
                Crate[j].iU = NuMcReadChar(fd);
                Crate[j].iD = NuMcReadChar(fd);
                Crate[j].iN = NuMcReadChar(fd);
                Crate[j].iS = NuMcReadChar(fd);
                Crate[j].iE = NuMcReadChar(fd);
                Crate[j].iW = NuMcReadChar(fd);
            } else {
                Crate[j].iU = NuMcReadShort(fd);
                Crate[j].iD = NuMcReadShort(fd);
                Crate[j].iN = NuMcReadShort(fd);
                Crate[j].iS = NuMcReadShort(fd);
                Crate[j].iE = NuMcReadShort(fd);
                Crate[j].iW = NuMcReadShort(fd);
            }
            if (version >= 4)
                Crate[j].trigger = NuMcReadShort(fd);
            else
                Crate[j].trigger = -1;
        }
    }
    sceMcClose(fd);
    sceMcSync(0, 0, 0);
    if (edcrt_mcresult < 0) {
        eduiCreateMessageMenu(menu, "File Load Error", 0);
        return;
    }
    ReadInCrateData();
    ResetCrates();
    eduiCreateMessageMenu(menu, "Loaded OK", 1);
}

static inline void edcrtcbMemCardLoadCratesMenu(struct eduimenu_s *menu)
{
    struct eduistyle_s style = { { 0x80000000, 0x80FF0000 }, { 0x80808080, 0x80404040 } };
    s32 i;

    sceMcGetDir(0, 0, "*.crt", 0, 10, edcrt_mcdir);
    sceMcSync(0, 0, &edcrt_mcresult);
    if (edcrt_mcresult <= 0)
        return;
    edcrt_loadeffects_menu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt,
                                            edcrtcbMemCardCancelLoadEffectsMenu,
                                            "Load Crates");
    if (edcrt_loadeffects_menu == 0)
        return;
    for (i = 0; i < edcrt_mcresult; i++) {
        eduiMenuAddItem(edcrt_loadeffects_menu,
                        eduiItemSelCreate(i, &style, 0, 0, edcrtcbMemCardLoadCrates,
                                          edcrt_mcdir[i].name));
    }
    eduiMenuAttach(menu, edcrt_loadeffects_menu);
    edcrt_loadeffects_menu->x = menu->x + 10;
    edcrt_loadeffects_menu->y = menu->y + 40;
}

static inline void edcrtcbChangeFileNameMenu(struct eduimenu_s *menu)
{
    struct eduistyle_s style = { { 0x80000000, 0x80FF0000 }, { 0x80808080, 0x80404040 } };

    edcrt_filename_menu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt,
                                         edcrtcbCancelChangeFileNameMenu,
                                         "File Name");
    if (edcrt_filename_menu == 0)
        return;
    eduiMenuAddItem(edcrt_filename_menu,
                    eduiItemTextPickCreate(0, &style, edcrtcbChangeFileName,
                                           "File Name"));
    strcpy(edui_last_item->u.textpick.text, edcrt_mcfilename);
    eduiMenuAttach(menu, edcrt_filename_menu);
    edcrt_filename_menu->x = menu->x + 10;
    edcrt_filename_menu->y = menu->y + 40;
}

static inline void edcrtcbMemCardDeleteCrates(struct eduimenu_s *menu, struct eduiitem_s *item)
{
    struct eduimenu_s *parent = 0;
    void (*cancel)(struct eduimenu_s *menu, struct eduimenu_s *parent);

    sceMcDelete(0, 0, edcrt_mcdir[item->value].name);
    sceMcSync(0, 0, &edcrt_mcresult);
    if (menu->parent != 0) {
        parent = menu->parent;
        eduiMenuDetach(menu);
    }
    cancel = menu->cancel;
    if (cancel)
        cancel(menu, parent);
    if (edcrt_mcresult < 0)
        eduiCreateMessageMenu(parent, "File Delete Error", 0);
    else
        eduiCreateMessageMenu(parent, "File Delete OK", 1);
}

static inline void edcrtcbMemCardDeleteCratesMenu(struct eduimenu_s *menu)
{
    struct eduistyle_s style = { { 0x80000000, 0x80FF0000 }, { 0x80808080, 0x80404040 } };
    s32 i;

    sceMcGetDir(0, 0, "*.crt", 0, 10, edcrt_mcdir);
    sceMcSync(0, 0, &edcrt_mcresult);
    if (edcrt_mcresult <= 0)
        return;
    edcrt_deleteeffects_menu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt,
                                              edcrtcbMemCardCancelDeleteEffectsMenu,
                                              "Delete Save");
    if (edcrt_deleteeffects_menu == 0)
        return;
    for (i = 0; i < edcrt_mcresult; i++) {
        eduiMenuAddItem(edcrt_deleteeffects_menu,
                        eduiItemSelCreate(i, &style, 0, 0, edcrtcbMemCardDeleteCrates,
                                          edcrt_mcdir[i].name));
    }
    eduiMenuAttach(menu, edcrt_deleteeffects_menu);
    edcrt_deleteeffects_menu->x = menu->x + 10;
    edcrt_deleteeffects_menu->y = menu->y + 40;
}

void edcrtcbMemCardMenu(struct eduimenu_s *menu)
{
    struct eduistyle_s style = { { 0x80000000, 0x80FF0000 }, { 0x80808080, 0x80404040 } };

    sceMcGetInfo(0, 0, &edcrt_mctype, &edcrt_mcfree, &edcrt_mcformat);
    sceMcSync(0, 0, &edcrt_mcresult);
    if (edcrt_mcformat == 0) {
        sceMcFormat(0, 0);
        sceMcSync(0, 0, &edcrt_mcresult);
        sceMcGetInfo(0, 0, &edcrt_mctype, &edcrt_mcfree, &edcrt_mcformat);
        sceMcSync(0, 0, &edcrt_mcresult);
    }
    sceMcChdir(0, 0, "/", 0);
    sceMcSync(0, 0, &edcrt_mcresult);
    sceMcGetDir(0, 0, "CrateData", 0, 10, edcrt_mcdir);
    sceMcSync(0, 0, &edcrt_mcresult);
    if (edcrt_mcresult == 0) {
        sceMcMkdir(0, 0, "CrateData");
        sceMcSync(0, 0, &edcrt_mcresult);
    }
    sceMcChdir(0, 0, "/CrateData", 0);
    sceMcSync(0, 0, &edcrt_mcresult);
    sceMcGetDir(0, 0, "*", 0, 10, edcrt_mcdir);
    sceMcSync(0, 0, &edcrt_mcresult);
    edcrt_memcard_menu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt,
                                        edcrtcbCancelMemCardMenu, "Memory Card");
    if (edcrt_memcard_menu == 0)
        return;
    eduiMenuAddItem(edcrt_memcard_menu,
                    eduiItemSelCreate(1, &style, 0, 0, edcrtcbChangeFileNameMenu,
                                      "File Name..."));
    eduiMenuAddItem(edcrt_memcard_menu,
                    eduiItemSelCreate(1, &style, 0, 0, edcrtcbMemCardSaveCrates,
                                      "Save Crates"));
    eduiMenuAddItem(edcrt_memcard_menu,
                    eduiItemSelCreate(1, &style, 0, 0,
                                      edcrtcbMemCardLoadCratesMenu, "Load Crates..."));
    eduiMenuAddItem(edcrt_memcard_menu,
                    eduiItemSelCreate(1, &style, 0, 0,
                                      edcrtcbMemCardDeleteCratesMenu, "Delete Save..."));
    eduiMenuAttach(menu, edcrt_memcard_menu);
    edcrt_memcard_menu->x = menu->x + 10;
    edcrt_memcard_menu->y = menu->y + 40;
}

void edcrtcbSpecificTypeMenu(struct eduimenu_s *menu, struct eduiitem_s *item)
{
    struct eduistyle_s style = { { 0x80000000, 0x80FF0000 }, { 0x80808080, 0x80404040 } };
    char title[32];
    s32 i;

    sprintf(title, "Type %d", item->value);
    edcrt_specifictype_menu = eduiMenuCreate(0x46, 0x46, 0xFA, 0xFA, app_fnt,
                                             edcrtcbCancelSpecificTypeMenu, title);
    if (edcrt_specifictype_menu == 0)
        return;
    eduiMenuAddItem(edcrt_specifictype_menu,
                    eduiItemCheckCreate(-1, &style,
                                        highlighted_crate->type[item->value - 1] == -1,
                                        0, edcrtcbSetCrateType, "NULL"));
    for (i = 0; i < 20; i++) {
        eduiMenuAddItem(edcrt_specifictype_menu,
                        eduiItemCheckCreate(i, &style,
                                            highlighted_crate->type[item->value - 1] == i,
                                            0, edcrtcbSetCrateType, crate_list[i].name));
        if (highlighted_crate->type[item->value - 1] == i) {
            edcrt_specifictype_menu->current = edui_last_item;
        }
    }
    eduiMenuAttach(menu, edcrt_specifictype_menu);
    edcrt_specifictype_menu->x = menu->x + 10;
    edcrt_specifictype_menu->y = menu->y + 40;
}

void edcrtcbCrateTypesMenu(struct eduimenu_s *menu)
{
    struct eduistyle_s style = { { 0x80000000, 0x80FF0000 }, { 0x80808080, 0x80404040 } };

    if (highlighted_crate == 0)
        return;
    edcrt_cratetypes_menu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt,
                                           edcrtcbCancelCrateTypesMenu,
                                           "Crate Types");
    if (edcrt_cratetypes_menu == 0)
        return;
    eduiMenuAddItem(edcrt_cratetypes_menu,
                    eduiItemSelCreate(1, &style, 0, 0, edcrtcbSpecificTypeMenu,
                                      "Type 1..."));
    eduiMenuAddItem(edcrt_cratetypes_menu,
                    eduiItemSelCreate(2, &style, 0, 0, edcrtcbSpecificTypeMenu,
                                      "Type 2..."));
    eduiMenuAddItem(edcrt_cratetypes_menu,
                    eduiItemSelCreate(3, &style, 0, 0, edcrtcbSpecificTypeMenu,
                                      "Type 3..."));
    eduiMenuAddItem(edcrt_cratetypes_menu,
                    eduiItemSelCreate(4, &style, 0, 0, edcrtcbSpecificTypeMenu,
                                      "Type 4..."));
    eduiMenuAttach(menu, edcrt_cratetypes_menu);
    edcrt_cratetypes_menu->x = menu->x + 10;
    edcrt_cratetypes_menu->y = menu->y + 40;
}

static inline void edcrtcbResetCratesMenu(struct eduimenu_s *menu)
{
    struct eduistyle_s style = { { 0x80000000, 0x80FF0000 }, { 0x80808080, 0x80404040 } };

    edcrt_resetcrates_menu = eduiMenuCreate(0x46, 0x46, 0xFA, 0xFA, app_fnt,
                                            edcrtcbCancelResetCratesMenu,
                                            "Reset Crates");
    if (edcrt_resetcrates_menu == 0)
        return;
    eduiMenuAddItem(edcrt_resetcrates_menu,
                    eduiItemSelCreate(2, &style, 0, 0, edcrtcbResetCrates,
                                      "Set Type 2 all to Type 1"));
    eduiMenuAddItem(edcrt_resetcrates_menu,
                    eduiItemSelCreate(3, &style, 0, 0, edcrtcbResetCrates,
                                      "Set Type 3 all to NULL"));
    eduiMenuAddItem(edcrt_resetcrates_menu,
                    eduiItemSelCreate(4, &style, 0, 0, edcrtcbResetCrates,
                                      "Set Type 4 all to NULL"));
    eduiMenuAttach(menu, edcrt_resetcrates_menu);
    edcrt_resetcrates_menu->x = menu->x + 10;
    edcrt_resetcrates_menu->y = menu->y + 40;
}

void edcrtcbDisplayTypeMenu(struct eduimenu_s *menu)
{
    struct eduistyle_s style = { { 0x80000000, 0x80FF0000 }, { 0x80808080, 0x80404040 } };

    edcrt_displaytype_menu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt,
                                            edcrtcbCancelDisplayTypeMenu,
                                            "Display Type");
    if (edcrt_displaytype_menu == 0)
        return;
    eduiMenuAddItem(edcrt_displaytype_menu,
                    eduiItemCheckCreate(0, &style, edcrtDrawType == 0, 0,
                                        edcrtcbDisplayType, "Type 1"));
    if (edcrtDrawType == 0) {
        edcrt_displaytype_menu->current = edui_last_item;
    }
    eduiMenuAddItem(edcrt_displaytype_menu,
                    eduiItemCheckCreate(1, &style, edcrtDrawType == 1, 0,
                                        edcrtcbDisplayType, "Type 2"));
    if (edcrtDrawType == 1) {
        edcrt_displaytype_menu->current = edui_last_item;
    }
    eduiMenuAddItem(edcrt_displaytype_menu,
                    eduiItemCheckCreate(2, &style, edcrtDrawType == 2, 0,
                                        edcrtcbDisplayType, "Type 3"));
    if (edcrtDrawType == 2) {
        edcrt_displaytype_menu->current = edui_last_item;
    }
    eduiMenuAddItem(edcrt_displaytype_menu,
                    eduiItemCheckCreate(3, &style, edcrtDrawType == 3, 0,
                                        edcrtcbDisplayType, "Type 4"));
    if (edcrtDrawType == 3) {
        edcrt_displaytype_menu->current = edui_last_item;
    }
    eduiMenuAttach(menu, edcrt_displaytype_menu);
    edcrt_displaytype_menu->x = menu->x + 10;
    edcrt_displaytype_menu->y = menu->y + 40;
}

void edcrtInit(void)
{
    struct eduistyle_s style = { { 0x80000000, 0x80FF0000 }, { 0x80808080, 0x80404040 } };
    struct numtl_s *mtl;

    mtl = NuMtlCreate(1);
    edcrt_mtl_zoff = mtl;
    mtl->attrib.cull = 2;
    mtl->attrib.zmode = 3;
    mtl->diffuse.r = 0.5f;
    mtl->diffuse.g = 0.5f;
    mtl->diffuse.b = 0.5f;
    mtl->alpha = 1.0f;
    mtl->attrib.alpha = 0;
    NuMtlUpdate(mtl);
    edcrt_options_menu = eduiMenuCreate(0x46, 0x46, 0xB4, 0x12C, app_fnt,
                                        edcrtCbCancelOptMenu, "Options");
    if (edcrt_options_menu == 0)
        return;
    eduiMenuAddItem(edcrt_options_menu,
                    eduiItemSelCreate(1, &style, 0, 0, edcrtCbSaveCrateData,
                                      "Save Crate Data"));
    eduiMenuAddItem(edcrt_options_menu,
                    eduiItemSelCreate(1, &style, 0, 0, edcrtcbCrateTypesMenu,
                                      "Crate Types..."));
    eduiMenuAddItem(edcrt_options_menu,
                    eduiItemSelCreate(1, &style, 0, 0, edcrtcbResetCratesMenu,
                                      "Reset Crates..."));
    eduiMenuAddItem(edcrt_options_menu,
                    eduiItemSelCreate(1, &style, 0, 0, edcrtcbDisplayTypeMenu,
                                      "Display Type..."));
    eduiMenuAddItem(edcrt_options_menu,
                    eduiItemToggleCreate(0, &style, edcrt_terrainlock, 1,
                                         edcrtcbTerrainToggle, "Terrain Lock"));
    eduiMenuAddItem(edcrt_options_menu,
                    eduiItemToggleCreate(0, &style, edcrt_showalltriggers, 2,
                                         edcrtcbShowTriggersToggle, "Show Triggers"));
    eduiMenuAddItem(edcrt_options_menu,
                    eduiItemSelCreate(1, &style, 0, 0, edcrtcbMemCardMenu,
                                      "Mem Card..."));
}

s32 edcrt_used = 0;

void edcrtDoInput(struct nupad_s *pad)
{
    struct crate_s *added;
    struct crate_s *crate;
    struct crate_s *first;

    edcamMove(pad);
    edcamGetPosAng(&edcrt_cam_pos, &edcrt_cam_ax, &edcrt_cam_ay);
    if (VEHICLECONTROL == 2 ||
        (VEHICLECONTROL == 1 && player->obj.vehicle == 0x20)) {
        if (pos_START != 0) {
            edcrt_cam_pos.x = pos_START->x;
            edcamSetPos(&edcrt_cam_pos);
        }
    }
    if (locked_crate == 0) {
        marker_crate->orientation += pad->rdata.l_algpad_l;
        marker_crate->orientation -= pad->rdata.l_algpad_r;
        flash_crate->orientation += pad->rdata.l_algpad_l;
        flash_crate->orientation -= pad->rdata.l_algpad_r;
    }
    if (pad->paddata_db & 0x80)
        edcrt_active_menu = edcrt_options_menu;
    if (pad->paddata & 0x100) {
        if (pad->paddata_db & 0x20) {
            if (triggerorigin_crate != 0) {
                triggerorigin_crate = 0;
                triggerdest_crate = 0;
            } else {
                triggerorigin_crate = FindLocalCrate(&edcrt_cam_pos);
                if (triggerorigin_crate != 0)
                    triggerdest_crate = triggerorigin_crate->trigger;
            }
        }
    } else {
        if (pad->paddata_db & 0x40) {
            if (triggerorigin_crate != 0) {
                if (highlighted_crate != 0) {
                    triggerorigin_crate->trigger = highlighted_crate;
                    triggerdest_crate = highlighted_crate;
                } else {
                    triggerorigin_crate->trigger = 0;
                    triggerdest_crate = 0;
                }
                ConvertCrateData();
                ResetCrates();
            } else if (FindOverlap(marker_crate) == 0) {
                added = AddCrate(edcrt_currentselcrate, &marker_crate->pos);
                if (added != 0) {
                    added->orientation = marker_crate->orientation;
                    added->id = crate_list[edcrt_currentselcrate].id;
                    if (locked_crate != 0) {
                        added->linked = locked_crate;
                        added->offx = marker_crate->offx;
                        added->offy = marker_crate->offy;
                        added->offz = marker_crate->offz;
                    } else {
                        added->linked = added;
                        added->offx = 0;
                        added->offy = 0;
                        added->offz = 0;
                    }
                }
                ConvertCrateData();
                ResetCrates();
            }
        }
        if ((pad->paddata_db & 0x10) && highlighted_crate != 0) {
            for (crate = NextCrate(0); crate != 0; crate = NextCrate(crate)) {
                if (crate->trigger == highlighted_crate)
                    crate->trigger = 0;
            }
            if (highlighted_crate == triggerdest_crate)
                triggerdest_crate = 0;
            if (highlighted_crate == triggerorigin_crate) {
                triggerorigin_crate = 0;
                triggerdest_crate = 0;
            }
            if (highlighted_crate->linked == highlighted_crate) {
                highlighted_crate->linked = 0;
                first = 0;
                for (crate = NextCrate(0); crate != 0; crate = NextCrate(crate)) {
                    if (crate->linked == highlighted_crate) {
                        if (first == 0) {
                            first = crate;
                        } else {
                            crate->offx -= first->offx;
                            crate->offy -= first->offy;
                            crate->offz -= first->offz;
                        }
                        crate->linked = first;
                    }
                }
                if (first != 0) {
                    first->offz = 0;
                    first->offy = 0;
                    first->offx = 0;
                    if (locked_crate == highlighted_crate) {
                        locked_crate = first;
                        lock_crate->orientation = first->orientation;
                        lock_crate->pos = first->pos;
                    }
                } else {
                    locked_crate = 0;
                }
            }
            DestroyCrate(highlighted_crate);
            ConvertCrateData();
            ResetCrates();
        }
        if (pad->paddata_db & 0x20) {
            if (triggerorigin_crate != 0) {
                triggerorigin_crate = 0;
                triggerdest_crate = 0;
            } else if (locked_crate != 0) {
                locked_crate = 0;
            } else {
                locked_crate = FindLocalCrate(&edcrt_cam_pos);
                if (locked_crate != 0) {
                    if (locked_crate->linked != locked_crate)
                        locked_crate = locked_crate->linked;
                    lock_crate->orientation = locked_crate->orientation;
                    lock_crate->pos = locked_crate->pos;
                    marker_crate->orientation = locked_crate->orientation;
                    flash_crate->orientation = locked_crate->orientation;
                }
            }
        }
    }
    if (pad->paddata_db & 0x1000) {
        if (edcrt_currentselcrate == 20)
            edcrt_currentselcrate = 0;
        else
            edcrt_currentselcrate++;
        flash_crate->type[0] = edcrt_currentselcrate;
    }
    if (pad->paddata_db & 0x4000) {
        if (edcrt_currentselcrate == 0)
            edcrt_currentselcrate = 20;
        else
            edcrt_currentselcrate--;
        flash_crate->type[0] = edcrt_currentselcrate;
    }
}

void edcrtDrawInfo(void)
{
    if (PANELOFF)
        return;
    NuFntScale(8, 0xC);
    NuFntSetPen(-1);
    strcpy(tbuf, LDATA->path);
    strupr(tbuf);
    NuFntPrintEx(0x400, 0x100, 0, "%s.CRT", tbuf);
    NuFntPrintEx(0x400, 0x200, 0, "CRATEGROUPCOUNT:   %i/%i", CRATEGROUPCOUNT,
                 0x80);
    NuFntPrintEx(0x400, 0x300, 0, "CRATECOUNT:   %i/%i", CRATECOUNT, 0x100);
    NuFntPrintEx(0x400, 0x400, 0, "DISPLAYED:   TYPE %i", edcrtDrawType + 1);
    if (triggerorigin_crate) {
        NuFntPrintEx(0x400, 0x500, 0, "CURSOR:   LINK MODE   %s",
                     triggerdest_crate ? "SET" : "UNSET");
    } else {
        NuFntPrintEx(0x400, 0x500, 0, "CURSOR:   %s",
                     locked_crate ? "LOCKED" : "FREE");
    }
    NuFntPrintEx(0x400, 0xD00, 0, "CURSOR:   X: %.2f   Y: %.2f   Z: %.2f",
                 edcrt_cam_pos.x, edcrt_cam_pos.y, edcrt_cam_pos.z);
}

struct numtl_s *edcrt_mtl_zoff = 0;
f32 edcrt_cam_dist = 0.0f;
f32 edcrt_cam_ax = 0.0f;
f32 edcrt_cam_ay = 0.0f;

static inline void edcrtcbCancelMemCardMenu(void)
{
    eduiMenuDestroy(edcrt_memcard_menu);
    edcrt_memcard_menu = 0;
}

static inline void edcrtCbSaveCrateData(void *menu)
{
    ConvertCrateData();
    WriteCrateData();
    ResetCrates();
    eduiCreateMessageMenu(menu, "Saved OK", 1);
}

static inline void edcrtcbSetCrateType(struct eduimenu_s *menu, struct eduiitem_s *item)
{
    highlighted_crate->type[menu->parent->current->value - 1] = item->value;
    ConvertCrateData();
    ResetCrates();
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
    edcrt_specifictype_menu = 0;
}

static inline void edcrtcbCancelSpecificTypeMenu(void)
{
    eduiMenuDestroy(edcrt_specifictype_menu);
    edcrt_specifictype_menu = 0;
}

static inline void edcrtcbCancelCrateTypesMenu(void)
{
    eduiMenuDestroy(edcrt_cratetypes_menu);
    edcrt_cratetypes_menu = 0;
}

static inline void edcrtcbDisplayType(struct eduimenu_s *menu, struct eduiitem_s *item)
{
    edcrtDrawType = item->value;
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
    edcrt_specifictype_menu = 0;
}

static inline void edcrtcbCancelDisplayTypeMenu(void)
{
    eduiMenuDestroy(edcrt_displaytype_menu);
    edcrt_displaytype_menu = 0;
}

static inline void edcrtcbTerrainToggle(void *menu, struct eduiitem_s *item)
{
    edcrt_terrainlock = item->on;
}

static inline void edcrtcbShowTriggersToggle(void *menu, struct eduiitem_s *item)
{
    edcrt_showalltriggers = item->on;
}

static inline void edcrtCbCancelOptMenu(void)
{
    edcrt_active_menu = 0;
}

static inline void edcrtClose(void)
{
    eduiMenuDestroy(edcrt_options_menu);
}

static inline void edcrtEnter(void)
{
    ResetTimeTrial();
    ResetCheckpoint(-1, -1, 0, 0.0f);
    ResetBonus();
    ResetDeath();
    ResetGemPath();
    RestoreCrateTypeData();
    ResetCrates();
    ResetWumpa();
    ResetAI();
    ConvertCrateData();
    ResetCrates();
    if (!edcrt_used) {
        struct nuvec_s *pos;

        if (PLAYERCOUNT != 0)
            pos = &player->obj.pos;
        else
            pos = &v000;
        edcamSetPosAng(pos, 0, 0);
        edcrt_used = 1;
    }
}

static inline s32 edcrtProc(struct nupad_s *pad)
{
    if (screendump)
        return 0;
    edcrt_count += NW_HZ(6, 5);
    if (edcrt_active_menu) {
        eduiMenuProcess(edcrt_active_menu, pad);
        return 0;
    }
    edcrtDoInput(pad);
    {
        s32 f = pad->paddata_db & 0x800;
        return f != 0;
    }
}

static inline void edcrtRender(void)
{
    edcrtDrawInfo();
    edcamSet();
    highlighted_crate = FindLocalCrate(&edcrt_cam_pos);
    DrawCameraTarget(&edcrt_cam_pos);
    edcrtDrawBoxyThing(&edcrt_cam_pos);
    if (edcrt_active_menu) {
        NuFntScale(0xC, 0xC);
        eduiMenuRender(edcrt_active_menu);
    }
}

static inline void edcrtcbMemCardCancelLoadEffectsMenu(void)
{
    eduiMenuDestroy(edcrt_loadeffects_menu);
    edcrt_loadeffects_menu = 0;
}

static inline void edcrtcbChangeFileName(void *menu, struct eduiitem_s *item)
{
    strcpy(edcrt_mcfilename, item->u.textpick.text);
}

static inline void edcrtcbCancelChangeFileNameMenu(void)
{
    eduiMenuDestroy(edcrt_filename_menu);
    edcrt_filename_menu = 0;
}

static inline void edcrtcbMemCardCancelDeleteEffectsMenu(void)
{
    eduiMenuDestroy(edcrt_deleteeffects_menu);
    edcrt_deleteeffects_menu = 0;
}

static inline void edcrtcbResetCrates(struct eduimenu_s *menu, struct eduiitem_s *item)
{
    s32 saved;

    ResetAllCrateTypes(item->value - 1);
    saved = CRATEGROUPCOUNT;
    ResetAllCrates();
    CRATEGROUPCOUNT = saved;
    ReadInCrateData();
    ResetCrates();
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
    edcrt_specifictype_menu = 0;
}

static inline void edcrtcbCancelResetCratesMenu(void)
{
    eduiMenuDestroy(edcrt_resetcrates_menu);
    edcrt_resetcrates_menu = 0;
}

struct edmainreg_s edcrtdesc = {
    0, 0, "Crate Editor", edcrtInit, edcrtClose, edcrtEnter, 0, 0, 0, 0,
    EDMAIN_BLOCKID('c', 'r', 'a', 't'), edcrtProc, edcrtRender
};
struct nuvec_s edcrt_cam_pos = { 0.0f, 0.0f, 0.0f };
/* sceMcGetDir needs a 64-byte aligned table */
struct mcdirent_s edcrt_mcdir[10] __attribute__((aligned(64))) = { { { 0 } } };
