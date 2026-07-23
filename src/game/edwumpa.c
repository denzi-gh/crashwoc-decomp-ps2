/*
 * Unit: game/edwumpa
 *
 * Functions:
 *   0x00235280 edwmpDrawInfo
 *   0x002353a8 edwmpDRAW
 *   0x00235830 edwmpDoInput
 *   0x00235d88 edwmpProc
 *   0x00235f70 edwmpADD
 *   0x00235fd0 edwmpDELETE
 *   0x00236058 edwmpInit
 *   0x002360f8 edwmpClose
 *   0x00236118 edwmpEnter
 *   0x002361c0 edwmpRender
 *   0x00236218 edwmpcbSaveWumpaData
 *   0x00236258 edwmpCbCancelOptMenu
 */

#include "creature.h"

struct pad_s {
    u8 unk_0x000[0x564];
    u32 buttons;            /* 0x564 */
};

/* Wumpa placement entry: pos at 0x00, stride 0x4C (verified in edwmpADD/DELETE). */
struct wumpa_s {
    struct nuvec_s pos;     /* 0x00 */
    u8 unk_0x0C[0x40];      /* 0x0C */
};                          /* 0x4C */

extern struct wumpa_s Wumpa[];
extern s32 WUMPACOUNT;
extern s32 PLAYERCOUNT;
extern s32 edwmp_used;
extern struct nuvec_s edwmp_cam_pos;
extern struct nuvec_s v000;
extern void *edwmp_active_menu;
extern void *edwmp_options_menu;
extern char D_00620338[];
extern s32 D_00633348;
#define edwmp_selected D_00633348

extern void *app_fnt;
extern u8 D_00631C18[];
extern u8 D_00620358[];
extern s32 PANELOFF;
extern char tbuf[];
extern char **LDATA;
extern char D_00631C10[];
extern char D_006202E8[];
extern char D_00620300[];
extern char D_00620310[];
extern f32 D_00633340;
extern s32 D_00633344;
extern f32 D_0062E334;
extern f32 D_0062E338;
extern s32 D_00631BF8;
extern s32 screendump;

struct edwmp_itemdef_s {
    u8 data[16];
};
extern struct edwmp_itemdef_s D_00620348;

extern void eduiMenuDestroy(void *menu);
extern void eduiMenuRender(void *menu);
extern void eduiCreateMessageMenu(void *menu, char *text, s32 flag);
extern void ResetWumpa(void);
extern void SaveWumpa(void);
extern void ResetTimeTrial(void);
extern void ResetCheckpoint(s32 a, s32 b, s32 c, f32 d);
extern void ResetBonus(void);
extern void ResetDeath(void);
extern void ResetGemPath(void);
extern void RestoreCrateTypeData(void);
extern void ResetCrates(void);
extern void ResetAI(void);
extern void edcamSet(void);
extern void edcamSetPosAng(struct nuvec_s *pos, s32 a, s32 b);
extern void DrawCameraTarget(struct nuvec_s *pos);
extern void NuFntScale(s32 x, s32 y);
extern void *eduiMenuCreate(s32 x, s32 y, s32 w, s32 h, void *font,
                            void *cancelcb, void *arg);
extern void *eduiItemSelCreate(s32 a0, void *tmpl, s32 a2, s32 a3, void *cb,
                               void *label);
extern void eduiMenuAddItem(void *menu, void *item);
extern void NuFntSetPen(s32 colour);
extern void NuFntPrintEx(s32 x, s32 y, s32 colour, char *fmt, ...);
extern u64 fptodp(f32 value);
extern char *strcpy(char *dst, const char *src);
extern char *strupr(char *s);
extern void eduiMenuProcess(void *menu, struct pad_s *pad);
extern void NuVecSub(struct nuvec_s *d, struct nuvec_s *a, struct nuvec_s *b);
extern f32 NewShadowMask(struct nuvec_s *pos, s32 flag, f32 y);
extern void *InCrate(f32 x, f32 z, f32 ytop, f32 ybot, f32 radius);


void edwmpDRAW(void);
void edwmpDrawInfo(void);
void edwmpDoInput(struct pad_s *pad);
void edwmpCbCancelOptMenu(void);
void edwmpcbSaveWumpaData(void *menu);

void edwmpDrawInfo(void)
{
    if (PANELOFF) {
        return;
    }
    NuFntScale(8, 12);
    NuFntSetPen(-1);
    strcpy(tbuf, LDATA[0]);
    strupr(tbuf);
    NuFntPrintEx(0x400, 0x100, 0, D_00631C10, tbuf);
    NuFntPrintEx(0x400, 0x200, 0, D_006202E8, WUMPACOUNT, 0x100);
    NuFntPrintEx(0x400, 0x300, 0, D_00620300, fptodp(D_00633340));
    NuFntPrintEx(0x400, 0xD00, 0, D_00620310, fptodp(edwmp_cam_pos.x),
                 fptodp(edwmp_cam_pos.y), fptodp(edwmp_cam_pos.z));
}

s32 edwmpProc(struct pad_s *pad)
{
    struct nuvec_s v;
    s32 i;
    f32 bestdist;
    f32 threshold;
    f32 mask;

    if (screendump) {
        return 0;
    }
    D_00631BF8 += 6;
    if (edwmp_active_menu) {
        eduiMenuProcess(edwmp_active_menu, pad);
        return 0;
    }
    edwmp_selected = -1;
    threshold = D_0062E334;
    for (i = 0; i < WUMPACOUNT; i++) {
        f32 d;
        NuVecSub(&v, &edwmp_cam_pos, &Wumpa[i].pos);
        d = v.x * v.x + v.y * v.y + v.z * v.z;
        if (d < threshold) {
            if (edwmp_selected == -1 || d < bestdist) {
                edwmp_selected = i;
                bestdist = d;
            }
        }
    }
    if (edwmp_selected != -1) {
        v = Wumpa[edwmp_selected].pos;
    } else {
        v = edwmp_cam_pos;
    }
    mask = NewShadowMask(&v, -1, 0.0f);
    if (mask != D_0062E338) {
        D_00633340 = v.y - mask;
    } else {
        D_00633340 = 0.0f;
    }
    D_00633344 = InCrate(edwmp_cam_pos.x, edwmp_cam_pos.z, edwmp_cam_pos.y,
                         edwmp_cam_pos.y, 0.0f) != 0;
    edwmpDoInput(pad);
    return (pad->buttons & 0x800) != 0;
}

void edwmpADD(void)
{
    Wumpa[WUMPACOUNT++].pos = edwmp_cam_pos;
    ResetWumpa();
}

void edwmpDELETE(void)
{
    s32 i = edwmp_selected;

    if (i != -1) {
        WUMPACOUNT--;
        for (; i < WUMPACOUNT; i++) {
            Wumpa[i].pos = Wumpa[i + 1].pos;
        }
        ResetWumpa();
        edwmp_selected = -1;
    }
}

void edwmpInit(void)
{
    struct edwmp_itemdef_s buf;

    buf = D_00620348;
    edwmp_options_menu = eduiMenuCreate(0x46, 0x46, 0xB4, 0x12C, app_fnt,
                                        edwmpCbCancelOptMenu, D_00631C18);
    if (edwmp_options_menu != 0) {
        eduiMenuAddItem(edwmp_options_menu,
                        eduiItemSelCreate(1, &buf, 0, 0, edwmpcbSaveWumpaData,
                                          D_00620358));
    }
}

void edwmpClose(void)
{
    eduiMenuDestroy(edwmp_options_menu);
}

void edwmpEnter(void)
{
    struct nuvec_s *pos;

    ResetTimeTrial();
    ResetCheckpoint(-1, -1, 0, 0.0f);
    ResetBonus();
    ResetDeath();
    ResetGemPath();
    RestoreCrateTypeData();
    ResetCrates();
    ResetWumpa();
    ResetAI();
    if (edwmp_used == 0) {
        if (PLAYERCOUNT != 0) {
            pos = &player->obj.pos;
        } else {
            pos = &v000;
        }
        edcamSetPosAng(pos, 0, 0);
        edwmp_used = 1;
    }
}

void edwmpRender(void)
{
    edcamSet();
    DrawCameraTarget(&edwmp_cam_pos);
    edwmpDRAW();
    edwmpDrawInfo();
    if (edwmp_active_menu) {
        NuFntScale(12, 12);
        eduiMenuRender(edwmp_active_menu);
    }
}

void edwmpcbSaveWumpaData(void *menu)
{
    ResetWumpa();
    SaveWumpa();
    eduiCreateMessageMenu(menu, D_00620338, 1);
}

void edwmpCbCancelOptMenu(void)
{
    edwmp_active_menu = 0;
}
