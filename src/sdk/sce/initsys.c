/*
 * Unit: sdk/sce/initsys
 *
 * Functions:
 *   0x00280bb0 supplement_crt0
 *   0x00280bf8 _InitSys
 */

typedef struct SemaParam {
    int currentCount;
    int maxCount;
    int initCount;
    int numWaitThreads;
    unsigned int attr;
    unsigned int option;
} SemaParam;

extern int __sce_sema_id[];
extern int __sce_eh_sema_id[];

extern int CreateSema(SemaParam *param);
extern void InitAlarm(void);
extern int InitThread(void);

void supplement_crt0(void)
{
    SemaParam sema;
    SemaParam eh_sema;

    sema.maxCount = 1;
    sema.initCount = 1;
    eh_sema.maxCount = 1;
    eh_sema.initCount = 1;
    __sce_sema_id[0] = CreateSema(&sema);
    __sce_eh_sema_id[0] = CreateSema(&eh_sema);
}


void _InitSys(void)
{
    supplement_crt0();
    InitAlarm();
    InitThread();
}
