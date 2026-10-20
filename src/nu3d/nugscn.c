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

#include "nu_libc.h"
#include "nu_stdarg.h"

#include "nu3d/nugobj.h"
#include "nu3d/nurndr.h"
#include "nu3d/nugscn.h"
#include "nu3d/numtl.h"
#include "nu3d/nunode.h"
#include "nu3d/nuscene.h"
#include "nu3d/nutex.h"
#include "nu3d/nutexanm.h"
#include "nucore/nuerror.h"
#include "numath/numathtypes.h"
#include "numath/numtx.h"
#include "numath/nuvec.h"
#include "nu3d/nuanim.h"
#include "nucore/nucoretypes.h"

struct nusplinecfg_s {
    char *name;                     /* 0x00 */
    int ptsize;                     /* 0x04 */
};

extern int NuRndrGScnObj(struct nugobj_s *gobj, struct numtx_s *m);

extern void *NuMemAllocFn(int size, char *file, int line);
extern void NuMemFreeFn(void *ptr, char *file, int line);
extern int NuFileRead(int fh, void *buf, int size);
extern int NuFileReadInt(int fh);
extern float NuFileReadFloat(int fh);
extern void NuFileBeginBlkWrite(int fh, int blk, int flags);
extern void NuFileEndBlkWrite(int fh);
extern int NuFileWrite(int fh, void *buf, int size);
extern int NuFileWriteInt(int fh, int value);
extern void NuFileWriteFloat(int fh, float value);
extern struct nunode_s *NuNodeRead(int fh, struct nunode_s *parent);
extern void CvtNodes(struct nunode_s *node, struct nunode_s *parent);

extern char *NuStrIStr(char *str, char *sub);

struct nucvtmtl_s {
    char pad000[0x170];
    unsigned int attrib_lo : 3;     /* 0x170 */
    unsigned int stream : 1;
    unsigned int attrib_hi : 28;
    char pad174[0x1A8 - 0x174];
    unsigned long long alpha_reg;   /* 0x1A8 */
    char pad1B0[0x1D0 - 0x1B0];
} __attribute__((aligned(16))); /* 0x1D0 */

extern int NuFilePos(int fh);
extern void *NuMemFileAddr(int fh);
extern void NuCvtGeoSetColourRefOffset(int offset);
extern void NuCvtDmaFaceOn(char **dma, struct nufaceon_s *faceons, int nfaceons, struct nucvtmtl_s *mtl);
extern void NuCvtDmaFaceY(char **dma, struct nufaceon_s *faceons, int nfaceons, struct nucvtmtl_s *mtl);
extern char *vpDmaTag_Ret(char *dma);

extern int verts_this_frame;

extern int NuFileLoadBuffer(char *name, void *buf, int size);
extern struct nugscn_s *NuGScnLoadFix(void);

struct nugscnload_s {
    int size;                       /* 0x00 */
    char *filebuffer;               /* 0x04 */
    union variptr_u *buffer;        /* 0x08 */
    char *bufferend;                /* 0x0C */
    char *filename;                 /* 0x10 */
};

extern char *D_00633098;
extern char **D_0063309C;
extern union variptr_u D_006330A0;
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

extern char D_00614D00[];
extern char D_00615130[];
#define nugscn_srcfile D_00614D00
#define removed_mtls_msg D_00615130

/* Spline set as read from the IFF: a 0x3C-byte record per spline, with all
 * the control points stored after them in one block. */
struct nucvtspline_s {
    int ncvs;                       /* 0x00 */
    struct nuvec_s *cvs;            /* 0x04 */
    char pad08[0x3C - 0x08];
}; /* 0x3C */

extern struct nucvtspline_s *cvt_splines;
extern int cvt_numsplines;
extern struct nuvec_s *cvt_spline_cvs;

extern char D_00614D70[];
extern char D_00614E08[];
#define texanimset_msg D_00614D70
#define splineset_msg D_00614E08

extern struct nuvec_s nugscn_gobj_min;
extern struct nuvec_s nugscn_gobj_max;

extern union variptr_u D_006330B0;
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



extern int NuFileSeek(int fh, int offset, int origin);
extern void NuCvtGeoBeginColourRef(char *buffer);
extern char *NuCvtGeoEndColourRef(void);

extern int nugscn_generate_colourref;
extern int info_tricount;
extern int info_tristripcount;
extern int info_colourrefsize;

void CvtNuIFFGeomDef(int fh)
{
    struct nuvec_s dims;
    char *colourref;
    int ngobjs;
    int type;
    int ngeoms;
    int hdrpos;
    int filepos;
    int tricount;
    int endpos;
    int size;
    int i;
    int j;

    ngobjs = NuFileReadInt(fh);
    NuFileWriteInt(cvt_fh, ngobjs);
    NuFileWriteInt(cvt_fh, 0);
    NuFileWriteInt(cvt_fh, 0);
    NuFileWriteInt(cvt_fh, 0);
    if (nugscn_generate_colourref) {
        colourref = NuMemAllocFn(0x100000, nugscn_srcfile, 0x3CE);
        NuCvtGeoBeginColourRef(colourref);
    }
    for (i = 0; i < ngobjs; i++) {
        type = NuFileReadInt(fh);
        NuFileWriteInt(cvt_fh, type);
        nugscn_gobj_min.x = 3.4028235e38f;
        nugscn_gobj_min.y = 3.4028235e38f;
        nugscn_gobj_min.z = 3.4028235e38f;
        nugscn_gobj_max.x = -3.4028235e38f;
        nugscn_gobj_max.y = -3.4028235e38f;
        nugscn_gobj_max.z = -3.4028235e38f;
        switch (type) {
        case 0:
            NuFileRead(fh, &dims, sizeof(struct nuvec_s));
            ngeoms = NuFileReadInt(fh);
            NuFileWriteInt(cvt_fh, ngeoms);
            NuFileWrite(cvt_fh, &dims, sizeof(struct nuvec_s));
            hdrpos = NuFilePos(cvt_fh);
            NuFileWriteInt(cvt_fh, 0);
            NuFileWriteInt(cvt_fh, 0);
            NuFileWriteInt(cvt_fh, 0);
            tricount = info_tricount + info_tristripcount;
            filepos = NuFilePos(fh);
            for (j = 0; j < ngeoms; j++) {
                CvtNuIFFGeom(fh, 0);
            }
            NuFileSeek(fh, filepos, 0);
            for (j = 0; j < ngeoms; j++) {
                CvtNuIFFGeom(fh, 1);
            }
            endpos = NuFilePos(cvt_fh);
            NuFileSeek(cvt_fh, hdrpos, 0);
            NuFileWriteInt(cvt_fh, info_tricount + info_tristripcount - tricount);
            NuFileSeek(cvt_fh, endpos, 0);
            break;
        case 1:
            ngeoms = NuFileReadInt(fh);
            NuFileWriteInt(cvt_fh, ngeoms);
            NuFileWriteInt(cvt_fh, 0);
            NuFileWriteInt(cvt_fh, 0);
            for (j = 0; j < ngeoms; j++) {
                CvtNuIFFFaceOnGeom(fh);
            }
            break;
        }
        NuFileWrite(cvt_fh, &nugscn_gobj_min, sizeof(struct nuvec_s));
        NuFileWrite(cvt_fh, &nugscn_gobj_max, sizeof(struct nuvec_s));
        NuFileWriteInt(cvt_fh, 0);
        NuFileWriteInt(cvt_fh, 0);
    }
    if (nugscn_generate_colourref) {
        size = (NuCvtGeoEndColourRef() - colourref + 0xF) & 0xFFFFFFF0;
        NuFileWriteInt(cvt_fh, size);
        NuFileWriteInt(cvt_fh, 0);
        NuFileWriteInt(cvt_fh, 0);
        NuFileWriteInt(cvt_fh, 0);
        if (size != 0) {
            NuFileWrite(cvt_fh, colourref, size);
            info_colourrefsize += size;
        }
        NuMemFreeFn(colourref, nugscn_srcfile, 0x423);
    }
}


extern char NuFileReadChar(int fh);
extern char D_00614E40[];
#define skin_in_scene_msg D_00614E40

void ReadNuIFFGeomSkin(int fh)
{
    struct nuskin_s *skin;
    int count;
    int size;
    int i;
    int j;

    count = NuFileReadInt(fh);
    if (count) {
        if (NuFileReadChar(fh)) {
            NuErrorProlog(nugscn_srcfile, 0x46B)(skin_in_scene_msg);
            return;
        }
        for (i = 0; i < count; i++) {
            skin = NuMemAllocFn(sizeof(struct nuskin_s), nugscn_srcfile, 0x473);
            memset(skin, 0, sizeof(struct nuskin_s));
            skin->vtxoffset = NuFileReadInt(fh);
            skin->vtxcnt = NuFileReadInt(fh);
            skin->mtxcnt = NuFileReadInt(fh);
            skin->mtxid = NuMemAllocFn(skin->mtxcnt * sizeof(int), nugscn_srcfile, 0x478);
            for (j = 0; j < skin->mtxcnt; j++) {
                skin->mtxid[j] = NuFileReadInt(fh);
            }
            size = skin->vtxcnt * (skin->mtxcnt << 2);
            skin->weights = NuMemAllocFn(size, nugscn_srcfile, 0x47C);
            NuFileRead(fh, skin->weights, size);
            NuMemFreeFn(skin->weights, nugscn_srcfile, 0x480);
            NuMemFreeFn(skin->mtxid, nugscn_srcfile, 0x481);
            NuMemFreeFn(skin, nugscn_srcfile, 0x482);
        }
    }
}


extern struct nuvtx_tc1_s *D_006330CC;
extern int D_006330D0;
extern char D_00614E58[];
#define cvt_vtxbuff D_006330CC
#define cvt_vtxcnt D_006330D0
#define vtxcnt_msg D_00614E58
#define alloc_error_msg D_00614E68

void ReadNuIFFGeomVtx(int fh)
{
    struct nuvtx_tc1_s *vtx;
    struct nucolour_s src;
    struct nucolour_s dst;
    int count;
    int size;
    int i;

    count = NuFileReadInt(fh);
    cvt_vtxcnt = count;
    if (count < 3) {
        NuDebugMsgProlog(nugscn_srcfile, 0x4FE)(vtxcnt_msg);
    }
    if (count) {
        size = count * sizeof(struct nuvtx_tc1_s);
        cvt_vtxbuff = NuMemAllocFn(size, nugscn_srcfile, 0x501);
        if (!cvt_vtxbuff) {
            NuErrorProlog(nugscn_srcfile, 0x504)(alloc_error_msg);
        }
        NuFileRead(fh, cvt_vtxbuff, size);
        for (i = 0; i < count; i++) {
            vtx = &cvt_vtxbuff[i];
            if (vtx->pnt.x < nugscn_gobj_min.x) {
                nugscn_gobj_min.x = vtx->pnt.x;
            }
            if (vtx->pnt.y < nugscn_gobj_min.y) {
                nugscn_gobj_min.y = vtx->pnt.y;
            }
            if (vtx->pnt.z < nugscn_gobj_min.z) {
                nugscn_gobj_min.z = vtx->pnt.z;
            }
            if (vtx->pnt.x > nugscn_gobj_max.x) {
                nugscn_gobj_max.x = vtx->pnt.x;
            }
            if (vtx->pnt.y > nugscn_gobj_max.y) {
                nugscn_gobj_max.y = vtx->pnt.y;
            }
            if (vtx->pnt.z > nugscn_gobj_max.z) {
                nugscn_gobj_max.z = vtx->pnt.z;
            }
            src = vtx->rgba;
            dst.r = src.b;
            dst.g = src.g;
            dst.b = src.r;
            dst.a = src.a;
            vtx->rgba = dst;
        }
    }
}


struct nudmatag_s {
    unsigned short qwc;             /* 0x00 */
    unsigned char flags;            /* 0x02 */
    unsigned char id;               /* 0x03 */
    unsigned int addr;              /* 0x04 */
};

extern struct nudmatag_s *nucvtgeo_lasttag;
extern int *D_006330F4;
extern char D_00614E80[];
#define cvt_streamcount D_006330F4
#define unknown_prim_msg D_00614E80

void CvtNuIFFGeomPrim(int fh, int mtlid, int write)
{
    int nprims;
    int pos;
    int dmastart;
    unsigned int size;
    int i;

    nprims = NuFileReadInt(fh);
    if (nprims) {
        cvt_dmaptr = cvt_dmabuffer;
        pos = NuFilePos(cvt_fh);
        dmastart = (int)cvt_dmaptr - 4;
        NuCvtGeoSetColourRefOffset(pos - dmastart);
        cvt_streamcount = 0;
        nucvtgeo_lasttag = 0;
        for (i = 0; i < nprims; i++) {
            switch (NuFileReadInt(fh)) {
            case 5:
                CvtNuIFFGeomTri(fh, mtlid, write);
                break;
            case 6:
                CvtNuIFFGeomTriStrip(fh, mtlid, write);
                break;
            default:
                NuErrorProlog(nugscn_srcfile, 0x54A)(unknown_prim_msg);
                break;
            }
        }
        if (write) {
            if (nucvtgeo_lasttag) {
                nucvtgeo_lasttag->id = 0x60;
            } else {
                cvt_dmaptr = vpDmaTag_Ret(cvt_dmaptr);
            }
            size = ((unsigned int)(cvt_dmaptr - cvt_dmabuffer) + 0xF) & ~0xF;
            if (size >= 0x400000) {
                NuErrorProlog(nugscn_srcfile, 0x55A)(dma_overflow_msg);
            }
            NuFileWriteInt(cvt_fh, size);
            NuFileWrite(cvt_fh, cvt_dmabuffer, size);
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
    switch (geom->faceon_type) {
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


extern void NuCvtCalcFastPoint(struct nuvec_s *min, struct nuvec_s *max, struct nucvtmtl_s *mtl);
extern int NuCvtTriStreamCFromIdx(char **dma, struct nuvtx_tc1_s *vtx, short *idx, int nidx, struct nucvtmtl_s *mtl);
extern void NuCvtDmaTriCFromIdx(char **dma, struct nuvtx_tc1_s *vtx, short *idx, int nidx, struct nucvtmtl_s *mtl);
extern int NuCvtTriStripStreamCFromIdx(char **dma, struct nuvtx_tc1_s *vtx, short *idx, int nidx, struct nucvtmtl_s *mtl);
extern int NuCvtDmaTriStripCFromIdx(char **dma, struct nuvtx_tc1_s *vtx, short *idx, int nidx, struct nucvtmtl_s *mtl);

extern short *D_006330D4;
extern char D_00614EC8[];
extern char D_0062EE48[];
#define cvt_idxbuff D_006330D4
#define bad_tricount_msg D_00614EC8
#define assert_msg D_0062EE48

void CvtNuIFFGeomTri(int fh, int mtlid, int write)
{
    short *idx;
    int nidx;
    int n;

    nidx = NuFileReadInt(fh);
    cvt_idxbuff = NuMemAllocFn(nidx * sizeof(short), nugscn_srcfile, 0x5AB);
    if (!cvt_idxbuff) {
        NuErrorProlog(nugscn_srcfile, 0x5AD)(alloc_error_msg);
    }
    NuFileRead(fh, cvt_idxbuff, nidx * sizeof(short));
    if (write) {
        idx = cvt_idxbuff;
        NuCvtCalcFastPoint(&nugscn_gobj_min, &nugscn_gobj_max, &cvt_mtls[mtlid]);
        if (cvt_mtls[mtlid].stream == 1) {
            if (!cvt_streamcount) {
                cvt_streamcount = (int *)cvt_dmaptr;
                cvt_dmaptr += 0x10;
                *cvt_streamcount = 0;
            }
            *cvt_streamcount += NuCvtTriStreamCFromIdx(&cvt_dmaptr, cvt_vtxbuff, idx, nidx, &cvt_mtls[mtlid]);
        } else {
            while (nidx > 0) {
                n = (nidx > 6000) ? 6000 : nidx;
                nidx -= 6000;
                if (n < 3 || n % 3 != 0) {
                    NuErrorProlog(nugscn_srcfile, 0x5C4)(bad_tricount_msg);
                }
                if (nucvtgeo_lasttag) {
                    nucvtgeo_lasttag->id = 0x20;
                    nucvtgeo_lasttag->addr = cvt_dmaptr - cvt_dmabuffer;
                }
                NuCvtDmaTriCFromIdx(&cvt_dmaptr, cvt_vtxbuff, idx, n, &cvt_mtls[mtlid]);
                idx += n;
                cvt_dmaptr = (char *)(((unsigned int)cvt_dmaptr + 0x7F) & ~0x7F);
            }
        }
        if ((((unsigned int)(cvt_dmaptr - cvt_dmabuffer) + 0xF) & ~0xF) >= 0x400000) {
            NuErrorProlog(nugscn_srcfile, 0x5D1)(dma_overflow_msg);
        }
    }
    NuMemFreeFn(cvt_idxbuff, nugscn_srcfile, 0x5D3);
}


void CvtNuIFFGeomTriStrip(int fh, int mtlid, int write)
{
    short *idx;
    short *strip;
    int nidx;
    int n;
    int next;
    int maxidx;

    nidx = NuFileReadInt(fh);
    cvt_idxbuff = NuMemAllocFn(nidx * sizeof(short), nugscn_srcfile, 0x628);
    if (!cvt_idxbuff) {
        NuErrorProlog(nugscn_srcfile, 0x62A)(alloc_error_msg);
    }
    NuFileRead(fh, cvt_idxbuff, nidx * sizeof(short));
    if (write) {
        idx = cvt_idxbuff;
        NuCvtCalcFastPoint(&nugscn_gobj_min, &nugscn_gobj_max, &cvt_mtls[mtlid]);
        if (cvt_mtls[mtlid].stream == 1) {
            if (!cvt_streamcount) {
                cvt_streamcount = (int *)cvt_dmaptr;
                cvt_dmaptr += 0x10;
                *cvt_streamcount = 0;
            }
            *cvt_streamcount += NuCvtTriStripStreamCFromIdx(&cvt_dmaptr, cvt_vtxbuff, idx, nidx, &cvt_mtls[mtlid]);
        } else {
            while (nidx > 0) {
                maxidx = 6000;
                n = 0;
                strip = idx;
                while (n < nidx) {
                    next = n + 1;
                    next += *strip;
                    strip += *strip + 1;
                    if (next > maxidx) {
                        break;
                    }
                    n = next;
                }
                nidx -= n;
                if (nucvtgeo_lasttag) {
                    if (nucvtgeo_lasttag->id != 0x10) {
                        NuErrorProlog(nugscn_srcfile, 0x64C)(assert_msg);
                    }
                    nucvtgeo_lasttag->id = 0x20;
                    nucvtgeo_lasttag->addr = cvt_dmaptr - cvt_dmabuffer;
                }
                NuCvtDmaTriStripCFromIdx(&cvt_dmaptr, cvt_vtxbuff, idx, n, &cvt_mtls[mtlid]);
                idx += n;
                cvt_dmaptr = (char *)(((unsigned int)cvt_dmaptr + 0x7F) & ~0x7F);
            }
        }
        if ((((unsigned int)(cvt_dmaptr - cvt_dmabuffer) + 0xF) & ~0xF) >= 0x400000) {
            NuErrorProlog(nugscn_srcfile, 0x657)(dma_overflow_msg);
        }
        NuCvtCalcFastPoint(&nugscn_gobj_min, &nugscn_gobj_max, 0);
    }
    NuMemFreeFn(cvt_idxbuff, nugscn_srcfile, 0x65C);
}


struct nugscn_s *NuGScnDuplicate(struct nugscn_s *scene, char **buffer, char **bufend)
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
    loading_scene->instanimdata = NuMemFileAddr(fh);
    for (i = 0; i < count; i++) {
        RELOCATE(loading_scene->instanimdata[i]);
        anim = loading_scene->instanimdata[i];
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
    temp_workspace.voidptr = workspace;
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
    if (temp_workspace.voidptr != 0) {
        gscn_bufferlimit.charptr = (char *)temp_workspace_size;
    } else {
        gscn_bufferlimit.charptr = bufend;
    }
    aligned = (char *)(((int)*gscn_buffer + 0x7F) & ~0x7F);
    if (aligned < gscn_bufferlimit.charptr) {
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
    gscn_load.buffer = (union variptr_u *)buffer;
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
        NuMtxTranslate(&instance->mtx, offset);
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
        mtx = &sph->special->instance->mtx;
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
        src = &sph->special->instance->mtx;
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
    /* Retail scales the index by the point size twice. */
    p0 = &((struct nuvec_s *)spline->pts)[spline->ptsize * ix];
    p1 = &((struct nuvec_s *)spline->pts)[spline->ptsize * next];
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


void CvtNuIFFTexAnimSet(int fh)
{
    int ntexanims;
    int nscript;
    int size;
    void *texanims;
    short *script;

    NuDebugMsgProlog(nugscn_srcfile, 0x234)(texanimset_msg);
    NuFileBeginBlkWrite(cvt_fh, 0x30534154, 0);
    ntexanims = NuFileReadInt(fh);
    NuFileReadInt(fh);
    size = ntexanims * 0x20;
    NuFileWriteInt(cvt_fh, ntexanims);
    NuFileWriteInt(cvt_fh, 0);
    texanims = NuMemAllocFn(size, nugscn_srcfile, 0x23F);
    NuFileRead(fh, texanims, size);
    NuFileWrite(cvt_fh, texanims, size);

    nscript = NuFileReadInt(fh);
    NuFileWriteInt(cvt_fh, nscript);
    size = nscript * sizeof(short);
    script = NuMemAllocFn(size, nugscn_srcfile, 0x246);
    NuFileRead(fh, script, size);
    NuFileWrite(cvt_fh, script, size);
    NuFileEndBlkWrite(cvt_fh);

    NuMemFreeFn(texanims, nugscn_srcfile, 0x24D);
    NuMemFreeFn(script, nugscn_srcfile, 0x24E);
}


extern int NuFileBeginBlkRead(int fh, int blk);
extern void NuFileEndBlkRead(int fh);
extern void NuTexSwizzelBits(struct nutex_s *tex);

extern int info_texturecount;
extern int info_texturesize;
extern char D_00614D90[];
extern char D_00614DB0[];
#define texsetheader_msg D_00614D90
#define nativetex_nosize_msg D_00614DB0

void CvtNuIFFTextureSet(int fh)
{
    struct nutex_s tex;
    unsigned char tmp[4];
    void *native;
    char *nativebuf;
    unsigned char *pal;
    unsigned char *col;
    int ntex;
    int imgsize;
    int palsize;
    unsigned int size;
    int tid;
    int pos;
    int pad;
    int i;
    int j;

    native = NuMemAllocFn(0x12007F, nugscn_srcfile, 0x25A);
    nativebuf = (char *)(((unsigned int)native + 0x7F) & 0xFFFFFF80);
    NuDebugMsgProlog(nugscn_srcfile, 0x25D)(texsetheader_msg);
    NuFileBeginBlkRead(fh, 0x30485354);
    ntex = NuFileReadInt(fh);
    NuFileEndBlkRead(fh);
    info_texturecount += ntex;
    NuFileBeginBlkWrite(cvt_fh, 0x30545354, 0);
    NuFileWriteInt(cvt_fh, ntex);
    NuFileWriteInt(cvt_fh, 0);
    for (i = 0; i < ntex; i++) {
        NuFileBeginBlkRead(fh, 0x304D5854);
        tex.type = NuFileReadInt(fh);
        tex.width = NuFileReadInt(fh);
        tex.height = NuFileReadInt(fh);
        tex.mmcnt = NuFileReadInt(fh);
        tex.pal = 0;
        if (tex.mmcnt == 1) {
            tex.mmcnt = 0;
        }
        tex.pal = 0;
        imgsize = NuTexImgSizeMM(tex.type, tex.width, tex.height, tex.mmcnt);
        tex.bits = NuMemAllocFn(imgsize, nugscn_srcfile, 0x27D);
        NuFileRead(fh, tex.bits, imgsize);
        palsize = NuTexPalSize(tex.type);
        if (palsize) {
            tex.pal = NuMemAllocFn(palsize, nugscn_srcfile, 0x285);
            NuFileRead(fh, tex.pal, palsize);
            pal = tex.pal;
            for (j = 0; j < palsize; j += 4) {
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
        NuFileEndBlkRead(fh);
        NuTexSwizzelBits(&tex);
        tid = NuTexCreate(&tex);
        size = NuTexRecoverNativeData(tid, nativebuf, 0x120000);
        if (size == 0) {
            NuErrorProlog(nugscn_srcfile, 0x29F)(nativetex_nosize_msg);
        }
        size = (size + 0xF) & 0xFFFFFFF0;
        info_texturesize += size;
        NuFileWriteInt(cvt_fh, size);
        pos = NuFilePos(cvt_fh);
        pad = ((pos + 0x7F) & 0xFFFFFF80) - pos;
        NuFileWriteInt(cvt_fh, pad - 4);
        for (j = 0; j < (pad - 4) / 4; j++) {
            NuFileWriteInt(cvt_fh, 0);
        }
        NuFileWrite(cvt_fh, nativebuf, size);
        NuTexDestroy(tid);
        if (tex.pal) {
            NuMemFreeFn(tex.pal, nugscn_srcfile, 0x2B6);
        }
        if (tex.bits) {
            NuMemFreeFn(tex.bits, nugscn_srcfile, 0x2B8);
        }
    }
    NuMemFreeFn(native, nugscn_srcfile, 0x2BA);
    NuFileEndBlkWrite(cvt_fh);
}


void CvtNuIFFSplineSet(int fh)
{
    int nsplines;
    int ncvs;
    int i;
    struct nuvec_s *cvs;

    NuDebugMsgProlog(nugscn_srcfile, 0x31E)(splineset_msg);
    nsplines = NuFileReadInt(fh);
    cvt_splines = NuMemAllocFn(nsplines * sizeof(struct nucvtspline_s), nugscn_srcfile, 0x324);
    cvt_numsplines = nsplines;
    if (nsplines != 0) {
        NuFileRead(fh, cvt_splines, nsplines * sizeof(struct nucvtspline_s));
        ncvs = NuFileReadInt(fh);
        if (ncvs != 0) {
            cvt_spline_cvs = NuMemAllocFn(ncvs * sizeof(struct nuvec_s), nugscn_srcfile, 0x32C);
            NuFileRead(fh, cvt_spline_cvs, ncvs * sizeof(struct nuvec_s));

            /* Point each spline at its control points. */
            cvs = cvt_spline_cvs;
            for (i = 0; i < nsplines; i++) {
                cvt_splines[i].cvs = cvs;
                cvs += cvt_splines[i].ncvs;
            }
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
    geom.faceon_type = NuFileReadInt(fh);
    NuFileWriteInt(cvt_fh, geom.faceon_type);
    geom.faceon_radius = NuFileReadFloat(fh);
    NuFileWriteFloat(cvt_fh, geom.faceon_radius);
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


void CvtNuIFFGeom(int fh, int write)
{
    int pos;
    int pad;
    int mtlid;
    int n;
    void *buff;

    if (write) {
        /* Pad so the geometry that follows (0x10 bytes on) starts on a
           128-byte boundary. */
        pos = NuFilePos(cvt_fh);
        pad = pos + 0x10;
        pad = ((pos + 0x8F) & 0xFFFFFF80) - pad;
        NuFileWriteInt(cvt_fh, 0);
        NuFileWriteInt(cvt_fh, pad);
    }
    mtlid = NuFileReadInt(fh);
    if (write) {
        NuFileWriteInt(cvt_fh, mtlid);
    }
    ReadNuIFFGeomVtx(fh);
    n = NuFileReadInt(fh);
    if (n) {
        buff = NuMemAllocFn(n * 0xC, nugscn_srcfile, 0x44C);
        NuFileRead(fh, buff, n * 0xC);
        NuMemFreeFn(buff, nugscn_srcfile, 0x451);
    }
    if (write) {
        for (; pad != 0; pad -= 4) {
            NuFileWriteInt(cvt_fh, 0);
        }
    }
    CvtNuIFFGeomPrim(fh, mtlid, write);
    ReadNuIFFGeomSkin(fh);
    NuFileReadInt(fh);
    NuMemFreeFn(cvt_vtxbuff, nugscn_srcfile, 0x4B7);
}

/* Copies the instance table (0x50 bytes each) and any animated-instance
   records (0x60 bytes each) straight through to the converted file. */
void CvtNuIFFInstSet(int fh)
{
    int ninstances;
    int nanims;
    void *instances;
    void *anims;

    ninstances = NuFileReadInt(fh);
    instances = NuMemAllocFn(ninstances * 0x50, nugscn_srcfile, 0x6EC);
    NuFileRead(fh, instances, ninstances * 0x50);
    NuFileBeginBlkWrite(cvt_fh, 0x54534E49, 0);
    NuFileWriteInt(cvt_fh, ninstances);
    NuFileWriteInt(cvt_fh, 0);
    NuFileWrite(cvt_fh, instances, ninstances * 0x50);
    NuFileEndBlkWrite(cvt_fh);

    nanims = NuFileReadInt(fh);
    if (nanims != 0) {
            anims = NuMemAllocFn(nanims * 0x60, nugscn_srcfile, 0x6FA);
        NuFileRead(fh, anims, nanims * 0x60);
        NuFileBeginBlkWrite(cvt_fh, 0x4C424149, 0);
        NuFileWriteInt(cvt_fh, nanims);
        NuFileWriteInt(cvt_fh, 0);
        NuFileWrite(cvt_fh, anims, nanims * 0x60);
        NuFileEndBlkWrite(cvt_fh);
        NuMemFreeFn(anims, nugscn_srcfile, 0x704);
    }
    NuMemFreeFn(instances, nugscn_srcfile, 0x707);
}


extern void NuFileInitAddress(int max);
extern void NuFileTidyAddress(void);
extern void NuFilePatchAddress(int fh);
extern void NuFileWriteAddress(int fh, void *id);
extern void NuFileSetAddress(int fh, void *id);
extern int NuFileAlign(int fh, int mask);
extern void NuFileWriteChar(int fh, char value);

void CvtNuIFFAnimationLibrary(int fh)
{
    struct nuanimdata_s **anims;
    struct nuanimdata_s *anim;
    struct nuanimdatachunk_s *chunk;
    struct nuanimcurveset_s *curveset;
    struct nuanimcurve_s *curve;
    unsigned char count;
    unsigned char i;
    unsigned char j;
    unsigned char k;
    unsigned char m;
    int nanims;

    nanims = 0;
    NuFileInitAddress(0x4000);
    count = NuFileReadChar(fh);
    if (count) {
        anims = NuMemAllocFn(count * sizeof(struct nuanimdata_s *), nugscn_srcfile, 0x72B);
        memset(anims, 0, count * sizeof(struct nuanimdata_s *));
        for (i = 0; i < count; i++) {
            if (NuFileReadChar(fh)) {
                anims[i] = NuAnimDataRead(fh);
            }
        }
        NuFileBeginBlkWrite(cvt_fh, 0x42494C41, 0);
        NuFileWriteInt(cvt_fh, count);
        NuFileWriteInt(cvt_fh, 0);
        for (i = 0; i < count; i++) {
            if (anims[i]) {
                NuFileWriteAddress(cvt_fh, anims[i]);
                nanims++;
            } else {
                NuFileWriteInt(cvt_fh, 0);
            }
        }
        NuFileAlign(fh, 0xF);
        NuFileWriteInt(cvt_fh, nanims);
        for (i = 0; i < count; i++) {
            anim = anims[i];
            if (anim == 0) {
                continue;
            }
            NuFileSetAddress(cvt_fh, anim);
            NuFileWriteFloat(cvt_fh, anim->time);
            NuFileWriteInt(cvt_fh, 0);
            NuFileWriteInt(cvt_fh, anim->nchunks);
            NuFileWriteAddress(cvt_fh, anim->chunks);
            NuFileSetAddress(cvt_fh, anim->chunks);
            for (j = 0; j < anim->nchunks; j++) {
                NuFileWriteAddress(cvt_fh, anim->chunks[j]);
            }
            NuFileAlign(fh, 0xF);
            for (j = 0; j < anim->nchunks; j++) {
                chunk = anim->chunks[j];
                NuFileSetAddress(cvt_fh, chunk);
                NuFileWriteInt(cvt_fh, chunk->numnodes);
                NuFileWriteInt(cvt_fh, chunk->num_valid_animcurvesets);
                NuFileWriteAddress(cvt_fh, chunk->animcurvesets);
                NuFileWriteInt(cvt_fh, 0);
                NuFileWriteInt(cvt_fh, 0);
                NuFileAlign(fh, 0xF);
                NuFileSetAddress(cvt_fh, chunk->animcurvesets);
                for (k = 0; k < chunk->numnodes; k++) {
                    NuFileWriteAddress(cvt_fh, chunk->animcurvesets[k]);
                }
                NuFileAlign(fh, 0xF);
                for (k = 0; k < chunk->numnodes; k++) {
                    curveset = chunk->animcurvesets[k];
                    NuFileSetAddress(cvt_fh, curveset);
                    NuFileWriteInt(cvt_fh, curveset->flags);
                    NuFileWriteAddress(cvt_fh, curveset->constants);
                    NuFileWriteAddress(cvt_fh, curveset->set);
                    NuFileWriteChar(cvt_fh, curveset->ncurves);
                    NuFileWriteChar(cvt_fh, curveset->pad[0]);
                    NuFileWriteChar(cvt_fh, curveset->pad[1]);
                    NuFileWriteChar(cvt_fh, curveset->pad[2]);
                    NuFileSetAddress(cvt_fh, curveset->set);
                    for (m = 0; m < curveset->ncurves; m++) {
                        NuFileWriteAddress(cvt_fh, curveset->set[m]);
                    }
                    NuFileAlign(fh, 0xF);
                    for (m = 0; m < curveset->ncurves; m++) {
                        curve = curveset->set[m];
                        if (curve) {
                            NuFileSetAddress(cvt_fh, curve);
                            NuFileWriteInt(cvt_fh, curve->mask.bits);
                            NuFileWriteAddress(cvt_fh, curve->animkeys);
                            NuFileWriteInt(cvt_fh, curve->numkeys);
                            NuFileWriteInt(cvt_fh, curve->flags);
                        }
                    }
                    NuFileAlign(fh, 0xF);
                    for (m = 0; m < curveset->ncurves; m++) {
                        curve = curveset->set[m];
                        if (curve) {
                            NuFileSetAddress(cvt_fh, curve->animkeys);
                            NuFileWrite(cvt_fh, curve->animkeys, curve->numkeys * 16);
                        }
                    }
                    NuFileSetAddress(cvt_fh, curveset->constants);
                    NuFileWrite(cvt_fh, curveset->constants, curveset->ncurves * sizeof(float));
                    NuFileAlign(fh, 0xF);
                }
            }
        }
        NuFileEndBlkWrite(cvt_fh);
        for (i = 0; i < count; i++) {
            if (anims[i]) {
                NuAnimDataDestroy(anims[i]);
            }
        }
        NuFilePatchAddress(cvt_fh);
        NuMemFreeFn(anims, nugscn_srcfile, 0x7AD);
    }
    NuFileTidyAddress();
}


extern char *nametable_base;
extern char *nametable_free;
extern int splcnt;

extern struct nuspecial_s *D_006330EC;
extern int D_006330F0;
extern int D_006330F8;
extern char **D_006330FC;
extern char *D_00633100;
extern char *D_00633108;
extern union variptr_u D_00633110;
extern char D_00614DD0[];
extern char D_00614DE8[];
#define cvt_specials D_006330EC
#define cvt_numspecials D_006330F0
#define cvt_numnames D_006330F8
#define cvt_names D_006330FC
#define cvt_namedata D_00633100
#define cvt_splinebuff D_00633108
#define cvt_splineend D_00633110
#define mtlset_msg D_00614DD0
#define gobjset_msg D_00614DE8

void CvtNuIFFScene(int fh)
{
    struct numtl_s *mtl;
    int blk;
    int nmtls;
    int ngobjs;
    int namesize;
    int size;
    int i;
    int j;
    int k;
    int pos;

    cvt_numinstances = 0;
    cvt_numspecials = 0;
    cvt_instances = NuMemAllocFn(4000 * sizeof(struct nuinstance_s), nugscn_srcfile, 0x7D7);
    cvt_specials = NuMemAllocFn(4000 * sizeof(struct nuspecial_s), nugscn_srcfile, 0x7D8);
    cvt_splineend.voidptr = cvt_splinebuff = NuMemAllocFn(0x80000, nugscn_srcfile, 0x7D9);
    splcnt = 0;
    while ((blk = NuFileBeginBlkRead(fh, 0)) != 0) {
        switch (blk) {
        case 0x30545354:
            CvtNuIFFTextureSet(fh);
            break;
        case 0x3030534D:
            NuDebugMsgProlog(nugscn_srcfile, 0x2CD)(mtlset_msg);
            nmtls = NuFileReadInt(fh);
            cvt_mtls = NuMemAllocFn(nmtls * sizeof(struct nucvtmtl_s), nugscn_srcfile, 0x2D3);
            for (i = 0; i < nmtls; i++) {
                mtl = NuMtlRead(fh);
                cvt_mtls[i] = *(struct nucvtmtl_s *)mtl;
                NuMtlDestroy(mtl);
            }
            NuFileBeginBlkWrite(cvt_fh, 0x3030534D, 0);
            NuFileWriteInt(cvt_fh, nmtls);
            NuFileWriteInt(cvt_fh, 0);
            NuFileWrite(cvt_fh, cvt_mtls, nmtls * sizeof(struct nucvtmtl_s));
            NuFileEndBlkWrite(cvt_fh);
            break;
        case 0x30545347:
            NuDebugMsgProlog(nugscn_srcfile, 0x2F4)(gobjset_msg);
            ngobjs = NuFileReadInt(fh);
            NuFileBeginBlkWrite(cvt_fh, 0x304A424F, 0);
            NuFileWriteInt(cvt_fh, ngobjs);
            if (nugscn_generate_colourref) {
                NuFileWriteInt(cvt_fh, 1);
            } else {
                NuFileWriteInt(cvt_fh, 0);
            }
            for (j = 0; j < ngobjs; j++) {
                CvtNuIFFGeomDef(fh);
            }
            NuFileEndBlkWrite(cvt_fh);
            break;
        case 0x30545353:
            CvtNuIFFSplineSet(fh);
            break;
        case 0x3054534E:
            cvt_numnames = NuFileReadInt(fh);
            namesize = NuFileReadInt(fh);
            cvt_names = NuMemAllocFn(cvt_numnames * sizeof(char *), nugscn_srcfile, 0x673);
            cvt_namedata = NuMemAllocFn(namesize, nugscn_srcfile, 0x674);
            NuFileRead(fh, cvt_namedata, namesize);
            for (k = 0; k < cvt_numnames; k++) {
                cvt_names[k] = cvt_namedata + NuFileReadInt(fh);
            }
            break;
        case 0x30524E4E:
            CvtNuIFFNodeHierarchy(fh);
            break;
        }
        NuFileEndBlkRead(fh);
    }

    size = nametable_free - nametable_base;
    if (size) {
        NuFileBeginBlkWrite(cvt_fh, 0x4C42544E, 0);
        NuFileWriteInt(cvt_fh, size);
        for (pos = 0; pos < size; pos++) {
            nametable_base[pos] = (nametable_base[pos] >= 'A' && nametable_base[pos] <= 'Z') ? nametable_base[pos] + 0x20 : nametable_base[pos];
        }
        NuFileWrite(cvt_fh, nametable_base, size);
        NuFileEndBlkWrite(cvt_fh);
    }
    if (cvt_numspecials) {
        NuFileBeginBlkWrite(cvt_fh, 0x43455053, 0);
        NuFileWriteInt(cvt_fh, cvt_numspecials);
        NuFileWriteInt(cvt_fh, 0);
        NuFileWrite(cvt_fh, cvt_specials, cvt_numspecials * sizeof(struct nuspecial_s));
        NuFileEndBlkWrite(cvt_fh);
    }
    if (splcnt) {
        NuFileBeginBlkWrite(cvt_fh, 0x30545353, 0);
        NuFileWriteInt(cvt_fh, splcnt);
        NuFileWriteInt(cvt_fh, cvt_splineend.charptr - cvt_splinebuff);
        NuFileWrite(cvt_fh, cvt_splinebuff, cvt_splineend.charptr - cvt_splinebuff);
        NuFileEndBlkWrite(cvt_fh);
    }
    if (cvt_splinebuff) {
        NuMemFreeFn(cvt_splinebuff, nugscn_srcfile, 0x7F2);
    }
}


extern char D_00614E20[];
#define splineconvert_msg D_00614E20

void CvtNuIFFGScene(int fh)
{
    struct numtl_s *mtl;
    void *splines;
    short *ids;
    char *names;
    void *specials;
    int blk;
    int nmtls;
    int ngobjs;
    int splsize;
    int nids;
    int idsize;
    int namesize;
    int nspecials;
    int specsize;
    int i;
    int j;

    cvt_numinstances = 0;
    cvt_numspecials = 0;
    cvt_instances = NuMemAllocFn(4000 * sizeof(struct nuinstance_s), nugscn_srcfile, 0x805);
    cvt_specials = NuMemAllocFn(4000 * sizeof(struct nuspecial_s), nugscn_srcfile, 0x806);
    cvt_splineend.voidptr = cvt_splinebuff = NuMemAllocFn(0x80000, nugscn_srcfile, 0x807);
    splcnt = 0;
    while ((blk = NuFileBeginBlkRead(fh, 0)) != 0) {
        switch (blk) {
        case 0x30545354:
            CvtNuIFFTextureSet(fh);
            break;
        case 0x3030534D:
            NuDebugMsgProlog(nugscn_srcfile, 0x2CD)(mtlset_msg);
            nmtls = NuFileReadInt(fh);
            cvt_mtls = NuMemAllocFn(nmtls * sizeof(struct nucvtmtl_s), nugscn_srcfile, 0x2D3);
            for (i = 0; i < nmtls; i++) {
                mtl = NuMtlRead(fh);
                cvt_mtls[i] = *(struct nucvtmtl_s *)mtl;
                NuMtlDestroy(mtl);
            }
            NuFileBeginBlkWrite(cvt_fh, 0x3030534D, 0);
            NuFileWriteInt(cvt_fh, nmtls);
            NuFileWriteInt(cvt_fh, 0);
            NuFileWrite(cvt_fh, cvt_mtls, nmtls * sizeof(struct nucvtmtl_s));
            NuFileEndBlkWrite(cvt_fh);
            break;
        case 0x30545347:
            NuDebugMsgProlog(nugscn_srcfile, 0x2F4)(gobjset_msg);
            ngobjs = NuFileReadInt(fh);
            NuFileBeginBlkWrite(cvt_fh, 0x304A424F, 0);
            NuFileWriteInt(cvt_fh, ngobjs);
            if (nugscn_generate_colourref) {
                NuFileWriteInt(cvt_fh, 1);
            } else {
                NuFileWriteInt(cvt_fh, 0);
            }
            for (j = 0; j < ngobjs; j++) {
                CvtNuIFFGeomDef(fh);
            }
            NuFileEndBlkWrite(cvt_fh);
            break;
        case 0x30545353:
            NuDebugMsgProlog(nugscn_srcfile, 0x347)(splineconvert_msg);
            NuFileBeginBlkWrite(cvt_fh, 0x30545353, 0);
            NuFileWriteInt(cvt_fh, NuFileReadInt(fh));
            splsize = NuFileReadInt(fh);
            NuFileWriteInt(cvt_fh, splsize);
            splines = NuMemAllocFn(splsize, nugscn_srcfile, 0x355);
            NuFileRead(fh, splines, splsize);
            NuFileWrite(cvt_fh, splines, splsize);
            NuFileEndBlkWrite(cvt_fh);
            NuMemFreeFn(splines, nugscn_srcfile, 0x35B);
            break;
        case 0x54534E49:
            CvtNuIFFInstSet(fh);
            break;
        case 0x44494E49:
            nids = NuFileReadInt(fh);
            idsize = nids * sizeof(short);
            ids = NuMemAllocFn(idsize, nugscn_srcfile, 0x713);
            NuFileRead(fh, ids, idsize);
            NuFileBeginBlkWrite(cvt_fh, 0x44494E49, 0);
            NuFileWriteInt(cvt_fh, nids);
            NuFileWriteInt(cvt_fh, 0);
            NuFileWrite(cvt_fh, ids, idsize);
            NuFileEndBlkWrite(cvt_fh);
            break;
        case 0x42494C41:
            CvtNuIFFAnimationLibrary(fh);
            break;
        case 0x4C42544E:
            namesize = NuFileReadInt(fh);
            names = NuMemAllocFn(namesize, nugscn_srcfile, 0x68D);
            NuFileRead(fh, names, namesize);
            NuFileBeginBlkWrite(cvt_fh, 0x4C42544E, 0);
            NuFileWriteInt(cvt_fh, namesize);
            NuFileWrite(cvt_fh, names, namesize);
            NuFileEndBlkWrite(cvt_fh);
            NuMemFreeFn(names, nugscn_srcfile, 0x696);
            break;
        case 0x43455053:
            nspecials = NuFileReadInt(fh);
            if (nspecials) {
                specsize = nspecials * sizeof(struct nuspecial_s);
                specials = NuMemAllocFn(specsize, nugscn_srcfile, 0x7BA);
                NuFileRead(fh, specials, specsize);
                NuFileBeginBlkWrite(cvt_fh, 0x43455053, 0);
                NuFileWriteInt(cvt_fh, nspecials);
                NuFileWriteInt(cvt_fh, 0);
                NuFileWrite(cvt_fh, specials, specsize);
                NuFileEndBlkWrite(cvt_fh);
                NuMemFreeFn(specials, nugscn_srcfile, 0x7C4);
            }
            break;
        case 0x30534154:
            CvtNuIFFTexAnimSet(fh);
            break;
        }
        NuFileEndBlkRead(fh);
    }
    if (cvt_splinebuff) {
        NuMemFreeFn(cvt_splinebuff, nugscn_srcfile, 0x81F);
    }
}


extern int NuFileOpen(char *name, int mode);
extern void NuFileClose(int fh);

extern int info_stripcount;
extern int info_pktcnt;
extern int D_0062EE2C;
extern char D_00614ED8[];
extern char D_00614F18[];
extern char D_00614F38[];
extern char D_00614F60[];
extern char D_00614F88[];
extern char D_00614FA8[];
extern char D_00614FD0[];
#define nugscn_version D_0062EE2C
#define stats_rule_msg D_00614ED8
#define stats_strips_msg D_00614F18
#define stats_striptris_msg D_00614F38
#define stats_tris_msg D_00614F60
#define stats_textures_msg D_00614F88
#define stats_texmem_msg D_00614FA8
#define stats_colourref_msg D_00614FD0

int NuGScnConvertScene(char *outfile, char *infile)
{
    char *dmabuf;
    int fh;

    info_tristripcount = 0;
    info_tricount = 0;
    info_stripcount = 0;
    info_texturecount = 0;
    info_texturesize = 0;
    info_pktcnt = 0;
    cvt_mtls = 0;
    nametable_base = nametable_free = NuMemAllocFn(0x4000, nugscn_srcfile, 0x83D);
    dmabuf = NuMemAllocFn(0x400004, nugscn_srcfile, 0x83E);
    cvt_dmabuffer = (char *)(((unsigned int)dmabuf + 0x7F) & ~0x7F);
    cvt_fh = NuFileOpen(outfile, 1);
    if (cvt_fh) {
        NuFileBeginBlkWrite(cvt_fh, 0x3032554E, 1);
        NuFileWriteInt(cvt_fh, nugscn_version);
        NuFileWriteInt(cvt_fh, 0);
        fh = NuFileOpen(infile, 0);
        if (fh) {
            switch (NuFileBeginBlkRead(fh, 0)) {
            case 0x3032554E:
                CvtNuIFFScene(fh);
                break;
            case 0x30435347:
                CvtNuIFFGScene(fh);
                break;
            }
            NuFileEndBlkRead(fh);
            NuFileClose(fh);
        }
        NuFileEndBlkWrite(cvt_fh);
        NuFileClose(cvt_fh);
        if (cvt_mtls) {
            NuMemFreeFn(cvt_mtls, nugscn_srcfile, 0x85D);
        }
        if (dmabuf) {
            NuMemFreeFn(dmabuf, nugscn_srcfile, 0x860);
        }
    }
    NuDebugMsgProlog(nugscn_srcfile, 0x863)(stats_rule_msg);
    NuDebugMsgProlog(nugscn_srcfile, 0x864)(stats_strips_msg, info_stripcount);
    NuDebugMsgProlog(nugscn_srcfile, 0x865)(stats_striptris_msg, info_tristripcount);
    NuDebugMsgProlog(nugscn_srcfile, 0x866)(stats_tris_msg, info_tricount);
    NuDebugMsgProlog(nugscn_srcfile, 0x867)(stats_textures_msg, info_texturecount);
    NuDebugMsgProlog(nugscn_srcfile, 0x868)(stats_texmem_msg, info_texturesize);
    NuDebugMsgProlog(nugscn_srcfile, 0x869)(stats_colourref_msg, info_colourrefsize);
    NuDebugMsgProlog(nugscn_srcfile, 0x86A)(stats_rule_msg);
    return 1;
}


extern unsigned int NuFileReadUnsignedInt(int fh);
extern char D_00614FF8[];
#define unknown_node_msg D_00614FF8

struct nunode_s *NuNodeRead(int fh, struct nunode_s *parent)
{
    struct nunode_s *node;
    struct nutransformnode_s *tnode;
    struct nujointnode_s *joint;
    struct nuskinnode_s *skin;
    struct nusplinenode_s *spline;
    struct numtx_s mtx;
    int *joints;
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
        node = &NuMeshNodeCreate((void *)NuFileReadInt(fh))->node;
        break;
    case 0x3030544E:
        NuFileBeginBlkRead(fh, 0x3044544E);
        NuFileRead(fh, &mtx, sizeof(struct numtx_s));
        tnode = NuTransformNodeCreate(&mtx);
        tnode->flags = NuFileReadInt(fh);
        if (tnode->node.type == NUNODETYPE_TRANSFORM && (tnode->flags & 2)) {
            tnode->node.data = (void *)NuFileReadInt(fh);
        }
        NuFileEndBlkRead(fh);
        node = &tnode->node;
        break;
    case 0x304B534E:
        NuFileBeginBlkRead(fh, 0x3044534E);
        NuFileReadInt(fh);
        skin = NuSkinNodeCreate(NuFileReadInt(fh));
        joints = skin->joint_ids;
        for (i = 0; i < skin->num_joints; i++) {
            *joints++ = NuFileReadInt(fh);
        }
        NuFileEndBlkRead(fh);
        skin->geometry = 0;
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
            spline->splines[j] = (struct nuspline_s *)NuFileReadInt(fh);
        }
        NuFileEndBlkRead(fh);
        break;
    default:
        NuErrorProlog(nugscn_srcfile, 0x980)(unknown_node_msg);
        break;
    }
    NuFileBeginBlkRead(fh, 0x30444E4E);
    node->id = NuFileReadUnsignedInt(fh);
    nameix = NuFileReadInt(fh);
    if (nameix == 0) {
        node->name = 0;
    } else {
        node->name = cvt_names[nameix - 1];
    }
    NuFileEndBlkRead(fh);
    NuFileEndBlkRead(fh);
    NuFileBeginBlkRead(fh, 0x30434E4E);
    nchildren = NuFileReadInt(fh);
    NuFileEndBlkRead(fh);
    for (j = 0; j < nchildren; j++) {
        NuNodeAddChild(node, NuNodeRead(fh, node));
    }
    return node;
}


extern char D_00615018[];
extern char D_00615030[];
#define animated_xform_msg D_00615018
#define spline_overflow_msg D_00615030

void CvtNodes(struct nunode_s *node, struct nunode_s *parent)
{
    struct numtx_s mtx;
    struct nuvec_s pt;
    struct nunode_s *p;
    struct nusplinenode_s *splnode;
    struct nucvtspline_s *spl;
    int i;

    for (; node != 0; node = node->next) {
        if (node->type == NUNODETYPE_MESH) {
            NuMtxSetIdentity(&mtx);
            for (p = parent; p != 0; p = p->parent) {
                if (p->type == NUNODETYPE_TRANSFORM) {
                    if (((struct nutransformnode_s *)p)->flags & 2) {
                        NuDebugMsgProlog(nugscn_srcfile, 0x9AF)(animated_xform_msg);
                    }
                    NuMtxMul(&mtx, &((struct nutransformnode_s *)p)->mtx, &mtx);
                }
            }
            memset(&cvt_instances[cvt_numinstances], 0, sizeof(struct nuinstance_s));
            cvt_instances[cvt_numinstances].mtx = mtx;
            cvt_instances[cvt_numinstances].objid = (int)((struct numeshnode_s *)node)->geometry;
            cvt_instances[cvt_numinstances].flags.visible = 1;
            if (node->name != 0) {
                cvt_specials[cvt_numspecials].instance = (struct nuinstance_s *)cvt_numinstances;
                cvt_specials[cvt_numspecials].name = (char *)(nametable_free - nametable_base);
                strcpy(nametable_free, node->name);
                nametable_free += strlen(node->name) + 1;
                cvt_numspecials++;
            }
            cvt_numinstances++;
        }
        if (node->type == NUNODETYPE_SPLINE) {
            NuMtxSetIdentity(&mtx);
            for (p = parent; p != 0; p = p->parent) {
                if (p->type == NUNODETYPE_TRANSFORM) {
                    if (((struct nutransformnode_s *)p)->flags & 2) {
                        NuDebugMsgProlog(nugscn_srcfile, 0x9DB)(animated_xform_msg);
                    }
                    NuMtxMul(&mtx, &((struct nutransformnode_s *)p)->mtx, &mtx);
                }
            }
            splnode = (struct nusplinenode_s *)node;
            if (node->name != 0 && splnode->num_splines != 0) {
                spl = &cvt_splines[(int)splnode->splines[0]];
                *((int *)cvt_splineend.voidptr)++ = spl->ncvs;
                *((int *)cvt_splineend.voidptr)++ = nametable_free - nametable_base;
                for (i = 0; i < spl->ncvs; i++) {
                    pt.x = spl->cvs[i].x;
                    pt.y = spl->cvs[i].y;
                    pt.z = spl->cvs[i].z;
                    NuVecMtxTransform(&pt, &pt, &mtx);
                    *((struct nuvec_s *)cvt_splineend.voidptr)++ = pt;
                }
                strcpy(nametable_free, node->name);
                nametable_free += strlen(node->name) + 1;
                splcnt++;
            }
            cvt_numinstances++;
            if ((unsigned int)(cvt_splineend.charptr - cvt_splinebuff) > 0x80000) {
                NuErrorProlog(nugscn_srcfile, 0xA06)(spline_overflow_msg);
            }
        }
        if (node->child != 0) {
            CvtNodes(node->child, node);
        }
    }
}

extern char D_00615060[];
#define texset_msg D_00615060
extern int NuFileSeek(int fh, int offset, int origin);
extern int NuMemFileSeek(int fh, int offset, int origin);

/* Carves `size` bytes, aligned to `align`, off the scene load buffer. */
static inline union variptr_u GScnBufferAlloc(int size, int align)
{
    union variptr_u buf;
    char *end;

    buf.charptr = (char *)(((int)*gscn_buffer + align - 1) & ~(align - 1));
    end = buf.charptr + size;
    if (end < gscn_bufferlimit.charptr) {
        *gscn_buffer = end;
        return buf;
    }
    NuErrorProlog(nugscn_srcfile, 0x184)(bufferalloc_failed_msg);
    buf.voidptr = 0;
    return buf;
}

void ReadTextureSet(int fh)
{
    int ntex;
    int size;
    int skip;
    void *data;
    int i;

    NuDebugMsgProlog(nugscn_srcfile, 0xA90)(texset_msg);
    ntex = NuFileReadInt(fh);
    NuFileReadInt(fh);
    loading_scene->tids = GScnBufferAlloc(ntex * sizeof(int), 4).voidptr;
    memset(loading_scene->tids, 0, ntex * sizeof(int));
    loading_scene->numtid = ntex;
    for (i = 0; i < ntex; i++) {
        size = NuFileReadInt(fh);
        skip = NuFileReadInt(fh);
        if (skip == 0) {
            NuFileReadInt(fh);
            NuFileReadInt(fh);
        } else {
            NuFileSeek(fh, skip, 1);
        }
        data = NuMemFileAddr(fh);
        NuMemFileSeek(fh, size, 1);
        loading_scene->tids[i] = NuTexCreateNative(data);
    }
}

extern void NuDisableVBlank(void);
extern void NuEnableVBlank(void);
extern NuMtlReadEventFunc NuMtlReadEvent;
extern int nugscn_mtlupdate_enabled;
extern int in_protected_bit;
extern char D_00615088[];
extern char D_006150A8[];
#define mtlset_read_msg D_00615088
#define mtlset_count_msg D_006150A8

void ReadMaterialSet(int fh)
{
    struct numtl_s *mtl;
    struct numtl_s *next;
    unsigned int ix;
    int nmtls;
    int i;

    NuDebugMsgProlog(nugscn_srcfile, 0xADC)(mtlset_read_msg);
    nmtls = NuFileReadInt(fh);
    NuFileReadInt(fh);
    loading_scene->mtls = GScnBufferAlloc(nmtls * sizeof(struct numtl_s *), 4).voidptr;
    loading_scene->nummtl = nmtls;
    for (i = 0; i < nmtls; i++) {
        loading_scene->mtls[i] = NuMemFileAddr(fh);
        NuMemFileSeek(fh, sizeof(struct numtl_s), 1);
        if (loading_scene->mtls[i]->tid == -1) {
            loading_scene->mtls[i]->tid = 0;
        } else {
            loading_scene->mtls[i]->tid = loading_scene->tids[loading_scene->mtls[i]->tid];
        }
        if ((int)loading_scene->mtls[i]->next == -1) {
            loading_scene->mtls[i]->next = 0;
        }
        next = loading_scene->mtls[i]->next;
        if (nugscn_mtlupdate_enabled) {
            NuDisableVBlank();
            in_protected_bit = 1;
            NuMtlAdd(loading_scene->mtls[i]);
            loading_scene->mtls[i]->next = next;
            if (!NuMtlReadEvent(loading_scene->mtls[i])) {
                NuMtlUpdate(loading_scene->mtls[i]);
            }
            in_protected_bit = 0;
            NuEnableVBlank();
        }
    }
    for (i = 0; i < nmtls; i++) {
        mtl = loading_scene->mtls[i];
        ix = (unsigned int)mtl->next;
        if (ix == 0xFFFFFFFF) {
            mtl->next = 0;
        } else if (ix >= 1 && ix <= 0xFFFF) {
            mtl->next = loading_scene->mtls[ix - 1];
        }
    }
    NuDebugMsgProlog(nugscn_srcfile, 0xB1A)(mtlset_count_msg, nmtls);
}


struct nucvttriblock_s {
    unsigned int tag;               /* 0x00 */
    void *data;                     /* 0x04 */
    char pad08[0x10 - 0x08];
    u128 qw[9];                     /* 0x10 */
}; /* 0xA0 */

struct nucvttribatch_s {
    int count;                      /* 0x00 */
    int pad04[3];
    struct nucvttriblock_s blocks[1]; /* 0x10 */
};

extern void NuCvtGeoFixup(struct nucolourblend_s *ref, int offset);
extern char D_006150C8[];
extern char D_006150E0[];
extern char D_006150F0[];
extern char D_00615118[];
#define objset_msg D_006150C8
#define geom_size_msg D_006150E0
#define dma_align_msg D_006150F0
#define unknown_gobj_msg D_00615118

#define DIST2(dx, dy, dz) ((dx) * (dx) + (dy) * (dy) + (dz) * (dz))
#define KEEP_MAX(dst, val) ((dst) = ((val) < (dst)) ? (dst) : (val))

void ReadObjSet(int fh)
{
    struct nuvec_s origin;
    struct nugobj_s *gobj;
    struct nugeom_s *geom;
    struct nufaceongeom_s *fgeom;
    int ngobjs;
    int nsub;
    int colourref;
    int tricount;
    int count;
    int skip;
    int size;
    int i;
    int j;
    int k;
    int m;

    memset(&origin, 0, sizeof(struct nuvec_s));
    NuDebugMsgProlog(nugscn_srcfile, 0xB5C)(objset_msg);
    ngobjs = NuFileReadInt(fh);
    colourref = NuFileReadInt(fh);
    loading_scene->gobjs = GScnBufferAlloc(ngobjs * sizeof(struct nugobj_s *), 4).voidptr;
    memset(loading_scene->gobjs, 0, ngobjs * sizeof(struct nugobj_s *));
    loading_scene->numgobj = ngobjs;
    for (i = 0; i < ngobjs; i++) {
        nsub = NuFileReadInt(fh);
        NuFileReadInt(fh);
        NuFileReadInt(fh);
        NuFileReadInt(fh);
        gobj = GScnBufferAlloc(nsub * sizeof(struct nugobj_s), 4).voidptr;
        loading_scene->gobjs[i] = gobj;
        memset(gobj, 0, nsub * sizeof(struct nugobj_s));
        gobj->ngobjs = nsub;
        for (j = 0; j < nsub; j++) {
            switch (gobj->type = NuFileReadInt(fh)) {
            case 0:
                count = NuFileReadInt(fh);
                NuFileRead(fh, &gobj->origin, sizeof(struct nuvec_s));
                geom = GScnBufferAlloc(count * sizeof(struct nugeom_s), 4).voidptr;
                memset(geom, 0, count * sizeof(struct nugeom_s));
                tricount = NuFileReadInt(fh);
                NuFileReadInt(fh);
                NuFileReadInt(fh);
                gobj->geom = geom;
                memset(geom, 0, count * sizeof(struct nugeom_s));
                for (k = 0; k < count; k++, geom++) {
                    if (k != count - 1) {
                        geom->next = geom + 1;
                    }
                    NuFileReadInt(fh);
                    skip = NuFileReadInt(fh);
                    geom->mtl = loading_scene->mtls[NuFileReadInt(fh)];
                    NuMemFileSeek(fh, skip, 1);
                    size = NuFileReadInt(fh);
                    if (size & 0xF) {
                        NuErrorProlog(nugscn_srcfile, 0xB95)(geom_size_msg);
                    }
                    if (geom->mtl->attrib.ot == 1) {
                        geom->otbatch = NuMemFileAddr(fh);
                        for (m = 0; m < ((struct nucvttribatch_s *)geom->otbatch)->count; m++) {
                            ((struct nucvttribatch_s *)geom->otbatch)->blocks[m].data = ((struct nucvttribatch_s *)geom->otbatch)->blocks[m].qw;
                        }
                    } else {
                        struct nudmatag_s *tag;
                        unsigned int base;

                        geom->dmastream = NuMemFileAddr(fh);
                        if ((int)geom->dmastream & 0x7F) {
                            NuDebugMsgProlog(nugscn_srcfile, 0xBA0)(dma_align_msg);
                        }
                        tag = geom->dmastream;
                        base = (unsigned int)tag;
                        while (tag->id == 0x20) {
                            tag->addr += base;
                            tag = (struct nudmatag_s *)tag->addr;
                        }
                    }
                    NuMemFileSeek(fh, size, 1);
                }
                gobj->tricount = tricount;
                break;
            case 1:
                count = NuFileReadInt(fh);
                NuFileReadInt(fh);
                NuFileReadInt(fh);
                gobj->origin = origin;
                fgeom = GScnBufferAlloc(count * sizeof(struct nufaceongeom_s), 4).voidptr;
                memset(fgeom, 0, count * sizeof(struct nufaceongeom_s));
                gobj->faceon_geom = fgeom;
                for (k = 0; k < count; k++, fgeom++) {
                    if (k != count - 1) {
                        fgeom->next = fgeom + 1;
                    }
                    fgeom->mtl = loading_scene->mtls[NuFileReadInt(fh)];
                    fgeom->faceon_type = NuFileReadInt(fh);
                    fgeom->faceon_radius = NuFileReadFloat(fh);
                    size = NuFileReadInt(fh);
                    fgeom->dmastream = NuMemFileAddr(fh);
                    NuMemFileSeek(fh, size, 1);
                }
                break;
            default:
                NuErrorProlog(nugscn_srcfile, 0xBD2)(unknown_gobj_msg);
                break;
            }
            NuFileRead(fh, &gobj->min, sizeof(struct nuvec_s));
            NuFileRead(fh, &gobj->max, sizeof(struct nuvec_s));
            NuFileReadInt(fh);
            NuFileReadInt(fh);
            gobj->cntr.x = (gobj->min.x + gobj->max.x) * 0.5f;
            gobj->cntr.y = (gobj->min.y + gobj->max.y) * 0.5f;
            gobj->cntr.z = (gobj->min.z + gobj->max.z) * 0.5f;
            gobj->r2 = DIST2(gobj->min.x, gobj->min.y, gobj->min.z);
            KEEP_MAX(gobj->r2, DIST2(gobj->max.x, gobj->min.y, gobj->min.z));
            KEEP_MAX(gobj->r2, DIST2(gobj->min.x, gobj->max.y, gobj->min.z));
            KEEP_MAX(gobj->r2, DIST2(gobj->max.x, gobj->max.y, gobj->min.z));
            KEEP_MAX(gobj->r2, DIST2(gobj->min.x, gobj->min.y, gobj->max.z));
            KEEP_MAX(gobj->r2, DIST2(gobj->max.x, gobj->min.y, gobj->max.z));
            KEEP_MAX(gobj->r2, DIST2(gobj->min.x, gobj->max.y, gobj->max.z));
            KEEP_MAX(gobj->r2, DIST2(gobj->max.x, gobj->max.y, gobj->max.z));
            gobj->r = sqrt(gobj->r);
            gobj->cntr_r2 = DIST2(gobj->min.x - gobj->cntr.x, gobj->min.y - gobj->cntr.y, gobj->min.z - gobj->cntr.z);
            KEEP_MAX(gobj->cntr_r2, DIST2(gobj->max.x - gobj->cntr.x, gobj->min.y - gobj->cntr.y, gobj->min.z - gobj->cntr.z));
            KEEP_MAX(gobj->cntr_r2, DIST2(gobj->min.x - gobj->cntr.x, gobj->max.y - gobj->cntr.y, gobj->min.z - gobj->cntr.z));
            KEEP_MAX(gobj->cntr_r2, DIST2(gobj->max.x - gobj->cntr.x, gobj->max.y - gobj->cntr.y, gobj->min.z - gobj->cntr.z));
            KEEP_MAX(gobj->cntr_r2, DIST2(gobj->min.x - gobj->cntr.x, gobj->min.y - gobj->cntr.y, gobj->max.z - gobj->cntr.z));
            KEEP_MAX(gobj->cntr_r2, DIST2(gobj->max.x - gobj->cntr.x, gobj->min.y - gobj->cntr.y, gobj->max.z - gobj->cntr.z));
            KEEP_MAX(gobj->cntr_r2, DIST2(gobj->min.x - gobj->cntr.x, gobj->max.y - gobj->cntr.y, gobj->max.z - gobj->cntr.z));
            KEEP_MAX(gobj->cntr_r2, DIST2(gobj->max.x - gobj->cntr.x, gobj->max.y - gobj->cntr.y, gobj->max.z - gobj->cntr.z));
            gobj->cntr_r = sqrt(gobj->cntr_r2);
            gobj++;
        }
        loading_scene->gobjs[i]->blend = 0;
        loading_scene->gobjs[i]->faceon_blend = 0;
        if (colourref == 1) {
            loading_scene->gobjs[i]->tricount = NuFileReadInt(fh);
            NuFileReadInt(fh);
            NuFileReadInt(fh);
            NuFileReadInt(fh);
            if (loading_scene->gobjs[i]->tricount != 0) {
                loading_scene->gobjs[i]->blend = NuMemFileAddr(fh);
                NuMemFileSeek(fh, loading_scene->gobjs[i]->tricount, 1);
                NuCvtGeoFixup(loading_scene->gobjs[i]->blend, (int)gscn_filebuffer);
            }
        }
    }
}

void ReadNuIFFTexAnimSet(int fh, struct nugscn_s *gsc, short *tids)
{
    struct nutexanim_s *tex;
    int size;
    int i;
    int j;

    NuDebugMsgProlog(nugscn_srcfile, 0xC64)(texanimset_msg);
    gsc->numtexanims = NuFileReadInt(fh);
    NuFileReadInt(fh);
    gsc->texanims = NuMemAllocFn(gsc->numtexanims * sizeof(struct nutexanim_s), nugscn_srcfile, 0xC6B);
    NuFileRead(fh, gsc->texanims, gsc->numtexanims * sizeof(struct nutexanim_s));
    size = NuFileReadInt(fh) * sizeof(short);
    gsc->texanim_tids = NuMemAllocFn(size, nugscn_srcfile, 0xC70);
    NuFileRead(fh, gsc->texanim_tids, size);

    for (i = 0; i < gsc->numtexanims; i++) {
        tex = &gsc->texanims[i];
        tex->succ = 0;
        tex->prev = 0;
        tex->tids = gsc->texanim_tids + (int)tex->tids;
        tex->dynalloc = 0;
        for (j = 0; j < tex->numtids; j++) {
            tex->tids[j] = tids[tex->tids[j]];
        }
        tex->ntaname = gsc->nametable + (int)tex->ntaname;
        tex->scriptname = gsc->nametable + (int)tex->scriptname;
        tex->mtl = gsc->mtls[(int)tex->mtl];
        tex->env = NuTexAnimEnvCreate(0, tex->mtl, tex->tids, NuTexAnimProgFind(tex->scriptname));
    }

    for (i = 0; i < gsc->numtexanims - 1; i++) {
        gsc->texanims[i].succ = &gsc->texanims[i] + 1;
        gsc->texanims[i + 1].prev = &gsc->texanims[i];
    }
    NuTexAnimAddList(gsc->texanims);
}

extern int D_00633090;
#define texanim_ix D_00633090

void ReadTexAnimSet(int fh)
{
    struct nutexanim_s *tex;
    short *tidtab;
    char *start;
    union variptr_u buff;
    char *scriptname;
    void *prog;
    int j;

    NuDebugMsgProlog(nugscn_srcfile, 0xC2C)(texanimset_msg);
    loading_scene->numtexanims = NuFileReadInt(fh);
    NuFileReadInt(fh);
    loading_scene->texanims = NuMemFileAddr(fh);
    NuMemFileSeek(fh, loading_scene->numtexanims * sizeof(struct nutexanim_s), 1);
    NuFileReadInt(fh);
    tidtab = NuMemFileAddr(fh);

    for (texanim_ix = 0; texanim_ix < loading_scene->numtexanims; texanim_ix++) {
        tex = &loading_scene->texanims[texanim_ix];
        tex->succ = 0;
        tex->prev = 0;
        tex->tids = tidtab + (int)tex->tids;
        for (j = 0; j < tex->numtids; j++) {
            tex->tids[j] = loading_scene->tids[tex->tids[j]];
        }
        tex->ntaname = loading_scene->nametable + (int)tex->ntaname;
        scriptname = loading_scene->nametable + (int)tex->scriptname;
        tex->mtl = loading_scene->mtls[(int)tex->mtl];
        tex->dynalloc = 0;
        prog = NuTexAnimProgFind(scriptname);
        start = buff.charptr = (char *)(((int)*gscn_buffer + 0xF) & ~0xF);
        tex->env = NuTexAnimEnvCreate(&buff, tex->mtl, tex->tids, prog);
        GScnBufferAlloc(buff.charptr - start, 0x10);
    }

    for (texanim_ix = 0; texanim_ix < loading_scene->numtexanims - 1; texanim_ix++) {
        loading_scene->texanims[texanim_ix].succ = &loading_scene->texanims[texanim_ix] + 1;
        loading_scene->texanims[texanim_ix + 1].prev = &loading_scene->texanims[texanim_ix];
    }
    NuTexAnimAddList(loading_scene->texanims);
}


inline int FindSplinePtSize(char *name)
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


extern char D_00614D20[];
#define tempalloc_failed_msg D_00614D20

static inline union variptr_u GScnTempAlloc(int size)
{
    union variptr_u buf;

    if (temp_workspace.voidptr != 0) {
        if (temp_workspace.charptr + size + 4 < gscn_bufferlimit.charptr) {
            gscn_bufferlimit.charptr = (char *)(((int)gscn_bufferlimit.charptr - size) & ~3);
            buf.charptr = gscn_bufferlimit.charptr;
            return buf;
        }
    } else if (*gscn_buffer + size + 4 < gscn_bufferlimit.charptr) {
        gscn_bufferlimit.charptr = (char *)(((int)gscn_bufferlimit.charptr - size) & ~3);
        buf.charptr = gscn_bufferlimit.charptr;
        return buf;
    }
    NuErrorProlog(nugscn_srcfile, 0x16F)(tempalloc_failed_msg);
    buf.voidptr = 0;
    return buf;
}

void ReadSplineSet(int fh)
{
    char *data;
    char *pts;
    int size;
    int i;
    int j;

    loading_scene->numsplines = NuFileReadInt(fh);
    size = NuFileReadInt(fh);
    loading_scene->splines = GScnBufferAlloc(loading_scene->numsplines * sizeof(struct nugspline_s), 4).voidptr;
    data = GScnTempAlloc(size).charptr;
    NuFileRead(fh, data, size);
    for (i = 0; i < loading_scene->numsplines; i++) {
        loading_scene->splines[i].len = *(unsigned short *)data;
        data += sizeof(int);
        loading_scene->splines[i].name = (char *)(*(int *)data + (int)loading_scene->nametable);
        data += sizeof(int);
        loading_scene->splines[i].ptsize = FindSplinePtSize(loading_scene->splines[i].name);
        loading_scene->splines[i].pts = GScnBufferAlloc(loading_scene->splines[i].ptsize * loading_scene->splines[i].len, 4).voidptr;
        pts = loading_scene->splines[i].pts;
        for (j = 0; j < loading_scene->splines[i].len; j++) {
            *(struct nuvec_s *)pts = *(struct nuvec_s *)data;
            data += sizeof(struct nuvec_s);
            pts += loading_scene->splines[i].ptsize;
        }
    }
    if (temp_workspace.voidptr != 0) {
        gscn_bufferlimit.charptr = (char *)temp_workspace_size;
    } else {
        gscn_bufferlimit.charptr = gscn_bufferend;
    }
}

extern unsigned int timeUserRead(void);

extern char D_0062EE60[];
extern char D_0062EE68[];
extern char D_0062EE70[];
extern char D_0062EE78[];
extern char D_0062EE80[];
extern char D_0062EE88[];
extern char D_0062EE90[];
extern char D_0062EE98[];
extern char D_0062EEA0[];
extern char D_00615150[];
extern char D_00615160[];
extern char D_00615170[];
extern char D_00615180[];
extern char D_00615190[];
extern char D_006151A0[];
#define blkname_texset2 D_0062EE60
#define blkname_texset D_0062EE68
#define blkname_mtlset2 D_0062EE70
#define blkname_mtlset D_0062EE78
#define blkname_objset D_0062EE80
#define blkname_instset D_0062EE88
#define blkname_animlib D_0062EE90
#define blkname_instids D_0062EE98
#define blkname_unknown D_0062EEA0
#define blkname_nametable D_00615150
#define blkname_specialset D_00615160
#define blkname_splineset D_00615170
#define blkname_instanimblock D_00615180
#define blkname_texanimset D_00615190
#define blktype_msg D_006151A0

void DbgBlkType(int blk)
{
    char *name;
    unsigned int tod;

    switch (blk) {
    case 0x32545354:
        name = blkname_texset2;
        break;
    case 0x30545354:
        name = blkname_texset;
        break;
    case 0x3230534D:
        name = blkname_mtlset2;
        break;
    case 0x3030534D:
        name = blkname_mtlset;
        break;
    case 0x304A424F:
        name = blkname_objset;
        break;
    case 0x54534E49:
        name = blkname_instset;
        break;
    case 0x4C42544E:
        name = blkname_nametable;
        break;
    case 0x43455053:
        name = blkname_specialset;
        break;
    case 0x30545353:
        name = blkname_splineset;
        break;
    case 0x4C424149:
        name = blkname_instanimblock;
        break;
    case 0x42494C41:
        name = blkname_animlib;
        break;
    case 0x44494E49:
        name = blkname_instids;
        break;
    case 0x30534154:
        name = blkname_texanimset;
        break;
    default:
        name = blkname_unknown;
        break;
    }
    tod = timeUserRead();
    NuDebugMsgProlog(nugscn_srcfile, 0xEE0)(blktype_msg, tod, name);
}


extern void timeUserReset(void);
extern int NuMemFileOpen(void *buf, int size, int mode);
extern void NuMemFileClose(int fh);
extern void NuRndrSplineSearch(void *scene);
extern char D_006151B8[];
extern char D_006151D8[];
extern char D_00615208[];
#define scene_alloc_msg D_006151B8
#define scene_old_msg D_006151D8
#define loader_old_msg D_00615208

struct nugscn_s *NuGScnLoadFix(void)
{
    struct nuspecial_s *special;
    struct nuspecial_s *src;
    char *data;
    char *filename;
    union variptr_u *buffer;
    int size;
    int fh;
    int version;
    int blk;
    int count;
    int i;
    int j;
    int k;
    int m;
    struct nuinstanim_s *anim;

    loading_scene = 0;
    timeUserReset();
    size = gscn_load.size;
    buffer = gscn_load.buffer;
    filename = gscn_load.filename;
    if (size > 0) {
        data = GScnBufferAlloc(size, 0x10).charptr;
        if (data == 0) {
            NuErrorProlog(nugscn_srcfile, 0xF00)(scene_alloc_msg, filename);
        }
        loading_scene = GScnBufferAlloc(sizeof(struct nugscn_s), 4).voidptr;
        if (loading_scene != 0) {
            memset(loading_scene, 0, sizeof(struct nugscn_s));
            fh = NuMemFileOpen(data, size, 0);
            if (fh != 0) {
                NuFileBeginBlkRead(fh, 0x3032554E);
                version = NuFileReadInt(fh);
                NuFileReadInt(fh);
                if (version < nugscn_version) {
                    NuErrorProlog(nugscn_srcfile, 0xF0C)(scene_old_msg, filename);
                }
                if (version > nugscn_version) {
                    NuErrorProlog(nugscn_srcfile, 0xF0F)(loader_old_msg);
                }
                while ((blk = NuFileBeginBlkRead(fh, 0)) != 0) {
                    DbgBlkType(blk);
                    switch (blk) {
                    case 0x32545354:
                    case 0x30545354:
                        ReadTextureSet(fh);
                        break;
                    case 0x3230534D:
                    case 0x3030534D:
                        ReadMaterialSet(fh);
                        break;
                    case 0x304A424F:
                        ReadObjSet(fh);
                        break;
                    case 0x54534E49:
                        loading_scene->numinstance = NuFileReadInt(fh);
                        NuFileReadInt(fh);
                        loading_scene->instances = NuMemFileAddr(fh);
                        NuMemFileSeek(fh, loading_scene->numinstance * sizeof(struct nuinstance_s), 1);
                        for (i = 0; i < loading_scene->numinstance; i++) {
                            loading_scene->instances[i].flags.visitest = 1;
                        }
                        break;
                    case 0x4C42544E:
                        count = NuFileReadInt(fh);
                        loading_scene->nametable = NuMemFileAddr(fh);
                        NuMemFileSeek(fh, count, 1);
                        for (j = 0; j < count; j++) {
                            loading_scene->nametable[j] = (loading_scene->nametable[j] >= 'A' && loading_scene->nametable[j] <= 'Z') ? loading_scene->nametable[j] + 0x20 : loading_scene->nametable[j];
                        }
                        break;
                    case 0x43455053:
                        loading_scene->numspecial = NuFileReadInt(fh);
                        NuFileReadInt(fh);
                        loading_scene->specials = NuMemFileAddr(fh);
                        src = NuMemFileAddr(fh);
                        NuMemFileSeek(fh, loading_scene->numspecial * sizeof(struct nuspecial_s), 1);
                        special = loading_scene->specials;
                        for (k = 0; k < loading_scene->numspecial; k++, src++, special++) {
                            special->mtx = loading_scene->instances[(int)src->instance].mtx;
                            special->instance = &loading_scene->instances[(int)src->instance];
                            special->name = loading_scene->nametable + (int)src->name;
                            special->flags &= ~1;
                        }
                        break;
                    case 0x30545353:
                        ReadSplineSet(fh);
                        break;
                    case 0x4C424149:
                        NuFileReadInt(fh);
                        NuFileReadInt(fh);
                        loading_scene->instanimblock = NuMemFileAddr(fh);
                        anim = loading_scene->instanimblock;
                        if (loading_scene->instanimblock != 0) {
                            for (m = 0; m < loading_scene->numinstance; m++) {
                                if (loading_scene->instances[m].anim != 0) {
                                    loading_scene->instances[m].anim = anim++;
                                    loading_scene->instances[m].anim->mtx = loading_scene->instances[m].mtx;
                                }
                            }
                        }
                        break;
                    case 0x42494C41:
                        ReadAnimationLibrary(fh);
                        break;
                    case 0x44494E49:
                        loading_scene->numinstanceixs = NuFileReadInt(fh);
                        NuFileReadInt(fh);
                        loading_scene->instanceixs = NuMemFileAddr(fh);
                        break;
                    case 0x30534154:
                        ReadTexAnimSet(fh);
                        break;
                    }
                    NuFileEndBlkRead(fh);
                }
                NuFileEndBlkRead(fh);
                NuMemFileClose(fh);
            }
            buffer->intaddr = (buffer->intaddr + 0xF) & ~0xF;
            NuRndrSplineSearch(loading_scene);
        }
    }
    return loading_scene;
}


extern int gscn_dbg;

void NuGScnUpdate(struct nugscn_s *scene, float dt)
{
    struct nuanimtime_s atime;
    struct numtx_s mtx;
    struct nuinstance_s *inst;
    struct nuinstanim_s *anim;
    struct nuanimdata_s *animdata;
    float time;
    int i;

    i = scene->numinstance;
    inst = &scene->instances[i];
    while (i != 0) {
        inst--;
        i--;
        if (gscn_dbg != 0 && gscn_dbg != i) {
            continue;
        }
        if (!(inst->flags.visible & inst->flags.visitest)) {
            continue;
        }
        anim = inst->anim;
        if (anim == 0 || scene->instanimdata == 0) {
            continue;
        }
        animdata = scene->instanimdata[anim->anim_ix];
        if (animdata == 0) {
            continue;
        }
        if (anim->playing) {
            anim->ltime += dt * anim->tfactor;
            if (anim->waiting) {
                if (anim->tfirst <= anim->ltime) {
                    anim->waiting = 0;
                    anim->ltime -= anim->tfirst - 1.0f;
                }
            }
            if (!anim->waiting) {
                if (animdata->time + anim->tinterval <= anim->ltime) {
                    if (anim->repeating) {
                        while (animdata->time + anim->tinterval <= anim->ltime) {
                            if (anim->oscillate) {
                                anim->backwards = !anim->backwards;
                            }
                            anim->ltime = anim->ltime + 1.0f - (animdata->time + anim->tinterval);
                        }
                        time = anim->ltime;
                    } else {
                        anim->playing = 0;
                        time = anim->ltime = animdata->time;
                    }
                } else {
                    time = (animdata->time < anim->ltime) ? animdata->time : anim->ltime;
                }
                if (anim->backwards) {
                    time = animdata->time + 1.0f - time;
                }
            }
        } else {
            time = anim->ltime;
        }
        NuAnimDataCalcTime(animdata, time, &atime);
        NuAnimCurveSetApplyToMatrix(animdata->chunks[atime.chunk]->animcurvesets[0], &atime, &mtx);
        anim->mtx = mtx;
        NuMtxTranslate(&anim->mtx, (struct nuvec_s *)&inst->mtx._30);
    }
}


extern int gscn_showsplines;

void NuGScnRndr3(struct nugscn_s *scene)
{
    struct nuvtx_tc1_s line[2];
    struct nuinstance_s *inst;
    struct nuspecial_s *special;
    struct nugspline_s *spline;
    char *pts;
    int i;
    int j;

    i = scene->numinstance;
    inst = &scene->instances[i];
    while (i != 0) {
        inst--;
        i--;
        if (gscn_dbg != 0 && gscn_dbg != i) {
            continue;
        }
        if (inst->flags.visible & inst->flags.visitest) {
            if (inst->anim != 0) {
                inst->flags.onscreen = NuRndrGScnObj(scene->gobjs[inst->objid], &inst->anim->mtx);
            } else {
                inst->flags.onscreen = NuRndrGScnObj(scene->gobjs[inst->objid], &inst->mtx);
            }
        }
    }
    i = scene->numexspecials;
    while (i != 0) {
        i--;
        special = &scene->exspecials[i];
        if (special->flags & 1) {
            if (special->flags & 2) {
                special->flags = (special->flags & ~4) | ((NuRndrGScnObj(scene->gobjs[special->instance->objid], &special->mtx) & 1) << 2);
            }
        }
    }
    if (gscn_showsplines != 0) {
        i = scene->numsplines;
        while (i != 0) {
            i--;
            spline = &scene->splines[i];
            pts = spline->pts;
            line[0].pnt = *(struct nuvec_s *)pts;
            line[0].diffuse = -1;
            line[1].diffuse = -1;
            for (j = 1; j < spline->len; j++) {
                pts += spline->ptsize;
                line[1].pnt = *(struct nuvec_s *)pts;
                NuRndrLine3d(line, 0, 0);
                line[0] = line[1];
            }
        }
    }
}


void NuGScnRndr2(struct nugscn_s *scene, float dt)
{
    struct nuvtx_tc1_s line[2];
    struct nuanimtime_s atime;
    struct numtx_s mtx;
    struct nuinstance_s *inst;
    struct nuspecial_s *special;
    struct nugspline_s *spline;
    struct nuinstanim_s *anim;
    struct nuanimdata_s *animdata;
    float time;
    char *pts;
    int i;
    int j;

    i = scene->numinstance;
    inst = &scene->instances[i];
    while (i != 0) {
        inst--;
        i--;
        if (gscn_dbg != 0 && gscn_dbg != i) {
            continue;
        }
        if (inst->flags.visible & inst->flags.visitest) {
            anim = inst->anim;
            if (anim != 0) {
                if (scene->instanimdata != 0) {
                    animdata = scene->instanimdata[anim->anim_ix];
                    if (animdata != 0) {
                        if (anim->playing) {
                            anim->ltime += dt * anim->tfactor;
                            if (anim->waiting) {
                                if (anim->tfirst <= anim->ltime) {
                                    anim->waiting = 0;
                                    anim->ltime -= anim->tfirst - 1.0f;
                                }
                            }
                            if (!anim->waiting) {
                                if (animdata->time + anim->tinterval <= anim->ltime) {
                                    if (anim->repeating) {
                                        while (animdata->time + anim->tinterval <= anim->ltime) {
                                            if (anim->oscillate) {
                                                anim->backwards = !anim->backwards;
                                            }
                                            anim->ltime = anim->ltime + 1.0f - (animdata->time + anim->tinterval);
                                        }
                                        time = anim->ltime;
                                    } else {
                                        anim->playing = 0;
                                        time = anim->ltime = animdata->time;
                                    }
                                } else {
                                    time = (animdata->time < anim->ltime) ? animdata->time : anim->ltime;
                                }
                                if (anim->backwards) {
                                    time = animdata->time + 1.0f - time;
                                }
                            }
                        } else {
                            time = anim->ltime;
                        }
                        NuAnimDataCalcTime(animdata, time, &atime);
                        NuAnimCurveSetApplyToMatrix(animdata->chunks[atime.chunk]->animcurvesets[0], &atime, &mtx);
                        anim->mtx = mtx;
                        NuMtxTranslate(&anim->mtx, (struct nuvec_s *)&inst->mtx._30);
                    }
                }
                NuRndrGScnObj(scene->gobjs[inst->objid], &anim->mtx);
            } else {
                inst->flags.onscreen = NuRndrGScnObj(scene->gobjs[inst->objid], &inst->mtx);
            }
        }
    }
    i = scene->numexspecials;
    while (i != 0) {
        i--;
        special = &scene->exspecials[i];
        if (special->flags & 1) {
            if (special->flags & 2) {
                special->flags = (special->flags & ~4) | ((NuRndrGScnObj(scene->gobjs[special->instance->objid], &special->mtx) & 1) << 2);
            }
        }
    }
    if (gscn_showsplines != 0) {
        i = scene->numsplines;
        while (i != 0) {
            i--;
            spline = &scene->splines[i];
            pts = spline->pts;
            line[0].pnt = *(struct nuvec_s *)pts;
            line[0].diffuse = -1;
            line[1].diffuse = -1;
            for (j = 1; j < spline->len; j++) {
                pts += spline->ptsize;
                line[1].pnt = *(struct nuvec_s *)pts;
                NuRndrLine3d(line, 0, 0);
                line[0] = line[1];
            }
        }
    }
}


void NuGScnRndr(struct nugscn_s *scene)
{
    struct nuvtx_tc1_s line[2];
    struct nuinstance_s *inst;
    struct nuspecial_s *special;
    struct nugspline_s *spline;
    char *pts;
    int i;
    int j;

    i = scene->numinstance;
    inst = &scene->instances[i];
    while (i != 0) {
        inst--;
        i--;
        if (gscn_dbg != 0 && gscn_dbg != i) {
            continue;
        }
        if (inst->flags.visible & inst->flags.visitest) {
            inst->flags.onscreen = NuRndrGScnObj(scene->gobjs[inst->objid], &inst->mtx);
        }
    }
    i = scene->numexspecials;
    while (i != 0) {
        i--;
        special = &scene->exspecials[i];
        if (special->flags & 1) {
            if (special->flags & 2) {
                special->flags = (special->flags & ~4) | ((NuRndrGScnObj(scene->gobjs[special->instance->objid], &special->mtx) & 1) << 2);
            }
        }
    }
    if (gscn_showsplines != 0) {
        i = scene->numsplines;
        while (i != 0) {
            i--;
            spline = &scene->splines[i];
            pts = spline->pts;
            line[0].pnt = *(struct nuvec_s *)pts;
            line[0].diffuse = -1;
            line[1].diffuse = -1;
            for (j = 1; j < spline->len; j++) {
                pts += spline->ptsize;
                line[1].pnt = *(struct nuvec_s *)pts;
                NuRndrLine3d(line, 0, 0);
                line[0] = line[1];
            }
        }
    }
}


void NuSpecialSetAttr(struct nuhspecial_s *sph, int attr, ...)
{
    struct nuvec_s pos;
    struct numtx_s *mtx;
    struct nuspecial_s *special;
    struct numtx_s *src;
    struct nuvec_s *vec;
    va_list ap;
    int done;

    done = 0;
    va_start(ap, attr);
    special = sph->special;
    if (special->flags & 1) {
        mtx = &special->mtx;
    } else {
        mtx = &special->instance->mtx;
    }
    while (!done) {
        switch (attr) {
        case 0:
            if (special->flags & 1) {
                special->flags = (special->flags & ~2) | ((va_arg(ap, int) & 1) << 1);
            } else {
                special->instance->flags.visible = va_arg(ap, int);
            }
            break;
        case 1:
            *mtx = special->mtx;
            break;
        case 2:
            src = va_arg(ap, struct numtx_s *);
            *mtx = *src;
            break;
        case 3:
            NuMtxMul(mtx, va_arg(ap, struct numtx_s *), &special->instance->mtx);
            break;
        case 4:
            NuMtxMul(mtx, &special->instance->mtx, va_arg(ap, struct numtx_s *));
            break;
        case 5:
            mtx->_30 = special->mtx._30;
            mtx->_31 = special->mtx._31;
            mtx->_32 = special->mtx._32;
            break;
        case 6:
            vec = va_arg(ap, struct nuvec_s *);
            mtx->_30 = vec->x;
            mtx->_31 = vec->y;
            mtx->_32 = vec->z;
            break;
        case 7:
            NuMtxTranslate(mtx, va_arg(ap, struct nuvec_s *));
            break;
        case 8:
            mtx->_30 = 0.0f;
            mtx->_31 = 0.0f;
            mtx->_32 = 0.0f;
            NuMtxPreTranslate(mtx, va_arg(ap, struct nuvec_s *));
            break;
        case 9:
            NuMtxPreTranslate(mtx, va_arg(ap, struct nuvec_s *));
            break;
        case 10:
            mtx->_00 = special->mtx._00;
            mtx->_01 = special->mtx._01;
            mtx->_02 = special->mtx._02;
            mtx->_10 = special->mtx._10;
            mtx->_11 = special->mtx._11;
            mtx->_12 = special->mtx._12;
            mtx->_20 = special->mtx._20;
            mtx->_21 = special->mtx._21;
            mtx->_22 = special->mtx._22;
            break;
        case 11:
            pos.x = mtx->_30;
            pos.y = mtx->_31;
            pos.z = mtx->_32;
            NuMtxSetRotationX(mtx, va_arg(ap, int));
            pos.x = mtx->_30;
            pos.y = mtx->_31;
            pos.z = mtx->_32;
            break;
        case 12:
            pos.x = mtx->_30;
            pos.y = mtx->_31;
            pos.z = mtx->_32;
            NuMtxSetRotationY(mtx, va_arg(ap, int));
            pos.x = mtx->_30;
            pos.y = mtx->_31;
            pos.z = mtx->_32;
            break;
        case 13:
            pos.x = mtx->_30;
            pos.y = mtx->_31;
            pos.z = mtx->_32;
            NuMtxSetRotationZ(mtx, va_arg(ap, int));
            pos.x = mtx->_30;
            pos.y = mtx->_31;
            pos.z = mtx->_32;
            break;
        case 14:
            NuMtxRotateX(mtx, va_arg(ap, int));
            break;
        case 15:
            NuMtxRotateY(mtx, va_arg(ap, int));
            break;
        case 16:
            NuMtxRotateZ(mtx, va_arg(ap, int));
            break;
        case 17:
            NuMtxPreRotateX(mtx, va_arg(ap, int));
            break;
        case 18:
            NuMtxPreRotateY(mtx, va_arg(ap, int));
            break;
        case 19:
            NuMtxPreRotateZ(mtx, va_arg(ap, int));
            break;
        case 20:
            NuMtxSetScale(mtx, va_arg(ap, struct nuvec_s *));
            break;
        case 21:
            NuMtxScale(mtx, va_arg(ap, struct nuvec_s *));
            break;
        case 22:
        default:
            done = 1;
            break;
        }
        attr = va_arg(ap, int);
    }
}
