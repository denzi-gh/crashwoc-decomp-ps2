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

static sceSifClientData cd;
static int rdata[16] __attribute__((aligned(64)));
static IopHeapArg sdata;
static IopLoadArg _lih_data __attribute__((aligned(64)));

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
        if (sceSifBindRpc(&cd, IH_RPC_SERVER, 0) < 0) {
            return -1;
        }
        if (cd.server != 0) {
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
    sdata.p.size = size;
    if (sceSifCallRpc(&cd, IH_ALLOC, 0, &sdata, 4, rdata, 4, 0, 0) < 0) {
        return 0;
    }
    return (void *)rdata[0];
}


void *sceSifAllocSysMemory(int type, unsigned int size, void *addr) {
    if (_bind < 0) {
        return 0;
    }
    sdata.p.size = size;
    sdata.type = type;
    sdata.addr = addr;
    if (sceSifCallRpc(&cd, IH_ALLOC_SYSMEM, 0, &sdata, 12, rdata, 4, 0, 0) < 0) {
        return 0;
    }
    return (void *)rdata[0];
}


int sceSifFreeSysMemory(void *addr) {
    if (_bind < 0) {
        return 0;
    }
    sdata.p.addr = addr;
    if (sceSifCallRpc(&cd, IH_FREE, 0, &sdata, 4, rdata, 4, 0, 0) < 0) {
        return -1;
    }
    return rdata[0];
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
        if ((_lih_data.path[len] = path[len]) == 0) {
            break;
        }
    }
    if (len == IH_PATH_MAX) {
        len = IH_PATH_MAX - 1;
        _lih_data.path[len] = 0;
    }
    _lih_data.addr = addr;
    _lih_data.path[IH_PATH_MAX - 1] = 0;
    if (sceSifCallRpc(&cd, IH_LOAD, 0, &_lih_data, len + 5, rdata, 4, 0, 0) < 0) {
        return -1;
    }
    return rdata[0];
}
