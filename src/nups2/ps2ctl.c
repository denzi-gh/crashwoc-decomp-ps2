/*
 * Unit: nups2/ps2ctl
 *
 * Functions:
 *   0x0016d718 NuPs2ReadPad
 *   0x0016e4d8 NuPs2ClosePad
 *   0x0016e518 NuPs2OpenPad
 *   0x0016e638 NuPs2ApplyDeadZone
 *   0x0016e698 InitPadPlayRecord
 *   0x0016e740 SavePadRecord
 *   0x0016e7c0 PadRecPtr
 *   0x0016e7e0 NuPs2PadDemoEnd
 *   0x0016e7e8 NuPs2PadSetMotors
 *   0x0016e830 PadPlayOff
 *   0x0016e838 DeadZoneValue
 */

typedef signed int s32;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef unsigned int u128 __attribute__((mode(TI)));
typedef float f32;

typedef struct nupadframe_s {
    u8 status;
    u8 type;
    u8 buttons_hi;
    u8 buttons_lo;
    u8 r_x;
    u8 r_y;
    u8 l_x;
    u8 l_y;
    u8 right_p;
    u8 left_p;
    u8 up_p;
    u8 down_p;
    u8 triangle_p;
    u8 circle_p;
    u8 cross_p;
    u8 square_p;
    u8 l1_p;
    u8 r1_p;
    u8 l2_p;
    u8 r2_p;
} NUPADFRAME;

typedef struct nupad_s {
    u128 dmabuf[0x50];
    s32 port;
    s32 slot;
    s32 state;
    s32 mode;
    s32 l_x;
    s32 l_y;
    s32 r_x;
    s32 r_y;
    f32 l_nx;
    f32 l_ny;
    f32 l_xrange;
    f32 l_yrange;
    f32 l_xdead;
    f32 l_ydead;
    f32 r_nx;
    f32 r_ny;
    f32 r_xrange;
    f32 r_yrange;
    f32 r_xdead;
    f32 r_ydead;
    u8 actdirect[6];
    u8 actalign[6];
    u32 paddata;
    u32 oldpaddata;
    u32 buttons;
    int analog : 1;
    int pressure : 1;
    int valid : 1;
    int deadzone : 1;
    int : 28;
    u8 pad56c[0xC];
    NUPADFRAME rdata;
    u8 pad58c[0x14];
} NUPAD;

typedef struct nupadrec_s {
    s32 frame;
    s32 mode;
    s32 nframes;
    s32 maxframes;
    NUPADFRAME data[1];
} NUPADREC;

extern NUPADREC *PadRecInfo;
extern s32 PadDemoEnd;
extern char D_00617880[];

extern void *memset(void *dst, s32 c, u32 n);
extern void *NuMemAllocFn(s32 size, char *file, s32 line);
extern void NuMemFreeFn(void *ptr, char *file, s32 line);
extern s32 NuFileOpen(char *name, s32 mode);
extern s32 NuFileWrite(s32 fh, void *data, s32 size);
extern void NuFileClose(s32 fh);
extern s32 NuFileLoadBuffer(char *name, void *buf, s32 max);
extern void NuPs2InitIOP(void);
extern s32 scePadPortOpen(s32 port, s32 slot, void *addr);
extern s32 scePadPortClose(s32 port, s32 slot);
extern s32 scePadSetActDirect(s32 port, s32 slot, u8 *data);
extern s32 scePadGetState(s32 port, s32 slot);
extern s32 scePadRead(s32 port, s32 slot, NUPADFRAME *data);
extern s32 scePadInfoMode(s32 port, s32 slot, s32 term, s32 offs);
extern s32 scePadSetMainMode(s32 port, s32 slot, s32 offs, s32 lock);
extern s32 scePadGetReqState(s32 port, s32 slot);
extern s32 scePadInfoAct(s32 port, s32 slot, s32 actno, s32 term);
extern s32 scePadSetActAlign(s32 port, s32 slot, u8 *data);
extern s32 scePadInfoPressMode(s32 port, s32 slot);
extern s32 scePadEnterPressMode(s32 port, s32 slot);

#define ps2ctl_srcfile D_00617880


inline s32 DeadZoneValue(s32 value) {
    if (value > 0) {
        if (value < 0x20) {
            value = 0;
        } else {
            value = ((value - 0x20) * 255) / (255 - 0x20);
        }
    } else {
        if (value > -0x20) {
            value = 0;
        } else {
            value = ((value + 0x20) * 255) / (255 - 0x20);
        }
    }
    return value;
}


static inline void PadUpdateSticks(NUPAD *pad) {
    if (pad->deadzone) {
        pad->l_x = DeadZoneValue(pad->l_x);
        pad->l_y = DeadZoneValue(pad->l_y);
        pad->r_x = DeadZoneValue(pad->r_x);
        pad->r_y = DeadZoneValue(pad->r_y);
    }

    pad->l_nx = (f32)pad->l_x * (pad->l_xrange + pad->l_xdead) * 0.0078125f;
    pad->l_ny = (f32)pad->l_y * (pad->l_yrange + pad->l_ydead) * 0.0078125f;
    if (pad->l_nx > 0.0f) {
        pad->l_nx = (pad->l_nx - pad->l_xdead < 0.0f) ? 0.0f : pad->l_nx - pad->l_xdead;
    }
    if (pad->l_nx < 0.0f) {
        pad->l_nx = (pad->l_nx + pad->l_xdead > 0.0f) ? 0.0f : pad->l_nx + pad->l_xdead;
    }
    if (pad->l_ny > 0.0f) {
        pad->l_ny = (pad->l_ny - pad->l_ydead < 0.0f) ? 0.0f : pad->l_ny - pad->l_ydead;
    }
    if (pad->l_ny < 0.0f) {
        pad->l_ny = (pad->l_ny + pad->l_ydead > 0.0f) ? 0.0f : pad->l_ny + pad->l_ydead;
    }

    pad->r_nx = (f32)pad->r_x * (pad->r_xrange + pad->r_xdead) * 0.0078125f;
    pad->r_ny = (f32)pad->r_y * (pad->r_yrange + pad->r_ydead) * 0.0078125f;
    if (pad->r_nx > 0.0f) {
        pad->r_nx = (pad->r_nx - pad->r_xdead < 0.0f) ? 0.0f : pad->r_nx - pad->r_xdead;
    }
    if (pad->r_nx < 0.0f) {
        pad->r_nx = (pad->r_nx + pad->r_xdead > 0.0f) ? 0.0f : pad->r_nx + pad->r_xdead;
    }
    if (pad->r_ny > 0.0f) {
        pad->r_ny = (pad->r_ny - pad->r_ydead < 0.0f) ? 0.0f : pad->r_ny - pad->r_ydead;
    }
    if (pad->r_ny < 0.0f) {
        pad->r_ny = (pad->r_ny + pad->r_ydead > 0.0f) ? 0.0f : pad->r_ny + pad->r_ydead;
    }
}


s32 NuPs2ReadPad(NUPAD *pad) {
    s32 state;
    s32 replay;
    s32 extmode;
    u32 oldpaddata;
    u32 paddata;
    u32 playdata;
    u8 oldtype;

    oldpaddata = pad->oldpaddata;
    state = scePadGetState(pad->port, pad->slot);
    if (state == 0) {
        pad->state = 0;
        pad->analog = 0;
        pad->pressure = 0;
        pad->valid = 0;
    } else {
        switch (pad->state) {
        case 0:
            pad->analog = 0;
            pad->pressure = 0;
            pad->valid = 0;
            pad->mode = scePadInfoMode(pad->port, pad->slot, 1, 0);
            if (pad->mode == 0) {
                break;
            }
            extmode = scePadInfoMode(pad->port, pad->slot, 2, 0);
            if (extmode > 0) {
                pad->mode = extmode;
            }
            switch (pad->mode) {
            case 4:
                pad->state = 1;
                break;
            case 7:
                pad->state = 4;
                break;
            default:
                pad->state = 9;
                break;
            }
            break;
        case 1:
            if (scePadInfoMode(pad->port, pad->slot, 4, -1) == 0) {
                pad->state = 9;
            } else {
                pad->state = 2;
            }
            break;
        case 2:
            if (scePadSetMainMode(pad->port, pad->slot, 1, 3) == 1) {
                pad->state = 3;
            }
            break;
        case 3:
            if (scePadGetReqState(pad->port, pad->slot) == 1) {
                pad->state = 2;
            }
            if (scePadGetReqState(pad->port, pad->slot) == 0) {
                pad->state = 0;
            }
            break;
        case 4:
            if (scePadInfoAct(pad->port, pad->slot, -1, 0) == 0) {
                pad->state = 9;
            } else if (scePadSetActAlign(pad->port, pad->slot, pad->actalign) != 0) {
                pad->state = 5;
            }
            break;
        case 5:
            if (scePadGetReqState(pad->port, pad->slot) == 1) {
                pad->state = 4;
            }
            if (scePadGetReqState(pad->port, pad->slot) == 0) {
                pad->state = 6;
            }
            break;
        case 6:
            pad->analog = 1;
            if (scePadInfoPressMode(pad->port, pad->slot) == 1) {
                pad->state = 7;
            } else {
                pad->state = 9;
            }
            break;
        case 7:
            if (scePadEnterPressMode(pad->port, pad->slot) == 1) {
                pad->state = 8;
            }
            break;
        case 8:
            if (scePadGetReqState(pad->port, pad->slot) == 1) {
                pad->state = 7;
            }
            if (scePadGetReqState(pad->port, pad->slot) == 0) {
                pad->pressure = 1;
                pad->state = 9;
            }
            break;
        default:
            if (state == 6 || state == 2) {
                oldtype = pad->rdata.type;
                if (scePadRead(pad->port, pad->slot, &pad->rdata) <= 0) {
                    pad->rdata.buttons_hi = 0;
                    pad->rdata.buttons_lo = 0;
                }
                paddata = (pad->rdata.buttons_lo | (pad->rdata.buttons_hi << 8)) ^ 0xFFFF;
                pad->buttons = paddata & (paddata ^ pad->oldpaddata);
                pad->oldpaddata = pad->paddata = paddata;
                if (pad->analog) {
                    pad->l_x = pad->rdata.l_x - 0x80;
                    pad->l_y = 0x80 - pad->rdata.l_y;
                    pad->r_x = pad->rdata.r_x - 0x80;
                    pad->r_y = 0x80 - pad->rdata.r_y;
                    PadUpdateSticks(pad);
                } else {
                    pad->rdata.l_x = pad->rdata.l_y = pad->rdata.r_x = pad->rdata.r_y = 0x80;
                    pad->l_x = pad->l_y = pad->r_x = pad->r_y = 0;
                    pad->l_nx = pad->l_ny = 0.0f;
                }
                pad->valid = 1;
                if (oldtype != 0 && pad->rdata.type != oldtype) {
                    pad->state = 0;
                    pad->analog = 0;
                    pad->pressure = 0;
                    pad->valid = 0;
                    pad->paddata = 0;
                }
            }
            break;
        }
    }

    if (!pad->valid) {
        pad->paddata = pad->buttons = pad->oldpaddata = 0;
        pad->l_x = pad->l_y = pad->r_x = pad->r_y = 0;
        pad->r_nx = pad->r_ny = pad->l_nx = pad->l_ny = 0.0f;
        pad->rdata.r_x = pad->rdata.r_y = pad->rdata.l_x = pad->rdata.l_y = 0x80;
        pad->rdata.right_p = pad->rdata.left_p = pad->rdata.up_p = pad->rdata.down_p = 0;
        pad->rdata.triangle_p = pad->rdata.circle_p = pad->rdata.cross_p = pad->rdata.square_p = 0;
        pad->rdata.l1_p = pad->rdata.r1_p = pad->rdata.l2_p = pad->rdata.r2_p = 0;
    }

    if (PadRecInfo && pad->port == 0) {
        replay = 0;
        if (PadRecInfo->mode == 1 && PadRecInfo->frame < PadRecInfo->maxframes) {
            if (pad->valid) {
                PadRecInfo->data[PadRecInfo->frame++] = pad->rdata;
            } else {
                pad->rdata.buttons_hi = 0;
                PadRecInfo->data[PadRecInfo->frame++] = pad->rdata;
            }
        }
        if (PadRecInfo->mode == 2) {
            pad->valid = 1;
            pad->oldpaddata = oldpaddata;
            if ((u16)pad->paddata) {
                PadDemoEnd = 1;
            }
            if (PadRecInfo->frame < PadRecInfo->nframes - 2) {
                pad->rdata = PadRecInfo->data[PadRecInfo->frame++];
            } else {
                pad->rdata = PadRecInfo->data[PadRecInfo->frame];
                PadDemoEnd = 1;
            }
            if (pad->rdata.buttons_hi) {
                replay = 1;
            } else {
                pad->valid = 0;
            }
        }
        if (replay) {
            playdata = (pad->rdata.buttons_lo | (pad->rdata.buttons_hi << 8)) ^ 0xFFFF;
            pad->buttons = playdata & (playdata ^ pad->oldpaddata);
            pad->oldpaddata = pad->paddata = playdata;
            pad->l_x = pad->rdata.l_x - 0x80;
            pad->l_y = 0x80 - pad->rdata.l_y;
            pad->r_x = pad->rdata.r_x - 0x80;
            pad->r_y = 0x80 - pad->rdata.r_y;
            PadUpdateSticks(pad);
        }
    }
    return pad->valid;
}


void NuPs2ClosePad(NUPAD *pad) {
    scePadPortClose(pad->port, pad->slot);
    NuMemFreeFn(pad, ps2ctl_srcfile, 0x84);
}


NUPAD *NuPs2OpenPad(s32 port, s32 slot) {
    NUPAD *pad;
    s32 i;

    NuPs2InitIOP();
    pad = NuMemAllocFn(sizeof(NUPAD), ps2ctl_srcfile, 0x9A);
    if (pad) {
        memset(pad, 0, sizeof(NUPAD));
        pad->port = port;
        pad->slot = slot;
        pad->state = 0;
        pad->l_xrange = 1.0f;
        pad->l_xdead = 0.1f;
        pad->l_yrange = 1.0f;
        pad->l_ydead = 0.1f;
        pad->r_xrange = 1.0f;
        pad->r_xdead = 0.1f;
        pad->r_yrange = 1.0f;
        pad->r_ydead = 0.1f;
        for (i = 0; i < 6; i++) {
            pad->actdirect[i] = 0;
            pad->actalign[i] = 0xFF;
        }
        pad->actalign[0] = 0;
        pad->actalign[1] = 1;
        if (!scePadPortOpen(pad->port, pad->slot,
                            (void *)(((u32)pad->dmabuf + 0x3F) & 0xFFFFFFC0))) {
            NuMemFreeFn(pad, ps2ctl_srcfile, 0xB8);
            pad = 0;
        }
    }
    return pad;
}


s32 NuPs2ApplyDeadZone(s32 value, s32 deadzone) {
    value -= 0x80;
    if (value > 0) {
        if (value < deadzone) {
            value = 0;
        } else {
            value = ((value - deadzone) * 255) / (255 - deadzone);
        }
    } else {
        if (value > -deadzone) {
            value = 0;
        } else {
            value = ((value + deadzone) * 255) / (255 - deadzone);
        }
    }
    return value;
}


void InitPadPlayRecord(char *file, s32 mode, s32 count, NUPADREC *buffer) {
    if (!PadRecInfo) {
        PadRecInfo = buffer;
    }
    memset(PadRecInfo, 0, count * sizeof(NUPADFRAME));
    PadDemoEnd = 0;
    if (file && mode == 2 && !NuFileLoadBuffer(file, PadRecInfo, 0xEA610)) {
        PadDemoEnd = 1;
    }
    PadRecInfo->mode = mode;
    PadRecInfo->maxframes = count - 2;
    PadRecInfo->frame = 0;
}


void SavePadRecord(char *file) {
    s32 fh;

    if (PadRecInfo && file && PadRecInfo->mode == 1) {
        PadRecInfo->frame -= 2;
        PadRecInfo->nframes = PadRecInfo->frame;
        fh = NuFileOpen(file, 1);
        if (fh) {
            NuFileWrite(fh, PadRecInfo,
                        (PadRecInfo->nframes + 2) * sizeof(NUPADFRAME));
            NuFileClose(fh);
        }
    }
}


s32 PadRecPtr(void) {
    if (PadRecInfo) {
        return PadRecInfo->frame;
    }
    return 0;
}


s32 NuPs2PadDemoEnd(void) {
    return PadDemoEnd;
}


void NuPs2PadSetMotors(NUPAD *pad, s32 small, s32 big) {
    if (pad->analog) {
        pad->actdirect[0] = small > 0;
        pad->actdirect[1] = big;
        scePadSetActDirect(pad->port, pad->slot, pad->actdirect);
    }
}


void PadPlayOff(void) {
    PadRecInfo = 0;
}
