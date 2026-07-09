/* nu2crash.ps2/nucore/nulist.c -- doubly-linked list helpers*/

typedef struct NuListNode {
    struct NuListNode *next;
    struct NuListNode *prev;
} NuListNode;

typedef struct NuList {
    NuListNode *head;
    NuListNode *tail;
} NuList;


typedef void (*NuErrorFunc)(const char *fmt, ...);

extern NuErrorFunc NuErrorProlog(const char *file, int line);

extern char D_00613D50[];
extern char D_0062EA38[];


int NuListCheck(NuList *list) {
    NuListNode *node;
    NuListNode *prev;
    int count;

    prev = 0;
    count = 0;
    for (node = list->head; node; node = node->next) {
        count++;
        if (node->prev != prev) {
            NuErrorProlog(D_00613D50, 0x38)(D_0062EA38);
        }
        prev = node;
    }
    if (prev != list->tail) {
        NuErrorProlog(D_00613D50, 0x3C)(D_0062EA38);
    }
    return count;
}


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


void NuListInsert(NuList *list, NuListNode *node) {
    node->next = list->head;
    node->prev = 0;
    if (list->head) {
        list->head->prev = node;
    }
    if (!list->tail) {
        list->tail = node;
    }
    list->head = node;
}


void NuListInsertBefore(NuList *list, NuListNode *at, NuListNode *node) {
    if (at) {
        node->next = at;
        node->prev = at->prev;
        if (at->prev) {
            at->prev->next = node;
        } else {
            list->head = node;
        }
        at->prev = node;
    } else {
        node->next = list->head;
        node->prev = 0;
        if (list->head) {
            list->head->prev = node;
        }
        if (!list->tail) {
            list->tail = node;
        }
        list->head = node;
    }
}


void NuListInsertAfter(NuList *list, NuListNode *at, NuListNode *node) {
    if (at) {
        node->prev = at;
        node->next = at->next;
        if (at->next) {
            at->next->prev = node;
        } else {
            list->tail = node;
        }
        at->next = node;
    } else {
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
}


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

void *NuListGetNext(NuList *list, NuListNode *node) {
    if (node) {
        return node->next;
    }
    return list->head;
}

void *NuListGetPrev(NuList *list, NuListNode *node) {
    if (node) {
        return node->prev;
    }
    return list->tail;
}
