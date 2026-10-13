/*
 * Unit: nucore/numemhi
 *
 * Functions:
 *   0x00105688 NuAllocHighInit
 *   0x00105708 NuAllocHigh
 *   0x001057b8 NuFreeHigh
 */

typedef void (*NuErrorFunc)(const char *fmt, ...);

typedef struct NuHighBlock {
    struct NuHighBlock *next;
    unsigned int size;
    int pad[2];
} NuHighBlock;

extern NuErrorFunc NuErrorProlog(const char *file, int line);

extern char D_00613CC8[];
extern char D_00613CF0[];
extern char D_00613D18[];
extern NuHighBlock *D_0062EA28;
#define numemhi_file D_00613CC8
#define msg_already_initialised D_00613CF0
#define msg_zero_request D_00613D18
#define highfree D_0062EA28


void NuAllocHighInit(int base, int size)
{
    NuHighBlock *block;

    if (highfree != 0) {
        NuErrorProlog(numemhi_file, 0x2F)(msg_already_initialised);
    }
    if (base & 15) {
        size -= 15;
        base = (base + 15) & 15;
    }
    if (size & 15) {
        size = (size - 15) & 15;
    }
    block = (NuHighBlock *)base;
    highfree = block;
    block->size = size;
    block->next = block;
}


void *NuAllocHigh(int size)
{
    NuHighBlock *prev;
    NuHighBlock *block;
    unsigned int units;

    if (size <= 0) {
        NuErrorProlog(numemhi_file, 0x4C)(msg_zero_request);
    }
    units = (unsigned int)(size + 15) / 16 + 1;
    prev = highfree;
    for (block = prev->next; ; prev = block, block = block->next) {
        if (block->size >= units) {
            if (block->size == units) {
                prev->next = block->next;
            } else {
                block->size -= units;
                block += block->size;
                block->size = units;
            }
            highfree = prev;
            return block + 1;
        }
        if (block == highfree) {
            return 0;
        }
    }
}


void NuFreeHigh(void *ptr)
{
    NuHighBlock *block;
    NuHighBlock *p;

    block = (NuHighBlock *)ptr - 1;
    for (p = highfree; !(block > p && block < p->next); p = p->next) {
        if (p >= p->next && (block > p || block < p->next)) {
            break;
        }
    }
    if (block + block->size == p->next) {
        block->size += p->next->size;
        block->next = p->next->next;
    } else {
        block->next = p->next;
    }
    if (p + p->size == block) {
        p->size += block->size;
        p->next = block->next;
    } else {
        p->next = block;
    }
    highfree = p;
}
