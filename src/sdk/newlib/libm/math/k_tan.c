/*
 * Unit: sdk/newlib/libm/math/k_tan
 *
 * Functions:
 *   0x0026c2c8 __kernel_tan
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
#define SET_LOW_WORD(d, v) do { ieee_double_shape_type sl_u; sl_u.value = (d); sl_u.parts.lsw = (v); (d) = sl_u.value; } while (0)

extern double fabs(double x, ...);

extern const double D_0062BAC0[];
#define T D_0062BAC0

#define one 1.00000000000000000000e+00
#define pio4 7.85398163397448278999e-01
#define pio4lo 3.06161699786838301793e-17

double __kernel_tan(double x, double y, int iy)
{
    double z, r, v, w, s;
    __int32_t ix, hx;

    GET_HIGH_WORD(hx, x);
    ix = hx & 0x7fffffff;
    if (ix < 0x3e300000) {
        if ((int)x == 0) {
            __uint32_t low;
            GET_LOW_WORD(low, x);
            if (((ix | low) | (iy + 1)) == 0) {
                return one / fabs(x);
            } else {
                return (iy == 1) ? x : -one / x;
            }
        }
    }
    if (ix >= 0x3FE59428) {
        if (hx < 0) {
            x = -x;
            y = -y;
        }
        z = pio4 - x;
        w = pio4lo - y;
        x = z + w;
        y = 0.0;
    }
    z = x * x;
    w = z * z;
    r = T[1] + w * (T[3] + w * (T[5] + w * (T[7] + w * (T[9] + w * T[11]))));
    v = z * (T[2] + w * (T[4] + w * (T[6] + w * (T[8] + w * (T[10] + w * T[12])))));
    s = z * x;
    r = y + z * (s * (r + v) + y);
    r += T[0] * s;
    w = x + r;
    if (ix >= 0x3FE59428) {
        v = (double)iy;
        return (double)(1 - ((hx >> 30) & 2)) * (v - 2.0 * (x - (w * w / (w + v) - r)));
    }
    if (iy == 1) {
        return w;
    } else {
        double a, t;
        z = w;
        SET_LOW_WORD(z, 0);
        v = r - (z - x);
        t = a = -1.0 / w;
        SET_LOW_WORD(t, 0);
        s = 1.0 + t * z;
        return t + a * (s + t * v);
    }
}
