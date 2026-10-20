/*
 * Unit: sdk/sce/eeloadfile
 *
 * Functions:
 *   0x0027fb40 _lf_bind
 *   0x0027fc40 _lf_version
 *   0x0027fcd0 sceSifLoadFileReset
 *   0x0027fd08 _sceSifLoadModuleBuffer
 *   0x0027ff10 sceSifStopModule
 *   0x00280118 sceSifUnloadModule
 *   0x002801a8 sceSifSearchModuleByName
 *   0x00280248 sceSifSearchModuleByAddress
 *   0x002802d8 sceSifLoadModuleBuffer
 *   0x002802f8 sceSifLoadStartModuleBuffer
 *   0x00280318 _sceSifLoadModule
 *   0x00280540 sceSifLoadModule
 *   0x00280560 sceSifLoadStartModule
 *   0x00280580 _sceSifLoadElfPart
 *   0x00280688 sceSifLoadElfPart
 *   0x002806a8 sceSifLoadElf
 *   0x002806d0 sceSifGetIopAddr
 *   0x002807c0 sceSifSetIopAddr
 */

#define LF_PATH_MAX 252
#define LF_ARG_MAX  252

#define LF_RPC_SERVER   0x80000006
#define LF_SET_ADDR     2
#define LF_GET_ADDR     3
#define LF_MODULE_BUFFER_LOAD 6
#define LF_MODULE_STOP  7
#define LF_UNLOAD       8
#define LF_SEARCH_NAME  9
#define LF_SEARCH_ADDR  10
#define LF_GET_VERSION  0xFF

#define SCE_EBINDMISS    0x10000
#define SCE_ECALLMISS    0x10001
#define SCE_ETYPEMISS    0x10002
#define SCE_ELOADMISS    0x10003
#define SCE_EVERSIONMISS 0x10004

typedef struct sceSifClientData {
    unsigned char rpc_data[16];
    unsigned int command;
    void *buff;
    void *cbuff;
    void (*end_func)(void *);
    void *end_param;
    void *server;
} sceSifClientData;

typedef struct sceExecData {
    int entry_point;
    int global_pointer;
    int stack_pointer;
    int reserved;
} sceExecData;

typedef struct LoadFileArg {
    union {
        const void *addr;
        int id;
        int arg_len;
        int entry_point;
        int result;
    } p;
    union {
        int arg_len;
        int start_result;
        int global_pointer;
    } q;
    char path[LF_PATH_MAX];
    char args[LF_ARG_MAX];
} LoadFileArg;

typedef struct LoadFileIopValArg {
    union {
        unsigned int iop_addr;
        int result;
    } p;
    int type;
    union {
        unsigned char b;
        unsigned short s;
        unsigned int l;
    } val;
} LoadFileIopValArg;

typedef union LoadFileSendData {
    LoadFileArg file;
    LoadFileIopValArg iopval;
} LoadFileSendData;

extern LoadFileSendData D_007066C0;
extern sceSifClientData D_007068C0;
extern char D_007068E8[];
extern char D_00612BB4[];
extern char *D_0061341C[];
extern int D_00613418[];
extern char D_0062C7E8[];

#define lf_arg          D_007066C0.file
#define lf_iopval       D_007066C0.iopval
#define lf_client       D_007068C0
#define lf_iop_version  D_007068E8
#define lf_ee_version   D_00612BB4
#define lf_dummy_version D_0061341C
#define lf_init         D_00613418
#define lf_all_sections D_0062C7E8

extern int sceSifBindRpc(sceSifClientData *client, unsigned int request,
                         unsigned int mode);
extern int sceSifCallRpc(sceSifClientData *client, unsigned int fno,
                         unsigned int mode, void *send, int ssize,
                         void *receive, int rsize, void (*end_func)(void *),
                         void *end_param);
extern int memcmp(const void *a, const void *b, unsigned int n);
extern void *memcpy(void *dest, const void *src, unsigned int n);
extern void *memset(void *dest, int c, unsigned int n);
extern char *strncpy(char *dest, const char *src, unsigned int n);
extern int _lf_bind(void);


int _lf_bind(void) {
    int wait;

    if (lf_init[0] < 0) {
        while (1) {
            if (sceSifBindRpc(&lf_client, LF_RPC_SERVER, 0) < 0) {
                return -1;
            }
            if (lf_client.server != 0) {
                lf_init[0] = 0;
                if (sceSifCallRpc(&lf_client, LF_GET_VERSION, 0, 0, 0, &lf_arg, 4, 0, 0) < 0) {
                    return -SCE_ECALLMISS;
                }
                memcpy(lf_iop_version, &lf_arg, 4);
                return 0;
            }
            wait = 0x100000;
            while (wait--) {
            }
        }
    }
    return 0;
}


int _lf_version(void) {
    int mismatch = 0;
    char *ee_version = lf_ee_version;
    char *iop_version = lf_iop_version;

    if (memcmp(iop_version, ee_version, 4) != 0
        && memcmp(iop_version, lf_dummy_version[0], 4) != 0) {
        mismatch = memcmp(ee_version, lf_dummy_version[0], 4) != 0;
    }
    return mismatch;
}


int sceSifLoadFileReset(void) {
    lf_init[0] = -1;
    memset(lf_iop_version, 0, 4);
    return 0;
}

int _sceSifLoadModuleBuffer(const void *addr, int arg_len, const char *args,
                            int *result) {
    int module_id;

    if (_lf_bind() < 0) {
        return -SCE_EBINDMISS;
    }
    if (_lf_version() != 0) {
        return -SCE_EVERSIONMISS;
    }
    lf_arg.p.addr = addr;
    if (args != 0) {
        if (arg_len > LF_ARG_MAX) {
            memcpy(lf_arg.args, args, LF_ARG_MAX);
            lf_arg.q.arg_len = LF_ARG_MAX;
        } else {
            memcpy(lf_arg.args, args, arg_len);
            lf_arg.q.arg_len = arg_len;
        }
    } else {
        lf_arg.q.arg_len = 0;
    }
    if (sceSifCallRpc(&lf_client, LF_MODULE_BUFFER_LOAD, 0, &lf_arg, sizeof(lf_arg), &lf_arg, 8, 0, 0) < 0) {
        return -SCE_ECALLMISS;
    }
    module_id = lf_arg.p.result;
    *result = lf_arg.q.start_result;
    return module_id;
}


int sceSifStopModule(int id, int arg_len, const char *args, int *result) {
    int module_id;

    if (_lf_bind() < 0) {
        return -SCE_EBINDMISS;
    }
    if (_lf_version() != 0) {
        return -SCE_EVERSIONMISS;
    }
    lf_arg.p.id = id;
    if (args != 0) {
        if (arg_len > LF_ARG_MAX) {
            memcpy(lf_arg.args, args, LF_ARG_MAX);
            lf_arg.q.arg_len = LF_ARG_MAX;
        } else {
            memcpy(lf_arg.args, args, arg_len);
            lf_arg.q.arg_len = arg_len;
        }
    } else {
        lf_arg.q.arg_len = 0;
    }
    if (sceSifCallRpc(&lf_client, LF_MODULE_STOP, 0, &lf_arg, sizeof(lf_arg), &lf_arg, 8, 0, 0) < 0) {
        return -SCE_ECALLMISS;
    }
    module_id = lf_arg.p.result;
    *result = lf_arg.q.start_result;
    return module_id;
}


int sceSifUnloadModule(int id) {
    if (_lf_bind() < 0) {
        return -SCE_EBINDMISS;
    }
    if (_lf_version() != 0) {
        return -SCE_EVERSIONMISS;
    }
    lf_arg.p.id = id;
    if (sceSifCallRpc(&lf_client, LF_UNLOAD, 0, &lf_arg, 4, &lf_arg, 4, 0, 0) < 0) {
        return -SCE_ECALLMISS;
    }
    return lf_arg.p.result;
}


int sceSifSearchModuleByName(const char *name) {
    if (_lf_bind() < 0) {
        return -SCE_EBINDMISS;
    }
    if (_lf_version() != 0) {
        return -SCE_EVERSIONMISS;
    }
    strncpy(lf_arg.path, name, LF_PATH_MAX);
    lf_arg.path[LF_PATH_MAX - 1] = 0;
    if (sceSifCallRpc(&lf_client, LF_SEARCH_NAME, 0, &lf_arg, sizeof(lf_arg), &lf_arg, 4, 0, 0) < 0) {
        return -SCE_ECALLMISS;
    }
    return lf_arg.p.result;
}


int sceSifSearchModuleByAddress(const void *addr) {
    if (_lf_bind() < 0) {
        return -SCE_EBINDMISS;
    }
    if (_lf_version() != 0) {
        return -SCE_EVERSIONMISS;
    }
    lf_arg.p.addr = addr;
    if (sceSifCallRpc(&lf_client, LF_SEARCH_ADDR, 0, &lf_arg, 4, &lf_arg, 4, 0, 0) < 0) {
        return -SCE_ECALLMISS;
    }
    return lf_arg.p.result;
}


int sceSifLoadModuleBuffer(const void *addr, int arg_len, const char *args) {
    int result;

    return _sceSifLoadModuleBuffer(addr, arg_len, args, &result);
}


int sceSifLoadStartModuleBuffer(const void *addr, int arg_len,
                                const char *args, int *result) {
    return _sceSifLoadModuleBuffer(addr, arg_len, args, result);
}


int _sceSifLoadModule(const char *filename, int arg_len, const char *args,
                      int *result, int mode) {
    int module_id;

    if (_lf_bind() < 0) {
        return -SCE_EBINDMISS;
    }
    if (_lf_version() != 0) {
        return -SCE_EVERSIONMISS;
    }
    strncpy(lf_arg.path, filename, LF_PATH_MAX);
    lf_arg.path[LF_PATH_MAX - 1] = 0;
    if (args != 0) {
        if (arg_len > LF_ARG_MAX) {
            memcpy(lf_arg.args, args, LF_ARG_MAX);
            lf_arg.p.arg_len = LF_ARG_MAX;
        } else {
            memcpy(lf_arg.args, args, arg_len);
            lf_arg.p.arg_len = arg_len;
        }
    } else {
        lf_arg.args[0] = 0;
        lf_arg.p.arg_len = 0;
    }
    if (sceSifCallRpc(&lf_client, mode, 0, &lf_arg, sizeof(lf_arg), &lf_arg, 8, 0, 0) < 0) {
        return -SCE_ECALLMISS;
    }
    module_id = lf_arg.p.result;
    *result = lf_arg.q.start_result;
    return module_id;
}


int sceSifLoadModule(const char *filename, int arg_len, const char *args) {
    int result;

    return _sceSifLoadModule(filename, arg_len, args, &result, 0);
}


int sceSifLoadStartModule(const char *filename, int arg_len,
                          const char *args, int *result) {
    return _sceSifLoadModule(filename, arg_len, args, result, 0);
}


int _sceSifLoadElfPart(const char *name, const char *secname,
                       sceExecData *data, int mode) {
    if (_lf_bind() < 0) {
        return -SCE_EBINDMISS;
    }
    if (_lf_version() != 0) {
        return -SCE_EVERSIONMISS;
    }
    strncpy(lf_arg.path, name, LF_PATH_MAX);
    lf_arg.path[LF_PATH_MAX - 1] = 0;
    strncpy(lf_arg.args, secname, LF_ARG_MAX);
    lf_arg.args[LF_ARG_MAX - 1] = 0;
    if (sceSifCallRpc(&lf_client, mode, 0, &lf_arg, sizeof(lf_arg), &lf_arg, sizeof(sceExecData), 0, 0) < 0) {
        return -SCE_ECALLMISS;
    }
    if (lf_arg.p.entry_point == 0) {
        return -SCE_ELOADMISS;
    }
    data->entry_point = lf_arg.p.entry_point;
    data->global_pointer = lf_arg.q.global_pointer;
    return 0;
}


int sceSifLoadElfPart(const char *name, const char *secname,
                      sceExecData *data) {
    return _sceSifLoadElfPart(name, secname, data, 1);
}


int sceSifLoadElf(const char *name, sceExecData *data) {
    return _sceSifLoadElfPart(name, lf_all_sections, data, 1);
}


int sceSifGetIopAddr(unsigned int iop_addr, void *val, int type) {
    if (_lf_bind() < 0) {
        return -SCE_EBINDMISS;
    }
    if (type < 0 || type > 2) {
        return -SCE_ETYPEMISS;
    }
    lf_iopval.p.iop_addr = iop_addr;
    lf_iopval.type = type;
    if (sceSifCallRpc(&lf_client, LF_GET_ADDR, 0, &lf_iopval, 32, &lf_iopval, 32, 0, 0) < 0) {
        return -SCE_ECALLMISS;
    }
    if (type == 0) {
        *(unsigned char *)val = lf_iopval.p.result;
    } else if (type == 1) {
        *(unsigned short *)val = lf_iopval.p.result;
    } else if (type == 2) {
        *(unsigned int *)val = lf_iopval.p.result;
    } else {
        return -SCE_ETYPEMISS;
    }
    return 0;
}


int sceSifSetIopAddr(unsigned int iop_addr, void *val, int type) {
    if (_lf_bind() < 0) {
        return -SCE_EBINDMISS;
    }
    lf_iopval.p.iop_addr = iop_addr;
    lf_iopval.type = type;
    if (type == 0) {
        lf_iopval.val.b = *(unsigned char *)val;
    } else if (type == 1) {
        lf_iopval.val.s = *(unsigned short *)val;
    } else if (type == 2) {
        lf_iopval.val.l = *(unsigned int *)val;
    } else {
        return -SCE_ETYPEMISS;
    }
    if (sceSifCallRpc(&lf_client, LF_SET_ADDR, 0, &lf_iopval, 32, &lf_iopval, 16, 0, 0) < 0) {
        return -SCE_ECALLMISS;
    }
    return 0;
}
