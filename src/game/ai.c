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

#include "creature.h"

/* AI type entry (AIType): PS2 stride 0x1C; character (+0x0), label (+0x2),
 * name (+0x8) and the reset delay (+0x18) verified in FindAIType / ResetAI;
 * command table pointer (+0x4) verified in AddCreature. */
struct aitype_s {
    short character;         /* 0x00 */
    short label;             /* 0x02 (matched against the type arg; path count) */
    struct creatcmd_s *cmd;  /* 0x04 */
    char name[0x10];         /* 0x08 (ASCII label, strcmp target) */
    f32 delay;               /* 0x18 (initial AITab.delay) */
}; /* 0x1C */

/* AI table entry (AITab): PS2 stride 0x80; the RPos fields (+0x5/+0x6/+0x8)
 * and origin (+0x74 == pos[8]) verified in ResetAI. */
struct aitab_s {
    u8 ai_type;              /* 0x00 */
    s8 status;               /* 0x01 */
    u8 unk_0x02[3];          /* 0x02 */
    s8 iRAIL;                /* 0x05 */
    s16 iALONG;              /* 0x06 */
    f32 fALONG;              /* 0x08 */
    f32 time;                /* 0x0C */
    f32 delay;               /* 0x10 */
    struct nuvec_s pos[8];   /* 0x14 */
    struct nuvec_s origin;   /* 0x74 (== pos[8]) */
}; /* 0x80 */

/* Rail segment (Rail): PS2 stride 0x28; type byte at +0x26. */
struct rail_s {
    u8 unk_0x00[0x26];       /* 0x00 */
    s8 type;                 /* 0x26 */
    u8 unk_0x27;             /* 0x27 */
}; /* 0x28 */

/* Saved AI path positions (D_005CA154): stride 0x80 per AI entry, up to 10
 * nuvec sub-points (AIType[ai_type].label of them) written by SaveAI. */
struct aipath_s {
    struct nuvec_s pos[10];  /* 0x00 (0x78) */
    u8 pad[8];               /* 0x78 */
}; /* 0x80 */

extern struct aitype_s AIType[];       /* 0x005c9518 */
extern struct aitab_s AITab[];         /* 0x005c9558 */
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
void LoadAI(void);                     /* 0x0022ef10 */
void ResetAI(void);                    /* 0x0022f2c8 */
f32 NuVecDistSqr(struct nuvec_s *a, struct nuvec_s *b, struct nuvec_s *d); /* 0x001073a8 */
s32 strcmp(const char *a, const char *b);
void GetALONG(struct nuvec_s *pos, s32 a, s32 b, s32 c, s32 d); /* 0x001dac00 */
s32 AheadOfCheckpoint(s32 iRAIL, s32 iALONG, f32 fALONG); /* 0x001f0278 */
void NuVecAdd(struct nuvec_s *d, struct nuvec_s *a, struct nuvec_s *b); /* 0x00106f28 */
void NuVecScale(struct nuvec_s *d, struct nuvec_s *s, f32 k); /* 0x00106f98 */
void RemoveGameObject(struct obj_s *obj); /* 0x00200c38 */

extern struct aipath_s D_005CA154[];   /* saved AI path positions */
extern char tbuf[];                    /* 0x00592090 */
extern char LevelFileName[];           /* 0x0058b098 */
extern char D_00631B88[];              /* sprintf format for the .ai filename */
extern u8 Chase[];                     /* 0x005f3ed0 (scratch load buffer) */

s32 sprintf(char *buf, const char *fmt, ...);
void *NuFileOpen(char *name, s32 mode);            /* 0x001002d8 */
s32 NuFileLoadBuffer(char *name, void *buf, s32 max); /* 0x00100a88 */
void *NuMemFileOpen(void *buf, s32 size, s32 mode);   /* 0x00103430 */
void NuFileClose(void *f);                         /* 0x00102c20 */
f32 NuFileReadFloat(void *f);                      /* 0x00102e20 */
s32 NuFileReadInt(void *f);                        /* 0x00102e48 */
s32 NuFileReadChar(void *f);                       /* 0x00102ec0 */
void NuFileWriteFloat(void *f, f32 v);             /* 0x00102f08 */
void NuFileWriteInt(void *f, s32 v);               /* 0x00102f30 */
void NuFileWriteChar(void *f, s32 v);              /* 0x00102fa8 */
s32 qrand(void);
f32 NuFsqrt(f32 x);
f32 RatioAlongLine(void *line, struct nuvec_s *a, struct nuvec_s *b, f32 p0, f32 p1); /* 0x001f0730 */
u64 fptodp(f32 value);                 /* 0x002702e8 */
s32 dpcmp(u64 a, u64 b);               /* 0x0026f448 */
u64 dpsub(u64 a, u64 b);               /* 0x0026eeb8 */

extern s32 D_00633300;
extern s32 DrainDamage_Intro;
extern s16 D_00633308[3];
extern s32 D_00633338;
extern s32 D_0063333C;
extern s32 SpaceCortex_Defeated;
extern s32 boss_dead;
extern s32 Hub;
extern u8 Game[];
extern s32 D_00633314;
extern s32 gamesfx_effect_volume;
extern void *world_scene[32];
extern char D_0061FFD8[];
extern char D_0061FFE8[];
extern char D_0061FFF8[];
void *NuSpecialFind(void *scene, void *out, char *name);
void *memset(void *s, s32 c, u32 n);

extern u8 GDeb[];                      /* 0x0060dd00 (gdeb_s[], byte-accessed here) */
extern struct nuvec_s punchpos1[4];    /* 0x005c7e08 */
extern struct nuvec_s punchpos2[4];    /* 0x005c7e38 */
void AddFiniteShotDebrisEffect(s32 *key, void *effect, struct nuvec_s *pos, s32 n);
void DebrisEmitterOrientation(s32 key, s32 a, s32 hdg);

/* debris key record (debkeydata): PS2 stride 0x56C; a short at +0x4C2. */
struct debkey_s {
    u8 pad0[0x4C2];          /* 0x000 */
    s16 field_4C2;           /* 0x4C2 */
    u8 pad1[0xA8];           /* 0x4C4 */
}; /* 0x56C */
extern struct debkey_s debkeydata[]; /* 0x00320eb0 */

/* gong-bonger locator lookup (FindGongBongerAnim) */
struct gongnode_s {
    u8 pad[0x30];            /* 0x00 */
    struct nuvec_s pos;      /* 0x30 */
};
struct gongspecial_s {
    u8 pad[0x40];            /* 0x00 */
    struct gongnode_s *node; /* 0x40 */
};
struct gongbuf_s {
    void *anim;              /* 0x00 */
    struct gongspecial_s *special; /* 0x04 */
}; /* 0x8 */

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

f32 FindNearestCreature(struct nuvec_s *pos, s32 type, struct nuvec_s *out)
{
    struct creature_s *c;
    s32 i;
    s32 nearest;
    f32 best;

    nearest = -1;
    best = D_0062E30C;
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

s32 FindAIType(char *name, s32 type)
{
    s32 i;

    for (i = 0; i < 107; i++) {
        if (strcmp(name, AIType[i].name) == 0) {
            if (type == AIType[i].label) {
                if (type < 9) {
                    return i;
                }
            }
        }
    }
    return -1;
}

s32 FindAILabel(struct creatcmd_s *cmd, s32 label)
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

void ResetAI(void)
{
    s32 n;
    s32 flag;
    struct aitab_s *ai;
    struct creature_s *c;
    u8 irail;
    s32 rail_idx;
    s32 j;

    flag = 0;
    if (Rail[7].type == 3) {
        flag = AheadOfCheckpoint(gempath_RPos.iRAIL, gempath_RPos.iALONG,
                                 gempath_RPos.fALONG) != 0;
    }

    ai = AITab;
    for (n = 0; n < LEVELAICOUNT; n++, ai++) {
        GetALONG(&ai->pos[0], 0, -1, -1, 1);
        irail = temp_iRAIL;
        ai->iRAIL = irail;
        ai->iALONG = temp_iALONG;
        ai->fALONG = temp_fALONG;

        rail_idx = (s8)irail;
        if (rail_idx != -1 && Rail[rail_idx].type == 3) {
            if (flag) {
                ai->status = 1;
            }
        } else if (bonus_restart == 0) {
            if (AheadOfCheckpoint(ai->iRAIL, ai->iALONG, ai->fALONG)) {
                ai->status = 1;
            }
        }

        ai->time = 0;
        ai->delay = AIType[ai->ai_type].delay;
        ai->origin = ai->pos[0];

        if (AIType[ai->ai_type].label >= 2) {
            for (j = 1; j < AIType[ai->ai_type].label; j++) {
                NuVecAdd(&ai->origin, &ai->origin, &ai->pos[j]);
            }
            NuVecScale(&ai->origin, &ai->origin,
                       1.0f / (f32)AIType[ai->ai_type].label);
        }
    }

    for (c = &Character[1]; c < &Character[9]; c++) {
        if (c->used != 0) {
            RemoveGameObject(&c->obj);
            c->used = 0;
            c->on = 0;
        }
    }
}

void FindGongBongerAnim(struct nuvec_s *pos, struct gongbuf_s *out)
{
    struct gongbuf_s buf[3];
    s32 i;

    memset(buf, 0, sizeof(buf));
    if (world_scene[0] != 0) {
        NuSpecialFind(world_scene[0], &buf[0], D_0061FFD8);
        NuSpecialFind(world_scene[0], &buf[1], D_0061FFE8);
        NuSpecialFind(world_scene[0], &buf[2], D_0061FFF8);
        for (i = 0; i < 3; i++) {
            if (buf[i].special != 0) {
                struct nuvec_s *np = &buf[i].special->node->pos;
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
    s32 key;
    s32 amt;
    struct nuvec_s *pp;

    GameSfx(0xCB, &c->obj.pos);
    amt = 0x1E;
    pp = punchpos1;
    do {
        key = -1;
        AddFiniteShotDebrisEffect(&key, *(void **)(GDeb + 0x8D0), pp, 1);
        DebrisEmitterOrientation(key, 0, (s16)c->obj.hdg);
        debkeydata[key].field_4C2 = amt;
        amt -= 0xA;
        pp++;
    } while (pp != &punchpos1[4]);
    return 1;
}

s32 WaterCrunchFunction_Punch2(struct creature_s *c)
{
    s32 key;
    s32 amt;
    struct nuvec_s *pp;

    GameSfx(0xCB, &c->obj.pos);
    amt = 0x1E;
    pp = punchpos2;
    do {
        key = -1;
        AddFiniteShotDebrisEffect(&key, *(void **)(GDeb + 0x8D0), pp, 1);
        DebrisEmitterOrientation(key, 0, (s16)c->obj.hdg);
        debkeydata[key].field_4C2 = amt;
        amt -= 0xA;
        pp++;
    } while (pp != &punchpos2[4]);
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
    boss_dead = (Game[Hub * 4 + 0x10] & 4) ? 2 : 1;
    return 1;
}

s32 SpaceCrunchFunction_NewCount(struct creature_s *c)
{
    s32 v;
    u8 one = 1;

    if (D_00633314 == 1) {
        v = 2;
        goto ee4;
    }
    if (D_00633314 == 0) {
        goto edc;
    }
    if (D_00633314 == 2) {
        v = 4;
        goto ee4;
    }
    if (D_00633314 == 3) {
        v = 8;
        goto ee4;
    }
edc:
    c->ai.count = one;
    goto ee8;
ee4:
    c->ai.count = v;
ee8:
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
    SpaceCortex_Defeated = 1;
    boss_dead = (Game[Hub * 4 + 0x10] & 4) ? 2 : 1;
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

s32 PlayerLateralInRange(void *line, struct nuvec_s *a, struct nuvec_s *b, f32 p0, f32 p1)
{
    f32 ratio;
    f32 dx;
    f32 dz;
    f32 d;
    u64 dd;

    ratio = RatioAlongLine(line, a, b, p0, p1) - p0;
    dz = (b->z - a->z) * ratio;
    dx = (b->x - a->x) * ratio;
    d = NuFsqrt(dx * dx + dz * dz);
    if (ratio < 0.0f) {
        d = -d;
    }
    dd = fptodp(d);
    if (dpcmp(dd, 0) < 0) {
        dd = dpsub(0, dd);
    }
    return dpcmp(dd, fptodp(p1)) < 0;
}

s32 PlayerLateralOutOfRange(void *line, struct nuvec_s *a, struct nuvec_s *b, f32 p0, f32 p1)
{
    f32 ratio;
    f32 dx;
    f32 dz;
    f32 d;
    u64 dd;

    ratio = RatioAlongLine(line, a, b, p0, p1) - p0;
    dz = (b->z - a->z) * ratio;
    dx = (b->x - a->x) * ratio;
    d = NuFsqrt(dx * dx + dz * dz);
    if (ratio < 0.0f) {
        d = -d;
    }
    dd = fptodp(d);
    if (dpcmp(dd, 0) < 0) {
        dd = dpsub(0, dd);
    }
    return dpcmp(dd, fptodp(p1)) > 0;
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
    s32 e;
    char namebuf[16];
    s32 found;
    s32 subcount;
    s32 i;
    s32 k;
    s32 slot;

    sprintf(tbuf, D_00631B88, LevelFileName);
    size = NuFileLoadBuffer(tbuf, Chase, 0x7FFFFFFF);
    file = NuMemFileOpen(Chase, size, 0);
    if (file == 0) {
        return;
    }

    count = NuFileReadInt(file);
    if (count > 0) {
        for (e = 1; e <= count; e++) {
            for (i = 0; i < 16; i++) {
                namebuf[i] = NuFileReadChar(file);
            }
            subcount = NuFileReadInt(file);

            for (found = 0; found < 107; found++) {
                if (strcmp(namebuf, AIType[found].name) == 0) {
                    if (subcount == AIType[found].label) {
                        if (subcount < 9) {
                            goto found_it;
                        }
                    }
                }
            }
            found = -1;
        found_it:

            if (subcount > 0) {
                for (k = 0; k < subcount; k++) {
                    slot = found != -1 ? k : 0;
                    D_005CA154[LEVELAICOUNT].pos[slot].x = NuFileReadFloat(file);
                    D_005CA154[LEVELAICOUNT].pos[slot].y = NuFileReadFloat(file);
                    D_005CA154[LEVELAICOUNT].pos[slot].z = NuFileReadFloat(file);
                }
            }

            if (found != -1) {
                AITab[LEVELAICOUNT++].ai_type = found;
            }
        }
    }

    NuFileClose(file);
}

void SaveAI(void)
{
    void *file;
    s32 i;
    u8 type;
    s32 count;
    s32 j;
    char *cp;
    struct nuvec_s *p;

    sprintf(tbuf, D_00631B88, LevelFileName);
    file = NuFileOpen(tbuf, 1);
    if (file == 0) {
        return;
    }

    NuFileWriteInt(file, LEVELAICOUNT);
    for (i = 0; i < LEVELAICOUNT; i++) {
        type = AITab[i].ai_type;

        cp = AIType[type].name;
        for (j = 15; j >= 0; j--) {
            NuFileWriteChar(file, *cp);
            cp++;
        }

        count = AIType[type].label;
        NuFileWriteInt(file, count);
        if (count > 0) {
            p = D_005CA154[i].pos;
            do {
                NuFileWriteFloat(file, p->x);
                NuFileWriteFloat(file, p->y);
                NuFileWriteFloat(file, p->z);
                p++;
            } while (--count != 0);
        }
    }

    NuFileClose(file);
}
