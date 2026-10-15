/*
 * Unit: gamelib/specterr
 *
 * Functions:
 *   0x001a8850 CreateSpecialTerrainData
 *   0x001a8a38 SpecialTerrainInstanceCollideCheck
 *   0x001a9020 SpecialTerrainTestRender
 *   0x001a99e8 SpecialTerrainCollideCheck
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

typedef struct SpecTerrShape {
    u32 type;
    s32 id;
} SpecTerrShape;

typedef struct SpecTerrSphere {
    struct nuvec_s pos;
    f32 radius;
} SpecTerrSphere;

typedef struct SpecTerrBox {
    struct numtx_s mtx;
    struct nuvec_s min;
    struct nuvec_s max;
} SpecTerrBox;

typedef struct SpecTerrCylinder {
    struct nuvec_s pos;
    f32 height;
    f32 radius;
} SpecTerrCylinder;

typedef struct SpecTerrObject {
    s32 instance;
    short nshapes;
    u16 flags;
    f32 radius;
    s32 size;
} SpecTerrObject;

typedef struct SpecTerrData {
    s32 *crashdata;
    struct nugscn_s *scene;
    SpecTerrObject **objects;
} SpecTerrData;

extern void *CrashDataPtr(void);
extern void NuRndrLine3d(struct nuvtx_tc1_s *line, void *mtl, void *mtx);

extern char D_00619AA0[];
extern char D_00619AC8[];
extern char D_00619B20[];
extern s32 D_0062F764;
extern s32 D_0062F768;

#define specterr_draw_bounds D_0062F764
#define specterr_draw_index D_0062F768


SpecTerrData *CreateSpecialTerrainData(s32 level, struct nugscn_s *scene, char **buffer,
                                       char *end) {
    SpecTerrData *data;
    SpecTerrObject *obj;
    SpecTerrShape *shape;
    char *p;
    s32 count;
    s32 i;
    s32 j;

    data = 0;
    if (scene && scene->numinstance) {
        p = CrashDataPtr();
        if (p) {
            data = (SpecTerrData *)*buffer;
            *buffer += sizeof(SpecTerrData);
            memset(data, 0, sizeof(SpecTerrData));
            data->crashdata = (s32 *)p;
            data->scene = scene;
            data->objects = (SpecTerrObject **)*buffer;
            *buffer += scene->numinstance * sizeof(SpecTerrObject *);
            memset(data->objects, 0, scene->numinstance * sizeof(SpecTerrObject *));
            count = *(s32 *)p;
            p += sizeof(s32);
            for (i = 0; i < count; i++) {
                obj = (SpecTerrObject *)p;
                if (obj->instance >= scene->numinstance) {
                    NuErrorProlog(D_00619AA0, 0x52)(D_00619AC8);
                }
                data->objects[obj->instance] = obj;
                p += sizeof(SpecTerrObject);
                for (j = 0; j < obj->nshapes; j++) {
                    shape = (SpecTerrShape *)p;
                    p += sizeof(SpecTerrShape);
                    switch (shape->type) {
                    case 0:
                        NuErrorProlog(D_00619AA0, 0x61)(D_00619B20, shape->type);
                        break;
                    case 1:
                        p += sizeof(SpecTerrSphere);
                        break;
                    case 2:
                        p += sizeof(SpecTerrBox);
                        break;
                    case 3:
                        p += sizeof(SpecTerrCylinder);
                        break;
                    }
                }
            }
        }
    }
    return data;
}


s32 SpecialTerrainInstanceCollideCheck(struct nuvec_s *pos, f32 radius, SpecTerrObject *obj,
                                       struct numtx_s *mtx, struct nuvec_s *normal, s32 *hitid) {
    struct nuvec_s delta;
    struct nuvec_s localpos;
    struct numtx_s invmtx;
    struct nuvec_s extent;
    struct nuvec_s centre;
    struct nuvec_s boxpos;
    struct nuvec_s boxextent;
    struct numtx_s boxinv;
    struct nuvec_s boxcentre;
    struct nuvec_s cylpos;
    SpecTerrShape *shape;
    SpecTerrSphere *sphere;
    SpecTerrBox *box;
    SpecTerrCylinder *cyl;
    SpecTerrShape *next;
    s32 i;
    f32 dx;
    f32 dz;

    delta.x = pos->x - mtx->_30;
    delta.y = pos->y - mtx->_31;
    delta.z = pos->z - mtx->_32;
    if (delta.x * delta.x + delta.y * delta.y + delta.z * delta.z
        > (obj->radius + radius) * (obj->radius + radius)) {
        return 0;
    }

    if (obj->flags & 1) {
        NuMtxInvH(&invmtx, mtx);
        NuVecMtxTransform(&localpos, pos, &invmtx);
        extent.x = extent.y = extent.z = radius;
        NuVecMtxTransform(&extent, &extent, &invmtx);
        NuVecSub(&extent, &extent, (struct nuvec_s *)&invmtx._30);
        extent.x = NuFabs(extent.x);
        extent.y = NuFabs(extent.y);
        extent.z = NuFabs(extent.z);
    }

    next = (SpecTerrShape *)(obj + 1);
    for (i = 0; i < obj->nshapes; i++) {
        shape = next;
        switch (shape->type) {
        case 0:
            NuErrorProlog(D_00619AA0, 0xA2)(D_00619B20, shape->type);
            break;
        case 1:
            sphere = (SpecTerrSphere *)(shape + 1);
            next = (SpecTerrShape *)(sphere + 1);
            NuVecMtxTransform(&centre, &sphere->pos, mtx);
            if (NuVecDistSqr(pos, &centre, &delta)
                < (sphere->radius + radius) * (sphere->radius + radius)) {
                if (normal) {
                    NuVecNorm(normal, &delta);
                }
                if (hitid) {
                    *hitid = shape->id;
                }
                return 1;
            }
            break;
        case 2:
            box = (SpecTerrBox *)(shape + 1);
            next = (SpecTerrShape *)(box + 1);
            NuVecMtxTransform(&boxpos, &localpos, &box->mtx);
            NuVecMtxTransform(&boxextent, &extent, &box->mtx);
            NuVecSub(&boxextent, &boxextent, (struct nuvec_s *)&box->mtx._30);
            boxextent.x = NuFabs(boxextent.x);
            boxextent.y = NuFabs(boxextent.y);
            boxextent.z = NuFabs(boxextent.z);
            if (box->max.x < boxpos.x - boxextent.x || boxpos.x + boxextent.x < box->min.x
                || box->max.y < boxpos.y - boxextent.y || boxpos.y + boxextent.y < box->min.y
                || box->max.z < boxpos.z - boxextent.z || boxpos.z + boxextent.z < box->min.z) {
                break;
            }
            if (normal) {
                normal->x = 0.0f;
                normal->y = 0.0f;
                normal->z = 0.0f;
                if (box->max.x < boxpos.x + boxextent.x) {
                    normal->x = 1.0f;
                }
                if (boxpos.x - boxextent.x < box->min.x) {
                    normal->x -= 1.0f;
                }
                if (box->max.y < boxpos.y + boxextent.y) {
                    normal->y = 1.0f;
                }
                if (boxpos.y - boxextent.y < box->min.y) {
                    normal->y -= 1.0f;
                }
                if (box->max.z < boxpos.z + boxextent.z) {
                    normal->z = 1.0f;
                }
                if (boxpos.z - boxextent.z < box->min.z) {
                    normal->z -= 1.0f;
                }
                if (normal->x == 0.0f && normal->y == 0.0f && normal->z == 0.0f) {
                    NuVecAdd(&boxcentre, &box->max, &box->min);
                    NuVecScale(&boxcentre, &boxcentre, 0.5f);
                    NuVecSub(normal, &boxpos, &boxcentre);
                    NuVecNorm(normal, normal);
                }
                NuMtxInvH(&boxinv, &box->mtx);
                NuVecMtxRotate(normal, normal, &boxinv);
                NuVecMtxRotate(normal, normal, mtx);
            }
            if (hitid) {
                *hitid = shape->id;
            }
            return 2;
        case 3:
            cyl = (SpecTerrCylinder *)(shape + 1);
            next = (SpecTerrShape *)(cyl + 1);
            NuVecMtxTransform(&cylpos, &cyl->pos, mtx);
            if (pos->y < cylpos.y - radius) {
                break;
            }
            if (cylpos.y + cyl->height + radius < pos->y) {
                break;
            }
            dx = pos->x - cylpos.x;
            dz = pos->z - cylpos.z;
            if (dx * dx + dz * dz < (cyl->radius + radius) * (cyl->radius + radius)) {
                if (normal) {
                    normal->x = dx;
                    normal->y = 0.0f;
                    normal->z = pos->z - cylpos.z;
                    NuVecNorm(normal, normal);
                }
                if (hitid) {
                    *hitid = shape->id;
                }
                return 1;
            }
            break;
        }
    }
    return 0;
}


void SpecialTerrainTestRender(SpecTerrData *data) {
    struct nuvtx_tc1_s line[2];
    struct nuvec_s centre;
    struct numtx_s invmtx;
    struct nuinstance_s *inst;
    SpecTerrObject *obj;
    SpecTerrShape *shape;
    SpecTerrSphere *sphere;
    SpecTerrBox *box;
    SpecTerrCylinder *cyl;
    char *cur;
    SpecTerrShape *next;
    s32 count;
    s32 draw;
    s32 i;
    s32 j;

    if (data == 0) {
        return;
    }
    cur = (char *)data->crashdata;
    line[1].diffuse = -1;
    line[0].diffuse = -1;
    count = *(s32 *)cur;
    cur += sizeof(s32);
    for (i = 0; i < count; i++) {
        obj = (SpecTerrObject *)cur;
        inst = &data->scene->instances[obj->instance];
        if (specterr_draw_index == i || specterr_draw_index == -1) {
            draw = 1;
        } else {
            draw = 0;
        }
        if (draw && specterr_draw_bounds) {
            line[0].pnt = line[1].pnt = *(struct nuvec_s *)&inst->mtx._30;
            line[0].pnt.x += obj->radius;
            line[0].pnt.y += obj->radius;
            line[0].pnt.z += obj->radius;
            line[1].pnt.x -= obj->radius;
            line[1].pnt.y += obj->radius;
            line[1].pnt.z += obj->radius;
            NuRndrLine3d(line, 0, 0);

            line[0].pnt = line[1].pnt = *(struct nuvec_s *)&inst->mtx._30;
            line[0].pnt.x += obj->radius;
            line[0].pnt.y += obj->radius;
            line[0].pnt.z += obj->radius;
            line[1].pnt.x += obj->radius;
            line[1].pnt.y -= obj->radius;
            line[1].pnt.z += obj->radius;
            NuRndrLine3d(line, 0, 0);

            line[0].pnt = line[1].pnt = *(struct nuvec_s *)&inst->mtx._30;
            line[0].pnt.x += obj->radius;
            line[0].pnt.y += obj->radius;
            line[0].pnt.z += obj->radius;
            line[1].pnt.x += obj->radius;
            line[1].pnt.y += obj->radius;
            line[1].pnt.z -= obj->radius;
            NuRndrLine3d(line, 0, 0);

            line[0].pnt = line[1].pnt = *(struct nuvec_s *)&inst->mtx._30;
            line[0].pnt.x -= obj->radius;
            line[0].pnt.y -= obj->radius;
            line[0].pnt.z -= obj->radius;
            line[1].pnt.x += obj->radius;
            line[1].pnt.y -= obj->radius;
            line[1].pnt.z -= obj->radius;
            NuRndrLine3d(line, 0, 0);

            line[0].pnt = line[1].pnt = *(struct nuvec_s *)&inst->mtx._30;
            line[0].pnt.x -= obj->radius;
            line[0].pnt.y -= obj->radius;
            line[0].pnt.z -= obj->radius;
            line[1].pnt.x -= obj->radius;
            line[1].pnt.y += obj->radius;
            line[1].pnt.z -= obj->radius;
            NuRndrLine3d(line, 0, 0);

            line[0].pnt = line[1].pnt = *(struct nuvec_s *)&inst->mtx._30;
            line[0].pnt.x -= obj->radius;
            line[0].pnt.y -= obj->radius;
            line[0].pnt.z -= obj->radius;
            line[1].pnt.x -= obj->radius;
            line[1].pnt.y -= obj->radius;
            line[1].pnt.z += obj->radius;
            NuRndrLine3d(line, 0, 0);
        }

        next = (SpecTerrShape *)(obj + 1);
        for (j = 0; j < obj->nshapes; j++) {
            shape = next;
            switch (shape->type) {
            case 0:
                NuErrorProlog(D_00619AA0, 0x1A7)(D_00619B20, shape->type);
                break;
            case 1:
                sphere = (SpecTerrSphere *)(shape + 1);
                next = (SpecTerrShape *)(sphere + 1);
                if (draw) {
                    NuVecMtxTransform(&centre, &sphere->pos, &inst->mtx);
                    line[0].pnt = centre;
                    line[1].pnt = centre;
                    line[0].pnt.x -= sphere->radius;
                    line[1].pnt.x += sphere->radius;
                    NuRndrLine3d(line, 0, 0);
                    line[0].pnt = centre;
                    line[1].pnt = centre;
                    line[0].pnt.y -= sphere->radius;
                    line[1].pnt.y += sphere->radius;
                    NuRndrLine3d(line, 0, 0);
                    line[0].pnt = centre;
                    line[1].pnt = centre;
                    line[0].pnt.z -= sphere->radius;
                    line[1].pnt.z += sphere->radius;
                    NuRndrLine3d(line, 0, 0);
                }
                break;
            case 2:
                box = (SpecTerrBox *)(shape + 1);
                next = (SpecTerrShape *)(box + 1);
                if (draw) {
                    NuMtxInvH(&invmtx, &box->mtx);

                    line[0].pnt.x = box->min.x;
                    line[0].pnt.y = box->min.y;
                    line[0].pnt.z = box->min.z;
                    line[1].pnt.x = box->max.x;
                    line[1].pnt.y = box->min.y;
                    line[1].pnt.z = box->min.z;
                    NuVecMtxTransform(&line[0].pnt, &line[0].pnt, &invmtx);
                    NuVecMtxTransform(&line[0].pnt, &line[0].pnt, &inst->mtx);
                    NuVecMtxTransform(&line[1].pnt, &line[1].pnt, &invmtx);
                    NuVecMtxTransform(&line[1].pnt, &line[1].pnt, &inst->mtx);
                    NuRndrLine3d(line, 0, 0);

                    line[0].pnt.x = box->min.x;
                    line[0].pnt.y = box->min.y;
                    line[0].pnt.z = box->min.z;
                    line[1].pnt.x = box->min.x;
                    line[1].pnt.y = box->max.y;
                    line[1].pnt.z = box->min.z;
                    NuVecMtxTransform(&line[0].pnt, &line[0].pnt, &invmtx);
                    NuVecMtxTransform(&line[0].pnt, &line[0].pnt, &inst->mtx);
                    NuVecMtxTransform(&line[1].pnt, &line[1].pnt, &invmtx);
                    NuVecMtxTransform(&line[1].pnt, &line[1].pnt, &inst->mtx);
                    NuRndrLine3d(line, 0, 0);

                    line[0].pnt.x = box->min.x;
                    line[0].pnt.y = box->min.y;
                    line[0].pnt.z = box->min.z;
                    line[1].pnt.x = box->min.x;
                    line[1].pnt.y = box->min.y;
                    line[1].pnt.z = box->max.z;
                    NuVecMtxTransform(&line[0].pnt, &line[0].pnt, &invmtx);
                    NuVecMtxTransform(&line[0].pnt, &line[0].pnt, &inst->mtx);
                    NuVecMtxTransform(&line[1].pnt, &line[1].pnt, &invmtx);
                    NuVecMtxTransform(&line[1].pnt, &line[1].pnt, &inst->mtx);
                    NuRndrLine3d(line, 0, 0);

                    line[0].pnt.x = box->max.x;
                    line[0].pnt.y = box->max.y;
                    line[0].pnt.z = box->max.z;
                    line[1].pnt.x = box->min.x;
                    line[1].pnt.y = box->max.y;
                    line[1].pnt.z = box->max.z;
                    NuVecMtxTransform(&line[0].pnt, &line[0].pnt, &invmtx);
                    NuVecMtxTransform(&line[0].pnt, &line[0].pnt, &inst->mtx);
                    NuVecMtxTransform(&line[1].pnt, &line[1].pnt, &invmtx);
                    NuVecMtxTransform(&line[1].pnt, &line[1].pnt, &inst->mtx);
                    NuRndrLine3d(line, 0, 0);

                    line[0].pnt.x = box->max.x;
                    line[0].pnt.y = box->max.y;
                    line[0].pnt.z = box->max.z;
                    line[1].pnt.x = box->max.x;
                    line[1].pnt.y = box->min.y;
                    line[1].pnt.z = box->max.z;
                    NuVecMtxTransform(&line[0].pnt, &line[0].pnt, &invmtx);
                    NuVecMtxTransform(&line[0].pnt, &line[0].pnt, &inst->mtx);
                    NuVecMtxTransform(&line[1].pnt, &line[1].pnt, &invmtx);
                    NuVecMtxTransform(&line[1].pnt, &line[1].pnt, &inst->mtx);
                    NuRndrLine3d(line, 0, 0);

                    line[0].pnt.x = box->max.x;
                    line[0].pnt.y = box->max.y;
                    line[0].pnt.z = box->max.z;
                    line[1].pnt.x = box->max.x;
                    line[1].pnt.y = box->max.y;
                    line[1].pnt.z = box->min.z;
                    NuVecMtxTransform(&line[0].pnt, &line[0].pnt, &invmtx);
                    NuVecMtxTransform(&line[0].pnt, &line[0].pnt, &inst->mtx);
                    NuVecMtxTransform(&line[1].pnt, &line[1].pnt, &invmtx);
                    NuVecMtxTransform(&line[1].pnt, &line[1].pnt, &inst->mtx);
                    NuRndrLine3d(line, 0, 0);
                }
                break;
            case 3:
                cyl = (SpecTerrCylinder *)(shape + 1);
                next = (SpecTerrShape *)(cyl + 1);
                if (draw) {
                    NuVecMtxTransform(&line[0].pnt, &cyl->pos, &inst->mtx);
                    line[1].pnt = line[0].pnt;
                    line[1].pnt.y += cyl->height;
                    NuRndrLine3d(line, 0, 0);
                }
                break;
            }
        }
        cur += obj->size;
    }
}


s32 SpecialTerrainCollideCheck(struct nuvec_s *pos, SpecTerrData *data, struct nuvec_s *normal,
                               s32 *hitid, f32 radius) {
    SpecTerrObject *obj;
    struct nuinstance_s *inst;
    char *p;
    s32 count;
    s32 i;
    s32 hit;

    if (data) {
        p = (char *)data->crashdata;
        count = *(s32 *)p;
        p += sizeof(s32);
        for (i = 0; i < count; i++) {
            obj = (SpecTerrObject *)p;
            inst = &data->scene->instances[obj->instance];
            if ((inst->flagword & 5) == 5) {
                if (inst->anim) {
                    hit = SpecialTerrainInstanceCollideCheck(pos, radius, obj, &inst->anim->mtx,
                                                             normal, hitid);
                } else {
                    hit = SpecialTerrainInstanceCollideCheck(pos, radius, obj, &inst->mtx, normal,
                                                             hitid);
                }
                if (hit) {
                    return hit;
                }
            }
            p += obj->size;
        }
    }
    return 0;
}
