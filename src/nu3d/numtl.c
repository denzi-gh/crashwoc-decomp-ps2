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

struct numtlfile_s {
    struct numtl_s *next;
    u32 alpha_mode : 2;
    u32 filter : 2;
    u32 fx : 2;
    u32 utc : 2;
    u32 vtc : 2;
    u32 cull : 2;
    u32 zmode : 2;
    u32 lighting : 2;
    u32 colour : 1;
    u32 fill : 1;
    u32 atst : 3;
    u32 aref : 8;
    u32 afail : 2;
    u32 fst : 1;
    struct nucolour3_s ambient;
    struct nucolour3_s diffuse;
    int fx1;
    int fx2;
    int fx3;
    int fx4;
    float power;
    float alpha;
    int tid;
    short alpha_sort;
    u8 fxid;
    u8 special_id;
    short K;
    u8 L;
    u8 uanmmode : 4;
    u8 vanmmode : 4;
    float du;
    float dv;
    float su;
    float sv;
};


extern void NuRndrStreamLink(struct nurndrstream_s *rs);
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

extern char D_00282980[];
#define rndr3d_microcode D_00282980

extern char D_002834E0[];
#define rndr3d_l_microcode D_002834E0

extern char D_00284620[];
#define rndr3d_u_microcode D_00284620

extern char D_00285EE0[];
#define rndr3d_mode3_microcode D_00285EE0

extern char D_00286FC0[];
#define rndr3d_fx_microcode D_00286FC0

extern struct nurndrstream_s *rndrstream_mtl_sort;
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
            mtl->vuattr.u32[9] = (int)(u * 4096.0f);
            break;
        case 2:
            mtl->du += mtl->su * dt;
            mtl->du = mtl->du - floor(mtl->du);
            u = mtl->du;
            mtl->vuattr.f32[0] = u;
            mtl->vuattr.u32[9] = (int)(u * 4096.0f);
            break;
        case 3:
            u = NuTrigTable[(int)(sinetime * mtl->su * 65536.0f) & 0xFFFF] * mtl->du;
            mtl->vuattr.f32[0] = u;
            mtl->vuattr.u32[9] = (int)(u * 4096.0f);
            break;
        case 4:
            u = NuTrigTable[((int)(sinetime * mtl->su * 65536.0f) + 0x4000) & 0xFFFF] * mtl->du;
            mtl->vuattr.f32[0] = u;
            mtl->vuattr.u32[9] = (int)(u * 4096.0f);
            break;
        }
        switch (mtl->vanmmode) {
        case 0:
            break;
        case 1:
            v = mtl->dv;
            mtl->vuattr.f32[1] = v;
            mtl->vuattr.u32[10] = (int)(v * 4096.0f);
            break;
        case 2:
            mtl->dv += mtl->sv * dt;
            mtl->dv = mtl->dv - floor(mtl->dv);
            v = mtl->dv;
            mtl->vuattr.f32[1] = v;
            mtl->vuattr.u32[10] = (int)(v * 4096.0f);
            break;
        case 3:
            v = NuTrigTable[(int)(sinetime * mtl->sv * 65536.0f) & 0xFFFF] * mtl->dv;
            mtl->vuattr.f32[1] = v;
            mtl->vuattr.u32[10] = (int)(v * 4096.0f);
            break;
        case 4:
            v = NuTrigTable[((int)(sinetime * mtl->sv * 65536.0f) + 0x4000) & 0xFFFF] * mtl->dv;
            mtl->vuattr.f32[1] = v;
            mtl->vuattr.u32[10] = (int)(v * 4096.0f);
            break;
        }
    }
}


inline int NuMtlGetZupd(struct numtl_s *mtl) {
    return mtl->zupd;
}


void NuMtlBuildRenderList(void) {
    struct nurndrstream_s *rs;
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
                last_tag[1] = (u32)rs->cur.b;
                last_tag = 0;
            }
            if (tex_mtl->rs.start && tex_mtl->rs.cur.b != tex_mtl->rs.start
                && tex_mtl->tid != accomodated_tid) {
                NuTexAccomodateRS(rs, tex_mtl->tid);
                accomodated_tid = tex_mtl->tid;
            }
            tex_mtl = tex_mtl->snext;
        }
        if (mtl->rs.start && mtl->rs.cur.b != mtl->rs.start) {
            if (last_tag) {
                last_tag[1] = (u32)rs->cur.b;
            }
            if (mtl->tid != tid) {
                rs->cur.b = NuTexSet(rs->cur.b, mtl->tid, mtl->context);
                tid = mtl->tid;
            }
            if (mpgtag != mtl->rs.mpgtag) {
                rs->cur.b = vpDmaTag_Call(rs->cur.b, mtl->rs.mpgtag);
                mpgtag = mtl->rs.mpgtag;
            }
            rs->cur.b = vpDmaTag_Call(rs->cur.b, mtl);
            rs->cur.b = NuRndrStreamAddZBState(rs->cur.b, NuMtlGetZupd(mtl));
            rs->cur.b = vpDmaTag_Next(rs->cur.b, mtl->rs.start);
            last_tag = mtl->rs.cur.b;
            NuRndrStreamResetStream(&mtl->rs, 1);
        }
        mtl->rs.start = 0;
        mtl->rs.cur.b = 0;
    }
    if (last_tag) {
        last_tag[1] = (u32)rs->cur.b;
    }
    rndrstream_free = vpDmaTag_Next(rs->cur.b, 0);
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
        mtl->rs.flags = 0x3D;
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
    struct numtlfile_s file;

    mtl = NuMtlCreate(1);
    if (numtl_force_mtlsize) {
        NuFileRead(fh, mtl, numtl_force_mtlsize);
    } else {
        NuFileRead(fh, &file, 0x54);
        mtl->next = file.next;
        mtl->ambient = file.ambient;
        mtl->diffuse = file.diffuse;
        mtl->fx1.s32 = file.fx1;
        mtl->fx2.s32 = file.fx2;
        mtl->fx3.s32 = file.fx3;
        mtl->fx4.s32 = file.fx4;
        mtl->power = file.power;
        mtl->alpha = file.alpha;
        mtl->tid = file.tid;
        mtl->sort_pri = file.alpha_sort;
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
        mtl->attrib.alpha = file.alpha_mode;
        mtl->attrib.filter = file.filter;
        mtl->attrib.fx = 0;
        mtl->attrib.utc = file.utc;
        mtl->attrib.vtc = file.vtc;
        mtl->attrib.cull = file.cull;
        mtl->attrib.zmode = file.zmode;
        mtl->attrib.lighting = file.lighting;
        mtl->attrib.colour = file.colour;
        mtl->attrib.fill = file.fill;
        mtl->attrib.atst = file.atst;
        mtl->attrib.aref = file.aref;
        mtl->attrib.afail = file.afail;
        mtl->attrib.uvmode = file.fst;
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
    pool = (struct numtl_s *)((buff->intaddr + 0xF) & 0xFFFFFFF0);
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
        mtl->rs.flags = 0x3D;
        mtl->rs.mpgtag = rndr3d_mode3_microcode;
        break;
    case 2:
        mtl->rs.flags = 0x3C;
        mtl->rs.mpgtag = rndr3d_u_microcode;
        break;
    case 0:
    default:
        if (mtl->attrib.colour == 1) {
            mtl->rs.mpgtag = rndr3d_l_microcode;
        } else {
            mtl->rs.mpgtag = rndr3d_microcode;
        }
        mtl->rs.flags = 0x3D;
        break;
    case 1:
        mtl->rs.flags = 0x3E;
        mtl->rs.mpgtag = rndr3d_fx_microcode;
        break;
    }
    switch (mtl->attrib.fx) {
    case 0:
        break;
    case 1:
        mtl->rs.flags = 0x7C;
        mtl->rs.mpgtag = rndr3d_fx_microcode;
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
