/*
 * Unit: sdk/gcc/fp-bit
 *
 * Functions:
 *   0x0026f748 __pack_f
 *   0x0026f858 __unpack_f
 *   0x0026f8e8 _fpadd_parts
 *   0x0026fb20 fpadd
 *   0x0026fb78 fpsub
 *   0x0026fbe0 fpmul
 *   0x0026fdd8 fpdiv
 *   0x0026ff38 __fpcmp_parts_f
 *   0x00270050 fpcmp
 *   0x002700a0 sitofp
 *   0x00270158 fptosi
 *   0x002701e8 fptoui
 *   0x00270280 __negsf2
 *   0x002702b8 __make_fp
 *   0x002702e8 fptodp
 */


typedef float FLO_type;
typedef unsigned int fractype;

typedef enum {
    CLASS_SNAN,
    CLASS_QNAN,
    CLASS_ZERO,
    CLASS_NUMBER,
    CLASS_INFINITY
} fp_class_type;

typedef struct {
    fp_class_type class;
    unsigned int sign;
    int normal_exp;
    union {
        fractype ll;
    } fraction;
} fp_number_type;

typedef union {
    FLO_type value;
    struct {
        fractype fraction : 23;
        unsigned int exp : 8;
        unsigned int sign : 1;
    } bits;
} FLO_union_type;

#define NGARDS 7
#define GARDROUND 0x3f
#define GARDMASK 0x7f
#define GARDMSB 0x40
#define EXPBIAS 127
#define FRACBITS 23
#define EXPMAX 0xff
#define QUIET_NAN 0x100000
#define FRAC_NBITS 32
#define NORMAL_EXPMIN (-EXPBIAS + 1)
#define IMPLICIT_1 (1 << (FRACBITS + NGARDS))
#define IMPLICIT_2 (1U << (FRACBITS + 1 + NGARDS))
#define F_D_BITOFF 30
#define FRACHIGH 0x80000000

extern double __make_dp(fp_class_type class, unsigned int sign, int exp, unsigned long long frac);


static inline fp_number_type *nan(void) {
    static fp_number_type thenan;

    return &thenan;
}


static inline int isnan(fp_number_type *x) {
    return x->class == CLASS_SNAN || x->class == CLASS_QNAN;
}


static inline int isinf(fp_number_type *x) {
    return x->class == CLASS_INFINITY;
}


static inline int iszero(fp_number_type *x) {
    return x->class == CLASS_ZERO;
}


FLO_type __pack_f(fp_number_type *src) {
    FLO_union_type dst;
    fractype fraction = src->fraction.ll;
    int sign = src->sign;
    int exp = 0;

    if (isnan(src)) {
        exp = EXPMAX;
        fraction |= QUIET_NAN;
    } else if (isinf(src)) {
        exp = EXPMAX;
        fraction = 0;
    } else if (iszero(src)) {
        exp = 0;
        fraction = 0;
    } else if (fraction == 0) {
        exp = 0;
    } else {
        if (src->normal_exp < NORMAL_EXPMIN) {
            int shift = NORMAL_EXPMIN - src->normal_exp;

            exp = 0;
            if (shift > FRAC_NBITS - NGARDS) {
                fraction = 0;
            } else {
                fraction >>= shift;
            }
            fraction >>= NGARDS;
        } else if (src->normal_exp > EXPBIAS) {
            exp = EXPMAX;
            fraction = 0;
        } else {
            exp = src->normal_exp + EXPBIAS;
            if ((fraction & GARDMASK) == GARDMSB) {
                if (fraction & (1 << NGARDS)) {
                    fraction += GARDROUND + 1;
                }
            } else {
                fraction += GARDROUND;
            }
            if (fraction >= IMPLICIT_2) {
                fraction >>= 1;
                exp += 1;
            }
            fraction >>= NGARDS;
        }
    }

    dst.bits.fraction = fraction;
    dst.bits.exp = exp;
    dst.bits.sign = sign;
    return dst.value;
}


void __unpack_f(FLO_union_type *src, fp_number_type *dst) {
    fractype fraction = src->bits.fraction;
    int exp = src->bits.exp;
    int sign = src->bits.sign;

    dst->sign = sign;
    if (exp == 0) {
        dst->class = CLASS_ZERO;
    } else if (exp == EXPMAX) {
        if (fraction == 0) {
            dst->class = CLASS_INFINITY;
        } else {
            if (fraction & QUIET_NAN) {
                dst->class = CLASS_QNAN;
            } else {
                dst->class = CLASS_SNAN;
            }
            dst->fraction.ll = fraction;
        }
    } else {
        dst->normal_exp = exp - EXPBIAS;
        dst->class = CLASS_NUMBER;
        dst->fraction.ll = (fraction << NGARDS) | IMPLICIT_1;
    }
}


static fp_number_type *_fpadd_parts(fp_number_type *a, fp_number_type *b, fp_number_type *tmp) {
    int tfraction;
    int a_normal_exp;
    int b_normal_exp;
    fractype a_fraction;
    fractype b_fraction;

    if (isnan(a)) {
        return a;
    }
    if (isnan(b)) {
        return b;
    }
    if (isinf(a)) {
        if (isinf(b) && a->sign != b->sign) {
            return nan();
        }
        return a;
    }
    if (isinf(b)) {
        return b;
    }
    if (iszero(b)) {
        if (iszero(a)) {
            *tmp = *a;
            tmp->sign = a->sign & b->sign;
            return tmp;
        }
        return a;
    }
    if (iszero(a)) {
        return b;
    }

    {
        int diff;

        a_normal_exp = a->normal_exp;
        b_normal_exp = b->normal_exp;
        a_fraction = a->fraction.ll;
        b_fraction = b->fraction.ll;

        diff = a_normal_exp - b_normal_exp;
        if (diff < 0) {
            diff = -diff;
        }
        if (diff < FRAC_NBITS) {
            while (a_normal_exp > b_normal_exp) {
                b_normal_exp++;
                b_fraction = (b_fraction & 1) | (b_fraction >> 1);
            }
            while (b_normal_exp > a_normal_exp) {
                a_normal_exp++;
                a_fraction = (a_fraction & 1) | (a_fraction >> 1);
            }
        } else {
            if (a_normal_exp > b_normal_exp) {
                b_normal_exp = a_normal_exp;
                b_fraction = 0;
            } else {
                a_normal_exp = b_normal_exp;
                a_fraction = 0;
            }
        }
    }

    if (a->sign != b->sign) {
        if (a->sign) {
            tfraction = -a_fraction + b_fraction;
        } else {
            tfraction = a_fraction - b_fraction;
        }
        if (tfraction >= 0) {
            tmp->sign = 0;
            tmp->normal_exp = a_normal_exp;
            tmp->fraction.ll = tfraction;
        } else {
            tmp->sign = 1;
            tmp->normal_exp = a_normal_exp;
            tmp->fraction.ll = -tfraction;
        }
        while (tmp->fraction.ll < IMPLICIT_1 && tmp->fraction.ll) {
            tmp->fraction.ll <<= 1;
            tmp->normal_exp--;
        }
    } else {
        tmp->sign = a->sign;
        tmp->normal_exp = a_normal_exp;
        tmp->fraction.ll = a_fraction + b_fraction;
    }
    tmp->class = CLASS_NUMBER;
    if (tmp->fraction.ll >= IMPLICIT_2) {
        tmp->fraction.ll = (tmp->fraction.ll & 1) | (tmp->fraction.ll >> 1);
        tmp->normal_exp++;
    }
    return tmp;
}


FLO_type fpadd(FLO_type arg_a, FLO_type arg_b) {
    fp_number_type a;
    fp_number_type b;
    fp_number_type tmp;
    fp_number_type *res;

    __unpack_f((FLO_union_type *)&arg_a, &a);
    __unpack_f((FLO_union_type *)&arg_b, &b);
    res = _fpadd_parts(&a, &b, &tmp);
    return __pack_f(res);
}


FLO_type fpsub(FLO_type arg_a, FLO_type arg_b) {
    fp_number_type a;
    fp_number_type b;
    fp_number_type tmp;
    fp_number_type *res;

    __unpack_f((FLO_union_type *)&arg_a, &a);
    __unpack_f((FLO_union_type *)&arg_b, &b);
    b.sign ^= 1;
    res = _fpadd_parts(&a, &b, &tmp);
    return __pack_f(res);
}


static inline fp_number_type *_fpmul_parts(fp_number_type *a, fp_number_type *b, fp_number_type *tmp) {
    fractype low = 0;
    fractype high = 0;

    if (isnan(a)) {
        a->sign = a->sign != b->sign;
        return a;
    }
    if (isnan(b)) {
        b->sign = a->sign != b->sign;
        return b;
    }
    if (isinf(a)) {
        if (iszero(b)) {
            return nan();
        }
        a->sign = a->sign != b->sign;
        return a;
    }
    if (isinf(b)) {
        if (iszero(a)) {
            return nan();
        }
        b->sign = a->sign != b->sign;
        return b;
    }
    if (iszero(a)) {
        a->sign = a->sign != b->sign;
        return a;
    }
    if (iszero(b)) {
        b->sign = a->sign != b->sign;
        return b;
    }

    {
        long long answer = (long long)a->fraction.ll * (long long)b->fraction.ll;

        high = answer >> 32;
        low = answer;
    }

    tmp->normal_exp = a->normal_exp + b->normal_exp;
    tmp->sign = a->sign != b->sign;
    tmp->normal_exp += 2;
    while (high >= IMPLICIT_2) {
        tmp->normal_exp++;
        if (high & 1) {
            low >>= 1;
            low |= FRACHIGH;
        }
        high >>= 1;
    }
    while (high < IMPLICIT_1) {
        tmp->normal_exp--;
        high <<= 1;
        if (low & FRACHIGH) {
            high |= 1;
        }
        low <<= 1;
    }
    if ((high & GARDMASK) == GARDMSB) {
        if (high & (1 << NGARDS)) {
            high += GARDROUND + 1;
        } else if (low) {
            high += GARDROUND + 1;
        }
    }
    tmp->fraction.ll = high;
    tmp->class = CLASS_NUMBER;
    return tmp;
}


FLO_type fpmul(FLO_type arg_a, FLO_type arg_b) {
    fp_number_type a;
    fp_number_type b;
    fp_number_type tmp;
    fp_number_type *res;

    __unpack_f((FLO_union_type *)&arg_a, &a);
    __unpack_f((FLO_union_type *)&arg_b, &b);
    res = _fpmul_parts(&a, &b, &tmp);
    return __pack_f(res);
}


static inline fp_number_type *_fpdiv_parts(fp_number_type *a, fp_number_type *b) {
    fractype bit;
    fractype numerator;
    fractype denominator;
    fractype quotient;

    if (isnan(a)) {
        return a;
    }
    if (isnan(b)) {
        return b;
    }

    a->sign = a->sign ^ b->sign;

    if (isinf(a) || iszero(a)) {
        if (a->class == b->class) {
            return nan();
        }
        return a;
    }
    if (isinf(b)) {
        a->fraction.ll = 0;
        a->normal_exp = 0;
        return a;
    }
    if (iszero(b)) {
        a->class = CLASS_INFINITY;
        return a;
    }

    a->normal_exp = a->normal_exp - b->normal_exp;
    numerator = a->fraction.ll;
    denominator = b->fraction.ll;

    if (numerator < denominator) {
        numerator *= 2;
        a->normal_exp--;
    }
    bit = IMPLICIT_1;
    quotient = 0;
    while (bit) {
        if (numerator >= denominator) {
            quotient |= bit;
            numerator -= denominator;
        }
        bit >>= 1;
        numerator *= 2;
    }

    if ((quotient & GARDMASK) == GARDMSB) {
        if (quotient & (1 << NGARDS)) {
            quotient += GARDROUND + 1;
        } else if (numerator) {
            quotient += GARDROUND + 1;
        }
    }

    a->fraction.ll = quotient;
    return a;
}


FLO_type fpdiv(FLO_type arg_a, FLO_type arg_b) {
    fp_number_type a;
    fp_number_type b;
    fp_number_type *res;

    __unpack_f((FLO_union_type *)&arg_a, &a);
    __unpack_f((FLO_union_type *)&arg_b, &b);
    res = _fpdiv_parts(&a, &b);
    return __pack_f(res);
}


int __fpcmp_parts_f(fp_number_type *a, fp_number_type *b) {
    if (isnan(a) || isnan(b)) {
        return 1;
    }
    if (isinf(a) && isinf(b)) {
        return b->sign - a->sign;
    }
    if (isinf(a)) {
        return a->sign ? -1 : 1;
    }
    if (isinf(b)) {
        return b->sign ? 1 : -1;
    }
    if (iszero(a) && iszero(b)) {
        return 0;
    }
    if (iszero(a)) {
        return b->sign ? 1 : -1;
    }
    if (iszero(b)) {
        return a->sign ? -1 : 1;
    }
    if (a->sign != b->sign) {
        return a->sign ? -1 : 1;
    }
    if (a->normal_exp > b->normal_exp) {
        return a->sign ? -1 : 1;
    }
    if (a->normal_exp < b->normal_exp) {
        return a->sign ? 1 : -1;
    }
    if (a->fraction.ll > b->fraction.ll) {
        return a->sign ? -1 : 1;
    }
    if (a->fraction.ll < b->fraction.ll) {
        return a->sign ? 1 : -1;
    }
    return 0;
}


int fpcmp(FLO_type arg_a, FLO_type arg_b) {
    fp_number_type a;
    fp_number_type b;

    __unpack_f((FLO_union_type *)&arg_a, &a);
    __unpack_f((FLO_union_type *)&arg_b, &b);
    return __fpcmp_parts_f(&a, &b);
}


FLO_type sitofp(int arg_a) {
    fp_number_type in;

    in.class = CLASS_NUMBER;
    in.sign = arg_a < 0;
    if (!arg_a) {
        in.class = CLASS_ZERO;
    } else {
        in.normal_exp = FRACBITS + NGARDS;
        if (in.sign) {
            if (arg_a == (-0x7fffffff - 1)) {
                return (FLO_type)(-0x7fffffff - 1);
            }
            in.fraction.ll = -arg_a;
        } else {
            in.fraction.ll = arg_a;
        }
        while (in.fraction.ll < IMPLICIT_1) {
            in.fraction.ll <<= 1;
            in.normal_exp -= 1;
        }
    }
    return __pack_f(&in);
}


int fptosi(FLO_type arg_a) {
    fp_number_type a;
    int tmp;

    __unpack_f((FLO_union_type *)&arg_a, &a);
    if (iszero(&a)) {
        return 0;
    }
    if (isnan(&a)) {
        return 0;
    }
    if (isinf(&a)) {
        return a.sign ? (-0x7fffffff - 1) : 0x7fffffff;
    }
    if (a.normal_exp < 0) {
        return 0;
    }
    if (a.normal_exp > 30) {
        return a.sign ? (-0x7fffffff - 1) : 0x7fffffff;
    }
    tmp = a.fraction.ll >> ((FRACBITS + NGARDS) - a.normal_exp);
    return a.sign ? -tmp : tmp;
}


unsigned int fptoui(FLO_type arg_a) {
    fp_number_type a;

    __unpack_f((FLO_union_type *)&arg_a, &a);
    if (iszero(&a)) {
        return 0;
    }
    if (isnan(&a)) {
        return 0;
    }
    if (a.sign) {
        return 0;
    }
    if (isinf(&a)) {
        return 0xffffffff;
    }
    if (a.normal_exp < 0) {
        return 0;
    }
    if (a.normal_exp > 31) {
        return 0xffffffff;
    } else if (a.normal_exp > FRACBITS + NGARDS) {
        return a.fraction.ll << (a.normal_exp - (FRACBITS + NGARDS));
    } else {
        return a.fraction.ll >> ((FRACBITS + NGARDS) - a.normal_exp);
    }
}


FLO_type __negsf2(FLO_type arg_a) {
    fp_number_type a;

    __unpack_f((FLO_union_type *)&arg_a, &a);
    a.sign = !a.sign;
    return __pack_f(&a);
}


FLO_type __make_fp(fp_class_type class, unsigned int sign, int exp, fractype frac) {
    fp_number_type in;

    in.class = class;
    in.sign = sign;
    in.normal_exp = exp;
    in.fraction.ll = frac;
    return __pack_f(&in);
}


double fptodp(FLO_type arg_a) {
    fp_number_type in;

    __unpack_f((FLO_union_type *)&arg_a, &in);
    return __make_dp(in.class, in.sign, in.normal_exp, (unsigned long long)in.fraction.ll << F_D_BITOFF);
}
