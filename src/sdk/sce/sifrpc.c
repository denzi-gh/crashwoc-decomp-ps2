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
    sceSifServeData *link;
    sceSifServeData *start;
    sceSifServeData *end;
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
    SifRpcOtherDataPkt other;
    unsigned char raw[64];
} SifRpcPacket;

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

#define sif_rpc_initialized D_00613378

#define SIF_CMD_RPC_END   0x80000008
#define SIF_CMD_RPC_BIND  0x80000009
#define SIF_CMD_RPC_CALL  0x8000000A
#define SIF_CMD_RPC_RDATA 0x8000000C

#define PACKET_F_ALLOC 0x01

extern int DIntr(void);
extern int EIntr(void);
extern int iSignalSema(int sema);
extern void SleepThread(void);
extern void sceSifExitCmd(void);
extern unsigned int isceSifSendCmd(int cid, void *packet, int packet_size,
                                   void *src, void *dest, int size);
extern void sceSifExecRequest(sceSifServeData *serve);


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


void sceSifRpcLoop(sceSifQueueData *queue) {
    sceSifServeData *serve;

    while (1) {
        while ((serve = sceSifGetNextRequest(queue))) {
            sceSifExecRequest(serve);
        }
        SleepThread();
    }
}
