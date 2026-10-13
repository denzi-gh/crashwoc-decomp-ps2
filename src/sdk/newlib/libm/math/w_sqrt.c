/*
 * Unit: sdk/newlib/libm/math/w_sqrt
 *
 * Functions:
 *   0x00268fe0 sqrt
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

extern char D_0062B2C8[];
#define sqrt_name D_0062B2C8

extern double __ieee754_sqrt(double x);
extern int isnan(double x);
extern int matherr(struct exception *e);
extern int *__errno(void);

#define errno (*__errno())

double sqrt(double x)
{
    double z;
    struct exception exc;

    z = __ieee754_sqrt(x);
    if (_LIB_VERSION == _IEEE_ || isnan(x)) {
        return z;
    }
    if (x < 0.0) {
        exc.type = DOMAIN;
        exc.name = sqrt_name;
        exc.err = 0;
        exc.operand1 = exc.operand2 = x;
        if (_LIB_VERSION == _SVID_) {
            exc.retval = 0.0;
        } else {
            exc.retval = 0.0 / 0.0;
        }
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
