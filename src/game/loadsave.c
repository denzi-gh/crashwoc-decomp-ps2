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

#include "nu_libc.h"
#include "nu_port.h"
#include "nu_types.h"

#include "nu3d/nugscn.h"
#include "game/gametypes.h"

extern struct game_s Game;
struct game_s SaveSlot[2][2] = {0};
extern struct cursor_s Cursor;
extern struct gtimer_s GlobalTimer;
extern struct objtab_s ObjTab[];
extern u8 CLetter[];
extern char *tEMPTY[];
extern char tbuf[];

extern s32 saveload_status;
extern s32 saveload_error;
extern s32 saveload_cardtype;
extern s32 saveload_cardformatted;

s32 loadsave_frame = 0;
s32 memcard_in = 0;
s32 memcard_formatted = 0;
s32 memcard_gamesavailable = 0;
s32 memcard_loadattempted = 0;
s32 memcard_loadcompleted = 0;
s32 memcard_loadfailed = 0;
s32 memcard_saveneeded = 0;
s32 memcard_savestarted = 0;
s32 memcard_savefailed = 0;
s32 memcard_deleteneeded = 0;
s32 memcard_deletestarted = 0;
s32 memcard_deletefailed = 0;
s32 memcard_formatme = 0;
s32 memcard_formatting = 0;
s32 memcard_formatfailed = 0;
s32 memcard_loadmessage_delay = 0;
s32 memcard_savemessage_delay = 0;
s32 memcard_formatmessage_delay = 0;
s32 memcard_loadresult_delay = 0;
s32 memcard_saveresult_delay = 0;
float SLOTPANELSX = 0.12f;
float SLOTPANELSZ = 0.08f;

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
extern void AddSpacesIntoText(char *text, s32 flag);
extern s32 DrawPanel3DObject(s32 object, float x, float y, float z, float scalex,
                             float scaley, float scalez, u16 xrot, u16 yrot,
                             u16 zrot, struct nugscn_s *scn,
                             struct nuspecial_s *obj, s32 rot);


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


void DrawGameSlot(struct game_s *game, float x, float y, s32 colour) {
    if (game->empty == 0) {
        Text3D(game->name, x, y + 0.15f, 1.0f, 0.6f, 0.6f, 0.6f, 0, colour);
        CalculateGamePercentage(game);
        sprintf(tbuf, "%i%%", game->percent);
        if (Game.language == 'c') {
            AddSpacesIntoText(tbuf, 1);
        }
        Text3D(tbuf, x - 0.15f, y, 1.0f, 0.6f, 0.6f, 0.6f, 0, 0);
        sprintf(tbuf, "%i%c", game->crystals, CLetter[0x75]);
        if (Game.language == 'c') {
            AddSpacesIntoText(tbuf, 1);
        }
        Text3D(tbuf, x + 0.15f, y, 1.0f, 0.6f, 0.6f, 0.6f, 0, 0);
        sprintf(tbuf, "%i%c", game->gems, CLetter[0x77]);
        if (Game.language == 'c') {
            AddSpacesIntoText(tbuf, 1);
        }
        Text3D(tbuf, x - 0.125f, y - 0.15f, 1.0f, 0.6f, 0.6f, 0.6f, 0, 0);
        sprintf(tbuf, "%i%c", game->relics, ObjTab[3].font3d_letter);
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

float SLOTPANELDX = 0.7f;
float SLOTPANELDY = 0.65f;

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
                colour = (GlobalTimer.frame % NW_HZ(10, 12) < 5) ? 4 : 3;
            }
            DrawGameSlot(&SaveSlot[row][column], x, y, colour);
        }
        y -= dy;
    }
}
