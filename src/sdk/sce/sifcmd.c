/*
 * Unit: sdk/sce/sifcmd
 *
 * Functions:
 *   0x0027ac20 _set_sreg
 *   0x0027ac40 _change_addr
 *   0x0027ac50 sceSifGetSreg
 *   0x0027ac68 sceSifSetSreg
 *   0x0027ac88 sceSifGetDataTable
 *   0x0027ac98 sceSifInitCmd
 *   0x0027af18 sceSifExitCmd
 *   0x0027af50 sceSifSetCmdBuffer
 *   0x0027af68 sceSifSetSysCmdBuffer
 *   0x0027af80 sceSifAddCmdHandler
 *   0x0027afb0 sceSifRemoveCmdHandler
 *   0x0027afd8 _sceSifSendCmd
 *   0x0027b110 sceSifSendCmd
 *   0x0027b150 isceSifSendCmd
 *   0x0027b190 _sceSifCmdIntrHdlr
 *   0x0027b2b8 sceSifWriteBackDCache
 */

typedef struct SifCmdHeader {
    unsigned int psize : 8;
    unsigned int dsize : 24;
    unsigned int dest;
    int cid;
    unsigned int opt;
} SifCmdHeader;

typedef struct SifCmdSRegData {
    SifCmdHeader header;
    int index;
    unsigned int value;
} SifCmdSRegData;

typedef struct SifCmdChangeAddrData {
    SifCmdHeader header;
    void *newaddr;
} SifCmdChangeAddrData;

typedef struct sceSifDmaData {
    unsigned int data;
    unsigned int addr;
    unsigned int size;
    unsigned int mode;
} sceSifDmaData;

typedef void (*SifCmdHandler)(void *packet, void *data);

typedef struct SifCmdHandlerData {
    SifCmdHandler func;
    void *data;
} SifCmdHandlerData;

typedef struct SifCmdData {
    void *pktbuf;
    void *cbuf;
    void *iopbuf;
    SifCmdHandlerData *sys_cmd_handlers;
    int nr_sys_handlers;
    SifCmdHandlerData *usr_cmd_handlers;
    int nr_usr_handlers;
    unsigned int *sregs;
} SifCmdData;

extern int D_00613374[];
extern unsigned char D_007036C0[];
extern unsigned char D_00703740[];
extern SifCmdChangeAddrData D_00703780;
extern int D_00703794[];
extern SifCmdData D_00703798;
extern SifCmdHandlerData D_007037C0[];
extern unsigned int D_007038C0[];

#define sif_cmd_initialized   D_00613374
#define sif_pktbuf            D_007036C0
#define sif_cbuf              D_00703740
#define sif_change_addr_pkt   D_00703780
#define sif_dmac_handler_id   D_00703794
#define sif_cmd_data          D_00703798
#define sif_sys_cmd_handlers  D_007037C0
#define sif_sregs             D_007038C0

#define UNCACHED(p) ((void *)((unsigned int)(p) | 0x20000000))

#define DMAC_SIF0_CHCR (*(volatile unsigned int *)0x1000C000)
#define DMAC_STAT      (*(volatile unsigned int *)0x1000E010)

#define SIF_CMD_CHANGE_SADDR 0x80000000
#define SIF_CMD_INIT_CMD     0x80000002
#define SIF_SYSREG_SUBADDR   0x80000000
#define SIF_SYSREG_MAINADDR  0x80000001
#define SIF_REG_SUBADDR      2
#define SIF_REG_SMFLAG       4
#define SIF_STAT_CMDINIT     0x20000

extern int DIntr(void);
extern int EIntr(void);
extern void FlushCache(int mode);
extern void sceSifSetDChain(void);
extern int AddDmacHandler(int channel, int (*handler)(int), int next);
extern int EnableDmac(int channel);
extern int DisableDmac(int channel);
extern int RemoveDmacHandler(int channel, int handler_id);
extern void sceSifWriteBackDCache(void *ptr, int size);
extern unsigned int sceSifSetDma(sceSifDmaData *sdd, int len);
extern unsigned int isceSifSetDma(sceSifDmaData *sdd, int len);
extern unsigned int sceSifGetReg(unsigned int reg);
extern int sceSifSetReg(unsigned int reg, unsigned int value);
extern int _sceSifCmdIntrHdlr(int channel);
extern unsigned int sceSifSendCmd(int cid, void *packet, int packet_size,
                                  void *src, void *dest, int size);


void _set_sreg(SifCmdSRegData *packet, SifCmdData *cmd) {
    cmd->sregs[packet->index] = packet->value;
}


void _change_addr(SifCmdChangeAddrData *packet, SifCmdData *cmd) {
    cmd->iopbuf = packet->newaddr;
}


unsigned int sceSifGetSreg(int index) {
    return sif_sregs[index];
}


unsigned int sceSifSetSreg(int index, unsigned int value) {
    return sif_sregs[index] = value;
}


SifCmdData *sceSifGetDataTable(void) {
    return &sif_cmd_data;
}


void sceSifInitCmd(void) {
    int i;

    DIntr();
    if (sif_cmd_initialized[0]) {
        EIntr();
        return;
    }
    sif_cmd_initialized[0] = 1;

    sif_cmd_data.pktbuf = UNCACHED(sif_pktbuf);
    sif_cmd_data.cbuf = UNCACHED(sif_cbuf);
    sif_cmd_data.iopbuf = 0;
    sif_cmd_data.sys_cmd_handlers = sif_sys_cmd_handlers;
    sif_cmd_data.nr_sys_handlers = 32;
    sif_cmd_data.usr_cmd_handlers = 0;
    sif_cmd_data.nr_usr_handlers = 0;
    sif_cmd_data.sregs = sif_sregs;

    for (i = 0; i < 32; i++) {
        sif_sys_cmd_handlers[i].func = 0;
        sif_sys_cmd_handlers[i].data = 0;
    }
    for (i = 0; i < 32; i++) {
        sif_sregs[i] = 0;
    }

    sif_sys_cmd_handlers[0].func = (SifCmdHandler)_change_addr;
    sif_sys_cmd_handlers[0].data = &sif_cmd_data;
    sif_sys_cmd_handlers[1].func = (SifCmdHandler)_set_sreg;
    sif_sys_cmd_handlers[1].data = &sif_cmd_data;
    EIntr();

    FlushCache(0);
    if (DMAC_STAT & 0x20) {
        DMAC_STAT = 0x20;
    }
    if (!(DMAC_SIF0_CHCR & 0x100)) {
        sceSifSetDChain();
    }
    sif_dmac_handler_id[0] = AddDmacHandler(5, _sceSifCmdIntrHdlr, 0);
    EnableDmac(5);

    sif_cmd_data.iopbuf = (void *)sceSifGetReg(SIF_SYSREG_SUBADDR);
    if (sif_cmd_data.iopbuf) {
        sif_change_addr_pkt.newaddr = sif_pktbuf;
        sceSifSendCmd(SIF_CMD_CHANGE_SADDR, &sif_change_addr_pkt,
                      sizeof(sif_change_addr_pkt), 0, 0, 0);
        return;
    }

    while (!(sceSifGetReg(SIF_REG_SMFLAG) & SIF_STAT_CMDINIT)) {
    }
    sif_cmd_data.iopbuf = (void *)sceSifGetReg(SIF_REG_SUBADDR);
    sceSifSetReg(SIF_SYSREG_SUBADDR, (unsigned int)sif_cmd_data.iopbuf);
    sceSifSetReg(SIF_SYSREG_MAINADDR, (unsigned int)&sif_cmd_data);
    sif_change_addr_pkt.newaddr = sif_pktbuf;
    sif_change_addr_pkt.header.opt = 0;
    sceSifSendCmd(SIF_CMD_INIT_CMD, &sif_change_addr_pkt,
                  sizeof(sif_change_addr_pkt), 0, 0, 0);
}


void sceSifExitCmd(void) {
    DisableDmac(5);
    RemoveDmacHandler(5, sif_dmac_handler_id[0]);
    sif_cmd_initialized[0] = 0;
}


SifCmdHandlerData *sceSifSetCmdBuffer(SifCmdHandlerData *buffer, int count) {
    SifCmdHandlerData *old;

    old = sif_cmd_data.usr_cmd_handlers;
    sif_cmd_data.usr_cmd_handlers = buffer;
    sif_cmd_data.nr_usr_handlers = count;
    return old;
}


SifCmdHandlerData *sceSifSetSysCmdBuffer(SifCmdHandlerData *buffer, int count) {
    SifCmdHandlerData *old;

    old = sif_cmd_data.sys_cmd_handlers;
    sif_cmd_data.sys_cmd_handlers = buffer;
    sif_cmd_data.nr_sys_handlers = count;
    return old;
}


void sceSifAddCmdHandler(int cid, SifCmdHandler handler, void *data) {
    if (cid < 0) {
        sif_cmd_data.sys_cmd_handlers[cid].func = handler;
        sif_cmd_data.sys_cmd_handlers[cid].data = data;
    } else {
        sif_cmd_data.usr_cmd_handlers[cid].func = handler;
        sif_cmd_data.usr_cmd_handlers[cid].data = data;
    }
}


void sceSifRemoveCmdHandler(int cid) {
    if (cid < 0) {
        sif_cmd_data.sys_cmd_handlers[cid].func = 0;
    } else {
        sif_cmd_data.usr_cmd_handlers[cid].func = 0;
    }
}


unsigned int _sceSifSendCmd(int cid, int mode, void *packet, int packet_size,
                            void *src, void *dest, int size) {
    sceSifDmaData dmat[2];
    SifCmdHeader *header;
    int count;

    header = packet;
    if ((unsigned int)(packet_size - 16) > 96) {
        return 0;
    }

    count = 0;
    if (size > 0) {
        dmat[0].data = (unsigned int)src;
        dmat[0].addr = (unsigned int)dest;
        dmat[0].size = size;
        dmat[0].mode = 0;
        header->dsize = size;
        header->dest = (unsigned int)dest;
        count = 1;
        if (mode & 4) {
            sceSifWriteBackDCache(src, size);
        }
    } else {
        header->dsize = 0;
        header->dest = 0;
    }

    dmat[count].data = (unsigned int)packet;
    dmat[count].addr = (unsigned int)sif_cmd_data.iopbuf;
    dmat[count].size = packet_size;
    header->cid = cid;
    header->psize = packet_size;
    dmat[count].mode = 0x44;
    count++;
    sceSifWriteBackDCache(packet, packet_size);

    if (mode & 1) {
        return isceSifSetDma(dmat, count);
    }
    return sceSifSetDma(dmat, count);
}


unsigned int sceSifSendCmd(int cid, void *packet, int packet_size, void *src,
                           void *dest, int size) {
    return _sceSifSendCmd(cid, 0, packet, packet_size, src, dest, size);
}


unsigned int isceSifSendCmd(int cid, void *packet, int packet_size, void *src,
                            void *dest, int size) {
    return _sceSifSendCmd(cid, 1, packet, packet_size, src, dest, size);
}
