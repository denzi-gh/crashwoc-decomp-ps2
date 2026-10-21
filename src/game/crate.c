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

#include "nu_libc.h"

#include "nu3d/nucamera.h"
#include "nu3d/nugscn.h"
#include "nu3d/numtl.h"
#include "nu3d/nustream.h"
#include "nu3d/nuvport.h"
#include "numath/nufloat.h"
#include "numath/numtx.h"
#include "numath/nuvec.h"
#include "game/gametypes.h"
#include "gamelib/terrain.h"
#include "game/creature.h"

struct nulsthdr_s;
struct nulnkhdr_s;

extern struct nulnkhdr_s *NuLstGetNext(struct nulsthdr_s *hdr,
                                       struct nulnkhdr_s *lnk);
extern struct nulnkhdr_s *NuLstAlloc(struct nulsthdr_s *hdr);
extern void NuLstFree(struct nulnkhdr_s *lnk);
extern struct nulsthdr_s *NuLstCreate(int num, int size);
extern void NuLstDestroy(struct nulsthdr_s *hdr);

struct CRATETYPEDATA {
    struct cratecube_s *crate; /* 0x00 */
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

extern struct crate_s LockCrate;

static struct nulsthdr_s *crates = 0;
struct crate_s *marker_crate = 0;
struct crate_s *flash_crate = 0;
struct crate_s *lock_crate = &LockCrate;
struct crate_s *locked_crate = 0;
struct crate_s *triggerorigin_crate = 0;
struct crate_s *triggerdest_crate = 0;
struct crate_s *highlight_crate = 0;
struct crate_s *highlighted_crate = 0;
s32 current_selected_crate = 0;
s32 CRATEGROUPCOUNT = 0;
s32 CRATECOUNT = 0;
s32 num_crates_used = 0;
f32 CRATEGRAVITY = -0.0072000003f;
f32 CRATEHOPSPEED = 0.054f;
f32 METALCRATEBOUNCESPEED = 0.168f;
f32 JUMPONCRATEBREAKSPEED = 0.0f;
f32 FALLONCRATEBREAKSPEED = 0.099999994f;
f32 plr_invisibility_time = 0.0f;

struct crateeditor_s crate_list[29] = {
    {{0, 0}, 0, "wireframe_crate", -1},
    {{0, 0}, 1, "crate", -1},
    {{0, 0}, 2, "crash_crate", -1},
    {{0, 0}, 3, "aku_aku_crate", 74},
    {{0, 0}, 4, "uparrow_crate", 72},
    {{0, 0}, 5, "question_crate", -1},
    {{0, 0}, 6, "bouncy_crate", 71},
    {{0, 0}, 7, "checkpoint_crate", 75},
    {{0, 0}, 8, "changer_crate", -1},
    {{0, 0}, 9, "tnt_crate", -1},
    {{0, 0}, 10, "time1_crate", -1},
    {{0, 0}, 11, "time2_crate", -1},
    {{0, 0}, 12, "time3_crate", -1},
    {{0, 0}, 13, "uparrow_steel_crate", 73},
    {{0, 0}, 14, "exclamation_crate", 76},
    {{0, 0}, 15, "steel_crate", -1},
    {{0, 0}, 16, "nitro_crate", -1},
    {{0, 0}, 17, "nitro_exploder_crate", 77},
    {{0, 0}, 18, "proximity_crate", -1},
    {{0, 0}, 19, "reinforced_crate", 78},
    {{0, 0}, 20, "invisibility_crate", -1},
    {{0, 0}, 21, "crate_open", -1},
    {{0, 0}, 22, "tnt3_crate", -1},
    {{0, 0}, 23, "tnt2_crate", -1},
    {{0, 0}, 24, "tnt1_crate", -1},
    {{0, 0}, 25, "coco_crate", -1},
    {{0, 0}, 26, "construction1", -1},
    {{0, 0}, 27, "construction3", -1},
    {{0, 0}, 28, "construction2", -1},
};
s32 DRAWCRATESHADOWS = 1;
f32 tntsfxtime[7] = { 0.7f, 1.4f, 1.8f, 2.2f, 2.4f, 2.6f, 2.8f };
f32 uvs[48] = {
    0.01f, 0.01f, 0.24f, 0.01f, 0.01f, 0.24f, 0.24f, 0.24f, 0.25f, 0.01f,
    0.49f, 0.01f, 0.25f, 0.25f, 0.49f, 0.25f, 0.5f, 0.01f, 0.74f, 0.01f, 0.5f,
    0.25f, 0.74f, 0.25f, 0.01f, 0.25f, 0.24f, 0.25f, 0.01f, 0.5f, 0.24f, 0.5f,
    0.25f, 0.25f, 0.49f, 0.25f, 0.25f, 0.5f, 0.49f, 0.5f, 0.5f, 0.25f, 0.74f,
    0.25f, 0.5f, 0.5f, 0.74f, 0.5f
};

extern struct cratecube_s Crate[];
extern struct CRATETYPEDATA CrateTypeData[];
extern s32 i_cratetypedata;
extern s32 Level;
extern f32 WATERBOSSGLASSMIX;
extern f32 glass_mix;
extern s32 glass_col_mix;
extern s32 glass_enabled;
extern s32 glass_col_enabled;
extern s32 GetCrateType(struct cratecube_s *crt, s32 flags);
extern struct crategroup_s *temp_pGroup;
extern struct cratecube_s *temp_pCrate;
extern void AddKaboom(s32 type, struct nuvec_s *pos, f32 radius);
extern void GameSfx(s32 id, struct nuvec_s *pos);
extern s32 level_part_2;
extern struct cratecube_s *InCrate(f32 x, f32 z, f32 top, f32 bot, f32 radius);
extern void BreakCrate(struct crategroup_s *group, struct cratecube_s *crate,
                       s32 type, s32 attack);
extern struct crategroup_s CrateGroup[];
extern s32 FurtherALONG(s32 iRAIL, s32 iALONG, f32 fALONG, s32 iRAIL2,
                        s32 iALONG2, f32 fALONG2);
extern s32 TimeTrial;

inline struct crate_s *NextCrate(struct crate_s *a) {
    return (struct crate_s *)NuLstGetNext(crates, (struct nulnkhdr_s *)a);
}


extern struct ldata_s *LDATA;

extern struct cammtx_s GameCam[];
extern void AddPanelDebris(f32 x, f32 y, s32 obj, f32 z, s32 flag);
extern u64 fptodp(f32 x);
extern s32 dpcmp(u64 a, u64 b);
extern u64 dpsub(u64 a, u64 b);
extern u64 dpmul(u64 a, u64 b);
extern f32 dptofp(u64 x);

extern s32 qrand(void);

extern struct rndrstream_s *rndrstream_3d;
extern u8 *rndrstream_free;

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
                            struct numtl_s *mtl, s32 *a, f32 *uvs, u32 colour);

extern struct nugscn_s *crate_scene;


s32 GetCrateType(struct cratecube_s *crt, s32 flags) {
    s32 type;

    type = (TimeTrial != 0) ? crt->type2 : crt->type1;
    if (type == 0) {
        type = (crt->newtype != -1) ? crt->newtype : 0;
    }
    if ((flags & 2) && type == 8) {
        if (crt->newtype != -1) {
            goto changed;
        }
    } else if (crt->newtype != -1) {
    changed:
        if (TimeTrial == 0 && crt->subtype != -1) {
            type = crt->subtype;
        } else {
            type = crt->newtype;
        }
    } else if ((TimeTrial == 0 || type == 9) && crt->subtype != -1) {
        type = crt->subtype;
    }
    if (!(flags & 1)) {
        if (type >= 0x16 && type <= 0x18) {
            type = 9;
        }
    } else if (type == 2 && LDATA->character == 1) {
        type = 0x19;
    }
    return type;
}

inline void AddExtraLife(struct nuvec_s *pos, s32 pdeb) {
    struct nuvec_s scr;
    struct nuvec_s cV[2];
    double scale;

    NuCameraTransformScreenClip(&scr, pos, 1, 0);
    cV[0].x = pos->x + GameCam[0].vX.x * 0.1f;
    cV[0].y = pos->y + GameCam[0].vX.y * 0.1f;
    cV[0].z = pos->z + GameCam[0].vX.z * 0.1f;
    NuCameraTransformScreenClip(&cV[1], &cV[0], 1, 0);
    scale = scr.x - cV[1].x;
    if (scale < 0.0) {
        scale = 0.0 - scale;
    }
    AddPanelDebris(scr.x, scr.y, pdeb, scale * 3.6363637f, 1);
}


inline s32 CrateOnTop(struct crategroup_s *group, struct cratecube_s *crate) {
    s32 i;
    struct cratecube_s *crate2;

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


inline void SaveCrateTypeData(struct cratecube_s *crate) {
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

s32 CrateAbove(struct obj_s *obj, struct crategroup_s *group,
               struct cratecube_s *crate) {
    struct cratecube_s *crate2;
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

s32 CrateBelow(struct obj_s *obj, struct crategroup_s *group,
               struct cratecube_s *crate) {
    struct cratecube_s *crate2;
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
extern s32 NewCrateAnimation(struct cratecube_s *crate, s32 character,
                             s32 action, s32 flag);
extern s32 CrateOff(struct crategroup_s *group, struct cratecube_s *crate,
                    s32 a, s32 b);
extern void JudderGameCamera(struct cammtx_s *cam, f32 time, void *pos);

inline void StartExclamationCrateSequence(struct crategroup_s *group,
                                          struct cratecube_s *crate) {
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


inline void HopCratesAbove(f32 speed, struct crategroup_s *group,
                           struct cratecube_s *crate) {
    struct cratecube_s *crate2;
    s32 i;

Loop:
    crate2 = &Crate[group->iCrate];
    for (i = 0; i < group->nCrates; i++, crate2++) {
        if (((crate2->on != 0) && (crate2->dx == crate->dx) &&
             (crate2->dz == crate->dz)) &&
            (crate2->pos.y == crate->pos.y + 0.5f)) {
            crate2->mom = speed;
            crate = crate2;
            goto Loop;
        }
    }
}


inline void DestroyAllNitroCrates(struct crategroup_s *group,
                                  struct cratecube_s *crate) {
    struct crategroup_s *group2;
    struct cratecube_s *crate2;
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
    JudderGameCamera(&GameCam[0], 0.5f, 0);
    GameSfx(0x33, &crate->pos);
}

void OpenPreviousCheckpoints(s32 iRAIL, s32 iALONG, f32 fALONG) {
    struct crategroup_s *group;
    struct cratecube_s *crate;
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

extern struct nuvec_s vTEMP;
extern char tbuf[];
extern char LevelFileName[];
extern struct chase_s Chase[3];
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

void ConvertCrateData(void) {
    struct crate_s *cr;
    struct crate_s *cr2;
    struct cratecube_s *cc;
    struct crategroup_s *crg;
    s32 i;
    s32 j;
    s32 k;
    struct nuvec_s tvec;
    s32 ddx;
    s32 ddy;
    s32 ddz;

    CRATEGROUPCOUNT = 0;
    CRATECOUNT = 0;
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
                    ddx = Crate[k].dx - Crate[j].dx;
                    ddy = Crate[k].dy - Crate[j].dy;
                    ddz = Crate[k].dz - Crate[j].dz;
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

void ReadInCrateData(void) {
    s32 i;
    s32 j;
    struct crate_s *new_crate;
    struct crategroup_s *crg;
    struct cratecube_s *cc;
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
            for (new_crate = NextCrate(0); new_crate != 0;
                 new_crate = NextCrate(new_crate)) {
                if (new_crate->ccindex == Crate[cr2->ccindex].trigger) {
                    cr2->trigger = new_crate;
                    break;
                }
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
    struct cratecube_s tcr;
    struct crategroup_s *crg;

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
    s32 maxversion;

    maxversion = 4;
    CRATECOUNT = 0;
    sprintf(tbuf, "%s.crt", LevelFileName);
    handle = NuMemFileOpen(Chase, NuFileLoadBuffer(tbuf, Chase, 0x7fffffff), 0);
    if (handle != 0) {
        version = NuFileReadInt(handle);
        if (version > maxversion) {
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

s32 i_cratetypedata = 0;
s32 last_questionmark_extralife = 0;
s32 CHANGERPITCH = 941;
s32 CHANGERVOLUME = 16383;
s32 SFX_CHANGER = 122;
s32 CRATEEXPLOSIONFRAMES = 50;
f32 CRATEBALLOONOFFSET = 2.65f;
f32 CRATEBALLOONRADIUS = 1.1f;
s32 cp_goto = -1;
s32 temp_crate_y_ceiling_adjust = 0;
s32 temp_crate_y_floor_adjust = 0;
s32 temp_crate_xz_adjust = 0;
s32 temp_crate_bounce = 0;
s32 DESTRUCTIBLECRATECOUNT = 0;
s32 DESTRUCTIBLEBONUSCRATECOUNT = 0;
struct crategroup_s *temp_pGroup = 0;
struct cratecube_s *temp_pCrate = 0;
struct numtl_s *CrateMat = 0;
s32 temp_crate_type = 0;
s32 temp_face = 0;
f32 temp_ratio = 0.0f;
s32 iBOXEXP = 0;

struct cratecube_s Crate[256] = {0};
struct crategroup_s CrateGroup[128] = {0};
struct CRATETYPEDATA CrateTypeData[32] = {0};
struct nuvec_s cpGOTO = {0};
struct crate_s MarkerCrate = {0};
struct crate_s FlashCrate = {0};
struct crate_s LockCrate = {0};
struct crate_s HighlightCrate = {0};
struct BoxExpType BoxExpList[16] = {0};

s32 RayIntersectCuboid(struct nuvec_s *p0, struct nuvec_s *p1,
                       struct nuvec_s *min, struct nuvec_s *max) {
    struct nuvec_s v;
    f32 ratio;
    f32 dx;
    f32 dy;
    f32 dz;
    s32 face;

    dx = p1->x - p0->x;
    dy = p1->y - p0->y;
    dz = p1->z - p0->z;
    if (p0->x <= min->x && min->x <= p1->x) {
        ratio = (min->x - p0->x) / dx;
        face = 8;
        v.x = min->x;
    } else if (p0->x >= max->x && p1->x <= max->x) {
        ratio = (max->x - p0->x) / dx;
        face = 4;
        v.x = max->x;
    } else {
        face = 0;
    }
    if (face != 0) {
        v.z = p0->z + dz * ratio;
        if (min->z <= v.z && v.z <= max->z) {
            v.y = p0->y + dy * ratio;
            if (min->y <= v.y && v.y <= max->y) {
                temp_face = face;
                temp_ratio = ratio;
                vTEMP = v;
                return 1;
            }
        }
    }
    if (p0->z <= min->z && min->z <= p1->z) {
        ratio = (min->z - p0->z) / dz;
        face = 2;
        v.z = min->z;
    } else if (p0->z >= max->z && p1->z <= max->z) {
        ratio = (max->z - p0->z) / dz;
        face = 1;
        v.z = max->z;
    } else {
        face = 0;
    }
    if (face != 0) {
        v.x = p0->x + dx * ratio;
        if (min->x <= v.x && v.x <= max->x) {
            v.y = p0->y + dy * ratio;
            if (min->y <= v.y && v.y <= max->y) {
                temp_face = face;
                temp_ratio = ratio;
                vTEMP = v;
                return 1;
            }
        }
    }
    if (p0->y <= min->y && min->y <= p1->y) {
        ratio = (min->y - p0->y) / dy;
        face = 0x20;
        v.y = min->y;
    } else if (p0->y >= max->y && p1->y <= max->y) {
        ratio = (max->y - p0->y) / dy;
        face = 0x10;
        v.y = max->y;
    } else {
        face = 0;
    }
    if (face != 0) {
        v.x = p0->x + dx * ratio;
        if (min->x <= v.x && v.x <= max->x) {
            v.z = p0->z + dz * ratio;
            if (min->z <= v.z && v.z <= max->z) {
                temp_face = face;
                temp_ratio = ratio;
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
    struct crategroup_s *group;
    struct cratecube_s *crate;

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

extern s32 KillPlayer(struct obj_s *obj, s32 anim);

void BreakCrate(struct crategroup_s *group, struct cratecube_s *crate, s32 type,
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

s32 AttackCrate(struct obj_s *obj, struct crategroup_s *group,
                struct cratecube_s *crate) {
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

struct cratecube_s *InCrate(f32 x, f32 z, f32 ytop, f32 ybot, f32 radius) {
    struct crategroup_s *group;
    struct cratecube_s *crate;
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
                        if (!(vNew.x < crate->dx * 0.5f - radius) &&
                            !(vNew.x > crate->dx * 0.5f + 0.5f + radius)) {
                            if (!(vNew.z < crate->dz * 0.5f - radius) &&
                                !(vNew.z > crate->dz * 0.5f + 0.5f + radius)) {
                                if (!(ytop < crate->pos.y) &&
                                    !(ybot > crate->pos.y + 0.5f)) {
                                    dx = crate->pos.x - x;
                                    dy = crate->pos.y + 0.25f - ymid;
                                    dz = crate->pos.z - z;
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

extern struct game_s Game;
extern struct nuvec_s vNEWMASK;
extern s32 mask_crates;
extern s32 newmask_advise;
extern f32 TimeTrialWait;
extern void ResetCheckpoint(s32 iRAIL, s32 iALONG, struct nuvec_s *pos,
                            f32 fALONG);
extern void AddGameDebris(s32 type, struct nuvec_s *pos);
extern void AddTempWumpa(struct cratecube_s *crate, s32 n, f32 x, f32 y, f32 z);
extern void AddCrateExplosion(struct nuvec_s *pos, s32 type, s32 ang,
                              struct nuvec_s *colbox);
extern s32 VEHICLECONTROL;
extern void AddScreenWumpa(f32 x, f32 y, f32 z, s32 n);

s32 CrateOff(struct crategroup_s *group, struct cratecube_s *crate, s32 how,
             s32 silent) {
    struct nuvec_s pos;
    s32 type;
    s32 sfx;
    s32 n;

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
            AddExtraLife(&pos, n);
            if (TimeTrial == 0 && (crate->flags & 0x40) == 0) {
                if (crate->type1 == 8 && crate->i > 0) {
                    SaveCrateTypeData(crate);
                    if (crate->i == 1) {
                        crate->type3 = crate->type4;
                    }
                    crate->type4 = -1;
                } else if (crate->type1 == 2) {
                    SaveCrateTypeData(crate);
                    crate->type1 = 5;
                } else if (crate->type1 == 0 && crate->type3 == 2) {
                    SaveCrateTypeData(crate);
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
        AddKaboom(2, &pos, 0.0f);
        AddGameDebris(6, &pos);
        sfx = 0x3B;
        break;
    case 9:
        AddKaboom(1, &pos, 0.0f);
        AddGameDebris(5, &pos);
        sfx = 0x3B;
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
                AddExtraLife(&pos, 2);
                last_questionmark_extralife = 1;
            } else {
                if (last_questionmark_extralife != 0) {
                    last_questionmark_extralife--;
                }
                if (CrateOnTop(group, crate) != 0) {
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

extern f32 NuTrigTable[];

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
s32 CrateBounceReaction(struct crategroup_s *group, struct cratecube_s *crate,
                        s32 type, s32 attack) {
    struct nuvec_s pos;
    struct crategroup_s *group2;
    struct cratecube_s *crate2;
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
        JudderGameCamera(&GameCam[0], 0.5f, 0);
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

void AddCrateExplosion(struct nuvec_s *pos, s32 type, s32 ang,
                       struct nuvec_s *colbox) {
    struct BoxExpType *box;
    struct BoxPol_s *face;
    s32 i;
    s32 j;
    f32 rndang;

    box = &BoxExpList[iBOXEXP];
    face = BoxExpList[iBOXEXP].BoxPol;
    box->type = type;
    box->time = CRATEEXPLOSIONFRAMES;
    box->colbox[0] = colbox[0];
    box->colbox[1] = colbox[1];
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 6; j++, face++) {
            face->rndfade = qrand() / 4096;
            face->ang[0] = 0;
            face->ang[1] = ang + i * 0x4000;
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
    for (j = 0; j < 6; j++, face++) {
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
                face = BoxExpList[i].BoxPol;
                for (j = 0; j < 5; j++) {
                    for (k = 5; k >= 0; k--, face++) {
                        bounce = 0;
                        face->pos.x += face->mom.x;
                        face->pos.y += face->mom.y;
                        face->mom.y -= 0.004f;
                        face->pos.z += face->mom.z;
                        face->ang[0] += face->angmom[0];
                        face->ang[1] += face->angmom[1];
                        face->ang[2] += face->angmom[2];
                        if (face->pos.x < box->colbox[0].x &&
                            face->mom.x < 0.0f) {
                            face->pos.x = box->colbox[0].x;
                            bounce = 1;
                            face->mom.x = -face->mom.x * 0.8f;
                        }
                        if (face->pos.y < box->colbox[0].y &&
                            face->mom.y < 0.0f) {
                            face->pos.y = box->colbox[0].y;
                            bounce = 1;
                            face->mom.y = -face->mom.y * 0.8f;
                        }
                        if (face->pos.z < box->colbox[0].z &&
                            face->mom.z < 0.0f) {
                            face->pos.z = box->colbox[0].z;
                            bounce = 1;
                            face->mom.z = -face->mom.z * 0.8f;
                        }
                        if (face->pos.x > box->colbox[1].x &&
                            face->mom.x > 0.0f) {
                            face->pos.x = box->colbox[1].x;
                            bounce = 1;
                            face->mom.x = -face->mom.x * 0.8f;
                        }
                        if (face->pos.z > box->colbox[1].z &&
                            face->mom.z > 0.0f) {
                            face->pos.z = box->colbox[1].z;
                            bounce = 1;
                            face->mom.z = -face->mom.z * 0.8f;
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

extern struct RPos_s *best_cRPos;

s32 GotoCheckpoint(struct nuvec_s *pos, s32 direction) {
    struct crategroup_s *group;
    struct cratecube_s *crate;
    s32 i;
    s32 j;
    s32 iRAIL;
    s32 iALONG;
    f32 fALONG;

    cp_goto = -1;
    if (best_cRPos == 0) {
        return 0;
    }
    if ((u32)direction > 1) {
        return 0;
    }
    group = CrateGroup;
    iRAIL = -1;
    for (i = 0; i < CRATEGROUPCOUNT; i++, group++) {
        crate = &Crate[group->iCrate];
        for (j = 0; j < group->nCrates; j++, crate++) {
            if (crate->type1 == 7 && NuVecDist(pos, &crate->pos0, 0) > 5.0f &&
                ((direction == 0 &&
                  FurtherALONG(crate->iRAIL, crate->iALONG, crate->fALONG,
                               best_cRPos->iRAIL, best_cRPos->iALONG,
                               best_cRPos->fALONG) != 0) ||
                 (direction == 1 &&
                  FurtherBEHIND(crate->iRAIL, crate->iALONG, crate->fALONG,
                                best_cRPos->iRAIL, best_cRPos->iALONG,
                                best_cRPos->fALONG) != 0)) &&
                (iRAIL == -1 ||
                 (direction == 0 &&
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
    if (iRAIL != -1) {
        cp_goto = direction;
        return 1;
    }
    return 0;
}

f32 CrateTopBelow(struct nuvec_s *pos) {
    struct crategroup_s *group;
    struct cratecube_s *crate;
    s32 i;
    s32 j;
    f32 top;
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
                            if (y == 2000000.0f || y < top) {
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
    struct crategroup_s *group;
    struct cratecube_s *crate;
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

void CrateSafety(struct crategroup_s *group, struct cratecube_s *crate,
                 struct obj_s *obj) {
    struct cratecube_s *crate2;
    struct cratecube_s *crate3;
    f32 size;
    s32 i;

    size = (obj->top - obj->bot) * obj->SCALE;
    if (obj->pos.y + (obj->bot + obj->top) * obj->SCALE * 0.5f <
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
        if ((crate3->shadow != 2000000.0f) &&
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

void AddQuad3DrotXYZ(struct nuvec_s *pos, struct nuvec_s *shape,
                     struct numtl_s *mtl, s32 *a, f32 *uvs, u32 colour) {
    struct nuvec_s screen[4];
    struct nuvec4_s v4;
    struct numtx_s m;
    struct numtx_s *vpcs;
    s32 i;

    if (NuCameraClipTestPoints(pos, 1, 0)) {
        return;
    }
    vpcs = NuCameraGetVPCSMtx();
    NuVpGetCurrentViewport();
    NuMtxSetRotateXYZ(&m, a);
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

    rndrstream_3d = &mtl->rs;
    NuRndrStreamLink(rndrstream_3d);
    rndrstream_3d->ptr.u8 = vpDmaTag_Cnt(rndrstream_3d->ptr.u8);
    *rndrstream_3d->ptr.u32++ = 0x10000000;
    *rndrstream_3d->ptr.u32++ = 0x5000000D;
    /* GNU cast-as-lvalue: the cursor union has no 64-bit member. */
    *((u64 *)rndrstream_3d->ptr.u32)++ = 0x3002400000008004ULL;
    *((u64 *)rndrstream_3d->ptr.u32)++ = 0x512;
    for (i = 0; i < 4; i++) {
        *rndrstream_3d->ptr.f32++ = uvs[i * 2];
        *rndrstream_3d->ptr.f32++ = uvs[i * 2 + 1];
        *rndrstream_3d->ptr.f32++ = 1.0f;
        rndrstream_3d->ptr.u32++;
        *rndrstream_3d->ptr.u32++ = colour & 0xFF;
        *rndrstream_3d->ptr.u32++ = (colour >> 8) & 0xFF;
        *rndrstream_3d->ptr.u32++ = (colour >> 16) & 0xFF;
        *rndrstream_3d->ptr.u32++ = colour >> 24;
        NuVecConvertToIntVU0(rndrstream_3d->ptr.u8, &screen[i]);
        rndrstream_3d->ptr.u8 += 0xC;
        *rndrstream_3d->ptr.u32++ = 0;
    }
    rndrstream_3d->ptr.u8 = vpDmaTag_Close(rndrstream_3d->ptr.u8);
    rndrstream_free = vpDmaTag_Next(rndrstream_3d->ptr.u8, 0);
}

void DrawCrateExplosions(void) {
    static struct nuvec_s shape[4] = {
        {-0.25f, 0.25f, 0.0f},
        {0.25f, 0.25f, 0.0f},
        {-0.25f, -0.25f, 0.0f},
        {0.25f, -0.25f, 0.0f},
    };
    struct BoxPol_s *BoxFace;
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
                    col = (lpo != 4) ? 0x108020 : 0x10C020;
                    break;
                case 9:
                    col = (lpo != 4) ? 0x40 : 0x80;
                    break;
                case 10:
                case 11:
                case 12:
                    col = (lpo != 4) ? 0xE880 : 0xFF90;
                    break;
                default:
                    col = (lpo != 4) ? 0x606060 : 0x808080;
                    break;
                }
                for (lp2 = 0; lp2 < 6; lp2++) {
                    fade = BoxExpList[lp].time - BoxFace->rndfade;
                    if (fade < 7) {
                        if (fade > 0) {
                            AddQuad3DrotXYZ(&BoxFace->pos, shape, CrateMat,
                                            BoxFace->ang, &uvs[lp2 * 8],
                                            col | (fade << 28));
                        }
                    } else {
                        AddQuad3DrotXYZ(&BoxFace->pos, shape, CrateMat,
                                        BoxFace->ang, &uvs[lp2 * 8],
                                        col + 0x80000000);
                    }
                    BoxFace++;
                }
            }
        }
    }
}

s32 NewCrateAnimation(struct cratecube_s *crate, s32 type, s32 action,
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
            crate->anim_time +=
                qrand() * 1.525902189e-05f * (crate->anim_duration - 1.0f);
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

struct cratecube_s *HitCrateBalloons(struct nuvec_s *pos, f32 radius) {
    struct crategroup_s *group;
    struct cratecube_s *crate;
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
    struct crategroup_s *group;
    struct cratecube_s *crate;
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

inline s32 LowestCrate(struct crategroup_s *group, struct cratecube_s *crate) {
    struct cratecube_s *crate2;
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

extern struct rail_s Rail[];

extern struct tersurface_s TerSurface[];

extern void GetALONG(struct nuvec_s *pos, s32 a, s32 b, s32 c, s32 d);
extern s32 AheadOfCheckpoint(s32 iRAIL, s32 iALONG, f32 fALONG);
extern void FindAnglesZX(struct nuvec_s *n);
extern void ResetKabooms(void);

extern struct gtimer_s GameTimer;
extern s32 gamesfx_pitch;
extern s32 gamesfx_effect_volume;
extern void AddGliderHitPoints(s32 x);

void UpdateCrates(void) {
    struct crategroup_s *group;
    struct cratecube_s *crate;
    struct cratecube_s *crate2;
    struct nuvec_s v;
    s32 i;
    s32 j;
    s32 k;
    s32 contact;
    s32 hit;
    s32 type;
    s32 old;
    s32 sfx;
    f32 speed;
    f32 y_adjust;
    f32 old_time;
    u16 angle;

    if (plr_invisibility_time < 5.0f) {
        old_time = plr_invisibility_time;
        plr_invisibility_time += 0.02f;
        if (old_time < 4.0f && plr_invisibility_time >= 4.0f) {
            GameSfx(0x21, 0);
        }
        if (plr_invisibility_time > 5.0f) {
            plr_invisibility_time = 5.0f;
        }
    }
    glass_enabled = plr_invisibility_time < 4.0f;
    angle = (u16)(((GameTimer.frame % 50) << 16) / 50);
    group = CrateGroup;
    for (i = 0; i < CRATEGROUPCOUNT; i++, group++) {
        crate = &Crate[group->iCrate];
        for (j = 0; j < group->nCrates; j++, crate++) {
            if (((LDATA->flags & 0x200) != 0 || Level == 0x1D) &&
                VEHICLECONTROL == 1) {
                y_adjust = NuTrigTable[angle] * 0.25f;
                angle += 0x1000;
            }
            sfx = -1;
            if (crate->on != 0 || (TimeTrial == 0 && crate->type1 == 7)) {
                if (crate->action != -1) {
                    if (crate->anim_time < crate->anim_duration) {
                        crate->anim_time += crate->anim_speed * 0.5999999642f;
                        if (crate->anim_time >= crate->anim_duration) {
                            if (crate->anim_cycle != 0) {
                                crate->anim_time -= crate->anim_duration - 1.0f;
                            } else {
                                crate->anim_time = crate->anim_duration;
                            }
                        }
                    } else if (TimeTrial != 0 || crate->type1 != 7) {
                        if ((TimeTrial == 0 && crate->type1 == 0xE) ||
                            (TimeTrial != 0 && crate->type2 == 0xE) ||
                            (TimeTrial == 0 && crate->type1 == 0 &&
                             crate->type3 == 0xE)) {
                            StartExclamationCrateSequence(group, crate);
                        } else if ((TimeTrial == 0 && crate->type1 == 0x11) ||
                                   (TimeTrial != 0 && crate->type2 == 0x11) ||
                                   (TimeTrial == 0 && crate->type1 == 0 &&
                                    crate->type3 == 0x11)) {
                            DestroyAllNitroCrates(group, crate);
                        }
                        crate->action = -1;
                    }
                    if (TimeTrial == 0 && crate->type1 == 7 && crate->on == 0) {
                        continue;
                    }
                }
                crate->oldy = crate->pos.y;
                type = GetCrateType(crate, 2);
                switch (type) {
                case 9:
                    if (crate->timer != 0.0f) {
                        crate->timer += 0.02f;
                        for (k = 0; k < 7; k++) {
                            if (crate->timer >= tntsfxtime[k] &&
                                crate->timer - 0.02f < tntsfxtime[k]) {
                                GameSfx(0x4F, &crate->pos);
                            }
                        }
                        if (crate->timer > 3.0f) {
                            crate->timer = 3.0f;
                        }
                        if (crate->timer == 3.0f) {
                            CrateOff(group, crate, 0, 0);
                            HopCratesAbove(CRATEHOPSPEED, group, crate);
                        } else if (crate->timer < 1.0f) {
                            crate->subtype = 0x16;
                        } else if (crate->timer < 2.0f) {
                            crate->subtype = 0x17;
                        } else {
                            crate->subtype = 0x18;
                        }
                    } else {
                        crate->subtype = 9;
                    }
                    break;
                case 8:
                    old = crate->subtype;
                    crate->timer += 0.02f;
                    if (crate->timer >= crate->duration) {
                        crate->i++;
                        switch (crate->i) {
                        case 1:
                            if (crate->type3 == -1) {
                                crate->i = 0;
                            }
                            break;
                        case 2:
                            if (crate->type4 == -1) {
                                crate->i = 0;
                            }
                            break;
                        default:
                            crate->i = 0;
                            break;
                        }
                        crate->timer = 0.0f;
                        if ((crate->flags & 8) != 0) {
                            crate->duration *= 0.9f;
                            if (crate->duration < 0.02f) {
                                crate->newtype = 0xF;
                                sfx = 0x82;
                                crate->subtype = -1;
                                NewBuzz(&player->rumble, 5);
                            }
                        } else if (player->used != 0) {
                            NuVecSub(&v, &crate->pos, &player->obj.pos);
                            if (v.x * v.x + v.y * v.y + v.z * v.z < 25.0f) {
                                crate->flags |= 8;
                            }
                        }
                    }
                    if (crate->newtype == -1) {
                        switch (crate->i) {
                        case 1:
                            crate->subtype = crate->type3;
                            break;
                        case 2:
                            crate->subtype = crate->type4;
                            break;
                        default:
                            crate->subtype = 8;
                            break;
                        }
                        if (crate->subtype != old) {
                            gamesfx_effect_volume = CHANGERVOLUME;
                            gamesfx_pitch = CHANGERPITCH;
                            GameSfx(SFX_CHANGER, &crate->pos);
                        }
                    }
                    break;
                case 6:
                    if (crate->timer > 0.0f) {
                        crate->timer += 0.02f;
                        if (((crate->flags & 0x20) != 0 &&
                             crate->timer >= 2.5f) ||
                            ((crate->flags & 0x20) == 0 &&
                             crate->timer >= 5.0f)) {
                            crate->timer =
                                (crate->flags & 0x20) != 0 ? 2.5f : 5.0f;
                            crate->counter = 0;
                        }
                    }
                    break;
                case 0x10:
                    if (qrand() <= 0xFF) {
                        sfx = 0x34;
                    }
                    break;
                case 3:
                    if (qrand() <= 0x7FF) {
                        sfx = 0;
                    }
                    break;
                }
            } else if (crate->timer != 0.0f) {
                crate->timer = 0.0f;
            }
            if (crate->model != 0) {
                crate->model->draw = 0;
                if (crate->on != 0) {
                    crate->model->draw = 1;
                }
            }
            if (crate->on != 0) {
                if ((crate->flags & 0x400) != 0) {
                    crate->pos.y = crate->pos0.y + y_adjust;
                    crate->mom = crate->pos.y - crate->oldy;
                } else if ((crate->flags & 1) != 0 ||
                           (crate->flags & 0xC00) == 0x800) {
                    contact = 0;
                    crate->pos.y += crate->mom;
                    if (crate->pos.y <= crate->shadow) {
                        if (crate->mom < 0.0f) {
                            contact = 1;
                            if ((crate->flags & 0x800) != 0) {
                                type = GetCrateType(crate, 0);
                                BreakCrate(group, crate, type, 0);
                                if (type == 5 && Level != 0x1D) {
                                    AddGliderHitPoints(0x19);
                                }
                                continue;
                            }
                        }
                        crate->mom = 0.0f;
                        crate->pos.y = crate->shadow;
                        if (qrand() < 0x1000) {
                            type = GetCrateType(crate, 0);
                            if (type == 0x10 || type == 3) {
                                speed = (f32)qrand() * 1.525902189e-05f *
                                        CRATEHOPSPEED;
                                crate->mom = speed;
                                HopCratesAbove(speed, group, crate);
                            }
                        }
                    } else {
                        hit = 0;
                        crate2 = &Crate[group->iCrate];
                        for (k = 0; k < group->nCrates; k++, crate2++) {
                            if (crate2 != crate && crate2->on != 0 &&
                                crate2->dx == crate->dx &&
                                crate2->dz == crate->dz) {
                                if (crate2->dy < crate->dy) {
                                    if (crate->pos.y < crate2->pos.y + 0.5f) {
                                        crate->pos.y = crate2->pos.y + 0.5f;
                                    }
                                    if (crate2->pos.y + 0.5f == crate->pos.y) {
                                        hit = 1;
                                        break;
                                    }
                                } else if (type == 0xF) {
                                    if (crate->pos.y + 0.5f > crate2->pos.y) {
                                        crate->pos.y = crate2->pos.y - 0.5f;
                                    }
                                    if (crate2->pos.y == crate->pos.y + 0.5f) {
                                        crate->mom = -0.02f;
                                        if (GetCrateType(crate2, 0) == 0x10) {
                                            CrateOff(group, crate2, 0, 0);
                                        }
                                        goto landed;
                                    }
                                }
                            }
                        }
                        if (hit != 0) {
                            if (type == 0xF && GetCrateType(crate2, 0) == 0xD) {
                                crate->mom = METALCRATEBOUNCESPEED;
                                NewCrateAnimation(crate2, 0xD, 0x58, 0);
                            } else {
                                crate->mom = (crate->mom + crate2->mom) * 0.5f;
                            }
                            if (qrand() < 0x1000) {
                                type = GetCrateType(crate, 0);
                                if (type == 0x10 || type == 3) {
                                    speed = (f32)qrand() * 1.525902189e-05f *
                                            CRATEHOPSPEED;
                                    crate->mom = speed;
                                    HopCratesAbove(speed, group, crate);
                                }
                            }
                        } else {
                            crate->mom += CRATEGRAVITY;
                        }
                    }
                landed:
                    if ((contact & 1) != 0) {
                        type = GetCrateType(crate, 0);
                        if (type == 9 && crate->timer == 0.0f &&
                            (crate->flags & 2) == 0) {
                            crate->timer = 0.02f;
                        } else if (type == 8 && crate->newtype == -1 &&
                                   crate->subtype == 9) {
                            crate->newtype = 9;
                            crate->timer = 0.02f;
                        }
                    }
                    if ((contact & 2) != 0) {
                        type = GetCrateType(crate2, 0);
                        if (type == 9 && crate2->timer == 0.0f &&
                            (crate->flags & 2) == 0) {
                            crate2->timer = 0.02f;
                        } else if (type == 8 && crate2->newtype == -1 &&
                                   crate2->subtype == 9) {
                            crate2->newtype = 9;
                            crate2->timer = 0.02f;
                        }
                    }
                }
            }
            if (sfx != -1) {
                GameSfx(sfx, &crate->pos);
            }
        }
    }
}

inline void ResetCrateType2(struct cratecube_s *crt) {
    crt->type2 = crt->type1;
    if (crt->model == 0) {
        return;
    }
    crt->model->type[1] = crt->type1;
}


inline struct cratecube_s *CrateInSlot(struct crategroup_s *group, s32 x, s32 y,
                              s32 z) {
    struct cratecube_s *crate;
    s32 i;

    crate = &Crate[group->iCrate];
    for (i = 0; i < group->nCrates; i++, crate++) {
        if (((crate->dx == x) && (crate->dy == y)) && (crate->dz == z)) {
            return crate;
        }
    }
    return 0;
}


inline void ResetInvisibility(void) {
    plr_invisibility_time = 5.0f;
    glass_mix = (Level != 0x17) ? 0.0f : WATERBOSSGLASSMIX;
    glass_col_mix = 0;
    glass_enabled = 0;
    glass_col_enabled = 0;
}


inline void ResetCrate(struct cratecube_s *crt) {
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


void ResetCrates(void) {
    struct nuvec_s pos;
    struct nuvec_s v;
    struct nuvec_s norm;
    struct nuvec_s dir;
    struct nuvec_s vrot = {0.25f, 0.0f, 0.25f};
    struct crategroup_s *group;
    struct cratecube_s *crate;
    struct cratecube_s *crate2;
    struct cratecube_s *below;
    s32 i;
    s32 j;
    s32 k;
    s32 x;
    s32 y;
    s32 z;
    s32 eshady;
    s32 surf;
    s32 esh;
    s32 mode;
    s32 lower;
    s32 index;
    s32 gempath_reset;
    u16 bits;
    f32 dy;
    short *handel;

    DESTRUCTIBLECRATECOUNT = 0;
    DESTRUCTIBLEBONUSCRATECOUNT = 0;
    index = 0;
    gempath_reset = 0;
    if (Rail[7].type == 3 &&
        AheadOfCheckpoint(gempath_RPos.iRAIL, gempath_RPos.iALONG,
                          gempath_RPos.fALONG) != 0) {
        gempath_reset = 1;
    }
    group = CrateGroup;
    for (i = 0; i < CRATEGROUPCOUNT; i++, group++) {
        NuVecRotateY(&v, &vrot, group->angle);
        crate = &Crate[group->iCrate];
        for (j = 0; j < group->nCrates; j++, crate++) {
            if (crate->type2 == -1) {
                ResetCrateType2(crate);
            }
            crate->index = index;
            crate->pos.x = crate->pos0.x + v.x;
            index++;
            bits = 0;
            crate->pos.z = crate->pos0.z + v.z;
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
                } else if (Rail[crate->iRAIL].type == 3) {
                    bits = 0x100;
                }
            }
            if (crate->type1 == 8) {
                crate->i = 0;
                crate->duration = 1.0f;
            }
            if (((LDATA->flags & 0x200) != 0 || Level == 0x1D) &&
                Level != 0x1E) {
                bits |= 0xC00;
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
            case 0x10:
            case 0x11:
            case 0x12:
            case 0x13:
            case 0x14:
            case 0xE:
                if (crate->iRAIL == -1) {
                    if (bits & 0x400) {
                        bits |= 0x10;
                    }
                } else {
                    bits |= 0x10;
                }
                break;
            case 0:
                if (crate->iRAIL != -1 || (bits & 0x400)) {
                    if (crate->type3 != -1) {
                        if (crate->type3 != 0 && crate->type3 != 0xD &&
                            crate->type3 != 0xF && crate->trigger != -1 &&
                            Crate[crate->trigger].type1 == 0xE) {
                            bits |= 0x10;
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
            } else if ((bits & 0x100) != 0) {
                if (gempath_reset != 0) {
                    goto reset;
                }
            } else if ((cp_iRAIL == -1 || cp_iALONG == -1 ||
                        crate->type1 != 7 || TimeTrial != 0) &&
                       bonus_restart == 0) {
                if (AheadOfCheckpoint(crate->iRAIL, crate->iALONG,
                                      crate->fALONG) != 0) {
                reset:
                    crate->on = 1;
                    ResetCrate(crate);
                }
            }
            if (crate->type1 == 3) {
                NewCrateAnimation(crate, 3, 0x22, 1);
            }
            pos.x = crate->pos.x;
            pos.y = crate->pos0.y;
            pos.z = crate->pos.z;
            crate->shadow = NewShadowMaskPlat(&pos, 0.0f, -1);
            if (Level == 0x12 && crate->shadow < 0.0f) {
                crate->shadow = 0.0f;
            }
            if (crate->shadow != 2000000.0f) {
                norm = ShadNorm;
                FindAnglesZX(&norm);
                crate->surface_xrot = temp_xrot;
                crate->surface_zrot = temp_zrot;
                surf = ShadowInfo();
                if ((TerSurface[surf].flags & 1) == 0) {
                    bits |= 0x2000;
                }
                eshady = EShadY;
                if (eshady != 2000000.0f) {
                    esh = EShadowInfo();
                    if (esh < 0 || esh > 10) {
                        esh = 0;
                    }
                } else {
                    esh = -1;
                }
                NewScanInit();
                crate->colbox[0].y = crate->shadow;
                crate->colbox[1].y = crate->shadow + 4.0f;
                dir.x = pos.x - 1.5f;
                dir.y = crate->shadow + 0.2f;
                dir.z = pos.z - 1.5f;
                norm.x = 3.0f;
                norm.y = 0.0f;
                norm.z = 3.0f;
                handel = NewScanHandel(&dir, &norm, 1, 0, 0.0f);
                pos.y = crate->shadow + 0.2f;
                dir.x = -1.5f;
                dir.y = 0.0f;
                dir.z = 0.0f;
                NewRayCastSetHandel(&pos, &dir, 0.0f, 0.0f, 0.0f, handel);
                crate->colbox[0].x = dir.x + pos.x;
                dir.x = 0.0f;
                dir.y = 0.0f;
                dir.z = -1.5f;
                NewRayCastSetHandel(&pos, &dir, 0.0f, 0.0f, 0.0f, handel);
                crate->colbox[0].z = dir.z + pos.z;
                dir.x = 1.5f;
                dir.y = 0.0f;
                dir.z = 0.0f;
                NewRayCastSetHandel(&pos, &dir, 0.0f, 0.0f, 0.0f, handel);
                crate->colbox[1].x = dir.x + pos.x;
                dir.x = 0.0f;
                dir.y = 0.0f;
                dir.z = 1.5f;
                NewRayCastSetHandel(&pos, &dir, 0.0f, 0.0f, 0.0f, handel);
                crate->colbox[1].z = dir.z + pos.z;
            } else {
                crate->surface_xrot = 0;
                crate->surface_zrot = 0;
                surf = 0;
                esh = -1;
            }
            if ((TerSurface[surf].flags & 4) != 0) {
                bits |= 0x200;
            }
            if (LowestCrate(group, crate) != 0 &&
                (crate->shadow == 2000000.0f ||
                 crate->pos.y - crate->shadow > 2.0f ||
                 (TerSurface[surf].flags & 1) != 0 ||
                 (esh != -1 && crate->pos.y > eshady))) {
                bits |= 0x1000;
            }
            crate->flags = bits;
            if (crate->shadow != 2000000.0f) {
                dy = crate->pos0.y - crate->shadow;
                if (dy < -0.1f) {
                    crate->flags = bits | 4;
                } else if (dy < 0.1f) {
                    crate->flags = bits | 2;
                    if (crate->type1 == 0x10 || crate->type1 == 3 ||
                        crate->type2 == 0x10 || crate->type2 == 3) {
                        crate->flags |= 1;
                    }
                }
            }
        }
        crate = &Crate[group->iCrate];
        for (j = 0; j < group->nCrates; j++, crate++) {
            if ((crate->flags & 6) == 0) {
                below = 0;
                x = crate->dx;
                y = crate->dy;
                z = crate->dz;
                crate2 = 0;
            down:
                y--;
                crate2 = CrateInSlot(group, x, y, z);
                if (crate2 != 0) {
                    below = crate2;
                    goto down;
                }
                if (below != 0 && (below->flags & 6) != 0) {
                    crate->flags |= 1;
                }
            }
            if (crate->type1 == 7 && crate->shadow != 2000000.0f) {
                crate->xrot0 = crate->surface_xrot;
                crate->zrot0 = crate->surface_zrot;
            } else if ((crate->flags & 4) != 0 &&
                       CrateInSlot(group, crate->dx, crate->dy + 1,
                                   crate->dz) == 0) {
                crate->xrot0 = (qrand() - 0x8000) / 16;
                crate->zrot0 = (qrand() - 0x8000) / 16;
            } else if ((crate->flags & 2) != 0 &&
                       CrateInSlot(group, crate->dx, crate->dy + 1,
                                   crate->dz) == 0) {
                crate->xrot0 = crate->surface_xrot;
                crate->zrot0 = crate->surface_zrot;
            } else {
                crate->xrot0 = 0;
                crate->zrot0 = 0;
            }
            crate->xrot = crate->xrot0;
            crate->zrot = crate->zrot0;
            if (crate->type1 == 6) {
                mode = 1;
            } else if (crate->type2 == 6) {
                mode = 2;
            } else if (crate->type1 == 0 && crate->type3 == 6) {
                mode = 3;
            } else {
                mode = 0;
            }
            if (mode != 0) {
                crate2 = &Crate[group->iCrate];
                for (k = 0; k < group->nCrates; k++, crate2++) {
                    lower = (crate2->dy < crate->dy) ? 1 : 0;
                    if (crate2 != crate && crate2->dx == crate->dx &&
                        crate2->dz == crate->dz &&
                        ((mode == 1 &&
                          (crate2->type1 == 6 ||
                           (lower && (crate2->type1 == 4 ||
                                      crate2->type1 == 0xD)))) ||
                         (mode == 2 &&
                          (crate2->type2 == 6 ||
                           (lower && (crate2->type2 == 4 ||
                                      crate2->type2 == 0xD)))) ||
                         (mode == 3 && crate2->type1 == 0 &&
                          crate2->trigger == crate->trigger &&
                          (crate2->type3 == 6 ||
                           (lower && (crate2->type3 == 4 ||
                                      crate2->type3 == 0xD)))))) {
                        crate->flags |= 0x20;
                        break;
                    }
                }
                crate->counter = crate->counter / 2;
            }
        }
    }
    ResetKabooms();
    UpdateCrates();
    ResetInvisibility();
}

extern s32 editor_active;
extern s32 edcrtDrawType;
extern struct gtimer_s GlobalTimer;
extern struct cammtx_s *pCam;

extern struct objtab_s ObjTab[];

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
extern s32 LowestActiveCrate(struct crategroup_s *group,
                             struct cratecube_s *crate);

void DrawCrates(void) {
    struct nuvec_s v;
    struct numtx_s mtx;
    struct numtx_s mtx2;
    struct numtx_s tmtx[256];
    struct crategroup_s *group;
    struct cratecube_s *crate;
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
                    type = ((GlobalTimer.frame % 10) > 4) ? type : 0;
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
                if (ot->obj.special != 0) {
                    NuRndrGScnObj(ot->obj.scene->gobjs[ot->obj.special->instance->objid],
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
                        NuHGobjEvalAnim(model->obj,
                                        model->anmdata[crate->action],
                                        crate->anim_time, 0, 0, tmtx);
                    } else {
                        NuHGobjEval(model->obj, 0, 0, tmtx);
                    }
                } else {
                    NuHGobjEval(model->obj, 0, 0, tmtx);
                    dwa = 0;
                }
                NuHGobjRndrMtxDwa(model->obj, pmtx, 1, 0, tmtx, dwa);
                if ((crate->flags & 0x200) != 0) {
                    mtx2 = *pmtx;
                    mtx2._01 = -mtx2._01;
                    mtx2._11 = -mtx2._11;
                    mtx2._21 = -mtx2._21;
                    mtx2._31 = crate->shadow - (mtx2._31 - crate->shadow);
                    NuHGobjRndrMtxDwa(model->obj, &mtx2, 1, 0, tmtx, dwa);
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
                 ((ObjTab[21].obj.special != 0) &&
                  (LowestActiveCrate(group, crate) != 0)))) {
                NuMtxSetRotationY(&mtx2, group->angle);
                NuMtxRotateZ(&mtx2, crate->surface_zrot);
                NuMtxRotateX(&mtx2, crate->surface_xrot);
                mtx2._30 = crate->pos.x;
                mtx2._31 = crate->shadow + 0.025f;
                mtx2._32 = crate->pos.z;
                NuRndrGScnObj(ObjTab[21]
                                  .obj.scene->gobjs[ObjTab[21]
                                                    .obj.special->instance->objid],
                              &mtx2);
            }
        }
    }
}

s32 LowestActiveCrate(struct crategroup_s *group, struct cratecube_s *crate) {
    struct cratecube_s *crate2;
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

s32 CrateInTheWay(struct obj_s *obj, struct crategroup_s *group,
                  struct cratecube_s *crate, s32 dx, s32 dz, char *hit) {
    struct cratecube_s *crate2;
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
    struct crategroup_s *group;
    struct cratecube_s *crate;
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
