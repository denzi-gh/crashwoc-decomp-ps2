/*
 * Unit: sdk/newlib/libm/math/s_atan
 *
 * Functions:
 *   0x0026c740 atan
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

extern double fabs(double x, ...);

static const double
atanhi[] = {
    0.4636476090008061, 0.7853981633974483, 0.982793723247329,
    1.5707963267948966,
},
atanlo[] = {
    2.2698777452961687e-17, 3.061616997868383e-17, 1.3903311031230998e-17,
    6.123233995736766e-17,
},
aT[] = {
    0.3333333333333293, -0.19999999999876483, 0.14285714272503466,
    -0.11111110405462356, 0.09090887133436507, -0.0769187620504483,
    0.06661073137387531, -0.058335701337905735, 0.049768779946159324,
    -0.036531572744216916, 0.016285820115365782,
},
one = 1.0,
huge = 1.0e300;

double atan(double x)
{
    double w, s1, s2, z;
    __int32_t ix, hx, id;

    GET_HIGH_WORD(hx, x);
    ix = hx & 0x7fffffff;
    if (ix >= 0x44100000) {
        __uint32_t low;
        GET_LOW_WORD(low, x);
        if (ix > 0x7ff00000 || (ix == 0x7ff00000 && (low != 0))) {
            return x + x;
        }
        if (hx > 0) {
            return atanhi[3] + atanlo[3];
        } else {
            return -atanhi[3] - atanlo[3];
        }
    }
    if (ix < 0x3fdc0000) {
        if (ix < 0x3e200000) {
            if (huge + x > one) {
                return x;
            }
        }
        id = -1;
    } else {
        x = fabs(x);
        if (ix < 0x3ff30000) {
            if (ix < 0x3fe60000) {
                id = 0;
                x = (2.0 * x - one) / (2.0 + x);
            } else {
                id = 1;
                x = (x - one) / (x + one);
            }
        } else {
            if (ix < 0x40038000) {
                id = 2;
                x = (x - 1.5) / (one + 1.5 * x);
            } else {
                id = 3;
                x = -1.0 / x;
            }
        }
    }
    z = x * x;
    w = z * z;
    s1 = z * (aT[0] + w * (aT[2] + w * (aT[4] + w * (aT[6] + w * (aT[8] + w * aT[10])))));
    s2 = w * (aT[1] + w * (aT[3] + w * (aT[5] + w * (aT[7] + w * aT[9]))));
    if (id < 0) {
        return x - x * (s1 + s2);
    } else {
        z = atanhi[id] - ((x * (s1 + s2) - atanlo[id]) - x);
        return (hx < 0) ? -z : z;
    }
}
