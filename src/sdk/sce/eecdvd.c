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

typedef struct sceCdlFILE {
    unsigned int lsn;
    unsigned int size;
    char name[16];
    unsigned char date[8];
    unsigned int flag;
} sceCdlFILE;

typedef struct CdSearchFileData {
    sceCdlFILE file;
    char name[256];
    void *eedata;
} CdSearchFileData;

#define UNCACHED(p) ((void *)((unsigned int)(p) | 0x20000000))

extern int SCE_CD_debug[];
extern volatile int _sceCd_ncmd_semid[];
extern volatile int _sceCd_scmd_semid[];
extern volatile int _sceCd_c_cb_sem[];
extern volatile int sceCdCbfunc_num[];
extern volatile int sceCdCbfunc_number[];
extern unsigned int _sceCd_ncmdrdata[];
extern unsigned int _sceCd_scmdrdata[];
extern unsigned int _sceCd_scmdsdata[];
extern int _sceCd_ee_read_mode;
extern sceSifClientData _sceCd_cd_ncmd;
extern sceSifClientData _sceCd_cd_scmd;

extern int D_00610994[];
extern int D_00610998[];
extern int D_0061099C[];
extern volatile int D_006109A0[];
extern int D_006109A4[];
extern int D_006109B8[];
extern int D_006109BC[];
extern int D_006109C0[];
extern int D_006109C4[];
extern int D_006109C8[];
extern int D_006109CC[];
extern int D_006109D0[];
extern void (*D_006FFE80[])(int);
extern void (*D_006FFE84[])(void *);
extern void *D_006FFE88[];
extern int D_006FFE8C[];
extern int D_006FFE90[];
extern ThreadParam D_006FFE98[];
extern ThreadParam D_006FFEC8[];
extern CdSearchFileData D_006FFF00;
extern unsigned int D_00700040[];
extern sceSifClientData D_00700080;
extern sceSifClientData D_007000A8;
extern sceSifClientData D_007000D0;
extern int D_00700100[];
extern int D_00700110[];
extern char D_0062B060[];
extern char D_0062B088[];
extern char D_0062B0A8[];
extern char D_0062B0C0[];
extern char D_0062B0D0[];
extern char D_0062B0E0[];
extern char D_0062B0F8[];
extern char D_0062B120[];
extern char D_0062B138[];
extern char D_0062B148[];
extern char D_0062B158[];
extern char D_0062B180[];
extern char D_0062B198[];
extern char D_0062B1B8[];
extern char D_0062B1C8[];
extern char D_0062B1D8[];
extern char D_0062B1F8[];

#define cd_debug        (SCE_CD_debug[0])
#define ncmd_semid      (_sceCd_ncmd_semid[0])
#define scmd_semid      (_sceCd_scmd_semid[0])
#define c_cb_sem        (_sceCd_c_cb_sem[0])
#define cbfunc_num      (sceCdCbfunc_num[0])
#define cbfunc_number   (sceCdCbfunc_number[0])
#define cb_thread_id    (D_00610994[0])
#define scmd_sema_keep  (D_00610998[0])
#define ncmd_sema_keep  (D_0061099C[0])
#define cb_sema         (D_006109A0[0])
#define init_seq        (D_006109A4[0])
#define ncmd_bind       (D_006109B8[0])
#define icmd_bind       (D_006109BC[0])
#define sf_bind         (D_006109C0[0])
#define scmd_dr_bind    (D_006109C4[0])
#define scmd_bind       (D_006109C8[0])
#define it_bind         (D_006109CC[0])
#define init_count      (D_006109D0[0])
#define cb_func         (D_006FFE80[0])
#define poff_func       (D_006FFE84[0])
#define poff_arg        (D_006FFE88[0])
#define poff_func_num   (D_006FFE8C[0])
#define init_th_id      (D_006FFE90[0])
#define init_th_param   (D_006FFE98[0])
#define cb_th_param     (D_006FFEC8[0])
#define sf_data         D_006FFF00
#define sf_rdata        D_00700040
#define sf_client       D_00700080
#define init_client     D_007000A8
#define init_mode       D_00700100
#define dr_client       D_007000D0
#define dr_mode         D_00700110
#define cbloop_msg      D_0062B060
#define sf_bind_err_msg D_0062B088
#define sf_name_msg     D_0062B0A8
#define sf_found_msg    D_0062B0C0
#define sf_size_msg     D_0062B0D0
#define sf_lsn_msg      D_0062B0E0
#define ncmd_fail_msg   D_0062B0F8
#define ncmd_bind_err_msg D_0062B120
#define ncmd_wait_msg   D_0062B138
#define scmd_wait_msg   D_0062B148
#define scmd_fail_msg   D_0062B158
#define scmd_bind_err_msg D_0062B180
#define init_bind_err_msg D_0062B198
#define exit_msg        D_0062B1B8
#define dr_msg          D_0062B1C8
#define dr_bind_err_msg D_0062B1D8
#define dr_done_msg     D_0062B1F8

extern int CreateSema(SemaParam *param);
extern int DeleteSema(int sema);
extern int WaitSema(int sema);
extern int SignalSema(int sema);
extern int iSignalSema(int sema);
extern int PollSema(int sema);
extern int SetAlarm(unsigned short time, void (*cb)(int, unsigned short, void *), void *arg);
extern int DIntr(void);
extern int EIntr(void);
extern void scePrintf(const char *fmt, ...);
extern void sceSifAddCmdHandler(unsigned int cid, void (*handler)(void *, void *), void *data);
extern void sceSifRemoveCmdHandler(unsigned int cid);
extern int sceSifCallRpc(sceSifClientData *client, unsigned int fno, unsigned int mode,
                         void *send, int ssize, void *receive, int rsize,
                         void (*end_func)(void *), void *end_param);
extern int sceSifCheckStatRpc(sceSifClientData *client);
extern void sceSifInitRpc(unsigned int mode);
extern int sceSifBindRpc(sceSifClientData *client, unsigned int request, unsigned int mode);
extern int GetThreadId(void);
extern int ReferThreadStatus(int thread, ThreadParam *info);
extern int CreateThread(ThreadParam *param);
extern int StartThread(int thread, void *arg);
extern int ChangeThreadPriority(int thread, int priority);
extern void ExitDeleteThread(void);
extern void *memcpy(void *dest, const void *src, unsigned int n);
extern void sceSifWriteBackDCache(void *addr, int size);
extern char _gp[];
static void _Cdvd_cbLoop(void *arg);
void CB_DelayTh(int id, unsigned short time, void *arg);
int sceCdSync(int mode);
static int PowerOffCB(void);
int _sceCd_ncmd_prechk(int cmd);
int _sceCd_scmd_prechk(int cmd);


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


static void _Cdvd_cbLoop(void *arg)
{
    while (1) {
        WaitSema(cb_sema);
        if (cbfunc_num == -1) {
            c_cb_sem = 0;
            *(int *)sceCdCbfunc_num = 0;
            cb_thread_id = 0;
            poff_func_num = 0;
            ExitDeleteThread();
        }
        if (cd_debug > 0) {
            scePrintf(cbloop_msg, cb_func, cbfunc_number);
        }
        if (cb_func && cbfunc_number) {
            cb_func(cbfunc_number);
        }
        c_cb_sem = 0;
    }
}


/* Start the callback thread at priority `prio` on the given stack; if it is
 * already running, only change its priority. Returns 1 if it was created. */
int sceCdInitEeCB(int prio, void *stack, int stack_size)
{
    int created = 1;

    if (cb_thread_id == 0) {
        init_th_id = GetThreadId();
        ReferThreadStatus(init_th_id, &init_th_param);
        cb_th_param.stackSize = stack_size;
        cb_th_param.gpReg = _gp;
        cb_th_param.entry = _Cdvd_cbLoop;
        cb_th_param.stack = stack;
        cb_th_param.initPriority = prio;
        cb_thread_id = CreateThread(&cb_th_param);
        StartThread(cb_thread_id, 0);
    } else {
        ChangeThreadPriority(cb_thread_id, prio);
        created = 0;
    }
    return created;
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


static void cmd_sem_init(void)
{
    SemaParam param;

    if (ncmd_semid == -1 || scmd_semid == -1) {
        param.initCount = 1;
        param.maxCount = 1;
        param.option = 0;
        ncmd_semid = CreateSema(&param);
        scmd_semid = CreateSema(&param);
        param.initCount = 0;
        cb_sema = CreateSema(&param);
        c_cb_sem = 0;
    }
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

    if (icmd_bind < 0) {
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
    if (poff_func && !init_seq) {
        poff_func(poff_arg);
    }
}


static int PowerOffCB(void)
{
    init_seq = 1;
    DIntr();
    sceSifAddCmdHandler(0x80000012, _sceCd_Poff_Intr, 0);
    EIntr();
    init_seq = 0;
    icmd_bind = 1;
    return 1;
}


int sceCdSearchFile(sceCdlFILE *file, const char *name)
{
    int i;
    int wait;
    int result;

    cmd_sem_init();
    if (ncmd_semid != PollSema(ncmd_semid)) {
        return 0;
    }
    ncmd_sema_keep = 1;
    ReferThreadStatus(init_th_id, &init_th_param);
    if (sceCdSync(1)) {
        SignalSema(ncmd_semid);
        return 0;
    }
    sceSifInitRpc(0);
    if (sf_bind < 0) {
        while (1) {
            if (sceSifBindRpc(&sf_client, 0x80000597, 0) < 0) {
                if (cd_debug > 0) {
                    scePrintf(sf_bind_err_msg);
                }
                wait = 0x100000;
                while (wait--) {
                }
                continue;
            }
            if (sf_client.serve != 0) {
                sf_bind = 0;
                break;
            }
            wait = 0x100000;
            while (wait--) {
            }
        }
    }
    for (i = 0; i < 256; i++) {
        if ((sf_data.name[i] = name[i]) == 0) {
            break;
        }
    }
    if (i == 256) {
        sf_data.name[255] = 0;
    }
    sf_data.eedata = &sf_data;
    if (cd_debug > 0) {
        scePrintf(sf_name_msg, sf_data.name);
    }
    sceSifWriteBackDCache(&sf_data, sizeof(sf_data));
    if (sceSifCallRpc(&sf_client, 0, 0, &sf_data, sizeof(sf_data), sf_rdata, 4, 0, 0) < 0) {
        SignalSema(ncmd_semid);
        return 0;
    }
    memcpy(file, UNCACHED(&sf_data), sizeof(sceCdlFILE));
    if (cd_debug > 0) {
        scePrintf(sf_found_msg, file->name);
    }
    if (cd_debug > 0) {
        scePrintf(sf_size_msg, file->size);
    }
    if (cd_debug > 0) {
        scePrintf(sf_lsn_msg, file->lsn);
    }
    result = *(int *)UNCACHED(sf_rdata);
    SignalSema(ncmd_semid);
    return result;
}


int _sceCd_ncmd_prechk(int cmd)
{
    int wait;

    cmd_sem_init();
    if (ncmd_semid != PollSema(ncmd_semid)) {
        if (cd_debug > 0) {
            scePrintf(ncmd_fail_msg, cmd, ncmd_sema_keep);
        }
        return 0;
    }
    ncmd_sema_keep = cmd;
    ReferThreadStatus(init_th_id, &init_th_param);
    if (sceCdSync(1)) {
        SignalSema(ncmd_semid);
        return 0;
    }
    sceSifInitRpc(0);
    if (ncmd_bind < 0) {
        while (1) {
            if (sceSifBindRpc(&_sceCd_cd_ncmd, 0x80000595, 0) < 0) {
                if (cd_debug > 0) {
                    scePrintf(ncmd_bind_err_msg);
                }
                wait = 0x100000;
                while (wait--) {
                }
                continue;
            }
            if (_sceCd_cd_ncmd.serve != 0) {
                ncmd_bind = 0;
                break;
            }
            wait = 0x100000;
            while (wait--) {
            }
        }
    }
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


int _sceCd_scmd_prechk(int cmd)
{
    int wait;

    cmd_sem_init();
    if (scmd_semid != PollSema(scmd_semid)) {
        if (cd_debug > 0) {
            scePrintf(scmd_fail_msg, cmd, scmd_sema_keep);
        }
        return 0;
    }
    scmd_sema_keep = cmd;
    ReferThreadStatus(init_th_id, &init_th_param);
    if (sceCdSyncS(1)) {
        SignalSema(scmd_semid);
        return 0;
    }
    sceSifInitRpc(0);
    if (scmd_bind < 0) {
        while (1) {
            if (sceSifBindRpc(&_sceCd_cd_scmd, 0x80000593, 0) < 0) {
                if (cd_debug > 0) {
                    scePrintf(scmd_bind_err_msg);
                }
                wait = 0x100000;
                while (wait--) {
                }
                continue;
            }
            if (_sceCd_cd_scmd.serve != 0) {
                scmd_bind = 0;
                break;
            }
            wait = 0x100000;
            while (wait--) {
            }
        }
    }
    return 1;
}


int sceCdInit(int mode)
{
    int result;
    int bind;
    int wait;
    int status;
    int drv_version;
    int mod_version;

    if (sceCdSyncS(1)) {
        return 0;
    }
    sceSifInitRpc(0);
    init_th_id = GetThreadId();
    init_seq = 1;
    icmd_bind = -1;
    sf_bind = -1;
    ncmd_bind = -1;
    scmd_bind = -1;
    scmd_dr_bind = -1;
    _sceCd_ee_read_mode = 0;
    init_count++;
    it_bind = -1;
    while (1) {
        bind = sceSifBindRpc(&init_client, 0x80000592, 0);
        if (bind < 0) {
            if (cd_debug > 0) {
                scePrintf(init_bind_err_msg, bind, init_count);
            }
            wait = 0x100000;
            while (wait--) {
            }
            continue;
        }
        if (init_client.serve != 0) {
            it_bind = 0;
            break;
        }
        wait = 0x100000;
        while (wait--) {
        }
    }
    init_mode[0] = mode;
    sceSifWriteBackDCache(init_mode, 4);
    if (sceSifCallRpc(&init_client, 0, 0, init_mode, 4, _sceCd_scmdrdata, 0x10, 0, 0) < 0) {
        init_seq = 0;
        return 0;
    }
    status = *(int *)UNCACHED(&_sceCd_scmdrdata[3]);
    drv_version = *(int *)UNCACHED(&_sceCd_scmdrdata[1]);
    mod_version = *(int *)UNCACHED(&_sceCd_scmdrdata[2]);
    result = 1;
    if (status != 0xff) {
        if (status == 0xfe) {
            cd_debug = 1;
        } else if (drv_version / 0x100 < 2 || mod_version / 0x100 < 2) {
            result = 2;
        }
    }
    init_seq = 0;
    switch (mode) {
    case 0:
    case 1:
        break;
    case 5:
        if (cd_debug > 0) {
            scePrintf(exit_msg);
        }
        cdvd_exit();
        ncmd_semid = -1;
        scmd_semid = -1;
        cb_sema = -1;
        return result;
    }
    cmd_sem_init();
    PowerOffCB();
    return result;
}


int sceCdDiskReady(int mode)
{
    int wait;
    int result;

    if (cd_debug > 0) {
        scePrintf(dr_msg);
    }
    cmd_sem_init();
    if (scmd_semid != PollSema(scmd_semid)) {
        return 6;
    }
    if (sceCdSyncS(1)) {
        SignalSema(scmd_semid);
        if (mode == 8) {
            return -1;
        }
        return 6;
    }
    sceSifInitRpc(0);
    if (scmd_dr_bind < 0) {
        while (1) {
            if (sceSifBindRpc(&dr_client, 0x8000059A, 0) < 0) {
                if (cd_debug > 0) {
                    scePrintf(dr_bind_err_msg);
                }
                wait = 0x100000;
                while (wait--) {
                }
                continue;
            }
            if (dr_client.serve != 0) {
                scmd_dr_bind = 0;
                break;
            }
            wait = 0x100000;
            while (wait--) {
            }
        }
    }
    dr_mode[0] = mode;
    sceSifWriteBackDCache(dr_mode, 4);
    if (sceSifCallRpc(&dr_client, 0, 0, dr_mode, 4, _sceCd_scmdrdata, 4, 0, 0) < 0) {
        SignalSema(scmd_semid);
        if (mode == 8) {
            return -1;
        }
        return 6;
    }
    if (cd_debug > 0) {
        scePrintf(dr_done_msg);
    }
    result = *(int *)UNCACHED(_sceCd_scmdrdata);
    SignalSema(scmd_semid);
    return result;
}


int sceCdMmode(int media)
{
    unsigned int *sdata = _sceCd_scmdsdata;
    int result;

    if (!_sceCd_scmd_prechk(0x22)) {
        return 0;
    }
    _sceCd_scmdsdata[0] = media;
    sceSifWriteBackDCache(sdata, 4);
    if (sceSifCallRpc(&_sceCd_cd_scmd, 0x22, 0, sdata, 4, _sceCd_scmdrdata, 4, 0, 0) < 0) {
        SignalSema(scmd_semid);
        return 0;
    }
    result = *(int *)UNCACHED(_sceCd_scmdrdata);
    SignalSema(scmd_semid);
    return result;
}
