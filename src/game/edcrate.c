/*
 * Unit: game/edcrate
 *
 * Functions:
 *   0x0022c430 edcrtDrawSpecialCrates
 *   0x0022c6b8 edcrtDrawBoxyThing
 *   0x0022caf0 WriteCrateData
 *   0x0022cd68 edcrtcbMemCardSaveCrates
 *   0x0022d128 edcrtcbMemCardLoadCrates
 *   0x0022d500 edcrtcbMemCardMenu
 *   0x0022d7d0 edcrtcbSpecificTypeMenu
 *   0x0022d978 edcrtcbCrateTypesMenu
 *   0x0022dae0 edcrtcbDisplayTypeMenu
 *   0x0022dcc8 edcrtInit
 *   0x0022df08 edcrtDoInput
 *   0x0022e368 edcrtDrawInfo
 *   0x0022e520 edcrtcbMemCardLoadCratesMenu
 *   0x0022e670 edcrtcbChangeFileNameMenu
 *   0x0022e758 edcrtcbMemCardDeleteCratesMenu
 *   0x0022e8a8 edcrtcbCancelMemCardMenu
 *   0x0022e8d0 edcrtCbSaveCrateData
 *   0x0022e918 edcrtcbSetCrateType
 *   0x0022e978 edcrtcbCancelSpecificTypeMenu
 *   0x0022e9a0 edcrtcbCancelCrateTypesMenu
 *   0x0022e9c8 edcrtcbResetCratesMenu
 *   0x0022eb00 edcrtcbDisplayType
 *   0x0022eb38 edcrtcbCancelDisplayTypeMenu
 *   0x0022eb60 edcrtcbTerrainToggle
 *   0x0022eb70 edcrtcbShowTriggersToggle
 *   0x0022eb80 edcrtCbCancelOptMenu
 *   0x0022eb88 edcrtClose
 *   0x0022eba8 edcrtEnter
 *   0x0022ec60 edcrtProc
 *   0x0022ecc0 edcrtRender
 *   0x0022ed28 edcrtcbMemCardCancelLoadEffectsMenu
 *   0x0022ed50 edcrtcbChangeFileName
 *   0x0022ed78 edcrtcbCancelChangeFileNameMenu
 *   0x0022eda0 edcrtcbMemCardDeleteCrates
 *   0x0022ee60 edcrtcbMemCardCancelDeleteEffectsMenu
 *   0x0022ee88 edcrtcbResetCrates
 *   0x0022eee8 edcrtcbCancelResetCratesMenu
 */

#include "creature.h"

/* Editor menu control (2nd callback arg is a menu-item record). */
struct edcrtsub_s {
    u8 pad00[0xC];
    s8 *base;                  /* 0x0C */
};
struct edcrtctx_s {
    u8 pad00[0x8];
    struct edcrtsub_s *sub;    /* 0x08 */
};
struct edmenu_s {
    u8 pad00[0x30];
    struct edcrtctx_s *ctx;    /* 0x30 */
};

/* Menu-item argument passed as the 2nd callback parameter. */
struct edcrtitem_s {
    u8 pad00[0xC];
    s32 newtype;               /* 0x0C */
    u8 toggle;                 /* 0x10 */
};
struct edcrttypeitem_s {
    u8 pad00[0xC];
    u8 type;                   /* 0x0C */
};
struct edcrtnameitem_s {
    u8 pad00[0x44];
    char name[1];              /* 0x44 */
};

extern void *edcrt_active_menu;
extern void *edcrt_options_menu;
extern void *edcrt_memcard_menu;
extern void *edcrt_specifictype_menu;
extern void *edcrt_cratetypes_menu;
extern void *edcrt_displaytype_menu;
extern void *edcrt_loadeffects_menu;
extern void *edcrt_filename_menu;
extern void *edcrt_deleteeffects_menu;
extern void *edcrt_resetcrates_menu;

extern s32 edcrtDrawType;
extern s32 D_00631ADC;
extern s32 D_00631AE0;
extern s32 CRATEGROUPCOUNT;
extern s32 highlighted_crate;

extern char D_005C1800[];       /* filename buffer (far) */
extern char D_0061FCB0[];       /* "saved" message (far) */

extern void eduiMenuDestroy(void *menu);
extern void eduiMenuDetach(void *menu);
extern void eduiCreateMessageMenu(void *menu, char *msg, s32 flag);
extern char *strcpy(char *dst, const char *src);
extern void ConvertCrateData(void);
extern void WriteCrateData(void);
extern void ResetCrates(void);
extern void ResetAllCrateTypes(s32 arg);
extern void ResetAllCrates(void);
extern void ReadInCrateData(void);

void edcrtCbSaveCrateData(void *menu)
{
    ConvertCrateData();
    WriteCrateData();
    ResetCrates();
    eduiCreateMessageMenu(menu, D_0061FCB0, 1);
}

void edcrtcbSetCrateType(struct edmenu_s *menu, struct edcrttypeitem_s *item)
{
    s8 *p = menu->ctx->sub->base + highlighted_crate;

    p[3] = item->type;
    ConvertCrateData();
    ResetCrates();
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
    edcrt_specifictype_menu = 0;
}

void edcrtcbCancelSpecificTypeMenu(void)
{
    eduiMenuDestroy(edcrt_specifictype_menu);
    edcrt_specifictype_menu = 0;
}

void edcrtcbCancelCrateTypesMenu(void)
{
    eduiMenuDestroy(edcrt_cratetypes_menu);
    edcrt_cratetypes_menu = 0;
}

void edcrtcbDisplayType(struct edmenu_s *menu, struct edcrtitem_s *item)
{
    edcrtDrawType = item->newtype;
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
    edcrt_specifictype_menu = 0;
}

void edcrtcbCancelDisplayTypeMenu(void)
{
    eduiMenuDestroy(edcrt_displaytype_menu);
    edcrt_displaytype_menu = 0;
}

void edcrtcbTerrainToggle(void *menu, struct edcrtitem_s *item)
{
    D_00631ADC = item->toggle;
}

void edcrtcbShowTriggersToggle(void *menu, struct edcrtitem_s *item)
{
    D_00631AE0 = item->toggle;
}

void edcrtCbCancelOptMenu(void)
{
    edcrt_active_menu = 0;
}

void edcrtClose(void)
{
    eduiMenuDestroy(edcrt_options_menu);
}

void edcrtcbMemCardCancelLoadEffectsMenu(void)
{
    eduiMenuDestroy(edcrt_loadeffects_menu);
    edcrt_loadeffects_menu = 0;
}

void edcrtcbChangeFileName(void *menu, struct edcrtnameitem_s *item)
{
    strcpy(D_005C1800, item->name);
}

void edcrtcbCancelChangeFileNameMenu(void)
{
    eduiMenuDestroy(edcrt_filename_menu);
    edcrt_filename_menu = 0;
}

void edcrtcbMemCardCancelDeleteEffectsMenu(void)
{
    eduiMenuDestroy(edcrt_deleteeffects_menu);
    edcrt_deleteeffects_menu = 0;
}

void edcrtcbResetCrates(struct edmenu_s *menu, struct edcrtitem_s *item)
{
    s32 saved;

    ResetAllCrateTypes(item->newtype - 1);
    saved = CRATEGROUPCOUNT;
    ResetAllCrates();
    CRATEGROUPCOUNT = saved;
    ReadInCrateData();
    ResetCrates();
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
    edcrt_specifictype_menu = 0;
}

void edcrtcbCancelResetCratesMenu(void)
{
    eduiMenuDestroy(edcrt_resetcrates_menu);
    edcrt_resetcrates_menu = 0;
}

void edcrtcbCancelMemCardMenu(void)
{
    eduiMenuDestroy(edcrt_memcard_menu);
    edcrt_memcard_menu = 0;
}

/* ------------------------------------------------------------------ */

/* edmenu with detach callback (cb @0x24, arg @0x30). */
struct edmenucb_s {
    u8 pad00[0x24];
    void (*cb)(void *menu, void *arg);  /* 0x24 */
    u8 pad28[0x8];
    void *arg;                          /* 0x30 */
};

/* Game-loop object carrying an input-flag word at 0x564. */
struct edproc_s {
    u8 pad00[0x564];
    s32 flags;                          /* 0x564 */
};

extern s32 screendump;
extern s32 D_00631AD8;
extern s32 D_006332F8;
extern s32 edcrt_used;
extern s32 PLAYERCOUNT;
extern struct nuvec_s v000;
extern struct nuvec_s edcrt_cam_pos;

extern char D_005C18A0[];
extern char D_0061FD10[];
extern char D_0061FD28[];

extern s32 eduiMenuProcess(void *menu, void *arg);
extern void eduiMenuRender(void *menu);
extern void NuFntScale(s32 x, s32 y);
extern void edcrtDoInput(void *arg);
extern void edcrtDrawInfo(void);
extern void edcrtDrawBoxyThing(struct nuvec_s *pos);
extern void edcamSet(void);
extern void edcamSetPosAng(struct nuvec_s *pos, s32 a, s32 b);
extern void *FindLocalCrate(struct nuvec_s *pos);
extern void DrawCameraTarget(struct nuvec_s *pos);
extern void ResetTimeTrial(void);
extern void ResetCheckpoint(s32 a, s32 b, s32 c, f32 d);
extern void ResetBonus(void);
extern void ResetDeath(void);
extern void ResetGemPath(void);
extern void RestoreCrateTypeData(void);
extern void ResetWumpa(void);
extern void ResetAI(void);
extern s32 sceMcDelete(s32 port, s32 slot, char *name);
extern s32 sceMcSync(s32 mode, s32 *cmd, s32 *result);

void edcrtEnter(void)
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
    ConvertCrateData();
    ResetCrates();
    if (!edcrt_used) {
        struct nuvec_s *pos;

        if (PLAYERCOUNT != 0)
            pos = (struct nuvec_s *)((u8 *)player + 0x6C);
        else
            pos = &v000;
        edcamSetPosAng(pos, 0, 0);
        edcrt_used = 1;
    }
}

s32 edcrtProc(struct edproc_s *obj)
{
    if (screendump)
        return 0;
    D_00631AD8 += 6;
    if (edcrt_active_menu) {
        eduiMenuProcess(edcrt_active_menu, obj);
        return 0;
    }
    edcrtDoInput(obj);
    {
        s32 f = obj->flags & 0x800;
        return f != 0;
    }
}

void edcrtRender(void)
{
    edcrtDrawInfo();
    edcamSet();
    highlighted_crate = (s32)FindLocalCrate(&edcrt_cam_pos);
    DrawCameraTarget(&edcrt_cam_pos);
    edcrtDrawBoxyThing(&edcrt_cam_pos);
    if (edcrt_active_menu) {
        NuFntScale(0xC, 0xC);
        eduiMenuRender(edcrt_active_menu);
    }
}

/* ------------------------------------------------------------------ */

struct edcrtgroup_s {
    struct nuvec_s origin;   /* 0x00 */
    f32 radius;              /* 0x0C */
    s16 iCrate;              /* 0x10 */
    s16 nCrates;             /* 0x12 */
    s16 angle;               /* 0x14 */
    u8 pad16[0x30 - 0x16];
};

struct edcrtcube_s {
    u8 pad00[0x04];
    struct nuvec_s pos0;     /* 0x04 */
    u8 pad10[0x20 - 0x10];
    f32 shadow;              /* 0x20 */
    u8 pad24[0x3A - 0x24];
    s8 type1;                /* 0x3A */
    s8 type2;                /* 0x3B */
    s8 type3;                /* 0x3C */
    s8 type4;                /* 0x3D */
    u8 pad3E[0x44 - 0x3E];
    s16 dx;                  /* 0x44 */
    s16 dy;                  /* 0x46 */
    s16 dz;                  /* 0x48 */
    s16 iU;                  /* 0x4A */
    s16 iD;                  /* 0x4C */
    s16 iN;                  /* 0x4E */
    s16 iS;                  /* 0x50 */
    s16 iE;                  /* 0x52 */
    s16 iW;                  /* 0x54 */
    s16 trigger;             /* 0x56 */
    u8 pad58[0x90 - 0x58];
};

extern struct edcrtgroup_s CrateGroup[];
extern struct edcrtcube_s Crate[];
extern char tbuf[];
extern char LevelFileName[];
extern char D_00631AF0[];

extern s32 sprintf(char *buf, const char *fmt, ...);
extern s32 NuFileOpen(char *name, s32 mode);
extern void NuFileWriteInt(s32 h, s32 v);
extern void NuFileWriteShort(s32 h, short v);
extern void NuFileWriteFloat(s32 h, f32 v);
extern void NuFileWriteChar(s32 h, char v);
extern void NuFileClose(s32 h);

/* ------------------------------------------------------------------ */

struct edcrtfont_s {
    u8 b[16];
};

extern struct edcrtfont_s D_0061FCE0;
extern void *edcrt_mtl_zoff;
extern void *app_fnt;
extern char D_00631B40[];
extern char D_0061FE78[];
extern char D_0061FE88[];
extern char D_0061FE98[];
extern char D_0061FEA8[];
extern char D_0061FEB8[];
extern char D_0061FEC8[];
extern char D_0061FED8[];

extern void *NuMtlCreate(s32 n);
extern void NuMtlUpdate(void *mtl);
extern void *eduiMenuCreate(s32 x, s32 y, s32 w, s32 h, void *fnt, void *cancelcb,
                            char *title);
extern void *eduiItemSelCreate(s32 a, void *fnt, s32 c, s32 d, void *cb,
                               char *label);
extern void *eduiItemToggleCreate(s32 a, void *fnt, s32 val, s32 e, void *cb,
                                  char *label);
extern void eduiMenuAddItem(void *menu, void *item);
extern void edcrtcbCrateTypesMenu(void);
extern void edcrtcbResetCratesMenu(void);
extern void edcrtcbDisplayTypeMenu(void);
extern void edcrtcbMemCardMenu(void);

void edcrtInit(void)
{
    struct edcrtfont_s fnt;
    u8 *mtl;
    u64 tmp;

    fnt = D_0061FCE0;
    mtl = (u8 *)NuMtlCreate(1);
    tmp = ((*(u64 *)(mtl + 0x168) & 0xFFFFCFFFFFFFFFFFULL) |
           0x0000200000000000ULL | 0x0000C00000000000ULL) & 0xFFFFFFF0FFFFFFFFULL;
    edcrt_mtl_zoff = mtl;
    *(f32 *)(mtl + 0x188) = 0.5f;
    *(f32 *)(mtl + 0x1A0) = 1.0f;
    *(u64 *)(mtl + 0x168) = tmp;
    *(f32 *)(mtl + 0x180) = 0.5f;
    *(f32 *)(mtl + 0x184) = 0.5f;
    NuMtlUpdate(mtl);
    edcrt_options_menu = eduiMenuCreate(0x46, 0x46, 0xB4, 0x12C, app_fnt,
                                        edcrtCbCancelOptMenu, D_00631B40);
    if (edcrt_options_menu == 0)
        return;
    eduiMenuAddItem(edcrt_options_menu,
                    eduiItemSelCreate(1, &fnt, 0, 0, edcrtCbSaveCrateData,
                                      D_0061FE78));
    eduiMenuAddItem(edcrt_options_menu,
                    eduiItemSelCreate(1, &fnt, 0, 0, edcrtcbCrateTypesMenu,
                                      D_0061FE88));
    eduiMenuAddItem(edcrt_options_menu,
                    eduiItemSelCreate(1, &fnt, 0, 0, edcrtcbResetCratesMenu,
                                      D_0061FE98));
    eduiMenuAddItem(edcrt_options_menu,
                    eduiItemSelCreate(1, &fnt, 0, 0, edcrtcbDisplayTypeMenu,
                                      D_0061FEA8));
    eduiMenuAddItem(edcrt_options_menu,
                    eduiItemToggleCreate(0, &fnt, D_00631ADC, 1,
                                         edcrtcbTerrainToggle, D_0061FEB8));
    eduiMenuAddItem(edcrt_options_menu,
                    eduiItemToggleCreate(0, &fnt, D_00631AE0, 2,
                                         edcrtcbShowTriggersToggle, D_0061FEC8));
    eduiMenuAddItem(edcrt_options_menu,
                    eduiItemSelCreate(1, &fnt, 0, 0, edcrtcbMemCardMenu,
                                      D_0061FED8));
}

extern s32 PANELOFF;
extern char **LDATA;
extern s32 CRATECOUNT;
extern void *triggerorigin_crate;
extern void *triggerdest_crate;
extern void *locked_crate;
extern char D_00631B50[];
extern char D_0061FEE8[];
extern char D_0061FF08[];
extern char D_0061FF20[];
extern char D_0061FF38[];
extern char D_00631B58[];
extern char D_00631B60[];
extern char D_0061FF58[];
extern char D_00631B68[];
extern char D_00631B70[];
extern char D_0061FF68[];

extern void NuFntSetPen(s32 colour);
extern void NuFntPrintEx(s32 x, s32 y, s32 colour, char *fmt, ...);
extern char *strupr(char *s);

void edcrtDrawInfo(void)
{
    if (PANELOFF)
        return;
    NuFntScale(8, 0xC);
    NuFntSetPen(-1);
    strcpy(tbuf, LDATA[0]);
    strupr(tbuf);
    NuFntPrintEx(0x400, 0x100, 0, D_00631B50, tbuf);
    NuFntPrintEx(0x400, 0x200, 0, D_0061FEE8, CRATEGROUPCOUNT, 0x80);
    NuFntPrintEx(0x400, 0x300, 0, D_0061FF08, CRATECOUNT, 0x100);
    NuFntPrintEx(0x400, 0x400, 0, D_0061FF20, edcrtDrawType + 1);
    if (triggerorigin_crate) {
        NuFntPrintEx(0x400, 0x500, 0, D_0061FF38,
                     triggerdest_crate ? D_00631B58 : D_00631B60);
    } else {
        NuFntPrintEx(0x400, 0x500, 0, D_0061FF58,
                     locked_crate ? D_00631B68 : D_00631B70);
    }
    NuFntPrintEx(0x400, 0xD00, 0, D_0061FF68, edcrt_cam_pos.x, edcrt_cam_pos.y,
                 edcrt_cam_pos.z);
}

void WriteCrateData(void)
{
    s32 handle;
    s32 i;
    s32 j;

    sprintf(tbuf, D_00631AF0, LevelFileName);
    handle = NuFileOpen(tbuf, 1);
    if (handle == 0)
        return;
    NuFileWriteInt(handle, 4);
    NuFileWriteShort(handle, CRATEGROUPCOUNT);
    for (i = 0; i < CRATEGROUPCOUNT; i++) {
        NuFileWriteFloat(handle, CrateGroup[i].origin.x);
        NuFileWriteFloat(handle, CrateGroup[i].origin.y);
        NuFileWriteFloat(handle, CrateGroup[i].origin.z);
        NuFileWriteShort(handle, CrateGroup[i].iCrate);
        NuFileWriteShort(handle, CrateGroup[i].nCrates);
        NuFileWriteShort(handle, CrateGroup[i].angle);
        for (j = CrateGroup[i].iCrate;
             j < CrateGroup[i].iCrate + CrateGroup[i].nCrates; j++) {
            NuFileWriteFloat(handle, Crate[j].pos0.x);
            NuFileWriteFloat(handle, Crate[j].pos0.y);
            NuFileWriteFloat(handle, Crate[j].pos0.z);
            NuFileWriteFloat(handle, Crate[j].shadow);
            NuFileWriteShort(handle, Crate[j].dx);
            NuFileWriteShort(handle, Crate[j].dy);
            NuFileWriteShort(handle, Crate[j].dz);
            NuFileWriteChar(handle, Crate[j].type1);
            NuFileWriteChar(handle, Crate[j].type2);
            NuFileWriteChar(handle, Crate[j].type3);
            NuFileWriteChar(handle, Crate[j].type4);
            NuFileWriteShort(handle, Crate[j].iU);
            NuFileWriteShort(handle, Crate[j].iD);
            NuFileWriteShort(handle, Crate[j].iN);
            NuFileWriteShort(handle, Crate[j].iS);
            NuFileWriteShort(handle, Crate[j].iE);
            NuFileWriteShort(handle, Crate[j].iW);
            NuFileWriteShort(handle, Crate[j].trigger);
        }
    }
    NuFileClose(handle);
}

void edcrtcbMemCardDeleteCrates(struct edmenucb_s *menu, struct edcrtitem_s *item)
{
    void *arg = 0;
    void (*fn)(void *menu, void *arg);

    sceMcDelete(0, 0, &D_005C18A0[item->newtype << 6]);
    sceMcSync(0, 0, &D_006332F8);
    if (menu->arg != 0) {
        arg = menu->arg;
        eduiMenuDetach(menu);
    }
    fn = menu->cb;
    if (fn)
        fn(menu, arg);
    if (D_006332F8 < 0)
        eduiCreateMessageMenu(arg, D_0061FD10, 0);
    else
        eduiCreateMessageMenu(arg, D_0061FD28, 1);
}
