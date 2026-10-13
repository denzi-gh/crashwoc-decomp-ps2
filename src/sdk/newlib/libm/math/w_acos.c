/*
 * Unit: sdk/newlib/libm/math/w_acos
 *
 * Functions:
 *   0x00268888 acos
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

extern char D_0062B288[];
#define acos_name D_0062B288

extern double __ieee754_acos(double x);
extern int isnan(double x);
extern double fabs(double x, ...);
extern int matherr(struct exception *e);
extern int *__errno(void);

#define errno (*__errno())

double acos(double x)
{
    double z;
    struct exception exc;

    z = __ieee754_acos(x);
    if (_LIB_VERSION == _IEEE_ || isnan(x)) {
        return z;
    }
    if (fabs(x) > 1.0) {
        exc.type = DOMAIN;
        exc.name = acos_name;
        exc.err = 0;
        exc.operand1 = exc.operand2 = x;
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
