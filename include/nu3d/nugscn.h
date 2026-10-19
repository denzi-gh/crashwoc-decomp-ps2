#ifndef NU3D_NUGSCN_H
#define NU3D_NUGSCN_H

#include "nu_types.h"
#include "numath/numathtypes.h"

struct numtl_s;
struct nugobj_s;
struct nuanimdata_s;
struct nutexanim_s;

struct nuinstanim_s {
    struct numtx_s mtx;                 /* 0x00 */
    float tfactor;                      /* 0x40 */
    float tfirst;                       /* 0x44 */
    float tinterval;                    /* 0x48 */
    float ltime;                        /* 0x4C */
    union {
        struct {
            unsigned int playing : 1;
            unsigned int backwards : 1;
            unsigned int waiting : 1;
            unsigned int repeating : 1;
            unsigned int oscillate : 1;
        };                              /* 0x50 */
        int flagword;                   /* 0x50 whole word */
    };
    int ipad[2];                        /* 0x54 */
    unsigned char anim_ix;              /* 0x5C */
    char pad5D[3];                      /* 0x5D */
}; /* 0x60 */

struct nuinstflags_s {
    int visible : 1;
    int onscreen : 1;
    int visitest : 1;
    u32 isanimated : 1;
}; /* 0x4 */

struct nuinstance_s {
    struct numtx_s mtx;                 /* 0x00 */
    int objid;                          /* 0x40 */
    union {
        struct nuinstflags_s flags;     /* 0x44 */
        int flagword;                   /* 0x44 whole word */
    };
    struct nuinstanim_s *anim;          /* 0x48 */
    short room_group;                   /* 0x4C */
    char special_flag;                  /* 0x4E */
    char pad4F;                         /* 0x4F */
}; /* 0x50 */

struct nuspecial_s {
    struct numtx_s mtx;                 /* 0x00 */
    struct nuinstance_s *instance;      /* 0x40 */
    char *name;                         /* 0x44 */
    int flags;                          /* 0x48 */
    int pad4C;                          /* 0x4C */
}; /* 0x50 */

struct nugspline_s {
    short len;                          /* 0x00 */
    short ptsize;                       /* 0x02 */
    char *name;                         /* 0x04 */
    char *pts;                          /* 0x08 */
}; /* 0x0C */

struct nugscn_s {
    int *tids;                          /* 0x00 */
    int numtid;                         /* 0x04 */
    struct numtl_s **mtls;              /* 0x08 */
    int nummtl;                         /* 0x0C */
    int numgobj;                        /* 0x10 */
    struct nugobj_s **gobjs;            /* 0x14 */
    int numinstance;                    /* 0x18 */
    struct nuinstance_s *instances;     /* 0x1C */
    int numspecial;                     /* 0x20 */
    struct nuspecial_s *specials;       /* 0x24 */
    void *splinedata;                   /* 0x28 */
    int numsplines;                     /* 0x2C */
    struct nugspline_s *splines;        /* 0x30 */
    char *nametable;                    /* 0x34 */
    int numexspecials;                  /* 0x38 */
    struct nuspecial_s *exspecials;     /* 0x3C */
    int exallocix;                      /* 0x40 */
    struct nuinstanim_s *instanimblock; /* 0x44 */
    struct nuanimdata_s **instanimdata; /* 0x48 */
    int numinstanceixs;                 /* 0x4C */
    short *instanceixs;                 /* 0x50 */
    short crossfade;                    /* 0x54 */
    short crossfaderate;                /* 0x56 */
    int crossfadegobj;                  /* 0x58 */
    int crossfadefirst;                 /* 0x5C */
    int numtexanims;                    /* 0x60 */
    struct nutexanim_s *texanims;       /* 0x64 */
    short *texanim_tids;                /* 0x68 */
}; /* 0x6C */

struct nuhspecial_s {
    struct nugscn_s *scene;             /* 0x00 */
    struct nuspecial_s *special;        /* 0x04 */
}; /* 0x08 */

struct nufaceongeom_s;
struct nusplinecfg_s;

void CalcDimsAddFaceOn(struct nufaceongeom_s *geom);
void CvtNuIFFGeomDef(int fh);
void CvtNuIFFGeomPrim(int fh, int mtlid, int write);
void MakeFaceOnDma(int fh, struct nufaceongeom_s *geom, int mtlid);
void CvtNuIFFGeomTri(int fh, int mtlid, int write);
void CvtNuIFFGeomTriStrip(int fh, int mtlid, int write);
struct nugscn_s *NuGScnDuplicate(struct nugscn_s *scene, char **buffer, char **bufend);
void ReadAnimationLibrary(int fh);
void NuGScnSetTmpWkspc(void *workspace, int size);
void NuGScnSetCfg(struct nusplinecfg_s *cfg);
void NuGScnRemove(struct nugscn_s *scene);
struct nugscn_s *NuGScnRead(char **buffer, char *bufend, char *filename);
void NuGScnTranslate(struct nugscn_s *scene, struct nuvec_s *offset);
void NuGScnAddExt(struct nugscn_s *scene, int count, struct nuspecial_s *specials);
int NuExtSpecialDuplicate(struct nuhspecial_s *dst, struct nuhspecial_s *src);
void NuExtSpecialDelete(struct nuhspecial_s *sph);
void NuGSceneSetCrossFade(struct nugscn_s *scene, short crossfade, short rate);
void NuGSceneProcessCrossFade(struct nugscn_s *scene);
void NuGScnMtlUpdate(void);
int NuSpecialFind(struct nugscn_s *scene, struct nuhspecial_s *sph, char *name);
void NuSpecialGetPos(struct nuhspecial_s *sph, struct nuvec_s *pos);
void NuSpecialGetMtx(struct nuhspecial_s *sph, struct numtx_s *mtx);
int NuSpecialDrawAt(struct nuhspecial_s *sph, struct numtx_s *mtx);
struct nugspline_s *NuSplineFind(struct nugscn_s *scene, char *name);
int NuSplineFindAllSub(struct nugscn_s *scene, char *name, struct nugspline_s **list, int max);
void NuSplineGetRandomPoint(struct nugspline_s *spline, struct nuvec_s *pt);
void NuSceneReAssignTexAnims(struct nutexanim_s *texanims, int count);
void CvtNuIFFTexAnimSet(int fh);
void CvtNuIFFTextureSet(int fh);
void CvtNuIFFSplineSet(int fh);
void CvtNuIFFFaceOnGeom(int fh);
void CvtNuIFFNodeHierarchy(int fh);
void CvtNuIFFGeom(int fh, int write);
void CvtNuIFFInstSet(int fh);
void CvtNuIFFAnimationLibrary(int fh);
void CvtNuIFFScene(int fh);
void CvtNuIFFGScene(int fh);
int NuGScnConvertScene(char *outfile, char *infile);
void ReadTextureSet(int fh);
void ReadNuIFFTexAnimSet(int fh, struct nugscn_s *gsc, short *tids);
void ReadTexAnimSet(int fh);
void DbgBlkType(int blk);
int FindSplinePtSize(char *name);

#endif /* NU3D_NUGSCN_H */
