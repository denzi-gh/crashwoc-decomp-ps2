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

#include "nu_libc.h"

#include "nucore/nuerror.h"
#include "numath/numathtypes.h"
#include "nu3d/numtl.h"
#include "nu3d/nutex.h"
#include "nu3d/nutexanm.h"
#include "nu3d/nugobj.h"
#include "nu3d/nunode.h"
#include "nu3d/nugscn.h"
#include "nu3d/nuscene.h"
#include "nu3d/nuanim.h"

struct nucntrl_s {
    char pad00[0x0C];
}; /* 0x0C */

struct nuspecialfile_s {
    struct numtx_s mtx;             /* 0x00 */
    int instance;                   /* 0x40 */
    int name;                       /* 0x44 */
    int flags;                      /* 0x48 */
    int pad4C;
}; /* 0x50 */

typedef int (*numtlreadevent_fn)(struct numtl_s *mtl);

extern void *NuMemAllocFn(int size, char *file, int line);
extern void NuMemFreeFn(void *ptr, char *file, int line);
extern int NuFileRead(int fh, void *buf, int size);
extern int NuFileBeginBlkRead(int fh, int blk);
extern void NuFileEndBlkRead(int fh);
extern void NuTexSwizzelBits(struct nutex_s *tex);
extern void NuPs2CreateRenderStream(struct nugobj_s *gobj);
extern void NuPs2CreateFaceOn(struct nugobj_s *gobj);
extern void NuPs2CreateSkin(struct nugobj_s *gobj);
void ReadNuIFFGeomVtx(int fh, struct nugeom_s *geom);
static void ReadNuIFFGeomSkin(int fh, struct nugeom_s *geom);
extern int NuFileReadInt(int fh);
extern unsigned int NuFileReadUnsignedInt(int fh);
extern char NuFileReadChar(int fh);
extern float NuFileReadFloat(int fh);
extern void NuNodeDestroyHierarchy(struct nunode_s *node);
extern int NuFileOpen(char *name, int mode);
extern void NuFileClose(int fh);
extern char *strlwr(char *s);

extern int show_strip;
extern int info_tricount;
extern int info_tristripcount;
extern numtlreadevent_fn NuMtlReadEvent;

static int mipmaplevel = 0;
int spectid = 0;
struct numtl_s *specmtl = 0;


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
            NuDebugMsgProlog("..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x15B)(
                "Failed to include file %s!", strlwr(name));
        }
    }
}


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

    NuDebugMsgProlog("..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x185)("Reading texture set header...");
    NuFileBeginBlkRead(fh, 0x30485354);
    count = NuFileReadInt(fh);
    NuFileEndBlkRead(fh);
    if (count) {
        scene->numtids = 0;
        scene->tids = NuMemAllocFn(count * sizeof(short), "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x190);
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
            bits = NuMemAllocFn(size, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x1B5);
            tex.bits = bits;
            NuFileRead(fh, bits, size);
            size = NuTexPalSize(tex.type);
            if (size) {
                tex.pal = NuMemAllocFn(size, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x1BE);
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
                NuMemFreeFn(tex.pal, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x1D6);
            }
            NuMemFreeFn(bits, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x1D7);
            NuFileEndBlkRead(fh);
        }
    }
}


inline void ReadNuIFFMaterialSet(int fh, struct nuscene_s *scene) {
    int count;
    int i;

    NuDebugMsgProlog("..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x1E6)("Reading Material set...");
    count = NuFileReadInt(fh);
    if (count) {
        scene->mtls = NuMemAllocFn(count * sizeof(struct numtl_s *), "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x1EF);
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

    NuDebugMsgProlog("..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x246)("Reading Spline set...");
    count = NuFileReadInt(fh);
    if (count) {
        scene->splines = NuMemAllocFn(count * sizeof(struct nuspline_s), "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x24E);
        scene->numsplines = count;
        NuFileRead(fh, scene->splines, count * sizeof(struct nuspline_s));
        npts = NuFileReadInt(fh);
        if (npts) {
            scene->splinedata = NuMemAllocFn(npts * sizeof(struct nuvec_s), "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x255);
            NuFileRead(fh, scene->splinedata, npts * sizeof(struct nuvec_s));
            pts = scene->splinedata;
            for (i = 0; i < count; i++) {
                scene->splines[i].cvs = pts;
                pts += scene->splines[i].ncvs;
            }
        }
    }
}


void ReadNuIFFGSplineSet(int fh, struct nugscn_s *gscene) {
    int size;
    int *data;
    int i;
    int j;
    char *pt;

    NuDebugMsgProlog("..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x27E)("Reading Spline set...");
    gscene->numsplines = NuFileReadInt(fh);
    size = NuFileReadInt(fh);
    if (gscene->numsplines) {
        gscene->splinedata = NuMemAllocFn(size, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x288);
        data = gscene->splinedata;
        NuFileRead(fh, data, size);
        gscene->splines = NuMemAllocFn(gscene->numsplines * sizeof(struct nugspline_s), "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x28A);
        for (i = 0; i < gscene->numsplines; i++) {
            gscene->splines[i].len = *data++;
            gscene->splines[i].name = gscene->nametable + *data++;
            gscene->splines[i].ptsize = FindSplinePtSize(gscene->splines[i].name);
            gscene->splines[i].pts = NuMemAllocFn(gscene->splines[i].ptsize * gscene->splines[i].len, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x292);
            pt = gscene->splines[i].pts;
            for (j = 0; j < gscene->splines[i].len; j++) {
                *(struct nuvec_s *)pt = *(struct nuvec_s *)data;
                data += 3;
                pt += gscene->splines[i].ptsize;
            }
        }
    }
}


inline void ReadNuIFFGobjSet(int fh, struct nuscene_s *scene) {
    int count;
    int i;

    NuDebugMsgProlog("..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x2A4)("Reading geometry set header...");
    count = NuFileReadInt(fh);
    if (scene->gobjs) {
        NuErrorProlog("..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x2AB)(
            "ReadNuIFFGobjSet : Object already defined!");
    }
    scene->gobjs = NuMemAllocFn(count * sizeof(struct nugobj_s *), "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x2AC);
    memset(scene->gobjs, 0, count * sizeof(struct nugobj_s *));
    scene->numgobjs = 0;
    for (i = 0; i < count; i++) {
        ReadNuIFFGeomDef(fh, scene);
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


/* inline (defined after their caller) only defers the output of GeomVtx and
   GeomPrim, which emits their strings ahead of NuNodeRead's as retail has them */
inline void ReadNuIFFGeomVtx(int fh, struct nugeom_s *geom) {
    int count;
    int size;
    int i;
    struct nuvtx_tc1_s *vtx;
    struct nucolour_s src;
    struct nucolour_s dst;

    count = NuFileReadInt(fh);
    if (count) {
        size = count * sizeof(struct nuvtx_tc1_s);
        vtx = NuMemAllocFn(size, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x371);
        geom->vtxptr = vtx;
        geom->vtxcnt = count;
        geom->vtxmax = count;
        if (!vtx) {
            NuErrorProlog("..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x378)(
                "ReadNuIFFGeomVtx : Lock VB failed!");
        }
        NuFileRead(fh, vtx, size);
        for (i = 0; i < count; i++) {
            src = vtx[i].rgba;
            dst.r = src.b;
            dst.g = src.g;
            dst.b = src.r;
            dst.a = src.a;
            vtx[i].rgba = dst;
        }
    }
}


inline void ReadNuIFFGeomPrim(int fh, struct nugeom_s *geom) {
    int nprims;
    int i;

    nprims = NuFileReadInt(fh);
    if (nprims) {
        for (i = 0; i < nprims; i++) {
            switch (NuFileReadInt(fh)) {
            case 5:
                ReadNuIFFGeomTri(fh, geom, 5);
                break;
            case 6:
                ReadNuIFFGeomTri(fh, geom, 6);
                break;
            case 8:
                ReadNuIFFGeomBezTri(fh, geom);
                break;
            default:
                NuErrorProlog("..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x3C5)("Unknown primitive type");
                break;
            }
        }
    }
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
            geom->vtxskininfo = NuMemAllocFn(size, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x3F8);
            NuFileRead(fh, geom->vtxskininfo, size);
        } else {
            for (i = 0; i < count; i++) {
                skin = NuMemAllocFn(sizeof(struct nuskin_s), "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x403);
                memset(skin, 0, sizeof(struct nuskin_s));
                skin->vtxoffset = NuFileReadInt(fh);
                skin->vtxcnt = NuFileReadInt(fh);
                skin->mtxcnt = NuFileReadInt(fh);
                skin->mtxid = NuMemAllocFn(skin->mtxcnt * sizeof(int), "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x408);
                for (j = 0; j < skin->mtxcnt; j++) {
                    skin->mtxid[j] = NuFileReadInt(fh);
                }
                size = skin->vtxcnt * (skin->mtxcnt << 2);
                skin->weights = NuMemAllocFn(size, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x40C);
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
        geom->blendgeom = NuMemAllocFn(sizeof(struct nublendgeom_s), "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x41C);
        memset(geom->blendgeom, 0, sizeof(struct nublendgeom_s));
        offset = 0;
        geom->blendgeom->nblends = count;
        geom->blendgeom->blend_offsets = NuMemAllocFn(count * sizeof(struct nuvec_s *), "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x420);
        memset(geom->blendgeom->blend_offsets, 0, count * sizeof(struct nuvec_s *));
        geom->blendgeom->ix = NuMemAllocFn(count * sizeof(int), "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x424);
        NuFileRead(fh, geom->blendgeom->ix, count * sizeof(int));
        size = NuFileReadInt(fh);
        geom->blendgeom->offsets = NuMemAllocFn(size, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x429);
        NuFileRead(fh, geom->blendgeom->offsets, size);
        for (i = 0; i < count; i++) {
            if (NuFileReadChar(fh)) {
                geom->blendgeom->blend_offsets[i] = &geom->blendgeom->offsets[offset];
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
        node = &NuMeshNodeCreate(scene->gobjs[NuFileReadInt(fh)])->node;
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
        joints = skin->joint_ids;
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
        NuFileRead(fh, &joint->pivot, sizeof(struct nuvec_s));
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
        NuErrorProlog("..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x552)("NuNodeRead: Unknown node type.");
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


static inline void ReadNuIFFNodeNames(int fh, struct nuscene_s *scene) {
    int size;
    int i;

    scene->numnodes = NuFileReadInt(fh);
    size = NuFileReadInt(fh);
    scene->names = NuMemAllocFn(scene->numnodes * sizeof(char *), "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x16C);
    scene->nametable = NuMemAllocFn(size, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x16D);
    scene->nodes = NuMemAllocFn(scene->numnodes * sizeof(struct nunode_s *), "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x16E);
    NuFileRead(fh, scene->nametable, size);
    for (i = 0; i < scene->numnodes; i++) {
        scene->names[i] = scene->nametable + NuFileReadInt(fh);
    }
}


inline void ReadNuIFFUnknown(int fh, int blk) {
    NuDebugMsgProlog("..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x57A)(
        "Unknown block type %c%c%c%c", blk & 0xFF, (blk >> 8) & 0xFF,
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
    gscene->numspecial = count;
    if (count) {
        data = NuMemAllocFn(count * sizeof(struct nuspecialfile_s), "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x5D8);
        NuFileRead(fh, data, gscene->numspecial * sizeof(struct nuspecialfile_s));
        gscene->specials = NuMemAllocFn(gscene->numspecial * sizeof(struct nuspecial_s), "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x5DC);
        for (i = 0; i < gscene->numspecial; i++) {
            gscene->specials[i].instance = &gscene->instances[data[i].instance];
            gscene->specials[i].name = gscene->nametable + data[i].name;
            gscene->specials[i].mtx = data[i].mtx;
        }
        NuMemFreeFn(data, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x5E5);
    }
}


inline int ReadNuIFFInstSet(int fh, struct nuinstance_s **instances, struct nuinstanim_s **instanimdata) {
    int count;
    int nanims;
    int i;
    struct nuinstance_s *inst;
    struct nuinstanim_s *anim;

    count = NuFileReadInt(fh);
    *instances = NuMemAllocFn(count * sizeof(struct nuinstance_s), "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x5A6);
    NuFileRead(fh, *instances, count * sizeof(struct nuinstance_s));
    nanims = NuFileReadInt(fh);
    if (nanims) {
        anim = NuMemAllocFn(nanims * sizeof(struct nuinstanim_s), "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x5AD);
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
        gscene->instanimdata = NuMemAllocFn(count * sizeof(struct nuanimdata_s *), "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x26C);
        memset(gscene->instanimdata, 0, count * sizeof(struct nuanimdata_s *));
        for (i = 0; i < count; i++) {
            if (NuFileReadChar(fh)) {
                gscene->instanimdata[i] = NuAnimDataRead(fh);
            }
        }
    }
}


static inline void ReadNuIFFInstanceIxs(int fh, struct nugscn_s *gscene) {
    int size;

    gscene->numinstanceixs = NuFileReadInt(fh);
    size = gscene->numinstanceixs * sizeof(short);
    if (size) {
        gscene->instanceixs = NuMemAllocFn(size, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x5C8);
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
            gscene->tids = NuMemAllocFn(scene.numtids * sizeof(int), "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x5FA);
            for (i = 0; i < scene.numtids; i++) {
                gscene->tids[i] = scene.tids[i];
            }
            gscene->numtid = scene.numtids;
            break;
        case 0x3030534D:
            ReadNuIFFMaterialSet(fh, &scene);
            gscene->mtls = scene.mtls;
            gscene->nummtl = scene.nummtls;
            break;
        case 0x30545347:
            ReadNuIFFGobjSet(fh, &scene);
            break;
        case 0x54534E49:
            gscene->numinstance = ReadNuIFFInstSet(fh, &gscene->instances, &gscene->instanimblock);
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
        NuMemFreeFn(scene.tids, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x615);
    }
    scene.tids = 0;
    gscene->numgobj = scene.numgobjs;
    gscene->gobjs = scene.gobjs;
}


struct nuscene_s *NuSceneLoad(char *filename) {
    struct nuscene_s *scene;
    int fh;
    int blk;

    scene = 0;
    fh = NuFileOpen(filename, 0);
    if (fh) {
        NuDebugMsgProlog("..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x62F)(
            "Reading scene file %s...", strlwr(filename));
        blk = NuFileBeginBlkRead(fh, 0);
        scene = NuMemAllocFn(sizeof(struct nuscene_s), "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x632);
        memset(scene, 0, sizeof(struct nuscene_s));
        switch (blk) {
        case 0x3032554E:
            ReadNuIFFScene(fh, scene);
            break;
        case 0x30435347:
            scene->gscene = NuMemAllocFn(sizeof(struct nugscn_s), "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x63D);
            memset(scene->gscene, 0, sizeof(struct nugscn_s));
            ReadNuIFFGScene(fh, scene->gscene);
            break;
        }
        NuFileClose(fh);
    } else {
        NuDebugMsgProlog("..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x646)(
            "Failed to open scene file %s!", strlwr(filename));
    }
    return scene;
}


void NuGSceneDestroy(struct nugscn_s *gscene) {
    int i;

    for (i = 0; i < gscene->numtid; i++) {
        NuTexDestroy(gscene->tids[i]);
    }
    NuMemFreeFn(gscene->tids, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x67E);
    for (i = 0; i < gscene->nummtl; i++) {
        NuMtlDestroy(gscene->mtls[i]);
    }
    NuMemFreeFn(gscene->mtls, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x683);
    for (i = 0; i < gscene->numgobj; i++) {
        NuGobjDestroy(gscene->gobjs[i]);
    }
    NuMemFreeFn(gscene->gobjs, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x688);
    if (gscene->instances) {
        NuMemFreeFn(gscene->instances, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x68C);
    }
    if (gscene->instances) {
        NuMemFreeFn(gscene->instances, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x690);
    }
    if (gscene->instanimblock) {
        NuMemFreeFn(gscene->instanimblock, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x694);
    }
    if (gscene->instanimdata) {
        NuMemFreeFn(gscene->instanimdata, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x697);
    }
    if (gscene->numsplines) {
        NuMemFreeFn(gscene->splinedata, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x69C);
        NuMemFreeFn(gscene->splines, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x69D);
    }
    if (gscene->nametable) {
        NuMemFreeFn(gscene->nametable, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x6A2);
    }
    if (gscene->specials) {
        NuMemFreeFn(gscene->exspecials, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x6A6);
    }
    for (i = 0; i < gscene->numtexanims; i++) {
        NuTexAnimDestroy(&gscene->texanims[i]);
    }
    NuMemFreeFn(gscene->texanims, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x6AB);
    NuMemFreeFn(gscene->texanim_tids, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x6AC);
    NuMemFreeFn(gscene, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x6AF);
}


void NuSceneDestroy(struct nuscene_s *scene) {
    int i;

    NuMemFreeFn(scene->names, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x6BB);
    NuMemFreeFn(scene->nodes, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x6BC);
    NuMemFreeFn(scene->nametable, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x6BD);
    if (scene->tids) {
        for (i = 0; i < scene->numtids; i++) {
            NuTexDestroy(scene->tids[i]);
        }
        NuMemFreeFn(scene->tids, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x6C4);
    }
    for (i = 0; i < scene->nummtls; i++) {
        NuMtlDestroy(scene->mtls[i]);
    }
    if (scene->mtls) {
        NuMemFreeFn(scene->mtls, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x6CA);
    }
    for (i = 0; i < scene->numgobjs; i++) {
        NuGobjDestroy(scene->gobjs[i]);
    }
    if (scene->gobjs) {
        NuMemFreeFn(scene->gobjs, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x6D0);
    }
    if (scene->splines) {
        NuMemFreeFn(scene->splines, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x6D4);
    }
    if (scene->splinedata) {
        NuMemFreeFn(scene->splinedata, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x6D6);
    }
    NuNodeDestroyHierarchy(scene->root);
    if (scene->gscene) {
        NuGSceneDestroy(scene->gscene);
    }
    NuMemFreeFn(scene, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x6E2);
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
    table = NuMemAllocFn(tablesize, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x6EF);
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
        geom->cntrls = NuMemAllocFn(size, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x398);
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
    fgeom->faceon_type = NuFileReadInt(fh);
    fgeom->faceon_radius = NuFileReadFloat(fh);
    size = fgeom->nfaceons * sizeof(struct nufaceon_s);
    fgeom->faceons = NuMemAllocFn(size, "..\\nu2crash.ps2\\nu3d\\nuscene.c", 0x3AA);
    NuFileRead(fh, fgeom->faceons, size);
}
