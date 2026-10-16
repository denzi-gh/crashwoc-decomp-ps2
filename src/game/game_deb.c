/*
 * Unit: game/game_deb
 *
 * Functions:
 *   0x0025fb08 AddAnimDebris
 *   0x00260ae8 InitGameDebris
 *   0x00260b60 AddGameDebris
 *   0x00260bb8 AddGameDebrisRot
 *   0x00260c20 AddGameDebrisMtx
 *   0x00260cd8 AddWarpDebris
 *   0x00260d50 AddMechanicalDebris
 */

#include "numath/numtx.h"
#include "numath/nuvec.h"
#include "game/gametypes.h"
#include "gamelib/terrain.h"
#include "game/creature.h"

struct adeb_s {
    short character;                    /* 0x00 */
    short action;                       /* 0x02 */
    float time;                         /* 0x04 */
    float time2;                        /* 0x08 */
    u32 flags;                          /* 0x0C */
    u16 locators;                       /* 0x10 */
    u8 gdeb;                            /* 0x12 */
    signed char count;                  /* 0x13 */
    u8 random;                          /* 0x14 */
    u8 xrot;                            /* 0x15 */
    signed char type;                   /* 0x16 */
    signed char target;                 /* 0x17 */
    short sfx;                          /* 0x18 */
    u8 rumble;                          /* 0x1A */
    u8 buzz;                            /* 0x1B */
    char pad[4];                        /* 0x1C */
    u64 levbits;                        /* 0x20 */
    float min;                          /* 0x28 */
    float max;                          /* 0x2C */
}; /* 0x30 */

extern struct gdeb_s GDeb[170];
extern struct adeb_s ADeb[112];
extern s32 NODEBRIS;
extern s32 Level;
extern s32 Paused;
extern s32 GemPath;
extern s32 Death;
extern s32 Bonus;
extern s32 jonframe1;
extern s32 SFXCOUNT_ALL;
extern s32 gamesfx_effect_volume;
extern s32 gamesfx_pitch;
extern f32 EShadY;
extern struct nuvec_s ShadNorm;
extern struct nuvec_s v000;
extern struct cammtx_s GameCam[];
extern struct aitab_s AITab[];

extern s32 LookupDebrisEffect(char *name);
extern void AddFiniteShotDebrisEffect(s32 *key, s32 effect, struct nuvec_s *pos,
                                      s32 n);
extern void AddVariableShotDebrisEffect(s32 effect, struct nuvec_s *pos, s32 n,
                                        short xrot, short yrot);
extern void AddVariableShotDebrisEffectMtx2(s32 effect, struct nuvec_s *pos,
                                            s32 n, struct numtx_s *m1,
                                            struct numtx_s *m2);

extern void AddDeb3(struct nuvec_s *pos, s32 effect, s32 n, void *arg);
extern s32 qrand(void);
extern f32 frand(void);
extern void JudderGameCamera(struct cammtx_s *cam, f32 time, struct nuvec_s *pos);
extern void GameSfx(s32 id, struct nuvec_s *pos);
extern void AddKaboom(s32 type, struct nuvec_s *pos, f32 radius);
extern void AddProjectile(struct nuvec_s *pos, struct nuvec_s *target,
                          struct nuvec_s *mom, s32 type, u16 hdg,
                          struct obj_s *owner);
extern void NuRndrAddFootPrint(u16 hdg, f32 sizex, f32 sizez, s32 depth,
                               struct nuvec_s *pos, struct nuvec_s *norm,
                               s32 type, s32 colour);

extern struct numtx_s mTEMP;
extern struct numtx_s numtx_identity;

extern s32 jeepbits[];
extern s32 helibits[];
extern s32 mechbits[];
extern s32 atlasbits[];


inline void AddGameDebris(s32 i, struct nuvec_s *pos) {
    s32 key;

    if (NODEBRIS == 0 && (u32)i < 0xaa && GDeb[i].i != -1) {
        key = -1;
        AddFiniteShotDebrisEffect(&key, GDeb[i].i, pos, 1);
    }
}

inline void AddGameDebrisRot(s32 i, struct nuvec_s *pos, s32 n, u16 xrot,
                             u16 yrot) {
    if (NODEBRIS == 0 && (u32)i < 0xaa && GDeb[i].i != -1) {
        AddVariableShotDebrisEffect(GDeb[i].i, pos, n, xrot, yrot + 0x4000);
    }
}

inline void AddGameDebrisMtx(s32 i, struct nuvec_s *pos, s32 n,
                             struct numtx_s *m) {
    if (NODEBRIS == 0 && (u32)i < 0xaa && GDeb[i].i != -1) {
        NuMtxSetRotationX(&mTEMP, 0x4000);
        NuMtxMulR(&mTEMP, &mTEMP, m);
        AddVariableShotDebrisEffectMtx2(GDeb[i].i, pos, n, &mTEMP,
                                        &numtx_identity);
    }
}

void AddAnimDebris(struct CharacterModel *model, struct numtx_s *mtxLOCATOR,
                   s32 action, float time, struct obj_s *obj) {
    struct adeb_s *adeb;
    struct creature_s *c;
    struct nuvec_s *p;
    u16 xrot;
    u16 yrot;
    float y;
    float sizex;
    float sizez;
    s32 n;
    s32 i;
    s32 j;
    s32 count;
    s32 sfx;
    s32 inf;
    s32 gindex;
    s32 depth;
    s32 sound;
    s32 list[16];
    struct numtx_s *mtx[16];
    struct nuvec_s pos;
    struct nuvec_s v[16];

    if (Paused != 0) {
        return;
    }
    for (adeb = ADeb; adeb->character != -1; adeb++) {
        if (model->character != adeb->character || action != adeb->action
            || ((adeb->levbits >> Level) & 1) == 0) {
            continue;
        }
        if (obj != 0) {
            if ((adeb->flags & 0x10) != 0 && (obj->ground & 2) == 0) {
                continue;
            }
            if ((adeb->flags & 0x80) != 0 && obj->submerged != 0) {
                continue;
            }
        }
        if (!((adeb->flags & 4) != 0
              || ((adeb->flags & 8) != 0 && time >= adeb->time
                  && (adeb->time2 <= 1.0f
                      || (adeb->time2 > adeb->time && time <= adeb->time2)))
              || (time > adeb->time
                  && time - model->animlist[action]->speed * 0.59999996f
                         <= adeb->time))) {
            continue;
        }
        if (adeb->character == 0x9A) {
            pos = v000;
        }
        pos = v000;
        count = 0;
        for (j = 0; j < 16; j++) {
            if (((adeb->locators >> j) & 1) != 0 && model->pLOCATOR[j] != 0) {
                list[count] = j;
                v[count] = *(struct nuvec_s *)&mtxLOCATOR[j]._30;
                mtx[count] = &mtxLOCATOR[j];
                if (count == 0 || (adeb->flags & 0x4000) != 0) {
                    NuVecAdd(&pos, &pos, (struct nuvec_s *)&mtxLOCATOR[j]._30);
                }
                count++;
            }
        }
        if (count > 0) {
            NuVecScale(&pos, &pos, 1.0f / (float)count);
        }
        if ((adeb->flags & 0x8000) != 0 && obj != 0) {
            count = 1;
            pos.x = obj->pos.x;
            pos.y = obj->pos.y + obj->bot * obj->SCALE;
            pos.z = obj->pos.z;
        }
        if (count == 0 && obj != 0) {
            v[count] = obj->pos;
            pos = v[count];
            count = 1;
        }
        if ((adeb->flags & 0x10000) != 0) {
            JudderGameCamera(GameCam, adeb->max, 0);
        }
        if (count != 0 && (adeb->flags & 0x80) != 0) {
            if (obj == 0) {
                goto debris;
            }
            if (obj->layer_type != -1 && pos.y < obj->layer_shadow) {
                continue;
            }
        }
        if (obj == 0
            || !(((obj->flags & 4) != 0 && obj->dead == 1)
                 || ((adeb->flags & 0x100) != 0 && obj->xz_distance < adeb->min)
                 || ((adeb->flags & 0x200) != 0 && obj->xz_distance > adeb->max)
                 || ((adeb->flags & 0x40000) != 0
                     && obj->xyz_distance < adeb->min)
                 || ((adeb->flags & 0x80000) != 0
                     && obj->xyz_distance > adeb->max)
                 || ((obj->flags & 1) != 0
                     && (GemPath == 1 || GemPath == 3 || Death == 1
                         || Death == 3 || Bonus == 1 || Bonus == 3)))) {
debris:
            if (count != 0 && adeb->gdeb < 0xAA) {
                if ((adeb->flags & 1) != 0) {
                    n = adeb->count;
                    if (n < 0) {
                        n = (jonframe1 % -n) == 0;
                    }
                    if (n != 0
                        && (adeb->random == 0
                            || qrand() <= adeb->random << 8)) {
                        if ((adeb->flags & 0x8000) != 0) {
                            AddGameDebrisRot(adeb->gdeb, &pos, n, 0, 0);
                        } else if ((adeb->flags & 0x400) != 0) {
                            if ((adeb->flags & 0x4000) != 0) {
                                AddGameDebrisMtx(adeb->gdeb, &pos, n, mtx[0]);
                            } else {
                                for (i = 0; i < count; i++) {
                                    AddGameDebrisMtx(adeb->gdeb, &v[i], n,
                                                     mtx[i]);
                                }
                            }
                        } else {
                            if ((adeb->flags & 2) != 0) {
                                xrot = adeb->xrot << 8;
                                if (obj == 0) {
                                    yrot = 0;
                                } else {
                                    yrot = obj->hdg;
                                }
                            } else {
                                xrot = 0;
                                yrot = 0;
                            }
                            if ((adeb->flags & 0x4000) != 0) {
                                AddGameDebrisRot(adeb->gdeb, &pos, n, xrot,
                                                 yrot);
                            } else {
                                for (i = 0; i < count; i++) {
                                    AddGameDebrisRot(adeb->gdeb, &v[i], n,
                                                     xrot, yrot);
                                }
                            }
                        }
                    }
                } else if ((adeb->flags & 0x8000) != 0) {
                    AddGameDebris(adeb->gdeb, &pos);
                } else if ((adeb->flags & 0x4000) != 0) {
                    AddGameDebris(adeb->gdeb, &pos);
                } else {
                    for (i = 0; i < count; i++) {
                        AddGameDebris(adeb->gdeb, &v[i]);
                    }
                }
            }
            if (adeb->rumble != 0) {
                NewRumble(&player->rumble, adeb->rumble);
            }
            if (adeb->buzz != 0) {
                NewBuzz(&player->rumble, adeb->buzz);
            }
        }
        if (((adeb->sfx >= 0 && adeb->sfx < SFXCOUNT_ALL)
             || adeb->sfx == 0x400 || adeb->sfx == 0x401)
            && (adeb->random == 0 || qrand() <= adeb->random << 8)) {
            sfx = adeb->sfx;
            if (sfx == 0x400) {
                sfx = 0x8D;
                if (obj != 0) {
                    if (obj->layer_type == 1) {
                        sfx = 0x2D;
                    } else {
                        switch (obj->surface_type) {
                        case 1:
                        case 5:
                        case 6:
                        case 13:
                        case 14:
                        case 15:
                            sfx = 0x1F;
                            break;
                        }
                    }
                }
            } else if (sfx == 0x401) {
                sfx = 0x8E;
                if (obj != 0) {
                    if (obj->layer_type == 1) {
                        sfx = 0x2E;
                    } else {
                        switch (obj->surface_type) {
                        case 1:
                        case 5:
                        case 6:
                        case 13:
                        case 14:
                        case 15:
                            sfx = 0x20;
                            break;
                        }
                    }
                }
            }
            sound = 1;
            if ((adeb->flags & 0x400000) != 0 && frand() > adeb->max) {
                sound = 0;
            }
            if (sound) {
                if ((adeb->flags & 0x100000) != 0) {
                    gamesfx_effect_volume = (s32)adeb->min;
                }
                if ((adeb->flags & 0x200000) != 0) {
                    gamesfx_pitch = (s32)adeb->max;
                }
                GameSfx(sfx, (count == 0 || (adeb->flags & 0x2000) == 0)
                                 ? 0 : &pos);
            }
        }
        if (count != 0 && (adeb->flags & 0x800) != 0 && adeb->type != -1
            && (obj == 0 || obj->dead == 0)
            && (adeb->random == 0 || qrand() <= adeb->random << 8)) {
            n = adeb->count;
            if (n < 0) {
                n = (jonframe1 % -n) == 0;
            }
            if (n != 0) {
                c = 0;
                if (obj != 0) {
                    c = obj->parent;
                }
                AddProjectile(&pos,
                              (adeb->target != -1 && c != 0
                               && c->i_aitab != -1)
                                  ? &AITab[c->i_aitab].pos[adeb->target] : 0,
                              0, adeb->type,
                              (obj != 0) ? obj->hdg : (u16)qrand(), obj);
            }
        }
        if ((adeb->flags & 0x1000) != 0 && (obj == 0 || obj->dead == 0)) {
            p = 0;
            if (adeb->target != -1) {
                if (obj != 0) {
                    p = &AITab[((struct creature_s *)obj->parent)->i_aitab]
                             .pos[adeb->target];
                }
            } else if (count != 0) {
                p = &pos;
            } else if (obj != 0) {
                p = &obj->pos;
            }
            if (p != 0) {
                AddKaboom(0x10, p, !(adeb->max > 0.0f) ? 1.0f : adeb->max);
            }
        }
        if (obj == 0 || (adeb->flags & 0x60) == 0 || (obj->ground & 2) == 0) {
            continue;
        }
        y = NewShadowMask(&pos, 0.0f, -1);
        if (y == 2000000.0f) {
            continue;
        }
        inf = ShadowInfo();
        gindex = -1;
        if (EShadY != 2000000.0f && EShadowInfo() == 4) {
            y = EShadY;
            depth = 0x78;
            obj->ddsnow = depth;
            gindex = 4;
        }
        switch (inf) {
        case 2:
            if (gindex == -1) {
                depth = 0x78;
                obj->ddr = 0x60;
                obj->ddg = 0x70;
                obj->ddb = -0x40;
                gindex = 4;
                obj->ddsnow = depth;
            }
            break;
        case 8:
            depth = 0x78;
            obj->ddr = -0x80;
            obj->ddg = 0x50;
            obj->ddb = 0x20;
            gindex = 4;
            obj->ddsand = depth;
            break;
        default:
            if (obj->ddsnow > 0 && gindex == -1) {
                obj->ddr = 0x60;
                depth = obj->ddsnow;
                obj->ddg = 0x70;
                obj->ddb = -0x40;
                gindex = 6;
                obj->ddsnow -= 2;
            }
            if (obj->ddsand > 0 && gindex == -1) {
                obj->ddr = -0x80;
                depth = obj->ddsand;
                obj->ddg = 0x50;
                gindex = 0;
                obj->ddb = 0x20;
                obj->ddsand -= 2;
            }
            if (obj->ddwater > 0) {
                if (gindex != -1) {
                    break;
                }
                if (obj->ddwater < 0x6E) {
                    depth = obj->ddwater;
                    obj->ddr = 0x30;
                    obj->ddg = 0x30;
                    obj->ddb = 0x30;
                    gindex = 2;
                }
                obj->ddwater -= 2;
            }
            break;
        }
        if (gindex == -1) {
            continue;
        }
        if (model->character == 1) {
            sizex = 0.07f;
            sizez = 0.12f;
        } else {
            sizex = 0.1f;
            sizez = 0.2f;
        }
        pos.y = y + 0.02f;
        if ((adeb->flags & 0x20) != 0) {
            if ((short)obj->hdg >= 0) {
                NuRndrAddFootPrint(obj->hdg, sizex, sizez, depth, &pos,
                                   &ShadNorm, gindex + 1,
                                   obj->ddb * 0x10000 + obj->ddg * 0x100
                                       + obj->ddr);
            } else {
                NuRndrAddFootPrint(obj->hdg, -sizex, sizez, depth, &pos,
                                   &ShadNorm, gindex,
                                   obj->ddb * 0x10000 + obj->ddg * 0x100
                                       + obj->ddr);
            }
        } else {
            if ((short)obj->hdg >= 0) {
                NuRndrAddFootPrint(obj->hdg, sizex, sizez, 0x30, &pos,
                                   &ShadNorm, gindex,
                                   obj->ddb * 0x10000 + obj->ddg * 0x100
                                       + obj->ddr);
            } else {
                NuRndrAddFootPrint(obj->hdg, -sizex, sizez, 0x30, &pos,
                                   &ShadNorm, gindex + 1,
                                   obj->ddb * 0x10000 + obj->ddg * 0x100
                                       + obj->ddr);
            }
        }
    }
}


void InitGameDebris(void) {
    s32 i;

    for (i = 0; i < 170; i++) {
        GDeb[i].i = -1;
        if (((GDeb[i].levbits >> Level) & 1) != 0) {
            GDeb[i].i = LookupDebrisEffect(GDeb[i].name);
        }
    }
}

void AddWarpDebris(struct obj_s *obj, struct nuvec_s *pos) {
    struct nuvec_s v;
    s32 key;

    key = -1;
    v.x = obj->pos.x;
    v.y = obj->pos.y + (obj->bot + obj->top) * obj->SCALE * 0.5f;
    v.z = obj->pos.z;
    AddFiniteShotDebrisEffect(&key, GDeb[77].i, &v, 1);
}

/*
 * Faithful near-match (state=asm): retail inlines AddGameDebris(0x8E,pos) at
 * case 0x20 -- reproduced by the `inline` on AddGameDebris above (gnu89: emits
 * standalone AND inlines), so the frame (0x60) and reg_mask (0x800f0000) match.
 * Residual is a register-allocation difference: retail keeps %hi(jeepbits) in
 * s1 and rematerialises &jeepbits after the inlined body. 288-byte extent
 * exact so the hybrid splice is unaffected.
 */
void AddMechanicalDebris(struct nuvec_s *pos, s32 vehicle) {
    s32 i;
    s32 *pt;

    pt = jeepbits;
    i = 1;
    switch (vehicle) {
    case 0x44:
    case 0xB2:
        i = 1;
        pt = mechbits;
        break;
    case 0x53:
        pt = atlasbits;
        break;
    case 0x3B:
        pt = helibits;
        break;
    case 0x20:
        AddGameDebris(0x8E, pos);
        i = 6;
        break;
    }
    while (*pt != -1) {
        AddDeb3(pos, pt[0], i, 0);
        pt++;
    }
}
