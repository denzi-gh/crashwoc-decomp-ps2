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

#include "nu_libc.h"
#include "nu_types.h"

#include "nucore/nuerror.h"
#include "numath/nufloat.h"
#include "numath/numathtypes.h"
#include "numath/numtx.h"
#include "numath/nuvec.h"
#include "nu3d/nugobj.h"
#include "nu3d/nugscn.h"

struct numtx2_s
{
    f32 _00;
    f32 _01;
    f32 _02;
    f32 _03;
    f32 _10;
    f32 _11;
    f32 _12;
    f32 _13;
    f32 _20;
    f32 _21;
    f32 _22;
    f32 _23;
    f32 _30;
    f32 _31;
    f32 _32;
    f32 _33;
};

union variptr_u
{
    void *voidptr;
    signed char *s8;
    unsigned char *u8;
    short unsigned int *u16;
    short int *s16;
    unsigned int *u32;
    unsigned int *s32;
    unsigned long long *u64;
    float *f32;
    struct nuvec_s* vec3;
    void* vec4;
    void* ivec3;
    void* ivec4;
    struct numtx_s* mtx44;
    void* mtl;
    void *vutri;
    void *tristream;
    unsigned int* viftag;
    unsigned int intaddr;
    void* dmatag;
    void *giftag;
    void *zbuf;
    void *test;
    void *prmode;
    void *frame;
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
    NuTriggerDefHdr *def;      /* 0x08 NUTRIGGERSYS triggersys; -- the definition passed in a0 */
    NuTrigger *triggers;       /* 0x0C -- per-trigger array (def->count * 4) */
    struct NuSceneInst *data;  /* 0x10 void* gscene; -- opaque payload passed in a1 */
    int userflags;             /* 0x14 -- bit0 = disabled */
} NuTriggerSys;


extern int NuSceneInstanceRunScript(void *scene, void *script);
extern int NuFileLoadBuffer(void *name, void *buf, int size);

extern char D_00619EF8[];
extern char D_00619F20[];
extern char D_00619F60[];

void NuRndrLine3d(struct nuvtx_tc1_s *line, void *arg1, struct numtx_s *mtx);

extern NuTriggerSys *D_00633230;
#define g_NuTriggerSysList D_00633230

extern int D_0062F7F8;
#define g_DbgTriggerExtents D_0062F7F8

extern int D_0062F7FC;
#define g_DbgTriggerShapes D_0062F7FC

extern int D_0062F800;
#define g_DbgTriggerOff D_0062F800


enum NUTRIGGERPRIMTYPES_e {
    NUTRIGGERPRIMTYPE_CYLINDER = 3,
    NUTRIGGERPRIMTYPE_CUBE = 2,
    NUTRIGGERPRIMTYPE_SPHERE = 1,
    NUTRIGGERPRIMTYPE_NONE = 0,
};

struct NUTRIGGERPRIM_s {
    // total size: 0x8
    enum NUTRIGGERPRIMTYPES_e type; // offset 0x0, size 0x4
    void * data; // offset 0x4, size 0x4
};

enum NUTRIGGERTYPE_s {
    NUTRIGGER_PLAYER_WEAPON_CONTACT = 2,
    NUTRIGGER_PLAYER_CONTACT = 1,
    NUTRIGGER_AUTO = 0,
};

struct NUTRIGGERPRIM_CUBE_s { // 0x58
	/* 0x00 */ struct numtx2_s invmtx;
	/* 0x40 */ struct nuvec_s min;
	/* 0x4c */ struct nuvec_s max;
};

struct NUTRIGGERPRIM_CYLINDER_s { // 0x14
	/* 0x00 */ struct nuvec_s bottom;
	/* 0x0c */ float height;
	/* 0x10 */ float radius;
};

struct NUTRIGGERPRIM_SPHERE_s { // 0x10
	/* 0x0 */ struct nuvec_s centre;
	/* 0xc */ float radius;
};

struct NUTRIGGER_s {
    char * triggername;
    enum NUTRIGGERTYPE_s trigger_type;
    short hitpoints;
    char enableflags;
    char pad;
    int scale_transform : 1;
    int display_box : 1;
    int persistant : 1;
    float radius;
    struct nuvec_s min;
    struct nuvec_s max;
    short numprims;
    short instance_ix;
    struct NUTRIGGERPRIM_s * prims;
};

struct instNUTRIGGER_s {
    short hitpoints;
    unsigned char enableflags;
    char flags;
};

struct NUTRIGGERSYS_s {
    int version;
    int address_offset;
    int ntriggers;
    struct NUTRIGGER_s* triggers;
};

struct instNUTRIGGERSYS_s {
    struct instNUTRIGGERSYS_s * next;
    struct instNUTRIGGERSYS_s * prev;
    struct NUTRIGGERSYS_s * triggersys;
    struct instNUTRIGGER_s * itriggers;
    void* gscene;
    int is_disabled : 1;
};

int CheckParentedTriggerWithPos(struct NUTRIGGER_s* trigger, struct numtx2_s* mtx, struct nuvec_s* pos, float r);
int CheckUnparentedTriggerWithPos(struct NUTRIGGER_s* trigger, struct nuvec_s* pos, float r);


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
                                    (struct NUTRIGGER_s*)d, (struct numtx2_s*)&node->mtx, pos, radius);
                            else
                                active = CheckParentedTriggerWithPos(
                                    (struct NUTRIGGER_s*)d, (struct numtx2_s*)&node->ref->mtx, pos, radius);
                        }
                    } else {
                        active = CheckUnparentedTriggerWithPos((struct NUTRIGGER_s*)d, pos, radius);
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
    struct nuvtx_tc1_s line[2];
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

    line[1].diffuse = -1;
    line[0].diffuse = -1;

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
                    line[1].pnt = *(struct nuvec_s *)&mtx->_30;
                    line[0].pnt = line[1].pnt;
                    line[0].pnt.x += def->radius;
                    line[0].pnt.y += def->radius;
                    line[0].pnt.z += def->radius;
                    line[1].pnt.x -= def->radius;
                    line[1].pnt.y += def->radius;
                    line[1].pnt.z += def->radius;
                    NuRndrLine3d(line, 0, 0);

                    line[1].pnt = *(struct nuvec_s *)&mtx->_30;
                    line[0].pnt = line[1].pnt;
                    line[0].pnt.x += def->radius;
                    line[0].pnt.y += def->radius;
                    line[0].pnt.z += def->radius;
                    line[1].pnt.x += def->radius;
                    line[1].pnt.y -= def->radius;
                    line[1].pnt.z += def->radius;
                    NuRndrLine3d(line, 0, 0);

                    line[1].pnt = *(struct nuvec_s *)&mtx->_30;
                    line[0].pnt = line[1].pnt;
                    line[0].pnt.x += def->radius;
                    line[0].pnt.y += def->radius;
                    line[0].pnt.z += def->radius;
                    line[1].pnt.x += def->radius;
                    line[1].pnt.y += def->radius;
                    line[1].pnt.z -= def->radius;
                    NuRndrLine3d(line, 0, 0);

                    line[1].pnt = *(struct nuvec_s *)&mtx->_30;
                    line[0].pnt = line[1].pnt;
                    line[0].pnt.x -= def->radius;
                    line[0].pnt.y -= def->radius;
                    line[0].pnt.z -= def->radius;
                    line[1].pnt.x += def->radius;
                    line[1].pnt.y -= def->radius;
                    line[1].pnt.z -= def->radius;
                    NuRndrLine3d(line, 0, 0);

                    line[1].pnt = *(struct nuvec_s *)&mtx->_30;
                    line[0].pnt = line[1].pnt;
                    line[0].pnt.x -= def->radius;
                    line[0].pnt.y -= def->radius;
                    line[0].pnt.z -= def->radius;
                    line[1].pnt.x -= def->radius;
                    line[1].pnt.y += def->radius;
                    line[1].pnt.z -= def->radius;
                    NuRndrLine3d(line, 0, 0);

                    line[1].pnt = *(struct nuvec_s *)&mtx->_30;
                    line[0].pnt = line[1].pnt;
                    line[0].pnt.x -= def->radius;
                    line[0].pnt.y -= def->radius;
                    line[0].pnt.z -= def->radius;
                    line[1].pnt.x -= def->radius;
                    line[1].pnt.y -= def->radius;
                    line[1].pnt.z += def->radius;
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

                            line[0].pnt = tmp;
                            line[1].pnt = tmp;
                            line[0].pnt.x -= sph->radius;
                            line[1].pnt.x += sph->radius;
                            NuRndrLine3d(line, 0, 0);

                            line[0].pnt = tmp;
                            line[1].pnt = tmp;
                            line[0].pnt.y -= sph->radius;
                            line[1].pnt.y += sph->radius;
                            NuRndrLine3d(line, 0, 0);

                            line[0].pnt = tmp;
                            line[1].pnt = tmp;
                            line[0].pnt.z -= sph->radius;
                            line[1].pnt.z += sph->radius;
                            NuRndrLine3d(line, 0, 0);
                        } break;
                        case 2: {
                            NuTriggerBox *box = (NuTriggerBox *)sub->ptr;

                            NuMtxInvH(&inv, &box->mtx);

                            line[0].pnt.x = box->bbmin.x;
                            line[0].pnt.y = box->bbmin.y;
                            line[0].pnt.z = box->bbmin.z;
                            line[1].pnt.x = box->bbmax.x;
                            line[1].pnt.y = box->bbmin.y;
                            line[1].pnt.z = box->bbmin.z;
                            NuVecMtxTransform(&line[0].pnt, &line[0].pnt, &inv);
                            NuVecMtxTransform(&line[0].pnt, &line[0].pnt, mtx);
                            NuVecMtxTransform(&line[1].pnt, &line[1].pnt, &inv);
                            NuVecMtxTransform(&line[1].pnt, &line[1].pnt, mtx);
                            NuRndrLine3d(line, 0, 0);

                            line[0].pnt.x = box->bbmin.x;
                            line[0].pnt.y = box->bbmin.y;
                            line[0].pnt.z = box->bbmin.z;
                            line[1].pnt.x = box->bbmin.x;
                            line[1].pnt.y = box->bbmax.y;
                            line[1].pnt.z = box->bbmin.z;
                            NuVecMtxTransform(&line[0].pnt, &line[0].pnt, &inv);
                            NuVecMtxTransform(&line[0].pnt, &line[0].pnt, mtx);
                            NuVecMtxTransform(&line[1].pnt, &line[1].pnt, &inv);
                            NuVecMtxTransform(&line[1].pnt, &line[1].pnt, mtx);
                            NuRndrLine3d(line, 0, 0);

                            line[0].pnt.x = box->bbmin.x;
                            line[0].pnt.y = box->bbmin.y;
                            line[0].pnt.z = box->bbmin.z;
                            line[1].pnt.x = box->bbmin.x;
                            line[1].pnt.y = box->bbmin.y;
                            line[1].pnt.z = box->bbmax.z;
                            NuVecMtxTransform(&line[0].pnt, &line[0].pnt, &inv);
                            NuVecMtxTransform(&line[0].pnt, &line[0].pnt, mtx);
                            NuVecMtxTransform(&line[1].pnt, &line[1].pnt, &inv);
                            NuVecMtxTransform(&line[1].pnt, &line[1].pnt, mtx);
                            NuRndrLine3d(line, 0, 0);

                            line[0].pnt.x = box->bbmax.x;
                            line[0].pnt.y = box->bbmax.y;
                            line[0].pnt.z = box->bbmax.z;
                            line[1].pnt.x = box->bbmin.x;
                            line[1].pnt.y = box->bbmax.y;
                            line[1].pnt.z = box->bbmax.z;
                            NuVecMtxTransform(&line[0].pnt, &line[0].pnt, &inv);
                            NuVecMtxTransform(&line[0].pnt, &line[0].pnt, mtx);
                            NuVecMtxTransform(&line[1].pnt, &line[1].pnt, &inv);
                            NuVecMtxTransform(&line[1].pnt, &line[1].pnt, mtx);
                            NuRndrLine3d(line, 0, 0);

                            line[0].pnt.x = box->bbmax.x;
                            line[0].pnt.y = box->bbmax.y;
                            line[0].pnt.z = box->bbmax.z;
                            line[1].pnt.x = box->bbmax.x;
                            line[1].pnt.y = box->bbmin.y;
                            line[1].pnt.z = box->bbmax.z;
                            NuVecMtxTransform(&line[0].pnt, &line[0].pnt, &inv);
                            NuVecMtxTransform(&line[0].pnt, &line[0].pnt, mtx);
                            NuVecMtxTransform(&line[1].pnt, &line[1].pnt, &inv);
                            NuVecMtxTransform(&line[1].pnt, &line[1].pnt, mtx);
                            NuRndrLine3d(line, 0, 0);

                            line[0].pnt.x = box->bbmax.x;
                            line[0].pnt.y = box->bbmax.y;
                            line[0].pnt.z = box->bbmax.z;
                            line[1].pnt.x = box->bbmax.x;
                            line[1].pnt.y = box->bbmax.y;
                            line[1].pnt.z = box->bbmin.z;
                            NuVecMtxTransform(&line[0].pnt, &line[0].pnt, &inv);
                            NuVecMtxTransform(&line[0].pnt, &line[0].pnt, mtx);
                            NuVecMtxTransform(&line[1].pnt, &line[1].pnt, &inv);
                            NuVecMtxTransform(&line[1].pnt, &line[1].pnt, mtx);
                            NuRndrLine3d(line, 0, 0);
                        } break;
                        case 3: {
                            NuTriggerSphere *cyl =
                                (NuTriggerSphere *)sub->ptr;

                            NuVecMtxTransform(&line[0].pnt, &cyl->pos, mtx);
                            line[1].pnt = line[0].pnt;
                            line[1].pnt.y += cyl->radius;
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
                    line[0].pnt.x = def->bbmin.x;
                    line[0].pnt.y = def->bbmin.y;
                    line[0].pnt.z = def->bbmin.z;
                    line[1].pnt.x = def->bbmax.x;
                    line[1].pnt.y = def->bbmin.y;
                    line[1].pnt.z = def->bbmin.z;
                    NuRndrLine3d(line, 0, 0);

                    line[0].pnt.x = def->bbmin.x;
                    line[0].pnt.y = def->bbmin.y;
                    line[0].pnt.z = def->bbmin.z;
                    line[1].pnt.x = def->bbmin.x;
                    line[1].pnt.y = def->bbmax.y;
                    line[1].pnt.z = def->bbmin.z;
                    NuRndrLine3d(line, 0, 0);

                    line[0].pnt.x = def->bbmin.x;
                    line[0].pnt.y = def->bbmin.y;
                    line[0].pnt.z = def->bbmin.z;
                    line[1].pnt.x = def->bbmin.x;
                    line[1].pnt.y = def->bbmin.y;
                    line[1].pnt.z = def->bbmax.z;
                    NuRndrLine3d(line, 0, 0);

                    line[0].pnt.x = def->bbmax.x;
                    line[0].pnt.y = def->bbmax.y;
                    line[0].pnt.z = def->bbmax.z;
                    line[1].pnt.x = def->bbmin.x;
                    line[1].pnt.y = def->bbmax.y;
                    line[1].pnt.z = def->bbmax.z;
                    NuRndrLine3d(line, 0, 0);

                    line[0].pnt.x = def->bbmax.x;
                    line[0].pnt.y = def->bbmax.y;
                    line[0].pnt.z = def->bbmax.z;
                    line[1].pnt.x = def->bbmax.x;
                    line[1].pnt.y = def->bbmin.y;
                    line[1].pnt.z = def->bbmax.z;
                    NuRndrLine3d(line, 0, 0);

                    line[0].pnt.x = def->bbmax.x;
                    line[0].pnt.y = def->bbmax.y;
                    line[0].pnt.z = def->bbmax.z;
                    line[1].pnt.x = def->bbmax.x;
                    line[1].pnt.y = def->bbmax.y;
                    line[1].pnt.z = def->bbmin.z;
                    NuRndrLine3d(line, 0, 0);
                }

                if (g_DbgTriggerShapes) {
                    for (k = 0; k < def->subcount; k++) {
                        NuTriggerSub *sub = &def->subs[k];

                        switch (sub->type) {
                        case 1: {
                            NuTriggerSphere *sph =
                                (NuTriggerSphere *)sub->ptr;

                            line[0].pnt = sph->pos;
                            line[1].pnt = sph->pos;
                            line[0].pnt.x -= sph->radius;
                            line[1].pnt.x += sph->radius;
                            NuRndrLine3d(line, 0, 0);

                            line[0].pnt = sph->pos;
                            line[1].pnt = sph->pos;
                            line[0].pnt.y -= sph->radius;
                            line[1].pnt.y += sph->radius;
                            NuRndrLine3d(line, 0, 0);

                            line[0].pnt = sph->pos;
                            line[1].pnt = sph->pos;
                            line[0].pnt.z -= sph->radius;
                            line[1].pnt.z += sph->radius;
                            NuRndrLine3d(line, 0, 0);
                        } break;
                        case 2: {
                            NuTriggerBox *box = (NuTriggerBox *)sub->ptr;

                            NuMtxInvH(&inv, &box->mtx);

                            line[0].pnt.x = box->bbmin.x;
                            line[0].pnt.y = box->bbmin.y;
                            line[0].pnt.z = box->bbmin.z;
                            line[1].pnt.x = box->bbmax.x;
                            line[1].pnt.y = box->bbmin.y;
                            line[1].pnt.z = box->bbmin.z;
                            NuVecMtxTransform(&line[0].pnt, &line[0].pnt, &inv);
                            NuVecMtxTransform(&line[1].pnt, &line[1].pnt, &inv);
                            NuRndrLine3d(line, 0, 0);

                            line[0].pnt.x = box->bbmin.x;
                            line[0].pnt.y = box->bbmin.y;
                            line[0].pnt.z = box->bbmin.z;
                            line[1].pnt.x = box->bbmin.x;
                            line[1].pnt.y = box->bbmax.y;
                            line[1].pnt.z = box->bbmin.z;
                            NuVecMtxTransform(&line[0].pnt, &line[0].pnt, &inv);
                            NuVecMtxTransform(&line[1].pnt, &line[1].pnt, &inv);
                            NuRndrLine3d(line, 0, 0);

                            line[0].pnt.x = box->bbmin.x;
                            line[0].pnt.y = box->bbmin.y;
                            line[0].pnt.z = box->bbmin.z;
                            line[1].pnt.x = box->bbmin.x;
                            line[1].pnt.y = box->bbmin.y;
                            line[1].pnt.z = box->bbmax.z;
                            NuVecMtxTransform(&line[0].pnt, &line[0].pnt, &inv);
                            NuVecMtxTransform(&line[1].pnt, &line[1].pnt, &inv);
                            NuRndrLine3d(line, 0, 0);

                            line[0].pnt.x = box->bbmax.x;
                            line[0].pnt.y = box->bbmax.y;
                            line[0].pnt.z = box->bbmax.z;
                            line[1].pnt.x = box->bbmin.x;
                            line[1].pnt.y = box->bbmax.y;
                            line[1].pnt.z = box->bbmax.z;
                            NuVecMtxTransform(&line[0].pnt, &line[0].pnt, &inv);
                            NuVecMtxTransform(&line[1].pnt, &line[1].pnt, &inv);
                            NuRndrLine3d(line, 0, 0);

                            line[0].pnt.x = box->bbmax.x;
                            line[0].pnt.y = box->bbmax.y;
                            line[0].pnt.z = box->bbmax.z;
                            line[1].pnt.x = box->bbmax.x;
                            line[1].pnt.y = box->bbmin.y;
                            line[1].pnt.z = box->bbmax.z;
                            NuVecMtxTransform(&line[0].pnt, &line[0].pnt, &inv);
                            NuVecMtxTransform(&line[1].pnt, &line[1].pnt, &inv);
                            NuRndrLine3d(line, 0, 0);

                            line[0].pnt.x = box->bbmax.x;
                            line[0].pnt.y = box->bbmax.y;
                            line[0].pnt.z = box->bbmax.z;
                            line[1].pnt.x = box->bbmax.x;
                            line[1].pnt.y = box->bbmax.y;
                            line[1].pnt.z = box->bbmin.z;
                            NuVecMtxTransform(&line[0].pnt, &line[0].pnt, &inv);
                            NuVecMtxTransform(&line[1].pnt, &line[1].pnt, &inv);
                            NuRndrLine3d(line, 0, 0);
                        } break;
                        case 3: {
                            NuTriggerSphere *cyl =
                                (NuTriggerSphere *)sub->ptr;

                            line[0].pnt = cyl->pos;
                            line[1].pnt = line[0].pnt;
                            line[1].pnt.y += cyl->radius;
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


NuTriggerSys *instNuTriggerSysCreate(NuTriggerDefHdr *triggersys,void *gscene,union variptr_u *buff) {
    NuTriggerSys *itriggersys = 0;
    int i;

    if (triggersys != 0 &&
        gscene != 0 &&
        *(int *)((char *)gscene + 0x10) != 0) {

        buff->intaddr = (buff->intaddr + 0xF) & ~0xF;
        itriggersys = buff->voidptr;

        buff->voidptr = itriggersys + 1;
        memset(itriggersys, 0, 0x18);

        itriggersys->prev = 0;
        itriggersys->next = g_NuTriggerSysList;

        if (g_NuTriggerSysList)
            g_NuTriggerSysList->prev = itriggersys;

        g_NuTriggerSysList = itriggersys;

        itriggersys->data = gscene;
        itriggersys->def = triggersys;
        itriggersys->triggers = (NuTrigger *)buff->voidptr;

        buff->intaddr += triggersys->count * 4;
        memset(itriggersys->triggers, 0, triggersys->count * 4);

        for (i = 0; i < triggersys->count; i++)
            itriggersys->triggers[i].flags = triggersys->defs[i].resetflag;
    }

    return itriggersys;
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

int CheckParentedTriggerWithPos(struct NUTRIGGER_s* trigger, struct numtx2_s* mtx, struct nuvec_s* pos, float r) {
    int i;
    struct NUTRIGGERPRIM_s* prim;
    struct nuvec_s dp;
    struct nuvec_s vr;
    struct nuvec_s lpos;
    struct numtx2_s invmtx;
    float f;
    struct NUTRIGGERPRIM_SPHERE_s* sphere;
    struct nuvec_s centre;
    struct NUTRIGGERPRIM_CUBE_s* cube;
    struct nuvec_s lpos2;
    struct nuvec_s lvr;
    struct NUTRIGGERPRIM_CYLINDER_s* cylinder;
    struct nuvec_s bottom;

    dp.x = pos->x - mtx->_30;
    dp.y = pos->y - mtx->_31;
    dp.z = pos->z - mtx->_32;
    if (dp.x * dp.x + dp.y * dp.y + dp.z * dp.z > (trigger->radius + r) * (trigger->radius + r)) {
	return 0;
    }
        if ((trigger->scale_transform & 1U) != 0) {
            NuMtxInvH((struct numtx_s *)&invmtx, (struct numtx_s *)mtx);
            NuVecMtxTransform(&lpos, pos, (struct numtx_s *)&invmtx);
            vr.x = vr.y = vr.z = r;
            NuVecMtxTransform(&vr, &vr, (struct numtx_s *)&invmtx);
            NuVecSub(&vr, &vr, (struct nuvec_s*)&invmtx._30);
            vr.x = NuFabs(vr.x);
            vr.y = NuFabs(vr.y);
            vr.z = NuFabs(vr.z);
        }
        for (i = 0; i < trigger->numprims; i++) {
            prim = (struct NUTRIGGERPRIM_s*)(trigger->prims) + i;
            switch (prim->type) {
                case NUTRIGGERPRIMTYPE_SPHERE:
                    sphere = prim->data;
                    NuVecMtxTransform(&centre, &sphere->centre, (struct numtx_s *)mtx);
                    if (NuVecDistSqr(pos, &centre, &dp) < (sphere->radius + r) * (sphere->radius + r)) {
                        return 1;
                    }
                    break;
                case NUTRIGGERPRIMTYPE_NONE:
                    break;
                case NUTRIGGERPRIMTYPE_CUBE:
                    cube = prim->data;
                    NuVecMtxTransform(&lpos2, &lpos, (struct numtx_s *)&cube->invmtx);
                    NuVecMtxTransform(&lvr, &vr, (struct numtx_s *)&cube->invmtx);
                    NuVecSub(&lvr, &lvr, (struct nuvec_s*)&cube->invmtx._30);
                    lvr.x = NuFabs(lvr.x);
                    lvr.y = NuFabs(lvr.y);
                    lvr.z = NuFabs(lvr.z);
                    if ((cube->max.x < lpos2.x - lvr.x) || (cube->min.x > lpos2.x + lvr.x)
                        || (cube->max.y < lpos2.y - lvr.y) || (cube->min.y > lpos2.y + lvr.y)
                        || (cube->max.z < lpos2.z - lvr.z) || (cube->min.z > lpos2.z + lvr.z))
                    {
                        break;
                    }
                    return 1;
                    break;
                case NUTRIGGERPRIMTYPE_CYLINDER:
                    cylinder = prim->data;
                    NuVecMtxTransform(&bottom, &cylinder->bottom, (struct numtx_s *)mtx);
                    if ((bottom.y - r > pos->y) || (pos->y > bottom.y + cylinder->height + r)) {
                        break;
                    }

                    f = (pos->x - bottom.x) * (pos->x - bottom.x) + (pos->z - bottom.z) * (pos->z - bottom.z);
                    if (f < (cylinder->radius + r) * (cylinder->radius + r)) {
                        return 1;
                    }
                    break;
        }
    }
    return 0;
}

int CheckUnparentedTriggerWithPos(struct NUTRIGGER_s* trigger, struct nuvec_s* pos, float r) {
    int i;
    struct NUTRIGGERPRIM_s* prim;
    struct NUTRIGGERPRIM_SPHERE_s* sphere;
    struct nuvec_s dp;
    struct NUTRIGGERPRIM_CUBE_s* cube;
    struct nuvec_s lpos;
    float f;
    struct NUTRIGGERPRIM_CYLINDER_s* cylinder;

    if ((((trigger->max.x < pos->x - r) || (pos->x + r < trigger->min.x)) || (trigger->max.y < pos->y - r))
        || (((pos->y + r < trigger->min.y || (trigger->max.z < pos->z - r)) || ((pos->z + r < trigger->min.z)))))
    {
        return 0;
    }
    for (i = 0; i < trigger->numprims; i++) {
        prim = trigger->prims + i;
        switch (prim->type) {
            case NUTRIGGERPRIMTYPE_SPHERE:
                sphere = prim->data;
                if (NuVecDistSqr(pos, &sphere->centre, &dp) < (sphere->radius + r) * (sphere->radius + r)) {
                    return 1;
                }
                break;
            case NUTRIGGERPRIMTYPE_CUBE:
                cube = prim->data;
                NuVecMtxTransform(&lpos, pos, (struct numtx_s *)&cube->invmtx);
                if ((cube->max.x < lpos.x - r) || (cube->min.x > lpos.x + r) || (cube->max.y < lpos.y - r)
                    || (cube->min.y > lpos.y + r) || (cube->max.z < lpos.z - r) || (cube->min.z > lpos.z + r))
                {
                    break;
                }
                return 1;
            case NUTRIGGERPRIMTYPE_CYLINDER:
                cylinder = prim->data;
                if ((cylinder->bottom.y - r > pos->y) || (pos->y > cylinder->bottom.y + cylinder->height + r)) {
                    break;
                }

                f = (pos->x - cylinder->bottom.x) * (pos->x - cylinder->bottom.x)
                    + (pos->z - cylinder->bottom.z) * (pos->z - cylinder->bottom.z);
                if (f < (cylinder->radius + r) * (cylinder->radius + r)) {
                    return 1;
                }
                break;
            case NUTRIGGERPRIMTYPE_NONE:
                break;
        }
    }
    return 0;
}

int WeaponCheckTriggers(struct nuvec_s* pos, float r, int hitpoints) {
    struct instNUTRIGGERSYS_s* itriggersys;
    int i;
    struct NUTRIGGERSYS_s* triggersys;
    struct nugscn_s* gscene;
    struct NUTRIGGER_s* trigger;
    struct instNUTRIGGER_s* itrigger;
    struct nuinstance_s* instance;
    char script[32];
    int check;

    for(itriggersys = g_NuTriggerSysList; itriggersys != 0; itriggersys = itriggersys->next) {
        gscene = *(struct nugscn_s**)(itriggersys->gscene + 0x10);
        triggersys = itriggersys->triggersys;
        if ((itriggersys->is_disabled & 1) == 0) {
            for (i = 0; i < triggersys->ntriggers; i++) {
                trigger = triggersys->triggers + i;
                itrigger = itriggersys->itriggers + i;
                //script[i] = trigger->triggername + i * 0xd;
                if (((itrigger->flags & 1U) == 0) && ((itrigger->enableflags == 0xff) && (trigger->trigger_type > 1))) {
                    if (trigger->trigger_type == NUTRIGGER_PLAYER_WEAPON_CONTACT) {
                        if (trigger->instance_ix >= 0) {
                            instance = gscene->instances + trigger->instance_ix;
                            if ((instance->flags.visible & 1) == 0) {
                                continue;
                            }
                            if (instance->anim != 0) {
                                check = CheckParentedTriggerWithPos(trigger, (struct numtx2_s *)&instance->anim->mtx, pos, r);
                            } else {
                                check = CheckParentedTriggerWithPos(trigger, (struct numtx2_s *)&instance->mtx, pos, r);
                            }
                        } else {
                            check = CheckUnparentedTriggerWithPos(trigger, pos, r);
                        }
                        if (check != 0) {
                            itrigger->hitpoints += hitpoints;
                            if (trigger->hitpoints <= itrigger->hitpoints) {
                                NuSceneInstanceRunScript(itriggersys->gscene, trigger->triggername);
                                itrigger->flags = itrigger->flags | 1;
                            }
                            return 1;
                        }
                    }
                }
            }
        }
    }
    return 0;
}
