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
extern struct nulnkhdr_s *NuLstAlloc(struct nulsthdr_s *hdr);
extern void NuLstFree(struct nulnkhdr_s *lnk);
extern void *memset(void *s, s32 c, s32 n);
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

struct BoxPol_s {
    s32 ang[3];              /* 0x00 (integer rotation) */
    s32 angmom[3];           /* 0x0C */
    struct nuvec_s pos;      /* 0x18 (float position) */
    struct nuvec_s mom;      /* 0x24 (float velocity) */
    s32 rndfade;             /* 0x30 */
};                           /* 0x34 */
struct BoxExpType {
    s16 time;                /* 0x00 */
    s16 type;                /* 0x02 */
    struct nuvec_s colbox[2]; /* 0x04 */
    struct BoxPol_s BoxPol[30]; /* 0x1C */
    u8 pad_end[0x76C - (0x1C + 30 * 0x34)];
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
extern s32 GetCrateType(struct CrateCube *crt, s32 flags);
extern struct CrateCubeGroup *temp_pGroup;
extern struct CrateCube *temp_pCrate;
extern void AddKaboom(s32 type, struct nuvec_s *pos, f32 radius);
extern void GameSfx(s32 id, struct nuvec_s *pos);
extern s32 level_part_2;
extern s32 temp_crate_type;
extern struct CrateCube *InCrate(f32 x, f32 z, f32 top, f32 bot, f32 radius);
extern void BreakCrate(struct CrateCubeGroup *group, struct CrateCube *crate,
                       s32 type, s32 attack);
extern struct CrateCubeGroup CrateGroup[];
extern s32 FurtherALONG(s32 iRAIL, s32 iALONG, f32 fALONG, s32 iRAIL2,
                        s32 iALONG2, f32 fALONG2);
extern f32 CRATEBALLOONRADIUS;
extern f32 CRATEBALLOONOFFSET;
extern void NuVecSub(struct nuvec_s *dest, struct nuvec_s *a, struct nuvec_s *b);
extern s32 TimeTrial;

/* Per-level descriptor (same layout as game/creature.c's copy). */
struct ldata_s {
    u8 unk_0x00[0x24];       /* 0x00 (opaque) */
    u16 flags;               /* 0x24 */
    s16 crate_swap;          /* 0x26 */
    u16 vehicle;             /* 0x28 */
    u16 farclip;             /* 0x2A */
};
extern struct ldata_s *LDATA;

struct gamecam_s {
    u8 pad_00[0x80];
    struct nuvec_s vX;       /* 0x80 */
    u8 pad_8C[0x18];         /* 0x8C */
    struct nuvec_s pos;      /* 0xA4 */
};
extern struct gamecam_s GameCam;
extern f32 D_0062D7B4;                   /* 0.1f (gp-rel) */
extern u64 D_0061EFB8[];                  /* extra-life scale, double const (abs) */
extern void NuCameraTransformScreenClip(struct nuvec_s *out, struct nuvec_s *in,
                                        s32 a, void *b);
extern void AddPanelDebris(f32 x, f32 y, s32 obj, f32 z, s32 flag);
extern u64 fptodp(f32 x);
extern s32 dpcmp(u64 a, u64 b);
extern u64 dpsub(u64 a, u64 b);
extern u64 dpmul(u64 a, u64 b);
extern f32 dptofp(u64 x);

struct crateeditor_s {
    struct nuhspecial_s obj; /* 0x00 (0x8 bytes) */
    u8 pad_08[4];
    char *name;              /* 0x0C */
    s32 character;           /* 0x10 */
};
extern s8 CRemap[];
extern struct CharacterModel CModel[];
extern f32 D_0062D714;
extern s32 qrand(void);
extern f32 uvs[];
extern struct nuvec_s D_00592DE8[];
extern struct numtl_s *CrateMat;
extern f32 D_0062D798;
extern f32 D_0062D79C;

struct rndrstream_s {
    u8 *pad_00;              /* 0x00 */
    u8 *cur;                 /* 0x04 */
};
extern struct rndrstream_s *rndrstream_3d;
extern u8 *rndrstream_free;

extern s32 NuCameraClipTestPoints(struct nuvec_s *pos, s32 n, s32 mode);
extern struct numtx_s *NuCameraGetVPCSMtx(void);
extern void *NuVpGetCurrentViewport(void);
extern void NuMtxSetRotateXYZ(struct numtx_s *m, s32 *ang);
extern void NuMtxTranslate(struct numtx_s *m, struct nuvec_s *v);
extern void NuMtxMulH(struct numtx_s *dst, struct numtx_s *a,
                      struct numtx_s *b);
extern void NuVec4MtxTransformVU0(struct nuvec4_s *dst, struct nuvec4_s *src,
                                  struct numtx_s *m);
extern void NuVec4ScaleXYZVU0(struct nuvec4_s *dst, struct nuvec4_s *src,
                              f32 scale);
extern void NuRndrStreamLink(struct rndrstream_s *rs);
extern void NuVecConvertToIntVU0(void *dst, struct nuvec_s *src);
extern u8 *vpDmaTag_Cnt(u8 *cur);
extern u8 *vpDmaTag_Close(u8 *cur);
extern u8 *vpDmaTag_Next(u8 *cur, s32 mode);

extern void AddQuad3DrotXYZ(struct nuvec_s *pos, struct nuvec_s *shape,
                            struct numtl_s *mat, s32 *rot, f32 *uv, u32 col);
struct nuinstance_s {
    struct numtx_s matrix;   /* 0x00 */
    s32 objid;               /* 0x40 */
};

struct nuspecial_s {
    struct numtx_s mtx;            /* 0x00 */
    struct nuinstance_s *instance; /* 0x40 */
    char *name;                    /* 0x44 */
};

struct nugscn_s {
    short *tids;  /* 0x00 */
    s32 numtid;   /* 0x04 */
    void *mtls;   /* 0x08 */
    s32 nummtl;   /* 0x0C */
    s32 numgobj;  /* 0x10 */
    void **gobjs; /* 0x14 */
};

extern struct nugscn_s *crate_scene;
extern struct crateeditor_s crate_list[];
extern s32 NuSpecialFind(void *scene, struct nuhspecial_s *sp, char *name);
extern struct crate_s MarkerCrate;
extern struct crate_s LockCrate;
extern struct crate_s HighlightCrate;
extern struct crate_s FlashCrate;
extern struct crate_s *marker_crate;
extern struct crate_s *lock_crate;
extern struct crate_s *highlight_crate;
extern struct crate_s *flash_crate;
extern struct crate_s *locked_crate;
extern struct crate_s *triggerorigin_crate;
extern struct crate_s *triggerdest_crate;
extern struct crate_s *highlighted_crate;
extern u8 current_selected_crate;


/* Faithful near-match (state=asm): structure/offsets re-derived from retail.
 * Blocked on non-source-steerable codegen ties -- gcc's movz/movn direction
 * and beql/bnel choice and its basic-block ordering differ from retail SN
 * ee-gcc 2.95.2 (candidate asm hash frozen across ternary re-spellings).
 * 58.3% (140/240). Same class as the prior LIKELY_EQUIVALENT session. */
s32 GetCrateType(struct CrateCube *crt, s32 flags) {
    s32 type;
    s32 nt;

    type = (TimeTrial == 0) ? crt->type1 : crt->type2;

    if (type == 0) {
        nt = (s8)crt->newtype;
        type = (nt != -1) ? nt : 0;
    }

    if (flags & 2) {
        if (type == 8) {
            if ((s8)crt->newtype == -1) {
                goto flags1;
            }
            goto sel34;
        }
    }
    if ((s8)crt->newtype == -1) {
        goto sel58;
    }
sel34:
    if (TimeTrial != 0) {
        type = (s8)crt->newtype;
    } else {
        type = crt->subtype;
        if (crt->subtype == -1) {
            type = (s8)crt->newtype;
        }
    }
    goto flags1;
sel58:
    if (TimeTrial != 0) {
        if (type != 9) {
            goto flags1;
        }
    }
    if (crt->subtype != -1) {
        type = crt->subtype;
    }
flags1:
    if (flags & 1) {
        if (type == 2) {
            type = (LDATA->crate_swap == 1) ? 0x19 : type;
        }
    } else {
        type = ((u32)(type - 0x16) > 2) ? type : 9;
    }
    return type;
}

/* Faithful near-match (state=asm): call structure exact (2x
 * NuCameraTransformScreenClip, soft-double fabs via fptodp/dpcmp/dpsub, scale
 * by the D_0061EFB8 double, AddPanelDebris). Blocked on whole-fn regalloc:
 * retail keeps the u64 0 in a callee-saved reg (s1) across the dp calls
 * (frame 112 / mask s0,s1,s2); gcc here folds it to $zero (frame 96 / s0,s1).
 * Not source-steerable (hash frozen across an explicit-zero-local variant). */
void AddExtraLife(struct nuvec_s *pos, s32 pdeb) {
    struct nuvec_s scr;
    struct nuvec_s cV[2];
    u64 d;

    NuCameraTransformScreenClip(&scr, pos, 1, 0);
    cV[0].x = pos->x + GameCam.vX.x * D_0062D7B4;
    cV[0].y = pos->y + GameCam.vX.y * D_0062D7B4;
    cV[0].z = pos->z + GameCam.vX.z * D_0062D7B4;
    NuCameraTransformScreenClip(&cV[1], &cV[0], 1, 0);
    d = fptodp(scr.x - cV[1].x);
    if (dpcmp(d, 0) < 0) {
        d = dpsub(0, d);
    }
    AddPanelDebris(scr.x, scr.y, pdeb, dptofp(dpmul(d, D_0061EFB8[0])), 1);
}

/* Faithful near-match (state=asm): same +0.5f column-search assembler wall as
 * CrateOnTop/HopCratesAbove (gcc .p2align 3 inner-loop padding vs retail SN-as). */
s32 CrateAbove(struct obj_s *obj, struct CrateCubeGroup *group,
               struct CrateCube *crate) {
    struct CrateCube *crate2;
    s32 i;

    crate2 = &Crate[group->iCrate];
    for (i = 0; i < group->nCrates; i++, crate2++) {
        if ((((crate2 != crate) && (crate2->on != 0)) &&
             (GetCrateType(crate2, 0) != 0)) &&
            ((crate2->dx == crate->dx) && (crate2->dz == crate->dz))) {
            if ((crate2->pos.y > crate->pos.y) &&
                (!(obj->objtop < crate2->pos.y) &&
                 !(obj->objbot > (crate2->pos.y + 0.5f)))) {
                return 1;
            }
        }
    }
    return 0;
}

/* Faithful near-match (state=asm): same +0.5f column-search assembler wall. */
s32 CrateBelow(struct obj_s *obj, struct CrateCubeGroup *group,
               struct CrateCube *crate) {
    struct CrateCube *crate2;
    s32 i;

    crate2 = &Crate[group->iCrate];
    for (i = 0; i < group->nCrates; i++, crate2++) {
        if (((((crate2 != crate) && (crate2->on != 0)) &&
              (GetCrateType(crate2, 0) != 0)) &&
             ((crate2->dx == crate->dx) && (crate2->dz == crate->dz))) &&
            ((crate2->pos.y < crate->pos.y) &&
             (!(obj->objtop < crate2->pos.y ||
                (obj->objbot > (crate2->pos.y + 0.5f)))))) {
            return 1;
        }
    }
    return 0;
}

extern s32 FurtherBEHIND(s32 iRAIL, s32 iALONG, f32 fALONG, s32 iRAIL2,
                         s32 iALONG2, f32 fALONG2);
extern s32 NewCrateAnimation(struct CrateCube *crate, s32 character,
                             s32 action, s32 flag);
extern s32 CrateOff(struct CrateCubeGroup *group, struct CrateCube *crate,
                    s32 a, s32 b);
extern void JudderGameCamera(struct gamecam_s *cam, f32 time, void *pos);

void DestroyAllNitroCrates(struct CrateCubeGroup *group,
                           struct CrateCube *crate) {
    struct CrateCubeGroup *group2;
    struct CrateCube *crate2;
    struct nuvec_s pos;
    s32 i;
    s32 j;

    group2 = CrateGroup;
    for (i = 0; i < CRATEGROUPCOUNT; i++, group2++) {
        crate2 = &Crate[group2->iCrate];
        for (j = 0; j < group2->nCrates; j++, crate2++) {
            if ((crate2->on != 0) && (GetCrateType(crate2, 0) == 0x10)) {
                CrateOff(group2, crate2, 0, 0);
            }
        }
    }
    pos.x = crate->pos.x;
    pos.y = crate->pos.y + 0.25f;
    pos.z = crate->pos.z;
    temp_pGroup = group;
    temp_pCrate = crate;
    AddKaboom(0x20, &pos, 0.0f);
    crate->newtype = 0xf;
    crate->metal_count = 1;
    JudderGameCamera(&GameCam, 0.5f, 0);
    GameSfx(0x33, &crate->pos);
}

void OpenPreviousCheckpoints(s32 iRAIL, s32 iALONG, f32 fALONG) {
    struct CrateCubeGroup *group;
    struct CrateCube *crate;
    s32 i;
    s32 j;

    group = CrateGroup;
    for (i = 0; i < CRATEGROUPCOUNT; i++, group++) {
        crate = &Crate[group->iCrate];
        for (j = 0; j < group->nCrates; j++, crate++) {
            if ((((crate->on != 0) && (crate->type1 == 7)) &&
                 (crate->iRAIL != -1)) &&
                (FurtherBEHIND(crate->iRAIL, crate->iALONG, crate->fALONG,
                               iRAIL, iALONG, fALONG) != 0)) {
                crate->on = 0;
                if (crate->model != 0) {
                    crate->model->draw = 0;
                }
                NewCrateAnimation(crate, 7, 0x34, 0);
            }
        }
    }
}

/* Faithful near-match (state=asm), 90.6%: logic/structure/offsets exact.
 * Residual is a 9-word FP instruction-scheduling tie in the qrand anim_time
 * block -- retail loads anim_duration before D_0062D714 and computes (dur-1.0)
 * eagerly (f2/f3/f4/f0), gcc here loads scale first (f3 vs f2 etc). Not
 * source-steerable (an explicit dur local regressed to a callee-saved f21). */
struct numtl_s;

extern f32 temp_ratio;
extern s32 temp_face;
extern struct nuvec_s vTEMP;
extern s32 sprintf(char *buf, const char *fmt, ...);
extern char tbuf[];
extern char LevelFileName[];
extern char D_00631250[];
extern u8 Chase[];
extern s32 NuFileLoadBuffer(char *name, void *buf, s32 max);
extern s32 NuMemFileOpen(void *buf, s32 size, s32 mode);
extern s32 NuFileReadInt(s32 h);
extern short NuFileReadShort(s32 h);
extern f32 NuFileReadFloat(s32 h);
extern char NuFileReadChar(s32 h);
extern void NuFileClose(s32 h);

/* Faithful near-match (state=asm), 92.5%: logic/offsets/version-branches exact
 * (drops GC NuFileExists, D_00631250 format, NUFILE_READ=0). Residual is 15
 * words in the version-check + loop-setup region -- gcc emits slti/5 where
 * retail materializes 4 for slt, cascading the loop preheader. Not steerable. */
extern void HeightSortCrateData(void);

/* Faithful reconstruction (state=asm), ~4%: logic/offsets correct (reverses
 * ReadInCrateData; rebuild groups, neighbor detection iU/iD/iN/iS/iE/iW, trigger
 * convert). Codegen-divergent -- retail uses a `*pGroupCount++` pointer idiom and
 * a different loop/store layout; needs dedicated reconstruction. */
void ConvertCrateData(void) {
    struct crate_s *cr;
    struct crate_s *cr2;
    struct CrateCube *cc;
    struct CrateCubeGroup *crg;
    s32 i;
    s32 j;
    s32 k;
    struct nuvec_s tvec;
    s32 ddx;
    s32 ddy;
    s32 ddz;

    CRATECOUNT = 0;
    CRATEGROUPCOUNT = 0;
    cr2 = NextCrate(0);
    if (cr2 != 0) {
        do {
            if (cr2->linked == cr2) {
                tvec.z = -0.25f;
                tvec.y = -0.25f;
                tvec.x = -0.25f;
                crg = &CrateGroup[CRATEGROUPCOUNT++];
                NuVecRotateY(&tvec, &tvec, (s32)cr2->orientation);
                crg->origin.x = cr2->pos.x + tvec.x;
                crg->origin.y = cr2->pos.y + tvec.y;
                crg->origin.z = cr2->pos.z + tvec.z;
                crg->radius = 0.0f;
                crg->iCrate = (u16)CRATECOUNT;
                cr2->ccindex = crg->iCrate;
                crg->nCrates = 1;
                crg->angle = cr2->orientation;
                cr = NextCrate(0);
                crg->maxclip = cr2->pos;
                crg->minclip = crg->maxclip;
                CRATECOUNT++;
                if (cr != 0) {
                    do {
                        if (cr->linked == cr2) {
                            if (cr2 == cr) {
                                cc = &Crate[(s32)crg->iCrate];
                            } else {
                                cc = &Crate[CRATECOUNT];
                                cr->ccindex = CRATECOUNT++;
                                crg->nCrates++;
                            }
                            if (cr->pos.x < crg->minclip.x)
                                crg->minclip.x = cr->pos.x;
                            if (cr->pos.y < crg->minclip.y)
                                crg->minclip.y = cr->pos.y;
                            if (cr->pos.z < crg->minclip.z)
                                crg->minclip.z = cr->pos.z;
                            if (cr->pos.x > crg->maxclip.x)
                                crg->maxclip.x = cr->pos.x;
                            if (cr->pos.y > crg->maxclip.y)
                                crg->maxclip.y = cr->pos.y;
                            if (cr->pos.z > crg->maxclip.z)
                                crg->maxclip.z = cr->pos.z;
                            cc->pos0.x = cr->pos.x + tvec.x;
                            cc->pos0.y = cr->pos.y + tvec.y;
                            cc->pos0.z = cr->pos.z + tvec.z;
                            cc->shadow = 0.0f;
                            cc->dx = cr->offx;
                            cc->dy = cr->offy;
                            cc->dz = cr->offz;
                            cc->type1 = cr->type[0];
                            cc->type2 = cr->type[1];
                            cc->type3 = cr->type[2];
                            cc->type4 = cr->type[3];
                            cc->trigger = -1;
                            cc->on = 1;
                            cc->timer = 0.0f;
                            cc->iU = -1;
                            cc->iD = -1;
                            cc->iN = -1;
                            cc->iS = -1;
                            cc->iE = -1;
                            cc->iW = -1;
                            cc->model = cr;
                        }
                        cr = NextCrate(cr);
                    } while (cr != 0);
                }
                crg->minclip.x -= 0.5f;
                crg->minclip.y -= 0.5f;
                crg->minclip.z -= 0.5f;
                crg->maxclip.x += 0.5f;
                crg->maxclip.y += 0.5f;
                crg->maxclip.z += 0.5f;
            }
            cr2 = NextCrate(cr2);
        } while (cr2 != 0);
    }
    for (i = 0; i < CRATEGROUPCOUNT; i++) {
        crg = &CrateGroup[i];
        for (j = crg->iCrate; j < crg->iCrate + crg->nCrates; j++) {
            for (k = crg->iCrate; k < crg->iCrate + crg->nCrates; k++) {
                if (j != k) {
                    cc = &Crate[k];
                    ddx = cc->dx - Crate[j].dx;
                    ddy = cc->dy - Crate[j].dy;
                    ddz = cc->dz - Crate[j].dz;
                    if (ddx == 1 && ddy == 0 && ddz == 0) {
                        Crate[j].iE = k;
                    }
                    if (ddx == -1 && ddy == 0 && ddz == 0) {
                        Crate[j].iW = k;
                    }
                    if (ddx == 0) {
                        if (ddy == 1 && ddz == 0) {
                            Crate[j].iU = k;
                        }
                    }
                    if (ddx == 0) {
                        if (ddy == -1 && ddz == 0) {
                            Crate[j].iD = k;
                        }
                    }
                    if (ddx == 0) {
                        if (ddy == 0 && ddz == 1) {
                            Crate[j].iN = k;
                        }
                    }
                    if (ddx == 0 && ddy == 0 && ddz == -1) {
                        Crate[j].iS = k;
                    }
                }
            }
        }
    }
    for (cr2 = NextCrate(0); cr2 != 0; cr2 = NextCrate(cr2)) {
        if (cr2->trigger != 0) {
            Crate[cr2->ccindex].trigger = cr2->trigger->ccindex;
        }
    }
    HeightSortCrateData();
}

/* Faithful near-match (state=asm), 77.8%: main body exact (crate_s build,
 * clip min/max, offx/y/z, cc->model). Residual is the goto trigger-linking tail
 * loop's control-flow layout vs retail. */
void ReadInCrateData(void) {
    s32 i;
    s32 j;
    struct crate_s *new_crate;
    struct CrateCubeGroup *crg;
    struct CrateCube *cc;
    struct crate_s *first_crate;
    struct nuvec_s tvec;
    struct crate_s *cr;
    struct crate_s *cr2;

    HeightSortCrateData();
    num_crates_used = 0;
    for (i = 0; i < CRATEGROUPCOUNT; i++) {
        tvec.z = 0.25f;
        tvec.y = 0.25f;
        tvec.x = 0.25f;
        crg = &CrateGroup[i];
        NuVecRotateY(&tvec, &tvec, (s32)crg->angle);
        for (j = 0; j < crg->nCrates; j++) {
            cc = &Crate[crg->iCrate + j];
            cr = (struct crate_s *)NuLstAlloc(crates);
            if (cr != 0) {
                cr->id = cc->type1;
                cr->pos.x = cc->pos0.x + tvec.x;
                cr->pos.y = cc->pos0.y + tvec.y;
                cr->pos.z = cc->pos0.z + tvec.z;
                if (j == 0) {
                    crg->minclip = crg->maxclip = cr->pos;
                    first_crate = cr;
                } else {
                    if (cr->pos.x < crg->minclip.x)
                        crg->minclip.x = cr->pos.x;
                    if (cr->pos.y < crg->minclip.y)
                        crg->minclip.y = cr->pos.y;
                    if (cr->pos.z < crg->minclip.z)
                        crg->minclip.z = cr->pos.z;
                    if (cr->pos.x > crg->maxclip.x)
                        crg->maxclip.x = cr->pos.x;
                    if (cr->pos.y > crg->maxclip.y)
                        crg->maxclip.y = cr->pos.y;
                    if (cr->pos.z > crg->maxclip.z)
                        crg->maxclip.z = cr->pos.z;
                }
                cr->orientation = crg->angle;
                cr->linked = first_crate;
                cr->trigger = 0;
                cr->type[0] = cc->type1;
                cr->type[1] = cc->type2;
                cr->type[2] = cc->type3;
                cr->type[3] = cc->type4;
                cr->ccindex = crg->iCrate + (short)j;
                cr->offx = cc->dx - Crate[crg->iCrate].dx;
                cr->offy = cc->dy - Crate[crg->iCrate].dy;
                cr->offz = cc->dz - Crate[crg->iCrate].dz;
                cr->draw = 1;
                num_crates_used++;
                cc->model = cr;
            }
        }
        crg->minclip.x -= 0.5f;
        crg->minclip.y -= 0.5f;
        crg->minclip.z -= 0.5f;
        crg->maxclip.x += 0.5f;
        crg->maxclip.y += 0.5f;
        crg->maxclip.z += 0.5f;
    }
    for (cr2 = NextCrate(0); cr2 != 0; cr2 = NextCrate(cr2)) {
        if (Crate[cr2->ccindex].trigger != -1) {
            new_crate = 0;
        LAB_1:
            new_crate = NextCrate(new_crate);
            if (new_crate != 0) {
                if (new_crate->ccindex != Crate[cr2->ccindex].trigger) {
                    goto LAB_1;
                }
                cr2->trigger = new_crate;
            }
        }
    }
    ConvertCrateData();
}

void HeightSortCrateData(void) {
    s32 i;
    s32 j;
    s32 l;
    s32 k;
    struct CrateCube tcr;
    struct CrateCubeGroup *crg;

    for (i = 0; i < CRATEGROUPCOUNT; i++) {
        crg = &CrateGroup[i];
        for (j = crg->iCrate; j < (s32)crg->iCrate + (s32)crg->nCrates - 1;
             j++) {
            for (k = j + 1; k < crg->iCrate + crg->nCrates; k++) {
                if (Crate[j].dy > Crate[k].dy) {
                    tcr = Crate[k];
                    Crate[k] = Crate[j];
                    Crate[j] = tcr;
                    for (l = 0; l < CRATECOUNT; l++) {
                        if (Crate[l].trigger == j) {
                            Crate[l].trigger = k;
                        } else if (Crate[l].trigger == k) {
                            Crate[l].trigger = j;
                        }
                    }
                }
            }
        }
    }
}

s32 ReadCrateData(void) {
    s32 handle;
    s32 i;
    s32 j;
    s32 version;

    CRATECOUNT = 0;
    sprintf(tbuf, D_00631250, LevelFileName);
    handle = NuMemFileOpen(Chase, NuFileLoadBuffer(tbuf, Chase, 0x7fffffff), 0);
    if (handle != 0) {
        version = NuFileReadInt(handle);
        if (4 < version) {
            NuFileClose(handle);
            return 0;
        }
        CRATEGROUPCOUNT = NuFileReadShort(handle);
        for (i = 0; i < CRATEGROUPCOUNT; i++) {
            CrateGroup[i].origin.x = NuFileReadFloat(handle);
            CrateGroup[i].origin.y = NuFileReadFloat(handle);
            CrateGroup[i].origin.z = NuFileReadFloat(handle);
            CrateGroup[i].radius = 0.0f;
            CrateGroup[i].iCrate = NuFileReadShort(handle);
            CrateGroup[i].nCrates = NuFileReadShort(handle);
            CRATECOUNT += CrateGroup[i].nCrates;
            CrateGroup[i].angle = NuFileReadShort(handle);
            for (j = CrateGroup[i].iCrate;
                 j < CrateGroup[i].iCrate + CrateGroup[i].nCrates; j++) {
                Crate[j].pos0.x = NuFileReadFloat(handle);
                Crate[j].pos0.y = NuFileReadFloat(handle);
                Crate[j].pos0.z = NuFileReadFloat(handle);
                Crate[j].shadow = NuFileReadFloat(handle);
                Crate[j].dx = NuFileReadShort(handle);
                Crate[j].dy = NuFileReadShort(handle);
                Crate[j].dz = NuFileReadShort(handle);
                Crate[j].type1 = NuFileReadChar(handle);
                if (version > 2) {
                    Crate[j].type2 = NuFileReadChar(handle);
                    Crate[j].type3 = NuFileReadChar(handle);
                    Crate[j].type4 = NuFileReadChar(handle);
                } else {
                    Crate[j].type2 = -1;
                    Crate[j].type3 = -1;
                    Crate[j].type4 = -1;
                }
                Crate[j].on = 1;
                Crate[j].timer = 0.0f;
                Crate[j].iU = NuFileReadShort(handle);
                Crate[j].iD = NuFileReadShort(handle);
                Crate[j].iN = NuFileReadShort(handle);
                Crate[j].iS = NuFileReadShort(handle);
                Crate[j].iE = NuFileReadShort(handle);
                Crate[j].iW = NuFileReadShort(handle);
                if (version > 3) {
                    Crate[j].trigger = NuFileReadShort(handle);
                } else {
                    Crate[j].trigger = -1;
                }
            }
        }
        NuFileClose(handle);
        return 1;
    }
    return 0;
}

/* Faithful reconstruction (state=asm), 56%: logic exact (x-face then z-face
 * ray/cuboid slab test, face 8/4/2/1). Residual is FP register allocation
 * (f0/f1, f8/f9 shifts) and the vTEMP=v struct-copy codegen. */
s32 RayIntersectCuboid(struct nuvec_s *p0, struct nuvec_s *p1,
                       struct nuvec_s *min, struct nuvec_s *max) {
    f32 new_ratio;
    f32 dx;
    f32 dy;
    f32 dz;
    s32 face;
    struct nuvec_s v;

    dx = p1->x - p0->x;
    dy = p1->y - p0->y;
    dz = p1->z - p0->z;
    if (p0->x <= min->x && p1->x >= min->x) {
        new_ratio = (min->x - p0->x) / dx;
        face = 8;
        v.x = min->x;
    } else if (p0->x >= max->x && p1->x <= max->x) {
        new_ratio = (max->x - p0->x) / dx;
        face = 4;
        v.x = max->x;
    } else {
        face = 0;
    }
    if (face != 0) {
        v.z = dz * new_ratio + p0->z;
        if (v.z >= min->z && v.z <= max->z) {
            v.y = dy * new_ratio + p0->y;
            if (v.y >= min->y && v.y <= max->y) {
                temp_ratio = new_ratio;
                temp_face = face;
                vTEMP = v;
                return 1;
            }
        }
    }
    if (p0->z <= min->z && p1->z >= min->z) {
        new_ratio = (min->z - p0->z) / dz;
        face = 2;
        v.z = min->z;
    } else if (p0->z >= max->z && p1->z <= max->z) {
        new_ratio = (max->z - p0->z) / dz;
        face = 1;
        v.z = max->z;
    } else {
        face = 0;
    }
    if (face != 0) {
        v.x = dx * new_ratio + p0->x;
        if (v.x >= min->x && v.x <= max->x) {
            v.y = dy * new_ratio + p0->y;
            if (v.y >= min->y && v.y <= max->y) {
                temp_ratio = new_ratio;
                temp_face = face;
                vTEMP = v;
                return 1;
            }
        }
    }
    return 0;
}

s32 CrateRayCast(struct nuvec_s *p0, struct nuvec_s *p1) {
    struct nuvec_s vMIN;
    struct nuvec_s vMAX;
    struct nuvec_s v0;
    struct nuvec_s v1;
    struct nuvec_s min;
    struct nuvec_s max;
    s32 i;
    s32 j;
    s32 face;
    s32 type;
    f32 ratio;
    struct CrateCubeGroup *group;
    struct CrateCube *crate;

    ratio = 1.0f;
    vMIN.x = (p0->x < p1->x) ? p0->x : p1->x;
    vMIN.z = (p0->z < p1->z) ? p0->z : p1->z;
    vMAX.x = (p0->x > p1->x) ? p0->x : p1->x;
    vMAX.z = (p0->z > p1->z) ? p0->z : p1->z;
    group = CrateGroup;
    for (i = 0; i < CRATEGROUPCOUNT; i++, group++) {
        if (((vMAX.x >= group->minclip.x && vMIN.x <= group->maxclip.x) &&
             vMAX.z >= group->minclip.z) &&
            vMIN.z <= group->maxclip.z) {
            v0.x = p0->x - group->origin.x;
            v0.y = p0->y;
            v0.z = p0->z - group->origin.z;
            NuVecRotateY(&v0, &v0, -(s32)group->angle);
            v1.x = p1->x - group->origin.x;
            v1.y = p1->y;
            v1.z = p1->z - group->origin.z;
            NuVecRotateY(&v1, &v1, -(s32)group->angle);
            crate = &Crate[group->iCrate];
            for (j = 0; j < group->nCrates; j++, crate++) {
                if (crate->on != 0 && crate->in_range != 0) {
                    type = GetCrateType(crate, 0) + 1;
                    if ((u32)type > 1) {
                        min.x = (s32)crate->dx * 0.5f;
                        min.y = crate->pos.y;
                        min.z = crate->dz * 0.5f;
                        max.x = min.x + 0.5f;
                        max.y = min.y + 0.5f;
                        max.z = min.z + 0.5f;
                        if (RayIntersectCuboid(&v0, &v1, &min, &max) != 0 &&
                            temp_ratio < ratio) {
                            face = temp_face;
                            ratio = temp_ratio;
                        }
                    }
                }
            }
        }
    }
    temp_face = face;
    temp_ratio = ratio;
    if (ratio < 1.0f) {
        return 1;
    }
    return 0;
}

extern void StartExclamationCrateSequence(struct CrateCubeGroup *group,
                                          struct CrateCube *crate);
extern void HopCratesAbove(f32 speed, struct CrateCubeGroup *group,
                           struct CrateCube *crate);
extern f32 CRATEHOPSPEED;
extern s32 KillPlayer(struct obj_s *obj, s32 anim);

/* Faithful reconstruction (state=asm): PS2 inlines StartExclamation (0xE) and
 * DestroyAllNitroCrates (0x11) like AttackCrate; kept as calls. */
void BreakCrate(struct CrateCubeGroup *group, struct CrateCube *crate, s32 type,
                s32 attack) {
    if (type == 0x13 && (attack & 0x200) == 0) {
        if (crate->action == -1) {
            NewCrateAnimation(crate, 0x13, 0x58, 0);
            GameSfx(0x39, &crate->pos);
        }
    } else if (type == 0xe) {
        if (crate->action == -1 &&
            NewCrateAnimation(crate, 0xe, 0x35, 0) == 0) {
            StartExclamationCrateSequence(group, crate);
        }
    } else if (type == 0x11) {
        if (crate->action == -1 &&
            NewCrateAnimation(crate, 0x11, 0x35, 0) == 0) {
            DestroyAllNitroCrates(group, crate);
        }
    } else {
        if (CrateOff(group, crate, 0, attack >> 9 & 1) != 0) {
            HopCratesAbove(CRATEHOPSPEED, group, crate);
        }
    }
}
extern s32 GetDieAnim(struct obj_s *obj, s32 x);

/* Faithful reconstruction (state=asm): PS2 INLINES StartExclamationCrateSequence
 * (type 0xE) and DestroyAllNitroCrates (type 0x11); the GC reference calls them.
 * Kept as calls -> codegen diverges at those two sites (jal vs inlined body). */
s32 AttackCrate(struct obj_s *obj, struct CrateCubeGroup *group,
                struct CrateCube *crate) {
    s32 type;

    type = GetCrateType(crate, 0);
    if (type == -1) {
        return 1;
    }
    if (type == 0xe) {
        if (crate->action == -1 &&
            NewCrateAnimation(crate, 0xe, 0x35, 0) == 0) {
            StartExclamationCrateSequence(group, crate);
        }
        return 2;
    }
    if (type == 0x11) {
        if (crate->action == -1 &&
            NewCrateAnimation(crate, 0x11, 0x35, 0) == 0) {
            DestroyAllNitroCrates(group, crate);
        }
        return 2;
    }
    if (CrateOff(group, crate, 0, obj->attack >> 9 & 1) != 0) {
        if (type == 9 || type == 0x10) {
            KillPlayer(obj, GetDieAnim(obj, 6));
        }
        return 1;
    }
    return 0;
}

/* Faithful near-match (state=asm), 92.4%: logic/offsets exact (long column
 * search, no .p2align wall). Residual is a consistent +1 FP-register shift in
 * the dx/dz clip-check math (temp+-radius compares) -- an allocation tie. */
struct CrateCube *InCrate(f32 x, f32 z, f32 ytop, f32 ybot, f32 radius) {
    struct CrateCubeGroup *group;
    struct CrateCube *crate;
    s32 i;
    s32 j;
    s32 type;
    struct nuvec_s vNew;
    f32 ymid;
    f32 d;
    f32 dbest;
    f32 dx;
    f32 dy;
    f32 dz;
    f32 temp;

    ymid = (ytop + ybot) * 0.5f;
    temp_pGroup = 0;
    temp_pCrate = 0;
    group = CrateGroup;
    for (i = 0; i < CRATEGROUPCOUNT; i++, group++) {
        if (((!(x < group->minclip.x - radius) &&
              !(x > group->maxclip.x + radius)) &&
             !(z < group->minclip.z - radius)) &&
            !(z > group->maxclip.z + radius)) {
            vNew.x = x - group->origin.x;
            vNew.z = z - group->origin.z;
            NuVecRotateY(&vNew, &vNew, -(s32)group->angle);
            crate = &Crate[group->iCrate];
            for (j = 0; j < group->nCrates; j++, crate++) {
                if (crate->on != 0) {
                    type = GetCrateType(crate, 0);
                    if (!(type == -1 || type == 0)) {
                        temp = (s32)crate->dx * 0.5f;
                        if (!(vNew.x < temp - radius) &&
                            !(vNew.x > temp + 0.5f + radius)) {
                            temp = (s32)crate->dz * 0.5f;
                            if (!(vNew.z < temp - radius) &&
                                !(vNew.z > temp + 0.5f + radius)) {
                                if (!(ytop < crate->pos.y) &&
                                    !(ybot > crate->pos.y + 0.5f)) {
                                    dy = crate->pos.y + 0.25f;
                                    dx = crate->pos.x;
                                    dz = crate->pos.z;
                                    dx -= x;
                                    dy -= ymid;
                                    dz -= z;
                                    d = dx * dx + dy * dy + dz * dz;
                                    if (temp_pCrate == 0 || d < dbest) {
                                        dbest = d;
                                        temp_crate_type = type;
                                        temp_pGroup = group;
                                        temp_pCrate = crate;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return temp_pCrate;
}

struct game_s {
    u8 pad_000[0x3FC];
    u8 lives;                /* 0x3FC */
};
extern struct game_s Game;
extern struct nuvec_s vNEWMASK;
extern s32 mask_crates;
extern s32 newmask_advise;
extern f32 TimeTrialWait;
extern s32 last_questionmark_extralife;
extern void ResetCheckpoint(s32 iRAIL, s32 iALONG, struct nuvec_s *pos,
                            f32 fALONG);
extern void AddGameDebris(s32 type, struct nuvec_s *pos);
extern void AddTempWumpa(struct CrateCube *crate, s32 n, f32 x, f32 y, f32 z);
extern void AddCrateExplosion(struct nuvec_s *pos, s32 type, s32 ang,
                              struct nuvec_s *colbox);
extern s32 VEHICLECONTROL;
extern void AddScreenWumpa(f32 x, f32 y, f32 z, s32 n);

/* Breaks one crate: type dispatch through a jump table over 2..0x14, then the
 * shared GameSfx tail. The panel-debris scale is genuinely `double` in the
 * source -- SN ee-gcc lowers plain doubles to fptodp/dpcmp/dpsub/dpmul itself,
 * so `if (d < 0.0) d = 0.0 - d;` is the abs, not a call.
 *
 * Faithful near-match (state=asm), 507 vs 506 words. Frame 0xC0, reg_mask
 * 0x803f0000 and the jump table are retail's, and the case bodies are emitted
 * in retail's order (2, 7, 3, 0x10, 9, 0xC, 0xB, 0xA, 0x14, 6/8, default) --
 * gcc lays case bodies out in source order, so the `case` order here is the
 * retail source order, not numeric. Levers: `(type == 0xD || type == 0xE)` is
 * parenthesised so fold turns the pair into retail's `(unsigned)(t-13) < 2`
 * range test; the `ictd` temp stops gcc reloading i_cratetypedata after the
 * aliasing stores; and `found` is assigned only on the two loop exits (hence
 * the goto) because an init before the loop would live across the GetCrateType
 * calls and burn a seventh callee-saved register. Two deltas remain -- see the
 * recorded blocker. */
s32 CrateOff(struct CrateCubeGroup *group, struct CrateCube *crate, s32 how,
             s32 silent) {
    struct nuvec_s pos;
    struct nuvec_s screen;
    struct nuvec_s v[2];
    struct CRATETYPEDATA *data;
    struct CrateCube *crate2;
    s32 type;
    s32 sfx;
    s32 n;
    s32 i;
    s32 found;
    s32 ictd;
    double d;

    type = GetCrateType(crate, 0);
    if (type == -1 || (type == 0xD || type == 0xE) || type == 0 ||
        type == 0xF || type == 0x11) {
        return 0;
    }
    crate->on = 0;
    if (crate->model != 0) {
        crate->model->draw = 0;
    }
    pos.x = crate->pos.x;
    pos.y = crate->pos.y + 0.25f;
    pos.z = crate->pos.z;
    if (type != 7 && crate->in_range != 0) {
        AddCrateExplosion(&crate->pos, type, group->angle, crate->colbox);
    }
    sfx = 0x25;
    switch (type) {
    case 2:
        if ((how & 3) == 0 && player->obj.dead == 0 && silent == 0) {
            n = (crate->flags & 0x40) ? 3 : 2;
            NuCameraTransformScreenClip(&screen, &pos, 1, 0);
            v[0].x = pos.x + GameCam.vX.x * 0.1f;
            v[0].y = pos.y + GameCam.vX.y * 0.1f;
            v[0].z = pos.z + GameCam.vX.z * 0.1f;
            NuCameraTransformScreenClip(&v[1], &v[0], 1, 0);
            d = screen.x - v[1].x;
            if (d < 0.0) {
                d = 0.0 - d;
            }
            AddPanelDebris(screen.x, screen.y, n, d * 3.6363637f, 1);
            if (TimeTrial == 0 && (crate->flags & 0x40) == 0) {
                if (crate->type1 == 8 && crate->i > 0) {
                    ictd = i_cratetypedata;
                    if (ictd <= 0x1f) {
                        data = &CrateTypeData[ictd];
                        data->crate = crate;
                        data->type1 = crate->type1;
                        data->type2 = crate->type2;
                        data->type3 = crate->type3;
                        data->type4 = crate->type4;
                        i_cratetypedata = ictd + 1;
                    }
                    if (crate->i == 1) {
                        crate->type3 = crate->type4;
                    }
                    crate->type4 = -1;
                } else if (crate->type1 == 2) {
                    ictd = i_cratetypedata;
                    if (ictd <= 0x1f) {
                        data = &CrateTypeData[ictd];
                        data->crate = crate;
                        data->type1 = crate->type1;
                        data->type2 = crate->type2;
                        data->type3 = crate->type3;
                        data->type4 = crate->type4;
                        i_cratetypedata = ictd + 1;
                    }
                    crate->type1 = 5;
                } else if (crate->type1 == 0 && crate->type3 == 2) {
                    ictd = i_cratetypedata;
                    if (ictd <= 0x1f) {
                        data = &CrateTypeData[ictd];
                        data->crate = crate;
                        data->type1 = crate->type1;
                        data->type2 = crate->type2;
                        data->type3 = crate->type3;
                        data->type4 = crate->type4;
                        i_cratetypedata = ictd + 1;
                    }
                    crate->type3 = 5;
                }
            }
        }
        break;
    case 7:
        sfx = 0x17;
        ResetCheckpoint(crate->iRAIL, crate->iALONG, &crate->pos,
                        crate->fALONG);
        NewCrateAnimation(crate, type, 0x34, 0);
        break;
    case 3:
        if ((how & 3) == 0 && player->obj.dead == 0 && silent == 0) {
            mask_crates++;
            vNEWMASK = crate->model->pos;
            newmask_advise = 0;
        }
        break;
    case 0x10:
        sfx = 0x3B;
        AddKaboom(2, &pos, 0.0f);
        AddGameDebris(6, &pos);
        break;
    case 9:
        sfx = 0x3B;
        AddKaboom(1, &pos, 0.0f);
        AddGameDebris(5, &pos);
        break;
    case 0xC:
        if (silent == 0) {
            TimeTrialWait = TimeTrialWait + 3.0f;
        }
        break;
    case 0xB:
        if (silent == 0) {
            TimeTrialWait = TimeTrialWait + 2.0f;
        }
        break;
    case 0xA:
        if (silent == 0) {
            TimeTrialWait = TimeTrialWait + 1.0f;
        }
        break;
    case 0x14:
        if (silent == 0 && (how == 0 || (how & 0xC) != 0)) {
            plr_invisibility_time = 0.0f;
            GameSfx(0x1E, 0);
        }
        break;
    case 6:
    case 8:
        break;
    default:
        if ((how & 3) == 0 && silent == 0 && TimeTrial == 0 &&
            player->obj.dead == 0 && (crate->flags & 0x400) == 0) {
            if (type == 5 && last_questionmark_extralife == 0 &&
                Game.lives < 10 && (crate->flags & 0x40) == 0 &&
                qrand() < 0x4000 - ((Game.lives << 14) / 10)) {
                NuCameraTransformScreenClip(&screen, &pos, 1, 0);
                v[0].x = pos.x + GameCam.vX.x * 0.1f;
                v[0].y = pos.y + GameCam.vX.y * 0.1f;
                v[0].z = pos.z + GameCam.vX.z * 0.1f;
                NuCameraTransformScreenClip(&v[1], &v[0], 1, 0);
                d = screen.x - v[1].x;
                if (d < 0.0) {
                    d = 0.0 - d;
                }
                AddPanelDebris(screen.x, screen.y, 2, d * 3.6363637f, 1);
                last_questionmark_extralife = 1;
            } else {
                if (last_questionmark_extralife != 0) {
                    last_questionmark_extralife--;
                }
                crate2 = &Crate[group->iCrate];
                for (i = 0; i < group->nCrates; i++, crate2++) {
                    if (crate2 != crate && crate2->on != 0 &&
                        GetCrateType(crate2, 0) != 0 &&
                        crate2->dx == crate->dx && crate2->dz == crate->dz &&
                        crate2->pos.y == crate->pos.y + 0.5f) {
                        found = 1;
                        goto searched;
                    }
                }
                found = 0;
            searched:
                if (found != 0) {
                    AddScreenWumpa(pos.x, pos.y, pos.z, 1);
                } else {
                    if (type == 0x13) {
                        n = 1;
                    } else {
                        n = qrand() / 0x3334 + 1;
                    }
                    if ((crate->flags & 0x1000) != 0 ||
                        (VEHICLECONTROL == 1 && player->obj.vehicle != -1)) {
                        AddScreenWumpa(pos.x, pos.y, pos.z, n);
                    } else {
                        AddTempWumpa(crate, n, pos.x, pos.y, pos.z);
                    }
                }
            }
        }
        break;
    }
    GameSfx(sfx, (crate->flags & 0x400) ? 0 : &pos);
    return 1;
}

extern u16 CRATEEXPLOSIONFRAMES;
extern f32 NuTrigTable[];
extern s32 temp_crate_bounce;

/* One crate's reaction to being bounced on, dispatched on `type` as a plain
 * if/else-if chain (retail tests 0xF, 9, 9, 0xE, 0x11, 0x13, 6, 4||0xD in
 * source order with pure beq/bne -- not a switch, which would sort the cases
 * or build a jump table). The 9 is tested twice because the first block is a
 * bare `if` that falls through into the chain.
 *
 * Both two-armed tests are ternaries of *comparisons*, not of values: do_jump
 * expands `c ? a == k : b == k` into two branchy arms sharing a tail, which is
 * what retail has -- a ternary of values would materialise a register first.
 * The arm order is the one whose false branch is retail's fall-through.
 * `crate->timer` is re-read rather than cached in a local: the lone `mov.s` is
 * GCSE PRE, not a C variable. `bounce = 1` sits at the end of the type==6 arm
 * (not in each branch) so cross-jumping folds it into the shared tail. */
s32 CrateBounceReaction(struct CrateCubeGroup *group, struct CrateCube *crate,
                        s32 type, s32 attack) {
    struct nuvec_s pos;
    struct CrateCubeGroup *group2;
    struct CrateCube *crate2;
    s32 i;
    s32 j;
    s32 sfx;
    s32 bounce;
    s32 ret;

    sfx = -1;
    bounce = 0;
    ret = 0;
    if (type == 0xF) {
        goto done;
    }
    if (type == 9) {
        if (crate->timer > 0.0f && crate->newtype != -1) {
            goto done;
        }
        /* gcc folds the newtype/subtype pair into one masked word compare at
         * 0x3C (fold_truthop merges adjacent field comparisons). */
        if ((TimeTrial == 0 ? crate->type1 == 8 : crate->type2 == 8) &&
            crate->newtype == -1 && crate->subtype == 9) {
            crate->newtype = 9;
            crate->timer = 0.02f;
            sfx = 2;
            bounce = 1;
            goto done;
        }
    }
    if (type == 9) {
        if (crate->timer != 0.0f) {
            goto done;
        }
        crate->timer = crate->timer + 0.02f;
        sfx = 2;
        bounce = 1;
    } else if (type == 0xE) {
        if (crate->action != -1) {
            goto done;
        }
        sfx = 0xE;
        bounce = 1;
        if (NewCrateAnimation(crate, 0xE, 0x35, 0) != 0) {
            goto done;
        }
        pos.x = crate->pos.x;
        pos.y = crate->pos.y + 0.25f;
        pos.z = crate->pos.z;
        temp_pGroup = group;
        temp_pCrate = crate;
        AddKaboom(0x20, &pos, 0.0f);
        crate->metal_count = 1;
        crate->newtype = 0xf;
        GameSfx(0x35, &temp_pCrate->pos);
    } else if (type == 0x11) {
        if (crate->action != -1) {
            goto done;
        }
        sfx = 0xE;
        bounce = 1;
        if (NewCrateAnimation(crate, 0x11, 0x35, 0) != 0) {
            goto done;
        }
        group2 = CrateGroup;
        for (i = 0; i < CRATEGROUPCOUNT; i++, group2++) {
            crate2 = &Crate[group2->iCrate];
            for (j = 0; j < group2->nCrates; j++, crate2++) {
                if ((crate2->on != 0) && (GetCrateType(crate2, 0) == 0x10)) {
                    CrateOff(group2, crate2, 0, 0);
                }
            }
        }
        pos.x = crate->pos.x;
        pos.y = crate->pos.y + 0.25f;
        pos.z = crate->pos.z;
        temp_pGroup = group;
        temp_pCrate = crate;
        AddKaboom(0x20, &pos, 0.0f);
        crate->newtype = 0xf;
        crate->metal_count = 1;
        JudderGameCamera(&GameCam, 0.5f, 0);
        GameSfx(0x33, &crate->pos);
    } else if (type == 0x13) {
        NewCrateAnimation(crate, 0x13, 0x58, 0);
        GameSfx(0x38, &crate->pos);
    } else if (type == 6) {
        sfx = 2;
        if (crate->timer == 0.0f) {
            crate->timer = crate->timer + 0.02f;
        }
        if (crate->counter >= 2) {
            crate->counter--;
            if (TimeTrial == 0 && player->obj.dead == 0) {
                AddScreenWumpa(crate->pos.x, crate->pos.y + 0.25f,
                               crate->pos.z, 2);
            }
            NewCrateAnimation(crate, type, attack == 2 ? 0x58 : 0x16, 0);
        } else {
            if (((crate->flags & 0x20) ? crate->timer < 2.5f
                                       : crate->timer < 5.0f) &&
                TimeTrial == 0 && player->obj.dead == 0) {
                AddScreenWumpa(crate->pos.x, crate->pos.y + 0.25f,
                               crate->pos.z, 2);
            }
            ret = CrateOff(group, crate, 0, 0);
        }
        bounce = 1;
    } else if (type == 4 || type == 0xD) {
        NewCrateAnimation(crate, type, 0x58, 0);
        sfx = type == 4 ? 2 : 0xE;
        bounce = 3;
    } else {
        ret = CrateOff(group, crate, 0, 0);
        sfx = 2;
        bounce = 1;
    }
done:
    if (VEHICLECONTROL != 2) {
        temp_crate_bounce |= bounce;
        if (bounce != 0) {
            NewRumble(&player->rumble, 0x7F);
            NewBuzz(&player->rumble, 0xA);
        }
    }
    if (sfx != -1) {
        GameSfx(sfx, &crate->pos);
    }
    return ret;
}

/* Fires the box-explosion at slot iBOXEXP: 4 side faces x 6 shards, each yawed
 * a quarter turn further, then a final 6 shards pitched up (ang[0] = 0x4000).
 * The -0.12f/0.035f/-0.1f factors are `li.s` constants that SN's `as` pools
 * into .lit4 as D_0062D788/78C/790/794 (0.035f twice -- it does not dedup).
 *
 * Faithful near-match (state=asm), 1064/1072 bytes exact: frame, reg_mask and
 * every $s0-$s7 / $f20-$f23 role are retail's. The 2-word residual is the
 * phase-2 preheader, where retail emits [li $s1,-1][li $s2,5] and we emit them
 * swapped: LICM inserts hoisted invariants *after* the front-end's loop-counter
 * init and the scheduler breaks ties on RTL order, so the compiler-generated
 * division constant can never precede `j = 5`. The same ordering in the phase-1
 * outer loop *is* steerable -- that is why `a = i * 0x4000;` is a statement of
 * its own: a real statement before the inner loop lands ahead of its init.
 * `face` is spelled BoxExpList[iBOXEXP].BoxPol rather than box->BoxPol so gcc
 * folds +0x1C into the base (retail's `addiu $t1,$v0,0x1C`), and the trig index
 * is masked with & 0xFFFF rather than a (u16) cast, which would narrow the load
 * to `lhu`. */
void AddCrateExplosion(struct nuvec_s *pos, s32 type, s32 ang,
                       struct nuvec_s *colbox) {
    struct BoxExpType *box;
    struct BoxPol_s *face;
    s32 i;
    s32 j;
    s32 a;
    f32 rndang;

    box = &BoxExpList[iBOXEXP];
    face = BoxExpList[iBOXEXP].BoxPol;
    box->type = type;
    box->time = CRATEEXPLOSIONFRAMES;
    box->colbox[0] = colbox[0];
    box->colbox[1] = colbox[1];
    for (i = 0; i < 4; i++) {
        a = i * 0x4000;
        for (j = 5; j >= 0; j--, face++) {
            face->rndfade = qrand() / 4096;
            face->ang[0] = 0;
            face->ang[1] = ang + a;
            face->ang[2] = 0;
            face->angmom[0] = qrand() / 64;
            face->angmom[1] = qrand() / 64;
            face->angmom[2] = qrand() / 64;
            face->pos = *pos;
            face->pos.y += 0.25f;
            face->pos.x -= NuTrigTable[face->ang[1] & 0xFFFF] * 0.25f;
            face->pos.z -=
                NuTrigTable[(face->ang[1] + 0x4000) & 0xFFFF] * 0.25f;
            rndang = (f32)(qrand() / 4 - 0x2000);
            face->mom.x =
                NuTrigTable[(s32)((f32)face->ang[1] + rndang) & 0xFFFF] *
                0.25f * -0.12f;
            face->mom.y = (f32)qrand() / 1966080.0f + 0.035f;
            face->mom.z = NuTrigTable[(s32)((f32)face->ang[1] + rndang +
                                            16384.0f) &
                                      0xFFFF] *
                          0.25f * -0.12f;
        }
    }
    for (j = 5; j >= 0; j--, face++) {
        face->rndfade = qrand() / 4096;
        face->ang[0] = 0x4000;
        face->ang[1] = ang;
        face->ang[2] = 0;
        face->angmom[0] = qrand() / 64;
        face->angmom[1] = qrand() / 64;
        face->angmom[2] = qrand() / 64;
        face->pos = *pos;
        face->pos.y += 0.5f;
        rndang = (f32)qrand();
        face->mom.x =
            NuTrigTable[(s32)rndang & 0xFFFF] * 0.25f * 0.5f * -0.1f;
        face->mom.y = (f32)qrand() / 3276800.0f + 0.035f;
        face->mom.z = NuTrigTable[(s32)(rndang + 16384.0f) & 0xFFFF] * 0.25f *
                      0.5f * -0.1f;
    }
    iBOXEXP++;
    if (iBOXEXP == 0x10) {
        iBOXEXP = 0;
    }
}

/* Faithful reconstruction (state=asm), ~9%: physics logic/offsets exact
 * (ang/angmom int @0x00/0x0C, pos/mom float @0x18/0x24, colbox[2]@0x4, 5 bounce
 * checks, angmom reflect). Codegen-divergent: retail precomputes pointers and
 * schedules the FP integrator differently. */
void UpdateCrateExplosions(void) {
    struct BoxExpType *box;
    struct BoxPol_s *face;
    s32 i;
    s32 j;
    s32 k;
    s32 bounce;

    box = BoxExpList;
    for (i = 0; i < 0x10; i++, box++) {
        if (box->time != 0) {
            box->time--;
            if (box->time != 0) {
                face = box->BoxPol;
                for (j = 0; j < 5; j++) {
                    for (k = 5; k >= 0; k--, face++) {
                        bounce = 0;
                        face->pos.x += face->mom.x;
                        face->pos.y += face->mom.y;
                        face->mom.y -= D_0062D798;
                        face->pos.z += face->mom.z;
                        face->ang[0] += face->angmom[0];
                        face->ang[1] += face->angmom[1];
                        face->ang[2] += face->angmom[2];
                        if (face->pos.x < box->colbox[0].x &&
                            face->mom.x < 0.0f) {
                            face->pos.x = box->colbox[0].x;
                            bounce = 1;
                            face->mom.x = -face->mom.x * D_0062D79C;
                        }
                        if (face->pos.y < box->colbox[0].y &&
                            face->mom.y < 0.0f) {
                            face->pos.y = box->colbox[0].y;
                            bounce = 1;
                            face->mom.y = -face->mom.y * D_0062D79C;
                        }
                        if (face->pos.z < box->colbox[0].z &&
                            face->mom.z < 0.0f) {
                            face->pos.z = box->colbox[0].z;
                            bounce = 1;
                            face->mom.z = -face->mom.z * D_0062D79C;
                        }
                        if (face->pos.x > box->colbox[1].x &&
                            0.0f < face->mom.x) {
                            face->pos.x = box->colbox[1].x;
                            bounce = 1;
                            face->mom.x = -face->mom.x * D_0062D79C;
                        }
                        if (face->pos.z > box->colbox[1].z &&
                            0.0f < face->mom.z) {
                            face->pos.z = box->colbox[1].z;
                            bounce = 1;
                            face->mom.z = -face->mom.z * D_0062D79C;
                        }
                        if (bounce) {
                            face->angmom[0] = -face->angmom[0];
                            face->angmom[1] = -face->angmom[1];
                            face->angmom[2] = -face->angmom[2];
                        }
                    }
                }
            }
        }
    }
}

extern void NewTopBot(struct obj_s *obj);
extern void OldTopBot(struct obj_s *obj);
extern f32 D_0062D7B0;
extern void NuVecRotateY(struct nuvec_s *dst, struct nuvec_s *src, s32 ang);
extern f32 D_0062D7A0;
extern f32 D_0062D7A4;

extern f32 NuFabs(f32 x);

/* The three `0.249` compares are double literals, not floats: each one makes
 * gcc call fptodp/dpcmp and emit its own `li.d $a1,0.249`, which SN's `as`
 * pools into the unit's .rodata as D_0061EE90/98/A0 (2.95 does not dedup equal
 * doubles). Writing them `0.249f` compares in single precision and loses the
 * helper calls entirely. */
struct crate_s *FindLocalCrate(struct nuvec_s *pos) {
    struct crate_s *crate;
    struct crate_s *next;
    struct nuvec_s v;

    crate = (struct crate_s *)NuLstGetNext(crates, 0);
    while (crate != 0) {
        next = (struct crate_s *)NuLstGetNext(crates, (struct nulnkhdr_s *)crate);
        v.x = pos->x - crate->pos.x;
        v.y = pos->y - crate->pos.y;
        v.z = pos->z - crate->pos.z;
        NuVecRotateY(&v, &v, -crate->orientation);
        v.x = NuFabs(v.x);
        v.y = NuFabs(v.y);
        v.z = NuFabs(v.z);
        if (v.x < 0.249 && v.y < 0.249 && v.z < 0.249) {
            return crate;
        }
        crate = next;
    }
    return 0;
}

struct crate_s *FindOverlap(struct crate_s *crate) {
    struct crate_s *found;
    struct nuvec_s v;
    s32 x;
    s32 y;
    s32 z;

    for (x = -1; x < 2; x += 2) {
        for (z = -1; z < 2; z += 2) {
            for (y = -1; y < 2; y += 2) {
                v.x = x * 0.25f;
                v.z = z * 0.25f;
                v.y = y * 0.25f;
                NuVecRotateY(&v, &v, crate->orientation);
                v.x += crate->pos.x;
                v.y += crate->pos.y;
                v.z += crate->pos.z;
                found = FindLocalCrate(&v);
                if (found != 0) {
                    return found;
                }
            }
        }
    }
    return FindLocalCrate(&crate->pos);
}

struct cRPos_s {
    s8 iRAIL;                /* 0x00 */
    u8 pad_01;
    s16 iALONG;              /* 0x02 */
    u8 pad_04[4];
    f32 fALONG;              /* 0x08 */
};
extern struct cRPos_s *best_cRPos;
extern s32 cp_goto;
extern struct nuvec_s cpGOTO;
extern f32 NuVecDist(struct nuvec_s *a, struct nuvec_s *b, void *c);

/* Faithful reconstruction (state=asm), ~11%: logic/offsets correct (best_cRPos
 * iRAIL@0/iALONG@2/fALONG@8, cpGOTO x@0/y@4/z@8, direction-dependent
 * FurtherALONG/FurtherBEHIND). Blocked on control-flow: the && chain compiles
 * to a different branch layout than retail's goto-nested direction dispatch. */
s32 GotoCheckpoint(struct nuvec_s *pos, s32 direction) {
    struct CrateCubeGroup *group;
    struct CrateCube *crate;
    s32 i;
    s32 j;
    s32 iRAIL;
    s32 iALONG;
    f32 fALONG;

    cp_goto = -1;
    if (best_cRPos == 0 || (u32)direction > 1) {
        return 0;
    }
    group = CrateGroup;
    iRAIL = -1;
    for (i = 0; i < CRATEGROUPCOUNT; i++, group++) {
        crate = &Crate[group->iCrate];
        for (j = 0; j < group->nCrates; j++, crate++) {
            if (((crate->type1 == 7) &&
                 (NuVecDist(pos, &crate->pos0, 0) > 5.0f)) &&
                (((direction == 0 &&
                   FurtherALONG(crate->iRAIL, crate->iALONG, crate->fALONG,
                                best_cRPos->iRAIL, best_cRPos->iALONG,
                                best_cRPos->fALONG) != 0) ||
                  (direction == 1 &&
                   FurtherBEHIND(crate->iRAIL, crate->iALONG, crate->fALONG,
                                 best_cRPos->iRAIL, best_cRPos->iALONG,
                                 best_cRPos->fALONG) != 0)))) {
                if ((iRAIL == -1) ||
                    ((direction == 0 &&
                      FurtherBEHIND(crate->iRAIL, crate->iALONG, crate->fALONG,
                                    iRAIL, iALONG, fALONG) != 0) ||
                     (direction == 1 &&
                      FurtherALONG(crate->iRAIL, crate->iALONG, crate->fALONG,
                                   iRAIL, iALONG, fALONG) != 0))) {
                    cpGOTO.x = crate->pos.x;
                    cpGOTO.y = crate->pos0.y + 0.5f + 1.0f;
                    cpGOTO.z = crate->pos.z;
                    iRAIL = crate->iRAIL;
                    iALONG = crate->iALONG;
                    fALONG = crate->fALONG;
                }
            }
        }
    }
    if (iRAIL != -1) {
        cp_goto = direction;
        return 1;
    }
    return 0;
}

/* Faithful near-match (state=asm), 94.4%: logic/offsets exact. Residual is
 * prologue f20/f21 ordering and the LICM placement of the f21 (D_0062D7A4)
 * load relative to the nCrates guard -- scheduling ties, not source-steerable. */
f32 CrateTopBelow(struct nuvec_s *pos) {
    struct CrateCubeGroup *group;
    struct CrateCube *crate;
    s32 i;
    s32 j;
    f32 top;
    f32 y;
    struct nuvec_s vNew;

    y = D_0062D7A0;
    group = CrateGroup;
    for (i = 0; i < CRATEGROUPCOUNT; i++, group++) {
        if (((!(pos->x < group->minclip.x) && !(pos->x > group->maxclip.x)) &&
             !(pos->z < group->minclip.z)) && !(pos->z > group->maxclip.z)) {
            f32 nc;
            vNew.x = pos->x - group->origin.x;
            vNew.z = pos->z - group->origin.z;
            NuVecRotateY(&vNew, &vNew, -(s32)group->angle);
            crate = &Crate[group->iCrate];
            nc = D_0062D7A4;
            for (j = 0; j < group->nCrates; j++, crate++) {
                if (crate->on != 0) {
                    if ((u32)(GetCrateType(crate, 0) + 1) > 1) {
                        top = crate->pos.y + 0.5f;
                        if (!(pos->y < top)) {
                            if (vNew.x < crate->dx * 0.5f ||
                                vNew.x > crate->dx * 0.5f + 0.5f) {
                                continue;
                            }
                            if (vNew.z < crate->dz * 0.5f ||
                                vNew.z > crate->dz * 0.5f + 0.5f) {
                                continue;
                            }
                            if (y == nc || y < top) {
                                y = top;
                            }
                        }
                    }
                }
            }
        }
    }
    return y;
}

f32 CrateBottomAbove(struct nuvec_s *pos) {
    struct CrateCubeGroup *group;
    struct CrateCube *crate;
    s32 i;
    s32 j;
    s32 type;
    f32 y;
    struct nuvec_s vNew;

    y = 2000000.0f;
    group = CrateGroup;
    for (i = 0; i < CRATEGROUPCOUNT; i++, group++) {
        if (((!(pos->x < group->minclip.x) && !(pos->x > group->maxclip.x)) &&
             !(pos->z < group->minclip.z)) && !(pos->z > group->maxclip.z)) {
            vNew.x = pos->x - group->origin.x;
            vNew.z = pos->z - group->origin.z;
            NuVecRotateY(&vNew, &vNew, -(s32)group->angle);
            crate = &Crate[group->iCrate];
            for (j = 0; j < group->nCrates; j++, crate++) {
                if (crate->on != 0) {
                    type = GetCrateType(crate, 0);
                    if (type == 0 || type == -1) {
                        continue;
                    }
                    if (!(crate->pos.y < pos->y)) {
                        if (vNew.x < crate->dx * 0.5f ||
                            vNew.x > crate->dx * 0.5f + 0.5f) {
                            continue;
                        }
                        if (vNew.z < crate->dz * 0.5f ||
                            vNew.z > crate->dz * 0.5f + 0.5f) {
                            continue;
                        }
                        if (y != 2000000.0f && !(crate->pos.y < y)) {
                            continue;
                        }
                        y = crate->pos.y;
                    }
                }
            }
        }
    }
    return y;
}

void CrateSafety(struct CrateCubeGroup *group, struct CrateCube *crate,
                 struct obj_s *obj) {
    struct CrateCube *crate2;
    struct CrateCube *crate3;
    f32 size;
    s32 i;

    size = (obj->top - obj->bot) * obj->SCALE;
    if ((obj->bot + obj->top) * obj->SCALE * 0.5f + obj->pos.y <
        crate->pos.y + 0.25f) {
        crate2 = &Crate[group->iCrate];
        crate3 = crate;
        for (i = 0; i < group->nCrates; i++, crate2++) {
            if (((crate2->on != 0) && (crate2->dx == crate3->dx)) &&
                (crate2->dz == crate3->dz)) {
                if (crate2->pos.y < crate3->pos.y) {
                    if (crate3->pos.y - (crate2->pos.y + 0.5f) > size) {
                        break;
                    }
                    crate3 = crate2;
                }
            }
        }
        obj->pos.y = crate3->pos.y - obj->top * obj->SCALE;
        if ((crate3->shadow != D_0062D7B0) &&
            (crate3->pos.y - crate3->shadow > size)) {
            goto SetBot;
        }
    }
    crate2 = &Crate[group->iCrate];
    crate3 = crate;
    for (i = 0; i < group->nCrates; i++, crate2++) {
        if (((crate2->on != 0) && (crate2->dx == crate3->dx)) &&
            (crate2->dz == crate3->dz)) {
            if (crate2->pos.y > crate3->pos.y) {
                if (crate2->pos.y - (crate3->pos.y + 0.5f) > size) {
                    break;
                }
                crate3 = crate2;
            }
        }
    }
    obj->pos.y = (crate3->pos.y + 0.5f) - obj->bot * obj->SCALE;
SetBot:
    NewTopBot(obj);
    OldTopBot(obj);
    obj->mom.y = 0.0f;
}

/* Every push re-reads the global cursor, so these stay macros over
 * `rndrstream_3d` rather than taking a local stream pointer. */
#define RS3D_PUSH_F(v)  do { *(f32 *)rndrstream_3d->cur = (v);  \
                             rndrstream_3d->cur += 4; } while (0)
#define RS3D_PUSH_W(v)  do { *(u32 *)rndrstream_3d->cur = (v);  \
                             rndrstream_3d->cur += 4; } while (0)
#define RS3D_PUSH_D(v)  do { *(u64 *)rndrstream_3d->cur = (v);  \
                             rndrstream_3d->cur += 8; } while (0)

/* Faithful reconstruction (state=asm): compiles to exactly 254 instructions =
 * the 1016-byte retail extent, frame 0x140 and reg_mask 0xc0ff0000 both exact.
 * Three codegen deltas remain -- retail spills `a` and keeps `mat` in $s7 (gcc
 * here does the reverse), retail holds 1.0f in two FP regs ($f21 copied from
 * $f20) where gcc rematerializes it in the loop, and retail's stream pushes
 * group as [load][data store][cursor store] while gcc pairs each cursor store
 * with the *next* push's data store. See the MCP blocker for the probes. */
void AddQuad3DrotXYZ(struct nuvec_s *pos, struct nuvec_s *shape,
                     struct numtl_s *mat, s32 *rot, f32 *uv, u32 col) {
    struct nuvec_s screen[4];
    struct nuvec4_s v4;
    struct numtx_s m;
    struct numtx_s *vpcs;
    u32 r;
    u32 g;
    u32 b;
    u32 a;
    s32 i;

    if (NuCameraClipTestPoints(pos, 1, 0)) {
        return;
    }
    /* Only r and a are masked here: g and b keep their unmasked shift and are
     * narrowed at the push, which is where retail's two loop-body `andi`s
     * come from (the shifts themselves are loop-invariant and hoisted). */
    r = col & 0xFF;
    g = col >> 8;
    b = col >> 16;
    a = col >> 24;
    vpcs = NuCameraGetVPCSMtx();
    NuVpGetCurrentViewport();
    NuMtxSetRotateXYZ(&m, rot);
    NuMtxTranslate(&m, pos);
    NuMtxMulH(&m, &m, vpcs);

    v4.x = shape[0].x;
    v4.y = shape[0].y;
    v4.z = shape[0].z;
    v4.w = 1.0f;
    NuVec4MtxTransformVU0(&v4, &v4, &m);
    NuVec4ScaleXYZVU0(&v4, &v4, 1.0f / v4.w);
    screen[0].x = v4.x;
    screen[0].y = v4.y;
    screen[0].z = v4.z;

    v4.x = shape[1].x;
    v4.y = shape[1].y;
    v4.z = shape[1].z;
    v4.w = 1.0f;
    NuVec4MtxTransformVU0(&v4, &v4, &m);
    NuVec4ScaleXYZVU0(&v4, &v4, 1.0f / v4.w);
    screen[1].x = v4.x;
    screen[1].y = v4.y;
    screen[1].z = v4.z;

    v4.x = shape[2].x;
    v4.y = shape[2].y;
    v4.z = shape[2].z;
    v4.w = 1.0f;
    NuVec4MtxTransformVU0(&v4, &v4, &m);
    NuVec4ScaleXYZVU0(&v4, &v4, 1.0f / v4.w);
    screen[2].x = v4.x;
    screen[2].y = v4.y;
    screen[2].z = v4.z;

    v4.x = shape[3].x;
    v4.y = shape[3].y;
    v4.z = shape[3].z;
    v4.w = 1.0f;
    NuVec4MtxTransformVU0(&v4, &v4, &m);
    NuVec4ScaleXYZVU0(&v4, &v4, 1.0f / v4.w);
    screen[3].x = v4.x;
    screen[3].y = v4.y;
    screen[3].z = v4.z;

    rndrstream_3d = (struct rndrstream_s *)((u8 *)mat + 0x124);
    NuRndrStreamLink(rndrstream_3d);
    rndrstream_3d->cur = vpDmaTag_Cnt(rndrstream_3d->cur);
    RS3D_PUSH_W(0x10000000);
    RS3D_PUSH_W(0x5000000D);
    RS3D_PUSH_D(0x3002400000008004ULL);
    RS3D_PUSH_D(0x512);
    for (i = 0; i < 4; i++) {
        RS3D_PUSH_F(uv[0]);
        RS3D_PUSH_F(uv[1]);
        uv += 2;
        RS3D_PUSH_F(1.0f);
        rndrstream_3d->cur += 4;
        RS3D_PUSH_W(r);
        RS3D_PUSH_W(g & 0xFF);
        RS3D_PUSH_W(b & 0xFF);
        RS3D_PUSH_W(a);
        NuVecConvertToIntVU0(rndrstream_3d->cur, &screen[i]);
        rndrstream_3d->cur += 0xC;
        RS3D_PUSH_W(0);
    }
    rndrstream_3d->cur = vpDmaTag_Close(rndrstream_3d->cur);
    rndrstream_free = vpDmaTag_Next(rndrstream_3d->cur, 0);
}

/* Faithful reconstruction from PS2 asm (state=asm), codegen-divergent (~3%):
 * logic/struct offsets exact -- BoxPol@0x1C stride 0x34 walked across the
 * lpo x lp2 nesting, col switch (movn per lpo==4), col2 = col|(fade<<28) or
 * col|0x80000000. Retail hoists the 0x606060 default color to the prologue and
 * spills it (sp+8) with a specific stack layout gcc here does not reproduce. */
void DrawCrateExplosions(void) {
    struct BoxPol_s *BoxFace;
    f32 *uv;
    s32 lp;
    s32 lpo;
    s32 lp2;
    s32 fade;
    s32 col;

    for (lp = 0; lp < 0x10; lp++) {
        if (BoxExpList[lp].time != 0) {
            BoxFace = BoxExpList[lp].BoxPol;
            for (lpo = 0; lpo < 5; lpo++) {
                switch (BoxExpList[lp].type) {
                case 0x10:
                    col = (lpo == 4) ? 0x10C020 : 0x108020;
                    break;
                case 9:
                    col = (lpo == 4) ? 0x80 : 0x40;
                    break;
                case 10:
                case 11:
                case 12:
                    col = (lpo == 4) ? 0xFF90 : 0xE880;
                    break;
                default:
                    col = (lpo == 4) ? 0x808080 : 0x606060;
                    break;
                }
                uv = uvs;
                for (lp2 = 5; lp2 >= 0; lp2--) {
                    fade = BoxExpList[lp].time - BoxFace->rndfade;
                    if (fade < 7) {
                        if (fade > 0) {
                            AddQuad3DrotXYZ(&BoxFace->pos, D_00592DE8, CrateMat,
                                            BoxFace->ang, uv,
                                            col | (fade << 28));
                        }
                    } else {
                        AddQuad3DrotXYZ(&BoxFace->pos, D_00592DE8, CrateMat,
                                        BoxFace->ang, uv,
                                        col - 0x80000000);
                    }
                    BoxFace++;
                    uv += 8;
                }
            }
        }
    }
}

s32 NewCrateAnimation(struct CrateCube *crate, s32 type, s32 action,
                      s32 random) {
    struct CharacterModel *model;
    s32 i;
    s32 character;

    if ((u32)type > 0x14) {
        return 0;
    }
    character = crate_list[type].character;
    crate->character = -1;
    crate->action = -1;
    if ((u32)character > 0xbe) {
        return 0;
    }
    if ((u32)action > 0x75) {
        return 0;
    }
    i = CRemap[character];
    if (i == -1) {
        return 0;
    }
    model = &CModel[i];
    if (model->anmdata[action] != 0) {
        i = 1;
        crate->anim_duration = model->anmdata[action]->time;
        crate->anim_cycle = model->animlist[action]->flags & 1;
        crate->anim_speed = model->animlist[action]->speed;
    } else if (model->fanmdata[action] != 0) {
        i = 1;
        crate->anim_duration = model->fanmdata[action]->time;
        crate->anim_cycle = model->fanimlist[action]->flags & 1;
        crate->anim_speed = model->fanimlist[action]->speed;
    } else {
        i = 0;
    }
    if (i != 0) {
        crate->anim_time = 1.0f;
        if (random != 0) {
            crate->anim_time = crate->anim_time +
                               (s32)qrand() * D_0062D714 *
                                   (crate->anim_duration - 1.0f);
            if (crate->anim_duration <= crate->anim_time) {
                crate->anim_time = 1.0f;
            }
        }
        crate->character = character;
        crate->action = action;
    }
    return i;
}

void InitCrates(void) {
    s32 i;

    crates = NuLstCreate(0x100, 0x28);
    CRATEGROUPCOUNT = 0;
    CRATECOUNT = 0;
    if (crate_scene != 0) {
        for (i = 0; i < 29; i++) {
            NuSpecialFind(crate_scene, &crate_list[i].obj, crate_list[i].name);
        }
    } else {
        for (i = 0; i < 0x1d; i++) {
            crate_list[i].obj.special = 0;
        }
    }
    MarkerCrate.type[0] = 0x1a;
    marker_crate = &MarkerCrate;
    LockCrate.type[0] = 0x1b;
    lock_crate = &LockCrate;
    HighlightCrate.type[0] = 0x1c;
    highlight_crate = &HighlightCrate;
    FlashCrate.type[0] = current_selected_crate;
    flash_crate = &FlashCrate;
    locked_crate = 0;
    triggerorigin_crate = 0;
    triggerdest_crate = 0;
    highlighted_crate = 0;
}

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

struct crate_s *AddCrate(s32 type, struct nuvec_s *pos) {
    struct crate_s *crate;

    crate = (struct crate_s *)NuLstAlloc(crates);
    if (crate != 0) {
        memset(crate, 0, 0x28);
        crate->pos = *pos;
        crate->type[1] = type;
        crate->type[3] = -1;
        crate->type[0] = type;
        crate->type[2] = -1;
        crate->draw = 1;
        crate->orientation = 0;
        crate->linked = 0;
        crate->offz = 0;
        crate->offy = 0;
        crate->offx = 0;
        num_crates_used++;
    }
    return crate;
}

void DestroyCrate(struct crate_s *crate) {
    NuLstFree((struct nulnkhdr_s *)crate);
    num_crates_used--;
}

extern struct numtx_s mTEMP;
extern void NuMtxSetRotationY(struct numtx_s *m, s32 rot);
extern void NuMtxTranslate(struct numtx_s *m, struct nuvec_s *v);
extern s32 NuRndrGScnObj(void *gobj, struct numtx_s *m);

void DrawCrate(struct crate_s *crate) {
    struct nuspecial_s *special;

    if (crate_scene != 0) {
        special = crate_list[crate->type[0]].obj.special;
        if (special != 0) {
            NuMtxSetRotationY(&mTEMP, crate->orientation);
            NuMtxTranslate(&mTEMP, &crate->pos);
            NuRndrGScnObj(crate_scene->gobjs[special->instance->objid], &mTEMP);
        }
    }
}

struct CrateCube *HitCrateBalloons(struct nuvec_s *pos, f32 radius) {
    struct CrateCubeGroup *group;
    struct CrateCube *crate;
    f32 r2;
    s32 iVar1;
    s32 iVar3;
    struct nuvec_s v;
    struct nuvec_s d;

    temp_pGroup = 0;
    temp_pCrate = 0;
    if (level_part_2 != 0) {
        return temp_pCrate;
    }
    group = CrateGroup;
    r2 = (radius + CRATEBALLOONRADIUS);
    r2 *= r2;
    for (iVar1 = 0; iVar1 < CRATEGROUPCOUNT; iVar1++, group++) {
        crate = Crate + group->iCrate;
        for (iVar3 = 0; iVar3 < group->nCrates; iVar3++, crate++) {
            if ((crate->on != 0) && ((crate->flags & 0x400) != 0)) {
                v.x = crate->pos.x;
                v.y = crate->pos.y + CRATEBALLOONOFFSET;
                v.z = crate->pos.z;
                NuVecSub(&d, pos, &v);
                if ((d.x * d.x + d.y * d.y + d.z * d.z) < r2) {
                    temp_crate_type = GetCrateType(crate, 0);
                    temp_pGroup = group;
                    temp_pCrate = crate;
                    GameSfx(0x50, &v);
                    return temp_pCrate;
                }
            }
        }
    }
    return temp_pCrate;
}

s32 WipeCrates(s32 iRAIL0, s32 iALONG0, f32 fALONG0, s32 iRAIL1, s32 iALONG1,
               f32 fALONG1, s32 destroy) {
    struct CrateCubeGroup *group;
    struct CrateCube *crate;
    s32 i;
    s32 j;
    s32 type;

    group = CrateGroup;
    for (i = 0; i < CRATEGROUPCOUNT; i++, group++) {
        crate = &Crate[group->iCrate];
        for (j = 0; j < group->nCrates; j++, crate++) {
            if (crate->on != 0) {
                type = GetCrateType(crate, 0);
                if ((u32)(type + 1) > 1 &&
                    (FurtherALONG(crate->iRAIL, crate->iALONG, crate->fALONG,
                                  iRAIL0, iALONG0, fALONG0) != 0) &&
                    (FurtherALONG(iRAIL1, iALONG1, fALONG1, crate->iRAIL,
                                  crate->iALONG, crate->fALONG) != 0) &&
                    ((destroy == 1) || ((destroy == 2 && (type != 7)) &&
                                        (type != 0xe && (type != 0x11))))) {
                    BreakCrate(group, crate, type, 0x200);
                }
            }
        }
    }
    return 0;
}

s32 HitCrates(struct obj_s *obj, s32 destroy) {
    if ((level_part_2 == 0) &&
        (InCrate(obj->pos.x, obj->pos.z, (obj->pos.y + obj->top * obj->SCALE),
                 (obj->pos.y + obj->bot * obj->SCALE), obj->RADIUS) != 0)) {
        if ((destroy == 1) ||
            ((((destroy == 2 && (temp_crate_type != 7)) &&
               (temp_crate_type != 0xe)) && (temp_crate_type != 0x11)))) {
            BreakCrate(temp_pGroup, temp_pCrate, temp_crate_type,
                       (u16)obj->attack);
        }
        return 1;
    } else {
        return 0;
    }
}

void StartExclamationCrateSequence(struct CrateCubeGroup *group,
                                   struct CrateCube *crate) {
    struct nuvec_s pos;

    pos.x = crate->pos.x;
    pos.y = crate->pos.y + 0.25f;
    pos.z = crate->pos.z;
    temp_pGroup = group;
    temp_pCrate = crate;
    AddKaboom(0x20, &pos, 0.0f);
    crate->newtype = 0xf;
    crate->metal_count = 1;
    GameSfx(0x35, &temp_pCrate->pos);
}

s32 LowestCrate(struct CrateCubeGroup *group, struct CrateCube *crate) {
    struct CrateCube *crate2;
    s32 i;
    s32 dx;
    s32 dz;

    crate2 = &Crate[group->iCrate];
    dx = crate->dx;
    dz = crate->dz;
    for (i = 0; i < group->nCrates; i++, crate2++) {
        if ((((crate2 != crate) && (crate2->dx == dx)) && (crate2->dz == dz)) &&
            ((crate2->pos0).y < (crate->pos0).y)) {
            return 0;
        }
    }
    return 1;
}

extern s32 DESTRUCTIBLECRATECOUNT;
extern s32 DESTRUCTIBLEBONUSCRATECOUNT;
extern s32 Bonus;
extern s32 bonus_restart;
extern s32 cp_iRAIL;
extern s32 cp_iALONG;
extern u8 temp_iRAIL;
extern s16 temp_iALONG;
extern f32 temp_fALONG;
extern u16 temp_xrot;
extern u16 temp_zrot;
extern struct RPos_s gempath_RPos;
extern struct nuvec_s ShadNorm;
extern f32 EShadY;

/* Rail entry stride 0x28; type (+0x26) verified in AheadOfCheckpoint. */
struct rail_s {
    void *spline;            /* 0x00 */
    u8 pad_04[0x26 - 0x04];  /* 0x04 */
    s8 type;                 /* 0x26 */
    u8 pad_27[1];            /* 0x27 */
}; /* 0x28 */
extern struct rail_s Rail[];

struct tersurface_s {
    f32 friction;            /* 0x0 */
    s16 unk_0x04;            /* 0x4 */
    u16 flags;               /* 0x6 */
}; /* 0x8 */
extern struct tersurface_s TerSurface[];

extern void GetALONG(struct nuvec_s *pos, s32 a, s32 b, s32 c, s32 d);
extern s32 AheadOfCheckpoint(s32 iRAIL, s32 iALONG, f32 fALONG);
extern f32 NewShadowMaskPlat(struct nuvec_s *pos, f32 y0, s32 flag);
extern void FindAnglesZX(struct nuvec_s *n);
extern s32 ShadowInfo(void);
extern s32 EShadowInfo(void);
extern void NewScanInit(void);
extern s32 NewScanHandel(struct nuvec_s *pos, struct nuvec_s *size, s32 a,
                         s32 b, f32 t);
extern s32 NewRayCastSetHandel(struct nuvec_s *vpos, struct nuvec_s *vvel,
                               f32 size, f32 timeadj, f32 impactadj,
                               s32 handel);
extern void ResetKabooms(void);
extern void UpdateCrates(void);

void ResetCrates(void) {
    struct nuvec_s p;
    struct nuvec_s off;
    struct nuvec_s norm;
    struct nuvec_s dir;
    struct nuvec_s v = {0.25f, 0.0f, 0.25f};
    s32 eshady;
    struct CrateCubeGroup *group;
    struct CrateCube *crate;
    struct CrateCube *crate2;
    struct CrateCube *below;
    f32 d;
    s32 i;
    s32 j;
    s32 k;
    u16 bits;
    s32 einfo;
    s32 icrate;
    s32 gemahead;
    s32 surf;
    s32 esh;
    s32 handel;
    s32 mode;
    s32 lowest;
    s32 dy;

    icrate = 0;
    gemahead = 0;
    DESTRUCTIBLECRATECOUNT = 0;
    DESTRUCTIBLEBONUSCRATECOUNT = 0;
    if (Rail[7].type == 3) {
        gemahead = AheadOfCheckpoint(gempath_RPos.iRAIL, gempath_RPos.iALONG,
                                     gempath_RPos.fALONG) != 0;
    }
    group = CrateGroup;
    for (i = 0; i < CRATEGROUPCOUNT; i++, group++) {
        NuVecRotateY(&off, &v, group->angle);
        crate = &Crate[group->iCrate];
        for (j = 0; j < group->nCrates; j++, crate++) {
            if (crate->type2 == -1) {
                crate->type2 = crate->type1;
                if (crate->model != 0) {
                    crate->model->type[1] = crate->type1;
                }
            }
            crate->index = icrate;
            icrate++;
            crate->pos.x = crate->pos0.x + off.x;
            crate->pos.z = crate->pos0.z + off.z;
            bits = 0;
            GetALONG(&crate->pos0, 0, -1, -1, 1);
            crate->iRAIL = temp_iRAIL;
            crate->iALONG = temp_iALONG;
            crate->fALONG = temp_fALONG;
            crate->timer = 0.0f;
            if (crate->iRAIL != -1) {
                if (Rail[crate->iRAIL].type == 1) {
                    bits = 0x40;
                } else if (Rail[crate->iRAIL].type == 2) {
                    bits = 0x80;
                } else {
                    bits = (Rail[crate->iRAIL].type == 3) ? 0x100 : 0;
                }
            }
            if (crate->type1 == 8) {
                crate->i = 0;
                crate->duration = 1.0f;
            }
            if (((LDATA->flags & 0x200) != 0) || (Level == 0x1D)) {
                if (Level != 0x1E) {
                    bits |= 0xC00;
                }
            }
            switch (crate->type1) {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 14:
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
                if ((crate->iRAIL != -1) || ((bits & 0x400) != 0)) {
                    bits |= 0x10;
                }
                break;
            case 0:
                if ((crate->iRAIL != -1) || ((bits & 0x400) != 0)) {
                    if (crate->type3 != -1) {
                        if (crate->type3 != 0) {
                            if ((crate->type3 != 13) && (crate->type3 != 15)) {
                                if ((crate->trigger != -1) &&
                                    (Crate[crate->trigger].type1 == 14)) {
                                    bits |= 0x10;
                                }
                            }
                        }
                    }
                }
                break;
            }
            if ((bits & 0x10) != 0) {
                DESTRUCTIBLECRATECOUNT++;
            }
            if ((bits & 0x40) != 0) {
                if ((bits & 0x10) != 0) {
                    DESTRUCTIBLEBONUSCRATECOUNT++;
                }
                if (Bonus != 4) {
                    goto reset;
                }
                goto noreset;
            } else if ((bits & 0x100) != 0) {
                if (gemahead != 0) {
                    goto reset;
                }
                goto noreset;
            } else {
                if (((cp_iRAIL != -1) && (cp_iALONG != -1)) &&
                    ((crate->type1 == 7) && (TimeTrial == 0))) {
                    goto noreset;
                }
                if (bonus_restart != 0) {
                    goto noreset;
                }
                if (AheadOfCheckpoint(crate->iRAIL, crate->iALONG,
                                      crate->fALONG) == 0) {
                    goto noreset;
                }
            }
        reset:
            crate->on = 1;
            crate->pos.y = crate->pos0.y;
            crate->subtype = -1;
            crate->oldy = crate->pos0.y;
            crate->mom = 0.0f;
            crate->newtype = -1;
            if (((crate->type1 == 6) || (crate->type2 == 6)) ||
                ((crate->type1 == 0) && (crate->type3 == 6))) {
                crate->counter = 10;
            } else {
                crate->counter = 0;
            }
            crate->metal_count = 0;
            crate->action = -1;
            crate->appeared = 0;
        noreset:
            if (crate->type1 == 3) {
                NewCrateAnimation(crate, 3, 0x22, 1);
            }
            p.x = crate->pos.x;
            p.y = crate->pos0.y;
            p.z = crate->pos.z;
            crate->shadow = NewShadowMaskPlat(&p, 0.0f, -1);
            if (Level == 0x12) {
                if (crate->shadow < 0.0f) {
                    crate->shadow = 0.0f;
                }
            }
            if (crate->shadow != 2000000.0f) {
                norm = ShadNorm;
                FindAnglesZX(&norm);
                crate->surface_zrot = temp_zrot;
                crate->surface_xrot = temp_xrot;
                surf = ShadowInfo();
                if ((TerSurface[surf].flags & 1) == 0) {
                    bits |= 0x2000;
                }
                eshady = (s32)EShadY;
                if ((f32)eshady == 2000000.0f) {
                    esh = -1;
                } else {
                    esh = 0;
                    einfo = EShadowInfo();
                    if ((u32)einfo < 0xB) {
                        esh = einfo;
                    }
                }
                NewScanInit();
                crate->colbox[0].y = crate->shadow;
                crate->colbox[1].y = crate->shadow + 4.0f;
                dir.x = p.x - 1.5f;
                dir.z = p.z - 1.5f;
                norm.y = 0.0f;
                dir.y = crate->shadow + 0.2f;
                norm.z = 3.0f;
                norm.x = 3.0f;
                handel = NewScanHandel(&dir, &norm, 1, 0, 0.0f);
                dir.x = -1.5f;
                dir.y = 0.0f;
                dir.z = 0.0f;
                p.y = crate->shadow + 0.2f;
                NewRayCastSetHandel(&p, &dir, 0.0f, 0.0f, 0.0f, handel);
                crate->colbox[0].x = dir.x + p.x;
                dir.z = -1.5f;
                dir.x = 0.0f;
                dir.y = 0.0f;
                NewRayCastSetHandel(&p, &dir, 0.0f, 0.0f, 0.0f, handel);
                crate->colbox[0].z = dir.z + p.z;
                dir.x = 1.5f;
                dir.y = 0.0f;
                dir.z = 0.0f;
                NewRayCastSetHandel(&p, &dir, 0.0f, 0.0f, 0.0f, handel);
                crate->colbox[1].x = dir.x + p.x;
                dir.z = 1.5f;
                dir.x = 0.0f;
                dir.y = 0.0f;
                NewRayCastSetHandel(&p, &dir, 0.0f, 0.0f, 0.0f, handel);
                crate->colbox[1].z = dir.z + p.z;
            } else {
                crate->surface_xrot = 0;
                surf = 0;
                crate->surface_zrot = 0;
                esh = -1;
            }
            if ((TerSurface[surf].flags & 4) != 0) {
                bits |= 0x200;
            }
            lowest = 1;
            crate2 = &Crate[group->iCrate];
            for (k = 0; k < group->nCrates; k++, crate2++) {
                if (((crate2 != crate) && (crate2->dx == crate->dx)) &&
                    ((crate2->dz == crate->dz) &&
                     (crate2->pos0.y < crate->pos0.y))) {
                    lowest = 0;
                    break;
                }
            }
            if (lowest != 0) {
                if (crate->shadow == 2000000.0f) {
                    bits |= 0x1000;
                } else if ((crate->pos.y - crate->shadow) > 2.0f) {
                    bits |= 0x1000;
                } else if ((TerSurface[surf].flags & 1) != 0) {
                    bits |= 0x1000;
                } else if ((esh != -1) && ((f32)eshady < crate->pos.y)) {
                    bits |= 0x1000;
                }
            }
            crate->flags = bits;
            if (crate->shadow != 2000000.0f) {
                d = crate->pos0.y - crate->shadow;
                if (d < -0.1f) {
                    crate->flags = bits | 4;
                } else if (d < 0.1f) {
                    crate->flags = bits | 2;
                    if (((crate->type1 == 16) || (crate->type1 == 3)) ||
                        ((crate->type2 == 16) || (crate->type2 == 3))) {
                        crate->flags |= 1;
                    }
                }
            }
        }
        crate = &Crate[group->iCrate];
        for (j = 0; j < group->nCrates; j++, crate++) {
            if ((crate->flags & 6) == 0) {
                below = 0;
                dy = crate->dy;
                for (;;) {
                    dy--;
                    crate2 = &Crate[group->iCrate];
                    for (k = 0; k < group->nCrates; k++, crate2++) {
                        if (((crate2->dx == crate->dx) && (crate2->dy == dy)) &&
                            (crate2->dz == crate->dz)) {
                            goto founddown;
                        }
                    }
                    crate2 = 0;
                founddown:
                    if (crate2 == 0) {
                        break;
                    }
                    below = crate2;
                }
                if ((below != 0) && ((below->flags & 6) != 0)) {
                    crate->flags |= 1;
                }
            }
            if ((crate->type1 == 7) && (crate->shadow != 2000000.0f)) {
                goto surfrot;
            }
            if ((crate->flags & 4) != 0) {
                crate2 = &Crate[group->iCrate];
                for (k = 0; k < group->nCrates; k++, crate2++) {
                    if (((crate2->dx == crate->dx) &&
                         (crate2->dy == crate->dy + 1)) &&
                        (crate2->dz == crate->dz)) {
                        goto foundup1;
                    }
                }
                crate2 = 0;
            foundup1:
                if (crate2 == 0) {
                    crate->xrot0 = (qrand() - 0x8000) / 16;
                    crate->zrot0 = (qrand() - 0x8000) / 16;
                    goto rotdone;
                }
            }
            if ((crate->flags & 2) != 0) {
                crate2 = &Crate[group->iCrate];
                for (k = 0; k < group->nCrates; k++, crate2++) {
                    if (((crate2->dx == crate->dx) &&
                         (crate2->dy == crate->dy + 1)) &&
                        (crate2->dz == crate->dz)) {
                        goto foundup2;
                    }
                }
                crate2 = 0;
            foundup2:
                if (crate2 == 0) {
                    goto surfrot;
                }
            }
            crate->xrot0 = 0;
            crate->zrot0 = 0;
            goto rotdone;
        surfrot:
            crate->xrot0 = crate->surface_xrot;
            crate->zrot0 = crate->surface_zrot;
        rotdone:
            crate->xrot = crate->xrot0;
            crate->zrot = crate->zrot0;
            if (crate->type1 == 6) {
                mode = 1;
            } else if (crate->type2 == 6) {
                mode = 2;
            } else if (crate->type1 != 0) {
                mode = 0;
            } else {
                mode = (crate->type3 == 6) ? 3 : 0;
            }
            if (mode != 0) {
                crate2 = &Crate[group->iCrate];
                for (k = 0; k < group->nCrates; k++, crate2++) {
                    if (((crate2 != crate) && (crate2->dx == crate->dx)) &&
                        (crate2->dz == crate->dz)) {
                        if (mode == 1) {
                            if ((crate2->type1 == 6) ||
                                ((crate2->dy < crate->dy) &&
                                 ((crate2->type1 == 4) ||
                                  (crate2->type1 == 13)))) {
                                crate->flags |= 0x20;
                                break;
                            }
                        }
                        if (mode == 2) {
                            if ((crate2->type2 == 6) ||
                                ((crate2->dy < crate->dy) &&
                                 ((crate2->type2 == 4) ||
                                  (crate2->type2 == 13)))) {
                                crate->flags |= 0x20;
                                break;
                            }
                        }
                        if (mode == 3) {
                            if (((crate2->type1 == 0) &&
                                 (crate2->trigger == crate->trigger)) &&
                                ((crate2->type3 == 6) ||
                                 ((crate2->dy < crate->dy) &&
                                  ((crate2->type3 == 4) ||
                                   (crate2->type3 == 13))))) {
                                crate->flags |= 0x20;
                                break;
                            }
                        }
                    }
                }
                crate->counter = crate->counter / 2;
            }
        }
    }
    ResetKabooms();
    UpdateCrates();
    plr_invisibility_time = 5.0f;
    glass_mix = (Level == 0x17) ? WATERBOSSGLASSMIX : 0.0f;
    glass_col_mix = 0;
    glass_enabled = 0;
    glass_col_enabled = 0;
}

extern s32 DRAWCRATESHADOWS;
extern s32 editor_active;
extern s32 edcrtDrawType;
extern u32 GlobalTimer[];
extern struct gamecam_s *pCam;

/* Global object table (scene + placed special per object id), stride 0x20. */
struct objtab_s {
    struct nugscn_s *scene;      /* 0x00 */
    struct nuspecial_s *special; /* 0x04 */
    u8 pad_08[0x18];             /* 0x08 */
}; /* 0x20 */
extern struct objtab_s ObjTab[];

extern void NuMtxRotateX(struct numtx_s *m, s32 r);
extern void NuMtxRotateZ(struct numtx_s *m, s32 r);
extern f32 **NuHGobjEvalDwa(s32 nlayers, s16 *layer,
                            struct nuanimdata_s *data, f32 time);
extern void NuHGobjEvalAnim(struct NUHGOBJ_s *hobj, struct nuanimdata_s *data,
                            f32 time, s32 nJ, void *pJ,
                            struct numtx_s *tmtx);
extern void NuHGobjEval(struct NUHGOBJ_s *hobj, s32 nJ, void *pJ,
                        struct numtx_s *tmtx);
extern s32 NuHGobjRndrMtxDwa(struct NUHGOBJ_s *hobj, struct numtx_s *mC,
                             s32 nlayers, s16 *layer, struct numtx_s *tmtx,
                             f32 **dwa);
extern s32 LowestActiveCrate(struct CrateCubeGroup *group,
                             struct CrateCube *crate);

void DrawCrates(void) {
    struct nuvec_s v;
    struct numtx_s mtx;
    struct numtx_s mtx2;
    struct numtx_s tmtx[256];
    struct CrateCubeGroup *group;
    struct CrateCube *crate;
    struct CharacterModel *model;
    struct nuspecial_s *special;
    struct numtx_s *pmtx;
    struct objtab_s *ot;
    f32 **dwa;
    f32 range2;
    f32 shadowrange2;
    f32 dist2;
    f32 dx;
    f32 dz;
    f32 d;
    s32 shadows;
    s32 i;
    s32 j;
    s32 type;
    s32 character;

    shadows = 0;
    if ((!((((LDATA->flags & 0x200) != 0) || (Level == 0x1D)) &&
           (VEHICLECONTROL == 1)) &&
         (Level != 0x1C)) &&
        (Level != 3)) {
        range2 = 25.0f;
        d = LDATA->farclip;
        if (d < range2) {
            range2 = d;
        }
        range2 = range2 * range2;
    } else {
        range2 = (f32)(LDATA->farclip * LDATA->farclip);
    }
    if (((DRAWCRATESHADOWS != 0) && (VEHICLECONTROL != 2) &&
         !((VEHICLECONTROL == 1) && (player->obj.vehicle == 0x20))) &&
        (((LDATA->flags & 0x1000) == 0) &&
         !((((LDATA->flags & 0x200) != 0) || (Level == 0x1D)) &&
           (VEHICLECONTROL == 1)) &&
         (Level != 0xB))) {
        shadowrange2 = range2;
        shadows = 1;
    }
    group = CrateGroup;
    for (i = 0; i < CRATEGROUPCOUNT; i++, group++) {
        NuMtxSetRotationY(&mtx, group->angle);
        crate = &Crate[group->iCrate];
        for (j = 0; j < group->nCrates; j++, crate++) {
            crate->in_range = 0;
            if (editor_active != 0) {
                switch (edcrtDrawType) {
                case 0:
                    type = crate->type1;
                    break;
                case 1:
                    type = crate->type2;
                    break;
                case 2:
                    type = crate->type3;
                    break;
                case 3:
                    type = crate->type4;
                    break;
                }
                if (type == -1) {
                    type = ((GlobalTimer[0] % 10) > 4) ? type : 0;
                }
            } else {
                type = GetCrateType(crate, 1);
            }
            if (type == -1) {
                continue;
            }
            if ((crate->on == 0) && (type != 7)) {
                continue;
            }
            if (!((((LDATA->flags & 0x200) != 0) || (Level == 0x1D)) &&
                  (VEHICLECONTROL == 1))) {
                dx = pCam->pos.x - crate->pos.x;
                dz = pCam->pos.z - crate->pos.z;
                dist2 = dx * dx + dz * dz;
                if ((editor_active == 0) && (range2 < dist2)) {
                    continue;
                }
            }
            v.y = crate->pos.y + 0.25f;
            v.x = crate->pos.x;
            v.z = crate->pos.z;
            crate->in_range = 1;
            if ((crate->xrot != 0) || (crate->zrot != 0)) {
                pmtx = &mtx2;
                NuMtxSetRotationY(pmtx, group->angle);
                NuMtxRotateZ(pmtx, crate->zrot);
                NuMtxRotateX(pmtx, crate->xrot);
            } else {
                pmtx = &mtx;
            }
            pmtx->_30 = v.x;
            pmtx->_31 = v.y;
            pmtx->_32 = v.z;
            if ((crate->flags & 0x400) != 0) {
                j = (type == 5) ? 0x43 : 0x41;
                ot = &ObjTab[j];
                if (ot->special != 0) {
                    NuRndrGScnObj(ot->scene->gobjs[ot->special->instance->objid],
                                  pmtx);
                }
            }
            character = crate_list[type].character;
            if (((u32)character < 0xBF) && (CRemap[character] != -1)) {
                model = &CModel[CRemap[character]];
                if ((crate->action != -1) && (crate->character == character)) {
                    if (model->fanmdata[crate->action] != 0) {
                        dwa = NuHGobjEvalDwa(1, 0,
                                             model->fanmdata[crate->action],
                                             crate->anim_time);
                    } else {
                        dwa = 0;
                    }
                    if (model->anmdata[crate->action] != 0) {
                        NuHGobjEvalAnim(model->hobj,
                                        model->anmdata[crate->action],
                                        crate->anim_time, 0, 0, tmtx);
                    } else {
                        NuHGobjEval(model->hobj, 0, 0, tmtx);
                    }
                } else {
                    NuHGobjEval(model->hobj, 0, 0, tmtx);
                    dwa = 0;
                }
                NuHGobjRndrMtxDwa(model->hobj, pmtx, 1, 0, tmtx, dwa);
                if ((crate->flags & 0x200) != 0) {
                    mtx2 = *pmtx;
                    mtx2._01 = -mtx2._01;
                    mtx2._11 = -mtx2._11;
                    mtx2._21 = -mtx2._21;
                    mtx2._31 = crate->shadow - (mtx2._31 - crate->shadow);
                    NuHGobjRndrMtxDwa(model->hobj, &mtx2, 1, 0, tmtx, dwa);
                }
            } else {
                special = crate_list[type].obj.special;
                if (special != 0) {
                    NuRndrGScnObj(crate_scene->gobjs[special->instance->objid],
                                  pmtx);
                    if ((crate->flags & 0x200) != 0) {
                        mtx2 = *pmtx;
                        mtx2._01 = -mtx2._01;
                        mtx2._11 = -mtx2._11;
                        mtx2._21 = -mtx2._21;
                        mtx2._31 = crate->shadow - (mtx2._31 - crate->shadow);
                        NuRndrGScnObj(
                            crate_scene->gobjs[special->instance->objid],
                            &mtx2);
                    }
                }
            }
            if ((((shadows != 0) && (type != 7)) &&
                 ((type != 0) && ((crate->flags & 0x2000) != 0))) &&
                ((((crate->shadow != 2000000.0f) &&
                   (crate->shadow < crate->pos.y)) &&
                  (dist2 < shadowrange2)) &&
                 ((ObjTab[21].special != 0) &&
                  (LowestActiveCrate(group, crate) != 0)))) {
                NuMtxSetRotationY(&mtx2, group->angle);
                NuMtxRotateZ(&mtx2, crate->surface_zrot);
                NuMtxRotateX(&mtx2, crate->surface_xrot);
                mtx2._30 = crate->pos.x;
                mtx2._31 = crate->shadow + 0.025f;
                mtx2._32 = crate->pos.z;
                NuRndrGScnObj(ObjTab[21]
                                  .scene->gobjs[ObjTab[21]
                                                    .special->instance->objid],
                              &mtx2);
            }
        }
    }
}

s32 LowestActiveCrate(struct CrateCubeGroup *group, struct CrateCube *crate) {
    struct CrateCube *crate2;
    s32 i;
    s32 dx;
    s32 dz;

    crate2 = &Crate[group->iCrate];
    dx = crate->dx;
    dz = crate->dz;
    for (i = 0; i < group->nCrates; i++, crate2++) {
        if ((((crate2 != crate) && (crate2->on != 0)) && (crate2->dx == dx)) &&
            ((crate2->dz == dz && ((crate2->pos).y < (crate->pos).y)))) {
            return 0;
        }
    }
    return 1;
}

s32 CrateInTheWay(struct obj_s *obj, struct CrateCubeGroup *group,
                  struct CrateCube *crate, s32 dx, s32 dz, char *hit) {
    struct CrateCube *crate2;
    s32 i;

    crate2 = &Crate[group->iCrate];
    for (i = 0; i < group->nCrates; i++, crate2++) {
        if ((((crate2 != crate) && (hit[i] == 1)) && (crate2->dx == dx)) &&
            (crate2->dz == dz)) {
            return 1;
        }
    }
    return 0;
}

/* Faithful near-match (state=asm): structure/offsets exact vs retail, but the
 * `+ 0.5f` FP-equality inner loop assembles one instruction over the extent
 * (decompals-as loop .p2align / FP-hazard nop). Same class as the documented
 * .org-backwards wall; kept for reference. */
s32 CrateOnTop(struct CrateCubeGroup *group, struct CrateCube *crate) {
    s32 i;
    struct CrateCube *crate2;

    crate2 = &Crate[group->iCrate];
    for (i = 0; i < group->nCrates; i++, crate2++) {
        if ((((crate2 != crate) && (crate2->on != 0)) &&
             (GetCrateType(crate2, 0) != 0)) &&
            (((crate2->dx == crate->dx && (crate2->dz == crate->dz)) &&
              (crate2->pos.y == crate->pos.y + 0.5f)))) {
            return 1;
        }
    }
    return 0;
}

/* Faithful near-match (state=asm): same `+ 0.5f` inner-loop assembler wall as
 * CrateOnTop; structure (outer goto restart, column match) is exact. */
void HopCratesAbove(f32 speed, struct CrateCubeGroup *group,
                    struct CrateCube *crate) {
    struct CrateCube *crate2;
    s32 i;

Loop:
    crate2 = &Crate[group->iCrate];
    for (i = 0; i < group->nCrates; i++, crate2++) {
        if (((crate2->on != 0) && (crate2->dx == crate->dx) &&
             (crate2->dz == crate->dz)) &&
            (crate->pos.y + 0.5f == crate2->pos.y)) {
            crate2->mom = speed;
            crate = crate2;
            goto Loop;
        }
    }
}

void ResetCrateType2(struct CrateCube *crt) {
    crt->type2 = crt->type1;
    if (crt->model == 0) {
        return;
    }
    crt->model->type[1] = crt->type1;
}

/* Cap test for an upright cylinder: face 0x20 is the cap the ray crosses on
 * the way up, 0x10 the one it crosses on the way down. */
s32 RayIntersectCylinder(struct nuvec_s *p0, struct nuvec_s *p1, f32 x, f32 z,
                         f32 ybot, f32 ytop, f32 radius) {
    f32 ratio;
    f32 r2;
    f32 dx;
    f32 dy;
    f32 dz;
    s32 face;
    struct nuvec_s v;

    r2 = radius * radius;
    dx = p1->x - p0->x;
    dy = p1->y - p0->y;
    dz = p1->z - p0->z;
    if (p0->y <= ybot && ybot <= p1->y) {
        face = 0x20;
        v.y = ybot;
        ratio = (ybot - p0->y) / dy;
    } else if (ytop <= p0->y && p1->y <= ytop) {
        face = 0x10;
        v.y = ytop;
        ratio = (ytop - p0->y) / dy;
    } else {
        face = 0;
    }
    if (face != 0) {
        v.x = p0->x + dx * ratio;
        v.z = p0->z + dz * ratio;
        dx = v.x - x;
        dz = v.z - z;
        if ((dx * dx) + (dz * dz) <= r2) {
            temp_face = face;
            temp_ratio = ratio;
            vTEMP = v;
            return 1;
        }
    }
    return 0;
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

void ResetAllCrateTypes(s32 type) {
    struct CrateCubeGroup *group;
    struct CrateCube *crate;
    s32 i;
    s32 j;

    if (type == 0) {
        return;
    }
    group = CrateGroup;
    for (i = 0; i < CRATEGROUPCOUNT; i++, group++) {
        crate = &Crate[group->iCrate];
        for (j = 0; j < group->nCrates; j++, crate++) {
            switch (type) {
            case 1:
                crate->type2 = crate->type1;
                if (crate->model != 0) {
                    crate->model->type[1] = crate->type1;
                }
                break;
            case 2:
                crate->type3 = -1;
                break;
            case 3:
                crate->type4 = -1;
                break;
            }
        }
    }
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
