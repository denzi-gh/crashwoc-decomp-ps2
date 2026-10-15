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
#include "gamelib/sceneman.h"
#include "gamelib/trigger.h"
#include "nucore/nucoretypes.h"

extern int NuFileLoadBuffer(void *name, void *buf, int size);

extern char D_00619EF8[];
extern char D_00619F20[];
extern char D_00619F60[];

void NuRndrLine3d(struct nuvtx_tc1_s *line, void *arg1, struct numtx_s *mtx);

extern struct instNUTRIGGERSYS_s *D_00633230;
#define g_NuTriggerSysList D_00633230

extern int D_0062F7F8;
#define g_DbgTriggerExtents D_0062F7F8

extern int D_0062F7FC;
#define g_DbgTriggerShapes D_0062F7FC

extern int D_0062F800;
#define g_DbgTriggerOff D_0062F800


struct NUTRIGGERSYS_s *NuTriggerSysLoad(char *name, union variptr_u *heap, union variptr_u *heapend)
{
    struct NUTRIGGERSYS_s *blob;
    int size;
    int i;

    heap->intaddr = (heap->intaddr + 0xF) & ~0xF;
    size = NuFileLoadBuffer(name, heap->voidptr, heapend->intaddr - heap->intaddr);
    if (size == 0)
        return 0;

    blob = heap->voidptr;
    heap->u8 = heap->u8 + size;

    if (blob->version != 1)
        NuErrorProlog(D_00619EF8, 0x87)(D_00619F20, name, blob->version);

    blob->address_offset = (int)blob - blob->address_offset;
    blob->triggers = blob->triggers ? (struct NUTRIGGER_s *)((char *)blob->triggers + blob->address_offset)
                                    : (struct NUTRIGGER_s *)0;
    if (blob->triggers == 0)
        return blob;

    for (i = 0; i < blob->ntriggers; i++) {
        struct NUTRIGGER_s *d = &blob->triggers[i];
        int k;

        d->triggername = d->triggername ? (char *)(d->triggername + blob->address_offset) : (char *)0;
        d->prims = d->prims ? (struct NUTRIGGERPRIM_s *)((char *)d->prims + blob->address_offset)
                            : (struct NUTRIGGERPRIM_s *)0;
        if (d->prims == 0)
            continue;

        for (k = 0; k < d->numprims; k++) {
            struct NUTRIGGERPRIM_s *e = &d->prims[k];
            e->data = e->data ? (void *)((char *)e->data + blob->address_offset) : (void *)0;
        }
    }

    return blob;
}


void NuTriggerSysUpdate(struct nuvec_s *pos, f32 radius)
{
    struct instNUTRIGGERSYS_s *sys;
    struct NUTRIGGERSYS_s *def;
    struct nugscn_s *gscene;
    struct NUTRIGGER_s *d;
    struct instNUTRIGGER_s *trig;
    int i;
    int active;
    char buf[32];

    for (sys = g_NuTriggerSysList; sys != 0; sys = sys->next) {
        gscene = sys->scenemanager->gscene;
        def = sys->triggersys;
        if (sys->is_disabled)
            continue;
        for (i = 0; i < def->ntriggers; i++) {
            trig = &sys->itriggers[i];
            d = &def->triggers[i];

            active = 1;
            if (trig->enableflags == 0xFF) {
                if (trig->flags & 1) {
                    if (!d->persistant)
                        active = 0;
                }
            } else {
                active = 0;
            }

            if (active) {
                switch (d->trigger_type) {
                case 1:
                    if (d->instance_ix >= 0) {
                        struct nuinstance_s *instance = &gscene->instances[d->instance_ix];
                        active = 0;
                        if (instance->flags.visible) {
                            if (instance->anim == 0)
                                active = CheckParentedTriggerWithPos(
                                    d, &instance->mtx, pos, radius);
                            else
                                active = CheckParentedTriggerWithPos(
                                    d, &instance->anim->mtx, pos, radius);
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
                if (!(trig->flags & 1)) {
                    NuSceneInstanceRunScript(sys->scenemanager, d->triggername);
                    trig->flags |= 1;
                }
            } else {
                if (trig->flags & 1) {
                    if (d->persistant) {
                        sprintf(buf, D_00619F60, d->triggername);
                        NuSceneInstanceRunScript(sys->scenemanager, buf);
                        trig->flags &= 0xFE;
                    }
                }
            }
        }
    }
}


void DebugRenderTriggers(void)
{
    struct nuvtx_tc1_s line[2];
    struct instNUTRIGGERSYS_s *sys;
    struct NUTRIGGERSYS_s *hdr;
    struct NUTRIGGER_s *def;
    struct nugscn_s *gscene;
    struct instNUTRIGGER_s *trig;
    int i;
    int k;

    sys = g_NuTriggerSysList;
    if (g_DbgTriggerOff)
        return;

    line[1].diffuse = -1;
    line[0].diffuse = -1;

    for (; sys != 0; sys = sys->next) {
        gscene = sys->scenemanager->gscene;
        hdr = sys->triggersys;
        if (sys->is_disabled)
            continue;

        for (i = 0; i < hdr->ntriggers; i++) {
            def = &hdr->triggers[i];
            trig = &sys->itriggers[i];
            if (trig->flags & 1)
                continue;
            if (trig->enableflags != 0xFF)
                continue;

            if (def->instance_ix >= 0) {
                struct nuvec_s tmp;
                struct numtx_s inv;
                struct numtx_s *mtx = &gscene->instances[def->instance_ix].mtx;

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
                    for (k = 0; k < def->numprims; k++) {
                        struct NUTRIGGERPRIM_s *sub = &def->prims[k];

                        switch (sub->type) {
                        case 1: {
                            struct NUTRIGGERPRIM_SPHERE_s *sph =
                                (struct NUTRIGGERPRIM_SPHERE_s *)sub->data;

                            NuVecMtxTransform(&tmp, &sph->centre, mtx);

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
                            struct NUTRIGGERPRIM_CUBE_s *box = (struct NUTRIGGERPRIM_CUBE_s *)sub->data;

                            NuMtxInvH(&inv, &box->invmtx);

                            line[0].pnt.x = box->min.x;
                            line[0].pnt.y = box->min.y;
                            line[0].pnt.z = box->min.z;
                            line[1].pnt.x = box->max.x;
                            line[1].pnt.y = box->min.y;
                            line[1].pnt.z = box->min.z;
                            NuVecMtxTransform(&line[0].pnt, &line[0].pnt, &inv);
                            NuVecMtxTransform(&line[0].pnt, &line[0].pnt, mtx);
                            NuVecMtxTransform(&line[1].pnt, &line[1].pnt, &inv);
                            NuVecMtxTransform(&line[1].pnt, &line[1].pnt, mtx);
                            NuRndrLine3d(line, 0, 0);

                            line[0].pnt.x = box->min.x;
                            line[0].pnt.y = box->min.y;
                            line[0].pnt.z = box->min.z;
                            line[1].pnt.x = box->min.x;
                            line[1].pnt.y = box->max.y;
                            line[1].pnt.z = box->min.z;
                            NuVecMtxTransform(&line[0].pnt, &line[0].pnt, &inv);
                            NuVecMtxTransform(&line[0].pnt, &line[0].pnt, mtx);
                            NuVecMtxTransform(&line[1].pnt, &line[1].pnt, &inv);
                            NuVecMtxTransform(&line[1].pnt, &line[1].pnt, mtx);
                            NuRndrLine3d(line, 0, 0);

                            line[0].pnt.x = box->min.x;
                            line[0].pnt.y = box->min.y;
                            line[0].pnt.z = box->min.z;
                            line[1].pnt.x = box->min.x;
                            line[1].pnt.y = box->min.y;
                            line[1].pnt.z = box->max.z;
                            NuVecMtxTransform(&line[0].pnt, &line[0].pnt, &inv);
                            NuVecMtxTransform(&line[0].pnt, &line[0].pnt, mtx);
                            NuVecMtxTransform(&line[1].pnt, &line[1].pnt, &inv);
                            NuVecMtxTransform(&line[1].pnt, &line[1].pnt, mtx);
                            NuRndrLine3d(line, 0, 0);

                            line[0].pnt.x = box->max.x;
                            line[0].pnt.y = box->max.y;
                            line[0].pnt.z = box->max.z;
                            line[1].pnt.x = box->min.x;
                            line[1].pnt.y = box->max.y;
                            line[1].pnt.z = box->max.z;
                            NuVecMtxTransform(&line[0].pnt, &line[0].pnt, &inv);
                            NuVecMtxTransform(&line[0].pnt, &line[0].pnt, mtx);
                            NuVecMtxTransform(&line[1].pnt, &line[1].pnt, &inv);
                            NuVecMtxTransform(&line[1].pnt, &line[1].pnt, mtx);
                            NuRndrLine3d(line, 0, 0);

                            line[0].pnt.x = box->max.x;
                            line[0].pnt.y = box->max.y;
                            line[0].pnt.z = box->max.z;
                            line[1].pnt.x = box->max.x;
                            line[1].pnt.y = box->min.y;
                            line[1].pnt.z = box->max.z;
                            NuVecMtxTransform(&line[0].pnt, &line[0].pnt, &inv);
                            NuVecMtxTransform(&line[0].pnt, &line[0].pnt, mtx);
                            NuVecMtxTransform(&line[1].pnt, &line[1].pnt, &inv);
                            NuVecMtxTransform(&line[1].pnt, &line[1].pnt, mtx);
                            NuRndrLine3d(line, 0, 0);

                            line[0].pnt.x = box->max.x;
                            line[0].pnt.y = box->max.y;
                            line[0].pnt.z = box->max.z;
                            line[1].pnt.x = box->max.x;
                            line[1].pnt.y = box->max.y;
                            line[1].pnt.z = box->min.z;
                            NuVecMtxTransform(&line[0].pnt, &line[0].pnt, &inv);
                            NuVecMtxTransform(&line[0].pnt, &line[0].pnt, mtx);
                            NuVecMtxTransform(&line[1].pnt, &line[1].pnt, &inv);
                            NuVecMtxTransform(&line[1].pnt, &line[1].pnt, mtx);
                            NuRndrLine3d(line, 0, 0);
                        } break;
                        case 3: {
                            struct NUTRIGGERPRIM_CYLINDER_s *cyl =
                                (struct NUTRIGGERPRIM_CYLINDER_s *)sub->data;

                            NuVecMtxTransform(&line[0].pnt, &cyl->bottom, mtx);
                            line[1].pnt = line[0].pnt;
                            line[1].pnt.y += cyl->height;
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
                    line[0].pnt.x = def->min.x;
                    line[0].pnt.y = def->min.y;
                    line[0].pnt.z = def->min.z;
                    line[1].pnt.x = def->max.x;
                    line[1].pnt.y = def->min.y;
                    line[1].pnt.z = def->min.z;
                    NuRndrLine3d(line, 0, 0);

                    line[0].pnt.x = def->min.x;
                    line[0].pnt.y = def->min.y;
                    line[0].pnt.z = def->min.z;
                    line[1].pnt.x = def->min.x;
                    line[1].pnt.y = def->max.y;
                    line[1].pnt.z = def->min.z;
                    NuRndrLine3d(line, 0, 0);

                    line[0].pnt.x = def->min.x;
                    line[0].pnt.y = def->min.y;
                    line[0].pnt.z = def->min.z;
                    line[1].pnt.x = def->min.x;
                    line[1].pnt.y = def->min.y;
                    line[1].pnt.z = def->max.z;
                    NuRndrLine3d(line, 0, 0);

                    line[0].pnt.x = def->max.x;
                    line[0].pnt.y = def->max.y;
                    line[0].pnt.z = def->max.z;
                    line[1].pnt.x = def->min.x;
                    line[1].pnt.y = def->max.y;
                    line[1].pnt.z = def->max.z;
                    NuRndrLine3d(line, 0, 0);

                    line[0].pnt.x = def->max.x;
                    line[0].pnt.y = def->max.y;
                    line[0].pnt.z = def->max.z;
                    line[1].pnt.x = def->max.x;
                    line[1].pnt.y = def->min.y;
                    line[1].pnt.z = def->max.z;
                    NuRndrLine3d(line, 0, 0);

                    line[0].pnt.x = def->max.x;
                    line[0].pnt.y = def->max.y;
                    line[0].pnt.z = def->max.z;
                    line[1].pnt.x = def->max.x;
                    line[1].pnt.y = def->max.y;
                    line[1].pnt.z = def->min.z;
                    NuRndrLine3d(line, 0, 0);
                }

                if (g_DbgTriggerShapes) {
                    for (k = 0; k < def->numprims; k++) {
                        struct NUTRIGGERPRIM_s *sub = &def->prims[k];

                        switch (sub->type) {
                        case 1: {
                            struct NUTRIGGERPRIM_SPHERE_s *sph =
                                (struct NUTRIGGERPRIM_SPHERE_s *)sub->data;

                            line[0].pnt = sph->centre;
                            line[1].pnt = sph->centre;
                            line[0].pnt.x -= sph->radius;
                            line[1].pnt.x += sph->radius;
                            NuRndrLine3d(line, 0, 0);

                            line[0].pnt = sph->centre;
                            line[1].pnt = sph->centre;
                            line[0].pnt.y -= sph->radius;
                            line[1].pnt.y += sph->radius;
                            NuRndrLine3d(line, 0, 0);

                            line[0].pnt = sph->centre;
                            line[1].pnt = sph->centre;
                            line[0].pnt.z -= sph->radius;
                            line[1].pnt.z += sph->radius;
                            NuRndrLine3d(line, 0, 0);
                        } break;
                        case 2: {
                            struct NUTRIGGERPRIM_CUBE_s *box = (struct NUTRIGGERPRIM_CUBE_s *)sub->data;

                            NuMtxInvH(&inv, &box->invmtx);

                            line[0].pnt.x = box->min.x;
                            line[0].pnt.y = box->min.y;
                            line[0].pnt.z = box->min.z;
                            line[1].pnt.x = box->max.x;
                            line[1].pnt.y = box->min.y;
                            line[1].pnt.z = box->min.z;
                            NuVecMtxTransform(&line[0].pnt, &line[0].pnt, &inv);
                            NuVecMtxTransform(&line[1].pnt, &line[1].pnt, &inv);
                            NuRndrLine3d(line, 0, 0);

                            line[0].pnt.x = box->min.x;
                            line[0].pnt.y = box->min.y;
                            line[0].pnt.z = box->min.z;
                            line[1].pnt.x = box->min.x;
                            line[1].pnt.y = box->max.y;
                            line[1].pnt.z = box->min.z;
                            NuVecMtxTransform(&line[0].pnt, &line[0].pnt, &inv);
                            NuVecMtxTransform(&line[1].pnt, &line[1].pnt, &inv);
                            NuRndrLine3d(line, 0, 0);

                            line[0].pnt.x = box->min.x;
                            line[0].pnt.y = box->min.y;
                            line[0].pnt.z = box->min.z;
                            line[1].pnt.x = box->min.x;
                            line[1].pnt.y = box->min.y;
                            line[1].pnt.z = box->max.z;
                            NuVecMtxTransform(&line[0].pnt, &line[0].pnt, &inv);
                            NuVecMtxTransform(&line[1].pnt, &line[1].pnt, &inv);
                            NuRndrLine3d(line, 0, 0);

                            line[0].pnt.x = box->max.x;
                            line[0].pnt.y = box->max.y;
                            line[0].pnt.z = box->max.z;
                            line[1].pnt.x = box->min.x;
                            line[1].pnt.y = box->max.y;
                            line[1].pnt.z = box->max.z;
                            NuVecMtxTransform(&line[0].pnt, &line[0].pnt, &inv);
                            NuVecMtxTransform(&line[1].pnt, &line[1].pnt, &inv);
                            NuRndrLine3d(line, 0, 0);

                            line[0].pnt.x = box->max.x;
                            line[0].pnt.y = box->max.y;
                            line[0].pnt.z = box->max.z;
                            line[1].pnt.x = box->max.x;
                            line[1].pnt.y = box->min.y;
                            line[1].pnt.z = box->max.z;
                            NuVecMtxTransform(&line[0].pnt, &line[0].pnt, &inv);
                            NuVecMtxTransform(&line[1].pnt, &line[1].pnt, &inv);
                            NuRndrLine3d(line, 0, 0);

                            line[0].pnt.x = box->max.x;
                            line[0].pnt.y = box->max.y;
                            line[0].pnt.z = box->max.z;
                            line[1].pnt.x = box->max.x;
                            line[1].pnt.y = box->max.y;
                            line[1].pnt.z = box->min.z;
                            NuVecMtxTransform(&line[0].pnt, &line[0].pnt, &inv);
                            NuVecMtxTransform(&line[1].pnt, &line[1].pnt, &inv);
                            NuRndrLine3d(line, 0, 0);
                        } break;
                        case 3: {
                            struct NUTRIGGERPRIM_CYLINDER_s *cyl =
                                (struct NUTRIGGERPRIM_CYLINDER_s *)sub->data;

                            line[0].pnt = cyl->bottom;
                            line[1].pnt = line[0].pnt;
                            line[1].pnt.y += cyl->height;
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


struct instNUTRIGGERSYS_s *instNuTriggerSysCreate(struct NUTRIGGERSYS_s *triggersys, struct NuSceneInstance *scenemanager,
                                                  union variptr_u *buff) {
    struct instNUTRIGGERSYS_s *itriggersys = 0;
    int i;

    if (triggersys != 0 &&
        scenemanager != 0 &&
        scenemanager->gscene != 0) {

        buff->intaddr = (buff->intaddr + 0xF) & ~0xF;
        itriggersys = buff->voidptr;

        buff->voidptr = itriggersys + 1;
        memset(itriggersys, 0, 0x18);

        itriggersys->prev = 0;
        itriggersys->next = g_NuTriggerSysList;

        if (g_NuTriggerSysList)
            g_NuTriggerSysList->prev = itriggersys;

        g_NuTriggerSysList = itriggersys;

        itriggersys->scenemanager = scenemanager;
        itriggersys->triggersys = triggersys;
        itriggersys->itriggers = (struct instNUTRIGGER_s *)buff->voidptr;

        buff->intaddr += triggersys->ntriggers * 4;
        memset(itriggersys->itriggers, 0, triggersys->ntriggers * 4);

        for (i = 0; i < triggersys->ntriggers; i++)
            itriggersys->itriggers[i].enableflags = triggersys->triggers[i].enableflags;
    }

    return itriggersys;
}

void instNuTriggerSysDestroy(struct instNUTRIGGERSYS_s *sys)
{
    if (sys->next)
        sys->next->prev = sys->prev;
    if (sys->prev)
        sys->prev->next = sys->next;
    else
        g_NuTriggerSysList = sys->next;
}


void instNuTriggerSetFlag(struct instNUTRIGGERSYS_s *sys, char *name, int index, int set)
{
    struct NUTRIGGERSYS_s *def = sys->triggersys;
    int i;

    for (i = 0; i < def->ntriggers; i++) {
        if (strcasecmp(def->triggers[i].triggername, name) == 0) {
            char bit = 1 << (index - 1);
            if (set)
                sys->itriggers[i].enableflags |= bit;
            else
                sys->itriggers[i].enableflags &= ~bit;
            break;
        }
    }
}


void instNuTriggerReset(struct instNUTRIGGERSYS_s *sys, char *name)
{
    struct NUTRIGGERSYS_s *def = sys->triggersys;
    int i;

    for (i = 0; i < def->ntriggers; i++) {
        if (strcasecmp(def->triggers[i].triggername, name) == 0) {
            sys->itriggers[i].enableflags = def->triggers[i].enableflags;
            sys->itriggers[i].flags &= 0xFE;
            break;
        }
    }
}


void instNuTriggerEnable(struct instNUTRIGGERSYS_s *sys)
{
    sys->is_disabled = 0;
}


void instNuTriggerDisable(struct instNUTRIGGERSYS_s *sys)
{
    sys->is_disabled = 1;
}


void NuTriggerPull(struct instNUTRIGGERSYS_s *sys, struct NUTRIGGER_s *def, struct instNUTRIGGER_s *trig, int pull)
{
    char buf[32];

    if (pull) {
        NuSceneInstanceRunScript(sys->scenemanager, def->triggername);
        trig->flags |= 1;
    } else {
        sprintf(buf, D_00619F60, def->triggername);
        NuSceneInstanceRunScript(sys->scenemanager, buf);
        trig->flags &= 0xFE;
    }
}

int CheckParentedTriggerWithPos(struct NUTRIGGER_s* trigger, struct numtx_s* mtx, struct nuvec_s* pos, float r) {
    int i;
    struct NUTRIGGERPRIM_s* prim;
    struct nuvec_s dp;
    struct nuvec_s vr;
    struct nuvec_s lpos;
    struct numtx_s invmtx;
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
            NuMtxInvH(&invmtx, mtx);
            NuVecMtxTransform(&lpos, pos, &invmtx);
            vr.x = vr.y = vr.z = r;
            NuVecMtxTransform(&vr, &vr, &invmtx);
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
                    NuVecMtxTransform(&centre, &sphere->centre, mtx);
                    if (NuVecDistSqr(pos, &centre, &dp) < (sphere->radius + r) * (sphere->radius + r)) {
                        return 1;
                    }
                    break;
                case NUTRIGGERPRIMTYPE_NONE:
                    break;
                case NUTRIGGERPRIMTYPE_CUBE:
                    cube = prim->data;
                    NuVecMtxTransform(&lpos2, &lpos, &cube->invmtx);
                    NuVecMtxTransform(&lvr, &vr, &cube->invmtx);
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
                    NuVecMtxTransform(&bottom, &cylinder->bottom, mtx);
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
                NuVecMtxTransform(&lpos, pos, &cube->invmtx);
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
        gscene = itriggersys->scenemanager->gscene;
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
                                check = CheckParentedTriggerWithPos(trigger, &instance->anim->mtx, pos, r);
                            } else {
                                check = CheckParentedTriggerWithPos(trigger, &instance->mtx, pos, r);
                            }
                        } else {
                            check = CheckUnparentedTriggerWithPos(trigger, pos, r);
                        }
                        if (check != 0) {
                            itrigger->hitpoints += hitpoints;
                            if (trigger->hitpoints <= itrigger->hitpoints) {
                                NuSceneInstanceRunScript(itriggersys->scenemanager, trigger->triggername);
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
