#ifndef NU3D_NUHGOBJ_H
#define NU3D_NUHGOBJ_H

#include "nu_types.h"
#include "numath/numathtypes.h"

struct numtl_s;
struct nugobj_s;
struct nutexanim_s;
union variptr_u;

/* Serialized GHG records. Offsets are measured from the file base. */
union NUGOBJEX_s {
    struct nuvec_s origin;              /* 0x00 */
    struct {
        float dist;
        float dist2;
        unsigned short centre_at_origin : 1;
        char pad[2];
    } lod;                              /* 0x00 */
}; /* 0x0C */
typedef union NUGOBJEX_s NUGOBJEX;

struct nufgobj_s {
    struct nugobj_s *sysnext;           /* 0x00 */
    struct nugobj_s *syslast;           /* 0x04 */
    int type;                           /* 0x08 */
    int geom;                           /* 0x0C */
    int faceon_geom;                    /* 0x10 */
    f32 r;                              /* 0x14 */
    f32 r2;                             /* 0x18 */
    struct nuvec_s min;                 /* 0x1C */
    struct nuvec_s max;                 /* 0x28 */
    struct nuvec_s cntr;                /* 0x34 */
    f32 cntr_r;                         /* 0x40 */
    f32 cntr_r2;                        /* 0x44 */
    int flags;                          /* 0x48: PS2 nugobj_s.ngobjs word */
    int next_gobj;                      /* 0x4C */
    union NUGOBJEX_s exdata;            /* 0x50 */
}; /* 0x5C */
typedef struct nufgobj_s NUFGOBJ;

struct nufgeomps2_s {
    int next;                           /* 0x00 */
    int mtl_id;                         /* 0x04 */
    int dmastream;                      /* 0x08 */
    int skindma;                        /* 0x0C */
}; /* 0x10 */
typedef struct nufgeomps2_s NUFGEOMPS2;

struct nuflayerdata_s {
    int name;                           /* 0x00 */
    int gobjs;                          /* 0x04 */
    int skin_gobj;                      /* 0x08 */
    int blend_gobjs;                    /* 0x0C */
    int blend_skin_gobj;                /* 0x10 */
}; /* 0x14 */
typedef struct nuflayerdata_s NUFLAYERDATA;

struct nuhghhdr_s {
    int filesize;                       /* 0x00 */
    int version;                        /* 0x04 */
    int numtxtrs;                       /* 0x08 */
    int txtrs;                          /* 0x0C */
    int nummtls;                        /* 0x10 */
    int mtls;                           /* 0x14 */
    int num_joints;                     /* 0x18 */
    int joints;                         /* 0x1C */
    int t;                              /* 0x20 */
    int inv_wt;                         /* 0x24 */
    int num_joint_ixs;                  /* 0x28 */
    int joint_ixs;                      /* 0x2C */
    int string_table;                   /* 0x30 */
    int string_table_size;              /* 0x34 */
    int num_poi_ixs;                    /* 0x38 */
    int poi_ixs;                        /* 0x3C */
    int num_points_of_interest;         /* 0x40 */
    int points_of_interest;             /* 0x44 */
    int num_layers;                     /* 0x48 */
    int layers;                         /* 0x4C */
    int shadow_data;                    /* 0x50 */
    f32 sphere_radius;                  /* 0x54 */
    f32 sphere_yoff;                    /* 0x58 */
    struct nuvec_s min;                 /* 0x5C */
    struct nuvec_s max;                 /* 0x68 */
    f32 cylinder_yoff;                  /* 0x74 */
    f32 cylinder_height;                /* 0x78 */
    f32 cylinder_radius;                /* 0x7C */
    int collision_data;                 /* 0x80 */
    f32 tbrdist;                        /* 0x84 */
    int numtexanims;                    /* 0x88 */
    int texanims;                       /* 0x8C */
}; /* 0x90 */
typedef struct nuhghhdr_s NUGHGHDR;

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

struct NULAYERDATA_s {
    char *name;                         /* 0x00 */
    struct nugobj_s **gobjs;            /* 0x04 */
    struct nugobj_s *skin_gobj;         /* 0x08 */
    struct nugobj_s **blend_gobjs;      /* 0x0C */
    struct nugobj_s *blend_skin_gobj;   /* 0x10 */
}; /* 0x14 */
typedef struct NULAYERDATA_s NULAYERDATA;

struct nushadowvtx_s {
    struct nuvec_s pos;                 /* 0x00 */
    int count;                          /* 0x0C */
}; /* 0x10 */

/* The renderer also reads integer counts from the fourth vector word. */
struct NUSHADOWMESH_s {
    union {
        struct nuvec4_s *normals;
        struct nushadowvtx_s *normal_counts;
    };                                  /* 0x00 */
    union {
        struct nuvec4_s *verts;
        struct nushadowvtx_s *vert_counts;
    };                                  /* 0x04 */
}; /* 0x08 */
typedef struct NUSHADOWMESH_s NUSHADOWMESH;

struct NUELLIPSOID_s {
    struct nuvec_s centre;              /* 0x00 */
    struct nuvec_s y_axis;              /* 0x0C */
    struct nuvec_s x_axis;              /* 0x18 */
    struct nuvec_s z_axis;              /* 0x24 */
}; /* 0x30 */
typedef struct NUELLIPSOID_s NUELLIPSOID;

struct NUCYLINDER_s {
    union {
        struct nuvec_s centre;
        int flags;
    };                                  /* 0x00 */
    struct nuvec_s y_axis;              /* 0x0C */
    struct nuvec4_s x_axis;             /* 0x18 */
    struct nuvec4_s z_axis;             /* 0x28 */
    char pad[8];                        /* 0x38 */
}; /* 0x40 */
typedef struct NUCYLINDER_s NUCYLINDER;

struct NUSHADOWDATA_s {
    struct NUELLIPSOID_s *ellipsoids;   /* 0x00 */
    struct NUCYLINDER_s *cylinders;     /* 0x04 */
    struct NUSHADOWMESH_s *shadow_meshes; /* 0x08 */
    u8 nellipsoids;                     /* 0x0C */
    u8 ncylinders;                      /* 0x0D */
    u8 nshadow_meshes;                  /* 0x0E */
    u8 joint;                           /* 0x0F */
}; /* 0x10 */
typedef struct NUSHADOWDATA_s NUSHADOWDATA;

struct NUCOLLISIONSPHERE_s {
    struct nuvec_s centre;              /* 0x00 */
    f32 radius;                         /* 0x0C */
    u8 joint;                           /* 0x10 */
}; /* 0x14 */
typedef struct NUCOLLISIONSPHERE_s NUCOLLISIONSPHERE;

struct NUCOLLISIONHDR_s {
    int type;                           /* 0x00 */
    void *data;                         /* 0x04 */
}; /* 0x08 */
typedef struct NUCOLLISIONHDR_s NUCOLLISIONHDR;

struct nuhcollide_s {
    unsigned int nentries;              /* 0x00 */
    struct NUCOLLISIONHDR_s *entries;   /* 0x04 */
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
    struct NULAYERDATA_s *layers;       /* 0x24 */
    struct NUPOINTOFINTEREST_s *pois;   /* 0x28 */
    u8 *poi_ixs;                        /* 0x2C */
    char *string_table;                 /* 0x30 */
    int string_table_size;              /* 0x34 */
    f32 scale;                          /* 0x38 */
    struct NUSHADOWDATA_s *shadow;      /* 0x3C */
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
