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

typedef void (*NuErrorFunc)(const char *fmt, ...);

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
    unsigned int *cursor;
    unsigned int end;
} NuExternalMemory;

extern void *malloc(unsigned int size);
extern void *realloc(void *ptr, unsigned int size);
extern void free(void *ptr);
extern void *memset(void *dst, int c, unsigned int len);
extern void *memcpy(void *dst, const void *src, unsigned int len);
extern int memcmp(const void *a, const void *b, unsigned int len);
extern char *strcpy(char *dst, const char *src);
extern int strlen(const char *s);

extern NuErrorFunc NuErrorProlog(const char *file, int line);
extern NuErrorFunc NuDebugMsgProlog(const char *file, int line);

extern NuMemHeader *D_0062EA04;
extern int D_0062EA08;
extern NuDiscardable *D_0062EA0C;
extern int D_0062EA10;
extern NuExternalMemory *D_0062EA14;
extern int highallocaddr;
extern int D_0062EA1C;
extern NuExternalMemory D_00633018;
extern NuMemGuard D_00293830;
extern NuMemGuard D_00293840;
extern char D_00613A10[];
extern char D_00613A30[];
extern char D_00613A58[];
extern char D_00613A78[];
extern char D_00613AA0[];
extern char D_00613AF0[];
extern char D_00613B18[];
extern char D_00613B40[];
extern char D_00613B68[];
extern char D_00613B90[];
extern char D_00613BB8[];
extern char D_00613BE8[];
extern char D_00613C48[];
extern char D_00613C80[];
#define memblock_list D_0062EA04
#define memory_allocated D_0062EA08
#define current_discardable D_0062EA0C
#define memblock_count D_0062EA10
#define current_external D_0062EA14
#define memory_high_water D_0062EA1C
#define external_memory D_00633018
#define memblock_header_id D_00293830
#define memblock_tail_id D_00293840
#define numem_file D_00613A10
#define msg_alloc_failed D_00613A30
#define msg_alloc_over_32mb D_00613A58
#define msg_alloc_failed_at D_00613A78
#define msg_misaligned_alloc D_00613AA0
#define msg_realloc_external D_00613AF0
#define msg_free_external D_00613B18
#define msg_invalid_free D_00613B40
#define msg_tail_overwritten D_00613B68
#define msg_header_overwritten D_00613B90
#define msg_already_free D_00613BB8
#define msg_validate_tail D_00613BE8
#define msg_block_not_released D_00613C48
#define msg_total_allocated D_00613C80


void *NuMemAlloc(int size)
{
    NuExternalMemory *external = current_external;
    NuDiscardable *pool;
    unsigned char *ptr;
    int end;

    if (external) {
        *external->cursor = (*external->cursor + 15) & 0xFFFFFFF0;
        if (external->end - *external->cursor >= size) {
            ptr = (unsigned char *)*external->cursor;
            *external->cursor = (unsigned int)(ptr + size);
            return ptr;
        }
        return 0;
    }
    pool = current_discardable;
    if (pool) {
        size = (size + 15) & 0xFFFFFFF0;
        if (size < pool->remaining) {
            pool->remaining -= size;
            ptr = pool->next;
            pool->next = ptr + size;
            return ptr;
        }
        NuErrorProlog(numem_file, 0xCF)(msg_alloc_failed, size);
        return 0;
    }
    memory_allocated += size;
    ptr = malloc(size);
    if (ptr == 0) {
        NuErrorProlog(numem_file, 0xD6)(msg_alloc_failed, size);
    }
    memset(ptr, 0, size);
    end = (int)ptr + size;
    if (highallocaddr < end) {
        highallocaddr = end;
    }
    if (memory_high_water < end) {
        memory_high_water = end;
    }
    if (end > 0x2000000) {
        NuDebugMsgProlog(numem_file, 0xE1)(msg_alloc_over_32mb, size, ptr);
    }
    return ptr;
}


void *NuMemReAlloc(void *old, int size)
{
    NuExternalMemory *external = current_external;
    NuDiscardable *pool;
    unsigned char *ptr;

    if (external) {
        *external->cursor = (*external->cursor + 15) & 0xFFFFFFF0;
        if (external->end - *external->cursor >= size) {
            ptr = (unsigned char *)*external->cursor;
            *external->cursor = (unsigned int)(ptr + size);
            return ptr;
        }
        return 0;
    }
    pool = current_discardable;
    if (pool) {
        size = (size + 15) & 0xFFFFFFF0;
        if (size < pool->remaining) {
            pool->remaining -= size;
            ptr = pool->next;
            pool->next = ptr + size;
            return ptr;
        }
        NuErrorProlog(numem_file, 0xFE)(msg_alloc_failed, size);
        return 0;
    }
    ptr = realloc(old, size);
    if (ptr == 0) {
        NuErrorProlog(numem_file, 0x104)(msg_alloc_failed, size);
    }
    return ptr;
}


void *NuMemAllocFn(int size, char *file, int line)
{
    NuExternalMemory *external = current_external;
    NuDiscardable *pool;
    NuMemHeader *block;
    unsigned char *ptr;
    unsigned char *data;
    int end;

    if (external) {
        *external->cursor = (*external->cursor + 15) & 0xFFFFFFF0;
        if (external->end - *external->cursor >= size) {
            ptr = (unsigned char *)*external->cursor;
            *external->cursor = (unsigned int)(ptr + size);
            return ptr;
        }
        return 0;
    }
    pool = current_discardable;
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
        NuDebugMsgProlog(numem_file, 0x139)(msg_alloc_failed_at, size, file, line);
        NuErrorProlog(numem_file, 0x13A)(msg_alloc_failed_at, size, file, line);
    } else {
        block = (NuMemHeader *)ptr;
        data = block->data;
        memset(block, 0, strlen(file) + size + sizeof(NuMemHeader) + sizeof(NuMemGuard) + 1);
        memory_allocated += size;
        block->size = size;
        block->guard = memblock_header_id;
        memcpy(data + size, &memblock_tail_id, sizeof(NuMemGuard));
        strcpy(data + size + sizeof(NuMemGuard), file);
        block->line = line;
        block->id = memblock_count++;
        block->next = memblock_list;
        if (memblock_list) {
            memblock_list->prev = block;
        }
        block->prev = 0;
        memblock_list = block;
        ptr = data;
    }
    if ((int)ptr & 15) {
        NuDebugMsgProlog(numem_file, 0x151)(msg_misaligned_alloc);
    }
    end = (int)ptr + size;
    if (highallocaddr < end) {
        highallocaddr = end;
    }
    if (memory_high_water < end) {
        memory_high_water = end;
    }
    if (end > 0x2000000) {
        NuDebugMsgProlog(numem_file, 0x15A)(msg_alloc_over_32mb, size, ptr);
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

    if (current_external) {
        NuErrorProlog(numem_file, 0x170)(msg_realloc_external);
        external = current_external;
        *external->cursor = (*external->cursor + 15) & 0xFFFFFFF0;
        if (external->end - *external->cursor >= size) {
            ptr = (unsigned char *)*external->cursor;
            *external->cursor = (unsigned int)(ptr + size);
            return ptr;
        }
        return 0;
    }
    ptr = old;
    ptr -= sizeof(NuMemHeader);
    block = (NuMemHeader *)ptr;
    pool = current_discardable;
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
        memblock_list = block->next;
    }
    ptr = realloc(ptr, strlen(file) + size + sizeof(NuMemHeader) + sizeof(NuMemGuard) + 1);
    if (ptr == 0) {
        NuDebugMsgProlog(numem_file, 0x195)(msg_alloc_failed_at, size, file, line);
    } else {
        block = (NuMemHeader *)ptr;
        data = block->data;
        memory_allocated = memory_allocated - block->size + size;
        block->size = size;
        memcpy(data + size, &memblock_tail_id, sizeof(NuMemGuard));
        strcpy(data + size + sizeof(NuMemGuard), file);
        block->line = line;
        block->next = memblock_list;
        if (memblock_list) {
            memblock_list->prev = block;
        }
        block->prev = 0;
        memblock_list = block;
        ptr = data;
    }
    if ((int)ptr & 15) {
        NuDebugMsgProlog(numem_file, 0x1AB)(msg_misaligned_alloc);
    }
    end = (int)ptr + size;
    if (highallocaddr < end) {
        highallocaddr = end;
    }
    if (memory_high_water < end) {
        memory_high_water = end;
    }
    if (end > 0x2000000) {
        NuDebugMsgProlog(numem_file, 0x1B4)(msg_alloc_over_32mb, size, ptr);
    }
    return ptr;
}


void NuMemFreeFn(void *ptr, char *file, int line)
{
    NuMemHeader *block;

    if (current_external) {
        NuErrorProlog(numem_file, 0x1C7)(msg_free_external);
    }
    if (ptr == 0) {
        NuDebugMsgProlog(numem_file, 0x1CA)(msg_invalid_free, file, line);
        return;
    }
    block = (NuMemHeader *)ptr - 1;
    if (memcmp((char *)ptr + block->size, &memblock_tail_id, sizeof(NuMemGuard))) {
        NuDebugMsgProlog(numem_file, 0x1D2)(msg_tail_overwritten, file, line);
    }
    if (memcmp(&block->guard, &memblock_header_id, sizeof(NuMemGuard))) {
        NuDebugMsgProlog(numem_file, 0x1D6)(msg_header_overwritten, file, line);
    }
    if (block->next == 0 && block->prev == 0) {
        NuDebugMsgProlog(numem_file, 0x1D9)(msg_already_free, file, line);
    }
    if (block->next) {
        block->next->prev = block->prev;
    }
    if (block->prev) {
        block->prev->next = block->next;
    } else {
        memblock_list = block->next;
    }
    block->next = 0;
    block->prev = 0;
    memory_allocated -= block->size;
    free(block);
}


void *NuMemValidateFn(void)
{
    NuMemHeader *block;
    unsigned char *tail;
    unsigned char *name;

    for (block = memblock_list; block; block = block->next) {
        tail = block->data;
        tail += block->size;
        name = tail + sizeof(NuMemGuard);
        if (memcmp(tail, &memblock_tail_id, sizeof(NuMemGuard))) {
            NuDebugMsgProlog(numem_file, 0x1FF)(msg_validate_tail, tail, name, block->line);
        }
        return tail;
    }
    return 0;
}


void NuMemDumpFn(int min_size)
{
    NuMemHeader *block;
    char *name;

    for (block = memblock_list; block; block = block->next) {
        name = (char *)block + block->size + sizeof(NuMemHeader) + sizeof(NuMemGuard);
        if (block->size >= min_size) {
            NuDebugMsgProlog(numem_file, 0x221)(msg_block_not_released, name, block->line, block->id,
                                                block->size);
        }
    }
    NuDebugMsgProlog(numem_file, 0x225)(msg_total_allocated, memory_allocated);
}


void NuMemFlushFn(void)
{
    NuMemHeader *block = memblock_list;
    NuMemHeader *next;

    while (block) {
        next = block->next;
        free(block);
        block = next;
    }
}


NuDiscardable *NuMemSetDiscardable(NuDiscardable *pool)
{
    NuDiscardable *previous = current_discardable;

    current_discardable = pool;
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
    NuDiscardable *saved = current_discardable;
    NuDiscardable *pool;

    current_discardable = 0;
    pool = NuMemAllocFn(size + sizeof(NuDiscardable), numem_file, 0x91);
    if (pool) {
        pool->size = size;
        pool->remaining = size;
        pool->next = pool + 1;
    }
    current_discardable = saved;
    return pool;
}


void NuMemDestroyDiscardable(NuDiscardable *pool)
{
    NuMemFreeFn(pool, numem_file, 0xA0);
}


void NuMemSetExternal(void *buffer, int *end)
{
    NuExternalMemory *memory;

    if (buffer) {
        memory = &external_memory;
        memory->cursor = buffer;
        current_external = memory;
        if (end) {
            memory->end = *end;
            return;
        }
    } else {
        current_external = 0;
    }
}


void NuMemFree(void *ptr)
{
    free(ptr);
}
