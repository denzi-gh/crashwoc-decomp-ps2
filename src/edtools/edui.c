/*
 * Unit: edtools/edui
 *
 * Functions:
 *   0x001b7f00 eduiInitMaterials
 *   0x001b8158 eduicbRenderSelWithClipColour
 *   0x001b8468 eduicbRenderCheck
 *   0x001b8650 eduiItemTextPickCreate
 *   0x001b87f0 eduiAddTextPickEnt
 *   0x001b8aa8 eduicbProcessTextPick
 *   0x001b8e38 eduicbRenderTextPick
 *   0x001b9108 eduiAddGradPickCFG
 *   0x001b9860 eduiGradStageSetHSV
 *   0x001b9a60 eduiGradStageAddRGB
 *   0x001b9cb8 eduiGradPickRead
 *   0x001b9f20 eduicbProcessGradPick
 *   0x001ba090 eduicbRenderGradPick
 *   0x001ba380 eduiItemGreyPickCreate
 *   0x001ba540 eduicbProcessColourPick
 *   0x001ba910 eduicbRenderColourPick
 *   0x001bbc50 eduicbRenderGreyPick
 *   0x001bbf10 eduicbProcessTexturePick
 *   0x001bc2d0 eduicbRenderTexturePick
 *   0x001bc8b0 eduiItemSliderCreate
 *   0x001bcac8 eduicbProcessSlider
 *   0x001bcfe8 eduicbRenderSlider
 *   0x001bd2c8 eduicbRenderSliderInt
 *   0x001bd538 eduiCreateMessageMenu
 *   0x001bd800 eduiMenuRender
 *   0x001bdba8 eduiMenuProcess
 *   0x001bde10 CreateTestMenu
 *   0x001befa8 eduicbProcessColourSlider
 *   0x001bf548 eduicbRenderColourSlider
 *   0x001bf828 eduiItemSetText
 *   0x001bf8e0 eduiItemSelCreate
 *   0x001bfa68 eduiItemSelWithClipColourCreate
 *   0x001bfc00 eduiItemCheckCreate
 *   0x001bfdb0 eduiItemToggleCreate
 *   0x001bff60 eduiGradStageAdd
 *   0x001c0050 eduiGradStageDelete
 *   0x001c00b8 eduiItemGradPickCreate
 *   0x001c0228 eduiItemGreyGradPickCreate
 *   0x001c03a8 eduiItemColourPickSetHSV
 *   0x001c03d0 eduiItemColourPickSetRGB
 *   0x001c0560 eduiItemColourPickCreate
 *   0x001c06f8 eduiItemTexturePickCreate
 *   0x001c0890 eduiItemSliderSetVal
 *   0x001c08e0 eduiItemSliderSetValEx
 *   0x001c0938 eduiItemSliderSetFmt
 *   0x001c09f0 eduiItemSliderSetGranularity
 *   0x001c09f8 eduiItemSliderCreateInt
 *   0x001c0b00 eduiItemNumberCreate
 *   0x001c0b38 eduiItemColourSliderCreate
 *   0x001c0d80 eduiMenuDestroy
 *   0x001c0de8 eduiMenuCreate
 *   0x001c0ed0 eduiMenuAttach
 *   0x001c0f18 eduiMenuDetach
 *   0x001c0f68 eduiMenuAddItem
 *   0x001c0fa0 eduiMenuHighlight
 *   0x001c1020 eduiMenuFitWidth
 *   0x001c1138 eduiInit
 *   0x001c1158 eduicbProcessGreyPick
 *   0x001c1238 eduicbProcessSel
 *   0x001c12f0 eduicbRenderSel
 *   0x001c1450 eduicbItemSliderDestroy
 *   0x001c14c8 eduicbRenderNumber
 *   0x001c15d0 eduicbItemDestroy
 *   0x001c1638 eduicbItemGradPickDestroy
 *   0x001c16d8 tpentOnEnter
 *   0x001c1780 cbgpcfgAdd
 *   0x001c1888 cbgpcfgDel
 *   0x001c1930 cbgpcfgCopy
 *   0x001c1970 cbgpcfgPaste
 *   0x001c1a48 cbgpcfgCPPress
 *   0x001c1b00 eduicbPickCFGCancel
 *   0x001c1b68 eduicbCancelMessageMenu
 *   0x001c1bd8 eduiItemRender
 *   0x001c1c70 cbSel
 *   0x001c1c80 cbSubMenu
 *   0x001c1cc0 cbGradChange
 *   0x001c1cc8 TestMenu
 */

#include "nu_libc.h"
#include "nu_types.h"

#include "numath/numathtypes.h"
#include "nu3d/numtl.h"
#include "nups2/ps2ctl.h"
#include "edtools/edui.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define EDUI_ITEM_SEL          0
#define EDUI_ITEM_TEXTPICK     1
#define EDUI_ITEM_CHECK        2
#define EDUI_ITEM_TOGGLE       3
#define EDUI_ITEM_SLIDER       6
#define EDUI_ITEM_GRADPICK     7
#define EDUI_ITEM_GREYGRADPICK 8
#define EDUI_ITEM_COLOURPICK   9
#define EDUI_ITEM_GREYPICK     10
#define EDUI_ITEM_TEXTUREPICK  11
#define EDUI_ITEM_COLOURSLIDER 12

extern char *D_004BFE58[4];
extern char *D_004BFE68[4];
extern char *D_006B40D8[4];
extern s32 ui_bgmtl;
extern s32 NuFntHeight;
extern s32 NuFntBase;
extern struct eduimenu_s *edui_messagemenu;
extern struct eduimenu_s *D_00633238;
extern struct eduimenu_s *D_0063324C;
extern char D_0062F910[];
extern char D_0062F968[];
extern char D_0062F980[];
extern char D_0062F988[];
extern char D_0061A868[];
extern char D_0061A878[];
extern char D_0061A7A0[];
extern f32 D_0062F8FC;
extern f32 D_0062F900;
extern f32 D_0062F904;
extern f32 D_0062F908;
extern s32 D_0062F970;
extern s32 D_0062F974;
extern void *D_0062F978;
extern f32 D_0062F960;
extern s32 D_0062F964;
extern f32 D_0062F98C;
extern s32 D_0062F990;
extern s32 D_0063323C;
extern s32 D_00633240;
extern s32 D_00633244;
extern s32 D_00633248;
extern s32 D_00633250;
extern struct eduimenu_s *D_00633254;
extern struct eduimenu_s *D_00633258;
extern struct eduimenu_s *D_0063325C;
extern struct numtl_s *D_006B40C0[6];

#define gpcfg_clip_h    D_0062F8FC
#define gpcfg_clip_s    D_0062F900
#define gpcfg_clip_v    D_0062F904
#define gpcfg_clip_grey D_0062F908
#define edui_up_repeat   D_0062F970
#define edui_textpick_up_repeat    D_0063323C
#define edui_textpick_down_repeat  D_00633240
#define edui_textpick_left_repeat  D_00633244
#define edui_textpick_right_repeat D_00633248
#define edui_down_repeat D_0062F974
#define edui_slider_speed  D_0062F960
#define edui_slider_repeat D_0062F964
#define edui_colourslider_speed  D_0062F98C
#define edui_colourslider_repeat D_0062F990
#define edui_test_sel   D_0062F978
#define edui_test_menu  D_00633254
#define edui_test_submenu0 D_00633258
#define edui_test_submenu1 D_0063325C
#define edui_item_width D_00633250
#define edui_mtl        D_006B40C0
#define edui_srcfile    D_0061A7A0
#define edui_textpick_menu D_00633238
#define edui_gradcfg_menu  D_0063324C
#define edui_emptystr   D_0062F910
#define edui_str_message D_0062F968
#define edui_str_subsel  D_0062F980
#define edui_str_sel     D_0062F988
#define edui_str_gradpickadjust D_0061A868
#define edui_str_colourpick     D_0061A878
#define edui_keyboard_upper D_004BFE58
#define edui_keyboard_lower D_004BFE68
#define edui_keyboard       D_006B40D8

extern s32 NuStrLen(char *str);
extern void NuStrCpy(char *dst, char *src);
extern void NuStrCat(char *dst, char *src);
extern void *NuMemAllocFn(s32 size, char *file, s32 line);
extern void NuMemFreeFn(void *ptr, char *file, s32 line);
extern void NuRndrLineRect2di(s32 x, s32 y, s32 w, s32 h, u32 colour,
                              struct numtl_s *mtl);
extern void NuRndrRect2di(s32 x, s32 y, s32 w, s32 h, u32 colour, struct numtl_s *mtl);
extern void NuRndrGradRect2di(s32 x, s32 y, s32 w, s32 h, u32 *colours,
                              struct numtl_s *mtl);
extern void NuRndrTriStrip2di(s32 *pnts, u32 *colours, s32 count, u32 colour,
                              struct numtl_s *mtl);
extern void NuRndrRectUV2di(s32 x, s32 y, s32 w, s32 h, f32 u0, f32 v0, f32 u1,
                            f32 v1, u32 colour, struct numtl_s *mtl);
extern void NuRndrLine2di(s32 x0, s32 y0, s32 x1, s32 y1, u32 colour,
                          struct numtl_s *mtl);
extern void NuRndrLineStrip2di(s32 *pnts, u32 *colours, s32 count, u32 colour,
                               struct numtl_s *mtl);
extern void NuFntSetPen(s32 pen);
extern void NuFntSet(void *font);
extern s32 NuFntPrintLen(char *text);
extern void NuFntPrintEx(s32 x, s32 y, s32 flags, char *fmt, ...);
s32 eduiMenuProcess(struct eduimenu_s *menu, f32 dt, struct nupad_s *pad);
struct eduiitem_s *eduiItemSliderCreate(void *data, struct eduistyle_s *style, s32 group,
                               void (*change)(struct eduimenu_s *menu, struct eduiitem_s *item,
                                              s32 buttons),
                               f32 min, f32 range, f32 val, char *text);
s32 eduicbProcessSlider(struct eduimenu_s *menu, struct eduiitem_s *item, struct nupad_s *pad);
s32 eduicbRenderSlider(struct eduimenu_s *menu, struct eduiitem_s *item, s32 x, s32 y, s32 w,
                       s32 highlight);
void eduicbItemSliderDestroy(struct eduimenu_s *menu, struct eduiitem_s *item);
s32 eduicbProcessColourSlider(struct eduimenu_s *menu, struct eduiitem_s *item, struct nupad_s *pad);
s32 eduicbRenderColourSlider(struct eduimenu_s *menu, struct eduiitem_s *item, s32 x, s32 y,
                             s32 w, s32 highlight);
void eduiAddGradPickCFG(struct eduimenu_s *menu, struct eduiitem_s *pick);
void eduiGradStageSetHSV(struct eduigradstage_s *stage, f32 h, f32 s, f32 v);
void eduiGradStageDelete(struct eduiitem_s *pick, struct eduigradstage_s *stage);
s32 eduicbRenderNumber(struct eduimenu_s *menu, struct eduiitem_s *item, s32 x, s32 y, s32 w,
                       s32 highlight);
s32 eduicbProcessTextPick(struct eduimenu_s *menu, struct eduiitem_s *item, struct nupad_s *pad, f32 dt);
s32 eduicbRenderTextPick(struct eduimenu_s *menu, struct eduiitem_s *item, s32 x, s32 y, s32 w,
                         s32 highlight);
void eduicbItemDestroy(struct eduimenu_s *menu, struct eduiitem_s *item);
void tpentOnEnter(struct eduimenu_s *menu, struct eduiitem_s *item, u32 paddata);
s32 eduicbProcessSel(struct eduimenu_s *menu, struct eduiitem_s *item, struct nupad_s *pad, f32 dt);
s32 eduicbRenderSel(struct eduimenu_s *menu, struct eduiitem_s *item, s32 x, s32 y, s32 w,
                    s32 highlight);
void eduicbPickCFGCancel(struct eduimenu_s *menu, struct eduimenu_s *parent);
void cbgpcfgAdd(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
void cbgpcfgDel(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
void cbgpcfgCopy(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
void cbgpcfgPaste(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
void cbgpcfgCPPress(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
struct eduiitem_s *eduiItemColourPickCreate(void *data, struct eduistyle_s *style,
                                   void (*change)(struct eduimenu_s *menu,
                                                  struct eduiitem_s *item, u32 buttons),
                                   char *text);
struct eduiitem_s *eduiItemGreyPickCreate(void *data, struct eduistyle_s *style,
                                 void (*change)(struct eduimenu_s *menu, struct eduiitem_s *item,
                                                u32 buttons),
                                 char *text);
void eduiItemColourPickSetHSV(struct eduiitem_s *item, f32 h, f32 s, f32 v);
s32 eduicbProcessColourPick(struct eduimenu_s *menu, struct eduiitem_s *item, struct nupad_s *pad);
s32 eduicbRenderColourPick(struct eduimenu_s *menu, struct eduiitem_s *item, s32 x, s32 y, s32 w,
                           s32 highlight);
s32 eduicbProcessGreyPick(struct eduimenu_s *menu, struct eduiitem_s *item, struct nupad_s *pad);
s32 eduicbRenderGreyPick(struct eduimenu_s *menu, struct eduiitem_s *item, s32 x, s32 y, s32 w,
                         s32 highlight);
s32 eduicbRenderSliderInt(struct eduimenu_s *menu, struct eduiitem_s *item, s32 x, s32 y, s32 w,
                          s32 highlight);
s32 eduicbProcessGradPick(struct eduimenu_s *menu, struct eduiitem_s *item, struct nupad_s *pad);
s32 eduicbRenderGradPick(struct eduimenu_s *menu, struct eduiitem_s *item, s32 x, s32 y, s32 w,
                         s32 highlight);
void eduicbItemGradPickDestroy(struct eduimenu_s *menu, struct eduiitem_s *item);
void eduicbCancelMessageMenu(struct eduimenu_s *menu, struct eduiitem_s *item);
void cbSel(struct eduimenu_s *menu, struct eduiitem_s *item);
void cbSubMenu(struct eduimenu_s *menu, struct eduiitem_s *item);
void cbGradChange(struct eduimenu_s *menu, struct eduiitem_s *item);
s32 eduicbProcessTexturePick(struct eduimenu_s *menu, struct eduiitem_s *item, struct nupad_s *pad);
s32 eduicbRenderTexturePick(struct eduimenu_s *menu, struct eduiitem_s *item, s32 x, s32 y,
                            s32 w, s32 highlight);


void eduiInitMaterials(void) {
    edui_mtl[0] = NuMtlCreate(1);
    edui_mtl[0]->diffuse.r = 0.5f;
    edui_mtl[0]->diffuse.g = 0.5f;
    edui_mtl[0]->diffuse.b = 0.5f;
    edui_mtl[0]->attrib.cull = 2;
    edui_mtl[0]->attrib.zmode = 3;
    edui_mtl[0]->alpha = 1.0f;
    edui_mtl[0]->attrib.alpha = 0;
    NuMtlUpdate(edui_mtl[0]);

    edui_mtl[1] = NuMtlCreate(1);
    edui_mtl[1]->diffuse.r = 0.2f;
    edui_mtl[1]->diffuse.g = 0.2f;
    edui_mtl[1]->diffuse.b = 0.2f;
    edui_mtl[1]->attrib.cull = 2;
    edui_mtl[1]->attrib.zmode = 3;
    edui_mtl[1]->alpha = 0.999f;
    edui_mtl[1]->attrib.alpha = 3;
    NuMtlUpdate(edui_mtl[1]);

    edui_mtl[2] = NuMtlCreate(1);
    edui_mtl[2]->diffuse.r = 0.2f;
    edui_mtl[2]->diffuse.g = 0.2f;
    edui_mtl[2]->diffuse.b = 0.2f;
    edui_mtl[2]->attrib.cull = 2;
    edui_mtl[2]->attrib.zmode = 3;
    edui_mtl[2]->alpha = 0.999f;
    edui_mtl[2]->attrib.alpha = 2;
    NuMtlUpdate(edui_mtl[2]);

    edui_mtl[3] = NuMtlCreate(1);
    edui_mtl[3]->diffuse.r = 0.2f;
    edui_mtl[3]->diffuse.g = 0.2f;
    edui_mtl[3]->diffuse.b = 0.2f;
    edui_mtl[3]->attrib.cull = 2;
    edui_mtl[3]->attrib.zmode = 3;
    edui_mtl[3]->alpha = 0.999f;
    edui_mtl[3]->attrib.alpha = 1;
    NuMtlUpdate(edui_mtl[3]);
}


static inline void eduiHSVToRGB(f32 h, f32 s, f32 v, f32 *r, f32 *g, f32 *b) {
    s32 sector;
    f32 f;
    f32 p;
    f32 q;
    f32 t;

    if (s == 0.0f) {
        *r = v;
        *g = v;
        *b = v;
        return;
    }
    if (h == 360.0f) {
        h = 0.0f;
    }
    h /= 60.0f;
    sector = floor(h);
    f = h - sector;
    p = v * (1.0f - s);
    q = v * (1.0f - s * f);
    t = v * (1.0f - s * (1.0f - f));
    switch (sector) {
    case 0:
        *r = v;
        *g = t;
        *b = p;
        break;
    case 1:
        *r = q;
        *g = v;
        *b = p;
        break;
    case 2:
        *r = p;
        *g = v;
        *b = t;
        break;
    case 3:
        *r = p;
        *g = q;
        *b = v;
        break;
    case 4:
        *r = t;
        *g = p;
        *b = v;
        break;
    case 5:
        *r = v;
        *g = p;
        *b = q;
        break;
    }
}


s32 eduicbRenderSelWithClipColour(struct eduimenu_s *menu, struct eduiitem_s *item, s32 x, s32 y,
                                  s32 w, s32 highlight) {
    s32 height;
    s32 texty;
    u32 colours[4];
    f32 r;
    f32 g;
    f32 b;
    u32 colour;

    height = (NuFntHeight + (NuFntHeight >> 2)) >> 3;
    item->x = x;
    item->y = y;
    texty = ((height * 8 - NuFntHeight) >> 1) + NuFntBase;
    NuRndrRect2di(x * 16, y * 8, w * 16, height * 8, item->style.colour[item->on],
                  edui_mtl[ui_bgmtl]);
    NuFntSetPen(item->style.pen[item->on]);
    NuFntPrintEx((x * 2 + w) * 8, y * 8 + texty, 2, item->text);
    if (item->type == EDUI_ITEM_GRADPICK) {
        eduiHSVToRGB(gpcfg_clip_h, gpcfg_clip_s, gpcfg_clip_v, &r, &g, &b);
    } else {
        r = gpcfg_clip_grey;
        g = gpcfg_clip_grey;
        b = gpcfg_clip_grey;
    }
    colour = 0x80000000 + ((s32)(b * 255.0f) << 16) +
             ((s32)(g * 255.0f) << 8) + (s32)(r * 255.0f);
    colours[2] = colour;
    colours[0] = colour;
    colours[3] = colour;
    colours[1] = colour;
    NuRndrGradRect2di((x + w - 45) * 16, y * 8, 640, height * 8, colours,
                      edui_mtl[ui_bgmtl]);
    return height;
}


s32 eduicbRenderCheck(struct eduimenu_s *menu, struct eduiitem_s *item, s32 x, s32 y, s32 w,
                      s32 highlight) {
    s32 height;
    s32 texty;
    s32 left;
    s32 top;
    s32 right;
    s32 bottom;
    s32 tick[10];
    s32 box[10];

    height = (NuFntHeight + (NuFntHeight >> 2)) >> 3;
    item->x = x;
    item->y = y;
    texty = ((height * 8 - NuFntHeight) >> 1) + NuFntBase;
    NuRndrRect2di(x * 16, y * 8, w * 16, height * 8, item->style.colour[0],
                  edui_mtl[ui_bgmtl]);
    NuFntSetPen(item->style.pen[0]);
    NuFntPrintEx(x * 16, y * 8 + texty, 0, item->text);
    left = (x + w - height) * 16 - 16;
    top = (y + 1) * 8 + 8;
    right = left + (height - 2) * 16 - 16;
    bottom = top + (height - 2) * 8 - 8;
    if (item->on) {
        tick[0] = left;
        tick[1] = (top + bottom) >> 1;
        tick[2] = (left + ((left + right) >> 1)) >> 1;
        tick[3] = (((top + bottom) >> 1) + bottom) >> 1;
        tick[4] = (left + ((left + right) >> 1)) >> 1;
        tick[5] = bottom;
        tick[6] = right;
        tick[7] = top;
        NuRndrTriStrip2di(tick, 0, 4, item->style.pen[0], edui_mtl[0]);
    } else {
        box[0] = left;
        box[1] = (top + bottom) >> 1;
        box[2] = (left + ((left + right) >> 1)) >> 1;
        box[3] = (((top + bottom) >> 1) + bottom) >> 1;
        box[4] = right;
        box[5] = top;
        box[6] = (left + ((left + right) >> 1)) >> 1;
        box[7] = bottom;
        box[8] = left;
        box[9] = (top + bottom) >> 1;
        NuRndrLineStrip2di(box, 0, 5, item->style.pen[0], edui_mtl[0]);
    }
    return height;
}


inline struct eduimenu_s *eduiMenuCreate(s32 x, s32 y, s32 w, s32 h, void *font,
                         void (*cancel)(struct eduimenu_s *menu, struct eduimenu_s *parent),
                         char *title) {
    struct eduimenu_s *menu;
    s32 size;

    size = sizeof(struct eduimenu_s);
    if (title) {
        size = NuStrLen(title) + sizeof(struct eduimenu_s) + 1;
    }
    menu = NuMemAllocFn(size, edui_srcfile, 0x1A1);
    if (menu) {
        memset(menu, 0, size);
        menu->x = x;
        menu->y = y;
        menu->w = w;
        menu->h = h;
        menu->font = font;
        menu->cancel = cancel;
        menu->title = menu->titlebuf;
        if (title) {
            NuStrCpy(menu->title, title);
        }
    }
    return menu;
}


inline s32 eduiMenuAttach(struct eduimenu_s *menu, struct eduimenu_s *child) {
    struct eduimenu_s *parent;

    if (!child->parent) {
        parent = menu;
        while (parent->child) {
            parent = parent->child;
        }
        child->parent = parent;
        parent->child = child;
        return 1;
    }
    return 0;
}


inline struct eduiitem_s *eduiMenuAddItem(struct eduimenu_s *menu, struct eduiitem_s *item) {
    if (menu->tail) {
        menu->tail->next = item;
    } else {
        menu->head = item;
    }
    item->prev = menu->tail;
    item->next = 0;
    menu->tail = item;
    menu->cursor = menu->head;
    edui_last_item = item;
    return item;
}


inline void eduiMenuFitWidth(struct eduimenu_s *menu, s32 margin) {
    struct eduiitem_s *item;
    s32 width;
    s32 w;
    s32 height;

    width = 0;
    item = menu->head;
    NuFntSet(menu->font);
    if (menu->title) {
        w = NuFntPrintLen(menu->title) >> 4;
        if (width < w) {
            width = w;
        }
    }
    for (; item; item = item->next) {
        switch (item->type) {
        case EDUI_ITEM_TEXTPICK:
            height = (NuFntHeight + (NuFntHeight >> 2)) >> 3;
            w = height * 10;
            break;
        case EDUI_ITEM_CHECK:
        case EDUI_ITEM_TOGGLE:
            if (item->text) {
                height = (NuFntHeight + (NuFntHeight >> 2)) >> 3;
                w = (NuFntPrintLen(item->text) >> 4) + height;
            }
            break;
        default:
            if (item->text) {
                w = NuFntPrintLen(item->text) >> 4;
            }
            break;
        }
        if (width < w) {
            width = w;
        }
    }
    width += margin;
    menu->w = width;
}


inline s32 eduiItemSetText(struct eduiitem_s *item, char *text) {
    if (item->text) {
        if (NuStrLen(item->text) < NuStrLen(text)) {
            NuMemFreeFn(item->text, edui_srcfile, 0x24F);
            item->text = 0;
        }
    }
    if (!item->text) {
        item->text = NuMemAllocFn(NuStrLen(text) + 1, edui_srcfile, 0x254);
        if (!item->text) {
            return 0;
        }
    }
    NuStrCpy(item->text, text);
    return 1;
}


inline struct eduiitem_s *eduiItemSelCreate(void *data, struct eduistyle_s *style, s32 on,
                                   s32 group,
                                   void (*select)(struct eduimenu_s *menu,
                                                  struct eduiitem_s *item, u32 buttons),
                                   char *text) {
    struct eduiitem_s *item;

    item = NuMemAllocFn(0x48, edui_srcfile, 0x269);
    if (item) {
        memset(item, 0, 0x48);
        item->type = EDUI_ITEM_SEL;
        item->data = data;
        item->style = *style;
        item->align = 2;
        item->group = group;
        item->on = on;
        item->process = eduicbProcessSel;
        item->render = eduicbRenderSel;
        item->destroy = eduicbItemDestroy;
        eduiItemSetText(item, text);
        item->u.sel.select = select;
        return item;
    }
    return 0;
}


struct eduiitem_s *eduiItemSelWithClipColourCreate(void *data, struct eduistyle_s *style, s32 on,
                                          s32 group,
                                          void (*select)(struct eduimenu_s *menu,
                                                         struct eduiitem_s *item,
                                                         u32 buttons),
                                          char *text) {
    struct eduiitem_s *item;

    item = eduiItemSelCreate(data, style, on, group, select, text);
    if (item) {
        item->render = eduicbRenderSelWithClipColour;
    }
    return item;
}


struct eduiitem_s *eduiItemCheckCreate(void *data, struct eduistyle_s *style, s32 on, s32 group,
                              void (*select)(struct eduimenu_s *menu, struct eduiitem_s *item,
                                             u32 buttons),
                              char *text) {
    struct eduiitem_s *item;

    item = eduiItemSelCreate(data, style, on, group, select, text);
    if (item) {
        item->type = EDUI_ITEM_CHECK;
        item->process = eduicbProcessSel;
        item->render = eduicbRenderCheck;
    }
    return item;
}


struct eduiitem_s *eduiItemToggleCreate(void *data, struct eduistyle_s *style, s32 on, s32 group,
                               void (*select)(struct eduimenu_s *menu, struct eduiitem_s *item,
                                              u32 buttons),
                               char *text) {
    struct eduiitem_s *item;

    item = eduiItemSelCreate(data, style, on, group, select, text);
    if (item) {
        item->type = EDUI_ITEM_TOGGLE;
        item->process = eduicbProcessSel;
        item->render = eduicbRenderCheck;
    }
    return item;
}


struct eduiitem_s *eduiItemTextPickCreate(void *data, struct eduistyle_s *style,
                                 void (*enter)(struct eduimenu_s *menu, struct eduiitem_s *item,
                                               u32 paddata),
                                 char *text) {
    struct eduiitem_s *item;

    item = NuMemAllocFn(sizeof(struct eduiitem_s), edui_srcfile, 0x355);
    if (item) {
        memset(item, 0, sizeof(struct eduiitem_s));
        item->type = EDUI_ITEM_TEXTPICK;
        item->data = data;
        item->style = *style;
        item->align = 2;
        item->group = 0;
        item->process = eduicbProcessTextPick;
        item->render = eduicbRenderTextPick;
        item->destroy = eduicbItemDestroy;
        eduiItemSetText(item, text);
        item->u.textpick.enter = enter;
        edui_keyboard[0] = edui_keyboard_upper[0];
        edui_keyboard[1] = edui_keyboard_upper[1];
        edui_keyboard[2] = edui_keyboard_upper[2];
        edui_keyboard[3] = edui_keyboard_upper[3];
        return item;
    }
    return 0;
}


void eduiAddTextPickEnt(struct eduimenu_s *menu, struct eduiitem_s *pick) {
    struct eduistyle_s style = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};
    struct eduiitem_s *item;
    struct eduiitem_s *owner;

    owner = pick;

    edui_textpick_menu = eduiMenuCreate(owner->x + 10, owner->y + 10, 180, 50,
                                        menu->font, 0, "Enter Text");
    if (edui_textpick_menu) {
        item = eduiItemTextPickCreate(pick, &style, tpentOnEnter, edui_emptystr);
        eduiMenuAddItem(edui_textpick_menu, item);
        eduiMenuAttach(menu, edui_textpick_menu);
        item->style = owner->style;
        NuStrCpy(item->u.textpick.text, pick->u.textpick.text);
        item->u.textpick.active = 1;
        eduiMenuFitWidth(edui_textpick_menu, 8);
    }
}


s32 eduicbProcessTextPick(struct eduimenu_s *menu, struct eduiitem_s *item, struct nupad_s *pad, f32 dt) {
    s32 up;
    s32 down;
    s32 left;
    s32 right;

    if (item->u.textpick.active) {
        right = 0;
        left = 0;
        down = 0;
        up = 0;
        if (pad->paddata_db & 0x1000) {
            edui_textpick_up_repeat = 20;
            up = 1;
        }
        if ((pad->paddata & 0x1000) && edui_textpick_up_repeat) {
            if (--edui_textpick_up_repeat == 0) {
                edui_textpick_up_repeat = 10;
                up = 1;
            }
        }
        if (pad->paddata_db & 0x4000) {
            edui_textpick_down_repeat = 20;
            down = 1;
        }
        if ((pad->paddata & 0x4000) && edui_textpick_down_repeat) {
            if (--edui_textpick_down_repeat == 0) {
                edui_textpick_down_repeat = 10;
                down = 1;
            }
        }
        if (pad->paddata_db & 0x8000) {
            edui_textpick_left_repeat = 20;
            left = 1;
        }
        if ((pad->paddata & 0x8000) && edui_textpick_left_repeat) {
            if (--edui_textpick_left_repeat == 0) {
                edui_textpick_left_repeat = 10;
                left = 1;
            }
        }
        if (pad->paddata_db & 0x2000) {
            edui_textpick_right_repeat = 20;
            right = 1;
        }
        if ((pad->paddata & 0x2000) && edui_textpick_right_repeat) {
            if (--edui_textpick_right_repeat == 0) {
                edui_textpick_right_repeat = 10;
                right = 1;
            }
        }
        if (down) {
            if (++item->u.textpick.row >= 4) {
                item->u.textpick.row = 0;
            }
        }
        if (up) {
            if (--item->u.textpick.row < 0) {
                item->u.textpick.row = 3;
            }
        }
        if (left) {
            if (--item->u.textpick.col < 0) {
                item->u.textpick.col =
                    NuStrLen(edui_keyboard[item->u.textpick.row]) - 1;
            }
        }
        if (right) {
            item->u.textpick.col++;
            if (item->u.textpick.col >=
                strlen(edui_keyboard[item->u.textpick.row])) {
                item->u.textpick.col = 0;
            }
        }
        if (pad->paddata_db & 0x40) {
            char key[2] = " ";

            key[0] = edui_keyboard[item->u.textpick.row][item->u.textpick.col];
            NuStrCat(item->u.textpick.text, key);
        }
        if (pad->paddata_db & 0x80) {
            if (item->u.textpick.text[0]) {
                item->u.textpick.text[NuStrLen(item->u.textpick.text) - 1] = 0;
            }
        }
        if (pad->paddata_db & 0x10) {
            if (item->u.textpick.enter) {
                item->u.textpick.enter(menu, item, pad->paddata);
            }
        }
        if (pad->paddata_db & 0x20) {
            if (edui_keyboard[1] == edui_keyboard_upper[1]) {
                edui_keyboard[0] = edui_keyboard_lower[0];
                edui_keyboard[1] = edui_keyboard_lower[1];
                edui_keyboard[2] = edui_keyboard_lower[2];
                edui_keyboard[3] = edui_keyboard_lower[3];
            } else {
                edui_keyboard[0] = edui_keyboard_upper[0];
                edui_keyboard[1] = edui_keyboard_upper[1];
                edui_keyboard[2] = edui_keyboard_upper[2];
                edui_keyboard[3] = edui_keyboard_upper[3];
            }
        }
        return 1;
    }
    if (pad->paddata_db & 0x40) {
        eduiAddTextPickEnt(menu, item);
    }
    if (pad->paddata_db & 0x10) {
        if (item->u.textpick.enter) {
            item->u.textpick.enter(menu, item, pad->paddata);
        }
    }
    return 0;
}


s32 eduicbRenderTextPick(struct eduimenu_s *menu, struct eduiitem_s *item, s32 x, s32 y, s32 w,
                         s32 highlight) {
    s32 height;
    s32 texty;
    s32 row;
    s32 col;
    s32 py;
    char key[2] = "0";

    height = (NuFntHeight + (NuFntHeight >> 2)) >> 3;
    texty = ((height * 8 - NuFntHeight) >> 1) + NuFntBase;
    item->x = x;
    item->y = y;
    if (item->u.textpick.active) {
        w = (height * 10 < w) ? w : height * 10;
        edui_item_width = w;
        NuRndrRect2di(x * 16, y * 8, w * 16, (height * 5) << 3,
                      item->style.colour[0], edui_mtl[ui_bgmtl]);
        for (row = 0; row < 4; row++) {
            py = (y + height * row) * 8;
            for (col = 0; col < 10; col++) {
                if (row == item->u.textpick.row && col == item->u.textpick.col) {
                    NuFntSetPen(item->style.pen[1]);
                } else {
                    NuFntSetPen(item->style.pen[0]);
                }
                key[0] = edui_keyboard[row][col];
                NuFntPrintEx((x + height * col) * 16, py + texty, 0, key);
            }
        }
        NuFntSetPen(item->style.pen[1]);
        NuFntPrintEx(x * 16, (y + height * row) * 8 + texty, 0,
                     item->u.textpick.text);
        return height * 5;
    }
    NuRndrRect2di(x * 16, y * 8, w * 16, height * 16,
                  item->style.colour[item->on], edui_mtl[ui_bgmtl]);
    NuFntSetPen(item->style.pen[item->on]);
    NuFntPrintEx((x * 2 + w) * 8, y * 8 + texty, 2, item->text);
    NuFntPrintEx((x * 2 + w) * 8, (y + height) * 8 + texty, 2, "\"%s\"",
                 item->u.textpick.text);
    return height * 2;
}


void eduiAddGradPickCFG(struct eduimenu_s *menu, struct eduiitem_s *pick) {
    struct eduistyle_s style = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};
    struct eduiitem_s *item;
    struct eduiitem_s *paste;

    edui_gradcfg_menu = eduiMenuCreate(pick->x + 10, pick->y + 10, 180, 250,
                                       menu->font, eduicbPickCFGCancel,
                                       edui_str_gradpickadjust);
    if (edui_gradcfg_menu) {
        switch (pick->type) {
        case EDUI_ITEM_GRADPICK:
        default:
            item = eduiItemColourPickCreate(pick, &style, cbgpcfgCPPress,
                                            edui_str_colourpick);
            break;
        case EDUI_ITEM_GREYGRADPICK:
            item = eduiItemGreyPickCreate(pick, &style, cbgpcfgCPPress,
                                          edui_str_colourpick);
            break;
        }
        if (pick->u.grad.stage) {
            eduiItemColourPickSetHSV(item, pick->u.grad.stage->h,
                                     pick->u.grad.stage->s,
                                     pick->u.grad.stage->v);
        }
        eduiMenuAddItem(edui_gradcfg_menu, item);
        eduiMenuAddItem(edui_gradcfg_menu,
                        eduiItemSelCreate(pick, &style, 0, 0, cbgpcfgAdd,
                                          "Add"));
        eduiMenuAddItem(edui_gradcfg_menu,
                        eduiItemSelCreate(pick, &style, 0, 0, cbgpcfgDel,
                                          "Del"));
        eduiMenuAddItem(edui_gradcfg_menu,
                        eduiItemSelCreate(pick, &style, 0, 0, cbgpcfgCopy,
                                          "Copy"));
        edui_last_item->type = pick->type;
        paste = eduiItemSelCreate(pick, &style, 0, 0, cbgpcfgPaste, "Paste");
        if (paste) {
            paste->render = eduicbRenderSelWithClipColour;
        }
        eduiMenuAddItem(edui_gradcfg_menu, paste);
        edui_last_item->type = pick->type;
        eduiMenuAttach(menu, edui_gradcfg_menu);
    }
}


void eduiGradStageSetHSV(struct eduigradstage_s *stage, f32 h, f32 s, f32 v) {
    f32 r;
    f32 g;
    f32 b;

    eduiHSVToRGB(h, s, v, &r, &g, &b);
    r *= 255.0f;
    g *= 255.0f;
    b *= 255.0f;
    stage->v = v;
    stage->h = h;
    stage->s = s;
    stage->colour = 0x80000000 | ((s32)b << 16) | ((s32)g << 8) | (s32)r;
}


static inline void eduiRGBToHSV(f32 r, f32 g, f32 b, f32 *h, f32 *s, f32 *v) {
    f32 max;
    f32 min;
    f32 delta;

    max = (r > ((g > b) ? g : b)) ? r : ((g > b) ? g : b);
    min = (r < ((g < b) ? g : b)) ? r : ((g < b) ? g : b);
    *v = max;
    *s = (max != 0.0f) ? (max - min) / max : 0.0f;
    if (*s == 0.0f) {
        *h = 0.0f;
        return;
    }
    delta = max - min;
    if (r == max) {
        *h = (g - b) / delta;
    } else if (g == max) {
        *h = 2.0f + (b - r) / delta;
    } else if (b == max) {
        *h = 4.0f + (r - g) / delta;
    }
    *h *= 60.0f;
    if (*h < 0.0f) {
        *h += 360.0f;
    }
}


static inline struct eduigradstage_s *eduiGradStageInsert(struct eduiitem_s *pick, f32 pos, f32 h,
                                                 f32 s, f32 v) {
    struct eduigradstage_s *stage;
    struct eduigradstage_s *prev;
    struct eduigradstage_s *next;

    stage = NuMemAllocFn(sizeof(struct eduigradstage_s), edui_srcfile, 0x521);
    if (stage) {
        memset(stage, 0, sizeof(struct eduigradstage_s));
        prev = 0;
        for (next = pick->u.grad.first; next; next = next->next) {
            if (pos < next->pos) {
                break;
            }
            prev = next;
        }
        if (prev) {
            prev->next = stage;
        } else {
            pick->u.grad.first = stage;
        }
        if (next) {
            next->prev = stage;
        }
        stage->pos = pos;
        stage->prev = prev;
        stage->next = next;
        eduiGradStageSetHSV(stage, h, s, v);
        pick->u.grad.stage = stage;
    }
    return stage;
}


struct eduigradstage_s *eduiGradStageAddRGB(struct eduiitem_s *pick, f32 pos, f32 r, f32 g,
                                   f32 b) {
    f32 h;
    f32 s;
    f32 v;

    eduiRGBToHSV(r, g, b, &h, &s, &v);
    return eduiGradStageInsert(pick, pos, h, s, v);
}


s32 eduiGradPickRead(struct eduiitem_s *pick, struct eduigradread_s *buf, s32 max) {
    struct eduigradstage_s *stage;
    s32 count;
    f32 r;
    f32 g;
    f32 b;

    count = 0;
    for (stage = pick->u.grad.first; stage; stage = stage->next) {
        if (count < max) {
            buf[count].pos = stage->pos;
            buf[count].h = stage->h;
            buf[count].s = stage->s;
            buf[count].v = stage->v;
            eduiHSVToRGB(stage->h, stage->s, stage->v, &r, &g, &b);
            buf[count].r = r;
            buf[count].g = g;
            buf[count].b = b;
            buf[count].colour = 0x80000000 + ((s32)(b * 255.0f) << 16) +
                                ((s32)(g * 255.0f) << 8) + (s32)(r * 255.0f);
        }
        count++;
    }
    return count;
}


s32 eduicbProcessGradPick(struct eduimenu_s *menu, struct eduiitem_s *item, struct nupad_s *pad) {
    struct eduigradstage_s *stage;
    struct eduigradstage_s *step;
    f32 min;
    f32 max;
    f32 pos;

    if (item->u.grad.change) {
        item->u.grad.change(menu, item, pad->paddata);
    }
    if (pad->paddata_db & 0x40) {
        eduiAddGradPickCFG(menu, item);
        return 0;
    }
    if (!item->u.grad.stage) {
        item->u.grad.stage = item->u.grad.first;
        if (!item->u.grad.stage) {
            return 0;
        }
    }
    if (pad->paddata_db & 0x8000) {
        step = item->u.grad.stage->prev;
    change:
        if (step) {
            item->u.grad.stage = step;
        }
        return 0;
    }
    if (pad->paddata_db & 0x2000) {
        step = item->u.grad.stage->next;
        goto change;
    }
    stage = item->u.grad.stage;
    min = 0.0f;
    max = 1.0f;
    if (stage->prev) {
        min = stage->prev->pos;
    }
    if (stage->next) {
        max = stage->next->pos;
    }
    if (pad->rdata.l2_alg) {
        pos = stage->pos - pad->rdata.l2_alg / 15360.0f;
        if (pos < min) {
            pos = min;
        }
    } else if (pad->rdata.r2_alg) {
        pos = stage->pos + pad->rdata.r2_alg / 15360.0f;
        if (pos > max) {
            pos = max;
        }
    } else {
        goto done;
    }
    stage->pos = pos;
    item->u.grad.flash = 60;
    return 0;
done:
    return 0;
}


/* state=asm: retail keeps x * 16 and (f32)(w * 16) inside the gradient loop
 * (only height >> 1, y + (height >> 1) and &edui_mtl are hoisted); every loop
 * form tried here hoists them or nothing at all. */
s32 eduicbRenderGradPick(struct eduimenu_s *menu, struct eduiitem_s *item, s32 x, s32 y, s32 w,
                         s32 highlight) {
    s32 height;
    s32 texty;
    s32 px;
    struct eduigradstage_s *stage;
    f32 pos;
    u32 colour;
    u32 colours[4];

    height = (NuFntHeight + (NuFntHeight >> 2)) >> 2;
    item->x = x;
    item->y = y;
    texty = ((height * 4 - NuFntHeight) >> 1) + NuFntBase;
    NuRndrRect2di(x * 16, y * 8, w * 16, height * 8, item->style.colour[item->on],
                  edui_mtl[ui_bgmtl]);
    NuFntSetPen(item->style.pen[item->on]);
    if (item->u.grad.flash && item->u.grad.stage) {
        NuFntPrintEx((x * 2 + w) * 8, y * 8 + texty, 2, "%1.03f",
                     item->u.grad.stage->pos);
        item->u.grad.flash--;
    } else {
        NuFntPrintEx((x * 2 + w) * 8, y * 8 + texty, 2, item->text);
    }
    stage = item->u.grad.first;
    if (stage) {
        pos = stage->pos;
        colour = stage->colour;
        for (stage = stage->next; stage; stage = stage->next) {
            colours[0] = colours[2] = colour;
            colours[1] = colours[3] = stage->colour;
            NuRndrGradRect2di(x * 16 + (s32)(w * 16 * pos),
                              (y + (height >> 1)) * 8,
                              (s32)(w * 16 * stage->pos) -
                                  (s32)(w * 16 * pos),
                              (height >> 1) * 8, colours, edui_mtl[ui_bgmtl]);
            pos = stage->pos;
            colour = stage->colour;
        }
    }
    for (stage = item->u.grad.first; stage; stage = stage->next) {
        px = ((s32)(stage->pos * (f32)w) + x - 2) * 16;
        NuRndrRect2di(px, (y + (height >> 1) - 2) * 8, 80,
                      ((height >> 1) + 2) * 8, stage->colour, edui_mtl[0]);
        if (stage == item->u.grad.stage) {
            NuRndrLineRect2di(px, (y + (height >> 1) - 2) * 8, 80,
                              ((height >> 1) + 2) * 8, 0x80FFFFFF,
                              edui_mtl[0]);
        } else {
            NuRndrLineRect2di(px, (y + (height >> 1) - 2) * 8, 80,
                              ((height >> 1) + 2) * 8, 0x80000000,
                              edui_mtl[0]);
        }
    }
    return height;
}


struct eduigradstage_s *eduiGradStageAdd(struct eduiitem_s *pick, f32 pos, f32 h, f32 s, f32 v) {
    return eduiGradStageInsert(pick, pos, h, s, v);
}


inline struct eduiitem_s *eduiItemGradPickCreate(void *data, struct eduistyle_s *style,
                                        void (*change)(struct eduimenu_s *menu,
                                                       struct eduiitem_s *item,
                                                       u32 paddata),
                                        char *text) {
    struct eduiitem_s *item;

    item = NuMemAllocFn(0x54, edui_srcfile, 0x597);
    if (item) {
        memset(item, 0, 0x54);
        item->type = EDUI_ITEM_GRADPICK;
        item->data = data;
        item->style = *style;
        item->align = 2;
        item->group = 0;
        item->process = eduicbProcessGradPick;
        item->render = eduicbRenderGradPick;
        item->destroy = eduicbItemGradPickDestroy;
        eduiItemSetText(item, text);
        item->u.grad.change = change;
        item->u.grad.flash = 0;
        return item;
    }
    return 0;
}


inline struct eduiitem_s *eduiItemGreyGradPickCreate(void *data, struct eduistyle_s *style,
                                     void (*change)(struct eduimenu_s *menu,
                                                    struct eduiitem_s *item,
                                                    u32 paddata),
                                     char *text) {
    struct eduiitem_s *item;

    item = eduiItemGradPickCreate(data, style, change, text);
    if (item) {
        item->type = EDUI_ITEM_GREYGRADPICK;
    }
    return item;
}


void eduiItemColourPickSetHSV(struct eduiitem_s *item, f32 h, f32 s, f32 v) {
    item->u.colour.h = h;
    item->u.colour.s = s;
    item->u.colour.v = v;
    item->u.colour.hue = h / 360.0f;
    item->u.colour.brightness = v;
}


inline struct eduiitem_s *eduiItemColourPickCreate(void *data, struct eduistyle_s *style,
                                          void (*change)(struct eduimenu_s *menu,
                                                         struct eduiitem_s *item,
                                                         u32 buttons),
                                          char *text) {
    struct eduiitem_s *item;

    item = NuMemAllocFn(0x68, edui_srcfile, 0x66A);
    if (item) {
        memset(item, 0, 0x68);
        item->type = EDUI_ITEM_COLOURPICK;
        item->data = data;
        item->style = *style;
        item->align = 2;
        item->group = 0;
        item->process = eduicbProcessColourPick;
        item->render = eduicbRenderColourPick;
        item->destroy = eduicbItemDestroy;
        eduiItemSetText(item, text);
        item->u.colour.change = change;
        item->u.colour.hue = 0.5f;
        item->u.colour.brightness = 0.5f;
        item->u.colour.h = 180.0f;
        item->u.colour.v = 0.5f;
        item->u.colour.s = 1.0f;
        return item;
    }
    return 0;
}


struct eduiitem_s *eduiItemGreyPickCreate(void *data, struct eduistyle_s *style,
                                 void (*change)(struct eduimenu_s *menu, struct eduiitem_s *item,
                                                u32 buttons),
                                 char *text) {
    struct eduiitem_s *item;

    item = eduiItemColourPickCreate(data, style, change, text);
    if (item) {
        item->type = EDUI_ITEM_GREYPICK;
        item->process = eduicbProcessGreyPick;
        item->render = eduicbRenderGreyPick;
    }
    return item;
}


struct eduiitem_s *eduiItemTexturePickCreate(void *data, struct eduistyle_s *style,
                                    void (*change)(struct eduimenu_s *menu,
                                                   struct eduiitem_s *item,
                                                   u32 buttons),
                                    char *text) {
    struct eduiitem_s *item;

    item = NuMemAllocFn(0x64, edui_srcfile, 0x75A);
    if (item) {
        memset(item, 0, 0x64);
        item->type = EDUI_ITEM_TEXTUREPICK;
        item->data = data;
        item->style = *style;
        item->align = 2;
        item->group = 0;
        item->process = eduicbProcessTexturePick;
        item->render = eduicbRenderTexturePick;
        item->destroy = eduicbItemDestroy;
        eduiItemSetText(item, text);
        item->u.texpick.change = change;
        item->u.texpick.u[0] = 0.4f;
        item->u.texpick.u[1] = 0.5f;
        item->u.texpick.v[0] = 0.4f;
        item->u.texpick.v[1] = 0.5f;
        item->u.texpick.corner = 0;
        item->u.texpick.zoom = 1.0f;
        return item;
    }
    return 0;
}


void eduiItemColourPickSetRGB(struct eduiitem_s *item, f32 r, f32 g, f32 b) {
    eduiRGBToHSV(r, g, b, &item->u.colour.h, &item->u.colour.s,
                 &item->u.colour.v);
    item->u.colour.hue = item->u.colour.h / 360.0f;
    item->u.colour.brightness = item->u.colour.v;
}


void eduiItemSliderSetVal(struct eduiitem_s *item, f32 val) {
    item->u.slider.flash = 60;
    item->u.slider.val = val;
    item->u.slider.frac = (val - item->u.slider.min) / item->u.slider.range;
    if (item->u.slider.change) {
        item->u.slider.change(0, item, 0);
    }
}


inline void eduiItemSliderSetValEx(struct eduiitem_s *item, f32 val, s32 flash,
                                  s32 notify) {
    item->u.slider.frac = (val - item->u.slider.min) / item->u.slider.range;
    item->u.slider.val = val;
    if (flash) {
        item->u.slider.flash = 60;
    }
    if (notify && item->u.slider.change) {
        item->u.slider.change(0, item, 0);
    }
}


inline void eduiItemSliderSetFmt(struct eduiitem_s *item, char *fmt) {
    if (item->u.slider.fmt) {
        if (NuStrLen(item->u.slider.fmt) < NuStrLen(fmt)) {
            NuMemFreeFn(item->u.slider.fmt, edui_srcfile, 0x833);
            item->u.slider.fmt = NuMemAllocFn(NuStrLen(fmt) + 1, edui_srcfile,
                                              0x834);
        }
    } else {
        item->u.slider.fmt = NuMemAllocFn(NuStrLen(fmt) + 1, edui_srcfile,
                                          0x838);
    }
    NuStrCpy(item->u.slider.fmt, fmt);
}


struct eduiitem_s *eduiItemSliderCreate(void *data, struct eduistyle_s *style, s32 group,
                               void (*change)(struct eduimenu_s *menu, struct eduiitem_s *item,
                                              s32 buttons),
                               f32 min, f32 range, f32 val, char *text) {
    struct eduiitem_s *item;

    item = NuMemAllocFn(0x64, edui_srcfile, 0x85C);
    if (item) {
        memset(item, 0, 0x64);
        item->type = EDUI_ITEM_SLIDER;
        item->data = data;
        item->style = *style;
        item->align = 2;
        item->group = group;
        item->process = eduicbProcessSlider;
        item->render = eduicbRenderSlider;
        item->destroy = eduicbItemSliderDestroy;
        eduiItemSetText(item, text);
        item->u.slider.change = change;
        item->u.slider.min = min;
        item->u.slider.range = range;
        item->u.slider.fmt = 0;
        item->u.slider.granularity = 0.001f;
        eduiItemSliderSetFmt(item, "(%1.03f)");
        eduiItemSliderSetValEx(item, val, 1, 1);
        return item;
    }
    return 0;
}


void eduiItemSliderSetGranularity(struct eduiitem_s *item, f32 granularity) {
    item->u.slider.granularity = granularity;
}


struct eduiitem_s *eduiItemSliderCreateInt(void *data, void *style, s32 group, void *cb,
                                  s32 val, s32 min, s32 max, char *text) {
    struct eduiitem_s *item;

    item = eduiItemSliderCreate(data, style, group, cb, val, min, max, text);
    if (item) {
        eduiItemSliderSetFmt(item, "(%d)");
        item->render = eduicbRenderSliderInt;
        item->u.slider.granularity = 1.0f;
    }
    return item;
}


struct eduiitem_s *eduiItemNumberCreate(void *data, void *style, s32 group, void *cb,
                               f32 val, f32 min, f32 max, char *text) {
    struct eduiitem_s *item;

    item = eduiItemSliderCreate(data, style, group, cb, val, min, max, text);
    if (item) {
        item->render = eduicbRenderNumber;
    }
    return item;
}


struct eduiitem_s *eduiItemColourSliderCreate(void *data, struct eduistyle_s *style, s32 group,
                                     void (*change)(struct eduimenu_s *menu,
                                                    struct eduiitem_s *item,
                                                    s32 buttons),
                                     f32 min, f32 range, f32 val, u8 r, u8 g,
                                     u8 b, char *text) {
    struct eduiitem_s *item;

    item = NuMemAllocFn(0x68, edui_srcfile, 0xB40);
    if (item) {
        memset(item, 0, 0x68);
        item->type = EDUI_ITEM_COLOURSLIDER;
        item->data = data;
        item->style = *style;
        item->align = 2;
        item->group = group;
        item->process = eduicbProcessColourSlider;
        item->render = eduicbRenderColourSlider;
        item->destroy = eduicbItemSliderDestroy;
        eduiItemSetText(item, text);
        item->u.slider.change = change;
        item->u.slider.min = min;
        item->u.slider.range = range;
        item->u.slider.r = r;
        item->u.slider.g = g;
        item->u.slider.b = b;
        item->u.slider.fmt = 0;
        item->u.slider.granularity = 1.0f;
        eduiItemSliderSetFmt(item, "(%d)");
        eduiItemSliderSetValEx(item, val, 1, 1);
        return item;
    }
    return 0;
}


inline void eduiMenuDestroy(struct eduimenu_s *menu) {
    struct eduiitem_s *item;
    struct eduiitem_s *next;

    while (menu->head) {
        item = menu->head;
        next = item->next;
        item->destroy(menu, item);
        menu->head = next;
    }
    NuMemFreeFn(menu, edui_srcfile, 0x192);
}


inline s32 eduiMenuDetach(struct eduimenu_s *menu) {
    if (menu->parent) {
        if (menu->child) {
            eduiMenuDetach(menu->child);
        }
        menu->parent->child = 0;
        menu->parent = 0;
        return 1;
    }
    return 0;
}


s32 eduiMenuProcess(struct eduimenu_s *menu, f32 dt, struct nupad_s *pad) {
    struct eduimenu_s *parent;
    s32 up;
    s32 down;
    s32 upfast;
    s32 downfast;

    if (!pad) {
        return 0;
    }
    if (menu->child) {
        eduiMenuProcess(menu->child, dt, pad);
        return 0;
    }
    if (!menu->current) {
        menu->current = menu->head;
        if (!menu->current) {
            return 0;
        }
    }
    if (menu->current->press && pad->paddata) {
        if (menu->current->press(menu, menu->current, pad->paddata,
                                 pad->paddata_db)) {
            return 0;
        }
    }
    if (menu->current->process(menu, menu->current, pad, dt)) {
        return 0;
    }
    downfast = 0;
    upfast = 0;
    down = 0;
    up = 0;
    if (pad->paddata_db & 0x1000) {
        edui_up_repeat = 20;
        up = 1;
    }
    if ((pad->paddata & 0x1000) && edui_up_repeat) {
        if (--edui_up_repeat == 0) {
            edui_up_repeat = 10;
            up = 1;
        }
    }
    if (pad->paddata_db & 0x4000) {
        edui_down_repeat = 20;
        down = 1;
    }
    if ((pad->paddata & 0x4000) && edui_down_repeat) {
        if (--edui_down_repeat == 0) {
            edui_down_repeat = 10;
            down = 1;
        }
    }
    if (up) {
        upfast = pad->paddata & 1;
    }
    if (down) {
        downfast = pad->paddata & 1;
    }
    if (up && menu->current->prev) {
        do {
            if (menu->current == menu->cursor) {
                menu->cursor = menu->current->prev;
            }
            menu->current = menu->current->prev;
            if (!menu->current->prev) {
                return 0;
            }
        } while (upfast);
        return 0;
    }
    if (down && menu->current->next) {
        do {
            menu->current = menu->current->next;
            if (!menu->current->next) {
                return 0;
            }
        } while (downfast);
        return 0;
    }
    if (pad->paddata_db & 0x10) {
        parent = 0;
        if (menu->parent) {
            parent = menu->parent;
            eduiMenuDetach(menu);
        }
        if (menu->cancel) {
            menu->cancel(menu, parent);
        }
    }
    return 0;
}


inline void eduiMenuHighlight(struct eduimenu_s *menu, struct eduiitem_s *item) {
    struct eduiitem_s *scan;

    if (!item->group) {
        return;
    }
    for (scan = menu->head; scan; scan = scan->next) {
        if (scan == item) {
            switch (scan->type) {
            case EDUI_ITEM_SEL:
            case EDUI_ITEM_CHECK:
            default:
                scan->on = 1;
                break;
            case EDUI_ITEM_TOGGLE:
                scan->on = 1 - scan->on;
                break;
            }
        } else if (scan->group == item->group) {
            scan->on = 0;
        }
    }
}


void eduiInit(void) {
    eduiInitMaterials();
}


s32 eduicbProcessColourPick(struct eduimenu_s *menu, struct eduiitem_s *item, struct nupad_s *pad) {
    s32 dx;
    s32 dy;
    f32 s;

    dx = pad->rdata.l_alg_x - 128;
    if (dx > 0) {
        if (dx < 32) {
            dx = 0;
        } else {
            dx = ((pad->rdata.l_alg_x - 160) * 255) / 223;
        }
    } else if (dx > -32) {
        dx = 0;
    } else {
        dx = ((pad->rdata.l_alg_x - 96) * 255) / 223;
    }
    dy = pad->rdata.l_alg_y - 128;
    if (dy > 0) {
        if (dy < 32) {
            dy = 0;
        } else {
            dy = ((pad->rdata.l_alg_y - 160) * 255) / 223;
        }
    } else if (dy > -32) {
        dy = 0;
    } else {
        dy = ((pad->rdata.l_alg_y - 96) * 255) / 223;
    }
    item->u.colour.hue += dx * (1.0f / 8192.0f);
    item->u.colour.brightness += dy * (1.0f / 8192.0f);
    item->u.colour.hue = MIN(1.0f, MAX(0.0f, item->u.colour.hue));
    item->u.colour.brightness =
        MIN(1.0f, MAX(0.0f, item->u.colour.brightness));
    if (pad->rdata.l2_alg) {
        s = item->u.colour.s - pad->rdata.l2_alg / 4192.0f;
        if (s < 0.0f) {
            s = 0.0f;
        }
        item->u.colour.s = s;
        return 1;
    }
    if (pad->rdata.r2_alg) {
        s = item->u.colour.s + pad->rdata.r2_alg / 4192.0f;
        if (s > 1.0f) {
            s = 1.0f;
        }
        item->u.colour.s = s;
        return 1;
    }
    item->u.colour.h = item->u.colour.hue * 360.0f;
    item->u.colour.v = item->u.colour.brightness;
    eduiHSVToRGB(item->u.colour.h, item->u.colour.s, item->u.colour.v,
                 &item->u.colour.r, &item->u.colour.g, &item->u.colour.b);
    if ((pad->paddata_db & 0x40) && item->u.colour.change) {
        item->u.colour.change(menu, item, pad->paddata_db);
    }
    return 0;
}


s32 eduicbRenderGreyPick(struct eduimenu_s *menu, struct eduiitem_s *item, s32 x, s32 y, s32 w,
                         s32 highlight) {
    s32 height;
    u32 colours[4];

    height = w / 4;
    item->x = x;
    item->y = y;
    colours[0] = colours[2] = 0x80000000;
    colours[1] = colours[3] = 0x80FFFFFF;
    NuRndrGradRect2di(x * 16 + (s32)(w * 16 * 0.0f), y * 8,
                      (s32)(w * 16 * 1.0f) - (s32)(w * 16 * 0.0f), height * 8,
                      colours, edui_mtl[ui_bgmtl]);
    NuRndrLine2di((s32)(x + 1 + (w - 2) * item->u.colour.v) * 16, y * 8,
                  (s32)(x + 1 + (w - 2) * item->u.colour.v) * 16,
                  (y + (height >> 1) - 1) * 8, 0x80FFFFFF, edui_mtl[0]);
    NuRndrLine2di((s32)(x + (w - 2) * item->u.colour.v) * 16, y * 8,
                  (s32)(x + (w - 2) * item->u.colour.v) * 16,
                  (y + (height >> 1) - 1) * 8, 0x80FFFFFF, edui_mtl[0]);
    NuRndrLine2di((s32)(x - 1 + (w - 2) * item->u.colour.v) * 16, y * 8,
                  (s32)(x - 1 + (w - 2) * item->u.colour.v) * 16,
                  (y + (height >> 1) - 1) * 8, 0x80FFFFFF, edui_mtl[0]);
    NuRndrLine2di((s32)(x + 1 + (w - 2) * item->u.colour.v) * 16,
                  (y + (height >> 1)) * 8,
                  (s32)(x + 1 + (w - 2) * item->u.colour.v) * 16,
                  (y + height - 1) * 8, 0x80000000, edui_mtl[0]);
    NuRndrLine2di((s32)(x + (w - 2) * item->u.colour.v) * 16,
                  (y + (height >> 1)) * 8,
                  (s32)(x + (w - 2) * item->u.colour.v) * 16,
                  (y + height - 1) * 8, 0x80000000, edui_mtl[0]);
    NuRndrLine2di((s32)(x - 1 + (w - 2) * item->u.colour.v) * 16,
                  (y + (height >> 1)) * 8,
                  (s32)(x - 1 + (w - 2) * item->u.colour.v) * 16,
                  (y + height - 1) * 8, 0x80000000, edui_mtl[0]);
    return height;
}


s32 eduicbProcessTexturePick(struct eduimenu_s *menu, struct eduiitem_s *item, struct nupad_s *pad) {
    s32 dx;
    s32 dy;
    f32 zoom;

    dx = pad->rdata.l_alg_x - 128;
    if (dx > 0) {
        if (dx < 32) {
            dx = 0;
        } else {
            dx = ((pad->rdata.l_alg_x - 160) * 255) / 223;
        }
    } else if (dx > -32) {
        dx = 0;
    } else {
        dx = ((pad->rdata.l_alg_x - 96) * 255) / 223;
    }
    dy = pad->rdata.l_alg_y - 128;
    if (dy > 0) {
        if (dy < 32) {
            dy = 0;
        } else {
            dy = ((pad->rdata.l_alg_y - 160) * 255) / 223;
        }
    } else if (dy > -32) {
        dy = 0;
    } else {
        dy = ((pad->rdata.l_alg_y - 96) * 255) / 223;
    }
    item->u.texpick.u[item->u.texpick.corner] +=
        dx / (item->u.texpick.zoom * 8192.0f);
    item->u.texpick.v[item->u.texpick.corner] +=
        dy / (item->u.texpick.zoom * 8192.0f);
    item->u.texpick.u[item->u.texpick.corner] =
        MIN(1.0f, MAX(0.0f, item->u.texpick.u[item->u.texpick.corner]));
    item->u.texpick.v[item->u.texpick.corner] =
        MIN(1.0f, MAX(0.0f, item->u.texpick.v[item->u.texpick.corner]));
    if (pad->rdata.l2_alg) {
        zoom = item->u.texpick.zoom - pad->rdata.l2_alg / 4192.0f;
        if (zoom < 1.0f) {
            zoom = 1.0f;
        }
        item->u.texpick.zoom = zoom;
        return 1;
    }
    if (pad->rdata.r2_alg) {
        zoom = item->u.texpick.zoom + pad->rdata.r2_alg / 4192.0f;
        if (zoom > 16.0f) {
            zoom = 16.0f;
        }
        item->u.texpick.zoom = zoom;
        return 1;
    }
    if (pad->paddata_db & 0x100) {
        item->u.texpick.u[item->u.texpick.corner] =
            (s32)(item->u.texpick.u[item->u.texpick.corner] * 256.0f + 0.5) *
            (1.0f / 256.0f);
        item->u.texpick.v[item->u.texpick.corner] =
            (s32)(item->u.texpick.v[item->u.texpick.corner] * 256.0f + 0.5) *
            (1.0f / 256.0f);
    }
    if (pad->paddata_db & 0x80) {
        item->u.texpick.corner = (item->u.texpick.corner == 1) ? 0 : 1;
    }
    if ((pad->paddata_db & 0x40) && item->u.texpick.change) {
        item->u.texpick.change(menu, item, pad->paddata_db);
    }
    return 0;
}


s32 eduicbRenderTexturePick(struct eduimenu_s *menu, struct eduiitem_s *item, s32 x, s32 y,
                            s32 w, s32 highlight) {
    s32 height;
    s32 texty;
    s32 i;
    f32 left;
    f32 top;
    f32 uscale;
    f32 vscale;
    u32 colours[2];
    f32 su[2];
    f32 sv[2];

    item->x = x;
    item->y = y;
    left = item->u.texpick.u[item->u.texpick.corner] -
           0.5 / item->u.texpick.zoom;
    top = item->u.texpick.v[item->u.texpick.corner] -
          0.5 / item->u.texpick.zoom;
    uscale = 1.0 / item->u.texpick.zoom;
    vscale = 1.0 / item->u.texpick.zoom;
    left = MIN(1.0f - uscale, MAX(0.0f, left));
    top = MIN(1.0f - vscale, MAX(0.0f, top));
    for (i = 0; i < 2; i++) {
        su[i] = (item->u.texpick.u[i] - left) / uscale;
        su[i] = MIN(1.0f, MAX(0.0f, su[i]));
        sv[i] = (item->u.texpick.v[i] - top) / vscale;
        sv[i] = MIN(1.0f, MAX(0.0f, sv[i]));
    }
    colours[0] = colours[1] = 0x807F7F7F;
    colours[item->u.texpick.corner] = 0x80FFFFFF;
    NuRndrRectUV2di(x * 16, y * 8, w * 16, w * 8, left, top, left + uscale,
                    top + vscale, 0x80808080, item->u.texpick.mtl);
    for (i = 0; i < 2; i++) {
        NuRndrLine2di(x * 16, (s32)(y + w * sv[i]) * 8, (x + w - 1) * 16,
                      (s32)(y + w * sv[i]) * 8, colours[i], edui_mtl[0]);
        NuRndrLine2di((s32)(x + w * su[i]) * 16, y * 8,
                      (s32)(x + w * su[i]) * 16, (y + w - 1) * 8, colours[i],
                      edui_mtl[0]);
    }
    height = (NuFntHeight + (NuFntHeight >> 2)) >> 3;
    texty = ((height * 8 - NuFntHeight) >> 1) + NuFntBase;
    NuRndrRect2di(x * 16, (y + w) * 8, w * 16, height * 8,
                  item->style.colour[item->on], edui_mtl[ui_bgmtl]);
    NuFntSetPen(item->style.pen[item->on]);
    NuFntPrintEx((x * 2 + w) * 8, (y + w) * 8 + texty, 2, "%1.03f,%1.03f",
                 item->u.texpick.u[item->u.texpick.corner] * 256.0f,
                 item->u.texpick.v[item->u.texpick.corner] * 256.0f);
    return height + w;
}


s32 eduicbProcessSlider(struct eduimenu_s *menu, struct eduiitem_s *item, struct nupad_s *pad) {
    f32 step;

    step = 1.6276043e-05f;
    if ((pad->paddata & 4) && (pad->paddata & 8)) {
        if (item->u.slider.min <= 0.0 &&
            item->u.slider.min + item->u.slider.range >= 0.0) {
            item->u.slider.val = 0.0f;
            item->u.slider.flash = 60;
            item->u.slider.frac = (item->u.slider.val - item->u.slider.min) /
                                  item->u.slider.range;
            if (item->u.slider.change) {
                item->u.slider.change(menu, item, pad->paddata);
            }
        }
        edui_slider_repeat = 30;
        return 0;
    }
    if (pad->paddata & 4) {
        edui_slider_repeat = MAX(edui_slider_repeat - 1, 0);
        if (edui_slider_repeat && !(pad->paddata_db & 4)) {
            return 0;
        }
        item->u.slider.val =
            MAX(item->u.slider.min,
                item->u.slider.val - item->u.slider.granularity);
        if (item->u.slider.val > 0.0f) {
            item->u.slider.val += item->u.slider.granularity * 0.5;
        } else {
            item->u.slider.val -= item->u.slider.granularity * 0.5;
        }
        item->u.slider.flash = 60;
        item->u.slider.val =
            item->u.slider.granularity *
            (s32)(item->u.slider.val / item->u.slider.granularity);
        item->u.slider.frac = (item->u.slider.val - item->u.slider.min) /
                              item->u.slider.range;
        if (item->u.slider.change) {
            item->u.slider.change(menu, item, pad->paddata);
        }
        return 0;
    }
    if (pad->paddata & 8) {
        edui_slider_repeat = MAX(edui_slider_repeat - 1, 0);
        if (edui_slider_repeat && !(pad->paddata_db & 8)) {
            return 0;
        }
        item->u.slider.val =
            MIN(item->u.slider.min + item->u.slider.range,
                item->u.slider.val + item->u.slider.granularity);
        if (item->u.slider.val > 0.0f) {
            item->u.slider.val += item->u.slider.granularity * 0.5;
        } else {
            item->u.slider.val -= item->u.slider.granularity * 0.5;
        }
        item->u.slider.flash = 60;
        item->u.slider.val =
            item->u.slider.granularity *
            (s32)(item->u.slider.val / item->u.slider.granularity);
        item->u.slider.frac = (item->u.slider.val - item->u.slider.min) /
                              item->u.slider.range;
        if (item->u.slider.change) {
            item->u.slider.change(menu, item, pad->paddata);
        }
        return 0;
    }
    edui_slider_repeat = 30;
    if (pad->rdata.l2_alg) {
        if (pad->rdata.r_algpad_r) {
            edui_slider_speed *= 1.05;
            item->u.slider.frac =
                MAX(0.0f, item->u.slider.frac -
                               pad->rdata.l2_alg * edui_slider_speed);
        } else {
            edui_slider_speed = 1.6276043e-07f;
            item->u.slider.frac =
                MAX(0.0f, item->u.slider.frac -
                               pad->rdata.l2_alg * step);
        }
        item->u.slider.flash = 60;
        item->u.slider.val =
            item->u.slider.min + item->u.slider.frac * item->u.slider.range;
        if (item->u.slider.change) {
            item->u.slider.change(menu, item, pad->paddata);
        }
        return 0;
    }
    if (pad->rdata.r2_alg) {
        if (pad->rdata.r_algpad_r) {
            edui_slider_speed *= 1.05;
            item->u.slider.frac =
                MIN(1.0f, item->u.slider.frac +
                               pad->rdata.r2_alg * edui_slider_speed);
        } else {
            edui_slider_speed = 1.6276043e-07f;
            item->u.slider.frac =
                MIN(1.0f, item->u.slider.frac +
                               pad->rdata.r2_alg * step);
        }
        item->u.slider.flash = 60;
        item->u.slider.val =
            item->u.slider.min + item->u.slider.frac * item->u.slider.range;
        if (item->u.slider.change) {
            item->u.slider.change(menu, item, pad->paddata);
        }
        return 0;
    }
    edui_slider_speed = 1.6276043e-07f;
    return 0;
}


s32 eduicbRenderSlider(struct eduimenu_s *menu, struct eduiitem_s *item, s32 x, s32 y, s32 w,
                       s32 highlight) {
    s32 height;
    s32 texty;
    char text[512];

    height = (NuFntHeight + (NuFntHeight >> 2)) >> 3;
    item->x = x;
    item->y = y;
    texty = ((height * 8 - NuFntHeight) >> 1) + NuFntBase;
    NuRndrRect2di(x * 16, y * 8, w * 16, height * 16,
                  item->style.colour[item->on], edui_mtl[ui_bgmtl]);
    NuFntSetPen(item->style.pen[item->on]);
    if (item->u.slider.flash) {
        NuFntPrintEx((x * 2 + w) * 8, y * 8 + texty, 2, item->u.slider.fmt,
                     item->u.slider.val);
        item->u.slider.flash--;
    } else {
        NuStrCpy(text, item->text);
        NuStrCat(text, item->u.slider.fmt);
        NuFntPrintEx((x * 2 + w) * 8, y * 8 + texty, 2, text,
                     item->u.slider.val);
    }
    y += height;
    if (item->u.slider.frac >= 0.0 && item->u.slider.frac <= 1.0) {
        NuRndrLine2di((s32)(x + 1 + (w - 2) * item->u.slider.frac) * 16, y * 8,
                      (s32)(x + 1 + (w - 2) * item->u.slider.frac) * 16,
                      (y + height - 1) * 8, item->style.pen[item->on],
                      edui_mtl[0]);
        NuRndrLine2di((s32)(x + (w - 2) * item->u.slider.frac) * 16, y * 8,
                      (s32)(x + (w - 2) * item->u.slider.frac) * 16,
                      (y + height - 1) * 8, item->style.pen[item->on],
                      edui_mtl[0]);
        NuRndrLine2di((s32)(x - 1 + (w - 2) * item->u.slider.frac) * 16, y * 8,
                      (s32)(x - 1 + (w - 2) * item->u.slider.frac) * 16,
                      (y + height - 1) * 8, item->style.pen[item->on],
                      edui_mtl[0]);
    }
    return height * 2;
}


s32 eduicbRenderSliderInt(struct eduimenu_s *menu, struct eduiitem_s *item, s32 x, s32 y, s32 w,
                          s32 highlight) {
    s32 height;
    s32 texty;
    char text[512];

    height = (NuFntHeight + (NuFntHeight >> 2)) >> 3;
    item->x = x;
    item->y = y;
    texty = ((height * 8 - NuFntHeight) >> 1) + NuFntBase;
    NuRndrRect2di(x * 16, y * 8, w * 16, height * 16,
                  item->style.colour[item->on], edui_mtl[0]);
    NuFntSetPen(item->style.pen[item->on]);
    if (item->u.slider.flash) {
        NuFntPrintEx((x * 2 + w) * 8, y * 8 + texty, 2, item->u.slider.fmt,
                     (s32)item->u.slider.val);
        item->u.slider.flash--;
    } else {
        NuStrCpy(text, item->text);
        NuStrCat(text, item->u.slider.fmt);
        NuFntPrintEx((x * 2 + w) * 8, y * 8 + texty, 2, text,
                     (s32)item->u.slider.val);
    }
    y += height;
    NuRndrLine2di((s32)(x + 1 + w * item->u.slider.frac) * 16, y * 8,
                  (s32)(x + 1 + w * item->u.slider.frac) * 16,
                  (y + height - 1) * 8, item->style.pen[item->on], edui_mtl[0]);
    NuRndrLine2di((s32)(x + w * item->u.slider.frac) * 16, y * 8,
                  (s32)(x + w * item->u.slider.frac) * 16,
                  (y + height - 1) * 8, item->style.pen[item->on], edui_mtl[0]);
    NuRndrLine2di((s32)(x - 1 + w * item->u.slider.frac) * 16, y * 8,
                  (s32)(x - 1 + w * item->u.slider.frac) * 16,
                  (y + height - 1) * 8, item->style.pen[item->on], edui_mtl[0]);
    return height * 2;
}


s32 eduicbProcessColourSlider(struct eduimenu_s *menu, struct eduiitem_s *item, struct nupad_s *pad) {
    f32 step;

    step = 1.6276043e-05f;
    if ((pad->paddata & 4) && (pad->paddata & 8)) {
        if (item->u.slider.min <= 0.0 &&
            item->u.slider.min + item->u.slider.range >= 0.0) {
            item->u.slider.val = 0.0f;
            item->u.slider.flash = 60;
            item->u.slider.frac = (item->u.slider.val - item->u.slider.min) /
                                  item->u.slider.range;
            if (item->u.slider.change) {
                item->u.slider.change(menu, item, pad->paddata);
            }
        }
        edui_colourslider_repeat = 30;
        return 0;
    }
    if ((pad->paddata & 1) && (pad->paddata & 2)) {
        if (item->u.slider.min <= 255.0 &&
            item->u.slider.min + item->u.slider.range >= 255.0) {
            item->u.slider.val = 255.0f;
            item->u.slider.flash = 60;
            item->u.slider.frac = (item->u.slider.val - item->u.slider.min) /
                                  item->u.slider.range;
            if (item->u.slider.change) {
                item->u.slider.change(menu, item, pad->paddata);
            }
        }
        edui_colourslider_repeat = 30;
        return 0;
    }
    if (pad->paddata & 4) {
        edui_colourslider_repeat = MAX(edui_colourslider_repeat - 1, 0);
        if (edui_colourslider_repeat && !(pad->paddata_db & 4)) {
            return 0;
        }
        item->u.slider.val =
            MAX(item->u.slider.min,
                item->u.slider.val - item->u.slider.granularity);
        if (item->u.slider.val > 0.0f) {
            item->u.slider.val += item->u.slider.granularity * 0.5;
        } else {
            item->u.slider.val -= item->u.slider.granularity * 0.5;
        }
        item->u.slider.flash = 60;
        item->u.slider.val =
            item->u.slider.granularity *
            (s32)(item->u.slider.val / item->u.slider.granularity);
        item->u.slider.frac = (item->u.slider.val - item->u.slider.min) /
                              item->u.slider.range;
        if (item->u.slider.change) {
            item->u.slider.change(menu, item, pad->paddata);
        }
        return 0;
    }
    if (pad->paddata & 8) {
        edui_colourslider_repeat = MAX(edui_colourslider_repeat - 1, 0);
        if (edui_colourslider_repeat && !(pad->paddata_db & 8)) {
            return 0;
        }
        item->u.slider.val =
            MIN(item->u.slider.min + item->u.slider.range,
                item->u.slider.val + item->u.slider.granularity);
        if (item->u.slider.val > 0.0f) {
            item->u.slider.val += item->u.slider.granularity * 0.5;
        } else {
            item->u.slider.val -= item->u.slider.granularity * 0.5;
        }
        item->u.slider.flash = 60;
        item->u.slider.val =
            item->u.slider.granularity *
            (s32)(item->u.slider.val / item->u.slider.granularity);
        item->u.slider.frac = (item->u.slider.val - item->u.slider.min) /
                              item->u.slider.range;
        if (item->u.slider.change) {
            item->u.slider.change(menu, item, pad->paddata);
        }
        return 0;
    }
    edui_colourslider_repeat = 30;
    if (pad->rdata.l2_alg) {
        if (pad->rdata.r_algpad_r) {
            edui_colourslider_speed *= 1.05;
            item->u.slider.frac =
                MAX(0.0f, item->u.slider.frac -
                               pad->rdata.l2_alg * edui_colourslider_speed);
        } else {
            edui_colourslider_speed = 1.6276043e-07f;
            item->u.slider.frac =
                MAX(0.0f, item->u.slider.frac -
                               pad->rdata.l2_alg * step);
        }
        item->u.slider.flash = 60;
        item->u.slider.val =
            item->u.slider.min + item->u.slider.frac * item->u.slider.range;
        if (item->u.slider.change) {
            item->u.slider.change(menu, item, pad->paddata);
        }
        return 0;
    }
    if (pad->rdata.r2_alg) {
        if (pad->rdata.r_algpad_r) {
            edui_colourslider_speed *= 1.05;
            item->u.slider.frac =
                MIN(1.0f, item->u.slider.frac +
                               pad->rdata.r2_alg * edui_colourslider_speed);
        } else {
            edui_colourslider_speed = 1.6276043e-07f;
            item->u.slider.frac =
                MIN(1.0f, item->u.slider.frac +
                               pad->rdata.r2_alg * step);
        }
        item->u.slider.flash = 60;
        item->u.slider.val =
            item->u.slider.min + item->u.slider.frac * item->u.slider.range;
        if (item->u.slider.change) {
            item->u.slider.change(menu, item, pad->paddata);
        }
        return 0;
    }
    edui_colourslider_speed = 1.6276043e-07f;
    return 0;
}


s32 eduicbRenderColourSlider(struct eduimenu_s *menu, struct eduiitem_s *item, s32 x, s32 y,
                             s32 w, s32 highlight) {
    s32 height;
    s32 texty;
    f32 width;
    u32 colour;
    u32 colours[4];

    height = (NuFntHeight + (NuFntHeight >> 2)) >> 3;
    item->x = x;
    item->y = y;
    texty = ((height * 8 - NuFntHeight) >> 1) + NuFntBase;
    NuRndrRect2di(x * 16, y * 8, w * 16, height * 16,
                  item->style.colour[item->on], edui_mtl[0]);
    NuFntSetPen(item->style.pen[item->on]);
    NuFntPrintEx((x * 3 + w) * 8, y * 8 + texty, 2, item->u.slider.fmt,
                 (s32)item->u.slider.val);
    NuFntPrintEx((x + w) * 8, y * 8 + texty, 2, item->text);
    y += height;
    width = w * 16;
    colour = item->u.slider.r + (item->u.slider.g << 8) +
             (item->u.slider.b << 16);
    colours[0] = colours[2] = 0;
    colours[1] = colours[3] = colour;
    NuRndrGradRect2di(x * 16 + (s32)(width * 0.0f), y * 8,
                      (s32)width - (s32)(width * 0.0f), height * 8, colours,
                      edui_mtl[ui_bgmtl]);
    NuRndrLine2di((s32)(x + 1 + w * item->u.slider.frac) * 16, y * 8,
                  (s32)(x + 1 + w * item->u.slider.frac) * 16,
                  (y + height - 1) * 8, item->style.pen[item->on], edui_mtl[0]);
    NuRndrLine2di((s32)(x + w * item->u.slider.frac) * 16, y * 8,
                  (s32)(x + w * item->u.slider.frac) * 16,
                  (y + height - 1) * 8, item->style.pen[item->on], edui_mtl[0]);
    NuRndrLine2di((s32)(x - 1 + w * item->u.slider.frac) * 16, y * 8,
                  (s32)(x - 1 + w * item->u.slider.frac) * 16,
                  (y + height - 1) * 8, item->style.pen[item->on], edui_mtl[0]);
    return height * 2;
}


void eduiCreateMessageMenu(struct eduimenu_s *parent, char *text, s32 error) {
    struct eduistyle_s style = {{0x800000C0, 0x80FF0000}, {0x80808080, 0x80404040}};

    if (error == 1) {
        style.pen[0] = 0x8000C000;
    }
    edui_messagemenu = eduiMenuCreate(70, 70, 180, 250, parent->font,
                                      eduicbCancelMessageMenu,
                                      edui_str_message);
    if (edui_messagemenu) {
        eduiMenuAddItem(edui_messagemenu,
                        eduiItemSelCreate((void *)1, &style, 0, 0, 0, text));
        eduiMenuAttach(parent, edui_messagemenu);
        edui_messagemenu->x = parent->x + 10;
        edui_messagemenu->y = parent->y + 40;
    }
}


void eduiMenuRender(struct eduimenu_s *menu) {
    struct eduiitem_s *item;
    s32 x;
    s32 y;
    s32 height;
    s32 size;
    s32 texty;
    s32 highlight;
    s32 scroll;

    y = menu->y;
    NuFntSet(menu->font);
    if (menu->title) {
        height = (NuFntHeight + (NuFntHeight >> 2)) >> 3;
        texty = ((height * 8 - NuFntHeight) >> 1) + NuFntBase;
        NuRndrRect2di(menu->x * 16, y * 8, menu->w * 16, height * 8,
                      0x80000000, edui_mtl[ui_bgmtl]);
        NuFntSetPen(0x80FFFFFF);
        NuFntPrintEx((menu->x + 1) * 16, y * 8 + texty, 0, menu->title);
        y += height;
    }
    if (menu->cursor) {
        item = menu->cursor;
        scroll = menu->current != 0;
        if (item != menu->head) {
            height = (NuFntHeight + (NuFntHeight >> 2)) >> 4;
            NuRndrRect2di(menu->x * 16, y * 8, menu->w * 16, height * 8,
                          0x80000000, edui_mtl[ui_bgmtl]);
            NuRndrLine2di((menu->x * 2 + menu->w) * 8, (y + 1) * 8,
                          (menu->x * 2 + menu->w) * 8 - (height - 2) * 16,
                          (y + height - 2) * 8, 0x80FFFFFF, edui_mtl[0]);
            NuRndrLine2di((menu->x * 2 + menu->w) * 8, (y + 1) * 8,
                          (menu->x * 2 + menu->w) * 8 + (height - 2) * 16,
                          (y + height - 2) * 8, 0x80FFFFFF, edui_mtl[0]);
            y += height;
        }
        do {
            if (item == menu->current) {
                scroll = 0;
            }
            highlight = 0;
            if (item == menu->current) {
                highlight = menu->child == 0;
            }
            x = menu->x;
            edui_item_width = menu->w;
            size = item->render(0, item, x, y, menu->w, highlight);
            if (highlight) {
                NuRndrLineRect2di(x * 16, y * 8, edui_item_width * 16,
                                  size * 8, 0x80FFFFFF, edui_mtl[0]);
            }
            item = item->next;
            y += size;
            if (item && y >= menu->y + menu->h) {
                height = (NuFntHeight + (NuFntHeight >> 2)) >> 4;
                NuRndrRect2di(menu->x * 16, y * 8, menu->w * 16, height * 8,
                              0x80000000, edui_mtl[ui_bgmtl]);
                NuRndrLine2di((menu->x * 2 + menu->w) * 8,
                              (y + height - 2) * 8,
                              (menu->x * 2 + menu->w) * 8 - (height - 2) * 16,
                              (y + 1) * 8, 0x80FFFFFF, edui_mtl[0]);
                NuRndrLine2di((menu->x * 2 + menu->w) * 8,
                              (y + height - 2) * 8,
                              (menu->x * 2 + menu->w) * 8 + (height - 2) * 16,
                              (y + 1) * 8, 0x80FFFFFF, edui_mtl[0]);
                break;
            }
        } while (item);
        if (scroll && menu->cursor->next) {
            menu->cursor = menu->cursor->next;
        }
    }
    NuRndrLineRect2di((menu->x - 1) * 16, (menu->y - 1) * 8, (menu->w + 2) * 16,
                      (y - menu->y + 2) * 8, 0x80000000, edui_mtl[0]);
    if (menu->child) {
        eduiMenuRender(menu->child);
    }
}


s32 eduicbProcessGreyPick(struct eduimenu_s *menu, struct eduiitem_s *item, struct nupad_s *pad) {
    f32 v;

    if (pad->rdata.l2_alg) {
        v = item->u.colour.v - pad->rdata.l2_alg / 4192.0f;
        if (v < 0.0f) {
            v = 0.0f;
        }
        item->u.colour.v = v;
        return 1;
    }
    if (pad->rdata.r2_alg) {
        v = item->u.colour.v + pad->rdata.r2_alg / 4192.0f;
        if (v > 1.0f) {
            v = 1.0f;
        }
        item->u.colour.v = v;
        return 1;
    }
    item->u.colour.h = item->u.colour.s = 0.0f;
    if ((pad->paddata_db & 0x40) && item->u.colour.change) {
        item->u.colour.change(menu, item, pad->paddata_db);
    }
    return 0;
}


s32 eduicbProcessSel(struct eduimenu_s *menu, struct eduiitem_s *item, struct nupad_s *pad, f32 dt) {
    if (!(pad->paddata_db & 0x40)) {
        return 0;
    }
    eduiMenuHighlight(menu, item);
    if (!item->u.sel.select) {
        return 0;
    }
    item->u.sel.select(menu, item, pad->paddata_db);
    return 0;
}


s32 eduicbRenderSel(struct eduimenu_s *menu, struct eduiitem_s *item, s32 x, s32 y, s32 w,
                    s32 highlight) {
    s32 height;
    s32 texty;

    height = (NuFntHeight + (NuFntHeight >> 2)) >> 3;
    item->x = x;
    item->y = y;
    texty = ((height * 8 - NuFntHeight) >> 1) + NuFntBase;
    NuRndrRect2di(x * 16, y * 8, w * 16, height * 8,
                  item->style.colour[item->on], edui_mtl[ui_bgmtl]);
    NuFntSetPen(item->style.pen[item->on]);
    switch (item->align) {
    case 0:
        NuFntPrintEx(x * 16, y * 8 + texty, 0, item->text);
        break;
    case 1:
        NuFntPrintEx((x + w) * 16, y * 8 + texty, 1, item->text);
        break;
    case 2:
    default:
        NuFntPrintEx((x * 2 + w) * 8, y * 8 + texty, item->align, item->text);
        break;
    }
    return height;
}


inline void eduicbItemDestroy(struct eduimenu_s *menu, struct eduiitem_s *item) {
    if (item->text) {
        NuMemFreeFn(item->text, edui_srcfile, 0x180);
    }
    NuMemFreeFn(item, edui_srcfile, 0x182);
    menu->cursor = menu->head;
}


void eduicbItemSliderDestroy(struct eduimenu_s *menu, struct eduiitem_s *item) {
    if (item->u.slider.fmt) {
        NuMemFreeFn(item->u.slider.fmt, edui_srcfile, 0x84F);
    }
    eduicbItemDestroy(menu, item);
}


s32 eduicbRenderNumber(struct eduimenu_s *menu, struct eduiitem_s *item, s32 x, s32 y, s32 w,
                       s32 highlight) {
    s32 height;
    s32 texty;
    char text[512];

    height = (NuFntHeight + (NuFntHeight >> 2)) >> 3;
    item->x = x;
    item->y = y;
    texty = ((height * 8 - NuFntHeight) >> 1) + NuFntBase;
    NuRndrRect2di(x * 16, y * 8, w * 16, height * 8,
                  item->style.colour[item->on], edui_mtl[ui_bgmtl]);
    NuFntSetPen(item->style.pen[item->on]);
    NuStrCpy(text, item->text);
    NuStrCat(text, item->u.slider.fmt);
    NuFntPrintEx(x * 16, y * 8 + texty, 0, text, item->u.slider.val);
    return height;
}


void cbgpcfgDel(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons) {
    struct eduiitem_s *pick;

    pick = item->data;
    if (pick->u.grad.stage) {
        eduiGradStageDelete(pick, pick->u.grad.stage);
    }
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
}


inline void eduiGradStageDelete(struct eduiitem_s *pick, struct eduigradstage_s *stage) {
    if (stage->prev) {
        stage->prev->next = stage->next;
    } else {
        pick->u.grad.first = stage->next;
    }
    if (stage->next) {
        stage->next->prev = stage->prev;
    }
    if (stage->next) {
        pick->u.grad.stage = stage->next;
    } else {
        pick->u.grad.stage = stage->prev;
    }
    NuMemFreeFn(stage, edui_srcfile, 0x55B);
}


void eduicbItemGradPickDestroy(struct eduimenu_s *menu, struct eduiitem_s *item) {
    while (item->u.grad.first) {
        eduiGradStageDelete(item, item->u.grad.first);
    }
    NuMemFreeFn(item, edui_srcfile, 0x58C);
}


void tpentOnEnter(struct eduimenu_s *menu, struct eduiitem_s *item, u32 paddata) {
    struct eduiitem_s *pick;

    pick = item->data;
    NuStrCpy(pick->u.textpick.text, item->u.textpick.text);
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
}


void cbgpcfgAdd(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons) {
    struct eduiitem_s *pick;
    struct eduigradstage_s *stage;
    f32 pos;
    f32 h;
    f32 s;
    f32 v;

    pick = item->data;
    stage = pick->u.grad.stage;
    if (!stage) {
        h = 0.0f;
        v = 1.0f;
        pos = h;
        s = h;
    } else {
        if (stage->next) {
            pos = (stage->next->pos + stage->pos) * 0.5f;
        } else {
            pos = (stage->pos + 1.0f) * 0.5f;
        }
        h = pick->u.grad.stage->h;
        s = pick->u.grad.stage->s;
        v = pick->u.grad.stage->v;
    }
    pick->u.grad.stage = eduiGradStageAdd(pick, pos, h, s, v);
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
}


void cbgpcfgCopy(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons) {
    struct eduiitem_s *pick;
    struct eduigradstage_s *stage;

    pick = item->data;
    stage = pick->u.grad.stage;
    if (stage) {
        if (item->type == EDUI_ITEM_GRADPICK) {
            gpcfg_clip_h = stage->h;
            gpcfg_clip_s = stage->s;
            gpcfg_clip_v = stage->v;
        } else {
            gpcfg_clip_grey = stage->v;
        }
    }
}


void cbgpcfgPaste(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons) {
    struct eduiitem_s *pick;

    pick = item->data;
    if (pick->u.grad.stage) {
        if (item->type == EDUI_ITEM_GRADPICK) {
            eduiGradStageSetHSV(pick->u.grad.stage, gpcfg_clip_h, gpcfg_clip_s,
                                gpcfg_clip_v);
        } else {
            eduiGradStageSetHSV(pick->u.grad.stage, 0.0f, 0.0f,
                                gpcfg_clip_grey);
        }
    }
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
}


void cbgpcfgCPPress(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons) {
    struct eduiitem_s *pick;

    pick = item->data;
    if (pick->u.grad.stage) {
        eduiGradStageSetHSV(pick->u.grad.stage, item->u.colour.h,
                            item->u.colour.s, item->u.colour.v);
    }
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
}


void eduicbPickCFGCancel(struct eduimenu_s *menu, struct eduimenu_s *parent) {
    eduiMenuDestroy(menu);
}


void eduicbCancelMessageMenu(struct eduimenu_s *menu, struct eduiitem_s *item) {
    eduiMenuDestroy(edui_messagemenu);
    edui_messagemenu = 0;
}


s32 eduiItemRender(struct eduiitem_s *item, s32 x, s32 y, s32 w, s32 highlight) {
    s32 height;

    edui_item_width = w;
    height = item->render(0, item, x, y, w, highlight);
    if (highlight) {
        NuRndrLineRect2di(x * 16, y * 8, edui_item_width * 16, height * 8,
                          0x80FFFFFF, edui_mtl[0]);
    }
    return height;
}


void CreateTestMenu(void *font) {
    struct eduistyle_s style = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};
    struct eduiitem_s *grad;

    eduiInitMaterials();
    edui_test_submenu1 = eduiMenuCreate(90, 90, 180, 50, font, 0, "SubMenu1");
    if (edui_test_submenu1) {
        eduiMenuAddItem(edui_test_submenu1,
                        eduiItemSelCreate((void *)0, &style, 0, 1, cbSel,
                                          "Sub Sel 0"));
        eduiMenuAddItem(edui_test_submenu1,
                        eduiItemSelCreate((void *)1, &style, 0, 1, cbSel,
                                          "Sub Sel 1"));
        eduiMenuAddItem(edui_test_submenu1,
                        eduiItemSelCreate((void *)2, &style, 0, 1, cbSel,
                                          "Sub Sel 2"));
    }
    edui_test_submenu0 = eduiMenuCreate(70, 70, 180, 50, font, 0, "SubMenu0");
    if (edui_test_submenu0) {
        eduiMenuAddItem(edui_test_submenu0,
                        eduiItemSelCreate((void *)0, &style, 0, 1, cbSel,
                                          edui_str_subsel));
        eduiMenuAddItem(edui_test_submenu0,
                        eduiItemSelCreate(edui_test_submenu1, &style, 0, 0,
                                          cbSubMenu, "Sub Menu 1.."));
        eduiMenuAddItem(edui_test_submenu0,
                        eduiItemSelCreate((void *)2, &style, 0, 1, cbSel,
                                          edui_str_subsel));
    }
    edui_test_menu = eduiMenuCreate(50, 50, 180, 50, font, 0, "Main Menu");
    if (edui_test_menu) {
        eduiMenuAddItem(edui_test_menu,
                        eduiItemSelCreate(edui_test_submenu0, &style, 0, 0,
                                          cbSubMenu, "Sub Menu 0.."));
        grad = eduiItemGradPickCreate((void *)1, &style, cbGradChange,
                                      "GradPick");
        if (grad) {
            eduiGradStageInsert(grad, 0.0f, 0.0f, 0.0f, 0.0f);
            eduiGradStageInsert(grad, 1.0f, 0.0f, 0.0f, 1.0f);
        }
        eduiMenuAddItem(edui_test_menu, grad);
        grad = eduiItemGreyGradPickCreate((void *)1, &style, cbGradChange,
                                          "GreyGradPick");
        if (grad) {
            eduiGradStageInsert(grad, 0.0f, 0.0f, 0.0f, 0.0f);
            eduiGradStageInsert(grad, 1.0f, 0.0f, 0.0f, 1.0f);
        }
        eduiMenuAddItem(edui_test_menu, grad);
        eduiMenuAddItem(edui_test_menu,
                        eduiItemSelCreate((void *)2, &style, 0, 1, cbSel,
                                          edui_str_sel));
    }
}


void cbSel(struct eduimenu_s *menu, struct eduiitem_s *item) {
    edui_test_sel = item->data;
}


void cbSubMenu(struct eduimenu_s *menu, struct eduiitem_s *item) {
    struct eduimenu_s *submenu;

    submenu = item->data;
    if (!submenu->parent) {
        while (menu->child) {
            menu = menu->child;
        }
        submenu->parent = menu;
        menu->child = submenu;
    }
}


void cbGradChange(struct eduimenu_s *menu, struct eduiitem_s *item) {
}


void TestMenu(void *pad) {
    eduiMenuProcess(edui_test_menu, 1.0f / 60.0f, pad);
    eduiMenuRender(edui_test_menu);
}
