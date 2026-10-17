/*
 * Unit: sdk/sce/iopheap
 *
 * Functions:
 *   0x0027f840 sceSifInitIopHeap
 *   0x0027f8c8 sceSifAllocIopHeap
 *   0x0027f938 sceSifAllocSysMemory
 *   0x0027f9b8 sceSifFreeSysMemory
 *   0x0027fa30 sceSifFreeIopHeap
 *   0x0027fa50 sceSifLoadIopHeap
 */

#define IH_RPC_SERVER   0x80000003
#define IH_ALLOC        1
#define IH_FREE         2
#define IH_LOAD         3
#define IH_ALLOC_SYSMEM 4
#define IH_PATH_MAX     252

typedef struct sceSifClientData {
    unsigned char rpc_data[16];
    unsigned int command;
    void *buff;
    void *cbuff;
    void (*end_func)(void *);
    void *end_param;
    void *server;
} sceSifClientData;

typedef struct IopHeapArg {
    union {
        unsigned int size;
        void *addr;
    } p;
    int type;
    void *addr;
} IopHeapArg;

typedef struct IopLoadArg {
    void *addr;
    char path[IH_PATH_MAX];
} IopLoadArg;

extern sceSifClientData D_00706500;
extern int D_00706540[];
extern IopHeapArg D_00706580;
extern IopLoadArg D_007065C0;

#define ih_client  D_00706500
#define ih_result  D_00706540
#define ih_arg     D_00706580
#define ih_load    D_007065C0

extern int sceSifBindRpc(sceSifClientData *client, unsigned int request,
                         unsigned int mode);
extern int sceSifCallRpc(sceSifClientData *client, unsigned int fno,
                         unsigned int mode, void *send, int ssize,
                         void *receive, int rsize, void (*end_func)(void *),
                         void *end_param);
extern int sceSifFreeSysMemory(void *addr);

static int _bind = -1;


int sceSifInitIopHeap(void) {
    int wait;

    while (1) {
        if (sceSifBindRpc(&ih_client, IH_RPC_SERVER, 0) < 0) {
            return -1;
        }
        if (ih_client.server != 0) {
            _bind = 0;
            break;
        }
        wait = 0x100000;
        while (wait--) {
        }
    }
    return 0;
}


void *sceSifAllocIopHeap(unsigned int size) {
    if (_bind < 0) {
        return 0;
    }
    ih_arg.p.size = size;
    if (sceSifCallRpc(&ih_client, IH_ALLOC, 0, &ih_arg, 4, ih_result, 4, 0, 0) < 0) {
        return 0;
    }
    return (void *)ih_result[0];
}


void *sceSifAllocSysMemory(int type, unsigned int size, void *addr) {
    if (_bind < 0) {
        return 0;
    }
    ih_arg.p.size = size;
    ih_arg.type = type;
    ih_arg.addr = addr;
    if (sceSifCallRpc(&ih_client, IH_ALLOC_SYSMEM, 0, &ih_arg, 12, ih_result, 4, 0, 0) < 0) {
        return 0;
    }
    return (void *)ih_result[0];
}


int sceSifFreeSysMemory(void *addr) {
    if (_bind < 0) {
        return 0;
    }
    ih_arg.p.addr = addr;
    if (sceSifCallRpc(&ih_client, IH_FREE, 0, &ih_arg, 4, ih_result, 4, 0, 0) < 0) {
        return -1;
    }
    return ih_result[0];
}


int sceSifFreeIopHeap(void *addr) {
    return sceSifFreeSysMemory(addr);
}


int sceSifLoadIopHeap(const char *path, void *addr) {
    int len;

    if (_bind < 0) {
        return 0;
    }
    for (len = 0; len < IH_PATH_MAX; len++) {
        if ((ih_load.path[len] = path[len]) == 0) {
            break;
        }
    }
    if (len == IH_PATH_MAX) {
        len = IH_PATH_MAX - 1;
        ih_load.path[len] = 0;
    }
    ih_load.addr = addr;
    ih_load.path[IH_PATH_MAX - 1] = 0;
    if (sceSifCallRpc(&ih_client, IH_LOAD, 0, &ih_load, len + 5, ih_result, 4, 0, 0) < 0) {
        return -1;
    }
    return ih_result[0];
}
