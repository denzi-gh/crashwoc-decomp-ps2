/*
 * Unit: game/crate
 *
 * Functions:
 *   0x001f11c8 InitCrates
 *   0x001f1308 NewCrateAnimation
 *   0x001f1488 DrawCrates
 *   0x001f1cc8 FindLocalCrate
 *   0x001f1e00 ReadInCrateData
 *   0x001f21a8 HeightSortCrateData
 *   0x001f2548 ConvertCrateData
 *   0x001f2aa8 ReadCrateData
 *   0x001f2dc8 GetCrateType
 *   0x001f2eb8 AttackCrate
 *   0x001f3178 CrateOff
 *   0x001f3960 CrateBounceReaction
 *   0x001f3ed8 UpdateCrates
 *   0x001f4ee0 ResetCrates
 *   0x001f5bf0 AddQuad3DrotXYZ
 *   0x001f5fe8 AddCrateExplosion
 *   0x001f6418 UpdateCrateExplosions
 *   0x001f6678 DrawCrateExplosions
 *   0x001f6870 CrateTopBelow
 *   0x001f6aa8 CrateBottomAbove
 *   0x001f6ce0 InCrate
 *   0x001f6fc0 BreakCrate
 *   0x001f7310 HitCrateBalloons
 *   0x001f7490 RayIntersectCuboid
 *   0x001f77a0 CrateRayCast
 *   0x001f7aa0 CrateSafety
 *   0x001f7cc8 GotoCheckpoint
 *   0x001f7f20 InitCrateExplosions
 *   0x001f7f50 NextCrate
 *   0x001f7f78 ResetAllCrates
 *   0x001f7fb8 CloseCrates
 *   0x001f7fe0 AddCrate
 *   0x001f8090 DestroyCrate
 *   0x001f80b8 DrawCrate
 *   0x001f8158 OpenPreviousCheckpoints
 *   0x001f82a0 FindOverlap
 *   0x001f83d8 StartExclamationCrateSequence
 *   0x001f8458 DestroyAllNitroCrates
 *   0x001f85d8 HopCratesAbove
 *   0x001f8670 CrateAbove
 *   0x001f8790 CrateBelow
 *   0x001f88b0 LowestCrate
 *   0x001f8938 LowestActiveCrate
 *   0x001f89c8 CrateInTheWay
 *   0x001f8a40 CrateOnTop
 *   0x001f8b28 AddExtraLife
 *   0x001f8c38 HitCrates
 *   0x001f8cf8 WipeCrates
 *   0x001f8ea0 RayIntersectCylinder
 *   0x001f8fb8 SaveCrateTypeData
 *   0x001f9010 RestoreCrateTypeData
 *   0x001f9078 ResetAllCrateTypes
 *   0x001f9148 ResetCrateType2
 *   0x001f9168 CrateInSlot
 *   0x001f91d0 ResetInvisibility
 *   0x001f9208 ResetCrate
 */

#include "creature.h"

/* A crate slot (stride 0x90); only the fields the functions below touch are
 * typed.  The type bytes 0x3A..0x3F are unsigned in the struct and read
 * through (s8) casts (-1 = none), mirroring the retail codegen (lbu + sign
 * extension, lb where single-use). */
struct crate_s {
    u8 unk_0x00[0x10];       /* 0x00 (opaque) */
    struct nuvec_s pos;      /* 0x10 */
    u8 unk_0x1C[0x08];       /* 0x1C */
    f32 hop_mom;             /* 0x24 vertical hop start speed */
    u8 unk_0x28[0x08];       /* 0x28 */
    s8 on;                   /* 0x30 0 = inactive slot */
    u8 unk_0x31[0x09];       /* 0x31 */
    u8 type;                 /* 0x3A crate type (checkpoint save byte 0) */
    u8 tt_type;              /* 0x3B time-trial type (save byte 1) */
    u8 save2;                /* 0x3C (save byte 2) */
    u8 save3;                /* 0x3D (save byte 3) */
    u8 newtype;              /* 0x3E post-break/remap type; 0xF = exploded */
    u8 extra;                /* 0x3F secondary type override */
    u8 unk_0x40;             /* 0x40 */
    u8 metal_count;          /* 0x41 nonzero = destroyed by explosion */
    u8 unk_0x42[0x02];       /* 0x42 */
    s16 grid_x;              /* 0x44 stacking grid column */
    u8 unk_0x46[0x02];       /* 0x46 */
    s16 grid_z;              /* 0x48 stacking grid row */
    u8 unk_0x4A[0x2C];       /* 0x4A */
    s16 armed;               /* 0x76 -1 = idle; TNT/nitro trigger state */
    u8 unk_0x78[0x18];       /* 0x78 */
};                           /* 0x90 */

struct crategroup_s {
    u8 unk_0x00[0x10];       /* 0x00 (opaque) */
    s16 first;               /* 0x10 index of the group's first Crate slot */
    s16 count;               /* 0x12 number of slots in the group */
    u8 unk_0x14[0x1C];       /* 0x14 */
};                           /* 0x30 */

/* One checkpoint snapshot record: the crate and its four type bytes. */
struct cratesave_s {
    struct crate_s *crate;   /* 0x0 */
    u8 data[4];              /* 0x4 saved bytes 0x3A..0x3D */
};                           /* 0x8 */

/* Level data record; only the short at +0x26 is touched here. */
struct ldata_s {
    u8 unk_0x00[0x26];       /* 0x00 (opaque) */
    s16 unk_0x26;            /* 0x26 == 1 remaps type 2 -> 0x19 */
};

extern struct crate_s Crate[];
extern struct crategroup_s CrateGroup[];
extern s32 CRATEGROUPCOUNT;
extern struct cratesave_s CrateTypeData[];
extern s32 i_cratetypedata;
extern struct crategroup_s *temp_pGroup;
extern struct crate_s *temp_pCrate;
extern f32 CRATEHOPSPEED;
extern s32 TimeTrial;
extern struct ldata_s *LDATA;
extern u8 GameCam[];

s32 NewCrateAnimation(struct crate_s *crate, s32 type, s32 sfx, s32 d);
void GameSfx(s32 id, struct nuvec_s *pos);
void AddKaboom(s32 type, struct nuvec_s *pos, f32 f);
s32 CrateOff(struct crategroup_s *group, struct crate_s *crate, s32 a, s32 b);
void JudderGameCamera(void *cam, f32 mag, struct nuvec_s *pos);
s32 GetCrateType(struct crate_s *crate, s32 flags);

s32 GetCrateType(struct crate_s *crate, s32 flags)
{
    s32 type;

    if (TimeTrial != 0) {
        type = (s8)crate->tt_type;
    } else {
        type = (s8)crate->type;
    }
    if (type == 0 && (type = (s8)crate->newtype) == -1) {
        type = 0;
    }
    if ((flags & 0x2) && type == 8) {
        if ((s8)crate->newtype != -1) {
            goto not_m1;
        }
        /* a broken (0x3E == -1) bounce crate keeps reporting 8 */
    } else if ((s8)crate->newtype != -1) {
    not_m1:
        if (TimeTrial != 0) {
            type = (s8)crate->newtype;
        } else if ((s8)crate->extra != -1) {
            type = (s8)crate->extra;
        } else {
            type = (s8)crate->newtype;
        }
    } else if (TimeTrial == 0 || type == 9) {
        if ((s8)crate->extra != -1) {
            type = (s8)crate->extra;
        }
    }
    if ((flags & 0x1) == 0) {
        if ((u32)(type - 0x16) <= 2) {
            type = 9;
        }
    } else if (type == 2) {
        if (LDATA->unk_0x26 == 1) {
            type = 0x19;
        }
    }
    return type;
}

void BreakCrate(struct crategroup_s *group, struct crate_s *crate, s32 type,
                s32 flags)
{
    struct nuvec_s kpos;
    s32 g;
    s32 i;

    if (type == 0x13 && (flags & 0x200) == 0) {
        if (crate->armed == -1) {
            NewCrateAnimation(crate, 0x13, 0x58, 0);
            GameSfx(0x39, &crate->pos);
        }
        return;
    }
    if (type == 0xE) {
        if (crate->armed != -1) {
            return;
        }
        if (NewCrateAnimation(crate, 0xE, 0x35, 0) != 0) {
            return;
        }
        kpos.x = crate->pos.x;
        kpos.y = crate->pos.y + 0.25f;
        kpos.z = crate->pos.z;
        temp_pGroup = group;
        temp_pCrate = crate;
        AddKaboom(0x20, &kpos, 0.0f);
        crate->newtype = 0xF;
        crate->metal_count = 1;
        GameSfx(0x35, &temp_pCrate->pos);
        return;
    }
    if (type == 0x11) {
        struct crategroup_s *pg;
        struct crate_s *pc;

        if (crate->armed != -1) {
            return;
        }
        if (NewCrateAnimation(crate, 0x11, 0x35, 0) != 0) {
            return;
        }
        pg = CrateGroup;
        for (g = 0; g < CRATEGROUPCOUNT; g++, pg++) {
            pc = &Crate[pg->first];
            for (i = 0; i < pg->count; i++, pc++) {
                if (pc->on != 0) {
                    if (GetCrateType(pc, 0) == 0x10) {
                        CrateOff(pg, pc, 0, 0);
                    }
                }
            }
        }
        kpos.x = crate->pos.x;
        kpos.y = crate->pos.y + 0.25f;
        kpos.z = crate->pos.z;
        temp_pGroup = group;
        temp_pCrate = crate;
        AddKaboom(0x20, &kpos, 0.0f);
        crate->newtype = 0xF;
        crate->metal_count = 1;
        JudderGameCamera(GameCam, 0.5f, 0);
        GameSfx(0x33, &crate->pos);
        return;
    }
    if (CrateOff(group, crate, 0, (flags >> 9) & 0x1) != 0) {
        f32 hop = CRATEHOPSPEED;
        struct crate_s *pc;
        struct crate_s *cur;
        s32 j;

        cur = crate;
    restart:
        pc = &Crate[group->first];
        for (j = 0; j < group->count; j++, pc++) {
            if (pc->on == 0) {
                continue;
            }
            if (pc->grid_x != cur->grid_x) {
                continue;
            }
            if (pc->grid_z != cur->grid_z) {
                continue;
            }
            if (pc->pos.y == cur->pos.y + 0.5f) {
                pc->hop_mom = hop;
                cur = pc;
                goto restart;
            }
        }
    }
}

void SaveCrateTypeData(struct crate_s *crate)
{
    if (i_cratetypedata < 0x20) {
        struct cratesave_s *p = &CrateTypeData[i_cratetypedata];

        p->crate = crate;
        p->data[0] = crate->type;
        p->data[1] = crate->tt_type;
        p->data[2] = crate->save2;
        p->data[3] = crate->save3;
        i_cratetypedata++;
    }
}

void RestoreCrateTypeData(void)
{
    struct cratesave_s *p = CrateTypeData;
    s32 i;

    for (i = 0; i < i_cratetypedata; i++, p++) {
        p->crate->type = p->data[0];
        p->crate->tt_type = p->data[1];
        p->crate->save2 = p->data[2];
        p->crate->save3 = p->data[3];
    }
    i_cratetypedata = 0;
}
