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

#include "creature.h"

struct debinfo_s;
struct debpart_s;

struct debemit_s {
    struct nuvec_s mom;
    f32 time;
};
struct debchunk_s;

struct deb_s {
    struct debchunk_s *chunks[256];
    struct numtx_s rotmtx;
    struct numtx_s emitrotmtx;
    s16 chunkcount;
    s16 debtype;
    s16 on;
    s16 limit;
    s16 ctrlcount;
    s16 index;
    s16 reqchunkcount;
    s16 reqcount;
    struct nuvec_s emitpos;
    struct nuvec_s emitoffset;
    struct debpart_s *(*gensort)();
    void (*gencode)(struct deb_s *deb, struct debinfo_s *info, struct debpart_s *part);
    struct deb_s *next;
    u8 pad_4B4[4];
    s16 rotory;
    s16 rotorz;
    s32 instances;
    s16 oncount;
    s16 delay;
    struct debemit_s emits[8];
    s16 emitflag;
    s16 emitting;
    s32 trigger;
    s32 trigger_param;
    f32 trigger_value;
    s16 reflect_a;
    s16 reflect_b;
    f32 reflect_x;
    f32 reflect_y;
    s16 soundtimer[4];
    s16 groupid;
    u8 pad_56A[2];
};

struct debpart_s {
    struct nuvec_s pos;
    f32 time;
    struct nuvec_s mom;
    f32 rate;
};

struct debsound_s {
    s32 id;
    s32 type;
    s16 delay;
    u8 pad_0A[2];
};

struct debkey_s {
    f32 time;
    f32 value;
};

struct debcolkey_s {
    f32 time;
    f32 r;
    f32 g;
    f32 b;
};

struct debdma_s;

union debchannel_u {
    f32 coord;
    u8 colour;
    u32 bits;
};

struct debinfo_s {
    char name[0x10];
    u8 pad_10[2];
    s16 emitrate;
    s16 partcount;
    s16 startoffset;
    s16 ival_on;
    s16 ival_on_ran;
    s16 ival_off;
    s16 ival_off_ran;
    s8 gensort;
    s8 gencode;
    s8 mtlindex;
    u8 pad_23[5];
    f32 mindist;
    f32 maxdist;
    f32 cutoff;
    f32 mom_ybias;
    struct nuvec_s variable_emit;
    struct nuvec_s variable_scale;
    struct nuvec_s pos_offset;
    struct nuvec_s pos_offset_scale;
    struct nuvec_s pos_range;
    struct nuvec_s pos_base;
    struct nuvec_s mom_range;
    struct nuvec_s mom_base;
    f32 gtime;
    f32 etime;
    s16 frames;
    s8 columns;
    s8 rows;
    union debchannel_u frame;
    f32 wobble_xfreq;
    f32 wobble_xamp;
    f32 wobble_yfreq;
    f32 wobble_yamp;
    struct debcolkey_s colkeys[8];
    struct debkey_s alphakeys[8];
    u8 pad_178[8];
    struct debkey_s widthkeys[8];
    struct debkey_s heightkeys[8];
    u8 pad_200[8];
    struct debkey_s rotkeys[8];
    u8 pad_248[0x80];
    f32 u0;
    f32 v0;
    f32 u1;
    f32 v1;
    struct debdma_s *dmatype;
    struct debkey_s radiuskeys[8];
    s8 emitcount;
    u8 pad_31D[3];
    s32 key;
    struct debsound_s sounds[4];
};

struct debtri_s {
    union debchannel_u v0x;
    union debchannel_u v0y;
    union debchannel_u v1x;
    union debchannel_u v1y;
    f32 v2x;
    f32 v2y;
    u8 pad_18[8];
};

struct debdma_s {
    u8 pad_00[0x14];
    f32 gtime;
    u8 pad_18[0x18];
    s32 u0;
    s32 v0;
    s32 u1;
    s32 v1;
    s32 u2;
    s32 v2;
    s32 u3;
    s32 v3;
    u8 pad_50[0x10];
    struct debtri_s tri[64];
};

struct debchunk_s {
    u8 pad_00[0x20];
    struct debpart_s parts[32];
};

struct chunkctrl_s {
    struct debchunk_s *chunk;
    s32 life;
    s32 state;
    struct deb_s *deb;
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

struct debrender_s {
    struct debchunk_s *chunk;
    struct debinfo_s *info;
    struct deb_s *deb;
    struct numtx_s mtx;
    struct nuvec_s pos;
};

struct numtlattrib_s {
    u64 unk0 : 32;
    u64 alpha : 4;
    u64 filter : 2;
    u64 unk38 : 2;
    u64 utc : 2;
    u64 vtc : 2;
    u64 cull : 2;
    u64 zmode : 2;
    u64 unk48 : 16;
};

struct numtlattrib2_s {
    u64 fx : 2;
    u64 blend : 1;
    u64 unk3 : 1;
    u64 dither : 1;
    u64 unk5 : 59;
};

struct numtl_s {
    u8 pad000[0x168];
    struct numtlattrib_s attrib;
    struct numtlattrib2_s attrib2;
    u8 pad178[8];
    struct nucolour3_s diffuse;
    u8 pad18C[0x14];
    f32 alpha;
    s32 tid;
    s32 alpha_sort;
};

struct nutex_s {
    s32 type;
    s32 width;
    s32 height;
    s32 mmcnt;
    void *bits;
    void *pal;
    s32 decal;
    s32 linear;
};

extern struct deb_s debkeydata[];
extern struct numtl_s *DebMat[8];
extern s32 debris_emitter_stack_index;
extern s32 D_0062F29C;
#define debris_draw_on D_0062F29C
extern struct debpart_s *(*gensorttab[])();
extern void (*gencodetab[])(struct deb_s *deb, struct debinfo_s *info, struct debpart_s *part);
extern struct debinfo_s *debtab[128];
extern struct numtx_s numtx_identity;
extern f32 NuTrigTable[];
extern s32 globalframes;
extern struct deb_s *debris_emitter_stack[];
extern s16 freedebkeys[];
extern s32 freedebkeyptr;
extern struct debrender_s ParticleChunkRenderStack[256];
extern struct debchunk_s *freedebchunks[];
extern s32 freedebchkptr;
extern struct chunkctrl_s *freechunkcontrols[];
extern s32 freechunkcontrolsptr;
extern struct chunkctrl_s *debris_chunk_control_stack[32];
extern s32 debris_chunk_control_stack_index;
extern s32 debris_render_group;
extern struct nuvec_s *CutoffCameraVec;
extern f32 D_0062CEB8;
#define deb_mom_scale D_0062CEB8
extern f32 D_0062CEBC;
#define deb_tree_pull D_0062CEBC
extern f32 D_0062CEC0;
#define deb_tree_rand_scale D_0062CEC0
extern s32 debris_sfx;
extern struct debdma_s *DmaDebTypes[];
extern s32 freeDmaDebType;
extern f32 globaltime;
extern u8 object_switches[];
extern f32 D_0062CEA4;
#define deb_randtime_scale D_0062CEA4

void SetupDebris(void);
struct debpart_s *GenDebIndex(struct deb_s *deb, struct debinfo_s *info);
f32 NuVecDist(struct nuvec_s *a, struct nuvec_s *b, void *c);
f32 NuVecDistSqr(struct nuvec_s *a, struct nuvec_s *b, void *c);
void DebFree(s32 *key);
void DebFreeInstantly(s32 *key);
struct deb_s **FindDebrisEffectStack(struct deb_s *node);
void RemoveDebrisEffectFromStack(struct deb_s *node, struct deb_s **list);
void DebReAlloc2(struct deb_s *deb);
void DebReAlloc(struct deb_s *deb, s32 count);
void DebFreeWithoutKey(struct deb_s *deb);
void DebrisDoSounds(struct deb_s *deb, s32 type);
void AddDebrisEffect(s32 *key, s32 type, f32 x, f32 y, f32 z);
void NuMtxRotateZ(struct numtx_s *m, s32 angle);
void NuMtxRotateY(struct numtx_s *m, s32 angle);
void NuMtxSetIdentity(struct numtx_s *m);
s32 strcmp(const char *a, const char *b);
f32 NuFsqrt(f32 x);
double sqrt(double x);
s32 rand(void);
s32 abs(s32 x);
void edbitsSoundPlay(struct nuvec_s *pos, s32 id);
void NuVecMtxTransform(struct nuvec_s *out, struct nuvec_s *in, struct numtx_s *mtx);
void NuVecRotateY(struct nuvec_s *out, struct nuvec_s *in, s32 angle);
void NuVecRotateZ(struct nuvec_s *out, struct nuvec_s *in, s32 angle);
void GenericDebinfoDmaTypeUpdate(struct debinfo_s *info);
void AddChunkControlToStack(struct chunkctrl_s *node, struct chunkctrl_s **list);
void RemoveChunkControlFromStack(struct chunkctrl_s *node, struct chunkctrl_s **list);
f32 CameraEmitterDistance(struct nuvec_s *pos);
s32 DebAlloc(void);
void AddDebrisEffectToStack(struct deb_s *node, struct deb_s **list);
void DebrisStartOffset(s32 key, s32 offset);
void NuMtxTranslate(struct numtx_s *m, struct nuvec_s *v);
void NuRndrParticleGroup(struct debchunk_s *chunk, struct debdma_s *dma, struct numtl_s *mtl, struct numtx_s *mtx, s32 flags, f32 time);
void DebrisEmitterOrientationMtx(s32 key, struct numtx_s *mtx);
void NuVecScale(struct nuvec_s *d, struct nuvec_s *s, f32 k);
s32 NuFileLoadBuffer(char *name, void *buf, s32 max);
struct numtl_s *NuMtlCreate(s32 count);
void NuMtlUpdate(struct numtl_s *mtl);
s32 NuTexCreate(struct nutex_s *tex);
void LinkDmaParticalSets(struct debchunk_s **sets, s32 count);
void NuVecSub(struct nuvec_s *d, struct nuvec_s *a, struct nuvec_s *b);


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

    if (info->dmatype == 0) {
        info->dmatype = DmaDebTypes[freeDmaDebType++];
    }
    info->dmatype->gtime = info->gtime;
    info->dmatype->u0 = info->dmatype->u2 = info->u0 * 16.0f;
    info->dmatype->u1 = info->dmatype->u3 = info->u1 * 16.0f;
    info->dmatype->v0 = info->dmatype->v1 = info->v0 * 16.0f;
    info->dmatype->v2 = info->dmatype->v3 = info->v1 * 16.0f;
    for (i = 0; i < 64; i++) {
        t = (f32)i * 0.015625f;

        j = 0;
        while (j < 7 && !(info->widthkeys[j].time <= t && t <= info->widthkeys[j + 1].time)) {
            j++;
        }
        dt = t - info->widthkeys[j].time;
        range = info->widthkeys[j + 1].time - info->widthkeys[j].time;
        width = info->widthkeys[j].value + dt / range * (info->widthkeys[j + 1].value - info->widthkeys[j].value);

        j = 0;
        while (j < 7 && !(info->heightkeys[j].time <= t && t <= info->heightkeys[j + 1].time)) {
            j++;
        }
        dt = t - info->heightkeys[j].time;
        range = info->heightkeys[j + 1].time - info->heightkeys[j].time;
        height = info->heightkeys[j].value + dt / range * (info->heightkeys[j + 1].value - info->heightkeys[j].value);

        j = 0;
        while (j < 7 && !(info->rotkeys[j].time <= t && t <= info->rotkeys[j + 1].time)) {
            j++;
        }
        dt = t - info->rotkeys[j].time;
        range = info->rotkeys[j + 1].time - info->rotkeys[j].time;
        angle = info->rotkeys[j].value + dt / range * (info->rotkeys[j + 1].value - info->rotkeys[j].value);

        tri = &info->dmatype->tri[i];
        wobx = info->wobble_xamp * NuTrigTable[(u16)(s32)(info->wobble_xfreq * t * 65536.0f)];
        woby = info->wobble_yamp * NuTrigTable[(u16)(s32)(info->wobble_yfreq * t * 65536.0f)];
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
        while (j < 7 && !(info->colkeys[j].time <= t && t <= info->colkeys[j + 1].time)) {
            j++;
        }
        dt = t - info->colkeys[j].time;
        range = info->colkeys[j + 1].time - info->colkeys[j].time;
        r = info->colkeys[j].r + dt / range * (info->colkeys[j + 1].r - info->colkeys[j].r);
        g = info->colkeys[j].g + dt / range * (info->colkeys[j + 1].g - info->colkeys[j].g);
        b = info->colkeys[j].b + dt / range * (info->colkeys[j + 1].b - info->colkeys[j].b);

        j = 0;
        while (j < 7 && !(info->alphakeys[j].time <= t && t <= info->alphakeys[j + 1].time)) {
            j++;
        }
        dt = t - info->alphakeys[j].time;
        range = info->alphakeys[j + 1].time - info->alphakeys[j].time;
        alpha = info->alphakeys[j].value + dt / range * (info->alphakeys[j + 1].value - info->alphakeys[j].value);

        tri->v0x.bits = (tri->v0x.bits & 0xFFFFFF00) + ((s32)r & 0xFF);
        tri->v0y.bits = (tri->v0y.bits & 0xFFFFFF00) + ((s32)g & 0xFF);
        tri->v1x.bits = (tri->v1x.bits & 0xFFFFFF00) + ((s32)b & 0xFF);
        tri->v1y.bits = (tri->v1y.bits & 0xFFFFFF00) + ((s32)alpha & 0xFF);
    }
}


struct debpart_s *GenDebIndex(struct deb_s *deb, struct debinfo_s *info) {
    struct nuvec_s v;
    struct debpart_s *part;

    if (deb->index >= deb->limit) {
        deb->index = 0;
    }
    part = deb->chunks[deb->index / 32]->parts;
    part += (s16)(deb->index % 32);
    deb->index++;
    part->time = globaltime;
    part->rate = 64.0f / info->etime;
    v.x = (f32)rand() * info->pos_offset_scale.x - info->pos_offset.x;
    v.y = (f32)rand() * info->pos_offset_scale.y - info->pos_offset.y;
    v.z = (f32)rand() * info->pos_offset_scale.z - info->pos_offset.z;
    NuVecMtxTransform(&v, &v, &deb->emitrotmtx);
    part->pos.x = v.x;
    part->pos.y = v.y;
    part->pos.z = v.z;
    v.x = (f32)rand() * info->variable_scale.x - info->variable_emit.x;
    v.y = (f32)rand() * info->variable_scale.y - info->variable_emit.y + info->mom_ybias;
    v.z = (f32)rand() * info->variable_scale.z - info->variable_emit.z;
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
    if (info->dmatype == 0) {
        GenericDebinfoDmaTypeUpdate(info);
    }
    return part;
}


struct debpart_s *GenDebIndexRadial(struct deb_s *deb, struct debinfo_s *info) {
    struct nuvec_s v;
    struct debpart_s *part;
    s32 roty;
    s32 rotz;
    f32 zero;

    if (deb->index >= deb->limit) {
        deb->index = 0;
    }
    part = deb->chunks[deb->index / 32]->parts;
    part += (s16)(deb->index % 32);
    deb->index++;
    part->time = globaltime;
    part->rate = 64.0f / info->etime;
    zero = 0.0f;
    roty = (f32)rand() * info->variable_scale.y - info->variable_emit.y + info->pos_offset.y;
    rotz = (f32)rand() * info->variable_scale.z - info->variable_emit.z + info->pos_offset.z;
    v.y = info->pos_offset.x;
    v.z = zero;
    v.x = zero;
    NuVecRotateZ(&v, &v, rotz);
    NuVecRotateY(&v, &v, roty);
    NuVecMtxTransform(&v, &v, &deb->emitrotmtx);
    part->pos.x = v.x;
    part->pos.y = v.y;
    part->pos.z = v.z;
    v.y = (f32)rand() * info->variable_scale.x - info->variable_emit.x + info->mom_ybias;
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
    if (info->dmatype == 0) {
        GenericDebinfoDmaTypeUpdate(info);
    }
    return part;
}


struct debpart_s *GenDebIndexRadialRotor(struct deb_s *deb, struct debinfo_s *info) {
    struct nuvec_s v;
    struct debpart_s *part;
    s32 roty;
    s32 rotz;
    f32 zero;

    if (deb->index >= deb->limit) {
        deb->index = 0;
    }
    part = deb->chunks[deb->index / 32]->parts;
    part += (s16)(deb->index % 32);
    deb->index++;
    part->time = globaltime;
    part->rate = 64.0f / info->etime;
    zero = 0.0f;
    v.z = zero;
    rotz = deb->rotorz + info->pos_offset.z;
    roty = deb->rotory + info->pos_offset.y;
    v.x = zero;
    v.y = info->pos_offset.x;
    NuVecRotateZ(&v, &v, rotz);
    NuVecRotateY(&v, &v, roty);
    NuVecMtxTransform(&v, &v, &deb->emitrotmtx);
    part->pos.x = v.x;
    part->pos.y = v.y;
    part->pos.z = v.z;
    v.y = (f32)rand() * info->variable_scale.x - info->variable_emit.x + info->mom_ybias;
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
    if (info->dmatype == 0) {
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


struct debpart_s *GenDebIndexSpheroid(struct deb_s *deb, struct debinfo_s *info) {
    struct nuvec_s v;
    struct numtx_s scale;
    struct debpart_s *part;
    s32 roty;
    s32 rotz;

    if (deb->index >= deb->limit) {
        deb->index = 0;
    }
    part = deb->chunks[deb->index / 32]->parts;
    part += (s16)(deb->index % 32);
    deb->index++;
    part->time = globaltime;
    part->rate = 64.0f / info->etime;
    v.y = rand() * 4.656612873077393e-10;
    v.x = v.z = 0.0f;
    roty = rand() * 1.52587890625e-05;
    rotz = rand() * 3.0517578125e-05;
    NuVecRotateZ(&v, &v, rotz);
    NuVecRotateY(&v, &v, roty);
    NuMtxSetIdentity(&scale);
    scale._00 = info->pos_offset.x;
    scale._11 = info->pos_offset.y;
    scale._22 = info->pos_offset.z;
    NuVecMtxTransform(&v, &v, &scale);
    NuVecMtxTransform(&v, &v, &deb->emitrotmtx);
    part->pos.x = v.x;
    part->pos.y = v.y;
    part->pos.z = v.z;
    v.x = (f32)rand() * info->variable_scale.x - info->variable_emit.x;
    v.y = (f32)rand() * info->variable_scale.y - info->variable_emit.y + info->mom_ybias;
    v.z = (f32)rand() * info->variable_scale.z - info->variable_emit.z;
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
    if (info->dmatype == 0) {
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


struct debpart_s *GenDebIndexBounceY(struct deb_s *deb, struct debinfo_s *info) {
    struct nuvec_s v;
    struct debchunk_s *chunk;
    struct debpart_s *part;
    f32 root1;
    f32 root2;

    if (deb->index >= deb->limit) {
        deb->index = 0;
    }
    chunk = deb->chunks[deb->index / 32];
    part = &chunk->parts[(s16)(deb->index % 32)];
    part->time = globaltime;
    part->rate = 64.0f / info->etime;
    v.x = (f32)rand() * info->pos_offset_scale.x - info->pos_offset.x;
    v.y = (f32)rand() * info->pos_offset_scale.y - info->pos_offset.y;
    v.z = (f32)rand() * info->pos_offset_scale.z - info->pos_offset.z;
    NuVecMtxTransform(&v, &v, &deb->emitrotmtx);
    part->pos.x = v.x;
    part->pos.y = v.y;
    part->pos.z = v.z;
    v.x = (f32)rand() * info->variable_scale.x - info->variable_emit.x;
    v.y = (f32)rand() * info->variable_scale.y - info->variable_emit.y + info->mom_ybias;
    v.z = (f32)rand() * info->variable_scale.z - info->variable_emit.z;
    NuVecMtxTransform(&v, &v, &deb->emitrotmtx);
    part->mom.x = v.x;
    part->mom.y = v.y;
    part->mom.z = v.z;
    if (SolveQuadratic(info->gtime, part->mom.y, part->pos.y - deb->reflect_x, &root1, &root2)
        && (root1 > root2 ? root1 : root2) > 0.0f && (root1 > root2 ? root1 : root2) < info->etime
        && freechunkcontrolsptr < 409) {
        freechunkcontrols[freechunkcontrolsptr]->chunk = chunk;
        freechunkcontrols[freechunkcontrolsptr]->life = (root1 > root2 ? root1 : root2) * 60.0f;
        freechunkcontrols[freechunkcontrolsptr]->state = 5;
        freechunkcontrols[freechunkcontrolsptr]->deb = 0;
        freechunkcontrols[freechunkcontrolsptr]->debtype = deb->debtype;
        freechunkcontrols[freechunkcontrolsptr]->reflect_a = 0;
        freechunkcontrols[freechunkcontrolsptr]->bounced = 0;
        freechunkcontrols[freechunkcontrolsptr]->reflect_x = deb->reflect_x;
        freechunkcontrols[freechunkcontrolsptr]->reflect_y = deb->reflect_y;
        freechunkcontrols[freechunkcontrolsptr]->partindex = (s16)(deb->index % 32);
        freechunkcontrols[freechunkcontrolsptr]->bouncetime = root1 > root2 ? root1 : root2;
        AddChunkControlToStack(freechunkcontrols[freechunkcontrolsptr],
                               &debris_chunk_control_stack[debris_chunk_control_stack_index]);
        freechunkcontrolsptr++;
    }
    if (deb->gencode != 0) {
        deb->gencode(deb, info, part);
    }
    deb->index++;
    part->pos.x += deb->emitoffset.x;
    part->pos.y += deb->emitoffset.y;
    part->pos.z += deb->emitoffset.z;
    if (info->dmatype == 0) {
        GenericDebinfoDmaTypeUpdate(info);
    }
    return part;
}


struct debpart_s *GenDebIndexBounceXZ(struct deb_s *deb, struct debinfo_s *info) {
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

    if (deb->index >= deb->limit) {
        deb->index = 0;
    }
    chunk = deb->chunks[deb->index / 32];
    part = &chunk->parts[(s16)(deb->index % 32)];
    part->time = globaltime;
    part->rate = 64.0f / info->etime;
    v.x = (f32)rand() * info->pos_offset_scale.x - info->pos_offset.x;
    v.y = (f32)rand() * info->pos_offset_scale.y - info->pos_offset.y;
    v.z = (f32)rand() * info->pos_offset_scale.z - info->pos_offset.z;
    NuVecMtxTransform(&v, &v, &deb->emitrotmtx);
    part->pos.x = v.x;
    part->pos.y = v.y;
    part->pos.z = v.z;
    v.x = (f32)rand() * info->variable_scale.x - info->variable_emit.x;
    v.y = (f32)rand() * info->variable_scale.y - info->variable_emit.y + info->mom_ybias;
    v.z = (f32)rand() * info->variable_scale.z - info->variable_emit.z;
    NuVecMtxTransform(&v, &v, &deb->emitrotmtx);
    part->mom.x = v.x;
    part->mom.y = v.y;
    part->mom.z = v.z;
    NuVecRotateY(&normal, &normal, deb->reflect_b);
    NuVecScale(&plane, &normal, deb->reflect_x);
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
            freechunkcontrols[freechunkcontrolsptr]->debtype = deb->debtype;
            freechunkcontrols[freechunkcontrolsptr]->reflect_a = deb->reflect_b;
            freechunkcontrols[freechunkcontrolsptr]->bounced = 0;
            freechunkcontrols[freechunkcontrolsptr]->reflect_x = deb->reflect_x;
            freechunkcontrols[freechunkcontrolsptr]->reflect_y = deb->reflect_y;
            freechunkcontrols[freechunkcontrolsptr]->partindex = (s16)(deb->index % 32);
            freechunkcontrols[freechunkcontrolsptr]->bouncetime = t;
            AddChunkControlToStack(freechunkcontrols[freechunkcontrolsptr],
                                   &debris_chunk_control_stack[debris_chunk_control_stack_index]);
            freechunkcontrolsptr++;
        }
    }
    if (deb->gencode != 0) {
        deb->gencode(deb, info, part);
    }
    deb->index++;
    part->pos.x += deb->emitoffset.x;
    part->pos.y += deb->emitoffset.y;
    part->pos.z += deb->emitoffset.z;
    if (info->dmatype == 0) {
        GenericDebinfoDmaTypeUpdate(info);
    }
    return part;
}


struct debpart_s *GenDebIndexPos(struct deb_s *deb, struct debinfo_s *info) {
    struct debpart_s *part;

    if (deb->index >= deb->limit) {
        deb->index = 0;
    }
    part = deb->chunks[deb->index / 32]->parts;
    part += (s16)(deb->index % 32);
    deb->index++;
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
    if (info->dmatype == 0) {
        GenericDebinfoDmaTypeUpdate(info);
    }
    return part;
}


struct debpart_s *GenDebIndexPosRandTime(struct deb_s *deb, struct debinfo_s *info) {
    struct debpart_s *part;

    if (deb->index >= deb->limit) {
        deb->index = 0;
    }
    part = deb->chunks[deb->index / 32]->parts;
    part += (s16)(deb->index % 32);
    deb->index++;
    part->time = globaltime;
    part->rate = 64.0f / (info->etime + info->etime * rand() / deb_randtime_scale);
    part->pos.x = (f32)rand() * info->pos_range.x + info->pos_base.x;
    part->pos.y = (f32)rand() * info->pos_range.y + info->pos_base.y;
    part->pos.z = (f32)rand() * info->pos_range.z + info->pos_base.z;
    part->mom.x = (f32)rand() * info->mom_range.x + info->mom_base.x;
    part->mom.y = (f32)rand() * info->mom_range.y + info->mom_base.y;
    part->mom.z = (f32)rand() * info->mom_range.z + info->mom_base.z;
    if (deb->gencode != 0) {
        deb->gencode(deb, info, part);
    }
    if (info->dmatype == 0) {
        GenericDebinfoDmaTypeUpdate(info);
    }
    return part;
}


struct debpart_s *GenDebIndexWaterFall(struct deb_s *deb, struct debinfo_s *info) {
    struct debpart_s *part;
    f32 spread;

    if (deb->index >= deb->limit) {
        deb->index = 0;
    }
    part = deb->chunks[deb->index / 32]->parts;
    part += (s16)(deb->index % 32);
    deb->index++;
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
    if (info->dmatype == 0) {
        GenericDebinfoDmaTypeUpdate(info);
    }
    return part;
}


struct debpart_s *GenDebIndexWaterFallSplash(struct deb_s *deb, struct debinfo_s *info) {
    struct debpart_s *part;
    f32 spread;
    f32 angle;
    f32 speed;

    if (deb->index >= deb->limit) {
        deb->index = 0;
    }
    part = deb->chunks[deb->index / 32]->parts;
    part += (s16)(deb->index % 32);
    deb->index++;
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
    if (info->dmatype == 0) {
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
    s32 spixel;
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
        dst--;
        pixel = (*src >> 8) | ((*src & 0xFF) << 8);
        if (pixel == 0x3E0) {
            spixel = 0;
        } else {
            spixel = pixel | 0x8000;
        }
        colour = spixel;
        *dst = ((spixel & 0x1F) * 255 / 31) << 24;
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
        mtl->attrib2.blend = 1;
        mtl->attrib2.dither = 0;
    }
    mtl->attrib.utc = 1;
    mtl->attrib.vtc = 1;
    mtl->tid = NuTexCreate(&tex);
    mtl->attrib2.fx = 1;
    mtl->alpha_sort = 0x100;
    NuMtlUpdate(mtl);
    return mtl;
}


void SplashEffect(struct debdma_s *dma) {
    struct debtri_s *tri;
    s32 i;

    dma->gtime = -0.6f;
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

    dma->gtime = 0.0f;
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


void DebReAlloc2(struct deb_s *deb) {
    s32 diff;
    s32 i;
    s32 j;

    diff = deb->reqchunkcount - deb->chunkcount;
    if (diff > 0) {
        if (diff + freedebchkptr > 256) {
            return;
        }
        for (i = 0; i < diff; i++) {
            deb->chunks[i + deb->chunkcount] = freedebchunks[i + freedebchkptr];
            for (j = 0; j < 32; j++) {
                deb->chunks[i + deb->chunkcount]->parts[j].time = 0.0f;
                deb->chunks[i + deb->chunkcount]->parts[j].rate = 32768.0f;
            }
        }
        if (deb->chunkcount == 0) {
            for (i = 0; i < 256; i++) {
                if (ParticleChunkRenderStack[i].chunk == 0) {
                    ParticleChunkRenderStack[i].chunk = deb->chunks[0];
                    ParticleChunkRenderStack[i].info = debtab[deb->debtype];
                    ParticleChunkRenderStack[i].deb = deb;
                    break;
                }
            }
        }
        deb->chunkcount = deb->reqchunkcount;
        deb->limit = deb->reqcount;
        freedebchkptr += diff;
        LinkDmaParticalSets(deb->chunks, deb->chunkcount);
        return;
    }
    if (freechunkcontrolsptr - diff > 512) {
        return;
    }
    for (i = diff; i < 0; i++) {
        freechunkcontrols[freechunkcontrolsptr]->chunk = deb->chunks[i + deb->chunkcount];
        freechunkcontrols[freechunkcontrolsptr]->life = debtab[deb->debtype]->etime * 60.0f;
        freechunkcontrols[freechunkcontrolsptr]->state = 0;
        AddChunkControlToStack(freechunkcontrols[freechunkcontrolsptr],
                               &debris_chunk_control_stack[debris_chunk_control_stack_index]);
        freechunkcontrolsptr++;
    }
    if (deb->reqcount == 0) {
        for (i = 0; i < 256; i++) {
            if (ParticleChunkRenderStack[i].chunk == deb->chunks[0]) {
                ParticleChunkRenderStack[i].info = debtab[deb->debtype];
                ParticleChunkRenderStack[i].deb = 0;
                ParticleChunkRenderStack[i].mtx = deb->rotmtx;
                ParticleChunkRenderStack[i].pos.x = deb->emitpos.x;
                ParticleChunkRenderStack[i].pos.y = deb->emitpos.y;
                ParticleChunkRenderStack[i].pos.z = deb->emitpos.z;
                break;
            }
        }
    } else {
        LinkDmaParticalSets(&deb->chunks[diff + deb->chunkcount], -diff);
        for (i = 0; i < 256; i++) {
            if (ParticleChunkRenderStack[i].chunk == 0) {
                ParticleChunkRenderStack[i].chunk = deb->chunks[diff + deb->chunkcount];
                ParticleChunkRenderStack[i].info = debtab[deb->debtype];
                ParticleChunkRenderStack[i].deb = 0;
                ParticleChunkRenderStack[i].mtx = deb->rotmtx;
                ParticleChunkRenderStack[i].pos.x = deb->emitpos.x;
                ParticleChunkRenderStack[i].pos.y = deb->emitpos.y;
                ParticleChunkRenderStack[i].pos.z = deb->emitpos.z;
                break;
            }
        }
    }
    deb->limit = deb->reqcount;
    deb->chunkcount = deb->reqchunkcount;
    if (deb->limit == 0) {
        deb->chunks[0] = 0;
    } else {
        LinkDmaParticalSets(deb->chunks, deb->chunkcount);
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
    struct deb_s **stack;
    s32 i;

    if (*key != -1) {
        if (freechunkcontrolsptr + debkeydata[*key].chunkcount > 512) {
            DebFreeInstantly(key);
            return;
        }
        for (i = 0; i < debkeydata[*key].chunkcount; i++) {
            freechunkcontrols[freechunkcontrolsptr]->chunk = debkeydata[*key].chunks[i];
            freechunkcontrols[freechunkcontrolsptr]->life = debtab[debkeydata[*key].debtype]->etime * 60.0f;
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
                ParticleChunkRenderStack[i].deb = 0;
                ParticleChunkRenderStack[i].mtx = debkeydata[*key].rotmtx;
                ParticleChunkRenderStack[i].pos.x = debkeydata[*key].emitpos.x;
                ParticleChunkRenderStack[i].pos.y = debkeydata[*key].emitpos.y;
                ParticleChunkRenderStack[i].pos.z = debkeydata[*key].emitpos.z;
                break;
            }
        }
        stack = FindDebrisEffectStack(&debkeydata[*key]);
        RemoveDebrisEffectFromStack(&debkeydata[*key], stack);
        debkeydata[*key].debtype = 0;
        freedebkeyptr--;
        freedebkeys[freedebkeyptr] = *key;
        *key = -1;
    }
}


void DebFreeInstantly(s32 *key) {
    struct deb_s **stack;
    s32 i;
    s32 j;

    if (*key != -1) {
        freedebchkptr -= debkeydata[*key].chunkcount;
        for (i = 0; i < debkeydata[*key].chunkcount; i++) {
            freedebchunks[i + freedebchkptr] = debkeydata[*key].chunks[i];
            for (j = 0; j < 256; j++) {
                if (ParticleChunkRenderStack[j].chunk == debkeydata[*key].chunks[i]) {
                    ParticleChunkRenderStack[j].chunk = 0;
                    ParticleChunkRenderStack[j].info = 0;
                }
            }
        }
        stack = FindDebrisEffectStack(&debkeydata[*key]);
        RemoveDebrisEffectFromStack(&debkeydata[*key], stack);
        debkeydata[*key].debtype = 0;
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
        type = debtab[debkeydata[key].debtype];
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
    struct deb_s *deb;
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
    if (dist < info->mindist || info->maxdist <= dist) {
        return;
    }
    if (info->key == -1) {
        if (freedebkeyptr >= 256) {
            return;
        }
        info->key = freedebkeys[freedebkeyptr++];
        deb = &debkeydata[info->key];
        deb->chunkcount = 0;
        deb->limit = 0;
        deb->index = 0;
        deb->gensort = gensorttab[info->gensort];
        deb->gencode = gencodetab[info->gencode];
        deb->rotmtx = *mtx;
        deb->rotmtx._30 = 0.0f;
        deb->rotmtx._31 = 0.0f;
        deb->rotmtx._32 = 0.0f;
        newkey = info->key;
        if (newkey != -1) {
            debkeydata[newkey].emitpos.x = 0.0f;
            debkeydata[newkey].emitpos.y = 0.0f;
            debkeydata[newkey].emitpos.z = 0.0f;
        }
    } else {
        deb = &debkeydata[info->key];
    }
    if (deb->index + count > deb->chunkcount * 32) {
        if (deb->chunkcount < 0) {
            deb->chunkcount = 0;
        }
        chunkcount = (deb->index + count + 31) / 32;
        newchunks = chunkcount - deb->chunkcount;
        if (freedebchkptr + newchunks >= 256) {
            return;
        }
        for (i = 0; i < newchunks; i++) {
            deb->chunks[deb->chunkcount++] = freedebchunks[freedebchkptr++];
        }
        deb->chunkcount = chunkcount;
        LinkDmaParticalSets(deb->chunks, chunkcount);
        if (chunkcount == newchunks) {
            for (i = 0; i < 256; i++) {
                if (ParticleChunkRenderStack[i].chunk == 0) {
                    ParticleChunkRenderStack[i].chunk = deb->chunks[0];
                    ParticleChunkRenderStack[i].info = info;
                    ParticleChunkRenderStack[i].deb = deb;
                    break;
                }
            }
        }
    }
    deb->limit += count;
    deb->emitoffset.x = pos->x;
    deb->emitoffset.y = pos->y;
    deb->emitoffset.z = pos->z;
    key = info->key;
    if (key != -1) {
        debkeydata[key].emitrotmtx = *emitmtx;
        debkeydata[key].emitrotmtx._30 = 0.0f;
        debkeydata[key].emitrotmtx._31 = 0.0f;
        debkeydata[key].emitrotmtx._32 = 0.0f;
    }
    for (i = 0; i < count; i++) {
        deb->gensort(deb, info);
    }
    if (deb->ctrlcount < deb->chunkcount) {
        i = deb->ctrlcount;
        do {
            if (freechunkcontrolsptr < 512) {
                freechunkcontrols[freechunkcontrolsptr]->chunk = deb->chunks[i];
                freechunkcontrols[freechunkcontrolsptr]->life = info->etime * 60.0f;
                freechunkcontrols[freechunkcontrolsptr]->state = 1;
                freechunkcontrols[freechunkcontrolsptr]->deb = deb;
                AddChunkControlToStack(freechunkcontrols[freechunkcontrolsptr],
                                       &debris_chunk_control_stack[debris_chunk_control_stack_index]);
                freechunkcontrolsptr++;
                deb->ctrlcount++;
            }
            i++;
        } while (i < deb->chunkcount);
    }
    if (deb->ctrlcount == deb->chunkcount) {
        for (j = 0; j < 32; j++) {
            for (ctrl = debris_chunk_control_stack[j]; ctrl != 0; ctrl = ctrl->next) {
                if (ctrl->chunk == deb->chunks[deb->ctrlcount - 1]) {
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
    struct deb_s *deb;

    if (freedebkeyptr >= 256) {
        return -1;
    }
    key = freedebkeys[freedebkeyptr++];
    deb = &debkeydata[key];
    deb->chunkcount = 0;
    deb->limit = 0;
    deb->reqchunkcount = 0;
    deb->reqcount = 0;
    deb->chunks[0] = 0;
    return key;
}


inline void AddDebrisEffectToStack(struct deb_s *node, struct deb_s **list) {
    struct deb_s **link;
    struct deb_s *cur;

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
    debkeydata[*key].debtype = type;
    debkeydata[*key].on = 1;
    DebrisStartOffset(*key, info->startoffset);
    debkeydata[*key].oncount = debtab[type]->ival_on
                             + (s32)(rand() * debtab[type]->ival_on_ran * 4.656612873077393e-10);
    debkeydata[*key].gensort = gensorttab[info->gensort];
    debkeydata[*key].gencode = gencodetab[info->gencode];
    debkeydata[*key].rotory = 0;
    debkeydata[*key].rotorz = 0;
    debkeydata[*key].emitflag = 0;
    debkeydata[*key].emitting = 1;
    for (i = 0; i < debtab[type]->emitcount; i++) {
        debkeydata[*key].emits[i].time = -1.0f;
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
        debkeydata[k].reflect_a = 0;
        debkeydata[k].reflect_b = 0;
        debkeydata[k].reflect_x = 0.0f;
        debkeydata[k].reflect_y = 0.9f;
    }
    AddDebrisEffectToStack(&debkeydata[*key], &debris_emitter_stack[debris_emitter_stack_index]);
}


void DebrisDraw(void) {
    struct numtx_s mtx;
    struct nuvec_s pos;
    struct debinfo_s *info;
    s32 visible;
    s32 i;

    if (debris_draw_on == 0) {
        return;
    }
    for (i = 0; i < 256; i++) {
        if (ParticleChunkRenderStack[i].chunk == 0) {
            continue;
        }
        info = ParticleChunkRenderStack[i].info;
        if (info->dmatype == 0) {
            continue;
        }
        visible = 1;
        if (ParticleChunkRenderStack[i].deb != 0) {
            if (info->cutoff > 0.0f
                && CameraEmitterDistance(&ParticleChunkRenderStack[i].deb->emitpos) > info->cutoff) {
                visible = 0;
            }
            mtx = ParticleChunkRenderStack[i].deb->rotmtx;
            pos.x = ParticleChunkRenderStack[i].deb->emitpos.x;
            pos.y = ParticleChunkRenderStack[i].deb->emitpos.y;
            pos.z = ParticleChunkRenderStack[i].deb->emitpos.z;
        } else {
            if (info->cutoff > 0.0f && CameraEmitterDistance(&ParticleChunkRenderStack[i].pos) > info->cutoff) {
                visible = 0;
            }
            mtx = ParticleChunkRenderStack[i].mtx;
            pos.x = ParticleChunkRenderStack[i].pos.x;
            pos.y = ParticleChunkRenderStack[i].pos.y;
            pos.z = ParticleChunkRenderStack[i].pos.z;
        }
        if (visible) {
            NuMtxTranslate(&mtx, &pos);
            NuRndrParticleGroup(ParticleChunkRenderStack[i].chunk, ParticleChunkRenderStack[i].info->dmatype,
                                DebMat[ParticleChunkRenderStack[i].info->mtlindex], &mtx, 0, globaltime);
        }
    }
}


inline void RemoveDebrisEffectFromStack(struct deb_s *node, struct deb_s **list) {
    struct deb_s *cur;

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


inline void DebReAlloc(struct deb_s *deb, s32 count) {
    deb->reqcount = count;
    deb->reqchunkcount = (count + 31) / 32;
    if (deb->reqchunkcount == deb->chunkcount) {
        deb->limit = count;
    } else {
        DebReAlloc2(deb);
    }
}


inline void DebFreeWithoutKey(struct deb_s *deb) {
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


inline void DebrisDoSounds(struct deb_s *deb, s32 type) {
    struct debinfo_s *info;
    s32 i;

    info = debtab[deb->debtype];
    for (i = 0; i < 4; i++) {
        if (info->sounds[i].id != -1 && info->sounds[i].type == type && deb->soundtimer[i] <= 0) {
            debris_sfx = 1;
            edbitsSoundPlay(&deb->emitpos, info->sounds[i].id);
            if (type == 1 || type == 2) {
                deb->soundtimer[i] = 9999;
            } else {
                deb->soundtimer[i] = info->sounds[i].delay;
            }
        }
    }
}


void Debris(s32 paused) {
    struct debinfo_s *info;
    union {
        struct deb_s *deb;
        struct chunkctrl_s *ctrl;
    } cur;
    struct deb_s *nextdeb;
    struct chunkctrl_s *nextctrl;
    struct debpart_s *part;
    f32 dist;
    f32 root1;
    f32 root2;
    s32 keep;
    s32 slot;
    s32 i;
    s32 j;

    if (debris_draw_on == 0 || paused != 0) {
        return;
    }
    globaltime += 1.0 / 60.0;
    globalframes++;
    cur.deb = debris_emitter_stack[debris_emitter_stack_index];
    while (cur.deb != 0) {
        nextdeb = cur.deb->next;
        info = debtab[cur.deb->debtype];
        switch (cur.deb->trigger) {
        case 0:
            break;
        case 1:
            if (cur.deb->trigger_param != -1) {
                cur.deb->on = object_switches[cur.deb->trigger_param];
            }
            break;
        }
        if (cur.deb->on != 0) {
            DebrisDoSounds(cur.deb, 1);
            DebrisDoSounds(cur.deb, 2);
            DebrisDoSounds(cur.deb, 4);
        }
        if (info->emitcount != 0) {
            for (i = 0; i < info->emitcount; i++) {
                if (cur.deb->emits[i].time != -1.0f) {
                    cur.deb->emits[i].time += 1.0 / 60.0;
                    if (cur.deb->emits[i].time > info->etime) {
                        cur.deb->emits[i].time = -1.0f;
                    }
                }
            }
            cur.deb->emitting--;
            if (cur.deb->delay == 0 && cur.deb->on != 0 && cur.deb->emitting <= 0) {
                struct nuvec_s v;

                cur.deb->emits[cur.deb->emitflag].time = 0.0f;
                v.x = 0.0f;
                v.y = info->mom_ybias;
                v.z = 0.0f;
                NuVecMtxTransform(&v, &v, &cur.deb->emitrotmtx);
                cur.deb->emits[cur.deb->emitflag].mom = v;
                cur.deb->emitflag++;
                if (cur.deb->emitflag >= info->emitcount) {
                    cur.deb->emitflag = 0;
                }
                cur.deb->emitting = info->etime * 60.0f / info->emitcount;
            }
        }
        if (cur.deb->reqcount != cur.deb->limit) {
            DebReAlloc2(cur.deb);
        }
        if (cur.deb->delay == 0) {
            dist = CameraEmitterDistance(&cur.deb->emitpos);
            if (cur.deb->on != 0 && info->mindist <= dist && dist <= info->maxdist
                && (debris_render_group == 0 || cur.deb->groupid == 0 || cur.deb->groupid == debris_render_group)) {
                if (cur.deb->reqchunkcount == 0) {
                    DebReAlloc(cur.deb, info->partcount);
                }
                if (cur.deb->chunkcount > 0) {
                    if (info->emitrate > 0) {
                        for (j = 0; j < info->emitrate; j++) {
                            cur.deb->gensort(cur.deb, info);
                            DebrisDoSounds(cur.deb, 3);
                        }
                    } else if (info->emitrate < 0 && globalframes % abs(info->emitrate) == 0) {
                        cur.deb->gensort(cur.deb, info);
                        DebrisDoSounds(cur.deb, 3);
                    }
                }
            } else if (cur.deb->reqchunkcount != 0) {
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
                            cur.deb->soundtimer[i] = cur.deb->delay;
                        } else if (info->sounds[i].type == 2) {
                            cur.deb->soundtimer[i] = cur.deb->delay + cur.deb->oncount - 1;
                        }
                    }
                }
                if (cur.deb->instances != 0 && --cur.deb->instances == 0) {
                    DebFreeWithoutKey(cur.deb);
                    cur.deb->delay = 33;
                }
            }
        }
        if (info->emitcount != 0) {
            RemoveDebrisEffectFromStack(cur.deb, &debris_emitter_stack[debris_emitter_stack_index]);
            AddDebrisEffectToStack(cur.deb, &debris_emitter_stack[(debris_emitter_stack_index + 1) & 31]);
            cur.deb->delay--;
            for (i = 0; i < 4; i++) {
                cur.deb->soundtimer[i]--;
            }
        } else if (cur.deb->delay >= 32) {
            cur.deb->delay -= 32;
            for (i = 0; i < 4; i++) {
                cur.deb->soundtimer[i] -= 32;
            }
        } else {
            RemoveDebrisEffectFromStack(cur.deb, &debris_emitter_stack[debris_emitter_stack_index]);
            AddDebrisEffectToStack(cur.deb, &debris_emitter_stack[(debris_emitter_stack_index + cur.deb->delay) & 31]);
            for (i = 0; i < 4; i++) {
                cur.deb->soundtimer[i] -= cur.deb->delay;
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
                vel = cur.ctrl->bouncetime * info->gtime * 2.0f;
                oldmom = part->mom.y;
                oldmove = oldmom * cur.ctrl->bouncetime;
                part->mom.y = -cur.ctrl->reflect_y * (part->mom.y + vel) - vel;
                part->pos.y -= part->mom.y * cur.ctrl->bouncetime - oldmove;
                if (SolveQuadratic(info->gtime, part->mom.y, part->pos.y - cur.ctrl->reflect_x, &root1, &root2)
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
                        ParticleChunkRenderStack[slot].info = 0;
                        ParticleChunkRenderStack[slot].deb = 0;
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
                for (i = 0; i < cur.ctrl->deb->chunkcount; i++) {
                    if (cur.ctrl->deb->chunks[i] == cur.ctrl->chunk) {
                        slot = i;
                        break;
                    }
                }
                for (i = slot; i < cur.ctrl->deb->chunkcount - 1; i++) {
                    cur.ctrl->deb->chunks[i] = cur.ctrl->deb->chunks[i + 1];
                }
                cur.ctrl->deb->chunks[cur.ctrl->deb->chunkcount - 1] = 0;
                cur.ctrl->deb->chunkcount--;
                cur.ctrl->deb->ctrlcount--;
                if (cur.ctrl->deb->chunkcount > 0) {
                    cur.ctrl->deb->index -= 32;
                    cur.ctrl->deb->limit -= 32;
                } else {
                    cur.ctrl->deb->index = 0;
                    cur.ctrl->deb->limit = 0;
                }
                if (cur.ctrl->deb->chunkcount > 0) {
                    LinkDmaParticalSets(cur.ctrl->deb->chunks, cur.ctrl->deb->chunkcount);
                } else {
                    cur.ctrl->deb->chunks[0] = 0;
                }
                if (slot == 0) {
                    for (i = 0; i < 256; i++) {
                        if (ParticleChunkRenderStack[i].chunk == cur.ctrl->chunk) {
                            ParticleChunkRenderStack[i].chunk = cur.ctrl->deb->chunks[0];
                            if (cur.ctrl->deb->chunkcount == 0) {
                                ParticleChunkRenderStack[i].info = 0;
                                ParticleChunkRenderStack[i].deb = 0;
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
        if (debkeydata[i].debtype == 0 || debkeydata[i].debtype == -1) {
            continue;
        }
        info = debtab[debkeydata[i].debtype];
        if (info->emitcount == 0) {
            continue;
        }
        for (j = 0; j < info->emitcount; j++) {
            t = debkeydata[i].emits[j].time;
            if (t != -1.0f) {
                v.x = debkeydata[i].emits[j].mom.x * t;
                v.y = debkeydata[i].emits[j].mom.y * t;
                v.z = debkeydata[i].emits[j].mom.z * t;
                v.y += info->gtime * (t * t);
                v.x += debkeydata[i].emitpos.x;
                v.y += debkeydata[i].emitpos.y;
                v.z += debkeydata[i].emitpos.z;
                nt = t / info->etime;
                k = 0;
                while (k < 7 && !(info->radiuskeys[k].time <= nt && nt <= info->radiuskeys[k + 1].time)) {
                    k++;
                }
                size = info->radiuskeys[k].value
                     + (nt - info->radiuskeys[k].time) / (info->radiuskeys[k + 1].time - info->radiuskeys[k].time)
                       * (info->radiuskeys[k + 1].value - info->radiuskeys[k].value);
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

    if (debkeydata[key].on != 0) {
        for (i = 0; i < count; i++) {
            debkeydata[key].gensort(&debkeydata[key], debtab[debkeydata[key].debtype], param);
        }
    }
}


void DebrisOff(s32 *key) {
    struct deb_s *deb;

    if (*key != -1) {
        deb = &debkeydata[*key];
        deb->on = 0;
    }
}


void DebrisOn(s32 *key) {
    struct deb_s *deb;

    if (*key != -1) {
        deb = &debkeydata[*key];
        deb->on = 1;
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
        debkeydata[key].reflect_a = a;
        debkeydata[key].reflect_b = b;
        debkeydata[key].reflect_x = x;
        debkeydata[key].reflect_y = y;
    }
}


void DebrisSetTrigger(s32 key, s32 trigger, s32 param, f32 value) {
    if (key != -1) {
        debkeydata[key].trigger = trigger;
        debkeydata[key].trigger_param = param;
        debkeydata[key].trigger_value = value;
    }
}


void DebrisSetGroupID(s32 key, s16 groupid) {
    struct deb_s *deb;

    if (key != -1) {
        deb = &debkeydata[key];
        deb->groupid = groupid;
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
        if (debtab[i] != 0 && strcmp(debtab[i]->name, name) == 0) {
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
        if (debkeydata[i].debtype == type
            && (!activeonly || debkeydata[i].chunkcount != 0)) {
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


struct deb_s **FindDebrisEffectStack(struct deb_s *node) {
    struct deb_s **list;
    struct deb_s *cur;
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


struct debpart_s *GenDebIndexSort(struct deb_s *deb, struct debinfo_s *info) {
    return GenDebIndex(deb, info);
}


void GenDebMomAdjFromPos(struct deb_s *deb, struct debinfo_s *info, struct debpart_s *part) {
    part->mom.x = part->pos.x * 2.0;
    part->mom.z = part->pos.z * 2.0;
}


void GenDebMomAdjFromPosAll(struct deb_s *deb, struct debinfo_s *info, struct debpart_s *part) {
    part->mom.x = part->pos.x * deb_mom_scale;
    part->mom.y = part->pos.y * deb_mom_scale;
    part->mom.z = part->pos.z * deb_mom_scale;
}


void GenDebMomAdjFromPosRev(struct deb_s *deb, struct debinfo_s *info, struct debpart_s *part) {
    part->mom.x = -part->pos.x;
    part->mom.z = -part->pos.z;
}


void GenDebMomAdjFromSplash(struct deb_s *deb, struct debinfo_s *info, struct debpart_s *part) {
    part->mom.x = part->pos.x * 4.0f;
    part->mom.z = part->pos.z * 4.0f;
}


void GenDebMomAdjFromAshRock(struct deb_s *deb, struct debinfo_s *info, struct debpart_s *part) {
    part->mom.x = part->pos.x * 16.0f;
    part->mom.z = part->pos.z * 16.0f;
}


void GenDebMomAdjFromPosRevTree(struct deb_s *deb, struct debinfo_s *info, struct debpart_s *part) {
    part->mom.x += part->pos.x * deb_tree_pull;
    part->mom.z += part->pos.z * deb_tree_pull;
    part->pos.y = part->pos.y - sqrt(part->pos.x * part->pos.x + part->pos.z * part->pos.z) * 0.4f;
    part->rate = 64.0f / (info->etime + info->etime * rand() / deb_tree_rand_scale);
}


struct numtl_s *CreateCopyMat(struct numtl_s *src, s32 blend, s32 alphamode, s32 zmode, s32 filter) {
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
        mtl->attrib2.blend = 1;
        mtl->attrib2.dither = 0;
    }
    mtl->attrib.utc = 1;
    mtl->attrib.vtc = 1;
    mtl->tid = src->tid;
    mtl->alpha_sort = src->alpha_sort;
    mtl->attrib2.fx = 1;
    NuMtlUpdate(mtl);
    return mtl;
}


void DebrisEmitterOrientationMtx(s32 key, struct numtx_s *mtx) {
    if (key != -1) {
        debkeydata[key].emitrotmtx = *mtx;
        debkeydata[key].emitrotmtx._30 = 0.0f;
        debkeydata[key].emitrotmtx._31 = 0.0f;
        debkeydata[key].emitrotmtx._32 = 0.0f;
    }
}
