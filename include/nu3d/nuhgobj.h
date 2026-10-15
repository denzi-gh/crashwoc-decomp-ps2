#ifndef NU3D_NUHGOBJ_H
#define NU3D_NUHGOBJ_H

#include "nu_types.h"
#include "numath/numathtypes.h"

struct numtl_s;
struct nugobj_s;
struct nutexanim_s;
union variptr_u;

struct NUPOINTOFINTEREST_s {
    struct numtx_s offset;              /* 0x00 */
    char *name;                         /* 0x40 */
    u8 parent_joint;                    /* 0x44 */
    char pad45[0x0B];                   /* 0x45 */
}; /* 0x50 */

struct NUJOINTDATA_s {
    struct numtx_s orient;              /* 0x00 */
    struct nuvec_s locator_offset;      /* 0x40 */
    char *name;                         /* 0x4C */
    char parent_ix;                     /* 0x50 */
    u8 flags;                           /* 0x51 */
    char pad52[0x0E];                   /* 0x52 */
}; /* 0x60 */

struct NUHLAYERDATA_s {
    char *name;                         /* 0x00 */
    struct nugobj_s **gobjs;            /* 0x04 */
    struct nugobj_s *skin_object;       /* 0x08 */
    struct nugobj_s **blend_gobjs;      /* 0x0C */
    struct nugobj_s *blend_skin_object; /* 0x10 */
}; /* 0x14 */

struct nushadowvtx_s {
    struct nuvec_s pos;                 /* 0x00 */
    int count;                          /* 0x0C */
}; /* 0x10 */

struct nushadowmesh_s {
    struct nushadowvtx_s *first;        /* 0x00 */
    struct nushadowvtx_s *second;       /* 0x04 */
}; /* 0x08 */

struct nushadowdata_s {
    void *ellipsoids;                   /* 0x00 */
    void *cylinders;                    /* 0x04 */
    struct nushadowmesh_s *meshes;      /* 0x08 */
    u8 numellipsoids;                   /* 0x0C */
    u8 numcylinders;                    /* 0x0D */
    u8 nummeshes;                       /* 0x0E */
    u8 last;                            /* 0x0F */
}; /* 0x10 */

struct nuhcollideentry_s {
    int type;                           /* 0x00 */
    void *data;                         /* 0x04 */
}; /* 0x08 */

struct nuhcollide_s {
    unsigned int nentries;              /* 0x00 */
    struct nuhcollideentry_s *entries;  /* 0x04 */
}; /* 0x08 */

struct NUHGOBJ_s {
    short *tids;                        /* 0x00 */
    int numtid;                         /* 0x04 */
    struct numtl_s **mtls;              /* 0x08 */
    int nummtl;                         /* 0x0C */
    int type;                           /* 0x10 */
    struct NUJOINTDATA_s *joints;       /* 0x14 */
    struct numtx_s *T;                  /* 0x18 */
    struct numtx_s *INV_WT;             /* 0x1C */
    u8 *joint_ixs;                      /* 0x20 */
    struct NUHLAYERDATA_s *layers;      /* 0x24 */
    struct NUPOINTOFINTEREST_s *pois;   /* 0x28 */
    u8 *poi_ixs;                        /* 0x2C */
    char *string_table;                 /* 0x30 */
    int string_table_size;              /* 0x34 */
    f32 scale;                          /* 0x38 */
    struct nushadowdata_s *shadow;      /* 0x3C */
    f32 sphere_radius;                  /* 0x40 */
    f32 sphere_y;                       /* 0x44 */
    struct nuvec_s bbox_min;            /* 0x48 */
    struct nuvec_s bbox_max;            /* 0x54 */
    f32 cyl_y;                          /* 0x60 */
    f32 cyl_height;                     /* 0x64 */
    f32 cyl_radius;                     /* 0x68 */
    struct nuhcollide_s *collide;       /* 0x6C */
    int numtexanims;                    /* 0x70 */
    struct nutexanim_s *texanims;       /* 0x74 */
    short *texanim_tids;                /* 0x78 */
    u8 num_joints;                      /* 0x7C */
    u8 num_joint_ixs;                   /* 0x7D */
    u8 num_layers;                      /* 0x7E */
    u8 num_pois;                        /* 0x7F */
    u8 num_poi_ixs;                     /* 0x80 */
    char pad81[0x03];                   /* 0x81 */
}; /* 0x84 */

int NuHGobjBoundingBoxToLine(struct NUHGOBJ_s *hobj, struct numtx_s *mtx, struct nuvec_s *p0, struct nuvec_s *p1, f32 radius);
int NuHGobjCollideSphere(struct NUHGOBJ_s *hobj, struct numtx_s *wm, struct numtx_s *tmtx, struct nuvec_s *pos, f32 radius, struct nuvec_s *hit);
void NuHGobjDestroyDynamic(struct NUHGOBJ_s *hobj);
void NuHGobjDestroy(struct NUHGOBJ_s *hobj);
void NuHGobjPOIMtx(struct NUHGOBJ_s *hobj, u8 i, struct numtx_s *mC, struct numtx_s *tmtx, struct numtx_s *out);
int NuHGobjPOIMtxWithCheck(struct NUHGOBJ_s *hobj, u8 i, struct numtx_s *mC, struct numtx_s *tmtx, struct numtx_s *out);
struct NUPOINTOFINTEREST_s *NuHGobjGetPOI(struct NUHGOBJ_s *hobj, u8 i);
void NuHGobjJointMtx(struct NUHGOBJ_s *hobj, u8 i, struct numtx_s *mC, struct numtx_s *tmtx, struct numtx_s *out);
int NuHGobjJointMtxWithCheck(struct NUHGOBJ_s *hobj, u8 i, struct numtx_s *mC, struct numtx_s *tmtx, struct numtx_s *out);
int NuHGobjBoundingSphereToSphere(struct NUHGOBJ_s *hobj, struct numtx_s *wm, struct nuvec_s *pos, f32 radius);
int NuHGobjBoundingBoxToSphere(struct NUHGOBJ_s *hobj, struct numtx_s *mtx, struct nuvec_s *pos, f32 radius);
int NuHGobjBoundingCylinderToSphere(struct NUHGOBJ_s *hobj, struct numtx_s *mtx, struct nuvec_s *pos, f32 radius);
int NuHGobjBoundingSphereToLine(struct NUHGOBJ_s *hobj, struct numtx_s *mtx, struct nuvec_s *p0, struct nuvec_s *p1, f32 radius);
struct NUHGOBJ_s *NuHGobjLoad(char *file);
void ReadNuIFFHGobjSet(int fh, struct NUHGOBJ_s *hobj);
struct NUHGOBJ_s *NuHGobjReadForCutScene(int fh, struct NUHGOBJ_s *shared);
void ReadNuIFFCollideData(int fh, struct NUHGOBJ_s *hobj);
void ReadNuIFFHGobj(int fh, struct NUHGOBJ_s *hobj);
char *NuGHGWriteGobj(char *base, char *ptr, struct NUHGOBJ_s *hobj, struct nugobj_s *gobj, int *offset);
struct NUHGOBJ_s *NuGHGRead(union variptr_u *buffer, char *file);

#endif /* NU3D_NUHGOBJ_H */
