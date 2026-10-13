/*
 * Unit: sdk/sce/sdr_cb
 *
 * Functions:
 *   0x0026d300 sceSdRemoteCallbackInit
 *   0x0026d3d8 _sdrCBThread
 *   0x0026d440 _sdrCB
 */

typedef struct ThreadParam {
    int status;
    void (*entry)(void *);
    void *stack;
    int stackSize;
    void *gpReg;
    int initPriority;
    int currentPriority;
    unsigned int attr;
    unsigned int option;
    int waitType;
    int waitId;
    int wakeupCount;
} ThreadParam;

typedef void *(*sceSifRpcFunc)(unsigned int fno, void *data, int size);

typedef struct sceSifQueueData {
    int key;
    int active;
    struct sceSifServeData *link;
    struct sceSifServeData *start;
    struct sceSifServeData *end;
    struct sceSifQueueData *next;
} sceSifQueueData;

typedef struct sceSifServeData {
    int command;
    sceSifRpcFunc func;
    void *buff;
    int size;
    sceSifRpcFunc cfunc;
    void *cbuff;
    int csize;
    void *client;
    void *paddr;
    unsigned int fno;
    void *receive;
    int rsize;
    int rmode;
    unsigned int rid;
    struct sceSifServeData *link;
    struct sceSifServeData *next;
    struct sceSifQueueData *base;
} sceSifServeData;

typedef int (*sceSdUserCB)(void);
typedef int (*sceSdTransIntrHandler)(int channel, void *arg);
typedef int (*sceSdSpu2IntrHandler)(int core_bit, void *arg);

enum {
    SDR_CB_DMA0 = 1,
    SDR_CB_DMA1 = 2,
    SDR_CB_IRQ = 3,
    SDR_CB_TRANS_INTR0 = 11,
    SDR_CB_TRANS_INTR1 = 12,
    SDR_CB_SPU2_INTR = 13
};

extern sceSdUserCB _sce_sdr_gDMA0CB[];
extern sceSdUserCB _sce_sdr_gDMA1CB[];
extern sceSdUserCB _sce_sdr_gIRQCB[];
extern sceSdTransIntrHandler _sce_sdr_transIntr0Hdr[];
extern sceSdTransIntrHandler _sce_sdr_transIntr1Hdr[];
extern sceSdSpu2IntrHandler _sce_sdr_spu2IntrHdr[];
extern void *_sce_sdr_transIntr0Arg[];
extern void *_sce_sdr_transIntr1Arg[];
extern void *_sce_sdr_spu2IntrArg[];
extern char D_00701900[];
extern char D_00702A00[];
extern char _gp[];

#define gDMA0CB        _sce_sdr_gDMA0CB[0]
#define gDMA1CB        _sce_sdr_gDMA1CB[0]
#define gIRQCB         _sce_sdr_gIRQCB[0]
#define transIntr0Hdr  _sce_sdr_transIntr0Hdr[0]
#define transIntr1Hdr  _sce_sdr_transIntr1Hdr[0]
#define spu2IntrHdr    _sce_sdr_spu2IntrHdr[0]
#define transIntr0Arg  _sce_sdr_transIntr0Arg[0]
#define transIntr1Arg  _sce_sdr_transIntr1Arg[0]
#define spu2IntrArg    _sce_sdr_spu2IntrArg[0]
#define cbThreadStack  D_00701900
#define cbRpcBuffer    D_00702A00

#define SDR_CB_RPC_ID 0x80000704

extern int sceSdRemote(int arg, int command, ...);
extern int CreateThread(ThreadParam *param);
extern int StartThread(int tid, void *arg);
extern int GetThreadId(void);
extern void sceSifInitRpc(unsigned int mode);
extern void sceSifSetRpcQueue(sceSifQueueData *queue, int tid);
extern void sceSifRegisterRpc(sceSifServeData *serve, unsigned int command, sceSifRpcFunc func, void *buff, sceSifRpcFunc cfunc, void *cbuff, sceSifQueueData *queue);
extern void sceSifRpcLoop(sceSifQueueData *queue);
extern int printf(const char *fmt, ...);

static void _sdrCBThread(void *arg);
static void *_sdrCB(unsigned int fno, void *data, int size);


int sceSdRemoteCallbackInit(int priority)
{
    ThreadParam param;

    gDMA0CB = 0;
    gDMA1CB = 0;
    gIRQCB = 0;
    transIntr0Hdr = 0;
    transIntr1Hdr = 0;
    spu2IntrHdr = 0;
    transIntr0Arg = 0;
    transIntr1Arg = 0;
    spu2IntrArg = 0;
    sceSdRemote(1, 0xE620);
    param.entry = _sdrCBThread;
    param.stack = cbThreadStack;
    param.stackSize = 0x1000;
    param.initPriority = priority;
    param.gpReg = _gp;
    param.option = CreateThread(&param);
    if (StartThread(param.option, 0) < 0) {
        printf("Can't start thread for streaming.\n");
        return -1;
    }
    return param.option;
}


static void _sdrCBThread(void *arg)
{
    sceSifQueueData queue;
    sceSifServeData serve;

    sceSifInitRpc(0);
    sceSifSetRpcQueue(&queue, GetThreadId());
    sceSifRegisterRpc(&serve, SDR_CB_RPC_ID, _sdrCB, cbRpcBuffer, 0, 0, &queue);
    sceSifRpcLoop(&queue);
}


static void *_sdrCB(unsigned int fno, void *data, int size)
{
    int *request = data;

    switch (request[0]) {
    case SDR_CB_DMA0:
        if (gDMA0CB) {
            gDMA0CB();
        }
        break;
    case SDR_CB_DMA1:
        if (gDMA1CB) {
            gDMA1CB();
        }
        break;
    case SDR_CB_IRQ:
        if (gIRQCB) {
            gIRQCB();
        }
        break;
    case SDR_CB_TRANS_INTR0:
        if (transIntr0Hdr) {
            transIntr0Hdr(0, transIntr0Arg);
        }
        break;
    case SDR_CB_TRANS_INTR1:
        if (transIntr1Hdr) {
            transIntr1Hdr(1, transIntr1Arg);
        }
        break;
    case SDR_CB_SPU2_INTR:
        if (spu2IntrHdr) {
            spu2IntrHdr(request[1], spu2IntrArg);
        }
        break;
    }
    return 0;
}
