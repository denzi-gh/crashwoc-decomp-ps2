/*
 * Unit: sdk/sce/libpad
 *
 * Functions:
 *   0x002645a8 _send_to_iop
 *   0x002646d8 scePadInit
 *   0x00264818 scePadInit2
 *   0x002648b8 scePadEnd
 *   0x00264938 scePadPortOpen
 *   0x00264b20 scePadPortClose
 *   0x00264bd8 scePadGetDmaStr
 *   0x00264c38 scePadGetFrameCount
 *   0x00264c88 scePadRead
 *   0x00264d08 scePadGetState
 *   0x00264d80 scePadStateIntToStr
 *   0x00264db8 scePadSetReqState
 *   0x00264e20 scePadGetReqState
 *   0x00264e70 scePadReqIntToStr
 *   0x00264ea8 scePadInfoAct
 *   0x00264fc8 scePadInfoComb
 *   0x002650e8 scePadInfoMode
 *   0x00265220 scePadSetMainMode
 *   0x002652d8 scePadSetActDirect
 *   0x00265390 scePadSetActAlign
 *   0x00265468 scePadGetButtonMask
 *   0x00265520 scePadSetButtonInfo
 *   0x002655d0 scePadInfoPressMode
 *   0x00265630 scePadEnterPressMode
 *   0x00265688 scePadExitPressMode
 *   0x002656e0 scePadSetVrefParam
 *   0x002657b0 scePadGetPortMax
 *   0x00265818 scePadGetSlotMax
 *   0x00265880 scePadGetModVersion
 *   0x002658e8 scePadSetWarningLevel
 */

typedef struct sceSifClientData {
    unsigned char rpc_data[16];
    unsigned int command;
    void *buff;
    void *cbuff;
    void (*end_func)(void *);
    void *end_param;
    void *server;
} sceSifClientData;

typedef struct sceSifDmaData {
    void *data;
    void *addr;
    unsigned int size;
    unsigned int mode;
} sceSifDmaData;

typedef struct PadDmaFrame {
    unsigned char data[32];
    unsigned int act_direct[2];
    unsigned int act_align[2];
    unsigned char act_info[4][4];
    unsigned char comb_info[4][4];
    unsigned short mode_table[4];
    int frame;
    int retries;
    int length;
    unsigned char mode_config;
    unsigned char mode_cur_id;
    unsigned char model;
    unsigned char data_ready;
    unsigned char mode_count;
    unsigned char mode_cur_offset;
    unsigned char act_count;
    unsigned char comb_count;
    unsigned char reserved_6c[4];
    unsigned char state;
    unsigned char req_state;
    unsigned char task;
    unsigned char reserved_73[6];
    unsigned char button_mask[4];
    unsigned char reserved_7d[3];
} PadDmaFrame;

typedef struct PadIopCommand {
    unsigned int sequence;
    int type;
    int size;
    unsigned char data[20];
} PadIopCommand;

typedef struct PadIopBuffer {
    PadIopCommand command;
    unsigned char reserved[32];
} PadIopBuffer;

typedef struct PadSlot {
    PadDmaFrame *dma;
    void *command;
    void *iop_addr;
    int dma_id;
    int opened;
    int reserved[2];
} PadSlot;

typedef struct PadVrefParam {
    unsigned char param[12];
} PadVrefParam;

typedef struct PadRpcOpen {
    int command;
    int port;
    int slot;
    int result;
    void *addr;
    void *iop_addr;
} PadRpcOpen;

typedef struct PadRpcBuffer {
    int command;
    union {
        struct {
            int port;
            int slot;
            int result;
            int param;
        } common;
        struct {
            int port;
            int slot;
            int mode;
            int lock;
            int result;
        } main_mode;
        struct {
            int port;
            int slot;
            unsigned char data[6];
            int result;
        } act_align;
        struct {
            int port;
            int slot;
            int info;
            int result;
        } button_info;
        struct {
            int port;
            int slot;
            PadVrefParam param;
            int reserved;
            int result;
        } vref;
        struct {
            int level;
            int result;
        } warning;
        unsigned char raw[124];
    } u;
} PadRpcBuffer;

static char __ps2_libinfo__[16] = "PsIIlibpad  2300";
static int isInit = 0;
static int isWarning = 1;

static sceSifClientData padsif[2] __attribute__((aligned(64)));
static PadSlot PadInfo[2][4];
static PadIopBuffer dma_iop_data[2][4] __attribute__((aligned(64)));
static PadRpcBuffer buffer __attribute__((aligned(64)));

extern int sceSifCallRpc(sceSifClientData *client, unsigned int fno,
                         unsigned int mode, void *send, int ssize,
                         void *receive, int rsize, void (*end_func)(void *),
                         void *end_param);
extern void SyncDCache(void *start, void *end);
extern void *memcpy(void *dest, const void *src, unsigned int n);
extern void *memset(void *dest, int c, unsigned int n);
extern char *strcpy(char *dest, const char *src);
extern int sceSifBindRpc(sceSifClientData *client, unsigned int request,
                         unsigned int mode);
extern int sceSifDmaStat(unsigned int id);
extern unsigned int sceSifSetDma(sceSifDmaData *sdd, int len);
extern int printf(const char *fmt, ...);
extern int scePadGetModVersion(void);


void _send_to_iop(int port, int slot) {
    sceSifDmaData transfer[16];
    PadIopCommand *command;
    char *dest;
    unsigned int id;

    command = PadInfo[port][slot].command;
    if (sceSifDmaStat(PadInfo[port][slot].dma_id) >= 0) {
        if (isWarning) {
            printf("libpad: sceSifSetDma faild\n");
        }
        return;
    }
    command->sequence++;
    dest = (char *)PadInfo[port][slot].iop_addr + ((command->sequence & 1) << 5);
    SyncDCache(command, command + 1);
    transfer[0].data = command;
    transfer[0].addr = dest;
    transfer[0].size = 0x20;
    transfer[0].mode = 0;
    id = sceSifSetDma(transfer, 1);
    if (id == 0) {
        if (isWarning) {
            printf("libpad: sceSifSetDma faild\n");
        }
    }
    PadInfo[port][slot].dma_id = id;
    /* retail keeps this message's literal from a disabled check */
    if (0) {
        printf("libpad: tPadDma Structure Invalid\n");
    }
}


int scePadInit(int mode) {
    int version;
    int wait;

    isInit = 1;
    while (1) {
        sceSifBindRpc(&padsif[0], 0x80000100, 0);
        if (padsif[0].server != 0) {
            break;
        }
        wait = 0x10000;
        while (wait--) {
        }
    }
    while (1) {
        sceSifBindRpc(&padsif[1], 0x80000101, 0);
        if (padsif[1].server != 0) {
            break;
        }
        wait = 0x10000;
        while (wait--) {
        }
    }
    version = scePadGetModVersion();
    if ((version >> 8) != 4) {
        if (isWarning) {
            printf("libpad: Module version mismatch ");
            printf("[libpad.a = %d.%d, padman.irx = %d.%d]\n", 4, 0, version >> 8, version & 0xFF);
        }
        return 0;
    }
    return scePadInit2(mode);
}


int scePadInit2(int mode) {
    int slot;

    for (slot = 0; slot < 4; slot++) {
        PadInfo[0][slot].opened = 0;
        PadInfo[0][slot].reserved[1] = 0;
        PadInfo[0][slot].reserved[0] = 0;
        PadInfo[1][slot].opened = 0;
        PadInfo[1][slot].reserved[1] = 0;
        PadInfo[1][slot].reserved[0] = 0;
    }
    buffer.command = 16;
    buffer.u.common.param = 0;
    if (sceSifCallRpc(&padsif[0], 1, 0, &buffer, 128, &buffer, 128, 0, 0) < 0) {
        return 0;
    }
    return buffer.u.common.result;
}


int scePadEnd(void) {
    int result;

    buffer.command = 15;
    if (sceSifCallRpc(&padsif[0], 1, 0, &buffer, 128, &buffer, 128, 0, 0) < 0) {
        return 0;
    }
    result = buffer.u.common.result;
    if (result == 1) {
        isInit = 0;
    }
    return result;
}


int scePadPortOpen(int port, int slot, void *addr) {
    PadDmaFrame *frame;
    PadIopCommand *command;
    PadRpcOpen *rpc;
    int i;

    if ((unsigned int)addr & 0x3F) {
        if (isWarning) {
            printf("libpad: buffer addr is not 64 byte align. %08x\n", addr);
        }
        return 0;
    }
    if (PadInfo[port][slot].opened == 1) {
        if (isWarning) {
            printf("libpad: pad port is already open [%d][%d]\n", port, slot);
        }
        return 0;
    }
    frame = addr;
    for (i = 0; i < 2; i++) {
        frame->frame = 0;
        frame->state = 5;
        frame->req_state = 2;
        frame->data_ready = 0;
        memset(frame->data, 0xFF, 0x20);
        frame->length = 0;
        frame++;
    }
    rpc = (PadRpcOpen *)&buffer;
    rpc->command = 1;
    rpc->port = port;
    rpc->slot = slot;
    rpc->addr = addr;
    if (sceSifCallRpc(&padsif[0], 1, 0, rpc, 128, rpc, 128, 0, 0) < 0) {
        return 0;
    }
    command = &dma_iop_data[port][slot].command;
    PadInfo[port][slot].opened = 1;
    PadInfo[port][slot].iop_addr = rpc->iop_addr;
    PadInfo[port][slot].dma = addr;
    PadInfo[port][slot].command = command;
    command->sequence = 0;
    PadInfo[port][slot].dma_id = 0;
    return rpc->result;
}


int scePadPortClose(int port, int slot) {
    if (PadInfo[port][slot].opened == 0) {
        return 0;
    }
    buffer.command = 14;
    buffer.u.common.port = port;
    buffer.u.common.slot = slot;
    buffer.u.common.param = 1;
    if (sceSifCallRpc(&padsif[0], 1, 0, &buffer, 128, &buffer, 128, 0, 0) < 0) {
        return 0;
    }
    PadInfo[port][slot].opened = 0;
    return buffer.u.common.result;
}


PadDmaFrame *scePadGetDmaStr(int port, int slot) {
    PadDmaFrame *frame = PadInfo[port][slot].dma;
    int index = 0;

    SyncDCache(frame, &frame[2]);
    if (frame[0].frame < frame[1].frame) {
        index = 1;
    }
    return &frame[index];
}


int scePadGetFrameCount(int port, int slot) {
    if (PadInfo[port][slot].opened == 0) {
        return 0;
    }
    return scePadGetDmaStr(port, slot)->frame;
}


int scePadRead(int port, int slot, unsigned char *rdata) {
    PadDmaFrame *frame;

    if (PadInfo[port][slot].opened == 0) {
        return 0;
    }
    frame = scePadGetDmaStr(port, slot);
    memcpy(rdata, frame->data, frame->length);
    return frame->length;
}


int scePadGetState(int port, int slot) {
    PadDmaFrame *frame;

    if (PadInfo[port][slot].opened == 0) {
        return 99;
    }
    frame = scePadGetDmaStr(port, slot);
    if (frame->state == 6) {
        if (frame->req_state == 2) {
            return 5;
        }
    }
    return frame->state;
}


static char *PadStateStr[8] = {
    "DISCONNECT", "", "FINDCTP1", "", "", "EXECCMD", "STABLE", "ERROR"
};

void scePadStateIntToStr(unsigned int state, char *str) {
    if (state < 8) {
        strcpy(str, PadStateStr[state]);
    } else {
        strcpy(str, "");
    }
}


int scePadSetReqState(int port, int slot, int state) {
    if (PadInfo[port][slot].opened == 0) {
        return 0;
    }
    scePadGetDmaStr(port, slot)->req_state = state;
    return 1;
}


int scePadGetReqState(int port, int slot) {
    if (PadInfo[port][slot].opened == 0) {
        return 0;
    }
    return scePadGetDmaStr(port, slot)->req_state;
}


static char *ReqStateStr[3] = { "COMPLETE", "FAILED", "BUSY" };

void scePadReqIntToStr(unsigned int state, char *str) {
    if (state < 4) {
        strcpy(str, ReqStateStr[state]);
    } else {
        strcpy(str, "");
    }
}


int scePadInfoAct(int port, int slot, int actno, int term) {
    PadDmaFrame *frame;

    if (PadInfo[port][slot].opened == 0) {
        return 0;
    }
    frame = scePadGetDmaStr(port, slot);
    if (frame->task != 1) {
        return 0;
    }
    if (frame->mode_config < 2) {
        return 0;
    }
    if (actno < frame->act_count) {
        if (actno == -1) {
            return frame->act_count;
        }
        switch (term) {
        case 1:
            return frame->act_info[actno][0];
        case 2:
            return frame->act_info[actno][1];
        case 3:
            return frame->act_info[actno][2];
        case 4:
            return frame->act_info[actno][3];
        }
    }
    return 0;
}


int scePadInfoComb(int port, int slot, int listno, int offs) {
    PadDmaFrame *frame;

    if (PadInfo[port][slot].opened == 0) {
        return 0;
    }
    frame = scePadGetDmaStr(port, slot);
    if (frame->task != 1) {
        return 0;
    }
    if (frame->mode_config < 2) {
        return 0;
    }
    if (listno == -1) {
        return frame->comb_count;
    }
    if (listno >= frame->comb_count) {
        return 0;
    }
    switch (offs) {
    case -1:
        return frame->comb_info[listno][0];
    case 0:
        return frame->comb_info[listno][1];
    case 1:
        return frame->comb_info[listno][2];
    case 2:
        return frame->comb_info[listno][3];
    }
    return 0;
}


int scePadInfoMode(int port, int slot, int term, int offs) {
    PadDmaFrame *frame;

    if (PadInfo[port][slot].opened == 0) {
        return 0;
    }
    frame = scePadGetDmaStr(port, slot);
    if (frame->task != 1) {
        return 0;
    }
    if (frame->req_state == 2) {
        return 0;
    }
    switch (term) {
    case 1:
        if (frame->mode_cur_id == 0xF3) {
            return 0;
        }
        return frame->mode_cur_id >> 4;
    case 2:
        if (frame->mode_config == 1) {
            return 0;
        }
        return frame->mode_table[frame->mode_cur_offset];
    case 3:
        if (frame->mode_config == 1) {
            return 0;
        }
        return frame->mode_cur_offset;
    case 4:
        if (frame->mode_config == 1) {
            return 0;
        }
        if (offs == -1) {
            return frame->mode_count;
        }
        if (offs < frame->mode_count) {
            return frame->mode_table[offs];
        }
        return 0;
    }
    return 0;
}


int scePadSetMainMode(int port, int slot, int mode, int lock) {
    buffer.command = 6;
    buffer.u.main_mode.port = port;
    buffer.u.main_mode.slot = slot;
    buffer.u.main_mode.mode = mode;
    buffer.u.main_mode.lock = lock;
    if (sceSifCallRpc(&padsif[0], 1, 0, &buffer, 128, &buffer, 128, 0, 0) < 0) {
        return 0;
    }
    if (buffer.u.main_mode.result == 1) {
        scePadSetReqState(port, slot, 2);
    }
    return buffer.u.main_mode.result;
}


int scePadSetActDirect(int port, int slot, const unsigned char *data) {
    PadIopCommand *command;
    int i;

    if (scePadGetDmaStr(port, slot)->task != 1) {
        return 0;
    }
    command = PadInfo[port][slot].command;
    for (i = 0; i < 6; i++) {
        command->data[i] = data[i];
    }
    command->size = 6;
    command->type = 1;
    _send_to_iop(port, slot);
    return 1;
}


int scePadSetActAlign(int port, int slot, const unsigned char *data) {
    PadRpcBuffer *rpc = &buffer;
    int i;

    rpc->command = 8;
    rpc->u.act_align.port = port;
    rpc->u.act_align.slot = slot;
    for (i = 0; i < 6; i++) {
        rpc->u.act_align.data[i] = data[i];
    }
    if (sceSifCallRpc(&padsif[0], 1, 0, rpc, 128, rpc, 128, 0, 0) < 0) {
        return 0;
    }
    if (rpc->u.act_align.result == 1) {
        scePadSetReqState(port, slot, 2);
    }
    return rpc->u.act_align.result;
}


int scePadGetButtonMask(int port, int slot) {
    PadDmaFrame *frame;

    if (PadInfo[port][slot].opened == 0) {
        return 0;
    }
    frame = scePadGetDmaStr(port, slot);
    if (frame->task != 1) {
        return 0;
    }
    if (frame->mode_config < 2) {
        return 0;
    }
    if (frame->model < 2) {
        return 0;
    }
    return frame->button_mask[0] + ((long)frame->button_mask[1] << 8)
         + ((long)frame->button_mask[2] << 16) + ((long)frame->button_mask[3] << 24);
}


int scePadSetButtonInfo(int port, int slot, int info) {
    buffer.command = 10;
    buffer.u.button_info.port = port;
    buffer.u.button_info.slot = slot;
    buffer.u.button_info.info = info;
    if (sceSifCallRpc(&padsif[0], 1, 0, &buffer, 128, &buffer, 128, 0, 0) < 0) {
        return 0;
    }
    if (buffer.u.button_info.result == 1) {
        scePadSetReqState(port, slot, 2);
    }
    return buffer.u.button_info.result;
}


int scePadInfoPressMode(int port, int slot) {
    if (PadInfo[port][slot].opened == 0) {
        return 0;
    }
    return scePadGetButtonMask(port, slot) == 0x3FFFF;
}


int scePadEnterPressMode(int port, int slot) {
    if (PadInfo[port][slot].opened == 0) {
        return 0;
    }
    return scePadSetButtonInfo(port, slot, 0xFFF);
}


int scePadExitPressMode(int port, int slot) {
    if (PadInfo[port][slot].opened == 0) {
        return 0;
    }
    return scePadSetButtonInfo(port, slot, 0);
}


int scePadSetVrefParam(int port, int slot, const PadVrefParam *param) {
    buffer.command = 11;
    buffer.u.vref.port = port;
    buffer.u.vref.slot = slot;
    buffer.u.vref.param = *param;
    if (sceSifCallRpc(&padsif[0], 1, 0, &buffer, 128, &buffer, 128, 0, 0) < 0) {
        return 0;
    }
    if (buffer.u.vref.result == 1) {
        scePadSetReqState(port, slot, 2);
    }
    return buffer.u.vref.result;
}


int scePadGetPortMax(void) {
    buffer.command = 12;
    if (sceSifCallRpc(&padsif[0], 1, 0, &buffer, 128, &buffer, 128, 0, 0) < 0) {
        return 0;
    }
    return buffer.u.common.result;
}


int scePadGetSlotMax(int port) {
    buffer.command = 13;
    buffer.u.common.port = port;
    if (sceSifCallRpc(&padsif[0], 1, 0, &buffer, 128, &buffer, 128, 0, 0) < 0) {
        return 0;
    }
    return buffer.u.common.result;
}


int scePadGetModVersion(void) {
    buffer.command = 18;
    if (sceSifCallRpc(&padsif[0], 1, 0, &buffer, 128, &buffer, 128, 0, 0) < 0) {
        return 0;
    }
    return buffer.u.common.result;
}


int scePadSetWarningLevel(int level) {
    buffer.command = 20;
    buffer.u.warning.level = level;
    if (sceSifCallRpc(&padsif[0], 1, 0, &buffer, 128, &buffer, 128, 0, 0) < 0) {
        return 0;
    }
    return buffer.u.warning.result;
}
