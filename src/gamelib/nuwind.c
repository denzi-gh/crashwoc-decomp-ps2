/*
 * Unit: gamelib/nuwind
 *
 * Functions:
 *   0x001b51e0 NuWindCreate
 *   0x001b5498 NuWindUpdate
 *   0x001b5c80 NuWindInit
 *   0x001b5c98 NuWindDraw
 *   0x001b5db0 NuWindRand
 *   0x001b5dd0 NuWindAllocateGrp
 *   0x001b5e08 NuWindFreeGrp
 *   0x001b5e20 NuWindAllocMtxs
 *   0x001b5e58 NuWindFreeMtxs
 */

struct nuvec_s {
    float x;
    float y;
    float z;
};

struct nuvec4_s {
    float x;
    float y;
    float z;
    float w;
};

struct numtx_s {
    float _00, _01, _02, _03;
    float _10, _11, _12, _13;
    float _20, _21, _22, _23;
    float _30, _31, _32, _33;
};

struct nucamera_s {
    struct numtx_s mtx;
    float fov;
    float aspect;
    float nearclip;
    float farclip;
};

struct nuinstance_s {
    struct numtx_s mtx;
    int objid;
};

struct nugobj_s;

struct nugscn_s {
    int *tids;
    int numtid;
    void *mtls;
    int nummtl;
    int numgobj;
    struct nugobj_s **gobjs;
};

struct nuwindgrp_s {
    struct nuinstance_s *instance;
    struct numtx_s *mtxs;
    float centrex;
    float centrey;
    float centrez;
    char drawn;
    char visible;
    short count;
    int reactive;
    float wind;
    float height;
    float radiussq;
};

extern void NuMtxSetIdentity(struct numtx_s *m);
extern void NuMtxPreRotateY(struct numtx_s *m, int angle);
extern int NuRndrGScnObj(struct nugobj_s *gobj, struct numtx_s *m);
extern float NuFsqrt(float x);

extern float NuTrigTable[];
extern struct nucamera_s global_camera;

extern int NuWindQS;
extern int NuWindGCount;
extern int NuWindMtxIndex;
extern int NuWindWave;
extern int NuWindDir;
extern int NuWindDir2;
extern struct numtx_s NuWindMtxs[1024];
extern struct nuwindgrp_s NuWindGroup[64];


inline int NuWindRand(void)
{
    NuWindQS = (NuWindQS * 0x24CD + 1) & 0xFFFF;
    return NuWindQS;
}


inline struct nuwindgrp_s *NuWindAllocateGrp(void)
{
    if (NuWindGCount < 64) {
        NuWindGCount++;
        return &NuWindGroup[NuWindGCount - 1];
    }
    return 0;
}


inline void NuWindFreeGrp(struct nuwindgrp_s *grp)
{
    if (grp) {
        NuWindGCount--;
    }
}


inline struct numtx_s *NuWindAllocMtxs(int count)
{
    int end;

    end = count + NuWindMtxIndex;
    if (end < 1024) {
        NuWindMtxIndex = end;
        return &NuWindMtxs[end - count];
    }
    return 0;
}


inline void NuWindFreeMtxs(struct numtx_s *mtxs, int count)
{
    if (mtxs) {
        NuWindMtxIndex -= count;
    }
}


struct nuwindgrp_s *NuWindCreate(struct nuinstance_s *instance, struct nuvec4_s *points, short count,
                                 float wind, float height, int reactive)
{
    struct nuwindgrp_s *grp;
    struct numtx_s *mtx;
    float minx, miny, minz;
    float maxx, maxy, maxz;
    float x, y, z;
    int i;

    grp = NuWindAllocateGrp();
    mtx = NuWindAllocMtxs(count);
    if (grp != 0 && mtx != 0) {
        grp->instance = instance;
        grp->height = height;
        grp->reactive = reactive;
        grp->mtxs = mtx;
        grp->count = count;
        grp->wind = wind;
        minx = miny = minz = 10000000.0f;
        maxx = maxy = maxz = -10000000.0f;
        for (i = 0; i < count; i++, mtx++) {
            NuMtxSetIdentity(mtx);
            NuMtxPreRotateY(mtx, NuWindRand());
            mtx->_11 = points[i].w;
            mtx->_00 *= points[i].w;
            mtx->_02 *= points[i].w;
            mtx->_20 *= points[i].w;
            mtx->_22 *= points[i].w;
            mtx->_30 = points[i].x;
            mtx->_31 = points[i].y;
            mtx->_32 = points[i].z;
            mtx->_33 = wind * points[i].w;
            if (points[i].x < minx) {
                minx = points[i].x;
            }
            if (points[i].y < miny) {
                miny = points[i].y;
            }
            if (points[i].z < minz) {
                minz = points[i].z;
            }
            if (maxx < points[i].x) {
                maxx = points[i].x;
            }
            if (maxy < points[i].y) {
                maxy = points[i].y;
            }
            if (maxz < points[i].z) {
                maxz = points[i].z;
            }
        }
        x = (maxx + minx) * 0.5f;
        y = (maxy + miny) * 0.5f;
        z = (maxz + minz) * 0.5f;
        grp->centrex = x;
        grp->centrey = y;
        grp->centrez = z;
        grp->radiussq = x * x + y * y + z * z + 1.0f;
        return grp;
    }
    NuWindFreeGrp(grp);
    NuWindFreeMtxs(mtx, count);
    return 0;
}


static inline int NuWindInRange(struct nuwindgrp_s *grp, struct nuvec_s *pos)
{
    float dx, dy, dz;

    dx = grp->centrex - pos->x;
    dy = grp->centrey - pos->y;
    dz = grp->centrez - pos->z;
    if (dx * dx + dy * dy + dz * dz < grp->radiussq) {
        return 1;
    }
    return 0;
}


void NuWindUpdate(struct nuvec_s *pos)
{
    struct nuwindgrp_s *grp;
    struct numtx_s *mtx;
    struct nuvec_s wind;
    float gust;
    float dx, dy, dz;
    float above, bend, limit, radius;
    float swayx, swayz, pushx, pushz, mag;
    int i, j;

    NuWindDir += 0x85;
    NuWindDir2 += 0x179;
    gust = NuTrigTable[NuWindDir & 0xFFFF] * 8192.0f;
    wind.x = NuTrigTable[(int)(gust + 16384.0f) & 0xFFFF] * 0.75f
           + NuTrigTable[(NuWindDir2 + 0x4000) & 0xFFFF] * 0.15f;
    wind.z = NuTrigTable[(int)gust & 0xFFFF] * 0.75f - NuTrigTable[NuWindDir2 & 0xFFFF] * 0.15f;
    NuWindWave += 0x1F5;

    grp = NuWindGroup;
    for (i = 0; i < NuWindGCount; i++, grp++) {
        dx = grp->centrex - global_camera.mtx._30;
        dy = grp->centrey - global_camera.mtx._31;
        dz = grp->centrez - global_camera.mtx._32;
        if (dx * dx + dy * dy + dz * dz < global_camera.farclip * global_camera.farclip + grp->radiussq) {
            grp->visible = 1;
            if (grp->drawn) {
                mtx = grp->mtxs;
                if (!grp->reactive || !NuWindInRange(grp, pos)) {
                    for (j = 0; j < grp->count; j++, mtx++) {
                        float phase;

                        phase = NuWindWave + (mtx->_30 + mtx->_32) * 8192.0f;
                        swayx = mtx->_33 * wind.x * (NuTrigTable[(int)phase & 0xFFFF] * 0.5f + 1.0f);
                        swayz = mtx->_33 * wind.z * (NuTrigTable[(int)(phase + 16384.0f) & 0xFFFF] * 0.5f + 1.0f);
                        mtx->_10 += (swayx - mtx->_10) * 0.2f;
                        mtx->_12 += (swayz - mtx->_12) * 0.2f;
                        mag = mtx->_10 * mtx->_10 + mtx->_12 * mtx->_12;
                        mag = 1.0f - NuFsqrt(mag) * 0.35f;
                        if (mag < 0.05f) {
                            mag = 0.05f;
                        }
                        mtx->_10 *= mag;
                        mtx->_12 *= mag;
                        mtx->_23 = 1.0f / mag;
                        if (mag < 0.19f) {
                            mag = 0.19f;
                        }
                        mtx->_11 = mtx->_33 / grp->wind * mag;
                    }
                } else {
                    for (j = 0; j < grp->count; j++, mtx++) {
                        above = pos->y - mtx->_31;
                        if (grp->height * mtx->_33 / grp->wind < above) {
                            radius = 0.0f;
                            bend = mtx->_33 * 0.2f / grp->wind;
                        } else {
                            radius = 0.45f;
                            if (above < 0.0f) {
                                bend = mtx->_33 * 0.2f / grp->wind;
                            } else {
                                bend = (above * 0.5f + 0.2f) * mtx->_33 / grp->wind;
                            }
                        }
                        limit = mtx->_33 * 0.2f / grp->wind;
                        if (limit < mtx->_11) {
                            mtx->_10 *= mtx->_23;
                            mtx->_12 *= mtx->_23;
                            pushx = mtx->_10 * bend + mtx->_30 - pos->x;
                            pushz = mtx->_12 * bend + mtx->_32 - pos->z;
                            mag = pushx * pushx + pushz * pushz;
                            if (radius * radius < mag) {
                                float phase;

                                phase = NuWindWave + (mtx->_30 + mtx->_32) * 8192.0f;
                                swayx = mtx->_33 * wind.x * (NuTrigTable[(int)phase & 0xFFFF] * 0.5f + 1.0f);
                                swayz = mtx->_33 * wind.z
                                      * (NuTrigTable[(int)(phase + 16384.0f) & 0xFFFF] * 0.5f + 1.0f);
                                mtx->_10 += (swayx - mtx->_10) * 0.2f;
                                mtx->_12 += (swayz - mtx->_12) * 0.2f;
                            } else {
                                float phase, tipx, tipz, twist;

                                if (mag == 0.0f) {
                                    pushz = 0.0f;
                                    pushx = radius;
                                } else {
                                    mag = radius / NuFsqrt(mag);
                                    pushx *= mag;
                                    pushz *= mag;
                                }
                                mtx->_10 += ((pushx + pos->x - mtx->_30) / bend - mtx->_10) * 0.2f;
                                mtx->_12 += ((pushz + pos->z - mtx->_32) / bend - mtx->_12) * 0.2f;
                                phase = NuWindWave + (mtx->_30 + mtx->_32) * 8192.0f;
                                swayx = mtx->_33 * wind.x * (NuTrigTable[(int)phase & 0xFFFF] * 0.5f + 1.0f);
                                swayz = mtx->_33 * wind.z
                                      * (NuTrigTable[(int)(phase + 16384.0f) & 0xFFFF] * 0.5f + 1.0f);
                                mtx->_10 += (swayx - mtx->_10) * 0.05f;
                                mtx->_12 += (swayz - mtx->_12) * 0.05f;
                                tipx = swayx * bend + mtx->_30 - pos->x;
                                tipz = swayz * bend + mtx->_32 - pos->z;
                                twist = (pushx * tipz - pushz * tipx) * -2048.0f;
                                mtx->_10 = mtx->_10 * NuTrigTable[(int)(twist + 16384.0f) & 0xFFFF]
                                         - mtx->_12 * NuTrigTable[(int)twist & 0xFFFF];
                                mtx->_12 = mtx->_10 * NuTrigTable[(int)twist & 0xFFFF]
                                         + mtx->_12 * NuTrigTable[(int)(twist + 16384.0f) & 0xFFFF];
                            }
                            mag = mtx->_10 * mtx->_10 + mtx->_12 * mtx->_12;
                            mag = 1.0f - NuFsqrt(mag) * 0.35f;
                            if (mag < 0.05f) {
                                mag = 0.05f;
                            }
                            mtx->_10 *= mag;
                            mtx->_12 *= mag;
                            mtx->_23 = 1.0f / mag;
                            if (mag < 0.19f) {
                                mag = 0.19f;
                            }
                            mtx->_11 = mtx->_33 / grp->wind * mag;
                        } else {
                            mtx->_11 = limit;
                        }
                    }
                }
            }
        } else {
            grp->visible = 0;
        }
    }
}


void NuWindInit(void)
{
    NuWindDir = 0;
    NuWindDir2 = 0;
    NuWindWave = 0;
    NuWindGCount = 0;
    NuWindMtxIndex = 0;
}


void NuWindDraw(struct nugscn_s *scene)
{
    struct nuwindgrp_s *grp;
    struct numtx_s *mtx;
    float saved23, saved33;
    int i, j;

    grp = NuWindGroup;
    for (i = 0; i < NuWindGCount; i++, grp++) {
        if (grp->visible) {
            grp->drawn = 0;
            mtx = grp->mtxs;
            for (j = 0; j < grp->count; j++, mtx++) {
                saved23 = mtx->_23;
                saved33 = mtx->_33;
                mtx->_23 = 0.0f;
                mtx->_33 = 1.0f;
                if (NuRndrGScnObj(scene->gobjs[grp->instance->objid], mtx)) {
                    grp->drawn = 1;
                }
                mtx->_23 = saved23;
                mtx->_33 = saved33;
            }
        }
    }
}
