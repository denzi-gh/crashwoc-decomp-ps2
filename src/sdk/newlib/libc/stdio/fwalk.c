/*
 * Unit: sdk/newlib/libc/stdio/fwalk
 *
 * Functions:
 *   0x00276ee0 _fwalk
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


int _fwalk(struct _reent *ptr, int (*function)(FILE *))
{
    FILE *fp;
    int n, ret = 0;
    struct _glue *g;

    for (g = &ptr->__sglue; g != 0; g = g->_next) {
        for (fp = g->_iobs, n = g->_niobs; --n >= 0; fp++) {
            if (fp->_flags != 0) {
                ret |= (*function)(fp);
            }
        }
    }
    return ret;
}
