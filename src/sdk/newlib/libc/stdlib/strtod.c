/*
 * Unit: sdk/newlib/libc/stdlib/strtod
 *
 * Functions:
 *   0x00272288 _strtod_r
 *   0x00273188 strtod
 *   0x002731b8 strtodf
 */

typedef unsigned int __ULong;
typedef int __Long;

#define ULong __ULong
#define Long __Long

struct _Bigint {
    struct _Bigint *_next;
    int _k, _maxwds, _sign, _wds;
    __ULong _x[1];
};

typedef struct _Bigint _Bigint;

#define Bigint _Bigint

struct _reent {
    int _errno;
    void *_stdin, *_stdout, *_stderr;
    int _inc;
    char _emergency[25];
    int _current_category;
    const char *_current_locale;
    int __sdidinit;
    void (*__cleanup)(struct _reent *);
    struct _Bigint *_result;
    int _result_k;
    struct _Bigint *_p5s;
    struct _Bigint **_freelist;
};

union double_union {
    double d;
    __ULong i[2];
};

union __dmath {
    __ULong i[2];
    double d;
};

#define word0(x) ((x).i[1])
#define word1(x) ((x).i[0])

#define ERANGE 34
#define DBL_DIG 15
#define DBL_MAX_EXP 1024
#define DBL_MAX_10_EXP 308
#define FLT_RADIX 2

#define Exp_msk1 0x100000
#define Exp_mask 0x7ff00000
#define P 53
#define Bias 1023
#define Emin (-1022)
#define Ten_pmax 22
#define Bndry_mask 0xfffff
#define Bndry_mask1 0xfffff
#define LSB 1
#define Log2P 1
#define Tiny0 0
#define Tiny1 1
#define Big0 0x7fefffff
#define Big1 0xffffffff
#define n_bigtens 5

#define Bcopy(x, y) memcpy((char *)&x->_sign, (char *)&y->_sign, y->_wds * sizeof(Long) + 2 * sizeof(int))

extern void *memcpy(void *dst, const void *src, unsigned int n);

extern Bigint *_Balloc(struct _reent *ptr, int k);
extern void _Bfree(struct _reent *ptr, Bigint *v);
extern Bigint *_s2b(struct _reent *ptr, const char *s, int nd0, int nd, ULong y9);
extern Bigint *_i2b(struct _reent *ptr, int i);
extern Bigint *_multiply(struct _reent *ptr, Bigint *a, Bigint *b);
extern Bigint *_pow5mult(struct _reent *ptr, Bigint *b, int k);
extern Bigint *_lshift(struct _reent *ptr, Bigint *b, int k);
extern int __mcmp(Bigint *a, Bigint *b);
extern Bigint *__mdiff(struct _reent *ptr, Bigint *a, Bigint *b);
extern double _ulp(double x);
extern Bigint *_d2b(struct _reent *ptr, double d, int *e, int *bits);
extern double _ratio(Bigint *a, Bigint *b);

extern const double __mprec_tens[];
extern const double __mprec_bigtens[];
extern const double __mprec_tinytens[];

extern const union __dmath __infinity[];
#define HUGE_VAL (__infinity[0].d)

extern struct _reent *_impure_ptr[];
#define _REENT (_impure_ptr[0])

#define Balloc _Balloc
#define Bfree _Bfree
#define s2b _s2b
#define i2b _i2b
#define mult _multiply
#define pow5mult _pow5mult
#define lshift _lshift
#define cmp __mcmp
#define diff __mdiff
#define ulp _ulp
#define d2b _d2b
#define ratio _ratio
#define tens __mprec_tens
#define bigtens __mprec_bigtens
#define tinytens __mprec_tinytens


double _strtod_r(struct _reent *ptr, const char *s00, char **se)
{
    int bb2, bb5, bbe, bd2, bd5, bbbits, bs2, c, dsign, e1, esign, i, j, k, nd, nd0, nf, nz, nz0, sign;
    long e;
    const char *s, *s0, *s1;
    double aadj, aadj1, adj;
    long L;
    unsigned long z;
    ULong y;
    union double_union rv, rv0;
    Bigint *bb, *bb1, *bd, *bd0, *bs, *delta;

    sign = nz0 = nz = 0;
    rv.d = 0.;
    for (s = s00;; s++) {
        switch (*s) {
        case '-':
            sign = 1;
        case '+':
            if (*++s == '\0') {
                goto ret0;
            }
        default:
            goto break2;
        case 0:
        ret0:
            s = s00;
            goto ret;
        case '\t':
        case '\n':
        case '\v':
        case '\f':
        case '\r':
        case ' ':
            continue;
        }
    }
break2:
    if (*s == '0') {
        nz0 = 1;
        while (*++s == '0') {
        }
        if (!*s) {
            goto ret;
        }
    }
    s0 = s;
    y = z = 0;
    for (nd = nf = 0; (c = *s) >= '0' && c <= '9'; nd++, s++) {
        if (nd < 9) {
            y = 10 * y + c - '0';
        } else if (nd < 16) {
            z = 10 * z + c - '0';
        }
    }
    nd0 = nd;
    if (c == '.') {
        c = *++s;
        if (!nd) {
            for (; c == '0'; c = *++s) {
                nz++;
            }
            if (c > '0' && c <= '9') {
                s0 = s;
                nf += nz;
                nz = 0;
                goto have_dig;
            }
            goto dig_done;
        }
        for (; c >= '0' && c <= '9'; c = *++s) {
        have_dig:
            nz++;
            if (c -= '0') {
                nf += nz;
                for (i = 1; i < nz; i++) {
                    if (nd++ < 9) {
                        y *= 10;
                    } else if (nd <= DBL_DIG + 1) {
                        z *= 10;
                    }
                }
                if (nd++ < 9) {
                    y = 10 * y + c;
                } else if (nd <= DBL_DIG + 1) {
                    z = 10 * z + c;
                }
                nz = 0;
            }
        }
    }
dig_done:
    e = 0;
    if (c == 'e' || c == 'E') {
        if (!nd && !nz && !nz0) {
            s = s00;
            goto ret;
        }
        s00 = s;
        esign = 0;
        switch (c = *++s) {
        case '-':
            esign = 1;
        case '+':
            c = *++s;
        }
        if (c >= '0' && c <= '9') {
            while (c == '0') {
                c = *++s;
            }
            if (c > '0' && c <= '9') {
                e = c - '0';
                s1 = s;
                while ((c = *++s) >= '0' && c <= '9') {
                    e = 10 * e + c - '0';
                }
                if (s - s1 > 8) {
                    e = 9999999L;
                }
                if (esign) {
                    e = -e;
                }
            } else {
                e = 0;
            }
        } else {
            s = s00;
        }
    }
    if (!nd) {
        if (!nz && !nz0) {
            s = s00;
        }
        goto ret;
    }
    e1 = e -= nf;

    if (!nd0) {
        nd0 = nd;
    }
    k = nd < DBL_DIG + 1 ? nd : DBL_DIG + 1;
    rv.d = y;
    if (k > 9) {
        rv.d = tens[k - 9] * rv.d + z;
    }
    bd0 = 0;
    if (nd <= DBL_DIG) {
        if (!e) {
            goto ret;
        }
        if (e > 0) {
            if (e <= Ten_pmax) {
                rv.d *= tens[e];
                goto ret;
            }
            i = DBL_DIG - nd;
            if (e <= Ten_pmax + i) {
                e -= i;
                rv.d *= tens[i];
                rv.d *= tens[e];
                goto ret;
            }
        } else if (e >= -Ten_pmax) {
            rv.d /= tens[-e];
            goto ret;
        }
    }
    e1 += nd - k;

    if (e1 > 0) {
        if ((i = e1 & 15) != 0) {
            rv.d *= tens[i];
        }
        if (e1 &= ~15) {
            if (e1 > DBL_MAX_10_EXP) {
            ovfl:
                ptr->_errno = ERANGE;
                rv.d = HUGE_VAL;
                if (bd0) {
                    goto retfree;
                }
                goto ret;
            }
            if ((e1 >>= 4) != 0) {
                for (j = 0; e1 > 1; j++, e1 >>= 1) {
                    if (e1 & 1) {
                        rv.d *= bigtens[j];
                    }
                }
                word0(rv) -= P * Exp_msk1;
                rv.d *= bigtens[j];
                if ((z = word0(rv) & Exp_mask) > Exp_msk1 * (DBL_MAX_EXP + Bias - P)) {
                    goto ovfl;
                }
                if (z > Exp_msk1 * (DBL_MAX_EXP + Bias - 1 - P)) {
                    word0(rv) = Big0;
                    word1(rv) = Big1;
                } else {
                    word0(rv) += P * Exp_msk1;
                }
            }
        }
    } else if (e1 < 0) {
        e1 = -e1;
        if ((i = e1 & 15) != 0) {
            rv.d /= tens[i];
        }
        if (e1 &= ~15) {
            e1 >>= 4;
            if (e1 >= 1 << n_bigtens) {
                goto undfl;
            }
            for (j = 0; e1 > 1; j++, e1 >>= 1) {
                if (e1 & 1) {
                    rv.d *= tinytens[j];
                }
            }
            rv0.d = rv.d;
            rv.d *= tinytens[j];
            if (!rv.d) {
                rv.d = 2. * rv0.d;
                rv.d *= tinytens[j];
                if (!rv.d) {
                undfl:
                    rv.d = 0.;
                    ptr->_errno = ERANGE;
                    if (bd0) {
                        goto retfree;
                    }
                    goto ret;
                }
                word0(rv) = Tiny0;
                word1(rv) = Tiny1;
            }
        }
    }

    bd0 = s2b(ptr, s0, nd0, nd, y);

    for (;;) {
        bd = Balloc(ptr, bd0->_k);
        Bcopy(bd, bd0);
        bb = d2b(ptr, rv.d, &bbe, &bbbits);
        bs = i2b(ptr, 1);

        if (e >= 0) {
            bb2 = bb5 = 0;
            bd2 = bd5 = e;
        } else {
            bb2 = bb5 = -e;
            bd2 = bd5 = 0;
        }
        if (bbe >= 0) {
            bb2 += bbe;
        } else {
            bd2 -= bbe;
        }
        bs2 = bb2;
        i = bbe + bbbits - 1;
        if (i < Emin) {
            j = bbe + (P - Emin);
        } else {
            j = P + 1 - bbbits;
        }
        bb2 += j;
        bd2 += j;
        i = bb2 < bd2 ? bb2 : bd2;
        if (i > bs2) {
            i = bs2;
        }
        if (i > 0) {
            bb2 -= i;
            bd2 -= i;
            bs2 -= i;
        }
        if (bb5 > 0) {
            bs = pow5mult(ptr, bs, bb5);
            bb1 = mult(ptr, bs, bb);
            Bfree(ptr, bb);
            bb = bb1;
        }
        if (bb2 > 0) {
            bb = lshift(ptr, bb, bb2);
        }
        if (bd5 > 0) {
            bd = pow5mult(ptr, bd, bd5);
        }
        if (bd2 > 0) {
            bd = lshift(ptr, bd, bd2);
        }
        if (bs2 > 0) {
            bs = lshift(ptr, bs, bs2);
        }
        delta = diff(ptr, bb, bd);
        dsign = delta->_sign;
        delta->_sign = 0;
        i = cmp(delta, bs);
        if (i < 0) {
            if (dsign || word1(rv) || word0(rv) & Bndry_mask) {
                break;
            }
            delta = lshift(ptr, delta, Log2P);
            if (cmp(delta, bs) > 0) {
                goto drop_down;
            }
            break;
        }
        if (i == 0) {
            if (dsign) {
                if ((word0(rv) & Bndry_mask1) == Bndry_mask1 && word1(rv) == 0xffffffff) {
                    word0(rv) = (word0(rv) & Exp_mask) + Exp_msk1;
                    word1(rv) = 0;
                    break;
                }
            } else if (!(word0(rv) & Bndry_mask) && !word1(rv)) {
            drop_down:
                L = (word0(rv) & Exp_mask) - Exp_msk1;
                word0(rv) = L | Bndry_mask1;
                word1(rv) = 0xffffffff;
                break;
            }
            if (!(word1(rv) & LSB)) {
                break;
            }
            if (dsign) {
                rv.d += ulp(rv.d);
            } else {
                rv.d -= ulp(rv.d);
                if (!rv.d) {
                    goto undfl;
                }
            }
            break;
        }
        if ((aadj = ratio(delta, bs)) <= 2.) {
            if (dsign) {
                aadj = aadj1 = 1.;
            } else if (word1(rv) || word0(rv) & Bndry_mask) {
                if (word1(rv) == Tiny1 && !word0(rv)) {
                    goto undfl;
                }
                aadj = 1.;
                aadj1 = -1.;
            } else {
                if (aadj < 2. / FLT_RADIX) {
                    aadj = 1. / FLT_RADIX;
                } else {
                    aadj *= 0.5;
                }
                aadj1 = -aadj;
            }
        } else {
            aadj *= 0.5;
            aadj1 = dsign ? aadj : -aadj;
        }
        y = word0(rv) & Exp_mask;

        if (y == Exp_msk1 * (DBL_MAX_EXP + Bias - 1)) {
            rv0.d = rv.d;
            word0(rv) -= P * Exp_msk1;
            adj = aadj1 * ulp(rv.d);
            rv.d += adj;
            if ((word0(rv) & Exp_mask) >= Exp_msk1 * (DBL_MAX_EXP + Bias - P)) {
                if (word0(rv0) == Big0 && word1(rv0) == Big1) {
                    goto ovfl;
                }
                word0(rv) = Big0;
                word1(rv) = Big1;
                goto cont;
            } else {
                word0(rv) += P * Exp_msk1;
            }
        } else {
            if (y <= (P - 1) * Exp_msk1 && aadj >= 1.) {
                aadj1 = (double)(int)(aadj + 0.5);
                if (!dsign) {
                    aadj1 = -aadj1;
                }
            }
            adj = aadj1 * ulp(rv.d);
            rv.d += adj;
        }
        z = word0(rv) & Exp_mask;
        if (y == z) {
            L = aadj;
            aadj -= L;
            if (dsign || word1(rv) || word0(rv) & Bndry_mask) {
                if (aadj < .4999999 || aadj > .5000001) {
                    break;
                }
            } else if (aadj < .4999999 / FLT_RADIX) {
                break;
            }
        }
    cont:
        Bfree(ptr, bb);
        Bfree(ptr, bd);
        Bfree(ptr, bs);
        Bfree(ptr, delta);
    }
retfree:
    Bfree(ptr, bb);
    Bfree(ptr, bd);
    Bfree(ptr, bs);
    Bfree(ptr, bd0);
    Bfree(ptr, delta);
ret:
    if (se) {
        *se = (char *)s;
    }
    return sign ? -rv.d : rv.d;
}


double strtod(const char *s00, char **se)
{
    return _strtod_r(_REENT, s00, se);
}


float strtodf(const char *s00, char **se)
{
    return (float)strtod(s00, se);
}
