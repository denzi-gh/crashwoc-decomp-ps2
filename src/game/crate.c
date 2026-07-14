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
