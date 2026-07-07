/*
 * Unit: gamelib/trigger
 *
 * Functions:
 *   0x001af0f8 NuTriggerSysLoad
 *   0x001af278 CheckParentedTriggerWithPos
 *   0x001af5f8 CheckUnparentedTriggerWithPos
 *   0x001af8a8 NuTriggerSysUpdate
 *   0x001afaa8 WeaponCheckTriggers
 *   0x001afc78 DebugRenderTriggers
 *   0x001b0c38 NuTriggerSysInit
 *   0x001b0c40 NuTriggerSysRender
 *   0x001b0c48 instNuTriggerSysCreate
 *   0x001b0d70 instNuTriggerSysDestroy
 *   0x001b0da0 instNuTriggerSetFlag
 *   0x001b0e90 instNuTriggerReset
 *   0x001b0f48 instNuTriggerEnable
 *   0x001b0f60 instNuTriggerDisable
 *   0x001b0f70 NuTriggerPull
 */

/* One live trigger system. Carved (16-byte aligned, 0x18-byte header) out
 * of the caller's bump-heap and threaded onto the global list below. */
typedef struct NuTriggerSys {
    struct NuTriggerSys *next; /* 0x00 */
    struct NuTriggerSys *prev; /* 0x04 */
    void *def;                 /* 0x08 -- the definition passed in a0 */
    void *triggers;            /* 0x0C -- per-trigger array (def->count * 4) */
    void *data;                /* 0x10 -- opaque payload passed in a1 */
} NuTriggerSys;


extern NuTriggerSys *g_NuTriggerSysList;


void *instNuTriggerSysCreate(void *def, void *data, void **heap)
{
    NuTriggerSys *sys = 0;

    if (def == 0)
        return sys;
    if (data == 0)
        return sys;
    if (*(int *)((char *)data + 0x10) == 0)
        return sys;

    return sys;
}
