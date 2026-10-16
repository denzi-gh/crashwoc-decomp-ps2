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

extern sceMcTblGetDir D_00700180;
extern McFileParam D_007001F0;
extern McEndParam D_00700640;

#define mc_file_info D_00700180
#define mc_file      D_007001F0
#define mc_end_param D_00700640

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

extern int D_00612400[];
extern int D_00612404[];
extern sceSifClientData D_00700140;
extern int *D_00700168[];
extern int *D_0070016C[];
extern int *D_00700170[];
extern McDescParam D_007001C0;
extern char D_00700700[];
extern int D_00701700[];

#define mc_last_cmd    (D_00612400[0])
#define mc_sema        (D_00612404[0])
#define mc_client      D_00700140
#define mc_info_type   (D_00700168[0])
#define mc_info_free   (D_0070016C[0])
#define mc_info_format (D_00700170[0])
#define mc_desc        D_007001C0
#define mc_pwd         D_00700700
#define mc_rdata       D_00701700

#define UNCACHED(p) ((unsigned int)(p) | 0x20000000)

extern int PollSema(int sema);
extern int SignalSema(int sema);
extern int GetThreadId(void);
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

    if (mc_sema < 0) {
        sema.initCount = 1;
        sema.maxCount = 1;
        sema.option = 0;
        mc_sema = CreateSema(&sema);
    }
    sceMcSync(0, 0, 0);
    WaitSema(mc_sema);
    sceSifInitRpc(0);
    while (1) {
        if (sceSifBindRpc(&mc_client, 0x80000400, 0) < 0) {
            printf("bind error libmc \n");
            while (1) {
            }
        }
        if (mc_client.server != 0) {
            break;
        }
        for (i = 0x100000; i != 0; i--) {
        }
    }
    ret = sceSifCallRpc(&mc_client, 0xFE, 0, &mc_desc, 0x30, mc_rdata, 0xC, 0, 0);
    SignalSema(mc_sema);
    if (ret < 0) {
        mc_client.server = 0;
        return ret - 100;
    }
    if (mc_rdata[1] < 0x20A) {
        printf("libmc: too old release of mcserv.irx\n");
        mc_client.server = 0;
        return -120;
    }
    if (mc_rdata[2] < 0x20E) {
        printf("libmc: too old release of mcman.irx\n");
        mc_client.server = 0;
        return -121;
    }
    return mc_rdata[0];
}


int sceMcSync(int mode, int *cmd, int *result) {
    int ret;

    if (mc_last_cmd == 0) {
        return -1;
    }
    ret = sceSifCheckStatRpc(&mc_client);
    if (mode == 0 && ret != 0) {
        while (sceSifCheckStatRpc(&mc_client) != 0) {
            mcDelayThread(60);
        }
        ret = 0;
    }
    ret = !ret;
    if (cmd != 0) {
        *cmd = mc_last_cmd;
    }
    if (ret != 0) {
        mc_last_cmd = 0;
        if (result != 0) {
            *result = mc_rdata[0];
        }
        SignalSema(mc_sema);
    }
    return ret;
}


sceSifClientData *_lmcGetClientPtr(unsigned int *rdata, unsigned int *last_cmd) {
    *rdata = (unsigned int)mc_rdata;
    *last_cmd = (unsigned int)&mc_last_cmd;
    mc_rdata[15] = mc_sema;
    return &mc_client;
}


int sceMcChangeThreadPriority(int level) {
    int ret;

    if (PollSema(mc_sema) < 0) {
        return -200;
    }
    if (mc_client.server == 0) {
        SignalSema(mc_sema);
        return -100;
    }
    mc_desc.origin = level;
    ret = sceSifCallRpc(&mc_client, 0x14, 1, &mc_desc, 0x30, mc_rdata, 4, 0, 0);
    if (ret == 0) {
        mc_last_cmd = 0x14;
    } else {
        SignalSema(mc_sema);
    }
    return ret;
}


int sceMcGetSlotMax(int port) {
    int ret;

    if (PollSema(mc_sema) < 0) {
        return -200;
    }
    if (mc_client.server == 0) {
        SignalSema(mc_sema);
        return -100;
    }
    mc_desc.port = port;
    ret = sceSifCallRpc(&mc_client, 0x15, 0, &mc_desc, 0x30, mc_rdata, 4, 0, 0);
    if (ret != 0) {
        SignalSema(mc_sema);
        return ret;
    }
    SignalSema(mc_sema);
    return mc_rdata[0];
}


int sceMcOpen(int port, int slot, const char *name, int mode) {
    int ret;

    if (PollSema(mc_sema) < 0) {
        return -200;
    }
    if (mc_client.server == 0) {
        SignalSema(mc_sema);
        return -100;
    }
    if (name == 0 || *name == 0) {
        SignalSema(mc_sema);
        return -210;
    }
    strncpy(mc_file.name, name, 0x3FF);
    mc_file.port = port;
    mc_file.flags = mode;
    mc_file.slot = slot;
    mc_file.name[0x3FF] = 0;
    ret = sceSifCallRpc(&mc_client, 2, 1, &mc_file, 0x414, mc_rdata, 4, 0, 0);
    if (ret == 0) {
        mc_last_cmd = 2;
    } else {
        SignalSema(mc_sema);
    }
    return ret;
}


int sceMcMkdir(int port, int slot, const char *name) {
    int ret;

    ret = sceMcOpen(port, slot, name, 0x40);
    if (ret == 0) {
        mc_last_cmd = 0xB;
    }
    return ret;
}


int sceMcClose(int fd) {
    int ret;

    if (PollSema(mc_sema) < 0) {
        return -200;
    }
    if (mc_client.server == 0) {
        SignalSema(mc_sema);
        return -100;
    }
    mc_desc.fd = fd;
    ret = sceSifCallRpc(&mc_client, 3, 1, &mc_desc, 0x30, mc_rdata, 4, 0, 0);
    if (ret == 0) {
        mc_last_cmd = 3;
    } else {
        SignalSema(mc_sema);
    }
    return ret;
}


int sceMcSeek(int fd, int offset, int origin) {
    int ret;

    if (PollSema(mc_sema) < 0) {
        return -200;
    }
    if (mc_client.server == 0) {
        SignalSema(mc_sema);
        return -100;
    }
    mc_desc.fd = fd;
    mc_desc.offset = offset;
    mc_desc.origin = origin;
    ret = sceSifCallRpc(&mc_client, 4, 1, &mc_desc, 0x30, mc_rdata, 4, 0, 0);
    if (ret == 0) {
        mc_last_cmd = 4;
    } else {
        SignalSema(mc_sema);
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

    if (PollSema(mc_sema) < 0) {
        return -200;
    }
    if (mc_client.server == 0) {
        SignalSema(mc_sema);
        return -100;
    }
    mc_desc.fd = fd;
    mc_desc.buffer = buff;
    mc_desc.size = size;
    mc_desc.param = &mc_end_param;
    sceSifWriteBackDCache(buff, size);
    sceSifWriteBackDCache(&mc_end_param, 0xC0);
    ret = sceSifCallRpc(&mc_client, 5, 1, &mc_desc, 0x30, mc_rdata, 4,
                        mceIntrReadFixAlign, &mc_end_param);
    if (ret == 0) {
        mc_last_cmd = 5;
    } else {
        SignalSema(mc_sema);
    }
    return ret;
}


int sceMcWrite(int fd, const void *buff, int size) {
    int ret;
    int head;
    unsigned int i;

    if (PollSema(mc_sema) < 0) {
        return -200;
    }
    if (mc_client.server == 0) {
        SignalSema(mc_sema);
        return -100;
    }
    mc_desc.fd = fd;
    if (size < 17) {
        mc_desc.origin = size;
        mc_desc.size = 0;
        mc_desc.buffer = 0;
    } else {
        head = ((((unsigned int)buff - 1) & 0xFFFFFFF0) + 0x10) - (unsigned int)buff;
        mc_desc.origin = head;
        mc_desc.size = size - head;
        mc_desc.buffer = (char *)buff + head;
    }
    for (i = 0; i < mc_desc.origin; i++) {
        mc_desc.data[i] = ((const unsigned char *)buff)[i];
    }
    FlushCache(0);
    ret = sceSifCallRpc(&mc_client, 6, 1, &mc_desc, 0x30, mc_rdata, 4, 0, 0);
    if (ret == 0) {
        mc_last_cmd = 6;
    } else {
        SignalSema(mc_sema);
    }
    return ret;
}


void mcDelayThread(unsigned short time) {
    SetAlarm(time, mcHearAlarm, (void *)GetThreadId());
    SleepThread();
}


void mceGetInfoApdx(void *data) {
    McEndParam *param;

    param = (McEndParam *)UNCACHED(data);
    if (mc_info_type != 0) {
        *mc_info_type = param->size1;
    }
    if (mc_info_free != 0) {
        *mc_info_free = param->size2;
    }
    if (mc_info_format != 0) {
        *mc_info_format = param->format;
    }
}


int sceMcGetInfo(int port, int slot, int *type, int *free, int *format) {
    int ret;

    if (PollSema(mc_sema) < 0) {
        return -200;
    }
    if (mc_client.server == 0) {
        SignalSema(mc_sema);
        return -100;
    }
    mc_desc.port = port;
    mc_desc.slot = slot;
    mc_desc.param = &mc_end_param;
    if (type != 0) {
        mc_desc.origin = 1;
    } else {
        mc_desc.origin = 0;
    }
    if (free != 0) {
        mc_desc.offset = 1;
    } else {
        mc_desc.offset = 0;
    }
    if (format != 0) {
        mc_desc.size = 1;
    } else {
        mc_desc.size = 0;
    }
    mc_info_type = type;
    mc_info_free = free;
    mc_info_format = format;
    sceSifWriteBackDCache(&mc_end_param, 0xC0);
    ret = sceSifCallRpc(&mc_client, 1, 1, &mc_desc, 0x30, mc_rdata, 4,
                        mceGetInfoApdx, &mc_end_param);
    if (ret == 0) {
        mc_last_cmd = 1;
    } else {
        SignalSema(mc_sema);
    }
    return ret;
}


int sceMcGetDir(int port, int slot, const char *name, unsigned int mode,
                int maxent, sceMcTblGetDir *table) {
    int ret;

    if (PollSema(mc_sema) < 0) {
        return -200;
    }
    if (mc_client.server == 0) {
        SignalSema(mc_sema);
        return -100;
    }
    if (name == 0 || *name == 0) {
        SignalSema(mc_sema);
        return -210;
    }
    mc_file.port = port;
    mc_file.slot = slot;
    mc_file.flags = mode;
    mc_file.maxent = maxent;
    mc_file.info = table;
    strncpy(mc_file.name, name, 0x3FF);
    mc_file.name[0x3FF] = 0;
    if (maxent >= 0) {
        sceSifWriteBackDCache(table, maxent * sizeof(sceMcTblGetDir));
    }
    ret = sceSifCallRpc(&mc_client, 0xD, 1, &mc_file, 0x414, mc_rdata, 4, 0, 0);
    if (ret == 0) {
        mc_last_cmd = 0xD;
    } else {
        SignalSema(mc_sema);
    }
    return ret;
}


void mceStorePwd(char *pwd) {
    int len;

    if (pwd != 0) {
        if (strlen((char *)UNCACHED(mc_pwd)) < 0x400) {
            len = strlen((char *)UNCACHED(mc_pwd));
        } else {
            len = 0x3FF;
        }
        memcpy(pwd, (char *)UNCACHED(mc_pwd), len);
        pwd[len] = 0;
    }
}


int sceMcChdir(int port, int slot, const char *newDir, char *curDir) {
    int ret;

    if (PollSema(mc_sema) < 0) {
        return -200;
    }
    if (mc_client.server == 0) {
        SignalSema(mc_sema);
        return -100;
    }
    if (newDir == 0 || *newDir == 0) {
        SignalSema(mc_sema);
        return -210;
    }
    mc_file.port = port;
    mc_file.slot = slot;
    mc_file.info = mc_pwd;
    strncpy(mc_file.name, newDir, 0x3FF);
    mc_file.name[0x3FF] = 0;
    sceSifWriteBackDCache(mc_pwd, 0x400);
    ret = sceSifCallRpc(&mc_client, 0xC, 1, &mc_file, 0x414, mc_rdata, 4,
                        mceStorePwd, curDir);
    if (ret == 0) {
        mc_last_cmd = 0xC;
    } else {
        SignalSema(mc_sema);
    }
    return ret;
}


int sceMcFormat(int port, int slot) {
    int ret;

    if (PollSema(mc_sema) < 0) {
        return -200;
    }
    if (mc_client.server == 0) {
        SignalSema(mc_sema);
        return -100;
    }
    mc_desc.port = port;
    mc_desc.slot = slot;
    ret = sceSifCallRpc(&mc_client, 0x10, 1, &mc_desc, 0x30, mc_rdata, 4, 0, 0);
    if (ret == 0) {
        mc_last_cmd = 0x10;
    } else {
        SignalSema(mc_sema);
    }
    return ret;
}


int sceMcDelete(int port, int slot, const char *name) {
    int ret;

    if (PollSema(mc_sema) < 0) {
        return -200;
    }
    if (mc_client.server == 0) {
        SignalSema(mc_sema);
        return -100;
    }
    if (name == 0 || *name == 0) {
        SignalSema(mc_sema);
        return -210;
    }
    strncpy(mc_file.name, name, 0x3FF);
    mc_file.port = port;
    mc_file.slot = slot;
    mc_file.name[0x3FF] = 0;
    mc_file.flags = 0;
    ret = sceSifCallRpc(&mc_client, 0xF, 1, &mc_file, 0x414, mc_rdata, 4, 0, 0);
    if (ret == 0) {
        mc_last_cmd = 0xF;
    } else {
        SignalSema(mc_sema);
    }
    return ret;
}


int sceMcFlush(int fd) {
    int ret;

    if (PollSema(mc_sema) < 0) {
        return -200;
    }
    if (mc_client.server == 0) {
        SignalSema(mc_sema);
        return -100;
    }
    mc_desc.fd = fd;
    ret = sceSifCallRpc(&mc_client, 0xA, 1, &mc_desc, 0x30, mc_rdata, 4, 0, 0);
    if (ret == 0) {
        mc_last_cmd = 0xA;
    } else {
        SignalSema(mc_sema);
    }
    return ret;
}


int sceMcSetFileInfo(int port, int slot, const char *name, const char *info,
                     unsigned int valid) {
    int ret;

    if (PollSema(mc_sema) < 0) {
        return -200;
    }
    if (mc_client.server == 0) {
        SignalSema(mc_sema);
        return -100;
    }
    if (name == 0 || *name == 0) {
        SignalSema(mc_sema);
        return -210;
    }
    mc_file.port = port;
    mc_file.slot = slot;
    valid &= 7;
    mc_file.flags = valid;
    mc_file_info = *(const sceMcTblGetDir *)info;
    mc_file.info = &mc_file_info;
    strncpy(mc_file.name, name, 0x3FF);
    mc_file.name[0x3FF] = 0;
    FlushCache(0);
    ret = sceSifCallRpc(&mc_client, 0xE, 1, &mc_file, 0x414, mc_rdata, 4, 0, 0);
    if (ret == 0) {
        mc_last_cmd = 0xE;
    } else {
        SignalSema(mc_sema);
    }
    return ret;
}


int sceMcRename(int port, int slot, const char *oldName, const char *newName) {
    int ret;

    if (PollSema(mc_sema) < 0) {
        return -200;
    }
    if (mc_client.server == 0) {
        SignalSema(mc_sema);
        return -100;
    }
    if (oldName == 0 || *oldName == 0 || newName == 0) {
        SignalSema(mc_sema);
        return -210;
    }
    mc_file.port = port;
    mc_file.slot = slot;
    mc_file.flags = 0x10;
    strncpy(mc_file.name, oldName, 0x3FF);
    mc_file.name[0x3FF] = 0;
    strncpy((char *)mc_file_info.entryName, newName, 0x20);
    mc_file_info.entryName[0x1F] = 0;
    mc_file.info = &mc_file_info;
    FlushCache(0);
    ret = sceSifCallRpc(&mc_client, 0xE, 1, &mc_file, 0x414, mc_rdata, 4, 0, 0);
    if (ret == 0) {
        mc_last_cmd = 0x13;
    } else {
        SignalSema(mc_sema);
    }
    return ret;
}


int sceMcUnformat(int port, int slot) {
    int ret;

    if (PollSema(mc_sema) < 0) {
        return -200;
    }
    if (mc_client.server == 0) {
        SignalSema(mc_sema);
        return -100;
    }
    mc_desc.port = port;
    mc_desc.slot = slot;
    ret = sceSifCallRpc(&mc_client, 0x11, 1, &mc_desc, 0x30, mc_rdata, 4, 0, 0);
    if (ret == 0) {
        mc_last_cmd = 0x11;
    } else {
        SignalSema(mc_sema);
    }
    return ret;
}


int sceMcGetEntSpace(int port, int slot, const char *path) {
    int ret;

    if (PollSema(mc_sema) < 0) {
        return -200;
    }
    if (mc_client.server == 0) {
        SignalSema(mc_sema);
        return -100;
    }
    if (path == 0 || *path == 0) {
        SignalSema(mc_sema);
        return -210;
    }
    mc_file.port = port;
    mc_file.slot = slot;
    strncpy(mc_file.name, path, 0x3FF);
    mc_file.name[0x3FF] = 0;
    ret = sceSifCallRpc(&mc_client, 0x12, 1, &mc_file, 0x414, mc_rdata, 4, 0, 0);
    if (ret == 0) {
        mc_last_cmd = 0x12;
    } else {
        SignalSema(mc_sema);
    }
    return ret;
}
