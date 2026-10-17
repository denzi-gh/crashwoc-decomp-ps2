/*
 * Unit: sdk/sce/graphdev_131
 *
 * Functions:
 *   0x00261bb0 sceGsSyncVCallback
 */

typedef int (*sceGsVSyncFunc)(int);

typedef struct {
    short sceGsInterMode;
    short sceGsOutMode;
    short sceGsFFMode;
    unsigned short sceGsVersion;
    sceGsVSyncFunc sceGsVSCfunc;
    int sceGsVSCid;
} sceGsGParam;

#define INTC_VBLANK_S 2

sceGsGParam *sceGsGetGParam(void);
int DisableIntc(int cause);
int EnableIntc(int cause);
int AddIntcHandler(int cause, sceGsVSyncFunc handler, int next);
int RemoveIntcHandler(int cause, int handler_id);


sceGsVSyncFunc sceGsSyncVCallback(sceGsVSyncFunc func)
{
    sceGsGParam *gp = sceGsGetGParam();
    sceGsVSyncFunc old = gp->sceGsVSCfunc;

    if (func == 0) {
        DisableIntc(INTC_VBLANK_S);
        RemoveIntcHandler(INTC_VBLANK_S, gp->sceGsVSCid);
        gp->sceGsVSCid = 0;
        gp->sceGsVSCfunc = 0;
    } else {
        if (old != 0) {
            DisableIntc(INTC_VBLANK_S);
            RemoveIntcHandler(INTC_VBLANK_S, gp->sceGsVSCid);
        }
        gp->sceGsVSCfunc = func;
        gp->sceGsVSCid = AddIntcHandler(INTC_VBLANK_S, func, -1);
        EnableIntc(INTC_VBLANK_S);
    }
    return old;
}
