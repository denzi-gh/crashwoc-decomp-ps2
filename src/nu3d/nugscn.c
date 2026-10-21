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


#define nugscn_srcfile "..\\nu2crash.ps2\\nu3d\\nugscn.c"

/* Spline set as read from the IFF: a 0x3C-byte record per spline, with all
 * the control points stored after them in one block. */
struct nucvtspline_s {
    int ncvs;                       /* 0x00 */
    struct nuvec_s *cvs;            /* 0x04 */
    char pad08[0x3C - 0x08];
}; /* 0x3C */

static union variptr_u buffer_start;
static char **buffer_freeptr;
static union variptr_u buffer_tmpptr;
static union variptr_u buffer_endptr;
static union variptr_u buffer_tmpstart;
static union variptr_u buffer_tmpend;
static union variptr_u readfile_base;
static int fhout;
static struct nucvtmtl_s *cvtmtls;
static struct nuvtx_tc1_s *cvtvtx;
static int cvtvtxcnt;
static short *cvtpd;
static char *dmabuff;
static union variptr_u dmabase;
static struct nuinstance_s *instances;
static int refcnt;
static struct nuspecial_s *fspecials;
static int speccnt;
static int *tristream_header;
static int sc_nnodes;
static char **sc_names;
static char *sc_strings;
static union variptr_u splout;
static union variptr_u sploutptr;
static struct nugscn_s *gscene;
static struct nugscnload_s gsc_load_context;


#define ALIGN16(p) ((char *)(((unsigned int)(p) + 0xF) & 0xFFFFFFF0))
#define RELOCATE(p) ((p) = ((p) != 0) ? (void *)((char *)&(p) + (int)(p)) : 0)


static inline union variptr_u GScnTempAlloc(int size)
{
    union variptr_u buf;

    if (buffer_tmpstart.voidptr != 0) {
        if (buffer_tmpstart.charptr + size + 4 < buffer_tmpptr.charptr) {
            buffer_tmpptr.charptr = (char *)(((nusintptr)buffer_tmpptr.charptr - size) & ~3);
            buf.charptr = buffer_tmpptr.charptr;
            return buf;
        }
    } else if (*buffer_freeptr + size + 4 < buffer_tmpptr.charptr) {
        buffer_tmpptr.charptr = (char *)(((nusintptr)buffer_tmpptr.charptr - size) & ~3);
        buf.charptr = buffer_tmpptr.charptr;
        return buf;
    }
    NuErrorProlog(nugscn_srcfile, 0x16F)("Temp buffer underrun");
    buf.voidptr = 0;
    return buf;
}

/* Carves `size` bytes, aligned to `align`, off the scene load buffer. */
static inline union variptr_u GScnBufferAlloc(int size, int align)
{
    union variptr_u ret;
    char *ptr;

    ptr = (char *)(((nusintptr)*buffer_freeptr + align - 1) & ~(align - 1));
    if (ptr + size < buffer_tmpptr.charptr) {
        *buffer_freeptr = ptr + size;
        ret.charptr = ptr;
    } else {
        NuErrorProlog(nugscn_srcfile, 0x184)("NuGScnBufferAllocAligned failed, scene buffer overrun. ");
        ret.voidptr = 0;
    }
    return ret;
}

struct nuvec_s nugscn_gobj_min = {0};
struct nuvec_s nugscn_gobj_max = {0};
int nugscn_generate_colourref = 0;
static int scene_version = 6;
struct nucvtspline_s *cvt_splines = 0;
int cvt_numsplines = 0;
struct nuvec_s *cvt_spline_cvs = 0;

void CvtNuIFFTexAnimSet(int fh)
{
    int ntexanims;
    int nscript;
    int size;
    void *texanims;
    short *script;

    NuDebugMsgProlog(nugscn_srcfile, 0x234)("Reading Animated texture set...");
    NuFileBeginBlkWrite(fhout, 0x30534154, 0);
    ntexanims = NuFileReadInt(fh);
    NuFileReadInt(fh);
    size = ntexanims * 0x20;
    NuFileWriteInt(fhout, ntexanims);
    NuFileWriteInt(fhout, 0);
    texanims = NuMemAllocFn(size, nugscn_srcfile, 0x23F);
    NuFileRead(fh, texanims, size);
    NuFileWrite(fhout, texanims, size);

    nscript = NuFileReadInt(fh);
    NuFileWriteInt(fhout, nscript);
    size = nscript * sizeof(short);
    script = NuMemAllocFn(size, nugscn_srcfile, 0x246);
    NuFileRead(fh, script, size);
    NuFileWrite(fhout, script, size);
    NuFileEndBlkWrite(fhout);

    NuMemFreeFn(texanims, nugscn_srcfile, 0x24D);
    NuMemFreeFn(script, nugscn_srcfile, 0x24E);
}


extern int NuFileBeginBlkRead(int fh, int blk);
extern void NuFileEndBlkRead(int fh);
extern void NuTexSwizzelBits(struct nutex_s *tex);

extern int info_texturecount;
extern int info_texturesize;

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
    NuDebugMsgProlog(nugscn_srcfile, 0x25D)("Reading texture set header...");
    NuFileBeginBlkRead(fh, 0x30485354);
    ntex = NuFileReadInt(fh);
    NuFileEndBlkRead(fh);
    info_texturecount += ntex;
    NuFileBeginBlkWrite(fhout, 0x30545354, 0);
    NuFileWriteInt(fhout, ntex);
    NuFileWriteInt(fhout, 0);
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
            NuErrorProlog(nugscn_srcfile, 0x29F)("Native Texture has no size");
        }
        size = (size + 0xF) & 0xFFFFFFF0;
        info_texturesize += size;
        NuFileWriteInt(fhout, size);
        pos = NuFilePos(fhout);
        pad = ((pos + 0x7F) & 0xFFFFFF80) - pos;
        NuFileWriteInt(fhout, pad - 4);
        for (j = 0; j < (pad - 4) / 4; j++) {
            NuFileWriteInt(fhout, 0);
        }
        NuFileWrite(fhout, nativebuf, size);
        NuTexDestroy(tid);
        if (tex.pal) {
            NuMemFreeFn(tex.pal, nugscn_srcfile, 0x2B6);
        }
        if (tex.bits) {
            NuMemFreeFn(tex.bits, nugscn_srcfile, 0x2B8);
        }
    }
    NuMemFreeFn(native, nugscn_srcfile, 0x2BA);
    NuFileEndBlkWrite(fhout);
}


/* The material and geometry set blocks of a scene, shared by both
 * converters. */
static inline void CvtNuIFFMaterialSet(int fh)
{
    struct numtl_s *mtl;
    int nmtls;
    int i;

    NuDebugMsgProlog(nugscn_srcfile, 0x2CD)("Reading Material set...");
    nmtls = NuFileReadInt(fh);
    cvtmtls = NuMemAllocFn(nmtls * sizeof(struct nucvtmtl_s), nugscn_srcfile, 0x2D3);
    for (i = 0; i < nmtls; i++) {
        mtl = NuMtlRead(fh);
        cvtmtls[i] = *(struct nucvtmtl_s *)mtl;
        NuMtlDestroy(mtl);
    }
    NuFileBeginBlkWrite(fhout, 0x3030534D, 0);
    NuFileWriteInt(fhout, nmtls);
    NuFileWriteInt(fhout, 0);
    NuFileWrite(fhout, cvtmtls, nmtls * sizeof(struct nucvtmtl_s));
    NuFileEndBlkWrite(fhout);
}


static inline void CvtNuIFFGobjSet(int fh)
{
    int ngobjs;
    int i;

    NuDebugMsgProlog(nugscn_srcfile, 0x2F4)("Reading geometry set header...");
    ngobjs = NuFileReadInt(fh);
    NuFileBeginBlkWrite(fhout, 0x304A424F, 0);
    NuFileWriteInt(fhout, ngobjs);
    if (nugscn_generate_colourref) {
        NuFileWriteInt(fhout, 1);
    } else {
        NuFileWriteInt(fhout, 0);
    }
    for (i = 0; i < ngobjs; i++) {
        CvtNuIFFGeomDef(fh);
    }
    NuFileEndBlkWrite(fhout);
}


void CvtNuIFFSplineSet(int fh)
{
    int nsplines;
    int ncvs;
    int i;
    struct nuvec_s *cvs;

    NuDebugMsgProlog(nugscn_srcfile, 0x31E)("Reading Spline set...");
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


/* A GSC spline set is copied through unchanged. */
static inline void CvtNuIFFGSplineSet(int fh)
{
    void *splines;
    int size;

    NuDebugMsgProlog(nugscn_srcfile, 0x347)("Converting Spline set...");
    NuFileBeginBlkWrite(fhout, 0x30545353, 0);
    NuFileWriteInt(fhout, NuFileReadInt(fh));
    size = NuFileReadInt(fh);
    NuFileWriteInt(fhout, size);
    splines = NuMemAllocFn(size, nugscn_srcfile, 0x355);
    NuFileRead(fh, splines, size);
    NuFileWrite(fhout, splines, size);
    NuFileEndBlkWrite(fhout);
    NuMemFreeFn(splines, nugscn_srcfile, 0x35B);
}


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
    NuFileWriteInt(fhout, ngobjs);
    NuFileWriteInt(fhout, 0);
    NuFileWriteInt(fhout, 0);
    NuFileWriteInt(fhout, 0);
    if (nugscn_generate_colourref) {
        colourref = NuMemAllocFn(0x100000, nugscn_srcfile, 0x3CE);
        NuCvtGeoBeginColourRef(colourref);
    }
    for (i = 0; i < ngobjs; i++) {
        type = NuFileReadInt(fh);
        NuFileWriteInt(fhout, type);
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
            NuFileWriteInt(fhout, ngeoms);
            NuFileWrite(fhout, &dims, sizeof(struct nuvec_s));
            hdrpos = NuFilePos(fhout);
            NuFileWriteInt(fhout, 0);
            NuFileWriteInt(fhout, 0);
            NuFileWriteInt(fhout, 0);
            tricount = info_tricount + info_tristripcount;
            filepos = NuFilePos(fh);
            for (j = 0; j < ngeoms; j++) {
                CvtNuIFFGeom(fh, 0);
            }
            NuFileSeek(fh, filepos, 0);
            for (j = 0; j < ngeoms; j++) {
                CvtNuIFFGeom(fh, 1);
            }
            endpos = NuFilePos(fhout);
            NuFileSeek(fhout, hdrpos, 0);
            NuFileWriteInt(fhout, info_tricount + info_tristripcount - tricount);
            NuFileSeek(fhout, endpos, 0);
            break;
        case 1:
            ngeoms = NuFileReadInt(fh);
            NuFileWriteInt(fhout, ngeoms);
            NuFileWriteInt(fhout, 0);
            NuFileWriteInt(fhout, 0);
            for (j = 0; j < ngeoms; j++) {
                CvtNuIFFFaceOnGeom(fh);
            }
            break;
        }
        NuFileWrite(fhout, &nugscn_gobj_min, sizeof(struct nuvec_s));
        NuFileWrite(fhout, &nugscn_gobj_max, sizeof(struct nuvec_s));
        NuFileWriteInt(fhout, 0);
        NuFileWriteInt(fhout, 0);
    }
    if (nugscn_generate_colourref) {
        size = (NuCvtGeoEndColourRef() - colourref + 0xF) & 0xFFFFFFF0;
        NuFileWriteInt(fhout, size);
        NuFileWriteInt(fhout, 0);
        NuFileWriteInt(fhout, 0);
        NuFileWriteInt(fhout, 0);
        if (size != 0) {
            NuFileWrite(fhout, colourref, size);
            info_colourrefsize += size;
        }
        NuMemFreeFn(colourref, nugscn_srcfile, 0x423);
    }
}


extern char NuFileReadChar(int fh);

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
            NuErrorProlog(nugscn_srcfile, 0x46B)("Skin in a scene !");
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


void ReadNuIFFGeomVtx(int fh)
{
    struct nuvtx_tc1_s *vtx;
    struct nucolour_s src;
    struct nucolour_s dst;
    int count;
    int size;
    int i;

    count = NuFileReadInt(fh);
    cvtvtxcnt = count;
    if (count < 3) {
        NuDebugMsgProlog(nugscn_srcfile, 0x4FE)("vtx cnt < 3");
    }
    if (count) {
        size = count * sizeof(struct nuvtx_tc1_s);
        cvtvtx = NuMemAllocFn(size, nugscn_srcfile, 0x501);
        if (!cvtvtx) {
            NuErrorProlog(nugscn_srcfile, 0x504)("Allocation error");
        }
        NuFileRead(fh, cvtvtx, size);
        for (i = 0; i < count; i++) {
            vtx = &cvtvtx[i];
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

void CvtNuIFFGeomPrim(int fh, int mtlid, int write)
{
    int nprims;
    int pos;
    int dmastart;
    unsigned int size;
    int i;

    nprims = NuFileReadInt(fh);
    if (nprims) {
        dmabuff = dmabase.charptr;
        pos = NuFilePos(fhout);
        dmastart = (int)dmabuff - 4;
        NuCvtGeoSetColourRefOffset(pos - dmastart);
        tristream_header = 0;
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
                NuErrorProlog(nugscn_srcfile, 0x54A)("Unknown primitive type");
                break;
            }
        }
        if (write) {
            if (nucvtgeo_lasttag) {
                nucvtgeo_lasttag->id = 0x60;
            } else {
                dmabuff = vpDmaTag_Ret(dmabuff);
            }
            size = ((unsigned int)(dmabuff - dmabase.charptr) + 0xF) & ~0xF;
            if (size >= 0x400000) {
                NuErrorProlog(nugscn_srcfile, 0x55A)("dmabuff overflow");
            }
            NuFileWriteInt(fhout, size);
            NuFileWrite(fhout, dmabase.charptr, size);
        }
    }
}


void MakeFaceOnDma(int fh, struct nufaceongeom_s *geom, int mtlid)
{
    char **dma;
    int pos;
    int dmastart;
    unsigned int size;

    dma = &dmabuff;
    dmabuff = dmabase.charptr;
    if (dmabase.charptr == 0) {
        NuErrorProlog(nugscn_srcfile, 0x57C)("Allocation error");
    }
    pos = NuFilePos(fhout);
    dmastart = (int)dmabuff - 4;
    NuCvtGeoSetColourRefOffset(pos - dmastart);
    switch (geom->faceon_type) {
    case 0:
        NuCvtDmaFaceOn(dma, geom->faceons, geom->nfaceons, &cvtmtls[mtlid]);
        break;
    case 1:
        NuCvtDmaFaceY(dma, geom->faceons, geom->nfaceons, &cvtmtls[mtlid]);
        break;
    default:
        NuErrorProlog(nugscn_srcfile, 0x585)("Unknown faceon type");
        break;
    }
    dmabuff = vpDmaTag_Ret(dmabuff);
    size = ((unsigned int)(dmabuff - dmabase.charptr) + 0xF) & ~0xF;
    if (size >= 0x400000) {
        NuErrorProlog(nugscn_srcfile, 0x58D)("dmabuff overflow");
    }
    NuFileWriteInt(fhout, size);
    NuFileWrite(fhout, dmabase.charptr, size);
}


extern void NuCvtCalcFastPoint(struct nuvec_s *min, struct nuvec_s *max, struct nucvtmtl_s *mtl);
extern int NuCvtTriStreamCFromIdx(char **dma, struct nuvtx_tc1_s *vtx, short *idx, int nidx, struct nucvtmtl_s *mtl);
extern void NuCvtDmaTriCFromIdx(char **dma, struct nuvtx_tc1_s *vtx, short *idx, int nidx, struct nucvtmtl_s *mtl);
extern int NuCvtTriStripStreamCFromIdx(char **dma, struct nuvtx_tc1_s *vtx, short *idx, int nidx, struct nucvtmtl_s *mtl);
extern int NuCvtDmaTriStripCFromIdx(char **dma, struct nuvtx_tc1_s *vtx, short *idx, int nidx, struct nucvtmtl_s *mtl);


void CvtNuIFFGeomTri(int fh, int mtlid, int write)
{
    short *idx;
    int nidx;
    int n;

    nidx = NuFileReadInt(fh);
    cvtpd = NuMemAllocFn(nidx * sizeof(short), nugscn_srcfile, 0x5AB);
    if (!cvtpd) {
        NuErrorProlog(nugscn_srcfile, 0x5AD)("Allocation error");
    }
    NuFileRead(fh, cvtpd, nidx * sizeof(short));
    if (write) {
        idx = cvtpd;
        NuCvtCalcFastPoint(&nugscn_gobj_min, &nugscn_gobj_max, &cvtmtls[mtlid]);
        if (cvtmtls[mtlid].stream == 1) {
            if (!tristream_header) {
                tristream_header = (int *)dmabuff;
                dmabuff += 0x10;
                *tristream_header = 0;
            }
            *tristream_header += NuCvtTriStreamCFromIdx(&dmabuff, cvtvtx, idx, nidx, &cvtmtls[mtlid]);
        } else {
            while (nidx > 0) {
                n = (nidx > 6000) ? 6000 : nidx;
                nidx -= 6000;
                if (n < 3 || n % 3 != 0) {
                    NuErrorProlog(nugscn_srcfile, 0x5C4)("dodgy makecnt");
                }
                if (nucvtgeo_lasttag) {
                    nucvtgeo_lasttag->id = 0x20;
                    nucvtgeo_lasttag->addr = dmabuff - dmabase.charptr;
                }
                NuCvtDmaTriCFromIdx(&dmabuff, cvtvtx, idx, n, &cvtmtls[mtlid]);
                idx += n;
                dmabuff = (char *)(((unsigned int)dmabuff + 0x7F) & ~0x7F);
            }
        }
        if ((((unsigned int)(dmabuff - dmabase.charptr) + 0xF) & ~0xF) >= 0x400000) {
            NuErrorProlog(nugscn_srcfile, 0x5D1)("dmabuff overflow");
        }
    }
    NuMemFreeFn(cvtpd, nugscn_srcfile, 0x5D3);
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
    cvtpd = NuMemAllocFn(nidx * sizeof(short), nugscn_srcfile, 0x628);
    if (!cvtpd) {
        NuErrorProlog(nugscn_srcfile, 0x62A)("Allocation error");
    }
    NuFileRead(fh, cvtpd, nidx * sizeof(short));
    if (write) {
        idx = cvtpd;
        NuCvtCalcFastPoint(&nugscn_gobj_min, &nugscn_gobj_max, &cvtmtls[mtlid]);
        if (cvtmtls[mtlid].stream == 1) {
            if (!tristream_header) {
                tristream_header = (int *)dmabuff;
                dmabuff += 0x10;
                *tristream_header = 0;
            }
            *tristream_header += NuCvtTriStripStreamCFromIdx(&dmabuff, cvtvtx, idx, nidx, &cvtmtls[mtlid]);
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
                        NuErrorProlog(nugscn_srcfile, 0x64C)("assert");
                    }
                    nucvtgeo_lasttag->id = 0x20;
                    nucvtgeo_lasttag->addr = dmabuff - dmabase.charptr;
                }
                NuCvtDmaTriStripCFromIdx(&dmabuff, cvtvtx, idx, n, &cvtmtls[mtlid]);
                idx += n;
                dmabuff = (char *)(((unsigned int)dmabuff + 0x7F) & ~0x7F);
            }
        }
        if ((((unsigned int)(dmabuff - dmabase.charptr) + 0xF) & ~0xF) >= 0x400000) {
            NuErrorProlog(nugscn_srcfile, 0x657)("dmabuff overflow");
        }
        NuCvtCalcFastPoint(&nugscn_gobj_min, &nugscn_gobj_max, 0);
    }
    NuMemFreeFn(cvtpd, nugscn_srcfile, 0x65C);
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
    gscene->instanimdata = NuMemFileAddr(fh);
    for (i = 0; i < count; i++) {
        RELOCATE(gscene->instanimdata[i]);
        anim = gscene->instanimdata[i];
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
    buffer_tmpstart.voidptr = workspace;
    buffer_tmpend.charptr = (char *)size;
}


inline int NuGScnLoad(char **buffer, char *bufend, char *filename)
{
    char *aligned;
    char *filebuf;
    int size;

    buffer_start.charptr = *buffer;
    buffer_freeptr = buffer;
    buffer_endptr.charptr = bufend;
    if (buffer_tmpstart.voidptr != 0) {
        buffer_tmpptr.charptr = buffer_tmpend.charptr;
    } else {
        buffer_tmpptr.charptr = bufend;
    }
    aligned = (char *)(((int)*buffer_freeptr + 0x7F) & ~0x7F);
    if (aligned < buffer_tmpptr.charptr) {
        filebuf = aligned;
        *buffer_freeptr = aligned;
    } else {
        NuErrorProlog(nugscn_srcfile, 0x184)("NuGScnBufferAllocAligned failed, scene buffer overrun. ");
        filebuf = 0;
    }
    readfile_base.charptr = filebuf;
    size = NuFileLoadBuffer(filename, filebuf, bufend - *buffer);
    gsc_load_context.size = size;
    gsc_load_context.filebuffer = filebuf;
    gsc_load_context.buffer = (union variptr_u *)buffer;
    gsc_load_context.bufferend = bufend;
    gsc_load_context.filename = filename;
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


void CvtNuIFFFaceOnGeom(int fh)
{
    struct nufaceongeom_s geom;
    int mtlid;
    int size;

    mtlid = NuFileReadInt(fh);
    NuFileWriteInt(fhout, mtlid);
    geom.nfaceons = NuFileReadInt(fh);
    geom.faceon_type = NuFileReadInt(fh);
    NuFileWriteInt(fhout, geom.faceon_type);
    geom.faceon_radius = NuFileReadFloat(fh);
    NuFileWriteFloat(fhout, geom.faceon_radius);
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
    NuFileBeginBlkWrite(fhout, 0x54534E49, 0);
    NuFileWriteInt(fhout, refcnt);
    NuFileWriteInt(fhout, 0);
    NuFileWrite(fhout, instances, sizeof(struct nuinstance_s) * refcnt);
    NuFileEndBlkWrite(fhout);
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
        pos = NuFilePos(fhout);
        pad = pos + 0x10;
        pad = ((pos + 0x8F) & 0xFFFFFF80) - pad;
        NuFileWriteInt(fhout, 0);
        NuFileWriteInt(fhout, pad);
    }
    mtlid = NuFileReadInt(fh);
    if (write) {
        NuFileWriteInt(fhout, mtlid);
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
            NuFileWriteInt(fhout, 0);
        }
    }
    CvtNuIFFGeomPrim(fh, mtlid, write);
    ReadNuIFFGeomSkin(fh);
    NuFileReadInt(fh);
    NuMemFreeFn(cvtvtx, nugscn_srcfile, 0x4B7);
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
    NuFileBeginBlkWrite(fhout, 0x54534E49, 0);
    NuFileWriteInt(fhout, ninstances);
    NuFileWriteInt(fhout, 0);
    NuFileWrite(fhout, instances, ninstances * 0x50);
    NuFileEndBlkWrite(fhout);

    nanims = NuFileReadInt(fh);
    if (nanims != 0) {
            anims = NuMemAllocFn(nanims * 0x60, nugscn_srcfile, 0x6FA);
        NuFileRead(fh, anims, nanims * 0x60);
        NuFileBeginBlkWrite(fhout, 0x4C424149, 0);
        NuFileWriteInt(fhout, nanims);
        NuFileWriteInt(fhout, 0);
        NuFileWrite(fhout, anims, nanims * 0x60);
        NuFileEndBlkWrite(fhout);
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
        NuFileBeginBlkWrite(fhout, 0x42494C41, 0);
        NuFileWriteInt(fhout, count);
        NuFileWriteInt(fhout, 0);
        for (i = 0; i < count; i++) {
            if (anims[i]) {
                NuFileWriteAddress(fhout, anims[i]);
                nanims++;
            } else {
                NuFileWriteInt(fhout, 0);
            }
        }
        NuFileAlign(fh, 0xF);
        NuFileWriteInt(fhout, nanims);
        for (i = 0; i < count; i++) {
            anim = anims[i];
            if (anim == 0) {
                continue;
            }
            NuFileSetAddress(fhout, anim);
            NuFileWriteFloat(fhout, anim->time);
            NuFileWriteInt(fhout, 0);
            NuFileWriteInt(fhout, anim->nchunks);
            NuFileWriteAddress(fhout, anim->chunks);
            NuFileSetAddress(fhout, anim->chunks);
            for (j = 0; j < anim->nchunks; j++) {
                NuFileWriteAddress(fhout, anim->chunks[j]);
            }
            NuFileAlign(fh, 0xF);
            for (j = 0; j < anim->nchunks; j++) {
                chunk = anim->chunks[j];
                NuFileSetAddress(fhout, chunk);
                NuFileWriteInt(fhout, chunk->numnodes);
                NuFileWriteInt(fhout, chunk->num_valid_animcurvesets);
                NuFileWriteAddress(fhout, chunk->animcurvesets);
                NuFileWriteInt(fhout, 0);
                NuFileWriteInt(fhout, 0);
                NuFileAlign(fh, 0xF);
                NuFileSetAddress(fhout, chunk->animcurvesets);
                for (k = 0; k < chunk->numnodes; k++) {
                    NuFileWriteAddress(fhout, chunk->animcurvesets[k]);
                }
                NuFileAlign(fh, 0xF);
                for (k = 0; k < chunk->numnodes; k++) {
                    curveset = chunk->animcurvesets[k];
                    NuFileSetAddress(fhout, curveset);
                    NuFileWriteInt(fhout, curveset->flags);
                    NuFileWriteAddress(fhout, curveset->constants);
                    NuFileWriteAddress(fhout, curveset->set);
                    NuFileWriteChar(fhout, curveset->ncurves);
                    NuFileWriteChar(fhout, curveset->pad[0]);
                    NuFileWriteChar(fhout, curveset->pad[1]);
                    NuFileWriteChar(fhout, curveset->pad[2]);
                    NuFileSetAddress(fhout, curveset->set);
                    for (m = 0; m < curveset->ncurves; m++) {
                        NuFileWriteAddress(fhout, curveset->set[m]);
                    }
                    NuFileAlign(fh, 0xF);
                    for (m = 0; m < curveset->ncurves; m++) {
                        curve = curveset->set[m];
                        if (curve) {
                            NuFileSetAddress(fhout, curve);
                            NuFileWriteInt(fhout, curve->mask.bits);
                            NuFileWriteAddress(fhout, curve->animkeys);
                            NuFileWriteInt(fhout, curve->numkeys);
                            NuFileWriteInt(fhout, curve->flags);
                        }
                    }
                    NuFileAlign(fh, 0xF);
                    for (m = 0; m < curveset->ncurves; m++) {
                        curve = curveset->set[m];
                        if (curve) {
                            NuFileSetAddress(fhout, curve->animkeys);
                            NuFileWrite(fhout, curve->animkeys, curve->numkeys * 16);
                        }
                    }
                    NuFileSetAddress(fhout, curveset->constants);
                    NuFileWrite(fhout, curveset->constants, curveset->ncurves * sizeof(float));
                    NuFileAlign(fh, 0xF);
                }
            }
        }
        NuFileEndBlkWrite(fhout);
        for (i = 0; i < count; i++) {
            if (anims[i]) {
                NuAnimDataDestroy(anims[i]);
            }
        }
        NuFilePatchAddress(fhout);
        NuMemFreeFn(anims, nugscn_srcfile, 0x7AD);
    }
    NuFileTidyAddress();
}


extern char *nametable_base;
extern char *nametable_free;
extern int splcnt;


void CvtNuIFFScene(int fh)
{
    int blk;
    int namesize;
    int size;
    int i;
    int pos;

    refcnt = 0;
    speccnt = 0;
    instances = NuMemAllocFn(4000 * sizeof(struct nuinstance_s), nugscn_srcfile, 0x7D7);
    fspecials = NuMemAllocFn(4000 * sizeof(struct nuspecial_s), nugscn_srcfile, 0x7D8);
    sploutptr.voidptr = splout.voidptr = NuMemAllocFn(0x80000, nugscn_srcfile, 0x7D9);
    splcnt = 0;
    while ((blk = NuFileBeginBlkRead(fh, 0)) != 0) {
        switch (blk) {
        case 0x30545354:
            CvtNuIFFTextureSet(fh);
            break;
        case 0x3030534D:
            CvtNuIFFMaterialSet(fh);
            break;
        case 0x30545347:
            CvtNuIFFGobjSet(fh);
            break;
        case 0x30545353:
            CvtNuIFFSplineSet(fh);
            break;
        case 0x3054534E:
            sc_nnodes = NuFileReadInt(fh);
            namesize = NuFileReadInt(fh);
            sc_names = NuMemAllocFn(sc_nnodes * sizeof(char *), nugscn_srcfile, 0x673);
            sc_strings = NuMemAllocFn(namesize, nugscn_srcfile, 0x674);
            NuFileRead(fh, sc_strings, namesize);
            for (i = 0; i < sc_nnodes; i++) {
                sc_names[i] = sc_strings + NuFileReadInt(fh);
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
        NuFileBeginBlkWrite(fhout, 0x4C42544E, 0);
        NuFileWriteInt(fhout, size);
        for (pos = 0; pos < size; pos++) {
            nametable_base[pos] = (nametable_base[pos] >= 'A' && nametable_base[pos] <= 'Z') ? nametable_base[pos] + 0x20 : nametable_base[pos];
        }
        NuFileWrite(fhout, nametable_base, size);
        NuFileEndBlkWrite(fhout);
    }
    if (speccnt) {
        NuFileBeginBlkWrite(fhout, 0x43455053, 0);
        NuFileWriteInt(fhout, speccnt);
        NuFileWriteInt(fhout, 0);
        NuFileWrite(fhout, fspecials, speccnt * sizeof(struct nuspecial_s));
        NuFileEndBlkWrite(fhout);
    }
    if (splcnt) {
        NuFileBeginBlkWrite(fhout, 0x30545353, 0);
        NuFileWriteInt(fhout, splcnt);
        NuFileWriteInt(fhout, sploutptr.charptr - splout.charptr);
        NuFileWrite(fhout, splout.charptr, sploutptr.charptr - splout.charptr);
        NuFileEndBlkWrite(fhout);
    }
    if (splout.charptr) {
        NuMemFreeFn(splout.charptr, nugscn_srcfile, 0x7F2);
    }
}


void CvtNuIFFGScene(int fh)
{
    short *ids;
    char *names;
    void *specials;
    int blk;
    int nids;
    int idsize;
    int namesize;
    int nspecials;
    int specsize;

    refcnt = 0;
    speccnt = 0;
    instances = NuMemAllocFn(4000 * sizeof(struct nuinstance_s), nugscn_srcfile, 0x805);
    fspecials = NuMemAllocFn(4000 * sizeof(struct nuspecial_s), nugscn_srcfile, 0x806);
    sploutptr.voidptr = splout.voidptr = NuMemAllocFn(0x80000, nugscn_srcfile, 0x807);
    splcnt = 0;
    while ((blk = NuFileBeginBlkRead(fh, 0)) != 0) {
        switch (blk) {
        case 0x30545354:
            CvtNuIFFTextureSet(fh);
            break;
        case 0x3030534D:
            CvtNuIFFMaterialSet(fh);
            break;
        case 0x30545347:
            CvtNuIFFGobjSet(fh);
            break;
        case 0x30545353:
            CvtNuIFFGSplineSet(fh);
            break;
        case 0x54534E49:
            CvtNuIFFInstSet(fh);
            break;
        case 0x44494E49:
            nids = NuFileReadInt(fh);
            idsize = nids * sizeof(short);
            ids = NuMemAllocFn(idsize, nugscn_srcfile, 0x713);
            NuFileRead(fh, ids, idsize);
            NuFileBeginBlkWrite(fhout, 0x44494E49, 0);
            NuFileWriteInt(fhout, nids);
            NuFileWriteInt(fhout, 0);
            NuFileWrite(fhout, ids, idsize);
            NuFileEndBlkWrite(fhout);
            break;
        case 0x42494C41:
            CvtNuIFFAnimationLibrary(fh);
            break;
        case 0x4C42544E:
            namesize = NuFileReadInt(fh);
            names = NuMemAllocFn(namesize, nugscn_srcfile, 0x68D);
            NuFileRead(fh, names, namesize);
            NuFileBeginBlkWrite(fhout, 0x4C42544E, 0);
            NuFileWriteInt(fhout, namesize);
            NuFileWrite(fhout, names, namesize);
            NuFileEndBlkWrite(fhout);
            NuMemFreeFn(names, nugscn_srcfile, 0x696);
            break;
        case 0x43455053:
            nspecials = NuFileReadInt(fh);
            if (nspecials) {
                specsize = nspecials * sizeof(struct nuspecial_s);
                specials = NuMemAllocFn(specsize, nugscn_srcfile, 0x7BA);
                NuFileRead(fh, specials, specsize);
                NuFileBeginBlkWrite(fhout, 0x43455053, 0);
                NuFileWriteInt(fhout, nspecials);
                NuFileWriteInt(fhout, 0);
                NuFileWrite(fhout, specials, specsize);
                NuFileEndBlkWrite(fhout);
                NuMemFreeFn(specials, nugscn_srcfile, 0x7C4);
            }
            break;
        case 0x30534154:
            CvtNuIFFTexAnimSet(fh);
            break;
        }
        NuFileEndBlkRead(fh);
    }
    if (splout.charptr) {
        NuMemFreeFn(splout.charptr, nugscn_srcfile, 0x81F);
    }
}


extern int NuFileOpen(char *name, int mode);
extern void NuFileClose(int fh);

extern int info_stripcount;
extern int info_pktcnt;

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
    cvtmtls = 0;
    nametable_base = nametable_free = NuMemAllocFn(0x4000, nugscn_srcfile, 0x83D);
    dmabuf = NuMemAllocFn(0x400004, nugscn_srcfile, 0x83E);
    dmabase.charptr = (char *)(((unsigned int)dmabuf + 0x7F) & ~0x7F);
    fhout = NuFileOpen(outfile, 1);
    if (fhout) {
        NuFileBeginBlkWrite(fhout, 0x3032554E, 1);
        NuFileWriteInt(fhout, scene_version);
        NuFileWriteInt(fhout, 0);
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
        NuFileEndBlkWrite(fhout);
        NuFileClose(fhout);
        if (cvtmtls) {
            NuMemFreeFn(cvtmtls, nugscn_srcfile, 0x85D);
        }
        if (dmabuf) {
            NuMemFreeFn(dmabuf, nugscn_srcfile, 0x860);
        }
    }
    NuDebugMsgProlog(nugscn_srcfile, 0x863)("***************************************************************");
    NuDebugMsgProlog(nugscn_srcfile, 0x864)("Total strips in scene: %d", info_stripcount);
    NuDebugMsgProlog(nugscn_srcfile, 0x865)("Total triangles in strips in scene: %d", info_tristripcount);
    NuDebugMsgProlog(nugscn_srcfile, 0x866)("Total individual triangles in scene: %d", info_tricount);
    NuDebugMsgProlog(nugscn_srcfile, 0x867)("Total textures in scene: %d", info_texturecount);
    NuDebugMsgProlog(nugscn_srcfile, 0x868)("Total texture memory used by scene: %d", info_texturesize);
    NuDebugMsgProlog(nugscn_srcfile, 0x869)("Total memory used by colourrefs: %d", info_colourrefsize);
    NuDebugMsgProlog(nugscn_srcfile, 0x86A)("***************************************************************");
    return 1;
}


extern unsigned int NuFileReadUnsignedInt(int fh);

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
        NuErrorProlog(nugscn_srcfile, 0x980)("NuNodeRead: Unknown node type.");
        break;
    }
    NuFileBeginBlkRead(fh, 0x30444E4E);
    node->id = NuFileReadUnsignedInt(fh);
    nameix = NuFileReadInt(fh);
    if (nameix == 0) {
        node->name = 0;
    } else {
        node->name = sc_names[nameix - 1];
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
                        NuDebugMsgProlog(nugscn_srcfile, 0x9AF)("its a reference node");
                    }
                    NuMtxMul(&mtx, &((struct nutransformnode_s *)p)->mtx, &mtx);
                }
            }
            memset(&instances[refcnt], 0, sizeof(struct nuinstance_s));
            instances[refcnt].mtx = mtx;
            instances[refcnt].objid = (int)((struct numeshnode_s *)node)->geometry;
            instances[refcnt].flags.visible = 1;
            if (node->name != 0) {
                fspecials[speccnt].instance = (struct nuinstance_s *)refcnt;
                fspecials[speccnt].name = (char *)(nametable_free - nametable_base);
                strcpy(nametable_free, node->name);
                nametable_free += strlen(node->name) + 1;
                speccnt++;
            }
            refcnt++;
        }
        if (node->type == NUNODETYPE_SPLINE) {
            NuMtxSetIdentity(&mtx);
            for (p = parent; p != 0; p = p->parent) {
                if (p->type == NUNODETYPE_TRANSFORM) {
                    if (((struct nutransformnode_s *)p)->flags & 2) {
                        NuDebugMsgProlog(nugscn_srcfile, 0x9DB)("its a reference node");
                    }
                    NuMtxMul(&mtx, &((struct nutransformnode_s *)p)->mtx, &mtx);
                }
            }
            splnode = (struct nusplinenode_s *)node;
            if (node->name != 0 && splnode->num_splines != 0) {
                spl = &cvt_splines[(int)splnode->splines[0]];
                *((int *)sploutptr.voidptr)++ = spl->ncvs;
                *((int *)sploutptr.voidptr)++ = nametable_free - nametable_base;
                for (i = 0; i < spl->ncvs; i++) {
                    pt.x = spl->cvs[i].x;
                    pt.y = spl->cvs[i].y;
                    pt.z = spl->cvs[i].z;
                    NuVecMtxTransform(&pt, &pt, &mtx);
                    *((struct nuvec_s *)sploutptr.voidptr)++ = pt;
                }
                strcpy(nametable_free, node->name);
                nametable_free += strlen(node->name) + 1;
                splcnt++;
            }
            refcnt++;
            if ((unsigned int)(sploutptr.charptr - splout.charptr) > 0x80000) {
                NuErrorProlog(nugscn_srcfile, 0xA06)("spline buffer overflow while converting scene");
            }
        }
        if (node->child != 0) {
            CvtNodes(node->child, node);
        }
    }
}

extern int NuFileSeek(int fh, int offset, int origin);
extern int NuMemFileSeek(int fh, int offset, int origin);

void ReadTextureSet(int fh)
{
    int ntex;
    int size;
    int skip;
    void *data;
    int i;

    NuDebugMsgProlog(nugscn_srcfile, 0xA90)("Reading type 2 texture set header...");
    ntex = NuFileReadInt(fh);
    NuFileReadInt(fh);
    gscene->tids = GScnBufferAlloc(ntex * sizeof(int), 4).voidptr;
    memset(gscene->tids, 0, ntex * sizeof(int));
    gscene->numtid = ntex;
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
        gscene->tids[i] = NuTexCreateNative(data);
    }
}

extern void NuDisableVBlank(void);
extern void NuEnableVBlank(void);
extern NuMtlReadEventFunc NuMtlReadEvent;
int nugscn_mtlupdate_enabled = 1;
int in_protected_bit = 0;

void ReadMaterialSet(int fh)
{
    struct numtl_s *mtl;
    struct numtl_s *next;
    unsigned int ix;
    int nmtls;
    int i;

    NuDebugMsgProlog(nugscn_srcfile, 0xADC)("Reading Type 2 Material set...");
    nmtls = NuFileReadInt(fh);
    NuFileReadInt(fh);
    gscene->mtls = GScnBufferAlloc(nmtls * sizeof(struct numtl_s *), 4).voidptr;
    gscene->nummtl = nmtls;
    for (i = 0; i < nmtls; i++) {
        gscene->mtls[i] = NuMemFileAddr(fh);
        NuMemFileSeek(fh, sizeof(struct numtl_s), 1);
        if (gscene->mtls[i]->tid == -1) {
            gscene->mtls[i]->tid = 0;
        } else {
            gscene->mtls[i]->tid = gscene->tids[gscene->mtls[i]->tid];
        }
        if ((int)gscene->mtls[i]->next == -1) {
            gscene->mtls[i]->next = 0;
        }
        next = gscene->mtls[i]->next;
        if (nugscn_mtlupdate_enabled) {
            NuDisableVBlank();
            in_protected_bit = 1;
            NuMtlAdd(gscene->mtls[i]);
            gscene->mtls[i]->next = next;
            if (!NuMtlReadEvent(gscene->mtls[i])) {
                NuMtlUpdate(gscene->mtls[i]);
            }
            in_protected_bit = 0;
            NuEnableVBlank();
        }
    }
    for (i = 0; i < nmtls; i++) {
        mtl = gscene->mtls[i];
        ix = (unsigned int)mtl->next;
        if (ix == 0xFFFFFFFF) {
            mtl->next = 0;
        } else if (ix >= 1 && ix <= 0xFFFF) {
            mtl->next = gscene->mtls[ix - 1];
        }
    }
    NuDebugMsgProlog(nugscn_srcfile, 0xB1A)("Added %d scene materials", nmtls);
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

#define DIST2(dx, dy, dz) ((dx) * (dx) + (dy) * (dy) + (dz) * (dz))
#define KEEP_MAX(dst, val) ((dst) = ((val) < (dst)) ? (dst) : (val))

void ReadObjSet(int fh)
{
    struct nuvec_s origin;
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
    struct nugobj_s *gobj;
    struct nugeom_s *geom;
    struct nufaceongeom_s *fgeom;

    memset(&origin, 0, sizeof(struct nuvec_s));
    NuDebugMsgProlog(nugscn_srcfile, 0xB5C)("Reading Objset set...");
    ngobjs = NuFileReadInt(fh);
    colourref = NuFileReadInt(fh);
    gscene->gobjs = GScnBufferAlloc(ngobjs * sizeof(struct nugobj_s *), 4).voidptr;
    memset(gscene->gobjs, 0, ngobjs * sizeof(struct nugobj_s *));
    gscene->numgobj = ngobjs;
    for (i = 0; i < ngobjs; i++) {
        nsub = NuFileReadInt(fh);
        NuFileReadInt(fh);
        NuFileReadInt(fh);
        NuFileReadInt(fh);
        gobj = GScnBufferAlloc(nsub * sizeof(struct nugobj_s), 4).voidptr;
        gscene->gobjs[i] = gobj;
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
                    geom->mtl = gscene->mtls[NuFileReadInt(fh)];
                    NuMemFileSeek(fh, skip, 1);
                    size = NuFileReadInt(fh);
                    if (size & 0xF) {
                        NuErrorProlog(nugscn_srcfile, 0xB95)("invalid qwc");
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
                            NuDebugMsgProlog(nugscn_srcfile, 0xBA0)("Geom DMASTREAM not 128 byte aligned");
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
                    fgeom->mtl = gscene->mtls[NuFileReadInt(fh)];
                    fgeom->faceon_type = NuFileReadInt(fh);
                    fgeom->faceon_radius = NuFileReadFloat(fh);
                    size = NuFileReadInt(fh);
                    fgeom->dmastream = NuMemFileAddr(fh);
                    NuMemFileSeek(fh, size, 1);
                }
                break;
            default:
                NuErrorProlog(nugscn_srcfile, 0xBD2)("Unknown geom type");
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
        gscene->gobjs[i]->blend = 0;
        gscene->gobjs[i]->faceon_blend = 0;
        if (colourref == 1) {
            gscene->gobjs[i]->tricount = NuFileReadInt(fh);
            NuFileReadInt(fh);
            NuFileReadInt(fh);
            NuFileReadInt(fh);
            if (gscene->gobjs[i]->tricount != 0) {
                gscene->gobjs[i]->blend = NuMemFileAddr(fh);
                NuMemFileSeek(fh, gscene->gobjs[i]->tricount, 1);
                NuCvtGeoFixup(gscene->gobjs[i]->blend, (int)readfile_base.charptr);
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

    NuDebugMsgProlog(nugscn_srcfile, 0xC64)("Reading Animated texture set...");
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


void ReadTexAnimSet(int fh)
{
    static int i;
    struct nutexanim_s *tex;
    short *tidtab;
    char *start;
    union variptr_u buff;
    char *scriptname;
    void *prog;
    int j;

    NuDebugMsgProlog(nugscn_srcfile, 0xC2C)("Reading Animated texture set...");
    gscene->numtexanims = NuFileReadInt(fh);
    NuFileReadInt(fh);
    gscene->texanims = NuMemFileAddr(fh);
    NuMemFileSeek(fh, gscene->numtexanims * sizeof(struct nutexanim_s), 1);
    NuFileReadInt(fh);
    tidtab = NuMemFileAddr(fh);

    for (i = 0; i < gscene->numtexanims; i++) {
        tex = &gscene->texanims[i];
        tex->succ = 0;
        tex->prev = 0;
        tex->tids = tidtab + (int)tex->tids;
        for (j = 0; j < tex->numtids; j++) {
            tex->tids[j] = gscene->tids[tex->tids[j]];
        }
        tex->ntaname = gscene->nametable + (int)tex->ntaname;
        scriptname = gscene->nametable + (int)tex->scriptname;
        tex->mtl = gscene->mtls[(int)tex->mtl];
        tex->dynalloc = 0;
        prog = NuTexAnimProgFind(scriptname);
        start = buff.charptr = (char *)(((int)*buffer_freeptr + 0xF) & ~0xF);
        tex->env = NuTexAnimEnvCreate(&buff, tex->mtl, tex->tids, prog);
        GScnBufferAlloc(buff.charptr - start, 0x10);
    }

    for (i = 0; i < gscene->numtexanims - 1; i++) {
        gscene->texanims[i].succ = &gscene->texanims[i] + 1;
        gscene->texanims[i + 1].prev = &gscene->texanims[i];
    }
    NuTexAnimAddList(gscene->texanims);
}


static struct nusplinecfg_s *nugscn_cfg = 0;


inline int FindSplinePtSize(char *name)
{
    struct nusplinecfg_s *cfg;

    cfg = nugscn_cfg;
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


inline void NuGScnRemove(struct nugscn_s *scene)
{
    int i;

    if (scene->texanims != 0) {
        NuTexAnimRemoveList(scene->texanims);
    }
    for (i = 0; i < scene->nummtl; i++) {
        NuMtlDestroy(scene->mtls[i]);
    }
    NuDebugMsgProlog(nugscn_srcfile, 0xE37)("Removed %d scene materials", scene->nummtl);
    for (i = 0; i < scene->numtid; i++) {
        NuTexDestroy(scene->tids[i]);
    }
}


void ReadSplineSet(int fh)
{
    char *data;
    char *pts;
    int size;
    int i;
    int j;

    gscene->numsplines = NuFileReadInt(fh);
    size = NuFileReadInt(fh);
    gscene->splines = GScnBufferAlloc(gscene->numsplines * sizeof(struct nugspline_s), 4).voidptr;
    data = GScnTempAlloc(size).charptr;
    NuFileRead(fh, data, size);
    for (i = 0; i < gscene->numsplines; i++) {
        gscene->splines[i].len = *(unsigned short *)data;
        data += sizeof(int);
        gscene->splines[i].name = (char *)(*(int *)data + (int)gscene->nametable);
        data += sizeof(int);
        gscene->splines[i].ptsize = FindSplinePtSize(gscene->splines[i].name);
        gscene->splines[i].pts = GScnBufferAlloc(gscene->splines[i].ptsize * gscene->splines[i].len, 4).voidptr;
        pts = gscene->splines[i].pts;
        for (j = 0; j < gscene->splines[i].len; j++) {
            *(struct nuvec_s *)pts = *(struct nuvec_s *)data;
            data += sizeof(struct nuvec_s);
            pts += gscene->splines[i].ptsize;
        }
    }
    if (buffer_tmpstart.voidptr != 0) {
        buffer_tmpptr.charptr = buffer_tmpend.charptr;
    } else {
        buffer_tmpptr.charptr = buffer_endptr.charptr;
    }
}


void NuGScnSetCfg(struct nusplinecfg_s *cfg)
{
    nugscn_cfg = cfg;
}


extern unsigned int timeUserRead(void);


void DbgBlkType(int blk)
{
    char *name;
    unsigned int tod;

    switch (blk) {
    case 0x32545354:
        name = "TEXSET2";
        break;
    case 0x30545354:
        name = "TEXSET";
        break;
    case 0x3230534D:
        name = "MTLSET2";
        break;
    case 0x3030534D:
        name = "MTLSET";
        break;
    case 0x304A424F:
        name = "OBJSET";
        break;
    case 0x54534E49:
        name = "INSTSET";
        break;
    case 0x4C42544E:
        name = "NAMETABLE";
        break;
    case 0x43455053:
        name = "SPECIALSET";
        break;
    case 0x30545353:
        name = "SPLINESET";
        break;
    case 0x4C424149:
        name = "INSTANIMBLOCK";
        break;
    case 0x42494C41:
        name = "ANIMLIB";
        break;
    case 0x44494E49:
        name = "INSTIDS";
        break;
    case 0x30534154:
        name = "TEXANIMSET";
        break;
    default:
        name = "unknown";
        break;
    }
    tod = timeUserRead();
    NuDebugMsgProlog(nugscn_srcfile, 0xEE0)("Tod %d Block Type = %s", tod, name);
}


extern void timeUserReset(void);
extern int NuMemFileOpen(void *buf, int size, int mode);
extern void NuMemFileClose(int fh);
extern void NuRndrSplineSearch(void *scene);

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

    gscene = 0;
    timeUserReset();
    size = gsc_load_context.size;
    buffer = gsc_load_context.buffer;
    filename = gsc_load_context.filename;
    if (size > 0) {
        data = GScnBufferAlloc(size, 0x10).charptr;
        if (data == 0) {
            NuErrorProlog(nugscn_srcfile, 0xF00)("GSC File too large, \"%s\" ", filename);
        }
        gscene = GScnBufferAlloc(sizeof(struct nugscn_s), 4).voidptr;
        if (gscene != 0) {
            memset(gscene, 0, sizeof(struct nugscn_s));
            fh = NuMemFileOpen(data, size, 0);
            if (fh != 0) {
                NuFileBeginBlkRead(fh, 0x3032554E);
                version = NuFileReadInt(fh);
                NuFileReadInt(fh);
                if (version < scene_version) {
                    NuErrorProlog(nugscn_srcfile, 0xF0C)("GSC File out of date - need newer \"%s\" file", filename);
                }
                if (version > scene_version) {
                    NuErrorProlog(nugscn_srcfile, 0xF0F)("Loader out of date - need newer 3D");
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
                        gscene->numinstance = NuFileReadInt(fh);
                        NuFileReadInt(fh);
                        gscene->instances = NuMemFileAddr(fh);
                        NuMemFileSeek(fh, gscene->numinstance * sizeof(struct nuinstance_s), 1);
                        for (i = 0; i < gscene->numinstance; i++) {
                            gscene->instances[i].flags.visitest = 1;
                        }
                        break;
                    case 0x4C42544E:
                        count = NuFileReadInt(fh);
                        gscene->nametable = NuMemFileAddr(fh);
                        NuMemFileSeek(fh, count, 1);
                        for (j = 0; j < count; j++) {
                            gscene->nametable[j] = (gscene->nametable[j] >= 'A' && gscene->nametable[j] <= 'Z') ? gscene->nametable[j] + 0x20 : gscene->nametable[j];
                        }
                        break;
                    case 0x43455053:
                        gscene->numspecial = NuFileReadInt(fh);
                        NuFileReadInt(fh);
                        gscene->specials = NuMemFileAddr(fh);
                        src = NuMemFileAddr(fh);
                        NuMemFileSeek(fh, gscene->numspecial * sizeof(struct nuspecial_s), 1);
                        special = gscene->specials;
                        for (k = 0; k < gscene->numspecial; k++, src++, special++) {
                            special->mtx = gscene->instances[(int)src->instance].mtx;
                            special->instance = &gscene->instances[(int)src->instance];
                            special->name = gscene->nametable + (int)src->name;
                            special->flags &= ~1;
                        }
                        break;
                    case 0x30545353:
                        ReadSplineSet(fh);
                        break;
                    case 0x4C424149:
                        NuFileReadInt(fh);
                        NuFileReadInt(fh);
                        gscene->instanimblock = NuMemFileAddr(fh);
                        anim = gscene->instanimblock;
                        if (gscene->instanimblock != 0) {
                            for (m = 0; m < gscene->numinstance; m++) {
                                if (gscene->instances[m].anim != 0) {
                                    gscene->instances[m].anim = anim++;
                                    gscene->instances[m].anim->mtx = gscene->instances[m].mtx;
                                }
                            }
                        }
                        break;
                    case 0x42494C41:
                        ReadAnimationLibrary(fh);
                        break;
                    case 0x44494E49:
                        gscene->numinstanceixs = NuFileReadInt(fh);
                        NuFileReadInt(fh);
                        gscene->instanceixs = NuMemFileAddr(fh);
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
            NuRndrSplineSearch(gscene);
        }
    }
    return gscene;
}


int gscn_dbg = 0;
int gscn_showsplines = 0;
int info_texturecount = 0;
int info_texturesize = 0;
int info_colourrefsize = 0;
char *nametable_base = 0;
char *nametable_free = 0;
int splcnt = 0;
int verts_this_frame = 0;

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
