/*
 * Unit: sdk/newlib/libm/common/s_finite
 *
 * Functions:
 *   0x0026cb88 finite
 */

typedef union {
    double value;
    struct {
        unsigned int lsw;
        unsigned int msw;
    } parts;
} ieee_double_shape_type;

#define GET_HIGH_WORD(i, d) do { ieee_double_shape_type gh_u; gh_u.value = (d); (i) = gh_u.parts.msw; } while (0)

int finite(double x)
{
    int hx;

    GET_HIGH_WORD(hx, x);
    return (int)((unsigned int)((hx & 0x7fffffff) - 0x7ff00000) >> 31);
}
