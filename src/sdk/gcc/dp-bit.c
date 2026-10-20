/*
 * Unit: sdk/gcc/dp-bit
 *
 * Functions:
 *   0x0026ea50 __pack_d
 *   0x0026eb80 __unpack_d
 *   0x0026ec20 _fpadd_parts
 *   0x0026ee60 dpadd
 *   0x0026eeb8 dpsub
 *   0x0026ef20 dpmul
 *   0x0026f1c8 dpdiv
 *   0x0026f330 __fpcmp_parts_d
 *   0x0026f448 dpcmp
 *   0x0026f498 litodp
 *   0x0026f550 dptoli
 *   0x0026f5e8 dptoul
 *   0x0026f688 __negdf2
 *   0x0026f6c0 __make_dp
 *   0x0026f6f0 dptofp
 */

#define FRACBITS      52
#define EXPBITS       11
#define EXPBIAS       1023
#define EXPMAX        0x7FF
#define NGARDS        8
#define GARDROUND     0x7F
#define GARDMASK      0xFF
#define GARDMSB       0x80
#define FRAC_NBITS    64
#define NORMAL_EXPMIN (-EXPBIAS + 1)
#define QUIET_NAN     0x8000000000000
#define IMPLICIT_1    (1L << (FRACBITS + NGARDS))
#define IMPLICIT_2    (1L << (FRACBITS + 1 + NGARDS))
#define FRACHIGH      0x8000000000000000L
#define F_D_BITOFF    30
#define BITS_PER_SI   32
#define MAX_SI_INT    0x7FFFFFFF
#define MAX_USI_INT   0xFFFFFFFF
#define NO_DENORMALS  1

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
        unsigned long ll;
        unsigned int l[2];
    } fraction;
} fp_number_type;

typedef union {
    double value;
    struct {
        unsigned long fraction : FRACBITS;
        unsigned int exp : EXPBITS;
        unsigned int sign : 1;
    } bits;
} FLO_union_type;

extern float __make_fp(fp_class_type class, unsigned int sign, int exp, unsigned int frac);


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


double __pack_d(fp_number_type *src) {
    FLO_union_type dst;
    unsigned long fraction = src->fraction.ll;
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


void __unpack_d(FLO_union_type *src, fp_number_type *dst) {
    unsigned long fraction = src->bits.fraction;
    int exp = src->bits.exp;
    int sign = src->bits.sign;

    dst->sign = sign;
    if (exp == 0) {
        if (fraction == 0 || NO_DENORMALS) {
            dst->class = CLASS_ZERO;
        } else {
            dst->normal_exp = exp - EXPBIAS + 1;
            fraction <<= NGARDS;
            dst->class = CLASS_NUMBER;
            while (fraction < IMPLICIT_1) {
                fraction <<= 1;
                dst->normal_exp--;
            }
            dst->fraction.ll = fraction;
        }
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
    long tfraction;
    int a_normal_exp;
    int b_normal_exp;
    unsigned long a_fraction;
    unsigned long b_fraction;
    int diff;

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


double dpadd(double arg_a, double arg_b) {
    fp_number_type a;
    fp_number_type b;
    fp_number_type tmp;
    fp_number_type *res;
    FLO_union_type au, bu;

    au.value = arg_a;
    bu.value = arg_b;
    __unpack_d(&au, &a);
    __unpack_d(&bu, &b);
    res = _fpadd_parts(&a, &b, &tmp);
    return __pack_d(res);
}


double dpsub(double arg_a, double arg_b) {
    fp_number_type a;
    fp_number_type b;
    fp_number_type tmp;
    fp_number_type *res;
    FLO_union_type au, bu;

    au.value = arg_a;
    bu.value = arg_b;
    __unpack_d(&au, &a);
    __unpack_d(&bu, &b);
    b.sign ^= 1;
    res = _fpadd_parts(&a, &b, &tmp);
    return __pack_d(res);
}


static inline fp_number_type *_fpmul_parts(fp_number_type *a, fp_number_type *b, fp_number_type *tmp) {
    unsigned long low = 0;
    unsigned long high = 0;

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
        unsigned long nl = a->fraction.ll & 0xFFFFFFFF;
        unsigned long nh = a->fraction.ll >> 32;
        unsigned long ml = b->fraction.ll & 0xFFFFFFFF;
        unsigned long mh = b->fraction.ll >> 32;
        unsigned long pp_ll = ml * nl;
        unsigned long pp_hl = mh * nl;
        unsigned long pp_lh = ml * nh;
        unsigned long pp_hh = mh * nh;
        unsigned long res2 = 0;
        unsigned long res0 = 0;
        unsigned long ps_hh = pp_hl + pp_lh;

        if (ps_hh < pp_hl) {
            res2 += 0x100000000L;
        }
        pp_hl = (ps_hh << 32) & 0xFFFFFFFF00000000L;
        res0 = pp_ll + pp_hl;
        if (res0 < pp_ll) {
            res2++;
        }
        res2 += ((ps_hh >> 32) & 0xFFFFFFFFL) + pp_hh;
        high = res2;
        low = res0;
    }

    tmp->normal_exp = a->normal_exp + b->normal_exp;
    tmp->sign = a->sign != b->sign;
    tmp->normal_exp += 4;
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


double dpmul(double arg_a, double arg_b) {
    fp_number_type a;
    fp_number_type b;
    fp_number_type tmp;
    fp_number_type *res;
    FLO_union_type au, bu;

    au.value = arg_a;
    bu.value = arg_b;
    __unpack_d(&au, &a);
    __unpack_d(&bu, &b);
    res = _fpmul_parts(&a, &b, &tmp);
    return __pack_d(res);
}


static inline fp_number_type *_fpdiv_parts(fp_number_type *a, fp_number_type *b) {
    unsigned long bit;
    unsigned long numerator;
    unsigned long denominator;
    unsigned long quotient;

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


double dpdiv(double arg_a, double arg_b) {
    fp_number_type a;
    fp_number_type b;
    fp_number_type *res;
    FLO_union_type au, bu;

    au.value = arg_a;
    bu.value = arg_b;
    __unpack_d(&au, &a);
    __unpack_d(&bu, &b);
    res = _fpdiv_parts(&a, &b);
    return __pack_d(res);
}


int __fpcmp_parts_d(fp_number_type *a, fp_number_type *b) {
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


int dpcmp(double arg_a, double arg_b) {
    fp_number_type a;
    fp_number_type b;
    FLO_union_type au, bu;

    au.value = arg_a;
    bu.value = arg_b;
    __unpack_d(&au, &a);
    __unpack_d(&bu, &b);
    return __fpcmp_parts_d(&a, &b);
}


double litodp(int arg_a) {
    fp_number_type in;

    in.class = CLASS_NUMBER;
    in.sign = arg_a < 0;
    if (!arg_a) {
        in.class = CLASS_ZERO;
    } else {
        in.normal_exp = FRACBITS + NGARDS;
        if (in.sign) {
            if (arg_a == -MAX_SI_INT - 1) {
                return -MAX_SI_INT - 1;
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
    return __pack_d(&in);
}


int dptoli(double arg_a) {
    fp_number_type a;
    int tmp;
    FLO_union_type au;

    au.value = arg_a;
    __unpack_d(&au, &a);
    if (iszero(&a)) {
        return 0;
    }
    if (isnan(&a)) {
        return 0;
    }
    if (isinf(&a)) {
        return a.sign ? -MAX_SI_INT - 1 : MAX_SI_INT;
    }
    if (a.normal_exp < 0) {
        return 0;
    }
    if (a.normal_exp > BITS_PER_SI - 2) {
        return a.sign ? -MAX_SI_INT - 1 : MAX_SI_INT;
    }
    tmp = a.fraction.ll >> (FRACBITS + NGARDS - a.normal_exp);
    return a.sign ? -tmp : tmp;
}


unsigned int dptoul(double arg_a) {
    fp_number_type a;
    FLO_union_type au;

    au.value = arg_a;
    __unpack_d(&au, &a);
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
        return MAX_USI_INT;
    }
    if (a.normal_exp < 0) {
        return 0;
    }
    if (a.normal_exp > BITS_PER_SI - 1) {
        return MAX_USI_INT;
    } else if (a.normal_exp > FRACBITS + NGARDS) {
        return a.fraction.ll << (a.normal_exp - (FRACBITS + NGARDS));
    } else {
        return a.fraction.ll >> (FRACBITS + NGARDS - a.normal_exp);
    }
}


double __negdf2(double arg_a) {
    fp_number_type a;
    FLO_union_type au;

    au.value = arg_a;
    __unpack_d(&au, &a);
    a.sign = !a.sign;
    return __pack_d(&a);
}


double __make_dp(fp_class_type class, unsigned int sign, int exp, unsigned long frac) {
    fp_number_type in;

    in.class = class;
    in.sign = sign;
    in.normal_exp = exp;
    in.fraction.ll = frac;
    return __pack_d(&in);
}


float dptofp(double arg_a) {
    fp_number_type in;
    unsigned int sffrac;
    FLO_union_type au;

    au.value = arg_a;
    __unpack_d(&au, &in);
    sffrac = in.fraction.ll >> F_D_BITOFF;
    if ((in.fraction.ll & ((1 << F_D_BITOFF) - 1)) != 0) {
        sffrac |= 1;
    }
    return __make_fp(in.class, in.sign, in.normal_exp, sffrac);
}
