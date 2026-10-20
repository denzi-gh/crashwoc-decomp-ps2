/*
 * Unit: gamelib/edptl
 *
 * Functions:
 *   0x0018a058 edppPtlCreate
 *   0x0018a280 edppPtlPlace
 *   0x0018a380 edppDoInput
 *   0x0018a7a8 edppDrawSpheres
 *   0x0018a9a0 edppDrawCursor
 *   0x0018b1a8 edppDetermineNearest
 *   0x0018b2f0 cbPtlApplyGrad
 *   0x0018b530 TidyAllEffects
 *   0x0018b808 cbPtlDeleteEffect
 *   0x0018b9f8 cbPtlAddEffect
 *   0x0018bba0 cbPtlCopyEffect
 *   0x0018bd58 cbPtlChangeY
 *   0x0018bf38 cbPtlChangeZ
 *   0x0018c118 UpdateTotalPtls
 *   0x0018c2a8 cbChangeGenRateMenu
 *   0x0018c410 cbPtlCopySize
 *   0x0018c618 cbPtlApplySize
 *   0x0018c7f0 cbPtlApplyRot
 *   0x0018c950 cbPtlSelGSort
 *   0x0018ca60 cbEffectListMenu
 *   0x0018cba8 edptlcbSwitchMenu
 *   0x0018cd40 edptlcbGroupMenu
 *   0x0018ce80 edptlcbDpadModeMenu
 *   0x0018d000 cbPtlTypeMenu
 *   0x0018d188 cbPtlColMenu
 *   0x0018d4b8 cbPtlJibMenu
 *   0x0018d6e0 edptlcbBounceMenu
 *   0x0018d840 edptlcbSoundIDMenu
 *   0x0018da58 edptlcbSoundControlMenu
 *   0x0018dd28 edptlcbSoundXMenu
 *   0x0018def8 cbPtlCollMenu
 *   0x0018e148 cbPtlSizeMenu
 *   0x0018e540 cbPtlRotMenu
 *   0x0018e800 cbPtlCutOffMenu
 *   0x0018ea28 cbPtlVarEmitMenu
 *   0x0018eef8 cbPtlVarStartMenu
 *   0x0018f2e8 cbPtlTextureSelectMenu
 *   0x0018f480 cbPtlTextureMenu
 *   0x0018f658 cbPtlEmitTimeMenu
 *   0x0018f8e0 cbPtlEmitVelMenu
 *   0x0018fa48 cbPtlGravMenu
 *   0x0018fbe0 edppSaveEffects
 *   0x00190298 cbFileSaveEffects
 *   0x00190640 FileLoadSingleEffectType
 *   0x00190a00 edppLoadEffects
 *   0x00190fd0 edppRestartAllEffectsInLevel
 *   0x00191118 edppMergeEffects
 *   0x001913e8 cbFileLoadEffects
 *   0x00191718 edppMemCardSaveEffects
 *   0x00191dd8 cbMemCardSaveEffects
 *   0x00191ff0 cbMemCardSaveEffectTypes
 *   0x00192208 MemCardLoadSingleEffectType
 *   0x00192588 edppMemCardLoadEffects
 *   0x00192950 cbMemCardLoadEffects
 *   0x00192b60 edppMemCardMergeEffects
 *   0x00192df8 cbMemCardMergeEffects
 *   0x00192f78 cbMemCardDeleteEffects
 *   0x00193120 cbMemCardMenu
 *   0x00193550 edptlcbClipboardMenu
 *   0x00193770 cbPtlDataMenu
 *   0x001939e8 cbPtlEmitMenu
 *   0x00193cd0 cbPtlGSortMenu
 *   0x00193f10 edppInit
 *   0x001945f0 ParticleReset
 *   0x00194620 edppDestroyAllParticles
 *   0x00194688 edppDestroyAllEffects
 *   0x001946b8 edppRegisterLevel
 *   0x00194708 edppRegisterPointerToGameCharLocation
 *   0x00194728 edppPtlDestroy
 *   0x00194780 edppPtlChangeType
 *   0x00194848 edppWrite
 *   0x00194910 edppRead
 *   0x00194998 edppProc
 *   0x001949f8 edppHighlightNearest
 *   0x00194a70 edppRender
 *   0x00194b08 cbPtlChangeGrav
 *   0x00194ba0 cbChangeNameMenu
 *   0x00194ca8 edptlcbEmptyClipboard
 *   0x00194ce0 edptlcbPasteClipboard
 *   0x00194d60 edptlcbCutClipboard
 *   0x00194dd8 edptlcbApplyBounceOffset
 *   0x00194e38 edptlcbApplyBounceFactor
 *   0x00194e98 cbPtlChangeEmitVel
 *   0x00194f00 cbPtlChangeX
 *   0x00194ff8 cbPtlChangeCutOff
 *   0x00195078 cbPtlChangeCutOn
 *   0x001950f8 cbPtlChangeDrawCutOff
 *   0x00195160 cbPtlChangeGenRate
 *   0x001951e0 cbCancelChangeGenRateMenu
 *   0x00195208 cbChangeETimeMenu
 *   0x00195348 cbPtlApplyJib
 *   0x00195408 cbPtlSelType
 *   0x00195450 cbPtlSelGCode
 *   0x00195518 cbCancelMessageMenu
 *   0x00195540 cbSelEffectList
 *   0x00195610 cbCancelEffectListMenu
 *   0x00195638 edptlcbSetSwitchId
 *   0x001956a0 edptlcbSetSwitchVar
 *   0x00195700 edptlcbSwitchTypeMenu
 *   0x00195850 edptlcbCancelSwitchMenu
 *   0x00195878 edptlcbCancelGroupMenu
 *   0x001958a0 edptlcbSetMasterGroup
 *   0x001958b8 edptlcbSetGroup
 *   0x00195930 edptlcbSetDpadMode
 *   0x00195940 edptlcbCancelDpadModeMenu
 *   0x00195968 edptlcbJumpToGameLocation
 *   0x00195998 cbPtlCancelTypeMenu
 *   0x001959c0 cbPtlCancelColMenu
 *   0x001959f0 cbPtlCancelJibMenu
 *   0x00195a28 edptlcbCancelBounceMenu
 *   0x00195a50 cbChangeNumCollSpheres
 *   0x00195ac0 cbPtlDefaultCollEnv
 *   0x00195b98 cbPtlApplyCollEnv
 *   0x00195c80 edptlcbSetSoundID
 *   0x00195d38 edptlcbSetSoundControl
 *   0x00195de8 edptlcbChangeSoundDelay
 *   0x00195e68 edptlcbCancelSoundIDMenu
 *   0x00195e90 edptlcbCancelSoundControlMenu
 *   0x00195eb8 edptlcbCancelSoundXMenu
 *   0x00195ee0 edptlcbSoundsMenu
 *   0x00196030 cbPtlCancelCollMenu
 *   0x00196058 cbPtlCancelSizeMenu
 *   0x00196090 cbPtlCancelRotMenu
 *   0x001960c0 cbPtlCancelCutOffMenu
 *   0x001960e8 cbPtlCancelVarEmitMenu
 *   0x00196110 cbPtlCancelVarStartMenu
 *   0x00196138 cbPtlSScaleMenu
 *   0x00196218 cbPtlChangeTextureSelect
 *   0x00196328 cbPtlSelTextureType
 *   0x00196390 cbPtlCancelTextureMenu
 *   0x001963c8 cbPtlChangeIvalOn
 *   0x00196448 cbPtlChangeIvalOnRan
 *   0x001964c8 cbPtlChangeIvalOff
 *   0x00196548 cbPtlChangeIvalOffRan
 *   0x001965c8 cbPtlCancelEmitTimeMenu
 *   0x001965f0 cbPtlCancelEmitVelMenu
 *   0x00196618 cbPtlCancelGravMenu
 *   0x00196640 cbMemCardEraseConfirm
 *   0x00196750 cbChangeFileNameMenu
 *   0x00196838 cbMemCardLoadEffectsMenu
 *   0x00196988 cbMemCardMergeEffectsMenu
 *   0x00196ad8 cbMemCardDeleteEffectsMenu
 *   0x00196c28 cbCancelMemCardMenu
 *   0x00196c50 edptlcbCancelClipboardMenu
 *   0x00196c78 cbPtlCancelDataMenu
 *   0x00196ca0 cbPtlCancelEmitMenu
 *   0x00196cc8 cbPtlCancelGSortMenu
 *   0x00196cf0 cbPtlSnapToggle
 *   0x00196d00 edptlcbResetParticles
 *   0x00196d70 cbPtlCancel
 *   0x00196d78 edppClose
 *   0x00196da0 edppEnter
 *   0x00196e78 edppApply
 *   0x00196e80 cbChangeName
 *   0x00196ec8 cbCancelChangeNameMenu
 *   0x00196f30 cbPtlChangeETime
 *   0x00196fa8 cbCancelChangeETimeMenu
 *   0x00196fd0 cbConfirmMenuNo
 *   0x00197020 cbCancelConfirmMenu
 *   0x00197048 edptlcbSetSwitchType
 *   0x001970e0 edptlcbCancelSwitchTypeMenu
 *   0x00197108 edptlcbCancelSoundsMenu
 *   0x00197130 cbPtlChangeSScale
 *   0x00197148 cbPtlCancelSScaleMenu
 *   0x00197170 cbMemCardErase
 *   0x00197270 cbChangeFileName
 *   0x00197298 cbCancelChangeFileNameMenu
 *   0x001972c0 cbMemCardCancelLoadEffectsMenu
 *   0x001972e8 cbMemCardCancelMergeEffectsMenu
 *   0x00197310 cbMemCardCancelDeleteEffectsMenu
 *   0x00197338 CreateMessageMenu
 */

#include "nu_libc.h"

#include "numath/numathtypes.h"
#include "numath/nuvec.h"
#include "nu3d/numtl.h"
#include "nu3d/nugobj.h"
#include "nu3d/nurndr.h"
#include "nu3d/nucamera.h"
#include "gamelib/terrain.h"
#include "nups2/nups2types.h"
#include "nups2/ps2ctl.h"
#include "gamelib/debris.h"
#include "edtools/edui.h"
#include "edtools/edmain.h"

struct edppptl_s {
    struct nuvec_s pos;
    int type;
    int debris;
    short rotz;
    short roty;
    short emitrotz;
    short emitroty;
    int offset;
    char name[0x10];
    int switchtype;
    int switchid;
    float switchvar;
    short refrotz;
    short refroty;
    float bounceoffset;
    float bouncefactor;
    short group;
    char pad4A[0x2];
};

static char mcfilename[32] = "Save1";
static char levelfilename[256] = {0};

int edpp_create_type = -1;
int edpp_dpad_mode = 0;
struct eduimenu_s *edptl_switch_menu = 0;
struct eduimenu_s *edptl_switchtype_menu = 0;
struct eduimenu_s *edptl_sounds_menu = 0;
struct eduimenu_s *edptl_soundx_menu = 0;
struct eduimenu_s *edptl_soundid_menu = 0;
struct eduimenu_s *edptl_soundcontrol_menu = 0;
struct eduimenu_s *edptl_bounce_menu = 0;
struct eduimenu_s *edptl_group_menu = 0;
int edpp_emitrotz = 0;
int edpp_emitroty = 0;
int edpp_rotz = 0;
int edpp_roty = 0;
int edpp_offset = 0;
int edpp_refrotz = 0;
int edpp_refroty = 0;
static int superscale = 1;
static int edptl_count = 0;
int edptl_clipboard_entry = -1;
static void *grad_item = 0;
static void *grad_alpha_item = 0;
static struct eduiitem_s *grad_size_w_item = 0;
static struct eduiitem_s *grad_size_h_item = 0;
static struct eduiitem_s *grad_size_min_item = 0;
static struct eduiitem_s *grad_size_max_item = 0;
static struct eduiitem_s *grad_rot_item = 0;
static struct eduiitem_s *grad_rot_min_item = 0;
static struct eduiitem_s *grad_rot_max_item = 0;
static struct eduiitem_s *grad_jib_x_freq_item = 0;
static struct eduiitem_s *grad_jib_x_amp_item = 0;
static struct eduiitem_s *grad_jib_y_freq_item = 0;
static struct eduiitem_s *grad_jib_y_amp_item = 0;
static void *coll_env_item = 0;
static signed char levelfilenametype = 0;

static int mctype;
static int mcfree;
static int mcformat;
static int mcresult;
static struct nuvec_s entry_position;

extern struct edppptl_s edpp_ptls[256];
extern struct debinfo_s effecttypes[128];

extern int edpp_nextalloc;
extern int edpp_nearest;
extern int debris_render_group;
extern int effect_types_used;

extern int edpp_snap_enabled;
extern void *edpp_active_menu;
extern struct eduimenu_s *changegenratemenu;
extern struct eduimenu_s *messagemenu;
extern struct eduimenu_s *effectlistmenu;
extern struct eduimenu_s *dpadmodemenu;
extern struct eduimenu_s *ptltypemenu;
extern struct eduimenu_s *collmenu;
extern struct eduimenu_s *ptlcutoffmenu;
extern struct eduimenu_s *ptlvaremitmenu;
extern struct eduimenu_s *ptlvarstartmenu;
extern struct eduimenu_s *emittimemenu;
extern struct eduimenu_s *ptlemitvelmenu;
extern struct eduimenu_s *ptlgravmenu;
extern struct eduimenu_s *memcardmenu;
extern struct eduimenu_s *ptlclipmenu;
extern struct eduimenu_s *ptldatamenu;
extern struct eduimenu_s *ptlemitmenu;
extern struct eduimenu_s *ptlgsortmenu;
extern void *ptloptmenu;
extern void *ptlgcodemenu;
extern struct eduimenu_s *etimemenu;
extern struct eduimenu_s *confirmmenu;
extern struct eduimenu_s *sscalemenu;
extern struct eduimenu_s *filenamemenu;
extern struct eduimenu_s *loadeffectsmenu;
extern struct eduimenu_s *mergeeffectsmenu;
extern struct eduimenu_s *deleteeffectsmenu;
extern struct eduimenu_s *ptlcolmenu;
extern struct eduimenu_s *ptlrotmenu;

extern void edmainRegisterLocVec(struct nuvec_s *loc);
extern void eduiMenuDetach(void *menu);
extern void eduiMenuDestroy(void *menu);
extern void edcamSetPosAng(void *pos, void *ang, int flags);
extern void cbPtlDeleteEffect(struct eduimenu_s *menu);

extern struct eduimenu_s *ptljibmenu;
extern struct eduimenu_s *ptlsizemenu;
extern struct eduimenu_s *texturemenu;
extern struct eduimenu_s *textureselectmenu;
extern void DebReAlloc(struct debkeydatatype_s *deb, int count);
extern void eduiGradStageDelete(struct eduiitem_s *menu, void *stage);
extern void cbCancelChangeGenRateMenu(void);
extern void cbPtlChangeGenRate(void *menu, struct eduiitem_s *item);

extern void NuStrCpy(char *dst, char *src);
extern void DebFreeInstantly(int *key);
extern void AddDebrisEffect(int *key, int type, float x, float y, float z);
extern void edppDoInput(struct nupad_s *pad);
extern int eduiMenuProcess(struct eduimenu_s *menu, float dt, struct nupad_s *pad);
extern void UpdateTotalPtls(struct debinfo_s *dt);
extern void edbitsDrawCube(float x, float y, float z, float sizex, float sizey, float sizez,
                           int rotx, int roty, int rotz, int flags, int colour, void *mtl);

extern void *edpp_mtl;
extern signed char edpp_effect_list;

extern struct eduimenu_s *namemenu;

extern int NuFileReadInt(void *file);
extern float NuFileReadFloat(void *file);
extern void NuFileWriteInt(void *file, int value);
extern void NuFileWriteFloat(void *file, float value);
extern int edppPtlCreate(struct nuvec_s *pos, int type);
extern void edppRestartAllEffectsInLevel(void);
extern void edcamSet(void);
extern void edppDrawCursor(void);
extern void edppClose(void);
extern void edppEnter(void);
extern void edppApply(void);
extern int edppWrite(void *file);
extern int edppRead(void *file);
extern int edppProc(float dt, struct nupad_s *pad);
extern void edppRender(void);
extern void GenericDebinfoDmaTypeUpdate(struct debinfo_s *dt);
static void CreateMessageMenu(struct eduimenu_s *parent, char *message, int flags);

extern int edpp_first_time_this_level;
extern void *gencodetab[];
extern void *gensorttab[];
extern struct nucamera_s global_camera;
extern void *app_fnt;

extern void DebrisEmiterPos(int key, float x, float y, float z);
extern void DebrisOrientation(int key, short rotz, short roty);
extern void DebrisEmitterOrientation(int key, short rotz, short roty);
extern void DebrisReflectionOrientation(int key, float offset, float factor, short rotz,
                                        short roty);
extern struct nuvec_s *edmainQueryLocVec(void);
extern struct eduimenu_s *eduiMenuCreate(int x, int y, int w, int h, void *font, void *cancelcb,
                                          char *title);
extern void *eduiMenuAddItem(struct eduimenu_s *menu, void *item);
extern int eduiMenuAttach(struct eduimenu_s *parent, struct eduimenu_s *child);
extern void *eduiItemSelCreate(int type, struct eduistyle_s *font, int value, int group, void *cb,
                               char *label);
extern void *eduiItemSliderCreateInt(int id, struct eduistyle_s *font, int group, void *cb,
                                     int min, int max, int value, char *label);
extern void *eduiItemCheckCreate(int value, struct eduistyle_s *font, int checked, int group,
                                 void *cb, char *label);
extern void *eduiItemSliderCreate(void *data, struct eduistyle_s *font, int group, void *cb,
                                  float min, float max, float value, char *label);
extern void *eduiItemTextPickCreate(void *data, struct eduistyle_s *font, void *cb, char *label);
extern void cbPtlCancelSScaleMenu(void);
extern void cbPtlChangeSScale(void *menu, struct eduiitem_s *item);
extern void cbCancelChangeFileNameMenu(void);
extern void cbCancelEffectListMenu(void);
extern void cbSelEffectList(struct eduimenu_s *menu, struct eduiitem_s *item);
extern void edptlcbCancelGroupMenu(void);
extern void edptlcbSetMasterGroup(void *menu, struct eduiitem_s *item);
extern void edptlcbSetGroup(void *menu, struct eduiitem_s *item);
extern void edptlcbCancelDpadModeMenu(void);
extern void edptlcbSetDpadMode(void *menu, struct eduiitem_s *item);
extern void edptlcbCancelBounceMenu(void);
extern void edptlcbApplyBounceOffset(void *menu, struct eduiitem_s *item);
extern void edptlcbApplyBounceFactor(void *menu, struct eduiitem_s *item);
extern int sceMcDelete(int port, int slot, char *name);
extern int sceMcSync(int mode, int *cmd, int *result);

extern void cbChangeFileName(void *menu, struct eduiitem_s *item);
extern void cbPtlCancelDataMenu(void);
static void cbChangeNameMenu(struct eduimenu_s *menu);
extern void cbFileSaveEffects(struct eduimenu_s *menu);
extern void cbFileLoadEffects(struct eduimenu_s *menu);
extern void cbMemCardMenu(struct eduimenu_s *menu);
extern void edptlcbClipboardMenu(struct eduimenu_s *menu);

extern void cbPtlCancelEmitMenu(void);
extern void cbPtlEmitVelMenu(struct eduimenu_s *menu);
extern void cbPtlGravMenu(struct eduimenu_s *menu);
extern void cbPtlVarStartMenu(struct eduimenu_s *menu);
extern void cbPtlVarEmitMenu(struct eduimenu_s *menu);
extern void cbChangeGenRateMenu(struct eduimenu_s *menu);
static void cbChangeETimeMenu(struct eduimenu_s *menu);
extern void cbPtlEmitTimeMenu(struct eduimenu_s *menu);
extern void cbPtlCutOffMenu(struct eduimenu_s *menu);

extern void cbCancelConfirmMenu(void);
extern void cbConfirmMenuNo(struct eduimenu_s *menu);
static void cbMemCardErase(struct eduimenu_s *menu);

extern void DebrisStartOffset(int key, int offset);
extern void DebrisSetGroupID(int key, short groupid);
extern struct nuvec_s edpp_cam_pos;
extern float edpp_cam_ax;
extern float edpp_cam_ay;
extern void edppDetermineNearest(float range);
extern void edcamMove(struct nupad_s *pad);
extern void edcamSetPos(struct nuvec_s *pos);
extern void edcamGetPosAng(struct nuvec_s *pos, float *ax, float *ay);
extern void edcamGetPosAngSnap(struct nuvec_s *pos, float *ax, float *ay);
extern void *edpp_boxmtl;
extern void NuRndrRect2di(int x, int y, int w, int h, int colour, void *mtl);
extern void NuRndrLine2di(int x0, int y0, int x1, int y1, int colour, void *mtl);
extern void NuFntSet(void *fnt);
extern void NuFntSetPen(int colour);
extern void NuFntPrintEx(int x, int y, int flags, char *fmt, ...);

extern int edSfxAllCount;
extern char *edbitsGetSoundName(int id);
extern void *eduiItemGradPickCreate(void *data, struct eduistyle_s *font, void *cb, char *label);
extern void *eduiItemGreyGradPickCreate(void *data, struct eduistyle_s *font, void *cb,
                                        char *label);
extern void cbPtlApplyGrad(void);
extern void cbPtlAddEffect(struct eduimenu_s *menu);
extern void edptlcbCancelSwitchMenu(void);
static void edptlcbSwitchTypeMenu(struct eduimenu_s *menu);
extern void edptlcbSetSwitchId(void *menu, struct eduiitem_s *item);
extern void edptlcbSetSwitchVar(void *menu, struct eduiitem_s *item);
extern void cbPtlCancelTypeMenu(void);
extern void cbPtlSelType(void *menu, struct eduiitem_s *item);
extern void cbPtlCancelColMenu(void);
extern void cbPtlCancelJibMenu(void);
extern void cbPtlApplyJib(void *menu, struct eduiitem_s *item);
extern void edptlcbCancelSoundIDMenu(void);
extern void edptlcbSetSoundID(void *menu, struct eduiitem_s *item);
extern void edptlcbCancelSoundControlMenu(void);
extern void edptlcbSetSoundControl(void *menu, struct eduiitem_s *item);
extern void edptlcbCancelSoundXMenu(void);
extern void edptlcbChangeSoundDelay(void *menu, struct eduiitem_s *item);
extern void edptlcbCancelSwitchTypeMenu(void);
extern void edptlcbSetSwitchType(void *menu, struct eduiitem_s *item);
extern void edptlcbCancelSoundsMenu(void);
extern void cbMemCardCancelLoadEffectsMenu(void);
extern void cbMemCardCancelMergeEffectsMenu(void);
extern void cbMemCardMergeEffects(struct eduimenu_s *menu, struct eduiitem_s *item);
extern void cbMemCardCancelDeleteEffectsMenu(void);
extern void cbMemCardDeleteEffects(struct eduimenu_s *menu, struct eduiitem_s *item);

extern void cbChangeName(void *menu, struct eduiitem_s *item);
static void cbCancelChangeNameMenu(void *menu);
extern void cbPtlChangeETime(void *menu, struct eduiitem_s *item);
extern void cbCancelChangeETimeMenu(void);

extern void edptlcbCancelClipboardMenu(void);
extern void edptlcbEmptyClipboard(struct eduimenu_s *menu);
extern void edptlcbPasteClipboard(struct eduimenu_s *menu);
extern void edptlcbCutClipboard(struct eduimenu_s *menu);

extern void cbPtlCancelGSortMenu(void);

extern int sceMcGetInfo(int port, int slot, int *type, int *free, int *format);
extern int sceMcFormat(int port, int slot);
extern int sceMcChdir(int port, int slot, char *newdir, char *curdir);
extern int sceMcMkdir(int port, int slot, char *name);
extern void cbCancelMemCardMenu(void);
static void cbMemCardDeleteEffectsMenu(struct eduimenu_s *menu);
static void cbMemCardEraseConfirm(struct eduimenu_s *menu);
static void cbChangeFileNameMenu(struct eduimenu_s *menu);
static void cbMemCardLoadEffectsMenu(struct eduimenu_s *menu);
static void cbMemCardMergeEffectsMenu(struct eduimenu_s *menu);

extern void *eduiItemToggleCreate(int value, struct eduistyle_s *font, int on, int group, void *cb,
                                  char *label);
extern int edpp_curr;
static void edptlcbSoundsMenu(struct eduimenu_s *menu);
static void cbPtlSScaleMenu(struct eduimenu_s *menu);
extern void edptlcbResetParticles(void);
extern void cbPtlSnapToggle(void *menu, struct eduiitem_s *item);
extern void cbPtlCancel(void);
extern void edptlcbJumpToGameLocation(void *menu, struct eduiitem_s *item);
extern void cbPtlSelGCode(struct eduimenu_s *menu, struct eduiitem_s *item);

extern struct numtl_s *DebMat[8];
extern void eduiItemSliderSetFmt(void *item, char *fmt);
extern void *eduiItemTexturePickCreate(void *data, struct eduistyle_s *font, void *cb, char *label);
extern void cbPtlCancelCollMenu(void);
extern void cbPtlApplyCollEnv(void);
extern void cbChangeNumCollSpheres(void *menu, struct eduiitem_s *item);
extern void cbPtlDefaultCollEnv(struct eduimenu_s *menu);
extern void cbPtlCancelSizeMenu(void);
extern void cbPtlCancelRotMenu(void);
extern void cbPtlCancelCutOffMenu(void);
extern void cbPtlChangeCutOff(void *menu, struct eduiitem_s *item);
extern void cbPtlChangeCutOn(void *menu, struct eduiitem_s *item);
extern void cbPtlChangeDrawCutOff(void *menu, struct eduiitem_s *item);
extern void cbPtlCancelVarEmitMenu(void);
extern void cbPtlCancelVarStartMenu(void);
extern void cbPtlChangeX(void *menu, struct eduiitem_s *item);
extern void cbPtlChangeTextureSelect(struct eduimenu_s *menu, struct eduiitem_s *item);
extern void cbPtlCancelTextureMenu(void);
extern void cbPtlSelTextureType(void *menu, struct eduiitem_s *item);
extern void cbPtlCancelEmitTimeMenu(void);
extern void cbPtlChangeIvalOn(void *menu, struct eduiitem_s *item);
extern void cbPtlChangeIvalOnRan(void *menu, struct eduiitem_s *item);
extern void cbPtlChangeIvalOff(void *menu, struct eduiitem_s *item);
extern void cbPtlChangeIvalOffRan(void *menu, struct eduiitem_s *item);
extern void cbPtlCancelEmitVelMenu(void);
extern void cbPtlChangeEmitVel(void *menu, struct eduiitem_s *item);
extern void cbPtlCancelGravMenu(void);
extern void cbPtlChangeGrav(void *menu, struct eduiitem_s *item);

extern void EdFileSetMedia(int media);
extern int EdFileOpen(char *name, int mode);
extern int EdFileClose(void);
extern void EdFileWrite(void *data, int size);
extern void EdFileWriteInt(int data);
extern void EdFileWriteShort(short data);
extern void EdFileWriteChar(char data);
extern void EdFileWriteFloat(float data);
extern int LookupDebrisEffect(char *name);
extern void DebrisSetTrigger(int key, int type, int id, float var);
extern void cbCancelMessageMenu(void);
extern void ParticleReset(void);

extern struct mcdirent_s mcdir[];
extern int sceMcGetDir(int port, int slot, char *name, unsigned int mode, int maxent,
                       struct mcdirent_s *table);

extern int freeDmaDebType;
extern int EdFileReadInt(void);
extern int NuFileSize(char *name);
extern void EdFileRead(void *data, int size);
extern float EdFileReadFloat(void);
extern short EdFileReadShort(void);
extern char EdFileReadChar(void);



int edppPtlCreate(struct nuvec_s *pos, int type) {
    int start;

    start = edpp_nextalloc;
    for (;;) {
        if (edpp_ptls[edpp_nextalloc].debris == -1) {
            goto found;
        }
        edpp_nextalloc++;
        if (edpp_nextalloc > 256) {
            edpp_nextalloc = 0;
        }
        if (edpp_nextalloc == start) {
            return -1;
        }
    }
found:
    AddDebrisEffect(&edpp_ptls[edpp_nextalloc].debris, type, pos->x, pos->y, pos->z);
    if (edpp_ptls[edpp_nextalloc].debris != -1) {
        DebrisOrientation(edpp_ptls[edpp_nextalloc].debris, edpp_rotz, edpp_roty);
        DebrisEmitterOrientation(edpp_ptls[edpp_nextalloc].debris, edpp_emitrotz, edpp_emitroty);
        DebrisStartOffset(edpp_ptls[edpp_nextalloc].debris, edpp_offset);
        DebrisReflectionOrientation(edpp_ptls[edpp_nextalloc].debris, 0.0f, 0.9f, edpp_refrotz,
                                    edpp_refroty);
        DebrisSetGroupID(edpp_ptls[edpp_nextalloc].debris, 0);
        edpp_ptls[edpp_nextalloc].group = 0;
        edpp_ptls[edpp_nextalloc].bounceoffset = 0.0f;
        edpp_ptls[edpp_nextalloc].bouncefactor = 0.9f;
        edpp_ptls[edpp_nextalloc].pos = *pos;
        edpp_ptls[edpp_nextalloc].type = type;
        edpp_ptls[edpp_nextalloc].rotz = edpp_rotz;
        edpp_ptls[edpp_nextalloc].roty = edpp_roty;
        edpp_ptls[edpp_nextalloc].emitrotz = edpp_emitrotz;
        edpp_ptls[edpp_nextalloc].emitroty = edpp_emitroty;
        edpp_ptls[edpp_nextalloc].offset = edpp_offset;
        edpp_ptls[edpp_nextalloc].switchtype = 0;
        edpp_ptls[edpp_nextalloc].switchid = -1;
        edpp_ptls[edpp_nextalloc].switchvar = 0.0f;
        strcpy(edpp_ptls[edpp_nextalloc].name, debtab[type]->id);
        start = edpp_nextalloc++;
        return start;
    }
    return -1;
}


int edppPtlPlace(int index, struct nuvec_s *pos) {
    DebrisEmiterPos(edpp_ptls[index].debris, pos->x, pos->y, pos->z);
    DebrisOrientation(edpp_ptls[index].debris, edpp_rotz, edpp_roty);
    DebrisEmitterOrientation(edpp_ptls[index].debris, edpp_emitrotz, edpp_emitroty);
    DebrisReflectionOrientation(edpp_ptls[index].debris, edpp_ptls[index].bounceoffset,
                                edpp_ptls[index].bouncefactor, edpp_refrotz, edpp_refroty);
    edpp_ptls[index].pos = *pos;
    edpp_ptls[index].emitrotz = edpp_emitrotz;
    edpp_ptls[index].emitroty = edpp_emitroty;
    edpp_ptls[index].rotz = edpp_rotz;
    edpp_ptls[index].roty = edpp_roty;
    edpp_ptls[index].refrotz = edpp_refrotz;
    edpp_ptls[index].refroty = edpp_refroty;
    return index;
}


void edppDoInput(struct nupad_s *pad) {
    int i;

    if (pad->paddata & 0x100) {
        if (edpp_nearest != -1) {
            if (pad->paddata_db & 0x8) {
                i = edpp_nearest;
                do {
                    i++;
                    if (i == 256) {
                        i %= 256;
                    }
                } while (edpp_ptls[i].debris == -1);
                edpp_nearest = i;
            }
            if (pad->paddata_db & 0x2) {
                do {
                    edpp_nearest--;
                    if (edpp_nearest == -1) {
                        edpp_nearest = 255;
                    }
                } while (edpp_ptls[edpp_nearest].debris == -1);
            }
        } else {
            edppDetermineNearest(-1.0f);
        }
        if (edpp_nearest != -1) {
            edcamSetPos(&edpp_ptls[edpp_nearest].pos);
            edpp_rotz = edpp_ptls[edpp_nearest].rotz;
            edpp_roty = edpp_ptls[edpp_nearest].roty;
            edpp_emitrotz = edpp_ptls[edpp_nearest].emitrotz;
            edpp_emitroty = edpp_ptls[edpp_nearest].emitroty;
            edpp_offset = edpp_ptls[edpp_nearest].offset;
            edpp_create_type = edpp_ptls[edpp_nearest].type;
            edpp_effect_list = debtab[edpp_create_type]->listid;
        }
    } else {
        edcamMove(pad);
    }
    if (edpp_snap_enabled) {
        edcamGetPosAngSnap(&edpp_cam_pos, &edpp_cam_ax, &edpp_cam_ay);
    } else {
        edcamGetPosAng(&edpp_cam_pos, &edpp_cam_ax, &edpp_cam_ay);
    }
    if (pad->paddata_db & 0x80) {
        edpp_active_menu = ptloptmenu;
    }
    if ((pad->paddata_db & 0x40) && edpp_create_type != -1) {
        edppPtlCreate(&edpp_cam_pos, edpp_create_type);
    }
    if (pad->paddata & 0x20) {
        if (edpp_nearest != -1) {
            edppPtlPlace(edpp_nearest, &edpp_cam_pos);
        }
    } else {
        edppDetermineNearest(1.0f);
    }
    if (pad->paddata_db & 0x10) {
        if (edpp_nearest != -1) {
            i = edpp_nearest;
            if (edpp_ptls[i].debris != -1) {
                DebFreeInstantly(&edpp_ptls[i].debris);
                edpp_ptls[i].debris = -1;
            }
        }
        edpp_nearest = -1;
    }
    if (edpp_dpad_mode == 1) {
        edpp_roty += pad->rdata.l_algpad_r;
        edpp_roty -= pad->rdata.l_algpad_l;
        edpp_rotz += pad->rdata.l_algpad_u;
        if (edpp_rotz > 0) {
            edpp_rotz = 0;
        }
        edpp_rotz -= pad->rdata.l_algpad_d;
        if (edpp_rotz < -0x8000) {
            edpp_rotz = -0x8000;
        }
    } else if (edpp_dpad_mode == 0) {
        edpp_emitroty += pad->rdata.l_algpad_r;
        edpp_emitroty -= pad->rdata.l_algpad_l;
        edpp_emitrotz += pad->rdata.l_algpad_u;
        if (edpp_emitrotz > 0x8000) {
            edpp_emitrotz = 0x8000;
        }
        edpp_emitrotz -= pad->rdata.l_algpad_d;
        if (edpp_emitrotz < -0x8000) {
            edpp_emitrotz = -0x8000;
        }
    } else if (edpp_dpad_mode == 3) {
        edpp_refroty += pad->rdata.l_algpad_r;
        edpp_refroty -= pad->rdata.l_algpad_l;
        edpp_refrotz += pad->rdata.l_algpad_u;
        if (edpp_refrotz > 0) {
            edpp_refrotz = 0;
        }
        edpp_refrotz -= pad->rdata.l_algpad_d;
        if (edpp_refrotz < -0x8000) {
            edpp_refrotz = -0x8000;
        }
    } else if (edpp_dpad_mode == 2) {
        if (pad->rdata.l_algpad_u == 0xFF || (pad->paddata_db & 0x1000)) {
            edpp_offset++;
        }
        if (pad->rdata.l_algpad_d == 0xFF || (pad->paddata_db & 0x4000)) {
            edpp_offset--;
        }
        if (edpp_offset < 0) {
            edpp_offset = 0;
        }
    }
}


void edppDrawSpheres(struct debinfo_s *dt, int key) {
    struct nuvec_s pos;
    float t;
    float life;
    float size;
    int i;
    int j;

    if (dt->numspheres == 0) {
        return;
    }
    for (i = 0; i < dt->numspheres; i++) {
        t = debkeydata[key].spheres[i].t;
        if (t == -1.0f) {
            continue;
        }
        pos.x = debkeydata[key].spheres[i].emit.x * t + debkeydata[key].emitpos.x;
        pos.y = debkeydata[key].spheres[i].emit.y * t + dt->grav * (t * t)
              + debkeydata[key].emitpos.y;
        pos.z = debkeydata[key].spheres[i].emit.z * t + debkeydata[key].emitpos.z;
        life = t / dt->etime;
        for (j = 0; j < 7; j++) {
            if (dt->sphereslot[j].t <= life && life <= dt->sphereslot[j + 1].t) {
                break;
            }
        }
        size = dt->sphereslot[j].value + (life - dt->sphereslot[j].t)
             / (dt->sphereslot[j + 1].t - dt->sphereslot[j].t)
             * (dt->sphereslot[j + 1].value - dt->sphereslot[j].value);
        pos.y -= size;
        SphereDraw(&pos, size, 1.0f);
    }
}


void edppDrawCursor(void) {
    struct nuvtx_tc1_s line[2];
    struct nuvec_s v;
    struct debinfo_s *dt;

    v.x = 0.5f;
    v.y = 0.0f;
    v.z = 0.0f;
    NuVecRotateZ(&v, &v, edpp_rotz);
    NuVecRotateY(&v, &v, edpp_roty);
    line[0].pnt.x = edpp_cam_pos.x - v.x;
    line[0].pnt.y = edpp_cam_pos.y - v.y;
    line[0].pnt.z = edpp_cam_pos.z - v.z;
    line[1].pnt.x = edpp_cam_pos.x + v.x;
    line[1].pnt.y = edpp_cam_pos.y + v.y;
    line[1].pnt.z = edpp_cam_pos.z + v.z;
    line[0].diffuse = -1;
    line[1].diffuse = -1;
    NuRndrLine3d(line, edpp_mtl, 0);
    v.x = 0.0f;
    v.y = 0.5f;
    v.z = 0.0f;
    NuVecRotateZ(&v, &v, edpp_rotz);
    NuVecRotateY(&v, &v, edpp_roty);
    line[0].pnt.x = edpp_cam_pos.x - v.x;
    line[0].pnt.y = edpp_cam_pos.y - v.y;
    line[0].pnt.z = edpp_cam_pos.z - v.z;
    line[1].pnt.x = edpp_cam_pos.x;
    line[1].pnt.y = edpp_cam_pos.y;
    line[1].pnt.z = edpp_cam_pos.z;
    line[0].diffuse = -1;
    line[1].diffuse = -1;
    NuRndrLine3d(line, edpp_mtl, 0);
    line[0].pnt.x = edpp_cam_pos.x;
    line[0].pnt.y = edpp_cam_pos.y;
    line[0].pnt.z = edpp_cam_pos.z;
    line[1].pnt.x = edpp_cam_pos.x + v.x;
    line[1].pnt.y = edpp_cam_pos.y + v.y;
    line[1].pnt.z = edpp_cam_pos.z + v.z;
    line[0].diffuse = 0xFF00FF00;
    line[1].diffuse = 0xFF00FF00;
    NuRndrLine3d(line, edpp_mtl, 0);
    v.x = 0.0f;
    v.y = 0.0f;
    v.z = 0.5f;
    NuVecRotateZ(&v, &v, edpp_rotz);
    NuVecRotateY(&v, &v, edpp_roty);
    line[0].pnt.x = edpp_cam_pos.x - v.x;
    line[0].pnt.y = edpp_cam_pos.y - v.y;
    line[0].pnt.z = edpp_cam_pos.z - v.z;
    line[1].pnt.x = edpp_cam_pos.x + v.x;
    line[1].pnt.y = edpp_cam_pos.y + v.y;
    line[1].pnt.z = edpp_cam_pos.z + v.z;
    line[0].diffuse = -1;
    line[1].diffuse = -1;
    NuRndrLine3d(line, edpp_mtl, 0);
    v.x = 0.0f;
    v.y = 0.375f;
    v.z = 0.0f;
    NuVecRotateZ(&v, &v, edpp_emitrotz);
    NuVecRotateY(&v, &v, edpp_emitroty);
    NuVecRotateZ(&v, &v, edpp_rotz);
    NuVecRotateY(&v, &v, edpp_roty);
    line[0].pnt.x = edpp_cam_pos.x;
    line[0].pnt.y = edpp_cam_pos.y;
    line[0].pnt.z = edpp_cam_pos.z;
    line[1].pnt.x = edpp_cam_pos.x + v.x;
    line[1].pnt.y = edpp_cam_pos.y + v.y;
    line[1].pnt.z = edpp_cam_pos.z + v.z;
    line[0].diffuse = 0xFF0000FF;
    line[1].diffuse = 0xFF0000FF;
    NuRndrLine3d(line, edpp_mtl, 0);
    if (edpp_dpad_mode == 3) {
        v.x = 0.0f;
        v.y = 0.25f;
        v.z = 0.0f;
        NuVecRotateZ(&v, &v, edpp_refrotz);
        NuVecRotateY(&v, &v, edpp_refroty);
        NuVecRotateZ(&v, &v, edpp_rotz);
        NuVecRotateY(&v, &v, edpp_roty);
        line[0].pnt.x = edpp_cam_pos.x;
        line[0].pnt.y = edpp_cam_pos.y;
        line[0].pnt.z = edpp_cam_pos.z;
        line[1].pnt.x = edpp_cam_pos.x + v.x;
        line[1].pnt.y = edpp_cam_pos.y + v.y;
        line[1].pnt.z = edpp_cam_pos.z + v.z;
        line[0].diffuse = 0xFFFF0000;
        line[1].diffuse = 0xFFFF0000;
        NuRndrLine3d(line, edpp_mtl, 0);
        if (edpp_nearest != -1) {
            v.x = 0.0f;
            v.y = edpp_ptls[edpp_nearest].bounceoffset;
            v.z = 0.0f;
            NuVecRotateZ(&v, &v, edpp_refrotz);
            NuVecRotateY(&v, &v, edpp_refroty);
            NuVecRotateZ(&v, &v, edpp_rotz);
            NuVecRotateY(&v, &v, edpp_roty);
            edbitsDrawCube(edpp_cam_pos.x + v.x, edpp_cam_pos.y + v.y, edpp_cam_pos.z + v.z,
                           0.25f, 0.0f, 0.25f, edpp_refrotz, edpp_refroty, edpp_rotz, edpp_roty,
                           0xFFFF0000, edpp_mtl);
        }
    }
    NuRndrRect2di(0x1720, 0x9B0, 0xDC0, 0x410, 0x80808080, edpp_boxmtl);
    NuRndrRect2di(0x1710, 0x8F0, 0xDE0, 0xC0, 0x80000000, edpp_boxmtl);
    NuRndrLine2di(0x1710, 0x9B0, 0x1710, 0xDC8, 0x80000000, edpp_boxmtl);
    NuRndrLine2di(0x24F0, 0x9B0, 0x24F0, 0xDC8, 0x80000000, edpp_boxmtl);
    NuRndrLine2di(0x1710, 0xDC8, 0x24F0, 0xDC8, 0x80000000, edpp_boxmtl);
    NuFntSet(app_fnt);
    NuFntSetPen(0x80808080);
    NuFntPrintEx(0x17C0, 0x988, 0, "Info Box");
    NuFntSetPen(0x80000000);
    if (edpp_effect_list == 1) {
        NuFntPrintEx(0x17C0, 0xA50, 0, "Current List: Level");
    } else if (edpp_effect_list == 0) {
        NuFntPrintEx(0x17C0, 0xA50, 0, "Current List: General");
    } else if (edpp_effect_list == 5) {
        NuFntPrintEx(0x17C0, 0xA50, 0, "Current List: Character");
    }
    if (edpp_create_type == -1) {
        NuFntPrintEx(0x17C0, 0xAF0, 0, "Current Type: <none>");
    } else {
        NuFntPrintEx(0x17C0, 0xAF0, 0, "Current Type: %s", debtab[edpp_create_type]->id);
    }
    if (edptl_clipboard_entry == -1) {
        NuFntPrintEx(0x17C0, 0xB90, 0, "Clipboard: <none>");
    } else {
        NuFntPrintEx(0x17C0, 0xB90, 0, "Clipboard: %s", debtab[edptl_clipboard_entry]->id);
    }
    if (edpp_nearest == -1) {
        NuFntPrintEx(0x17C0, 0xC30, 0, "Highlight: <none>");
    } else {
        dt = debtab[debkeydata[edpp_ptls[edpp_nearest].debris].type];
        if (dt->gensort == 0) {
            edbitsDrawCube(edpp_cam_pos.x, edpp_cam_pos.y, edpp_cam_pos.z, dt->variable_start.x,
                           dt->variable_start.y, dt->variable_start.z, edpp_emitrotz, edpp_emitroty,
                           edpp_rotz, edpp_roty, 0xFF0000FF, edpp_mtl);
            edppDrawSpheres(dt, edpp_ptls[edpp_nearest].debris);
        }
        NuFntPrintEx(0x17C0, 0xC30, 0, "Highlight: %s", dt->id);
        NuFntPrintEx(0x17C0, 0xCD0, 0, "Particles: %d (%d)", dt->debnum,
                     ((dt->debnum - 1) / 32 + 1) * 32);
    }
    if (edpp_dpad_mode == 0) {
        NuFntPrintEx(0x17C0, 0xD70, 0, "Dpad Mode: Emit Rotate");
    } else if (edpp_dpad_mode == 1) {
        NuFntPrintEx(0x17C0, 0xD70, 0, "Dpad Mode: Grav Rotate");
    } else if (edpp_dpad_mode == 2) {
        NuFntPrintEx(0x17C0, 0xD70, 0, "Dpad Mode: Offset (%d)", edpp_offset);
    } else if (edpp_dpad_mode == 3) {
        NuFntPrintEx(0x17C0, 0xD70, 0, "Dpad Mode: Reflections");
    }
}


void edppDetermineNearest(float range) {
    struct nuvec_s d;
    float dist;
    int i;

    if (edpp_nearest != -1) {
        NuVecSub(&d, &edpp_cam_pos, &edpp_ptls[edpp_nearest].pos);
        if (d.x * d.x + d.y * d.y + d.z * d.z == 0.0) {
            return;
        }
    }
    edpp_nearest = -1;
    for (i = 0; i < 256; i++) {
        if (edpp_ptls[i].debris != -1) {
            NuVecSub(&d, &edpp_cam_pos, &edpp_ptls[i].pos);
            dist = d.x * d.x + d.y * d.y + d.z * d.z;
            if (range < 0.0f || dist < range) {
                edpp_nearest = i;
                range = dist;
            }
        }
    }
}


void cbPtlApplyGrad(void) {
    struct eduigradread_s keys[8];
    struct debinfo_s *coldt;
    struct debinfo_s *alphadt;
    int count;
    int i;
    int j;

    count = eduiGradPickRead(grad_item, keys, 8);
    if (count >= 2 && count <= 8 && edpp_nearest != -1
        && edpp_ptls[edpp_nearest].debris != -1) {
        coldt = debtab[debkeydata[edpp_ptls[edpp_nearest].debris].type];
        for (i = 0; i < count; i++) {
            coldt->colslot[i].t = keys[i].pos;
            coldt->colslot[i].r = keys[i].r * 255.0;
            coldt->colslot[i].g = keys[i].g * 255.0;
            coldt->colslot[i].b = keys[i].b * 255.0;
        }
        GenericDebinfoDmaTypeUpdate(coldt);
    }
    count = eduiGradPickRead(grad_alpha_item, keys, 8);
    if (count >= 2 && count <= 8 && edpp_nearest != -1
        && edpp_ptls[edpp_nearest].debris != -1) {
        alphadt = debtab[debkeydata[edpp_ptls[edpp_nearest].debris].type];
        for (j = 0; j < count; j++) {
            alphadt->alpslot[j].t = keys[j].pos;
            alphadt->alpslot[j].value = keys[j].r * 255.0;
        }
        GenericDebinfoDmaTypeUpdate(alphadt);
    }
}


void TidyAllEffects(void) {
    int i;
    int j;
    int k;

    for (i = 1; i < effect_types_used; i++) {
        j = i;
        while (debtab[j] == 0) {
            j++;
        }
        if (j != i) {
            for (k = 0; k < 256; k++) {
                if (edpp_ptls[k].debris != -1 && edpp_ptls[k].type == j) {
                    edpp_ptls[k].type = i;
                    debkeydata[edpp_ptls[k].debris].type = i;
                }
            }
            debtab[i] = &effecttypes[i];
            for (k = 0; k < 256; k++) {
                if (ParticleChunkRenderStack[k].chunk != 0
                    && ParticleChunkRenderStack[k].debinfo == debtab[j]) {
                    ParticleChunkRenderStack[k].debinfo = debtab[i];
                }
            }
            *debtab[i] = *debtab[j];
            *debtab[j] = *debtab[0];
            debtab[j] = 0;
        }
    }
}


void cbPtlDeleteEffect(struct eduimenu_s *menu) {
    struct eduimenu_s *arg;
    int i;

    arg = 0;
    if (edpp_ptls[edpp_nearest].type == edpp_create_type) {
        edpp_nearest = -1;
    }
    for (i = 0; i < 256; i++) {
        if (edpp_ptls[i].type == edpp_create_type && edpp_ptls[i].debris != -1) {
            DebFreeInstantly(&edpp_ptls[i].debris);
            edpp_ptls[i].debris = -1;
        }
    }
    edppDetermineNearest(1.0f);
    *debtab[edpp_create_type] = *debtab[0];
    debtab[edpp_create_type] = 0;
    effect_types_used--;
    if (menu->parent != 0) {
        arg = menu->parent;
        eduiMenuDetach(menu);
    }
    if (menu->cancel != 0) {
        menu->cancel(menu, arg);
    }
    edpp_create_type = -1;
}


void cbPtlAddEffect(struct eduimenu_s *menu) {
    struct eduimenu_s *arg;
    int i;

    arg = 0;
    if (effect_types_used < 128) {
        for (i = 1; i < 128; i++) {
            if (debtab[i] == 0) {
                debtab[i] = &effecttypes[i];
                *debtab[i] = *debtab[0];
                sprintf(debtab[i]->id, "New%d", i);
                debtab[i]->listid = edpp_effect_list;
                edpp_create_type = i;
                effect_types_used++;
                break;
            }
        }
    }
    if (menu->parent != 0) {
        arg = menu->parent;
        eduiMenuDetach(menu);
    }
    if (menu->cancel != 0) {
        menu->cancel(menu, arg);
    }
}


void cbPtlCopyEffect(struct eduimenu_s *menu) {
    struct eduimenu_s *arg;
    int i;

    arg = 0;
    if (effect_types_used < 128 && edpp_create_type != -1) {
        for (i = 1; i < 128; i++) {
            if (debtab[i] == 0) {
                debtab[i] = &effecttypes[i];
                *debtab[i] = *debtab[edpp_create_type];
                debtab[i]->DmaDebTypePointer = 0;
                sprintf(debtab[i]->id, "Copy%d", i);
                edpp_create_type = i;
                effect_types_used++;
                break;
            }
        }
    }
    if (menu->parent != 0) {
        arg = menu->parent;
        eduiMenuDetach(menu);
    }
    if (menu->cancel != 0) {
        menu->cancel(menu, arg);
    }
}


void cbPtlChangeY(void *menu, struct eduiitem_s *item) {
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    if (menu == 0) {
        return;
    }
    if (menu == ptlvaremitmenu) {
        if (dt->gensort == 0 || dt->gensort == 8 || dt->gensort == 9 || dt->gensort == 10) {
            dt->variable_emit.y = item->u.slider.val;
            dt->variable_emit_ranscale.y = (item->u.slider.val * 2.0f) * (1.0 / 2147483648.0);
        } else if (dt->gensort == 6 || dt->gensort == 7) {
            dt->variable_emit.y = (int)(item->u.slider.val * 182.04445f);
            dt->variable_emit_ranscale.y = (int)((item->u.slider.val * 2.0f) * 182.04445f) * (1.0 / 2147483648.0);
        }
    } else if (menu == ptlvarstartmenu) {
        if (dt->gensort == 0 || dt->gensort == 8 || dt->gensort == 9 || dt->gensort == 10) {
            dt->variable_start.y = item->u.slider.val;
            dt->variable_start_ranscale.y = (item->u.slider.val * 2.0f) * (1.0 / 2147483648.0);
        } else if (dt->gensort == 6 || dt->gensort == 7) {
            dt->variable_start.y = (int)(item->u.slider.val * 182.04445f);
        }
    }
}


void cbPtlChangeZ(void *menu, struct eduiitem_s *item) {
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    if (menu == 0) {
        return;
    }
    if (menu == ptlvaremitmenu) {
        if (dt->gensort == 0 || dt->gensort == 8 || dt->gensort == 9 || dt->gensort == 10) {
            dt->variable_emit.z = item->u.slider.val;
            dt->variable_emit_ranscale.z = (item->u.slider.val * 2.0f) * (1.0 / 2147483648.0);
        } else if (dt->gensort == 6 || dt->gensort == 7) {
            dt->variable_emit.z = (int)(item->u.slider.val * 182.04445f);
            dt->variable_emit_ranscale.z = (int)((item->u.slider.val * 2.0f) * 182.04445f) * (1.0 / 2147483648.0);
        }
    } else if (menu == ptlvarstartmenu) {
        if (dt->gensort == 0 || dt->gensort == 8 || dt->gensort == 9 || dt->gensort == 10) {
            dt->variable_start.z = item->u.slider.val;
            dt->variable_start_ranscale.z = (item->u.slider.val * 2.0f) * (1.0 / 2147483648.0);
        } else if (dt->gensort == 6 || dt->gensort == 7) {
            dt->variable_start.z = (int)(item->u.slider.val * 182.04445f);
        }
    }
}


void UpdateTotalPtls(struct debinfo_s *dt) {
    int frames;
    int total;
    int count;
    int ontimer;
    int offtimer;
    int i;

    frames = dt->etime * 60.0f;
    total = 0;
    count = 0;
    offtimer = 1;
    if (dt->ival_off == 0) {
        ontimer = frames;
    } else {
        ontimer = dt->ival_on + dt->ival_on_ran;
    }
    for (i = 0; i < frames; i++) {
        if (ontimer != 0) {
            if (dt->generate >= 0) {
                total += dt->generate;
            } else if (count % abs(dt->generate) == 0) {
                total++;
            }
            ontimer--;
            count++;
            if (ontimer == 0) {
                offtimer = dt->ival_off;
            }
        } else {
            offtimer--;
            if (offtimer == 0) {
                count = 0;
                ontimer = dt->ival_on + dt->ival_on_ran;
            }
        }
    }
    dt->debnum = total;
    if (dt->debnum <= 0) {
        dt->debnum = 1;
    }
    for (i = 0; i < 256; i++) {
        if (edpp_ptls[i].debris != -1
            && debtab[debkeydata[edpp_ptls[i].debris].type] == dt) {
            DebReAlloc(&debkeydata[edpp_ptls[i].debris], dt->debnum);
        }
    }
}


static inline void cbCancelChangeNameMenu(void *menu) {
    struct debinfo_s *dt;

    if (edpp_create_type != -1) {
        dt = debtab[edpp_create_type];
        if (dt != 0) {
            if (dt->id[0] == 0) {
                CreateMessageMenu(menu, "Name Duplicate", 0);
                return;
            }
            eduiMenuDestroy(namemenu);
            namemenu = 0;
        }
    }
}


static inline void cbChangeNameMenu(struct eduimenu_s *menu) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};
    struct debinfo_s *dt;

    if (edpp_create_type == -1) {
        return;
    }
    dt = debtab[edpp_create_type];
    namemenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, cbCancelChangeNameMenu, "Type Name");
    if (namemenu != 0) {
        eduiMenuAddItem(namemenu, eduiItemTextPickCreate(0, &font, cbChangeName, "Type Name"));
        strcpy(edui_last_item->u.textpick.text, dt->id);
        eduiMenuAttach(menu, namemenu);
        namemenu->x = menu->x + 10;
        namemenu->y = menu->y + 40;
    }
}


void cbChangeGenRateMenu(struct eduimenu_s *menu) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    changegenratemenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, cbCancelChangeGenRateMenu,
                                       "Gen Rate");
    if (changegenratemenu != 0) {
        eduiMenuAddItem(changegenratemenu,
                        eduiItemSliderCreateInt(0, &font, 0, cbPtlChangeGenRate, -10,
                                                superscale * 20.0, dt->generate, "Gen Rate"));
        eduiMenuAttach(menu, changegenratemenu);
        changegenratemenu->x = menu->x + 10;
        changegenratemenu->y = menu->y + 40;
    }
}


static inline void cbChangeETimeMenu(struct eduimenu_s *menu) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    etimemenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, cbCancelChangeETimeMenu,
                               "Particle Life");
    if (etimemenu != 0) {
        eduiMenuAddItem(etimemenu, eduiItemSliderCreate(0, &font, 0, cbPtlChangeETime, 0.0f, 10.0f,
                                                        dt->etime, "Particle Life"));
        eduiMenuAttach(menu, etimemenu);
        etimemenu->x = menu->x + 10;
        etimemenu->y = menu->y + 40;
    }
}


void cbPtlCopySize(void *menu, struct eduiitem_s *item) {
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;
    struct eduiitem_s *grad;
    float ratio;
    int i;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    for (i = 0; i < 8; i++) {
        if (item->value == 1) {
            dt->hslot[i].t = dt->wslot[i].t;
            dt->hslot[i].value = dt->wslot[i].value;
        } else {
            dt->wslot[i].t = dt->hslot[i].t;
            dt->wslot[i].value = dt->hslot[i].value;
        }
    }
    if (item->value == 1) {
        grad = grad_size_h_item;
    } else {
        grad = grad_size_w_item;
    }
    while (grad->u.grad.first != 0) {
        eduiGradStageDelete(grad, grad->u.grad.first);
    }
    for (i = 0; i < 8; i++) {
        if (item->value == 1) {
            ratio = (dt->hslot[i].value - dt->minsize) / (dt->maxsize - dt->minsize);
            eduiGradStageAddRGB(grad_size_h_item, dt->hslot[i].t, ratio, ratio, ratio);
            if (dt->hslot[i].t == 1.0f) {
                break;
            }
        } else {
            ratio = (dt->wslot[i].value - dt->minsize) / (dt->maxsize - dt->minsize);
            eduiGradStageAddRGB(grad_size_w_item, dt->wslot[i].t, ratio, ratio, ratio);
            if (dt->wslot[i].t == 1.0f) {
                break;
            }
        }
    }
    GenericDebinfoDmaTypeUpdate(dt);
}


void cbPtlApplySize(void) {
    struct eduigradread_s keys[8];
    struct debinfo_s *dt;
    struct eduiitem_s *item;
    float lo;
    float hi;
    int count;
    int i;

    if (grad_size_min_item == 0 || grad_size_max_item == 0 || grad_size_h_item == 0
        || grad_size_w_item == 0) {
        return;
    }
    count = eduiGradPickRead(grad_size_w_item, keys, 8);
    item = grad_size_min_item;
    lo = item->u.slider.val;
    item = grad_size_max_item;
    hi = item->u.slider.val;
    if (count >= 2 && count <= 8 && edpp_nearest != -1
        && edpp_ptls[edpp_nearest].debris != -1) {
        dt = debtab[debkeydata[edpp_ptls[edpp_nearest].debris].type];
        for (i = 0; i < count; i++) {
            dt->wslot[i].t = keys[i].pos;
            dt->wslot[i].value = lo + keys[i].r * (hi - lo);
        }
        dt->minsize = lo;
        dt->maxsize = hi;
        GenericDebinfoDmaTypeUpdate(dt);
    }
    count = eduiGradPickRead(grad_size_h_item, keys, 8);
    if (count >= 2 && count <= 8 && edpp_nearest != -1
        && edpp_ptls[edpp_nearest].debris != -1) {
        dt = debtab[debkeydata[edpp_ptls[edpp_nearest].debris].type];
        for (i = 0; i < count; i++) {
            dt->hslot[i].t = keys[i].pos;
            dt->hslot[i].value = lo + keys[i].r * (hi - lo);
        }
        dt->minsize = lo;
        dt->maxsize = hi;
        GenericDebinfoDmaTypeUpdate(dt);
    }
}


void cbPtlApplyRot(void) {
    struct eduigradread_s keys[8];
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;
    struct eduiitem_s *item;
    float lo;
    float hi;
    int count;
    int i;

    if (grad_rot_min_item == 0 || grad_rot_max_item == 0 || grad_rot_item == 0) {
        return;
    }
    count = eduiGradPickRead(grad_rot_item, keys, 8);
    item = grad_rot_min_item;
    lo = item->u.slider.val;
    item = grad_rot_max_item;
    hi = item->u.slider.val;
    if (lo == hi) {
        hi = hi + 0.1;
    }
    if (count >= 2 && count <= 8 && edpp_nearest != -1
        && edpp_ptls[edpp_nearest].debris != -1) {
        deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
        dt = debtab[deb->type];
        for (i = 0; i < count; i++) {
            dt->rotslot[i].t = keys[i].pos;
            dt->rotslot[i].value = (int)((lo + keys[i].r * (hi - lo)) * 182.04445f);
        }
        dt->minrot = lo;
        dt->maxrot = hi;
        GenericDebinfoDmaTypeUpdate(dt);
    }
}


void cbPtlSelGSort(struct eduimenu_s *menu, struct eduiitem_s *item) {
    struct debinfo_s *dt;

    eduiMenuDetach(menu);
    edpp_active_menu = 0;
    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    dt = debtab[debkeydata[edpp_ptls[edpp_nearest].debris].type];
    if (dt->gensort != item->value) {
        dt->gensort = item->value;
        debkeydata[edpp_ptls[edpp_nearest].debris].genptr = gensorttab[item->value];
        memset(&dt->variable_emit, 0, sizeof(struct nuvec_s));
        memset(&dt->variable_emit_ranscale, 0, sizeof(struct nuvec_s));
        memset(&dt->variable_start, 0, sizeof(struct nuvec_s));
        memset(&dt->variable_start_ranscale, 0, sizeof(struct nuvec_s));
    }
}


static inline void CreateMessageMenu(struct eduimenu_s *parent, char *message, int flags) {
    struct eduistyle_s font = {{0x800000C0, 0x80FF0000}, {0x80808080, 0x80404040}};

    if (flags == 1) {
        font.pen[0] = 0x8000C000;
    }
    messagemenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, cbCancelMessageMenu, "Message");
    if (messagemenu != 0) {
        eduiMenuAddItem(messagemenu, eduiItemSelCreate(1, &font, 0, 0, 0, message));
        eduiMenuAttach(parent, messagemenu);
        messagemenu->x = parent->x + 10;
        messagemenu->y = parent->y + 40;
    }
}


static inline void CreateConfirmMenu(struct eduimenu_s *parent, char *title, void *yescb) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};

    confirmmenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, cbCancelConfirmMenu, title);
    if (confirmmenu != 0) {
        eduiMenuAddItem(confirmmenu, eduiItemSelCreate(1, &font, 0, 0, cbConfirmMenuNo, "No"));
        eduiMenuAddItem(confirmmenu, eduiItemSelCreate(1, &font, 0, 0, yescb, "Yes"));
        eduiMenuAttach(parent, confirmmenu);
        confirmmenu->x = parent->x + 10;
        confirmmenu->y = parent->y + 40;
    }
}


void cbEffectListMenu(struct eduimenu_s *menu) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};

    effectlistmenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, cbCancelEffectListMenu,
                                    "Effect List");
    if (effectlistmenu != 0) {
        eduiMenuAddItem(effectlistmenu, eduiItemCheckCreate(0, &font, edpp_effect_list == 0, 1,
                                                            cbSelEffectList, "General"));
        eduiMenuAddItem(effectlistmenu, eduiItemCheckCreate(1, &font, edpp_effect_list == 1, 1,
                                                            cbSelEffectList, "Level"));
        eduiMenuAddItem(effectlistmenu, eduiItemCheckCreate(5, &font, edpp_effect_list == 5, 1,
                                                            cbSelEffectList, "Char"));
        eduiMenuAttach(menu, effectlistmenu);
        effectlistmenu->x = menu->x + 10;
        effectlistmenu->y = menu->y + 40;
    }
}


static inline void edptlcbSwitchTypeMenu(struct eduimenu_s *menu) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};

    edptl_switchtype_menu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt,
                                           edptlcbCancelSwitchTypeMenu, "Switch Type");
    if (edptl_switchtype_menu != 0) {
        eduiMenuAddItem(edptl_switchtype_menu,
                        eduiItemCheckCreate(0, &font, edpp_ptls[edpp_nearest].switchtype == 0, 1,
                                            edptlcbSetSwitchType, "None"));
        eduiMenuAddItem(edptl_switchtype_menu,
                        eduiItemCheckCreate(1, &font, edpp_ptls[edpp_nearest].switchtype == 1, 1,
                                            edptlcbSetSwitchType, "Global Switch"));
        eduiMenuAttach(menu, edptl_switchtype_menu);
        edptl_switchtype_menu->x = menu->x + 10;
        edptl_switchtype_menu->y = menu->y + 40;
    }
}


void edptlcbSwitchMenu(struct eduimenu_s *menu) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    edptl_switch_menu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, edptlcbCancelSwitchMenu,
                                       "Switch Menu");
    if (edptl_switch_menu != 0) {
        eduiMenuAddItem(edptl_switch_menu, eduiItemSelCreate(1, &font, 0, 0, edptlcbSwitchTypeMenu,
                                                             "Switch Type..."));
        eduiMenuAddItem(edptl_switch_menu,
                        eduiItemSliderCreateInt(0, &font, 0, edptlcbSetSwitchId, -1, 0x81,
                                                edpp_ptls[edpp_nearest].switchid, "Switch ID"));
        eduiMenuAddItem(edptl_switch_menu,
                        eduiItemSliderCreate(0, &font, 0, edptlcbSetSwitchVar, 0.0f, 20.0f,
                                             edpp_ptls[edpp_nearest].switchvar, "Switch Var"));
        eduiMenuAttach(menu, edptl_switch_menu);
        edptl_switch_menu->x = menu->x + 10;
        edptl_switch_menu->y = menu->y + 40;
    }
}


void edptlcbGroupMenu(struct eduimenu_s *menu) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};

    edptl_group_menu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, edptlcbCancelGroupMenu,
                                      "Render Group");
    if (edptl_group_menu != 0) {
        eduiMenuAddItem(edptl_group_menu,
                        eduiItemSliderCreateInt(0, &font, 0, edptlcbSetMasterGroup, 0, 0x20,
                                                debris_render_group, "Master Switch"));
        if (edpp_nearest != -1 && edpp_ptls[edpp_nearest].debris != -1) {
            eduiMenuAddItem(edptl_group_menu,
                            eduiItemSliderCreateInt(0, &font, 0, edptlcbSetGroup, 0, 0x20,
                                                    edpp_ptls[edpp_nearest].group, "Group ID"));
        }
        eduiMenuAttach(menu, edptl_group_menu);
        edptl_group_menu->x = menu->x + 10;
        edptl_group_menu->y = menu->y + 40;
    }
}


void edptlcbDpadModeMenu(struct eduimenu_s *menu) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};

    dpadmodemenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, edptlcbCancelDpadModeMenu,
                                  "Dpad Mode");
    if (dpadmodemenu != 0) {
        eduiMenuAddItem(dpadmodemenu, eduiItemCheckCreate(0, &font, edpp_dpad_mode == 0, 1,
                                                          edptlcbSetDpadMode, "Emitter Rotate"));
        eduiMenuAddItem(dpadmodemenu, eduiItemCheckCreate(1, &font, edpp_dpad_mode == 1, 1,
                                                          edptlcbSetDpadMode, "Gravity Rotate"));
        eduiMenuAddItem(dpadmodemenu, eduiItemCheckCreate(2, &font, edpp_dpad_mode == 2, 1,
                                                          edptlcbSetDpadMode, "Offset"));
        eduiMenuAddItem(dpadmodemenu, eduiItemCheckCreate(3, &font, edpp_dpad_mode == 3, 1,
                                                          edptlcbSetDpadMode, "Reflections"));
        eduiMenuAttach(menu, dpadmodemenu);
        dpadmodemenu->x = menu->x + 10;
        dpadmodemenu->y = menu->y + 40;
    }
}


void cbPtlTypeMenu(struct eduimenu_s *menu) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};
    int checked;
    int i;

    ptltypemenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, cbPtlCancelTypeMenu,
                                 "Particle Type");
    if (ptltypemenu != 0) {
        eduiMenuAddItem(ptltypemenu, eduiItemSelCreate(1, &font, 0, 0, cbPtlAddEffect,
                                                       "Add Effect"));
        for (i = 1; i < 128; i++) {
            if (debtab[i] != 0 && debtab[i]->listid == edpp_effect_list) {
                checked = 0;
                if (edpp_create_type == i) {
                    checked = 1;
                }
                eduiMenuAddItem(ptltypemenu, eduiItemCheckCreate(i, &font, checked, 1,
                                                                 cbPtlSelType, debtab[i]->id));
                if (checked == 1) {
                    ptltypemenu->current = edui_last_item;
                }
            }
        }
        eduiMenuAttach(menu, ptltypemenu);
        ptltypemenu->x = menu->x + 10;
        ptltypemenu->y = menu->y + 40;
    }
}


void cbPtlColMenu(struct eduimenu_s *menu) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;
    int i;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    ptlcolmenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, cbPtlCancelColMenu,
                                "Particle Colour");
    if (ptlcolmenu != 0) {
        eduiMenuAddItem(ptlcolmenu, eduiItemGradPickCreate(0, &font, cbPtlApplyGrad,
                                                           "Colour Grad"));
        grad_item = edui_last_item;
        for (i = 0; i < 8; i++) {
            eduiGradStageAddRGB(grad_item, dt->colslot[i].t, dt->colslot[i].r / 255.0,
                                dt->colslot[i].g / 255.0, dt->colslot[i].b / 255.0);
            if (dt->colslot[i].t == 1.0f) {
                break;
            }
        }
        eduiMenuAddItem(ptlcolmenu, eduiItemGreyGradPickCreate(0, &font, cbPtlApplyGrad,
                                                               "Alpha Grad"));
        grad_alpha_item = edui_last_item;
        for (i = 0; i < 8; i++) {
            eduiGradStageAddRGB(grad_alpha_item, dt->alpslot[i].t, dt->alpslot[i].value / 255.0,
                                dt->alpslot[i].value / 255.0, dt->alpslot[i].value / 255.0);
            if (dt->alpslot[i].t == 1.0f) {
                break;
            }
        }
        eduiMenuAttach(menu, ptlcolmenu);
        ptlcolmenu->x = menu->x + 10;
        ptlcolmenu->y = menu->y + 40;
    }
}


void cbPtlJibMenu(struct eduimenu_s *menu) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    ptljibmenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0x12C, app_fnt, cbPtlCancelJibMenu,
                                "Particle Jibber");
    if (ptljibmenu != 0) {
        eduiMenuAddItem(ptljibmenu, eduiItemSliderCreate(0, &font, 0, cbPtlApplyJib, 0.0f, 32.0f,
                                                         dt->jibxfreq, "Jibber X Freq"));
        grad_jib_x_freq_item = edui_last_item;
        eduiMenuAddItem(ptljibmenu, eduiItemSliderCreate(0, &font, 0, cbPtlApplyJib, 0.0f, 1000.0f,
                                                         dt->jibxamp, "Jibber X Amp"));
        grad_jib_x_amp_item = edui_last_item;
        eduiMenuAddItem(ptljibmenu, eduiItemSliderCreate(0, &font, 0, cbPtlApplyJib, 0.0f, 32.0f,
                                                         dt->jibyfreq, "Jibber Y Freq"));
        grad_jib_y_freq_item = edui_last_item;
        eduiMenuAddItem(ptljibmenu, eduiItemSliderCreate(0, &font, 0, cbPtlApplyJib, 0.0f, 1000.0f,
                                                         dt->jibyamp, "Jibber Y Amp"));
        grad_jib_y_amp_item = edui_last_item;
        eduiMenuAttach(menu, ptljibmenu);
        ptljibmenu->x = menu->x + 10;
        ptljibmenu->y = menu->y + 40;
    }
}


void edptlcbBounceMenu(struct eduimenu_s *menu) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};
    struct debkeydatatype_s *deb;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    edptl_bounce_menu = eduiMenuCreate(0x46, 0x46, 0xC8, 0x12C, app_fnt, edptlcbCancelBounceMenu,
                                       "Particle Bounce");
    if (edptl_bounce_menu != 0) {
        eduiMenuAddItem(edptl_bounce_menu,
                        eduiItemSliderCreate(0, &font, 0, edptlcbApplyBounceOffset, -2.0f, 4.0f,
                                             deb->refoff, "Plane Offset"));
        eduiMenuAddItem(edptl_bounce_menu,
                        eduiItemSliderCreate(0, &font, 0, edptlcbApplyBounceFactor, 0.0f, 2.0f,
                                             deb->refbounce, "Bounce Factor"));
        eduiMenuAttach(menu, edptl_bounce_menu);
        edptl_bounce_menu->x = menu->x + 10;
        edptl_bounce_menu->y = menu->y + 40;
    }
}


void edptlcbSoundIDMenu(struct eduimenu_s *menu, struct eduiitem_s *item) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;
    int checked;
    int i;

    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    edptl_soundid_menu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, edptlcbCancelSoundIDMenu,
                                        "Sound ID");
    if (edptl_soundid_menu != 0) {
        eduiMenuAddItem(edptl_soundid_menu,
                        eduiItemCheckCreate((item->value << 16) | 9999, &font,
                                            dt->sounds[item->value].id == -1, 0,
                                            edptlcbSetSoundID, "NONE"));
        for (i = 0; i < edSfxAllCount; i++) {
            checked = 0;
            if (dt->sounds[item->value].id == i) {
                checked = 1;
            }
            eduiMenuAddItem(edptl_soundid_menu,
                            eduiItemCheckCreate((item->value << 16) + i, &font, checked, 1,
                                                edptlcbSetSoundID, edbitsGetSoundName(i)));
            if (checked == 1) {
                edptl_soundid_menu->current = edui_last_item;
            }
        }
        eduiMenuAttach(menu, edptl_soundid_menu);
        edptl_soundid_menu->x = menu->x + 10;
        edptl_soundid_menu->y = menu->y + 40;
    }
}


void edptlcbSoundControlMenu(struct eduimenu_s *menu, struct eduiitem_s *item) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;

    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    edptl_soundcontrol_menu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt,
                                             edptlcbCancelSoundControlMenu, "Sound Control");
    if (edptl_soundcontrol_menu != 0) {
        eduiMenuAddItem(edptl_soundcontrol_menu,
                        eduiItemCheckCreate(item->value << 16, &font,
                                            dt->sounds[item->value].type == 0, 1,
                                            edptlcbSetSoundControl, "Off"));
        if (edui_last_item->on) {
            edptl_soundcontrol_menu->current = edui_last_item;
        }
        eduiMenuAddItem(edptl_soundcontrol_menu,
                        eduiItemCheckCreate((item->value << 16) | 1, &font,
                                            dt->sounds[item->value].type == 1, 1,
                                            edptlcbSetSoundControl, "On Edge"));
        if (edui_last_item->on) {
            edptl_soundcontrol_menu->current = edui_last_item;
        }
        eduiMenuAddItem(edptl_soundcontrol_menu,
                        eduiItemCheckCreate((item->value << 16) | 2, &font,
                                            dt->sounds[item->value].type == 2, 1,
                                            edptlcbSetSoundControl, "Off Edge"));
        if (edui_last_item->on) {
            edptl_soundcontrol_menu->current = edui_last_item;
        }
        eduiMenuAddItem(edptl_soundcontrol_menu,
                        eduiItemCheckCreate((item->value << 16) | 3, &font,
                                            dt->sounds[item->value].type == 3, 1,
                                            edptlcbSetSoundControl, "Per Particle"));
        if (edui_last_item->on) {
            edptl_soundcontrol_menu->current = edui_last_item;
        }
        eduiMenuAddItem(edptl_soundcontrol_menu,
                        eduiItemCheckCreate((item->value << 16) | 4, &font,
                                            dt->sounds[item->value].type == 4, 1,
                                            edptlcbSetSoundControl, "Regular"));
        if (edui_last_item->on) {
            edptl_soundcontrol_menu->current = edui_last_item;
        }
        eduiMenuAttach(menu, edptl_soundcontrol_menu);
        edptl_soundcontrol_menu->x = menu->x + 10;
        edptl_soundcontrol_menu->y = menu->y + 40;
    }
}


void edptlcbSoundXMenu(struct eduimenu_s *menu, struct eduiitem_s *item) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};
    char title[16];
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    sprintf(title, "Sound %d Menu", item->value + 1);
    edptl_soundx_menu = eduiMenuCreate(0x46, 0x46, 0xFA, 0x12C, app_fnt, edptlcbCancelSoundXMenu,
                                       title);
    if (edptl_soundx_menu != 0) {
        eduiMenuAddItem(edptl_soundx_menu, eduiItemSelCreate(item->value, &font, 0, 0,
                                                             edptlcbSoundIDMenu, "Sound ID..."));
        eduiMenuAddItem(edptl_soundx_menu, eduiItemSelCreate(item->value, &font, 0, 0,
                                                             edptlcbSoundControlMenu,
                                                             "Sound Control..."));
        eduiMenuAddItem(edptl_soundx_menu,
                        eduiItemSliderCreateInt(item->value, &font, 0, edptlcbChangeSoundDelay, 0,
                                                0x32, dt->sounds[item->value].delay,
                                                "Sound Delay"));
        eduiMenuAttach(menu, edptl_soundx_menu);
        edptl_soundx_menu->x = menu->x + 10;
        edptl_soundx_menu->y = menu->y + 40;
    }
}


static inline void edptlcbSoundsMenu(struct eduimenu_s *menu) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};
    char label[16];
    int i;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    edptl_sounds_menu = eduiMenuCreate(0x46, 0x46, 0xFA, 0x12C, app_fnt, edptlcbCancelSoundsMenu,
                                       "Attached Sounds");
    if (edptl_sounds_menu != 0) {
        for (i = 0; i < 4; i++) {
            sprintf(label, "Sound %d...", i + 1);
            eduiMenuAddItem(edptl_sounds_menu,
                            eduiItemSelCreate(i, &font, 0, 0, edptlcbSoundXMenu, label));
        }
        eduiMenuAttach(menu, edptl_sounds_menu);
        edptl_sounds_menu->x = menu->x + 10;
        edptl_sounds_menu->y = menu->y + 40;
    }
}


void cbPtlCollMenu(struct eduimenu_s *menu) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;
    float ratio;
    int i;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    collmenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0x12C, app_fnt, cbPtlCancelCollMenu,
                              "Collision Spheres");
    if (collmenu != 0) {
        eduiMenuAddItem(collmenu, eduiItemGreyGradPickCreate(0, &font, cbPtlApplyCollEnv,
                                                             "Collision Envelope"));
        coll_env_item = edui_last_item;
        for (i = 0; i < 8; i++) {
            if (dt->minsize == dt->maxsize) {
                ratio = 1.0f;
            } else {
                ratio = (dt->sphereslot[i].value - dt->minsize / 10000.0f)
                      / ((dt->maxsize - dt->minsize) / 10000.0f);
            }
            eduiGradStageAddRGB(coll_env_item, dt->sphereslot[i].t, ratio, ratio, ratio);
            if (dt->sphereslot[i].t == 1.0f) {
                break;
            }
        }
        eduiMenuAddItem(collmenu, eduiItemSliderCreateInt(0, &font, 0, cbChangeNumCollSpheres, 0, 8,
                                                          dt->numspheres, "Num Spheres"));
        eduiMenuAddItem(collmenu, eduiItemSelCreate(1, &font, 0, 0, cbPtlDefaultCollEnv,
                                                    "Default Envelope"));
        eduiMenuAttach(menu, collmenu);
        collmenu->x = menu->x + 10;
        collmenu->y = menu->y + 40;
    }
}


void cbPtlSizeMenu(struct eduimenu_s *menu) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;
    float ratio;
    int i;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    ptlsizemenu = eduiMenuCreate(0x46, 0x46, 0xDC, 0x12C, app_fnt, cbPtlCancelSizeMenu,
                                 "Particle Size");
    if (ptlsizemenu != 0) {
        eduiMenuAddItem(ptlsizemenu, eduiItemGreyGradPickCreate(0, &font, cbPtlApplySize,
                                                                "Width Envelope"));
        grad_size_w_item = edui_last_item;
        for (i = 0; i < 8; i++) {
            if (dt->minsize == dt->maxsize) {
                ratio = 1.0f;
            } else {
                ratio = (dt->wslot[i].value - dt->minsize) / (dt->maxsize - dt->minsize);
            }
            eduiGradStageAddRGB(grad_size_w_item, dt->wslot[i].t, ratio, ratio, ratio);
            if (dt->wslot[i].t == 1.0f) {
                break;
            }
        }
        eduiMenuAddItem(ptlsizemenu, eduiItemGreyGradPickCreate(0, &font, cbPtlApplySize,
                                                                "Height Envelope"));
        grad_size_h_item = edui_last_item;
        for (i = 0; i < 8; i++) {
            if (dt->minsize == dt->maxsize) {
                ratio = 1.0f;
            } else {
                ratio = (dt->hslot[i].value - dt->minsize) / (dt->maxsize - dt->minsize);
            }
            eduiGradStageAddRGB(grad_size_h_item, dt->hslot[i].t, ratio, ratio, ratio);
            if (dt->hslot[i].t == 1.0f) {
                break;
            }
        }
        eduiMenuAddItem(ptlsizemenu, eduiItemSliderCreate(0, &font, 0, cbPtlApplySize, 0.0f,
                                                          superscale * 5000.0, dt->minsize,
                                                          "Min Size"));
        grad_size_min_item = edui_last_item;
        eduiItemSliderSetFmt(edui_last_item, "(%1.01f)");
        eduiItemSliderSetGranularity(edui_last_item, 0.1f);
        eduiMenuAddItem(ptlsizemenu, eduiItemSliderCreate(0, &font, 0, cbPtlApplySize, 0.0f,
                                                          superscale * 5000.0, dt->maxsize,
                                                          "Max Size"));
        grad_size_max_item = edui_last_item;
        eduiItemSliderSetFmt(edui_last_item, "(%1.01f)");
        eduiItemSliderSetGranularity(edui_last_item, 0.1f);
        eduiMenuAddItem(ptlsizemenu, eduiItemSelCreate(1, &font, 0, 0, cbPtlCopySize,
                                                       "Copy Width to Height"));
        eduiMenuAddItem(ptlsizemenu, eduiItemSelCreate(2, &font, 0, 0, cbPtlCopySize,
                                                       "Copy Height to Width"));
        eduiMenuAttach(menu, ptlsizemenu);
        ptlsizemenu->x = menu->x + 10;
        ptlsizemenu->y = menu->y + 40;
    }
}


void cbPtlRotMenu(struct eduimenu_s *menu) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;
    float ratio;
    int i;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    ptlrotmenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0x12C, app_fnt, cbPtlCancelRotMenu,
                                "Particle Rotation");
    if (ptlrotmenu != 0) {
        eduiMenuAddItem(ptlrotmenu, eduiItemGreyGradPickCreate(0, &font, cbPtlApplyRot,
                                                               "Rotation Envelope"));
        grad_rot_item = edui_last_item;
        for (i = 0; i < 8; i++) {
            if (dt->minrot == dt->maxrot) {
                ratio = 1.0f;
            } else {
                ratio = (dt->rotslot[i].value * (360.0f / 65536.0f) - dt->minrot)
                      / (dt->maxrot - dt->minrot);
            }
            eduiGradStageAddRGB(grad_rot_item, dt->rotslot[i].t, ratio, ratio, ratio);
            if (dt->rotslot[i].t == 1.0f) {
                break;
            }
        }
        eduiMenuAddItem(ptlrotmenu, eduiItemSliderCreate(0, &font, 0, cbPtlApplyRot, -720.0f,
                                                         1440.0f, dt->minrot, "Min Rotation"));
        grad_rot_min_item = edui_last_item;
        eduiItemSliderSetFmt(edui_last_item, "(%1.01f)");
        eduiItemSliderSetGranularity(edui_last_item, 0.1f);
        eduiMenuAddItem(ptlrotmenu, eduiItemSliderCreate(0, &font, 0, cbPtlApplyRot, -720.0f,
                                                         1440.0f, dt->maxrot, "Max Rotation"));
        grad_rot_max_item = edui_last_item;
        eduiItemSliderSetFmt(edui_last_item, "(%1.01f)");
        eduiItemSliderSetGranularity(edui_last_item, 0.1f);
        eduiMenuAttach(menu, ptlrotmenu);
        ptlrotmenu->x = menu->x + 10;
        ptlrotmenu->y = menu->y + 40;
    }
}


void cbPtlCutOffMenu(struct eduimenu_s *menu) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    ptlcutoffmenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0x12C, app_fnt, cbPtlCancelCutOffMenu,
                                   "CutOff/On Radii");
    if (ptlcutoffmenu != 0) {
        eduiMenuAddItem(ptlcutoffmenu, eduiItemSliderCreate(0, &font, 0, cbPtlChangeCutOff, 0.0f,
                                                            superscale * 25.0, dt->cutoff,
                                                            "CutOff Rad"));
        eduiMenuAddItem(ptlcutoffmenu, eduiItemSliderCreate(0, &font, 0, cbPtlChangeCutOn, 0.0f,
                                                            superscale * 25.0, dt->cuton,
                                                            "CutOn Rad"));
        eduiMenuAddItem(ptlcutoffmenu, eduiItemSliderCreate(0, &font, 0, cbPtlChangeDrawCutOff,
                                                            0.0f, superscale * 25.0,
                                                            dt->drawcutoff, "DrawCutOff Rad"));
        eduiMenuAttach(menu, ptlcutoffmenu);
        ptlcutoffmenu->x = menu->x + 10;
        ptlcutoffmenu->y = menu->y + 40;
    }
}


void cbPtlVarEmitMenu(struct eduimenu_s *menu) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    ptlvaremitmenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0x12C, app_fnt, cbPtlCancelVarEmitMenu,
                                    "Emitter Variation");
    if (ptlvaremitmenu != 0) {
        if (dt->gensort == 0 || dt->gensort == 8 || dt->gensort == 9 || dt->gensort == 10) {
            eduiMenuAddItem(ptlvaremitmenu,
                            eduiItemSliderCreate(0, &font, 0, cbPtlChangeX, 0.0f,
                                                 superscale * 5.0, dt->variable_emit.x,
                                                 "Rand Emit X"));
            eduiMenuAddItem(ptlvaremitmenu,
                            eduiItemSliderCreate(0, &font, 0, cbPtlChangeY, 0.0f,
                                                 superscale * 5.0, dt->variable_emit.y,
                                                 "Rand Emit Y"));
            eduiMenuAddItem(ptlvaremitmenu,
                            eduiItemSliderCreate(0, &font, 0, cbPtlChangeZ, 0.0f,
                                                 superscale * 5.0, dt->variable_emit.z,
                                                 "Rand Emit Z"));
        } else if (dt->gensort == 6) {
            eduiMenuAddItem(ptlvaremitmenu,
                            eduiItemSliderCreate(0, &font, 0, cbPtlChangeX, 0.0f,
                                                 superscale * 5.0, dt->variable_emit.x,
                                                 "Rand Mag X"));
            eduiMenuAddItem(ptlvaremitmenu,
                            eduiItemSliderCreate(0, &font, 0, cbPtlChangeY, 0.0f, 180.0f,
                                                 dt->variable_emit.y * (360.0f / 65536.0f),
                                                 "Rand Rot Y"));
            eduiItemSliderSetFmt(edui_last_item, "(%1.01f)");
            eduiItemSliderSetGranularity(edui_last_item, 0.1f);
            eduiMenuAddItem(ptlvaremitmenu,
                            eduiItemSliderCreate(0, &font, 0, cbPtlChangeZ, 0.0f, 180.0f,
                                                 dt->variable_emit.z * (360.0f / 65536.0f),
                                                 "Rand Rot Z"));
            eduiItemSliderSetFmt(edui_last_item, "(%1.01f)");
            eduiItemSliderSetGranularity(edui_last_item, 0.1f);
        } else if (dt->gensort == 7) {
            eduiMenuAddItem(ptlvaremitmenu,
                            eduiItemSliderCreate(0, &font, 0, cbPtlChangeX, 0.0f,
                                                 superscale * 5.0, dt->variable_emit.x,
                                                 "Rand Mag"));
            eduiMenuAddItem(ptlvaremitmenu,
                            eduiItemSliderCreate(0, &font, 0, cbPtlChangeY, -90.0f, 180.0f,
                                                 dt->variable_emit.y * (360.0f / 65536.0f),
                                                 "Step Rot Y"));
            eduiItemSliderSetFmt(edui_last_item, "(%1.01f)");
            eduiItemSliderSetGranularity(edui_last_item, 0.1f);
            eduiMenuAddItem(ptlvaremitmenu,
                            eduiItemSliderCreate(0, &font, 0, cbPtlChangeZ, -90.0f, 180.0f,
                                                 dt->variable_emit.z * (360.0f / 65536.0f),
                                                 "Step Rot Z"));
            eduiItemSliderSetFmt(edui_last_item, "(%1.01f)");
            eduiItemSliderSetGranularity(edui_last_item, 0.1f);
        }
        eduiMenuAttach(menu, ptlvaremitmenu);
        ptlvaremitmenu->x = menu->x + 10;
        ptlvaremitmenu->y = menu->y + 40;
    }
}


void cbPtlVarStartMenu(struct eduimenu_s *menu) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    if (dt->gensort == 0 || dt->gensort == 8 || dt->gensort == 9 || dt->gensort == 10) {
        ptlvarstartmenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0x12C, app_fnt, cbPtlCancelVarStartMenu,
                                         "Random Start");
    } else if (dt->gensort == 6 || dt->gensort == 7) {
        ptlvarstartmenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0x12C, app_fnt, cbPtlCancelVarStartMenu,
                                         "Base Emit Vector");
    }
    if (ptlvarstartmenu != 0) {
        if (dt->gensort == 0 || dt->gensort == 8 || dt->gensort == 9 || dt->gensort == 10) {
            eduiMenuAddItem(ptlvarstartmenu,
                            eduiItemSliderCreate(0, &font, 0, cbPtlChangeX, 0.0f,
                                                 superscale * 5.0, dt->variable_start.x,
                                                 "Rand Start X"));
            eduiMenuAddItem(ptlvarstartmenu,
                            eduiItemSliderCreate(0, &font, 0, cbPtlChangeY, 0.0f,
                                                 superscale * 5.0, dt->variable_start.y,
                                                 "Rand Start Y"));
            eduiMenuAddItem(ptlvarstartmenu,
                            eduiItemSliderCreate(0, &font, 0, cbPtlChangeZ, 0.0f,
                                                 superscale * 5.0, dt->variable_start.z,
                                                 "Rand Start Z"));
        } else if (dt->gensort == 6 || dt->gensort == 7) {
            eduiMenuAddItem(ptlvarstartmenu,
                            eduiItemSliderCreate(0, &font, 0, cbPtlChangeX, 0.0f,
                                                 superscale * 5.0, dt->variable_start.x,
                                                 "Base Mag"));
            eduiMenuAddItem(ptlvarstartmenu,
                            eduiItemSliderCreate(0, &font, 0, cbPtlChangeY, -180.0f, 360.0f,
                                                 dt->variable_start.y * (360.0f / 65536.0f),
                                                 "Base Rot Y"));
            eduiItemSliderSetFmt(edui_last_item, "(%1.01f)");
            eduiItemSliderSetGranularity(edui_last_item, 0.1f);
            eduiMenuAddItem(ptlvarstartmenu,
                            eduiItemSliderCreate(0, &font, 0, cbPtlChangeZ, -180.0f, 180.0f,
                                                 dt->variable_start.z * (360.0f / 65536.0f),
                                                 "Base Rot Z"));
            eduiItemSliderSetFmt(edui_last_item, "(%1.01f)");
            eduiItemSliderSetGranularity(edui_last_item, 0.1f);
        }
        eduiMenuAttach(menu, ptlvarstartmenu);
        ptlvarstartmenu->x = menu->x + 10;
        ptlvarstartmenu->y = menu->y + 40;
    }
}


static inline void cbPtlSScaleMenu(struct eduimenu_s *menu) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};

    sscalemenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0x12C, app_fnt, cbPtlCancelSScaleMenu,
                                "Super Scale");
    if (sscalemenu != 0) {
        eduiMenuAddItem(sscalemenu, eduiItemSliderCreateInt(0, &font, 0, cbPtlChangeSScale, 1, 0x13,
                                                            superscale, "Super Scale"));
        eduiMenuAttach(menu, sscalemenu);
        sscalemenu->x = menu->x + 10;
        sscalemenu->y = menu->y + 40;
    }
}


void cbPtlTextureSelectMenu(struct eduimenu_s *menu) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;
    struct eduiitem_s *item;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    if (textureselectmenu == 0) {
        textureselectmenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0x12C, app_fnt, 0, "Texture Select");
        if (textureselectmenu == 0) {
            return;
        }
        eduiMenuAddItem(textureselectmenu,
                        eduiItemTexturePickCreate(0, &font, cbPtlChangeTextureSelect,
                                                  "Texture Select"));
        item = (struct eduiitem_s *)edui_last_item;
        item->u.texpick.mtl = DebMat[1];
        item->u.texpick.u[0] = (dt->u1 - 524288.0f) * (1.0f / 256.0f);
        item->u.texpick.v[0] = (dt->v1 - 524288.0f) * (1.0f / 256.0f);
        item->u.texpick.u[1] = (dt->u2 - 524288.0f) * (1.0f / 256.0f);
        item->u.texpick.v[1] = (dt->v2 - 524288.0f) * (1.0f / 256.0f);
    }
    if (textureselectmenu != 0) {
        eduiMenuAttach(menu, textureselectmenu);
        textureselectmenu->x = menu->x + 10;
        textureselectmenu->y = menu->y + 40;
    }
}


void cbPtlTextureMenu(struct eduimenu_s *menu) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    texturemenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0x12C, app_fnt, cbPtlCancelTextureMenu,
                                 "Texture");
    if (texturemenu != 0) {
        eduiMenuAddItem(texturemenu, eduiItemCheckCreate(0, &font, dt->mat == 0, 1,
                                                         cbPtlSelTextureType, "Addative"));
        eduiMenuAddItem(texturemenu, eduiItemCheckCreate(2, &font, dt->mat == 2, 1,
                                                         cbPtlSelTextureType, "Modulation"));
        eduiMenuAddItem(texturemenu, eduiItemCheckCreate(3, &font, dt->mat == 3, 1,
                                                         cbPtlSelTextureType, "Subtractive"));
        eduiMenuAddItem(texturemenu, eduiItemSelCreate(1, &font, 0, 0, cbPtlTextureSelectMenu,
                                                       "Texture Selector..."));
        eduiMenuAttach(menu, texturemenu);
        texturemenu->x = menu->x + 10;
        texturemenu->y = menu->y + 40;
    }
}


void cbPtlEmitTimeMenu(struct eduimenu_s *menu) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    emittimemenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0x12C, app_fnt, cbPtlCancelEmitTimeMenu,
                                  "Emitter Timing");
    if (emittimemenu != 0) {
        eduiMenuAddItem(emittimemenu,
                        eduiItemSliderCreateInt(0, &font, 0, cbPtlChangeIvalOn, 1,
                                                superscale * 120.0 - 1.0, dt->ival_on, "On Time"));
        eduiMenuAddItem(emittimemenu,
                        eduiItemSliderCreateInt(0, &font, 0, cbPtlChangeIvalOnRan, 0,
                                                superscale * 120.0, dt->ival_on_ran,
                                                "Random On Time"));
        eduiMenuAddItem(emittimemenu,
                        eduiItemSliderCreateInt(0, &font, 0, cbPtlChangeIvalOff, 0,
                                                superscale * 120.0, dt->ival_off, "Off Time"));
        eduiMenuAddItem(emittimemenu,
                        eduiItemSliderCreateInt(0, &font, 0, cbPtlChangeIvalOffRan, 0,
                                                superscale * 120.0, dt->ival_off_ran,
                                                "Random Off Time"));
        eduiMenuAttach(menu, emittimemenu);
        emittimemenu->x = menu->x + 10;
        emittimemenu->y = menu->y + 40;
    }
}


void cbPtlEmitVelMenu(struct eduimenu_s *menu) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    ptlemitvelmenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, cbPtlCancelEmitVelMenu,
                                    "Emitter Vel");
    if (ptlemitvelmenu != 0) {
        eduiMenuAddItem(ptlemitvelmenu, eduiItemSliderCreate(0, &font, 0, cbPtlChangeEmitVel, 0.0f,
                                                             superscale * 10.0, dt->emitmag,
                                                             "Emitter Vel"));
        eduiMenuAttach(menu, ptlemitvelmenu);
        ptlemitvelmenu->x = menu->x + 10;
        ptlemitvelmenu->y = menu->y + 40;
    }
}


void cbPtlGravMenu(struct eduimenu_s *menu) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    ptlgravmenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, cbPtlCancelGravMenu, "Gravity");
    if (ptlgravmenu != 0) {
        eduiMenuAddItem(ptlgravmenu, eduiItemSliderCreate(0, &font, 0, cbPtlChangeGrav,
                                                          superscale * -10.0,
                                                          superscale * 20.0, dt->grav, "Gravity"));
        eduiMenuAttach(menu, ptlgravmenu);
        ptlgravmenu->x = menu->x + 10;
        ptlgravmenu->y = menu->y + 40;
    }
}


static inline void cbMemCardErase(struct eduimenu_s *menu) {
    struct eduimenu_s *arg;

    arg = 0;
    if (menu->parent != 0) {
        arg = menu->parent;
        eduiMenuDetach(menu);
    }
    if (menu->cancel != 0) {
        menu->cancel(menu, arg);
    }
    CreateMessageMenu((struct eduimenu_s *)arg, "Test message", 0);
}


static inline void cbMemCardEraseConfirm(struct eduimenu_s *menu) {
    CreateConfirmMenu(menu, "Erase Card?", cbMemCardErase);
}


static inline void cbChangeFileNameMenu(struct eduimenu_s *menu) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};

    filenamemenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, cbCancelChangeFileNameMenu,
                                  "File Name");
    if (filenamemenu != 0) {
        eduiMenuAddItem(filenamemenu, eduiItemTextPickCreate(0, &font, cbChangeFileName,
                                                             "File Name"));
        strcpy(edui_last_item->u.textpick.text, mcfilename);
        eduiMenuAttach(menu, filenamemenu);
        filenamemenu->x = menu->x + 10;
        filenamemenu->y = menu->y + 40;
    }
}


int edppSaveEffects(char *filename, signed char list) {
    int numtypes;
    int numptls;
    int i;
    int j;

    TidyAllEffects();
    numtypes = 0;
    numptls = 0;
    for (i = 1; i < effect_types_used; i++) {
        if (effecttypes[i].listid == list || list == 2) {
            numtypes++;
        }
    }
    EdFileSetMedia(1);
    if (EdFileOpen(filename, 1)) {
        EdFileWriteInt(15);
        EdFileWriteInt(numtypes);
        for (i = 1; i < effect_types_used; i++) {
            if (effecttypes[i].listid == list || list == 2) {
                EdFileWrite(effecttypes[i].id, 0x10);
                EdFileWriteShort(effecttypes[i].generate);
                EdFileWriteShort(effecttypes[i].debnum);
                EdFileWriteShort(effecttypes[i].ival_offset);
                EdFileWriteShort(effecttypes[i].ival_on);
                EdFileWriteShort(effecttypes[i].ival_on_ran);
                EdFileWriteShort(effecttypes[i].ival_off);
                EdFileWriteShort(effecttypes[i].ival_off_ran);
                EdFileWriteChar(effecttypes[i].gensort);
                EdFileWriteChar(effecttypes[i].gencode);
                EdFileWriteChar(effecttypes[i].mat);
                EdFileWriteChar(effecttypes[i].efftab);
                EdFileWriteFloat(effecttypes[i].radius);
                EdFileWriteFloat(effecttypes[i].cuton);
                EdFileWriteFloat(effecttypes[i].cutoff);
                EdFileWriteFloat(effecttypes[i].drawcutoff);
                EdFileWriteFloat(effecttypes[i].emitmag);
                EdFileWrite(&effecttypes[i].variable_emit, 0xC);
                EdFileWrite(&effecttypes[i].variable_emit_ranscale, 0xC);
                EdFileWrite(&effecttypes[i].variable_start, 0xC);
                EdFileWrite(&effecttypes[i].variable_start_ranscale, 0xC);
                EdFileWriteFloat(effecttypes[i].pos_range.x);
                EdFileWriteFloat(effecttypes[i].pos_range.y);
                EdFileWriteFloat(effecttypes[i].pos_range.z);
                EdFileWriteFloat(effecttypes[i].pos_base.x);
                EdFileWriteFloat(effecttypes[i].pos_base.y);
                EdFileWriteFloat(effecttypes[i].pos_base.z);
                EdFileWriteFloat(effecttypes[i].mom_range.x);
                EdFileWriteFloat(effecttypes[i].mom_range.y);
                EdFileWriteFloat(effecttypes[i].mom_range.z);
                EdFileWriteFloat(effecttypes[i].mom_base.x);
                EdFileWriteFloat(effecttypes[i].mom_base.y);
                EdFileWriteFloat(effecttypes[i].mom_base.z);
                EdFileWriteFloat(effecttypes[i].grav);
                EdFileWriteFloat(effecttypes[i].etime);
                EdFileWriteShort(effecttypes[i].uw);
                EdFileWriteChar(effecttypes[i].frames);
                EdFileWriteChar(effecttypes[i].rate);
                EdFileWriteFloat(effecttypes[i].uvanim);
                EdFileWriteFloat(effecttypes[i].jibxfreq);
                EdFileWriteFloat(effecttypes[i].jibxamp);
                EdFileWriteFloat(effecttypes[i].jibyfreq);
                EdFileWriteFloat(effecttypes[i].jibyamp);
                for (j = 0; j < 8; j++) {
                    EdFileWrite(&effecttypes[i].colslot[j], 0x10);
                }
                for (j = 0; j < 8; j++) {
                    EdFileWrite(&effecttypes[i].alpslot[j], 8);
                }
                EdFileWriteFloat(effecttypes[i].minsize);
                EdFileWriteFloat(effecttypes[i].maxsize);
                for (j = 0; j < 8; j++) {
                    EdFileWrite(&effecttypes[i].wslot[j], 8);
                }
                for (j = 0; j < 8; j++) {
                    EdFileWrite(&effecttypes[i].hslot[j], 8);
                }
                EdFileWriteFloat(effecttypes[i].minrot);
                EdFileWriteFloat(effecttypes[i].maxrot);
                for (j = 0; j < 8; j++) {
                    EdFileWrite(&effecttypes[i].rotslot[j], 8);
                }
                for (j = 0; j < 8; j++) {
                    EdFileWrite(&effecttypes[i].jibxslot[j], 8);
                }
                for (j = 0; j < 8; j++) {
                    EdFileWrite(&effecttypes[i].jibyslot[j], 8);
                }
                EdFileWriteFloat(effecttypes[i].u1);
                EdFileWriteFloat(effecttypes[i].v1);
                EdFileWriteFloat(effecttypes[i].u2);
                EdFileWriteFloat(effecttypes[i].v2);
                for (j = 0; j < 8; j++) {
                    EdFileWrite(&effecttypes[i].sphereslot[j], 8);
                }
                EdFileWriteChar(effecttypes[i].numspheres);
                for (j = 0; j < 4; j++) {
                    EdFileWrite(&effecttypes[i].sounds[j], 0xC);
                }
            }
        }
        if (list == 1 || list == 2) {
            for (i = 0; i < 256; i++) {
                if (edpp_ptls[i].debris != -1) {
                    numptls++;
                }
            }
            EdFileWriteInt(numptls);
            for (i = 0; i < 256; i++) {
                if (edpp_ptls[i].debris != -1) {
                    EdFileWrite(&edpp_ptls[i].pos, 0xC);
                    EdFileWriteShort(edpp_ptls[i].rotz);
                    EdFileWriteShort(edpp_ptls[i].roty);
                    EdFileWriteShort(edpp_ptls[i].emitrotz);
                    EdFileWriteShort(edpp_ptls[i].emitroty);
                    EdFileWriteInt(edpp_ptls[i].offset);
                    EdFileWrite(debtab[edpp_ptls[i].type]->id, 0x10);
                    EdFileWriteInt(edpp_ptls[i].switchtype);
                    EdFileWriteInt(edpp_ptls[i].switchid);
                    EdFileWriteFloat(edpp_ptls[i].switchvar);
                    EdFileWriteShort(edpp_ptls[i].refrotz);
                    EdFileWriteShort(edpp_ptls[i].refroty);
                    EdFileWriteFloat(edpp_ptls[i].bounceoffset);
                    EdFileWriteFloat(edpp_ptls[i].bouncefactor);
                    EdFileWriteShort(edpp_ptls[i].group);
                }
            }
        }
        EdFileClose();
        return 1;
    }
    return 0;
}


void cbFileSaveEffects(struct eduimenu_s *menu) {
    char filename[256];

    if (levelfilenametype == 1) {
        if (edpp_effect_list == 0) {
            sprintf(filename, "Generic\\Particle.dat");
        } else {
            sprintf(filename, "%s\\Particle.dat", levelfilename);
        }
        if (edppSaveEffects(filename, edpp_effect_list)) {
            CreateMessageMenu(menu, "Saved OK", 1);
        } else {
            CreateMessageMenu(menu, "File Save Error", 0);
        }
    } else if (levelfilenametype == 2) {
        if (edpp_effect_list == 0) {
            sprintf(filename, "stuff\\general.ptl");
        } else if (edpp_effect_list == 1) {
            sprintf(filename, "%s.ptl", levelfilename);
        } else if (edpp_effect_list == 5) {
            sprintf(filename, "%s.cpt", levelfilename);
        } else {
            CreateMessageMenu(menu, "File Save Error", 0);
        }
        if (edppSaveEffects(filename, edpp_effect_list)) {
            CreateMessageMenu(menu, "Saved OK", 1);
        } else {
            CreateMessageMenu(menu, "File Save Error", 0);
        }
    } else {
        CreateMessageMenu(menu, "File Save Error", 0);
    }
}


void FileLoadSingleEffectType(struct debinfo_s *dt, int version, signed char list) {
    int i;

    EdFileRead(dt->id, 0x10);
    dt->listid = list;
    dt->generate = EdFileReadShort();
    dt->debnum = EdFileReadShort();
    dt->ival_offset = EdFileReadShort();
    dt->ival_on = EdFileReadShort();
    dt->ival_on_ran = EdFileReadShort();
    dt->ival_off = EdFileReadShort();
    dt->ival_off_ran = EdFileReadShort();
    dt->gensort = EdFileReadChar();
    dt->gencode = EdFileReadChar();
    dt->mat = EdFileReadChar();
    dt->efftab = EdFileReadChar();
    dt->radius = EdFileReadFloat();
    if (version < 6) {
        dt->cuton = 0.0f;
        dt->cutoff = 25.0f;
    } else {
        dt->cuton = EdFileReadFloat();
        dt->cutoff = EdFileReadFloat();
    }
    if (version < 10) {
        dt->drawcutoff = 0.0f;
    } else {
        dt->drawcutoff = EdFileReadFloat();
    }
    if (version < 7) {
        EdFileReadInt();
        EdFileReadInt();
    }
    dt->emitmag = EdFileReadFloat();
    EdFileRead(&dt->variable_emit, 0xC);
    EdFileRead(&dt->variable_emit_ranscale, 0xC);
    EdFileRead(&dt->variable_start, 0xC);
    EdFileRead(&dt->variable_start_ranscale, 0xC);
    dt->pos_range.x = EdFileReadFloat();
    dt->pos_range.y = EdFileReadFloat();
    dt->pos_range.z = EdFileReadFloat();
    dt->pos_base.x = EdFileReadFloat();
    dt->pos_base.y = EdFileReadFloat();
    dt->pos_base.z = EdFileReadFloat();
    dt->mom_range.x = EdFileReadFloat();
    dt->mom_range.y = EdFileReadFloat();
    dt->mom_range.z = EdFileReadFloat();
    dt->mom_base.x = EdFileReadFloat();
    dt->mom_base.y = EdFileReadFloat();
    dt->mom_base.z = EdFileReadFloat();
    dt->grav = EdFileReadFloat();
    dt->etime = EdFileReadFloat();
    dt->uw = EdFileReadShort();
    dt->frames = EdFileReadChar();
    dt->rate = EdFileReadChar();
    dt->uvanim = EdFileReadFloat();
    dt->jibxfreq = EdFileReadFloat();
    dt->jibxamp = EdFileReadFloat();
    dt->jibyfreq = EdFileReadFloat();
    dt->jibyamp = EdFileReadFloat();
    for (i = 0; i < 8; i++) {
        EdFileRead(&dt->colslot[i], 0x10);
    }
    for (i = 0; i < 8; i++) {
        EdFileRead(&dt->alpslot[i], 8);
    }
    dt->minsize = EdFileReadFloat();
    dt->maxsize = EdFileReadFloat();
    for (i = 0; i < 8; i++) {
        EdFileRead(&dt->wslot[i], 8);
    }
    for (i = 0; i < 8; i++) {
        EdFileRead(&dt->hslot[i], 8);
    }
    dt->minrot = EdFileReadFloat();
    dt->maxrot = EdFileReadFloat();
    for (i = 0; i < 8; i++) {
        EdFileRead(&dt->rotslot[i], 8);
    }
    for (i = 0; i < 8; i++) {
        EdFileRead(&dt->jibxslot[i], 8);
    }
    for (i = 0; i < 8; i++) {
        EdFileRead(&dt->jibyslot[i], 8);
    }
    dt->u1 = EdFileReadFloat();
    dt->v1 = EdFileReadFloat();
    dt->u2 = EdFileReadFloat();
    dt->v2 = EdFileReadFloat();
    if (version > 2) {
        for (i = 0; i < 8; i++) {
            EdFileRead(&dt->sphereslot[i], 8);
        }
        dt->numspheres = EdFileReadChar();
    } else {
        dt->numspheres = 0;
    }
    for (i = 0; i < 4; i++) {
        if (version > 10) {
            EdFileRead(&dt->sounds[i], 0xC);
        } else {
            dt->sounds[i].id = -1;
            dt->sounds[i].type = 0;
            dt->sounds[i].delay = 0;
        }
    }
}


int edppLoadEffects(char *filename, signed char list) {
    int current_version;
    int version;
    int numptls;
    int count;
    int i;

    current_version = 15;
    EdFileSetMedia(1);
    if (!EdFileOpen(filename, 0)) {
        return 0;
    }
    version = EdFileReadInt();
    if (version > current_version) {
        EdFileClose();
        return 0;
    }
    if (version > 1) {
        effect_types_used = EdFileReadInt();
    } else {
        effect_types_used = 128;
    }
    if (version > 3) {
        effect_types_used++;
    }
    if (version < 4) {
        for (i = 0; i < effect_types_used; i++) {
            FileLoadSingleEffectType(&effecttypes[i], version, list);
        }
    } else {
        for (i = 1; i < effect_types_used; i++) {
            FileLoadSingleEffectType(&effecttypes[i], version, list);
        }
    }
    for (i = effect_types_used; i < 128; i++) {
        effecttypes[i] = effecttypes[0];
    }
    if (version < 4) {
        EdFileRead(debtab, 0x200);
        effect_types_used = 0;
        for (i = 0; i < 128; i++) {
            if (debtab[i] != 0) {
                debtab[i] = &effecttypes[i];
                effect_types_used++;
                UpdateTotalPtls(&effecttypes[i]);
            }
        }
    } else {
        for (i = 0; i < 128; i++) {
            if (i < effect_types_used) {
                debtab[i] = &effecttypes[i];
                UpdateTotalPtls(&effecttypes[i]);
            } else {
                debtab[i] = 0;
            }
        }
    }
    if (list == 1 || list == 2) {
        ParticleReset();
        if (version < 5) {
            for (i = 0; i < 256; i++) {
                EdFileRead(&edpp_ptls[i].pos, 0xC);
                edpp_ptls[i].type = EdFileReadInt();
                edpp_ptls[i].debris = EdFileReadInt();
                edpp_ptls[i].emitrotz = EdFileReadInt();
                edpp_ptls[i].emitroty = EdFileReadInt();
                if (debtab[edpp_ptls[i].type] == 0) {
                    edpp_ptls[i].name[0] = 0;
                } else {
                    memcpy(edpp_ptls[i].name, debtab[edpp_ptls[i].type]->id, 0x10);
                }
                edpp_ptls[i].offset = 0;
                edpp_ptls[i].switchtype = 0;
                edpp_ptls[i].switchid = -1;
                edpp_ptls[i].switchvar = 0.0f;
            }
        } else {
            numptls = EdFileReadInt();
            count = (numptls > 256) ? 256 : numptls;
            for (i = 0; i < count; i++) {
                EdFileRead(&edpp_ptls[i].pos, 0xC);
                if (version < 7) {
                    edpp_ptls[i].rotz = 0;
                    edpp_ptls[i].roty = 0;
                    edpp_ptls[i].emitrotz = EdFileReadInt();
                    edpp_ptls[i].emitroty = EdFileReadInt();
                } else {
                    edpp_ptls[i].rotz = EdFileReadShort();
                    edpp_ptls[i].roty = EdFileReadShort();
                    edpp_ptls[i].emitrotz = EdFileReadShort();
                    edpp_ptls[i].emitroty = EdFileReadShort();
                }
                if (version < 8) {
                    edpp_ptls[i].offset = 0;
                } else {
                    edpp_ptls[i].offset = EdFileReadInt();
                }
                EdFileRead(edpp_ptls[i].name, 0x10);
                edpp_ptls[i].debris = i;
                edpp_ptls[i].type = LookupDebrisEffect(edpp_ptls[i].name);
                if (version > 8) {
                    edpp_ptls[i].switchtype = EdFileReadInt();
                    edpp_ptls[i].switchid = EdFileReadInt();
                    edpp_ptls[i].switchvar = EdFileReadFloat();
                } else {
                    edpp_ptls[i].switchtype = 0;
                    edpp_ptls[i].switchid = -1;
                    edpp_ptls[i].switchvar = 0.0f;
                }
                if (version > 11) {
                    edpp_ptls[i].refrotz = EdFileReadShort();
                    edpp_ptls[i].refroty = EdFileReadShort();
                    edpp_ptls[i].bounceoffset = EdFileReadFloat();
                } else {
                    edpp_ptls[i].refrotz = 0;
                    edpp_ptls[i].refroty = 0;
                    edpp_ptls[i].bounceoffset = 0.0f;
                }
                if (version > 12) {
                    edpp_ptls[i].bouncefactor = EdFileReadFloat();
                } else {
                    edpp_ptls[i].bouncefactor = 0.9f;
                }
                if (version > 14) {
                    edpp_ptls[i].group = EdFileReadShort();
                } else {
                    edpp_ptls[i].group = 0;
                }
            }
        }
    }
    EdFileClose();
    for (i = 0; i < 128; i++) {
        effecttypes[i].DmaDebTypePointer = 0;
        effecttypes[i].variable_key = -1;
    }
    freeDmaDebType = 0;
    edppDetermineNearest(1.0f);
    return 1;
}


void edppRestartAllEffectsInLevel(void) {
    int i;

    for (i = 0; i < 256; i++) {
        if (edpp_ptls[i].debris != -1) {
            edpp_ptls[i].debris = -1;
            edpp_ptls[i].type = LookupDebrisEffect(edpp_ptls[i].name);
            AddDebrisEffect(&edpp_ptls[i].debris, edpp_ptls[i].type, edpp_ptls[i].pos.x,
                            edpp_ptls[i].pos.y, edpp_ptls[i].pos.z);
            if (edpp_ptls[i].debris != -1) {
                DebrisOrientation(edpp_ptls[i].debris, edpp_ptls[i].rotz, edpp_ptls[i].roty);
                DebrisEmitterOrientation(edpp_ptls[i].debris, edpp_ptls[i].emitrotz,
                                         edpp_ptls[i].emitroty);
                DebrisStartOffset(edpp_ptls[i].debris, edpp_ptls[i].offset);
                DebrisSetTrigger(edpp_ptls[i].debris, edpp_ptls[i].switchtype,
                                 edpp_ptls[i].switchid, edpp_ptls[i].switchvar);
                DebrisReflectionOrientation(edpp_ptls[i].debris, edpp_ptls[i].bounceoffset,
                                            edpp_ptls[i].bouncefactor, edpp_ptls[i].refrotz,
                                            edpp_ptls[i].refroty);
                DebrisSetGroupID(edpp_ptls[i].debris, edpp_ptls[i].group);
            }
        }
    }
}


int edppMergeEffects(char *filename, signed char list) {
    struct debinfo_s discard;
    int version;
    int count;
    int i;

    TidyAllEffects();
    EdFileSetMedia(1);
    if (!EdFileOpen(filename, 0)) {
        return 0;
    }
    version = EdFileReadInt();
    if (version < 2) {
        EdFileClose();
        return 0;
    }
    count = EdFileReadInt();
    if (version < 4) {
        count--;
        FileLoadSingleEffectType(&discard, version, list);
    }
    if (effect_types_used + count > 128) {
        count = 128 - effect_types_used;
    }
    for (i = effect_types_used; i < effect_types_used + count; i++) {
        FileLoadSingleEffectType(&effecttypes[i], version, list);
    }
    for (i = effect_types_used + count; i < 128; i++) {
        effecttypes[i] = effecttypes[0];
    }
    for (i = effect_types_used; i < effect_types_used + count; i++) {
        debtab[i] = &effecttypes[i];
        UpdateTotalPtls(&effecttypes[i]);
    }
    effect_types_used += count;
    EdFileClose();
    for (i = 0; i < 128; i++) {
        effecttypes[i].DmaDebTypePointer = 0;
        effecttypes[i].variable_key = -1;
    }
    freeDmaDebType = 0;
    return 1;
}


void cbFileLoadEffects(struct eduimenu_s *menu) {
    char filename[256];
    int i;
    int j;

    for (i = 0; i < 256; i++) {
        if (edpp_ptls[i].debris != -1) {
            DebFreeInstantly(&edpp_ptls[i].debris);
            edpp_ptls[i].debris = -1;
        }
    }
    for (j = 1; j < 128; j++) {
        debtab[j] = 0;
    }
    effect_types_used = 1;
    if (levelfilenametype == 1) {
        sprintf(filename, "%s\\Particle.dat", levelfilename);
        if (NuFileSize(filename) > 0) {
            edppLoadEffects(filename, 1);
        }
        sprintf(filename, "Generic\\Particle.dat");
        if (NuFileSize(filename) > 0) {
            edppMergeEffects(filename, 0);
        }
        edppRestartAllEffectsInLevel();
        CreateMessageMenu(menu, "Loaded OK", 1);
    } else if (levelfilenametype == 2) {
        sprintf(filename, "%s.ptl", levelfilename);
        if (NuFileSize(filename) > 0) {
            edppLoadEffects(filename, 1);
        }
        sprintf(filename, "stuff\\general.ptl");
        if (NuFileSize(filename) > 0) {
            edppMergeEffects(filename, 0);
        }
        sprintf(filename, "%s.cpt", levelfilename);
        if (NuFileSize(filename) > 0) {
            edppMergeEffects(filename, 5);
        }
        edppRestartAllEffectsInLevel();
        CreateMessageMenu(menu, "Loaded OK", 1);
    } else {
        CreateMessageMenu(menu, "File Load Error", 0);
    }
}


int edppMemCardSaveEffects(char *filename, signed char list) {
    int numtypes;
    int numptls;
    int i;
    int j;

    TidyAllEffects();
    numtypes = 0;
    numptls = 0;
    for (i = 1; i < effect_types_used; i++) {
        if (effecttypes[i].listid == list || list == 2) {
            numtypes++;
        }
    }
    EdFileSetMedia(2);
    if (!EdFileOpen(filename, 1)) {
        return 0;
    }
    EdFileWriteInt(15);
    EdFileWriteInt(numtypes);
    for (i = 1; i < effect_types_used; i++) {
        if (effecttypes[i].listid == list || list == 2) {
            EdFileWrite(effecttypes[i].id, 0x10);
            EdFileWriteChar(effecttypes[i].listid);
            EdFileWriteShort(effecttypes[i].generate);
            EdFileWriteShort(effecttypes[i].debnum);
            EdFileWriteShort(effecttypes[i].ival_offset);
            EdFileWriteShort(effecttypes[i].ival_on);
            EdFileWriteShort(effecttypes[i].ival_on_ran);
            EdFileWriteShort(effecttypes[i].ival_off);
            EdFileWriteShort(effecttypes[i].ival_off_ran);
            EdFileWriteChar(effecttypes[i].gensort);
            EdFileWriteChar(effecttypes[i].gencode);
            EdFileWriteChar(effecttypes[i].mat);
            EdFileWriteChar(effecttypes[i].efftab);
            EdFileWriteFloat(effecttypes[i].radius);
            EdFileWriteFloat(effecttypes[i].cuton);
            EdFileWriteFloat(effecttypes[i].cutoff);
            EdFileWriteFloat(effecttypes[i].drawcutoff);
            EdFileWriteFloat(effecttypes[i].emitmag);
            EdFileWrite(&effecttypes[i].variable_emit, 0xC);
            EdFileWrite(&effecttypes[i].variable_emit_ranscale, 0xC);
            EdFileWrite(&effecttypes[i].variable_start, 0xC);
            EdFileWrite(&effecttypes[i].variable_start_ranscale, 0xC);
            EdFileWriteFloat(effecttypes[i].pos_range.x);
            EdFileWriteFloat(effecttypes[i].pos_range.y);
            EdFileWriteFloat(effecttypes[i].pos_range.z);
            EdFileWriteFloat(effecttypes[i].pos_base.x);
            EdFileWriteFloat(effecttypes[i].pos_base.y);
            EdFileWriteFloat(effecttypes[i].pos_base.z);
            EdFileWriteFloat(effecttypes[i].mom_range.x);
            EdFileWriteFloat(effecttypes[i].mom_range.y);
            EdFileWriteFloat(effecttypes[i].mom_range.z);
            EdFileWriteFloat(effecttypes[i].mom_base.x);
            EdFileWriteFloat(effecttypes[i].mom_base.y);
            EdFileWriteFloat(effecttypes[i].mom_base.z);
            EdFileWriteFloat(effecttypes[i].grav);
            EdFileWriteFloat(effecttypes[i].etime);
            EdFileWriteShort(effecttypes[i].uw);
            EdFileWriteChar(effecttypes[i].frames);
            EdFileWriteChar(effecttypes[i].rate);
            EdFileWriteFloat(effecttypes[i].uvanim);
            EdFileWriteFloat(effecttypes[i].jibxfreq);
            EdFileWriteFloat(effecttypes[i].jibxamp);
            EdFileWriteFloat(effecttypes[i].jibyfreq);
            EdFileWriteFloat(effecttypes[i].jibyamp);
            for (j = 0; j < 8; j++) {
                EdFileWrite(&effecttypes[i].colslot[j], 0x10);
            }
            for (j = 0; j < 8; j++) {
                EdFileWrite(&effecttypes[i].alpslot[j], 8);
            }
            EdFileWriteFloat(effecttypes[i].minsize);
            EdFileWriteFloat(effecttypes[i].maxsize);
            for (j = 0; j < 8; j++) {
                EdFileWrite(&effecttypes[i].wslot[j], 8);
            }
            for (j = 0; j < 8; j++) {
                EdFileWrite(&effecttypes[i].hslot[j], 8);
            }
            EdFileWriteFloat(effecttypes[i].minrot);
            EdFileWriteFloat(effecttypes[i].maxrot);
            for (j = 0; j < 8; j++) {
                EdFileWrite(&effecttypes[i].rotslot[j], 8);
            }
            for (j = 0; j < 8; j++) {
                EdFileWrite(&effecttypes[i].jibxslot[j], 8);
            }
            for (j = 0; j < 8; j++) {
                EdFileWrite(&effecttypes[i].jibyslot[j], 8);
            }
            EdFileWriteFloat(effecttypes[i].u1);
            EdFileWriteFloat(effecttypes[i].v1);
            EdFileWriteFloat(effecttypes[i].u2);
            EdFileWriteFloat(effecttypes[i].v2);
            for (j = 0; j < 8; j++) {
                EdFileWrite(&effecttypes[i].sphereslot[j], 8);
            }
            EdFileWriteChar(effecttypes[i].numspheres);
            for (j = 0; j < 4; j++) {
                EdFileWrite(&effecttypes[i].sounds[j], 0xC);
            }
        }
    }
    if (list == 2) {
        for (i = 0; i < 256; i++) {
            if (edpp_ptls[i].debris != -1) {
                numptls++;
            }
        }
        EdFileWriteInt(numptls);
        for (i = 0; i < 256; i++) {
            if (edpp_ptls[i].debris != -1) {
                EdFileWrite(&edpp_ptls[i].pos, 0xC);
                EdFileWriteShort(edpp_ptls[i].rotz);
                EdFileWriteShort(edpp_ptls[i].roty);
                EdFileWriteShort(edpp_ptls[i].emitrotz);
                EdFileWriteShort(edpp_ptls[i].emitroty);
                EdFileWriteInt(edpp_ptls[i].offset);
                EdFileWrite(debtab[edpp_ptls[i].type]->id, 0x10);
                EdFileWriteInt(edpp_ptls[i].switchtype);
                EdFileWriteInt(edpp_ptls[i].switchid);
                EdFileWriteFloat(edpp_ptls[i].switchvar);
                EdFileWriteShort(edpp_ptls[i].refrotz);
                EdFileWriteShort(edpp_ptls[i].refroty);
                EdFileWriteFloat(edpp_ptls[i].bounceoffset);
                EdFileWriteFloat(edpp_ptls[i].bouncefactor);
                EdFileWriteShort(edpp_ptls[i].group);
            }
        }
    } else {
        EdFileWriteInt(0);
    }
    return EdFileClose() != 0;
}


void cbMemCardSaveEffects(struct eduimenu_s *menu) {
    char filename[256];

    sceMcGetDir(0, 0, "*.eff", 0, 10, mcdir);
    sceMcSync(0, 0, &mcresult);
    if (mcresult >= 8) {
        CreateMessageMenu(menu, "Too Many Effects Saves", 0);
    } else {
        sprintf(filename, "%s.eff", mcfilename);
        if (edppMemCardSaveEffects(filename, 2)) {
            CreateMessageMenu(menu, "Saved OK", 1);
        } else {
            CreateMessageMenu(menu, "File Save Error", 0);
        }
    }
}


void cbMemCardSaveEffectTypes(struct eduimenu_s *menu) {
    char filename[256];

    sceMcGetDir(0, 0, "*.eft", 0, 10, mcdir);
    sceMcSync(0, 0, &mcresult);
    if (mcresult >= 8) {
        CreateMessageMenu(menu, "Too Many Effect Type Saves", 0);
    } else {
        sprintf(filename, "%s.eft", mcfilename);
        if (edppMemCardSaveEffects(filename, edpp_effect_list)) {
            CreateMessageMenu(menu, "Saved OK", 1);
        } else {
            CreateMessageMenu(menu, "File Save Error", 0);
        }
    }
}


void MemCardLoadSingleEffectType(struct debinfo_s *dt, int version, signed char list) {
    int i;

    EdFileRead(dt->id, 0x10);
    dt->listid = EdFileReadChar();
    if (list != 2) {
        dt->listid = list;
    }
    dt->generate = EdFileReadShort();
    dt->debnum = EdFileReadShort();
    dt->ival_offset = EdFileReadShort();
    dt->ival_on = EdFileReadShort();
    dt->ival_on_ran = EdFileReadShort();
    dt->ival_off = EdFileReadShort();
    dt->ival_off_ran = EdFileReadShort();
    dt->gensort = EdFileReadChar();
    dt->gencode = EdFileReadChar();
    dt->mat = EdFileReadChar();
    dt->efftab = EdFileReadChar();
    dt->radius = EdFileReadFloat();
    dt->cuton = EdFileReadFloat();
    dt->cutoff = EdFileReadFloat();
    if (version < 10) {
        dt->drawcutoff = 0.0f;
    } else {
        dt->drawcutoff = EdFileReadFloat();
    }
    dt->emitmag = EdFileReadFloat();
    EdFileRead(&dt->variable_emit, 0xC);
    EdFileRead(&dt->variable_emit_ranscale, 0xC);
    EdFileRead(&dt->variable_start, 0xC);
    EdFileRead(&dt->variable_start_ranscale, 0xC);
    dt->pos_range.x = EdFileReadFloat();
    dt->pos_range.y = EdFileReadFloat();
    dt->pos_range.z = EdFileReadFloat();
    dt->pos_base.x = EdFileReadFloat();
    dt->pos_base.y = EdFileReadFloat();
    dt->pos_base.z = EdFileReadFloat();
    dt->mom_range.x = EdFileReadFloat();
    dt->mom_range.y = EdFileReadFloat();
    dt->mom_range.z = EdFileReadFloat();
    dt->mom_base.x = EdFileReadFloat();
    dt->mom_base.y = EdFileReadFloat();
    dt->mom_base.z = EdFileReadFloat();
    dt->grav = EdFileReadFloat();
    dt->etime = EdFileReadFloat();
    dt->uw = EdFileReadShort();
    dt->frames = EdFileReadChar();
    dt->rate = EdFileReadChar();
    dt->uvanim = EdFileReadFloat();
    dt->jibxfreq = EdFileReadFloat();
    dt->jibxamp = EdFileReadFloat();
    dt->jibyfreq = EdFileReadFloat();
    dt->jibyamp = EdFileReadFloat();
    for (i = 0; i < 8; i++) {
        EdFileRead(&dt->colslot[i], 0x10);
    }
    for (i = 0; i < 8; i++) {
        EdFileRead(&dt->alpslot[i], 8);
    }
    dt->minsize = EdFileReadFloat();
    dt->maxsize = EdFileReadFloat();
    for (i = 0; i < 8; i++) {
        EdFileRead(&dt->wslot[i], 8);
    }
    for (i = 0; i < 8; i++) {
        EdFileRead(&dt->hslot[i], 8);
    }
    dt->minrot = EdFileReadFloat();
    dt->maxrot = EdFileReadFloat();
    for (i = 0; i < 8; i++) {
        EdFileRead(&dt->rotslot[i], 8);
    }
    for (i = 0; i < 8; i++) {
        EdFileRead(&dt->jibxslot[i], 8);
    }
    for (i = 0; i < 8; i++) {
        EdFileRead(&dt->jibyslot[i], 8);
    }
    dt->u1 = EdFileReadFloat();
    dt->v1 = EdFileReadFloat();
    dt->u2 = EdFileReadFloat();
    dt->v2 = EdFileReadFloat();
    for (i = 0; i < 8; i++) {
        EdFileRead(&dt->sphereslot[i], 8);
    }
    dt->numspheres = EdFileReadChar();
    for (i = 0; i < 4; i++) {
        if (version > 10) {
            EdFileRead(&dt->sounds[i], 0xC);
        } else {
            dt->sounds[i].id = -1;
            dt->sounds[i].type = 0;
            dt->sounds[i].delay = 0;
        }
    }
}


int edppMemCardLoadEffects(char *filename, signed char list) {
    int current_version;
    int version;
    int numptls;
    int count;
    int i;

    current_version = 15;
    EdFileSetMedia(2);
    if (!EdFileOpen(filename, 0)) {
        return 0;
    }
    version = EdFileReadInt();
    if (version < 14 || version > current_version) {
        EdFileClose();
        return 0;
    }
    effect_types_used = EdFileReadInt() + 1;
    for (i = 1; i < effect_types_used; i++) {
        MemCardLoadSingleEffectType(&effecttypes[i], version, list);
    }
    for (i = effect_types_used; i < 128; i++) {
        effecttypes[i] = effecttypes[0];
    }
    for (i = 0; i < 128; i++) {
        if (i < effect_types_used) {
            debtab[i] = &effecttypes[i];
            UpdateTotalPtls(&effecttypes[i]);
        } else {
            debtab[i] = 0;
        }
    }
    if (list == 1 || list == 2) {
        ParticleReset();
        numptls = EdFileReadInt();
        count = (numptls > 256) ? 256 : numptls;
        for (i = 0; i < count; i++) {
            EdFileRead(&edpp_ptls[i].pos, 0xC);
            edpp_ptls[i].rotz = EdFileReadShort();
            edpp_ptls[i].roty = EdFileReadShort();
            edpp_ptls[i].emitrotz = EdFileReadShort();
            edpp_ptls[i].emitroty = EdFileReadShort();
            edpp_ptls[i].offset = EdFileReadInt();
            EdFileRead(edpp_ptls[i].name, 0x10);
            edpp_ptls[i].debris = i;
            edpp_ptls[i].type = LookupDebrisEffect(edpp_ptls[i].name);
            edpp_ptls[i].switchtype = EdFileReadInt();
            edpp_ptls[i].switchid = EdFileReadInt();
            edpp_ptls[i].switchvar = EdFileReadFloat();
            edpp_ptls[i].refrotz = EdFileReadShort();
            edpp_ptls[i].refroty = EdFileReadShort();
            edpp_ptls[i].bounceoffset = EdFileReadFloat();
            edpp_ptls[i].bouncefactor = EdFileReadFloat();
            if (version > 14) {
                edpp_ptls[i].group = EdFileReadShort();
            } else {
                edpp_ptls[i].group = 0;
            }
        }
    }
    EdFileClose();
    for (i = 0; i < 128; i++) {
        effecttypes[i].DmaDebTypePointer = 0;
        effecttypes[i].variable_key = -1;
    }
    freeDmaDebType = 0;
    edppDetermineNearest(1.0f);
    return 1;
}


void cbMemCardLoadEffects(struct eduimenu_s *menu, struct eduiitem_s *item) {
    char filename[256];
    int i;
    int j;

    for (i = 0; i < 256; i++) {
        if (edpp_ptls[i].debris != -1) {
            DebFreeInstantly(&edpp_ptls[i].debris);
            edpp_ptls[i].debris = -1;
        }
    }
    for (j = 1; j < 128; j++) {
        debtab[j] = 0;
    }
    effect_types_used = 1;
    sprintf(filename, mcdir[item->value].name);
    if (edppMemCardLoadEffects(filename, 2)) {
        edppRestartAllEffectsInLevel();
        CreateMessageMenu(menu, "Loaded OK", 1);
    } else {
        CreateMessageMenu(menu, "File Load Error", 0);
    }
}


static inline void cbMemCardLoadEffectsMenu(struct eduimenu_s *menu) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};
    int i;

    sceMcGetDir(0, 0, "*.eff", 0, 10, mcdir);
    sceMcSync(0, 0, &mcresult);
    if (mcresult > 0) {
        loadeffectsmenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt,
                                         cbMemCardCancelLoadEffectsMenu, "Load Effects");
        if (loadeffectsmenu != 0) {
            for (i = 0; i < mcresult; i++) {
                eduiMenuAddItem(loadeffectsmenu, eduiItemSelCreate(i, &font, 0, 0,
                                                                   cbMemCardLoadEffects,
                                                                   mcdir[i].name));
            }
            eduiMenuAttach(menu, loadeffectsmenu);
            loadeffectsmenu->x = menu->x + 10;
            loadeffectsmenu->y = menu->y + 40;
        }
    }
}


int edppMemCardMergeEffects(char *filename, signed char list) {
    int current_version;
    int version;
    int count;
    int i;

    current_version = 14;
    TidyAllEffects();
    EdFileSetMedia(2);
    if (!EdFileOpen(filename, 0)) {
        return 0;
    }
    version = EdFileReadInt();
    if (version < 14 || version > current_version) {
        return 0;
    }
    count = EdFileReadInt();
    if (effect_types_used + count > 128) {
        count = 128 - effect_types_used;
    }
    for (i = effect_types_used; i < effect_types_used + count; i++) {
        MemCardLoadSingleEffectType(&effecttypes[i], version, list);
    }
    for (i = effect_types_used + count; i < 128; i++) {
        effecttypes[i] = effecttypes[0];
    }
    for (i = effect_types_used; i < effect_types_used + count; i++) {
        debtab[i] = &effecttypes[i];
    }
    effect_types_used += count;
    EdFileClose();
    for (i = 0; i < 128; i++) {
        effecttypes[i].DmaDebTypePointer = 0;
    }
    freeDmaDebType = 0;
    return 1;
}


void cbMemCardMergeEffects(struct eduimenu_s *menu, struct eduiitem_s *item) {
    char filename[256];

    sprintf(filename, mcdir[item->value].name);
    if (edppMemCardMergeEffects(filename, edpp_effect_list)) {
        CreateMessageMenu(menu, "Merged OK", 1);
    } else {
        CreateMessageMenu(menu, "File Load Error", 0);
    }
}


static inline void cbMemCardMergeEffectsMenu(struct eduimenu_s *menu) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};
    int i;

    sceMcGetDir(0, 0, "*.eft", 0, 10, mcdir);
    sceMcSync(0, 0, &mcresult);
    if (mcresult > 0) {
        mergeeffectsmenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt,
                                          cbMemCardCancelMergeEffectsMenu, "Merge Effects");
        if (mergeeffectsmenu != 0) {
            for (i = 0; i < mcresult; i++) {
                eduiMenuAddItem(mergeeffectsmenu, eduiItemSelCreate(i, &font, 0, 0,
                                                                    cbMemCardMergeEffects,
                                                                    mcdir[i].name));
            }
            eduiMenuAttach(menu, mergeeffectsmenu);
            mergeeffectsmenu->x = menu->x + 10;
            mergeeffectsmenu->y = menu->y + 40;
        }
    }
}


void cbMemCardDeleteEffects(struct eduimenu_s *menu, struct eduiitem_s *item) {
    struct eduimenu_s *arg;

    arg = 0;
    sceMcDelete(0, 0, mcdir[item->value].name);
    sceMcSync(0, 0, &mcresult);
    if (menu->parent != 0) {
        arg = menu->parent;
        eduiMenuDetach(menu);
    }
    if (menu->cancel != 0) {
        menu->cancel(menu, arg);
    }
    if (mcresult < 0) {
        CreateMessageMenu((struct eduimenu_s *)arg, "File Delete Error", 0);
    } else {
        CreateMessageMenu((struct eduimenu_s *)arg, "File Delete OK", 1);
    }
}


static inline void cbMemCardDeleteEffectsMenu(struct eduimenu_s *menu) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};
    int i;

    sceMcGetDir(0, 0, "*.eff", 0, 10, mcdir);
    sceMcSync(0, 0, &mcresult);
    if (mcresult > 0) {
        deleteeffectsmenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt,
                                           cbMemCardCancelDeleteEffectsMenu, "Delete Effects");
        if (deleteeffectsmenu != 0) {
            for (i = 0; i < mcresult; i++) {
                eduiMenuAddItem(deleteeffectsmenu, eduiItemSelCreate(i, &font, 0, 0,
                                                                     cbMemCardDeleteEffects,
                                                                     mcdir[i].name));
            }
            eduiMenuAttach(menu, deleteeffectsmenu);
            deleteeffectsmenu->x = menu->x + 10;
            deleteeffectsmenu->y = menu->y + 40;
        }
    }
}


void cbMemCardMenu(struct eduimenu_s *menu) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};

    sceMcGetInfo(0, 0, &mctype, &mcfree, &mcformat);
    sceMcSync(0, 0, &mcresult);
    if (mcformat == 0) {
        sceMcFormat(0, 0);
        sceMcSync(0, 0, &mcresult);
        sceMcGetInfo(0, 0, &mctype, &mcfree, &mcformat);
        sceMcSync(0, 0, &mcresult);
    }
    sceMcChdir(0, 0, "/", 0);
    sceMcSync(0, 0, &mcresult);
    sceMcGetDir(0, 0, "ParticleData", 0, 10, mcdir);
    sceMcSync(0, 0, &mcresult);
    if (mcresult == 0) {
        sceMcMkdir(0, 0, "ParticleData");
        sceMcSync(0, 0, &mcresult);
    }
    sceMcChdir(0, 0, "/ParticleData", 0);
    sceMcSync(0, 0, &mcresult);
    sceMcGetDir(0, 0, "*", 0, 10, mcdir);
    sceMcSync(0, 0, &mcresult);
    memcardmenu = eduiMenuCreate(0x46, 0x46, 0x10E, 0xFA, app_fnt, cbCancelMemCardMenu,
                                 "Memory Card");
    if (memcardmenu != 0) {
        eduiMenuAddItem(memcardmenu, eduiItemSelCreate(1, &font, 0, 0, cbChangeFileNameMenu,
                                                       "File Name..."));
        eduiMenuAddItem(memcardmenu, eduiItemSelCreate(1, &font, 0, 0, cbMemCardSaveEffects,
                                                       "Save All Effects"));
        eduiMenuAddItem(memcardmenu, eduiItemSelCreate(1, &font, 0, 0, cbMemCardLoadEffectsMenu,
                                                       "Load All Effects..."));
        if (edpp_effect_list == 0) {
            eduiMenuAddItem(memcardmenu, eduiItemSelCreate(1, &font, 0, 0,
                                                           cbMemCardSaveEffectTypes,
                                                           "Save General Effect Types"));
            eduiMenuAddItem(memcardmenu, eduiItemSelCreate(1, &font, 0, 0,
                                                           cbMemCardMergeEffectsMenu,
                                                           "Merge Effect Types > General..."));
        } else if (edpp_effect_list == 1) {
            eduiMenuAddItem(memcardmenu, eduiItemSelCreate(1, &font, 0, 0,
                                                           cbMemCardSaveEffectTypes,
                                                           "Save Level Effect Types"));
            eduiMenuAddItem(memcardmenu, eduiItemSelCreate(1, &font, 0, 0,
                                                           cbMemCardMergeEffectsMenu,
                                                           "Merge Effect Types > Level..."));
        } else if (edpp_effect_list == 5) {
            eduiMenuAddItem(memcardmenu, eduiItemSelCreate(1, &font, 0, 0,
                                                           cbMemCardSaveEffectTypes,
                                                           "Save Char Effect Types"));
            eduiMenuAddItem(memcardmenu, eduiItemSelCreate(1, &font, 0, 0,
                                                           cbMemCardMergeEffectsMenu,
                                                           "Merge Effect Types > Char..."));
        }
        eduiMenuAddItem(memcardmenu, eduiItemSelCreate(1, &font, 0, 0, cbMemCardDeleteEffectsMenu,
                                                       "Delete Effects..."));
        eduiMenuAddItem(memcardmenu, eduiItemSelCreate(1, &font, 0, 0, cbMemCardEraseConfirm,
                                                       "Erase Card"));
        eduiMenuAttach(menu, memcardmenu);
        memcardmenu->x = menu->x + 10;
        memcardmenu->y = menu->y + 40;
    }
}


void edptlcbClipboardMenu(struct eduimenu_s *menu) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};
    char label[64];
    char listname[32];
    struct debinfo_s *dt;

    if (edpp_create_type == -1) {
        return;
    }
    dt = debtab[edpp_create_type];
    ptlclipmenu = eduiMenuCreate(0x46, 0x46, 0x12C, 0xFA, app_fnt, edptlcbCancelClipboardMenu,
                                 "Clipboard Menu");
    if (ptlclipmenu != 0) {
        if (edpp_effect_list == 0) {
            sprintf(listname, "General List");
        } else if (edpp_effect_list == 1) {
            sprintf(listname, "Level List");
        } else if (edpp_effect_list == 5) {
            sprintf(listname, "Char List");
        }
        if (edptl_clipboard_entry != -1) {
            eduiMenuAddItem(ptlclipmenu, eduiItemSelCreate(1, &font, 0, 0, edptlcbEmptyClipboard,
                                                           "Empty Clipboard"));
            sprintf(label, "Paste %s into %s", debtab[edptl_clipboard_entry]->id, listname);
            eduiMenuAddItem(ptlclipmenu,
                            eduiItemSelCreate(1, &font, 0, 0, edptlcbPasteClipboard, label));
        } else {
            sprintf(label, "Cut %s from %s", dt->id, listname);
            eduiMenuAddItem(ptlclipmenu,
                            eduiItemSelCreate(1, &font, 0, 0, edptlcbCutClipboard, label));
        }
        eduiMenuAttach(menu, ptlclipmenu);
        ptlclipmenu->x = menu->x + 10;
        ptlclipmenu->y = menu->y + 40;
    }
}


void cbPtlDataMenu(struct eduimenu_s *menu) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};

    ptldatamenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, cbPtlCancelDataMenu, "Data Menu");
    if (ptldatamenu != 0) {
        eduiMenuAddItem(ptldatamenu, eduiItemSelCreate(1, &font, 0, 0, cbChangeNameMenu,
                                                       "Type Name..."));
        if (edpp_create_type != -1) {
            eduiMenuAddItem(ptldatamenu, eduiItemSelCreate(1, &font, 0, 0, cbPtlDeleteEffect,
                                                           "Delete Effect"));
            eduiMenuAddItem(ptldatamenu, eduiItemSelCreate(1, &font, 0, 0, cbPtlCopyEffect,
                                                           "Copy Effect"));
        }
        if (edpp_effect_list == 0) {
            eduiMenuAddItem(ptldatamenu, eduiItemSelCreate(1, &font, 0, 0, cbFileSaveEffects,
                                                           "Save General list"));
        } else if (edpp_effect_list == 1) {
            eduiMenuAddItem(ptldatamenu, eduiItemSelCreate(1, &font, 0, 0, cbFileSaveEffects,
                                                           "Save Level list"));
        } else if (edpp_effect_list == 5) {
            eduiMenuAddItem(ptldatamenu, eduiItemSelCreate(1, &font, 0, 0, cbFileSaveEffects,
                                                           "Save Char list"));
        }
        eduiMenuAddItem(ptldatamenu, eduiItemSelCreate(1, &font, 0, 0, cbFileLoadEffects,
                                                       "Load all from file"));
        eduiMenuAddItem(ptldatamenu, eduiItemSelCreate(1, &font, 0, 0, cbMemCardMenu,
                                                       "Mem Card..."));
        if (edpp_create_type != -1) {
            eduiMenuAddItem(ptldatamenu, eduiItemSelCreate(1, &font, 0, 0, edptlcbClipboardMenu,
                                                           "Clipboard..."));
        }
        eduiMenuAttach(menu, ptldatamenu);
        ptldatamenu->x = menu->x + 10;
        ptldatamenu->y = menu->y + 40;
    }
}


void cbPtlEmitMenu(struct eduimenu_s *menu) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};
    struct debinfo_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    dt = debtab[debkeydata[edpp_ptls[edpp_nearest].debris].type];
    ptlemitmenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0x12C, app_fnt, cbPtlCancelEmitMenu,
                                 "Emitter Settings");
    if (ptlemitmenu != 0) {
        eduiMenuAddItem(ptlemitmenu, eduiItemSelCreate(1, &font, 0, 0, cbPtlEmitVelMenu,
                                                       "Emitter Vel..."));
        eduiMenuAddItem(ptlemitmenu, eduiItemSelCreate(1, &font, 0, 0, cbPtlGravMenu,
                                                       "Gravity..."));
        if (dt->gensort == 0 || dt->gensort == 8 || dt->gensort == 9 || dt->gensort == 10) {
            eduiMenuAddItem(ptlemitmenu, eduiItemSelCreate(1, &font, 0, 0, cbPtlVarStartMenu,
                                                           "Random Start..."));
        } else if (dt->gensort == 6 || dt->gensort == 7) {
            eduiMenuAddItem(ptlemitmenu, eduiItemSelCreate(1, &font, 0, 0, cbPtlVarStartMenu,
                                                           "Base Emit Vector..."));
        }
        eduiMenuAddItem(ptlemitmenu, eduiItemSelCreate(1, &font, 0, 0, cbPtlVarEmitMenu,
                                                       "Emitter Variation..."));
        eduiMenuAddItem(ptlemitmenu, eduiItemSelCreate(1, &font, 0, 0, cbChangeGenRateMenu,
                                                       "Gen Rate..."));
        eduiMenuAddItem(ptlemitmenu, eduiItemSelCreate(1, &font, 0, 0, cbChangeETimeMenu,
                                                       "Particle Life..."));
        eduiMenuAddItem(ptlemitmenu, eduiItemSelCreate(1, &font, 0, 0, cbPtlEmitTimeMenu,
                                                       "Emitter Timing..."));
        eduiMenuAddItem(ptlemitmenu, eduiItemSelCreate(1, &font, 0, 0, cbPtlCutOffMenu,
                                                       "CutOff/On Radii..."));
    }
    eduiMenuAttach(menu, ptlemitmenu);
    ptlemitmenu->x = menu->x + 10;
    ptlemitmenu->y = menu->y + 40;
}


void cbPtlGSortMenu(struct eduimenu_s *menu) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};
    struct debinfo_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    dt = debtab[debkeydata[edpp_ptls[edpp_nearest].debris].type];
    ptlgsortmenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, cbPtlCancelGSortMenu,
                                  "GenSort Type");
    if (ptlgsortmenu != 0) {
        eduiMenuAddItem(ptlgsortmenu, eduiItemCheckCreate(0, &font, dt->gensort == 0, 1,
                                                          cbPtlSelGSort, "Normal"));
        eduiMenuAddItem(ptlgsortmenu, eduiItemCheckCreate(6, &font, dt->gensort == 6, 1,
                                                          cbPtlSelGSort, "Radial"));
        eduiMenuAddItem(ptlgsortmenu, eduiItemCheckCreate(7, &font, dt->gensort == 7, 1,
                                                          cbPtlSelGSort, "Radial Rotor"));
        eduiMenuAddItem(ptlgsortmenu, eduiItemCheckCreate(8, &font, dt->gensort == 8, 1,
                                                          cbPtlSelGSort, "Spheroid"));
        eduiMenuAddItem(ptlgsortmenu, eduiItemCheckCreate(9, &font, dt->gensort == 9, 1,
                                                          cbPtlSelGSort, "BounceY"));
        eduiMenuAddItem(ptlgsortmenu, eduiItemCheckCreate(10, &font, dt->gensort == 10, 1,
                                                          cbPtlSelGSort, "BounceXZ"));
    }
    eduiMenuAttach(menu, ptlgsortmenu);
    ptlgsortmenu->x = menu->x + 10;
    ptlgsortmenu->y = menu->y + 40;
}


void edppInit(void) {
    struct eduistyle_s font = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};
    struct numtl_s *mtl;
    struct numtl_s *boxmtl;
    int i;

    edpp_first_time_this_level = 1;
    mtl = NuMtlCreate(1);
    edpp_mtl = mtl;
    mtl->attrib.cull = 2;
    mtl->attrib.zmode = 0;
    mtl->diffuse.r = 0.5f;
    mtl->diffuse.g = 0.5f;
    mtl->diffuse.b = 0.5f;
    mtl->alpha = 1.0f;
    mtl->attrib.alpha = 0;
    NuMtlUpdate(mtl);
    boxmtl = NuMtlCreate(1);
    edpp_boxmtl = boxmtl;
    boxmtl->attrib.cull = 2;
    boxmtl->attrib.zmode = 3;
    boxmtl->diffuse.r = 0.5f;
    boxmtl->diffuse.g = 0.5f;
    boxmtl->diffuse.b = 0.5f;
    boxmtl->alpha = 1.0f;
    boxmtl->attrib.alpha = 0;
    NuMtlUpdate(boxmtl);
    for (i = 0; i < 256; i++) {
        edpp_ptls[i].debris = -1;
    }
    edpp_nextalloc = 0;
    edpp_nearest = -1;
    edpp_curr = -1;
    edpp_create_type = -1;
    edpp_effect_list = 0;
    edpp_emitrotz = 0;
    edpp_emitroty = 0;
    ptlgcodemenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, 0, "GenCode Type");
    if (ptlgcodemenu != 0) {
        eduiMenuAddItem(ptlgcodemenu, eduiItemCheckCreate(1, &font, 0, 1,
                                                          cbPtlSelGCode, "None"));
        eduiMenuAddItem(ptlgcodemenu, eduiItemCheckCreate(2, &font, 0, 1,
                                                          cbPtlSelGCode, "Pos"));
        eduiMenuAddItem(ptlgcodemenu, eduiItemCheckCreate(3, &font, 0, 1,
                                                          cbPtlSelGCode, "PosRev"));
        eduiMenuAddItem(ptlgcodemenu, eduiItemCheckCreate(4, &font, 0, 1,
                                                          cbPtlSelGCode, "Splash"));
        eduiMenuAddItem(ptlgcodemenu, eduiItemCheckCreate(5, &font, 0, 1,
                                                          cbPtlSelGCode, "AshRock"));
        eduiMenuAddItem(ptlgcodemenu, eduiItemCheckCreate(6, &font, 0, 1,
                                                          cbPtlSelGCode, "PosRevTree"));
        eduiMenuAddItem(ptlgcodemenu, eduiItemCheckCreate(7, &font, 0, 1,
                                                          cbPtlSelGCode, "PosAll"));
    }
    ptloptmenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0x12C, app_fnt, cbPtlCancel, "Options");
    if (ptloptmenu != 0) {
        eduiMenuAddItem(ptloptmenu, eduiItemSelCreate(1, &font, 0, 0,
                                                      cbEffectListMenu, "Effect List..."));
        eduiMenuAddItem(ptloptmenu, eduiItemSelCreate(1, &font, 0, 0,
                                                      cbPtlTypeMenu, "Particle Type..."));
        eduiMenuAddItem(ptloptmenu, eduiItemSelCreate(1, &font, 0, 0,
                                                      cbPtlDataMenu, "Data Menu..."));
        eduiMenuAddItem(ptloptmenu, eduiItemSelCreate(1, &font, 0, 0,
                                                      cbPtlGSortMenu, "GenSort Type..."));
        eduiMenuAddItem(ptloptmenu, eduiItemSelCreate(1, &font, 0, 0,
                                                      cbPtlColMenu, "Particle Colour..."));
        eduiMenuAddItem(ptloptmenu, eduiItemSelCreate(1, &font, 0, 0,
                                                      cbPtlTextureMenu, "Particle Texture..."));
        eduiMenuAddItem(ptloptmenu, eduiItemSelCreate(1, &font, 0, 0,
                                                      cbPtlEmitMenu, "Emitter Settings..."));
        eduiMenuAddItem(ptloptmenu, eduiItemSelCreate(1, &font, 0, 0,
                                                      cbPtlSizeMenu, "Particle Size..."));
        eduiMenuAddItem(ptloptmenu, eduiItemSelCreate(1, &font, 0, 0,
                                                      cbPtlRotMenu, "Particle Rotation..."));
        eduiMenuAddItem(ptloptmenu, eduiItemSelCreate(1, &font, 0, 0,
                                                      cbPtlJibMenu, "Particle Jibber..."));
        eduiMenuAddItem(ptloptmenu, eduiItemSelCreate(1, &font, 0, 0,
                                                      edptlcbBounceMenu, "Particle Bounces..."));
        eduiMenuAddItem(ptloptmenu, eduiItemSelCreate(1, &font, 0, 0,
                                                      edptlcbSoundsMenu, "Attached Sounds..."));
        eduiMenuAddItem(ptloptmenu, eduiItemSelCreate(1, &font, 0, 0,
                                                      cbPtlCollMenu, "Collision Spheres..."));
        eduiMenuAddItem(ptloptmenu, eduiItemSelCreate(1, &font, 0, 0,
                                                      cbPtlSScaleMenu, "Super Scale..."));
        eduiMenuAddItem(ptloptmenu, eduiItemSelCreate(1, &font, 0, 0,
                                                      edptlcbDpadModeMenu, "Dpad Mode..."));
        eduiMenuAddItem(ptloptmenu, eduiItemSelCreate(1, &font, 0, 0,
                                                      edptlcbSwitchMenu, "Switch Menu..."));
        eduiMenuAddItem(ptloptmenu, eduiItemSelCreate(1, &font, 0, 0,
                                                      edptlcbGroupMenu, "Render Group..."));
        eduiMenuAddItem(ptloptmenu, eduiItemSelCreate(1, &font, 0, 0,
                                                      edptlcbResetParticles, "Reset Particles"));
        eduiMenuAddItem(ptloptmenu, eduiItemToggleCreate(1, &font, 0, 1,
                                                         cbPtlSnapToggle, "Snap"));
        eduiMenuAddItem(ptloptmenu, eduiItemSelCreate(1, &font, 0, 0, edptlcbJumpToGameLocation,
                                                      "Jump To Game Location"));
    }
}


float edpp_cam_dist = 0.0f;
float edpp_cam_ax = 0.0f;
float edpp_cam_ay = 0.0f;
int edpp_first_time_this_level = 0;
int edpp_numparticles = 0;
void *edpp_mtl = 0;
void *edpp_boxmtl = 0;
int edpp_nextalloc = 0;
int edpp_nearest = 0;
int edpp_curr = 0;
int edpp_snap_enabled = 0;
signed char edpp_effect_list = 0;
void *edpp_active_menu = 0;
struct eduimenu_s *ptltypemenu = 0;
void *ptloptmenu = 0;
struct eduimenu_s *ptlgsortmenu = 0;
void *ptlgcodemenu = 0;
struct eduimenu_s *ptlemitmenu = 0;
struct eduimenu_s *ptlsizemenu = 0;
struct eduimenu_s *ptlrotmenu = 0;
struct eduimenu_s *ptljibmenu = 0;
struct eduimenu_s *ptlcolmenu = 0;
struct eduimenu_s *ptlemitvelmenu = 0;
struct eduimenu_s *ptlgravmenu = 0;
struct eduimenu_s *ptlvaremitmenu = 0;
struct eduimenu_s *ptlvarstartmenu = 0;
struct eduimenu_s *ptlcutoffmenu = 0;
struct eduimenu_s *ptldatamenu = 0;
struct eduimenu_s *memcardmenu = 0;
struct eduimenu_s *messagemenu = 0;
struct eduimenu_s *loadeffectsmenu = 0;
struct eduimenu_s *mergeeffectsmenu = 0;
struct eduimenu_s *deleteeffectsmenu = 0;
struct eduimenu_s *filenamemenu = 0;
struct eduimenu_s *namemenu = 0;
struct eduimenu_s *confirmmenu = 0;
struct eduimenu_s *changegenratemenu = 0;
struct eduimenu_s *totalptlsmenu = 0;
struct eduimenu_s *etimemenu = 0;
struct eduimenu_s *emittimemenu = 0;
struct eduimenu_s *sscalemenu = 0;
struct eduimenu_s *texturemenu = 0;
struct eduimenu_s *textureselectmenu = 0;
struct eduimenu_s *collmenu = 0;
struct eduimenu_s *effectlistmenu = 0;
struct eduimenu_s *ptlclipmenu = 0;
struct eduimenu_s *dpadmodemenu = 0;
struct edmainreg_s edptldesc = {
    0, 0, "Particle Positioner", edppInit, edppClose, edppEnter, 0, edppApply,
    edppWrite, edppRead, EDMAIN_BLOCKID('p', 't', 'l', 'p'), edppProc, edppRender
};
struct edppptl_s edpp_ptls[256] = {{{0}}};
struct nuvec_s edpp_cam_pos = {0.0f, 0.0f, 0.0f};
/* sceMcGetDir needs a 64-byte aligned table */
struct mcdirent_s mcdir[10] __attribute__((aligned(64))) = {{{0}}};


void ParticleReset(void) {
    int i;

    for (i = 0; i < 256; i++) {
        edpp_ptls[i].debris = -1;
    }
    edpp_nextalloc = 0;
}


void edppDestroyAllParticles(void) {
    int i;

    for (i = 0; i < 256; i++) {
        if (edpp_ptls[i].debris != -1) {
            DebFreeInstantly(&edpp_ptls[i].debris);
            edpp_ptls[i].debris = -1;
        }
    }
}


void edppDestroyAllEffects(void) {
    int i;

    for (i = 1; i < 128; i++) {
        debtab[i] = 0;
    }
    effect_types_used = 1;
}


void edppRegisterLevel(char *path, int id) {
    if (path != 0) {
        NuStrCpy(levelfilename, path);
    } else {
        levelfilename[0] = 0;
    }
    levelfilenametype = id;
}


void edppRegisterPointerToGameCharLocation(struct nuvec_s *loc) {
    edmainRegisterLocVec(loc);
}


void edppPtlDestroy(int index) {
    if (edpp_ptls[index].debris != -1) {
        DebFreeInstantly(&edpp_ptls[index].debris);
        edpp_ptls[index].debris = -1;
    }
}


int edppPtlChangeType(int index, int type) {
    if (edpp_ptls[index].type != type) {
        if (edpp_ptls[index].debris != -1) {
            DebFreeInstantly(&edpp_ptls[index].debris);
            edpp_ptls[index].debris = -1;
        }
        AddDebrisEffect(&edpp_ptls[index].debris, type, edpp_ptls[index].pos.x,
                        edpp_ptls[index].pos.y, edpp_ptls[index].pos.z);
        edpp_ptls[index].type = type;
    }
    return index;
}


int edppWrite(void *file) {
    int count;
    int i;

    count = 0;
    for (i = 0; i < 256; i++) {
        if (edpp_ptls[i].debris != -1) {
            count++;
        }
    }
    NuFileWriteInt(file, count);
    for (i = 0; i < count; i++) {
        NuFileWriteFloat(file, edpp_ptls[i].pos.x);
        NuFileWriteFloat(file, edpp_ptls[i].pos.y);
        NuFileWriteFloat(file, edpp_ptls[i].pos.z);
        NuFileWriteInt(file, edpp_ptls[i].type);
    }
    return 1;
}


int edppRead(void *file) {
    struct nuvec_s pos;
    int count;
    int i;
    int type;

    count = NuFileReadInt(file);
    edppDestroyAllParticles();
    for (i = 0; i < count; i++) {
        pos.x = NuFileReadFloat(file);
        pos.y = NuFileReadFloat(file);
        pos.z = NuFileReadFloat(file);
        type = NuFileReadInt(file);
        edppPtlCreate(&pos, type);
    }
    return 1;
}


int edppProc(float dt, struct nupad_s *pad) {
    edptl_count += 5;
    if (edpp_active_menu != 0) {
        eduiMenuProcess(edpp_active_menu, dt, pad);
        return 0;
    }
    edppDoInput(pad);
    if (pad->paddata_db & 0x800) {
        return 1;
    }
    return 0;
}


inline void edppHighlightNearest(void) {
    if (edpp_nearest != -1) {
        edbitsDrawCube(edpp_ptls[edpp_nearest].pos.x, edpp_ptls[edpp_nearest].pos.y,
                       edpp_ptls[edpp_nearest].pos.z, 0.5f, 0.5f, 0.5f, 0, 0, 0, 0, -1, edpp_mtl);
    }
}


void edppRender(void) {
    edcamSet();
    edppDrawCursor();
    edppHighlightNearest();
    if (edpp_active_menu != 0) {
        eduiMenuRender(edpp_active_menu);
    }
}


void cbPtlChangeGrav(void *menu, struct eduiitem_s *item) {
    struct debinfo_s *dt;
    struct debinfo_s *current;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    dt = debtab[debkeydata[edpp_ptls[edpp_nearest].debris].type];
    dt->grav = item->u.slider.val;
    current = debtab[debkeydata[edpp_ptls[edpp_nearest].debris].type];
    current->DmaDebTypePointer->grav = item->u.slider.val;
}


void edptlcbEmptyClipboard(struct eduimenu_s *menu) {
    int saved;

    saved = edpp_create_type;
    edpp_create_type = edptl_clipboard_entry;
    cbPtlDeleteEffect(menu);
    edpp_create_type = saved;
    edptl_clipboard_entry = -1;
}


void edptlcbPasteClipboard(struct eduimenu_s *menu) {
    struct debinfo_s *dt;
    struct eduimenu_s *arg;

    dt = debtab[edptl_clipboard_entry];
    dt->listid = edpp_effect_list;
    edptl_clipboard_entry = -1;
    arg = 0;
    if (menu->parent != 0) {
        arg = menu->parent;
        eduiMenuDetach(menu);
    }
    if (menu->cancel != 0) {
        menu->cancel(menu, arg);
    }
}


void edptlcbCutClipboard(struct eduimenu_s *menu) {
    struct debinfo_s *dt;
    struct eduimenu_s *arg;

    dt = debtab[edpp_create_type];
    dt->listid = 4;
    edptl_clipboard_entry = edpp_create_type;
    arg = 0;
    if (menu->parent != 0) {
        arg = menu->parent;
        eduiMenuDetach(menu);
    }
    if (menu->cancel != 0) {
        menu->cancel(menu, arg);
    }
}


void edptlcbApplyBounceOffset(void *menu, struct eduiitem_s *item) {
    struct debkeydatatype_s *deb;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    edpp_ptls[edpp_nearest].bounceoffset = item->u.slider.val;
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    deb->refoff = item->u.slider.val;
}


void edptlcbApplyBounceFactor(void *menu, struct eduiitem_s *item) {
    struct debkeydatatype_s *deb;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    edpp_ptls[edpp_nearest].bouncefactor = item->u.slider.val;
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    deb->refbounce = item->u.slider.val;
}


void cbPtlChangeEmitVel(void *menu, struct eduiitem_s *item) {
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    dt->emitmag = item->u.slider.val;
}


void cbPtlChangeX(void *menu, struct eduiitem_s *item) {
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    if (menu == 0) {
        return;
    }
    if (menu == ptlvaremitmenu) {
        dt->variable_emit.x = item->u.slider.val;
        dt->variable_emit_ranscale.x = (item->u.slider.val * 2.0f) * (1.0 / 2147483648.0);
    } else if (menu == ptlvarstartmenu) {
        dt->variable_start.x = item->u.slider.val;
        dt->variable_start_ranscale.x = (item->u.slider.val * 2.0f) * (1.0 / 2147483648.0);
    }
}


void cbPtlChangeCutOff(void *menu, struct eduiitem_s *item) {
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    dt->cutoff = item->u.slider.val;
    if (dt->cutoff < dt->cuton) {
        dt->cuton = dt->cutoff;
    }
}


void cbPtlChangeCutOn(void *menu, struct eduiitem_s *item) {
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    dt->cuton = item->u.slider.val;
    if (dt->cutoff < dt->cuton) {
        dt->cutoff = dt->cuton;
    }
}


void cbPtlChangeDrawCutOff(void *menu, struct eduiitem_s *item) {
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    dt->drawcutoff = item->u.slider.val;
}


void cbPtlChangeGenRate(void *menu, struct eduiitem_s *item) {
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    dt->generate = item->u.slider.val;
    UpdateTotalPtls(dt);
}


void cbCancelChangeGenRateMenu(void) {
    eduiMenuDestroy(changegenratemenu);
    changegenratemenu = 0;
}


void cbPtlApplyJib(void *menu, struct eduiitem_s *item) {
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    if (grad_jib_x_freq_item != 0) {
        dt->jibxfreq = grad_jib_x_freq_item->u.slider.val;
    }
    if (grad_jib_x_amp_item != 0) {
        dt->jibxamp = grad_jib_x_amp_item->u.slider.val;
    }
    if (grad_jib_y_freq_item != 0) {
        dt->jibyfreq = grad_jib_y_freq_item->u.slider.val;
    }
    if (grad_jib_y_amp_item != 0) {
        dt->jibyamp = grad_jib_y_amp_item->u.slider.val;
    }
    GenericDebinfoDmaTypeUpdate(dt);
}


void cbPtlSelType(void *menu, struct eduiitem_s *item) {
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
    ptltypemenu = 0;
    edpp_create_type = item->value;
    edpp_active_menu = 0;
}


void cbPtlSelGCode(struct eduimenu_s *menu, struct eduiitem_s *item) {
    struct debinfo_s *dt;

    eduiMenuDetach(menu);
    edpp_active_menu = 0;
    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    dt = debtab[debkeydata[edpp_ptls[edpp_nearest].debris].type];
    dt->gencode = item->value - 1;
    debkeydata[edpp_ptls[edpp_nearest].debris].gencode = gencodetab[item->value - 1];
}


void cbCancelMessageMenu(void) {
    eduiMenuDestroy(messagemenu);
    messagemenu = 0;
}


void cbSelEffectList(struct eduimenu_s *menu, struct eduiitem_s *item) {
    struct eduimenu_s *arg;
    int i;

    arg = 0;
    if (item->value != edpp_effect_list) {
        edpp_effect_list = item->value;
        edpp_create_type = -1;
        for (i = 1; i < 128; i++) {
            if (debtab[i] != 0 && debtab[i]->listid == edpp_effect_list) {
                edpp_create_type = i;
                break;
            }
        }
    }
    if (menu->parent != 0) {
        arg = menu->parent;
        eduiMenuDetach(menu);
    }
    if (menu->cancel != 0) {
        menu->cancel(menu, arg);
    }
}


void cbCancelEffectListMenu(void) {
    eduiMenuDestroy(effectlistmenu);
    effectlistmenu = 0;
}


void edptlcbSetSwitchId(void *menu, struct eduiitem_s *item) {
    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    edpp_ptls[edpp_nearest].switchid = item->u.slider.val;
    debkeydata[edpp_ptls[edpp_nearest].debris].trigger_id = item->u.slider.val;
}


void edptlcbSetSwitchVar(void *menu, struct eduiitem_s *item) {
    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    edpp_ptls[edpp_nearest].switchvar = item->u.slider.val;
    debkeydata[edpp_ptls[edpp_nearest].debris].trigger_var = item->u.slider.val;
}


void edptlcbCancelSwitchMenu(void) {
    eduiMenuDestroy(edptl_switch_menu);
    edptl_switch_menu = 0;
}


void edptlcbCancelGroupMenu(void) {
    eduiMenuDestroy(edptl_group_menu);
    edptl_group_menu = 0;
}


void edptlcbSetMasterGroup(void *menu, struct eduiitem_s *item) {
    debris_render_group = item->u.slider.val;
}


void edptlcbSetGroup(void *menu, struct eduiitem_s *item) {
    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    edpp_ptls[edpp_nearest].group = item->u.slider.val;
    debkeydata[edpp_ptls[edpp_nearest].debris].group_id = item->u.slider.val;
}


void edptlcbSetDpadMode(void *menu, struct eduiitem_s *item) {
    edpp_dpad_mode = item->value;
}


void edptlcbCancelDpadModeMenu(void) {
    eduiMenuDestroy(dpadmodemenu);
    dpadmodemenu = 0;
}


void edptlcbJumpToGameLocation(void *menu, struct eduiitem_s *item) {
    item->on = 0;
    edcamSetPosAng(&entry_position, 0, 0);
}


void cbPtlCancelTypeMenu(void) {
    eduiMenuDestroy(ptltypemenu);
    ptltypemenu = 0;
}


void cbPtlCancelColMenu(void) {
    eduiMenuDestroy(ptlcolmenu);
    ptlcolmenu = 0;
    grad_item = 0;
    grad_alpha_item = 0;
}


void cbPtlCancelJibMenu(void) {
    eduiMenuDestroy(ptljibmenu);
    ptljibmenu = 0;
    grad_jib_x_freq_item = 0;
    grad_jib_x_amp_item = 0;
    grad_jib_y_freq_item = 0;
    grad_jib_y_amp_item = 0;
}


void edptlcbCancelBounceMenu(void) {
    eduiMenuDestroy(edptl_bounce_menu);
    edptl_bounce_menu = 0;
}


void cbChangeNumCollSpheres(void *menu, struct eduiitem_s *item) {
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    dt->numspheres = item->u.slider.val;
}


void cbPtlDefaultCollEnv(struct eduimenu_s *menu) {
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;
    struct eduimenu_s *arg;
    int i;

    arg = 0;
    if (edpp_nearest != -1 && edpp_ptls[edpp_nearest].debris != -1) {
        deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
        dt = debtab[deb->type];
        for (i = 0; i < 8; i++) {
            dt->sphereslot[i].t = dt->wslot[i].t;
            dt->sphereslot[i].value = dt->wslot[i].value / 10000.0f;
        }
    }
    if (menu->parent != 0) {
        arg = menu->parent;
        eduiMenuDetach(menu);
    }
    if (menu->cancel != 0) {
        menu->cancel(menu, arg);
    }
}


void cbPtlApplyCollEnv(void) {
    struct eduigradread_s keys[8];
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;
    float lo;
    float hi;
    int count;
    int i;

    if (coll_env_item == 0) {
        return;
    }
    count = eduiGradPickRead(coll_env_item, keys, 8);
    if (count < 2 || count > 8) {
        return;
    }
    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    lo = dt->minsize / 10000.0f;
    hi = dt->maxsize / 10000.0f;
    for (i = 0; i < count; i++) {
        dt->sphereslot[i].t = keys[i].pos;
        dt->sphereslot[i].value = lo + keys[i].r * (hi - lo);
    }
}


void edptlcbSetSoundID(void *menu, struct eduiitem_s *item) {
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;
    unsigned int slot;
    int id;

    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
    edptl_soundid_menu = 0;
    slot = (unsigned int)item->value >> 16;
    id = item->value - (slot << 16);
    if (id == 9999) {
        id = -1;
    }
    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    dt->sounds[slot].id = id;
}


void edptlcbSetSoundControl(void *menu, struct eduiitem_s *item) {
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;
    unsigned int slot;
    int control;

    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
    edptl_soundcontrol_menu = 0;
    slot = (unsigned int)item->value >> 16;
    control = item->value - (slot << 16);
    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    dt->sounds[slot].type = control;
}


void edptlcbChangeSoundDelay(void *menu, struct eduiitem_s *item) {
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    dt->sounds[item->value].delay = item->u.slider.val;
}


void edptlcbCancelSoundIDMenu(void) {
    eduiMenuDestroy(edptl_soundid_menu);
    edptl_soundid_menu = 0;
}


void edptlcbCancelSoundControlMenu(void) {
    eduiMenuDestroy(edptl_soundcontrol_menu);
    edptl_soundcontrol_menu = 0;
}


void edptlcbCancelSoundXMenu(void) {
    eduiMenuDestroy(edptl_soundx_menu);
    edptl_soundx_menu = 0;
}


void cbPtlCancelCollMenu(void) {
    eduiMenuDestroy(collmenu);
    collmenu = 0;
    coll_env_item = 0;
}


void cbPtlCancelSizeMenu(void) {
    eduiMenuDestroy(ptlsizemenu);
    ptlsizemenu = 0;
    grad_size_w_item = 0;
    grad_size_h_item = 0;
    grad_size_min_item = 0;
    grad_size_max_item = 0;
}


void cbPtlCancelRotMenu(void) {
    eduiMenuDestroy(ptlrotmenu);
    ptlrotmenu = 0;
    grad_rot_item = 0;
    grad_rot_min_item = 0;
    grad_rot_max_item = 0;
}


void cbPtlCancelCutOffMenu(void) {
    eduiMenuDestroy(ptlcutoffmenu);
    ptlcutoffmenu = 0;
}


void cbPtlCancelVarEmitMenu(void) {
    eduiMenuDestroy(ptlvaremitmenu);
    ptlvaremitmenu = 0;
}


void cbPtlCancelVarStartMenu(void) {
    eduiMenuDestroy(ptlvarstartmenu);
    ptlvarstartmenu = 0;
}


void cbPtlChangeTextureSelect(struct eduimenu_s *menu, struct eduiitem_s *item) {
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;
    struct eduimenu_s *arg;

    arg = 0;
    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    dt->u1 = item->u.texpick.u[0] * 256.0f + 524288.0f;
    dt->v1 = item->u.texpick.v[0] * 256.0f + 524288.0f;
    dt->u2 = item->u.texpick.u[1] * 256.0f + 524288.0f;
    dt->v2 = item->u.texpick.v[1] * 256.0f + 524288.0f;
    if (menu->parent != 0) {
        arg = menu->parent;
        eduiMenuDetach(menu);
    }
    if (menu->cancel != 0) {
        menu->cancel(menu, arg);
    }
    GenericDebinfoDmaTypeUpdate(dt);
}


void cbPtlSelTextureType(void *menu, struct eduiitem_s *item) {
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    dt->mat = item->value;
}


void cbPtlCancelTextureMenu(void) {
    if (textureselectmenu != 0) {
        eduiMenuDestroy(textureselectmenu);
        textureselectmenu = 0;
    }
    eduiMenuDestroy(texturemenu);
    texturemenu = 0;
}


void cbPtlChangeIvalOn(void *menu, struct eduiitem_s *item) {
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    dt->ival_on = item->u.slider.val;
    UpdateTotalPtls(dt);
}


void cbPtlChangeIvalOnRan(void *menu, struct eduiitem_s *item) {
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    dt->ival_on_ran = item->u.slider.val;
    UpdateTotalPtls(dt);
}


void cbPtlChangeIvalOff(void *menu, struct eduiitem_s *item) {
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    dt->ival_off = item->u.slider.val;
    UpdateTotalPtls(dt);
}


void cbPtlChangeIvalOffRan(void *menu, struct eduiitem_s *item) {
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    dt->ival_off_ran = item->u.slider.val;
    UpdateTotalPtls(dt);
}


void cbPtlCancelEmitTimeMenu(void) {
    eduiMenuDestroy(emittimemenu);
    emittimemenu = 0;
}


void cbPtlCancelEmitVelMenu(void) {
    eduiMenuDestroy(ptlemitvelmenu);
    ptlemitvelmenu = 0;
}


void cbPtlCancelGravMenu(void) {
    eduiMenuDestroy(ptlgravmenu);
    ptlgravmenu = 0;
}


void cbCancelMemCardMenu(void) {
    eduiMenuDestroy(memcardmenu);
    memcardmenu = 0;
}


void edptlcbCancelClipboardMenu(void) {
    eduiMenuDestroy(ptlclipmenu);
    ptlclipmenu = 0;
}


void cbPtlCancelDataMenu(void) {
    eduiMenuDestroy(ptldatamenu);
    ptldatamenu = 0;
}


void cbPtlCancelEmitMenu(void) {
    eduiMenuDestroy(ptlemitmenu);
    ptlemitmenu = 0;
}


void cbPtlCancelGSortMenu(void) {
    eduiMenuDestroy(ptlgsortmenu);
    ptlgsortmenu = 0;
}


void cbPtlSnapToggle(void *menu, struct eduiitem_s *item) {
    edpp_snap_enabled = item->on;
}


void edptlcbResetParticles(void) {
    int i;

    for (i = 0; i < 256; i++) {
        if (edpp_ptls[i].debris != -1) {
            DebFreeInstantly(&edpp_ptls[i].debris);
            edpp_ptls[i].debris = i;
        }
    }
    edppRestartAllEffectsInLevel();
}


void cbPtlCancel(void) {
    edpp_active_menu = 0;
}


void edppClose(void) {
    eduiMenuDestroy(ptloptmenu);
    eduiMenuDestroy(ptlgcodemenu);
}


void edppEnter(void) {
    struct nuvec_s origin;

    memset(&origin, 0, sizeof(origin));
    if (edmainQueryLocVec() != 0) {
        entry_position = *edmainQueryLocVec();
    } else {
        entry_position.x = global_camera.mtx._30;
        entry_position.y = global_camera.mtx._31;
        entry_position.z = global_camera.mtx._32;
    }
    if (edpp_first_time_this_level != 0) {
        if (edmainQueryLocVec() != 0) {
            edcamSetPosAng(edmainQueryLocVec(), 0, 0);
        } else {
            edcamSetPosAng(&origin, 0, 0);
        }
        edpp_first_time_this_level = 0;
    }
}


void edppApply(void) {
}


void cbChangeName(void *menu, struct eduiitem_s *item) {
    struct debinfo_s *dt;

    if (edpp_create_type != -1) {
        dt = debtab[edpp_create_type];
        if (dt != 0) {
            strcpy(dt->id, item->u.textpick.text);
        }
    }
}


void cbPtlChangeETime(void *menu, struct eduiitem_s *item) {
    struct debkeydatatype_s *deb;
    struct debinfo_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->type];
    dt->etime = item->u.slider.val;
    UpdateTotalPtls(dt);
}


void cbCancelChangeETimeMenu(void) {
    eduiMenuDestroy(etimemenu);
    etimemenu = 0;
}


void cbConfirmMenuNo(struct eduimenu_s *menu) {
    struct eduimenu_s *arg;

    arg = 0;
    if (menu->parent != 0) {
        arg = menu->parent;
        eduiMenuDetach(menu);
    }
    if (menu->cancel != 0) {
        menu->cancel(menu, arg);
    }
}


void cbCancelConfirmMenu(void) {
    eduiMenuDestroy(confirmmenu);
    confirmmenu = 0;
}


void edptlcbSetSwitchType(void *menu, struct eduiitem_s *item) {
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
    edptl_switchtype_menu = 0;
    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    edpp_ptls[edpp_nearest].switchtype = item->value;
    debkeydata[edpp_ptls[edpp_nearest].debris].trigger_type = item->value;
}


void edptlcbCancelSwitchTypeMenu(void) {
    eduiMenuDestroy(edptl_switchtype_menu);
    edptl_switchtype_menu = 0;
}


void edptlcbCancelSoundsMenu(void) {
    eduiMenuDestroy(edptl_sounds_menu);
    edptl_sounds_menu = 0;
}


void cbPtlChangeSScale(void *menu, struct eduiitem_s *item) {
    superscale = item->u.slider.val;
}


void cbPtlCancelSScaleMenu(void) {
    eduiMenuDestroy(sscalemenu);
    sscalemenu = 0;
}


void cbChangeFileName(void *menu, struct eduiitem_s *item) {

    strcpy(mcfilename, item->u.textpick.text);
}


void cbCancelChangeFileNameMenu(void) {
    eduiMenuDestroy(filenamemenu);
    filenamemenu = 0;
}


void cbMemCardCancelLoadEffectsMenu(void) {
    eduiMenuDestroy(loadeffectsmenu);
    loadeffectsmenu = 0;
}


void cbMemCardCancelMergeEffectsMenu(void) {
    eduiMenuDestroy(mergeeffectsmenu);
    mergeeffectsmenu = 0;
}


void cbMemCardCancelDeleteEffectsMenu(void) {
    eduiMenuDestroy(deleteeffectsmenu);
    deleteeffectsmenu = 0;
}
