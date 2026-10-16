/*
 * Unit: sdk/newlib/libc/stdio/vfprintf
 *
 * Functions:
 *   0x002734a8 __sprint
 *   0x002734f0 __sbprintf
 *   0x002735a8 vfprintf
 *   0x00273620 _vfprintf_r
 *   0x00274b38 cvt
 *   0x00274ce8 exponent
 */

typedef char *va_list;

struct _reent;

struct __sbuf {
    unsigned char *_base;
    int _size;
};

typedef struct __sFILE {
    unsigned char *_p;
    int _r;
    int _w;
    short _flags;
    short _file;
    struct __sbuf _bf;
    int _lbfsize;
    void *_cookie;
    int (*_read)(void *cookie, char *buf, int n);
    int (*_write)(void *cookie, const char *buf, int n);
    long (*_seek)(void *cookie, long offset, int whence);
    int (*_close)(void *cookie);
    struct __sbuf _ub;
    unsigned char *_up;
    int _ur;
    unsigned char _ubuf[3];
    unsigned char _nbuf[1];
    struct __sbuf _lb;
    int _blksize;
    int _offset;
    struct _reent *_data;
} FILE;

struct _reent {
    int _errno;
    FILE *_stdin;
    FILE *_stdout;
    FILE *_stderr;
    int _inc;
    char _emergency[25];
    int _current_category;
    const char *_current_locale;
    int __sdidinit;
};

struct __siov {
    void *iov_base;
    int iov_len;
};

struct __suio {
    struct __siov *uio_iov;
    int uio_iovcnt;
    int uio_resid;
};

union double_union {
    double d;
    unsigned int i[2];
};

struct lconv {
    char *decimal_point;
};

#define __SNBF 0x0002
#define __SWR 0x0008
#define __SRW 0x0010
#define __SERR 0x0040

#define BUFSIZ 1024
#define EOF (-1)

#define va_arg(ap, type) (*(type *)(((ap) += 8) - 8))

#define BUF 348
#define DEFPREC 6
#define NIOV 8
#define PADSIZE 16

#define ALT 0x001
#define HEXPREFIX 0x002
#define LADJUST 0x004
#define LONGDBL 0x008
#define LONGINT 0x010
#define QUADINT 0x020
#define SHORTINT 0x040
#define ZEROPAD 0x080
#define FPT 0x100

#define to_digit(c) ((c) - '0')
#define is_digit(c) ((unsigned)to_digit(c) <= 9)

#define PRINT(ptr, len) { iovp->iov_base = (ptr); iovp->iov_len = (len); uio.uio_resid += (len); iovp++; if (++uio.uio_iovcnt >= NIOV) { if (__sprint(fp, &uio)) goto error; iovp = iov; } }
#define PAD(howmany, with) { if ((n = (howmany)) > 0) { while (n > PADSIZE) { PRINT(with, PADSIZE); n -= PADSIZE; } PRINT(with, n); } }
#define FLUSH() { if (uio.uio_resid && __sprint(fp, &uio)) goto error; uio.uio_iovcnt = 0; iovp = iov; }

#define SARG() (flags & LONGINT ? va_arg(ap, long) : flags & SHORTINT ? (long)(short)va_arg(ap, int) : (long)va_arg(ap, int))
#define UARG() (flags & LONGINT ? va_arg(ap, unsigned long) : flags & SHORTINT ? (unsigned long)(unsigned short)va_arg(ap, int) : (unsigned long)va_arg(ap, unsigned int))

#define _REENT (_impure_ptr[0])
#define DEFAULT_REENT(fp) do { if ((fp)->_data == 0) (fp)->_data = _REENT; } while (0)
#define CHECK_INIT(ptr) do { if (!(ptr)->__sdidinit) __sinit(ptr); } while (0)

#define MAXEXP 308
#define to_char(n) ((n) + '0')

extern struct _reent *_impure_ptr[];
extern int __mb_cur_max[];

extern char D_0062C0C0[];
extern char D_0062C0C8[];
extern char D_0062C0D0[];
extern char D_0062C0E8[];
extern char D_0062C0F0[];
extern char D_0062C108[];
extern char D_0062C128[];
extern char D_0062C130[];

#define str_inf D_0062C0C0
#define str_nan D_0062C0C8
#define xdigits_lower D_0062C0D0
#define str_null D_0062C0E8
#define xdigits_upper D_0062C0F0
#define str_bad_base D_0062C108
#define str_zero D_0062C128
#define str_point D_0062C130

#define MB_CUR_MAX (__mb_cur_max[0])

extern int __sfvwrite(FILE *fp, struct __suio *uio);
extern void __sinit(struct _reent *ptr);
extern int fflush(FILE *fp);
extern char *_dtoa_r(struct _reent *ptr, double d, int mode, int ndigits, int *decpt, int *sign, char **rve);
extern struct lconv *localeconv(void);
extern int __swsetup(FILE *fp);
extern int _mbtowc_r(struct _reent *r, int *pwc, const char *s, int n, int *state);
extern int isinf(double x);
extern int isnan(double x);
extern void *memchr(const void *s, int c, unsigned int n);
extern unsigned int strlen(const char *s);

int _vfprintf_r(struct _reent *data, FILE *fp, const char *fmt0, va_list ap);
int vfprintf(FILE *fp, const char *fmt0, va_list ap);
char *cvt(struct _reent *data, double value, int ndigits, int flags, char *sign, int *decpt, int ch, int *length);
int exponent(char *p0, int exp, int fmtch);


int __sprint(FILE *fp, struct __suio *uio)
{
    int err;

    if (uio->uio_resid == 0) {
        uio->uio_iovcnt = 0;
        return 0;
    }
    err = __sfvwrite(fp, uio);
    uio->uio_resid = 0;
    uio->uio_iovcnt = 0;
    return err;
}


int __sbprintf(FILE *fp, const char *fmt, va_list ap)
{
    int ret;
    FILE fake;
    unsigned char buf[BUFSIZ];

    fake._data = fp->_data;
    fake._flags = fp->_flags & ~__SNBF;
    fake._file = fp->_file;
    fake._cookie = fp->_cookie;
    fake._write = fp->_write;

    fake._bf._base = fake._p = buf;
    fake._bf._size = fake._w = sizeof(buf);
    fake._lbfsize = 0;

    ret = vfprintf(&fake, fmt, ap);
    if (ret >= 0 && fflush(&fake)) {
        ret = EOF;
    }
    if (fake._flags & __SERR) {
        fp->_flags |= __SERR;
    }
    return ret;
}


int vfprintf(FILE *fp, const char *fmt0, va_list ap)
{
    DEFAULT_REENT(fp);
    CHECK_INIT(fp->_data);
    return _vfprintf_r(fp->_data, fp, fmt0, ap);
}


int _vfprintf_r(struct _reent *data, FILE *fp, const char *fmt0, va_list ap)
{
    char *fmt;
    int ch;
    int n, m;
    char *cp;
    struct __siov *iovp;
    int flags;
    int ret;
    int width;
    int prec;
    char sign;
    int wc;
    char *decimal_point = localeconv()->decimal_point;
    char softsign;
    double _double;
    int expt;
    int expsize;
    int ndig;
    char expstr[7];
    unsigned long _uquad;
    enum { OCT, DEC, HEX } base;
    int dprec;
    int realsz;
    int size;
    char *xdigs;
    struct __suio uio;
    struct __siov iov[NIOV];
    char buf[BUF];
    char ox[2];
    int state = 0;
    static const char blanks[16] =
        { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
          ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
    static const char zeroes[16] =
        { '0', '0', '0', '0', '0', '0', '0', '0',
          '0', '0', '0', '0', '0', '0', '0', '0' };

    if ((!(fp->_flags & __SWR) || fp->_bf._base == 0) && __swsetup(fp)) {
        return EOF;
    }

    if ((fp->_flags & (__SNBF | __SWR | __SRW)) == (__SNBF | __SWR) && fp->_file >= 0) {
        return __sbprintf(fp, fmt0, ap);
    }

    fmt = (char *)fmt0;
    uio.uio_iov = iovp = iov;
    uio.uio_resid = 0;
    uio.uio_iovcnt = 0;
    ret = 0;

    for (;;) {
        cp = fmt;
        while ((n = _mbtowc_r(_REENT, &wc, fmt, MB_CUR_MAX, &state)) > 0) {
            fmt += n;
            if (wc == '%') {
                fmt--;
                break;
            }
        }
        if ((m = fmt - cp) != 0) {
            PRINT(cp, m);
            ret += m;
        }
        if (n <= 0) {
            goto done;
        }
        fmt++;

        flags = 0;
        dprec = 0;
        width = 0;
        prec = -1;
        sign = '\0';

rflag:
        ch = *fmt++;
reswitch:
        switch (ch) {
        case ' ':
            if (!sign) {
                sign = ' ';
            }
            goto rflag;
        case '#':
            flags |= ALT;
            goto rflag;
        case '*':
            if ((width = va_arg(ap, int)) >= 0) {
                goto rflag;
            }
            width = -width;
        case '-':
            flags |= LADJUST;
            goto rflag;
        case '+':
            sign = '+';
            goto rflag;
        case '.':
            if ((ch = *fmt++) == '*') {
                n = va_arg(ap, int);
                prec = n < 0 ? -1 : n;
                goto rflag;
            }
            n = 0;
            while (is_digit(ch)) {
                n = 10 * n + to_digit(ch);
                ch = *fmt++;
            }
            prec = n < 0 ? -1 : n;
            goto reswitch;
        case '0':
            flags |= ZEROPAD;
            goto rflag;
        case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            n = 0;
            do {
                n = 10 * n + to_digit(ch);
                ch = *fmt++;
            } while (is_digit(ch));
            width = n;
            goto reswitch;
        case 'L':
            flags |= LONGDBL;
            goto rflag;
        case 'h':
            flags |= SHORTINT;
            goto rflag;
        case 'l':
            if (*fmt == 'l') {
                fmt++;
                flags |= QUADINT;
            } else {
                flags |= LONGINT;
            }
            goto rflag;
        case 'q':
            flags |= QUADINT;
            goto rflag;
        case 'c':
            *(cp = buf) = va_arg(ap, int);
            size = 1;
            sign = '\0';
            break;
        case 'D':
            flags |= LONGINT;
        case 'd':
        case 'i':
            _uquad = SARG();
            if ((long)_uquad < 0) {
                _uquad = -_uquad;
                sign = '-';
            }
            base = DEC;
            goto number;
        case 'e':
        case 'E':
        case 'f':
        case 'g':
        case 'G':
            if (prec == -1) {
                prec = DEFPREC;
            } else if ((ch == 'g' || ch == 'G') && prec == 0) {
                prec = 1;
            }
            if (flags & LONGDBL) {
                _double = (double)va_arg(ap, long double);
            } else {
                _double = va_arg(ap, double);
            }
            if (isinf(_double)) {
                if (_double < 0) {
                    sign = '-';
                }
                cp = str_inf;
                size = 3;
                break;
            }
            if (isnan(_double)) {
                cp = str_nan;
                size = 3;
                break;
            }
            flags |= FPT;
            cp = cvt(data, _double, prec, flags, &softsign, &expt, ch, &ndig);
            if (ch == 'g' || ch == 'G') {
                if (expt <= -4 || expt > prec) {
                    ch = (ch == 'g') ? 'e' : 'E';
                } else {
                    ch = 'g';
                }
            }
            if (ch <= 'e') {
                --expt;
                expsize = exponent(expstr, expt, ch);
                size = expsize + ndig;
                if (ndig > 1 || flags & ALT) {
                    ++size;
                }
            } else if (ch == 'f') {
                if (expt > 0) {
                    size = expt;
                    if (prec || flags & ALT) {
                        size += prec + 1;
                    }
                } else {
                    size = prec + 2;
                }
            } else if (expt >= ndig) {
                size = expt;
                if (flags & ALT) {
                    ++size;
                }
            } else {
                size = ndig + (expt > 0 ? 1 : 2 - expt);
            }
            if (softsign) {
                sign = '-';
            }
            break;
        case 'n':
            if (flags & LONGINT) {
                *va_arg(ap, long *) = ret;
            } else if (flags & SHORTINT) {
                *va_arg(ap, short *) = ret;
            } else {
                *va_arg(ap, int *) = ret;
            }
            continue;
        case 'O':
            flags |= LONGINT;
        case 'o':
            _uquad = UARG();
            base = OCT;
            goto nosign;
        case 'p':
            _uquad = (unsigned long)va_arg(ap, void *);
            base = HEX;
            xdigs = xdigits_lower;
            flags |= HEXPREFIX;
            ch = 'x';
            goto nosign;
        case 's':
            if ((cp = va_arg(ap, char *)) == 0) {
                cp = str_null;
            }
            if (prec >= 0) {
                char *p = memchr(cp, 0, prec);

                if (p != 0) {
                    size = p - cp;
                    if (size > prec) {
                        size = prec;
                    }
                } else {
                    size = prec;
                }
            } else {
                size = strlen(cp);
            }
            sign = '\0';
            break;
        case 'U':
            flags |= LONGINT;
        case 'u':
            _uquad = UARG();
            base = DEC;
            goto nosign;
        case 'X':
            xdigs = xdigits_upper;
            goto hex;
        case 'x':
            xdigs = xdigits_lower;
hex:
            _uquad = UARG();
            base = HEX;
            if (flags & ALT && _uquad != 0) {
                flags |= HEXPREFIX;
            }
nosign:
            sign = '\0';
number:
            if ((dprec = prec) >= 0) {
                flags &= ~ZEROPAD;
            }
            cp = buf + BUF;
            if (_uquad != 0 || prec != 0) {
                switch (base) {
                case OCT:
                    do {
                        *--cp = to_char(_uquad & 7);
                        _uquad >>= 3;
                    } while (_uquad);
                    if (flags & ALT && *cp != '0') {
                        *--cp = '0';
                    }
                    break;
                case DEC:
                    while (_uquad >= 10) {
                        *--cp = to_char(_uquad % 10);
                        _uquad /= 10;
                    }
                    *--cp = to_char(_uquad);
                    break;
                case HEX:
                    do {
                        *--cp = xdigs[_uquad & 15];
                        _uquad >>= 4;
                    } while (_uquad);
                    break;
                default:
                    cp = str_bad_base;
                    size = strlen(cp);
                    goto skipsize;
                }
            }
            size = buf + BUF - cp;
skipsize:
            break;
        default:
            if (ch == '\0') {
                goto done;
            }
            cp = buf;
            *cp = ch;
            size = 1;
            sign = '\0';
            break;
        }

        realsz = dprec > size ? dprec : size;
        if (sign) {
            realsz++;
        } else if (flags & HEXPREFIX) {
            realsz += 2;
        }

        if ((flags & (LADJUST | ZEROPAD)) == 0) {
            PAD(width - realsz, blanks);
        }

        if (sign) {
            PRINT(&sign, 1);
        } else if (flags & HEXPREFIX) {
            ox[0] = '0';
            ox[1] = ch;
            PRINT(ox, 2);
        }

        if ((flags & (LADJUST | ZEROPAD)) == ZEROPAD) {
            PAD(width - realsz, zeroes);
        }

        PAD(dprec - size, zeroes);

        if ((flags & FPT) == 0) {
            PRINT(cp, size);
        } else {
            if (ch >= 'f') {
                if (_double == 0) {
                    PRINT(str_zero, 1);
                    if (expt < ndig || (flags & ALT) != 0) {
                        PRINT(decimal_point, 1);
                        PAD(ndig - 1, zeroes);
                    }
                } else if (expt <= 0) {
                    PRINT(str_zero, 1);
                    PRINT(decimal_point, 1);
                    PAD(-expt, zeroes);
                    PRINT(cp, ndig);
                } else if (expt >= ndig) {
                    PRINT(cp, ndig);
                    PAD(expt - ndig, zeroes);
                    if (flags & ALT) {
                        PRINT(str_point, 1);
                    }
                } else {
                    PRINT(cp, expt);
                    cp += expt;
                    PRINT(str_point, 1);
                    PRINT(cp, ndig - expt);
                }
            } else {
                if (ndig > 1 || flags & ALT) {
                    ox[0] = *cp++;
                    ox[1] = '.';
                    PRINT(ox, 2);
                    if (_double) {
                        PRINT(cp, ndig - 1);
                    } else {
                        PAD(ndig - 1, zeroes);
                    }
                } else {
                    PRINT(cp, 1);
                }
                PRINT(expstr, expsize);
            }
        }

        if (flags & LADJUST) {
            PAD(width - realsz, blanks);
        }

        ret += width > realsz ? width : realsz;

        FLUSH();
    }
done:
    FLUSH();
error:
    return (fp->_flags & __SERR) ? EOF : ret;
}


char *cvt(struct _reent *data, double value, int ndigits, int flags, char *sign, int *decpt, int ch, int *length)
{
    int mode, dsgn;
    char *digits, *bp, *rve;
    union double_union tmp;

    if (ch == 'f') {
        mode = 3;
    } else {
        if (ch == 'e' || ch == 'E') {
            ndigits++;
        }
        mode = 2;
    }
    tmp.d = value;
    if (tmp.i[1] & 0x80000000) {
        value = -value;
        *sign = '-';
    } else {
        *sign = '\0';
    }
    digits = _dtoa_r(data, value, mode, ndigits, decpt, &dsgn, &rve);
    if ((ch != 'g' && ch != 'G') || flags & ALT) {
        bp = digits + ndigits;
        if (ch == 'f') {
            if (*digits == '0' && value) {
                *decpt = -ndigits + 1;
            }
            bp += *decpt;
        }
        if (value == 0) {
            rve = bp;
        }
        while (rve < bp) {
            *rve++ = '0';
        }
    }
    *length = rve - digits;
    return digits;
}


int exponent(char *p0, int exp, int fmtch)
{
    char *p, *t;
    char expbuf[MAXEXP];

    p = p0;
    *p++ = fmtch;
    if (exp < 0) {
        exp = -exp;
        *p++ = '-';
    } else {
        *p++ = '+';
    }
    t = expbuf + MAXEXP;
    if (exp > 9) {
        do {
            *--t = to_char(exp % 10);
        } while ((exp /= 10) > 9);
        *--t = to_char(exp);
        for (; t < expbuf + MAXEXP; *p++ = *t++) {
        }
    } else {
        *p++ = '0';
        *p++ = to_char(exp);
    }
    return p - p0;
}
