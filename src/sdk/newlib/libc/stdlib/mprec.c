/*
 * Unit: sdk/newlib/libc/stdlib/mprec
 *
 * Functions:
 *   0x002773d8 _Balloc
 *   0x00277480 _Bfree
 *   0x002774b0 _multadd
 *   0x002775c8 _s2b
 *   0x00277710 _hi0bits
 *   0x00277798 _lo0bits
 *   0x00277858 _i2b
 *   0x00277890 _multiply
 *   0x00277ac0 _pow5mult
 *   0x00277bc0 _lshift
 *   0x00277d38 __mcmp
 *   0x00277da0 __mdiff
 *   0x00277f30 _ulp
 *   0x00277fc8 _b2d
 *   0x00278148 _d2b
 *   0x002782c8 _ratio
 *   0x00278388 _mprec_log10
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

#define word0(x) ((x).i[1])
#define word1(x) ((x).i[0])

#define Exp_shift 20
#define Exp_msk1 0x100000
#define Exp_mask 0x7ff00000
#define P 53
#define _Kmax 15

#define Bcopy(x, y) memcpy((char *)&x->_sign, (char *)&y->_sign, y->_wds * sizeof(Long) + 2 * sizeof(int))

extern void *memcpy(void *dst, const void *src, unsigned int n);
extern void *_calloc_r(struct _reent *ptr, unsigned int nmemb, unsigned int size);

Bigint *_multiply(struct _reent *ptr, Bigint *a, Bigint *b);
double _b2d(Bigint *a, int *e);

extern const int D_0062C450[];
extern const double __mprec_tens[];

#define p05 D_0062C450
#define tens __mprec_tens

#define Balloc _Balloc
#define Bfree _Bfree
#define multadd _multadd
#define i2b _i2b
#define mult _multiply
#define b2d _b2d


Bigint *_Balloc(struct _reent *ptr, int k)
{
    int x;
    Bigint *rv;

    if (ptr->_freelist == 0) {
        ptr->_freelist = (Bigint **)_calloc_r(ptr, sizeof(Bigint *), _Kmax + 1);
        if (ptr->_freelist == 0) {
            return 0;
        }
    }

    if ((rv = ptr->_freelist[k]) != 0) {
        ptr->_freelist[k] = rv->_next;
    } else {
        x = 1 << k;
        rv = (Bigint *)_calloc_r(ptr, 1, sizeof(Bigint) + (x - 1) * sizeof(rv->_x));
        if (rv == 0) {
            return 0;
        }
        rv->_k = k;
        rv->_maxwds = x;
    }
    rv->_sign = rv->_wds = 0;
    return rv;
}


void _Bfree(struct _reent *ptr, Bigint *v)
{
    if (v) {
        v->_next = ptr->_freelist[v->_k];
        ptr->_freelist[v->_k] = v;
    }
}


Bigint *_multadd(struct _reent *ptr, Bigint *b, int m, int a)
{
    int i, wds;
    ULong *x, y;
    ULong xi, z;
    Bigint *b1;

    wds = b->_wds;
    x = b->_x;
    i = 0;
    do {
        xi = *x;
        y = (xi & 0xffff) * m + a;
        z = (xi >> 16) * m + (y >> 16);
        a = (int)(z >> 16);
        *x++ = (z << 16) + (y & 0xffff);
    } while (++i < wds);
    if (a) {
        if (wds >= b->_maxwds) {
            b1 = Balloc(ptr, b->_k + 1);
            Bcopy(b1, b);
            Bfree(ptr, b);
            b = b1;
        }
        b->_x[wds++] = a;
        b->_wds = wds;
    }
    return b;
}


Bigint *_s2b(struct _reent *ptr, const char *s, int nd0, int nd, ULong y9)
{
    Bigint *b;
    int i, k;
    Long x, y;

    x = (nd + 8) / 9;
    for (k = 0, y = 1; x > y; y <<= 1, k++) {
    }
    b = Balloc(ptr, k);
    b->_x[0] = y9;
    b->_wds = 1;

    i = 9;
    if (9 < nd0) {
        s += 9;
        do {
            b = multadd(ptr, b, 10, *s++ - '0');
        } while (++i < nd0);
        s++;
    } else {
        s += 10;
    }
    for (; i < nd; i++) {
        b = multadd(ptr, b, 10, *s++ - '0');
    }
    return b;
}


int _hi0bits(ULong x)
{
    int k = 0;

    if (!(x & 0xffff0000)) {
        k = 16;
        x <<= 16;
    }
    if (!(x & 0xff000000)) {
        k += 8;
        x <<= 8;
    }
    if (!(x & 0xf0000000)) {
        k += 4;
        x <<= 4;
    }
    if (!(x & 0xc0000000)) {
        k += 2;
        x <<= 2;
    }
    if (!(x & 0x80000000)) {
        k++;
        if (!(x & 0x40000000)) {
            return 32;
        }
    }
    return k;
}


int _lo0bits(ULong *y)
{
    int k;
    ULong x = *y;

    if (x & 7) {
        if (x & 1) {
            return 0;
        }
        if (x & 2) {
            *y = x >> 1;
            return 1;
        }
        *y = x >> 2;
        return 2;
    }
    k = 0;
    if (!(x & 0xffff)) {
        k = 16;
        x >>= 16;
    }
    if (!(x & 0xff)) {
        k += 8;
        x >>= 8;
    }
    if (!(x & 0xf)) {
        k += 4;
        x >>= 4;
    }
    if (!(x & 0x3)) {
        k += 2;
        x >>= 2;
    }
    if (!(x & 1)) {
        k++;
        x >>= 1;
        if (!x) {
            return 32;
        }
    }
    *y = x;
    return k;
}


Bigint *_i2b(struct _reent *ptr, int i)
{
    Bigint *b;

    b = Balloc(ptr, 1);
    b->_x[0] = i;
    b->_wds = 1;
    return b;
}


Bigint *_pow5mult(struct _reent *ptr, Bigint *b, int k)
{
    Bigint *b1, *p5, *p51;
    int i;

    if ((i = k & 3) != 0) {
        b = multadd(ptr, b, p05[i - 1], 0);
    }

    if (!(k >>= 2)) {
        return b;
    }
    if (!(p5 = ptr->_p5s)) {
        p5 = ptr->_p5s = i2b(ptr, 625);
        p5->_next = 0;
    }
    for (;;) {
        if (k & 1) {
            b1 = mult(ptr, b, p5);
            Bfree(ptr, b);
            b = b1;
        }
        if (!(k >>= 1)) {
            break;
        }
        if (!(p51 = p5->_next)) {
            p51 = p5->_next = mult(ptr, p5, p5);
            p51->_next = 0;
        }
        p5 = p51;
    }
    return b;
}


int __mcmp(Bigint *a, Bigint *b)
{
    ULong *xa, *xa0, *xb, *xb0;
    int i, j;

    i = a->_wds;
    j = b->_wds;
    if (i -= j) {
        return i;
    }
    xa0 = a->_x;
    xa = xa0 + j;
    xb0 = b->_x;
    xb = xb0 + j;
    for (;;) {
        if (*--xa != *--xb) {
            return *xa < *xb ? -1 : 1;
        }
        if (xa <= xa0) {
            break;
        }
    }
    return 0;
}


double _ulp(double _x)
{
    union double_union x, a;
    Long L;

    x.d = _x;

    L = (word0(x) & Exp_mask) - (P - 1) * Exp_msk1;
    if (L > 0) {
        word0(a) = L;
        word1(a) = 0;
    } else {
        L = -L >> Exp_shift;
        if (L < Exp_shift) {
            word0(a) = 0x80000 >> L;
            word1(a) = 0;
        } else {
            word0(a) = 0;
            L -= Exp_shift;
            word1(a) = L >= 31 ? 1 : 1 << (31 - L);
        }
    }
    return a.d;
}


double _ratio(Bigint *a, Bigint *b)
{
    union double_union da, db;
    int k, ka, kb;

    da.d = b2d(a, &ka);
    db.d = b2d(b, &kb);
    k = ka - kb + 32 * (a->_wds - b->_wds);
    if (k > 0) {
        word0(da) += k * Exp_msk1;
    } else {
        k = -k;
        word0(db) += k * Exp_msk1;
    }
    return da.d / db.d;
}


double _mprec_log10(int dig)
{
    double v = 1.0;

    if (dig < 24) {
        return tens[dig];
    }
    while (dig > 0) {
        v *= 10;
        dig--;
    }
    return v;
}
