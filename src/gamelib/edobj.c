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

struct nuinstance_s {
    char pad00[0x50];
};

struct nuspecial_s {
    char pad00[0x40];
    struct nuinstance_s *instance;
    char pad44[0x50 - 0x44];
};

struct nugscn_s {
    char pad00[0x18];
    int numinstance;
    struct nuinstance_s *instances;
    char pad20[0x24 - 0x20];
    struct nuspecial_s *specials;
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
    char pad014[0x24 - 0x14];
    int anim_start_offset;
    char pad028[0x2C - 0x28];
    struct nuvec_s waypoints[8];
    float waypoint_speed[8];
    struct nuvec_s waypoint_rot[8];
    char pad10c[0x12C - 0x10C];
    int switch_type;
    int switch_id;
    float switch_var;
    float switch_delay;
    char pad13c[0x1DC - 0x13C];
    int particle_rate[8];
    int particle_switch[8];
    char pad21c[0x340 - 0x21C];
    int sound_type[8];
    float sound_timing[8];
    struct nuvec_s sound_pos[8];
    float bouncy_player_grav;
    float bouncy_tension;
    float bouncy_damping;
};

struct edobjinstance_s {
    char pad00[0x30];
    struct nuvec_s pos;
    char pad3c[0x40 - 0x3C];
    int id;
    char pad44[0x50 - 0x44];
};

extern struct edobject_s ObjectPath[];
extern struct edobjinstance_s ObjectInstance[];

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
extern void *edobj_particle_menu;
extern void *edobj_localparticletype_menu;
extern void *edobj_sound_menu;
extern void *edobj_soundtype_menu;
extern int edobj_nearest_waypoint;
extern int edobj_instance_type;
extern struct nuvec_s edobj_snap_vec;
extern struct nuvec_s edobj_cam_pos;
extern float D_0062CF24;
extern float D_0062CF28;

extern void edbitsRegisterLevel(char *filename, int whatgame);
extern void eduiMenuDestroy(void *menu);
extern void eduiMenuDetach(void *menu);
extern void edobjObjectDestroy(int instance);
extern void edobjConvertPathToAnim(int instance);
extern void edcamSetPos(struct nuvec_s *pos);
extern void NuVecSub(struct nuvec_s *dst, struct nuvec_s *a, struct nuvec_s *b);
extern float NuVecDist(struct nuvec_s *a, struct nuvec_s *b, void *c);
extern struct nuvec_s *edmainQueryLocVec(void);


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


void edobjObjectDestroyAll(void)
{
    int i;

    for (i = 0; i < 64; i++) {
        edobjObjectDestroy(i);
    }
}


float edobjPlayerObjectDistance(int obj)
{
    if (edmainQueryLocVec() == 0) {
        return 0.0f;
    }
    return NuVecDist(&ObjectInstance[obj].pos, edmainQueryLocVec(), 0);
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


void edobjWaypointPlace(int index, struct nuvec_s *wpos)
{
    ObjectPath[edobj_nearest].waypoints[index] = *wpos;
    edobjConvertPathToAnim(edobj_nearest);
}


void edobjcbCancelOptMenu(void)
{
    edobj_active_menu = 0;
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
