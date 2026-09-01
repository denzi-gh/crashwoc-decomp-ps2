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
typedef unsigned long long u64;

typedef struct NuMtl {
    u8 unk000[0x120];
    u64 flags;
    u8 unk128[0x38];
    struct NuMtl *next;
    u8 unk164[0x6C];
} NuMtl;

typedef int (*NuMtlReadEventFunc)(NuMtl *mtl);

#define NUMTL_VALID 0x01

extern int NuMtlReadEventDefault(NuMtl *mtl);

extern NuMtlReadEventFunc NuMtlReadEvent;

extern NuMtl *D_0062EBA4;
#define mtl_list D_0062EBA4

extern NuMtl *D_0062EBDC;
#define mtl_iter D_0062EBDC


int NuMtlValid(NuMtl *mtl) {
    u64 flags;

    flags = mtl->flags;
    return -((u8)flags & NUMTL_VALID);
}


int NuMtlGetSysBuffSize(int count) {
    return count * sizeof(NuMtl);
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


int NuMtlReadEventDefault(NuMtl *mtl) {
    return 0;
}
