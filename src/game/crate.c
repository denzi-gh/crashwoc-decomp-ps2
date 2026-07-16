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
extern s16 *LDATA;

struct gamecam_s {
    u8 pad_00[0x80];
    struct nuvec_s vX;       /* 0x80 (verified in AddExtraLife) */
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
/* AddQuad3DrotXYZ (0x1F5BF0, state=asm): reconstructed logic is a quad->2-tri
 * render (byte-swizzled colour, NuMtxSetRotateXYZ+Translate, per-vertex
 * pnt/nrm/tc/diffuse), but the GC render callee NuRndrTri3d does not exist on
 * PS2 (unresolved symbol). Left as skeleton pending the PS2 render-primitive
 * name + verified nuvtx_tc1_s layout. */
extern f32 D_0062D798;
extern f32 D_0062D79C;
extern void AddQuad3DrotXYZ(struct nuvec_s *pos, struct nuvec_s *shape,
                            struct numtl_s *mat, struct nuvec_s *a, f32 *uv,
                            u32 col);
extern void *crate_scene;
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
            type = (LDATA[0x13] == 1) ? 0x19 : type;
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
                                            (struct nuvec_s *)BoxFace->ang, uv,
                                            col | (fade << 28));
                        }
                    } else {
                        AddQuad3DrotXYZ(&BoxFace->pos, D_00592DE8, CrateMat,
                                        (struct nuvec_s *)BoxFace->ang, uv,
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
