/*
 * Unit: nucore/numem
 *
 * Functions:
 *   0x001047c8 NuMemAlloc
 *   0x00104958 NuMemReAlloc
 *   0x00104a78 NuMemAllocFn
 *   0x00104d08 NuMemReAllocFn
 *   0x00104f78 NuMemFreeFn
 *   0x00105118 NuMemValidateFn
 *   0x001051b0 NuMemDumpFn
 *   0x00105268 NuMemFlushFn
 *   0x001052a0 NuMemSetDiscardable
 *   0x001052b0 NuMemFlushDiscardable
 *   0x001052d0 NuMemCreateDiscardable
 *   0x00105338 NuMemDestroyDiscardable
 *   0x00105360 NuMemSetExternal
 *   0x00105390 NuMemFree
 */

#include "nu_libc.h"
#include "nu_types.h"

#include "nucore/nuerror.h"

typedef struct NuMemGuard {
    int magic[3];
} NuMemGuard;

typedef struct NuMemHeader {
    struct NuMemHeader *next;
    struct NuMemHeader *prev;
    int line;
    int id;
    int size;
    NuMemGuard guard;
    unsigned char data[0];
} NuMemHeader;

typedef struct NuDiscardable {
    int size;
    int remaining;
    void *next;
    unsigned char pad0C[4];
} NuDiscardable;

typedef struct NuExternalMemory {
    nuintptr *cursor;
    nuintptr end;
} NuExternalMemory;

static char blk_hdr[13] = "MEMBLKHEADER";
static char blk_end[13] = "MEMBLKTAIL01";
static NuMemHeader *alloc_list = 0;
static int totalloc = 0;
static NuDiscardable *discardbuff = 0;
static int alloccnt = 0;
static NuExternalMemory *memexternal = 0;
int highallocaddr = 0;
static int peakallocaddr = 0;

static NuExternalMemory memext;


void *NuMemAlloc(int size)
{
    NuExternalMemory *external = memexternal;
    NuDiscardable *pool;
    unsigned char *ptr;
    int end;

    if (external) {
        *external->cursor = (*external->cursor + 15) & ~(nuintptr)15;
        if (external->end - *external->cursor >= size) {
            ptr = (unsigned char *)*external->cursor;
            *external->cursor = (nuintptr)(ptr + size);
            return ptr;
        }
        return 0;
    }
    pool = discardbuff;
    if (pool) {
        size = (size + 15) & 0xFFFFFFF0;
        if (size < pool->remaining) {
            pool->remaining -= size;
            ptr = pool->next;
            pool->next = ptr + size;
            return ptr;
        }
        NuErrorProlog("..\\nu2crash.ps2\\nucore\\numem.c", 0xCF)("NuMemAlloc : Failed to alloc %d bytes!", size);
        return 0;
    }
    totalloc += size;
    ptr = malloc(size);
    if (ptr == 0) {
        NuErrorProlog("..\\nu2crash.ps2\\nucore\\numem.c", 0xD6)("NuMemAlloc : Failed to alloc %d bytes!", size);
    }
    memset(ptr, 0, size);
    end = (nusintptr)ptr + size;
    if (highallocaddr < end) {
        highallocaddr = end;
    }
    if (peakallocaddr < end) {
        peakallocaddr = end;
    }
    if (end > 0x2000000) {
        NuDebugMsgProlog("..\\nu2crash.ps2\\nucore\\numem.c", 0xE1)("Warning - alloc > 32MB %d @ %d", size, ptr);
    }
    return ptr;
}


void *NuMemReAlloc(void *old, int size)
{
    NuExternalMemory *external = memexternal;
    NuDiscardable *pool;
    unsigned char *ptr;

    if (external) {
        *external->cursor = (*external->cursor + 15) & ~(nuintptr)15;
        if (external->end - *external->cursor >= size) {
            ptr = (unsigned char *)*external->cursor;
            *external->cursor = (nuintptr)(ptr + size);
            return ptr;
        }
        return 0;
    }
    pool = discardbuff;
    if (pool) {
        size = (size + 15) & 0xFFFFFFF0;
        if (size < pool->remaining) {
            pool->remaining -= size;
            ptr = pool->next;
            pool->next = ptr + size;
            return ptr;
        }
        NuErrorProlog("..\\nu2crash.ps2\\nucore\\numem.c", 0xFE)("NuMemAlloc : Failed to alloc %d bytes!", size);
        return 0;
    }
    ptr = realloc(old, size);
    if (ptr == 0) {
        NuErrorProlog("..\\nu2crash.ps2\\nucore\\numem.c", 0x104)("NuMemAlloc : Failed to alloc %d bytes!", size);
    }
    return ptr;
}


void *NuMemAllocFn(int size, char *file, int line)
{
    NuExternalMemory *external = memexternal;
    NuDiscardable *pool;
    NuMemHeader *block;
    unsigned char *ptr;
    unsigned char *data;
    int end;

    if (external) {
        *external->cursor = (*external->cursor + 15) & ~(nuintptr)15;
        if (external->end - *external->cursor >= size) {
            ptr = (unsigned char *)*external->cursor;
            *external->cursor = (nuintptr)(ptr + size);
            return ptr;
        }
        return 0;
    }
    pool = discardbuff;
    if (pool) {
        size = (size + 15) & 0xFFFFFFF0;
        if (size < pool->remaining) {
            void *chunk = pool->next;

            pool->remaining -= size;
            pool->next = (unsigned char *)chunk + size;
            return chunk;
        }
        return 0;
    }
    ptr = malloc(strlen(file) + size + sizeof(NuMemHeader) + sizeof(NuMemGuard) + 1);
    if (ptr == 0) {
        NuDebugMsgProlog("..\\nu2crash.ps2\\nucore\\numem.c", 0x139)("Alloc failed %d requested at : %s (%d)", size, file, line);
        NuErrorProlog("..\\nu2crash.ps2\\nucore\\numem.c", 0x13A)("Alloc failed %d requested at : %s (%d)", size, file, line);
    } else {
        block = (NuMemHeader *)ptr;
        data = block->data;
        memset(block, 0, strlen(file) + size + sizeof(NuMemHeader) + sizeof(NuMemGuard) + 1);
        totalloc += size;
        block->size = size;
        block->guard = *(NuMemGuard *)blk_hdr;
        memcpy(data + size, blk_end, sizeof(NuMemGuard));
        strcpy(data + size + sizeof(NuMemGuard), file);
        block->line = line;
        block->id = alloccnt++;
        block->next = alloc_list;
        if (alloc_list) {
            alloc_list->prev = block;
        }
        block->prev = 0;
        alloc_list = block;
        ptr = data;
    }
    if ((nusintptr)ptr & 15) {
        NuDebugMsgProlog("..\\nu2crash.ps2\\nucore\\numem.c", 0x151)("Warning misaligned alloc ***************************************************");
    }
    end = (nusintptr)ptr + size;
    if (highallocaddr < end) {
        highallocaddr = end;
    }
    if (peakallocaddr < end) {
        peakallocaddr = end;
    }
    if (end > 0x2000000) {
        NuDebugMsgProlog("..\\nu2crash.ps2\\nucore\\numem.c", 0x15A)("Warning - alloc > 32MB %d @ %d", size, ptr);
    }
    return ptr;
}


void *NuMemReAllocFn(void *old, int size, char *file, int line)
{
    NuExternalMemory *external;
    NuDiscardable *pool;
    NuMemHeader *block;
    unsigned char *ptr;
    unsigned char *data;
    int end;

    if (memexternal) {
        NuErrorProlog("..\\nu2crash.ps2\\nucore\\numem.c", 0x170)("cannot realloc while memexternal active");
        external = memexternal;
        *external->cursor = (*external->cursor + 15) & ~(nuintptr)15;
        if (external->end - *external->cursor >= size) {
            ptr = (unsigned char *)*external->cursor;
            *external->cursor = (nuintptr)(ptr + size);
            return ptr;
        }
        return 0;
    }
    ptr = old;
    ptr -= sizeof(NuMemHeader);
    block = (NuMemHeader *)ptr;
    pool = discardbuff;
    if (pool) {
        size = (size + 15) & 0xFFFFFFF0;
        if (size < pool->remaining) {
            void *chunk = pool->next;

            pool->remaining -= size;
            pool->next = (unsigned char *)chunk + size;
            return chunk;
        }
        return 0;
    }
    if (block->next) {
        block->next->prev = block->prev;
    }
    if (block->prev) {
        block->prev->next = block->next;
    } else {
        alloc_list = block->next;
    }
    ptr = realloc(ptr, strlen(file) + size + sizeof(NuMemHeader) + sizeof(NuMemGuard) + 1);
    if (ptr == 0) {
        NuDebugMsgProlog("..\\nu2crash.ps2\\nucore\\numem.c", 0x195)("Alloc failed %d requested at : %s (%d)", size, file, line);
    } else {
        block = (NuMemHeader *)ptr;
        data = block->data;
        totalloc = totalloc - block->size + size;
        block->size = size;
        memcpy(data + size, blk_end, sizeof(NuMemGuard));
        strcpy(data + size + sizeof(NuMemGuard), file);
        block->line = line;
        block->next = alloc_list;
        if (alloc_list) {
            alloc_list->prev = block;
        }
        block->prev = 0;
        alloc_list = block;
        ptr = data;
    }
    if ((nusintptr)ptr & 15) {
        NuDebugMsgProlog("..\\nu2crash.ps2\\nucore\\numem.c", 0x1AB)("Warning misaligned alloc ***************************************************");
    }
    end = (nusintptr)ptr + size;
    if (highallocaddr < end) {
        highallocaddr = end;
    }
    if (peakallocaddr < end) {
        peakallocaddr = end;
    }
    if (end > 0x2000000) {
        NuDebugMsgProlog("..\\nu2crash.ps2\\nucore\\numem.c", 0x1B4)("Warning - alloc > 32MB %d @ %d", size, ptr);
    }
    return ptr;
}


void NuMemFreeFn(void *ptr, char *file, int line)
{
    NuMemHeader *block;

    if (memexternal) {
        NuErrorProlog("..\\nu2crash.ps2\\nucore\\numem.c", 0x1C7)("cannot free while memexternal active");
    }
    if (ptr == 0) {
        NuDebugMsgProlog("..\\nu2crash.ps2\\nucore\\numem.c", 0x1CA)("Invalid Free attempted at : %s (%d)", file, line);
        return;
    }
    block = (NuMemHeader *)ptr - 1;
    if (memcmp((char *)ptr + block->size, blk_end, sizeof(NuMemGuard))) {
        NuDebugMsgProlog("..\\nu2crash.ps2\\nucore\\numem.c", 0x1D2)("Mem Block tail overwritten %s (%d)", file, line);
    }
    if (memcmp(&block->guard, blk_hdr, sizeof(NuMemGuard))) {
        NuDebugMsgProlog("..\\nu2crash.ps2\\nucore\\numem.c", 0x1D6)("Mem Block header overwritten %s (%d)", file, line);
    }
    if (block->next == 0 && block->prev == 0) {
        NuDebugMsgProlog("..\\nu2crash.ps2\\nucore\\numem.c", 0x1D9)("Attempted to free already free memory %s (%d)", file, line);
    }
    if (block->next) {
        block->next->prev = block->prev;
    }
    if (block->prev) {
        block->prev->next = block->next;
    } else {
        alloc_list = block->next;
    }
    block->next = 0;
    block->prev = 0;
    totalloc -= block->size;
    free(block);
}


void *NuMemValidateFn(void)
{
    NuMemHeader *block;
    unsigned char *tail;
    unsigned char *name;

    for (block = alloc_list; block; block = block->next) {
        tail = block->data;
        tail += block->size;
        name = tail + sizeof(NuMemGuard);
        if (memcmp(tail, blk_end, sizeof(NuMemGuard))) {
            NuDebugMsgProlog("..\\nu2crash.ps2\\nucore\\numem.c", 0x1FF)("Mem Block tail overwritten (@%d) [%s(%d)]", tail, name, block->line);
        }
        return tail;
        /* unreachable; retail still emits its string */
        if (memcmp(&block->guard, blk_hdr, sizeof(NuMemGuard))) {
            NuDebugMsgProlog("..\\nu2crash.ps2\\nucore\\numem.c", 0x203)("Mem Block header overwritten (@%d) [%s(%d)]", block, name, block->line);
        }
    }
    return 0;
}


void NuMemDumpFn(int min_size)
{
    NuMemHeader *block;
    char *name;

    for (block = alloc_list; block; block = block->next) {
        name = (char *)block + block->size + sizeof(NuMemHeader) + sizeof(NuMemGuard);
        if (block->size >= min_size) {
            NuDebugMsgProlog("..\\nu2crash.ps2\\nucore\\numem.c", 0x221)("Mem block not released %s(%d)[CNT:%d] ->%d bytes", name, block->line, block->id,
                                                block->size);
        }
    }
    NuDebugMsgProlog("..\\nu2crash.ps2\\nucore\\numem.c", 0x225)("Total allocated memory %d", totalloc);
}


void NuMemFlushFn(void)
{
    NuMemHeader *block = alloc_list;
    NuMemHeader *next;

    while (block) {
        next = block->next;
        free(block);
        block = next;
    }
}


NuDiscardable *NuMemSetDiscardable(NuDiscardable *pool)
{
    NuDiscardable *previous = discardbuff;

    discardbuff = pool;
    return previous;
}


void NuMemFlushDiscardable(NuDiscardable *pool)
{
    if (pool) {
        pool->remaining = pool->size;
        pool->next = pool + 1;
    }
}


NuDiscardable *NuMemCreateDiscardable(int size)
{
    NuDiscardable *saved = discardbuff;
    NuDiscardable *pool;

    discardbuff = 0;
    pool = NuMemAllocFn(size + sizeof(NuDiscardable), "..\\nu2crash.ps2\\nucore\\numem.c", 0x91);
    if (pool) {
        pool->size = size;
        pool->remaining = size;
        pool->next = pool + 1;
    }
    discardbuff = saved;
    return pool;
}


void NuMemDestroyDiscardable(NuDiscardable *pool)
{
    NuMemFreeFn(pool, "..\\nu2crash.ps2\\nucore\\numem.c", 0xA0);
}


void NuMemSetExternal(void *buffer, int *end)
{
    NuExternalMemory *memory;

    if (buffer) {
        memory = &memext;
        memory->cursor = buffer;
        memexternal = memory;
        if (end) {
            memory->end = *end;
            return;
        }
    } else {
        memexternal = 0;
    }
}


void NuMemFree(void *ptr)
{
    free(ptr);
}
