/*
 * Unit: gamelib/trigger
 *
 * Functions:
 *   0x001af0f8 NuTriggerSysLoad
 *   0x001af278 CheckParentedTriggerWithPos
 *   0x001af5f8 CheckUnparentedTriggerWithPos
 *   0x001af8a8 NuTriggerSysUpdate
 *   0x001afaa8 WeaponCheckTriggers
 *   0x001afc78 DebugRenderTriggers
 *   0x001b0c38 NuTriggerSysInit
 *   0x001b0c40 NuTriggerSysRender
 *   0x001b0c48 instNuTriggerSysCreate
 *   0x001b0d70 instNuTriggerSysDestroy
 *   0x001b0da0 instNuTriggerSetFlag
 *   0x001b0e90 instNuTriggerReset
 *   0x001b0f48 instNuTriggerEnable
 *   0x001b0f60 instNuTriggerDisable
 *   0x001b0f70 NuTriggerPull
 */

typedef float f32;

struct nuvec_s {
    f32 x;
    f32 y;
    f32 z;
};

struct numtx_s {
    f32 _00, _01, _02, _03;
    f32 _10, _11, _12, _13;
    f32 _20, _21, _22, _23;
    struct nuvec_s pos; /* 0x30 -- translation row */
    f32 _33;            /* 0x3C */
};

/* Renderer vertex; NuRndrLine3d takes two of them. */
struct NuRndrVtx {
    struct nuvec_s pos; /* 0x00 */
    char pad0c[0x0C];   /* 0x0C */
    int col;            /* 0x18 */
    char pad1c[8];      /* 0x1C */
};

/* Sub-record shape 1/3: a sphere (radius) or an upright line (height). */
typedef struct NuTriggerSphere {
    struct nuvec_s pos; /* 0x00 */
    f32 radius;         /* 0x0C */
} NuTriggerSphere;

/* Sub-record shape 2: an oriented box. */
typedef struct NuTriggerBox {
    struct numtx_s mtx;   /* 0x00 */
    struct nuvec_s bbmin; /* 0x40 */
    struct nuvec_s bbmax; /* 0x4C */
} NuTriggerBox;

/* One 8-byte sub-record; only its 0x4 pointer is fixed up on load. */
typedef struct NuTriggerSub {
    unsigned int type; /* 0x00 -- 0 none, 1 sphere, 2 box, 3 line */
    void *ptr;  /* 0x04 -- relocated */
} NuTriggerSub;

/* One trigger definition record (0x34 bytes), tiled in the loaded blob. */
typedef struct NuTriggerDef {
    char *name;              /* 0x00 -- relocated on load */
    unsigned int mode;       /* 0x04 -- 1 spatial, 2 disabled, else manual */
    char pad08[2];           /* 0x08 */
    unsigned char resetflag; /* 0x0A -- default flag byte */
    char pad0b[1];           /* 0x0B */
    int defflags;            /* 0x0C -- bit2 = emit un-pull script */
    f32 radius;              /* 0x10 */
    struct nuvec_s bbmin;    /* 0x14 */
    struct nuvec_s bbmax;    /* 0x20 */
    short subcount;          /* 0x2C */
    short parent;            /* 0x2E -- node index, < 0 when unparented */
    NuTriggerSub *subs;      /* 0x30 -- relocated on load */
} NuTriggerDef;

/* Header of the loaded trigger-def blob. */
typedef struct NuTriggerDefHdr {
    int version;         /* 0x00 -- must be 1 */
    int reloc;           /* 0x04 -- load delta */
    int count;           /* 0x08 -- number of NuTriggerDef records */
    NuTriggerDef *defs;  /* 0x0C -- the records */
} NuTriggerDefHdr;

/* Per-trigger runtime state (4 bytes). */
typedef struct NuTrigger {
    unsigned char pad0;  /* 0x00 */
    unsigned char pad1;  /* 0x01 */
    unsigned char flags; /* 0x02 */
    unsigned char state; /* 0x03 -- bit0 = pulled */
} NuTrigger;

/* One animated node of the owning scene instance; the trigger's parent
 * index selects one of these. */
struct NuTriggerNode {
    struct numtx_s mtx;        /* 0x00 */
    char pad40[4];             /* 0x40 */
    int flags;                 /* 0x44 -- bit0 = active */
    struct NuTriggerNode *ref; /* 0x48 -- optional override node */
    char pad4c[4];             /* 0x4C */
};

struct NuSceneInstData {
    char pad00[0x1C];
    struct NuTriggerNode *nodes; /* 0x1C */
};

struct NuSceneInst {
    char pad00[0x10];
    struct NuSceneInstData *inst; /* 0x10 */
};

/* One live trigger system. Carved (16-byte aligned, 0x18-byte header) out
 * of the caller's bump-heap and threaded onto the global list below. */
typedef struct NuTriggerSys {
    struct NuTriggerSys *next; /* 0x00 */
    struct NuTriggerSys *prev; /* 0x04 */
    NuTriggerDefHdr *def;      /* 0x08 -- the definition passed in a0 */
    NuTrigger *triggers;       /* 0x0C -- per-trigger array (def->count * 4) */
    struct NuSceneInst *data;  /* 0x10 -- opaque payload passed in a1 */
    int userflags;             /* 0x14 -- bit0 = disabled */
} NuTriggerSys;


typedef void (*NuErrorFunc)(const char *fmt, ...);

extern void *memset(void *s, int c, int n);
extern int strcasecmp(const char *a, const char *b);
extern int sprintf(char *buf, const char *fmt, ...);
extern int NuSceneInstanceRunScript(void *scene, void *script);
extern int NuFileLoadBuffer(void *name, void *buf, int size);
extern NuErrorFunc NuErrorProlog(const char *file, int line);

extern char D_00619EF8[];
extern char D_00619F20[];
extern char D_00619F60[];

int CheckParentedTriggerWithPos(NuTriggerDef *def, struct NuTriggerNode *node,
                                struct nuvec_s *pos, float radius);
int CheckUnparentedTriggerWithPos(NuTriggerDef *def, struct nuvec_s *pos,
                                  float radius);
void NuRndrLine3d(struct NuRndrVtx *line, void *arg1, struct numtx_s *mtx);
void NuVecMtxTransform(struct nuvec_s *dst, struct nuvec_s *src,
                       struct numtx_s *mtx);
void NuMtxInvH(struct numtx_s *dst, struct numtx_s *src);

extern NuTriggerSys *D_00633230;
#define g_NuTriggerSysList D_00633230

extern int D_0062F7F8;
#define g_DbgTriggerExtents D_0062F7F8

extern int D_0062F7FC;
#define g_DbgTriggerShapes D_0062F7FC

extern int D_0062F800;
#define g_DbgTriggerOff D_0062F800


void *NuTriggerSysLoad(char *name, void **heap, void **heapend)
{
    NuTriggerDefHdr *blob;
    int size;
    int i;

    *heap = (void *)(((unsigned int)*heap + 0xF) & ~0xFU);
    size = NuFileLoadBuffer(name, *heap, (int)*heapend - (int)*heap);
    if (size == 0)
        return (void *)0;

    blob = (NuTriggerDefHdr *)*heap;
    *heap = (char *)*heap + size;

    if (blob->version != 1)
        NuErrorProlog(D_00619EF8, 0x87)(D_00619F20, name, blob->version);

    blob->reloc = (int)blob - blob->reloc;
    blob->defs = blob->defs ? (NuTriggerDef *)((char *)blob->defs + blob->reloc)
                            : (NuTriggerDef *)0;
    if (blob->defs == 0)
        return blob;

    for (i = 0; i < blob->count; i++) {
        NuTriggerDef *d = &blob->defs[i];
        int k;

        d->name = d->name ? (char *)(d->name + blob->reloc) : (char *)0;
        d->subs = d->subs ? (NuTriggerSub *)((char *)d->subs + blob->reloc)
                          : (NuTriggerSub *)0;
        if (d->subs == 0)
            continue;

        for (k = 0; k < d->subcount; k++) {
            NuTriggerSub *e = &d->subs[k];
            e->ptr = e->ptr ? (void *)((char *)e->ptr + blob->reloc) : (void *)0;
        }
    }

    return blob;
}


void NuTriggerSysUpdate(struct nuvec_s *pos, f32 radius)
{
    NuTriggerSys *sys;
    NuTriggerDefHdr *def;
    struct NuSceneInstData *inst;
    NuTriggerDef *d;
    NuTrigger *trig;
    int i;
    int active;
    char buf[32];

    for (sys = g_NuTriggerSysList; sys != 0; sys = sys->next) {
        inst = sys->data->inst;
        def = sys->def;
        if (sys->userflags & 1)
            continue;
        for (i = 0; i < def->count; i++) {
            trig = &sys->triggers[i];
            d = &def->defs[i];

            active = 1;
            if (trig->flags == 0xFF) {
                if (trig->state & 1) {
                    if (!(d->defflags & 4))
                        active = 0;
                }
            } else {
                active = 0;
            }

            if (active) {
                switch (d->mode) {
                case 1:
                    if (d->parent >= 0) {
                        struct NuTriggerNode *node = &inst->nodes[d->parent];
                        active = 0;
                        if (node->flags & 1) {
                            if (node->ref == 0)
                                active = CheckParentedTriggerWithPos(
                                    d, node, pos, radius);
                            else
                                active = CheckParentedTriggerWithPos(
                                    d, node->ref, pos, radius);
                        }
                    } else {
                        active = CheckUnparentedTriggerWithPos(d, pos, radius);
                    }
                    break;
                case 0:
                    break;
                case 2:
                    active = 0;
                    break;
                }
            }

            if (active) {
                if (!(trig->state & 1)) {
                    NuSceneInstanceRunScript(sys->data, d->name);
                    trig->state |= 1;
                }
            } else {
                if (trig->state & 1) {
                    if (d->defflags & 4) {
                        sprintf(buf, D_00619F60, d->name);
                        NuSceneInstanceRunScript(sys->data, buf);
                        trig->state &= 0xFE;
                    }
                }
            }
        }
    }
}


void DebugRenderTriggers(void)
{
    struct NuRndrVtx line[2];
    NuTriggerSys *sys;
    NuTriggerDefHdr *hdr;
    NuTriggerDef *def;
    struct NuSceneInstData *inst;
    NuTrigger *trig;
    int i;
    int k;

    sys = g_NuTriggerSysList;
    if (g_DbgTriggerOff)
        return;

    line[1].col = -1;
    line[0].col = -1;

    for (; sys != 0; sys = sys->next) {
        inst = sys->data->inst;
        hdr = sys->def;
        if (sys->userflags & 1)
            continue;

        for (i = 0; i < hdr->count; i++) {
            def = &hdr->defs[i];
            trig = &sys->triggers[i];
            if (trig->state & 1)
                continue;
            if (trig->flags != 0xFF)
                continue;

            if (def->parent >= 0) {
                struct nuvec_s tmp;
                struct numtx_s inv;
                struct numtx_s *mtx = &inst->nodes[def->parent].mtx;

                if (g_DbgTriggerExtents) {
                    line[1].pos = mtx->pos;
                    line[0].pos = line[1].pos;
                    line[0].pos.x += def->radius;
                    line[0].pos.y += def->radius;
                    line[0].pos.z += def->radius;
                    line[1].pos.x -= def->radius;
                    line[1].pos.y += def->radius;
                    line[1].pos.z += def->radius;
                    NuRndrLine3d(line, 0, 0);

                    line[1].pos = mtx->pos;
                    line[0].pos = line[1].pos;
                    line[0].pos.x += def->radius;
                    line[0].pos.y += def->radius;
                    line[0].pos.z += def->radius;
                    line[1].pos.x += def->radius;
                    line[1].pos.y -= def->radius;
                    line[1].pos.z += def->radius;
                    NuRndrLine3d(line, 0, 0);

                    line[1].pos = mtx->pos;
                    line[0].pos = line[1].pos;
                    line[0].pos.x += def->radius;
                    line[0].pos.y += def->radius;
                    line[0].pos.z += def->radius;
                    line[1].pos.x += def->radius;
                    line[1].pos.y += def->radius;
                    line[1].pos.z -= def->radius;
                    NuRndrLine3d(line, 0, 0);

                    line[1].pos = mtx->pos;
                    line[0].pos = line[1].pos;
                    line[0].pos.x -= def->radius;
                    line[0].pos.y -= def->radius;
                    line[0].pos.z -= def->radius;
                    line[1].pos.x += def->radius;
                    line[1].pos.y -= def->radius;
                    line[1].pos.z -= def->radius;
                    NuRndrLine3d(line, 0, 0);

                    line[1].pos = mtx->pos;
                    line[0].pos = line[1].pos;
                    line[0].pos.x -= def->radius;
                    line[0].pos.y -= def->radius;
                    line[0].pos.z -= def->radius;
                    line[1].pos.x -= def->radius;
                    line[1].pos.y += def->radius;
                    line[1].pos.z -= def->radius;
                    NuRndrLine3d(line, 0, 0);

                    line[1].pos = mtx->pos;
                    line[0].pos = line[1].pos;
                    line[0].pos.x -= def->radius;
                    line[0].pos.y -= def->radius;
                    line[0].pos.z -= def->radius;
                    line[1].pos.x -= def->radius;
                    line[1].pos.y -= def->radius;
                    line[1].pos.z += def->radius;
                    NuRndrLine3d(line, 0, 0);
                }

                if (g_DbgTriggerShapes) {
                    for (k = 0; k < def->subcount; k++) {
                        NuTriggerSub *sub = &def->subs[k];

                        switch (sub->type) {
                        case 1: {
                            NuTriggerSphere *sph =
                                (NuTriggerSphere *)sub->ptr;

                            NuVecMtxTransform(&tmp, &sph->pos, mtx);

                            line[0].pos = tmp;
                            line[1].pos = tmp;
                            line[0].pos.x -= sph->radius;
                            line[1].pos.x += sph->radius;
                            NuRndrLine3d(line, 0, 0);

                            line[0].pos = tmp;
                            line[1].pos = tmp;
                            line[0].pos.y -= sph->radius;
                            line[1].pos.y += sph->radius;
                            NuRndrLine3d(line, 0, 0);

                            line[0].pos = tmp;
                            line[1].pos = tmp;
                            line[0].pos.z -= sph->radius;
                            line[1].pos.z += sph->radius;
                            NuRndrLine3d(line, 0, 0);
                        } break;
                        case 2: {
                            NuTriggerBox *box = (NuTriggerBox *)sub->ptr;

                            NuMtxInvH(&inv, &box->mtx);

                            line[0].pos.x = box->bbmin.x;
                            line[0].pos.y = box->bbmin.y;
                            line[0].pos.z = box->bbmin.z;
                            line[1].pos.x = box->bbmax.x;
                            line[1].pos.y = box->bbmin.y;
                            line[1].pos.z = box->bbmin.z;
                            NuVecMtxTransform(&line[0].pos, &line[0].pos, &inv);
                            NuVecMtxTransform(&line[0].pos, &line[0].pos, mtx);
                            NuVecMtxTransform(&line[1].pos, &line[1].pos, &inv);
                            NuVecMtxTransform(&line[1].pos, &line[1].pos, mtx);
                            NuRndrLine3d(line, 0, 0);

                            line[0].pos.x = box->bbmin.x;
                            line[0].pos.y = box->bbmin.y;
                            line[0].pos.z = box->bbmin.z;
                            line[1].pos.x = box->bbmin.x;
                            line[1].pos.y = box->bbmax.y;
                            line[1].pos.z = box->bbmin.z;
                            NuVecMtxTransform(&line[0].pos, &line[0].pos, &inv);
                            NuVecMtxTransform(&line[0].pos, &line[0].pos, mtx);
                            NuVecMtxTransform(&line[1].pos, &line[1].pos, &inv);
                            NuVecMtxTransform(&line[1].pos, &line[1].pos, mtx);
                            NuRndrLine3d(line, 0, 0);

                            line[0].pos.x = box->bbmin.x;
                            line[0].pos.y = box->bbmin.y;
                            line[0].pos.z = box->bbmin.z;
                            line[1].pos.x = box->bbmin.x;
                            line[1].pos.y = box->bbmin.y;
                            line[1].pos.z = box->bbmax.z;
                            NuVecMtxTransform(&line[0].pos, &line[0].pos, &inv);
                            NuVecMtxTransform(&line[0].pos, &line[0].pos, mtx);
                            NuVecMtxTransform(&line[1].pos, &line[1].pos, &inv);
                            NuVecMtxTransform(&line[1].pos, &line[1].pos, mtx);
                            NuRndrLine3d(line, 0, 0);

                            line[0].pos.x = box->bbmax.x;
                            line[0].pos.y = box->bbmax.y;
                            line[0].pos.z = box->bbmax.z;
                            line[1].pos.x = box->bbmin.x;
                            line[1].pos.y = box->bbmax.y;
                            line[1].pos.z = box->bbmax.z;
                            NuVecMtxTransform(&line[0].pos, &line[0].pos, &inv);
                            NuVecMtxTransform(&line[0].pos, &line[0].pos, mtx);
                            NuVecMtxTransform(&line[1].pos, &line[1].pos, &inv);
                            NuVecMtxTransform(&line[1].pos, &line[1].pos, mtx);
                            NuRndrLine3d(line, 0, 0);

                            line[0].pos.x = box->bbmax.x;
                            line[0].pos.y = box->bbmax.y;
                            line[0].pos.z = box->bbmax.z;
                            line[1].pos.x = box->bbmax.x;
                            line[1].pos.y = box->bbmin.y;
                            line[1].pos.z = box->bbmax.z;
                            NuVecMtxTransform(&line[0].pos, &line[0].pos, &inv);
                            NuVecMtxTransform(&line[0].pos, &line[0].pos, mtx);
                            NuVecMtxTransform(&line[1].pos, &line[1].pos, &inv);
                            NuVecMtxTransform(&line[1].pos, &line[1].pos, mtx);
                            NuRndrLine3d(line, 0, 0);

                            line[0].pos.x = box->bbmax.x;
                            line[0].pos.y = box->bbmax.y;
                            line[0].pos.z = box->bbmax.z;
                            line[1].pos.x = box->bbmax.x;
                            line[1].pos.y = box->bbmax.y;
                            line[1].pos.z = box->bbmin.z;
                            NuVecMtxTransform(&line[0].pos, &line[0].pos, &inv);
                            NuVecMtxTransform(&line[0].pos, &line[0].pos, mtx);
                            NuVecMtxTransform(&line[1].pos, &line[1].pos, &inv);
                            NuVecMtxTransform(&line[1].pos, &line[1].pos, mtx);
                            NuRndrLine3d(line, 0, 0);
                        } break;
                        case 3: {
                            NuTriggerSphere *cyl =
                                (NuTriggerSphere *)sub->ptr;

                            NuVecMtxTransform(&line[0].pos, &cyl->pos, mtx);
                            line[1].pos = line[0].pos;
                            line[1].pos.y += cyl->radius;
                            NuRndrLine3d(line, 0, 0);
                        } break;
                        case 0:
                            break;
                        default:
                            break;
                        }
                    }
                }
            } else {
                struct numtx_s inv;

                if (g_DbgTriggerExtents) {
                    line[0].pos.x = def->bbmin.x;
                    line[0].pos.y = def->bbmin.y;
                    line[0].pos.z = def->bbmin.z;
                    line[1].pos.x = def->bbmax.x;
                    line[1].pos.y = def->bbmin.y;
                    line[1].pos.z = def->bbmin.z;
                    NuRndrLine3d(line, 0, 0);

                    line[0].pos.x = def->bbmin.x;
                    line[0].pos.y = def->bbmin.y;
                    line[0].pos.z = def->bbmin.z;
                    line[1].pos.x = def->bbmin.x;
                    line[1].pos.y = def->bbmax.y;
                    line[1].pos.z = def->bbmin.z;
                    NuRndrLine3d(line, 0, 0);

                    line[0].pos.x = def->bbmin.x;
                    line[0].pos.y = def->bbmin.y;
                    line[0].pos.z = def->bbmin.z;
                    line[1].pos.x = def->bbmin.x;
                    line[1].pos.y = def->bbmin.y;
                    line[1].pos.z = def->bbmax.z;
                    NuRndrLine3d(line, 0, 0);

                    line[0].pos.x = def->bbmax.x;
                    line[0].pos.y = def->bbmax.y;
                    line[0].pos.z = def->bbmax.z;
                    line[1].pos.x = def->bbmin.x;
                    line[1].pos.y = def->bbmax.y;
                    line[1].pos.z = def->bbmax.z;
                    NuRndrLine3d(line, 0, 0);

                    line[0].pos.x = def->bbmax.x;
                    line[0].pos.y = def->bbmax.y;
                    line[0].pos.z = def->bbmax.z;
                    line[1].pos.x = def->bbmax.x;
                    line[1].pos.y = def->bbmin.y;
                    line[1].pos.z = def->bbmax.z;
                    NuRndrLine3d(line, 0, 0);

                    line[0].pos.x = def->bbmax.x;
                    line[0].pos.y = def->bbmax.y;
                    line[0].pos.z = def->bbmax.z;
                    line[1].pos.x = def->bbmax.x;
                    line[1].pos.y = def->bbmax.y;
                    line[1].pos.z = def->bbmin.z;
                    NuRndrLine3d(line, 0, 0);
                }

                if (g_DbgTriggerShapes) {
                    for (k = 0; k < def->subcount; k++) {
                        NuTriggerSub *sub = &def->subs[k];

                        switch (sub->type) {
                        case 1: {
                            NuTriggerSphere *sph =
                                (NuTriggerSphere *)sub->ptr;

                            line[0].pos = sph->pos;
                            line[1].pos = sph->pos;
                            line[0].pos.x -= sph->radius;
                            line[1].pos.x += sph->radius;
                            NuRndrLine3d(line, 0, 0);

                            line[0].pos = sph->pos;
                            line[1].pos = sph->pos;
                            line[0].pos.y -= sph->radius;
                            line[1].pos.y += sph->radius;
                            NuRndrLine3d(line, 0, 0);

                            line[0].pos = sph->pos;
                            line[1].pos = sph->pos;
                            line[0].pos.z -= sph->radius;
                            line[1].pos.z += sph->radius;
                            NuRndrLine3d(line, 0, 0);
                        } break;
                        case 2: {
                            NuTriggerBox *box = (NuTriggerBox *)sub->ptr;

                            NuMtxInvH(&inv, &box->mtx);

                            line[0].pos.x = box->bbmin.x;
                            line[0].pos.y = box->bbmin.y;
                            line[0].pos.z = box->bbmin.z;
                            line[1].pos.x = box->bbmax.x;
                            line[1].pos.y = box->bbmin.y;
                            line[1].pos.z = box->bbmin.z;
                            NuVecMtxTransform(&line[0].pos, &line[0].pos, &inv);
                            NuVecMtxTransform(&line[1].pos, &line[1].pos, &inv);
                            NuRndrLine3d(line, 0, 0);

                            line[0].pos.x = box->bbmin.x;
                            line[0].pos.y = box->bbmin.y;
                            line[0].pos.z = box->bbmin.z;
                            line[1].pos.x = box->bbmin.x;
                            line[1].pos.y = box->bbmax.y;
                            line[1].pos.z = box->bbmin.z;
                            NuVecMtxTransform(&line[0].pos, &line[0].pos, &inv);
                            NuVecMtxTransform(&line[1].pos, &line[1].pos, &inv);
                            NuRndrLine3d(line, 0, 0);

                            line[0].pos.x = box->bbmin.x;
                            line[0].pos.y = box->bbmin.y;
                            line[0].pos.z = box->bbmin.z;
                            line[1].pos.x = box->bbmin.x;
                            line[1].pos.y = box->bbmin.y;
                            line[1].pos.z = box->bbmax.z;
                            NuVecMtxTransform(&line[0].pos, &line[0].pos, &inv);
                            NuVecMtxTransform(&line[1].pos, &line[1].pos, &inv);
                            NuRndrLine3d(line, 0, 0);

                            line[0].pos.x = box->bbmax.x;
                            line[0].pos.y = box->bbmax.y;
                            line[0].pos.z = box->bbmax.z;
                            line[1].pos.x = box->bbmin.x;
                            line[1].pos.y = box->bbmax.y;
                            line[1].pos.z = box->bbmax.z;
                            NuVecMtxTransform(&line[0].pos, &line[0].pos, &inv);
                            NuVecMtxTransform(&line[1].pos, &line[1].pos, &inv);
                            NuRndrLine3d(line, 0, 0);

                            line[0].pos.x = box->bbmax.x;
                            line[0].pos.y = box->bbmax.y;
                            line[0].pos.z = box->bbmax.z;
                            line[1].pos.x = box->bbmax.x;
                            line[1].pos.y = box->bbmin.y;
                            line[1].pos.z = box->bbmax.z;
                            NuVecMtxTransform(&line[0].pos, &line[0].pos, &inv);
                            NuVecMtxTransform(&line[1].pos, &line[1].pos, &inv);
                            NuRndrLine3d(line, 0, 0);

                            line[0].pos.x = box->bbmax.x;
                            line[0].pos.y = box->bbmax.y;
                            line[0].pos.z = box->bbmax.z;
                            line[1].pos.x = box->bbmax.x;
                            line[1].pos.y = box->bbmax.y;
                            line[1].pos.z = box->bbmin.z;
                            NuVecMtxTransform(&line[0].pos, &line[0].pos, &inv);
                            NuVecMtxTransform(&line[1].pos, &line[1].pos, &inv);
                            NuRndrLine3d(line, 0, 0);
                        } break;
                        case 3: {
                            NuTriggerSphere *cyl =
                                (NuTriggerSphere *)sub->ptr;

                            line[0].pos = cyl->pos;
                            line[1].pos = line[0].pos;
                            line[1].pos.y += cyl->radius;
                            NuRndrLine3d(line, 0, 0);
                        } break;
                        case 0:
                            break;
                        default:
                            break;
                        }
                    }
                }
            }
        }
    }
}


void NuTriggerSysInit(void)
{
    g_NuTriggerSysList = 0;
}


void NuTriggerSysRender(void)
{
}


NuTriggerSys *instNuTriggerSysCreate(NuTriggerDefHdr *def, void *data, void **heap)
{
    NuTriggerSys *sys = 0;
    int i;

    if (def == 0)
        goto out;
    if (data == 0)
        goto out;
    if (*(int *)((char *)data + 0x10) == 0)
        goto out;

    sys = (NuTriggerSys *)(((unsigned int)*heap + 0xF) & ~0xFU);
    *heap = (char *)sys + 0x18;
    memset(sys, 0, 0x18);

    sys->prev = 0;
    sys->next = g_NuTriggerSysList;
    if (g_NuTriggerSysList)
        g_NuTriggerSysList->prev = sys;
    g_NuTriggerSysList = sys;

    sys->data = data;
    sys->def = def;
    sys->triggers = (NuTrigger *)*heap;
    *heap = (char *)*heap + def->count * 4;
    memset(sys->triggers, 0, def->count * 4);

    for (i = 0; i < def->count; i++)
        sys->triggers[i].flags = def->defs[i].resetflag;

out:
    return sys;
}


void instNuTriggerSysDestroy(NuTriggerSys *sys)
{
    if (sys->next)
        sys->next->prev = sys->prev;
    if (sys->prev)
        sys->prev->next = sys->next;
    else
        g_NuTriggerSysList = sys->next;
}


void instNuTriggerSetFlag(NuTriggerSys *sys, char *name, int index, int set)
{
    NuTriggerDefHdr *def = sys->def;
    int i;

    for (i = 0; i < def->count; i++) {
        if (strcasecmp(def->defs[i].name, name) == 0) {
            char bit = 1 << (index - 1);
            if (set)
                sys->triggers[i].flags |= bit;
            else
                sys->triggers[i].flags &= ~bit;
            break;
        }
    }
}


void instNuTriggerReset(NuTriggerSys *sys, char *name)
{
    NuTriggerDefHdr *def = sys->def;
    int i;

    for (i = 0; i < def->count; i++) {
        if (strcasecmp(def->defs[i].name, name) == 0) {
            sys->triggers[i].flags = def->defs[i].resetflag;
            sys->triggers[i].state &= 0xFE;
            break;
        }
    }
}


void instNuTriggerEnable(NuTriggerSys *sys)
{
    sys->userflags &= ~1;
}


void instNuTriggerDisable(NuTriggerSys *sys)
{
    sys->userflags |= 1;
}


void NuTriggerPull(NuTriggerSys *sys, NuTriggerDef *def, NuTrigger *trig, int pull)
{
    char buf[32];

    if (pull) {
        NuSceneInstanceRunScript(sys->data, def->name);
        trig->state |= 1;
    } else {
        sprintf(buf, D_00619F60, def->name);
        NuSceneInstanceRunScript(sys->data, buf);
        trig->state &= 0xFE;
    }
}
