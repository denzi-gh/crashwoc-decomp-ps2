/*
 * Unit: sdk/newlib/libc/reent/fstatr
 *
 * Functions:
 *   0x00278728 _fstat_r
 */

struct _reent {
    int _errno;
};

struct stat;

extern int errno[];

extern int fstat(int fd, struct stat *st);


int _fstat_r(struct _reent *ptr, int fd, struct stat *st)
{
    int ret;

    errno[0] = 0;
    ret = fstat(fd, st);
    if (ret == -1 && errno[0] != 0) {
        ptr->_errno = errno[0];
    }
    return ret;
}
