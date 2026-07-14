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
