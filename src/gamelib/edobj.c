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

struct camera_s {
    struct numtx_s mtx;
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
    char pad5d[3];
};

struct nuanimcurveset_s;

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
    char pad000[0x564];
    unsigned int buttons;
};

struct nuinstance_s {
    char pad00[0x50];
};

struct nuspecial_s {
    char pad00[0x40];
    struct nuinstance_s *instance;
    char *name;
    char pad48[0x50 - 0x48];
};

struct nugobj_s;

struct nugscn_s {
    char pad00[0x14];
    struct nugobj_s **gobjs;
    int numinstance;
    struct nuinstance_s *instances;
    int numspecial;
    struct nuspecial_s *specials;
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
    char pad00c[0x10 - 0x0C];
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

struct nuinstflags_s {
    int visible : 1;
    int onscreen : 1;
    int visitest : 1;
    int isanimated : 1;
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

extern struct nugscn_s *edobj_base_scene;
extern void *edobj_active_menu;
extern void *edobj_instance_menu;
extern void *edobj_switchtype_menu;
extern void *edobj_switch_menu;
extern void *edobj_animprop_menu;
extern void *edobj_wayprop_menu;
extern void *edobj_localsound_menu;
extern void *edobj_localsoundtype_menu;
extern void *edobj_particletype_menu;
extern void *edobj_localparticle_menu;
extern void *edobj_bouncy_menu;
extern void *edobj_options_menu;
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
extern void *edobj_localparticletype_menu;
extern struct eduimenu_s *edobj_sound_menu;
extern void *edobj_soundtype_menu;
extern int edobj_nearest_waypoint;
extern int edobj_instance_type;
extern struct nuvec_s edobj_snap_vec;
extern struct nuvec_s edobj_cam_pos;
extern unsigned short edobj_emitrotz;
extern unsigned short edobj_emitroty;
extern struct camera_s global_camera;
extern char *debtab[];
extern float D_0062CF24;
extern float D_0062CF28;
extern float D_0062CF2C;
extern int D_0062F57C;
extern float D_0062F588;

#define edobj_blink_timer D_0062F57C
#define edobj_time D_0062F588

extern signed char edbits_what_game;
extern char edbits_level_filename[];
extern void *edobj_mtl_zoff;
extern char D_00618D80[];
extern char D_00618D90[];
extern char D_00618DA0[];
extern char D_00618DB0[];
extern char D_00618DC0[];
extern char D_0062F598[];
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
#define edobj_item_style D_00618DD0
#define particle_menu_title D_006190B0
#define particle_menu_text_type D_006190C8
#define particle_menu_text_local D_006190E0
#define sound_menu_title D_00619100
#define sound_menu_text_type D_00619110
#define sound_menu_text_local D_00619120

extern void edbitsRegisterLevel(char *filename, int whatgame);
extern void eduiMenuDestroy(void *menu);
extern void eduiMenuDetach(void *menu);
extern void edobjObjectDestroy(int instance);
extern void edobjConvertPathToAnim(int instance);
extern void edcamSetPos(struct nuvec_s *pos);
extern void NuVecSub(struct nuvec_s *dst, struct nuvec_s *a, struct nuvec_s *b);
extern float NuVecDist(struct nuvec_s *a, struct nuvec_s *b, void *c);
extern struct nuvec_s *edmainQueryLocVec(void);
extern float NuFsqrt(float x);
extern int strncmp(const char *a, const char *b, int n);
extern char *strcpy(char *dst, const char *src);
extern char *edbitsGetSoundName(int id);
extern void eduiMenuProcess(void *menu, struct pad_s *pad);
extern void edobjDoInput(struct pad_s *pad);
extern void edobjDetermineNearestObject(float range);
extern void edobjDetermineNearestWaypoint(float range);
extern void edobjDetermineNearestParticle(float range);
extern void edobjDetermineNearestSound(float range);
extern void NuAnimDataDestroy(struct nuanimdata_s *animdata);
extern int DeletePlatinst(int id);
extern void *memset(void *dst, int c, unsigned int n);
extern int sprintf(char *buf, const char *fmt, ...);
extern int NuFileSize(char *name);
extern void edobjObjectReset(void);
extern int edobjFileSaveObjects(char *filename);
extern void edobjFileLoadObjects(char *filename);
extern void eduiCreateMessageMenu(void *menu, char *msg, int flag);
extern void edcamSet(void);
extern void edobjDrawCursor(void);
extern void edbitsDrawCube(float x, float y, float z, float sizex, float sizey, float sizez,
                           int rotx, int roty, int rotz, int flags, int colour, void *mtl);
extern void edobjRenderWaypoints(int obj);
extern void edobjRenderParticleEmitters(int obj);
extern void edobjRenderSoundEmitters(int obj);
extern void NuFntScale(int x, int y);
extern void eduiMenuRender(void *menu);
extern int NuRndrGScnObj(struct nugobj_s *gobj, struct numtx_s *mtx);
extern unsigned char object_switches[];
extern struct numtx_s numtx_identity;
extern int PlatImpactInfo(struct nuvec_s *norm, int *info0, int *info1);
extern void NuMtxSetIdentity(struct numtx_s *m);
extern void NuMtxTranslate(struct numtx_s *m, struct nuvec_s *v);
extern void NuMtxRotateZ(struct numtx_s *m, int angle);
extern void NuMtxRotateY(struct numtx_s *m, int angle);
extern void NuMtxMul(struct numtx_s *dst, struct numtx_s *a, struct numtx_s *b);
extern void NuVecMtxTransform(struct nuvec_s *dst, struct nuvec_s *src, struct numtx_s *m);
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
extern int abs(int x);
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


inline float edobjPlayerObjectDistance(int obj)
{
    if (edmainQueryLocVec() != 0) {
        return NuVecDist(&ObjectInstance[obj].mtx.pos, edmainQueryLocVec(), 0);
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
        inst->mtx.pos.x = ObjectPath[i].waypoints[0].x;
        inst->mtx.pos.y = ObjectPath[i].waypoints[0].y;
        inst->mtx.pos.z = ObjectPath[i].waypoints[0].z;
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
                NuMtxTranslate(&anim->mtx, &inst->mtx.pos);
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

    dx = pos->x - global_camera.mtx.pos.x;
    dy = pos->y - global_camera.mtx.pos.y;
    dz = pos->z - global_camera.mtx.pos.z;
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
                if (edobjRenderCutoffTest(&anim->mtx.pos)) {
                    inst->flags.onscreen = NuRndrGScnObj(scene->gobjs[inst->id], &anim->mtx);
                }
            } else if (edobjRenderCutoffTest(&inst->mtx.pos)) {
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
        NuVecSub(&delta, &edobj_cam_pos, &ObjectInstance[edobj_nearest].mtx.pos);
        if (delta.x * delta.x + delta.y * delta.y + delta.z * delta.z == 0.0) {
            return;
        }
    }
    edobj_nearest = -1;
    for (i = 0; i < 64; i++) {
        if (ObjectInstance[i].id != -1) {
            NuVecSub(&delta, &edobj_cam_pos, &ObjectInstance[i].mtx.pos);
            dist = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
            if (range < 0.0f || dist < range) {
                edobj_nearest = i;
                range = dist;
            }
        }
    }
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
        edbitsDrawCube(ObjectInstance[edobj_nearest].mtx.pos.x, ObjectInstance[edobj_nearest].mtx.pos.y,
                       ObjectInstance[edobj_nearest].mtx.pos.z, 0.5f, 0.5f, 0.5f, 0, 0, 0, 0, colour,
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


void edobjResetAnimsToZero(void)
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


int edobjLookupInstanceIndex(int special)
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


void edobjSoundPlace(int slot, struct nuvec_s *wpos)
{
    NuVecSub(&ObjectPath[edobj_nearest].sound_pos[slot], wpos,
             &ObjectPath[edobj_nearest].waypoints[0]);
}


void edobjParticlePlace(int slot, struct nuvec_s *wpos)
{
    NuVecSub(&ObjectPath[edobj_nearest].particle_pos[slot], wpos,
             &ObjectPath[edobj_nearest].waypoints[0]);
    ObjectPath[edobj_nearest].particle_rotz[slot] = edobj_emitrotz;
    ObjectPath[edobj_nearest].particle_roty[slot] = edobj_emitroty;
}


void edobjWaypointPlace(int index, struct nuvec_s *wpos)
{
    ObjectPath[edobj_nearest].waypoints[index] = *wpos;
    edobjConvertPathToAnim(edobj_nearest);
}


void edobjObjectPlace(int obj, struct nuvec_s *pos)
{
    struct nuvec_s delta;
    int i;

    NuVecSub(&delta, &ObjectPath[obj].waypoints[0], pos);
    for (i = 0; i < ObjectPath[obj].num_waypoints; i++) {
        NuVecSub(&ObjectPath[obj].waypoints[i], &ObjectPath[obj].waypoints[i], &delta);
    }
    edobjConvertPathToAnim(obj);
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
           debtab[item->value]);
    edobj_particle_type = item->value;
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
