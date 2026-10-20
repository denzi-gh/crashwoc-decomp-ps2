/*
 * Unit: sdk/sce/kprintf
 *
 * Functions:
 *   0x0027a2a8 kputchar
 *   0x0027a2e0 deci2Putchar
 *   0x0027a390 serialPutchar
 *   0x0027a3c8 ftoi
 *   0x0027a458 printfloat
 *   0x0027a5c0 _printf
 *   0x0027ab88 kprintf
 *   0x0027abc0 scePrintf
 */

typedef unsigned int u_int;
typedef unsigned short u_short;
typedef unsigned long u_long;
typedef volatile u_int vu_int;
typedef volatile unsigned char vu_char;

typedef char *va_list;
#define va_start(ap, last) ((ap) = (va_list)__builtin_next_arg(last) - (__builtin_args_info(2) < 8 ? (8 - __builtin_args_info(2)) * 8 : 0))
#define va_arg(ap, type) (*(type *)(((ap) += 8) - 8))
#define va_end(ap)

#define SIO_ISR    (*(vu_int *)0x1000f130)
#define SIO_TXFIFO (*(vu_char *)0x1000f180)

#define SIO_ISR_TXFULL 0x8000

#define DECI2_BUF_SIZE 128

extern int kputs(const char *str);

void serialPutchar(int c);
void kprintf(const char *fmt, ...);

static int count = 0;
static void (*_putchar)(int c) = serialPutchar;
static char linebuf[DECI2_BUF_SIZE];


int kputchar(int c)
{
    while (SIO_ISR & SIO_ISR_TXFULL) {
    }
    SIO_TXFIFO = c;
    return c;
}


void deci2Putchar(int c)
{
    if (count >= DECI2_BUF_SIZE - 2) {
        count = 0;
        linebuf[DECI2_BUF_SIZE - 1] = 0;
        kputs(linebuf);
    }
    if (c == '\n') {
        linebuf[count] = c;
        linebuf[count + 1] = 0;
        count = 0;
        kputs(linebuf);
    } else {
        linebuf[count++] = c;
    }
}


void serialPutchar(int c)
{
    if (c == '\n') {
        kputchar('\r');
        kputchar('\n');
    } else {
        kputchar(c);
    }
}


int ftoi(u_long bits)
{
    u_long mant;
    long exp;

    mant = bits;
    exp = (mant << 1) >> 53;
    exp -= 1075;
    if (exp < -53) {
        return 0;
    }
    if (exp >= 13) {
        return 9999;
    }
    mant = (mant << 12) >> 12;
    mant |= 1L << 52;
    if (exp < 0) {
        exp = -exp;
        mant >>= exp - 2;
        if ((mant & 3) == 3) {
            mant = (mant >> 2) + 1;
        } else {
            mant >>= 2;
        }
    } else {
        mant <<= exp;
    }
    return mant;
}


void printfloat(double d)
{
    int exp;
    int digits;

    exp = 0;
    if (d < 0.0) {
        d = 0.0 - d;
        _putchar('-');
    }
    if (d < 0.1) {
        while (d < 0.1) {
            d *= 10.0;
            exp--;
        }
    } else if (d >= 1.0) {
        while (d >= 1.0) {
            d /= 10.0;
            exp++;
        }
    }
    digits = ftoi(d * 1000000.0);
    kprintf("0.%d", digits);
    if (exp >= 0) {
        kprintf("e+%d", exp);
    } else {
        kprintf("e%d", exp);
    }
}


void _printf(const char *fmt, va_list ap)
{
    char buf[32];
    char *p;
    char *pad;
    char *s;
    int width;
    int size;
    long n;
    u_long u;
    u_long digit;
    float f;
    char c;

    while (*fmt) {
        pad = 0;
        size = 0;
        if (*fmt == '%') {
        again:
            fmt++;
            switch (*fmt) {
            case '0':
                width = fmt[1] - '0';
                c = fmt[2];
                if ((unsigned char)width < 10) {
                    if ((unsigned)(c - '0') < 10) {
                        width = width * 10 + (c - '0');
                        fmt += 2;
                        if (width > 31) {
                            width = 31;
                        }
                    } else {
                        fmt++;
                    }
                    pad = &buf[31 - width];
                    for (; width > 0; width--) {
                        buf[31 - width] = '0';
                    }
                }
                goto again;
            case 'l':
                size = 'l';
                goto again;
            case 'h':
                size = 'h';
                goto again;
            case 'o':
                if (size == 'l') {
                    u = va_arg(ap, u_long);
                } else if (size == 'h') {
                    u = va_arg(ap, u_short);
                } else {
                    u = va_arg(ap, u_int);
                }
                p = &buf[31];
                *p = 0;
                if (u == 0) {
                    *--p = '0';
                } else {
                    while (u) {
                        *--p = (u & 7) + '0';
                        u >>= 3;
                    }
                }
                if (pad && pad < p) {
                    p = pad;
                }
                while (*p) {
                    _putchar(*p++);
                }
                break;
            case 'x':
                if (size == 'l') {
                    u = va_arg(ap, u_long);
                } else if (size == 'h') {
                    u = va_arg(ap, u_short);
                } else {
                    u = va_arg(ap, u_int);
                }
                p = &buf[31];
                *p = 0;
                if (u == 0) {
                    *--p = '0';
                } else {
                    while (u) {
                        digit = u & 0xf;
                        if (digit >= 10) {
                            *--p = digit + 'a' - 10;
                        } else {
                            *--p = digit + '0';
                        }
                        u >>= 4;
                    }
                }
                if (pad && pad < p) {
                    p = pad;
                }
                while (*p) {
                    _putchar(*p++);
                }
                break;
            case 'd':
                if (size == 'l') {
                    n = va_arg(ap, long);
                } else if (size == 'h') {
                    n = va_arg(ap, short);
                } else {
                    n = va_arg(ap, int);
                }
                p = &buf[31];
                *p = 0;
                if (n == 0) {
                    *--p = '0';
                } else {
                    if (n < 0) {
                        n = -n;
                        _putchar('-');
                    }
                    while (n) {
                        *--p = n % 10 + '0';
                        n /= 10;
                    }
                }
                if (pad && pad < p) {
                    p = pad;
                }
                while (*p) {
                    _putchar(*p++);
                }
                break;
            case 'u':
                if (size == 'l') {
                    u = va_arg(ap, u_long);
                } else if (size == 'h') {
                    u = va_arg(ap, u_short);
                } else {
                    u = va_arg(ap, u_int);
                }
                p = &buf[31];
                *p = 0;
                if (u == 0) {
                    *--p = '0';
                } else {
                    while (u) {
                        *--p = u % 10 + '0';
                        u /= 10;
                    }
                }
                if (pad && pad < p) {
                    p = pad;
                }
                while (*p) {
                    _putchar(*p++);
                }
                break;
            case 'e':
            case 'f':
                f = va_arg(ap, float);
                if (f == 0.0f) {
                    _putchar('0');
                } else {
                    printfloat(f);
                }
                break;
            case 's':
                s = va_arg(ap, char *);
                if (*s == 0) {
                    _putchar('(');
                    _putchar('n');
                    _putchar('u');
                    _putchar('l');
                    _putchar('l');
                    _putchar(')');
                } else {
                    for (p = s; *p; p++) {
                        _putchar(*p);
                    }
                }
                break;
            case 'c':
                n = va_arg(ap, char);
                _putchar(n);
                break;
            }
        } else {
            _putchar(*fmt);
        }
        fmt++;
    }
}


void kprintf(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    _printf(fmt, ap);
    va_end(ap);
}


void scePrintf(const char *fmt, ...)
{
    va_list ap;
    void (*saved)(int c);

    saved = _putchar;
    _putchar = deci2Putchar;
    va_start(ap, fmt);
    _printf(fmt, ap);
    va_end(ap);
    _putchar = saved;
}
