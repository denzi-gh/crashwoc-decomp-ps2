/* nu2crash.ps2/nucore/nulist.c -- doubly-linked list helpers.
 *
 * First matched translation unit. This file contains only the decompiled
 * functions; the rest of retail unit 7 is still assembly, and the matching
 * build splices those functions in from the retail slices automatically
 * (tools/gen_hybrid.py, driven by config/pal103/status/nucore/nulist.toml --
 * no fallback ever appears in this source). Built with EE GCC
 * 2.9-ee-991111-01 under the `default` profile (-O2 -G8
 * -fomit-frame-pointer); tools/match.py verifies each function against the
 * retail bytes, tools/verify_hybrid.py proves the assembled hybrid unit
 * byte-identical to retail.
 */

typedef struct NuListNode {
    struct NuListNode *next;
    struct NuListNode *prev;
} NuListNode;

typedef struct NuList {
    NuListNode *head;
    NuListNode *tail;
} NuList;

/* WIP (state = "asm" in the manifest): scored by objdiff/report only; the
 * matching build still uses the retail bytes until promoted. */
void NuListAppend(NuList *list, NuListNode *node) {
    node->next = 0;
    node->prev = list->tail;
    if (list->tail) {
        list->tail->next = node;
    }
    list->tail = node;
    if (!list->head) {
        list->head = node;
    }
}

/* WIP (state = "asm"). */
void NuListRemove(NuList *list, NuListNode *node) {
    if (node->next) {
        node->next->prev = node->prev;
    } else {
        list->tail = node->prev;
    }
    if (node->prev) {
        node->prev->next = node->next;
    } else {
        list->head = node->next;
    }
}

void *NuListGetHead(NuList *list) {
    return list->head;
}

void *NuListGetTail(NuList *list) {
    return list->tail;
}
