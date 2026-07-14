/*
 * Unit: game/listman
 *
 * Functions:
 *   0x001cba08 NuLstCreate
 *   0x001cbac8 NuLstDestroy
 *   0x001cbaf0 NuLstAlloc
 *   0x001cbb48 NuLstAllocBefore
 *   0x001cbba8 NuLstAllocAfter
 *   0x001cbc00 NuLstFree
 *   0x001cbc60 NuLstGetByIdx
 *   0x001cbc98 NuLstGetNext
 *   0x001cbcc0 NuLstGetPrev
 *
 * Free-list pool allocator. Each element is preceded by a 0x10-byte
 * nulnkhdr_s; the returned/consumed pointer is (hdr + 1). The "in use" flag
 * is 0x10000 (bit 16), verified in NuLstAlloc/NuLstFree -- the GC reference
 * had 0x8000, which is wrong for this build.
 */

struct nulnkhdr_s;

struct nulsthdr_s {
    struct nulnkhdr_s *free;   /* +0x0  verified in NuLstAlloc/NuLstCreate */
    struct nulnkhdr_s *head;   /* +0x4  verified in NuLstAlloc/NuLstFree */
    struct nulnkhdr_s *tail;   /* +0x8  verified in NuLstAlloc/NuLstFree */
    short elcnt;               /* +0xC  verified in NuLstCreate (sh) */
    short elsize;              /* +0xE  verified in NuLstCreate (sh) */
};

struct nulnkhdr_s {
    struct nulsthdr_s *owner;  /* +0x0  verified in NuLstFree/NuLstCreate */
    struct nulnkhdr_s *succ;   /* +0x4  verified in NuLstAlloc/NuLstGetNext */
    struct nulnkhdr_s *prev;   /* +0x8  verified in NuLstAlloc/NuLstFree */
    short id;                  /* +0xC  verified in NuLstCreate (sh) */
    short flags;               /* +0xE  (0x10000 in-use bit lives here) */
};

extern void *NuMemAllocFn(int size, char *file, int line);
extern void NuMemFreeFn(void *ptr, char *file, int line);

extern char D_0061B600[];  /* "..\nu2crash.ps2\...\listman.c" filename string */


struct nulsthdr_s *NuLstCreate(int elcnt, int elsize) {
    struct nulsthdr_s *list;
    struct nulnkhdr_s *curr;
    struct nulnkhdr_s *start;
    int n;

    list = (struct nulsthdr_s *)NuMemAllocFn(elcnt * (elsize + 0x10) + 0x10,
                                             D_0061B600, 0x24);
    if (list != 0) {
        curr = (struct nulnkhdr_s *)(list + 1);
        list->free = curr;
        list->head = 0;
        list->elcnt = (short)elcnt;
        list->elsize = (short)elsize;
        start = (struct nulnkhdr_s *)((char *)curr + elsize + 0x10);
        for (n = 1; n < elcnt; n++) {
            curr->succ = start;
            curr->id = (short)(n - 1);
            curr->owner = list;
            curr = start;
            start = (struct nulnkhdr_s *)((char *)start + (elsize + 0x10));
        }
        curr->owner = list;
        curr->id = (short)(n - 1);
        curr->succ = 0;
    }
    return list;
}

void NuLstDestroy(struct nulsthdr_s *hdr) {
    NuMemFreeFn(hdr, D_0061B600, 0x40);
}

struct nulnkhdr_s *NuLstAlloc(struct nulsthdr_s *hdr) {
    struct nulnkhdr_s *rv;

    rv = hdr->free;
    if (rv != 0) {
        hdr->free = rv->succ;
        rv->succ = hdr->head;
        if (hdr->head != 0) {
            hdr->head->prev = rv;
        } else {
            hdr->tail = rv;
        }
        rv->prev = 0;
        hdr->head = rv;
        *(unsigned int *)&rv->id = *(unsigned int *)&rv->id | 0x10000;
        return rv + 1;
    }
    return 0;
}

void NuLstFree(struct nulnkhdr_s *lnk) {
    struct nulsthdr_s *hdr;

    lnk -= 1;
    hdr = lnk->owner;
    if (lnk->succ != 0) {
        lnk->succ->prev = lnk->prev;
    } else {
        hdr->tail = lnk->prev;
    }
    if (lnk->prev != 0) {
        lnk->prev->succ = lnk->succ;
    } else {
        hdr->head = lnk->succ;
    }
    lnk->succ = hdr->free;
    hdr->free = lnk;
    *(unsigned int *)&lnk->id = *(unsigned int *)&lnk->id & 0xFFFEFFFF;
}

struct nulnkhdr_s *NuLstGetNext(struct nulsthdr_s *hdr, struct nulnkhdr_s *lnk) {
    struct nulnkhdr_s *rv;

    if (lnk != 0) {
        rv = (lnk - 1)->succ;
        if (rv == 0) {
            return 0;
        }
        return rv + 1;
    }
    rv = hdr->head;
    if (rv == 0) {
        return 0;
    }
    return rv + 1;
}
