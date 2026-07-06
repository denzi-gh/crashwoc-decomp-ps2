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
