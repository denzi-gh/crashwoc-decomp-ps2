/*
 * Unit: nu3d/numtl
 *
 * Functions:
 *   0x0011a900 CreateStateBlock
 *   0x0011ace8 NuMtlAnimate
 *   0x0011aff0 NuMtlBuildRenderList
 *   0x0011b1a0 NuMtlCreateDefault
 *   0x0011b2c8 NuMtlAlloc
 *   0x0011b3c8 NuMtlCreate
 *   0x0011b550 NuMtlRead
 *   0x0011bae8 NuMtlUpdateEx
 *   0x0011be38 NuMtlValid
 *   0x0011be50 NuMtlInit
 *   0x0011be98 NuMtlGetSysBuffSize
 *   0x0011bea8 NuMtlInitEx
 *   0x0011bf80 NuMtlClose
 *   0x0011c008 NuMtlRelease
 *   0x0011c080 NuMtlDestroy
 *   0x0011c130 NuMtlUpdate
 *   0x0011c150 NuMtlWrite
 *   0x0011c170 NuMtlReadEventSetHandler
 *   0x0011c198 NuMtlGetStateBlockDma
 *   0x0011c1a0 NuMtlGetZupd
 *   0x0011c1b8 NuMtlAdd
 *   0x0011c2c0 NuMtlIterate
 *   0x0011c300 NuMtlRemove
 *   0x0011c358 NuMtlReadEventDefault
 *   0x0011c360 NuMtlInsert
 *   0x0011c430 NuMtlUpdateAll
 *   0x0011c4b0 NuMtlDisplayMtl
 */

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef struct NuMtl {
    u8 unk000[0x120];
    u64 flags;
    u8 unk128[0x38];
    struct NuMtl *next;
    struct NuMtl *prev;
    struct NuMtl *display_next;
    u8 unk16C[0x38];
    int tid;
    int alpha;
    u8 unk1AC[0x24];
} NuMtl;

typedef struct NuMtlRect {
    float x;
    float y;
    float w;
    float h;
    float z;
    float u0;
    float v0;
    float u1;
    float v1;
    NuMtl *mtl;
} NuMtlRect;

typedef int (*NuMtlReadEventFunc)(NuMtl *mtl);
typedef void (*NuErrorFunc)(const char *fmt, ...);

#define NUMTL_VALID 0x01
#define NUMTL_POOLED 0x02
#define NUMTL_ALLOCATED 0x04

extern int NuMtlReadEventDefault(NuMtl *mtl);
extern void NuMtlCreateDefault(void);
extern int NuMtlUpdateEx(NuMtl *mtl, int force);
extern void NuMtlClose(void);
extern void NuMtlDestroy(NuMtl *mtl);
extern void NuMtlRemove(NuMtl *mtl);
extern NuMtlReadEventFunc NuMtlReadEventSetHandler(NuMtlReadEventFunc handler);

extern void *memset(void *dst, int value, unsigned int size);
extern void *NuMemAllocFn(int size, char *file, int line);
extern void NuMemFreeFn(void *ptr, char *file, int line);
extern NuErrorFunc NuErrorProlog(const char *file, int line);
extern NuErrorFunc NuDebugMsgProlog(const char *file, int line);
extern int NuFileWrite(int fh, void *data, int size);
extern short NuTexUnRef(int tid);
extern void NuDisableVBlank(void);
extern void NuEnableVBlank(void);
extern int VU1_DmaGetStatus(void);
extern void NuRndrRect(NuMtlRect *rect);
extern void NuFntSet(void *font);
extern void NuFntScale(int x, int y);
extern void NuFntPrintEx(int x, int y, int colour, const char *fmt, ...);

extern NuMtlReadEventFunc NuMtlReadEvent;
extern void *system_font;

extern char D_00614470[];
#define numtl_file D_00614470

extern char D_00614508[];
#define msg_release_free D_00614508

extern char D_00614540[];
#define msg_update_all_dangerous D_00614540

extern char D_00614568[];
#define fmt_display_mtl D_00614568

extern int D_0062EBA0;
#define mtl_initialised D_0062EBA0

extern NuMtl *D_0062EBA4;
#define mtl_list D_0062EBA4

extern NuMtl *D_0062EBA8;
#define mtl_pool D_0062EBA8

extern int D_0062EBAC;
#define mtl_pool_last D_0062EBAC

extern int D_0062EBB0;
#define mtl_pool_count D_0062EBB0

extern NuMtl *D_0062EBDC;
#define mtl_iter D_0062EBDC


NuMtl *NuMtlAlloc(void) {
    NuMtl *mtl;
    int i;

    mtl = 0;
    if (mtl_pool_count) {
        i = mtl_pool_last + 1;
        if (i >= mtl_pool_count) {
            i = 0;
        }
        while (i != mtl_pool_last) {
            if (!(mtl_pool[i].flags & NUMTL_VALID)) {
                mtl = &mtl_pool[i];
                memset(mtl, 0, sizeof(NuMtl));
                mtl->flags |= NUMTL_VALID;
                mtl->flags |= NUMTL_POOLED;
                mtl_pool_last = i;
                break;
            }
            i++;
            if (i >= mtl_pool_count) {
                i = 0;
            }
        }
    } else {
        mtl = NuMemAllocFn(sizeof(NuMtl), numtl_file, 0x398);
        memset(mtl, 0, sizeof(NuMtl));
        mtl->flags |= NUMTL_VALID;
        mtl->flags |= NUMTL_ALLOCATED;
    }
    return mtl;
}


int NuMtlValid(NuMtl *mtl) {
    u64 flags;

    flags = mtl->flags;
    return -((u8)flags & NUMTL_VALID);
}


void NuMtlInit(void) {
    if (mtl_initialised) {
        NuMtlClose();
    }
    mtl_list = 0;
    mtl_initialised = 1;
    mtl_pool_count = 0;
    mtl_pool = 0;
    NuMtlCreateDefault();
    NuMtlReadEventSetHandler(0);
}


int NuMtlGetSysBuffSize(int count) {
    return count * sizeof(NuMtl);
}


void NuMtlInitEx(void **buff, int count) {
    NuMtl *pool;
    int i;

    if (mtl_initialised) {
        NuMtlClose();
    }
    mtl_list = 0;
    mtl_initialised = 1;
    mtl_pool_count = 0;
    mtl_pool = 0;
    NuMtlCreateDefault();
    NuMtlReadEventSetHandler(0);
    pool = (NuMtl *)(((u32)*buff + 0xF) & 0xFFFFFFF0);
    mtl_pool_count = count;
    mtl_pool_last = 0;
    *buff = pool;
    mtl_pool = pool;
    *buff = pool + count;
    memset(pool, 0, count * sizeof(NuMtl));
    for (i = 0; i < mtl_pool_count; i++) {
        mtl_pool[i].flags |= NUMTL_POOLED;
    }
    NuMtlCreateDefault();
}


void NuMtlClose(void) {
    NuMtl *mtl;
    NuMtl *next;
    int i;

    if (mtl_initialised) {
        mtl = mtl_list;
        if (mtl_pool) {
            for (i = 0; i < mtl_pool_count; i++) {
                mtl_pool[i].flags &= ~NUMTL_VALID;
            }
            mtl_list = 0;
        } else {
            for (; mtl; mtl = next) {
                next = mtl->next;
                NuMtlDestroy(mtl);
            }
        }
        NuMtlCreateDefault();
        mtl_initialised = 0;
    }
}


void NuMtlRelease(void) {
    NuMtl *mtl;
    NuMtl *next;
    int i;

    mtl = mtl_list;
    if (mtl_pool) {
        for (i = 0; i < mtl_pool_count; i++) {
            mtl_pool[i].flags &= ~NUMTL_VALID;
        }
        mtl_list = 0;
    } else {
        for (; mtl; mtl = next) {
            next = mtl->next;
            NuMtlDestroy(mtl);
        }
    }
    NuMtlCreateDefault();
}


void NuMtlDestroy(NuMtl *mtl) {
    if (mtl->tid) {
        NuTexUnRef(mtl->tid);
    }
    NuMtlRemove(mtl);
    if (mtl->flags & NUMTL_POOLED) {
        if (!(mtl->flags & NUMTL_VALID)) {
            NuDebugMsgProlog(numtl_file, 0x3EC)(msg_release_free);
        }
        mtl->flags &= ~NUMTL_VALID;
    } else {
        mtl->flags &= ~NUMTL_VALID;
        if (mtl->flags & NUMTL_ALLOCATED) {
            NuMemFreeFn(mtl, numtl_file, 0x3F2);
        }
    }
}


void NuMtlUpdate(NuMtl *mtl) {
    NuMtlUpdateEx(mtl, 1);
}


int NuMtlWrite(int fh, NuMtl *mtl) {
    NuFileWrite(fh, mtl, sizeof(NuMtl));
    return sizeof(NuMtl);
}


NuMtlReadEventFunc NuMtlReadEventSetHandler(NuMtlReadEventFunc handler) {
    NuMtlReadEventFunc old;

    old = NuMtlReadEvent;
    if (handler) {
        NuMtlReadEvent = handler;
    } else {
        NuMtlReadEvent = NuMtlReadEventDefault;
    }
    return old;
}


void *NuMtlGetStateBlockDma(void *dma) {
    return dma;
}


NuMtl *NuMtlIterate(int restart) {
    NuMtl *mtl;

    if (restart) {
        mtl = mtl_list;
        if (mtl) {
            mtl_iter = mtl->next;
        }
        return mtl;
    }
    mtl = mtl_iter;
    if (mtl) {
        mtl_iter = mtl->next;
    }
    return mtl;
}


void NuMtlRemove(NuMtl *mtl) {
    NuDisableVBlank();
    if (mtl->prev) {
        mtl->prev->next = mtl->next;
    } else {
        mtl_list = mtl->next;
    }
    if (mtl->next) {
        mtl->next->prev = mtl->prev;
    }
    NuEnableVBlank();
}


int NuMtlReadEventDefault(NuMtl *mtl) {
    return 0;
}


void NuMtlInsert(NuMtl *mtl) {
    NuMtl *cur;
    NuMtl *prev;
    int key;

    prev = 0;
    cur = mtl_list;
    NuDisableVBlank();
    key = (mtl->alpha << 16) + (mtl->tid >= 0 ? mtl->tid : 0);
    mtl->next = 0;
    mtl->prev = 0;
    while (cur && (cur->alpha << 16) + (cur->tid >= 0 ? cur->tid : 0) < key) {
        prev = cur;
        cur = cur->next;
    }
    if (prev) {
        mtl->prev = prev;
        mtl->next = prev->next;
        prev->next = mtl;
    } else {
        mtl->next = mtl_list;
        mtl_list = mtl;
    }
    if (mtl->next) {
        mtl->next->prev = mtl;
    }
    NuEnableVBlank();
}


void NuMtlUpdateAll(void) {
    NuMtl *mtl;
    NuMtl *next;

    mtl = mtl_list;
    NuErrorProlog(numtl_file, 0x5A8)(msg_update_all_dangerous);
    while (VU1_DmaGetStatus()) {
    }
    for (; mtl; mtl = next) {
        next = mtl->next;
        NuMtlUpdateEx(mtl, 1);
    }
}


int NuMtlDisplayMtl(int index) {
    NuMtlRect rect;
    NuMtl *mtl;
    int i;

    mtl = mtl_list;
    for (i = index; i; i--) {
        if (!mtl) {
            return 0;
        }
        mtl = mtl->display_next;
    }
    rect.y = rect.x = 0.25f;
    rect.h = rect.w = 0.5f;
    rect.u0 = rect.v0 = 0.0f;
    rect.u1 = rect.v1 = 1.0f;
    rect.mtl = mtl;
    NuRndrRect(&rect);
    if (system_font) {
        NuFntSet(system_font);
        NuFntScale(32, 32);
        NuFntPrintEx(480, 320, 0, fmt_display_mtl, index, mtl);
    }
    return 1;
}
