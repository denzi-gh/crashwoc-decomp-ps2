/*
 * Unit: nucore/numemblk
 *
 * Functions:
 *   0x001053b0 NuMemBlkCreateHigh
 *   0x00105498 NuMemBlkCreate
 *   0x00105560 NuMemBlkDestroy
 *   0x00105588 NuMemBlkAlloc
 *   0x001055d8 NuMemBlkFree
 *   0x00105630 NuMemBlkCheckFreeList
 */

typedef void (*NuErrorFunc)(const char *fmt, ...);

typedef struct NuMemBlk {
    void **freelist;
    unsigned int blksize;
    int count;
    int nfree;
} NuMemBlk;

extern void *memset(void *dst, int c, unsigned int len);
extern void *NuAllocHigh(int size);
extern void *NuMemAllocFn(int size, char *file, int line);
extern void NuMemFreeFn(void *ptr, char *file, int line);
extern NuErrorFunc NuErrorProlog(const char *file, int line);

extern char D_00613CA0[];
extern char D_0062EA20[];
#define numemblk_file D_00613CA0
#define msg_assert D_0062EA20


NuMemBlk *NuMemBlkCreateHigh(unsigned int size, int count, int align)
{
    unsigned int blksize;
    int hdrsize;
    NuMemBlk *blk;
    void **block;
    int stride;
    int i;

    if (size < 4) {
        size = 4;
    }
    blksize = (size + align) & ~align;
    hdrsize = (align + 16) & ~align;
    blk = NuAllocHigh(hdrsize + blksize * count);
    blk->freelist = (void **)((char *)blk + hdrsize);
    blk->blksize = blksize;
    blk->count = count;
    blk->nfree = count;
    if ((int)blk & align) {
        NuErrorProlog(numemblk_file, 0x59)(msg_assert);
    }
    stride = blksize / 4;
    block = blk->freelist;
    for (i = 0; i < count - 1; i++) {
        *block = block + stride;
        block = *block;
    }
    *block = 0;
    return blk;
}


NuMemBlk *NuMemBlkCreate(unsigned int size, int count, int align)
{
    unsigned int blksize;
    NuMemBlk *blk;
    void **block;
    int stride;
    int i;

    if (size < 4) {
        size = 4;
    }
    blksize = (size + align) & ~align;
    blk = NuMemAllocFn(((align + 16) & ~align) + blksize * count, numemblk_file, 0x79);
    stride = blksize / 4;
    blk->freelist = (void **)(blk + 1);
    blk->freelist = (void **)(((int)blk->freelist + align) & ~align);
    blk->blksize = blksize;
    blk->count = count;
    blk->nfree = count;
    block = blk->freelist;
    for (i = 0; i < count - 1; i++) {
        *block = block + stride;
        block = *block;
    }
    *block = 0;
    return blk;
}


void NuMemBlkDestroy(NuMemBlk *blk)
{
    NuMemFreeFn(blk, numemblk_file, 0x95);
}


void *NuMemBlkAlloc(NuMemBlk *blk)
{
    void **block = blk->freelist;

    if (block) {
        blk->freelist = *block;
        blk->nfree--;
        memset(block, -1, blk->blksize);
    }
    return block;
}


void NuMemBlkFree(NuMemBlk *blk, void **block)
{
    blk->nfree++;
    memset(block, -2, blk->blksize);
    *block = blk->freelist;
    blk->freelist = block;
}


void NuMemBlkCheckFreeList(NuMemBlk *blk)
{
    void **block;
    int count = 0;

    for (block = blk->freelist; block; block = *block) {
        count++;
    }
    if (count != blk->nfree) {
        NuErrorProlog(numemblk_file, 0xA3)(msg_assert);
    }
}
