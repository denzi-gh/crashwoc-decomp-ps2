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

#include "nu_libc.h"

#include "numath/nuvec.h"
#include "game/gametypes.h"
#include "gamelib/terrain.h"
#include "game/creature.h"
#include "nups2/ps2ctl.h"
#include "edtools/edui.h"

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
#define edwmp_cam_in_crate D_00633344
extern s32 D_00631BF8;
extern s32 screendump;

struct edwmp_itemdef_s {
    u8 data[16];
};
extern struct edwmp_itemdef_s D_00620348;

extern void eduiMenuDestroy(void *menu);
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
extern char *strupr(char *s);
extern void eduiMenuProcess(void *menu, struct nupad_s *pad);
extern void *InCrate(f32 x, f32 z, f32 ytop, f32 ybot, f32 radius);
extern void NuRndrLine3dDbg(s32 colour, f32 x0, f32 y0, f32 z0, f32 x1, f32 y1,
                            f32 z1);
extern s32 wmp_start;
extern f32 cursor_terrain_height_diff;
extern f32 old_cursor_height;
extern f32 edwmp_cam_ax;
extern f32 edwmp_cam_ay;
extern f32 D_00631BFC;
#define edwmp_move_step D_00631BFC
extern s32 D_00631C24;
#define edwmp_goto_selected D_00631C24
extern f32 NuTrigTable[];
extern s32 VEHICLECONTROL;
extern struct nuvec_s *pos_START;
extern void ComplexRailPosition(struct nuvec_s *pos, s32 iRAIL, s32 iALONG,
                                struct RPos_s *rpos, s32 a4);
extern void edcamMove(struct nupad_s *pad);
extern void edcamGetPosAng(struct nuvec_s *pos, f32 *ax, f32 *ay);
extern void edcamSetPos(struct nuvec_s *pos);


void edwmpDRAW(void);
void edwmpDrawInfo(void);
void edwmpDoInput(struct nupad_s *pad);
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

void edwmpDRAW(void)
{
    struct nuvec_s v[6];
    struct wumpa_s *wumpa;
    s32 i;

    if (edwmp_selected != -1) {
        wumpa = &Wumpa[edwmp_selected];
        for (i = 0; i < 6; i++) {
            v[i] = wumpa->pos0;
        }
        v[0].y += 0.3f;
        v[1].y -= 0.3f;
        v[2].z += 0.3f;
        v[3].x += 0.3f;
        v[4].z -= 0.3f;
        v[5].x -= 0.3f;
        NuRndrLine3dDbg(0xFF00, v[0].x, v[0].y, v[0].z, v[2].x, v[2].y, v[2].z);
        NuRndrLine3dDbg(0xFF00, v[0].x, v[0].y, v[0].z, v[3].x, v[3].y, v[3].z);
        NuRndrLine3dDbg(0xFF00, v[0].x, v[0].y, v[0].z, v[4].x, v[4].y, v[4].z);
        NuRndrLine3dDbg(0xFF00, v[0].x, v[0].y, v[0].z, v[5].x, v[5].y, v[5].z);
        NuRndrLine3dDbg(0xFF00, v[1].x, v[1].y, v[1].z, v[2].x, v[2].y, v[2].z);
        NuRndrLine3dDbg(0xFF00, v[1].x, v[1].y, v[1].z, v[3].x, v[3].y, v[3].z);
        NuRndrLine3dDbg(0xFF00, v[1].x, v[1].y, v[1].z, v[4].x, v[4].y, v[4].z);
        NuRndrLine3dDbg(0xFF00, v[1].x, v[1].y, v[1].z, v[5].x, v[5].y, v[5].z);
        NuRndrLine3dDbg(0xFF00, v[2].x, v[2].y, v[2].z, v[3].x, v[3].y, v[3].z);
        NuRndrLine3dDbg(0xFF00, v[3].x, v[3].y, v[3].z, v[4].x, v[4].y, v[4].z);
        NuRndrLine3dDbg(0xFF00, v[4].x, v[4].y, v[4].z, v[5].x, v[5].y, v[5].z);
        NuRndrLine3dDbg(0xFF00, v[5].x, v[5].y, v[5].z, v[2].x, v[2].y, v[2].z);
    }
    if (edwmp_cam_in_crate) {
        for (i = 0; i < 6; i++) {
            v[i] = edwmp_cam_pos;
        }
        v[0].x -= 0.3f;
        v[0].y -= 0.3f;
        v[1].x += 0.3f;
        v[1].y += 0.3f;
        v[2].z -= 0.3f;
        v[2].y -= 0.3f;
        v[3].z += 0.3f;
        v[3].y += 0.3f;
        v[4].x -= 0.3f;
        v[4].z -= 0.3f;
        v[5].x += 0.3f;
        v[5].z += 0.3f;
        NuRndrLine3dDbg(0xFF, v[0].x, v[0].y, v[0].z, v[1].x, v[1].y, v[1].z);
        NuRndrLine3dDbg(0xFF, v[2].x, v[2].y, v[2].z, v[3].x, v[3].y, v[3].z);
        NuRndrLine3dDbg(0xFF, v[4].x, v[4].y, v[4].z, v[5].x, v[5].y, v[5].z);
        for (i = 0; i < 6; i++) {
            v[i] = edwmp_cam_pos;
        }
        v[0].x += 0.3f;
        v[0].y -= 0.3f;
        v[1].x -= 0.3f;
        v[1].y += 0.3f;
        v[2].z += 0.3f;
        v[2].y -= 0.3f;
        v[3].z -= 0.3f;
        v[3].y += 0.3f;
        v[4].x += 0.3f;
        v[4].z -= 0.3f;
        v[5].x -= 0.3f;
        v[5].z += 0.3f;
        NuRndrLine3dDbg(0xFF, v[0].x, v[0].y, v[0].z, v[1].x, v[1].y, v[1].z);
        NuRndrLine3dDbg(0xFF, v[2].x, v[2].y, v[2].z, v[3].x, v[3].y, v[3].z);
        NuRndrLine3dDbg(0xFF, v[4].x, v[4].y, v[4].z, v[5].x, v[5].y, v[5].z);
    }
}

void edwmpDoInput(struct nupad_s *pad)
{
    struct RPos_s rpos;
    f32 shadow = 0.0f;
    u16 angle;
    s32 i;

    ComplexRailPosition(&edwmp_cam_pos, -1, -1, &rpos, 0);
    angle = rpos.angle;
    if (wmp_start) {
        shadow = NewShadowMask(&edwmp_cam_pos, shadow, -1);
        if (shadow != 2000000.0f) {
            wmp_start = 0;
            old_cursor_height = edwmp_cam_pos.y;
            cursor_terrain_height_diff = edwmp_cam_pos.y - shadow;
            edwmp_cam_pos.y = shadow + cursor_terrain_height_diff;
        }
    }
    if (edwmp_cam_pos.y != old_cursor_height) {
        shadow = NewShadowMask(&edwmp_cam_pos, 0.0f, -1);
        if (shadow != 2000000.0f) {
            old_cursor_height = edwmp_cam_pos.y;
            cursor_terrain_height_diff = edwmp_cam_pos.y - shadow;
        }
    }
    edcamMove(pad);
    edcamGetPosAng(&edwmp_cam_pos, &edwmp_cam_ax, &edwmp_cam_ay);
    if (pad->paddata_db & 0x80) {
        edwmp_active_menu = edwmp_options_menu;
    }
    if (edwmp_goto_selected) {
        if (edwmp_selected != -1) {
            edcamSetPos(&Wumpa[edwmp_selected].pos0);
        }
        edwmp_goto_selected = 0;
    } else if (pad->paddata & 0x20) {
        if (pad->paddata_db & 0x20) {
            edwmp_goto_selected = 1;
        } else {
            if (pad->paddata_db & 0x1000) {
                edwmp_cam_pos.y += edwmp_move_step;
                edcamSetPos(&edwmp_cam_pos);
            }
            if (pad->paddata_db & 0x4000) {
                edwmp_cam_pos.y -= edwmp_move_step;
                edcamSetPos(&edwmp_cam_pos);
            }
        }
    } else {
        if (pad->paddata_db & 0x1000) {
            edwmp_cam_pos.x += NuTrigTable[angle] * edwmp_move_step;
            edwmp_cam_pos.z += NuTrigTable[(angle + 0x4000) & 0xFFFF] * edwmp_move_step;
            shadow = NewShadowMask(&edwmp_cam_pos, 0.0f, -1);
            if (shadow != 2000000.0f) {
                edwmp_cam_pos.y = shadow + cursor_terrain_height_diff;
            }
            edcamSetPos(&edwmp_cam_pos);
        }
        if (pad->paddata_db & 0x2000) {
            edwmp_cam_pos.x += NuTrigTable[(angle + 0x4000) & 0xFFFF] * edwmp_move_step;
            edwmp_cam_pos.z -= NuTrigTable[angle] * edwmp_move_step;
            shadow = NewShadowMask(&edwmp_cam_pos, 0.0f, -1);
            if (shadow != 2000000.0f) {
                edwmp_cam_pos.y = shadow + cursor_terrain_height_diff;
            }
            edcamSetPos(&edwmp_cam_pos);
        }
        if (pad->paddata_db & 0x4000) {
            edwmp_cam_pos.x -= NuTrigTable[angle] * edwmp_move_step;
            edwmp_cam_pos.z -= NuTrigTable[(angle + 0x4000) & 0xFFFF] * edwmp_move_step;
            shadow = NewShadowMask(&edwmp_cam_pos, 0.0f, -1);
            if (shadow != 2000000.0f) {
                edwmp_cam_pos.y = shadow + cursor_terrain_height_diff;
            }
            edcamSetPos(&edwmp_cam_pos);
        }
        if (pad->paddata_db & 0x8000) {
            edwmp_cam_pos.x -= NuTrigTable[(angle + 0x4000) & 0xFFFF] * edwmp_move_step;
            edwmp_cam_pos.z += NuTrigTable[angle] * edwmp_move_step;
            shadow = NewShadowMask(&edwmp_cam_pos, 0.0f, -1);
            if (shadow != 2000000.0f) {
                edwmp_cam_pos.y = shadow + cursor_terrain_height_diff;
            }
            edcamSetPos(&edwmp_cam_pos);
        }
    }
    if (VEHICLECONTROL == 2 ||
        (VEHICLECONTROL == 1 && player->obj.vehicle == 0x20)) {
        if (pos_START != 0) {
            edwmp_cam_pos.x = pos_START->x;
            edcamSetPos(&edwmp_cam_pos);
        }
    }
    if ((pad->paddata_db & 0x40) && edwmp_selected == -1 && WUMPACOUNT != 0x100 &&
        !edwmp_cam_in_crate) {
        Wumpa[WUMPACOUNT++].pos0 = edwmp_cam_pos;
        ResetWumpa();
    }
    if (pad->paddata_db & 0x10) {
        i = edwmp_selected;
        if (i != -1) {
            WUMPACOUNT--;
            for (; i < WUMPACOUNT; i++) {
                Wumpa[i].pos0 = Wumpa[i + 1].pos0;
            }
            ResetWumpa();
            edwmp_selected = -1;
        }
    }
}

s32 edwmpProc(struct nupad_s *pad)
{
    struct nuvec_s v;
    s32 i;
    f32 bestdist;
    f32 shadow = 0.0f;

    if (screendump) {
        return 0;
    }
    D_00631BF8 += 6;
    if (edwmp_active_menu) {
        eduiMenuProcess(edwmp_active_menu, pad);
        return 0;
    }
    edwmp_selected = -1;
    for (i = 0; i < WUMPACOUNT; i++) {
        f32 distsq;
        NuVecSub(&v, &edwmp_cam_pos, &Wumpa[i].pos0);
        distsq = v.x * v.x + v.y * v.y + v.z * v.z;
        if (distsq < 0.09f) {
            if (edwmp_selected == -1 || distsq < bestdist) {
                edwmp_selected = i;
                bestdist = distsq;
            }
        }
    }
    if (edwmp_selected != -1) {
        v = Wumpa[edwmp_selected].pos0;
    } else {
        v = edwmp_cam_pos;
    }
    shadow = NewShadowMask(&v, shadow, -1);
    if (shadow != 2000000.0f) {
        D_00633340 = v.y - shadow;
    } else {
        D_00633340 = 0.0f;
    }
    edwmp_cam_in_crate = InCrate(edwmp_cam_pos.x, edwmp_cam_pos.z, edwmp_cam_pos.y,
                                 edwmp_cam_pos.y, 0.0f) != 0;
    edwmpDoInput(pad);
    if (pad->paddata_db & 0x800) {
        return 1;
    }
    return 0;
}


void edwmpADD(void)
{
    Wumpa[WUMPACOUNT++].pos0 = edwmp_cam_pos;
    ResetWumpa();
}

void edwmpDELETE(void)
{
    s32 i = edwmp_selected;

    if (i != -1) {
        WUMPACOUNT--;
        for (; i < WUMPACOUNT; i++) {
            Wumpa[i].pos0 = Wumpa[i + 1].pos0;
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
