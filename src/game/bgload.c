/*
 * Unit: game/bgload
 *
 * Functions:
 *   0x0025b668 BGSuspendMain
 *   0x0025b6c0 BGLoadInit
 *   0x0025b720 BGLoadStart
 *   0x0025b7b0 BGLoadEnd
 *   0x0025b800 VBlankFn
 */

#include "nu_types.h"

#include "nucore/nuerror.h"
#include "nups2/ps2video.h"

/* Sony EE kernel parameter blocks (offsets verified against the retail
 * stores in BGLoadInit / BGLoadStart). */
struct ThreadParam {
    s32 status;           /* 0x00 */
    void (*entry)(void *);/* 0x04 */
    void *stack;          /* 0x08 */
    s32 stackSize;        /* 0x0C */
    void *gpReg;          /* 0x10 */
    s32 initPriority;     /* 0x14 */
    s32 currentPriority;  /* 0x18 */
    u32 attr;             /* 0x1C */
    u32 option;           /* 0x20 */
    s32 waitType;         /* 0x24 */
    s32 waitId;           /* 0x28 */
    s32 wakeupCount;      /* 0x2C */
};

struct SemaParam {
    s32 count;            /* 0x00 */
    s32 max_count;        /* 0x04 */
    s32 init_count;       /* 0x08 */
    s32 wait_threads;     /* 0x0C */
    u32 attr;             /* 0x10 */
    u32 option;           /* 0x14 */
};

extern s32 GetThreadId(void);
extern s32 ChangeThreadPriority(s32 thid, s32 prio);
extern s32 CreateThread(struct ThreadParam *param);
extern s32 StartThread(s32 thid, void *arg);
extern s32 ResumeThread(s32 thid);
extern s32 TerminateThread(s32 thid);
extern s32 DeleteThread(s32 thid);
extern s32 iSuspendThread(s32 thid);
extern s32 CreateSema(struct SemaParam *param);
extern s32 PollSema(s32 sema_id);
extern s32 WaitSema(s32 sema_id);
extern s32 iSignalSema(s32 sema_id);

extern void NuPs2ScreenSwap(s32 mode);

static s32 thread_id;
static s32 sem_id;
static u8 loadstackmem[0x48000];
extern int _gp;

void VBlankFn(void);

static struct NuVBlankHandler vblank_handler = { 0, 0, VBlankFn, 0 };
struct ThreadParam th_param = { 0 };
struct SemaParam sem_param = { 0 };
volatile s32 bgload_complete = 0; /* written by the load thread */
s32 loadthread_id = 0;

void BGSuspendMain(void) {
    if (loadthread_id != 0) {
        ResumeThread(loadthread_id);
    }
    if (sem_id != 0) {
        while (PollSema(sem_id) == sem_id) {
        }
        if (sem_id != 0) {
            WaitSema(sem_id);
        }
    }
}

void BGLoadInit(void) {
    s32 id;

    id = GetThreadId();
    thread_id = id;
    ChangeThreadPriority(id, 1);
    sem_param.max_count = 1;
    sem_param.init_count = 0;
    sem_param.option = 0;
    sem_id = CreateSema(&sem_param);
    NuIntAddVBlankS(&vblank_handler);
}

void BGLoadStart(void (*func)(void *)) {
    th_param.stack = loadstackmem;
    th_param.stackSize = 0x48000;
    th_param.initPriority = 2;
    th_param.gpReg = &_gp;
    th_param.entry = func;
    th_param.option = 0;
    bgload_complete = 0;
    loadthread_id = CreateThread(&th_param);
    StartThread(loadthread_id, 0);
    NuDebugMsgProlog(".\\bgload.c", 0x7C)("Background Load Thread Created");
}

void BGLoadEnd(void) {
    s32 id;

    id = loadthread_id;
    loadthread_id = 0;
    TerminateThread(id);
    DeleteThread(id);
    NuDebugMsgProlog(".\\bgload.c", 0x89)("Background Load Thread Destroyed");
}

void VBlankFn(void) {
    if (sem_id != 0) {
        iSignalSema(sem_id);
    }
    if (loadthread_id != 0) {
        iSuspendThread(loadthread_id);
        if (loadthread_id != 0) {
            NuPs2ScreenSwap(1);
        }
    }
}
