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

extern char D_00612BB4[];

#define lf_ee_version   D_00612BB4

static int _bind_check = -1;
static char *_lfwildcard = "....";

static LoadFileSendData _senddata;
static sceSifClientData cd;
static char _lfversion[4];

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

    if (_bind_check < 0) {
        while (1) {
            if (sceSifBindRpc(&cd, LF_RPC_SERVER, 0) < 0) {
                return -1;
            }
            if (cd.server != 0) {
                _bind_check = 0;
                if (sceSifCallRpc(&cd, LF_GET_VERSION, 0, 0, 0, &_senddata.file, 4, 0, 0) < 0) {
                    return -SCE_ECALLMISS;
                }
                memcpy(_lfversion, &_senddata.file, 4);
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
    char *iop_version = _lfversion;

    if (memcmp(iop_version, ee_version, 4) != 0
        && memcmp(iop_version, _lfwildcard, 4) != 0) {
        mismatch = memcmp(ee_version, _lfwildcard, 4) != 0;
    }
    return mismatch;
}


int sceSifLoadFileReset(void) {
    _bind_check = -1;
    memset(_lfversion, 0, 4);
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
    _senddata.file.p.addr = addr;
    if (args != 0) {
        if (arg_len > LF_ARG_MAX) {
            memcpy(_senddata.file.args, args, LF_ARG_MAX);
            _senddata.file.q.arg_len = LF_ARG_MAX;
        } else {
            memcpy(_senddata.file.args, args, arg_len);
            _senddata.file.q.arg_len = arg_len;
        }
    } else {
        _senddata.file.q.arg_len = 0;
    }
    if (sceSifCallRpc(&cd, LF_MODULE_BUFFER_LOAD, 0, &_senddata.file, sizeof(_senddata.file), &_senddata.file, 8, 0, 0) < 0) {
        return -SCE_ECALLMISS;
    }
    module_id = _senddata.file.p.result;
    *result = _senddata.file.q.start_result;
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
    _senddata.file.p.id = id;
    if (args != 0) {
        if (arg_len > LF_ARG_MAX) {
            memcpy(_senddata.file.args, args, LF_ARG_MAX);
            _senddata.file.q.arg_len = LF_ARG_MAX;
        } else {
            memcpy(_senddata.file.args, args, arg_len);
            _senddata.file.q.arg_len = arg_len;
        }
    } else {
        _senddata.file.q.arg_len = 0;
    }
    if (sceSifCallRpc(&cd, LF_MODULE_STOP, 0, &_senddata.file, sizeof(_senddata.file), &_senddata.file, 8, 0, 0) < 0) {
        return -SCE_ECALLMISS;
    }
    module_id = _senddata.file.p.result;
    *result = _senddata.file.q.start_result;
    return module_id;
}


int sceSifUnloadModule(int id) {
    if (_lf_bind() < 0) {
        return -SCE_EBINDMISS;
    }
    if (_lf_version() != 0) {
        return -SCE_EVERSIONMISS;
    }
    _senddata.file.p.id = id;
    if (sceSifCallRpc(&cd, LF_UNLOAD, 0, &_senddata.file, 4, &_senddata.file, 4, 0, 0) < 0) {
        return -SCE_ECALLMISS;
    }
    return _senddata.file.p.result;
}


int sceSifSearchModuleByName(const char *name) {
    if (_lf_bind() < 0) {
        return -SCE_EBINDMISS;
    }
    if (_lf_version() != 0) {
        return -SCE_EVERSIONMISS;
    }
    strncpy(_senddata.file.path, name, LF_PATH_MAX);
    _senddata.file.path[LF_PATH_MAX - 1] = 0;
    if (sceSifCallRpc(&cd, LF_SEARCH_NAME, 0, &_senddata.file, sizeof(_senddata.file), &_senddata.file, 4, 0, 0) < 0) {
        return -SCE_ECALLMISS;
    }
    return _senddata.file.p.result;
}


int sceSifSearchModuleByAddress(const void *addr) {
    if (_lf_bind() < 0) {
        return -SCE_EBINDMISS;
    }
    if (_lf_version() != 0) {
        return -SCE_EVERSIONMISS;
    }
    _senddata.file.p.addr = addr;
    if (sceSifCallRpc(&cd, LF_SEARCH_ADDR, 0, &_senddata.file, 4, &_senddata.file, 4, 0, 0) < 0) {
        return -SCE_ECALLMISS;
    }
    return _senddata.file.p.result;
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
    strncpy(_senddata.file.path, filename, LF_PATH_MAX);
    _senddata.file.path[LF_PATH_MAX - 1] = 0;
    if (args != 0) {
        if (arg_len > LF_ARG_MAX) {
            memcpy(_senddata.file.args, args, LF_ARG_MAX);
            _senddata.file.p.arg_len = LF_ARG_MAX;
        } else {
            memcpy(_senddata.file.args, args, arg_len);
            _senddata.file.p.arg_len = arg_len;
        }
    } else {
        _senddata.file.args[0] = 0;
        _senddata.file.p.arg_len = 0;
    }
    if (sceSifCallRpc(&cd, mode, 0, &_senddata.file, sizeof(_senddata.file), &_senddata.file, 8, 0, 0) < 0) {
        return -SCE_ECALLMISS;
    }
    module_id = _senddata.file.p.result;
    *result = _senddata.file.q.start_result;
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
    strncpy(_senddata.file.path, name, LF_PATH_MAX);
    _senddata.file.path[LF_PATH_MAX - 1] = 0;
    strncpy(_senddata.file.args, secname, LF_ARG_MAX);
    _senddata.file.args[LF_ARG_MAX - 1] = 0;
    if (sceSifCallRpc(&cd, mode, 0, &_senddata.file, sizeof(_senddata.file), &_senddata.file, sizeof(sceExecData), 0, 0) < 0) {
        return -SCE_ECALLMISS;
    }
    if (_senddata.file.p.entry_point == 0) {
        return -SCE_ELOADMISS;
    }
    data->entry_point = _senddata.file.p.entry_point;
    data->global_pointer = _senddata.file.q.global_pointer;
    return 0;
}


int sceSifLoadElfPart(const char *name, const char *secname,
                      sceExecData *data) {
    return _sceSifLoadElfPart(name, secname, data, 1);
}


int sceSifLoadElf(const char *name, sceExecData *data) {
    return _sceSifLoadElfPart(name, "all", data, 1);
}


int sceSifGetIopAddr(unsigned int iop_addr, void *val, int type) {
    if (_lf_bind() < 0) {
        return -SCE_EBINDMISS;
    }
    if (type < 0 || type > 2) {
        return -SCE_ETYPEMISS;
    }
    _senddata.iopval.p.iop_addr = iop_addr;
    _senddata.iopval.type = type;
    if (sceSifCallRpc(&cd, LF_GET_ADDR, 0, &_senddata.iopval, 32, &_senddata.iopval, 32, 0, 0) < 0) {
        return -SCE_ECALLMISS;
    }
    if (type == 0) {
        *(unsigned char *)val = _senddata.iopval.p.result;
    } else if (type == 1) {
        *(unsigned short *)val = _senddata.iopval.p.result;
    } else if (type == 2) {
        *(unsigned int *)val = _senddata.iopval.p.result;
    } else {
        return -SCE_ETYPEMISS;
    }
    return 0;
}


int sceSifSetIopAddr(unsigned int iop_addr, void *val, int type) {
    if (_lf_bind() < 0) {
        return -SCE_EBINDMISS;
    }
    _senddata.iopval.p.iop_addr = iop_addr;
    _senddata.iopval.type = type;
    if (type == 0) {
        _senddata.iopval.val.b = *(unsigned char *)val;
    } else if (type == 1) {
        _senddata.iopval.val.s = *(unsigned short *)val;
    } else if (type == 2) {
        _senddata.iopval.val.l = *(unsigned int *)val;
    } else {
        return -SCE_ETYPEMISS;
    }
    if (sceSifCallRpc(&cd, LF_SET_ADDR, 0, &_senddata.iopval, 32, &_senddata.iopval, 16, 0, 0) < 0) {
        return -SCE_ECALLMISS;
    }
    return 0;
}
