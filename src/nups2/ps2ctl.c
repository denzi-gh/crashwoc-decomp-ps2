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

#include "nu_libc.h"
#include "nu_types.h"

#include "nups2/ps2ctl.h"

struct nupadrec_s *PadRecInfo = 0;
s32 PadDemoEnd = 0;

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
extern s32 scePadRead(s32 port, s32 slot, struct nupadframe_s *data);
extern s32 scePadInfoMode(s32 port, s32 slot, s32 term, s32 offs);
extern s32 scePadSetMainMode(s32 port, s32 slot, s32 offs, s32 lock);
extern s32 scePadGetReqState(s32 port, s32 slot);
extern s32 scePadInfoAct(s32 port, s32 slot, s32 actno, s32 term);
extern s32 scePadSetActAlign(s32 port, s32 slot, u8 *data);
extern s32 scePadInfoPressMode(s32 port, s32 slot);
extern s32 scePadEnterPressMode(s32 port, s32 slot);



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


static inline void PadUpdateSticks(struct nupad_s *pad) {
    if (pad->deadzone) {
        pad->ldx = DeadZoneValue(pad->ldx);
        pad->ldy = DeadZoneValue(pad->ldy);
        pad->rdx = DeadZoneValue(pad->rdx);
        pad->rdy = DeadZoneValue(pad->rdy);
    }

    pad->l_nx = (f32)pad->ldx * (pad->l_rx + pad->l_dx) * 0.0078125f;
    pad->l_ny = (f32)pad->ldy * (pad->l_ry + pad->l_dy) * 0.0078125f;
    if (pad->l_nx > 0.0f) {
        pad->l_nx = (pad->l_nx - pad->l_dx < 0.0f) ? 0.0f : pad->l_nx - pad->l_dx;
    }
    if (pad->l_nx < 0.0f) {
        pad->l_nx = (pad->l_nx + pad->l_dx > 0.0f) ? 0.0f : pad->l_nx + pad->l_dx;
    }
    if (pad->l_ny > 0.0f) {
        pad->l_ny = (pad->l_ny - pad->l_dy < 0.0f) ? 0.0f : pad->l_ny - pad->l_dy;
    }
    if (pad->l_ny < 0.0f) {
        pad->l_ny = (pad->l_ny + pad->l_dy > 0.0f) ? 0.0f : pad->l_ny + pad->l_dy;
    }

    pad->r_nx = (f32)pad->rdx * (pad->r_rx + pad->r_dx) * 0.0078125f;
    pad->r_ny = (f32)pad->rdy * (pad->r_ry + pad->r_dy) * 0.0078125f;
    if (pad->r_nx > 0.0f) {
        pad->r_nx = (pad->r_nx - pad->r_dx < 0.0f) ? 0.0f : pad->r_nx - pad->r_dx;
    }
    if (pad->r_nx < 0.0f) {
        pad->r_nx = (pad->r_nx + pad->r_dx > 0.0f) ? 0.0f : pad->r_nx + pad->r_dx;
    }
    if (pad->r_ny > 0.0f) {
        pad->r_ny = (pad->r_ny - pad->r_dy < 0.0f) ? 0.0f : pad->r_ny - pad->r_dy;
    }
    if (pad->r_ny < 0.0f) {
        pad->r_ny = (pad->r_ny + pad->r_dy > 0.0f) ? 0.0f : pad->r_ny + pad->r_dy;
    }
}


s32 NuPs2ReadPad(struct nupad_s *pad) {
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
        pad->stage = 0;
        pad->analogue_sticks = 0;
        pad->analogue_buttons = 0;
        pad->valid = 0;
    } else {
        switch (pad->stage) {
        case 0:
            pad->analogue_sticks = 0;
            pad->analogue_buttons = 0;
            pad->valid = 0;
            pad->id = scePadInfoMode(pad->port, pad->slot, 1, 0);
            if (pad->id == 0) {
                break;
            }
            extmode = scePadInfoMode(pad->port, pad->slot, 2, 0);
            if (extmode > 0) {
                pad->id = extmode;
            }
            switch (pad->id) {
            case 4:
                pad->stage = 1;
                break;
            case 7:
                pad->stage = 4;
                break;
            default:
                pad->stage = 9;
                break;
            }
            break;
        case 1:
            if (scePadInfoMode(pad->port, pad->slot, 4, -1) == 0) {
                pad->stage = 9;
            } else {
                pad->stage = 2;
            }
            break;
        case 2:
            if (scePadSetMainMode(pad->port, pad->slot, 1, 3) == 1) {
                pad->stage = 3;
            }
            break;
        case 3:
            if (scePadGetReqState(pad->port, pad->slot) == 1) {
                pad->stage = 2;
            }
            if (scePadGetReqState(pad->port, pad->slot) == 0) {
                pad->stage = 0;
            }
            break;
        case 4:
            if (scePadInfoAct(pad->port, pad->slot, -1, 0) == 0) {
                pad->stage = 9;
            } else if (scePadSetActAlign(pad->port, pad->slot, pad->actalign) != 0) {
                pad->stage = 5;
            }
            break;
        case 5:
            if (scePadGetReqState(pad->port, pad->slot) == 1) {
                pad->stage = 4;
            }
            if (scePadGetReqState(pad->port, pad->slot) == 0) {
                pad->stage = 6;
            }
            break;
        case 6:
            pad->analogue_sticks = 1;
            if (scePadInfoPressMode(pad->port, pad->slot) == 1) {
                pad->stage = 7;
            } else {
                pad->stage = 9;
            }
            break;
        case 7:
            if (scePadEnterPressMode(pad->port, pad->slot) == 1) {
                pad->stage = 8;
            }
            break;
        case 8:
            if (scePadGetReqState(pad->port, pad->slot) == 1) {
                pad->stage = 7;
            }
            if (scePadGetReqState(pad->port, pad->slot) == 0) {
                pad->analogue_buttons = 1;
                pad->stage = 9;
            }
            break;
        default:
            if (state == 6 || state == 2) {
                oldtype = pad->rdata.read_id;
                if (scePadRead(pad->port, pad->slot, &pad->rdata) <= 0) {
                    pad->rdata.buttons_hi = 0;
                    pad->rdata.buttons_lo = 0;
                }
                paddata = (pad->rdata.buttons_lo | (pad->rdata.buttons_hi << 8)) ^ 0xFFFF;
                pad->paddata_db = paddata & (paddata ^ pad->oldpaddata);
                pad->oldpaddata = pad->paddata = paddata;
                if (pad->analogue_sticks) {
                    pad->ldx = pad->rdata.l_alg_x - 0x80;
                    pad->ldy = 0x80 - pad->rdata.l_alg_y;
                    pad->rdx = pad->rdata.r_alg_x - 0x80;
                    pad->rdy = 0x80 - pad->rdata.r_alg_y;
                    PadUpdateSticks(pad);
                } else {
                    pad->rdata.l_alg_x = pad->rdata.l_alg_y = pad->rdata.r_alg_x = pad->rdata.r_alg_y = 0x80;
                    pad->ldx = pad->ldy = pad->rdx = pad->rdy = 0;
                    pad->l_nx = pad->l_ny = 0.0f;
                }
                pad->valid = 1;
                if (oldtype != 0 && pad->rdata.read_id != oldtype) {
                    pad->stage = 0;
                    pad->analogue_sticks = 0;
                    pad->analogue_buttons = 0;
                    pad->valid = 0;
                    pad->paddata = 0;
                }
            }
            break;
        }
    }

    if (!pad->valid) {
        pad->paddata = pad->paddata_db = pad->oldpaddata = 0;
        pad->ldx = pad->ldy = pad->rdx = pad->rdy = 0;
        pad->r_nx = pad->r_ny = pad->l_nx = pad->l_ny = 0.0f;
        pad->rdata.r_alg_x = pad->rdata.r_alg_y = pad->rdata.l_alg_x = pad->rdata.l_alg_y = 0x80;
        pad->rdata.l_algpad_r = pad->rdata.l_algpad_l = pad->rdata.l_algpad_u = pad->rdata.l_algpad_d = 0;
        pad->rdata.r_algpad_u = pad->rdata.r_algpad_r = pad->rdata.r_algpad_d = pad->rdata.r_algpad_l = 0;
        pad->rdata.l1_alg = pad->rdata.r1_alg = pad->rdata.l2_alg = pad->rdata.r2_alg = 0;
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
            pad->paddata_db = playdata & (playdata ^ pad->oldpaddata);
            pad->oldpaddata = pad->paddata = playdata;
            pad->ldx = pad->rdata.l_alg_x - 0x80;
            pad->ldy = 0x80 - pad->rdata.l_alg_y;
            pad->rdx = pad->rdata.r_alg_x - 0x80;
            pad->rdy = 0x80 - pad->rdata.r_alg_y;
            PadUpdateSticks(pad);
        }
    }
    return pad->valid;
}


void NuPs2ClosePad(struct nupad_s *pad) {
    scePadPortClose(pad->port, pad->slot);
    NuMemFreeFn(pad, "..\\nu2crash.ps2\\nups2\\ps2ctl.c", 0x84);
}


struct nupad_s *NuPs2OpenPad(s32 port, s32 slot) {
    struct nupad_s *pad;
    s32 i;

    NuPs2InitIOP();
    pad = NuMemAllocFn(sizeof(struct nupad_s), "..\\nu2crash.ps2\\nups2\\ps2ctl.c", 0x9A);
    if (pad) {
        memset(pad, 0, sizeof(struct nupad_s));
        pad->port = port;
        pad->slot = slot;
        pad->stage = 0;
        pad->l_rx = 1.0f;
        pad->l_dx = 0.1f;
        pad->l_ry = 1.0f;
        pad->l_dy = 0.1f;
        pad->r_rx = 1.0f;
        pad->r_dx = 0.1f;
        pad->r_ry = 1.0f;
        pad->r_dy = 0.1f;
        for (i = 0; i < 6; i++) {
            pad->actdirect[i] = 0;
            pad->actalign[i] = 0xFF;
        }
        pad->actalign[0] = 0;
        pad->actalign[1] = 1;
        if (!scePadPortOpen(pad->port, pad->slot,
                            (void *)(((nuintptr)pad->dmabuf + 0x3F) & ~(nuintptr)0x3F))) {
            NuMemFreeFn(pad, "..\\nu2crash.ps2\\nups2\\ps2ctl.c", 0xB8);
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


void InitPadPlayRecord(char *file, s32 mode, s32 count, struct nupadrec_s *buffer) {
    if (!PadRecInfo) {
        PadRecInfo = buffer;
    }
    memset(PadRecInfo, 0, count * sizeof(struct nupadframe_s));
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
                        (PadRecInfo->nframes + 2) * sizeof(struct nupadframe_s));
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


void NuPs2PadSetMotors(struct nupad_s *pad, s32 small, s32 big) {
    if (pad->analogue_sticks) {
        pad->actdirect[0] = small > 0;
        pad->actdirect[1] = big;
        scePadSetActDirect(pad->port, pad->slot, pad->actdirect);
    }
}


void PadPlayOff(void) {
    PadRecInfo = 0;
}
