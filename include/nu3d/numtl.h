#ifndef NU3D_NUMTL_H
#define NU3D_NUMTL_H

#include "nu_types.h"
#include "nucore/nucoretypes.h"
#include "numath/numathtypes.h"
#include "nu3d/nustream.h"

/* Material flags. numtl_s is 8-byte aligned (sbdata), so the compiler
 * accesses these bit-fields through whole 64-bit units: the first word
 * shares its unit with numtl_s.next, the second with ambient.r. */
struct numtlattrib_s {
    unsigned int alpha : 4;
    unsigned int filter : 2;
    unsigned int fx : 2;
    unsigned int utc : 2;
    unsigned int vtc : 2;
    unsigned int cull : 2;
    unsigned int zmode : 2;
    unsigned int lighting : 2;
    unsigned int colour : 1;
    unsigned int fill : 1;
    unsigned int atst : 3;
    unsigned int aref : 8;
    unsigned int afail : 2;
    unsigned int uvmode : 1;
    unsigned int ot : 1;
    unsigned int shade : 1;
    unsigned int date : 1;
    unsigned int datm : 1;
    unsigned int pad : 25;
}; /* 0x8 */

/* Per-effect material parameter. */
union nufx_u {
    void *voidptr;
    unsigned char u8;
    char s8;
    unsigned short u16;
    short s16;
    unsigned int u32;
    int s32;
    float f32;
}; /* 0x4 */

struct numtl_s {
    u64 sbdata[36];                         /* 0x000 GS state block */
    int used : 1;                           /* 0x120 */
    int fromblock : 1;
    int intalloc : 1;
    int zupd : 1;
    int sbexists : 1;
    int renderplane_id : 8;
    char accesscount;                       /* 0x122 */
    char curr_rp;                           /* 0x123 */
    struct nurndrstream_s rs;               /* 0x124 */
    union variptr_u vuattr;                 /* 0x15C */
    struct numtl_s *snext;                  /* 0x160 */
    struct numtl_s *slast;                  /* 0x164 */
    struct numtl_s *next;               /* 0x168 */
    struct numtlattrib_s attrib;            /* 0x16C */
    struct nucolour3_s ambient;             /* 0x174 */
    struct nucolour3_s diffuse;             /* 0x180 */
    union nufx_u fx1;                       /* 0x18C */
    union nufx_u fx2;                       /* 0x190 */
    union nufx_u fx3;                       /* 0x194 */
    union nufx_u fx4;                       /* 0x198 */
    float power;                            /* 0x19C */
    float alpha;                            /* 0x1A0 */
    int tid;                                /* 0x1A4 */
    int sort_pri;                           /* 0x1A8 */
    short K;                                /* 0x1AC mipmap K */
    unsigned char L;                        /* 0x1AE mipmap L */
    unsigned char uanmmode : 4;             /* 0x1AF */
    unsigned char vanmmode : 4;
    float du;                               /* 0x1B0 */
    float dv;                               /* 0x1B4 */
    float su;                               /* 0x1B8 */
    float sv;                               /* 0x1BC */
    void *callback;                         /* 0x1C0 */
    char context;                           /* 0x1C4 */
    unsigned char fxid;                     /* 0x1C5 */
    unsigned char special_id;               /* 0x1C6 */
    unsigned char inuvanimlist;             /* 0x1C7 */
    unsigned int fb_mask;                   /* 0x1C8 */
    union variptr_u callbackdata;           /* 0x1CC */
}; /* 0x1D0 */

typedef int (*NuMtlReadEventFunc)(struct numtl_s *mtl);

struct numtlrect_s {
    float x;                /* 0x00 */
    float y;                /* 0x04 */
    float w;                /* 0x08 */
    float h;                /* 0x0C */
    float z;                /* 0x10 */
    float u0;               /* 0x14 */
    float v0;               /* 0x18 */
    float u1;               /* 0x1C */
    float v1;               /* 0x20 */
    struct numtl_s *mtl;    /* 0x24 */
}; /* 0x28 */

int CreateStateBlock(struct numtl_s *mtl, int keep);
void NuMtlAnimate(float dt);
int NuMtlGetZupd(struct numtl_s *mtl);
void NuMtlBuildRenderList(void);
void NuMtlCreateDefault(void);
struct numtl_s *NuMtlAlloc(void);
struct numtl_s *NuMtlCreate(int count);
struct numtl_s *NuMtlRead(int fh);
int NuMtlValid(struct numtl_s *mtl);
void NuMtlInit(void);
int NuMtlGetSysBuffSize(int count);
void NuMtlInitEx(union variptr_u *buff, int count);
void NuMtlClose(void);
void NuMtlRelease(void);
void NuMtlDestroy(struct numtl_s *mtl);
void NuMtlUpdate(struct numtl_s *mtl);
int NuMtlWrite(int fh, struct numtl_s *mtl);
NuMtlReadEventFunc NuMtlReadEventSetHandler(NuMtlReadEventFunc handler);
void *NuMtlGetStateBlockDma(void *dma);
void NuMtlInsert(struct numtl_s *mtl);
void NuMtlAdd(struct numtl_s *mtl);
struct numtl_s *NuMtlIterate(int restart);
void NuMtlRemove(struct numtl_s *mtl);
void NuMtlUpdateEx(struct numtl_s *mtl, int flags);
int NuMtlReadEventDefault(struct numtl_s *mtl);
void NuMtlUpdateAll(void);
int NuMtlDisplayMtl(int index);

#endif /* NU3D_NUMTL_H */
