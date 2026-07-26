/*
 * Unit: game/deb3
 *
 * Functions:
 *   0x00247aa0 PlayRandSFX
 *   0x00247bc8 RBodyInitClasses
 *   0x00247e18 RBodyMove
 *   0x00248118 RBodyImpact
 *   0x00248310 InitDeb3
 *   0x002484d8 AddDeb3Ang
 *   0x00248ae0 AddDeb3
 *   0x00249108 JonExtraDraw
 *   0x002495a8 LaunchObjects
 *   0x0024bcc0 ProcDeb3
 *   0x0024c5b0 DrawDeb3
 *   0x0024c778 CoalSpark
 *   0x0024c7e0 CoalBreak
 *   0x0024c830 RockSpark
 *   0x0024c8a8 RockBreak
 *   0x0024c908 DynaBreak
 *   0x0024c950 InitRandSFX
 *   0x0024c960 JonMaskFPS
 */

#include "creature.h"

struct deb3_s;


struct deb3info {
    s16 type;                          /* 0x00 */
    s16 classid;                       /* 0x02 */
    u16 info;                          /* 0x04 */
    f32 timer;                         /* 0x08 */
    f32 size;                          /* 0x0C */
    s16 deb;                           /* 0x10 */
    s16 rate;                          /* 0x12 */
    void (*impact)(struct deb3_s *);   /* 0x14 */
    void (*end)(struct deb3_s *);      /* 0x18 */
    s32 data;                          /* 0x1C */
}; /* 0x20 */


struct deb3_s {
    struct numtx_s mtx;                /* 0x00 */
    struct numtx_s invWorldInertiaTensor; /* 0x40 */
    struct nuvec_s velocity;           /* 0x80 */
    struct nuvec_s angularvelocity;    /* 0x8C */
    struct nuvec_s angularMomentum;    /* 0x98 */
    struct nuvec_s impact;             /* 0xA4 */
    struct nuvec_s norm;               /* 0xB0 */
    struct nuvec_s diff;               /* 0xBC */
    f32 shadow;                        /* 0xC8 */
    f32 grav;                          /* 0xCC */
    u16 status;                        /* 0xD0 */
    s16 timer;                         /* 0xD2 */
    s16 check;                         /* 0xD4 */
    s16 count;                         /* 0xD6 */
    struct deb3info *info;             /* 0xD8 */
    s32 data;                          /* 0xDC */
}; /* 0xE0 */

/* Rigid-body class table, stride 0x4C (mass 0x40, kr 0x44, kf 0x48). */
struct rbclass_s {
    struct numtx_s invBodyInertiaTensor; /* 0x00 */
    f32 mass;                          /* 0x40 */
    f32 kr;                            /* 0x44 */
    f32 kf;                            /* 0x48 */
}; /* 0x4C */

extern struct deb3_s deb3[64];
extern struct rbclass_s rbclass[5];
extern struct nuvec_s ShadNorm;

extern f32 D_0062E774;   /* rigid-body integration timestep */
extern f32 D_0062E778;   /* minimum ShadNorm.y for debris to come to rest */

extern void NuMtxSetIdentity(struct numtx_s *m);
extern void RBodyMove(struct deb3_s *deb, f32 dt);
extern void RBodyImpact(struct deb3_s *deb, struct nuvec_s *pos,
                        struct nuvec_s *norm);
extern void CubeImpact(struct numtx_s *mat, struct numtx_s *nmat,
                       struct nuvec_s *norm, f32 size,
                       struct nuvec_s *impact);
extern void FullReflect(struct nuvec_s *n, struct nuvec_s *l,
                        struct nuvec_s *r);
extern s32 NewRayCast(struct nuvec_s *pos, struct nuvec_s *dir, f32 size);

struct gdeb_s {
    s32 i;                /* 0x0 */
    char *name;           /* 0x4 */
    u64 levbits;          /* 0x8 */
};

extern struct gdeb_s GDeb[170];
extern s32 rsfxcount;
extern s32 *rsfxpt;

extern void AddGameDebris(s32 id, struct nuvec_s *pos);
extern void GameSfx(s32 id, struct nuvec_s *pos);
extern void AddFiniteShotDebrisEffect(s32 *key, s32 effect, struct nuvec_s *pos,
                                      s32 n);
extern s32 qrand(void);

struct camera_s {
    struct numtx_s mtx;   /* 0x00 */
};
extern struct camera_s global_camera;

/* ------------------------------------------------------------------ */
/* LaunchObjects support                                               */
/* ------------------------------------------------------------------ */

/* LevelData: fog/haze block verified against LaunchObjects (fognear 0x44,
 * fogfar 0x48, fogr..foga 0x4C..0x4F, hazer..hazea 0x50..0x53). */
struct ldata_s {
    u8 unk_0x00[0x44];    /* 0x00 */
    f32 fognear;          /* 0x44 */
    f32 fogfar;           /* 0x48 */
    u8 fogr;              /* 0x4C */
    u8 fogg;              /* 0x4D */
    u8 fogb;              /* 0x4E */
    u8 foga;              /* 0x4F */
    u8 hazer;             /* 0x50 */
    u8 hazeg;             /* 0x51 */
    u8 hazeb;             /* 0x52 */
    u8 hazea;             /* 0x53 */
    s32 totalsize;        /* 0x54 */
}; /* 0x58 */

struct nunode_s {
    u8 unk_0x00[0x30];        /* 0x00 */
    struct nuvec_s pos;       /* 0x30 */
};

struct nuspecial_s {
    u8 unk_0x00[0x40];        /* 0x00 */
    struct nunode_s *node;    /* 0x40 */
};

struct nugspline_s {
    s16 len;                  /* 0x0 */
    s16 ptsize;               /* 0x2 */
    char *name;               /* 0x4 */
    char *pts;                /* 0x8 */
}; /* 0xC */

struct nugscn_s {
    u8 unk_0x00[0x30];             /* 0x00 */
    struct nugspline_s *splines;   /* 0x30 */
};

struct objtab_s {
    struct nuhspecial_s obj;   /* 0x00 */
    struct nugscn_s **scene;   /* 0x08 */
    char visible;              /* 0x0C */
    char font3d_letter;        /* 0x0D */
    char pad1;                 /* 0x0E */
    char pad2;                 /* 0x0F */
    char *name;                /* 0x10 */
    char unk[4];               /* 0x14 */
    u64 levbits;               /* 0x18 */
}; /* 0x20 */

struct firedrop_s {
    struct nuvec_s pos;   /* 0x00 */
    f32 time;             /* 0x0C */
    s32 type;             /* 0x10 */
}; /* 0x14 */

struct chase_s {
    u8 unk_0x00[0x24];        /* 0x0000 */
    struct nuvec_s pos;       /* 0x0024 */
    u8 unk_0x30[0x7510];      /* 0x0030 */
    signed char mode;         /* 0x7540 */
};

struct pad_s {
    u8 unk_0x000[0x55C];  /* 0x000 */
    u32 paddata;          /* 0x55C */
};

extern struct ldata_s *LDATA;
extern struct objtab_s ObjTab[201];
extern struct chase_s Chase;
extern struct pad_s *Pad[];
extern struct nugscn_s *world_scene[32];
extern struct nugspline_s *pVIS;
extern s32 iVIS;
extern char *HutList[];
extern s32 cortlights[42];
extern u32 cortcols[];
extern struct firedrop_s firedrop[16];
extern f32 HotRocks[73];
extern s32 rsfxJung[];

extern f32 NuTrigTable[];
extern struct nuvec_s D_00632868;   /* Level 0x13 tornado reference point */

extern s32 Level;
extern s32 VEHICLECONTROL;
extern s32 FRAME;
extern s32 FRAMES;
extern s32 Paused;
extern s32 SKELETALCRASH;
extern s32 in_finish_range;

extern s32 jonframe1;
extern s32 jcrunch;
extern s32 water1;
extern s32 water2;
extern s32 cmask;
extern s32 dmask;
extern s32 maskoff;
extern s32 maskx;
extern s32 masky;
extern s32 maskrot;
extern s32 flooron;
extern s32 roofon;
extern f32 floor1;
extern f32 roof1;
extern f32 ShadRoofY;
extern s32 xrayon;
extern s32 subprop;
extern f32 torndist;
extern f32 *rockpt;
extern s32 hutexplode;
extern s32 hutframe;
extern s32 exkey;
extern s32 exroty;
extern s32 dropfire;
extern s32 firedroppt;

extern void AddVariableShotDebrisEffect(s32 type, struct nuvec_s *pos,
                                        s32 count, s32 a, s32 b);
extern struct deb3_s *AddDeb3Ang(struct nuvec_s *pos, s32 db, s32 emit,
                                 void *angle, s32 rotx, s32 roty);
extern s32 NuAtan2D(f32 x, f32 z);
extern f32 NuFsqrt(f32 x);
extern f32 NewShadow(struct nuvec_s *pos, f32 y);
extern void NuLightAddSpot(struct nuvec_s *pos, u32 colour, s32 mode, f32 size);
extern void NuBridgeOn(s32 on);
extern void FireBossWaterFire(s32 on);
extern s32 NuSpecialFind(struct nugscn_s *scene, struct nuhspecial_s *sp,
                         char *name);
extern void StartHGobjAnim(struct nuhspecial_s *sp);
extern void DebrisEmitterOrientation(s32 key, s32 a, s32 hdg);
extern void NewRumble(struct rumble_s *rumble, s32 power);
extern void NewBuzz(struct rumble_s *rumble, s32 frames);
extern s32 NuCameraClipTestPoints(struct nuvec_s *pos, s32 n, s32 mode);
extern void PlayRandSFX(void);

#define JONMASK(v, a) JonMaskFPS((v), (a))


void PlayRandSFX(void) {
    struct nuvec_s vec;
    s32 iVar1;

    if (rsfxpt != 0) {
        rsfxcount -= 0x3c;
        if (rsfxcount < 1) {
            vec.x = global_camera.mtx._30 +
                    (s32)(qrand() - 0x8000U) * 0.00024414062f;
            vec.y = global_camera.mtx._31 +
                    (s32)(qrand() - 0x8000U) * 0.00024414062f;
            vec.z = global_camera.mtx._32 +
                    (s32)(qrand() - 0x8000U) * 0.00024414062f;
            iVar1 = qrand() * *rsfxpt;
            if (iVar1 < 0) {
                iVar1 += 0xffff;
            }
            rsfxcount = ((iVar1 >> 0x10) + rsfxpt[1]) * 0x32;
            iVar1 = qrand() * rsfxpt[2];
            if (iVar1 < 0) {
                iVar1 += 0xffff;
            }
            GameSfx(rsfxpt[(iVar1 >> 0x10) + 3], &vec);
        }
    }
}


void InitRandSFX(void) {
    rsfxcount = 0;
    rsfxpt = 0;
}

inline s32 JonMaskFPS(s32 val, s32 add) {
    return add * 0x3c + val <= 0x320000 ? val + add * 0x3c
                                        : (add * 0x3c + val) - 0x320000;
}

void DynaBreak(struct deb3_s *deb) {
    struct nuvec_s pos;

    pos.x = deb->mtx._30;
    pos.y = deb->mtx._31;
    pos.z = deb->mtx._32;
    AddGameDebris(0x44, &pos);
    GameSfx(0x3b, &pos);
}

void RockBreak(struct deb3_s *deb) {
    s32 key;
    struct nuvec_s vec;

    vec.x = deb->mtx._30;
    vec.y = deb->mtx._31;
    vec.z = deb->mtx._32;
    key = -1;
    AddFiniteShotDebrisEffect(&key, GDeb[33].i, &vec, 1);
    GameSfx(0x5f, &vec);
}

void CoalBreak(struct deb3_s *deb) {
    s32 key;
    struct nuvec_s vec;

    vec.x = deb->mtx._30;
    vec.y = deb->mtx._31;
    vec.z = deb->mtx._32;
    key = -1;
    AddFiniteShotDebrisEffect(&key, GDeb[36].i, &vec, 1);
}

void RockSpark(struct deb3_s *deb) {
    s32 key;
    struct nuvec_s vec;

    vec.x = deb->mtx._30;
    vec.y = deb->mtx._31;
    vec.z = deb->mtx._32;
    key = -1;
    deb->data--;
    if (deb->data < 0) {
        deb->timer = 1;
    } else {
        AddFiniteShotDebrisEffect(&key, GDeb[33].i, &vec, 1);
        GameSfx(0x5f, &vec);
    }
}

void CoalSpark(struct deb3_s *deb) {
    s32 key;
    struct nuvec_s vec;

    vec.x = deb->mtx._30;
    vec.y = deb->mtx._31;
    vec.z = deb->mtx._32;
    key = -1;
    deb->data--;
    if (deb->data < 0) {
        deb->timer = 1;
    } else {
        AddFiniteShotDebrisEffect(&key, GDeb[35].i, &vec, 1);
    }
}


void LaunchObjects(void) {
    struct nuvec_s vec;
    struct nuvec_s vec2;
    struct nuhspecial_s obj;
    s32 key;
    f32 *pt;
    f32 temp;
    f32 dx;
    f32 dy;
    f32 dz;
    s32 loop;
    s32 count;
    s32 r;
    s32 g;
    s32 b;
    s32 hazea;
    s32 ang;

    jonframe1++;

    g = 0x80;
    r = 0x80;
    b = 0x80;
    hazea = 0;
    if (Level == 0x19) {
        if (jcrunch == 1) {
            b = 1;
            g = 0x40;
        }
        if (jcrunch == 2) {
            r = 0x40;
            g = 0x60;
            b = 0x7E;
        }
        if (jcrunch == 3) {
            r = 0x80;
            g = 0x40;
            b = 3;
            hazea = 1;
        }
        if (jcrunch == 4) {
            g = 0;
            b = 0x40;
            hazea = 0;
            r = (s32)(NuTrigTable[(u16)(water1 / 50)] * 64.0f) + 0x40;
        }
        if (jcrunch == 0) {
            if (water2 < 0) {
                water2 = -water2;
            }
            water2 -= 480;
            if (water2 < 0) {
                water2 = 0;
            }
            if (LDATA->hazea != 0) {
                LDATA->hazea -= 9;
                if ((s8)LDATA->hazea < 0) {
                    LDATA->hazea = 0;
                }
            }
        } else if (water2 >= 0) {
            if (LDATA->hazea != 0) {
                LDATA->hazea -= 9;
                if ((s8)LDATA->hazea < 0) {
                    LDATA->hazea = 0;
                }
            }
            water2 -= 480;
            if (water2 < 0) {
                LDATA->fogr = r;
                LDATA->fogg = g;
                LDATA->fogb = b;
                LDATA->hazea = hazea;
            }
        } else {
            vec.x = 0.0f;
            vec.y = 2.13f;
            vec.z = 4.69f;
            switch (LDATA->fogb & 3) {
            case 1:
                AddVariableShotDebrisEffect(GDeb[122].i, &vec, 1, 0, 0);
                break;
            case 2:
                AddVariableShotDebrisEffect(GDeb[123].i, &vec, 1, 0, 0);
                break;
            case 3:
                vec.y = 3.43f;
                AddVariableShotDebrisEffect(GDeb[125].i, &vec, 2, -0x8000, 0);
                break;
            case 0:
                AddVariableShotDebrisEffect(GDeb[124].i, &vec, 1, 0, 0);
                break;
            }
            water2 -= 480;
            if (water2 < -0x1900) {
                water2 = -0x1900;
            }
            if (LDATA->hazea != 0) {
                LDATA->hazea += 9;
                if ((s8)LDATA->hazea < 0) {
                    LDATA->hazea = 0x7F;
                }
            }
            if (LDATA->fogb == 0x40) {
                water1 = JONMASK(water1, 800);
                LDATA->fogr = (s32)(NuTrigTable[(u16)(water1 / 50)] * 64.0f)
                              + 0x40;
            }
        }
        count = water2 / 100;
        if (count < 0) {
            count = -count;
        }
        LDATA->foga = count;
        dmask = jcrunch;
        if (cmask == jcrunch && cmask != 0) {
            maskoff += 0x5A00;
            if (maskoff > 0xC8000) {
                maskoff = 0xC8000;
            }
        } else {
            maskoff -= 0x5A00;
            if (maskoff < 0) {
                maskoff = 0;
                cmask = dmask;
            }
        }
        maskx = JONMASK(maskx, 450);
        masky = JONMASK(masky, 800);
        maskrot = JONMASK(maskrot, 600);
    }

    if (Level == 0x21) {
        if ((jonframe1 & 1) == 0) {
            vec.x = global_camera.mtx._30;
            vec.y = global_camera.mtx._31;
            vec.z = global_camera.mtx._32;
            vec2.x = player->obj.pos.x - vec.x;
            vec2.z = player->obj.pos.z - vec.z;
            ang = NuAtan2D(vec2.x, vec2.z);
            vec.z = vec.z + NuTrigTable[(u16)(ang + 0x4000)] * 5.0f;
            vec.x = vec.x + NuTrigTable[(u16)ang] * 5.0f;
            AddVariableShotDebrisEffect(GDeb[115].i, &vec, 1, 0, 0);
        }
        for (loop = 0; loop < 7; loop++) {
            vec.x = global_camera.mtx._30;
            vec.y = global_camera.mtx._31;
            vec.z = global_camera.mtx._32;
            vec2.x = player->obj.pos.x - vec.x;
            vec2.z = player->obj.pos.z - vec.z;
            ang = NuAtan2D(vec2.x, vec2.z);
            vec.x = vec.x + NuTrigTable[(u16)ang] * 10.0f;
            vec.z = vec.z + NuTrigTable[(u16)(ang + 0x4000)] * 10.0f;
            dx = NuTrigTable[(u16)cortlights[loop * 6]];
            dy = NuTrigTable[(u16)(cortlights[loop * 6 + 1] + 0x4000)];
            dz = NuTrigTable[(u16)(cortlights[loop * 6 + 2] + 0x4000)];
            vec.y = vec.y + dy * 3.5f;
            vec.x = vec.x + dx * 3.5f;
            vec.z = vec.z + dz * 3.5f;
            vec2 = player->obj.pos;
            vec2.x = vec2.x + dx;
            vec2.y = vec2.y + dy;
            vec2.z = vec2.z + dz;
            if (FRAME == FRAMES - 1) {
                NuLightAddSpot(&vec, cortcols[loop], 2, 3.0f);
            }
            cortlights[loop * 6] =
                (cortlights[loop * 6] + cortlights[loop * 6 + 3]) & 0xFFFF;
            cortlights[loop * 6 + 1] =
                (cortlights[loop * 6 + 1] + cortlights[loop * 6 + 4]) & 0xFFFF;
            cortlights[loop * 6 + 2] =
                (cortlights[loop * 6 + 2] + cortlights[loop * 6 + 5]) & 0xFFFF;
        }
    }

    if (Level == 0x1F && (jonframe1 & 1) == 0) {
        vec.x = global_camera.mtx._30;
        vec.y = global_camera.mtx._31;
        vec.z = global_camera.mtx._32;
        vec2.x = player->obj.pos.x - vec.x;
        vec2.z = player->obj.pos.z - vec.z;
        ang = NuAtan2D(vec2.x, vec2.z);
        vec.z = vec.z + NuTrigTable[(u16)(ang + 0x4000)] * 5.0f;
        vec.x = vec.x + NuTrigTable[(u16)ang] * 5.0f;
        AddVariableShotDebrisEffect(GDeb[115].i, &vec, 1, 0, 0);
    }

    if (Level == 0x14) {
        if ((jonframe1 & 1) == 0) {
            vec.x = global_camera.mtx._30;
            vec.y = global_camera.mtx._31;
            vec.z = global_camera.mtx._32;
            vec2.x = player->obj.pos.x - vec.x;
            vec2.z = player->obj.pos.z - vec.z;
            ang = NuAtan2D(vec2.x, vec2.z);
            vec.z = vec.z + NuTrigTable[(u16)(ang + 0x4000)] * 5.0f;
            vec.x = vec.x + NuTrigTable[(u16)ang] * 5.0f;
            AddVariableShotDebrisEffect(GDeb[115].i, &vec, 1, 0, 0);
        }
        if (flooron != 0 || roofon != 0) {
            for (loop = 3; loop >= 0; loop--) {
                vec.x = player->obj.pos.x
                        + (f32)(qrand() - 0x8000) * 0.000244140625f;
                vec.y = player->obj.pos.y + 0.5f;
                vec.z = player->obj.pos.z
                        + (f32)(qrand() - 0x8000) * 0.000244140625f;
                if (vec.z > 58.0f && vec.z < 66.0f) {
                    vec.y = NewShadow(&vec, 0.0f);
                    if (flooron != 0) {
                        if (vec.y != 2000000.0f && vec.x > 18.0f
                            && vec.x < 50.0f && floor1 - 0.2f < vec.y) {
                            key = -1;
                            vec.y = vec.y - 0.1f;
                            AddFiniteShotDebrisEffect(&key, GDeb[109].i, &vec,
                                                      1);
                        }
                    }
                    if (roofon != 0) {
                        vec.y = ShadRoofY;
                        if (vec.y != 2000000.0f && vec.y < roof1 + 0.2f
                            && vec.x > 15.0f && vec.x < 55.0f) {
                            key = -1;
                            vec.y = vec.y + 0.1f;
                            AddFiniteShotDebrisEffect(&key, GDeb[109].i, &vec,
                                                      1);
                        }
                    }
                }
            }
        }
        if (roofon != 0) {
            roofon -= 60;
            if (roofon < 0) {
                roofon = 0;
            }
            if (roofon < 0xDAC) {
                if (ObjTab[150].obj.special != 0) {
                    ObjTab[150].obj.special->node->pos.y = floor1 - 100.0f;
                }
                if (ObjTab[151].obj.special != 0) {
                    ObjTab[151].obj.special->node->pos.y = roof1;
                }
            }
        } else if (flooron != 0) {
            flooron -= 60;
            if (flooron < 0) {
                flooron = 0;
            }
            if (flooron < 0xDAC) {
                if (ObjTab[150].obj.special != 0) {
                    ObjTab[150].obj.special->node->pos.y = floor1;
                }
                if (ObjTab[151].obj.special != 0) {
                    ObjTab[151].obj.special->node->pos.y = roof1 - 100.0f;
                }
            }
        } else {
            if (ObjTab[150].obj.special != 0) {
                ObjTab[150].obj.special->node->pos.y = floor1 - 100.0f;
            }
            if (ObjTab[151].obj.special != 0) {
                ObjTab[151].obj.special->node->pos.y = roof1 - 100.0f;
            }
            water1 -= 60;
            if (water1 < 0) {
                water1 = ((qrand() & 0x7F) + 30) * 50;
                water2 = 1 - water2;
                if (water2 != 0) {
                    flooron = 6000;
                } else {
                    roofon = 6000;
                }
            }
        }
    }

    if (Level == 0x16 && VEHICLECONTROL == 1
        && player->obj.anim.newaction == 0x68) {
        FireBossWaterFire(Pad[0]->paddata & 0x80);
        vec.x = player->mtxLOCATOR[1][0]._30;
        vec.y = player->mtxLOCATOR[1][0]._31;
        vec.z = player->mtxLOCATOR[1][0]._32;
        if ((Pad[0]->paddata & 0x80) != 0) {
            AddVariableShotDebrisEffect(GDeb[108].i, &vec, 4, 0x3D54,
                                        (s16)(player->obj.hdg + 0x4000));
        }
    }

    if (Level == 0xE && Chase.mode == 2) {
        vec.x = Chase.pos.x;
        vec.y = Chase.pos.y + 0.2f;
        vec.z = Chase.pos.z - 2.5f;
        AddVariableShotDebrisEffect(GDeb[100].i, &vec, 2, 0, 0);
        vec.y = Chase.pos.y + 0.6f;
        AddVariableShotDebrisEffect(GDeb[101].i, &vec, 1, 0, 0);
        vec.y = Chase.pos.y + 3.0f;
        AddVariableShotDebrisEffect(GDeb[103].i, &vec, 2, -0x218, 0);
        vec.y = Chase.pos.y + 3.6f;
        AddVariableShotDebrisEffect(GDeb[102].i, &vec, 4, -0x600, 0);
    }

    if (Level == 0x13) {
        dz = global_camera.mtx._32 - D_00632868.x;
        dx = global_camera.mtx._30 - D_00632868.x;
        temp = 50.0f - NuFsqrt(dx * dx + dz * dz) * 0.125f;
        if (temp < 0.0f) {
            temp = 0.0f;
        }
        LDATA->fogg = (s32)temp;
    }

    if (Level == 0x22 || Level == 2 || Level == 6) {
        LDATA->fogg = (s32)((f32)(s32)(NuTrigTable[(u16)(water1 / 50)] * 30.0f)
                            + 40.0f);
        LDATA->fogb = (s32)((f32)(s32)(NuTrigTable[(u16)(water2 / 50)] * 30.0f)
                            + 50.0f);
        water1 = JONMASK(water1, 110);
        water2 = JONMASK(water2, 105);
        if ((jonframe1 & 1) == 0 && VEHICLECONTROL != 0) {
            vec.x = global_camera.mtx._30;
            vec.y = global_camera.mtx._31;
            vec.z = global_camera.mtx._32;
            vec2.x = player->obj.pos.x - vec.x;
            vec2.z = player->obj.pos.z - vec.z;
            ang = NuAtan2D(vec2.x, vec2.z);
            vec.x = vec.x + NuTrigTable[(u16)ang] * 5.0f;
            vec.z = vec.z + NuTrigTable[(u16)(ang + 0x4000)] * 5.0f;
            AddVariableShotDebrisEffect(GDeb[151].i, &vec, 1, 0, 0);
        }
    }

    if (Level == 0xF && player->obj.dead == 0) {
        key = LDATA->foga * 50;
        if (SKELETALCRASH != 0) {
            LDATA->fogr = 0;
            LDATA->fogg =
                (s32)((f32)(s32)(NuTrigTable[(u16)(water1 / 50)] * 5.0f)
                      + 65.0f);
            water1 = JONMASK(water1, 256);
            LDATA->fogb = 50;
            if (key < 0x1900) {
                key = 0x1900;
            }
            if (key < 0x3070) {
                key = key + 480;
            } else {
                key = 0x31CE;
            }
            LDATA->fogfar = 3.21f;
            LDATA->fognear = 3.2f;
        } else {
            xrayon = 0;
            LDATA->fogr = 0x40;
            LDATA->fogg = 0x10;
            LDATA->fogb = 0;
            LDATA->fognear = 1.0f;
            LDATA->fogfar = 9.0f;
            if (key > 0x1900) {
                key = 0x1900;
            }
            if (key > 0xC80) {
                key = key - 480;
            } else {
                key = 0xC80;
            }
        }
        LDATA->foga = key / 50;
        key = LDATA->hazea * 50;
        if (VEHICLECONTROL != 1) {
            key -= 300;
            if (key < 0) {
                key = 0;
            }
        } else {
            key += 300;
            if (key >= 0x2455) {
                key = 0x2454;
            }
            vec.x = player->obj.pos.x
                    + NuTrigTable[(u16)(player->obj.hdg + 0x1000)] * -0.3f;
            vec.y = player->obj.pos.y + 0.55f;
            vec.z = player->obj.pos.z
                    + NuTrigTable[(u16)(player->obj.hdg + 0x5000)] * -0.3f;
            AddVariableShotDebrisEffect(GDeb[104].i, &vec, 1, 0x4000,
                                        (s16)(player->obj.hdg - 0x4000));
            vec.x = player->obj.pos.x
                    + NuTrigTable[(u16)(player->obj.hdg - 0x1000)] * -0.3f;
            vec.z = player->obj.pos.z
                    + NuTrigTable[(u16)(player->obj.hdg + 0x3000)] * -0.3f;
            AddVariableShotDebrisEffect(GDeb[104].i, &vec, 1, 0x4000,
                                        (s16)(player->obj.hdg - 0x4000));
        }
        LDATA->hazea = key / 50;
    }

    if (Level == 0x1D && in_finish_range < 0x32 && player->obj.dead == 0) {
        vec.x = player->obj.pos.x
                + NuTrigTable[(u16)(player->obj.hdg + 0x1000)] * -0.3f;
        vec.y = player->obj.pos.y + 0.55f;
        vec.z = player->obj.pos.z
                + NuTrigTable[(u16)(player->obj.hdg + 0x5000)] * -0.3f;
        AddVariableShotDebrisEffect(GDeb[104].i, &vec, 1, 0x4000,
                                    (s16)(player->obj.hdg - 0x4000));
        vec.x = player->obj.pos.x
                + NuTrigTable[(u16)(player->obj.hdg - 0x1000)] * -0.3f;
        vec.z = player->obj.pos.z
                + NuTrigTable[(u16)(player->obj.hdg + 0x3000)] * -0.3f;
        AddVariableShotDebrisEffect(GDeb[104].i, &vec, 1, 0x4000,
                                    (s16)(player->obj.hdg - 0x4000));
    }

    if (Level == 0xA) {
        dz = global_camera.mtx._32 - -77.3f;
        dx = global_camera.mtx._30 - -0.46f;
        dy = dx * dx + dz * dz;
        dx = global_camera.mtx._30 - -75.24f;
        dz = global_camera.mtx._32 - -249.74f;
        temp = dx * dx + dz * dz;
        if (dy < temp) {
            temp = dy;
        }
        temp = NuFsqrt(temp) - 8.0f;
        if (temp < 1.0f) {
            temp = 1.0f;
        }
        if (temp > 40.0f) {
            LDATA->fogr = 0x30;
            LDATA->fogg = 0x20;
            LDATA->fogb = 0x10;
            LDATA->foga = 0x40;
            LDATA->hazea = 0;
            LDATA->fognear = 14.0f;
            LDATA->fogfar = 45.0f;
        } else {
            LDATA->fogr = 0x50;
            LDATA->fogg = 0xF;
            LDATA->fogb = 0;
            LDATA->foga = 0x40;
            LDATA->hazea = 0x80;
            LDATA->fognear = temp;
            LDATA->fogfar = temp + 5.0f;
        }
    }

    if (VEHICLECONTROL == 1 && player->obj.vehicle == 0x20) {
        vec.y = player->obj.pos.y + NuTrigTable[(u16)(subprop / 50)] * 0.15f;
        temp = NuTrigTable[(u16)(subprop / 50 + 0x4000)] * 0.15f + 0.8f;
        vec.x = player->obj.pos.x
                + NuTrigTable[(u16)(player->obj.hdg + 0x5000)] * temp;
        vec.z = player->obj.pos.z
                + NuTrigTable[(u16)(player->obj.hdg - 0x7000)] * temp;
        AddVariableShotDebrisEffect(GDeb[22].i, &vec, 1, 0x4000,
                                    (s16)(player->obj.hdg - 0x4000));
        vec.x = player->obj.pos.x
                + NuTrigTable[(u16)(player->obj.hdg - 0x5000)] * temp;
        vec.z = player->obj.pos.z
                + NuTrigTable[(u16)(player->obj.hdg - 0x1000)] * temp;
        AddVariableShotDebrisEffect(GDeb[22].i, &vec, 1, 0x4000,
                                    (s16)(player->obj.hdg - 0x4000));
        subprop = JONMASK(subprop, 35000);
        if (Paused == 0) {
            vec.x = player->mtxLOCATOR[1][2]._30;
            vec.y = player->mtxLOCATOR[1][2]._31;
            vec.z = player->mtxLOCATOR[1][2]._32;
            vec.x = vec.x + player->mtxLOCATOR[1][2]._20 * -2.0f;
            vec.y = vec.y + player->mtxLOCATOR[1][2]._21 * -2.0f;
            vec.z = vec.z + player->mtxLOCATOR[1][2]._22 * -2.0f;
            if (FRAME == FRAMES - 1) {
                NuLightAddSpot(&vec, 0x60000020, 0, 1.5f);
            }
            vec.x = player->mtxLOCATOR[1][3]._30;
            vec.y = player->mtxLOCATOR[1][3]._31;
            vec.z = player->mtxLOCATOR[1][3]._32;
            vec.x = vec.x + player->mtxLOCATOR[1][3]._20 * -2.0f;
            vec.y = vec.y + player->mtxLOCATOR[1][3]._21 * -2.0f;
            vec.z = vec.z + player->mtxLOCATOR[1][3]._22 * -2.0f;
            if (FRAME == FRAMES - 1) {
                NuLightAddSpot(&vec, 0x60000020, 0, 1.5f);
            }
        }
    }

    if ((Level == 0x22 || Level == 6)
        && (player->tap != 0 || player->spin != 0) && VEHICLECONTROL == 2) {
        AddVariableShotDebrisEffect(GDeb[20].i, &player->obj.pos, 1, 0, 0);
    }

    if (Level == 0xB) {
        vec.x = global_camera.mtx._30;
        vec.y = global_camera.mtx._31 - 9.0f;
        vec.z = global_camera.mtx._32;
        AddVariableShotDebrisEffect(GDeb[94].i, &vec, 1, 0, 0);
    }

    if ((Level == 0xB || Level == 0xC) && (jonframe1 & 1) == 0) {
        vec.x = global_camera.mtx._30;
        vec.y = global_camera.mtx._31;
        vec.z = global_camera.mtx._32;
        vec2.x = player->obj.pos.x - vec.x;
        vec2.z = player->obj.pos.z - vec.z;
        ang = NuAtan2D(vec2.x, vec2.z);
        vec.z = vec.z + NuTrigTable[(u16)(ang + 0x4000)] * 5.0f;
        vec.x = vec.x + NuTrigTable[(u16)ang] * 5.0f;
        AddVariableShotDebrisEffect(GDeb[98].i, &vec, 1, 0, 0);
    }

    if (Level == 0x1A) {
        for (loop = 3; loop >= 0; loop--) {
            vec.x = global_camera.mtx._30;
            vec.y = global_camera.mtx._31;
            vec.z = global_camera.mtx._32;
            vec2.x = player->obj.pos.x - vec.x;
            vec2.z = player->obj.pos.z - vec.z;
            ang = NuAtan2D(vec2.x, vec2.z);
            vec.x = vec.x + NuTrigTable[(u16)ang] * 10.0f;
            vec.z = vec.z + NuTrigTable[(u16)(ang + 0x4000)] * 10.0f;
            AddVariableShotDebrisEffect(GDeb[113].i, &vec, 1, 0, 0);
        }
    }

    if (Level == 0xD) {
        temp = NuFsqrt(torndist) * 0.25f;
        if (temp < 30.0f) {
            if (temp < 10.0f) {
                key = 1;
                r = (s32)(10.0f - temp + 1.0f);
                if (r >= 6) {
                    r = 5;
                }
            } else {
                key = (s32)(temp / 10.0f);
                r = 1;
            }
        } else {
            r = 1;
            key = 3;
        }
        if (jonframe1 % key == 0 || r != 1) {
            for (loop = r; loop > 0; loop--) {
                vec.x = global_camera.mtx._30;
                vec.y = global_camera.mtx._31;
                vec.z = global_camera.mtx._32;
                vec2.x = player->obj.pos.x - vec.x;
                vec2.z = player->obj.pos.z - vec.z;
                ang = NuAtan2D(vec2.x, vec2.z);
                vec.x = vec.x + NuTrigTable[(u16)ang] * 10.0f;
                vec.z = vec.z + NuTrigTable[(u16)(ang + 0x4000)] * 10.0f;
                if ((qrand() & 7) != 0) {
                    AddVariableShotDebrisEffect(GDeb[110].i, &vec, 1, 0, 0);
                } else if ((jonframe1 & 1) != 0) {
                    AddVariableShotDebrisEffect(GDeb[111].i, &vec, 1, 0, 0);
                } else {
                    AddVariableShotDebrisEffect(GDeb[112].i, &vec, 1, 0, 0);
                }
            }
        }
    }

    if (Level == 8 || Level == 0xE || Level == 0x17) {
        if (Level != 8) {
            water1 -= 60;
            if (water1 < 0) {
                if (qrand() <= 0x7FFF) {
                    water1 = ((qrand() & 0xFF) + 60) * 50;
                } else {
                    water1 = ((qrand() & 0xF) + 10) * 50;
                }
            }
            if (water1 < 0x96) {
                if (water1 + 60 >= 0x96) {
                    if (Level == 0xE) {
                        GameSfx(0xD3, 0);
                    } else {
                        GameSfx(0xCF, 0);
                    }
                }
                LDATA->fogr = 0x7F;
                LDATA->fogg = 0x7F;
                LDATA->fogb = 0x7F;
                LDATA->foga = 0x4C;
                LDATA->fogfar = 3.0f;
                LDATA->fognear = 1.0f;
            } else {
                LDATA->fogr -= 8;
                if ((s8)LDATA->fogr < 0) {
                    LDATA->fogr = 0;
                }
                LDATA->fogg -= 8;
                if ((s8)LDATA->fogg < 6) {
                    LDATA->fogg = 6;
                }
                LDATA->fogb -= 8;
                if ((s8)LDATA->fogb < 0x21) {
                    LDATA->fogb = 0x21;
                }
                LDATA->foga = 0x4C;
                LDATA->fognear = LDATA->fognear + 2.0f / 50.0f * 60.0f;
                if (LDATA->fognear > 3.0f) {
                    LDATA->fognear = 3.0f;
                }
                LDATA->fogfar = LDATA->fogfar + 2.0f / 50.0f * 60.0f;
                if (LDATA->fogfar > 10.0f) {
                    LDATA->fogfar = 10.0f;
                }
            }
        } else {
            if (VEHICLECONTROL != 0) {
                NuBridgeOn(0);
            } else {
                NuBridgeOn(1);
            }
        }
        vec.z = global_camera.mtx._32;
        vec.x = global_camera.mtx._30;
        vec.y = global_camera.mtx._31 + 3.0f;
        vec2.x = player->obj.pos.x - vec.x;
        vec2.z = player->obj.pos.z - vec.z;
        ang = NuAtan2D(vec2.x, vec2.z);
        vec.z = vec.z + NuTrigTable[(u16)(ang + 0x4000)] * 5.0f;
        vec.x = vec.x + NuTrigTable[(u16)ang] * 5.0f;
        AddVariableShotDebrisEffect(GDeb[95].i, &vec, 0x10, -0x8000, 0);
        if (Level == 0x17 || (Level == 8 && VEHICLECONTROL != 0)) {
            g = 1;
            temp = 8192.0f;
            r = 1;
        } else {
            temp = 4096.0f;
            g = 0;
            r = 4;
        }
        for (loop = r; loop != 0; loop--) {
            vec.x = player->obj.pos.x + (f32)(qrand() - 0x8000) / temp;
            vec.y = player->obj.pos.y + 10.0f;
            vec.z = player->obj.pos.z + (f32)(qrand() - 0x8000) / temp;
            vec.y = NewShadow(&vec, 0.0f);
            if (vec.y != 2000000.0f) {
                key = -1;
                vec.y = vec.y - 0.2f;
                AddFiniteShotDebrisEffect(&key, GDeb[96].i, &vec, 1);
                if (g == 0) {
                    g = 1;
                    vec.y = vec.y + 0.35f;
                    AddVariableShotDebrisEffect(GDeb[97].i, &vec, 1, 0, 0);
                }
            }
        }
    }

    if (rockpt != 0) {
        pt = rockpt;
        if (pt[0] != 9999.0f) {
            do {
                temp = pt[3];
                pt[3] = temp - 1.0f;
                if (pt[3] <= 0.0f) {
                    pt[3] = pt[3] + pt[4] * 50.0f / 60.0f;
                }
                vec.x = pt[0];
                vec.y = pt[1];
                vec.z = pt[2];
                dx = player->obj.pos.x - vec.x;
                dy = player->obj.pos.y - vec.y;
                dz = player->obj.pos.z - vec.z;
                if (pt[11] < 0.0f
                    || dx * dx + dy * dy + dz * dz < pt[11] * pt[11]) {
                    for (loop = 1; (f32)loop <= pt[5]; loop++) {
                        if (pt[6] * (f32)loop < temp
                            && pt[3] <= pt[6] * (f32)loop) {
                            AddDeb3Ang(&vec, (s32)pt[7], (s32)pt[8], 0,
                                       (s32)pt[9], (s32)pt[10]);
                            if (rockpt == HotRocks) {
                                GameSfx(0x85, &vec);
                            }
                            break;
                        }
                    }
                }
                pt += 12;
            } while (pt[0] != 9999.0f);
        }
        if (rockpt == HotRocks) {
            if (pVIS - world_scene[0]->splines == 1) {
                if (iVIS >= 0x6C && hutexplode == 0) {
                    hutexplode = 1;
                    for (loop = 0; HutList[loop] != 0; loop++) {
                        if (NuSpecialFind(world_scene[0], &obj,
                                          HutList[loop]) != 0) {
                            StartHGobjAnim(&obj);
                        }
                    }
                }
            }
            if (hutexplode > 0) {
                key = hutexplode;
                hutexplode += 60;
                if (hutexplode >= 0x3E8) {
                    vec.x = 11.94f;
                    vec.y = 2.12f;
                    vec.z = -42.61f;
                    if (key < 0x3E8) {
                        exkey = -1;
                        AddFiniteShotDebrisEffect(&exkey, GDeb[43].i, &vec, 1);
                        exroty = 0xC8000;
                        GameSfx(0xD1, 0);
                        NewRumble(&player->rumble, 0xFF);
                        NewBuzz(&player->rumble, 0xA);
                    }
                    hutexplode = 0x5DC;
                    hutframe += 60;
                    if (hutframe >= 0xC9) {
                        hutframe -= 200;
                        AddVariableShotDebrisEffect(GDeb[42].i, &vec, 1, 0, 0);
                    }
                    if (exroty > 0) {
                        DebrisEmitterOrientation(exkey, 0x1000,
                                                 (s16)(exroty / 50));
                        exroty -= 0x1E000;
                    }
                }
            }
            if (iVIS >= 0xCA && iVIS < 0xE5 && pVIS != 0) {
                dropfire = 0x1F4;
            }
            dropfire -= 60;
            if (dropfire <= 0) {
                vec.x = player->obj.pos.x
                        + (f32)(qrand() - 0x8000) * 0.0001220703125f;
                vec.y = player->obj.pos.y + 10.0f;
                vec.z = player->obj.pos.z
                        + (f32)(qrand() - 0x8000) * 0.0001220703125f;
                vec.y = NewShadow(&vec, 0.0f);
                if (vec.y != 2000000.0f) {
                    if (vec.y < 0.0f) {
                        firedrop[firedroppt].type = 0x29;
                        vec.y = 0.0f;
                    } else {
                        firedrop[firedroppt].type = 0x28;
                    }
                    firedrop[firedroppt].time = 3000.0f;
                    firedrop[firedroppt].pos = vec;
                    firedroppt = (firedroppt + 1) & 0xF;
                    vec.y = vec.y + 10.0f;
                    AddVariableShotDebrisEffect(GDeb[39].i, &vec, 1, -0x8000,
                                                0);
                    dropfire = 0x1F4;
                } else {
                    dropfire = 0x32;
                }
            }
            for (loop = 0; loop <= 0xF; loop++) {
                if (firedrop[loop].time != 0.0f) {
                    firedrop[loop].time = firedrop[loop].time - 60.0f;
                    if (firedrop[loop].time <= 0.0f) {
                        firedrop[loop].time = 0.0f;
                        key = -1;
                        AddFiniteShotDebrisEffect(&key,
                                                  GDeb[firedrop[loop].type].i,
                                                  &firedrop[loop].pos, 1);
                        if (NuCameraClipTestPoints(&firedrop[loop].pos, 1, 0)
                            == 0) {
                            if (firedrop[loop].type == 0x28) {
                                GameSfx(0x9C, &firedrop[loop].pos);
                            } else {
                                GameSfx(0x85, &firedrop[loop].pos);
                            }
                        }
                    }
                }
            }
        }
    }

    if (Level == 8) {
        rsfxpt = rsfxJung;
    }
    PlayRandSFX();
}


void ProcDeb3(void) {
    s32 loop;
    s32 flag;
    struct deb3_s *deb;
    struct nuvec_s vec;
    struct nuvec_s t;
    struct numtx_s mat;
    f32 dist;
    f32 dx;
    f32 dy;
    f32 dz;
    f32 radius;
    f32 r;

    LaunchObjects();
    deb = deb3;
    NuMtxSetIdentity(&mat);
    radius = (player->obj.max.y - player->obj.min.y) * player->obj.SCALE * 0.5f;
    for (loop = 0; loop < 0x40; deb++, loop++) {
        if (deb->timer != 0) {
            deb->diff.x *= 0.5f;
            deb->diff.y *= 0.5f;
            deb->diff.z *= 0.5f;
            if ((deb->status & 1) == 0) {
                mat = deb->mtx;
                RBodyMove(deb, D_0062E774);
                if ((deb->info->info & 6) != 0) {
                    dx = deb->mtx._30 - player->obj.pos.x;
                    dy = deb->mtx._31
                         - (player->obj.pos.y
                            + (player->obj.bot + player->obj.top)
                                  * player->obj.SCALE * 0.5f);
                    dz = deb->mtx._32 - player->obj.pos.z;
                    r = deb->info->size * 0.5f + radius;
                    if (dx * dx + dy * dy + dz * dz < r * r) {
                        if ((deb->info->info & 2) != 0
                            && deb->info->type == 0x93) {
                            player->freeze = 100;
                            player->spin = 0;
                            GameSfx(0x54, &player->obj.pos);
                        }
                        deb->timer = 1;
                        deb->status |= 4;
                    }
                }
                deb->check--;
                if (deb->check < 1) {
                    if (deb->norm.y != 100.0f) {
                        deb->diff.x += deb->mtx._30 - deb->impact.x;
                        deb->diff.y += deb->mtx._31 - deb->impact.y;
                        deb->diff.z += deb->mtx._32 - deb->impact.z;
                        deb->mtx._30 = deb->impact.x;
                        deb->mtx._31 = deb->impact.y;
                        deb->mtx._32 = deb->impact.z;
                        if (deb->info->impact != 0) {
                            (*deb->info->impact)(deb);
                        }
                        if (rbclass[deb->info->classid].mass != 0.0f) {
                            t = deb->impact;
                            CubeImpact(&mat, &deb->mtx, &deb->norm,
                                       deb->info->size * 0.5f, &t);
                            deb->norm.x = -deb->norm.x;
                            deb->norm.y = -deb->norm.y;
                            deb->norm.z = -deb->norm.z;
                            RBodyImpact(deb, &t, &deb->norm);
                        } else {
                            FullReflect(&deb->norm, &deb->velocity,
                                        &deb->velocity);
                            deb->velocity.x *=
                                rbclass[deb->info->classid].kr;
                            deb->velocity.y *=
                                rbclass[deb->info->classid].kr;
                            deb->velocity.z *=
                                rbclass[deb->info->classid].kr;
                            if (rbclass[deb->info->classid].kf != 0.0f) {
                                deb->angularMomentum.x =
                                    (qrand() - 0x8000 >> 8)
                                    * rbclass[deb->info->classid].kf;
                                deb->angularMomentum.y =
                                    (qrand() - 0x8000 >> 8)
                                    * rbclass[deb->info->classid].kf;
                                deb->angularMomentum.z =
                                    (qrand() - 0x8000 >> 8)
                                    * rbclass[deb->info->classid].kf;
                            }
                        }
                        if (deb->info->type == 0x93) {
                            GameSfx(0x44, (struct nuvec_s *)&deb->mtx._30);
                        }
                    }
                    vec.x = deb->velocity.x * 8.0f / 50.0f;
                    vec.y = deb->velocity.y * 8.0f / 50.0f;
                    vec.z = deb->velocity.z * 8.0f / 50.0f;
                    dist = vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
                    t.x = deb->mtx._30;
                    t.y = deb->mtx._31;
                    t.z = deb->mtx._32;
                    if ((deb->info->info & 8) != 0) {
                        flag = 0;
                    } else {
                        flag = NewRayCast(&t, &vec, deb->info->size);
                    }
                    if (flag == 0) {
                        deb->status |= 2;
                    }
                    if ((flag >= 1 && flag <= 15)
                        || (flag > 15 && (deb->status & 2) != 0)) {
                        deb->impact.x = t.x + vec.x;
                        deb->impact.y = t.y + vec.y;
                        deb->impact.z = t.z + vec.z;
                        deb->norm = ShadNorm;
                        vec.x = vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
                        vec.x = NuFsqrt(vec.x);
                        dist = NuFsqrt(dist);
                        if (dist == 0.0f) {
                            deb->check = 0;
                        } else {
                            deb->check = vec.x * 8.0f / dist;
                        }
                        if (deb->check == 0) {
                            deb->diff.x += deb->mtx._30 - deb->impact.x;
                            deb->diff.y += deb->mtx._31 - deb->impact.y;
                            deb->diff.z += deb->mtx._32 - deb->impact.z;
                            deb->mtx._30 = deb->impact.x;
                            deb->mtx._31 = deb->impact.y;
                            deb->mtx._32 = deb->impact.z;
                            deb->count++;
                            if (deb->count > 4) {
                                if (D_0062E778 < ShadNorm.y) {
                                    if ((deb->info->info & 1) != 0) {
                                        deb->timer = 1;
                                    } else {
                                        deb->status |= 1;
                                    }
                                }
                            }
                        } else {
                            deb->count = 0;
                        }
                    } else {
                        deb->check = 8;
                        deb->norm.y = 100.0f;
                        deb->count = 0;
                    }
                }
            }
            deb->mtx._30 += deb->diff.x;
            deb->mtx._31 += deb->diff.y;
            deb->mtx._32 += deb->diff.z;
            vec.x = deb->mtx._30;
            vec.y = deb->mtx._31;
            vec.z = deb->mtx._32;
            if (deb->info->deb != 0 && (deb->status & 1) == 0) {
                if (deb->info->rate > 0) {
                    AddVariableShotDebrisEffect(GDeb[deb->info->deb].i, &vec,
                                                deb->info->rate, 0, 0);
                } else {
                    if (deb->timer % -deb->info->rate == 0) {
                        AddVariableShotDebrisEffect(GDeb[deb->info->deb].i,
                                                    &vec, 1, 0, 0);
                    }
                }
            }
            deb->mtx._30 -= deb->diff.x;
            deb->mtx._31 -= deb->diff.y;
            deb->mtx._32 -= deb->diff.z;
            deb->timer--;
            if (deb->timer < 1) {
                if (deb->info->end != 0) {
                    (*deb->info->end)(deb);
                }
                if (deb->info->type == 0x93) {
                    GameSfx(0x70, (struct nuvec_s *)&deb->mtx._30);
                }
            } else if ((deb->info->info & 0x10) != 0) {
                deb->shadow = NewShadow((struct nuvec_s *)&deb->mtx._30, 0.0f);
            }
        }
    }
}
