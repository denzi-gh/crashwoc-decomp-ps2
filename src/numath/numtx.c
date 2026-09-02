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
extern struct numtx_s D_002921a0; //gm

void NuMtxSetRotateXYZ(struct numtx_s* m, struct nuangvec_s* a)
{
    float sx; 
    float cx; 
    float sy;
    float cy; 
    float sz; 
    float cz;
    
	cx = NuTrigTable[a->x + 0x4000 & 0xffff];
	sx = NuTrigTable[a->x & 0xffff];
	cy = NuTrigTable[a->y + 0x4000 & 0xffff];
	sy = NuTrigTable[a->y & 0xffff];
	cz = NuTrigTable[a->z + 0x4000 & 0xffff];
	sz = NuTrigTable[a->z & 0xffff];
	m->_00 = cy * cz;
	m->_01 = cy * sz;
	m->_02 = -sy;
	m->_03 = 0.00000000f;
	m->_10 = sx * sy * cz - cx * sz;
	m->_11 = sx * sy * sz + cx * cz;
	m->_12 = sx * cy;
	m->_13 = 0.00000000f;
	m->_23 = 0.00000000f;
	m->_30 = 0.00000000f;
	m->_31 = 0.00000000f;
	m->_32 = 0.00000000f;
	m->_33 = 1.00000000f;
	m->_20 = cx * sy * cz + sx * sz;
	m->_21 = cx * sy * sz - sx * cz;
	m->_22 = cx * cy;
}

void NuMtxMul(struct numtx_s* dest, struct numtx_s* a, struct numtx_s* b)
{
	D_002921a0._00 = a->_00 * b->_00 + a->_01 * b->_10 + a->_02 * b->_20;
	D_002921a0._01 = a->_00 * b->_01 + a->_01 * b->_11 + a->_02 * b->_21;
	D_002921a0._02 = a->_00 * b->_02 + a->_01 * b->_12 + a->_02 * b->_22;
	D_002921a0._03 = 0.00000000f;
	D_002921a0._10 = a->_10 * b->_00 + a->_11 * b->_10 + a->_12 * b->_20;
	D_002921a0._11 = a->_10 * b->_01 + a->_11 * b->_11 + a->_12 * b->_21;
	D_002921a0._12 = a->_10 * b->_02 + a->_11 * b->_12 + a->_12 * b->_22;
	D_002921a0._13 = 0.00000000f;
	D_002921a0._20 = a->_20 * b->_00 + a->_21 * b->_10 + a->_22 * b->_20;
	D_002921a0._21 = a->_20 * b->_01 + a->_21 * b->_11 + a->_22 * b->_21;
	D_002921a0._22 = a->_20 * b->_02 + a->_21 * b->_12 + a->_22 * b->_22;
	D_002921a0._23 = 0.00000000f;
	D_002921a0._30 = a->_30 * b->_00 + a->_31 * b->_10 + a->_32 * b->_20 + b->_30;
	D_002921a0._31 = a->_30 * b->_01 + a->_31 * b->_11 + a->_32 * b->_21 + b->_31;
	D_002921a0._32 = a->_30 * b->_02 + a->_31 * b->_12 + a->_32 * b->_22 + b->_32;
	D_002921a0._33 = 1.00000000f;
	*dest = D_002921a0;
}

void NuMtxTranslate(struct numtx_s *m, struct nuvec_s *v)
{
    m->_30 += v->x;
    m->_31 += v->y;
    m->_32 += v->z;
}

void NuMtxSetTranslation(struct numtx_s *m, struct nuvec_s *v)
{
    m->_30 = v->x;
    m->_31 = v->y;
    m->_32 = v->z;
    m->_01 = m->_02 = m->_03 = m->_10 = m->_12 = m->_13 = m->_20 =
        m->_21 = m->_23 = 0.0f;
    m->_00 = m->_11 = m->_22 = m->_33 = 1.0f;
}

void NuMtxSetScale(struct numtx_s *m, struct nuvec_s *v)
{
    m->_00 = v->x;
    m->_11 = v->y;
    m->_22 = v->z;
    m->_01 = m->_02 = m->_03 = m->_10 = m->_12 = m->_13 = m->_20 =
        m->_21 = m->_23 = m->_30 = m->_31 = m->_32 = 0.0f;
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
    float a;
    float b;

    a = src->_01;
    b = src->_10;
    dst->_10 = a;
    dst->_01 = b;
    a = src->_02;
    b = src->_20;
    dst->_20 = a;
    dst->_02 = b;
    b = src->_21;
    a = src->_12;
    dst->_12 = b;
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
    float a;
    float b;

    a = src->_01;
    b = src->_10;
    dst->_10 = a;
    dst->_01 = b;
    a = src->_02;
    b = src->_20;
    dst->_20 = a;
    dst->_02 = b;
    b = src->_21;
    a = src->_12;
    dst->_12 = b;
    dst->_21 = a;
    dst->_00 = src->_00;
    dst->_11 = src->_11;
    dst->_22 = src->_22;
    dst->_30 = dst->_31 = dst->_32 = dst->_03 = dst->_13 = dst->_23 = 0.0f;
    dst->_33 = 1.0f;
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
