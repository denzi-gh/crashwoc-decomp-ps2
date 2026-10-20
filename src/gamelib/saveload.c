/*
 * Unit: gamelib/saveload
 *
 * Functions:
 *   0x001a9b08 saveloadGetDirectory
 *   0x001a9c98 saveloadSaveSlot
 *   0x001aa020 saveloadDeleteSlot
 *   0x001aa1c0 saveloadASCallEachFrame
 *   0x001aa918 saveloadLoadIcon
 *   0x001aa968 saveloadCheckCardPresent
 *   0x001aa9b0 saveloadCheckCardType
 *   0x001aaa08 saveloadCheckCardFormatted
 *   0x001aaa70 saveloadCheckCardFreeSpace
 *   0x001aab08 saveloadFormatCard
 *   0x001aab40 saveloadUnFormatCard
 *   0x001aab78 saveloadCheckSlotsUsed
 *   0x001aabb0 saveloadLoadSlot
 *   0x001aacf8 saveloadASFormat
 *   0x001aad30 saveloadASLoad
 *   0x001aada8 saveloadASSave
 *   0x001aae20 saveloadASDelete
 */

#include "nu_libc.h"
#include "nu_types.h"

#include "nu3d/nustream.h"

extern s32 NuFileSize(char *name);
extern s32 NuFileLoadBuffer(char *name, void *buf, s32 max);
extern s32 sceMcSync(s32 mode, s32 *cmd, s32 *result);
extern s32 sceMcGetInfo(s32 port, s32 slot, s32 *type, s32 *free, s32 *format);
extern s32 sceMcChdir(s32 port, s32 slot, const char *path, char *pwd);
extern s32 sceMcFormat(s32 port, s32 slot);
extern s32 sceMcUnformat(s32 port, s32 slot);
extern s32 sceMcGetDir(s32 port, s32 slot, const char *name, unsigned int mode, s32 maxent,
                       void *table);
extern s32 sceMcOpen(s32 port, s32 slot, const char *name, s32 mode);
extern s32 sceMcRead(s32 fd, void *buf, s32 size);
extern s32 sceMcClose(s32 fd);

/* The memory card icon.sys header (the SDK's sceMcIconSys). */
struct mciconsys_s {
    u8 Head[4];                         /* 0x000 */
    u16 Reserv1;                        /* 0x004 */
    u16 OffsLF;                         /* 0x006 */
    u32 Reserv2;                        /* 0x008 */
    u32 TransRate;                      /* 0x00C */
    s32 BgColor[4][4];                  /* 0x010 */
    f32 LightDir[3][4];                 /* 0x050 */
    f32 LightColor[3][4];               /* 0x080 */
    f32 Ambient[4];                     /* 0x0B0 */
    u8 TitleName[68];                   /* 0x0C0 */
    u8 FnameView[64];                   /* 0x104 */
    u8 FnameCopy[64];                   /* 0x144 */
    u8 FnameDel[64];                    /* 0x184 */
    u8 Reserve3[512];                   /* 0x1C4 */
}; /* 0x3C4 */

static struct mciconsys_s mciconsys = {
    "PS2D",
    0,
    30,                                 /* line break after "Crash Bandicoot" */
    0,
    64,
    { { 64, 64, 0, 0 }, { 64, 64, 0, 0 }, { 64, 0, 0, 0 }, { 64, 0, 0, 0 } },
    { { 1.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 0.0f } },
    { { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f } },
    { 0.5f, 0.5f, 0.5f, 0.0f },
    /* "Crash Bandicoot" "Wrath of Cortex", full-width Shift-JIS */
    "\202\142\202\222\202\201\202\223\202\210\201\100\202\141\202\201"
    "\202\216\202\204\202\211\202\203\202\217\202\217\202\224"
    "\202\166\202\222\202\201\202\224\202\210\201\100\202\217\202\206"
    "\201\100\202\142\202\217\202\222\202\224\202\205\202\230",
    "crash.ico",
    "crash.ico",
    "crash.ico",
};

char fname[32] = { 0 };
char dname[32] = { 0 };
char saveload_slotname[32] = { 0 };

s32 saveloadGetDirectory(void);

static s32 iconsize = 0;
void *iconbuff = 0;
s32 saveload_status = 0;
s32 saveload_error = 0;
s32 saveload_cardtype = -1;
s32 saveload_freespace = -1;
s32 saveload_cardformatted = -1;
s32 saveload_cardchanged = 0;
extern s32 saveload_slotid;
extern void *saveload_data;
extern s32 saveload_datasize;
extern s32 saveload_handle;

extern s32 D_0063321C;
extern s32 D_00633220;
extern s32 D_00633224;
extern s32 D_00633228;

#define mc_type D_0063321C
#define mc_free D_00633220
#define mc_format D_00633224
#define mc_result D_00633228

#define SCE_RDONLY 1

extern char D_006B3B40[];               /* sceMcGetDir entry table */
#define mc_dirtable D_006B3B40


s32 saveloadCheckCardPresent(void) {
    s32 nocard;

    sceMcGetInfo(0, 0, 0, 0, 0);
    sceMcSync(0, 0, &mc_result);
    nocard = -10;
    return mc_result > nocard;
}


/* Fills mc_dirtable with the save directory listing. Returns 0 when the
   directory does not exist yet, otherwise the entry count or -1 on error. */
s32 saveloadGetDirectory(void) {
    char path[32];
    s32 present;

    sceMcGetInfo(0, 0, 0, 0, 0);
    sceMcSync(0, 0, &mc_result);
    present = mc_result > -10;
    if (!present) {
        return -1;
    }
    sceMcChdir(0, 0, "/", 0);
    sceMcSync(0, 0, &mc_result);
    if (mc_result != 0) {
        return -1;
    }
    sprintf(fname, "%s", "BESLES-50386");
    sprintf(dname, "/%s", fname);
    sprintf(path, "%s*", fname);
    sceMcGetDir(0, 0, fname, 0, 0x12, mc_dirtable);
    sceMcSync(0, 0, &mc_result);
    if (mc_result == 0) {
        return 0;
    }
    sceMcChdir(0, 0, dname, 0);
    sceMcSync(0, 0, &mc_result);
    if (mc_result != 0) {
        return -1;
    }
    sceMcGetDir(0, 0, path, 0, 0x12, mc_dirtable);
    sceMcSync(0, 0, &mc_result);
    if (mc_result == 0) {
        return -1;
    }
    return mc_result;
}

extern s32 sceMcDelete(s32 port, s32 slot, const char *name);

extern s32 sceMcMkdir(s32 port, s32 slot, const char *name);
extern s32 sceMcWrite(s32 fd, const void *buf, s32 size);

/* Writes `size` bytes of `buf` to save `slot`, creating the save directory
   (with its icon files) first if the card has none. Returns 1 on success. */
s32 saveloadSaveSlot(s32 slot, void *buf, s32 size) {
    char path[32];
    s32 count;
    s32 fd;
    void *icon;

    if (slot >= 4) {
        return 0;
    }
    count = saveloadGetDirectory();
    if (count == -1) {
        return 0;
    }
    if (count == 0) {
        sceMcChdir(0, 0, "/", 0);
        sceMcSync(0, 0, &mc_result);
        if (mc_result != 0) {
            return 0;
        }
        sceMcMkdir(0, 0, fname);
        sceMcSync(0, 0, &mc_result);
        if (mc_result != 0) {
            return 0;
        }
        sceMcChdir(0, 0, dname, 0);
        sceMcSync(0, 0, &mc_result);
        if (mc_result != 0) {
            return 0;
        }
        sceMcOpen(0, 0, fname, 0x200);
        sceMcSync(0, 0, &mc_result);
        fd = mc_result;
        sceMcWrite(fd, &count, 4);
        sceMcSync(0, 0, &mc_result);
        sceMcClose(fd);
        sceMcSync(0, 0, &mc_result);

        sceMcOpen(0, 0, "icon.sys", 0x200);
        sceMcSync(0, 0, &mc_result);
        fd = mc_result;
        sceMcWrite(fd, &mciconsys, 0x3C4);
        sceMcSync(0, 0, &mc_result);
        sceMcClose(fd);
        sceMcSync(0, 0, &mc_result);

        iconsize = NuFileSize("stuff\\crash.ico");
        icon = NuStreamGetOutputBuffer(0);
        iconsize = NuFileLoadBuffer("stuff\\crash.ico", icon, 0x7FFFFFFF);
        sceMcOpen(0, 0, "crash.ico", 0x200);
        sceMcSync(0, 0, &mc_result);
        fd = mc_result;
        sceMcWrite(fd, icon, iconsize);
        sceMcSync(0, 0, &mc_result);
        sceMcClose(fd);
        sceMcSync(0, 0, &mc_result);
        NuRndrSwapStreamBuffers();
    }
    sprintf(path, "%sslot%02d", fname, slot);
    sceMcGetDir(0, 0, path, 0, 0x12, mc_dirtable);
    sceMcSync(0, 0, &mc_result);
    if (mc_result == 1) {
        sceMcDelete(0, 0, path);
        sceMcSync(0, 0, &mc_result);
        if (mc_result != 0) {
            return 0;
        }
    }
    sceMcOpen(0, 0, path, 0x200);
    sceMcSync(0, 0, &mc_result);
    fd = mc_result;
    if (fd < 0) {
        return 0;
    }
    sceMcWrite(fd, buf, size);
    sceMcSync(0, 0, &mc_result);
    if (mc_result < 0) {
        return 0;
    }
    sceMcClose(fd);
    sceMcSync(0, 0, &mc_result);
    if (mc_result < 0) {
        return 0;
    }
    return 1;
}

/* Deletes save `slot`; deleting the last one removes the whole save
   directory. Returns 1 on success. */
s32 saveloadDeleteSlot(s32 slot) {
    char path[32];
    s32 count;

    if (slot >= 4) {
        return 0;
    }
    count = saveloadGetDirectory();
    if (count == -1 || count == 0) {
        return 0;
    }
    sprintf(path, "%sslot%02d", fname, slot);
    sceMcGetDir(0, 0, path, 0, 0x12, mc_dirtable);
    sceMcSync(0, 0, &mc_result);
    if (mc_result != 1) {
        return 0;
    }
    sceMcDelete(0, 0, path);
    sceMcSync(0, 0, &mc_result);
    if (mc_result < 0) {
        return 0;
    }
    if (count == 2) {
        sceMcDelete(0, 0, fname);
        sceMcSync(0, 0, &mc_result);
        sceMcDelete(0, 0, "icon.sys");
        sceMcSync(0, 0, &mc_result);
        sceMcDelete(0, 0, "crash.ico");
        sceMcSync(0, 0, &mc_result);
        sceMcChdir(0, 0, "/", 0);
        sceMcSync(0, 0, &mc_result);
        sceMcDelete(0, 0, fname);
        sceMcSync(0, 0, &mc_result);
    }
    return 1;
}

void saveloadASCallEachFrame(void) {
    static s32 freqcount = 0;
    char path[32];
    s32 result;
    s32 done;

    if (saveload_status == 0) {
        sceMcGetInfo(0, 0, &saveload_cardtype, &saveload_freespace, &saveload_cardformatted);
        saveload_status = 3;
        return;
    }
    done = sceMcSync(1, 0, &result);
    if (done == -1) {
        freqcount++;
        if (freqcount >= 5) {
            freqcount = 0;
            sceMcGetInfo(0, 0, 0, 0, 0);
            saveload_status = 2;
        }
        return;
    }
    if (done == 0) {
        return;
    }
    switch (saveload_status) {
    case 2:
        if (result == 0) {
            saveload_error = 0;
            saveload_status = 1;
            saveload_cardchanged = 0;
        } else if (result == -1 || result == -2) {
            sceMcGetInfo(0, 0, &saveload_cardtype, &saveload_freespace, &saveload_cardformatted);
            saveload_status = 3;
            saveload_cardchanged = 1;
        } else {
            if (saveload_cardtype != -1) {
                saveload_cardchanged = 1;
            } else {
                saveload_cardchanged = 0;
            }
            saveload_cardtype = -1;
            saveload_freespace = -1;
            saveload_cardformatted = -1;
            saveload_status = 1;
            saveload_error = 0;
        }
        break;
    case 3:
        if (result == 0) {
            saveload_error = 0;
            saveload_status = 1;
            saveload_cardchanged = 0;
        } else if (result == -1 || result == -2) {
            sceMcGetInfo(0, 0, &saveload_cardtype, &saveload_freespace, &saveload_cardformatted);
            saveload_status = 3;
            saveload_cardchanged = 1;
        } else {
            saveload_cardtype = -1;
            saveload_freespace = -1;
            saveload_cardformatted = -1;
            saveload_status = 1;
            saveload_cardchanged = 1;
        }
        break;
    case 4:
        sceMcGetInfo(0, 0, &saveload_cardtype, &saveload_freespace, &saveload_cardformatted);
        saveload_status = 3;
        if (result == 0) {
            saveload_error = 0;
        } else {
            saveload_error = 1;
        }
        break;
    case 5:
        if (result == 0) {
            sceMcOpen(0, 0, saveload_slotname, SCE_RDONLY);
            saveload_status = 6;
        } else {
            saveload_status = 1;
            saveload_error = 5;
        }
        break;
    case 6:
        if (result >= 0) {
            saveload_handle = result;
            sceMcRead(result, saveload_data, saveload_datasize);
            saveload_status = 7;
        } else {
            saveload_status = 1;
            saveload_error = 2;
        }
        break;
    case 7:
        if (result >= 0) {
            sceMcClose(saveload_handle);
            saveload_status = 8;
        } else {
            saveload_status = 1;
            saveload_error = 2;
        }
        break;
    case 8:
        if (result == 0) {
            saveload_error = 0;
            saveload_status = 1;
        } else {
            saveload_status = 1;
            saveload_error = 2;
        }
        break;
    case 9:
        if (result == -4) {
            sceMcMkdir(0, 0, "/BESLES-50386");
            saveload_status = 14;
        } else if (result == 0) {
            sceMcGetDir(0, 0, "BESLES-50386", 0, 0x12, mc_dirtable);
            saveload_status = 10;
        } else {
            saveload_status = 1;
            saveload_error = 3;
        }
        break;
    case 10:
        if (result == 1) {
            sceMcOpen(0, 0, saveload_slotname, 0x200);
            saveload_status = 11;
        } else if (result == 0) {
            sceMcOpen(0, 0, "crash.ico", 0x200);
            saveload_status = 16;
        } else {
            saveload_status = 1;
            saveload_error = 3;
        }
        break;
    case 11:
        if (result >= 0) {
            saveload_handle = result;
            sceMcWrite(result, saveload_data, saveload_datasize);
            saveload_status = 12;
        } else {
            saveload_status = 1;
            saveload_error = 3;
        }
        break;
    case 12:
        if (result >= 0) {
            sceMcClose(saveload_handle);
            saveload_status = 13;
        } else {
            saveload_status = 1;
            saveload_error = 3;
        }
        break;
    case 13:
        if (result == 0) {
            sceMcGetInfo(0, 0, &saveload_cardtype, &saveload_freespace, &saveload_cardformatted);
            saveload_error = 0;
            saveload_status = 3;
        } else {
            saveload_status = 1;
            saveload_error = 3;
        }
        break;
    case 14:
        if (result == 0) {
            sceMcChdir(0, 0, "/BESLES-50386", 0);
            saveload_status = 15;
        } else {
            saveload_status = 1;
            saveload_error = 3;
        }
        break;
    case 15:
        if (result == 0) {
            sceMcOpen(0, 0, "crash.ico", 0x200);
            saveload_status = 16;
        } else {
            saveload_status = 1;
            saveload_error = 3;
        }
        break;
    case 16:
        if (result >= 0) {
            saveload_handle = result;
            sceMcWrite(result, iconbuff, iconsize);
            saveload_status = 17;
        } else {
            saveload_status = 1;
            saveload_error = 3;
        }
        break;
    case 17:
        if (result >= 0) {
            sceMcClose(saveload_handle);
            saveload_status = 18;
        } else {
            saveload_status = 1;
            saveload_error = 3;
        }
        break;
    case 18:
        if (result == 0) {
            sceMcOpen(0, 0, "icon.sys", 0x200);
            saveload_status = 19;
        } else {
            saveload_status = 1;
            saveload_error = 3;
        }
        break;
    case 19:
        if (result >= 0) {
            saveload_handle = result;
            sceMcWrite(result, &mciconsys, 0x3C4);
            saveload_status = 20;
        } else {
            saveload_status = 1;
            saveload_error = 3;
        }
        break;
    case 20:
        if (result >= 0) {
            sceMcClose(saveload_handle);
            saveload_status = 21;
        } else {
            saveload_status = 1;
            saveload_error = 3;
        }
        break;
    case 21:
        if (result == 0) {
            sceMcOpen(0, 0, "BESLES-50386", 0x200);
            saveload_status = 22;
        } else {
            saveload_status = 1;
            saveload_error = 3;
        }
        break;
    case 22:
        if (result >= 0) {
            saveload_handle = result;
            sceMcWrite(result, &saveload_handle, 4);
            saveload_status = 23;
        } else {
            saveload_status = 1;
            saveload_error = 3;
        }
        break;
    case 23:
        if (result >= 0) {
            sceMcClose(saveload_handle);
            saveload_status = 24;
        } else {
            saveload_status = 1;
            saveload_error = 3;
        }
        break;
    case 24:
        if (result >= 0) {
            sceMcOpen(0, 0, saveload_slotname, 0x200);
            saveload_status = 11;
        } else {
            saveload_status = 1;
            saveload_error = 3;
        }
        break;
    case 25:
        if (result == 0) {
            sceMcDelete(0, 0, saveload_slotname);
            saveload_status = 26;
        } else {
            saveload_status = 1;
            saveload_error = 4;
        }
        break;
    case 26:
        if (result == 0) {
            sprintf(path, "%sslot*", "BESLES-50386");
            sceMcGetDir(0, 0, path, 0, 0x12, mc_dirtable);
            saveload_status = 27;
        } else {
            saveload_status = 1;
            saveload_error = 4;
        }
        break;
    case 27:
        if (result > 0) {
            sceMcGetInfo(0, 0, &saveload_cardtype, &saveload_freespace, &saveload_cardformatted);
            saveload_error = 0;
            saveload_status = 3;
        } else if (result == 0) {
            sceMcDelete(0, 0, "BESLES-50386");
            saveload_status = 28;
        } else {
            saveload_status = 1;
            saveload_error = 4;
        }
        break;
    case 28:
        if (result == 0) {
            sceMcDelete(0, 0, "icon.sys");
            saveload_status = 29;
        } else {
            saveload_status = 1;
            saveload_error = 4;
        }
        break;
    case 29:
        if (result == 0) {
            sceMcDelete(0, 0, "crash.ico");
            saveload_status = 30;
        } else {
            saveload_status = 1;
            saveload_error = 4;
        }
        break;
    case 30:
        if (result == 0) {
            sceMcChdir(0, 0, "/", 0);
            saveload_status = 31;
        } else {
            saveload_status = 1;
            saveload_error = 4;
        }
        break;
    case 31:
        if (result == 0) {
            sceMcDelete(0, 0, "BESLES-50386");
            saveload_status = 32;
        } else {
            saveload_status = 1;
            saveload_error = 4;
        }
        break;
    case 32:
        if (result == 0) {
            sceMcGetInfo(0, 0, &saveload_cardtype, &saveload_freespace, &saveload_cardformatted);
            saveload_error = 0;
            saveload_status = 3;
        } else {
            saveload_status = 1;
            saveload_error = 4;
        }
        break;
    }
}

s32 saveload_slotid = 0;
void *saveload_data = 0;
s32 saveload_datasize = 0;
s32 saveload_handle = 0;

s32 saveloadLoadIcon(char *name, void *buf) {
    iconsize = NuFileSize(name);
    iconbuff = buf;
    NuFileLoadBuffer(name, buf, iconsize);
    return iconsize;
}



s32 saveloadCheckCardType(void) {
    sceMcGetInfo(0, 0, &mc_type, 0, 0);
    sceMcSync(0, 0, &mc_result);
    if (mc_result < -9) {
        return 0;
    }
    return mc_type == 2;
}


s32 saveloadCheckCardFormatted(void) {
    sceMcGetInfo(0, 0, &mc_type, 0, &mc_format);
    sceMcSync(0, 0, &mc_result);
    if (mc_result < -9) {
        return 0;
    }
    if (mc_type != 2) {
        return 0;
    }
    return mc_format == 1;
}


s32 saveloadCheckCardFreeSpace(s32 size) {
    sceMcGetInfo(0, 0, &mc_type, &mc_free, &mc_format);
    sceMcSync(0, 0, &mc_result);
    if (mc_result < -9) {
        return 0;
    }
    if (mc_type != 2) {
        return 0;
    }
    if (mc_format != 1) {
        return 0;
    }
    if (size <= 0) {
        return mc_free;
    }
    if (mc_free >= size) {
        return 1;
    }
    return 0;
}


s32 saveloadFormatCard(void) {
    sceMcFormat(0, 0);
    sceMcSync(0, 0, &mc_result);
    return mc_result == 0;
}


s32 saveloadUnFormatCard(void) {
    sceMcUnformat(0, 0);
    sceMcSync(0, 0, &mc_result);
    return mc_result == 0;
}


s32 saveloadCheckSlotsUsed(void) {
    s32 slots;

    slots = saveloadGetDirectory();
    if (slots == -1 || slots == 0) {
        return 0;
    }
    return slots - 1;
}


s32 saveloadLoadSlot(s32 slot, void *buf, s32 size) {
    char path[32];
    s32 dir;
    s32 fd;

    if (slot >= 4) {
        return 0;
    }
    dir = saveloadGetDirectory();
    if (dir == 0 || dir == -1) {
        return 0;
    }
    sprintf(path, "%sslot%02d", fname, slot);
    sceMcGetDir(0, 0, path, 0, 0x12, mc_dirtable);
    sceMcSync(0, 0, &mc_result);
    if (mc_result != 1) {
        return 0;
    }
    sceMcOpen(0, 0, path, SCE_RDONLY);
    sceMcSync(0, 0, &mc_result);
    fd = mc_result;
    if (fd < 0) {
        return 0;
    }
    sceMcRead(fd, buf, size);
    sceMcSync(0, 0, &mc_result);
    if (mc_result < 0) {
        return 0;
    }
    sceMcClose(fd);
    sceMcSync(0, 0, &mc_result);
    if (mc_result < 0) {
        return 0;
    }
    return 1;
}

void saveloadASFormat(void) {
    if (saveload_status == 1) {
        sceMcFormat(0, 0);
        saveload_status = 4;
    }
}


void saveloadASLoad(s32 slot, void *data, s32 size) {
    if (slot < 4) {
        saveload_slotid = slot;
        saveload_data = data;
        saveload_datasize = size;
        sprintf(saveload_slotname, "%sslot%02d", "BESLES-50386", slot);
        sceMcChdir(0, 0, "/BESLES-50386", 0);
        saveload_status = 5;
    }
}


void saveloadASSave(s32 slot, void *data, s32 size) {
    if (slot < 4) {
        saveload_slotid = slot;
        saveload_data = data;
        saveload_datasize = size;
        sprintf(saveload_slotname, "%sslot%02d", "BESLES-50386", slot);
        sceMcChdir(0, 0, "/BESLES-50386", 0);
        saveload_status = 9;
    }
}


void saveloadASDelete(s32 slot) {
    if (slot < 4) {
        saveload_slotid = slot;
        sprintf(saveload_slotname, "%sslot%02d", "BESLES-50386", slot);
        sceMcChdir(0, 0, "/BESLES-50386", 0);
        saveload_status = 0x19;
    }
}
