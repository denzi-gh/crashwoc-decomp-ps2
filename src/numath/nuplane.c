/*
 * Unit: numath/nuplane
 *
 * Functions:
 *   0x0010be70 NuPlnPlnIntersect
 *   0x0010c2d0 NuPlnEqn
 *   0x0010c388 NuPlnEqnPn
 *   0x0010c3d0 NuPlnDist
 *   0x0010c408 NuPlnDist2
 *   0x0010c4d8 NuPlnLine
 *   0x0010c5c0 NuPlnLine2
 *   0x0010c768 NuPtInPoly
 *   0x0010c920 NuPtInPolyXY
 *   0x0010ca38 NuPtInPolyYX
 *   0x0010cb50 NuPtInPolyXZ
 *   0x0010cc68 NuPtInPolyZX
 *   0x0010cd80 NuPtInPolyYZ
 *   0x0010ce98 NuPtInPolyZY
 *   0x0010cfb0 NuLineToPointDistSqr
 *   0x0010d0d0 NuPointRelToBoundingBox
 *   0x0010d1d8 NuClipXPlane
 *   0x0010d230 NuClipYPlane
 *   0x0010d288 NuClipZPlane
 */

#include "creature.h"

struct nuplane_s {
    f32 x;
    f32 y;
    f32 z;
    f32 d;
};

f32 NuVecDot(struct nuvec_s *a, struct nuvec_s *b);
void NuVecSub(struct nuvec_s *dst, struct nuvec_s *a, struct nuvec_s *b);
void NuVecCross(struct nuvec_s *dst, struct nuvec_s *a, struct nuvec_s *b);
void NuVecNorm(struct nuvec_s *dst, struct nuvec_s *src);
void NuVecScale(struct nuvec_s *dst, struct nuvec_s *src, f32 scale);
void NuVecAdd(struct nuvec_s *dst, struct nuvec_s *a, struct nuvec_s *b);
f32 NuFsign(f32 value);
f32 NuFabs(f32 value);
f32 NuVecDistSqr(struct nuvec_s *a, struct nuvec_s *b,
                 struct nuvec_s *delta);
f32 NuVecMag(struct nuvec_s *vec);
s32 NuPtInPoly(struct nuvec_s *point, struct nuvec_s *v0,
               struct nuvec_s *v1, struct nuvec_s *v2,
               struct nuplane_s *plane);

typedef void (*nuerror_callback)(char *message);
extern char D_00614020[];
extern char D_00614048[];
extern char D_0062EA58[];
nuerror_callback NuErrorProlog(char *file, s32 line);
u64 fptodp(f32 value);
s32 dpcmp(u64 a, u64 b);
extern u64 D_00614070;
extern u64 D_00614078;
extern u64 D_00614080;
extern u64 D_00614088;

s32 NuPlnPlnIntersect(struct nuplane_s *p0, struct nuplane_s *p1,
                      struct nuvec_s *point, struct nuvec_s *direction)
{
    struct nuvec_s cross;
    u64 tolerance;
    cross.x = p0->y * p1->z - p0->z * p1->y;
    cross.y = p0->z * p1->x - p0->x * p1->z;
    cross.z = p0->x * p1->y - p0->y * p1->x;
    NuVecNorm(&cross, &cross);
    *direction = cross;

    cross.x = NuFabs(cross.x);
    cross.y = NuFabs(cross.y);
    cross.z = NuFabs(cross.z);

    if (cross.x <= cross.z && cross.y <= cross.z) {
        if (dpcmp(fptodp(NuFabs(p1->x)), D_00614070) < 0) {
            point->y = (p0->d * p1->x / p0->x - p1->d) /
                       (-p1->x * p0->y / p0->x + p1->y);
            point->x = (-p0->d - p0->y * point->y) / p0->x;
        } else {
            point->y = (p1->d * p0->x / p1->x - p0->d) /
                       (-p0->x * p1->y / p1->x + p0->y);
            point->x = (-p1->d - p1->y * point->y) / p1->x;
        }
        point->z = 0.0f;
    } else if (cross.x <= cross.y && cross.z <= cross.y) {
        if (dpcmp(fptodp(NuFabs(p1->x)), D_00614078) < 0) {
            point->z = (p0->d * p1->x / p0->x - p1->d) /
                       (-p1->x * p0->z / p0->x + p1->z);
            point->x = (-p0->d - p0->z * point->z) / p0->x;
        } else {
            point->z = (p1->d * p0->x / p1->x - p0->d) /
                       (-p0->x * p1->z / p1->x + p0->z);
            point->x = (-p1->d - p1->z * point->z) / p1->x;
        }
        point->y = 0.0f;
    } else {
        if (dpcmp(fptodp(NuFabs(p1->y)), D_00614080) < 0) {
            point->z = (p0->d * p1->y / p0->y - p1->d) /
                       (-p1->y * p0->z / p0->y + p1->z);
            point->y = (-p0->d - p0->z * point->z) / p0->y;
        } else {
            point->z = (p1->d * p0->y / p1->y - p0->d) /
                       (-p0->y * p1->z / p1->y + p0->z);
            point->y = (-p1->d - p1->z * point->z) / p1->y;
        }
        point->x = 0.0f;
    }

    tolerance = D_00614088;
    if (dpcmp(fptodp(p0->x * point->x + p0->y * point->y +
                     p0->z * point->z + p0->d), tolerance) > 0 ||
        dpcmp(fptodp(p1->x * point->x + p1->y * point->y +
                     p1->z * point->z + p1->d), tolerance) > 0) {
        NuErrorProlog(D_00614020, 0x187)(D_00614048);
    }
    return 0;
}

void NuPlnEqn(struct nuplane_s *plane, struct nuvec_s *p0,
              struct nuvec_s *p1, struct nuvec_s *p2)
{
    struct nuvec_s v0;
    struct nuvec_s v1;
    struct nuvec_s normal;

    NuVecSub(&v0, p1, p0);
    NuVecSub(&v1, p2, p0);
    NuVecCross(&normal, &v0, &v1);
    NuVecNorm((struct nuvec_s *)plane, &normal);
    plane->d = -((plane->x * p0->x + plane->y * p0->y) +
                 plane->z * p0->z);
}

void NuPlnEqnPn(struct nuplane_s *plane, struct nuvec_s *point,
                struct nuvec_s *normal)
{
    plane->x = normal->x;
    plane->y = normal->y;
    plane->z = normal->z;
    plane->d = -((plane->x * point->x + plane->y * point->y) +
                 plane->z * point->z);
}

f32 NuPlnDist(struct nuplane_s *plane, struct nuvec_s *point)
{
    return NuVecDot(point, (struct nuvec_s *)plane) + plane->d;
}

f32 NuPlnDist2(struct nuplane_s *plane, struct nuvec_s *p0,
               struct nuvec_s *p1)
{
    f32 d0 = NuVecDot(p0, (struct nuvec_s *)plane) + plane->d;
    f32 d1 = NuVecDot(p1, (struct nuvec_s *)plane) + plane->d;

    if (d0 < 0.0f && d1 < 0.0f) {
        return d1 < d0 ? d0 : d1;
    }
    if (0.0f < d0 && 0.0f < d1) {
        return d0 < d1 ? d0 : d1;
    }
    return 0.0f;
}

inline s32 NuPlnLine(struct nuplane_s *plane, struct nuvec_s *p0,
                     struct nuvec_s *p1, struct nuvec_s *intersection)
{
    f32 d0 = NuVecDot(p0, (struct nuvec_s *)plane) + plane->d;
    f32 d1 = NuVecDot(p1, (struct nuvec_s *)plane) + plane->d;
    struct nuvec_s delta;

    if (NuFsign(d0) == NuFsign(d1)) {
        return 0;
    }
    NuVecSub(&delta, p1, p0);
    NuVecScale(intersection, &delta, -d0 / (d1 - d0));
    NuVecAdd(intersection, intersection, p0);
    return 1;
}

s32 NuPointRelToBoundingBox(struct nuvec_s *point, struct nuvec_s *upper,
                            struct nuvec_s *lower)
{
    s32 result = 0;

    if (point == 0 || upper == 0 || lower == 0) {
        NuErrorProlog(D_00614020, 0x1b3)(D_0062EA58);
    }
    if (point->x >= upper->x) {
        result = 1;
    } else if (point->x <= lower->x) {
        result = 8;
    }
    if (point->y >= upper->y) {
        result |= 2;
    } else if (point->y <= lower->y) {
        result |= 0x10;
    }
    if (point->z >= upper->z) {
        result |= 4;
    } else if (point->z <= lower->z) {
        result |= 0x20;
    }
    return result;
}

#define PT_IN_POLY_BODY(A, B)                                                  \
    if (((v1->A - v0->A) * (point->B - v1->B) -                              \
         (point->A - v1->A) * (v1->B - v0->B) <= 0.0f) &&                    \
        ((v2->A - v1->A) * (point->B - v2->B) -                              \
         (point->A - v2->A) * (v2->B - v1->B) <= 0.0f) &&                    \
        ((v0->A - v2->A) * (point->B - v0->B) -                              \
         (point->A - v0->A) * (v0->B - v2->B) <= 0.0f) &&                    \
        (v0->A != v1->A || v0->B != v1->B) &&                                \
        (v0->A != v2->A || v0->B != v2->B) &&                                \
        (v1->A != v2->A || v1->B != v2->B)) {                                \
        return 1;                                                              \
    }                                                                          \
    return 0

s32 NuPtInPolyXY(struct nuvec_s *point, struct nuvec_s *v0,
                 struct nuvec_s *v1, struct nuvec_s *v2)
{
    PT_IN_POLY_BODY(x, y);
}

s32 NuPtInPolyYX(struct nuvec_s *point, struct nuvec_s *v0,
                 struct nuvec_s *v1, struct nuvec_s *v2)
{
    PT_IN_POLY_BODY(y, x);
}

s32 NuPtInPolyXZ(struct nuvec_s *point, struct nuvec_s *v0,
                 struct nuvec_s *v1, struct nuvec_s *v2)
{
    PT_IN_POLY_BODY(x, z);
}

s32 NuPtInPolyZX(struct nuvec_s *point, struct nuvec_s *v0,
                 struct nuvec_s *v1, struct nuvec_s *v2)
{
    PT_IN_POLY_BODY(z, x);
}

s32 NuPtInPolyYZ(struct nuvec_s *point, struct nuvec_s *v0,
                 struct nuvec_s *v1, struct nuvec_s *v2)
{
    PT_IN_POLY_BODY(y, z);
}

s32 NuPtInPolyZY(struct nuvec_s *point, struct nuvec_s *v0,
                 struct nuvec_s *v1, struct nuvec_s *v2)
{
    PT_IN_POLY_BODY(z, y);
}

#undef PT_IN_POLY_BODY

f32 NuLineToPointDistSqr(struct nuvec_s *p0, struct nuvec_s *p1,
                         struct nuvec_s *point)
{
    struct nuvec_s direction;
    struct nuvec_s to_point;
    struct nuvec_s closest;
    struct nuvec_s delta;
    f32 along;

    NuVecSub(&direction, p1, p0);
    NuVecNorm(&direction, &direction);
    NuVecSub(&to_point, point, p0);
    along = NuVecDot(&direction, &to_point);
    if (along <= 0.0f) {
        closest = *p0;
    } else if (1.0f <= along) {
        closest = *p1;
    } else {
        NuVecScale(&closest, &direction, along);
        NuVecAdd(&closest, &closest, p0);
    }
    return NuVecDistSqr(&closest, point, &delta);
}

s32 NuPlnLine2(struct nuplane_s *plane, struct nuvec_s *v0,
               struct nuvec_s *v1, struct nuvec_s *v2,
               struct nuvec_s *p0, struct nuvec_s *p1,
               struct nuvec_s *intersection, f32 *distance, f32 *along)
{
    struct nuvec_s hit_delta;
    struct nuvec_s line_delta;
    f32 hit_length;
    s32 result;

    result = NuPlnLine(plane, p0, p1, intersection);
    if (result != 0) {
        if (NuPtInPoly(intersection, v0, v1, v2, plane) == 0) {
            return 0;
        }
        if (distance != 0 || along != 0) {
            NuVecSub(&hit_delta, intersection, p0);
            hit_length = NuVecMag(&hit_delta);
            if (distance != 0) {
                *distance = hit_length;
            }
            if (along != 0) {
                plane = (struct nuplane_s *)&line_delta;
                NuVecSub((struct nuvec_s *)plane, p1, p0);
                *along = hit_length / NuVecMag((struct nuvec_s *)plane);
            }
        }
        return 1;
    }
    return 0;
}

s32 NuPtInPoly(struct nuvec_s *point, struct nuvec_s *v0,
               struct nuvec_s *v1, struct nuvec_s *v2,
               struct nuplane_s *plane)
{
    f32 ax = NuFabs(plane->x);
    f32 ay = NuFabs(plane->y);
    f32 az = NuFabs(plane->z);

    if (ay < ax) {
        if (az < ax) {
            if (plane->x <= 0.0f) {
                return NuPtInPolyYZ(point, v0, v1, v2);
            }
            return NuPtInPolyZY(point, v0, v1, v2);
        }
        if (plane->z <= 0.0f) {
            goto project_xy;
        }
        return NuPtInPolyYX(point, v0, v1, v2);
    }
    if (az < ay) {
        if (plane->y <= 0.0f) {
            return NuPtInPolyZX(point, v0, v1, v2);
        }
        return NuPtInPolyXZ(point, v0, v1, v2);
    }
    if (!(plane->z <= 0.0f)) {
        return NuPtInPolyYX(point, v0, v1, v2);
    }

project_xy:
    return NuPtInPolyXY(point, v0, v1, v2);
}

void NuClipXPlane(struct nuvec_s *out, struct nuvec_s *point,
                  struct nuvec_s *direction, f32 *x)
{
    f32 delta;

    out->x = x == 0 ? 0.0f : *x;
    delta = out->x - point->x;
    out->y = point->y + delta * direction->y / direction->x;
    out->z = point->z + delta * direction->z / direction->x;
}

void NuClipYPlane(struct nuvec_s *out, struct nuvec_s *point,
                  struct nuvec_s *direction, f32 *y)
{
    f32 delta;

    out->y = y == 0 ? 0.0f : *y;
    delta = out->y - point->y;
    out->x = point->x + delta * direction->x / direction->y;
    out->z = point->z + delta * direction->z / direction->y;
}

void NuClipZPlane(struct nuvec_s *out, struct nuvec_s *point,
                  struct nuvec_s *direction, f32 *z)
{
    f32 delta;

    out->z = z == 0 ? 0.0f : *z;
    delta = out->z - point->z;
    out->x = point->x + delta * direction->x / direction->z;
    out->y = point->y + delta * direction->y / direction->z;
}
