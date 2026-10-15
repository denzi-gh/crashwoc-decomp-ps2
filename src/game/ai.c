/*
 * Unit: game/ai
 *
 * Functions:
 *   0x0022ef10 LoadAI
 *   0x0022f148 SaveAI
 *   0x0022f2c8 ResetAI
 *   0x0022f568 UpdateDRAINDAMAGE
 *   0x0022f858 DrawDRAINDAMAGE
 *   0x0022f928 WaterCrunchFunction_Spladoosh
 *   0x0022fac8 UpdateCRUNCHTIME
 *   0x002308c0 DrawCRUNCHTIME
 *   0x00230ae0 SpaceCrunchFunction_NewPoint
 *   0x00230c20 FindGongBongerAnim
 *   0x00230d28 MoveCreature
 *   0x00234920 InitAI
 *   0x002349c0 FindNearestCreature
 *   0x00234ab8 FindAIType
 *   0x00234b60 ResetDRAINDAMAGE
 *   0x00234ba0 WaterCrunchFunction_Attack
 *   0x00234bf0 WaterCrunchFunction_Punch1
 *   0x00234cc8 WaterCrunchFunction_Punch2
 *   0x00234da0 WaterCrunchFunction_Defeated
 *   0x00234dd8 ResetCRUNCHTIME
 *   0x00234eb0 SpaceCrunchFunction_NewCount
 *   0x00234ef0 SpaceCrunchFunction_AttackCortex
 *   0x00234f20 SpaceCrunchFunction_CheckCortexBack
 *   0x00234f28 SpaceCrunchFunction_PunchCortex
 *   0x00234f88 SpaceCortexFunction_CheckPunch
 *   0x00234f90 SpaceCortexFunction_Defeated
 *   0x00234fd0 SpaceCortexFunction_TakeHit
 *   0x00235000 RatioDifferenceAlongLine
 *   0x00235068 FindAILabel
 *   0x002350a8 PlayerLateralInRange
 *   0x00235198 PlayerLateralOutOfRange
 */

#include "nu_libc.h"

#include "nu3d/nugscn.h"
#include "nu3d/nulight.h"
#include "numath/nutrig.h"
#include "numath/nuvec.h"
#include "game/gametypes.h"
#include "gamelib/terrain.h"

#include "creature.h"

/* AI path positions (D_005CA154 == AITab[0].pos): stride 0x80 per AI entry,
 * AIType[ai_type].label nuvec sub-points each. */
struct aipath_s {
    struct nuvec_s pos[10];  /* 0x00 (0x78) */
    u8 pad[8];               /* 0x78 */
}; /* 0x80 */

extern struct aitype_s AIType[];       /* 0x005c9518 */
extern struct aitab_s AITab[];         /* 0x005ca140 */
extern struct rail_s Rail[];           /* 0x00586388 */
extern struct RPos_s gempath_RPos;     /* 0x0058b518 */
extern u8 LevelAIType[];               /* 0x005ca0d0 */
extern s32 LEVELAITYPES;               /* 0x00631bc8 */
extern s32 LEVELAICOUNT;               /* 0x00631bcc */
extern s32 Level;                      /* 0x00630b90 */
extern s32 bonus_restart;              /* 0x00630d34 */
extern u8 temp_iRAIL;                  /* 0x00630ab4 */
extern s16 temp_iALONG;                /* 0x00630ab8 */
extern f32 temp_fALONG;                /* 0x00630abc */
extern char D_0061FFA0[];              /* load-screen name for InitAI */
extern f32 D_0062E30C;                 /* FindNearestCreature initial best dist */
extern s32 temp_creature_i;            /* 0x00631bd0 */

void LoadScreen(char *name);           /* 0x001cabf0 */
f32 FindNearestCreature(struct nuvec_s *pos, s32 type, struct nuvec_s *out); /* 0x002349c0 */
void LoadAI(void);                     /* 0x0022ef10 */
void ResetAI(void);                    /* 0x0022f2c8 */
void GetALONG(struct nuvec_s *pos, struct RPos_s *rpos, s32 iRAIL, s32 iALONG, s32 dir); /* 0x001dac00 */
s32 AheadOfCheckpoint(s32 iRAIL, s32 iALONG, f32 fALONG); /* 0x001f0278 */
void RemoveGameObject(struct obj_s *obj); /* 0x00200c38 */

extern struct aipath_s D_005CA154[];   /* saved AI path positions */
extern char tbuf[];                    /* 0x00592090 */
extern char LevelFileName[];           /* 0x0058b098 */
extern char D_00631B88[];              /* sprintf format for the .ai filename */
extern u8 Chase[];                     /* 0x005f3ed0 (scratch load buffer) */

void *NuFileOpen(char *name, s32 mode);            /* 0x001002d8 */
s32 NuFileLoadBuffer(char *name, void *buf, s32 max); /* 0x00100a88 */
void *NuMemFileOpen(void *buf, s32 size, s32 mode);   /* 0x00103430 */
void NuFileClose(void *f);                         /* 0x00102c20 */
f32 NuFileReadFloat(void *f);                      /* 0x00102e20 */
s32 NuFileReadInt(void *f);                        /* 0x00102e48 */
s32 NuFileReadChar(void *f);                       /* 0x00102ec0 */
void NuFileWriteFloat(void *f, f32 v);             /* 0x00102f08 */
s32 NuFileWriteInt(void *f, s32 v);                /* 0x00102f30 */
void NuFileWriteChar(void *f, s32 v);              /* 0x00102fa8 */
s32 qrand(void);
f32 NuFsqrt(f32 x);
f32 RatioAlongLine(struct nuvec_s *pos, struct nuvec_s *a, struct nuvec_s *b); /* 0x001f0730 */

extern s32 D_00633300;
extern s32 DrainDamage_Intro;
extern s16 D_00633308[3];
extern s32 D_00633338;
extern s32 D_0063333C;
extern s32 SpaceCortex_Defeated;
extern s32 boss_dead;
extern s32 Hub;

extern struct game_s Game;
extern s32 D_00633314;
extern s32 gamesfx_effect_volume;
extern void *world_scene[32];
extern char D_0061FFD8[];
extern char D_0061FFE8[];
extern char D_0061FFF8[];

extern s32 D_00633318;
extern s32 D_0063331C;
extern s32 D_00633320;
extern s32 D_00633324;
extern s32 D_00633328;
extern s32 D_00631B94;
extern s32 jcrunch;
extern s32 CrunchTime_Intro;
extern s32 fire_attack_on;
extern s32 earth_attack_on;
extern s32 water_attack_on;
extern s32 weather_attack_on[];        /* 0x005c8xxx (absolute-addressed array) */
extern u8 D_005C8870[];
extern struct nuvec_s D_0061FFB0;
void InitVehMasks(void);
void InitVehMask(s32 a, s32 b);
void SetNewMaskStuff(s32 a, void *b, struct nuvec_s *c, s32 d, s32 e,
                     f32 f, f32 g, f32 h, f32 i, f32 j);

extern u16 D_00633304;
extern u16 D_00633306;
extern struct CharacterModel *D_00633310;
extern f32 D_0062E268;
extern f32 NuTrigTable[];
extern struct nuvec_s D_006B7610;      /* DRAINDAMAGE base position */
extern struct anim_s D_006B7620;       /* DRAINDAMAGE anim packet */
extern struct nuvec_s D_0058656C;      /* DRAINDAMAGE aim target */
extern f32 DRAINWAWADY;                /* 0x00631b90 */
extern f32 D_0062E264;                 /* DRAINDAMAGE anim rate */
u16 SeekRot(u16 a, u16 target, s32 rate);

extern struct cammtx_s GameCam;        /* 0x005864c8 */
extern struct nuvec_s v001;
extern f32 D_0062E26C;                 /* Spladoosh camera judder time */
extern f32 D_0062E270;                 /* Spladoosh debris spread scale */
void JudderGameCamera(struct cammtx_s *cam, f32 time, struct nuvec_s *pos);
void AddGameDebrisRot(s32 i, struct nuvec_s *pos, s32 n, u16 xrot, u16 yrot);
void Draw3DCharacter(struct nuvec_s *pos, s32 xrot, s32 yrot, s32 z,
                     struct CharacterModel *model, f32 scale, s32 action,
                     f32 anim_time, s32 last);

/* debris emitter (debkeydata): PS2 stride 0x56C; delay at +0x4C2. */
struct deb_s {
    u8 pad0[0x4C0];          /* 0x000 */
    s16 oncount;             /* 0x4C0 */
    s16 delay;               /* 0x4C2 */
    u8 pad1[0xA8];           /* 0x4C4 */
}; /* 0x56C */

extern struct gdeb_s GDeb[170];        /* 0x0060dd00 */
extern struct deb_s debkeydata[];      /* 0x00320eb0 */
extern struct nuvec_s punchpos1[4];    /* 0x005c7e08 */
extern struct nuvec_s punchpos2[4];    /* 0x005c7e38 */
void AddFiniteShotDebrisEffect(s32 *key, s32 effect, struct nuvec_s *pos, s32 n);
void DebrisEmitterOrientation(s32 key, s32 a, s32 hdg);

void GameSfx(s32 id, struct nuvec_s *pos);
void NewRumble(struct rumble_s *rumble, s32 power);
void NewBuzz(struct rumble_s *rumble, s32 frames);

void InitAI(void)
{
    s32 i;

    LEVELAITYPES = 0;
    for (i = 0; i < 107; i++) {
        if (CRemap[AIType[i].character] != -1) {
            LevelAIType[LEVELAITYPES++] = i;
        }
    }
    LEVELAICOUNT = 0;
    LoadScreen(D_0061FFA0);
    if (Level != 40) {
        LoadAI();
    }
    ResetAI();
}

inline s32 FindAIType(char *name, s32 type)
{
    s32 i;

    for (i = 0; i < 107; i++) {
        if (strcmp(name, AIType[i].name) == 0) {
            if (type == AIType[i].points) {
                if (type < 9) {
                    return i;
                }
            }
        }
    }
    return -1;
}

void ResetAI(void)
{
    s32 n;
    s32 flag;
    struct aitab_s *ai;
    struct rail_s *rail;
    s32 irail;
    s32 j;

    flag = 0;
    if (Rail[7].type == 3) {
        flag = AheadOfCheckpoint(gempath_RPos.iRAIL, gempath_RPos.iALONG,
                                 gempath_RPos.fALONG) != 0;
    }

    ai = AITab;
    for (n = 0; n < LEVELAICOUNT; n++, ai++) {
        GetALONG(&ai->pos[0], 0, -1, -1, 1);
        ai->iRAIL = temp_iRAIL;
        ai->iALONG = temp_iALONG;
        ai->fALONG = temp_fALONG;

        irail = (s8)temp_iRAIL;
        rail = Rail;
        if (irail != -1 && rail[irail].type == 3) {
            if (flag) {
                goto active;
            }
        } else if (bonus_restart == 0 && AheadOfCheckpoint(ai->iRAIL, ai->iALONG, ai->fALONG)) {
        active:
            ai->status = 1;
        }
        ai->time = 0;
        ai->delay = AIType[ai->ai_type].delay;
        ai->origin = ai->pos[0];

        if (AIType[ai->ai_type].points >= 2) {
            for (j = 1; j < AIType[ai->ai_type].points; j++) {
                NuVecAdd(&ai->origin, &ai->origin, &ai->pos[j]);
            }
            NuVecScale(&ai->origin, &ai->origin,
                       1.0f / (f32)AIType[ai->ai_type].points);
        }
    }

    for (n = 1; n < 9; n++) {
        if (Character[n].used != 0) {
            RemoveGameObject(&Character[n].obj);
            Character[n].used = 0;
            Character[n].on = 0;
        }
    }
}


void FindGongBongerAnim(struct nuvec_s *pos, struct nuhspecial_s *out)
{
    struct nuhspecial_s buf[3];
    s32 i;

    memset(buf, 0, sizeof(buf));
    if (world_scene[0] != 0) {
        NuSpecialFind(world_scene[0], &buf[0], D_0061FFD8);
        NuSpecialFind(world_scene[0], &buf[1], D_0061FFE8);
        NuSpecialFind(world_scene[0], &buf[2], D_0061FFF8);
        for (i = 0; i < 3; i++) {
            if (buf[i].special != 0) {
                struct nuvec_s *np = (struct nuvec_s *)&buf[i].special->instance->mtx._30;
                f32 dx = np->x - pos->x;
                f32 dz = np->z - pos->z;
                if (dx * dx + dz * dz < 25.0f) {
                    *out = buf[i];
                    return;
                }
            }
        }
    }
}

s32 WaterCrunchFunction_Punch1(struct creature_s *c)
{
    struct deb_s *deb;
    s32 key;
    s32 i;

    GameSfx(0xCB, &c->obj.pos);
    for (i = 0; i < 4; i++) {
        key = -1;
        AddFiniteShotDebrisEffect(&key, GDeb[141].i, &punchpos1[i], 1);
        DebrisEmitterOrientation(key, 0, (s16)c->obj.hdg);
        deb = &debkeydata[key];
        deb->delay = 30 - i * 10;
    }
    return 1;
}


s32 WaterCrunchFunction_Punch2(struct creature_s *c)
{
    struct deb_s *deb;
    s32 key;
    s32 i;

    GameSfx(0xCB, &c->obj.pos);
    for (i = 0; i < 4; i++) {
        key = -1;
        AddFiniteShotDebrisEffect(&key, GDeb[141].i, &punchpos2[i], 1);
        DebrisEmitterOrientation(key, 0, (s16)c->obj.hdg);
        deb = &debkeydata[key];
        deb->delay = 30 - i * 10;
    }
    return 1;
}


s32 WaterCrunchFunction_Attack(struct creature_s *c)
{
    gamesfx_effect_volume = 0x7FFE;
    GameSfx(0x88, &c->obj.pos);
    NewRumble(&player->rumble, 0xBF);
    NewBuzz(&player->rumble, 0xA);
    return 1;
}

s32 WaterCrunchFunction_Defeated(void)
{
    boss_dead = (Game.hub[Hub].flags & 4) ? 2 : 1;
    return 1;
}

void UpdateDRAINDAMAGE(void)
{
    struct creature_s *c;
    struct nuvec_s pos;
    struct nuvec_s d;
    s32 model;
    u16 xrot;
    u16 yrot;

    FindNearestCreature(&player->obj.pos, 0x7F, 0);
    if (temp_creature_i != -1) {
        c = &Character[temp_creature_i];
    } else {
        c = 0;
    }
    D_00633310 = 0;
    model = CRemap[0x57];
    if (model != -1) {
        D_00633310 = &CModel[model];
        if (D_00633310->anmdata[12] != 0 && D_00633310->anmdata[13] != 0) {
            D_00633308[0] += 0x816;
            D_00633308[1] += 0x57C;
            D_00633308[2] += 0xAB0;
            if (c != 0 || D_00633300 != 0) {
                if (c != 0) {
                    pos.x = c->obj.pos.x - NuTrigTable[c->obj.hdg] * 3.0f;
                    pos.y = c->obj.pos.y + DRAINWAWADY;
                    pos.z = c->obj.pos.z
                          - NuTrigTable[(u16)(c->obj.hdg + 0x4000)] * 3.0f;
                }
                NuVecSub(&d, &D_0058656C, &D_006B7610);
                xrot = NuAtan2D(d.y, NuFsqrt(d.x * d.x + d.z * d.z));
                yrot = NuAtan2D(d.x, d.z);
                if (D_00633300 == 0) {
                    D_00633300 = 1;
                    D_00633304 = xrot;
                    D_00633306 = yrot;
                    D_006B7610 = pos;
                    ResetAnimPacket(&D_006B7620, 0xC);
                    GameSfx(0x1E, &D_006B7610);
                } else {
                    D_00633304 = SeekRot(D_00633304, xrot, 3);
                    D_00633306 = SeekRot(D_00633306, yrot, 3);
                }
                D_006B7620.oldaction = D_006B7620.action;
                UpdateAnimPacket(&CModel[model], &D_006B7620, D_0062E264, 0.0f);
                if (D_006B7620.flags & 1) {
                    if (D_006B7620.action == 0xC) {
                        if (c == 0
                            || NuVecDistSqr(&D_006B7610, &c->obj.pos, 0) > 25.0f) {
                            D_006B7620.newaction = 0xD;
                            GameSfx(0x21, &D_006B7610);
                        }
                    } else {
                        D_00633300 = 0;
                    }
                }
            } else {
                D_00633300 = 0;
            }
        } else {
            D_00633310 = 0;
        }
    }
}

void DrawDRAINDAMAGE(void)
{
    struct nuvec_s pos;

    if (D_00633310 != 0 && D_00633300 != 0) {
        pos.x = D_006B7610.x + NuTrigTable[(u16)D_00633308[0]] * D_0062E268;
        pos.y = D_006B7610.y + NuTrigTable[(u16)D_00633308[1]] * D_0062E268;
        pos.z = D_006B7610.z + NuTrigTable[(u16)D_00633308[2]] * D_0062E268;
        Draw3DCharacter(&pos, D_00633304, (D_00633306 - 0x8000) & 0xffff, 0,
                        D_00633310, 3.0f, D_006B7620.action,
                        D_006B7620.anim_time, 0);
    }
}

s32 WaterCrunchFunction_Spladoosh(struct creature_s *c)
{
    struct nuvec_s v;
    s32 i;
    s32 r;

    GameSfx(0x47, &c->obj.pos);
    JudderGameCamera(&GameCam, D_0062E26C, 0);
    NewRumble(&player->rumble, 0x7F);
    NewBuzz(&player->rumble, 0xA);
    for (i = 0; i < 0x40; i++) {
        NuVecScale(&v, &v001, (f32)qrand() * D_0062E270);
        NuVecRotateY(&v, &v, qrand());
        r = qrand();
        v.y += c->obj.pos.y;
        if (r < 0x4000) {
            v.x += c->mtxLOCATOR[0][0]._30;
            v.z += c->mtxLOCATOR[0][0]._32;
        } else if (r < 0x8000) {
            v.x += c->mtxLOCATOR[0][1]._30;
            v.z += c->mtxLOCATOR[0][1]._32;
        } else {
            v.x += c->obj.pos.x;
            v.z += c->obj.pos.z;
        }
        v.x -= NuTrigTable[c->obj.hdg] * 2.0f;
        v.z -= NuTrigTable[(c->obj.hdg + 0x4000) & 0xFFFF] * 2.0f;
        AddGameDebrisRot(0x8E, &v, 1, 0, 0);
    }
    return 1;
}

extern struct objtab_s ObjTab[];
extern u16 D_00633330[3];              /* CRUNCHTIME wobble angles */
extern struct nuvec_s D_005C8A10[4];   /* CRUNCHTIME stone rest positions */
extern struct nuvec_s D_005C8880[];    /* water attack points */
extern struct nuvec_s D_005C88B0[];    /* fire attack edge a */
extern struct nuvec_s D_005C88FC[];    /* fire attack edge b */
extern struct nuvec_s D_005C8930[9];   /* weather attack ends */
extern struct nuvec_s D_005C89A0[9];   /* weather attack ends */
extern struct nuvec_s D_006B7640;      /* CRUNCHTIME cortex position */
extern struct nuvec_s D_0061FFC0;
extern f32 D_00631B98;
extern f32 D_00631B9C;
extern f32 D_00631BA0;
#define CrunchLava (*(struct nuvec_s *)D_005C8870)
void AddProjectile(struct nuvec_s *pos, struct nuvec_s *target, struct nuvec_s *mom, s32 type,
                   u16 hdg, struct obj_s *owner);
void AddDeb3(struct nuvec_s *pos, s32 effect, s32 n, struct nuangvec_s *rot);
s32 KillPlayer(struct obj_s *obj, s32 anim);
void ProcessVehMasks(void);


s32 UpdateCRUNCHTIME(void)
{
    struct nuvec_s v[2];
    struct nuvec_s p;
    struct nuvec_s ppos;
    struct nuangvec_s rot2;
    struct nuangvec_s rot;
    struct creature_s *c;
    struct creature_s *cortex;
    struct nuvec_s *a;
    struct nuvec_s *b;
    struct nuvec_s *d;
    struct nuvec_s *e;
    struct nuvec_s *path;
    struct nuvec_s *path2;
    f32 t;
    s32 i;
    s32 n;
    s32 fire;
    s32 kill;
    s32 five;
    s32 old;
    u16 ang;
    double dd;

    jcrunch = 0;
    FindNearestCreature(&player->obj.pos, 0x7F, 0);
    if (temp_creature_i != -1) {
        c = &Character[temp_creature_i];
    } else {
        c = 0;
    }
    FindNearestCreature(&player->obj.pos, 2, 0);
    if (temp_creature_i != -1) {
        cortex = &Character[temp_creature_i];
    } else {
        cortex = 0;
    }
    ppos.x = player->obj.pos.x;
    ppos.y = player->obj.pos.y
           + (player->obj.bot + player->obj.top) * player->obj.SCALE * 0.5f;
    ppos.z = player->obj.pos.z;
    D_00633330[0] += 0x57C;
    D_00633330[1] += 0x804;
    D_00633330[2] += 0xA20;
    D_00633314 = (cortex != 0) ? 4 - cortex->ai.hits : 0;
    five = 5;

    if (c != 0 && (u32)D_00633314 < 4 && c->ai.i0 > 0 && c->ai.i0 < 5
        && c->ai.terflags == 0 && c->ai.movflags == 0
        && (path = D_005CA154[c->i_aitab].pos,
            NuVecDistSqr(&c->obj.pos, &path[c->ai.i0], 0) < 1.0f)
        && c->obj.anim.newaction != 0x73 && c->obj.anim.newaction != 0x49
        && !(c->obj.anim.action == 0x2C && c->obj.anim.anim_time < 15.0f)
        && !(c->obj.anim.newaction == 0x2C && c->obj.anim.blend != 0)) {
        jcrunch = c->ai.i0;
    } else {
        jcrunch = 0;
    }

    n = -1;
    if (c != 0) {
        for (i = 0; i < 4; i++) {
            path2 = D_005CA154[c->i_aitab].pos;
            if (NuVecDistSqr(&c->obj.pos, &path2[i + 1], 0) < 0.5f) {
                n = i;
                i = 4;
            }
        }
    }
    for (i = 0; i < 4; i++) {
        if (ObjTab[181 + i].obj.special != 0) {
            *(struct nuvec_s *)&ObjTab[181 + i].obj.special->instance->mtx._30 = D_005C8A10[i];
            if (i == n) {
                ObjTab[181 + i].obj.special->instance->mtx._31 -= 0.125f;
            }
        }
    }

    if (c != 0 && c->ai.i0 == 1 && c->obj.anim.newaction == 0x22
        && (u32)D_00633314 < 4) {
        earth_attack_on = five;
    }
    if (earth_attack_on != 0) {
        earth_attack_on--;
    }
    if (D_00633318 != 0) {
        D_00633318--;
    }
    if (D_00633318 == 0 && earth_attack_on != 0) {
        AddProjectile(0, 0, 0, 0xE, qrand(), 0);
        D_00633318 = 0x19;
    }

    if (c != 0 && c->ai.i0 == 2 && c->obj.anim.newaction == 0x22
        && (u32)D_00633314 < 4) {
        water_attack_on = five;
    }
    if (water_attack_on != 0) {
        water_attack_on--;
    }
    if (D_0063331C != 0) {
        D_0063331C--;
    }
    if (D_0063331C == 0 && water_attack_on != 0) {
        if ((u32)D_00633320 < 2) {
            D_00633320 = qrand() / 0x8000 + 2;
        } else {
            D_00633320 = qrand() / 0x8000;
        }
        a = &D_005C8880[D_00633320];
        rot.x = 0;
        rot.y = NuAtan2D(CrunchLava.x - a->x, CrunchLava.z - a->z);
        rot.z = 0;
        rot2 = rot;
        AddDeb3(a, 0x11, 5, &rot2);
        GameSfx(0x48, a);
        D_0063331C = 0x3E;
    }

    fire = 0;
    if (c != 0 && c->ai.i0 == 3 && c->obj.anim.newaction == 0x22
        && (u32)c->ai.i0 >= (u32)D_00633314) {
        fire = 1;
    }
    if (D_00633324 != 0) {
        D_00633324--;
        if (D_00633324 == 0x32) {
            GameSfx(0x4D, 0);
        }
    }
    if (fire && D_00633324 == 0) {
        do {
            n = qrand() / 0x4000;
        } while (n == fire_attack_on);
        fire_attack_on = n;
        D_00633324 = 100;
        GameSfx(0x85, 0);
    }
    if (D_00633324 != 0) {
        a = &D_005C88B0[fire_attack_on];
        b = &D_005C88B0[fire_attack_on + 1];
        d = &D_005C88FC[fire_attack_on];
        e = &D_005C88FC[fire_attack_on - 1];
        t = qrand() * 1.5259022e-05f;
        v[0].x = a->x + (b->x - a->x) * t;
        v[0].y = a->y + (b->y - a->y) * t;
        v[0].z = a->z + (b->z - a->z) * t;
        v[1].x = e->x + (d->x - e->x) * t;
        v[1].y = e->y + (d->y - e->y) * t;
        v[1].z = e->z + (d->z - e->z) * t;
        t = qrand() * 1.5259022e-05f;
        p.x = v[1].x + (v[0].x - v[1].x) * t;
        p.y = v[1].y + (v[0].y - v[1].y) * t;
        p.z = v[1].z + (v[0].z - v[1].z) * t;
        if ((u32)D_00633324 >= 0x32) {
            for (i = 0; i < 2; i++) {
                AddGameDebrisRot(0x75, &p, 1, 0, 0);
            }
        } else if (D_00633324 != 0) {
            for (i = 0; i < 4; i++) {
                AddGameDebrisRot(0x76, &p, 1, 0, 0);
            }
            if (player->obj.pos.y + player->obj.bot * player->obj.SCALE
                < CrunchLava.y + 1.0f
                && (player->obj.pos.x - a->x) * (b->z - a->z)
                   + (player->obj.pos.z - a->z) * (a->x - b->x) >= 0.0f
                && (player->obj.pos.x - b->x) * (d->z - b->z)
                   + (player->obj.pos.z - b->z) * (b->x - d->x) >= 0.0f
                && (player->obj.pos.x - d->x) * (e->z - d->z)
                   + (player->obj.pos.z - d->z) * (d->x - e->x) >= 0.0f
                && (player->obj.pos.x - e->x) * (a->z - e->z)
                   + (player->obj.pos.z - e->z) * (e->x - a->x) >= 0.0f) {
                KillPlayer(&player->obj, 0xD);
            }
        }
    }

    if (c != 0 && c->ai.i0 == 4 && c->obj.anim.newaction == 0x22
        && (u32)D_00633314 < 4 && D_00633328 == 0) {
        do {
            n = qrand() / 0x1C72;
        } while (weather_attack_on[n] != 0);
        weather_attack_on[n] = 100;
        D_00633328 = 0x21;
        GameSfx(0x32, &D_005C89A0[n]);
    }
    kill = 0;
    for (n = 0; n < 9; n++) {
        if (weather_attack_on[n] != 0) {
            weather_attack_on[n]--;
            if (weather_attack_on[n] == 0x32) {
                gamesfx_effect_volume = 0xBFFD;
                GameSfx(0x4B, &D_005C89A0[n]);
            }
            if (weather_attack_on[n] != 0) {
                AddGameDebrisRot(0x78, &D_005C8930[n], 1, 0, 0);
                AddGameDebrisRot(0x78, &D_005C89A0[n], 1, 0, 0);
            }
        }
        if ((u32)(weather_attack_on[n] - 1) < 0x31) {
            if (NuVecDistSqr(&ppos, &D_005C8930[n], 0) < 0.110889f
                || NuVecDistSqr(&ppos, &D_005C89A0[n], 0) < 0.110889f) {
                kill = 1;
            } else {
                ang = NuAtan2D(D_005C8930[n].x - D_005C89A0[n].x,
                               D_005C8930[n].z - D_005C89A0[n].z);
                NuVecSub(&v[1], &ppos, &D_005C89A0[n]);
                NuVecRotateY(&v[1], &v[1], -ang);
                dd = v[1].x;
                if (dd < 0.0) {
                    dd = 0.0 - dd;
                }
                if (dd < 0.333f) {
                    kill = 1;
                }
            }
        }
    }
    if (D_00633328 != 0) {
        D_00633328--;
    }
    if (kill) {
        KillPlayer(&player->obj, 0xD);
    }

    if (cortex != 0) {
        p.x = cortex->obj.pos.x - NuTrigTable[cortex->obj.hdg] * D_00631B9C;
        p.z = cortex->obj.pos.z
            - NuTrigTable[(cortex->obj.hdg + 0x4000) & 0xFFFF] * D_00631B9C;
        p.y = cortex->obj.pos.y + cortex->obj.top * cortex->obj.SCALE + D_00631B98;
        if (CrunchTime_Intro != 0
            || (cortex->obj.anim.newaction != 0x1C
                && cortex->obj.anim.newaction != 0x46
                && !(cortex->obj.anim.action == 0x49
                     && D_00631BA0 < cortex->obj.anim.anim_time))) {
            p.y += 2.0f;
        }
        p.x += NuTrigTable[D_00633330[0]] * 0.1f;
        p.y += NuTrigTable[D_00633330[1]] * 0.1f;
        p.z += NuTrigTable[D_00633330[2]] * 0.1f;
        D_006B7640.x += (p.x - D_006B7640.x) * 0.039960004f;
        D_006B7640.z += (p.z - D_006B7640.z) * 0.039960004f;
        D_006B7640.y += (p.y - D_006B7640.y) * 0.039960004f;
    }

    old = D_00631B94;
    D_00631B94 = (c != 0 && c->obj.anim.action == 0x73);
    if (D_00631B94 != old) {
        if (D_00631B94 != 0) {
            p = D_0061FFC0;
            SetNewMaskStuff(0, &c->obj.pos, &p, 0, 0,
                            1.0f, -540.0f, 1.0f, 1.0f, 0.0f);
        } else {
            p = D_0061FFB0;
            SetNewMaskStuff(0, D_005C8870, &p, 0, 0,
                            0.0f, -540.0f, 1.0f, 1.0f, 0.0f);
        }
    }
    ProcessVehMasks();

    if (jcrunch == 1) {
        return 0xC8;
    }
    if (jcrunch == 2) {
        return 0xCB;
    }
    if (jcrunch != 3) {
        return (jcrunch != 4) ? 0xC7 : 0xC9;
    }
    return 0xCA;
}

extern s32 Paused;
void DrawVehMasks(void);

void DrawCRUNCHTIME(void)
{
    struct nuvec_s mid;
    s32 i;
    s32 a;
    u32 col;

    for (i = 0; i < 9; i++) {
        if ((u32)weather_attack_on[i] >= 0x32) {
            continue;
        }
        if (weather_attack_on[i] != 0 && Paused == 0) {
            a = weather_attack_on[i];
            if (a >= 0x23) {
                a = (0x32 - a) << 4;
            } else {
                a = a << 4;
            }
            if (a > 0x80) {
                a = 0x80;
            }
            col = a << 24;
            NuVecAdd(&mid, &D_005C8930[i], &D_005C89A0[i]);
            NuVecScale(&mid, &mid, 0.5f);
            NuLgtArcLaser(0, &D_005C8930[i], &D_005C89A0[i], &mid,
                          0.1f, 0.2f, 0.01f, 1.65f, col + 0xFF7F3F);
            NuLgtArcLaser(0, &D_005C8930[i], &D_005C89A0[i], &mid,
                          0.1f, 0.2f, 0.01f, 1.65f, col + 0xFF7F3F);
            NuLgtArcLaser(0, &D_005C8930[i], &D_005C89A0[i], &mid,
                          0.4f, 0.3f, 0.001f, 1.65f, col | 0x800040);
        }
    }
    i = CRemap[4];
    if (i != -1) {
        Draw3DCharacter(&D_006B7640, 0, 0, 0, &CModel[i],
                        1.0f, -1, 1.0f, 0);
    }
    DrawVehMasks();
}

void ResetCRUNCHTIME(void)
{
    s32 i;
    struct nuvec_s v;

    fire_attack_on = 0xFFFFFFFF;
    jcrunch = 0;
    D_00631B94 = 0;
    D_00633314 = 0;
    earth_attack_on = 0;
    D_00633318 = 0;
    water_attack_on = 0;
    D_0063331C = 0;
    D_00633320 = 0xFFFFFFFF;
    D_00633324 = 0;
    for (i = 8; i >= 0; i--) {
        weather_attack_on[i] = 0;
    }
    D_00633328 = 0;
    CrunchTime_Intro = 1;

    InitVehMasks();
    InitVehMask(0, 3);
    v = D_0061FFB0;
    SetNewMaskStuff(0, D_005C8870, &v, 0, 0,
                    0.0f, -540.0f, 1.0f, 1.0f, 0.0f);
}

extern s8 crunchtime_phase1tab[8];    /* 0x00631ba8 */
extern s8 crunchtime_phase2tab[8];    /* 0x00631bb0 */
extern s8 crunchtime_phase3tab[8];    /* 0x00631bb8 */
extern s8 crunchtime_phase4tab[8];    /* 0x00631bc0 */

s32 SpaceCrunchFunction_NewPoint(struct creature_s *c, struct nuvec_s *pos)
{
    s32 old;

    old = c->ai.i0;
    switch ((u32)D_00633314) {
    case 0:
        c->ai.i0 = crunchtime_phase1tab[1 - c->ai.count];
        break;
    case 1:
        c->ai.i0 = crunchtime_phase2tab[2 - c->ai.count];
        break;
    case 2:
        c->ai.i0 = crunchtime_phase3tab[4 - c->ai.count];
        break;
    case 3:
        c->ai.i0 = crunchtime_phase4tab[8 - c->ai.count];
        break;
    default:
        do {
            c->ai.i0 = qrand() / 0x4000 + 1;
        } while (c->ai.i0 == old);
        break;
    }
    c->ai.newpos = pos[c->ai.i0];
    if (old == 0 || c->ai.i0 == old) {
        c->obj.anim.newaction = 0x2C;
    } else if (old < c->ai.i0) {
        c->obj.anim.newaction = 0x32;
    } else {
        c->obj.anim.newaction = 0x39;
    }
    return 1;
}

s32 SpaceCrunchFunction_NewCount(struct creature_s *c)
{
    switch ((u32)D_00633314) {
    case 0:
    default:
        c->ai.count = 1;
        break;
    case 1:
        c->ai.count = 2;
        break;
    case 2:
        c->ai.count = 4;
        break;
    case 3:
        c->ai.count = 8;
        break;
    }
    return 1;
}


s32 SpaceCrunchFunction_AttackCortex(struct creature_s *c, struct nuvec_s *v)
{
    c->ai.i0 = 0;
    c->ai.newpos = *v;
    c->obj.anim.newaction = 0x2C;
    return 1;
}

s32 SpaceCortexFunction_TakeHit(struct creature_s *c)
{
    GameSfx(0x37, &c->obj.pos);
    player->obj.mom.x = 0.0f;
    player->obj.mom.z = 0.0f;
    return 1;
}

s32 SpaceCortexFunction_CheckPunch(void)
{
    return D_00633338;
}

s32 SpaceCrunchFunction_CheckCortexBack(void)
{
    return D_0063333C;
}

s32 SpaceCortexFunction_Defeated(void)
{
    boss_dead = (Game.hub[Hub].flags & 4) ? 2 : 1;
    SpaceCortex_Defeated = 1;
    return 1;
}

s32 SpaceCrunchFunction_PunchCortex(void)
{
    FindNearestCreature(&player->obj.pos, 2, 0);
    if (temp_creature_i != -1) {
        Character[temp_creature_i].obj.anim.newaction = 0x49;
    }
    D_00633338 = 1;
    return 1;
}

void ResetDRAINDAMAGE(void)
{
    D_00633300 = 0;
    DrainDamage_Intro = 1;
    D_00633308[0] = qrand();
    D_00633308[1] = qrand();
    D_00633308[2] = qrand();
}


f32 RatioDifferenceAlongLine(struct nuvec_s *a, struct nuvec_s *b, f32 p0, f32 p1)
{
    f32 r;
    f32 dx;
    f32 dz;
    f32 d;

    r = p1 - p0;
    dx = (b->x - a->x) * r;
    dz = (b->z - a->z) * r;
    d = NuFsqrt(dx * dx + dz * dz);
    if (r < 0.0f) {
        d = -d;
    }
    return d;
}

void LoadAI(void)
{
    void *file;
    s32 size;
    s32 count;
    s32 n;
    char name[16];
    s32 points;
    s32 type;
    s32 i;
    s32 slot;

    sprintf(tbuf, D_00631B88, LevelFileName);
    size = NuFileLoadBuffer(tbuf, Chase, 0x7FFFFFFF);
    file = NuMemFileOpen(Chase, size, 0);
    if (file == 0) {
        return;
    }

    count = NuFileReadInt(file);
    for (n = 0; n < count; n++) {
        for (i = 0; i < 16; i++) {
            name[i] = NuFileReadChar(file);
        }
        points = NuFileReadInt(file);
        type = FindAIType(name, points);
        for (i = 0; i < points; i++) {
            slot = (type != -1) ? i : 0;
            AITab[LEVELAICOUNT].pos[slot].x = NuFileReadFloat(file);
            AITab[LEVELAICOUNT].pos[slot].y = NuFileReadFloat(file);
            AITab[LEVELAICOUNT].pos[slot].z = NuFileReadFloat(file);
        }
        if (type != -1) {
            AITab[LEVELAICOUNT].ai_type = type;
            LEVELAICOUNT++;
        }
    }

    NuFileClose(file);
}


void SaveAI(void)
{
    void *file;
    s32 i;
    s32 j;
    u8 type;
    s32 count;

    sprintf(tbuf, D_00631B88, LevelFileName);
    file = NuFileOpen(tbuf, 1);
    if (file == 0) {
        return;
    }

    NuFileWriteInt(file, LEVELAICOUNT);
    for (i = 0; i < LEVELAICOUNT; i++) {
        type = AITab[i].ai_type;
        for (j = 0; j < 16; j++) {
            NuFileWriteChar(file, AIType[type].name[j]);
        }
        count = AIType[type].points;
        NuFileWriteInt(file, count);
        for (j = 0; j < count; j++) {
            NuFileWriteFloat(file, AITab[i].pos[j].x);
            NuFileWriteFloat(file, AITab[i].pos[j].y);
            NuFileWriteFloat(file, AITab[i].pos[j].z);
        }
    }

    NuFileClose(file);
}

/* Inline helpers: defined after every non-inlining caller (UpdateDRAINDAMAGE,
 * UpdateCRUNCHTIME, SpaceCrunchFunction_PunchCortex call FindNearestCreature
 * out of line in retail) and before MoveCreature, which inlines all four.
 * gcc 2.95 defers public inline functions to the end of the unit, which is
 * why their out-of-line copies sit after MoveCreature. */
inline f32 FindNearestCreature(struct nuvec_s *pos, s32 type, struct nuvec_s *out)
{
    struct creature_s *c;
    s32 i;
    s32 nearest;
    f32 best;

    nearest = -1;
    best = 1000000.0f;
    c = Character;
    for (i = 0; i < 9; i++, c++) {
        if (c->on == 0) {
            continue;
        }
        if (c->used == 0) {
            continue;
        }
        if (c->obj.character != type) {
            continue;
        }
        if (pos != &c->obj.pos) {
            f32 d = NuVecDistSqr(pos, &c->obj.pos, 0);
            if (d < best) {
                best = d;
                nearest = i;
                if (out != 0) {
                    *out = c->obj.pos;
                }
            }
        }
    }
    temp_creature_i = nearest;
    return best;
}

inline s32 FindAILabel(struct creatcmd_s *cmd, s32 label)
{
    s32 i;

    i = 0;
loop:
    if (cmd->cmd == 0x90) {
        return 0;
    }
    if (cmd->cmd == 0x8B) {
        if (cmd->i == label) {
            return i;
        }
    }
    cmd++;
    i++;
    goto loop;
}

inline s32 PlayerLateralInRange(struct nuvec_s *line, struct nuvec_s *a, struct nuvec_s *b, f32 p0, f32 p1)
{
    f32 ratio;
    f32 dx;
    f32 dz;
    f32 d;
    double dist;

    ratio = RatioAlongLine(line, a, b) - p0;
    dx = (b->x - a->x) * ratio;
    dz = (b->z - a->z) * ratio;
    d = NuFsqrt(dx * dx + dz * dz);
    if (ratio < 0.0f) {
        d = -d;
    }
    dist = d;
    if (dist < 0.0) {
        dist = -dist;
    }
    return dist < p1;
}


inline s32 PlayerLateralOutOfRange(struct nuvec_s *line, struct nuvec_s *a, struct nuvec_s *b, f32 p0, f32 p1)
{
    f32 ratio;
    f32 dx;
    f32 dz;
    f32 d;
    double dist;

    ratio = RatioAlongLine(line, a, b) - p0;
    dx = (b->x - a->x) * ratio;
    dz = (b->z - a->z) * ratio;
    d = NuFsqrt(dx * dx + dz * dz);
    if (ratio < 0.0f) {
        d = -d;
    }
    dist = d;
    if (dist < 0.0) {
        dist = -dist;
    }
    return dist > p1;
}

extern struct nuvec_s v000;
extern struct nuvec_s ai_lookpos;
extern u16 temp_xrot;
extern u16 temp_zrot;
extern s32 gamesfx_pitch;
extern s32 DESTRUCTIBLECRATECOUNT;
extern struct panelcount_s plr_crates;
s32 HitCreatures(struct obj_s *obj, s32 a, s32 b);
s32 HitCrates(struct obj_s *obj, s32 destroy);
s32 HitItems(struct obj_s *obj);
s32 HitWumpa(struct obj_s *obj, s32 destroy);
void GetTopBot(struct creature_s *c);
void NewTopBot(struct obj_s *obj);
void OldTopBot(struct obj_s *obj);
void GetSurfaceInfo(struct obj_s *obj, s32 mode, f32 y);
void ObjectRotation(struct obj_s *obj, s32 mode, s32 vehicle);
s32 RotDiff(u16 a, u16 b);
u16 TurnRot(u16 a, u16 b, s32 rate);
void AddKaboom(s32 type, struct nuvec_s *pos, f32 radius);
void AddGameDebris(s32 type, struct nuvec_s *pos);
s32 GetDieAnim(struct obj_s *obj, s32 anim);
s32 KillGameObject(struct obj_s *obj, s32 anim);
void FlyGameObject(struct obj_s *obj, u16 yrot);
s32 ResetHGobjAnim(struct nuhspecial_s *obj);
s32 StartHGobjAnim(struct nuhspecial_s *obj);
void GameSfxLoop(s32 sfx, struct nuvec_s *pos);

typedef void (*aicmdfn_t)(struct creature_s *c, struct nuvec_s *pos);
typedef s32 (*aieventfn_t)(struct creature_s *c, struct nuvec_s *pos);

/* Per-type creature AI brain: steps the AITab command list (three jump
 * tables: jtbl_00620010 commands 0..0x8B, jtbl_00620240 the 0x7E test
 * conditions, jtbl_00620290 event kinds), then movement, rotation, terrain,
 * events and attack locators. FindNearestCreature, FindAILabel and the two
 * PlayerLateral tests are inlined.
 *
 * Faithful full-C near-match (state=asm): frame/reg_mask/freg_mask exact,
 * 3157/3838 instructions aligned (3759 by opcode). Blocked by nine SN-as
 * mtc1 hazard nops the decompals `as` drops (see the recorded blocker), then
 * by whole-function callee-saved numbering. Levers that mattered: the spill
 * slots 0x40..0x60 follow declaration order (oldhdg .. ai_type); `default`
 * is the first switch block; event cases are in source order 0,1,2,3,5,7,4,6;
 * the 0x8C label walk is a goto loop and 0x8D/0x8E a two-case switch; two
 * shared floats fa/fb land in $f20/$f21 (block-scoped temps push t to $f23);
 * one UpdateAnimPacket call with a ternary model. */
void MoveCreature(struct creature_s *c)
{
    struct nuvec_s v;
    struct nuvec_s v2;
    struct nuvec_s v3;
    struct nuvec_s mid;
    s32 oldhdg;
    u16 touchmask;
    s32 ptype;
    s32 ptarget;
    s32 evhit;
    s32 branch;
    s32 ok;
    s32 points;
    s32 ai_type;
    struct creature_s *plr;
    struct nuvec_s *pos;
    struct nuvec_s *p;
    struct nuvec_s *q;
    s32 hit;
    s32 result;
    s32 i;
    s32 j;
    s32 n;
    s32 back;
    u16 ang;
    f32 t;
    f32 fa;
    f32 fb;

    plr = player;
    GetTopBot(player);
    NewTopBot(&plr->obj);
    switch (c->obj.dead) {
    case 1:
        NuVecAdd(&c->obj.pos, &c->obj.pos, &c->obj.mom);
        if (HitCreatures(&c->obj, 1, 4) || HitCrates(&c->obj, 1) || HitItems(&c->obj)) {
            c->obj.die_time = c->obj.die_duration;
        } else {
            HitWumpa(&c->obj, 0);
        }
        break;
    case 0:
        mid.x = c->obj.pos.x;
        mid.y = c->obj.pos.y + (c->obj.bot + c->obj.top) * c->obj.SCALE * 0.5f;
        mid.z = c->obj.pos.z;
        c->obj.oldpos = c->obj.pos;
        c->obj.anim.oldaction = c->obj.anim.action;
        OldTopBot(&c->obj);
        pos = AITab[c->i_aitab].pos;
        ai_type = AITab[c->i_aitab].ai_type;
        points = AIType[ai_type].points;
        for (;;) {
            if (c->cmdcurr == 0) {
                c->cmdcurr = c->cmdtable;
            }
            ok = 1;
            switch (c->cmdcurr->cmd) {
            default:
                ok = 0;
                break;
            case 0x00:
                memset(&c->ai, 0, sizeof(struct AI_s));
                c->ai.scale = 1.0f;
                for (i = 0; i < 2; i++) {
                    c->ai.event[i] = -1;
                }
                if (c->obj.character == 0x3F) {
                    FindGongBongerAnim(pos, &c->ai.obj);
                }
                c->ai.locator_kill = -1;
                c->ai.kill = -1;
                c->ai.force_action = -1;
                c->ai.look_creature = -1;
                break;
            case 0x2B:
                GetALONG(&c->obj.pos, &c->obj.RPos, c->obj.RPos.iRAIL, c->obj.RPos.iALONG,
                         c->cmdcurr->i);
                break;
            case 0x85:
                ((aicmdfn_t)c->cmdcurr->i)(c, pos);
                break;
            case 0x01:
                c->ai.i0 = c->cmdcurr->i;
                c->obj.pos = pos[c->ai.i0];
                break;
            case 0x02:
                player->obj.pos = pos[c->cmdcurr->i];
                player->obj.oldpos = player->obj.pos;
                player->obj.mom = v000;
                c->obj.contact = 0;
                c->obj.kill_contact = 0;
                c->obj.touch = 0;
                break;
            case 0x05:
                c->ai.lateral = c->cmdcurr->f;
                c->obj.pos.x = pos[c->ai.i0].x + (pos[c->ai.i1].x - pos[c->ai.i0].x) * c->ai.lateral;
                c->obj.pos.y = pos[c->ai.i0].y + (pos[c->ai.i1].y - pos[c->ai.i0].y) * c->ai.lateral;
                c->obj.pos.z = pos[c->ai.i0].z + (pos[c->ai.i1].z - pos[c->ai.i0].z) * c->ai.lateral;
                break;
            case 0x06:
                c->ai.lateral = RatioAlongLine(&plr->obj.pos, &pos[c->ai.i0], &pos[c->ai.i1]);
                c->obj.pos.x = pos[c->ai.i0].x + (pos[c->ai.i1].x - pos[c->ai.i0].x) * c->ai.lateral;
                c->obj.pos.y = pos[c->ai.i0].y + (pos[c->ai.i1].y - pos[c->ai.i0].y) * c->ai.lateral;
                c->obj.pos.z = pos[c->ai.i0].z + (pos[c->ai.i1].z - pos[c->ai.i0].z) * c->ai.lateral;
                break;
            case 0x03:
                if (c->obj.flags & 0x10000) {
                    if (c->obj.roof_y != 2000000.0f) {
                        c->obj.pos.y = c->obj.roof_y - c->obj.top * c->obj.SCALE;
                    }
                } else if (c->obj.shadow != 2000000.0f) {
                    c->obj.pos.y = c->obj.shadow - c->obj.bot * c->obj.SCALE;
                }
                break;
            case 0x04:
                c->obj.pos.y += c->cmdcurr->f;
                break;
            case 0x07:
                i = c->cmdcurr->i >> 8;
                j = c->cmdcurr->i & 0xFF;
                c->obj.pos.x = pos[i].x + (pos[j].x - pos[i].x) * c->cmdcurr->f;
                c->obj.pos.y = pos[i].y + (pos[j].y - pos[i].y) * c->cmdcurr->f;
                c->obj.pos.z = pos[i].z + (pos[j].z - pos[i].z) * c->cmdcurr->f;
                break;
            case 0x08:
                c->ai.i0 = points - 1;
                c->obj.pos = pos[c->ai.i0];
                break;
            case 0x09:
                c->ai.newpos.x = pos[c->cmdcurr->i].x;
                c->ai.newpos.y = pos[c->cmdcurr->i].y;
                c->ai.newpos.z = pos[c->cmdcurr->i].z;
                break;
            case 0x0D:
                c->ai.lateral = c->cmdcurr->f;
                c->ai.newpos.x = pos[c->ai.i0].x + (pos[c->ai.i1].x - pos[c->ai.i0].x) * c->ai.lateral;
                c->ai.newpos.y = pos[c->ai.i0].y + (pos[c->ai.i1].y - pos[c->ai.i0].y) * c->ai.lateral;
                c->ai.newpos.z = pos[c->ai.i0].z + (pos[c->ai.i1].z - pos[c->ai.i0].z) * c->ai.lateral;
                break;
            case 0x0E:
                c->ai.lateral = RatioAlongLine(&plr->obj.pos, &pos[c->ai.i0], &pos[c->ai.i1]);
                c->ai.newpos.x = pos[c->ai.i0].x + (pos[c->ai.i1].x - pos[c->ai.i0].x) * c->ai.lateral;
                c->ai.newpos.y = pos[c->ai.i0].y + (pos[c->ai.i1].y - pos[c->ai.i0].y) * c->ai.lateral;
                c->ai.newpos.z = pos[c->ai.i0].z + (pos[c->ai.i1].z - pos[c->ai.i0].z) * c->ai.lateral;
                break;
            case 0x0A:
                c->ai.newpos = c->obj.pos;
                break;
            case 0x0B:
                if (c->obj.shadow != 2000000.0f) {
                    c->ai.newpos.y = c->obj.shadow;
                }
                break;
            case 0x0C:
                c->ai.newpos.y += c->cmdcurr->f;
                break;
            case 0x0F:
                i = c->cmdcurr->i >> 8;
                j = c->cmdcurr->i & 0xFF;
                c->ai.newpos.x = pos[i].x + (pos[j].x - pos[i].x) * c->cmdcurr->f;
                c->ai.newpos.y = pos[i].y + (pos[j].y - pos[i].y) * c->cmdcurr->f;
                c->ai.newpos.z = pos[i].z + (pos[j].z - pos[i].z) * c->cmdcurr->f;
                break;
            case 0x10:
                c->ai.i0++;
                if (c->ai.i0 == points) {
                    c->ai.i0 = 0;
                }
                c->ai.newpos = pos[c->ai.i0];
                break;
            case 0x11:
                c->ai.i0--;
                if (c->ai.i0 < 0) {
                    c->ai.i0 += points;
                }
                c->ai.newpos = pos[c->ai.i0];
                break;
            case 0x12:
                c->ai.i0 = points - 1;
                c->ai.newpos = pos[c->ai.i0];
                break;
            case 0x13:
            case 0x14:
                v.x = 0.0f;
                v.y = 0.0f;
                v.z = c->cmdcurr->f;
                if (c->cmdcurr->cmd == 0x13) {
                    v.z *= qrand() * 1.5259022e-05f;
                }
                NuVecRotateX(&v, &v, qrand());
                NuVecRotateY(&c->ai.newpos, &v, qrand());
                NuVecAdd(&c->ai.newpos, &c->ai.newpos, &c->ai.wobblepos);
                break;
            case 0x15:
                i = qrand() % points;
                c->ai.newpos.x = pos[i].x;
                c->ai.newpos.y = pos[i].y;
                c->ai.newpos.z = pos[i].z;
                break;
            case 0x21:
                c->obj.hdg = c->cmdcurr->i;
                break;
            case 0x22:
                c->obj.hdg = NuAtan2D(pos[c->cmdcurr->i].x - c->obj.pos.x,
                                      pos[c->cmdcurr->i].z - c->obj.pos.z);
                break;
            case 0x23:
            case 0x24:
            case 0x25:
            case 0x28:
                if (c->cmdcurr->cmd == 0x28) {
                    p = &ai_lookpos;
                } else if (c->cmdcurr->cmd == 0x25) {
                    p = &c->ai.newpos;
                } else if (c->cmdcurr->cmd == 0x24) {
                    p = &D_0058656C;
                } else {
                    p = &plr->obj.pos;
                }
                c->obj.hdg = NuAtan2D(p->x - c->obj.pos.x, p->z - c->obj.pos.z);
                break;
            case 0x27:
                if (FindNearestCreature(&c->obj.pos, c->ai.look_creature, &v) < 1000000.0f) {
                    c->obj.hdg = NuAtan2D(v.x - c->obj.pos.x, v.z - c->obj.pos.z);
                }
                break;
            case 0x26:
                fa = points;
                v = v000;
                for (i = 0; i < points; i++) {
                    NuVecAdd(&v, &v, &pos[i]);
                }
                NuVecScale(&v, &v, 1.0f / fa);
                c->obj.hdg = NuAtan2D(v.x - c->obj.pos.x, v.z - c->obj.pos.z);
                break;
            case 0x29:
                c->obj.hdg = qrand();
                break;
            case 0x2A:
                c->obj.hdg += c->cmdcurr->i;
                break;
            case 0x2D:
                c->ai.rotspeed = c->cmdcurr->i;
                break;
            case 0x2E:
                c->ai.rotspeed = (c->cmdcurr->i * qrand()) / 0x10000;
                break;
            case 0x32:
                if (qrand() < (u16)c->cmdcurr->i) {
                    c->ai.rotspeed = -c->ai.rotspeed;
                }
                break;
            case 0x2C:
                if (c->obj.RPos.iRAIL != -1) {
                    c->obj.hdg = c->obj.RPos.angle + c->cmdcurr->i;
                }
                break;
            case 0x2F:
                c->ai.wobblepos = c->obj.pos;
                break;
            case 0x30:
                c->ai.wobble_ang = qrand();
                c->ai.wobble_speed = c->cmdcurr->i;
                c->ai.wobble = c->cmdcurr->f;
                break;
            case 0x74:
            case 0x75:
            case 0x76:
            case 0x77:
                if (c->obj.dead != 0 || plr->obj.dead != 0) {
                    for (i = 0; i < 2; i++) {
                        c->ai.event[i] = -1;
                    }
                } else {
                    if (c->cmdcurr->cmd == 0x76 || c->cmdcurr->cmd == 0x77) {
                        j = c->cmdcurr->cmd - 0x76;
                        c->ai.event[j] = 0x14;
                        c->ai.event_function[j] = (void *)c->cmdcurr->i;
                    } else {
                        j = c->cmdcurr->cmd - 0x74;
                        c->ai.event[j] = c->cmdcurr->i;
                        switch (c->ai.event[j]) {
                        case 0 ... 7:
                        case 10 ... 13:
                            c->ai.event_distance[j] = c->cmdcurr->f;
                            break;
                        }
                    }
                    c->ai.event_branch[j] = -1;
                    c->ai.event_flags[j] = 0;
                    c->ai.event_occured[j] = 0;
                }
                break;
            case 0x78:
            case 0x79:
                j = c->cmdcurr->cmd - 0x78;
                c->ai.event_branch[j] = FindAILabel(c->cmdtable, c->cmdcurr->i);
                break;
            case 0x7A:
            case 0x7B:
            case 0x7C:
            case 0x7D:
                c->ai.event_index[c->cmdcurr->cmd - 0x7A] = c->cmdcurr->i;
                break;
            case 0x4B:
                c->ai.oldangle = c->cmdcurr->i;
                c->ai.angle = c->cmdcurr->i;
                break;
            case 0x4C:
                i = qrand();
                c->ai.oldangle = i;
                c->ai.angle = i;
                break;
            case 0x53:
                c->ai.radius = c->cmdcurr->f;
                break;
            case 0x54:
                c->ai.height = c->cmdcurr->f;
                break;
            case 0x4D:
                c->ai.speed = c->cmdcurr->f;
                break;
            case 0x4E:
                c->ai.speed = qrand() * 1.5259022e-05f * c->cmdcurr->f;
                break;
            case 0x4F:
                c->ai.speed += c->cmdcurr->f;
                break;
            case 0x52:
                c->ai.distance = c->cmdcurr->f;
                break;
            case 0x50:
                c->ai.direction = c->cmdcurr->i;
                break;
            case 0x51:
                if (c->cmdcurr->i > 0) {
                    c->ai.direction = qrand() / (0xFFFF / c->cmdcurr->i + 1);
                } else {
                    c->ai.direction = 0;
                }
                break;
            case 0x5B:
                c->obj.invisible = 1;
                break;
            case 0x5C:
                c->obj.invisible = 0;
                break;
            case 0x5D:
                if (c->cmdcurr->i != 0) {
                    *(s32 *)c->cmdcurr->i = c->cmdcurr->f;
                }
                break;
            case 0x5E:
                c->ai.i0 = c->cmdcurr->i;
                break;
            case 0x5F:
                c->ai.i1 = c->cmdcurr->i;
                break;
            case 0x60:
                c->obj.flags |= c->cmdcurr->i;
                if (c->cmdcurr->i & 0x10000) {
                    GetTopBot(c);
                }
                break;
            case 0x61:
                c->obj.flags |= c->cmdcurr->i;
                c->obj.flags ^= c->cmdcurr->i;
                if (c->cmdcurr->i & 0x10000) {
                    GetTopBot(c);
                }
                break;
            case 0x86:
                touchmask = c->cmdcurr->i;
                break;
            case 0x31:
                c->ai.look_creature = c->cmdcurr->i;
                break;
            case 0x16:
                c->ai.count = c->cmdcurr->i;
                break;
            case 0x17:
                n = c->cmdcurr->i;
                if (n < 0) {
                    n = -n;
                }
                if (n > 0) {
                    c->ai.count = qrand() / (0xFFFF / (n + 1) + 1);
                } else {
                    c->ai.count = 0;
                }
                break;
            case 0x18:
                n = c->ai.count + c->cmdcurr->i;
                if (n < 0) {
                    n = 0;
                }
                c->ai.count = n;
                break;
            case 0x19:
                c->ai.count++;
                break;
            case 0x1A:
                if (c->ai.count != 0) {
                    c->ai.count--;
                }
                break;
            case 0x1B:
                c->ai.hits = c->cmdcurr->i;
                break;
            case 0x1C:
                n = c->cmdcurr->i;
                if (n < 0) {
                    n = -n;
                }
                if (n > 0) {
                    c->ai.hits = qrand() / (0xFFFF / (n + 1) + 1);
                } else {
                    c->ai.hits = 0;
                }
                break;
            case 0x1D:
                n = c->ai.hits + c->cmdcurr->i;
                if (n < 0) {
                    n = 0;
                }
                c->ai.hits = n;
                break;
            case 0x1E:
                c->ai.hits++;
                break;
            case 0x1F:
                if (c->ai.hits != 0) {
                    c->ai.hits--;
                }
                break;
            case 0x45:
                NewRumble(&player->rumble, c->cmdcurr->i);
                break;
            case 0x46:
                NewBuzz(&player->rumble, c->cmdcurr->i);
                break;
            case 0x55:
                c->ai.movflags = c->cmdcurr->i;
                if (c->cmdcurr->i & 0x800) {
                    c->ai.height = c->cmdcurr->f;
                }
                break;
            case 0x56:
                c->ai.rotflags = c->cmdcurr->i;
                if (c->ai.rotflags & 1) {
                    double dd = c->cmdcurr->f;
                    if (dd < 0.0) {
                        dd = 0.0 - dd;
                    }
                    c->ai.spins = dd;
                }
                break;
            case 0x33:
                c->ai.rotadjust = c->cmdcurr->i;
                break;
            case 0x57:
                c->ai.terflags = c->cmdcurr->i;
                break;
            case 0x34:
                c->obj.shadow = 2000000.0f;
                break;
            case 0x35:
                if (c->obj.flags & 0x10000) {
                    v.x = c->obj.pos.x;
                    v.y = c->obj.pos.y - 1.0f;
                    v.z = c->obj.pos.z;
                    c->obj.shadow = NewShadowMaskPlat(&v, 0.0f, -1);
                } else {
                    c->obj.shadow = NewShadowMaskPlat(&c->obj.pos, 0.0f, -1);
                }
                GetSurfaceInfo(&c->obj, 1, c->obj.shadow);
                ObjectRotation(&c->obj, 0, 0);
                c->obj.surface_xrot = temp_xrot;
                c->obj.surface_zrot = temp_zrot;
                break;
            case 0x36:
                if (c->obj.flags & 0x10000) {
                    c->obj.xrot = c->obj.roof_xrot;
                    c->obj.zrot = c->obj.roof_zrot;
                } else {
                    c->obj.xrot = c->obj.surface_xrot;
                    c->obj.zrot = c->obj.surface_zrot;
                }
                break;
            case 0x89:
                n = c->obj.character;
                ChangeCharacter(c, player->obj.character);
                ChangeCharacter(player, n);
                break;
            case 0x87:
                ChangeCharacter(c, c->cmdcurr->i);
                break;
            case 0x88:
                ChangeCharacter(player, c->cmdcurr->i);
                break;
            case 0x47:
                ResetAnimPacket(&c->obj.anim, c->cmdcurr->i);
                break;
            case 0x48:
                c->obj.anim.newaction = c->cmdcurr->i;
                break;
            case 0x59: {
                f32 f = c->cmdcurr->f;
                c->obj.anim.anim_time = f;
                if (f < 1.0f) {
                    c->obj.anim.anim_time = 1.0f;
                }
                break;
            }
            case 0x5A:
                if (c->obj.model->anmdata[c->obj.anim.newaction] != 0) {
                    c->obj.anim.anim_time = qrand() * 1.5259022e-05f
                        * (c->obj.model->anmdata[c->obj.anim.newaction]->time - 1.0f) + 1.0f;
                }
                break;
            case 0x49:
                c->ai.force_action = c->cmdcurr->i;
                break;
            case 0x4A:
                c->ai.idle_update = c->cmdcurr->i;
                break;
            case 0x3C:
                c->ai.duration = c->cmdcurr->f;
                break;
            case 0x3D:
                NuVecSub(&v, &c->ai.newpos, &c->ai.oldpos);
                c->ai.duration = NuVecMag(&v) / c->cmdcurr->f;
                if (c->ai.duration < 0.02f) {
                    c->ai.duration = 0.02f;
                }
                break;
            case 0x3E:
                if (c->obj.model->anmdata[c->obj.anim.newaction] != 0) {
                    c->ai.duration = (c->obj.model->anmdata[c->obj.anim.newaction]->time - 1.0f)
                        / (c->obj.model->animlist[c->obj.anim.newaction]->speed * 30.0f);
                } else {
                    c->ai.duration = 1.0f;
                }
                if (1.0f < c->cmdcurr->f) {
                    c->ai.duration *= c->cmdcurr->f;
                }
                break;
            case 0x40: {
                f32 end;
                if (c->obj.model->anmdata[c->obj.anim.newaction] == 0) {
                    c->ai.duration = 1.0f;
                    break;
                }
                end = c->obj.model->anmdata[c->obj.anim.newaction]->time;
                if (1.0f < c->cmdcurr->f && c->cmdcurr->f < end
                    && c->obj.anim.anim_time < c->cmdcurr->f) {
                    end = c->cmdcurr->f;
                }
                c->ai.duration = (end - c->obj.anim.anim_time)
                    / (c->obj.model->animlist[c->obj.anim.newaction]->speed * 30.0f);
                break;
            }
            case 0x3F:
                if (c->obj.model->anmdata[c->obj.anim.newaction] == 0) {
                    c->ai.duration = 1.0f;
                } else {
                    c->ai.duration = (c->cmdcurr->f - 1.0f)
                        / (c->obj.model->animlist[c->obj.anim.newaction]->speed * 30.0f);
                }
                break;
            case 0x41:
                c->ai.duration = 0.0f;
                break;
            case 0x42:
                c->ai.duration = qrand() * 1.5259022e-05f * c->cmdcurr->f;
                break;
            case 0x43:
                c->ai.duration += c->cmdcurr->f;
                break;
            case 0x44:
                if (c->ai.rotspeed != 0) {
                    n = RotDiff(c->ai.old_yrot, NuAtan2D(c->ai.newpos.x - c->obj.pos.x,
                                                         c->ai.newpos.z - c->obj.pos.z));
                    if ((c->ai.rotflags & 0x200) && n < 0) {
                        n += 0x10000;
                    } else if ((c->ai.rotflags & 0x400) && n > 0) {
                        n -= 0x10000;
                    }
                    if (n < 0) {
                        n = -n;
                    }
                    c->ai.duration = (f32)n / (f32)(c->ai.rotspeed * 50);
                } else {
                    c->ai.duration = 1.0f;
                }
                break;
            case 0x37:
                c->obj.attack = c->cmdcurr->i;
                break;
            case 0x38:
                c->obj.vulnerable = c->cmdcurr->i;
                if ((c->obj.flags & 4) && ai_type >= 0 && ai_type < 0x3E
                    && c->obj.character != 0x16) {
                    c->obj.vulnerable |= 0x180;
                }
                break;
            case 0x39:
                c->ai.kill = c->cmdcurr->i;
                break;
            case 0x3A:
                c->ai.locator_kill = c->cmdcurr->i;
                break;
            case 0x3B:
                c->ai.attack_locator_bits = c->cmdcurr->i;
                c->ai.attack_locator_radius = c->cmdcurr->f;
                break;
            case 0x6A:
                if (c->cmdcurr->i != -1) {
                    v = pos[c->cmdcurr->i];
                } else {
                    v = c->obj.pos;
                }
                AddKaboom(0x10, &v, c->cmdcurr->f);
                if (c->obj.character == 0x80) {
                    AddDeb3(&v, 9, 1, 0);
                    AddDeb3(&v, 9, 1, 0);
                    AddDeb3(&v, 9, 1, 0);
                    AddGameDebris(0xD, &v);
                    AddGameDebris(0xE, &v);
                }
                break;
            case 0x6C:
                JudderGameCamera(&GameCam, 0.3f, 0);
                break;
            case 0x71:
                if (KillPlayer(&plr->obj, GetDieAnim(&plr->obj, c->cmdcurr->i)) == 0) {
                    KillGameObject(&c->obj, 4);
                }
                break;
            case 0x72:
                plr->obj.mom.x = NuTrigTable[c->obj.hdg] * c->cmdcurr->f;
                plr->obj.mom.z = NuTrigTable[(c->obj.hdg + 0x4000) & 0xFFFF] * c->cmdcurr->f;
                break;
            case 0x6D:
                AddGameDebris(c->cmdcurr->i, &c->obj.pos);
                break;
            case 0x6E:
                AddGameDebris(c->cmdcurr->i, &mid);
                break;
            case 0x7E:
            case 0x7F:
            case 0x80:
            case 0x81:
            case 0x82:
            case 0x83:
            case 0x84:
                result = 0;
                if (c->cmdcurr->cmd == 0x7E) {
                    switch (c->cmdcurr->i) {
                    case 0:
                        if (NuVecDist(&plr->obj.pos, &c->obj.pos, 0) < c->cmdcurr->f) {
                            result = 1;
                        }
                        break;
                    case 1:
                        if (c->cmdcurr->f < NuVecDist(&plr->obj.pos, &c->obj.pos, 0)) {
                            result = 1;
                        }
                        break;
                    case 2:
                        if (NuVecXZDist(&plr->obj.pos, &c->obj.pos, 0) < c->cmdcurr->f) {
                            result = 1;
                        }
                        break;
                    case 3:
                        if (c->cmdcurr->f < NuVecXZDist(&plr->obj.pos, &c->obj.pos, 0)) {
                            result = 1;
                        }
                        break;
                    case 4:
                        result = PlayerLateralInRange(&plr->obj.pos, &pos[c->ai.i0], &pos[c->ai.i1],
                                                      c->ai.lateral, c->cmdcurr->f);
                        break;
                    case 5:
                        result = PlayerLateralOutOfRange(&plr->obj.pos, &pos[c->ai.i0],
                                                         &pos[c->ai.i1], c->ai.lateral,
                                                         c->cmdcurr->f);
                        break;
                    case 6:
                        result = (plr->obj.dead != 0);
                        break;
                    case 7:
                        result = (plr_crates.count >= DESTRUCTIBLECRATECOUNT);
                        break;
                    case 8:
                        result = (c->ai.i0 == (s32)c->cmdcurr->f);
                        break;
                    case 9:
                        result = (c->ai.i1 == (s32)c->cmdcurr->f);
                        break;
                    case 10:
                        result = (c->ai.i0 == points - 1);
                        break;
                    case 11:
                        result = (c->ai.i1 == points - 1);
                        break;
                    case 12:
                    case 13:
                        result = (c->ai.i0 == 0);
                        break;
                    case 14:
                        result = ((c->obj.touch & touchmask) != 0);
                        break;
                    }
                } else if (c->cmdcurr->cmd == 0x7F) {
                    result = (c->obj.anim.newaction == c->cmdcurr->i);
                } else if (c->cmdcurr->cmd == 0x80) {
                    result = (c->ai.i0 == c->cmdcurr->i);
                } else if (c->cmdcurr->cmd == 0x81) {
                    result = (c->ai.count == c->cmdcurr->i);
                } else if (c->cmdcurr->cmd == 0x82) {
                    result = (c->ai.hits == c->cmdcurr->i);
                } else if (c->cmdcurr->cmd == 0x83) {
                    if (c->cmdcurr->i != 0) {
                        result = (*(s32 *)c->cmdcurr->i == (s32)c->cmdcurr->f);
                    }
                } else if (c->cmdcurr->cmd == 0x84) {
                    result = (player->obj.character == c->cmdcurr->i);
                }
                c->cmdcurr++;
                switch (c->cmdcurr->cmd) {
                case 0x8D:
                    if (result) {
                        c->cmdcurr = &c->cmdtable[FindAILabel(c->cmdtable, c->cmdcurr->i)];
                    }
                    break;
                case 0x8E:
                    if (!result) {
                        c->cmdcurr = &c->cmdtable[FindAILabel(c->cmdtable, c->cmdcurr->i)];
                    }
                    break;
                }
                break;
            case 0x62:
                ptarget = -1;
                ptype = c->cmdcurr->i;
                break;
            case 0x63:
                ptarget = c->cmdcurr->i;
                break;
            case 0x6B:
                v = v000;
                n = 0;
                for (i = 0; i < 16; i++) {
                    if ((c->cmdcurr->i >> i) & 1) {
                        NuVecAdd(&v, &v, (struct nuvec_s *)&c->mtxLOCATOR[0][i]._30);
                        n++;
                    }
                }
                if (n != 0) {
                    if (n > 1) {
                        NuVecScale(&v, &v, 1.0f / n);
                    }
                    AddProjectile(&v, (ptarget != -1) ? &pos[ptarget] : 0,
                                  &c->momLOCATOR[0][i], ptype, c->obj.hdg, &c->obj);
                }
                break;
            case 0x65:
                gamesfx_effect_volume = c->cmdcurr->i;
                break;
            case 0x66:
                gamesfx_pitch = c->cmdcurr->i;
                break;
            case 0x67:
                GameSfx(c->cmdcurr->i, 0);
                break;
            case 0x68:
                GameSfx(c->cmdcurr->i, &c->obj.pos);
                break;
            case 0x69:
                gamesfx_effect_volume = c->cmdcurr->f;
                GameSfx(c->cmdcurr->i, &c->obj.pos);
                break;
            case 0x6F:
                ResetHGobjAnim(&c->ai.obj);
                break;
            case 0x70:
                StartHGobjAnim(&c->ai.obj);
                break;
            case 0x73:
                c->ai.scale = c->cmdcurr->f;
                break;
            case 0x8B:
                break;
            }
            if (ok) {
                c->ai.oldpos = c->obj.pos;
                c->ai.time = c->obj.mom.x = c->obj.mom.y = c->obj.mom.z = 0.0f;
                c->cmdcurr++;
                c->ai.oldpos = c->obj.pos;
                c->ai.old_yrot = c->obj.hdg;
                c->ai.oldangle = c->ai.angle;
                continue;
            }
            i = 0;
label8c:
            if (c->cmdcurr->cmd == 0x8C) {
                c->cmdcurr = &c->cmdtable[FindAILabel(c->cmdtable, c->cmdcurr->i)];
                i++;
                goto label8c;
            }
            if (i != 0) {
                continue;
            }

            evhit = 0;
            branch = -2;
            c->ai.time += 0.02f;
            if (0.0f < c->ai.duration && c->ai.duration <= c->ai.time) {
                c->ai.time = c->ai.duration;
                branch = -1;
                for (i = 0; i < 2; i++) {
                    if (c->ai.event[i] != -1 && c->ai.event_occured[i] != 0) {
                        branch = c->ai.event_branch[i];
                    }
                    c->ai.event[i] = -1;
                }
                evhit = 1;
            }
            t = 0.0f;
            if (0.0f < c->ai.duration) {
                t = c->ai.time / c->ai.duration;
            }

            c->ai.wobble_ang += c->ai.wobble_speed;
            if (c->ai.movflags & 0x40) {
                n = c->ai.i0 + 1;
                if (n == points) {
                    n = 0;
                }
                NuVecSub(&v, &pos[n], &pos[c->ai.i0]);
                v2.x = v.x;
                v.x = v.z + c->obj.pos.x;
                v.z = -v2.x + c->obj.pos.z;
                if ((player->obj.pos.x - c->obj.pos.x) * (v.z - c->obj.pos.z)
                    + (player->obj.pos.z - c->obj.pos.z) * (c->obj.pos.x - v.x) >= 0.0f) {
                    back = 0;
                    n = c->ai.i0 + 1;
                    if (n == points) {
                        n = 0;
                    }
                } else {
                    n = c->ai.i0;
                    back = 1;
                }
                q = &pos[n];
                {
                f32 dx = q->x - c->obj.pos.x;
                f32 dz = q->z - c->obj.pos.z;
                fa = dx * dx + dz * dz;
                ang = NuAtan2D(dx, dz);
                fb = c->ai.speed * 0.02f;
                if (fa <= fb * fb) {
                    c->obj.pos = *q;
                    if (!back) {
                        c->ai.i0 = n;
                    } else {
                        c->ai.i0--;
                        if (c->ai.i0 < 0) {
                            c->ai.i0 += points;
                        }
                    }
                } else {
                    c->obj.pos.x += NuTrigTable[ang] * fb;
                    c->obj.pos.z += NuTrigTable[(ang + 0x4000) & 0xFFFF] * fb;
                }
                }
            } else if (c->ai.movflags & 2) {
                if (c->ai.direction == 0) {
                    c->ai.angle += c->ai.rotspeed;
                } else {
                    c->ai.angle -= c->ai.rotspeed;
                }
                c->obj.pos.x = c->ai.newpos.x + NuTrigTable[c->ai.angle] * c->ai.radius;
                c->obj.pos.y = c->ai.newpos.y;
                c->obj.pos.z = c->ai.newpos.z
                    + NuTrigTable[(c->ai.angle + 0x4000) & 0xFFFF] * c->ai.radius;
            } else if (c->ai.movflags & 1) {
                if (c->ai.movflags & 0x30) {
                    f32 k;
                    if (c->ai.movflags & 0x10) {
                        k = 1.0f - NuTrigTable[(s32)(t * 16384.0f) & 0xFFFF];
                    } else {
                        k = NuTrigTable[(s32)(t * 16384.0f + 16384.0f) & 0xFFFF];
                    }
                    c->obj.pos.x = c->ai.newpos.x + (c->ai.oldpos.x - c->ai.newpos.x) * k;
                    c->obj.pos.y = c->ai.newpos.y + (c->ai.oldpos.y - c->ai.newpos.y) * k;
                    c->obj.pos.z = c->ai.newpos.z + (c->ai.oldpos.z - c->ai.newpos.z) * k;
                } else {
                    c->obj.pos.x = c->ai.oldpos.x + (c->ai.newpos.x - c->ai.oldpos.x) * t;
                    c->obj.pos.y = c->ai.oldpos.y + (c->ai.newpos.y - c->ai.oldpos.y) * t;
                    c->obj.pos.z = c->ai.oldpos.z + (c->ai.newpos.z - c->ai.oldpos.z) * t;
                    if (c->ai.movflags & 0x800) {
                        c->obj.pos.y += NuTrigTable[(s32)(t * 32768.0f) & 0xFFFF] * c->ai.height;
                    }
                }
            } else if (c->ai.movflags & 4) {
                f32 dx = pos[c->ai.i1].x - pos[c->ai.i0].x;
                f32 dz = pos[c->ai.i1].z - pos[c->ai.i0].z;
                double dd;
                fb = (c->ai.speed * 0.02f) / NuFsqrt(dx * dx + dz * dz);
                fa = RatioAlongLine(&plr->obj.pos, &pos[c->ai.i0], &pos[c->ai.i1]);
                dd = c->ai.lateral - fa;
                if (dd < 0.0) {
                    dd = 0.0 - dd;
                }
                if (dd > fb) {
                    if (fa < c->ai.lateral) {
                        c->obj.anim.newaction = 0x39;
                        c->ai.lateral -= fb;
                    } else if (c->ai.lateral < fa) {
                        c->obj.anim.newaction = 0x32;
                        c->ai.lateral += fb;
                    }
                    c->obj.pos.x = pos[c->ai.i0].x + (pos[c->ai.i1].x - pos[c->ai.i0].x) * c->ai.lateral;
                    c->obj.pos.y = pos[c->ai.i0].y + (pos[c->ai.i1].y - pos[c->ai.i0].y) * c->ai.lateral;
                    c->obj.pos.z = pos[c->ai.i0].z + (pos[c->ai.i1].z - pos[c->ai.i0].z) * c->ai.lateral;
                }
            } else if (c->ai.movflags & 8) {
                f32 lo = pos[c->ai.i0].y;
                f32 hi = pos[c->ai.i1].y;
                f32 target;
                f32 own;
                double dd;
                if (hi < lo) {
                    f32 tmp = hi;
                    hi = lo;
                    lo = tmp;
                }
                target = plr->obj.pos.y + (plr->obj.bot + plr->obj.top) * plr->obj.SCALE * 0.5f;
                own = c->obj.pos.y + (c->obj.bot + c->obj.top) * c->obj.SCALE * 0.5f;
                if (target < lo) {
                    target = lo;
                } else if (hi < target) {
                    target = hi;
                }
                fa = target - own;
                dd = fa;
                if (dd < 0.0) {
                    dd = 0.0 - dd;
                }
                if (dd > 0.2f) {
                    if (fa < 0.0f) {
                        c->obj.anim.newaction = 0x16;
                    } else {
                        c->obj.anim.newaction = 0x58;
                    }
                } else {
                    c->obj.anim.newaction = 0x22;
                }
                if (c->obj.anim.newaction != 0x22
                    && c->obj.model->anmdata[c->obj.anim.newaction] == 0) {
                    c->obj.anim.newaction = 0x22;
                }
                c->obj.pos.y += fa * 0.02f;
            } else if (c->ai.movflags & 0x400) {
                oldhdg = c->obj.hdg;
                c->ai.angle += c->ai.rotspeed;
                NuVecSub(&v, &pos[c->ai.i1], &pos[c->ai.i0]);
                c->obj.hdg = NuAtan2D(v.x, v.z);
                NuVecScale(&v, &v, NuTrigTable[c->ai.angle] * 0.5f + 0.5f);
                NuVecAdd(&c->obj.pos, &pos[c->ai.i0], &v);
                c->obj.hdg += c->ai.angle;
                if (c->ai.rotflags & 0x400) {
                    c->obj.hdg += 0x8000;
                }
                c->obj.dyrot = RotDiff(oldhdg, c->obj.hdg);
            }
            if (c->ai.movflags & 0x100) {
                c->ai.wobblepos = c->obj.pos;
            }
            if (c->ai.movflags & 0x200) {
                c->obj.pos.y = c->ai.wobblepos.y + NuTrigTable[c->ai.wobble_ang] * c->ai.wobble;
            }
            c->obj.mom.x = c->obj.pos.x - c->obj.oldpos.x;
            c->obj.mom.y = c->obj.pos.y - c->obj.oldpos.y;
            c->obj.mom.z = c->obj.pos.z - c->obj.oldpos.z;
            c->obj.xz_distance = NuFsqrt(c->obj.mom.x * c->obj.mom.x + c->obj.mom.z * c->obj.mom.z);
            c->obj.xyz_distance = NuFsqrt(c->obj.mom.x * c->obj.mom.x + c->obj.mom.y * c->obj.mom.y
                                          + c->obj.mom.z * c->obj.mom.z);

            if (!(c->ai.movflags & 0x400)) {
                oldhdg = c->obj.hdg;
            }
            if (c->ai.movflags & 2) {
                if (c->ai.direction == 0) {
                    c->obj.hdg = c->ai.angle + 0x4000;
                } else {
                    c->obj.hdg = c->ai.angle - 0x4000;
                }
            } else if (c->ai.rotflags & 1) {
                fb = c->ai.spins * 65536.0f;
                if (c->ai.rotflags & 0x80) {
                    fa = fb * (1.0f - NuTrigTable[(s32)(t * 16384.0f) & 0xFFFF]);
                    if (c->ai.rotflags & 0x400) {
                        c->obj.hdg = c->ai.old_yrot + (s32)(fa - fb);
                    } else {
                        c->obj.hdg = c->ai.old_yrot + (s32)(fb - fa);
                    }
                } else if (c->ai.rotflags & 0x100) {
                    fa = fb * NuTrigTable[(s32)(t * 16384.0f + 16384.0f) & 0xFFFF];
                    if (c->ai.rotflags & 0x400) {
                        c->obj.hdg = c->ai.old_yrot + (s32)(fa - fb);
                    } else {
                        c->obj.hdg = c->ai.old_yrot + (s32)(fb - fa);
                    }
                } else {
                    n = (s32)(fb * t);
                    if (c->ai.rotflags & 0x400) {
                        c->obj.hdg = c->ai.old_yrot - n;
                    } else {
                        c->obj.hdg = c->ai.old_yrot + n;
                    }
                }
            } else if (c->ai.rotflags & 2) {
                n = RotDiff(c->ai.old_yrot,
                            NuAtan2D(c->ai.newpos.x - c->ai.oldpos.x, c->ai.newpos.z - c->ai.oldpos.z)
                                + c->ai.rotadjust);
                if ((c->ai.rotflags & 0x200) && n < 0) {
                    n += 0x10000;
                } else if ((c->ai.rotflags & 0x400) && n > 0) {
                    n -= 0x10000;
                }
                {
                fb = n;
                if (c->ai.rotflags & 0x80) {
                    fa = fb * (1.0f - NuTrigTable[(s32)(t * 16384.0f) & 0xFFFF]);
                    if (c->ai.rotflags & 0x400) {
                        c->obj.hdg = c->ai.old_yrot + (s32)(fa - fb);
                    } else {
                        c->obj.hdg = c->ai.old_yrot + (s32)(fb - fa);
                    }
                } else if (c->ai.rotflags & 0x100) {
                    fa = fb * NuTrigTable[(s32)(t * 16384.0f + 16384.0f) & 0xFFFF];
                    if (c->ai.rotflags & 0x400) {
                        c->obj.hdg = c->ai.old_yrot + (s32)(fa - fb);
                    } else {
                        c->obj.hdg = c->ai.old_yrot + (s32)(fb - fa);
                    }
                } else {
                    c->obj.hdg = c->ai.old_yrot + (s32)(fb * t);
                }
                }
            } else if (c->ai.rotflags & 0x180C) {
                if (c->ai.rotflags & 0x800) {
                    if (!(FindNearestCreature(&c->obj.pos, c->ai.look_creature, &v) < 1000000.0f)) {
                        goto rotdone;
                    }
                    p = &v;
                } else if (c->ai.rotflags & 0x1000) {
                    p = &ai_lookpos;
                } else if (c->ai.rotflags & 8) {
                    p = &D_0058656C;
                } else {
                    p = &plr->obj.pos;
                }
                ang = NuAtan2D(p->x - c->obj.pos.x, p->z - c->obj.pos.z) + c->ai.rotadjust;
                if (c->ai.rotflags & 0x10) {
                    c->obj.hdg = SeekRot(c->obj.hdg, ang, 3);
                } else {
                    c->obj.hdg = TurnRot(c->obj.hdg, ang, c->ai.rotspeed);
                }
            } else if (c->ai.rotflags & 0x20) {
                if (0.0f < c->obj.xz_distance) {
                    ang = NuAtan2D(c->obj.mom.x, c->obj.mom.z) + c->ai.rotadjust;
                    n = RotDiff(c->obj.hdg, ang);
                    if (((n < 0) ? -n : n) <= c->ai.rotspeed) {
                        c->obj.hdg = ang;
                    } else if (n > 0) {
                        c->obj.hdg += c->ai.rotspeed;
                    } else if (n < 0) {
                        c->obj.hdg -= c->ai.rotspeed;
                    }
                }
            } else if (c->ai.rotflags & 0x40) {
                c->obj.hdg += c->ai.rotspeed;
            }
rotdone:
            if (!(c->ai.movflags & 0x400)) {
                c->obj.dyrot = RotDiff(oldhdg, c->obj.hdg);
            }

            c->obj.ground = 0;
            if (c->ai.terflags & 1) {
                if (c->ai.terflags & 2) {
                    c->obj.shadow = NewShadowMaskPlat(&c->obj.pos, 0.0f, -1);
                    GetSurfaceInfo(&c->obj, 1, c->obj.shadow);
                    if (c->obj.shadow != 2000000.0f) {
                        c->obj.ground = 3;
                        c->obj.pos.y = c->obj.shadow - c->obj.bot * c->obj.SCALE;
                    }
                } else if (c->ai.terflags & 4) {
                    c->obj.shadow = NewShadowMaskPlat(&c->obj.pos, 0.0f, -1);
                    GetSurfaceInfo(&c->obj, 1, c->obj.shadow);
                    if (c->obj.shadow != 2000000.0f) {
                        f32 y = c->obj.shadow - c->obj.bot * c->obj.SCALE;
                        if (c->obj.pos.y <= y) {
                            c->obj.pos.y = y;
                            c->obj.ground = 3;
                        }
                    }
                } else if (c->ai.terflags & 8) {
                    v.x = c->obj.pos.x;
                    v.y = c->obj.pos.y - 1.0f;
                    v.z = c->obj.pos.z;
                    c->obj.shadow = NewShadowMaskPlat(&v, 0.0f, -1);
                    GetSurfaceInfo(&c->obj, 1, c->obj.shadow);
                    if (c->obj.roof_type != -1) {
                        c->obj.ground = 0;
                        c->obj.pos.y = c->obj.roof_y - c->obj.top * c->obj.SCALE;
                    }
                } else if (c->ai.terflags & 0x10) {
                    c->obj.shadow = NewShadowMaskPlat(&c->obj.pos, 0.0f, -1);
                    GetSurfaceInfo(&c->obj, 1, c->obj.shadow);
                    if (c->obj.shadow != 2000000.0f) {
                        c->obj.ground = 0;
                        c->obj.pos.y = (c->obj.shadow - c->obj.bot * c->obj.SCALE) + c->ai.height;
                    }
                } else if (c->ai.terflags & 0x60) {
                    c->obj.shadow = NewShadowMaskPlat(&c->obj.pos, 0.0f, -1);
                    GetSurfaceInfo(&c->obj, 1, c->obj.shadow);
                    if (c->obj.shadow != 2000000.0f) {
                        f32 k = t;
                        if (c->ai.terflags & 0x40) {
                            k = 1.0f - t;
                        }
                        c->obj.ground = 0;
                        c->obj.pos.y = (c->obj.shadow - c->obj.bot * c->obj.SCALE) + c->ai.height * k;
                    }
                } else {
                    c->obj.shadow = NewShadowMaskPlat(&c->obj.pos, 0.0f, -1);
                    GetSurfaceInfo(&c->obj, 1, c->obj.shadow);
                }
                ObjectRotation(&c->obj, 2, 0);
            }
            c->obj.mom.y = c->obj.pos.y - c->obj.oldpos.y;

            if (!evhit) {
                hit = -1;
                for (i = 0; i < 2; i++) {
                    if (c->ai.event[i] == -1) {
                        continue;
                    }
                    switch (c->ai.event[i]) {
                    case 0:
                        if (plr->obj.dead == 0) {
                            v2.x = c->obj.pos.x;
                            v2.y = (c->obj.objtop + c->obj.objbot) * 0.5f;
                            v2.z = c->obj.pos.z;
                            v3.x = plr->obj.pos.x;
                            v3.y = (plr->obj.objtop + plr->obj.objbot) * 0.5f;
                            v3.z = plr->obj.pos.z;
                            NuVecSub(&v, &v3, &v2);
                            if (v.x * v.x + v.y * v.y + v.z * v.z
                                < c->ai.event_distance[i] * c->ai.event_distance[i]) {
                                hit = i;
                            }
                        }
                        break;
                    case 1:
                        if (plr->obj.dead == 0) {
                            f32 y;
                            v2.x = c->obj.pos.x;
                            y = c->obj.shadow;
                            if (y == 2000000.0f) {
                                y = c->obj.pos.y;
                            }
                            v2.y = y;
                            v2.z = c->obj.pos.z;
                            v3.x = plr->obj.pos.x;
                            v3.y = plr->obj.objbot;
                            v3.z = plr->obj.pos.z;
                            NuVecSub(&v, &v3, &v2);
                            if (v.x * v.x + v.y * v.y + v.z * v.z
                                < c->ai.event_distance[i] * c->ai.event_distance[i]) {
                                hit = i;
                            }
                        }
                        break;
                    case 2:
                        if (plr->obj.dead == 0
                            && PlayerLateralInRange(&plr->obj.pos, &pos[c->ai.i0], &pos[c->ai.i1],
                                                    c->ai.lateral, c->ai.event_distance[i])) {
                            hit = i;
                        }
                        break;
                    case 3:
                        if (plr->obj.dead == 0
                            && PlayerLateralOutOfRange(&plr->obj.pos, &pos[c->ai.i0], &pos[c->ai.i1],
                                                       c->ai.lateral, c->ai.event_distance[i])) {
                            hit = i;
                        }
                        break;
                    case 5:
                        if (plr->obj.dead == 0) {
                            v2.x = plr->obj.pos.x;
                            v2.y = (plr->obj.objtop + plr->obj.objbot) * 0.5f;
                            v2.z = plr->obj.pos.z;
                            v3 = pos[c->ai.event_index[i]];
                            NuVecSub(&v, &v3, &v2);
                            if (v.x * v.x + v.y * v.y + v.z * v.z
                                < c->ai.event_distance[i] * c->ai.event_distance[i]) {
                                hit = i;
                            }
                        }
                        break;
                    case 7:
                        if (plr->obj.dead == 0) {
                            v2.x = plr->obj.pos.x;
                            v2.y = (plr->obj.objtop + plr->obj.objbot) * 0.5f;
                            v2.z = plr->obj.pos.z;
                            v3 = pos[c->ai.event_index[i]];
                            NuVecSub(&v, &v3, &v2);
                            if (c->ai.event_distance[i] * c->ai.event_distance[i]
                                < v.x * v.x + v.y * v.y + v.z * v.z) {
                                hit = i;
                            }
                        }
                        break;
                    case 4:
                        if (plr->obj.dead == 0) {
                            f32 k = plr->obj.pos.x - pos[c->ai.event_index[i]].x;
                            f32 d = plr->obj.pos.z - pos[c->ai.event_index[i]].z;
                            if (k * k + d * d < c->ai.event_distance[i] * c->ai.event_distance[i]) {
                                hit = i;
                            }
                        }
                        break;
                    case 6:
                        if (plr->obj.dead == 0) {
                            f32 k = plr->obj.pos.x - pos[c->ai.event_index[i]].x;
                            f32 d = plr->obj.pos.z - pos[c->ai.event_index[i]].z;
                            if (c->ai.event_distance[i] * c->ai.event_distance[i] < k * k + d * d) {
                                hit = i;
                            }
                        }
                        break;
                    case 8:
                        if (plr->obj.dead == 0 && c->obj.contact == &plr->obj
                            && c->obj.contact_type == 2) {
                            hit = i;
                        }
                        break;
                    case 9:
                        if (plr->obj.dead != 0) {
                            hit = i;
                        }
                        break;
                    case 10:
                        if (plr->obj.dead == 0
                            && NuVecDist(&plr->obj.pos, &c->obj.pos, 0) < c->ai.event_distance[i]) {
                            hit = i;
                        }
                        break;
                    case 11:
                        if (plr->obj.dead == 0
                            && c->ai.event_distance[i] < NuVecDist(&plr->obj.pos, &c->obj.pos, 0)) {
                            hit = i;
                        }
                        break;
                    case 12:
                        if (plr->obj.dead == 0
                            && NuVecXZDist(&plr->obj.pos, &c->obj.pos, 0) < c->ai.event_distance[i]) {
                            hit = i;
                        }
                        break;
                    case 13:
                        if (plr->obj.dead == 0
                            && c->ai.event_distance[i] < NuVecXZDist(&plr->obj.pos, &c->obj.pos, 0)) {
                            hit = i;
                        }
                        break;
                    case 14:
                        if (plr->obj.dead == 0 && plr_crates.count >= DESTRUCTIBLECRATECOUNT) {
                            hit = i;
                        }
                        break;
                    case 16:
                        if (plr->obj.dead == 0 && c->obj.contact == &plr->obj) {
                            hit = i;
                        }
                        break;
                    case 18:
                        if (plr->obj.dead == 0 && c->obj.kill_contact != 0) {
                            hit = i;
                        }
                        break;
                    case 19:
                        if (c->hit_type == 2) {
                            hit = i;
                        }
                        break;
                    case 20:
                        if (((aieventfn_t)c->ai.event_function[i])(c, pos)) {
                            hit = i;
                        }
                        break;
                    }
                    if (hit != -1) {
                        if (c->ai.event_flags[i] & 1) {
                            c->ai.event_occured[i] = 1;
                        } else {
                            branch = c->ai.event_branch[i];
                        }
                        for (j = 0; j < 2; j++) {
                            c->ai.event[j] = -1;
                        }
                        break;
                    }
                }
            }
            if (branch == -2) {
                break;
            }
            c->ai.time = 0.0f;
            c->ai.oldpos = c->obj.pos;
            c->ai.old_yrot = c->obj.hdg;
            if (branch == -1) {
                c->cmdcurr++;
            } else {
                c->cmdcurr = &c->cmdtable[branch];
            }
        }

        if ((u32)(c->cmdcurr->cmd - 0x8F) < 2) {
            if (c->obj.dead != 0) {
                break;
            }
            if (c->cmdcurr->i == 0) {
                n = 1;
                if (c->obj.touch & 6) {
                    n = 4;
                }
            } else {
                n = c->cmdcurr->i;
            }
            if (n == 1) {
                FlyGameObject(&c->obj, NuAtan2D(c->obj.pos.x - plr->obj.pos.x,
                                                c->obj.pos.z - plr->obj.pos.z));
            }
            KillGameObject(&c->obj, n);
        }
        if (c->obj.dead != 0) {
            break;
        }
        if (plr->obj.dead == 0 && c->ai.attack_locator_bits != 0 && c->obj.draw_frame != 0) {
            for (i = 0; i < 16; i++) {
                if (!((c->ai.attack_locator_bits >> i) & 1)) {
                    continue;
                }
                if (c->obj.model->pLOCATOR[i] == 0) {
                    continue;
                }
                {
                f32 d = c->ai.attack_locator_radius * c->obj.SCALE;
                f32 k;
                f32 lo;
                f32 hi;
                v3.y = c->mtxLOCATOR[0][i]._31;
                if (v3.y + d < plr->obj.objbot) {
                    continue;
                }
                if (plr->obj.objtop < v3.y - d) {
                    continue;
                }
                v3.x = c->mtxLOCATOR[0][i]._30;
                v3.z = c->mtxLOCATOR[0][i]._32;
                k = v3.x - plr->obj.pos.x;
                lo = v3.z - plr->obj.pos.z;
                hi = plr->obj.RADIUS + d;
                fb = k * k + lo * lo;
                if (hi * hi < fb) {
                    continue;
                }
                v2.y = v3.y - c->momLOCATOR[0][i].y;
                if (c->obj.attack != 0) {
                    if (plr->spin != 0
                        && plr->spin_frame < plr->spin_frames - plr->OnFootMoveInfo->SPINRESETFRAMES) {
                        if (v2.y + d - 0.012f <= plr->obj.oldobjbot) {
                            plr->obj.mom.y = 0.0f;
                            plr->obj.boing |= 1;
                            NewRumble(&plr->rumble, 0x7F);
                            NewBuzz(&plr->rumble, 0xA);
                        } else if (plr->obj.oldobjtop <= v2.y - d + 0.012f) {
                            plr->obj.mom.y = 0.0f;
                        } else {
                            ang = NuAtan2D(k, lo);
                            plr->obj.mom.x = -NuTrigTable[ang] * 0.12f;
                            plr->obj.mom.z = -NuTrigTable[(ang + 0x4000) & 0xFFFF] * 0.12f;
                        }
                    } else {
                        if (KillPlayer(&plr->obj, (c->ai.locator_kill != -1)
                                                      ? c->ai.locator_kill
                                                      : GetDieAnim(&plr->obj, -1)) == 0) {
                            KillGameObject(&c->obj, 4);
                        }
                    }
                }
                break;
                }
            }
        }
        break;
    }

    if (c->obj.dead != 0) {
        if (c->obj.die_action != -1) {
            c->obj.anim.newaction = c->obj.die_action;
            fb = 0.59999996f;
        } else {
            fb = 0.0f;
        }
    } else {
        if (c->ai.force_action != -1) {
            c->obj.anim.newaction = c->ai.force_action;
        }
        fb = 0.59999996f;
    }
    if (c->ai.idle_update != 0) {
        UpdateCharacterIdle(c, c->obj.character);
    }
    UpdateAnimPacket((c->obj.dead != 0) ? &CModel[c->obj.die_model[0]] : c->obj.model,
                     &c->obj.anim, fb, c->obj.xz_distance);
    c->obj.frame++;
    switch (c->obj.character) {
    case 0xB1:
        gamesfx_pitch = 0x75A;
        gamesfx_effect_volume = 0x3FFF;
        GameSfxLoop(0x61, &c->obj.pos);
        break;
    case 0x9A:
        if (ai_type == 0x35 && 0.00666f < c->obj.xyz_distance) {
            switch (Level) {
            case 6:
                n = 0xCA;
                break;
            case 0x22:
                n = 0xC9;
                break;
            case 2:
                n = 0xCB;
                break;
            default:
                n = -1;
                break;
            }
            if (n != -1) {
                GameSfxLoop(n, &c->obj.pos);
            }
        }
        break;
    }
}
