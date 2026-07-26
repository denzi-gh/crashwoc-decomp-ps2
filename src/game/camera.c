/*
 * Unit: game/camera
 *
 * Functions:
 *   0x001d5f80 InitRails
 *   0x001d6228 BestRailPosition
 *   0x001d6850 ComplexRailPosition
 *   0x001d6b08 MoveRailPosition
 *   0x001d6e08 RailInfo
 *   0x001d73b8 MoveGameCamera
 *   0x001daaf8 DrawCameraTarget
 *   0x001dac00 GetALONG
 *   0x001dac58 FurtherALONG
 *   0x001dace0 FurtherBEHIND
 *   0x001dad68 ResetGameCameras
 *   0x001dada0 JudderGameCamera
 *   0x001dae28 BlendGameCamera
 *   0x001dae68 InitCameraTargetMaterial
 *   0x001daf10 DrawMtlLine3D
 *   0x001daf58 LookUpDownRail
 *   0x001dafd0 InSplineArea
 *   0x001db078 AddCross
 */

#include "creature.h"

/* cammtx_s (game camera): offsets verified in ResetGameCameras/BlendGameCamera. */
struct cammtx_s {
    u8 unk_0x00[0xB0];
    struct nuvec_s oldpos;        /* 0xB0 */
    struct nuvec_s newpos;        /* 0xBC */
    u8 unk_0xC8[0x18];
    f32 distance;                 /* 0xE0 */
    f32 ahead;                    /* 0xE4 */
    f32 judder;                   /* 0xE8 */
    f32 blend_time;               /* 0xEC */
    f32 blend_duration;           /* 0xF0 */
    u8 unk_0xF4[0xC];
    u16 old_xrot;                 /* 0x100 */
    u16 new_xrot;                 /* 0x102 */
    u16 old_yrot;                 /* 0x104 */
    u16 new_yrot;                 /* 0x106 */
    u16 old_zrot;                 /* 0x108 */
    u16 new_zrot;                 /* 0x10A */
    u8 unk_0x10C[0x8];
    s8 mode;                      /* 0x114 */
    u8 unk_0x115[0x5];
    s8 vertical;                  /* 0x11A */
    u8 unk_0x11B[0x1];
};                                /* 0x11C */

/* rail_s: stride 0x28. type @0x26 (mirrors src/game/ai.c and bug.c). */
struct nugspline_s;
struct rail_s {
    struct nugspline_s *pINPLAT;   /* 0x00 */
    struct nugspline_s *pINCAM;    /* 0x04 */
    struct nugspline_s *pLEFT;     /* 0x08 */
    struct nugspline_s *pCAM;      /* 0x0C */
    struct nugspline_s *pRIGHT;    /* 0x10 */
    struct nugspline_s *pOUTCAM;   /* 0x14 */
    struct nugspline_s *pOUTPLAT;  /* 0x18 */
    f32 in_distance;               /* 0x1C */
    f32 out_distance;              /* 0x20 */
    s16 edges;                     /* 0x24 */
    s8 type;                       /* 0x26 */
    s8 circuit;                    /* 0x27 */
};                                 /* 0x28 */

extern struct rail_s Rail[];
extern struct RPos_s TempRPos;
extern s32 temp_rail_end;
extern s32 nRAILS;
extern s32 temp_iRAIL;
extern s32 temp_iALONG;
extern f32 temp_fALONG;
extern f32 temp_fACROSS;
extern s32 Level;

extern void ComplexRailPosition(struct nuvec_s *pos, s32 iRAIL, s32 iALONG,
                                struct RPos_s *rpos, s32 a4);
extern f32 NuVecDist(struct nuvec_s *a, struct nuvec_s *b, void *c);
extern f32 RatioBetweenEdges(struct nuvec_s *pos, struct nuvec_s *p0,
                             struct nuvec_s *p1, struct nuvec_s *p2,
                             struct nuvec_s *p3);
extern void RailInfo(struct RPos_s *rpos, struct nuvec_s *pos, u16 *angle,
                     u16 *cam_angle, u8 *mode);
extern int abs(int value);

/* nugspline_s: len@0x0, ptsize@0x2, pts@0x8. */
struct nugspline_s {
    s16 len;          /* 0x0 */
    s16 ptsize;       /* 0x2 */
    u8 pad4[0x4];
    u8 *pts;          /* 0x8 */
};


/* dbest and iVar6 are deliberately left uninitialised: retail never seeds
 * $f20 nor the 0x60(sp) slot before the search loop. */
f32 BestRailPosition(struct nuvec_s *pos, struct RPos_s *rpos, s32 iRAIL,
                     s32 iALONG) {
    struct nuvec_s v;
    struct nuvec_s v0;
    struct nuvec_s v1;
    struct nuvec_s v2;
    struct nuvec_s v3;
    struct nuvec_s *p0;
    struct nuvec_s *p1;
    struct nuvec_s *p2;
    struct nuvec_s *p3;
    struct rail_s *rail;
    s32 iVar1;
    s32 iVar2;
    s32 iVar3;
    s32 iVar4;
    s32 iVar8;
    s32 iVar7;
    s32 iVar5;
    s32 iVar6;
    s32 bVar2;
    f32 d;
    f32 dbest;

    bVar2 = 0;
    if ((Level == 6) || (Level == 0x22)) {
        bVar2 = 1;
    }
    if (bVar2 != 0) {
        v.x = -pos->y;
        v.y = pos->x;
        v.z = pos->z;
    } else {
        v = *pos;
    }
    rail = &Rail[iRAIL];
    rpos->iRAIL = -1;
    rpos->iALONG = -1;
    rpos->fALONG = 0.0f;
    rpos->fACROSS = 0.0f;
    if (rail->type == -1) {
        return 0.0f;
    }
    if (iALONG == -1) {
        iVar3 = (s32)rail->edges / 2;
    } else {
        iVar3 = iALONG;
    }
    iVar8 = iVar3 + 1;
    iVar7 = iVar3 - 1;
    iVar4 = 0;
    iVar5 = 0;
Loop:
    if (iVar4 == 0) {
        iVar1 = iVar3;
    } else if (iVar4 == 1) {
        iVar1 = iVar8;
        iVar8++;
    } else {
        iVar1 = iVar7;
        iVar7--;
    }
    if (iVar1 >= 0 && iVar1 < rail->edges) {
        iVar2 = iVar1 + 1;
        if ((iVar2 == rail->edges) && (rail->circuit != 0)) {
            iVar2 = 0;
        }
        p0 = (struct nuvec_s *)(rail->pLEFT->pts + (iVar1 * rail->pLEFT->ptsize));
        p1 = (struct nuvec_s *)(rail->pLEFT->pts + (iVar2 * rail->pLEFT->ptsize));
        p2 = (struct nuvec_s *)(rail->pRIGHT->pts + (iVar2 * rail->pRIGHT->ptsize));
        p3 = (struct nuvec_s *)(rail->pRIGHT->pts + (iVar1 * rail->pRIGHT->ptsize));
        if (bVar2 != 0) {
            v0.x = -p0->y;
            v0.y = p0->x;
            v0.z = p0->z;

            v1.x = -p1->y;
            v1.y = p1->x;
            v1.z = p1->z;

            v2.x = -p2->y;
            v2.y = p2->x;
            v2.z = p2->z;

            v3.x = -p3->y;
            v3.y = p3->x;
            v3.z = p3->z;
        } else {
            v0 = *p0;
            v1 = *p1;
            v2 = *p2;
            v3 = *p3;
        }
        if ((((0.0f <= (v.x - v0.x) * (v1.z - v0.z) +
                           (v.z - v0.z) * (v0.x - v1.x)) &&
              (0.0f <= (v.x - v1.x) * (v2.z - v1.z) +
                           (v.z - v1.z) * (v1.x - v2.x))) &&
             (0.0f <= (v.x - v2.x) * (v3.z - v2.z) +
                          (v.z - v2.z) * (v2.x - v3.x))) &&
            (0.0f <= (v.x - v3.x) * (v0.z - v3.z) +
                         (v.z - v3.z) * (v3.x - v0.x))) {
            d = abs((s32)((v0.y + v1.y + v2.y + v3.y) * 0.25f - v.y));
            if (iALONG == -1) {
                if (rpos->iALONG == -1 || d < dbest) {
                    dbest = d;
                    iVar6 = 1;
                }
            } else {
                dbest = d;
                iVar6 = 2;
            }
            if (iVar6 != 0) {
                rpos->iALONG = iVar1;
                rpos->iRAIL = iRAIL;
                rpos->i1 = iVar2;
                rpos->i2 = iVar2 + 1;
                if ((rpos->i2 == rail->edges) && (rail->circuit != 0)) {
                    rpos->i2 = 0;
                }
                rpos->fALONG = RatioBetweenEdges(&v, &v3, &v0, &v2, &v1);
                rpos->fACROSS = RatioBetweenEdges(&v, &v0, &v1, &v3, &v2);
                if (iVar6 == 2) {
                    goto Finish;
                }
            }
        }
    } else {
        iVar5 |= iVar4;
    }
    if (iVar5 == 3) {
        goto Finish;
    }
    iVar4 = (iVar4 == 1) ? 2 : 1;
    goto Loop;

Finish:
    temp_iRAIL = rpos->iRAIL;
    temp_iALONG = rpos->iALONG;
    temp_fALONG = rpos->fALONG;
    temp_fACROSS = rpos->fACROSS;

    if ((rpos->iRAIL != -1) && (temp_iALONG != -1)) {
        rpos->vertical = 0;
        if ((rpos->i2 != rpos->i1) && (rpos->i2 < rail->edges) &&
            (bVar2 == 0)) {
            p0 = (struct nuvec_s *)(rail->pLEFT->pts +
                                    (rpos->i1 * rail->pLEFT->ptsize));
            p1 = (struct nuvec_s *)(rail->pLEFT->pts +
                                    (rpos->i2 * rail->pLEFT->ptsize));
            if ((p0->x == p1->x) && (p0->z == p1->z)) {
                p0 = (struct nuvec_s *)(rail->pRIGHT->pts +
                                        (rpos->i1 * rail->pRIGHT->ptsize));
                p1 = (struct nuvec_s *)(rail->pRIGHT->pts +
                                        (rpos->i2 * rail->pRIGHT->ptsize));
                if ((p0->x == p1->x) && (p0->z == p1->z)) {
                    rpos->vertical = 1;
                }
            }
        }
        if (bVar2 != 0) {
            RailInfo(rpos, &rpos->pos, 0, &rpos->cam_angle, 0);
        } else {
            RailInfo(rpos, &rpos->pos, &rpos->angle, &rpos->cam_angle,
                     &rpos->mode);
        }
    }
    return dbest;
}


void MoveRailPosition(struct nuvec_s *dst, struct RPos_s *rpos, f32 distance,
                      s32 direction) {
    struct rail_s *rail;
    struct RPos_s RPos;
    struct nuvec_s *p1;
    struct nuvec_s *p;
    f32 f;
    f32 d;
    s32 i0;
    s32 i1;

    d = distance;
    temp_rail_end = 0;
    TempRPos = RPos = *rpos;
    if ((RPos.iRAIL == -1) || ((s32)RPos.iALONG == -1)) {
        return;
    }
    *dst = RPos.pos;
    rail = &Rail[RPos.iRAIL];
Loop:
    i1 = RPos.iALONG;
    i0 = i1 + 1;
    if ((i0 == rail->edges) && (rail->circuit != 0)) {
        i0 = 0;
    }
    p1 = (struct nuvec_s *)(rail->pCAM->pts + (i1 * rail->pCAM->ptsize));
    p = (struct nuvec_s *)(rail->pCAM->pts + (i0 * rail->pCAM->ptsize));
    if (direction == 0) {
        p1 = p;
    }
    f = NuVecDist(dst, p1, 0);
    if (d > f) {
        d = (d - f);
        *dst = *p1;
        if (direction == 0) {
            if (rail->circuit != 0) {
                RPos.iALONG++;
                if (RPos.iALONG == rail->edges) {
                    RPos.iALONG = direction;
                }
                goto Loop;
            } else {
                if (RPos.iALONG < rail->edges) {
                    RPos.iALONG++;
                    goto Loop;
                }
                temp_rail_end = 1;
                goto end;
            }
        }
        if (rail->circuit != 0) {
            RPos.iALONG--;
            if (RPos.iALONG == -1) {
                RPos.iALONG = rail->edges;
                RPos.iALONG--;
            }
            goto Loop;
        }
        if (RPos.iALONG > 0) {
            RPos.iALONG--;
            goto Loop;
        }
        temp_rail_end = 2;
    } else {
        dst->x = (p1->x - dst->x) * (d / f) + dst->x;
        dst->y = (p1->y - dst->y) * (d / f) + dst->y;
        dst->z = (p1->z - dst->z) * (d / f) + dst->z;
    }
end:
    TempRPos = RPos;
}


void ResetGameCameras(struct cammtx_s *Gamecam, s32 n) {
    while (n > 0) {
        Gamecam->mode = -1;
        Gamecam->judder = 0.0f;
        Gamecam->blend_time = 0.0f;
        Gamecam->blend_duration = 0.0f;
        Gamecam->distance = 0.0f;
        Gamecam->ahead = 0.0f;
        Gamecam->vertical = '\0';
        Gamecam++;
        n--;
    }
}

void BlendGameCamera(struct cammtx_s *cam, f32 time) {
    cam->oldpos = cam->newpos;
    cam->old_xrot = cam->new_xrot;
    cam->old_yrot = cam->new_yrot;
    cam->old_zrot = cam->new_zrot;
    cam->blend_time = 0.0f;
    cam->blend_duration = time;
}

void JudderGameCamera(struct cammtx_s *cam, f32 time, struct nuvec_s *pos) {
    f32 d;

    if (time > cam->judder) {
        if (pos != 0) {
            d = NuVecDist(&player->obj.pos, pos, 0);
            if (d < 10.0f) {
                cam->judder = time * ((10.0f - d) / 10.0f);
            }
        } else {
            cam->judder = time;
        }
    }
}

s32 InSplineArea(struct nuvec_s *pos, struct nugspline_s *spl) {
    struct nuvec_s *p0;
    struct nuvec_s *p1;
    s32 i;
    s32 j;

    for (i = 1; i < spl->len; i++) {
        p1 = (struct nuvec_s *)((s32)spl->pts + i * spl->ptsize);
        j = i + 1;
        if (j == spl->len) {
            j = 1;
        }
        p0 = (struct nuvec_s *)((s32)spl->pts + j * spl->ptsize);
        if (!((pos->x - p1->x) * (p0->z - p1->z) +
              (pos->z - p1->z) * (p1->x - p0->x) >= 0.0f)) {
            return 0;
        }
    }
    return 1;
}

void GetALONG(struct nuvec_s *pos, struct RPos_s *rpos, s32 iRAIL, s32 iALONG,
              s32 info) {
    if (rpos == 0) {
        rpos = &TempRPos;
    }
    if (nRAILS != 0) {
        ComplexRailPosition(pos, iRAIL, iALONG, rpos, 0);
    } else {
        temp_iALONG = -1;
        temp_iRAIL = -1;
    }
}

s32 FurtherALONG(s32 iRAIL0, s32 iALONG0, f32 fALONG0, s32 iRAIL1, s32 iALONG1,
                 f32 fALONG1) {
    if (iRAIL0 == -1) {
        return 0;
    }
    if (iRAIL1 == -1) {
        return 0;
    }
    if ((Rail + iRAIL0)->type != (Rail + iRAIL1)->type) {
        return 0;
    }
    if (iRAIL0 < iRAIL1) {
        return 0;
    }
    if (iRAIL0 > iRAIL1) {
        return 1;
    }
    if (iALONG0 < iALONG1) {
        return 0;
    }
    if (iALONG0 > iALONG1) {
        return 1;
    }
    if (fALONG0 > fALONG1) {
        return 1;
    }
    return 0;
}

s32 FurtherBEHIND(s32 iRAIL0, s32 iALONG0, f32 fALONG0, s32 iRAIL1, s32 iALONG1,
                  f32 fALONG1) {
    if (iRAIL0 == -1) {
        return 0;
    }
    if (iRAIL1 == -1) {
        return 0;
    }
    if ((Rail + iRAIL0)->type != (Rail + iRAIL1)->type) {
        return 0;
    }
    if (iRAIL0 > iRAIL1) {
        return 0;
    }
    if (iRAIL0 < iRAIL1) {
        return 1;
    }
    if (iALONG0 > iALONG1) {
        return 0;
    }
    if (iALONG0 < iALONG1) {
        return 1;
    }
    if (fALONG0 < fALONG1) {
        return 1;
    }
    return 0;
}
