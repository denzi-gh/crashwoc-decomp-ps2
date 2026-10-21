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

#include "nu_types.h"

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

static int _cmd_init_check = 0;

static unsigned char _pckt_buffer[128];
static unsigned char _send_buffer[64];
static SifCmdChangeAddrData _csdata;
static int sif0_handleid;
static SifCmdData _data_table;
static SifCmdHandlerData _sys_buffer[32] __attribute__((aligned(64)));
static unsigned int soft_reg[32];

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
#define SIF_CMD_INDEX_MASK   0x7FFFFFFF

extern int DIntr(void);
extern int EIntr(void);
extern void FlushCache(int mode);
extern void sceSifSetDChain(void);
extern int isceSifSetDChain(void);
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
    return soft_reg[index];
}


unsigned int sceSifSetSreg(int index, unsigned int value) {
    return soft_reg[index] = value;
}


SifCmdData *sceSifGetDataTable(void) {
    return &_data_table;
}


void sceSifInitCmd(void) {
    int i;

    DIntr();
    if (_cmd_init_check) {
        EIntr();
        return;
    }
    _cmd_init_check = 1;

    _data_table.pktbuf = UNCACHED(_pckt_buffer);
    _data_table.cbuf = UNCACHED(_send_buffer);
    _data_table.iopbuf = 0;
    _data_table.sys_cmd_handlers = _sys_buffer;
    _data_table.nr_sys_handlers = 32;
    _data_table.usr_cmd_handlers = 0;
    _data_table.nr_usr_handlers = 0;
    _data_table.sregs = soft_reg;

    for (i = 0; i < 32; i++) {
        _sys_buffer[i].func = 0;
        _sys_buffer[i].data = 0;
    }
    for (i = 0; i < 32; i++) {
        soft_reg[i] = 0;
    }

    _sys_buffer[0].func = (SifCmdHandler)_change_addr;
    _sys_buffer[0].data = &_data_table;
    _sys_buffer[1].func = (SifCmdHandler)_set_sreg;
    _sys_buffer[1].data = &_data_table;
    EIntr();

    FlushCache(0);
    if (DMAC_STAT & 0x20) {
        DMAC_STAT = 0x20;
    }
    if (!(DMAC_SIF0_CHCR & 0x100)) {
        sceSifSetDChain();
    }
    sif0_handleid = AddDmacHandler(5, _sceSifCmdIntrHdlr, 0);
    EnableDmac(5);

    _data_table.iopbuf = (void *)sceSifGetReg(SIF_SYSREG_SUBADDR);
    if (_data_table.iopbuf) {
        _csdata.newaddr = _pckt_buffer;
        sceSifSendCmd(SIF_CMD_CHANGE_SADDR, &_csdata,
                      sizeof(_csdata), 0, 0, 0);
        return;
    }

    while (!(sceSifGetReg(SIF_REG_SMFLAG) & SIF_STAT_CMDINIT)) {
    }
    _data_table.iopbuf = (void *)sceSifGetReg(SIF_REG_SUBADDR);
    sceSifSetReg(SIF_SYSREG_SUBADDR, (unsigned int)_data_table.iopbuf);
    sceSifSetReg(SIF_SYSREG_MAINADDR, (unsigned int)&_data_table);
    _csdata.newaddr = _pckt_buffer;
    _csdata.header.opt = 0;
    sceSifSendCmd(SIF_CMD_INIT_CMD, &_csdata,
                  sizeof(_csdata), 0, 0, 0);
}


void sceSifExitCmd(void) {
    DisableDmac(5);
    RemoveDmacHandler(5, sif0_handleid);
    _cmd_init_check = 0;
}


SifCmdHandlerData *sceSifSetCmdBuffer(SifCmdHandlerData *buffer, int count) {
    SifCmdHandlerData *old;

    old = _data_table.usr_cmd_handlers;
    _data_table.usr_cmd_handlers = buffer;
    _data_table.nr_usr_handlers = count;
    return old;
}


SifCmdHandlerData *sceSifSetSysCmdBuffer(SifCmdHandlerData *buffer, int count) {
    SifCmdHandlerData *old;

    old = _data_table.sys_cmd_handlers;
    _data_table.sys_cmd_handlers = buffer;
    _data_table.nr_sys_handlers = count;
    return old;
}


void sceSifAddCmdHandler(int cid, SifCmdHandler handler, void *data) {
    if (cid < 0) {
        _data_table.sys_cmd_handlers[cid].func = handler;
        _data_table.sys_cmd_handlers[cid].data = data;
    } else {
        _data_table.usr_cmd_handlers[cid].func = handler;
        _data_table.usr_cmd_handlers[cid].data = data;
    }
}


void sceSifRemoveCmdHandler(int cid) {
    if (cid < 0) {
        _data_table.sys_cmd_handlers[cid].func = 0;
    } else {
        _data_table.usr_cmd_handlers[cid].func = 0;
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
    dmat[count].addr = (unsigned int)_data_table.iopbuf;
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


int _sceSifCmdIntrHdlr(int channel) {
    u128 packet[7];
    SifCmdData *cmd = &_data_table;
    volatile SifCmdHeader *header;
    u128 *src;
    SifCmdHandler func;
    int n;
    int i;

    EIntr();
    header = cmd->pktbuf;
    n = header->psize;
    if (n == 0) {
        return 0;
    }
    n = (n + 15) / 16;
    header->psize = 0;
    src = (u128 *)header;
    for (i = 0; i < n; i++) {
        packet[i] = *src++;
    }
    isceSifSetDChain();
    header = (volatile SifCmdHeader *)packet;
    if (header->cid < 0) {
        n = header->cid & SIF_CMD_INDEX_MASK;
        if (n < cmd->nr_sys_handlers) {
            func = cmd->sys_cmd_handlers[n].func;
            if (func) {
                func(packet, cmd->sys_cmd_handlers[n].data);
            }
        }
    } else {
        n = header->cid;
        if (n < cmd->nr_usr_handlers) {
            func = cmd->usr_cmd_handlers[n].func;
            if (func) {
                func(packet, cmd->usr_cmd_handlers[n].data);
            }
        }
    }
    __asm__ volatile ("sync\n\tei");
    return 0;
}


void sceSifWriteBackDCache(void *ptr, int size) {
    __asm__ volatile (
        ".set noreorder\n"
        "    lui     $25, 0xffff\n"
        "    ori     $25, $25, 0xffc0\n"
        "    blez    %1, 3f\n"
        "    addu    $10, %0, %1\n"
        "    and     $8, %0, $25\n"
        "    addiu   $10, $10, -1\n"
        "    and     $9, $10, $25\n"
        "    subu    $10, $9, $8\n"
        "    srl     $11, $10, 6\n"
        "    addiu   $11, $11, 1\n"
        "    andi    $9, $11, 7\n"
        "    beqz    $9, 2f\n"
        "    srl     $10, $11, 3\n"
        "1:  sync\n"
        "    cache   0x18, 0($8)\n"
        "    sync\n"
        "    addiu   $9, $9, -1\n"
        "    nop\n"
        "    bgtz    $9, 1b\n"
        "    addiu   $8, $8, 64\n"
        "2:  beqz    $10, 3f\n"
        "4:  addiu   $10, $10, -1\n"
        "    sync\n"
        "    cache   0x18, 0($8)\n"
        "    sync\n"
        "    cache   0x18, 64($8)\n"
        "    sync\n"
        "    cache   0x18, 128($8)\n"
        "    sync\n"
        "    cache   0x18, 192($8)\n"
        "    sync\n"
        "    cache   0x18, 256($8)\n"
        "    sync\n"
        "    cache   0x18, 320($8)\n"
        "    sync\n"
        "    cache   0x18, 384($8)\n"
        "    sync\n"
        "    cache   0x18, 448($8)\n"
        "    sync\n"
        "    bgtz    $10, 4b\n"
        "    addiu   $8, $8, 512\n"
        "3:  jr      $31\n"
        "    nop"
        : : "r"(ptr), "r"(size) : "$8", "$9", "$10", "$11", "$25", "memory");
}
