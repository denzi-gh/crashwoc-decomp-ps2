/*
 * Unit: sdk/newlib/libc/stdlib/rand
 *
 * Functions:
 *   0x00271090 srand
 *   0x002710a0 rand
 */

#define RAND_MAX 0x7FFFFFFF

struct _reent {
    int _errno;
    void *_stdin;
    void *_stdout;
    void *_stderr;
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
            unsigned int _rand_next;
        } _reent;
    } _new;
};

extern struct _reent *_impure_ptr[];

#define _REENT (_impure_ptr[0])


void srand(unsigned int seed)
{
    _REENT->_new._reent._rand_next = seed;
}


int rand(void)
{
    struct _reent *reent = _REENT;

    reent->_new._reent._rand_next = reent->_new._reent._rand_next * 1103515245 + 12345;
    return reent->_new._reent._rand_next & RAND_MAX;
}
