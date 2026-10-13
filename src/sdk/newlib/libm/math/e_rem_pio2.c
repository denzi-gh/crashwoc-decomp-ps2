/*
 * Unit: sdk/newlib/libm/math/e_rem_pio2
 *
 * Functions:
 *   0x0026ab10 __ieee754_rem_pio2
 */

typedef int __int32_t;
typedef unsigned int __uint32_t;

typedef union {
    double value;
    struct {
        __uint32_t lsw;
        __uint32_t msw;
    } parts;
} ieee_double_shape_type;

#define GET_HIGH_WORD(i, d) do { ieee_double_shape_type gh_u; gh_u.value = (d); (i) = gh_u.parts.msw; } while (0)
#define GET_LOW_WORD(i, d) do { ieee_double_shape_type gl_u; gl_u.value = (d); (i) = gl_u.parts.lsw; } while (0)
#define SET_HIGH_WORD(d, v) do { ieee_double_shape_type sh_u; sh_u.value = (d); sh_u.parts.msw = (v); (d) = sh_u.value; } while (0)
#define SET_LOW_WORD(d, v) do { ieee_double_shape_type sl_u; sl_u.value = (d); sl_u.parts.lsw = (v); (d) = sl_u.value; } while (0)

extern double fabs(double x, ...);
extern int __kernel_rem_pio2(double *x, double *y, int e0, int nx, int prec, const __int32_t *ipio2);

extern const __int32_t D_0062B700[];
#define two_over_pi D_0062B700
extern const __int32_t D_0062B808[];
#define npio2_hw D_0062B808

#define zero 0.00000000000000000000e+00
#define half 5.00000000000000000000e-01
#define two24 1.67772160000000000000e+07
#define invpio2 6.36619772367581382433e-01
#define pio2_1 1.57079632673412561417e+00
#define pio2_1t 6.07710050650619224932e-11
#define pio2_2 6.07710050630396597660e-11
#define pio2_2t 2.02226624879595063154e-21
#define pio2_3 2.02226624871116645580e-21
#define pio2_3t 8.47842766036889956997e-32

__int32_t __ieee754_rem_pio2(double x, double *y)
{
    double z, w, t, r, fn;
    double tx[3];
    __int32_t i, j, n, ix, hx;
    int e0, nx;
    __uint32_t low;

    GET_HIGH_WORD(hx, x);
    ix = hx & 0x7fffffff;
    if (ix <= 0x3fe921fb) {
        y[0] = x;
        y[1] = 0;
        return 0;
    }
    if (ix < 0x4002d97c) {
        if (hx > 0) {
            z = x - pio2_1;
            if (ix != 0x3ff921fb) {
                y[0] = z - pio2_1t;
                y[1] = (z - y[0]) - pio2_1t;
            } else {
                z -= pio2_2;
                y[0] = z - pio2_2t;
                y[1] = (z - y[0]) - pio2_2t;
            }
            return 1;
        } else {
            z = x + pio2_1;
            if (ix != 0x3ff921fb) {
                y[0] = z + pio2_1t;
                y[1] = (z - y[0]) + pio2_1t;
            } else {
                z += pio2_2;
                y[0] = z + pio2_2t;
                y[1] = (z - y[0]) + pio2_2t;
            }
            return -1;
        }
    }
    if (ix <= 0x413921fb) {
        t = fabs(x);
        n = (__int32_t)(t * invpio2 + half);
        fn = (double)n;
        r = t - fn * pio2_1;
        w = fn * pio2_1t;
        if (n < 32 && ix != npio2_hw[n - 1]) {
            y[0] = r - w;
        } else {
            __uint32_t high;
            j = ix >> 20;
            y[0] = r - w;
            GET_HIGH_WORD(high, y[0]);
            i = j - ((high >> 20) & 0x7ff);
            if (i > 16) {
                t = r;
                w = fn * pio2_2;
                r = t - w;
                w = fn * pio2_2t - ((t - r) - w);
                y[0] = r - w;
                GET_HIGH_WORD(high, y[0]);
                i = j - ((high >> 20) & 0x7ff);
                if (i > 49) {
                    t = r;
                    w = fn * pio2_3;
                    r = t - w;
                    w = fn * pio2_3t - ((t - r) - w);
                    y[0] = r - w;
                }
            }
        }
        y[1] = (r - y[0]) - w;
        if (hx < 0) {
            y[0] = -y[0];
            y[1] = -y[1];
            return -n;
        } else {
            return n;
        }
    }
    if (ix >= 0x7ff00000) {
        y[0] = y[1] = x - x;
        return 0;
    }
    GET_LOW_WORD(low, x);
    SET_LOW_WORD(z, low);
    e0 = (int)((ix >> 20) - 1046);
    SET_HIGH_WORD(z, ix - ((__int32_t)(e0 << 20)));
    for (i = 0; i < 2; i++) {
        tx[i] = (double)((__int32_t)(z));
        z = (z - tx[i]) * two24;
    }
    tx[2] = z;
    nx = 3;
    while (tx[nx - 1] == zero) {
        nx--;
    }
    n = __kernel_rem_pio2(tx, y, e0, nx, 2, two_over_pi);
    if (hx < 0) {
        y[0] = -y[0];
        y[1] = -y[1];
        return -n;
    }
    return n;
}
