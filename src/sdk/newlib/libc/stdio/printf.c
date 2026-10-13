/*
 * Unit: sdk/newlib/libc/stdio/printf
 *
 * Functions:
 *   0x00271000 _printf_r
 *   0x00271040 printf
 */

typedef char *va_list;
#define va_start(ap, last) ((ap) = (va_list)__builtin_next_arg(last) - (__builtin_args_info(2) < 8 ? (8 - __builtin_args_info(2)) * 8 : 0))
#define va_end(ap)

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
};

extern struct _reent *_impure_ptr[];

#define _REENT (_impure_ptr[0])

extern int _vfprintf_r(struct _reent *ptr, FILE *fp, const char *fmt, va_list ap);
extern int vfprintf(FILE *fp, const char *fmt, va_list ap);


int _printf_r(struct _reent *ptr, const char *fmt, ...)
{
    int ret;
    va_list ap;

    va_start(ap, fmt);
    ret = _vfprintf_r(ptr, ptr->_stdout, fmt, ap);
    va_end(ap);
    return ret;
}


int printf(const char *fmt, ...)
{
    struct _reent *ptr = _REENT;
    int ret;
    va_list ap;

    va_start(ap, fmt);
    ptr->_stdout->_data = ptr;
    ret = vfprintf(ptr->_stdout, fmt, ap);
    va_end(ap);
    return ret;
}
