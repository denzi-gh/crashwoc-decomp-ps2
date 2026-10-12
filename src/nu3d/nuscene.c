/*
 * Unit: nu3d/nuscene
 *
 * Functions:
 *   0x00117f18 ReadNuIFFTextureSet
 *   0x00118180 ReadNuIFFGSplineSet
 *   0x00118360 ReadNuIFFGeom
 *   0x001185d8 ReadNuIFFGeomSkin
 *   0x00118780 ReadNuIFFBlendShape
 *   0x00118900 NuNodeRead
 *   0x00118ca8 ReadNuIFFScene
 *   0x00119198 ReadNuIFFSpecialObjects
 *   0x00119340 ReadNuIFFGScene
 *   0x001199a0 NuSceneLoad
 *   0x00119b10 NuGSceneDestroy
 *   0x00119d30 NuSceneDestroy
 *   0x00119ed8 NuSceneFindNodeByName
 *   0x00119f90 ReadNuIFFMaterialSet
 *   0x0011a070 ReadNuIFFUnknown
 *   0x0011a0c8 NuSceneMtlUpdate
 *   0x0011a198 ReadNuIFFNameTable
 *   0x0011a210 ReadNuIFFGobjSet
 *   0x0011a2f0 ReadNuIFFInstSet
 *   0x0011a478 ReadNuIFFGeomDef
 *   0x0011a4c0 ReadNuIFFGeomVtx
 *   0x0011a5f8 ReadNuIFFGeomCntrl
 *   0x0011a668 ReadNuIFFGeomPrim
 *   0x0011a788 ReadNuIFFGeomTri
 *   0x0011a800 ReadNuIFFGeomBezTri
 *   0x0011a878 ReadNuIFFFaceOnGeom
 */

struct numtl_s {
    char pad000[0x168];
    struct numtl_s *specmtl;        /* 0x168 */
    char pad16C[0x38];
    int tid;                        /* 0x1A4 */
};

struct numtx_s {
    float m[4][4];
}; /* 0x40 */

struct nuanimdata_s;

struct nunode_s {
    char *name;                     /* 0x00 */
    int id;                         /* 0x04 */
    int type;                       /* 0x08 */
    int num_children;               /* 0x0C */
    struct nunode_s *parent;        /* 0x10 */
    struct nunode_s *child;         /* 0x14 */
    struct nunode_s *next;          /* 0x18 */
    void *data;                     /* 0x1C */
}; /* 0x20 */

struct nutransformnode_s {
    struct nunode_s node;           /* 0x00 */
    struct numtx_s mtx;             /* 0x20 */
    struct numtx_s wmtx;            /* 0x60 */
    int flags;                      /* 0xA0 */
    char padA4[0x04];
}; /* 0xA8 */

struct nuskinnode_s {
    struct nunode_s node;           /* 0x00 */
    struct nugobj_s *geometry;      /* 0x20 */
    char pad24[0x04];
    int num_joints;                 /* 0x28 */
    int *joints;                    /* 0x2C */
    struct numtx_s *joint_mtxs;     /* 0x30 */
}; /* 0x34 */

struct nuvec_s {
    float x;
    float y;
    float z;
}; /* 0x0C */

struct nuprim_s {
    char pad00[0x10];
    unsigned short *vid;            /* 0x10 */
};

struct nucntrl_s {
    char pad00[0x0C];
}; /* 0x0C */

struct nublendgeom_s {
    int nblends;                    /* 0x00 */
    struct nuvec_s **blends;        /* 0x04 */
    int *ix;                        /* 0x08 */
    struct nuvec_s *data;           /* 0x0C */
}; /* 0x10 */

struct nuskin_s {
    struct nuskin_s *next;          /* 0x00 */
    int vtxoffset;                  /* 0x04 */
    int vtxcnt;                     /* 0x08 */
    int mtxcnt;                     /* 0x0C */
    int *mtxid;                     /* 0x10 */
    float *weights;                 /* 0x14 */
    char pad18[0x10];
}; /* 0x28 */

struct nugeom_s {
    struct nugeom_s *next;          /* 0x00 */
    struct numtl_s *mtl;            /* 0x04 */
    char pad08[0x0C];
    int mtl_id;                     /* 0x14 */
    char pad18[0x08];
    int vtxcnt;                     /* 0x20 */
    char pad24[0x08];
    struct nuprim_s *prim;          /* 0x2C */
    struct nuskin_s *skins;         /* 0x30 */
    void *vtxskininfo;              /* 0x34 */
    struct nucntrl_s *cntrls;       /* 0x38 */
    int ncntrls;                    /* 0x3C */
    char pad40[0x08];
    struct nublendgeom_s *blendgeom; /* 0x48 */
    int id;                         /* 0x4C */
}; /* 0x50 */

struct nufaceon_s {
    char pad00[0x18];
}; /* 0x18 */

struct nufaceongeom_s {
    struct nufaceongeom_s *next;    /* 0x00 */
    struct numtl_s *mtl;            /* 0x04 */
    int mtl_id;                     /* 0x08 */
    int faceontype;                 /* 0x0C */
    struct nufaceon_s *faceons;     /* 0x10 */
    int nfaceons;                   /* 0x14 */
    float faceonradius;             /* 0x18 */
    int pad1C;
}; /* 0x20 */

struct nutex_s {
    int type;                       /* 0x00 */
    int width;                      /* 0x04 */
    int height;                     /* 0x08 */
    int mmcnt;                      /* 0x0C */
    void *bits;                     /* 0x10 */
    void *pal;                      /* 0x14 */
    int decal;                      /* 0x18 */
    int linear;                     /* 0x1C */
}; /* 0x20 */

struct nugobj_s {
    struct nugobj_s *sysnext;       /* 0x00 */
    struct nugobj_s *syslast;       /* 0x04 */
    int type;                       /* 0x08 */
    struct nugeom_s *geom;          /* 0x0C */
    struct nufaceongeom_s *faceon_geom; /* 0x10 */
    char pad14[0x34];
    int ngobjs;                     /* 0x48 */
    struct nugobj_s *next_gobj;     /* 0x4C */
    struct nuvec_s origin;          /* 0x50 */
    char pad5C[0x04];
    int tricount;                   /* 0x60 */
    char pad64[0x08];
}; /* 0x6C */

struct nusplinedef_s {
    int len;                        /* 0x00 */
    struct nuvec_s *pts;            /* 0x04 */
    char pad08[0x34];
}; /* 0x3C */

struct nujointnode_s {
    struct nunode_s node;           /* 0x00 */
    struct numtx_s orient;          /* 0x20 */
    char pad60[0xA4];
    struct nuvec_s offset;          /* 0x104 */
    char flags;                     /* 0x110 */
};

struct nusplinenode_s {
    struct nunode_s node;           /* 0x00 */
    int num_splines;                /* 0x20 */
    struct nusplinedef_s **splines; /* 0x24 */
    void **spline_data;             /* 0x28 */
}; /* 0x2C */

struct nuscene_s {
    int numnodes;                   /* 0x00 */
    char **names;                   /* 0x04 */
    struct nunode_s **nodes;        /* 0x08 */
    char *nametable;                /* 0x0C */
    int numtids;                    /* 0x10 */
    short *tids;                    /* 0x14 */
    int nummtls;                    /* 0x18 */
    struct numtl_s **mtls;          /* 0x1C */
    int numgobjs;                   /* 0x20 */
    struct nugobj_s **gobjs;        /* 0x24 */
    int numsplines;                 /* 0x28 */
    struct nusplinedef_s *splines;  /* 0x2C */
    struct nuvec_s *splinedata;     /* 0x30 */
    struct nunode_s *root;          /* 0x34 */
    struct nugscn_s *gscene;        /* 0x38 */
}; /* 0x3C */

struct nutexanim_s {
    char pad00[0x20];
}; /* 0x20 */

struct nuinstanim_s {
    struct numtx_s mtx;             /* 0x00 */
    char pad40[0x20];
}; /* 0x60 */

struct nuinstance_s {
    struct numtx_s mtx;             /* 0x00 */
    int objid;                      /* 0x40 */
    int flags;                      /* 0x44 */
    struct nuinstanim_s *anim;      /* 0x48 */
    int pad4C;
}; /* 0x50 */

struct nuspecial_s {
    struct numtx_s mtx;             /* 0x00 */
    struct nuinstance_s *instance;  /* 0x40 */
    char *name;                     /* 0x44 */
    int flags;                      /* 0x48 */
    int pad4C;
}; /* 0x50 */

struct nuspecialfile_s {
    struct numtx_s mtx;             /* 0x00 */
    int instance;                   /* 0x40 */
    int name;                       /* 0x44 */
    int flags;                      /* 0x48 */
    int pad4C;
}; /* 0x50 */

struct nugspline_s {
    short len;                      /* 0x00 */
    short ptsize;                   /* 0x02 */
    char *name;                     /* 0x04 */
    char *pts;                      /* 0x08 */
}; /* 0x0C */

struct nugscn_s {
    int *tids;                      /* 0x00 */
    int numtids;                    /* 0x04 */
    struct numtl_s **mtls;          /* 0x08 */
    int nummtls;                    /* 0x0C */
    int numgobjs;                   /* 0x10 */
    struct nugobj_s **gobjs;        /* 0x14 */
    int numinstances;               /* 0x18 */
    struct nuinstance_s *instances; /* 0x1C */
    int numspecials;                /* 0x20 */
    struct nuspecial_s *specials;   /* 0x24 */
    int *splinedata;                /* 0x28 */
    int numsplines;                 /* 0x2C */
    struct nugspline_s *splines;    /* 0x30 */
    char *nametable;                /* 0x34 */
    int numexspecials;              /* 0x38 */
    struct nuspecial_s *exspecials; /* 0x3C */
    int exallocix;                  /* 0x40 */
    struct nuinstanim_s *instanimdata; /* 0x44 */
    struct nuanimdata_s **animlib;  /* 0x48 */
    int numinstanceixs;             /* 0x4C */
    short *instanceixs;             /* 0x50 */
    char pad54[0x0C];
    int numtexanims;                /* 0x60 */
    struct nutexanim_s *texanims;   /* 0x64 */
    void *texanimbits;              /* 0x68 */
};

typedef void (*nuerror_fn)(char *fmt, ...);
typedef int (*numtlreadevent_fn)(struct numtl_s *mtl);

extern nuerror_fn NuErrorProlog(char *file, int line);
extern nuerror_fn NuDebugMsgProlog(char *file, int line);
extern void *NuMemAllocFn(int size, char *file, int line);
extern void NuMemFreeFn(void *ptr, char *file, int line);
extern void *memset(void *s, int c, unsigned int n);
extern int strcmp(const char *s1, const char *s2);
extern int NuFileRead(int fh, void *buf, int size);
extern int NuFileBeginBlkRead(int fh, int blk);
extern void NuFileEndBlkRead(int fh);
extern int NuTexImgSizeMM(int type, int width, int height, int mmcnt);
extern int NuTexPalSize(int type);
extern void NuTexSwizzelBits(struct nutex_s *tex);
extern int NuTexCreate(struct nutex_s *tex);
extern int FindSplinePtSize(char *name);
extern struct nugobj_s *NuGobjCreate(void);
extern struct nugeom_s *NuGeomCreate(void);
extern struct nufaceongeom_s *NuFaceOnGeomCreate(void);
extern void NuGobjAddGeom(struct nugobj_s *gobj, struct nugeom_s *geom);
extern void NuGobjAddFaceOnGeom(struct nugobj_s *gobj, struct nufaceongeom_s *geom);
extern void NuGobjCalcDims(struct nugobj_s *gobj);
extern void NuPs2CreateRenderStream(struct nugobj_s *gobj);
extern void NuPs2CreateFaceOn(struct nugobj_s *gobj);
extern void NuPs2CreateSkin(struct nugobj_s *gobj);
extern void NuGeomAddSkin(struct nugeom_s *geom, struct nuskin_s *skin);
extern void ReadNuIFFGeomVtx(int fh, struct nugeom_s *geom);
extern void ReadNuIFFGeomPrim(int fh, struct nugeom_s *geom);
void ReadNuIFFGeomCntrl(int fh, struct nugeom_s *geom);
static void ReadNuIFFGeomSkin(int fh, struct nugeom_s *geom);
void ReadNuIFFBlendShape(int fh, struct nugeom_s *geom);
void ReadNuIFFFaceOnGeom(int fh, struct nufaceongeom_s *fgeom);
extern int NuFileReadInt(int fh);
extern unsigned int NuFileReadUnsignedInt(int fh);
extern char NuFileReadChar(int fh);
extern float NuFileReadFloat(int fh);
extern void NuTexDestroy(int tid);
extern void NuMtlDestroy(struct numtl_s *mtl);
extern struct numtl_s *NuMtlRead(int fh);
extern void NuMtlUpdate(struct numtl_s *mtl);
extern void NuGobjDestroy(struct nugobj_s *gobj);
extern void NuNodeDestroyHierarchy(struct nunode_s *node);
extern struct nunode_s *NuNodeCreate(void);
extern struct nunode_s *NuMeshNodeCreate(struct nugobj_s *geometry);
extern struct nutransformnode_s *NuTransformNodeCreate(struct numtx_s *mtx);
extern struct nujointnode_s *NuJointNodeCreate(struct numtx_s *mtx);
extern struct nuskinnode_s *NuSkinNodeCreate(int num_joints);
extern struct nusplinenode_s *NuSplineNodeCreate(int num_splines);
extern void NuNodeAddChild(struct nunode_s *parent, struct nunode_s *child);
extern void NuNodeGraphInit(struct nunode_s *root);
extern int NuFileOpen(char *name, int mode);
extern void NuFileClose(int fh);
extern char *strlwr(char *s);
void ReadNuIFFScene(int fh, struct nuscene_s *scene);
char *ReadNuIFFNameTable(int fh, int *size);
extern struct nuanimdata_s *NuAnimDataRead(int fh);
extern void ReadNuIFFTexAnimSet(int fh, struct nugscn_s *gscene, short *tids);
extern void NuTexAnimDestroy(struct nutexanim_s *texanim);
extern struct nuprim_s *NuPrimCreate(int nvids, int type);
extern void NuGeomAddPrim(struct nugeom_s *geom, struct nuprim_s *prim);
void NuSceneMtlUpdate(struct nuscene_s *scene);
void ReadNuIFFGeomDef(int fh, struct nuscene_s *scene);

extern int show_strip;
extern int info_tricount;
extern int info_tristripcount;
extern struct numtl_s *specmtl;
extern numtlreadevent_fn NuMtlReadEvent;

extern char D_006142C8[];
extern char D_006142E8[];
extern char D_00614308[];
extern char D_00614328[];
extern char D_00614340[];
extern char D_00614358[];
extern char D_00614378[];
extern char D_006143E8[];
extern char D_00614408[];
extern char D_00614428[];
extern char D_00614448[];
#define nuscene_srcfile D_006142C8
#define include_failed_msg D_006142E8
#define reading_texset_msg D_00614308
#define reading_mtlset_msg D_00614328
#define reading_splineset_msg D_00614340
#define reading_gobjset_msg D_00614358
#define gobj_defined_msg D_00614378
#define unknown_node_msg D_006143E8
#define unknown_block_msg D_00614408
#define loading_scene_msg D_00614428
#define scene_open_failed_msg D_00614448


void ReadNuIFFTextureSet(int fh, struct nuscene_s *scene) {
    int count;
    int i;
    int size;
    int j;
    unsigned char *col;
    unsigned char *pal;
    void *bits;
    struct nutex_s tex;
    unsigned char tmp[4];

    NuDebugMsgProlog(nuscene_srcfile, 0x185)(reading_texset_msg);
    NuFileBeginBlkRead(fh, 0x30485354);
    count = NuFileReadInt(fh);
    NuFileEndBlkRead(fh);
    if (count) {
        scene->numtids = 0;
        scene->tids = NuMemAllocFn(count * sizeof(short), nuscene_srcfile, 0x190);
        scene->tids[0] = 0;
        for (i = 0; i < count; i++) {
            NuFileBeginBlkRead(fh, 0x304D5854);
            tex.type = NuFileReadInt(fh);
            tex.width = NuFileReadInt(fh);
            tex.height = NuFileReadInt(fh);
            tex.mmcnt = NuFileReadInt(fh);
            if (tex.mmcnt == 1) {
                tex.mmcnt = 0;
            }
            tex.pal = 0;
            size = NuTexImgSizeMM(tex.type, tex.width, tex.height, tex.mmcnt);
            bits = NuMemAllocFn(size, nuscene_srcfile, 0x1B5);
            tex.bits = bits;
            NuFileRead(fh, bits, size);
            size = NuTexPalSize(tex.type);
            if (size) {
                tex.pal = NuMemAllocFn(size, nuscene_srcfile, 0x1BE);
                NuFileRead(fh, tex.pal, size);
                pal = tex.pal;
                for (j = 0; j < size; j += 4) {
                    col = pal + j;
                    tmp[0] = col[0];
                    tmp[1] = col[1];
                    tmp[2] = col[2];
                    tmp[3] = col[3];
                    col[0] = tmp[3];
                    col[1] = tmp[2];
                    col[2] = tmp[1];
                    col[3] = tmp[0] >> 1;
                }
            }
            NuTexSwizzelBits(&tex);
            scene->tids[scene->numtids++] = NuTexCreate(&tex);
            if (tex.pal) {
                NuMemFreeFn(tex.pal, nuscene_srcfile, 0x1D6);
            }
            NuMemFreeFn(bits, nuscene_srcfile, 0x1D7);
            NuFileEndBlkRead(fh);
        }
    }
}


void ReadNuIFFGSplineSet(int fh, struct nugscn_s *gscene) {
    int size;
    int *data;
    int i;
    int j;
    char *pt;

    NuDebugMsgProlog(nuscene_srcfile, 0x27E)(reading_splineset_msg);
    gscene->numsplines = NuFileReadInt(fh);
    size = NuFileReadInt(fh);
    if (gscene->numsplines) {
        gscene->splinedata = NuMemAllocFn(size, nuscene_srcfile, 0x288);
        data = gscene->splinedata;
        NuFileRead(fh, data, size);
        gscene->splines = NuMemAllocFn(gscene->numsplines * sizeof(struct nugspline_s), nuscene_srcfile, 0x28A);
        for (i = 0; i < gscene->numsplines; i++) {
            gscene->splines[i].len = *data++;
            gscene->splines[i].name = gscene->nametable + *data++;
            gscene->splines[i].ptsize = FindSplinePtSize(gscene->splines[i].name);
            gscene->splines[i].pts = NuMemAllocFn(gscene->splines[i].ptsize * gscene->splines[i].len, nuscene_srcfile, 0x292);
            pt = gscene->splines[i].pts;
            for (j = 0; j < gscene->splines[i].len; j++) {
                *(struct nuvec_s *)pt = *(struct nuvec_s *)data;
                data += 3;
                pt += gscene->splines[i].ptsize;
            }
        }
    }
}


struct nugobj_s *ReadNuIFFGeom(int fh, struct numtl_s **mtls) {
    int count;
    int i;
    int type;
    int n;
    int j;
    int tricount;
    struct nugobj_s *gobj;
    struct nugobj_s *first;
    struct nugobj_s *prev;
    struct nugeom_s *geom;
    struct nufaceongeom_s *fgeom;
    struct nuvec_s origin;

    memset(&origin, 0, sizeof(struct nuvec_s));
    prev = 0;
    count = NuFileReadInt(fh);
    for (i = 0; i < count; i++) {
        gobj = NuGobjCreate();
        if (i == 0) {
            first = gobj;
            first->ngobjs = count;
        } else {
            gobj->ngobjs = 0;
        }
        gobj->type = type = NuFileReadInt(fh);
        switch (type) {
        case 0:
            NuFileRead(fh, &gobj->origin, sizeof(struct nuvec_s));
            n = NuFileReadInt(fh);
            for (j = 0; j < n; j++) {
                geom = NuGeomCreate();
                geom->mtl_id = NuFileReadInt(fh);
                geom->mtl = mtls[geom->mtl_id];
                ReadNuIFFGeomVtx(fh, geom);
                ReadNuIFFGeomCntrl(fh, geom);
                ReadNuIFFGeomPrim(fh, geom);
                ReadNuIFFGeomSkin(fh, geom);
                ReadNuIFFBlendShape(fh, geom);
                NuGobjAddGeom(gobj, geom);
            }
            break;
        case 1:
            n = NuFileReadInt(fh);
            for (j = 0; j < n; j++) {
                gobj->origin = origin;
                fgeom = NuFaceOnGeomCreate();
                fgeom->mtl_id = NuFileReadInt(fh);
                fgeom->mtl = mtls[fgeom->mtl_id];
                ReadNuIFFFaceOnGeom(fh, fgeom);
                NuGobjAddFaceOnGeom(gobj, fgeom);
            }
            break;
        }
        NuGobjCalcDims(gobj);
        if (gobj->geom) {
            if (!gobj->geom->skins) {
                tricount = info_tricount + info_tristripcount;
                NuPs2CreateRenderStream(gobj);
                NuPs2CreateFaceOn(gobj);
                gobj->tricount = info_tricount + info_tristripcount - tricount;
            }
        } else {
            NuPs2CreateFaceOn(gobj);
        }
        NuPs2CreateSkin(gobj);
        if (prev) {
            prev->next_gobj = gobj;
        }
        prev = gobj;
    }
    return first;
}


static void ReadNuIFFGeomSkin(int fh, struct nugeom_s *geom) {
    int count;
    int i;
    int j;
    int size;
    struct nuskin_s *skin;

    count = NuFileReadInt(fh);
    if (count) {
        if (NuFileReadChar(fh)) {
            size = geom->vtxcnt * 16;
            geom->vtxskininfo = NuMemAllocFn(size, nuscene_srcfile, 0x3F8);
            NuFileRead(fh, geom->vtxskininfo, size);
        } else {
            for (i = 0; i < count; i++) {
                skin = NuMemAllocFn(sizeof(struct nuskin_s), nuscene_srcfile, 0x403);
                memset(skin, 0, sizeof(struct nuskin_s));
                skin->vtxoffset = NuFileReadInt(fh);
                skin->vtxcnt = NuFileReadInt(fh);
                skin->mtxcnt = NuFileReadInt(fh);
                skin->mtxid = NuMemAllocFn(skin->mtxcnt * sizeof(int), nuscene_srcfile, 0x408);
                for (j = 0; j < skin->mtxcnt; j++) {
                    skin->mtxid[j] = NuFileReadInt(fh);
                }
                size = skin->vtxcnt * (skin->mtxcnt << 2);
                skin->weights = NuMemAllocFn(size, nuscene_srcfile, 0x40C);
                NuFileRead(fh, skin->weights, size);
                NuGeomAddSkin(geom, skin);
            }
        }
    }
}


void ReadNuIFFBlendShape(int fh, struct nugeom_s *geom) {
    int count;
    int size;
    int i;
    int offset;

    count = NuFileReadInt(fh);
    if (count) {
        geom->blendgeom = NuMemAllocFn(sizeof(struct nublendgeom_s), nuscene_srcfile, 0x41C);
        memset(geom->blendgeom, 0, sizeof(struct nublendgeom_s));
        offset = 0;
        geom->blendgeom->nblends = count;
        geom->blendgeom->blends = NuMemAllocFn(count * sizeof(struct nuvec_s *), nuscene_srcfile, 0x420);
        memset(geom->blendgeom->blends, 0, count * sizeof(struct nuvec_s *));
        geom->blendgeom->ix = NuMemAllocFn(count * sizeof(int), nuscene_srcfile, 0x424);
        NuFileRead(fh, geom->blendgeom->ix, count * sizeof(int));
        size = NuFileReadInt(fh);
        geom->blendgeom->data = NuMemAllocFn(size, nuscene_srcfile, 0x429);
        NuFileRead(fh, geom->blendgeom->data, size);
        for (i = 0; i < count; i++) {
            if (NuFileReadChar(fh)) {
                geom->blendgeom->blends[i] = &geom->blendgeom->data[offset];
                offset += geom->vtxcnt;
            }
        }
    }
}


static struct nunode_s *NuNodeRead(int fh, struct nuscene_s *scene) {
    struct nunode_s *node;
    struct nutransformnode_s *tnode;
    struct nujointnode_s *joint;
    struct nuskinnode_s *skin;
    struct nusplinenode_s *spline;
    struct numtx_s mtx;
    int *joints;
    int gobjix;
    int nameix;
    int nsplines;
    int nchildren;
    int i;
    int j;

    switch (NuFileBeginBlkRead(fh, 0)) {
    case 0x30304E4E:
        node = NuNodeCreate();
        break;
    case 0x30304D4E:
        node = NuMeshNodeCreate(scene->gobjs[NuFileReadInt(fh)]);
        break;
    case 0x3030544E:
        NuFileBeginBlkRead(fh, 0x3044544E);
        NuFileRead(fh, &mtx, sizeof(struct numtx_s));
        tnode = NuTransformNodeCreate(&mtx);
        tnode->flags = NuFileReadInt(fh);
        if (tnode->node.type == 2 && (tnode->flags & 2)) {
            tnode->node.data = (void *)NuFileReadInt(fh);
        }
        NuFileEndBlkRead(fh);
        node = &tnode->node;
        break;
    case 0x304B534E:
        NuFileBeginBlkRead(fh, 0x3044534E);
        gobjix = NuFileReadInt(fh);
        skin = NuSkinNodeCreate(NuFileReadInt(fh));
        joints = skin->joints;
        for (i = 0; i < skin->num_joints; i++) {
            *joints++ = NuFileReadInt(fh);
        }
        NuFileEndBlkRead(fh);
        skin->geometry = scene->gobjs[gobjix];
        node = &skin->node;
        break;
    case 0x30304A4E:
        NuFileBeginBlkRead(fh, 0x30444A4E);
        NuFileRead(fh, &mtx, sizeof(struct numtx_s));
        joint = NuJointNodeCreate(&mtx);
        NuFileRead(fh, &joint->orient, sizeof(struct numtx_s));
        NuFileRead(fh, &joint->flags, sizeof(char));
        NuFileRead(fh, &joint->offset, sizeof(struct nuvec_s));
        NuFileEndBlkRead(fh);
        node = &joint->node;
        break;
    case 0x3030534E:
        NuFileBeginBlkRead(fh, 0x3044534E);
        nsplines = NuFileReadInt(fh);
        spline = NuSplineNodeCreate(nsplines);
        node = &spline->node;
        for (j = 0; j < nsplines; j++) {
            spline->splines[j] = &scene->splines[NuFileReadInt(fh)];
        }
        NuFileEndBlkRead(fh);
        break;
    default:
        NuErrorProlog(nuscene_srcfile, 0x552)(unknown_node_msg);
        break;
    }
    NuFileBeginBlkRead(fh, 0x30444E4E);
    node->id = NuFileReadUnsignedInt(fh);
    nameix = NuFileReadInt(fh);
    if (nameix == 0) {
        node->name = 0;
    } else {
        node->name = scene->names[nameix - 1];
        scene->nodes[nameix - 1] = node;
    }
    NuFileEndBlkRead(fh);
    NuFileEndBlkRead(fh);
    NuFileBeginBlkRead(fh, 0x30434E4E);
    nchildren = NuFileReadInt(fh);
    NuFileEndBlkRead(fh);
    for (j = 0; j < nchildren; j++) {
        NuNodeAddChild(node, NuNodeRead(fh, scene));
    }
    return node;
}


static inline void ReadNuIFFInclude(int fh, struct nuscene_s *scene) {
    char name[0x400];
    int len;
    int ifh;

    len = NuFileReadInt(fh);
    if (len) {
        NuFileRead(fh, name, len);
        name[len] = 0;
        ifh = NuFileOpen(name, 0);
        if (ifh) {
            NuFileBeginBlkRead(ifh, 0x3032554E);
            ReadNuIFFScene(ifh, scene);
            NuFileEndBlkRead(ifh);
        } else {
            NuDebugMsgProlog(nuscene_srcfile, 0x15B)(include_failed_msg, strlwr(name));
        }
    }
}


static inline void ReadNuIFFNodeNames(int fh, struct nuscene_s *scene) {
    int size;
    int i;

    scene->numnodes = NuFileReadInt(fh);
    size = NuFileReadInt(fh);
    scene->names = NuMemAllocFn(scene->numnodes * sizeof(char *), nuscene_srcfile, 0x16C);
    scene->nametable = NuMemAllocFn(size, nuscene_srcfile, 0x16D);
    scene->nodes = NuMemAllocFn(scene->numnodes * sizeof(struct nunode_s *), nuscene_srcfile, 0x16E);
    NuFileRead(fh, scene->nametable, size);
    for (i = 0; i < scene->numnodes; i++) {
        scene->names[i] = scene->nametable + NuFileReadInt(fh);
    }
}


inline void ReadNuIFFMaterialSet(int fh, struct nuscene_s *scene) {
    int count;
    int i;

    NuDebugMsgProlog(nuscene_srcfile, 0x1E6)(reading_mtlset_msg);
    count = NuFileReadInt(fh);
    if (count) {
        scene->mtls = NuMemAllocFn(count * sizeof(struct numtl_s *), nuscene_srcfile, 0x1EF);
        scene->nummtls = count;
        for (i = 0; i < count; i++) {
            scene->mtls[i] = NuMtlRead(fh);
            if (show_strip) {
                scene->mtls[i]->tid = -1;
            }
        }
        NuSceneMtlUpdate(scene);
    }
}


static inline void ReadNuIFFSplineSet(int fh, struct nuscene_s *scene) {
    int count;
    int npts;
    int i;
    struct nuvec_s *pts;

    NuDebugMsgProlog(nuscene_srcfile, 0x246)(reading_splineset_msg);
    count = NuFileReadInt(fh);
    if (count) {
        scene->splines = NuMemAllocFn(count * sizeof(struct nusplinedef_s), nuscene_srcfile, 0x24E);
        scene->numsplines = count;
        NuFileRead(fh, scene->splines, count * sizeof(struct nusplinedef_s));
        npts = NuFileReadInt(fh);
        if (npts) {
            scene->splinedata = NuMemAllocFn(npts * sizeof(struct nuvec_s), nuscene_srcfile, 0x255);
            NuFileRead(fh, scene->splinedata, npts * sizeof(struct nuvec_s));
            pts = scene->splinedata;
            for (i = 0; i < count; i++) {
                scene->splines[i].pts = pts;
                pts += scene->splines[i].len;
            }
        }
    }
}


inline void ReadNuIFFGobjSet(int fh, struct nuscene_s *scene) {
    int count;
    int i;

    NuDebugMsgProlog(nuscene_srcfile, 0x2A4)(reading_gobjset_msg);
    count = NuFileReadInt(fh);
    if (scene->gobjs) {
        NuErrorProlog(nuscene_srcfile, 0x2AB)(gobj_defined_msg);
    }
    scene->gobjs = NuMemAllocFn(count * sizeof(struct nugobj_s *), nuscene_srcfile, 0x2AC);
    memset(scene->gobjs, 0, count * sizeof(struct nugobj_s *));
    scene->numgobjs = 0;
    for (i = 0; i < count; i++) {
        ReadNuIFFGeomDef(fh, scene);
    }
}


inline void ReadNuIFFUnknown(int fh, int blk) {
    NuDebugMsgProlog(nuscene_srcfile, 0x57A)(unknown_block_msg, blk & 0xFF, (blk >> 8) & 0xFF,
                                             (blk >> 16) & 0xFF, (unsigned int)blk >> 24);
}


void ReadNuIFFScene(int fh, struct nuscene_s *scene) {
    int blk;

    while ((blk = NuFileBeginBlkRead(fh, 0)) != 0) {
        switch (blk) {
        case 0x30434E49:
            ReadNuIFFInclude(fh, scene);
            break;
        case 0x30545354:
            ReadNuIFFTextureSet(fh, scene);
            break;
        case 0x3030534D:
            ReadNuIFFMaterialSet(fh, scene);
            break;
        case 0x30545347:
            ReadNuIFFGobjSet(fh, scene);
            break;
        case 0x30545353:
            ReadNuIFFSplineSet(fh, scene);
            break;
        case 0x3054534E:
            ReadNuIFFNodeNames(fh, scene);
            break;
        case 0x30524E4E:
            scene->root = NuNodeRead(fh, scene);
            NuNodeGraphInit(scene->root);
            break;
        default:
            ReadNuIFFUnknown(fh, blk);
            break;
        }
        NuFileEndBlkRead(fh);
    }
}


void ReadNuIFFSpecialObjects(int fh, struct nugscn_s *gscene) {
    struct nuspecialfile_s *data;
    int count;
    int i;

    count = NuFileReadInt(fh);
    gscene->numspecials = count;
    if (count) {
        data = NuMemAllocFn(count * sizeof(struct nuspecialfile_s), nuscene_srcfile, 0x5D8);
        NuFileRead(fh, data, gscene->numspecials * sizeof(struct nuspecialfile_s));
        gscene->specials = NuMemAllocFn(gscene->numspecials * sizeof(struct nuspecial_s), nuscene_srcfile, 0x5DC);
        for (i = 0; i < gscene->numspecials; i++) {
            gscene->specials[i].instance = &gscene->instances[data[i].instance];
            gscene->specials[i].name = gscene->nametable + data[i].name;
            gscene->specials[i].mtx = data[i].mtx;
        }
        NuMemFreeFn(data, nuscene_srcfile, 0x5E5);
    }
}


inline int ReadNuIFFInstSet(int fh, struct nuinstance_s **instances, struct nuinstanim_s **instanimdata) {
    int count;
    int nanims;
    int i;
    struct nuinstance_s *inst;
    struct nuinstanim_s *anim;

    count = NuFileReadInt(fh);
    *instances = NuMemAllocFn(count * sizeof(struct nuinstance_s), nuscene_srcfile, 0x5A6);
    NuFileRead(fh, *instances, count * sizeof(struct nuinstance_s));
    nanims = NuFileReadInt(fh);
    if (nanims) {
        anim = NuMemAllocFn(nanims * sizeof(struct nuinstanim_s), nuscene_srcfile, 0x5AD);
        NuFileRead(fh, anim, nanims * sizeof(struct nuinstanim_s));
        if (instanimdata) {
            *instanimdata = anim;
        }
        for (i = 0; i < count; i++) {
            inst = &(*instances)[i];
            if (inst->anim) {
                inst->anim = anim++;
                inst->anim->mtx = inst->mtx;
            }
        }
    }
    return count;
}


static inline void ReadNuIFFAnimLibrary(int fh, struct nugscn_s *gscene) {
    unsigned char count;
    unsigned char i;

    count = NuFileReadChar(fh);
    if (count) {
        gscene->animlib = NuMemAllocFn(count * sizeof(struct nuanimdata_s *), nuscene_srcfile, 0x26C);
        memset(gscene->animlib, 0, count * sizeof(struct nuanimdata_s *));
        for (i = 0; i < count; i++) {
            if (NuFileReadChar(fh)) {
                gscene->animlib[i] = NuAnimDataRead(fh);
            }
        }
    }
}


static inline void ReadNuIFFInstanceIxs(int fh, struct nugscn_s *gscene) {
    int size;

    gscene->numinstanceixs = NuFileReadInt(fh);
    size = gscene->numinstanceixs * sizeof(short);
    if (size) {
        gscene->instanceixs = NuMemAllocFn(size, nuscene_srcfile, 0x5C8);
        NuFileRead(fh, gscene->instanceixs, size);
    }
}


void ReadNuIFFGScene(int fh, struct nugscn_s *gscene) {
    struct nuscene_s scene;
    int blk;
    int i;

    memset(&scene, 0, sizeof(struct nuscene_s));
    while ((blk = NuFileBeginBlkRead(fh, 0)) != 0) {
        switch (blk) {
        case 0x30434E49:
            ReadNuIFFInclude(fh, &scene);
            break;
        case 0x30545354:
            ReadNuIFFTextureSet(fh, &scene);
            gscene->tids = NuMemAllocFn(scene.numtids * sizeof(int), nuscene_srcfile, 0x5FA);
            for (i = 0; i < scene.numtids; i++) {
                gscene->tids[i] = scene.tids[i];
            }
            gscene->numtids = scene.numtids;
            break;
        case 0x3030534D:
            ReadNuIFFMaterialSet(fh, &scene);
            gscene->mtls = scene.mtls;
            gscene->nummtls = scene.nummtls;
            break;
        case 0x30545347:
            ReadNuIFFGobjSet(fh, &scene);
            break;
        case 0x54534E49:
            gscene->numinstances = ReadNuIFFInstSet(fh, &gscene->instances, &gscene->instanimdata);
            break;
        case 0x44494E49:
            ReadNuIFFInstanceIxs(fh, gscene);
            break;
        case 0x4C42544E:
            gscene->nametable = ReadNuIFFNameTable(fh, 0);
            break;
        case 0x30545353:
            ReadNuIFFGSplineSet(fh, gscene);
            break;
        case 0x42494C41:
            ReadNuIFFAnimLibrary(fh, gscene);
            break;
        case 0x30534154:
            ReadNuIFFTexAnimSet(fh, gscene, scene.tids);
            break;
        default:
            ReadNuIFFUnknown(fh, blk);
            break;
        }
        NuFileEndBlkRead(fh);
    }
    if (scene.tids) {
        NuMemFreeFn(scene.tids, nuscene_srcfile, 0x615);
    }
    scene.tids = 0;
    gscene->numgobjs = scene.numgobjs;
    gscene->gobjs = scene.gobjs;
}


struct nuscene_s *NuSceneLoad(char *filename) {
    struct nuscene_s *scene;
    int fh;
    int blk;

    scene = 0;
    fh = NuFileOpen(filename, 0);
    if (fh) {
        NuDebugMsgProlog(nuscene_srcfile, 0x62F)(loading_scene_msg, strlwr(filename));
        blk = NuFileBeginBlkRead(fh, 0);
        scene = NuMemAllocFn(sizeof(struct nuscene_s), nuscene_srcfile, 0x632);
        memset(scene, 0, sizeof(struct nuscene_s));
        switch (blk) {
        case 0x3032554E:
            ReadNuIFFScene(fh, scene);
            break;
        case 0x30435347:
            scene->gscene = NuMemAllocFn(sizeof(struct nugscn_s), nuscene_srcfile, 0x63D);
            memset(scene->gscene, 0, sizeof(struct nugscn_s));
            ReadNuIFFGScene(fh, scene->gscene);
            break;
        }
        NuFileClose(fh);
    } else {
        NuDebugMsgProlog(nuscene_srcfile, 0x646)(scene_open_failed_msg, strlwr(filename));
    }
    return scene;
}


void NuGSceneDestroy(struct nugscn_s *gscene) {
    int i;

    for (i = 0; i < gscene->numtids; i++) {
        NuTexDestroy(gscene->tids[i]);
    }
    NuMemFreeFn(gscene->tids, nuscene_srcfile, 0x67E);
    for (i = 0; i < gscene->nummtls; i++) {
        NuMtlDestroy(gscene->mtls[i]);
    }
    NuMemFreeFn(gscene->mtls, nuscene_srcfile, 0x683);
    for (i = 0; i < gscene->numgobjs; i++) {
        NuGobjDestroy(gscene->gobjs[i]);
    }
    NuMemFreeFn(gscene->gobjs, nuscene_srcfile, 0x688);
    if (gscene->instances) {
        NuMemFreeFn(gscene->instances, nuscene_srcfile, 0x68C);
    }
    if (gscene->instances) {
        NuMemFreeFn(gscene->instances, nuscene_srcfile, 0x690);
    }
    if (gscene->instanimdata) {
        NuMemFreeFn(gscene->instanimdata, nuscene_srcfile, 0x694);
    }
    if (gscene->animlib) {
        NuMemFreeFn(gscene->animlib, nuscene_srcfile, 0x697);
    }
    if (gscene->numsplines) {
        NuMemFreeFn(gscene->splinedata, nuscene_srcfile, 0x69C);
        NuMemFreeFn(gscene->splines, nuscene_srcfile, 0x69D);
    }
    if (gscene->nametable) {
        NuMemFreeFn(gscene->nametable, nuscene_srcfile, 0x6A2);
    }
    if (gscene->specials) {
        NuMemFreeFn(gscene->exspecials, nuscene_srcfile, 0x6A6);
    }
    for (i = 0; i < gscene->numtexanims; i++) {
        NuTexAnimDestroy(&gscene->texanims[i]);
    }
    NuMemFreeFn(gscene->texanims, nuscene_srcfile, 0x6AB);
    NuMemFreeFn(gscene->texanimbits, nuscene_srcfile, 0x6AC);
    NuMemFreeFn(gscene, nuscene_srcfile, 0x6AF);
}


void NuSceneDestroy(struct nuscene_s *scene) {
    int i;

    NuMemFreeFn(scene->names, nuscene_srcfile, 0x6BB);
    NuMemFreeFn(scene->nodes, nuscene_srcfile, 0x6BC);
    NuMemFreeFn(scene->nametable, nuscene_srcfile, 0x6BD);
    if (scene->tids) {
        for (i = 0; i < scene->numtids; i++) {
            NuTexDestroy(scene->tids[i]);
        }
        NuMemFreeFn(scene->tids, nuscene_srcfile, 0x6C4);
    }
    for (i = 0; i < scene->nummtls; i++) {
        NuMtlDestroy(scene->mtls[i]);
    }
    if (scene->mtls) {
        NuMemFreeFn(scene->mtls, nuscene_srcfile, 0x6CA);
    }
    for (i = 0; i < scene->numgobjs; i++) {
        NuGobjDestroy(scene->gobjs[i]);
    }
    if (scene->gobjs) {
        NuMemFreeFn(scene->gobjs, nuscene_srcfile, 0x6D0);
    }
    if (scene->splines) {
        NuMemFreeFn(scene->splines, nuscene_srcfile, 0x6D4);
    }
    if (scene->splinedata) {
        NuMemFreeFn(scene->splinedata, nuscene_srcfile, 0x6D6);
    }
    NuNodeDestroyHierarchy(scene->root);
    if (scene->gscene) {
        NuGSceneDestroy(scene->gscene);
    }
    NuMemFreeFn(scene, nuscene_srcfile, 0x6E2);
}


struct nunode_s *NuSceneFindNodeByName(struct nuscene_s *scene, char *name) {
    int lo;
    int hi;
    int mid;
    int cmp;

    lo = 0;
    hi = scene->numnodes - 1;
    do {
        mid = (lo + hi) / 2;
        cmp = strcmp(name, scene->names[mid]);
        if (cmp == 0) {
            break;
        }
        if (cmp < 0) {
            hi = mid - 1;
        } else {
            lo = mid + 1;
        }
    } while (lo <= hi);
    if (cmp != 0) {
        return 0;
    }
    return scene->nodes[mid];
}


void NuSceneMtlUpdate(struct nuscene_s *scene) {
    struct numtl_s *mtl;
    int i;

    for (i = 0; i < scene->nummtls; i++) {
        mtl = scene->mtls[i];
        if (mtl->tid == -1) {
            mtl->tid = 0;
        } else {
            mtl->tid = scene->tids[mtl->tid];
        }
        scene->mtls[i]->specmtl = specmtl;
        if (!NuMtlReadEvent(scene->mtls[i])) {
            NuMtlUpdate(scene->mtls[i]);
        }
    }
}


char *ReadNuIFFNameTable(int fh, int *size) {
    int tablesize;
    char *table;

    tablesize = NuFileReadInt(fh);
    table = NuMemAllocFn(tablesize, nuscene_srcfile, 0x6EF);
    NuFileRead(fh, table, tablesize);
    if (size) {
        *size = tablesize;
    }
    return table;
}


void ReadNuIFFGeomDef(int fh, struct nuscene_s *scene) {
    struct nugobj_s *gobj;

    gobj = ReadNuIFFGeom(fh, scene->mtls);
    scene->gobjs[scene->numgobjs++] = gobj;
}


void ReadNuIFFGeomCntrl(int fh, struct nugeom_s *geom) {
    int count;
    int size;

    count = NuFileReadInt(fh);
    geom->ncntrls = count;
    if (count) {
        size = count * sizeof(struct nucntrl_s);
        geom->cntrls = NuMemAllocFn(size, nuscene_srcfile, 0x398);
        NuFileRead(fh, geom->cntrls, size);
    }
}


void ReadNuIFFGeomTri(int fh, struct nugeom_s *geom, int type) {
    int nvids;
    struct nuprim_s *prim;

    nvids = NuFileReadInt(fh);
    prim = NuPrimCreate(nvids, type);
    NuFileRead(fh, prim->vid, nvids * sizeof(unsigned short));
    NuGeomAddPrim(geom, prim);
}


void ReadNuIFFGeomBezTri(int fh, struct nugeom_s *geom) {
    int nvids;
    int ntris;
    struct nuprim_s *prim;

    nvids = NuFileReadInt(fh);
    prim = NuPrimCreate(nvids, 8);
    ntris = nvids / 6;
    NuFileRead(fh, prim->vid, nvids * sizeof(unsigned short));
    NuGeomAddPrim(geom, prim);
}


void ReadNuIFFFaceOnGeom(int fh, struct nufaceongeom_s *fgeom) {
    int size;

    fgeom->nfaceons = NuFileReadInt(fh);
    fgeom->faceontype = NuFileReadInt(fh);
    fgeom->faceonradius = NuFileReadFloat(fh);
    size = fgeom->nfaceons * sizeof(struct nufaceon_s);
    fgeom->faceons = NuMemAllocFn(size, nuscene_srcfile, 0x3AA);
    NuFileRead(fh, fgeom->faceons, size);
}
