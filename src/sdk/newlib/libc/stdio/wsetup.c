/*
 * Unit: sdk/newlib/libc/stdio/wsetup
 *
 * Functions:
 *   0x00274e20 __swsetup
 */

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

#define __SLBF 0x0001
#define __SNBF 0x0002
#define __SRD 0x0004
#define __SWR 0x0008
#define __SRW 0x0010
#define __SEOF 0x0020

#define EOF (-1)

#define _REENT (_impure_ptr[0])
#define CHECK_INIT(fp) do { if ((fp)->_data == 0) (fp)->_data = _REENT; if (!(fp)->_data->__sdidinit) __sinit((fp)->_data); } while (0)
#define HASUB(fp) ((fp)->_ub._base != 0)
#define FREEUB(fp) { if ((fp)->_ub._base != (fp)->_ubuf) _free_r((fp)->_data, (fp)->_ub._base); (fp)->_ub._base = 0; }

extern struct _reent *_impure_ptr[];

extern void __sinit(struct _reent *s);
extern void _free_r(struct _reent *ptr, void *p);
extern void __smakebuf(FILE *fp);


int __swsetup(FILE *fp)
{
    CHECK_INIT(fp);

    if ((fp->_flags & __SWR) == 0) {
        if ((fp->_flags & __SRW) == 0) {
            return EOF;
        }
        if (fp->_flags & __SRD) {
            if (HASUB(fp)) {
                FREEUB(fp);
            }
            fp->_flags &= ~(__SRD | __SEOF);
            fp->_r = 0;
            fp->_p = fp->_bf._base;
        }
        fp->_flags |= __SWR;
    }

    if (fp->_bf._base == 0) {
        __smakebuf(fp);
    }
    if (fp->_flags & __SLBF) {
        fp->_w = 0;
        fp->_lbfsize = -fp->_bf._size;
    } else {
        fp->_w = fp->_flags & __SNBF ? 0 : fp->_bf._size;
    }
    return 0;
}
