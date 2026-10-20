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

extern Queue D_00703280;
extern TtyInfo D_00703390;
extern unsigned char D_007033C0[0x140];
extern unsigned char D_00703500[0x140];

#define tty_queue D_00703280
#define tty_info  D_00703390
#define tty_wbuf  D_007033C0
#define tty_rbuf  D_00703500

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
    Queue *q = &tty_queue;

    q->size = size;
    q->count = 0;
    q->rp = q->wp = q->buf;
    return q;
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
            if (ti->rlen + param > sizeof(tty_rbuf)) {
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

    if (tty_info.writing) {
        return -1;
    }
    DIntr();
    tty_info.writing = 1;
    pkt = UNCACHED(tty_wbuf);
    tty_info.wbuf = (unsigned char *)pkt;
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
    tty_info.wlen = n + sizeof(TtyHdr);
    pkt->hdr.len = tty_info.wlen;
    if (sceDeci2ReqSend(tty_info.socket, pkt->hdr.dest) < 0) {
        tty_info.writing = 0;
        EIntr();
        return -1;
    }
    while (tty_info.writing) {
        sceDeci2Poll(tty_info.socket);
    }
    EIntr();
    return written;
}


int sceTtyRead(char *buf, int len)
{
    int i;

    for (i = 0; i < len; i++) {
        while (tty_info.queue->count == 0) {
        }
        buf[i] = *tty_info.queue->rp;
        QueuePeekReadDone(tty_info.queue);
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
    tty_info.socket = sceDeci2Open(PROTO_ETTY, &tty_info, (Deci2Handler)sceTtyHandler);
    if (tty_info.socket < 0) {
        return 0;
    }
    tty_info.writing = 0;
    tty_info.wlen = 0;
    tty_info.rlen = 0;
    tty_info.rbuf = UNCACHED(tty_rbuf);
    hdr = UNCACHED(tty_wbuf);
    tty_info.wbuf = (unsigned char *)hdr;
    hdr->proto = PROTO_ETTY;
    hdr->src = 'E';
    hdr->dest = 'H';
    hdr->rsvd = 0;
    hdr->id = 0;
    tty_info.queue = QueueInit(TTY_BUFSZ);
    return 1;
}
