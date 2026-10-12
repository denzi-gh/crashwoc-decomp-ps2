/*
 * Unit: sdk/newlib/libc/stdlib/mbtowc_r
 *
 * Functions:
 *   0x002771b8 _mbtowc_r
 */

typedef unsigned int size_t;
typedef int wchar_t;

struct _reent;

int _mbtowc_r(struct _reent *r, wchar_t *pwc, const char *s, size_t n, int *state)
{
    wchar_t dummy;
    unsigned char *t = (unsigned char *)s;

    if (pwc == 0) {
        pwc = &dummy;
    }

    if (s != 0 && n == 0) {
        return -1;
    }

    if (s == 0) {
        return 0;
    }

    *pwc = (wchar_t)*t;

    if (*t == '\0') {
        return 0;
    }

    return 1;
}
