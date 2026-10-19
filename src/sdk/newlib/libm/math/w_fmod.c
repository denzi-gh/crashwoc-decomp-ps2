/*
 * Unit: sdk/newlib/libm/math/w_fmod
 *
 * Functions:
 *   0x00268a90 fmod
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

extern double __ieee754_fmod(double x, double y);
extern int isnan(double x);
extern int matherr(struct exception *e);
extern int *__errno(void);

#define errno (*__errno())

double fmod(double x, double y)
{
    double z;
    struct exception exc;

    z = __ieee754_fmod(x, y);
    if (_LIB_VERSION == _IEEE_ || isnan(y) || isnan(x)) {
        return z;
    }
    if (y == 0.0) {
        exc.type = DOMAIN;
        exc.name = "fmod";
        exc.operand1 = x;
        exc.operand2 = y;
        exc.err = 0;
        if (_LIB_VERSION == _SVID_) {
            exc.retval = x;
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
