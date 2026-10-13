/*
 * Unit: sdk/newlib/libc/reent/writer
 *
 * Functions:
 *   0x002785b0 _write_r
 */

struct _reent {
    int _errno;
};

extern int errno[];

extern int write(int fd, const void *buf, unsigned int count);


int _write_r(struct _reent *ptr, int fd, const void *buf, unsigned int count)
{
    int ret;

    errno[0] = 0;
    ret = write(fd, buf, count);
    if (ret == -1 && errno[0] != 0) {
        ptr->_errno = errno[0];
    }
    return ret;
}
