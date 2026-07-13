/*
 * Unit: numath/numtx
 *
 * Functions:
 *   0x00107b00 NuMtxSetRotateXYZ
 *   0x00107c18 NuMtxMul
 *   0x00107f20 NuMtxMulH
 *   0x001083b8 NuMtxMulR
 *   0x00108628 NuMtxInvRSS
 *   0x001087e0 NuMtxInvH
 *   0x00108d30 NuMtxAlignZ
 *   0x00108fe0 NuMtxLookAtX
 *   0x00109118 NuMtxLookAtY
 *   0x00109250 NuMtxOrth
 *   0x00109378 NuMtxSetZero
 *   0x00109408 NuMtxSetIdentity
 *   0x00109498 NuMtxSetTranslation
 *   0x001094f8 NuMtxSetScale
 *   0x00109558 NuMtxSetRotationX
 *   0x001095e0 NuMtxSetRotationY
 *   0x00109668 NuMtxSetRotationZ
 *   0x001096f0 NuMtxSetRotationAxis
 *   0x001097e8 NuMtxTranslate
 *   0x00109820 NuMtxPreTranslate
 *   0x001098d0 NuMtxScale
 *   0x00109998 NuMtxGetScale
 *   0x00109a58 NuMtxPreScale
 *   0x00109af0 NuMtxRotateX
 *   0x00109bc0 NuMtxRotateXR
 *   0x00109cc8 NuMtxPreRotateX
 *   0x00109d70 NuMtxRotateY
 *   0x00109e40 NuMtxRotateYR
 *   0x00109f48 NuMtxPreRotateY
 *   0x00109ff0 NuMtxRotateZ
 *   0x0010a0c0 NuMtxRotateZR
 *   0x0010a1c8 NuMtxPreRotateZ
 *   0x0010a270 NuMtxTransposeR
 *   0x0010a2e0 NuMtxTranspose
 *   0x0010a3c8 NuMtxInv
 *   0x0010a498 NuMtxInvR
 *   0x0010a510 NuMtxDet3
 *   0x0010a570 NuMtxAlignX
 *   0x0010a668 NuMtxAlignY
 *   0x0010a760 NuMtxLookAtZ
 *   0x0010a7d0 NuMtxAddR
 *   0x0010a888 NuMtxSubR
 *   0x0010a940 NuMtxSkewSymmetric
 *   0x0010a9b0 NuMtxGetXAxis
 *   0x0010a9d0 NuMtxGetYAxis
 *   0x0010a9f0 NuMtxGetZAxis
 *   0x0010aa10 NuMtxGetTranslation
 */

#include "creature.h"

void NuVecNorm(struct nuvec_s *dst, struct nuvec_s *src);
void NuMtxAlignZ(struct numtx_s *m, struct nuvec_s *v);
extern float NuTrigTable[];

void NuMtxTranslate(struct numtx_s *m, struct nuvec_s *v)
{
    m->_30 += v->x;
    m->_31 += v->y;
    m->_32 += v->z;
}

void NuMtxSetTranslation(struct numtx_s *m, struct nuvec_s *v)
{
    float x = v->x;
    float zero = 0.0f;

    __asm__ volatile("nop" : : "f"(zero));
    m->_30 = x;
    m->_31 = v->y;
    m->_32 = v->z;
    m->_01 = m->_02 = m->_03 = m->_10 = m->_12 = m->_13 = m->_20 =
        m->_21 = m->_23 = zero;
    m->_00 = m->_11 = m->_22 = m->_33 = 1.0f;
}

void NuMtxSetScale(struct numtx_s *m, struct nuvec_s *v)
{
    float x = v->x;
    float zero = 0.0f;

    __asm__ volatile("nop" : : "f"(zero));
    m->_00 = x;
    m->_11 = v->y;
    m->_22 = v->z;
    m->_01 = m->_02 = m->_03 = m->_10 = m->_12 = m->_13 = m->_20 =
        m->_21 = m->_23 = m->_30 = m->_31 = m->_32 = zero;
    m->_33 = 1.0f;
}

void NuMtxSetRotationX(struct numtx_s *m, int r)
{
    float zero = 0.0f;
    float c;
    float s;

    c = NuTrigTable[(r + 0x4000) & 0xffff];
    m->_11 = c;
    m->_22 = c;
    m->_33 = 1.0f;
    s = NuTrigTable[r & 0xffff];
    m->_01 = zero;
    m->_12 = s;
    m->_00 = 1.0f;
    m->_21 = -s;
    m->_32 = m->_02 = m->_03 = m->_23 = m->_10 = m->_20 = m->_13 =
        m->_30 = m->_31 = zero;
}

void NuMtxSetRotationY(struct numtx_s *m, int r)
{
    float zero = 0.0f;
    float c;
    float s;

    c = NuTrigTable[(r + 0x4000) & 0xffff];
    __asm__ volatile("nop");
    m->_22 = c;
    m->_00 = c;
    s = NuTrigTable[r & 0xffff];
    m->_33 = 1.0f;
    m->_01 = zero;
    m->_20 = s;
    m->_11 = 1.0f;
    m->_02 = -s;
    m->_32 = m->_10 = m->_03 = m->_23 = m->_12 = m->_21 = m->_13 =
        m->_30 = m->_31 = zero;
}

float NuMtxDet3(struct numtx_s *m)
{
    return m->_00 * (m->_11 * m->_22 - m->_12 * m->_21) -
           m->_01 * (m->_10 * m->_22 - m->_12 * m->_20) +
           m->_02 * (m->_10 * m->_21 - m->_11 * m->_20);
}

void NuMtxTransposeR(struct numtx_s *dst, struct numtx_s *src)
{
    register float a __asm__("$f2");
    register float b __asm__("$f0");
    register float c __asm__("$f1");

    a = src->_01;
    __asm__ volatile("" : "+f"(a));
    b = src->_10;
    dst->_10 = a;
    dst->_01 = b;
    a = src->_02;
    __asm__ volatile("" : "+f"(a));
    b = src->_20;
    dst->_20 = a;
    dst->_02 = b;
    c = src->_21;
    __asm__ volatile("" : "+f"(c));
    a = src->_12;
    dst->_12 = c;
    dst->_21 = a;
    dst->_00 = src->_00;
    dst->_11 = src->_11;
    dst->_22 = src->_22;
    dst->_30 = src->_30;
    dst->_31 = src->_31;
    dst->_32 = src->_32;
    dst->_33 = src->_33;
}

void NuMtxInvR(struct numtx_s *dst, struct numtx_s *src)
{
    register float a __asm__("$f4");
    register float b __asm__("$f0");
    register float c __asm__("$f1");
    register float zero __asm__("$f2");
    register float one __asm__("$f3");

    a = src->_01;
    __asm__ volatile("" : "+f"(a));
    b = src->_10;
    dst->_10 = a;
    dst->_01 = b;
    zero = 0.0f;
    __asm__ volatile("" : : "f"(zero));
    a = src->_02;
    __asm__ volatile("" : "+f"(a));
    b = src->_20;
    dst->_20 = a;
    dst->_02 = b;
    one = 1.0f;
    __asm__ volatile("" : : "f"(one));
    c = src->_21;
    __asm__ volatile("" : "+f"(c));
    a = src->_12;
    dst->_12 = c;
    dst->_21 = a;
    dst->_00 = src->_00;
    dst->_11 = src->_11;
    dst->_22 = src->_22;
    dst->_30 = dst->_31 = dst->_32 = dst->_03 = dst->_13 = dst->_23 = zero;
    dst->_33 = one;
}

void NuMtxLookAtZ(struct numtx_s *m, struct nuvec_s *v)
{
    struct nuvec_s d;

    d.x = v->x - m->_30;
    d.y = v->y - m->_31;
    d.z = v->z - m->_32;
    NuVecNorm(&d, &d);
    NuMtxAlignZ(m, &d);
}

void NuMtxSkewSymmetric(struct numtx_s *m, struct nuvec_s *v)
{
    m->_00 = 0.0f;
    m->_01 = -v->z;
    m->_02 = v->y;
    m->_03 = 0.0f;
    m->_10 = v->z;
    m->_11 = 0.0f;
    m->_12 = -v->x;
    m->_13 = 0.0f;
    m->_20 = -v->y;
    m->_21 = v->x;
    m->_22 = 0.0f;
    m->_23 = 0.0f;
    m->_30 = 0.0f;
    m->_31 = 0.0f;
    m->_32 = 0.0f;
    m->_33 = 1.0f;
}

void NuMtxGetXAxis(struct numtx_s *m, struct nuvec_s *v)
{
    v->x = m->_00;
    v->y = m->_01;
    v->z = m->_02;
}

void NuMtxGetYAxis(struct numtx_s *m, struct nuvec_s *v)
{
    v->x = m->_10;
    v->y = m->_11;
    v->z = m->_12;
}

void NuMtxGetZAxis(struct numtx_s *m, struct nuvec_s *v)
{
    v->x = m->_20;
    v->y = m->_21;
    v->z = m->_22;
}

void NuMtxGetTranslation(struct numtx_s *m, struct nuvec_s *v)
{
    v->x = m->_30;
    v->y = m->_31;
    v->z = m->_32;
}
