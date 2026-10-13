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

struct nuprim_s {
    struct nuprim_s *next;          /* 0x00 */
};

struct nuskin_s {
    struct nuskin_s *next;          /* 0x00 */
};

struct nugeom_s {
    struct nugeom_s *next;          /* 0x00 */
    char pad04[0x28];
    struct nuprim_s *prim;          /* 0x2C */
    struct nuskin_s *skins;         /* 0x30 */
    char pad34[0x18];
    int id;                         /* 0x4C */
}; /* 0x50 */

struct nufaceongeom_s {
    struct nufaceongeom_s *next;    /* 0x00 */
};

struct nucolourblend_s;

struct nugobj_s {
    struct nugobj_s *sysnext;       /* 0x00 */
    struct nugobj_s *syslast;       /* 0x04 */
    char pad08[0x04];
    struct nugeom_s *geom;          /* 0x0C */
    struct nufaceongeom_s *faceon_geom; /* 0x10 */
    char pad14[0x48];
    struct nucolourblend_s *blend;  /* 0x5C */
    char pad60[0x04];
    struct nucolourblend_s *faceon_blend; /* 0x64 */
    char pad68[0x04];
}; /* 0x6C */

extern void *NuMemAllocFn(int size, char *file, int line);
extern void *memset(void *s, int c, unsigned int n);
extern void NuGobjDestroy(struct nugobj_s *gobj);
void NuGobjClose(void);
extern void NuGeomCalcPlanes(struct nugeom_s *geom);
extern int NuGobjColourBlendGobj(struct nucolourblend_s *blend, short fade);

extern char D_00614198[];
extern int D_0062EB60;
extern struct nugobj_s *D_0062EB64;
extern int D_0062EB68;
extern int D_0062EB7C;

#define nugobj_file D_00614198
#define sysinit D_0062EB60
#define sysgobj D_0062EB64
#define sysgobjcount D_0062EB68
#define sysgeomid D_0062EB7C


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
