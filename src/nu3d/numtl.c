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

#include "nu_libc.h"
#include "nu_types.h"

#include "nu3d/numtl.h"
#include "nu3d/nustream.h"
#include "nu3d/nutex.h"
#include "nucore/nuerror.h"
#include "numath/numathtypes.h"
#include "nucore/nucoretypes.h"
#include "nups2/nups2types.h"

/* Material attributes as stored in a scene file. */
struct nufmtlattrib_s {
    unsigned int alpha : 2;
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
}; /* 0x4 */

/* Material record of a scene file (the GC-layout numtl_s). */
struct nufilemtl_s {
    struct numtl_s *next;                   /* 0x00 */
    struct nufmtlattrib_s attrib;           /* 0x04 */
    struct nucolour3_s ambient;             /* 0x08 */
    struct nucolour3_s diffuse;             /* 0x14 */
    union nufx_u fx1;                       /* 0x20 */
    union nufx_u fx2;                       /* 0x24 */
    union nufx_u fx3;                       /* 0x28 */
    union nufx_u fx4;                       /* 0x2C */
    float power;                            /* 0x30 */
    float alpha;                            /* 0x34 */
    s32 tid;                                /* 0x38 */
    short sort_pri;                         /* 0x3C */
    unsigned char fxid;                     /* 0x3E */
    unsigned char special_id;               /* 0x3F */
    short K;                                /* 0x40 */
    unsigned char L;                        /* 0x42 */
    unsigned char uanmmode : 4;             /* 0x43 */
    unsigned char vanmmode : 4;
    float du;                               /* 0x44 */
    float dv;                               /* 0x48 */
    float su;                               /* 0x4C */
    float sv;                               /* 0x50 */
}; /* 0x54 */


extern void NuRndrStreamLink(struct rndrstream_s *rs);
extern void *vpDmaTag_Call(void *cur, void *addr);
extern void *vpDmaTag_Next(void *cur, void *addr);

extern void *vpDmaTag_RetEx(void *packet, u32 vif0, u32 vif1);
extern void *vpDmaTag_Close(void *packet);
extern void *vtBeginDirect(void *packet, int irq);
extern void *vtCloseDirect(void *packet);
extern void *gsBeginGifPacket(void *packet, int eop, int nloop, int pre, int prim, int flg, int nreg, ...);
extern void *gsCloseGifPacket(void *packet);
extern float NuTrigTable[];

extern void *NuMemAllocFn(int size, char *file, int line);
extern void NuMemFreeFn(void *ptr, char *file, int line);
extern int NuFileRead(int fh, void *data, int size);
extern int NuFileWrite(int fh, void *data, int size);
extern void NuDisableVBlank(void);
extern void NuEnableVBlank(void);
extern int VU1_DmaGetStatus(void);
extern void NuRndrRect(struct numtlrect_s *rect);
extern void NuFntSet(void *font);
extern void NuFntScale(int x, int y);
extern void NuFntPrintEx(int x, int y, int colour, const char *fmt, ...);

extern void *system_font;

extern struct NuPs2GsReg D_002EA1A0;
#define NuPs2ZBufReg0 D_002EA1A0

extern char vu_load_tri_mpg[];

extern char vu_load_tric_mpg[];

extern char vu_load_tricu_mpg[];

extern char vu_load_trica_mpg[];

extern char vu_load_xformuv_mpg[];

extern struct rndrstream_s *rndrstream_mtl_sort;
extern void *rndrstream_free;

extern struct numtl_s *numtl_defaultmtl2d;
extern struct numtl_s *numtl_defaultmtl3d;

static int initialised = 0;
static struct numtl_s *smlist = 0;
static struct numtl_s *mtl_store = 0;
static int mtl_allocix = 0;
static int mtl_allocmax = 0;
int numtl_lighting_disabled = 0;
int numtl_force_colour = 0;
int numtl_force_mtlsize = 0;
int numtl_force_lit = 0;
int numtl_force_mipmap = 0;
NuMtlReadEventFunc NuMtlReadEvent = NuMtlReadEventDefault;


int CreateStateBlock(struct numtl_s *mtl, int keep) {
    int tme;
    int abe;
    int atst;
    int afail;
    int alpha;
    int zte;
    int ztst;
    int zmsk;
    int iip;
    int fst;
    u64 prmode;
    u64 test;
    u32 *p;

    afail = mtl->attrib.afail;
    tme = mtl->tid != 0;
    abe = mtl->attrib.alpha != 0;
    atst = mtl->attrib.atst;
    switch (mtl->attrib.alpha) {
    case 1:
    default:
        alpha = 0x44;
        break;
    case 2:
        alpha = 0x48;
        break;
    case 3:
        alpha = 0x42;
        break;
    case 4:
        alpha = 0x49;
        break;
    case 5:
        alpha = 0x46;
        break;
    case 6:
        alpha = 0x89;
        break;
    }
    zte = 1;
    switch (mtl->attrib.zmode) {
    case 0:
    default:
        ztst = 2;
        zmsk = 0;
        break;
    case 1:
        ztst = 2;
        zmsk = 1;
        break;
    case 2:
        ztst = 1;
        zmsk = 0;
        break;
    case 3:
        ztst = 1;
        zmsk = 1;
        break;
    }
    p = vpDmaTag_RetEx(mtl, 0x10000000, 0x50000000);
    ((u8 *)p)[-14] = 1;
    fst = mtl->attrib.uvmode;
    iip = mtl->attrib.shade;
    p = vtBeginDirect(p - 1, 0);
    p = gsBeginGifPacket(p, 0, 1, 0, 0, 0, 1, 0xE);
    prmode = ((u64)iip << 3) | ((u64)tme << 4);
    *((u64 *)p)++ = prmode | ((u64)abe << 6) | ((u64)fst << 8);
    *((u64 *)p)++ = 0x1B;
    *((u64 *)p)++ = alpha;
    *((u64 *)p)++ = 0x42 + mtl->context;
    *((u64 *)p)++ = (NuPs2ZBufReg0.zbuf.zbp | (NuPs2ZBufReg0.zbuf.psm << 24)) | ((u64)zmsk << 32);
    *((u64 *)p)++ = 0x4E + mtl->context;
    test = (u64)abe | ((u64)atst << 1) | ((u64)mtl->attrib.aref << 4);
    test |= (u64)afail << 12;
    *((u64 *)p)++ = test | ((u64)zte << 16) | ((u64)ztst << 17);
    *((u64 *)p)++ = 0x47 + mtl->context;
    if (mtl->tid) {
        p = NuTexWriteMaterialSettings(p, mtl);
    }
    p = gsCloseGifPacket(p);
    p = vtCloseDirect(p);
    *p++ = 0x01000101;
    *p++ = 0x6C030013;
    *((float *)p)++ = mtl->diffuse.r * 128.0f;
    *((float *)p)++ = mtl->diffuse.g * 128.0f;
    *((float *)p)++ = mtl->diffuse.b * 128.0f;
    *((float *)p)++ = mtl->alpha * 128.0f;
    mtl->vuattr.u32 = p;
    if (keep) {
        p += 8;
    } else {
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
    }
    *p++ = 0x30000000;
    *p++ = 0;
    *p++ = 0;
    *p++ = 0;
    *p++ = 0;
    vpDmaTag_Close(p);
    if ((u32)p - (u32)mtl >= 0x120) {
        NuErrorProlog("..\\nu2crash.ps2\\nu3d\\numtl.c", 0x1E3)("assert");
    }
    return 1;
}


void NuMtlAnimate(float dt) {
    struct numtl_s *mtl;
    float u;
    float v;
    static float sinetime = 0.0f;

    sinetime += dt;
    for (mtl = smlist; mtl; mtl = mtl->snext) {
        switch (mtl->uanmmode) {
        case 0:
            break;
        case 1:
            u = mtl->du;
            mtl->vuattr.f32[0] = u;
            mtl->vuattr.s32[9] = u * 4096.0f;
            break;
        case 2:
            mtl->du += mtl->su * dt;
            mtl->du = mtl->du - floor(mtl->du);
            u = mtl->du;
            mtl->vuattr.f32[0] = u;
            mtl->vuattr.s32[9] = u * 4096.0f;
            break;
        case 3:
            u = NuTrigTable[(int)(sinetime * mtl->su * 65536.0f) & 0xFFFF] * mtl->du;
            mtl->vuattr.f32[0] = u;
            mtl->vuattr.s32[9] = u * 4096.0f;
            break;
        case 4:
            u = NuTrigTable[((int)(sinetime * mtl->su * 65536.0f) + 0x4000) & 0xFFFF] * mtl->du;
            mtl->vuattr.f32[0] = u;
            mtl->vuattr.s32[9] = u * 4096.0f;
            break;
        }
        switch (mtl->vanmmode) {
        case 0:
            break;
        case 1:
            v = mtl->dv;
            mtl->vuattr.f32[1] = v;
            mtl->vuattr.s32[10] = v * 4096.0f;
            break;
        case 2:
            mtl->dv += mtl->sv * dt;
            mtl->dv = mtl->dv - floor(mtl->dv);
            v = mtl->dv;
            mtl->vuattr.f32[1] = v;
            mtl->vuattr.s32[10] = v * 4096.0f;
            break;
        case 3:
            v = NuTrigTable[(int)(sinetime * mtl->sv * 65536.0f) & 0xFFFF] * mtl->dv;
            mtl->vuattr.f32[1] = v;
            mtl->vuattr.s32[10] = v * 4096.0f;
            break;
        case 4:
            v = NuTrigTable[((int)(sinetime * mtl->sv * 65536.0f) + 0x4000) & 0xFFFF] * mtl->dv;
            mtl->vuattr.f32[1] = v;
            mtl->vuattr.s32[10] = v * 4096.0f;
            break;
        }
    }
}


inline int NuMtlGetZupd(struct numtl_s *mtl) {
    return mtl->zupd;
}


void NuMtlBuildRenderList(void) {
    struct rndrstream_s *rs;
    struct numtl_s *mtl;
    struct numtl_s *tex_mtl;
    u32 *last_tag;
    int tid;
    int accomodated_tid;
    void *mpgtag;

    rs = rndrstream_mtl_sort;
    mtl = smlist;
    mpgtag = 0;
    accomodated_tid = 0;
    tid = 0;
    last_tag = 0;
    NuRndrStreamLink(rs);
    for (tex_mtl = mtl; mtl; mtl = mtl->snext) {
        if (tex_mtl) {
            if (last_tag) {
                last_tag[1] = (u32)(nuintptr)rs->ptr.u8;
                last_tag = 0;
            }
            if (tex_mtl->rs.base.u8 && tex_mtl->rs.ptr.u8 != tex_mtl->rs.base.u8
                && tex_mtl->tid != accomodated_tid) {
                NuTexAccomodateRS(rs, tex_mtl->tid);
                accomodated_tid = tex_mtl->tid;
            }
            tex_mtl = tex_mtl->snext;
        }
        if (mtl->rs.base.u8 && mtl->rs.ptr.u8 != mtl->rs.base.u8) {
            if (last_tag) {
                last_tag[1] = (u32)(nuintptr)rs->ptr.u8;
            }
            if (mtl->tid != tid) {
                rs->ptr.u8 = NuTexSet(rs->ptr.u8, mtl->tid, mtl->context);
                tid = mtl->tid;
            }
            if (mpgtag != mtl->rs.mpg) {
                rs->ptr.u8 = vpDmaTag_Call(rs->ptr.u8, mtl->rs.mpg);
                mpgtag = mtl->rs.mpg;
            }
            rs->ptr.u8 = vpDmaTag_Call(rs->ptr.u8, mtl);
            rs->ptr.u8 = NuRndrStreamAddZBState(rs->ptr.u8, NuMtlGetZupd(mtl));
            rs->ptr.u8 = vpDmaTag_Next(rs->ptr.u8, mtl->rs.base.u8);
            last_tag = (u32 *)mtl->rs.ptr.u8;
            NuRndrStreamResetStream(&mtl->rs, 1);
        }
        mtl->rs.base.u8 = 0;
        mtl->rs.ptr.u8 = 0;
    }
    if (last_tag) {
        last_tag[1] = (u32)(nuintptr)rs->ptr.u8;
    }
    rndrstream_free = vpDmaTag_Next(rs->ptr.u8, 0);
}


void NuMtlCreateDefault(void) {
    numtl_defaultmtl3d = NuMtlCreate(1);
    numtl_defaultmtl3d->attrib.zmode = 0;
    numtl_defaultmtl3d->attrib.zmode = 3;
    numtl_defaultmtl3d->attrib.alpha = 0;
    numtl_defaultmtl3d->diffuse.r = 0.5f;
    numtl_defaultmtl3d->diffuse.g = 0.5f;
    numtl_defaultmtl3d->diffuse.b = 0.5f;
    numtl_defaultmtl3d->alpha = 1.0f;
    numtl_defaultmtl3d->K = -100;
    numtl_defaultmtl3d->L = 0;
    NuMtlUpdate(numtl_defaultmtl3d);
    numtl_defaultmtl2d = NuMtlCreate(1);
    numtl_defaultmtl2d->attrib.cull = 2;
    numtl_defaultmtl2d->attrib.zmode = 3;
    numtl_defaultmtl2d->attrib.alpha = 0;
    numtl_defaultmtl2d->diffuse.r = 0.5f;
    numtl_defaultmtl2d->diffuse.g = 0.5f;
    numtl_defaultmtl2d->alpha = 1.0f;
    numtl_defaultmtl2d->diffuse.b = 0.5f;
    numtl_defaultmtl3d->K = -100;
    numtl_defaultmtl3d->L = 0;
    NuMtlUpdate(numtl_defaultmtl2d);
}


struct numtl_s *NuMtlAlloc(void) {
    struct numtl_s *mtl;
    int i;

    mtl = 0;
    if (mtl_allocmax) {
        i = mtl_allocix + 1;
        if (i >= mtl_allocmax) {
            i = 0;
        }
        while (i != mtl_allocix) {
            if (!mtl_store[i].used) {
                mtl = &mtl_store[i];
                memset(mtl, 0, sizeof(struct numtl_s));
                mtl->used = 1;
                mtl->fromblock = 1;
                mtl_allocix = i;
                break;
            }
            i++;
            if (i >= mtl_allocmax) {
                i = 0;
            }
        }
    } else {
        mtl = NuMemAllocFn(sizeof(struct numtl_s), "..\\nu2crash.ps2\\nu3d\\numtl.c", 0x398);
        memset(mtl, 0, sizeof(struct numtl_s));
        mtl->used = 1;
        mtl->intalloc = 1;
    }
    return mtl;
}


struct numtl_s *NuMtlCreate(int count) {
    struct numtl_s *mtl;
    int i;

    mtl = 0;
    for (i = 0; i < count; i++) {
        mtl = NuMtlAlloc();
        if (!mtl) {
            NuErrorProlog("..\\nu2crash.ps2\\nu3d\\numtl.c", 0x3AF)("Unable to allocate material");
        }
        mtl->attrib.filter = 1;
        mtl->attrib.atst = 5;
        mtl->attrib.aref = 0x7F;
        mtl->attrib.afail = 3;
        mtl->attrib.shade = 1;
        mtl->alpha = 1.0f;
        mtl->L = 0;
        mtl->du = 0.0f;
        mtl->dv = 0.0f;
        mtl->su = 0.0f;
        mtl->sv = 1.0f;
        mtl->rs.mtl = mtl;
        mtl->K = -90;
        mtl->sort_pri = 0x7FFF;
        mtl->rs.attr = 0x3D;
        NuRndrStreamResetStream(&mtl->rs, 1);
        NuMtlInsert(mtl);
        mtl->next = 0;
        mtl->fxid = 0;
        mtl->special_id = 0;
    }
    return mtl;
}


struct numtl_s *NuMtlRead(int fh) {
    struct numtl_s *mtl;
    struct nufilemtl_s file;

    mtl = NuMtlCreate(1);
    if (numtl_force_mtlsize) {
        NuFileRead(fh, mtl, numtl_force_mtlsize);
    } else {
        NuFileRead(fh, &file, 0x54);
        mtl->next = file.next;
        mtl->ambient = file.ambient;
        mtl->diffuse = file.diffuse;
        mtl->fx1.s32 = file.fx1.s32;
        mtl->fx2.s32 = file.fx2.s32;
        mtl->fx3.s32 = file.fx3.s32;
        mtl->fx4.s32 = file.fx4.s32;
        mtl->power = file.power;
        mtl->alpha = file.alpha;
        mtl->tid = file.tid;
        mtl->sort_pri = file.sort_pri;
        mtl->K += file.K;
        mtl->L += file.L;
        mtl->uanmmode = file.uanmmode;
        mtl->vanmmode = file.vanmmode;
        mtl->du = file.du;
        mtl->dv = file.dv;
        mtl->su = file.su;
        mtl->sv = file.sv;
        mtl->fxid = file.fxid;
        mtl->special_id = file.special_id;
        mtl->attrib.alpha = file.attrib.alpha;
        mtl->attrib.filter = file.attrib.filter;
        mtl->attrib.fx = 0;
        mtl->attrib.utc = file.attrib.utc;
        mtl->attrib.vtc = file.attrib.vtc;
        mtl->attrib.cull = file.attrib.cull;
        mtl->attrib.zmode = file.attrib.zmode;
        mtl->attrib.lighting = file.attrib.lighting;
        mtl->attrib.colour = file.attrib.colour;
        mtl->attrib.fill = file.attrib.fill;
        mtl->attrib.atst = file.attrib.atst;
        mtl->attrib.aref = file.attrib.aref;
        mtl->attrib.afail = file.attrib.afail;
        mtl->attrib.uvmode = file.attrib.uvmode;
        mtl->attrib.ot = 0;
        if (mtl->attrib.alpha && mtl->attrib.atst == 5) {
            if (mtl->attrib.aref == 0xFF) {
                mtl->attrib.ot = 1;
                mtl->attrib.aref = 0x7F;
            }
        }
        mtl->attrib.shade = 1;
        if (mtl->attrib.afail == 1) {
            mtl->attrib.afail = 3;
        }
    }
    if (numtl_lighting_disabled) {
        mtl->attrib.lighting = 2;
    }
    if (mtl->attrib.alpha) {
        mtl->sort_pri++;
    }
    if (numtl_force_mipmap) {
        mtl->attrib.filter = 2;
    }
    if (mtl->tid != -1) {
        mtl->diffuse.r *= 0.5f;
        mtl->diffuse.g *= 0.5f;
        mtl->diffuse.b *= 0.5f;
    }
    return mtl;
}


int NuMtlValid(struct numtl_s *mtl) {
    return mtl->used;
}


void NuMtlInit(void) {
    if (initialised) {
        NuMtlClose();
    }
    smlist = 0;
    initialised = 1;
    mtl_allocmax = 0;
    mtl_store = 0;
    NuMtlCreateDefault();
    NuMtlReadEventSetHandler(0);
}


int NuMtlGetSysBuffSize(int count) {
    return count * sizeof(struct numtl_s);
}


void NuMtlInitEx(union variptr_u *buff, int count) {
    struct numtl_s *pool;
    int i;

    if (initialised) {
        NuMtlClose();
    }
    smlist = 0;
    initialised = 1;
    mtl_allocmax = 0;
    mtl_store = 0;
    NuMtlCreateDefault();
    NuMtlReadEventSetHandler(0);
    pool = (struct numtl_s *)((buff->intaddr + 0xF) & ~(nuintptr)0xF);
    mtl_allocmax = count;
    mtl_allocix = 0;
    buff->voidptr = pool;
    mtl_store = pool;
    buff->voidptr = pool + count;
    memset(pool, 0, count * sizeof(struct numtl_s));
    for (i = 0; i < mtl_allocmax; i++) {
        mtl_store[i].fromblock = 1;
    }
    NuMtlCreateDefault();
}


void NuMtlClose(void) {
    struct numtl_s *mtl;
    struct numtl_s *next;
    int i;

    if (initialised) {
        mtl = smlist;
        if (mtl_store) {
            for (i = 0; i < mtl_allocmax; i++) {
                mtl_store[i].used = 0;
            }
            smlist = 0;
        } else {
            for (; mtl; mtl = next) {
                next = mtl->snext;
                NuMtlDestroy(mtl);
            }
        }
        NuMtlCreateDefault();
        initialised = 0;
    }
}


void NuMtlRelease(void) {
    struct numtl_s *mtl;
    struct numtl_s *next;
    int i;

    mtl = smlist;
    if (mtl_store) {
        for (i = 0; i < mtl_allocmax; i++) {
            mtl_store[i].used = 0;
        }
        smlist = 0;
    } else {
        for (; mtl; mtl = next) {
            next = mtl->snext;
            NuMtlDestroy(mtl);
        }
    }
    NuMtlCreateDefault();
}


void NuMtlDestroy(struct numtl_s *mtl) {
    if (mtl->tid) {
        NuTexUnRef(mtl->tid);
    }
    NuMtlRemove(mtl);
    if (mtl->fromblock) {
        if (!mtl->used) {
            NuDebugMsgProlog("..\\nu2crash.ps2\\nu3d\\numtl.c", 0x3EC)(
                "Attempted to release a material that is already free");
        }
        mtl->used = 0;
    } else {
        mtl->used = 0;
        if (mtl->intalloc) {
            NuMemFreeFn(mtl, "..\\nu2crash.ps2\\nu3d\\numtl.c", 0x3F2);
        }
    }
}


void NuMtlUpdate(struct numtl_s *mtl) {
    NuMtlUpdateEx(mtl, 1);
}


int NuMtlWrite(int fh, struct numtl_s *mtl) {
    NuFileWrite(fh, mtl, sizeof(struct numtl_s));
    return sizeof(struct numtl_s);
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


inline void NuMtlInsert(struct numtl_s *mtl) {
    struct numtl_s *cur;
    struct numtl_s *prev;
    int key;

    prev = 0;
    cur = smlist;
    NuDisableVBlank();
    key = (mtl->sort_pri << 16) + (mtl->tid >= 0 ? mtl->tid : 0);
    mtl->snext = 0;
    mtl->slast = 0;
    while (cur && (cur->sort_pri << 16) + (cur->tid >= 0 ? cur->tid : 0) < key) {
        prev = cur;
        cur = cur->snext;
    }
    if (prev) {
        mtl->slast = prev;
        mtl->snext = prev->snext;
        prev->snext = mtl;
    } else {
        mtl->snext = smlist;
        smlist = mtl;
    }
    if (mtl->snext) {
        mtl->snext->slast = mtl;
    }
    NuEnableVBlank();
}


void NuMtlAdd(struct numtl_s *mtl) {
    mtl->vuattr.voidptr = 0;
    mtl->slast = 0;
    mtl->snext = 0;
    mtl->next = 0;
    mtl->used = 1;
    mtl->fromblock = 0;
    mtl->intalloc = 0;
    mtl->rs.mtl = mtl;
    NuRndrStreamResetStream(&mtl->rs, 1);
    NuMtlInsert(mtl);
}

struct numtl_s *NuMtlIterate(int restart) {
    struct numtl_s *mtl;
    static struct numtl_s *sm = 0;

    if (restart) {
        mtl = smlist;
        if (mtl) {
            sm = mtl->snext;
        }
        return mtl;
    }
    mtl = sm;
    if (mtl) {
        sm = mtl->snext;
    }
    return mtl;
}


inline void NuMtlRemove(struct numtl_s *mtl) {
    NuDisableVBlank();
    if (mtl->slast) {
        mtl->slast->snext = mtl->snext;
    } else {
        smlist = mtl->snext;
    }
    if (mtl->snext) {
        mtl->snext->slast = mtl->slast;
    }
    NuEnableVBlank();
}


void NuMtlUpdateEx(struct numtl_s *mtl, int flags) {
    int keep;
    struct numtl_s *zmtl;

    NuDisableVBlank();
    if (numtl_lighting_disabled) {
        mtl->attrib.lighting = 2;
    }
    if (numtl_force_colour) {
        mtl->attrib.colour = 1;
    }
    if (numtl_force_lit) {
        mtl->attrib.lighting = 0;
    }
    keep = flags & 2;
    if (!keep) {
        memset(mtl, 0, 0x120);
    }
    CreateStateBlock(mtl, keep);
    switch (mtl->attrib.zmode) {
    case 0:
    case 2:
    default:
        mtl->zupd = 1;
        break;
    case 1:
    case 3:
        mtl->zupd = 0;
        break;
    }
    zmtl = mtl->callback;
    if (zmtl) {
        switch (zmtl->attrib.zmode) {
        case 0:
        case 2:
        default:
            zmtl->zupd = 1;
            break;
        case 1:
        case 3:
            zmtl->zupd = 0;
            break;
        }
    }
    switch (mtl->attrib.lighting) {
    case 3:
        mtl->rs.attr = 0x3D;
        mtl->rs.mpg = vu_load_trica_mpg;
        break;
    case 2:
        mtl->rs.attr = 0x3C;
        mtl->rs.mpg = vu_load_tricu_mpg;
        break;
    case 0:
    default:
        if (mtl->attrib.colour == 1) {
            mtl->rs.mpg = vu_load_tric_mpg;
        } else {
            mtl->rs.mpg = vu_load_tri_mpg;
        }
        mtl->rs.attr = 0x3D;
        break;
    case 1:
        mtl->rs.attr = 0x3E;
        mtl->rs.mpg = vu_load_xformuv_mpg;
        break;
    }
    switch (mtl->attrib.fx) {
    case 0:
        break;
    case 1:
        mtl->rs.attr = 0x7C;
        mtl->rs.mpg = vu_load_xformuv_mpg;
        break;
    }
    if (flags & 1) {
        NuMtlRemove(mtl);
        NuMtlInsert(mtl);
    }
    NuEnableVBlank();
}


struct numtl_s *numtl_defaultmtl2d = 0;
struct numtl_s *numtl_defaultmtl3d = 0;


int NuMtlReadEventDefault(struct numtl_s *mtl) {
    return 0;
}



void NuMtlUpdateAll(void) {
    struct numtl_s *mtl;
    struct numtl_s *next;

    mtl = smlist;
    NuErrorProlog("..\\nu2crash.ps2\\nu3d\\numtl.c", 0x5A8)("Potentially dangerous - do not use");
    while (VU1_DmaGetStatus()) {
    }
    for (; mtl; mtl = next) {
        next = mtl->snext;
        NuMtlUpdateEx(mtl, 1);
    }
}


int NuMtlDisplayMtl(int index) {
    struct numtlrect_s rect;
    struct numtl_s *mtl;
    int i;

    mtl = smlist;
    for (i = index; i; i--) {
        if (!mtl) {
            return 0;
        }
        mtl = mtl->next;
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
        NuFntPrintEx(480, 320, 0, "MTL ID %03d MTL ADDR %08x", index, mtl);
    }
    return 1;
}
