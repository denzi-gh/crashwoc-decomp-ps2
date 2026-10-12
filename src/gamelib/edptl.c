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
    char pad1C[0x14];
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

struct debkey_s {
    char pad000[0x482];
    short debtype;
    char pad484[0x24];
    void *gensort;
    void *gencode;
    char pad4B0[0x98];
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
    char pad14[0x4];
    short ivalon;
    short ivalonran;
    short ivaloff;
    short ivaloffran;
    signed char gensort;
    unsigned char gencode;
    unsigned char texturetype;
    char pad23[0x5];
    float cuton;
    float cutoff;
    float drawcutoff;
    float emitvel;
    struct nuvec_s varemit;
    struct nuvec_s varemitrand;
    struct nuvec_s varstart;
    struct nuvec_s varstartrand;
    char pad68[0x30];
    float gravity;
    float etime;
    char padA0[0x8];
    float jib[4];
    char padB8[0xC0];
    float collenvmin;
    float collenvmax;
    struct debenvkey_s defcollenv[8];
    char pad1C0[0x118];
    struct debphysics_s *physics;
    struct debenvkey_s collenv[8];
    unsigned char numcollspheres;
    char pad31D[0x7];
    struct debsound_s sounds[4];
};

struct edptlgradkey_s {
    float time;
    char pad04[0xC];
    float value[3];
    char pad1C[0x4];
};

struct edptlmenu_s {
    char pad00[0x10];
    int x;
    int y;
    char pad18[0xC];
    void (*callback)(struct edptlmenu_s *menu, int arg);
    char pad28[0x8];
    int detacharg;
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
    unsigned char unk_0x000[0x564];
    unsigned int buttons;
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
extern void *changegenratemenu;
extern struct edptlmenu_s *messagemenu;
extern struct edptlmenu_s *effectlistmenu;
extern void *edptl_switch_menu;
extern struct edptlmenu_s *edptl_group_menu;
extern struct edptlmenu_s *dpadmodemenu;
extern void *ptltypemenu;
extern struct edptlmenu_s *edptl_bounce_menu;
extern void *edptl_soundid_menu;
extern void *edptl_soundcontrol_menu;
extern void *edptl_soundx_menu;
extern void *collmenu;
extern void *ptlcutoffmenu;
extern void *ptlvaremitmenu;
extern void *ptlvarstartmenu;
extern void *emittimemenu;
extern void *ptlemitvelmenu;
extern void *ptlgravmenu;
extern void *memcardmenu;
extern void *ptlclipmenu;
extern void *ptldatamenu;
extern void *ptlemitmenu;
extern void *ptlgsortmenu;
extern void *ptloptmenu;
extern void *ptlgcodemenu;
extern void *etimemenu;
extern void *confirmmenu;
extern void *edptl_switchtype_menu;
extern void *edptl_sounds_menu;
extern struct edptlmenu_s *sscalemenu;
extern struct edptlmenu_s *filenamemenu;
extern void *loadeffectsmenu;
extern void *mergeeffectsmenu;
extern void *deleteeffectsmenu;
extern void *ptlcolmenu;
extern void *ptlrotmenu;
extern void *D_0062F354;
extern int D_0062F314;
extern int edptl_clipboard_entry;
extern int edpp_create_type;
extern int D_0062F320;
extern int D_0062F324;
extern int D_0062F338;
extern int D_0062F33C;
extern int D_0062F340;

#define edpp_slider_scale D_0062F314
#define collenvmenu D_0062F354

extern void edmainRegisterLocVec(struct nuvec_s *loc);
extern void eduiMenuDetach(void *menu);
extern void eduiMenuDestroy(void *menu);
extern char *strcpy(char *dst, const char *src);
extern void edcamSetPosAng(void *pos, void *ang, int flags);
extern void cbPtlDeleteEffect(void);

extern int sprintf(char *buf, const char *fmt, ...);

extern char D_0062F368[];
#define copynameformat D_0062F368

extern char D_00484DC0[];
#define savefilename D_00484DC0

extern struct nuvec_s D_006B3B30;
#define edpp_game_loc D_006B3B30

extern void *ptljibmenu;
extern void *ptlsizemenu;
extern void *texturemenu;
extern void *textureselectmenu;
extern int D_0062F328;
extern int D_0062F32C;
extern int D_0062F330;
extern int D_0062F334;
extern struct edptlitem_s *D_0062F344;
extern struct edptlitem_s *D_0062F348;
extern struct edptlitem_s *D_0062F34C;
extern struct edptlitem_s *D_0062F350;
extern unsigned char D_0062F358;
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
extern void edppPtlCreate(struct nuvec_s *pos, int type);
extern void edppRestartAllEffectsInLevel(void);
extern void edcamSet(void);
extern void edppDrawCursor(void);
extern void eduiMenuRender(void *menu);
extern void GenericDebinfoDmaTypeUpdate(struct debtypedef_s *dt);
static void CreateMessageMenu(struct edptlmenu_s *parent, char *message, int flags);

extern short edpp_rotz;
extern short edpp_roty;
extern short edpp_emitrotz;
extern short edpp_emitroty;
extern short edpp_refrotz;
extern short edpp_refroty;
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
extern void DebrisOrientation(int key, int rotz, int roty);
extern void DebrisEmitterOrientation(int key, int rotz, int roty);
extern void DebrisReflectionOrientation(int key, float offset, float factor, int rotz, int roty);
extern struct nuvec_s *edmainQueryLocVec(void);
extern void *memset(void *dst, int value, unsigned int size);
extern struct edptlmenu_s *eduiMenuCreate(int x, int y, int w, int h, void *font, void *cancelcb,
                                          char *title);
extern void eduiMenuAddItem(struct edptlmenu_s *menu, void *item);
extern int eduiMenuAttach(struct edptlmenu_s *parent, struct edptlmenu_s *child);
extern void *eduiItemSelCreate(int type, struct edptlfont_s *font, int value, int group, void *cb,
                               char *label);
extern void *eduiItemSliderCreateInt(void *data, struct edptlfont_s *font, int group, void *cb,
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


void edptlcbEmptyClipboard(void) {
    int saved;

    saved = edpp_create_type;
    edpp_create_type = edptl_clipboard_entry;
    cbPtlDeleteEffect();
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
    D_0062F320 = 0;
    D_0062F324 = 0;
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
            dt->collenv[i].time = dt->defcollenv[i].time;
            dt->collenv[i].value = dt->defcollenv[i].value / 10000.0f;
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
    lo = dt->collenvmin / 10000.0f;
    hi = dt->collenvmax / 10000.0f;
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
    D_0062F328 = 0;
    D_0062F32C = 0;
    D_0062F330 = 0;
    D_0062F334 = 0;
}


void cbPtlCancelRotMenu(void) {
    eduiMenuDestroy(ptlrotmenu);
    ptlrotmenu = 0;
    D_0062F338 = 0;
    D_0062F33C = 0;
    D_0062F340 = 0;
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
