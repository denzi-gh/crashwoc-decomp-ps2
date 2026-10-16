/*
 * Unit: numath/nuvec4
 *
 * Functions:
 *   0x00107718 NuVec4Add
 *   0x00107760 NuVec4Dot
 *   0x001077a0 NuVec4Sub
 *   0x001077e8 NuVec4Mag
 *   0x00107840 NuVec4MagSqr
 *   0x00107870 NuVec4Scale
 *   0x001078a8 NuVec4ScaleAccum
 *   0x00107900 NuVec4MtxTransform
 *   0x001079c0 NuVec4MtxTransformH
 *   0x00107a98 NuVec4Lerp
 */

#include "nu_libc.h"

#include "numath/numathtypes.h"
#include "numath/nuvec4.h"

const struct nuvec4_s nuvec4_zero = { 0.0f, 0.0f, 0.0f, 0.0f };
const struct nuvec4_s nuvec4_one = { 1.0f, 1.0f, 1.0f, 1.0f };
const struct nuvec4_s nuvec4_x = { 1.0f, 0.0f, 0.0f, 0.0f };
const struct nuvec4_s nuvec4_y = { 0.0f, 1.0f, 0.0f, 0.0f };
const struct nuvec4_s nuvec4_z = { 0.0f, 0.0f, 1.0f, 0.0f };
const struct nuvec4_s nuvec4_w = { 0.0f, 0.0f, 0.0f, 1.0f };

void NuVec4Add(struct nuvec4_s *dst, struct nuvec4_s *a,
               struct nuvec4_s *b)
{
    dst->w = a->w + b->w;
    dst->x = a->x + b->x;
    dst->y = a->y + b->y;
    dst->z = a->z + b->z;
}

float NuVec4Dot(struct nuvec4_s *a, struct nuvec4_s *b)
{
    return a->w * b->w + a->x * b->x + a->y * b->y + a->z * b->z;
}

void NuVec4Sub(struct nuvec4_s *dst, struct nuvec4_s *a,
               struct nuvec4_s *b)
{
    dst->x = a->x - b->x;
    dst->y = a->y - b->y;
    dst->z = a->z - b->z;
    dst->w = a->w - b->w;
}

float NuVec4Mag(struct nuvec4_s *v)
{
    return sqrt(v->w * v->w + v->x * v->x + v->y * v->y + v->z * v->z);
}

float NuVec4MagSqr(struct nuvec4_s *v)
{
    return v->w * v->w + v->x * v->x + v->y * v->y + v->z * v->z;
}

void NuVec4Scale(struct nuvec4_s *dst, struct nuvec4_s *src, float scale)
{
    dst->x = src->x * scale;
    dst->y = src->y * scale;
    dst->z = src->z * scale;
    dst->w = src->w * scale;
}

void NuVec4ScaleAccum(struct nuvec4_s *dst, struct nuvec4_s *src, float scale)
{
    dst->x += src->x * scale;
    dst->y += src->y * scale;
    dst->z += src->z * scale;
    dst->w += src->w * scale;
}

void NuVec4MtxTransform(struct nuvec4_s *dst, struct nuvec4_s *src,
                        struct numtx_s *m)
{
    float x = src->x;
    float y = src->y;
    float z = src->z;
    float tx;
    float ty;
    float tz;
    float tw;

    tw = x * m->_03 + y * m->_13 + z * m->_23 + m->_33;
    ty = x * m->_01 + y * m->_11 + z * m->_21 + m->_31;
    tz = x * m->_02 + y * m->_12 + z * m->_22 + m->_32;
    tx = x * m->_00 + y * m->_10 + z * m->_20 + m->_30;

    dst->w = tw;
    dst->y = ty;
    dst->z = tz;
    dst->x = tx;
}

void NuVec4MtxTransformH(struct nuvec4_s *dst, struct nuvec4_s *src,
                         struct numtx_s *m)
{
    float x = src->x;
    float y = src->y;
    float z = src->z;
    float w = src->w;
    float tx;
    float ty;
    float tz;
    float tw;

    tw = x * m->_03 + y * m->_13 + z * m->_23 + w * m->_33;
    ty = x * m->_01 + y * m->_11 + z * m->_21 + w * m->_31;
    tz = x * m->_02 + y * m->_12 + z * m->_22 + w * m->_32;
    tx = x * m->_00 + y * m->_10 + z * m->_20 + w * m->_30;

    dst->w = tw;
    dst->y = ty;
    dst->z = tz;
    dst->x = tx;
}

void NuVec4Lerp(struct nuvec4_s *dst, struct nuvec4_s *a,
                struct nuvec4_s *b, float t)
{
    dst->x = b->x + (a->x - b->x) * t;
    dst->y = b->y + (a->y - b->y) * t;
    dst->z = b->z + (a->z - b->z) * t;
    dst->w = b->w + (a->w - b->w) * t;
}
