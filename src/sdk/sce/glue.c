/*
 * Unit: sdk/sce/glue
 *
 * Functions:
 *   0x00279110 sceResetttyinit
 *   0x00279120 VSync
 *   0x00279160 VSync2
 *   0x002791c0 write
 *   0x00279240 read
 *   0x002792b8 open
 *   0x002792e0 close
 *   0x002792e8 ioctl
 *   0x002792f0 lseek
 *   0x002792f8 sbrk
 *   0x002793a8 isatty
 *   0x002793b0 fstat
 *   0x002793c8 getpid
 *   0x002793d0 kill
 *   0x002793f8 stat
 *   0x00279420 unlink
 */


#define EIO 5
#define ENOMEM 12
#define S_IFCHR 0x2000
#define STATUS_EIE 0x10000

#define INTC_STAT (*(volatile int *)0x1000F000)
#define INTC_STAT_SAMPLE (*(int *)0x1000F000)
#define INTC_VBLANK_START 4

struct stat {
    short st_dev;
    short st_ino;
    unsigned int st_mode;
    short st_nlink;
    short st_uid;
    short st_gid;
    short st_rdev;
    long st_size;
    long st_atime;
    long st_spare1;
    long st_mtime;
    long st_spare2;
    long st_ctime;
    long st_spare3;
    long st_blksize;
    long st_blocks;
};

extern int D_00612BB8[];
extern char *D_00612BBC[];

#define tty_initialized D_00612BB8
#define heap_ptr        (D_00612BBC[0])

extern int *__errno(void);
extern void *EndOfHeap(void);
extern void Exit(int status);
extern void SetVSyncFlag(volatile int *flag, volatile long *count);
extern int sceTtyInit(void);
extern int sceTtyWrite(const char *buf, int len);
extern int sceTtyRead(char *buf, int len);


void sceResetttyinit(void) {
    tty_initialized[0] = 0;
}


void VSync(void) {
    INTC_STAT = INTC_VBLANK_START;
wait:
    if (!(INTC_STAT_SAMPLE & INTC_VBLANK_START)) {
        goto wait;
    }
    INTC_STAT = INTC_VBLANK_START;
}


long VSync2(void) {
    volatile int vsync_seen;
    volatile long vsync_count;

    vsync_seen = 0;
    SetVSyncFlag(&vsync_seen, &vsync_count);
    INTC_STAT = INTC_VBLANK_START;
wait:
    if (!(INTC_STAT_SAMPLE & INTC_VBLANK_START) && vsync_seen == 0) {
        goto wait;
    }
    INTC_STAT = INTC_VBLANK_START;
    return vsync_count;
}


int write(int fd, const char *buf, int len) {
    if (fd == 1 || fd == 2) {
        if (!tty_initialized[0]) {
            if (!sceTtyInit()) {
                return -1;
            }
            tty_initialized[0] = 1;
        }
        return sceTtyWrite(buf, len);
    }
    return -1;
}


int read(int fd, char *buf, int len) {
    if (fd == 0) {
        if (!tty_initialized[0]) {
            if (!sceTtyInit()) {
                return -1;
            }
            tty_initialized[0] = 1;
        }
        return sceTtyRead(buf, len);
    }
    return -1;
}


int open(const char *path, int flags) {
    *__errno() = EIO;
    return -1;
}


int close(int fd) {
    return -1;
}


int ioctl(int fd, int request, void *arg) {
    return -1;
}


int lseek(int fd, int offset, int whence) {
    return -1;
}


void *sbrk(int incr) {
    int intr;
    int status;
    char *mp;
    char *ret;

    __asm__ volatile ("mfc0 %0, $12" : "=r"(intr));
    intr &= STATUS_EIE;
    if (intr) {
        do {
            __asm__ volatile ("di");
            __asm__ volatile ("sync.p");
            __asm__ volatile ("mfc0 %0, $12" : "=r"(status));
        } while (status & STATUS_EIE);
    }
    mp = heap_ptr + incr;
    if (mp > (char *)EndOfHeap()) {
        *__errno() = ENOMEM;
        if (intr) {
            __asm__ volatile ("ei");
        }
        return (void *)-1;
    }
    ret = heap_ptr;
    heap_ptr = mp;
    if (intr) {
        __asm__ volatile ("ei");
    }
    return ret;
}


int isatty(int fd) {
    return 1;
}


int fstat(int fd, struct stat *buf) {
    buf->st_mode = S_IFCHR;
    buf->st_blksize = 0;
    return 0;
}


int getpid(void) {
    return 1;
}


int kill(int pid, int sig) {
    if (pid == 1) {
        Exit(sig);
    }
    return 0;
}


int stat(const char *path, struct stat *buf) {
    *__errno() = EIO;
    return -1;
}


int unlink(const char *path) {
    *__errno() = EIO;
    return -1;
}
