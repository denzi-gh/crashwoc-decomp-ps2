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

#include "creature.h"

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

/* Rail[i].type @0x26, stride 0x28 (mirrors src/game/ai.c). */
struct rail_s {
    u8 unk_0x00[0x26];
    s8 type;
    u8 unk_0x27;
};

extern struct rail_s Rail[];
extern BUGAREA BugArea[4];

extern s32 FurtherALONG(s32 iRAIL, s32 iALONG, f32 fALONG, s32 iRAIL2,
                        s32 iALONG2, f32 fALONG2);
extern s32 FurtherBEHIND(s32 iRAIL, s32 iALONG, f32 fALONG, s32 iRAIL2,
                         s32 iALONG2, f32 fALONG2);

/* spline table: entry stride 0x18, .spl @0x0 (SplTab[70]=0x690, SplTab[67]=0x648).
 * spline .len @0x0, .ptsize @0x2, .pts @0x8 (verified in InitBugAreas). */
struct spline_s {
    s16 len;          /* 0x0 */
    s16 ptsize;       /* 0x2 */
    u8 pad4[0x4];     /* 0x4 */
    u8 *pts;          /* 0x8 */
};
struct spltab_s {
    struct spline_s *spl;   /* 0x0 */
    u8 pad[0x14];
};

extern struct spltab_s SplTab[];
extern struct nuvec_s D_006FFA90;   /* bug_splpos */
extern f32 D_006333B8;              /* bug_splratio */

extern s32 temp_iRAIL;
extern u16 temp_iALONG;
extern f32 temp_fALONG;

extern s32 NearestSplinePoint(struct nuvec_s *pos, struct spline_s *spl);
extern void PointAlongSpline(struct spline_s *spl, f32 ratio,
                             struct nuvec_s *out, void *a3, void *a4);
extern void GetALONG(struct nuvec_s *pos, void *rpos, s32 iRAIL, s32 iALONG,
                     s32 info);

/* File-scope statics in the retail TU (data-from-C unsupported -> extern). */
extern struct nuvec_s D_006FFA80;   /* bug_pos */
extern f32 D_006333A8;              /* bug_scale */
extern f32 D_006333AC;              /* spot fade arg */
extern f32 D_006333B0;              /* bug_fade (guard + alpha) */
extern f32 D_00632CD4;              /* alpha divisor */
extern u16 D_006333B4;              /* bug_xrot */
extern u16 D_006333B6;              /* bug_yrot */
extern struct anim_s BugAnim;
extern s32 FRAME;
extern s32 FRAMES;

/* ---- UpdateBugLight support (PS2 v1.03 PAL) ---- */
extern u64 LBIT;
extern s32 GemPath;
extern s32 VEHICLECONTROL;
extern f32 NuTrigTable[];
extern struct RPos_s *best_cRPos;

/* GameCam: only pos.x/y/z (0xA4/0xA8/0xAC) are touched here. */
struct gamecam_s {
    char pad_a4[0xA4];
    f32 x; /* 0xA4 */
    f32 y; /* 0xA8 */
    f32 z; /* 0xAC */
};
extern struct gamecam_s GameCam;

extern s32 qrand(void);
extern s32 NuAtan2D(f32 x, f32 z);
extern f32 NuVecDistSqr(struct nuvec_s *a, struct nuvec_s *b, struct nuvec_s *d);
extern void NuVecSub(struct nuvec_s *d, struct nuvec_s *a, struct nuvec_s *b);
extern f32 NuFsqrt(f32 x);
extern u16 SeekRot(u16 a, u16 target, s32 rate);
extern void AddGameDebris(s32 type, struct nuvec_s *pos);
extern void UpdateAnimPacket(struct CharacterModel *mod, struct anim_s *anim,
                             f32 dt, f32 xz_distance);

/* Retail-owned .sdata scalars this TU never defines (data-from-C unsupported).
 * Named-global "constants" (fade/scale rates) held gp-relative in the PS2 build
 * -- each is its own gp_rel symbol, so reference them individually. */
extern f32 D_00632CD8; /* mechlight_distance                */
extern f32 D_00632CDC; /* mech-scale random target base     */
extern f32 D_00632CE0; /* buglight_distance_                */
extern f32 D_00632CE4; /* bug-scale wobble base             */
extern f32 D_0062E914; /* mechlight_fade decrement          */
extern f32 D_0062E918; /* mechlight_fade increment          */
extern f32 D_0062E91C; /* mech bug-scale wobble factor      */
extern f32 D_0062E920; /* mech bug-scale random factor      */
extern f32 D_0062E924; /* mech bug-scale lerp factor        */
extern f32 D_0062E928; /* best_cRPos distance multiplier    */
extern f32 D_0062E92C; /* spline dr base                    */
extern f32 D_0062E930; /* spline dr when sprinting          */
extern f32 D_0062E934; /* bug_splratio decrement factor     */
extern f32 D_0062E938; /* bug_splratio increment factor     */
extern f32 D_0062E93C; /* bug_splpos lerp factor            */
extern f32 D_0062E940; /* tail bug-scale wobble factor      */
extern f32 D_0062E944; /* UpdateAnimPacket dt               */
extern f32 D_0062E948; /* buglight_fade decrement           */
extern f32 D_0062E94C; /* buglight_fade increment           */

extern f32 D_00633398; /* mechlight_fade  */
extern f32 D_0063339C; /* buglight_fade   */
extern u16 D_006333A0; /* buglight_ang[0] */
extern u16 D_006333A2; /* buglight_ang[1] */
extern u16 D_006333A4; /* buglight_ang[2] */
extern u16 D_006333A6; /* buglight_ang[3] */
/* D_006333A8 bug_scale, D_006333AC spot-span, D_006333B0 bug_fade,
 * D_006333B4 bug_xrot, D_006333B6 bug_yrot, D_006333B8 bug_splratio,
 * D_006FFA80 bug_pos, D_006FFA90 bug_splpos, D_00632CD4 BUGFADETIME
 * are declared above.  All struct offsets re-derived from PS2 loads/stores:
 *   c->obj.RPos.iRAIL +0x48, iALONG +0x4A, fALONG +0x50 (InBugArea args)
 *   c->sprint +0xCBF ; player->obj.pos +0x6C, SCALE +0x108, top +0x120
 *   GameCam.x +0xA4 / .z +0xAC ; best_cRPos->angle +0x10, mode +0x14 */
#define BUGFADETIME D_00632CD4

/* PS2 signatures derived from these call sites (EABI: int and float args use
 * independent register files a0.. / f12..). */
extern void NuLightAddSpotXSpanFade(struct nuvec_s *pos, f32 scale, f32 fade,
                                    u32 color, s32 a4, s32 alpha, s32 a6);
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
    D_006333B8 = 0.0f;
}

void ResetBug(void) {
    s32 i;

    ResetAnimPacket(&BugAnim, 0x22);
    D_006333B0 = 0.0f;
    i = NearestSplinePoint(&player->obj.pos, SplTab[70].spl);
    if (i != -1) {
        D_006333B8 = (f32)i / (f32)(s32)(SplTab[70].spl->len - 1U);
    } else {
        D_006333B8 = 0.0f;
    }
    PointAlongSpline(SplTab[70].spl, D_006333B8, &D_006FFA90, 0, 0);
    D_006FFA90 = D_006FFA80;
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

void AddBugLight(void) {
    if (D_006333B0 > 0.0f) {
        if (FRAME == FRAMES - 1) {
            NuLightAddSpotXSpanFade(&D_006FFA80, D_006333A8, D_006333AC,
                                    0xFF000000, 1,
                                    128 - (s32)(D_006333B0 * (128.0f / D_00632CD4)),
                                    1);
        }
    }
}

void DrawBug(void) {
    if (Level == 0x1b && CRemap[174] != -1) {
        Draw3DCharacter(&D_006FFA80, D_006333B4, (D_006333B6 - 0x8000) & 0xffff,
                        0, &CModel[CRemap[174]], 1.25f, BugAnim.action,
                        BugAnim.anim_time, 0);
    }
}

void UpdateBugLight(struct creature_s *c) {
    struct nuvec_s oldpos;
    struct nuvec_s v;
    struct nuvec_s v0;
    struct nuvec_s v1;
    struct nuvec_s pos;
    f32 f;
    f32 r;
    f32 r0;
    f32 r1;
    f32 d;
    f32 d0;
    f32 d1;
    f32 dr;
    f32 qr;
    f32 target;
    u16 a;

    D_006333B0 = 0.0f;
    if ((LBIT & 0x88100000) == 0) {
        return;
    }
    if (((Level == 0x14) && (GemPath != 0)) ||
        ((InBugArea((s32)c->obj.RPos.iRAIL, (s32)c->obj.RPos.iALONG,
                    c->obj.RPos.fALONG) != -1) &&
         (((LBIT & 0x80100000) == 0) || (VEHICLECONTROL == 1)))) {
        f = BUGFADETIME;
    } else {
        f = 0.0f;
    }
    D_006333A6 += qrand() / 0x40 * 6;
    if ((LBIT & 0x80100000) != 0) {
        if (D_00633398 > f) {
            D_00633398 -= D_0062E914;
            if (D_00633398 < f) {
                D_00633398 = f;
            }
        } else if (D_00633398 < f) {
            D_00633398 += D_0062E918;
            if (D_00633398 > f) {
                D_00633398 = f;
            }
        }
        D_006333A8 = D_00632CE4 + NuTrigTable[D_006333A6] * D_00632CE4 * D_0062E91C;
        if (D_00633398 > 0.0f) {
            D_006FFA80.x = player->obj.pos.x;
            D_006FFA80.y = player->obj.top * player->obj.SCALE + player->obj.pos.y;
            D_006FFA80.z = player->obj.pos.z;
            a = NuAtan2D(D_006FFA80.x - GameCam.x, D_006FFA80.z - GameCam.z);
            D_006FFA80.x = NuTrigTable[a] * D_00632CD8 + D_006FFA80.x;
            D_006333AC = 1.5f;
            D_006FFA80.z = NuTrigTable[(u16)(a + 0x4000)] * D_00632CD8 + D_006FFA80.z;
        }
        D_006333B0 = D_00633398;
        qr = (f32)qrand();
        target = D_00632CDC + (qr * D_0062E920 - 0.5f) * 0.25f;
        D_006333A8 = D_006333A8 + (target - D_006333A8) * D_0062E924;
        return;
    }
    if (Level != 0x1b) {
        return;
    }
    oldpos = D_006FFA80;
    if (SplTab[70].spl != 0) {
        pos = player->obj.pos;
        if (best_cRPos != 0) {
            r = D_00632CE0;
            if ((best_cRPos->mode & 0xc) != 0) {
                r = D_00632CE0 * D_0062E928;
            }
            pos.x = NuTrigTable[best_cRPos->angle] * r + pos.x;
            pos.z = NuTrigTable[(u16)(best_cRPos->angle + 0x4000)] * r + pos.z;
        }
        d0 = D_006333B8;
        PointAlongSpline(SplTab[70].spl, d0, &v, 0, 0);
        r = NuVecDistSqr(&pos, &v, 0);
        dr = D_0062E92C;
        if (c->sprint != 0) {
            dr = D_0062E930;
        }
        r0 = D_006333B8 - dr;
        if (r0 < 0.0f) {
            r0 = 0.0f;
        }
        PointAlongSpline(SplTab[70].spl, r0, &v0, 0, 0);
        d = NuVecDistSqr(&pos, &v0, 0);
        r1 = D_006333B8 + dr;
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
        if (D_006333B8 > d0) {
            D_006333B8 = D_006333B8 - dr * D_0062E934 * 10.0f;
            if (D_006333B8 < d0) {
                D_006333B8 = d0;
            }
        } else if (D_006333B8 < d0) {
            D_006333B8 = D_006333B8 + dr * D_0062E938 * 10.0f;
            if (D_006333B8 > d0) {
                D_006333B8 = d0;
            }
        }
        PointAlongSpline(SplTab[70].spl, D_006333B8, &pos, 0, 0);
        D_006FFA90.x = (pos.x - D_006FFA90.x) * D_0062E93C + D_006FFA90.x;
        D_006FFA90.y = (pos.y - D_006FFA90.y) * D_0062E93C + D_006FFA90.y;
        D_006FFA90.z = (pos.z - D_006FFA90.z) * D_0062E93C + D_006FFA90.z;
        D_006FFA80 = D_006FFA90;
    } else {
        D_006FFA80.x = player->obj.pos.x;
        D_006FFA80.y = player->obj.top * player->obj.SCALE + player->obj.pos.y;
        D_006FFA80.z = player->obj.pos.z;
        a = NuAtan2D(D_006FFA80.x - GameCam.x, D_006FFA80.z - GameCam.z);
        D_006FFA80.x = NuTrigTable[a] * D_00632CE0 + D_006FFA80.x;
        D_006FFA80.z = NuTrigTable[(u16)(a + 0x4000)] * D_00632CE0 + D_006FFA80.z;
    }
    D_006333A0 += 0x276;
    D_006333A2 += 0x162;
    D_006333A4 += 0x414;
    D_006333A8 = D_00632CE4 + NuTrigTable[D_006333A6] * D_00632CE4 * D_0062E940;
    D_006FFA80.x = NuTrigTable[D_006333A0] * 0.25f + D_006FFA80.x;
    D_006FFA80.y = NuTrigTable[D_006333A2] * 0.25f + D_006FFA80.y;
    D_006FFA80.z = NuTrigTable[D_006333A4] * 0.25f + D_006FFA80.z;
    NuVecSub(&v, &D_006FFA80, &oldpos);
    a = NuAtan2D(v.y, NuFsqrt(v.x * v.x + v.z * v.z));
    D_006333B4 = SeekRot(D_006333B4, a, 3);
    a = NuAtan2D(v.x, v.z);
    D_006333B6 = SeekRot(D_006333B6, a, 3);
    if (CRemap[174] != -1) {
        BugAnim.oldaction = BugAnim.action;
        UpdateAnimPacket(&CModel[CRemap[174]], &BugAnim, D_0062E944, 0.0f);
    }
    if (D_0063339C > f) {
        D_0063339C -= D_0062E948;
        if (D_0063339C < f) {
            D_0063339C = f;
        }
    } else if (D_0063339C < f) {
        D_0063339C += D_0062E94C;
        if (D_0063339C > f) {
            D_0063339C = f;
        }
    }
    if (D_0063339C > 0.0f) {
        D_006333AC = 1.0f;
    }
    D_006333B0 = D_0063339C;
    if (0x7fff < qrand()) {
        return;
    }
    AddGameDebris(0x9c, &D_006FFA80);
}
