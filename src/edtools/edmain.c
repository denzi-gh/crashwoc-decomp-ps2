/*
 * Unit: edtools/edmain
 *
 * Functions:
 *   0x001b7840 edmainCreateMainMenu
 *   0x001b79f8 edmainSetReturn
 *   0x001b7a00 edmainRegister
 *   0x001b7a50 edmainProcess
 *   0x001b7ac0 edmainRender
 *   0x001b7b10 edmainSetCamera
 *   0x001b7bb0 edmainInit
 *   0x001b7c78 edmainClose
 *   0x001b7cf8 edmainRegisterLocVec
 *   0x001b7d00 edmainQueryLocVec
 *   0x001b7d08 edmainMainMenuProc
 *   0x001b7d30 edmainMainMenuRender
 *   0x001b7d58 edmainReadLevelFile
 *   0x001b7e08 cbMMRegSel
 *   0x001b7e40 cbMMCancel
 *   0x001b7e48 cbMMEditorConfig
 *   0x001b7e88 cbMMReturnToApp
 *   0x001b7e98 cbmcfgCancel
 *   0x001b7eb8 cbmcfgCameraSpeed
 */

#include "nu_types.h"

#include "numath/numathtypes.h"
#include "nu3d/nucamera.h"
#include "nups2/ps2ctl.h"
#include "edtools/edui.h"
#include "edtools/edmain.h"

typedef void (*EDUICALLBACK)(struct eduimenu_s *menu, struct eduiitem_s *item);

static s32 ed_init = 0;
static struct edmainreg_s *ed_list = 0;
static struct edmainreg_s *ed_curr = 0;
static s32 ed_module_active = 0;
static struct eduimenu_s *ed_main_menu = 0;
static struct eduimenu_s *ed_cfg_menu = 0;
static void *ed_fnt = 0;
static struct nucamera_s *edmaincam = 0;
static struct nuvec_s *ed_loc = 0;

static s32 editor_return;
static char ed_levelfile[0x100];

extern void edmainClose(void);
extern void eduiMenuProcess(struct eduimenu_s *menu, struct nupad_s *pad);
extern s32 eduiMenuAttach(struct eduimenu_s *menu, struct eduimenu_s *child);
extern void eduiMenuDetach(struct eduimenu_s *menu);
extern void eduiMenuDestroy(struct eduimenu_s *menu);
extern struct eduimenu_s *eduiMenuCreate(s32 x, s32 y, s32 w, s32 h, void *font,
                                EDUICALLBACK cancel, char *title);
extern struct eduiitem_s *eduiMenuAddItem(struct eduimenu_s *menu, struct eduiitem_s *item);
extern void eduiMenuFitWidth(struct eduimenu_s *menu, s32 margin);
extern void eduiMenuHighlight(struct eduimenu_s *menu, struct eduiitem_s *item);
extern struct eduiitem_s *eduiItemSelCreate(void *data, struct eduistyle_s *style, s32 on,
                                   s32 group, EDUICALLBACK select, char *text);
extern struct eduiitem_s *eduiItemSliderCreate(void *data, struct eduistyle_s *style, s32 group,
                                      EDUICALLBACK change, f32 val, f32 min,
                                      f32 max, char *text);
extern void edcamSetSpeed(f32 rise, f32 move, f32 fall, f32 zoom);
extern void NuCameraSet(struct nucamera_s *cam);
extern void NuStrCpy(char *dst, char *src);
extern s32 NuFileOpen(char *name, s32 mode);
extern void NuFileClose(s32 fh);
extern s32 NuFileBeginBlkRead(s32 fh, s32 blockid);
extern void NuFileEndBlkRead(s32 fh);

void edmainMainMenuProc(struct nupad_s *pad);
void edmainMainMenuRender(void);
void edmainReadLevelFile(void);
void cbMMRegSel(struct eduimenu_s *menu, struct eduiitem_s *item);
void cbMMCancel(struct eduimenu_s *menu, struct eduiitem_s *item);
void cbMMEditorConfig(struct eduimenu_s *menu, struct eduiitem_s *item);
void cbMMReturnToApp(struct eduimenu_s *menu, struct eduiitem_s *item);
void cbmcfgCancel(struct eduimenu_s *menu, struct eduiitem_s *item);
void cbmcfgCameraSpeed(struct eduimenu_s *menu, struct eduiitem_s *item);
void popmcbCancel(struct eduimenu_s *menu, struct eduiitem_s *item);
void popmcbOkay(struct eduimenu_s *menu, struct eduiitem_s *item);


/* The Reload All / Reload Current / Save All callbacks (still called in the
 * e3 demo build) end in an "Okay" popup. Nothing calls them here, so, being
 * inline, GCC 2.95 emits no code for them, only their literals; their file
 * handling leaves no data and is not reproduced. */
static inline void cbMMReloadAll(struct eduimenu_s *menu, struct eduiitem_s *item) {
    struct eduistyle_s style = { { 0x80000000, 0x80FF0000 }, { 0x80808080, 0x80404040 } };
    struct eduimenu_s *popup;

    popup = eduiMenuCreate(item->x + 10, item->y + 10, 0xB4, 0x32, menu->font,
                           popmcbCancel, "Loaded all");
    if (popup) {
        eduiMenuAddItem(popup, eduiItemSelCreate(0, &style, 0, 0, popmcbOkay, "Okay"));
        eduiMenuAttach(menu, popup);
    }
}


static inline void cbMMReloadCurr(struct eduimenu_s *menu, struct eduiitem_s *item) {
    struct eduistyle_s style = { { 0x80000000, 0x80FF0000 }, { 0x80808080, 0x80404040 } };
    struct eduimenu_s *popup;

    popup = eduiMenuCreate(item->x + 10, item->y + 10, 0xB4, 0x32, menu->font,
                           popmcbCancel, "Loaded current");
    if (popup) {
        eduiMenuAddItem(popup, eduiItemSelCreate(0, &style, 0, 0, popmcbOkay, "Okay"));
        eduiMenuAttach(menu, popup);
    }
}


static inline void cbMMSaveAll(struct eduimenu_s *menu, struct eduiitem_s *item) {
    struct eduistyle_s style = { { 0x80000000, 0x80FF0000 }, { 0x80808080, 0x80404040 } };
    struct eduimenu_s *popup;

    popup = eduiMenuCreate(item->x + 10, item->y + 10, 0xB4, 0x32, menu->font,
                           popmcbCancel, "Written all");
    if (popup) {
        eduiMenuAddItem(popup, eduiItemSelCreate(0, &style, 0, 0, popmcbOkay, "Okay"));
        eduiMenuAttach(menu, popup);
    }
}


void edmainCreateMainMenu(void) {
    struct eduistyle_s style = { { 0x80000000, 0x80FF0000 }, { 0x80808080, 0x80404040 } };
    struct edmainreg_s *reg;
    struct eduiitem_s *item;

    ed_cfg_menu = eduiMenuCreate(0xDC, 0x32, 0xB4, 0x190, ed_fnt,
                                 cbmcfgCancel, "Editor Config");
    if (ed_cfg_menu) {
        eduiMenuAddItem(ed_cfg_menu,
                        eduiItemSliderCreate(0, &style, 0, cbmcfgCameraSpeed,
                                             1.0f, 9.0f, 1.0f, "Camera Speed"));
        eduiMenuFitWidth(ed_cfg_menu, 8);
    }
    ed_main_menu = eduiMenuCreate(0xDC, 0x32, 0xB4, 0x190, ed_fnt,
                                  cbMMCancel, "Editor Options");
    if (ed_main_menu) {
        for (reg = ed_list; reg; reg = reg->next) {
            item = eduiMenuAddItem(ed_main_menu,
                                   eduiItemSelCreate(reg, &style, 0, 1,
                                                     cbMMRegSel, reg->name));
            if (reg == ed_curr) {
                eduiMenuHighlight(ed_main_menu, item);
            }
        }
        eduiMenuAddItem(ed_main_menu,
                        eduiItemSelCreate(0, &style, 0, 0, cbMMEditorConfig,
                                          "Cursor Speed"));
        eduiMenuAddItem(ed_main_menu,
                        eduiItemSelCreate(0, &style, 0, 0, cbMMReturnToApp,
                                          "Return To App"));
    }
}


void edmainSetReturn(s32 value) {
    editor_return = value;
}


s32 edmainRegister(struct edmainreg_s *reg) {
    if (ed_init) {
        edmainClose();
    }
    if (ed_list) {
        ed_list->prev = reg;
    }
    reg->prev = 0;
    reg->next = ed_list;
    ed_list = reg;
    return 1;
}


s32 edmainProcess(struct nupad_s *pad, f32 dt) {
    editor_return = 0;
    if (ed_module_active && ed_curr) {
        if (!ed_curr->process || ed_curr->process(pad, dt)) {
            ed_module_active = 0;
        }
    } else {
        if (!ed_curr) {
            ed_curr = ed_list;
        }
        edmainMainMenuProc(pad);
    }
    return editor_return;
}


void edmainRender(void) {
    if (ed_module_active && ed_curr) {
        if (ed_curr->render) {
            ed_curr->render();
        } else {
            ed_module_active = 0;
        }
    } else {
        edmainMainMenuRender();
    }
}


void edmainSetCamera(struct numtx_s *mtx) {
    edmaincam->mtx = *mtx;
    NuCameraSet(edmaincam);
}


void edmainInit(void *font, char *levelfile) {
    struct edmainreg_s *reg;

    if (ed_init) {
        edmainClose();
    }
    ed_fnt = font;
    ed_module_active = 0;
    for (reg = ed_list; reg; reg = reg->next) {
        if (reg->init) {
            reg->init();
        }
    }
    edmaincam = NuCameraCreate();
    eduiInit();
    edmainCreateMainMenu();
    if (levelfile) {
        NuStrCpy(ed_levelfile, levelfile);
    } else {
        ed_levelfile[0] = 0;
    }
    edmainReadLevelFile();
    ed_init = 1;
}


void edmainClose(void) {
    if (ed_init) {
        NuCameraDestroy(edmaincam);
        while (ed_list) {
            if (ed_list->close) {
                ed_list->close();
            }
            ed_list = ed_list->next;
        }
        if (ed_main_menu) {
            eduiMenuDestroy(ed_main_menu);
            ed_main_menu = 0;
        }
        ed_list = 0;
        ed_curr = 0;
        ed_init = 0;
    }
}


void edmainRegisterLocVec(struct nuvec_s *vec) {
    ed_loc = vec;
}


struct nuvec_s *edmainQueryLocVec(void) {
    return ed_loc;
}


void edmainMainMenuProc(struct nupad_s *pad) {
    if (ed_main_menu) {
        eduiMenuProcess(ed_main_menu, pad);
    }
}


void edmainMainMenuRender(void) {
    if (ed_main_menu) {
        eduiMenuRender(ed_main_menu);
    }
}


void edmainReadLevelFile(void) {
    s32 fh;
    s32 blockid;
    struct edmainreg_s *reg;

    if (ed_levelfile[0]) {
        fh = NuFileOpen(ed_levelfile, 0);
        if (fh) {
            NuFileBeginBlkRead(fh, 0x3032554E);
            while ((blockid = NuFileBeginBlkRead(fh, 0)) != 0) {
                for (reg = ed_list; reg; reg = reg->next) {
                    if (reg->blockid == blockid) {
                        if (reg->readblock) {
                            reg->readblock(fh);
                        }
                        break;
                    }
                }
                NuFileEndBlkRead(fh);
            }
            NuFileEndBlkRead(fh);
            NuFileClose(fh);
        }
    }
}


void cbMMRegSel(struct eduimenu_s *menu, struct eduiitem_s *item) {
    struct edmainreg_s *reg;

    reg = item->data;
    ed_module_active = 1;
    ed_curr = reg;
    if (reg->select) {
        reg->select();
    }
}


void cbMMCancel(struct eduimenu_s *menu, struct eduiitem_s *item) {
}


void cbMMEditorConfig(struct eduimenu_s *menu, struct eduiitem_s *item) {
    ed_cfg_menu->x = item->x + 10;
    ed_cfg_menu->y = item->y + 10;
    eduiMenuAttach(menu, ed_cfg_menu);
}


void cbMMReturnToApp(struct eduimenu_s *menu, struct eduiitem_s *item) {
    editor_return = 1;
}


void cbmcfgCancel(struct eduimenu_s *menu, struct eduiitem_s *item) {
    eduiMenuDetach(menu);
}


void cbmcfgCameraSpeed(struct eduimenu_s *menu, struct eduiitem_s *item) {
    f32 scale;
    f32 speed;

    scale = 512.0f / item->u.slider.val;
    speed = 0.2f / scale;
    edcamSetSpeed(speed, speed, speed, 1.0f / scale);
}
