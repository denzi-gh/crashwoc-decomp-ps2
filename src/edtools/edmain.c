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

typedef struct eduistyle_s {
    u32 colour[4];
} EDUISTYLE;

typedef struct edmainreg_s {
    struct edmainreg_s *next;
    struct edmainreg_s *prev;
    char *name;
    void (*init)(void);
    void (*close)(void);
    void (*select)(void);
    char pad_0x18[0xC];
    void (*readblock)(s32 fh);
    s32 blockid;
    s32 (*process)(struct nupad_s *pad, f32 dt);
    void (*render)(void);
} EDMAINREG;

typedef struct eduiitem_s {
    char pad_0x00[0xC];
    void *data;
    char pad_0x10[0x8];
    s32 x;
    s32 y;
    char pad_0x20[0x2C];
    f32 slider_val;
} EDUIITEM;

typedef struct eduimenu_s {
    char pad_0x00[0x10];
    s32 x;
    s32 y;
} EDUIMENU;

typedef void (*EDUICALLBACK)(EDUIMENU *menu, EDUIITEM *item);

extern s32 D_0062F8C8;
extern EDMAINREG *D_0062F8CC;
extern EDMAINREG *D_0062F8D0;
extern s32 D_0062F8D4;
extern EDUIMENU *D_0062F8D8;
extern EDUIMENU *D_0062F8DC;
extern void *D_0062F8E0;
extern struct nucamera_s *D_0062F8E4;
extern struct nuvec_s *D_0062F8E8;
extern s32 D_00633234;
extern char D_006B3FC0[0x100];
extern EDUISTYLE D_0061A710;
extern char D_0061A750[];
extern char D_0061A760[];
extern char D_0061A770[];
extern char D_0061A780[];
extern char D_0061A790[];

#define edmain_initialised D_0062F8C8
#define edmain_registry    D_0062F8CC
#define edmain_current     D_0062F8D0
#define edmain_active      D_0062F8D4
#define edmain_mainmenu    D_0062F8D8
#define edmain_cfgmenu     D_0062F8DC
#define edmain_font        D_0062F8E0
#define edmain_camera      D_0062F8E4
#define edmain_locvec      D_0062F8E8
#define edmain_return      D_00633234
#define edmain_levelfile   D_006B3FC0
#define edmain_itemstyle   D_0061A710

extern void edmainClose(void);
extern void eduiInit(void);
extern void eduiMenuProcess(EDUIMENU *menu, struct nupad_s *pad);
extern void eduiMenuRender(EDUIMENU *menu);
extern s32 eduiMenuAttach(EDUIMENU *menu, EDUIMENU *child);
extern void eduiMenuDetach(EDUIMENU *menu);
extern void eduiMenuDestroy(EDUIMENU *menu);
extern EDUIMENU *eduiMenuCreate(s32 x, s32 y, s32 w, s32 h, void *font,
                                EDUICALLBACK cancel, char *title);
extern EDUIITEM *eduiMenuAddItem(EDUIMENU *menu, EDUIITEM *item);
extern void eduiMenuFitWidth(EDUIMENU *menu, s32 margin);
extern void eduiMenuHighlight(EDUIMENU *menu, EDUIITEM *item);
extern EDUIITEM *eduiItemSelCreate(void *data, EDUISTYLE *style, s32 on,
                                   s32 group, EDUICALLBACK select, char *text);
extern EDUIITEM *eduiItemSliderCreate(void *data, EDUISTYLE *style, s32 group,
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
void cbMMRegSel(EDUIMENU *menu, EDUIITEM *item);
void cbMMCancel(EDUIMENU *menu, EDUIITEM *item);
void cbMMEditorConfig(EDUIMENU *menu, EDUIITEM *item);
void cbMMReturnToApp(EDUIMENU *menu, EDUIITEM *item);
void cbmcfgCancel(EDUIMENU *menu, EDUIITEM *item);
void cbmcfgCameraSpeed(EDUIMENU *menu, EDUIITEM *item);


void edmainCreateMainMenu(void) {
    EDUISTYLE style;
    EDMAINREG *reg;
    EDUIITEM *item;

    style = edmain_itemstyle;
    edmain_cfgmenu = eduiMenuCreate(0xDC, 0x32, 0xB4, 0x190, edmain_font,
                                    cbmcfgCancel, D_0061A750);
    if (edmain_cfgmenu) {
        eduiMenuAddItem(edmain_cfgmenu,
                        eduiItemSliderCreate(0, &style, 0, cbmcfgCameraSpeed,
                                             1.0f, 9.0f, 1.0f, D_0061A760));
        eduiMenuFitWidth(edmain_cfgmenu, 8);
    }
    edmain_mainmenu = eduiMenuCreate(0xDC, 0x32, 0xB4, 0x190, edmain_font,
                                     cbMMCancel, D_0061A770);
    if (edmain_mainmenu) {
        for (reg = edmain_registry; reg; reg = reg->next) {
            item = eduiMenuAddItem(edmain_mainmenu,
                                   eduiItemSelCreate(reg, &style, 0, 1,
                                                     cbMMRegSel, reg->name));
            if (reg == edmain_current) {
                eduiMenuHighlight(edmain_mainmenu, item);
            }
        }
        eduiMenuAddItem(edmain_mainmenu,
                        eduiItemSelCreate(0, &style, 0, 0, cbMMEditorConfig,
                                          D_0061A780));
        eduiMenuAddItem(edmain_mainmenu,
                        eduiItemSelCreate(0, &style, 0, 0, cbMMReturnToApp,
                                          D_0061A790));
    }
}


void edmainSetReturn(s32 value) {
    edmain_return = value;
}


s32 edmainRegister(EDMAINREG *reg) {
    if (edmain_initialised) {
        edmainClose();
    }
    if (edmain_registry) {
        edmain_registry->prev = reg;
    }
    reg->prev = 0;
    reg->next = edmain_registry;
    edmain_registry = reg;
    return 1;
}


s32 edmainProcess(struct nupad_s *pad, f32 dt) {
    edmain_return = 0;
    if (edmain_active && edmain_current) {
        if (!edmain_current->process || edmain_current->process(pad, dt)) {
            edmain_active = 0;
        }
    } else {
        if (!edmain_current) {
            edmain_current = edmain_registry;
        }
        edmainMainMenuProc(pad);
    }
    return edmain_return;
}


void edmainRender(void) {
    if (edmain_active && edmain_current) {
        if (edmain_current->render) {
            edmain_current->render();
        } else {
            edmain_active = 0;
        }
    } else {
        edmainMainMenuRender();
    }
}


void edmainSetCamera(struct numtx_s *mtx) {
    edmain_camera->mtx = *mtx;
    NuCameraSet(edmain_camera);
}


void edmainInit(void *font, char *levelfile) {
    EDMAINREG *reg;

    if (edmain_initialised) {
        edmainClose();
    }
    edmain_font = font;
    edmain_active = 0;
    for (reg = edmain_registry; reg; reg = reg->next) {
        if (reg->init) {
            reg->init();
        }
    }
    edmain_camera = NuCameraCreate();
    eduiInit();
    edmainCreateMainMenu();
    if (levelfile) {
        NuStrCpy(edmain_levelfile, levelfile);
    } else {
        edmain_levelfile[0] = 0;
    }
    edmainReadLevelFile();
    edmain_initialised = 1;
}


void edmainClose(void) {
    if (edmain_initialised) {
        NuCameraDestroy(edmain_camera);
        while (edmain_registry) {
            if (edmain_registry->close) {
                edmain_registry->close();
            }
            edmain_registry = edmain_registry->next;
        }
        if (edmain_mainmenu) {
            eduiMenuDestroy(edmain_mainmenu);
            edmain_mainmenu = 0;
        }
        edmain_registry = 0;
        edmain_current = 0;
        edmain_initialised = 0;
    }
}


void edmainRegisterLocVec(struct nuvec_s *vec) {
    edmain_locvec = vec;
}


struct nuvec_s *edmainQueryLocVec(void) {
    return edmain_locvec;
}


void edmainMainMenuProc(struct nupad_s *pad) {
    if (edmain_mainmenu) {
        eduiMenuProcess(edmain_mainmenu, pad);
    }
}


void edmainMainMenuRender(void) {
    if (edmain_mainmenu) {
        eduiMenuRender(edmain_mainmenu);
    }
}


void edmainReadLevelFile(void) {
    s32 fh;
    s32 blockid;
    EDMAINREG *reg;

    if (edmain_levelfile[0]) {
        fh = NuFileOpen(edmain_levelfile, 0);
        if (fh) {
            NuFileBeginBlkRead(fh, 0x3032554E);
            while ((blockid = NuFileBeginBlkRead(fh, 0)) != 0) {
                for (reg = edmain_registry; reg; reg = reg->next) {
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


void cbMMRegSel(EDUIMENU *menu, EDUIITEM *item) {
    EDMAINREG *reg;

    reg = item->data;
    edmain_active = 1;
    edmain_current = reg;
    if (reg->select) {
        reg->select();
    }
}


void cbMMCancel(EDUIMENU *menu, EDUIITEM *item) {
}


void cbMMEditorConfig(EDUIMENU *menu, EDUIITEM *item) {
    edmain_cfgmenu->x = item->x + 10;
    edmain_cfgmenu->y = item->y + 10;
    eduiMenuAttach(menu, edmain_cfgmenu);
}


void cbMMReturnToApp(EDUIMENU *menu, EDUIITEM *item) {
    edmain_return = 1;
}


void cbmcfgCancel(EDUIMENU *menu, EDUIITEM *item) {
    eduiMenuDetach(menu);
}


void cbmcfgCameraSpeed(EDUIMENU *menu, EDUIITEM *item) {
    f32 scale;
    f32 speed;

    scale = 512.0f / item->slider_val;
    speed = 0.2f / scale;
    edcamSetSpeed(speed, speed, speed, 1.0f / scale);
}
