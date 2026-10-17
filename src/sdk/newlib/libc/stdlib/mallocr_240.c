/*
 * Unit: sdk/newlib/libc/stdlib/mallocr_240
 *
 * Functions:
 *   0x00278610 _calloc_r
 */

typedef unsigned int size_t;

struct _reent;

#define INTERNAL_SIZE_T size_t
#define SIZE_SZ (sizeof(INTERNAL_SIZE_T))

struct malloc_chunk {
    INTERNAL_SIZE_T prev_size;
    INTERNAL_SIZE_T size;
    struct malloc_chunk *fd;
    struct malloc_chunk *bk;
};

typedef struct malloc_chunk *mchunkptr;

#define mem2chunk(mem) ((mchunkptr)((char *)(mem) - 2 * SIZE_SZ))

#define PREV_INUSE 0x1
#define IS_MMAPPED 0x2
#define SIZE_BITS (PREV_INUSE | IS_MMAPPED)

#define chunksize(p) ((p)->size & ~(SIZE_BITS))

#define MALLOC_ZERO(charp, nbytes) \
do { \
    INTERNAL_SIZE_T mzsz = (nbytes); \
    if (mzsz <= 9 * sizeof(mzsz)) { \
        INTERNAL_SIZE_T *mz = (INTERNAL_SIZE_T *)(charp); \
        if (mzsz >= 5 * sizeof(mzsz)) { \
            *mz++ = 0; \
            *mz++ = 0; \
            if (mzsz >= 7 * sizeof(mzsz)) { \
                *mz++ = 0; \
                *mz++ = 0; \
                if (mzsz >= 9 * sizeof(mzsz)) { \
                    *mz++ = 0; \
                    *mz++ = 0; \
                } \
            } \
        } \
        *mz++ = 0; \
        *mz++ = 0; \
        *mz = 0; \
    } else { \
        memset((charp), 0, mzsz); \
    } \
} while (0)

extern void *memset(void *s, int c, size_t n);
extern void *_malloc_r(struct _reent *reent_ptr, size_t bytes);


void *_calloc_r(struct _reent *reent_ptr, size_t n, size_t elem_size)
{
    mchunkptr p;
    INTERNAL_SIZE_T csz;
    INTERNAL_SIZE_T sz = n * elem_size;
    void *mem = _malloc_r(reent_ptr, sz);

    if (mem == 0) {
        return 0;
    } else {
        p = mem2chunk(mem);
        csz = chunksize(p);
        MALLOC_ZERO(mem, csz - SIZE_SZ);
        return mem;
    }
}
