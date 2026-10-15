/*
 * Unit: game/vehterr
 *
 * Functions:
 *   0x00221b60 TryUnembeddPointDir
 *   0x00221de0 BestGuessActualsJeep
 *   0x00222198 MyCast
 *   0x00222918 FindSurfaceNormalAndUnembedd
 *   0x00222f80 TerrainVehicleSoft
 *   0x00223608 TryUnembeddPointDirSimple
 *   0x002237a8 TryUnembeddPointSafe
 *   0x00223948 FindVehicleNormalGiven4Points
 *   0x00223a70 FindSurfaceRotXZFromNormal
 */

#include "nu_libc.h"

#include "numath/nutrig.h"
#include "numath/nuvec.h"
#include "gamelib/terrain.h"

#include "creature.h"

struct VEHICLE {
    struct nuvec_s ActualWheelPosition[4];
    struct nuvec_s OldWheelPosition[4];
    s32 BigSpin[4];
    struct nuvec_s ActualPosition;
    struct nuvec_s Resolved;
    struct nuvec_s Velocity;
    struct nuvec_s WheelAxis[3];
    f32 FrontWheelSpeedAdj;
    short aTargetAngle;
    short aTarSurfRotX;
    short aTarSurfRotZ;
    short aActualAngle;
    short aActSurfRotX;
    short aActSurfRotZ;
    short ActFrontRotX;
    short ActRearRotX;
    short TarFrontRotX;
    short TarRearRotX;
    s32 AnyOnGroundBits;
    s32 AllOnGroundBits;
    s32 AllTouchingGroundBits;
    s32 AnyTouchingGroundBits;
    struct nuvec_s AirNormal;
    struct nuvec_s SurfaceNormal;
    s16 *TerrHandle;
    s32 FrontWheelGroundBits;
};

extern struct nuvec_s CrossProduct(struct nuvec_s *a, struct nuvec_s *b);
extern f32 DotProduct(struct nuvec_s *a, struct nuvec_s *b);
extern f32 NuFsqrt(f32 x);
extern struct nuvec_s GenerateJeepWheelPoint(s32 WheelId);
extern void NewGenerateJeepMatrix(struct numtx_s *Mat, short YAng, short SurfaceX, short SurfaceZ,
                                  short TiltX, short TiltZ, struct nuvec_s *Pos);
extern void SeekHalfLife(f32 *dest, f32 target, f32 halflife, f32 dt);
extern void SeekHalfLifeNUVEC(struct nuvec_s *src, struct nuvec_s *target, f32 halflife, f32 dt);
extern f32 frand(void);

extern struct nuvec_s NullNuVec;
extern struct nuvec_s ShadNorm;         /* 0x002ea9d0 */
extern struct nuvec_s WorldAxis[3];     /* 0x005b93e8 */
extern short JeepBaseAng;
extern short D_0063193C;
extern s32 D_00631940;
extern s32 Level;
extern struct nuvec_s D_005B9410;
extern struct nuvec_s D_005B9420;
extern struct nuvec_s D_005B9430;

#define LastJeepBaseAng D_0063193C
#define JeepAngleLocked D_00631940
#define JeepAxisX D_005B9410
#define JeepAxisY D_005B9420
#define JeepAxisZ D_005B9430

#define D_0061FA58 (*(const double *)0x0061FA58)

s32 TryUnembeddPointDir(struct nuvec_s *pos, struct nuvec_s *dir1,
                        struct nuvec_s *dir2, s16 *handle, f32 radius)
{
    struct nuvec_s test;
    struct nuvec_s avg;
    struct nuvec_s *dir;
    s32 pass;
    s32 i;
    s32 steps;
    f32 base;
    f32 step;
    f32 dist;
    f32 best;
    f32 last;

    NuVecAdd(&avg, dir1, dir2);
    base = 0.0f;
    NuVecScale(&avg, &avg, 0.5f);
    best = 0.0f;
    last = 0.0f;
    step = 0.03f;
    steps = 5;
    pass = 0;
    while (pass++ < 5) {
        for (i = 1; i <= steps; i++) {
            dist = base + step * i;
            test = *pos;
            NuVecScaleAccum(&test, dir1, dist);
            if (NewRayCastSetHandel(&test, &NullNuVec, radius, 0.01f, 0.0f, handle) & 0x10) {
                test = *pos;
                NuVecScaleAccum(&test, dir2, dist);
                if (NewRayCastSetHandel(&test, &NullNuVec, radius, 0.01f, 0.0f, handle) & 0x10) {
                    test = *pos;
                    NuVecScaleAccum(&test, &avg, dist);
                    if (NewRayCastSetHandel(&test, &NullNuVec, radius, 0.01f, 0.0f, handle) & 0x10) {
                        last = dist;
                    } else {
                        best = dist;
                        dir = &avg;
                        goto next;
                    }
                } else {
                    best = dist;
                    dir = dir2;
                    goto next;
                }
            } else {
                best = dist;
                dir = dir1;
                goto next;
            }
        }
    next:
        base = last;
        step *= 0.5f;
        steps = 2;
    }
    if (best != 0.0f) {
        NuVecScaleAccum(pos, dir, best);
        return 1;
    }
    return 0;
}


inline s32 TryUnembeddPointDirSimple(struct nuvec_s *pos, struct nuvec_s *dir, s16 *handle,
                              s32 passes, f32 radius, f32 step)
{
    struct nuvec_s test;
    s32 pass;
    s32 i;
    s32 steps;
    f32 base;
    f32 dist;
    f32 best;
    f32 last;

    base = 0.0f;
    steps = 5;
    best = 0.0f;
    last = 0.0f;
    pass = 0;
    while (pass++ < passes) {
        for (i = 1; i <= steps; i++) {
            dist = base + step * i;
            test = *pos;
            NuVecScaleAccum(&test, dir, dist);
            if (NewRayCastSetHandel(&test, &NullNuVec, radius, 0.01f, 0.0f, handle) != 0) {
                last = dist;
            } else {
                best = dist;
                break;
            }
        }
        base = last;
        steps = 2;
        step *= 0.5f;
    }
    if (best != 0.0f) {
        NuVecScaleAccum(pos, dir, best);
        return 1;
    }
    return 0;
}


inline s32 TryUnembeddPointSafe(struct nuvec_s *pos, struct nuvec_s *target, s16 *handle, f32 radius)
{
    struct nuvec_s diff;
    struct nuvec_s test;
    s32 count;
    s32 total;
    s32 i;
    s32 found;
    f32 t;

    found = 0;
    diff.x = pos->x - target->x;
    diff.y = pos->y - target->y;
    diff.z = pos->z - target->z;
    count = NuVecMag(&diff) * 10.0f;
    total = count + 1;
    for (i = count; i >= 0; i--) {
        if (i != 0) {
            t = (f32)i / (f32)total;
            test.x = target->x + diff.x * t;
            test.y = target->y + diff.y * t;
            test.z = target->z + diff.z * t;
        } else {
            test = *target;
        }
        found = (NewRayCastSetHandel(&test, &NullNuVec, radius, 0.01f, 0.0f, handle) & 0x10) == 0;
        if (found) {
            *pos = test;
            break;
        }
    }
    return found;
}


inline void FindVehicleNormalGiven4Points(struct nuvec_s *normal, struct nuvec_s *points)
{
    struct nuvec_s front;
    struct nuvec_s rear;
    struct nuvec_s dir;
    struct nuvec_s side;

    NuVecAdd(&front, &points[0], &points[1]);
    NuVecScale(&front, &front, 0.5f);
    NuVecAdd(&rear, &points[2], &points[3]);
    NuVecScale(&rear, &rear, 0.5f);
    NuVecSub(&dir, &rear, &front);
    side.x = (points[1].x - points[0].x + points[3].x - points[2].x) * 0.5f;
    side.y = (points[1].y - points[0].y + points[3].y - points[2].y) * 0.5f;
    side.z = (points[1].z - points[0].z + points[3].z - points[2].z) * 0.5f;
    NuVecCross(normal, &side, &dir);
    NuVecNorm(normal, normal);
}


inline void FindSurfaceRotXZFromNormal(struct nuvec_s *normal, s16 *rotX, s16 *rotZ) {
    struct nuvec_s temp;
    u16 rx;
    int rz;

    rx = NuAtan2D(normal->z, normal->y);
    NuVecRotateX(&temp, normal, -rx);
    rz = NuAtan2D(temp.x, temp.y);
    *rotX = rx;
    *rotZ = -rz;
}


void BestGuessActualsJeep(struct VEHICLE *Vehicle)
{
    struct nuvec_s WheelBuf[4];
    struct nuvec_s *Wheel;
    struct nuvec_s Front;
    struct nuvec_s Rear;
    struct nuvec_s Dir;
    struct nuvec_s Normal;
    struct nuvec_s Side;
    struct nuvec_s Rotated;
    struct nuvec_s Offset;
    struct nuvec_s WheelPoint;
    s32 i;
    u16 rotX;
    s32 rotZ;
    s32 base;
    short delta;
    short turn;
    f32 len;

    Wheel = WheelBuf;
    for (i = 0; i < 4; i++) {
        Wheel[i] = Vehicle->ActualWheelPosition[i];
    }
    NuVecAdd(&Front, &Wheel[0], &Wheel[1]);
    NuVecScale(&Front, &Front, 0.5f);
    NuVecAdd(&Rear, &Wheel[2], &Wheel[3]);
    NuVecScale(&Rear, &Rear, 0.5f);
    NuVecSub(&Dir, &Rear, &Front);
    len = NuFsqrt(NuVecMagSqr(&Dir));
    Side.x = (Wheel[1].x - Wheel[0].x + Wheel[3].x - Wheel[2].x) * 0.5f;
    Side.y = (Wheel[1].y - Wheel[0].y + Wheel[3].y - Wheel[2].y) * 0.5f;
    Side.z = (Wheel[1].z - Wheel[0].z + Wheel[3].z - Wheel[2].z) * 0.5f;
    NuVecCross(&Normal, &Side, &Dir);
    NuVecNorm(&Normal, &Normal);

    rotX = NuAtan2D(Normal.z, Normal.y);
    NuVecRotateX(&Rotated, &Normal, -rotX);
    rotZ = NuAtan2D(Rotated.x, Rotated.y);
    Vehicle->aActSurfRotX = rotX;
    Vehicle->aActSurfRotZ = -rotZ;

    if (Vehicle->aActSurfRotX < -0x1555) {
        Vehicle->aActSurfRotX = -0x1555;
    } else if (Vehicle->aActSurfRotX > 0x1555) {
        Vehicle->aActSurfRotX = 0x1555;
    }

    NuVecRotateX(&Normal, &Dir, -Vehicle->aActSurfRotX);
    NuVecRotateZ(&Normal, &Normal, -Vehicle->aActSurfRotZ);
    Vehicle->aActualAngle = NuAtan2D(-Normal.x, -Normal.z);
    base = JeepBaseAng;
    delta = Vehicle->aActualAngle - base;

    turn = JeepBaseAng - LastJeepBaseAng;
    if (turn < -0xAAB || turn > 0xAAB) {
        JeepAngleLocked = 0;
    }
    if (JeepAngleLocked != 0 && (Level != 3 || Vehicle->ActualPosition.z < 751.5f)) {
        if (delta > 0x2000) {
            Vehicle->aActualAngle = base + 0x2000;
        } else if (delta < -0x2000) {
            Vehicle->aActualAngle = base - 0x2000;
        }
    } else if (delta > -0x2000 && delta < 0x2000) {
        JeepAngleLocked = 1;
    }

    Side.x = (Front.x + Rear.x) * 0.5f;
    Side.y = (Front.y + Rear.y) * 0.5f;
    Side.z = (Front.z + Rear.z) * 0.5f;
    LastJeepBaseAng = JeepBaseAng;
    WheelPoint = GenerateJeepWheelPoint(5);
    NuVecRotateY(&Offset, &WheelPoint, Vehicle->aActualAngle);
    NuVecRotateZ(&Offset, &Offset, Vehicle->aActSurfRotZ);
    NuVecRotateX(&Offset, &Offset, Vehicle->aActSurfRotX);
    Vehicle->ActualPosition.x = Side.x - Offset.x;
    Vehicle->ActualPosition.y = Side.y - Offset.y;
    Vehicle->ActualPosition.z = Side.z - Offset.z;
}


s32 MyCast(struct nuvec_s *pos, struct nuvec_s *target, struct nuvec_s *normals,
           f32 radius, s32 count, s32 flags, s32 index, s16 *handle)
{
    struct nuvec_s rel;
    struct nuvec_s temp;
    struct nuvec_s snorm;
    struct nuvec_s refl;
    struct nuvec_s lastnorm;
    struct nuvec_s cross;
    struct nuvec_s cr;
    s32 i;
    s32 retval;
    s32 res;
    s32 wall;
    s32 flip;
    s32 down;
    double dd;
    f32 d;
    f32 magcross;
    f32 magrel;
    f32 dp;
    f32 dot;
    f32 t;

    memset(&lastnorm, 0, 0xC);
    retval = 0;

    rel.x = target->x - pos->x;
    rel.y = target->y - pos->y;
    rel.z = target->z - pos->z;

    for (i = 0; i < count; ) {
        temp = rel;
        res = NewRayCastSetHandel(pos, &temp, radius, 0.01f, 0.0f, handle);
        if (res & 0x10) {
            TryUnembeddPointDir(pos, &ShadNorm, &WorldAxis[1], handle, 0.23669f);
            temp = rel;
            res = NewRayCastSetHandel(pos, &temp, radius, 0.01f, 0.0f, handle);
        }

        pos->x = pos->x + temp.x;
        pos->y = pos->y + temp.y;
        pos->z = pos->z + temp.z;

        if (res != 0) {
        snorm = ShadNorm;

        flip = snorm.x * lastnorm.x + snorm.y * lastnorm.y + snorm.z * lastnorm.z < 0.0f;
        wall = snorm.x * normals[1].x + snorm.y * normals[1].y + snorm.z * normals[1].z < 0.5f;

        if (wall == 0) {
            dd = snorm.x * normals[0].x + snorm.y * normals[0].y +
                 snorm.z * normals[0].z;
            if (dd < 0.0) {
                dd = -dd;
            }
            wall = dd > D_0061FA58;
        }

        if (wall != 0) {
            cross = CrossProduct(&normals[1], &snorm);
            cross = CrossProduct(&cross, &snorm);
            dot = DotProduct(&cross, &rel);
            if (DotProduct(&cross, &normals[1]) * dot <= 0.0f) {
                wall = 0;
            }
        }

        down = DotProduct(&snorm, &WorldAxis[1]) < -0.5f;
        wall = wall | down;

        if (wall != 0 && flip == 0) {
            lastnorm = normals[1];
        }

        if ((flip | wall | down) == 0 || (wall != 0 && flags == 0)) {
            NuVecScale(&refl, &snorm, 0.005f);
            NuVecSub(&rel, &rel, &temp);
            if (wall != 0 && flags == 0) {
                retval = 1;
                NuVecScale(&rel, &rel, 0.1f);
            }
            d = rel.x * snorm.x + rel.y * snorm.y + rel.z * snorm.z - 0.005f;
            rel.x = rel.x - snorm.x * d;
            rel.y = rel.y - snorm.y * d;
            rel.z = rel.z - snorm.z * d;
        } else {
            cross.x = snorm.y * lastnorm.z - snorm.z * lastnorm.y;
            cross.y = snorm.z * lastnorm.x - snorm.x * lastnorm.z;
            cross.z = snorm.x * lastnorm.y - snorm.y * lastnorm.x;

            magcross = NuFsqrt(cross.x * cross.x + cross.y * cross.y + cross.z * cross.z);
            magrel = NuFsqrt(rel.x * rel.x + rel.y * rel.y + rel.z * rel.z);
            dp = (cross.x * rel.x + cross.y * rel.y + cross.z * rel.z) / (magcross * magrel);

            if (flags != 0) {
                if (dp >= 0.5f) {
                    t = 1.0f;
                } else if (dp <= -0.5f) {
                    t = -1.0f;
                } else {
                    t = dp;
                }
            } else {
                t = dp;
            }

            t = t * magrel / magcross;
            rel.x = cross.x * t;
            rel.y = cross.y * t;
            rel.z = cross.z * t;

            if (flip != 0) {
                refl.x = (snorm.x + lastnorm.x) * 0.005f;
                refl.y = (snorm.y + lastnorm.y) * 0.005f;
                refl.z = (snorm.z + lastnorm.z) * 0.005f;
                rel.x = rel.x + refl.x;
                rel.y = rel.y + refl.y;
                rel.z = rel.z + refl.z;
            } else {
                cr = CrossProduct(&cross, &normals[1]);
                if (DotProduct(&cr, &snorm) > 0.0f) {
                    t = 1.0f / magcross;
                } else {
                    t = -1.0f / magcross;
                }
                cr.x = (cr.x * t - (normals[1].x + normals[1].x)) * 0.005f;
                cr.y = (cr.y * t - (normals[1].y + normals[1].y)) * 0.005f;
                cr.z = (cr.z * t - (normals[1].z + normals[1].z)) * 0.005f;
                refl = cr;
                rel.x = rel.x + cr.x;
                rel.y = rel.y + cr.y;
                rel.z = rel.z + cr.z;
            }
        }

        NewRayCastSetHandel(pos, &refl, radius, 0.01f, 0.0f, handle);
        i++;

        pos->x = pos->x + refl.x;
        pos->y = pos->y + refl.y;
        pos->z = pos->z + refl.z;

        lastnorm = snorm;
        } else {
            i = count;
        }
    }

    return retval;
}

void FindSurfaceNormalAndUnembedd(struct VEHICLE *Vehicle, f32 dt)
{
    struct nuvec_s axis;
    struct nuvec_s ray[4];
    struct nuvec_s point[4];
    struct nuvec_s down;
    s32 hit[4];
    s32 i;
    s32 tries;

    axis = Vehicle->WheelAxis[1];
    NuVecScale(&down, &axis, -0.1f);
    for (i = 0; i < 4; i++) {
        tries = 1;
        do {
            ray[i] = down;
            hit[i] = NewRayCastSetHandel(&Vehicle->ActualWheelPosition[i], &ray[i], 0.23669f, 0.01f,
                                         0.0f, Vehicle->TerrHandle);
            if (hit[i] & 0x10) {
                if (!TryUnembeddPointDir(&Vehicle->ActualWheelPosition[i], &ShadNorm, &WorldAxis[1],
                                         Vehicle->TerrHandle, 0.23669f)) {
                    TryUnembeddPointSafe(&Vehicle->ActualWheelPosition[i],
                                         &Vehicle->OldWheelPosition[i], Vehicle->TerrHandle, 0.23669f);
                }
            } else {
                tries = 0;
            }
            if (hit[i] != 0 || i < 2 || !(Vehicle->FrontWheelGroundBits & 1)) {
                NuVecAdd(&point[i], &Vehicle->ActualWheelPosition[i], &ray[i]);
            } else {
                point[i] = Vehicle->ActualWheelPosition[i];
            }
        } while (tries-- > 0);
    }

    Vehicle->FrontWheelGroundBits <<= 1;
    if (hit[0] != 0 || hit[1] != 0) {
        Vehicle->FrontWheelGroundBits |= 1;
    }

    Vehicle->AnyOnGroundBits <<= 1;
    Vehicle->AllOnGroundBits = (Vehicle->AllOnGroundBits << 1) | 1;
    for (i = 0; i < 4; i++) {
        if (hit[i] != 0) {
            Vehicle->AnyOnGroundBits |= 1;
        } else {
            Vehicle->AllOnGroundBits &= ~1u;
        }
    }

    Vehicle->AnyTouchingGroundBits <<= 1;
    Vehicle->AllTouchingGroundBits = (Vehicle->AllTouchingGroundBits << 1) | 1;
    for (i = 0; i < 4; i++) {
        if (hit[i] != 0 && (NuVecMag(&ray[i]) <= 0.01f || (hit[i] & 0x10))) {
            Vehicle->AnyTouchingGroundBits |= 1;
        } else {
            Vehicle->AllTouchingGroundBits &= ~1u;
        }
    }

    if (Vehicle->AnyOnGroundBits & 1) {
        FindVehicleNormalGiven4Points(&Vehicle->SurfaceNormal, point);
    } else {
        if (Vehicle->AnyOnGroundBits & 2) {
            Vehicle->AirNormal = WorldAxis[1];
            NuVecScaleAccum(&Vehicle->AirNormal, &WorldAxis[0], (frand() - 0.5f) * 1.2f);
            NuVecScaleAccum(&Vehicle->AirNormal, &WorldAxis[2], frand() - 0.5f - 0.25f);
            NuVecRotateY(&axis, &axis, Vehicle->aActualAngle);
            NuVecNorm(&Vehicle->AirNormal, &Vehicle->AirNormal);
        }
        SeekHalfLifeNUVEC(&Vehicle->SurfaceNormal, &Vehicle->AirNormal, 0.3f, dt);
    }
    FindSurfaceRotXZFromNormal(&Vehicle->SurfaceNormal, &Vehicle->aTarSurfRotX,
                               &Vehicle->aTarSurfRotZ);
}


struct nuvec_s TerrainVehicleSoft(struct VEHICLE *Vehicle, struct nuvec_s *Vel)
{
    struct numtx_s TargetMtx;
    struct numtx_s OldMtx;
    struct nuvec_s Wheel[4];
    struct nuvec_s Move;
    struct nuvec_s OldPos;
    struct nuvec_s OldOffset;
    struct nuvec_s Centre;
    struct nuvec_s Offset;
    struct nuvec_s Min;
    struct nuvec_s Unused;
    struct nuvec_s Max;
    struct nuvec_s Size;
    struct numtx_s NewMtx;
    struct nuvec_s NewOffset;
    struct nuvec_s Shift;
    struct nuvec_s *p;
    s32 oldAngle;
    s32 i;
    s32 j;
    s32 pass;
    short dTarget;
    short dActual;
    f32 maxMove;
    f32 moveMag;
    f32 rate;
    f32 radius;

    oldAngle = Vehicle->aActualAngle;
    OldPos = Vehicle->ActualPosition;
    for (i = 0; i < 4; i++) {
        Vehicle->OldWheelPosition[i] = Vehicle->ActualWheelPosition[i];
    }
    NewGenerateJeepMatrix(&TargetMtx, Vehicle->aTargetAngle, Vehicle->aTarSurfRotX,
                          Vehicle->aTarSurfRotZ, 0, 0, 0);
    NewGenerateJeepMatrix(&OldMtx, Vehicle->aActualAngle, Vehicle->aTarSurfRotX,
                          Vehicle->aTarSurfRotZ, 0, 0, 0);
    Centre = GenerateJeepWheelPoint(4);
    for (i = 0; i < 4; i++) {
        Offset = GenerateJeepWheelPoint(i);
        Wheel[i] = Offset;
        NuVecMtxRotate(&Wheel[i], &Wheel[i], &TargetMtx);
        Wheel[i].x += Vehicle->ActualPosition.x + Vel->x;
        Wheel[i].y += Vehicle->ActualPosition.y + Vel->y;
        Wheel[i].z += Vehicle->ActualPosition.z + Vel->z;
    }
    NuVecMtxRotate(&OldOffset, &Centre, &OldMtx);
    NuVecMtxRotate(&Offset, &Centre, &TargetMtx);
    Min.x = OldOffset.x - Offset.x;
    Min.y = OldOffset.y - Offset.y;
    Min.z = OldOffset.z - Offset.z;
    for (i = 0; i < 4; i++) {
        Wheel[i].x += Min.x;
        Wheel[i].y += Min.y;
        Wheel[i].z += Min.z;
    }
    NuVecMtxRotate(&Vehicle->WheelAxis[0], &JeepAxisX, &TargetMtx);
    NuVecMtxRotate(&Vehicle->WheelAxis[1], &JeepAxisY, &TargetMtx);
    NuVecMtxRotate(&Vehicle->WheelAxis[2], &JeepAxisZ, &TargetMtx);

    radius = 0.23669f;
    Min = Vehicle->ActualWheelPosition[0];
    Max = Min;
    p = &Vehicle->ActualWheelPosition[1];
    j = 1;
    for (pass = 0; pass < 2; pass++) {
        for (; j < 4; j++, p++) {
            if (p->x < Min.x) {
                Min.x = p->x;
            } else if (p->x > Max.x) {
                Max.x = p->x;
            }
            if (p->y < Min.y) {
                Min.y = p->y;
            } else if (p->y > Max.y) {
                Max.y = p->y;
            }
            if (p->z < Min.z) {
                Min.z = p->z;
            } else if (p->z > Max.z) {
                Max.z = p->z;
            }
        }
        j = 0;
        p = Wheel;
    }
    NuVecSub(&Size, &Max, &Min);
    NewScanInit();
    Vehicle->TerrHandle = NewScanHandel(&Min, &Size, 0, 0, radius + 0.5f);
    for (i = 0; i < 4; i++) {
        Vehicle->BigSpin[i] = MyCast(&Vehicle->ActualWheelPosition[i], &Wheel[i], Vehicle->WheelAxis,
                                     radius, 4, i < 2, i, Vehicle->TerrHandle);
    }
    BestGuessActualsJeep(Vehicle);
    Move.x = Vehicle->ActualPosition.x - OldPos.x;
    Move.y = Vehicle->ActualPosition.y - OldPos.y;
    Move.z = Vehicle->ActualPosition.z - OldPos.z;
    NewGenerateJeepMatrix(&NewMtx, Vehicle->aActualAngle, Vehicle->aActSurfRotX,
                          Vehicle->aActSurfRotZ, 0, 0, 0);
    NuVecMtxRotate(&OldOffset, &Centre, &OldMtx);
    NuVecMtxRotate(&NewOffset, &Centre, &NewMtx);
    Shift.x = OldOffset.x - NewOffset.x;
    Shift.y = OldOffset.y - NewOffset.y;
    Shift.z = OldOffset.z - NewOffset.z;
    Move.x -= Shift.x;
    Move.y -= Shift.y;
    Move.z -= Shift.z;
    do {
    } while (0);
    maxMove = NuVecMag(Vel);
    moveMag = NuVecMag(&Move);
    if (maxMove < moveMag) {
        NuVecScale(&Move, &Move, maxMove / moveMag);
    }
    dTarget = Vehicle->aTargetAngle - oldAngle;
    dActual = Vehicle->aActualAngle - oldAngle;
    rate = 0.0f;
    if (Vehicle->Resolved.z < 2.0f) {
        if (dTarget > 0x16C && dActual < 0xB6) {
            rate = (dTarget - dActual) * 10922.667f;
        } else if (dTarget < -0x16C && dActual > -0xB6) {
            rate = (dTarget - dActual) * 10922.667f;
        }
    }
    if (rate == 0.0f) {
        SeekHalfLife(&Vehicle->FrontWheelSpeedAdj, rate, 0.2f, 0.02f);
    } else {
        SeekHalfLife(&Vehicle->FrontWheelSpeedAdj, rate, 0.1f, 0.02f);
    }
    return Move;
}
