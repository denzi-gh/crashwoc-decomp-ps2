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
