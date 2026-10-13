/*
 * Unit: sdk/newlib/libc/stdio/makebuf
 *
 * Functions:
 *   0x00277068 __smakebuf
 */

#define BUFSIZ 1024

#define __SLBF 0x0001
#define __SNBF 0x0002
#define __SMBF 0x0080
#define __SOPT 0x0400
#define __SNPT 0x0800

#define S_IFMT 0170000
#define S_IFCHR 0020000
#define S_IFREG 0100000

typedef unsigned int size_t;

struct __sbuf {
    unsigned char *_base;
    int _size;
};

struct _reent;

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
    void (*__cleanup)(struct _reent *);
};

struct stat {
    short st_dev;
    short st_ino;
    unsigned int st_mode;
    short st_nlink;
    short st_uid;
    short st_gid;
    short st_rdev;
    long st_size;
    long st_atime;
    long st_spare1;
    long st_mtime;
    long st_spare2;
    long st_ctime;
    long st_spare3;
    long st_blksize;
    long st_blocks;
    long st_spare4[2];
};

extern int _fstat_r(struct _reent *ptr, int fd, struct stat *st);
extern void *_malloc_r(struct _reent *ptr, size_t size);
extern int isatty(int fd);
extern long __sseek(void *cookie, long offset, int whence);
extern void _cleanup_r(struct _reent *ptr);


void __smakebuf(FILE *fp)
{
    size_t size, couldbetty;
    void *p;
    struct stat st;

    if (fp->_flags & __SNBF) {
        fp->_bf._base = fp->_p = fp->_nbuf;
        fp->_bf._size = 1;
        return;
    }
    if (fp->_file < 0 || _fstat_r(fp->_data, fp->_file, &st) < 0) {
        couldbetty = 0;
        size = BUFSIZ;
        fp->_flags |= __SNPT;
    } else {
        couldbetty = (st.st_mode & S_IFMT) == S_IFCHR;
        size = BUFSIZ;
        if ((st.st_mode & S_IFMT) == S_IFREG && fp->_seek == __sseek) {
            fp->_flags |= __SOPT;
            fp->_blksize = 1024;
        } else {
            fp->_flags |= __SNPT;
        }
    }
    if ((p = _malloc_r(fp->_data, size)) == 0) {
        fp->_flags |= __SNBF;
        fp->_bf._base = fp->_p = fp->_nbuf;
        fp->_bf._size = 1;
    } else {
        fp->_data->__cleanup = _cleanup_r;
        fp->_flags |= __SMBF;
        fp->_bf._base = fp->_p = p;
        fp->_bf._size = size;
        if (couldbetty && isatty(fp->_file)) {
            fp->_flags |= __SLBF;
        }
    }
}
