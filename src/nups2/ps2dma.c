/*
 * Unit: nups2/ps2dma
 *
 * Functions:
 *   0x0016b9a0 GSRegDecode
 *   0x0016bd00 DirectDecode
 *   0x0016bfe8 VTDecode
 *   0x0016c8f0 TagWalk
 *   0x0016cc18 gsBeginGifPacket
 *   0x0016ccc0 gsCloseGifPacket
 *   0x0016cda0 NuScratchReset
 *   0x0016cdb0 NuScratchAlloc32
 *   0x0016cde8 NuScratchAlloc64
 *   0x0016ce30 NuScratchAlloc128
 *   0x0016ce78 NuScratchRelease
 *   0x0016ce88 vpDmaTag_Set
 *   0x0016ceb0 vpDmaTag_Cnt
 *   0x0016ced0 vpDmaTag_End
 *   0x0016cef0 vpDmaTag_EndEx
 *   0x0016cf18 vpDmaTag_Ref
 *   0x0016cf40 VU0_DmaWrite
 *   0x0016cfa8 VU0_DmaGetStatus
 *   0x0016cfc0 GS_DmaGetStatus
 *   0x0016d028 GS_DmaWrite
 *   0x0016d0e0 VU1_DmaWrite
 *   0x0016d198 VU1_DmaWriteN
 *   0x0016d218 VU1_DmaRead
 *   0x0016d298 VU1_DmaGetStatus
 *   0x0016d310 SPR_DmaWriteN
 *   0x0016d388 SPR_DmaReadN
 *   0x0016d400 SPR_DmaGetWriteStatus
 *   0x0016d418 SPR_DmaGetReadStatus
 *   0x0016d430 GS_DmaWriteN
 *   0x0016d498 GS_DmaGetWriteStatus
 *   0x0016d4b0 vtBeginDirect
 *   0x0016d4d0 vtCloseDirect
 *   0x0016d4f8 VifTagDummy
 *   0x0016d500 VifTagWalk
 *   0x0016d5e0 CrcChain
 *   0x0016d700 VU1_DmaGetStatusI
 */

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct DmaTag {
    u16 qwc;
    u8 pad;
    u8 id;
    u32 addr;
    u32 vifcode[2];
} __attribute__((aligned(16))) DmaTag;

#define VIF0_CHCR       (*(volatile int *)0x10008000)
#define VIF1_CHCR       (*(volatile int *)0x10009000)
#define GIF_CHCR        (*(volatile int *)0x1000A000)
#define SPR_FROM_CHCR   (*(volatile int *)0x1000D000)
#define SPR_TO_CHCR     (*(volatile int *)0x1000D400)

#define SCRATCHPAD_BASE 0x70000000

extern DmaTag D_002EA550;
extern DmaTag D_002EA570;
extern u32 *D_0062F0E0;
extern DmaTag *vpdmatag_curr;

#define dmatag_cnt      D_002EA550
#define dmatag_end      D_002EA570
#define nuscratch_ptr   D_0062F0E0


void NuScratchReset(void)
{
    nuscratch_ptr = (u32 *)SCRATCHPAD_BASE;
}


void NuScratchRelease(void)
{
    nuscratch_ptr = (u32 *)nuscratch_ptr[-1];
}


DmaTag *vpDmaTag_Cnt(DmaTag *tag)
{
    vpdmatag_curr = tag;
    *tag = dmatag_cnt;
    return tag + 1;
}


DmaTag *vpDmaTag_End(DmaTag *tag)
{
    vpdmatag_curr = tag;
    *tag = dmatag_end;
    return tag + 1;
}


int VU0_DmaGetStatus(void)
{
    return VIF0_CHCR & 0x100;
}


int SPR_DmaGetWriteStatus(void)
{
    return (SPR_TO_CHCR >> 8) & 1;
}


int SPR_DmaGetReadStatus(void)
{
    return (SPR_FROM_CHCR >> 8) & 1;
}


int GS_DmaGetWriteStatus(void)
{
    return (GIF_CHCR >> 8) & 1;
}


DmaTag *VifTagDummy(DmaTag *tag, void *user)
{
    return tag;
}


int VU1_DmaGetStatusI(void)
{
    return VIF1_CHCR & 0x100;
}
