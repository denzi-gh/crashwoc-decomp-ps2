/*
 * Unit: sdk/sce/cache
 *
 * Functions:
 *   0x00279448 _sceSDC
 *   0x002794f0 SyncDCache
 *   0x00279570 iSyncDCache
 *   0x00279588 _sceIDC
 *   0x00279630 InvalidDCache
 *   0x002796b0 iInvalidDCache
 */

#define STATUS_EIE 0x10000
#define DCACHE_WAY_SIZE 0x1000
#define DCACHE_LINE_SIZE 0x40
#define TAG_PADDR_MASK 0xFFFFF000

#define SYNC() __asm__ volatile ("sync")

extern int DIntr(void);
extern int EIntr(void);


void _sceSDC(unsigned int start, unsigned int end)
{
    int line;
    unsigned int tag, addr;

    for (line = 0; line < DCACHE_WAY_SIZE; line += DCACHE_LINE_SIZE) {
        SYNC();
        __asm__ volatile ("cache 0x10, 0(%0)" : : "r"(line));
        SYNC();
        __asm__ volatile ("mfc0 %0, $28" : "=r"(tag));
        addr = (tag & TAG_PADDR_MASK) + line;
        if (addr >= start && addr <= end) {
            SYNC();
            __asm__ volatile ("cache 0x14, 0(%0)" : : "r"(line));
            SYNC();
        }
        SYNC();
        __asm__ volatile ("cache 0x10, 1(%0)" : : "r"(line));
        SYNC();
        __asm__ volatile ("mfc0 %0, $28" : "=r"(tag));
        addr = (tag & TAG_PADDR_MASK) + line;
        if (addr >= start && addr <= end) {
            SYNC();
            __asm__ volatile ("cache 0x14, 1(%0)" : : "r"(line));
            SYNC();
        }
        SYNC();
    }
}


void SyncDCache(void *start, void *end)
{
    int eie;

    __asm__ volatile ("mfc0 %0, $12" : "=r"(eie));
    eie &= STATUS_EIE;
    if (eie) {
        DIntr();
    }
    _sceSDC((unsigned int)start & ~0x3F, (unsigned int)end & ~0x3F);
    if (eie) {
        EIntr();
    }
}


void iSyncDCache(void *start, void *end)
{
    _sceSDC((unsigned int)start & ~0x3F, (unsigned int)end & ~0x3F);
}


void _sceIDC(unsigned int start, unsigned int end)
{
    int line;
    unsigned int tag, addr;

    for (line = 0; line < DCACHE_WAY_SIZE; line += DCACHE_LINE_SIZE) {
        SYNC();
        __asm__ volatile ("cache 0x10, 0(%0)" : : "r"(line));
        SYNC();
        __asm__ volatile ("mfc0 %0, $28" : "=r"(tag));
        addr = (tag & TAG_PADDR_MASK) + line;
        if (addr >= start && addr <= end) {
            SYNC();
            __asm__ volatile ("cache 0x16, 0(%0)" : : "r"(line));
            SYNC();
        }
        SYNC();
        __asm__ volatile ("cache 0x10, 1(%0)" : : "r"(line));
        SYNC();
        __asm__ volatile ("mfc0 %0, $28" : "=r"(tag));
        addr = (tag & TAG_PADDR_MASK) + line;
        if (addr >= start && addr <= end) {
            SYNC();
            __asm__ volatile ("cache 0x16, 1(%0)" : : "r"(line));
            SYNC();
        }
        SYNC();
    }
}


void InvalidDCache(void *start, void *end)
{
    int eie;

    __asm__ volatile ("mfc0 %0, $12" : "=r"(eie));
    eie &= STATUS_EIE;
    if (eie) {
        DIntr();
    }
    _sceIDC((unsigned int)start & ~0x3F, (unsigned int)end & ~0x3F);
    if (eie) {
        EIntr();
    }
}


void iInvalidDCache(void *start, void *end)
{
    _sceIDC((unsigned int)start & ~0x3F, (unsigned int)end & ~0x3F);
}
