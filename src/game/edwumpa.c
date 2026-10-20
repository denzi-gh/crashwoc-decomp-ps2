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
#include "edtools/edmain.h"

extern struct wumpa_s Wumpa[];
extern s32 WUMPACOUNT;
extern s32 PLAYERCOUNT;
extern struct nuvec_s v000;
extern struct nuvec_s edwmp_cam_pos;
static f32 edwmp_ter_dy;
static s32 edwmp_in_crate;
static s32 edwmp_i_wumpa;

extern void *app_fnt;
extern s32 PANELOFF;
extern char tbuf[];
extern char **LDATA;
extern s32 screendump;

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
extern f32 edwmp_cam_ax;
extern f32 edwmp_cam_ay;
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
void edwmpInit(void);
void edwmpClose(void);
void edwmpEnter(void);
s32 edwmpProc(struct nupad_s *pad);
void edwmpRender(void);
void edwmpcbSaveWumpaData(void *menu);
void edwmpCbCancelOptMenu(void);

void *edwmp_active_menu = 0;
void *edwmp_options_menu = 0;
static s32 edwmp_count = 0;
static f32 edwmp_radius = 0.25f;
s32 wmp_start = 1;
f32 cursor_terrain_height_diff = 0.0f;
f32 old_cursor_height = 0.0f;

void edwmpDrawInfo(void)
{
    if (PANELOFF) {
        return;
    }
    NuFntScale(8, 12);
    NuFntSetPen(-1);
    strcpy(tbuf, LDATA[0]);
    strupr(tbuf);
    NuFntPrintEx(0x400, 0x100, 0, "%s.WMP", tbuf);
    NuFntPrintEx(0x400, 0x200, 0, "WUMPACOUNT:   %i/%i", WUMPACOUNT, 0x100);
    NuFntPrintEx(0x400, 0x300, 0, "HEIGHT:   %.2f", fptodp(edwmp_ter_dy));
    NuFntPrintEx(0x400, 0xD00, 0, "CURSOR:   X: %.2f   Y: %.2f   Z: %.2f",
                 fptodp(edwmp_cam_pos.x), fptodp(edwmp_cam_pos.y), fptodp(edwmp_cam_pos.z));
}

void edwmpDRAW(void)
{
    struct nuvec_s v[6];
    struct wumpa_s *wumpa;
    s32 i;

    if (edwmp_i_wumpa != -1) {
        wumpa = &Wumpa[edwmp_i_wumpa];
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
    if (edwmp_in_crate) {
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

inline void edwmpcbSaveWumpaData(void *menu)
{
    ResetWumpa();
    SaveWumpa();
    eduiCreateMessageMenu(menu, "Saved OK", 1);
}

inline void edwmpInit(void)
{
    struct eduistyle_s style = { { 0x80000000, 0x80FF0000 }, { 0x80808080, 0x80404040 } };

    edwmp_options_menu = eduiMenuCreate(0x46, 0x46, 0xB4, 0x12C, app_fnt,
                                        edwmpCbCancelOptMenu, "Options");
    if (edwmp_options_menu != 0) {
        eduiMenuAddItem(edwmp_options_menu,
                        eduiItemSelCreate(1, &style, 0, 0, edwmpcbSaveWumpaData,
                                          "Save Wumpa Data"));
    }
}

struct edmainreg_s edwmpdesc = {
    0, 0, "Wumpa Editor", edwmpInit, edwmpClose, edwmpEnter, {0}, 0,
    EDMAIN_BLOCKID('w', 'u', 'm', 'p'), edwmpProc, edwmpRender
};
struct nuvec_s edwmp_cam_pos = { 0.0f, 0.0f, 0.0f };
struct nuvec_s wmp_pos = { 0.0f, 0.0f, 0.0f };
s32 edwmp_used = 0;

void edwmpDoInput(struct nupad_s *pad)
{
    static s32 wumpa_lock_button = 0;
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
    if (wumpa_lock_button) {
        if (edwmp_i_wumpa != -1) {
            edcamSetPos(&Wumpa[edwmp_i_wumpa].pos0);
        }
        wumpa_lock_button = 0;
    } else if (pad->paddata & 0x20) {
        if (pad->paddata_db & 0x20) {
            wumpa_lock_button = 1;
        } else {
            if (pad->paddata_db & 0x1000) {
                edwmp_cam_pos.y += edwmp_radius;
                edcamSetPos(&edwmp_cam_pos);
            }
            if (pad->paddata_db & 0x4000) {
                edwmp_cam_pos.y -= edwmp_radius;
                edcamSetPos(&edwmp_cam_pos);
            }
        }
    } else {
        if (pad->paddata_db & 0x1000) {
            edwmp_cam_pos.x += NuTrigTable[angle] * edwmp_radius;
            edwmp_cam_pos.z += NuTrigTable[(angle + 0x4000) & 0xFFFF] * edwmp_radius;
            shadow = NewShadowMask(&edwmp_cam_pos, 0.0f, -1);
            if (shadow != 2000000.0f) {
                edwmp_cam_pos.y = shadow + cursor_terrain_height_diff;
            }
            edcamSetPos(&edwmp_cam_pos);
        }
        if (pad->paddata_db & 0x2000) {
            edwmp_cam_pos.x += NuTrigTable[(angle + 0x4000) & 0xFFFF] * edwmp_radius;
            edwmp_cam_pos.z -= NuTrigTable[angle] * edwmp_radius;
            shadow = NewShadowMask(&edwmp_cam_pos, 0.0f, -1);
            if (shadow != 2000000.0f) {
                edwmp_cam_pos.y = shadow + cursor_terrain_height_diff;
            }
            edcamSetPos(&edwmp_cam_pos);
        }
        if (pad->paddata_db & 0x4000) {
            edwmp_cam_pos.x -= NuTrigTable[angle] * edwmp_radius;
            edwmp_cam_pos.z -= NuTrigTable[(angle + 0x4000) & 0xFFFF] * edwmp_radius;
            shadow = NewShadowMask(&edwmp_cam_pos, 0.0f, -1);
            if (shadow != 2000000.0f) {
                edwmp_cam_pos.y = shadow + cursor_terrain_height_diff;
            }
            edcamSetPos(&edwmp_cam_pos);
        }
        if (pad->paddata_db & 0x8000) {
            edwmp_cam_pos.x -= NuTrigTable[(angle + 0x4000) & 0xFFFF] * edwmp_radius;
            edwmp_cam_pos.z += NuTrigTable[angle] * edwmp_radius;
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
    if ((pad->paddata_db & 0x40) && edwmp_i_wumpa == -1 && WUMPACOUNT != 0x100 &&
        !edwmp_in_crate) {
        Wumpa[WUMPACOUNT++].pos0 = edwmp_cam_pos;
        ResetWumpa();
    }
    if (pad->paddata_db & 0x10) {
        i = edwmp_i_wumpa;
        if (i != -1) {
            WUMPACOUNT--;
            for (; i < WUMPACOUNT; i++) {
                Wumpa[i].pos0 = Wumpa[i + 1].pos0;
            }
            ResetWumpa();
            edwmp_i_wumpa = -1;
        }
    }
}

f32 edwmp_cam_dist = 0.0f;
f32 edwmp_cam_ax = 0.0f;
f32 edwmp_cam_ay = 0.0f;

s32 edwmpProc(struct nupad_s *pad)
{
    struct nuvec_s v;
    s32 i;
    f32 bestdist;
    f32 shadow = 0.0f;

    if (screendump) {
        return 0;
    }
    edwmp_count += 6;
    if (edwmp_active_menu) {
        eduiMenuProcess(edwmp_active_menu, pad);
        return 0;
    }
    edwmp_i_wumpa = -1;
    for (i = 0; i < WUMPACOUNT; i++) {
        f32 distsq;
        NuVecSub(&v, &edwmp_cam_pos, &Wumpa[i].pos0);
        distsq = v.x * v.x + v.y * v.y + v.z * v.z;
        if (distsq < 0.09f) {
            if (edwmp_i_wumpa == -1 || distsq < bestdist) {
                edwmp_i_wumpa = i;
                bestdist = distsq;
            }
        }
    }
    if (edwmp_i_wumpa != -1) {
        v = Wumpa[edwmp_i_wumpa].pos0;
    } else {
        v = edwmp_cam_pos;
    }
    shadow = NewShadowMask(&v, shadow, -1);
    if (shadow != 2000000.0f) {
        edwmp_ter_dy = v.y - shadow;
    } else {
        edwmp_ter_dy = 0.0f;
    }
    edwmp_in_crate = InCrate(edwmp_cam_pos.x, edwmp_cam_pos.z, edwmp_cam_pos.y,
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
    s32 i = edwmp_i_wumpa;

    if (i != -1) {
        WUMPACOUNT--;
        for (; i < WUMPACOUNT; i++) {
            Wumpa[i].pos0 = Wumpa[i + 1].pos0;
        }
        ResetWumpa();
        edwmp_i_wumpa = -1;
    }
}

inline void edwmpClose(void)
{
    eduiMenuDestroy(edwmp_options_menu);
}

inline void edwmpEnter(void)
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

inline void edwmpRender(void)
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

inline void edwmpCbCancelOptMenu(void)
{
    edwmp_active_menu = 0;
}
