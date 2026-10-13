/*
 * Unit: sdk/newlib/libc/reent/lseekr
 *
 * Functions:
 *   0x00278788 _lseek_r
 */

struct _reent {
    int _errno;
};

extern int errno[];

extern long lseek(int fd, long offset, int whence);


long _lseek_r(struct _reent *ptr, int fd, long offset, int whence)
{
    long ret;

    errno[0] = 0;
    ret = lseek(fd, offset, whence);
    if (ret == -1 && errno[0] != 0) {
        ptr->_errno = errno[0];
    }
    return ret;
}
