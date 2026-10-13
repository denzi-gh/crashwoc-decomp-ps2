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
#define S_IFCHR 0x2000

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

#define tty_initialized D_00612BB8

extern int *__errno(void);
extern void Exit(int status);


void sceResetttyinit(void) {
    tty_initialized[0] = 0;
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
