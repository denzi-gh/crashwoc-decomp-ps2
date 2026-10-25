/*
 * Unit: game/game_obj
 *
 * Functions:
 *   0x001f9270 CylinderCuboidOverlapXZ
 *   0x001f9558 ObjectCylinderCollision
 *   0x001f99e0 GameObjectOverlap
 *   0x001f9bc0 PlayerCreatureCollisions
 *   0x001fa998 GetTopBot
 *   0x001fab20 CrateCollisions
 *   0x001fbef0 WumpaCollisions
 *   0x001fc100 MoveLoopXZ
 *   0x001fc2e0 GetSurfaceInfo
 *   0x001fc788 ObjectRotation
 *   0x001fce88 PickupItem
 *   0x001fd158 HitCreatures
 *   0x001fd2e0 WipeCreatures
 *   0x001fd4a8 KillGameObject
 *   0x001fe0a0 AddProjectile
 *   0x001feeb0 UpdateProjectiles
 *   0x00200050 DrawProjectiles
 *   0x002005f8 CreatureTopBelow
 *   0x002007b0 CreatureRayCast
 *   0x00200ab0 ClearGameObjects
 *   0x00200ad8 ResetGameObject
 *   0x00200b38 AddGameObject
 *   0x00200c38 RemoveGameObject
 *   0x00200ca8 GameObjectRadius
 *   0x00200d30 CylinderCylinderOverlapXZ
 *   0x00200d80 FlyGameObject
 *   0x00200df8 GetDieAnim
 *   0x00200e90 KillPlayer
 *   0x00200f38 KillItem
 *   0x00200f58 PickupCrystal
 *   0x00200fb0 PickupCrateGem
 *   0x00201008 PickupBonusGem
 *   0x00201068 PickupPower
 *   0x00201140 HitItems
 *   0x00201200 NewTopBot
 *   0x00201230 OldTopBot
 *   0x00201248 FindAnglesZX
 *   0x002012a0 ScaleFlatShadow
 *   0x00201320 ResetProjectiles
 *   0x002013d8 CountGameObjects
 *   0x00201418 PickupRelic
 */

#include "nu_libc.h"

#include "nu3d/nugscn.h"
#include "nu3d/nulight.h"
#include "numath/numtx.h"
#include "numath/nutrig.h"
#include "numath/nuvec.h"
#include "game/gametypes.h"
#include "gamelib/terrain.h"
#include "game/creature.h"

extern struct ldata_s *LDATA;

extern f32 vtog_time;
extern f32 vtog_duration;
extern s32 vtog_blend;

void LoseMask(struct obj_s *obj);
s32 KillGameObject(struct obj_s *obj, s32 anim);

extern s32 level_part_2;

/* Projectile record; obj header at 0x0, active flag at 0x1A9, stride 0x1B0. */
struct projectile_s {
    struct obj_s obj;        /* 0x000 */
    struct nuvec_s srcpos;   /* 0x188 */
    struct nuvec_s dstpos;   /* 0x194 */
    f32 time;                /* 0x1A0 */
    f32 duration;            /* 0x1A4 */
    s8 type;                 /* 0x1A8 */
    s8 active;               /* 0x1A9 */
    short object;            /* 0x1AA */
    s8 kill;                 /* 0x1AC */
    u8 owner_safety;         /* 0x1AD */
    u8 unk_0x1AE[0x2];       /* 0x1AE */
};

struct tersurface_s TerSurface[16] = {
    { 1.0f, -1, 0x08 }, { 0.15f, -1, 0x28 }, { 1.0f, -1, 0x08 }, { 1.0f, -1, 0x01 },
    { 1.0f, -1, 0x02 }, { 1.0f, -1, 0x04 }, { 1.0f, -1, 0x00 }, { 1.0f, -1, 0x01 },
    { 1.0f, -1, 0x08 }, { 1.0f, -1, 0x10 }, { 1.0f, -1, 0x00 }, { 1.0f, -1, 0x00 },
    { 0.15f, -1, 0x28 }, { 0.15f, -1, 0x2C }, { 1.0f, -1, 0x04 }, { 1.0f, -1, 0x00 },
};

struct tersurface_s TerLayer[11] = {
    { 1.0f, -1, 0x00 }, { 2.0f, -1, 0x00 }, { 1.0f, -1, 0x00 }, { 3.0f, -1, 0x00 },
    { 3.0f, -1, 0x08 }, { 1.0f, -1, 0x40 }, { 1.0f, -1, 0x40 }, { 1.0f, -1, 0x40 },
    { 1.0f, -1, 0x40 }, { 1.0f, -1, 0x40 }, { 1.0f, -1, 0x40 },
};

/* indexed by new_power; character is the pickup type (see PickupPower) */
struct powerdata_s PData[6] = {
    { 0xA7, 0x67, -1, -1 },
    { 0xA5, 0x68, -1, -1 },
    { 0xA6, 0x69, -1, -1 },
    { 0xA2, 0x6A, -1, -1 },
    { 0xA4, 0x6B, -1, -1 },
    { 0xA3, 0x6C, -1, -1 },
};

struct nuvec_s vNEWMASK = { 0.0f, 0.0f, 0.0f };
struct obj_s *pObj[64] = { 0 };
struct projectile_s Projectile[16] = { 0 };
struct nuvec_s plr_conveyor_mom = { 0.0f, 0.0f, 0.0f };
struct nuvec_s jonglowpos = { 0.0f, 0.0f, 0.0f };

s32 newmask_advise = 0;
s32 boss_dead = 0;
s32 i_projectile = 0;
s32 jonglow = 0;
f32 jonwidth = 0.0f;
f32 jonwidth2 = 0.0f;
s32 FIZZLEVOL = 0xFFFF;
s32 GAMEOBJECTCOUNT = 0;
s32 new_power = 0;
f32 in_speed = 0.0f;
f32 in_f_friction = 0.0f;
f32 in_s_friction = 0.0f;
s32 mask_crates = 0;
s32 crates_destroyed = 0;
s32 crate_wumpa = 0;
u16 temp_xrot = 0;
u16 temp_yrot = 0;
u16 temp_zrot = 0;
f32 temp_cuboid_bounce_angle = 0.0f;
s32 temp_cuboid_side = 0;
s32 temp_xzmomset = 0;
u16 temp_roof_xrot = 0;
u16 temp_roof_zrot = 0;
u16 plr_conveyor_yrot = 0;
f32 jonglowsize = 0.0f;

extern s32 VEHICLECONTROL;
extern s32 gamesfx_effect_volume;
extern f32 NuTrigTable[];
extern struct nuvec_s v010;

extern struct nuvec_s v000;
extern f32 GRAVITY;
extern f32 D_005F28F8[];
#define terminal_fall_speed D_005F28F8[0]
s32 HitCrates(struct obj_s *obj, s32 destroy);
s32 HitWumpa(struct obj_s *obj, s32 destroy);
void AddKaboom(s32 type, struct nuvec_s *pos, f32 radius);
void AddVariableShotDebrisEffect(s32 type, struct nuvec_s *pos, s32 count, s16 rotz, s16 roty);

void NewTopBot(struct obj_s *obj);
s32 GameObjectOverlap(struct obj_s *a, struct obj_s *b, s32 find_angle);
void PickupItem(struct obj_s *obj);
s32 CylinderCuboidOverlapXZ(struct nuvec_s *pos, struct obj_s *obj,
                            struct nuvec_s *p2, f32 radius);
void AddGameDebris(s32 type, struct nuvec_s *pos);
void GameSfx(s32 sfx, struct nuvec_s *pos);
f32 NuFsqrt(f32 x);
s32 GetDieAnim(struct obj_s *obj, s32 anim);
void ObjectToAtlas(struct obj_s *obj, struct creature_s *player);

extern u16 plr_items;
extern struct panelcount_s plr_crystal;
extern struct panelcount_s plr_crategem;
extern struct panelcount_s plr_bonusgem;

void AddPanelDebris(f32 x, f32 y, s32 obj, f32 z, s32 flag);

extern struct game_s Game;
extern struct cursor_s Cursor;
extern s32 Level;
s32 qrand(void);
extern s32 new_mode;
extern s32 new_level;
extern u64 LBIT;
void ClockOff(void);
void StartTimeTrial(struct nuvec_s *pos, s32 clock);
void NewMenu(void *cursor, s32 menu, s32 mode, s32 item);

extern s32 TimeTrial;
extern struct wumpa_s Wumpa[320];
extern struct winfo_s WInfo[8];
extern struct gdeb_s GDeb[170];

void FlyWumpa(struct wumpa_s *wumpa);
void AddScreenWumpa(f32 x, f32 y, f32 z, s32 n);
void AddFiniteShotDebrisEffect(s32 *key, s32 effect, struct nuvec_s *pos, s32 n);


extern f32 SAFEY;
extern f32 EShadY;
extern f32 ShadRoofY;
extern struct nuvec_s ShadNorm;
extern struct nuvec_s EShadNorm;
extern struct nuvec_s ShadRoofNorm;

s32 RotDiff(u16 a, u16 b);
u16 SeekRot(u16 a, u16 target, s32 rate);
void GetALONG(struct nuvec_s *pos, RailPosition *rpos, s32 iRAIL, s32 iALONG, s32 dir);
s32 FurtherALONG(s32 iRAIL, s32 iALONG, f32 fALONG, s32 iRAIL2, s32 iALONG2, f32 fALONG2);
f32 CrateBottomAbove(struct nuvec_s *pos);
s32 RayIntersectCuboid(struct nuvec_s *p0, struct nuvec_s *p1, struct nuvec_s *min, struct nuvec_s *max);
extern f32 temp_ratio;

extern struct objtab_s ObjTab[];
extern struct cammtx_s *pCam;
extern s32 D_00588824[];
extern s32 Paused;
extern s32 FRAME;
extern s32 FRAMES;
extern struct cammtx_s GameCam[];
void JudderGameCamera(struct cammtx_s *cam, f32 time, struct nuvec_s *pos);
extern s32 InvincibilityCHEAT;
extern s32 bonus_crates_destroyed;
extern s32 old_bonus_crates;
extern f32 plr_invisibility_time;
extern s32 Hub;

void BlendGameCamera(struct cammtx_s *cam, f32 time);
void AddExtraLife(struct nuvec_s *pos, s32 pdeb);
void AddMechanicalDebris(struct nuvec_s *pos, s32 vehicle);

s32 NuRndrGScnObj(void *gobj, struct numtx_s *m);
#ifdef NW_HOST /* retail's prototype moves size last; the EE passes it in $f12 either way */
void NuRndrAddShadow(struct nuvec_s *pos, f32 size, s32 alpha, s32 xrot, s32 yrot, s32 zrot);
#define NuRndrAddShadow(pos, alpha, xrot, yrot, zrot, size) NuRndrAddShadow(pos, size, alpha, xrot, yrot, zrot)
#else
void NuRndrAddShadow(struct nuvec_s *pos, s32 alpha, s32 xrot, s32 yrot, s32 zrot, f32 size);
#endif
extern s32 temp_face;


s32 CylinderCuboidOverlapXZ(struct nuvec_s *pos, struct obj_s *obj,
                            struct nuvec_s *p2, f32 radius) {
    struct nuvec_s v;
    f32 minx;
    f32 maxx;
    f32 minz;
    f32 maxz;
    f32 x0;
    f32 x1;
    f32 z0;
    f32 z1;
    f32 cx;
    f32 cz;
    f32 dx;
    f32 dz;
    s32 corner;
    s32 ang;

    NuVecSub(&v, pos, p2);
    NuVecRotateY(&v, &v, -obj->hdg);
    minx = obj->min.x * obj->SCALE;
    x0 = minx - radius;
    if (v.x < x0) {
        return 0;
    }
    maxx = obj->max.x * obj->SCALE;
    x1 = maxx + radius;
    if (v.x > x1) {
        return 0;
    }
    minz = obj->min.z * obj->SCALE;
    z0 = minz - radius;
    if (v.z < z0) {
        return 0;
    }
    maxz = obj->max.z * obj->SCALE;
    z1 = maxz + radius;
    if (v.z > z1) {
        return 0;
    }

    corner = 0;
    if (v.x < minx) {
        if (v.z < minz) {
            corner = 10;
            cx = minx;
            cz = minz;
            temp_cuboid_side = (minz - v.z < minx - v.x) ? 0x20 : 0x40;
        } else if (v.z > maxz) {
            corner = 9;
            cx = minx;
            cz = maxz;
            temp_cuboid_side = (v.z - maxz < minx - v.x) ? 0x10 : 0x40;
        }
    } else if (v.x > maxx) {
        if (v.z < minz) {
            corner = 6;
            cx = maxx;
            cz = minz;
            temp_cuboid_side = (minz - v.z < v.x - maxx) ? 0x20 : 0x80;
        } else if (v.z > maxz) {
            corner = 5;
            cx = maxx;
            cz = maxz;
            temp_cuboid_side = (v.z - maxz < v.x - maxx) ? 0x10 : 0x80;
        }
    }

    if (corner != 0) {
        dx = v.x - cx;
        dz = v.z - cz;
        if (dx * dx + dz * dz > radius * radius) {
            return 0;
        }
        temp_cuboid_bounce_angle = NuAtan2D(dx, dz) + obj->hdg;
        return 1;
    }

    if (v.x - x0 < x1 - v.x) {
        dx = v.x - x0;
        if (v.z - z0 < z1 - v.z) {
            ang = (v.z - z0 < dx) ? 0x8000 : 0xC000;
        } else {
            ang = (z1 - v.z < dx) ? 0 : 0xC000;
        }
    } else {
        dx = x1 - v.x;
        if (v.z - z0 < z1 - v.z) {
            ang = (v.z - z0 < dx) ? 0x8000 : 0x4000;
        } else {
            ang = (z1 - v.z < dx) ? 0 : 0x4000;
        }
    }
    temp_cuboid_bounce_angle = ang + obj->hdg;
    if (ang == 0) {
        temp_cuboid_side = 0x10;
    } else if (ang == 0x4000) {
        temp_cuboid_side = 0x80;
    } else if (ang == 0x8000) {
        temp_cuboid_side = 0x20;
    } else {
        temp_cuboid_side = 0x40;
    }
    return 1;
}


s32 ObjectCylinderCollision(struct obj_s *obj, struct obj_s *cyl, f32 scale,
                            s32 flag) {
    f32 dx;
    f32 dz;
    f32 r;
    f32 dist;
    s32 side;
    s32 result;
    u16 a;
    f32 obj_top;
    f32 obj_bot;
    f32 cyl_top;
    f32 cyl_bot;

    dx = obj->pos.x - cyl->pos.x;
    dz = obj->pos.z - cyl->pos.z;
    r = obj->RADIUS + cyl->RADIUS;
    dist = dx * dx + dz * dz;
    if (flag != 0) {
        r *= scale;
    }
    if (dist > r * r) {
        if (flag == 0) {
            return 0;
        }
        a = NuAtan2D(dx, dz);
        obj->pos.x = cyl->pos.x + NuTrigTable[a] * r;
        obj->pos.z = cyl->pos.z + NuTrigTable[(u16)(a + 0x4000)] * r;
    }

    side = 0;
    obj_top = obj->top * obj->SCALE;
    if (obj->pos.y + obj_top < cyl->pos.y + cyl->bot * cyl->SCALE) {
        side = 1;
    } else {
        obj_bot = obj->bot * obj->SCALE;
        cyl_top = cyl->pos.y + cyl->top * cyl->SCALE;
        cyl_bot = cyl->pos.y + cyl->bot * cyl->SCALE;
        if (obj->pos.y + obj_bot > cyl_top) {
            side = 1;
        } else if (obj->oldpos.y + obj->bot * obj->old_SCALE >=
                   cyl->oldpos.y + cyl->top * cyl->old_SCALE - 0.012f) {
            obj->pos.y = cyl_top - obj_bot;
            obj->ground = 3;
            result = 2;
            obj->shadow = cyl->pos.y + cyl->top * cyl->SCALE;
            obj->got_shadow = 1;
            obj->surface_type = 0;
            obj->vSN = v010;
            obj->mom.y = cyl->mom.y;
            goto apply;
        } else if (obj->oldpos.y + obj->top * obj->old_SCALE <=
                   cyl->oldpos.y + cyl->bot * cyl->old_SCALE + 0.012f) {
            result = 4;
            obj->pos.y = cyl_bot - obj_top;
            obj->mom.y = cyl->mom.y - 0.012f;
            goto apply;
        }
    }

    if (side != 0) {
        f32 mid;
        f32 top;

        top = cyl->pos.y + cyl->top * cyl->SCALE;
        mid = obj->pos.y + (obj->bot + obj->top) * obj->SCALE * 0.5f;
        result = 0;
        if (top < mid) {
            if (obj->got_shadow == 0 || top > obj->shadow) {
                obj->shadow = top;
                obj->got_shadow = 1;
                obj->surface_type = 0;
                obj->vSN = v010;
            }
            result = 0;
        }
    } else {
        result = 1;
        a = NuAtan2D(obj->pos.x - cyl->pos.x, obj->pos.z - cyl->pos.z);
        obj->mom.x = NuTrigTable[a] * 0.06f;
        obj->mom.z = NuTrigTable[(u16)(a + 0x4000)] * 0.06f;
    }

apply:
    if (result & 6) {
        f32 new_x;
        f32 new_z;

        obj->vSN = v010;
        new_x = obj->pos.x + cyl->mom.x;
        obj->pos.x = new_x;
        new_z = obj->pos.z + cyl->mom.z;
        obj->pos.z = new_z;
        if (cyl->dyrot != 0) {
            if ((result & 2) || ((result & 4) && (cyl->flags & 0x200))) {
                f32 ox;
                f32 oz;
                f32 xoff;
                f32 zoff;

                obj->hdg += cyl->dyrot;
                dx = new_x - cyl->pos.x;
                dz = new_z - cyl->pos.z;
                a = NuAtan2D(dx, dz);
                dist = NuFsqrt(dx * dx + dz * dz);
                ox = NuTrigTable[a] * dist;
                oz = NuTrigTable[(u16)(a + 0x4000)] * dist;
                a += cyl->dyrot;
                xoff = NuTrigTable[a] * dist - ox;
                zoff = NuTrigTable[(u16)(a + 0x4000)] * dist - oz;
                obj->pos.x += xoff;
                obj->pos.z += zoff;
            }
        }
    }
    return result;
}


s32 GameObjectOverlap(struct obj_s *a, struct obj_s *b, s32 find_angle) {
    struct nuvec_s pa;
    struct nuvec_s pb;
    f32 dx;
    f32 dz;
    f32 r;

    if (a->flags & 0x2000) {
        if (a->draw_frame == 0 || a->pLOCATOR == 0 ||
            (a->model != 0 && a->model->pLOCATOR[0] == 0)) {
            return 0;
        }
    }
    if (b->flags & 0x2000) {
        if (b->draw_frame == 0 || b->pLOCATOR == 0 ||
            (b->model != 0 && b->model->pLOCATOR[0] == 0)) {
            return 0;
        }
    }
    if (a->flags & 0x2000) {
        pa.x = a->pLOCATOR->_30;
        pa.y = a->pLOCATOR->_31;
        pa.z = a->pLOCATOR->_32;
    } else {
        pa = a->pos;
    }
    if (b->flags & 0x2000) {
        pb.x = b->pLOCATOR->_30;
        pb.y = b->pLOCATOR->_31;
        pb.z = b->pLOCATOR->_32;
    } else {
        pb = b->pos;
    }
    if (pa.y + a->bot * a->SCALE > pb.y + b->top * b->SCALE) {
        return 0;
    }
    if (pb.y + b->bot * b->SCALE > pa.y + a->top * a->SCALE) {
        return 0;
    }
    dx = pb.x - pa.x;
    dz = pb.z - pa.z;
    r = a->RADIUS + b->RADIUS;
    if (dx * dx + dz * dz <= r * r) {
        if (find_angle) {
            temp_yrot = NuAtan2D(dx, dz);
        }
        return 1;
    }
    return 0;
}


void PlayerCreatureCollisions(struct obj_s *obj)
{
    struct obj_s *po;
    struct creature_s *killer;
    struct nuvec_s cpos;
    struct nuvec_s dpos;
    s32 i;
    s32 maskflag;
    s32 lose;
    s32 overlap;
    s32 is_cuboid;
    s32 side;
    s32 ctype;
    s32 above;
    s32 push;
    s32 bounce;
    s32 hitside;
    s32 ang;
    s32 die_anim;
    f32 reach;
    f32 radius;
    f32 dx;
    f32 dz;
    f32 dist;
    f32 shiftx;
    f32 shiftz;
    f32 oldx;
    f32 oldz;

    if (level_part_2 != 0) {
        return;
    }
    temp_xzmomset = 0;
    if (obj->dead != 0) {
        return;
    }
    maskflag = 0;
    if (obj->mask != 0 && obj->mask->active != 0) {
        maskflag = obj->mask->active;
        if (LDATA->flags & 0xE00) {
            maskflag = 0;
        }
    }
    lose = 0;
    for (i = 0; i < GAMEOBJECTCOUNT; i++) {
        po = pObj[i];
        if (po == 0) {
            continue;
        }
        po->touch = 0;
        po->contact = 0;
        po->kill_contact = 0;
        if (po == obj) {
            continue;
        }
        if (po->dead != 0) {
            continue;
        }
        if (po->invisible != 0) {
            continue;
        }
        if (po->draw_frame == 0) {
            continue;
        }
        if (!(po->flags & 0x4)) {
            continue;
        }
        if (po->flags & 0x1000) {
            continue;
        }
        NewTopBot(obj);
        if ((po->flags & 0x2000) && po->draw_frame == 0) {
            continue;
        }
        if ((po->flags & 0x2000) && po->pLOCATOR != 0 && po->model->pLOCATOR[0] != 0) {
            cpos.x = po->pLOCATOR->_30;
            cpos.y = po->pLOCATOR->_31;
            cpos.z = po->pLOCATOR->_32;
        } else {
            cpos = po->pos;
        }

        if (po->flags & 0x8000) {
            overlap = CylinderCuboidOverlapXZ(&obj->pos, po, &cpos, obj->RADIUS);
            is_cuboid = 1;
            side = temp_cuboid_side;
        } else {
            dx = obj->pos.x - cpos.x;
            dz = obj->pos.z - cpos.z;
            radius = obj->RADIUS + po->RADIUS;
            dist = dx * dx + dz * dz;
            overlap = (radius * radius < dist) ? 0 : 1;
            is_cuboid = 0;
            side = 8;
        }

        po->objbot = cpos.y + po->bot * po->SCALE;
        po->objtop = cpos.y + po->top * po->SCALE;

        if (!overlap) {
            if (!(po->attack & 0x2) &&
                (((obj->attack & 0x2) && (po->vulnerable & 0x2)) ||
                 (obj->character == 1 && (obj->attack & 0x10) && (po->vulnerable & 0x10)) ||
                 (obj->mom.y < 0.0f && (obj->attack & 0xC) && (po->vulnerable & 0xC)))) {
                if (obj->objtop < po->objbot || obj->objbot > po->objtop) {
                    continue;
                }
                reach = obj->radius * obj->SCALE;
                if (obj->attack & 0x8) {
                    reach += 0.5f;
                } else if (obj->attack & 0x4) {
                    reach += 0.25f;
                } else if (obj->attack & 0x2) {
                    if (obj->character == 1 && ((struct creature_s *)obj->parent)->slide != 0) {
                        reach *= 3.0f;
                    } else {
                        reach += reach;
                    }
                }
                if (is_cuboid) {
                    if (!CylinderCuboidOverlapXZ(&obj->pos, po, &po->pos, reach)) {
                        continue;
                    }
                    po->dead = (obj->attack & 0x2) ? 1 : 4;
                    hitside = temp_cuboid_side;
                } else {
                    radius = reach + po->RADIUS;
                    if (radius * radius < dist) {
                        continue;
                    }
                    po->dead = (obj->attack & 0x2) ? 1 : 4;
                    hitside = 8;
                }
                po->touch = (obj->attack & 0xC) ? 2 : hitside;
                if (po->dead != 0) {
                    dpos.x = (obj->pos.x + cpos.x) * 0.5f;
                    dpos.z = (obj->pos.z + cpos.z) * 0.5f;
                    if (obj->attack & 0xC) {
                        dpos.y = obj->pos.y + obj->bot * obj->SCALE;
                    } else {
                        dpos.y = (obj->pos.y + (obj->bot + obj->top) * obj->SCALE * 0.5f +
                                  (po->objbot + po->objtop) * 0.5f) * 0.5f;
                    }
                    AddGameDebris(0x82, &dpos);
                    if (obj->attack & 0x2) {
                        if (po->oldobjtop - 0.012f <= obj->oldobjbot) {
                            obj->mom.y = 0.0f;
                        } else if (obj->oldobjtop <= po->oldobjbot + 0.012f) {
                            obj->mom.y = 0.0f;
                        } else {
                            obj->mom.x *= 0.3f;
                            obj->mom.z *= 0.3f;
                        }
                    }
                    goto killed;
                }
            }
        }
        if (!overlap) {
            continue;
        }

        above = 0;
        if (obj->objtop < po->objbot || obj->objbot > po->objtop) {
            above = 1;
        } else if (VEHICLECONTROL == 2 || (VEHICLECONTROL == 1 && obj->vehicle == 0x20)) {
            s32 veh_vuln = po->vulnerable & 0x82;
            if (obj->attack & veh_vuln) {
                po->dead = 0x15;
            } else if (maskflag == 0) {
                obj->dead = 1;
            } else if (maskflag < 3) {
                lose = 1;
                po->dead = 0x15;
            } else {
                obj->dead = 1;
            }
            NuVecScale(&obj->mom, &obj->mom, 0.3f);
            goto contact;
        } else if (po->oldobjtop - 0.012f <= obj->oldobjbot) {
            s32 top_vuln = po->vulnerable & 0x8E;
            ctype = 2;
            bounce = 0;
            push = 0;
            if (obj->attack & top_vuln) {
                po->dead = (obj->attack & 0x8C) ? 4 : 1;
                if (obj->attack & 0x2) {
                    obj->mom.y = 0.0f;
                }
            } else {
                s32 bounce_vuln = po->vulnerable & 0x20;
                if (obj->attack & bounce_vuln) {
                    po->dead = 4;
                    bounce = 1;
                } else if ((obj->attack & 0x2) && (po->attack & 0x2)) {
                    bounce = 1;
                } else if (po->attack != 0 || (po->flags & 0x100)) {
                    if (maskflag != 0 && maskflag < 3) {
                        lose = 1;
                        po->dead = 4;
                    } else {
                        obj->dead = 1;
                        push = 1;
                        if (po->attack & 0x2) {
                            bounce = 1;
                        } else {
                            bounce = 0;
                        }
                    }
                } else {
                    push = 1;
                }
            }
            if (bounce == 0) {
                if (po->flags & 0x40) {
                    bounce = 1;
                } else {
                    bounce = 0;
                }
                if (bounce == 0) {
                    if (po->flags & 0x80) {
                        bounce = 2;
                    } else {
                        bounce = 0;
                    }
                }
            }
            if (bounce != 0 || push != 0) {
                obj->pos.y = po->objtop - obj->bot * obj->SCALE;
                obj->mom.y = 0.0f;
                obj->ground = 2;
                obj->shadow = po->objtop;
                obj->surface_type = 0xF;
                obj->got_shadow = 1;
                obj->vSN = v010;
                if (bounce != 0) {
                    obj->boing |= bounce;
                    GameSfx(2, &obj->pos);
                    NewRumble(&player->rumble, 0x7F);
                    NewBuzz(&player->rumble, 0xA);
                } else if (push != 0) {
                    obj->pos.x += po->mom.x;
                    obj->pos.z += po->mom.z;
                    if (po->dyrot != 0) {
                        obj->hdg += po->dyrot;
                        dx = obj->pos.x - po->pos.x;
                        dz = obj->pos.z - po->pos.z;
                        ang = NuAtan2D(dx, dz);
                        dist = NuFsqrt(dx * dx + dz * dz);
                        oldx = NuTrigTable[(u16)ang] * dist;
                        oldz = NuTrigTable[(u16)(ang + 0x4000)] * dist;
                        ang += po->dyrot;
                        shiftx = NuTrigTable[(u16)ang] * dist - oldx;
                        shiftz = NuTrigTable[(u16)(ang + 0x4000)] * dist - oldz;
                        obj->pos.x += shiftx;
                        obj->pos.z += shiftz;
                    }
                }
            }
            po->touch = 2;
            goto contact;
        } else if (obj->oldobjtop <= po->oldobjbot + 0.012f) {
            s32 bot_vuln = po->vulnerable & 0x8E;
            ctype = 4;
            push = 0;
            if (obj->attack & bot_vuln) {
                po->dead = (obj->attack & 0x8C) ? 4 : 1;
                if (obj->attack & 0x2) {
                    obj->mom.y = 0.0f;
                    if ((obj->flags & 0x1) && player->jump != 0) {
                        player->jump = 6;
                        player->jump_frame = player->jump_frames;
                        player->jump_type = 4;
                    }
                }
            } else {
                s32 carry_vuln = po->vulnerable & 0x40;
                if (obj->attack & carry_vuln) {
                    po->dead = 4;
                    push = 1;
                } else if ((obj->attack & 0x2) && (po->attack & 0x2)) {
                    push = 0;
                } else if (po->attack == 0) {
                    push = 1;
                } else if (maskflag == 0) {
                    obj->dead = 1;
                    push = 1;
                } else if (maskflag < 3) {
                    lose = 1;
                    po->dead = 4;
                } else {
                    obj->dead = 1;
                    push = 1;
                }
            }
            if (push != 0) {
                obj->pos.y = po->objbot - obj->top * obj->SCALE;
                obj->mom.y = 0.0f;
            }
            po->touch = 4;
            goto contact;
        }

        if (above != 0) {
            if (!(po->objtop < (obj->objtop + obj->objbot) * 0.5f)) {
                continue;
            }
            if (obj->got_shadow != 0 && !(obj->shadow < po->objtop)) {
                continue;
            }
            obj->shadow = po->objtop;
            obj->got_shadow = 1;
            obj->surface_type = 0;
            obj->vSN = v010;
            continue;
        } else {
            s32 side_vuln = po->vulnerable & 0x9E;
            ctype = 1;
            push = 0;
            if (obj->attack & side_vuln) {
                po->dead = (obj->attack & 0x80) ? 4 : 1;
            } else if ((obj->attack & 0x2) && (po->attack & 0x2)) {
                push = 1;
            } else if (po->attack == 0) {
                push = 1;
            } else if (maskflag == 0) {
                obj->dead = 1;
                push = 1;
            } else if (maskflag < 3) {
                lose = 1;
                po->dead = 4;
                push = 0;
            } else {
                obj->dead = 1;
                push = 1;
            }
            if (push != 0) {
                f32 speed;
                if (po->character == 0x39) {
                    gamesfx_effect_volume = 0x9FFD;
                    GameSfx(0x49, &obj->pos);
                }
                if (is_cuboid) {
                    ang = temp_cuboid_bounce_angle;
                } else {
                    ang = NuAtan2D(obj->pos.x - po->pos.x, obj->pos.z - po->pos.z);
                }
                speed = (po->attack & 0x2) ? 0.12f : 0.06f;
                obj->mom.x = NuTrigTable[(u16)ang] * speed;
                obj->mom.z = NuTrigTable[(u16)(ang + 0x4000)] * speed;
                temp_xzmomset = 1;
            }
            po->touch = side;
        }

    contact:
        po->contact = obj;
        po->contact_type = ctype;
    killed:
        if (po->dead != 0) {
            dpos.x = (obj->pos.x + cpos.x) * 0.5f;
            dpos.z = (obj->pos.z + cpos.z) * 0.5f;
            if (ctype == 2) {
                dpos.y = obj->pos.y + obj->bot * obj->SCALE;
            } else if (ctype == 4) {
                dpos.y = obj->pos.y + obj->top * obj->SCALE;
            } else {
                dpos.y = (obj->pos.y + (obj->bot + obj->top) * obj->SCALE * 0.5f +
                          (po->objbot + po->objtop) * 0.5f) * 0.5f;
            }
            AddGameDebris(0x82, &dpos);
            NewRumble(&player->rumble, 0x7F);
            NewBuzz(&player->rumble, 5);
            if ((po->flags & 0x40000) && !(obj->attack & 0x80)) {
                po->kill_contact = 1;
                po->dead = 0;
                continue;
            }
            if (po->dead == 1) {
                u16 yrot = NuAtan2D(po->pos.x - obj->pos.x, po->pos.z - obj->pos.z);
                po->mom.x = 0.0f;
                po->mom.y = 0.0f;
                po->mom.z = 0.19999999f;
                NuVecRotateX(&po->mom, &po->mom, -0x400);
                NuVecRotateY(&po->mom, &po->mom, yrot);
                GameSfx(0x37, &po->pos);
            }
            KillGameObject(po, po->dead);
        } else if (obj->dead != 0) {
            goto dead;
        }
    }

    if (lose != 0) {
        if (obj->invincible == 0) {
            LoseMask(obj);
        }
    } else if (obj->dead != 0) {
    dead:
        if (obj->invincible != 0) {
            obj->dead = 0;
        } else {
            killer = po->parent;
            die_anim = killer->ai.kill;
            if (die_anim == -1) {
                die_anim = GetDieAnim(obj, -1);
            }
            KillGameObject(obj, die_anim);
        }
    }
    if (VEHICLECONTROL == 1 && obj->vehicle == 0x53) {
        ObjectToAtlas(obj, player);
    }
}


void GetTopBot(struct creature_s *c) {
    if (c == player && VEHICLECONTROL == 1 && c->obj.vehicle != -1) {
        c->obj.bot = CData[c->obj.vehicle].min.y;
        c->obj.top = CData[c->obj.vehicle].max.y;
    } else if (c == player && VEHICLECONTROL == 2) {
        c->obj.bot = CData[115].min.y;
        c->obj.top = CData[115].max.y;
    } else if (c->obj.flags & 0x10000) {
        c->obj.bot = -c->obj.max.y;
        c->obj.top = -c->obj.min.y;
    } else if (c->obj.dangle != 0) {
        c->obj.top = c->obj.max.y;
        if (c->obj.dangle == 2) {
            c->obj.bot = (c->obj.min.y + c->obj.top) * 0.5f;
        } else {
            c->obj.bot = c->obj.min.y;
        }
    } else {
        c->obj.bot = c->obj.min.y;
        if (((c->obj.ground != 0 || c->obj.old_ground != 0) &&
             (c->crawl != 0 || (c->slide != 0 && c->obj.character != 1) ||
              c->slam_wait != 0)) ||
            (c->obj.ground == 0 && c->slam != 0 && c->slam < 3)) {
            c->obj.top = (c->obj.min.y + c->obj.max.y) * 0.5f;
        } else {
            c->obj.top = c->obj.max.y;
        }
    }
}


extern struct crategroup_s CrateGroup[];
extern struct cratecube_s Crate[];
extern s32 CRATEGROUPCOUNT;
extern s32 temp_crate_y_floor_adjust;
extern s32 temp_crate_y_ceiling_adjust;
extern s32 temp_crate_xz_adjust;
extern s32 temp_crate_bounce;
extern f32 CRATEHOPSPEED;
extern f32 JUMPONCRATEBREAKSPEED;
extern f32 FALLONCRATEBREAKSPEED;
extern f32 METALCRATEBOUNCESPEED;

s32 GetCrateType(struct cratecube_s *crate, s32 flags);
s32 AttackCrate(struct obj_s *obj, struct crategroup_s *group,
                struct cratecube_s *crate);
void HopCratesAbove(f32 speed, struct crategroup_s *group,
                    struct cratecube_s *crate);
s32 CrateAbove(struct obj_s *obj, struct crategroup_s *group,
               struct cratecube_s *crate);
s32 CrateBelow(struct obj_s *obj, struct crategroup_s *group,
               struct cratecube_s *crate);
s32 NewCrateAnimation(struct cratecube_s *crate, s32 type, s32 action,
                      s32 random);
void CrateSafety(struct crategroup_s *group, struct cratecube_s *crate,
                 struct obj_s *obj);
s32 CrateBounceReaction(struct crategroup_s *group, struct cratecube_s *crate,
                        s32 type, s32 attack);
s32 CrateInTheWay(struct obj_s *obj, struct crategroup_s *group,
                  struct cratecube_s *crate, s32 dx, s32 dz, char *hit);


inline f32 GameObjectRadius(struct obj_s *obj) {
    f32 r;

    r = obj->radius * obj->SCALE;
    if ((obj->attack & 8) != 0) {
        return r + 0.5f;
    }
    if ((obj->attack & 4) != 0) {
        return r + 0.25f;
    }
    if ((obj->attack & 2) != 0) {
        return (obj->character == 1 &&
                ((struct creature_s *)obj->parent)->slide != 0) ? r * 3.0f : r + r;
    }
    return r;
}


void CrateCollisions(struct obj_s *obj) {
    struct crategroup_s *group;
    struct cratecube_s *crate;
    struct nuvec_s vOld;
    struct nuvec_s vNew;
    struct nuvec_s vMom;
    s32 i;
    s32 j;
    s32 in;
    s32 mask;
    s32 miss;
    s32 xz_overlap;
    f32 x0;
    f32 z0;
    f32 x1;
    f32 z1;
    f32 y;
    f32 d;
    f32 d0;
    f32 dy;
    f32 y_floor;
    f32 y_ceiling;
    f32 reach;
    f32 f;
    s32 corner;
    s32 i_floor;
    s32 i_ceiling;
    s32 xz_adjust;
    s32 got_floor;
    s32 got_ceiling;
    s32 ceiling_attack;
    s32 type;
    s32 attack;
    s32 jump;
    s32 appeared;
    s32 obj_dx;
    s32 obj_dz;
    char hit[256];

    if (level_part_2 != 0) {
        return;
    }
    obj->objbot = obj->pos.y + obj->bot * obj->SCALE;
    obj->objtop = obj->pos.y + obj->top * obj->SCALE;
    if (obj->parent == player) {
        jump = ((struct creature_s *)obj->parent)->jump;
    } else {
        jump = 0;
    }
    obj->clearance = 2000000.0f;
    temp_crate_y_floor_adjust = 0;
    temp_crate_y_ceiling_adjust = 0;
    temp_crate_xz_adjust = 0;
    temp_crate_bounce = 0;
    group = CrateGroup;
    for (i = 0; i < CRATEGROUPCOUNT; i++, group++) {
        if (!(obj->pos.x < group->minclip.x - obj->RADIUS - 0.5f) &&
            !(obj->pos.x > group->maxclip.x + obj->RADIUS + 0.5f) &&
            !(obj->pos.z < group->minclip.z - obj->RADIUS - 0.5f) &&
            !(obj->pos.z > group->maxclip.z + obj->RADIUS + 0.5f)) {
            NuVecSub(&vNew, &obj->pos, &group->origin);
            NuVecRotateY(&vNew, &vNew, -group->angle);
            NuVecSub(&vOld, &obj->oldpos, &group->origin);
            NuVecRotateY(&vOld, &vOld, -group->angle);
            NuVecRotateY(&vMom, &obj->mom, -group->angle);
            obj_dx = (s32)(vNew.x + vNew.x);
            if (vNew.x < 0.0f) {
                obj_dx--;
            }
            obj_dz = (s32)(vNew.z + vNew.z);
            if (vNew.z < 0.0f) {
                obj_dz--;
            }
            xz_adjust = 0;
            got_floor = 0;
            got_ceiling = 0;
            crate = &Crate[group->iCrate];
            for (j = 0; j < group->nCrates; j++, crate++) {
                hit[j] = 0;
                if (crate->on == 0) {
                    continue;
                }
                appeared = crate->appeared;
                crate->appeared = 0;
                type = GetCrateType(crate, 0);
                if ((u32)(type + 1) <= 1) {
                    continue;
                }
                xz_overlap = 0;
                x0 = crate->dx * 0.5f - obj->RADIUS;
                if (!(vNew.x < x0)) {
                    x1 = crate->dx * 0.5f + 0.5f + obj->RADIUS;
                    if (!(vNew.x > x1)) {
                        z0 = crate->dz * 0.5f - obj->RADIUS;
                        if (!(vNew.z < z0)) {
                            z1 = crate->dz * 0.5f + 0.5f + obj->RADIUS;
                            if (!(vNew.z > z1)) {
                                xz_overlap = 1;
                            }
                        }
                    }
                }
                if (xz_overlap == 0 &&
                    ((obj->attack & 2) ||
                     ((obj->attack & 0xC) && obj->mom.y < 0.0f))) {
                    if (!(obj->objtop < crate->pos.y) &&
                        !(obj->objbot > crate->pos.y + 0.5f)) {
                        reach = GameObjectRadius(obj);
                        if (!(vNew.x < crate->dx * 0.5f - reach) &&
                            !(vNew.x > crate->dx * 0.5f + 0.5f + reach) &&
                            !(vNew.z < crate->dz * 0.5f - reach) &&
                            !(vNew.z > crate->dz * 0.5f + 0.5f + reach)) {
                            if (obj->attack & 2) {
                                if (obj->pad_speed == 0.0f) {
                                    in = 0;
                                    y = obj->objbot + 0.25f;
                                    if (y > crate->pos.y && y < crate->pos.y + 0.5f) {
                                        in = 1;
                                    }
                                } else {
                                    in = 1;
                                }
                                if (in != 0) {
                                    if (type != 0x13) {
                                        if (AttackCrate(obj, group, crate) == 1) {
                                            HopCratesAbove(CRATEHOPSPEED, group, crate);
                                            if ((obj->oldobjbot >= crate->oldy + 0.5f - 0.012f &&
                                                 CrateAbove(obj, group, crate) == 0) ||
                                                (obj->oldobjtop <= crate->oldy + 0.012f &&
                                                 CrateBelow(obj, group, crate) == 0)) {
                                                obj->mom.y = 0.0f;
                                                temp_crate_y_ceiling_adjust = 1;
                                            } else {
                                                if (VEHICLECONTROL == 1 && obj->vehicle == 0x3B) {
                                                    f = 0.90000004f;
                                                } else {
                                                    f = 0.3f;
                                                }
                                                obj->mom.x *= f;
                                                obj->mom.z *= f;
                                            }
                                        }
                                    } else if (crate->action == -1) {
                                        NewCrateAnimation(crate, 0x13, 0x58, 0);
                                        GameSfx(0x39, &crate->pos);
                                    }
                                }
                            } else if (obj->oldobjbot >= crate->oldy + 0.5f - 0.012f &&
                                       CrateAbove(obj, group, crate) == 0) {
                                AttackCrate(obj, group, crate);
                            }
                        }
                    }
                    continue;
                }
                if (xz_overlap != 0) {
                    miss = 0;
                    if (obj->objtop < crate->pos.y || obj->objbot > crate->pos.y + 0.5f) {
                        miss = 1;
                    } else if (appeared != 0) {
                        CrateSafety(group, crate, obj);
                        continue;
                    } else if (obj->oldobjbot >= crate->oldy + 0.5f - 0.012f &&
                               CrateAbove(obj, group, crate) == 0) {
                        attack = obj->attack & 0xE;
                        if ((type == 0x13 ? (attack & 0x8C) : (attack & 0xE)) != 0 ||
                            (type == 0x10 && obj->anim.newaction != 0x53) ||
                            (*(u32 *)&obj->attack & 0x480)) {
                            if (AttackCrate(obj, group, crate) == 1) {
                                if (attack & 2) {
                                    obj->mom.y = 0.0f;
                                }
                                continue;
                            }
                        }
                        if (type == 0x13 && attack != 0 && crate->action == -1) {
                            NewCrateAnimation(crate, 0x13, 0x58, 0);
                            GameSfx(0x38, &crate->pos);
                        }
                        y = crate->pos.y + 0.5f;
                        if (got_floor == 0 || y >= y_floor) {
                            d = NuVecXZDistSqr(&obj->pos, &crate->pos, 0);
                            if (got_floor == 0 ||
                                (crate->dx == obj_dx && crate->dz == obj_dz) || d < d0) {
                                i_floor = j;
                                d0 = d;
                            }
                            y_floor = y;
                            got_floor = 1;
                        }
                        hit[j] = 2;
                        continue;
                    } else if (obj->oldobjtop <= crate->oldy + 0.012f &&
                               CrateBelow(obj, group, crate) == 0) {
                        attack = obj->attack;
                        if (type == 0x13 ? (attack & 0x80)
                                         : (attack != 0 || crate->mom < 0.0f || type == 0x10 ||
                                            (*(u32 *)&obj->attack & 0x480))) {
                            if (AttackCrate(obj, group, crate) == 1) {
                                if (attack & 2) {
                                    temp_crate_y_ceiling_adjust = 1;
                                    obj->mom.y = 0.0f;
                                }
                                continue;
                            }
                        }
                        if (type == 0x13 && attack != 0 && crate->action == -1) {
                            NewCrateAnimation(crate, 0x13, 0x58, 0);
                            GameSfx(0x39, &crate->pos);
                        }
                        y = crate->pos.y;
                        if (got_ceiling == 0 || y <= y_ceiling) {
                            d = NuVecXZDistSqr(&obj->pos, &crate->pos, 0);
                            if (got_ceiling == 0 ||
                                (crate->dx == obj_dx && crate->dz == obj_dz) || d < d0) {
                                i_ceiling = j;
                                d0 = d;
                            }
                            ceiling_attack = attack;
                            y_ceiling = y;
                            got_ceiling = 1;
                        }
                        hit[j] = 4;
                        continue;
                    }
                    if (miss != 0) {
                        dy = crate->pos.y - obj->objbot;
                        if (dy > 0.0f && dy < obj->clearance &&
                            CrateBelow(obj, group, crate) == 0) {
                            obj->clearance = dy;
                        }
                        y = crate->pos.y + 0.5f;
                        if (y < (obj->objtop + obj->objbot) * 0.5f &&
                            (obj->got_shadow == 0 || y > obj->shadow) &&
                            CrateAbove(obj, group, crate) == 0) {
                            obj->shadow = y;
                            obj->surface_type = 0xF;
                            obj->got_shadow = 1;
                            obj->vSN = v010;
                        }
                    } else {
                        hit[j] = 1;
                    }
                }
            }
            if (got_floor != 0) {
                crate = &Crate[group->iCrate + i_floor];
                type = GetCrateType(crate, 0);
                if (crate->oldy + 0.5f < obj->oldobjbot &&
                    (type == 4 || type == 6 || type == 0xD ||
                     (jump != 0 ? fabs(crate->mom - obj->mom.y) > JUMPONCRATEBREAKSPEED
                                : fabs(crate->mom - obj->mom.y) > FALLONCRATEBREAKSPEED) ||
                     (VEHICLECONTROL != 0 && type == 9))) {
                    CrateBounceReaction(group, crate, type, 2);
                    obj->boing = temp_crate_bounce;
                } else {
                    obj->boing = 0;
                }
                obj->ground = 2;
                obj->pos.y = y_floor - obj->bot * obj->SCALE;
                obj->got_shadow = 1;
                obj->surface_type = 0xF;
                obj->shadow = y_floor;
                obj->mom.y = 0.0f;
                obj->vSN = v010;
                temp_crate_y_floor_adjust = 1;
            }
            if (got_ceiling != 0) {
                crate = &Crate[group->iCrate + i_ceiling];
                type = GetCrateType(crate, 0);
                if (crate->oldy > obj->oldobjtop) {
                    if ((obj == &player->obj && VEHICLECONTROL == 0) || ceiling_attack != 0 ||
                        (VEHICLECONTROL == 2 && type == 9)) {
                        CrateBounceReaction(group, crate, type, 4);
                    }
                }
                mask = 0;
                if (obj->mask != 0 && obj->mask->active != 0 && (LDATA->flags & 0xE00) == 0) {
                    mask = obj->mask->active;
                }
                if (type == 0xF && (crate->flags & 1) && obj->invincible == 0 && mask < 3) {
                    if (mask != 0) {
                        LoseMask(obj);
                        crate->mom = METALCRATEBOUNCESPEED;
                    } else {
                        KillGameObject(obj, GetDieAnim(obj, -1));
                    }
                }
                if (got_floor == 0 && obj->ground == 0) {
                    obj->pos.y = y_ceiling - obj->top * obj->SCALE;
                    obj->mom.y = 0.0f;
                    temp_crate_y_ceiling_adjust = 1;
                }
            }
            obj->objbot = obj->pos.y + obj->bot * obj->SCALE;
            obj->objtop = obj->pos.y + obj->top * obj->SCALE;
            crate = &Crate[group->iCrate];
            for (j = 0; j < group->nCrates; j++, crate++) {
                if (hit[j] != 1) {
                    continue;
                }
                if (crate->pos.y + 0.5f == obj->objbot) {
                    continue;
                }
                type = GetCrateType(crate, 0);
                attack = obj->attack & 0x12;
                dy = obj->objbot + 0.25f;
                if ((attack & 2) &&
                    (obj->pad_speed > 0.0f ||
                     ((obj->flags & 1) && VEHICLECONTROL == 1 && obj->vehicle == 0x3B))) {
                    in = 1;
                } else {
                    in = 0;
                    if (dy > crate->pos.y && dy < crate->pos.y + 0.5f) {
                        in = 1;
                    }
                }
                if ((type == 0x13 && (obj->attack & 0x80)) ||
                    (type != 0x13 && ((attack != 0 && in != 0) || type == 0x10 ||
                                      (obj->attack & 0x480)))) {
                    if (AttackCrate(obj, group, crate) == 1) {
                        HopCratesAbove(CRATEHOPSPEED, group, crate);
                        continue;
                    }
                }
                if (type == 0x13 && attack != 0 && crate->action == -1) {
                    NewCrateAnimation(crate, type, 0x58, 0);
                    GameSfx(0x39, &crate->pos);
                }
                x0 = crate->dx * 0.5f - obj->RADIUS;
                x1 = crate->dx * 0.5f + 0.5f + obj->RADIUS;
                z0 = crate->dz * 0.5f - obj->RADIUS;
                z1 = crate->dz * 0.5f + 0.5f + obj->RADIUS;
                corner = 0;
                if (vNew.x - x0 < x1 - vNew.x) {
                    if (vNew.z - z0 < z1 - vNew.z) {
                        if (vNew.z - z0 < vNew.x - x0) {
                            if (CrateInTheWay(obj, group, crate, crate->dx, crate->dz - 1, hit) == 0) {
                                vNew.z = z0;
                                vMom.z = 0.0f;
                            } else if (CrateInTheWay(obj, group, crate, crate->dx - 1, crate->dz, hit) == 0) {
                                vNew.x = x0;
                                vMom.x = 0.0f;
                            } else {
                                corner = 0xA;
                            }
                        } else {
                            if (CrateInTheWay(obj, group, crate, crate->dx - 1, crate->dz, hit) == 0) {
                                vNew.x = x0;
                                vMom.x = 0.0f;
                            } else if (CrateInTheWay(obj, group, crate, crate->dx, crate->dz - 1, hit) == 0) {
                                vNew.z = z0;
                                vMom.z = 0.0f;
                            } else {
                                corner = 0xA;
                            }
                        }
                    } else {
                        if (z1 - vNew.z < vNew.x - x0) {
                            if (CrateInTheWay(obj, group, crate, crate->dx, crate->dz + 1, hit) == 0) {
                                vNew.z = z1;
                                vMom.z = 0.0f;
                            } else if (CrateInTheWay(obj, group, crate, crate->dx - 1, crate->dz, hit) == 0) {
                                vNew.x = x0;
                                vMom.x = 0.0f;
                            } else {
                                corner = 9;
                            }
                        } else {
                            if (CrateInTheWay(obj, group, crate, crate->dx - 1, crate->dz, hit) == 0) {
                                vNew.x = x0;
                                vMom.x = 0.0f;
                            } else if (CrateInTheWay(obj, group, crate, crate->dx, crate->dz + 1, hit) == 0) {
                                vNew.z = z1;
                                vMom.z = 0.0f;
                            } else {
                                corner = 9;
                            }
                        }
                    }
                } else {
                    if (vNew.z - z0 < z1 - vNew.z) {
                        if (vNew.z - z0 < x1 - vNew.x) {
                            if (CrateInTheWay(obj, group, crate, crate->dx, crate->dz - 1, hit) == 0) {
                                vNew.z = z0;
                                vMom.z = 0.0f;
                            } else if (CrateInTheWay(obj, group, crate, crate->dx + 1, crate->dz, hit) == 0) {
                                vNew.x = x1;
                                vMom.x = 0.0f;
                            } else {
                                corner = 6;
                            }
                        } else {
                            if (CrateInTheWay(obj, group, crate, crate->dx + 1, crate->dz, hit) == 0) {
                                vNew.x = x1;
                                vMom.x = 0.0f;
                            } else if (CrateInTheWay(obj, group, crate, crate->dx, crate->dz - 1, hit) == 0) {
                                vNew.z = z0;
                                vMom.z = 0.0f;
                            } else {
                                corner = 6;
                            }
                        }
                    } else {
                        if (z1 - vNew.z < x1 - vNew.x) {
                            if (CrateInTheWay(obj, group, crate, crate->dx, crate->dz + 1, hit) == 0) {
                                vNew.z = z1;
                                vMom.z = 0.0f;
                            } else if (CrateInTheWay(obj, group, crate, crate->dx + 1, crate->dz, hit) == 0) {
                                vNew.x = x1;
                                vMom.x = 0.0f;
                            } else {
                                corner = 5;
                            }
                        } else {
                            if (CrateInTheWay(obj, group, crate, crate->dx + 1, crate->dz, hit) == 0) {
                                vNew.x = x1;
                                vMom.x = 0.0f;
                            } else if (CrateInTheWay(obj, group, crate, crate->dx, crate->dz + 1, hit) == 0) {
                                vNew.z = z1;
                                vMom.z = 0.0f;
                            } else {
                                corner = 5;
                            }
                        }
                    }
                }
                if (corner != 0) {
                    if (corner & 8) {
                        vNew.x = x0;
                    } else {
                        vNew.x = x1;
                    }
                    if (corner & 2) {
                        vNew.z = z0;
                    } else {
                        vNew.z = z1;
                    }
                    vMom.x = 0.0f;
                    vMom.z = 0.0f;
                }
                xz_adjust = 1;
            }
            if (xz_adjust != 0) {
                NuVecRotateY(&vNew, &vNew, group->angle);
                obj->pos.x = group->origin.x + vNew.x;
                obj->pos.z = group->origin.z + vNew.z;
                NuVecRotateY(&vMom, &vMom, group->angle);
                obj->mom.x = vMom.x;
                obj->mom.z = vMom.z;
                temp_crate_xz_adjust = 1;
            }
        }
    }
}


s32 WumpaCollisions(struct obj_s *obj) {
    s32 i;
    s32 key;
    f32 r;
    f32 reach;
    f32 y;
    f32 dx;
    f32 dz;
    struct wumpa_s *w;
    struct winfo_s *info;

    if (TimeTrial == 0) {
        if (level_part_2 != 0) {
            return 0;
        }
        r = obj->radius * obj->SCALE;
        if (obj->attack & 8) {
            reach = r + 0.5f;
        } else if (obj->attack & 4) {
            reach = r + 0.25f;
        } else {
            if (obj->attack & 2) {
                if (obj->character == 1 &&
                    ((struct creature_s *)obj->parent)->slide != 0) {
                    r = r * 3.0f;
                } else {
                    r = r + r;
                }
            }
            reach = r;
        }
        reach = reach + 0.2f;
        reach = reach * reach;
        w = Wumpa;
        for (i = 0; i < 320; i++, w++) {
            if (w->active != 2) {
                continue;
            }
            info = &WInfo[i & 7];
            y = w->pos.y + info->dy;
            if (!(obj->pos.y + obj->bot * obj->SCALE < y + 0.1f)) {
                continue;
            }
            if (!(y - 0.1f < obj->pos.y + obj->top * obj->SCALE)) {
                continue;
            }
            dx = obj->pos.x - w->pos.x;
            dz = obj->pos.z - w->pos.z;
            if (!(dx * dx + dz * dz < reach)) {
                continue;
            }
            if (obj->attack & 2) {
                FlyWumpa(w);
            } else if (obj->dead == 0) {
                AddScreenWumpa(w->pos.x, y, w->pos.z, 1);
                key = -1;
                AddFiniteShotDebrisEffect(&key, GDeb[130].i, &w->pos, 1);
                w->active = 0;
                GameSfx(0x2C, &obj->pos);
            }
            return 0;
        }
    }
    return 0;
}


void MoveLoopXZ(struct obj_s *obj, u16 *hdg) {
    struct nuvec_s v;
    u16 angle;

    if (hdg != 0) {
        angle = *hdg;
    } else {
        angle = obj->hdg;
    }
    NuVecRotateY(&v, &obj->mom, -angle);

    if (v.x > 0.0f) {
        v.x -= in_s_friction;
        if (v.x < 0.0f) {
            v.x = 0.0f;
        }
    } else if (v.x < 0.0f) {
        v.x += in_s_friction;
        if (v.x > 0.0f) {
            v.x = 0.0f;
        }
    }
    if (v.x < -0.08999999613f) {
        v.x = v.x + (-0.08999999613f - v.x) * 0.25f;
    } else if (v.x > 0.08999999613f) {
        v.x = v.x + (0.08999999613f - v.x) * 0.25f;
    }
    obj->side = v.x;
    obj->abs_side = fabs(v.x);

    if (v.z > in_speed) {
        v.z -= in_f_friction;
        if (v.z < in_speed) {
            v.z = in_speed;
        }
    } else if (v.z < in_speed) {
        v.z += in_f_friction;
        if (v.z > in_speed) {
            v.z = in_speed;
        }
    }
    obj->forward = v.z;
    obj->abs_forward = fabs(v.z);

    NuVecRotateY(&obj->mom, &v, angle);
}


inline void FindAnglesZX(struct nuvec_s *v) {
    struct nuvec_s rotated;
    u16 xrot;
    u16 zrot;

    xrot = NuAtan2D(v->z, v->y);
    temp_xrot = xrot;
    NuVecRotateX(&rotated, v, -xrot);
    zrot = NuAtan2D(rotated.x, rotated.y);
    temp_zrot = -zrot;
}


void GetSurfaceInfo(struct obj_s *obj, s32 mode, f32 reflect) {
    struct nuvec_s dir;
    u32 surface;
    s32 old_surface;
    u16 yrot;

    if (reflect != 2000000.0f) {
        surface = ShadowInfo();
        if (surface >= 16) {
            surface = 0;
        }
        obj->reflect_y = (TerSurface[surface].flags & 4) ? reflect : 2000000.0f;
    } else {
        obj->reflect_y = 2000000.0f;
        surface = 0;
    }

    if (mode != 0) {
        old_surface = obj->surface_type;
        obj->surface_type = surface;
        if (obj->shadow == 2000000.0f) {
            obj->shadow = SAFEY;
            obj->vSN = v010;
        } else {
            obj->vSN = ShadNorm;
            if ((obj->flags & 1) && obj->ground != 0 && mode >= 2) {
                if (obj->surface_type == 6) {
                    ShadowDir(&dir);
                    plr_conveyor_yrot = NuAtan2D(dir.x, dir.z) + 0x4000;
                    obj->pos.x += NuTrigTable[plr_conveyor_yrot] * 0.03f;
                    obj->pos.z += NuTrigTable[(plr_conveyor_yrot + 0x4000) & 0xFFFF] * 0.03f;
                    NuVecSub(&plr_conveyor_mom, &obj->pos, &obj->oldpos);
                } else if (old_surface == 6) {
                    yrot = NuAtan2D(plr_conveyor_mom.x, plr_conveyor_mom.z);
                    if (fabs(RotDiff(plr_conveyor_yrot, yrot)) < 16384.0) {
                        obj->mom.x = NuTrigTable[plr_conveyor_yrot] * 0.03f * 2.0f;
                        obj->mom.z = NuTrigTable[(u16)(plr_conveyor_yrot + 0x4000)] * 0.03f * 2.0f;
                    }
                }
            }
        }
    }

    obj->layer_shadow = EShadY;
    if (EShadY != 2000000.0f && obj->pos.y + obj->bot * obj->SCALE < EShadY) {
        obj->layer_type = EShadowInfo();
        if ((u8)obj->layer_type >= 11) {
            obj->layer_type = 0;
        }
        if (!(TerLayer[obj->layer_type].flags & 0x40)) {
            obj->vLN = EShadNorm;
            FindAnglesZX(&obj->vLN);
            obj->layer_xrot = temp_xrot;
            obj->layer_zrot = temp_zrot;
            goto roof;
        }
    }
    obj->layer_type = -1;
    obj->vLN = v010;
    obj->layer_xrot = 0;
    obj->layer_zrot = 0;

roof:
    obj->roof_y = ShadRoofY;
    if (ShadRoofY != 2000000.0f && obj->pos.y + obj->bot * obj->SCALE < ShadRoofY) {
        obj->roof_type = ShadowRoofInfo();
        if ((u8)obj->roof_type >= 16) {
            obj->roof_type = 0;
        }
        obj->vRN.x = -ShadRoofNorm.x;
        obj->vRN.y = -ShadRoofNorm.y;
        obj->vRN.z = -ShadRoofNorm.z;
        FindAnglesZX(&obj->vRN);
        temp_roof_xrot = temp_xrot;
        temp_roof_zrot = temp_zrot;
    } else {
        obj->roof_type = -1;
        obj->vRN = v010;
        temp_roof_xrot = 0;
        temp_roof_zrot = 0;
    }
}


void ObjectRotation(struct obj_s *obj, s32 mode, s32 blend) {
    struct nuvec_s v;
    f32 t;
    s32 vehicle;
    s32 grounded;
    s32 blended;
    u16 xrot;
    u16 pitch;
    u16 zrot;

    vehicle = -1;
    if (obj == &player->obj && VEHICLECONTROL == 1 && obj->vehicle != -1) {
        vehicle = obj->vehicle;
    }
    grounded = (obj->pos.y + obj->bot * obj->SCALE - obj->shadow < 0.25f) ? 1 : 0;

    blended = 0;
    if (obj->vSN.x == 0.0f && obj->vSN.y == 1.0f && obj->vSN.z == 0.0f) {
        temp_xrot = 0;
        temp_zrot = 0;
    } else {
        if (blend) {
            t = obj->xz_distance / 0.1999999881f;
            v = obj->vSN;
            if (t > 1.0f) {
                t = 1.0f;
            }
            blended = 1;
            v.x = v010.x + (v.x - v010.x) * t;
            v.y = v010.y + (v.y - v010.y) * t;
            v.z = v010.z + (v.z - v010.z) * t;
            FindAnglesZX(&v);
            xrot = temp_xrot;
            zrot = temp_zrot;
        }
        FindAnglesZX(&obj->vSN);
    }

    if (obj->flags & 0x10000) {
        if (mode == 1) {
            obj->surface_xrot = temp_xrot;
            obj->surface_zrot = temp_zrot;
            obj->roof_xrot = temp_roof_xrot;
            obj->xrot = temp_roof_xrot;
            obj->roof_zrot = temp_roof_zrot;
            obj->zrot = temp_roof_zrot;
        } else if (mode == 2) {
            obj->surface_xrot = SeekRot(obj->surface_xrot, temp_xrot, 3);
            obj->surface_zrot = SeekRot(obj->surface_zrot, temp_zrot, 3);
            obj->roof_xrot = SeekRot(obj->roof_xrot, temp_roof_xrot, 3);
            obj->roof_zrot = SeekRot(obj->roof_zrot, temp_roof_zrot, 3);
            obj->xrot = SeekRot(obj->xrot, obj->roof_xrot, 2);
            obj->zrot = SeekRot(obj->zrot, obj->roof_zrot, 2);
        }
    } else if (mode == 1) {
        obj->surface_xrot = temp_xrot;
        obj->surface_zrot = temp_zrot;
        if (grounded == 0 || vehicle == 0x3B) {
            obj->zrot = 0;
            obj->xrot = 0;
        } else {
            obj->xrot = temp_xrot;
            obj->zrot = temp_zrot;
        }
    } else if (mode == 2) {
        if (obj == &player->obj && VEHICLECONTROL == 2) {
            obj->surface_xrot = SeekRot(obj->surface_xrot, temp_xrot, 3);
            obj->surface_zrot = SeekRot(obj->surface_zrot, temp_zrot, 3);
            if (obj->pad_speed == 0.0f || fabs(obj->pad_dx) < 0.33333334f) {
                xrot = obj->pad_dz * 16384.0f;
            } else {
                xrot = 0x4000 - abs(RotDiff(0, obj->pad_angle));
            }
            if (obj->shadow != 2000000.0f && xrot > 0x8000 && xrot <= 0xF554) {
                t = obj->pos.y + obj->bot * obj->SCALE - obj->shadow;
                if (t < 1.0f) {
                    xrot = -(s32)(t * 16384.0f);
                    goto seek;
                }
            }
            if (obj->roof_y != 2000000.0f && (s16)xrot >= 0 && xrot >= 0xAAC) {
                t = obj->roof_y - (obj->pos.y + obj->top * obj->SCALE);
                if (t < 1.0f) {
                    goto tilt;
                }
            }
            v.x = obj->pos.x;
            v.z = obj->pos.z;
            v.y = obj->pos.y + (obj->bot + obj->top) * obj->SCALE * 0.5f;
            t = CrateBottomAbove(&v);
            if (t != 2000000.0f && (s16)xrot >= 0) {
                t = t - (obj->pos.y + obj->top * obj->SCALE);
                if (t < 1.0f) {
                    if (t < 0.0f) {
                        t = 0.0f;
                    }
                tilt:
                    xrot = t * 16384.0f;
                }
            }
        seek:
            obj->xrot = SeekRot(obj->xrot, xrot, 5);
            obj->zrot = 0;
        } else if (vehicle == 0x20) {
            obj->surface_xrot = SeekRot(obj->surface_xrot, temp_xrot, 3);
            obj->surface_zrot = SeekRot(obj->surface_zrot, temp_zrot, 3);
            if (obj->pad_speed > 0.0) {
                pitch = 0x4000 - abs(RotDiff(0, obj->pad_angle));
            } else {
                pitch = 0;
            }
            obj->xrot = SeekRot(obj->xrot, pitch, 5);
            obj->zrot = 0;
        } else {
            obj->surface_xrot = SeekRot(obj->surface_xrot, temp_xrot, 3);
            obj->surface_zrot = SeekRot(obj->surface_zrot, temp_zrot, 3);
            if (vehicle != 0x3B) {
                if (grounded != 0) {
                    if (blended == 0) {
                        xrot = obj->surface_xrot;
                        zrot = obj->surface_zrot;
                    }
                } else {
                    zrot = 0;
                    xrot = 0;
                }
                if (!(obj->flags & 0x20000) && (grounded != 0 || vehicle != 0x99)) {
                    obj->xrot = SeekRot(obj->xrot, xrot, 2);
                    obj->zrot = SeekRot(obj->zrot, zrot, 2);
                }
            } else {
                obj->zrot = 0;
                obj->xrot = 0;
            }
        }
    }
}

void PickupItem(struct obj_s *obj) {
    struct creature_s *c;
    s32 mask;
    s32 power;

    if (new_mode == -1 && new_level == -1) {
        switch (obj->character) {
        case 0x75:
            plr_crystal.count = 1;
            plr_crystal.frame = 0x19;
            plr_items |= 1;
            GameSfx(0x26, 0);
            AddPanelDebris(0.0f, -0.7f, 6, 0.125f, 0x10);
            ClockOff();
            GameSfx(0x26, 0);
            break;
        case 0x76:
            StartTimeTrial(&obj->pos, 0);
            break;
        case 0x77:
            plr_crategem.count = 1;
            plr_crategem.frame = 0x19;
            plr_items |= 2;
            GameSfx(0x26, 0);
            AddPanelDebris(-0.2f, -0.7f, 6, 0.125f, 0x10);
            ClockOff();
            GameSfx(0x26, 0);
            break;
        case 0x78:
        case 0x79:
        case 0x7A:
        case 0x7B:
        case 0x7C:
        case 0x7D:
            if (obj->character == 0x79) {
                mask = 0x8;
            } else if (obj->character == 0x7A) {
                mask = 0x20;
            } else if (obj->character == 0x7B) {
                mask = 0x10;
            } else if (obj->character == 0x7C) {
                mask = 0x40;
            } else {
                mask = (obj->character == 0x7D) ? 0x80 : 0x4;
            }
            plr_items |= mask;
            plr_bonusgem.item = mask;
            plr_bonusgem.count = 1;
            plr_bonusgem.frame = 0x19;
            GameSfx(0x26, 0);
            AddPanelDebris(0.2f, -0.7f, 6, 0.125f, 0x10);
            GameSfx(0x26, 0);
            break;
        case 0xA2:
        case 0xA3:
        case 0xA4:
        case 0xA5:
        case 0xA6:
        case 0xA7:
            if (LBIT & 0x3E00000) {
                boss_dead = 2;
            }
            power = obj->character;
            switch (power) {
            case 0xA7:
                new_power = 0;
                break;
            case 0xA5:
                new_power = 1;
                break;
            case 0xA6:
                new_power = 2;
                break;
            case 0xA2:
                new_power = 3;
                break;
            case 0xA4:
                new_power = 4;
                break;
            case 0xA3:
                new_power = 5;
                break;
            }
            Game.powerbits |= 1 << new_power;
            NewMenu(&Cursor, 0x1F, -1, -1);
            GameSfx(0x26, 0);
            if (Level != 0x15 && Level != 0x18) {
                player->obj.mom.x = 0.0f;
                player->obj.mom.z = 0.0f;
                player->slide = 0;
            }
            break;
        }
    }
    c = (struct creature_s *)obj->parent;
    obj->dead = 1;
    c->on = 0;
    c->off_wait = 2;
}


inline void FlyGameObject(struct obj_s *obj, u16 yrot) {
    obj->mom.x = 0.0f;
    obj->mom.y = 0.0f;
    obj->mom.z = 0.1999999881f;
    NuVecRotateX(&obj->mom, &obj->mom, -0x400);
    NuVecRotateY(&obj->mom, &obj->mom, yrot);
    GameSfx(0x37, &obj->pos);
}

s32 HitCreatures(struct obj_s *obj, s32 how, s32 hit_type) {
    s32 i;
    struct obj_s *po;

    if (level_part_2 == 0) {
        for (i = 0; i < 64; i++) {
            po = pObj[i];
            if (po == 0 || po->dead != 0 || po->invisible != 0 || (po->flags & 1)) {
                continue;
            }
            if ((po->flags & 4) && GameObjectOverlap(obj, po, 1)) {
                if (po->vulnerable & 0x100) {
                    if (po->flags & 0x40000) {
                        po->kill_contact = 1;
                    } else if (how == 2) {
                        KillGameObject(po, 0x15);
                    } else if (how != 0) {
                        KillGameObject(po, 4);
                    } else {
                        FlyGameObject(po, temp_yrot);
                        KillGameObject(po, 1);
                    }
                }
                ((struct creature_s *)po->parent)->hit_type = hit_type;
                return 1;
            }
        }
    }
    return 0;
}


s32 WipeCreatures(RailPosition *rpos) {
    s32 i;
    struct obj_s *po;
    struct nuvec_s pos;

    if (level_part_2 != 0) {
        return 0;
    }
    for (i = 0; i < 64; i++) {
        po = pObj[i];
        if (po == 0 || po->dead != 0 || po->invisible != 0 || (po->flags & 1)) {
            continue;
        }
        if (po->flags & 4) {
            if (po->flags & 0x2000) {
                if (po->draw_frame == 0 || po->pLOCATOR == 0 ||
                    (po->model != 0 && po->model->pLOCATOR[0] == 0)) {
                    continue;
                }
                pos.x = po->pLOCATOR->_30;
                pos.y = po->pLOCATOR->_31;
                pos.z = po->pLOCATOR->_32;
            } else {
                pos = po->pos;
            }
            GetALONG(&pos, &po->RPos, po->RPos.iRAIL, po->RPos.iALONG, 1);
            if (FurtherALONG(rpos->iRAIL, rpos->iALONG, rpos->fALONG,
                             po->RPos.iRAIL, po->RPos.iALONG, po->RPos.fALONG)) {
                FlyGameObject(po, NuAtan2D(rpos->pos.x - pos.x, rpos->pos.z - pos.z));
                KillGameObject(po, 1);
            }
        }
    }
    return 0;
}


s32 KillGameObject(struct obj_s *obj, s32 anim) {
    struct nuvec_s pos;
    s32 vehicle;
    s32 sfx;
    s32 check_anim;
    u16 angle;
    f32 delay;
    f32 duration;
    s32 control;

    vehicle = -1;
    sfx = -1;
    obj->dead = anim;
    pos.x = obj->pos.x;
    pos.z = obj->pos.z;
    pos.y = obj->pos.y + (obj->bot + obj->top) * obj->SCALE * 0.5f;
    if (obj == &player->obj && VEHICLECONTROL == 1 && obj->vehicle != -1) {
        vehicle = obj->vehicle;
    }
    control = VEHICLECONTROL;
    if (anim == 0x14) {
        anim = 6;
        if (obj->flags & 1) {
            if (control == 2) {
                anim = 0xE;
            } else if (control == 1 && obj->vehicle != -1) {
                anim = 0xB;
            } else if (Level == 0xF) {
                anim = 0xD;
            }
        }
        obj->dead = anim;
    }

    if (obj->flags & 1) {
        if (obj->transporting != 0 || InvincibilityCHEAT != 0) {
            obj->dead = 0;
            return 0;
        }
        if (vehicle == 0x44 || vehicle == 0xB2 || vehicle == 0x20 || vehicle == 0x3B ||
            vehicle == 0x53) {
            obj->dead = 0x16;
        } else if (anim == 0x11 && VEHICLECONTROL == 1 && obj->vehicle == 0x20) {
            obj->dead = 1;
        }
        if (obj->dead != 0) {
            old_bonus_crates = bonus_crates_destroyed;
            NewBuzz(&player->rumble, 0x32);
            NewRumble(&player->rumble, 0xFF);
            if (vehicle == 0xA1) {
                sfx = 0x71;
            }
        }
        if (sfx != -1) {
            GameSfx(sfx, &obj->pos);
        }
    }
    obj->die_time = 0.0f;

    sfx = -1;
    delay = obj->die_time;
    angle = NuAtan2D(GameCam->pos.x - obj->pos.x, GameCam->pos.z - obj->pos.z);
    check_anim = (obj->dead != 0x16);
    switch (obj->character) {
    case 0:
        sfx = 0x15;
        switch (obj->dead) {
        case 6:
            obj->die_model[0] = CRemap[10];
            sfx = 0x16;
            delay = 1.0f;
            obj->die_model[1] = -1;
            obj->die_action = 0x18;
            break;
        case 14:
            obj->die_model[0] = CRemap[0x9D];
            delay = 1.0f;
            obj->die_model[1] = -1;
            obj->die_action = 0x18;
            break;
        case 5:
            obj->die_model[0] = CRemap[10];
            sfx = 8;
            delay = 1.0f;
            obj->die_model[1] = -1;
            obj->die_action = 0x19;
            break;
        case 3:
            obj->die_model[0] = CRemap[0];
            obj->die_model[1] = CRemap[7];
            obj->die_action = 0x1D;
            break;
        case 8:
            sfx = 9;
            obj->die_model[0] = CRemap[0];
            obj->die_model[1] = CRemap[0x4F];
            obj->die_action = 0xF;
            break;
        case 9:
            obj->die_model[0] = CRemap[0];
            obj->die_model[1] = -1;
            obj->die_action = 0xF;
            break;
        case 10:
            obj->die_model[0] = CRemap[0];
            delay = 1.0f;
            obj->die_model[1] = -1;
            obj->die_action = 0x17;
            break;
        case 11:
            obj->die_model[0] = CRemap[0];
            if (obj->vehicle != 0x63 && obj->vehicle != 0x36 && obj->vehicle != 0x81 &&
                obj->vehicle != 0x53 && obj->vehicle != 0x8B) {
                obj->die_model[1] = CRemap[obj->vehicle];
            }
            delay = 1.0f;
            obj->die_action = 0x59;
            break;
        case 12:
            obj->die_model[0] = CRemap[0];
            if (obj->vehicle != 0x63 && obj->vehicle != 0x36 && obj->vehicle != 0x81 &&
                obj->vehicle != 0x53 && obj->vehicle != 0x8B) {
                obj->die_model[1] = CRemap[obj->vehicle];
            }
            delay = 1.0f;
            obj->die_action = 0x5B;
            break;
        case 13:
            obj->die_model[0] = CRemap[0x97];
            sfx = 7;
            delay = 1.0f;
            obj->die_model[1] = -1;
            obj->die_action = 1;
            break;
        case 15:
            obj->die_model[0] = CRemap[0x9E];
            delay = 1.0f;
            obj->die_model[1] = -1;
            obj->die_action = 0xE;
            break;
        case 16:
            obj->die_model[0] = CRemap[0];
            sfx = 0x1D;
            delay = 1.0f;
            obj->die_model[1] = -1;
            obj->die_action = 0x10;
            break;
        case 17:
            if (LBIT & 0x400000040) {
                obj->die_model[0] = CRemap[0x73];
                obj->die_model[1] = CRemap[0xB0];
            } else {
                obj->die_model[0] = CRemap[0];
                obj->die_model[1] = CRemap[0xAF];
            }
            delay = 2.0f;
            obj->die_action = 0x12;
            break;
        case 18:
            delay = 1.0f;
            sfx = 0x23;
            obj->die_model[0] = CRemap[0];
            obj->die_model[1] = CRemap[0xA8];
            obj->die_action = 0x10;
            break;
        case 19:
            obj->die_model[0] = CRemap[0];
            sfx = 0x24;
            delay = 1.0f;
            obj->die_model[1] = -1;
            obj->die_action = 0x11;
            break;
        case 4:
            obj->die_model[0] = CRemap[0];
            check_anim = 0;
            delay = 3.0f;
            obj->die_model[1] = -1;
            obj->die_action = -1;
            break;
        case 7:
        default:
            obj->die_model[0] = CRemap[0];
            sfx = 0x14;
            delay = 1.0f;
            obj->die_model[1] = -1;
            obj->die_action = 0xE;
            break;
        }
        break;
    case 1:
        switch (obj->dead) {
        case 10:
            obj->die_model[0] = CRemap[1];
            delay = 1.0f;
            obj->die_model[1] = -1;
            obj->die_action = 0x17;
            break;
        case 11:
            obj->die_model[0] = CRemap[1];
            delay = 1.0f;
            obj->die_model[1] = CRemap[obj->vehicle];
            obj->die_action = 0x59;
            break;
        case 12:
            obj->die_model[0] = CRemap[1];
            if (obj->vehicle != 0x81) {
                obj->die_model[1] = CRemap[obj->vehicle];
            }
            delay = 1.0f;
            obj->die_action = 0x5B;
            break;
        case 3:
            obj->die_model[0] = CRemap[1];
            obj->die_model[1] = CRemap[7];
            obj->die_action = 0x1D;
            break;
        default:
            obj->die_model[0] = CRemap[1];
            obj->die_model[1] = -1;
            obj->die_action = 0xE;
            break;
        }
        break;
    case 0xF:
        obj->die_model[0] = CRemap[0xF];
        obj->die_action = 0xE;
        if (obj->dead == 6) {
            obj->die_model[0] = CRemap[0x10];
            if (obj->die_model[0] != -1) {
                obj->die_action = 0x18;
            }
        }
        obj->die_model[1] = -1;
        break;
    default:
        obj->die_model[0] = CRemap[obj->character];
        obj->die_model[1] = -1;
        obj->die_action = -1;
        if (obj->dead != 4 && obj->dead != 7 && obj->dead != 0x15) {
            obj->dead = 1;
        }
        check_anim = 0;
        break;
    }

    if (obj->die_model[0] == -1) {
        obj->dead = 1;
    } else if (check_anim && CModel[obj->die_model[0]].anmdata[obj->die_action] == 0) {
        obj->dead = 1;
    }
    if (obj->dead == 1) {
        obj->oldpos = obj->pos;
        obj->die_model[0] = CRemap[obj->character];
        obj->die_action = -1;
    }

    switch (obj->dead) {
    case 3:
        plr_invisibility_time = 0.0f;
        obj->hdg = angle - 0x8000;
        obj->die_duration =
            (CModel[obj->die_model[0]].anmdata[obj->die_action]->time - 1.0f) *
            (1.0f / (CModel[obj->die_model[0]].animlist[obj->die_action]->speed * 30.0f)) + delay;
        BlendGameCamera(GameCam, obj->die_duration * 0.5f);
        break;
    case 8:
        obj->pos.y = obj->layer_shadow;
        obj->hdg = angle - 0x8000;
        BlendGameCamera(GameCam, 0.5f);
        obj->die_duration =
            (CModel[obj->die_model[0]].anmdata[obj->die_action]->time - 1.0f) *
            (1.0f / (CModel[obj->die_model[0]].animlist[obj->die_action]->speed * 30.0f)) + delay;
        BlendGameCamera(GameCam, obj->die_duration * 0.5f);
        break;
    case 17:
        obj->hdg = angle;
        obj->xrot = 0;
        obj->zrot = 0;
        obj->die_duration =
            (CModel[obj->die_model[0]].anmdata[obj->die_action]->time - 1.0f) *
            (1.0f / (CModel[obj->die_model[0]].animlist[obj->die_action]->speed * 30.0f)) + delay;
        break;
    case 2:
    case 5:
    case 6:
    case 10:
    case 11:
    case 12:
    case 14:
    case 16:
    case 18:
    case 19:
        duration = (CModel[obj->die_model[0]].anmdata[obj->die_action]->time - 1.0f) *
                   (1.0f / (CModel[obj->die_model[0]].animlist[obj->die_action]->speed * 30.0f));
        obj->die_duration = duration;
        if (obj->dead == 2 && obj == &player->obj) {
            BlendGameCamera(GameCam, duration * 0.5f);
        }
        obj->die_duration = obj->die_duration + delay;
        break;
    case 9:
    case 13:
        obj->hdg = angle;
        obj->die_duration =
            (CModel[obj->die_model[0]].anmdata[obj->die_action]->time - 1.0f) *
            (1.0f / (CModel[obj->die_model[0]].animlist[obj->die_action]->speed * 30.0f)) + delay;
        break;
    case 7:
        obj->die_duration = 0.0f;
        break;
    case 15:
        obj->hdg = angle;
        obj->die_duration = 3.0f;
        obj->pos.y = obj->pos.y + (obj->bot + obj->top) * obj->SCALE * 0.5f;
        BlendGameCamera(GameCam, 1.0f);
        break;
    case 4:
        obj->die_duration = 0.0f;
        obj->die_duration = delay + obj->die_duration;
        AddGameDebris(7, &pos);
        break;
    case 22:
        obj->die_model[0] = CRemap[obj->character];
        obj->die_model[1] = -1;
        sfx = 0x3B;
        obj->die_duration = 3.0f;
        obj->die_action = -1;
        AddMechanicalDebris(&obj->pos, vehicle);
        break;
    case 21:
        obj->die_duration = 0.0f;
        AddGameDebris(0x8E, &pos);
        sfx = 0x47;
        break;
    default:
        obj->die_duration = 0.5f;
        break;
    }

    if (obj->die_action != -1 && obj->dead != 10) {
        ResetAnimPacket(&obj->anim, obj->die_action);
    }
    if (obj->flags & 0x20) {
        boss_dead = (Game.hub[Hub].flags & 4) ? 2 : 1;
    }
    if (obj->character == 0x46) {
        AddExtraLife(&pos, 2);
    }
    if (sfx != -1) {
        GameSfx(sfx, &obj->pos);
    }
    return 1;
}


inline void CountGameObjects(void) {
    s32 i;

    for (i = 64; i > 0; i--) {
        if (pObj[i - 1] != 0) {
            break;
        }
    }
    GAMEOBJECTCOUNT = i;
}

inline s32 AddGameObject(struct obj_s *obj, struct obj_s *parent) {
    s32 i;
    s32 added;

    memset(obj, 0, 0x188);
    obj->SCALE = 1.0f;
    obj->scale = 1.0f;
    obj->RPos.iRAIL = -1;
    obj->RPos.iALONG = -1;
    obj->layer_type = -1;
    obj->roof_type = -1;
    obj->reflect_y = 2000000.0f;

    for (i = 0; i < 64; i++) {
        if (pObj[i] == 0) {
            break;
        }
    }
    if (i < 64) {
        pObj[i] = obj;
        obj->parent = parent;
        obj->i = i;
        added = 1;
    } else {
        added = 0;
    }
    CountGameObjects();
    return added;
}

inline void RemoveGameObject(struct obj_s *obj) {
    s32 i;

    for (i = 0; i < 64; i++) {
        if (pObj[i] == obj) {
            pObj[i] = 0;
            i = 64;
        }
    }
    CountGameObjects();
}


void AddProjectile(struct nuvec_s *src, struct nuvec_s *dst, struct nuvec_s *mom, s32 type,
                   u16 yrot, struct obj_s *owner) {
    struct nuvec_s v;
    struct projectile_s *p;
    f32 f;
    f32 y;
    s32 sfx;
    s32 sfxvol;

    p = &Projectile[i_projectile];
    if (p->active) {
        RemoveGameObject(&p->obj);
    }
    memset(p, 0, sizeof(struct projectile_s));
    sfx = -1;
    sfxvol = -1;
    if (AddGameObject(&p->obj, &p->obj)) {
        p->obj.max.y = 0.1f;
        p->obj.min.y = -0.1f;
        p->obj.RADIUS = 0.1f;
        p->obj.radius = 0.1f;
        p->obj.SCALE = 1.0f;
        p->obj.scale = 1.0f;
        p->obj.flags = 8;
        if (src != 0) {
            p->obj.pos = *src;
        } else {
            p->obj.pos = v000;
        }
        if (mom != 0) {
            p->obj.mom = *mom;
        } else {
            p->obj.mom = v000;
        }
        p->srcpos = p->obj.pos;
        if (dst != 0) {
            p->dstpos = *dst;
        } else {
            p->dstpos = player->obj.pos;
        }
        p->type = type;
        p->time = 0.0f;
        p->object = -1;
        p->kill = -1;

        switch (p->type) {
        case 14:
            if (qrand() < 0x8000) {
                struct nuvec_s v0 = {-3.73f, 0.0f, 4.26f};
                struct nuvec_s v1 = {3.73f, 0.0f, 4.26f};
                struct nuvec_s v2 = {1.63f, 0.0f, 2.44f};
                struct nuvec_s v3 = {-1.63f, 0.0f, 2.44f};
                struct nuvec_s pos[2];

                f = qrand() * (1.0f / 65535.0f);
                pos[0].x = v0.x + (v1.x - v0.x) * f;
                pos[0].y = v0.y + (v1.y - v0.y) * f;
                pos[0].z = v0.z + (v1.z - v0.z) * f;
                pos[1].x = v3.x + (v2.x - v3.x) * f;
                pos[1].y = v3.y + (v2.y - v3.y) * f;
                pos[1].z = v3.z + (v2.z - v3.z) * f;
                f = qrand() * (1.0f / 65535.0f);
                v.x = pos[1].x + (pos[0].x - pos[1].x) * f;
                v.y = pos[1].y + (pos[0].y - pos[1].y) * f;
                v.z = pos[1].z + (pos[0].z - pos[1].z) * f;
                p->obj.pos = v;
            } else {
                p->obj.pos = player->obj.pos;
            }
            p->obj.shadow = NewShadow(&p->obj.pos, 0.0f);
            if (p->obj.shadow != 2000000.0f) {
                p->obj.pos.y = p->obj.shadow;
            } else {
                p->obj.shadow = p->obj.pos.y;
            }
            p->obj.pos.y += 7.0f;
            p->obj.mom.y = -0.099999994f;
            p->obj.flags |= 0xC00;
            p->object = 0x98;
            p->duration = 2.0f;
            sfx = 8;
            p->obj.xrot = qrand();
            p->obj.hdg = qrand();
            break;
        case 3:
            p->obj.hdg = yrot;
            f = 0.06f;
            p->obj.flags |= 0x4800;
            p->duration = 3.0f;
            p->obj.mom.x = NuTrigTable[p->obj.hdg] * f;
            p->obj.mom.z = NuTrigTable[(p->obj.hdg + 0x4000) & 0xFFFF] * f;
            p->object = 0x3C;
            p->obj.xrot = 0x8000;
            p->obj.mom.y = 0.0f;
            break;
        case 11:
            if (dst != 0) {
                p->duration = NuVecDist(src, dst, 0) / 5.0f;
                p->obj.mom.x = (dst->x - src->x) / (p->duration * 50.0f);
                p->obj.mom.y = (dst->y - src->y) / (p->duration * 50.0f);
                p->obj.mom.z = (dst->z - src->z) / (p->duration * 50.0f);
            } else {
                p->obj.mom.x = NuTrigTable[yrot] * 5.0f;
                p->obj.mom.z = NuTrigTable[(u16)(yrot + 0x4000)] * 5.0f;
                p->duration = 1.0f;
            }
            p->object = -1;
            p->obj.flags |= 0x4800;
            break;
        case 2:
            p->obj.RADIUS = 0.2f;
            p->obj.radius = 0.2f;
            p->obj.min.y = -p->obj.RADIUS;
            p->obj.max.y = 0.2f;
            p->duration = 2.5f;
            p->obj.flags |= 0x4800;
            p->obj.mom.y = 0.0f;
            if (owner != 0) {
                p->obj.shadow = owner->pos.y + owner->bot * owner->SCALE;
            } else {
                p->obj.shadow = p->obj.pos.y;
            }
            p->object = 1;
            jonglow = 0;
            jonglowsize = 0.0f;
            if (player->obj.character == 0) {
                p->kill = 0xF;
            }
            sfxvol = FIZZLEVOL;
            sfx = 0x4B;
            break;
        case 1:
            p->duration = 1.0f;
            p->obj.flags |= 0x4800;
            p->object = 0;
            break;
        case 12:
            jonwidth = 0.0f;
        case 13:
            p->obj.hdg = yrot;
            y = NewShadowMaskPlat((owner != 0) ? &owner->pos : &p->obj.pos, 0.0f, -1);
            if (p->type == 13) {
                p->obj.min.y = -0.375f;
                p->obj.max.y = 0.375f;
                if (y != 2000000.0f) {
                    p->obj.pos.y = y + 0.375f;
                }
                p->obj.RADIUS = 0.375f;
                p->obj.radius = 0.375f;
            } else {
                p->obj.min.y = -0.5f;
                p->obj.max.y = 0.5f;
                if (y != 2000000.0f) {
                    p->obj.pos.y = y + 0.75f + 0.5f;
                }
                p->obj.RADIUS = 0.5f;
                p->obj.radius = 0.5f;
            }
            p->obj.min.x = -1.0f;
            p->obj.min.z = -0.1f;
            p->obj.max.z = 0.1f;
            p->duration = 3.0f;
            p->obj.mom.x = NuTrigTable[yrot] * 5.0f * 0.02f;
            p->obj.mom.z = NuTrigTable[(u16)(yrot + 0x4000)] * 5.0f * 0.02f;
            p->obj.hdg = yrot;
            p->object = -1;
            p->obj.max.x = 1.0f;
            p->obj.SCALE = 1.0f;
            p->obj.scale = 1.0f;
            p->obj.flags |= 0xC00;
            if (p->type == 13) {
                jonwidth2 = 0.0f;
            }
            p->kill = 4;
            break;
        case 4:
            p->obj.flags |= 0x4000;
            p->obj.RADIUS = 0.1f;
            p->obj.radius = 0.1f;
            p->duration = 2.0f;
            p->object = 0x6A;
            p->obj.hdg = yrot - 0x8000;
            sfx = 0x88;
            p->obj.direction = qrand() / 0x8000;
            p->obj.zrot = qrand();
            p->obj.SCALE = 0.6f;
            break;
        case 10:
            p->obj.flags |= 0x4000;
            p->object = 0x6B;
            p->duration = 2.0f;
            sfx = 0x5D;
            p->obj.hdg = qrand();
            p->obj.xrot = qrand();
            p->obj.direction = qrand() / 0x8000;
            p->owner_safety = 10;
            p->obj.shadow = NewShadowMaskPlat(&p->obj.pos, 0.0f, -1);
            break;
        case 5:
            p->duration = 1.333f;
            p->obj.mom.x = NuTrigTable[yrot] * 5.0f * 0.02f;
            p->obj.mom.z = NuTrigTable[(u16)(yrot + 0x4000)] * 5.0f * 0.02f;
            p->obj.flags |= 0x4800;
            p->object = 0x53;
            p->obj.hdg = yrot - 0x8000;
            break;
        case 6:
            p->obj.RADIUS = 0.25f;
            p->obj.radius = 0.25f;
            p->obj.min.y = -p->obj.RADIUS;
            p->obj.max.y = 0.25f;
            p->duration = 2.0f;
            p->object = -1;
            y = NewShadowMaskPlat(&p->dstpos, 0.0f, -1);
            if (y != 2000000.0f) {
                p->dstpos.y = y - p->obj.min.y * p->obj.SCALE;
            }
            p->obj.flags |= 0xC00;
            break;
        case 16:
        case 17:
            if (owner != 0) {
                p->obj.mom.x = p->obj.pos.x - owner->pos.x;
                p->obj.mom.y = 0.0f;
                p->obj.mom.z = p->obj.pos.z - owner->pos.z;
                f = 0.12f / NuFsqrt(p->obj.mom.x * p->obj.mom.x + p->obj.mom.z * p->obj.mom.z);
                p->duration = 0.5f;
                p->obj.flags |= 0xC00;
                p->object = -1;
                p->obj.mom.x *= f;
                p->obj.mom.z *= f;
            }
            break;
        case 8:
            p->obj.RADIUS = 0.1f;
            p->obj.radius = 0.1f;
            p->obj.min.y = -p->obj.RADIUS;
            p->obj.max.y = 0.1f;
            p->duration = 1.0f;
            p->obj.hdg = yrot;
            p->object = 100;
            y = NewShadowMaskPlat(&p->dstpos, 0.0f, -1);
            if (y != 2000000.0f) {
                p->dstpos.y = y - p->obj.min.y * p->obj.SCALE;
            }
            p->obj.xrot = 0x2000;
            p->obj.flags |= 0xC00;
            break;
        case 9:
            p->obj.RADIUS = 0.1f;
            p->obj.radius = 0.1f;
            p->obj.min.y = -p->obj.RADIUS;
            p->obj.max.y = 0.1f;
            p->duration = 1.0f;
            p->obj.hdg = yrot;
            p->object = 0x95;
            y = NewShadowMaskPlat(&p->dstpos, 0.0f, -1);
            if (y != 2000000.0f) {
                p->dstpos.y = y - p->obj.min.y * p->obj.SCALE;
            }
            p->obj.xrot = 0xAAB;
            p->obj.flags |= 0xC00;
            p->kill = 2;
            sfx = 0x83;
            break;
        case 15:
            p->obj.RADIUS = 0.1f;
            p->obj.radius = 0.1f;
            p->obj.min.y = -p->obj.RADIUS;
            p->obj.max.y = 0.1f;
            p->duration = 1.0f;
            p->obj.hdg = yrot;
            p->object = -1;
            y = NewShadowMaskPlat(&p->dstpos, 0.0f, -1);
            if (y != 2000000.0f) {
                p->dstpos.y = y - p->obj.min.y * p->obj.SCALE;
            }
            p->obj.flags |= 0xC00;
            break;
        case 7:
            p->duration = 3.0f;
            p->obj.mom.x = NuTrigTable[yrot] * 0.02f;
            p->obj.mom.z = NuTrigTable[(u16)(yrot + 0x4000)] * 0.02f;
            p->obj.flags |= 0x800;
            p->object = 0;
            break;
        default:
            p->duration = 3.0f;
            p->obj.flags |= 0x400;
            p->object = 0;
            break;
        }

        if (p->type == 4 || p->type == 5) {
            f = p->duration * 50.0f;
            v.x = p->obj.mom.x * f;
            v.y = p->obj.mom.y * f;
            v.z = p->obj.mom.z * f;
            f = NuVecMag(&v);
            if (NewRayCast(&p->obj.pos, &v, 0.0f)) {
                p->duration *= NuVecMag(&v) / f;
            }
        }
        p->obj.bot = p->obj.min.y;
        p->obj.top = p->obj.max.y;
        p->active = 1;
    }

    i_projectile++;
    if (i_projectile == 16) {
        i_projectile = 0;
    }
    if (sfx != -1) {
        if (sfxvol != -1) {
            gamesfx_effect_volume = sfxvol;
        }
        GameSfx(sfx, &p->obj.pos);
    }
}


inline void NewTopBot(struct obj_s *obj) {
    obj->objbot = obj->pos.y + obj->bot * obj->SCALE;
    obj->objtop = obj->pos.y + obj->top * obj->SCALE;
}

inline s32 HitItems(struct obj_s *obj) {
    s32 i;
    struct obj_s *item;

    if (level_part_2 != 0) {
        return 0;
    }
    for (i = 0; i < 64; i++) {
        item = pObj[i];
        if (item != 0 && item->dead == 0 && item->invisible == 0 &&
            (item->flags & 0x10) && GameObjectOverlap(obj, item, 0)) {
            PickupItem(item);
            return 1;
        }
    }
    return 0;
}

inline s32 GetDieAnim(struct obj_s *obj, s32 anim) {
    if (VEHICLECONTROL == 1 && obj->vehicle != -1) {
        if (anim == 6 && obj->vehicle == 0x99) {
            return 0xC;
        }
        return 0xB;
    }
    if (VEHICLECONTROL == 2) {
        return 0xE;
    }
    if (anim == -1) {
        if (Level == 0x17 || qrand() <= 0x7FFF) {
            return 2;
        }
        return 3;
    }
    return anim;
}

inline s32 KillPlayer(struct obj_s *obj, s32 anim)
{
    MaskInfo *mask;

    if (obj->dead) {
        return 0;
    }
    if (obj->finished) {
        return 0;
    }
    if (obj->invincible) {
        return 0;
    }
    if (vtog_time < vtog_duration && vtog_blend != 0) {
        return 0;
    }
    mask = obj->mask;
    if (mask != 0 && mask->active != 0 && (LDATA->flags & 0xE00) == 0) {
        if (mask->active < 3) {
            LoseMask(obj);
        }
        return 0;
    }
    return KillGameObject(obj, anim);
}


void UpdateProjectiles(void) {
    struct nuvec_s vec;
    struct creature_s *plr;
    struct projectile_s *p;
    f32 tmul;
    f32 speed;
    f32 d;
    f32 f;
    f32 width;
    f32 r;
    f32 dx;
    f32 dz;
    s32 i;
    s32 sfx;
    s32 sfxvol;
    s32 expired;
    s32 kill;
    s32 loop;
    s32 hit;

    if (level_part_2 != 0) {
        return;
    }
    plr = player;
    GetTopBot(plr);
    NewTopBot(&plr->obj);
    p = Projectile;
    for (i = 0; i < 16; i++, p++) {
        if (!p->active) {
            continue;
        }
        expired = 0;
        sfx = -1;
        sfxvol = -1;
        p->time += 0.02f;
        if (p->time >= p->duration) {
            expired = 1;
            p->active = 0;
        } else {
            tmul = p->time / p->duration;
            p->obj.oldpos = p->obj.pos;
            if (p->owner_safety != 0) {
                p->owner_safety--;
            }

            switch (p->type) {
            case 14:
                p->obj.xrot += 0x384;
                p->obj.zrot += 0x514;
                NuVecAdd(&p->obj.pos, &p->obj.pos, &p->obj.mom);
                if (p->obj.pos.y + p->obj.bot * p->obj.SCALE < p->obj.shadow) {
                    p->obj.pos.y = p->obj.shadow - p->obj.bot * p->obj.SCALE;
                    p->active = 0;
                }
                AddVariableShotDebrisEffect(GDeb[116].i, &p->obj.pos, 1, 0, 0);
                break;
            case 3:
                p->obj.xrot -= 0xA3D;
                NuVecAdd(&p->obj.pos, &p->obj.pos, &p->obj.mom);
                break;
            case 2:
                if (plr->obj.dead == 0) {
                    dx = plr->obj.pos.x - p->obj.pos.x;
                    dz = plr->obj.pos.z - p->obj.pos.z;
                    speed = 0.035f;
                    d = NuFsqrt(dx * dx + dz * dz);
                    if (d > speed) {
                        f = 1.0f / d * speed;
                        p->obj.pos.x += dx * f;
                        p->obj.pos.z += dz * f;
                    }
                    NuVecSub(&p->obj.mom, &p->obj.pos, &p->obj.oldpos);
                } else {
                    NuVecAdd(&p->obj.pos, &p->obj.pos, &p->obj.mom);
                }
                p->obj.pos.y += (p->obj.shadow + p->obj.RADIUS * 2.0f - p->obj.pos.y) * 0.02f * 5.0f;
                AddVariableShotDebrisEffect(GDeb[37].i, &p->obj.pos, 1, 0, 0);
                break;
            case 1:
                p->obj.pos.x = p->srcpos.x + (p->dstpos.x - p->srcpos.x) * tmul;
                p->obj.pos.y = p->srcpos.y + (p->dstpos.y - p->srcpos.y) * tmul;
                p->obj.pos.z = p->srcpos.z + (p->dstpos.z - p->srcpos.z) * tmul;
                break;
            case 4:
                NuVecAdd(&p->obj.pos, &p->obj.pos, &p->obj.mom);
                if (p->obj.direction == 0) {
                    p->obj.zrot += 0x51E;
                } else {
                    p->obj.zrot -= 0x51E;
                }
                if (HitCreatures(&p->obj, 2, 5) || HitCrates(&p->obj, 1) || HitItems(&p->obj) ||
                    HitWumpa(&p->obj, 1)) {
                    p->active = 0;
                }
                AddVariableShotDebrisEffect(GDeb[20].i, &p->obj.pos, 1, 0, 0);
                if (qrand() < 0x1000) {
                    sfx = 0x92;
                }
                break;
            case 5:
                NuVecAdd(&p->obj.pos, &p->obj.pos, &p->obj.mom);
                AddVariableShotDebrisEffect(GDeb[21].i, &p->obj.pos, 1, 0, 0);
                break;
            case 11:
                NuVecAdd(&p->obj.pos, &p->obj.pos, &p->obj.mom);
                AddVariableShotDebrisEffect(GDeb[114].i, &p->obj.pos, 1, 0, 0);
                break;
            case 10:
                p->obj.mom.y += (-terminal_fall_speed - p->obj.mom.y) * 0.02f;
                NuVecAdd(&p->obj.pos, &p->obj.pos, &p->obj.mom);
                AddVariableShotDebrisEffect(GDeb[20].i, &p->obj.pos, 1, 0, 0);
                if (p->obj.direction == 0) {
                    p->obj.hdg += 0x249;
                } else {
                    p->obj.hdg -= 0x249;
                }
                p->obj.xrot += 0x51E;
                if (p->obj.shadow != 2000000.0f &&
                    p->obj.pos.y + p->obj.bot * p->obj.SCALE < p->obj.shadow) {
                    p->obj.pos.y = p->obj.shadow - p->obj.bot * p->obj.SCALE;
                    p->active = 0;
                } else if (HitCreatures(&p->obj, 2, 5) || HitCrates(&p->obj, 1) ||
                           HitItems(&p->obj) || HitWumpa(&p->obj, 1)) {
                    p->active = 0;
                }
                if (qrand() < 0x1000) {
                    sfx = 0x92;
                }
                break;
            case 6:
                p->obj.pos.x = p->srcpos.x + (p->dstpos.x - p->srcpos.x) * tmul;
                p->obj.pos.y = p->srcpos.y + (p->dstpos.y - p->srcpos.y) * tmul;
                p->obj.pos.z = p->srcpos.z + (p->dstpos.z - p->srcpos.z) * tmul;
                p->obj.pos.y += NuTrigTable[(u16)(tmul * 32768.0f)] * 10.0f;
                AddVariableShotDebrisEffect(GDeb[135].i, &p->obj.pos, 1, 0, 0);
                break;
            case 8:
                p->obj.pos.x = p->srcpos.x + (p->dstpos.x - p->srcpos.x) * tmul;
                p->obj.pos.y = p->srcpos.y + (p->dstpos.y - p->srcpos.y) * tmul;
                p->obj.pos.z = p->srcpos.z + (p->dstpos.z - p->srcpos.z) * tmul;
                p->obj.xrot += 0x47A;
                p->obj.pos.y += NuTrigTable[(u16)(tmul * 32768.0f)] * 1.5f;
                break;
            case 9:
                p->obj.pos.x = p->srcpos.x + (p->dstpos.x - p->srcpos.x) * tmul;
                p->obj.pos.y = p->srcpos.y + (p->dstpos.y - p->srcpos.y) * tmul;
                p->obj.pos.z = p->srcpos.z + (p->dstpos.z - p->srcpos.z) * tmul;
                p->obj.xrot -= 0xDA;
                p->obj.pos.y += NuTrigTable[(u16)(tmul * 32768.0f)];
                break;
            case 15:
                p->obj.pos.x = p->srcpos.x + (p->dstpos.x - p->srcpos.x) * tmul;
                p->obj.pos.y = p->dstpos.y + (p->srcpos.y - p->dstpos.y) *
                               NuTrigTable[(u16)(tmul * 16384.0f + 16384.0f)];
                p->obj.pos.z = p->srcpos.z + (p->dstpos.z - p->srcpos.z) * tmul;
                AddVariableShotDebrisEffect(GDeb[144].i, &p->obj.pos, 1, 0, 0);
                break;
            case 7:
                NuVecAdd(&p->obj.pos, &p->obj.pos, &p->obj.mom);
                p->obj.SCALE = tmul * 2.0f + 1.0f;
                p->obj.RADIUS = p->obj.radius * p->obj.SCALE;
                break;
            case 16:
                p->obj.mom.y += GRAVITY;
                p->obj.pos.x += p->obj.mom.x;
                p->obj.pos.y += p->obj.mom.y;
                p->obj.pos.z += p->obj.mom.z;
                AddVariableShotDebrisEffect(GDeb[135].i, &p->obj.pos, 1, 0, 0);
                break;
            case 17:
                p->obj.mom.y += GRAVITY;
                p->obj.pos.x += p->obj.mom.x;
                p->obj.pos.y += p->obj.mom.y;
                p->obj.pos.z += p->obj.mom.z;
                AddVariableShotDebrisEffect(GDeb[99].i, &p->obj.pos, 1, 0, 0);
                break;
            case 12:
                NuVecAdd(&p->obj.pos, &p->obj.pos, &p->obj.mom);
                for (loop = 0; loop < 5; loop++) {
                    width = jonwidth * (f32)(qrand() - 0x8000) * (1.0f / 32768.0f);
                    vec.x = p->obj.pos.x + NuTrigTable[(p->obj.hdg + 0x4000) & 0xFFFF] * width;
                    vec.y = p->obj.pos.y;
                    vec.z = p->obj.pos.z + NuTrigTable[(p->obj.hdg + 0x8000) & 0xFFFF] * width;
                    AddVariableShotDebrisEffect(GDeb[139].i, &vec, 1, 0, p->obj.hdg);
                }
                jonwidth += 0.025f;
                if (jonwidth > 1.5f) {
                    jonwidth = 1.5f;
                }
                break;
            case 13:
                NuVecAdd(&p->obj.pos, &p->obj.pos, &p->obj.mom);
                for (loop = 0; loop < 10; loop++) {
                    width = jonwidth2 * (f32)(qrand() - 0x8000) * (1.0f / 32768.0f);
                    vec.x = p->obj.pos.x + NuTrigTable[(p->obj.hdg + 0x4000) & 0xFFFF] * width;
                    vec.y = p->obj.pos.y;
                    vec.z = p->obj.pos.z + NuTrigTable[(p->obj.hdg + 0x8000) & 0xFFFF] * width;
                    AddVariableShotDebrisEffect(GDeb[138].i, &vec, 1, 0, p->obj.hdg);
                }
                jonwidth2 += 0.025f;
                if (jonwidth2 > 1.5f) {
                    jonwidth2 = 1.5f;
                }
                break;
            default:
                NuVecAdd(&p->obj.pos, &p->obj.pos, &p->obj.mom);
                break;
            }

            NewTopBot(&p->obj);
            if ((p->obj.flags & 0x400) && p->type != 14) {
                p->obj.shadow = NewShadowMaskPlat(&p->obj.pos, 0.0f, -1);
                if (p->obj.shadow != 2000000.0f) {
                    p->obj.surface_type = ShadowInfo();
                    p->obj.reflect_y = (TerSurface[p->obj.surface_type].flags & 4) ? p->obj.shadow
                                                                                   : 2000000.0f;
                    FindAnglesZX(&ShadNorm);
                    p->obj.surface_xrot = temp_xrot;
                    p->obj.surface_zrot = temp_zrot;
                    if ((p->type == 16 || p->type == 17) &&
                        p->obj.pos.y + p->obj.bot * p->obj.SCALE < p->obj.shadow) {
                        p->obj.pos.y = p->obj.shadow - p->obj.bot * p->obj.SCALE;
                        p->active = 0;
                    }
                }
            }

            if ((p->obj.flags & 0x800) && plr->obj.dead == 0 && p->owner_safety == 0 &&
                !(p->obj.objtop < plr->obj.objbot) && !(p->obj.objbot > plr->obj.objtop)) {
                hit = 0;
                if (p->type == 12 || p->type == 13) {
                    hit = (CylinderCuboidOverlapXZ(&plr->obj.pos, &p->obj, &p->obj.pos,
                                                   plr->obj.RADIUS) != 0);
                } else {
                    dx = plr->obj.pos.x - p->obj.pos.x;
                    dz = plr->obj.pos.z - p->obj.pos.z;
                    d = dx * dx + dz * dz;
                    r = plr->obj.RADIUS + p->obj.RADIUS;
                    if (d < r * r) {
                        hit = 1;
                    }
                }
                if (hit) {
                    kill = (p->kill != -1) ? p->kill : 6;
                    KillPlayer(&plr->obj, GetDieAnim(&plr->obj, kill));
                    if (p->type == 2) {
                        AddGameDebris(0x58, &p->obj.pos);
                    }
                    p->active = 0;
                }
            }
        }

        if (!p->active) {
            switch (p->type) {
            case 9:
                AddGameDebris(3, &p->obj.pos);
                sfx = 0x20;
                break;
            case 14:
                JudderGameCamera(GameCam, 0.15f, 0);
                AddGameDebris(0x79, &p->obj.pos);
                sfx = 0x5F;
                break;
            case 10:
                AddKaboom(0x80, &p->obj.pos, 0.0f);
                AddGameDebris(5, &p->obj.pos);
                break;
            case 4:
                AddGameDebris(5, &p->obj.pos);
                GameSfx(0x3B, &p->obj.pos);
                JudderGameCamera(GameCam, 0.3f, &p->obj.pos);
                break;
            case 8:
            case 11:
            case 15:
                AddGameDebris(0x4B, &p->obj.pos);
                sfx = 0x45;
                break;
            case 6:
            case 16:
                AddGameDebris(0x88, &p->obj.pos);
                break;
            case 2:
                sfx = 0x9C;
                break;
            case 17:
                AddGameDebris(0x96, &p->obj.pos);
                break;
            case 12:
            case 13:
                if (!expired) {
                    sfx = 0x47;
                    sfxvol = 0xBFFD;
                }
                break;
            }
            RemoveGameObject(&p->obj);
        }
        if (sfx != -1) {
            if (sfxvol != -1) {
                gamesfx_effect_volume = sfxvol;
            }
            GameSfx(sfx, &p->obj.pos);
        }
    }
}


void DrawProjectiles(void) {
    struct numtx_s m;
    struct nuvec_s scale;
    struct projectile_s *p;
    struct objtab_s *ot;
    s32 i;
    s32 glow;
    f32 farclip;
    f32 farsq;
    f32 distsq;
    f32 ratio;
    f32 size;
    f32 shadow_scale;
    f32 dx;
    f32 dz;

    if (level_part_2 != 0) {
        return;
    }
    if ((LDATA->flags & 0x202) || Level == 0x1C) {
        farclip = LDATA->farclip;
    } else {
        farclip = 25.0f;
    }
    if (LDATA->farclip < farclip) {
        farclip = LDATA->farclip;
    }
    farsq = farclip * farclip;

    p = Projectile;
    glow = 0;
    for (i = 0; i < 16; i++, p++) {
        if (!p->active) {
            continue;
        }
        dx = pCam->pos.x - p->obj.pos.x;
        dz = pCam->pos.z - p->obj.pos.z;
        distsq = dx * dx + dz * dz;
        if (!(distsq < farsq)) {
            continue;
        }
        ratio = p->time / p->duration;
        switch (p->type) {
        case 3:
        case 8:
        case 9:
        case 14:
        case 15:
            NuMtxSetRotationX(&m, p->obj.xrot);
            NuMtxRotateY(&m, p->obj.hdg + 0x8000);
            NuMtxTranslate(&m, &p->obj.pos);
            break;
        case 4:
            if (p->time < 0.2f) {
                scale.x = p->time / 0.2f;
            } else {
                scale.x = 1.0f;
            }
            scale.z = scale.x * p->obj.SCALE;
            scale.y = scale.z;
            scale.x = scale.z;
            NuMtxSetScale(&m, &scale);
            NuMtxRotateZ(&m, p->obj.zrot);
            NuMtxRotateY(&m, p->obj.hdg);
            NuMtxTranslate(&m, &p->obj.pos);
            break;
        case 5:
            NuMtxSetRotationY(&m, p->obj.hdg);
            NuMtxTranslate(&m, &p->obj.pos);
            break;
        case 7:
            scale.z = p->obj.SCALE;
            scale.y = p->obj.SCALE;
            scale.x = p->obj.SCALE;
            NuMtxSetScale(&m, &scale);
            NuMtxTranslate(&m, &p->obj.pos);
            break;
        case 10:
            if (p->time < 0.2f) {
                scale.x = p->time / 0.2f;
            } else {
                scale.x = 1.0f;
            }
            scale.z = scale.x * p->obj.SCALE;
            scale.y = scale.z;
            scale.x = scale.z;
            NuMtxSetScale(&m, &scale);
            NuMtxRotateX(&m, NuTrigTable[p->obj.xrot] * 2731.0f);
            NuMtxRotateY(&m, p->obj.hdg);
            NuMtxTranslate(&m, &p->obj.pos);
            break;
        default:
            NuMtxSetTranslation(&m, &p->obj.pos);
            break;
        }
        if (p->object != -1) {
            ot = &ObjTab[p->object];
            if (ot->obj.special != 0) {
                if (p->object == 1) {
                    jonglow = (jonglow + 0x600) & 0xFFFF;
                    size = NuTrigTable[jonglow] / 6.0f + 1.5f;
                    if (Paused == 0 && FRAME == FRAMES - 1) {
                        NuLightAddSpot(&p->obj.pos, size, 0xFF200020, 0);
                    }
                    jonglowsize = size;
                    jonglowpos = p->obj.pos;
                    glow = 1;
                } else {
                    NuRndrGScnObj(ot->obj.scene->gobjs[ot->obj.special->instance->objid], &m);
                }
            }
        }
        if (distsq < farsq && (p->obj.flags & 0x4400) == 0x400 &&
            p->obj.shadow != 2000000.0f && D_00588824[0] != 0) {
            if (p->type == 6 || p->type == 8) {
                shadow_scale = 1.0f - NuTrigTable[(u16)(ratio * 32768.0f)];
            } else if (p->type == 14) {
                shadow_scale = p->obj.pos.y + p->obj.bot * p->obj.SCALE - p->obj.shadow;
                if (shadow_scale < 4.0f) {
                    shadow_scale = 5.0f;
                } else if (shadow_scale > 7.0f) {
                    shadow_scale = 0.0f;
                } else {
                    shadow_scale = 5.0f - (shadow_scale - 4.0f) * 1.666666627f;
                }
            } else {
                shadow_scale = 2.0f;
            }
            m._30 = p->obj.pos.x;
            m._31 = p->obj.shadow + 0.025f;
            m._32 = p->obj.pos.z;
            NuRndrAddShadow((struct nuvec_s *)&m._30, 0x7F, p->obj.surface_xrot, 0, p->obj.surface_zrot,
                            shadow_scale * p->obj.RADIUS);
        }
    }
    if (glow == 0 && jonglowsize > 0.0f) {
        jonglowsize -= 0.1f;
        if (Paused == 0 && FRAME == FRAMES - 1) {
            NuLightAddSpot(&jonglowpos, jonglowsize, 0xFF200020, 0);
        }
    }
}


f32 CreatureTopBelow(struct nuvec_s *pos, u32 flags) {
    s32 i;
    struct obj_s *po;
    struct nuvec_s cpos;
    f32 dx;
    f32 dz;
    f32 top;
    f32 best;

    best = 2000000.0f;
    for (i = 0; i < GAMEOBJECTCOUNT; i++) {
        po = pObj[i];
        if (po == 0 || po->invisible != 0) {
            continue;
        }
        if (po->flags & flags) {
            if (po->flags & 0x2000) {
                if (po->draw_frame == 0) {
                    continue;
                }
                if (po->pLOCATOR != 0 && po->model->pLOCATOR[0] != 0) {
                    cpos.x = po->pLOCATOR->_30;
                    cpos.y = po->pLOCATOR->_31;
                    cpos.z = po->pLOCATOR->_32;
                } else {
                    cpos = po->pos;
                }
            } else {
                cpos = po->pos;
            }
            if (po->flags & 0x8000) {
                if (!CylinderCuboidOverlapXZ(pos, po, &cpos, 0.0f)) {
                    continue;
                }
            } else {
                dx = pos->x - cpos.x;
                dz = pos->z - cpos.z;
                if (dx * dx + dz * dz > po->RADIUS * po->RADIUS) {
                    continue;
                }
            }
            top = po->pos.y + po->top * po->SCALE;
            if (best == 2000000.0f || top > best) {
                best = top;
            }
        }
    }
    return best;
}


s32 CreatureRayCast(struct nuvec_s *start, struct nuvec_s *end) {
    s32 i;
    s32 face;
    struct obj_s *po;
    struct nuvec_s s;
    struct nuvec_s e;
    struct nuvec_s cpos;
    struct nuvec_s min;
    struct nuvec_s max;
    f32 best;

    best = 1.0f;
    for (i = 0; i < GAMEOBJECTCOUNT; i++) {
        po = pObj[i];
        if (po == 0 || po->invisible != 0) {
            continue;
        }
        if (po->flags & 0x14) {
            if (po->flags & 0x2000) {
                if (po->draw_frame == 0) {
                    continue;
                }
                if (po->pLOCATOR != 0 && po->model->pLOCATOR[0] != 0) {
                    cpos.x = po->pLOCATOR->_30;
                    cpos.y = po->pLOCATOR->_31;
                    cpos.z = po->pLOCATOR->_32;
                } else {
                    cpos = po->pos;
                }
            } else {
                cpos = po->pos;
            }
            if (po->flags & 0x8000) {
                NuVecSub(&s, start, &cpos);
                NuVecSub(&e, end, &cpos);
                NuVecRotateY(&s, &s, -po->hdg);
                NuVecRotateY(&e, &e, -po->hdg);
                min.x = po->min.x * po->SCALE;
                min.y = po->min.y * po->SCALE;
                min.z = po->min.z * po->SCALE;
                max.x = po->max.x * po->SCALE;
                max.y = po->max.y * po->SCALE;
                max.z = po->max.z * po->SCALE;
                if (RayIntersectCuboid(&s, &e, &min, &max) && temp_ratio < best) {
                    best = temp_ratio;
                    face = temp_face;
                }
            } else {
                NuVecSub(&s, start, &cpos);
                NuVecSub(&e, end, &cpos);
                NuVecRotateY(&s, &s, -po->hdg);
                NuVecRotateY(&e, &e, -po->hdg);
                min.x = -po->RADIUS;
                min.y = po->min.y * po->SCALE;
                min.z = -po->RADIUS;
                max.x = po->RADIUS;
                max.y = po->max.y * po->SCALE;
                max.z = po->RADIUS;
                if (RayIntersectCuboid(&s, &e, &min, &max) && temp_ratio < best) {
                    face = temp_face;
                    best = temp_ratio;
                }
            }
        }
    }
    temp_face = face;
    temp_ratio = best;
    return best < 1.0f;
}


void PickupCrystal(void) {
    plr_crystal.count = 1;
    plr_crystal.frame = 0x19;
    plr_items |= 1;
    GameSfx(0x26, 0);
    AddPanelDebris(0.0f, -0.7f, 6, 0.125f, 0x10);
}

void PickupCrateGem(void) {
    plr_crategem.count = 1;
    plr_crategem.frame = 0x19;
    plr_items |= 2;
    GameSfx(0x26, 0);
    AddPanelDebris(-0.2f, -0.7f, 6, 0.125f, 0x10);
}

void PickupBonusGem(s32 mask) {
    plr_items |= mask;
    plr_bonusgem.item = mask;
    plr_bonusgem.count = 1;
    plr_bonusgem.frame = 0x19;
    GameSfx(0x26, 0);
    AddPanelDebris(0.2f, -0.7f, 6, 0.125f, 0x10);
}

void PickupPower(s32 type) {
    switch (type) {
    case 0xA7:
        new_power = 0;
        break;
    case 0xA5:
        new_power = 1;
        break;
    case 0xA6:
        new_power = 2;
        break;
    case 0xA2:
        new_power = 3;
        break;
    case 0xA4:
        new_power = 4;
        break;
    case 0xA3:
        new_power = 5;
        break;
    }
    Game.powerbits |= 1 << new_power;
    NewMenu(&Cursor, 0x1F, -1, -1);
    GameSfx(0x26, 0);
    if (Level != 0x15 && Level != 0x18) {
        player->obj.mom.x = 0.0f;
        player->obj.mom.z = 0.0f;
        player->slide = 0;
    }
}


void ClearGameObjects(void) {
    s32 i;

    for (i = 0; i < 0x40; i++) {
        pObj[i] = 0;
    }
}

s32 CylinderCylinderOverlapXZ(struct nuvec_s *p0, f32 r0, struct nuvec_s *p1,
                              f32 r1) {
    f32 dz;
    f32 dx;

    dx = p1->x - p0->x;
    dz = p1->z - p0->z;
    if (dx * dx + dz * dz <= r0 * r0 + r1 * r1) {
        return 1;
    }
    return 0;
}

void OldTopBot(struct obj_s *obj) {
    obj->oldobjbot = obj->objbot;
    obj->oldobjtop = obj->objtop;
}


void ScaleFlatShadow(struct nuvec_s *s, f32 y, f32 shadow, f32 scale) {
    f32 dy;

    dy = y - shadow;
    if (dy <= 0.0f) {
        s->x = scale;
    } else if (dy >= 2.0f) {
        s->x = scale * 0.5f;
    } else {
        s->x = scale - scale * 0.5f * dy * 0.5f;
    }
    s->y = 0.0f;
    s->z = s->x;
}

void KillItem(struct obj_s *obj) {
    struct creature_s *c;

    c = (struct creature_s *)obj->parent;
    obj->dead = 1;
    c->on = 0;
    c->off_wait = 2;
}

void ResetGameObject(struct obj_s *obj) {
    memset(obj, 0, 0x188);
    obj->reflect_y = 2000000.0f;
    obj->SCALE = 1.0f;
    obj->RPos.iRAIL = -1;
    obj->RPos.iALONG = -1;
    obj->layer_type = -1;
    obj->roof_type = -1;
    obj->scale = 1.0f;
}

void ResetProjectiles(void) {
    s32 i;
    s32 j;
    struct projectile_s *p;

    p = Projectile;
    for (i = 0; i < 16; i++) {
        if (p->active) {
            for (j = 0; j < 64; j++) {
                if (pObj[j] == &p->obj) {
                    pObj[j] = 0;
                    j = 64;
                }
            }
            CountGameObjects();
            p->active = 0;
        }
        p++;
    }
    jonglow = 0;
    jonglowsize = 0;
}

void PickupRelic(s32 relic) {
    plr_crystal.item = relic;
    plr_crystal.count = 1;
    plr_crystal.frame = 0x19;
    GameSfx(0x26, 0);
}
