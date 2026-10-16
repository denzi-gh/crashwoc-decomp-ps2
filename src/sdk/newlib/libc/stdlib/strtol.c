/*
 * Unit: sdk/newlib/libc/stdlib/strtol
 *
 * Functions:
 *   0x002731e0 _strtol_r
 *   0x00273418 strtol
 */

struct _reent {
    int _errno;
};

#define _REENT (_impure_ptr[0])

extern struct _reent *_impure_ptr[];

extern const char _ctype_[];

#define _U 01
#define _L 02
#define _N 04
#define _S 010

#define isspace(c) ((_ctype_ + 1)[(int)(c)] & _S)
#define isdigit(c) ((_ctype_ + 1)[(int)(c)] & _N)
#define isupper(c) ((_ctype_ + 1)[(int)(c)] & _U)
#define isalpha(c) ((_ctype_ + 1)[(int)(c)] & (_U | _L))

#define LONG_MAX 0x7fffffffffffffffL
#define LONG_MIN (-LONG_MAX - 1L)
#define ERANGE 34

long _strtol_r(struct _reent *rptr, const char *nptr, char **endptr, int base)
{
    register const char *s = nptr;
    register unsigned long acc;
    register int c;
    register unsigned long cutoff;
    register int neg = 0, any, cutlim;

    do {
        c = *s++;
    } while (isspace(c));
    if (c == '-') {
        neg = 1;
        c = *s++;
    } else if (c == '+') {
        c = *s++;
    }
    if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X')) {
        c = s[1];
        s += 2;
        base = 16;
    }
    if (base == 0) {
        base = c == '0' ? 8 : 10;
    }

    cutoff = neg ? -(unsigned long)LONG_MIN : LONG_MAX;
    cutlim = cutoff % (unsigned long)base;
    cutoff /= (unsigned long)base;
    for (acc = 0, any = 0;; c = *s++) {
        if (isdigit(c)) {
            c -= '0';
        } else if (isalpha(c)) {
            c -= isupper(c) ? 'A' - 10 : 'a' - 10;
        } else {
            break;
        }
        if (c >= base) {
            break;
        }
        if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim)) {
            any = -1;
        } else {
            any = 1;
            acc *= base;
            acc += c;
        }
    }
    if (any < 0) {
        acc = neg ? LONG_MIN : LONG_MAX;
        rptr->_errno = ERANGE;
    } else if (neg) {
        acc = -acc;
    }
    if (endptr != 0) {
        *endptr = (char *)(any ? s - 1 : nptr);
    }
    return acc;
}


long strtol(const char *s, char **ptr, int base)
{
    return _strtol_r(_REENT, s, ptr, base);
}
