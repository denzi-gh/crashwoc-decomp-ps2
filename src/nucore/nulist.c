/* nu2crash.ps2/nucore/nulist.c -- doubly-linked list helpers.
 *
 * First matched translation unit. Only the two trivial accessors below are
 * decompiled so far; the rest of the unit's functions are still assembly and
 * are not present in this file yet. Built with EE GCC 2.9-ee-991111-01 at
 * -O2 -G8 -fomit-frame-pointer (the locked matching flags); tools/match.py
 * verifies each function here against the retail bytes.
 */

typedef struct NuList {
    void *head;
    void *tail;
} NuList;

void *NuListGetHead(NuList *list) {
    return list->head;
}

void *NuListGetTail(NuList *list) {
    return list->tail;
}
