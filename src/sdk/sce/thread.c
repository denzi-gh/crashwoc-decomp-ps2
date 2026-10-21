/*
 * Unit: sdk/sce/thread
 *
 * Functions:
 *   0x00279a28 topThread
 *   0x00279b00 InitThread
 *   0x00279bd8 iWakeupThread
 *   0x00279c70 iRotateThreadReadyQueue
 *   0x00279cf0 iSuspendThread
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

typedef struct TopRequest {
    unsigned char mode;
    unsigned char data;
} TopRequest;

typedef struct TopArg {
    int readPos;
    int writePos;
    TopRequest request[0x200];
} TopArg;

enum {
    TOP_WAKEUP,
    TOP_ROTATE,
    TOP_SUSPEND
};

extern char _gp[];

static int topId = 0;
static int topSema;
static TopArg topArg;

extern int CreateThread(ThreadParam *param);
extern int StartThread(int tid, void *arg);
extern int GetThreadId(void);
extern int ChangeThreadPriority(int tid, int priority);
extern int RotateThreadReadyQueue(int priority);
extern int WakeupThread(int tid);
extern int SuspendThread(int tid);
extern int _iWakeupThread(int tid);
extern int _iSuspendThread(int tid);
extern int CreateSema(SemaParam *param);
extern int DeleteSema(int sema);
extern int WaitSema(int sema);
extern int iSignalSema(int sema);
extern int kprintf(const char *fmt, ...);


static inline int iGetThreadId(void)
{
    int tid;

    __asm__ volatile ("addiu $3, $0, -47\n\tsyscall\n\tdaddu %0, $2, $0"
                      : "=r"(tid) : : "$2", "$3");
    return tid;
}


static void topThread(void *arg)
{
    TopArg *top = arg;
    int pos;

    while (1) {
        WaitSema(topSema);
        pos = top->readPos & 0x1FF;
        top->readPos = pos + 1;
        switch (top->request[pos].mode) {
        case TOP_WAKEUP:
            WakeupThread(top->request[pos].data);
            break;
        case TOP_ROTATE:
            RotateThreadReadyQueue(top->request[pos].data);
            break;
        case TOP_SUSPEND:
            SuspendThread(top->request[pos].data);
            break;
        default:
            kprintf("## internel error in libkernl.a!\n");
            break;
        }
    }
}


int InitThread(void)
{
    static char stack[0x400];
    ThreadParam thread;
    SemaParam sema;

    if (topId > 0) {
        return -1;
    }
    sema.maxCount = 0xFF;
    sema.initCount = 0;
    topSema = CreateSema(&sema);
    if (topSema < 0) {
        return -1;
    }
    thread.entry = topThread;
    thread.stack = stack;
    thread.stackSize = 0x400;
    thread.gpReg = _gp;
    thread.initPriority = 0;
    topId = CreateThread(&thread);
    if (topId < 0) {
        DeleteSema(topSema);
        return -1;
    }
    topArg.readPos = 0;
    topArg.writePos = 0;
    StartThread(topId, &topArg);
    ChangeThreadPriority(GetThreadId(), 1);
    return topId;
}


int iWakeupThread(int tid)
{
    int pos;

    if (iGetThreadId() != tid) {
        return _iWakeupThread(tid);
    }
    if (tid < 0 || tid >= 256 || topId == 0) {
        return -1;
    }
    pos = topArg.writePos & 0x1FF;
    topArg.writePos = pos + 1;
    topArg.request[pos].mode = TOP_WAKEUP;
    topArg.request[pos].data = tid;
    iSignalSema(topSema);
    return tid;
}


int iRotateThreadReadyQueue(int priority)
{
    int pos;

    if (priority < 0 || priority >= 128 || topId == 0) {
        return -1;
    }
    pos = topArg.writePos & 0x1FF;
    topArg.writePos = pos + 1;
    topArg.request[pos].mode = TOP_ROTATE;
    topArg.request[pos].data = priority;
    iSignalSema(topSema);
    return priority;
}


int iSuspendThread(int tid)
{
    int pos;

    if (iGetThreadId() != tid) {
        return _iSuspendThread(tid);
    }
    if (tid < 0 || tid >= 256 || topId == 0) {
        return -1;
    }
    pos = topArg.writePos & 0x1FF;
    topArg.writePos = pos + 1;
    topArg.request[pos].mode = TOP_SUSPEND;
    topArg.request[pos].data = tid;
    iSignalSema(topSema);
    return tid;
}
