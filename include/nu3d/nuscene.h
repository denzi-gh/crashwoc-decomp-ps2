#ifndef NU3D_NUSCENE_H
#define NU3D_NUSCENE_H

#include "numath/numathtypes.h"

struct numtl_s;
struct nugobj_s;
struct nunode_s;
struct nugscn_s;

struct nuspline_s {
    int ncvs;                           /* 0x00 */
    struct nuvec_s *cvs;                /* 0x04 */
    char pad08[0x34];                   /* 0x08 */
}; /* 0x3C */

struct nuscene_s {
    int numnodes;                       /* 0x00 */
    char **names;                       /* 0x04 */
    struct nunode_s **nodes;            /* 0x08 */
    char *nametable;                    /* 0x0C */
    int numtids;                        /* 0x10 */
    short *tids;                        /* 0x14 */
    int nummtls;                        /* 0x18 */
    struct numtl_s **mtls;              /* 0x1C */
    int numgobjs;                       /* 0x20 */
    struct nugobj_s **gobjs;            /* 0x24 */
    int numsplines;                     /* 0x28 */
    struct nuspline_s *splines;         /* 0x2C */
    struct nuvec_s *splinedata;         /* 0x30 */
    struct nunode_s *root;              /* 0x34 */
    struct nugscn_s *gscene;            /* 0x38 */
}; /* 0x3C */

struct nufaceongeom_s;
struct nugeom_s;

void ReadNuIFFTextureSet(int fh, struct nuscene_s *scene);
void ReadNuIFFGSplineSet(int fh, struct nugscn_s *gscene);
struct nugobj_s *ReadNuIFFGeom(int fh, struct numtl_s **mtls);
void ReadNuIFFBlendShape(int fh, struct nugeom_s *geom);
void ReadNuIFFScene(int fh, struct nuscene_s *scene);
void ReadNuIFFSpecialObjects(int fh, struct nugscn_s *gscene);
void ReadNuIFFGScene(int fh, struct nugscn_s *gscene);
struct nuscene_s *NuSceneLoad(char *filename);
void NuGSceneDestroy(struct nugscn_s *gscene);
void NuSceneDestroy(struct nuscene_s *scene);
struct nunode_s *NuSceneFindNodeByName(struct nuscene_s *scene, char *name);
void NuSceneMtlUpdate(struct nuscene_s *scene);
char *ReadNuIFFNameTable(int fh, int *size);
void ReadNuIFFGeomDef(int fh, struct nuscene_s *scene);
void ReadNuIFFGeomCntrl(int fh, struct nugeom_s *geom);
void ReadNuIFFGeomPrim(int fh, struct nugeom_s *geom);
void ReadNuIFFGeomTri(int fh, struct nugeom_s *geom, int type);
void ReadNuIFFGeomBezTri(int fh, struct nugeom_s *geom);
void ReadNuIFFFaceOnGeom(int fh, struct nufaceongeom_s *fgeom);

#endif /* NU3D_NUSCENE_H */
