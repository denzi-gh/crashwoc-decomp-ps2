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

struct nurndrstream_s {
    void *start;
    void *cur;
    void *txtr_start;
    void *txtr_end;
    void *zbuf;
    struct NuMtl *mtl;
    void *mpgtag;
    unsigned int sortkey;
    int local_mtx_id;
    short view_mtx_id;
    short scissor_id;
    short xyoffset_id;
    char pad_0x2A[2];
    void *light_addr;
    unsigned int flags;
    int mtl_tid;
};

struct numtluv_s {
    float u;
    float v;
    u32 pad08[6];
    u32 strow;
    int row_u;
    int row_v;
    int row_z;
    int row_w;
};

struct nucolour3_s {
    float r;
    float g;
    float b;
};

typedef struct NuMtl {
    u8 unk000[0x120];
    /* The attribute bits only fill the low word but are accessed as a dword;
       the render stream starts right after them, at 0x124. */
    union {
        u64 flags;                          /* 0x120 */
        struct {
            u32 attrib;
            struct nurndrstream_s stream;   /* 0x124 */
            struct numtluv_s *uv;           /* 0x15C */
        };
    };
    struct NuMtl *next;
    struct NuMtl *prev;
    struct NuMtl *display_next;
    u32 alpha_mode : 4;
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
    u32 pad3F : 1;
    u32 afail : 2;
    u32 fst : 1;
    u32 punch : 1;
    u32 gouraud : 1;
    struct nucolour3_s ambient;
    struct nucolour3_s diffuse;
    int fx1;
    int fx2;
    int fx3;
    int fx4;
    float power;
    float alpha;
    int tid;
    int alpha_sort;
    short K;
    u8 L;
    u8 uanmode : 4;
    u8 vanmode : 4;
    float su;
    float sv;
    float du;
    float dv;
    struct NuMtl *zmtl;
    signed char ctxt;
    u8 fxid;
    u8 special_id;
    u8 pad1C7[9];
} NuMtl;

struct numtlfile_s {
    struct NuMtl *next;
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
    u8 uanmode : 4;
    u8 vanmode : 4;
    float su;
    float sv;
    float du;
    float dv;
};

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

typedef struct GsZbufReg {
    u64 zbp : 9;
    u64 pad09 : 15;
    u64 psm : 4;
    u64 pad1c : 4;
    u64 zmsk : 1;
    u64 pad21 : 31;
} GsZbufReg;

typedef struct GsZbufPair {
    GsZbufReg zbuf;
    u64 addr;
} GsZbufPair;

typedef int (*NuMtlReadEventFunc)(NuMtl *mtl);
typedef void (*NuErrorFunc)(const char *fmt, ...);

#define NUMTL_VALID 0x01
#define NUMTL_POOLED 0x02
#define NUMTL_ALLOCATED 0x04
#define NUMTL_ZUPD 0x08

extern int NuMtlReadEventDefault(NuMtl *mtl);
extern void NuMtlCreateDefault(void);
extern NuMtl *NuMtlCreate(int count);
extern void NuMtlUpdateEx(NuMtl *mtl, int flags);
extern int CreateStateBlock(NuMtl *mtl, int keep);
extern void NuMtlClose(void);
extern void NuMtlDestroy(NuMtl *mtl);
extern void NuMtlRemove(NuMtl *mtl);
extern void NuMtlInsert(NuMtl *mtl);
extern void NuMtlUpdate(NuMtl *mtl);
extern int NuMtlGetZupd(NuMtl *mtl);
extern void NuRndrStreamResetStream(struct nurndrstream_s *stream, int flag);
extern NuMtlReadEventFunc NuMtlReadEventSetHandler(NuMtlReadEventFunc handler);

extern void NuRndrStreamLink(struct nurndrstream_s *rs);
extern void NuTexAccomodateRS(struct nurndrstream_s *rs, int tid);
extern void *NuTexSet(void *cur, int tid, int ctxt);
extern void *NuRndrStreamAddZBState(void *cur, int zupd);
extern void *vpDmaTag_Call(void *cur, void *addr);
extern void *vpDmaTag_Next(void *cur, void *addr);

extern void *vpDmaTag_RetEx(void *packet, u32 vif0, u32 vif1);
extern void *vpDmaTag_Close(void *packet);
extern void *vtBeginDirect(void *packet, int irq);
extern void *vtCloseDirect(void *packet);
extern void *gsBeginGifPacket(void *packet, int eop, int nloop, int pre, int prim, int flg, int nreg, ...);
extern void *gsCloseGifPacket(void *packet);
extern void *NuTexWriteMaterialSettings(void *packet, NuMtl *mtl);
extern double floor(double x);
extern float NuTrigTable[];

extern void *memset(void *dst, int value, unsigned int size);
extern void *NuMemAllocFn(int size, char *file, int line);
extern void NuMemFreeFn(void *ptr, char *file, int line);
extern NuErrorFunc NuErrorProlog(const char *file, int line);
extern NuErrorFunc NuDebugMsgProlog(const char *file, int line);
extern int NuFileRead(int fh, void *data, int size);
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

extern char D_006144E8[];
#define msg_alloc_failed D_006144E8

extern GsZbufPair D_002EA1A0;
#define NuPs2ZBufReg0 D_002EA1A0

extern char D_0062EBD0[];
#define msg_assert D_0062EBD0

extern float D_0062EBD8;
#define mtl_anim_time D_0062EBD8

extern int numtl_lighting_disabled;
extern int numtl_force_colour;
extern int numtl_force_lit;
extern int numtl_force_mtlsize;
extern int numtl_force_mipmap;

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

extern NuMtl *numtl_defaultmtl2d;
extern NuMtl *numtl_defaultmtl3d;


int CreateStateBlock(NuMtl *mtl, int keep) {
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

    afail = mtl->afail;
    tme = mtl->tid != 0;
    abe = mtl->alpha_mode != 0;
    atst = mtl->atst;
    switch (mtl->alpha_mode) {
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
    switch (mtl->zmode) {
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
    fst = mtl->fst;
    iip = mtl->gouraud;
    p = vtBeginDirect(p - 1, 0);
    p = gsBeginGifPacket(p, 0, 1, 0, 0, 0, 1, 0xE);
    prmode = ((u64)iip << 3) | ((u64)tme << 4);
    *((u64 *)p)++ = prmode | ((u64)abe << 6) | ((u64)fst << 8);
    *((u64 *)p)++ = 0x1B;
    *((u64 *)p)++ = alpha;
    *((u64 *)p)++ = 0x42 + mtl->ctxt;
    *((u64 *)p)++ = (NuPs2ZBufReg0.zbuf.zbp | (NuPs2ZBufReg0.zbuf.psm << 24)) | ((u64)zmsk << 32);
    *((u64 *)p)++ = 0x4E + mtl->ctxt;
    test = (u64)abe | ((u64)atst << 1) | ((u64)mtl->aref << 4);
    test |= (u64)afail << 12;
    *((u64 *)p)++ = test | ((u64)zte << 16) | ((u64)ztst << 17);
    *((u64 *)p)++ = 0x47 + mtl->ctxt;
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
    mtl->uv = (struct numtluv_s *)p;
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
        NuErrorProlog(numtl_file, 0x1E3)(msg_assert);
    }
    return 1;
}


void NuMtlAnimate(float dt) {
    NuMtl *mtl;
    float u;
    float v;

    mtl_anim_time += dt;
    for (mtl = mtl_list; mtl; mtl = mtl->next) {
        switch (mtl->uanmode) {
        case 0:
            break;
        case 1:
            u = mtl->su;
            mtl->uv->u = u;
            mtl->uv->row_u = u * 4096.0f;
            break;
        case 2:
            mtl->su += mtl->du * dt;
            mtl->su = mtl->su - floor(mtl->su);
            u = mtl->su;
            mtl->uv->u = u;
            mtl->uv->row_u = u * 4096.0f;
            break;
        case 3:
            u = NuTrigTable[(int)(mtl_anim_time * mtl->du * 65536.0f) & 0xFFFF] * mtl->su;
            mtl->uv->u = u;
            mtl->uv->row_u = u * 4096.0f;
            break;
        case 4:
            u = NuTrigTable[((int)(mtl_anim_time * mtl->du * 65536.0f) + 0x4000) & 0xFFFF] * mtl->su;
            mtl->uv->u = u;
            mtl->uv->row_u = u * 4096.0f;
            break;
        }
        switch (mtl->vanmode) {
        case 0:
            break;
        case 1:
            v = mtl->sv;
            mtl->uv->v = v;
            mtl->uv->row_v = v * 4096.0f;
            break;
        case 2:
            mtl->sv += mtl->dv * dt;
            mtl->sv = mtl->sv - floor(mtl->sv);
            v = mtl->sv;
            mtl->uv->v = v;
            mtl->uv->row_v = v * 4096.0f;
            break;
        case 3:
            v = NuTrigTable[(int)(mtl_anim_time * mtl->dv * 65536.0f) & 0xFFFF] * mtl->sv;
            mtl->uv->v = v;
            mtl->uv->row_v = v * 4096.0f;
            break;
        case 4:
            v = NuTrigTable[((int)(mtl_anim_time * mtl->dv * 65536.0f) + 0x4000) & 0xFFFF] * mtl->sv;
            mtl->uv->v = v;
            mtl->uv->row_v = v * 4096.0f;
            break;
        }
    }
}


inline int NuMtlGetZupd(NuMtl *mtl) {
    u64 flags;

    flags = mtl->flags;
    return ((signed char)((u8)flags << 4)) >> 7;
}


void NuMtlBuildRenderList(void) {
    struct nurndrstream_s *rs;
    NuMtl *mtl;
    NuMtl *tex_mtl;
    u32 *last_tag;
    int tid;
    int accomodated_tid;
    void *mpgtag;

    rs = rndrstream_mtl_sort;
    mtl = mtl_list;
    mpgtag = 0;
    accomodated_tid = 0;
    tid = 0;
    last_tag = 0;
    NuRndrStreamLink(rs);
    for (tex_mtl = mtl; mtl; mtl = mtl->next) {
        if (tex_mtl) {
            if (last_tag) {
                last_tag[1] = (u32)rs->cur;
                last_tag = 0;
            }
            if (tex_mtl->stream.start && tex_mtl->stream.cur != tex_mtl->stream.start
                && tex_mtl->tid != accomodated_tid) {
                NuTexAccomodateRS(rs, tex_mtl->tid);
                accomodated_tid = tex_mtl->tid;
            }
            tex_mtl = tex_mtl->next;
        }
        if (mtl->stream.start && mtl->stream.cur != mtl->stream.start) {
            if (last_tag) {
                last_tag[1] = (u32)rs->cur;
            }
            if (mtl->tid != tid) {
                rs->cur = NuTexSet(rs->cur, mtl->tid, mtl->ctxt);
                tid = mtl->tid;
            }
            if (mpgtag != mtl->stream.mpgtag) {
                rs->cur = vpDmaTag_Call(rs->cur, mtl->stream.mpgtag);
                mpgtag = mtl->stream.mpgtag;
            }
            rs->cur = vpDmaTag_Call(rs->cur, mtl);
            rs->cur = NuRndrStreamAddZBState(rs->cur, NuMtlGetZupd(mtl));
            rs->cur = vpDmaTag_Next(rs->cur, mtl->stream.start);
            last_tag = mtl->stream.cur;
            NuRndrStreamResetStream(&mtl->stream, 1);
        }
        mtl->stream.start = 0;
        mtl->stream.cur = 0;
    }
    if (last_tag) {
        last_tag[1] = (u32)rs->cur;
    }
    rndrstream_free = vpDmaTag_Next(rs->cur, 0);
}


void NuMtlCreateDefault(void) {
    numtl_defaultmtl3d = NuMtlCreate(1);
    numtl_defaultmtl3d->zmode = 0;
    numtl_defaultmtl3d->zmode = 3;
    numtl_defaultmtl3d->alpha_mode = 0;
    numtl_defaultmtl3d->diffuse.r = 0.5f;
    numtl_defaultmtl3d->diffuse.g = 0.5f;
    numtl_defaultmtl3d->diffuse.b = 0.5f;
    numtl_defaultmtl3d->alpha = 1.0f;
    numtl_defaultmtl3d->K = -100;
    numtl_defaultmtl3d->L = 0;
    NuMtlUpdate(numtl_defaultmtl3d);
    numtl_defaultmtl2d = NuMtlCreate(1);
    numtl_defaultmtl2d->cull = 2;
    numtl_defaultmtl2d->zmode = 3;
    numtl_defaultmtl2d->alpha_mode = 0;
    numtl_defaultmtl2d->diffuse.r = 0.5f;
    numtl_defaultmtl2d->diffuse.g = 0.5f;
    numtl_defaultmtl2d->alpha = 1.0f;
    numtl_defaultmtl2d->diffuse.b = 0.5f;
    numtl_defaultmtl3d->K = -100;
    numtl_defaultmtl3d->L = 0;
    NuMtlUpdate(numtl_defaultmtl2d);
}


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


NuMtl *NuMtlCreate(int count) {
    NuMtl *mtl;
    int i;

    mtl = 0;
    for (i = 0; i < count; i++) {
        mtl = NuMtlAlloc();
        if (!mtl) {
            NuErrorProlog(numtl_file, 0x3AF)(msg_alloc_failed);
        }
        mtl->filter = 1;
        mtl->atst = 5;
        mtl->aref = 0x7F;
        mtl->afail = 3;
        mtl->gouraud = 1;
        mtl->alpha = 1.0f;
        mtl->L = 0;
        mtl->su = 0.0f;
        mtl->sv = 0.0f;
        mtl->du = 0.0f;
        mtl->dv = 1.0f;
        mtl->stream.mtl = mtl;
        mtl->K = -90;
        mtl->alpha_sort = 0x7FFF;
        mtl->stream.flags = 0x3D;
        NuRndrStreamResetStream(&mtl->stream, 1);
        NuMtlInsert(mtl);
        mtl->display_next = 0;
        mtl->fxid = 0;
        mtl->special_id = 0;
    }
    return mtl;
}


NuMtl *NuMtlRead(int fh) {
    NuMtl *mtl;
    struct numtlfile_s file;

    mtl = NuMtlCreate(1);
    if (numtl_force_mtlsize) {
        NuFileRead(fh, mtl, numtl_force_mtlsize);
    } else {
        NuFileRead(fh, &file, 0x54);
        mtl->display_next = file.next;
        mtl->ambient = file.ambient;
        mtl->diffuse = file.diffuse;
        mtl->fx1 = file.fx1;
        mtl->fx2 = file.fx2;
        mtl->fx3 = file.fx3;
        mtl->fx4 = file.fx4;
        mtl->power = file.power;
        mtl->alpha = file.alpha;
        mtl->tid = file.tid;
        mtl->alpha_sort = file.alpha_sort;
        mtl->K += file.K;
        mtl->L += file.L;
        mtl->uanmode = file.uanmode;
        mtl->vanmode = file.vanmode;
        mtl->su = file.su;
        mtl->sv = file.sv;
        mtl->du = file.du;
        mtl->dv = file.dv;
        mtl->fxid = file.fxid;
        mtl->special_id = file.special_id;
        mtl->alpha_mode = file.alpha_mode;
        mtl->filter = file.filter;
        mtl->fx = 0;
        mtl->utc = file.utc;
        mtl->vtc = file.vtc;
        mtl->cull = file.cull;
        mtl->zmode = file.zmode;
        mtl->lighting = file.lighting;
        mtl->colour = file.colour;
        mtl->fill = file.fill;
        mtl->atst = file.atst;
        mtl->aref = file.aref;
        mtl->afail = file.afail;
        mtl->fst = file.fst;
        mtl->punch = 0;
        if (mtl->alpha_mode && mtl->atst == 5) {
            if (mtl->aref == 0xFF) {
                mtl->punch = 1;
                mtl->aref = 0x7F;
            }
        }
        mtl->gouraud = 1;
        if (mtl->afail == 1) {
            mtl->afail = 3;
        }
    }
    if (numtl_lighting_disabled) {
        mtl->lighting = 2;
    }
    if (mtl->alpha_mode) {
        mtl->alpha_sort++;
    }
    if (numtl_force_mipmap) {
        mtl->filter = 2;
    }
    if (mtl->tid != -1) {
        mtl->diffuse.r *= 0.5f;
        mtl->diffuse.g *= 0.5f;
        mtl->diffuse.b *= 0.5f;
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


inline void NuMtlInsert(NuMtl *mtl) {
    NuMtl *cur;
    NuMtl *prev;
    int key;

    prev = 0;
    cur = mtl_list;
    NuDisableVBlank();
    key = (mtl->alpha_sort << 16) + (mtl->tid >= 0 ? mtl->tid : 0);
    mtl->next = 0;
    mtl->prev = 0;
    while (cur && (cur->alpha_sort << 16) + (cur->tid >= 0 ? cur->tid : 0) < key) {
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


void NuMtlAdd(NuMtl *mtl) {
    mtl->uv = 0;
    mtl->prev = 0;
    mtl->next = 0;
    mtl->display_next = 0;
    mtl->flags |= NUMTL_VALID;
    mtl->flags &= ~NUMTL_POOLED;
    mtl->flags &= ~NUMTL_ALLOCATED;
    mtl->stream.mtl = mtl;
    NuRndrStreamResetStream(&mtl->stream, 1);
    NuMtlInsert(mtl);
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


inline void NuMtlRemove(NuMtl *mtl) {
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


void NuMtlUpdateEx(NuMtl *mtl, int flags) {
    int keep;
    NuMtl *zmtl;

    NuDisableVBlank();
    if (numtl_lighting_disabled) {
        mtl->lighting = 2;
    }
    if (numtl_force_colour) {
        mtl->colour = 1;
    }
    if (numtl_force_lit) {
        mtl->lighting = 0;
    }
    keep = flags & 2;
    if (!keep) {
        memset(mtl, 0, 0x120);
    }
    CreateStateBlock(mtl, keep);
    switch (mtl->zmode) {
    case 0:
    case 2:
    default:
        mtl->flags |= NUMTL_ZUPD;
        break;
    case 1:
    case 3:
        mtl->flags &= ~NUMTL_ZUPD;
        break;
    }
    zmtl = mtl->zmtl;
    if (zmtl) {
        switch (zmtl->zmode) {
        case 0:
        case 2:
        default:
            zmtl->flags |= NUMTL_ZUPD;
            break;
        case 1:
        case 3:
            zmtl->flags &= ~NUMTL_ZUPD;
            break;
        }
    }
    switch (mtl->lighting) {
    case 3:
        mtl->stream.flags = 0x3D;
        mtl->stream.mpgtag = rndr3d_mode3_microcode;
        break;
    case 2:
        mtl->stream.flags = 0x3C;
        mtl->stream.mpgtag = rndr3d_u_microcode;
        break;
    case 0:
    default:
        if (mtl->colour == 1) {
            mtl->stream.mpgtag = rndr3d_l_microcode;
        } else {
            mtl->stream.mpgtag = rndr3d_microcode;
        }
        mtl->stream.flags = 0x3D;
        break;
    case 1:
        mtl->stream.flags = 0x3E;
        mtl->stream.mpgtag = rndr3d_fx_microcode;
        break;
    }
    switch (mtl->fx) {
    case 0:
        break;
    case 1:
        mtl->stream.flags = 0x7C;
        mtl->stream.mpgtag = rndr3d_fx_microcode;
        break;
    }
    if (flags & 1) {
        NuMtlRemove(mtl);
        NuMtlInsert(mtl);
    }
    NuEnableVBlank();
}


int NuMtlReadEventDefault(NuMtl *mtl) {
    return 0;
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
