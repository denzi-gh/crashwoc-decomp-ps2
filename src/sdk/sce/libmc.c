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
