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

#include "nu_libc.h"

#include "numath/nuvec.h"
#include "nu3d/numtl.h"

#include "creature.h"

struct eduistyle_s {
    s32 pen[2];
    u32 colour[2];
};

/* Editor light record, stride 0x48. */
struct edlight_s {
    s32 type;              /* 0x00 */
    struct nuvec_s pos;    /* 0x04 */
    struct nuvec_s wpos;   /* 0x10 */
    f32 radius;            /* 0x1C */
    u8 r;                  /* 0x20 */
    u8 g;                  /* 0x21 */
    u8 b;                  /* 0x22 */
    u8 pad23;              /* 0x23 */
    struct nucolour3_s col;/* 0x24 */
    struct nuvec_s dir;    /* 0x30 */
    u8 glbtype;            /* 0x3C */
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
    u8 pad00[0x10];
    s32 x;                 /* 0x10 */
    s32 y;                 /* 0x14 */
    u8 pad18[0xC];
    void (*cb)(struct edmenu_s *menu, s32 arg);  /* 0x24 */
    u8 pad28[0x8];
    s32 arg;                                      /* 0x30 */
};

struct edpad_s {
    u8 pad000[0x55C];
    u32 held;              /* 0x55C */
    u8 pad560[0x4];
    u32 buttons;           /* 0x564 */
};

/* Editor selection item. */
struct edsel_s {
    u8 pad00[0xC];
    s32 newtype;           /* 0x0C */
};

extern s32 edlight_used;
extern s32 screendump;
extern struct edmenu_s *edlight_options_menu;
extern struct edmenu_s *edlight_double_menu;
extern struct edmenu_s *edlight_glb_menu;
extern struct eduistyle_s D_00625170;
extern void *app_fnt;
extern struct nuvec_s last_radius;

struct ldata_s {
    char *filepath;
};

extern struct ldata_s *LDATA;
extern s32 PANELOFF;
extern char tbuf[];

extern s32 SWIDTH;
extern s32 SHEIGHT;
extern s32 LIGHTCOUNT;
extern s32 PLAYERCOUNT;
extern s32 AMBIENTCOUNT;
extern s32 DIRECTCOUNT;
extern s32 POINTCOUNT;
extern s32 NEGATIVECOUNT;
extern s32 D_0063268C;
extern f32 default_radius;
extern f32 topx, topy, topz;
extern f32 bottomx, bottomy, bottomz;
extern f32 northx, northy, northz;
extern f32 eastx, easty, eastz;
extern f32 southx, southy, southz;
extern f32 westx, westy, westz;

extern struct edlight_s Lights[];
extern struct nuvec_s v000;
extern struct nuvec_s edlight_cam_pos;
extern struct numtl_s *mtl;
extern void *edlight_active_menu;

extern s32 D_00633364;
extern s32 D_00633368;
extern s32 D_0063336C;
extern s32 D_00633370;
extern s32 D_00633374;
extern s32 D_00633378;
extern s32 D_0063337C;
extern s32 edlight_glb_dir_light;
extern s32 edlight_glb_amb_light;
extern s32 glb_dir_error_flag;
extern s32 glb_amb_error_flag;
extern struct slider_s *edlight_r_slider;
extern struct slider_s *edlight_g_slider;
extern struct slider_s *edlight_b_slider;

#define edlight_default_type D_0063268C
#define edlight_menu_style D_00625170
#define edlight_str_options D_006326E8
#define edlight_str_save D_006251A8
#define edlight_str_red D_006326F0
#define edlight_str_green D_006326F8
#define edlight_str_blue D_00632700
#define edlight_str_glbmenu D_006251B8
#define edlight_str_powermenu D_006251C8
#define edlight_str_brightness D_00625180
#define edlight_str_double D_006326C8
#define edlight_str_single D_006326D0
#define edlight_str_glblocal D_00625190
#define edlight_str_global D_006326D8
#define edlight_str_local D_006326E0
#define edlight_cam_ax D_00633364
#define edlight_cam_ay D_00633368
#define edlight_neg_r D_00632693
#define edlight_neg_g D_00632694[0]
#define edlight_neg_b D_00632694[1]
#define edlight_cur_light D_00633374
#define edlight_cur_dir D_00633378
#define edlight_last_light D_0063337C
#define edlight_default_r D_00632690
#define edlight_default_g D_00632691
#define edlight_default_b D_00632692
#define edlight_add_error D_0063336C
#define edlight_inside_error D_00633370
#define edlight_dir_r D_006326B8
#define edlight_dir_g D_006326BC
#define edlight_dir_b D_006326C0

extern char D_006326B0[];
extern s32 D_006326B8;
extern s32 D_006326BC;
extern s32 D_006326C0;
extern char D_00624F30[];
extern char D_00624F48[];
extern char D_00624F60[];
extern char D_00624F78[];
extern char D_00624FA0[];
extern char D_00624FC0[];
extern char D_00624FD8[];
extern char D_00624FF8[];
extern char D_00625010[];
extern char D_00625028[];
extern char D_00625048[];
extern char D_00625070[];
extern char D_00625090[];
extern char D_006250B0[];
extern char D_006250C8[];
extern char D_006250D8[];
extern char D_006250E8[];
extern char D_00625100[];
extern char D_00625120[];
extern char D_00625140[];
extern char D_00625160[];
extern char D_00625180[];
extern char D_00625190[];
extern char D_006251A8[];
extern char D_006251B8[];
extern char D_006251C8[];
extern char D_006326C8[];
extern char D_006326D0[];
extern char D_006326D8[];
extern char D_006326E0[];
extern char D_006326E8[];
extern char D_006326F0[];
extern char D_006326F8[];
extern char D_00632700[];
extern f32 D_0062E688;
extern f32 D_0062E690;
extern u8 D_00632690;
extern u8 D_00632691;
extern u8 D_00632692;
extern u8 D_00632693;
extern u8 D_00632694[2];
extern struct nuvec_s edlight_old_cam_pos;
extern s32 radius_has_changed;

extern void eduiMenuDetach(struct edmenu_s *menu);
extern struct edmenu_s *eduiMenuCreate(s32 x, s32 y, s32 w, s32 h, void *font,
                                       void *cancelcb, char *title);
extern void eduiMenuAttach(struct edmenu_s *menu, struct edmenu_s *child);
extern void *eduiMenuAddItem(struct edmenu_s *menu, void *item);
extern void *eduiItemSelCreate(s32 value, struct eduistyle_s *style, s32 on, s32 group,
                               void *cb, char *text);
extern void *eduiItemCheckCreate(s32 value, struct eduistyle_s *style, s32 checked,
                                 s32 group, void *cb, char *text);
extern void *eduiItemColourSliderCreate(s32 value, struct eduistyle_s *style, s32 on,
                                        void *cb, f32 min, f32 max, f32 val, s32 r,
                                        s32 g, s32 b, char *text);
extern s32 eduiMenuProcess(void *menu, struct edpad_s *pad);
extern void edlightDoInput(struct edpad_s *pad);
extern void edcamGetPosAng(struct nuvec_s *pos, s32 *ax, s32 *ay);
extern void edcamSetPos(struct nuvec_s *pos);
extern void edcamMove(struct edpad_s *pad);
extern void eduiItemSliderSetVal(struct slider_s *item, f32 val);
extern void edlightcb_r_sliderval(void *menu, struct slider_s *slider);
extern void edlightcb_g_sliderval(void *menu, struct slider_s *slider);
extern void edlightcb_b_sliderval(void *menu, struct slider_s *slider);
extern void edlightcbSaveLightData(void *menu);
extern void edlightCbCancelOptMenu(void);
extern void cbSelGlbType(struct edmenu_s *menu, struct edsel_s *sel);
extern void cbSelDoubleType(struct edmenu_s *menu, struct edsel_s *sel);
extern void cbCancelDoubleMenu(void);
extern void cbCancelGlbMenu(void);
extern void cbGlbMenu(struct edmenu_s *menu, struct edsel_s *sel);
extern void cbDoubleMenu(struct edmenu_s *menu, struct edsel_s *sel);

extern void eduiMenuDestroy(struct edmenu_s *menu);
extern void eduiCreateMessageMenu(void *menu, char *msg, s32 flag);
extern void SaveLights(void);
extern char *strupr(char *s);
extern void NuFntSetPen(s32 colour);
extern void NuFntPrintEx(s32 x, s32 y, s32 colour, char *fmt, ...);
extern void NuRndrRect2di(s32 x, s32 y, s32 w, s32 h, s32 colour,
                          struct numtl_s *mtl);
extern void edlightDrawSelection(void);
extern void edlightDrawBox(void);
extern void edlightDrawRadius(struct nuvec_s *pos, u8 r, u8 g, u8 b, f32 radius);
extern void edcamSet(void);
extern void DrawCameraTarget(struct nuvec_s *pos);
extern void NuFntScale(s32 x, s32 y);
extern void eduiMenuRender(void *menu);
extern void NuRndrLine3dDbg(s32 colour, f32 x0, f32 y0, f32 z0, f32 x1, f32 y1,
                            f32 z1);
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

inline void edlightMakeNUCOLOUR3(s32 r, s32 g, s32 b, struct nucolour3_s *col)
{
    col->r = (r & 0xFF) * (1.0f / 255.0f);
    col->g = (g & 0xFF) * (1.0f / 255.0f);
    col->b = (b & 0xFF) * (1.0f / 255.0f);
}

s32 InsideLight(struct nuvec_s *pos, s32 idx)
{
    s32 i = 0;
    s32 inside = 0;
    f32 dist;

    if (idx != -1) {
        if (Lights[idx].type == 0) {
            for (i = 0; i < LIGHTCOUNT; i++) {
                if (i != edlight_cur_light && i != edlight_cur_dir && i != idx &&
                    Lights[i].glbtype != 4 && Lights[i].type == 0) {
                    dist = fabs(NuVecDist(&Lights[idx].pos, pos, 0));
                    if ((f32)fabs(NuVecDist(&Lights[i].pos, &Lights[idx].pos, 0)) -
                            Lights[i].radius < dist)
                        inside = 1;
                }
            }
        } else if (Lights[idx].type == 3) {
            for (i = 0; i < LIGHTCOUNT; i++) {
                if (i != edlight_cur_light && i != edlight_cur_dir && i != idx &&
                    Lights[i].glbtype != 4 && Lights[i].type == 3) {
                    dist = fabs(NuVecDist(&Lights[idx].pos, pos, 0));
                    if ((f32)fabs(NuVecDist(&Lights[i].pos, &Lights[idx].pos, 0)) -
                            Lights[i].radius < dist)
                        inside = 1;
                }
            }
        }
    } else if (edlight_default_type == 0) {
        for (i = 0; i < LIGHTCOUNT; i++) {
            if (Lights[i].glbtype != 4 && Lights[i].type == 0) {
                if ((f32)fabs(NuVecDist(&Lights[i].pos, pos, 0)) <
                        Lights[i].radius + default_radius)
                    inside = 1;
            }
        }
    } else if (edlight_default_type == 3) {
        for (i = 0; i < LIGHTCOUNT; i++) {
            if (Lights[i].glbtype != 4 && Lights[i].type == 3) {
                if ((f32)fabs(NuVecDist(&Lights[i].pos, pos, 0)) <
                        Lights[i].radius + default_radius)
                    inside = 1;
            }
        }
    }
    return inside;
}

void edlightDrawInfo(void)
{
    if (PANELOFF)
        return;
    NuFntScale(8, 0xC);
    NuFntSetPen(-1);
    strcpy(tbuf, LDATA->filepath);
    strupr(tbuf);
    NuFntPrintEx(0x400, 0x100, 0, D_006326B0, tbuf);
    NuFntPrintEx(0x400, 0x200, 0, D_00624F30, LIGHTCOUNT, 0x100);
    if (edlight_cur_light != -1)
        NuFntPrintEx(0x400, 0x300, 0, D_00624F48, edlight_cur_light);
    else
        NuFntPrintEx(0x400, 0x300, 0, D_00624F60);
    NuFntPrintEx(0x400, 0xD00, 0, D_00624F78, edlight_cam_pos.x, edlight_cam_pos.y,
                 edlight_cam_pos.z);
    if (edlight_cur_light == -1)
        NuFntPrintEx(0x400, 0x400, 0, D_00624FA0, 0, 0, 0);
    else
        NuFntPrintEx(0x400, 0x400, 0, D_00624FA0, Lights[edlight_cur_light].r,
                     Lights[edlight_cur_light].g, Lights[edlight_cur_light].b);
    if (edlight_cur_light == -1) {
        if (edlight_default_type == 0)
            NuFntPrintEx(0x400, 0x500, 0, D_00624FC0);
        else if (edlight_default_type == 1)
            NuFntPrintEx(0x400, 0x500, 0, D_00624FD8);
        else if (edlight_default_type == 2)
            NuFntPrintEx(0x400, 0x500, 0, D_00624FF8);
        else if (edlight_default_type == 3)
            NuFntPrintEx(0x400, 0x500, 0, D_00625010);
        return;
    }
    if (Lights[edlight_cur_light].type == 0)
        NuFntPrintEx(0x400, 0x500, 0, D_00625028);
    else if (Lights[edlight_cur_light].type == 1)
        NuFntPrintEx(0x400, 0x500, 0, D_00625048);
    else if (Lights[edlight_cur_light].type == 2)
        NuFntPrintEx(0x400, 0x500, 0, D_00625070);
    else if (Lights[edlight_cur_light].type == 3)
        NuFntPrintEx(0x400, 0x500, 0, D_00625090);
    NuFntPrintEx(0x400, 0x600, 0, D_006250B0, Lights[edlight_cur_light].radius);
    if (Lights[edlight_cur_light].glbtype == 4)
        NuFntPrintEx(0x400, 0x700, 0, D_006250C8);
    else
        NuFntPrintEx(0x400, 0x700, 0, D_006250D8);
    if (edlight_glb_amb_light != -1 && glb_amb_error_flag &&
        edlight_cur_light != edlight_glb_amb_light && Lights[edlight_cur_light].type == 0) {
        NuFntSetPen(0xFF0000FF);
        NuFntPrintEx(0x400, 0x900, 0, D_006250E8);
    }
    if (edlight_glb_dir_light != -1 && glb_dir_error_flag &&
        edlight_cur_light != edlight_glb_dir_light &&
        (Lights[edlight_cur_light].type == 1 || Lights[edlight_cur_light].type == 2)) {
        NuFntSetPen(0xFF0000FF);
        NuFntPrintEx(0x400, 0x900, 0, D_00625100);
    }
    NuFntSetPen(-1);
    if (Lights[edlight_cur_light].dbltype == 6 && Lights[edlight_cur_light].type != 3)
        NuFntPrintEx(0x400, 0x800, 0, D_00625120);
    else if (Lights[edlight_cur_light].dbltype == 7 && Lights[edlight_cur_light].type != 3)
        NuFntPrintEx(0x400, 0x800, 0, D_00625140);
}

void edlightDrawLightAxis(struct edlight_s *l)
{
    struct nuvec_s pt[9];
    struct nuvec_s centre;
    f32 len = 0.3f;
    f32 head = 0.075f;
    s32 i;

    centre = l->pos;
    for (i = 0; i < 9; i++)
        pt[i] = l->pos;
    pt[0].x += len;
    pt[1].y += len;
    pt[2].z += len;
    pt[3].x += len - head;
    pt[3].z += head;
    pt[4].x += len - head;
    pt[4].z -= head;
    pt[5].y += len - head;
    pt[5].x += head;
    pt[6].y += len - head;
    pt[6].x -= head;
    pt[7].z += len - head;
    pt[7].x += head;
    pt[8].z += len - head;
    pt[8].x -= head;
    NuRndrLine3dDbg(0xFF, centre.x, centre.y, centre.z, pt[0].x, pt[0].y, pt[0].z);
    NuRndrLine3dDbg(0xFF, pt[0].x, pt[0].y, pt[0].z, pt[3].x, pt[3].y, pt[3].z);
    NuRndrLine3dDbg(0xFF, pt[0].x, pt[0].y, pt[0].z, pt[4].x, pt[4].y, pt[4].z);
    NuRndrLine3dDbg(0xFF00, centre.x, centre.y, centre.z, pt[1].x, pt[1].y, pt[1].z);
    NuRndrLine3dDbg(0xFF00, pt[1].x, pt[1].y, pt[1].z, pt[5].x, pt[5].y, pt[5].z);
    NuRndrLine3dDbg(0xFF00, pt[1].x, pt[1].y, pt[1].z, pt[6].x, pt[6].y, pt[6].z);
    NuRndrLine3dDbg(0xFF0000, centre.x, centre.y, centre.z, pt[2].x, pt[2].y, pt[2].z);
    NuRndrLine3dDbg(0xFF0000, pt[2].x, pt[2].y, pt[2].z, pt[7].x, pt[7].y, pt[7].z);
    NuRndrLine3dDbg(0xFF0000, pt[2].x, pt[2].y, pt[2].z, pt[8].x, pt[8].y, pt[8].z);
}

void edlightHighlight(void)
{
    s32 i;
    f32 mindist = 0.0f;
    struct nuvec_s d;

    edlight_cur_light = -1;
    if (edlight_cur_dir == -1) {
        for (i = 0; i < LIGHTCOUNT; i++) {
            f32 dist2;

            NuVecSub(&d, &edlight_cam_pos, &Lights[i].pos);
            dist2 = d.x * d.x + d.y * d.y + d.z * d.z;
            if (dist2 < 0.0225f) {
                if (edlight_cur_light == -1 || dist2 < mindist) {
                    edlight_cur_light = i;
                    mindist = dist2;
                }
            }
        }
    }
    if (edlight_cur_light != -1 && LIGHTCOUNT > 0) {
        edlight_last_light = edlight_cur_light;
        edlight_default_r = Lights[edlight_cur_light].r;
        edlight_default_g = Lights[edlight_cur_light].g;
        edlight_default_b = Lights[edlight_cur_light].b;
    }
}

void edlightDrawBox(void)
{
    struct nuvec_s pt[8];
    s32 idx = edlight_cur_dir;
    f32 size = 0.0375f;
    s32 i;

    if (idx != -1) {
        for (i = 0; i < 8; i++)
            pt[i] = Lights[idx].wpos;
        pt[0].y += size;
        pt[0].x -= size;
        pt[0].z += size;
        pt[1].y += size;
        pt[1].x += size;
        pt[1].z += size;
        pt[2].y += size;
        pt[2].x += size;
        pt[2].z -= size;
        pt[3].y += size;
        pt[3].x -= size;
        pt[3].z -= size;
        pt[4].y -= size;
        pt[4].x -= size;
        pt[4].z += size;
        pt[5].y -= size;
        pt[5].x += size;
        pt[5].z += size;
        pt[6].y -= size;
        pt[6].x += size;
        pt[6].z -= size;
        pt[7].y -= size;
        pt[7].x -= size;
        pt[7].z -= size;
        NuRndrLine3dDbg(0xFF00, pt[0].x, pt[0].y, pt[0].z, pt[1].x, pt[1].y, pt[1].z);
        NuRndrLine3dDbg(0xFF00, pt[1].x, pt[1].y, pt[1].z, pt[2].x, pt[2].y, pt[2].z);
        NuRndrLine3dDbg(0xFF00, pt[2].x, pt[2].y, pt[2].z, pt[3].x, pt[3].y, pt[3].z);
        NuRndrLine3dDbg(0xFF00, pt[3].x, pt[3].y, pt[3].z, pt[0].x, pt[0].y, pt[0].z);
        NuRndrLine3dDbg(0xFF00, pt[4].x, pt[4].y, pt[4].z, pt[5].x, pt[5].y, pt[5].z);
        NuRndrLine3dDbg(0xFF00, pt[5].x, pt[5].y, pt[5].z, pt[6].x, pt[6].y, pt[6].z);
        NuRndrLine3dDbg(0xFF00, pt[6].x, pt[6].y, pt[6].z, pt[7].x, pt[7].y, pt[7].z);
        NuRndrLine3dDbg(0xFF00, pt[7].x, pt[7].y, pt[7].z, pt[4].x, pt[4].y, pt[4].z);
        NuRndrLine3dDbg(0xFF00, pt[4].x, pt[4].y, pt[4].z, pt[0].x, pt[0].y, pt[0].z);
        NuRndrLine3dDbg(0xFF00, pt[5].x, pt[5].y, pt[5].z, pt[1].x, pt[1].y, pt[1].z);
        NuRndrLine3dDbg(0xFF00, pt[6].x, pt[6].y, pt[6].z, pt[2].x, pt[2].y, pt[2].z);
        NuRndrLine3dDbg(0xFF00, pt[7].x, pt[7].y, pt[7].z, pt[3].x, pt[3].y, pt[3].z);
    }
}

void edlightDrawSelection(void)
{
    struct nuvec_s pt[6];
    s32 idx = edlight_cur_light;
    s32 i;

    if (idx != -1) {
        for (i = 0; i < 6; i++)
            pt[i] = Lights[idx].pos;
        pt[0].y += 0.15f;
        pt[1].y -= 0.15f;
        pt[2].z += 0.15f;
        pt[3].x += 0.15f;
        pt[4].z -= 0.15f;
        pt[5].x -= 0.15f;
        NuRndrLine3dDbg(0xFF00, pt[0].x, pt[0].y, pt[0].z, pt[2].x, pt[2].y, pt[2].z);
        NuRndrLine3dDbg(0xFF00, pt[0].x, pt[0].y, pt[0].z, pt[3].x, pt[3].y, pt[3].z);
        NuRndrLine3dDbg(0xFF00, pt[0].x, pt[0].y, pt[0].z, pt[4].x, pt[4].y, pt[4].z);
        NuRndrLine3dDbg(0xFF00, pt[0].x, pt[0].y, pt[0].z, pt[5].x, pt[5].y, pt[5].z);
        NuRndrLine3dDbg(0xFF00, pt[1].x, pt[1].y, pt[1].z, pt[2].x, pt[2].y, pt[2].z);
        NuRndrLine3dDbg(0xFF00, pt[1].x, pt[1].y, pt[1].z, pt[3].x, pt[3].y, pt[3].z);
        NuRndrLine3dDbg(0xFF00, pt[1].x, pt[1].y, pt[1].z, pt[4].x, pt[4].y, pt[4].z);
        NuRndrLine3dDbg(0xFF00, pt[1].x, pt[1].y, pt[1].z, pt[5].x, pt[5].y, pt[5].z);
        NuRndrLine3dDbg(0xFF00, pt[2].x, pt[2].y, pt[2].z, pt[3].x, pt[3].y, pt[3].z);
        NuRndrLine3dDbg(0xFF00, pt[3].x, pt[3].y, pt[3].z, pt[4].x, pt[4].y, pt[4].z);
        NuRndrLine3dDbg(0xFF00, pt[4].x, pt[4].y, pt[4].z, pt[5].x, pt[5].y, pt[5].z);
        NuRndrLine3dDbg(0xFF00, pt[5].x, pt[5].y, pt[5].z, pt[2].x, pt[2].y, pt[2].z);
    }
    if (edlight_add_error || edlight_inside_error) {
        for (i = 0; i < 6; i++)
            pt[i] = edlight_cam_pos;
        pt[0].x -= 0.15f;
        pt[0].y -= 0.15f;
        pt[1].x += 0.15f;
        pt[1].y += 0.15f;
        pt[2].z -= 0.15f;
        pt[2].y -= 0.15f;
        pt[3].z += 0.15f;
        pt[3].y += 0.15f;
        pt[4].x -= 0.15f;
        pt[4].z -= 0.15f;
        pt[5].x += 0.15f;
        pt[5].z += 0.15f;
        NuRndrLine3dDbg(0xFF, pt[0].x, pt[0].y, pt[0].z, pt[1].x, pt[1].y, pt[1].z);
        NuRndrLine3dDbg(0xFF, pt[2].x, pt[2].y, pt[2].z, pt[3].x, pt[3].y, pt[3].z);
        NuRndrLine3dDbg(0xFF, pt[4].x, pt[4].y, pt[4].z, pt[5].x, pt[5].y, pt[5].z);
        for (i = 0; i < 6; i++)
            pt[i] = edlight_cam_pos;
        pt[0].x += 0.15f;
        pt[0].y -= 0.15f;
        pt[1].x -= 0.15f;
        pt[1].y += 0.15f;
        pt[2].z += 0.15f;
        pt[2].y -= 0.15f;
        pt[3].z -= 0.15f;
        pt[3].y += 0.15f;
        pt[4].x += 0.15f;
        pt[4].z -= 0.15f;
        pt[5].x -= 0.15f;
        pt[5].z += 0.15f;
        NuRndrLine3dDbg(0xFF, pt[0].x, pt[0].y, pt[0].z, pt[1].x, pt[1].y, pt[1].z);
        NuRndrLine3dDbg(0xFF, pt[2].x, pt[2].y, pt[2].z, pt[3].x, pt[3].y, pt[3].z);
        NuRndrLine3dDbg(0xFF, pt[4].x, pt[4].y, pt[4].z, pt[5].x, pt[5].y, pt[5].z);
    }
}

void edlightDrawDirection(void)
{
    s32 i;

    for (i = 0; i < LIGHTCOUNT; i++) {
        struct nuvec_s pos = Lights[i].pos;
        struct nuvec_s wpos = Lights[i].wpos;

        edlight_dir_r = Lights[i].r;
        edlight_dir_g = Lights[i].g;
        edlight_dir_b = Lights[i].b;
        NuRndrLine3dDbg(((edlight_dir_b & 0xFF) << 16) | ((edlight_dir_g & 0xFF) << 8) |
                            (edlight_dir_r & 0xFF),
                        pos.x, pos.y, pos.z, wpos.x, wpos.y, wpos.z);
        if (Lights[i].type == 2 || Lights[i].type == 0 || Lights[i].type == 3) {
            f32 radius = fabs(NuVecDist(&pos, &wpos, 0));

            edlightDrawRadius(&Lights[i].pos, Lights[i].r, Lights[i].g, Lights[i].b, radius);
        }
    }
}

void edlightDrawRadius(struct nuvec_s *pos, u8 r, u8 g, u8 b, f32 radius)
{
    s32 colour = (b << 16) | (g << 8) | r;

    topx = pos->x;
    topy = pos->y + radius;
    topz = pos->z;
    bottomx = pos->x;
    bottomy = pos->y - radius;
    bottomz = pos->z;
    northx = pos->x;
    northy = pos->y;
    northz = pos->z + radius;
    southx = pos->x;
    southy = pos->y;
    southz = pos->z - radius;
    eastx = pos->x + radius;
    easty = pos->y;
    eastz = pos->z;
    westx = pos->x - radius;
    westy = pos->y;
    westz = pos->z;
    NuRndrLine3dDbg(colour, northx, northy, northz, eastx, easty, eastz);
    NuRndrLine3dDbg(colour, eastx, easty, eastz, southx, southy, southz);
    NuRndrLine3dDbg(colour, southx, southy, southz, westx, westy, westz);
    NuRndrLine3dDbg(colour, westx, westy, westz, northx, northy, northz);
    NuRndrLine3dDbg(colour, topx, topy, topz, southx, southy, southz);
    NuRndrLine3dDbg(colour, southx, southy, southz, bottomx, bottomy, bottomz);
    NuRndrLine3dDbg(colour, bottomx, bottomy, bottomz, northx, northy, northz);
    NuRndrLine3dDbg(colour, northx, northy, northz, topx, topy, topz);
    NuRndrLine3dDbg(colour, topx, topy, topz, eastx, easty, eastz);
    NuRndrLine3dDbg(colour, eastx, easty, eastz, bottomx, bottomy, bottomz);
    NuRndrLine3dDbg(colour, bottomx, bottomy, bottomz, westx, westy, westz);
    NuRndrLine3dDbg(colour, westx, westy, westz, topx, topy, topz);
}

void edlightAdd(void)
{
    if (edlight_default_type == 0)
        AMBIENTCOUNT++;
    else if (edlight_default_type == 1)
        DIRECTCOUNT++;
    else if (edlight_default_type == 2)
        POINTCOUNT++;
    else if (edlight_default_type == 3)
        NEGATIVECOUNT++;
    Lights[LIGHTCOUNT].type = edlight_default_type;
    Lights[LIGHTCOUNT].pos = edlight_cam_pos;
    Lights[LIGHTCOUNT].wpos = edlight_cam_pos;
    Lights[LIGHTCOUNT].radius = 0.0f;
    Lights[LIGHTCOUNT].r = edlight_default_r;
    Lights[LIGHTCOUNT].g = edlight_default_g;
    Lights[LIGHTCOUNT].b = edlight_default_b;
    Lights[LIGHTCOUNT].col.r = 1.0f;
    Lights[LIGHTCOUNT].col.g = 1.0f;
    Lights[LIGHTCOUNT].col.b = 1.0f;
    Lights[LIGHTCOUNT].dir.x = 0.0f;
    Lights[LIGHTCOUNT].dir.y = 0.0f;
    Lights[LIGHTCOUNT].dir.z = 0.0f;
    Lights[LIGHTCOUNT].glbtype = 5;
    Lights[LIGHTCOUNT].field_3D = 8;
    Lights[LIGHTCOUNT].dbltype = 7;
    LIGHTCOUNT++;
}

void edlightDelete(void)
{
    s32 i;

    if (edlight_cur_light == -1)
        return;
    if (Lights[edlight_cur_light].type == 0) {
        if (Lights[edlight_cur_light].glbtype == 4) {
            edlight_glb_amb_light = -1;
            glb_amb_error_flag = 0;
        }
    } else if (Lights[edlight_cur_light].type == 1 || Lights[edlight_cur_light].type == 2) {
        if (Lights[edlight_cur_light].glbtype == 4) {
            edlight_glb_dir_light = -1;
            glb_dir_error_flag = 0;
        }
    }
    if (Lights[edlight_cur_light].type == 0)
        AMBIENTCOUNT--;
    else if (Lights[edlight_cur_light].type == 1)
        DIRECTCOUNT--;
    else if (Lights[edlight_cur_light].type == 2)
        POINTCOUNT--;
    else if (Lights[edlight_cur_light].type == 3)
        NEGATIVECOUNT--;
    LIGHTCOUNT--;
    for (i = edlight_cur_light; i <= LIGHTCOUNT; i++) {
        Lights[i].type = Lights[i + 1].type;
        Lights[i].pos = Lights[i + 1].pos;
        Lights[i].wpos = Lights[i + 1].wpos;
        Lights[i].r = Lights[i + 1].r;
        Lights[i].g = Lights[i + 1].g;
        Lights[i].b = Lights[i + 1].b;
        Lights[i].radius = Lights[i + 1].radius;
        Lights[i].col = Lights[i + 1].col;
        Lights[i].dir = Lights[i + 1].dir;
        Lights[i].glbtype = Lights[i + 1].glbtype;
        Lights[i].field_3D = Lights[i + 1].field_3D;
        Lights[i].dbltype = Lights[i + 1].dbltype;
    }
    edlight_cur_light = -1;
}

void edlightInit(void)
{
    struct eduistyle_s style = edlight_menu_style;
    f32 r;
    f32 g;
    f32 b;

    if (mtl == 0) {
        struct numtl_s *m = NuMtlCreate(1);
        struct numtlattrib_s attrib = m->attrib;

        attrib.zmode = 3;
        attrib.filter = 1;
        attrib.lighting = 2;
        attrib.utc = 1;
        attrib.vtc = 1;
        attrib.colour = 1;
        attrib.alpha = 0;
        mtl = m;
        m->attrib = attrib;
        NuMtlUpdate(m);
    }
    if (edlight_cur_light != -1) {
        r = Lights[edlight_cur_light].r;
        g = Lights[edlight_cur_light].g;
        b = Lights[edlight_cur_light].b;
    } else {
        r = edlight_default_r;
        g = edlight_default_g;
        b = edlight_default_b;
    }
    edlight_options_menu = eduiMenuCreate(0x46, 0x46, 0xB4, 0x12C, app_fnt,
                                          edlightCbCancelOptMenu, edlight_str_options);
    if (edlight_options_menu) {
        eduiMenuAddItem(edlight_options_menu,
                        eduiItemSelCreate(1, &style, 0, 0, edlightcbSaveLightData,
                                          edlight_str_save));
        if (edlight_cur_light != -1) {
            eduiMenuAddItem(edlight_options_menu,
                            eduiItemColourSliderCreate(0, &style, 0, edlightcb_r_sliderval,
                                                       0.0f, 255.0f, r, 0xFF, 0, 0,
                                                       edlight_str_red));
            eduiMenuAddItem(edlight_options_menu,
                            eduiItemColourSliderCreate(0, &style, 0, edlightcb_g_sliderval,
                                                       0.0f, 255.0f, g, 0, 0xFF, 0,
                                                       edlight_str_green));
            eduiMenuAddItem(edlight_options_menu,
                            eduiItemColourSliderCreate(0, &style, 0, edlightcb_b_sliderval,
                                                       0.0f, 255.0f, b, 0, 0, 0xFF,
                                                       edlight_str_blue));
            eduiMenuAddItem(edlight_options_menu,
                            eduiItemSelCreate(1, &style, 0, 0, cbGlbMenu, edlight_str_glbmenu));
            eduiMenuAddItem(edlight_options_menu,
                            eduiItemSelCreate(1, &style, 0, 0, cbDoubleMenu, edlight_str_powermenu));
        }
    }
    if (LIGHTCOUNT == 0) {
        edlight_default_r = 0xFF;
        edlight_default_g = 0xFF;
        edlight_default_b = 0xFF;
    }
    if (LIGHTCOUNT > 0) {
        default_radius = Lights[LIGHTCOUNT].radius;
        if (default_radius < 0.25)
            default_radius = 1.0f;
    } else {
        default_radius = 1.0f;
    }
    if (LIGHTCOUNT > 0) {
        last_radius = Lights[LIGHTCOUNT].wpos;
    } else {
        last_radius.x = 0.0f;
        last_radius.y = 0.0f;
        last_radius.z = 0.0f;
    }
}

s32 edlightClosestLight(void)
{
    struct nuvec_s cam = edlight_cam_pos;
    struct nuvec_s pos = Lights[0].pos;
    s32 closest = 0;
    s32 i;
    f32 mindist = NuVecDist(&cam, &pos, 0);

    for (i = 1; i < LIGHTCOUNT; i++) {
        f32 dist;

        pos = Lights[i].pos;
        dist = fabs(NuVecDist(&cam, &pos, 0));
        if (dist < mindist) {
            mindist = dist;
            closest = i;
        }
    }
    return closest;
}

void edlightDoInput(struct edpad_s *pad)
{
    struct nuvec_s step;
    struct nuvec_s probe;
    struct nuvec_s offset;
    s32 sel = -1;
    s32 i;

    edcamGetPosAng(&edlight_cam_pos, &edlight_cam_ax, &edlight_cam_ay);
    edlight_old_cam_pos = edlight_cam_pos;
    if (edlight_cur_light == -1 && edlight_cur_dir == -1) {
        if (InsideLight(&edlight_cam_pos, -1))
            edlight_inside_error = 1;
        else
            edlight_inside_error = 0;
    } else if (edlight_cur_light != -1) {
        if (InsideLight(&edlight_cam_pos, edlight_cur_light))
            edlight_inside_error = 1;
        else
            edlight_inside_error = 0;
    } else {
        if (InsideLight(&edlight_cam_pos, edlight_cur_dir))
            edlight_inside_error = 1;
        else
            edlight_inside_error = 0;
    }

    if (pad->held & 0x100) {
        if (pad->buttons & 0x8) {
            if (edlight_cur_light != -1) {
                edlight_cur_light--;
                if (edlight_cur_light < 0)
                    edlight_cur_light = LIGHTCOUNT - 1;
                edcamSetPos(&Lights[edlight_cur_light].pos);
            } else if (edlight_cur_dir != -1) {
                edlight_cur_dir--;
                if (edlight_cur_dir < 0)
                    edlight_cur_dir = LIGHTCOUNT - 1;
                edcamSetPos(&Lights[edlight_cur_dir].pos);
            } else if (LIGHTCOUNT > 0) {
                edlight_cur_light--;
                if (edlight_cur_light < 0)
                    edlight_cur_light = LIGHTCOUNT - 1;
                edcamSetPos(&Lights[edlight_cur_light].pos);
            }
        } else if (pad->buttons & 0x2) {
            if (edlight_cur_light != -1) {
                edlight_cur_light++;
                if (edlight_cur_light > LIGHTCOUNT - 1)
                    edlight_cur_light = 0;
                edcamSetPos(&Lights[edlight_cur_light].pos);
            } else if (edlight_cur_dir != -1) {
                edlight_cur_dir++;
                if (edlight_cur_dir > LIGHTCOUNT - 1)
                    edlight_cur_dir = 0;
                edcamSetPos(&Lights[edlight_cur_dir].pos);
            } else if (LIGHTCOUNT > 0) {
                edlight_cur_light++;
                if (edlight_cur_light > LIGHTCOUNT - 1)
                    edlight_cur_light = 0;
                edcamSetPos(&Lights[edlight_cur_light].pos);
            }
        }
    } else if (pad->buttons & 0x1000) {
        if (edlight_cur_light != -1)
            sel = edlight_cur_light;
        else if (edlight_cur_dir != -1)
            sel = edlight_cur_dir;
        if (sel != -1) {
            if (edlight_default_type == 2) {
                edlight_default_type = 0;
                Lights[sel].type = 0;
            } else if (edlight_default_type == 0) {
                edlight_default_type = 1;
                Lights[sel].type = 1;
            } else if (edlight_default_type == 1) {
                edlight_neg_r = Lights[sel].r;
                edlight_neg_g = Lights[sel].g;
                edlight_neg_b = Lights[sel].b;
                Lights[sel].type = 3;
                Lights[sel].r = 0;
                Lights[sel].g = 0;
                Lights[sel].b = 0;
                edlight_default_type = 3;
            }
        } else {
            if (edlight_default_type == 2)
                edlight_default_type = 0;
            else if (edlight_default_type == 0)
                edlight_default_type = 1;
            else if (edlight_default_type == 1)
                edlight_default_type = 3;
        }
    } else if (pad->buttons & 0x4000) {
        if (edlight_cur_light != -1)
            sel = edlight_cur_light;
        else if (edlight_cur_dir != -1)
            sel = edlight_cur_dir;
        if (sel != -1) {
            if (edlight_default_type == 3) {
                Lights[sel].type = 1;
                Lights[sel].r = edlight_neg_r;
                Lights[sel].g = edlight_neg_g;
                Lights[sel].b = edlight_neg_b;
                edlight_default_type = 1;
            } else if (edlight_default_type == 1) {
                edlight_default_type = 0;
                Lights[sel].type = 0;
            } else if (edlight_default_type == 0) {
                edlight_default_type = 2;
                Lights[sel].type = 2;
            }
        } else {
            if (edlight_default_type == 3)
                edlight_default_type = 1;
            else if (edlight_default_type == 1)
                edlight_default_type = 0;
            else if (edlight_default_type == 0)
                edlight_default_type = 2;
        }
    } else if (pad->buttons & 0x2000) {
        if (edlight_cur_light != -1)
            edcamSetPos(&Lights[edlight_cur_light].wpos);
        else if (edlight_cur_dir != -1)
            edcamSetPos(&Lights[edlight_cur_dir].pos);
    } else {
        edcamMove(pad);
    }

    if (pad->buttons & 0x80) {
        if (edlight_cur_light != -1 || LIGHTCOUNT == 0) {
            edlight_active_menu = edlight_options_menu;
            eduiItemSliderSetVal(edlight_r_slider, edlight_default_r);
            eduiItemSliderSetVal(edlight_g_slider, edlight_default_g);
            eduiItemSliderSetVal(edlight_b_slider, edlight_default_b);
        }
    } else if (pad->held & 0x20) {
        if (pad->buttons & 0x20) {
            if (edlight_cur_light == -1 && edlight_cur_dir == -1)
                edcamSetPos(&Lights[edlightClosestLight()].pos);
        } else {
            s32 idx = -1;

            if (edlight_cur_light != -1) {
                for (i = 0; i < LIGHTCOUNT; i++) {
                    if (i != edlight_cur_light) {
                        f32 scale;

                        NuVecSub(&probe, &Lights[i].pos, &edlight_cam_pos);
                        scale = Lights[edlight_cur_light].radius / NuVecMag(&probe);
                        step.x = probe.x * scale;
                        step.y = probe.y * scale;
                        step.z = probe.z * scale;
                        probe.x = edlight_cam_pos.x + step.x;
                        probe.y = edlight_cam_pos.y + step.y;
                        probe.z = edlight_cam_pos.z + step.z;
                    }
                    if (InsideLight(&probe, edlight_cur_light) &&
                        Lights[edlight_cur_light].glbtype != 4) {
                        edlight_inside_error = 1;
                    } else {
                        NuVecSub(&offset, &Lights[edlight_cur_light].wpos,
                                 &Lights[edlight_cur_light].pos);
                        Lights[edlight_cur_light].pos = edlight_cam_pos;
                        NuVecAdd(&Lights[edlight_cur_light].wpos, &offset,
                                 &Lights[edlight_cur_light].pos);
                        Lights[edlight_cur_light].radius =
                            fabs(NuVecDist(&Lights[edlight_cur_light].pos,
                                           &Lights[edlight_cur_light].wpos, 0));
                        NuVecSub(&Lights[edlight_cur_light].dir, &Lights[edlight_cur_light].pos,
                                 &Lights[edlight_cur_light].wpos);
                        NuVecNorm(&Lights[edlight_cur_light].dir, &Lights[edlight_cur_light].dir);
                    }
                }
            } else {
                if (edlight_cur_dir != -1) {
                    if (InsideLight(&edlight_cam_pos, edlight_cur_dir) &&
                        Lights[edlight_cur_dir].glbtype != 4) {
                        edlight_inside_error = 1;
                    } else {
                        edlight_inside_error = 0;
                        Lights[edlight_cur_dir].wpos = edlight_cam_pos;
                    }
                    radius_has_changed = 1;
                    edlight_last_light = edlight_cur_dir;
                    idx = edlight_cur_dir;
                }
                if (idx != -1) {
                    Lights[idx].radius =
                        fabs(NuVecDist(&Lights[idx].pos, &Lights[idx].wpos, 0));
                    NuVecSub(&Lights[idx].dir, &Lights[idx].pos, &Lights[idx].wpos);
                    NuVecNorm(&Lights[idx].dir, &Lights[idx].dir);
                }
            }
        }
    } else if (pad->buttons & 0x40) {
        if (edlight_cur_light == -1 && LIGHTCOUNT != 256 && !edlight_add_error &&
            !edlight_inside_error) {
            edlightAdd();
            if (edlight_default_type == 3) {
                Lights[LIGHTCOUNT - 1].r = 0;
                Lights[LIGHTCOUNT - 1].g = 0;
                Lights[LIGHTCOUNT - 1].b = 0;
            }
            if (radius_has_changed) {
                default_radius = Lights[edlight_last_light].radius;
                if (default_radius < 0.25)
                    default_radius = 1.0f;
                radius_has_changed = 0;
            }
            Lights[LIGHTCOUNT - 1].radius = default_radius;
            last_radius = Lights[LIGHTCOUNT - 1].pos;
            last_radius.z += default_radius;
            Lights[LIGHTCOUNT - 1].wpos = last_radius;
            Lights[LIGHTCOUNT - 1].radius = fabs(
                NuVecDist(&Lights[LIGHTCOUNT - 1].pos, &Lights[LIGHTCOUNT - 1].wpos, 0));
            NuVecSub(&Lights[LIGHTCOUNT - 1].dir, &Lights[LIGHTCOUNT - 1].pos,
                     &Lights[LIGHTCOUNT - 1].wpos);
            NuVecNorm(&Lights[LIGHTCOUNT - 1].dir, &Lights[LIGHTCOUNT - 1].dir);
        }
    } else if (pad->buttons & 0x10) {
        if (edlight_cur_light != -1)
            edlightDelete();
    }
}

inline void edlightHighlightDirection(void)
{
    s32 i;
    f32 mindist = 0.0f;
    struct nuvec_s d;

    edlight_cur_dir = -1;
    if (edlight_cur_light != -1)
        return;
    for (i = 0; i < LIGHTCOUNT; i++) {
        f32 dist2;

        NuVecSub(&d, &edlight_cam_pos, &Lights[i].wpos);
        dist2 = d.x * d.x + d.y * d.y + d.z * d.z;
        if (dist2 < 0.0225f) {
            if (edlight_cur_dir == -1 || dist2 < mindist) {
                edlight_cur_dir = i;
                mindist = dist2;
            }
        }
    }
}

s32 edlightProc(struct edpad_s *pad)
{

    if (screendump)
        return 0;
    if (edlight_active_menu) {
        eduiMenuProcess(edlight_active_menu, pad);
        return 0;
    }
    if (edlight_glb_amb_light == -1) {
        s32 i;

        for (i = 0; i < LIGHTCOUNT; i++) {
            if (Lights[i].glbtype == 4 && Lights[i].type == 0)
                edlight_glb_amb_light = i;
            if (Lights[i].glbtype == 4 && (Lights[i].type == 1 || Lights[i].type == 2))
                edlight_glb_dir_light = i;
        }
    }
    edlightDrawDirection();
    edlightHighlight();
    edlightHighlightDirection();
    edlightDoInput(pad);
    {
        s32 cancel = pad->buttons & 0x800;

        return cancel != 0;
    }
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
                  (edlight_default_b << 16) | (edlight_default_g << 8) |
                      edlight_default_r,
                  mtl);
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
    s32 idx = edlight_cur_light;

    if (idx == -1)
        return;
    l = &Lights[idx];
    if (l->type == 3)
        return;
    edlight_r_slider = slider;
    {
        s32 value = slider->val;

        edlight_default_r = value;
        l->r = value;
    }
    if (l)
        edlightMakeNUCOLOUR3(l->r, l->g, l->b, &l->col);
}

void edlightcb_g_sliderval(void *menu, struct slider_s *slider)
{
    struct edlight_s *l;
    s32 idx = edlight_cur_light;

    if (idx == -1)
        return;
    l = &Lights[idx];
    if (l->type == 3)
        return;
    edlight_g_slider = slider;
    {
        s32 value = slider->val;

        edlight_default_g = value;
        l->g = value;
    }
    if (l)
        edlightMakeNUCOLOUR3(l->r, l->g, l->b, &l->col);
}

void edlightcb_b_sliderval(void *menu, struct slider_s *slider)
{
    struct edlight_s *l;
    s32 idx = edlight_cur_light;

    if (idx == -1)
        return;
    l = &Lights[idx];
    if (l->type == 3)
        return;
    edlight_b_slider = slider;
    {
        s32 value = slider->val;

        edlight_default_b = value;
        l->b = value;
    }
    if (l)
        edlightMakeNUCOLOUR3(l->r, l->g, l->b, &l->col);
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
            pos = &player->obj.pos;
        else
            pos = &v000;
        edcamSetPosAng(pos, 0, 0);
        edlight_used = 1;
    }
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
                  (edlight_default_b << 16) | (edlight_default_g << 8) |
                      edlight_default_r,
                  mtl);
}

void cbSelGlbType(struct edmenu_s *menu, struct edsel_s *sel)
{
    s32 idx = edlight_cur_light;
    s32 arg = 0;
    struct edlight_s *l;
    void (*fn)(struct edmenu_s *menu, s32 arg);

    if (idx == -1)
        return;
    l = &Lights[idx];
    if (l->type == 1 || l->type == 2) {
        if (sel->newtype != l->glbtype) {
            if (l->glbtype == 4) {
                edlight_glb_dir_light = -1;
                l->glbtype = 5;
                glb_dir_error_flag = 0;
            } else if (edlight_glb_dir_light == -1) {
                l->glbtype = 4;
                glb_dir_error_flag = 1;
                edlight_glb_dir_light = idx;
            }
        }
    } else if (l->type == 0) {
        if (sel->newtype != l->glbtype) {
            if (l->glbtype == 4) {
                edlight_glb_amb_light = -1;
                l->glbtype = 5;
                glb_amb_error_flag = 0;
            } else if (edlight_glb_amb_light == -1) {
                l->glbtype = 4;
                glb_amb_error_flag = 1;
                edlight_glb_amb_light = idx;
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

void cbDoubleMenu(struct edmenu_s *menu, struct edsel_s *sel)
{
    struct eduistyle_s style = edlight_menu_style;

    edlight_double_menu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, cbCancelDoubleMenu,
                                         edlight_str_brightness);
    if (edlight_double_menu == 0)
        return;
    eduiMenuAddItem(edlight_double_menu,
                    eduiItemCheckCreate(6, &style, Lights[edlight_cur_light].dbltype == 6, 1,
                                        cbSelDoubleType, edlight_str_double));
    eduiMenuAddItem(edlight_double_menu,
                    eduiItemCheckCreate(7, &style, Lights[edlight_cur_light].dbltype == 7, 1,
                                        cbSelDoubleType, edlight_str_single));
    eduiMenuAttach(menu, edlight_double_menu);
    edlight_double_menu->x = menu->x + 10;
    edlight_double_menu->y = menu->y + 40;
}

void cbGlbMenu(struct edmenu_s *menu, struct edsel_s *sel)
{
    struct eduistyle_s style = edlight_menu_style;

    edlight_glb_menu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, cbCancelGlbMenu,
                                      edlight_str_glblocal);
    if (edlight_glb_menu == 0)
        return;
    eduiMenuAddItem(edlight_glb_menu,
                    eduiItemCheckCreate(4, &style, Lights[edlight_cur_light].glbtype == 4, 1,
                                        cbSelGlbType, edlight_str_global));
    eduiMenuAddItem(edlight_glb_menu,
                    eduiItemCheckCreate(5, &style, Lights[edlight_cur_light].glbtype == 5, 1,
                                        cbSelGlbType, edlight_str_local));
    eduiMenuAttach(menu, edlight_glb_menu);
    edlight_glb_menu->x = menu->x + 10;
    edlight_glb_menu->y = menu->y + 40;
}

void cbSelDoubleType(struct edmenu_s *menu, struct edsel_s *sel)
{
    s32 idx = edlight_cur_light;
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
