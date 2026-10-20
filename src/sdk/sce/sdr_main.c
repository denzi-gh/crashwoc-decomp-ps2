/*
 * Unit: sdk/sce/sdr_main
 *
 * Functions:
 *   0x0026d008 sceSdRemoteInit
 *   0x0026d0b0 sceSdTransToIOP
 *   0x0026d138 sceSdCallBack
 *   0x0026d148 sceSdRemote
 */

#define SD_RPC_SERVER 0x80000701
#define SD_BIND_WAIT 10000

#define SD_SPU2_PROC_BATCH 0x8130
#define SD_SPU2_PROC_BATCH_EX 0x8140
#define SD_SET_TRANS_INTR_HANDLER 0x8160
#define SD_SET_SPU2_INTR_HANDLER 0x8170

#define SIF_RPCM_NOWAIT 1

typedef char *va_list;
#define va_start(ap, last) ((ap) = (va_list)__builtin_next_arg(last) - (__builtin_args_info(2) < 8 ? (8 - __builtin_args_info(2)) * 8 : 0))
#define va_arg(ap, type) (*(type *)(((ap) += 8) - 8))
#define va_end(ap)

typedef int (*sceSdSpu2IntrHandler)(int core, void *common);
typedef int (*sceSdTransIntrHandler)(int channel, void *common);

typedef struct sceSifClientData {
    unsigned char rpc_data[16];
    unsigned int command;
    void *buff;
    void *cbuff;
    void (*end_func)(void *);
    void *end_param;
    void *server;
} sceSifClientData;

typedef struct sceSifDmaData {
    unsigned int data;
    unsigned int addr;
    unsigned int size;
    unsigned int mode;
} sceSifDmaData;

extern sceSdTransIntrHandler _sce_sdr_transIntr0Hdr;
extern sceSdTransIntrHandler _sce_sdr_transIntr1Hdr;
extern sceSdSpu2IntrHandler _sce_sdr_spu2IntrHdr;
extern void *_sce_sdr_transIntr0Arg;
extern void *_sce_sdr_transIntr1Arg;
extern void *_sce_sdr_spu2IntrArg;
static char stack[256];
static int sbuff[16] __attribute__((aligned(64)));
static sceSifClientData sceSd_gCd;

extern int printf(const char *fmt, ...);
extern void FlushCache(int mode);
extern void sceSifInitRpc(unsigned int mode);
extern int sceSifBindRpc(sceSifClientData *client, unsigned int request,
                         unsigned int mode);
extern int sceSifCallRpc(sceSifClientData *client, unsigned int fno,
                         unsigned int mode, void *send, int ssize,
                         void *receive, int rsize, void (*end_func)(void *),
                         void *end_param);
extern unsigned int sceSifSetDma(sceSifDmaData *sdd, int len);
extern int sceSifDmaStat(unsigned int id);

static char __ps2_libinfo__[16] = "PsIIlibsdr  2300";
static sceSdSpu2IntrHandler sceSd_gEnd_func = 0;


int sceSdRemoteInit(void)
{
    int wait;

    sceSifInitRpc(0);
    while (1) {
        if (sceSifBindRpc(&sceSd_gCd, SD_RPC_SERVER, 0) < 0) {
            printf("sceSdRemoteInit() RPC bind error!\n");
            return -1;
        }
        wait = SD_BIND_WAIT;
        while (wait--) {
        }
        if (sceSd_gCd.server != 0) {
            break;
        }
    }
    FlushCache(0);
    return 0;
}


int sceSdTransToIOP(void *buff, unsigned int sendAddr, unsigned int size, unsigned int isBlock)
{
    static sceSifDmaData transData;
    unsigned int did;

    if (isBlock == 0) {
        return -1;
    }
    transData.data = (unsigned int)buff;
    transData.addr = sendAddr;
    transData.size = size;
    transData.mode = 0;
    did = sceSifSetDma(&transData, 1);
    if (did == 0) {
        return -1;
    }
    if (isBlock == 1) {
        while (sceSifDmaStat(did) >= 0) {
        }
    }
    return 0;
}


/* Install the SPU2 interrupt callback; returns the previous one. */
sceSdSpu2IntrHandler sceSdCallBack(sceSdSpu2IntrHandler func)
{
    sceSdSpu2IntrHandler old;

    old = sceSd_gEnd_func;
    sceSd_gEnd_func = func;
    return old;
}


int sceSdRemote(int arg, ...)
{
    va_list ap;
    unsigned int command;
    unsigned int mode;
    sceSdSpu2IntrHandler func;
    int i;
    int ret;
    int reply[4];

    func = 0;
    sbuff[0] = (int)sbuff;
    va_start(ap, arg);
    command = va_arg(ap, unsigned int);
    for (i = 1; i < 7; i++) {
        sbuff[i] = va_arg(ap, int);
    }
    va_end(ap);

    mode = 0;
    if (arg == 0) {
        mode = SIF_RPCM_NOWAIT;
        func = sceSd_gEnd_func;
    }

    if (command == SD_SET_TRANS_INTR_HANDLER) {
        if (sbuff[1] == 0) {
            _sce_sdr_transIntr0Hdr = (sceSdTransIntrHandler)sbuff[2];
            _sce_sdr_transIntr0Arg = (void *)sbuff[3];
        } else {
            _sce_sdr_transIntr1Hdr = (sceSdTransIntrHandler)sbuff[2];
            _sce_sdr_transIntr1Arg = (void *)sbuff[3];
        }
    } else if (command == SD_SET_SPU2_INTR_HANDLER) {
        _sce_sdr_spu2IntrHdr = (sceSdSpu2IntrHandler)sbuff[1];
        _sce_sdr_spu2IntrArg = (void *)sbuff[2];
    }

    if (command == SD_SPU2_PROC_BATCH) {
        sceSifCallRpc(&sceSd_gCd, sbuff[1] | SD_SPU2_PROC_BATCH, mode,
                      (void *)sbuff[2], 64, 0, 0,
                      (void (*)(void *))func, (void *)sbuff[0]);
        ret = sbuff[0];
    } else if (command == SD_SPU2_PROC_BATCH_EX) {
        sceSifCallRpc(&sceSd_gCd, sbuff[1] | SD_SPU2_PROC_BATCH_EX, mode,
                      sbuff, 64, (void *)sbuff[2], 64,
                      (void (*)(void *))func, (void *)sbuff[2]);
    } else {
        sceSifCallRpc(&sceSd_gCd, command, mode, sbuff, 64, sbuff, 16,
                      (void (*)(void *))func, sbuff);
        ret = sbuff[0];
    }
    return ret;
}
