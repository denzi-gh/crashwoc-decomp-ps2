/*
 * Unit: game/edlight
 *
 * Functions:
 *   0x00240d28 InsideLight
 *   0x002411a8 edlightDrawInfo
 *   0x00241658 edlightDrawLightAxis
 *   0x002418a0 edlightHighlight
 *   0x002419d8 edlightDrawBox
 *   0x00241d00 edlightDrawSelection
 *   0x00242190 edlightDrawDirection
 *   0x002422f0 edlightDrawRadius
 *   0x00242518 edlightAdd
 *   0x00242668 edlightDelete
 *   0x00242840 cbSelGlbType
 *   0x00242960 edlightInit
 *   0x00242da0 edlightClosestLight
 *   0x00242ee0 edlightDoInput
 *   0x00243988 edlightProc
 *   0x00243b48 edlightRender
 *   0x00243c78 edlightHighlightDirection
 *   0x00243d68 edlightDrawLights
 *   0x00243df0 edlightcb_r_sliderval
 *   0x00243ea0 edlightcb_g_sliderval
 *   0x00243f50 edlightcb_b_sliderval
 *   0x00244000 edlightcbSaveLightData
 *   0x00244038 edlightCbCancelOptMenu
 *   0x00244040 edlightClose
 *   0x00244060 edlightEnter
 *   0x00244108 edlightMakeNUCOLOUR3
 *   0x00244158 edlightWarning
 *   0x002441a8 edlightDrawLightBlock
 *   0x00244220 cbDoubleMenu
 *   0x00244370 cbGlbMenu
 *   0x002444c0 cbSelDoubleType
 *   0x00244558 cbCancelDoubleMenu
 *   0x00244580 cbCancelGlbMenu
 */

#include "creature.h"

struct numtl_s;

/* Editor light record, stride 0x48. */
struct edlight_s {
    s32 type;              /* 0x00 */
    struct nuvec_s pos;    /* 0x04 */
    struct nuvec_s wpos;   /* 0x10 */
    s32 field_1C;          /* 0x1C */
    u8 r;                  /* 0x20 */
    u8 g;                  /* 0x21 */
    u8 b;                  /* 0x22 */
    u8 pad23;              /* 0x23 */
    struct nucolour3_s col;/* 0x24 */
    s32 field_30;          /* 0x30 */
    s32 field_34;          /* 0x34 */
    s32 field_38;          /* 0x38 */
    u8 field_3C;           /* 0x3C */
    u8 field_3D;           /* 0x3D */
    u8 dbltype;            /* 0x3E */
    u8 pad3F[0x9];         /* 0x3F */
};                         /* 0x48 */

/* Editor slider control; value at 0x4C. */
struct slider_s {
    u8 pad00[0x4C];
    f32 val;               /* 0x4C */
};

/* Editor menu control. */
struct edmenu_s {
    u8 pad00[0x24];
    void (*cb)(struct edmenu_s *menu, s32 arg);  /* 0x24 */
    u8 pad28[0x8];
    s32 arg;                                      /* 0x30 */
};

/* Editor selection item. */
struct edsel_s {
    u8 pad00[0xC];
    s32 newtype;           /* 0x0C */
};

extern s32 edlight_used;
extern void *edlight_options_menu;
extern void *edlight_double_menu;
extern void *edlight_glb_menu;

extern s32 SWIDTH;
extern s32 SHEIGHT;
extern s32 LIGHTCOUNT;
extern s32 PLAYERCOUNT;
extern s32 AMBIENTCOUNT;
extern s32 DIRECTCOUNT;
extern s32 POINTCOUNT;
extern s32 NEGATIVECOUNT;
extern s32 D_0063268C;

extern struct edlight_s Lights[];
extern struct nuvec_s v000;
extern struct nuvec_s edlight_cam_pos;
extern struct numtl_s *mtl;
extern void *edlight_active_menu;

extern s32 D_00633374;
extern s32 D_00633378;
extern s32 edlight_glb_dir_light;
extern s32 edlight_glb_amb_light;
extern s32 glb_dir_error_flag;
extern s32 glb_amb_error_flag;
extern struct slider_s *edlight_r_slider;
extern struct slider_s *edlight_g_slider;
extern struct slider_s *edlight_b_slider;

extern char D_00625160[];
extern f32 D_0062E6A0;
extern f32 D_0062E688;
extern f32 D_0062E68C;
extern f32 D_0062E690;
extern f32 D_0062E694;
extern f32 D_0062E698;
extern f32 D_0062E69C;
extern u8 D_00632690;
extern u8 D_00632691;
extern u8 D_00632692;

extern void eduiMenuDetach(struct edmenu_s *menu);

extern void eduiMenuDestroy(void *menu);
extern void eduiCreateMessageMenu(void *menu, char *msg, s32 flag);
extern void SaveLights(void);
extern void NuFntSetPen(u32 colour);
extern void NuFntPrintEx(s32 x, s32 y, s32 colour, char *fmt, ...);
extern void NuRndrRect2di(s32 x, s32 y, s32 w, s32 h, s32 colour,
                          struct numtl_s *mtl);
extern void edlightDrawLightAxis(struct edlight_s *l);
extern void edlightDrawRadius(struct nuvec_s *pos, s32 r, s32 g, s32 b, f32 scale);
extern void edlightDrawSelection(void);
extern void edlightDrawBox(void);
extern void edlightDrawInfo(void);
extern void edcamSet(void);
extern void DrawCameraTarget(struct nuvec_s *pos);
extern void NuFntScale(s32 x, s32 y);
extern void eduiMenuRender(void *menu);
extern void NuVecSub(struct nuvec_s *out, struct nuvec_s *a, struct nuvec_s *b);
extern void edcamSetPosAng(struct nuvec_s *pos, s32 a, s32 b);
extern void ResetCheckpoint(s32 a, s32 b, s32 c, f32 d);
extern void ResetTimeTrial(void);
extern void ResetBonus(void);
extern void ResetDeath(void);
extern void ResetGemPath(void);
extern void RestoreCrateTypeData(void);
extern void ResetCrates(void);
extern void ResetWumpa(void);
extern void ResetAI(void);

void edlightAdd(void)
{
    struct edlight_s *l;
    s32 type = D_0063268C;

    if (type == 0)
        AMBIENTCOUNT++;
    else if (type == 1)
        DIRECTCOUNT++;
    else if (type == 2)
        POINTCOUNT++;
    else if (type == 3)
        NEGATIVECOUNT++;
    l = &Lights[LIGHTCOUNT];
    l->type = D_0063268C;
    l->pos = edlight_cam_pos;
    l->wpos = edlight_cam_pos;
    l->field_1C = 0;
    l->r = D_00632690;
    l->g = D_00632691;
    l->b = D_00632692;
    l->col.r = 1.0f;
    l->col.g = 1.0f;
    l->col.b = 1.0f;
    l->field_30 = 0;
    l->field_34 = 0;
    l->field_38 = 0;
    l->dbltype = 7;
    l->field_3C = 5;
    l->field_3D = 8;
    LIGHTCOUNT++;
}

void edlightRender(void)
{
    s32 i;

    edcamSet();
    DrawCameraTarget(&edlight_cam_pos);
    edlightDrawSelection();
    edlightDrawBox();
    edlightDrawInfo();
    NuRndrRect2di(0x400, 0xB80, SWIDTH / 16 * 16, SHEIGHT / 16 * 8,
                  (D_00632692 << 16) | (D_00632691 << 8) | D_00632690, mtl);
    for (i = 0; i < LIGHTCOUNT; i++) {
        edlightDrawLightAxis(&Lights[i]);
        edlightDrawRadius(&Lights[i].pos, Lights[i].r, Lights[i].g, Lights[i].b,
                          D_0062E688);
    }
    if (edlight_active_menu) {
        NuFntScale(0xC, 0xC);
        eduiMenuRender(edlight_active_menu);
    }
}

void edlightHighlightDirection(void)
{
    s32 i;
    f32 mindist = 0.0f;
    f32 threshold;
    struct nuvec_s d;

    D_00633378 = -1;
    if (D_00633374 != -1)
        return;
    threshold = D_0062E68C;
    for (i = 0; i < LIGHTCOUNT; i++) {
        f32 dist2;

        NuVecSub(&d, &edlight_cam_pos, &Lights[i].wpos);
        dist2 = d.x * d.x + d.y * d.y + d.z * d.z;
        if (dist2 < threshold) {
            if (D_00633378 == -1 || dist2 < mindist) {
                D_00633378 = i;
                mindist = dist2;
            }
        }
    }
}

void edlightDrawLights(void)
{
    s32 i;

    for (i = 0; i < LIGHTCOUNT; i++) {
        edlightDrawLightAxis(&Lights[i]);
        edlightDrawRadius(&Lights[i].pos, Lights[i].r, Lights[i].g, Lights[i].b,
                          D_0062E690);
    }
}

void edlightcb_r_sliderval(void *menu, struct slider_s *slider)
{
    struct edlight_s *l;
    s32 idx = D_00633374;

    if (idx == -1)
        return;
    l = &Lights[idx];
    if (l->type == 3)
        return;
    edlight_r_slider = slider;
    {
        s32 v = (s32)slider->val;
        D_00632690 = v;
        l->r = v;
    }
    if (l) {
        l->col.r = l->r * D_0062E694;
        l->col.g = l->g * D_0062E694;
        l->col.b = l->b * D_0062E694;
    }
}

void edlightcb_g_sliderval(void *menu, struct slider_s *slider)
{
    struct edlight_s *l;
    s32 idx = D_00633374;

    if (idx == -1)
        return;
    l = &Lights[idx];
    if (l->type == 3)
        return;
    edlight_g_slider = slider;
    {
        s32 v = (s32)slider->val;
        D_00632691 = v;
        l->g = v;
    }
    if (l) {
        l->col.r = l->r * D_0062E698;
        l->col.g = l->g * D_0062E698;
        l->col.b = l->b * D_0062E698;
    }
}

void edlightcb_b_sliderval(void *menu, struct slider_s *slider)
{
    struct edlight_s *l;
    s32 idx = D_00633374;

    if (idx == -1)
        return;
    l = &Lights[idx];
    if (l->type == 3)
        return;
    edlight_b_slider = slider;
    {
        s32 v = (s32)slider->val;
        D_00632692 = v;
        l->b = v;
    }
    if (l) {
        l->col.r = l->r * D_0062E69C;
        l->col.g = l->g * D_0062E69C;
        l->col.b = l->b * D_0062E69C;
    }
}

void edlightcbSaveLightData(void *menu)
{
    SaveLights();
    eduiCreateMessageMenu(menu, D_00625160, 1);
}

void edlightCbCancelOptMenu(void)
{
    edlight_active_menu = 0;
}

void edlightClose(void)
{
    eduiMenuDestroy(edlight_options_menu);
}

void edlightEnter(void)
{
    ResetTimeTrial();
    ResetCheckpoint(-1, -1, 0, 0.0f);
    ResetBonus();
    ResetDeath();
    ResetGemPath();
    RestoreCrateTypeData();
    ResetCrates();
    ResetWumpa();
    ResetAI();
    if (!edlight_used) {
        struct nuvec_s *pos;

        if (PLAYERCOUNT != 0)
            pos = (struct nuvec_s *)((u8 *)player + 0x6C);
        else
            pos = &v000;
        edcamSetPosAng(pos, 0, 0);
        edlight_used = 1;
    }
}

void edlightMakeNUCOLOUR3(s32 r, s32 g, s32 b, struct nucolour3_s *col)
{
    col->r = (r & 0xFF) * D_0062E6A0;
    col->g = (g & 0xFF) * D_0062E6A0;
    col->b = (b & 0xFF) * D_0062E6A0;
}

void edlightWarning(char *text)
{
    NuFntSetPen(0xFF0000FF);
    NuFntPrintEx(1024, 256, 0, text);
    NuFntSetPen(0xFFFFFF);
}

void edlightDrawLightBlock(void)
{
    NuRndrRect2di(0x400, 0xB80, SWIDTH / 16 * 16, SHEIGHT / 16 * 8,
                  (D_00632692 << 16) | (D_00632691 << 8) | D_00632690, mtl);
}

void cbSelGlbType(struct edmenu_s *menu, struct edsel_s *sel)
{
    s32 idx = D_00633374;
    s32 arg = 0;
    void (*fn)(struct edmenu_s *menu, s32 arg);

    if (idx != -1) {
        struct edlight_s *l = &Lights[idx];
        s32 t = l->type;

        if ((u32)(t - 1) < 2) {
            if (sel->newtype != l->field_3C) {
                if (l->field_3C == 4) {
                    edlight_glb_dir_light = -1;
                    l->field_3C = 5;
                    glb_dir_error_flag = 0;
                } else if (sel->newtype == -1) {
                    l->field_3C = 4;
                    glb_dir_error_flag = 1;
                    edlight_glb_dir_light = idx;
                }
            }
        } else if (t == 0) {
            if (sel->newtype != l->field_3C) {
                if (l->field_3C == 4) {
                    edlight_glb_amb_light = -1;
                    l->field_3C = 5;
                    glb_amb_error_flag = 0;
                } else if (sel->newtype == -1) {
                    l->field_3C = 4;
                    glb_amb_error_flag = 1;
                    edlight_glb_amb_light = idx;
                }
            }
        }
    }
    if (menu->arg != 0) {
        arg = menu->arg;
        eduiMenuDetach(menu);
    }
    fn = menu->cb;
    if (fn)
        fn(menu, arg);
}

void cbSelDoubleType(struct edmenu_s *menu, struct edsel_s *sel)
{
    s32 idx = D_00633374;
    s32 arg = 0;
    void (*fn)(struct edmenu_s *menu, s32 arg);

    if (idx != -1) {
        struct edlight_s *l = &Lights[idx];
        if (sel->newtype != l->dbltype)
            l->dbltype = sel->newtype;
    }
    if (menu->arg != 0) {
        arg = menu->arg;
        eduiMenuDetach(menu);
    }
    fn = menu->cb;
    if (fn)
        fn(menu, arg);
}

void cbCancelDoubleMenu(void)
{
    eduiMenuDestroy(edlight_double_menu);
    edlight_double_menu = 0;
}

void cbCancelGlbMenu(void)
{
    eduiMenuDestroy(edlight_glb_menu);
    edlight_glb_menu = 0;
}
