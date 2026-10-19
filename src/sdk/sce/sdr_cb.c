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

sceSdUserCB _sce_sdr_gDMA0CB = 0;
sceSdUserCB _sce_sdr_gDMA1CB = 0;
sceSdUserCB _sce_sdr_gIRQCB = 0;
sceSdTransIntrHandler _sce_sdr_transIntr0Hdr = 0;
sceSdTransIntrHandler _sce_sdr_transIntr1Hdr = 0;
sceSdSpu2IntrHandler _sce_sdr_spu2IntrHdr = 0;
void *_sce_sdr_transIntr0Arg = 0;
void *_sce_sdr_transIntr1Arg = 0;
void *_sce_sdr_spu2IntrArg = 0;

static char stack[256];
static char gEECBData[64] __attribute__((aligned(64)));

extern char _gp[];

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
    static char stack[0x1000];
    ThreadParam param;

    _sce_sdr_gDMA0CB = 0;
    _sce_sdr_gDMA1CB = 0;
    _sce_sdr_gIRQCB = 0;
    _sce_sdr_transIntr0Hdr = 0;
    _sce_sdr_transIntr1Hdr = 0;
    _sce_sdr_spu2IntrHdr = 0;
    _sce_sdr_transIntr0Arg = 0;
    _sce_sdr_transIntr1Arg = 0;
    _sce_sdr_spu2IntrArg = 0;
    sceSdRemote(1, 0xE620);
    param.entry = _sdrCBThread;
    param.stack = stack;
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
    sceSifRegisterRpc(&serve, SDR_CB_RPC_ID, _sdrCB, gEECBData, 0, 0, &queue);
    sceSifRpcLoop(&queue);
}


static void *_sdrCB(unsigned int fno, void *data, int size)
{
    int *request = data;

    switch (request[0]) {
    case SDR_CB_DMA0:
        if (_sce_sdr_gDMA0CB) {
            _sce_sdr_gDMA0CB();
        }
        break;
    case SDR_CB_DMA1:
        if (_sce_sdr_gDMA1CB) {
            _sce_sdr_gDMA1CB();
        }
        break;
    case SDR_CB_IRQ:
        if (_sce_sdr_gIRQCB) {
            _sce_sdr_gIRQCB();
        }
        break;
    case SDR_CB_TRANS_INTR0:
        if (_sce_sdr_transIntr0Hdr) {
            _sce_sdr_transIntr0Hdr(0, _sce_sdr_transIntr0Arg);
        }
        break;
    case SDR_CB_TRANS_INTR1:
        if (_sce_sdr_transIntr1Hdr) {
            _sce_sdr_transIntr1Hdr(1, _sce_sdr_transIntr1Arg);
        }
        break;
    case SDR_CB_SPU2_INTR:
        if (_sce_sdr_spu2IntrHdr) {
            _sce_sdr_spu2IntrHdr(request[1], _sce_sdr_spu2IntrArg);
        }
        break;
    }
    return 0;
}
