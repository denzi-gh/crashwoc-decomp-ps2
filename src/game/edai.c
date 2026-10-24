/*
 * Unit: game/edai
 *
 * Functions:
 *   0x00236260 edaiHIGHLIGHT
 *   0x002363c0 AdjustPosY
 *   0x00236570 edaiJUMP
 *   0x00236660 edaiDrawInfo
 *   0x00236800 DrawCube
 *   0x00236bc0 edaiDRAW
 *   0x00237418 edaicbSelectType
 *   0x002375c0 edaiInit
 *   0x002376c8 edaiDoInput
 *   0x002379b8 DrawAxes
 *   0x00237ab8 edaiCbCancelSelectType
 *   0x00237ae0 edaicbSetType
 *   0x00237b08 edaicbSaveAIData
 *   0x00237b40 edaicbTerrainToggle
 *   0x00237b50 edaiCbCancelOptMenu
 *   0x00237b58 edaiClose
 *   0x00237b78 edaiEnter
 *   0x00237c40 edaiProc
 *   0x00237d00 edaiRender
 */

#include "nu_libc.h"

#include "numath/numtx.h"
#include "numath/nutrig.h"
#include "numath/nuvec.h"
#include "game/gametypes.h"
#include "gamelib/terrain.h"
#include "game/creature.h"
#include "nups2/ps2ctl.h"
#include "edtools/edui.h"

extern void *edai_active_menu;
extern s32 D_00631C3C;
extern s32 D_00631C38;
extern s32 D_00631C40;
extern s32 D_00631C44;
extern s32 D_00631C48;
extern void *edai_options_menu;
extern struct eduimenu_s *edai_selecttype_menu;
extern struct nuvec_s edai_cam_pos;
extern u8 LevelAIType[];
extern u8 D_006203F8[];
extern s32 screendump;
extern s32 edai_used;
extern s32 LEVELAITYPES;
extern s32 PLAYERCOUNT;
extern struct nuvec_s v000;
extern f32 EShadY;
extern f32 ShadRoofY;
extern f32 D_0062E34C;
extern f32 D_0063334C;
extern f32 D_00633350;
extern f32 D_00633354;

extern void eduiMenuDestroy(void *menu);
extern void eduiMenuProcess(void *menu, struct nupad_s *pad);
extern void SaveAI(void);
extern void edcamSet(void);
extern void edcamSetPosAng(struct nuvec_s *pos, s32 a, s32 b);
extern void DrawCameraTarget(struct nuvec_s *pos);
extern void NuFntScale(s32 x, s32 y);
extern void ResetAI(void);
extern void ResetTimeTrial(void);
extern void ResetCheckpoint(s32 a, s32 b, s32 c, f32 d);
extern void ResetBonus(void);
extern void ResetDeath(void);
extern void ResetGemPath(void);
extern void RestoreCrateTypeData(void);
extern void ResetCrates(void);
extern void ResetWumpa(void);

extern s32 LEVELAICOUNT;
extern s32 D_00631C4C;
extern s32 D_00631C50;

extern struct aitype_s AIType[];

extern s32 PANELOFF;
extern char tbuf[];
extern u8 D_00631C58[];
extern u8 D_00631C60[];
extern u8 D_00620378[];
extern u8 D_00620390[];
extern u8 D_006203A0[];
extern u8 D_006203B0[];

extern struct ldata_s *LDATA;

extern s32 VEHICLECONTROL;
extern struct nuvec_s *pos_START;
extern f32 edai_cam_ax;
extern f32 edai_cam_ay;
extern void edcamMove(struct nupad_s *pad);
extern void edcamGetPosAng(struct nuvec_s *pos, f32 *ax, f32 *ay);
extern void AdjustPosY(struct nuvec_s *pos);


extern void NuFntSetPen(s32 col);
extern void NuFntPrintEx(s32 x, s32 y, s32 flag, const char *fmt, ...);
extern char *strupr(char *s);

extern struct aitab_s AITab[];
extern f32 NuTrigTable[];
extern void *app_fnt;
extern u8 D_00631C80[];

struct edai_itemdef_s {
    u8 data[16];
};
extern struct edai_itemdef_s D_006203D8;
extern u8 D_006203E8[];
extern u8 D_00620408[];
extern u8 D_00620418[];
extern u8 D_00631C78[];
extern u8 D_005C9520[][0x1C];


extern void edcamSetPos(struct nuvec_s *pos);

extern struct gtimer_s GlobalTimer;
extern struct numtx_s mTEMP;
extern s32 D_005CD1D0[8];

extern void NuHGobjRndr(struct NUHGOBJ_s *hobj, struct numtx_s *m, s32 nlayers,
                        s16 *layer);
extern void NuRndrLine3dDbg(f32 x0, f32 y0, f32 z0, f32 x1, f32 y1, f32 z1,
                            u32 colour);
extern void *eduiMenuCreate(s32 x, s32 y, s32 w, s32 h, void *font,
                            void *cancelcb, void *arg);
extern void *eduiItemSelCreate(s32 a0, void *tmpl, s32 a2, s32 a3, void *cb,
                               void *label);
extern void *eduiItemToggleCreate(s32 a0, void *tmpl, s32 a2, s32 a3, void *cb,
                                  void *label);
extern void *eduiMenuAddItem(void *menu, void *item);
extern void *eduiItemCheckCreate(u32 value, void *style, s32 checked,
                                 s32 group, void *cb, void *label);
extern s32 eduiMenuAttach(void *menu, void *child);

/* callbacks referenced by edaiInit / edaicbSelectType */
void edaiCbCancelOptMenu(void);
void edaicbSelectType(struct eduimenu_s *parent);
void edaicbSaveAIData(struct eduimenu_s *menu);
void edaicbTerrainToggle(s32 a0, u8 *item);
void edaicbSetType(struct eduimenu_s *menu, struct eduiitem_s *item);
void edaiCbCancelSelectType(void);

/* forward decls for same-unit callees */
void edaiDRAW(void);
void edaiDrawInfo(void);
void edaiHIGHLIGHT(void);
void edaiDoInput(struct nupad_s *pad);

void edaiHIGHLIGHT(void)
{
    s32 i;
    s32 j;
    f32 best;

    D_00631C4C = -1;
    for (i = 0; i < LEVELAICOUNT; i++) {
        struct aitype_s *t = &AIType[AITab[i].ai_type];
        s32 n = t->points;
        for (j = 0; j < n; j++) {
            struct nuvec_s d;
            f32 distsq;

            NuVecSub(&d, &edai_cam_pos, &AITab[i].pos[j]);
            distsq = d.x * d.x + d.y * d.y + d.z * d.z;
            if (distsq < 0.25f) {
                if (D_00631C4C == -1 || distsq < best) {
                    D_00631C4C = i;
                    best = distsq;
                    D_00631C50 = j;
                }
            }
        }
    }
}

void AdjustPosY(struct nuvec_s *pos)
{
    f32 best;
    f32 y;
    s32 found;
    double d;

    if (D_00631C3C == 0) {
        return;
    }
    found = 0;
    if (D_0063334C != 2000000.0f) {
        y = D_0063334C;
        d = y - pos->y;
        if (d < 0.0) {
            d = 0.0 - d;
        }
        best = d;
        found = 1;
    }
    if (D_00633350 != 2000000.0f) {
        if (found) {
            d = D_00633350 - pos->y;
            if (d < 0.0) {
                d = 0.0 - d;
            }
        }
        if (!found || d < best) {
            y = D_00633350;
            found = 1;
        }
    }
    if (D_00633354 != 2000000.0f) {
        if (found) {
            d = D_00633354 - pos->y;
            if (d < 0.0) {
                d = 0.0 - d;
            }
        }
        if (!found || d < best) {
            y = D_00633354;
            found = 1;
        }
    }
    if (found) {
        pos->y = y;
    }
}

void edaiJUMP(void)
{
    s32 i;
    s32 best;
    f32 bestdist;

    if (D_00631C48 > 0) {
        return;
    }
    best = -1;
    for (i = 0; i < LEVELAICOUNT; i++) {
        f32 dx = AITab[i].pos[0].x - edai_cam_pos.x;
        f32 dy = AITab[i].pos[0].y - edai_cam_pos.y;
        f32 dz = AITab[i].pos[0].z - edai_cam_pos.z;
        f32 d = dx * dx + dy * dy + dz * dz;
        if (best == -1 || d < bestdist) {
            bestdist = d;
            best = i;
        }
    }
    if (best != -1) {
        edai_cam_pos = AITab[best].pos[0];
        edcamSetPos(&edai_cam_pos);
    }
}

void edaicbSelectType(struct eduimenu_s *parent)
{
    struct edai_itemdef_s buf;
    s32 i;
    s32 sel;

    buf = D_006203D8;
    edai_selecttype_menu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt,
                                          edaiCbCancelSelectType, D_006203E8);
    if (edai_selecttype_menu != 0) {
        eduiMenuAddItem(edai_selecttype_menu,
                        eduiItemCheckCreate(-1, &buf, D_00631C40 == -1, 1,
                                            edaicbSetType, D_00631C78));
        for (i = 0; i < LEVELAITYPES; i++) {
            sel = (D_00631C40 == i);
            eduiMenuAddItem(edai_selecttype_menu,
                            eduiItemCheckCreate(i, &buf, sel, 1,
                                                edaicbSetType,
                                                D_005C9520[LevelAIType[i]]));
            if (sel == 1) {
                edai_selecttype_menu->current = edui_last_item;
            }
        }
        eduiMenuAttach(parent, edai_selecttype_menu);
        edai_selecttype_menu->x = parent->x + 10;
        edai_selecttype_menu->y = parent->y + 40;
    }
}

void edaiDrawInfo(void)
{
    if (PANELOFF) {
        return;
    }
    NuFntScale(8, 12);
    NuFntSetPen(-1);
    strcpy(tbuf, LDATA->path);
    strupr(tbuf);
    NuFntPrintEx(0x400, 0x100, 0, (char *)D_00631C58, tbuf);
    NuFntPrintEx(0x400, 0x200, 0, (char *)D_00620378, LEVELAICOUNT, 0x60);
    strcpy(tbuf, (D_00631C40 >= 0) ? (char *)D_005C9520[D_00631C44]
                                   : (char *)D_00631C60);
    strupr(tbuf);
    NuFntPrintEx(0x400, 0x300, 0, (char *)D_00620390, tbuf);
    if (D_00631C40 >= 0) {
        struct aitype_s *t = &AIType[D_00631C44];
        NuFntPrintEx(0x400, 0x400, 0, (char *)D_006203A0, D_00631C48 + 1,
                     t->points);
    }
    NuFntPrintEx(0x400, 0xD00, 0, (char *)D_006203B0, edai_cam_pos.x,
                 edai_cam_pos.y, edai_cam_pos.z);
}

void DrawCube(struct nuvec_s *pos, s32 ang, f32 len)
{
    f32 bx[4] = {-len, -len, len, len};
    f32 rx[4];
    f32 bz[4] = {-len, len, len, -len};
    f32 rz[4];
    f32 c = NuTrigTable[(ang + 0x4000) & 0xFFFF];
    f32 s = NuTrigTable[ang & 0xFFFF];
    s32 i;

    for (i = 0; i < 4; i++) {
        rx[i] = bx[i] * c + bz[i] * s;
        rz[i] = -(bx[i] * s) + bz[i] * c;
    }
    NuRndrLine3dDbg(pos->x + rx[0], pos->y - len, pos->z + rz[0],
                    pos->x + rx[1], pos->y - len, pos->z + rz[1], 0x7FFF7F);
    NuRndrLine3dDbg(pos->x + rx[1], pos->y - len, pos->z + rz[1],
                    pos->x + rx[2], pos->y - len, pos->z + rz[2], 0x7FFF7F);
    NuRndrLine3dDbg(pos->x + rx[2], pos->y - len, pos->z + rz[2],
                    pos->x + rx[3], pos->y - len, pos->z + rz[3], 0x7FFF7F);
    NuRndrLine3dDbg(pos->x + rx[3], pos->y - len, pos->z + rz[3],
                    pos->x + rx[0], pos->y - len, pos->z + rz[0], 0x7FFF7F);
    NuRndrLine3dDbg(pos->x + rx[0], pos->y - len, pos->z + rz[0],
                    pos->x + rx[0], pos->y + len, pos->z + rz[0], 0x7FFF7F);
    NuRndrLine3dDbg(pos->x + rx[1], pos->y - len, pos->z + rz[1],
                    pos->x + rx[1], pos->y + len, pos->z + rz[1], 0x7FFF7F);
    NuRndrLine3dDbg(pos->x + rx[2], pos->y - len, pos->z + rz[2],
                    pos->x + rx[2], pos->y + len, pos->z + rz[2], 0x7FFF7F);
    NuRndrLine3dDbg(pos->x + rx[3], pos->y - len, pos->z + rz[3],
                    pos->x + rx[3], pos->y + len, pos->z + rz[3], 0x7FFF7F);
    NuRndrLine3dDbg(pos->x + rx[0], pos->y + len, pos->z + rz[0],
                    pos->x + rx[1], pos->y + len, pos->z + rz[1], 0x7FFF7F);
    NuRndrLine3dDbg(pos->x + rx[1], pos->y + len, pos->z + rz[1],
                    pos->x + rx[2], pos->y + len, pos->z + rz[2], 0x7FFF7F);
    NuRndrLine3dDbg(pos->x + rx[2], pos->y + len, pos->z + rz[2],
                    pos->x + rx[3], pos->y + len, pos->z + rz[3], 0x7FFF7F);
    NuRndrLine3dDbg(pos->x + rx[3], pos->y + len, pos->z + rz[3],
                    pos->x + rx[0], pos->y + len, pos->z + rz[0], 0x7FFF7F);
}

inline void DrawAxes(struct nuvec_s *pos, s32 ang, s32 col, f32 len)
{
    f32 s;
    f32 c;

    NuRndrLine3dDbg(pos->x, pos->y - len, pos->z, pos->x, pos->y + len,
                    pos->z, col);
    s = NuTrigTable[ang & 0xFFFF] * len;
    c = NuTrigTable[(ang + 0x4000) & 0xFFFF] * len;
    NuRndrLine3dDbg(pos->x - s, pos->y, pos->z - c, pos->x + s, pos->y,
                    pos->z + c, col);
    NuRndrLine3dDbg(pos->x + c, pos->y, pos->z - s, pos->x - c, pos->y,
                    pos->z + s, col);
}


void edaiDRAW(void)
{
    struct nuvec_s v;
    struct nuvec_s *pos;
    u16 ang;
    s32 i;
    s32 j;
    s32 count;
    s32 character;
    struct aitype_s *type;
    struct nuvec_s *next;

    ang = ((GlobalTimer.frame % 200) << 16) / 200;
    for (i = 0; i < LEVELAICOUNT; i++) {
        type = &AIType[AITab[i].ai_type];
        count = type->points;
        for (j = 0; j < count; j++) {
            pos = &AITab[i].pos[j];
            if (j == 0) {
                character = AIType[AITab[i].ai_type].character;
                v.x = v.y = v.z = CData[character].scale;
                NuMtxSetScale(&mTEMP, &v);
                NuMtxRotateY(&mTEMP,
                             (count != 1)
                                 ? NuAtan2D(pos[1].x - pos[0].x,
                                            pos[1].z - pos[0].z) + 0x8000
                                 : 0x8000);
                NuMtxTranslate(&mTEMP, pos);
                if (CRemap[character] != -1) {
                    NuHGobjRndr(CModel[CRemap[character]].obj, &mTEMP, 1, 0);
                }
            }
            DrawAxes(pos, ang, D_005CD1D0[j], 0.125f);
            if (count > 1) {
                if (j < count - 1) {
                    next = pos + 1;
                    NuRndrLine3dDbg(pos->x, pos->y, pos->z,
                                    next->x, next->y, next->z, D_005CD1D0[j]);
                }
            }
        }
    }
    if (D_00631C48 != 0) {
        for (j = 0; j < D_00631C48; j++) {
            pos = &AITab[i].pos[j];
            DrawAxes(pos, ang, D_005CD1D0[j], 0.125f);
        }
        next = &edai_cam_pos;
        NuRndrLine3dDbg(pos->x, pos->y, pos->z,
                        next->x, next->y, next->z, D_005CD1D0[j - 1]);
        pos = AITab[i].pos;
    } else {
        pos = &edai_cam_pos;
    }
    if (D_00631C4C != -1) {
        DrawCube(&AITab[D_00631C4C].pos[D_00631C50], ang, 0.125f);
    } else if (D_00631C40 >= 0 && GlobalTimer.frame % 50 < 25) {
        character = AIType[D_00631C44].character;
        v.x = v.y = v.z = CData[character].scale * 0.5f;
        NuMtxSetScale(&mTEMP, &v);
        NuMtxRotateY(&mTEMP,
                     (D_00631C48 == 0)
                         ? ang + 0x8000
                         : NuAtan2D(edai_cam_pos.x - pos->x,
                                    edai_cam_pos.z - pos->z) + 0x8000);
        NuMtxTranslate(&mTEMP, pos);
        if (CRemap[character] != -1) {
            NuHGobjRndr(CModel[CRemap[character]].obj, &mTEMP, 1, 0);
        }
    }
    if (D_00631C3C != 0) {
        v.x = edai_cam_pos.x;
        v.z = edai_cam_pos.z;
        if (D_0063334C != 2000000.0f) {
            v.y = D_0063334C;
            DrawAxes(&v, 0x2000, 0xFF00FF, 0.125f);
        }
        if (D_00633350 != 2000000.0f) {
            v.y = D_00633350;
            DrawAxes(&v, 0x2000, 0xFF00FF, 0.125f);
        }
        if (D_00633354 != 2000000.0f) {
            v.y = D_00633354;
            DrawAxes(&v, 0x2000, 0xFF00FF, 0.125f);
        }
    }
}

void edaiInit(void)
{
    struct edai_itemdef_s buf;

    buf = D_006203D8;
    edai_options_menu = eduiMenuCreate(0x46, 0x46, 0xB4, 0x12C, app_fnt,
                                       edaiCbCancelOptMenu, D_00631C80);
    if (edai_options_menu != 0) {
        eduiMenuAddItem(edai_options_menu,
                        eduiItemSelCreate(1, &buf, 0, 0, edaicbSelectType,
                                          D_006203E8));
        eduiMenuAddItem(edai_options_menu,
                        eduiItemSelCreate(1, &buf, 0, 0, edaicbSaveAIData,
                                          D_00620408));
        eduiMenuAddItem(edai_options_menu,
                        eduiItemToggleCreate(1, &buf, 0, 1, edaicbTerrainToggle,
                                             D_00620418));
        edui_last_item->on = D_00631C3C;
    }
}

void edaiDoInput(struct nupad_s *pad)
{
    s32 i;
    struct aitype_s *type;

    edcamMove(pad);
    edcamGetPosAng(&edai_cam_pos, &edai_cam_ax, &edai_cam_ay);
    if (VEHICLECONTROL == 2 ||
        (VEHICLECONTROL == 1 && player->obj.vehicle == 0x20)) {
        if (pos_START != 0) {
            edai_cam_pos.x = pos_START->x;
            edcamSetPos(&edai_cam_pos);
        }
    }
    if (pad->paddata_db & 0x80) {
        edai_active_menu = edai_options_menu;
    }
    if ((pad->paddata_db & 0x40) && LEVELAICOUNT != 0x60 && D_00631C40 >= 0) {
        AITab[LEVELAICOUNT].pos[D_00631C48] = edai_cam_pos;
        AdjustPosY(&AITab[LEVELAICOUNT].pos[D_00631C48]);
        type = &AIType[D_00631C44];
        if (++D_00631C48 == type->points) {
            AITab[LEVELAICOUNT].ai_type = D_00631C44;
            LEVELAICOUNT++;
            ResetAI();
            D_00631C48 = 0;
        }
    }
    if (pad->paddata_db & 0x10) {
        if (D_00631C4C != -1) {
            LEVELAICOUNT--;
            if (D_00631C4C < LEVELAICOUNT) {
                for (i = D_00631C4C; i < LEVELAICOUNT; i++) {
                    AITab[i] = AITab[i + 1];
                }
            }
            ResetAI();
            D_00631C4C = -1;
        }
    }
    if (pad->paddata & 0x20) {
        if (D_00631C4C != -1) {
            AITab[D_00631C4C].pos[D_00631C50] = edai_cam_pos;
            AdjustPosY(&AITab[D_00631C4C].pos[D_00631C50]);
        }
    }
    if (pad->paddata & 0x100) {
        edaiJUMP();
    }
}

void edaiCbCancelSelectType(void)
{
    eduiMenuDestroy(edai_selecttype_menu);
    edai_selecttype_menu = 0;
}

void edaicbSetType(struct eduimenu_s *menu, struct eduiitem_s *item)
{
    s32 t = item->value;
    D_00631C48 = 0;
    D_00631C40 = t;
    D_00631C44 = LevelAIType[t];
}

void edaicbSaveAIData(struct eduimenu_s *menu)
{
    SaveAI();
    eduiCreateMessageMenu(menu, D_006203F8, 1);
}

void edaicbTerrainToggle(s32 a0, u8 *item)
{
    D_00631C3C = item[0x10];
}

void edaiCbCancelOptMenu(void)
{
    edai_active_menu = 0;
}

void edaiClose(void)
{
    eduiMenuDestroy(edai_options_menu);
}

void edaiEnter(void)
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
    if (D_00631C40 >= LEVELAITYPES) {
        D_00631C40 = -1;
    }
    if (edai_used == 0) {
        if (PLAYERCOUNT != 0) {
            pos = &player->obj.pos;
        } else {
            pos = &v000;
        }
        edcamSetPosAng(pos, 0, 0);
        edai_used = 1;
    }
    D_00631C48 = 0;
}

s32 edaiProc(struct nupad_s *pad)
{
    if (screendump) {
        return 0;
    }
    D_00631C38 += 6;
    if (edai_active_menu) {
        eduiMenuProcess(edai_active_menu, pad);
        return 0;
    }
    edaiHIGHLIGHT();
    if (D_00631C3C != 0) {
        D_0063334C = NewShadowMask(&edai_cam_pos, 0.0f, -1);
        D_00633350 = EShadY;
        D_00633354 = ShadRoofY;
    } else {
        D_0063334C = D_0062E34C;
        D_00633350 = D_0062E34C;
        D_00633354 = D_0062E34C;
    }
    edaiDoInput(pad);
    if (pad->paddata_db & 0x800) {
        ResetAI();
        return 1;
    }
    return 0;
}

void edaiRender(void)
{
    edcamSet();
    DrawCameraTarget(&edai_cam_pos);
    edaiDRAW();
    edaiDrawInfo();
    if (edai_active_menu) {
        NuFntScale(12, 12);
        eduiMenuRender(edai_active_menu);
    }
}
