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
