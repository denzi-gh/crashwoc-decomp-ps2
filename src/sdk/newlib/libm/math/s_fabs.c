/*
 * Unit: sdk/newlib/libm/math/s_fabs
 *
 * Functions:
 *   0x0026cb50 fabs
 */

typedef unsigned int __uint32_t;

typedef union {
    double value;
    struct {
        __uint32_t lsw;
        __uint32_t msw;
    } parts;
} ieee_double_shape_type;

#define GET_HIGH_WORD(i, d)                     do {                                            ieee_double_shape_type gh_u;                gh_u.value = (d);                           (i) = gh_u.parts.msw;                   } while (0)

#define SET_HIGH_WORD(d, v)                     do {                                            ieee_double_shape_type sh_u;                sh_u.value = (d);                           sh_u.parts.msw = (v);                       (d) = sh_u.value;                       } while (0)


double fabs(double x)
{
    __uint32_t high;

    GET_HIGH_WORD(high, x);
    SET_HIGH_WORD(x, high & 0x7fffffff);
    return x;
}
