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

void _sceSDC(unsigned int start, unsigned int end);
void _sceIDC(unsigned int start, unsigned int end);


void iSyncDCache(void *start, void *end)
{
    _sceSDC((unsigned int)start & ~0x3F, (unsigned int)end & ~0x3F);
}


void iInvalidDCache(void *start, void *end)
{
    _sceIDC((unsigned int)start & ~0x3F, (unsigned int)end & ~0x3F);
}
