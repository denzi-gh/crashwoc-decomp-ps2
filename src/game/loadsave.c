/*
 * Unit: game/loadsave
 *
 * Functions:
 *   0x0025ad80 UpdateSaveSlots
 *   0x0025b0f8 DrawGameSlot
 *   0x0025b3e0 loadsaveCallEachFrame
 *   0x0025b420 LoadGameSlots
 *   0x0025b4c8 SaveGameSlots
 *   0x0025b508 DrawGameSlots
 */

typedef unsigned char u8;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef unsigned long long u64;

struct hub_s {
    u8 flags;
    u8 crystals;
    char pad1;
    char pad2;
};

struct time_s {
    char name[4];
    u32 itime;
};

struct level_s {
    u16 flags;
    char pad1;
    char pad2;
    struct time_s time[3];
};

struct game_s {
    char name[9];
    u8 vibration;
    u8 surround;
    u8 sfx_volume;
    u8 music_volume;
    char screen_x;
    char screen_y;
    u8 language;
    struct hub_s hub[6];
    struct level_s level[35];
    u8 lives;
    u8 wumpas;
    u8 mask;
    u8 percent;
    u8 crystals;
    u8 relics;
    u8 crate_gems;
    u8 bonus_gems;
    u8 gems;
    u8 gembits;
    u8 powerbits;
    u8 empty;
    u32 cutbits;
};

struct remember_s {
    u8 x;
    u8 y;
};

struct cursor_s {
    struct remember_s remember[48];
    u32 menu_frame;
    u32 item_frame;
    char x;
    char y;
    char x_min;
    char y_min;
    char x_max;
    char y_max;
    char menu;
    char new_menu;
    char new_level;
    u8 wait;
    u8 wait_frames;
    char wait_hack;
    u8 button_lock;
};

struct gametimer_s {
    u32 frame;
    u8 pad_4[0x8];
    float ftime;
    u8 pad_10[0x8];
};

struct nugscn_s;
struct objspecial_s;

struct objinfo_s {
    struct nugscn_s *scene;
    struct objspecial_s *special;
};

struct objtab_s {
    struct objinfo_s obj;
    struct nugscn_s **scene;
    signed char visible;
    char font3d_letter;
    char pad_e[0x10 - 0xE];
    char *name;
    char pad_14[0x18 - 0x14];
    u64 levbits;
};

extern struct game_s Game;
extern struct game_s SaveSlot[2][2];
extern struct cursor_s Cursor;
extern struct gametimer_s GlobalTimer;
extern struct objtab_s ObjTab[];
extern u8 CLetter[];
extern char *tEMPTY[];
extern char tbuf[];
extern char D_00632B78[];
extern char D_00632B80[];
#define percent_format D_00632B78
#define count_format D_00632B80

extern float SLOTPANELSX;
extern float SLOTPANELSZ;
extern float SLOTPANELDX;
extern float SLOTPANELDY;

extern s32 saveload_status;
extern s32 saveload_error;
extern s32 saveload_cardtype;
extern s32 saveload_cardformatted;

extern s32 memcard_in;
extern s32 memcard_formatted;
extern s32 memcard_gamesavailable;
extern s32 memcard_loadattempted;
extern s32 memcard_loadcompleted;
extern s32 memcard_loadfailed;
extern s32 memcard_saveneeded;
extern s32 memcard_savestarted;
extern s32 memcard_savefailed;
extern s32 memcard_deleteneeded;
extern s32 memcard_deletestarted;
extern s32 memcard_deletefailed;
extern s32 memcard_formatme;
extern s32 memcard_formatting;
extern s32 memcard_formatfailed;
extern s32 memcard_loadmessage_delay;
extern s32 memcard_savemessage_delay;
extern s32 memcard_saveresult_delay;
extern s32 memcard_formatmessage_delay;
extern s32 memcard_loadresult_delay;

extern void saveloadASCallEachFrame(void);
extern void saveloadASFormat(void);
extern void saveloadASLoad(s32 slot, void *data, s32 size);
extern void saveloadASSave(s32 slot, void *data, s32 size);
extern void saveloadASDelete(s32 slot);
extern s32 saveloadCheckCardType(void);
extern s32 saveloadCheckCardFormatted(void);
extern s32 saveloadFormatCard(void);
extern s32 saveloadLoadSlot(s32 slot, void *data, s32 size);
extern s32 saveloadSaveSlot(s32 slot, void *data, s32 size);

extern void Text3D(char *txt, float x, float y, float z, float scalex, float scaley,
                   float scalez, s32 align, s32 colour);
extern void CalculateGamePercentage(struct game_s *game);
extern s32 sprintf(char *, const char *, ...);
extern void AddSpacesIntoText(char *text, s32 flag);
extern s32 DrawPanel3DObject(s32 object, float x, float y, float z, float scalex,
                             float scaley, float scalez, u16 xrot, u16 yrot,
                             u16 zrot, struct nugscn_s *scn,
                             struct objspecial_s *obj, s32 rot);


void UpdateSaveSlots(struct cursor_s *cursor) {
    s32 row;
    s32 column;

    if (memcard_loadmessage_delay != 0) {
        memcard_loadmessage_delay--;
    }
    if (memcard_savemessage_delay != 0) {
        memcard_savemessage_delay--;
    } else if (memcard_saveresult_delay != 0) {
        memcard_saveresult_delay--;
    }
    if (memcard_formatmessage_delay != 0) {
        memcard_formatmessage_delay--;
    }
    if (memcard_loadresult_delay != 0) {
        memcard_loadresult_delay--;
    }
    if (saveload_cardtype != 2 || saveload_cardformatted == 0) {
        memcard_loadattempted = 0;
    }
    if (memcard_loadattempted == 0) {
        if (saveload_status == 1 && saveload_cardtype == 2 && saveload_cardformatted != 0) {
            for (row = 0; row < 2; row++) {
                for (column = 0; column < 2; column++) {
                    SaveSlot[row][column].empty = 1;
                }
            }
            saveloadASLoad(0, SaveSlot, 0x1030);
            memcard_loadattempted = 1;
            memcard_loadmessage_delay = 0x28;
            memcard_loadcompleted = 0;
            memcard_loadfailed = 0;
            memcard_gamesavailable = 0;
        }
    } else if (memcard_loadcompleted == 0 && saveload_status == 1) {
        if (saveload_error == 0) {
            memcard_gamesavailable = 1;
        } else if (saveload_error == 5) {
            memcard_gamesavailable = 0;
        } else {
            memcard_loadfailed = 1;
            memcard_gamesavailable = 0;
            for (row = 0; row < 2; row++) {
                for (column = 0; column < 2; column++) {
                    SaveSlot[row][column].empty = 1;
                }
            }
        }
        memcard_loadcompleted = 1;
    }
    if (memcard_saveneeded != 0) {
        if (saveload_status == 1 && saveload_cardtype == 2 && saveload_cardformatted != 0) {
            saveloadASSave(0, SaveSlot, 0x1030);
            memcard_savestarted = 1;
            memcard_savemessage_delay = 0x28;
            memcard_saveneeded = 0;
            memcard_savefailed = 0;
            memcard_deletefailed = 0;
        }
    } else if (memcard_savestarted != 0 && saveload_status == 1) {
        if (saveload_error == 0) {
            memcard_gamesavailable = 1;
            memcard_savestarted = 0;
        } else {
            memcard_savefailed = 1;
            memcard_savestarted = 0;
            memcard_gamesavailable = 0;
        }
    }
    if (memcard_deleteneeded != 0) {
        if (saveload_status == 1 && saveload_cardtype == 2 && saveload_cardformatted != 0) {
            saveloadASDelete(0);
            memcard_deletestarted = 1;
            memcard_savemessage_delay = 0x28;
            memcard_deleteneeded = 0;
            memcard_savefailed = 0;
            memcard_deletefailed = 0;
        }
    } else if (memcard_deletestarted != 0 && saveload_status == 1) {
        if (saveload_error != 0) {
            memcard_deletefailed = 1;
        }
        memcard_deletestarted = 0;
    }
    if (memcard_formatme != 0) {
        if (saveload_status == 1 && saveload_cardtype == 2) {
            saveloadASFormat();
            memcard_formatting = 1;
            memcard_formatmessage_delay = 0x28;
            memcard_formatme = 0;
            memcard_formatfailed = 0;
        }
    } else if (memcard_formatting != 0 && saveload_status == 3) {
        if (saveload_error == 0) {
            memcard_formatting = 0;
        } else {
            memcard_formatting = 0;
            memcard_formatfailed = 1;
        }
        memcard_loadattempted = 0;
    }
}


void DrawGameSlot(struct game_s *game, s32 colour, float x, float y) {
    if (game->empty == 0) {
        Text3D(game->name, x, y + 0.15f, 1.0f, 0.6f, 0.6f, 0.6f, 0, colour);
        CalculateGamePercentage(game);
        sprintf(tbuf, percent_format, game->percent);
        if (Game.language == 'c') {
            AddSpacesIntoText(tbuf, 1);
        }
        Text3D(tbuf, x - 0.15f, y, 1.0f, 0.6f, 0.6f, 0.6f, 0, 0);
        sprintf(tbuf, count_format, game->crystals, CLetter[0x75]);
        if (Game.language == 'c') {
            AddSpacesIntoText(tbuf, 1);
        }
        Text3D(tbuf, x + 0.15f, y, 1.0f, 0.6f, 0.6f, 0.6f, 0, 0);
        sprintf(tbuf, count_format, game->gems, CLetter[0x77]);
        if (Game.language == 'c') {
            AddSpacesIntoText(tbuf, 1);
        }
        Text3D(tbuf, x - 0.125f, y - 0.15f, 1.0f, 0.6f, 0.6f, 0.6f, 0, 0);
        sprintf(tbuf, count_format, game->relics, ObjTab[3].font3d_letter);
        if (Game.language == 'c') {
            AddSpacesIntoText(tbuf, 1);
        }
        Text3D(tbuf, x + 0.125f, y - 0.15f, 1.0f, 0.6f, 0.6f, 0.6f, 0, 0);
    } else {
        Text3D(tEMPTY[Game.language], x, y, 1.0f, 0.6f, 0.6f, 0.6f, 0, colour);
    }
    DrawPanel3DObject(0x81, x, y, 1.01f, SLOTPANELSX, 0.125f, SLOTPANELSZ, 0xC000, 0, 0,
                      ObjTab[129].obj.scene, ObjTab[129].obj.special, 0);
}


void loadsaveCallEachFrame(void) {
    saveloadASCallEachFrame();
    if (saveload_cardtype != 2 || saveload_cardformatted == 0) {
        memcard_loadattempted = 0;
    }
}


void LoadGameSlots(void) {
    s32 row;
    s32 column;

    for (row = 0; row < 2; row++) {
        for (column = 0; column < 2; column++) {
            SaveSlot[row][column].empty = 1;
        }
    }
    memcard_in = saveloadCheckCardType();
    memcard_formatted = 0;
    memcard_gamesavailable = 0;
    if (memcard_in != 0) {
        memcard_formatted = saveloadCheckCardFormatted();
        if (memcard_formatted != 0) {
            memcard_gamesavailable = saveloadLoadSlot(0, SaveSlot, 0x1030);
        }
    }
}


void SaveGameSlots(void) {
    if (saveloadCheckCardFormatted() == 0) {
        saveloadFormatCard();
    }
    saveloadSaveSlot(0, SaveSlot, 0x1030);
}


void DrawGameSlots(void) {
    float dx;
    float dy;
    float x;
    float y;
    s32 row;
    s32 column;
    s32 colour;

    dy = SLOTPANELDY;
    dx = SLOTPANELDX;
    y = 0.0f + dy * 0.5f;
    for (row = 0; row < 2; row++) {
        x = 0.0f - dx * 0.5f;
        for (column = 0; column < 2; column++, x += dx) {
            colour = 0;
            if (Cursor.y == row && Cursor.x == column) {
                colour = (GlobalTimer.frame % 10 < 5) ? 4 : 3;
            }
            DrawGameSlot(&SaveSlot[row][column], colour, x, y);
        }
        y -= dy;
    }
}
