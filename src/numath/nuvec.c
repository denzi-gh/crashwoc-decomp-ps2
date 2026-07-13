/*
 * Unit: numath/nuvec
 *
 * Functions:
 *   0x001066f0 NuVecRotateAxis
 *   0x00106848 NuVecMtxTransform
 *   0x001068e0 NuVecMtxTransformH
 *   0x001069b8 NuVecMtxTranslate
 *   0x001069f0 NuVecMtxRotate
 *   0x00106a70 NuVecMtxRotateValX
 *   0x00106a98 NuVecMtxRotateValY
 *   0x00106ac0 NuVecMtxRotateValZ
 *   0x00106ae8 NuVecMtxRotateH
 *   0x00106ba0 NuVecMtxScale
 *   0x00106bd8 NuVecInvMtxTransform
 *   0x00106c70 NuVecInvMtxTranslate
 *   0x00106ca8 NuVecInvMtxRotate
 *   0x00106d28 NuVecInvMtxRotateValX
 *   0x00106d58 NuVecInvMtxRotateValY
 *   0x00106d88 NuVecInvMtxRotateValZ
 *   0x00106db8 NuVecInvMtxScale
 *   0x00106df0 NuVecRotateX
 *   0x00106e58 NuVecRotateY
 *   0x00106ec0 NuVecRotateZ
 *   0x00106f28 NuVecAdd
 *   0x00106f60 NuVecSub
 *   0x00106f98 NuVecScale
 *   0x00106fc0 NuVecScaleAccum
 *   0x00107000 NuVecInvScale
 *   0x00107038 NuVecCross
 *   0x00107088 NuVecCrossRel
 *   0x001070f8 NuVecDot
 *   0x00107128 NuVecMag
 *   0x00107160 NuVecMagSqr
 *   0x00107188 NuVecNorm
 *   0x00107210 NuVecSurfaceNormal
 *   0x00107300 NuVecDist
 *   0x001073a8 NuVecDistSqr
 *   0x00107440 NuVecXZDist
 *   0x001074d0 NuVecXZDistSqr
 *   0x00107550 NuVecLerp
 *   0x001075a8 NuLineLineIntersect
 */

typedef float f32;

struct nuvec_s {
    f32 x;
    f32 y;
    f32 z;
};

struct numtx_s {
    f32 _00, _01, _02, _03;
    f32 _10, _11, _12, _13;
    f32 _20, _21, _22, _23;
    f32 _30, _31, _32, _33;
};

f32 NuFsqrt(f32 x);
f32 NuMtxDet3(struct numtx_s *mtx);
extern f32 NuTrigTable[];

void NuVecRotateAxis(struct nuvec_s *dst, struct nuvec_s *src, void *unused,
                     struct nuvec_s *rot)
{
    f32 sx = NuTrigTable[(int)rot->x & 0xffff];
    f32 sy = NuTrigTable[(int)rot->y & 0xffff];
    f32 sz = NuTrigTable[(int)rot->z & 0xffff];
    f32 cx = NuTrigTable[(int)(rot->x + 16384.0f) & 0xffff];
    f32 cy = NuTrigTable[(int)(rot->y + 16384.0f) & 0xffff];
    f32 cz = NuTrigTable[(int)(rot->z + 16384.0f) & 0xffff];
    f32 x = src->x * (cy * cz) +
            src->y * (sx * sy * cz - cx * sz) +
            src->z * (cx * sy * cz + sx * sz);
    f32 z = src->y * (sx * cy) + src->z * (cx * cy) - src->x * sy;
    f32 y = src->x * (cy * sz) +
            src->y * (sx * sy * sz + cx * cz) +
            src->z * (cx * sy * sz - sx * cz);

    dst->x = x;
    dst->z = z;
    dst->y = y;
}

void NuVecMtxTranslate(struct nuvec_s *dst, struct nuvec_s *src,
                       struct numtx_s *mtx)
{
    dst->x = src->x + mtx->_30;
    dst->y = src->y + mtx->_31;
    dst->z = src->z + mtx->_32;
}

void NuVecMtxRotate(struct nuvec_s *dst, struct nuvec_s *src,
                    struct numtx_s *mtx)
{
    f32 y = src->x * mtx->_01 + src->y * mtx->_11 + src->z * mtx->_21;
    f32 z = src->x * mtx->_02 + src->y * mtx->_12 + src->z * mtx->_22;
    f32 x = src->x * mtx->_00 + src->y * mtx->_10 + src->z * mtx->_20;

    dst->y = y;
    dst->x = x;
    dst->z = z;
}

void NuVecMtxTransform(struct nuvec_s *dst, struct nuvec_s *src,
                       struct numtx_s *mtx)
{
    f32 y = src->x * mtx->_01 + src->y * mtx->_11 +
            src->z * mtx->_21 + mtx->_31;
    f32 z = src->x * mtx->_02 + src->y * mtx->_12 +
            src->z * mtx->_22 + mtx->_32;
    f32 x = src->x * mtx->_00 + src->y * mtx->_10 +
            src->z * mtx->_20 + mtx->_30;

    dst->y = y;
    dst->x = x;
    dst->z = z;
}

void NuVecMtxTransformH(struct nuvec_s *dst, struct nuvec_s *src,
                        struct numtx_s *mtx)
{
    f32 w = src->x * mtx->_03 + src->y * mtx->_13 +
            src->z * mtx->_23 + mtx->_33;
    f32 inv_w = 1.0f / w;
    f32 y = (src->x * mtx->_01 + src->y * mtx->_11 +
             src->z * mtx->_21 + mtx->_31) * inv_w;
    f32 z = (src->x * mtx->_02 + src->y * mtx->_12 +
             src->z * mtx->_22 + mtx->_32) * inv_w;
    f32 x = (src->x * mtx->_00 + src->y * mtx->_10 +
             src->z * mtx->_20 + mtx->_30) * inv_w;

    dst->y = y;
    dst->z = z;
    dst->x = x;
}

void NuVecMtxRotateValX(struct nuvec_s *dst, struct numtx_s *mtx, f32 val)
{
    dst->x = val * mtx->_00;
    dst->y = val * mtx->_01;
    dst->z = val * mtx->_02;
}

void NuVecMtxRotateValY(struct nuvec_s *dst, struct numtx_s *mtx, f32 val)
{
    dst->x = val * mtx->_10;
    dst->y = val * mtx->_11;
    dst->z = val * mtx->_12;
}

void NuVecMtxRotateValZ(struct nuvec_s *dst, struct numtx_s *mtx, f32 val)
{
    dst->x = val * mtx->_20;
    dst->y = val * mtx->_21;
    dst->z = val * mtx->_22;
}

void NuVecMtxRotateH(struct nuvec_s *dst, struct nuvec_s *src,
                     struct numtx_s *mtx)
{
    f32 w = src->x * mtx->_03 + src->y * mtx->_13 + src->z * mtx->_23;
    f32 inv_w = 1.0f / w;
    f32 y = (src->x * mtx->_01 + src->y * mtx->_11 +
             src->z * mtx->_21) * inv_w;
    f32 z = (src->x * mtx->_02 + src->y * mtx->_12 +
             src->z * mtx->_22) * inv_w;
    f32 x = (src->x * mtx->_00 + src->y * mtx->_10 +
             src->z * mtx->_20) * inv_w;

    dst->y = y;
    dst->x = x;
    dst->z = z;
}

void NuVecMtxScale(struct nuvec_s *dst, struct nuvec_s *src,
                   struct numtx_s *mtx)
{
    dst->x = src->x * mtx->_00;
    dst->y = src->y * mtx->_11;
    dst->z = src->z * mtx->_22;
}

void NuVecInvMtxTranslate(struct nuvec_s *dst, struct nuvec_s *src,
                          struct numtx_s *mtx)
{
    dst->x = src->x - mtx->_30;
    dst->y = src->y - mtx->_31;
    dst->z = src->z - mtx->_32;
}

void NuVecInvMtxRotate(struct nuvec_s *dst, struct nuvec_s *src,
                       struct numtx_s *mtx)
{
    f32 y = src->x * mtx->_10 + src->y * mtx->_11 + src->z * mtx->_12;
    f32 z = src->x * mtx->_20 + src->y * mtx->_21 + src->z * mtx->_22;
    f32 x = src->x * mtx->_00 + src->y * mtx->_01 + src->z * mtx->_02;

    dst->y = y;
    dst->x = x;
    dst->z = z;
}

void NuVecInvMtxTransform(struct nuvec_s *dst, struct nuvec_s *src,
                          struct numtx_s *mtx)
{
    f32 x = src->x - mtx->_30;
    f32 y = src->y - mtx->_31;
    f32 z = src->z - mtx->_32;

    dst->x = x * mtx->_00 + y * mtx->_01 + z * mtx->_02;
    dst->y = x * mtx->_10 + y * mtx->_11 + z * mtx->_12;
    dst->z = x * mtx->_20 + y * mtx->_21 + z * mtx->_22;
}

void NuVecInvMtxRotateValX(struct nuvec_s *v, struct numtx_s *mtx)
{
    v->x *= mtx->_00;
    v->y = v->x * mtx->_10;
    v->z = v->x * mtx->_20;
}

void NuVecInvMtxRotateValY(struct nuvec_s *v, struct numtx_s *mtx)
{
    v->x = v->y * mtx->_01;
    v->y *= mtx->_11;
    v->z = v->y * mtx->_21;
}

void NuVecInvMtxRotateValZ(struct nuvec_s *v, struct numtx_s *mtx)
{
    v->x = v->z * mtx->_02;
    v->y = v->z * mtx->_12;
    v->z *= mtx->_22;
}

void NuVecInvMtxScale(struct nuvec_s *dst, struct nuvec_s *src,
                      struct numtx_s *mtx)
{
    dst->x = src->x / mtx->_00;
    dst->y = src->y / mtx->_11;
    dst->z = src->z / mtx->_22;
}

void NuVecRotateX(struct nuvec_s *dst, struct nuvec_s *src, int angle)
{
    f32 c = NuTrigTable[(angle + 0x4000) & 0xffff];
    f32 s = NuTrigTable[angle & 0xffff];
    f32 y = src->y;

    dst->x = src->x;
    dst->y = y * c - src->z * s;
    dst->z = y * s + src->z * c;
}

void NuVecRotateY(struct nuvec_s *dst, struct nuvec_s *src, int angle)
{
    f32 c = NuTrigTable[(angle + 0x4000) & 0xffff];
    f32 s = NuTrigTable[angle & 0xffff];
    f32 x = src->x;

    dst->x = x * c + src->z * s;
    dst->y = src->y;
    dst->z = src->z * c - x * s;
}

void NuVecRotateZ(struct nuvec_s *dst, struct nuvec_s *src, int angle)
{
    f32 c = NuTrigTable[(angle + 0x4000) & 0xffff];
    f32 s = NuTrigTable[angle & 0xffff];
    f32 x = src->x;

    dst->x = x * c - src->y * s;
    dst->y = x * s + src->y * c;
    dst->z = src->z;
}

void NuVecAdd(struct nuvec_s *dst, struct nuvec_s *a, struct nuvec_s *b)
{
    dst->x = a->x + b->x;
    dst->y = a->y + b->y;
    dst->z = a->z + b->z;
}

void NuVecSub(struct nuvec_s *dst, struct nuvec_s *a, struct nuvec_s *b)
{
    dst->x = a->x - b->x;
    dst->y = a->y - b->y;
    dst->z = a->z - b->z;
}

void NuVecScale(struct nuvec_s *dst, struct nuvec_s *src, f32 scale)
{
    dst->x = src->x * scale;
    dst->y = src->y * scale;
    dst->z = src->z * scale;
}

void NuVecScaleAccum(struct nuvec_s *dst, struct nuvec_s *src, f32 scale)
{
    dst->x += src->x * scale;
    dst->y += src->y * scale;
    dst->z += src->z * scale;
}

void NuVecInvScale(struct nuvec_s *dst, struct nuvec_s *src, f32 scale)
{
    f32 inv_scale = 1.0f / scale;

    dst->x = src->x * inv_scale;
    dst->y = src->y * inv_scale;
    dst->z = src->z * inv_scale;
}

void NuVecCross(struct nuvec_s *dst, struct nuvec_s *a, struct nuvec_s *b)
{
    f32 y = a->z * b->x - a->x * b->z;
    f32 z = a->x * b->y - a->y * b->x;
    f32 x = a->y * b->z - a->z * b->y;

    dst->z = z;
    dst->x = x;
    dst->y = y;
}

void NuVecCrossRel(struct nuvec_s *dst, struct nuvec_s *origin,
                   struct nuvec_s *p1, struct nuvec_s *p2)
{
    f32 y = (p1->z - origin->z) * (p2->x - origin->x) -
            (p1->x - origin->x) * (p2->z - origin->z);
    f32 z = (p1->x - origin->x) * (p2->y - origin->y) -
            (p1->y - origin->y) * (p2->x - origin->x);
    f32 x = (p1->y - origin->y) * (p2->z - origin->z) -
            (p1->z - origin->z) * (p2->y - origin->y);

    dst->z = z;
    dst->x = x;
    dst->y = y;
}

f32 NuVecMagSqr(struct nuvec_s *v)
{
    return v->x * v->x + v->y * v->y + v->z * v->z;
}

f32 NuVecMag(struct nuvec_s *v)
{
    return NuFsqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}

void NuVecNorm(struct nuvec_s *dst, struct nuvec_s *src)
{
    f32 scale;

    scale = NuFsqrt(src->x * src->x + src->y * src->y + src->z * src->z);
    scale = 1.0f / scale;
    dst->x = src->x * scale;
    dst->y = src->y * scale;
    dst->z = src->z * scale;
}

void NuVecSurfaceNormal(struct nuvec_s *dst, struct nuvec_s *p0,
                        struct nuvec_s *p1, struct nuvec_s *p2)
{
    struct nuvec_s u;
    struct nuvec_s v;
    f32 scale;

    u.x = p0->x - p1->x;
    u.y = p0->y - p1->y;
    u.z = p0->z - p1->z;
    v.x = p0->x - p2->x;
    v.y = p0->y - p2->y;
    v.z = p0->z - p2->z;
    dst->x = v.y * u.z - v.z * u.y;
    dst->y = v.z * u.x - v.x * u.z;
    dst->z = v.x * u.y - v.y * u.x;
    scale = NuFsqrt(dst->x * dst->x + dst->y * dst->y + dst->z * dst->z);
    scale = 1.0f / scale;
    dst->x *= scale;
    dst->y *= scale;
    dst->z *= scale;
}

f32 NuVecDot(struct nuvec_s *a, struct nuvec_s *b)
{
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

void NuVecLerp(struct nuvec_s *dst, struct nuvec_s *a, struct nuvec_s *b,
               f32 t)
{
    f32 omt = 1.0f - t;

    dst->x = a->x * t + b->x * omt;
    dst->y = a->y * t + b->y * omt;
    dst->z = a->z * t + b->z * omt;
}

f32 NuVecDist(struct nuvec_s *a, struct nuvec_s *b, struct nuvec_s *diff)
{
    struct nuvec_s local;
    f32 first;
    f32 last;

    if (diff == 0) {
        local.x = a->x - b->x;
        local.y = a->y - b->y;
        local.z = a->z - b->z;
        first = local.x * local.x + local.y * local.y;
        last = local.z * local.z;
    } else {
        diff->x = a->x - b->x;
        diff->y = a->y - b->y;
        diff->z = a->z - b->z;
        first = diff->x * diff->x + diff->y * diff->y;
        last = diff->z * diff->z;
    }
    return NuFsqrt(first + last);
}

f32 NuVecDistSqr(struct nuvec_s *a, struct nuvec_s *b,
                 struct nuvec_s *diff)
{
    struct nuvec_s local;
    f32 first;
    f32 last;

    if (diff == 0) {
        local.x = a->x - b->x;
        local.y = a->y - b->y;
        local.z = a->z - b->z;
        first = local.x * local.x + local.y * local.y;
        last = local.z * local.z;
    } else {
        diff->x = a->x - b->x;
        diff->y = a->y - b->y;
        diff->z = a->z - b->z;
        first = diff->x * diff->x + diff->y * diff->y;
        last = diff->z * diff->z;
    }
    return first + last;
}

f32 NuVecXZDist(struct nuvec_s *a, struct nuvec_s *b,
                struct nuvec_s *diff)
{
    struct nuvec_s local;
    f32 first;
    f32 last;

    if (diff == 0) {
        local.x = a->x - b->x;
        local.y = 0.0f;
        local.z = a->z - b->z;
        first = local.x * local.x + local.y;
        last = local.z * local.z;
    } else {
        diff->x = a->x - b->x;
        diff->y = 0.0f;
        diff->z = a->z - b->z;
        first = diff->x * diff->x + diff->y;
        last = diff->z * diff->z;
    }
    return NuFsqrt(first + last);
}

f32 NuVecXZDistSqr(struct nuvec_s *a, struct nuvec_s *b,
                   struct nuvec_s *diff)
{
    struct nuvec_s local;
    f32 first;
    f32 last;

    if (diff == 0) {
        struct nuvec_s *d = &local;

        d->x = a->x - b->x;
        d->y = 0.0f;
        d->z = a->z - b->z;
        first = d->x * d->x + d->y;
        last = d->z * d->z;
    } else {
        diff->x = a->x - b->x;
        diff->y = 0.0f;
        diff->z = a->z - b->z;
        first = diff->x * diff->x + diff->y;
        last = diff->z * diff->z;
    }
    return first + last;
}

void NuLineLineIntersect(struct nuvec_s *p0, struct nuvec_s *d0,
                         struct nuvec_s *p1, struct nuvec_s *d1,
                         f32 *t0, f32 *t1)
{
    struct numtx_s m;
    struct nuvec_s n;
    f32 inv_mag_sqr;

    n.y = d0->z * d1->x - d0->x * d1->z;
    n.x = d0->y * d1->z - d0->z * d1->y;
    n.z = d0->x * d1->y - d0->y * d1->x;
    inv_mag_sqr = 1.0f / NuFsqrt(n.x * n.x + n.y * n.y + n.z * n.z);
    inv_mag_sqr *= inv_mag_sqr;

    m._00 = p1->x - p0->x;
    m._01 = p1->y - p0->y;
    m._02 = p1->z - p0->z;
    m._10 = d1->x;
    m._11 = d1->y;
    m._12 = d1->z;
    m._20 = n.x;
    m._21 = n.y;
    m._22 = n.z;
    *t0 = NuMtxDet3(&m) * inv_mag_sqr;

    m._10 = d0->x;
    m._11 = d0->y;
    m._12 = d0->z;
    *t1 = NuMtxDet3(&m) * inv_mag_sqr;
}
