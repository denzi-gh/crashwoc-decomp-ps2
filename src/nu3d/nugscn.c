/*
 * Unit: nu3d/nugscn
 *
 * Functions:
 *   0x001453b8 CvtNuIFFTexAnimSet
 *   0x00145508 CvtNuIFFTextureSet
 *   0x00145890 CvtNuIFFSplineSet
 *   0x00145998 CalcDimsAddFaceOn
 *   0x00145aa0 CvtNuIFFGeomDef
 *   0x00145e30 ReadNuIFFGeomSkin
 *   0x00145fd0 ReadNuIFFGeomVtx
 *   0x001461b0 CvtNuIFFGeomPrim
 *   0x00146350 MakeFaceOnDma
 *   0x001464b8 CvtNuIFFGeomTri
 *   0x00146728 CvtNuIFFGeomTriStrip
 *   0x001469e8 CvtNuIFFInstSet
 *   0x00146b50 CvtNuIFFAnimationLibrary
 *   0x001470e0 CvtNuIFFScene
 *   0x001475b8 CvtNuIFFGScene
 *   0x00147b50 NuGScnConvertScene
 *   0x00147dd0 NuNodeRead
 *   0x00148128 CvtNodes
 *   0x00148528 ReadTextureSet
 *   0x001486a0 ReadMaterialSet
 *   0x00148928 NuGScnDuplicate
 *   0x00148c90 ReadObjSet
 *   0x00149690 ReadTexAnimSet
 *   0x00149918 ReadNuIFFTexAnimSet
 *   0x00149b40 ReadAnimationLibrary
 *   0x00149d50 ReadSplineSet
 *   0x0014a060 DbgBlkType
 *   0x0014a258 NuGScnLoadFix
 *   0x0014a958 NuGScnUpdate
 *   0x0014aca8 NuGScnRndr3
 *   0x0014af00 NuGScnRndr2
 *   0x0014b428 NuGScnRndr
 *   0x0014b670 NuSpecialSetAttr
 *   0x0014baf0 NuGScnSetTmpWkspc
 *   0x0014bb00 NuGScnSetCfg
 *   0x0014bb08 NuGScnRemove
 *   0x0014bbd0 NuGScnLoad
 *   0x0014bcb8 NuGScnRead
 *   0x0014bdb8 NuGScnTranslate
 *   0x0014be20 NuGScnAddExt
 *   0x0014be60 NuExtSpecialDuplicate
 *   0x0014bf98 NuExtSpecialDelete
 *   0x0014bfb0 NuGSceneSetCrossFade
 *   0x0014c000 NuGSceneProcessCrossFade
 *   0x0014c098 NuGScnMtlUpdate
 *   0x0014c0a0 NuSpecialFind
 *   0x0014c130 NuSpecialGetPos
 *   0x0014c160 NuSpecialGetMtx
 *   0x0014c1f8 NuSpecialDrawAt
 *   0x0014c230 NuSplineFind
 *   0x0014c2b8 NuSplineFindAllSub
 *   0x0014c370 NuSplineGetRandomPoint
 *   0x0014c428 NuSceneReAssignTexAnims
 *   0x0014c488 CvtNuIFFGeom
 *   0x0014c5f0 CvtNuIFFFaceOnGeom
 *   0x0014c6d8 CvtNuIFFNodeHierarchy
 *   0x0014c768 FindSplinePtSize
 */

struct numtx_s {
    float _00, _01, _02, _03;
    float _10, _11, _12, _13;
    float _20, _21, _22, _23;
    float _30, _31, _32, _33;
}; /* 0x40 */

struct nuvec_s {
    float x;
    float y;
    float z;
};

struct nufaceon_s {
    struct nuvec_s point;           /* 0x00 */
    float width;                    /* 0x0C */
    float height;                   /* 0x10 */
    int colour;                     /* 0x14 */
}; /* 0x18 */

struct nufaceongeom_s {
    char pad00[0x0C];
    int faceontype;                 /* 0x0C */
    struct nufaceon_s *faceons;     /* 0x10 */
    int nfaceons;                   /* 0x14 */
    float faceonradius;             /* 0x18 */
    int pad1C;
}; /* 0x20 */

struct nuinstance_s {
    struct numtx_s matrix;          /* 0x00 */
    int objid;                      /* 0x40 */
    char pad44[0x0C];
}; /* 0x50 */

struct nuspecial_s {
    struct numtx_s mtx;             /* 0x00 */
    struct nuinstance_s *instance;  /* 0x40 */
    char *name;                     /* 0x44 */
    int flags;                      /* 0x48: bit 0 = ext (own matrix) */
    int pad;                        /* 0x4C */
}; /* 0x50 */

struct nugspline_s {
    short len;                      /* 0x00 */
    short ptsize;                   /* 0x02 */
    char *name;                     /* 0x04 */
    struct nuvec_s *pts;            /* 0x08 */
}; /* 0x0C */

struct nugscn_s {
    int *tids;                      /* 0x00 */
    int numtid;                     /* 0x04 */
    struct numtl_s **mtls;          /* 0x08 */
    int nummtl;                     /* 0x0C */
    int numgobj;                    /* 0x10 */
    struct nugobj_s **gobjs;        /* 0x14 */
    int numinstance;                /* 0x18 */
    struct nuinstance_s *instances; /* 0x1C */
    int numspecial;                 /* 0x20 */
    struct nuspecial_s *specials;   /* 0x24 */
    char pad28[0x04];
    int numsplines;                 /* 0x2C */
    struct nugspline_s *splines;    /* 0x30 */
    char pad34[0x04];
    int numexspecials;              /* 0x38 */
    struct nuspecial_s *exspecials; /* 0x3C */
    int exallocix;                  /* 0x40 */
    char pad44[0x04];
    struct nuanimdata_s **animlib;  /* 0x48 */
    char pad4C[0x08];
    short crossfade;                /* 0x54 */
    short crossfaderate;            /* 0x56 */
    int crossfadegobj;              /* 0x58 */
    int crossfadefirst;             /* 0x5C */
    char pad60[0x04];
    struct nutexanim_s *texanims;   /* 0x64 */
    char pad68[0x04];
}; /* 0x6C */

struct nuhspecial_s {
    struct nugscn_s *scene;         /* 0x00 */
    struct nuspecial_s *special;    /* 0x04 */
};

struct nuanimcurve_s {
    unsigned char mask[4];          /* 0x00 */
    void *animkeys;                 /* 0x04 */
    unsigned int numkeys;           /* 0x08 */
    unsigned int flags;             /* 0x0C */
};

struct nuanimcurveset_s {
    int flags;                      /* 0x00 */
    float *constants;               /* 0x04 */
    struct nuanimcurve_s **set;     /* 0x08 */
    signed char ncurves;            /* 0x0C */
};

struct nuanimdatachunk_s {
    int numnodes;                   /* 0x00 */
    int num_valid_animcurvesets;    /* 0x04 */
    struct nuanimcurveset_s **animcurvesets; /* 0x08 */
};

struct nuanimdata_s {
    float time;                     /* 0x00 */
    char *node_names;               /* 0x04 */
    int nchunks;                    /* 0x08 */
    struct nuanimdatachunk_s **chunks; /* 0x0C */
};

struct nutexanimenv_s {
    void *prog;                     /* 0x00 */
};

struct nutexanim_s {
    char pad00[0x14];
    struct nutexanimenv_s *env;     /* 0x14 */
    char *ntaname;                  /* 0x18 */
    char *scriptname;               /* 0x1C */
}; /* 0x20 */

struct nusplinecfg_s {
    char *name;                     /* 0x00 */
    int ptsize;                     /* 0x04 */
};

extern void *memset(void *s, int c, unsigned int n);
extern char *strstr(const char *haystack, const char *needle);
extern void NuMtxTranslate(struct numtx_s *m, struct nuvec_s *v);
extern int NuRndrGScnObj(struct nugobj_s *gobj, struct numtx_s *m);
extern void *NuTexAnimProgFind(char *name);
extern void NuTexAnimEnvReset(struct nutexanimenv_s *env);
extern void NuTexAnimRemoveList(struct nutexanim_s *list);
extern void NuMtlDestroy(struct numtl_s *mtl);
extern void NuTexDestroy(int tid);
extern int NuGobjApplyCrossFade(struct nugobj_s *gobj, short crossfade);

extern void *NuMemAllocFn(int size, char *file, int line);
extern void NuMemFreeFn(void *ptr, char *file, int line);
extern int NuFileRead(int fh, void *buf, int size);
extern int NuFileReadInt(int fh);
extern float NuFileReadFloat(int fh);
extern void NuFileBeginBlkWrite(int fh, int blk, int flags);
extern void NuFileEndBlkWrite(int fh);
extern void NuFileWrite(int fh, void *buf, int size);
extern void NuFileWriteInt(int fh, int value);
extern void NuFileWriteFloat(int fh, float value);
extern struct nunode_s *NuNodeRead(int fh, int depth);
extern void NuNodeGraphInit(struct nunode_s *root);
extern void CvtNodes(struct nunode_s *node, int depth);
extern void MakeFaceOnDma(int fh, struct nufaceongeom_s *geom, int mtlid);

extern int strcasecmp(const char *s1, const char *s2);
extern char *NuStrIStr(char *str, char *sub);
extern int rand(void);
extern void NuVecLerp(struct nuvec_s *out, struct nuvec_s *a, struct nuvec_s *b, float t);

struct nucvtmtl_s {
    char pad00[0x1D0];
}; /* 0x1D0 */

extern int NuFilePos(int fh);
extern void *NuMemFileAddr(int fh);
extern void NuCvtGeoSetColourRefOffset(int offset);
extern void NuCvtDmaFaceOn(char **dma, struct nufaceon_s *faceons, int nfaceons, struct nucvtmtl_s *mtl);
extern void NuCvtDmaFaceY(char **dma, struct nufaceon_s *faceons, int nfaceons, struct nucvtmtl_s *mtl);
extern char *vpDmaTag_Ret(char *dma);

extern int verts_this_frame;

typedef void (*nuerror_fn)(char *fmt, ...);
extern nuerror_fn NuErrorProlog(char *file, int line);
extern int NuFileLoadBuffer(char *name, void *buf, int size);
extern struct nugscn_s *NuGScnLoadFix(void);

struct nugscnload_s {
    int size;                       /* 0x00 */
    char *filebuffer;               /* 0x04 */
    char **buffer;                  /* 0x08 */
    char *bufferend;                /* 0x0C */
    char *filename;                 /* 0x10 */
};

extern char *D_00633098;
extern char **D_0063309C;
extern char *D_006330A0;
extern char *D_006330A8;
extern char *D_006330C0;
extern struct nugscnload_s D_006ADD00;
extern char D_00614D38[];
#define gscn_bufferstart D_00633098
#define gscn_buffer D_0063309C
#define gscn_bufferlimit D_006330A0
#define gscn_bufferend D_006330A8
#define gscn_filebuffer D_006330C0
#define gscn_load D_006ADD00
#define bufferalloc_failed_msg D_00614D38

extern int D_006330C4;
extern struct nuinstance_s *D_006330E4;
extern int D_006330E8;
#define cvt_fh D_006330C4
#define cvt_instances D_006330E4
#define cvt_numinstances D_006330E8

extern struct nucvtmtl_s *D_006330C8;
extern char *D_006330D8;
extern char *D_006330E0;
extern char D_00614E68[];
extern char D_00614E98[];
extern char D_00614EB0[];
extern struct nugscn_s *D_00633114;
#define loading_scene D_00633114

#define cvt_mtls D_006330C8
#define cvt_dmaptr D_006330D8
#define cvt_dmabuffer D_006330E0
#define no_dmabuffer_msg D_00614E68
#define dma_overflow_msg D_00614E98
#define unknown_faceon_msg D_00614EB0

typedef void (*nudebugmsg_fn)(char *fmt, ...);
extern nudebugmsg_fn NuDebugMsgProlog(char *file, int line);

extern char D_00614D00[];
extern char D_00615130[];
#define nugscn_srcfile D_00614D00
#define removed_mtls_msg D_00615130

extern struct nuvec_s nugscn_gobj_min;
extern struct nuvec_s nugscn_gobj_max;

extern void *D_006330B0;
extern int D_006330B8;
extern struct nusplinecfg_s *D_0062EE58;
#define temp_workspace D_006330B0
#define temp_workspace_size D_006330B8
#define spline_cfg D_0062EE58

#define ALIGN16(p) ((char *)(((unsigned int)(p) + 0xF) & 0xFFFFFFF0))
#define RELOCATE(p) ((p) = ((p) != 0) ? (void *)((char *)&(p) + (int)(p)) : 0)


void CalcDimsAddFaceOn(struct nufaceongeom_s *geom)
{
    struct nufaceon_s *faceon;
    float size;
    int i;

    for (i = 0; i < geom->nfaceons; i++) {
        faceon = &geom->faceons[i];
        size = faceon->width;
        if (size < faceon->height) {
            size = faceon->height;
        }
        if (faceon->point.x - size < nugscn_gobj_min.x) {
            nugscn_gobj_min.x = faceon->point.x - size;
        }
        if (faceon->point.y - size < nugscn_gobj_min.y) {
            nugscn_gobj_min.y = faceon->point.y - size;
        }
        if (faceon->point.z - size < nugscn_gobj_min.z) {
            nugscn_gobj_min.z = faceon->point.z - size;
        }
        if (faceon->point.x + size > nugscn_gobj_max.x) {
            nugscn_gobj_max.x = faceon->point.x + size;
        }
        if (faceon->point.y + size > nugscn_gobj_max.y) {
            nugscn_gobj_max.y = faceon->point.y + size;
        }
        if (faceon->point.z + size > nugscn_gobj_max.z) {
            nugscn_gobj_max.z = faceon->point.z + size;
        }
    }
}


void MakeFaceOnDma(int fh, struct nufaceongeom_s *geom, int mtlid)
{
    char **dma;
    int pos;
    int dmastart;
    unsigned int size;

    dma = &cvt_dmaptr;
    cvt_dmaptr = cvt_dmabuffer;
    if (cvt_dmabuffer == 0) {
        NuErrorProlog(nugscn_srcfile, 0x57C)(no_dmabuffer_msg);
    }
    pos = NuFilePos(cvt_fh);
    dmastart = (int)cvt_dmaptr - 4;
    NuCvtGeoSetColourRefOffset(pos - dmastart);
    switch (geom->faceontype) {
    case 0:
        NuCvtDmaFaceOn(dma, geom->faceons, geom->nfaceons, &cvt_mtls[mtlid]);
        break;
    case 1:
        NuCvtDmaFaceY(dma, geom->faceons, geom->nfaceons, &cvt_mtls[mtlid]);
        break;
    default:
        NuErrorProlog(nugscn_srcfile, 0x585)(unknown_faceon_msg);
        break;
    }
    cvt_dmaptr = vpDmaTag_Ret(cvt_dmaptr);
    size = ((unsigned int)(cvt_dmaptr - cvt_dmabuffer) + 0xF) & ~0xF;
    if (size >= 0x400000) {
        NuErrorProlog(nugscn_srcfile, 0x58D)(dma_overflow_msg);
    }
    NuFileWriteInt(cvt_fh, size);
    NuFileWrite(cvt_fh, cvt_dmabuffer, size);
}


struct nugscn_s *NuGScnDuplicate(struct nugscn_s *scene, char **buffer)
{
    struct nugscn_s *dup;
    int i;

    dup = (struct nugscn_s *)ALIGN16(*buffer);
    *buffer = (char *)&dup[1];
    *dup = *scene;

    *buffer = ALIGN16(*buffer);
    dup->instances = (struct nuinstance_s *)*buffer;
    *buffer += dup->numinstance * sizeof(struct nuinstance_s);
    for (i = 0; i < dup->numinstance; i++) {
        dup->instances[i] = scene->instances[i];
    }

    *buffer = ALIGN16(*buffer);
    dup->specials = (struct nuspecial_s *)*buffer;
    *buffer += dup->numspecial * sizeof(struct nuspecial_s);
    for (i = 0; i < dup->numspecial; i++) {
        dup->specials[i] = scene->specials[i];
        dup->specials[i].instance = &dup->instances[scene->specials[i].instance - scene->instances];
    }
    return dup;
}


void ReadAnimationLibrary(int fh)
{
    struct nuanimdata_s *anim;
    struct nuanimdatachunk_s *chunk;
    struct nuanimcurveset_s *curveset;
    int count;
    int i;
    int j;
    int k;
    int m;

    count = NuFileReadInt(fh);
    NuFileReadInt(fh);
    loading_scene->animlib = NuMemFileAddr(fh);
    for (i = 0; i < count; i++) {
        RELOCATE(loading_scene->animlib[i]);
        anim = loading_scene->animlib[i];
        if (anim != 0) {
            RELOCATE(anim->chunks);
            for (j = 0; j < anim->nchunks; j++) {
                RELOCATE(anim->chunks[j]);
                chunk = anim->chunks[j];
                RELOCATE(chunk->animcurvesets);
                for (k = 0; k < chunk->numnodes; k++) {
                    RELOCATE(chunk->animcurvesets[k]);
                    curveset = chunk->animcurvesets[k];
                    RELOCATE(curveset->constants);
                    RELOCATE(curveset->set);
                    for (m = 0; m < curveset->ncurves; m++) {
                        if (curveset->set[m] != 0) {
                            curveset->set[m] = (void *)((char *)&curveset->set[m] + (int)curveset->set[m]);
                            RELOCATE(curveset->set[m]->animkeys);
                        }
                    }
                }
            }
        }
    }
}


void NuGScnSetTmpWkspc(void *workspace, int size)
{
    temp_workspace = workspace;
    temp_workspace_size = size;
}


void NuGScnSetCfg(struct nusplinecfg_s *cfg)
{
    spline_cfg = cfg;
}


void NuGScnRemove(struct nugscn_s *scene)
{
    int i;

    if (scene->texanims != 0) {
        NuTexAnimRemoveList(scene->texanims);
    }
    for (i = 0; i < scene->nummtl; i++) {
        NuMtlDestroy(scene->mtls[i]);
    }
    NuDebugMsgProlog(nugscn_srcfile, 0xE37)(removed_mtls_msg, scene->nummtl);
    for (i = 0; i < scene->numtid; i++) {
        NuTexDestroy(scene->tids[i]);
    }
}


inline int NuGScnLoad(char **buffer, char *bufend, char *filename)
{
    char *aligned;
    char *filebuf;
    int size;

    gscn_bufferstart = *buffer;
    gscn_buffer = buffer;
    gscn_bufferend = bufend;
    if (temp_workspace != 0) {
        gscn_bufferlimit = (char *)temp_workspace_size;
    } else {
        gscn_bufferlimit = bufend;
    }
    aligned = (char *)(((int)*gscn_buffer + 0x7F) & ~0x7F);
    if (aligned < gscn_bufferlimit) {
        filebuf = aligned;
        *gscn_buffer = aligned;
    } else {
        NuErrorProlog(nugscn_srcfile, 0x184)(bufferalloc_failed_msg);
        filebuf = 0;
    }
    gscn_filebuffer = filebuf;
    size = NuFileLoadBuffer(filename, filebuf, bufend - *buffer);
    gscn_load.size = size;
    gscn_load.filebuffer = filebuf;
    gscn_load.buffer = buffer;
    gscn_load.bufferend = bufend;
    gscn_load.filename = filename;
    return size;
}


struct nugscn_s *NuGScnRead(char **buffer, char *bufend, char *filename)
{
    if (NuGScnLoad(buffer, bufend, filename) != 0) {
        return NuGScnLoadFix();
    }
    return 0;
}


void NuGScnTranslate(struct nugscn_s *scene, struct nuvec_s *offset)
{
    struct nuinstance_s *instance;
    int i;

    i = scene->numinstance;
    instance = &scene->instances[i];
    while (i != 0) {
        i--;
        instance--;
        NuMtxTranslate(&instance->matrix, offset);
    }
}


void NuGScnAddExt(struct nugscn_s *scene, int count, struct nuspecial_s *specials)
{
    scene->numexspecials = count;
    scene->exspecials = specials;
    scene->exallocix = 0;
    memset(specials, 0, count * sizeof(struct nuspecial_s));
}


int NuExtSpecialDuplicate(struct nuhspecial_s *dst, struct nuhspecial_s *src)
{
    struct nugscn_s *scene;
    struct nuspecial_s *special;
    int start;

    scene = src->scene;
    start = scene->exallocix;
    do {
        scene->exallocix++;
        if (scene->exallocix == scene->numexspecials) {
            scene->exallocix = 0;
        }
        special = &scene->exspecials[scene->exallocix];
        if ((special->flags & 1) == 0) {
            dst->scene = scene;
            dst->special = special;
            *special = *src->special;
            special->flags |= 3;
            return 1;
        }
    } while (scene->exallocix != start);
    return 0;
}


void NuExtSpecialDelete(struct nuhspecial_s *sph)
{
    sph->special->flags &= ~1;
}


void NuGSceneSetCrossFade(struct nugscn_s *scene, short crossfade, short rate)
{
    if (crossfade != scene->crossfade) {
        scene->crossfade = crossfade;
        scene->crossfaderate = rate;
        scene->crossfadefirst = scene->crossfadegobj;
        scene->crossfadegobj++;
        if (scene->crossfadegobj >= scene->numgobj) {
            scene->crossfadegobj = 0;
        }
    }
}


void NuGSceneProcessCrossFade(struct nugscn_s *scene)
{
    int count;

    count = scene->crossfaderate;
    verts_this_frame = 0;
    if (count == 0) {
        count = 0x7FFFFFFF;
    }
    do {
        if (scene->crossfadegobj == scene->crossfadefirst) {
            break;
        }
        verts_this_frame += NuGobjApplyCrossFade(scene->gobjs[scene->crossfadegobj++], scene->crossfade);
        if (scene->crossfadegobj >= scene->numgobj) {
            scene->crossfadegobj = 0;
        }
    } while (--count != 0);
}


void NuGScnMtlUpdate(void)
{
}


int NuSpecialFind(struct nugscn_s *scene, struct nuhspecial_s *sph, char *name)
{
    struct nuspecial_s *special;
    int i;

    special = scene->specials;
    for (i = 0; i < scene->numspecial; i++, special++) {
        if (strcasecmp(name, special->name) == 0) {
            sph->scene = scene;
            sph->special = special;
            return 1;
        }
    }
    return 0;
}


void NuSpecialGetPos(struct nuhspecial_s *sph, struct nuvec_s *pos)
{
    struct numtx_s *mtx;

    mtx = &sph->special->mtx;
    if ((sph->special->flags & 1) == 0) {
        mtx = &sph->special->instance->matrix;
    }
    pos->x = mtx->_30;
    pos->y = mtx->_31;
    pos->z = mtx->_32;
}


void NuSpecialGetMtx(struct nuhspecial_s *sph, struct numtx_s *mtx)
{
    struct numtx_s *src;

    src = &sph->special->mtx;
    if ((sph->special->flags & 1) == 0) {
        src = &sph->special->instance->matrix;
    }
    *mtx = *src;
}


int NuSpecialDrawAt(struct nuhspecial_s *sph, struct numtx_s *mtx)
{
    struct nugscn_s *scene;

    scene = sph->scene;
    return NuRndrGScnObj(scene->gobjs[sph->special->instance->objid], mtx);
}


struct nugspline_s *NuSplineFind(struct nugscn_s *scene, char *name)
{
    struct nugspline_s *spline;
    int i;

    if (scene != 0) {
        spline = scene->splines;
        for (i = 0; i < scene->numsplines; i++, spline++) {
            if (strcasecmp(name, spline->name) == 0) {
                return spline;
            }
        }
    }
    return 0;
}


int NuSplineFindAllSub(struct nugscn_s *scene, char *name, struct nugspline_s **list, int max)
{
    struct nugspline_s *spline;
    int count;
    int i;

    if (max <= 0) {
        return 0;
    }
    count = 0;
    spline = scene->splines;
    for (i = 0; i < scene->numsplines; i++, spline++) {
        if (NuStrIStr(spline->name, name) != 0) {
            list[count++] = spline;
            if (count >= max) {
                return count;
            }
        }
    }
    return count;
}


void NuSplineGetRandomPoint(struct nugspline_s *spline, struct nuvec_s *pt)
{
    struct nuvec_s *p0;
    struct nuvec_s *p1;
    int ix;
    int next;

    ix = rand() % spline->len;
    next = (ix + 1) % spline->len;
    p0 = &spline->pts[spline->ptsize * ix];
    p1 = &spline->pts[spline->ptsize * next];
    NuVecLerp(pt, p0, p1, (float)(rand() & 0x7FFF) * (1.0f / 32768.0f));
}


void NuSceneReAssignTexAnims(struct nutexanim_s *texanims, int count)
{
    int i;

    if (texanims != 0) {
        for (i = 0; i < count; i++) {
            texanims->env->prog = NuTexAnimProgFind(texanims->scriptname);
            NuTexAnimEnvReset(texanims->env);
            texanims++;
        }
    }
}


void CvtNuIFFFaceOnGeom(int fh)
{
    struct nufaceongeom_s geom;
    int mtlid;
    int size;

    mtlid = NuFileReadInt(fh);
    NuFileWriteInt(cvt_fh, mtlid);
    geom.nfaceons = NuFileReadInt(fh);
    geom.faceontype = NuFileReadInt(fh);
    NuFileWriteInt(cvt_fh, geom.faceontype);
    geom.faceonradius = NuFileReadFloat(fh);
    NuFileWriteFloat(cvt_fh, geom.faceonradius);
    size = geom.nfaceons * sizeof(struct nufaceon_s);
    geom.faceons = NuMemAllocFn(size, nugscn_srcfile, 0x4D9);
    NuFileRead(fh, geom.faceons, size);
    MakeFaceOnDma(fh, &geom, mtlid);
    CalcDimsAddFaceOn(&geom);
    NuMemFreeFn(geom.faceons, nugscn_srcfile, 0x4E0);
}


void CvtNuIFFNodeHierarchy(int fh)
{
    struct nunode_s *root;

    root = NuNodeRead(fh, 0);
    NuNodeGraphInit(root);
    CvtNodes(root, 0);
    NuFileBeginBlkWrite(cvt_fh, 0x54534E49, 0);
    NuFileWriteInt(cvt_fh, cvt_numinstances);
    NuFileWriteInt(cvt_fh, 0);
    NuFileWrite(cvt_fh, cvt_instances, sizeof(struct nuinstance_s) * cvt_numinstances);
    NuFileEndBlkWrite(cvt_fh);
}


int FindSplinePtSize(char *name)
{
    struct nusplinecfg_s *cfg;

    cfg = spline_cfg;
    if (cfg != 0) {
        while (cfg->name != 0) {
            if (strstr(name, cfg->name) != 0) {
                return cfg->ptsize;
            }
            cfg++;
        }
    }
    return 12;
}
