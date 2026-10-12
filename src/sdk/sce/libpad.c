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

typedef struct PadDmaFrame {
    unsigned char data[32];
    unsigned int act_direct[2];
    unsigned int act_align[2];
    unsigned char act_info[32];
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
    unsigned char reserved_73[13];
} PadDmaFrame;

typedef struct PadSlot {
    PadDmaFrame *dma;
    void *iop_status;
    void *iop_addr;
    int dma_id;
    int opened;
    int reserved[2];
} PadSlot;

typedef struct PadRpcBuffer {
    int command;
    int port;
    int slot;
    int result;
    int param;
    void *iop_addr;
    unsigned char reserved[104];
} PadRpcBuffer;

extern sceSifClientData D_006FFAC0[2];
extern PadSlot D_006FFB10[2][4];
extern PadRpcBuffer D_006FFE00;
extern int D_00610930[];
extern char *D_00610938[];
extern char *D_00610958[];

#define pad_clients     D_006FFAC0
#define pad_slots       D_006FFB10
#define pad_rpc         D_006FFE00
#define pad_initialized D_00610930
#define pad_state_names D_00610938
#define pad_req_names   D_00610958

extern int sceSifCallRpc(sceSifClientData *client, unsigned int fno,
                         unsigned int mode, void *send, int ssize,
                         void *receive, int rsize, void (*end_func)(void *),
                         void *end_param);
extern int SyncDCache(void *start, void *end);
extern void *memcpy(void *dest, const void *src, unsigned int n);
extern char *strcpy(char *dest, const char *src);
int scePadGetButtonMask(int port, int slot);
int scePadSetButtonInfo(int port, int slot, int info);


int scePadEnd(void) {
    int result;

    pad_rpc.command = 15;
    if (sceSifCallRpc(&pad_clients[0], 1, 0, &pad_rpc, 128, &pad_rpc, 128, 0, 0) < 0) {
        return 0;
    }
    result = pad_rpc.result;
    if (result == 1) {
        pad_initialized[0] = 0;
    }
    return result;
}


PadDmaFrame *scePadGetDmaStr(int port, int slot) {
    PadDmaFrame *frame = pad_slots[port][slot].dma;
    int index = 0;

    SyncDCache(frame, &frame[2]);
    if (frame[0].frame < frame[1].frame) {
        index = 1;
    }
    return &frame[index];
}


int scePadGetFrameCount(int port, int slot) {
    if (pad_slots[port][slot].opened == 0) {
        return 0;
    }
    return scePadGetDmaStr(port, slot)->frame;
}


int scePadRead(int port, int slot, unsigned char *rdata) {
    PadDmaFrame *frame;

    if (pad_slots[port][slot].opened == 0) {
        return 0;
    }
    frame = scePadGetDmaStr(port, slot);
    memcpy(rdata, frame->data, frame->length);
    return frame->length;
}


int scePadGetState(int port, int slot) {
    PadDmaFrame *frame;

    if (pad_slots[port][slot].opened == 0) {
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


void scePadStateIntToStr(unsigned int state, char *str) {
    if (state < 8) {
        strcpy(str, pad_state_names[state]);
    } else {
        strcpy(str, "");
    }
}


int scePadSetReqState(int port, int slot, int state) {
    if (pad_slots[port][slot].opened == 0) {
        return 0;
    }
    scePadGetDmaStr(port, slot)->req_state = state;
    return 1;
}


int scePadGetReqState(int port, int slot) {
    if (pad_slots[port][slot].opened == 0) {
        return 0;
    }
    return scePadGetDmaStr(port, slot)->req_state;
}


void scePadReqIntToStr(unsigned int state, char *str) {
    if (state < 4) {
        strcpy(str, pad_req_names[state]);
    } else {
        strcpy(str, "");
    }
}


int scePadInfoPressMode(int port, int slot) {
    if (pad_slots[port][slot].opened == 0) {
        return 0;
    }
    return scePadGetButtonMask(port, slot) == 0x3FFFF;
}


int scePadEnterPressMode(int port, int slot) {
    if (pad_slots[port][slot].opened == 0) {
        return 0;
    }
    return scePadSetButtonInfo(port, slot, 0xFFF);
}


int scePadExitPressMode(int port, int slot) {
    if (pad_slots[port][slot].opened == 0) {
        return 0;
    }
    return scePadSetButtonInfo(port, slot, 0);
}
