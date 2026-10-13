/*
 * Unit: nu3d/nugobj
 *
 * Functions:
 *   0x001151a0 NuGobjDestroy
 *   0x001152e8 NuGobjCalcFaceOnDims
 *   0x00115678 NuGobjCalcDims
 *   0x00115978 NuPrimCalcPlanes
 *   0x00115d30 NuCreateStarfield
 *   0x00115f68 NuGobjInit
 *   0x00115fa0 NuGobjClose
 *   0x00115fe8 NuGobjCreate
 *   0x00116048 NuGobjAddGeom
 *   0x00116080 NuGobjAddFaceOnGeom
 *   0x001160b8 NuGobjCalcPlanes
 *   0x001160f0 NuGobjApplyCrossFade
 *   0x00116160 NuGeomCreate
 *   0x001161b8 NuGeomDestroy
 *   0x001162b0 NuFaceOnGeomCreate
 *   0x001162f8 NuFaceOnGeomDestroy
 *   0x00116348 NuGeomCalcPlanes
 *   0x00116398 NuGeomAddPrim
 *   0x001163d0 NuGeomAddSkin
 *   0x00116408 NuPrimCreate
 *   0x001164c0 NuPrimDestroy
 *   0x00116528 NuVtxStride
 *   0x00116558 NuPrimStride
 */

typedef unsigned long long u64;
typedef unsigned int u32;

struct nuvec_s {
    float x;
    float y;
    float z;
}; /* 0x0C */

struct nuplane_s {
    float a;
    float b;
    float c;
    float d;
}; /* 0x10 */

struct nuvtx_tc1_s {
    struct nuvec_s pnt;             /* 0x00 */
    struct nuvec_s nrm;             /* 0x0C */
    int diffuse;                    /* 0x18 */
    float tc[2];                    /* 0x1C */
}; /* 0x24 */

struct nuprim_s {
    struct nuprim_s *next;          /* 0x00 */
    unsigned int type;              /* 0x04 */
    int cnt;                        /* 0x08 */
    int max;                        /* 0x0C */
    short *vid;                     /* 0x10 */
    struct nuplane_s *pln;          /* 0x14 */
}; /* 0x18 */

struct nuskin_s {
    struct nuskin_s *next;          /* 0x00 */
    int vtxoffset;                  /* 0x04 */
    int vtxcnt;                     /* 0x08 */
    int mtxcnt;                     /* 0x0C */
    int *mtxid;                     /* 0x10 */
    float *weights;                 /* 0x14 */
}; /* 0x18 */

struct nublendgeom_s {
    int nblends;                    /* 0x00 */
    struct nuvec_s **blend_offsets; /* 0x04 */
    int *ix;                        /* 0x08 */
    struct nuvec_s *offsets;        /* 0x0C */
}; /* 0x10 */

struct nugeom_s {
    struct nugeom_s *next;          /* 0x00 */
    char pad04[0x14];
    int vtxtype;                    /* 0x18 */
    int vtxflags;                   /* 0x1C */
    int vtxcnt;                     /* 0x20 */
    int vtxmax;                     /* 0x24 */
    void *vtxptr;                   /* 0x28 */
    struct nuprim_s *prim;          /* 0x2C */
    struct nuskin_s *skins;         /* 0x30 */
    void *vtxskininfo;              /* 0x34 */
    char pad38[0x10];
    struct nublendgeom_s *blendgeom; /* 0x48 */
    int id;                         /* 0x4C */
}; /* 0x50 */

struct nufaceon_s {
    struct nuvec_s point;           /* 0x00 */
    float width;                    /* 0x0C */
    float height;                   /* 0x10 */
    int colour;                     /* 0x14 */
}; /* 0x18 */

struct nufaceongeom_s {
    struct nufaceongeom_s *next;    /* 0x00 */
    char pad04[0x0C];
    struct nufaceon_s *faceons;     /* 0x10 */
    int nfaceons;                   /* 0x14 */
    float faceon_radius;            /* 0x18 */
    void *dmastream;                /* 0x1C */
    char pad20[0x30];
}; /* 0x50 */

struct nucolourblend_s;

struct nugobj_s {
    struct nugobj_s *sysnext;       /* 0x00 */
    struct nugobj_s *syslast;       /* 0x04 */
    int type;                       /* 0x08 */
    struct nugeom_s *geom;          /* 0x0C */
    struct nufaceongeom_s *faceon_geom; /* 0x10 */
    float r;                        /* 0x14 */
    float r2;                       /* 0x18 */
    struct nuvec_s min;             /* 0x1C */
    struct nuvec_s max;             /* 0x28 */
    struct nuvec_s cntr;            /* 0x34 */
    float cntr_r;                   /* 0x40 */
    float cntr_r2;                  /* 0x44 */
    char pad48[0x14];
    struct nucolourblend_s *blend;  /* 0x5C */
    char pad60[0x04];
    struct nucolourblend_s *faceon_blend; /* 0x64 */
    char pad68[0x04];
}; /* 0x6C */

union nugiftag_u {
    struct {
        u64 nloop : 15;
        u64 eop : 1;
        u64 pad16 : 30;
        u64 pre : 1;
        u64 prim : 11;
        u64 flg : 2;
        u64 nreg : 4;
        u64 regs;
    } tag;
    u64 dw[2];
    u32 w[4];
};

typedef void (*NuErrorFunc)(const char *fmt, ...);

extern void *NuMemAllocFn(int size, char *file, int line);
extern void NuMemFreeFn(void *ptr, char *file, int line);
extern void *memset(void *s, int c, unsigned int n);
extern NuErrorFunc NuErrorProlog(const char *file, int line);
extern double sqrt(double x);
extern long NuRand(void *rand);
extern void NuVecSub(struct nuvec_s *dst, struct nuvec_s *a, struct nuvec_s *b);
extern void NuVecRotateX(struct nuvec_s *dst, struct nuvec_s *src, int angle);
extern void NuVecRotateY(struct nuvec_s *dst, struct nuvec_s *src, int angle);
extern void NuPlnEqn(struct nuplane_s *pln, struct nuvec_s *p0, struct nuvec_s *p1, struct nuvec_s *p2);
extern u32 *vpDmaTag_RetEx(void *buf, u32 vif0, u32 vif1);
extern void *vpDmaTag_Close(void *end);
extern void NuPs2DestroyRenderStream(struct nugobj_s *gobj);
extern void NuGobjDestroy(struct nugobj_s *gobj);
void NuGobjClose(void);
void NuGobjCalcFaceOnDims(struct nugobj_s *gobj);
extern void NuGeomDestroy(struct nugeom_s *geom);
extern void NuFaceOnGeomDestroy(struct nufaceongeom_s *geom);
extern void NuGeomCalcPlanes(struct nugeom_s *geom);
extern void NuPrimDestroy(struct nuprim_s *prim);
extern int NuVtxStride(int type);
extern int NuGobjColourBlendGobj(struct nucolourblend_s *blend, short fade);

extern char D_00614198[];
extern char D_006141B8[];
extern char D_0062EB80[];
extern int D_0062EB60;
extern struct nugobj_s *D_0062EB64;
extern int D_0062EB68;
extern float D_0062EB6C[];
extern float D_0062EB70[];
extern float D_0062EB74[];
extern float D_0062EB78[];
extern int D_0062EB7C;

#define nugobj_file D_00614198
#define err_no_vtxptr D_006141B8
#define err_planes_exist D_0062EB80
#define sysinit D_0062EB60
#define sysgobj D_0062EB64
#define sysgobjcount D_0062EB68
#define faceon_bound_min D_0062EB6C
#define faceon_bound_max D_0062EB70
#define gobj_bound_min D_0062EB74
#define gobj_bound_max D_0062EB78
#define sysgeomid D_0062EB7C


void NuGobjDestroy(struct nugobj_s *gobj) {
    struct nugeom_s *geom;
    struct nugeom_s *nextgeom;
    struct nufaceongeom_s *faceon;
    struct nufaceongeom_s *nextfaceon;

    geom = gobj->geom;
    faceon = gobj->faceon_geom;
    NuPs2DestroyRenderStream(gobj);
    while (geom) {
        nextgeom = geom->next;
        if (geom->blendgeom) {
            if (geom->blendgeom->blend_offsets) {
                NuMemFreeFn(geom->blendgeom->blend_offsets, nugobj_file, 0xBF);
            }
            if (geom->blendgeom->offsets) {
                NuMemFreeFn(geom->blendgeom->offsets, nugobj_file, 0xC2);
            }
            if (geom->blendgeom->ix) {
                NuMemFreeFn(geom->blendgeom->ix, nugobj_file, 0xC5);
            }
            NuMemFreeFn(geom->blendgeom, nugobj_file, 0xC7);
        }
        NuGeomDestroy(geom);
        geom = nextgeom;
    }
    if (faceon && faceon->dmastream) {
        NuMemFreeFn(faceon->dmastream, nugobj_file, 0xD1);
    }
    while (faceon) {
        nextfaceon = faceon->next;
        NuFaceOnGeomDestroy(faceon);
        faceon = nextfaceon;
    }
    if (gobj->sysnext) {
        gobj->sysnext->syslast = gobj->syslast;
    }
    if (gobj->syslast) {
        gobj->syslast->sysnext = gobj->sysnext;
    } else {
        sysgobj = gobj->sysnext;
    }
    NuMemFreeFn(gobj, nugobj_file, 0xE1);
}


void NuGobjCalcFaceOnDims(struct nugobj_s *gobj) {
    struct nufaceongeom_s *geom;
    struct nufaceon_s *faceon;
    struct nuvec_s v;
    float size;
    float maxsize;
    float dist;
    float lo;
    float hi;
    int i;

    lo = faceon_bound_min[0];
    hi = faceon_bound_max[0];
    gobj->r2 = 0.0f;
    gobj->min.x = lo;
    gobj->min.y = lo;
    gobj->min.z = lo;
    gobj->max.x = hi;
    gobj->max.y = hi;
    gobj->max.z = hi;
    maxsize = gobj->r2;
    for (geom = gobj->faceon_geom; geom; geom = geom->next) {
        for (i = 0; i < geom->nfaceons; i++) {
            faceon = &geom->faceons[i];
            size = faceon->width;
            if (size < faceon->height) {
                size = faceon->height;
            }
            if (faceon->point.x - size < gobj->min.x) {
                gobj->min.x = faceon->point.x - size;
            }
            if (faceon->point.y - size < gobj->min.y) {
                gobj->min.y = faceon->point.y - size;
            }
            if (faceon->point.z - size < gobj->min.z) {
                gobj->min.z = faceon->point.z - size;
            }
            if (faceon->point.x + size > gobj->max.x) {
                gobj->max.x = faceon->point.x + size;
            }
            if (faceon->point.y + size > gobj->max.y) {
                gobj->max.y = faceon->point.y + size;
            }
            if (faceon->point.z + size > gobj->max.z) {
                gobj->max.z = faceon->point.z + size;
            }
            dist = faceon->point.x * faceon->point.x + faceon->point.y * faceon->point.y
                 + faceon->point.z * faceon->point.z;
            if (dist > gobj->r2) {
                gobj->r2 = dist;
            }
        }
    }
    gobj->r = sqrt(gobj->r2);
    gobj->cntr.x = (gobj->min.x + gobj->max.x) * 0.5f;
    gobj->cntr.y = (gobj->min.y + gobj->max.y) * 0.5f;
    gobj->cntr.z = (gobj->min.z + gobj->max.z) * 0.5f;
    gobj->cntr_r2 = 0.0f;
    for (geom = gobj->faceon_geom; geom; geom = geom->next) {
        for (i = 0; i < geom->nfaceons; i++) {
            faceon = &geom->faceons[i];
            size = faceon->width;
            if (size < faceon->height) {
                size = faceon->height;
            }
            if (maxsize < size) {
                maxsize = size;
            }
            NuVecSub(&v, &faceon->point, &gobj->cntr);
            if (v.x < 0.0f) {
                v.x = -v.x;
            }
            if (v.y < 0.0f) {
                v.y = -v.y;
            }
            if (v.z < 0.0f) {
                v.z = -v.z;
            }
            dist = (v.x + size) * (v.x + size) + (v.y + size) * (v.y + size)
                 + (v.z + size) * (v.z + size);
            if (dist > gobj->cntr_r2) {
                gobj->cntr_r2 = dist;
            }
        }
    }
    gobj->min.x -= maxsize;
    gobj->min.y -= maxsize;
    gobj->min.z -= maxsize;
    gobj->max.x += maxsize;
    gobj->max.y += maxsize;
    gobj->max.z += maxsize;
    gobj->cntr_r = sqrt(gobj->cntr_r2);
}


void NuGobjCalcDims(struct nugobj_s *gobj) {
    struct nugeom_s *geom;
    struct nuvec_s *pnt;
    struct nuvec_s v;
    char *vtx;
    char *end;
    int stride;
    float dist;
    float lo;
    float hi;

    geom = gobj->geom;
    if (gobj->type == 1) {
        NuGobjCalcFaceOnDims(gobj);
        return;
    }
    lo = gobj_bound_min[0];
    hi = gobj_bound_max[0];
    gobj->min.x = lo;
    gobj->min.y = lo;
    gobj->min.z = lo;
    gobj->max.x = hi;
    gobj->max.y = hi;
    gobj->max.z = hi;
    gobj->r2 = 0.0f;
    for (; geom; geom = geom->next) {
        stride = NuVtxStride(geom->vtxtype);
        vtx = geom->vtxptr;
        if (!vtx) {
            NuErrorProlog(nugobj_file, 0x180)(err_no_vtxptr);
        }
        end = vtx + stride * geom->vtxcnt;
        for (; vtx < end; vtx += stride) {
            pnt = (struct nuvec_s *)vtx;
            if (pnt->x < gobj->min.x) {
                gobj->min.x = pnt->x;
            }
            if (pnt->y < gobj->min.y) {
                gobj->min.y = pnt->y;
            }
            if (pnt->z < gobj->min.z) {
                gobj->min.z = pnt->z;
            }
            if (pnt->x > gobj->max.x) {
                gobj->max.x = pnt->x;
            }
            if (pnt->y > gobj->max.y) {
                gobj->max.y = pnt->y;
            }
            if (pnt->z > gobj->max.z) {
                gobj->max.z = pnt->z;
            }
            dist = pnt->x * pnt->x + pnt->y * pnt->y + pnt->z * pnt->z;
            if (dist > gobj->r2) {
                gobj->r2 = dist;
            }
        }
    }
    gobj->r = sqrt(gobj->r2);
    gobj->cntr.x = (gobj->min.x + gobj->max.x) * 0.5f;
    gobj->cntr.y = (gobj->min.y + gobj->max.y) * 0.5f;
    gobj->cntr.z = (gobj->min.z + gobj->max.z) * 0.5f;
    gobj->cntr_r2 = 0.0f;
    for (geom = gobj->geom; geom; geom = geom->next) {
        stride = NuVtxStride(geom->vtxtype);
        vtx = geom->vtxptr;
        if (!vtx) {
            NuErrorProlog(nugobj_file, 0x19B)(err_no_vtxptr);
        }
        end = vtx + stride * geom->vtxcnt;
        for (; vtx < end; vtx += stride) {
            NuVecSub(&v, (struct nuvec_s *)vtx, &gobj->cntr);
            dist = v.x * v.x + v.y * v.y + v.z * v.z;
            if (dist > gobj->cntr_r2) {
                gobj->cntr_r2 = dist;
            }
        }
    }
    gobj->cntr_r = sqrt(gobj->cntr_r2);
}


void NuPrimCalcPlanes(struct nuprim_s *prim, struct nugeom_s *geom) {
    struct nuvtx_tc1_s *vtx;
    int i;
    int n;

    if (prim->pln) {
        NuErrorProlog(nugobj_file, 0x25F)(err_planes_exist);
    }
    n = 0;
    vtx = geom->vtxptr;
    switch (prim->type) {
    case 2:
        prim->pln = NuMemAllocFn(sizeof(struct nuplane_s) * geom->vtxcnt / 3, nugobj_file, 0x266);
        for (i = 0; i < prim->cnt; i += 3) {
            NuPlnEqn(&prim->pln[n++], &vtx[i].pnt, &vtx[i + 1].pnt, &vtx[i + 2].pnt);
        }
        break;
    case 3:
        prim->pln = NuMemAllocFn(sizeof(struct nuplane_s) * geom->vtxcnt - 2, nugobj_file, 0x26D);
        for (i = 0; i < prim->cnt - 2; i++) {
            if (i & 1) {
                NuPlnEqn(&prim->pln[n++], &vtx[i].pnt, &vtx[i + 2].pnt, &vtx[i + 1].pnt);
            } else {
                NuPlnEqn(&prim->pln[n++], &vtx[i].pnt, &vtx[i + 1].pnt, &vtx[i + 2].pnt);
            }
        }
        break;
    case 5:
        prim->pln = NuMemAllocFn(sizeof(struct nuplane_s) * prim->cnt / 3, nugobj_file, 0x279);
        for (i = 0; i < prim->cnt; i += 3) {
            NuPlnEqn(&prim->pln[n++], &vtx[prim->vid[i]].pnt, &vtx[prim->vid[i + 1]].pnt,
                     &vtx[prim->vid[i + 2]].pnt);
        }
        break;
    case 6:
        prim->pln = NuMemAllocFn(sizeof(struct nuplane_s) * prim->cnt - 2, nugobj_file, 0x280);
        for (i = 0; i < prim->cnt - 2; i++) {
            if (i & 1) {
                NuPlnEqn(&prim->pln[n++], &vtx[prim->vid[i]].pnt, &vtx[prim->vid[i + 2]].pnt,
                         &vtx[prim->vid[i + 1]].pnt);
            } else {
                NuPlnEqn(&prim->pln[n++], &vtx[prim->vid[i]].pnt, &vtx[prim->vid[i + 1]].pnt,
                         &vtx[prim->vid[i + 2]].pnt);
            }
        }
        break;
    }
}


u32 *NuCreateStarfield(int count, float radius) {
    union nugiftag_u gif;
    struct nuvec_s star;
    u32 *buf;
    u32 *p;
    int size;
    int n;
    int i;

    size = (count + 41) / 42 * 0x2D8 + 0x10;
    gif.dw[0] = 0x2000400000008000LL;
    gif.dw[1] = 0x51;
    buf = NuMemAllocFn(size, nugobj_file, 0x2C8);
    if (buf) {
        p = vpDmaTag_RetEx(buf, 0, 0);
        while (count > 0) {
            n = (count > 42) ? 42 : count;
            *p++ = ((n + 3) << 16) | 0x6C0080D2;
            gif.tag.nloop = n;
            *p++ = gif.w[0];
            *p++ = gif.w[1];
            *p++ = gif.w[2];
            *p++ = gif.w[3];
            *p++ = gif.w[0];
            *p++ = gif.w[1];
            *p++ = gif.w[2];
            *p++ = gif.w[3];
            *p++ = n;
            *p++ = 0;
            *p++ = 0;
            *p++ = 0;
            for (i = 0; i < n; i++) {
                star.x = 0.0f;
                star.y = 0.0f;
                star.z = radius;
                NuVecRotateX(&star, &star, NuRand(0));
                NuVecRotateY(&star, &star, (int)NuRand(0) * 2);
                *(struct nuvec_s *)p = star;
                p += 3;
                *(float *)p = 255.0f;
                p++;
            }
            *p++ = 0x14000300;
            count -= 42;
        }
        vpDmaTag_Close(p);
    }
    return buf;
}


void NuGobjInit(void) {
    if (sysinit) {
        NuGobjClose();
    }
    sysinit = 1;
    sysgobj = 0;
    sysgobjcount = 0;
}


void NuGobjClose(void) {
    struct nugobj_s *next;

    if (sysinit) {
        while (sysgobj) {
            next = sysgobj->sysnext;
            NuGobjDestroy(sysgobj);
            sysgobj = next;
        }
    }
}


struct nugobj_s *NuGobjCreate(void) {
    struct nugobj_s *gobj;

    gobj = NuMemAllocFn(sizeof(struct nugobj_s), nugobj_file, 0x99);
    memset(gobj, 0, sizeof(struct nugobj_s));
    gobj->sysnext = sysgobj;
    if (sysgobj) {
        sysgobj->syslast = gobj;
    }
    sysgobj = gobj;
    return gobj;
}


void NuGobjAddGeom(struct nugobj_s *gobj, struct nugeom_s *geom) {
    struct nugeom_s *last;
    struct nugeom_s *g;

    last = 0;
    for (g = gobj->geom; g; g = g->next) {
        last = g;
    }
    if (last) {
        last->next = geom;
    } else {
        gobj->geom = geom;
    }
}


void NuGobjAddFaceOnGeom(struct nugobj_s *gobj, struct nufaceongeom_s *geom) {
    struct nufaceongeom_s *last;
    struct nufaceongeom_s *g;

    last = 0;
    for (g = gobj->faceon_geom; g; g = g->next) {
        last = g;
    }
    if (last) {
        last->next = geom;
    } else {
        gobj->faceon_geom = geom;
    }
}


void NuGobjCalcPlanes(struct nugobj_s *gobj) {
    struct nugeom_s *geom;

    for (geom = gobj->geom; geom; geom = geom->next) {
        NuGeomCalcPlanes(geom);
    }
}


int NuGobjApplyCrossFade(struct nugobj_s *gobj, short fade) {
    int count;

    count = 0;
    if (gobj->blend) {
        count = NuGobjColourBlendGobj(gobj->blend, fade);
    }
    if (gobj->faceon_blend) {
        count += NuGobjColourBlendGobj(gobj->faceon_blend, fade);
    }
    return count;
}


struct nugeom_s *NuGeomCreate(void) {
    struct nugeom_s *geom;

    geom = NuMemAllocFn(sizeof(struct nugeom_s), nugobj_file, 0x1B9);
    memset(geom, 0, sizeof(struct nugeom_s));
    geom->id = sysgeomid++;
    return geom;
}


void NuGeomDestroy(struct nugeom_s *geom) {
    struct nuprim_s *prim;
    struct nuprim_s *nextprim;
    struct nuskin_s *nextskin;

    prim = geom->prim;
    if (geom->vtxptr) {
        NuMemFreeFn(geom->vtxptr, nugobj_file, 0x1C8);
    }
    while (prim) {
        nextprim = prim->next;
        NuPrimDestroy(prim);
        prim = nextprim;
    }
    while (geom->skins) {
        nextskin = geom->skins->next;
        if (geom->skins->mtxid) {
            NuMemFreeFn(geom->skins->mtxid, nugobj_file, 0x1D4);
        }
        if (geom->skins->weights) {
            NuMemFreeFn(geom->skins->weights, nugobj_file, 0x1D6);
        }
        NuMemFreeFn(geom->skins, nugobj_file, 0x1D7);
        geom->skins = nextskin;
    }
    if (geom->vtxskininfo) {
        NuMemFreeFn(geom->vtxskininfo, nugobj_file, 0x1DC);
    }
    NuMemFreeFn(geom, nugobj_file, 0x1DF);
}


struct nufaceongeom_s *NuFaceOnGeomCreate(void) {
    struct nufaceongeom_s *geom;

    geom = NuMemAllocFn(sizeof(struct nufaceongeom_s), nugobj_file, 0x1E8);
    memset(geom, 0, sizeof(struct nufaceongeom_s));
    return geom;
}


void NuFaceOnGeomDestroy(struct nufaceongeom_s *geom) {
    if (geom->faceons) {
        NuMemFreeFn(geom->faceons, nugobj_file, 0x1F3);
    }
    NuMemFreeFn(geom, nugobj_file, 0x1F6);
}


void NuGeomCalcPlanes(struct nugeom_s *geom) {
    struct nuprim_s *prim;

    for (prim = geom->prim; prim; prim = prim->next) {
        NuPrimCalcPlanes(prim, geom);
    }
}


void NuGeomAddPrim(struct nugeom_s *geom, struct nuprim_s *prim) {
    struct nuprim_s *last;
    struct nuprim_s *p;

    last = 0;
    for (p = geom->prim; p; p = p->next) {
        last = p;
    }
    if (last) {
        last->next = prim;
    } else {
        geom->prim = prim;
    }
}


void NuGeomAddSkin(struct nugeom_s *geom, struct nuskin_s *skin) {
    struct nuskin_s *last;
    struct nuskin_s *s;

    last = 0;
    for (s = geom->skins; s; s = s->next) {
        last = s;
    }
    if (last) {
        last->next = skin;
    } else {
        geom->skins = skin;
    }
}


struct nuprim_s *NuPrimCreate(int amount, unsigned int type) {
    struct nuprim_s *prim;

    prim = NuMemAllocFn(sizeof(struct nuprim_s), nugobj_file, 0x239);
    memset(prim, 0, sizeof(struct nuprim_s));
    prim->type = type;
    prim->cnt = amount;
    prim->max = amount;
    if (type == 7) {
        prim->vid = NuMemAllocFn(amount * 2, nugobj_file, 0x243);
    } else if (type >= 3) {
        prim->vid = NuMemAllocFn(amount * 2, nugobj_file, 0x247);
    }
    return prim;
}


void NuPrimDestroy(struct nuprim_s *prim) {
    if (prim) {
        if (prim->vid) {
            NuMemFreeFn(prim->vid, nugobj_file, 0x252);
        }
        if (prim->pln) {
            NuMemFreeFn(prim->pln, nugobj_file, 0x253);
        }
        NuMemFreeFn(prim, nugobj_file, 0x254);
    }
}


int NuVtxStride(int type) {
    switch (type) {
    case 0:
        return 0x24;
    case 1:
        return 0x1C;
    }
    return 0;
}


int NuPrimStride(unsigned int type) {
    switch (type) {
    case 4:
        return 4;
    case 5:
        return 6;
    case 7:
        return 0x20;
    }
    return 0;
}
