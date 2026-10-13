/*
 * Unit: sdk/newlib/libc/reent/readr
 *
 * Functions:
 *   0x002787e8 _read_r
 */

struct _reent {
    int _errno;
};

extern int errno[];

extern int read(int fd, void *buf, unsigned int count);


int _read_r(struct _reent *ptr, int fd, void *buf, unsigned int count)
{
    int ret;

    errno[0] = 0;
    ret = read(fd, buf, count);
    if (ret == -1 && errno[0] != 0) {
        ptr->_errno = errno[0];
    }
    return ret;
}
