/*
 * Unit: sdk/newlib/libm/math/s_infconst
 *
 * Data only (no functions): the double +infinity, as words (little-endian).
 */

typedef unsigned int __uint32_t;

union __dmath {
    __uint32_t i[2];
    double d;
};

const union __dmath __infinity = { { 0, 0x7ff00000 } };
