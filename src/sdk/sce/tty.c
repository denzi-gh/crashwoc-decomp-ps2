/*
 * Unit: sdk/sce/tty
 *
 * Functions:
 *   0x00279d88 QueueInit
 *   0x00279db0 QueuePeekWriteDone
 *   0x00279df0 QueuePeekReadDone
 *   0x00279e30 sceTtyHandler
 *   0x00279fc8 sceTtyWrite
 *   0x0027a118 sceTtyRead
 *   0x0027a1e8 sceTtyInit
 */


#define UNCACHED_SEGMENT 0x20000000
#define UNCACHED(p) ((void *)((unsigned int)(p) | UNCACHED_SEGMENT))

#define PROTO_ETTY 0x210

#define DECI2_READ      1
#define DECI2_READDONE  2
#define DECI2_WRITE     3
#define DECI2_WRITEDONE 4

#define TTY_BUFSZ 0x100

typedef struct {
    int size;
    volatile int count;
    unsigned char *rp;
    unsigned char *wp;
    unsigned char buf[TTY_BUFSZ];
} Queue;

typedef struct {
    unsigned short len;
    unsigned short rsvd;
    unsigned short proto;
    char src;
    char dest;
    int id;
} TtyHdr;

typedef struct {
    TtyHdr hdr;
    unsigned char data[TTY_BUFSZ];
} TtyPacket;

typedef struct {
    volatile int socket;
    volatile int wlen;
    volatile int rlen;
    volatile int writing;
    unsigned char *wbuf;
    unsigned char *rbuf;
    Queue *queue;
} TtyInfo;

typedef void (*Deci2Handler)(int event, int param, void *opt);

static TtyInfo tinfo;
static unsigned char wbuf[0x140] __attribute__((aligned(64)));
static unsigned char rbuf[0x140] __attribute__((aligned(64)));

extern int sceDeci2Open(unsigned short protocol, void *opt, Deci2Handler handler);
extern int sceDeci2ReqSend(int socket, char dest);
extern void sceDeci2Poll(int socket);
extern int sceDeci2ExRecv(int socket, void *buf, unsigned short len);
extern int sceDeci2ExSend(int socket, void *buf, unsigned short len);
extern int DIntr(void);
extern int EIntr(void);
extern void FlushCache(int mode);
extern int kprintf(const char *fmt, ...);


Queue *QueueInit(int size)
{
    static Queue q;

    q.size = size;
    q.count = 0;
    q.rp = q.wp = q.buf;
    return &q;
}


void QueuePeekWriteDone(Queue *q)
{
    q->count++;
    q->wp++;
    if (q->wp == q->buf + q->size) {
        q->wp = q->buf;
    }
}


void QueuePeekReadDone(Queue *q)
{
    q->count--;
    q->rp++;
    if (q->rp == q->buf + q->size) {
        q->rp = q->buf;
    }
}


void sceTtyHandler(int event, int param, TtyInfo *ti)
{
    TtyHdr *hdr;
    int n;
    int sent;

    switch (event) {
    case DECI2_READ:
    case DECI2_READDONE:
        if (param != 0) {
            if (ti->rlen + param > sizeof(rbuf)) {
                kprintf("TTY: packet size larger than expect\n");
            }
            n = sceDeci2ExRecv(ti->socket, ti->rbuf + ti->rlen, param);
            if (n < 0) {
                kprintf("TTY: receive error");
            }
            ti->rlen += n;
        } else {
            hdr = (TtyHdr *)ti->rbuf;
            for (n = sizeof(TtyHdr); n < hdr->len; n++) {
                *ti->queue->wp = ti->rbuf[n];
                QueuePeekWriteDone(ti->queue);
            }
            ti->rlen = 0;
        }
        break;
    case DECI2_WRITE:
        sent = sceDeci2ExSend(ti->socket, ti->wbuf, ti->wlen);
        if (sent < 0) {
            kprintf("TTY: send err %d\n", sent);
            ti->writing = 0;
        } else {
            ti->wbuf += sent;
            ti->wlen -= sent;
        }
        break;
    case DECI2_WRITEDONE:
        if (ti->wlen != 0) {
            kprintf("TTY: err ti->wlen=%08x\n", ti->wlen);
        }
        ti->writing = 0;
        break;
    }
}


int sceTtyWrite(const char *buf, int len)
{
    TtyPacket *pkt;
    unsigned char *p;
    int n = 0;
    int written = 0;

    if (tinfo.writing) {
        return -1;
    }
    DIntr();
    tinfo.writing = 1;
    pkt = UNCACHED(wbuf);
    tinfo.wbuf = (unsigned char *)pkt;
    p = pkt->data;
    while (len-- != 0) {
        if (*buf == '\n') {
            *p++ = '\r';
            if (++n >= TTY_BUFSZ) {
                break;
            }
        }
        *p++ = *buf++;
        n++;
        written++;
        if (n >= TTY_BUFSZ) {
            break;
        }
    }
    tinfo.wlen = n + sizeof(TtyHdr);
    pkt->hdr.len = tinfo.wlen;
    if (sceDeci2ReqSend(tinfo.socket, pkt->hdr.dest) < 0) {
        tinfo.writing = 0;
        EIntr();
        return -1;
    }
    while (tinfo.writing) {
        sceDeci2Poll(tinfo.socket);
    }
    EIntr();
    return written;
}


int sceTtyRead(char *buf, int len)
{
    int i;

    for (i = 0; i < len; i++) {
        while (tinfo.queue->count == 0) {
        }
        buf[i] = *tinfo.queue->rp;
        QueuePeekReadDone(tinfo.queue);
        if (buf[i] == '\n' || buf[i] == '\r') {
            return i + 1;
        }
    }
    return i;
}


int sceTtyInit(void)
{
    TtyHdr *hdr;

    FlushCache(0);
    tinfo.socket = sceDeci2Open(PROTO_ETTY, &tinfo, (Deci2Handler)sceTtyHandler);
    if (tinfo.socket < 0) {
        return 0;
    }
    tinfo.writing = 0;
    tinfo.wlen = 0;
    tinfo.rlen = 0;
    tinfo.rbuf = UNCACHED(rbuf);
    hdr = UNCACHED(wbuf);
    tinfo.wbuf = (unsigned char *)hdr;
    hdr->proto = PROTO_ETTY;
    hdr->src = 'E';
    hdr->dest = 'H';
    hdr->rsvd = 0;
    hdr->id = 0;
    tinfo.queue = QueueInit(TTY_BUFSZ);
    return 1;
}
