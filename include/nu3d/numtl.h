#ifndef NU3D_NUMTL_H
#define NU3D_NUMTL_H

#include "nu_types.h"
#include "numath/numathtypes.h"
#include "nu3d/nustream.h"

/* Material flags. The struct is 8-byte aligned, so the compiler accesses
 * these bit-fields through whole 64-bit units: attrib shares its unit with
 * display_next (low word), attrib2 with ambient.r (high word). */
struct numtlattrib_s {
    unsigned int : 32;               /* numtl_s.display_next */
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
    unsigned int : 1;
} __attribute__((aligned(8))); /* 0x8 */

struct numtlattrib2_s {
    unsigned int afail : 2;
    unsigned int uvmode : 1;
    unsigned int punch : 1;
    unsigned int gouraud : 1;
    unsigned int : 27;
} __attribute__((aligned(8))); /* 0x8 */

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

struct numtluv_s {
    float u;                /* 0x00 */
    float v;                /* 0x04 */
    u32 pad08[6];           /* 0x08 */
    u32 strow;              /* 0x20 */
    int row_u;              /* 0x24 */
    int row_v;              /* 0x28 */
    int row_z;              /* 0x2C */
    int row_w;              /* 0x30 */
}; /* 0x34 */

struct nuwaterfx_s;
union variptr_u;

struct numtl_s {
    u8 pad000[0x120];                       /* 0x000 GS state, not decoded */
    union {
        u64 flags;                          /* 0x120 NUMTL_* */
        struct {
            u32 : 32;
            struct nurndrstream_s stream;   /* 0x124 */
            union {
                struct numtluv_s *uv;
                f32 *param;
                struct nuwaterfx_s *water;
            } effect;                       /* 0x15C */
        };
    };
    struct numtl_s *next;                   /* 0x160 */
    struct numtl_s *prev;                   /* 0x164 */
    union {
        struct numtl_s *display_next;       /* 0x168 */
        struct numtl_s *specmtl;            /* 0x168 glass: specular material */
        struct numtl_s *layer;              /* 0x168 water: layer material */
        u64 attrib_reg;                     /* 0x168 whole unit */
        struct numtlattrib_s attrib;        /* 0x168 */
    };
    union {
        struct numtlattrib2_s attrib2;      /* 0x170 */
        struct {
            u32 : 32;
            struct nucolour3_s ambient;     /* 0x174 */
        };
    };
    struct nucolour3_s diffuse;             /* 0x180 */
    union nufx_u fx1;                       /* 0x18C */
    union nufx_u fx2;                       /* 0x190 */
    union nufx_u fx3;                       /* 0x194 */
    union nufx_u fx4;                       /* 0x198 */
    float power;                            /* 0x19C */
    float alpha;                            /* 0x1A0 */
    int tid;                                /* 0x1A4 */
    union {
        u64 alpha_reg;                      /* 0x1A8 whole unit */
        struct {
            int alpha_sort;                 /* 0x1A8 */
            short K;                        /* 0x1AC mipmap K */
            u8 L;                           /* 0x1AE mipmap L */
            u8 uanmode : 4;                 /* 0x1AF */
            u8 vanmode : 4;
        };
    };
    float su;                               /* 0x1B0 */
    float sv;                               /* 0x1B4 */
    float du;                               /* 0x1B8 */
    float dv;                               /* 0x1BC */
    struct numtl_s *zmtl;                   /* 0x1C0 */
    s8 ctxt;                                /* 0x1C4 */
    u8 fxid;                                /* 0x1C5 */
    u8 special_id;                          /* 0x1C6 */
    u8 pad1C7[9];                           /* 0x1C7 */
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
