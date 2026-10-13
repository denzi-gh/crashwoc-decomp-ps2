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

typedef float f32;
typedef unsigned char u8;

struct nuvec_s {
    f32 x;
    f32 y;
    f32 z;
};

struct numtx_s {
    f32 _00, _01, _02, _03;
    f32 _10, _11, _12, _13;
    f32 _20, _21, _22, _23;
    f32 _30, _31, _32, _33;
};

struct numtl_s;
struct SkinDmaTag;

struct nugeom_s {
    struct nugeom_s *next;
    struct numtl_s *mtl;
    void *dmastream;
    struct SkinDmaTag *skindma;
};

struct nugobj_s {
    struct nugobj_s *sysnext;
    struct nugobj_s *syslast;
    int type;
    struct nugeom_s *geom;
    char pad10[0x3C];
    struct nugobj_s *next_gobj;
};

struct NUPOINTOFINTEREST_s {
    struct numtx_s offset;
    int id;
    u8 parent_joint;
    char pad45[0x0B];
};

struct nuhcollideentry_s {
    int type;
    void *data;
};

struct nuhcollide_s {
    unsigned int nentries;
    struct nuhcollideentry_s *entries;
};

struct NUHGOBJ_s {
    short *tids;
    int numtid;
    struct numtl_s **mtls;
    int nummtl;
    int type;
    char pad14[0x0C];
    u8 *joint_ixs;
    char pad24[0x04];
    struct NUPOINTOFINTEREST_s *pois;
    u8 *poi_ixs;
    char pad30[0x08];
    f32 scale;
    char pad3C[0x04];
    f32 sphere_radius;
    f32 sphere_y;
    struct nuvec_s bbox_min;
    struct nuvec_s bbox_max;
    f32 cyl_y;
    f32 cyl_height;
    f32 cyl_radius;
    struct nuhcollide_s *collide;
    char pad70[0x04];
    void *texanims;
    char pad78[0x05];
    u8 num_joint_ixs;
    char pad7E[0x02];
    u8 num_poi_ixs;
    char pad81[0x03];
};

extern void *NuMemAllocFn(int size, char *file, int line);
extern void *memset(void *s, int c, unsigned int n);
extern int NuFileBeginBlkRead(int fh, int blockid);
extern void NuFileEndBlkRead(int fh);
extern int NuFileGetBlkSize(int fh);
extern int NuFileRead(int fh, void *buf, int size);
extern void NuTexAnimRemoveList(void *list);
extern void NuMtlDestroy(struct numtl_s *mtl);
extern void NuTexDestroy(int tid);
extern void NuMtxMulVU0(struct numtx_s *dst, struct numtx_s *a, struct numtx_s *b);
extern void NuVecInvMtxTransform(struct nuvec_s *dst, struct nuvec_s *src, struct numtx_s *mtx);
extern void NuVecMtxTransform(struct nuvec_s *dst, struct nuvec_s *src, struct numtx_s *mtx);
extern f32 NuLineToPointDistSqr(struct nuvec_s *p0, struct nuvec_s *p1, struct nuvec_s *point);
extern void NuCvtSknInstFix(struct SkinDmaTag *tag);
extern void NuHGobjDestroyDynamic(struct NUHGOBJ_s *hobj);
extern void ReadNuIFFHGobjSet(int fh, struct NUHGOBJ_s *hobj);

extern char D_00615290[];

#define nuhgobj_file D_00615290

#define NU_RELPTR(p) ((p) ? (void *)((char *)&(p) + (int)(p)) : 0)
#define NU_OFFSET_PTR(off, base) ((void *)((int)(off) + (base)))


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


struct NUHGOBJ_s *NuHGobjReadForCutScene(int fh, struct NUHGOBJ_s *shared) {
    struct NUHGOBJ_s *hobj;

    hobj = NuMemAllocFn(sizeof(struct NUHGOBJ_s), nuhgobj_file, 0xE5);
    memset(hobj, 0, sizeof(struct NUHGOBJ_s));
    hobj->type = 0;
    hobj->scale = 1.0f;
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


void NuGHGFixupGobj(int base, struct nugobj_s *gobj, struct NUHGOBJ_s *hobj) {
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
