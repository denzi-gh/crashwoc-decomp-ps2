/*
 * Unit: gamelib/edanim
 *
 * Functions:
 *   0x001a1198 edanimUpdateObjects
 *   0x001a1ce8 edanimDetermineNearestAnim
 *   0x001a1e90 edanimDetermineNearestParticle
 *   0x001a2088 edanimDetermineNearestSound
 *   0x001a2280 edanimParamCreate
 *   0x001a2398 edanimSoundCreate
 *   0x001a24c8 edanimSoundDestroy
 *   0x001a2610 edanimParticleCreate
 *   0x001a2750 edanimParticleDestroy
 *   0x001a28a0 edanimDoInput
 *   0x001a2fa0 edanimFileSave
 *   0x001a32a8 edanimFileLoad
 *   0x001a3810 edanimDrawCursor
 *   0x001a3c90 edanimcbSwitchTypeMenu
 *   0x001a41b0 edanimcbSwitchMenu
 *   0x001a4398 edanimcbBouncyMenu
 *   0x001a4548 edanimcbParticleTypeMenu
 *   0x001a46c8 edanimcbLocalParticleMenu
 *   0x001a4858 edanimcbLocalSoundMenu
 *   0x001a4ab0 edanimcbMCTBMenu
 *   0x001a4dd8 edanimInit
 *   0x001a50c8 edanimRenderParticleEmitters
 *   0x001a5260 edanimRenderSoundEmitters
 *   0x001a5390 edanimRender
 *   0x001a5490 edanimLookupInstanceIndex
 *   0x001a54f0 edanimRegisterBaseScene
 *   0x001a54f8 edanimLookupSpecial
 *   0x001a5580 edanimParamReset
 *   0x001a55b0 edanimPlayerAnimDistance
 *   0x001a5628 edanimParamDestroy
 *   0x001a5658 edanimSoundPlace
 *   0x001a56b0 edanimParticlePlace
 *   0x001a5750 edanimcbSetSwitchType
 *   0x001a57b0 edanimcbSetSwitchId
 *   0x001a57f0 edanimcbSetSwitchDelay
 *   0x001a5828 edanimcbSetSwitchVar
 *   0x001a5860 edanimcbCancelSwitchTypeMenu
 *   0x001a5888 edanimcbCancelSwitchMenu
 *   0x001a58b0 edanimcbSetBouncyPlayerGrav
 *   0x001a5910 edanimcbSetBouncyTension
 *   0x001a5970 edanimcbSetBouncyDamping
 *   0x001a59d0 edanimcbCancelBouncyMenu
 *   0x001a59f8 edanimcbToggleSoundType
 *   0x001a5ab0 edanimcbSetSoundTiming
 *   0x001a5ae0 edanimcbToggleParticleSwitch
 *   0x001a5b20 edanimcbSetParticleType
 *   0x001a5b70 edanimcbSetParticleRate
 *   0x001a5ba8 edanimcbCancelParticleTypeMenu
 *   0x001a5bd0 edanimcbLocalParticleTypeMenu
 *   0x001a5d48 edanimcbCancelLocalParticleMenu
 *   0x001a5d70 edanimcbParticleMenu
 *   0x001a5e90 edanimcbLocalSoundTypeMenu
 *   0x001a6008 edanimcbCancelLocalSoundMenu
 *   0x001a6030 edanimcbSoundMenu
 *   0x001a6150 edanimcbCancelOptMenu
 *   0x001a6158 edanimcbFileSave
 *   0x001a6230 edanimcbFileLoad
 *   0x001a62d8 edanimcbMCTBCardPresent
 *   0x001a6330 edanimcbMCTBCardType
 *   0x001a6388 edanimcbMCTBCardCheckFormat
 *   0x001a63e0 edanimcbMCTBCardFreeSpace
 *   0x001a6430 edanimcbMCTBCardSlotsUsed
 *   0x001a6478 edanimcbMCTBCardSaveSlot
 *   0x001a6500 edanimcbMCTBCardLoadSlot
 *   0x001a6560 edanimcbMCTBCardDeleteSlot
 *   0x001a65c0 edanimcbMCTBCardUnFormat
 *   0x001a6618 edanimcbMCTBCardFormat
 *   0x001a6670 edanimcbCancelMCTBMenu
 *   0x001a6698 edanimClose
 *   0x001a66b8 edanimEnter
 *   0x001a66d8 edanimProc
 *   0x001a6790 edanimcbSetLocalSoundType
 *   0x001a6828 edanimcbSetLocalParticleType
 *   0x001a68c0 edanimcbCancelLocalParticleTypeMenu
 *   0x001a68e8 edanimcbCancelParticleMenu
 *   0x001a6910 edanimcbSoundTypeMenu
 *   0x001a6a90 edanimcbCancelLocalSoundTypeMenu
 *   0x001a6ab8 edanimcbCancelSoundMenu
 *   0x001a6ae0 edanimcbSetSoundType
 *   0x001a6b40 edanimcbCancelSoundTypeMenu
 */

struct nuvec_s {
    float x;
    float y;
    float z;
};

struct numtx_s {
    float _00, _01, _02, _03;
    float _10, _11, _12, _13;
    float _20, _21, _22, _23;
    struct nuvec_s pos;
    float _33;
};

struct nuinstanim_s {
    struct numtx_s mtx;
    float tfactor;
    float tfirst;
    float tinterval;
    float ltime;
    unsigned int playing : 1;
    unsigned int backwards : 1;
    unsigned int waiting : 1;
    unsigned int repeating : 1;
    unsigned int oscillate : 1;
    int ipad[2];
    unsigned char anim_ix;
};

struct nuanimdata_s {
    float time;
};

struct nuinstance_s {
    struct numtx_s mtx;
    int objid;
    int flags;
    struct nuinstanim_s *anim;
    char pad4c[0x50 - 0x4C];
};

struct nuspecial_s {
    struct numtx_s mtx;
    struct nuinstance_s *instance;
    char *name;
    char pad48[0x50 - 0x48];
};

struct nugscn_s {
    char pad00[0x18];
    int numinstance;
    struct nuinstance_s *instances;
    int numspecial;
    struct nuspecial_s *specials;
    char pad28[0x48 - 0x28];
    struct nuanimdata_s **instanimdata;
};

struct edanimitem_s {
    char pad00[0x0C];
    int value;
    unsigned char toggle;
    char pad11[0x4C - 0x11];
    float fvalue;
};

struct nurndrvtx_s {
    struct nuvec_s pnt;
    char pad0c[0x18 - 0x0C];
    int colour;
    char pad1c[0x24 - 0x1C];
};

struct eduimenu_s {
    struct edanimitem_s *head;
    struct edanimitem_s *tail;
    struct edanimitem_s *current;
    struct edanimitem_s *cursor;
    int x;
    int y;
    int w;
    int h;
};

struct eduistyle_s {
    unsigned int colour[4];
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

struct edanimparam_s {
    int id;
    int particle_count;
    int sound_count;
    int switch_type;
    int switch_id;
    float switch_var;
    float switch_delay;
    char particle_name[8][16];
    int particle_type[8];
    int particle_rate[8];
    int particle_switch[8];
    struct nuvec_s particle_pos[8];
    short particle_rotz[8];
    short particle_roty[8];
    float sound_last_time;
    char sound_name[8][16];
    int sound_type[8];
    int sound_switch[8];
    float sound_timing[8];
    struct nuvec_s sound_pos[8];
    int bouncy_instance;
    float bouncy_player_grav;
    float bouncy_tension;
    float bouncy_damping;
};

extern struct edanimparam_s AnimParams[];

extern struct nuvec_s edanim_cam_pos;
extern struct nugscn_s *edanim_base_scene;
extern void *edanim_active_menu;
extern struct eduimenu_s *edanim_bouncy_menu;
extern struct eduimenu_s *edanim_localparticle_menu;
extern void *edanim_localparticletype_menu;
extern void *edanim_localsound_menu;
extern void *edanim_localsoundtype_menu;
extern void *edanim_mctb_menu;
extern void *edanim_options_menu;
extern struct eduimenu_s *edanim_particle_menu;
extern struct eduimenu_s *edanim_particletype_menu;
extern struct eduimenu_s *edanim_sound_menu;
extern struct eduimenu_s *edanim_switch_menu;
extern struct eduimenu_s *edanim_switchtype_menu;
extern int edanim_nearest;
extern int edanim_nearest_param_id;
extern int edanim_nearest_particle;
extern int edanim_nearest_sound;
extern int edanim_next_param;
extern int edanim_particle_mode;
extern int edanim_particle_type;
extern int edanim_sound_type;
extern int edanim_sound_mode;
extern int edanim_emitrotz;
extern int edanim_emitroty;
extern int edanim_cam_ax;
extern int edanim_cam_ay;
extern void *edanim_mtl;
extern void *edanim_mtl_zoff;
extern void *edanim_soundtype_menu;
extern char *debtab[];

extern void *app_fnt;
extern struct eduistyle_s D_00619328;
extern char D_00619520[];
extern char D_00619538[];
extern char D_00619550[];
extern char D_00619620[];
extern char D_00619630[];
extern char D_00619640[];

#define edanim_item_style D_00619328
#define particle_menu_title D_00619520
#define particle_menu_text_type D_00619538
#define particle_menu_text_local D_00619550
#define sound_menu_title D_00619620
#define sound_menu_text_type D_00619630
#define sound_menu_text_local D_00619640

extern unsigned char object_switches[];
extern struct numtx_s numtx_identity;
extern int D_0062F69C;

#define edanim_frame_count D_0062F69C

extern char D_00619278[];
extern char D_00619288[];
extern char D_006192A0[];
extern char D_006192B0[];
extern char D_006192C0[];
extern char D_006192D0[];
extern char D_006192E8[];
extern char D_00619300[];
extern char D_00619310[];
extern char D_0062F6A0[];

#define cursor_text_info_box D_00619278
#define cursor_text_no_special D_00619288
#define cursor_text_special D_006192A0
#define cursor_text_no_params D_006192B0
#define cursor_text_params D_006192C0
#define cursor_text_particles D_006192D0
#define cursor_text_no_type D_006192E8
#define cursor_text_type D_00619300
#define cursor_text_sounds D_00619310
#define cursor_text_coord D_0062F6A0

extern char D_00619338[];
extern char D_0062F6A8[];
extern char D_0062F6B0[];
extern char D_00619348[];
extern char D_00619360[];
extern char D_00619378[];
extern char D_00619388[];
extern char D_006193A0[];
extern char D_0062F6B8[];
extern char D_006193B8[];
extern char D_006193D0[];
extern char D_006193E8[];
extern char D_006193F8[];
extern char D_00619408[];
extern char D_00619420[];
extern char D_00619430[];
extern char D_00619440[];
extern char D_00619450[];
extern char D_00619460[];

#define switchtype_menu_title D_00619338
#define switchtype_text_none D_0062F6A8
#define switchtype_text_switch D_0062F6B0
#define switchtype_text_switch_once D_00619348
#define switchtype_text_switch_cont D_00619360
#define switchtype_text_proximity D_00619378
#define switchtype_text_proximity_once D_00619388
#define switchtype_text_proximity_cont D_006193A0
#define switchtype_text_terrain D_0062F6B8
#define switchtype_text_terrain_once D_006193B8
#define switchtype_text_terrain_cont D_006193D0
#define switchtype_text_override_noanim D_006193E8
#define switchtype_text_override_play D_006193F8
#define switchtype_text_override_playcont D_00619408
#define switch_menu_title D_00619420
#define switch_menu_text_type D_00619430
#define switch_menu_text_id D_00619440
#define switch_menu_text_delay D_00619450
#define switch_menu_text_var D_00619460

extern char D_00619470[];
extern char D_00619480[];
extern char D_0062F6C0[];
extern char D_0062F6C8[];
extern char D_00619490[];
extern char D_0062F6D0[];
extern char D_006194C0[];
extern char D_006194E0[];
extern char D_00619500[];
extern char D_00619510[];

#define bouncy_menu_title D_00619470
#define bouncy_menu_text_grav D_00619480
#define bouncy_menu_text_tension D_0062F6C0
#define bouncy_menu_text_damping D_0062F6C8
#define particletype_menu_title D_00619490
#define particletype_text_none D_0062F6D0
#define localparticle_menu_title D_006194C0
#define localparticle_menu_text_type D_006194E0
#define localparticle_menu_text_rate D_00619500
#define localparticle_menu_text_moving D_00619510

extern char D_0062F6E0[];
extern char D_006196B0[];
extern char D_006196C0[];
extern char D_006196D0[];
extern char D_006196E0[];
extern char D_006196F0[];
extern char D_00619700[];

extern signed char edbits_what_game;
extern char edbits_level_filename[];
extern char D_00619660[];
extern char D_0062F6D8[];
extern char D_00619670[];
extern char D_00619680[];
extern char D_00619690[];
extern char D_006196A0[];
extern char D_00619728[];
extern char D_00619738[];

#define anim_file_format_game1 D_00619660
#define anim_file_format_game2 D_0062F6D8
#define anim_text_saved D_00619670
#define anim_text_save_failed D_00619680
#define anim_text_loaded D_00619690
#define anim_text_load_failed D_006196A0
#define mctb_text_slot_name D_00619728
#define mctb_text_save_failed D_00619738

#define mctb_text_present D_0062F6E0
#define mctb_text_not_present D_006196B0
#define mctb_text_ps2_card D_006196C0
#define mctb_text_not_ps2_card D_006196D0
#define mctb_text_formatted D_006196E0
#define mctb_text_not_formatted D_006196F0
#define mctb_text_free_space D_00619700

extern char D_00619710[];
extern char D_00619748[];
extern char D_00619758[];
extern char D_00619768[];
extern char D_00619778[];
extern char D_00619788[];
extern char D_00619798[];
extern char D_006197A8[];

#define mctb_text_slots_used D_00619710
#define mctb_text_load_failed D_00619748
#define mctb_text_deleted D_00619758
#define mctb_text_delete_failed D_00619768
#define mctb_text_unformatted D_00619778
#define mctb_text_unformat_failed D_00619788
#define mctb_text_formatted_ok D_00619798
#define mctb_text_format_failed D_006197A8

extern void eduiMenuDestroy(void *menu);
extern void eduiMenuDetach(void *menu);
extern void eduiCreateMessageMenu(void *menu, char *text, int flag);
extern struct nuvec_s *edmainQueryLocVec(void);
extern float NuVecDist(struct nuvec_s *a, struct nuvec_s *b, void *c);
extern void NuVecAdd(struct nuvec_s *dst, struct nuvec_s *a, struct nuvec_s *b);
extern void NuVecSub(struct nuvec_s *dst, struct nuvec_s *a, struct nuvec_s *b);
extern void PlatInstBounce(int instance, float player_grav, float tension, float damping);
extern int saveloadCheckCardPresent(void);
extern int saveloadCheckCardType(void);
extern int saveloadCheckCardFormatted(void);
extern int saveloadCheckCardFreeSpace(int port);
extern int sprintf(char *buf, const char *fmt, ...);
extern int saveloadCheckSlotsUsed(void);
extern int saveloadSaveSlot(int slot, char *text, int size);
extern int saveloadLoadSlot(int slot, char *text, int size);
extern int saveloadDeleteSlot(int slot);
extern int saveloadUnFormatCard(void);
extern int saveloadFormatCard(void);
extern int strncmp(const char *a, const char *b, int n);
extern char *strncpy(char *dst, const char *src, unsigned int n);
extern void EdFileSetMedia(int media);
extern int EdFileOpen(char *name, int mode);
extern void EdFileClose(void);
extern void EdFileWrite(void *data, int size);
extern void EdFileWriteInt(int data);
extern void EdFileWriteFloat(float data);
extern void EdFileWriteShort(short data);
extern void EdFileRead(void *data, int size);
extern int EdFileReadInt(void);
extern float EdFileReadFloat(void);
extern short EdFileReadShort(void);
extern char *strcpy(char *dst, const char *src);
extern char *edbitsGetSoundName(int type);
extern void edbitsDrawCube(float x, float y, float z, float sizex, float sizey, float sizez,
                           int rotx, int roty, int rotz, int flags, int colour, void *mtl);
extern void edcamSet(void);
extern int NuRndrLine3d(struct nurndrvtx_s *line, void *mtl, void *mtx);
extern void NuRndrRect2di(int x, int y, int w, int h, int colour, void *mtl);
extern void NuRndrLine2di(int x0, int y0, int x1, int y1, int colour, void *mtl);
extern void NuFntSet(void *fnt);
extern void NuFntSetPen(int colour);
extern void NuFntPrintEx(int x, int y, int flags, char *fmt, ...);
extern void NuVecRotateY(struct nuvec_s *dst, struct nuvec_s *src, int angle);
extern void NuVecRotateZ(struct nuvec_s *dst, struct nuvec_s *src, int angle);
extern void edcamMove(struct pad_s *pad);
extern void edcamSetPos(struct nuvec_s *pos);
extern void edcamGetPosAng(struct nuvec_s *pos, int *ax, int *ay);
extern void edanimParticleCreate(struct nuvec_s *wpos);
extern void edanimSoundCreate(struct nuvec_s *wpos);
extern int edanimParamCreate(int special);
extern void NuFntScale(int x, int y);
extern void eduiMenuRender(void *menu);
extern void eduiMenuProcess(void *menu, struct pad_s *pad);
extern int edanimFileSave(char *filename);
extern int edanimFileLoad(char *filename);
extern int NuFileSize(char *name);
extern struct eduimenu_s *eduiMenuCreate(int x, int y, int w, int h, void *font,
                                         void *cancel, char *title);
extern struct edanimitem_s *eduiItemSelCreate(int value, struct eduistyle_s *style, int on,
                                              int group, void *select, char *text);
extern struct edanimitem_s *eduiMenuAddItem(struct eduimenu_s *menu, struct edanimitem_s *item);
extern int eduiMenuAttach(void *menu, struct eduimenu_s *child);
extern struct edanimitem_s *eduiItemCheckCreate(int value, struct eduistyle_s *style, int on,
                                                int group, void *select, char *text);
extern struct edanimitem_s *eduiItemSliderCreate(int value, struct eduistyle_s *style, int group,
                                                 void *change, float min, float max, float val,
                                                 char *text);
extern struct edanimitem_s *eduiItemSliderCreateInt(int value, struct eduistyle_s *style,
                                                    int group, void *change, int min, int max,
                                                    int val, char *text);
extern struct edanimitem_s *edui_last_item;
extern void edanimcbSetSwitchType(void *menu, struct edanimitem_s *item);
extern void edanimcbSetSwitchId(void *menu, struct edanimitem_s *item);
extern void edanimcbSetSwitchDelay(void *menu, struct edanimitem_s *item);
extern void edanimcbSetSwitchVar(void *menu, struct edanimitem_s *item);
extern void edanimcbCancelSwitchTypeMenu(void);
extern void edanimcbCancelSwitchMenu(void);
extern struct edanimitem_s *eduiItemToggleCreate(int value, struct eduistyle_s *style, int on,
                                                 int group, void *toggle, char *text);
extern void edanimcbSetBouncyPlayerGrav(void *menu, struct edanimitem_s *item);
extern void edanimcbSetBouncyTension(void *menu, struct edanimitem_s *item);
extern void edanimcbSetBouncyDamping(void *menu, struct edanimitem_s *item);
extern void edanimcbCancelBouncyMenu(void);
extern void edanimcbSetParticleType(void *menu, struct edanimitem_s *item);
extern void edanimcbCancelParticleTypeMenu(void);
extern void edanimcbLocalParticleTypeMenu(struct eduimenu_s *menu);
extern void edanimcbSetParticleRate(void *menu, struct edanimitem_s *item);
extern void edanimcbToggleParticleSwitch(void *menu, struct edanimitem_s *item);
extern void edanimcbCancelLocalParticleMenu(void);
extern int edanimLookupInstanceIndex(int special);
extern int edanimLookupSpecial(char *name);
extern int FindPlatInst(int instance);
extern void edanimcbParticleTypeMenu(struct eduimenu_s *menu);
extern void edanimcbLocalParticleMenu(struct eduimenu_s *menu);
extern void edanimcbSoundTypeMenu(struct eduimenu_s *menu);
extern void edanimcbLocalSoundMenu(struct eduimenu_s *menu);
extern void edanimcbCancelParticleMenu(void);
extern void edanimcbCancelSoundMenu(void);
extern void NuMtxInvRSS(struct numtx_s *dst, struct numtx_s *src);
extern void NuMtxMul(struct numtx_s *dst, struct numtx_s *a, struct numtx_s *b);
extern void NuMtxRotateZ(struct numtx_s *m, int angle);
extern void NuMtxRotateY(struct numtx_s *m, int angle);
extern void NuVecMtxTransform(struct nuvec_s *dst, struct nuvec_s *src, struct numtx_s *m);
extern void AddVariableShotDebrisEffectMtx2(int effect, struct nuvec_s *pos, int count,
                                            struct numtx_s *mtx, struct numtx_s *orient);
extern int LookupDebrisEffect(char *name);
extern void edanimParticleDestroy(int param, int slot);
extern void edbitsSoundPlay(struct nuvec_s *pos, int id);
extern int edbitsLookupSoundFX(char *name);
extern void edanimSoundDestroy(int param, int slot);
extern void edanimDrawCursor(void);
extern void edanimRenderParticleEmitters(int param);
extern void edanimRenderSoundEmitters(int param);
extern void edanimDoInput(struct pad_s *pad);
extern void edanimDetermineNearestAnim(float range);
extern void edanimDetermineNearestParticle(float range);
extern void edanimDetermineNearestSound(float range);


static inline float edanimParamDistance(int param)
{
    if (edmainQueryLocVec() != 0) {
        return NuVecDist(&edanim_base_scene->specials[AnimParams[param].id].mtx.pos,
                         edmainQueryLocVec(), 0);
    }
    return 0.0f;
}


void edanimUpdateObjects(float dt)
{
    int i;
    int j;
    int k;
    struct nuinstance_s *instance;
    struct nuinstanim_s *instanim;
    struct nuvec_s emit;
    struct numtx_s tmtx;
    struct numtx_s orient;
    int sid;

    if (edanim_base_scene == 0) {
        return;
    }
    for (i = 0; i < 64; i++) {
        if (AnimParams[i].id == -1) {
            continue;
        }
        instance = edanim_base_scene->specials[AnimParams[i].id].instance;
        instanim = instance->anim;
        if (instanim != 0) {
            switch (AnimParams[i].switch_type) {
            case 0:
                break;
            case 1:
                instanim->playing = 0;
                if (AnimParams[i].switch_id != -1 && object_switches[AnimParams[i].switch_id] != 0) {
                    instanim->playing = 1;
                }
                break;
            case 2:
                instanim->repeating = 0;
                if (AnimParams[i].switch_id != -1 && object_switches[AnimParams[i].switch_id] != 0
                    && !instanim->playing) {
                    instanim->ltime = 1.0f;
                    instanim->playing = 1;
                    instanim->waiting = 0;
                    instanim->backwards = 0;
                }
                break;
            case 3:
                instanim->repeating = 1;
                if (AnimParams[i].switch_id != -1 && object_switches[AnimParams[i].switch_id] != 0) {
                    instanim->playing = 1;
                }
                break;
            case 4:
                if (edanimParamDistance(i) < AnimParams[i].switch_var) {
                    instanim->playing = 1;
                } else {
                    instanim->playing = 0;
                }
                break;
            case 5:
                instanim->repeating = 0;
                if (edanimParamDistance(i) < AnimParams[i].switch_var && !instanim->playing) {
                    instanim->playing = 1;
                    instanim->ltime = 1.0f;
                    instanim->waiting = 0;
                    instanim->backwards = 0;
                }
                break;
            case 6:
                instanim->repeating = 1;
                if (edanimParamDistance(i) < AnimParams[i].switch_var) {
                    instanim->playing = 1;
                }
                break;
            case 7:
            case 8:
            case 9:
                break;
            case 10:
                instanim->ltime = 1.0f;
                instanim->playing = 0;
                break;
            case 11:
                instanim->playing = 1;
                break;
            case 12:
                instanim->playing = 1;
                instanim->repeating = 1;
                break;
            }
            if (i == edanim_nearest_param_id && (edanim_particle_mode || edanim_sound_mode)) {
                instanim->ltime = 1.0f;
            }
        }
        for (j = 0; j < (int)dt; j++) {
            for (k = 0; k < AnimParams[i].particle_count; k++) {
                if (AnimParams[i].particle_type[k] != -1) {
                    if (AnimParams[i].particle_switch[k] != 0 && instanim == 0) {
                        continue;
                    }
                    if (AnimParams[i].particle_switch[k] != 0 && !instanim->playing) {
                        continue;
                    }
                    if (AnimParams[i].particle_switch[k] != 0
                        && instanim->ltime
                               >= edanim_base_scene->instanimdata[instanim->anim_ix]->time) {
                        continue;
                    }
                    if (instanim != 0) {
                        NuMtxInvRSS(&tmtx, &edanim_base_scene->specials[AnimParams[i].id].mtx);
                        NuMtxMul(&tmtx, &tmtx, &instanim->mtx);
                    } else {
                        NuMtxInvRSS(&tmtx, &edanim_base_scene->specials[AnimParams[i].id].mtx);
                        NuMtxMul(&tmtx, &tmtx, &instance->mtx);
                    }
                    emit = AnimParams[i].particle_pos[k];
                    NuVecMtxTransform(&emit, &emit, &tmtx);
                    orient = numtx_identity;
                    NuMtxRotateZ(&orient, AnimParams[i].particle_rotz[k]);
                    NuMtxRotateY(&orient, AnimParams[i].particle_roty[k]);
                    NuMtxMul(&tmtx, &orient, &tmtx);
                    if (AnimParams[i].particle_rate[k] > 0) {
                        AddVariableShotDebrisEffectMtx2(AnimParams[i].particle_type[k], &emit,
                                                        AnimParams[i].particle_rate[k], &tmtx,
                                                        &numtx_identity);
                    } else if (AnimParams[i].particle_rate[k] < 0
                               && (edanim_frame_count + j)
                                          % (AnimParams[i].particle_rate[k] >= 0
                                                 ? AnimParams[i].particle_rate[k]
                                                 : -AnimParams[i].particle_rate[k])
                                      == 0) {
                        AddVariableShotDebrisEffectMtx2(AnimParams[i].particle_type[k], &emit, 1,
                                                        &tmtx, &numtx_identity);
                    }
                } else if (AnimParams[i].particle_name[k][0] != 0) {
                    AnimParams[i].particle_type[k] =
                        LookupDebrisEffect(AnimParams[i].particle_name[k]);
                    if (AnimParams[i].particle_type[k] == -1) {
                        edanimParticleDestroy(i, k);
                        k--;
                    }
                }
            }
        }
        for (j = 0; j < AnimParams[i].sound_count; j++) {
            if (AnimParams[i].sound_type[j] != -1) {
                sid = -1;
                if (AnimParams[i].sound_switch[j] == 1) {
                    if ((int)(edanim_frame_count + dt) / (int)AnimParams[i].sound_timing[j]
                        > edanim_frame_count / (int)AnimParams[i].sound_timing[j]) {
                        sid = AnimParams[i].sound_type[j];
                    }
                } else if (instanim != 0) {
                    if (instanim->ltime >= AnimParams[i].sound_timing[j]
                        && AnimParams[i].sound_timing[j] > AnimParams[i].sound_last_time) {
                        sid = AnimParams[i].sound_type[j];
                    }
                    if (instanim->oscillate && instanim->ltime <= AnimParams[i].sound_timing[j]
                        && AnimParams[i].sound_timing[j]
                               < ((int)AnimParams[i].sound_last_time >= 0
                                      ? (int)AnimParams[i].sound_last_time
                                      : -(int)AnimParams[i].sound_last_time)) {
                        sid = AnimParams[i].sound_type[j];
                    }
                }
                if (sid != -1) {
                    if (instanim != 0) {
                        NuMtxInvRSS(&tmtx, &edanim_base_scene->specials[AnimParams[i].id].mtx);
                        NuMtxMul(&tmtx, &tmtx, &instanim->mtx);
                    } else {
                        NuMtxInvRSS(&tmtx, &edanim_base_scene->specials[AnimParams[i].id].mtx);
                        NuMtxMul(&tmtx, &tmtx, &instance->mtx);
                    }
                    emit = AnimParams[i].sound_pos[k];
                    NuVecMtxTransform(&emit, &emit, &tmtx);
                    edbitsSoundPlay(&emit, sid);
                }
            } else if (AnimParams[i].sound_name[j][0] != 0) {
                AnimParams[i].sound_type[j] = edbitsLookupSoundFX(AnimParams[i].sound_name[j]);
                if (AnimParams[i].sound_type[j] == -1) {
                    edanimSoundDestroy(i, j);
                    j--;
                }
            }
        }
        if (instanim != 0) {
            if (instanim->oscillate) {
                AnimParams[i].sound_last_time = -instanim->ltime;
            } else {
                AnimParams[i].sound_last_time = instanim->ltime;
            }
        }
    }
    edanim_frame_count += (int)dt;
}


void edanimDetermineNearestAnim(float range)
{
    struct nuvec_s delta;
    float dist;
    int i;

    if (edanim_base_scene == 0) {
        return;
    }
    if (edanim_nearest != -1) {
        NuVecSub(&delta, &edanim_cam_pos,
                 &edanim_base_scene->specials[edanim_nearest].instance->mtx.pos);
        if (delta.x * delta.x + delta.y * delta.y + delta.z * delta.z == 0.0) {
            return;
        }
    }
    edanim_nearest = -1;
    edanim_nearest_param_id = -1;
    for (i = 0; i < edanim_base_scene->numspecial; i++) {
        NuVecSub(&delta, &edanim_cam_pos, &edanim_base_scene->specials[i].instance->mtx.pos);
        dist = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
        if (range < 0.0f || dist < range) {
            edanim_nearest = i;
            range = dist;
        }
    }
    if (edanim_nearest != -1) {
        for (i = 0; i < 64; i++) {
            if (AnimParams[i].id == edanim_nearest) {
                edanim_nearest_param_id = i;
                break;
            }
        }
    }
}


void edanimDetermineNearestParticle(float range)
{
    struct nuvec_s delta;
    struct nuvec_s pos;
    struct nuvec_s *origin;
    float dist;
    int i;

    if (edanim_nearest == -1 || edanim_nearest_param_id == -1) {
        edanim_nearest_particle = -1;
        return;
    }
    origin = &edanim_base_scene->specials[edanim_nearest].mtx.pos;
    if (edanim_nearest_particle != -1) {
        NuVecAdd(&pos, origin,
                 &AnimParams[edanim_nearest_param_id].particle_pos[edanim_nearest_particle]);
        NuVecSub(&delta, &edanim_cam_pos, &pos);
        if (delta.x * delta.x + delta.y * delta.y + delta.z * delta.z == 0.0) {
            return;
        }
    }
    edanim_nearest_particle = -1;
    for (i = 0; i < AnimParams[edanim_nearest_param_id].particle_count; i++) {
        NuVecAdd(&pos, origin, &AnimParams[edanim_nearest_param_id].particle_pos[i]);
        NuVecSub(&delta, &edanim_cam_pos, &pos);
        dist = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
        if (range < 0.0f || dist < range) {
            edanim_nearest_particle = i;
            range = dist;
        }
    }
}


void edanimDetermineNearestSound(float range)
{
    struct nuvec_s delta;
    struct nuvec_s pos;
    struct nuvec_s *origin;
    float dist;
    int i;

    if (edanim_nearest == -1 || edanim_nearest_param_id == -1) {
        edanim_nearest_sound = -1;
        return;
    }
    origin = &edanim_base_scene->specials[edanim_nearest].mtx.pos;
    if (edanim_nearest_sound != -1) {
        NuVecAdd(&pos, origin,
                 &AnimParams[edanim_nearest_param_id].sound_pos[edanim_nearest_sound]);
        NuVecSub(&delta, &edanim_cam_pos, &pos);
        if (delta.x * delta.x + delta.y * delta.y + delta.z * delta.z == 0.0) {
            return;
        }
    }
    edanim_nearest_sound = -1;
    for (i = 0; i < AnimParams[edanim_nearest_param_id].sound_count; i++) {
        NuVecAdd(&pos, origin, &AnimParams[edanim_nearest_param_id].sound_pos[i]);
        NuVecSub(&delta, &edanim_cam_pos, &pos);
        dist = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
        if (range < 0.0f || dist < range) {
            edanim_nearest_sound = i;
            range = dist;
        }
    }
}


int edanimParamCreate(int special)
{
    int param;

    param = edanim_next_param;
    for (;;) {
        if (AnimParams[edanim_next_param].id == -1) {
            goto found;
        }
        edanim_next_param++;
        if (edanim_next_param > 64) {
            edanim_next_param = 0;
        }
        if (edanim_next_param == param) {
            return -1;
        }
    }
found:
    AnimParams[edanim_next_param].id = special;
    AnimParams[edanim_next_param].particle_count = 0;
    AnimParams[edanim_next_param].bouncy_instance =
        FindPlatInst(edanimLookupInstanceIndex(special));
    AnimParams[edanim_next_param].bouncy_player_grav = 0.0f;
    AnimParams[edanim_next_param].bouncy_tension = 0.0f;
    AnimParams[edanim_next_param].bouncy_damping = 0.0f;
    if (AnimParams[edanim_next_param].bouncy_instance != -1) {
        PlatInstBounce(AnimParams[edanim_next_param].bouncy_instance,
                       AnimParams[edanim_next_param].bouncy_player_grav,
                       AnimParams[edanim_next_param].bouncy_tension,
                       AnimParams[edanim_next_param].bouncy_damping);
    }
    param = edanim_next_param++;
    return param;
}


void edanimSoundCreate(struct nuvec_s *wpos)
{
    int slot;

    slot = AnimParams[edanim_nearest_param_id].sound_count;
    if (slot != 8 && edanim_sound_type != -1) {
        NuVecSub(&AnimParams[edanim_nearest_param_id].sound_pos[slot], wpos,
                 &edanim_base_scene->specials[edanim_nearest].mtx.pos);
        AnimParams[edanim_nearest_param_id].sound_type[slot] = edanim_sound_type;
        AnimParams[edanim_nearest_param_id].sound_switch[slot] = 1;
        strcpy(AnimParams[edanim_nearest_param_id].sound_name[slot],
               edbitsGetSoundName(edanim_sound_type));
        AnimParams[edanim_nearest_param_id].sound_timing[slot] = 50.0f;
        AnimParams[edanim_nearest_param_id].sound_count++;
    }
}


void edanimSoundDestroy(int param, int slot)
{
    int i;

    for (i = slot; i < AnimParams[param].sound_count - 1; i++) {
        AnimParams[param].sound_type[i] = AnimParams[param].sound_type[i + 1];
        AnimParams[param].sound_timing[i] = AnimParams[param].sound_timing[i + 1];
        AnimParams[param].sound_pos[i] = AnimParams[param].sound_pos[i + 1];
        strcpy(AnimParams[param].sound_name[i], AnimParams[param].sound_name[i + 1]);
    }
    AnimParams[param].sound_count--;
}


void edanimParticleCreate(struct nuvec_s *wpos)
{
    int slot;

    slot = AnimParams[edanim_nearest_param_id].particle_count;
    if (slot != 8 && edanim_particle_type != -1) {
        NuVecSub(&AnimParams[edanim_nearest_param_id].particle_pos[slot], wpos,
                 &edanim_base_scene->specials[edanim_nearest].mtx.pos);
        AnimParams[edanim_nearest_param_id].particle_rotz[slot] = edanim_emitrotz;
        AnimParams[edanim_nearest_param_id].particle_roty[slot] = edanim_emitroty;
        AnimParams[edanim_nearest_param_id].particle_type[slot] = edanim_particle_type;
        strcpy(AnimParams[edanim_nearest_param_id].particle_name[slot],
               debtab[edanim_particle_type]);
        AnimParams[edanim_nearest_param_id].particle_rate[slot] = 1;
        AnimParams[edanim_nearest_param_id].particle_switch[slot] = 0;
        AnimParams[edanim_nearest_param_id].particle_count++;
    }
}


void edanimParticleDestroy(int param, int slot)
{
    int i;

    for (i = slot; i < AnimParams[param].particle_count - 1; i++) {
        AnimParams[param].particle_type[i] = AnimParams[param].particle_type[i + 1];
        AnimParams[param].particle_rate[i] = AnimParams[param].particle_rate[i + 1];
        AnimParams[param].particle_switch[i] = AnimParams[param].particle_switch[i + 1];
        AnimParams[param].particle_pos[i] = AnimParams[param].particle_pos[i + 1];
        strcpy(AnimParams[param].particle_name[i], AnimParams[param].particle_name[i + 1]);
    }
    AnimParams[param].particle_count--;
}


inline void edanimParamDestroy(int param)
{
    if (AnimParams[param].id != -1) {
        AnimParams[param].id = -1;
    }
}


inline void edanimSoundPlace(int slot, struct nuvec_s *wpos)
{
    struct nuvec_s *origin = &edanim_base_scene->specials[edanim_nearest].mtx.pos;

    NuVecSub(&AnimParams[edanim_nearest_param_id].sound_pos[slot], wpos, origin);
}


inline void edanimParticlePlace(int slot, struct nuvec_s *wpos)
{
    struct nuvec_s *origin = &edanim_base_scene->specials[edanim_nearest].mtx.pos;

    NuVecSub(&AnimParams[edanim_nearest_param_id].particle_pos[slot], wpos, origin);
    AnimParams[edanim_nearest_param_id].particle_rotz[slot] = edanim_emitrotz;
    AnimParams[edanim_nearest_param_id].particle_roty[slot] = edanim_emitroty;
}


void edanimDoInput(struct pad_s *pad)
{
    struct nuvec_s pos;
    int i;

    if (!(pad->paddata & 0x100)) {
        edcamMove(pad);
    }
    if (pad->paddata & 0x100) {
        if (!edanim_sound_mode && (pad->buttons & 0x80)) {
            if (edanim_particle_mode) {
                edanim_particle_mode = 0;
            } else if (edanim_nearest != -1 && edanim_nearest_param_id != -1) {
                edanim_nearest_particle = -1;
                edanim_particle_mode = 1;
            }
        }
        if (!edanim_particle_mode && (pad->buttons & 0x10)) {
            if (edanim_sound_mode) {
                edanim_sound_mode = 0;
            } else if (edanim_nearest != -1 && edanim_nearest_param_id != -1) {
                edanim_nearest_sound = -1;
                edanim_sound_mode = 1;
            }
        }
        if (edanim_particle_mode) {
            if (edanim_nearest_particle != -1) {
                if (pad->buttons & 0x8) {
                    edanim_nearest_particle++;
                    if (edanim_nearest_particle
                        == AnimParams[edanim_nearest_param_id].particle_count) {
                        edanim_nearest_particle = 0;
                    }
                }
                if (pad->buttons & 0x2) {
                    edanim_nearest_particle--;
                    if (edanim_nearest_particle == -1) {
                        edanim_nearest_particle =
                            AnimParams[edanim_nearest_param_id].particle_count - 1;
                    }
                }
            } else {
                edanimDetermineNearestParticle(-1.0f);
            }
            if (edanim_nearest_particle != -1) {
                NuVecAdd(&pos, &edanim_base_scene->specials[edanim_nearest].mtx.pos,
                         &AnimParams[edanim_nearest_param_id].particle_pos[edanim_nearest_particle]);
                edcamSetPos(&pos);
                edanim_emitrotz =
                    AnimParams[edanim_nearest_param_id].particle_rotz[edanim_nearest_particle];
                edanim_emitroty =
                    AnimParams[edanim_nearest_param_id].particle_roty[edanim_nearest_particle];
                edanim_particle_type =
                    AnimParams[edanim_nearest_param_id].particle_type[edanim_nearest_particle];
            }
        } else if (edanim_sound_mode) {
            if (edanim_nearest_sound != -1) {
                if (pad->buttons & 0x8) {
                    edanim_nearest_sound++;
                    if (edanim_nearest_sound == AnimParams[edanim_nearest_param_id].sound_count) {
                        edanim_nearest_sound = 0;
                    }
                }
                if (pad->buttons & 0x2) {
                    edanim_nearest_sound--;
                    if (edanim_nearest_sound == -1) {
                        edanim_nearest_sound = AnimParams[edanim_nearest_param_id].sound_count - 1;
                    }
                }
            } else {
                edanimDetermineNearestSound(-1.0f);
            }
            if (edanim_nearest_sound != -1) {
                NuVecAdd(&pos, &edanim_base_scene->specials[edanim_nearest].mtx.pos,
                         &AnimParams[edanim_nearest_param_id].sound_pos[edanim_nearest_sound]);
                edcamSetPos(&pos);
                edanim_sound_type =
                    AnimParams[edanim_nearest_param_id].sound_type[edanim_nearest_sound];
            }
        } else {
            if (edanim_nearest != -1) {
                if (pad->buttons & 0x8) {
                    edanim_nearest++;
                    if (edanim_nearest == edanim_base_scene->numspecial) {
                        edanim_nearest = 0;
                    }
                }
                if (pad->buttons & 0x2) {
                    edanim_nearest--;
                    if (edanim_nearest == -1) {
                        edanim_nearest = edanim_base_scene->numspecial - 1;
                    }
                }
            } else {
                edanimDetermineNearestAnim(-1.0f);
            }
            if (edanim_nearest != -1) {
                edcamSetPos(&edanim_base_scene->specials[edanim_nearest].instance->mtx.pos);
                edanim_nearest_param_id = -1;
                for (i = 0; i < 64; i++) {
                    if (AnimParams[i].id == edanim_nearest) {
                        edanim_nearest_param_id = i;
                        break;
                    }
                }
            }
        }
    }
    edcamGetPosAng(&edanim_cam_pos, &edanim_cam_ax, &edanim_cam_ay);
    if (!(pad->paddata & 0x100)) {
        if (pad->buttons & 0x80) {
            edanim_active_menu = edanim_options_menu;
        }
        if (pad->buttons & 0x40) {
            if (edanim_particle_mode) {
                edanimParticleCreate(&edanim_cam_pos);
            } else if (edanim_sound_mode) {
                edanimSoundCreate(&edanim_cam_pos);
            } else if (edanim_nearest != -1 && edanim_nearest_param_id == -1) {
                edanim_nearest_param_id = edanimParamCreate(edanim_nearest);
            }
        }
        if (pad->paddata & 0x20) {
            if (edanim_particle_mode) {
                if (edanim_nearest_particle != -1) {
                    edanimParticlePlace(edanim_nearest_particle, &edanim_cam_pos);
                }
            } else if (edanim_sound_mode) {
                if (edanim_nearest_sound != -1) {
                    edanimSoundPlace(edanim_nearest_sound, &edanim_cam_pos);
                }
            }
        }
        if (pad->buttons & 0x10) {
            if (edanim_particle_mode) {
                if (edanim_nearest_particle != -1) {
                    edanimParticleDestroy(edanim_nearest_param_id, edanim_nearest_particle);
                    edanim_nearest_particle = -1;
                }
            } else if (edanim_sound_mode) {
                if (edanim_nearest_sound != -1) {
                    edanimSoundDestroy(edanim_nearest_param_id, edanim_nearest_sound);
                    edanim_nearest_sound = -1;
                }
            } else {
                if (edanim_nearest_param_id != -1) {
                    edanimParamDestroy(edanim_nearest_param_id);
                }
                edanim_nearest_param_id = -1;
                edanim_nearest = -1;
            }
        }
    }
    if (edanim_particle_mode) {
        edanim_emitroty += pad->right_pressure;
        edanim_emitroty -= pad->left_pressure;
        edanim_emitrotz += pad->up_pressure;
        if (edanim_emitrotz > 0) {
            edanim_emitrotz = 0;
        }
        edanim_emitrotz -= pad->down_pressure;
        if (edanim_emitrotz < -0x8000) {
            edanim_emitrotz = -0x8000;
        }
    }
}


int edanimFileSave(char *filename)
{
    char name[32];
    int count;
    int i;
    int j;

    count = 0;
    for (i = 0; i < 64; i++) {
        if (AnimParams[i].id != -1) {
            count++;
        }
    }
    EdFileSetMedia(1);
    if (EdFileOpen(filename, 1)) {
        EdFileWriteInt(4);
        EdFileWriteInt(count);
        for (i = 0; i < 64; i++) {
            if (AnimParams[i].id == -1) {
                continue;
            }
            strncpy(name, edanim_base_scene->specials[AnimParams[i].id].name, 20);
            name[20] = 0;
            EdFileWrite(name, 20);
            EdFileWriteInt(AnimParams[i].particle_count);
            EdFileWriteInt(AnimParams[i].sound_count);
            EdFileWriteInt(AnimParams[i].switch_type);
            EdFileWriteInt(AnimParams[i].switch_id);
            EdFileWriteFloat(AnimParams[i].switch_var);
            EdFileWriteFloat(AnimParams[i].switch_delay);
            for (j = 0; j < AnimParams[i].particle_count; j++) {
                EdFileWrite(AnimParams[i].particle_name[j], 16);
                EdFileWriteInt(AnimParams[i].particle_rate[j]);
                EdFileWriteInt(AnimParams[i].particle_switch[j]);
                EdFileWrite(&AnimParams[i].particle_pos[j], 12);
                EdFileWriteShort(AnimParams[i].particle_rotz[j]);
                EdFileWriteShort(AnimParams[i].particle_roty[j]);
            }
            for (j = 0; j < AnimParams[i].sound_count; j++) {
                EdFileWrite(AnimParams[i].sound_name[j], 16);
                EdFileWriteInt(AnimParams[i].sound_switch[j]);
                EdFileWriteFloat(AnimParams[i].sound_timing[j]);
                EdFileWrite(&AnimParams[i].sound_pos[j], 12);
            }
            EdFileWriteFloat(AnimParams[i].bouncy_player_grav);
            EdFileWriteFloat(AnimParams[i].bouncy_tension);
            EdFileWriteFloat(AnimParams[i].bouncy_damping);
        }
        EdFileClose();
        return 1;
    }
    return 0;
}


int edanimFileLoad(char *filename)
{
    char name[32];
    struct nuvec_s pos;
    char skipname[16];
    int current_version;
    int version;
    int count;
    int skip;
    int i;
    int j;

    current_version = 4;
    EdFileSetMedia(1);
    if (EdFileOpen(filename, 0) == 0) {
        return 0;
    }
    version = EdFileReadInt();
    if (version > current_version) {
        EdFileClose();
        return 0;
    }
    count = EdFileReadInt();
    for (i = 0; i < count; i++) {
        EdFileRead(name, 20);
        AnimParams[i].id = edanimLookupSpecial(name);
        AnimParams[i].particle_count = EdFileReadInt();
        if (version >= 2) {
            AnimParams[i].sound_count = EdFileReadInt();
        } else {
            AnimParams[i].sound_count = 0;
        }
        skip = 0;
        AnimParams[i].switch_type = EdFileReadInt();
        AnimParams[i].switch_id = EdFileReadInt();
        AnimParams[i].switch_var = EdFileReadFloat();
        AnimParams[i].switch_delay = EdFileReadFloat();
        if (AnimParams[i].particle_count > 8) {
            skip = 8 - AnimParams[i].particle_count;
            AnimParams[i].particle_count = 8;
        }
        for (j = 0; j < AnimParams[i].particle_count; j++) {
            EdFileRead(AnimParams[i].particle_name[j], 16);
            AnimParams[i].particle_type[j] = -1;
            AnimParams[i].particle_rate[j] = EdFileReadInt();
            AnimParams[i].particle_switch[j] = EdFileReadInt();
            EdFileRead(&AnimParams[i].particle_pos[j], 12);
            if (version >= 3) {
                AnimParams[i].particle_rotz[j] = EdFileReadShort();
                AnimParams[i].particle_roty[j] = EdFileReadShort();
            } else {
                AnimParams[i].particle_rotz[j] = 0;
                AnimParams[i].particle_roty[j] = 0;
            }
        }
        for (j = 0; j < skip; j++) {
            EdFileRead(skipname, 16);
            EdFileReadInt();
            EdFileReadInt();
            EdFileRead(&pos, 12);
            if (version >= 3) {
                EdFileReadShort();
                EdFileReadShort();
            }
        }
        AnimParams[i].sound_last_time = 0.99f;
        if (version >= 2) {
            skip = 0;
            if (AnimParams[i].sound_count > 8) {
                skip = 8 - AnimParams[i].sound_count;
                AnimParams[i].sound_count = 8;
            }
            for (j = 0; j < AnimParams[i].sound_count; j++) {
                EdFileRead(AnimParams[i].sound_name[j], 16);
                AnimParams[i].sound_type[j] = -1;
                AnimParams[i].sound_switch[j] = EdFileReadInt();
                AnimParams[i].sound_timing[j] = EdFileReadFloat();
                EdFileRead(&AnimParams[i].sound_pos[j], 12);
            }
            for (j = 0; j < skip; j++) {
                EdFileRead(skipname, 16);
                EdFileReadInt();
                EdFileReadFloat();
                EdFileRead(&pos, 12);
            }
        }
        AnimParams[i].bouncy_instance =
            FindPlatInst(edanimLookupInstanceIndex(AnimParams[i].id));
        if (version >= 4) {
            AnimParams[i].bouncy_player_grav = EdFileReadFloat();
            AnimParams[i].bouncy_tension = EdFileReadFloat();
            AnimParams[i].bouncy_damping = EdFileReadFloat();
        } else {
            AnimParams[i].bouncy_player_grav = 0.0f;
            AnimParams[i].bouncy_tension = 0.0f;
            AnimParams[i].bouncy_damping = 0.0f;
        }
        if (AnimParams[i].bouncy_instance != -1) {
            PlatInstBounce(AnimParams[i].bouncy_instance, AnimParams[i].bouncy_player_grav,
                           AnimParams[i].bouncy_tension, AnimParams[i].bouncy_damping);
        }
    }
    EdFileClose();
    edanim_next_param = count;
    edanim_nearest = -1;
    edanim_particle_mode = 0;
    edanim_nearest_param_id = -1;
    edanimDetermineNearestAnim(1.0f);
    return 1;
}


void edanimDrawCursor(void)
{
    struct nurndrvtx_s line[2];
    struct nuvec_s dir;

    line[0].pnt.x = edanim_cam_pos.x - 0.5f;
    line[0].pnt.y = edanim_cam_pos.y;
    line[0].pnt.z = edanim_cam_pos.z;
    line[1].pnt.x = edanim_cam_pos.x + 0.5f;
    line[1].pnt.y = edanim_cam_pos.y;
    line[1].pnt.z = edanim_cam_pos.z;
    line[0].colour = -1;
    line[1].colour = -1;
    NuRndrLine3d(line, edanim_mtl, 0);
    line[0].pnt.x = edanim_cam_pos.x;
    line[0].pnt.y = edanim_cam_pos.y - 0.5f;
    line[0].pnt.z = edanim_cam_pos.z;
    line[1].pnt.x = edanim_cam_pos.x;
    line[1].pnt.y = edanim_cam_pos.y + 0.5f;
    line[1].pnt.z = edanim_cam_pos.z;
    line[0].colour = -1;
    line[1].colour = -1;
    NuRndrLine3d(line, edanim_mtl, 0);
    line[0].pnt.x = edanim_cam_pos.x;
    line[0].pnt.y = edanim_cam_pos.y;
    line[0].pnt.z = edanim_cam_pos.z - 0.5f;
    line[1].pnt.x = edanim_cam_pos.x;
    line[1].pnt.y = edanim_cam_pos.y;
    line[1].pnt.z = edanim_cam_pos.z + 0.5f;
    line[0].colour = -1;
    line[1].colour = -1;
    NuRndrLine3d(line, edanim_mtl, 0);
    if (edanim_particle_mode) {
        dir.x = 0.0f;
        dir.y = 0.375f;
        dir.z = 0.0f;
        NuVecRotateZ(&dir, &dir, edanim_emitrotz);
        NuVecRotateY(&dir, &dir, edanim_emitroty);
        line[0].pnt.x = edanim_cam_pos.x;
        line[0].pnt.y = edanim_cam_pos.y;
        line[0].pnt.z = edanim_cam_pos.z;
        line[1].pnt.x = edanim_cam_pos.x + dir.x;
        line[1].pnt.y = edanim_cam_pos.y + dir.y;
        line[1].pnt.z = edanim_cam_pos.z + dir.z;
        line[0].colour = 0xFF0000FF;
        line[1].colour = 0xFF0000FF;
        NuRndrLine3d(line, edanim_mtl, 0);
    }
    NuRndrRect2di(0x1720, 0xA50, 0xDC0, 0x370, 0x80808080, edanim_mtl_zoff);
    NuRndrRect2di(0x1710, 0x990, 0xDE0, 0xC0, 0x80000000, edanim_mtl_zoff);
    NuRndrLine2di(0x1710, 0xA50, 0x1710, 0xDC8, 0x80000000, edanim_mtl_zoff);
    NuRndrLine2di(0x24F0, 0xA50, 0x24F0, 0xDC8, 0x80000000, edanim_mtl_zoff);
    NuRndrLine2di(0x1710, 0xDC8, 0x24F0, 0xDC8, 0x80000000, edanim_mtl_zoff);
    NuFntSet(app_fnt);
    NuFntSetPen(0x80808080);
    NuFntPrintEx(0x17C0, 0xA28, 0, cursor_text_info_box);
    NuFntSetPen(0x80000000);
    if (edanim_nearest == -1) {
        NuFntPrintEx(0x17C0, 0xAF0, 0, cursor_text_no_special);
    } else {
        NuFntPrintEx(0x17C0, 0xAF0, 0, cursor_text_special,
                     edanim_base_scene->specials[edanim_nearest].name);
    }
    if (edanim_nearest_param_id == -1) {
        NuFntPrintEx(0x17C0, 0xB90, 0, cursor_text_no_params);
    } else {
        NuFntPrintEx(0x17C0, 0xB90, 0, cursor_text_params);
        if (edanim_particle_mode) {
            NuFntPrintEx(0x1810, 0xC30, 0, cursor_text_particles,
                         AnimParams[edanim_nearest_param_id].particle_count, 8);
            if (edanim_particle_type == -1) {
                NuFntPrintEx(0x1810, 0xCD0, 0, cursor_text_no_type);
            } else {
                NuFntPrintEx(0x1810, 0xCD0, 0, cursor_text_type, debtab[edanim_particle_type]);
            }
        } else if (edanim_sound_mode) {
            NuFntPrintEx(0x1810, 0xC30, 0, cursor_text_sounds,
                         AnimParams[edanim_nearest_param_id].sound_count, 8);
            if (edanim_sound_type == -1) {
                NuFntPrintEx(0x1810, 0xCD0, 0, cursor_text_no_type);
            } else {
                NuFntPrintEx(0x1810, 0xCD0, 0, cursor_text_type,
                             edbitsGetSoundName(edanim_sound_type));
            }
        }
    }
    NuFntPrintEx(0x17C0, 0xD70, 0, cursor_text_coord, edanim_cam_pos.x);
    NuFntPrintEx(0x1C20, 0xD70, 0, cursor_text_coord, edanim_cam_pos.y);
    NuFntPrintEx(0x2080, 0xD70, 0, cursor_text_coord, edanim_cam_pos.z);
}


static inline void edanimAddSwitchTypeItem(struct eduistyle_s *style, int type, char *text)
{
    eduiMenuAddItem(edanim_switchtype_menu,
                    eduiItemCheckCreate(type, style,
                                        AnimParams[edanim_nearest_param_id].switch_type == type,
                                        1, edanimcbSetSwitchType, text));
    if (edui_last_item->toggle) {
        edanim_switchtype_menu->current = edui_last_item;
    }
}


void edanimcbSwitchTypeMenu(struct eduimenu_s *menu)
{
    struct eduistyle_s style;

    style = edanim_item_style;
    edanim_switchtype_menu = eduiMenuCreate(70, 70, 250, 200, app_fnt,
                                            edanimcbCancelSwitchTypeMenu, switchtype_menu_title);
    if (edanim_switchtype_menu != 0) {
        edanimAddSwitchTypeItem(&style, 0, switchtype_text_none);
        edanimAddSwitchTypeItem(&style, 1, switchtype_text_switch);
        edanimAddSwitchTypeItem(&style, 2, switchtype_text_switch_once);
        edanimAddSwitchTypeItem(&style, 3, switchtype_text_switch_cont);
        edanimAddSwitchTypeItem(&style, 4, switchtype_text_proximity);
        edanimAddSwitchTypeItem(&style, 5, switchtype_text_proximity_once);
        edanimAddSwitchTypeItem(&style, 6, switchtype_text_proximity_cont);
        edanimAddSwitchTypeItem(&style, 7, switchtype_text_terrain);
        edanimAddSwitchTypeItem(&style, 8, switchtype_text_terrain_once);
        edanimAddSwitchTypeItem(&style, 9, switchtype_text_terrain_cont);
        edanimAddSwitchTypeItem(&style, 10, switchtype_text_override_noanim);
        edanimAddSwitchTypeItem(&style, 11, switchtype_text_override_play);
        edanimAddSwitchTypeItem(&style, 12, switchtype_text_override_playcont);
        eduiMenuAttach(menu, edanim_switchtype_menu);
        edanim_switchtype_menu->x = menu->x + 10;
        edanim_switchtype_menu->y = menu->y + 40;
    }
}


void edanimcbSwitchMenu(struct eduimenu_s *menu)
{
    struct eduistyle_s style;

    style = edanim_item_style;
    if (edanim_nearest != -1 && edanim_nearest_param_id != -1) {
        edanim_switch_menu = eduiMenuCreate(70, 70, 180, 250, app_fnt, edanimcbCancelSwitchMenu,
                                            switch_menu_title);
        if (edanim_switch_menu != 0) {
            eduiMenuAddItem(edanim_switch_menu,
                            eduiItemSelCreate(1, &style, 0, 0, edanimcbSwitchTypeMenu,
                                              switch_menu_text_type));
            eduiMenuAddItem(edanim_switch_menu,
                            eduiItemSliderCreateInt(0, &style, 0, edanimcbSetSwitchId, -1, 129,
                                                    AnimParams[edanim_nearest_param_id].switch_id,
                                                    switch_menu_text_id));
            eduiMenuAddItem(edanim_switch_menu,
                            eduiItemSliderCreate(0, &style, 0, edanimcbSetSwitchDelay, 0.0f, 20.0f,
                                                 AnimParams[edanim_nearest_param_id].switch_delay,
                                                 switch_menu_text_delay));
            eduiMenuAddItem(edanim_switch_menu,
                            eduiItemSliderCreate(0, &style, 0, edanimcbSetSwitchVar, 0.0f, 20.0f,
                                                 AnimParams[edanim_nearest_param_id].switch_var,
                                                 switch_menu_text_var));
            eduiMenuAttach(menu, edanim_switch_menu);
            edanim_switch_menu->x = menu->x + 10;
            edanim_switch_menu->y = menu->y + 40;
        }
    }
}


void edanimcbBouncyMenu(struct eduimenu_s *menu)
{
    struct eduistyle_s style;

    style = edanim_item_style;
    if (edanim_nearest != -1) {
        edanim_bouncy_menu = eduiMenuCreate(70, 70, 250, 300, app_fnt, edanimcbCancelBouncyMenu,
                                            bouncy_menu_title);
        if (edanim_bouncy_menu != 0) {
            eduiMenuAddItem(edanim_bouncy_menu,
                            eduiItemSliderCreate(0, &style, 0, edanimcbSetBouncyPlayerGrav, -0.1f,
                                                 0.2f,
                                                 AnimParams[edanim_nearest_param_id].bouncy_player_grav,
                                                 bouncy_menu_text_grav));
            eduiMenuAddItem(edanim_bouncy_menu,
                            eduiItemSliderCreate(0, &style, 0, edanimcbSetBouncyTension, 0.0f, 1.0f,
                                                 AnimParams[edanim_nearest_param_id].bouncy_tension,
                                                 bouncy_menu_text_tension));
            eduiMenuAddItem(edanim_bouncy_menu,
                            eduiItemSliderCreate(0, &style, 0, edanimcbSetBouncyDamping, 0.0f, 1.0f,
                                                 AnimParams[edanim_nearest_param_id].bouncy_damping,
                                                 bouncy_menu_text_damping));
            eduiMenuAttach(menu, edanim_bouncy_menu);
            edanim_bouncy_menu->x = menu->x + 10;
            edanim_bouncy_menu->y = menu->y + 40;
        }
    }
}


void edanimcbParticleTypeMenu(struct eduimenu_s *menu)
{
    struct eduistyle_s style;
    int selected;
    int i;

    style = edanim_item_style;
    edanim_particletype_menu = eduiMenuCreate(70, 70, 250, 250, app_fnt,
                                              edanimcbCancelParticleTypeMenu,
                                              particletype_menu_title);
    if (edanim_particletype_menu != 0) {
        eduiMenuAddItem(edanim_particletype_menu,
                        eduiItemCheckCreate(0, &style, edanim_particle_type == -1, 0,
                                            edanimcbSetParticleType, particletype_text_none));
        for (i = 1; i < 128; i++) {
            if (debtab[i] != 0) {
                selected = 0;
                if (edanim_particle_type == i) {
                    selected = 1;
                }
                eduiMenuAddItem(edanim_particletype_menu,
                                eduiItemCheckCreate(i, &style, selected, 1,
                                                    edanimcbSetParticleType, debtab[i]));
                if (selected == 1) {
                    edanim_particletype_menu->current = edui_last_item;
                }
            }
        }
        eduiMenuAttach(menu, edanim_particletype_menu);
        edanim_particletype_menu->x = menu->x + 10;
        edanim_particletype_menu->y = menu->y + 40;
    }
}


void edanimcbLocalParticleMenu(struct eduimenu_s *menu)
{
    struct eduistyle_s style;

    style = edanim_item_style;
    if (edanim_nearest_particle != -1) {
        edanim_localparticle_menu = eduiMenuCreate(70, 70, 250, 300, app_fnt,
                                                   edanimcbCancelLocalParticleMenu,
                                                   localparticle_menu_title);
        if (edanim_localparticle_menu != 0) {
            eduiMenuAddItem(edanim_localparticle_menu,
                            eduiItemSelCreate(1, &style, 0, 0, edanimcbLocalParticleTypeMenu,
                                              localparticle_menu_text_type));
            eduiMenuAddItem(
                edanim_localparticle_menu,
                eduiItemSliderCreateInt(
                    0, &style, 0, edanimcbSetParticleRate, -10, 20,
                    AnimParams[edanim_nearest_param_id].particle_rate[edanim_nearest_particle],
                    localparticle_menu_text_rate));
            eduiMenuAddItem(
                edanim_localparticle_menu,
                eduiItemToggleCreate(
                    0, &style,
                    AnimParams[edanim_nearest_param_id].particle_switch[edanim_nearest_particle], 1,
                    edanimcbToggleParticleSwitch, localparticle_menu_text_moving));
            eduiMenuAttach(menu, edanim_localparticle_menu);
            edanim_localparticle_menu->x = menu->x + 10;
            edanim_localparticle_menu->y = menu->y + 40;
        }
    }
}


void edanimRender(void)
{
    int colour;
    struct nuinstance_s *instance;

    edcamSet();
    edanimDrawCursor();
    if (edanim_nearest != -1) {
        if (edanim_particle_mode) {
            colour = 0xFF00FF00;
        } else if (edanim_sound_mode) {
            colour = 0xFFFF0000;
        } else if (edanim_nearest_param_id != -1) {
            colour = 0xFFFFFFFF;
        } else {
            colour = 0xFF808080;
        }
        instance = edanim_base_scene->specials[edanim_nearest].instance;
        edbitsDrawCube(instance->mtx.pos.x, instance->mtx.pos.y, instance->mtx.pos.z,
                       0.5f, 0.5f, 0.5f, 0, 0, 0, 0, colour, edanim_mtl_zoff);
        if (edanim_particle_mode) {
            edanimRenderParticleEmitters(edanim_nearest_param_id);
        }
        if (edanim_sound_mode) {
            edanimRenderSoundEmitters(edanim_nearest_param_id);
        }
    }
    if (edanim_active_menu != 0) {
        NuFntScale(12, 12);
        eduiMenuRender(edanim_active_menu);
    }
}


int edanimLookupInstanceIndex(int special)
{
    int i;

    if (edanim_base_scene == 0) {
        return -1;
    }
    for (i = 0; i < edanim_base_scene->numinstance; i++) {
        if (&edanim_base_scene->instances[i]
            == edanim_base_scene->specials[special].instance) {
            return i;
        }
    }
    return -1;
}


void edanimRegisterBaseScene(struct nugscn_s *scene)
{
    edanim_base_scene = scene;
}


int edanimLookupSpecial(char *name)
{
    int i;

    if (edanim_base_scene != 0) {
        for (i = 0; i < edanim_base_scene->numspecial; i++) {
            if (strncmp(edanim_base_scene->specials[i].name, name, 19) == 0) {
                return i;
            }
        }
    }
    return -1;
}


void edanimParamReset(void)
{
    int i;

    for (i = 0; i < 64; i++) {
        AnimParams[i].id = -1;
    }
    edanim_next_param = 0;
}


float edanimPlayerAnimDistance(int param)
{
    if (edmainQueryLocVec() != 0) {
        return NuVecDist(&edanim_base_scene->specials[AnimParams[param].id].mtx.pos,
                         edmainQueryLocVec(), 0);
    }
    return 0.0f;
}


void edanimcbSetSwitchType(void *menu, struct edanimitem_s *item)
{
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
    edanim_switchtype_menu = 0;
    AnimParams[edanim_nearest_param_id].switch_type = item->value;
}


void edanimcbSetSwitchId(void *menu, struct edanimitem_s *item)
{
    if (edanim_nearest != -1 && edanim_nearest_param_id != -1) {
        AnimParams[edanim_nearest_param_id].switch_id = item->fvalue;
    }
}


void edanimcbSetSwitchDelay(void *menu, struct edanimitem_s *item)
{
    if (edanim_nearest != -1 && edanim_nearest_param_id != -1) {
        AnimParams[edanim_nearest_param_id].switch_delay = item->fvalue;
    }
}


void edanimcbSetSwitchVar(void *menu, struct edanimitem_s *item)
{
    if (edanim_nearest != -1 && edanim_nearest_param_id != -1) {
        AnimParams[edanim_nearest_param_id].switch_var = item->fvalue;
    }
}


void edanimcbCancelSwitchTypeMenu(void)
{
    eduiMenuDestroy(edanim_switchtype_menu);
    edanim_switchtype_menu = 0;
}


void edanimcbCancelSwitchMenu(void)
{
    eduiMenuDestroy(edanim_switch_menu);
    edanim_switch_menu = 0;
}


void edanimcbSetBouncyPlayerGrav(void *menu, struct edanimitem_s *item)
{
    int id;

    if (edanim_nearest != -1) {
        id = edanim_nearest_param_id;
        AnimParams[id].bouncy_player_grav = item->fvalue;
        if (AnimParams[id].bouncy_instance != -1) {
            PlatInstBounce(AnimParams[id].bouncy_instance,
                           AnimParams[id].bouncy_player_grav,
                           AnimParams[id].bouncy_tension,
                           AnimParams[id].bouncy_damping);
        }
    }
}


void edanimcbSetBouncyTension(void *menu, struct edanimitem_s *item)
{
    int id;

    if (edanim_nearest != -1) {
        id = edanim_nearest_param_id;
        AnimParams[id].bouncy_tension = item->fvalue;
        if (AnimParams[id].bouncy_instance != -1) {
            PlatInstBounce(AnimParams[id].bouncy_instance,
                           AnimParams[id].bouncy_player_grav,
                           AnimParams[id].bouncy_tension,
                           AnimParams[id].bouncy_damping);
        }
    }
}


void edanimcbSetBouncyDamping(void *menu, struct edanimitem_s *item)
{
    int id;

    if (edanim_nearest != -1) {
        id = edanim_nearest_param_id;
        AnimParams[id].bouncy_damping = item->fvalue;
        if (AnimParams[id].bouncy_instance != -1) {
            PlatInstBounce(AnimParams[id].bouncy_instance,
                           AnimParams[id].bouncy_player_grav,
                           AnimParams[id].bouncy_tension,
                           AnimParams[id].bouncy_damping);
        }
    }
}


void edanimcbCancelBouncyMenu(void)
{
    eduiMenuDestroy(edanim_bouncy_menu);
    edanim_bouncy_menu = 0;
}


void edanimcbToggleSoundType(void *menu, struct edanimitem_s *item)
{
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
    edanim_soundtype_menu = 0;
    if (item->toggle == 0) {
        AnimParams[edanim_nearest_param_id].sound_switch[edanim_nearest_sound] = 0;
        AnimParams[edanim_nearest_param_id].sound_timing[edanim_nearest_sound] = 1.0f;
    } else {
        AnimParams[edanim_nearest_param_id].sound_switch[edanim_nearest_sound] = 1;
        AnimParams[edanim_nearest_param_id].sound_timing[edanim_nearest_sound] = 50.0f;
    }
}


void edanimcbSetSoundTiming(void *menu, struct edanimitem_s *item)
{
    AnimParams[edanim_nearest_param_id].sound_timing[edanim_nearest_sound] = item->fvalue;
}


void edanimcbToggleParticleSwitch(void *menu, struct edanimitem_s *item)
{
    if (edanim_nearest_param_id != -1 && edanim_nearest_particle != -1) {
        AnimParams[edanim_nearest_param_id].particle_switch[edanim_nearest_particle] =
            item->toggle;
    }
}


void edanimcbSetParticleType(void *menu, struct edanimitem_s *item)
{
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
    edanim_particletype_menu = 0;
    if (item->value == 0) {
        edanim_particle_type = -1;
    } else {
        edanim_particle_type = item->value;
    }
}


void edanimcbSetParticleRate(void *menu, struct edanimitem_s *item)
{
    AnimParams[edanim_nearest_param_id].particle_rate[edanim_nearest_particle] =
        item->fvalue;
}


void edanimcbCancelParticleTypeMenu(void)
{
    eduiMenuDestroy(edanim_particletype_menu);
    edanim_particletype_menu = 0;
}


void edanimcbCancelLocalParticleMenu(void)
{
    eduiMenuDestroy(edanim_localparticle_menu);
    edanim_localparticle_menu = 0;
}


void edanimcbParticleMenu(struct eduimenu_s *menu)
{
    struct eduistyle_s style;

    style = edanim_item_style;
    if (edanim_nearest_param_id != -1) {
        edanim_particle_menu = eduiMenuCreate(70, 70, 250, 300, app_fnt,
                                              edanimcbCancelParticleMenu, particle_menu_title);
        if (edanim_particle_menu != 0) {
            eduiMenuAddItem(edanim_particle_menu,
                            eduiItemSelCreate(1, &style, 0, 0, edanimcbParticleTypeMenu,
                                              particle_menu_text_type));
            if (edanim_nearest_particle != -1) {
                eduiMenuAddItem(edanim_particle_menu,
                                eduiItemSelCreate(1, &style, 0, 0, edanimcbLocalParticleMenu,
                                                  particle_menu_text_local));
            }
            eduiMenuAttach(menu, edanim_particle_menu);
            edanim_particle_menu->x = menu->x + 10;
            edanim_particle_menu->y = menu->y + 40;
        }
    }
}


void edanimcbCancelLocalSoundMenu(void)
{
    eduiMenuDestroy(edanim_localsound_menu);
    edanim_localsound_menu = 0;
}


void edanimcbSoundMenu(struct eduimenu_s *menu)
{
    struct eduistyle_s style;

    style = edanim_item_style;
    if (edanim_nearest_param_id != -1) {
        edanim_sound_menu = eduiMenuCreate(70, 70, 250, 300, app_fnt,
                                           edanimcbCancelSoundMenu, sound_menu_title);
        if (edanim_sound_menu != 0) {
            eduiMenuAddItem(edanim_sound_menu,
                            eduiItemSelCreate(1, &style, 0, 0, edanimcbSoundTypeMenu,
                                              sound_menu_text_type));
            if (edanim_nearest_sound != -1) {
                eduiMenuAddItem(edanim_sound_menu,
                                eduiItemSelCreate(1, &style, 0, 0, edanimcbLocalSoundMenu,
                                                  sound_menu_text_local));
            }
            eduiMenuAttach(menu, edanim_sound_menu);
            edanim_sound_menu->x = menu->x + 10;
            edanim_sound_menu->y = menu->y + 40;
        }
    }
}


void edanimcbCancelOptMenu(void)
{
    edanim_active_menu = 0;
}


void edanimcbFileSave(void *menu)
{
    char filename[256];

    if (edbits_what_game == 1) {
        sprintf(filename, anim_file_format_game1, edbits_level_filename);
        if (edanimFileSave(filename)) {
            eduiCreateMessageMenu(menu, anim_text_saved, 1);
        } else {
            eduiCreateMessageMenu(menu, anim_text_save_failed, 0);
        }
    } else if (edbits_what_game == 2) {
        sprintf(filename, anim_file_format_game2, edbits_level_filename);
        if (edanimFileSave(filename)) {
            eduiCreateMessageMenu(menu, anim_text_saved, 1);
        } else {
            eduiCreateMessageMenu(menu, anim_text_save_failed, 0);
        }
    } else {
        eduiCreateMessageMenu(menu, anim_text_save_failed, 0);
    }
}


void edanimcbFileLoad(void *menu)
{
    char filename[256];

    if (edbits_what_game == 1) {
        sprintf(filename, anim_file_format_game1, edbits_level_filename);
        if (NuFileSize(filename) > 0) {
            edanimFileLoad(filename);
        }
        eduiCreateMessageMenu(menu, anim_text_loaded, 1);
    } else if (edbits_what_game == 2) {
        sprintf(filename, anim_file_format_game2, edbits_level_filename);
        if (NuFileSize(filename) > 0) {
            edanimFileLoad(filename);
        }
        eduiCreateMessageMenu(menu, anim_text_loaded, 1);
    } else {
        eduiCreateMessageMenu(menu, anim_text_load_failed, 0);
    }
}


void edanimcbMCTBCardPresent(void *menu)
{
    if (saveloadCheckCardPresent()) {
        eduiCreateMessageMenu(menu, mctb_text_present, 1);
    } else {
        eduiCreateMessageMenu(menu, mctb_text_not_present, 0);
    }
}


void edanimcbMCTBCardType(void *menu)
{
    if (saveloadCheckCardType()) {
        eduiCreateMessageMenu(menu, mctb_text_ps2_card, 1);
    } else {
        eduiCreateMessageMenu(menu, mctb_text_not_ps2_card, 0);
    }
}


void edanimcbMCTBCardCheckFormat(void *menu)
{
    if (saveloadCheckCardFormatted()) {
        eduiCreateMessageMenu(menu, mctb_text_formatted, 1);
    } else {
        eduiCreateMessageMenu(menu, mctb_text_not_formatted, 0);
    }
}


void edanimcbMCTBCardFreeSpace(void *menu)
{
    char text[32];
    int space;

    space = saveloadCheckCardFreeSpace(0);
    sprintf(text, mctb_text_free_space, space);
    eduiCreateMessageMenu(menu, text, 1);
}


void edanimcbMCTBCardSlotsUsed(void *menu)
{
    char text[32];
    int used;

    used = saveloadCheckSlotsUsed();
    sprintf(text, mctb_text_slots_used, used);
    eduiCreateMessageMenu(menu, text, 1);
}


void edanimcbMCTBCardSaveSlot(void *menu, struct edanimitem_s *item)
{
    char text[32];

    sprintf(text, mctb_text_slot_name, item->value);
    if (saveloadSaveSlot(item->value, text, 32)) {
        eduiCreateMessageMenu(menu, anim_text_saved, 1);
    } else {
        eduiCreateMessageMenu(menu, mctb_text_save_failed, 0);
    }
}


void edanimcbMCTBCardLoadSlot(void *menu, struct edanimitem_s *item)
{
    char text[32];

    if (saveloadLoadSlot(item->value, text, 32)) {
        eduiCreateMessageMenu(menu, text, 1);
    } else {
        eduiCreateMessageMenu(menu, mctb_text_load_failed, 0);
    }
}


void edanimcbMCTBCardDeleteSlot(void *menu, struct edanimitem_s *item)
{
    if (saveloadDeleteSlot(item->value)) {
        eduiCreateMessageMenu(menu, mctb_text_deleted, 1);
    } else {
        eduiCreateMessageMenu(menu, mctb_text_delete_failed, 0);
    }
}


void edanimcbMCTBCardUnFormat(void *menu)
{
    if (saveloadUnFormatCard()) {
        eduiCreateMessageMenu(menu, mctb_text_unformatted, 1);
    } else {
        eduiCreateMessageMenu(menu, mctb_text_unformat_failed, 0);
    }
}


void edanimcbMCTBCardFormat(void *menu)
{
    if (saveloadFormatCard()) {
        eduiCreateMessageMenu(menu, mctb_text_formatted_ok, 1);
    } else {
        eduiCreateMessageMenu(menu, mctb_text_format_failed, 0);
    }
}


void edanimcbCancelMCTBMenu(void)
{
    eduiMenuDestroy(edanim_mctb_menu);
    edanim_mctb_menu = 0;
}


void edanimClose(void)
{
    eduiMenuDestroy(edanim_options_menu);
}


void edanimEnter(void)
{
    edanim_nearest = -1;
    edanim_nearest_param_id = -1;
    edanim_nearest_particle = -1;
    edanim_particle_mode = 0;
    edanim_particle_type = -1;
    edanim_sound_type = -1;
}


int edanimProc(struct pad_s *pad)
{
    if (edanim_active_menu != 0) {
        eduiMenuProcess(edanim_active_menu, pad);
        return 0;
    }
    edanimDoInput(pad);
    if (edanim_particle_mode) {
        edanimDetermineNearestParticle(1.0f);
    } else if (edanim_sound_mode) {
        edanimDetermineNearestSound(1.0f);
    } else {
        edanimDetermineNearestAnim(1.0f);
    }
    if (pad->buttons & 0x800) {
        edanim_particle_mode = 0;
        return 1;
    }
    return 0;
}


void edanimcbSetLocalSoundType(void *menu, struct edanimitem_s *item)
{
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
    edanim_localsoundtype_menu = 0;
    AnimParams[edanim_nearest_param_id].sound_type[edanim_nearest_sound] = item->value;
    strcpy(AnimParams[edanim_nearest_param_id].sound_name[edanim_nearest_sound],
           edbitsGetSoundName(item->value));
    edanim_sound_type = item->value;
}


void edanimcbSetLocalParticleType(void *menu, struct edanimitem_s *item)
{
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
    edanim_localparticletype_menu = 0;
    AnimParams[edanim_nearest_param_id].particle_type[edanim_nearest_particle] = item->value;
    strcpy(AnimParams[edanim_nearest_param_id].particle_name[edanim_nearest_particle],
           debtab[edanim_nearest_particle]);
    edanim_particle_type = item->value;
}


void edanimcbCancelLocalParticleTypeMenu(void)
{
    eduiMenuDestroy(edanim_localparticletype_menu);
    edanim_localparticletype_menu = 0;
}


void edanimcbCancelParticleMenu(void)
{
    eduiMenuDestroy(edanim_particle_menu);
    edanim_particle_menu = 0;
}


void edanimcbCancelLocalSoundTypeMenu(void)
{
    eduiMenuDestroy(edanim_localsoundtype_menu);
    edanim_localsoundtype_menu = 0;
}


void edanimcbCancelSoundMenu(void)
{
    eduiMenuDestroy(edanim_sound_menu);
    edanim_sound_menu = 0;
}


void edanimcbSetSoundType(void *menu, struct edanimitem_s *item)
{
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
    edanim_soundtype_menu = 0;
    if (item->value == 99999) {
        edanim_sound_type = -1;
    } else {
        edanim_sound_type = item->value;
    }
}


void edanimcbCancelSoundTypeMenu(void)
{
    eduiMenuDestroy(edanim_soundtype_menu);
    edanim_soundtype_menu = 0;
}
