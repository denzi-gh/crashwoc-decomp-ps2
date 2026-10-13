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

extern int D_00613400[];
extern int D_00613404[];
extern int D_00613408[];
extern int D_0061340C[];
extern FsIob D_00706280[];
extern char D_007064A8[];

#define fs_initialized  D_00613400[0]
#define fs_sema         D_00613404[0]
#define fs_iob_sema     D_00613408[0]
#define fs_io_sema      D_0061340C[0]
#define fs_iob          D_00706280
#define fs_iop_version  D_007064A8

extern int CreateSema(SemaParam *param);
extern int WaitSema(int sema);
extern int SignalSema(int sema);
extern void *memset(void *dest, int c, unsigned int n);
extern int _sceCallCode(const char *name, int code);


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


int _sceFsWaitS(void)
{
    _sceFsSemInit();
    WaitSema(fs_sema);
    return 0;
}


void _sceFsSigSema(void)
{
    SignalSema(fs_sema);
}


int sceFsReset(void)
{
    fs_initialized = 0;
    memset(fs_iop_version, 0, 4);
    return 0;
}


int sceRemove(const char *name)
{
    return _sceCallCode(name, 6);
}


int sceRmdir(const char *name)
{
    return _sceCallCode(name, 8);
}


int sceDelDrv(const char *name)
{
    return _sceCallCode(name, 0x10);
}


int sceChdir(const char *name)
{
    return _sceCallCode(name, 0x12);
}


int sceUmount(const char *name)
{
    return _sceCallCode(name, 0x15);
}
