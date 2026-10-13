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

typedef enum nunodetype_e {
    NUNODETYPE_NODE,
    NUNODETYPE_MESH,
    NUNODETYPE_TRANSFORM,
    NUNODETYPE_JOINT,
    NUNODETYPE_SKIN,
    NUNODETYPE_SPLINE
} NuNodeType;

typedef struct nunode_s {
    char *name;
    int id;
    NuNodeType type;
    int num_children;
    struct nunode_s *parent;
    struct nunode_s *child;
    struct nunode_s *next;
    void *data;
} NuNode;

typedef struct numtx_s {
    float m[4][4];
} NuMtx;

typedef struct numeshnode_s {
    NuNode node;
    void *geometry;
    char pad_24[4];
    int flags;
} NuMeshNode;

typedef struct nutransformnode_s {
    NuNode node;
    NuMtx mtx;
    NuMtx wmtx;
    int flags;
    char pad_a4[4];
} NuTransformNode;

typedef struct nujointnode_s {
    NuNode node;
    NuMtx orient;
    NuMtx mtx;
    NuMtx wmtx;
    char pad_e0[0x18];
    float scale[3];
    float rot[3];
    char locked;
} NuJointNode;

typedef struct nuskinnode_s {
    NuNode node;
    void *geometry;
    char pad_24[4];
    int num_joints;
    NuMtx **joints;
    NuMtx *joint_mtxs;
} NuSkinNode;

typedef struct nusplinenode_s {
    NuNode node;
    int num_splines;
    void **splines;
    void **spline_data;
} NuSplineNode;


typedef void (*NuErrorFunc)(const char *fmt, ...);
typedef void (*NuNodeDataFunc)(void *data);
typedef void (*NuNodeIterFunc)(NuNode *node, void *data);

extern NuErrorFunc NuErrorProlog(const char *file, int line);
extern void *NuMemAllocFn(int size, char *file, int line);
extern void *NuMemReAllocFn(void *ptr, int size, char *file, int line);
extern void NuMemFreeFn(void *ptr, char *file, int line);
extern void *memset(void *s, int c, unsigned int n);
extern int strcmp(const char *a, const char *b);
extern unsigned int strlen(const char *s);
extern char *strcpy(char *dst, const char *src);
extern char *strcat(char *dst, const char *src);
extern void NuMtxSetIdentity(NuMtx *m);
extern void NuMtxInv(NuMtx *dst, NuMtx *src);
extern void NuMtxMulVU0(NuMtx *dst, NuMtx *a, NuMtx *b);
extern int NuEquiv(float a, float b);
extern void NuEquivTollerance(float tolerance);

void NuMeshNodeDestroy(NuNode *node);
void NuTransformNodeDestroy(NuNode *node);
void NuJointNodeDestroy(NuNode *node);
void NuSkinNodeDestroy(NuSkinNode *skin);
void NuSplineNodeDestroy(NuSplineNode *spline);
void NuNodeUpdateXfms(NuNode *node, NuMtx *parent_mtx);

extern char D_006141E0[];
#define nunode_file D_006141E0

extern char D_00614200[];
#define bad_node_type_msg D_00614200

extern char D_00614248[];
#define bad_parent_type_msg D_00614248

extern char D_0062EB90[];
#define empty_name D_0062EB90

extern int D_0062EB88;
#define next_node_id D_0062EB88

extern int D_0062EB8C;
#define keep_names D_0062EB8C


inline char *NuNodeStrCpy(char *src)
{
    char *dst;

    if (src != 0) {
        dst = NuMemAllocFn(strlen(src) + 1, nunode_file, 0x60);
        strcpy(dst, src);
    } else {
        dst = NuMemAllocFn(1, nunode_file, 0x65);
        *dst = empty_name[0];
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
    *str = NuMemReAllocFn(*str, len + 1, nunode_file, 0x91);
    strcat(*str, append);
    return *str;
}


inline void NuNodeDestroy(NuNode *node)
{
    if (!keep_names && node->name != 0) {
        NuMemFreeFn(node->name, nunode_file, 0xCB);
    }
    switch (node->type) {
    case NUNODETYPE_NODE:
        NuMemFreeFn(node, nunode_file, 0xF7);
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
        NuSkinNodeDestroy((NuSkinNode *)node);
        break;
    case NUNODETYPE_SPLINE:
        NuSplineNodeDestroy((NuSplineNode *)node);
        break;
    default:
        NuErrorProlog(nunode_file, 0x109)(bad_node_type_msg);
        break;
    }
}


inline void NuNodeDestroyHierarchy(NuNode *node)
{
    NuTransformNode *xform;
    NuNode *child;
    NuNode *cur;

    xform = (NuTransformNode *)node;
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


inline NuNode *NuNodeFindByID(NuNode *node, int id)
{
    NuNode *child;
    NuNode *found;

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


static inline void resolveReferenceNodes(NuNode *root, NuNode *node)
{
    NuTransformNode *xform;
    NuNode *child;
    NuNode *cur;

    xform = (NuTransformNode *)node;
    if (node->type == NUNODETYPE_TRANSFORM && (xform->flags & 2)) {
        node->child = NuNodeFindByID(root, (int)node->data);
        return;
    }
    child = node->child;
    while (child != 0) {
        cur = child;
        child = child->next;
        resolveReferenceNodes(root, cur);
    }
}


inline void NuNodeResolveReferenceNodes(NuNode *root)
{
    resolveReferenceNodes(root, root);
}


static inline void resolveSkinNodes(NuNode *node, NuNode *root)
{
    NuSkinNode *skin;
    NuJointNode *joint;
    NuNode *child;
    int i;

    if (node->type == NUNODETYPE_SKIN) {
        skin = (NuSkinNode *)node;
        for (i = 0; i < skin->num_joints; i++) {
            joint = (NuJointNode *)NuNodeFindByID(root, (int)skin->joints[i]);
            skin->joints[i] = &joint->wmtx;
            NuMtxInv(&skin->joint_mtxs[i], skin->joints[i]);
        }
    }
    for (child = node->child; child != 0; child = child->next) {
        resolveSkinNodes(child, root);
    }
}


inline void NuNodeResolveSkinNodes(NuNode *root)
{
    resolveSkinNodes(root, root);
}


void NuNodeDestroyChildren(NuNode *node)
{
    NuNodeDestroyHierarchy(node->child);
    node->child = 0;
}


void NuNodeGraphInit(NuNode *root)
{
    NuMtx mtx;

    NuMtxSetIdentity(&mtx);
    NuNodeResolveReferenceNodes(root);
    NuNodeUpdateXfms(root, &mtx);
    NuNodeResolveSkinNodes(root);
}


int NuMtxEqual(NuMtx *a, NuMtx *b)
{
    NuEquivTollerance(0.001f);
    if (NuEquiv(a->m[0][0], b->m[0][0]) && NuEquiv(a->m[0][1], b->m[0][1])
        && NuEquiv(a->m[0][2], b->m[0][2]) && NuEquiv(a->m[0][3], b->m[0][3])
        && NuEquiv(a->m[1][0], b->m[1][0]) && NuEquiv(a->m[1][1], b->m[1][1])
        && NuEquiv(a->m[1][2], b->m[1][2]) && NuEquiv(a->m[1][3], b->m[1][3])
        && NuEquiv(a->m[2][0], b->m[2][0]) && NuEquiv(a->m[2][1], b->m[2][1])
        && NuEquiv(a->m[2][2], b->m[2][2]) && NuEquiv(a->m[2][3], b->m[2][3])
        && NuEquiv(a->m[3][0], b->m[3][0]) && NuEquiv(a->m[3][1], b->m[3][1])
        && NuEquiv(a->m[3][2], b->m[3][2]) && NuEquiv(a->m[3][3], b->m[3][3])) {
        return 1;
    }
    return 0;
}


NuNode *NuNodeCreate(void)
{
    NuNode *node;

    node = NuMemAllocFn(sizeof(NuNode), nunode_file, 0xDC);
    memset(node, 0, sizeof(NuNode));
    node->type = NUNODETYPE_NODE;
    node->id = next_node_id++;
    node->num_children = 0;
    node->child = 0;
    node->next = 0;
    node->parent = 0;
    node->name = 0;
    node->data = 0;
    return node;
}


void NuNodeSetName(NuNode *node, char *name)
{
    if (node->name != 0) {
        NuMemFreeFn(node->name, nunode_file, 0x119);
    }
    node->name = NuNodeStrCpy(name);
}


void NuNodePreAddChild(NuNode *parent, NuNode *child)
{
    NuNode *first;

    if (parent != 0 && child != 0) {
        switch (parent->type) {
        default:
            NuErrorProlog(nunode_file, 0x23F)(bad_parent_type_msg);
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


void NuNodeAddChild(NuNode *parent, NuNode *child)
{
    NuNode *last;

    if (parent != 0 && child != 0) {
        switch (parent->type) {
        default:
            NuErrorProlog(nunode_file, 0x211)(bad_parent_type_msg);
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


void NuNodeRemoveChild(NuNode *parent, NuNode *child)
{
    NuNode *node;

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


NuNode *NuNodeFindByName(NuNode *node, char *name)
{
    NuNode *child;
    NuNode *found;

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


void NuNodeIterator(NuNode *node, void *context, NuNodeType type, NuNodeIterFunc func, void *data)
{
    NuNode *child;

    if (type == NUNODETYPE_NODE || node->type == type) {
        func(node, data);
    }
    for (child = node->child; child != 0; child = child->next) {
        NuNodeIterator(child, context, type, func, data);
    }
}


void NuNodeDestroyHierarchyData(NuNode *node, NuNodeDataFunc destroy)
{
    NuNode *child;

    if (node->data != 0) {
        destroy(node->data);
    }
    for (child = node->child; child != 0; child = child->next) {
        NuNodeDestroyHierarchyData(child, destroy);
    }
}


int NuNodeNumBranchNodesOfType(NuNode *node, NuNodeType type)
{
    NuNode *child;
    int count;

    count = node->type == type;
    for (child = node->child; child != 0; child = child->next) {
        count += NuNodeNumBranchNodesOfType(child, type);
    }
    return count;
}


int NuNodeNumSubNodes(NuNode *node)
{
    NuNode *child;
    int count;

    count = 1;
    if (node->num_children != 0) {
        for (child = node->child; child != 0; child = child->next) {
            count += NuNodeNumSubNodes(child);
        }
    }
    return count;
}


void NuNodeUpdateXfms(NuNode *node, NuMtx *parent_mtx)
{
    NuMtx mtx;
    NuTransformNode *xform;
    NuJointNode *joint;
    NuNode *child;

    if (parent_mtx != 0) {
        mtx = *parent_mtx;
    } else {
        NuMtxSetIdentity(&mtx);
    }
    switch (node->type) {
    case NUNODETYPE_TRANSFORM:
        xform = (NuTransformNode *)node;
        NuMtxMulVU0(&mtx, &xform->mtx, &mtx);
        xform->wmtx = mtx;
        break;
    case NUNODETYPE_JOINT:
        joint = (NuJointNode *)node;
        NuMtxMulVU0(&mtx, &joint->mtx, &mtx);
        joint->wmtx = mtx;
        break;
    }
    for (child = node->child; child != 0; child = child->next) {
        NuNodeUpdateXfms(child, &mtx);
    }
}


NuMeshNode *NuMeshNodeCreate(void *geometry)
{
    NuMeshNode *mesh;

    mesh = NuMemAllocFn(sizeof(NuMeshNode), nunode_file, 0x35A);
    memset(mesh, 0, sizeof(NuMeshNode));
    mesh->node.type = NUNODETYPE_MESH;
    mesh->node.id = next_node_id++;
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


void NuMeshNodeDestroy(NuNode *node)
{
    NuMemFreeFn(node, nunode_file, 0x377);
}


NuTransformNode *NuTransformNodeCreate(NuMtx *mtx)
{
    NuTransformNode *xform;

    xform = NuMemAllocFn(sizeof(NuTransformNode), nunode_file, 0x2D6);
    memset(xform, 0, sizeof(NuTransformNode));
    xform->node.type = NUNODETYPE_TRANSFORM;
    xform->node.id = next_node_id++;
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


void NuTransformNodeDestroy(NuNode *node)
{
    NuMemFreeFn(node, nunode_file, 0x2EB);
}


NuJointNode *NuJointNodeCreate(NuMtx *mtx)
{
    NuJointNode *joint;

    joint = NuMemAllocFn(sizeof(NuJointNode), nunode_file, 0x2FC);
    memset(joint, 0, sizeof(NuJointNode));
    joint->node.type = NUNODETYPE_JOINT;
    joint->node.id = next_node_id++;
    joint->node.num_children = 0;
    joint->node.child = 0;
    joint->node.next = 0;
    joint->node.parent = 0;
    joint->node.name = 0;
    joint->node.data = 0;
    if (mtx != 0) {
        joint->mtx = *mtx;
    }
    joint->locked = 0;
    joint->scale[0] = 1.0f;
    joint->scale[1] = 1.0f;
    joint->scale[2] = 1.0f;
    joint->rot[0] = 0.0f;
    joint->rot[1] = 0.0f;
    joint->rot[2] = 0.0f;
    return joint;
}


void NuJointNodeDestroy(NuNode *node)
{
    NuMemFreeFn(node, nunode_file, 0x31A);
}


NuSkinNode *NuSkinNodeCreate(int num_joints)
{
    NuSkinNode *skin;

    skin = NuMemAllocFn(sizeof(NuSkinNode), nunode_file, 0x32A);
    memset(skin, 0, sizeof(NuSkinNode));
    skin->node.type = NUNODETYPE_SKIN;
    skin->node.id = next_node_id++;
    skin->node.num_children = 0;
    skin->node.child = 0;
    skin->node.next = 0;
    skin->node.parent = 0;
    skin->node.name = 0;
    skin->node.data = 0;
    skin->geometry = 0;
    skin->num_joints = num_joints;
    if (num_joints != 0) {
        skin->joints = NuMemAllocFn(num_joints * sizeof(int), nunode_file, 0x333);
        skin->joint_mtxs = NuMemAllocFn(num_joints * sizeof(NuMtx), nunode_file, 0x334);
    }
    return skin;
}


void NuSkinNodeDestroy(NuSkinNode *skin)
{
    if (skin->num_joints != 0) {
        NuMemFreeFn(skin->joints, nunode_file, 0x346);
        NuMemFreeFn(skin->joint_mtxs, nunode_file, 0x347);
    }
    NuMemFreeFn(skin, nunode_file, 0x349);
}


NuSplineNode *NuSplineNodeCreate(int num_splines)
{
    NuSplineNode *spline;

    spline = NuMemAllocFn(sizeof(NuSplineNode), nunode_file, 0x389);
    memset(spline, 0, sizeof(NuSplineNode));
    spline->node.type = NUNODETYPE_SPLINE;
    spline->node.id = next_node_id++;
    spline->node.num_children = 0;
    spline->node.child = 0;
    spline->node.next = 0;
    spline->node.parent = 0;
    spline->node.name = 0;
    spline->node.data = 0;
    spline->num_splines = num_splines;
    spline->splines = NuMemAllocFn(num_splines * sizeof(void *), nunode_file, 0x38E);
    spline->spline_data = NuMemAllocFn(num_splines * sizeof(void *), nunode_file, 0x38F);
    return spline;
}


void NuSplineNodeDestroy(NuSplineNode *spline)
{
    if (spline->splines != 0) {
        NuMemFreeFn(spline->splines, nunode_file, 0x3A1);
    }
    if (spline->spline_data != 0) {
        NuMemFreeFn(spline->spline_data, nunode_file, 0x3A3);
    }
    NuMemFreeFn(spline, nunode_file, 0x3A4);
}


void NuNodeKeepNames(int keep)
{
    keep_names = keep;
}


void NuMeshNodeSetGeometry(struct numeshnode_s *node, void *geometry)
{
    node->geometry = geometry;
}
