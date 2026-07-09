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

/* One 8-byte sub-record; only its 0x4 pointer is fixed up on load. */
typedef struct NuTriggerSub {
    int pad0;   /* 0x00 */
    void *ptr;  /* 0x04 -- relocated */
} NuTriggerSub;

/* One trigger definition record (0x34 bytes), tiled in the loaded blob. */
typedef struct NuTriggerDef {
    char *name;              /* 0x00 -- relocated on load */
    char pad04[6];           /* 0x04 */
    unsigned char resetflag; /* 0x0A -- default flag byte */
    char pad0b[0x21];        /* 0x0B */
    short subcount;          /* 0x2C */
    char pad2e[2];           /* 0x2E */
    NuTriggerSub *subs;      /* 0x30 -- relocated on load */
} NuTriggerDef;

/* Header of the loaded trigger-def blob. */
typedef struct NuTriggerDefHdr {
    int version;         /* 0x00 -- must be 1 */
    int reloc;           /* 0x04 -- load delta */
    int count;           /* 0x08 -- number of NuTriggerDef records */
    NuTriggerDef *defs;  /* 0x0C -- the records */
} NuTriggerDefHdr;

/* Per-trigger runtime state (4 bytes). */
typedef struct NuTrigger {
    unsigned char pad0;  /* 0x00 */
    unsigned char pad1;  /* 0x01 */
    unsigned char flags; /* 0x02 */
    unsigned char state; /* 0x03 -- bit0 = pulled */
} NuTrigger;

/* One live trigger system. Carved (16-byte aligned, 0x18-byte header) out
 * of the caller's bump-heap and threaded onto the global list below. */
typedef struct NuTriggerSys {
    struct NuTriggerSys *next; /* 0x00 */
    struct NuTriggerSys *prev; /* 0x04 */
    NuTriggerDefHdr *def;      /* 0x08 -- the definition passed in a0 */
    NuTrigger *triggers;       /* 0x0C -- per-trigger array (def->count * 4) */
    void *data;                /* 0x10 -- opaque payload passed in a1 */
    int userflags;             /* 0x14 -- bit0 = disabled */
} NuTriggerSys;


typedef void (*NuErrorFunc)(const char *fmt, ...);

extern void *memset(void *s, int c, int n);
extern int strcasecmp(const char *a, const char *b);
extern int sprintf(char *buf, const char *fmt, ...);
extern int NuSceneInstanceRunScript(void *scene, void *script);
extern int NuFileLoadBuffer(void *name, void *buf, int size);
extern NuErrorFunc NuErrorProlog(const char *file, int line);

extern char D_00619EF8[];
extern char D_00619F20[];
extern char D_00619F60[];

extern NuTriggerSys *D_00633230;
#define g_NuTriggerSysList D_00633230


void *NuTriggerSysLoad(char *name, void **heap, void **heapend)
{
    NuTriggerDefHdr *blob;
    int size;
    int i;

    *heap = (void *)(((unsigned int)*heap + 0xF) & ~0xFU);
    size = NuFileLoadBuffer(name, *heap, (int)*heapend - (int)*heap);
    if (size == 0)
        return (void *)0;

    blob = (NuTriggerDefHdr *)*heap;
    *heap = (char *)*heap + size;

    if (blob->version != 1)
        NuErrorProlog(D_00619EF8, 0x87)(D_00619F20, name, blob->version);

    blob->reloc = (int)blob - blob->reloc;
    blob->defs = blob->defs ? (NuTriggerDef *)((char *)blob->defs + blob->reloc)
                            : (NuTriggerDef *)0;
    if (blob->defs == 0)
        return blob;

    for (i = 0; i < blob->count; i++) {
        NuTriggerDef *d = &blob->defs[i];
        int k;

        d->name = d->name ? (char *)(d->name + blob->reloc) : (char *)0;
        d->subs = d->subs ? (NuTriggerSub *)((char *)d->subs + blob->reloc)
                          : (NuTriggerSub *)0;
        if (d->subs == 0)
            continue;

        for (k = 0; k < d->subcount; k++) {
            NuTriggerSub *e = &d->subs[k];
            e->ptr = e->ptr ? (void *)((char *)e->ptr + blob->reloc) : (void *)0;
        }
    }

    return blob;
}


void NuTriggerSysInit(void)
{
    g_NuTriggerSysList = 0;
}


void NuTriggerSysRender(void)
{
}


NuTriggerSys *instNuTriggerSysCreate(NuTriggerDefHdr *def, void *data, void **heap)
{
    NuTriggerSys *sys = 0;
    int i;

    if (def == 0)
        goto out;
    if (data == 0)
        goto out;
    if (*(int *)((char *)data + 0x10) == 0)
        goto out;

    sys = (NuTriggerSys *)(((unsigned int)*heap + 0xF) & ~0xFU);
    *heap = (char *)sys + 0x18;
    memset(sys, 0, 0x18);

    sys->prev = 0;
    sys->next = g_NuTriggerSysList;
    if (g_NuTriggerSysList)
        g_NuTriggerSysList->prev = sys;
    g_NuTriggerSysList = sys;

    sys->data = data;
    sys->def = def;
    sys->triggers = (NuTrigger *)*heap;
    *heap = (char *)*heap + def->count * 4;
    memset(sys->triggers, 0, def->count * 4);

    for (i = 0; i < def->count; i++)
        sys->triggers[i].flags = def->defs[i].resetflag;

out:
    return sys;
}


void instNuTriggerSysDestroy(NuTriggerSys *sys)
{
    if (sys->next)
        sys->next->prev = sys->prev;
    if (sys->prev)
        sys->prev->next = sys->next;
    else
        g_NuTriggerSysList = sys->next;
}


void instNuTriggerSetFlag(NuTriggerSys *sys, char *name, int index, int set)
{
    NuTriggerDefHdr *def = sys->def;
    int i;

    for (i = 0; i < def->count; i++) {
        if (strcasecmp(def->defs[i].name, name) == 0) {
            char bit = 1 << (index - 1);
            if (set)
                sys->triggers[i].flags |= bit;
            else
                sys->triggers[i].flags &= ~bit;
            break;
        }
    }
}


void instNuTriggerReset(NuTriggerSys *sys, char *name)
{
    NuTriggerDefHdr *def = sys->def;
    int i;

    for (i = 0; i < def->count; i++) {
        if (strcasecmp(def->defs[i].name, name) == 0) {
            sys->triggers[i].flags = def->defs[i].resetflag;
            sys->triggers[i].state &= 0xFE;
            break;
        }
    }
}


void instNuTriggerEnable(NuTriggerSys *sys)
{
    sys->userflags &= ~1;
}


void instNuTriggerDisable(NuTriggerSys *sys)
{
    sys->userflags |= 1;
}


void NuTriggerPull(NuTriggerSys *sys, NuTriggerDef *def, NuTrigger *trig, int pull)
{
    char buf[32];

    if (pull) {
        NuSceneInstanceRunScript(sys->data, def->name);
        trig->state |= 1;
    } else {
        sprintf(buf, D_00619F60, def->name);
        NuSceneInstanceRunScript(sys->data, buf);
        trig->state &= 0xFE;
    }
}
