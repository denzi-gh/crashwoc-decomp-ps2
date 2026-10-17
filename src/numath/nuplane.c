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

#include "nucore/nuerror.h"
#include "numath/nufloat.h"
#include "numath/numathtypes.h"
#include "numath/nuplane.h"
#include "numath/nuvec.h"

extern char D_00614020[];
extern char D_00614048[];
extern char D_0062EA58[];

s32 NuPlnPlnIntersect(struct nuplane_s *p0, struct nuplane_s *p1,
                      struct nuvec_s *point, struct nuvec_s *direction)
{
    struct nuvec_s axis;

    axis.y = p0->c * p1->a - p0->a * p1->c;
    axis.z = p0->a * p1->b - p0->b * p1->a;
    axis.x = p0->b * p1->c - p0->c * p1->b;
    NuVecNorm(&axis, &axis);
    *direction = axis;
    axis.x = NuFabs(axis.x);
    axis.y = NuFabs(axis.y);
    axis.z = NuFabs(axis.z);

    if (axis.z >= axis.x && axis.z >= axis.y) {
        if (NuFabs(p1->a) < 0.0001) {
            point->y = (p0->d * p1->a / p0->a - p1->d) /
                       (-p1->a * p0->b / p0->a + p1->b);
            point->x = (-p0->d - p0->b * point->y) / p0->a;
            point->z = 0.0f;
        } else {
            point->y = (p1->d * p0->a / p1->a - p0->d) /
                       (-p0->a * p1->b / p1->a + p0->b);
            point->x = (-p1->d - p1->b * point->y) / p1->a;
            point->z = 0.0f;
        }
    } else if (axis.y >= axis.x && axis.y >= axis.z) {
        if (NuFabs(p1->a) < 0.0001) {
            point->z = (p0->d * p1->a / p0->a - p1->d) /
                       (-p1->a * p0->c / p0->a + p1->c);
            point->x = (-p0->d - p0->c * point->z) / p0->a;
            point->y = 0.0f;
        } else {
            point->z = (p1->d * p0->a / p1->a - p0->d) /
                       (-p0->a * p1->c / p1->a + p0->c);
            point->x = (-p1->d - p1->c * point->z) / p1->a;
            point->y = 0.0f;
        }
    } else {
        if (NuFabs(p1->b) < 0.0001) {
            point->z = (p0->d * p1->b / p0->b - p1->d) /
                       (-p1->b * p0->c / p0->b + p1->c);
            point->y = (-p0->d - p0->c * point->z) / p0->b;
            point->x = 0.0f;
        } else {
            point->z = (p1->d * p0->b / p1->b - p0->d) /
                       (-p0->b * p1->c / p1->b + p0->c);
            point->y = (-p1->d - p1->c * point->z) / p1->b;
            point->x = 0.0f;
        }
    }

    if (p0->a * point->x + p0->b * point->y + p0->c * point->z + p0->d > 0.0001 ||
        p1->a * point->x + p1->b * point->y + p1->c * point->z + p1->d > 0.0001) {
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
    plane->d = -((plane->a * p0->x + plane->b * p0->y) +
                 plane->c * p0->z);
}

void NuPlnEqnPn(struct nuplane_s *plane, struct nuvec_s *point,
                struct nuvec_s *normal)
{
    plane->a = normal->x;
    plane->b = normal->y;
    plane->c = normal->z;
    plane->d = -((plane->a * point->x + plane->b * point->y) +
                 plane->c * point->z);
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
    f32 ax = NuFabs(plane->a);
    f32 ay = NuFabs(plane->b);
    f32 az = NuFabs(plane->c);
    s32 inside;

    if (ax > ay) {
        if (ax > az) {
            if (plane->a <= 0.0f) {
                inside = NuPtInPolyYZ(point, v0, v1, v2);
            } else {
                inside = NuPtInPolyZY(point, v0, v1, v2);
            }
        } else {
            if (plane->c <= 0.0f) {
                inside = NuPtInPolyXY(point, v0, v1, v2);
            } else {
                inside = NuPtInPolyYX(point, v0, v1, v2);
            }
        }
    } else {
        if (ay > az) {
            if (plane->b <= 0.0f) {
                inside = NuPtInPolyZX(point, v0, v1, v2);
            } else {
                inside = NuPtInPolyXZ(point, v0, v1, v2);
            }
        } else {
            if (plane->c <= 0.0f) {
                inside = NuPtInPolyXY(point, v0, v1, v2);
            } else {
                inside = NuPtInPolyYX(point, v0, v1, v2);
            }
        }
    }
    return inside;
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
