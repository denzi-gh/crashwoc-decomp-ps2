/*
 * Unit: sdk/newlib/libc/reent/impure
 *
 * Data only (no functions): the single reentrancy structure and the pointer
 * every newlib routine reaches it through. Layouts from the unit's STABS.
 */

struct __sbuf {
    unsigned char *_base;
    int _size;
};

struct _reent;

struct __sFILE {
    unsigned char *_p;
    int _r;
    int _w;
    short _flags;
    short _file;
    struct __sbuf _bf;
    int _lbfsize;
    void *_cookie;
    int (*_read)();
    int (*_write)();
    long (*_seek)();
    int (*_close)();
    struct __sbuf _ub;
    unsigned char *_up;
    int _ur;
    unsigned char _ubuf[3];
    unsigned char _nbuf[1];
    struct __sbuf _lb;
    int _blksize;
    int _offset;
    struct _reent *_data;
};

struct _glue {
    struct _glue *_next;
    int _niobs;
    struct __sFILE *_iobs;
};

struct _atexit {
    struct _atexit *_next;
    int _ind;
    void (*_fns[32])();
};

struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};

struct _Bigint;

struct _reent {
    int _errno;
    struct __sFILE *_stdin, *_stdout, *_stderr;
    int _inc;
    char _emergency[25];
    int _current_category;
    const char *_current_locale;
    int __sdidinit;
    void (*__cleanup)();
    struct _Bigint *_result;
    int _result_k;
    struct _Bigint *_p5s;
    struct _Bigint **_freelist;
    int _cvtlen;
    char *_cvtbuf;
    union {
        struct {
            unsigned int _rand_next;
            char *_strtok_last;
            char _asctime_buf[26];
            struct tm _localtime_buf;
            int _gamma_signgam;
        } _reent;
        struct {
            unsigned char *_nextf[30];
            unsigned int _nmalloc[30];
        } _unused;
    } _new;
    struct _atexit *_atexit;
    struct _atexit _atexit0;
    void (**_sig_func)();
    struct _glue __sglue;
    struct __sFILE __sf[3];
};

#define _REENT_INIT(var) \
    { 0, &var.__sf[0], &var.__sf[1], &var.__sf[2], 0, "", 0, "C", \
      0, 0, 0, 0, 0, 0, 0, 0, { { 1, 0, "", { 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 0 } } }

static struct _reent impure_data = _REENT_INIT(impure_data);
struct _reent *_impure_ptr = &impure_data;
