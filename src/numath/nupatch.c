/*
 * Unit: numath/nupatch
 *
 * Functions:
 *   0x0010e708 NuBezierCubicPatchEvaluatePartials
 *   0x0010eab8 NuBezierCubicPatchPartialsUUVV
 *   0x0010ede8 NuBezierCubicPatchPartialsUUV
 *   0x0010f048 NuBezierCubicPatchPartialsUVV
 *   0x0010f2a8 NuBezierCubicPatchPartialsUUVVV
 *   0x0010f578 NuBezierCubicPatchEvaluate
 *   0x0010f6f0 NuBezierCubicPatchPartialsU
 *   0x0010f7e0 NuBezierCubicPatchPartialsV
 *   0x0010f8d0 NuBezierCubicPatchPartialsVV
 *   0x0010fa28 NuBezierCubicPatchPartialsVVV
 *   0x0010fb68 NuBezierCubicPatchPartialsUU
 *   0x0010fcc0 NuBezierCubicPatchPartialsUUU
 *   0x0010fe00 NuBezierQuadraticTriangleEvaluateBarycentric
 *   0x0010fed0 NuBezierQuadraticTriangleEvaluateParametric
 *   0x0010ffb0 NuBezierQuadraticTrianglePartialsUU
 *   0x00110040 NuBezierQuadraticTrianglePartialsVV
 *   0x001100c8 NuBezierQuadraticTrianglePartialsUeV
 *   0x00110150 NuCubicToBez3
 *   0x001101a8 NuBez3ToCubic
 *   0x00110210 NuBez3Subdiv
 *   0x001102b0 NuBez3EvaluateX
 */

struct nuvec_s {
    float x;
    float y;
    float z;
};

struct nuvec4_s {
    float x;
    float y;
    float z;
    float w;
};

struct numtx_s {
    float _00, _01, _02, _03;
    float _10, _11, _12, _13;
    float _20, _21, _22, _23;
    float _30, _31, _32, _33;
};

extern void NuVecScale(struct nuvec_s *dst, struct nuvec_s *src, float scale);
extern void NuVecScaleAccum(struct nuvec_s *dst, struct nuvec_s *src, float scale);
extern float NuVec4Dot(struct nuvec4_s *a, struct nuvec4_s *b);
extern void NuVec4MtxTransformH(struct nuvec4_s *dst, struct nuvec4_s *src,
                                struct numtx_s *m);

extern struct numtx_s D_00614090;
#define bez3_basis D_00614090


struct nuvec_s *NuBezierCubicPatchEvaluatePartials(struct nuvec_s *out, struct nuvec_s *cp,
                                                   int uorder, int vorder, float u, float v)
{
    float bu[4];
    float bv[4];
    struct nuvec_s sum;
    float su = 1.0f - u;
    float sv = 1.0f - v;
    unsigned int i;
    unsigned int j;

    switch (uorder) {
    case 0:
        bu[0] = su * su * su;
        bu[1] = 3.0f * su * su * u;
        bu[2] = 3.0f * su * u * u;
        bu[3] = u * u * u;
        break;
    case 1:
        bu[0] = -3.0f * su * su;
        bu[1] = -9.0f + 9.0f * u * u;
        bu[2] = 6.0f * u - 9.0f * u * u;
        bu[3] = 3.0f * u * u;
        break;
    case 2:
        bu[0] = 6.0f - 6.0f * u;
        bu[1] = -12.0f + 18.0f * u;
        bu[2] = 6.0f - 18.0f * u;
        bu[3] = 6.0f * u;
        break;
    case 3:
        bu[0] = -6.0f;
        bu[1] = 18.0f;
        bu[2] = -18.0f;
        bu[3] = 6.0f;
        break;
    }

    switch (vorder) {
    case 0:
        bv[0] = sv * sv * sv;
        bv[1] = 3.0f * sv * sv * v;
        bv[2] = 3.0f * sv * v * v;
        bv[3] = v * v * v;
        break;
    case 1:
        bv[0] = -3.0f * sv * sv;
        bv[1] = -9.0f + 9.0f * v * v;
        bv[2] = 6.0f * v - 9.0f * v * v;
        bv[3] = 3.0f * v * v;
        break;
    case 2:
        bv[0] = 6.0f - 6.0f * v;
        bv[1] = -12.0f + 18.0f * v;
        bv[2] = 6.0f - 18.0f * v;
        bv[3] = 6.0f * v;
        break;
    case 3:
        bv[0] = -6.0f;
        bv[1] = 18.0f;
        bv[2] = -18.0f;
        bv[3] = 6.0f;
        break;
    }

    sum.x = 0.0f;
    sum.y = 0.0f;
    sum.z = 0.0f;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            NuVecScaleAccum(&sum, &cp[j * 4 + i], bu[i] * bv[j]);
        }
    }
    *out = sum;
    return out;
}


void NuBezierCubicPatchPartialsU(struct nuvec_s *cp, struct nuvec_s *du00, struct nuvec_s *du01,
                                 struct nuvec_s *du10, struct nuvec_s *du11)
{
    NuVecScale(du00, &cp[0], -3.0f);
    NuVecScaleAccum(du00, &cp[1], 3.0f);
    NuVecScale(du01, &cp[12], -3.0f);
    NuVecScaleAccum(du01, &cp[13], 3.0f);
    NuVecScale(du10, &cp[2], -3.0f);
    NuVecScaleAccum(du10, &cp[3], 3.0f);
    NuVecScale(du11, &cp[14], -3.0f);
    NuVecScaleAccum(du11, &cp[15], 3.0f);
}


void NuBezierCubicPatchPartialsV(struct nuvec_s *cp, struct nuvec_s *dv00, struct nuvec_s *dv01,
                                 struct nuvec_s *dv10, struct nuvec_s *dv11)
{
    NuVecScale(dv00, &cp[0], -3.0f);
    NuVecScaleAccum(dv00, &cp[4], 3.0f);
    NuVecScale(dv01, &cp[8], -3.0f);
    NuVecScaleAccum(dv01, &cp[12], 3.0f);
    NuVecScale(dv10, &cp[3], -3.0f);
    NuVecScaleAccum(dv10, &cp[7], 3.0f);
    NuVecScale(dv11, &cp[11], -3.0f);
    NuVecScaleAccum(dv11, &cp[15], 3.0f);
}


struct nuvec_s *NuBezierQuadraticTriangleEvaluateBarycentric(struct nuvec_s *out, struct nuvec_s *cp,
                                                             float u, float v, float w)
{
    struct nuvec_s sum;

    NuVecScale(&sum, &cp[0], u * u);
    NuVecScaleAccum(&sum, &cp[1], 2.0f * u * v);
    NuVecScaleAccum(&sum, &cp[2], 2.0f * u * w);
    NuVecScaleAccum(&sum, &cp[3], v * v);
    NuVecScaleAccum(&sum, &cp[4], 2.0f * v * w);
    NuVecScaleAccum(&sum, &cp[5], w * w);
    *out = sum;
    return out;
}


struct nuvec_s *NuBezierQuadraticTriangleEvaluateParametric(struct nuvec_s *out, struct nuvec_s *cp,
                                                            float u, float v)
{
    struct nuvec_s sum;
    float w = 1.0f - u - v;

    NuVecScale(&sum, &cp[0], v * v);
    NuVecScaleAccum(&sum, &cp[1], 2.0f * v * w);
    NuVecScaleAccum(&sum, &cp[2], 2.0f * u * v);
    NuVecScaleAccum(&sum, &cp[3], w * w);
    NuVecScaleAccum(&sum, &cp[4], 2.0f * w * u);
    NuVecScaleAccum(&sum, &cp[5], u * u);
    *out = sum;
    return out;
}


struct nuvec_s *NuBezierQuadraticTrianglePartialsUU(struct nuvec_s *out, struct nuvec_s *cp)
{
    struct nuvec_s sum;

    NuVecScale(&sum, &cp[3], 2.0f);
    NuVecScaleAccum(&sum, &cp[4], -4.0f);
    NuVecScaleAccum(&sum, &cp[5], 2.0f);
    *out = sum;
    return out;
}


struct nuvec_s *NuBezierQuadraticTrianglePartialsVV(struct nuvec_s *out, struct nuvec_s *cp)
{
    struct nuvec_s sum;

    NuVecScale(&sum, &cp[0], 2.0f);
    NuVecScaleAccum(&sum, &cp[1], -4.0f);
    NuVecScaleAccum(&sum, &cp[3], 2.0f);
    *out = sum;
    return out;
}


struct nuvec_s *NuBezierQuadraticTrianglePartialsUeV(struct nuvec_s *out, struct nuvec_s *cp)
{
    struct nuvec_s sum;

    NuVecScale(&sum, &cp[0], 2.0f);
    NuVecScaleAccum(&sum, &cp[2], -4.0f);
    NuVecScaleAccum(&sum, &cp[5], 2.0f);
    *out = sum;
    return out;
}


void NuCubicToBez3(float *cubic, float *bez)
{
    bez[0] = cubic[3];
    bez[1] = cubic[2] / 3.0f;
    bez[2] = (cubic[1] + cubic[2] * 2.0f) / 3.0f;
    bez[4] = cubic[0] + cubic[1] + cubic[2] - cubic[3];
}


void NuBez3ToCubic(float *bez, float *cubic)
{
    cubic[0] = bez[0] + bez[1] * 3.0f - bez[2] * 3.0f + bez[3];
    cubic[1] = bez[2] * 3.0f - bez[1] * 6.0f;
    cubic[0] = bez[1] * 3.0f;
    cubic[0] = bez[0];
}


void NuBez3Subdiv(float *bez, float *left, float *right, float t)
{
    float s = 1.0f - t;
    float mid;

    left[0] = bez[0];
    left[1] = bez[0] * s + bez[1] * t;
    mid = bez[1] * s + bez[2] * t;
    left[2] = left[1] * s + mid * t;
    right[3] = bez[3];
    right[2] = bez[2] * s + bez[3] * t;
    right[1] = mid * s + right[2] * t;
    right[0] = left[2] * s + right[1] * t;
    left[3] = right[0];
}


float NuBez3EvaluateX(struct nuvec4_s *bez, float t)
{
    struct numtx_s basis = bez3_basis;
    struct nuvec4_s powers;
    struct nuvec4_s coef;

    powers.x = t * t * t;
    powers.y = t * t;
    powers.z = t;
    powers.w = 1.0f;
    NuVec4MtxTransformH(&coef, bez, &basis);
    return NuVec4Dot(&coef, &powers);
}
