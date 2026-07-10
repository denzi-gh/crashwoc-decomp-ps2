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

#include "creature.h"

/* Level data record; only flags at +0x24 is touched here. */
struct ldata_s {
    u8 unk_0x00[0x24];       /* 0x00 (opaque) */
    unsigned short flags;    /* 0x24 */
};

extern struct ldata_s *LDATA;

extern f32 vtog_time;
extern f32 vtog_duration;
extern s32 vtog_blend;

void LoseMask(struct obj_s *obj);
s32 KillGameObject(struct obj_s *obj, s32 anim);

extern s32 level_part_2;
extern s32 temp_xzmomset;
extern s32 GAMEOBJECTCOUNT;
extern struct obj_s *pObj[];
extern s32 temp_cuboid_side;
extern f32 temp_cuboid_bounce_angle;
extern s32 VEHICLECONTROL;
extern s32 gamesfx_effect_volume;
extern f32 NuTrigTable[];
extern struct nuvec_s v010;

extern f32 D_0062D7C0;
extern f32 D_0062D7C4;
extern f32 D_0062D7C8;
extern f32 D_0062D7CC;
extern f32 D_0062D7D0;
extern f32 D_0062D7D4;
extern f32 D_0062D7D8;

void NewTopBot(struct obj_s *obj);
s32 CylinderCuboidOverlapXZ(struct nuvec_s *pos, struct obj_s *obj,
                            struct nuvec_s *p2, f32 radius);
void AddGameDebris(s32 type, struct nuvec_s *pos);
void NuVecScale(struct nuvec_s *dst, struct nuvec_s *src, f32 s);
void GameSfx(s32 sfx, struct nuvec_s *pos);
s32 NuAtan2D(f32 x, f32 z);
f32 NuFsqrt(f32 x);
void NuVecRotateX(struct nuvec_s *dst, struct nuvec_s *src, s32 angle);
void NuVecRotateY(struct nuvec_s *dst, struct nuvec_s *src, s32 angle);
s32 GetDieAnim(struct obj_s *obj, s32 anim);
void ObjectToAtlas(struct obj_s *obj, struct creature_s *player);

void PlayerCreatureCollisions(struct obj_s *obj)
{
    s32 i;
    s32 maskflag;
    s32 mask_weak;
    struct obj_s *po;
    s32 lose;
    s32 killanim;

    if (level_part_2 != 0) {
        return;
    }
    temp_xzmomset = 0;
    if (obj->dead != 0) {
        return;
    }
    maskflag = 0;
    if (obj->mask != 0) {
        s32 act = obj->mask->active;
        if (act != 0) {
            if (LDATA->flags & 0xE00) {
                act = 0;
            }
            maskflag = act;
        }
    }
    lose = 0;
    mask_weak = maskflag < 3;
    for (i = 0; i < GAMEOBJECTCOUNT; i++) {
        struct nuvec_s cpos;
        struct nuvec_s dpos;
        s32 overlap;
        s32 is_cuboid;
        s32 side;
        s32 ctype;
        s32 dir;
        s32 hitside;
        f32 reach;
        f32 distsq;
        s32 ang;

        po = pObj[i];
        if (po == 0) {
            continue;
        }
        po->touch = 0;
        po->contact = 0;
        if (po == obj) {
            continue;
        }
        po->kill_contact = 0;
        if (po->dead != 0) {
            continue;
        }
        if (po->invisible != 0) {
            continue;
        }
        if (po->draw_frame == 0) {
            continue;
        }
        if ((po->flags & 0x4) == 0) {
            continue;
        }
        if (po->flags & 0x1000) {
            continue;
        }
        NewTopBot(obj);
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
            overlap = CylinderCuboidOverlapXZ(&obj->pos, po, &cpos, obj->RADIUS);
            is_cuboid = 1;
            side = temp_cuboid_side;
        } else {
            f32 dx = obj->pos.x - cpos.x;
            f32 dz = obj->pos.z - cpos.z;
            f32 r = obj->RADIUS + po->RADIUS;
            distsq = dx * dx + dz * dz;
            overlap = (r * r < distsq) ? 0 : 1;
            is_cuboid = 0;
            side = 8;
        }

        po->objbot = cpos.y + po->bot * po->SCALE;
        po->objtop = cpos.y + po->top * po->SCALE;

        if (!overlap) {
            f32 f1;
            if (po->attack & 0x2) {
                continue;
            }
            if ((obj->attack & 0x2) && (po->vulnerable & 0x2)) {
                goto vertical;
            }
            if (obj->character == 1 && (obj->attack & 0x10) &&
                (po->vulnerable & 0x10)) {
                goto vertical;
            }
            f1 = obj->mom.y;
        ycheck:
            if (!(f1 < 0.0f)) {
                continue;
            }
            if ((obj->attack & 0xC) == 0) {
                continue;
            }
            if ((po->vulnerable & 0xC) == 0) {
                continue;
            }
        vertical:
            if (obj->objtop < po->objbot) {
                continue;
            }
            if (po->objtop < obj->objbot) {
                continue;
            }
            if (obj->attack & 0x8) {
                reach = obj->radius * obj->SCALE + 0.5f;
            } else if (obj->attack & 0x4) {
                reach = obj->radius * obj->SCALE + 0.25f;
            } else {
                reach = obj->radius * obj->SCALE;
                if (obj->attack & 0x2) {
                    if (obj->character != 1) {
                        reach = reach + reach;
                    } else if (((struct creature_s *)obj->parent)->slide == 0) {
                        reach = reach + reach;
                    } else {
                        reach = reach * 3.0f;
                    }
                }
            }

            if (is_cuboid) {
                if (CylinderCuboidOverlapXZ(&obj->pos, po, &po->pos, reach) == 0) {
                    continue;
                }
                po->dead = (obj->attack & 0x2) ? 1 : 4;
                hitside = temp_cuboid_side;
            } else {
                f32 rr = reach + po->RADIUS;
                if (rr * rr < distsq) {
                    continue;
                }
                po->dead = (obj->attack & 0x2) ? 1 : 4;
                hitside = 8;
            }
            po->touch = (obj->attack & 0xC) ? 2 : hitside;
            if (po->dead == 0) {
                continue;
            }

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
                if (po->oldobjtop - D_0062D7C0 <= obj->oldobjbot) {
                    obj->mom.y = 0.0f;
                } else if (obj->oldobjtop <= po->oldobjbot + D_0062D7C0) {
                    obj->mom.y = 0.0f;
                } else {
                    obj->mom.x = obj->mom.x * D_0062D7C4;
                    obj->mom.z = obj->mom.z * D_0062D7C4;
                }
            }
            goto kill_tail_72C;
        }

        /* physical overlap */
        dir = 0;
        if (obj->objtop < po->objbot || po->objtop < obj->objbot) {
            dir = 1;
            goto topbot;
        }
        if (VEHICLECONTROL == 2 ||
            (VEHICLECONTROL == 1 && obj->vehicle == 0x20)) {
            if (obj->attack & (po->vulnerable & 0x82)) {
                po->dead = 0x15;
            } else if (maskflag == 0) {
                obj->dead = 1;
            } else if (mask_weak) {
                lose = 1;
                po->dead = 0x15;
            } else {
                obj->dead = 1;
            }
            NuVecScale(&obj->mom, &obj->mom, D_0062D7C8);
            goto kill_tail_724;
        }

        if (po->oldobjtop - D_0062D7CC <= obj->oldobjbot) {
            s32 a1 = 0;
            s32 a2 = 0;
            ctype = 2;
            if (obj->attack & (po->vulnerable & 0x8E)) {
                po->dead = (obj->attack & 0x8C) ? 4 : 1;
                if (obj->attack & 0x2) {
                    obj->mom.y = 0.0f;
                }
            } else if (obj->attack & (po->vulnerable & 0x20)) {
                po->dead = 4;
                a2 = 1;
            } else if ((obj->attack & 0x2) && (po->attack & 0x2)) {
                a2 = 1;
            } else {
                if (po->attack == 0) {
                    if ((po->flags & 0x100) == 0) {
                        a1 = 1;
                        goto push_up;
                    }
                }
                if (maskflag == 0) {
                    obj->dead = 1;
                } else if (mask_weak) {
                    lose = 1;
                    po->dead = 4;
                    goto push_up;
                } else {
                    obj->dead = 1;
                }
                a1 = 1;
                a2 = (po->attack & 0x2) ? 1 : 0;
            }
        push_up:
            if (a2 == 0) {
                a2 = (po->flags & 0x40) ? 1 : 0;
                if (a2 == 0) {
                    a2 = (po->flags & 0x80) ? 2 : 0;
                    if (a2 == 0 && a1 == 0) {
                        dir = 2;
                        goto push_up_done;
                    }
                }
            }
            obj->mom.y = 0.0f;
            obj->ground = 2;
            obj->pos.y = po->objtop - obj->bot * obj->SCALE;
            obj->shadow = po->objtop;
            obj->surface_type = 0xF;
            obj->got_shadow = 1;
            obj->vSN = v010;
            if (a2 != 0) {
                obj->boing = obj->boing | a2;
                GameSfx(2, &obj->pos);
                NewRumble(&player->rumble, 0x7F);
                NewBuzz(&player->rumble, 0xA);
                dir = 2;
            } else if (a1 != 0) {
                obj->pos.x = obj->pos.x + po->mom.x;
                obj->pos.z = obj->pos.z + po->mom.z;
                if (po->dyrot != 0) {
                    f32 mdx;
                    f32 mdz;
                    f32 dist;
                    s32 base;
                    s32 na;
                    obj->hdg = obj->hdg + po->dyrot;
                    mdx = obj->pos.x - po->pos.x;
                    mdz = obj->pos.z - po->pos.z;
                    base = NuAtan2D(mdx, mdz);
                    dist = NuFsqrt(mdx * mdx + mdz * mdz);
                    na = base + po->dyrot;
                    obj->pos.x = obj->pos.x +
                                 (NuTrigTable[(u16)na] * dist -
                                  NuTrigTable[(u16)base] * dist);
                    obj->pos.z = obj->pos.z +
                                 (NuTrigTable[(u16)(na + 0x4000)] * dist -
                                  NuTrigTable[(u16)(base + 0x4000)] * dist);
                }
                dir = 2;
            }
        push_up_done:
            po->touch = dir;
            goto kill_tail_724;
        }

        if (obj->oldobjtop <= po->oldobjbot + D_0062D7CC) {
            s32 a1 = 0;
            ctype = 4;
            if (obj->attack & (po->vulnerable & 0x8E)) {
                po->dead = (obj->attack & 0x8C) ? 4 : 1;
                if (obj->attack & 0x2) {
                    obj->mom.y = 0.0f;
                    if (obj->flags & 0x1) {
                        if (player->jump != 0) {
                            player->jump = 6;
                            player->jump_type = 4;
                            player->jump_frame = player->jump_frames;
                        }
                    }
                }
            } else if (obj->attack & (po->vulnerable & 0x40)) {
                po->dead = 4;
                a1 = 1;
            } else if ((obj->attack & 0x2) && (po->attack & 0x2)) {
                /* nothing */
            } else if (po->attack == 0) {
                a1 = 1;
            } else if (maskflag == 0) {
                obj->dead = 1;
                a1 = 1;
            } else if (mask_weak) {
                lose = 1;
                po->dead = 4;
            } else {
                obj->dead = 1;
                a1 = 1;
            }
            if (a1 != 0) {
                obj->pos.y = po->objbot - obj->top * obj->SCALE;
                obj->mom.y = 0.0f;
            }
            po->touch = 4;
            goto kill_tail_724;
        }

    topbot:
        if (dir == 0) {
            s32 a1 = 0;
            ctype = 1;
            if (obj->attack & (po->vulnerable & 0x9E)) {
                po->dead = (obj->attack & 0x80) ? 4 : 1;
            } else if ((obj->attack & 0x2) && (po->attack & 0x2)) {
                a1 = 1;
            } else if (po->attack == 0) {
                a1 = 1;
            } else if (maskflag == 0) {
                obj->dead = 1;
                a1 = 1;
            } else if (mask_weak) {
                lose = 1;
                po->dead = 4;
            } else {
                obj->dead = 1;
                a1 = 1;
            }
            if (a1 != 0) {
                if (po->character == 0x39) {
                    gamesfx_effect_volume = 0x9FFD;
                    GameSfx(0x49, &obj->pos);
                }
                if (is_cuboid) {
                    ang = (s32)temp_cuboid_bounce_angle;
                } else {
                    ang = NuAtan2D(obj->pos.x - po->pos.x,
                                   obj->pos.z - po->pos.z);
                }
                {
                    f32 bf = (po->attack & 0x2) ? D_0062D7D4 : D_0062D7D0;
                    obj->mom.x = NuTrigTable[(u16)ang] * bf;
                    obj->mom.z = NuTrigTable[(u16)(ang + 0x4000)] * bf;
                    temp_xzmomset = 1;
                }
            }
            po->touch = side;
            goto kill_tail_724;
        } else {
            f32 objmid = (obj->objtop + obj->objbot) * 0.5f;
            if (!(po->objtop < objmid)) {
                continue;
            }
            if (obj->got_shadow == 0) {
                obj->shadow = po->objtop;
            } else if (obj->shadow < po->objtop) {
                obj->shadow = po->objtop;
            } else {
                continue;
            }
            obj->got_shadow = 1;
            obj->surface_type = 0;
            obj->vSN = v010;
            continue;
        }

    kill_tail_724:
        po->contact = obj;
        po->contact_type = ctype;
    kill_tail_72C:
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
            NewBuzz(&player->rumble, 0x5);
            if ((po->flags & 0x40000) && !(obj->attack & 0x80)) {
                po->kill_contact = 1;
                po->dead = 0;
                continue;
            }
            if (po->dead == 1) {
                ang = NuAtan2D(po->pos.x - obj->pos.x, po->pos.z - obj->pos.z);
                po->mom.x = 0.0f;
                po->mom.y = 0.0f;
                po->mom.z = D_0062D7D8;
                NuVecRotateX(&po->mom, &po->mom, -0x400);
                NuVecRotateY(&po->mom, &po->mom, (u16)ang);
                GameSfx(0x37, &po->pos);
            }
            KillGameObject(po, po->dead);
            continue;
        }
        if (obj->dead == 0) {
            continue;
        }
        goto obj_death;
    }

    if (lose != 0) {
        if (obj->invincible == 0) {
            LoseMask(obj);
        }
        goto after_death;
    }
    if (obj->dead == 0) {
        goto after_death;
    }
obj_death:
    if (obj->invincible != 0) {
        obj->dead = 0;
    } else {
        if (((s8 *)po->parent)[0x21C] == -1) {
            killanim = GetDieAnim(obj, -1);
        } else {
            killanim = ((s8 *)po->parent)[0x21C];
        }
        KillGameObject(obj, killanim);
    }
after_death:
    if (VEHICLECONTROL == 1 && obj->vehicle == 0x53) {
        ObjectToAtlas(obj, player);
    }
}

s32 KillPlayer(struct obj_s *obj, s32 anim)
{
    struct mask_s *mask;

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
