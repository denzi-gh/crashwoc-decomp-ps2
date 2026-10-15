/*
 * Unit: gamelib/nubridge
 *
 * Functions:
 *   0x001b2870 NuBridgeCreate
 *   0x001b2ee8 NuBridgeUpdate
 *   0x001b4660 NuBridgeStreamRope
 *   0x001b4c58 NuBridgeDraw
 *   0x001b5028 NuBridgeInit
 *   0x001b5030 NuBridgeOn
 *   0x001b5038 NuBridgeRegisterBaseScene
 *   0x001b5040 NuBridgeAlloc
 *   0x001b5078 NuBridgeLookupInstanceIndex
 *   0x001b50c8 NuBrdigeDrawRope
 */

#include "nu_libc.h"
#include "nu_types.h"

#include "numath/numathtypes.h"
#include "numath/numtx.h"
#include "numath/nutrig.h"
#include "nu3d/numtl.h"
#include "nu3d/nustream.h"
#include "nu3d/nucamera.h"
#include "nu3d/nugscn.h"

#define BRIDGE_MAX 8
#define BRIDGE_MAX_SECTIONS 24
#define BRIDGE_MAX_ROPE_POINTS 512
#define BRIDGE_MAX_ROPE_BREAKS 8
#define ROPE_BATCH_VERTS 42
#define BRIDGE_CONTACT_FRAMES 5

#define SQR(x) ((x) * (x))
#define PLANK_SUM(p, c) ((p).left.c + (p).right.c)
#define PLANK_SPAN(p, c) ((p).right.c - (p).left.c)

#define ROPE_SPRING(v, near, here, k) ((v) += fabs((near) - (here)) * ((near) - (here)) * (k) * 0.5 + ((near) - (here)) * (k))


union giftag_u {
    u64 dw[2];
    u32 w[4];
    struct {
        u64 nloop : 15;
        u64 eop : 1;
        u64 pad16 : 30;
        u64 pre : 1;
        u64 prim : 11;
        u64 flg : 2;
        u64 nreg : 4;
        u64 regs;
    } tag;
};

struct bridgeplank_s {
    struct nuvec_s left;
    struct nuvec_s right;
};

struct nubridge_s {
    struct bridgeplank_s pos[BRIDGE_MAX_SECTIONS];
    struct bridgeplank_s vel[BRIDGE_MAX_SECTIONS];
    struct numtx_s mtx[BRIDGE_MAX_SECTIONS];
    struct nuinstance_s *inst[BRIDGE_MAX_SECTIONS];
    struct nuinstance_s *ipost;
    struct nuvec_s centre;
    float rangesq;
    short platinst[BRIDGE_MAX_SECTIONS];
    char active;
    char visible;
    char sections;
    char postint;
    int colour;
    short contact;
    short yang;
    float width;
    float tension;
    float gravity;
    float damp;
    float plrweight;
    float postw;
    float posth;
    char padB4C[0xB50 - 0xB4C];
};

extern struct nubridge_s Bridges[BRIDGE_MAX];
extern int BridgeFree;
extern int NuBridgeProc;
extern struct nugscn_s *NuBridge_base_scene;
extern struct nurndrstream_s *rndrstream_fx;
extern u8 *rndrstream_free;
extern struct numtx_s numtx_identity;
extern struct nucamera_s global_camera;

extern char D_0061A590[];
#define bridge_sections_msg D_0061A590

extern float NuFsqrt(float x);
extern void NuVec4MtxTransformVU0(struct nuvec4_s *dst, struct nuvec4_s *src, struct numtx_s *m);
extern int NewPlatInst(struct numtx_s *mtx, int instance);
extern void PlatInstRotate(int platinst, int rotate);
extern int PlatInstGetHit(int platinst);
extern int NuRndrGScnObj(void *gobj, struct numtx_s *m);
extern void NuRndrStreamLink(struct nurndrstream_s *rs);
extern u8 *vpDmaTag_Cnt(u8 *cur);
extern u8 *vpDmaTag_Close(u8 *cur);
extern u8 *vpDmaTag_Next(u8 *cur, int mode);

int NuBridgeStreamRope(struct nurndrstream_s *rs, struct nuvec_s *points, int npoints, int colour);


inline void NuBridgeInit(void) {
    BridgeFree = 0;
}


inline void NuBridgeOn(int on) {
    NuBridgeProc = on;
}


inline void NuBridgeRegisterBaseScene(struct nugscn_s *scene) {
    NuBridge_base_scene = scene;
}


inline struct nubridge_s *NuBridgeAlloc(void) {
    if (BridgeFree < BRIDGE_MAX) {
        return &Bridges[BridgeFree++];
    }
    return 0;
}


inline int NuBridgeLookupInstanceIndex(struct nuinstance_s *instance) {
    int i;

    if (NuBridge_base_scene == 0) {
        return -1;
    }
    for (i = 0; i < NuBridge_base_scene->numinstance; i++) {
        if (&NuBridge_base_scene->instances[i] == instance) {
            return i;
        }
    }
    return -1;
}


inline void NuBrdigeDrawRope(struct numtl_s *mtl, struct nuvec_s *left, struct nuvec_s *right,
                             int npoints, int *breaks, int colour) {
    int i;

    NuRndrStreamLink(rndrstream_fx);
    NuRndrStreamSetMisc(rndrstream_fx);
    NuRndrStreamAddMtl(rndrstream_fx, mtl);
    NuRndrSetLocalMtx(&numtx_identity, 2);
    NuRndrStreamSetViewMtxScissorClip(rndrstream_fx);
    NuRndrStreamSetLocalMtx(rndrstream_fx);
    NuRndrStreamFxMPG(1);
    for (i = 0; breaks[i] < breaks[i + 1]; i++) {
        NuBridgeStreamRope(rndrstream_fx, &left[breaks[i]], breaks[i + 1] - breaks[i], colour);
        NuBridgeStreamRope(rndrstream_fx, &right[breaks[i]], breaks[i + 1] - breaks[i], colour);
    }
    rndrstream_free = vpDmaTag_Next(rndrstream_fx->cur.b, 0);
}


struct nubridge_s *NuBridgeCreate(struct nuinstance_s **instance, struct nuinstance_s *ipost,
                                  struct nuvec_s *start, struct nuvec_s *end, float width,
                                  short yang, float tension, float damp, float gravity,
                                  float plrweight, int sections, float postw, float posth,
                                  int postint, int colour) {
    struct nubridge_s *bridge;
    struct nuvec_s delta;
    struct nuvec_s side;
    float scale;
    float t;
    int i;

    if (sections > BRIDGE_MAX_SECTIONS) {
        printf(bridge_sections_msg);
    }
    NuBridgeOn(1);
    bridge = NuBridgeAlloc();
    if (bridge != 0) {
        bridge->tension = tension;
        bridge->damp = damp;
        bridge->gravity = gravity;
        bridge->ipost = ipost;
        bridge->plrweight = plrweight;
        bridge->postw = postw;
        bridge->posth = posth;
        bridge->postint = postint;
        bridge->colour = colour;
        bridge->sections = sections;
        bridge->width = width;
        bridge->centre.x = (end->x + start->x) * 0.5f;
        bridge->centre.y = (end->y + start->y) * 0.5f;
        bridge->centre.z = (end->z + start->z) * 0.5f;
        delta.x = end->x - start->x;
        delta.y = end->y - start->y;
        delta.z = end->z - start->z;
        bridge->rangesq = SQR(delta.x * 0.5f) + SQR(delta.y * 0.5f)
                        + SQR(delta.z * 0.5f) + 1.0f;
        bridge->active = 0;
        scale = 1.0f / NuFsqrt(delta.x * delta.x + delta.z * delta.z);
        side.x = -delta.z * scale;
        side.y = 0.0f;
        side.z = delta.x * scale;
        bridge->yang = yang;
        for (i = 0; i < sections; i++) {
            bridge->inst[i] = instance[i];
            if (bridge->inst[i] != 0) {
                bridge->platinst[i] = NewPlatInst(&bridge->mtx[i], NuBridgeLookupInstanceIndex(instance[i]));
                PlatInstRotate(bridge->platinst[i], 1);
            } else {
                bridge->platinst[i] = -1;
            }
            NuMtxSetIdentity(&bridge->mtx[i]);
            NuMtxPreRotateY(&bridge->mtx[i], bridge->yang);
            t = i;
            bridge->mtx[i]._30 = delta.x * t / (sections - 1) + start->x;
            bridge->mtx[i]._31 = delta.y * t / (sections - 1) + start->y;
            bridge->mtx[i]._32 = delta.z * t / (sections - 1) + start->z;
            bridge->pos[i].left.x = delta.x * t / (sections - 1) + start->x - side.x * width * 0.5f;
            bridge->pos[i].left.y = delta.y * t / (sections - 1) + start->y;
            bridge->pos[i].left.z = delta.z * t / (sections - 1) + start->z - side.z * width * 0.5f;
            bridge->pos[i].right.x = delta.x * t / (sections - 1) + start->x + side.x * width * 0.5f;
            bridge->pos[i].right.y = delta.y * t / (sections - 1) + start->y;
            bridge->pos[i].right.z = delta.z * t / (sections - 1) + start->z + side.z * width * 0.5f;
            bridge->vel[i].left.x = 0.0f;
            bridge->vel[i].left.y = 0.0f;
            bridge->vel[i].left.z = 0.0f;
            bridge->vel[i].right.x = 0.0f;
            bridge->vel[i].right.y = 0.0f;
            bridge->vel[i].right.z = 0.0f;
            bridge->contact = 0;
        }
    }
    return bridge;
}


void NuBridgeUpdate(struct nuvec_s *playerpos) {
    struct nuvec_s dir;
    int i;
    struct nubridge_s *bridge;
    float t;
    float side;
    float len;
    float leftweight;
    float level;
    int seg;
    int j;

    if (NuBridgeProc) {
        bridge = Bridges;
        for (i = 0; i < BridgeFree; i++, bridge++) {
            if (SQR(bridge->centre.x - global_camera.mtx._30)
                + SQR(bridge->centre.y - global_camera.mtx._31)
                + SQR(bridge->centre.z - global_camera.mtx._32)
                < global_camera.farclip * global_camera.farclip + bridge->rangesq) {
                bridge->active = 1;
                if (bridge->visible) {
                    if (bridge->contact != 0) {
                        dir.x = (PLANK_SUM(bridge->pos[bridge->sections - 1], x)
                                 - bridge->pos[0].left.x - bridge->pos[0].right.x) * 0.5f;
                        dir.z = (PLANK_SUM(bridge->pos[bridge->sections - 1], z)
                                 - bridge->pos[0].left.z - bridge->pos[0].right.z) * 0.5f;
                        t = dir.x * (playerpos->x - PLANK_SUM(bridge->pos[0], x) * 0.5f)
                          + dir.z * (playerpos->z - PLANK_SUM(bridge->pos[0], z) * 0.5f);
                        t = t * (bridge->sections - 1) / (dir.x * dir.x + dir.z * dir.z);
                        if (t < 0.0f) {
                            t = 0.0f;
                        }
                        if (t > bridge->sections - 1.0f) {
                            t = bridge->sections - 1.0f;
                        }
                        seg = t;
                        t -= seg;
                        len = NuFsqrt(dir.x * dir.x + dir.z * dir.z);
                        side = 2.0f / (len * bridge->width);
                        side *= dir.z * (playerpos->x - PLANK_SUM(bridge->pos[0], x) * 0.5f)
                              + dir.x * (PLANK_SUM(bridge->pos[0], z) * 0.5f - playerpos->z);
                        if (side > 1.0f) {
                            side = 1.0f;
                        }
                        if (side < -1.0f) {
                            side = -1.0f;
                        }
                        leftweight = bridge->gravity * (3.0f - side) * 0.25f;
                        level = bridge->gravity * (side + 3.0f) * 0.25f;
                        if (t == 0.0f) {
                            if (seg > 0 && seg < bridge->sections - 1) {
                                bridge->vel[seg].left.y += leftweight * bridge->plrweight;
                                bridge->vel[seg].right.y += level * bridge->plrweight;
                            }
                        } else {
                            if (seg > 0 && seg < bridge->sections - 1) {
                                bridge->vel[seg].left.y += leftweight * bridge->plrweight * (1.0f - t);
                                bridge->vel[seg].right.y += level * bridge->plrweight * (1.0f - t);
                            }
                            if (seg + 1 < bridge->sections - 1) {
                                bridge->vel[seg + 1].left.y += leftweight * bridge->plrweight * t;
                                bridge->vel[seg + 1].right.y += level * bridge->plrweight * t;
                            }
                        }
                        bridge->contact--;
                    } else {
                        side = 0.0f;
                    }
                    if (PlatInstGetHit(bridge->platinst[0])) {
                        bridge->contact = BRIDGE_CONTACT_FRAMES;
                    }
                    if (PlatInstGetHit(bridge->platinst[bridge->sections - 1])) {
                        bridge->contact = BRIDGE_CONTACT_FRAMES;
                    }
                    for (j = 1; j < bridge->sections - 1; j++) {
                        bridge->vel[j].left.x += -bridge->vel[j].left.x * bridge->damp;
                        bridge->vel[j].left.y += bridge->gravity - bridge->vel[j].left.y * bridge->damp;
                        bridge->vel[j].left.z += -bridge->vel[j].left.z * bridge->damp;
                        bridge->vel[j].right.x += -bridge->vel[j].right.x * bridge->damp;
                        bridge->vel[j].right.y += bridge->gravity - bridge->vel[j].right.y * bridge->damp;
                        bridge->vel[j].right.z += -bridge->vel[j].right.z * bridge->damp;
                        if (PlatInstGetHit(bridge->platinst[j])) {
                            bridge->contact = BRIDGE_CONTACT_FRAMES;
                        }
                        ROPE_SPRING(bridge->vel[j].left.x, bridge->pos[j - 1].left.x, bridge->pos[j].left.x, bridge->tension);
                        ROPE_SPRING(bridge->vel[j].left.y, bridge->pos[j - 1].left.y, bridge->pos[j].left.y, bridge->tension);
                        ROPE_SPRING(bridge->vel[j].left.z, bridge->pos[j - 1].left.z, bridge->pos[j].left.z, bridge->tension);
                        ROPE_SPRING(bridge->vel[j].left.x, bridge->pos[j + 1].left.x, bridge->pos[j].left.x, bridge->tension);
                        ROPE_SPRING(bridge->vel[j].left.y, bridge->pos[j + 1].left.y, bridge->pos[j].left.y, bridge->tension);
                        ROPE_SPRING(bridge->vel[j].left.z, bridge->pos[j + 1].left.z, bridge->pos[j].left.z, bridge->tension);
                        ROPE_SPRING(bridge->vel[j].right.x, bridge->pos[j - 1].right.x, bridge->pos[j].right.x, bridge->tension);
                        ROPE_SPRING(bridge->vel[j].right.y, bridge->pos[j - 1].right.y, bridge->pos[j].right.y, bridge->tension);
                        ROPE_SPRING(bridge->vel[j].right.z, bridge->pos[j - 1].right.z, bridge->pos[j].right.z, bridge->tension);
                        ROPE_SPRING(bridge->vel[j].right.x, bridge->pos[j + 1].right.x, bridge->pos[j].right.x, bridge->tension);
                        ROPE_SPRING(bridge->vel[j].right.y, bridge->pos[j + 1].right.y, bridge->pos[j].right.y, bridge->tension);
                        ROPE_SPRING(bridge->vel[j].right.z, bridge->pos[j + 1].right.z, bridge->pos[j].right.z, bridge->tension);
                    }
                    for (j = 1; j < bridge->sections - 1; j++) {
                        bridge->pos[j].left.x += bridge->vel[j].left.x;
                        bridge->pos[j].left.y += bridge->vel[j].left.y;
                        bridge->pos[j].left.z += bridge->vel[j].left.z;
                        bridge->pos[j].right.x += bridge->vel[j].right.x;
                        bridge->pos[j].right.y += bridge->vel[j].right.y;
                        bridge->pos[j].right.z += bridge->vel[j].right.z;
                        NuMtxSetRotationY(&bridge->mtx[j], bridge->yang);
                        len = NuFsqrt(SQR(PLANK_SPAN(bridge->pos[j], x))
                                      + SQR(PLANK_SPAN(bridge->pos[j], z)));
                        NuMtxPreRotateX(&bridge->mtx[j], NuAtan2D(PLANK_SPAN(bridge->pos[j], y), len));
                        len = NuFsqrt(SQR(PLANK_SUM(bridge->pos[j + 1], x) - bridge->pos[j - 1].left.x
                                          - bridge->pos[j - 1].right.x)
                                      + SQR(PLANK_SUM(bridge->pos[j + 1], z) - bridge->pos[j - 1].left.z
                                            - bridge->pos[j - 1].right.z));
                        NuMtxPreRotateZ(&bridge->mtx[j], NuAtan2D(PLANK_SUM(bridge->pos[j + 1], y)
                                                                  - bridge->pos[j - 1].left.y - bridge->pos[j - 1].right.y, len));
                        bridge->mtx[j]._30 = PLANK_SUM(bridge->pos[j], x) * 0.5f;
                        bridge->mtx[j]._31 = PLANK_SUM(bridge->pos[j], y) * 0.5f;
                        bridge->mtx[j]._32 = PLANK_SUM(bridge->pos[j], z) * 0.5f;
                    }
                    NuMtxSetRotationY(&bridge->mtx[0], bridge->yang);
                    len = NuFsqrt(SQR(PLANK_SPAN(bridge->pos[0], x))
                                  + SQR(PLANK_SPAN(bridge->pos[0], z)));
                    NuMtxPreRotateX(&bridge->mtx[0], NuAtan2D(PLANK_SPAN(bridge->pos[0], y), len));
                    len = NuFsqrt(SQR(PLANK_SUM(bridge->pos[1], x) - bridge->pos[0].left.x - bridge->pos[0].right.x)
                                  + SQR(PLANK_SUM(bridge->pos[1], z) - bridge->pos[0].left.z - bridge->pos[0].right.z));
                    level = (PLANK_SUM(bridge->pos[0], y) + bridge->pos[1].left.y + bridge->pos[1].right.y) * 0.25f;
                    NuMtxPreRotateZ(&bridge->mtx[0], NuAtan2D(PLANK_SUM(bridge->pos[1], y) - (level + level), len));
                    bridge->mtx[0]._30 = PLANK_SUM(bridge->pos[0], x) * 0.5f;
                    bridge->mtx[0]._31 = level;
                    bridge->mtx[0]._32 = PLANK_SUM(bridge->pos[0], z) * 0.5f;
                    NuMtxSetRotationY(&bridge->mtx[bridge->sections - 1], bridge->yang);
                    len = NuFsqrt(SQR(PLANK_SPAN(bridge->pos[bridge->sections - 2], x))
                                  + SQR(PLANK_SPAN(bridge->pos[bridge->sections - 2], z)));
                    NuMtxPreRotateX(&bridge->mtx[0], NuAtan2D(PLANK_SPAN(bridge->pos[bridge->sections - 2], y), len));
                    len = NuFsqrt(SQR(PLANK_SUM(bridge->pos[bridge->sections - 1], x)
                                      - bridge->pos[bridge->sections - 2].left.x
                                      - bridge->pos[bridge->sections - 2].right.x)
                                  + SQR(PLANK_SUM(bridge->pos[bridge->sections - 1], z)
                                        - bridge->pos[bridge->sections - 2].left.z
                                        - bridge->pos[bridge->sections - 2].right.z));
                    level = (PLANK_SUM(bridge->pos[bridge->sections - 1], y) + bridge->pos[bridge->sections - 2].left.y
                             + bridge->pos[bridge->sections - 2].right.y) * 0.25f;
                    NuMtxPreRotateZ(&bridge->mtx[bridge->sections - 1],
                                    NuAtan2D(PLANK_SUM(bridge->pos[bridge->sections - 1], y) - (level + level), len));
                    bridge->mtx[bridge->sections - 1]._30 = PLANK_SUM(bridge->pos[bridge->sections - 1], x) * 0.5f;
                    bridge->mtx[bridge->sections - 1]._31 = level;
                    bridge->mtx[bridge->sections - 1]._32 = PLANK_SUM(bridge->pos[bridge->sections - 1], z) * 0.5f;
                }
            } else {
                bridge->active = 0;
            }
        }
    }
}

int NuBridgeStreamRope(struct nurndrstream_s *rs, struct nuvec_s *points, int npoints, int colour) {
    union giftag_u stripgif;
    union giftag_u headgif;
    struct nuvec_s side;
    struct nuvec_s up;
    struct nuvec_s tmp;
    struct nuvec_s *p0;
    struct nuvec_s *p1;
    float scale;
    int count;
    int base;
    int remaining;
    int i;

    side.x = points[1].z - points[0].z;
    side.y = 0.0f;
    side.z = points[0].x - points[1].x;
    scale = 0.05f / NuFsqrt(side.x * side.x + side.z * side.z);
    up.x = 0.0f;
    up.y = 0.05f;
    up.z = 0.0f;
    side.x *= scale;
    side.z *= scale;
    stripgif.dw[0] = 0x3002400000008000LL;
    stripgif.dw[1] = 0x512;
    headgif.dw[0] = 0x3002C00000008000LL;
    headgif.dw[1] = 0x512;
    remaining = (npoints - 1) * 16;
    rs->cur.b = vpDmaTag_Cnt(rs->cur.b);
    rs->cur.b[-14] = 0x12;
    base = 0;
    *((u32 *)rs->cur.b)++ = 0x6C010056;
    *((u32 *)rs->cur.b)++ = headgif.w[0];
    *((u32 *)rs->cur.b)++ = headgif.w[1];
    *((u32 *)rs->cur.b)++ = headgif.w[2];
    *((u32 *)rs->cur.b)++ = headgif.w[3];
    while (remaining != 0) {
        count = (remaining > ROPE_BATCH_VERTS) ? ROPE_BATCH_VERTS : remaining;
        count &= 0xF0;
        stripgif.tag.nloop = count;
        *((u32 *)rs->cur.b)++ = 0x6C0180D2;
        *((u32 *)rs->cur.b)++ = stripgif.w[0];
        *((u32 *)rs->cur.b)++ = stripgif.w[1];
        *((u32 *)rs->cur.b)++ = stripgif.w[2];
        *((u32 *)rs->cur.b)++ = stripgif.w[3];
        *((u32 *)rs->cur.b)++ = 0x6D018002;
        *((u16 *)rs->cur.b)++ = count * 3;
        *((u16 *)rs->cur.b)++ = 0;
        *((u16 *)rs->cur.b)++ = 0;
        *((u16 *)rs->cur.b)++ = 0;
        *((u32 *)rs->cur.b)++ = 0x01000103;
        *((u32 *)rs->cur.b)++ = (count << 16) | 0x6E00C005;
        for (i = 0; i < count; i += 4) {
            *((u32 *)rs->cur.b)++ = colour;
            *((u32 *)rs->cur.b)++ = colour;
            *((u32 *)rs->cur.b)++ = colour;
            *((u32 *)rs->cur.b)++ = colour;
        }
        *((u32 *)rs->cur.b)++ = 0x01000203;
        *((u32 *)rs->cur.b)++ = (count << 17) | 0x6C008003;
        for (i = 0; i < count; i += 4) {
            p0 = &points[(base + i) >> 4];
            p1 = p0 + 1;
            *((float *)rs->cur.b)++ = p0->x + side.x;
            *((float *)rs->cur.b)++ = p0->y + side.y;
            *((float *)rs->cur.b)++ = p0->z + side.z;
            *((u32 *)rs->cur.b)++ = 0x8000;
            *((u32 *)rs->cur.b)++ = 0x400;
            *((u32 *)rs->cur.b)++ = 0xC00;
            rs->cur.b += 8;
            *((float *)rs->cur.b)++ = p0->x + up.x;
            *((float *)rs->cur.b)++ = p0->y + up.y;
            *((float *)rs->cur.b)++ = p0->z + up.z;
            *((u32 *)rs->cur.b)++ = 0x8000;
            *((u32 *)rs->cur.b)++ = 0x5F0;
            *((u32 *)rs->cur.b)++ = 0xC00;
            rs->cur.b += 8;
            *((float *)rs->cur.b)++ = p1->x + side.x;
            *((float *)rs->cur.b)++ = p1->y + side.y;
            *((float *)rs->cur.b)++ = p1->z + side.z;
            *((u32 *)rs->cur.b)++ = 0;
            *((u32 *)rs->cur.b)++ = 0x400;
            *((u32 *)rs->cur.b)++ = 0xFF0;
            rs->cur.b += 8;
            *((float *)rs->cur.b)++ = p1->x + up.x;
            *((float *)rs->cur.b)++ = p1->y + up.y;
            *((float *)rs->cur.b)++ = p1->z + up.z;
            *((u32 *)rs->cur.b)++ = 0;
            *((u32 *)rs->cur.b)++ = 0x5F0;
            *((u32 *)rs->cur.b)++ = 0xFF0;
            rs->cur.b += 8;
            tmp = side;
            side.x = -up.x;
            side.y = -up.y;
            side.z = -up.z;
            up = tmp;
        }
        *((u32 *)rs->cur.b)++ = 0x01000101;
        *((u32 *)rs->cur.b)++ = 0x14000306;
        base += count;
        remaining -= count;
    }
    rs->cur.b = vpDmaTag_Close(rs->cur.b);
    return 1;
}


void NuBridgeDraw(struct nugscn_s *scene, struct numtl_s *mtl) {
    struct numtx_s postmtx;
    struct nuvec4_s v;
    struct nuvec4_s leftpost;
    struct nuvec4_s rightpost;
    struct nuvec_s left[BRIDGE_MAX_ROPE_POINTS];
    struct nuvec_s right[BRIDGE_MAX_ROPE_POINTS];
    int breaks[BRIDGE_MAX_ROPE_BREAKS];
    struct nubridge_s *bridge;
    int nbreaks;
    int npoints;
    int i;
    int j;

    if (NuBridgeProc) {
        bridge = Bridges;
        nbreaks = 0;
        breaks[0] = 0;
        npoints = 0;
        for (i = 0; i < BridgeFree; i++, bridge++) {
            if (bridge->active) {
                bridge->visible = 0;
                NuMtxSetRotationY(&postmtx, bridge->yang);
                leftpost.x = 0.0f;
                leftpost.y = 0.0f;
                leftpost.z = -bridge->postw;
                leftpost.w = 1.0f;
                rightpost.x = 0.0f;
                rightpost.y = 0.0f;
                rightpost.z = bridge->postw;
                rightpost.w = 1.0f;
                for (j = 0; j < bridge->sections; j++) {
                    if (bridge->inst[j] != 0
                        && NuRndrGScnObj(scene->gobjs[bridge->inst[j]->objid], &bridge->mtx[j])) {
                        bridge->visible = 1;
                    }
                    if (j % bridge->postint == 0 && bridge->ipost != 0) {
                        NuVec4MtxTransformVU0(&v, &leftpost, &bridge->mtx[j]);
                        if (npoints < BRIDGE_MAX_ROPE_POINTS) {
                            left[npoints].x = v.x;
                            left[npoints].y = v.y + bridge->posth;
                            left[npoints].z = v.z;
                        }
                        postmtx._30 = v.x;
                        postmtx._31 = v.y;
                        postmtx._32 = v.z;
                        NuRndrGScnObj(scene->gobjs[bridge->ipost->objid], &postmtx);
                        v.x = 0.0f;
                        v.y = 0.0f;
                        v.z = 1.0f;
                        v.w = 1.0f;
                        NuVec4MtxTransformVU0(&v, &rightpost, &bridge->mtx[j]);
                        if (npoints < BRIDGE_MAX_ROPE_POINTS) {
                            right[npoints].x = v.x;
                            right[npoints].y = v.y + bridge->posth;
                            right[npoints].z = v.z;
                            npoints++;
                        }
                        postmtx._30 = v.x;
                        postmtx._31 = v.y;
                        postmtx._32 = v.z;
                        NuRndrGScnObj(scene->gobjs[bridge->ipost->objid], &postmtx);
                    }
                }
                if (breaks[nbreaks] != npoints) {
                    nbreaks++;
                    breaks[nbreaks] = npoints;
                }
            }
        }
        breaks[nbreaks + 1] = npoints;
        if (npoints >= 2) {
            NuBrdigeDrawRope(mtl, left, right, npoints, breaks, Bridges[0].colour);
        }
    }
}
