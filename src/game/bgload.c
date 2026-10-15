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

extern void NuIntAddVBlankS(void *handler);
extern void NuPs2ScreenSwap(s32 mode);

/* Retail-owned data (data-from-C unsupported -> extern). */
extern volatile s32 bgload_complete; /* written by the load thread */
extern s32 loadthread_id;
extern s32 D_00633380;              /* main thread id */
extern s32 D_00633384;              /* vblank semaphore id */
extern struct ThreadParam th_param;
extern struct SemaParam sem_param;
extern u8 D_0060AF00[];             /* vblank handler record */
extern u8 D_006B7680[];             /* load thread stack (0x48000) */
extern char D_00628798[];           /* __FILE__ */
extern char D_006287A8[];           /* thread-started message */
extern char D_006287C8[];           /* thread-ended message */
extern int _gp;

void BGSuspendMain(void) {
    if (loadthread_id != 0) {
        ResumeThread(loadthread_id);
    }
    if (D_00633384 != 0) {
        while (PollSema(D_00633384) == D_00633384) {
        }
        if (D_00633384 != 0) {
            WaitSema(D_00633384);
        }
    }
}

void BGLoadInit(void) {
    s32 id;

    id = GetThreadId();
    D_00633380 = id;
    ChangeThreadPriority(id, 1);
    sem_param.max_count = 1;
    sem_param.init_count = 0;
    sem_param.option = 0;
    D_00633384 = CreateSema(&sem_param);
    NuIntAddVBlankS(D_0060AF00);
}

void BGLoadStart(void (*func)(void *)) {
    th_param.stack = D_006B7680;
    th_param.stackSize = 0x48000;
    th_param.initPriority = 2;
    th_param.gpReg = &_gp;
    th_param.entry = func;
    th_param.option = 0;
    bgload_complete = 0;
    loadthread_id = CreateThread(&th_param);
    StartThread(loadthread_id, 0);
    NuDebugMsgProlog(D_00628798, 0x7C)(D_006287A8);
}

void BGLoadEnd(void) {
    s32 id;

    id = loadthread_id;
    loadthread_id = 0;
    TerminateThread(id);
    DeleteThread(id);
    NuDebugMsgProlog(D_00628798, 0x89)(D_006287C8);
}

void VBlankFn(void) {
    if (D_00633384 != 0) {
        iSignalSema(D_00633384);
    }
    if (loadthread_id != 0) {
        iSuspendThread(loadthread_id);
        if (loadthread_id != 0) {
            NuPs2ScreenSwap(1);
        }
    }
}
