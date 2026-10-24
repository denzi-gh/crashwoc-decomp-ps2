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

#include "nu_libc.h"

#include "numath/numtx.h"
#include "numath/nutrig.h"
#include "numath/nuvec.h"
#include "nu3d/numtl.h"
#include "nu3d/nugobj.h"
#include "nu3d/nurndr.h"
#include "nu3d/nugscn.h"
#include "nu3d/nucamera.h"
#include "game/gametypes.h"
#include "gamelib/terrain.h"
#include "game/creature.h"
#include "nups2/ps2ctl.h"

#define NULL ((void *)0)

extern struct rail_s Rail[];
extern struct RPos_s TempRPos;
extern s32 temp_rail_end;
extern s32 nRAILS;
extern s32 temp_iRAIL;
extern s32 temp_iALONG;
extern f32 temp_fALONG;
extern f32 temp_fACROSS;
extern s32 Level;
extern s32 Bonus;
extern s32 Death;
extern s32 GemPath;
extern struct RPos_s cRPos[];
extern struct RPos_s temp_cRPos[];
extern struct RPos_s *best_cRPos;
extern struct RPos_s *temp_best_cRPos;
extern s32 cRPosCOUNT;
extern s32 temp_cRPosCOUNT;

extern void ComplexRailPosition(struct nuvec_s *pos, s32 iRAIL, s32 iALONG,
                                struct RPos_s *rpos, s32 a4);
extern f32 RatioBetweenEdges(struct nuvec_s *pos, struct nuvec_s *p0,
                             struct nuvec_s *p1, struct nuvec_s *p2,
                             struct nuvec_s *p3);
extern void RailInfo(struct RPos_s *rpos, struct nuvec_s *pos, u16 *angle,
                     u16 *cam_angle, u8 *mode);
extern f32 NuTrigTable[];
extern s32 RotDiff(u16 a, u16 b);
extern f32 NuFsqrt(f32 x);


extern struct nugscn_s *world_scene[];
extern char tbuf[];
extern char *tRailExt[];
extern char *tInPlatRail;
extern char *tInCamRail;
extern char *tLeftRail;
extern char *tCamRail;
extern char *tRightRail;
extern char *tOutCamRail;
extern char *tOutPlatRail;

extern char D_00630AA0[];
#define fmt_railname D_00630AA0
extern char D_00630A70[];
#define railext_bonus D_00630A70
extern char D_00630A68[];
#define railext_death D_00630A68
extern char D_00630A60[];
#define railext_gem D_00630A60

extern f32 SplineDistance(struct nugspline_s *spl);


void InitRails(void) {
    struct rail_s *rail;
    s32 i;
    s16 camlen;
    s16 leftlen;
    s16 rightlen;

    nRAILS = 0;
    if (world_scene[0] != 0) {
        rail = Rail;
        for (i = 0; i < 8; i++, rail++) {
            rail->in_distance = 25.0f;
            rail->out_distance = 25.0f;
            rail->type = -1;
            rail->pINPLAT = 0;
            rail->pINCAM = 0;
            rail->pLEFT = 0;
            rail->pCAM = 0;
            rail->pRIGHT = 0;
            rail->pOUTCAM = 0;
            rail->pOUTPLAT = 0;
            rail->circuit = 0;
            sprintf(tbuf, fmt_railname, tCamRail, tRailExt[i]);
            rail->pCAM = NuSplineFind(world_scene[0], tbuf);
            if (rail->pCAM == 0) {
                continue;
            }
            camlen = rail->pCAM->len;
            if (camlen < 2) {
                continue;
            }
            sprintf(tbuf, fmt_railname, tLeftRail, tRailExt[i]);
            rail->pLEFT = NuSplineFind(world_scene[0], tbuf);
            if (rail->pLEFT == 0) {
                continue;
            }
            leftlen = rail->pLEFT->len;
            if (leftlen != camlen) {
                continue;
            }
            sprintf(tbuf, fmt_railname, tRightRail, tRailExt[i]);
            rail->pRIGHT = NuSplineFind(world_scene[0], tbuf);
            if (rail->pRIGHT == 0) {
                continue;
            }
            rightlen = rail->pRIGHT->len;
            if (rightlen != leftlen) {
                continue;
            }
            rail->edges = rightlen - 1;
            sprintf(tbuf, fmt_railname, tInPlatRail, tRailExt[i]);
            rail->pINPLAT = NuSplineFind(world_scene[0], tbuf);
            if (rail->pINPLAT != 0) {
                rail->in_distance = SplineDistance(rail->pINPLAT);
            }
            sprintf(tbuf, fmt_railname, tInCamRail, tRailExt[i]);
            rail->pINCAM = NuSplineFind(world_scene[0], tbuf);
            sprintf(tbuf, fmt_railname, tOutPlatRail, tRailExt[i]);
            rail->pOUTPLAT = NuSplineFind(world_scene[0], tbuf);
            if (rail->pOUTPLAT != 0) {
                rail->out_distance = SplineDistance(rail->pOUTPLAT);
            }
            sprintf(tbuf, fmt_railname, tOutCamRail, tRailExt[i]);
            rail->pOUTCAM = NuSplineFind(world_scene[0], tbuf);
            if (strcmp(tRailExt[i], railext_bonus) == 0) {
                rail->type = 1;
            } else if (strcmp(tRailExt[i], railext_death) == 0) {
                rail->type = 2;
            } else if (strcmp(tRailExt[i], railext_gem) == 0) {
                rail->type = 3;
            } else {
                rail->type = 0;
            }
            nRAILS++;
        }
    }
}


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


void ComplexRailPosition(struct nuvec_s *pos, s32 iRAIL, s32 iALONG,
                         struct RPos_s *rpos, s32 set) {
    struct RPos_s *list;
    struct RPos_s *best;
    s32 i;
    s32 bonus;
    s32 death;
    s32 gempath;
    s32 count;
    f32 d;
    f32 dbest;

    if (set != 0) {
        list = cRPos;
    } else {
        list = temp_cRPos;
    }
    if (set != 0) {
        bonus = (u32)(Bonus - 1) < 3;
        death = (u32)(Death - 1) < 3;
        gempath = (u32)(GemPath - 1) < 3;
    }
    count = 0;
    best = 0;
    if (iRAIL == -1) {
        iRAIL = 0;
        iALONG = -1;
    }
    for (i = 0; i < 8; i++) {
        if ((set == 0) ||
            ((bonus == 0 || Rail[iRAIL].type == 1) &&
             (death == 0 || Rail[iRAIL].type == 2) &&
             (gempath == 0 || Rail[iRAIL].type == 3) &&
             (bonus != 0 || death != 0 || gempath != 0 ||
              Rail[iRAIL].type == 0))) {
            dbest = BestRailPosition(pos, &list[count], iRAIL, iALONG);
            if (list[count].iRAIL != -1 && list[count].iALONG != -1) {
                if (best == 0 || dbest < d) {
                    best = &list[count];
                    d = dbest;
                }
                count++;
                if (count == 3) {
                    break;
                }
            }
        }
        iRAIL = (iRAIL + 1) % 8;
        iALONG = -1;
    }
    if (best != 0) {
        *rpos = *best;
    } else {
        rpos->iRAIL = rpos->iALONG = -1;
    }
    if (set != 0) {
        best_cRPos = best;
        cRPosCOUNT = count;
    } else {
        temp_best_cRPos = best;
        temp_cRPosCOUNT = count;
    }
    temp_iRAIL = rpos->iRAIL;
    temp_iALONG = rpos->iALONG;
    temp_fALONG = rpos->fALONG;
    temp_fACROSS = rpos->fACROSS;
}


void MoveRailPosition(struct nuvec_s *dst, struct RPos_s *rpos, f32 distance,
                      s32 direction) {
    struct rail_s *rail;
    struct RPos_s RPos;
    struct nuvec_s *p0;
    struct nuvec_s *p1;
    struct nuvec_s *p;
    f32 edgedist;
    f32 ratio;
    s32 i0;
    s32 i1;

    temp_rail_end = 0;
    TempRPos = RPos = *rpos;
    if ((RPos.iRAIL == -1) || (RPos.iALONG == -1)) {
        return;
    }
    *dst = RPos.pos;
    rail = &Rail[RPos.iRAIL];
Loop:
    i0 = RPos.iALONG;
    i1 = i0 + 1;
    if ((i1 == rail->edges) && (rail->circuit != 0)) {
        i1 = 0;
    }
    p0 = (struct nuvec_s *)(rail->pCAM->pts + i0 * rail->pCAM->ptsize);
    p1 = (struct nuvec_s *)(rail->pCAM->pts + i1 * rail->pCAM->ptsize);
    p = p0;
    if (direction == 0) {
        p = p1;
    }
    edgedist = NuVecDist(dst, p, 0);
    if (distance > edgedist) {
        distance -= edgedist;
        *dst = *p;
        if (direction == 0) {
            if (rail->circuit != 0) {
                RPos.iALONG++;
                if (RPos.iALONG == rail->edges) {
                    RPos.iALONG = 0;
                }
                goto Loop;
            }
            if (RPos.iALONG < rail->edges) {
                RPos.iALONG++;
                goto Loop;
            }
            temp_rail_end = 1;
        } else {
            if (rail->circuit != 0) {
                RPos.iALONG--;
                if (RPos.iALONG == -1) {
                    RPos.iALONG = rail->edges - 1;
                }
                goto Loop;
            }
            if (RPos.iALONG > 0) {
                RPos.iALONG--;
                goto Loop;
            }
            temp_rail_end = 2;
        }
    } else {
        ratio = distance / edgedist;
        dst->x += (p->x - dst->x) * ratio;
        dst->y += (p->y - dst->y) * ratio;
        dst->z += (p->z - dst->z) * ratio;
    }
    TempRPos = RPos;
}


void RailInfo(struct RPos_s *rpos, struct nuvec_s *pos, u16 *angle,
              u16 *cam_angle, u8 *mode) {
    struct rail_s *rail;
    struct nuvec_s *pL;
    struct nuvec_s *pR;
    struct nuvec_s *pC;
    struct nuvec_s *p0;
    struct nuvec_s *p1;
    struct nuvec_s v0;
    struct nuvec_s v1;
    struct nuvec_s mid;
    struct nuvec_s vC;
    s32 i0;
    s32 i1;
    s32 rot0;
    s32 rot1;
    u16 ang;
    f32 dx;
    f32 dz;
    f32 l;
    f32 dp;

    if (rpos->iRAIL == -1 || rpos->iALONG == -1) {
        return;
    }
    i0 = rpos->iALONG;
    i1 = i0 + 1;
    rail = &Rail[rpos->iRAIL];
    if (i1 == rail->edges && rail->circuit != 0) {
        i1 = 0;
    }
    if (pos != 0) {
        p0 = (struct nuvec_s *)(rail->pCAM->pts + i0 * rail->pCAM->ptsize);
        p1 = (struct nuvec_s *)(rail->pCAM->pts + i1 * rail->pCAM->ptsize);
        pos->x = p0->x + (p1->x - p0->x) * rpos->fALONG;
        pos->y = p0->y + (p1->y - p0->y) * rpos->fALONG;
        pos->z = p0->z + (p1->z - p0->z) * rpos->fALONG;
    }
    if (angle != 0) {
        pL = (struct nuvec_s *)(rail->pLEFT->pts + i0 * rail->pLEFT->ptsize);
        pR = (struct nuvec_s *)(rail->pRIGHT->pts + i0 * rail->pRIGHT->ptsize);
        v0.x = (pL->x + pR->x) * 0.5f;
        v0.z = (pL->z + pR->z) * 0.5f;
        rot0 = NuAtan2D(pR->x - pL->x, pR->z - pL->z);
        pL = (struct nuvec_s *)(rail->pLEFT->pts + i1 * rail->pLEFT->ptsize);
        pR = (struct nuvec_s *)(rail->pRIGHT->pts + i1 * rail->pRIGHT->ptsize);
        v1.x = (pL->x + pR->x) * 0.5f;
        v1.z = (pL->z + pR->z) * 0.5f;
        rot1 = NuAtan2D(pR->x - pL->x, pR->z - pL->z);
        *angle = rot0 + (s32)(rpos->fALONG * RotDiff(rot0, rot1)) - 0x4000;
        if (mode != 0) {
            if (pos != 0) {
                ang = *angle;
                mid.x = v0.x + (v1.x - v0.x) * rpos->fALONG;
                mid.z = v0.z + (v1.z - v0.z) * rpos->fALONG;
                v0.x = NuTrigTable[ang];
                v0.z = NuTrigTable[(ang + 0x4000) & 0xFFFF];
                dx = mid.x - pos->x;
                dz = mid.z - pos->z;
                l = 1.0f / NuFsqrt(dx * dx + dz * dz);
                v1.x = dx * l;
                v1.z = dz * l;
                dp = v0.x * v1.x + v0.z * v1.z;
                if (dp > 0.866f) {
                    *mode = 1;
                } else if (dp < -0.866f) {
                    *mode = 2;
                } else if (fabs(dp) < 0.5) {
                    if (RotDiff(NuAtan2D(dx, dz), *angle) > 0) {
                        *mode = 4;
                    } else {
                        *mode = 8;
                    }
                } else {
                    *mode = 0;
                }
            } else {
                *mode = 0;
            }
        }
    }
    if (cam_angle != 0) {
        pC = (struct nuvec_s *)(rail->pCAM->pts + i0 * rail->pCAM->ptsize);
        pL = (struct nuvec_s *)(rail->pLEFT->pts + i0 * rail->pLEFT->ptsize);
        pR = (struct nuvec_s *)(rail->pRIGHT->pts + i0 * rail->pRIGHT->ptsize);
        v0.x = (pL->x + pR->x) * 0.5f;
        v0.z = (pL->z + pR->z) * 0.5f;
        vC = *pC;
        if (Level == 6 || Level == 0x22) {
            vC.x += 5.0f;
        }
        rot0 = NuAtan2D(v0.x - vC.x, v0.z - vC.z);
        pC = (struct nuvec_s *)(rail->pCAM->pts + i1 * rail->pCAM->ptsize);
        pL = (struct nuvec_s *)(rail->pLEFT->pts + i1 * rail->pLEFT->ptsize);
        pR = (struct nuvec_s *)(rail->pRIGHT->pts + i1 * rail->pRIGHT->ptsize);
        v1.x = (pL->x + pR->x) * 0.5f;
        v1.z = (pL->z + pR->z) * 0.5f;
        vC = *pC;
        if (Level == 6 || Level == 0x22) {
            vC.x += 5.0f;
        }
        rot1 = NuAtan2D(v1.x - vC.x, v1.z - vC.z);
        *cam_angle = rot0 + (s32)(rpos->fALONG * RotDiff(rot0, rot1));
    }
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

/* BlendGameCamera, InSplineArea and LookUpDownRail are public inline
 * functions: MoveGameCamera expands all three, and gcc 2.95 defers their
 * out-of-line copies to the end of the unit (they sit after MoveGameCamera
 * in retail). */
inline void BlendGameCamera(struct cammtx_s *cam, f32 time) {
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

inline s32 InSplineArea(struct nuvec_s *pos, struct nugspline_s *spl) {
    struct nuvec_s *p0;
    struct nuvec_s *p1;
    s32 i;
    s32 j;

    for (i = 1; i < spl->len; i++) {
        p1 = (struct nuvec_s *)((nusintptr)spl->pts + i * spl->ptsize);
        j = i + 1;
        if (j == spl->len) {
            j = 1;
        }
        p0 = (struct nuvec_s *)((nusintptr)spl->pts + j * spl->ptsize);
        if (!((pos->x - p1->x) * (p0->z - p1->z) +
              (pos->z - p1->z) * (p1->x - p0->x) >= 0.0f)) {
            return 0;
        }
    }
    return 1;
}

/* ------------------------------------------------------------------ */
/* MoveGameCamera                                                      */
/* ------------------------------------------------------------------ */

extern struct nucamera_s *pNuCam;
extern struct spltab_s SplTab[];
extern struct cursor_s Cursor;
extern struct gtimer_s GameTimer;
extern struct gtimer_s GlobalTimer;
extern struct nupad_s *Pad[];
extern struct cammtx_s GameCam[];
extern struct anim_s TempAnim;
extern struct numtx_s cutscenecammtx;

extern s32 set_cutscenecammtx;
extern s32 cut_on;
extern struct nugspline_s *pVIS;
extern s32 iVIS;
extern s32 iTEMP;
extern s32 PLAYERCOUNT;
extern s32 VEHICLECONTROL;
extern s32 GameMode;
extern s32 level_part_2;
extern u64 LBIT;
extern s32 CrunchTime_Intro;
extern s32 DrainDamage_Intro;
extern s32 SpaceCortex_Defeated;
extern s32 temp_creature_i;

extern u16 best_railangle;

extern f32 vtog_time;
extern f32 vtog_duration;
extern s32 vtog_blend;
extern struct vtog_s *pVTog;

extern struct nugspline_s *bonus_pCAM;
extern f32 bonus_time;
extern f32 bonus_duration;
extern struct nugspline_s *death_pCAM;
extern f32 death_time;
extern f32 death_duration;
extern struct nugspline_s *gempath_pCAM;
extern f32 gempath_time;
extern f32 gempath_duration;

extern struct nuvec_s *pos_START;
extern struct nuvec_s v000;
extern struct nuvec_s JeepvObj;
extern struct nuvec_s JeepvPos;
extern struct nuvec_s campos_SPACE;
extern struct nuvec_s cutcampos_FRONTEND;
extern struct nuvec_s cutcamlook_FRONTEND;
extern struct nuvec_s BOSSCAMPOS;
extern struct nuvec_s BOSSCAMPOS2;
extern f32 ATLASCAMHEIGHT;
extern s32 ResetAtlasCamera;
extern f32 LOGOCAMX;
extern f32 LOGOCAMY;
extern f32 LOGOCAMZ;
extern f32 CAMSTOPNEAR;
extern f32 CAMSTOPFAR;
extern f32 SIDECAMDISTANCE;
extern f32 WesternCountdown;

/* {1,1,1} in camera.c's .rodata: the jeep cameras' seek rate. */
/* MoveGameCamera's function-static rail blend factor. */
extern f32 D_00630AA8;
#define rail_blend D_00630AA8

extern f32 FindNearestCreature(struct nuvec_s *pos, s32 type,
                               struct nuvec_s *out);
extern void RumbleCam(struct cammtx_s *cam);
extern void GliderCam(struct cammtx_s *cam);
extern void JeepCam(struct cammtx_s *cam);
extern void JeepCamFollowAng(struct cammtx_s *cam, s32 a1);
extern void JeepCamIntro(struct cammtx_s *cam);
extern void NuCameraSet(struct nucamera_s *cam);
extern u16 TurnRot(u16 a, u16 b, s32 rate);
extern u16 SeekRot(u16 a, u16 dist, s32 rate);
extern void PointAlongSpline(struct nugspline_s *spl, f32 ratio,
                             struct nuvec_s *pos, void *a3, void *a4);

inline f32 LookUpDownRail(struct obj_s *obj, u16 angle, s32 mode) {
    s32 dyrot;
    s32 dir;

    dyrot = abs(RotDiff(angle, obj->hdg));
    if (dyrot < 0x2AAB) {
        dir = 0;
    } else {
        if (dyrot < 0x5556) {
            return 0.0f;
        }
        dir = 1;
    }
    if ((mode & 3) && (dir == 1)) {
        return -2.0f;
    }
    return 0.0f;
}

/* Global-timer wobble: one sine period every n frames, as a trig index. */
#define WOBBLE(n) NuTrigTable[(((GlobalTimer.frame % (n)) << 16) / (n)) & 0xFFFF]

/* Per-frame game camera.
 *
 * 1. Pick GameCamera->mode from the level and game state (hub menus, bonus /
 *    death / gem-path flythroughs, vehicle switches, boss fights, rails, the
 *    trigger splines SplTab[8..11]) and blend when the mode changes.
 * 2. Compute the target camera position `pos` and look-at point `look` for
 *    that mode (the switch). Some modes hand the whole matrix to a vehicle
 *    camera and jump straight to Finish.
 * 3. Ease GameCamera->pos toward `pos` with per-axis `seek` rates, then derive
 *    the pitch/yaw/roll, apply judder and build the view matrix.
 *
 * Shapes kept to match retail codegen: the float temporaries are shared the
 * way the original shared them (`f` is general scratch, `t` the spline
 * parameter, `dist` a distance or seek target); cases 0..3 are listed so the
 * jump table is indexed from mode 0; the 0xB sweep keeps its period in `i`.
 * The PAL build runs at 50Hz, so per-frame rates are the GC values x1.2. */
void MoveGameCamera(struct cammtx_s *GameCamera, struct obj_s *obj) {
    struct RPos_s RPos;
    struct nuvec_s pos;          /* where the camera wants to be */
    struct nuvec_s look;         /* where it looks */
    struct nuvec_s seek;         /* per-axis seek rate toward pos */
    struct nuvec_s tmp;
    struct nuvec_s splpos[2];    /* scripted-intro camera / look splines */
    struct nuvec_s railpos[2];   /* mode 0xC: near [0] and far [1] camera */
    struct nuvec_s raillook[2];  /* mode 0xC: near [0] and far [1] look-at */
    struct nuvec_s d;
    struct nuvec_s along;
    struct nuvec_s shift;
    struct creature_s *c;
    struct creature_s *plr;
    struct anim_s *anim;
    struct nugspline_s *spl;
    struct nuvec_s *p0;
    struct nuvec_s *p1;
    s32 xrot_fixed;
    s32 yrot_fixed;
    s32 zrot_fixed;
    s32 oldmode;
    s32 xseek;
    s32 yseek;
    s32 vehicle;
    u16 cam_angle;
    s32 iSPL;
    s32 character;
    s32 old_vertical;
    s32 stride;
    s32 behind;
    s32 tilt;
    s32 xrot;
    s32 yrot;
    s32 zrot;
    u16 ang;
    s32 idx;
    s32 rot;
    s32 i;
    s32 n;
    s32 railsum;
    f32 t;
    f32 f;
    f32 countdown;
    f32 cd_min;
    f32 cd_max;
    f32 height;
    f32 back;    /* mode 0xC: distance behind the player */
    f32 side;    /* mode 0xC: allowed sideways drift from the rail */
    f32 span;
    f32 inv;
    f32 raillen;
    f32 dist;
    f32 dx;
    f32 dy;
    f32 dz;

    pVIS = NULL;
    vehicle = -1;
    cut_on = set_cutscenecammtx;
    if (set_cutscenecammtx != 0) {
        GameCamera->m = cutscenecammtx;
        set_cutscenecammtx = 0;
        goto Finish;
    }

    if ((PLAYERCOUNT != 0) && (VEHICLECONTROL == 1) &&
        (player->obj.vehicle != -1)) {
        vehicle = player->obj.vehicle;
    }

    /* --- pick the camera mode --- */
    oldmode = GameCamera->mode;
    c = NULL;
    GameCamera->mode = 0;
    if (Level == 0x23) {
        GameCamera->mode = 8;
        if ((SplTab[12].spl != NULL) && (SplTab[13].spl != NULL) &&
            ((i = CRemap[96]) != -1) && ((CModel + i)->anmdata[0x2B] != NULL)) {
            anim = &TempAnim;
            GameCamera->mode = 0x18;
        }
    } else if (Level == 0x26) {
        if (SplTab[1].spl != NULL) {
            GameCamera->mode = 10;
        } else {
            GameCamera->mode = 0x19;
        }
    } else if (Level == 0x28) {
        GameCamera->mode = 0x20;
    } else if (Level == 0x2B) {
        GameCamera->mode = 0x22;
    } else if (Level == 0x19) {
        character = 2;
        GameCamera->mode = 0x1A;
        if (CrunchTime_Intro != 0) {
            character = 0x7F;
        }
        if (FindNearestCreature(&pos, character, &tmp) < 1000000.0f) {
            c = &Character[temp_creature_i];
            if (((CrunchTime_Intro != 0) || (c->obj.anim.newaction == 0x49) ||
                 (c->obj.anim.newaction == 0x46)) &&
                (SpaceCortex_Defeated == 0)) {
                GameCamera->mode = 0x1B;
            }
        }
        if ((oldmode != -1) && (GameCamera->mode != oldmode)) {
            BlendGameCamera(GameCam, 1.0f);
        }
    } else if ((Level == 0xD) || (Level == 0x12) || (Level == 0x1A)) {
        GameCamera->mode = 0xE;
    } else if ((Level == 0x18) || (Level == 0x24)) {
        GameCamera->mode = 0xE;
    } else if ((Level == 0x1E) && (level_part_2 != 0)) {
        GameCamera->mode = 0xE;
    } else if (Level == 0x25) {
        if (((Cursor.menu == 0x13) ||
             ((Cursor.wait != 0) && (Cursor.new_menu == 0x13)) ||
             (Cursor.menu == 0x14) || (Cursor.menu == 0x17) ||
             (Cursor.menu == 0x1A) || (Cursor.menu == 0x16) ||
             (Cursor.menu == 0x19) || (Cursor.menu == 0x18) ||
             (Cursor.menu == 0x15) || (Cursor.menu == 0x1B) ||
             (Cursor.menu == 0x1D) || (Cursor.menu == 0x1C) ||
             (Cursor.menu == 0x1E)) &&
            (SplTab[65].spl != NULL) && (SplTab[66].spl != NULL)) {
            GameCamera->mode = 0x1F;
        } else if (GameMode == 1) {
            GameCamera->mode = 0x21;
        } else if ((Death == 1) || (Death == 3)) {
            GameCamera->mode = 6;
        } else if ((nRAILS != 0) && (best_cRPos != NULL)) {
            GameCamera->mode = 5;
        } else if (Death != 2) {
            GameCamera->mode = 9;
        } else {
            GameCamera->mode = 4;
        }
    } else if (PLAYERCOUNT != 0) {
        if ((Bonus == 1) || (Bonus == 3) || (Death == 1) || (Death == 3) ||
            (GemPath == 1) || (GemPath == 3)) {
            GameCamera->mode = 6;
        } else if ((vtog_time < vtog_duration) && (vtog_blend != 0) &&
                   (pVTog != NULL) && (pVTog->pCAM != NULL)) {
            GameCamera->mode = 0x1C;
        } else if ((Level == 6) || (Level == 0x22)) {
            GameCamera->mode = 5;
        } else if ((LBIT & 0x100210801) != 0) {
            GameCamera->mode = 0x10;
        } else if ((obj->flags & 1) &&
                   ((obj->dead == 2) || (obj->dead == 3) || (obj->dead == 8))) {
            GameCamera->mode = 0x1E;
        } else if ((nRAILS != 0) && (best_cRPos != NULL)) {
            GameCamera->mode = 5;
        } else if (SplTab[1].spl != NULL) {
            GameCamera->mode = 10;
        } else {
            GameCamera->mode = 4;
        }
    }

    if ((SplTab[8].spl != NULL) && InSplineArea(&obj->pos, SplTab[8].spl)) {
        GameCamera->mode = 0x16;
    } else if ((SplTab[9].spl != NULL) &&
               InSplineArea(&obj->pos, SplTab[9].spl)) {
        GameCamera->mode = 0x17;
        iSPL = 0;
    } else if ((SplTab[10].spl != NULL) &&
               InSplineArea(&obj->pos, SplTab[10].spl)) {
        GameCamera->mode = 0x17;
        iSPL = 1;
    } else if ((SplTab[11].spl != NULL) &&
               InSplineArea(&obj->pos, SplTab[11].spl)) {
        GameCamera->mode = 0x17;
        iSPL = 2;
    }

    if ((Level == 0x17) && (GameCamera->mode == 5)) {
        if (FindNearestCreature(&pos, 0x7F, &tmp) < 1000000.0f) {
            c = &Character[temp_creature_i];
            if (DrainDamage_Intro != 0) {
                GameCamera->mode = 0x1B;
            }
        }
        if ((oldmode != -1) && (GameCamera->mode != oldmode)) {
            BlendGameCamera(GameCam, 1.0f);
        }
    }

    if (GameCamera->mode != oldmode) {
        if ((GameCamera->mode == 0x17) || (oldmode == 0x17)) {
            BlendGameCamera(GameCamera, 0.5f);
        } else if ((GameCamera->mode == 0x16) || (oldmode == 0x16) ||
                   (oldmode == 0x1C) ||
                   ((oldmode == 0x15) && (GameCamera->mode == 5)) ||
                   ((GameCamera->mode == 0x1F) && (oldmode == 9)) ||
                   ((GameCamera->mode == 9) && (oldmode == 0x1F))) {
            BlendGameCamera(GameCamera, 1.0f);
        }
    }

    /* --- defaults --- */
    seek.x = 0.14999999f;
    seek.y = 0.14999999f;
    seek.z = 0.14999999f;
    if (pos_START != NULL) {
        pos = *pos_START;
    } else {
        pos = v000;
    }
    if (GameCamera->mode != oldmode) {
        GameCamera->iRAIL = GameCamera->iALONG = -1;
    }
    xrot_fixed = -1;
    yrot_fixed = -1;
    zrot_fixed = 0;
    look.x = obj->pos.x;
    look.y = obj->pos.y + obj->max.y * obj->SCALE * 0.5f;
    look.z = obj->pos.z;
    xseek = 3;
    yseek = 4;

    /* --- per-mode dist --- */
    switch (GameCamera->mode) {
    case 0x16:
        look.x = obj->pos.x;
        look.y = obj->pos.y + (obj->min.y + obj->max.y) * obj->SCALE * 0.5f;
        look.z = obj->pos.z;
        stride = SplTab[8].spl->ptsize;
        p0 = (struct nuvec_s *)SplTab[8].spl->pts;
        p1 = (struct nuvec_s *)((u8 *)p0 + stride);
        pos.x = look.x + (p1->x - p0->x);
        pos.y = look.y + (p1->y - p0->y);
        pos.z = look.z + (p1->z - p0->z);
        break;

    case 0x17:
        yseek = 3;
        look.x = obj->pos.x;
        look.y = obj->pos.y + (obj->min.y + obj->max.y) * obj->SCALE * 0.5f;
        look.z = obj->pos.z;
        pos = *(struct nuvec_s *)SplTab[iSPL + 9].spl->pts;
        break;

    case 0x10:
        if (Level == 0x15) {
            RumbleCam(GameCamera);
            pNuCam->mtx = GameCamera->m;
            NuCameraSet(pNuCam);
            goto Finish;
        }
        if (GameTimer.frame < 100) {
            height = 2.5f;
        } else {
            height = 5.0f;
        }
        if (ATLASCAMHEIGHT > height) {
            ATLASCAMHEIGHT -= 0.04f;
            if (ATLASCAMHEIGHT < height) {
                ATLASCAMHEIGHT = height;
            }
        } else if (ATLASCAMHEIGHT < height) {
            ATLASCAMHEIGHT += 0.04f;
            if (ATLASCAMHEIGHT > height) {
                ATLASCAMHEIGHT = height;
            }
        }
        look.x = obj->pos.x;
        look.y = obj->pos.y + (obj->min.y + obj->max.y) * obj->SCALE * 0.5f;
        look.z = obj->pos.z;
        pos.x = look.x + ATLASCAMHEIGHT;
        pos.y = look.y + ATLASCAMHEIGHT;
        pos.z = look.z - ATLASCAMHEIGHT;
        if (ResetAtlasCamera != 0) {
            ResetAtlasCamera = 0;
            GameCamera->pos = pos;
            GameCamera->blend_time = GameCamera->blend_duration;
        }
        break;

    case 0xE:
        GliderCam(GameCamera);
        pNuCam->mtx = GameCamera->m;
        NuCameraSet(pNuCam);
        goto Finish;

    case 0xF:
        GameCamera->seek = (struct nuvec_s){1.0f, 1.0f, 1.0f};
        seek = (struct nuvec_s){1.0f, 1.0f, 1.0f};
        look = JeepvObj;
        GameCamera->pos = pos = JeepvPos;
        break;

    case 0x1D:
        JeepCam(GameCamera);
        GameCamera->seek = (struct nuvec_s){1.0f, 1.0f, 1.0f};
        seek = (struct nuvec_s){1.0f, 1.0f, 1.0f};
        look = JeepvObj;
        GameCamera->pos = pos = JeepvPos;
        look = JeepvObj;
        GameCamera->seek = (struct nuvec_s){1.0f, 1.0f, 1.0f};
        break;

    case 0x14:
        JeepCamFollowAng(GameCamera, 0);
        pNuCam->mtx = GameCamera->m;
        NuCameraSet(pNuCam);
        goto Finish;

    case 0x15:
        JeepCamIntro(GameCamera);
        pNuCam->mtx = GameCamera->m;
        NuCameraSet(pNuCam);
        goto Finish;

    case 0x11:
        JeepCam(GameCamera);
        GameCamera->seek = (struct nuvec_s){1.0f, 1.0f, 1.0f};
        seek = (struct nuvec_s){1.0f, 1.0f, 1.0f};
        look = JeepvObj;
        GameCamera->pos = pos = JeepvPos;
        break;

    case 0xC:
        /* blend between a close rail camera (0) and a far one (1) */
        behind = abs(RotDiff(-GameCamera->yrot, obj->hdg)) > 0x4000;
        if (behind) {
            if (rail_blend > 1.0f) {
                rail_blend -= 0.025f;
                if (rail_blend < 1.0f) {
                    rail_blend = 1.0f;
                }
            } else if (rail_blend < 1.0f) {
                rail_blend += 0.025f;
                if (rail_blend > 1.0f) {
                    rail_blend = 1.0f;
                }
            }
        } else {
            if (rail_blend > 0.0f) {
                rail_blend -= 0.025f;
                if (rail_blend < 0.0f) {
                    rail_blend = 0.0f;
                }
            } else if (rail_blend < 0.0f) {
                rail_blend += 0.025f;
                if (rail_blend > 0.0f) {
                    rail_blend = 0.0f;
                }
            }
        }
        RPos = obj->RPos;
        if ((RPos.iRAIL != -1) && (RPos.iALONG != -1)) {
            back = 5.0f;
            if (rail_blend != 1.0f) {
                MoveRailPosition(&railpos[0], &RPos, 3.0f, 1);
                GameCamera->iRAIL = TempRPos.iRAIL;
                GameCamera->iALONG = TempRPos.iALONG;
                MoveRailPosition(&raillook[0], &RPos, 3.0f, 0);
                d.x = railpos[0].x - raillook[0].x;
                d.y = railpos[0].y - raillook[0].y;
                d.z = railpos[0].z - raillook[0].z;
                span = sqrt(d.x * d.x + d.y * d.y + d.z * d.z);
                raillook[0].x = obj->pos.x;
                raillook[0].y = obj->pos.y + obj->top * obj->SCALE + 0.5f;
                raillook[0].z = obj->pos.z;
                side = 2.0f;
                railpos[0].x = raillook[0].x + (d.x * back) / span;
                railpos[0].y = raillook[0].y + (d.y * back) / span;
                railpos[0].z = raillook[0].z + (d.z * back) / span;
                MoveRailPosition(&along, &RPos, 3.0f, 1);
                d.x = railpos[0].x - along.x;
                d.z = railpos[0].z - along.z;
                raillen = sqrt(d.x * d.x + d.z * d.z);
                if (raillen > side) {
                    shift.x = (d.x * (raillen - side)) / raillen;
                    shift.z = (d.z * (raillen - side)) / raillen;
                    railpos[0].x -= shift.x;
                    railpos[0].z -= shift.z;
                    raillook[0].x -= shift.x;
                    raillook[0].z -= shift.z;
                }
            }
            if (rail_blend != 0.0f) {
                raillook[1].x = obj->pos.x;
                raillook[1].y = obj->pos.y + obj->top * obj->SCALE + 0.5f;
                raillook[1].z = obj->pos.z;
                MoveRailPosition(&railpos[1], &RPos, 7.0f, 1);
                GameCamera->iRAIL = TempRPos.iRAIL;
                GameCamera->iALONG = TempRPos.iALONG;
            }
            look.x = (1.0f - rail_blend) * raillook[0].x + rail_blend * raillook[1].x;
            look.y = (1.0f - rail_blend) * raillook[0].y + rail_blend * raillook[1].y;
            look.z = (1.0f - rail_blend) * raillook[0].z + rail_blend * raillook[1].z;
            pos.x = (1.0f - rail_blend) * railpos[0].x + rail_blend * railpos[1].x;
            pos.y = (1.0f - rail_blend) * railpos[0].y + rail_blend * railpos[1].y;
            pos.z = (1.0f - rail_blend) * railpos[0].z + rail_blend * railpos[1].z;
        } else {
            pos = GameCamera->pos;
        }
        break;

    case 0xD:
        pos.x = obj->pos.x;
        pos.y = obj->pos.y + obj->top * obj->SCALE + 0.5f;
        pos.z = obj->pos.z;
        pos = look;
        pos.y += 10.0f;
        pos.z += 1.0f;
        break;

    case 0x1E:
        look.x = obj->pos.x;
        look.y = obj->pos.y + (obj->bot + obj->top) * obj->SCALE * 0.5f;
        look.z = obj->pos.z;
        NuVecSub(&tmp, &GameCamera->pos, &look);
        NuVecNorm(&tmp, &tmp);
        pos.x = look.x + tmp.x * 3.0f;
        pos.y = look.y + tmp.y * 3.0f;
        pos.z = look.z + tmp.z * 3.0f;
        if (obj->dead == 3) {
            look.y += 1.0f;
        }
        break;

    case 5:
        /* follow the camera rails */
        if ((best_cRPos != NULL) && (cRPosCOUNT != 0)) {
            old_vertical = GameCamera->vertical;
            GameCamera->vertical = best_cRPos->vertical;
            if (GameCamera->vertical != old_vertical) {
                BlendGameCamera(GameCamera, 1.0f);
            }
            if ((Level == 0x25) || (Level == 6) || (Level == 0x22)) {
                GameCamera->distance = 0.0f;
            } else {
                plr = (struct creature_s *)obj->parent;
                if ((plr != player) || (plr->slam != 2) ||
                    (plr->slam_wait != 0)) {
                    if (best_cRPos->vertical == 0) {
                        dist = LookUpDownRail(obj, best_cRPos->angle,
                                                best_cRPos->mode);
                    } else {
                        dist = 0.0f;
                    }
                    if (GameCamera->distance > dist) {
                        GameCamera->distance -= 0.04f;
                        if (GameCamera->distance < dist) {
                            GameCamera->distance = dist;
                        }
                    } else if (GameCamera->distance < dist) {
                        GameCamera->distance += 0.04f;
                        if (GameCamera->distance > dist) {
                            GameCamera->distance = dist;
                        }
                    }
                }
            }
            pos = v000;
            for (i = 0; i < cRPosCOUNT; i++) {
                MoveRailPosition(&tmp, &cRPos[i], fabs(GameCamera->distance),
                                 (GameCamera->distance > 0.0f) ? 0 : 1);
                if (best_cRPos == &cRPos[i]) {
                    pVIS = Rail[cRPos[i].iRAIL].pCAM;
                    iVIS = TempRPos.iALONG;
                }
                if (cRPos[i].vertical != 0) {
                    tmp.y = obj->pos.y + obj->max.y * obj->SCALE;
                    p0 = (struct nuvec_s *)(Rail[cRPos[i].iRAIL].pCAM->pts +
                                            cRPos[i].i1 * Rail[cRPos[i].iRAIL].pCAM->ptsize);
                    p1 = (struct nuvec_s *)(Rail[cRPos[i].iRAIL].pCAM->pts +
                                            cRPos[i].i2 * Rail[cRPos[i].iRAIL].pCAM->ptsize);
                    if (tmp.y < p0->y) {
                        tmp.y = p0->y;
                    } else if (tmp.y > p1->y) {
                        tmp.y = p1->y;
                    }
                }
                NuVecAdd(&pos, &pos, &tmp);
            }
            f = 1.0f / cRPosCOUNT;
            best_railangle = cRPos[0].angle;
            cam_angle = cRPos[0].cam_angle;
            pos.x *= f;
            pos.y *= f;
            pos.z *= f;
            if (cRPosCOUNT > 1) {
                n = 0;
                railsum = 0;
                for (i = 1; i < cRPosCOUNT; i++) {
                    n += RotDiff(cam_angle, cRPos[i].cam_angle);
                    railsum += RotDiff(best_railangle, cRPos[i].angle);
                }
                cam_angle += n / (cRPosCOUNT - 1);
                best_railangle += railsum / (cRPosCOUNT - 1);
            }
        } else {
            pos = GameCamera->pos;
        }
        look.x = obj->pos.x;
        look.y = obj->pos.y + (obj->min.y + obj->max.y) * obj->SCALE * 0.5f;
        look.z = obj->pos.z;

        /* look ahead along the rail */
        dist = 0.0f;
        f = 0.04f;
        if ((VEHICLECONTROL != 2) &&
            ((VEHICLECONTROL != 1) || (obj->vehicle != 0x20)) &&
            (best_cRPos != NULL) && (best_cRPos->mode & 0xC)) {
            if (Level == 0x17) {
                dist = -2.0f;
            } else {
                dist = NuTrigTable[(RotDiff(obj->hdg, best_cRPos->angle) + 0x4000) & 0xFFFF];
            }
            f = fabs(dist) * 0.02f;
        }
        if (GameCamera->ahead > dist) {
            GameCamera->ahead -= f;
            if (GameCamera->ahead < dist) {
                GameCamera->ahead = dist;
            }
        } else if (GameCamera->ahead < dist) {
            GameCamera->ahead += f;
            if (GameCamera->ahead > dist) {
                GameCamera->ahead = dist;
            }
        }
        if (GameCamera->ahead != 0.0f) {
            dx = NuTrigTable[best_cRPos->angle] * GameCamera->ahead;
            dz = NuTrigTable[(best_cRPos->angle + 0x4000) & 0xFFFF] * GameCamera->ahead;
            look.x += dx;
            look.z += dz;
            pos.x += dx;
            pos.z += dz;
        }
        if (vehicle == 0xA1) {
            xseek = 2;
            yseek = 3;
        }

        /* scripted level intros */
        if ((Level == 0x1D) && (SplTab[68].spl != NULL) &&
            (SplTab[69].spl != NULL)) {
            if (GameTimer.ftime < 3.0f) {
                t = GameTimer.ftime / 3.0f;
                PointAlongSpline(SplTab[68].spl, t, &splpos[0], NULL, NULL);
                PointAlongSpline(SplTab[69].spl, t, &splpos[1], NULL, NULL);
                inv = 1.0f - t;
                NuVecScale(&splpos[0], &splpos[0], inv);
                NuVecScale(&splpos[1], &splpos[1], inv);
                NuVecScaleAccum(&splpos[0], &pos, t);
                NuVecScaleAccum(&splpos[1], &look, t);
                pos = splpos[0];
                look = splpos[1];
            }
        } else if ((Level == 3) && (SplTab[68].spl != NULL) &&
                   (SplTab[69].spl != NULL)) {
            countdown = WesternCountdown;
            cd_min = 1.0f;
            cd_max = 6.0f;
            if (countdown < cd_min) {
                t = 1.0f;
            } else if (countdown > cd_max) {
                t = 0.0f;
            } else {
                t = (5.0f - (countdown - cd_min)) / 5.0f;
            }
            if (t > 0.95f) {
                t = 0.95f;
            }
            PointAlongSpline(SplTab[68].spl, t, &splpos[0], NULL, NULL);
            PointAlongSpline(SplTab[69].spl, t, &splpos[1], NULL, NULL);
            if (countdown < cd_min) {
                t = 1.0f;
            } else if (countdown > cd_max) {
                t = 0.0f;
            } else {
                t = ((cd_max - cd_min) - (countdown - cd_min)) / (cd_max - cd_min);
            }
            if (t != 0.0f) {
                t = t * t;
                f = 1.0f;
                f -= t;
                NuVecScale(&splpos[0], &splpos[0], f);
                NuVecScale(&splpos[1], &splpos[1], f);
                NuVecScaleAccum(&splpos[0], &pos, t);
                NuVecScaleAccum(&splpos[1], &look, t);
                pos = splpos[0];
                look = splpos[1];
            }
        }
        break;

    case 0x1F:
        /* hub menu camera */
        if (Cursor.wait != 0) {
            if (Cursor.new_menu == 0x13) {
                t = (f32)(Cursor.wait_frames - Cursor.wait) / (f32)Cursor.wait_frames;
            } else {
                t = (f32)Cursor.wait / (f32)Cursor.wait_frames;
            }
        } else if (Cursor.menu != -1) {
            t = 1.0f;
        } else {
            t = 0.0f;
        }
        PointAlongSpline(SplTab[65].spl, t, &pos, NULL, NULL);
        pos.x += WOBBLE(206) * 0.1f;
        pos.y += WOBBLE(159) * 0.1f;
        pos.z += WOBBLE(178) * 0.1f;
        pVIS = death_pCAM;
        iVIS = iTEMP;
        PointAlongSpline(SplTab[66].spl, t, &look, NULL, NULL);
        break;

    case 6:
        /* bonus / death / gem path flythrough */
        seek.x = 0.1f;
        seek.y = 0.1f;
        seek.z = 0.1f;
        look.x = obj->pos.x;
        look.y = obj->pos.y + (obj->min.y + obj->max.y) * obj->SCALE * 0.5f;
        look.z = obj->pos.z;
        if ((Bonus == 1) || (Bonus == 3)) {
            t = bonus_time / bonus_duration;
            PointAlongSpline(bonus_pCAM,
                             -NuTrigTable[(s32)(t * 32768.0f + 16384.0f) & 0xFFFF] * 0.5f + 0.5f,
                             &pos, NULL, NULL);
            pVIS = bonus_pCAM;
            iVIS = iTEMP;
        } else if ((Death == 1) || (Death == 3)) {
            t = death_time / death_duration;
            if (Death == 3) {
                t = 1.0f - t;
            }
            PointAlongSpline(death_pCAM,
                             -NuTrigTable[(s32)(t * 32768.0f + 16384.0f) & 0xFFFF] * 0.5f + 0.5f,
                             &pos, NULL, NULL);
            pVIS = death_pCAM;
            iVIS = iTEMP;
        } else {
            t = gempath_time / gempath_duration;
            if (GemPath == 3) {
                t = 1.0f - t;
            }
            PointAlongSpline(gempath_pCAM,
                             -NuTrigTable[(s32)(t * 32768.0f + 16384.0f) & 0xFFFF] * 0.5f + 0.5f,
                             &pos, NULL, NULL);
            pVIS = gempath_pCAM;
            iVIS = iTEMP;
        }
        break;

    case 0x1C:
        t = vtog_time / vtog_duration;
        seek.x = 0.2f;
        seek.y = 0.2f;
        seek.z = 0.2f;
        PointAlongSpline(pVTog->pCAM, t, &pos, NULL, NULL);
        if (pVTog->pLOOK != NULL) {
            PointAlongSpline(pVTog->pLOOK, t, &look, NULL, NULL);
        }
        break;

    case 0x18:
        f = anim->anim_time - 1.0f;
        if (f > 30.0f) {
            f = 30.0f;
        }
        f *= 0.033333335f;
        PointAlongSpline(SplTab[12].spl, f, &pos, NULL, NULL);
        PointAlongSpline(SplTab[13].spl, f, &look, NULL, NULL);
        break;

    case 10:
        spl = SplTab[1].spl;
        pos = *(struct nuvec_s *)spl->pts;
        look = *(struct nuvec_s *)(spl->pts + spl->ptsize);
        if (Level == 0x26) {
            pos.x += WOBBLE(518) * 0.05f;
            pos.y += WOBBLE(409) * 0.05f;
            pos.z += WOBBLE(396) * 0.05f;
        }
        break;

    case 0x20:
        oldmode = -1;
        pos = campos_SPACE;
        look = v000;
        break;

    case 0x22:
        oldmode = -1;
        look.x = 0.0f;
        look.z = 0.0f;
        look.y = (CData[187].min.y + CData[187].max.y) * 0.5f;
        pos.x = NuTrigTable[0x8000] * 3.0f + look.x;
        pos.z = NuTrigTable[0xC000] * 3.0f + look.z;
        pos.y = look.y;
        break;

    case 0x21:
        pos = cutcampos_FRONTEND;
        look = cutcamlook_FRONTEND;
        pos.x += WOBBLE(206) * 0.1f;
        pos.y += WOBBLE(159) * 0.1f;
        pos.z += WOBBLE(178) * 0.1f;
        break;

    case 0x1A:
        pos = BOSSCAMPOS;
        look.x = obj->pos.x;
        look.y = obj->pos.y + (obj->min.y + obj->max.y) * obj->SCALE * 0.5f;
        look.z = obj->pos.z;
        break;

    case 0x1B:
        /* boss camera: look between the player and the boss */
        if (Level == 0x17) {
            pos = BOSSCAMPOS2;
        } else {
            pos = BOSSCAMPOS;
        }
        look.x = obj->pos.x;
        look.y = obj->pos.y + (obj->min.y + obj->max.y) * obj->SCALE * 0.5f + 0.666f;
        look.z = obj->pos.z;
        tmp.x = c->obj.pos.x;
        tmp.y = c->obj.pos.y + (c->obj.min.y + c->obj.max.y) * c->obj.SCALE * 0.5f + 0.666f;
        tmp.z = c->obj.pos.z;
        NuVecAdd(&look, &look, &tmp);
        NuVecScale(&look, &look, 0.5f);
        if (Level == 0x19) {
            NuVecSub(&tmp, &look, &pos);
            NuVecNorm(&tmp, &tmp);
            NuVecScale(&tmp, &tmp, 3.0f);
            NuVecAdd(&pos, &pos, &tmp);
        }
        break;

    case 9:
        /* circle a fixed point at radius 4 */
        pos.x = 0.0f;
        pos.y = 9.3f;
        look.x = obj->pos.x;
        look.y = obj->pos.y + (obj->min.y + obj->max.y) * obj->SCALE * 0.5f;
        look.z = obj->pos.z;
        pos.z = 0.0f;
        dx = pos.x - look.x;
        dz = pos.z - look.z;
        dist = NuFsqrt(dx * dx + dz * dz) + 0.1f;
        if (dist < 1.0f) {
            ang = GameCamera->angle;
        } else {
            ang = NuAtan2D(dx, dz) & 0xFFFF;
        }
        if (GameCamera->mode == oldmode) {
            rot = TurnRot(GameCamera->angle, ang, 0x51E);
        } else {
            rot = ang;
        }
        GameCamera->angle = rot;
        f = 4.0f;
        pos.x = NuTrigTable[GameCamera->angle] * f + 0.0f;
        pos.z = NuTrigTable[(GameCamera->angle + 0x4000) & 0xFFFF] * f + 0.0f;
        dx = look.x - pos.x;
        dz = look.z - pos.z;
        dist = NuFsqrt(dx * dx + dz * dz) + 0.1f;
        if (dist != f) {
            f = -(f - dist) / dist;
            pos.x += dx * f;
            pos.z += dz * f;
        }
        yseek = 2;
        break;

    case 8:
        xrot_fixed = 0x4000;
        yrot_fixed = 0;
        zrot_fixed = 0;
        pos.x = LOGOCAMX;
        pos.y = LOGOCAMY;
        pos.z = LOGOCAMZ;
        break;

    case 0x19:
        xrot_fixed = 0;
        pos.x = 0.0f;
        pos.y = 0.5f;
        pos.z = -1.0f;
        yrot_fixed = 0;
        pos.x += WOBBLE(206) * 0.1f;
        pos.y += WOBBLE(159) * 0.1f;
        pos.z += WOBBLE(178) * 0.1f;
        break;

    case 4:
        /* free camera that keeps between CAMSTOPNEAR and CAMSTOPFAR */
        if (GameCamera->mode == oldmode) {
            dx = GameCamera->pos.x - obj->pos.x;
            dz = GameCamera->pos.z - obj->pos.z;
            f = NuFsqrt(dx * dx + dz * dz);
            ang = NuAtan2D(dx, dz) & 0xFFFF;
            if (f < CAMSTOPNEAR) {
                f = CAMSTOPNEAR - f;
                seek.x = seek.x * f * 2.0f / CAMSTOPNEAR;
                seek.z = seek.z * f * 2.0f / CAMSTOPNEAR;
            } else if (f < CAMSTOPFAR) {
                f = f - CAMSTOPNEAR;
                seek.x = seek.x * f / (CAMSTOPFAR - CAMSTOPNEAR);
                seek.z = seek.z * f / (CAMSTOPFAR - CAMSTOPNEAR);
            }
        } else {
            ang = (obj->hdg - 0x8000) & 0xFFFF;
        }
        pos.x = obj->pos.x + NuTrigTable[ang] * CAMSTOPNEAR;
        pos.y = obj->pos.y + obj->max.y * obj->SCALE;
        pos.z = obj->pos.z + NuTrigTable[(ang + 0x4000) & 0xFFFF] * CAMSTOPNEAR;
        if (GameCamera->mode == oldmode) {
            tmp.x = GameCamera->pos.x;
            tmp.y = pos.y;
            tmp.z = GameCamera->pos.z;
        } else {
            tmp = pos;
        }
        height = NewShadowMask(&tmp, 0.0f, -1);
        if (height == 2000000.0f) {
            height = pos.y;
        } else {
            height = height + obj->max.y * obj->SCALE;
        }
        if (pos.y < height) {
            pos.y = height;
        }
        break;

    case 0xB:
        /* sweep along rail 1 */
        i = 500; /* sweep period in frames */
        idx = (((GameTimer.frame % i) << 16) / i + 0x4000) & 0xFFFF;
        PointAlongSpline(Rail[1].pINCAM, NuTrigTable[idx] * 0.5f + 0.5f, &pos,
                         NULL, NULL);
        PointAlongSpline(Rail[1].pINPLAT, NuTrigTable[idx] * 0.5f + 0.5f, &look,
                         NULL, NULL);
        break;

    case 7:
        /* side-on camera, distance adjustable with the shoulder buttons */
        if (Pad[0] != NULL) {
            if ((Pad[0]->paddata & 4) && !(Pad[0]->paddata & 1)) {
                SIDECAMDISTANCE += 0.02f;
                if (SIDECAMDISTANCE > 9.0f) {
                    SIDECAMDISTANCE = 9.0f;
                }
            } else if (!(Pad[0]->paddata & 4) && (Pad[0]->paddata & 1)) {
                SIDECAMDISTANCE -= 0.02f;
                if (SIDECAMDISTANCE < 3.0f) {
                    SIDECAMDISTANCE = 3.0f;
                }
            } else if ((Pad[0]->paddata & 4) && (Pad[0]->paddata & 1)) {
                SIDECAMDISTANCE = 6.0f;
            }
        }
        xrot_fixed = 0;
        yrot_fixed = 0xC000;
        look.x = obj->pos.x;
        pos.x = look.x + SIDECAMDISTANCE;
        pos.y = look.y = obj->pos.y + (obj->min.y + obj->max.y) * obj->SCALE * 0.5f;
        pos.z = look.z = obj->pos.z;
        break;

    case 0:
    case 1:
    case 2:
    case 3:
    default:
        break;
    }

    if ((PLAYERCOUNT != 0) && (obj->finished != 0)) {
        pos = GameCamera->pos;
    }

    /* --- move toward the dist --- */
    if (GameCamera->blend_time < GameCamera->blend_duration) {
        GameCamera->blend_time += 0.02f;
        if (GameCamera->blend_time > GameCamera->blend_duration) {
            GameCamera->blend_time = GameCamera->blend_duration;
        }
    }
    if ((GameCamera->mode != oldmode) &&
        (GameCamera->blend_time >= GameCamera->blend_duration)) {
        GameCamera->pos = pos;
        GameCamera->seek = seek;
    } else {
        if (GameCamera->blend_time < GameCamera->blend_duration) {
            seek.y *= 0.25f;
        }
        if (GameCamera->seek.x > seek.x) {
            GameCamera->seek.x -= 0.0011999999f;
            if (GameCamera->seek.x < seek.x) {
                GameCamera->seek.x = seek.x;
            }
        } else if (GameCamera->seek.x < seek.x) {
            GameCamera->seek.x += 0.0011999999f;
            if (GameCamera->seek.x > seek.x) {
                GameCamera->seek.x = seek.x;
            }
        }
        if (GameCamera->seek.y > seek.y) {
            GameCamera->seek.y -= 0.0011999999f;
            if (GameCamera->seek.y < seek.y) {
                GameCamera->seek.y = seek.y;
            }
        } else if (GameCamera->seek.y < seek.y) {
            GameCamera->seek.y += 0.0011999999f;
            if (GameCamera->seek.y > seek.y) {
                GameCamera->seek.y = seek.y;
            }
        }
        if (GameCamera->seek.z > seek.z) {
            GameCamera->seek.z -= 0.0011999999f;
            if (GameCamera->seek.z < seek.z) {
                GameCamera->seek.z = seek.z;
            }
        } else if (GameCamera->seek.z < seek.z) {
            GameCamera->seek.z += 0.0011999999f;
            if (GameCamera->seek.z > seek.z) {
                GameCamera->seek.z = seek.z;
            }
        }
        if (GameCamera->blend_time < GameCamera->blend_duration) {
            f = GameCamera->blend_time / GameCamera->blend_duration;
            pos.x = GameCamera->oldpos.x + (pos.x - GameCamera->oldpos.x) * f;
            pos.y = GameCamera->oldpos.y + (pos.y - GameCamera->oldpos.y) * f;
            pos.z = GameCamera->oldpos.z + (pos.z - GameCamera->oldpos.z) * f;
        }
        GameCamera->pos.x = GameCamera->pos.x + (pos.x - GameCamera->pos.x) * GameCamera->seek.x;
        GameCamera->pos.y = GameCamera->pos.y + (pos.y - GameCamera->pos.y) * GameCamera->seek.y;
        GameCamera->pos.z = GameCamera->pos.z + (pos.z - GameCamera->pos.z) * GameCamera->seek.z;
    }
    GameCamera->newpos = pos;

    /* --- rotation --- */
    if ((GameCamera->mode == 5) || (GameCamera->mode == 9) ||
        (GameCamera->mode == 6) || (GameCamera->mode == 0x1A) ||
        (GameCamera->mode == 0x17)) {
        tilt = 4;
    } else {
        tilt = 0;
    }
    if (tilt != 0) {
        if ((Level == 0x10) || (Level == 0) || (Level == 0xB) || (Level == 9)) {
            tilt = 0;
        }
        if ((tilt != 0) && (vehicle == 0xA1)) {
            tilt = 6;
        }
    }

    dx = look.x - GameCamera->pos.x;
    dy = look.y - GameCamera->pos.y;
    dz = look.z - GameCamera->pos.z;
    if (xrot_fixed == -1) {
        xrot = -NuAtan2D(dy, NuFsqrt(dx * dx + dz * dz));
        if (tilt != 0) {
            xrot = (xrot * tilt) / 8;
        }
        xrot &= 0xFFFF;
    } else {
        xrot = xrot_fixed;
    }
    if (GameCamera->blend_time < GameCamera->blend_duration) {
        xrot = GameCamera->old_xrot +
               (s32)(RotDiff(GameCamera->old_xrot, xrot) *
                     (GameCamera->blend_time / GameCamera->blend_duration));
    }
    GameCamera->new_xrot = xrot;

    if ((tilt != 0) &&
        ((Level == 3) || ((Level == 0x1D) && (GameTimer.ftime < 3.0f)))) {
        tilt = 0;
    }
    if (yrot_fixed == -1) {
        if ((GameCamera->mode == 0x1A) && (tilt != 0)) {
            yrot = (RotDiff(0, NuAtan2D(dx, dz)) * tilt) / 8;
        } else if ((GameCamera->mode == 0x17) && (tilt != 0)) {
            /* aim between the spline centre and the dist */
            ang = NuAtan2D(dx, dz) & 0xFFFF;
            tmp = v000;
            n = SplTab[iSPL + 9].spl->len;
            for (i = 0; i < n; i++) {
                NuVecAdd(&tmp, &tmp,
                         (struct nuvec_s *)(SplTab[iSPL + 9].spl->pts +
                                            i * SplTab[iSPL + 9].spl->ptsize));
            }
            NuVecScale(&tmp, &tmp, 1.0f / n);
            yrot = NuAtan2D(tmp.x - GameCamera->pos.x, tmp.z - GameCamera->pos.z);
            yrot += (RotDiff(yrot, ang) * tilt) / 8;
        } else if ((GameCamera->mode == 5) && (tilt != 0) && (cRPosCOUNT == 1)) {
            ang = NuAtan2D(dx, dz) & 0xFFFF;
            yrot = cam_angle;
            if (yrot != -1) {
                yrot += (RotDiff(yrot, ang) * tilt) / 8;
            } else {
                yrot = ang;
            }
        } else {
            yrot = NuAtan2D(dx, dz);
        }
    } else {
        yrot = yrot_fixed;
    }
    if (GameCamera->blend_time < GameCamera->blend_duration) {
        yrot = GameCamera->old_yrot +
               (s32)(RotDiff(GameCamera->old_yrot, yrot) *
                     (GameCamera->blend_time / GameCamera->blend_duration));
    }
    GameCamera->new_yrot = yrot;
    zrot = 0;
    if (GameCamera->blend_time < GameCamera->blend_duration) {
        zrot = GameCamera->old_zrot +
               (s32)(RotDiff(GameCamera->old_zrot, 0) *
                     (GameCamera->blend_time / GameCamera->blend_duration));
    }
    GameCamera->new_zrot = zrot;

    if ((GameCamera->mode != oldmode) &&
        (GameCamera->blend_time >= GameCamera->blend_duration)) {
        GameCamera->xrot = xrot;
        GameCamera->yrot = yrot;
        GameCamera->zrot = zrot;
    } else {
        if (xrot_fixed != -1) {
            GameCamera->xrot = xrot_fixed;
        } else {
            GameCamera->xrot = SeekRot(GameCamera->xrot, xrot, xseek);
        }
        if (yrot_fixed != -1) {
            GameCamera->yrot = yrot_fixed;
        } else {
            GameCamera->yrot = SeekRot(GameCamera->yrot, yrot, yseek);
        }
        if (zrot_fixed != -1) {
            GameCamera->zrot = zrot_fixed;
        } else {
            GameCamera->zrot = SeekRot(GameCamera->zrot, zrot, 5);
        }
    }

    /* --- shake and build the matrix --- */
    xrot = GameCamera->xrot;
    yrot = GameCamera->yrot;
    zrot = GameCamera->zrot;
    if (GameCamera->judder > 0.0f) {
        GameCamera->judder -= 0.02f;
        if (GameCamera->judder > 0.0f) {
            i = (s32)((GameCamera->judder + GameCamera->judder) * 256.0f);
            if (fmod(GameCamera->judder, 0.08f) < 0.04f) {
                i = -i;
            }
            xrot += i;
        } else {
            GameCamera->judder = 0.0f;
        }
    }
    NuMtxSetIdentity(&GameCamera->m);
    NuMtxTranslate(&GameCamera->m, &GameCamera->pos);
    NuMtxPreRotateY(&GameCamera->m, yrot);
    NuMtxPreRotateX(&GameCamera->m, xrot);
    NuMtxPreRotateZ(&GameCamera->m, zrot);

Finish:
    GameCamera->vX.x = GameCamera->m._00;
    GameCamera->vX.y = GameCamera->m._01;
    GameCamera->vX.z = GameCamera->m._02;
    GameCamera->vY.x = GameCamera->m._10;
    GameCamera->vY.y = GameCamera->m._11;
    GameCamera->vY.z = GameCamera->m._12;
    GameCamera->vZ.x = GameCamera->m._20;
    GameCamera->vZ.y = GameCamera->m._21;
    GameCamera->vZ.z = GameCamera->m._22;
    pNuCam->mtx = GameCamera->m;
    NuCameraSet(pNuCam);
    if (player->used != 0) {
        GameCamera->hdg_to_player =
            NuAtan2D(player->obj.pos.x - GameCamera->pos.x,
                     player->obj.pos.z - GameCamera->pos.z);
    }
}


extern struct numtl_s *D_00633268;
#define camtarget_mtl D_00633268

extern void NuRndrLine3dDbg(u32 colour, f32 x0, f32 y0, f32 z0, f32 x1,
                            f32 y1, f32 z1);

inline void DrawMtlLine3D(struct numtl_s *mtl, f32 x0, f32 y0, f32 z0, f32 x1,
                          f32 y1, f32 z1) {
    struct nuvtx_tc1_s line[2];

    line[0].pnt.x = x0;
    line[0].pnt.y = y0;
    line[0].pnt.z = z0;
    line[1].pnt.x = x1;
    line[1].pnt.y = y1;
    line[1].pnt.z = z1;
    line[0].diffuse = -1;
    line[1].diffuse = -1;
    NuRndrLine3d(line, mtl, 0);
}


void DrawCameraTarget(struct nuvec_s *pos) {
    DrawMtlLine3D(camtarget_mtl, pos->x - 0.25f, pos->y, pos->z,
                  pos->x + 0.25f, pos->y, pos->z);
    DrawMtlLine3D(camtarget_mtl, pos->x, pos->y - 0.25f, pos->z,
                  pos->x, pos->y + 0.25f, pos->z);
    DrawMtlLine3D(camtarget_mtl, pos->x, pos->y, pos->z - 0.25f,
                  pos->x, pos->y, pos->z + 0.25f);
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


void InitCameraTargetMaterial(void) {
    struct numtl_s *mtl;

    mtl = NuMtlCreate(1);
    camtarget_mtl = mtl;
    mtl->attrib.cull = 2;
    mtl->attrib.zmode = 0;
    mtl->diffuse.r = 0.5f;
    mtl->diffuse.g = 0.5f;
    mtl->diffuse.b = 0.5f;
    mtl->alpha = 1.0f;
    mtl->attrib.alpha = 0;
    NuMtlUpdate(mtl);
}


void AddCross(struct nuvec_s *pos, u32 colour, f32 size) {
    f32 line[6];
    s32 axis;
    s32 i;

    for (axis = 0; axis < 3; axis++) {
        for (i = 0; i < 2; i++) {
            line[i * 3 + 0] = pos->x;
            line[i * 3 + 1] = pos->y;
            line[i * 3 + 2] = pos->z;
        }
        line[axis] -= size;
        line[axis + 3] += size;
        NuRndrLine3dDbg(colour, line[0], line[1], line[2], line[3], line[4],
                        line[5]);
    }
}
