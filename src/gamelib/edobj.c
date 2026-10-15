/*
 * Unit: gamelib/edobj
 *
 * Functions:
 *   0x00197420 edobjUpdateObjects
 *   0x001981e8 edobjRenderObjects
 *   0x001983b0 edobjDetermineNearestObject
 *   0x00198500 edobjDetermineNearestWaypoint
 *   0x001986a0 edobjDetermineNearestParticle
 *   0x00198880 edobjDetermineNearestSound
 *   0x00198a60 edobjConvertPathToAnim
 *   0x0019a838 edobjSoundCreate
 *   0x0019a950 edobjSoundDestroy
 *   0x0019aa98 edobjParticleCreate
 *   0x0019abc0 edobjParticleDestroy
 *   0x0019ad10 edobjWaypointCreate
 *   0x0019ae48 edobjWaypointDestroy
 *   0x0019afe8 edobjObjectCreate
 *   0x0019b1a0 edobjObjectCreateCopy
 *   0x0019b348 edobjObjectDestroy
 *   0x0019b428 edobjDoInput
 *   0x0019bee0 edobjDrawCursor
 *   0x0019c570 edobjFileSaveObjects
 *   0x0019c968 edobjFileLoadObjects
 *   0x0019d430 edobjcbInstanceMenu
 *   0x0019d5c0 edobjcbSwitchTypeMenu
 *   0x0019d9d8 edobjcbSwitchMenu
 *   0x0019dbd8 edobjcbAnimPropertiesMenu
 *   0x0019de08 edobjcbWayPropertiesMenu
 *   0x0019e130 edobjcbLocalSoundMenu
 *   0x0019e358 edobjcbParticleTypeMenu
 *   0x0019e4d8 edobjcbLocalParticleMenu
 *   0x0019e668 edobjcbBouncyMenu
 *   0x0019e818 edobjInit
 *   0x0019ebe0 edobjRenderSoundEmitters
 *   0x0019ed20 edobjRenderParticleEmitters
 *   0x0019eec0 edobjRenderWaypoints
 *   0x0019f050 edobjRender
 *   0x0019f190 edobjRegisterLevel
 *   0x0019f1b0 edobjResetAnimsToZero
 *   0x0019f230 edobjRegisterBaseScene
 *   0x0019f238 edobjObjectReset
 *   0x0019f278 edobjLookupInstance
 *   0x0019f300 edobjObjectDestroyAll
 *   0x0019f338 edobjPlayerObjectDistance
 *   0x0019f3a0 edobjRenderCutoffTest
 *   0x0019f418 edobjcbResetAnims
 *   0x0019f498 edobjLookupInstanceIndex
 *   0x0019f4f8 edobjSoundPlace
 *   0x0019f540 edobjParticlePlace
 *   0x0019f5c8 edobjWaypointPlace
 *   0x0019f628 edobjObjectPlace
 *   0x0019f6d8 edobjcbCancelOptMenu
 *   0x0019f6e0 edobjcbFileSaveObjects
 *   0x0019f7b8 edobjcbFileLoadObjects
 *   0x0019f868 edobjcbChangeAnimSpeed
 *   0x0019f8a8 edobjcbChangeAnimPause
 *   0x0019f8e8 edobjcbChangeAnimStartOffset
 *   0x0019f920 edobjcbOscillateToggle
 *   0x0019f960 edobjcbSnapYToggle
 *   0x0019f9c0 edobjcbSnapXZToggle
 *   0x0019fa20 edobjcbToggleParticleSwitch
 *   0x0019fa60 edobjcbChangeWaypointSpeed
 *   0x0019fab8 edobjcbChangeWaypointXRot
 *   0x0019fb20 edobjcbChangeWaypointYRot
 *   0x0019fb88 edobjcbChangeWaypointZRot
 *   0x0019fbf0 edobjcbChangeWaypointTime
 *   0x0019fc70 edobjcbSetInstanceType
 *   0x0019fcb8 edobjcbSetSwitchType
 *   0x0019fd18 edobjcbSetSwitchId
 *   0x0019fd50 edobjcbSetSwitchDelay
 *   0x0019fd80 edobjcbSetSwitchVar
 *   0x0019fdb0 edobjcbCancelInstanceMenu
 *   0x0019fdd8 edobjcbCancelSwitchTypeMenu
 *   0x0019fe00 edobjcbCancelSwitchMenu
 *   0x0019fe28 edobjcbCancelAnimPropertiesMenu
 *   0x0019fe50 edobjcbCancelWayPropertiesMenu
 *   0x0019fe78 edobjcbSetBouncyPlayerGrav
 *   0x0019feb8 edobjcbSetBouncyTension
 *   0x0019fef8 edobjcbSetBouncyDamping
 *   0x0019ff38 edobjcbToggleSoundType
 *   0x0019fff0 edobjcbSetSoundTiming
 *   0x001a0020 edobjcbSetParticleType
 *   0x001a0070 edobjcbSetParticleRate
 *   0x001a00a8 edobjcbLocalSoundTypeMenu
 *   0x001a0220 edobjcbCancelLocalSoundMenu
 *   0x001a0248 edobjcbCancelParticleTypeMenu
 *   0x001a0270 edobjcbLocalParticleTypeMenu
 *   0x001a03e8 edobjcbCancelLocalParticleMenu
 *   0x001a0410 edobjcbParticleMenu
 *   0x001a0530 edobjcbSoundMenu
 *   0x001a0650 edobjcbCancelBouncyMenu
 *   0x001a0678 edobjClose
 *   0x001a0698 edobjEnter
 *   0x001a06c8 edobjProc
 *   0x001a07c0 edobjcbSetLocalSoundType
 *   0x001a0858 edobjcbSetLocalParticleType
 *   0x001a08f8 edobjcbSoundTypeMenu
 *   0x001a0a78 edobjcbCancelLocalSoundTypeMenu
 *   0x001a0aa0 edobjcbCancelLocalParticleTypeMenu
 *   0x001a0ac8 edobjcbCancelParticleMenu
 *   0x001a0af0 edobjcbCancelSoundMenu
 *   0x001a0b18 edobjcbSetSoundType
 *   0x001a0b78 edobjcbCancelSoundTypeMenu
 */

#include "nu_libc.h"

#include "numath/numathtypes.h"
#include "numath/numtx.h"
#include "numath/nuvec.h"
#include "nu3d/numtl.h"
#include "nu3d/nugobj.h"
#include "nu3d/nugscn.h"

struct camera_s {
    struct numtx_s mtx;
};

struct nuanimkey_s {
    float time;
    float dtime;
    float c;
    float d;
};

struct nuanimcurve_s {
    unsigned int mask;
    struct nuanimkey_s *animkeys;
    int numkeys;
    unsigned int flags;
};

struct nuanimcurveset_s {
    int flags;
    float *constants;
    struct nuanimcurve_s **set;
    signed char ncurves;
};

struct nuanimdatachunk_s {
    int numnodes;
    int num_valid_animcurvesets;
    struct nuanimcurveset_s **animcurvesets;
};

struct nuanimdata_s {
    float time;
    char *node_names;
    int nchunks;
    struct nuanimdatachunk_s **chunks;
};

struct nuanimtime_s {
    float time;
    float time_offset;
    int chunk;
    unsigned int time_mask;
    unsigned int time_byte;
};

struct pad_s {
    char pad000[0x55C];
    unsigned int paddata;
    char pad560[0x564 - 0x560];
    unsigned int buttons;
    char pad568[0x580 - 0x568];
    unsigned char rotleft;
    unsigned char rotright;
    unsigned char rotup;
    unsigned char rotdown;
};

struct eduimenu_s {
    void *head;
    void *tail;
    void *current;
    void *cursor;
    int x;
    int y;
    int w;
    int h;
};

struct eduistyle_s {
    unsigned int colour[4];
};

struct edobjitem_s {
    char pad00[0x0C];
    int value;
    unsigned char toggle;
    char pad11[0x4C - 0x11];
    float fvalue;
};

struct edobject_s {
    int instance;
    float anim_speed;
    int oscillate;
    int repeating;
    float anim_pause;
    int num_waypoints;
    int total_time;
    int num_particles;
    int num_sounds;
    int anim_start_offset;
    int platinst;
    struct nuvec_s waypoints[8];
    float waypoint_speed[8];
    struct nuvec_s waypoint_rot[8];
    int waypoint_time[8];
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
    float prev_time;
    char sound_name[8][16];
    int sound_id[8];
    int sound_type[8];
    float sound_timing[8];
    struct nuvec_s sound_pos[8];
    float bouncy_player_grav;
    float bouncy_tension;
    float bouncy_damping;
};

struct debtypedef_s {
    char name[0x10];
    char pad10[0x20 - 0x10];
    signed char gensort;
    char pad21[0x50 - 0x21];
    struct nuvec_s varstart;
};

struct edobjinstance_s {
    struct numtx_s mtx;
    int id;
    struct nuinstflags_s flags;
    struct nuinstanim_s *anim;
    char pad4c[0x50 - 0x4C];
};

extern struct edobject_s ObjectPath[];
extern struct edobjinstance_s ObjectInstance[];
extern struct nuanimdata_s *ObjectAnim[];
extern struct nuinstanim_s ObjectInstAnim[];

extern struct nugscn_s *edobj_base_scene;
extern void *edobj_active_menu;
extern struct eduimenu_s *edobj_instance_menu;
extern struct eduimenu_s *edobj_switchtype_menu;
extern struct eduimenu_s *edobj_switch_menu;
extern struct eduimenu_s *edobj_animprop_menu;
extern struct eduimenu_s *edobj_wayprop_menu;
extern struct eduimenu_s *edobj_localsound_menu;
extern struct eduimenu_s *edobj_localsoundtype_menu;
extern struct eduimenu_s *edobj_particletype_menu;
extern struct eduimenu_s *edobj_localparticle_menu;
extern struct eduimenu_s *edobj_bouncy_menu;
extern struct eduimenu_s *edobj_options_menu;
extern int edobj_nearest;
extern int edobj_nearest_sound;
extern int edobj_waypoint_mode;
extern int edobj_copy_mode;
extern int edobj_particle_mode;
extern int edobj_sound_mode;
extern int edobj_particle_type;
extern int edobj_sound_type;
extern int edobj_snap_xz;
extern int edobj_snap_y;
extern int edobj_nearest_particle;
extern int edobj_next_instance;
extern struct eduimenu_s *edobj_particle_menu;
extern struct eduimenu_s *edobj_localparticletype_menu;
extern struct eduimenu_s *edobj_sound_menu;
extern struct eduimenu_s *edobj_soundtype_menu;
extern int edobj_nearest_waypoint;
extern int edobj_instance_type;
extern struct nuvec_s edobj_snap_vec;
extern struct nuvec_s edobj_cam_pos;
extern int edobj_cam_ax;
extern int edobj_cam_ay;
extern int edobj_emitrotz;
extern int edobj_emitroty;
extern struct camera_s global_camera;
extern struct debtypedef_s *debtab[];
extern float D_0062CF04;
extern float D_0062CF24;
extern float D_0062CF28;
extern float D_0062CF2C;
extern int D_0062F57C;
extern float D_0062F588;
extern float D_0062F58C[];

#define edobj_blink_timer D_0062F57C
#define edobj_time D_0062F588
#define edobj_curve_unused D_0062F58C[0]

#define OBJECT_CURVESET(obj) (ObjectAnim[obj]->chunks[0]->animcurvesets[0])
#define edobj_default_anim_speed D_0062CF04

extern signed char edbits_what_game;
extern char edbits_level_filename[];
extern struct numtl_s *edobj_mtl;
extern struct numtl_s *edobj_mtl_zoff;
extern struct numtx_s edobj_cursor_matrix;
extern char D_00618D80[];
extern char D_00618D90[];
extern char D_00618DA0[];
extern char D_00618DB0[];
extern char D_00618DC0[];
extern char D_0062F598[];
extern char D_00618C78[];
extern char D_00618C88[];
extern char D_00618CA0[];
extern char D_00618CB0[];
extern char D_00618CC8[];
extern char D_00618CD8[];
extern char D_00618CF0[];
extern char D_00618D08[];
extern char D_00618D18[];
extern char D_00618D30[];
extern char D_00618D48[];
extern char D_00618D58[];
extern char D_00618D70[];
extern char D_0062F590[];
extern struct eduistyle_s D_00618DD0;
extern char D_006190B0[];
extern char D_006190C8[];
extern char D_006190E0[];
extern char D_00619100[];
extern char D_00619110[];
extern char D_00619120[];
extern void *app_fnt;

#define edobj_objects_dat_fmt D_00618D80
#define edobj_save_ok_msg D_00618D90
#define edobj_save_fail_msg D_00618DA0
#define edobj_load_ok_msg D_00618DB0
#define edobj_load_fail_msg D_00618DC0
#define edobj_obj_file_fmt D_0062F598
#define info_box_title D_00618C78
#define info_inst_none D_00618C88
#define info_inst_fmt D_00618CA0
#define info_highlight_none D_00618CB0
#define info_locked_fmt D_00618CC8
#define info_waypoints_fmt D_00618CD8
#define info_hilite_wp_none D_00618CF0
#define info_hilite_wp_fmt D_00618D08
#define info_particles_fmt D_00618D18
#define info_type_none D_00618D30
#define info_type_fmt D_00618D48
#define info_sounds_fmt D_00618D58
#define info_highlight_fmt D_00618D70
#define info_coord_fmt D_0062F590
#define edobj_item_style D_00618DD0
#define particle_menu_title D_006190B0
#define particle_menu_text_type D_006190C8
#define particle_menu_text_local D_006190E0
#define sound_menu_title D_00619100
#define sound_menu_text_type D_00619110
#define sound_menu_text_local D_00619120

extern char D_00618DE0[];
extern char D_0062F5A0[];
extern char D_00618DF0[];
extern char D_0062F5A8[];
extern char D_00618E00[];
extern char D_00618E18[];
extern char D_00618E30[];
extern char D_00618E40[];
extern char D_00618E58[];
extern char D_0062F5B0[];
extern char D_00618E70[];
extern char D_00618E88[];
extern char D_00618EA0[];
extern char D_00618EB0[];
extern char D_00618EC0[];
extern char D_00618ED0[];
extern char D_00618EE0[];
extern char D_00618EF0[];
extern char D_00618F00[];
extern char D_00618F10[];
extern char D_00618F20[];
extern char D_00618F30[];
extern char D_00618F40[];
extern char D_00618F50[];
extern char D_0062F5B8[];
extern char D_0062F5C0[];
extern char D_0062F5C8[];
extern char D_0062F5D0[];
extern char D_00618F78[];
extern char D_00618F68[];
extern char D_0062F5F0[];
extern char D_00619160[];
extern char D_00619178[];
extern char D_00619190[];
extern char D_006191A8[];
extern char D_006191C0[];
extern char D_006191D0[];
extern char D_006191E0[];
extern char D_006191F0[];
extern char D_0062F5F8[];
extern char D_0062F600[];
extern char D_00618FB0[];
extern char D_00618FC8[];
extern char D_00618FE8[];
extern char D_00618FF8[];
extern char D_00619008[];
extern char D_00619020[];
extern char D_0062F5D8[];
extern char D_00619050[];
extern char D_00619070[];
extern char D_00619090[];
extern char D_006190A0[];
extern char D_00619140[];
extern char D_00619150[];
extern char D_0062F5E0[];
extern char D_0062F5E8[];
extern char D_00618F88[];
extern char D_00618F98[];
extern char D_00619030[];

#define instance_menu_title D_00618DE0
#define menu_text_none D_0062F5A0
#define switchtype_menu_title D_00618DF0
#define switchtype_text_switch D_0062F5A8
#define switchtype_text_switch_once D_00618E00
#define switchtype_text_switch_cont D_00618E18
#define switchtype_text_proximity D_00618E30
#define switchtype_text_proximity_once D_00618E40
#define switchtype_text_proximity_cont D_00618E58
#define switchtype_text_terrain D_0062F5B0
#define switchtype_text_terrain_once D_00618E70
#define switchtype_text_terrain_cont D_00618E88
#define switch_menu_title D_00618EA0
#define switch_menu_text_type D_00618EB0
#define switch_menu_text_id D_00618EC0
#define switch_menu_text_delay D_00618ED0
#define switch_menu_text_var D_00618EE0
#define animprop_menu_title D_00618EF0
#define animprop_menu_text_speed D_00618F00
#define animprop_menu_text_pause D_00618F10
#define animprop_menu_text_start_offset D_00618F20
#define animprop_menu_text_switch D_00618F30
#define animprop_menu_text_oscillate D_00618F40
#define wayprop_menu_title D_00618F50
#define wayprop_menu_text_speed D_0062F5B8
#define wayprop_menu_text_xrot D_0062F5C0
#define wayprop_menu_text_yrot D_0062F5C8
#define wayprop_menu_text_zrot D_0062F5D0
#define wayprop_menu_text_time D_00618F78
#define slider_fmt_1dp D_00618F68
#define options_menu_title D_0062F5F0
#define options_text_animprop D_00619160
#define options_text_wayprop D_00619178
#define options_text_particles D_00619190
#define options_text_sounds D_006191A8
#define options_text_bouncy D_006191C0
#define options_text_save D_006191D0
#define options_text_load D_006191E0
#define options_text_reset_anims D_006191F0
#define options_text_snap_y D_0062F5F8
#define options_text_snap_xz D_0062F600
#define localsound_menu_title D_00618FB0
#define localsound_menu_text_type D_00618FC8
#define localsound_menu_text_repeating D_00618FE8
#define localsound_menu_text_repeat_every D_00618FF8
#define localsound_menu_text_trigger_time D_00619008
#define particletype_menu_title D_00619020
#define particletype_text_none D_0062F5D8
#define localparticle_menu_title D_00619050
#define localparticle_menu_text_type D_00619070
#define localparticle_menu_text_rate D_00619090
#define localparticle_menu_text_moving D_006190A0
#define bouncy_menu_title D_00619140
#define bouncy_menu_text_grav D_00619150
#define bouncy_menu_text_tension D_0062F5E0
#define bouncy_menu_text_damping D_0062F5E8
#define soundtype_menu_title D_00618F88
#define localsoundtype_menu_title D_00618F98
#define localparticletype_menu_title D_00619030

extern void edbitsRegisterLevel(char *filename, int whatgame);
extern void eduiMenuDestroy(void *menu);
extern void eduiMenuDetach(void *menu);
extern void edobjObjectDestroy(int instance);
extern void edobjConvertPathToAnim(int instance);
extern void edcamSetPos(struct nuvec_s *pos);
extern void edcamMove(struct pad_s *pad);
extern void edcamGetPosAng(struct nuvec_s *pos, int *ax, int *ay);
extern struct nuvec_s *edmainQueryLocVec(void);
extern float NuFsqrt(float x);
extern char *edbitsGetSoundName(int id);
extern void eduiMenuProcess(void *menu, struct pad_s *pad);
extern void edobjDoInput(struct pad_s *pad);
extern void edobjDetermineNearestObject(float range);
extern void edobjDetermineNearestWaypoint(float range);
extern void edobjDetermineNearestParticle(float range);
extern void edobjDetermineNearestSound(float range);
extern void NuAnimDataDestroy(struct nuanimdata_s *animdata);
extern struct nuanimdata_s *NuAnimDataCreate(int nchunks);
extern struct nuanimdatachunk_s *NuAnimDataChunkCreate(int numnodes);
extern struct nuanimcurveset_s *NuAnimCurveSetCreate(int ncurves);
extern struct nuanimcurve_s *NuAnimCurveCreate(int numkeys);
extern void NuAnimCurveDestroy(struct nuanimcurve_s *curve);
extern int NewPlatInst(struct numtx_s *mtx, int instance);
extern void UpdatePlatinst(int platinst, struct numtx_s *mtx);
extern void PlatInstRotate(int platinst, int rotate);
extern void PlatInstBounce(int platinst, float player_grav, float tension, float damping);
extern int DeletePlatinst(int id);
extern int NuFileSize(char *name);
extern void edobjObjectReset(void);
extern int edobjFileSaveObjects(char *filename);
extern int edobjFileLoadObjects(char *filename);
extern void eduiCreateMessageMenu(void *menu, char *msg, int flag);
extern void edcamSet(void);
extern void edobjDrawCursor(void);
extern void edbitsDrawCube(float x, float y, float z, float sizex, float sizey, float sizez,
                           int rotx, int roty, int rotz, int flags, int colour, void *mtl);
extern void edbitsDrawDiagonalCross(float x, float y, float z, float size, int colour, void *mtl);
extern void edobjRenderWaypoints(int obj);
extern void edobjRenderParticleEmitters(int obj);
extern void edobjRenderSoundEmitters(int obj);
extern void NuFntScale(int x, int y);
extern void NuFntSet(void *font);
extern void NuFntSetPen(int colour);
extern void NuFntPrintEx(int x, int y, int flags, char *fmt, ...);
extern int NuRndrLine3d(struct nuvtx_tc1_s *line, void *mtl, struct numtx_s *mtx);
extern void NuRndrRect2di(int x, int y, int w, int h, int colour, void *mtl);
extern void NuRndrLine2di(int x0, int y0, int x1, int y1, int colour, void *mtl);
extern void eduiMenuRender(void *menu);
extern int NuRndrGScnObj(struct nugobj_s *gobj, struct numtx_s *mtx);
extern unsigned char object_switches[];
extern struct numtx_s numtx_identity;
extern int PlatImpactInfo(struct nuvec_s *norm, int *info0, int *info1);
extern void NuAnimDataCalcTime(struct nuanimdata_s *animdata, float time,
                               struct nuanimtime_s *atime);
extern void NuAnimCurveSetApplyToMatrix(struct nuanimcurveset_s *set, struct nuanimtime_s *atime,
                                        struct numtx_s *m);
extern void AddVariableShotDebrisEffectMtx2(int effect, struct nuvec_s *pos, int n,
                                            struct numtx_s *m1, struct numtx_s *m2);
extern int LookupDebrisEffect(char *name);
extern int edbitsLookupSoundFX(char *name);
extern void edbitsSoundPlay(struct nuvec_s *pos, int id);
extern void edobjParticleDestroy(int obj, int slot);
extern void edobjSoundDestroy(int obj, int slot);
extern struct eduimenu_s *eduiMenuCreate(int x, int y, int w, int h, void *font,
                                         void *cancel, char *title);
extern void *eduiItemSelCreate(int value, struct eduistyle_s *style, int on,
                               int group, void *select, char *text);
extern void *eduiMenuAddItem(struct eduimenu_s *menu, void *item);
extern int eduiMenuAttach(struct eduimenu_s *menu, struct eduimenu_s *child);
extern void edobjcbParticleTypeMenu(struct eduimenu_s *menu);
extern void edobjcbLocalParticleMenu(struct eduimenu_s *menu);
extern void edobjcbSoundTypeMenu(struct eduimenu_s *menu);
extern void edobjcbLocalSoundMenu(struct eduimenu_s *menu);
extern void edobjcbCancelParticleMenu(void);
extern void edobjcbCancelSoundMenu(void);
extern void *eduiItemCheckCreate(unsigned int value, struct eduistyle_s *style, int on,
                                 int group, void *select, char *text);
extern void *eduiItemToggleCreate(int value, struct eduistyle_s *style, int on, int group,
                                  void *toggle, char *text);
extern void *eduiItemSliderCreate(int value, struct eduistyle_s *style, int group, void *change,
                                  float min, float max, float val, char *text);
extern void *eduiItemSliderCreateInt(int value, struct eduistyle_s *style, int group,
                                     void *change, int min, int max, int val, char *text);
extern void eduiItemSliderSetFmt(void *item, char *fmt);
extern void eduiItemSliderSetGranularity(void *item, float granularity);
extern struct edobjitem_s *edui_last_item;
extern void edobjcbCancelInstanceMenu(void);
extern void edobjcbSetInstanceType(void *menu, struct edobjitem_s *item);
extern void edobjcbSwitchTypeMenu(struct eduimenu_s *menu);
extern void edobjcbSetSwitchType(void *menu, struct edobjitem_s *item);
extern void edobjcbCancelSwitchTypeMenu(void);
extern void edobjcbCancelSwitchMenu(void);
extern void edobjcbSetSwitchId(void *menu, struct edobjitem_s *item);
extern void edobjcbSetSwitchDelay(void *menu, struct edobjitem_s *item);
extern void edobjcbSetSwitchVar(void *menu, struct edobjitem_s *item);
extern void edobjcbCancelAnimPropertiesMenu(void);
extern void edobjcbChangeAnimSpeed(void *menu, struct edobjitem_s *item);
extern void edobjcbChangeAnimPause(void *menu, struct edobjitem_s *item);
extern void edobjcbChangeAnimStartOffset(void *menu, struct edobjitem_s *item);
extern void edobjcbOscillateToggle(void *menu, struct edobjitem_s *item);
extern void edobjcbCancelWayPropertiesMenu(void);
extern void edobjcbChangeWaypointSpeed(void *menu, struct edobjitem_s *item);
extern void edobjcbChangeWaypointXRot(void *menu, struct edobjitem_s *item);
extern void edobjcbChangeWaypointYRot(void *menu, struct edobjitem_s *item);
extern void edobjcbChangeWaypointZRot(void *menu, struct edobjitem_s *item);
extern void edobjcbChangeWaypointTime(void *menu, struct edobjitem_s *item);
extern void edobjcbCancelLocalSoundMenu(void);
extern void edobjcbLocalSoundTypeMenu(struct eduimenu_s *menu);
extern void edobjcbToggleSoundType(void *menu, struct edobjitem_s *item);
extern void edobjcbSetSoundTiming(void *menu, struct edobjitem_s *item);
extern void edobjcbCancelParticleTypeMenu(void);
extern void edobjcbSetParticleType(void *menu, struct edobjitem_s *item);
extern void edobjcbCancelLocalParticleMenu(void);
extern void edobjcbLocalParticleTypeMenu(struct eduimenu_s *menu);
extern void edobjcbSetParticleRate(void *menu, struct edobjitem_s *item);
extern void edobjcbToggleParticleSwitch(void *menu, struct edobjitem_s *item);
extern void edobjcbCancelBouncyMenu(void);
extern void edobjcbSetBouncyPlayerGrav(void *menu, struct edobjitem_s *item);
extern void edobjcbSetBouncyTension(void *menu, struct edobjitem_s *item);
extern void edobjcbSetBouncyDamping(void *menu, struct edobjitem_s *item);
extern void EdFileSetMedia(int media);
extern int EdFileOpen(char *name, int mode);
extern int EdFileClose(void);
extern void EdFileWrite(void *data, int size);
extern void EdFileWriteInt(int data);
extern void EdFileWriteFloat(float data);
extern void EdFileWriteShort(short data);
extern void EdFileRead(void *data, int size);
extern int EdFileReadInt(void);
extern float EdFileReadFloat(void);
extern short EdFileReadShort(void);
extern int edobjLookupInstance(char *name);
extern void edobjcbCancelOptMenu(void);
extern void edobjcbParticleMenu(struct eduimenu_s *menu);
extern void edobjcbSoundMenu(struct eduimenu_s *menu);
extern void edobjcbFileSaveObjects(void *menu);
extern void edobjcbFileLoadObjects(void *menu);
extern void edobjcbResetAnims(void *menu, struct edobjitem_s *item);
extern void edobjcbSnapYToggle(void *menu, struct edobjitem_s *item);
extern void edobjcbSnapXZToggle(void *menu, struct edobjitem_s *item);
extern int edSfxAllCount;
extern void edobjcbSetLocalSoundType(void *menu, struct edobjitem_s *item);
extern void edobjcbSetLocalParticleType(void *menu, struct edobjitem_s *item);
extern void edobjcbSetSoundType(void *menu, struct edobjitem_s *item);
extern void edobjcbCancelLocalSoundTypeMenu(void);
extern void edobjcbCancelLocalParticleTypeMenu(void);
extern void edobjcbCancelSoundTypeMenu(void);


inline float edobjPlayerObjectDistance(int obj)
{
    if (edmainQueryLocVec() != 0) {
        return NuVecDist((struct nuvec_s *)&ObjectInstance[obj].mtx._30, edmainQueryLocVec(), 0);
    }
    return 0.0f;
}


void edobjUpdateObjects(float dt)
{
    int i;
    struct edobjinstance_s *inst;
    int platinst;
    struct nuvec_s impact_norm;
    struct nuinstanim_s *anim;
    struct nuanimdata_s *animdata;
    float t;
    int impact_info0;
    int impact_info1;
    int sound;
    int interval;
    int j;
    int k;

    platinst = PlatImpactInfo(&impact_norm, &impact_info0, &impact_info1);
    for (i = 0; i < 64; i++) {
        t = 1.0f;
        inst = &ObjectInstance[i];
        if (inst->id == -1) {
            continue;
        }
        anim = inst->anim;
        NuMtxSetIdentity(&inst->mtx);
        inst->mtx._30 = ObjectPath[i].waypoints[0].x;
        inst->mtx._31 = ObjectPath[i].waypoints[0].y;
        inst->mtx._32 = ObjectPath[i].waypoints[0].z;
        if (anim != 0 && ObjectAnim[anim->anim_ix] != 0) {
            switch (ObjectPath[i].switch_type) {
            case 0:
                anim->playing = 1;
                break;
            case 1:
                anim->playing = 0;
                if (ObjectPath[i].switch_id != -1 && object_switches[ObjectPath[i].switch_id]) {
                    anim->playing = 1;
                }
                break;
            case 2:
                anim->repeating = 0;
                if (ObjectPath[i].switch_id != -1 && object_switches[ObjectPath[i].switch_id]) {
                    if (!anim->playing) {
                        anim->playing = 1;
                        anim->waiting = 0;
                        anim->ltime = 1.0f;
                        anim->backwards = 0;
                    }
                }
                break;
            case 3:
                anim->repeating = 1;
                if (ObjectPath[i].switch_id != -1 && object_switches[ObjectPath[i].switch_id]) {
                    anim->playing = 1;
                }
                break;
            case 4:
                if (edobjPlayerObjectDistance(i) < ObjectPath[i].switch_var) {
                    anim->playing = 1;
                } else {
                    anim->playing = 0;
                }
                break;
            case 5:
                anim->repeating = 0;
                if (edobjPlayerObjectDistance(i) < ObjectPath[i].switch_var) {
                    if (!anim->playing) {
                        anim->playing = 1;
                        anim->waiting = 0;
                        anim->ltime = 1.0f;
                        anim->backwards = 0;
                    }
                }
                break;
            case 6:
                anim->repeating = 1;
                if (edobjPlayerObjectDistance(i) < ObjectPath[i].switch_var) {
                    anim->playing = 1;
                }
                break;
            case 7:
                if (ObjectPath[i].platinst == platinst) {
                    ObjectPath[i].switch_var = 10.0f;
                }
                if (ObjectPath[i].switch_var > 0.0f) {
                    ObjectPath[i].switch_var -= dt;
                    anim->playing = 1;
                } else {
                    anim->playing = 0;
                }
                break;
            case 8:
                anim->repeating = 0;
                if (ObjectPath[i].platinst == platinst) {
                    if (!anim->playing) {
                        anim->playing = 1;
                        anim->ltime = 1.0f;
                        anim->waiting = 0;
                        if (ObjectPath[i].switch_delay > 0.0f) {
                            anim->tfirst = ObjectPath[i].switch_delay + 1.0f;
                            anim->waiting = 1;
                        }
                        anim->backwards = 0;
                    }
                }
                break;
            case 9:
                anim->repeating = 1;
                if (ObjectPath[i].platinst == platinst) {
                    anim->playing = 1;
                }
                break;
            }
            animdata = ObjectAnim[anim->anim_ix];
            if (animdata != 0 && anim->playing) {
                anim->ltime += dt * anim->tfactor;
                if (anim->waiting) {
                    if (anim->tfirst <= anim->ltime) {
                        anim->waiting = 0;
                        anim->ltime -= anim->tfirst - 1.0f;
                    }
                }
                if (!anim->waiting) {
                    if (animdata->time + anim->tinterval <= anim->ltime) {
                        if (anim->repeating) {
                            if (anim->oscillate) {
                                anim->backwards = !anim->backwards;
                            }
                            anim->ltime = 1.0f;
                        } else if (!anim->oscillate || anim->backwards) {
                            anim->playing = 0;
                            if (ObjectPath[i].switch_id != -1) {
                                object_switches[ObjectPath[i].switch_id] = 0;
                            }
                        } else {
                            anim->backwards = !anim->backwards;
                            anim->ltime = 1.0f;
                        }
                    }
                    if (anim->ltime > animdata->time) {
                        t = animdata->time;
                    } else {
                        t = anim->ltime;
                    }
                }
            } else {
                t = anim->ltime;
            }
            if (anim->backwards) {
                t = (animdata->time + 1.0f) - t;
            }
            {
                struct nuanimtime_s atime;
                struct numtx_s animmtx;

                NuAnimDataCalcTime(animdata, t, &atime);
                NuAnimCurveSetApplyToMatrix(animdata->chunks[atime.chunk]->animcurvesets[0], &atime,
                                            &animmtx);
                anim->mtx = animmtx;
                NuMtxTranslate(&anim->mtx, (struct nuvec_s *)&inst->mtx._30);
            }
        }
        for (j = 0; j < (int)dt; j++) {
            for (k = 0; k < ObjectPath[i].num_particles; k++) {
                if (ObjectPath[i].particle_type[k] != -1) {
                    struct nuvec_s pos;
                    struct numtx_s emitmtx;
                    struct numtx_s rotmtx;
                    struct numtx_s *mtx;

                    if (ObjectPath[i].particle_switch[k] != 0 && anim == 0) {
                        continue;
                    }
                    if (ObjectPath[i].particle_switch[k] != 0 && !anim->playing) {
                        continue;
                    }
                    if (ObjectPath[i].particle_switch[k] != 0
                        && ObjectAnim[anim->anim_ix]->time <= anim->ltime) {
                        continue;
                    }
                    mtx = &inst->mtx;
                    if (anim != 0) {
                        mtx = &anim->mtx;
                    }
                    if ((edobj_particle_mode || edobj_sound_mode) && edobj_nearest == i) {
                        mtx = &inst->mtx;
                    }
                    NuVecMtxTransform(&pos, &ObjectPath[i].particle_pos[k], mtx);
                    rotmtx = numtx_identity;
                    NuMtxRotateZ(&rotmtx, ObjectPath[i].particle_rotz[k]);
                    NuMtxRotateY(&rotmtx, ObjectPath[i].particle_roty[k]);
                    NuMtxMul(&emitmtx, &rotmtx, mtx);
                    if (ObjectPath[i].particle_rate[k] > 0) {
                        AddVariableShotDebrisEffectMtx2(ObjectPath[i].particle_type[k], &pos,
                                                        ObjectPath[i].particle_rate[k],
                                                        &emitmtx, &numtx_identity);
                    } else if (ObjectPath[i].particle_rate[k] < 0) {
                        if ((int)(edobj_time + j) % abs(ObjectPath[i].particle_rate[k]) == 0) {
                            AddVariableShotDebrisEffectMtx2(ObjectPath[i].particle_type[k],
                                                            &pos, 1, &emitmtx,
                                                            &numtx_identity);
                        }
                    }
                } else if (ObjectPath[i].particle_name[k][0] != 0) {
                    ObjectPath[i].particle_type[k] =
                        LookupDebrisEffect(ObjectPath[i].particle_name[k]);
                    if (ObjectPath[i].particle_type[k] == -1) {
                        edobjParticleDestroy(i, k);
                        k--;
                    }
                }
            }
        }
        for (j = 0; j < ObjectPath[i].num_sounds; j++) {
            if (ObjectPath[i].sound_id[j] != -1) {
                sound = -1;
                if (ObjectPath[i].sound_type[j] == 1) {
                    interval = ObjectPath[i].sound_timing[j];
                    if ((int)edobj_time / interval < (int)(edobj_time + dt) / interval) {
                        sound = ObjectPath[i].sound_id[j];
                    }
                } else {
                    if (ObjectPath[i].sound_timing[j] <= t
                        && ObjectPath[i].prev_time < ObjectPath[i].sound_timing[j]) {
                        sound = ObjectPath[i].sound_id[j];
                    }
                    if (ObjectPath[i].oscillate && t <= ObjectPath[i].sound_timing[j]
                        && ObjectPath[i].sound_timing[j] < abs((int)ObjectPath[i].prev_time)) {
                        sound = ObjectPath[i].sound_id[j];
                    }
                }
                if (sound != -1) {
                    struct nuvec_s pos;
                    struct numtx_s *mtx;

                    mtx = anim != 0 ? &anim->mtx : &inst->mtx;
                    if ((edobj_particle_mode || edobj_sound_mode) && edobj_nearest == i) {
                        mtx = &inst->mtx;
                    }
                    NuVecMtxTransform(&pos, &ObjectPath[i].sound_pos[j], mtx);
                    edbitsSoundPlay(&pos, sound);
                }
            } else if (ObjectPath[i].sound_name[j][0] != 0) {
                ObjectPath[i].sound_id[j] = edbitsLookupSoundFX(ObjectPath[i].sound_name[j]);
                if (ObjectPath[i].sound_id[j] == -1) {
                    edobjSoundDestroy(i, j);
                    j--;
                }
            }
        }
        if (ObjectPath[i].oscillate) {
            ObjectPath[i].prev_time = -t;
        } else {
            ObjectPath[i].prev_time = t;
        }
    }
    edobj_time += dt;
}


inline int edobjRenderCutoffTest(struct nuvec_s *pos)
{
    float dx;
    float dy;
    float dz;

    dx = pos->x - global_camera.mtx._30;
    dy = pos->y - global_camera.mtx._31;
    dz = pos->z - global_camera.mtx._32;
    if (NuFsqrt(dx * dx + dy * dy + dz * dz) > 40.0f) {
        return 0;
    }
    return 1;
}


void edobjRenderObjects(struct nugscn_s *scene)
{
    struct edobjinstance_s *inst;
    struct nuinstanim_s *anim;
    int i;

    for (i = 0; i < 64; i++) {
        inst = &ObjectInstance[i];
        if (inst->id != -1) {
            anim = inst->anim;
            if ((edobj_particle_mode || edobj_sound_mode) && edobj_nearest == i) {
                inst->flags.onscreen = NuRndrGScnObj(scene->gobjs[inst->id], &inst->mtx);
            } else if (anim != 0) {
                if (edobjRenderCutoffTest((struct nuvec_s *)&anim->mtx._30)) {
                    inst->flags.onscreen = NuRndrGScnObj(scene->gobjs[inst->id], &anim->mtx);
                }
            } else if (edobjRenderCutoffTest((struct nuvec_s *)&inst->mtx._30)) {
                inst->flags.onscreen = NuRndrGScnObj(scene->gobjs[inst->id], &inst->mtx);
            }
        }
    }
}


void edobjDetermineNearestObject(float range)
{
    struct nuvec_s delta;
    float dist;
    int i;

    if (edobj_nearest != -1) {
        NuVecSub(&delta, &edobj_cam_pos, (struct nuvec_s *)&ObjectInstance[edobj_nearest].mtx._30);
        if (delta.x * delta.x + delta.y * delta.y + delta.z * delta.z == 0.0) {
            return;
        }
    }
    edobj_nearest = -1;
    for (i = 0; i < 64; i++) {
        if (ObjectInstance[i].id != -1) {
            NuVecSub(&delta, &edobj_cam_pos, (struct nuvec_s *)&ObjectInstance[i].mtx._30);
            dist = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
            if (range < 0.0f || dist < range) {
                edobj_nearest = i;
                range = dist;
            }
        }
    }
}


void edobjDetermineNearestWaypoint(float range)
{
    struct nuvec_s delta;
    float dist;
    int i;

    if (edobj_nearest == -1) {
        edobj_nearest_waypoint = -1;
        return;
    }
    if (edobj_nearest_waypoint != -1) {
        NuVecSub(&delta, &edobj_cam_pos,
                 &ObjectPath[edobj_nearest].waypoints[edobj_nearest_waypoint]);
        if (delta.x * delta.x + delta.y * delta.y + delta.z * delta.z == 0.0) {
            return;
        }
    }
    edobj_nearest_waypoint = -1;
    for (i = 0; i < ObjectPath[edobj_nearest].num_waypoints; i++) {
        NuVecSub(&delta, &edobj_cam_pos, &ObjectPath[edobj_nearest].waypoints[i]);
        dist = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
        if (range < 0.0f || dist < range) {
            edobj_nearest_waypoint = i;
            range = dist;
        }
    }
}


void edobjDetermineNearestParticle(float range)
{
    struct nuvec_s delta;
    struct nuvec_s pos;
    float dist;
    int i;

    if (edobj_nearest == -1) {
        edobj_nearest_particle = -1;
        return;
    }
    if (edobj_nearest_particle != -1) {
        NuVecAdd(&pos, &ObjectPath[edobj_nearest].waypoints[0],
                 &ObjectPath[edobj_nearest].particle_pos[edobj_nearest_particle]);
        NuVecSub(&delta, &edobj_cam_pos, &pos);
        if (delta.x * delta.x + delta.y * delta.y + delta.z * delta.z == 0.0) {
            return;
        }
    }
    edobj_nearest_particle = -1;
    for (i = 0; i < ObjectPath[edobj_nearest].num_particles; i++) {
        NuVecAdd(&pos, &ObjectPath[edobj_nearest].waypoints[0],
                 &ObjectPath[edobj_nearest].particle_pos[i]);
        NuVecSub(&delta, &edobj_cam_pos, &pos);
        dist = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
        if (range < 0.0f || dist < range) {
            edobj_nearest_particle = i;
            range = dist;
        }
    }
}


void edobjDetermineNearestSound(float range)
{
    struct nuvec_s delta;
    struct nuvec_s pos;
    float dist;
    int i;

    if (edobj_nearest == -1) {
        edobj_nearest_sound = -1;
        return;
    }
    if (edobj_nearest_sound != -1) {
        NuVecAdd(&pos, &ObjectPath[edobj_nearest].waypoints[0],
                 &ObjectPath[edobj_nearest].sound_pos[edobj_nearest_sound]);
        NuVecSub(&delta, &edobj_cam_pos, &pos);
        if (delta.x * delta.x + delta.y * delta.y + delta.z * delta.z == 0.0) {
            return;
        }
    }
    edobj_nearest_sound = -1;
    for (i = 0; i < ObjectPath[edobj_nearest].num_sounds; i++) {
        NuVecAdd(&pos, &ObjectPath[edobj_nearest].waypoints[0],
                 &ObjectPath[edobj_nearest].sound_pos[i]);
        NuVecSub(&delta, &edobj_cam_pos, &pos);
        dist = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
        if (range < 0.0f || dist < range) {
            edobj_nearest_sound = i;
            range = dist;
        }
    }
}


inline int edobjLookupInstanceIndex(int special)
{
    int i;

    if (edobj_base_scene == 0) {
        return -1;
    }
    for (i = 0; i < edobj_base_scene->numinstance; i++) {
        if (&edobj_base_scene->instances[i]
            == edobj_base_scene->specials[special].instance) {
            return i;
        }
    }
    return -1;
}


void edobjConvertPathToAnim(int obj)
{
    int rotating;
    unsigned int mask;
    struct nuanimkey_s *key;
    int num;
    int changed;
    int time;
    int i;
    int j;

    ObjectInstance[obj].anim = 0;
    NuMtxSetIdentity(&ObjectInstance[obj].mtx);
    ObjectInstance[obj].mtx._30 = ObjectPath[obj].waypoints[0].x;
    ObjectInstance[obj].mtx._31 = ObjectPath[obj].waypoints[0].y;
    ObjectInstance[obj].mtx._32 = ObjectPath[obj].waypoints[0].z;
    num = ObjectPath[obj].num_waypoints;
    if (ObjectPath[obj].platinst == -1) {
        ObjectPath[obj].platinst = NewPlatInst(&ObjectInstance[obj].mtx,
                                               edobjLookupInstanceIndex(ObjectPath[obj].instance));
    } else {
        UpdatePlatinst(ObjectPath[obj].platinst, &ObjectInstance[obj].mtx);
        PlatInstRotate(ObjectPath[obj].platinst, 0);
    }
    PlatInstBounce(ObjectPath[obj].platinst, ObjectPath[obj].bouncy_player_grav,
                   ObjectPath[obj].bouncy_tension, ObjectPath[obj].bouncy_damping);
    if (num < 2) {
        return;
    }
    if (ObjectAnim[obj] == 0) {
        ObjectAnim[obj] = NuAnimDataCreate(1);
        ObjectAnim[obj]->chunks[0] = NuAnimDataChunkCreate(1);
        ObjectAnim[obj]->nchunks = 1;
        ObjectAnim[obj]->chunks[0]->animcurvesets[0] = NuAnimCurveSetCreate(9);
        OBJECT_CURVESET(obj)->flags = 1;
    }
    ObjectAnim[obj]->time = 1.0f;
    time = 0;
    mask = 1;
    for (i = 0; i < num - 1; i++) {
        ObjectAnim[obj]->time += ObjectPath[obj].waypoint_time[i];
        time += ObjectPath[obj].waypoint_time[i];
        mask |= 1 << time;
    }

    changed = 0;
    for (i = 1; i < num; i++) {
        if (ObjectPath[obj].waypoints[i].x != ObjectPath[obj].waypoints[0].x) {
            changed = 1;
            break;
        }
    }
    if (changed) {
        OBJECT_CURVESET(obj)->constants[0] = edobj_curve_unused;
        if (OBJECT_CURVESET(obj)->set[0] == 0) {
            OBJECT_CURVESET(obj)->set[0] = NuAnimCurveCreate(num);
        }
        if (OBJECT_CURVESET(obj)->set[0]->numkeys != num) {
            NuAnimCurveDestroy(OBJECT_CURVESET(obj)->set[0]);
            OBJECT_CURVESET(obj)->set[0] = NuAnimCurveCreate(num);
        }
        OBJECT_CURVESET(obj)->set[0]->mask = mask;
        for (i = 0; i < num; i++) {
            key = &OBJECT_CURVESET(obj)->set[0]->animkeys[i];
            key->time = 1.0f;
            for (j = 0; j < i; j++) {
                key->time += ObjectPath[obj].waypoint_time[j];
            }
            if (i == num - 1) {
                key->dtime = 0.0f;
            } else {
                key->dtime = 1.0 / (float)ObjectPath[obj].waypoint_time[i];
            }
            if (i == 0) {
                key->c = (ObjectPath[obj].waypoints[i + 1].x - ObjectPath[obj].waypoints[i].x) * 0.5
                       * ObjectPath[obj].waypoint_speed[i];
            } else if (i == num - 1) {
                key->c = (ObjectPath[obj].waypoints[i].x - ObjectPath[obj].waypoints[i - 1].x) * 0.5
                       * ObjectPath[obj].waypoint_speed[i];
            } else {
                key->c = (ObjectPath[obj].waypoints[i + 1].x - ObjectPath[obj].waypoints[i - 1].x)
                       * ObjectPath[obj].waypoint_speed[i];
            }
            key->d = ObjectPath[obj].waypoints[i].x - ObjectPath[obj].waypoints[0].x;
        }
    } else {
        OBJECT_CURVESET(obj)->constants[0] = 0.0f;
        if (OBJECT_CURVESET(obj)->set[0] != 0) {
            NuAnimCurveDestroy(OBJECT_CURVESET(obj)->set[0]);
            OBJECT_CURVESET(obj)->set[0] = 0;
        }
    }

    changed = 0;
    for (i = 1; i < num; i++) {
        if (ObjectPath[obj].waypoints[i].y != ObjectPath[obj].waypoints[0].y) {
            changed = 1;
            break;
        }
    }
    if (changed) {
        OBJECT_CURVESET(obj)->constants[1] = edobj_curve_unused;
        if (OBJECT_CURVESET(obj)->set[1] == 0) {
            OBJECT_CURVESET(obj)->set[1] = NuAnimCurveCreate(num);
        }
        if (OBJECT_CURVESET(obj)->set[1]->numkeys != num) {
            NuAnimCurveDestroy(OBJECT_CURVESET(obj)->set[1]);
            OBJECT_CURVESET(obj)->set[1] = NuAnimCurveCreate(num);
        }
        OBJECT_CURVESET(obj)->set[1]->mask = mask;
        for (i = 0; i < num; i++) {
            key = &OBJECT_CURVESET(obj)->set[1]->animkeys[i];
            key->time = 1.0f;
            for (j = 0; j < i; j++) {
                key->time += ObjectPath[obj].waypoint_time[j];
            }
            if (i == num - 1) {
                key->dtime = 0.0f;
            } else {
                key->dtime = 1.0 / (float)ObjectPath[obj].waypoint_time[i];
            }
            if (i == 0) {
                key->c = (ObjectPath[obj].waypoints[i + 1].y - ObjectPath[obj].waypoints[i].y) * 0.5
                       * ObjectPath[obj].waypoint_speed[i];
            } else if (i == num - 1) {
                key->c = (ObjectPath[obj].waypoints[i].y - ObjectPath[obj].waypoints[i - 1].y) * 0.5
                       * ObjectPath[obj].waypoint_speed[i];
            } else {
                key->c = (ObjectPath[obj].waypoints[i + 1].y - ObjectPath[obj].waypoints[i - 1].y)
                       * ObjectPath[obj].waypoint_speed[i];
            }
            key->d = ObjectPath[obj].waypoints[i].y - ObjectPath[obj].waypoints[0].y;
        }
    } else {
        OBJECT_CURVESET(obj)->constants[1] = 0.0f;
        if (OBJECT_CURVESET(obj)->set[1] != 0) {
            NuAnimCurveDestroy(OBJECT_CURVESET(obj)->set[1]);
            OBJECT_CURVESET(obj)->set[1] = 0;
        }
    }

    changed = 0;
    for (i = 1; i < num; i++) {
        if (ObjectPath[obj].waypoints[i].z != ObjectPath[obj].waypoints[0].z) {
            changed = 1;
            break;
        }
    }
    if (changed) {
        OBJECT_CURVESET(obj)->constants[2] = edobj_curve_unused;
        if (OBJECT_CURVESET(obj)->set[2] == 0) {
            OBJECT_CURVESET(obj)->set[2] = NuAnimCurveCreate(num);
        }
        if (OBJECT_CURVESET(obj)->set[2]->numkeys != num) {
            NuAnimCurveDestroy(OBJECT_CURVESET(obj)->set[2]);
            OBJECT_CURVESET(obj)->set[2] = NuAnimCurveCreate(num);
        }
        OBJECT_CURVESET(obj)->set[2]->mask = mask;
        for (i = 0; i < num; i++) {
            key = &OBJECT_CURVESET(obj)->set[2]->animkeys[i];
            key->time = 1.0f;
            for (j = 0; j < i; j++) {
                key->time += ObjectPath[obj].waypoint_time[j];
            }
            if (i == num - 1) {
                key->dtime = 0.0f;
            } else {
                key->dtime = 1.0 / (float)ObjectPath[obj].waypoint_time[i];
            }
            if (i == 0) {
                key->c = (ObjectPath[obj].waypoints[i + 1].z - ObjectPath[obj].waypoints[i].z) * 0.5
                       * ObjectPath[obj].waypoint_speed[i];
            } else if (i == num - 1) {
                key->c = (ObjectPath[obj].waypoints[i].z - ObjectPath[obj].waypoints[i - 1].z) * 0.5
                       * ObjectPath[obj].waypoint_speed[i];
            } else {
                key->c = -(ObjectPath[obj].waypoints[i + 1].z - ObjectPath[obj].waypoints[i - 1].z)
                       * ObjectPath[obj].waypoint_speed[i];
            }
            key->d = -(ObjectPath[obj].waypoints[i].z - ObjectPath[obj].waypoints[0].z);
        }
    } else {
        OBJECT_CURVESET(obj)->constants[2] = 0.0f;
        if (OBJECT_CURVESET(obj)->set[2] != 0) {
            NuAnimCurveDestroy(OBJECT_CURVESET(obj)->set[2]);
            OBJECT_CURVESET(obj)->set[2] = 0;
        }
    }

    rotating = 0;
    changed = 0;
    for (i = 1; i < num; i++) {
        if (ObjectPath[obj].waypoint_rot[i].x != ObjectPath[obj].waypoint_rot[0].x) {
            changed = 1;
            rotating = 1;
            break;
        }
    }
    if (changed) {
        OBJECT_CURVESET(obj)->constants[3] = edobj_curve_unused;
        if (OBJECT_CURVESET(obj)->set[3] == 0) {
            OBJECT_CURVESET(obj)->set[3] = NuAnimCurveCreate(num);
        }
        if (OBJECT_CURVESET(obj)->set[3]->numkeys != num) {
            NuAnimCurveDestroy(OBJECT_CURVESET(obj)->set[3]);
            OBJECT_CURVESET(obj)->set[3] = NuAnimCurveCreate(num);
        }
        OBJECT_CURVESET(obj)->set[3]->mask = mask;
        for (i = 0; i < num; i++) {
            key = &OBJECT_CURVESET(obj)->set[3]->animkeys[i];
            key->time = 1.0f;
            for (j = 0; j < i; j++) {
                key->time += ObjectPath[obj].waypoint_time[j];
            }
            if (i == num - 1) {
                key->dtime = 0.0f;
            } else {
                key->dtime = 1.0 / (float)ObjectPath[obj].waypoint_time[i];
            }
            if (i == 0) {
                key->c = (ObjectPath[obj].waypoint_rot[i + 1].x - ObjectPath[obj].waypoint_rot[i].x) * 0.5
                       * ObjectPath[obj].waypoint_speed[i];
            } else if (i == num - 1) {
                key->c = (ObjectPath[obj].waypoint_rot[i].x - ObjectPath[obj].waypoint_rot[i - 1].x) * 0.5
                       * ObjectPath[obj].waypoint_speed[i];
            } else {
                key->c = (ObjectPath[obj].waypoint_rot[i + 1].x - ObjectPath[obj].waypoint_rot[i - 1].x)
                       * ObjectPath[obj].waypoint_speed[i];
            }
            key->d = ObjectPath[obj].waypoint_rot[i].x;
        }
    } else {
        OBJECT_CURVESET(obj)->constants[3] = ObjectPath[obj].waypoint_rot[0].x;
        if (OBJECT_CURVESET(obj)->set[3] != 0) {
            NuAnimCurveDestroy(OBJECT_CURVESET(obj)->set[3]);
            OBJECT_CURVESET(obj)->set[3] = 0;
        }
    }

    changed = 0;
    for (i = 1; i < num; i++) {
        if (ObjectPath[obj].waypoint_rot[i].y != ObjectPath[obj].waypoint_rot[0].y) {
            changed = 1;
            rotating = 1;
            break;
        }
    }
    if (changed) {
        OBJECT_CURVESET(obj)->constants[4] = edobj_curve_unused;
        if (OBJECT_CURVESET(obj)->set[4] == 0) {
            OBJECT_CURVESET(obj)->set[4] = NuAnimCurveCreate(num);
        }
        if (OBJECT_CURVESET(obj)->set[4]->numkeys != num) {
            NuAnimCurveDestroy(OBJECT_CURVESET(obj)->set[4]);
            OBJECT_CURVESET(obj)->set[4] = NuAnimCurveCreate(num);
        }
        OBJECT_CURVESET(obj)->set[4]->mask = mask;
        for (i = 0; i < num; i++) {
            key = &OBJECT_CURVESET(obj)->set[4]->animkeys[i];
            key->time = 1.0f;
            for (j = 0; j < i; j++) {
                key->time += ObjectPath[obj].waypoint_time[j];
            }
            if (i == num - 1) {
                key->dtime = 0.0f;
            } else {
                key->dtime = 1.0 / (float)ObjectPath[obj].waypoint_time[i];
            }
            if (i == 0) {
                key->c = (ObjectPath[obj].waypoint_rot[i + 1].y - ObjectPath[obj].waypoint_rot[i].y) * 0.5
                       * ObjectPath[obj].waypoint_speed[i];
            } else if (i == num - 1) {
                key->c = (ObjectPath[obj].waypoint_rot[i].y - ObjectPath[obj].waypoint_rot[i - 1].y) * 0.5
                       * ObjectPath[obj].waypoint_speed[i];
            } else {
                key->c = (ObjectPath[obj].waypoint_rot[i + 1].y - ObjectPath[obj].waypoint_rot[i - 1].y)
                       * ObjectPath[obj].waypoint_speed[i];
            }
            key->d = ObjectPath[obj].waypoint_rot[i].y;
        }
    } else {
        OBJECT_CURVESET(obj)->constants[4] = ObjectPath[obj].waypoint_rot[0].y;
        if (OBJECT_CURVESET(obj)->set[4] != 0) {
            NuAnimCurveDestroy(OBJECT_CURVESET(obj)->set[4]);
            OBJECT_CURVESET(obj)->set[4] = 0;
        }
    }

    changed = 0;
    for (i = 1; i < num; i++) {
        if (ObjectPath[obj].waypoint_rot[i].z != ObjectPath[obj].waypoint_rot[0].z) {
            changed = 1;
            rotating = 1;
            break;
        }
    }
    if (changed) {
        OBJECT_CURVESET(obj)->constants[5] = edobj_curve_unused;
        if (OBJECT_CURVESET(obj)->set[5] == 0) {
            OBJECT_CURVESET(obj)->set[5] = NuAnimCurveCreate(num);
        }
        if (OBJECT_CURVESET(obj)->set[5]->numkeys != num) {
            NuAnimCurveDestroy(OBJECT_CURVESET(obj)->set[5]);
            OBJECT_CURVESET(obj)->set[5] = NuAnimCurveCreate(num);
        }
        OBJECT_CURVESET(obj)->set[5]->mask = mask;
        for (i = 0; i < num; i++) {
            key = &OBJECT_CURVESET(obj)->set[5]->animkeys[i];
            key->time = 1.0f;
            for (j = 0; j < i; j++) {
                key->time += ObjectPath[obj].waypoint_time[j];
            }
            if (i == num - 1) {
                key->dtime = 0.0f;
            } else {
                key->dtime = 1.0 / (float)ObjectPath[obj].waypoint_time[i];
            }
            if (i == 0) {
                key->c = (ObjectPath[obj].waypoint_rot[i + 1].z - ObjectPath[obj].waypoint_rot[i].z) * 0.5
                       * ObjectPath[obj].waypoint_speed[i];
            } else if (i == num - 1) {
                key->c = (ObjectPath[obj].waypoint_rot[i].z - ObjectPath[obj].waypoint_rot[i - 1].z) * 0.5
                       * ObjectPath[obj].waypoint_speed[i];
            } else {
                key->c = (ObjectPath[obj].waypoint_rot[i + 1].z - ObjectPath[obj].waypoint_rot[i - 1].z)
                       * ObjectPath[obj].waypoint_speed[i];
            }
            key->d = ObjectPath[obj].waypoint_rot[i].z;
        }
    } else {
        OBJECT_CURVESET(obj)->constants[5] = ObjectPath[obj].waypoint_rot[0].z;
        if (OBJECT_CURVESET(obj)->set[5] != 0) {
            NuAnimCurveDestroy(OBJECT_CURVESET(obj)->set[5]);
            OBJECT_CURVESET(obj)->set[5] = 0;
        }
    }

    if (ObjectPath[obj].waypoint_rot[0].x != 0.0f || ObjectPath[obj].waypoint_rot[0].y != 0.0f
        || ObjectPath[obj].waypoint_rot[0].z != 0.0f) {
        rotating = 1;
    }
    ObjectInstance[obj].anim = &ObjectInstAnim[obj];
    if (ObjectPath[obj].platinst != -1) {
        UpdatePlatinst(ObjectPath[obj].platinst, &ObjectInstAnim[obj].mtx);
        if (rotating) {
            PlatInstRotate(ObjectPath[obj].platinst, 1);
        }
    }
    ObjectInstAnim[obj].tfactor = ObjectPath[obj].anim_speed;
    ObjectInstAnim[obj].playing = 0;
    ObjectInstAnim[obj].repeating = ObjectPath[obj].repeating;
    ObjectInstAnim[obj].oscillate = ObjectPath[obj].oscillate;
    if (!ObjectPath[obj].oscillate) {
        ObjectInstAnim[obj].backwards = 0;
    }
    ObjectInstAnim[obj].anim_ix = obj;
    ObjectInstAnim[obj].tinterval = ObjectPath[obj].anim_pause;
}


void edobjSoundCreate(struct nuvec_s *pos)
{
    int slot;

    slot = ObjectPath[edobj_nearest].num_sounds;
    if (slot != 8 && edobj_sound_type != -1) {
        NuVecSub(&ObjectPath[edobj_nearest].sound_pos[slot], pos,
                 &ObjectPath[edobj_nearest].waypoints[0]);
        ObjectPath[edobj_nearest].sound_id[slot] = edobj_sound_type;
        ObjectPath[edobj_nearest].sound_type[slot] = 1;
        strcpy(ObjectPath[edobj_nearest].sound_name[slot], edbitsGetSoundName(edobj_sound_type));
        ObjectPath[edobj_nearest].sound_timing[slot] = 50.0f;
        ObjectPath[edobj_nearest].num_sounds++;
    }
}


void edobjSoundDestroy(int obj, int slot)
{
    int i;

    for (i = slot; i < ObjectPath[obj].num_sounds - 1; i++) {
        ObjectPath[obj].sound_id[i] = ObjectPath[obj].sound_id[i + 1];
        ObjectPath[obj].sound_timing[i] = ObjectPath[obj].sound_timing[i + 1];
        ObjectPath[obj].sound_pos[i] = ObjectPath[obj].sound_pos[i + 1];
        strcpy(ObjectPath[obj].sound_name[i], ObjectPath[obj].sound_name[i + 1]);
    }
    ObjectPath[obj].num_sounds--;
}


void edobjParticleCreate(struct nuvec_s *pos)
{
    int slot;

    slot = ObjectPath[edobj_nearest].num_particles;
    if (slot != 8 && edobj_particle_type != -1) {
        NuVecSub(&ObjectPath[edobj_nearest].particle_pos[slot], pos,
                 &ObjectPath[edobj_nearest].waypoints[0]);
        ObjectPath[edobj_nearest].particle_rotz[slot] = edobj_emitrotz;
        ObjectPath[edobj_nearest].particle_roty[slot] = edobj_emitroty;
        ObjectPath[edobj_nearest].particle_type[slot] = edobj_particle_type;
        strcpy(ObjectPath[edobj_nearest].particle_name[slot], debtab[edobj_particle_type]->name);
        ObjectPath[edobj_nearest].particle_rate[slot] = 1;
        ObjectPath[edobj_nearest].particle_switch[slot] = 0;
        ObjectPath[edobj_nearest].num_particles++;
    }
}


void edobjParticleDestroy(int obj, int slot)
{
    int i;

    for (i = slot; i < ObjectPath[obj].num_particles - 1; i++) {
        ObjectPath[obj].particle_type[i] = ObjectPath[obj].particle_type[i + 1];
        ObjectPath[obj].particle_rate[i] = ObjectPath[obj].particle_rate[i + 1];
        ObjectPath[obj].particle_switch[i] = ObjectPath[obj].particle_switch[i + 1];
        ObjectPath[obj].particle_pos[i] = ObjectPath[obj].particle_pos[i + 1];
        strcpy(ObjectPath[obj].particle_name[i], ObjectPath[obj].particle_name[i + 1]);
    }
    ObjectPath[obj].num_particles--;
}


void edobjWaypointCreate(struct nuvec_s *pos)
{
    if (ObjectPath[edobj_nearest].num_waypoints != 8) {
        ObjectPath[edobj_nearest].waypoints[ObjectPath[edobj_nearest].num_waypoints] = *pos;
        ObjectPath[edobj_nearest].waypoint_speed[ObjectPath[edobj_nearest].num_waypoints] = 0.0f;
        if (ObjectPath[edobj_nearest].num_waypoints >= 2) {
            ObjectPath[edobj_nearest].waypoint_speed[ObjectPath[edobj_nearest].num_waypoints - 1] =
                0.25f;
        }
        memset(&ObjectPath[edobj_nearest].waypoint_rot[ObjectPath[edobj_nearest].num_waypoints], 0,
               sizeof(struct nuvec_s));
        ObjectPath[edobj_nearest].waypoint_time[ObjectPath[edobj_nearest].num_waypoints - 1] = 2;
        ObjectPath[edobj_nearest].num_waypoints++;
        ObjectPath[edobj_nearest].total_time += 2;
        edobjConvertPathToAnim(edobj_nearest);
    }
}


void edobjWaypointDestroy(int index)
{
    int i;

    if (ObjectPath[edobj_nearest].num_waypoints != 1) {
        ObjectPath[edobj_nearest].total_time -= ObjectPath[edobj_nearest].waypoint_time[index];
        for (i = index; i < ObjectPath[edobj_nearest].num_waypoints - 1; i++) {
            ObjectPath[edobj_nearest].waypoints[i] = ObjectPath[edobj_nearest].waypoints[i + 1];
            ObjectPath[edobj_nearest].waypoint_speed[i] = ObjectPath[edobj_nearest].waypoint_speed[i + 1];
            ObjectPath[edobj_nearest].waypoint_rot[i] = ObjectPath[edobj_nearest].waypoint_rot[i + 1];
            ObjectPath[edobj_nearest].waypoint_time[i] = ObjectPath[edobj_nearest].waypoint_time[i + 1];
        }
        ObjectPath[edobj_nearest].num_waypoints--;
        ObjectPath[edobj_nearest].waypoint_time[ObjectPath[edobj_nearest].num_waypoints - 1] = 0;
        edobjConvertPathToAnim(edobj_nearest);
        edobj_nearest_waypoint = -1;
    }
}


int edobjObjectCreate(struct nuvec_s *pos, int special)
{
    struct nuvec_s delta;
    int start;
    int obj;
    int objid;
    int i;

    start = edobj_next_instance;
    obj = start;
    do {
        if (ObjectInstance[obj].id == -1) {
            break;
        }
        edobj_next_instance = obj + 1;
        if (edobj_next_instance > 64) {
            edobj_next_instance = 0;
        }
        obj = edobj_next_instance;
        if (obj == start) {
            return -1;
        }
    } while (1);
    objid = edobj_base_scene->specials[special].instance->objid;
    ObjectPath[obj].instance = special;
    ObjectPath[obj].num_waypoints = 1;
    ObjectPath[obj].total_time = 1;
    ObjectPath[obj].num_particles = 0;
    ObjectPath[obj].num_sounds = 0;
    ObjectPath[obj].anim_speed = edobj_default_anim_speed;
    ObjectPath[obj].oscillate = 1;
    ObjectPath[obj].repeating = 1;
    ObjectPath[obj].anim_pause = 0.0f;
    ObjectPath[obj].waypoint_time[0] = 0;
    ObjectPath[obj].anim_start_offset = 0;
    ObjectPath[obj].platinst = -1;
    ObjectPath[obj].switch_type = 0;
    ObjectPath[obj].switch_id = -1;
    ObjectPath[obj].switch_var = 0.0f;
    ObjectPath[obj].bouncy_player_grav = 0.0f;
    ObjectPath[obj].bouncy_tension = 0.0f;
    ObjectInstance[obj].id = objid;
    ObjectPath[obj].bouncy_damping = 0.0f;
    NuVecSub(&delta, &ObjectPath[obj].waypoints[0], pos);
    for (i = 0; i < ObjectPath[obj].num_waypoints; i++) {
        NuVecSub(&ObjectPath[obj].waypoints[i], &ObjectPath[obj].waypoints[i], &delta);
    }
    edobjConvertPathToAnim(obj);
    start = edobj_next_instance++;
    return start;
}


int edobjObjectCreateCopy(struct nuvec_s *pos, int src)
{
    struct nuvec_s delta;
    int obj;
    int i;

    obj = edobjObjectCreate(pos, ObjectPath[src].instance);
    if (obj == -1) {
        return -1;
    }
    ObjectPath[obj] = ObjectPath[src];
    ObjectPath[obj].platinst = -1;
    NuVecSub(&delta, &ObjectPath[obj].waypoints[0], pos);
    for (i = 0; i < ObjectPath[obj].num_waypoints; i++) {
        NuVecSub(&ObjectPath[obj].waypoints[i], &ObjectPath[obj].waypoints[i], &delta);
    }
    edobjConvertPathToAnim(obj);
    return obj;
}


void edobjObjectDestroy(int obj)
{
    if (ObjectInstance[obj].id != -1) {
        if (ObjectInstance[obj].anim != 0) {
            NuAnimDataDestroy(ObjectAnim[ObjectInstance[obj].anim->anim_ix]);
            ObjectAnim[ObjectInstance[obj].anim->anim_ix] = 0;
            ObjectInstance[obj].anim = 0;
        }
        if (ObjectPath[obj].platinst != -1) {
            DeletePlatinst(ObjectPath[obj].platinst);
        }
        ObjectInstance[obj].id = -1;
        ObjectPath[obj].instance = -1;
    }
}


inline void edobjSoundPlace(int slot, struct nuvec_s *wpos)
{
    NuVecSub(&ObjectPath[edobj_nearest].sound_pos[slot], wpos,
             &ObjectPath[edobj_nearest].waypoints[0]);
}


inline void edobjParticlePlace(int slot, struct nuvec_s *wpos)
{
    NuVecSub(&ObjectPath[edobj_nearest].particle_pos[slot], wpos,
             &ObjectPath[edobj_nearest].waypoints[0]);
    ObjectPath[edobj_nearest].particle_rotz[slot] = edobj_emitrotz;
    ObjectPath[edobj_nearest].particle_roty[slot] = edobj_emitroty;
}


inline void edobjWaypointPlace(int index, struct nuvec_s *wpos)
{
    ObjectPath[edobj_nearest].waypoints[index] = *wpos;
    edobjConvertPathToAnim(edobj_nearest);
}


inline void edobjObjectPlace(int obj, struct nuvec_s *pos)
{
    struct nuvec_s delta;
    int i;

    NuVecSub(&delta, &ObjectPath[obj].waypoints[0], pos);
    for (i = 0; i < ObjectPath[obj].num_waypoints; i++) {
        NuVecSub(&ObjectPath[obj].waypoints[i], &ObjectPath[obj].waypoints[i], &delta);
    }
    edobjConvertPathToAnim(obj);
}


void edobjDoInput(struct pad_s *pad)
{
    struct nuvec_s pos;
    int obj;

    if (!(pad->paddata & 0x100)) {
        edcamMove(pad);
    }
    if (pad->paddata & 0x100) {
        if (!edobj_copy_mode && !edobj_particle_mode && !edobj_sound_mode && (pad->buttons & 0x40)) {
            if (edobj_waypoint_mode) {
                edobj_waypoint_mode = 0;
            } else if (edobj_nearest != -1) {
                edobj_nearest_waypoint = 0;
                edobj_waypoint_mode = 1;
            }
        }
        if (!edobj_waypoint_mode && !edobj_particle_mode && !edobj_sound_mode && (pad->buttons & 0x20)) {
            if (edobj_copy_mode) {
                edobj_copy_mode = 0;
            } else if (edobj_nearest != -1) {
                edobj_copy_mode = 1;
            }
        }
        if (!edobj_waypoint_mode && !edobj_copy_mode && !edobj_sound_mode && (pad->buttons & 0x80)) {
            if (edobj_particle_mode) {
                edobj_particle_mode = 0;
            } else if (edobj_nearest != -1) {
                edobj_nearest_particle = -1;
                edobj_particle_mode = 1;
            }
        }
        if (!edobj_waypoint_mode && !edobj_copy_mode && !edobj_particle_mode && (pad->buttons & 0x10)) {
            if (edobj_sound_mode) {
                edobj_sound_mode = 0;
            } else if (edobj_nearest != -1) {
                edobj_nearest_sound = -1;
                edobj_sound_mode = 1;
            }
        }
        if (edobj_waypoint_mode) {
            if (edobj_nearest_waypoint != -1) {
                if (pad->buttons & 8) {
                    edobj_nearest_waypoint++;
                    if (edobj_nearest_waypoint == ObjectPath[edobj_nearest].num_waypoints) {
                        edobj_nearest_waypoint = 0;
                    }
                }
                if (pad->buttons & 2) {
                    edobj_nearest_waypoint--;
                    if (edobj_nearest_waypoint == -1) {
                        edobj_nearest_waypoint = ObjectPath[edobj_nearest].num_waypoints - 1;
                    }
                }
            } else {
                edobjDetermineNearestWaypoint(-1.0f);
            }
            if (edobj_nearest_waypoint != -1) {
                edcamSetPos(&ObjectPath[edobj_nearest].waypoints[edobj_nearest_waypoint]);
            }
        } else if (!edobj_copy_mode) {
            if (edobj_particle_mode) {
                if (edobj_nearest_particle != -1) {
                    if (pad->buttons & 8) {
                        edobj_nearest_particle++;
                        if (edobj_nearest_particle == ObjectPath[edobj_nearest].num_particles) {
                            edobj_nearest_particle = 0;
                        }
                    }
                    if (pad->buttons & 2) {
                        edobj_nearest_particle--;
                        if (edobj_nearest_particle == -1) {
                            edobj_nearest_particle = ObjectPath[edobj_nearest].num_particles - 1;
                        }
                    }
                } else {
                    edobjDetermineNearestParticle(-1.0f);
                }
                if (edobj_nearest_particle != -1) {
                    NuVecAdd(&pos, &ObjectPath[edobj_nearest].waypoints[0],
                             &ObjectPath[edobj_nearest].particle_pos[edobj_nearest_particle]);
                    edcamSetPos(&pos);
                    edobj_emitrotz = ObjectPath[edobj_nearest].particle_rotz[edobj_nearest_particle];
                    edobj_emitroty = ObjectPath[edobj_nearest].particle_roty[edobj_nearest_particle];
                    edobj_particle_type =
                        ObjectPath[edobj_nearest].particle_type[edobj_nearest_particle];
                }
            } else if (edobj_sound_mode) {
                if (edobj_nearest_sound != -1) {
                    if (pad->buttons & 8) {
                        edobj_nearest_sound++;
                        if (edobj_nearest_sound == ObjectPath[edobj_nearest].num_sounds) {
                            edobj_nearest_sound = 0;
                        }
                    }
                    if (pad->buttons & 2) {
                        edobj_nearest_sound--;
                        if (edobj_nearest_sound == -1) {
                            edobj_nearest_sound = ObjectPath[edobj_nearest].num_sounds - 1;
                        }
                    }
                } else {
                    edobjDetermineNearestSound(-1.0f);
                }
                if (edobj_nearest_sound != -1) {
                    NuVecAdd(&pos, &ObjectPath[edobj_nearest].waypoints[0],
                             &ObjectPath[edobj_nearest].sound_pos[edobj_nearest_sound]);
                    edcamSetPos(&pos);
                    edobj_sound_type = ObjectPath[edobj_nearest].sound_id[edobj_nearest_sound];
                }
            } else {
                if (edobj_nearest != -1) {
                    if (pad->buttons & 8) {
                        obj = edobj_nearest;
                        do {
                            obj++;
                            if (obj == 64) {
                                obj %= 64;
                            }
                        } while (ObjectPath[obj].instance == -1);
                        edobj_nearest = obj;
                    }
                    if (pad->buttons & 2) {
                        do {
                            edobj_nearest--;
                            if (edobj_nearest == -1) {
                                edobj_nearest = 63;
                            }
                        } while (ObjectPath[edobj_nearest].instance == -1);
                    }
                } else {
                    edobjDetermineNearestObject(-1.0f);
                }
                if (edobj_nearest != -1) {
                    edcamSetPos(&ObjectPath[edobj_nearest].waypoints[0]);
                }
            }
        }
    }
    edcamGetPosAng(&edobj_cam_pos, &edobj_cam_ax, &edobj_cam_ay);
    if (edobj_snap_y) {
        edobj_cam_pos.y = edobj_snap_vec.y
                        + (int)(((edobj_cam_pos.y - edobj_snap_vec.y) + 0.25) * 2.0) * 0.5f;
    }
    if (edobj_snap_xz) {
        edobj_cam_pos.x = edobj_snap_vec.x
                        + (int)(((edobj_cam_pos.x - edobj_snap_vec.x) + 0.25) * 2.0) * 0.5f;
        edobj_cam_pos.z = edobj_snap_vec.z
                        + (int)(((edobj_cam_pos.z - edobj_snap_vec.z) + 0.25) * 2.0) * 0.5f;
    }
    if (!(pad->paddata & 0x100)) {
        if (pad->buttons & 0x80) {
            edobj_active_menu = edobj_options_menu;
        }
        if (pad->buttons & 0x40) {
            if (edobj_waypoint_mode) {
                edobjWaypointCreate(&edobj_cam_pos);
            } else if (edobj_copy_mode) {
                edobjObjectCreateCopy(&edobj_cam_pos, edobj_nearest);
            } else if (edobj_particle_mode) {
                edobjParticleCreate(&edobj_cam_pos);
            } else if (edobj_sound_mode) {
                edobjSoundCreate(&edobj_cam_pos);
            } else if (edobj_instance_type != -1) {
                edobjObjectCreate(&edobj_cam_pos, edobj_instance_type);
            }
        }
        if (pad->paddata & 0x20) {
            if (edobj_waypoint_mode) {
                if (edobj_nearest_waypoint != -1) {
                    edobjWaypointPlace(edobj_nearest_waypoint, &edobj_cam_pos);
                }
            } else if (!edobj_copy_mode) {
                if (edobj_particle_mode) {
                    if (edobj_nearest_particle != -1) {
                        edobjParticlePlace(edobj_nearest_particle, &edobj_cam_pos);
                    }
                } else if (edobj_sound_mode) {
                    if (edobj_nearest_sound != -1) {
                        edobjSoundPlace(edobj_nearest_sound, &edobj_cam_pos);
                    }
                } else if (edobj_nearest != -1) {
                    edobjObjectPlace(edobj_nearest, &edobj_cam_pos);
                }
            }
        }
        if (pad->buttons & 0x10) {
            if (edobj_waypoint_mode) {
                if (edobj_nearest_waypoint != -1) {
                    edobjWaypointDestroy(edobj_nearest_waypoint);
                }
            } else if (!edobj_copy_mode) {
                if (edobj_particle_mode) {
                    if (edobj_nearest_particle != -1) {
                        edobjParticleDestroy(edobj_nearest, edobj_nearest_particle);
                        edobj_nearest_particle = -1;
                    }
                } else if (edobj_sound_mode) {
                    if (edobj_nearest_sound != -1) {
                        edobjSoundDestroy(edobj_nearest, edobj_nearest_sound);
                        edobj_nearest_sound = -1;
                    }
                } else {
                    if (edobj_nearest != -1) {
                        edobjObjectDestroy(edobj_nearest);
                    }
                    edobj_nearest = -1;
                }
            }
        }
    }
    if (edobj_particle_mode) {
        edobj_emitroty += pad->rotleft;
        edobj_emitroty -= pad->rotright;
        edobj_emitrotz += pad->rotup;
        if (edobj_emitrotz > 0) {
            edobj_emitrotz = 0;
        }
        edobj_emitrotz -= pad->rotdown;
        if (edobj_emitrotz < -0x8000) {
            edobj_emitrotz = -0x8000;
        }
    }
}


void edobjDrawCursor(void)
{
    struct nuvtx_tc1_s line[2];
    struct nuvec_s dir;
    int type;

    line[0].pnt.x = edobj_cam_pos.x - 0.5f;
    line[0].pnt.y = edobj_cam_pos.y;
    line[0].pnt.z = edobj_cam_pos.z;
    line[0].diffuse = -1;
    line[1].pnt.x = edobj_cam_pos.x + 0.5f;
    line[1].pnt.y = edobj_cam_pos.y;
    line[1].pnt.z = edobj_cam_pos.z;
    line[1].diffuse = -1;
    NuRndrLine3d(line, edobj_mtl, 0);
    line[0].pnt.x = edobj_cam_pos.x;
    line[0].pnt.y = edobj_cam_pos.y - 0.5f;
    line[0].pnt.z = edobj_cam_pos.z;
    line[0].diffuse = -1;
    line[1].pnt.x = edobj_cam_pos.x;
    line[1].pnt.y = edobj_cam_pos.y + 0.5f;
    line[1].pnt.z = edobj_cam_pos.z;
    line[1].diffuse = -1;
    NuRndrLine3d(line, edobj_mtl, 0);
    line[0].pnt.x = edobj_cam_pos.x;
    line[0].pnt.y = edobj_cam_pos.y;
    line[0].pnt.z = edobj_cam_pos.z - 0.5f;
    line[0].diffuse = -1;
    line[1].pnt.x = edobj_cam_pos.x;
    line[1].pnt.y = edobj_cam_pos.y;
    line[1].pnt.z = edobj_cam_pos.z + 0.5f;
    line[1].diffuse = -1;
    NuRndrLine3d(line, edobj_mtl, 0);
    if (edobj_particle_mode) {
        dir.x = 0.0f;
        dir.y = 0.375f;
        dir.z = 0.0f;
        NuVecRotateZ(&dir, &dir, edobj_emitrotz);
        NuVecRotateY(&dir, &dir, edobj_emitroty);
        line[0].pnt.x = edobj_cam_pos.x;
        line[0].pnt.y = edobj_cam_pos.y;
        line[0].pnt.z = edobj_cam_pos.z;
        line[0].diffuse = 0xFF0000FF;
        line[1].pnt.x = edobj_cam_pos.x + dir.x;
        line[1].pnt.y = edobj_cam_pos.y + dir.y;
        line[1].pnt.z = edobj_cam_pos.z + dir.z;
        line[1].diffuse = 0xFF0000FF;
        NuRndrLine3d(line, edobj_mtl, 0);
    }
    if (!edobj_waypoint_mode && !edobj_particle_mode) {
        if (edobj_copy_mode) {
            type = ObjectPath[edobj_nearest].instance;
        } else {
            type = edobj_instance_type;
        }
        if (type != -1 && edobj_blink_timer % 150 < 75) {
            NuMtxSetIdentity(&edobj_cursor_matrix);
            edobj_cursor_matrix._30 = edobj_cam_pos.x;
            edobj_cursor_matrix._31 = edobj_cam_pos.y;
            edobj_cursor_matrix._32 = edobj_cam_pos.z;
            NuRndrGScnObj(edobj_base_scene->gobjs[edobj_base_scene->specials[type].instance->objid],
                          &edobj_cursor_matrix);
        }
    }
    NuRndrRect2di(0x1720, 0xA50, 0xDC0, 0x370, 0x80808080, edobj_mtl_zoff);
    NuRndrRect2di(0x1710, 0x990, 0xDE0, 0xC0, 0x80000000, edobj_mtl_zoff);
    NuRndrLine2di(0x1710, 0xA50, 0x1710, 0xDC8, 0x80000000, edobj_mtl_zoff);
    NuRndrLine2di(0x24F0, 0xA50, 0x24F0, 0xDC8, 0x80000000, edobj_mtl_zoff);
    NuRndrLine2di(0x1710, 0xDC8, 0x24F0, 0xDC8, 0x80000000, edobj_mtl_zoff);
    NuFntSet(app_fnt);
    NuFntSetPen(0x80808080);
    NuFntPrintEx(0x17C0, 0xA28, 0, info_box_title);
    NuFntSetPen(0x80000000);
    if (edobj_instance_type == -1) {
        NuFntPrintEx(0x17C0, 0xAF0, 0, info_inst_none);
    } else {
        NuFntPrintEx(0x17C0, 0xAF0, 0, info_inst_fmt,
                     edobj_base_scene->specials[edobj_instance_type].name);
    }
    if (edobj_nearest == -1) {
        NuFntPrintEx(0x17C0, 0xB90, 0, info_highlight_none);
    } else if (edobj_waypoint_mode) {
        NuFntPrintEx(0x17C0, 0xB90, 0, info_locked_fmt,
                     edobj_base_scene->specials[ObjectPath[edobj_nearest].instance].name);
        NuFntPrintEx(0x1810, 0xC30, 0, info_waypoints_fmt, ObjectPath[edobj_nearest].num_waypoints,
                     8);
        if (edobj_nearest_waypoint == -1) {
            NuFntPrintEx(0x1810, 0xCD0, 0, info_hilite_wp_none);
        } else {
            NuFntPrintEx(0x1810, 0xCD0, 0, info_hilite_wp_fmt, edobj_nearest_waypoint + 1);
        }
    } else if (edobj_particle_mode) {
        NuFntPrintEx(0x17C0, 0xB90, 0, info_locked_fmt,
                     edobj_base_scene->specials[ObjectPath[edobj_nearest].instance].name);
        NuFntPrintEx(0x1810, 0xC30, 0, info_particles_fmt, ObjectPath[edobj_nearest].num_particles,
                     8);
        if (edobj_particle_type == -1) {
            NuFntPrintEx(0x1810, 0xCD0, 0, info_type_none);
        } else {
            NuFntPrintEx(0x1810, 0xCD0, 0, info_type_fmt, debtab[edobj_particle_type]->name);
        }
    } else if (edobj_sound_mode) {
        NuFntPrintEx(0x17C0, 0xB90, 0, info_locked_fmt,
                     edobj_base_scene->specials[ObjectPath[edobj_nearest].instance].name);
        NuFntPrintEx(0x1810, 0xC30, 0, info_sounds_fmt, ObjectPath[edobj_nearest].num_sounds, 8);
        if (edobj_sound_type == -1) {
            NuFntPrintEx(0x1810, 0xCD0, 0, info_type_none);
        } else {
            NuFntPrintEx(0x1810, 0xCD0, 0, info_type_fmt, edbitsGetSoundName(edobj_sound_type));
        }
    } else {
        NuFntPrintEx(0x17C0, 0xB90, 0, info_highlight_fmt,
                     edobj_base_scene->specials[ObjectPath[edobj_nearest].instance].name);
    }
    NuFntPrintEx(0x17C0, 0xD70, 0, info_coord_fmt, edobj_cam_pos.x);
    NuFntPrintEx(0x1C20, 0xD70, 0, info_coord_fmt, edobj_cam_pos.y);
    NuFntPrintEx(0x2080, 0xD70, 0, info_coord_fmt, edobj_cam_pos.z);
}


int edobjFileSaveObjects(char *filename)
{
    char name[32];
    int count;
    int i;
    int j;

    count = 0;
    for (i = 0; i < 64; i++) {
        if (ObjectPath[i].instance != -1) {
            count++;
        }
    }
    EdFileSetMedia(1);
    if (EdFileOpen(filename, 1)) {
        EdFileWriteInt(13);
        EdFileWriteInt(count);
        for (i = 0; i < 64; i++) {
            if (ObjectPath[i].instance == -1) {
                continue;
            }
            strncpy(name, edobj_base_scene->specials[ObjectPath[i].instance].name, 20);
            name[20] = 0;
            EdFileWrite(name, 20);
            EdFileWriteFloat(ObjectPath[i].anim_speed);
            EdFileWriteInt(ObjectPath[i].oscillate);
            EdFileWriteInt(ObjectPath[i].repeating);
            EdFileWriteFloat(ObjectPath[i].anim_pause);
            EdFileWriteInt(ObjectPath[i].num_waypoints);
            EdFileWriteInt(ObjectPath[i].num_particles);
            EdFileWriteInt(ObjectPath[i].num_sounds);
            EdFileWriteInt(ObjectPath[i].switch_type);
            EdFileWriteInt(ObjectPath[i].switch_id);
            EdFileWriteFloat(ObjectPath[i].switch_var);
            EdFileWriteFloat(ObjectPath[i].switch_delay);
            for (j = 0; j < ObjectPath[i].num_particles; j++) {
                EdFileWrite(ObjectPath[i].particle_name[j], 16);
                EdFileWriteInt(ObjectPath[i].particle_rate[j]);
                EdFileWriteInt(ObjectPath[i].particle_switch[j]);
                EdFileWrite(&ObjectPath[i].particle_pos[j], 12);
                EdFileWriteShort(ObjectPath[i].particle_rotz[j]);
                EdFileWriteShort(ObjectPath[i].particle_roty[j]);
            }
            EdFileWriteInt(ObjectPath[i].anim_start_offset);
            for (j = 0; j < ObjectPath[i].num_waypoints; j++) {
                EdFileWrite(&ObjectPath[i].waypoints[j], 12);
                EdFileWriteFloat(ObjectPath[i].waypoint_speed[j]);
                EdFileWrite(&ObjectPath[i].waypoint_rot[j], 12);
                EdFileWriteInt(ObjectPath[i].waypoint_time[j]);
            }
            for (j = 0; j < ObjectPath[i].num_sounds; j++) {
                EdFileWrite(ObjectPath[i].sound_name[j], 16);
                EdFileWriteInt(ObjectPath[i].sound_type[j]);
                EdFileWriteFloat(ObjectPath[i].sound_timing[j]);
                EdFileWrite(&ObjectPath[i].sound_pos[j], 12);
            }
            EdFileWriteFloat(ObjectPath[i].bouncy_player_grav);
            EdFileWriteFloat(ObjectPath[i].bouncy_tension);
            EdFileWriteFloat(ObjectPath[i].bouncy_damping);
        }
        EdFileClose();
        return 1;
    }
    return 0;
}


inline void edobjResetAnimsToZero(void)
{
    struct edobjinstance_s *oi;
    struct nuinstanim_s *anim;
    int i;

    oi = ObjectInstance;
    for (i = 0; i < 64; i++) {
        ObjectPath[i].particle_type[0] = -1;
        if (oi[i].id != -1 && (anim = oi[i].anim) != 0) {
            anim->tfirst = 0.0f;
            anim->waiting = 1;
            anim->ltime = ObjectPath[i].anim_start_offset;
            anim->backwards = 0;
            anim->playing = 0;
        }
    }
}


int edobjFileLoadObjects(char *filename)
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

    current_version = 13;
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
        if (version == 1) {
            ObjectPath[i].instance = EdFileReadInt();
        } else {
            EdFileRead(name, 20);
            ObjectPath[i].instance = edobjLookupInstance(name);
        }
        if (ObjectPath[i].instance != -1) {
            ObjectInstance[i].id =
                edobj_base_scene->specials[ObjectPath[i].instance].instance->objid;
        } else {
            ObjectInstance[i].id = -1;
        }
        ObjectPath[i].anim_speed = EdFileReadFloat();
        ObjectPath[i].oscillate = EdFileReadInt();
        if (version >= 6) {
            ObjectPath[i].repeating = EdFileReadInt();
        } else {
            ObjectPath[i].repeating = 1;
        }
        ObjectPath[i].anim_pause = EdFileReadFloat();
        ObjectPath[i].num_waypoints = EdFileReadInt();
        if (version >= 9) {
            ObjectPath[i].num_particles = EdFileReadInt();
        } else {
            ObjectPath[i].num_particles = 0;
        }
        if (version >= 10) {
            ObjectPath[i].num_sounds = EdFileReadInt();
        } else {
            ObjectPath[i].num_sounds = 0;
        }
        ObjectPath[i].platinst = -1;
        if (version >= 6) {
            ObjectPath[i].switch_type = EdFileReadInt();
            ObjectPath[i].switch_id = EdFileReadInt();
            ObjectPath[i].switch_var = EdFileReadFloat();
        } else {
            ObjectPath[i].switch_type = 0;
            ObjectPath[i].switch_id = -1;
            ObjectPath[i].switch_var = 0.0f;
        }
        if (version >= 8) {
            ObjectPath[i].switch_delay = EdFileReadFloat();
        } else {
            ObjectPath[i].switch_delay = 0.0f;
        }
        if (version >= 9) {
            skip = 0;
            if (ObjectPath[i].num_particles > 8) {
                skip = 8 - ObjectPath[i].num_particles;
                ObjectPath[i].num_particles = 8;
            }
            for (j = 0; j < ObjectPath[i].num_particles; j++) {
                EdFileRead(ObjectPath[i].particle_name[j], 16);
                ObjectPath[i].particle_type[j] = -1;
                ObjectPath[i].particle_rate[j] = EdFileReadInt();
                ObjectPath[i].particle_switch[j] = EdFileReadInt();
                EdFileRead(&ObjectPath[i].particle_pos[j], 12);
                if (version >= 13) {
                    ObjectPath[i].particle_rotz[j] = EdFileReadShort();
                    ObjectPath[i].particle_roty[j] = EdFileReadShort();
                } else {
                    ObjectPath[i].particle_rotz[j] = 0;
                    ObjectPath[i].particle_roty[j] = 0;
                }
            }
            for (j = 0; j < skip; j++) {
                EdFileRead(skipname, 16);
                EdFileReadInt();
                EdFileReadInt();
                EdFileRead(&pos, 12);
                if (version >= 13) {
                    EdFileReadShort();
                    EdFileReadShort();
                }
            }
        } else if (version >= 7) {
            EdFileRead(ObjectPath[i].particle_name[0], 16);
            ObjectPath[i].particle_type[0] = -1;
            ObjectPath[i].particle_rate[0] = EdFileReadInt();
            ObjectPath[i].particle_switch[0] = EdFileReadInt();
            if (ObjectPath[i].particle_name[0][0] != 0) {
                ObjectPath[i].num_particles = 1;
            }
        }
        if (version >= 5) {
            ObjectPath[i].anim_start_offset = EdFileReadInt();
        } else {
            ObjectPath[i].anim_start_offset = 0;
        }
        skip = 0;
        if (ObjectPath[i].num_waypoints > 8) {
            skip = 8 - ObjectPath[i].num_waypoints;
            ObjectPath[i].num_waypoints = 8;
        }
        ObjectPath[i].total_time = 0;
        for (j = 0; j < ObjectPath[i].num_waypoints; j++) {
            EdFileRead(&ObjectPath[i].waypoints[j], 12);
            ObjectPath[i].waypoint_speed[j] = EdFileReadFloat();
            if (version >= 3) {
                EdFileRead(&ObjectPath[i].waypoint_rot[j], 12);
            } else {
                memset(&ObjectPath[i].waypoint_rot[j], 0, 12);
            }
            if (version >= 4) {
                ObjectPath[i].waypoint_time[j] = EdFileReadInt();
            } else if (j < ObjectPath[i].num_waypoints - 1) {
                ObjectPath[i].waypoint_time[j] = 2;
            } else {
                ObjectPath[i].waypoint_time[j] = 0;
            }
            ObjectPath[i].total_time += ObjectPath[i].waypoint_time[j];
        }
        for (j = 0; j < skip; j++) {
            EdFileRead(&pos, 12);
            EdFileReadFloat();
            if (version >= 3) {
                EdFileRead(&pos, 12);
            }
            if (version >= 4) {
                EdFileReadInt();
            }
        }
        ObjectPath[i].prev_time = 0.99f;
        skip = 0;
        if (ObjectPath[i].num_sounds > 8) {
            skip = 8 - ObjectPath[i].num_sounds;
            ObjectPath[i].num_sounds = 8;
        }
        if (version >= 11) {
            for (j = 0; j < ObjectPath[i].num_sounds; j++) {
                EdFileRead(ObjectPath[i].sound_name[j], 16);
                ObjectPath[i].sound_id[j] = -1;
                ObjectPath[i].sound_type[j] = EdFileReadInt();
                ObjectPath[i].sound_timing[j] = EdFileReadFloat();
                EdFileRead(&ObjectPath[i].sound_pos[j], 12);
            }
            for (j = 0; j < skip; j++) {
                EdFileRead(skipname, 16);
                EdFileReadInt();
                EdFileReadFloat();
                EdFileRead(&pos, 12);
            }
        } else if (version == 10) {
            for (j = 0; j < ObjectPath[i].num_sounds; j++) {
                ObjectPath[i].sound_id[j] = EdFileReadFloat();
                ObjectPath[i].sound_type[j] = 1;
                ObjectPath[i].sound_timing[j] = EdFileReadFloat();
                EdFileRead(&ObjectPath[i].sound_pos[j], 12);
            }
            for (j = 0; j < skip; j++) {
                EdFileReadFloat();
                EdFileReadFloat();
                EdFileRead(&pos, 12);
            }
        }
        if (version >= 12) {
            ObjectPath[i].bouncy_player_grav = EdFileReadFloat();
            ObjectPath[i].bouncy_tension = EdFileReadFloat();
            ObjectPath[i].bouncy_damping = EdFileReadFloat();
        } else {
            ObjectPath[i].bouncy_player_grav = 0.0f;
            ObjectPath[i].bouncy_tension = 0.0f;
            ObjectPath[i].bouncy_damping = 0.0f;
        }
    }
    EdFileClose();
    for (i = 0; i < count; i++) {
        if (ObjectPath[i].instance != -1) {
            edobjConvertPathToAnim(i);
        }
    }
    edobjResetAnimsToZero();
    edobj_next_instance = count;
    edobj_nearest = -1;
    edobj_waypoint_mode = 0;
    edobj_copy_mode = 0;
    edobj_particle_mode = 0;
    edobj_sound_mode = 0;
    edobjDetermineNearestObject(1.0f);
    return 1;
}


void edobjcbInstanceMenu(struct eduimenu_s *menu)
{
    struct eduistyle_s style;
    int checked;
    int i;

    style = edobj_item_style;
    edobj_instance_menu = eduiMenuCreate(70, 70, 180, 250, app_fnt, edobjcbCancelInstanceMenu,
                                         instance_menu_title);
    if (edobj_instance_menu != 0) {
        eduiMenuAddItem(edobj_instance_menu,
                        eduiItemCheckCreate(-1, &style, edobj_instance_type == -1, 1,
                                            edobjcbSetInstanceType, menu_text_none));
        for (i = 0; i < edobj_base_scene->numspecial; i++) {
            if (edobj_instance_type == i) {
                checked = 1;
            } else {
                checked = 0;
            }
            eduiMenuAddItem(edobj_instance_menu,
                            eduiItemCheckCreate(i, &style, checked, 1, edobjcbSetInstanceType,
                                                edobj_base_scene->specials[i].name));
            if (checked == 1) {
                edobj_instance_menu->current = edui_last_item;
            }
        }
        eduiMenuAttach(menu, edobj_instance_menu);
        edobj_instance_menu->x = menu->x + 10;
        edobj_instance_menu->y = menu->y + 40;
    }
}


static inline void edobjAddSwitchTypeItem(struct eduistyle_s *style, int type, char *text)
{
    eduiMenuAddItem(edobj_switchtype_menu,
                    eduiItemCheckCreate(type, style, ObjectPath[edobj_nearest].switch_type == type,
                                        1, edobjcbSetSwitchType, text));
    if (edui_last_item->toggle) {
        edobj_switchtype_menu->current = edui_last_item;
    }
}


void edobjcbSwitchTypeMenu(struct eduimenu_s *menu)
{
    struct eduistyle_s style;

    style = edobj_item_style;
    edobj_switchtype_menu = eduiMenuCreate(70, 70, 250, 200, app_fnt,
                                           edobjcbCancelSwitchTypeMenu, switchtype_menu_title);
    if (edobj_switchtype_menu != 0) {
        edobjAddSwitchTypeItem(&style, 0, menu_text_none);
        edobjAddSwitchTypeItem(&style, 1, switchtype_text_switch);
        edobjAddSwitchTypeItem(&style, 2, switchtype_text_switch_once);
        edobjAddSwitchTypeItem(&style, 3, switchtype_text_switch_cont);
        edobjAddSwitchTypeItem(&style, 4, switchtype_text_proximity);
        edobjAddSwitchTypeItem(&style, 5, switchtype_text_proximity_once);
        edobjAddSwitchTypeItem(&style, 6, switchtype_text_proximity_cont);
        edobjAddSwitchTypeItem(&style, 7, switchtype_text_terrain);
        edobjAddSwitchTypeItem(&style, 8, switchtype_text_terrain_once);
        edobjAddSwitchTypeItem(&style, 9, switchtype_text_terrain_cont);
        eduiMenuAttach(menu, edobj_switchtype_menu);
        edobj_switchtype_menu->x = menu->x + 10;
        edobj_switchtype_menu->y = menu->y + 40;
    }
}


void edobjcbSwitchMenu(struct eduimenu_s *menu)
{
    struct eduistyle_s style;

    style = edobj_item_style;
    if (edobj_nearest != -1 && ObjectInstance[edobj_nearest].anim != 0) {
        edobj_switch_menu = eduiMenuCreate(70, 70, 180, 250, app_fnt, edobjcbCancelSwitchMenu,
                                           switch_menu_title);
        if (edobj_switch_menu != 0) {
            eduiMenuAddItem(edobj_switch_menu,
                            eduiItemSelCreate(1, &style, 0, 0, edobjcbSwitchTypeMenu,
                                              switch_menu_text_type));
            eduiMenuAddItem(edobj_switch_menu,
                            eduiItemSliderCreateInt(0, &style, 0, edobjcbSetSwitchId, -1, 129,
                                                    ObjectPath[edobj_nearest].switch_id,
                                                    switch_menu_text_id));
            eduiMenuAddItem(edobj_switch_menu,
                            eduiItemSliderCreate(0, &style, 0, edobjcbSetSwitchDelay, 0.0f, 20.0f,
                                                 ObjectPath[edobj_nearest].switch_delay,
                                                 switch_menu_text_delay));
            eduiMenuAddItem(edobj_switch_menu,
                            eduiItemSliderCreate(0, &style, 0, edobjcbSetSwitchVar, 0.0f, 20.0f,
                                                 ObjectPath[edobj_nearest].switch_var,
                                                 switch_menu_text_var));
            eduiMenuAttach(menu, edobj_switch_menu);
            edobj_switch_menu->x = menu->x + 10;
            edobj_switch_menu->y = menu->y + 40;
        }
    }
}


void edobjcbAnimPropertiesMenu(struct eduimenu_s *menu)
{
    struct eduistyle_s style;

    style = edobj_item_style;
    if (edobj_nearest != -1 && ObjectInstance[edobj_nearest].anim != 0) {
        edobj_animprop_menu = eduiMenuCreate(70, 70, 180, 250, app_fnt,
                                             edobjcbCancelAnimPropertiesMenu, animprop_menu_title);
        if (edobj_animprop_menu != 0) {
            eduiMenuAddItem(edobj_animprop_menu,
                            eduiItemSliderCreate(0, &style, 0, edobjcbChangeAnimSpeed, 0.0f, 0.1f,
                                                 ObjectPath[edobj_nearest].anim_speed,
                                                 animprop_menu_text_speed));
            eduiMenuAddItem(edobj_animprop_menu,
                            eduiItemSliderCreate(0, &style, 0, edobjcbChangeAnimPause, 0.0f, 10.0f,
                                                 ObjectPath[edobj_nearest].anim_pause,
                                                 animprop_menu_text_pause));
            eduiMenuAddItem(edobj_animprop_menu,
                            eduiItemSliderCreateInt(0, &style, 0, edobjcbChangeAnimStartOffset, 0,
                                                    20, ObjectPath[edobj_nearest].anim_start_offset,
                                                    animprop_menu_text_start_offset));
            eduiMenuAddItem(edobj_animprop_menu,
                            eduiItemSelCreate(1, &style, 0, 0, edobjcbSwitchMenu,
                                              animprop_menu_text_switch));
            eduiMenuAddItem(edobj_animprop_menu,
                            eduiItemToggleCreate(0, &style, ObjectPath[edobj_nearest].oscillate, 1,
                                                 edobjcbOscillateToggle,
                                                 animprop_menu_text_oscillate));
            eduiMenuAttach(menu, edobj_animprop_menu);
            edobj_animprop_menu->x = menu->x + 10;
            edobj_animprop_menu->y = menu->y + 40;
        }
    }
}


void edobjcbWayPropertiesMenu(struct eduimenu_s *menu)
{
    struct eduistyle_s style;

    style = edobj_item_style;
    if (edobj_waypoint_mode && edobj_nearest_waypoint != -1) {
        edobj_wayprop_menu = eduiMenuCreate(70, 70, 180, 300, app_fnt,
                                            edobjcbCancelWayPropertiesMenu, wayprop_menu_title);
        if (edobj_wayprop_menu != 0) {
            eduiMenuAddItem(
                edobj_wayprop_menu,
                eduiItemSliderCreate(0, &style, 0, edobjcbChangeWaypointSpeed, 0.0f, 1.0f,
                                     ObjectPath[edobj_nearest].waypoint_speed[edobj_nearest_waypoint],
                                     wayprop_menu_text_speed));
            eduiMenuAddItem(
                edobj_wayprop_menu,
                eduiItemSliderCreate(
                    0, &style, 0, edobjcbChangeWaypointXRot, -1440.0f, 2880.0f,
                    ObjectPath[edobj_nearest].waypoint_rot[edobj_nearest_waypoint].x * 57.295776f,
                    wayprop_menu_text_xrot));
            eduiItemSliderSetFmt(edui_last_item, slider_fmt_1dp);
            eduiItemSliderSetGranularity(edui_last_item, 0.1f);
            eduiMenuAddItem(
                edobj_wayprop_menu,
                eduiItemSliderCreate(
                    0, &style, 0, edobjcbChangeWaypointYRot, -1440.0f, 2880.0f,
                    ObjectPath[edobj_nearest].waypoint_rot[edobj_nearest_waypoint].y * 57.295776f,
                    wayprop_menu_text_yrot));
            eduiItemSliderSetFmt(edui_last_item, slider_fmt_1dp);
            eduiItemSliderSetGranularity(edui_last_item, 0.1f);
            eduiMenuAddItem(
                edobj_wayprop_menu,
                eduiItemSliderCreate(
                    0, &style, 0, edobjcbChangeWaypointZRot, -1440.0f, 2880.0f,
                    ObjectPath[edobj_nearest].waypoint_rot[edobj_nearest_waypoint].z * 57.295776f,
                    wayprop_menu_text_zrot));
            eduiItemSliderSetFmt(edui_last_item, slider_fmt_1dp);
            eduiItemSliderSetGranularity(edui_last_item, 0.1f);
            if (edobj_nearest_waypoint != ObjectPath[edobj_nearest].num_waypoints - 1) {
                eduiMenuAddItem(
                    edobj_wayprop_menu,
                    eduiItemSliderCreateInt(
                        0, &style, 0, edobjcbChangeWaypointTime, 1,
                        ObjectPath[edobj_nearest].waypoint_time[edobj_nearest_waypoint] -
                            (ObjectPath[edobj_nearest].total_time - 30),
                        ObjectPath[edobj_nearest].waypoint_time[edobj_nearest_waypoint],
                        wayprop_menu_text_time));
            }
            eduiMenuAttach(menu, edobj_wayprop_menu);
            edobj_wayprop_menu->x = menu->x + 10;
            edobj_wayprop_menu->y = menu->y + 40;
        }
    }
}


void edobjcbLocalSoundMenu(struct eduimenu_s *menu)
{
    struct eduistyle_s style;

    style = edobj_item_style;
    if (edobj_nearest_sound != -1) {
        edobj_localsound_menu = eduiMenuCreate(70, 70, 250, 300, app_fnt,
                                               edobjcbCancelLocalSoundMenu,
                                               localsound_menu_title);
        if (edobj_localsound_menu != 0) {
            eduiMenuAddItem(edobj_localsound_menu,
                            eduiItemSelCreate(1, &style, 0, 0, edobjcbLocalSoundTypeMenu,
                                              localsound_menu_text_type));
            eduiMenuAddItem(
                edobj_localsound_menu,
                eduiItemToggleCreate(
                    1, &style, ObjectPath[edobj_nearest].sound_type[edobj_nearest_sound] == 1, 1,
                    edobjcbToggleSoundType, localsound_menu_text_repeating));
            if (ObjectPath[edobj_nearest].sound_type[edobj_nearest_sound] == 1) {
                eduiMenuAddItem(
                    edobj_localsound_menu,
                    eduiItemSliderCreateInt(
                        0, &style, 0, edobjcbSetSoundTiming, 1, 99,
                        ObjectPath[edobj_nearest].sound_timing[edobj_nearest_sound],
                        localsound_menu_text_repeat_every));
            } else {
                eduiMenuAddItem(
                    edobj_localsound_menu,
                    eduiItemSliderCreate(
                        0, &style, 0, edobjcbSetSoundTiming, 1.0f,
                        ObjectPath[edobj_nearest].total_time,
                        ObjectPath[edobj_nearest].sound_timing[edobj_nearest_sound],
                        localsound_menu_text_trigger_time));
                eduiItemSliderSetFmt(edui_last_item, slider_fmt_1dp);
                eduiItemSliderSetGranularity(edui_last_item, 0.1f);
            }
            eduiMenuAttach(menu, edobj_localsound_menu);
            edobj_localsound_menu->x = menu->x + 10;
            edobj_localsound_menu->y = menu->y + 40;
        }
    }
}


void edobjcbParticleTypeMenu(struct eduimenu_s *menu)
{
    struct eduistyle_s style;
    int selected;
    int i;

    style = edobj_item_style;
    edobj_particletype_menu = eduiMenuCreate(70, 70, 250, 250, app_fnt,
                                             edobjcbCancelParticleTypeMenu,
                                             particletype_menu_title);
    if (edobj_particletype_menu != 0) {
        eduiMenuAddItem(edobj_particletype_menu,
                        eduiItemCheckCreate(0, &style, edobj_particle_type == -1, 0,
                                            edobjcbSetParticleType, particletype_text_none));
        for (i = 1; i < 128; i++) {
            if (debtab[i] != 0) {
                selected = 0;
                if (edobj_particle_type == i) {
                    selected = 1;
                }
                eduiMenuAddItem(edobj_particletype_menu,
                                eduiItemCheckCreate(i, &style, selected, 1,
                                                    edobjcbSetParticleType, debtab[i]->name));
                if (selected == 1) {
                    edobj_particletype_menu->current = edui_last_item;
                }
            }
        }
        eduiMenuAttach(menu, edobj_particletype_menu);
        edobj_particletype_menu->x = menu->x + 10;
        edobj_particletype_menu->y = menu->y + 40;
    }
}


void edobjcbLocalParticleMenu(struct eduimenu_s *menu)
{
    struct eduistyle_s style;

    style = edobj_item_style;
    if (edobj_nearest_particle != -1) {
        edobj_localparticle_menu = eduiMenuCreate(70, 70, 250, 300, app_fnt,
                                                  edobjcbCancelLocalParticleMenu,
                                                  localparticle_menu_title);
        if (edobj_localparticle_menu != 0) {
            eduiMenuAddItem(edobj_localparticle_menu,
                            eduiItemSelCreate(1, &style, 0, 0, edobjcbLocalParticleTypeMenu,
                                              localparticle_menu_text_type));
            eduiMenuAddItem(
                edobj_localparticle_menu,
                eduiItemSliderCreateInt(
                    0, &style, 0, edobjcbSetParticleRate, -10, 20,
                    ObjectPath[edobj_nearest].particle_rate[edobj_nearest_particle],
                    localparticle_menu_text_rate));
            eduiMenuAddItem(
                edobj_localparticle_menu,
                eduiItemToggleCreate(
                    0, &style, ObjectPath[edobj_nearest].particle_switch[edobj_nearest_particle],
                    1, edobjcbToggleParticleSwitch, localparticle_menu_text_moving));
            eduiMenuAttach(menu, edobj_localparticle_menu);
            edobj_localparticle_menu->x = menu->x + 10;
            edobj_localparticle_menu->y = menu->y + 40;
        }
    }
}


void edobjcbBouncyMenu(struct eduimenu_s *menu)
{
    struct eduistyle_s style;

    style = edobj_item_style;
    if (edobj_nearest != -1) {
        edobj_bouncy_menu = eduiMenuCreate(70, 70, 250, 300, app_fnt, edobjcbCancelBouncyMenu,
                                           bouncy_menu_title);
        if (edobj_bouncy_menu != 0) {
            eduiMenuAddItem(edobj_bouncy_menu,
                            eduiItemSliderCreate(0, &style, 0, edobjcbSetBouncyPlayerGrav, -0.1f,
                                                 0.2f, ObjectPath[edobj_nearest].bouncy_player_grav,
                                                 bouncy_menu_text_grav));
            eduiMenuAddItem(edobj_bouncy_menu,
                            eduiItemSliderCreate(0, &style, 0, edobjcbSetBouncyTension, 0.0f, 1.0f,
                                                 ObjectPath[edobj_nearest].bouncy_tension,
                                                 bouncy_menu_text_tension));
            eduiMenuAddItem(edobj_bouncy_menu,
                            eduiItemSliderCreate(0, &style, 0, edobjcbSetBouncyDamping, 0.0f, 1.0f,
                                                 ObjectPath[edobj_nearest].bouncy_damping,
                                                 bouncy_menu_text_damping));
            eduiMenuAttach(menu, edobj_bouncy_menu);
            edobj_bouncy_menu->x = menu->x + 10;
            edobj_bouncy_menu->y = menu->y + 40;
        }
    }
}


void edobjInit(void)
{
    struct eduistyle_s style;
    struct edobjinstance_s *oi;
    struct numtl_s *mtl;
    struct numtl_s *zoff;
    int i;

    style = edobj_item_style;
    oi = ObjectInstance;
    for (i = 0; i < 64; i++) {
        ObjectPath[i].instance = -1;
        oi[i].id = -1;
    }
    edobj_next_instance = 0;
    mtl = NuMtlCreate(1);
    edobj_mtl = mtl;
    mtl->attrib.cull = 2;
    mtl->attrib.zmode = 0;
    mtl->diffuse.r = 0.5f;
    mtl->diffuse.g = 0.5f;
    mtl->diffuse.b = 0.5f;
    mtl->alpha = 1.0f;
    mtl->attrib.alpha = 0;
    NuMtlUpdate(mtl);
    zoff = NuMtlCreate(1);
    edobj_mtl_zoff = zoff;
    zoff->attrib.cull = 2;
    zoff->attrib.zmode = 3;
    zoff->diffuse.r = 0.5f;
    zoff->diffuse.g = 0.5f;
    zoff->diffuse.b = 0.5f;
    zoff->alpha = 1.0f;
    zoff->attrib.alpha = 0;
    NuMtlUpdate(zoff);
    edobj_instance_type = -1;
    edobj_waypoint_mode = 0;
    edobj_options_menu = eduiMenuCreate(70, 70, 220, 300, app_fnt, edobjcbCancelOptMenu,
                                        options_menu_title);
    if (edobj_options_menu != 0) {
        eduiMenuAddItem(edobj_options_menu,
                        eduiItemSelCreate(1, &style, 0, 0, edobjcbInstanceMenu,
                                          instance_menu_title));
        eduiMenuAddItem(edobj_options_menu,
                        eduiItemSelCreate(1, &style, 0, 0, edobjcbAnimPropertiesMenu,
                                          options_text_animprop));
        eduiMenuAddItem(edobj_options_menu,
                        eduiItemSelCreate(1, &style, 0, 0, edobjcbWayPropertiesMenu,
                                          options_text_wayprop));
        eduiMenuAddItem(edobj_options_menu,
                        eduiItemSelCreate(1, &style, 0, 0, edobjcbParticleMenu,
                                          options_text_particles));
        eduiMenuAddItem(edobj_options_menu,
                        eduiItemSelCreate(1, &style, 0, 0, edobjcbSoundMenu,
                                          options_text_sounds));
        eduiMenuAddItem(edobj_options_menu,
                        eduiItemSelCreate(1, &style, 0, 0, edobjcbBouncyMenu,
                                          options_text_bouncy));
        eduiMenuAddItem(edobj_options_menu,
                        eduiItemSelCreate(1, &style, 0, 0, edobjcbFileSaveObjects,
                                          options_text_save));
        eduiMenuAddItem(edobj_options_menu,
                        eduiItemSelCreate(1, &style, 0, 0, edobjcbFileLoadObjects,
                                          options_text_load));
        eduiMenuAddItem(edobj_options_menu,
                        eduiItemSelCreate(1, &style, 0, 0, edobjcbResetAnims,
                                          options_text_reset_anims));
        eduiMenuAddItem(edobj_options_menu,
                        eduiItemToggleCreate(0, &style, edobj_snap_y, 1, edobjcbSnapYToggle,
                                             options_text_snap_y));
        eduiMenuAddItem(edobj_options_menu,
                        eduiItemToggleCreate(0, &style, edobj_snap_xz, 2, edobjcbSnapXZToggle,
                                             options_text_snap_xz));
    }
}


void edobjRenderSoundEmitters(int obj)
{
    int colour;
    int i;

    for (i = 0; i < ObjectPath[obj].num_sounds; i++) {
        if (ObjectPath[obj].sound_id[i] != -1) {
            colour = -1;
            if (edobj_sound_mode && edobj_nearest_sound == i) {
                colour = 0xFFFF0000;
            }
            edbitsDrawDiagonalCross(ObjectPath[obj].waypoints[0].x + ObjectPath[obj].sound_pos[i].x,
                                    ObjectPath[obj].waypoints[0].y + ObjectPath[obj].sound_pos[i].y,
                                    ObjectPath[obj].waypoints[0].z + ObjectPath[obj].sound_pos[i].z,
                                    0.25f, colour, edobj_mtl_zoff);
        }
    }
}


void edobjRenderParticleEmitters(int obj)
{
    struct debtypedef_s *type;
    int colour;
    int i;

    for (i = 0; i < ObjectPath[obj].num_particles; i++) {
        if (ObjectPath[obj].particle_type[i] != -1) {
            if (edobj_particle_mode && edobj_nearest_particle == i) {
                type = debtab[ObjectPath[obj].particle_type[i]];
                if (type->gensort == 0) {
                    edbitsDrawCube(
                        ObjectPath[obj].waypoints[0].x + ObjectPath[obj].particle_pos[i].x,
                        ObjectPath[obj].waypoints[0].y + ObjectPath[obj].particle_pos[i].y,
                        ObjectPath[obj].waypoints[0].z + ObjectPath[obj].particle_pos[i].z,
                        type->varstart.x, type->varstart.y, type->varstart.z, edobj_emitrotz,
                        edobj_emitroty, 0, 0, 0xFF008000, edobj_mtl_zoff);
                }
                colour = 0xFF00FF00;
            } else {
                colour = -1;
            }
            edbitsDrawDiagonalCross(
                ObjectPath[obj].waypoints[0].x + ObjectPath[obj].particle_pos[i].x,
                ObjectPath[obj].waypoints[0].y + ObjectPath[obj].particle_pos[i].y,
                ObjectPath[obj].waypoints[0].z + ObjectPath[obj].particle_pos[i].z, 0.25f, colour,
                edobj_mtl_zoff);
        }
    }
}


void edobjRenderWaypoints(int obj)
{
    struct nuvtx_tc1_s line[2];
    int colour;
    int i;

    for (i = 0; i < ObjectPath[obj].num_waypoints; i++) {
        colour = -1;
        if (edobj_waypoint_mode && edobj_nearest_waypoint == i) {
            colour = 0xFF0000FF;
        }
        edbitsDrawDiagonalCross(ObjectPath[obj].waypoints[i].x, ObjectPath[obj].waypoints[i].y,
                                ObjectPath[obj].waypoints[i].z, 0.25f, colour, edobj_mtl_zoff);
        if (i > 0) {
            line[0].pnt.x = ObjectPath[obj].waypoints[i - 1].x;
            line[0].pnt.y = ObjectPath[obj].waypoints[i - 1].y;
            line[0].pnt.z = ObjectPath[obj].waypoints[i - 1].z;
            line[0].diffuse = -1;
            line[1].pnt.x = ObjectPath[obj].waypoints[i].x;
            line[1].pnt.y = ObjectPath[obj].waypoints[i].y;
            line[1].pnt.z = ObjectPath[obj].waypoints[i].z;
            line[1].diffuse = -1;
            NuRndrLine3d(line, edobj_mtl_zoff, 0);
        }
    }
}


void edobjRender(void)
{
    int colour;

    edcamSet();
    edobjDrawCursor();
    if (edobj_nearest != -1) {
        if (edobj_waypoint_mode) {
            colour = 0xFF0000FF;
        } else if (edobj_copy_mode) {
            colour = 0xFF00D0FF;
        } else if (edobj_particle_mode) {
            colour = 0xFF00FF00;
        } else if (edobj_sound_mode) {
            colour = 0xFFFF0000;
        } else {
            colour = -1;
        }
        edbitsDrawCube(ObjectInstance[edobj_nearest].mtx._30, ObjectInstance[edobj_nearest].mtx._31,
                       ObjectInstance[edobj_nearest].mtx._32, 0.5f, 0.5f, 0.5f, 0, 0, 0, 0, colour,
                       edobj_mtl_zoff);
        if (!edobj_copy_mode && !edobj_particle_mode && !edobj_sound_mode) {
            edobjRenderWaypoints(edobj_nearest);
        }
        if (edobj_particle_mode) {
            edobjRenderParticleEmitters(edobj_nearest);
        }
        if (edobj_sound_mode) {
            edobjRenderSoundEmitters(edobj_nearest);
        }
    }
    if (edobj_active_menu != 0) {
        NuFntScale(12, 12);
        eduiMenuRender(edobj_active_menu);
    }
}


void edobjRegisterLevel(char *filename, int whatgame)
{
    edbitsRegisterLevel(filename, whatgame);
}


void edobjRegisterBaseScene(struct nugscn_s *scene)
{
    edobj_base_scene = scene;
}


void edobjObjectReset(void)
{
    struct edobjinstance_s *oi;
    int i;

    oi = ObjectInstance;
    for (i = 0; i < 64; i++) {
        ObjectPath[i].instance = -1;
        oi[i].id = -1;
    }
    edobj_next_instance = 0;
}


int edobjLookupInstance(char *name)
{
    int i;

    if (edobj_base_scene != 0) {
        for (i = 0; i < edobj_base_scene->numspecial; i++) {
            if (strncmp(edobj_base_scene->specials[i].name, name, 19) == 0) {
                return i;
            }
        }
    }
    return -1;
}


void edobjObjectDestroyAll(void)
{
    int i;

    for (i = 0; i < 64; i++) {
        edobjObjectDestroy(i);
    }
}


void edobjcbResetAnims(void *menu, struct edobjitem_s *item)
{
    edobjResetAnimsToZero();
}


void edobjcbCancelOptMenu(void)
{
    edobj_active_menu = 0;
}


void edobjcbFileSaveObjects(void *menu)
{
    char filename[256];

    if (edbits_what_game == 1) {
        sprintf(filename, edobj_objects_dat_fmt, edbits_level_filename);
        if (edobjFileSaveObjects(filename)) {
            eduiCreateMessageMenu(menu, edobj_save_ok_msg, 1);
        } else {
            eduiCreateMessageMenu(menu, edobj_save_fail_msg, 0);
        }
    } else if (edbits_what_game == 2) {
        sprintf(filename, edobj_obj_file_fmt, edbits_level_filename);
        if (edobjFileSaveObjects(filename)) {
            eduiCreateMessageMenu(menu, edobj_save_ok_msg, 1);
        } else {
            eduiCreateMessageMenu(menu, edobj_save_fail_msg, 0);
        }
    } else {
        eduiCreateMessageMenu(menu, edobj_save_fail_msg, 0);
    }
}


void edobjcbFileLoadObjects(void *menu)
{
    char filename[256];

    edobjObjectReset();
    if (edbits_what_game == 1) {
        sprintf(filename, edobj_objects_dat_fmt, edbits_level_filename);
        if (NuFileSize(filename) > 0) {
            edobjFileLoadObjects(filename);
        }
        eduiCreateMessageMenu(menu, edobj_load_ok_msg, 1);
    } else if (edbits_what_game == 2) {
        sprintf(filename, edobj_obj_file_fmt, edbits_level_filename);
        if (NuFileSize(filename) > 0) {
            edobjFileLoadObjects(filename);
        }
        eduiCreateMessageMenu(menu, edobj_load_ok_msg, 1);
    } else {
        eduiCreateMessageMenu(menu, edobj_load_fail_msg, 0);
    }
}


void edobjcbChangeAnimSpeed(void *menu, struct edobjitem_s *item)
{
    if (edobj_nearest != -1) {
        ObjectPath[edobj_nearest].anim_speed = item->fvalue;
        edobjConvertPathToAnim(edobj_nearest);
    }
}


void edobjcbChangeAnimPause(void *menu, struct edobjitem_s *item)
{
    if (edobj_nearest != -1) {
        ObjectPath[edobj_nearest].anim_pause = item->fvalue;
        edobjConvertPathToAnim(edobj_nearest);
    }
}


void edobjcbChangeAnimStartOffset(void *menu, struct edobjitem_s *item)
{
    if (edobj_nearest != -1) {
        ObjectPath[edobj_nearest].anim_start_offset = item->fvalue;
    }
}


void edobjcbOscillateToggle(void *menu, struct edobjitem_s *item)
{
    if (edobj_nearest != -1) {
        ObjectPath[edobj_nearest].oscillate = item->toggle;
        edobjConvertPathToAnim(edobj_nearest);
    }
}


void edobjcbSnapYToggle(void *menu, struct edobjitem_s *item)
{
    int on;

    on = item->toggle;
    edobj_snap_y = on;
    if (on) {
        edobj_snap_vec = edobj_cam_pos;
    } else {
        edcamSetPos(&edobj_cam_pos);
    }
}


void edobjcbSnapXZToggle(void *menu, struct edobjitem_s *item)
{
    int on;

    on = item->toggle;
    edobj_snap_xz = on;
    if (on) {
        edobj_snap_vec = edobj_cam_pos;
    } else {
        edcamSetPos(&edobj_cam_pos);
    }
}


void edobjcbToggleParticleSwitch(void *menu, struct edobjitem_s *item)
{
    if (edobj_nearest != -1) {
        ObjectPath[edobj_nearest].particle_switch[edobj_nearest_particle] = item->toggle;
    }
}


void edobjcbChangeWaypointSpeed(void *menu, struct edobjitem_s *item)
{
    if (edobj_waypoint_mode) {
        if (edobj_nearest_waypoint != -1) {
            ObjectPath[edobj_nearest].waypoint_speed[edobj_nearest_waypoint] = item->fvalue;
            edobjConvertPathToAnim(edobj_nearest);
        }
    }
}


void edobjcbChangeWaypointXRot(void *menu, struct edobjitem_s *item)
{
    if (edobj_waypoint_mode) {
        if (edobj_nearest_waypoint != -1) {
            ObjectPath[edobj_nearest].waypoint_rot[edobj_nearest_waypoint].x =
                item->fvalue * D_0062CF24;
            edobjConvertPathToAnim(edobj_nearest);
        }
    }
}


void edobjcbChangeWaypointYRot(void *menu, struct edobjitem_s *item)
{
    if (edobj_waypoint_mode) {
        if (edobj_nearest_waypoint != -1) {
            ObjectPath[edobj_nearest].waypoint_rot[edobj_nearest_waypoint].y =
                item->fvalue * D_0062CF28;
            edobjConvertPathToAnim(edobj_nearest);
        }
    }
}


void edobjcbChangeWaypointZRot(void *menu, struct edobjitem_s *item)
{
    if (edobj_waypoint_mode) {
        if (edobj_nearest_waypoint != -1) {
            ObjectPath[edobj_nearest].waypoint_rot[edobj_nearest_waypoint].z =
                item->fvalue * D_0062CF2C;
            edobjConvertPathToAnim(edobj_nearest);
        }
    }
}


void edobjcbChangeWaypointTime(void *menu, struct edobjitem_s *item)
{
    int time;
    int old;

    if (edobj_waypoint_mode) {
        if (edobj_nearest_waypoint != -1) {
            time = item->fvalue;
            old = ObjectPath[edobj_nearest].waypoint_time[edobj_nearest_waypoint];
            ObjectPath[edobj_nearest].waypoint_time[edobj_nearest_waypoint] = time;
            ObjectPath[edobj_nearest].total_time += time - old;
            edobjConvertPathToAnim(edobj_nearest);
        }
    }
}


void edobjcbSetInstanceType(void *menu, struct edobjitem_s *item)
{
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
    edobj_instance_menu = 0;
    edobj_instance_type = item->value;
    edobj_active_menu = 0;
}


void edobjcbSetSwitchType(void *menu, struct edobjitem_s *item)
{
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
    ObjectPath[edobj_nearest].switch_type = item->value;
    edobj_switchtype_menu = 0;
}


void edobjcbSetSwitchId(void *menu, struct edobjitem_s *item)
{
    if (edobj_nearest != -1) {
        ObjectPath[edobj_nearest].switch_id = item->fvalue;
    }
}


void edobjcbSetSwitchDelay(void *menu, struct edobjitem_s *item)
{
    if (edobj_nearest != -1) {
        ObjectPath[edobj_nearest].switch_delay = item->fvalue;
    }
}


void edobjcbSetSwitchVar(void *menu, struct edobjitem_s *item)
{
    if (edobj_nearest != -1) {
        ObjectPath[edobj_nearest].switch_var = item->fvalue;
    }
}


void edobjcbCancelInstanceMenu(void)
{
    eduiMenuDestroy(edobj_instance_menu);
    edobj_instance_menu = 0;
}


void edobjcbCancelSwitchTypeMenu(void)
{
    eduiMenuDestroy(edobj_switchtype_menu);
    edobj_switchtype_menu = 0;
}


void edobjcbCancelSwitchMenu(void)
{
    eduiMenuDestroy(edobj_switch_menu);
    edobj_switch_menu = 0;
}


void edobjcbCancelAnimPropertiesMenu(void)
{
    eduiMenuDestroy(edobj_animprop_menu);
    edobj_animprop_menu = 0;
}


void edobjcbCancelWayPropertiesMenu(void)
{
    eduiMenuDestroy(edobj_wayprop_menu);
    edobj_wayprop_menu = 0;
}


void edobjcbSetBouncyPlayerGrav(void *menu, struct edobjitem_s *item)
{
    if (edobj_nearest != -1) {
        ObjectPath[edobj_nearest].bouncy_player_grav = item->fvalue;
        edobjConvertPathToAnim(edobj_nearest);
    }
}


void edobjcbSetBouncyTension(void *menu, struct edobjitem_s *item)
{
    if (edobj_nearest != -1) {
        ObjectPath[edobj_nearest].bouncy_tension = item->fvalue;
        edobjConvertPathToAnim(edobj_nearest);
    }
}


void edobjcbSetBouncyDamping(void *menu, struct edobjitem_s *item)
{
    if (edobj_nearest != -1) {
        ObjectPath[edobj_nearest].bouncy_damping = item->fvalue;
        edobjConvertPathToAnim(edobj_nearest);
    }
}


void edobjcbToggleSoundType(void *menu, struct edobjitem_s *item)
{
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
    edobj_soundtype_menu = 0;
    if (item->toggle == 0) {
        ObjectPath[edobj_nearest].sound_type[edobj_nearest_sound] = 0;
        ObjectPath[edobj_nearest].sound_timing[edobj_nearest_sound] = 1.0f;
    } else {
        ObjectPath[edobj_nearest].sound_type[edobj_nearest_sound] = 1;
        ObjectPath[edobj_nearest].sound_timing[edobj_nearest_sound] = 50.0f;
    }
}


void edobjcbSetSoundTiming(void *menu, struct edobjitem_s *item)
{
    ObjectPath[edobj_nearest].sound_timing[edobj_nearest_sound] = item->fvalue;
}


void edobjcbSetParticleType(void *menu, struct edobjitem_s *item)
{
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
    edobj_particletype_menu = 0;
    if (item->value == 0) {
        edobj_particle_type = -1;
    } else {
        edobj_particle_type = item->value;
    }
}


void edobjcbSetParticleRate(void *menu, struct edobjitem_s *item)
{
    ObjectPath[edobj_nearest].particle_rate[edobj_nearest_particle] = item->fvalue;
}


void edobjcbLocalSoundTypeMenu(struct eduimenu_s *menu)
{
    struct eduistyle_s style;
    int selected;
    int i;

    style = edobj_item_style;
    edobj_localsoundtype_menu = eduiMenuCreate(70, 70, 180, 250, app_fnt,
                                               edobjcbCancelLocalSoundTypeMenu,
                                               localsoundtype_menu_title);
    if (edobj_localsoundtype_menu != 0) {
        for (i = 0; i < edSfxAllCount; i++) {
            selected = 0;
            if (ObjectPath[edobj_nearest].sound_id[edobj_nearest_sound] == i) {
                selected = 1;
            }
            eduiMenuAddItem(edobj_localsoundtype_menu,
                            eduiItemCheckCreate(i, &style, selected, 1, edobjcbSetLocalSoundType,
                                                edbitsGetSoundName(i)));
            if (selected == 1) {
                edobj_localsoundtype_menu->current = edui_last_item;
            }
        }
        eduiMenuAttach(menu, edobj_localsoundtype_menu);
        edobj_localsoundtype_menu->x = menu->x + 10;
        edobj_localsoundtype_menu->y = menu->y + 40;
    }
}


void edobjcbCancelLocalSoundMenu(void)
{
    eduiMenuDestroy(edobj_localsound_menu);
    edobj_localsound_menu = 0;
}


void edobjcbCancelParticleTypeMenu(void)
{
    eduiMenuDestroy(edobj_particletype_menu);
    edobj_particletype_menu = 0;
}


void edobjcbLocalParticleTypeMenu(struct eduimenu_s *menu)
{
    struct eduistyle_s style;
    int selected;
    int i;

    style = edobj_item_style;
    edobj_localparticletype_menu = eduiMenuCreate(70, 70, 250, 200, app_fnt,
                                                  edobjcbCancelLocalParticleTypeMenu,
                                                  localparticletype_menu_title);
    if (edobj_localparticletype_menu != 0) {
        for (i = 1; i < 128; i++) {
            if (debtab[i] != 0) {
                selected = 0;
                if (ObjectPath[edobj_nearest].particle_type[edobj_nearest_particle] == i) {
                    selected = 1;
                }
                eduiMenuAddItem(edobj_localparticletype_menu,
                                eduiItemCheckCreate(i, &style, selected, 1,
                                                    edobjcbSetLocalParticleType, debtab[i]->name));
                if (selected == 1) {
                    edobj_localparticletype_menu->current = edui_last_item;
                }
            }
        }
        eduiMenuAttach(menu, edobj_localparticletype_menu);
        edobj_localparticletype_menu->x = menu->x + 10;
        edobj_localparticletype_menu->y = menu->y + 40;
    }
}


void edobjcbCancelLocalParticleMenu(void)
{
    eduiMenuDestroy(edobj_localparticle_menu);
    edobj_localparticle_menu = 0;
}


void edobjcbParticleMenu(struct eduimenu_s *menu)
{
    struct eduistyle_s style;

    style = edobj_item_style;
    if (edobj_nearest != -1) {
        edobj_particle_menu = eduiMenuCreate(70, 70, 250, 300, app_fnt,
                                             edobjcbCancelParticleMenu, particle_menu_title);
        if (edobj_particle_menu != 0) {
            eduiMenuAddItem(edobj_particle_menu,
                            eduiItemSelCreate(1, &style, 0, 0, edobjcbParticleTypeMenu,
                                              particle_menu_text_type));
            if (edobj_nearest_particle != -1) {
                eduiMenuAddItem(edobj_particle_menu,
                                eduiItemSelCreate(1, &style, 0, 0, edobjcbLocalParticleMenu,
                                                  particle_menu_text_local));
            }
            eduiMenuAttach(menu, edobj_particle_menu);
            edobj_particle_menu->x = menu->x + 10;
            edobj_particle_menu->y = menu->y + 40;
        }
    }
}


void edobjcbSoundMenu(struct eduimenu_s *menu)
{
    struct eduistyle_s style;

    style = edobj_item_style;
    if (edobj_nearest != -1) {
        edobj_sound_menu = eduiMenuCreate(70, 70, 250, 300, app_fnt,
                                          edobjcbCancelSoundMenu, sound_menu_title);
        if (edobj_sound_menu != 0) {
            eduiMenuAddItem(edobj_sound_menu,
                            eduiItemSelCreate(1, &style, 0, 0, edobjcbSoundTypeMenu,
                                              sound_menu_text_type));
            if (edobj_nearest_sound != -1) {
                eduiMenuAddItem(edobj_sound_menu,
                                eduiItemSelCreate(1, &style, 0, 0, edobjcbLocalSoundMenu,
                                                  sound_menu_text_local));
            }
            eduiMenuAttach(menu, edobj_sound_menu);
            edobj_sound_menu->x = menu->x + 10;
            edobj_sound_menu->y = menu->y + 40;
        }
    }
}


void edobjcbCancelBouncyMenu(void)
{
    eduiMenuDestroy(edobj_bouncy_menu);
    edobj_bouncy_menu = 0;
}


void edobjClose(void)
{
    eduiMenuDestroy(edobj_options_menu);
}


void edobjEnter(void)
{
    edobj_nearest = -1;
    edobj_waypoint_mode = 0;
    edobj_copy_mode = 0;
    edobj_particle_mode = 0;
    edobj_sound_mode = 0;
    edobj_particle_type = -1;
    edobj_sound_type = -1;
    edobj_snap_xz = 0;
    edobj_snap_y = 0;
}


int edobjProc(struct pad_s *pad)
{
    edobj_blink_timer += 5;
    if (edobj_active_menu != 0) {
        eduiMenuProcess(edobj_active_menu, pad);
        return 0;
    }
    edobjDoInput(pad);
    if (edobj_waypoint_mode) {
        edobjDetermineNearestWaypoint(1.0f);
    } else if (edobj_particle_mode) {
        edobjDetermineNearestParticle(1.0f);
    } else if (edobj_sound_mode) {
        edobjDetermineNearestSound(1.0f);
    } else if (!edobj_copy_mode) {
        edobjDetermineNearestObject(1.0f);
    }
    if (pad->buttons & 0x800) {
        edobj_waypoint_mode = 0;
        edobj_copy_mode = 0;
        edobj_particle_mode = 0;
        edobj_sound_mode = 0;
        return 1;
    }
    return 0;
}


void edobjcbSetLocalSoundType(void *menu, struct edobjitem_s *item)
{
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
    edobj_localsoundtype_menu = 0;
    ObjectPath[edobj_nearest].sound_id[edobj_nearest_sound] = item->value;
    strcpy(ObjectPath[edobj_nearest].sound_name[edobj_nearest_sound],
           edbitsGetSoundName(item->value));
    edobj_sound_type = item->value;
}


void edobjcbSetLocalParticleType(void *menu, struct edobjitem_s *item)
{
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
    edobj_localparticletype_menu = 0;
    ObjectPath[edobj_nearest].particle_type[edobj_nearest_particle] = item->value;
    strcpy(ObjectPath[edobj_nearest].particle_name[edobj_nearest_particle],
           debtab[item->value]->name);
    edobj_particle_type = item->value;
}


void edobjcbSoundTypeMenu(struct eduimenu_s *menu)
{
    struct eduistyle_s style;
    int selected;
    int i;

    style = edobj_item_style;
    edobj_soundtype_menu = eduiMenuCreate(70, 70, 180, 250, app_fnt, edobjcbCancelSoundTypeMenu,
                                          soundtype_menu_title);
    if (edobj_soundtype_menu != 0) {
        eduiMenuAddItem(edobj_soundtype_menu,
                        eduiItemCheckCreate(99999, &style, edobj_sound_type == -1, 0,
                                            edobjcbSetSoundType, particletype_text_none));
        for (i = 0; i < edSfxAllCount; i++) {
            selected = 0;
            if (edobj_sound_type == i) {
                selected = 1;
            }
            eduiMenuAddItem(edobj_soundtype_menu,
                            eduiItemCheckCreate(i, &style, selected, 1, edobjcbSetSoundType,
                                                edbitsGetSoundName(i)));
            if (selected == 1) {
                edobj_soundtype_menu->current = edui_last_item;
            }
        }
        eduiMenuAttach(menu, edobj_soundtype_menu);
        edobj_soundtype_menu->x = menu->x + 10;
        edobj_soundtype_menu->y = menu->y + 40;
    }
}


void edobjcbCancelLocalSoundTypeMenu(void)
{
    eduiMenuDestroy(edobj_localsoundtype_menu);
    edobj_localsoundtype_menu = 0;
}


void edobjcbCancelLocalParticleTypeMenu(void)
{
    eduiMenuDestroy(edobj_localparticletype_menu);
    edobj_localparticletype_menu = 0;
}


void edobjcbCancelParticleMenu(void)
{
    eduiMenuDestroy(edobj_particle_menu);
    edobj_particle_menu = 0;
}


void edobjcbCancelSoundMenu(void)
{
    eduiMenuDestroy(edobj_sound_menu);
    edobj_sound_menu = 0;
}


void edobjcbSetSoundType(void *menu, struct edobjitem_s *item)
{
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
    edobj_soundtype_menu = 0;
    if (item->value == 99999) {
        edobj_sound_type = -1;
    } else {
        edobj_sound_type = item->value;
    }
}


void edobjcbCancelSoundTypeMenu(void)
{
    eduiMenuDestroy(edobj_soundtype_menu);
    edobj_soundtype_menu = 0;
}
