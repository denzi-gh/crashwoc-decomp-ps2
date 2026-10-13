/*
 * Unit: sdk/sce/eecdvd
 *
 * Functions:
 *   0x00265950 CB_DelayTh
 *   0x00265978 sceCdDelayThread
 *   0x002659e0 sceCdCallback
 *   0x00265a38 _sceCd_cd_callback
 *   0x00265ad8 _Cdvd_cbLoop
 *   0x00265b98 sceCdInitEeCB
 *   0x00265c70 _sceCd_cd_read_intr
 *   0x00265d10 cmd_sem_init
 *   0x00265da8 cdvd_exit
 *   0x00265e28 sceCdPOffCallback
 *   0x00265e98 _sceCd_Poff_Intr
 *   0x00265ed8 PowerOffCB
 *   0x00265f40 sceCdSearchFile
 *   0x00266240 _sceCd_ncmd_prechk
 *   0x002663b0 sceCdNcmdDiskReady
 *   0x00266448 sceCdSync
 *   0x002664e8 sceCdSyncS
 *   0x00266558 _sceCd_scmd_prechk
 *   0x002666c8 sceCdInit
 *   0x002669a8 sceCdDiskReady
 *   0x00266ba0 sceCdMmode
 */

typedef struct SemaParam {
    int currentCount;
    int maxCount;
    int initCount;
    int numWaitThreads;
    unsigned int attr;
    unsigned int option;
} SemaParam;

typedef struct sceSifRpcHeader {
    void *pkt_addr;
    unsigned int rpc_id;
    int sema_id;
    unsigned int mode;
} sceSifRpcHeader;

typedef struct sceSifClientData {
    sceSifRpcHeader hdr;
    unsigned int command;
    void *buff;
    void *cbuff;
    void *end_function;
    void *end_param;
    void *serve;
} sceSifClientData;

typedef struct CdReadIntrData {
    int size1;
    int size2;
    unsigned char *dest1;
    unsigned char *dest2;
    unsigned char buf1[64];
    unsigned char buf2[64];
} CdReadIntrData;

#define UNCACHED(p) ((void *)((unsigned int)(p) | 0x20000000))

extern int SCE_CD_debug[];
extern volatile int _sceCd_ncmd_semid[];
extern volatile int _sceCd_scmd_semid[];
extern volatile int _sceCd_c_cb_sem[];
extern volatile int sceCdCbfunc_num[];
extern volatile int sceCdCbfunc_number[];
extern unsigned int _sceCd_ncmdrdata[];
extern sceSifClientData _sceCd_cd_ncmd;
extern sceSifClientData _sceCd_cd_scmd;

extern int D_00610994[];
extern volatile int D_006109A0[];
extern int D_006109A4[];
extern int D_006109BC[];
extern void (*D_006FFE80[])(int);
extern void (*D_006FFE84[])(void *);
extern void *D_006FFE88[];
extern char D_0062B138[];
extern char D_0062B148[];

#define cd_debug        (SCE_CD_debug[0])
#define ncmd_semid      (_sceCd_ncmd_semid[0])
#define scmd_semid      (_sceCd_scmd_semid[0])
#define c_cb_sem        (_sceCd_c_cb_sem[0])
#define cbfunc_num      (sceCdCbfunc_num[0])
#define cbfunc_number   (sceCdCbfunc_number[0])
#define cb_thread_id    (D_00610994[0])
#define cb_sema         (D_006109A0[0])
#define poff_busy       (D_006109A4[0])
#define poff_init       (D_006109BC[0])
#define cb_func         (D_006FFE80[0])
#define poff_func       (D_006FFE84[0])
#define poff_arg        (D_006FFE88[0])
#define ncmd_wait_msg   D_0062B138
#define scmd_wait_msg   D_0062B148

extern int CreateSema(SemaParam *param);
extern int DeleteSema(int sema);
extern int WaitSema(int sema);
extern int SignalSema(int sema);
extern int iSignalSema(int sema);
extern int SetAlarm(unsigned short time, void (*cb)(int, unsigned short, void *), void *arg);
extern int DIntr(void);
extern int EIntr(void);
extern int scePrintf(const char *fmt, ...);
extern void sceSifAddCmdHandler(unsigned int cid, void (*handler)(void *, void *), void *data);
extern void sceSifRemoveCmdHandler(unsigned int cid);
extern int sceSifCallRpc(sceSifClientData *client, unsigned int fno, unsigned int mode,
                         void *send, int ssize, void *receive, int rsize,
                         void (*end_func)(void *), void *end_param);
extern int sceSifCheckStatRpc(sceSifClientData *client);
void CB_DelayTh(int id, unsigned short time, void *arg);
int sceCdSync(int mode);
static int PowerOffCB(void);
int _sceCd_ncmd_prechk(int cmd);


void sceCdDelayThread(unsigned short hsync)
{
    SemaParam param;
    int sema;

    param.initCount = 0;
    param.maxCount = 1;
    param.option = 0;
    sema = CreateSema(&param);
    SetAlarm(hsync, CB_DelayTh, (void *)sema);
    WaitSema(sema);
    DeleteSema(sema);
}


void *sceCdCallback(void (*func)(int))
{
    void *old;

    if (sceCdSync(1)) {
        return 0;
    }
    DIntr();
    old = cb_func;
    cb_func = func;
    EIntr();
    return old;
}


static void _sceCd_cd_callback(int *data)
{
    cbfunc_num = *data;
    cbfunc_number = cbfunc_num;
    if (cbfunc_num == 11) {
        cbfunc_num = 0;
        c_cb_sem = 0;
        return;
    }
    iSignalSema(ncmd_semid);
    if (cb_thread_id && cb_func) {
        iSignalSema(cb_sema);
    } else {
        c_cb_sem = 0;
    }
    cbfunc_num = 0;
}


static void _sceCd_cd_read_intr(void *addr)
{
    CdReadIntrData *data = UNCACHED(addr);
    unsigned char *dest;
    int i;

    if (data->size1 > 0) {
        dest = data->dest1;
        for (i = 0; i < data->size1; i++) {
            dest[i] = data->buf1[i];
        }
    }
    if (data->size2 > 0) {
        dest = data->dest2;
        for (i = 0; i < data->size2; i++) {
            dest[i] = data->buf2[i];
        }
    }
    _sceCd_cd_callback((int *)sceCdCbfunc_num);
}


void cdvd_exit(void)
{
    if (cb_thread_id) {
        cbfunc_num = -1;
        SignalSema(cb_sema);
    }
    DeleteSema(ncmd_semid);
    DeleteSema(scmd_semid);
    DeleteSema(cb_sema);
    DIntr();
    sceSifRemoveCmdHandler(0x80000012);
    EIntr();
}


void *sceCdPOffCallback(void (*func)(void *), void *addr)
{
    void *old;

    if (poff_init < 0) {
        PowerOffCB();
    }
    DIntr();
    old = poff_func;
    poff_arg = addr;
    poff_func = func;
    EIntr();
    return old;
}


static void _sceCd_Poff_Intr(void *data, void *arg)
{
    if (poff_func && !poff_busy) {
        poff_func(poff_arg);
    }
}


static int PowerOffCB(void)
{
    poff_busy = 1;
    DIntr();
    sceSifAddCmdHandler(0x80000012, _sceCd_Poff_Intr, 0);
    EIntr();
    poff_busy = 0;
    poff_init = 1;
    return 1;
}


int sceCdNcmdDiskReady(void)
{
    int ready;

    if (!_sceCd_ncmd_prechk(2)) {
        return 0;
    }
    if (sceSifCallRpc(&_sceCd_cd_ncmd, 14, 0, 0, 0, _sceCd_ncmdrdata, 4, 0, 0) < 0) {
        SignalSema(ncmd_semid);
        return 0;
    }
    ready = *(int *)UNCACHED(_sceCd_ncmdrdata);
    SignalSema(ncmd_semid);
    return ready;
}


int sceCdSync(int mode)
{
    if (mode == 0) {
        if (cd_debug > 0) {
            scePrintf(ncmd_wait_msg);
        }
        while (c_cb_sem || sceSifCheckStatRpc(&_sceCd_cd_ncmd)) {
            sceCdDelayThread(60);
        }
        return 0;
    }
    if (c_cb_sem || sceSifCheckStatRpc(&_sceCd_cd_ncmd)) {
        return 1;
    }
    return 0;
}


int sceCdSyncS(int mode)
{
    if (mode == 0) {
        if (cd_debug > 0) {
            scePrintf(scmd_wait_msg);
        }
        while (sceSifCheckStatRpc(&_sceCd_cd_scmd)) {
            sceCdDelayThread(60);
        }
        return 0;
    }
    return sceSifCheckStatRpc(&_sceCd_cd_scmd);
}
