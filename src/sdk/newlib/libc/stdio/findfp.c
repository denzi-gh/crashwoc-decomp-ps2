/*
 * Unit: sdk/newlib/libc/stdio/findfp
 *
 * Functions:
 *   0x00276420 std
 *   0x00276478 __sfmoreglue
 *   0x002764e8 __sfp
 *   0x002765c8 _cleanup_r
 *   0x002765d8 _cleanup
 *   0x002765e8 __sinit
 */

#define _ATEXIT_SIZE 32
#define _N_LISTS 30

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

struct _glue {
    struct _glue *_next;
    int _niobs;
    FILE *_iobs;
};

struct _atexit {
    struct _atexit *_next;
    int _ind;
    void (*_fns[_ATEXIT_SIZE])(void);
};

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
    void (*__cleanup)(struct _reent *);
    struct _Bigint *_result;
    int _result_k;
    struct _Bigint *_p5s;
    struct _Bigint **_freelist;
    int _cvtlen;
    char *_cvtbuf;
    union {
        struct {
            unsigned char *_nextf[_N_LISTS];
            unsigned int _nmalloc[_N_LISTS];
        } _unused;
    } _new;
    struct _atexit *_atexit;
    struct _atexit _atexit0;
    void (**_sig_func)(int);
    struct _glue __sglue;
    FILE __sf[3];
};

#define __SLBF 0x0001
#define __SNBF 0x0002
#define __SRD 0x0004
#define __SWR 0x0008

#define ENOMEM 12
#define NDYNAMIC 4

#define _REENT (_impure_ptr[0])

extern struct _reent *_impure_ptr[];

extern int __sread(void *cookie, char *buf, int n);
extern int __swrite(void *cookie, const char *buf, int n);
extern long __sseek(void *cookie, long offset, int whence);
extern int __sclose(void *cookie);
extern void *_malloc_r(struct _reent *ptr, unsigned int size);
extern void *memset(void *s, int c, unsigned int n);
extern int fflush(FILE *fp);
extern int _fwalk(struct _reent *ptr, int (*function)(FILE *));

void __sinit(struct _reent *s);


static void std(FILE *ptr, int flags, int file, struct _reent *data)
{
    ptr->_p = 0;
    ptr->_r = 0;
    ptr->_w = 0;
    ptr->_flags = flags;
    ptr->_file = file;
    ptr->_bf._base = 0;
    ptr->_lbfsize = 0;
    ptr->_cookie = ptr;
    ptr->_read = __sread;
    ptr->_write = __swrite;
    ptr->_seek = __sseek;
    ptr->_close = __sclose;
    ptr->_data = data;
}


struct _glue *__sfmoreglue(struct _reent *d, int n)
{
    struct _glue *g;
    FILE *p;

    g = (struct _glue *)_malloc_r(d, sizeof(*g) + n * sizeof(FILE));
    if (g == 0)
        return 0;
    p = (FILE *)(g + 1);
    g->_next = 0;
    g->_niobs = n;
    g->_iobs = p;
    memset(p, 0, n * sizeof(FILE));
    return g;
}


FILE *__sfp(struct _reent *d)
{
    FILE *fp;
    int n;
    struct _glue *g;

    if (!d->__sdidinit)
        __sinit(d);
    for (g = &d->__sglue;; g = g->_next) {
        for (fp = g->_iobs, n = g->_niobs; --n >= 0; fp++)
            if (fp->_flags == 0)
                goto found;
        if (g->_next == 0 && (g->_next = __sfmoreglue(d, NDYNAMIC)) == 0)
            break;
    }
    d->_errno = ENOMEM;
    return 0;

found:
    fp->_flags = 1;
    fp->_p = 0;
    fp->_w = 0;
    fp->_r = 0;
    fp->_bf._base = 0;
    fp->_bf._size = 0;
    fp->_lbfsize = 0;
    fp->_file = -1;
    fp->_ub._base = 0;
    fp->_ub._size = 0;
    fp->_lb._base = 0;
    fp->_lb._size = 0;
    fp->_data = d;
    return fp;
}


void _cleanup_r(struct _reent *ptr)
{
    _fwalk(ptr, fflush);
}


void _cleanup(void)
{
    _cleanup_r(_REENT);
}


void __sinit(struct _reent *s)
{
    s->__cleanup = _cleanup_r;
    s->__sdidinit = 1;
    std(s->__sf + 0, __SRD, 0, s);
    std(s->__sf + 1, __SWR | __SLBF, 1, s);
    std(s->__sf + 2, __SWR | __SNBF, 2, s);
    s->__sglue._next = 0;
    s->__sglue._niobs = 3;
    s->__sglue._iobs = &s->__sf[0];
}
