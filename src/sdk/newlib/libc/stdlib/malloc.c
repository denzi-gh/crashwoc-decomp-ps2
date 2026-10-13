/*
 * Unit: sdk/newlib/libc/stdlib/malloc
 *
 * Functions:
 *   0x002703e0 malloc
 *   0x00270408 free
 */

typedef unsigned int size_t;

struct _reent;

extern struct _reent *_impure_ptr[];

#define _REENT (_impure_ptr[0])

extern void *_malloc_r(struct _reent *reent, size_t nbytes);
extern void _free_r(struct _reent *reent, void *aptr);


void *malloc(size_t nbytes)
{
    return _malloc_r(_REENT, nbytes);
}


void free(void *aptr)
{
    _free_r(_REENT, aptr);
}
