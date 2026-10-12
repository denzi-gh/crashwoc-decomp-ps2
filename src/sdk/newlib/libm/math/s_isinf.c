/*
 * Unit: sdk/newlib/libm/math/s_isinf
 *
 * Functions:
 *   0x002783f8 isinf
 */

typedef unsigned int u_int32_t;
typedef int int32_t;

typedef union {
    double value;
    struct {
        u_int32_t lsw;
        u_int32_t msw;
    } parts;
} ieee_double_shape_type;

#define EXTRACT_WORDS(ix0, ix1, d) do { ieee_double_shape_type ew_u; ew_u.value = (d); (ix0) = ew_u.parts.msw; (ix1) = ew_u.parts.lsw; } while (0)

int isinf(double x)
{
    int32_t hx, lx;

    EXTRACT_WORDS(hx, lx, x);
    hx &= 0x7fffffff;
    hx |= (u_int32_t)(lx | (-lx)) >> 31;
    hx = 0x7ff00000 - hx;
    return 1 - (int)((u_int32_t)(hx | (-hx)) >> 31);
}
