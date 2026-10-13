/*
 * Unit: sdk/newlib/libc/reent/sbrkr
 *
 * Functions:
 *   0x00271670 _sbrk_r
 */

struct _reent {
    int _errno;
};

extern int errno[];

extern void *sbrk(int increment);


void *_sbrk_r(struct _reent *ptr, int increment)
{
    void *ret;

    errno[0] = 0;
    ret = sbrk(increment);
    if (ret == (void *)-1 && errno[0] != 0) {
        ptr->_errno = errno[0];
    }
    return ret;
}
