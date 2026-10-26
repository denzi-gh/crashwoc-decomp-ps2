/*
 * Unit: gamelib/debris
 *
 * Functions:
 *   0x00182ca8 GenericDebinfoDmaTypeUpdate
 *   0x00183418 GenDebIndex
 *   0x00183660 GenDebIndexRadial
 *   0x001838d0 GenDebIndexRadialRotor
 *   0x00183b98 GenDebIndexSpheroid
 *   0x00183e58 GenDebIndexBounceY
 *   0x00184328 GenDebIndexBounceXZ
 *   0x00184790 GenDebIndexPos
 *   0x00184948 GenDebIndexPosRandTime
 *   0x00184b20 GenDebIndexWaterFall
 *   0x00184d68 GenDebIndexWaterFallSplash
 *   0x00184f70 CreateAlphaBlendTexture256
 *   0x00185208 SplashEffect
 *   0x00185388 SplashBodyEffect
 *   0x00185510 SetupDebris
 *   0x00185fa0 DebReAlloc2
 *   0x001864b8 DebFree
 *   0x001867d8 DebFreeInstantly
 *   0x00186960 DebrisStartOffset
 *   0x00186a68 AddVariableShotDebrisEffectMtx2
 *   0x00187098 AddDebrisEffect
 *   0x00187568 DebrisDraw
 *   0x00187800 Debris
 *   0x00188b40 DebrisCollisionCheck
 *   0x00188e08 AddDebEff
 *   0x00188eb0 DebrisOff
 *   0x00188ee0 DebrisOn
 *   0x00188f10 AddFiniteShotDebrisEffect
 *   0x00188f80 AddVariableShotDebrisEffect
 *   0x00189090 AddVariableShotDebrisEffectMtx
 *   0x001891a0 DebReAlloc
 *   0x00189200 DebrisOrientation
 *   0x001892f8 DebrisOrientationMtx
 *   0x001893b8 DebrisPosOrientationMtx
 *   0x001894a0 DebrisEmitterOrientation
 *   0x00189598 DebrisEmiterPos
 *   0x001895d0 DebrisReflectionOrientation
 *   0x00189620 DebrisSetTrigger
 *   0x00189660 DebrisSetGroupID
 *   0x00189690 DebrisSetup
 *   0x001896b0 DebrisSetRenderGroup
 *   0x001896b8 LookupDebrisEffect
 *   0x00189730 DebrisRegisterCutoffCameraVec
 *   0x00189738 CameraEmitterDistance
 *   0x00189768 DebrisFindAllOfType
 *   0x001897f8 FindDebrisEffectStack
 *   0x00189858 RemoveDebrisEffectFromStack
 *   0x001898a0 AddChunkControlToStack
 *   0x001898d0 SolveQuadratic
 *   0x001899b8 GenDebIndexSort
 *   0x001899d8 GenDebMomAdjFromPos
 *   0x00189a38 GenDebMomAdjFromPosAll
 *   0x00189a68 GenDebMomAdjFromPosRev
 *   0x00189a88 GenDebMomAdjFromSplash
 *   0x00189ab0 GenDebMomAdjFromAshRock
 *   0x00189ad8 GenDebMomAdjFromPosRevTree
 *   0x00189bc0 CreateCopyMat
 *   0x00189d80 DebAlloc
 *   0x00189de0 RemoveChunkControlFromStack
 *   0x00189e28 DebFreeWithoutKey
 *   0x00189e70 DebrisEmitterOrientationMtx
 *   0x00189f30 AddDebrisEffectToStack
 *   0x00189f60 DebrisDoSounds
 */

#include "nu_libc.h"

#include "numath/numtx.h"
#include "numath/nuvec.h"
#include "nu3d/numtl.h"
#include "nu3d/nutex.h"
#include "gamelib/debris.h"

struct chunkctrl_s {
    struct debchunk_s *chunk;
    s32 life;
    s32 state;
    struct debkeydatatype_s *deb;
    struct chunkctrl_s *next;
    s32 partindex;
    f32 bouncetime;
    s16 debtype;
    s16 bounced;
    s16 reflect_a;
    u8 pad_22[2];
    f32 reflect_x;
    f32 reflect_y;
};

struct singledebriskey_s {
    struct singledebriskey_s *next;
    struct debpart_s *debris;
};

struct debpart_s *GenDebIndex(struct debkeydatatype_s *deb, struct debinfo_s *info);
struct debpart_s *GenDebIndexRadial(struct debkeydatatype_s *deb, struct debinfo_s *info);
struct debpart_s *GenDebIndexRadialRotor(struct debkeydatatype_s *deb, struct debinfo_s *info);
struct debpart_s *GenDebIndexSpheroid(struct debkeydatatype_s *deb, struct debinfo_s *info);
struct debpart_s *GenDebIndexBounceY(struct debkeydatatype_s *deb, struct debinfo_s *info);
struct debpart_s *GenDebIndexBounceXZ(struct debkeydatatype_s *deb, struct debinfo_s *info);
struct debpart_s *GenDebIndexPos(struct debkeydatatype_s *deb, struct debinfo_s *info);
struct debpart_s *GenDebIndexPosRandTime(struct debkeydatatype_s *deb, struct debinfo_s *info);
struct debpart_s *GenDebIndexWaterFall(struct debkeydatatype_s *deb, struct debinfo_s *info);
struct debpart_s *GenDebIndexWaterFallSplash(struct debkeydatatype_s *deb, struct debinfo_s *info);
struct debpart_s *GenDebIndexSort(struct debkeydatatype_s *deb, struct debinfo_s *info);
void GenDebMomAdjFromPos(struct debkeydatatype_s *deb, struct debinfo_s *info, struct debpart_s *part);
void GenDebMomAdjFromPosAll(struct debkeydatatype_s *deb, struct debinfo_s *info, struct debpart_s *part);
void GenDebMomAdjFromPosRev(struct debkeydatatype_s *deb, struct debinfo_s *info, struct debpart_s *part);
void GenDebMomAdjFromSplash(struct debkeydatatype_s *deb, struct debinfo_s *info, struct debpart_s *part);
void GenDebMomAdjFromAshRock(struct debkeydatatype_s *deb, struct debinfo_s *info, struct debpart_s *part);
void GenDebMomAdjFromPosRevTree(struct debkeydatatype_s *deb, struct debinfo_s *info, struct debpart_s *part);

struct debinfo_s effecttypes[128] = {
    {
        "null", 0, 1, 100, 0, 1, 0, 0, 0,
        0, 0, 0, 0,
        40000.0f, 0.0f, 25.0f, 0.0f, 1.0f,
        {0}, {0}, {0}, {0},
        {0}, {0}, {0}, {0},
        0.0f, 1.0f,
        0, 0, 0, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        { { 0.0f, 64.0f, 64.0f, 64.0f }, { 1.0f, 0.0f, 0.0f, 0.0f } },
        { { 0.0f, 64.0f }, { 1.0f, 0.0f } },
        0.0f, 500.0f,
        { { 0.0f, 500.0f }, { 1.0f, 500.0f } },
        { { 0.0f, 500.0f }, { 1.0f, 500.0f } },
        -360.0f, 360.0f,
        { {0}, { 1.0f, 0.0f } },
        { {0}, { 1.0f, 0.0f } },
        { {0}, { 1.0f, 0.0f } },
        0.0f, 0.0f, 256.0f, 256.0f,
        0,
        {0},
        0, -1,
        { { -1, 0, 0 }, { -1, 0, 0 }, { -1, 0, 0 }, { -1, 0, 0 } },
    },
    {
        "flamet", 0, 2, 240, 0, 1, 0, 0, 0,
        0, 0, 0, 0,
        40000.0f, 0.0f, 25.0f, 0.0f, 1.0f,
        {0}, {0}, {0}, {0},
        { 2.3283064e-10f, 0.0f, 2.3283064e-10f },
        { -0.25f, 0.0f, -0.25f },
        { 1.1641532e-10f, 1.8626452e-10f, 1.1641532e-10f },
        { -0.125f, 1.6f, -0.125f },
        0.0f, 2.0f,
        16, 1, 3, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        {
            { 0.0f, 192.0f, 192.0f, 0.0f },
            { 0.4f, 192.0f, 0.0f, 0.0f },
            { 0.6f, 64.0f, 64.0f, 64.0f },
            { 1.0f, 0.0f, 0.0f, 0.0f },
        },
        { { 0.0f, 96.0f }, { 0.4f, 128.0f }, { 0.6f, 64.0f }, { 1.0f, 0.0f } },
        0.0f, 5000.0f,
        { { 0.0f, 1500.0f }, { 1.0f, 3500.0f } },
        { { 0.0f, 1500.0f }, { 1.0f, 3500.0f } },
        -360.0f, 360.0f,
        { {0}, { 1.0f, 32768.0f } },
        { {0}, { 1.0f, 0.0f } },
        { {0}, { 1.0f, 0.0f } },
        3.5f, 4.5f, 12.5f, 11.5f,
        0,
        { { 0.0f, 0.1f }, { 1.0f, 0.4f } },
        0, -1,
        { { -1, 0, 0 }, { -1, 0, 0 }, { -1, 0, 0 }, { -1, 0, 0 } },
    },
};
struct debinfo_s *debtab[128] = { &effecttypes[0], &effecttypes[1] };
struct debpart_s *(*gensorttab[11])() = {
    GenDebIndex,
    GenDebIndexSort,
    GenDebIndexPos,
    GenDebIndexWaterFall,
    GenDebIndexWaterFallSplash,
    GenDebIndexPosRandTime,
    GenDebIndexRadial,
    GenDebIndexRadialRotor,
    GenDebIndexSpheroid,
    GenDebIndexBounceY,
    GenDebIndexBounceXZ,
};
void (*gencodetab[7])(struct debkeydatatype_s *deb, struct debinfo_s *info, struct debpart_s *part) = {
    0,
    GenDebMomAdjFromPos,
    GenDebMomAdjFromPosRev,
    GenDebMomAdjFromSplash,
    GenDebMomAdjFromAshRock,
    GenDebMomAdjFromPosRevTree,
    GenDebMomAdjFromPosAll,
};
struct numtl_s *DebMat[8] = { 0 };
struct debkeydatatype_s debkeydata[256] = { 0 };
struct debrender_s ParticleChunkRenderStack[256] = { 0 };
/* Layout unknown: unreferenced in every build. */
u8 ColourSlots[2400] = { 0 };
struct chunkctrl_s debris_chunk_controls[512] = { 0 };
struct chunkctrl_s *debris_chunk_control_stack[32] = { 0 };
struct debchunk_s *freedebchunks[256] = { 0 };
s16 freedebkeys[256] = { 0 };
struct chunkctrl_s *freechunkcontrols[512] = { 0 };
struct debdma_s *DmaDebTypes[128] = { 0 };
struct debkeydatatype_s *debris_emitter_stack[32] = { 0 };
struct singledebriskey_s singledebriskey[256] = { 0 };
struct singledebriskey_s *debris_single_stack[32] = { 0 };
char debbuffer[0x100000] = { 0 };
/* Layout unknown: unreferenced in every build. */
u8 flamehandel[32] = { 0 };

s32 effect_types_used = 2;
struct nuvec_s *CutoffCameraVec = 0;
s32 curdeba = 0;
s32 curdebb = 0;
s32 debris_render_group = 0;
static s32 render_debris_enabled = 1;
s32 debris_sfx = 0;
s32 freeDmaDebType = 0;
f32 globaltime = 0.0f;
s32 globalframes = 0;
s32 FreeColourSlot = 0;
s32 debris_chunk_control_stack_index = 0;
s32 freedebchkptr = 0;
s32 freedebkeyptr = 0;
s32 freechunkcontrolsptr = 0;
s32 debris_emitter_stack_index = 0;
s32 debris_single_stack_index = 0;

extern struct numtx_s numtx_identity;
extern f32 NuTrigTable[];
extern u8 object_switches[];

void SetupDebris(void);
void DebFree(s32 *key);
void DebFreeInstantly(s32 *key);
struct debkeydatatype_s **FindDebrisEffectStack(struct debkeydatatype_s *node);
void RemoveDebrisEffectFromStack(struct debkeydatatype_s *node, struct debkeydatatype_s **list);
void DebReAlloc2(struct debkeydatatype_s *deb);
void DebReAlloc(struct debkeydatatype_s *deb, s32 count);
void DebFreeWithoutKey(struct debkeydatatype_s *deb);
void DebrisDoSounds(struct debkeydatatype_s *deb, s32 type);
void AddDebrisEffect(s32 *key, s32 type, f32 x, f32 y, f32 z);
f32 NuFsqrt(f32 x);
void edbitsSoundPlay(struct nuvec_s *pos, s32 id);
void GenericDebinfoDmaTypeUpdate(struct debinfo_s *info);
void AddChunkControlToStack(struct chunkctrl_s *node, struct chunkctrl_s **list);
void RemoveChunkControlFromStack(struct chunkctrl_s *node, struct chunkctrl_s **list);
f32 CameraEmitterDistance(struct nuvec_s *pos);
s32 DebAlloc(void);
void AddDebrisEffectToStack(struct debkeydatatype_s *node, struct debkeydatatype_s **list);
void DebrisStartOffset(s32 key, s32 offset);
void NuRndrParticleGroup(struct debchunk_s *chunk, struct debdma_s *dma, struct numtl_s *mtl, struct numtx_s *mtx, s32 flags, f32 time);
void DebrisEmitterOrientationMtx(s32 key, struct numtx_s *mtx);
s32 NuFileLoadBuffer(char *name, void *buf, s32 max);
void LinkDmaParticalSets(struct debchunk_s **sets, s32 count);
struct numtl_s *CreateAlphaBlendTexture256_32(char *file, s32 blend, s32 alphamode, s32 alpha_sort, s32 zmode);
struct DmaTag *CreateDmaParticleSet(struct DmaTag *set, s32 *size);
struct PartEffectList *CreateDmaPartEffectList(struct PartEffectList *list, s32 *size);


void GenericDebinfoDmaTypeUpdate(struct debinfo_s *info) {
    struct debtri_s *tri;
    s32 i;
    s32 j;
    f32 t;
    f32 dt;
    f32 range;
    f32 width;
    f32 height;
    f32 angle;
    f32 wobx;
    f32 woby;
    f32 r;
    f32 g;
    f32 b;
    f32 alpha;

    if (info->DmaDebTypePointer == 0) {
        info->DmaDebTypePointer = DmaDebTypes[freeDmaDebType++];
    }
    info->DmaDebTypePointer->grav = info->grav;
    info->DmaDebTypePointer->u0 = info->DmaDebTypePointer->u2 = info->u1 * 16.0f;
    info->DmaDebTypePointer->u1 = info->DmaDebTypePointer->u3 = info->u2 * 16.0f;
    info->DmaDebTypePointer->v0 = info->DmaDebTypePointer->v1 = info->v1 * 16.0f;
    info->DmaDebTypePointer->v2 = info->DmaDebTypePointer->v3 = info->v2 * 16.0f;
    for (i = 0; i < 64; i++) {
        t = (f32)i * 0.015625f;

        j = 0;
        while (j < 7 && !(info->wslot[j].t <= t && t <= info->wslot[j + 1].t)) {
            j++;
        }
        dt = t - info->wslot[j].t;
        range = info->wslot[j + 1].t - info->wslot[j].t;
        width = info->wslot[j].value + dt / range * (info->wslot[j + 1].value - info->wslot[j].value);

        j = 0;
        while (j < 7 && !(info->hslot[j].t <= t && t <= info->hslot[j + 1].t)) {
            j++;
        }
        dt = t - info->hslot[j].t;
        range = info->hslot[j + 1].t - info->hslot[j].t;
        height = info->hslot[j].value + dt / range * (info->hslot[j + 1].value - info->hslot[j].value);

        j = 0;
        while (j < 7 && !(info->rotslot[j].t <= t && t <= info->rotslot[j + 1].t)) {
            j++;
        }
        dt = t - info->rotslot[j].t;
        range = info->rotslot[j + 1].t - info->rotslot[j].t;
        angle = info->rotslot[j].value + dt / range * (info->rotslot[j + 1].value - info->rotslot[j].value);

        tri = &info->DmaDebTypePointer->tri[i];
        wobx = info->jibxamp * NuTrigTable[(u16)(s32)(info->jibxfreq * t * 65536.0f)];
        woby = info->jibyamp * NuTrigTable[(u16)(s32)(info->jibyfreq * t * 65536.0f)];
        tri->v0x.coord = (-(width * 0.5f) * NuTrigTable[(u16)(s32)(angle + 16384.0f)]
                          - height * 0.5f * NuTrigTable[(u16)(s32)angle] + wobx) * 640.0f * 0.00390625f;
        tri->v0y.coord = width * 0.5f * NuTrigTable[(u16)(s32)angle]
                       - height * 0.5f * NuTrigTable[(u16)(s32)(angle + 16384.0f)] + woby;
        tri->v2x = (width * 0.5f * NuTrigTable[(u16)(s32)(angle + 16384.0f)]
                    - height * 0.5f * NuTrigTable[(u16)(s32)angle] + wobx) * 640.0f * 0.00390625f;
        tri->v2y = -(width * 0.5f) * NuTrigTable[(u16)(s32)angle]
                 - height * 0.5f * NuTrigTable[(u16)(s32)(angle + 16384.0f)] + woby;
        tri->v1x.coord = height * NuTrigTable[(u16)(s32)angle] * 640.0f * 0.00390625f;
        tri->v1y.coord = height * NuTrigTable[(u16)(s32)(angle + 16384.0f)];

        j = 0;
        while (j < 7 && !(info->colslot[j].t <= t && t <= info->colslot[j + 1].t)) {
            j++;
        }
        dt = t - info->colslot[j].t;
        range = info->colslot[j + 1].t - info->colslot[j].t;
        r = info->colslot[j].r + dt / range * (info->colslot[j + 1].r - info->colslot[j].r);
        g = info->colslot[j].g + dt / range * (info->colslot[j + 1].g - info->colslot[j].g);
        b = info->colslot[j].b + dt / range * (info->colslot[j + 1].b - info->colslot[j].b);

        j = 0;
        while (j < 7 && !(info->alpslot[j].t <= t && t <= info->alpslot[j + 1].t)) {
            j++;
        }
        dt = t - info->alpslot[j].t;
        range = info->alpslot[j + 1].t - info->alpslot[j].t;
        alpha = info->alpslot[j].value + dt / range * (info->alpslot[j + 1].value - info->alpslot[j].value);

        tri->v0x.bits = (tri->v0x.bits & 0xFFFFFF00) + ((s32)r & 0xFF);
        tri->v0y.bits = (tri->v0y.bits & 0xFFFFFF00) + ((s32)g & 0xFF);
        tri->v1x.bits = (tri->v1x.bits & 0xFFFFFF00) + ((s32)b & 0xFF);
        tri->v1y.bits = (tri->v1y.bits & 0xFFFFFF00) + ((s32)alpha & 0xFF);
    }
}


struct debpart_s *GenDebIndex(struct debkeydatatype_s *deb, struct debinfo_s *info) {
    struct nuvec_s v;
    struct debpart_s *part;

    if (deb->pointer >= deb->debcount) {
        deb->pointer = 0;
    }
    part = deb->chunks[deb->pointer / 32]->parts;
    part += (s16)(deb->pointer % 32);
    deb->pointer++;
    part->time = globaltime;
    part->rate = 64.0f / info->etime;
    v.x = (f32)rand() * info->variable_start_ranscale.x - info->variable_start.x;
    v.y = (f32)rand() * info->variable_start_ranscale.y - info->variable_start.y;
    v.z = (f32)rand() * info->variable_start_ranscale.z - info->variable_start.z;
    NuVecMtxTransform(&v, &v, &deb->emitrotmtx);
    part->pos.x = v.x;
    part->pos.y = v.y;
    part->pos.z = v.z;
    v.x = (f32)rand() * info->variable_emit_ranscale.x - info->variable_emit.x;
    v.y = (f32)rand() * info->variable_emit_ranscale.y - info->variable_emit.y + info->emitmag;
    v.z = (f32)rand() * info->variable_emit_ranscale.z - info->variable_emit.z;
    NuVecMtxTransform(&v, &v, &deb->emitrotmtx);
    part->mom.x = v.x;
    part->mom.y = v.y;
    part->mom.z = v.z;
    if (deb->gencode != 0) {
        deb->gencode(deb, info, part);
    }
    part->pos.x += deb->emitoffset.x;
    part->pos.y += deb->emitoffset.y;
    part->pos.z += deb->emitoffset.z;
    if (info->DmaDebTypePointer == 0) {
        GenericDebinfoDmaTypeUpdate(info);
    }
    return part;
}


struct debpart_s *GenDebIndexRadial(struct debkeydatatype_s *deb, struct debinfo_s *info) {
    struct nuvec_s v;
    struct debpart_s *part;
    s32 roty;
    s32 rotz;
    f32 zero;

    if (deb->pointer >= deb->debcount) {
        deb->pointer = 0;
    }
    part = deb->chunks[deb->pointer / 32]->parts;
    part += (s16)(deb->pointer % 32);
    deb->pointer++;
    part->time = globaltime;
    part->rate = 64.0f / info->etime;
    zero = 0.0f;
    roty = (f32)rand() * info->variable_emit_ranscale.y - info->variable_emit.y + info->variable_start.y;
    rotz = (f32)rand() * info->variable_emit_ranscale.z - info->variable_emit.z + info->variable_start.z;
    v.y = info->variable_start.x;
    v.z = zero;
    v.x = zero;
    NuVecRotateZ(&v, &v, rotz);
    NuVecRotateY(&v, &v, roty);
    NuVecMtxTransform(&v, &v, &deb->emitrotmtx);
    part->pos.x = v.x;
    part->pos.y = v.y;
    part->pos.z = v.z;
    v.y = (f32)rand() * info->variable_emit_ranscale.x - info->variable_emit.x + info->emitmag;
    v.z = zero;
    v.x = zero;
    NuVecRotateZ(&v, &v, rotz);
    NuVecRotateY(&v, &v, roty);
    NuVecMtxTransform(&v, &v, &deb->emitrotmtx);
    part->mom.x = v.x;
    part->mom.y = v.y;
    part->mom.z = v.z;
    if (deb->gencode != 0) {
        deb->gencode(deb, info, part);
    }
    part->pos.x += deb->emitoffset.x;
    part->pos.y += deb->emitoffset.y;
    part->pos.z += deb->emitoffset.z;
    if (info->DmaDebTypePointer == 0) {
        GenericDebinfoDmaTypeUpdate(info);
    }
    return part;
}


struct debpart_s *GenDebIndexRadialRotor(struct debkeydatatype_s *deb, struct debinfo_s *info) {
    struct nuvec_s v;
    struct debpart_s *part;
    s32 roty;
    s32 rotz;
    f32 zero;

    if (deb->pointer >= deb->debcount) {
        deb->pointer = 0;
    }
    part = deb->chunks[deb->pointer / 32]->parts;
    part += (s16)(deb->pointer % 32);
    deb->pointer++;
    part->time = globaltime;
    part->rate = 64.0f / info->etime;
    zero = 0.0f;
    v.z = zero;
    rotz = deb->rotorz + info->variable_start.z;
    roty = deb->rotory + info->variable_start.y;
    v.x = zero;
    v.y = info->variable_start.x;
    NuVecRotateZ(&v, &v, rotz);
    NuVecRotateY(&v, &v, roty);
    NuVecMtxTransform(&v, &v, &deb->emitrotmtx);
    part->pos.x = v.x;
    part->pos.y = v.y;
    part->pos.z = v.z;
    v.y = (f32)rand() * info->variable_emit_ranscale.x - info->variable_emit.x + info->emitmag;
    v.z = zero;
    v.x = zero;
    NuVecRotateZ(&v, &v, rotz);
    NuVecRotateY(&v, &v, roty);
    NuVecMtxTransform(&v, &v, &deb->emitrotmtx);
    part->mom.x = v.x;
    part->mom.y = v.y;
    part->mom.z = v.z;
    if (deb->gencode != 0) {
        deb->gencode(deb, info, part);
    }
    part->pos.x += deb->emitoffset.x;
    part->pos.y += deb->emitoffset.y;
    part->pos.z += deb->emitoffset.z;
    if (info->DmaDebTypePointer == 0) {
        GenericDebinfoDmaTypeUpdate(info);
    }
    if (info->variable_emit.y == zero) {
        deb->rotory = 0;
    } else {
        deb->rotory = deb->rotory + info->variable_emit.y;
    }
    if (info->variable_emit.z == 0.0f) {
        deb->rotorz = 0;
    } else {
        deb->rotorz = deb->rotorz + info->variable_emit.z;
    }
    return part;
}


struct debpart_s *GenDebIndexSpheroid(struct debkeydatatype_s *deb, struct debinfo_s *info) {
    struct nuvec_s v;
    struct numtx_s scale;
    struct debpart_s *part;
    s32 roty;
    s32 rotz;

    if (deb->pointer >= deb->debcount) {
        deb->pointer = 0;
    }
    part = deb->chunks[deb->pointer / 32]->parts;
    part += (s16)(deb->pointer % 32);
    deb->pointer++;
    part->time = globaltime;
    part->rate = 64.0f / info->etime;
    v.y = rand() * 4.656612873077393e-10;
    v.x = v.z = 0.0f;
    roty = rand() * 1.52587890625e-05;
    rotz = rand() * 3.0517578125e-05;
    NuVecRotateZ(&v, &v, rotz);
    NuVecRotateY(&v, &v, roty);
    NuMtxSetIdentity(&scale);
    scale._00 = info->variable_start.x;
    scale._11 = info->variable_start.y;
    scale._22 = info->variable_start.z;
    NuVecMtxTransform(&v, &v, &scale);
    NuVecMtxTransform(&v, &v, &deb->emitrotmtx);
    part->pos.x = v.x;
    part->pos.y = v.y;
    part->pos.z = v.z;
    v.x = (f32)rand() * info->variable_emit_ranscale.x - info->variable_emit.x;
    v.y = (f32)rand() * info->variable_emit_ranscale.y - info->variable_emit.y + info->emitmag;
    v.z = (f32)rand() * info->variable_emit_ranscale.z - info->variable_emit.z;
    NuVecMtxTransform(&v, &v, &deb->emitrotmtx);
    part->mom.x = v.x;
    part->mom.y = v.y;
    part->mom.z = v.z;
    if (deb->gencode != 0) {
        deb->gencode(deb, info, part);
    }
    part->pos.x += deb->emitoffset.x;
    part->pos.y += deb->emitoffset.y;
    part->pos.z += deb->emitoffset.z;
    if (info->DmaDebTypePointer == 0) {
        GenericDebinfoDmaTypeUpdate(info);
    }
    return part;
}


inline s32 SolveQuadratic(f32 a, f32 b, f32 c, f32 *x1, f32 *x2) {
    f32 bb;
    f32 ac4;
    f32 root;

    if (a == 0.0f) {
        if (b == 0.0f) {
            return 0;
        }
        *x1 = *x2 = -c / b;
        return 1;
    }
    bb = b * b;
    ac4 = a * 4.0f * c;
    if (bb < ac4) {
        return 0;
    }
    if (ac4 == bb) {
        *x1 = *x2 = -b / (a + a);
        return 1;
    }
    root = NuFsqrt(bb - ac4);
    *x1 = (-b - root) / (a + a);
    *x2 = (-b + root) / (a + a);
    return 1;
}


struct debpart_s *GenDebIndexBounceY(struct debkeydatatype_s *deb, struct debinfo_s *info) {
    struct nuvec_s v;
    struct debchunk_s *chunk;
    struct debpart_s *part;
    f32 root1;
    f32 root2;

    if (deb->pointer >= deb->debcount) {
        deb->pointer = 0;
    }
    chunk = deb->chunks[deb->pointer / 32];
    part = &chunk->parts[(s16)(deb->pointer % 32)];
    part->time = globaltime;
    part->rate = 64.0f / info->etime;
    v.x = (f32)rand() * info->variable_start_ranscale.x - info->variable_start.x;
    v.y = (f32)rand() * info->variable_start_ranscale.y - info->variable_start.y;
    v.z = (f32)rand() * info->variable_start_ranscale.z - info->variable_start.z;
    NuVecMtxTransform(&v, &v, &deb->emitrotmtx);
    part->pos.x = v.x;
    part->pos.y = v.y;
    part->pos.z = v.z;
    v.x = (f32)rand() * info->variable_emit_ranscale.x - info->variable_emit.x;
    v.y = (f32)rand() * info->variable_emit_ranscale.y - info->variable_emit.y + info->emitmag;
    v.z = (f32)rand() * info->variable_emit_ranscale.z - info->variable_emit.z;
    NuVecMtxTransform(&v, &v, &deb->emitrotmtx);
    part->mom.x = v.x;
    part->mom.y = v.y;
    part->mom.z = v.z;
    if (SolveQuadratic(info->grav, part->mom.y, part->pos.y - deb->refoff, &root1, &root2)
        && (root1 > root2 ? root1 : root2) > 0.0f && (root1 > root2 ? root1 : root2) < info->etime
        && freechunkcontrolsptr < 409) {
        freechunkcontrols[freechunkcontrolsptr]->chunk = chunk;
        freechunkcontrols[freechunkcontrolsptr]->life = (root1 > root2 ? root1 : root2) * 60.0f;
        freechunkcontrols[freechunkcontrolsptr]->state = 5;
        freechunkcontrols[freechunkcontrolsptr]->deb = 0;
        freechunkcontrols[freechunkcontrolsptr]->debtype = deb->type;
        freechunkcontrols[freechunkcontrolsptr]->reflect_a = 0;
        freechunkcontrols[freechunkcontrolsptr]->bounced = 0;
        freechunkcontrols[freechunkcontrolsptr]->reflect_x = deb->refoff;
        freechunkcontrols[freechunkcontrolsptr]->reflect_y = deb->refbounce;
        freechunkcontrols[freechunkcontrolsptr]->partindex = (s16)(deb->pointer % 32);
        freechunkcontrols[freechunkcontrolsptr]->bouncetime = root1 > root2 ? root1 : root2;
        AddChunkControlToStack(freechunkcontrols[freechunkcontrolsptr],
                               &debris_chunk_control_stack[debris_chunk_control_stack_index]);
        freechunkcontrolsptr++;
    }
    if (deb->gencode != 0) {
        deb->gencode(deb, info, part);
    }
    deb->pointer++;
    part->pos.x += deb->emitoffset.x;
    part->pos.y += deb->emitoffset.y;
    part->pos.z += deb->emitoffset.z;
    if (info->DmaDebTypePointer == 0) {
        GenericDebinfoDmaTypeUpdate(info);
    }
    return part;
}


struct debpart_s *GenDebIndexBounceXZ(struct debkeydatatype_s *deb, struct debinfo_s *info) {
    struct nuvec_s v;
    struct nuvec_s normal = {1.0f, 0.0f, 0.0f};
    struct nuvec_s plane;
    struct nuvec_s end;
    struct nuvec_s tostart;
    struct nuvec_s toend;
    struct debchunk_s *chunk;
    struct debpart_s *part;
    f32 startdist;
    f32 enddist;
    f32 t;

    if (deb->pointer >= deb->debcount) {
        deb->pointer = 0;
    }
    chunk = deb->chunks[deb->pointer / 32];
    part = &chunk->parts[(s16)(deb->pointer % 32)];
    part->time = globaltime;
    part->rate = 64.0f / info->etime;
    v.x = (f32)rand() * info->variable_start_ranscale.x - info->variable_start.x;
    v.y = (f32)rand() * info->variable_start_ranscale.y - info->variable_start.y;
    v.z = (f32)rand() * info->variable_start_ranscale.z - info->variable_start.z;
    NuVecMtxTransform(&v, &v, &deb->emitrotmtx);
    part->pos.x = v.x;
    part->pos.y = v.y;
    part->pos.z = v.z;
    v.x = (f32)rand() * info->variable_emit_ranscale.x - info->variable_emit.x;
    v.y = (f32)rand() * info->variable_emit_ranscale.y - info->variable_emit.y + info->emitmag;
    v.z = (f32)rand() * info->variable_emit_ranscale.z - info->variable_emit.z;
    NuVecMtxTransform(&v, &v, &deb->emitrotmtx);
    part->mom.x = v.x;
    part->mom.y = v.y;
    part->mom.z = v.z;
    NuVecRotateY(&normal, &normal, deb->refroty);
    NuVecScale(&plane, &normal, deb->refoff);
    end.x = part->pos.x + info->etime * part->mom.x;
    end.y = 0.0f;
    end.z = part->pos.z + info->etime * part->mom.z;
    NuVecSub(&tostart, &plane, &part->pos);
    NuVecSub(&toend, &plane, &end);
    startdist = tostart.x * normal.x + tostart.z * normal.z;
    enddist = toend.x * normal.x + toend.z * normal.z;
    if ((startdist < 0.0f && enddist > 0.0f) || (startdist > 0.0f && enddist < 0.0f)) {
        t = info->etime * ((0.0f - startdist) / (enddist - startdist));
        if (freechunkcontrolsptr < 409) {
            freechunkcontrols[freechunkcontrolsptr]->chunk = chunk;
            freechunkcontrols[freechunkcontrolsptr]->life = t * 60.0f;
            freechunkcontrols[freechunkcontrolsptr]->state = 6;
            freechunkcontrols[freechunkcontrolsptr]->deb = 0;
            freechunkcontrols[freechunkcontrolsptr]->debtype = deb->type;
            freechunkcontrols[freechunkcontrolsptr]->reflect_a = deb->refroty;
            freechunkcontrols[freechunkcontrolsptr]->bounced = 0;
            freechunkcontrols[freechunkcontrolsptr]->reflect_x = deb->refoff;
            freechunkcontrols[freechunkcontrolsptr]->reflect_y = deb->refbounce;
            freechunkcontrols[freechunkcontrolsptr]->partindex = (s16)(deb->pointer % 32);
            freechunkcontrols[freechunkcontrolsptr]->bouncetime = t;
            AddChunkControlToStack(freechunkcontrols[freechunkcontrolsptr],
                                   &debris_chunk_control_stack[debris_chunk_control_stack_index]);
            freechunkcontrolsptr++;
        }
    }
    if (deb->gencode != 0) {
        deb->gencode(deb, info, part);
    }
    deb->pointer++;
    part->pos.x += deb->emitoffset.x;
    part->pos.y += deb->emitoffset.y;
    part->pos.z += deb->emitoffset.z;
    if (info->DmaDebTypePointer == 0) {
        GenericDebinfoDmaTypeUpdate(info);
    }
    return part;
}


struct debpart_s *GenDebIndexPos(struct debkeydatatype_s *deb, struct debinfo_s *info) {
    struct debpart_s *part;

    if (deb->pointer >= deb->debcount) {
        deb->pointer = 0;
    }
    part = deb->chunks[deb->pointer / 32]->parts;
    part += (s16)(deb->pointer % 32);
    deb->pointer++;
    part->time = globaltime;
    part->rate = 64.0f / info->etime;
    part->pos.x = (f32)rand() * info->pos_range.x + info->pos_base.x;
    part->pos.y = (f32)rand() * info->pos_range.y + info->pos_base.y;
    part->pos.z = (f32)rand() * info->pos_range.z + info->pos_base.z;
    part->mom.x = (f32)rand() * info->mom_range.x + info->mom_base.x;
    part->mom.y = (f32)rand() * info->mom_range.y + info->mom_base.y;
    part->mom.z = (f32)rand() * info->mom_range.z + info->mom_base.z;
    if (deb->gencode != 0) {
        deb->gencode(deb, info, part);
    }
    if (info->DmaDebTypePointer == 0) {
        GenericDebinfoDmaTypeUpdate(info);
    }
    return part;
}


struct debpart_s *GenDebIndexPosRandTime(struct debkeydatatype_s *deb, struct debinfo_s *info) {
    struct debpart_s *part;

    if (deb->pointer >= deb->debcount) {
        deb->pointer = 0;
    }
    part = deb->chunks[deb->pointer / 32]->parts;
    part += (s16)(deb->pointer % 32);
    deb->pointer++;
    part->time = globaltime;
    part->rate = 64.0f / (info->etime + info->etime * rand() / 1503238528.0f);
    part->pos.x = (f32)rand() * info->pos_range.x + info->pos_base.x;
    part->pos.y = (f32)rand() * info->pos_range.y + info->pos_base.y;
    part->pos.z = (f32)rand() * info->pos_range.z + info->pos_base.z;
    part->mom.x = (f32)rand() * info->mom_range.x + info->mom_base.x;
    part->mom.y = (f32)rand() * info->mom_range.y + info->mom_base.y;
    part->mom.z = (f32)rand() * info->mom_range.z + info->mom_base.z;
    if (deb->gencode != 0) {
        deb->gencode(deb, info, part);
    }
    if (info->DmaDebTypePointer == 0) {
        GenericDebinfoDmaTypeUpdate(info);
    }
    return part;
}


struct debpart_s *GenDebIndexWaterFall(struct debkeydatatype_s *deb, struct debinfo_s *info) {
    struct debpart_s *part;
    f32 spread;

    if (deb->pointer >= deb->debcount) {
        deb->pointer = 0;
    }
    part = deb->chunks[deb->pointer / 32]->parts;
    part += (s16)(deb->pointer % 32);
    deb->pointer++;
    part->time = globaltime;
    part->rate = 64.0f / info->etime;
    spread = rand() * 0.6 * 4.656612873077393e-10 - 0.3;
    part->pos.x = spread * NuTrigTable[14000];
    part->pos.z = spread * NuTrigTable[14000 + 0x4000];
    spread = rand() * 1.5 * 4.656612873077393e-10 + 1.0;
    part->mom.x = part->pos.x * 1.5f + spread * NuTrigTable[30000];
    part->mom.z = part->pos.z * 1.5f + spread * NuTrigTable[30000 + 0x4000];
    part->pos.y = (f32)rand() * info->pos_range.y + info->pos_base.y;
    part->mom.y = (f32)rand() * info->mom_range.y + info->mom_base.y;
    if (deb->gencode != 0) {
        deb->gencode(deb, info, part);
    }
    if (info->DmaDebTypePointer == 0) {
        GenericDebinfoDmaTypeUpdate(info);
    }
    return part;
}


struct debpart_s *GenDebIndexWaterFallSplash(struct debkeydatatype_s *deb, struct debinfo_s *info) {
    struct debpart_s *part;
    f32 spread;
    f32 angle;
    f32 speed;

    if (deb->pointer >= deb->debcount) {
        deb->pointer = 0;
    }
    part = deb->chunks[deb->pointer / 32]->parts;
    part += (s16)(deb->pointer % 32);
    deb->pointer++;
    part->time = globaltime;
    part->rate = 64.0f / info->etime;
    spread = rand() * 0.2 * 4.656612873077393e-10 + 0.7f;
    angle = rand() * 1.52587890625e-05 + 16384.0;
    speed = rand() * 1.5 * 4.656612873077393e-10 + 1.0;
    part->mom.x = part->pos.x * 2.0f;
    part->mom.z = part->pos.z * 2.0f;
    part->pos.y = (f32)rand() * info->pos_range.y + info->pos_base.y;
    part->mom.y = (f32)rand() * info->mom_range.y + info->mom_base.y;
    if (deb->gencode != 0) {
        deb->gencode(deb, info, part);
    }
    if (info->DmaDebTypePointer == 0) {
        GenericDebinfoDmaTypeUpdate(info);
    }
    return part;
}


struct numtl_s *CreateAlphaBlendTexture256(char *file, s32 blend, s32 alphamode) {
    struct nutex_s tex;
    u32 pixels[0x10000];
    struct numtl_s *mtl;
    u16 *src;
    u32 *dst;
    s32 i;
    u16 pixel;
    s32 texel;
    u16 colour;

    NuFileLoadBuffer(file, pixels, 0x7FFFFFFF);
    tex.type = 3;
    tex.width = 256;
    tex.height = 256;
    tex.mmcnt = 0;
    tex.bits = pixels;
    dst = pixels + 0x10000;
    src = (u16 *)pixels + 0x10000;
    for (i = 0; i < 0x10000; i++) {
        src--;
        texel = *src;
        pixel = ((u32)texel >> 8) | ((texel & 0xFF) << 8);
        do {
            if (pixel == 0x3E0) {
                dst--;
                texel = 0;
                goto convert;
            }
            dst--;
        } while (0);
        texel = pixel | 0x8000;
    convert:
        colour = texel;
        *dst = ((texel & 0x1F) * 255 / 31) << 24;
        *dst |= (((colour >> 5) & 0x1F) * 255 / 31) << 16;
        *dst |= (((colour >> 10) & 0x1F) * 255 / 31) << 8;
        *dst |= 0x80;
    }
    mtl = NuMtlCreate(1);
    mtl->diffuse.r = 1.0f;
    mtl->diffuse.g = 1.0f;
    mtl->diffuse.b = 1.0f;
    mtl->alpha = 0.999f;
    mtl->attrib.cull = 2;
    mtl->attrib.zmode = 1;
    mtl->attrib.alpha = alphamode;
    if (blend != 0) {
        mtl->attrib.uvmode = 1;
        mtl->attrib.shade = 0;
    }
    mtl->attrib.utc = 1;
    mtl->attrib.vtc = 1;
    mtl->tid = NuTexCreate(&tex);
    mtl->attrib.afail = 1;
    mtl->sort_pri = 0x100;
    NuMtlUpdate(mtl);
    return mtl;
}


void SplashEffect(struct debdma_s *dma) {
    struct debtri_s *tri;
    s32 i;

    dma->grav = -0.6f;
    dma->u0 = 0xC00;
    dma->v0 = 0x400;
    dma->u1 = 0xDF0;
    dma->v1 = 0x400;
    dma->u2 = 0xC00;
    dma->v2 = 0x5F0;
    dma->u3 = 0xDF0;
    dma->v3 = 0x5F0;
    tri = dma->tri;
    for (i = 0; i < 64; i++) {
        tri->v0x.coord = (NuTrigTable[(u16)((i << 9) + 0x4000)] * -75.0f
                          - NuTrigTable[(u16)(i << 9)] * 75.0f) * 640.0f * 0.00390625f;
        tri->v0y.coord = NuTrigTable[(u16)(i << 9)] * -75.0f
                       + NuTrigTable[(u16)((i << 9) + 0x4000)] * 75.0f;
        tri->v2x = (NuTrigTable[(u16)((i << 9) + 0x4000)] * 75.0f
                    - NuTrigTable[(u16)(i << 9)] * 75.0f) * 640.0f * 0.00390625f;
        tri->v2y = NuTrigTable[(u16)(i << 9)] * 75.0f
                 + NuTrigTable[(u16)((i << 9) + 0x4000)] * 75.0f;
        tri->v1x.coord = NuTrigTable[(u16)(i << 9)] * 150.0f * 640.0f * 0.00390625f;
        tri->v1y.coord = NuTrigTable[(u16)((i << 9) + 0x4000)] * -150.0f;
        tri->v0x.colour = 0x1E;
        tri->v0y.colour = 0x20;
        tri->v1x.colour = 0x30;
        if (i < 41) {
            tri->v1y.colour = 0x30;
        } else {
            tri->v1y.colour = (64 - i) * 2;
        }
        tri++;
    }
}


void SplashBodyEffect(struct debdma_s *dma) {
    struct debtri_s *tri;
    s32 i;
    s32 size;

    dma->grav = 0.0f;
    dma->u0 = 0;
    dma->v0 = 0;
    dma->u1 = 0x3F0;
    dma->v1 = 0;
    dma->u2 = 0;
    dma->v2 = 0x3F0;
    dma->u3 = 0x3F0;
    dma->v3 = 0x3F0;
    tri = dma->tri;
    for (i = 0; i < 64; i++, tri++) {
        size = 75 + i * 2;
        tri->v0x.coord = ((f32)-size * NuTrigTable[0x8000] - (f32)size * NuTrigTable[0x4000])
                       * 640.0f * 0.00390625f;
        tri->v0y.coord = (f32)-size * NuTrigTable[0x4000] + (f32)size * NuTrigTable[0x8000];
        tri->v2x = ((f32)size * NuTrigTable[0x8000] - (f32)size * NuTrigTable[0x4000])
                 * 640.0f * 0.00390625f;
        tri->v2y = (f32)size * NuTrigTable[0x4000] + (f32)size * NuTrigTable[0x8000];
        tri->v1x.coord = (f32)(size * 2) * NuTrigTable[0x4000] * 640.0f * 0.00390625f;
        tri->v1y.coord = (f32)(-size * 2) * NuTrigTable[0x8000];
        tri->v0x.colour = 0x1E;
        tri->v0y.colour = 0x20;
        tri->v1x.colour = 0x30;
        tri->v1y.colour = (64 - i) * 80 / 64;
    }
}


inline struct numtl_s *CreateCopyMat(struct numtl_s *src, s32 blend, s32 alphamode, s32 zmode, s32 filter) {
    struct numtl_s *mtl;

    mtl = NuMtlCreate(1);
    mtl->diffuse.r = 1.0f;
    mtl->diffuse.g = 1.0f;
    mtl->diffuse.b = 1.0f;
    mtl->alpha = 0.999f;
    mtl->attrib.cull = 2;
    mtl->attrib.zmode = zmode;
    mtl->attrib.filter = filter;
    mtl->attrib.alpha = alphamode;
    if (blend != 0) {
        mtl->attrib.uvmode = 1;
        mtl->attrib.shade = 0;
    }
    mtl->attrib.utc = 1;
    mtl->attrib.vtc = 1;
    mtl->tid = src->tid;
    mtl->sort_pri = src->sort_pri;
    mtl->attrib.afail = 1;
    NuMtlUpdate(mtl);
    return mtl;
}


void SetupDebris(void) {
    s32 i;
    s32 size;
    s32 chunksize;
    char *buffer;

    memset(debbuffer, 0, sizeof(debbuffer));
    memset(freedebchunks, 0, sizeof(freedebchunks));
    memset(freedebkeys, 0, sizeof(freedebkeys));
    memset(debkeydata, 0, sizeof(debkeydata));
    DebMat[0] = CreateAlphaBlendTexture256_32("stuff\\particle.raw", 1, 2, 0x100, 0);
    DebMat[1] = CreateCopyMat(DebMat[0], 0, 0, 3, 0);
    DebMat[2] = CreateCopyMat(DebMat[0], 1, 1, 1, 1);
    DebMat[3] = CreateCopyMat(DebMat[0], 1, 3, 1, 1);
    DebMat[4] = CreateCopyMat(DebMat[0], 0, 1, 1, 1);
    DebMat[5] = CreateCopyMat(DebMat[0], 0, 2, 1, 1);
    DebMat[6] = CreateCopyMat(DebMat[0], 0, 0, 0, 1);
    for (i = 0; debtab[i] != 0; i++) {
        if (debtab[i]->rate == 0) {
            debtab[i]->rate = 1;
        }
        debtab[i]->uvanim = (debtab[i]->uw * 60) / debtab[i]->rate;
        *(s32 *)&debtab[i]->uvanim = (*(s32 *)&debtab[i]->uvanim & 0xFFFFF800)
                                   + (debtab[i]->uw * (debtab[i]->frames - 1) & 0x3FF);
        debtab[i]->alpslot[0].value = debtab[i]->alpslot[0].value / debtab[i]->etime;
        debtab[i]->u1 = debtab[i]->u1 + 524288.0f;
        debtab[i]->u2 = debtab[i]->u2 + 524288.0f;
        debtab[i]->v1 = debtab[i]->v1 + 524288.0f;
        debtab[i]->v2 = debtab[i]->v2 + 524288.0f;
    }
    buffer = (char *)(((nusintptr)debbuffer + 15) & ~(nuintptr)15);
    freeDmaDebType = 0;
    size = 0x43000;
    for (i = 0; i < 256; i++) {
        chunksize = size;
        freedebchunks[i] = (struct debchunk_s *)CreateDmaParticleSet((struct DmaTag *)buffer, &chunksize);
        buffer += chunksize;
        size -= chunksize;
    }
    freedebchkptr = 0;
    for (i = 0; i < 256; i++) {
        freedebkeys[i] = i;
    }
    freedebkeyptr = 0;
    for (i = 0; i < 256; i++) {
        debkeydata[i].active = 0;
        debkeydata[i].pointer = 0;
        debkeydata[i].rotory = 0;
        debkeydata[i].rotorz = 0;
        debkeydata[i].instances = 0;
        debkeydata[i].type = 0;
    }
    for (i = 0; i < 32; i++) {
        debris_emitter_stack[i] = 0;
    }
    debris_emitter_stack_index = 0;
    for (i = 0; i < 32; i++) {
        debris_chunk_control_stack[i] = 0;
    }
    debris_chunk_control_stack_index = 0;
    for (i = 0; i < 512; i++) {
        freechunkcontrols[i] = &debris_chunk_controls[i];
    }
    freechunkcontrolsptr = 0;
    for (i = 0; i < 128; i++) {
        chunksize = size;
        DmaDebTypes[i] = (struct debdma_s *)CreateDmaPartEffectList((struct PartEffectList *)buffer, &chunksize);
        buffer += chunksize;
        size -= chunksize;
    }
    for (i = 0; i < 256; i++) {
        ParticleChunkRenderStack[i].chunk = 0;
        ParticleChunkRenderStack[i].debinfo = 0;
        ParticleChunkRenderStack[i].debdata = 0;
    }
}


void DebReAlloc2(struct debkeydatatype_s *deb) {
    s32 diff;
    s32 i;
    s32 j;

    diff = deb->reqcount - deb->count;
    if (diff > 0) {
        if (diff + freedebchkptr > 256) {
            return;
        }
        for (i = 0; i < diff; i++) {
            deb->chunks[i + deb->count] = freedebchunks[i + freedebchkptr];
            for (j = 0; j < 32; j++) {
                deb->chunks[i + deb->count]->parts[j].time = 0.0f;
                deb->chunks[i + deb->count]->parts[j].rate = 32768.0f;
            }
        }
        if (deb->count == 0) {
            for (i = 0; i < 256; i++) {
                if (ParticleChunkRenderStack[i].chunk == 0) {
                    ParticleChunkRenderStack[i].chunk = deb->chunks[0];
                    ParticleChunkRenderStack[i].debinfo = debtab[deb->type];
                    ParticleChunkRenderStack[i].debdata = deb;
                    break;
                }
            }
        }
        deb->count = deb->reqcount;
        deb->debcount = deb->reqdebcount;
        freedebchkptr += diff;
        LinkDmaParticalSets(deb->chunks, deb->count);
        return;
    }
    if (freechunkcontrolsptr - diff > 512) {
        return;
    }
    for (i = diff; i < 0; i++) {
        freechunkcontrols[freechunkcontrolsptr]->chunk = deb->chunks[i + deb->count];
        freechunkcontrols[freechunkcontrolsptr]->life = debtab[deb->type]->etime * 60.0f;
        freechunkcontrols[freechunkcontrolsptr]->state = 0;
        AddChunkControlToStack(freechunkcontrols[freechunkcontrolsptr],
                               &debris_chunk_control_stack[debris_chunk_control_stack_index]);
        freechunkcontrolsptr++;
    }
    if (deb->reqdebcount == 0) {
        for (i = 0; i < 256; i++) {
            if (ParticleChunkRenderStack[i].chunk == deb->chunks[0]) {
                ParticleChunkRenderStack[i].debinfo = debtab[deb->type];
                ParticleChunkRenderStack[i].debdata = 0;
                ParticleChunkRenderStack[i].rotmtx = deb->rotmtx;
                ParticleChunkRenderStack[i].pos.x = deb->emitpos.x;
                ParticleChunkRenderStack[i].pos.y = deb->emitpos.y;
                ParticleChunkRenderStack[i].pos.z = deb->emitpos.z;
                break;
            }
        }
    } else {
        LinkDmaParticalSets(&deb->chunks[diff + deb->count], -diff);
        for (i = 0; i < 256; i++) {
            if (ParticleChunkRenderStack[i].chunk == 0) {
                ParticleChunkRenderStack[i].chunk = deb->chunks[diff + deb->count];
                ParticleChunkRenderStack[i].debinfo = debtab[deb->type];
                ParticleChunkRenderStack[i].debdata = 0;
                ParticleChunkRenderStack[i].rotmtx = deb->rotmtx;
                ParticleChunkRenderStack[i].pos.x = deb->emitpos.x;
                ParticleChunkRenderStack[i].pos.y = deb->emitpos.y;
                ParticleChunkRenderStack[i].pos.z = deb->emitpos.z;
                break;
            }
        }
    }
    deb->debcount = deb->reqdebcount;
    deb->count = deb->reqcount;
    if (deb->debcount == 0) {
        deb->chunks[0] = 0;
    } else {
        LinkDmaParticalSets(deb->chunks, deb->count);
    }
}


inline void AddChunkControlToStack(struct chunkctrl_s *node, struct chunkctrl_s **list) {
    struct chunkctrl_s *cur;

    if (*list != 0) {
        do {
            cur = *list;
            list = &cur->next;
        } while (cur->next != 0);
    }
    *list = node;
    node->next = 0;
}


inline void RemoveChunkControlFromStack(struct chunkctrl_s *node, struct chunkctrl_s **list) {
    struct chunkctrl_s *cur;

    if (*list != 0) {
        if (*list == node) {
            *list = node->next;
        } else {
            do {
                cur = *list;
                list = &cur->next;
                if (cur->next == 0) {
                    goto unlinked;
                }
            } while (cur->next != node);
            cur->next = node->next;
        }
    }
unlinked:
    node->next = 0;
}


void DebFree(s32 *key) {
    struct chunkctrl_s **list;
    struct chunkctrl_s *cur;
    struct chunkctrl_s *node;
    struct debkeydatatype_s **stack;
    s32 i;

    if (*key != -1) {
        if (freechunkcontrolsptr + debkeydata[*key].count > 512) {
            DebFreeInstantly(key);
            return;
        }
        for (i = 0; i < debkeydata[*key].count; i++) {
            freechunkcontrols[freechunkcontrolsptr]->chunk = debkeydata[*key].chunks[i];
            freechunkcontrols[freechunkcontrolsptr]->life = debtab[debkeydata[*key].type]->etime * 60.0f;
            freechunkcontrols[freechunkcontrolsptr]->state = 0;
            freechunkcontrols[freechunkcontrolsptr]->deb = &debkeydata[*key];
            list = &debris_chunk_control_stack[debris_chunk_control_stack_index];
            node = freechunkcontrols[freechunkcontrolsptr];
            if (*list != 0) {
                do {
                    cur = *list;
                    list = &cur->next;
                } while (cur->next != 0);
            }
            *list = node;
            node->next = 0;
            freechunkcontrolsptr++;
        }
        for (i = 0; i < 256; i++) {
            if (ParticleChunkRenderStack[i].chunk == debkeydata[*key].chunks[0]) {
                ParticleChunkRenderStack[i].debdata = 0;
                ParticleChunkRenderStack[i].rotmtx = debkeydata[*key].rotmtx;
                ParticleChunkRenderStack[i].pos.x = debkeydata[*key].emitpos.x;
                ParticleChunkRenderStack[i].pos.y = debkeydata[*key].emitpos.y;
                ParticleChunkRenderStack[i].pos.z = debkeydata[*key].emitpos.z;
                break;
            }
        }
        stack = FindDebrisEffectStack(&debkeydata[*key]);
        RemoveDebrisEffectFromStack(&debkeydata[*key], stack);
        debkeydata[*key].type = 0;
        freedebkeyptr--;
        freedebkeys[freedebkeyptr] = *key;
        *key = -1;
    }
}


void DebFreeInstantly(s32 *key) {
    struct debkeydatatype_s **stack;
    s32 i;
    s32 j;

    if (*key != -1) {
        freedebchkptr -= debkeydata[*key].count;
        for (i = 0; i < debkeydata[*key].count; i++) {
            freedebchunks[i + freedebchkptr] = debkeydata[*key].chunks[i];
            for (j = 0; j < 256; j++) {
                if (ParticleChunkRenderStack[j].chunk == debkeydata[*key].chunks[i]) {
                    ParticleChunkRenderStack[j].chunk = 0;
                    ParticleChunkRenderStack[j].debinfo = 0;
                }
            }
        }
        stack = FindDebrisEffectStack(&debkeydata[*key]);
        RemoveDebrisEffectFromStack(&debkeydata[*key], stack);
        debkeydata[*key].type = 0;
        freedebkeyptr--;
        freedebkeys[freedebkeyptr] = *key;
        *key = -1;
    }
}


void DebrisStartOffset(s32 key, s32 offset) {
    struct debinfo_s *type;
    s32 looptime;
    s32 loopfrac;

    if (key != -1) {
        type = debtab[debkeydata[key].type];
        if (type->ival_on_ran != 0 || type->ival_off_ran != 0) {
            debkeydata[key].delay = 0;
        } else {
            if (type->gensort == 7) {
                debkeydata[key].delay = 0;
                debkeydata[key].rotory = offset * type->variable_emit.y;
                debkeydata[key].rotorz = offset * type->variable_emit.z;
            }
            looptime = type->ival_on + type->ival_off;
            loopfrac = globalframes % looptime - offset;
            if (loopfrac == 0) {
                debkeydata[key].delay = 0;
            } else {
                debkeydata[key].delay = looptime - loopfrac;
            }
            debkeydata[key].oncount = type->ival_on;
        }
    }
}


void AddVariableShotDebrisEffectMtx2(s32 type, struct nuvec_s *pos, s32 count, struct numtx_s *emitmtx,
                                     struct numtx_s *mtx) {
    struct debinfo_s *info;
    struct debkeydatatype_s *deb;
    struct chunkctrl_s *ctrl;
    struct chunkctrl_s *found;
    f32 dist;
    s32 chunkcount;
    s32 newchunks;
    s32 newkey;
    s32 key;
    s32 i;
    s32 j;
    s32 stack;

    stack = 0;
    found = 0;
    if (type == -1) {
        return;
    }
    info = debtab[type];
    dist = CameraEmitterDistance(pos);
    if (dist < info->cuton || info->cutoff <= dist) {
        return;
    }
    if (info->variable_key == -1) {
        if (freedebkeyptr >= 256) {
            return;
        }
        info->variable_key = freedebkeys[freedebkeyptr++];
        deb = &debkeydata[info->variable_key];
        deb->count = 0;
        deb->debcount = 0;
        deb->pointer = 0;
        deb->genptr = gensorttab[info->gensort];
        deb->gencode = gencodetab[info->gencode];
        deb->rotmtx = *mtx;
        deb->rotmtx._30 = 0.0f;
        deb->rotmtx._31 = 0.0f;
        deb->rotmtx._32 = 0.0f;
        newkey = info->variable_key;
        if (newkey != -1) {
            debkeydata[newkey].emitpos.x = 0.0f;
            debkeydata[newkey].emitpos.y = 0.0f;
            debkeydata[newkey].emitpos.z = 0.0f;
        }
    } else {
        deb = &debkeydata[info->variable_key];
    }
    if (deb->pointer + count > deb->count * 32) {
        if (deb->count < 0) {
            deb->count = 0;
        }
        chunkcount = (deb->pointer + count + 31) / 32;
        newchunks = chunkcount - deb->count;
        if (freedebchkptr + newchunks >= 256) {
            return;
        }
        for (i = 0; i < newchunks; i++) {
            deb->chunks[deb->count++] = freedebchunks[freedebchkptr++];
        }
        deb->count = chunkcount;
        LinkDmaParticalSets(deb->chunks, chunkcount);
        if (chunkcount == newchunks) {
            for (i = 0; i < 256; i++) {
                if (ParticleChunkRenderStack[i].chunk == 0) {
                    ParticleChunkRenderStack[i].chunk = deb->chunks[0];
                    ParticleChunkRenderStack[i].debinfo = info;
                    ParticleChunkRenderStack[i].debdata = deb;
                    break;
                }
            }
        }
    }
    deb->debcount += count;
    deb->emitoffset.x = pos->x;
    deb->emitoffset.y = pos->y;
    deb->emitoffset.z = pos->z;
    key = info->variable_key;
    if (key != -1) {
        debkeydata[key].emitrotmtx = *emitmtx;
        debkeydata[key].emitrotmtx._30 = 0.0f;
        debkeydata[key].emitrotmtx._31 = 0.0f;
        debkeydata[key].emitrotmtx._32 = 0.0f;
    }
    for (i = 0; i < count; i++) {
        deb->genptr(deb, info);
    }
    if (deb->disposed < deb->count) {
        i = deb->disposed;
        do {
            if (freechunkcontrolsptr < 512) {
                freechunkcontrols[freechunkcontrolsptr]->chunk = deb->chunks[i];
                freechunkcontrols[freechunkcontrolsptr]->life = info->etime * 60.0f;
                freechunkcontrols[freechunkcontrolsptr]->state = 1;
                freechunkcontrols[freechunkcontrolsptr]->deb = deb;
                AddChunkControlToStack(freechunkcontrols[freechunkcontrolsptr],
                                       &debris_chunk_control_stack[debris_chunk_control_stack_index]);
                freechunkcontrolsptr++;
                deb->disposed++;
            }
            i++;
        } while (i < deb->count);
    }
    if (deb->disposed == deb->count) {
        for (j = 0; j < 32; j++) {
            for (ctrl = debris_chunk_control_stack[j]; ctrl != 0; ctrl = ctrl->next) {
                if (ctrl->chunk == deb->chunks[deb->disposed - 1]) {
                    found = ctrl;
                    stack = j;
                    break;
                }
            }
            if (found != 0) {
                break;
            }
        }
        if (found != 0) {
            found->life = info->etime * 60.0f;
            if (stack != debris_chunk_control_stack_index) {
                RemoveChunkControlFromStack(found, &debris_chunk_control_stack[stack]);
                AddChunkControlToStack(found, &debris_chunk_control_stack[debris_chunk_control_stack_index]);
            }
        }
    }
}


inline s32 DebAlloc(void) {
    s32 key;
    struct debkeydatatype_s *deb;

    if (freedebkeyptr >= 256) {
        return -1;
    }
    key = freedebkeys[freedebkeyptr++];
    deb = &debkeydata[key];
    deb->count = 0;
    deb->debcount = 0;
    deb->reqcount = 0;
    deb->reqdebcount = 0;
    deb->chunks[0] = 0;
    return key;
}


inline void AddDebrisEffectToStack(struct debkeydatatype_s *node, struct debkeydatatype_s **list) {
    struct debkeydatatype_s **link;
    struct debkeydatatype_s *cur;

    if (node->next != 0) {
        node->next = 0;
    }
    link = list;
    if (*link != 0) {
        do {
            cur = *link;
            link = &cur->next;
        } while (cur->next != 0);
    }
    *link = node;
}


void AddDebrisEffect(s32 *key, s32 type, f32 x, f32 y, f32 z) {
    struct debinfo_s *info;
    s32 k;
    s32 i;

    if (type == -1) {
        return;
    }
    if (*key == -1) {
        *key = DebAlloc();
        if (*key == -1) {
            return;
        }
    }
    info = debtab[type];
    debkeydata[*key].type = type;
    debkeydata[*key].active = 1;
    DebrisStartOffset(*key, info->ival_offset);
    debkeydata[*key].oncount = debtab[type]->ival_on
                             + (s32)(rand() * debtab[type]->ival_on_ran * 4.656612873077393e-10);
    debkeydata[*key].genptr = gensorttab[info->gensort];
    debkeydata[*key].gencode = gencodetab[info->gencode];
    debkeydata[*key].rotory = 0;
    debkeydata[*key].rotorz = 0;
    debkeydata[*key].sphere_next = 0;
    debkeydata[*key].sphere_next_emit = 1;
    for (i = 0; i < debtab[type]->numspheres; i++) {
        debkeydata[*key].spheres[i].t = -1.0f;
    }
    debkeydata[*key].emitoffset.x = 0.0f;
    debkeydata[*key].emitoffset.y = 0.0f;
    debkeydata[*key].emitoffset.z = 0.0f;
    k = *key;
    if (k != -1) {
        debkeydata[k].emitpos.x = x;
        debkeydata[k].emitpos.y = y;
        debkeydata[k].emitpos.z = z;
    }
    k = *key;
    if (k != -1) {
        debkeydata[k].emitrotmtx = numtx_identity;
        NuMtxRotateZ(&debkeydata[k].emitrotmtx, 0);
        NuMtxRotateY(&debkeydata[k].emitrotmtx, 0);
    }
    k = *key;
    if (k != -1) {
        debkeydata[k].rotmtx = numtx_identity;
        NuMtxRotateZ(&debkeydata[k].rotmtx, 0);
        NuMtxRotateY(&debkeydata[k].rotmtx, 0);
    }
    k = *key;
    if (k != -1) {
        debkeydata[k].refrotz = 0;
        debkeydata[k].refroty = 0;
        debkeydata[k].refoff = 0.0f;
        debkeydata[k].refbounce = 0.9f;
    }
    AddDebrisEffectToStack(&debkeydata[*key], &debris_emitter_stack[debris_emitter_stack_index]);
}


void DebrisDraw(void) {
    struct numtx_s mtx;
    struct nuvec_s pos;
    struct debinfo_s *info;
    s32 visible;
    s32 i;

    if (render_debris_enabled == 0) {
        return;
    }
    for (i = 0; i < 256; i++) {
        if (ParticleChunkRenderStack[i].chunk == 0) {
            continue;
        }
        info = ParticleChunkRenderStack[i].debinfo;
        if (info->DmaDebTypePointer == 0) {
            continue;
        }
        visible = 1;
        if (ParticleChunkRenderStack[i].debdata != 0) {
            if (info->drawcutoff > 0.0f
                && CameraEmitterDistance(&ParticleChunkRenderStack[i].debdata->emitpos) > info->drawcutoff) {
                visible = 0;
            }
            mtx = ParticleChunkRenderStack[i].debdata->rotmtx;
            pos.x = ParticleChunkRenderStack[i].debdata->emitpos.x;
            pos.y = ParticleChunkRenderStack[i].debdata->emitpos.y;
            pos.z = ParticleChunkRenderStack[i].debdata->emitpos.z;
        } else {
            if (info->drawcutoff > 0.0f && CameraEmitterDistance(&ParticleChunkRenderStack[i].pos) > info->drawcutoff) {
                visible = 0;
            }
            mtx = ParticleChunkRenderStack[i].rotmtx;
            pos.x = ParticleChunkRenderStack[i].pos.x;
            pos.y = ParticleChunkRenderStack[i].pos.y;
            pos.z = ParticleChunkRenderStack[i].pos.z;
        }
        if (visible) {
            NuMtxTranslate(&mtx, &pos);
            NuRndrParticleGroup(ParticleChunkRenderStack[i].chunk, ParticleChunkRenderStack[i].debinfo->DmaDebTypePointer,
                                DebMat[ParticleChunkRenderStack[i].debinfo->mat], &mtx, 0, globaltime);
        }
    }
}


inline void RemoveDebrisEffectFromStack(struct debkeydatatype_s *node, struct debkeydatatype_s **list) {
    struct debkeydatatype_s *cur;

    if (*list != 0) {
        if (*list == node) {
            *list = node->next;
        } else {
            do {
                cur = *list;
                list = &cur->next;
                if (cur->next == 0) {
                    break;
                }
                if (cur->next == node) {
                    cur->next = node->next;
                    break;
                }
            } while (1);
        }
    }
    node->next = 0;
}


inline void DebReAlloc(struct debkeydatatype_s *deb, s32 count) {
    deb->reqdebcount = count;
    deb->reqcount = (count + 31) / 32;
    if (deb->reqcount == deb->count) {
        deb->debcount = count;
    } else {
        DebReAlloc2(deb);
    }
}


inline void DebFreeWithoutKey(struct debkeydatatype_s *deb) {
    s32 i;
    s32 key;

    for (i = 0; i < 256; i++) {
        if (&debkeydata[i] == deb) {
            key = i;
            DebFree(&key);
            break;
        }
    }
}


inline void DebrisDoSounds(struct debkeydatatype_s *deb, s32 type) {
    struct debinfo_s *info;
    s32 i;

    info = debtab[deb->type];
    for (i = 0; i < 4; i++) {
        if (info->sounds[i].id != -1 && info->sounds[i].type == type && deb->sound_next[i] <= 0) {
            debris_sfx = 1;
            edbitsSoundPlay(&deb->emitpos, info->sounds[i].id);
            if (type == 1 || type == 2) {
                deb->sound_next[i] = 9999;
            } else {
                deb->sound_next[i] = info->sounds[i].delay;
            }
        }
    }
}


void Debris(s32 paused) {
    struct debinfo_s *info;
    union {
        struct debkeydatatype_s *deb;
        struct chunkctrl_s *ctrl;
    } cur;
    struct debkeydatatype_s *nextdeb;
    struct chunkctrl_s *nextctrl;
    struct debpart_s *part;
    f32 dist;
    f32 root1;
    f32 root2;
    s32 keep;
    s32 slot;
    s32 i;
    s32 j;

    if (render_debris_enabled == 0 || paused != 0) {
        return;
    }
    globaltime += 1.0 / 60.0;
    globalframes++;
    cur.deb = debris_emitter_stack[debris_emitter_stack_index];
    while (cur.deb != 0) {
        nextdeb = cur.deb->next;
        info = debtab[cur.deb->type];
        switch (cur.deb->trigger_type) {
        case 0:
            break;
        case 1:
            if (cur.deb->trigger_id != -1) {
                cur.deb->active = object_switches[cur.deb->trigger_id];
            }
            break;
        }
        if (cur.deb->active != 0) {
            DebrisDoSounds(cur.deb, 1);
            DebrisDoSounds(cur.deb, 2);
            DebrisDoSounds(cur.deb, 4);
        }
        if (info->numspheres != 0) {
            for (i = 0; i < info->numspheres; i++) {
                if (cur.deb->spheres[i].t != -1.0f) {
                    cur.deb->spheres[i].t += 1.0 / 60.0;
                    if (cur.deb->spheres[i].t > info->etime) {
                        cur.deb->spheres[i].t = -1.0f;
                    }
                }
            }
            cur.deb->sphere_next_emit--;
            if (cur.deb->delay == 0 && cur.deb->active != 0 && cur.deb->sphere_next_emit <= 0) {
                struct nuvec_s v;

                cur.deb->spheres[cur.deb->sphere_next].t = 0.0f;
                v.x = 0.0f;
                v.y = info->emitmag;
                v.z = 0.0f;
                NuVecMtxTransform(&v, &v, &cur.deb->emitrotmtx);
                cur.deb->spheres[cur.deb->sphere_next].emit = v;
                cur.deb->sphere_next++;
                if (cur.deb->sphere_next >= info->numspheres) {
                    cur.deb->sphere_next = 0;
                }
                cur.deb->sphere_next_emit = info->etime * 60.0f / info->numspheres;
            }
        }
        if (cur.deb->reqdebcount != cur.deb->debcount) {
            DebReAlloc2(cur.deb);
        }
        if (cur.deb->delay == 0) {
            dist = CameraEmitterDistance(&cur.deb->emitpos);
            if (cur.deb->active != 0 && info->cuton <= dist && dist <= info->cutoff
                && (debris_render_group == 0 || cur.deb->group_id == 0 || cur.deb->group_id == debris_render_group)) {
                if (cur.deb->reqcount == 0) {
                    DebReAlloc(cur.deb, info->debnum);
                }
                if (cur.deb->count > 0) {
                    if (info->generate > 0) {
                        for (j = 0; j < info->generate; j++) {
                            cur.deb->genptr(cur.deb, info);
                            DebrisDoSounds(cur.deb, 3);
                        }
                    } else if (info->generate < 0 && globalframes % abs(info->generate) == 0) {
                        cur.deb->genptr(cur.deb, info);
                        DebrisDoSounds(cur.deb, 3);
                    }
                }
            } else if (cur.deb->reqcount != 0) {
                DebReAlloc(cur.deb, 0);
            }
            cur.deb->delay = 1;
            cur.deb->oncount--;
            if (cur.deb->oncount == 0) {
                cur.deb->delay += info->ival_off + (s32)(rand() * ((f32)info->ival_off_ran * 4.656612873077393e-10));
                cur.deb->oncount = info->ival_on + (s32)(rand() * ((f32)info->ival_on_ran * 4.656612873077393e-10));
                for (i = 0; i < 4; i++) {
                    if (info->sounds[i].id != -1) {
                        if (info->sounds[i].type == 1) {
                            cur.deb->sound_next[i] = cur.deb->delay;
                        } else if (info->sounds[i].type == 2) {
                            cur.deb->sound_next[i] = cur.deb->delay + cur.deb->oncount - 1;
                        }
                    }
                }
                if (cur.deb->instances != 0 && --cur.deb->instances == 0) {
                    DebFreeWithoutKey(cur.deb);
                    cur.deb->delay = 33;
                }
            }
        }
        if (info->numspheres != 0) {
            RemoveDebrisEffectFromStack(cur.deb, &debris_emitter_stack[debris_emitter_stack_index]);
            AddDebrisEffectToStack(cur.deb, &debris_emitter_stack[(debris_emitter_stack_index + 1) & 31]);
            cur.deb->delay--;
            for (i = 0; i < 4; i++) {
                cur.deb->sound_next[i]--;
            }
        } else if (cur.deb->delay >= 32) {
            cur.deb->delay -= 32;
            for (i = 0; i < 4; i++) {
                cur.deb->sound_next[i] -= 32;
            }
        } else {
            RemoveDebrisEffectFromStack(cur.deb, &debris_emitter_stack[debris_emitter_stack_index]);
            AddDebrisEffectToStack(cur.deb, &debris_emitter_stack[(debris_emitter_stack_index + cur.deb->delay) & 31]);
            for (i = 0; i < 4; i++) {
                cur.deb->sound_next[i] -= cur.deb->delay;
            }
            cur.deb->delay = 0;
        }
        cur.deb = nextdeb;
    }
    debris_emitter_stack_index = (debris_emitter_stack_index + 1) & 31;
    for (cur.ctrl = debris_chunk_control_stack[debris_chunk_control_stack_index]; cur.ctrl != 0; cur.ctrl = nextctrl) {
        nextctrl = cur.ctrl->next;
        if (cur.ctrl->life == 0) {
            if (cur.ctrl->state == 5) {
                f32 vel;
                f32 oldmom;
                f32 oldmove;

                info = debtab[cur.ctrl->debtype];
                part = &cur.ctrl->chunk->parts[cur.ctrl->partindex];
                keep = 0;
                vel = cur.ctrl->bouncetime * info->grav * 2.0f;
                oldmom = part->mom.y;
                oldmove = oldmom * cur.ctrl->bouncetime;
                part->mom.y = -cur.ctrl->reflect_y * (part->mom.y + vel) - vel;
                part->pos.y -= part->mom.y * cur.ctrl->bouncetime - oldmove;
                if (SolveQuadratic(info->grav, part->mom.y, part->pos.y - cur.ctrl->reflect_x, &root1, &root2)
                    && (root1 > root2 ? root1 : root2) < info->etime) {
                    cur.ctrl->life = ((root1 > root2 ? root1 : root2) - cur.ctrl->bouncetime) * 60.0f;
                    cur.ctrl->bouncetime = root1 > root2 ? root1 : root2;
                    if (cur.ctrl->life == 0) {
                        part->pos.y = cur.ctrl->reflect_x;
                        part->mom.y = 0.0f;
                    } else {
                        if (cur.ctrl->life >= 32) {
                            cur.ctrl->life -= 32;
                        } else {
                            RemoveChunkControlFromStack(cur.ctrl,
                                                        &debris_chunk_control_stack[debris_chunk_control_stack_index]);
                            AddChunkControlToStack(cur.ctrl, &debris_chunk_control_stack[(debris_chunk_control_stack_index
                                                                                          + cur.ctrl->life) & 31]);
                            cur.ctrl->life = 0;
                        }
                        keep = 1;
                    }
                }
                if (keep == 0) {
                    RemoveChunkControlFromStack(cur.ctrl,
                                                &debris_chunk_control_stack[debris_chunk_control_stack_index]);
                    freechunkcontrolsptr--;
                    freechunkcontrols[freechunkcontrolsptr] = cur.ctrl;
                    cur.ctrl->chunk = 0;
                }
            } else if (cur.ctrl->state == 6) {
                struct nuvec_s normal = {1.0f, 0.0f, 0.0f};
                f32 dot;
                f32 oldmovex;
                f32 oldmovez;
                f32 newmovex;
                f32 newmovez;

                part = &cur.ctrl->chunk->parts[cur.ctrl->partindex];
                NuVecRotateY(&normal, &normal, cur.ctrl->reflect_a);
                dot = (part->mom.x * normal.x + part->mom.z * normal.z) * -(cur.ctrl->reflect_y + 1.0f);
                oldmovex = part->mom.x * cur.ctrl->bouncetime;
                oldmovez = part->mom.z * cur.ctrl->bouncetime;
                part->mom.x += dot * normal.x;
                part->mom.z += dot * normal.z;
                newmovex = part->mom.x * cur.ctrl->bouncetime;
                newmovez = part->mom.z * cur.ctrl->bouncetime;
                part->pos.x -= newmovex - oldmovex;
                part->pos.z -= newmovez - oldmovez;
                RemoveChunkControlFromStack(cur.ctrl, &debris_chunk_control_stack[debris_chunk_control_stack_index]);
                freechunkcontrolsptr--;
                freechunkcontrols[freechunkcontrolsptr] = cur.ctrl;
                cur.ctrl->chunk = 0;
            } else if (cur.ctrl->state == 3) {
                freedebchkptr--;
                freedebchunks[freedebchkptr] = cur.ctrl->chunk;
                RemoveChunkControlFromStack(cur.ctrl, &debris_chunk_control_stack[debris_chunk_control_stack_index]);
                freechunkcontrolsptr--;
                freechunkcontrols[freechunkcontrolsptr] = cur.ctrl;
                cur.ctrl->chunk = 0;
            } else if (cur.ctrl->state == 2) {
                LinkDmaParticalSets(&cur.ctrl->chunk, 1);
                RemoveChunkControlFromStack(cur.ctrl, &debris_chunk_control_stack[debris_chunk_control_stack_index]);
                AddChunkControlToStack(cur.ctrl,
                                       &debris_chunk_control_stack[(debris_chunk_control_stack_index + 4) & 31]);
                cur.ctrl->life = 0;
                cur.ctrl->state = 3;
            } else if (cur.ctrl->state == 0) {
                for (slot = 0; slot < 256; slot++) {
                    if (ParticleChunkRenderStack[slot].chunk == cur.ctrl->chunk) {
                        ParticleChunkRenderStack[slot].chunk = 0;
                        ParticleChunkRenderStack[slot].debinfo = 0;
                        ParticleChunkRenderStack[slot].debdata = 0;
                        break;
                    }
                }
                RemoveChunkControlFromStack(cur.ctrl, &debris_chunk_control_stack[debris_chunk_control_stack_index]);
                AddChunkControlToStack(cur.ctrl,
                                       &debris_chunk_control_stack[(debris_chunk_control_stack_index + 4) & 31]);
                cur.ctrl->life = 0;
                cur.ctrl->state = 3;
            } else if (cur.ctrl->state == 1) {
                slot = 0;
                for (i = 0; i < cur.ctrl->deb->count; i++) {
                    if (cur.ctrl->deb->chunks[i] == cur.ctrl->chunk) {
                        slot = i;
                        break;
                    }
                }
                for (i = slot; i < cur.ctrl->deb->count - 1; i++) {
                    cur.ctrl->deb->chunks[i] = cur.ctrl->deb->chunks[i + 1];
                }
                cur.ctrl->deb->chunks[cur.ctrl->deb->count - 1] = 0;
                cur.ctrl->deb->count--;
                cur.ctrl->deb->disposed--;
                if (cur.ctrl->deb->count > 0) {
                    cur.ctrl->deb->pointer -= 32;
                    cur.ctrl->deb->debcount -= 32;
                } else {
                    cur.ctrl->deb->pointer = 0;
                    cur.ctrl->deb->debcount = 0;
                }
                if (cur.ctrl->deb->count > 0) {
                    LinkDmaParticalSets(cur.ctrl->deb->chunks, cur.ctrl->deb->count);
                } else {
                    cur.ctrl->deb->chunks[0] = 0;
                }
                if (slot == 0) {
                    for (i = 0; i < 256; i++) {
                        if (ParticleChunkRenderStack[i].chunk == cur.ctrl->chunk) {
                            ParticleChunkRenderStack[i].chunk = cur.ctrl->deb->chunks[0];
                            if (cur.ctrl->deb->count == 0) {
                                ParticleChunkRenderStack[i].debinfo = 0;
                                ParticleChunkRenderStack[i].debdata = 0;
                            }
                            break;
                        }
                    }
                }
                RemoveChunkControlFromStack(cur.ctrl, &debris_chunk_control_stack[debris_chunk_control_stack_index]);
                AddChunkControlToStack(cur.ctrl,
                                       &debris_chunk_control_stack[(debris_chunk_control_stack_index + 4) & 31]);
                cur.ctrl->life = 0;
                cur.ctrl->state = 2;
            }
        } else if (cur.ctrl->life >= 32) {
            cur.ctrl->life -= 32;
        } else {
            RemoveChunkControlFromStack(cur.ctrl, &debris_chunk_control_stack[debris_chunk_control_stack_index]);
            AddChunkControlToStack(cur.ctrl, &debris_chunk_control_stack[(debris_chunk_control_stack_index
                                                                          + cur.ctrl->life) & 31]);
            cur.ctrl->life = 0;
        }
    }
    debris_chunk_control_stack_index = (debris_chunk_control_stack_index + 1) & 31;
}


s32 DebrisCollisionCheck(struct nuvec_s *pos, f32 radius) {
    struct nuvec_s v;
    struct debinfo_s *info;
    f32 t;
    f32 nt;
    f32 size;
    s32 i;
    s32 j;
    s32 k;

    size = 0.0f;
    for (i = 0; i < 256; i++) {
        if (debkeydata[i].type == 0 || debkeydata[i].type == -1) {
            continue;
        }
        info = debtab[debkeydata[i].type];
        if (info->numspheres == 0) {
            continue;
        }
        for (j = 0; j < info->numspheres; j++) {
            t = debkeydata[i].spheres[j].t;
            if (t != -1.0f) {
                v.x = debkeydata[i].spheres[j].emit.x * t;
                v.y = debkeydata[i].spheres[j].emit.y * t;
                v.z = debkeydata[i].spheres[j].emit.z * t;
                v.y += info->grav * (t * t);
                v.x += debkeydata[i].emitpos.x;
                v.y += debkeydata[i].emitpos.y;
                v.z += debkeydata[i].emitpos.z;
                nt = t / info->etime;
                k = 0;
                while (k < 7 && !(info->sphereslot[k].t <= nt && nt <= info->sphereslot[k + 1].t)) {
                    k++;
                }
                size = info->sphereslot[k].value
                     + (nt - info->sphereslot[k].t) / (info->sphereslot[k + 1].t - info->sphereslot[k].t)
                       * (info->sphereslot[k + 1].value - info->sphereslot[k].value);
            }
            if (NuVecDistSqr(pos, &v, 0) < (radius + size) * (radius + size)) {
                return i;
            }
        }
    }
    return -1;
}


void AddDebEff(s32 key, s32 param, s32 count) {
    s32 i;

    if (debkeydata[key].active != 0) {
        for (i = 0; i < count; i++) {
            debkeydata[key].genptr(&debkeydata[key], debtab[debkeydata[key].type], param);
        }
    }
}


void DebrisOff(s32 *key) {
    struct debkeydatatype_s *deb;

    if (*key != -1) {
        deb = &debkeydata[*key];
        deb->active = 0;
    }
}


void DebrisOn(s32 *key) {
    struct debkeydatatype_s *deb;

    if (*key != -1) {
        deb = &debkeydata[*key];
        deb->active = 1;
    }
}


void AddFiniteShotDebrisEffect(s32 *key, s32 type, struct nuvec_s *pos, s32 instances) {
    AddDebrisEffect(key, type, pos->x, pos->y, pos->z);
    if (*key != -1) {
        debkeydata[*key].instances = instances;
        debkeydata[*key].delay = 0;
    }
}


void AddVariableShotDebrisEffect(s32 type, struct nuvec_s *pos, s32 count, s16 rotz, s16 roty) {
    struct numtx_s emitmtx;

    emitmtx = numtx_identity;
    NuMtxRotateZ(&emitmtx, rotz);
    NuMtxRotateY(&emitmtx, roty);
    AddVariableShotDebrisEffectMtx2(type, pos, count, &emitmtx, &numtx_identity);
}


void AddVariableShotDebrisEffectMtx(s32 type, struct nuvec_s *pos, s32 count, s16 rotz, s16 roty,
                                    struct numtx_s *mtx) {
    struct numtx_s emitmtx;

    emitmtx = numtx_identity;
    NuMtxRotateZ(&emitmtx, rotz);
    NuMtxRotateY(&emitmtx, roty);
    AddVariableShotDebrisEffectMtx2(type, pos, count, &emitmtx, mtx);
}


void DebrisOrientation(s32 key, s16 rotz, s16 roty) {
    if (key != -1) {
        debkeydata[key].rotmtx = numtx_identity;
        NuMtxRotateZ(&debkeydata[key].rotmtx, rotz);
        NuMtxRotateY(&debkeydata[key].rotmtx, roty);
    }
}


void DebrisOrientationMtx(s32 key, struct numtx_s *mtx) {
    if (key != -1) {
        debkeydata[key].rotmtx = *mtx;
        debkeydata[key].rotmtx._30 = 0.0f;
        debkeydata[key].rotmtx._31 = 0.0f;
        debkeydata[key].rotmtx._32 = 0.0f;
    }
}


void DebrisPosOrientationMtx(s32 key, struct numtx_s *mtx) {
    if (key != -1) {
        debkeydata[key].rotmtx = *mtx;
        debkeydata[key].emitpos.x = mtx->_30;
        debkeydata[key].emitpos.y = mtx->_31;
        debkeydata[key].emitpos.z = mtx->_32;
        debkeydata[key].rotmtx._30 = 0.0f;
        debkeydata[key].rotmtx._31 = 0.0f;
        debkeydata[key].rotmtx._32 = 0.0f;
    }
}


void DebrisEmitterOrientation(s32 key, s16 rotz, s16 roty) {
    if (key != -1) {
        debkeydata[key].emitrotmtx = numtx_identity;
        NuMtxRotateZ(&debkeydata[key].emitrotmtx, rotz);
        NuMtxRotateY(&debkeydata[key].emitrotmtx, roty);
    }
}


void DebrisEmiterPos(s32 key, f32 x, f32 y, f32 z) {
    if (key != -1) {
        debkeydata[key].emitpos.x = x;
        debkeydata[key].emitpos.y = y;
        debkeydata[key].emitpos.z = z;
    }
}


void DebrisReflectionOrientation(s32 key, s16 a, s16 b, f32 x, f32 y) {
    if (key != -1) {
        debkeydata[key].refrotz = a;
        debkeydata[key].refroty = b;
        debkeydata[key].refoff = x;
        debkeydata[key].refbounce = y;
    }
}


void DebrisSetTrigger(s32 key, s32 trigger, s32 param, f32 value) {
    if (key != -1) {
        debkeydata[key].trigger_type = trigger;
        debkeydata[key].trigger_id = param;
        debkeydata[key].trigger_var = value;
    }
}


void DebrisSetGroupID(s32 key, s16 groupid) {
    struct debkeydatatype_s *deb;

    if (key != -1) {
        deb = &debkeydata[key];
        deb->group_id = groupid;
    }
}


void DebrisSetup(void) {
    SetupDebris();
}


void DebrisSetRenderGroup(s32 group) {
    debris_render_group = group;
}


s32 LookupDebrisEffect(char *name) {
    s32 i;

    for (i = 1; i < 128; i++) {
        if (debtab[i] != 0 && strcmp(debtab[i]->id, name) == 0) {
            return i;
        }
    }
    return -1;
}


void DebrisRegisterCutoffCameraVec(struct nuvec_s *vec) {
    CutoffCameraVec = vec;
}


f32 CameraEmitterDistance(struct nuvec_s *pos) {
    if (CutoffCameraVec == 0) {
        return 0.0f;
    }
    return NuVecDist(pos, CutoffCameraVec, 0);
}


s32 DebrisFindAllOfType(s32 type, struct nuvec_s **list, s32 max, s32 skip, s32 activeonly) {
    s32 i;
    s32 found;

    found = 0;
    for (i = 0; i < 256; i++) {
        if (debkeydata[i].type == type
            && (!activeonly || debkeydata[i].count != 0)) {
            if (found - skip >= 0 && found - skip < max) {
                list[found - skip] = &debkeydata[i].emitpos;
            }
            found++;
            if (found - skip >= max) {
                break;
            }
        }
    }
    return found - skip;
}


struct debkeydatatype_s **FindDebrisEffectStack(struct debkeydatatype_s *node) {
    struct debkeydatatype_s **list;
    struct debkeydatatype_s *cur;
    s32 i;

    for (i = 0; i < 32; i++) {
        list = &debris_emitter_stack[i];
        if (*list != 0) {
            do {
                cur = *list;
                if (cur == node) {
                    return &debris_emitter_stack[i];
                }
                list = &cur->next;
            } while (*list != 0);
        }
    }
    return 0;
}


struct debpart_s *GenDebIndexSort(struct debkeydatatype_s *deb, struct debinfo_s *info) {
    return GenDebIndex(deb, info);
}


void GenDebMomAdjFromPos(struct debkeydatatype_s *deb, struct debinfo_s *info, struct debpart_s *part) {
    part->mom.x = part->pos.x * 2.0;
    part->mom.z = part->pos.z * 2.0;
}


void GenDebMomAdjFromPosAll(struct debkeydatatype_s *deb, struct debinfo_s *info, struct debpart_s *part) {
    part->mom.x = part->pos.x * 5.4f;
    part->mom.y = part->pos.y * 5.4f;
    part->mom.z = part->pos.z * 5.4f;
}


void GenDebMomAdjFromPosRev(struct debkeydatatype_s *deb, struct debinfo_s *info, struct debpart_s *part) {
    part->mom.x = -part->pos.x;
    part->mom.z = -part->pos.z;
}


void GenDebMomAdjFromSplash(struct debkeydatatype_s *deb, struct debinfo_s *info, struct debpart_s *part) {
    part->mom.x = part->pos.x * 4.0f;
    part->mom.z = part->pos.z * 4.0f;
}


void GenDebMomAdjFromAshRock(struct debkeydatatype_s *deb, struct debinfo_s *info, struct debpart_s *part) {
    part->mom.x = part->pos.x * 16.0f;
    part->mom.z = part->pos.z * 16.0f;
}


void GenDebMomAdjFromPosRevTree(struct debkeydatatype_s *deb, struct debinfo_s *info, struct debpart_s *part) {
    part->mom.x += part->pos.x * -0.6f;
    part->mom.z += part->pos.z * -0.6f;
    part->pos.y = part->pos.y - sqrt(part->pos.x * part->pos.x + part->pos.z * part->pos.z) * 0.4f;
    part->rate = 64.0f / (info->etime + info->etime * rand() / 1503238528.0f);
}


void DebrisEmitterOrientationMtx(s32 key, struct numtx_s *mtx) {
    if (key != -1) {
        debkeydata[key].emitrotmtx = *mtx;
        debkeydata[key].emitrotmtx._30 = 0.0f;
        debkeydata[key].emitrotmtx._31 = 0.0f;
        debkeydata[key].emitrotmtx._32 = 0.0f;
    }
}
