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
double cos(double);
double sin(double);
extern float NuTrigTable[];
extern struct numtx_s D_002921a0; //gm
extern struct numtx_s D_002921e0; //numtx_identity
extern struct numtx_s D_00292220; //numtx_zero

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

void NuMtxMulH(struct numtx_s* dest, struct numtx_s* a, struct numtx_s* b)
{
	D_002921a0._00 = a->_00 * b->_00 + a->_01 * b->_10 + a->_02 * b->_20 + a->_03 * b->_30;
	D_002921a0._01 = a->_00 * b->_01 + a->_01 * b->_11 + a->_02 * b->_21 + a->_03 * b->_31;
	D_002921a0._02 = a->_00 * b->_02 + a->_01 * b->_12 + a->_02 * b->_22 + a->_03 * b->_32;
	D_002921a0._03 = a->_00 * b->_03 + a->_01 * b->_13 + a->_02 * b->_23 + a->_03 * b->_33;
	D_002921a0._10 = a->_10 * b->_00 + a->_11 * b->_10 + a->_12 * b->_20 + a->_13 * b->_30;
	D_002921a0._11 = a->_10 * b->_01 + a->_11 * b->_11 + a->_12 * b->_21 + a->_13 * b->_31;
	D_002921a0._12 = a->_10 * b->_02 + a->_11 * b->_12 + a->_12 * b->_22 + a->_13 * b->_32;
	D_002921a0._13 = a->_10 * b->_03 + a->_11 * b->_13 + a->_12 * b->_23 + a->_13 * b->_33;
	D_002921a0._20 = a->_20 * b->_00 + a->_21 * b->_10 + a->_22 * b->_20 + a->_23 * b->_30;
	D_002921a0._21 = a->_20 * b->_01 + a->_21 * b->_11 + a->_22 * b->_21 + a->_23 * b->_31;
	D_002921a0._22 = a->_20 * b->_02 + a->_21 * b->_12 + a->_22 * b->_22 + a->_23 * b->_32;
	D_002921a0._23 = a->_20 * b->_03 + a->_21 * b->_13 + a->_22 * b->_23 + a->_23 * b->_33;
	D_002921a0._30 = a->_30 * b->_00 + a->_31 * b->_10 + a->_32 * b->_20 + a->_33 * b->_30;
	D_002921a0._31 = a->_30 * b->_01 + a->_31 * b->_11 + a->_32 * b->_21 + a->_33 * b->_31;
	D_002921a0._32 = a->_30 * b->_02 + a->_31 * b->_12 + a->_32 * b->_22 + a->_33 * b->_32;
	D_002921a0._33 = a->_30 * b->_03 + a->_31 * b->_13 + a->_32 * b->_23 + a->_33 * b->_33;
	*dest = D_002921a0;
}

void NuMtxMulR(struct numtx_s* dest, struct numtx_s* a, struct numtx_s* b)
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
	D_002921a0._30 = D_002921a0._31 = D_002921a0._32 = 0.00000000f;
	D_002921a0._33 = 1.00000000f;
    *dest = D_002921a0;
}

void NuMtxInvRSS(struct numtx_s* dest, struct numtx_s* m)
{
    float scale;

    float det = m->_00 * (m->_11 * m->_22 - m->_12 * m->_21)
          - m->_01 * (m->_10 * m->_22 - m->_12 * m->_20)
          + m->_02 * (m->_10 * m->_21 - m->_11 * m->_20);
    
	scale = 1.0f / det;
    
	dest->_00 = (m->_11 * m->_22 - m->_12 * m->_21) * scale;
	dest->_10 = (m->_10 * m->_22 - m->_12 * m->_20) * -scale;
	dest->_20 = (m->_10 * m->_21 - m->_11 * m->_20) * scale;
    
	dest->_01 = (m->_01 * m->_22 - m->_02 * m->_21) * -scale;
	dest->_11 = (m->_00 * m->_22 - m->_02 * m->_20) * scale;
	dest->_21 = (m->_00 * m->_21 - m->_01 * m->_20) * -scale;
    
	dest->_02 = (m->_01 * m->_12 - m->_02 * m->_11) * scale;
	dest->_12 = (m->_00 * m->_12 - m->_02 * m->_10) * -scale;
	dest->_22 = (m->_00 * m->_11 - m->_01 * m->_10) * scale;
    
	dest->_32 = 0.0f;
	dest->_03 = 0.0f;
	dest->_13 = 0.0f;
	dest->_23 = 0.0f;
	dest->_30 = 0.0f;
	dest->_31 = 0.0f;
	dest->_33 = 1.0f;
}

void NuMtxInvH(struct numtx_s *mi,struct numtx_s *m0) {
    int cnt;
    int iVar4;
    int iVar5;
    int iVar9;

    double a [4] [4];
    int i;
    int j;
    int p [4];
    double dVar10;
    double uVar12;
    double uVar13;
    double dVar12;
    double dVar13;
    double dVar14;
    char bVar1;
    
    a[0][0] = (double)m0->_00;
    a[0][1] = (double)m0->_01;
    a[0][2] = (double)m0->_02;
    a[0][3] = (double)m0->_03;
    a[1][0] = (double)m0->_10;
    a[1][1] = (double)m0->_11;
    a[1][2] = (double)m0->_12;
    a[1][3] = (double)m0->_13;
    a[2][0] = (double)m0->_20;
    a[2][1] = (double)m0->_21;
    a[2][2] = (double)m0->_22;
    a[2][3] = (double)m0->_23;
    a[3][0] = (double)m0->_30;
    a[3][1] = (double)m0->_31;
    a[3][2] = (double)m0->_32;
    a[3][3] = (double)m0->_33;
    
    for(iVar4 = 0; iVar4 < 4; iVar4++ ) {
        p[iVar4] = 0;
        dVar12 = 0.0;
        for (iVar5 = iVar4; iVar5 < 4; iVar5++) {
            dVar13 = 0.0;
            for (i = 0; i < 4; i++) {
                dVar10 = ((u32)a[iVar5][i] & 0x7fffffff);
                dVar13 += dVar10;
            } 
            dVar10 = (double)((u32)a[iVar5][iVar4] & 0x7fffffff);
            if (dVar12 < dVar10 / dVar13) {
                p[iVar4] = iVar5;
                dVar12 = dVar10 / dVar13;
            }
        }
        if (dVar12 == 0.0) break;
        if (p[iVar4] != iVar4) {
            for (j = 0; j < 4; j++) {
                uVar12 = a[iVar4][j];
                a[iVar4][j] = a[p[iVar4]][j];
                a[p[iVar4]][j] = uVar12;
            }
        }
        dVar14 = a[iVar4][iVar4];
        for (i = 0; i < 4; i++) {
            if (i != iVar4) {
                a[iVar4][i] = -a[iVar4][i] / dVar14;
                for (iVar9 = 0; iVar9 < 4; iVar9++) {
                    if (iVar9 != iVar4) {
                        a[iVar9][i] += a[iVar9][iVar4] * a[iVar4][i];
                    }
                }
            }
        } 
        for (cnt = 0; cnt < 4; cnt++) {
            a[cnt][iVar4] = a[cnt][iVar4] / dVar14;
        }
        a[iVar4][iVar4] = 1.0 / dVar14;
    }
    
    for (iVar4 = 4; iVar4 >= 0; iVar4--) {
        if (p[iVar4] != iVar4) {
            for (j = 0; j < 4; j++) {
                uVar13 = a[j][iVar4];
                a[j][iVar4] = a[j][p[iVar4]];
                a[j][p[iVar4]] = uVar13;
            } 
        }
    } 
    mi->_00 = (float)a[0][0];
    mi->_01 = (float)a[0][1];
    mi->_02 = (float)a[0][2];
    mi->_03 = (float)a[0][3];
    mi->_10 = (float)a[1][0];
    mi->_11 = (float)a[1][1];
    mi->_12 = (float)a[1][2];
    mi->_13 = (float)a[1][3];
    mi->_20 = (float)a[2][0];
    mi->_21 = (float)a[2][1];
    mi->_22 = (float)a[2][2];
    mi->_23 = (float)a[2][3];
    mi->_30 = (float)a[3][0];
    mi->_31 = (float)a[3][1];
    mi->_32 = (float)a[3][2];
    mi->_33 = (float)a[3][3];
}



void NuVecCross(struct nuvec_s* dest, struct nuvec_s* a, struct nuvec_s* b);
float NuVecDot(struct nuvec_s* a, struct nuvec_s* b);
float NuFabs(float);
float NuFsqrt(float); 
extern float D_00643780; //0.86602539f

void NuMtxAlignZ(struct numtx_s *m, struct nuvec_s *v)
{
  float *a;
  float *b;
  float fVar1;
  float fVar2;
  float fVar3;
  
  a = &m->_10;
  b = &m->_20;
  fVar2 = m->_00 * m->_00 + m->_01 * m->_01 + m->_02 * m->_02;
  fVar3 = m->_10 * m->_10 + m->_11 * m->_11 + m->_12 * m->_12;
  fVar1 = NuFsqrt((m->_20 * m->_20 + m->_21 * m->_21 + m->_22 * m->_22) /
                  (v->x * v->x + v->y * v->y + v->z * v->z));
  m->_20 = fVar1 * v->x;
  m->_21 = fVar1 * v->y;
  m->_22 = fVar1 * v->z;
  fVar1 = NuVecDot((struct nuvec_s*)a,(struct nuvec_s*)b);
  fVar1 = NuFabs(fVar1);
  if (0.86602539f < fVar1) {
    NuVecCross((struct nuvec_s*)a,(struct nuvec_s*)b,(struct nuvec_s*)m);
    fVar1 = NuFsqrt(fVar3 / (m->_10 * m->_10 + m->_11 * m->_11 + m->_12 * m->_12));
    m->_10 = m->_10 * fVar1;
    m->_11 = m->_11 * fVar1;
    m->_12 = m->_12 * fVar1;
    NuVecCross((struct nuvec_s*)m,(struct nuvec_s*)a,(struct nuvec_s*)b);
    fVar1 = NuFsqrt(fVar2 / (m->_00 * m->_00 + m->_01 * m->_01 + m->_02 * m->_02));
    m->_00 = m->_00 * fVar1;
    m->_01 = m->_01 * fVar1;
    m->_02 = m->_02 * fVar1;
  }
  else {
    NuVecCross((struct nuvec_s*)m,(struct nuvec_s*)a,(struct nuvec_s*)b);
    fVar1 = NuFsqrt(fVar2 / (m->_00 * m->_00 + m->_01 * m->_01 + m->_02 * m->_02));
    m->_00 = m->_00 * fVar1;
    m->_01 = m->_01 * fVar1;
    m->_02 = m->_02 * fVar1;
    NuVecCross((struct nuvec_s*)a,(struct nuvec_s*)b,(struct nuvec_s*)m);
    fVar1 = NuFsqrt(fVar3 / (m->_10 * m->_10 + m->_11 * m->_11 + m->_12 * m->_12));
    m->_10 = m->_10 * fVar1;
    m->_11 = m->_11 * fVar1;
    m->_12 = m->_12 * fVar1;
  }
  return;
}

void NuMtxAlignX(struct numtx_s *m,struct nuvec_s *v) {
  float fVar4;
  
  m->_00 = v->x;
  m->_01 = v->y;
  m->_02 = v->z;
  m->_20 = m->_01 * m->_12 - m->_02 * m->_11;
  m->_21 = m->_02 * m->_10 - m->_00 * m->_12;
  m->_22 = m->_00 * m->_11 - m->_01 * m->_10;
  fVar4 = (1.0f / (float)sqrt(m->_20 * m->_20 + m->_21 * m->_21 + m->_22 * m->_22));
  m->_20 = m->_20 * fVar4;
  m->_21 = m->_21 * fVar4;
  m->_22 = m->_22 * fVar4;
  m->_10 = m->_21 * m->_02 - m->_22 * m->_01;
  m->_11 = m->_22 * m->_00 - m->_20 * m->_02;
  m->_12 = m->_20 * m->_01 - m->_21 * m->_00;
}

void NuMtxAlignY(struct numtx_s *m,struct nuvec_s *v) {
  float fVar4;
  
  m->_10 = v->x;
  m->_11 = v->y;
  m->_12 = v->z;
  m->_00 = m->_11 * m->_22 - m->_12 * m->_21;
  m->_01 = m->_12 * m->_20 - m->_10 * m->_22;
  m->_02 = m->_10 * m->_21 - m->_11 * m->_20;
  fVar4 = (1.0f / (float)sqrt(m->_00 * m->_00 + m->_01 * m->_01 + m->_02 * m->_02));
  m->_00 = m->_00 * fVar4;
  m->_01 = m->_01 * fVar4;
  m->_02 = m->_02 * fVar4;
  m->_20 = m->_01 * m->_12 - m->_02 * m->_11;
  m->_21 = m->_02 * m->_10 - m->_00 * m->_12;
  m->_22 = m->_00 * m->_11 - m->_01 * m->_10;
}

void NuMtxLookAtX(struct numtx_s *dest,struct nuvec_s *pnt) {
  struct nuvec_s v;
  
  v.x = pnt->x - dest->_30;
  v.y = pnt->y - dest->_31;
  v.z = pnt->z - dest->_32;
  NuVecNorm(&v,&v);
  NuMtxAlignX(dest,&v);
}

void NuMtxLookAtY(struct numtx_s *dest,struct nuvec_s *pnt) {
  struct nuvec_s v;
  
  v.x = pnt->x - dest->_30;
  v.y = pnt->y - dest->_31;
  v.z = pnt->z - dest->_32;
  NuVecNorm(&v,&v);
  NuMtxAlignY(dest,&v);
}

void NuMtxLookAtZ(struct numtx_s *dest,struct nuvec_s *pnt) {
  struct nuvec_s v;
  
  v.x = pnt->x - dest->_30;
  v.y = pnt->y - dest->_31;
  v.z = pnt->z - dest->_32;
  NuVecNorm(&v,&v);
  NuMtxAlignZ(dest,&v);
}

void NuMtxAddR(struct numtx_s *m,struct numtx_s *m0,struct numtx_s *m1) {
  m->_00 = m0->_00 + m1->_00;
  m->_01 = m0->_01 + m1->_01;
  m->_02 = m0->_02 + m1->_02;
  m->_03 = 0.0f;
  m->_10 = m0->_10 + m1->_10;
  m->_11 = m0->_11 + m1->_11;
  m->_12 = m0->_12 + m1->_12;
  m->_13 = 0.0f;
  m->_20 = m0->_20 + m1->_20;
  m->_21 = m0->_21 + m1->_21;
  m->_22 = m0->_22 + m1->_22;
  m->_23 = 0.0f;
  m->_30 = 0.0f;
  m->_31 = 0.0f;
  m->_32 = 0.0f;
  m->_33 = 1.0f;
}

void NuMtxSubR(struct numtx_s *m,struct numtx_s *m0,struct numtx_s *m1) {
  m->_00 = m0->_00 - m1->_00;
  m->_01 = m0->_01 - m1->_01;
  m->_02 = m0->_02 - m1->_02;
  m->_03 = 0.0f;
  m->_10 = m0->_10 - m1->_10;
  m->_11 = m0->_11 - m1->_11;
  m->_12 = m0->_12 - m1->_12;
  m->_13 = 0.0f;
  m->_20 = m0->_20 - m1->_20;
  m->_21 = m0->_21 - m1->_21;
  m->_22 = m0->_22 - m1->_22;
  m->_23 = 0.0f;
  m->_30 = 0.0f;
  m->_31 = 0.0f;
  m->_32 = 0.0f;
  m->_33 = 1.0f;
}

void NuMtxOrth(struct numtx_s* m)
{
    float mag;
    float t1;
    float t2;
    float t3;
    float t4;
    
	mag = NuFsqrt(m->_00 * m->_00 + m->_01 * m->_01 + m->_02 * m->_02);
	t3 = 1.0f / mag;
	m->_00 = m->_00 * t3;
	m->_01 = m->_01 * t3;
	m->_02 = m->_02 * t3;
	mag = NuFsqrt(m->_10 * m->_10 + m->_11 * m->_11 + m->_12 * m->_12);
	t3 = 1.0f / mag;
	t1 = m->_10 * t3;
	t2 = m->_11 * t3;
	t4 = m->_12 * t3;
	m->_20 = m->_01 * t4 - m->_02 * t2;
	m->_21 = m->_02 * t1 - m->_00 * t4;
	m->_22 = m->_00 * t2 - m->_01 * t1;
	m->_10 = m->_21 * m->_02 - m->_22 * m->_01;
	m->_11 = m->_22 * m->_00 - m->_20 * m->_02;
	m->_12 = m->_20 * m->_01 - m->_21 * m->_00;
}

void NuMtxSetZero(struct numtx_s* m)
{
	*m = D_00292220;
}

void NuMtxSetIdentity(struct numtx_s* m)
{
	*m = D_002921e0;
}

void NuMtxTranslate(struct numtx_s *m, struct nuvec_s *v)
{
    m->_30 += v->x;
    m->_31 += v->y;
    m->_32 += v->z;
}

void NuMtxPreTranslate(struct numtx_s *m,struct nuvec_s *t) {
  m->_30 += t->x * m->_00 + t->y * m->_10 + t->z * m->_20;
  m->_31 += t->x * m->_01 + t->y * m->_11 + t->z * m->_21;
  m->_32 += t->x * m->_02 + t->y * m->_12 + t->z * m->_22;
}

void NuMtxScale(struct numtx_s *m,struct nuvec_s *s)
{
  m->_00 = m->_00 * s->x;
  m->_01 = m->_01 * s->y;
  m->_02 = m->_02 * s->z;
  m->_10 = m->_10 * s->x;
  m->_11 = m->_11 * s->y;
  m->_12 = m->_12 * s->z;
  m->_20 = m->_20 * s->x;
  m->_21 = m->_21 * s->y;
  m->_22 = m->_22 * s->z;
  m->_30 = m->_30 * s->x;
  m->_31 = m->_31 * s->y;
  m->_32 = m->_32 * s->z;
}

struct nuvec_s NuMtxGetScale(struct numtx_s* m)
{
    struct nuvec_s scale;
    
	scale.x = sqrt(m->_00 * m->_00 + m->_01 * m->_01 + m->_02 * m->_02);
	scale.y = sqrt(m->_10 * m->_10 + m->_11 * m->_11 + m->_12 * m->_12);
	scale.z = sqrt(m->_20 * m->_20 + m->_21 * m->_21 + m->_22 * m->_22);

    return scale;
}

void NuMtxPreScale(struct numtx_s *m,struct nuvec_s *s) {
  m->_00 = m->_00 * s->x;
  m->_01 = m->_01 * s->x;
  m->_02 = m->_02 * s->x;
  m->_10 = m->_10 * s->y;
  m->_11 = m->_11 * s->y;
  m->_12 = m->_12 * s->y;
  m->_20 = m->_20 * s->z;
  m->_21 = m->_21 * s->z;
  m->_22 = m->_22 * s->z;
}

void NuMtxRotateX(struct numtx_s *m,int a) {
  float cosx;
  float sinx;
  float _01;
  float _11;
  float _21;
  float _31;
  
  cosx = NuTrigTable[a + 0x4000U & 0xffff];
  sinx = NuTrigTable[a & 0xffff];
  _01 = m->_01;
  _11 = m->_11;
  _21 = m->_21;
  _31 = m->_31;
  m->_01 = _01 * cosx - m->_02 * sinx;
  m->_02 = _01 * sinx + m->_02 * cosx;
  m->_11 = _11 * cosx - m->_12 * sinx;
  m->_12 = _11 * sinx + m->_12 * cosx;
  m->_21 = _21 * cosx - m->_22 * sinx;
  m->_22 = _21 * sinx + m->_22 * cosx;
  m->_31 = _31 * cosx - m->_32 * sinx;
  m->_32 = _31 * sinx + m->_32 * cosx;
}

void NuMtxRotateXR(float x,struct numtx_s *m) {
  float cosx;
  float sinx;
  float _01;
  float _11;
  float _21;
  float _31;
  
  cosx = (float)(double)cos((double)x);
  sinx = (float)(double)sin((double)x);
  _01 = m->_01;
  _11 = m->_11;
  _21 = m->_21;
  _31 = m->_31;
  m->_01 = _01 * cosx - m->_02 * sinx;
  m->_02 = _01 * sinx + m->_02 * cosx;
  m->_11 = _11 * cosx - m->_12 * sinx;
  m->_12 = _11 * sinx + m->_12 * cosx;
  m->_21 = _21 * cosx - m->_22 * sinx;
  m->_22 = _21 * sinx + m->_22 * cosx;
  m->_31 = _31 * cosx - m->_32 * sinx;
  m->_32 = _31 * sinx + m->_32 * cosx;
}

void NuMtxPreRotateX(struct numtx_s *m,int a) {
  float cosx;
  float sinx;
  float _10;
  float _11;
  float _12;
  
  cosx = NuTrigTable[a + 0x4000U & 0xffff];
  sinx = NuTrigTable[a & 0xffff];
  _10 = m->_10;
  _11 = m->_11;
  _12 = m->_12;
  m->_10 = cosx * _10 + sinx * m->_20;
  m->_11 = cosx * _11 + sinx * m->_21;
  m->_12 = cosx * _12 + sinx * m->_22;
  m->_20 = cosx * m->_20 - sinx * _10;
  m->_21 = cosx * m->_21 - sinx * _11;
  m->_22 = cosx * m->_22 - sinx * _12;
}

void NuMtxRotateY(struct numtx_s *m,int a) {
  float cosy;
  float siny;
  float _00;
  float _10;
  float _20;
  float _30;
  
  cosy = NuTrigTable[a + 0x4000U & 0xffff];
  siny = NuTrigTable[a & 0xffff];
  _00 = m->_00;
  _10 = m->_10;
  _20 = m->_20;
  _30 = m->_30;
  m->_00 = _00 * cosy + m->_02 * siny;
  m->_02 = m->_02 * cosy - _00 * siny;
  m->_10 = _10 * cosy + m->_12 * siny;
  m->_12 = m->_12 * cosy - _10 * siny;
  m->_20 = _20 * cosy + m->_22 * siny;
  m->_22 = m->_22 * cosy - _20 * siny;
  m->_30 = _30 * cosy + m->_32 * siny;
  m->_32 = m->_32 * cosy - _30 * siny;
}

void NuMtxRotateYR(float x,struct numtx_s *m) {
  float cosy;
  float siny;
  float _00;
  float _10;
  float _30;
  float _20;
  
  cosy = cos((double)x);
  siny = sin((double)x);
  _00 = m->_00;
  _10 = m->_10;
  _20 = m->_20;
  _30 = m->_30;
  m->_00 = _00 * cosy + m->_02 * siny;
  m->_02 = m->_02 * cosy - _00 * siny;
  m->_10 = _10 * cosy + m->_12 * siny;
  m->_12 = m->_12 * cosy - _10 * siny;
  m->_20 = _20 * cosy + m->_22 * siny;
  m->_22 = m->_22 * cosy - _20 * siny;
  m->_30 = _30 * cosy + m->_32 * siny;
  m->_32 = m->_32 * cosy - _30 * siny;
}

void NuMtxPreRotateY(struct numtx_s *m,int a) {
  float cosy;
  float siny;
  float _01;
  float _00;
  float _02;
  
  cosy = NuTrigTable[a + 0x4000U & 0xffff];
  siny = NuTrigTable[a & 0xffff];
  _00 = m->_00;
  _01 = m->_01;
  _02 = m->_02;
  m->_00 = cosy * _00 - siny * m->_20;
  m->_01 = cosy * _01 - siny * m->_21;
  m->_02 = cosy * _02 - siny * m->_22;
  m->_20 = siny * _00 + cosy * m->_20;
  m->_21 = siny * _01 + cosy * m->_21;
  m->_22 = siny * _02 + cosy * m->_22;
}

void NuMtxRotateZ(struct numtx_s *m,int a) {
  float cosz;
  float sinz;
  float _00;
  float _10;
  float _20;
  float _30;
  
  cosz = NuTrigTable[a + 0x4000U & 0xffff];
  sinz = NuTrigTable[a & 0xffff];
  _00 = m->_00;
  _10 = m->_10;
  _20 = m->_20;
  _30 = m->_30;
  m->_00 = _00 * cosz - m->_01 * sinz;
  m->_01 = _00 * sinz + m->_01 * cosz;
  m->_10 = _10 * cosz - m->_11 * sinz;
  m->_11 = _10 * sinz + m->_11 * cosz;
  m->_20 = _20 * cosz - m->_21 * sinz;
  m->_21 = _20 * sinz + m->_21 * cosz;
  m->_30 = _30 * cosz - m->_31 * sinz;
  m->_31 = _30 * sinz + m->_31 * cosz;
}

void NuMtxRotateZR(float x,struct numtx_s *m) {
  float cosz;
  float sinz;
  float _00;
  float _10;
  float _20;
  float _30;
  
  cosz = (float)cos((double)x);
  sinz = (float)sin((double)x);
  _00 = m->_00;
  _10 = m->_10;
  _20 = m->_20;
  _30 = m->_30;
  m->_00 = _00 * cosz - m->_01 * sinz;
  m->_01 = _00 * sinz + m->_01 * cosz;
  m->_10 = _10 * cosz - m->_11 * sinz;
  m->_11 = _10 * sinz + m->_11 * cosz;
  m->_20 = _20 * cosz - m->_21 * sinz;
  m->_21 = _20 * sinz + m->_21 * cosz;
  m->_30 = _30 * cosz - m->_31 * sinz;
  m->_31 = _30 * sinz + m->_31 * cosz;
}

void NuMtxPreRotateZ(struct numtx_s *m,int a) {
  float cosz;
  float sinz;
  float _00;
  float _01;
  float _02;
  
  cosz = NuTrigTable[a + 0x4000U & 0xffff];
  sinz = NuTrigTable[a & 0xffff];
  _00 = m->_00;
  _01 = m->_01;
  _02 = m->_02;
  m->_00 = cosz * _00 + sinz * m->_10;
  m->_01 = cosz * _01 + sinz * m->_11;
  m->_02 = cosz * _02 + sinz * m->_12;
  m->_10 = cosz * m->_10 - sinz * _00;
  m->_11 = cosz * m->_11 - sinz * _01;
  m->_12 = cosz * m->_12 - sinz * _02;
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
    float c;
    float s;
    
    c = NuTrigTable[(r + 0x4000) & 0xffff];
	m->_22 = c;
	m->_11 = c;
    s = NuTrigTable[r & 0xffff];
	m->_12 = s;
	m->_00 = 1.0f;
	m->_21 = -s;
	m->_01 = m->_02 = m->_03 = m->_23 = m->_10 = m->_20 = m->_13 = m->_30 = m->_31 = m->_32 = 0.0f;
	m->_33 = 1.0f;
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

void NuMtxSetRotationZ(struct numtx_s *m,int a) {
  m->_00 = m->_11 = NuTrigTable[a + 0x4000U & 0xffff];
  m->_01 = NuTrigTable[a & 0xffff];
  m->_22 = 1.0f;
  m->_10 = -m->_01;
  m->_02 = m->_12 = m->_03 = m->_23 = m->_20 = m->_21 = m->_13 = m->_30 = m->_31 = m->_32 = 0.0f;
  m->_33 = 1.0f;
  return;
}

float NuMtxDet3(struct numtx_s *m)
{
    return m->_00 * (m->_11 * m->_22 - m->_12 * m->_21) -
           m->_01 * (m->_10 * m->_22 - m->_12 * m->_20) +
           m->_02 * (m->_10 * m->_21 - m->_11 * m->_20);
}

void NuMtxTransposeR(struct numtx_s *m, struct numtx_s *m0)
{
  float t;
    
  t = m0->_01;
  m->_01 = m0->_10;
  m->_10 = t;  
  t = m0->_02;
  m->_02 = m0->_20;
  m->_20 = t;
  t = m0->_12;
  m->_12 = m0->_21;
  m->_21 = t;
  m->_00 = m0->_00;
  m->_11 = m0->_11;
  m->_22 = m0->_22;
  m->_30 = m0->_30;
  m->_31 = m0->_31;
  m->_32 = m0->_32;
  m->_33 = m0->_33;
}

void NuMtxTranspose(struct numtx_s *m,struct numtx_s *m0) {
  float t;

  t = m0->_01;
  m->_01 = m0->_10;
  m->_10 = t;
    
  t = m0->_02;
  m->_02 = m0->_20;
  m->_20 = t;
    
  t = m0->_03;
  m->_03 = m0->_30;
  m->_30 = t;
    
  t = m0->_10;
  m->_10 = m0->_01;
  m->_01 = t;
    
  t = m0->_12;
  m->_12 = m0->_21;
  m->_21 = t;
    
  t = m0->_13;
  m->_13 = m0->_31;
  m->_31 = t;
    
  t = m0->_20;
  m->_20 = m0->_02;
  m->_02 = t;
    
  t = m0->_21;
  m->_21 = m0->_12;
  m->_12 = t;
    
  t = m0->_23;
  m->_23 = m0->_32;
  m->_32 = t;
    
  t = m0->_30;
  m->_30 = m0->_03;
  m->_03 = t;
    
  t = m0->_31;
  m->_31 = m0->_13;
  m->_13 = t;
    
  t = m0->_32;
  m->_32 = m0->_23;
  m->_23 = t;
    
  m->_00 = m0->_00;
  m->_11 = m0->_11;
  m->_22 = m0->_22;
  m->_33 = m0->_33;
}

void NuMtxInvR(struct numtx_s* dest, struct numtx_s* m) {
    float tmp;

    tmp = m->_01;
    dest->_01 = m->_10;
    dest->_10 = tmp;
    tmp = m->_02;
    dest->_02 = m->_20;
    dest->_20 = tmp;
    tmp = m->_12;
    dest->_12 = m->_21;
    dest->_21 = tmp;
    dest->_00 = m->_00;
    dest->_11 = m->_11;
    dest->_22 = m->_22;
    dest->_30 = dest->_31 = dest->_32 = dest->_03 = dest->_13 = dest->_23 = 0.0f;
    dest->_33 = 1.0f;
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
