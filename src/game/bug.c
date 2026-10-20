/*
 * Unit: game/bug
 *
 * Functions:
 *   0x0025ee00 InitBugAreas
 *   0x0025ef88 InBugArea
 *   0x0025f080 UpdateBugLight
 *   0x0025f930 ResetBug
 *   0x0025fa00 AddBugLight
 *   0x0025fa88 DrawBug
 */

#include "nu3d/nulight.h"
#include "numath/nutrig.h"
#include "numath/nuvec.h"
#include "nu3d/nugscn.h"
#include "game/gametypes.h"
#include "game/creature.h"

extern s32 Level;

/* BUGAREA: PS2 offsets verified in InBugArea (in @0x1/0x2/0x4, out @0x9/0xA/0xC,
 * stride 0x10). */
typedef struct {
    char in_pad;      /* 0x0 */
    s8 in_iRAIL;      /* 0x1 */
    s16 in_iALONG;    /* 0x2 */
    f32 in_fALONG;    /* 0x4 */
    char out_pad;     /* 0x8 */
    s8 out_iRAIL;     /* 0x9 */
    s16 out_iALONG;   /* 0xA */
    f32 out_fALONG;   /* 0xC */
} BUGAREA;            /* 0x10 */

extern struct rail_s Rail[];
BUGAREA BugArea[4] = { 0 };

extern s32 FurtherALONG(s32 iRAIL, s32 iALONG, f32 fALONG, s32 iRAIL2,
                        s32 iALONG2, f32 fALONG2);
extern s32 FurtherBEHIND(s32 iRAIL, s32 iALONG, f32 fALONG, s32 iRAIL2,
                         s32 iALONG2, f32 fALONG2);

extern struct spltab_s SplTab[];
extern struct nuvec_s D_006FFA90;
#define bug_splpos D_006FFA90
extern f32 D_006333B8;
#define bug_splratio D_006333B8

extern s32 temp_iRAIL;
extern u16 temp_iALONG;
extern f32 temp_fALONG;

extern s32 NearestSplinePoint(struct nuvec_s *pos, struct nugspline_s *spl);
extern void PointAlongSpline(struct nugspline_s *spl, f32 ratio,
                             struct nuvec_s *out, void *a3, void *a4);
extern void GetALONG(struct nuvec_s *pos, void *rpos, s32 iRAIL, s32 iALONG,
                     s32 info);

/* File-scope statics in the retail TU (data-from-C unsupported -> extern). */
extern struct nuvec_s D_006FFA80;
#define bug_pos D_006FFA80
extern f32 D_006333A8;
#define bug_scale D_006333A8
extern f32 D_006333AC;
#define bug_span D_006333AC
extern f32 D_006333B0;
#define bug_fade D_006333B0
static f32 BUGFADETIME = 2.0f;
extern u16 D_006333B4;
#define bug_xrot D_006333B4
extern u16 D_006333B6;
#define bug_yrot D_006333B6
struct anim_s BugAnim = { 0 };
extern s32 FRAME;
extern s32 FRAMES;

/* ---- UpdateBugLight support (PS2 v1.03 PAL) ---- */
extern u64 LBIT;
extern s32 GemPath;
extern s32 VEHICLECONTROL;
extern f32 NuTrigTable[];
extern struct RPos_s *best_cRPos;

extern struct cammtx_s GameCam[];

extern s32 qrand(void);
extern f32 NuFsqrt(f32 x);
extern u16 SeekRot(u16 a, u16 target, s32 rate);
extern void AddGameDebris(s32 type, struct nuvec_s *pos);

static f32 mechlight_distance = 3.0f;
static f32 mechlight_radius = 5.0f;
static f32 buglight_distance = 3.0f;
static f32 buglight_radius = 5.0f;

extern f32 D_00633398;
#define mechlight_fade D_00633398
extern f32 D_0063339C;
#define buglight_fade D_0063339C
extern u16 D_006333A0[4];
#define buglight_ang D_006333A0

/* PS2 signatures derived from these call sites (EABI: int and float args use
 * independent register files a0.. / f12..). */
extern void Draw3DCharacter(struct nuvec_s *pos, s32 xrot, s32 yrot, s32 z,
                            struct CharacterModel *model, f32 scale, s32 action,
                            f32 anim_time, s32 last);


void InitBugAreas(void) {
    s32 index;
    s32 i;
    struct nuvec_s *vec;

    for (i = 0; i < 4; i++) {
        BugArea[i].in_iRAIL = -1;
        BugArea[i].out_iRAIL = -1;
    }
    if (SplTab[67].spl != 0) {
        for (i = 0; i < SplTab[67].spl->len; i++) {
            vec = (struct nuvec_s *)(SplTab[67].spl->pts +
                                     i * (s32)SplTab[67].spl->ptsize);
            GetALONG(vec, 0, -1, -1, 1);
            if (temp_iRAIL != -1 && (Rail + temp_iRAIL)->type == 0) {
                if ((i & 1) != 0) {
                    index = i / 2;
                    BugArea[index].out_iRAIL = temp_iRAIL;
                    BugArea[index].out_iALONG = temp_iALONG;
                    BugArea[index].out_fALONG = temp_fALONG;
                } else {
                    index = i / 2;
                    BugArea[index].in_iRAIL = temp_iRAIL;
                    BugArea[index].in_iALONG = temp_iALONG;
                    BugArea[index].in_fALONG = temp_fALONG;
                }
            }
        }
    }
    bug_splratio = 0.0f;
}

void ResetBug(void) {
    s32 i;

    ResetAnimPacket(&BugAnim, 0x22);
    bug_fade = 0.0f;
    i = NearestSplinePoint(&player->obj.pos, SplTab[70].spl);
    if (i != -1) {
        bug_splratio = (f32)i / (f32)(s32)(SplTab[70].spl->len - 1U);
    } else {
        bug_splratio = 0.0f;
    }
    PointAlongSpline(SplTab[70].spl, bug_splratio, &bug_splpos, 0, 0);
    bug_splpos = bug_pos;
}

s32 InBugArea(s32 iRAIL, s32 iALONG, f32 fALONG) {
    s32 i;

    if (iRAIL != -1 && (Rail + iRAIL)->type == 0) {
        for (i = 0; i < 4; i++) {
            if (BugArea[i].in_iRAIL != -1 &&
                FurtherALONG(iRAIL, iALONG, fALONG, BugArea[i].in_iRAIL,
                             BugArea[i].in_iALONG, BugArea[i].in_fALONG) != 0) {
                if (BugArea[i].out_iRAIL == -1) {
                    return i;
                }
                if (FurtherBEHIND(iRAIL, iALONG, fALONG, BugArea[i].out_iRAIL,
                                  BugArea[i].out_iALONG,
                                  BugArea[i].out_fALONG) != 0) {
                    return i;
                }
            }
        }
    }
    return -1;
}

void UpdateBugLight(struct creature_s *c) {
    struct nuvec_s oldpos;
    struct nuvec_s v;
    struct nuvec_s v0;
    struct nuvec_s v1;
    struct nuvec_s pos;
    f32 fade;
    f32 r;
    f32 r0;
    f32 r1;
    f32 d;
    f32 d0;
    f32 d1;
    f32 dr;
    f32 rnd;
    u16 ang;

    bug_fade = 0.0f;
    if ((LBIT & 0x88100000) == 0) {
        return;
    }
    if (((Level == 0x14) && (GemPath != 0)) ||
        ((InBugArea((s32)c->obj.RPos.iRAIL, (s32)c->obj.RPos.iALONG,
                    c->obj.RPos.fALONG) != -1) &&
         (((LBIT & 0x80100000) == 0) || (VEHICLECONTROL == 1)))) {
        fade = BUGFADETIME;
    } else {
        fade = 0.0f;
    }
    buglight_ang[3] += qrand() / 0x40 * 6;
    if ((LBIT & 0x80100000) != 0) {
        if (mechlight_fade > fade) {
            mechlight_fade -= 0.02f;
            if (mechlight_fade < fade) {
                mechlight_fade = fade;
            }
        } else if (mechlight_fade < fade) {
            mechlight_fade += 0.02f;
            if (mechlight_fade > fade) {
                mechlight_fade = fade;
            }
        }
        bug_scale = buglight_radius + NuTrigTable[buglight_ang[3]] * buglight_radius * 0.025f;
        if (mechlight_fade > 0.0f) {
            bug_pos.x = player->obj.pos.x;
            bug_pos.y = player->obj.pos.y + player->obj.top * player->obj.SCALE;
            bug_pos.z = player->obj.pos.z;
            ang = NuAtan2D(bug_pos.x - GameCam[0].pos.x, bug_pos.z - GameCam[0].pos.z);
            bug_pos.x = bug_pos.x + NuTrigTable[ang] * mechlight_distance;
            bug_span = 1.5f;
            bug_pos.z = bug_pos.z + NuTrigTable[(u16)(ang + 0x4000)] * mechlight_distance;
        }
        bug_fade = mechlight_fade;
        rnd = (f32)qrand();
        d0 = mechlight_radius + (rnd * (1.0f / 65535.0f) - 0.5f) * 0.25f;
        bug_scale = bug_scale + (d0 - bug_scale) * 0.02f;
        return;
    }
    if (Level != 0x1b) {
        return;
    }
    oldpos = bug_pos;
    if (SplTab[70].spl != 0) {
        pos = player->obj.pos;
        if (best_cRPos != 0) {
            r = buglight_distance;
            if ((best_cRPos->mode & 0xc) != 0) {
                r = buglight_distance * 0.333f;
            }
            pos.x = pos.x + NuTrigTable[best_cRPos->angle] * r;
            pos.z = pos.z + NuTrigTable[(u16)(best_cRPos->angle + 0x4000)] * r;
        }
        d0 = bug_splratio;
        PointAlongSpline(SplTab[70].spl, d0, &v, 0, 0);
        r = NuVecDistSqr(&pos, &v, 0);
        dr = 0.001f;
        if (c->sprint != 0) {
            dr = 0.002f;
        }
        r0 = bug_splratio - dr;
        if (r0 < 0.0f) {
            r0 = 0.0f;
        }
        PointAlongSpline(SplTab[70].spl, r0, &v0, 0, 0);
        d = NuVecDistSqr(&pos, &v0, 0);
        r1 = bug_splratio + dr;
        if (r1 > 1.0f) {
            r1 = 1.0f;
        }
        PointAlongSpline(SplTab[70].spl, r1, &v1, 0, 0);
        d1 = NuVecDistSqr(&pos, &v1, 0);
        if ((d < r) && (d < d1)) {
            d0 = r0;
        } else if ((d1 < r) && (d1 < d)) {
            d0 = r1;
        }
        if (bug_splratio > d0) {
            bug_splratio = bug_splratio - dr * 0.02f * 10.0f;
            if (bug_splratio < d0) {
                bug_splratio = d0;
            }
        } else if (bug_splratio < d0) {
            bug_splratio = bug_splratio + dr * 0.02f * 10.0f;
            if (bug_splratio > d0) {
                bug_splratio = d0;
            }
        }
        PointAlongSpline(SplTab[70].spl, bug_splratio, &pos, 0, 0);
        bug_splpos.x = bug_splpos.x + (pos.x - bug_splpos.x) * 0.3f;
        bug_splpos.y = bug_splpos.y + (pos.y - bug_splpos.y) * 0.3f;
        bug_splpos.z = bug_splpos.z + (pos.z - bug_splpos.z) * 0.3f;
        bug_pos = bug_splpos;
    } else {
        bug_pos.x = player->obj.pos.x;
        bug_pos.y = player->obj.pos.y + player->obj.top * player->obj.SCALE;
        bug_pos.z = player->obj.pos.z;
        ang = NuAtan2D(bug_pos.x - GameCam[0].pos.x, bug_pos.z - GameCam[0].pos.z);
        bug_pos.x = bug_pos.x + NuTrigTable[ang] * buglight_distance;
        bug_pos.z = bug_pos.z + NuTrigTable[(u16)(ang + 0x4000)] * buglight_distance;
    }
    buglight_ang[0] += 0x276;
    buglight_ang[1] += 0x162;
    buglight_ang[2] += 0x414;
    bug_scale = buglight_radius + NuTrigTable[buglight_ang[3]] * buglight_radius * 0.025f;
    bug_pos.x = bug_pos.x + NuTrigTable[buglight_ang[0]] * 0.25f;
    bug_pos.y = bug_pos.y + NuTrigTable[buglight_ang[1]] * 0.25f;
    bug_pos.z = bug_pos.z + NuTrigTable[buglight_ang[2]] * 0.25f;
    NuVecSub(&v, &bug_pos, &oldpos);
    ang = NuAtan2D(v.y, NuFsqrt(v.x * v.x + v.z * v.z));
    bug_xrot = SeekRot(bug_xrot, ang, 3);
    ang = NuAtan2D(v.x, v.z);
    bug_yrot = SeekRot(bug_yrot, ang, 3);
    if (CRemap[174] != -1) {
        BugAnim.oldaction = BugAnim.action;
        UpdateAnimPacket(&CModel[CRemap[174]], &BugAnim, 0.59999996f, 0.0f);
    }
    if (buglight_fade > fade) {
        buglight_fade -= 0.02f;
        if (buglight_fade < fade) {
            buglight_fade = fade;
        }
    } else if (buglight_fade < fade) {
        buglight_fade += 0.02f;
        if (buglight_fade > fade) {
            buglight_fade = fade;
        }
    }
    if (buglight_fade > 0.0f) {
        bug_span = 1.0f;
    }
    bug_fade = buglight_fade;
    if (0x7fff < qrand()) {
        return;
    }
    AddGameDebris(0x9c, &bug_pos);
}

void AddBugLight(void) {
    if (bug_fade > 0.0f) {
        if (FRAME == FRAMES - 1) {
            NuLightAddSpotXSpanFade(&bug_pos, bug_scale, bug_span,
                                    0xFF000000, 1,
                                    128 - (s32)(bug_fade * (128.0f / BUGFADETIME)),
                                    1);
        }
    }
}

void DrawBug(void) {
    if (Level == 0x1b && CRemap[174] != -1) {
        Draw3DCharacter(&bug_pos, bug_xrot, (bug_yrot - 0x8000) & 0xffff,
                        0, &CModel[CRemap[174]], 1.25f, BugAnim.action,
                        BugAnim.anim_time, 0);
    }
}
