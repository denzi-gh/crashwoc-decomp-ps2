/*
 * Unit: sdk/newlib/libm/math/w_atan2
 *
 * Functions:
 *   0x00268978 atan2
 */

struct exception {
    int type;
    char *name;
    double operand1;
    double operand2;
    double retval;
    int err;
};

#define DOMAIN 1
#define EDOM 33

typedef enum {
    _IEEE_ = -1,
    _SVID_,
    _XOPEN_,
    _POSIX_,
    _ISOC_
} _LIB_VERSION_TYPE;

extern const _LIB_VERSION_TYPE __fdlib_version[];
#define _LIB_VERSION (__fdlib_version[0])

extern char D_0062B290[];
#define atan2_name D_0062B290

extern double __ieee754_atan2(double y, double x);
extern int isnan(double x);
extern int matherr(struct exception *e);
extern int *__errno(void);

#define errno (*__errno())

double atan2(double y, double x)
{
    double z;
    struct exception exc;

    z = __ieee754_atan2(y, x);
    if (_LIB_VERSION == _IEEE_ || isnan(x) || isnan(y)) {
        return z;
    }
    if (x == 0.0 && y == 0.0) {
        exc.operand1 = y;
        exc.operand2 = x;
        exc.type = DOMAIN;
        exc.name = atan2_name;
        exc.err = 0;
        exc.retval = 0.0;
        if (_LIB_VERSION == _POSIX_) {
            errno = EDOM;
        } else if (!matherr(&exc)) {
            errno = EDOM;
        }
        if (exc.err != 0) {
            errno = exc.err;
        }
        return exc.retval;
    } else {
        return z;
    }
}
