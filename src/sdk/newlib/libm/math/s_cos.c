/*
 * Unit: sdk/newlib/libm/math/s_cos
 *
 * Functions:
 *   0x00268420 cos
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

extern double __kernel_sin(double x, double y, int iy);
extern double __kernel_cos(double x, double y);
extern __int32_t __ieee754_rem_pio2(double x, double *y);

double cos(double x)
{
    double y[2], z = 0.0;
    __int32_t n, ix;

    GET_HIGH_WORD(ix, x);

    ix &= 0x7fffffff;
    if (ix <= 0x3fe921fb) {
        return __kernel_cos(x, z);
    } else if (ix >= 0x7ff00000) {
        return x - x;
    } else {
        n = __ieee754_rem_pio2(x, y);
        switch (n & 3) {
        case 0:
            return __kernel_cos(y[0], y[1]);
        case 1:
            return -__kernel_sin(y[0], y[1], 1);
        case 2:
            return -__kernel_cos(y[0], y[1]);
        default:
            return __kernel_sin(y[0], y[1], 1);
        }
    }
}
