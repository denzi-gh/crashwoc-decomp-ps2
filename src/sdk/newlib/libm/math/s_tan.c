/*
 * Unit: sdk/newlib/libm/math/s_tan
 *
 * Functions:
 *   0x002687f8 tan
 */

typedef unsigned int __uint32_t;
typedef int __int32_t;

typedef union {
    double value;
    struct {
        __uint32_t lsw;
        __uint32_t msw;
    } parts;
} ieee_double_shape_type;

#define GET_HIGH_WORD(i, d) do { ieee_double_shape_type gh_u; gh_u.value = (d); (i) = gh_u.parts.msw; } while (0)

extern double __kernel_tan(double x, double y, int iy);
extern __int32_t __ieee754_rem_pio2(double x, double *y);


double tan(double x)
{
    double y[2];
    double z = 0.0;
    __int32_t n, ix;

    GET_HIGH_WORD(ix, x);
    ix &= 0x7fffffff;
    if (ix <= 0x3fe921fb) {
        return __kernel_tan(x, z, 1);
    } else if (ix >= 0x7ff00000) {
        return x - x;
    } else {
        n = __ieee754_rem_pio2(x, y);
        return __kernel_tan(y[0], y[1], 1 - ((n & 1) << 1));
    }
}
