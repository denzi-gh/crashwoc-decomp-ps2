/*
 * Unit: sdk/sce/libmc
 *
 * Functions:
 *   0x00266c68 sceMcInit
 *   0x00266e20 _lmcGetClientPtr
 *   0x00266e50 sceMcChangeThreadPriority
 *   0x00266f08 sceMcGetSlotMax
 *   0x00266fc8 sceMcOpen
 *   0x00267100 sceMcMkdir
 *   0x00267138 sceMcClose
 *   0x002671f0 sceMcSeek
 *   0x002672c8 mceIntrReadFixAlign
 *   0x00267358 sceMcRead
 *   0x00267478 sceMcWrite
 *   0x002675f8 mcHearAlarm
 *   0x00267620 mcDelayThread
 *   0x00267668 sceMcSync
 *   0x00267748 mceGetInfoApdx
 *   0x002677a0 sceMcGetInfo
 *   0x00267928 sceMcGetDir
 *   0x00267a80 mceStorePwd
 *   0x00267b08 sceMcChdir
 *   0x00267c58 sceMcFormat
 *   0x00267d20 sceMcDelete
 *   0x00267e48 sceMcFlush
 *   0x00267f00 sceMcSetFileInfo
 *   0x002680d8 sceMcRename
 *   0x00268240 sceMcUnformat
 *   0x00268308 sceMcGetEntSpace
 */

typedef struct sceSifClientData {
    unsigned char rpc_data[16];
    unsigned int command;
    void *buff;
    void *cbuff;
    void (*end_func)(void *);
    void *end_param;
    void *server;
} sceSifClientData;

typedef struct McDescParam {
    int fd;
    int port;
    int slot;
    int size;
    int offset;
    int origin;
    void *buffer;
    void *param;
    unsigned char data[16];
} McDescParam;

typedef struct McEndParam {
    int size1;
    int size2;
    unsigned char *dest1;
    unsigned char *dest2;
    unsigned char src1[64];
    unsigned char src2[64];
    int format;
    int reserved[11];
} McEndParam;

typedef struct McFileParam {
    int port;
    int slot;
    int flags;
    int maxent;
    void *info;
    char name[1024];
} McFileParam;

typedef struct sceMcStDateTime {
    unsigned char resv2;
    unsigned char sec;
    unsigned char min;
    unsigned char hour;
    unsigned char day;
    unsigned char month;
    unsigned short year;
} sceMcStDateTime;

typedef struct sceMcTblGetDir {
    sceMcStDateTime create;
    sceMcStDateTime modify;
    unsigned int fileSizeByte;
    unsigned short attrFile;
    unsigned short reserve1;
    unsigned int reserve2;
    unsigned int pdaAplNo;
    unsigned char entryName[32];
} sceMcTblGetDir;

typedef struct SemaParam {
    int currentCount;
    int maxCount;
    int initCount;
    int numWaitThreads;
    unsigned int attr;
    unsigned int option;
} SemaParam;

extern int CreateSema(SemaParam *param);
extern int WaitSema(int sema);
extern void FlushCache(int mode);
extern int printf(const char *fmt, ...);
extern char *strncpy(char *dest, const char *src, unsigned int n);
extern void sceSifInitRpc(unsigned int mode);
extern int sceSifBindRpc(sceSifClientData *client, unsigned int request,
                         unsigned int mode);
extern int sceSifCheckStatRpc(sceSifClientData *client);
extern void sceSifWriteBackDCache(void *ptr, int size);
extern int sceMcSync(int mode, int *cmd, int *result);
extern void mcDelayThread(unsigned short time);

static char __ps2_libinfo__[16] = "PsIIlibmc   2320";
static int mcRunCmdNo = 0;
static int semaidRegFunc = -1;

static sceSifClientData mcClientID;
static int *typeAddr;
static int *freeAddr;
static int *formAddr;
static sceMcTblGetDir buffFileInfo __attribute__((aligned(64)));
static McDescParam sifParamOrd;
static McFileParam sifParamFname;
static McEndParam sifParamNext __attribute__((aligned(64)));
static char currentDir[4096];
static int retval[16];

#define UNCACHED(p) ((unsigned int)(p) | 0x20000000)

extern int PollSema(int sema);
extern int SignalSema(int sema);
extern int GetThreadId(void);
extern int iWakeupThread(int tid);
extern int SetAlarm(unsigned short time, void (*handler)(int, unsigned short, void *), void *arg);
extern void SleepThread(void);
extern int sceSifCallRpc(sceSifClientData *client, unsigned int fno,
                         unsigned int mode, void *send, int ssize,
                         void *receive, int rsize, void (*end_func)(void *),
                         void *end_param);
extern unsigned int strlen(const char *s);
extern void *memcpy(void *dest, const void *src, unsigned int n);
extern int sceMcOpen(int port, int slot, const char *name, int mode);
extern void mcHearAlarm(int id, unsigned short time, void *thread);


int sceMcInit(void) {
    SemaParam sema;
    int ret;
    int i;

    if (semaidRegFunc < 0) {
        sema.initCount = 1;
        sema.maxCount = 1;
        sema.option = 0;
        semaidRegFunc = CreateSema(&sema);
    }
    sceMcSync(0, 0, 0);
    WaitSema(semaidRegFunc);
    sceSifInitRpc(0);
    while (1) {
        if (sceSifBindRpc(&mcClientID, 0x80000400, 0) < 0) {
            printf("bind error libmc \n");
            while (1) {
            }
        }
        if (mcClientID.server != 0) {
            break;
        }
        for (i = 0x100000; i != 0; i--) {
        }
    }
    ret = sceSifCallRpc(&mcClientID, 0xFE, 0, &sifParamOrd, 0x30, retval, 0xC, 0, 0);
    SignalSema(semaidRegFunc);
    if (ret < 0) {
        mcClientID.server = 0;
        return ret - 100;
    }
    if (retval[1] < 0x20A) {
        printf("libmc: too old release of mcserv.irx\n");
        mcClientID.server = 0;
        return -120;
    }
    if (retval[2] < 0x20E) {
        printf("libmc: too old release of mcman.irx\n");
        mcClientID.server = 0;
        return -121;
    }
    return retval[0];
}


int sceMcSync(int mode, int *cmd, int *result) {
    int ret;

    if (mcRunCmdNo == 0) {
        return -1;
    }
    ret = sceSifCheckStatRpc(&mcClientID);
    if (mode == 0 && ret != 0) {
        while (sceSifCheckStatRpc(&mcClientID) != 0) {
            mcDelayThread(60);
        }
        ret = 0;
    }
    ret = !ret;
    if (cmd != 0) {
        *cmd = mcRunCmdNo;
    }
    if (ret != 0) {
        mcRunCmdNo = 0;
        if (result != 0) {
            *result = retval[0];
        }
        SignalSema(semaidRegFunc);
    }
    return ret;
}


sceSifClientData *_lmcGetClientPtr(unsigned int *rdata, unsigned int *last_cmd) {
    *rdata = (unsigned int)retval;
    *last_cmd = (unsigned int)&mcRunCmdNo;
    retval[15] = semaidRegFunc;
    return &mcClientID;
}


int sceMcChangeThreadPriority(int level) {
    int ret;

    if (PollSema(semaidRegFunc) < 0) {
        return -200;
    }
    if (mcClientID.server == 0) {
        SignalSema(semaidRegFunc);
        return -100;
    }
    sifParamOrd.origin = level;
    ret = sceSifCallRpc(&mcClientID, 0x14, 1, &sifParamOrd, 0x30, retval, 4, 0, 0);
    if (ret == 0) {
        mcRunCmdNo = 0x14;
    } else {
        SignalSema(semaidRegFunc);
    }
    return ret;
}


int sceMcGetSlotMax(int port) {
    int ret;

    if (PollSema(semaidRegFunc) < 0) {
        return -200;
    }
    if (mcClientID.server == 0) {
        SignalSema(semaidRegFunc);
        return -100;
    }
    sifParamOrd.port = port;
    ret = sceSifCallRpc(&mcClientID, 0x15, 0, &sifParamOrd, 0x30, retval, 4, 0, 0);
    if (ret != 0) {
        SignalSema(semaidRegFunc);
        return ret;
    }
    SignalSema(semaidRegFunc);
    return retval[0];
}


int sceMcOpen(int port, int slot, const char *name, int mode) {
    int ret;

    if (PollSema(semaidRegFunc) < 0) {
        return -200;
    }
    if (mcClientID.server == 0) {
        SignalSema(semaidRegFunc);
        return -100;
    }
    if (name == 0 || *name == 0) {
        SignalSema(semaidRegFunc);
        return -210;
    }
    strncpy(sifParamFname.name, name, 0x3FF);
    sifParamFname.port = port;
    sifParamFname.flags = mode;
    sifParamFname.slot = slot;
    sifParamFname.name[0x3FF] = 0;
    ret = sceSifCallRpc(&mcClientID, 2, 1, &sifParamFname, 0x414, retval, 4, 0, 0);
    if (ret == 0) {
        mcRunCmdNo = 2;
    } else {
        SignalSema(semaidRegFunc);
    }
    return ret;
}


int sceMcMkdir(int port, int slot, const char *name) {
    int ret;

    ret = sceMcOpen(port, slot, name, 0x40);
    if (ret == 0) {
        mcRunCmdNo = 0xB;
    }
    return ret;
}


int sceMcClose(int fd) {
    int ret;

    if (PollSema(semaidRegFunc) < 0) {
        return -200;
    }
    if (mcClientID.server == 0) {
        SignalSema(semaidRegFunc);
        return -100;
    }
    sifParamOrd.fd = fd;
    ret = sceSifCallRpc(&mcClientID, 3, 1, &sifParamOrd, 0x30, retval, 4, 0, 0);
    if (ret == 0) {
        mcRunCmdNo = 3;
    } else {
        SignalSema(semaidRegFunc);
    }
    return ret;
}


int sceMcSeek(int fd, int offset, int origin) {
    int ret;

    if (PollSema(semaidRegFunc) < 0) {
        return -200;
    }
    if (mcClientID.server == 0) {
        SignalSema(semaidRegFunc);
        return -100;
    }
    sifParamOrd.fd = fd;
    sifParamOrd.offset = offset;
    sifParamOrd.origin = origin;
    ret = sceSifCallRpc(&mcClientID, 4, 1, &sifParamOrd, 0x30, retval, 4, 0, 0);
    if (ret == 0) {
        mcRunCmdNo = 4;
    } else {
        SignalSema(semaidRegFunc);
    }
    return ret;
}


void mceIntrReadFixAlign(void *data) {
    McEndParam *param;
    unsigned char *dest;
    int i;

    param = (McEndParam *)UNCACHED(data);
    if (param->size1 != 0) {
        dest = param->dest1;
        for (i = 0; i < param->size1; i++) {
            *dest++ = param->src1[i];
        }
    }
    if (param->size2 != 0) {
        dest = param->dest2;
        for (i = 0; i < param->size2; i++) {
            *dest++ = param->src2[i];
        }
    }
}


int sceMcRead(int fd, void *buff, int size) {
    int ret;

    if (PollSema(semaidRegFunc) < 0) {
        return -200;
    }
    if (mcClientID.server == 0) {
        SignalSema(semaidRegFunc);
        return -100;
    }
    sifParamOrd.fd = fd;
    sifParamOrd.buffer = buff;
    sifParamOrd.size = size;
    sifParamOrd.param = &sifParamNext;
    sceSifWriteBackDCache(buff, size);
    sceSifWriteBackDCache(&sifParamNext, 0xC0);
    ret = sceSifCallRpc(&mcClientID, 5, 1, &sifParamOrd, 0x30, retval, 4,
                        mceIntrReadFixAlign, &sifParamNext);
    if (ret == 0) {
        mcRunCmdNo = 5;
    } else {
        SignalSema(semaidRegFunc);
    }
    return ret;
}


int sceMcWrite(int fd, const void *buff, int size) {
    int ret;
    int head;
    unsigned int i;

    if (PollSema(semaidRegFunc) < 0) {
        return -200;
    }
    if (mcClientID.server == 0) {
        SignalSema(semaidRegFunc);
        return -100;
    }
    sifParamOrd.fd = fd;
    if (size < 17) {
        sifParamOrd.origin = size;
        sifParamOrd.size = 0;
        sifParamOrd.buffer = 0;
    } else {
        head = ((((unsigned int)buff - 1) & 0xFFFFFFF0) + 0x10) - (unsigned int)buff;
        sifParamOrd.origin = head;
        sifParamOrd.size = size - head;
        sifParamOrd.buffer = (char *)buff + head;
    }
    for (i = 0; i < sifParamOrd.origin; i++) {
        sifParamOrd.data[i] = ((const unsigned char *)buff)[i];
    }
    FlushCache(0);
    ret = sceSifCallRpc(&mcClientID, 6, 1, &sifParamOrd, 0x30, retval, 4, 0, 0);
    if (ret == 0) {
        mcRunCmdNo = 6;
    } else {
        SignalSema(semaidRegFunc);
    }
    return ret;
}


void mcHearAlarm(int id, unsigned short time, void *thread) {
    iWakeupThread((int)thread);
    __asm__ volatile ("sync\n\tei");
}


void mcDelayThread(unsigned short time) {
    SetAlarm(time, mcHearAlarm, (void *)GetThreadId());
    SleepThread();
}


void mceGetInfoApdx(void *data) {
    McEndParam *param;

    param = (McEndParam *)UNCACHED(data);
    if (typeAddr != 0) {
        *typeAddr = param->size1;
    }
    if (freeAddr != 0) {
        *freeAddr = param->size2;
    }
    if (formAddr != 0) {
        *formAddr = param->format;
    }
}


int sceMcGetInfo(int port, int slot, int *type, int *free, int *format) {
    int ret;

    if (PollSema(semaidRegFunc) < 0) {
        return -200;
    }
    if (mcClientID.server == 0) {
        SignalSema(semaidRegFunc);
        return -100;
    }
    sifParamOrd.port = port;
    sifParamOrd.slot = slot;
    sifParamOrd.param = &sifParamNext;
    if (type != 0) {
        sifParamOrd.origin = 1;
    } else {
        sifParamOrd.origin = 0;
    }
    if (free != 0) {
        sifParamOrd.offset = 1;
    } else {
        sifParamOrd.offset = 0;
    }
    if (format != 0) {
        sifParamOrd.size = 1;
    } else {
        sifParamOrd.size = 0;
    }
    typeAddr = type;
    freeAddr = free;
    formAddr = format;
    sceSifWriteBackDCache(&sifParamNext, 0xC0);
    ret = sceSifCallRpc(&mcClientID, 1, 1, &sifParamOrd, 0x30, retval, 4,
                        mceGetInfoApdx, &sifParamNext);
    if (ret == 0) {
        mcRunCmdNo = 1;
    } else {
        SignalSema(semaidRegFunc);
    }
    return ret;
}


int sceMcGetDir(int port, int slot, const char *name, unsigned int mode,
                int maxent, sceMcTblGetDir *table) {
    int ret;

    if (PollSema(semaidRegFunc) < 0) {
        return -200;
    }
    if (mcClientID.server == 0) {
        SignalSema(semaidRegFunc);
        return -100;
    }
    if (name == 0 || *name == 0) {
        SignalSema(semaidRegFunc);
        return -210;
    }
    sifParamFname.port = port;
    sifParamFname.slot = slot;
    sifParamFname.flags = mode;
    sifParamFname.maxent = maxent;
    sifParamFname.info = table;
    strncpy(sifParamFname.name, name, 0x3FF);
    sifParamFname.name[0x3FF] = 0;
    if (maxent >= 0) {
        sceSifWriteBackDCache(table, maxent * sizeof(sceMcTblGetDir));
    }
    ret = sceSifCallRpc(&mcClientID, 0xD, 1, &sifParamFname, 0x414, retval, 4, 0, 0);
    if (ret == 0) {
        mcRunCmdNo = 0xD;
    } else {
        SignalSema(semaidRegFunc);
    }
    return ret;
}


void mceStorePwd(char *pwd) {
    int len;

    if (pwd != 0) {
        if (strlen((char *)UNCACHED(currentDir)) < 0x400) {
            len = strlen((char *)UNCACHED(currentDir));
        } else {
            len = 0x3FF;
        }
        memcpy(pwd, (char *)UNCACHED(currentDir), len);
        pwd[len] = 0;
    }
}


int sceMcChdir(int port, int slot, const char *newDir, char *curDir) {
    int ret;

    if (PollSema(semaidRegFunc) < 0) {
        return -200;
    }
    if (mcClientID.server == 0) {
        SignalSema(semaidRegFunc);
        return -100;
    }
    if (newDir == 0 || *newDir == 0) {
        SignalSema(semaidRegFunc);
        return -210;
    }
    sifParamFname.port = port;
    sifParamFname.slot = slot;
    sifParamFname.info = currentDir;
    strncpy(sifParamFname.name, newDir, 0x3FF);
    sifParamFname.name[0x3FF] = 0;
    sceSifWriteBackDCache(currentDir, 0x400);
    ret = sceSifCallRpc(&mcClientID, 0xC, 1, &sifParamFname, 0x414, retval, 4,
                        mceStorePwd, curDir);
    if (ret == 0) {
        mcRunCmdNo = 0xC;
    } else {
        SignalSema(semaidRegFunc);
    }
    return ret;
}


int sceMcFormat(int port, int slot) {
    int ret;

    if (PollSema(semaidRegFunc) < 0) {
        return -200;
    }
    if (mcClientID.server == 0) {
        SignalSema(semaidRegFunc);
        return -100;
    }
    sifParamOrd.port = port;
    sifParamOrd.slot = slot;
    ret = sceSifCallRpc(&mcClientID, 0x10, 1, &sifParamOrd, 0x30, retval, 4, 0, 0);
    if (ret == 0) {
        mcRunCmdNo = 0x10;
    } else {
        SignalSema(semaidRegFunc);
    }
    return ret;
}


int sceMcDelete(int port, int slot, const char *name) {
    int ret;

    if (PollSema(semaidRegFunc) < 0) {
        return -200;
    }
    if (mcClientID.server == 0) {
        SignalSema(semaidRegFunc);
        return -100;
    }
    if (name == 0 || *name == 0) {
        SignalSema(semaidRegFunc);
        return -210;
    }
    strncpy(sifParamFname.name, name, 0x3FF);
    sifParamFname.port = port;
    sifParamFname.slot = slot;
    sifParamFname.name[0x3FF] = 0;
    sifParamFname.flags = 0;
    ret = sceSifCallRpc(&mcClientID, 0xF, 1, &sifParamFname, 0x414, retval, 4, 0, 0);
    if (ret == 0) {
        mcRunCmdNo = 0xF;
    } else {
        SignalSema(semaidRegFunc);
    }
    return ret;
}


int sceMcFlush(int fd) {
    int ret;

    if (PollSema(semaidRegFunc) < 0) {
        return -200;
    }
    if (mcClientID.server == 0) {
        SignalSema(semaidRegFunc);
        return -100;
    }
    sifParamOrd.fd = fd;
    ret = sceSifCallRpc(&mcClientID, 0xA, 1, &sifParamOrd, 0x30, retval, 4, 0, 0);
    if (ret == 0) {
        mcRunCmdNo = 0xA;
    } else {
        SignalSema(semaidRegFunc);
    }
    return ret;
}


int sceMcSetFileInfo(int port, int slot, const char *name, const char *info,
                     unsigned int valid) {
    int ret;

    if (PollSema(semaidRegFunc) < 0) {
        return -200;
    }
    if (mcClientID.server == 0) {
        SignalSema(semaidRegFunc);
        return -100;
    }
    if (name == 0 || *name == 0) {
        SignalSema(semaidRegFunc);
        return -210;
    }
    sifParamFname.port = port;
    sifParamFname.slot = slot;
    valid &= 7;
    sifParamFname.flags = valid;
    buffFileInfo = *(const sceMcTblGetDir *)info;
    sifParamFname.info = &buffFileInfo;
    strncpy(sifParamFname.name, name, 0x3FF);
    sifParamFname.name[0x3FF] = 0;
    FlushCache(0);
    ret = sceSifCallRpc(&mcClientID, 0xE, 1, &sifParamFname, 0x414, retval, 4, 0, 0);
    if (ret == 0) {
        mcRunCmdNo = 0xE;
    } else {
        SignalSema(semaidRegFunc);
    }
    return ret;
}


int sceMcRename(int port, int slot, const char *oldName, const char *newName) {
    int ret;

    if (PollSema(semaidRegFunc) < 0) {
        return -200;
    }
    if (mcClientID.server == 0) {
        SignalSema(semaidRegFunc);
        return -100;
    }
    if (oldName == 0 || *oldName == 0 || newName == 0) {
        SignalSema(semaidRegFunc);
        return -210;
    }
    sifParamFname.port = port;
    sifParamFname.slot = slot;
    sifParamFname.flags = 0x10;
    strncpy(sifParamFname.name, oldName, 0x3FF);
    sifParamFname.name[0x3FF] = 0;
    strncpy((char *)buffFileInfo.entryName, newName, 0x20);
    buffFileInfo.entryName[0x1F] = 0;
    sifParamFname.info = &buffFileInfo;
    FlushCache(0);
    ret = sceSifCallRpc(&mcClientID, 0xE, 1, &sifParamFname, 0x414, retval, 4, 0, 0);
    if (ret == 0) {
        mcRunCmdNo = 0x13;
    } else {
        SignalSema(semaidRegFunc);
    }
    return ret;
}


int sceMcUnformat(int port, int slot) {
    int ret;

    if (PollSema(semaidRegFunc) < 0) {
        return -200;
    }
    if (mcClientID.server == 0) {
        SignalSema(semaidRegFunc);
        return -100;
    }
    sifParamOrd.port = port;
    sifParamOrd.slot = slot;
    ret = sceSifCallRpc(&mcClientID, 0x11, 1, &sifParamOrd, 0x30, retval, 4, 0, 0);
    if (ret == 0) {
        mcRunCmdNo = 0x11;
    } else {
        SignalSema(semaidRegFunc);
    }
    return ret;
}


int sceMcGetEntSpace(int port, int slot, const char *path) {
    int ret;

    if (PollSema(semaidRegFunc) < 0) {
        return -200;
    }
    if (mcClientID.server == 0) {
        SignalSema(semaidRegFunc);
        return -100;
    }
    if (path == 0 || *path == 0) {
        SignalSema(semaidRegFunc);
        return -210;
    }
    sifParamFname.port = port;
    sifParamFname.slot = slot;
    strncpy(sifParamFname.name, path, 0x3FF);
    sifParamFname.name[0x3FF] = 0;
    ret = sceSifCallRpc(&mcClientID, 0x12, 1, &sifParamFname, 0x414, retval, 4, 0, 0);
    if (ret == 0) {
        mcRunCmdNo = 0x12;
    } else {
        SignalSema(semaidRegFunc);
    }
    return ret;
}
