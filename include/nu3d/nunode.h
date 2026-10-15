#ifndef NU3D_NUNODE_H
#define NU3D_NUNODE_H

#include "nu_types.h"
#include "numath/numathtypes.h"

struct nugobj_s;
struct nuspline_s;

enum nunodetype_e {
    NUNODETYPE_NODE,
    NUNODETYPE_MESH,
    NUNODETYPE_TRANSFORM,
    NUNODETYPE_JOINT,
    NUNODETYPE_SKIN,
    NUNODETYPE_SPLINE
};

struct nunode_s {
    char *name;                         /* 0x00 */
    int id;                             /* 0x04 */
    enum nunodetype_e type;             /* 0x08 */
    int num_children;                   /* 0x0C */
    struct nunode_s *parent;            /* 0x10 */
    struct nunode_s *child;             /* 0x14 */
    struct nunode_s *next;              /* 0x18 */
    void *data;                         /* 0x1C */
}; /* 0x20 */

struct numeshnode_s {
    struct nunode_s node;               /* 0x00 */
    struct nugobj_s *geometry;          /* 0x20 */
    char pad24[0x04];                   /* 0x24 */
    int flags;                          /* 0x28 */
}; /* 0x2C */

struct nutransformnode_s {
    struct nunode_s node;               /* 0x00 */
    struct numtx_s mtx;                 /* 0x20 */
    struct numtx_s wmtx;                /* 0x60 */
    int flags;                          /* 0xA0 */
    char padA4[0x04];                   /* 0xA4 */
}; /* 0xA8 */

struct nujointnode_s {
    struct nunode_s node;               /* 0x000 */
    struct numtx_s orient;              /* 0x020 */
    struct numtx_s mtx;                 /* 0x060 */
    struct numtx_s wmtx;                /* 0x0A0 */
    char pad0E0[0x18];                  /* 0x0E0 */
    struct nuvec_s scale;               /* 0x0F8 */
    struct nuvec_s pivot;               /* 0x104 */
    u8 flags;                           /* 0x110 */
}; /* 0x114 */

struct nuskinnode_s {
    struct nunode_s node;               /* 0x00 */
    struct nugobj_s *geometry;          /* 0x20 */
    char pad24[0x04];                   /* 0x24 */
    int num_joints;                     /* 0x28 */
    union {
        struct numtx_s **joints;        /* 0x2C once NuNodeResolveSkinNodes ran */
        int *joint_ids;                 /* 0x2C node ids, as read from the file */
    };
    struct numtx_s *joint_mtxs;         /* 0x30 */
}; /* 0x34 */

struct nusplinenode_s {
    struct nunode_s node;               /* 0x00 */
    int num_splines;                    /* 0x20 */
    struct nuspline_s **splines;        /* 0x24 */
    void **spline_data;                 /* 0x28 */
}; /* 0x2C */

typedef void (*NuNodeDataFunc)(void *data);
typedef void (*NuNodeIterFunc)(struct nunode_s *node, void *data);

void NuNodeDestroyChildren(struct nunode_s *node);
void NuNodeGraphInit(struct nunode_s *root);
int NuMtxEqual(struct numtx_s *a, struct numtx_s *b);
struct nunode_s *NuNodeCreate(void);
void NuNodeSetName(struct nunode_s *node, char *name);
void NuNodePreAddChild(struct nunode_s *parent, struct nunode_s *child);
void NuNodeAddChild(struct nunode_s *parent, struct nunode_s *child);
void NuNodeRemoveChild(struct nunode_s *parent, struct nunode_s *child);
int NuNodeNumChildren(struct nunode_s *node);
struct nunode_s *NuNodeFindByName(struct nunode_s *node, char *name);
void NuNodeIterator(struct nunode_s *node, void *context, enum nunodetype_e type, NuNodeIterFunc func, void *data);
void NuNodeDestroyHierarchyData(struct nunode_s *node, NuNodeDataFunc destroy);
int NuNodeNumBranchNodesOfType(struct nunode_s *node, enum nunodetype_e type);
int NuNodeNumSubNodes(struct nunode_s *node);
void NuNodeUpdateXfms(struct nunode_s *node, struct numtx_s *parent_mtx);
struct numeshnode_s *NuMeshNodeCreate(void *geometry);
void NuMeshNodeDestroy(struct nunode_s *node);
struct nutransformnode_s *NuTransformNodeCreate(struct numtx_s *mtx);
void NuTransformNodeDestroy(struct nunode_s *node);
struct nujointnode_s *NuJointNodeCreate(struct numtx_s *mtx);
void NuJointNodeDestroy(struct nunode_s *node);
struct nuskinnode_s *NuSkinNodeCreate(int num_joints);
void NuSkinNodeDestroy(struct nuskinnode_s *skin);
struct nusplinenode_s *NuSplineNodeCreate(int num_splines);
void NuSplineNodeDestroy(struct nusplinenode_s *spline);
void NuNodeKeepNames(int keep);
void NuMeshNodeSetGeometry(struct numeshnode_s *node, void *geometry);

#endif /* NU3D_NUNODE_H */
