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

#include "creature.h"

struct pad_s {
    u8 unk_0x000[0x55C];
    u32 paddata;            /* 0x55C */
    u8 unk_0x560[0x4];
    u32 buttons;            /* 0x564 */
};

extern void *edai_active_menu;
extern s32 D_00631C3C;
extern s32 D_00631C38;
extern s32 D_00631C40;
extern s32 D_00631C44;
extern s32 D_00631C48;
extern void *edai_options_menu;
extern void *edai_selecttype_menu;
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
extern void eduiMenuRender(void *menu);
extern void eduiMenuProcess(void *menu, struct pad_s *pad);
extern s32 SaveAI(s32 arg);
extern void *eduiCreateMessageMenu(s32 arg, void *msg, s32 mode);
extern void edcamSet(void);
extern void edcamSetPosAng(struct nuvec_s *pos, s32 a, s32 b);
extern void DrawCameraTarget(struct nuvec_s *pos);
extern void NuFntScale(s32 x, s32 y);
extern f32 NewShadowMask(struct nuvec_s *pos, s32 flag, f32 y);
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

struct aitype_s {
    u8 unk_0x00[0x2];
    s16 count;               /* 0x2 */
    u8 unk_0x04[0x18];
};                           /* 0x1C */
extern struct aitype_s AIType[];

struct aihl_s {
    struct nuvec_s pos[8];   /* 0x00, stride 0x0C */
    u8 unk_0x60[0x20];
};                           /* 0x80 */
extern struct aihl_s D_005CA154[];

extern void NuVecSub(struct nuvec_s *dst, struct nuvec_s *a, struct nuvec_s *b);

extern s32 PANELOFF;
extern char tbuf[];
extern u8 D_00631C58[];
extern u8 D_00631C60[];
extern u8 D_00620378[];
extern u8 D_00620390[];
extern u8 D_006203A0[];
extern u8 D_006203B0[];

struct ldata_s {
    char *name;              /* 0x00 */
};
extern struct ldata_s *LDATA;

extern s32 VEHICLECONTROL;
extern struct nuvec_s *pos_START;
extern f32 edai_cam_ax;
extern f32 edai_cam_ay;
extern void edcamMove(void);
extern void edcamGetPosAng(struct nuvec_s *pos, f32 *ax, f32 *ay);
extern void AdjustPosY(struct nuvec_s *pos);

extern f32 D_0062E33C;
extern f32 D_0062E340;
extern f32 D_0062E344;
extern u64 fptodp(f32 x);
extern s32 dpcmp(u64 a, u64 b);
extern u64 dpsub(u64 a, u64 b);
extern f32 dptofp(u64 x);

extern void NuFntSetPen(s32 col);
extern void NuFntPrintEx(s32 x, s32 y, s32 flag, const char *fmt, ...);
extern char *strcpy(char *dst, const char *src);
extern char *strupr(char *s);

struct aitab_s {
    u8 ai_type;              /* 0x00 */
    u8 unk_0x01[0x13];
    struct nuvec_s pos[8];   /* 0x14 */
    struct nuvec_s origin;   /* 0x74 */
};                           /* 0x80 */
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

struct edmenu_s {
    u8 unk_0x00[0x8];
    void *sel_item;          /* 0x08 */
    u8 unk_0x0C[0x4];
    s32 x;                   /* 0x10 */
    s32 y;                   /* 0x14 */
};

struct eduiitem_s {
    u8 unk_0x00[0x10];
    u8 field_0x10;
};
extern struct eduiitem_s *edui_last_item;

extern void edcamSetPos(struct nuvec_s *pos);
extern void NuRndrLine3dDbg(s32 colour, f32 x0, f32 y0, f32 z0, f32 x1, f32 y1,
                            f32 z1);
extern void *eduiMenuCreate(s32 x, s32 y, s32 w, s32 h, void *font,
                            void *cancelcb, void *arg);
extern void *eduiItemSelCreate(s32 a0, void *tmpl, s32 a2, s32 a3, void *cb,
                               void *label);
extern void *eduiItemToggleCreate(s32 a0, void *tmpl, s32 a2, s32 a3, void *cb,
                                  void *label);
extern void eduiMenuAddItem(void *menu, void *item);
extern void *eduiItemCheckCreate(s32 a0, void *tmpl, s32 a2, s32 a3, void *cb,
                                 void *label);
extern void eduiMenuAttach(void *item, void *menu);

/* callbacks referenced by edaiInit / edaicbSelectType */
void edaiCbCancelOptMenu(void);
void edaicbSelectType(void *item);
void edaicbSaveAIData(s32 arg);
void edaicbTerrainToggle(s32 a0, u8 *item);
void edaicbSetType(s32 a0, u8 *item);
void edaiCbCancelSelectType(void);

/* forward decls for same-unit callees */
void edaiDRAW(void);
void edaiDrawInfo(void);
void edaiHIGHLIGHT(void);
void edaiDoInput(struct pad_s *pad);

void edaiHIGHLIGHT(void)
{
    s32 i;
    s32 j;
    f32 best;

    D_00631C4C = -1;
    for (i = 0; i < LEVELAICOUNT; i++) {
        struct aitype_s *t = &AIType[AITab[i].ai_type];
        s32 n = t->count;
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

void AdjustPosY(struct nuvec_s *obj)
{
    f32 best;
    f32 bestval;
    f32 v;
    s32 found;
    u64 d;

    if (D_00631C3C == 0) {
        return;
    }
    found = 0;
    if (D_0063334C != D_0062E33C) {
        bestval = D_0063334C;
        d = fptodp(bestval - obj->y);
        if (dpcmp(d, 0) < 0) {
            d = dpsub(0, d);
        }
        best = dptofp(d);
        found = 1;
    }
    v = D_00633350;
    if (v != D_0062E340) {
        if (found == 0) {
            bestval = v;
            found = 1;
        } else {
            d = fptodp(v - obj->y);
            if (dpcmp(d, 0) < 0) {
                d = dpsub(0, d);
            }
            if (dpcmp(d, fptodp(best)) < 0) {
                bestval = v;
                found = 1;
            }
        }
    }
    v = D_00633354;
    if (v != D_0062E344) {
        if (found == 0) {
            bestval = v;
            found = 1;
        } else {
            d = fptodp(v - obj->y);
            if (dpcmp(d, 0) < 0) {
                d = dpsub(0, d);
            }
            if (dpcmp(d, fptodp(best)) < 0) {
                bestval = v;
                found = 1;
            }
        }
    }
    if (found) {
        obj->y = bestval;
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

void edaicbSelectType(void *item)
{
    struct edai_itemdef_s buf;
    struct edmenu_s *menu;
    s32 i;

    buf = D_006203D8;
    edai_selecttype_menu = eduiMenuCreate(0x46, 0x46, 0xB4, 0xFA, app_fnt,
                                          edaiCbCancelSelectType, D_006203E8);
    if (edai_selecttype_menu != 0) {
        eduiMenuAddItem(edai_selecttype_menu,
                        eduiItemCheckCreate(-1, &buf, D_00631C40 == -1, 1,
                                            edaicbSetType, D_00631C78));
        for (i = 0; i < LEVELAITYPES; i++) {
            s32 sel = (D_00631C40 == i);
            eduiMenuAddItem(edai_selecttype_menu,
                            eduiItemCheckCreate(i, &buf, sel, 1,
                                                edaicbSetType,
                                                D_005C9520[LevelAIType[i]]));
            if (sel) {
                menu = (struct edmenu_s *)edai_selecttype_menu;
                menu->sel_item = edui_last_item;
            }
        }
        menu = (struct edmenu_s *)edai_selecttype_menu;
        menu->x = ((struct edmenu_s *)item)->x + 0xA;
        menu->y = ((struct edmenu_s *)item)->y + 0x28;
    }
}

void edaiDrawInfo(void)
{
    if (PANELOFF) {
        return;
    }
    NuFntScale(8, 12);
    NuFntSetPen(-1);
    strcpy(tbuf, LDATA->name);
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
                     t->count);
    }
    NuFntPrintEx(0x400, 0xD00, 0, (char *)D_006203B0, edai_cam_pos.x,
                 edai_cam_pos.y, edai_cam_pos.z);
}

void DrawCube(struct nuvec_s *pos, s32 ang, f32 len)
{
    f32 bx[4];
    f32 rx[4];
    f32 bz[4];
    f32 rz[4];
    f32 c;
    f32 s;
    s32 i;

    bx[0] = -len;
    bx[1] = -len;
    bx[2] = len;
    bx[3] = len;
    bz[0] = -len;
    bz[1] = len;
    bz[2] = len;
    bz[3] = -len;
    c = NuTrigTable[(ang + 0x4000) & 0xFFFF];
    s = NuTrigTable[ang & 0xFFFF];
    for (i = 0; i < 4; i++) {
        rx[i] = bx[i] * c + bz[i] * s;
        rz[i] = -(bx[i] * s) + bz[i] * c;
    }
    NuRndrLine3dDbg(0x7FFF7F, pos->x + rx[0], pos->y - len, pos->z + rz[0],
                    pos->x + rx[1], pos->y - len, pos->z + rz[1]);
    NuRndrLine3dDbg(0x7FFF7F, pos->x + rx[1], pos->y - len, pos->z + rz[1],
                    pos->x + rx[2], pos->y - len, pos->z + rz[2]);
    NuRndrLine3dDbg(0x7FFF7F, pos->x + rx[2], pos->y - len, pos->z + rz[2],
                    pos->x + rx[3], pos->y - len, pos->z + rz[3]);
    NuRndrLine3dDbg(0x7FFF7F, pos->x + rx[3], pos->y - len, pos->z + rz[3],
                    pos->x + rx[0], pos->y - len, pos->z + rz[0]);
    NuRndrLine3dDbg(0x7FFF7F, pos->x + rx[0], pos->y - len, pos->z + rz[0],
                    pos->x + rx[0], pos->y + len, pos->z + rz[0]);
    NuRndrLine3dDbg(0x7FFF7F, pos->x + rx[1], pos->y - len, pos->z + rz[1],
                    pos->x + rx[1], pos->y + len, pos->z + rz[1]);
    NuRndrLine3dDbg(0x7FFF7F, pos->x + rx[2], pos->y - len, pos->z + rz[2],
                    pos->x + rx[2], pos->y + len, pos->z + rz[2]);
    NuRndrLine3dDbg(0x7FFF7F, pos->x + rx[3], pos->y - len, pos->z + rz[3],
                    pos->x + rx[3], pos->y + len, pos->z + rz[3]);
    NuRndrLine3dDbg(0x7FFF7F, pos->x + rx[0], pos->y + len, pos->z + rz[0],
                    pos->x + rx[1], pos->y + len, pos->z + rz[1]);
    NuRndrLine3dDbg(0x7FFF7F, pos->x + rx[1], pos->y + len, pos->z + rz[1],
                    pos->x + rx[2], pos->y + len, pos->z + rz[2]);
    NuRndrLine3dDbg(0x7FFF7F, pos->x + rx[2], pos->y + len, pos->z + rz[2],
                    pos->x + rx[3], pos->y + len, pos->z + rz[3]);
    NuRndrLine3dDbg(0x7FFF7F, pos->x + rx[3], pos->y + len, pos->z + rz[3],
                    pos->x + rx[0], pos->y + len, pos->z + rz[0]);
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
        edui_last_item->field_0x10 = D_00631C3C;
    }
}

void edaiDoInput(struct pad_s *pad)
{
    s32 i;

    edcamMove();
    edcamGetPosAng(&edai_cam_pos, &edai_cam_ax, &edai_cam_ay);
    if (VEHICLECONTROL == 2 ||
        (VEHICLECONTROL == 1 && *(s16 *)((u8 *)player + 0x36) == 0x20)) {
        if (pos_START != 0) {
            edai_cam_pos.x = pos_START->x;
            edcamSetPos(&edai_cam_pos);
        }
    }
    if (pad->buttons & 0x80) {
        edai_active_menu = edai_options_menu;
    }
    if ((pad->buttons & 0x40) && LEVELAICOUNT != 0x60 && D_00631C40 >= 0) {
        AITab[LEVELAICOUNT].pos[D_00631C48] = edai_cam_pos;
        AdjustPosY(&AITab[LEVELAICOUNT].pos[D_00631C48]);
        D_00631C48++;
        if (D_00631C48 == AIType[D_00631C44].count) {
            AITab[LEVELAICOUNT].ai_type = D_00631C44;
            LEVELAICOUNT++;
            ResetAI();
            D_00631C48 = 0;
        }
    }
    if (pad->buttons & 0x10) {
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

void DrawAxes(struct nuvec_s *pos, s32 ang, s32 col, f32 len)
{
    f32 s;
    f32 c;

    NuRndrLine3dDbg(col, pos->x, pos->y - len, pos->z, pos->x, pos->y + len,
                    pos->z);
    s = NuTrigTable[ang & 0xFFFF] * len;
    c = NuTrigTable[(ang + 0x4000) & 0xFFFF] * len;
    NuRndrLine3dDbg(col, pos->x - s, pos->y, pos->z - c, pos->x + s, pos->y,
                    pos->z + c);
    NuRndrLine3dDbg(col, pos->x + c, pos->y, pos->z - s, pos->x - c, pos->y,
                    pos->z + s);
}

void edaiCbCancelSelectType(void)
{
    eduiMenuDestroy(edai_selecttype_menu);
    edai_selecttype_menu = 0;
}

void edaicbSetType(s32 a0, u8 *item)
{
    s32 t = *(s32 *)(item + 0xC);
    D_00631C48 = 0;
    D_00631C40 = t;
    D_00631C44 = LevelAIType[t];
}

void edaicbSaveAIData(s32 arg)
{
    SaveAI(arg);
    eduiCreateMessageMenu(arg, D_006203F8, 1);
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

s32 edaiProc(struct pad_s *pad)
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
        D_0063334C = NewShadowMask(&edai_cam_pos, -1, 0.0f);
        D_00633350 = EShadY;
        D_00633354 = ShadRoofY;
    } else {
        D_0063334C = D_0062E34C;
        D_00633350 = D_0062E34C;
        D_00633354 = D_0062E34C;
    }
    edaiDoInput(pad);
    if (pad->buttons & 0x800) {
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
