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

static char __ps2_libinfo__[16] = "PsIIlibcdvd 2300";
int SCE_CD_debug = 0;
static int cb_thid = 0;
static int scmd_sema_keep_cmd = 0;
static int ncmd_sema_keep_cmd = 0;
static volatile int cb_semid = -1;
static volatile int Init_seq = 0;
volatile int _sceCd_ncmd_semid = -1;
volatile int _sceCd_scmd_semid = -1;
volatile int _sceCd_c_cb_sem = 0;
volatile int _sceCd_ee_read_mode = 0;
static int _ncmd_bind = -1;
static int _icmd_bind = -1;
static int _sf_bind = -1;
static int _scmd_dr_bind = -1;
static int _scmd_bind = -1;
static int _it_bind = -1;

extern volatile int sceCdCbfunc_num;
extern volatile int sceCdCbfunc_number;
extern unsigned int _sceCd_ncmdrdata[24];
extern unsigned int _sceCd_scmdrdata[264];
extern unsigned int _sceCd_scmdsdata[258];
extern sceSifClientData _sceCd_cd_ncmd;
extern sceSifClientData _sceCd_cd_scmd;

static void (*sceCdCbfunc)(int);
static void (*sceCdPoffCbfunc)(void *);
static void *sceCdPoffCbdata;
static int sceCdPoffCbfunc_num;
static int my_thid;
static ThreadParam my_th_info;
static ThreadParam cb_tp;
static CdSearchFileData _sf_data __attribute__((aligned(64)));
static unsigned int sfrdata[16] __attribute__((aligned(64)));
static sceSifClientData cdrdsf;
static sceSifClientData cdit;
static sceSifClientData cddr;
static unsigned int _i_mode[4] __attribute__((aligned(16)));
static unsigned int _dr_mode[4] __attribute__((aligned(16)));

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


void CB_DelayTh(int id, unsigned short time, void *arg)
{
    iSignalSema((int)arg);
    __asm__ volatile ("sync\n\tei");
}


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
    old = sceCdCbfunc;
    sceCdCbfunc = func;
    EIntr();
    return old;
}


static void _sceCd_cd_callback(int *data)
{
    sceCdCbfunc_num = *data;
    sceCdCbfunc_number = sceCdCbfunc_num;
    if (sceCdCbfunc_num == 11) {
        sceCdCbfunc_num = 0;
        _sceCd_c_cb_sem = 0;
        return;
    }
    iSignalSema(_sceCd_ncmd_semid);
    if (cb_thid && sceCdCbfunc) {
        iSignalSema(cb_semid);
    } else {
        _sceCd_c_cb_sem = 0;
    }
    sceCdCbfunc_num = 0;
}


static void _Cdvd_cbLoop(void *arg)
{
    while (1) {
        WaitSema(cb_semid);
        if (sceCdCbfunc_num == -1) {
            _sceCd_c_cb_sem = 0;
            *(int *)&sceCdCbfunc_num = 0;
            cb_thid = 0;
            sceCdPoffCbfunc_num = 0;
            ExitDeleteThread();
        }
        if (SCE_CD_debug > 0) {
            scePrintf("sceCdCbfunc= %d sceCdCbfunc_num= %d\n", sceCdCbfunc, sceCdCbfunc_number);
        }
        if (sceCdCbfunc && sceCdCbfunc_number) {
            sceCdCbfunc(sceCdCbfunc_number);
        }
        _sceCd_c_cb_sem = 0;
    }
}


/* Start the callback thread at priority `prio` on the given stack; if it is
 * already running, only change its priority. Returns 1 if it was created. */
int sceCdInitEeCB(int prio, void *stack, int stack_size)
{
    int created = 1;

    if (cb_thid == 0) {
        my_thid = GetThreadId();
        ReferThreadStatus(my_thid, &my_th_info);
        cb_tp.stackSize = stack_size;
        cb_tp.gpReg = _gp;
        cb_tp.entry = _Cdvd_cbLoop;
        cb_tp.stack = stack;
        cb_tp.initPriority = prio;
        cb_thid = CreateThread(&cb_tp);
        StartThread(cb_thid, 0);
    } else {
        ChangeThreadPriority(cb_thid, prio);
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
    _sceCd_cd_callback((int *)&sceCdCbfunc_num);
}


static void cmd_sem_init(void)
{
    SemaParam param;

    if (_sceCd_ncmd_semid == -1 || _sceCd_scmd_semid == -1) {
        param.initCount = 1;
        param.maxCount = 1;
        param.option = 0;
        _sceCd_ncmd_semid = CreateSema(&param);
        _sceCd_scmd_semid = CreateSema(&param);
        param.initCount = 0;
        cb_semid = CreateSema(&param);
        _sceCd_c_cb_sem = 0;
    }
}


void cdvd_exit(void)
{
    if (cb_thid) {
        sceCdCbfunc_num = -1;
        SignalSema(cb_semid);
    }
    DeleteSema(_sceCd_ncmd_semid);
    DeleteSema(_sceCd_scmd_semid);
    DeleteSema(cb_semid);
    DIntr();
    sceSifRemoveCmdHandler(0x80000012);
    EIntr();
}


void *sceCdPOffCallback(void (*func)(void *), void *addr)
{
    void *old;

    if (_icmd_bind < 0) {
        PowerOffCB();
    }
    DIntr();
    old = sceCdPoffCbfunc;
    sceCdPoffCbdata = addr;
    sceCdPoffCbfunc = func;
    EIntr();
    return old;
}


static void _sceCd_Poff_Intr(void *data, void *arg)
{
    if (sceCdPoffCbfunc && !Init_seq) {
        sceCdPoffCbfunc(sceCdPoffCbdata);
    }
}


static int PowerOffCB(void)
{
    Init_seq = 1;
    DIntr();
    sceSifAddCmdHandler(0x80000012, _sceCd_Poff_Intr, 0);
    EIntr();
    Init_seq = 0;
    _icmd_bind = 1;
    return 1;
}


int sceCdSearchFile(sceCdlFILE *file, const char *name)
{
    int i;
    int wait;
    int result;

    cmd_sem_init();
    if (_sceCd_ncmd_semid != PollSema(_sceCd_ncmd_semid)) {
        return 0;
    }
    ncmd_sema_keep_cmd = 1;
    ReferThreadStatus(my_thid, &my_th_info);
    if (sceCdSync(1)) {
        SignalSema(_sceCd_ncmd_semid);
        return 0;
    }
    sceSifInitRpc(0);
    if (_sf_bind < 0) {
        while (1) {
            if (sceSifBindRpc(&cdrdsf, 0x80000597, 0) < 0) {
                if (SCE_CD_debug > 0) {
                    scePrintf("Libcdvd bind err CdSearchFile\n");
                }
                wait = 0x100000;
                while (wait--) {
                }
                continue;
            }
            if (cdrdsf.serve != 0) {
                _sf_bind = 0;
                break;
            }
            wait = 0x100000;
            while (wait--) {
            }
        }
    }
    for (i = 0; i < 256; i++) {
        if ((_sf_data.name[i] = name[i]) == 0) {
            break;
        }
    }
    if (i == 256) {
        _sf_data.name[255] = 0;
    }
    _sf_data.eedata = &_sf_data;
    if (SCE_CD_debug > 0) {
        scePrintf("ee call cmd search %s\n", _sf_data.name);
    }
    sceSifWriteBackDCache(&_sf_data, sizeof(_sf_data));
    if (sceSifCallRpc(&cdrdsf, 0, 0, &_sf_data, sizeof(_sf_data), sfrdata, 4, 0, 0) < 0) {
        SignalSema(_sceCd_ncmd_semid);
        return 0;
    }
    memcpy(file, UNCACHED(&_sf_data), sizeof(sceCdlFILE));
    if (SCE_CD_debug > 0) {
        scePrintf("search name %s\n", file->name);
    }
    if (SCE_CD_debug > 0) {
        scePrintf("search size %d\n", file->size);
    }
    if (SCE_CD_debug > 0) {
        scePrintf("search loc lbn %d\n", file->lsn);
    }
    result = *(int *)UNCACHED(sfrdata);
    SignalSema(_sceCd_ncmd_semid);
    return result;
}


int _sceCd_ncmd_prechk(int cmd)
{
    int wait;

    cmd_sem_init();
    if (_sceCd_ncmd_semid != PollSema(_sceCd_ncmd_semid)) {
        if (SCE_CD_debug > 0) {
            scePrintf("Ncmd fail sema cur_cmd:%d keep_cmd:%d\n", cmd, ncmd_sema_keep_cmd);
        }
        return 0;
    }
    ncmd_sema_keep_cmd = cmd;
    ReferThreadStatus(my_thid, &my_th_info);
    if (sceCdSync(1)) {
        SignalSema(_sceCd_ncmd_semid);
        return 0;
    }
    sceSifInitRpc(0);
    if (_ncmd_bind < 0) {
        while (1) {
            if (sceSifBindRpc(&_sceCd_cd_ncmd, 0x80000595, 0) < 0) {
                if (SCE_CD_debug > 0) {
                    scePrintf("Libcdvd bind err N CMD\n");
                }
                wait = 0x100000;
                while (wait--) {
                }
                continue;
            }
            if (_sceCd_cd_ncmd.serve != 0) {
                _ncmd_bind = 0;
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
        SignalSema(_sceCd_ncmd_semid);
        return 0;
    }
    ready = *(int *)UNCACHED(_sceCd_ncmdrdata);
    SignalSema(_sceCd_ncmd_semid);
    return ready;
}


int sceCdSync(int mode)
{
    if (mode == 0) {
        if (SCE_CD_debug > 0) {
            scePrintf("N cmd wait\n");
        }
        while (_sceCd_c_cb_sem || sceSifCheckStatRpc(&_sceCd_cd_ncmd)) {
            sceCdDelayThread(60);
        }
        return 0;
    }
    if (_sceCd_c_cb_sem || sceSifCheckStatRpc(&_sceCd_cd_ncmd)) {
        return 1;
    }
    return 0;
}


int sceCdSyncS(int mode)
{
    if (mode == 0) {
        if (SCE_CD_debug > 0) {
            scePrintf("S cmd wait\n");
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
    if (_sceCd_scmd_semid != PollSema(_sceCd_scmd_semid)) {
        if (SCE_CD_debug > 0) {
            scePrintf("Scmd fail sema cur_cmd:%d keep_cmd:%d\n", cmd, scmd_sema_keep_cmd);
        }
        return 0;
    }
    scmd_sema_keep_cmd = cmd;
    ReferThreadStatus(my_thid, &my_th_info);
    if (sceCdSyncS(1)) {
        SignalSema(_sceCd_scmd_semid);
        return 0;
    }
    sceSifInitRpc(0);
    if (_scmd_bind < 0) {
        while (1) {
            if (sceSifBindRpc(&_sceCd_cd_scmd, 0x80000593, 0) < 0) {
                if (SCE_CD_debug > 0) {
                    scePrintf("Libcdvd bind err S cmd\n");
                }
                wait = 0x100000;
                while (wait--) {
                }
                continue;
            }
            if (_sceCd_cd_scmd.serve != 0) {
                _scmd_bind = 0;
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
    static int c_cnt = 0;

    if (sceCdSyncS(1)) {
        return 0;
    }
    sceSifInitRpc(0);
    my_thid = GetThreadId();
    Init_seq = 1;
    _sf_bind = -1;
    _ncmd_bind = -1;
    _scmd_bind = -1;
    _scmd_dr_bind = -1;
    _it_bind = -1;
    _icmd_bind = -1;
    _sceCd_ee_read_mode = 0;
    c_cnt++;
    while (1) {
        bind = sceSifBindRpc(&cdit, 0x80000592, 0);
        if (bind < 0) {
            if (SCE_CD_debug > 0) {
                scePrintf("Libcdvd bind err %d CD_Init %d\n", bind, c_cnt);
            }
            wait = 0x100000;
            while (wait--) {
            }
            continue;
        }
        if (cdit.serve != 0) {
            _it_bind = 0;
            break;
        }
        wait = 0x100000;
        while (wait--) {
        }
    }
    _i_mode[0] = mode;
    sceSifWriteBackDCache(_i_mode, 4);
    if (sceSifCallRpc(&cdit, 0, 0, _i_mode, 4, _sceCd_scmdrdata, 0x10, 0, 0) < 0) {
        Init_seq = 0;
        return 0;
    }
    drv_version = *(int *)UNCACHED(&_sceCd_scmdrdata[1]);
    mod_version = *(int *)UNCACHED(&_sceCd_scmdrdata[2]);
    status = *(int *)UNCACHED(&_sceCd_scmdrdata[3]);
    result = 1;
    if (status != 0xff) {
        if (status == 0xfe) {
            SCE_CD_debug = 1;
        } else if (drv_version / 0x100 < 2 || mod_version / 0x100 < 2) {
            result = 2;
        }
    }
    Init_seq = 0;
    switch (mode) {
    case 5:
        if (SCE_CD_debug > 0) {
            scePrintf("Libcdvd Exit\n");
        }
        cdvd_exit();
        _sceCd_ncmd_semid = -1;
        _sceCd_scmd_semid = -1;
        cb_semid = -1;
        break;
    case 0:
    case 1:
    default:
        cmd_sem_init();
        PowerOffCB();
        break;
    }
    return result;
}

volatile int sceCdCbfunc_num = 0;
volatile int sceCdCbfunc_number = 0;
unsigned int _sceCd_ncmdrdata[24] __attribute__((aligned(64))) = { 0 };
unsigned int _sceCd_ncmdsdata[1024] __attribute__((aligned(64))) = { 0 };
CdReadIntrData _sceCd_rd_intr_data __attribute__((aligned(64))) = { 0 };
int _sceCd_Read_cur_pos[4] __attribute__((aligned(64))) = { 0 };
sceSifClientData _sceCd_cd_ncmd = { 0 };
unsigned int _sceCd_scmdrdata[264] __attribute__((aligned(64))) = { 0 };
unsigned int _sceCd_scmdsdata[258] __attribute__((aligned(64))) = { 0 };
sceSifClientData _sceCd_cd_scmd = { 0 };


int sceCdDiskReady(int mode)
{
    int wait;
    int result;

    if (SCE_CD_debug > 0) {
        scePrintf("DiskReady 0\n");
    }
    cmd_sem_init();
    if (_sceCd_scmd_semid != PollSema(_sceCd_scmd_semid)) {
        return 6;
    }
    if (sceCdSyncS(1)) {
        SignalSema(_sceCd_scmd_semid);
        if (mode == 8) {
            return -1;
        }
        return 6;
    }
    sceSifInitRpc(0);
    if (_scmd_dr_bind < 0) {
        while (1) {
            if (sceSifBindRpc(&cddr, 0x8000059A, 0) < 0) {
                if (SCE_CD_debug > 0) {
                    scePrintf("Libcdvd bind err CdDiskReady\n");
                }
                wait = 0x100000;
                while (wait--) {
                }
                continue;
            }
            if (cddr.serve != 0) {
                _scmd_dr_bind = 0;
                break;
            }
            wait = 0x100000;
            while (wait--) {
            }
        }
    }
    _dr_mode[0] = mode;
    sceSifWriteBackDCache(_dr_mode, 4);
    if (sceSifCallRpc(&cddr, 0, 0, _dr_mode, 4, _sceCd_scmdrdata, 4, 0, 0) < 0) {
        SignalSema(_sceCd_scmd_semid);
        if (mode == 8) {
            return -1;
        }
        return 6;
    }
    if (SCE_CD_debug > 0) {
        scePrintf("DiskReady ended\n");
    }
    result = *(int *)UNCACHED(_sceCd_scmdrdata);
    SignalSema(_sceCd_scmd_semid);
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
        SignalSema(_sceCd_scmd_semid);
        return 0;
    }
    result = *(int *)UNCACHED(_sceCd_scmdrdata);
    SignalSema(_sceCd_scmd_semid);
    return result;
}
