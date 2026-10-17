/*
 * Unit: sdk/newlib/libc/stdlib/mallocr_228
 *
 * Functions:
 *   0x00276678 _free_r
 *   0x00276998 _malloc_trim_r
 */

typedef unsigned int size_t;
typedef int ptrdiff_t;

struct _reent;

#define INTERNAL_SIZE_T size_t
#define SIZE_SZ (sizeof(INTERNAL_SIZE_T))
#define MALLOC_ALIGNMENT 16
#define MALLOC_ALIGN_MASK (MALLOC_ALIGNMENT - 1)

struct malloc_chunk {
    INTERNAL_SIZE_T prev_size;
    INTERNAL_SIZE_T size;
    struct malloc_chunk *fd;
    struct malloc_chunk *bk;
};

typedef struct malloc_chunk *mchunkptr;
typedef struct malloc_chunk *mbinptr;

struct mallinfo {
    int arena;
    int ordblks;
    int smblks;
    int hblks;
    int hblkhd;
    int usmblks;
    int fsmblks;
    int uordblks;
    int fordblks;
    int keepcost;
};

#define MINSIZE (sizeof(struct malloc_chunk))

#define mem2chunk(mem) ((mchunkptr)((char *)(mem) - 2 * SIZE_SZ))

#define PREV_INUSE 0x1
#define IS_MMAPPED 0x2
#define SIZE_BITS (PREV_INUSE | IS_MMAPPED)

#define chunksize(p) ((p)->size & ~(SIZE_BITS))
#define chunk_at_offset(p, s) ((mchunkptr)(((char *)(p)) + (s)))
#define inuse_bit_at_offset(p, s) (((mchunkptr)(((char *)(p)) + (s)))->size & PREV_INUSE)

#define set_head(p, s) ((p)->size = (s))
#define set_foot(p, s) (((mchunkptr)((char *)(p) + (s)))->prev_size = (s))

#define NAV 128

#define av_ __malloc_av_
#define bin_at(i) ((mbinptr)((char *)&(av_[2 * (i) + 2]) - 2 * SIZE_SZ))

#define top (bin_at(0)->fd)
#define last_remainder (bin_at(1))

#define MAX_SMALLBIN_SIZE 512
#define SMALLBIN_WIDTH 8

#define bin_index(sz) \
(((((unsigned long)(sz)) >> 9) == 0) ? (((unsigned long)(sz)) >> 3) : \
 ((((unsigned long)(sz)) >> 9) <= 4) ? 56 + (((unsigned long)(sz)) >> 6) : \
 ((((unsigned long)(sz)) >> 9) <= 20) ? 91 + (((unsigned long)(sz)) >> 9) : \
 ((((unsigned long)(sz)) >> 9) <= 84) ? 110 + (((unsigned long)(sz)) >> 12) : \
 ((((unsigned long)(sz)) >> 9) <= 340) ? 119 + (((unsigned long)(sz)) >> 15) : \
 ((((unsigned long)(sz)) >> 9) <= 1364) ? 124 + (((unsigned long)(sz)) >> 18) : \
 126)

#define smallbin_index(sz) (((unsigned long)(sz)) >> 3)

#define binblocks (bin_at(0)->size)
#define BINBLOCKWIDTH 4
#define idx2binblock(ix) ((unsigned long)1 << (ix / BINBLOCKWIDTH))
#define mark_binblock(ii) (binblocks |= idx2binblock(ii))

#define frontlink(P, S, IDX, BK, FD) \
{ \
    if (S < MAX_SMALLBIN_SIZE) { \
        IDX = smallbin_index(S); \
        mark_binblock(IDX); \
        BK = bin_at(IDX); \
        FD = BK->fd; \
        P->bk = BK; \
        P->fd = FD; \
        FD->bk = BK->fd = P; \
    } else { \
        IDX = bin_index(S); \
        BK = bin_at(IDX); \
        FD = BK->fd; \
        if (FD == BK) { \
            mark_binblock(IDX); \
        } else { \
            while (FD != BK && S < chunksize(FD)) { \
                FD = FD->fd; \
            } \
            BK = FD->bk; \
        } \
        P->bk = BK; \
        P->fd = FD; \
        FD->bk = BK->fd = P; \
    } \
}

#define unlink(P, BK, FD) \
{ \
    BK = P->bk; \
    FD = P->fd; \
    FD->bk = BK; \
    BK->fd = FD; \
}

#define link_last_remainder(P) \
{ \
    last_remainder->fd = last_remainder->bk = P; \
    P->fd = P->bk = last_remainder; \
}

#define MORECORE(size) _sbrk_r(reent_ptr, (size))
#define MORECORE_FAILURE (-1)

#define MALLOC_LOCK __malloc_lock(reent_ptr)
#define MALLOC_UNLOCK __malloc_unlock(reent_ptr)

#define malloc_getpagesize (4096)

#define trim_threshold __malloc_trim_threshold
#define top_pad __malloc_top_pad
#define sbrk_base __malloc_sbrk_base
#define current_mallinfo __malloc_current_mallinfo
#define sbrked_mem (current_mallinfo.arena)

extern mbinptr __malloc_av_[NAV * 2 + 2];
extern unsigned long __malloc_trim_threshold;
extern unsigned long __malloc_top_pad;
extern char *__malloc_sbrk_base;
extern struct mallinfo __malloc_current_mallinfo;

extern void __malloc_lock(struct _reent *reent_ptr);
extern void __malloc_unlock(struct _reent *reent_ptr);
extern void *_sbrk_r(struct _reent *reent_ptr, ptrdiff_t incr);

int _malloc_trim_r(struct _reent *reent_ptr, size_t pad);


void _free_r(struct _reent *reent_ptr, void *mem)
{
    mchunkptr p;
    INTERNAL_SIZE_T hd;
    INTERNAL_SIZE_T sz;
    int idx;
    mchunkptr next;
    INTERNAL_SIZE_T nextsz;
    INTERNAL_SIZE_T prevsz;
    mchunkptr bck;
    mchunkptr fwd;
    int islr;

    if (mem == 0) {
        return;
    }

    MALLOC_LOCK;

    p = mem2chunk(mem);
    hd = p->size;

    sz = hd & ~PREV_INUSE;
    next = chunk_at_offset(p, sz);
    nextsz = chunksize(next);

    if (next == top) {
        sz += nextsz;

        if (!(hd & PREV_INUSE)) {
            prevsz = p->prev_size;
            p = chunk_at_offset(p, -prevsz);
            sz += prevsz;
            unlink(p, bck, fwd);
        }

        set_head(p, sz | PREV_INUSE);
        top = p;
        if ((unsigned long)(sz) >= (unsigned long)trim_threshold) {
            _malloc_trim_r(reent_ptr, top_pad);
        }
        MALLOC_UNLOCK;
        return;
    }

    set_head(next, nextsz);

    islr = 0;

    if (!(hd & PREV_INUSE)) {
        prevsz = p->prev_size;
        p = chunk_at_offset(p, -prevsz);
        sz += prevsz;

        if (p->fd == last_remainder) {
            islr = 1;
        } else {
            unlink(p, bck, fwd);
        }
    }

    if (!(inuse_bit_at_offset(next, nextsz))) {
        sz += nextsz;

        if (!islr && next->fd == last_remainder) {
            islr = 1;
            link_last_remainder(p);
        } else {
            unlink(next, bck, fwd);
        }
    }

    set_head(p, sz | PREV_INUSE);
    set_foot(p, sz);
    if (!islr) {
        frontlink(p, sz, idx, bck, fwd);
    }

    MALLOC_UNLOCK;
}


int _malloc_trim_r(struct _reent *reent_ptr, size_t pad)
{
    long top_size;
    long extra;
    char *current_brk;
    char *new_brk;
    unsigned long pagesz = malloc_getpagesize;

    MALLOC_LOCK;

    top_size = chunksize(top);
    extra = ((top_size - pad - MINSIZE + (pagesz - 1)) / pagesz - 1) * pagesz;

    if (extra < (long)pagesz) {
        MALLOC_UNLOCK;
        return 0;
    } else {
        current_brk = (char *)(MORECORE(0));
        if (current_brk != (char *)(top) + top_size) {
            MALLOC_UNLOCK;
            return 0;
        } else {
            new_brk = (char *)(MORECORE(-extra));

            if (new_brk == (char *)(MORECORE_FAILURE)) {
                current_brk = (char *)(MORECORE(0));
                top_size = current_brk - (char *)top;
                if (top_size >= (long)MINSIZE) {
                    sbrked_mem = current_brk - sbrk_base;
                    set_head(top, top_size | PREV_INUSE);
                }
                MALLOC_UNLOCK;
                return 0;
            } else {
                set_head(top, (top_size - extra) | PREV_INUSE);
                sbrked_mem -= extra;
                MALLOC_UNLOCK;
                return 1;
            }
        }
    }
}
