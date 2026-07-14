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

struct nulsthdr_s;
struct nulnkhdr_s;

extern struct nulnkhdr_s *NuLstGetNext(struct nulsthdr_s *hdr,
                                       struct nulnkhdr_s *lnk);
extern void NuLstFree(struct nulnkhdr_s *lnk);
extern struct nulsthdr_s *NuLstCreate(int num, int size);
extern void NuLstDestroy(struct nulsthdr_s *hdr);

struct crate_s {
    s32 id;                  /* 0x00 */
    s8 type[4];              /* 0x04 */
    struct nuvec_s pos;      /* 0x08 */
    struct crate_s *linked;  /* 0x14 */
    struct crate_s *trigger; /* 0x18 */
    u16 orientation;         /* 0x1C */
    s16 offx;                /* 0x1E */
    s16 offy;                /* 0x20 */
    s16 offz;                /* 0x22 */
    s16 ccindex;             /* 0x24 */
    s8 draw;                 /* 0x26 */
    s8 cpad1;                /* 0x27 */
};

struct CrateCube {
    struct crate_s *model;   /* 0x00 */
    struct nuvec_s pos0;     /* 0x04 */
    struct nuvec_s pos;      /* 0x10 */
    f32 oldy;                /* 0x1C */
    f32 shadow;              /* 0x20 */
    f32 mom;                 /* 0x24 */
    f32 timer;               /* 0x28 */
    f32 duration;            /* 0x2C */
    s8 on;                   /* 0x30 */
    s8 iRAIL;                /* 0x31 */
    s16 iALONG;              /* 0x32 */
    f32 fALONG;              /* 0x34 */
    u16 flags;               /* 0x38 */
    s8 type1;                /* 0x3A */
    s8 type2;                /* 0x3B */
    s8 type3;                /* 0x3C */
    s8 type4;                /* 0x3D */
    s8 newtype;              /* 0x3E */
    s8 subtype;              /* 0x3F */
    s8 i;                    /* 0x40 */
    s8 metal_count;          /* 0x41 */
    s8 appeared;             /* 0x42 */
    s8 in_range;             /* 0x43 */
    s16 dx;                  /* 0x44 */
    s16 dy;                  /* 0x46 */
    s16 dz;                  /* 0x48 */
    s16 iU;                  /* 0x4A */
    s16 iD;                  /* 0x4C */
    s16 iN;                  /* 0x4E */
    s16 iS;                  /* 0x50 */
    s16 iE;                  /* 0x52 */
    s16 iW;                  /* 0x54 */
    s16 trigger;             /* 0x56 */
    s8 counter;              /* 0x58 */
    s8 anim_cycle;           /* 0x59 */
    s16 index;               /* 0x5A */
    f32 anim_time;           /* 0x5C */
    f32 anim_duration;       /* 0x60 */
    f32 anim_speed;          /* 0x64 */
    u16 xrot0;               /* 0x68 */
    u16 zrot0;               /* 0x6A */
    u16 xrot;                /* 0x6C */
    u16 zrot;                /* 0x6E */
    u16 surface_xrot;        /* 0x70 */
    u16 surface_zrot;        /* 0x72 */
    s16 character;           /* 0x74 */
    s16 action;              /* 0x76 */
    struct nuvec_s colbox[2];/* 0x78 */
};

struct CrateCubeGroup {
    struct nuvec_s origin;   /* 0x00 */
    f32 radius;              /* 0x0C */
    s16 iCrate;              /* 0x10 */
    s16 nCrates;             /* 0x12 */
    u16 angle;               /* 0x14 */
    s8 pad1;                 /* 0x16 */
    s8 pad2;                 /* 0x17 */
    struct nuvec_s minclip;  /* 0x18 */
    struct nuvec_s maxclip;  /* 0x24 */
};

struct CRATETYPEDATA {
    struct CrateCube *crate; /* 0x00 */
    s8 type1;                /* 0x04 */
    s8 type2;                /* 0x05 */
    s8 type3;                /* 0x06 */
    s8 type4;                /* 0x07 */
};

struct BoxExpType {
    s16 time;                /* 0x00 */
    s16 type;                /* 0x02 */
    u8 pad_04[0x76C - 0x04];
};

extern struct nulsthdr_s *D_006311F0;   /* static crates list */
#define crates D_006311F0
extern s32 num_crates_used;
extern s32 CRATECOUNT;
extern s32 CRATEGROUPCOUNT;
extern s32 iBOXEXP;
extern struct BoxExpType BoxExpList[];
extern struct CrateCube Crate[];
extern struct CRATETYPEDATA CrateTypeData[];
extern s32 i_cratetypedata;
extern s32 Level;
extern f32 plr_invisibility_time;
extern f32 WATERBOSSGLASSMIX;
extern f32 glass_mix;
extern s32 glass_col_mix;
extern s32 glass_enabled;
extern s32 glass_col_enabled;


void InitCrateExplosions(void) {
    s32 i;

    iBOXEXP = 0;
    for (i = 0; i < 0x10; i++) {
        BoxExpList[i].time = 0;
    }
}

struct crate_s *NextCrate(struct crate_s *a) {
    return (struct crate_s *)NuLstGetNext(crates, (struct nulnkhdr_s *)a);
}

void ResetAllCrates(void) {
    if (crates != 0) {
        NuLstDestroy(crates);
        crates = 0;
    }
    crates = NuLstCreate(0x100, 0x28);
    CRATEGROUPCOUNT = 0;
    CRATECOUNT = 0;
}

void CloseCrates(void) {
    if (crates != 0) {
        NuLstDestroy(crates);
        crates = 0;
    }
}

void DestroyCrate(struct crate_s *crate) {
    NuLstFree((struct nulnkhdr_s *)crate);
    num_crates_used--;
}

void ResetCrateType2(struct CrateCube *crt) {
    crt->type2 = crt->type1;
    if (crt->model == 0) {
        return;
    }
    crt->model->type[1] = crt->type1;
}

void SaveCrateTypeData(struct CrateCube *crate) {
    struct CRATETYPEDATA *data;

    if (i_cratetypedata > 0x1f) {
        return;
    }
    data = &CrateTypeData[i_cratetypedata];
    data->crate = crate;
    data->type1 = crate->type1;
    data->type2 = crate->type2;
    data->type3 = crate->type3;
    data->type4 = crate->type4;
    i_cratetypedata++;
}

void RestoreCrateTypeData(void) {
    s32 i;

    struct CRATETYPEDATA *data;

    data = CrateTypeData;
    for (i = 0; i < i_cratetypedata; i++, data++) {
        data->crate->type1 = data->type1;
        data->crate->type2 = data->type2;
        data->crate->type3 = data->type3;
        data->crate->type4 = data->type4;
    }
    i_cratetypedata = 0;
}

struct CrateCube *CrateInSlot(struct CrateCubeGroup *group, s32 x, s32 y,
                              s32 z) {
    struct CrateCube *crate;
    s32 i;

    crate = &Crate[group->iCrate];
    for (i = 0; i < group->nCrates; i++, crate++) {
        if (((crate->dx == x) && (crate->dy == y)) && (crate->dz == z)) {
            return crate;
        }
    }
    return 0;
}

void ResetInvisibility(void) {
    plr_invisibility_time = 5.0f;
    glass_mix = (Level != 0x17) ? 0.0f : WATERBOSSGLASSMIX;
    glass_col_mix = 0;
    glass_enabled = 0;
    glass_col_enabled = 0;
}

void ResetCrate(struct CrateCube *crt) {
    crt->oldy = crt->pos0.y;
    crt->pos.y = crt->pos0.y;
    crt->mom = 0.0f;
    crt->newtype = -1;
    crt->subtype = -1;
    if (((crt->type1 == 6) || (crt->type2 == 6)) ||
        ((crt->type1 == 0 && (crt->type3 == 6)))) {
        crt->counter = 0xa;
    } else {
        crt->counter = 0;
    }
    crt->metal_count = 0;
    crt->action = -1;
    crt->appeared = 0;
}
