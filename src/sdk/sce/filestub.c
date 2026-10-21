/*
 * Unit: sdk/sce/filestub
 *
 * Functions:
 *   0x0027c2c8 _sceFsIobSemaMK
 *   0x0027c328 new_iob
 *   0x0027c3b0 get_iob
 *   0x0027c420 _sceFs_Rcv_Intr
 *   0x0027c7e0 _sceFsSemInit
 *   0x0027c830 _sceFsWaitS
 *   0x0027c860 _sceFsSigSema
 *   0x0027c870 sceFsInit
 *   0x0027ca10 _fs_version
 *   0x0027caa0 sceFsReset
 *   0x0027cad8 sceOpen
 *   0x0027cd60 sceClose
 *   0x0027cee0 sceLseek
 *   0x0027d118 sceRead
 *   0x0027d388 sceWrite
 *   0x0027d648 sceIoctl
 *   0x0027d998 sceIoctl2
 *   0x0027db70 _sceCallCode
 *   0x0027dd18 sceRemove
 *   0x0027dd38 sceMkdir
 *   0x0027dee8 sceRmdir
 *   0x0027df08 sceFormat
 *   0x0027e178 sceAddDrv
 *   0x0027e298 sceDelDrv
 *   0x0027e2b8 sceDopen
 *   0x0027e358 sceDclose
 *   0x0027e4c0 sceDread
 *   0x0027e618 sceGetstat
 *   0x0027e7b8 sceChstat
 *   0x0027e9f8 sceRename
 *   0x0027ebe8 sceChdir
 *   0x0027ec08 sceSync
 *   0x0027eda0 sceMount
 *   0x0027f010 sceUmount
 *   0x0027f030 sceLseek64
 *   0x0027f268 sceDevctl
 *   0x0027f498 sceSymlink
 *   0x0027f678 sceReadlink
 */

#define FS_NIOB 32
#define FS_IOB_BUSY 0x10000000
#define FS_NQUEUE 32
#define FS_NAME_MAX 0x400
#define FS_IOCTL_ARG_MAX 0x400
#define FS_BIND_WAIT 0x100000
#define FS_RCV_READ_SIZE 0xa4

#define FS_RPC_SERVER 0x80000001
#define FS_CMD_RCV 0x80000011

#define FS_OPEN 0
#define FS_CLOSE 1
#define FS_READ 2
#define FS_WRITE 3
#define FS_LSEEK 4
#define FS_IOCTL 5
#define FS_MKDIR 7
#define FS_DCLOSE 10
#define FS_DREAD 11
#define FS_GETSTAT 12
#define FS_CHSTAT 13
#define FS_FORMAT 14
#define FS_ADDDRV 15
#define FS_RENAME 17
#define FS_SYNC 19
#define FS_MOUNT 20
#define FS_LSEEK64 22
#define FS_DEVCTL 23
#define FS_SYMLINK 24
#define FS_READLINK 25
#define FS_IOCTL2 26
#define FS_INIT 0xff

#define FS_OPEN_FLAG_MASK 0x0fffffff
#define FS_NOWAIT 0x8000
#define FS_UNCACHED_BUF 0x20000000

#define FS_IOCTL_PENDING 1
#define FS_IOCTL_RESULT 2
#define FS_IOCTL_RESULT64 3

#define SCE_EBADF 9
#define SCE_E2BIG 7
#define SCE_EAGAIN 11
#define SCE_ENODEV 19
#define SCE_EINVAL 22
#define SCE_ECALLMISS 0x10001
#define SCE_EVERSIONMISS 0x10004

#define UNCACHED(p) ((void *)((unsigned int)(p) | 0x20000000))

typedef char *va_list;
#define va_start(ap, last) ((ap) = (va_list)__builtin_next_arg(last) - (__builtin_args_info(2) < 8 ? (8 - __builtin_args_info(2)) * 8 : 0))
#define va_arg(ap, type) (*(type *)(((ap) += 8) - 8))
#define va_end(ap)

typedef struct SemaParam {
    int currentCount;
    int maxCount;
    int initCount;
    int numWaitThreads;
    unsigned int attr;
    unsigned int option;
} SemaParam;

typedef struct FsIob {
    int fd;
    int flags;
    int pad[2];
} FsIob;

typedef struct sce_stat {
    unsigned int st_mode;
    unsigned int st_attr;
    unsigned int st_size;
    unsigned char st_ctime[8];
    unsigned char st_atime[8];
    unsigned char st_mtime[8];
    unsigned int st_hisize;
    unsigned int st_private[6];
} sce_stat;
typedef struct FsRcvHeader {
    int semid;
    int code;
    void *dest;
    int size;
} FsRcvHeader;

typedef struct FsReadSplit {
    int head_size;
    int tail_size;
    char *head_dest;
    char *tail_dest;
    char head[64];
    char tail[64];
} FsReadSplit;

typedef struct FsRcvData {
    FsRcvHeader hdr;
    int result;
    union {
        FsReadSplit read;
        struct {
            void *dest;
            char dirent[0x144];
        } dread;
        struct {
            void *dest;
            char stat[0x40];
        } getstat;
        struct {
            void *dest;
            unsigned int size;
            char data[0x400];
        } buf;
    } u;
} FsRcvData;

typedef struct FsCallArg {
    int semid;
    int *result;
    int result_size;
    char name[FS_NAME_MAX];
} FsCallArg;

typedef struct FsOpenArg {
    int semid;
    int *result;
    int result_size;
    int flags;
    int mode;
    char name[FS_NAME_MAX];
    int fd;
} FsOpenArg;

typedef struct FsCloseArg {
    int semid;
    int *result;
    int result_size;
    int fd;
    int index;
} FsCloseArg;

typedef struct FsLseekArg {
    int semid;
    int *result;
    int result_size;
    int fd;
    int offset;
    int whence;
    int index;
} FsLseekArg;

typedef struct FsReadArg {
    int semid;
    int *result;
    int result_size;
    int fd;
    void *buf;
    int size;
    int reserved;
    int index;
} FsReadArg;

typedef struct FsWriteArg {
    int semid;
    int *result;
    int result_size;
    int fd;
    const void *buf;
    int size;
    int head_size;
    char head[16];
    int index;
} FsWriteArg;

typedef struct FsIoctlArg {
    int semid;
    int *result;
    int result_size;
    int fd;
    int request;
    char arg[FS_IOCTL_ARG_MAX];
    void *buf;
    unsigned int buflen;
    unsigned int arglen;
} FsIoctlArg;

typedef struct FsMkdirArg {
    int semid;
    int *result;
    int result_size;
    int mode;
    char name[FS_NAME_MAX];
} FsMkdirArg;

typedef struct FsFormatArg {
    int semid;
    int *result;
    int result_size;
    char dev[FS_NAME_MAX];
    char blockdev[FS_NAME_MAX];
    char arg[FS_IOCTL_ARG_MAX];
    int arglen;
} FsFormatArg;

typedef struct FsAddDrvArg {
    int semid;
    int *result;
    int result_size;
    void *device;
} FsAddDrvArg;

typedef struct FsGetstatArg {
    int semid;
    int *result;
    int result_size;
    sce_stat *stat;
    unsigned char name[FS_NAME_MAX];
} FsGetstatArg;

typedef struct FsChstatArg {
    int semid;
    int *result;
    int result_size;
    unsigned int cbit;
    sce_stat stat;
    unsigned char name[FS_NAME_MAX];
} FsChstatArg;

typedef struct FsRenameArg {
    int semid;
    int *result;
    int result_size;
    char oldname[FS_NAME_MAX];
    char newname[FS_NAME_MAX];
} FsRenameArg;

typedef struct FsSyncArg {
    int semid;
    int *result;
    int result_size;
    int reserved;
    int flag;
    char name[FS_NAME_MAX];
} FsSyncArg;

typedef struct FsMountArg {
    int semid;
    int *result;
    int result_size;
    char fsname[FS_NAME_MAX];
    char devname[FS_NAME_MAX];
    char arg[FS_IOCTL_ARG_MAX];
    int flag;
    int arglen;
} FsMountArg;

typedef struct FsLseek64Arg {
    int semid;
    long *result;
    int result_size;
    int fd;
    long offset;
    int whence;
    int index;
} FsLseek64Arg;

typedef struct FsDevctlArg {
    int semid;
    int *result;
    int result_size;
    char name[FS_NAME_MAX];
    char arg[FS_IOCTL_ARG_MAX];
    int cmd;
    unsigned int arglen;
    void *buf;
    unsigned int buflen;
} FsDevctlArg;

typedef struct FsSymlinkArg {
    int semid;
    int *result;
    int result_size;
    char existing[FS_NAME_MAX];
    char newname[FS_NAME_MAX];
} FsSymlinkArg;

typedef struct FsReadlinkArg {
    int semid;
    int *result;
    int result_size;
    unsigned int buflen;
    char *buf;
    char name[FS_NAME_MAX];
} FsReadlinkArg;
typedef union FsSendData {
    FsCallArg call;
    FsOpenArg open;
    FsCloseArg close;
    FsLseekArg lseek;
    FsReadArg read;
    FsWriteArg write;
    FsIoctlArg ioctl;
    FsMkdirArg mkdir;
    FsFormatArg format;
    FsAddDrvArg adddrv;
    FsGetstatArg getstat;
    FsChstatArg chstat;
    FsRenameArg rename;
    FsSyncArg sync;
    FsMountArg mount;
    FsLseek64Arg lseek64;
    FsDevctlArg devctl;
    FsSymlinkArg symlink;
    FsReadlinkArg readlink;
} FsSendData;

typedef struct sceSifClientData {
    unsigned char rpc_data[16];
    unsigned int command;
    void *buff;
    void *cbuff;
    void (*end_func)(void *);
    void *end_param;
    void *server;
} sceSifClientData;

extern int D_00613400;
extern int D_00613404;
extern int D_00613408;
extern int D_0061340C;
extern FsIob D_00706280[];
extern FsRcvData D_00705E40;
extern FsSendData D_007051C0;
extern char D_00705E00[];
extern char D_007064C0[];
extern sceSifClientData D_00706480;
extern volatile int _sceFs_q[];
extern char D_007064A8[];
extern char D_00612BB4[];
extern char *D_00613410;
extern int *D_00705180;

#define fs_initialized  D_00613400
#define fs_sema         D_00613404
#define fs_iob_sema     D_00613408
#define fs_io_sema      D_0061340C
#define fs_iob          D_00706280
#define fs_rcv_data     D_00705E40
#define fs_send         D_007051C0
#define fs_rcv_rpc      D_00705E00
#define fs_cmd_data     D_007064C0
#define fs_client       D_00706480
#define fs_iop_version  D_007064A8
#define fs_ee_version   D_00612BB4
#define fs_wildcard     D_00613410
#define fs_ioctl_out    D_00705180

extern int CreateSema(SemaParam *param);
extern int WaitSema(int sema);
extern int SignalSema(int sema);
extern int DeleteSema(int sema);
extern int iSignalSema(int sema);
extern void *memset(void *dest, int c, unsigned int n);
extern void *memcpy(void *dest, const void *src, unsigned int n);
extern int memcmp(const void *a, const void *b, unsigned int n);
extern int _sceCallCode(const char *name, int code);
extern void sceSifWriteBackDCache(void *ptr, int size);
extern void sceSifInitRpc(unsigned int mode);
extern int DIntr(void);
extern int EIntr(void);
extern void sceSifAddCmdHandler(unsigned int cid, void (*handler)(void *, void *), void *data);
extern int sceSifBindRpc(sceSifClientData *client, unsigned int request,
                         unsigned int mode);
extern int sceSifCallRpc(sceSifClientData *client, unsigned int fno,
                         unsigned int mode, void *send, int ssize,
                         void *receive, int rsize, void (*end_func)(void *),
                         void *end_param);


void _sceFsIobSemaMK(void)
{
    SemaParam sema;

    if (fs_iob_sema == -1) {
        sema.initCount = 1;
        sema.maxCount = 1;
        sema.option = 0;
        fs_iob_sema = CreateSema(&sema);
        fs_io_sema = CreateSema(&sema);
    }
}


FsIob *new_iob(void)
{
    FsIob *iob;

    _sceFsIobSemaMK();
    WaitSema(fs_iob_sema);
    for (iob = fs_iob; iob < &fs_iob[FS_NIOB]; iob++) {
        if (iob->flags == 0) {
            iob->flags = FS_IOB_BUSY;
            SignalSema(fs_iob_sema);
            return iob;
        }
    }
    SignalSema(fs_iob_sema);
    return 0;
}


FsIob *get_iob(int fd)
{
    FsIob *iob;

    _sceFsIobSemaMK();
    WaitSema(fs_iob_sema);
    if (fd < 0 || fd >= FS_NIOB) {
        SignalSema(fs_iob_sema);
        return 0;
    }
    iob = &fs_iob[fd];
    SignalSema(fs_iob_sema);
    return iob;
}


void _sceFs_Rcv_Intr(void *pkt, void *param)
{
    int semid;
    int code;
    void *rcv_dest;
    int rcv_size;
    void *dest;
    unsigned int size;
    FsReadSplit *split;
    char *dst;
    int i;

    memcpy(&semid, UNCACHED(&fs_rcv_data.hdr.semid), 4);
    memcpy(&code, UNCACHED(&fs_rcv_data.hdr.code), 4);
    memcpy(&rcv_dest, UNCACHED(&fs_rcv_data.hdr.dest), 4);
    memcpy(&rcv_size, UNCACHED(&fs_rcv_data.hdr.size), 4);
    if (semid >= 0) {
        memcpy(rcv_dest, UNCACHED(&fs_rcv_data.result), rcv_size);
    }
    switch (code) {
    case 2:
        split = UNCACHED(&fs_rcv_data.u.read);
        if (split->head_size > 0) {
            dst = split->head_dest;
            for (i = 0; i < split->head_size; i++) {
                dst[i] = split->head[i];
            }
        }
        if (split->tail_size > 0) {
            dst = split->tail_dest;
            for (i = 0; i < split->tail_size; i++) {
                dst[i] = split->tail[i];
            }
        }
        break;
    case 11:
        memcpy(&dest, UNCACHED(&fs_rcv_data.u.dread.dest), 4);
        memcpy(dest, UNCACHED(fs_rcv_data.u.dread.dirent), sizeof(fs_rcv_data.u.dread.dirent));
        break;
    case 12:
        memcpy(&dest, UNCACHED(&fs_rcv_data.u.getstat.dest), 4);
        memcpy(dest, UNCACHED(fs_rcv_data.u.getstat.stat), sizeof(fs_rcv_data.u.getstat.stat));
        break;
    case 23:
    case 25:
    case 26:
        memcpy(&dest, UNCACHED(&fs_rcv_data.u.buf.dest), 4);
        memcpy(&size, UNCACHED(&fs_rcv_data.u.buf.size), 4);
        if (size > 0x400) {
            size = 0x400;
        }
        memcpy(dest, UNCACHED(fs_rcv_data.u.buf.data), size);
        break;
    }
    if (semid < 0) {
        semid = -semid;
        for (i = 0; i < FS_NQUEUE; i++) {
            if (_sceFs_q[i] == semid) {
                _sceFs_q[i] = -1;
                break;
            }
        }
    } else {
        iSignalSema(semid);
    }
}


void _sceFsSemInit(void)
{
    SemaParam sema;

    if (fs_sema == -1) {
        sema.initCount = 1;
        sema.maxCount = 1;
        sema.option = 0;
        fs_sema = CreateSema(&sema);
    }
}


int _sceFsWaitS(int code)
{
    _sceFsSemInit();
    WaitSema(fs_sema);
    return 0;
}


void _sceFsSigSema(void)
{
    SignalSema(fs_sema);
}


int sceFsInit(void)
{
    void *cmd_buf;
    FsIob *iob;
    int wait;

    sceSifInitRpc(0);
    DIntr();
    sceSifAddCmdHandler(FS_CMD_RCV, _sceFs_Rcv_Intr, fs_cmd_data);
    EIntr();
    while (1) {
        if (sceSifBindRpc(&fs_client, FS_RPC_SERVER, 0) < 0) {
            return -1;
        }
        if (fs_client.server != 0) {
            break;
        }
        wait = FS_BIND_WAIT;
        while (wait--) {
        }
    }
    _sceFsIobSemaMK();
    WaitSema(fs_iob_sema);
    for (iob = fs_iob; iob < &fs_iob[FS_NIOB]; iob++) {
        iob->flags = 0;
    }
    SignalSema(fs_iob_sema);
    cmd_buf = &fs_rcv_data;
    if (sceSifCallRpc(&fs_client, FS_INIT, 0, &cmd_buf, 4, fs_rcv_rpc, 4, 0, 0) < 0) {
        return -SCE_ECALLMISS;
    }
    memcpy(fs_iop_version, UNCACHED(fs_rcv_rpc), 4);
    fs_initialized = 1;
    return 0;
}


int _fs_version(void)
{
    int mismatch = 0;
    char *ee_version = fs_ee_version;
    char *iop_version = fs_iop_version;

    if (memcmp(iop_version, ee_version, 4) != 0
        && memcmp(iop_version, fs_wildcard, 4) != 0) {
        mismatch = memcmp(ee_version, fs_wildcard, 4) != 0;
    }
    return mismatch;
}


int sceFsReset(void)
{
    fs_initialized = 0;
    memset(fs_iop_version, 0, 4);
    return 0;
}


int sceOpen(const char *filename, int flag, ...)
{
    FsOpenArg *arg = &fs_send.open;
    va_list ap;
    SemaParam param;
    int result;
    int len;
    int sema;
    int rv;
    int mode;
    int fd;
    FsIob *iob;

    va_start(ap, flag);
    _sceFsWaitS(FS_OPEN);
    if (!fs_initialized) {
        sceFsInit();
    }
    if (_fs_version()) {
        _sceFsSigSema();
        return -SCE_EVERSIONMISS;
    }
    iob = new_iob();
    if (iob == 0) {
        _sceFsSigSema();
        return -SCE_ENODEV;
    }
    mode = va_arg(ap, int);
    va_end(ap);
    for (len = 0; len < FS_NAME_MAX; len++) {
        if ((arg->name[len] = filename[len]) == '\0') {
            break;
        }
    }
    if (len == FS_NAME_MAX) {
        arg->name[FS_NAME_MAX - 1] = '\0';
    }
    fd = iob - fs_iob;
    arg->flags = flag & FS_OPEN_FLAG_MASK;
    arg->mode = mode;
    arg->fd = fd;
    param.initCount = 0;
    param.maxCount = 1;
    param.option = 0;
    sema = CreateSema(&param);
    arg->semid = sema;
    arg->result = &result;
    arg->result_size = 4;
    if (sceSifCallRpc(&fs_client, FS_OPEN, 0, &fs_send, sizeof(FsOpenArg), fs_rcv_rpc, 4, 0, 0) < 0) {
        DeleteSema(sema);
        _sceFsSigSema();
        return -SCE_EAGAIN;
    }
    rv = *(int *)UNCACHED(fs_rcv_rpc);
    _sceFsSigSema();
    if (rv == 0) {
        DeleteSema(sema);
        return -SCE_EAGAIN;
    }
    WaitSema(sema);
    DeleteSema(sema);
    if (result < 0) {
        WaitSema(fs_iob_sema);
        iob->flags = 0;
        SignalSema(fs_iob_sema);
        return result;
    }
    rv = fd;
    WaitSema(fs_iob_sema);
    iob->fd = result;
    iob->flags |= flag;
    SignalSema(fs_iob_sema);
    return rv;


}


int sceClose(int fd)
{
    FsCloseArg *arg = &fs_send.close;
    SemaParam param;
    int result;
    int sema;
    int rv;
    FsIob *iob;

    iob = get_iob(fd);
    _sceFsWaitS(FS_CLOSE);
    if (!fs_initialized) {
        _sceFsSigSema();
        return -1;
    }
    if (iob == 0 || iob->flags == 0) {
        _sceFsSigSema();
        return -SCE_EBADF;
    }
    arg->fd = iob->fd;
    arg->index = iob - fs_iob;
    param.initCount = 0;
    param.maxCount = 1;
    param.option = 0;
    sema = CreateSema(&param);
    arg->semid = sema;
    arg->result = &result;
    arg->result_size = 4;
    if (sceSifCallRpc(&fs_client, FS_CLOSE, 0, arg, sizeof(FsCloseArg), fs_rcv_rpc, 4, 0, 0) < 0) {
        DeleteSema(sema);
        _sceFsSigSema();
        return -SCE_EAGAIN;
    }
    iob->flags = 0;
    rv = *(int *)UNCACHED(fs_rcv_rpc);
    _sceFsSigSema();
    if (rv == 0) {
        DeleteSema(sema);
        return -SCE_EAGAIN;
    }
    WaitSema(sema);
    DeleteSema(sema);
    if (result < 0) {
        return result;
    }
    return 0;
}


int sceLseek(int fd, int offset, int whence)
{
    FsLseekArg *arg = &fs_send.lseek;
    SemaParam param;
    int result;
    int sema;
    int flags;
    int rv;
    int i;
    FsIob *iob;

    iob = get_iob(fd);
    _sceFsWaitS(FS_LSEEK);
    if (!fs_initialized) {
        _sceFsSigSema();
        return -1;
    }
    if (iob == 0 || iob->flags == 0) {
        _sceFsSigSema();
        return -SCE_EBADF;
    }
    flags = iob->flags;
    arg->fd = iob->fd;
    arg->offset = offset;

    arg->whence = whence;
    arg->index = iob - fs_iob;
    param.initCount = 0;
    param.maxCount = 1;
    param.option = 0;
    sema = CreateSema(&param);
    arg->semid = sema;
    arg->result = &result;
    arg->result_size = 4;
    if (flags & FS_NOWAIT) {
        WaitSema(fs_io_sema);
        for (i = 0; i < FS_NQUEUE; i++) {
            if (_sceFs_q[i] == -1) {
                _sceFs_q[i] = arg->semid;
                arg->semid = -arg->semid;
                break;
            }
        }
        SignalSema(fs_io_sema);
    }
    if (sceSifCallRpc(&fs_client, FS_LSEEK, 0, &fs_send, sizeof(FsLseekArg), fs_rcv_rpc, 4, 0, 0) < 0) {
        DeleteSema(sema);
        _sceFsSigSema();
        return -SCE_EAGAIN;
    }
    rv = *(int *)UNCACHED(fs_rcv_rpc);
    _sceFsSigSema();
    if (rv == 0) {
        DeleteSema(sema);
        return -SCE_EAGAIN;
    }
    if (flags & FS_NOWAIT) {
        DeleteSema(sema);
        return 0;
    }
    WaitSema(sema);
    DeleteSema(sema);
    return result;
}


int sceRead(int fd, void *buf, int nbyte)
{
    FsReadArg *arg = &fs_send.read;
    SemaParam param;
    int result;
    int sema;
    int flags;
    int rv;
    int i;
    FsIob *iob;

    iob = get_iob(fd);
    _sceFsWaitS(FS_READ);
    if (!fs_initialized) {
        _sceFsSigSema();
        return -1;
    }
    if (iob == 0 || iob->flags == 0) {
        _sceFsSigSema();
        return -SCE_EBADF;
    }
    flags = iob->flags;
    arg->fd = iob->fd;
    arg->buf = buf;
    arg->size = nbyte;
    arg->index = iob - fs_iob;
    param.initCount = 0;
    param.maxCount = 1;
    param.option = 0;
    sema = CreateSema(&param);
    arg->semid = sema;
    arg->result = &result;
    arg->result_size = 4;
    if (flags & FS_NOWAIT) {
        WaitSema(fs_io_sema);
        for (i = 0; i < FS_NQUEUE; i++) {
            if (_sceFs_q[i] == -1) {
                _sceFs_q[i] = arg->semid;
                arg->semid = -arg->semid;
                break;
            }
        }
        SignalSema(fs_io_sema);
    }
    if (!(flags & FS_UNCACHED_BUF)) {
        sceSifWriteBackDCache(buf, nbyte);
    }
    sceSifWriteBackDCache(&fs_rcv_data, FS_RCV_READ_SIZE);
    sceSifWriteBackDCache(arg, sizeof(FsReadArg));
    if (sceSifCallRpc(&fs_client, FS_READ, 0, &fs_send, sizeof(FsReadArg), fs_rcv_rpc, 4, 0, 0) < 0) {
        DeleteSema(sema);
        _sceFsSigSema();
        return -SCE_EAGAIN;
    }
    rv = *(int *)UNCACHED(fs_rcv_rpc);
    _sceFsSigSema();
    if (rv == 0) {
        DeleteSema(sema);
        return -SCE_EAGAIN;
    }
    if (flags & FS_NOWAIT) {
        DeleteSema(sema);
        return 0;
    }
    WaitSema(sema);
    DeleteSema(sema);
    return result;
}


int sceWrite(int fd, const void *buf, int nbyte)
{
    FsWriteArg *arg = &fs_send.write;
    SemaParam param;
    int result;
    int sema;
    int flags;
    int head;
    int rv;
    int i;
    int len;
    FsIob *iob;

    iob = get_iob(fd);
    _sceFsWaitS(FS_WRITE);


    if (!fs_initialized) {
        _sceFsSigSema();
        return -1;
    }
    if (iob == 0 || iob->flags == 0) {
        _sceFsSigSema();
        return -SCE_EBADF;
    }
    flags = iob->flags;
    arg->fd = iob->fd;
    arg->size = nbyte;
    arg->buf = buf;
    arg->index = iob - fs_iob;
    param.initCount = 0;
    param.maxCount = 1;
    param.option = 0;
    sema = CreateSema(&param);
    arg->semid = sema;
    arg->result = &result;
    arg->result_size = 4;
    if (flags & FS_NOWAIT) {
        WaitSema(fs_io_sema);
        for (i = 0; i < FS_NQUEUE; i++) {
            if (_sceFs_q[i] == -1) {
                _sceFs_q[i] = arg->semid;
                arg->semid = -arg->semid;
                break;
            }
        }
        SignalSema(fs_io_sema);
    }
    if (((unsigned int)buf & 0xf) == 0) {
        head = 0;
    } else {
        head = (((unsigned int)buf >> 4) << 4) + 16 - (unsigned int)buf;
    }
    if (nbyte < head) {
        head = nbyte;
    }
    if (!(flags & FS_UNCACHED_BUF)) {
        sceSifWriteBackDCache((void *)buf, nbyte);
    }
    buf = UNCACHED(buf);
    arg->head_size = head;
    for (len = 0; len < head; len++) {
        arg->head[len] = ((const char *)buf)[len];
    }
    if (sceSifCallRpc(&fs_client, FS_WRITE, 0, &fs_send, sizeof(FsWriteArg), fs_rcv_rpc, 4, 0, 0) < 0) {
        DeleteSema(sema);
        _sceFsSigSema();
        return -SCE_EAGAIN;
    }
    rv = *(int *)UNCACHED(fs_rcv_rpc);
    _sceFsSigSema();
    if (rv == 0) {
        DeleteSema(sema);
        return -SCE_EAGAIN;
    }
    if (flags & FS_NOWAIT) {
        DeleteSema(sema);
        return 0;
    }
    WaitSema(sema);
    DeleteSema(sema);
    return result;
}


int sceIoctl(int fd, int req, void *argp)
{
    FsIoctlArg *arg = &fs_send.ioctl;
    SemaParam param;
    int result;
    int sema;
    int rv;
    int i;
    FsIob *iob;

    iob = get_iob(fd);
    _sceFsWaitS(FS_IOCTL);
    fs_ioctl_out = argp;
    if (!fs_initialized) {
        sceFsInit();
    }
    if (iob == 0 || iob->flags == 0) {
        _sceFsSigSema();
        return -SCE_EBADF;
    }
    arg->buf = 0;
    arg->buflen = 0;
    switch (req) {
    case FS_IOCTL_PENDING:
        WaitSema(fs_io_sema);
        for (i = 0; i < FS_NQUEUE; i++) {
            if (_sceFs_q[i] != -1) {
                break;
            }
        }
        if (i == FS_NQUEUE) {
            *fs_ioctl_out = 0;
        } else {
            *fs_ioctl_out = 1;
        }
        SignalSema(fs_io_sema);
        _sceFsSigSema();
        return 0;
    case FS_IOCTL_RESULT:
        *(int *)argp = *(int *)UNCACHED(&fs_rcv_data.result);
        _sceFsSigSema();
        return 0;
    case FS_IOCTL_RESULT64:
        *(long *)argp = *(long *)UNCACHED(&fs_rcv_data.result);
        _sceFsSigSema();
        return 0;
    }
    arg->fd = iob->fd;
    arg->request = req;
    if (argp == 0) {
        arg->arglen = 0;
    } else {
        arg->arglen = FS_IOCTL_ARG_MAX;
        memcpy(arg->arg, argp, FS_IOCTL_ARG_MAX);
    }
    param.initCount = 0;
    param.maxCount = 1;
    param.option = 0;
    sema = CreateSema(&param);
    arg->semid = sema;
    arg->result = &result;
    arg->result_size = 4;
    sceSifWriteBackDCache(&fs_send, sizeof(FsIoctlArg));
    if (sceSifCallRpc(&fs_client, FS_IOCTL, 0, &fs_send, sizeof(FsIoctlArg), fs_rcv_rpc, 4, 0, 0) < 0) {
        DeleteSema(sema);
        _sceFsSigSema();
        return -SCE_EAGAIN;
    }
    rv = *(int *)UNCACHED(fs_rcv_rpc);
    _sceFsSigSema();
    if (rv == 0) {
        DeleteSema(sema);
        return -SCE_EAGAIN;
    }
    WaitSema(sema);
    DeleteSema(sema);
    return result;
}


int sceIoctl2(int fd, int request, void *argp, unsigned int arglen, void *bufp, unsigned int buflen)
{
    FsIoctlArg *arg = &fs_send.ioctl;
    SemaParam param;
    int result;
    int sema;
    int rv;
    FsIob *iob;

    iob = get_iob(fd);
    _sceFsWaitS(FS_IOCTL2);
    if (!fs_initialized) {
        sceFsInit();
    }
    if (iob == 0 || iob->flags == 0) {
        _sceFsSigSema();
        return -SCE_EBADF;
    }
    if (arglen > FS_IOCTL_ARG_MAX || buflen > FS_IOCTL_ARG_MAX) {
        return -SCE_EINVAL;
    }
    if (argp == 0) {
        arg->arglen = 0;
    } else {
        memcpy(arg->arg, argp, arglen);
    }
    arg->fd = iob->fd;
    arg->request = request;
    arg->arglen = arglen;
    param.initCount = 0;
    param.maxCount = 1;
    param.option = 0;
    sema = CreateSema(&param);
    arg->semid = sema;
    arg->result = &result;
    arg->result_size = 4;
    arg->buf = bufp;
    arg->buflen = buflen;
    sceSifWriteBackDCache(&fs_send, sizeof(FsIoctlArg));
    if (sceSifCallRpc(&fs_client, FS_IOCTL2, 0, &fs_send, sizeof(FsIoctlArg), fs_rcv_rpc, 4, 0, 0) < 0) {
        DeleteSema(sema);
        _sceFsSigSema();
        return -SCE_EAGAIN;
    }
    rv = *(int *)UNCACHED(fs_rcv_rpc);
    _sceFsSigSema();
    if (rv == 0) {
        DeleteSema(sema);
        return -SCE_EAGAIN;
    }
    WaitSema(sema);
    DeleteSema(sema);
    return result;
}


int _sceCallCode(const char *name, int code)
{



    FsCallArg *arg = &fs_send.call;
    SemaParam param;
    int result;
    int len;
    int sema;
    int accepted;

    _sceFsWaitS(code);
    if (!fs_initialized) {
        sceFsInit();
    }
    for (len = 0; len < FS_NAME_MAX; len++) {
        if ((arg->name[len] = name[len]) == '\0') {
            break;
        }
    }
    if (len == FS_NAME_MAX) {
        arg->name[FS_NAME_MAX - 1] = '\0';
        len = FS_NAME_MAX - 1;
    }
    param.initCount = 0;
    param.maxCount = 1;
    param.option = 0;
    sema = CreateSema(&param);
    arg->semid = sema;
    arg->result = &result;
    arg->result_size = 4;
    if (sceSifCallRpc(&fs_client, code, 0, &fs_send, 12 + len + 1, fs_rcv_rpc, 4, 0, 0) < 0) {
        DeleteSema(sema);
        _sceFsSigSema();
        return -SCE_EAGAIN;
    }
    accepted = *(int *)UNCACHED(fs_rcv_rpc);
    _sceFsSigSema();
    if (accepted == 0) {
        DeleteSema(sema);
        return -SCE_EAGAIN;
    }
    WaitSema(sema);
    DeleteSema(sema);
    return result;
}


int sceRemove(const char *name)
{
    return _sceCallCode(name, 6);
}


int sceMkdir(const char *name, int mode)
{
    FsMkdirArg *arg = &fs_send.mkdir;
    SemaParam param;
    int result;
    int len;
    int sema;
    int accepted;

    _sceFsWaitS(FS_MKDIR);
    if (!fs_initialized) {
        sceFsInit();
    }
    for (len = 0; len < FS_NAME_MAX; len++) {
        if ((arg->name[len] = name[len]) == '\0') {
            break;
        }
    }
    if (len == FS_NAME_MAX) {
        arg->name[FS_NAME_MAX - 1] = '\0';
        len = FS_NAME_MAX - 1;
    }
    arg->mode = mode;
    param.initCount = 0;
    param.maxCount = 1;
    param.option = 0;
    sema = CreateSema(&param);
    arg->semid = sema;
    arg->result = &result;
    arg->result_size = 4;
    if (sceSifCallRpc(&fs_client, FS_MKDIR, 0, &fs_send, 16 + len + 1, fs_rcv_rpc, 4, 0, 0) < 0) {
        DeleteSema(sema);
        _sceFsSigSema();
        return -SCE_EAGAIN;
    }
    accepted = *(int *)UNCACHED(fs_rcv_rpc);
    _sceFsSigSema();
    if (accepted == 0) {
        DeleteSema(sema);
        return -SCE_EAGAIN;
    }
    WaitSema(sema);
    DeleteSema(sema);
    return result;
}

int sceRmdir(const char *name)
{
    return _sceCallCode(name, 8);
}


int sceFormat(const char *dev, const char *blockdev, void *argp, int arglen)
{
    FsFormatArg *arg = &fs_send.format;
    SemaParam param;
    int result;
    int len;
    int i;
    int sema;
    int accepted;

    _sceFsWaitS(FS_FORMAT);
    if (!fs_initialized) {
        sceFsInit();
    }
    for (len = 0; len < FS_NAME_MAX; len++) {
        if ((arg->dev[len] = dev[len]) == '\0') {
            break;
        }
    }
    if (len == FS_NAME_MAX) {
        arg->dev[FS_NAME_MAX - 1] = '\0';
    }
    if (blockdev == 0) {
        arg->blockdev[0] = '\0';
    } else {
        for (len = 0; len < FS_NAME_MAX; len++) {
            if ((arg->blockdev[len] = blockdev[len]) == '\0') {
                break;
            }
        }
        if (len == FS_NAME_MAX) {
            arg->blockdev[FS_NAME_MAX - 1] = '\0';
        }
    }
    if (arglen > FS_IOCTL_ARG_MAX) {
        _sceFsSigSema();
        return -SCE_E2BIG;
    }
    for (i = 0; i < arglen; i++) {
        arg->arg[i] = ((char *)argp)[i];
    }
    arg->arglen = arglen;
    param.initCount = 0;
    param.maxCount = 1;
    param.option = 0;
    sema = CreateSema(&param);
    arg->semid = sema;
    arg->result = &result;
    arg->result_size = 4;
    sceSifWriteBackDCache(&fs_send, sizeof(FsFormatArg));
    if (sceSifCallRpc(&fs_client, FS_FORMAT, 0, &fs_send, sizeof(FsFormatArg), fs_rcv_rpc, 4, 0, 0) < 0) {
        DeleteSema(sema);
        _sceFsSigSema();
        return -SCE_EAGAIN;
    }
    accepted = *(int *)UNCACHED(fs_rcv_rpc);
    _sceFsSigSema();
    if (accepted == 0) {
        DeleteSema(sema);
        return -SCE_EAGAIN;
    }
    WaitSema(sema);
    DeleteSema(sema);
    return result;
}


int sceAddDrv(void *device)
{
    FsAddDrvArg *arg = &fs_send.adddrv;
    SemaParam param;
    int result;
    int sema;
    int accepted;

    _sceFsWaitS(FS_ADDDRV);
    if (!fs_initialized) {
        sceFsInit();
    }
    arg->device = device;
    param.initCount = 0;
    param.maxCount = 1;
    param.option = 0;
    sema = CreateSema(&param);
    arg->semid = sema;
    arg->result = &result;
    arg->result_size = 4;
    if (sceSifCallRpc(&fs_client, FS_ADDDRV, 0, arg, sizeof(FsAddDrvArg), fs_rcv_rpc, 4, 0, 0) < 0) {
        DeleteSema(sema);
        _sceFsSigSema();
        return -1;
    }
    accepted = *(int *)UNCACHED(fs_rcv_rpc);
    _sceFsSigSema();
    if (accepted == 0) {
        DeleteSema(sema);
        return -1;
    }
    WaitSema(sema);
    DeleteSema(sema);
    return result;
}

int sceDelDrv(const char *name)
{
    return _sceCallCode(name, 0x10);
}


int sceDopen(const char *name)
{
    FsIob *iob;
    int fd;

    iob = new_iob();
    if (iob == 0) {
        return -SCE_ENODEV;
    }
    fd = _sceCallCode(name, 9);
    if (fd < 0) {
        WaitSema(fs_iob_sema);
        iob->flags = 0;
        SignalSema(fs_iob_sema);
        return fd;
    }
    WaitSema(fs_iob_sema);
    iob->fd = fd;
    fd = iob - fs_iob;
    SignalSema(fs_iob_sema);
    return fd;
}


int sceDclose(int fd)
{
    FsCloseArg *arg = &fs_send.close;
    SemaParam param;
    int result;
    int sema;
    int rv;
    FsIob *iob;

    iob = get_iob(fd);
    _sceFsWaitS(FS_DCLOSE);
    if (!fs_initialized) {
        _sceFsSigSema();
        return -1;
    }
    if (iob == 0 || iob->flags == 0) {
        _sceFsSigSema();
        return -SCE_EBADF;
    }
    arg->fd = iob->fd;
    param.initCount = 0;
    param.maxCount = 1;
    param.option = 0;
    sema = CreateSema(&param);
    arg->semid = sema;
    arg->result = &result;
    arg->result_size = 4;
    if (sceSifCallRpc(&fs_client, FS_DCLOSE, 0, arg, sizeof(FsCloseArg), fs_rcv_rpc, 4, 0, 0) < 0) {
        DeleteSema(sema);
        _sceFsSigSema();
        return -SCE_EAGAIN;
    }
    iob->flags = 0;
    rv = *(int *)UNCACHED(fs_rcv_rpc);
    _sceFsSigSema();
    if (rv == 0) {
        DeleteSema(sema);
        return -SCE_EAGAIN;
    }
    WaitSema(sema);
    DeleteSema(sema);
    if (result < 0) {
        return result;
    }
    return 0;
}


int sceDread(int fd, void *dirent)
{
    FsReadArg *arg = &fs_send.read;
    SemaParam param;
    int result;
    int sema;
    int rv;
    FsIob *iob;

    iob = get_iob(fd);
    _sceFsWaitS(FS_DREAD);
    if (!fs_initialized) {
        _sceFsSigSema();
        return -1;
    }
    if (iob == 0 || iob->flags == 0) {
        _sceFsSigSema();
        return -SCE_EBADF;
    }
    arg->fd = iob->fd;
    arg->buf = dirent;
    param.initCount = 0;
    param.maxCount = 1;
    param.option = 0;
    sema = CreateSema(&param);
    arg->semid = sema;
    arg->result = &result;
    arg->result_size = 4;
    if (sceSifCallRpc(&fs_client, FS_DREAD, 0, arg, sizeof(FsReadArg), fs_rcv_rpc, 4, 0, 0) < 0) {
        WaitSema(sema);
        _sceFsSigSema();
        return -SCE_EAGAIN;
    }
    rv = *(int *)UNCACHED(fs_rcv_rpc);
    _sceFsSigSema();
    if (rv == 0) {
        DeleteSema(sema);
        return -SCE_EAGAIN;
    }
    WaitSema(sema);
    DeleteSema(sema);
    return result;
}


int sceGetstat(const char *name, sce_stat *stat)
{
    FsGetstatArg *arg = &fs_send.getstat;
    SemaParam param;
    int result;
    int len;
    int sema;
    int accepted;

    _sceFsWaitS(FS_GETSTAT);
    if (!fs_initialized) {
        sceFsInit();
    }
    for (len = 0; len < FS_NAME_MAX; len++) {
        if ((arg->name[len] = name[len]) == '\0') {
            break;
        }
    }
    if (len == FS_NAME_MAX) {
        arg->name[FS_NAME_MAX - 1] = '\0';
        len = FS_NAME_MAX - 1;
    }
    arg->stat = stat;
    param.initCount = 0;
    param.maxCount = 1;
    param.option = 0;
    sema = CreateSema(&param);
    arg->semid = sema;
    arg->result = &result;
    arg->result_size = 4;
    if (sceSifCallRpc(&fs_client, FS_GETSTAT, 0, &fs_send, 16 + len + 1, fs_rcv_rpc, 4, 0, 0) < 0) {
        DeleteSema(sema);
        _sceFsSigSema();
        return -SCE_EAGAIN;
    }
    accepted = *(int *)UNCACHED(fs_rcv_rpc);
    _sceFsSigSema();
    if (accepted == 0) {
        DeleteSema(sema);
        return -SCE_EAGAIN;
    }
    WaitSema(sema);
    DeleteSema(sema);
    return result;
}


int sceChstat(const char *name, sce_stat *stat, unsigned int cbit)
{
    FsChstatArg *arg = &fs_send.chstat;
    SemaParam param;
    int result;
    int len;
    int sema;
    int accepted;

    _sceFsWaitS(FS_CHSTAT);
    if (!fs_initialized) {
        sceFsInit();
    }
    for (len = 0; len < FS_NAME_MAX; len++) {
        if ((arg->name[len] = name[len]) == '\0') {
            break;
        }
    }
    if (len == FS_NAME_MAX) {
        arg->name[FS_NAME_MAX - 1] = '\0';
        len = FS_NAME_MAX - 1;
    }
    arg->stat = *stat;
    arg->cbit = cbit;
    param.initCount = 0;
    param.maxCount = 1;
    param.option = 0;
    sema = CreateSema(&param);
    arg->semid = sema;
    arg->result = &result;
    arg->result_size = 4;
    sceSifWriteBackDCache(&fs_send, sizeof(FsChstatArg));
    if (sceSifCallRpc(&fs_client, FS_CHSTAT, 0, &fs_send, 0x50 + len + 1, fs_rcv_rpc, 4, 0, 0) < 0) {
        DeleteSema(sema);
        _sceFsSigSema();
        return -SCE_EAGAIN;
    }
    accepted = *(int *)UNCACHED(fs_rcv_rpc);
    _sceFsSigSema();
    if (accepted == 0) {
        DeleteSema(sema);
        return -SCE_EAGAIN;
    }
    WaitSema(sema);
    DeleteSema(sema);
    return result;
}


int sceRename(const char *oldname, const char *newname)
{
    FsRenameArg *arg = &fs_send.rename;
    SemaParam param;
    int result;
    int len;
    int sema;
    int accepted;

    _sceFsWaitS(FS_RENAME);
    if (!fs_initialized) {
        sceFsInit();
    }
    for (len = 0; len < FS_NAME_MAX; len++) {
        if ((arg->oldname[len] = oldname[len]) == '\0') {
            break;
        }
    }
    if (len == FS_NAME_MAX) {
        arg->oldname[FS_NAME_MAX - 1] = '\0';
    }
    for (len = 0; len < FS_NAME_MAX; len++) {
        if ((arg->newname[len] = newname[len]) == '\0') {
            break;
        }
    }
    if (len == FS_NAME_MAX) {
        arg->newname[FS_NAME_MAX - 1] = '\0';
    }
    param.initCount = 0;
    param.maxCount = 1;
    param.option = 0;
    sema = CreateSema(&param);
    arg->semid = sema;
    arg->result = &result;
    arg->result_size = 4;
    sceSifWriteBackDCache(&fs_send, sizeof(FsRenameArg));
    if (sceSifCallRpc(&fs_client, FS_RENAME, 0, &fs_send, sizeof(FsRenameArg), fs_rcv_rpc, 4, 0, 0) < 0) {
        DeleteSema(sema);
        _sceFsSigSema();
        return -SCE_EAGAIN;
    }
    accepted = *(int *)UNCACHED(fs_rcv_rpc);
    _sceFsSigSema();
    if (accepted == 0) {
        DeleteSema(sema);
        return -SCE_EAGAIN;
    }
    WaitSema(sema);
    DeleteSema(sema);
    return result;
}

int sceChdir(const char *name)
{
    return _sceCallCode(name, 0x12);
}


int sceSync(const char *name, int flag)
{
    FsSyncArg *arg = &fs_send.sync;
    SemaParam param;
    int result;
    int len;
    int sema;
    int accepted;

    _sceFsWaitS(FS_SYNC);
    if (!fs_initialized) {
        sceFsInit();
    }
    for (len = 0; len < FS_NAME_MAX; len++) {
        if ((arg->name[len] = name[len]) == '\0') {
            break;
        }
    }
    if (len == FS_NAME_MAX) {
        arg->name[FS_NAME_MAX - 1] = '\0';
    }
    arg->flag = flag;
    param.initCount = 0;
    param.maxCount = 1;
    param.option = 0;
    sema = CreateSema(&param);
    arg->semid = sema;
    arg->result = &result;
    arg->result_size = 4;
    if (sceSifCallRpc(&fs_client, FS_SYNC, 0, &fs_send, sizeof(FsSyncArg), fs_rcv_rpc, 4, 0, 0) < 0) {
        DeleteSema(sema);
        _sceFsSigSema();
        return -SCE_EAGAIN;
    }
    accepted = *(int *)UNCACHED(fs_rcv_rpc);
    _sceFsSigSema();
    if (accepted == 0) {
        DeleteSema(sema);
        return -SCE_EAGAIN;
    }
    WaitSema(sema);
    DeleteSema(sema);
    return result;
}


int sceMount(const char *fsname, const char *devname, int flag, void *argp, int arglen)
{
    FsMountArg *arg = &fs_send.mount;
    SemaParam param;
    int result;
    int len;
    int i;
    int sema;
    int accepted;

    _sceFsWaitS(FS_MOUNT);
    if (!fs_initialized) {
        sceFsInit();
    }
    for (len = 0; len < FS_NAME_MAX; len++) {
        if ((arg->fsname[len] = fsname[len]) == '\0') {
            break;
        }
    }
    if (len == FS_NAME_MAX) {
        arg->fsname[FS_NAME_MAX - 1] = '\0';
    }
    for (len = 0; len < FS_NAME_MAX; len++) {
        if ((arg->devname[len] = devname[len]) == '\0') {
            break;
        }
    }
    if (len == FS_NAME_MAX) {
        arg->devname[FS_NAME_MAX - 1] = '\0';
    }
    if (arglen > FS_IOCTL_ARG_MAX) {
        _sceFsSigSema();
        return -SCE_E2BIG;
    }
    for (i = 0; i < arglen; i++) {
        arg->arg[i] = ((char *)argp)[i];
    }
    arg->arglen = arglen;
    arg->flag = flag;
    param.initCount = 0;
    param.maxCount = 1;
    param.option = 0;
    sema = CreateSema(&param);
    arg->semid = sema;
    arg->result = &result;
    arg->result_size = 4;
    sceSifWriteBackDCache(&fs_send, sizeof(FsMountArg));
    if (sceSifCallRpc(&fs_client, FS_MOUNT, 0, &fs_send, sizeof(FsMountArg), fs_rcv_rpc, 4, 0, 0) < 0) {
        DeleteSema(sema);
        _sceFsSigSema();
        return -SCE_EAGAIN;
    }
    accepted = *(int *)UNCACHED(fs_rcv_rpc);
    _sceFsSigSema();
    if (accepted == 0) {
        DeleteSema(sema);
        return -SCE_EAGAIN;
    }
    WaitSema(sema);
    DeleteSema(sema);
    return result;
}

int sceUmount(const char *name)
{
    return _sceCallCode(name, 0x15);
}

long sceLseek64(int fd, long offset, int whence)
{
    FsLseek64Arg *arg = &fs_send.lseek64;
    SemaParam param;
    long result;
    int sema;
    int flags;
    int rv;
    int i;
    FsIob *iob;

    iob = get_iob(fd);
    _sceFsWaitS(FS_LSEEK64);
    if (!fs_initialized) {
        _sceFsSigSema();
        return -1;
    }
    if (iob == 0 || iob->flags == 0) {
        _sceFsSigSema();
        return -SCE_EBADF;
    }
    flags = iob->flags;
    arg->fd = iob->fd;
    arg->offset = offset;
    arg->whence = whence;
    arg->index = iob - fs_iob;
    param.initCount = 0;
    param.maxCount = 1;
    param.option = 0;
    sema = CreateSema(&param);
    arg->semid = sema;
    arg->result = &result;
    arg->result_size = 8;
    if (flags & FS_NOWAIT) {
        WaitSema(fs_io_sema);
        for (i = 0; i < FS_NQUEUE; i++) {
            if (_sceFs_q[i] == -1) {
                _sceFs_q[i] = arg->semid;
                arg->semid = -arg->semid;
                break;
            }
        }
        SignalSema(fs_io_sema);
    }
    if (sceSifCallRpc(&fs_client, FS_LSEEK64, 0, &fs_send, sizeof(FsLseek64Arg), fs_rcv_rpc, 4, 0, 0) < 0) {
        DeleteSema(sema);
        _sceFsSigSema();
        return -SCE_EAGAIN;
    }
    rv = *(int *)UNCACHED(fs_rcv_rpc);
    _sceFsSigSema();
    if (rv == 0) {
        DeleteSema(sema);
        return -SCE_EAGAIN;
    }
    if (flags & FS_NOWAIT) {
        DeleteSema(sema);
        return 0;
    }
    WaitSema(sema);
    DeleteSema(sema);
    return result;
}


int sceDevctl(const char *name, int cmd, void *argp, unsigned int arglen, void *bufp, unsigned int buflen)
{
    FsDevctlArg *arg = &fs_send.devctl;
    SemaParam param;
    int result;
    int len;
    unsigned int i;
    int sema;
    int accepted;

    _sceFsWaitS(FS_DEVCTL);
    if (!fs_initialized) {
        sceFsInit();
    }
    for (len = 0; len < FS_NAME_MAX; len++) {
        if ((arg->name[len] = name[len]) == '\0') {
            break;
        }
    }
    if (len == FS_NAME_MAX) {
        arg->name[FS_NAME_MAX - 1] = '\0';
    }
    if (arglen > FS_IOCTL_ARG_MAX || buflen > FS_IOCTL_ARG_MAX) {
        return -SCE_EINVAL;
    }
    for (i = 0; i < arglen; i++) {
        arg->arg[i] = ((char *)argp)[i];
    }
    arg->arglen = arglen;
    arg->cmd = cmd;
    param.initCount = 0;
    param.maxCount = 1;
    param.option = 0;
    sema = CreateSema(&param);
    arg->semid = sema;
    arg->result = &result;
    arg->result_size = 4;
    arg->buf = bufp;
    arg->buflen = buflen;
    sceSifWriteBackDCache(&fs_send, sizeof(FsDevctlArg));
    if (sceSifCallRpc(&fs_client, FS_DEVCTL, 0, &fs_send, sizeof(FsDevctlArg), fs_rcv_rpc, 4, 0, 0) < 0) {
        DeleteSema(sema);
        _sceFsSigSema();
        return -SCE_EAGAIN;
    }
    accepted = *(int *)UNCACHED(fs_rcv_rpc);
    _sceFsSigSema();
    if (accepted == 0) {
        DeleteSema(sema);
        return -SCE_EAGAIN;
    }
    WaitSema(sema);
    DeleteSema(sema);
    return result;
}


int sceSymlink(const char *existing, const char *newname)
{
    FsSymlinkArg *arg = &fs_send.symlink;
    SemaParam param;
    int result;
    int len;
    int sema;
    int accepted;

    _sceFsWaitS(FS_RENAME);
    if (!fs_initialized) {
        sceFsInit();
    }
    for (len = 0; len < FS_NAME_MAX; len++) {
        if ((arg->existing[len] = existing[len]) == ' ') {
            break;
        }
    }
    if (len == FS_NAME_MAX) {
        arg->existing[FS_NAME_MAX - 1] = ' ';
    }
    for (len = 0; len < FS_NAME_MAX; len++) {
        if ((arg->newname[len] = newname[len]) == ' ') {
            break;
        }
    }
    if (len == FS_NAME_MAX) {
        arg->newname[FS_NAME_MAX - 1] = ' ';
    }
    param.initCount = 0;
    param.maxCount = 1;
    param.option = 0;
    sema = CreateSema(&param);
    arg->semid = sema;
    arg->result = &result;
    arg->result_size = 4;
    if (sceSifCallRpc(&fs_client, FS_SYMLINK, 0, &fs_send, sizeof(FsSymlinkArg), fs_rcv_rpc, 4, 0, 0) < 0) {
        DeleteSema(sema);
        _sceFsSigSema();
        return -SCE_EAGAIN;
    }
    accepted = *(int *)UNCACHED(fs_rcv_rpc);
    _sceFsSigSema();
    if (accepted == 0) {
        DeleteSema(sema);
        return -SCE_EAGAIN;
    }
    WaitSema(sema);
    DeleteSema(sema);
    return result;
}


int sceReadlink(const char *name, char *buf, unsigned int buflen)
{
    FsReadlinkArg *arg = &fs_send.readlink;
    SemaParam param;
    int result;
    int len;
    int sema;
    int accepted;

    _sceFsWaitS(FS_RENAME);
    if (!fs_initialized) {
        sceFsInit();
    }
    for (len = 0; len < FS_NAME_MAX; len++) {
        if ((arg->name[len] = name[len]) == ' ') {
            break;
        }
    }
    if (len == FS_NAME_MAX) {
        arg->name[FS_NAME_MAX - 1] = ' ';
    }
    if (buflen >= FS_NAME_MAX) {
        buflen = FS_NAME_MAX - 1;
    }
    arg->buf = buf;
    arg->buflen = buflen;
    sceSifWriteBackDCache(buf, buflen);
    param.initCount = 0;
    param.maxCount = 1;
    param.option = 0;
    sema = CreateSema(&param);
    arg->semid = sema;
    arg->result = &result;
    arg->result_size = 4;
    if (sceSifCallRpc(&fs_client, FS_READLINK, 0, &fs_send, sizeof(FsSymlinkArg), fs_rcv_rpc, 4, 0, 0) < 0) {
        DeleteSema(sema);
        _sceFsSigSema();
        return -SCE_EAGAIN;
    }
    accepted = *(int *)UNCACHED(fs_rcv_rpc);
    _sceFsSigSema();
    if (accepted == 0) {
        DeleteSema(sema);
        return -SCE_EAGAIN;
    }
    WaitSema(sema);
    DeleteSema(sema);
    return result;
}
