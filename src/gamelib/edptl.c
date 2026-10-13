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

struct edptlitem_s {
    char pad00[0xC];
    int value;
    unsigned char toggle;
    char pad11[0x33];
    char text[8];
    float fvalue;
};

struct edptltexitem_s {
    char pad00[0x44];
    float x0;
    float y0;
    float x1;
    float y1;
    void *change;
    void *mtl;
};

struct nuvec_s {
    float x;
    float y;
    float z;
};

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

struct debcollsphere_s {
    struct nuvec_s vel;
    float time;
};

struct debkey_s {
    char pad000[0x482];
    short debtype;
    char pad484[0xC];
    struct nuvec_s emitpos;
    char pad49C[0xC];
    void *gensort;
    void *gencode;
    char pad4B0[0x14];
    struct debcollsphere_s collspheres[8];
    char pad544[0x4];
    int switchtype;
    int switchid;
    float switchvar;
    char pad554[0x4];
    float bounceoffset;
    float bouncefactor;
    char pad560[0x8];
    short group;
    char pad56A[0x2];
};

struct debphysics_s {
    char pad00[0x14];
    float gravity;
};

struct debenvkey_s {
    float time;
    float value;
};

struct debcolkey_s {
    float time;
    float r;
    float g;
    float b;
};

struct debsound_s {
    int id;
    int control;
    int delay;
};

struct debtypedef_s {
    char name[0x10];
    signed char effectlist;
    char pad11[0x1];
    short genrate;
    short numptls;
    short startoffset;
    short ivalon;
    short ivalonran;
    short ivaloff;
    short ivaloffran;
    signed char gensort;
    signed char gencode;
    signed char texturetype;
    signed char reserved23;
    float reserved24;
    float cuton;
    float cutoff;
    float drawcutoff;
    float emitvel;
    struct nuvec_s varemit;
    struct nuvec_s varemitrand;
    struct nuvec_s varstart;
    struct nuvec_s varstartrand;
    struct nuvec_s posrange;
    struct nuvec_s posbase;
    struct nuvec_s momrange;
    struct nuvec_s mombase;
    float gravity;
    float etime;
    short frames;
    signed char columns;
    signed char rows;
    float frame;
    float jib[4];
    struct debcolkey_s colenv[8];
    struct debenvkey_s alphaenv[8];
    float sizemin;
    float sizemax;
    struct debenvkey_s widthenv[8];
    struct debenvkey_s heightenv[8];
    float rotmin;
    float rotmax;
    struct debenvkey_s rotenv[8];
    struct debenvkey_s extraenv0[8];
    struct debenvkey_s extraenv1[8];
    float texu0;
    float texu1;
    float texv0;
    float texv1;
    struct debphysics_s *physics;
    struct debenvkey_s collenv[8];
    signed char numcollspheres;
    char pad31D[0x3];
    int key;
    struct debsound_s sounds[4];
};

struct nurndrvtx_s {
    struct nuvec_s pnt;
    char pad0C[0x18 - 0x0C];
    int colour;
    char pad1C[0x24 - 0x1C];
};

struct edptlgradkey_s {
    float time;
    char pad04[0xC];
    float value[3];
    char pad1C[0x4];
};

struct edptlmenu_s {
    char pad00[0x8];
    struct edptlitem_s *curitem;
    char pad0C[0x4];
    int x;
    int y;
    char pad18[0xC];
    void (*callback)(struct edptlmenu_s *menu, int arg);
    char pad28[0x8];
    int detacharg;
};

struct edptlgradmenu_s {
    char pad00[0x44];
    void *stages;
};

struct edptlfont_s {
    unsigned int colour;
    char pad04[0xC];
};

struct numtx_s {
    float _00, _01, _02, _03;
    float _10, _11, _12, _13;
    float _20, _21, _22, _23;
    float _30, _31, _32, _33;
};

struct camera_s {
    struct numtx_s mtx;
};

struct debrender_s {
    void *chunk;
    struct debtypedef_s *info;
    void *deb;
    struct numtx_s mtx;
    struct nuvec_s pos;
};

struct pad_s {
    char pad000[0x55C];
    unsigned int paddata;
    char pad560[0x564 - 0x560];
    unsigned int buttons;
    char pad568[0x580 - 0x568];
    unsigned char right_pressure;
    unsigned char left_pressure;
    unsigned char up_pressure;
    unsigned char down_pressure;
};

extern struct edppptl_s edpp_ptls[256];
extern struct debrender_s ParticleChunkRenderStack[256];
extern struct debtypedef_s effecttypes[128];

extern int edpp_nextalloc;
extern int edpp_nearest;
extern int debris_render_group;
extern int effect_types_used;

extern struct debtypedef_s *debtab[128];
extern struct debkey_s debkeydata[];

extern int edpp_dpad_mode;
extern int edpp_snap_enabled;
extern void *edpp_active_menu;
extern struct edptlmenu_s *changegenratemenu;
extern struct edptlmenu_s *messagemenu;
extern struct edptlmenu_s *effectlistmenu;
extern struct edptlmenu_s *edptl_switch_menu;
extern struct edptlmenu_s *edptl_group_menu;
extern struct edptlmenu_s *dpadmodemenu;
extern struct edptlmenu_s *ptltypemenu;
extern struct edptlmenu_s *edptl_bounce_menu;
extern struct edptlmenu_s *edptl_soundid_menu;
extern struct edptlmenu_s *edptl_soundcontrol_menu;
extern struct edptlmenu_s *edptl_soundx_menu;
extern struct edptlmenu_s *collmenu;
extern struct edptlmenu_s *ptlcutoffmenu;
extern struct edptlmenu_s *ptlvaremitmenu;
extern struct edptlmenu_s *ptlvarstartmenu;
extern struct edptlmenu_s *emittimemenu;
extern struct edptlmenu_s *ptlemitvelmenu;
extern struct edptlmenu_s *ptlgravmenu;
extern void *memcardmenu;
extern void *ptlclipmenu;
extern struct edptlmenu_s *ptldatamenu;
extern struct edptlmenu_s *ptlemitmenu;
extern void *ptlgsortmenu;
extern void *ptloptmenu;
extern void *ptlgcodemenu;
extern void *etimemenu;
extern struct edptlmenu_s *confirmmenu;
extern void *edptl_switchtype_menu;
extern void *edptl_sounds_menu;
extern struct edptlmenu_s *sscalemenu;
extern struct edptlmenu_s *filenamemenu;
extern void *loadeffectsmenu;
extern void *mergeeffectsmenu;
extern void *deleteeffectsmenu;
extern struct edptlmenu_s *ptlcolmenu;
extern struct edptlmenu_s *ptlrotmenu;
extern void *D_0062F354;
extern int D_0062F314;
extern int edptl_clipboard_entry;
extern int edpp_create_type;
extern void *D_0062F320;
extern void *D_0062F324;
extern struct edptlgradmenu_s *D_0062F338;
extern struct edptlitem_s *D_0062F33C;
extern struct edptlitem_s *D_0062F340;

#define edpp_slider_scale D_0062F314
#define collenvmenu D_0062F354
#define colgradmenu D_0062F320
#define alphagradmenu D_0062F324

extern void edmainRegisterLocVec(struct nuvec_s *loc);
extern void eduiMenuDetach(void *menu);
extern void eduiMenuDestroy(void *menu);
extern char *strcpy(char *dst, const char *src);
extern void edcamSetPosAng(void *pos, void *ang, int flags);
extern void cbPtlDeleteEffect(struct edptlmenu_s *menu);

extern int sprintf(char *buf, const char *fmt, ...);

extern char D_0062F368[];
#define copynameformat D_0062F368

extern char D_0062F360[];
#define newnameformat D_0062F360

extern char D_00484DC0[];
#define savefilename D_00484DC0

extern struct nuvec_s D_006B3B30;
#define edpp_game_loc D_006B3B30

extern struct edptlmenu_s *ptljibmenu;
extern struct edptlmenu_s *ptlsizemenu;
extern struct edptlmenu_s *texturemenu;
extern struct edptlmenu_s *textureselectmenu;
extern struct edptlgradmenu_s *D_0062F328;
extern struct edptlgradmenu_s *D_0062F32C;
extern struct edptlitem_s *D_0062F330;
extern struct edptlitem_s *D_0062F334;
extern char D_00617FB8[];
extern int abs(int value);
extern void DebReAlloc(struct debkey_s *deb, int count);
extern void eduiGradStageDelete(struct edptlgradmenu_s *menu, void *stage);
extern void eduiGradStageAddRGB(struct edptlgradmenu_s *menu, float time, float r, float g,
                                float b);
extern void cbCancelChangeGenRateMenu(void);
extern void cbPtlChangeGenRate(void *menu, struct edptlitem_s *item);

#define genratetitle D_00617FB8
#define widthgradmenu D_0062F328
#define heightgradmenu D_0062F32C
#define sizeminitem D_0062F330
#define sizemaxitem D_0062F334
#define rotgradmenu D_0062F338
#define rotminitem D_0062F33C
#define rotmaxitem D_0062F340
extern struct edptlitem_s *D_0062F344;
extern struct edptlitem_s *D_0062F348;
extern struct edptlitem_s *D_0062F34C;
extern struct edptlitem_s *D_0062F350;
extern signed char D_0062F358;
extern char D_00484DE0[];
extern int D_0062F318;

extern void NuStrCpy(char *dst, char *src);
extern void DebFreeInstantly(int *key);
extern void AddDebrisEffect(int *key, int type, float x, float y, float z);
extern void edppDoInput(struct pad_s *pad);
extern void eduiMenuProcess(void *menu, struct pad_s *pad);
extern void UpdateTotalPtls(struct debtypedef_s *dt);
extern int eduiGradPickRead(void *menu, struct edptlgradkey_s *keys, int max);
extern void edbitsDrawCube(float x, float y, float z, float sizex, float sizey, float sizez,
                           int rotx, int roty, int rotz, int flags, int colour, void *mtl);

extern void *edpp_mtl;
extern signed char edpp_effect_list;

#define ptljibitem0 D_0062F344
#define ptljibitem1 D_0062F348
#define ptljibitem2 D_0062F34C
#define ptljibitem3 D_0062F350

extern void *namemenu;
extern char D_00617F88[];
#define nameduplicatemessage D_00617F88

extern int NuFileReadInt(void *file);
extern float NuFileReadFloat(void *file);
extern void NuFileWriteInt(void *file, int value);
extern void NuFileWriteFloat(void *file, float value);
extern int edppPtlCreate(struct nuvec_s *pos, int type);
extern void edppRestartAllEffectsInLevel(void);
extern void edcamSet(void);
extern void edppDrawCursor(void);
extern void eduiMenuRender(void *menu);
extern void GenericDebinfoDmaTypeUpdate(struct debtypedef_s *dt);
static void CreateMessageMenu(struct edptlmenu_s *parent, char *message, int flags);

extern int edpp_rotz;
extern int edpp_roty;
extern int edpp_emitrotz;
extern int edpp_emitroty;
extern int edpp_refrotz;
extern int edpp_refroty;
extern int edpp_first_time_this_level;
extern void *gencodetab[];
extern void *gensorttab[];
extern struct camera_s global_camera;
extern void *app_fnt;
extern struct edptlitem_s *edui_last_item;

extern struct edptlfont_s D_00617F98;
extern struct edptlfont_s D_00617FE0;
extern char D_00618500[];
extern char D_006185B8[];
extern char D_006185D8[];
extern char D_0062F370[];

#define edpp_menu_font D_00617F98
#define edpp_message_font D_00617FE0
#define sscalemenutitle D_00618500
#define memcarderasemessage D_006185B8
#define filenamemenutitle D_006185D8
#define messagemenutitle D_0062F370

extern void DebrisEmiterPos(int key, float x, float y, float z);
extern void DebrisOrientation(int key, short rotz, short roty);
extern void DebrisEmitterOrientation(int key, short rotz, short roty);
extern void DebrisReflectionOrientation(int key, float offset, float factor, short rotz,
                                        short roty);
extern struct nuvec_s *edmainQueryLocVec(void);
extern void *memset(void *dst, int value, unsigned int size);
extern struct edptlmenu_s *eduiMenuCreate(int x, int y, int w, int h, void *font, void *cancelcb,
                                          char *title);
extern void *eduiMenuAddItem(struct edptlmenu_s *menu, void *item);
extern int eduiMenuAttach(struct edptlmenu_s *parent, struct edptlmenu_s *child);
extern void *eduiItemSelCreate(int type, struct edptlfont_s *font, int value, int group, void *cb,
                               char *label);
extern void *eduiItemSliderCreateInt(int id, struct edptlfont_s *font, int group, void *cb,
                                     int min, int max, int value, char *label);
extern void *eduiItemCheckCreate(int value, struct edptlfont_s *font, int checked, int group,
                                 void *cb, char *label);
extern void *eduiItemSliderCreate(void *data, struct edptlfont_s *font, int group, void *cb,
                                  float min, float max, float value, char *label);
extern void *eduiItemTextPickCreate(void *data, struct edptlfont_s *font, void *cb, char *label);
extern void cbPtlCancelSScaleMenu(void);
extern void cbPtlChangeSScale(void *menu, struct edptlitem_s *item);
extern void cbCancelChangeFileNameMenu(void);
extern void cbCancelEffectListMenu(void);
extern void cbSelEffectList(struct edptlmenu_s *menu, struct edptlitem_s *item);
extern void edptlcbCancelGroupMenu(void);
extern void edptlcbSetMasterGroup(void *menu, struct edptlitem_s *item);
extern void edptlcbSetGroup(void *menu, struct edptlitem_s *item);
extern void edptlcbCancelDpadModeMenu(void);
extern void edptlcbSetDpadMode(void *menu, struct edptlitem_s *item);
extern void edptlcbCancelBounceMenu(void);
extern void edptlcbApplyBounceOffset(void *menu, struct edptlitem_s *item);
extern void edptlcbApplyBounceFactor(void *menu, struct edptlitem_s *item);
extern char D_00617FF0[];
extern char D_0062F388[];
extern char D_0062F390[];
extern char D_0062F398[];
extern char D_00618060[];
extern char D_00618070[];
extern char D_00618080[];
extern char D_00618090[];
extern char D_006180A0[];
extern char D_006180B0[];
extern char D_0062F3A8[];
extern char D_006180C0[];
extern char D_00618178[];
extern char D_00618188[];
extern char D_00618198[];
extern char D_006186D0[];
extern char D_006186E8[];
extern char D_00489B60[][64];
extern int D_00633218;
extern int sceMcDelete(int port, int slot, char *name);
extern int sceMcSync(int mode, int *cmd, int *result);

#define effectlisttitle D_00617FF0
#define effectlistgeneral D_0062F388
#define effectlistlevel D_0062F390
#define effectlistchar D_0062F398
#define grouptitle D_00618060
#define mastergrouplabel D_00618070
#define grouplabel D_00618080
#define dpadmodetitle D_00618090
#define dpademitrotlabel D_006180A0
#define dpadgravrotlabel D_006180B0
#define dpadoffsetlabel D_0062F3A8
#define dpadreflectlabel D_006180C0
#define bouncetitle D_00618178
#define bounceoffsetlabel D_00618188
#define bouncefactorlabel D_00618198
#define memcarddeleteerror D_006186D0
#define memcarddeleteok D_006186E8
#define memcardfilenames D_00489B60
#define memcardresult D_00633218
extern void cbChangeFileName(void *menu, struct edptlitem_s *item);
extern void cbPtlCancelDataMenu(void);
extern void cbChangeNameMenu(struct edptlmenu_s *menu);
extern void cbFileSaveEffects(struct edptlmenu_s *menu);
extern void cbFileLoadEffects(struct edptlmenu_s *menu);
extern void cbMemCardMenu(struct edptlmenu_s *menu);
extern void edptlcbClipboardMenu(struct edptlmenu_s *menu);
extern char D_006188C8[];
extern char D_006188D8[];
extern char D_006188E8[];
extern char D_006188F8[];
extern char D_00618908[];
extern char D_00618920[];
extern char D_00618930[];
extern char D_00618940[];
extern char D_00618958[];
extern char D_00618968[];

#define datamenutitle D_006188C8
#define datamenutypename D_006188D8
#define datamenudelete D_006188E8
#define datamenucopy D_006188F8
#define datamenusavegeneral D_00618908
#define datamenusavelevel D_00618920
#define datamenusavechar D_00618930
#define datamenuloadall D_00618940
#define datamenumemcard D_00618958
#define datamenuclipboard D_00618968
extern void cbPtlCancelEmitMenu(void);
extern void cbPtlEmitVelMenu(struct edptlmenu_s *menu);
extern void cbPtlGravMenu(struct edptlmenu_s *menu);
extern void cbPtlVarStartMenu(struct edptlmenu_s *menu);
extern void cbPtlVarEmitMenu(struct edptlmenu_s *menu);
extern void cbChangeGenRateMenu(struct edptlmenu_s *menu);
extern void cbChangeETimeMenu(struct edptlmenu_s *menu);
extern void cbPtlEmitTimeMenu(struct edptlmenu_s *menu);
extern void cbPtlCutOffMenu(struct edptlmenu_s *menu);
extern char D_00618978[];
extern char D_00618990[];
extern char D_006189A0[];
extern char D_006189B0[];
extern char D_006189C0[];
extern char D_006189D8[];
extern char D_006189F0[];
extern char D_00618A00[];
extern char D_00618A18[];
extern char D_00618A30[];

#define emitmenutitle D_00618978
#define emitmenuvel D_00618990
#define emitmenugravity D_006189A0
#define emitmenurandomstart D_006189B0
#define emitmenubasevector D_006189C0
#define emitmenuvariation D_006189D8
#define emitmenugenrate D_006189F0
#define emitmenulife D_00618A00
#define emitmenutiming D_00618A18
#define emitmenucutoff D_00618A30
extern void cbCancelConfirmMenu(void);
extern void cbConfirmMenuNo(struct edptlmenu_s *menu);
extern void cbMemCardErase(struct edptlmenu_s *menu);
extern char D_006185C8[];
extern char D_0062F378[];
extern char D_0062F380[];

#define memcarderasetitle D_006185C8
#define confirmnolabel D_0062F378
#define confirmyeslabel D_0062F380
extern int edpp_offset;
extern void DebrisStartOffset(int key, int offset);
extern void DebrisSetGroupID(int key, short groupid);
extern struct nuvec_s edpp_cam_pos;
extern float edpp_cam_ax;
extern float edpp_cam_ay;
extern void edppDetermineNearest(float range);
extern void edcamMove(struct pad_s *pad);
extern void edcamSetPos(struct nuvec_s *pos);
extern void edcamGetPosAng(struct nuvec_s *pos, float *ax, float *ay);
extern void edcamGetPosAngSnap(struct nuvec_s *pos, float *ax, float *ay);
extern void SphereDraw(struct nuvec_s *pos, float radius, float scale);
extern void *edpp_boxmtl;
extern void NuVecSub(struct nuvec_s *d, struct nuvec_s *a, struct nuvec_s *b);
extern void NuVecRotateZ(struct nuvec_s *out, struct nuvec_s *in, int angle);
extern void NuVecRotateY(struct nuvec_s *out, struct nuvec_s *in, int angle);
extern void NuRndrLine3d(struct nurndrvtx_s *line, void *mtl, void *mtx);
extern void NuRndrRect2di(int x, int y, int w, int h, int colour, void *mtl);
extern void NuRndrLine2di(int x0, int y0, int x1, int y1, int colour, void *mtl);
extern void NuFntSet(void *fnt);
extern void NuFntSetPen(int colour);
extern void NuFntPrintEx(int x, int y, int flags, char *fmt, ...);
extern char D_00617E28[];
extern char D_00617E38[];
extern char D_00617E50[];
extern char D_00617E68[];
extern char D_00617E80[];
extern char D_00617E98[];
extern char D_00617EB0[];
extern char D_00617EC8[];
extern char D_00617ED8[];
extern char D_00617EF0[];
extern char D_00617F00[];
extern char D_00617F18[];
extern char D_00617F30[];
extern char D_00617F48[];
extern char D_00617F60[];

#define infoboxtitle D_00617E28
#define infolistlevel D_00617E38
#define infolistgeneral D_00617E50
#define infolistchar D_00617E68
#define infonotype D_00617E80
#define infotype D_00617E98
#define infonoclipboard D_00617EB0
#define infoclipboard D_00617EC8
#define infonohighlight D_00617ED8
#define infohighlight D_00617EF0
#define infoparticles D_00617F00
#define infodpademit D_00617F18
#define infodpadgrav D_00617F30
#define infodpadoffset D_00617F48
#define infodpadreflect D_00617F60


extern char D_00618020[];
extern char D_00618030[];
extern char D_00618040[];
extern char D_00618050[];
extern char D_006180D0[];
extern char D_006180E0[];
extern char D_006180F0[];
extern char D_00618100[];
extern char D_00618110[];
extern char D_00618128[];
extern char D_00618138[];
extern char D_00618148[];
extern char D_00618158[];
extern char D_00618168[];
extern char D_006181A8[];
extern char D_0062F3B0[];
extern char D_006181B8[];
extern char D_0062F3B8[];
extern char D_0062F3C0[];
extern char D_006181C8[];
extern char D_006181D8[];
extern char D_0062F3C8[];
extern char D_006181E8[];
extern char D_006181F8[];
extern char D_00618208[];
extern char D_00618220[];

#define switchmenutitle D_00618020
#define switchtypelabel D_00618030
#define switchidlabel D_00618040
#define switchvarlabel D_00618050
#define typemenutitle D_006180D0
#define addeffectlabel D_006180E0
#define colmenutitle D_006180F0
#define colgradlabel D_00618100
#define alphagradlabel D_00618110
#define jibmenutitle D_00618128
#define jibxfreqlabel D_00618138
#define jibxamplabel D_00618148
#define jibyfreqlabel D_00618158
#define jibyamplabel D_00618168
#define soundidtitle D_006181A8
#define soundnonelabel D_0062F3B0
#define soundcontroltitle D_006181B8
#define soundofflabel D_0062F3B8
#define soundonedgelabel D_0062F3C0
#define soundoffedgelabel D_006181C8
#define soundperparticlelabel D_006181D8
#define soundregularlabel D_0062F3C8
#define soundxtitleformat D_006181E8
#define soundidlabel D_006181F8
#define soundcontrollabel D_00618208
#define sounddelaylabel D_00618220

extern int edSfxAllCount;
extern char *edbitsGetSoundName(int id);
extern void *eduiItemGradPickCreate(void *data, struct edptlfont_s *font, void *cb, char *label);
extern void *eduiItemGreyGradPickCreate(void *data, struct edptlfont_s *font, void *cb,
                                        char *label);
extern void cbPtlApplyGrad(void);
extern void cbPtlAddEffect(struct edptlmenu_s *menu);
extern void edptlcbCancelSwitchMenu(void);
extern void edptlcbSwitchTypeMenu(struct edptlmenu_s *menu);
extern void edptlcbSetSwitchId(void *menu, struct edptlitem_s *item);
extern void edptlcbSetSwitchVar(void *menu, struct edptlitem_s *item);
extern void cbPtlCancelTypeMenu(void);
extern void cbPtlSelType(void *menu, struct edptlitem_s *item);
extern void cbPtlCancelColMenu(void);
extern void cbPtlCancelJibMenu(void);
extern void cbPtlApplyJib(void *menu, struct edptlitem_s *item);
extern void edptlcbCancelSoundIDMenu(void);
extern void edptlcbSetSoundID(void *menu, struct edptlitem_s *item);
extern void edptlcbCancelSoundControlMenu(void);
extern void edptlcbSetSoundControl(void *menu, struct edptlitem_s *item);
extern void edptlcbCancelSoundXMenu(void);
extern void edptlcbChangeSoundDelay(void *menu, struct edptlitem_s *item);
extern struct numtl_s *DebMat[8];
extern void eduiItemSliderSetFmt(void *item, char *fmt);
extern void eduiItemSliderSetGranularity(void *item, float granularity);
extern void *eduiItemTexturePickCreate(void *data, struct edptlfont_s *font, void *cb, char *label);
extern void cbPtlCancelCollMenu(void);
extern void cbPtlApplyCollEnv(void);
extern void cbChangeNumCollSpheres(void *menu, struct edptlitem_s *item);
extern void cbPtlDefaultCollEnv(struct edptlmenu_s *menu);
extern void cbPtlCancelSizeMenu(void);
extern void cbPtlCancelRotMenu(void);
extern void cbPtlCancelCutOffMenu(void);
extern void cbPtlChangeCutOff(void *menu, struct edptlitem_s *item);
extern void cbPtlChangeCutOn(void *menu, struct edptlitem_s *item);
extern void cbPtlChangeDrawCutOff(void *menu, struct edptlitem_s *item);
extern void cbPtlCancelVarEmitMenu(void);
extern void cbPtlCancelVarStartMenu(void);
extern void cbPtlChangeX(void *menu, struct edptlitem_s *item);
extern void cbPtlChangeTextureSelect(struct edptlmenu_s *menu, struct edptltexitem_s *item);
extern void cbPtlCancelTextureMenu(void);
extern void cbPtlSelTextureType(void *menu, struct edptlitem_s *item);
extern void cbPtlCancelEmitTimeMenu(void);
extern void cbPtlChangeIvalOn(void *menu, struct edptlitem_s *item);
extern void cbPtlChangeIvalOnRan(void *menu, struct edptlitem_s *item);
extern void cbPtlChangeIvalOff(void *menu, struct edptlitem_s *item);
extern void cbPtlChangeIvalOffRan(void *menu, struct edptlitem_s *item);
extern void cbPtlCancelEmitVelMenu(void);
extern void cbPtlChangeEmitVel(void *menu, struct edptlitem_s *item);
extern void cbPtlCancelGravMenu(void);
extern void cbPtlChangeGrav(void *menu, struct edptlitem_s *item);
extern char D_00618250[];
extern char D_00618268[];
extern char D_00618280[];
extern char D_00618290[];
extern char D_006182A8[];
extern char D_006182B8[];
extern char D_006182C8[];
extern char D_006182D8[];
extern char D_006182E8[];
extern char D_006182F8[];
extern char D_00618308[];
extern char D_00618320[];
extern char D_00618340[];
extern char D_00618358[];
extern char D_00618370[];
extern char D_00618380[];
extern char D_00618390[];
extern char D_006183A0[];
extern char D_006183B0[];
extern char D_006183C0[];
extern char D_006183D0[];
extern char D_006183E8[];
extern char D_006183F8[];
extern char D_00618408[];
extern char D_00618418[];
extern char D_00618428[];
extern char D_00618438[];
extern char D_00618448[];
extern char D_00618458[];
extern char D_00618468[];
extern char D_00618478[];
extern char D_00618488[];
extern char D_006184A0[];
extern char D_006184B0[];
extern char D_006184C0[];
extern char D_006184D0[];
extern char D_006184E0[];
extern char D_006184F0[];
extern char D_00618510[];
extern char D_00618520[];
extern char D_00618530[];
extern char D_00618540[];
extern char D_00618550[];
extern char D_00618568[];
extern char D_0062F3D8[];
extern char D_00618578[];
extern char D_00618588[];
extern char D_00618598[];
extern char D_006185A8[];
extern char D_0062F3E0[];
extern char D_0062F3D0[];

#define collmenutitle D_00618250
#define collenvlabel D_00618268
#define numsphereslabel D_00618280
#define defaultenvlabel D_00618290
#define sizemenutitle D_006182A8
#define widthenvlabel D_006182B8
#define heightenvlabel D_006182C8
#define minsizelabel D_006182D8
#define onedecimalfmt D_006182E8
#define maxsizelabel D_006182F8
#define copywidthlabel D_00618308
#define copyheightlabel D_00618320
#define rotmenutitle D_00618340
#define rotenvlabel D_00618358
#define minrotlabel D_00618370
#define maxrotlabel D_00618380
#define cutoffmenutitle D_00618390
#define cutofflabel D_006183A0
#define cutonlabel D_006183B0
#define drawcutofflabel D_006183C0
#define varemitmenutitle D_006183D0
#define randemitxlabel D_006183E8
#define randemitylabel D_006183F8
#define randemitzlabel D_00618408
#define randmagxlabel D_00618418
#define randrotylabel D_00618428
#define randrotzlabel D_00618438
#define randmaglabel D_00618448
#define steprotylabel D_00618458
#define steprotzlabel D_00618468
#define randstarttitle D_00618478
#define baseemittitle D_00618488
#define randstartxlabel D_006184A0
#define randstartylabel D_006184B0
#define randstartzlabel D_006184C0
#define basemaglabel D_006184D0
#define baserotylabel D_006184E0
#define baserotzlabel D_006184F0
#define textureselecttitle D_00618510
#define additivelabel D_00618520
#define modulationlabel D_00618530
#define subtractivelabel D_00618540
#define textureselectorlabel D_00618550
#define emittimemenutitle D_00618568
#define ontimelabel D_0062F3D8
#define randontimelabel D_00618578
#define offtimelabel D_00618588
#define randofftimelabel D_00618598
#define emitveltitle D_006185A8
#define gravitytitle D_0062F3E0
#define texturemenutitle D_0062F3D0
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
extern char D_00618610[];
extern char D_00618620[];
extern char D_00618648[];
extern char D_00618658[];

#define savedokmessage D_00618610
#define saveerrormessage D_00618620
#define loadedokmessage D_00618648
#define loaderrormessage D_00618658

struct mcdirent_s {
    char pad00[0x20];
    char name[32];
};

extern struct mcdirent_s mcdir[];
extern int sceMcGetDir(int port, int slot, char *name, unsigned int mode, int maxent,
                       struct mcdirent_s *table);
extern char D_0062F3F8[];
extern char D_0062F400[];
extern char D_0062F408[];
extern char D_0062F410[];
extern char D_00618668[];
extern char D_00618680[];

#define effectsfilter D_0062F3F8
#define effectsnameformat D_0062F400
#define effecttypesfilter D_0062F408
#define effecttypesnameformat D_0062F410
#define toomanyeffectsmessage D_00618668
#define toomanytypesmessage D_00618680

extern int freeDmaDebType;
extern int EdFileReadInt(void);
extern int NuFileSize(char *name);
extern void *memcpy(void *dst, const void *src, unsigned int size);
extern void EdFileRead(void *data, int size);
extern float EdFileReadFloat(void);
extern short EdFileReadShort(void);
extern char EdFileReadChar(void);
extern char D_006185E8[];
extern char D_00618600[];
extern char D_00618630[];
extern char D_0062F3E8[];
extern char D_0062F3F0[];

#define edpp_level_path D_00484DE0
#define edpp_level_type D_0062F358
#define genericeffectsfile D_006185E8
#define leveleffectsformat D_00618600
#define generalptlfile D_00618630
#define levelptlformat D_0062F3E8
#define charptlformat D_0062F3F0



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
        strcpy(edpp_ptls[edpp_nextalloc].name, debtab[type]->name);
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


void edppDoInput(struct pad_s *pad) {
    int i;

    if (pad->paddata & 0x100) {
        if (edpp_nearest != -1) {
            if (pad->buttons & 0x8) {
                i = edpp_nearest;
                do {
                    i++;
                    if (i == 256) {
                        i %= 256;
                    }
                } while (edpp_ptls[i].debris == -1);
                edpp_nearest = i;
            }
            if (pad->buttons & 0x2) {
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
            edpp_effect_list = debtab[edpp_create_type]->effectlist;
        }
    } else {
        edcamMove(pad);
    }
    if (edpp_snap_enabled) {
        edcamGetPosAngSnap(&edpp_cam_pos, &edpp_cam_ax, &edpp_cam_ay);
    } else {
        edcamGetPosAng(&edpp_cam_pos, &edpp_cam_ax, &edpp_cam_ay);
    }
    if (pad->buttons & 0x80) {
        edpp_active_menu = ptloptmenu;
    }
    if ((pad->buttons & 0x40) && edpp_create_type != -1) {
        edppPtlCreate(&edpp_cam_pos, edpp_create_type);
    }
    if (pad->paddata & 0x20) {
        if (edpp_nearest != -1) {
            edppPtlPlace(edpp_nearest, &edpp_cam_pos);
        }
    } else {
        edppDetermineNearest(1.0f);
    }
    if (pad->buttons & 0x10) {
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
        edpp_roty += pad->right_pressure;
        edpp_roty -= pad->left_pressure;
        edpp_rotz += pad->up_pressure;
        if (edpp_rotz > 0) {
            edpp_rotz = 0;
        }
        edpp_rotz -= pad->down_pressure;
        if (edpp_rotz < -0x8000) {
            edpp_rotz = -0x8000;
        }
    } else if (edpp_dpad_mode == 0) {
        edpp_emitroty += pad->right_pressure;
        edpp_emitroty -= pad->left_pressure;
        edpp_emitrotz += pad->up_pressure;
        if (edpp_emitrotz > 0x8000) {
            edpp_emitrotz = 0x8000;
        }
        edpp_emitrotz -= pad->down_pressure;
        if (edpp_emitrotz < -0x8000) {
            edpp_emitrotz = -0x8000;
        }
    } else if (edpp_dpad_mode == 3) {
        edpp_refroty += pad->right_pressure;
        edpp_refroty -= pad->left_pressure;
        edpp_refrotz += pad->up_pressure;
        if (edpp_refrotz > 0) {
            edpp_refrotz = 0;
        }
        edpp_refrotz -= pad->down_pressure;
        if (edpp_refrotz < -0x8000) {
            edpp_refrotz = -0x8000;
        }
    } else if (edpp_dpad_mode == 2) {
        if (pad->up_pressure == 0xFF || (pad->buttons & 0x1000)) {
            edpp_offset++;
        }
        if (pad->down_pressure == 0xFF || (pad->buttons & 0x4000)) {
            edpp_offset--;
        }
        if (edpp_offset < 0) {
            edpp_offset = 0;
        }
    }
}


void edppDrawSpheres(struct debtypedef_s *dt, int key) {
    struct nuvec_s pos;
    float t;
    float life;
    float size;
    int i;
    int j;

    if (dt->numcollspheres == 0) {
        return;
    }
    for (i = 0; i < dt->numcollspheres; i++) {
        t = debkeydata[key].collspheres[i].time;
        if (t == -1.0f) {
            continue;
        }
        pos.x = debkeydata[key].collspheres[i].vel.x * t + debkeydata[key].emitpos.x;
        pos.y = debkeydata[key].collspheres[i].vel.y * t + dt->gravity * (t * t)
              + debkeydata[key].emitpos.y;
        pos.z = debkeydata[key].collspheres[i].vel.z * t + debkeydata[key].emitpos.z;
        life = t / dt->etime;
        for (j = 0; j < 7; j++) {
            if (dt->collenv[j].time <= life && life <= dt->collenv[j + 1].time) {
                break;
            }
        }
        size = dt->collenv[j].value + (life - dt->collenv[j].time)
             / (dt->collenv[j + 1].time - dt->collenv[j].time)
             * (dt->collenv[j + 1].value - dt->collenv[j].value);
        pos.y -= size;
        SphereDraw(&pos, size, 1.0f);
    }
}


void edppDrawCursor(void) {
    struct nurndrvtx_s line[2];
    struct nuvec_s v;
    struct debtypedef_s *dt;

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
    line[0].colour = -1;
    line[1].colour = -1;
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
    line[0].colour = -1;
    line[1].colour = -1;
    NuRndrLine3d(line, edpp_mtl, 0);
    line[0].pnt.x = edpp_cam_pos.x;
    line[0].pnt.y = edpp_cam_pos.y;
    line[0].pnt.z = edpp_cam_pos.z;
    line[1].pnt.x = edpp_cam_pos.x + v.x;
    line[1].pnt.y = edpp_cam_pos.y + v.y;
    line[1].pnt.z = edpp_cam_pos.z + v.z;
    line[0].colour = 0xFF00FF00;
    line[1].colour = 0xFF00FF00;
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
    line[0].colour = -1;
    line[1].colour = -1;
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
    line[0].colour = 0xFF0000FF;
    line[1].colour = 0xFF0000FF;
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
        line[0].colour = 0xFFFF0000;
        line[1].colour = 0xFFFF0000;
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
    NuFntPrintEx(0x17C0, 0x988, 0, infoboxtitle);
    NuFntSetPen(0x80000000);
    if (edpp_effect_list == 1) {
        NuFntPrintEx(0x17C0, 0xA50, 0, infolistlevel);
    } else if (edpp_effect_list == 0) {
        NuFntPrintEx(0x17C0, 0xA50, 0, infolistgeneral);
    } else if (edpp_effect_list == 5) {
        NuFntPrintEx(0x17C0, 0xA50, 0, infolistchar);
    }
    if (edpp_create_type == -1) {
        NuFntPrintEx(0x17C0, 0xAF0, 0, infonotype);
    } else {
        NuFntPrintEx(0x17C0, 0xAF0, 0, infotype, debtab[edpp_create_type]->name);
    }
    if (edptl_clipboard_entry == -1) {
        NuFntPrintEx(0x17C0, 0xB90, 0, infonoclipboard);
    } else {
        NuFntPrintEx(0x17C0, 0xB90, 0, infoclipboard, debtab[edptl_clipboard_entry]->name);
    }
    if (edpp_nearest == -1) {
        NuFntPrintEx(0x17C0, 0xC30, 0, infonohighlight);
    } else {
        dt = debtab[debkeydata[edpp_ptls[edpp_nearest].debris].debtype];
        if (dt->gensort == 0) {
            edbitsDrawCube(edpp_cam_pos.x, edpp_cam_pos.y, edpp_cam_pos.z, dt->varstart.x,
                           dt->varstart.y, dt->varstart.z, edpp_emitrotz, edpp_emitroty,
                           edpp_rotz, edpp_roty, 0xFF0000FF, edpp_mtl);
            edppDrawSpheres(dt, edpp_ptls[edpp_nearest].debris);
        }
        NuFntPrintEx(0x17C0, 0xC30, 0, infohighlight, dt->name);
        NuFntPrintEx(0x17C0, 0xCD0, 0, infoparticles, dt->numptls,
                     ((dt->numptls - 1) / 32 + 1) * 32);
    }
    if (edpp_dpad_mode == 0) {
        NuFntPrintEx(0x17C0, 0xD70, 0, infodpademit);
    } else if (edpp_dpad_mode == 1) {
        NuFntPrintEx(0x17C0, 0xD70, 0, infodpadgrav);
    } else if (edpp_dpad_mode == 2) {
        NuFntPrintEx(0x17C0, 0xD70, 0, infodpadoffset, edpp_offset);
    } else if (edpp_dpad_mode == 3) {
        NuFntPrintEx(0x17C0, 0xD70, 0, infodpadreflect);
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
    struct edptlgradkey_s keys[8];
    struct debtypedef_s *coldt;
    struct debtypedef_s *alphadt;
    int count;
    int i;
    int j;

    count = eduiGradPickRead(colgradmenu, keys, 8);
    if (count >= 2 && count <= 8 && edpp_nearest != -1
        && edpp_ptls[edpp_nearest].debris != -1) {
        coldt = debtab[debkeydata[edpp_ptls[edpp_nearest].debris].debtype];
        for (i = 0; i < count; i++) {
            coldt->colenv[i].time = keys[i].time;
            coldt->colenv[i].r = keys[i].value[0] * 255.0;
            coldt->colenv[i].g = keys[i].value[1] * 255.0;
            coldt->colenv[i].b = keys[i].value[2] * 255.0;
        }
        GenericDebinfoDmaTypeUpdate(coldt);
    }
    count = eduiGradPickRead(alphagradmenu, keys, 8);
    if (count >= 2 && count <= 8 && edpp_nearest != -1
        && edpp_ptls[edpp_nearest].debris != -1) {
        alphadt = debtab[debkeydata[edpp_ptls[edpp_nearest].debris].debtype];
        for (j = 0; j < count; j++) {
            alphadt->alphaenv[j].time = keys[j].time;
            alphadt->alphaenv[j].value = keys[j].value[0] * 255.0;
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
                    debkeydata[edpp_ptls[k].debris].debtype = i;
                }
            }
            debtab[i] = &effecttypes[i];
            for (k = 0; k < 256; k++) {
                if (ParticleChunkRenderStack[k].chunk != 0
                    && ParticleChunkRenderStack[k].info == debtab[j]) {
                    ParticleChunkRenderStack[k].info = debtab[i];
                }
            }
            *debtab[i] = *debtab[j];
            *debtab[j] = *debtab[0];
            debtab[j] = 0;
        }
    }
}


void cbPtlDeleteEffect(struct edptlmenu_s *menu) {
    int arg;
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
    if (menu->detacharg != 0) {
        arg = menu->detacharg;
        eduiMenuDetach(menu);
    }
    if (menu->callback != 0) {
        menu->callback(menu, arg);
    }
    edpp_create_type = -1;
}


void cbPtlAddEffect(struct edptlmenu_s *menu) {
    int arg;
    int i;

    arg = 0;
    if (effect_types_used < 128) {
        for (i = 1; i < 128; i++) {
            if (debtab[i] == 0) {
                debtab[i] = &effecttypes[i];
                *debtab[i] = *debtab[0];
                sprintf(debtab[i]->name, newnameformat, i);
                debtab[i]->effectlist = edpp_effect_list;
                edpp_create_type = i;
                effect_types_used++;
                break;
            }
        }
    }
    if (menu->detacharg != 0) {
        arg = menu->detacharg;
        eduiMenuDetach(menu);
    }
    if (menu->callback != 0) {
        menu->callback(menu, arg);
    }
}


void cbPtlCopyEffect(struct edptlmenu_s *menu) {
    int arg;
    int i;

    arg = 0;
    if (effect_types_used < 128 && edpp_create_type != -1) {
        for (i = 1; i < 128; i++) {
            if (debtab[i] == 0) {
                debtab[i] = &effecttypes[i];
                *debtab[i] = *debtab[edpp_create_type];
                debtab[i]->physics = 0;
                sprintf(debtab[i]->name, copynameformat, i);
                edpp_create_type = i;
                effect_types_used++;
                break;
            }
        }
    }
    if (menu->detacharg != 0) {
        arg = menu->detacharg;
        eduiMenuDetach(menu);
    }
    if (menu->callback != 0) {
        menu->callback(menu, arg);
    }
}


void cbPtlChangeY(void *menu, struct edptlitem_s *item) {
    struct debkey_s *deb;
    struct debtypedef_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->debtype];
    if (menu == 0) {
        return;
    }
    if (menu == ptlvaremitmenu) {
        if (dt->gensort == 0 || dt->gensort == 8 || dt->gensort == 9 || dt->gensort == 10) {
            dt->varemit.y = item->fvalue;
            dt->varemitrand.y = (item->fvalue * 2.0f) * (1.0 / 2147483648.0);
        } else if (dt->gensort == 6 || dt->gensort == 7) {
            dt->varemit.y = (int)(item->fvalue * 182.04445f);
            dt->varemitrand.y = (int)((item->fvalue * 2.0f) * 182.04445f) * (1.0 / 2147483648.0);
        }
    } else if (menu == ptlvarstartmenu) {
        if (dt->gensort == 0 || dt->gensort == 8 || dt->gensort == 9 || dt->gensort == 10) {
            dt->varstart.y = item->fvalue;
            dt->varstartrand.y = (item->fvalue * 2.0f) * (1.0 / 2147483648.0);
        } else if (dt->gensort == 6 || dt->gensort == 7) {
            dt->varstart.y = (int)(item->fvalue * 182.04445f);
        }
    }
}


void cbPtlChangeZ(void *menu, struct edptlitem_s *item) {
    struct debkey_s *deb;
    struct debtypedef_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->debtype];
    if (menu == 0) {
        return;
    }
    if (menu == ptlvaremitmenu) {
        if (dt->gensort == 0 || dt->gensort == 8 || dt->gensort == 9 || dt->gensort == 10) {
            dt->varemit.z = item->fvalue;
            dt->varemitrand.z = (item->fvalue * 2.0f) * (1.0 / 2147483648.0);
        } else if (dt->gensort == 6 || dt->gensort == 7) {
            dt->varemit.z = (int)(item->fvalue * 182.04445f);
            dt->varemitrand.z = (int)((item->fvalue * 2.0f) * 182.04445f) * (1.0 / 2147483648.0);
        }
    } else if (menu == ptlvarstartmenu) {
        if (dt->gensort == 0 || dt->gensort == 8 || dt->gensort == 9 || dt->gensort == 10) {
            dt->varstart.z = item->fvalue;
            dt->varstartrand.z = (item->fvalue * 2.0f) * (1.0 / 2147483648.0);
        } else if (dt->gensort == 6 || dt->gensort == 7) {
            dt->varstart.z = (int)(item->fvalue * 182.04445f);
        }
    }
}


void UpdateTotalPtls(struct debtypedef_s *dt) {
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
    if (dt->ivaloff == 0) {
        ontimer = frames;
    } else {
        ontimer = dt->ivalon + dt->ivalonran;
    }
    for (i = 0; i < frames; i++) {
        if (ontimer != 0) {
            if (dt->genrate >= 0) {
                total += dt->genrate;
            } else if (count % abs(dt->genrate) == 0) {
                total++;
            }
            ontimer--;
            count++;
            if (ontimer == 0) {
                offtimer = dt->ivaloff;
            }
        } else {
            offtimer--;
            if (offtimer == 0) {
                count = 0;
                ontimer = dt->ivalon + dt->ivalonran;
            }
        }
    }
    dt->numptls = total;
    if (dt->numptls <= 0) {
        dt->numptls = 1;
    }
    for (i = 0; i < 256; i++) {
        if (edpp_ptls[i].debris != -1
            && debtab[debkeydata[edpp_ptls[i].debris].debtype] == dt) {
            DebReAlloc(&debkeydata[edpp_ptls[i].debris], dt->numptls);
        }
    }
}


void cbChangeGenRateMenu(struct edptlmenu_s *menu) {
    struct edptlfont_s font;
    struct debkey_s *deb;
    struct debtypedef_s *dt;

    font = edpp_menu_font;
    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->debtype];
    changegenratemenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, cbCancelChangeGenRateMenu,
                                       genratetitle);
    if (changegenratemenu != 0) {
        eduiMenuAddItem(changegenratemenu,
                        eduiItemSliderCreateInt(0, &font, 0, cbPtlChangeGenRate, -10,
                                                edpp_slider_scale * 20.0, dt->genrate,
                                                genratetitle));
        eduiMenuAttach(menu, changegenratemenu);
        changegenratemenu->x = menu->x + 10;
        changegenratemenu->y = menu->y + 40;
    }
}


void cbPtlCopySize(void *menu, struct edptlitem_s *item) {
    struct debkey_s *deb;
    struct debtypedef_s *dt;
    struct edptlgradmenu_s *grad;
    float ratio;
    int i;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->debtype];
    for (i = 0; i < 8; i++) {
        if (item->value == 1) {
            dt->heightenv[i].time = dt->widthenv[i].time;
            dt->heightenv[i].value = dt->widthenv[i].value;
        } else {
            dt->widthenv[i].time = dt->heightenv[i].time;
            dt->widthenv[i].value = dt->heightenv[i].value;
        }
    }
    if (item->value == 1) {
        grad = heightgradmenu;
    } else {
        grad = widthgradmenu;
    }
    while (grad->stages != 0) {
        eduiGradStageDelete(grad, grad->stages);
    }
    for (i = 0; i < 8; i++) {
        if (item->value == 1) {
            ratio = (dt->heightenv[i].value - dt->sizemin) / (dt->sizemax - dt->sizemin);
            eduiGradStageAddRGB(heightgradmenu, dt->heightenv[i].time, ratio, ratio, ratio);
            if (dt->heightenv[i].time == 1.0f) {
                break;
            }
        } else {
            ratio = (dt->widthenv[i].value - dt->sizemin) / (dt->sizemax - dt->sizemin);
            eduiGradStageAddRGB(widthgradmenu, dt->widthenv[i].time, ratio, ratio, ratio);
            if (dt->widthenv[i].time == 1.0f) {
                break;
            }
        }
    }
    GenericDebinfoDmaTypeUpdate(dt);
}


void cbPtlApplySize(void) {
    struct edptlgradkey_s keys[8];
    struct debtypedef_s *dt;
    struct edptlitem_s *item;
    float lo;
    float hi;
    int count;
    int i;

    if (sizeminitem == 0 || sizemaxitem == 0 || heightgradmenu == 0 || widthgradmenu == 0) {
        return;
    }
    count = eduiGradPickRead(widthgradmenu, keys, 8);
    item = sizeminitem;
    lo = item->fvalue;
    item = sizemaxitem;
    hi = item->fvalue;
    if (count >= 2 && count <= 8 && edpp_nearest != -1
        && edpp_ptls[edpp_nearest].debris != -1) {
        dt = debtab[debkeydata[edpp_ptls[edpp_nearest].debris].debtype];
        for (i = 0; i < count; i++) {
            dt->widthenv[i].time = keys[i].time;
            dt->widthenv[i].value = lo + keys[i].value[0] * (hi - lo);
        }
        dt->sizemin = lo;
        dt->sizemax = hi;
        GenericDebinfoDmaTypeUpdate(dt);
    }
    count = eduiGradPickRead(heightgradmenu, keys, 8);
    if (count >= 2 && count <= 8 && edpp_nearest != -1
        && edpp_ptls[edpp_nearest].debris != -1) {
        dt = debtab[debkeydata[edpp_ptls[edpp_nearest].debris].debtype];
        for (i = 0; i < count; i++) {
            dt->heightenv[i].time = keys[i].time;
            dt->heightenv[i].value = lo + keys[i].value[0] * (hi - lo);
        }
        dt->sizemin = lo;
        dt->sizemax = hi;
        GenericDebinfoDmaTypeUpdate(dt);
    }
}


void cbPtlApplyRot(void) {
    struct edptlgradkey_s keys[8];
    struct debkey_s *deb;
    struct debtypedef_s *dt;
    struct edptlitem_s *item;
    float lo;
    float hi;
    int count;
    int i;

    if (rotminitem == 0 || rotmaxitem == 0 || rotgradmenu == 0) {
        return;
    }
    count = eduiGradPickRead(rotgradmenu, keys, 8);
    item = rotminitem;
    lo = item->fvalue;
    item = rotmaxitem;
    hi = item->fvalue;
    if (lo == hi) {
        hi = hi + 0.1;
    }
    if (count >= 2 && count <= 8 && edpp_nearest != -1
        && edpp_ptls[edpp_nearest].debris != -1) {
        deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
        dt = debtab[deb->debtype];
        for (i = 0; i < count; i++) {
            dt->rotenv[i].time = keys[i].time;
            dt->rotenv[i].value = (int)((lo + keys[i].value[0] * (hi - lo)) * 182.04445f);
        }
        dt->rotmin = lo;
        dt->rotmax = hi;
        GenericDebinfoDmaTypeUpdate(dt);
    }
}


void cbPtlSelGSort(struct edptlmenu_s *menu, struct edptlitem_s *item) {
    struct debtypedef_s *dt;

    eduiMenuDetach(menu);
    edpp_active_menu = 0;
    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    dt = debtab[debkeydata[edpp_ptls[edpp_nearest].debris].debtype];
    if (dt->gensort != item->value) {
        dt->gensort = item->value;
        debkeydata[edpp_ptls[edpp_nearest].debris].gensort = gensorttab[item->value];
        memset(&dt->varemit, 0, sizeof(struct nuvec_s));
        memset(&dt->varemitrand, 0, sizeof(struct nuvec_s));
        memset(&dt->varstart, 0, sizeof(struct nuvec_s));
        memset(&dt->varstartrand, 0, sizeof(struct nuvec_s));
    }
}


void cbEffectListMenu(struct edptlmenu_s *menu) {
    struct edptlfont_s font;

    font = edpp_menu_font;
    effectlistmenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, cbCancelEffectListMenu,
                                    effectlisttitle);
    if (effectlistmenu != 0) {
        eduiMenuAddItem(effectlistmenu, eduiItemCheckCreate(0, &font, edpp_effect_list == 0, 1,
                                                            cbSelEffectList, effectlistgeneral));
        eduiMenuAddItem(effectlistmenu, eduiItemCheckCreate(1, &font, edpp_effect_list == 1, 1,
                                                            cbSelEffectList, effectlistlevel));
        eduiMenuAddItem(effectlistmenu, eduiItemCheckCreate(5, &font, edpp_effect_list == 5, 1,
                                                            cbSelEffectList, effectlistchar));
        eduiMenuAttach(menu, effectlistmenu);
        effectlistmenu->x = menu->x + 10;
        effectlistmenu->y = menu->y + 40;
    }
}


void edptlcbSwitchMenu(struct edptlmenu_s *menu) {
    struct edptlfont_s font;

    font = edpp_menu_font;
    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    edptl_switch_menu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, edptlcbCancelSwitchMenu,
                                       switchmenutitle);
    if (edptl_switch_menu != 0) {
        eduiMenuAddItem(edptl_switch_menu, eduiItemSelCreate(1, &font, 0, 0, edptlcbSwitchTypeMenu,
                                                             switchtypelabel));
        eduiMenuAddItem(edptl_switch_menu,
                        eduiItemSliderCreateInt(0, &font, 0, edptlcbSetSwitchId, -1, 0x81,
                                                edpp_ptls[edpp_nearest].switchid, switchidlabel));
        eduiMenuAddItem(edptl_switch_menu,
                        eduiItemSliderCreate(0, &font, 0, edptlcbSetSwitchVar, 0.0f, 20.0f,
                                             edpp_ptls[edpp_nearest].switchvar, switchvarlabel));
        eduiMenuAttach(menu, edptl_switch_menu);
        edptl_switch_menu->x = menu->x + 10;
        edptl_switch_menu->y = menu->y + 40;
    }
}


void edptlcbGroupMenu(struct edptlmenu_s *menu) {
    struct edptlfont_s font;

    font = edpp_menu_font;
    edptl_group_menu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, edptlcbCancelGroupMenu,
                                      grouptitle);
    if (edptl_group_menu != 0) {
        eduiMenuAddItem(edptl_group_menu,
                        eduiItemSliderCreateInt(0, &font, 0, edptlcbSetMasterGroup, 0, 0x20,
                                                debris_render_group, mastergrouplabel));
        if (edpp_nearest != -1 && edpp_ptls[edpp_nearest].debris != -1) {
            eduiMenuAddItem(edptl_group_menu,
                            eduiItemSliderCreateInt(0, &font, 0, edptlcbSetGroup, 0, 0x20,
                                                    edpp_ptls[edpp_nearest].group, grouplabel));
        }
        eduiMenuAttach(menu, edptl_group_menu);
        edptl_group_menu->x = menu->x + 10;
        edptl_group_menu->y = menu->y + 40;
    }
}


void edptlcbDpadModeMenu(struct edptlmenu_s *menu) {
    struct edptlfont_s font;

    font = edpp_menu_font;
    dpadmodemenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, edptlcbCancelDpadModeMenu,
                                  dpadmodetitle);
    if (dpadmodemenu != 0) {
        eduiMenuAddItem(dpadmodemenu, eduiItemCheckCreate(0, &font, edpp_dpad_mode == 0, 1,
                                                          edptlcbSetDpadMode, dpademitrotlabel));
        eduiMenuAddItem(dpadmodemenu, eduiItemCheckCreate(1, &font, edpp_dpad_mode == 1, 1,
                                                          edptlcbSetDpadMode, dpadgravrotlabel));
        eduiMenuAddItem(dpadmodemenu, eduiItemCheckCreate(2, &font, edpp_dpad_mode == 2, 1,
                                                          edptlcbSetDpadMode, dpadoffsetlabel));
        eduiMenuAddItem(dpadmodemenu, eduiItemCheckCreate(3, &font, edpp_dpad_mode == 3, 1,
                                                          edptlcbSetDpadMode, dpadreflectlabel));
        eduiMenuAttach(menu, dpadmodemenu);
        dpadmodemenu->x = menu->x + 10;
        dpadmodemenu->y = menu->y + 40;
    }
}


void cbPtlTypeMenu(struct edptlmenu_s *menu) {
    struct edptlfont_s font;
    int checked;
    int i;

    font = edpp_menu_font;
    ptltypemenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, cbPtlCancelTypeMenu,
                                 typemenutitle);
    if (ptltypemenu != 0) {
        eduiMenuAddItem(ptltypemenu, eduiItemSelCreate(1, &font, 0, 0, cbPtlAddEffect,
                                                       addeffectlabel));
        for (i = 1; i < 128; i++) {
            if (debtab[i] != 0 && debtab[i]->effectlist == edpp_effect_list) {
                checked = 0;
                if (edpp_create_type == i) {
                    checked = 1;
                }
                eduiMenuAddItem(ptltypemenu, eduiItemCheckCreate(i, &font, checked, 1,
                                                                 cbPtlSelType, debtab[i]->name));
                if (checked == 1) {
                    ptltypemenu->curitem = edui_last_item;
                }
            }
        }
        eduiMenuAttach(menu, ptltypemenu);
        ptltypemenu->x = menu->x + 10;
        ptltypemenu->y = menu->y + 40;
    }
}


void cbPtlColMenu(struct edptlmenu_s *menu) {
    struct edptlfont_s font;
    struct debkey_s *deb;
    struct debtypedef_s *dt;
    int i;

    font = edpp_menu_font;
    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->debtype];
    ptlcolmenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, cbPtlCancelColMenu, colmenutitle);
    if (ptlcolmenu != 0) {
        eduiMenuAddItem(ptlcolmenu, eduiItemGradPickCreate(0, &font, cbPtlApplyGrad, colgradlabel));
        colgradmenu = edui_last_item;
        for (i = 0; i < 8; i++) {
            eduiGradStageAddRGB(colgradmenu, dt->colenv[i].time, dt->colenv[i].r / 255.0,
                                dt->colenv[i].g / 255.0, dt->colenv[i].b / 255.0);
            if (dt->colenv[i].time == 1.0f) {
                break;
            }
        }
        eduiMenuAddItem(ptlcolmenu, eduiItemGreyGradPickCreate(0, &font, cbPtlApplyGrad,
                                                               alphagradlabel));
        alphagradmenu = edui_last_item;
        for (i = 0; i < 8; i++) {
            eduiGradStageAddRGB(alphagradmenu, dt->alphaenv[i].time, dt->alphaenv[i].value / 255.0,
                                dt->alphaenv[i].value / 255.0, dt->alphaenv[i].value / 255.0);
            if (dt->alphaenv[i].time == 1.0f) {
                break;
            }
        }
        eduiMenuAttach(menu, ptlcolmenu);
        ptlcolmenu->x = menu->x + 10;
        ptlcolmenu->y = menu->y + 40;
    }
}


void cbPtlJibMenu(struct edptlmenu_s *menu) {
    struct edptlfont_s font;
    struct debkey_s *deb;
    struct debtypedef_s *dt;

    font = edpp_menu_font;
    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->debtype];
    ptljibmenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0x12C, app_fnt, cbPtlCancelJibMenu, jibmenutitle);
    if (ptljibmenu != 0) {
        eduiMenuAddItem(ptljibmenu, eduiItemSliderCreate(0, &font, 0, cbPtlApplyJib, 0.0f, 32.0f,
                                                         dt->jib[0], jibxfreqlabel));
        ptljibitem0 = edui_last_item;
        eduiMenuAddItem(ptljibmenu, eduiItemSliderCreate(0, &font, 0, cbPtlApplyJib, 0.0f, 1000.0f,
                                                         dt->jib[1], jibxamplabel));
        ptljibitem1 = edui_last_item;
        eduiMenuAddItem(ptljibmenu, eduiItemSliderCreate(0, &font, 0, cbPtlApplyJib, 0.0f, 32.0f,
                                                         dt->jib[2], jibyfreqlabel));
        ptljibitem2 = edui_last_item;
        eduiMenuAddItem(ptljibmenu, eduiItemSliderCreate(0, &font, 0, cbPtlApplyJib, 0.0f, 1000.0f,
                                                         dt->jib[3], jibyamplabel));
        ptljibitem3 = edui_last_item;
        eduiMenuAttach(menu, ptljibmenu);
        ptljibmenu->x = menu->x + 10;
        ptljibmenu->y = menu->y + 40;
    }
}


void edptlcbBounceMenu(struct edptlmenu_s *menu) {
    struct edptlfont_s font;
    struct debkey_s *deb;

    font = edpp_menu_font;
    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    edptl_bounce_menu = eduiMenuCreate(0x46, 0x46, 0xC8, 0x12C, app_fnt, edptlcbCancelBounceMenu,
                                       bouncetitle);
    if (edptl_bounce_menu != 0) {
        eduiMenuAddItem(edptl_bounce_menu,
                        eduiItemSliderCreate(0, &font, 0, edptlcbApplyBounceOffset, -2.0f, 4.0f,
                                             deb->bounceoffset, bounceoffsetlabel));
        eduiMenuAddItem(edptl_bounce_menu,
                        eduiItemSliderCreate(0, &font, 0, edptlcbApplyBounceFactor, 0.0f, 2.0f,
                                             deb->bouncefactor, bouncefactorlabel));
        eduiMenuAttach(menu, edptl_bounce_menu);
        edptl_bounce_menu->x = menu->x + 10;
        edptl_bounce_menu->y = menu->y + 40;
    }
}


void edptlcbSoundIDMenu(struct edptlmenu_s *menu, struct edptlitem_s *item) {
    struct edptlfont_s font;
    struct debkey_s *deb;
    struct debtypedef_s *dt;
    int checked;
    int i;

    font = edpp_menu_font;
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->debtype];
    edptl_soundid_menu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, edptlcbCancelSoundIDMenu,
                                        soundidtitle);
    if (edptl_soundid_menu != 0) {
        eduiMenuAddItem(edptl_soundid_menu,
                        eduiItemCheckCreate((item->value << 16) | 9999, &font,
                                            dt->sounds[item->value].id == -1, 0,
                                            edptlcbSetSoundID, soundnonelabel));
        for (i = 0; i < edSfxAllCount; i++) {
            checked = 0;
            if (dt->sounds[item->value].id == i) {
                checked = 1;
            }
            eduiMenuAddItem(edptl_soundid_menu,
                            eduiItemCheckCreate((item->value << 16) + i, &font, checked, 1,
                                                edptlcbSetSoundID, edbitsGetSoundName(i)));
            if (checked == 1) {
                edptl_soundid_menu->curitem = edui_last_item;
            }
        }
        eduiMenuAttach(menu, edptl_soundid_menu);
        edptl_soundid_menu->x = menu->x + 10;
        edptl_soundid_menu->y = menu->y + 40;
    }
}


void edptlcbSoundControlMenu(struct edptlmenu_s *menu, struct edptlitem_s *item) {
    struct edptlfont_s font;
    struct debkey_s *deb;
    struct debtypedef_s *dt;

    font = edpp_menu_font;
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->debtype];
    edptl_soundcontrol_menu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt,
                                             edptlcbCancelSoundControlMenu, soundcontroltitle);
    if (edptl_soundcontrol_menu != 0) {
        eduiMenuAddItem(edptl_soundcontrol_menu,
                        eduiItemCheckCreate(item->value << 16, &font,
                                            dt->sounds[item->value].control == 0, 1,
                                            edptlcbSetSoundControl, soundofflabel));
        if (edui_last_item->toggle) {
            edptl_soundcontrol_menu->curitem = edui_last_item;
        }
        eduiMenuAddItem(edptl_soundcontrol_menu,
                        eduiItemCheckCreate((item->value << 16) | 1, &font,
                                            dt->sounds[item->value].control == 1, 1,
                                            edptlcbSetSoundControl, soundonedgelabel));
        if (edui_last_item->toggle) {
            edptl_soundcontrol_menu->curitem = edui_last_item;
        }
        eduiMenuAddItem(edptl_soundcontrol_menu,
                        eduiItemCheckCreate((item->value << 16) | 2, &font,
                                            dt->sounds[item->value].control == 2, 1,
                                            edptlcbSetSoundControl, soundoffedgelabel));
        if (edui_last_item->toggle) {
            edptl_soundcontrol_menu->curitem = edui_last_item;
        }
        eduiMenuAddItem(edptl_soundcontrol_menu,
                        eduiItemCheckCreate((item->value << 16) | 3, &font,
                                            dt->sounds[item->value].control == 3, 1,
                                            edptlcbSetSoundControl, soundperparticlelabel));
        if (edui_last_item->toggle) {
            edptl_soundcontrol_menu->curitem = edui_last_item;
        }
        eduiMenuAddItem(edptl_soundcontrol_menu,
                        eduiItemCheckCreate((item->value << 16) | 4, &font,
                                            dt->sounds[item->value].control == 4, 1,
                                            edptlcbSetSoundControl, soundregularlabel));
        if (edui_last_item->toggle) {
            edptl_soundcontrol_menu->curitem = edui_last_item;
        }
        eduiMenuAttach(menu, edptl_soundcontrol_menu);
        edptl_soundcontrol_menu->x = menu->x + 10;
        edptl_soundcontrol_menu->y = menu->y + 40;
    }
}


void edptlcbSoundXMenu(struct edptlmenu_s *menu, struct edptlitem_s *item) {
    struct edptlfont_s font;
    char title[16];
    struct debkey_s *deb;
    struct debtypedef_s *dt;

    font = edpp_menu_font;
    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->debtype];
    sprintf(title, soundxtitleformat, item->value + 1);
    edptl_soundx_menu = eduiMenuCreate(0x46, 0x46, 0xFA, 0x12C, app_fnt, edptlcbCancelSoundXMenu,
                                       title);
    if (edptl_soundx_menu != 0) {
        eduiMenuAddItem(edptl_soundx_menu, eduiItemSelCreate(item->value, &font, 0, 0,
                                                             edptlcbSoundIDMenu, soundidlabel));
        eduiMenuAddItem(edptl_soundx_menu, eduiItemSelCreate(item->value, &font, 0, 0,
                                                             edptlcbSoundControlMenu,
                                                             soundcontrollabel));
        eduiMenuAddItem(edptl_soundx_menu,
                        eduiItemSliderCreateInt(item->value, &font, 0, edptlcbChangeSoundDelay, 0, 0x32,
                                                dt->sounds[item->value].delay, sounddelaylabel));
        eduiMenuAttach(menu, edptl_soundx_menu);
        edptl_soundx_menu->x = menu->x + 10;
        edptl_soundx_menu->y = menu->y + 40;
    }
}


void cbPtlCollMenu(struct edptlmenu_s *menu) {
    struct edptlfont_s font;
    struct debkey_s *deb;
    struct debtypedef_s *dt;
    float ratio;
    int i;

    font = edpp_menu_font;
    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->debtype];
    collmenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0x12C, app_fnt, cbPtlCancelCollMenu, collmenutitle);
    if (collmenu != 0) {
        eduiMenuAddItem(collmenu, eduiItemGreyGradPickCreate(0, &font, cbPtlApplyCollEnv,
                                                             collenvlabel));
        collenvmenu = edui_last_item;
        for (i = 0; i < 8; i++) {
            if (dt->sizemin == dt->sizemax) {
                ratio = 1.0f;
            } else {
                ratio = (dt->collenv[i].value - dt->sizemin / 10000.0f)
                      / ((dt->sizemax - dt->sizemin) / 10000.0f);
            }
            eduiGradStageAddRGB(collenvmenu, dt->collenv[i].time, ratio, ratio, ratio);
            if (dt->collenv[i].time == 1.0f) {
                break;
            }
        }
        eduiMenuAddItem(collmenu, eduiItemSliderCreateInt(0, &font, 0, cbChangeNumCollSpheres, 0, 8,
                                                          dt->numcollspheres, numsphereslabel));
        eduiMenuAddItem(collmenu, eduiItemSelCreate(1, &font, 0, 0, cbPtlDefaultCollEnv,
                                                    defaultenvlabel));
        eduiMenuAttach(menu, collmenu);
        collmenu->x = menu->x + 10;
        collmenu->y = menu->y + 40;
    }
}


void cbPtlSizeMenu(struct edptlmenu_s *menu) {
    struct edptlfont_s font;
    struct debkey_s *deb;
    struct debtypedef_s *dt;
    float ratio;
    int i;

    font = edpp_menu_font;
    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->debtype];
    ptlsizemenu = eduiMenuCreate(0x46, 0x46, 0xDC, 0x12C, app_fnt, cbPtlCancelSizeMenu,
                                 sizemenutitle);
    if (ptlsizemenu != 0) {
        eduiMenuAddItem(ptlsizemenu, eduiItemGreyGradPickCreate(0, &font, cbPtlApplySize,
                                                                widthenvlabel));
        widthgradmenu = edui_last_item;
        for (i = 0; i < 8; i++) {
            if (dt->sizemin == dt->sizemax) {
                ratio = 1.0f;
            } else {
                ratio = (dt->widthenv[i].value - dt->sizemin) / (dt->sizemax - dt->sizemin);
            }
            eduiGradStageAddRGB(widthgradmenu, dt->widthenv[i].time, ratio, ratio, ratio);
            if (dt->widthenv[i].time == 1.0f) {
                break;
            }
        }
        eduiMenuAddItem(ptlsizemenu, eduiItemGreyGradPickCreate(0, &font, cbPtlApplySize,
                                                                heightenvlabel));
        heightgradmenu = edui_last_item;
        for (i = 0; i < 8; i++) {
            if (dt->sizemin == dt->sizemax) {
                ratio = 1.0f;
            } else {
                ratio = (dt->heightenv[i].value - dt->sizemin) / (dt->sizemax - dt->sizemin);
            }
            eduiGradStageAddRGB(heightgradmenu, dt->heightenv[i].time, ratio, ratio, ratio);
            if (dt->heightenv[i].time == 1.0f) {
                break;
            }
        }
        eduiMenuAddItem(ptlsizemenu, eduiItemSliderCreate(0, &font, 0, cbPtlApplySize, 0.0f,
                                                          edpp_slider_scale * 5000.0, dt->sizemin,
                                                          minsizelabel));
        sizeminitem = edui_last_item;
        eduiItemSliderSetFmt(edui_last_item, onedecimalfmt);
        eduiItemSliderSetGranularity(edui_last_item, 0.1f);
        eduiMenuAddItem(ptlsizemenu, eduiItemSliderCreate(0, &font, 0, cbPtlApplySize, 0.0f,
                                                          edpp_slider_scale * 5000.0, dt->sizemax,
                                                          maxsizelabel));
        sizemaxitem = edui_last_item;
        eduiItemSliderSetFmt(edui_last_item, onedecimalfmt);
        eduiItemSliderSetGranularity(edui_last_item, 0.1f);
        eduiMenuAddItem(ptlsizemenu, eduiItemSelCreate(1, &font, 0, 0, cbPtlCopySize,
                                                       copywidthlabel));
        eduiMenuAddItem(ptlsizemenu, eduiItemSelCreate(2, &font, 0, 0, cbPtlCopySize,
                                                       copyheightlabel));
        eduiMenuAttach(menu, ptlsizemenu);
        ptlsizemenu->x = menu->x + 10;
        ptlsizemenu->y = menu->y + 40;
    }
}


void cbPtlRotMenu(struct edptlmenu_s *menu) {
    struct edptlfont_s font;
    struct debkey_s *deb;
    struct debtypedef_s *dt;
    float ratio;
    int i;

    font = edpp_menu_font;
    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->debtype];
    ptlrotmenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0x12C, app_fnt, cbPtlCancelRotMenu, rotmenutitle);
    if (ptlrotmenu != 0) {
        eduiMenuAddItem(ptlrotmenu, eduiItemGreyGradPickCreate(0, &font, cbPtlApplyRot,
                                                               rotenvlabel));
        rotgradmenu = edui_last_item;
        for (i = 0; i < 8; i++) {
            if (dt->rotmin == dt->rotmax) {
                ratio = 1.0f;
            } else {
                ratio = (dt->rotenv[i].value * (360.0f / 65536.0f) - dt->rotmin)
                      / (dt->rotmax - dt->rotmin);
            }
            eduiGradStageAddRGB(rotgradmenu, dt->rotenv[i].time, ratio, ratio, ratio);
            if (dt->rotenv[i].time == 1.0f) {
                break;
            }
        }
        eduiMenuAddItem(ptlrotmenu, eduiItemSliderCreate(0, &font, 0, cbPtlApplyRot, -720.0f,
                                                         1440.0f, dt->rotmin, minrotlabel));
        rotminitem = edui_last_item;
        eduiItemSliderSetFmt(edui_last_item, onedecimalfmt);
        eduiItemSliderSetGranularity(edui_last_item, 0.1f);
        eduiMenuAddItem(ptlrotmenu, eduiItemSliderCreate(0, &font, 0, cbPtlApplyRot, -720.0f,
                                                         1440.0f, dt->rotmax, maxrotlabel));
        rotmaxitem = edui_last_item;
        eduiItemSliderSetFmt(edui_last_item, onedecimalfmt);
        eduiItemSliderSetGranularity(edui_last_item, 0.1f);
        eduiMenuAttach(menu, ptlrotmenu);
        ptlrotmenu->x = menu->x + 10;
        ptlrotmenu->y = menu->y + 40;
    }
}


void cbPtlCutOffMenu(struct edptlmenu_s *menu) {
    struct edptlfont_s font;
    struct debkey_s *deb;
    struct debtypedef_s *dt;

    font = edpp_menu_font;
    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->debtype];
    ptlcutoffmenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0x12C, app_fnt, cbPtlCancelCutOffMenu,
                                   cutoffmenutitle);
    if (ptlcutoffmenu != 0) {
        eduiMenuAddItem(ptlcutoffmenu, eduiItemSliderCreate(0, &font, 0, cbPtlChangeCutOff, 0.0f,
                                                            edpp_slider_scale * 25.0, dt->cutoff,
                                                            cutofflabel));
        eduiMenuAddItem(ptlcutoffmenu, eduiItemSliderCreate(0, &font, 0, cbPtlChangeCutOn, 0.0f,
                                                            edpp_slider_scale * 25.0, dt->cuton,
                                                            cutonlabel));
        eduiMenuAddItem(ptlcutoffmenu, eduiItemSliderCreate(0, &font, 0, cbPtlChangeDrawCutOff,
                                                            0.0f, edpp_slider_scale * 25.0,
                                                            dt->drawcutoff, drawcutofflabel));
        eduiMenuAttach(menu, ptlcutoffmenu);
        ptlcutoffmenu->x = menu->x + 10;
        ptlcutoffmenu->y = menu->y + 40;
    }
}


void cbPtlVarEmitMenu(struct edptlmenu_s *menu) {
    struct edptlfont_s font;
    struct debkey_s *deb;
    struct debtypedef_s *dt;

    font = edpp_menu_font;
    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->debtype];
    ptlvaremitmenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0x12C, app_fnt, cbPtlCancelVarEmitMenu,
                                    varemitmenutitle);
    if (ptlvaremitmenu != 0) {
        if (dt->gensort == 0 || dt->gensort == 8 || dt->gensort == 9 || dt->gensort == 10) {
            eduiMenuAddItem(ptlvaremitmenu,
                            eduiItemSliderCreate(0, &font, 0, cbPtlChangeX, 0.0f,
                                                 edpp_slider_scale * 5.0, dt->varemit.x,
                                                 randemitxlabel));
            eduiMenuAddItem(ptlvaremitmenu,
                            eduiItemSliderCreate(0, &font, 0, cbPtlChangeY, 0.0f,
                                                 edpp_slider_scale * 5.0, dt->varemit.y,
                                                 randemitylabel));
            eduiMenuAddItem(ptlvaremitmenu,
                            eduiItemSliderCreate(0, &font, 0, cbPtlChangeZ, 0.0f,
                                                 edpp_slider_scale * 5.0, dt->varemit.z,
                                                 randemitzlabel));
        } else if (dt->gensort == 6) {
            eduiMenuAddItem(ptlvaremitmenu,
                            eduiItemSliderCreate(0, &font, 0, cbPtlChangeX, 0.0f,
                                                 edpp_slider_scale * 5.0, dt->varemit.x,
                                                 randmagxlabel));
            eduiMenuAddItem(ptlvaremitmenu,
                            eduiItemSliderCreate(0, &font, 0, cbPtlChangeY, 0.0f, 180.0f,
                                                 dt->varemit.y * (360.0f / 65536.0f),
                                                 randrotylabel));
            eduiItemSliderSetFmt(edui_last_item, onedecimalfmt);
            eduiItemSliderSetGranularity(edui_last_item, 0.1f);
            eduiMenuAddItem(ptlvaremitmenu,
                            eduiItemSliderCreate(0, &font, 0, cbPtlChangeZ, 0.0f, 180.0f,
                                                 dt->varemit.z * (360.0f / 65536.0f),
                                                 randrotzlabel));
            eduiItemSliderSetFmt(edui_last_item, onedecimalfmt);
            eduiItemSliderSetGranularity(edui_last_item, 0.1f);
        } else if (dt->gensort == 7) {
            eduiMenuAddItem(ptlvaremitmenu,
                            eduiItemSliderCreate(0, &font, 0, cbPtlChangeX, 0.0f,
                                                 edpp_slider_scale * 5.0, dt->varemit.x,
                                                 randmaglabel));
            eduiMenuAddItem(ptlvaremitmenu,
                            eduiItemSliderCreate(0, &font, 0, cbPtlChangeY, -90.0f, 180.0f,
                                                 dt->varemit.y * (360.0f / 65536.0f),
                                                 steprotylabel));
            eduiItemSliderSetFmt(edui_last_item, onedecimalfmt);
            eduiItemSliderSetGranularity(edui_last_item, 0.1f);
            eduiMenuAddItem(ptlvaremitmenu,
                            eduiItemSliderCreate(0, &font, 0, cbPtlChangeZ, -90.0f, 180.0f,
                                                 dt->varemit.z * (360.0f / 65536.0f),
                                                 steprotzlabel));
            eduiItemSliderSetFmt(edui_last_item, onedecimalfmt);
            eduiItemSliderSetGranularity(edui_last_item, 0.1f);
        }
        eduiMenuAttach(menu, ptlvaremitmenu);
        ptlvaremitmenu->x = menu->x + 10;
        ptlvaremitmenu->y = menu->y + 40;
    }
}


void cbPtlVarStartMenu(struct edptlmenu_s *menu) {
    struct edptlfont_s font;
    struct debkey_s *deb;
    struct debtypedef_s *dt;

    font = edpp_menu_font;
    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->debtype];
    if (dt->gensort == 0 || dt->gensort == 8 || dt->gensort == 9 || dt->gensort == 10) {
        ptlvarstartmenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0x12C, app_fnt, cbPtlCancelVarStartMenu,
                                         randstarttitle);
    } else if (dt->gensort == 6 || dt->gensort == 7) {
        ptlvarstartmenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0x12C, app_fnt, cbPtlCancelVarStartMenu,
                                         baseemittitle);
    }
    if (ptlvarstartmenu != 0) {
        if (dt->gensort == 0 || dt->gensort == 8 || dt->gensort == 9 || dt->gensort == 10) {
            eduiMenuAddItem(ptlvarstartmenu,
                            eduiItemSliderCreate(0, &font, 0, cbPtlChangeX, 0.0f,
                                                 edpp_slider_scale * 5.0, dt->varstart.x,
                                                 randstartxlabel));
            eduiMenuAddItem(ptlvarstartmenu,
                            eduiItemSliderCreate(0, &font, 0, cbPtlChangeY, 0.0f,
                                                 edpp_slider_scale * 5.0, dt->varstart.y,
                                                 randstartylabel));
            eduiMenuAddItem(ptlvarstartmenu,
                            eduiItemSliderCreate(0, &font, 0, cbPtlChangeZ, 0.0f,
                                                 edpp_slider_scale * 5.0, dt->varstart.z,
                                                 randstartzlabel));
        } else if (dt->gensort == 6 || dt->gensort == 7) {
            eduiMenuAddItem(ptlvarstartmenu,
                            eduiItemSliderCreate(0, &font, 0, cbPtlChangeX, 0.0f,
                                                 edpp_slider_scale * 5.0, dt->varstart.x,
                                                 basemaglabel));
            eduiMenuAddItem(ptlvarstartmenu,
                            eduiItemSliderCreate(0, &font, 0, cbPtlChangeY, -180.0f, 360.0f,
                                                 dt->varstart.y * (360.0f / 65536.0f),
                                                 baserotylabel));
            eduiItemSliderSetFmt(edui_last_item, onedecimalfmt);
            eduiItemSliderSetGranularity(edui_last_item, 0.1f);
            eduiMenuAddItem(ptlvarstartmenu,
                            eduiItemSliderCreate(0, &font, 0, cbPtlChangeZ, -180.0f, 180.0f,
                                                 dt->varstart.z * (360.0f / 65536.0f),
                                                 baserotzlabel));
            eduiItemSliderSetFmt(edui_last_item, onedecimalfmt);
            eduiItemSliderSetGranularity(edui_last_item, 0.1f);
        }
        eduiMenuAttach(menu, ptlvarstartmenu);
        ptlvarstartmenu->x = menu->x + 10;
        ptlvarstartmenu->y = menu->y + 40;
    }
}


void cbPtlTextureSelectMenu(struct edptlmenu_s *menu) {
    struct edptlfont_s font;
    struct debkey_s *deb;
    struct debtypedef_s *dt;
    struct edptltexitem_s *item;

    font = edpp_menu_font;
    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->debtype];
    if (textureselectmenu == 0) {
        textureselectmenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0x12C, app_fnt, 0,
                                           textureselecttitle);
        if (textureselectmenu == 0) {
            return;
        }
        eduiMenuAddItem(textureselectmenu,
                        eduiItemTexturePickCreate(0, &font, cbPtlChangeTextureSelect,
                                                  textureselecttitle));
        item = (struct edptltexitem_s *)edui_last_item;
        item->mtl = DebMat[1];
        item->x0 = (dt->texu0 - 524288.0f) * (1.0f / 256.0f);
        item->x1 = (dt->texu1 - 524288.0f) * (1.0f / 256.0f);
        item->y0 = (dt->texv0 - 524288.0f) * (1.0f / 256.0f);
        item->y1 = (dt->texv1 - 524288.0f) * (1.0f / 256.0f);
    }
    if (textureselectmenu != 0) {
        eduiMenuAttach(menu, textureselectmenu);
        textureselectmenu->x = menu->x + 10;
        textureselectmenu->y = menu->y + 40;
    }
}


void cbPtlTextureMenu(struct edptlmenu_s *menu) {
    struct edptlfont_s font;
    struct debkey_s *deb;
    struct debtypedef_s *dt;

    font = edpp_menu_font;
    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->debtype];
    texturemenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0x12C, app_fnt, cbPtlCancelTextureMenu,
                                 texturemenutitle);
    if (texturemenu != 0) {
        eduiMenuAddItem(texturemenu, eduiItemCheckCreate(0, &font, dt->texturetype == 0, 1,
                                                         cbPtlSelTextureType, additivelabel));
        eduiMenuAddItem(texturemenu, eduiItemCheckCreate(2, &font, dt->texturetype == 2, 1,
                                                         cbPtlSelTextureType, modulationlabel));
        eduiMenuAddItem(texturemenu, eduiItemCheckCreate(3, &font, dt->texturetype == 3, 1,
                                                         cbPtlSelTextureType, subtractivelabel));
        eduiMenuAddItem(texturemenu, eduiItemSelCreate(1, &font, 0, 0, cbPtlTextureSelectMenu,
                                                       textureselectorlabel));
        eduiMenuAttach(menu, texturemenu);
        texturemenu->x = menu->x + 10;
        texturemenu->y = menu->y + 40;
    }
}


void cbPtlEmitTimeMenu(struct edptlmenu_s *menu) {
    struct edptlfont_s font;
    struct debkey_s *deb;
    struct debtypedef_s *dt;

    font = edpp_menu_font;
    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->debtype];
    emittimemenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0x12C, app_fnt, cbPtlCancelEmitTimeMenu,
                                  emittimemenutitle);
    if (emittimemenu != 0) {
        eduiMenuAddItem(emittimemenu,
                        eduiItemSliderCreateInt(0, &font, 0, cbPtlChangeIvalOn, 1,
                                                edpp_slider_scale * 120.0 - 1.0, dt->ivalon,
                                                ontimelabel));
        eduiMenuAddItem(emittimemenu,
                        eduiItemSliderCreateInt(0, &font, 0, cbPtlChangeIvalOnRan, 0,
                                                edpp_slider_scale * 120.0, dt->ivalonran,
                                                randontimelabel));
        eduiMenuAddItem(emittimemenu,
                        eduiItemSliderCreateInt(0, &font, 0, cbPtlChangeIvalOff, 0,
                                                edpp_slider_scale * 120.0, dt->ivaloff,
                                                offtimelabel));
        eduiMenuAddItem(emittimemenu,
                        eduiItemSliderCreateInt(0, &font, 0, cbPtlChangeIvalOffRan, 0,
                                                edpp_slider_scale * 120.0, dt->ivaloffran,
                                                randofftimelabel));
        eduiMenuAttach(menu, emittimemenu);
        emittimemenu->x = menu->x + 10;
        emittimemenu->y = menu->y + 40;
    }
}


void cbPtlEmitVelMenu(struct edptlmenu_s *menu) {
    struct edptlfont_s font;
    struct debkey_s *deb;
    struct debtypedef_s *dt;

    font = edpp_menu_font;
    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->debtype];
    ptlemitvelmenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, cbPtlCancelEmitVelMenu,
                                    emitveltitle);
    if (ptlemitvelmenu != 0) {
        eduiMenuAddItem(ptlemitvelmenu, eduiItemSliderCreate(0, &font, 0, cbPtlChangeEmitVel, 0.0f,
                                                             edpp_slider_scale * 10.0, dt->emitvel,
                                                             emitveltitle));
        eduiMenuAttach(menu, ptlemitvelmenu);
        ptlemitvelmenu->x = menu->x + 10;
        ptlemitvelmenu->y = menu->y + 40;
    }
}


void cbPtlGravMenu(struct edptlmenu_s *menu) {
    struct edptlfont_s font;
    struct debkey_s *deb;
    struct debtypedef_s *dt;

    font = edpp_menu_font;
    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->debtype];
    ptlgravmenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, cbPtlCancelGravMenu,
                                 gravitytitle);
    if (ptlgravmenu != 0) {
        eduiMenuAddItem(ptlgravmenu, eduiItemSliderCreate(0, &font, 0, cbPtlChangeGrav,
                                                          edpp_slider_scale * -10.0,
                                                          edpp_slider_scale * 20.0, dt->gravity,
                                                          gravitytitle));
        eduiMenuAttach(menu, ptlgravmenu);
        ptlgravmenu->x = menu->x + 10;
        ptlgravmenu->y = menu->y + 40;
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
        if (effecttypes[i].effectlist == list || list == 2) {
            numtypes++;
        }
    }
    EdFileSetMedia(1);
    if (EdFileOpen(filename, 1)) {
        EdFileWriteInt(15);
        EdFileWriteInt(numtypes);
        for (i = 1; i < effect_types_used; i++) {
            if (effecttypes[i].effectlist == list || list == 2) {
                EdFileWrite(effecttypes[i].name, 0x10);
                EdFileWriteShort(effecttypes[i].genrate);
                EdFileWriteShort(effecttypes[i].numptls);
                EdFileWriteShort(effecttypes[i].startoffset);
                EdFileWriteShort(effecttypes[i].ivalon);
                EdFileWriteShort(effecttypes[i].ivalonran);
                EdFileWriteShort(effecttypes[i].ivaloff);
                EdFileWriteShort(effecttypes[i].ivaloffran);
                EdFileWriteChar(effecttypes[i].gensort);
                EdFileWriteChar(effecttypes[i].gencode);
                EdFileWriteChar(effecttypes[i].texturetype);
                EdFileWriteChar(effecttypes[i].reserved23);
                EdFileWriteFloat(effecttypes[i].reserved24);
                EdFileWriteFloat(effecttypes[i].cuton);
                EdFileWriteFloat(effecttypes[i].cutoff);
                EdFileWriteFloat(effecttypes[i].drawcutoff);
                EdFileWriteFloat(effecttypes[i].emitvel);
                EdFileWrite(&effecttypes[i].varemit, 0xC);
                EdFileWrite(&effecttypes[i].varemitrand, 0xC);
                EdFileWrite(&effecttypes[i].varstart, 0xC);
                EdFileWrite(&effecttypes[i].varstartrand, 0xC);
                EdFileWriteFloat(effecttypes[i].posrange.x);
                EdFileWriteFloat(effecttypes[i].posrange.y);
                EdFileWriteFloat(effecttypes[i].posrange.z);
                EdFileWriteFloat(effecttypes[i].posbase.x);
                EdFileWriteFloat(effecttypes[i].posbase.y);
                EdFileWriteFloat(effecttypes[i].posbase.z);
                EdFileWriteFloat(effecttypes[i].momrange.x);
                EdFileWriteFloat(effecttypes[i].momrange.y);
                EdFileWriteFloat(effecttypes[i].momrange.z);
                EdFileWriteFloat(effecttypes[i].mombase.x);
                EdFileWriteFloat(effecttypes[i].mombase.y);
                EdFileWriteFloat(effecttypes[i].mombase.z);
                EdFileWriteFloat(effecttypes[i].gravity);
                EdFileWriteFloat(effecttypes[i].etime);
                EdFileWriteShort(effecttypes[i].frames);
                EdFileWriteChar(effecttypes[i].columns);
                EdFileWriteChar(effecttypes[i].rows);
                EdFileWriteFloat(effecttypes[i].frame);
                EdFileWriteFloat(effecttypes[i].jib[0]);
                EdFileWriteFloat(effecttypes[i].jib[1]);
                EdFileWriteFloat(effecttypes[i].jib[2]);
                EdFileWriteFloat(effecttypes[i].jib[3]);
                for (j = 0; j < 8; j++) {
                    EdFileWrite(&effecttypes[i].colenv[j], 0x10);
                }
                for (j = 0; j < 8; j++) {
                    EdFileWrite(&effecttypes[i].alphaenv[j], 8);
                }
                EdFileWriteFloat(effecttypes[i].sizemin);
                EdFileWriteFloat(effecttypes[i].sizemax);
                for (j = 0; j < 8; j++) {
                    EdFileWrite(&effecttypes[i].widthenv[j], 8);
                }
                for (j = 0; j < 8; j++) {
                    EdFileWrite(&effecttypes[i].heightenv[j], 8);
                }
                EdFileWriteFloat(effecttypes[i].rotmin);
                EdFileWriteFloat(effecttypes[i].rotmax);
                for (j = 0; j < 8; j++) {
                    EdFileWrite(&effecttypes[i].rotenv[j], 8);
                }
                for (j = 0; j < 8; j++) {
                    EdFileWrite(&effecttypes[i].extraenv0[j], 8);
                }
                for (j = 0; j < 8; j++) {
                    EdFileWrite(&effecttypes[i].extraenv1[j], 8);
                }
                EdFileWriteFloat(effecttypes[i].texu0);
                EdFileWriteFloat(effecttypes[i].texu1);
                EdFileWriteFloat(effecttypes[i].texv0);
                EdFileWriteFloat(effecttypes[i].texv1);
                for (j = 0; j < 8; j++) {
                    EdFileWrite(&effecttypes[i].collenv[j], 8);
                }
                EdFileWriteChar(effecttypes[i].numcollspheres);
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
                    EdFileWrite(debtab[edpp_ptls[i].type]->name, 0x10);
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


static inline void ShowMessageMenu(struct edptlmenu_s *parent, char *message, int flags) {
    struct edptlfont_s font;

    font = edpp_message_font;
    if (flags == 1) {
        font.colour = 0x8000C000;
    }
    messagemenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, cbCancelMessageMenu,
                                 messagemenutitle);
    if (messagemenu != 0) {
        eduiMenuAddItem(messagemenu, eduiItemSelCreate(1, &font, 0, 0, 0, message));
        eduiMenuAttach(parent, messagemenu);
        messagemenu->x = parent->x + 10;
        messagemenu->y = parent->y + 40;
    }
}


void cbFileSaveEffects(struct edptlmenu_s *menu) {
    char filename[256];

    if (edpp_level_type == 1) {
        if (edpp_effect_list == 0) {
            sprintf(filename, genericeffectsfile);
        } else {
            sprintf(filename, leveleffectsformat, edpp_level_path);
        }
        if (edppSaveEffects(filename, edpp_effect_list)) {
            ShowMessageMenu(menu, savedokmessage, 1);
        } else {
            ShowMessageMenu(menu, saveerrormessage, 0);
        }
    } else if (edpp_level_type == 2) {
        if (edpp_effect_list == 0) {
            sprintf(filename, generalptlfile);
        } else if (edpp_effect_list == 1) {
            sprintf(filename, levelptlformat, edpp_level_path);
        } else if (edpp_effect_list == 5) {
            sprintf(filename, charptlformat, edpp_level_path);
        } else {
            ShowMessageMenu(menu, saveerrormessage, 0);
        }
        if (edppSaveEffects(filename, edpp_effect_list)) {
            ShowMessageMenu(menu, savedokmessage, 1);
        } else {
            ShowMessageMenu(menu, saveerrormessage, 0);
        }
    } else {
        ShowMessageMenu(menu, saveerrormessage, 0);
    }
}


void FileLoadSingleEffectType(struct debtypedef_s *dt, int version, signed char list) {
    int i;

    EdFileRead(dt->name, 0x10);
    dt->effectlist = list;
    dt->genrate = EdFileReadShort();
    dt->numptls = EdFileReadShort();
    dt->startoffset = EdFileReadShort();
    dt->ivalon = EdFileReadShort();
    dt->ivalonran = EdFileReadShort();
    dt->ivaloff = EdFileReadShort();
    dt->ivaloffran = EdFileReadShort();
    dt->gensort = EdFileReadChar();
    dt->gencode = EdFileReadChar();
    dt->texturetype = EdFileReadChar();
    dt->reserved23 = EdFileReadChar();
    dt->reserved24 = EdFileReadFloat();
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
    dt->emitvel = EdFileReadFloat();
    EdFileRead(&dt->varemit, 0xC);
    EdFileRead(&dt->varemitrand, 0xC);
    EdFileRead(&dt->varstart, 0xC);
    EdFileRead(&dt->varstartrand, 0xC);
    dt->posrange.x = EdFileReadFloat();
    dt->posrange.y = EdFileReadFloat();
    dt->posrange.z = EdFileReadFloat();
    dt->posbase.x = EdFileReadFloat();
    dt->posbase.y = EdFileReadFloat();
    dt->posbase.z = EdFileReadFloat();
    dt->momrange.x = EdFileReadFloat();
    dt->momrange.y = EdFileReadFloat();
    dt->momrange.z = EdFileReadFloat();
    dt->mombase.x = EdFileReadFloat();
    dt->mombase.y = EdFileReadFloat();
    dt->mombase.z = EdFileReadFloat();
    dt->gravity = EdFileReadFloat();
    dt->etime = EdFileReadFloat();
    dt->frames = EdFileReadShort();
    dt->columns = EdFileReadChar();
    dt->rows = EdFileReadChar();
    dt->frame = EdFileReadFloat();
    dt->jib[0] = EdFileReadFloat();
    dt->jib[1] = EdFileReadFloat();
    dt->jib[2] = EdFileReadFloat();
    dt->jib[3] = EdFileReadFloat();
    for (i = 0; i < 8; i++) {
        EdFileRead(&dt->colenv[i], 0x10);
    }
    for (i = 0; i < 8; i++) {
        EdFileRead(&dt->alphaenv[i], 8);
    }
    dt->sizemin = EdFileReadFloat();
    dt->sizemax = EdFileReadFloat();
    for (i = 0; i < 8; i++) {
        EdFileRead(&dt->widthenv[i], 8);
    }
    for (i = 0; i < 8; i++) {
        EdFileRead(&dt->heightenv[i], 8);
    }
    dt->rotmin = EdFileReadFloat();
    dt->rotmax = EdFileReadFloat();
    for (i = 0; i < 8; i++) {
        EdFileRead(&dt->rotenv[i], 8);
    }
    for (i = 0; i < 8; i++) {
        EdFileRead(&dt->extraenv0[i], 8);
    }
    for (i = 0; i < 8; i++) {
        EdFileRead(&dt->extraenv1[i], 8);
    }
    dt->texu0 = EdFileReadFloat();
    dt->texu1 = EdFileReadFloat();
    dt->texv0 = EdFileReadFloat();
    dt->texv1 = EdFileReadFloat();
    if (version > 2) {
        for (i = 0; i < 8; i++) {
            EdFileRead(&dt->collenv[i], 8);
        }
        dt->numcollspheres = EdFileReadChar();
    } else {
        dt->numcollspheres = 0;
    }
    for (i = 0; i < 4; i++) {
        if (version > 10) {
            EdFileRead(&dt->sounds[i], 0xC);
        } else {
            dt->sounds[i].id = -1;
            dt->sounds[i].control = 0;
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
                    memcpy(edpp_ptls[i].name, debtab[edpp_ptls[i].type]->name, 0x10);
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
        effecttypes[i].physics = 0;
        effecttypes[i].key = -1;
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
    struct debtypedef_s discard;
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
        effecttypes[i].physics = 0;
        effecttypes[i].key = -1;
    }
    freeDmaDebType = 0;
    return 1;
}


void cbFileLoadEffects(struct edptlmenu_s *menu) {
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
    if (edpp_level_type == 1) {
        sprintf(filename, leveleffectsformat, edpp_level_path);
        if (NuFileSize(filename) > 0) {
            edppLoadEffects(filename, 1);
        }
        sprintf(filename, genericeffectsfile);
        if (NuFileSize(filename) > 0) {
            edppMergeEffects(filename, 0);
        }
        edppRestartAllEffectsInLevel();
        ShowMessageMenu(menu, loadedokmessage, 1);
    } else if (edpp_level_type == 2) {
        sprintf(filename, levelptlformat, edpp_level_path);
        if (NuFileSize(filename) > 0) {
            edppLoadEffects(filename, 1);
        }
        sprintf(filename, generalptlfile);
        if (NuFileSize(filename) > 0) {
            edppMergeEffects(filename, 0);
        }
        sprintf(filename, charptlformat, edpp_level_path);
        if (NuFileSize(filename) > 0) {
            edppMergeEffects(filename, 5);
        }
        edppRestartAllEffectsInLevel();
        ShowMessageMenu(menu, loadedokmessage, 1);
    } else {
        ShowMessageMenu(menu, loaderrormessage, 0);
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
        if (effecttypes[i].effectlist == list || list == 2) {
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
        if (effecttypes[i].effectlist == list || list == 2) {
            EdFileWrite(effecttypes[i].name, 0x10);
            EdFileWriteChar(effecttypes[i].effectlist);
            EdFileWriteShort(effecttypes[i].genrate);
            EdFileWriteShort(effecttypes[i].numptls);
            EdFileWriteShort(effecttypes[i].startoffset);
            EdFileWriteShort(effecttypes[i].ivalon);
            EdFileWriteShort(effecttypes[i].ivalonran);
            EdFileWriteShort(effecttypes[i].ivaloff);
            EdFileWriteShort(effecttypes[i].ivaloffran);
            EdFileWriteChar(effecttypes[i].gensort);
            EdFileWriteChar(effecttypes[i].gencode);
            EdFileWriteChar(effecttypes[i].texturetype);
            EdFileWriteChar(effecttypes[i].reserved23);
            EdFileWriteFloat(effecttypes[i].reserved24);
            EdFileWriteFloat(effecttypes[i].cuton);
            EdFileWriteFloat(effecttypes[i].cutoff);
            EdFileWriteFloat(effecttypes[i].drawcutoff);
            EdFileWriteFloat(effecttypes[i].emitvel);
            EdFileWrite(&effecttypes[i].varemit, 0xC);
            EdFileWrite(&effecttypes[i].varemitrand, 0xC);
            EdFileWrite(&effecttypes[i].varstart, 0xC);
            EdFileWrite(&effecttypes[i].varstartrand, 0xC);
            EdFileWriteFloat(effecttypes[i].posrange.x);
            EdFileWriteFloat(effecttypes[i].posrange.y);
            EdFileWriteFloat(effecttypes[i].posrange.z);
            EdFileWriteFloat(effecttypes[i].posbase.x);
            EdFileWriteFloat(effecttypes[i].posbase.y);
            EdFileWriteFloat(effecttypes[i].posbase.z);
            EdFileWriteFloat(effecttypes[i].momrange.x);
            EdFileWriteFloat(effecttypes[i].momrange.y);
            EdFileWriteFloat(effecttypes[i].momrange.z);
            EdFileWriteFloat(effecttypes[i].mombase.x);
            EdFileWriteFloat(effecttypes[i].mombase.y);
            EdFileWriteFloat(effecttypes[i].mombase.z);
            EdFileWriteFloat(effecttypes[i].gravity);
            EdFileWriteFloat(effecttypes[i].etime);
            EdFileWriteShort(effecttypes[i].frames);
            EdFileWriteChar(effecttypes[i].columns);
            EdFileWriteChar(effecttypes[i].rows);
            EdFileWriteFloat(effecttypes[i].frame);
            EdFileWriteFloat(effecttypes[i].jib[0]);
            EdFileWriteFloat(effecttypes[i].jib[1]);
            EdFileWriteFloat(effecttypes[i].jib[2]);
            EdFileWriteFloat(effecttypes[i].jib[3]);
            for (j = 0; j < 8; j++) {
                EdFileWrite(&effecttypes[i].colenv[j], 0x10);
            }
            for (j = 0; j < 8; j++) {
                EdFileWrite(&effecttypes[i].alphaenv[j], 8);
            }
            EdFileWriteFloat(effecttypes[i].sizemin);
            EdFileWriteFloat(effecttypes[i].sizemax);
            for (j = 0; j < 8; j++) {
                EdFileWrite(&effecttypes[i].widthenv[j], 8);
            }
            for (j = 0; j < 8; j++) {
                EdFileWrite(&effecttypes[i].heightenv[j], 8);
            }
            EdFileWriteFloat(effecttypes[i].rotmin);
            EdFileWriteFloat(effecttypes[i].rotmax);
            for (j = 0; j < 8; j++) {
                EdFileWrite(&effecttypes[i].rotenv[j], 8);
            }
            for (j = 0; j < 8; j++) {
                EdFileWrite(&effecttypes[i].extraenv0[j], 8);
            }
            for (j = 0; j < 8; j++) {
                EdFileWrite(&effecttypes[i].extraenv1[j], 8);
            }
            EdFileWriteFloat(effecttypes[i].texu0);
            EdFileWriteFloat(effecttypes[i].texu1);
            EdFileWriteFloat(effecttypes[i].texv0);
            EdFileWriteFloat(effecttypes[i].texv1);
            for (j = 0; j < 8; j++) {
                EdFileWrite(&effecttypes[i].collenv[j], 8);
            }
            EdFileWriteChar(effecttypes[i].numcollspheres);
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
                EdFileWrite(debtab[edpp_ptls[i].type]->name, 0x10);
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


void cbMemCardSaveEffects(struct edptlmenu_s *menu) {
    char filename[256];

    sceMcGetDir(0, 0, effectsfilter, 0, 10, mcdir);
    sceMcSync(0, 0, &memcardresult);
    if (memcardresult >= 8) {
        ShowMessageMenu(menu, toomanyeffectsmessage, 0);
    } else {
        sprintf(filename, effectsnameformat, savefilename);
        if (edppMemCardSaveEffects(filename, 2)) {
            ShowMessageMenu(menu, savedokmessage, 1);
        } else {
            ShowMessageMenu(menu, saveerrormessage, 0);
        }
    }
}


void cbMemCardSaveEffectTypes(struct edptlmenu_s *menu) {
    char filename[256];

    sceMcGetDir(0, 0, effecttypesfilter, 0, 10, mcdir);
    sceMcSync(0, 0, &memcardresult);
    if (memcardresult >= 8) {
        ShowMessageMenu(menu, toomanytypesmessage, 0);
    } else {
        sprintf(filename, effecttypesnameformat, savefilename);
        if (edppMemCardSaveEffects(filename, edpp_effect_list)) {
            ShowMessageMenu(menu, savedokmessage, 1);
        } else {
            ShowMessageMenu(menu, saveerrormessage, 0);
        }
    }
}


void MemCardLoadSingleEffectType(struct debtypedef_s *dt, int version, signed char list) {
    int i;

    EdFileRead(dt->name, 0x10);
    dt->effectlist = EdFileReadChar();
    if (list != 2) {
        dt->effectlist = list;
    }
    dt->genrate = EdFileReadShort();
    dt->numptls = EdFileReadShort();
    dt->startoffset = EdFileReadShort();
    dt->ivalon = EdFileReadShort();
    dt->ivalonran = EdFileReadShort();
    dt->ivaloff = EdFileReadShort();
    dt->ivaloffran = EdFileReadShort();
    dt->gensort = EdFileReadChar();
    dt->gencode = EdFileReadChar();
    dt->texturetype = EdFileReadChar();
    dt->reserved23 = EdFileReadChar();
    dt->reserved24 = EdFileReadFloat();
    dt->cuton = EdFileReadFloat();
    dt->cutoff = EdFileReadFloat();
    if (version < 10) {
        dt->drawcutoff = 0.0f;
    } else {
        dt->drawcutoff = EdFileReadFloat();
    }
    dt->emitvel = EdFileReadFloat();
    EdFileRead(&dt->varemit, 0xC);
    EdFileRead(&dt->varemitrand, 0xC);
    EdFileRead(&dt->varstart, 0xC);
    EdFileRead(&dt->varstartrand, 0xC);
    dt->posrange.x = EdFileReadFloat();
    dt->posrange.y = EdFileReadFloat();
    dt->posrange.z = EdFileReadFloat();
    dt->posbase.x = EdFileReadFloat();
    dt->posbase.y = EdFileReadFloat();
    dt->posbase.z = EdFileReadFloat();
    dt->momrange.x = EdFileReadFloat();
    dt->momrange.y = EdFileReadFloat();
    dt->momrange.z = EdFileReadFloat();
    dt->mombase.x = EdFileReadFloat();
    dt->mombase.y = EdFileReadFloat();
    dt->mombase.z = EdFileReadFloat();
    dt->gravity = EdFileReadFloat();
    dt->etime = EdFileReadFloat();
    dt->frames = EdFileReadShort();
    dt->columns = EdFileReadChar();
    dt->rows = EdFileReadChar();
    dt->frame = EdFileReadFloat();
    dt->jib[0] = EdFileReadFloat();
    dt->jib[1] = EdFileReadFloat();
    dt->jib[2] = EdFileReadFloat();
    dt->jib[3] = EdFileReadFloat();
    for (i = 0; i < 8; i++) {
        EdFileRead(&dt->colenv[i], 0x10);
    }
    for (i = 0; i < 8; i++) {
        EdFileRead(&dt->alphaenv[i], 8);
    }
    dt->sizemin = EdFileReadFloat();
    dt->sizemax = EdFileReadFloat();
    for (i = 0; i < 8; i++) {
        EdFileRead(&dt->widthenv[i], 8);
    }
    for (i = 0; i < 8; i++) {
        EdFileRead(&dt->heightenv[i], 8);
    }
    dt->rotmin = EdFileReadFloat();
    dt->rotmax = EdFileReadFloat();
    for (i = 0; i < 8; i++) {
        EdFileRead(&dt->rotenv[i], 8);
    }
    for (i = 0; i < 8; i++) {
        EdFileRead(&dt->extraenv0[i], 8);
    }
    for (i = 0; i < 8; i++) {
        EdFileRead(&dt->extraenv1[i], 8);
    }
    dt->texu0 = EdFileReadFloat();
    dt->texu1 = EdFileReadFloat();
    dt->texv0 = EdFileReadFloat();
    dt->texv1 = EdFileReadFloat();
    for (i = 0; i < 8; i++) {
        EdFileRead(&dt->collenv[i], 8);
    }
    dt->numcollspheres = EdFileReadChar();
    for (i = 0; i < 4; i++) {
        if (version > 10) {
            EdFileRead(&dt->sounds[i], 0xC);
        } else {
            dt->sounds[i].id = -1;
            dt->sounds[i].control = 0;
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
        effecttypes[i].physics = 0;
        effecttypes[i].key = -1;
    }
    freeDmaDebType = 0;
    edppDetermineNearest(1.0f);
    return 1;
}


void cbMemCardLoadEffects(struct edptlmenu_s *menu, struct edptlitem_s *item) {
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
    sprintf(filename, memcardfilenames[item->value]);
    if (edppMemCardLoadEffects(filename, 2)) {
        edppRestartAllEffectsInLevel();
        ShowMessageMenu(menu, loadedokmessage, 1);
    } else {
        ShowMessageMenu(menu, loaderrormessage, 0);
    }
}


void cbPtlDataMenu(struct edptlmenu_s *menu) {
    struct edptlfont_s font;

    font = edpp_menu_font;
    ptldatamenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, cbPtlCancelDataMenu,
                                 datamenutitle);
    if (ptldatamenu != 0) {
        eduiMenuAddItem(ptldatamenu, eduiItemSelCreate(1, &font, 0, 0, cbChangeNameMenu,
                                                       datamenutypename));
        if (edpp_create_type != -1) {
            eduiMenuAddItem(ptldatamenu, eduiItemSelCreate(1, &font, 0, 0, cbPtlDeleteEffect,
                                                           datamenudelete));
            eduiMenuAddItem(ptldatamenu, eduiItemSelCreate(1, &font, 0, 0, cbPtlCopyEffect,
                                                           datamenucopy));
        }
        if (edpp_effect_list == 0) {
            eduiMenuAddItem(ptldatamenu, eduiItemSelCreate(1, &font, 0, 0, cbFileSaveEffects,
                                                           datamenusavegeneral));
        } else if (edpp_effect_list == 1) {
            eduiMenuAddItem(ptldatamenu, eduiItemSelCreate(1, &font, 0, 0, cbFileSaveEffects,
                                                           datamenusavelevel));
        } else if (edpp_effect_list == 5) {
            eduiMenuAddItem(ptldatamenu, eduiItemSelCreate(1, &font, 0, 0, cbFileSaveEffects,
                                                           datamenusavechar));
        }
        eduiMenuAddItem(ptldatamenu, eduiItemSelCreate(1, &font, 0, 0, cbFileLoadEffects,
                                                       datamenuloadall));
        eduiMenuAddItem(ptldatamenu, eduiItemSelCreate(1, &font, 0, 0, cbMemCardMenu,
                                                       datamenumemcard));
        if (edpp_create_type != -1) {
            eduiMenuAddItem(ptldatamenu, eduiItemSelCreate(1, &font, 0, 0, edptlcbClipboardMenu,
                                                           datamenuclipboard));
        }
        eduiMenuAttach(menu, ptldatamenu);
        ptldatamenu->x = menu->x + 10;
        ptldatamenu->y = menu->y + 40;
    }
}


void cbPtlEmitMenu(struct edptlmenu_s *menu) {
    struct edptlfont_s font;
    struct debtypedef_s *dt;

    font = edpp_menu_font;
    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    dt = debtab[debkeydata[edpp_ptls[edpp_nearest].debris].debtype];
    ptlemitmenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0x12C, app_fnt, cbPtlCancelEmitMenu,
                                 emitmenutitle);
    if (ptlemitmenu != 0) {
        eduiMenuAddItem(ptlemitmenu, eduiItemSelCreate(1, &font, 0, 0, cbPtlEmitVelMenu,
                                                       emitmenuvel));
        eduiMenuAddItem(ptlemitmenu, eduiItemSelCreate(1, &font, 0, 0, cbPtlGravMenu,
                                                       emitmenugravity));
        if (dt->gensort == 0 || dt->gensort == 8 || dt->gensort == 9 || dt->gensort == 10) {
            eduiMenuAddItem(ptlemitmenu, eduiItemSelCreate(1, &font, 0, 0, cbPtlVarStartMenu,
                                                           emitmenurandomstart));
        } else if (dt->gensort == 6 || dt->gensort == 7) {
            eduiMenuAddItem(ptlemitmenu, eduiItemSelCreate(1, &font, 0, 0, cbPtlVarStartMenu,
                                                           emitmenubasevector));
        }
        eduiMenuAddItem(ptlemitmenu, eduiItemSelCreate(1, &font, 0, 0, cbPtlVarEmitMenu,
                                                       emitmenuvariation));
        eduiMenuAddItem(ptlemitmenu, eduiItemSelCreate(1, &font, 0, 0, cbChangeGenRateMenu,
                                                       emitmenugenrate));
        eduiMenuAddItem(ptlemitmenu, eduiItemSelCreate(1, &font, 0, 0, cbChangeETimeMenu,
                                                       emitmenulife));
        eduiMenuAddItem(ptlemitmenu, eduiItemSelCreate(1, &font, 0, 0, cbPtlEmitTimeMenu,
                                                       emitmenutiming));
        eduiMenuAddItem(ptlemitmenu, eduiItemSelCreate(1, &font, 0, 0, cbPtlCutOffMenu,
                                                       emitmenucutoff));
    }
    eduiMenuAttach(menu, ptlemitmenu);
    ptlemitmenu->x = menu->x + 10;
    ptlemitmenu->y = menu->y + 40;
}


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
        NuStrCpy(D_00484DE0, path);
    } else {
        D_00484DE0[0] = 0;
    }
    D_0062F358 = id;
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


int edppProc(struct pad_s *pad) {
    unsigned int mask;

    D_0062F318 += 5;
    if (edpp_active_menu != 0) {
        eduiMenuProcess(edpp_active_menu, pad);
        return 0;
    }
    edppDoInput(pad);
    mask = 0x800;
    return (pad->buttons & mask) != 0;
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


void cbPtlChangeGrav(void *menu, struct edptlitem_s *item) {
    struct debtypedef_s *dt;
    struct debtypedef_s *current;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    dt = debtab[debkeydata[edpp_ptls[edpp_nearest].debris].debtype];
    dt->gravity = item->fvalue;
    current = debtab[debkeydata[edpp_ptls[edpp_nearest].debris].debtype];
    current->physics->gravity = item->fvalue;
}


void edptlcbEmptyClipboard(struct edptlmenu_s *menu) {
    int saved;

    saved = edpp_create_type;
    edpp_create_type = edptl_clipboard_entry;
    cbPtlDeleteEffect(menu);
    edpp_create_type = saved;
    edptl_clipboard_entry = -1;
}


void edptlcbPasteClipboard(struct edptlmenu_s *menu) {
    struct debtypedef_s *dt;
    int arg;

    dt = debtab[edptl_clipboard_entry];
    dt->effectlist = edpp_effect_list;
    edptl_clipboard_entry = -1;
    arg = 0;
    if (menu->detacharg != 0) {
        arg = menu->detacharg;
        eduiMenuDetach(menu);
    }
    if (menu->callback != 0) {
        menu->callback(menu, arg);
    }
}


void edptlcbCutClipboard(struct edptlmenu_s *menu) {
    struct debtypedef_s *dt;
    int arg;

    dt = debtab[edpp_create_type];
    dt->effectlist = 4;
    edptl_clipboard_entry = edpp_create_type;
    arg = 0;
    if (menu->detacharg != 0) {
        arg = menu->detacharg;
        eduiMenuDetach(menu);
    }
    if (menu->callback != 0) {
        menu->callback(menu, arg);
    }
}


void edptlcbApplyBounceOffset(void *menu, struct edptlitem_s *item) {
    struct debkey_s *deb;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    edpp_ptls[edpp_nearest].bounceoffset = item->fvalue;
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    deb->bounceoffset = item->fvalue;
}


void edptlcbApplyBounceFactor(void *menu, struct edptlitem_s *item) {
    struct debkey_s *deb;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    edpp_ptls[edpp_nearest].bouncefactor = item->fvalue;
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    deb->bouncefactor = item->fvalue;
}


void cbPtlChangeEmitVel(void *menu, struct edptlitem_s *item) {
    struct debkey_s *deb;
    struct debtypedef_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->debtype];
    dt->emitvel = item->fvalue;
}


void cbPtlChangeX(void *menu, struct edptlitem_s *item) {
    struct debkey_s *deb;
    struct debtypedef_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->debtype];
    if (menu == 0) {
        return;
    }
    if (menu == ptlvaremitmenu) {
        dt->varemit.x = item->fvalue;
        dt->varemitrand.x = (item->fvalue * 2.0f) * (1.0 / 2147483648.0);
    } else if (menu == ptlvarstartmenu) {
        dt->varstart.x = item->fvalue;
        dt->varstartrand.x = (item->fvalue * 2.0f) * (1.0 / 2147483648.0);
    }
}


void cbPtlChangeCutOff(void *menu, struct edptlitem_s *item) {
    struct debkey_s *deb;
    struct debtypedef_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->debtype];
    dt->cutoff = item->fvalue;
    if (dt->cutoff < dt->cuton) {
        dt->cuton = dt->cutoff;
    }
}


void cbPtlChangeCutOn(void *menu, struct edptlitem_s *item) {
    struct debkey_s *deb;
    struct debtypedef_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->debtype];
    dt->cuton = item->fvalue;
    if (dt->cutoff < dt->cuton) {
        dt->cutoff = dt->cuton;
    }
}


void cbPtlChangeDrawCutOff(void *menu, struct edptlitem_s *item) {
    struct debkey_s *deb;
    struct debtypedef_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->debtype];
    dt->drawcutoff = item->fvalue;
}


void cbPtlChangeGenRate(void *menu, struct edptlitem_s *item) {
    struct debkey_s *deb;
    struct debtypedef_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->debtype];
    dt->genrate = item->fvalue;
    UpdateTotalPtls(dt);
}


void cbCancelChangeGenRateMenu(void) {
    eduiMenuDestroy(changegenratemenu);
    changegenratemenu = 0;
}


void cbPtlApplyJib(void *menu, struct edptlitem_s *item) {
    struct debkey_s *deb;
    struct debtypedef_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->debtype];
    if (ptljibitem0 != 0) {
        dt->jib[0] = ptljibitem0->fvalue;
    }
    if (ptljibitem1 != 0) {
        dt->jib[1] = ptljibitem1->fvalue;
    }
    if (ptljibitem2 != 0) {
        dt->jib[2] = ptljibitem2->fvalue;
    }
    if (ptljibitem3 != 0) {
        dt->jib[3] = ptljibitem3->fvalue;
    }
    GenericDebinfoDmaTypeUpdate(dt);
}


void cbPtlSelType(void *menu, struct edptlitem_s *item) {
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
    ptltypemenu = 0;
    edpp_create_type = item->value;
    edpp_active_menu = 0;
}


void cbPtlSelGCode(struct edptlmenu_s *menu, struct edptlitem_s *item) {
    struct debtypedef_s *dt;

    eduiMenuDetach(menu);
    edpp_active_menu = 0;
    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    dt = debtab[debkeydata[edpp_ptls[edpp_nearest].debris].debtype];
    dt->gencode = item->value - 1;
    debkeydata[edpp_ptls[edpp_nearest].debris].gencode = gencodetab[item->value - 1];
}


void cbCancelMessageMenu(void) {
    eduiMenuDestroy(messagemenu);
    messagemenu = 0;
}


void cbSelEffectList(struct edptlmenu_s *menu, struct edptlitem_s *item) {
    int arg;
    int i;

    arg = 0;
    if (item->value != edpp_effect_list) {
        edpp_effect_list = item->value;
        edpp_create_type = -1;
        for (i = 1; i < 128; i++) {
            if (debtab[i] != 0 && debtab[i]->effectlist == edpp_effect_list) {
                edpp_create_type = i;
                break;
            }
        }
    }
    if (menu->detacharg != 0) {
        arg = menu->detacharg;
        eduiMenuDetach(menu);
    }
    if (menu->callback != 0) {
        menu->callback(menu, arg);
    }
}


void cbCancelEffectListMenu(void) {
    eduiMenuDestroy(effectlistmenu);
    effectlistmenu = 0;
}


void edptlcbSetSwitchId(void *menu, struct edptlitem_s *item) {
    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    edpp_ptls[edpp_nearest].switchid = item->fvalue;
    debkeydata[edpp_ptls[edpp_nearest].debris].switchid = item->fvalue;
}


void edptlcbSetSwitchVar(void *menu, struct edptlitem_s *item) {
    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    edpp_ptls[edpp_nearest].switchvar = item->fvalue;
    debkeydata[edpp_ptls[edpp_nearest].debris].switchvar = item->fvalue;
}


void edptlcbCancelSwitchMenu(void) {
    eduiMenuDestroy(edptl_switch_menu);
    edptl_switch_menu = 0;
}


void edptlcbCancelGroupMenu(void) {
    eduiMenuDestroy(edptl_group_menu);
    edptl_group_menu = 0;
}


void edptlcbSetMasterGroup(void *menu, struct edptlitem_s *item) {
    debris_render_group = item->fvalue;
}


void edptlcbSetGroup(void *menu, struct edptlitem_s *item) {
    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    edpp_ptls[edpp_nearest].group = item->fvalue;
    debkeydata[edpp_ptls[edpp_nearest].debris].group = item->fvalue;
}


void edptlcbSetDpadMode(void *menu, struct edptlitem_s *item) {
    edpp_dpad_mode = item->value;
}


void edptlcbCancelDpadModeMenu(void) {
    eduiMenuDestroy(dpadmodemenu);
    dpadmodemenu = 0;
}


void edptlcbJumpToGameLocation(void *menu, struct edptlitem_s *item) {
    item->toggle = 0;
    edcamSetPosAng(&edpp_game_loc, 0, 0);
}


void cbPtlCancelTypeMenu(void) {
    eduiMenuDestroy(ptltypemenu);
    ptltypemenu = 0;
}


void cbPtlCancelColMenu(void) {
    eduiMenuDestroy(ptlcolmenu);
    ptlcolmenu = 0;
    colgradmenu = 0;
    alphagradmenu = 0;
}


void cbPtlCancelJibMenu(void) {
    eduiMenuDestroy(ptljibmenu);
    ptljibmenu = 0;
    D_0062F344 = 0;
    D_0062F348 = 0;
    D_0062F34C = 0;
    D_0062F350 = 0;
}


void edptlcbCancelBounceMenu(void) {
    eduiMenuDestroy(edptl_bounce_menu);
    edptl_bounce_menu = 0;
}


void cbChangeNumCollSpheres(void *menu, struct edptlitem_s *item) {
    struct debkey_s *deb;
    struct debtypedef_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->debtype];
    dt->numcollspheres = item->fvalue;
}


void cbPtlDefaultCollEnv(struct edptlmenu_s *menu) {
    struct debkey_s *deb;
    struct debtypedef_s *dt;
    int arg;
    int i;

    arg = 0;
    if (edpp_nearest != -1 && edpp_ptls[edpp_nearest].debris != -1) {
        deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
        dt = debtab[deb->debtype];
        for (i = 0; i < 8; i++) {
            dt->collenv[i].time = dt->widthenv[i].time;
            dt->collenv[i].value = dt->widthenv[i].value / 10000.0f;
        }
    }
    if (menu->detacharg != 0) {
        arg = menu->detacharg;
        eduiMenuDetach(menu);
    }
    if (menu->callback != 0) {
        menu->callback(menu, arg);
    }
}


void cbPtlApplyCollEnv(void) {
    struct edptlgradkey_s keys[8];
    struct debkey_s *deb;
    struct debtypedef_s *dt;
    float lo;
    float hi;
    int count;
    int i;

    if (collenvmenu == 0) {
        return;
    }
    count = eduiGradPickRead(collenvmenu, keys, 8);
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
    dt = debtab[deb->debtype];
    lo = dt->sizemin / 10000.0f;
    hi = dt->sizemax / 10000.0f;
    for (i = 0; i < count; i++) {
        dt->collenv[i].time = keys[i].time;
        dt->collenv[i].value = lo + keys[i].value[0] * (hi - lo);
    }
}


void edptlcbSetSoundID(void *menu, struct edptlitem_s *item) {
    struct debkey_s *deb;
    struct debtypedef_s *dt;
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
    dt = debtab[deb->debtype];
    dt->sounds[slot].id = id;
}


void edptlcbSetSoundControl(void *menu, struct edptlitem_s *item) {
    struct debkey_s *deb;
    struct debtypedef_s *dt;
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
    dt = debtab[deb->debtype];
    dt->sounds[slot].control = control;
}


void edptlcbChangeSoundDelay(void *menu, struct edptlitem_s *item) {
    struct debkey_s *deb;
    struct debtypedef_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->debtype];
    dt->sounds[item->value].delay = item->fvalue;
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
    collenvmenu = 0;
}


void cbPtlCancelSizeMenu(void) {
    eduiMenuDestroy(ptlsizemenu);
    ptlsizemenu = 0;
    widthgradmenu = 0;
    heightgradmenu = 0;
    sizeminitem = 0;
    sizemaxitem = 0;
}


void cbPtlCancelRotMenu(void) {
    eduiMenuDestroy(ptlrotmenu);
    ptlrotmenu = 0;
    rotgradmenu = 0;
    rotminitem = 0;
    rotmaxitem = 0;
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


void cbPtlSScaleMenu(struct edptlmenu_s *menu) {
    struct edptlfont_s font;

    font = edpp_menu_font;
    sscalemenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0x12C, app_fnt, cbPtlCancelSScaleMenu,
                                sscalemenutitle);
    if (sscalemenu != 0) {
        eduiMenuAddItem(sscalemenu, eduiItemSliderCreateInt(0, &font, 0, cbPtlChangeSScale, 1, 0x13,
                                                            edpp_slider_scale, sscalemenutitle));
        eduiMenuAttach(menu, sscalemenu);
        sscalemenu->x = menu->x + 10;
        sscalemenu->y = menu->y + 40;
    }
}


void cbPtlChangeTextureSelect(struct edptlmenu_s *menu, struct edptltexitem_s *item) {
    struct debkey_s *deb;
    struct debtypedef_s *dt;
    int arg;

    arg = 0;
    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->debtype];
    dt->texu0 = item->x0 * 256.0f + 524288.0f;
    dt->texu1 = item->x1 * 256.0f + 524288.0f;
    dt->texv0 = item->y0 * 256.0f + 524288.0f;
    dt->texv1 = item->y1 * 256.0f + 524288.0f;
    if (menu->detacharg != 0) {
        arg = menu->detacharg;
        eduiMenuDetach(menu);
    }
    if (menu->callback != 0) {
        menu->callback(menu, arg);
    }
    GenericDebinfoDmaTypeUpdate(dt);
}


void cbPtlSelTextureType(void *menu, struct edptlitem_s *item) {
    struct debkey_s *deb;
    struct debtypedef_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->debtype];
    dt->texturetype = item->value;
}


void cbPtlCancelTextureMenu(void) {
    if (textureselectmenu != 0) {
        eduiMenuDestroy(textureselectmenu);
        textureselectmenu = 0;
    }
    eduiMenuDestroy(texturemenu);
    texturemenu = 0;
}


void cbPtlChangeIvalOn(void *menu, struct edptlitem_s *item) {
    struct debkey_s *deb;
    struct debtypedef_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->debtype];
    dt->ivalon = item->fvalue;
    UpdateTotalPtls(dt);
}


void cbPtlChangeIvalOnRan(void *menu, struct edptlitem_s *item) {
    struct debkey_s *deb;
    struct debtypedef_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->debtype];
    dt->ivalonran = item->fvalue;
    UpdateTotalPtls(dt);
}


void cbPtlChangeIvalOff(void *menu, struct edptlitem_s *item) {
    struct debkey_s *deb;
    struct debtypedef_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->debtype];
    dt->ivaloff = item->fvalue;
    UpdateTotalPtls(dt);
}


void cbPtlChangeIvalOffRan(void *menu, struct edptlitem_s *item) {
    struct debkey_s *deb;
    struct debtypedef_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->debtype];
    dt->ivaloffran = item->fvalue;
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


static inline void CreateConfirmMenu(struct edptlmenu_s *parent, char *title, void *yescb) {
    struct edptlfont_s font;

    font = edpp_menu_font;
    confirmmenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, cbCancelConfirmMenu, title);
    if (confirmmenu != 0) {
        eduiMenuAddItem(confirmmenu, eduiItemSelCreate(1, &font, 0, 0, cbConfirmMenuNo,
                                                       confirmnolabel));
        eduiMenuAddItem(confirmmenu, eduiItemSelCreate(1, &font, 0, 0, yescb, confirmyeslabel));
        eduiMenuAttach(parent, confirmmenu);
        confirmmenu->x = parent->x + 10;
        confirmmenu->y = parent->y + 40;
    }
}


void cbMemCardEraseConfirm(struct edptlmenu_s *menu) {
    CreateConfirmMenu(menu, memcarderasetitle, cbMemCardErase);
}


void cbChangeFileNameMenu(struct edptlmenu_s *menu) {
    struct edptlfont_s font;

    font = edpp_menu_font;
    filenamemenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, cbCancelChangeFileNameMenu,
                                  filenamemenutitle);
    if (filenamemenu != 0) {
        eduiMenuAddItem(filenamemenu, eduiItemTextPickCreate(0, &font, cbChangeFileName,
                                                             filenamemenutitle));
        strcpy(edui_last_item->text, savefilename);
        eduiMenuAttach(menu, filenamemenu);
        filenamemenu->x = menu->x + 10;
        filenamemenu->y = menu->y + 40;
    }
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


void cbPtlSnapToggle(void *menu, struct edptlitem_s *item) {
    edpp_snap_enabled = item->toggle;
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
        edpp_game_loc = *edmainQueryLocVec();
    } else {
        edpp_game_loc.x = global_camera.mtx._30;
        edpp_game_loc.y = global_camera.mtx._31;
        edpp_game_loc.z = global_camera.mtx._32;
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


void cbChangeName(void *menu, struct edptlitem_s *item) {
    struct debtypedef_s *dt;

    if (edpp_create_type != -1) {
        dt = debtab[edpp_create_type];
        if (dt != 0) {
            strcpy(dt->name, item->text);
        }
    }
}


void cbCancelChangeNameMenu(void *menu) {
    struct debtypedef_s *dt;

    if (edpp_create_type != -1) {
        dt = debtab[edpp_create_type];
        if (dt != 0) {
            if (dt->name[0] == 0) {
                CreateMessageMenu(menu, nameduplicatemessage, 0);
                return;
            }
            eduiMenuDestroy(namemenu);
            namemenu = 0;
        }
    }
}


void cbPtlChangeETime(void *menu, struct edptlitem_s *item) {
    struct debkey_s *deb;
    struct debtypedef_s *dt;

    if (edpp_nearest == -1) {
        return;
    }
    if (edpp_ptls[edpp_nearest].debris == -1) {
        return;
    }
    deb = &debkeydata[edpp_ptls[edpp_nearest].debris];
    dt = debtab[deb->debtype];
    dt->etime = item->fvalue;
    UpdateTotalPtls(dt);
}


void cbCancelChangeETimeMenu(void) {
    eduiMenuDestroy(etimemenu);
    etimemenu = 0;
}


void cbConfirmMenuNo(struct edptlmenu_s *menu) {
    int arg;

    arg = 0;
    if (menu->detacharg != 0) {
        arg = menu->detacharg;
        eduiMenuDetach(menu);
    }
    if (menu->callback != 0) {
        menu->callback(menu, arg);
    }
}


void cbCancelConfirmMenu(void) {
    eduiMenuDestroy(confirmmenu);
    confirmmenu = 0;
}


void edptlcbSetSwitchType(void *menu, struct edptlitem_s *item) {
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
    debkeydata[edpp_ptls[edpp_nearest].debris].switchtype = item->value;
}


void edptlcbCancelSwitchTypeMenu(void) {
    eduiMenuDestroy(edptl_switchtype_menu);
    edptl_switchtype_menu = 0;
}


void edptlcbCancelSoundsMenu(void) {
    eduiMenuDestroy(edptl_sounds_menu);
    edptl_sounds_menu = 0;
}


void cbPtlChangeSScale(void *menu, struct edptlitem_s *item) {
    edpp_slider_scale = item->fvalue;
}


void cbPtlCancelSScaleMenu(void) {
    eduiMenuDestroy(sscalemenu);
    sscalemenu = 0;
}


static inline void CreateMessageMenu(struct edptlmenu_s *parent, char *message, int flags) {
    struct edptlfont_s font;

    font = edpp_message_font;
    if (flags == 1) {
        font.colour = 0x8000C000;
    }
    messagemenu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, cbCancelMessageMenu,
                                 messagemenutitle);
    if (messagemenu != 0) {
        eduiMenuAddItem(messagemenu, eduiItemSelCreate(1, &font, 0, 0, 0, message));
        eduiMenuAttach(parent, messagemenu);
        messagemenu->x = parent->x + 10;
        messagemenu->y = parent->y + 40;
    }
}


void cbMemCardDeleteEffects(struct edptlmenu_s *menu, struct edptlitem_s *item) {
    int arg;

    arg = 0;
    sceMcDelete(0, 0, memcardfilenames[item->value]);
    sceMcSync(0, 0, &memcardresult);
    if (menu->detacharg != 0) {
        arg = menu->detacharg;
        eduiMenuDetach(menu);
    }
    if (menu->callback != 0) {
        menu->callback(menu, arg);
    }
    if (memcardresult < 0) {
        CreateMessageMenu((struct edptlmenu_s *)arg, memcarddeleteerror, 0);
    } else {
        CreateMessageMenu((struct edptlmenu_s *)arg, memcarddeleteok, 1);
    }
}


void cbMemCardErase(struct edptlmenu_s *menu) {
    int arg;

    arg = 0;
    if (menu->detacharg != 0) {
        arg = menu->detacharg;
        eduiMenuDetach(menu);
    }
    if (menu->callback != 0) {
        menu->callback(menu, arg);
    }
    CreateMessageMenu((struct edptlmenu_s *)arg, memcarderasemessage, 0);
}


void cbChangeFileName(void *menu, struct edptlitem_s *item) {

    strcpy(savefilename, item->text);
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
