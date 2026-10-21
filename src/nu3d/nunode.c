/*
 * Unit: nu3d/nunode
 *
 * Functions:
 *   0x001165b0 NuNodeDestroyChildren
 *   0x001166f8 NuNodeGraphInit
 *   0x001168c0 NuMtxEqual
 *   0x00116a40 NuNodeCreate
 *   0x00116ab8 NuNodeDestroy
 *   0x00116ba8 NuNodeSetName
 *   0x00116c50 NuNodeDestroyHierarchy
 *   0x00116d88 NuNodePreAddChild
 *   0x00116e08 NuNodeAddChild
 *   0x00116eb0 NuNodeRemoveChild
 *   0x00116f20 NuNodeNumChildren
 *   0x00116f28 NuNodeFindByName
 *   0x00116fa8 NuNodeFindByID
 *   0x00117018 NuNodeIterator
 *   0x001170b8 NuNodeDestroyHierarchyData
 *   0x00117118 NuNodeNumBranchNodesOfType
 *   0x00117180 NuNodeNumSubNodes
 *   0x001171d8 NuNodeUpdateXfms
 *   0x001173f8 NuMeshNodeCreate
 *   0x00117488 NuMeshNodeDestroy
 *   0x001174b0 NuTransformNodeCreate
 *   0x001175b8 NuTransformNodeDestroy
 *   0x001175e0 NuJointNodeCreate
 *   0x00117710 NuJointNodeDestroy
 *   0x00117738 NuSkinNodeCreate
 *   0x00117800 NuSkinNodeDestroy
 *   0x00117870 NuSplineNodeCreate
 *   0x00117938 NuSplineNodeDestroy
 *   0x001179a0 NuNodeStrCpy
 *   0x00117a28 NuNodeStrToUpper
 *   0x00117aa0 NuNodeStrCat
 *   0x00117b18 NuNodeKeepNames
 *   0x00117b20 resolveReferenceNodes
 *   0x00117be8 NuNodeResolveReferenceNodes
 *   0x00117ca8 NuMeshNodeSetGeometry
 *   0x00117cb0 resolveSkinNodes
 *   0x00117de8 NuNodeResolveSkinNodes
 */

#include "nu_libc.h"

#include "nucore/nuerror.h"
#include "numath/nufloat.h"
#include "numath/numathtypes.h"
#include "numath/numtx.h"
#include "nu3d/nunode.h"

extern void *NuMemAllocFn(int size, char *file, int line);
extern void *NuMemReAllocFn(void *ptr, int size, char *file, int line);
extern void NuMemFreeFn(void *ptr, char *file, int line);
extern void NuMtxMulVU0(struct numtx_s *dst, struct numtx_s *a, struct numtx_s *b);

static int global_id = 0;
static int g_nunode_keep_names = 1;


inline char *NuNodeStrCpy(char *src)
{
    char *dst;

    if (src != 0) {
        dst = NuMemAllocFn(strlen(src) + 1, "..\\nu2crash.ps2\\nu3d\\nunode.c", 0x60);
        strcpy(dst, src);
    } else {
        dst = NuMemAllocFn(1, "..\\nu2crash.ps2\\nu3d\\nunode.c", 0x65);
        strcpy(dst, "");
    }
    return dst;
}


inline void NuNodeStrToUpper(char *str)
{
    int i;

    for (i = 0; i < strlen(str); i++) {
        str[i] = (str[i] >= 'a' && str[i] <= 'z') ? str[i] - ('a' - 'A') : str[i];
    }
}


inline char *NuNodeStrCat(char **str, char *append)
{
    int len;

    len = strlen(*str);
    len += strlen(append);
    *str = NuMemReAllocFn(*str, len + 1, "..\\nu2crash.ps2\\nu3d\\nunode.c", 0x91);
    strcat(*str, append);
    return *str;
}


inline void NuNodeDestroy(struct nunode_s *node)
{
    if (!g_nunode_keep_names && node->name != 0) {
        NuMemFreeFn(node->name, "..\\nu2crash.ps2\\nu3d\\nunode.c", 0xCB);
    }
    switch (node->type) {
    case NUNODETYPE_NODE:
        NuMemFreeFn(node, "..\\nu2crash.ps2\\nu3d\\nunode.c", 0xF7);
        break;
    case NUNODETYPE_MESH:
        NuMeshNodeDestroy(node);
        break;
    case NUNODETYPE_TRANSFORM:
        NuTransformNodeDestroy(node);
        break;
    case NUNODETYPE_JOINT:
        NuJointNodeDestroy(node);
        break;
    case NUNODETYPE_SKIN:
        NuSkinNodeDestroy((struct nuskinnode_s *)node);
        break;
    case NUNODETYPE_SPLINE:
        NuSplineNodeDestroy((struct nusplinenode_s *)node);
        break;
    default:
        NuErrorProlog("..\\nu2crash.ps2\\nu3d\\nunode.c", 0x109)(
            "NuNodeDestroy: Unknown node type.\n");
        break;
    }
}


inline void NuNodeDestroyHierarchy(struct nunode_s *node)
{
    struct nutransformnode_s *xform;
    struct nunode_s *child;
    struct nunode_s *cur;

    xform = (struct nutransformnode_s *)node;
    child = node->child;
    if (node->type != NUNODETYPE_TRANSFORM || !(xform->flags & 2)) {
        while (child != 0) {
            cur = child;
            child = child->next;
            NuNodeDestroyHierarchy(cur);
        }
    }
    NuNodeDestroy(node);
}


inline struct nunode_s *NuNodeFindByID(struct nunode_s *node, int id)
{
    struct nunode_s *child;
    struct nunode_s *found;

    child = node->child;
    if (node->id == id) {
        return node;
    }
    for (; child != 0; child = child->next) {
        found = NuNodeFindByID(child, id);
        if (found != 0) {
            return found;
        }
    }
    return 0;
}


static inline void resolveReferenceNodes(struct nunode_s *root, struct nunode_s *node)
{
    struct nutransformnode_s *xform;
    struct nunode_s *child;
    struct nunode_s *cur;

    xform = (struct nutransformnode_s *)node;
    if (node->type == NUNODETYPE_TRANSFORM && (xform->flags & 2)) {
        node->child = NuNodeFindByID(root, (nusintptr)node->data);
        return;
    }
    child = node->child;
    while (child != 0) {
        cur = child;
        child = child->next;
        resolveReferenceNodes(root, cur);
    }
}


inline void NuNodeResolveReferenceNodes(struct nunode_s *root)
{
    resolveReferenceNodes(root, root);
}


static inline void resolveSkinNodes(struct nunode_s *node, struct nunode_s *root)
{
    struct nuskinnode_s *skin;
    struct nujointnode_s *joint;
    struct nunode_s *child;
    int i;

    if (node->type == NUNODETYPE_SKIN) {
        skin = (struct nuskinnode_s *)node;
        for (i = 0; i < skin->num_joints; i++) {
            joint = (struct nujointnode_s *)NuNodeFindByID(root, (nusintptr)skin->joints[i]);
            skin->joints[i] = &joint->wmtx;
            NuMtxInv(&skin->joint_mtxs[i], skin->joints[i]);
        }
    }
    for (child = node->child; child != 0; child = child->next) {
        resolveSkinNodes(child, root);
    }
}


inline void NuNodeResolveSkinNodes(struct nunode_s *root)
{
    resolveSkinNodes(root, root);
}


void NuNodeDestroyChildren(struct nunode_s *node)
{
    NuNodeDestroyHierarchy(node->child);
    node->child = 0;
}


/* inline only defers the output; defined here, its message is emitted between
   NuNodeDestroyChildren's and NuNodeDestroy's jump tables as retail has it */
inline void NuNodeAddChild(struct nunode_s *parent, struct nunode_s *child)
{
    struct nunode_s *last;

    if (parent != 0 && child != 0) {
        switch (parent->type) {
        default:
            NuErrorProlog("..\\nu2crash.ps2\\nu3d\\nunode.c", 0x211)(
                "NuNodeAddChild: Only works on transform, joint or null nodes.");
            break;
        case NUNODETYPE_NODE:
        case NUNODETYPE_TRANSFORM:
        case NUNODETYPE_JOINT:
            child->parent = parent;
            if (parent->child != 0) {
                last = parent->child;
                while (last->next != 0) {
                    last = last->next;
                }
                last->next = child;
            } else {
                parent->child = child;
            }
            parent->num_children++;
            break;
        }
    }
}


void NuNodeGraphInit(struct nunode_s *root)
{
    struct numtx_s mtx;

    NuMtxSetIdentity(&mtx);
    NuNodeResolveReferenceNodes(root);
    NuNodeUpdateXfms(root, &mtx);
    NuNodeResolveSkinNodes(root);
}


int NuMtxEqual(struct numtx_s *a, struct numtx_s *b)
{
    NuEquivTollerance(0.001f);
    if (NuEquiv(a->_00, b->_00) && NuEquiv(a->_01, b->_01)
        && NuEquiv(a->_02, b->_02) && NuEquiv(a->_03, b->_03)
        && NuEquiv(a->_10, b->_10) && NuEquiv(a->_11, b->_11)
        && NuEquiv(a->_12, b->_12) && NuEquiv(a->_13, b->_13)
        && NuEquiv(a->_20, b->_20) && NuEquiv(a->_21, b->_21)
        && NuEquiv(a->_22, b->_22) && NuEquiv(a->_23, b->_23)
        && NuEquiv(a->_30, b->_30) && NuEquiv(a->_31, b->_31)
        && NuEquiv(a->_32, b->_32) && NuEquiv(a->_33, b->_33)) {
        return 1;
    }
    return 0;
}


struct nunode_s *NuNodeCreate(void)
{
    struct nunode_s *node;

    node = NuMemAllocFn(sizeof(struct nunode_s), "..\\nu2crash.ps2\\nu3d\\nunode.c", 0xDC);
    memset(node, 0, sizeof(struct nunode_s));
    node->type = NUNODETYPE_NODE;
    node->id = global_id++;
    node->num_children = 0;
    node->child = 0;
    node->next = 0;
    node->parent = 0;
    node->name = 0;
    node->data = 0;
    return node;
}


void NuNodeSetName(struct nunode_s *node, char *name)
{
    if (node->name != 0) {
        NuMemFreeFn(node->name, "..\\nu2crash.ps2\\nu3d\\nunode.c", 0x119);
    }
    node->name = NuNodeStrCpy(name);
}


void NuNodePreAddChild(struct nunode_s *parent, struct nunode_s *child)
{
    struct nunode_s *first;

    if (parent != 0 && child != 0) {
        switch (parent->type) {
        default:
            NuErrorProlog("..\\nu2crash.ps2\\nu3d\\nunode.c", 0x23F)(
                "NuNodeAddChild: Only works on transform, joint or null nodes.");
            break;
        case NUNODETYPE_NODE:
        case NUNODETYPE_TRANSFORM:
        case NUNODETYPE_JOINT:
            child->parent = parent;
            first = parent->child;
            parent->child = child;
            child->next = first;
            parent->num_children++;
            break;
        }
    }
}


void NuNodeRemoveChild(struct nunode_s *parent, struct nunode_s *child)
{
    struct nunode_s *node;

    if (parent != 0 && child != 0) {
        node = parent->child;
        if (node == child) {
            parent->child = child->next;
            parent->num_children--;
        } else {
            do {
                node = node->next;
            } while (node != child && node != 0);
            if (node == 0) {
                return;
            }
            child->next = node->next;
            parent->num_children--;
        }
        child->parent = 0;
    }
}


int NuNodeNumChildren(struct nunode_s *node)
{
    return node->num_children;
}


struct nunode_s *NuNodeFindByName(struct nunode_s *node, char *name)
{
    struct nunode_s *child;
    struct nunode_s *found;

    child = node->child;
    if (node->name != 0 && strcmp(node->name, name) == 0) {
        return node;
    }
    for (; child != 0; child = child->next) {
        found = NuNodeFindByName(child, name);
        if (found != 0) {
            return found;
        }
    }
    return 0;
}


void NuNodeIterator(struct nunode_s *node, void *context, enum nunodetype_e type, NuNodeIterFunc func, void *data)
{
    struct nunode_s *child;

    if (type == NUNODETYPE_NODE || node->type == type) {
        func(node, data);
    }
    for (child = node->child; child != 0; child = child->next) {
        NuNodeIterator(child, context, type, func, data);
    }
}


void NuNodeDestroyHierarchyData(struct nunode_s *node, NuNodeDataFunc destroy)
{
    struct nunode_s *child;

    if (node->data != 0) {
        destroy(node->data);
    }
    for (child = node->child; child != 0; child = child->next) {
        NuNodeDestroyHierarchyData(child, destroy);
    }
}


int NuNodeNumBranchNodesOfType(struct nunode_s *node, enum nunodetype_e type)
{
    struct nunode_s *child;
    int count;

    count = node->type == type;
    for (child = node->child; child != 0; child = child->next) {
        count += NuNodeNumBranchNodesOfType(child, type);
    }
    return count;
}


int NuNodeNumSubNodes(struct nunode_s *node)
{
    struct nunode_s *child;
    int count;

    count = 1;
    if (node->num_children != 0) {
        for (child = node->child; child != 0; child = child->next) {
            count += NuNodeNumSubNodes(child);
        }
    }
    return count;
}


void NuNodeUpdateXfms(struct nunode_s *node, struct numtx_s *parent_mtx)
{
    struct numtx_s mtx;
    struct nutransformnode_s *xform;
    struct nujointnode_s *joint;
    struct nunode_s *child;

    if (parent_mtx != 0) {
        mtx = *parent_mtx;
    } else {
        NuMtxSetIdentity(&mtx);
    }
    switch (node->type) {
    case NUNODETYPE_TRANSFORM:
        xform = (struct nutransformnode_s *)node;
        NuMtxMulVU0(&mtx, &xform->mtx, &mtx);
        xform->wmtx = mtx;
        break;
    case NUNODETYPE_JOINT:
        joint = (struct nujointnode_s *)node;
        NuMtxMulVU0(&mtx, &joint->mtx, &mtx);
        joint->wmtx = mtx;
        break;
    }
    for (child = node->child; child != 0; child = child->next) {
        NuNodeUpdateXfms(child, &mtx);
    }
}


struct numeshnode_s *NuMeshNodeCreate(void *geometry)
{
    struct numeshnode_s *mesh;

    mesh = NuMemAllocFn(sizeof(struct numeshnode_s), "..\\nu2crash.ps2\\nu3d\\nunode.c", 0x35A);
    memset(mesh, 0, sizeof(struct numeshnode_s));
    mesh->node.type = NUNODETYPE_MESH;
    mesh->node.id = global_id++;
    mesh->geometry = geometry;
    mesh->node.num_children = 0;
    mesh->node.child = 0;
    mesh->node.next = 0;
    mesh->node.parent = 0;
    mesh->node.name = 0;
    mesh->node.data = 0;
    mesh->flags = 0;
    return mesh;
}


void NuMeshNodeDestroy(struct nunode_s *node)
{
    NuMemFreeFn(node, "..\\nu2crash.ps2\\nu3d\\nunode.c", 0x377);
}


struct nutransformnode_s *NuTransformNodeCreate(struct numtx_s *mtx)
{
    struct nutransformnode_s *xform;

    xform = NuMemAllocFn(sizeof(struct nutransformnode_s), "..\\nu2crash.ps2\\nu3d\\nunode.c", 0x2D6);
    memset(xform, 0, sizeof(struct nutransformnode_s));
    xform->node.type = NUNODETYPE_TRANSFORM;
    xform->node.id = global_id++;
    xform->node.num_children = 0;
    xform->node.child = 0;
    xform->node.next = 0;
    xform->node.parent = 0;
    xform->node.name = 0;
    xform->node.data = 0;
    if (mtx != 0) {
        xform->mtx = *mtx;
    }
    return xform;
}


void NuTransformNodeDestroy(struct nunode_s *node)
{
    NuMemFreeFn(node, "..\\nu2crash.ps2\\nu3d\\nunode.c", 0x2EB);
}


struct nujointnode_s *NuJointNodeCreate(struct numtx_s *mtx)
{
    struct nujointnode_s *joint;

    joint = NuMemAllocFn(sizeof(struct nujointnode_s), "..\\nu2crash.ps2\\nu3d\\nunode.c", 0x2FC);
    memset(joint, 0, sizeof(struct nujointnode_s));
    joint->node.type = NUNODETYPE_JOINT;
    joint->node.id = global_id++;
    joint->node.num_children = 0;
    joint->node.child = 0;
    joint->node.next = 0;
    joint->node.parent = 0;
    joint->node.name = 0;
    joint->node.data = 0;
    if (mtx != 0) {
        joint->mtx = *mtx;
    }
    joint->flags = 0;
    joint->scale.x = 1.0f;
    joint->scale.y = 1.0f;
    joint->scale.z = 1.0f;
    joint->pivot.x = 0.0f;
    joint->pivot.y = 0.0f;
    joint->pivot.z = 0.0f;
    return joint;
}


void NuJointNodeDestroy(struct nunode_s *node)
{
    NuMemFreeFn(node, "..\\nu2crash.ps2\\nu3d\\nunode.c", 0x31A);
}


struct nuskinnode_s *NuSkinNodeCreate(int num_joints)
{
    struct nuskinnode_s *skin;

    skin = NuMemAllocFn(sizeof(struct nuskinnode_s), "..\\nu2crash.ps2\\nu3d\\nunode.c", 0x32A);
    memset(skin, 0, sizeof(struct nuskinnode_s));
    skin->node.type = NUNODETYPE_SKIN;
    skin->node.id = global_id++;
    skin->node.num_children = 0;
    skin->node.child = 0;
    skin->node.next = 0;
    skin->node.parent = 0;
    skin->node.name = 0;
    skin->node.data = 0;
    skin->geometry = 0;
    skin->num_joints = num_joints;
    if (num_joints != 0) {
        skin->joints = NuMemAllocFn(num_joints * sizeof(int), "..\\nu2crash.ps2\\nu3d\\nunode.c", 0x333);
        skin->joint_mtxs = NuMemAllocFn(num_joints * sizeof(struct numtx_s), "..\\nu2crash.ps2\\nu3d\\nunode.c", 0x334);
    }
    return skin;
}


void NuSkinNodeDestroy(struct nuskinnode_s *skin)
{
    if (skin->num_joints != 0) {
        NuMemFreeFn(skin->joints, "..\\nu2crash.ps2\\nu3d\\nunode.c", 0x346);
        NuMemFreeFn(skin->joint_mtxs, "..\\nu2crash.ps2\\nu3d\\nunode.c", 0x347);
    }
    NuMemFreeFn(skin, "..\\nu2crash.ps2\\nu3d\\nunode.c", 0x349);
}


struct nusplinenode_s *NuSplineNodeCreate(int num_splines)
{
    struct nusplinenode_s *spline;

    spline = NuMemAllocFn(sizeof(struct nusplinenode_s), "..\\nu2crash.ps2\\nu3d\\nunode.c", 0x389);
    memset(spline, 0, sizeof(struct nusplinenode_s));
    spline->node.type = NUNODETYPE_SPLINE;
    spline->node.id = global_id++;
    spline->node.num_children = 0;
    spline->node.child = 0;
    spline->node.next = 0;
    spline->node.parent = 0;
    spline->node.name = 0;
    spline->node.data = 0;
    spline->num_splines = num_splines;
    spline->splines = NuMemAllocFn(num_splines * sizeof(void *), "..\\nu2crash.ps2\\nu3d\\nunode.c", 0x38E);
    spline->spline_data = NuMemAllocFn(num_splines * sizeof(void *), "..\\nu2crash.ps2\\nu3d\\nunode.c", 0x38F);
    return spline;
}


void NuSplineNodeDestroy(struct nusplinenode_s *spline)
{
    if (spline->splines != 0) {
        NuMemFreeFn(spline->splines, "..\\nu2crash.ps2\\nu3d\\nunode.c", 0x3A1);
    }
    if (spline->spline_data != 0) {
        NuMemFreeFn(spline->spline_data, "..\\nu2crash.ps2\\nu3d\\nunode.c", 0x3A3);
    }
    NuMemFreeFn(spline, "..\\nu2crash.ps2\\nu3d\\nunode.c", 0x3A4);
}


void NuNodeKeepNames(int keep)
{
    g_nunode_keep_names = keep;
}


void NuMeshNodeSetGeometry(struct numeshnode_s *node, void *geometry)
{
    node->geometry = geometry;
}
