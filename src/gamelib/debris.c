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

struct deb_s {
    s32 field_000;
    u8 pad_004[0x47C];
    s16 field_480;
    u8 pad_482[2];
    s16 on;
    s16 limit;
    u8 pad_488[2];
    s16 index;
    s16 field_48C;
    s16 field_48E;
    struct nuvec_s emitpos;
    u8 pad_49C[0x14];
    struct deb_s *next;
    u8 pad_4B4[0x94];
    s32 trigger;
    s32 trigger_param;
    f32 trigger_value;
    s16 reflect_a;
    s16 reflect_b;
    f32 reflect_x;
    f32 reflect_y;
    u8 pad_560[8];
    s16 groupid;
    u8 pad_56A[2];
};

struct debpart_s {
    struct nuvec_s pos;
    f32 time;
    struct nuvec_s mom;
    f32 rate;
};

struct chunkctrl_s {
    u8 pad_000[0x10];
    struct chunkctrl_s *next;
};

extern struct deb_s debkeydata[];
extern struct deb_s *debris_emitter_stack[];
extern s16 freedebkeys[];
extern s32 freedebkeyptr;
extern s32 debris_render_group;
extern struct nuvec_s *CutoffCameraVec;
extern f32 D_0062CEB8;
#define deb_mom_scale D_0062CEB8

void SetupDebris(void);
struct debpart_s *GenDebIndex(struct deb_s *deb, struct debinfo_s *info);
f32 NuVecDist(struct nuvec_s *a, struct nuvec_s *b, void *c);
void DebFree(s32 *key);


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


void DebrisRegisterCutoffCameraVec(struct nuvec_s *vec) {
    CutoffCameraVec = vec;
}


f32 CameraEmitterDistance(struct nuvec_s *pos) {
    if (CutoffCameraVec == 0) {
        return 0.0f;
    }
    return NuVecDist(pos, CutoffCameraVec, 0);
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


void RemoveDebrisEffectFromStack(struct deb_s *node, struct deb_s **list) {
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


void AddChunkControlToStack(struct chunkctrl_s *node, struct chunkctrl_s **list) {
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


struct debpart_s *GenDebIndexSort(struct deb_s *deb, struct debinfo_s *info) {
    return GenDebIndex(deb, info);
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


s32 DebAlloc(void) {
    s32 key;
    struct deb_s *deb;

    if (freedebkeyptr >= 256) {
        return -1;
    }
    key = freedebkeys[freedebkeyptr++];
    deb = &debkeydata[key];
    deb->field_480 = 0;
    deb->limit = 0;
    deb->field_48C = 0;
    deb->field_48E = 0;
    deb->field_000 = 0;
    return key;
}


void RemoveChunkControlFromStack(struct chunkctrl_s *node, struct chunkctrl_s **list) {
    struct chunkctrl_s *cur;

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


void DebFreeWithoutKey(struct deb_s *deb) {
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


void AddDebrisEffectToStack(struct deb_s *node, struct deb_s **list) {
    struct deb_s *cur;

    if (node->next != 0) {
        node->next = 0;
    }
    if (*list != 0) {
        do {
            cur = *list;
            list = &cur->next;
        } while (cur->next != 0);
    }
    *list = node;
}
