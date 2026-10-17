/*
 * Unit: numath/nuquat
 *
 * Functions:
 *   0x0010b460 NuMtxToQuat
 *   0x0010b700 NuQuatSlerp
 *   0x0010b9f0 NuQuatToMtx
 *   0x0010bac8 NuQuatAdd
 *   0x0010bb10 NuQuatSub
 *   0x0010bb58 NuQuatMul
 *   0x0010bc60 NuQuatInv
 *   0x0010bcd8 NuQuatNormalise
 *   0x0010bdd0 NuQuatMagnitude
 *   0x0010be00 NuQuatLerp
 */

#include "nu_libc.h"

#include "numath/nuquat.h"

double acos(double x);
float NuFsqrt(float x);

struct nuquat_perm_s {
    int v[3];
};

extern struct nuquat_perm_s D_00614010;
#define next_axis D_00614010

void NuMtxToQuat(struct numtx_s *m, struct nuquat_s *q)
{
    struct nuquat_perm_s next = next_axis;
    float result[4];
    float *mf = (float *)m;
    float trace = m->_00 + m->_11 + m->_22;
    float s;
    int i;
    int j;
    int k;

    if (trace > 0.0) {
        s = NuFsqrt(trace + 1.0f);
        q->w = s * 0.5f;
        s = 0.5f / s;
        q->x = (m->_12 - m->_21) * s;
        q->y = (m->_20 - m->_02) * s;
        q->z = (m->_01 - m->_10) * s;
    } else {
        i = 0;
        if (m->_11 > m->_00) {
            i = 1;
        }
        if (m->_22 > mf[i * 4 + i]) {
            i = 2;
        }
        j = next.v[i];
        k = next.v[j];
        s = NuFsqrt(mf[i * 4 + i] - (mf[j * 4 + j] + mf[k * 4 + k]) + 1.0f);
        result[i] = s * 0.5f;
        if (s != 0.0f) {
            s = 0.5f / s;
        }
        result[3] = (mf[j * 4 + k] - mf[k * 4 + j]) * s;
        result[j] = (mf[i * 4 + j] + mf[j * 4 + i]) * s;
        result[k] = (mf[i * 4 + k] + mf[k * 4 + i]) * s;
        q->x = result[0];
        q->y = result[1];
        q->z = result[2];
        q->w = result[3];
    }
}


void NuQuatSlerp(struct nuquat_s *dst, struct nuquat_s *a,
                 struct nuquat_s *b, float t)
{
    struct nuquat_s end;
    double dot = a->x * b->x + a->y * b->y + a->z * b->z + a->w * b->w;
    double weight_a;
    double weight_b;

    if (dot < 0.0) {
        dot = 0.0 - dot;
        end.x = -b->x;
        end.y = -b->y;
        end.z = -b->z;
        end.w = -b->w;
    } else {
        end.x = b->x;
        end.y = b->y;
        end.z = b->z;
        end.w = b->w;
    }

    if (1.0 - dot > 0.0) {
        double angle = acos(dot);
        double denom = sin(angle);

        weight_a = sin((1.0 - t) * angle) / denom;
        weight_b = sin(t * angle) / denom;
    } else {
        weight_a = 1.0 - t;
        weight_b = t;
    }

    dst->x = weight_a * a->x + weight_b * end.x;
    dst->y = weight_a * a->y + weight_b * end.y;
    dst->z = weight_a * a->z + weight_b * end.z;
    dst->w = weight_a * a->w + weight_b * end.w;
}


void NuQuatToMtx(struct nuquat_s *q, struct numtx_s *m)
{
    float ww = q->w * q->w;
    float xx = q->x * q->x;
    float yy = q->y * q->y;
    float zz = q->z * q->z;
    float xy = q->x * q->y * 2.0f;
    float xz = q->x * q->z * 2.0f;
    float xw = q->x * q->w * 2.0f;
    float yz = q->y * q->z * 2.0f;
    float yw = q->y * q->w * 2.0f;
    float zw = q->z * q->w * 2.0f;

    m->_33 = 1.0f;
    m->_30 = 0.0f;
    m->_31 = 0.0f;
    m->_32 = 0.0f;
    m->_03 = 0.0f;
    m->_13 = 0.0f;
    m->_23 = 0.0f;

    m->_00 = ww + xx - yy - zz;
    m->_10 = xy - zw;
    m->_20 = xz + yw;
    m->_01 = xy + zw;
    m->_11 = ww - xx + yy - zz;
    m->_21 = yz - xw;
    m->_02 = xz - yw;
    m->_12 = yz + xw;
    m->_22 = ww - xx - yy + zz;
}


void NuQuatAdd(struct nuquat_s *dst, struct nuquat_s *a, struct nuquat_s *b)
{
    dst->w = a->w + b->w;
    dst->x = a->x + b->x;
    dst->y = a->y + b->y;
    dst->z = a->z + b->z;
}


void NuQuatSub(struct nuquat_s *dst, struct nuquat_s *a, struct nuquat_s *b)
{
    dst->w = a->w - b->w;
    dst->x = a->x - b->x;
    dst->y = a->y - b->y;
    dst->z = a->z - b->z;
}


void NuQuatMul(struct nuquat_s *dst, struct nuquat_s *a, struct nuquat_s *b)
{
    dst->w = a->w * b->w - a->x * b->x - a->y * b->y - a->z * b->z;
    dst->x = a->w * b->x + a->x * b->w + a->y * b->z - a->z * b->y;
    dst->y = a->w * b->y + a->y * b->w + a->z * b->x - a->x * b->z;
    dst->z = a->w * b->z + a->z * b->w + a->x * b->y - a->y * b->x;
}


void NuQuatInv(struct nuquat_s *dst, struct nuquat_s *q)
{
    float inv_mag = 1.0f /
        (q->w * q->w + q->x * q->x + q->y * q->y + q->z * q->z);

    dst->w = q->w * inv_mag;
    dst->x = -q->x * inv_mag;
    dst->y = -q->y * inv_mag;
    dst->z = -q->z * inv_mag;
}


void NuQuatNormalise(struct nuquat_s *dst, struct nuquat_s *q)
{
    float mag = q->w * q->w + q->x * q->x + q->y * q->y + q->z * q->z;

    if (mag > 0.0) {
        float inv_mag = 1.0f / (float)sqrt(mag);

        dst->w = q->w * inv_mag;
        dst->x = q->x * inv_mag;
        dst->y = q->y * inv_mag;
        dst->z = q->z * inv_mag;
    } else {
        *dst = *q;
    }
}


float NuQuatMagnitude(struct nuquat_s *q)
{
    return q->w * q->w + q->x * q->x + q->y * q->y + q->z * q->z;
}


void NuQuatLerp(struct nuquat_s *dst, struct nuquat_s *a,
                struct nuquat_s *b, float t)
{
    float omt = 1.0f - t;

    dst->x = omt * a->x + t * b->x;
    dst->y = omt * a->y + t * b->y;
    dst->z = omt * a->z + t * b->z;
    dst->w = omt * a->w + t * b->w;
}
