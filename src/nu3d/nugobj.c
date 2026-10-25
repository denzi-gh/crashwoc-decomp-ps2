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

#include "nu_libc.h"
#include "nu_types.h"

#include "nucore/nuerror.h"
#include "numath/numathtypes.h"
#include "numath/nuplane.h"
#include "numath/nurand.h"
#include "numath/nuvec.h"
#include "nu3d/nugobj.h"

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

extern void *NuMemAllocFn(int size, char *file, int line);
extern void NuMemFreeFn(void *ptr, char *file, int line);
extern u32 *vpDmaTag_RetEx(void *buf, u32 vif0, u32 vif1);
extern void *vpDmaTag_Close(void *end);
extern void NuPs2DestroyRenderStream(struct nugobj_s *gobj);
extern int NuGobjColourBlendGobj(struct nucolourref_s *blend, short fade);

static int sysinit = 0;
static struct nugobj_s *sysgobj = 0;
static int hasHWTnL = 0;


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
                NuMemFreeFn(geom->blendgeom->blend_offsets, "..\\nu2crash.ps2\\nu3d\\nugobj.c", 0xBF);
            }
            if (geom->blendgeom->offsets) {
                NuMemFreeFn(geom->blendgeom->offsets, "..\\nu2crash.ps2\\nu3d\\nugobj.c", 0xC2);
            }
            if (geom->blendgeom->ix) {
                NuMemFreeFn(geom->blendgeom->ix, "..\\nu2crash.ps2\\nu3d\\nugobj.c", 0xC5);
            }
            NuMemFreeFn(geom->blendgeom, "..\\nu2crash.ps2\\nu3d\\nugobj.c", 0xC7);
        }
        NuGeomDestroy(geom);
        geom = nextgeom;
    }
    if (faceon && faceon->dmastream) {
        NuMemFreeFn(faceon->dmastream, "..\\nu2crash.ps2\\nu3d\\nugobj.c", 0xD1);
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
    NuMemFreeFn(gobj, "..\\nu2crash.ps2\\nu3d\\nugobj.c", 0xE1);
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

    lo = 3.4028235e38f;
    hi = -3.4028235e38f;
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
    lo = 3.4028235e38f;
    hi = -3.4028235e38f;
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
            NuErrorProlog("..\\nu2crash.ps2\\nu3d\\nugobj.c", 0x180)(
                "NuGobjCalcDims : Lock VB failed!");
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
            NuErrorProlog("..\\nu2crash.ps2\\nu3d\\nugobj.c", 0x19B)(
                "NuGobjCalcDims : Lock VB failed!");
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


static int geomid = 0;


void NuPrimCalcPlanes(struct nuprim_s *prim, struct nugeom_s *geom) {
    struct nuvtx_tc1_s *vtx;
    int i;
    int n;

    if (prim->pln) {
        NuErrorProlog("..\\nu2crash.ps2\\nu3d\\nugobj.c", 0x25F)("assert");
    }
    n = 0;
    vtx = geom->vtxptr;
    switch (prim->type) {
    case 2:
        prim->pln = NuMemAllocFn(sizeof(struct nuplane_s) * geom->vtxcnt / 3, "..\\nu2crash.ps2\\nu3d\\nugobj.c", 0x266);
        for (i = 0; i < prim->cnt; i += 3) {
            NuPlnEqn(&prim->pln[n++], &vtx[i].pnt, &vtx[i + 1].pnt, &vtx[i + 2].pnt);
        }
        break;
    case 3:
        prim->pln = NuMemAllocFn(sizeof(struct nuplane_s) * geom->vtxcnt - 2, "..\\nu2crash.ps2\\nu3d\\nugobj.c", 0x26D);
        for (i = 0; i < prim->cnt - 2; i++) {
            if (i & 1) {
                NuPlnEqn(&prim->pln[n++], &vtx[i].pnt, &vtx[i + 2].pnt, &vtx[i + 1].pnt);
            } else {
                NuPlnEqn(&prim->pln[n++], &vtx[i].pnt, &vtx[i + 1].pnt, &vtx[i + 2].pnt);
            }
        }
        break;
    case 5:
        prim->pln = NuMemAllocFn(sizeof(struct nuplane_s) * prim->cnt / 3, "..\\nu2crash.ps2\\nu3d\\nugobj.c", 0x279);
        for (i = 0; i < prim->cnt; i += 3) {
            NuPlnEqn(&prim->pln[n++], &vtx[prim->vid[i]].pnt, &vtx[prim->vid[i + 1]].pnt,
                     &vtx[prim->vid[i + 2]].pnt);
        }
        break;
    case 6:
        prim->pln = NuMemAllocFn(sizeof(struct nuplane_s) * prim->cnt - 2, "..\\nu2crash.ps2\\nu3d\\nugobj.c", 0x280);
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
    buf = NuMemAllocFn(size, "..\\nu2crash.ps2\\nu3d\\nugobj.c", 0x2C8);
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
    hasHWTnL = 0;
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

    gobj = NuMemAllocFn(sizeof(struct nugobj_s), "..\\nu2crash.ps2\\nu3d\\nugobj.c", 0x99);
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

    geom = NuMemAllocFn(sizeof(struct nugeom_s), "..\\nu2crash.ps2\\nu3d\\nugobj.c", 0x1B9);
    memset(geom, 0, sizeof(struct nugeom_s));
    geom->id = geomid++;
    return geom;
}


void NuGeomDestroy(struct nugeom_s *geom) {
    struct nuprim_s *prim;
    struct nuprim_s *nextprim;
    struct nuskin_s *nextskin;

    prim = geom->prim;
    if (geom->vtxptr) {
        NuMemFreeFn(geom->vtxptr, "..\\nu2crash.ps2\\nu3d\\nugobj.c", 0x1C8);
    }
    while (prim) {
        nextprim = prim->next;
        NuPrimDestroy(prim);
        prim = nextprim;
    }
    while (geom->skin) {
        nextskin = geom->skin->next;
        if (geom->skin->mtxid) {
            NuMemFreeFn(geom->skin->mtxid, "..\\nu2crash.ps2\\nu3d\\nugobj.c", 0x1D4);
        }
        if (geom->skin->weights) {
            NuMemFreeFn(geom->skin->weights, "..\\nu2crash.ps2\\nu3d\\nugobj.c", 0x1D6);
        }
        NuMemFreeFn(geom->skin, "..\\nu2crash.ps2\\nu3d\\nugobj.c", 0x1D7);
        geom->skin = nextskin;
    }
    if (geom->vtxskininfo) {
        NuMemFreeFn(geom->vtxskininfo, "..\\nu2crash.ps2\\nu3d\\nugobj.c", 0x1DC);
    }
    NuMemFreeFn(geom, "..\\nu2crash.ps2\\nu3d\\nugobj.c", 0x1DF);
}


struct nufaceongeom_s *NuFaceOnGeomCreate(void) {
    struct nufaceongeom_s *geom;

    /* Retail allocates a nugeom_s here (GC does the same). */
    geom = NuMemAllocFn(sizeof(struct nugeom_s), "..\\nu2crash.ps2\\nu3d\\nugobj.c", 0x1E8);
    memset(geom, 0, sizeof(struct nugeom_s));
    return geom;
}


void NuFaceOnGeomDestroy(struct nufaceongeom_s *geom) {
    if (geom->faceons) {
        NuMemFreeFn(geom->faceons, "..\\nu2crash.ps2\\nu3d\\nugobj.c", 0x1F3);
    }
    NuMemFreeFn(geom, "..\\nu2crash.ps2\\nu3d\\nugobj.c", 0x1F6);
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
    for (s = geom->skin; s; s = s->next) {
        last = s;
    }
    if (last) {
        last->next = skin;
    } else {
        geom->skin = skin;
    }
}


struct nuprim_s *NuPrimCreate(int amount, unsigned int type) {
    struct nuprim_s *prim;

    prim = NuMemAllocFn(sizeof(struct nuprim_s), "..\\nu2crash.ps2\\nu3d\\nugobj.c", 0x239);
    memset(prim, 0, sizeof(struct nuprim_s));
    prim->type = type;
    prim->cnt = amount;
    prim->max = amount;
    if (type == 7) {
        prim->vid = NuMemAllocFn(amount * 2, "..\\nu2crash.ps2\\nu3d\\nugobj.c", 0x243);
    } else if (type >= 3) {
        prim->vid = NuMemAllocFn(amount * 2, "..\\nu2crash.ps2\\nu3d\\nugobj.c", 0x247);
    }
    return prim;
}


void NuPrimDestroy(struct nuprim_s *prim) {
    if (prim) {
        if (prim->vid) {
            NuMemFreeFn(prim->vid, "..\\nu2crash.ps2\\nu3d\\nugobj.c", 0x252);
        }
        if (prim->pln) {
            NuMemFreeFn(prim->pln, "..\\nu2crash.ps2\\nu3d\\nugobj.c", 0x253);
        }
        NuMemFreeFn(prim, "..\\nu2crash.ps2\\nu3d\\nugobj.c", 0x254);
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
