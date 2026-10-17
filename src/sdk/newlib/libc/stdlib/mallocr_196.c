/*
 * Unit: sdk/newlib/libc/stdlib/mallocr_196
 *
 * Functions:
 *   0x00270418 malloc_extend_top
 *   0x00270670 _malloc_r
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

#define chunk2mem(p) ((void *)&(p)->fd)

#define request2size(req) \
    (((unsigned long)((req) + (SIZE_SZ + MALLOC_ALIGN_MASK)) < \
      (unsigned long)(MINSIZE + MALLOC_ALIGN_MASK)) ? MINSIZE : \
     (((req) + (SIZE_SZ + MALLOC_ALIGN_MASK)) & ~(MALLOC_ALIGN_MASK)))

#define PREV_INUSE 0x1
#define IS_MMAPPED 0x2
#define SIZE_BITS (PREV_INUSE | IS_MMAPPED)

#define chunksize(p) ((p)->size & ~(SIZE_BITS))
#define chunk_at_offset(p, s) ((mchunkptr)(((char *)(p)) + (s)))
#define set_inuse_bit_at_offset(p, s) (((mchunkptr)(((char *)(p)) + (s)))->size |= PREV_INUSE)

#define set_head_size(p, s) ((p)->size = (((p)->size & PREV_INUSE) | (s)))
#define set_head(p, s) ((p)->size = (s))
#define set_foot(p, s) (((mchunkptr)((char *)(p) + (s)))->prev_size = (s))

#define NAV 128

#define av_ __malloc_av_
#define bin_at(i) ((mbinptr)((char *)&(av_[2 * (i) + 2]) - 2 * SIZE_SZ))
#define next_bin(b) ((mbinptr)((mbinptr *)(b) + 2))
#define prev_bin(b) ((mbinptr)((char *)(b) - 2 * sizeof(mbinptr)))

#define first(b) ((b)->fd)
#define last(b) ((b)->bk)

#define top (bin_at(0)->fd)
#define last_remainder (bin_at(1))
#define initial_top ((mchunkptr)(bin_at(0)))

#define MAX_SMALLBIN 63
#define MAX_SMALLBIN_SIZE 512
#define SMALLBIN_WIDTH 8

#define is_small_request(nb) (nb < MAX_SMALLBIN_SIZE - SMALLBIN_WIDTH)

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

#define clear_last_remainder (last_remainder->fd = last_remainder->bk = last_remainder)

#define long_sub_size_t(x, y) \
    (sizeof(long) > sizeof(INTERNAL_SIZE_T) && (x) < (y) ? -(long)((y) - (x)) : (long)((x) - (y)))

#define MORECORE(size) _sbrk_r(reent_ptr, (size))
#define MORECORE_FAILURE (-1)

#define MALLOC_LOCK __malloc_lock(reent_ptr)
#define MALLOC_UNLOCK __malloc_unlock(reent_ptr)

#define malloc_getpagesize (4096)

#define top_pad __malloc_top_pad
#define sbrk_base __malloc_sbrk_base
#define max_sbrked_mem __malloc_max_sbrked_mem
#define max_total_mem __malloc_max_total_mem
#define current_mallinfo __malloc_current_mallinfo
#define sbrked_mem (current_mallinfo.arena)

extern mbinptr __malloc_av_[NAV * 2 + 2];
extern unsigned long __malloc_top_pad;
extern char *__malloc_sbrk_base;
extern unsigned long __malloc_max_sbrked_mem;
extern unsigned long __malloc_max_total_mem;
extern struct mallinfo __malloc_current_mallinfo;

extern void __malloc_lock(struct _reent *reent_ptr);
extern void __malloc_unlock(struct _reent *reent_ptr);
extern void *_sbrk_r(struct _reent *reent_ptr, ptrdiff_t incr);
extern void _free_r(struct _reent *reent_ptr, void *mem);


static void malloc_extend_top(struct _reent *reent_ptr, INTERNAL_SIZE_T nb)
{
    char *brk;
    INTERNAL_SIZE_T front_misalign;
    INTERNAL_SIZE_T correction;
    char *new_brk;
    INTERNAL_SIZE_T top_size;

    mchunkptr old_top = top;
    INTERNAL_SIZE_T old_top_size = chunksize(old_top);
    char *old_end = (char *)(chunk_at_offset(old_top, old_top_size));

    INTERNAL_SIZE_T sbrk_size = nb + top_pad + MINSIZE;
    unsigned long pagesz = malloc_getpagesize;

    if (sbrk_base != (char *)(-1)) {
        sbrk_size = (sbrk_size + (pagesz - 1)) & ~(pagesz - 1);
    }

    brk = (char *)(MORECORE(sbrk_size));

    if (brk == (char *)(MORECORE_FAILURE) || (brk < old_end && old_top != initial_top)) {
        return;
    }

    sbrked_mem += sbrk_size;

    if (brk == old_end) {
        top_size = sbrk_size + old_top_size;
        set_head(top, top_size | PREV_INUSE);
    } else {
        if (sbrk_base == (char *)(-1)) {
            sbrk_base = brk;
        } else {
            sbrked_mem += brk - (char *)old_end;
        }

        front_misalign = (unsigned long)chunk2mem((mchunkptr)brk) & MALLOC_ALIGN_MASK;
        if (front_misalign > 0) {
            correction = (MALLOC_ALIGNMENT) - front_misalign;
            brk += correction;
        } else {
            correction = 0;
        }

        correction += pagesz - ((unsigned long)(brk + sbrk_size) & (pagesz - 1));

        new_brk = (char *)(MORECORE(correction));
        if (new_brk == (char *)(MORECORE_FAILURE)) {
            return;
        }

        sbrked_mem += correction;

        top = (mchunkptr)brk;
        top_size = new_brk - brk + correction;
        set_head(top, top_size | PREV_INUSE);

        if (old_top != initial_top) {
            if (old_top_size < MINSIZE) {
                set_head(top, PREV_INUSE);
                return;
            }

            old_top_size = (old_top_size - 3 * SIZE_SZ) & ~MALLOC_ALIGN_MASK;
            set_head_size(old_top, old_top_size);
            chunk_at_offset(old_top, old_top_size)->size = SIZE_SZ | PREV_INUSE;
            chunk_at_offset(old_top, old_top_size + SIZE_SZ)->size = SIZE_SZ | PREV_INUSE;
            if (old_top_size >= MINSIZE) {
                _free_r(reent_ptr, chunk2mem(old_top));
            }
        }
    }

    if ((unsigned long)sbrked_mem > (unsigned long)max_sbrked_mem) {
        max_sbrked_mem = sbrked_mem;
    }
    if ((unsigned long)(sbrked_mem) > (unsigned long)max_total_mem) {
        max_total_mem = sbrked_mem;
    }
}


void *_malloc_r(struct _reent *reent_ptr, size_t bytes)
{
    mchunkptr victim;
    INTERNAL_SIZE_T victim_size;
    int idx;
    mbinptr bin;
    mchunkptr remainder;
    long remainder_size;
    int remainder_index;
    unsigned long block;
    int startidx;
    mchunkptr fwd;
    mchunkptr bck;
    mbinptr q;

    INTERNAL_SIZE_T nb = request2size(bytes);

    MALLOC_LOCK;

    if (is_small_request(nb)) {
        idx = smallbin_index(nb);

        q = bin_at(idx);
        victim = last(q);

        if (victim != q) {
            victim_size = chunksize(victim);
            unlink(victim, bck, fwd);
            set_inuse_bit_at_offset(victim, victim_size);
            MALLOC_UNLOCK;
            return chunk2mem(victim);
        }

        idx += 2;
    } else {
        idx = bin_index(nb);
        bin = bin_at(idx);

        for (victim = last(bin); victim != bin; victim = victim->bk) {
            victim_size = chunksize(victim);
            remainder_size = long_sub_size_t(victim_size, nb);

            if (remainder_size >= (long)MINSIZE) {
                --idx;
                break;
            } else if (remainder_size >= 0) {
                unlink(victim, bck, fwd);
                set_inuse_bit_at_offset(victim, victim_size);
                MALLOC_UNLOCK;
                return chunk2mem(victim);
            }
        }

        ++idx;
    }

    if ((victim = last_remainder->fd) != last_remainder) {
        victim_size = chunksize(victim);
        remainder_size = long_sub_size_t(victim_size, nb);

        if (remainder_size >= (long)MINSIZE) {
            remainder = chunk_at_offset(victim, nb);
            set_head(victim, nb | PREV_INUSE);
            link_last_remainder(remainder);
            set_head(remainder, remainder_size | PREV_INUSE);
            set_foot(remainder, remainder_size);
            MALLOC_UNLOCK;
            return chunk2mem(victim);
        }

        clear_last_remainder;

        if (remainder_size >= 0) {
            set_inuse_bit_at_offset(victim, victim_size);
            MALLOC_UNLOCK;
            return chunk2mem(victim);
        }

        frontlink(victim, victim_size, remainder_index, bck, fwd);
    }

    if ((block = idx2binblock(idx)) <= binblocks) {
        if ((block & binblocks) == 0) {
            idx = (idx & ~(BINBLOCKWIDTH - 1)) + BINBLOCKWIDTH;
            block <<= 1;
            while ((block & binblocks) == 0) {
                idx += BINBLOCKWIDTH;
                block <<= 1;
            }
        }

        for (;;) {
            startidx = idx;
            q = bin = bin_at(idx);

            do {
                for (victim = last(bin); victim != bin; victim = victim->bk) {
                    victim_size = chunksize(victim);
                    remainder_size = long_sub_size_t(victim_size, nb);

                    if (remainder_size >= (long)MINSIZE) {
                        remainder = chunk_at_offset(victim, nb);
                        set_head(victim, nb | PREV_INUSE);
                        unlink(victim, bck, fwd);
                        link_last_remainder(remainder);
                        set_head(remainder, remainder_size | PREV_INUSE);
                        set_foot(remainder, remainder_size);
                        MALLOC_UNLOCK;
                        return chunk2mem(victim);
                    } else if (remainder_size >= 0) {
                        set_inuse_bit_at_offset(victim, victim_size);
                        unlink(victim, bck, fwd);
                        MALLOC_UNLOCK;
                        return chunk2mem(victim);
                    }
                }

                bin = next_bin(bin);

                if (idx < MAX_SMALLBIN) {
                    bin = next_bin(bin);
                    ++idx;
                }
            } while ((++idx & (BINBLOCKWIDTH - 1)) != 0);

            do {
                if ((startidx & (BINBLOCKWIDTH - 1)) == 0) {
                    binblocks &= ~block;
                    break;
                }
                --startidx;
                q = prev_bin(q);
            } while (first(q) == q);

            if ((block <<= 1) <= binblocks && (block != 0)) {
                while ((block & binblocks) == 0) {
                    idx += BINBLOCKWIDTH;
                    block <<= 1;
                }
            } else {
                break;
            }
        }
    }

    remainder_size = long_sub_size_t(chunksize(top), nb);
    if (chunksize(top) < nb || remainder_size < (long)MINSIZE) {
        malloc_extend_top(reent_ptr, nb);
        remainder_size = long_sub_size_t(chunksize(top), nb);
        if (chunksize(top) < nb || remainder_size < (long)MINSIZE) {
            MALLOC_UNLOCK;
            return 0;
        }
    }

    victim = top;
    set_head(victim, nb | PREV_INUSE);
    top = chunk_at_offset(victim, nb);
    set_head(top, remainder_size | PREV_INUSE);
    MALLOC_UNLOCK;
    return chunk2mem(victim);
}
