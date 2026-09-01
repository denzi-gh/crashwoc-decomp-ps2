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
    char pad11[0x3B];
    float fvalue;
};

struct edppptl_s {
    char pad00[0x10];
    int debris;
    char pad14[0x2C];
    float bounceoffset;
    char pad44[0x8];
};

struct debkey_s {
    char pad000[0x558];
    float bounceoffset;
    char pad55C[0x10];
};

struct nuvec_s;

extern struct edppptl_s edpp_ptls[256];

extern int edpp_nextalloc;
extern int edpp_nearest;
extern int debris_render_group;
extern int effect_types_used;

extern void *debtab[128];
extern struct debkey_s debkeydata[];

extern int edpp_dpad_mode;
extern int edpp_snap_enabled;
extern void *edpp_active_menu;
extern int D_0062F314;

#define edpp_slider_scale D_0062F314

extern void edmainRegisterLocVec(struct nuvec_s *loc);


void ParticleReset(void) {
    int i;

    for (i = 0; i < 256; i++) {
        edpp_ptls[i].debris = -1;
    }
    edpp_nextalloc = 0;
}


void edppDestroyAllEffects(void) {
    int i;

    for (i = 1; i < 128; i++) {
        debtab[i] = 0;
    }
    effect_types_used = 1;
}


void edppRegisterPointerToGameCharLocation(struct nuvec_s *loc) {
    edmainRegisterLocVec(loc);
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


void edptlcbSetMasterGroup(void *menu, struct edptlitem_s *item) {
    debris_render_group = item->fvalue;
}


void edptlcbSetDpadMode(void *menu, struct edptlitem_s *item) {
    edpp_dpad_mode = item->value;
}


void cbPtlSnapToggle(void *menu, struct edptlitem_s *item) {
    edpp_snap_enabled = item->toggle;
}


void cbPtlCancel(void) {
    edpp_active_menu = 0;
}


void edppApply(void) {
}


void cbPtlChangeSScale(void *menu, struct edptlitem_s *item) {
    edpp_slider_scale = item->fvalue;
}
