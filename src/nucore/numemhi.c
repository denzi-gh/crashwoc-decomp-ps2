/*
 * Unit: nucore/numemhi
 *
 * Functions:
 *   0x00105688 NuAllocHighInit
 *   0x00105708 NuAllocHigh
 *   0x001057b8 NuFreeHigh
 */

#include "nucore/nuerror.h"

typedef struct NuHighBlock {
    struct NuHighBlock *next;
    unsigned int size;
    int pad[2];
} NuHighBlock;

static NuHighBlock *freep = 0;


void NuAllocHighInit(int base, int size)
{
    NuHighBlock *block;

    if (freep != 0) {
        NuErrorProlog("..\\nu2crash.ps2\\nucore\\numemhi.c", 0x2F)("NuAllocHighInit : Already initialised\n");
    }
    if (base & 15) {
        size -= 15;
        base = (base + 15) & 15;
    }
    if (size & 15) {
        size = (size - 15) & 15;
    }
    block = (NuHighBlock *)base;
    freep = block;
    block->size = size;
    block->next = block;
}


void *NuAllocHigh(int size)
{
    NuHighBlock *prev;
    NuHighBlock *block;
    unsigned int units;

    if (size <= 0) {
        NuErrorProlog("..\\nu2crash.ps2\\nucore\\numemhi.c", 0x4C)("NuAllocHigh : request for 0 bytes of memory denied");
    }
    units = (unsigned int)(size + 15) / 16 + 1;
    prev = freep;
    for (block = prev->next; ; prev = block, block = block->next) {
        if (block->size >= units) {
            if (block->size == units) {
                prev->next = block->next;
            } else {
                block->size -= units;
                block += block->size;
                block->size = units;
            }
            freep = prev;
            return block + 1;
        }
        if (block == freep) {
            return 0;
        }
    }
}


void NuFreeHigh(void *ptr)
{
    NuHighBlock *block;
    NuHighBlock *p;

    block = (NuHighBlock *)ptr - 1;
    for (p = freep; !(block > p && block < p->next); p = p->next) {
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
    freep = p;
}
