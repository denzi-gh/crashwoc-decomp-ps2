/*
 * Unit: sdk/sce/deci2
 *
 * Functions:
 *   0x00280c20 sceDeci2Open
 *   0x00280c68 sceDeci2Close
 *   0x00280c90 sceDeci2ReqSend
 *   0x00280cc0 sceDeci2Poll
 *   0x00280ce8 sceDeci2ExRecv
 *   0x00280d20 sceDeci2ExSend
 *   0x00280d58 sceDeci2ExReqSend
 *   0x00280d88 sceDeci2ExLock
 *   0x00280db0 sceDeci2ExUnLock
 *   0x00280dd8 kputs
 */


#define UNCACHED_SEGMENT 0x20000000

typedef void (*Deci2Handler)(int event, int param, void *opt);

typedef struct {
    int protocol;
    void *opt;
    Deci2Handler handler;
    void *work;
} Deci2OpenParam;

typedef struct {
    int socket;
} Deci2SocketParam;

typedef struct {
    int socket;
    int dest;
} Deci2ReqSendParam;

typedef struct {
    int socket;
    void *buf;
    int len;
} Deci2TransferParam;

typedef struct {
    const char *str;
} Deci2PutsParam;

static unsigned char userarea[36];

extern int Deci2Call(int func, void *param);


int sceDeci2Open(unsigned short protocol, void *opt, Deci2Handler handler) {
    Deci2OpenParam param;

    param.protocol = protocol;
    param.opt = opt;
    param.handler = handler;
    param.work = (void *)((unsigned int)userarea | UNCACHED_SEGMENT);
    return Deci2Call(1, &param);
}


int sceDeci2Close(int socket) {
    Deci2SocketParam param;

    param.socket = socket;
    return Deci2Call(2, &param);
}


int sceDeci2ReqSend(int socket, char dest) {
    Deci2ReqSendParam param;

    param.socket = socket;
    param.dest = dest;
    return Deci2Call(3, &param);
}


int sceDeci2Poll(int socket) {
    Deci2SocketParam param;

    param.socket = socket;
    return Deci2Call(4, &param);
}


int sceDeci2ExRecv(int socket, void *buf, unsigned short len) {
    Deci2TransferParam param;

    param.socket = socket;
    param.buf = buf;
    param.len = len;
    return Deci2Call(-5, &param);
}


int sceDeci2ExSend(int socket, void *buf, unsigned short len) {
    Deci2TransferParam param;

    param.socket = socket;
    param.buf = buf;
    param.len = len;
    return Deci2Call(-6, &param);
}


int sceDeci2ExReqSend(int socket, char dest) {
    Deci2ReqSendParam param;

    param.socket = socket;
    param.dest = dest;
    return Deci2Call(-7, &param);
}


int sceDeci2ExLock(int socket) {
    Deci2SocketParam param;

    param.socket = socket;
    return Deci2Call(-8, &param);
}


int sceDeci2ExUnLock(int socket) {
    Deci2SocketParam param;

    param.socket = socket;
    return Deci2Call(-9, &param);
}


int kputs(const char *str) {
    Deci2PutsParam param;

    param.str = str;
    return Deci2Call(16, &param);
}
