/*
 * Unit: nu3d/nuhgobj
 *
 * Functions:
 *   0x0014c828 NuHGobjDestroyDynamic
 *   0x0014ccc0 NuHGobjLoad
 *   0x0014ce70 ReadNuIFFHGobj
 *   0x0014d0a0 ReadNuIFFHGobjSet
 *   0x0014d9e8 NuGHGWriteGobj
 *   0x0014dc50 NuGHGWrite
 *   0x0014ed60 NuGHGRead
 *   0x0014f7e8 NuHGobjBoundingBoxToLine
 *   0x0014fae0 NuHGobjCollideSphere
 *   0x0014fce8 NuHGobjDestroy
 *   0x0014fdb8 NuHGobjPOIMtx
 *   0x0014fe28 NuHGobjPOIMtxWithCheck
 *   0x0014feb8 NuHGobjGetPOI
 *   0x0014fef8 NuHGobjJointMtx
 *   0x0014ff30 NuHGobjJointMtxWithCheck
 *   0x0014ff88 NuHGobjBoundingSphereToSphere
 *   0x001500e0 NuHGobjBoundingBoxToSphere
 *   0x001501c8 NuHGobjBoundingCylinderToSphere
 *   0x00150278 NuHGobjBoundingSphereToLine
 *   0x00150338 NuHGobjReadForCutScene
 *   0x001503f0 ReadNuIFFCollideData
 *   0x001504c8 NuGHGFixupGobj
 */

#include "nu_libc.h"
#include "nu_types.h"

#include "nu3d/numtl.h"
#include "nu3d/nutex.h"
#include "nu3d/nutexanm.h"
#include "nucore/nuerror.h"
#include "numath/numathtypes.h"
#include "numath/numtx.h"
#include "numath/nuplane.h"
#include "numath/nuvec.h"
#include "nu3d/nugobj.h"
#include "nu3d/nuhgobj.h"
#include "nu3d/nugscn.h"
#include "nu3d/nuscene.h"
#include "nucore/nucoretypes.h"
#include "nucore/nufile.h"

/* A gobj as NuGHGWrite stores it: nugobj_s up to origin. */
struct nughggobj_s {
    struct nugobj_s *sysnext;
    struct nugobj_s *syslast;
    int type;
    struct nugeom_s *geom;
    void *faceon_geom;
    f32 r;
    f32 r2;
    struct nuvec_s min;
    struct nuvec_s max;
    struct nuvec_s cntr;
    f32 cntr_r;
    f32 cntr_r2;
    int ngobjs;
    struct nugobj_s *next_gobj;
    struct nuvec_s origin;
};

struct nughggeom_s {
    struct nughggeom_s *next;
    int mtl_id;
    int dmastream;
    int skindma;
};

struct nutexanimenv_s;

extern void *NuMemAllocFn(int size, char *file, int line);
extern void NuMemFreeFn(void *ptr, char *file, int line);
extern int NuFileBeginBlkRead(int fh, int blockid);
extern void NuFileEndBlkRead(int fh);
extern int NuFileGetBlkSize(int fh);
extern int NuFileRead(int fh, void *buf, int size);
extern void NuMtxMulVU0(struct numtx_s *dst, struct numtx_s *a, struct numtx_s *b);
extern void NuCvtSknInstFix(struct SkinDmaTag *tag);
extern void NuCvtSknInstUnFix(struct SkinDmaTag *tag);

extern char D_00615290[];

#define nuhgobj_file D_00615290

#define NU_RELPTR(p) ((p) ? (void *)((char *)&(p) + (int)(p)) : 0)
#define NU_OFFSET_PTR(off, base) ((void *)((int)(off) + (base)))


struct nuhcollidesphere_s {
    struct nuvec_s centre;
    f32 radius;
    u8 joint;
};


int NuHGobjBoundingBoxToLine(struct NUHGOBJ_s *hobj, struct numtx_s *mtx, struct nuvec_s *p0,
                             struct nuvec_s *p1, f32 radius) {
    struct nuvec_s local0;
    struct nuvec_s local1;
    struct nuvec_s dir;
    struct nuvec_s hit;
    struct nuvec_s min;
    struct nuvec_s max;
    int code0;
    int code1;

    min.x = hobj->bbox_min.x - radius;
    min.y = hobj->bbox_min.y - radius;
    min.z = hobj->bbox_min.z - radius;
    max.x = hobj->bbox_max.x + radius;
    max.y = hobj->bbox_max.y + radius;
    max.z = hobj->bbox_max.z + radius;
    NuVecInvMtxTransform(&local0, p0, mtx);
    code0 = NuPointRelToBoundingBox(&local0, &max, &min);
    if (code0 == 0) {
        return 1;
    }
    NuVecInvMtxTransform(&local1, p1, mtx);
    code1 = NuPointRelToBoundingBox(&local1, &max, &min);
    if (code1 == 0) {
        return 1;
    }
    if (code0 & code1) {
        return 0;
    }
    NuVecSub(&dir, &local1, &local0);
    NuVecNorm(&dir, &dir);
    if (code0 & 1) {
        NuClipXPlane(&hit, &local0, &dir, &max.x);
        if (hit.y < max.y && min.y < hit.y && hit.z < max.z && min.z < hit.z) {
            return 1;
        }
    } else if (code0 & 8) {
        NuClipXPlane(&hit, &local0, &dir, &min.x);
        if (hit.y < max.y && min.y < hit.y && hit.z < max.z && min.z < hit.z) {
            return 1;
        }
    }
    if (code0 & 2) {
        NuClipYPlane(&hit, &local0, &dir, &max.y);
        if (hit.x < max.x && min.x < hit.x && hit.z < max.z && min.z < hit.z) {
            return 1;
        }
    } else if (code0 & 0x10) {
        NuClipYPlane(&hit, &local0, &dir, &min.y);
        if (hit.x < max.x && min.x < hit.x && hit.z < max.z && min.z < hit.z) {
            return 1;
        }
    }
    if (code0 & 4) {
        NuClipZPlane(&hit, &local0, &dir, &max.z);
        if (hit.x < max.x && min.x < hit.x && hit.y < max.y && min.y < hit.y) {
            return 1;
        }
    } else if (code0 & 0x20) {
        NuClipZPlane(&hit, &local0, &dir, &min.z);
        if (hit.x < max.x && min.x < hit.x && hit.y < max.y && min.y < hit.y) {
            return 1;
        }
    }
    return 0;
}


int NuHGobjCollideSphere(struct NUHGOBJ_s *hobj, struct numtx_s *wm, struct numtx_s *tmtx,
                         struct nuvec_s *pos, f32 radius, struct nuvec_s *hit) {
    struct nuvec_s d;
    struct numtx_s jm;
    struct nuhcollide_s *collide;
    struct nuhcollidesphere_s *sphere;
    struct nuhcollideentry_s *entry;
    unsigned int i;
    f32 reach;

    d.x = pos->x - wm->_30;
    d.y = pos->y - (wm->_31 + hobj->sphere_y);
    d.z = pos->z - wm->_32;
    reach = hobj->sphere_radius + radius;
    if (d.x * d.x + d.y * d.y + d.z * d.z < reach * reach) {
        collide = hobj->collide;
        if (collide) {
            for (i = 0; i < collide->nentries; i++) {
                entry = &collide->entries[i];
                if (entry->type == 1) {
                    sphere = entry->data;
                    NuMtxMul(&jm, &tmtx[sphere->joint], wm);
                    NuVecInvMtxTransform(&d, pos, &jm);
                    NuVecSub(&d, &d, &sphere->centre);
                    if (d.x * d.x + d.y * d.y + d.z * d.z < (radius + sphere->radius) * (radius + sphere->radius)) {
                        if (hit) {
                            hit->x = wm->_30;
                            hit->y = wm->_31 + hobj->sphere_y;
                            hit->z = wm->_32;
                        }
                        return 1;
                    }
                }
            }
        } else {
            if (hit) {
                hit->x = wm->_30;
                hit->y = wm->_31 + hobj->sphere_y;
                hit->z = wm->_32;
            }
            return 1;
        }
    }
    return 0;
}


void NuHGobjDestroyDynamic(struct NUHGOBJ_s *hobj) {
    struct nushadowdata_s *shadow;
    struct nushadowdata_s *next;
    int i;
    int j;

    if (hobj->texanims) {
        NuTexAnimRemoveList(hobj->texanims);
    }
    if (hobj->shadow) {
        for (shadow = hobj->shadow; shadow->last != 0xFF; shadow = next) {
            if (shadow->ellipsoids) {
                NuMemFreeFn(shadow->ellipsoids, nuhgobj_file, 0xFE);
            }
            if (shadow->cylinders) {
                NuMemFreeFn(shadow->cylinders, nuhgobj_file, 0x101);
            }
            next = shadow + 1;
            if (shadow->meshes) {
                while (shadow->nummeshes) {
                    shadow->nummeshes--;
                    if (shadow->meshes[shadow->nummeshes].first) {
                        NuMemFreeFn(shadow->meshes[shadow->nummeshes].first, nuhgobj_file, 0x107);
                    }
                    if (shadow->meshes[shadow->nummeshes].second) {
                        NuMemFreeFn(shadow->meshes[shadow->nummeshes].second, nuhgobj_file, 0x109);
                    }
                }
                NuMemFreeFn(shadow->meshes, nuhgobj_file, 0x10B);
            }
        }
        NuMemFreeFn(hobj->shadow, nuhgobj_file, 0x10F);
    }
    if (hobj->string_table) {
        NuMemFreeFn(hobj->string_table, nuhgobj_file, 0x113);
    }
    if (hobj->joints) {
        NuMemFreeFn(hobj->joints, nuhgobj_file, 0x115);
    }
    if (hobj->T) {
        NuMemFreeFn(hobj->T, nuhgobj_file, 0x117);
    }
    if (hobj->INV_WT) {
        NuMemFreeFn(hobj->INV_WT, nuhgobj_file, 0x119);
    }
    if (hobj->layers) {
        for (i = 0; i < hobj->num_layers; i++) {
            if (hobj->layers[i].gobjs) {
                for (j = 0; j < hobj->num_joints; j++) {
                    if (hobj->layers[i].gobjs[j]) {
                        NuGobjDestroy(hobj->layers[i].gobjs[j]);
                    }
                }
                NuMemFreeFn(hobj->layers[i].gobjs, nuhgobj_file, 0x126);
            }
            if (hobj->layers[i].blend_gobjs) {
                for (j = 0; j < hobj->num_joints; j++) {
                    if (hobj->layers[i].blend_gobjs[j]) {
                        NuGobjDestroy(hobj->layers[i].blend_gobjs[j]);
                    }
                }
                NuMemFreeFn(hobj->layers[i].blend_gobjs, nuhgobj_file, 0x130);
            }
            if (hobj->layers[i].skin_object) {
                NuGobjDestroy(hobj->layers[i].skin_object);
            }
            if (hobj->layers[i].blend_skin_object) {
                NuGobjDestroy(hobj->layers[i].blend_skin_object);
            }
        }
        NuMemFreeFn(hobj->layers, nuhgobj_file, 0x138);
    }
    if (hobj->mtls) {
        for (i = 0; i < hobj->nummtl; i++) {
            NuMtlDestroy(hobj->mtls[i]);
        }
        NuMemFreeFn(hobj->mtls, nuhgobj_file, 0x140);
    }
    if (hobj->tids) {
        for (i = 0; i < hobj->numtid; i++) {
            NuTexDestroy(hobj->tids[i]);
        }
        NuMemFreeFn(hobj->tids, nuhgobj_file, 0x148);
    }
    if (hobj->texanims) {
        NuTexAnimRemoveList(hobj->texanims);
    }
    for (i = 0; i < hobj->numtexanims; i++) {
        NuTexAnimDestroy(&hobj->texanims[i]);
    }
    if (hobj->texanims) {
        NuMemFreeFn(hobj->texanims, nuhgobj_file, 0x151);
    }
    if (hobj->texanim_tids) {
        NuMemFreeFn(hobj->texanim_tids, nuhgobj_file, 0x154);
    }
    if (hobj->poi_ixs) {
        NuMemFreeFn(hobj->poi_ixs, nuhgobj_file, 0x157);
    }
    if (hobj->joint_ixs) {
        NuMemFreeFn(hobj->joint_ixs, nuhgobj_file, 0x15A);
    }
    if (hobj->pois) {
        NuMemFreeFn(hobj->pois, nuhgobj_file, 0x15D);
    }
    if (hobj->collide) {
        NuMemFreeFn(hobj->collide, nuhgobj_file, 0x160);
    }
    NuMemFreeFn(hobj, nuhgobj_file, 0x162);
}


void NuHGobjDestroy(struct NUHGOBJ_s *hobj) {
    int i;

    if (hobj) {
        switch (hobj->type) {
        case 0:
        default:
            NuHGobjDestroyDynamic(hobj);
            break;
        case 1:
            if (hobj->texanims) {
                NuTexAnimRemoveList(hobj->texanims);
            }
            for (i = 0; i < hobj->nummtl; i++) {
                NuMtlDestroy(hobj->mtls[i]);
            }
            for (i = 0; i < hobj->numtid; i++) {
                NuTexDestroy(hobj->tids[i]);
            }
            break;
        }
    }
}


void NuHGobjPOIMtx(struct NUHGOBJ_s *hobj, u8 i, struct numtx_s *mC, struct numtx_s *tmtx,
                   struct numtx_s *out) {
    struct NUPOINTOFINTEREST_s *poi;

    poi = &hobj->pois[hobj->poi_ixs[i]];
    NuMtxMulVU0(out, &poi->offset, &tmtx[poi->parent_joint]);
    NuMtxMulVU0(out, out, mC);
}


int NuHGobjPOIMtxWithCheck(struct NUHGOBJ_s *hobj, u8 i, struct numtx_s *mC,
                           struct numtx_s *tmtx, struct numtx_s *out) {
    struct NUPOINTOFINTEREST_s *poi;
    u8 ix;

    if (i < hobj->num_poi_ixs) {
        ix = hobj->poi_ixs[i];
        if (ix != 0xFF) {
            poi = &hobj->pois[ix];
            NuMtxMulVU0(out, &poi->offset, &tmtx[poi->parent_joint]);
            NuMtxMulVU0(out, out, mC);
            return 1;
        }
    }
    return 0;
}


struct NUPOINTOFINTEREST_s *NuHGobjGetPOI(struct NUHGOBJ_s *hobj, u8 i) {
    u8 ix;

    if (i < hobj->num_poi_ixs) {
        ix = hobj->poi_ixs[i];
        if (ix != 0xFF) {
            return &hobj->pois[ix];
        }
    }
    return 0;
}


void NuHGobjJointMtx(struct NUHGOBJ_s *hobj, u8 i, struct numtx_s *mC, struct numtx_s *tmtx,
                     struct numtx_s *out) {
    NuMtxMulVU0(out, &tmtx[hobj->joint_ixs[i]], mC);
}


int NuHGobjJointMtxWithCheck(struct NUHGOBJ_s *hobj, u8 i, struct numtx_s *mC,
                             struct numtx_s *tmtx, struct numtx_s *out) {
    u8 ix;

    if (i < hobj->num_joint_ixs) {
        ix = hobj->joint_ixs[i];
        if (ix != 0xFF) {
            NuMtxMulVU0(out, &tmtx[ix], mC);
            return 1;
        }
    }
    return 0;
}


/* Whether the object's bounding sphere, placed by `wm`, touches the sphere
   of `radius` at `pos`. */
int NuHGobjBoundingSphereToSphere(struct NUHGOBJ_s *hobj, struct numtx_s *wm,
                                  struct nuvec_s *pos, f32 radius) {
    struct nuvec_s d;

    if (hobj->sphere_y == 0.0f) {
        d.x = wm->_30 - pos->x;
        d.y = wm->_31 - pos->y;
        d.z = wm->_32 - pos->z;
        if ((radius + hobj->sphere_radius) * (radius + hobj->sphere_radius) < d.x * d.x + d.y * d.y + d.z * d.z) {
            return 0;
        }
    } else {
        d.x = d.z = 0.0f;
        d.y = hobj->sphere_y;
        NuVecMtxRotate(&d, &d, wm);
        d.x = (wm->_30 + d.x) - pos->x;
        d.y = (wm->_31 + d.y) - pos->y;
        d.z = (wm->_32 + d.z) - pos->z;
        if ((radius + hobj->sphere_radius) * (radius + hobj->sphere_radius) < d.x * d.x + d.y * d.y + d.z * d.z) {
            return 0;
        }
    }
    return 1;
}


int NuHGobjBoundingBoxToSphere(struct NUHGOBJ_s *hobj, struct numtx_s *mtx, struct nuvec_s *pos,
                               f32 radius) {
    struct nuvec_s local;

    NuVecInvMtxTransform(&local, pos, mtx);
    if (local.x - radius > hobj->bbox_max.x) {
        return 0;
    }
    if (local.x + radius < hobj->bbox_min.x) {
        return 0;
    }
    if (local.y - radius > hobj->bbox_max.y) {
        return 0;
    }
    if (local.y + radius < hobj->bbox_min.y) {
        return 0;
    }
    if (local.z - radius > hobj->bbox_max.z) {
        return 0;
    }
    if (local.z + radius < hobj->bbox_min.z) {
        return 0;
    }
    return 1;
}


int NuHGobjBoundingCylinderToSphere(struct NUHGOBJ_s *hobj, struct numtx_s *mtx,
                                    struct nuvec_s *pos, f32 radius) {
    struct nuvec_s local;
    f32 distsq;
    f32 reach;

    NuVecInvMtxTransform(&local, pos, mtx);
    distsq = local.x * local.x + local.z * local.z;
    reach = radius + hobj->cyl_radius;
    if (distsq > reach * reach) {
        return 0;
    }
    if (hobj->cyl_y + hobj->cyl_height < local.y - radius) {
        return 0;
    }
    if (local.y + radius < hobj->cyl_y) {
        return 0;
    }
    return 1;
}


int NuHGobjBoundingSphereToLine(struct NUHGOBJ_s *hobj, struct numtx_s *mtx, struct nuvec_s *p0,
                                struct nuvec_s *p1, f32 radius) {
    struct nuvec_s centre;
    f32 distsq;
    f32 reach;

    if (hobj->sphere_y == 0.0f) {
        distsq = NuLineToPointDistSqr(p0, p1, (struct nuvec_s *)&mtx->_30);
    } else {
        centre.z = 0.0f;
        centre.x = 0.0f;
        centre.y = hobj->sphere_y;
        NuVecMtxTransform(&centre, &centre, mtx);
        distsq = NuLineToPointDistSqr(p0, p1, &centre);
    }
    reach = radius + hobj->sphere_radius;
    if (distsq < reach * reach) {
        return 1;
    }
    return 0;
}


static inline struct NUHGOBJ_s *NuHGobjCreate(void) {
    struct NUHGOBJ_s *hobj;

    hobj = NuMemAllocFn(sizeof(struct NUHGOBJ_s), nuhgobj_file, 0xE5);
    memset(hobj, 0, sizeof(struct NUHGOBJ_s));
    hobj->type = 0;
    hobj->scale = 1.0f;
    return hobj;
}


extern int NuFileSize(char *name);
extern int NuFileLoadBuffer(char *name, void *buf, int size);
extern int NuMemFileOpen(void *buf, int size, int mode);
extern void NuFileClose(int fh);
extern char *strlwr(char *s);
extern char D_006152B0[];
extern char D_006152D0[];
extern char D_006152F0[];
#define msg_hgobj_loading D_006152B0
#define msg_hgobj_open_failed D_006152D0
#define msg_hgobj_alloc_failed D_006152F0

struct NUHGOBJ_s *NuHGobjLoad(char *file) {
    struct NUHGOBJ_s *hobj;
    void *buf;
    int size;
    int fh;

    hobj = 0;
    size = NuFileSize(file);
    buf = NuMemAllocFn(size, nuhgobj_file, 0x19F);
    if (buf) {
        fh = NuMemFileOpen(buf, NuFileLoadBuffer(file, buf, size), 0);
        if (fh) {
            NuDebugMsgProlog(nuhgobj_file, 0x1A8)(msg_hgobj_loading, strlwr(file));
            NuFileBeginBlkRead(fh, 0x464F4748);
            hobj = NuHGobjCreate();
            ReadNuIFFHGobj(fh, hobj);
            NuFileEndBlkRead(fh);
            NuFileClose(fh);
        } else {
            NuDebugMsgProlog(nuhgobj_file, 0x1B2)(msg_hgobj_open_failed, strlwr(file));
        }
        NuMemFreeFn(buf, nuhgobj_file, 0x1B4);
    } else {
        NuErrorProlog(nuhgobj_file, 0x1B7)(msg_hgobj_alloc_failed, size);
    }
    return hobj;
}


extern int NuFileReadInt(int fh);
extern char NuFileReadChar(int fh);
extern float NuFileReadFloat(int fh);
extern char D_00615310[];
extern char D_00615330[];
extern char D_00615358[];
extern char D_00615380[];
#define msg_shadow_alloc_failed D_00615310
#define msg_ellipsoid_alloc_failed D_00615330
#define msg_cylinder_alloc_failed D_00615358
#define msg_mesh_alloc_failed D_00615380

void ReadNuIFFHGobjSet(int fh, struct NUHGOBJ_s *hobj) {
    struct nushadowdata_s *shadow;
    int numshadows;
    int size;
    int ix;
    int i;
    int j;

    hobj->num_joints = NuFileReadChar(fh);
    if (hobj->num_joints) {
        hobj->joints = NuMemAllocFn(hobj->num_joints * sizeof(struct NUJOINTDATA_s), nuhgobj_file, 0x22C);
        memset(hobj->joints, 0, hobj->num_joints * sizeof(struct NUJOINTDATA_s));
        hobj->T = NuMemAllocFn(hobj->num_joints * sizeof(struct numtx_s), nuhgobj_file, 0x22E);
        memset(hobj->T, 0, hobj->num_joints * sizeof(struct numtx_s));
        hobj->INV_WT = NuMemAllocFn(hobj->num_joints * sizeof(struct numtx_s), nuhgobj_file, 0x230);
        memset(hobj->INV_WT, 0, hobj->num_joints * sizeof(struct numtx_s));
        for (i = 0; i < hobj->num_joints; i++) {
            NuFileRead(fh, &hobj->joints[i].flags, 1);
            NuFileRead(fh, &hobj->T[i], sizeof(struct numtx_s));
            NuFileRead(fh, &hobj->INV_WT[i], sizeof(struct numtx_s));
            NuFileRead(fh, &hobj->joints[i].orient, sizeof(struct numtx_s));
            NuFileRead(fh, &hobj->joints[i].locator_offset, sizeof(struct nuvec_s));
            hobj->joints[i].parent_ix = NuFileReadChar(fh);
            ix = NuFileReadInt(fh);
            if (ix) {
                hobj->joints[i].name = &hobj->string_table[ix - 1];
            }
        }
        hobj->num_joint_ixs = NuFileReadChar(fh);
        if (hobj->num_joint_ixs) {
            hobj->joint_ixs = NuMemAllocFn(hobj->num_joint_ixs, nuhgobj_file, 0x252);
            NuFileRead(fh, hobj->joint_ixs, hobj->num_joint_ixs);
        }
        hobj->num_poi_ixs = NuFileReadChar(fh);
        if (hobj->num_poi_ixs) {
            hobj->poi_ixs = NuMemAllocFn(hobj->num_poi_ixs, nuhgobj_file, 0x259);
            NuFileRead(fh, hobj->poi_ixs, hobj->num_poi_ixs);
        }
        hobj->num_layers = NuFileReadChar(fh);
        if (hobj->num_layers) {
            hobj->layers = NuMemAllocFn(hobj->num_layers * sizeof(struct NUHLAYERDATA_s), nuhgobj_file, 0x263);
            memset(hobj->layers, 0, hobj->num_layers * sizeof(struct NUHLAYERDATA_s));
            for (i = 0; i < hobj->num_layers; i++) {
                ix = NuFileReadInt(fh);
                if (ix) {
                    hobj->layers[i].name = &hobj->string_table[ix - 1];
                }
                if (NuFileReadChar(fh)) {
                    hobj->layers[i].gobjs = NuMemAllocFn(hobj->num_joints * sizeof(struct nugobj_s *), nuhgobj_file, 0x271);
                    memset(hobj->layers[i].gobjs, 0, hobj->num_joints * sizeof(struct nugobj_s *));
                    for (j = 0; j < hobj->num_joints; j++) {
                        if (NuFileReadChar(fh)) {
                            hobj->layers[i].gobjs[j] = ReadNuIFFGeom(fh, hobj->mtls);
                        }
                    }
                }
                if (NuFileReadChar(fh)) {
                    hobj->layers[i].skin_object = ReadNuIFFGeom(fh, hobj->mtls);
                }
                if (NuFileReadChar(fh)) {
                    hobj->layers[i].blend_gobjs = NuMemAllocFn(hobj->num_joints * sizeof(struct nugobj_s *), nuhgobj_file, 0x283);
                    memset(hobj->layers[i].blend_gobjs, 0, hobj->num_joints * sizeof(struct nugobj_s *));
                    for (j = 0; j < hobj->num_joints; j++) {
                        if (NuFileReadChar(fh)) {
                            hobj->layers[i].blend_gobjs[j] = ReadNuIFFGeom(fh, hobj->mtls);
                        }
                    }
                }
                if (NuFileReadChar(fh)) {
                    hobj->layers[i].blend_skin_object = ReadNuIFFGeom(fh, hobj->mtls);
                }
            }
        }
        hobj->num_pois = NuFileReadChar(fh);
        if (hobj->num_pois) {
            hobj->pois = NuMemAllocFn(hobj->num_pois * sizeof(struct NUPOINTOFINTEREST_s), nuhgobj_file, 0x298);
            memset(hobj->pois, 0, hobj->num_pois * sizeof(struct NUPOINTOFINTEREST_s));
            for (i = 0; i < hobj->num_pois; i++) {
                hobj->pois[i].parent_joint = NuFileReadChar(fh);
                NuFileRead(fh, &hobj->pois[i].offset, sizeof(struct numtx_s));
                ix = NuFileReadInt(fh);
                if (ix) {
                    hobj->pois[i].name = &hobj->string_table[ix - 1];
                }
            }
        }
        numshadows = NuFileReadChar(fh);
        if (numshadows) {
            hobj->shadow = NuMemAllocFn((numshadows + 1) * sizeof(struct nushadowdata_s), nuhgobj_file, 0x2B0);
            if (!hobj->shadow) {
                NuErrorProlog(nuhgobj_file, 0x2B3)(msg_shadow_alloc_failed);
            }
            memset(hobj->shadow, 0, (numshadows + 1) * sizeof(struct nushadowdata_s));
            hobj->shadow[numshadows].last = 0xFF;
            for (i = 0; i < numshadows; i++) {
                shadow = &hobj->shadow[i];
                shadow->ellipsoids = 0;
                shadow->meshes = 0;
                shadow->cylinders = 0;
                shadow->numellipsoids = NuFileReadChar(fh);
                if (shadow->numellipsoids) {
                    size = shadow->numellipsoids * 0x30;
                    shadow->ellipsoids = NuMemAllocFn(size, nuhgobj_file, 0x2C6);
                    if (!shadow->ellipsoids) {
                        NuErrorProlog(nuhgobj_file, 0x2C8)(msg_ellipsoid_alloc_failed);
                    }
                    NuFileRead(fh, shadow->ellipsoids, size);
                }
                shadow->numcylinders = NuFileReadChar(fh);
                if (shadow->numcylinders) {
                    size = shadow->numcylinders * 0x40;
                    shadow->cylinders = NuMemAllocFn(size, nuhgobj_file, 0x2D1);
                    if (!shadow->cylinders) {
                        NuErrorProlog(nuhgobj_file, 0x2D3)(msg_cylinder_alloc_failed);
                    }
                    NuFileRead(fh, shadow->cylinders, size);
                }
                shadow->nummeshes = NuFileReadChar(fh);
                if (shadow->nummeshes) {
                    size = shadow->nummeshes * sizeof(struct nushadowmesh_s);
                    shadow->meshes = NuMemAllocFn(size, nuhgobj_file, 0x2DC);
                    if (!shadow->meshes) {
                        NuErrorProlog(nuhgobj_file, 0x2DE)(msg_mesh_alloc_failed);
                    }
                    memset(shadow->meshes, 0, size);
                    for (j = 0; j < shadow->nummeshes; j++) {
                        size = NuFileReadInt(fh) * 0x10;
                        shadow->meshes[j].first = NuMemAllocFn(size, nuhgobj_file, 0x2E6);
                        NuFileRead(fh, shadow->meshes[j].first, size);
                        size = NuFileReadInt(fh) * 0x10;
                        shadow->meshes[j].second = NuMemAllocFn(size, nuhgobj_file, 0x2EB);
                        NuFileRead(fh, shadow->meshes[j].second, size);
                    }
                }
                shadow->last = NuFileReadChar(fh);
            }
        }
    }
    hobj->sphere_radius = NuFileReadFloat(fh);
    hobj->sphere_y = NuFileReadFloat(fh);
    hobj->bbox_min.x = NuFileReadFloat(fh);
    hobj->bbox_min.y = NuFileReadFloat(fh);
    hobj->bbox_min.z = NuFileReadFloat(fh);
    hobj->bbox_max.x = NuFileReadFloat(fh);
    hobj->bbox_max.y = NuFileReadFloat(fh);
    hobj->bbox_max.z = NuFileReadFloat(fh);
    hobj->cyl_y = NuFileReadFloat(fh);
    hobj->cyl_height = NuFileReadFloat(fh);
    hobj->cyl_radius = NuFileReadFloat(fh);
    if (hobj->bbox_min.x == 0.0f && hobj->bbox_min.y == 0.0f && hobj->bbox_min.z == 0.0f) {
        hobj->bbox_min.x = hobj->bbox_min.y = hobj->bbox_min.z = -1.0f;
    }
    if (hobj->bbox_max.x == 0.0f && hobj->bbox_max.y == 0.0f && hobj->bbox_max.z == 0.0f) {
        hobj->bbox_max.x = hobj->bbox_max.y = hobj->bbox_max.z = 1.0f;
    }
}


struct NUHGOBJ_s *NuHGobjReadForCutScene(int fh, struct NUHGOBJ_s *shared) {
    struct NUHGOBJ_s *hobj;

    hobj = NuHGobjCreate();
    hobj->nummtl = shared->nummtl;
    hobj->mtls = shared->mtls;
    hobj->numtid = shared->numtid;
    hobj->tids = shared->tids;
    NuFileBeginBlkRead(fh, 0x304F4748);
    ReadNuIFFHGobjSet(fh, hobj);
    NuFileEndBlkRead(fh);
    return hobj;
}


void ReadNuIFFCollideData(int fh, struct NUHGOBJ_s *hobj) {
    unsigned int i;
    int size;

    size = NuFileGetBlkSize(fh);
    hobj->collide = NuMemAllocFn(size, nuhgobj_file, 0x218);
    NuFileRead(fh, hobj->collide, size);
    hobj->collide->entries = NU_RELPTR(hobj->collide->entries);
    for (i = 0; i < hobj->collide->nentries; i++) {
        hobj->collide->entries[i].data = NU_RELPTR(hobj->collide->entries[i].data);
    }
}


extern void ReadNuIFFMaterialSet(int fh, struct nuscene_s *scene);
extern void ReadNuIFFUnknown(int fh, int blk);

void ReadNuIFFHGobj(int fh, struct NUHGOBJ_s *hobj) {
    struct nuscene_s scene;
    struct nugscn_s gscn;
    int blk;
    int i;

    memset(&scene, 0, sizeof(struct nuscene_s));
    memset(&gscn, 0, sizeof(struct nugscn_s));
    scene.numtids = 0;
    scene.tids = 0;
    scene.nummtls = 0;
    scene.mtls = 0;
    while ((blk = NuFileBeginBlkRead(fh, 0)) != 0) {
        switch (blk) {
        case 0x4C42544E:
            hobj->string_table = ReadNuIFFNameTable(fh, &hobj->string_table_size);
            break;
        case 0x30545354:
            ReadNuIFFTextureSet(fh, &scene);
            hobj->numtid = scene.numtids;
            hobj->tids = NuMemAllocFn(scene.numtids << 2, nuhgobj_file, 0x1EB);
            for (i = 0; i < scene.numtids; i++) {
                hobj->tids[i] = scene.tids[i];
            }
            break;
        case 0x3030534D:
            ReadNuIFFMaterialSet(fh, &scene);
            hobj->mtls = scene.mtls;
            hobj->nummtl = scene.nummtls;
            gscn.mtls = scene.mtls;
            gscn.nummtl = scene.nummtls;
            break;
        case 0x304F4748:
            ReadNuIFFHGobjSet(fh, hobj);
            break;
        case 0x4C4C4F43:
            ReadNuIFFCollideData(fh, hobj);
            break;
        case 0x30534154:
            gscn.nametable = hobj->string_table;
            ReadNuIFFTexAnimSet(fh, &gscn, scene.tids);
            hobj->texanim_tids = gscn.texanim_tids;
            hobj->texanims = gscn.texanims;
            hobj->numtexanims = gscn.numtexanims;
            break;
        default:
            ReadNuIFFUnknown(fh, blk);
            break;
        }
        NuFileEndBlkRead(fh);
    }
    if (scene.tids) {
        NuMemFreeFn(scene.tids, nuhgobj_file, 0x20D);
    }
    scene.tids = 0;
}


char *NuGHGWriteGobj(char *base, char *ptr, struct NUHGOBJ_s *hobj, struct nugobj_s *gobj,
                     int *offset) {
    struct nughggobj_s *out;
    struct nughggobj_s *last;
    struct nugeom_s *geom;
    struct nughggeom_s *ogeom;
    struct nughggeom_s *lastgeom;

    last = 0;
    *offset = 0;
    while (gobj) {
        ptr = (char *)(((unsigned int)ptr + 3) & ~3);
        out = (struct nughggobj_s *)ptr;
        if (last) {
            last->next_gobj = (struct nugobj_s *)(ptr - base);
        } else {
            *offset = ptr - base;
        }
        ptr = (char *)&out[1];
        memcpy(out, gobj, sizeof(struct nughggobj_s));
        out->faceon_geom = 0;
        lastgeom = 0;
        for (geom = gobj->geom; geom; geom = geom->next) {
            ptr = (char *)(((unsigned int)ptr + 3) & ~3);
            ogeom = (struct nughggeom_s *)ptr;
            if (lastgeom) {
                lastgeom->next = (struct nughggeom_s *)(ptr - base);
            } else {
                out->geom = (struct nugeom_s *)(ptr - base);
            }
            ogeom->next = 0;
            ogeom->mtl_id = geom->mtl_id;
            ptr = (char *)&ogeom[1];
            if (geom->dmastream) {
                ptr = (char *)(((unsigned int)ptr + 0xF) & ~0xF);
                ogeom->dmastream = ptr - base;
                memcpy(ptr, geom->dmastream, geom->dmasize);
                ptr += geom->dmasize;
            } else {
                ogeom->dmastream = 0;
            }
            if (geom->skindma) {
                ptr = (char *)(((unsigned int)ptr + 0xF) & ~0xF);
                ogeom->skindma = ptr - base;
                NuCvtSknInstUnFix(geom->skindma);
                memcpy(ptr, geom->skindma, geom->dmasize);
                NuCvtSknInstFix(geom->skindma);
                ptr += geom->dmasize;
            } else {
                ogeom->skindma = 0;
            }
            lastgeom = ogeom;
        }
        last = out;
        gobj = gobj->next_gobj;
    }
    return ptr;
}


inline void NuGHGFixupGobj(int base, struct nugobj_s *gobj, struct NUHGOBJ_s *hobj) {
    struct nugeom_s *geom;

    while (gobj) {
        if (gobj->geom) {
            geom = gobj->geom = NU_OFFSET_PTR(gobj->geom, base);
            while (geom) {
                geom->mtl = hobj->mtls[(int)geom->mtl];
                if (geom->dmastream) {
                    geom->dmastream = NU_OFFSET_PTR(geom->dmastream, base);
                }
                if (geom->skindma) {
                    geom->skindma = NU_OFFSET_PTR(geom->skindma, base);
                    NuCvtSknInstFix(geom->skindma);
                }
                if (geom->next) {
                    geom = geom->next = NU_OFFSET_PTR(geom->next, base);
                } else {
                    geom = 0;
                }
            }
        }
        if (gobj->next_gobj) {
            gobj = gobj->next_gobj = NU_OFFSET_PTR(gobj->next_gobj, base);
        } else {
            gobj = 0;
        }
    }
}


struct nughgheader_s {
    int size;
    int version;
    int numtid;
    int tids;
    int nummtl;
    int mtls;
    int num_joints;
    int joints;
    int T;
    int INV_WT;
    int num_joint_ixs;
    int joint_ixs;
    int string_table;
    int string_table_size;
    int num_poi_ixs;
    int poi_ixs;
    int num_pois;
    int pois;
    int num_layers;
    int layers;
    int shadow;
    f32 sphere_radius;
    f32 sphere_y;
    struct nuvec_s bbox_min;
    struct nuvec_s bbox_max;
    f32 cyl_y;
    f32 cyl_height;
    f32 cyl_radius;
    int collide;
    f32 scale;
    int numtexanims;
    int texanims;
};


extern void NuDisableVBlank(void);
extern void NuEnableVBlank(void);
extern NuMtlReadEventFunc NuMtlReadEvent;
extern int D_0062EECC;
extern char D_00615470[];
extern char D_006154C0[];
extern char D_00615510[];
extern char D_00615520[];
#define nughg_version D_0062EECC
#define msg_ghg_file_old D_00615470
#define msg_ghg_loader_old D_006154C0
#define msg_ghg_reading D_00615510
#define msg_ghg_read_size D_00615520

/* A layer as NuGHGWrite stores it: every pointer as an offset from the file base. */
struct nughglayer_s {
    int name;
    int gobjs;
    int skin_object;
    int blend_gobjs;
    int blend_skin_object;
};

extern int NuFileOpen(char *file, enum nufilemode_e mode);
extern int NuFileWrite(int fh, void *data, int size);
extern void NuStrCpy(char *dst, char *src);
extern int NuStrLen(char *str);
extern char D_006153A0[];
extern char D_006153B8[];
extern char D_006153D0[];
extern char D_006153E8[];
extern char D_00615400[];
extern char D_00615420[];
extern char D_00615438[];
extern char D_00615458[];
#define msg_ghg_wrote_textures D_006153A0
#define msg_ghg_wrote_materials D_006153B8
#define msg_ghg_wrote_strings D_006153D0
#define msg_ghg_wrote_joints D_006153E8
#define msg_ghg_wrote_pois D_00615400
#define msg_ghg_wrote_layer D_00615420
#define msg_ghg_wrote_texanims D_00615438
#define msg_ghg_wrote_shadow D_00615458

void NuGHGWrite(struct NUHGOBJ_s *hobj, char *file, int size) {
    char *base;
    struct nughgheader_s *header;
    char *ptr;
    int *offsets;
    struct numtl_s *mtl;
    struct NUJOINTDATA_s *joints;
    struct NUPOINTOFINTEREST_s *pois;
    struct nughglayer_s *layer;
    struct nuhcollide_s *collide;
    struct nuhcollideentry_s *entry;
    struct nutexanim_s *anims;
    struct nutexanim_s *anim;
    short *tids;
    int fh;
    int i;
    int j;
    int k;

    ptr = NuMemAllocFn(size, nuhgobj_file, 0x425);
    if (ptr) {
        header = (struct nughgheader_s *)ptr;
        base = ptr;
        memset(header, 0, size);
        header->version = nughg_version;
        ptr = (char *)(((unsigned int)&header[1] + 3) & ~3);
        offsets = (int *)ptr;
        header->scale = hobj->scale;
        header->numtid = hobj->numtid;
        if (header->numtid) {
            header->tids = ptr - base;
            ptr += hobj->numtid * 4;
            for (i = 0; i < hobj->numtid; i++) {
                ptr = (char *)(((unsigned int)ptr + 0xF) & ~0xF);
                offsets[i] = ptr - base;
                ptr += NuTexRecoverNativeData(hobj->tids[i], ptr, size);
            }
        }
        NuDebugMsgProlog(nuhgobj_file, 0x444)(msg_ghg_wrote_textures, ptr - base);

        ptr = (char *)(((unsigned int)ptr + 3) & ~3);
        offsets = (int *)ptr;
        header->mtls = ptr - base;
        header->nummtl = hobj->nummtl;
        ptr += hobj->nummtl * 4;
        for (i = 0; i < hobj->nummtl; i++) {
            mtl = (struct numtl_s *)(((unsigned int)ptr + 0xF) & ~0xF);
            offsets[i] = (char *)mtl - base;
            *mtl = *hobj->mtls[i];
            if (mtl->tid) {
                for (j = 0; j < hobj->numtid; j++) {
                    if (mtl->tid == hobj->tids[j]) {
                        mtl->tid = j + 1;
                        break;
                    }
                }
                if (j == hobj->numtid) {
                    mtl->tid = 0;
                }
            }
            ptr = (char *)&mtl[1];
        }
        NuDebugMsgProlog(nuhgobj_file, 0x45E)(msg_ghg_wrote_materials, ptr - base);

        header->string_table = ptr - base;
        header->string_table_size = hobj->string_table_size;
        memcpy(ptr, hobj->string_table, hobj->string_table_size);
        ptr += hobj->string_table_size;
        NuDebugMsgProlog(nuhgobj_file, 0x467)(msg_ghg_wrote_strings, ptr - base);

        header->num_joints = hobj->num_joints;
        ptr = (char *)(((unsigned int)ptr + 0xF) & ~0xF);
        joints = (struct NUJOINTDATA_s *)ptr;
        header->joints = ptr - base;
        for (i = 0; i < hobj->num_joints; i++) {
            *joints = hobj->joints[i];
            joints->name = joints->name - hobj->string_table + header->string_table;
            joints++;
        }
        ptr += hobj->num_joints * sizeof(struct NUJOINTDATA_s);
        ptr = (char *)(((unsigned int)ptr + 0xF) & ~0xF);
        header->T = ptr - base;
        memcpy(ptr, hobj->T, hobj->num_joints * sizeof(struct numtx_s));
        ptr += hobj->num_joints * sizeof(struct numtx_s);
        ptr = (char *)(((unsigned int)ptr + 0xF) & ~0xF);
        header->INV_WT = ptr - base;
        memcpy(ptr, hobj->INV_WT, hobj->num_joints * sizeof(struct numtx_s));
        header->num_joint_ixs = hobj->num_joint_ixs;
        ptr += hobj->num_joints * sizeof(struct numtx_s);
        header->joint_ixs = ptr - base;
        for (i = 0; i < hobj->num_joint_ixs; i++) {
            *ptr++ = hobj->joint_ixs[i];
        }
        NuDebugMsgProlog(nuhgobj_file, 0x486)(msg_ghg_wrote_joints, ptr - base);

        if (hobj->num_poi_ixs) {
            header->poi_ixs = ptr - base;
            header->num_poi_ixs = hobj->num_poi_ixs;
            for (i = 0; i < hobj->num_poi_ixs; i++) {
                *ptr++ = hobj->poi_ixs[i];
            }
        }
        if (hobj->num_pois) {
            ptr = (char *)(((unsigned int)ptr + 0xF) & ~0xF);
            header->num_pois = hobj->num_pois;
            header->pois = ptr - base;
            pois = (struct NUPOINTOFINTEREST_s *)ptr;
            ptr += hobj->num_pois * sizeof(struct NUPOINTOFINTEREST_s);
            for (i = 0; i < hobj->num_pois; i++) {
                *pois = hobj->pois[i];
                if (pois->name) {
                    pois->name = pois->name - hobj->string_table + header->string_table;
                }
                pois++;
            }
        }
        NuDebugMsgProlog(nuhgobj_file, 0x4A0)(msg_ghg_wrote_pois, ptr - base);

        ptr = (char *)(((unsigned int)ptr + 3) & ~3);
        header->num_layers = hobj->num_layers;
        header->layers = ptr - base;
        layer = (struct nughglayer_s *)ptr;
        ptr += hobj->num_layers * sizeof(struct nughglayer_s);
        for (i = 0; i < hobj->num_layers; i++) {
            if (hobj->layers[i].name) {
                layer->name = ptr - base;
                NuStrCpy(ptr, hobj->layers[i].name);
                ptr += NuStrLen(hobj->layers[i].name) + 1;
            } else {
                layer->name = 0;
            }
            if (hobj->layers[i].gobjs) {
                ptr = (char *)(((unsigned int)ptr + 3) & ~3);
                offsets = (int *)ptr;
                layer->gobjs = ptr - base;
                ptr += hobj->num_joints * 4;
                for (j = 0; j < hobj->num_joints; j++) {
                    ptr = NuGHGWriteGobj(base, ptr, hobj, hobj->layers[i].gobjs[j], &offsets[j]);
                }
            } else {
                layer->gobjs = 0;
            }
            if (hobj->layers[i].blend_gobjs) {
                ptr = (char *)(((unsigned int)ptr + 3) & ~3);
                offsets = (int *)ptr;
                layer->blend_gobjs = ptr - base;
                ptr += hobj->num_joints * 4;
                for (j = 0; j < hobj->num_joints; j++) {
                    ptr = NuGHGWriteGobj(base, ptr, hobj, hobj->layers[i].blend_gobjs[j], &offsets[j]);
                }
            } else {
                layer->blend_gobjs = 0;
            }
            ptr = NuGHGWriteGobj(base, ptr, hobj, hobj->layers[i].skin_object, &layer->skin_object);
            ptr = NuGHGWriteGobj(base, ptr, hobj, hobj->layers[i].blend_skin_object,
                                 &layer->blend_skin_object);
            NuDebugMsgProlog(nuhgobj_file, 0x4D2)(msg_ghg_wrote_layer, i, ptr - base);
            layer++;
        }

        header->sphere_radius = hobj->sphere_radius;
        header->sphere_y = hobj->sphere_y;
        header->bbox_min = hobj->bbox_min;
        header->bbox_max = hobj->bbox_max;
        header->cyl_y = hobj->cyl_y;
        header->cyl_height = hobj->cyl_height;
        header->cyl_radius = hobj->cyl_radius;
        header->collide = 0;
        if (hobj->collide) {
            ptr = (char *)(((unsigned int)ptr + 3) & ~3);
            header->collide = ptr - base;
            collide = (struct nuhcollide_s *)ptr;
            *collide = *hobj->collide;
            ptr += sizeof(struct nuhcollide_s);
            collide->entries = (struct nuhcollideentry_s *)(ptr - base);
            entry = (struct nuhcollideentry_s *)ptr;
            memcpy(ptr, hobj->collide->entries,
                   hobj->collide->nentries * sizeof(struct nuhcollideentry_s));
            ptr += hobj->collide->nentries * sizeof(struct nuhcollideentry_s);
            ptr = (char *)(((unsigned int)ptr + 3) & ~3);
            for (i = 0; i < hobj->collide->nentries; i++) {
                entry[i].data = (void *)(ptr - base);
                if (hobj->collide->entries[i].type == 1) {
                    memcpy(ptr, hobj->collide->entries[i].data, sizeof(struct nuhcollidesphere_s));
                    ptr += sizeof(struct nuhcollidesphere_s);
                }
            }
        }

        header->numtexanims = hobj->numtexanims;
        if (hobj->numtexanims) {
            ptr = (char *)(((unsigned int)ptr + 0xF) & ~0xF);
            anims = (struct nutexanim_s *)ptr;
            header->texanims = ptr - base;
            tids = (short *)(ptr += hobj->numtexanims * sizeof(struct nutexanim_s));
            anim = anims;
            for (i = 0; i < hobj->numtexanims; i++, anim++) {
                *anim = hobj->texanims[i];
                for (j = 0; j < hobj->nummtl; j++) {
                    if (anim->mtl == hobj->mtls[j]) {
                        anim->mtl = (struct numtl_s *)j;
                        break;
                    }
                }
                anim->scriptname = anim->scriptname - hobj->string_table + header->string_table;
                anim->tids = (short *)((char *)tids - base);
                tids += anim->numtids;
                anim->ntaname = anim->ntaname - hobj->string_table + header->string_table;
            }
            ptr = (char *)&anims[hobj->numtexanims];
            for (i = 0; i < hobj->numtexanims; i++) {
                for (j = 0; j < hobj->texanims[i].numtids; j++) {
                    for (k = 0; k < hobj->numtid; k++) {
                        if (hobj->texanims[i].tids[j] == hobj->tids[k]) {
                            *(short *)ptr = k;
                            break;
                        }
                    }
                    ptr += 2;
                }
            }
            NuDebugMsgProlog(nuhgobj_file, 0x538)(msg_ghg_wrote_texanims, ptr - base);
        }

        header->shadow = 0;
        if (hobj->shadow) {
        struct nushadowdata_s *shadow;
        struct nushadowdata_s *outshadow;
        struct nushadowmesh_s *mesh;
        struct nushadowmesh_s *outmesh;
        char *ellipsoids;
        char *cylinders;
        char *meshes;
        char *verts;
        int nshadows;
        int nellipsoids;
        int ncylinders;
        int nmeshes;
        int nnormals;
        int nverts;
        int count;

            shadow = hobj->shadow;
            nshadows = 0;
            nellipsoids = 0;
            ncylinders = 0;
            nmeshes = 0;
            nverts = 0;
            nnormals = 0;
            for (; shadow->last != 0xFF; shadow++) {
                nshadows++;
                nellipsoids += shadow->numellipsoids;
                ncylinders += shadow->numcylinders;
                nmeshes += shadow->nummeshes;
                if (shadow->nummeshes) {
                    for (i = 0; i < shadow->nummeshes; i++) {
                        mesh = &shadow->meshes[i];
                        count = mesh->second->count;
                        nnormals += count;
                        for (j = 0; j < count; j++) {
                            nverts += mesh->first[j].count;
                        }
                    }
                }
            }
            ptr = (char *)(((unsigned int)ptr + 0xF) & ~0xF);
            outshadow = (struct nushadowdata_s *)ptr;
            ptr += (nshadows + 1) * sizeof(struct nushadowdata_s);
            memcpy(outshadow, hobj->shadow, (nshadows + 1) * sizeof(struct nushadowdata_s));
            header->shadow = (char *)outshadow - base;
            if (nellipsoids) {
                ptr = (char *)(((unsigned int)ptr + 0xF) & ~0xF);
                ellipsoids = ptr;
                ptr += nellipsoids * 0x30;
            }
            if (ncylinders) {
                ptr = (char *)(((unsigned int)ptr + 0xF) & ~0xF);
                cylinders = ptr;
                ptr += ncylinders * 0x40;
            }
            if (nmeshes) {
                ptr = (char *)(((unsigned int)ptr + 0xF) & ~0xF);
                meshes = ptr;
                ptr += nmeshes * sizeof(struct nushadowmesh_s);
            }
            if (nnormals + nverts) {
                ptr = (char *)(((unsigned int)ptr + 0xF) & ~0xF);
                verts = ptr;
                ptr += (nnormals + nverts) * sizeof(struct nushadowvtx_s);
            }
            for (shadow = hobj->shadow; shadow->last != 0xFF; shadow++, outshadow++) {
                if (shadow->numellipsoids) {
                    outshadow->ellipsoids = (void *)(ellipsoids - base);
                    memcpy(ellipsoids, shadow->ellipsoids, shadow->numellipsoids * 0x30);
                    ellipsoids += shadow->numellipsoids * 0x30;
                }
                if (shadow->numcylinders) {
                    outshadow->cylinders = (void *)(cylinders - base);
                    memcpy(cylinders, shadow->cylinders, shadow->numcylinders * 0x40);
                    cylinders += shadow->numcylinders * 0x40;
                }
                if (shadow->nummeshes) {
                    outshadow->meshes = (struct nushadowmesh_s *)(meshes - base);
                    outmesh = (struct nushadowmesh_s *)meshes;
                    memcpy(meshes, shadow->meshes, shadow->nummeshes * sizeof(struct nushadowmesh_s));
                    meshes += shadow->nummeshes * sizeof(struct nushadowmesh_s);
                    mesh = shadow->meshes;
                    for (j = 0; j < shadow->nummeshes; j++) {
                        count = mesh->second->count;
                        outmesh->first = (struct nushadowvtx_s *)(verts - base);
                        memcpy(verts, mesh->first, count * sizeof(struct nushadowvtx_s));
                        verts += count * sizeof(struct nushadowvtx_s);
                        nverts = 0;
                        for (k = 0; k < count; k++) {
                            nverts += mesh->first[k].count;
                        }
                        outmesh->second = (struct nushadowvtx_s *)(verts - base);
                        memcpy(verts, mesh->second, nverts * sizeof(struct nushadowvtx_s));
                        verts += nverts * sizeof(struct nushadowvtx_s);
                        mesh++;
                        outmesh++;
                    }
                }
            }
            NuDebugMsgProlog(nuhgobj_file, 0x5C4)(msg_ghg_wrote_shadow, ptr - base);
        }

        header->size = ptr - base;
        fh = NuFileOpen(file, NUFILE_WRITE);
        if (fh) {
            NuFileWrite(fh, base, ptr - base);
            NuFileClose(fh);
        }
        NuMemFreeFn(base, nuhgobj_file, 0x5D3);
    }
}

struct NUHGOBJ_s *NuGHGRead(union variptr_u *buffer, char *file) {
    struct nughgheader_s *header;
    struct NUHGOBJ_s *hobj;
    struct NUHLAYERDATA_s *layer;
    struct nugobj_s **gobjs;
    struct nuhcollide_s *collide;
    struct nuhcollideentry_s *entry;
    struct nutexanim_s *anim;
    struct nushadowdata_s *shadow;
    struct nushadowmesh_s *mesh;
    struct numtl_s *mtl;
    int *texoffsets;
    void *prog;
    union variptr_u ptr;
    int base;
    int i;
    int j;

    hobj = 0;
    ptr.intaddr = (buffer->intaddr + 0xF) & ~0xF;
    base = ptr.intaddr;
    if (NuFileLoadBuffer(file, ptr.voidptr, 0x7FFFFFFF) > 0) {
        header = ptr.voidptr;
        if (header->version < nughg_version) {
            NuErrorProlog(nuhgobj_file, 0x61D)(msg_ghg_file_old, header->version, nughg_version);
        }
        if (header->version > nughg_version) {
            NuErrorProlog(nuhgobj_file, 0x61F)(msg_ghg_loader_old, header->version, nughg_version);
        }
        NuDebugMsgProlog(nuhgobj_file, 0x622)(msg_ghg_reading, file);
        hobj = (struct NUHGOBJ_s *)((ptr.intaddr + ((struct nughgheader_s *)ptr.voidptr)->size + 0xF) & ~0xF);
        ptr.voidptr = &hobj[1];
        memset(hobj, 0, sizeof(struct NUHGOBJ_s));
        hobj->type = 1;
        if (header->tids) {
            hobj->tids = (short *)(base + header->tids);
            hobj->numtid = header->numtid;
            texoffsets = (int *)(base + header->tids);
            for (i = 0; i < hobj->numtid; i++) {
                hobj->tids[i] = NuTexCreateNative((void *)(base + texoffsets[i]));
            }
        }
        if (header->mtls) {
            hobj->mtls = (struct numtl_s **)(base + header->mtls);
            hobj->nummtl = header->nummtl;
            for (i = 0; i < hobj->nummtl; i++) {
                hobj->mtls[i] = (struct numtl_s *)(base + (int)hobj->mtls[i]);
                mtl = hobj->mtls[i];
                if (mtl->tid) {
                    mtl->tid = hobj->tids[mtl->tid - 1];
                }
                NuDisableVBlank();
                NuMtlAdd(hobj->mtls[i]);
                if (!NuMtlReadEvent(hobj->mtls[i])) {
                    NuMtlUpdate(hobj->mtls[i]);
                }
                NuEnableVBlank();
            }
        }
        hobj->scale = header->scale;
        hobj->string_table = (char *)(base + header->string_table);
        hobj->string_table_size = header->string_table_size;
        hobj->num_joints = header->num_joints;
        hobj->joints = (struct NUJOINTDATA_s *)(base + header->joints);
        hobj->T = (struct numtx_s *)(base + header->T);
        hobj->INV_WT = (struct numtx_s *)(base + header->INV_WT);
        hobj->num_joint_ixs = header->num_joint_ixs;
        hobj->joint_ixs = (u8 *)(base + header->joint_ixs);
        for (i = 0; i < hobj->num_joints; i++) {
            if (hobj->joints[i].name) {
                hobj->joints[i].name = NU_OFFSET_PTR(hobj->joints[i].name, base);
            }
        }
        if (header->num_poi_ixs) {
            hobj->num_poi_ixs = header->num_poi_ixs;
            hobj->poi_ixs = (u8 *)(base + header->poi_ixs);
        }
        if (header->num_pois) {
            hobj->num_pois = header->num_pois;
            hobj->pois = (struct NUPOINTOFINTEREST_s *)(base + header->pois);
            for (i = 0; i < hobj->num_pois; i++) {
                if (hobj->pois[i].name) {
                    hobj->pois[i].name = NU_OFFSET_PTR(hobj->pois[i].name, base);
                }
            }
        }
        layer = (struct NUHLAYERDATA_s *)(base + header->layers);
        hobj->layers = layer;
        hobj->num_layers = header->num_layers;
        for (i = 0; i < hobj->num_layers; i++) {
            if (layer->name) {
                layer->name = NU_OFFSET_PTR(layer->name, base);
            }
            if (layer->gobjs) {
                gobjs = layer->gobjs = NU_OFFSET_PTR(layer->gobjs, base);
                for (j = 0; j < hobj->num_joints; j++) {
                    if (gobjs[j]) {
                        NuGHGFixupGobj(base, gobjs[j] = NU_OFFSET_PTR(gobjs[j], base), hobj);
                    }
                }
            }
            if (layer->skin_object) {
                NuGHGFixupGobj(base, layer->skin_object = NU_OFFSET_PTR(layer->skin_object, base), hobj);
            }
            if (layer->blend_gobjs) {
                gobjs = layer->blend_gobjs = NU_OFFSET_PTR(layer->blend_gobjs, base);
                for (j = 0; j < hobj->num_joints; j++) {
                    if (gobjs[j]) {
                        NuGHGFixupGobj(base, gobjs[j] = NU_OFFSET_PTR(gobjs[j], base), hobj);
                    }
                }
            }
            if (layer->blend_skin_object) {
                NuGHGFixupGobj(base, layer->blend_skin_object = NU_OFFSET_PTR(layer->blend_skin_object, base), hobj);
            }
            layer++;
        }
        hobj->sphere_radius = header->sphere_radius;
        hobj->sphere_y = header->sphere_y;
        hobj->bbox_min = header->bbox_min;
        hobj->bbox_max = header->bbox_max;
        hobj->cyl_y = header->cyl_y;
        hobj->cyl_height = header->cyl_height;
        hobj->cyl_radius = header->cyl_radius;
        hobj->collide = 0;
        if (header->collide) {
            collide = hobj->collide = NU_OFFSET_PTR(header->collide, base);
            collide->entries = NU_OFFSET_PTR(collide->entries, base);
            for (i = 0; i < collide->nentries; i++) {
                entry = &collide->entries[i];
                entry->data = NU_OFFSET_PTR(entry->data, base);
            }
        }
        hobj->numtexanims = header->numtexanims;
        if (hobj->numtexanims) {
            hobj->texanims = (struct nutexanim_s *)(base + header->texanims);
            for (i = 0; i < hobj->numtexanims; i++) {
                anim = &hobj->texanims[i];
                anim->tids = (short *)(base + (int)anim->tids);
                anim->prev = 0;
                anim->succ = 0;
                for (j = 0; j < anim->numtids; j++) {
                    anim->tids[j] = hobj->tids[anim->tids[j]];
                }
                anim->mtl = hobj->mtls[(int)anim->mtl];
                anim->scriptname = NU_OFFSET_PTR(anim->scriptname, base);
                anim->ntaname = NU_OFFSET_PTR(anim->ntaname, base);
                prog = NuTexAnimProgFind(anim->scriptname);
                ptr.intaddr = (ptr.intaddr + 0xF) & ~0xF;
                anim->env = NuTexAnimEnvCreate(&ptr, anim->mtl, anim->tids, prog);
            }
            for (j = 0; j < hobj->numtexanims - 1; j++) {
                hobj->texanims[j].succ = &hobj->texanims[j] + 1;
                hobj->texanims[j + 1].prev = &hobj->texanims[j];
            }
            NuTexAnimAddList(hobj->texanims);
        }
        hobj->shadow = 0;
        if (header->shadow) {
            shadow = hobj->shadow = NU_OFFSET_PTR(header->shadow, base);
            for (; shadow->last != 0xFF; shadow++) {
                if (shadow->numellipsoids) {
                    shadow->ellipsoids = NU_OFFSET_PTR(shadow->ellipsoids, base);
                }
                if (shadow->numcylinders) {
                    shadow->cylinders = NU_OFFSET_PTR(shadow->cylinders, base);
                }
                if (shadow->nummeshes) {
                    mesh = shadow->meshes = NU_OFFSET_PTR(shadow->meshes, base);
                    for (i = 0; i < shadow->nummeshes; i++) {
                        mesh->second = NU_OFFSET_PTR(mesh->second, base);
                        mesh->first = NU_OFFSET_PTR(mesh->first, base);
                        mesh++;
                    }
                }
            }
        }
        ptr.intaddr = (ptr.intaddr + 0xF) & ~0xF;
        NuDebugMsgProlog(nuhgobj_file, 0x70C)(msg_ghg_read_size, file, ptr.u8 - buffer->u8);
        *buffer = ptr;
    }
    return hobj;
}
