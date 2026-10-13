/*
 * Unit: sdk/newlib/libm/math/w_pow
 *
 * Functions:
 *   0x00268bb0 pow
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
#define OVERFLOW 3
#define UNDERFLOW 4
#define EDOM 33
#define ERANGE 34

#define HUGE 3.40282346638528860e+38

typedef enum {
    _IEEE_ = -1,
    _SVID_,
    _XOPEN_,
    _POSIX_,
    _ISOC_
} _LIB_VERSION_TYPE;

extern const _LIB_VERSION_TYPE __fdlib_version[];
#define _LIB_VERSION (__fdlib_version[0])

union __dmath {
    unsigned int i[2];
    double d;
};

extern const union __dmath __infinity[];
#define HUGE_VAL (__infinity[0].d)

extern char D_0062B2A8[];
#define pow_name D_0062B2A8

extern double __ieee754_pow(double x, double y);
extern int isnan(double x);
extern int finite(double x);
extern double rint(double x);
extern int matherr(struct exception *e);
extern int *__errno(void);

#define errno (*__errno())

double pow(double x, double y)
{
    double z;
    struct exception exc;

    z = __ieee754_pow(x, y);
    if (_LIB_VERSION == _IEEE_ || isnan(y)) {
        return z;
    }
    if (isnan(x)) {
        if (y == 0.0) {
            exc.type = DOMAIN;
            exc.name = pow_name;
            exc.err = 0;
            exc.operand1 = x;
            exc.operand2 = y;
            exc.retval = x;
            if (_LIB_VERSION == _IEEE_ || _LIB_VERSION == _POSIX_) {
                exc.retval = 1.0;
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
    if (x == 0.0) {
        if (y == 0.0) {
            exc.type = DOMAIN;
            exc.name = pow_name;
            exc.err = 0;
            exc.operand1 = x;
            exc.operand2 = y;
            exc.retval = 0.0;
            if (_LIB_VERSION != _SVID_) {
                exc.retval = 1.0;
            } else if (!matherr(&exc)) {
                errno = EDOM;
            }
            if (exc.err != 0) {
                errno = exc.err;
            }
            return exc.retval;
        }
        if (finite(y) && y < 0.0) {
            exc.type = DOMAIN;
            exc.name = pow_name;
            exc.err = 0;
            exc.operand1 = x;
            exc.operand2 = y;
            if (_LIB_VERSION == _SVID_) {
                exc.retval = 0.0;
            } else {
                exc.retval = -HUGE_VAL;
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
        }
        return z;
    }
    if (!finite(z)) {
        if (finite(x) && finite(y)) {
            if (isnan(z)) {
                exc.type = DOMAIN;
                exc.name = pow_name;
                exc.err = 0;
                exc.operand1 = x;
                exc.operand2 = y;
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
                exc.type = OVERFLOW;
                exc.name = pow_name;
                exc.err = 0;
                exc.operand1 = x;
                exc.operand2 = y;
                if (_LIB_VERSION == _SVID_) {
                    exc.retval = HUGE;
                    y *= 0.5;
                    if (x < 0.0 && rint(y) != y) {
                        exc.retval = (-HUGE);
                    }
                } else {
                    exc.retval = HUGE_VAL;
                    y *= 0.5;
                    if (x < 0.0 && rint(y) != y) {
                        exc.retval = -HUGE_VAL;
                    }
                }
                if (_LIB_VERSION == _POSIX_) {
                    errno = ERANGE;
                } else if (!matherr(&exc)) {
                    errno = ERANGE;
                }
                if (exc.err != 0) {
                    errno = exc.err;
                }
                return exc.retval;
            }
        }
    }
    if (z == 0.0 && finite(x) && finite(y)) {
        exc.type = UNDERFLOW;
        exc.name = pow_name;
        exc.err = 0;
        exc.operand1 = x;
        exc.operand2 = y;
        exc.retval = 0.0;
        if (_LIB_VERSION == _POSIX_) {
            errno = ERANGE;
        } else if (!matherr(&exc)) {
            errno = ERANGE;
        }
        if (exc.err != 0) {
            errno = exc.err;
        }
        return exc.retval;
    }
    return z;
}
