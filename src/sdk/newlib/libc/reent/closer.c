/*
 * Unit: sdk/newlib/libc/reent/closer
 *
 * Functions:
 *   0x002786d0 _close_r
 */

struct _reent {
    int _errno;
};

extern int errno[];

extern int close(int fd);


int _close_r(struct _reent *ptr, int fd)
{
    int ret;

    errno[0] = 0;
    ret = close(fd);
    if (ret == -1 && errno[0] != 0) {
        ptr->_errno = errno[0];
    }
    return ret;
}
