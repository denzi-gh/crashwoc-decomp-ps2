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
#include "game/gametypes.h"
#include "game/creature.h"
#include "nups2/ps2ctl.h"
#include "edtools/edui.h"
#include "edtools/edmain.h"

extern s32 edlight_used;
extern s32 screendump;
extern void *app_fnt;

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
extern f32 default_radius;
extern f32 topx, topy, topz;
extern f32 bottomx, bottomy, bottomz;
extern f32 northx, northy, northz;
extern f32 eastx, easty, eastz;
extern f32 southx, southy, southz;
extern f32 westx, westy, westz;

extern struct lights_s Lights[];
extern struct nuvec_s v000;
extern struct numtl_s *mtl;
extern struct nuvec_s edlight_cam_pos;
extern struct nuvec_s edlight_old_cam_pos;
extern struct nuvec_s last_radius;

static s32 edlight_cam_ax;
static s32 edlight_cam_ay;
static s32 edlight_in_crate;
static s32 edlight_in_light;
static s32 edlight_i_light;
static s32 edlight_d_light;
static s32 edlight_l_light;

extern void eduiMenuDetach(struct eduimenu_s *menu);
extern struct eduimenu_s *eduiMenuCreate(s32 x, s32 y, s32 w, s32 h, void *font,
                                       void *cancelcb, char *title);
extern void eduiMenuAttach(struct eduimenu_s *menu, struct eduimenu_s *child);
extern void *eduiMenuAddItem(struct eduimenu_s *menu, void *item);
extern void *eduiItemSelCreate(s32 value, struct eduistyle_s *style, s32 on, s32 group,
                               void *cb, char *text);
extern void *eduiItemCheckCreate(s32 value, struct eduistyle_s *style, s32 checked,
                                 s32 group, void *cb, char *text);
extern void *eduiItemColourSliderCreate(s32 value, struct eduistyle_s *style, s32 on,
                                        void *cb, f32 min, f32 max, f32 val, s32 r,
                                        s32 g, s32 b, char *text);
extern s32 eduiMenuProcess(void *menu, struct nupad_s *pad);
extern void edlightDoInput(struct nupad_s *pad);
extern void edcamGetPosAng(struct nuvec_s *pos, s32 *ax, s32 *ay);
extern void edcamSetPos(struct nuvec_s *pos);
extern void edcamMove(struct nupad_s *pad);
extern void cbSelGlbType(struct eduimenu_s *menu, struct eduiitem_s *sel);
/* The functions below are inline: GCC 2.95 then emits their code at the end
 * of the unit (in this declaration order, after edlightRender, as retail has
 * it) but their literals where they are defined, which is where retail keeps
 * them. */
extern void edlightHighlightDirection(void);
extern void edlightDrawLights(void);
extern void edlightcb_r_sliderval(void *menu, struct eduiitem_s *slider);
extern void edlightcb_g_sliderval(void *menu, struct eduiitem_s *slider);
extern void edlightcb_b_sliderval(void *menu, struct eduiitem_s *slider);
extern void edlightcbSaveLightData(void *menu);
extern void edlightCbCancelOptMenu(void);
extern void edlightClose(void);
extern void edlightEnter(void);
extern void edlightMakeNUCOLOUR3(s32 r, s32 g, s32 b, struct nucolour3_s *col);
extern void edlightWarning(char *text);
extern void edlightDrawLightBlock(void);
static void cbDoubleMenu(struct eduimenu_s *menu, struct eduiitem_s *sel);
static void cbGlbMenu(struct eduimenu_s *menu, struct eduiitem_s *sel);
static void cbSelDoubleType(struct eduimenu_s *menu, struct eduiitem_s *sel);
static void cbCancelDoubleMenu(void);
static void cbCancelGlbMenu(void);

extern void eduiMenuDestroy(struct eduimenu_s *menu);
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

void edlightInit(void);
s32 edlightProc(struct nupad_s *pad);
void edlightRender(void);

void *edlight_active_menu = 0;
struct eduimenu_s *edlight_options_menu = 0;
struct eduimenu_s *edlight_double_menu = 0;
struct eduimenu_s *edlight_glb_menu = 0;
struct eduiitem_s *edlight_r_slider = 0;
struct eduiitem_s *edlight_g_slider = 0;
struct eduiitem_s *edlight_b_slider = 0;
static s32 edlight_type = 0;
static u8 Gr = 0xFF;
static u8 Gg = 0xFF;
static u8 Gb = 0xFF;
static u8 Lr = 0xFF;
static u8 Lg = 0xFF;
static u8 Lb = 0xFF;
s32 edlight_glb_amb_light = -1;
s32 edlight_glb_dir_light = -1;
s32 glb_amb_error_flag = -1;
s32 glb_dir_error_flag = -1;
s32 radius_has_changed = 0;

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
                if (i != edlight_i_light && i != edlight_d_light && i != idx &&
                    Lights[i].globalflag != 4 && Lights[i].type == 0) {
                    dist = fabs(NuVecDist(&Lights[idx].pos, pos, 0));
                    if ((f32)fabs(NuVecDist(&Lights[i].pos, &Lights[idx].pos, 0)) -
                            Lights[i].radius < dist)
                        inside = 1;
                }
            }
        } else if (Lights[idx].type == 3) {
            for (i = 0; i < LIGHTCOUNT; i++) {
                if (i != edlight_i_light && i != edlight_d_light && i != idx &&
                    Lights[i].globalflag != 4 && Lights[i].type == 3) {
                    dist = fabs(NuVecDist(&Lights[idx].pos, pos, 0));
                    if ((f32)fabs(NuVecDist(&Lights[i].pos, &Lights[idx].pos, 0)) -
                            Lights[i].radius < dist)
                        inside = 1;
                }
            }
        }
    } else if (edlight_type == 0) {
        for (i = 0; i < LIGHTCOUNT; i++) {
            if (Lights[i].globalflag != 4 && Lights[i].type == 0) {
                if ((f32)fabs(NuVecDist(&Lights[i].pos, pos, 0)) <
                        Lights[i].radius + default_radius)
                    inside = 1;
            }
        }
    } else if (edlight_type == 3) {
        for (i = 0; i < LIGHTCOUNT; i++) {
            if (Lights[i].globalflag != 4 && Lights[i].type == 3) {
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
    strcpy(tbuf, LDATA->path);
    strupr(tbuf);
    NuFntPrintEx(0x400, 0x100, 0, "%s.LGT", tbuf);
    NuFntPrintEx(0x400, 0x200, 0, "LIGHTCOUNT:   %i/%i", LIGHTCOUNT, 0x100);
    if (edlight_i_light != -1)
        NuFntPrintEx(0x400, 0x300, 0, "CURRENT LIGHT: %i", edlight_i_light);
    else
        NuFntPrintEx(0x400, 0x300, 0, "NO CURRENT LIGHT");
    NuFntPrintEx(0x400, 0xD00, 0, "CURSOR:   X: %.2f   Y: %.2f   Z: %.2f", edlight_cam_pos.x,
                 edlight_cam_pos.y, edlight_cam_pos.z);
    if (edlight_i_light == -1)
        NuFntPrintEx(0x400, 0x400, 0, "R:   %i   G:   %i    B:    %i", 0, 0, 0);
    else
        NuFntPrintEx(0x400, 0x400, 0, "R:   %i   G:   %i    B:    %i", Lights[edlight_i_light].r,
                     Lights[edlight_i_light].g, Lights[edlight_i_light].b);
    if (edlight_i_light == -1) {
        if (edlight_type == 0)
            NuFntPrintEx(0x400, 0x500, 0, "DEFAULT TYPE: AMBIENT");
        else if (edlight_type == 1)
            NuFntPrintEx(0x400, 0x500, 0, "DEFAULT TYPE: DIRECTIONAL");
        else if (edlight_type == 2)
            NuFntPrintEx(0x400, 0x500, 0, "DEFAULT TYPE: POINT");
        else if (edlight_type == 3)
            NuFntPrintEx(0x400, 0x500, 0, "DEFAULT TYPE: NEGATIVE");
        return;
    }
    if (Lights[edlight_i_light].type == 0)
        NuFntPrintEx(0x400, 0x500, 0, "SELECTED LIGHT TYPE: AMBIENT");
    else if (Lights[edlight_i_light].type == 1)
        NuFntPrintEx(0x400, 0x500, 0, "SELECTED LIGHT TYPE: DIRECTIONAL");
    else if (Lights[edlight_i_light].type == 2)
        NuFntPrintEx(0x400, 0x500, 0, "SELECTED LIGHT TYPE: POINT");
    else if (Lights[edlight_i_light].type == 3)
        NuFntPrintEx(0x400, 0x500, 0, "SELECTED LIGHT TYPE: NEGATIVE");
    NuFntPrintEx(0x400, 0x600, 0, "CURRENT RADIUS: %.2f", Lights[edlight_i_light].radius);
    if (Lights[edlight_i_light].globalflag == 4)
        NuFntPrintEx(0x400, 0x700, 0, "LIGHT IS GLOBAL");
    else
        NuFntPrintEx(0x400, 0x700, 0, "LIGHT IS LOCAL");
    if (edlight_glb_amb_light != -1 && glb_amb_error_flag &&
        edlight_i_light != edlight_glb_amb_light && Lights[edlight_i_light].type == 0) {
        NuFntSetPen(0xFF0000FF);
        NuFntPrintEx(0x400, 0x900, 0, "GLOBAL AMBIENT EXISTS");
    }
    if (edlight_glb_dir_light != -1 && glb_dir_error_flag &&
        edlight_i_light != edlight_glb_dir_light &&
        (Lights[edlight_i_light].type == 1 || Lights[edlight_i_light].type == 2)) {
        NuFntSetPen(0xFF0000FF);
        NuFntPrintEx(0x400, 0x900, 0, "GLOBAL DIRECTIONAL/POINT EXISTS");
    }
    NuFntSetPen(-1);
    if (Lights[edlight_i_light].brightness == 6 && Lights[edlight_i_light].type != 3)
        NuFntPrintEx(0x400, 0x800, 0, "LIGHT IS DOUBLE BRIGHTNESS");
    else if (Lights[edlight_i_light].brightness == 7 && Lights[edlight_i_light].type != 3)
        NuFntPrintEx(0x400, 0x800, 0, "LIGHT IS SINGLE BRIGHTNESS");
}

void edlightDrawLightAxis(struct lights_s *l)
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

    edlight_i_light = -1;
    if (edlight_d_light == -1) {
        for (i = 0; i < LIGHTCOUNT; i++) {
            f32 dist2;

            NuVecSub(&d, &edlight_cam_pos, &Lights[i].pos);
            dist2 = d.x * d.x + d.y * d.y + d.z * d.z;
            if (dist2 < 0.0225f) {
                if (edlight_i_light == -1 || dist2 < mindist) {
                    edlight_i_light = i;
                    mindist = dist2;
                }
            }
        }
    }
    if (edlight_i_light != -1 && LIGHTCOUNT > 0) {
        edlight_l_light = edlight_i_light;
        Gr = Lights[edlight_i_light].r;
        Gg = Lights[edlight_i_light].g;
        Gb = Lights[edlight_i_light].b;
    }
}

void edlightDrawBox(void)
{
    struct nuvec_s pt[8];
    s32 idx = edlight_d_light;
    f32 size = 0.0375f;
    s32 i;

    if (idx != -1) {
        for (i = 0; i < 8; i++)
            pt[i] = Lights[idx].radius_pos;
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
    s32 idx = edlight_i_light;
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
    if (edlight_in_crate || edlight_in_light) {
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
    static s32 r = 0;
    static s32 g = 0;
    static s32 b = 0;
    s32 i;

    for (i = 0; i < LIGHTCOUNT; i++) {
        struct nuvec_s pos = Lights[i].pos;
        struct nuvec_s wpos = Lights[i].radius_pos;

        r = Lights[i].r;
        g = Lights[i].g;
        b = Lights[i].b;
        NuRndrLine3dDbg(((b & 0xFF) << 16) | ((g & 0xFF) << 8) | (r & 0xFF),
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
    if (edlight_type == 0)
        AMBIENTCOUNT++;
    else if (edlight_type == 1)
        DIRECTCOUNT++;
    else if (edlight_type == 2)
        POINTCOUNT++;
    else if (edlight_type == 3)
        NEGATIVECOUNT++;
    Lights[LIGHTCOUNT].type = edlight_type;
    Lights[LIGHTCOUNT].pos = edlight_cam_pos;
    Lights[LIGHTCOUNT].radius_pos = edlight_cam_pos;
    Lights[LIGHTCOUNT].radius = 0.0f;
    Lights[LIGHTCOUNT].r = Gr;
    Lights[LIGHTCOUNT].g = Gg;
    Lights[LIGHTCOUNT].b = Gb;
    Lights[LIGHTCOUNT].colour.r = 1.0f;
    Lights[LIGHTCOUNT].colour.g = 1.0f;
    Lights[LIGHTCOUNT].colour.b = 1.0f;
    Lights[LIGHTCOUNT].direction.x = 0.0f;
    Lights[LIGHTCOUNT].direction.y = 0.0f;
    Lights[LIGHTCOUNT].direction.z = 0.0f;
    Lights[LIGHTCOUNT].globalflag = 5;
    Lights[LIGHTCOUNT].blendtype = 8;
    Lights[LIGHTCOUNT].brightness = 7;
    LIGHTCOUNT++;
}

void edlightDelete(void)
{
    s32 i;

    if (edlight_i_light == -1)
        return;
    if (Lights[edlight_i_light].type == 0) {
        if (Lights[edlight_i_light].globalflag == 4) {
            edlight_glb_amb_light = -1;
            glb_amb_error_flag = 0;
        }
    } else if (Lights[edlight_i_light].type == 1 || Lights[edlight_i_light].type == 2) {
        if (Lights[edlight_i_light].globalflag == 4) {
            edlight_glb_dir_light = -1;
            glb_dir_error_flag = 0;
        }
    }
    if (Lights[edlight_i_light].type == 0)
        AMBIENTCOUNT--;
    else if (Lights[edlight_i_light].type == 1)
        DIRECTCOUNT--;
    else if (Lights[edlight_i_light].type == 2)
        POINTCOUNT--;
    else if (Lights[edlight_i_light].type == 3)
        NEGATIVECOUNT--;
    LIGHTCOUNT--;
    for (i = edlight_i_light; i <= LIGHTCOUNT; i++) {
        Lights[i].type = Lights[i + 1].type;
        Lights[i].pos = Lights[i + 1].pos;
        Lights[i].radius_pos = Lights[i + 1].radius_pos;
        Lights[i].r = Lights[i + 1].r;
        Lights[i].g = Lights[i + 1].g;
        Lights[i].b = Lights[i + 1].b;
        Lights[i].radius = Lights[i + 1].radius;
        Lights[i].colour = Lights[i + 1].colour;
        Lights[i].direction = Lights[i + 1].direction;
        Lights[i].globalflag = Lights[i + 1].globalflag;
        Lights[i].blendtype = Lights[i + 1].blendtype;
        Lights[i].brightness = Lights[i + 1].brightness;
    }
    edlight_i_light = -1;
}

void cbSelGlbType(struct eduimenu_s *menu, struct eduiitem_s *sel)
{
    s32 idx = edlight_i_light;
    struct eduimenu_s *arg = 0;
    struct lights_s *l;
    void (*fn)(struct eduimenu_s *menu, struct eduimenu_s *parent);

    if (idx == -1)
        return;
    l = &Lights[idx];
    if (l->type == 1 || l->type == 2) {
        if (sel->value != l->globalflag) {
            if (l->globalflag == 4) {
                edlight_glb_dir_light = -1;
                l->globalflag = 5;
                glb_dir_error_flag = 0;
            } else if (edlight_glb_dir_light == -1) {
                l->globalflag = 4;
                glb_dir_error_flag = 1;
                edlight_glb_dir_light = idx;
            }
        }
    } else if (l->type == 0) {
        if (sel->value != l->globalflag) {
            if (l->globalflag == 4) {
                edlight_glb_amb_light = -1;
                l->globalflag = 5;
                glb_amb_error_flag = 0;
            } else if (edlight_glb_amb_light == -1) {
                l->globalflag = 4;
                glb_amb_error_flag = 1;
                edlight_glb_amb_light = idx;
            }
        }
    }
    if (menu->parent != 0) {
        arg = menu->parent;
        eduiMenuDetach(menu);
    }
    fn = menu->cancel;
    if (fn)
        fn(menu, arg);
}

inline void edlightcbSaveLightData(void *menu)
{
    SaveLights();
    eduiCreateMessageMenu(menu, "Saved OK", 1);
}

static inline void cbDoubleMenu(struct eduimenu_s *menu, struct eduiitem_s *sel)
{
    struct eduistyle_s style = { { 0x80000000, 0x80FF0000 }, { 0x80808080, 0x80404040 } };

    edlight_double_menu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, cbCancelDoubleMenu,
                                         "Brightness");
    if (edlight_double_menu == 0)
        return;
    eduiMenuAddItem(edlight_double_menu,
                    eduiItemCheckCreate(6, &style, Lights[edlight_i_light].brightness == 6, 1,
                                        cbSelDoubleType, "Double"));
    eduiMenuAddItem(edlight_double_menu,
                    eduiItemCheckCreate(7, &style, Lights[edlight_i_light].brightness == 7, 1,
                                        cbSelDoubleType, "Single"));
    eduiMenuAttach(menu, edlight_double_menu);
    edlight_double_menu->x = menu->x + 10;
    edlight_double_menu->y = menu->y + 40;
}

static inline void cbGlbMenu(struct eduimenu_s *menu, struct eduiitem_s *sel)
{
    struct eduistyle_s style = { { 0x80000000, 0x80FF0000 }, { 0x80808080, 0x80404040 } };

    edlight_glb_menu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt, cbCancelGlbMenu,
                                      "Global/Local Light");
    if (edlight_glb_menu == 0)
        return;
    eduiMenuAddItem(edlight_glb_menu,
                    eduiItemCheckCreate(4, &style, Lights[edlight_i_light].globalflag == 4, 1,
                                        cbSelGlbType, "Global"));
    eduiMenuAddItem(edlight_glb_menu,
                    eduiItemCheckCreate(5, &style, Lights[edlight_i_light].globalflag == 5, 1,
                                        cbSelGlbType, "Local"));
    eduiMenuAttach(menu, edlight_glb_menu);
    edlight_glb_menu->x = menu->x + 10;
    edlight_glb_menu->y = menu->y + 40;
}

void edlightInit(void)
{
    struct eduistyle_s style = { { 0x80000000, 0x80FF0000 }, { 0x80808080, 0x80404040 } };
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
    if (edlight_i_light != -1) {
        r = Lights[edlight_i_light].r;
        g = Lights[edlight_i_light].g;
        b = Lights[edlight_i_light].b;
    } else {
        r = Gr;
        g = Gg;
        b = Gb;
    }
    edlight_options_menu = eduiMenuCreate(0x46, 0x46, 0xB4, 0x12C, app_fnt,
                                          edlightCbCancelOptMenu, "Options");
    if (edlight_options_menu) {
        eduiMenuAddItem(edlight_options_menu,
                        eduiItemSelCreate(1, &style, 0, 0, edlightcbSaveLightData,
                                          "Save Light Data"));
        if (edlight_i_light != -1) {
            eduiMenuAddItem(edlight_options_menu,
                            eduiItemColourSliderCreate(0, &style, 0, edlightcb_r_sliderval,
                                                       0.0f, 255.0f, r, 0xFF, 0, 0,
                                                       "Red"));
            eduiMenuAddItem(edlight_options_menu,
                            eduiItemColourSliderCreate(0, &style, 0, edlightcb_g_sliderval,
                                                       0.0f, 255.0f, g, 0, 0xFF, 0,
                                                       "Green"));
            eduiMenuAddItem(edlight_options_menu,
                            eduiItemColourSliderCreate(0, &style, 0, edlightcb_b_sliderval,
                                                       0.0f, 255.0f, b, 0, 0, 0xFF,
                                                       "Blue"));
            eduiMenuAddItem(edlight_options_menu,
                            eduiItemSelCreate(1, &style, 0, 0, cbGlbMenu, "Global Options"));
            eduiMenuAddItem(edlight_options_menu,
                            eduiItemSelCreate(1, &style, 0, 0, cbDoubleMenu, "Light Power"));
        }
    }
    if (LIGHTCOUNT == 0) {
        Gr = 0xFF;
        Gg = 0xFF;
        Gb = 0xFF;
    }
    if (LIGHTCOUNT > 0) {
        default_radius = Lights[LIGHTCOUNT].radius;
        if (default_radius < 0.25)
            default_radius = 1.0f;
    } else {
        default_radius = 1.0f;
    }
    if (LIGHTCOUNT > 0) {
        last_radius = Lights[LIGHTCOUNT].radius_pos;
    } else {
        last_radius.x = 0.0f;
        last_radius.y = 0.0f;
        last_radius.z = 0.0f;
    }
}

s32 edlight_used = 0;
struct numtl_s *mtl = 0;
f32 default_radius = 0.0f;
f32 topx = 0.0f, topy = 0.0f, topz = 0.0f;
f32 bottomx = 0.0f, bottomy = 0.0f, bottomz = 0.0f;
f32 northx = 0.0f, northy = 0.0f, northz = 0.0f;
f32 eastx = 0.0f, easty = 0.0f, eastz = 0.0f;
f32 southx = 0.0f, southy = 0.0f, southz = 0.0f;
f32 westx = 0.0f, westy = 0.0f, westz = 0.0f;

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

void edlightDoInput(struct nupad_s *pad)
{
    struct nuvec_s step;
    struct nuvec_s probe;
    struct nuvec_s offset;
    s32 sel = -1;
    s32 i;

    edcamGetPosAng(&edlight_cam_pos, &edlight_cam_ax, &edlight_cam_ay);
    edlight_old_cam_pos = edlight_cam_pos;
    if (edlight_i_light == -1 && edlight_d_light == -1) {
        if (InsideLight(&edlight_cam_pos, -1))
            edlight_in_light = 1;
        else
            edlight_in_light = 0;
    } else if (edlight_i_light != -1) {
        if (InsideLight(&edlight_cam_pos, edlight_i_light))
            edlight_in_light = 1;
        else
            edlight_in_light = 0;
    } else {
        if (InsideLight(&edlight_cam_pos, edlight_d_light))
            edlight_in_light = 1;
        else
            edlight_in_light = 0;
    }

    if (pad->paddata & 0x100) {
        if (pad->paddata_db & 0x8) {
            if (edlight_i_light != -1) {
                edlight_i_light--;
                if (edlight_i_light < 0)
                    edlight_i_light = LIGHTCOUNT - 1;
                edcamSetPos(&Lights[edlight_i_light].pos);
            } else if (edlight_d_light != -1) {
                edlight_d_light--;
                if (edlight_d_light < 0)
                    edlight_d_light = LIGHTCOUNT - 1;
                edcamSetPos(&Lights[edlight_d_light].pos);
            } else if (LIGHTCOUNT > 0) {
                edlight_i_light--;
                if (edlight_i_light < 0)
                    edlight_i_light = LIGHTCOUNT - 1;
                edcamSetPos(&Lights[edlight_i_light].pos);
            }
        } else if (pad->paddata_db & 0x2) {
            if (edlight_i_light != -1) {
                edlight_i_light++;
                if (edlight_i_light > LIGHTCOUNT - 1)
                    edlight_i_light = 0;
                edcamSetPos(&Lights[edlight_i_light].pos);
            } else if (edlight_d_light != -1) {
                edlight_d_light++;
                if (edlight_d_light > LIGHTCOUNT - 1)
                    edlight_d_light = 0;
                edcamSetPos(&Lights[edlight_d_light].pos);
            } else if (LIGHTCOUNT > 0) {
                edlight_i_light++;
                if (edlight_i_light > LIGHTCOUNT - 1)
                    edlight_i_light = 0;
                edcamSetPos(&Lights[edlight_i_light].pos);
            }
        }
    } else if (pad->paddata_db & 0x1000) {
        if (edlight_i_light != -1)
            sel = edlight_i_light;
        else if (edlight_d_light != -1)
            sel = edlight_d_light;
        if (sel != -1) {
            if (edlight_type == 2) {
                edlight_type = 0;
                Lights[sel].type = 0;
            } else if (edlight_type == 0) {
                edlight_type = 1;
                Lights[sel].type = 1;
            } else if (edlight_type == 1) {
                Lr = Lights[sel].r;
                Lg = Lights[sel].g;
                Lb = Lights[sel].b;
                Lights[sel].type = 3;
                Lights[sel].r = 0;
                Lights[sel].g = 0;
                Lights[sel].b = 0;
                edlight_type = 3;
            }
        } else {
            if (edlight_type == 2)
                edlight_type = 0;
            else if (edlight_type == 0)
                edlight_type = 1;
            else if (edlight_type == 1)
                edlight_type = 3;
        }
    } else if (pad->paddata_db & 0x4000) {
        if (edlight_i_light != -1)
            sel = edlight_i_light;
        else if (edlight_d_light != -1)
            sel = edlight_d_light;
        if (sel != -1) {
            if (edlight_type == 3) {
                Lights[sel].type = 1;
                Lights[sel].r = Lr;
                Lights[sel].g = Lg;
                Lights[sel].b = Lb;
                edlight_type = 1;
            } else if (edlight_type == 1) {
                edlight_type = 0;
                Lights[sel].type = 0;
            } else if (edlight_type == 0) {
                edlight_type = 2;
                Lights[sel].type = 2;
            }
        } else {
            if (edlight_type == 3)
                edlight_type = 1;
            else if (edlight_type == 1)
                edlight_type = 0;
            else if (edlight_type == 0)
                edlight_type = 2;
        }
    } else if (pad->paddata_db & 0x2000) {
        if (edlight_i_light != -1)
            edcamSetPos(&Lights[edlight_i_light].radius_pos);
        else if (edlight_d_light != -1)
            edcamSetPos(&Lights[edlight_d_light].pos);
    } else {
        edcamMove(pad);
    }

    if (pad->paddata_db & 0x80) {
        if (edlight_i_light != -1 || LIGHTCOUNT == 0) {
            edlight_active_menu = edlight_options_menu;
            eduiItemSliderSetVal(edlight_r_slider, Gr);
            eduiItemSliderSetVal(edlight_g_slider, Gg);
            eduiItemSliderSetVal(edlight_b_slider, Gb);
        }
    } else if (pad->paddata & 0x20) {
        if (pad->paddata_db & 0x20) {
            if (edlight_i_light == -1 && edlight_d_light == -1)
                edcamSetPos(&Lights[edlightClosestLight()].pos);
        } else {
            s32 idx = -1;

            if (edlight_i_light != -1) {
                for (i = 0; i < LIGHTCOUNT; i++) {
                    if (i != edlight_i_light) {
                        f32 scale;

                        NuVecSub(&probe, &Lights[i].pos, &edlight_cam_pos);
                        scale = Lights[edlight_i_light].radius / NuVecMag(&probe);
                        step.x = probe.x * scale;
                        step.y = probe.y * scale;
                        step.z = probe.z * scale;
                        probe.x = edlight_cam_pos.x + step.x;
                        probe.y = edlight_cam_pos.y + step.y;
                        probe.z = edlight_cam_pos.z + step.z;
                    }
                    if (InsideLight(&probe, edlight_i_light) &&
                        Lights[edlight_i_light].globalflag != 4) {
                        edlight_in_light = 1;
                    } else {
                        NuVecSub(&offset, &Lights[edlight_i_light].radius_pos,
                                 &Lights[edlight_i_light].pos);
                        Lights[edlight_i_light].pos = edlight_cam_pos;
                        NuVecAdd(&Lights[edlight_i_light].radius_pos, &offset,
                                 &Lights[edlight_i_light].pos);
                        Lights[edlight_i_light].radius =
                            fabs(NuVecDist(&Lights[edlight_i_light].pos,
                                           &Lights[edlight_i_light].radius_pos, 0));
                        NuVecSub(&Lights[edlight_i_light].direction, &Lights[edlight_i_light].pos,
                                 &Lights[edlight_i_light].radius_pos);
                        NuVecNorm(&Lights[edlight_i_light].direction, &Lights[edlight_i_light].direction);
                    }
                }
            } else {
                if (edlight_d_light != -1) {
                    if (InsideLight(&edlight_cam_pos, edlight_d_light) &&
                        Lights[edlight_d_light].globalflag != 4) {
                        edlight_in_light = 1;
                    } else {
                        edlight_in_light = 0;
                        Lights[edlight_d_light].radius_pos = edlight_cam_pos;
                    }
                    radius_has_changed = 1;
                    edlight_l_light = edlight_d_light;
                    idx = edlight_d_light;
                }
                if (idx != -1) {
                    Lights[idx].radius =
                        fabs(NuVecDist(&Lights[idx].pos, &Lights[idx].radius_pos, 0));
                    NuVecSub(&Lights[idx].direction, &Lights[idx].pos, &Lights[idx].radius_pos);
                    NuVecNorm(&Lights[idx].direction, &Lights[idx].direction);
                }
            }
        }
    } else if (pad->paddata_db & 0x40) {
        if (edlight_i_light == -1 && LIGHTCOUNT != 256 && !edlight_in_crate &&
            !edlight_in_light) {
            edlightAdd();
            if (edlight_type == 3) {
                Lights[LIGHTCOUNT - 1].r = 0;
                Lights[LIGHTCOUNT - 1].g = 0;
                Lights[LIGHTCOUNT - 1].b = 0;
            }
            if (radius_has_changed) {
                default_radius = Lights[edlight_l_light].radius;
                if (default_radius < 0.25)
                    default_radius = 1.0f;
                radius_has_changed = 0;
            }
            Lights[LIGHTCOUNT - 1].radius = default_radius;
            last_radius = Lights[LIGHTCOUNT - 1].pos;
            last_radius.z += default_radius;
            Lights[LIGHTCOUNT - 1].radius_pos = last_radius;
            Lights[LIGHTCOUNT - 1].radius = fabs(
                NuVecDist(&Lights[LIGHTCOUNT - 1].pos, &Lights[LIGHTCOUNT - 1].radius_pos, 0));
            NuVecSub(&Lights[LIGHTCOUNT - 1].direction, &Lights[LIGHTCOUNT - 1].pos,
                     &Lights[LIGHTCOUNT - 1].radius_pos);
            NuVecNorm(&Lights[LIGHTCOUNT - 1].direction, &Lights[LIGHTCOUNT - 1].direction);
        }
    } else if (pad->paddata_db & 0x10) {
        if (edlight_i_light != -1)
            edlightDelete();
    }
}

inline void edlightHighlightDirection(void)
{
    s32 i;
    f32 mindist = 0.0f;
    struct nuvec_s d;

    edlight_d_light = -1;
    if (edlight_i_light != -1)
        return;
    for (i = 0; i < LIGHTCOUNT; i++) {
        f32 dist2;

        NuVecSub(&d, &edlight_cam_pos, &Lights[i].radius_pos);
        dist2 = d.x * d.x + d.y * d.y + d.z * d.z;
        if (dist2 < 0.0225f) {
            if (edlight_d_light == -1 || dist2 < mindist) {
                edlight_d_light = i;
                mindist = dist2;
            }
        }
    }
}

s32 edlightProc(struct nupad_s *pad)
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
            if (Lights[i].globalflag == 4 && Lights[i].type == 0)
                edlight_glb_amb_light = i;
            if (Lights[i].globalflag == 4 && (Lights[i].type == 1 || Lights[i].type == 2))
                edlight_glb_dir_light = i;
        }
    }
    edlightDrawDirection();
    edlightHighlight();
    edlightHighlightDirection();
    edlightDoInput(pad);
    {
        s32 cancel = pad->paddata_db & 0x800;

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
                  (Gb << 16) | (Gg << 8) | Gr, mtl);
    for (i = 0; i < LIGHTCOUNT; i++) {
        edlightDrawLightAxis(&Lights[i]);
        edlightDrawRadius(&Lights[i].pos, Lights[i].r, Lights[i].g, Lights[i].b, 0.05f);
    }
    if (edlight_active_menu) {
        NuFntScale(0xC, 0xC);
        eduiMenuRender(edlight_active_menu);
    }
}

inline void edlightDrawLights(void)
{
    s32 i;

    for (i = 0; i < LIGHTCOUNT; i++) {
        edlightDrawLightAxis(&Lights[i]);
        edlightDrawRadius(&Lights[i].pos, Lights[i].r, Lights[i].g, Lights[i].b, 0.05f);
    }
}

inline void edlightcb_r_sliderval(void *menu, struct eduiitem_s *slider)
{
    struct lights_s *l;
    s32 idx = edlight_i_light;

    if (idx == -1)
        return;
    l = &Lights[idx];
    if (l->type == 3)
        return;
    edlight_r_slider = slider;
    {
        s32 value = slider->u.slider.val;

        Gr = value;
        l->r = value;
    }
    if (l)
        edlightMakeNUCOLOUR3(l->r, l->g, l->b, &l->colour);
}

inline void edlightcb_g_sliderval(void *menu, struct eduiitem_s *slider)
{
    struct lights_s *l;
    s32 idx = edlight_i_light;

    if (idx == -1)
        return;
    l = &Lights[idx];
    if (l->type == 3)
        return;
    edlight_g_slider = slider;
    {
        s32 value = slider->u.slider.val;

        Gg = value;
        l->g = value;
    }
    if (l)
        edlightMakeNUCOLOUR3(l->r, l->g, l->b, &l->colour);
}

inline void edlightcb_b_sliderval(void *menu, struct eduiitem_s *slider)
{
    struct lights_s *l;
    s32 idx = edlight_i_light;

    if (idx == -1)
        return;
    l = &Lights[idx];
    if (l->type == 3)
        return;
    edlight_b_slider = slider;
    {
        s32 value = slider->u.slider.val;

        Gb = value;
        l->b = value;
    }
    if (l)
        edlightMakeNUCOLOUR3(l->r, l->g, l->b, &l->colour);
}

inline void edlightCbCancelOptMenu(void)
{
    edlight_active_menu = 0;
}

inline void edlightClose(void)
{
    eduiMenuDestroy(edlight_options_menu);
}

inline void edlightEnter(void)
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

inline void edlightWarning(char *text)
{
    NuFntSetPen(0xFF0000FF);
    NuFntPrintEx(1024, 256, 0, text);
    NuFntSetPen(0xFFFFFF);
}

inline void edlightDrawLightBlock(void)
{
    NuRndrRect2di(0x400, 0xB80, SWIDTH / 16 * 16, SHEIGHT / 16 * 8,
                  (Gb << 16) | (Gg << 8) | Gr, mtl);
}

static inline void cbSelDoubleType(struct eduimenu_s *menu, struct eduiitem_s *sel)
{
    s32 idx = edlight_i_light;
    struct eduimenu_s *arg = 0;
    void (*fn)(struct eduimenu_s *menu, struct eduimenu_s *parent);

    if (idx != -1) {
        struct lights_s *l = &Lights[idx];
        if (sel->value != l->brightness)
            l->brightness = sel->value;
    }
    if (menu->parent != 0) {
        arg = menu->parent;
        eduiMenuDetach(menu);
    }
    fn = menu->cancel;
    if (fn)
        fn(menu, arg);
}

static inline void cbCancelDoubleMenu(void)
{
    eduiMenuDestroy(edlight_double_menu);
    edlight_double_menu = 0;
}

static inline void cbCancelGlbMenu(void)
{
    eduiMenuDestroy(edlight_glb_menu);
    edlight_glb_menu = 0;
}

struct edmainreg_s edlightdesc = {
    0, 0, "Light Editor", edlightInit, edlightClose, edlightEnter, {0}, 0,
    EDMAIN_BLOCKID('l', 'g', 'h', 't'), edlightProc, edlightRender
};
struct nuvec_s edlight_cam_pos = { 0.0f, 0.0f, 0.0f };
struct nuvec_s edlight_old_cam_pos = { 0.0f, 0.0f, 0.0f };
struct nuvec_s last_radius = { 0.0f, 0.0f, 0.0f };
