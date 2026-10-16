/*
 * Unit: sdk/sce/sifrpc
 *
 * Functions:
 *   0x0027b368 sceSifInitRpc
 *   0x0027b508 sceSifExitRpc
 *   0x0027b530 _sceRpcGetPacket
 *   0x0027b5d8 _sceRpcFreePacket
 *   0x0027b5f8 _sceRpcGetFPacket
 *   0x0027b628 _sceRpcGetFPacket2
 *   0x0027b668 _request_end
 *   0x0027b720 _request_rdata
 *   0x0027b780 sceSifGetOtherData
 *   0x0027b8d8 _search_svdata
 *   0x0027b928 _request_bind
 *   0x0027b9d8 sceSifBindRpc
 *   0x0027bb18 _request_call
 *   0x0027bba8 sceSifCallRpc
 *   0x0027bd98 sceSifCheckStatRpc
 *   0x0027bdd8 sceSifSetRpcQueue
 *   0x0027be70 sceSifRegisterRpc
 *   0x0027bf40 sceSifRemoveRpc
 *   0x0027bfd8 sceSifRemoveRpcQueue
 *   0x0027c068 sceSifGetNextRequest
 *   0x0027c0c0 sceSifExecRequest
 *   0x0027c288 sceSifRpcLoop
 */

typedef struct SifCmdHeader {
    unsigned int psize : 8;
    unsigned int dsize : 24;
    unsigned int dest;
    int cid;
    unsigned int opt;
} SifCmdHeader;

typedef struct sceSifRpcHeader {
    void *pkt_addr;
    unsigned int rpc_id;
    int sema_id;
    unsigned int mode;
} sceSifRpcHeader;

typedef void (*sceSifEndFunc)(void *param);

typedef struct sceSifClientData {
    sceSifRpcHeader hdr;
    unsigned int command;
    void *buff;
    void *cbuff;
    sceSifEndFunc end_function;
    void *end_param;
    void *serve;
} sceSifClientData;

typedef struct sceSifReceiveData {
    sceSifRpcHeader hdr;
    void *src;
    void *dest;
    int size;
} sceSifReceiveData;

typedef void *(*sceSifRpcFunc)(unsigned int fno, void *data, int size);

typedef struct sceSifServeData {
    unsigned int command;
    sceSifRpcFunc func;
    void *buff;
    int size;
    sceSifRpcFunc cfunc;
    void *cbuff;
    int csize;
    sceSifClientData *client;
    void *paddr;
    unsigned int fno;
    void *receive;
    int rsize;
    int rmode;
    unsigned int rid;
    struct sceSifServeData *link;
    struct sceSifServeData *next;
    struct sceSifQueueData *base;
} sceSifServeData;

typedef struct sceSifQueueData {
    int key;
    int active;
    struct sceSifServeData *link;
    struct sceSifServeData *start;
    struct sceSifServeData *end;
    struct sceSifQueueData *next;
} sceSifQueueData;

typedef struct SifRpcPktHeader {
    SifCmdHeader sifcmd;
    unsigned int rec_id;
    unsigned int pkt_addr;
    unsigned int rpc_id;
} SifRpcPktHeader;

typedef struct SifRpcRendPkt {
    SifRpcPktHeader header;
    unsigned int client;
    unsigned int cid;
    void *server;
    void *buff;
    void *cbuff;
} SifRpcRendPkt;

typedef struct SifRpcBindPkt {
    SifRpcPktHeader header;
    unsigned int client;
    unsigned int sid;
} SifRpcBindPkt;

typedef struct SifRpcCallPkt {
    SifRpcPktHeader header;
    unsigned int client;
    unsigned int rpc_number;
    int send_size;
    void *receive;
    int rsize;
    int rmode;
    void *server;
} SifRpcCallPkt;

typedef struct SifRpcOtherDataPkt {
    SifRpcPktHeader header;
    sceSifReceiveData *receive;
    unsigned int src;
    unsigned int dest;
    int size;
} SifRpcOtherDataPkt;

typedef union SifRpcPacket {
    SifRpcPktHeader header;
    SifRpcRendPkt rend;
    SifRpcBindPkt bind;
    SifRpcCallPkt call;
    SifRpcOtherDataPkt other;
    unsigned char raw[64];
} SifRpcPacket;

typedef struct sceSifDmaData {
    unsigned int data;
    unsigned int addr;
    unsigned int size;
    unsigned int mode;
} sceSifDmaData;

typedef struct SemaParam {
    int currentCount;
    int maxCount;
    int initCount;
    int numWaitThreads;
    unsigned int attr;
    unsigned int option;
} SemaParam;

typedef struct SifRpcData {
    unsigned int pid;
    SifRpcPacket *pkt_table;
    int pkt_table_len;
    int unused1;
    int unused2;
    SifRpcPacket *rdata_table;
    int rdata_table_len;
    SifRpcPacket *client_table;
    int client_table_len;
    int rdata_table_idx;
    sceSifQueueData *active_queue;
} SifRpcData;

extern int D_00613378[];
extern SifRpcPacket D_00703940[];
extern SifRpcPacket D_00704140[];
extern SifRpcPacket D_00704940[];
extern SifRpcData D_00705140;

#define sif_rpc_initialized D_00613378
#define _packet_buffer D_00703940
#define _free_buffer D_00704140
#define _free_buffer2 D_00704940
#define _sif_rpc_data D_00705140

#define SIF_CMD_RPC_END   0x80000008
#define SIF_CMD_RPC_BIND  0x80000009
#define SIF_CMD_RPC_CALL  0x8000000A
#define SIF_CMD_RPC_RDATA 0x8000000C

#define SIF_CMD_INIT_CMD   0x80000002
#define SIF_SYSREG_RPCINIT 0x80000002
#define SIF_SREG_RPCINIT   0

#define SIF_RPC_M_NOWAIT 0x01
#define SIF_RPC_M_NOWBDC 0x02

#define PACKET_F_ALLOC 0x01
#define PACKET_F_CLIENT 0x04
#define PACKET_TABLE_LEN 32
#define UNCACHED(p) ((SifRpcPacket *)((unsigned int)(p) | 0x20000000))

extern int DIntr(void);
extern int EIntr(void);
extern int CreateSema(SemaParam *param);
extern int DeleteSema(int sema);
extern int WaitSema(int sema);
extern int iSignalSema(int sema);
extern void SleepThread(void);
typedef void (*SifCmdHandler)(void *packet, void *data);

extern void sceSifInitCmd(void);
extern void sceSifExitCmd(void);
extern void sceSifAddCmdHandler(int cid, SifCmdHandler handler, void *data);
extern unsigned int sceSifGetReg(unsigned int reg);
extern int sceSifSetReg(unsigned int reg, unsigned int value);
extern unsigned int sceSifGetSreg(int index);
extern void sceSifWriteBackDCache(void *ptr, int size);
extern unsigned int sceSifSetDma(sceSifDmaData *sdd, int len);
extern unsigned int sceSifSendCmd(int cid, void *packet, int packet_size,
                                  void *src, void *dest, int size);
extern unsigned int isceSifSendCmd(int cid, void *packet, int packet_size,
                                   void *src, void *dest, int size);

void _request_end(SifRpcRendPkt *request, void *data);
void _request_rdata(SifRpcOtherDataPkt *request, SifRpcData *rpc_data);
void _request_bind(SifRpcBindPkt *request, SifRpcData *rpc_data);
void _request_call(void *request, SifRpcData *rpc_data);


void sceSifInitRpc(unsigned int mode) {
    SifCmdHeader *init_cmd;

    DIntr();
    if (sif_rpc_initialized[0]) {
        EIntr();
        return;
    }
    sif_rpc_initialized[0] = 1;
    EIntr();
    sceSifInitCmd();
    DIntr();
    _sif_rpc_data.pkt_table = UNCACHED(_packet_buffer);
    _sif_rpc_data.pkt_table_len = PACKET_TABLE_LEN;
    _sif_rpc_data.unused1 = 0;
    _sif_rpc_data.unused2 = 0;
    _sif_rpc_data.rdata_table = UNCACHED(_free_buffer);
    _sif_rpc_data.rdata_table_len = PACKET_TABLE_LEN;
    _sif_rpc_data.client_table = UNCACHED(_free_buffer2);
    _sif_rpc_data.client_table_len = PACKET_TABLE_LEN;
    _sif_rpc_data.rdata_table_idx = 0;
    _sif_rpc_data.pid = 1;
    sceSifAddCmdHandler(SIF_CMD_RPC_END, (SifCmdHandler)_request_end, &_sif_rpc_data);
    sceSifAddCmdHandler(SIF_CMD_RPC_BIND, (SifCmdHandler)_request_bind, &_sif_rpc_data);
    sceSifAddCmdHandler(SIF_CMD_RPC_CALL, (SifCmdHandler)_request_call, &_sif_rpc_data);
    sceSifAddCmdHandler(SIF_CMD_RPC_RDATA, (SifCmdHandler)_request_rdata, &_sif_rpc_data);
    EIntr();
    if (sceSifGetReg(SIF_SYSREG_RPCINIT)) {
        return;
    }
    init_cmd = &_packet_buffer[1].header.sifcmd;
    init_cmd->opt = 1;
    sceSifSendCmd(SIF_CMD_INIT_CMD, init_cmd, sizeof(SifCmdHeader), 0, 0, 0);
    while (sceSifGetSreg(SIF_SREG_RPCINIT) == 0) {
    }
    sceSifSetReg(SIF_SYSREG_RPCINIT, 1);
}


void sceSifExitRpc(void) {
    sceSifExitCmd();
    sif_rpc_initialized[0] = 0;
}


SifRpcPacket *_sceRpcGetPacket(SifRpcData *rpc_data) {
    SifRpcPacket *packet;
    unsigned int rpc_id;
    int rid;

    DIntr();
    packet = rpc_data->pkt_table;
    for (rid = 0; rid < rpc_data->pkt_table_len; rid++, packet++) {
        if (!(packet->header.rec_id & PACKET_F_ALLOC)) {
            packet->header.rec_id = (rid << 16) | 0x04 | PACKET_F_ALLOC;
            if (++rpc_data->pid == 1) {
                rpc_data->pid++;
                rpc_id = 1;
            } else {
                rpc_id = rpc_data->pid;
            }
            packet->header.pkt_addr = (unsigned int)packet;
            packet->header.rpc_id = rpc_id;
            EIntr();
            return packet;
        }
    }
    EIntr();
    return 0;
}


void _sceRpcFreePacket(SifRpcPacket *packet) {
    packet->header.rpc_id = 0;
    packet->header.rec_id &= ~PACKET_F_ALLOC;
}


SifRpcPacket *_sceRpcGetFPacket(SifRpcData *rpc_data) {
    SifRpcPacket *packet;
    int index;

    index = rpc_data->rdata_table_idx % rpc_data->rdata_table_len;
    packet = &rpc_data->rdata_table[index];
    rpc_data->rdata_table_idx = index + 1;
    return packet;
}


SifRpcPacket *_sceRpcGetFPacket2(SifRpcData *rpc_data, int rid) {
    if (rid < 0 || rid >= rpc_data->client_table_len) {
        return _sceRpcGetFPacket(rpc_data);
    }
    return &rpc_data->client_table[rid];
}


void _request_end(SifRpcRendPkt *request, void *data) {
    sceSifClientData *client;

    switch (request->cid) {
    case SIF_CMD_RPC_CALL:
        client = (sceSifClientData *)request->client;
        if (client->end_function) {
            client->end_function(client->end_param);
        }
        break;
    case SIF_CMD_RPC_BIND:
        client = (sceSifClientData *)request->client;
        client->serve = request->server;
        client->buff = request->buff;
        client->cbuff = request->cbuff;
        break;
    case SIF_CMD_RPC_RDATA:
        break;
    }
    client = (sceSifClientData *)request->client;
    if (client->hdr.sema_id >= 0) {
        iSignalSema(client->hdr.sema_id);
    }
    _sceRpcFreePacket(client->hdr.pkt_addr);
    client->hdr.pkt_addr = 0;
}


void _request_rdata(SifRpcOtherDataPkt *request, SifRpcData *rpc_data) {
    SifRpcRendPkt *reply;

    reply = &_sceRpcGetFPacket(rpc_data)->rend;
    reply->header.pkt_addr = request->header.pkt_addr;
    reply->client = (unsigned int)request->receive;
    reply->cid = SIF_CMD_RPC_RDATA;
    isceSifSendCmd(SIF_CMD_RPC_END, reply, sizeof(SifRpcPacket),
                   (void *)request->src, (void *)request->dest, request->size);
}


int sceSifGetOtherData(sceSifReceiveData *rd, void *src, void *dest, int size, unsigned int mode) {
    SifRpcOtherDataPkt *packet;
    SemaParam sema;

    packet = &_sceRpcGetPacket(&_sif_rpc_data)->other;
    if (packet == 0) {
        return -1;
    }
    rd->hdr.pkt_addr = packet;
    rd->hdr.rpc_id = packet->header.rpc_id;
    packet->src = (unsigned int)src;
    packet->dest = (unsigned int)dest;
    packet->size = size;
    packet->header.pkt_addr = (unsigned int)packet;
    packet->receive = rd;
    if ((mode & SIF_RPC_M_NOWAIT) == 0) {
        sema.initCount = 0;
        sema.maxCount = 1;
        rd->hdr.sema_id = CreateSema(&sema);
        if (rd->hdr.sema_id < 0) {
            _sceRpcFreePacket((SifRpcPacket *)packet);
            return -3;
        }
        if (sceSifSendCmd(SIF_CMD_RPC_RDATA, packet, sizeof(SifRpcPacket), 0, 0, 0) == 0) {
            _sceRpcFreePacket((SifRpcPacket *)packet);
            DeleteSema(rd->hdr.sema_id);
            return -2;
        }
        WaitSema(rd->hdr.sema_id);
        DeleteSema(rd->hdr.sema_id);
        return 0;
    }
    rd->hdr.sema_id = -1;
    if (sceSifSendCmd(SIF_CMD_RPC_RDATA, packet, sizeof(SifRpcPacket), 0, 0, 0) == 0) {
        _sceRpcFreePacket((SifRpcPacket *)packet);
        return -2;
    }
    return 0;
}


sceSifServeData *_search_svdata(unsigned int command, SifRpcData *rpc_data) {
    sceSifQueueData *queue;
    sceSifServeData *serve;

    for (queue = rpc_data->active_queue; queue; queue = queue->next) {
        for (serve = queue->link; serve; serve = serve->link) {
            if (serve->command == command) {
                return serve;
            }
        }
    }
    return 0;
}


void _request_bind(SifRpcBindPkt *request, SifRpcData *rpc_data) {
    SifRpcRendPkt *reply;
    sceSifServeData *serve;
    unsigned int pkt_addr;
    unsigned int client;

    reply = &_sceRpcGetFPacket(rpc_data)->rend;
    pkt_addr = request->header.pkt_addr;
    client = request->client;
    reply->header.pkt_addr = pkt_addr;
    reply->client = client;
    reply->cid = SIF_CMD_RPC_BIND;
    serve = _search_svdata(request->sid, rpc_data);
    if (serve == 0) {
        reply->server = 0;
        reply->buff = 0;
        reply->cbuff = 0;
    } else {
        reply->server = serve;
        reply->buff = serve->buff;
        reply->cbuff = serve->cbuff;
    }
    isceSifSendCmd(SIF_CMD_RPC_END, reply, sizeof(SifRpcPacket), 0, 0, 0);
}


int sceSifBindRpc(sceSifClientData *cd, unsigned int request, unsigned int mode) {
    SifRpcBindPkt *packet;
    SemaParam sema;

    cd->command = 0;
    cd->serve = 0;
    packet = &_sceRpcGetPacket(&_sif_rpc_data)->bind;
    if (packet == 0) {
        return -1;
    }
    cd->hdr.pkt_addr = packet;
    cd->hdr.rpc_id = packet->header.rpc_id;
    packet->sid = request;
    packet->header.pkt_addr = (unsigned int)packet;
    packet->client = (unsigned int)cd;
    if ((mode & SIF_RPC_M_NOWAIT) == 0) {
        sema.initCount = 0;
        sema.maxCount = 1;
        cd->hdr.sema_id = CreateSema(&sema);
        if (cd->hdr.sema_id < 0) {
            _sceRpcFreePacket((SifRpcPacket *)packet);
            return -3;
        }
        if (sceSifSendCmd(SIF_CMD_RPC_BIND, packet, sizeof(SifRpcPacket), 0, 0, 0) == 0) {
            _sceRpcFreePacket((SifRpcPacket *)packet);
            DeleteSema(cd->hdr.sema_id);
            return -2;
        }
        WaitSema(cd->hdr.sema_id);
        DeleteSema(cd->hdr.sema_id);
        return 0;
    }
    cd->hdr.sema_id = -1;
    if (sceSifSendCmd(SIF_CMD_RPC_BIND, packet, sizeof(SifRpcPacket), 0, 0, 0) == 0) {
        _sceRpcFreePacket((SifRpcPacket *)packet);
        return -2;
    }
    return 0;
}


int sceSifCallRpc(sceSifClientData *cd, unsigned int fno, unsigned int mode, void *send, int ssize,
                  void *receive, int rsize, sceSifEndFunc end_func, void *end_param) {
    SifRpcCallPkt *packet;
    SemaParam sema;

    packet = &_sceRpcGetPacket(&_sif_rpc_data)->call;
    if (packet == 0) {
        return -1;
    }
    cd->end_param = end_param;
    cd->hdr.pkt_addr = packet;
    cd->hdr.rpc_id = packet->header.rpc_id;
    cd->end_function = end_func;
    packet->rpc_number = fno;
    packet->send_size = ssize;
    packet->receive = receive;
    packet->rsize = rsize;
    packet->header.pkt_addr = (unsigned int)packet;
    packet->server = cd->serve;
    packet->client = (unsigned int)cd;
    if ((mode & SIF_RPC_M_NOWBDC) == 0) {
        if (send == receive) {
            sceSifWriteBackDCache(send, ssize < rsize ? rsize : ssize);
        } else {
            if (ssize > 0) {
                sceSifWriteBackDCache(send, ssize);
            }
            if (rsize > 0) {
                sceSifWriteBackDCache(receive, rsize);
            }
        }
    }
    if (mode & SIF_RPC_M_NOWAIT) {
        if (end_func == 0) {
            packet->rmode = 0;
        } else {
            packet->rmode = 1;
        }
        cd->hdr.sema_id = -1;
        if (sceSifSendCmd(SIF_CMD_RPC_CALL, packet, sizeof(SifRpcPacket), send, cd->buff, ssize) != 0) {
            return 0;
        }
        _sceRpcFreePacket((SifRpcPacket *)packet);
        return -2;
    }
    sema.initCount = 0;
    sema.maxCount = 1;
    cd->hdr.sema_id = CreateSema(&sema);
    if (cd->hdr.sema_id < 0) {
        _sceRpcFreePacket((SifRpcPacket *)packet);
        return -3;
    }
    packet->rmode = 1;
    if (sceSifSendCmd(SIF_CMD_RPC_CALL, packet, sizeof(SifRpcPacket), send, cd->buff, ssize) == 0) {
        DeleteSema(cd->hdr.sema_id);
        _sceRpcFreePacket((SifRpcPacket *)packet);
        return -2;
    }
    WaitSema(cd->hdr.sema_id);
    DeleteSema(cd->hdr.sema_id);
    return 0;
}


void sceSifSetRpcQueue(sceSifQueueData *qd, int key) {
    sceSifQueueData *queue;

    DIntr();
    qd->key = key;
    qd->active = 0;
    qd->link = 0;
    qd->start = 0;
    qd->end = 0;
    qd->next = 0;
    if (_sif_rpc_data.active_queue == 0) {
        _sif_rpc_data.active_queue = qd;
    } else {
        for (queue = _sif_rpc_data.active_queue; queue->next; queue = queue->next) {
        }
        queue->next = qd;
    }
    EIntr();
}


void sceSifRegisterRpc(sceSifServeData *sd, unsigned int command, sceSifRpcFunc func, void *buff,
                       sceSifRpcFunc cfunc, void *cbuff, sceSifQueueData *qd) {
    sceSifServeData *server;

    DIntr();
    sd->next = 0;
    sd->link = 0;
    sd->command = command;
    sd->func = func;
    sd->buff = buff;
    sd->cfunc = cfunc;
    sd->cbuff = cbuff;
    sd->base = qd;
    if (qd->link == 0) {
        qd->link = sd;
    } else {
        server = qd->link;
        while (server->link) {
            server = server->link;
        }
        server->link = sd;
    }
    EIntr();
}


sceSifServeData *sceSifRemoveRpc(sceSifServeData *sd, sceSifQueueData *qd) {
    sceSifServeData *server;

    DIntr();
    if ((server = qd->link) == sd) {
        qd->link = server->link;
    } else {
        for (; server; server = server->link) {
            if (server->link == sd) {
                server->link = sd->link;
                break;
            }
        }
    }
    EIntr();
    return server;
}


sceSifQueueData *sceSifRemoveRpcQueue(sceSifQueueData *qd) {
    sceSifQueueData *queue;

    DIntr();
    if ((queue = _sif_rpc_data.active_queue) == qd) {
        _sif_rpc_data.active_queue = queue->next;
    } else {
        for (; queue; queue = queue->next) {
            if (queue->next == qd) {
                queue->next = qd->next;
                break;
            }
        }
    }
    EIntr();
    return queue;
}

sceSifServeData *sceSifGetNextRequest(sceSifQueueData *queue) {
    sceSifServeData *serve;

    DIntr();
    serve = queue->start;
    if (serve == 0) {
        queue->active = 0;
    } else {
        queue->active = 1;
        queue->start = serve->next;
    }
    EIntr();
    return serve;
}


void sceSifExecRequest(sceSifServeData *sd) {
    SifRpcRendPkt *reply;
    sceSifDmaData dma[2];
    void *result;
    int rsize;
    int count;
    int delay;
    unsigned int dma_id;

    rsize = 0;
    result = sd->func(sd->fno, sd->buff, sd->size);
    if (result) {
        rsize = sd->rsize;
    }
    if (sd->size > 0) {
        sceSifWriteBackDCache(sd->buff, sd->size);
    }
    if (rsize > 0) {
        sceSifWriteBackDCache(result, rsize);
    }
    DIntr();
    if (sd->rid & PACKET_F_CLIENT) {
        reply = &_sceRpcGetFPacket2(&_sif_rpc_data, sd->rid >> 16)->rend;
    } else {
        reply = &_sceRpcGetFPacket(&_sif_rpc_data)->rend;
    }
    EIntr();
    reply->cid = SIF_CMD_RPC_CALL;
    reply->client = (unsigned int)sd->client;
    if (sd->rmode) {
        while (sceSifSendCmd(SIF_CMD_RPC_END, reply, sizeof(SifRpcPacket), result, sd->receive, rsize) == 0) {
        }
        return;
    }
    reply->header.rpc_id = 0;
    reply->header.rec_id = 0;
    count = 0;
    if (rsize > 0) {
        count = 1;
        dma[0].data = (unsigned int)result;
        dma[0].addr = (unsigned int)sd->receive;
        dma[0].size = rsize;
        dma[0].mode = 0;
    }
    dma[count].data = (unsigned int)reply;
    dma[count].addr = (unsigned int)sd->paddr;
    dma[count].size = sizeof(SifRpcPacket);
    dma[count].mode = 0;
    count++;
    do {
        dma_id = sceSifSetDma(dma, count);
        if (dma_id == 0) {
            delay = 0x100000;
            while (delay--) {
            }
        }
    } while (dma_id == 0);
}


void sceSifRpcLoop(sceSifQueueData *queue) {
    sceSifServeData *serve;

    while (1) {
        while ((serve = sceSifGetNextRequest(queue))) {
            sceSifExecRequest(serve);
        }
        SleepThread();
    }
}
