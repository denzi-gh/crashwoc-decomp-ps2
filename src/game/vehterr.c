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

#include "creature.h"

extern int NuAtan2D(float a, float b);
extern void NuVecRotateX(struct nuvec_s *dst, struct nuvec_s *src, int angle);

extern void *memset(void *s, s32 c, s32 n);

extern struct nuvec_s CrossProduct(struct nuvec_s *a, struct nuvec_s *b);
extern f32 DotProduct(struct nuvec_s *a, struct nuvec_s *b);
extern void NuVecScale(f32 scale, struct nuvec_s *dest, struct nuvec_s *src);
extern void NuVecSub(struct nuvec_s *dest, struct nuvec_s *a, struct nuvec_s *b);
extern f32 NuFsqrt(f32 x);

extern s32 NewRayCastSetHandel(struct nuvec_s *vpos, struct nuvec_s *vvel, f32 size,
                               f32 timeadj, f32 impactadj, s16 *handel);
extern s32 TryUnembeddPointDir(struct nuvec_s *pos, struct nuvec_s *dir1,
                               struct nuvec_s *dir2, s16 *handle, f32 radius);

extern struct nuvec_s ShadNorm;         /* 0x002ea9d0 */
extern struct nuvec_s WorldAxis[3];     /* 0x005b93e8 */

#define D_0061FA58 (*(const double *)0x0061FA58)

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
            NuVecScale(0.005f, &refl, &snorm);
            NuVecSub(&rel, &rel, &temp);
            if (wall != 0 && flags == 0) {
                retval = 1;
                NuVecScale(0.1f, &rel, &rel);
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

void FindSurfaceRotXZFromNormal(struct nuvec_s *normal, s16 *rotX, s16 *rotZ) {
    struct nuvec_s temp;
    u16 rx;
    int rz;

    rx = NuAtan2D(normal->z, normal->y);
    NuVecRotateX(&temp, normal, -rx);
    rz = NuAtan2D(temp.x, temp.y);
    *rotX = rx;
    *rotZ = -rz;
}
