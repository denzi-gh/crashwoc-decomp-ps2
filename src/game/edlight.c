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
    f32 radius;            /* 0x1C */
    u8 r;                  /* 0x20 */
    u8 g;                  /* 0x21 */
    u8 b;                  /* 0x22 */
    u8 pad23;              /* 0x23 */
    struct nucolour3_s col;/* 0x24 */
    struct nuangvec_s ang; /* 0x30 */
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
#define edlight_cur_light D_00633374
#define edlight_cur_dir D_00633378
#define edlight_last_light D_0063337C
#define edlight_default_r D_00632690
#define edlight_default_g D_00632691
#define edlight_default_b D_00632692

extern char D_006326B0[];
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
extern f32 D_0062E688;
extern f32 D_0062E690;
extern u8 D_00632690;
extern u8 D_00632691;
extern u8 D_00632692;

extern void eduiMenuDetach(struct edmenu_s *menu);

extern void eduiMenuDestroy(void *menu);
extern void eduiCreateMessageMenu(void *menu, char *msg, s32 flag);
extern void SaveLights(void);
extern char *strcpy(char *dst, const char *src);
extern char *strupr(char *s);
extern void NuFntSetPen(s32 colour);
extern void NuFntPrintEx(s32 x, s32 y, s32 colour, char *fmt, ...);
extern void NuRndrRect2di(s32 x, s32 y, s32 w, s32 h, s32 colour,
                          struct numtl_s *mtl);
extern void edlightDrawSelection(void);
extern void edlightDrawBox(void);
extern void edcamSet(void);
extern void DrawCameraTarget(struct nuvec_s *pos);
extern void NuFntScale(s32 x, s32 y);
extern void eduiMenuRender(void *menu);
extern void NuRndrLine3dDbg(s32 colour, f32 x0, f32 y0, f32 z0, f32 x1, f32 y1,
                            f32 z1);
extern f32 NuVecDist(struct nuvec_s *a, struct nuvec_s *b, struct nuvec_s *d);
extern double fabs(double x);
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

inline void edlightMakeNUCOLOUR3(s32 r, s32 g, s32 b, struct nucolour3_s *col)
{
    col->r = (r & 0xFF) * (1.0f / 255.0f);
    col->g = (g & 0xFF) * (1.0f / 255.0f);
    col->b = (b & 0xFF) * (1.0f / 255.0f);
}

s32 InsideLight(struct nuvec_s *pos, s32 idx)
{
    s32 inside = 0;
    s32 i;
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
    Lights[LIGHTCOUNT].ang.x = 0;
    Lights[LIGHTCOUNT].ang.y = 0;
    Lights[LIGHTCOUNT].ang.z = 0;
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
        Lights[i].ang = Lights[i + 1].ang;
        Lights[i].glbtype = Lights[i + 1].glbtype;
        Lights[i].field_3D = Lights[i + 1].field_3D;
        Lights[i].dbltype = Lights[i + 1].dbltype;
    }
    edlight_cur_light = -1;
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

void edlightHighlightDirection(void)
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
            pos = (struct nuvec_s *)((u8 *)player + 0x6C);
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
