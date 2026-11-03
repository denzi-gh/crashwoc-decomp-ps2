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

#include "nu_libc.h"
#include "nu_stdarg.h"
#include "nu_types.h"

#include "nucore/nuerror.h"
#include "nups2/nups2types.h"
#include "nups2/ps2dma.h"

typedef struct DmaTag *(*TagWalkFunc)(struct DmaTag *tag, void *user);

#define VIF0_CHCR       (*(volatile int *)0x10008000)
#define VIF0_QWC        (*(volatile int *)0x10008020)
#define VIF0_TADR       (*(volatile int *)0x10008030)
#define VIF1_CHCR       (*(volatile int *)0x10009000)
#define VIF1_QWC        (*(volatile int *)0x10009020)
#define VIF1_TADR       (*(volatile int *)0x10009030)
#define GIF_CHCR        (*(volatile int *)0x1000A000)
#define GIF_MADR        (*(volatile int *)0x1000A010)
#define GIF_QWC         (*(volatile int *)0x1000A020)
#define GIF_TADR        (*(volatile int *)0x1000A030)
#define SPR_FROM_CHCR   (*(volatile int *)0x1000D000)
#define SPR_FROM_MADR   (*(volatile int *)0x1000D010)
#define SPR_FROM_QWC    (*(volatile int *)0x1000D020)
#define SPR_FROM_SADR   (*(volatile int *)0x1000D080)
#define SPR_TO_CHCR     (*(volatile int *)0x1000D400)
#define SPR_TO_MADR     (*(volatile int *)0x1000D410)
#define SPR_TO_QWC      (*(volatile int *)0x1000D420)
#define SPR_TO_SADR     (*(volatile int *)0x1000D480)
#define DMAC_CTRL       (*(volatile int *)0x1000E000)
#define DMAC_STAT       (*(volatile int *)0x1000E010)

#define DMA_ADDR_MASK   0x0FFFFFFF
#define DMA_STR         0x100

#define DMAC_CH_VIF0    0x001
#define DMAC_CH_VIF1    0x002
#define DMAC_CH_GIF     0x004
#define DMAC_CH_FROMSPR 0x100
#define DMAC_CH_TOSPR   0x200

#define SCRATCHPAD_BASE 0x70000000

#define GS_PRIM         0x00
#define GS_RGBAQ        0x01
#define GS_ST           0x02
#define GS_UV           0x03
#define GS_XYZF2        0x04
#define GS_XYZ2         0x05
#define GS_FOG          0x0A
#define GS_XYZF3        0x0C
#define GS_XYZ3         0x0D
#define GS_XYOFFSET_1   0x18
#define GS_XYOFFSET_2   0x19
#define GS_PRMODECONT   0x1A
#define GS_PRMODE       0x1B
#define GS_TEX0_1       0x06
#define GS_TEX0_2       0x07
#define GS_TEX1_1       0x14
#define GS_TEX1_2       0x15
#define GS_TEX2_1       0x16
#define GS_TEX2_2       0x17
#define GS_TEXCLUT      0x1C
#define GS_SCANMSK      0x22
#define GS_MIPTBP1_1    0x34
#define GS_MIPTBP1_2    0x35
#define GS_MIPTBP2_1    0x36
#define GS_MIPTBP2_2    0x37
#define GS_CLAMP_1      0x08
#define GS_CLAMP_2      0x09
#define GS_TEXA         0x3B
#define GS_FOGCOL       0x3D
#define GS_TEXFLUSH     0x3F
#define GS_SCISSOR_1    0x40
#define GS_SCISSOR_2    0x41
#define GS_ALPHA_1      0x42
#define GS_ALPHA_2      0x43
#define GS_DIMX         0x44
#define GS_DTHE         0x45
#define GS_COLCLAMP     0x46
#define GS_TEST_1       0x47
#define GS_TEST_2       0x48
#define GS_PABE         0x49
#define GS_FBA_1        0x4A
#define GS_FBA_2        0x4B
#define GS_FRAME_1      0x4C
#define GS_FRAME_2      0x4D
#define GS_ZBUF_1       0x4E
#define GS_ZBUF_2       0x4F
#define GS_BITBLTBUF    0x50
#define GS_TRXPOS       0x51
#define GS_TRXREG       0x52
#define GS_TRXDIR       0x53
#define GS_HWREG        0x54
#define GS_SIGNAL       0x60
#define GS_FINISH       0x61
#define GS_LABEL        0x62
#define GS_NOP          0x7F

static int debug_gs_list = 0;
static int debug_dma_list = 0;
static u32 *ps2_scratch_free = (u32 *)SCRATCHPAD_BASE;
int debug_dma_level = 0;
static int decode_error = 0;

static u32 *vt_curr;
static struct GifTag *gt_curr __attribute__((aligned(8)));

extern void NuStrCat(char *dst, char *src);
extern void TagWalk(struct DmaTag *tag, TagWalkFunc walker);
extern struct DmaTag *VifTagWalk(struct DmaTag *tag, void *user);
int VU0_DmaGetStatus(void);
int VU1_DmaGetStatus(void);



void GSRegDecode(char *indent, u32 reg, u32 *data, int packed)
{
    char *name;

    switch (reg) {
    case GS_PRIM:
        name = "PRIM      ";
        break;
    case GS_RGBAQ:
        name = "RGBAQ     ";
        break;
    case GS_ST:
        name = "ST        ";
        break;
    case GS_UV:
        name = "UV        ";
        break;
    case GS_XYZF2:
        name = "XYZF2     ";
        break;
    case GS_XYZ2:
        name = "XYZ2      ";
        break;
    case GS_FOG:
        name = "FOG       ";
        break;
    case GS_XYZF3:
        name = "XYZF3     ";
        break;
    case GS_XYZ3:
        name = "XYZ3      ";
        break;
    case GS_XYOFFSET_1:
        name = "XYOFFSET_1";
        break;
    case GS_XYOFFSET_2:
        name = "XYOFFSET_2";
        break;
    case GS_PRMODECONT:
        name = "PRMODECONT";
        break;
    case GS_PRMODE:
        name = "PRMODE    ";
        break;
    case GS_TEX0_1:
        name = "TEX0_1    ";
        break;
    case GS_TEX0_2:
        name = "TEX0_2    ";
        break;
    case GS_TEX1_1:
        name = "TEX1_1    ";
        break;
    case GS_TEX1_2:
        name = "TEX1_2    ";
        break;
    case GS_TEX2_1:
        name = "TEX2_1    ";
        break;
    case GS_TEX2_2:
        name = "TEX2_2    ";
        break;
    case GS_TEXCLUT:
        name = "TEXCLUT   ";
        break;
    case GS_SCANMSK:
        name = "SCANMSK   ";
        break;
    case GS_MIPTBP1_1:
        name = "MIPTBP1_1 ";
        break;
    case GS_MIPTBP1_2:
        name = "MIPTBP1_2 ";
        break;
    case GS_MIPTBP2_1:
        name = "MIPTBP2_1 ";
        break;
    case GS_MIPTBP2_2:
        name = "MIPTBP2_2 ";
        break;
    case GS_CLAMP_1:
        name = "CLAMP_1   ";
        break;
    case GS_CLAMP_2:
        name = "CLAMP_2   ";
        break;
    case GS_TEXA:
        name = "TEXA      ";
        break;
    case GS_FOGCOL:
        name = "FOGCOL    ";
        break;
    case GS_TEXFLUSH:
        name = "TEXFLUSH  ";
        break;
    case GS_SCISSOR_1:
        name = "SCISSOR_1 ";
        break;
    case GS_SCISSOR_2:
        name = "SCISSOR_2 ";
        break;
    case GS_ALPHA_1:
        name = "ALPHA_1   ";
        break;
    case GS_ALPHA_2:
        name = "ALPHA_2   ";
        break;
    case GS_DIMX:
        name = "DIMX      ";
        break;
    case GS_DTHE:
        name = "DTHE      ";
        break;
    case GS_COLCLAMP:
        name = "COLCLAMP  ";
        break;
    case GS_TEST_1:
        name = "TEST_1    ";
        break;
    case GS_TEST_2:
        name = "TEST_2    ";
        break;
    case GS_PABE:
        name = "PABE      ";
        break;
    case GS_FBA_1:
        name = "FBA_1     ";
        break;
    case GS_FBA_2:
        name = "FBA_2     ";
        break;
    case GS_FRAME_1:
        name = "FRAME_1   ";
        break;
    case GS_FRAME_2:
        name = "FRAME_2   ";
        break;
    case GS_ZBUF_1:
        name = "ZBUF_1    ";
        break;
    case GS_ZBUF_2:
        name = "ZBUF_2    ";
        break;
    case GS_BITBLTBUF:
        name = "BITBLTBUF ";
        break;
    case GS_TRXPOS:
        name = "TRXPOS    ";
        break;
    case GS_TRXREG:
        name = "TRXREG    ";
        break;
    case GS_TRXDIR:
        name = "TRXDIR    ";
        break;
    case GS_HWREG:
        name = "HWREG     ";
        break;
    case GS_SIGNAL:
        name = "SIGNAL    ";
        break;
    case GS_FINISH:
        name = "FINISH    ";
        break;
    case GS_LABEL:
        name = "LABEL     ";
        break;
    case GS_NOP:
        name = "NOP       ";
        break;
    default:
        name = "**UNKNOWN**";
        break;
    }
    if (packed) {
        NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x107)("%s[p]GS %s %08x %08x %08x %08x", indent, name,
                                             data[3], data[2], data[1], data[0]);
    } else {
        NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x10A)("%s[r]GS %s %08x %08x", indent, name,
                                             data[1], data[0]);
    }
}


u64 *DirectDecode(u64 *packet, char *prefix, int qwc)
{
    char indent[256] = "    ";
    char prim[256];
    struct GifTag *gif;
    u64 regs;
    char *eop;
    int nloop;
    int nreg;
    int count;
    int size;
    int reg;
    int i;

    NuStrCat(indent, prefix);
    while (qwc != 0) {
        gif = (struct GifTag *)packet;
        switch ((int)gif->flg) {
        case 0:
            nloop = gif->nloop;
            nreg = gif->nreg;
            size = nloop * nreg + 1;
            regs = packet[1];
            count = nloop;
            packet += 2;
            if (gif->pre) {
                sprintf(prim, ",PRIM %08x", (int)gif->prim);
            } else {
                prim[0] = 0;
            }
            if (gif->eop) {
                eop = ",EOP";
            } else {
                eop = "";
            }
            NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x12E)("%sGiftag(PACKED,QWC=%d%s%s)", indent, size - 1, prim, eop);
            while (count != 0) {
                count--;
                for (i = 0; i < nreg; i++) {
                    reg = (regs & (0xF << (i * 4))) >> (i * 4);
                    if (reg == 0xE) {
                        GSRegDecode(indent, ((u32 *)packet)[2], (u32 *)packet, 0);
                    } else {
                        GSRegDecode(indent, reg, (u32 *)packet, 1);
                    }
                    packet += 2;
                }
            }
            break;
        case 1:
            size = gif->nloop * gif->nreg + 1;
            count = size - 1;
            packet += 2;
            NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x146)("%sGiftag(REGLIST,QWC=%d)", prefix, count);
            while (count != 0) {
                GSRegDecode(indent, ((u32 *)packet)[2], (u32 *)packet, 0);
                count--;
                packet += 2;
            }
            break;
        default:
            size = gif->nloop + 1;
            NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x152)("%sGiftag(IMAGE,QWC=%d)", indent, size - 1);
            packet += size * 2;
            break;
        }
        qwc -= size;
    }
    return packet;
}

u32 *VTDecode(u32 *code, char *indent)
{
    char name[32];
    u32 *start;
    u32 vifcode;
    u32 i;
    int num;

    start = code;
    vifcode = *code++;
    switch ((vifcode >> 24) & 0x7F) {
    case 0x00:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x16C)("%s%08d VT NOP", indent, start);
        }
        break;
    case 0x01:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x170)("%s%08d VT STCYCL(%d,%d)", indent, start,
                (vifcode >> 8) & 0xFF, vifcode & 0xFF);
        }
        break;
    case 0x02:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x174)("%s%08d VT OFFSET", indent, start);
        }
        break;
    case 0x03:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x178)("%s%08d VT BASE", indent, start);
        }
        break;
    case 0x04:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x17C)("%s%08d VT ITOP", indent, start);
        }
        break;
    case 0x05:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x180)("%s%08d VT STMOD", indent, start);
        }
        break;
    case 0x06:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x184)("%s%08d VT MSKPATH3", indent, start);
        }
        break;
    case 0x07:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x188)("%s%08d VT MARK", indent, start);
        }
        break;
    case 0x10:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x18C)("%s%08d VT FLUSHE", indent, start);
        }
        break;
    case 0x11:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x190)("%s%08d VT FLUSH", indent, start);
        }
        break;
    case 0x13:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x194)("%s%08d VT FLUSHA", indent, start);
        }
        break;
    case 0x14:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x198)("%s%08d VT MSCAL(%08x)", indent, start,
                vifcode & 0xFFFF);
        }
        break;
    case 0x17:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x19C)("%s%08d VT MSCNT", indent, start);
        }
        break;
    case 0x15:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x1A0)("%s%08d VT MSCALF(%08x)", indent, start,
                vifcode & 0xFFFF);
        }
        break;
    case 0x20:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x1A4)("%s%08d VT STMASK %08x", indent, start, code[0]);
        }
        code += 1;
        break;
    case 0x30:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x1A9)("%s%08d VT STROW %08x %08x %08x %08x", indent, start,
                code[0], code[1], code[2], code[3]);
        }
        code += 4;
        break;
    case 0x31:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x1AE)("%s%08d VT STCOL %08d %08d %08d %08d", indent, start,
                code[0], code[1], code[2], code[3]);
        }
        code += 4;
        break;
    case 0x4A:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x1B3)("%s%08d VT MPG(%d,%d)", indent, start,
                vifcode & 0xFFFF, (vifcode >> 16) & 0xFF);
        }
        if ((vifcode & 0xFF0000) == 0) {
            code += 512;
        } else {
            code += ((vifcode & 0xFF0000) >> 16) * 2;
        }
        break;
    case 0x50:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x1BB)("%s%08d VT DIRECT %d", indent, start, (vifcode & 0xFFFF));
        }
        if (debug_dma_level == 5) {
            for (i = 0; i < ((vifcode & 0xFFFF) > 10 ? 10 : (vifcode & 0xFFFF)); i++) {
                NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x1BE)("%s  %d - %08x %08x %08x %08x", indent, i,
                    code[i * 4 + 3], code[i * 4 + 2], code[i * 4 + 1], code[i * 4]);
            }
        }
        if (debug_dma_level >= 7) {
            DirectDecode((u64 *)code, indent, (vifcode & 0xFFFF));
        }
        goto direct_advance;
    case 0x51:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x1C8)("%s%08d VT DIRECT HL %d", indent, start, (vifcode & 0xFFFF));
        }
direct_advance:
        code += (vifcode & 0xFFFF) * 4;
        break;
    default:
        if ((vifcode & 0x60000000) == 0x60000000) {
            strcpy(name, "VT UNPACK");
            if (vifcode & 0x8000) {
                strcat(name, "R");
            }
            if (vifcode & 0x4000) {
                strcat(name, "(U)");
            }
            num = (vifcode >> 16) & 0xFF;
            if (num == 0) {
                num = 256;
            }
            switch ((vifcode >> 24) & 0xF) {
            case 0:
                if (debug_dma_level > 0) {
                    NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x1DD)("%s%08d %s S-32 %d to %d", indent, start, name,
                        num, vifcode & 0x3FF);
                }
                code = (u32 *)((u8 *)code + num * 4);
                break;
            case 1:
                if (debug_dma_level > 0) {
                    NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x1E2)("%s%08d %s S-16 %d to %d", indent, start, name,
                        num, vifcode & 0x3FF);
                }
                code = (u32 *)((u8 *)code + num * 2);
                break;
            case 2:
                if (debug_dma_level > 0) {
                    NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x1E7)("%s%08d %s S-8 %d to %d", indent, start, name,
                        num, vifcode & 0x3FF);
                }
                code = (u32 *)((u8 *)code + num);
                break;
            case 4:
                if (debug_dma_level > 0) {
                    NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x1EC)("%s%08d %s V2-32 %d to %d", indent, start, name,
                        num, vifcode & 0x3FF);
                }
                code = (u32 *)((u8 *)code + num * 8);
                break;
            case 5:
                if (debug_dma_level > 0) {
                    NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x1F1)("%s%08d %s V2-16 %d to %d", indent, start, name,
                        num, vifcode & 0x3FF);
                }
                code = (u32 *)((u8 *)code + num * 4);
                break;
            case 6:
                if (debug_dma_level > 0) {
                    NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x1F6)("%s%08d %s V2-8 %d to %d", indent, start, name,
                        num, vifcode & 0x3FF);
                }
                code = (u32 *)((u8 *)code + num * 2);
                break;
            case 8:
                if (debug_dma_level > 0) {
                    NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x1FB)("%s%08d %s V3-32 %d to %d", indent, start, name,
                        num, vifcode & 0x3FF);
                }
                code = (u32 *)((u8 *)code + num * 12);
                break;
            case 9:
                if (debug_dma_level > 0) {
                    NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x200)("%s%08d %s V3-16 %d to %d", indent, start, name,
                        num, vifcode & 0x3FF);
                }
                code = (u32 *)((u8 *)code + num * 6);
                break;
            case 10:
                if (debug_dma_level > 0) {
                    NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x205)("%s%08d %s V3-8 %d to %d", indent, start, name,
                        num, vifcode & 0x3FF);
                }
                code = (u32 *)((u8 *)code + num * 3);
                break;
            case 12:
                if (debug_dma_level > 0) {
                    NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x20A)("%s%08d %s V4-32 %d to %d", indent, start, name,
                        num, vifcode & 0x3FF);
                }
                code = (u32 *)((u8 *)code + num * 16);
                break;
            case 13:
                if (debug_dma_level > 0) {
                    NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x20F)("%s%08d %s V4-16 %d to %d", indent, start, name,
                        num, vifcode & 0x3FF);
                }
                code = (u32 *)((u8 *)code + num * 8);
                break;
            case 14:
                if (debug_dma_level > 0) {
                    NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x214)("%s%08d %s V4-8 %d to %d", indent, start, name,
                        num, vifcode & 0x3FF);
                }
                code = (u32 *)((u8 *)code + num * 4);
                break;
            case 15:
                if (debug_dma_level > 0) {
                    NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x219)("%s%08d %s V4-5 %d to %d", indent, start, name,
                        num, vifcode & 0x3FF);
                }
                code = (u32 *)((u8 *)code + num * 8);
                break;
            }
            if ((u32)code & 3) {
                code = (u32 *)((u32)code + (4 - ((u32)code & 3)));
            }
        } else {
            NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x222)("%s%08d Unknown Viftag (%08d)- stopping decode", indent, start, vifcode);
            for (;;) {
            }
        }
        break;
    }
    return code;
}


inline struct DmaTag *VifTagWalk(struct DmaTag *tag, void *user)
{
    u32 *code;
    u32 *end;

    switch (tag->id >> 4) {
    case 0:
    case 3:
    case 4:
        VTDecode(&tag->vifcode[0], user);
        VTDecode(&tag->vifcode[1], user);
        return tag + 1;
    }
    code = tag->vifcode;
    end = (u32 *)(tag->qwc * 16 + (u32)code + 8);
    while (code < end) {
        code = VTDecode(code, user);
        if (decode_error) {
            return (struct DmaTag *)code;
        }
    }
    if (code != end) {
        NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x251)("VIF code decoded length does not match dmatag length");
        for (;;) {
        }
    }
    return (struct DmaTag *)code;
}


static char *tagnames[8] = {
    "REFE", "CNT ", "NEXT", "REF ", "REFS", "CALL", "RET ", "END "
};

static char *marknames[25] = {
    "unknown", "MTL", "TXTRSET", "TXTRLOAD", "GEOMCALL", "GOBJDATA",
    "SKINDATA", "SKINHDR", "SKINMTX", "BEGINSCENE FB", "BEGINSCENE FBT",
    "BEGINSCENE ZB", "BEGINSCENE ZBS", "STREAMFLUSHLNK", "TEXTPRINT",
    "CLEARSCREEN", "GRADCLEARSCREEN", "CLIPTRI3D", "CLIPQUAD3D",
    "CLIPTRISTRIP3D", "MTXLOAD", "LIGHTLOAD", "FXMTXLOAD", "SPECLOAD", "FREE"
};

static int mark_suppressed[25] = {
    0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static int find_mark = -1;

int gs_dma_status = 0;
int vu1_dma_status = 0;
int crc_check = 0;
struct DmaTag *gs_termaddr = 0;
int wait_until_finished = 0;
int cycle_steal = 0;
struct DmaTag *termaddr = 0;
struct DmaTag *vpdmatag_curr __attribute__((aligned(8))) = 0;
struct DmaTag *gs_last_write __attribute__((aligned(8))) = 0;
struct DmaTag *gs_prev_write __attribute__((aligned(8))) = 0;
struct DmaTag *vu1_last_write __attribute__((aligned(8))) = 0;
struct DmaTag *vu1_prev_write __attribute__((aligned(8))) = 0;
u64 old_crc = 0;


void TagWalk(struct DmaTag *tag, TagWalkFunc walker)
{
    struct DmaTag *history[32];
    char indent[256];
    struct DmaTag *ret_stack[2];
    int quiet_stack[2];
    int history_pos;
    int indent_len;
    int depth;
    u32 mark;
    int quiet;
    char *quiet_str;

    history_pos = 0;
    indent_len = 0;
    depth = 0;
    if (tag == 0) {
        NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x2B4)("Duff frame pointer");
    }
    do {
        if (indent_len) {
            memset(indent, ' ', indent_len);
        }
        indent[indent_len] = 0;
        if ((u32)tag & 0xF) {
            NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x2BB)("** Misaligned DMATAG **");
            for (;;) {
            }
        }
        history[history_pos] = tag;
        history_pos = (history_pos + 1) & 0x1F;
        mark = tag->pad;
        if (mark >= 26) {
            mark = 0;
        }
        quiet = mark_suppressed[mark];
        if (depth) {
            quiet |= quiet_stack[depth - 1];
        }
        if (debug_dma_level > 0) {
            if (quiet) {
                quiet_str = "<suppressed>";
            } else {
                quiet_str = "";
            }
            NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x2D5)("%s%08d TAG %s: QWC %d NEXT %08d MK %d (%s)%s", indent, tag,
                tagnames[tag->id >> 4], tag->qwc, tag->addr, tag->pad,
                marknames[mark], quiet_str);
        }
        if (!quiet) {
            walker(tag, indent);
        }
        switch (tag->id >> 4) {
        case 1:
            tag += tag->qwc + 1;
            break;
        case 3:
            if (tag->addr & 0xF) {
                NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x2FA)("** Misaligned DMATAG reference **");
            }
            tag++;
            break;
        case 4:
            if (tag->addr & 0xF) {
                NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x303)("** Misaligned DMATAG reference **");
            }
            tag++;
            break;
        case 0:
            tag++;
            break;
        case 5:
            if (depth >= 2) {
                NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x30C)("***** Stack overflow ******");
                return;
            }
            indent_len += 4;
            quiet_stack[depth] = quiet;
            ret_stack[depth] = tag + tag->qwc + 1;
            depth++;
        case 2:
            tag = (struct DmaTag *)tag->addr;
            break;
        case 6:
            if (depth <= 0) {
                NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x31A)("***** Stack underflow ******");
                return;
            }
            depth--;
            indent_len -= 4;
            tag = ret_stack[depth];
            break;
        case 7:
            return;
        default:
            NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x32A)("Unknown tag id");
            for (;;) {
            }
        }
    } while (!decode_error);
}


u32 *gsBeginGifPacket(u32 *packet, int nloop, int eop, int pre, int prim,
                      int flg, int nreg, ...)
{
    u64 *tag = (u64 *)packet;
    va_list regs;
    int i;

    va_start(regs, nreg);
    tag[1] = 0;
    for (i = 0; i < nreg; i++) {
        tag[1] |= (u64)va_arg(regs, int) << (i * 4);
    }
    va_end(regs);
    tag[0] = (nloop & 0x7FFF) | ((u64)(eop & 1) << 15)
        | ((u64)(pre & 1) << 46) | ((u64)(prim & 0x7FF) << 47)
        | ((u64)(flg & 3) << 58) | ((u64)nreg << 60);
    gt_curr = (struct GifTag *)tag;
    return packet + 4;
}


u32 *gsCloseGifPacket(u32 *packet)
{
    struct GifTag *tag = gt_curr;

    switch (tag->flg) {
    case 0:
        tag->nloop = (((u32)packet - (u32)tag - sizeof(struct GifTag)) >> 4)
            / tag->nreg;
        break;
    case 1:
        tag->nloop = (((u32)packet - (u32)tag - sizeof(struct GifTag)) >> 3)
            / tag->nreg;
        break;
    case 2:
        tag->nloop = ((u32)packet - (u32)tag - sizeof(struct GifTag)) >> 4;
        break;
    }
    return packet;
}


void NuScratchReset(void)
{
    ps2_scratch_free = (u32 *)SCRATCHPAD_BASE;
}


u32 *NuScratchAlloc32(u32 size)
{
    u32 *previous = ps2_scratch_free;
    u32 *block = (u32 *)(((u32)previous + 3) & ~3U);

    size = (size + 3) & ~3U;
    ps2_scratch_free = (u32 *)((u32)block + size);
    *ps2_scratch_free = (u32)previous;
    ps2_scratch_free++;
    return block;
}


u32 *NuScratchAlloc64(u32 size)
{
    u32 *previous = ps2_scratch_free;
    u32 *block = (u32 *)(((u32)previous + 7) & ~7U);

    size = (size + 3) & ~3U;
    ps2_scratch_free = (u32 *)((u32)block + size);
    *ps2_scratch_free = (u32)previous;
    ps2_scratch_free++;
    return block;
}


u32 *NuScratchAlloc128(u32 size)
{
    u32 *previous = ps2_scratch_free;
    u32 *block = (u32 *)(((u32)previous + 15) & ~15U);

    size = (size + 3) & ~3U;
    ps2_scratch_free = (u32 *)((u32)block + size);
    *ps2_scratch_free = (u32)previous;
    ps2_scratch_free++;
    return block;
}


void NuScratchRelease(void)
{
    ps2_scratch_free = (u32 *)ps2_scratch_free[-1];
}


struct DmaTag *vpDmaTag_Set(struct DmaTag *tag, u32 qwc, u32 id, u32 addr)
{
    tag->qwc = qwc;
    tag->id = id << 4;
    tag->addr = addr;
    tag->pad = 0;
    tag->vifcode[0] = 0;
    tag->vifcode[1] = 0;
    return tag + 1;
}


struct DmaTag *vpDmaTag_Cnt(struct DmaTag *tag)
{
    static struct DmaTag dmatag = { { { 0, 0, 0x10 } }, 0, { 0, 0 } };

    vpdmatag_curr = tag;
    *tag = dmatag;
    return tag + 1;
}


inline struct DmaTag *vpDmaTag_Ref(struct DmaTag *tag, u32 addr)
{
    static struct DmaTag dmatag = { { { 0, 0, 0x30 } }, 0, { 0, 0 } };

    vpdmatag_curr = tag;
    dmatag.addr = addr;
    *tag = dmatag;
    return tag + 1;
}


struct DmaTag *vpDmaTag_End(struct DmaTag *tag)
{
    static struct DmaTag dmatag = { { { 0, 0, 0x70 } }, 0, { 0, 0 } };

    vpdmatag_curr = tag;
    *tag = dmatag;
    return tag + 1;
}


struct DmaTag *vpDmaTag_EndEx(struct DmaTag *tag, u32 vifcode0, u32 vifcode1)
{
    static struct DmaTag dmatag = { { { 0, 0, 0x70 } }, 0, { 0, 0 } };

    vpdmatag_curr = tag;
    dmatag.vifcode[0] = vifcode0;
    dmatag.vifcode[1] = vifcode1;
    *tag = dmatag;
    return tag + 1;
}


void VU0_DmaWrite(struct DmaTag *tag)
{
    while (VU0_DmaGetStatus()) {
    }
    VIF0_QWC = 0;
    VIF0_TADR = (int)tag;
    DMAC_STAT = DMAC_CH_VIF0;
    VIF0_CHCR = 0x145;
}


int VU0_DmaGetStatus(void)
{
    return VIF0_CHCR & 0x100;
}


int GS_DmaGetStatus(void)
{
    int status;

    if (debug_gs_list) {
        debug_gs_list = 0;
    }
    status = gs_dma_status;
    if (status == 0 && (GIF_CHCR & DMA_STR)) {
        NuErrorProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x570)("bad status");
    }
    return status;
}


void GS_DmaWrite(struct DmaTag *tag)
{
    while (GS_DmaGetStatus()) {
    }
    if (gs_termaddr) {
        gs_termaddr->id = 7;
        gs_termaddr->pad = 0x99;
        gs_termaddr->qwc = 0;
    }
    gs_dma_status = 1;
    DMAC_CTRL |= cycle_steal;
    GIF_QWC = 0;
    GIF_TADR = (int)tag;
    DMAC_STAT = DMAC_CH_GIF;
    GIF_CHCR = 0x185;
    gs_prev_write = gs_last_write;
    gs_last_write = tag;
}


void VU1_DmaWrite(struct DmaTag *tag)
{
    while (VU1_DmaGetStatus()) {
    }
    if (termaddr) {
        termaddr->id = 7;
        termaddr->pad = 0x99;
        termaddr->qwc = 0;
    }
    vu1_dma_status = 1;
    DMAC_CTRL |= cycle_steal;
    VIF1_QWC = 0;
    VIF1_TADR = (int)tag;
    DMAC_STAT = DMAC_CH_VIF1;
    VIF1_CHCR = 0x1C5;
    vu1_prev_write = vu1_last_write;
    vu1_last_write = tag;
}


void VU1_DmaWriteN(struct DmaTag *tag, int qwc)
{
    while (VU1_DmaGetStatus()) {
    }
    VIF1_QWC = qwc;
    VIF1_TADR = (u32)tag & DMA_ADDR_MASK;
    DMAC_STAT = DMAC_CH_VIF1;
    VIF1_CHCR = 0x141;
    vu1_last_write = tag;
}


void VU1_DmaRead(struct DmaTag *tag, int wait)
{
    if (wait) {
        while (VU1_DmaGetStatus()) {
        }
    }
    VIF1_QWC = 0;
    VIF1_TADR = (u32)tag & DMA_ADDR_MASK;
    DMAC_STAT = DMAC_CH_VIF1;
    VIF1_CHCR = 0x141;
}


int VU1_DmaGetStatus(void)
{
    int status;

    if (debug_dma_list) {
        debug_dma_list = 0;
        TagWalk(vu1_last_write, VifTagWalk);
    }
    status = vu1_dma_status;
    if (status == 0 && (VIF1_CHCR & DMA_STR)) {
        NuErrorProlog("..\\nu2crash.ps2\\nups2\\ps2dma.c", 0x580)("bad status");
    }
    return status;
}


void SPR_DmaWriteN(u32 spr, void *data, int qwc)
{
    while (SPR_TO_CHCR & DMA_STR) {
    }
    SPR_TO_QWC = qwc;
    SPR_TO_MADR = (u32)data & DMA_ADDR_MASK;
    SPR_TO_SADR = spr & DMA_ADDR_MASK;
    DMAC_STAT = DMAC_CH_TOSPR;
    SPR_TO_CHCR = 0x101;
}


void SPR_DmaReadN(void *data, u32 spr, int qwc)
{
    while (SPR_FROM_CHCR & DMA_STR) {
    }
    SPR_FROM_QWC = qwc;
    SPR_FROM_MADR = (u32)data & DMA_ADDR_MASK;
    SPR_FROM_SADR = spr & DMA_ADDR_MASK;
    DMAC_STAT = DMAC_CH_FROMSPR;
    SPR_FROM_CHCR = 0x100;
}


int SPR_DmaGetWriteStatus(void)
{
    return (SPR_TO_CHCR >> 8) & 1;
}


int SPR_DmaGetReadStatus(void)
{
    return (SPR_FROM_CHCR >> 8) & 1;
}


void GS_DmaWriteN(void *data, int qwc)
{
    while (GIF_CHCR & DMA_STR) {
    }
    GIF_QWC = qwc;
    GIF_MADR = (u32)data & DMA_ADDR_MASK;
    DMAC_STAT = DMAC_CH_GIF;
    GIF_CHCR = 0x101;
}


int GS_DmaGetWriteStatus(void)
{
    return (GIF_CHCR >> 8) & 1;
}


u32 *vtBeginDirect(u32 *packet, u32 irq)
{
    vt_curr = packet;
    *packet = (irq << 31) | 0x50000000;
    return packet + 1;
}


u32 *vtCloseDirect(u32 *packet)
{
    u32 *last = packet - 1;

    *vt_curr |= (((u32)last - (u32)vt_curr) >> 4) & 0xFFFF;
    return packet;
}


struct DmaTag *VifTagDummy(struct DmaTag *tag, void *user)
{
    return tag;
}


u64 CrcChain(void *addr, int uncached)
{
    struct DmaTag *tag;
    struct DmaTag *next;
    u64 tot;
    int id;
    struct DmaTag *stack[2];
    int stackptr;
    u64 *lptr;
    int n;

    tag = addr;
    stackptr = 0;
    tot = 0;
    if (uncached) {
        tag = (struct DmaTag *)((u32)tag | 0x20000000);
    }
    while (tag) {
        tot += ((u64 *)tag)[0];
        id = tag->id >> 4;
        switch (id) {
        case 1:
            lptr = (u64 *)(tag + 1);
            next = tag + tag->qwc + 1;
            break;
        case 5:
            stack[stackptr++] = tag + 1;
            lptr = (u64 *)(tag + 1);
            next = (struct DmaTag *)tag->addr;
            break;
        case 6:
            lptr = (u64 *)(tag + 1);
            next = stack[--stackptr];
            break;
        case 2:
            next = (struct DmaTag *)tag->addr;
            lptr = (u64 *)(tag + 1);
            break;
        case 0:
        case 7:
            next = 0;
            lptr = (u64 *)(tag + 1);
            break;
        default:
            next = tag + 1;
            lptr = (u64 *)next;
            break;
        }
        tot += ((u64 *)tag)[1];
        do {
            if (tag->qwc) {
                for (n = 0; n < tag->qwc; n++) {
                    tot += *lptr++;
                    tot += *lptr++;
                }
            }
        } while (0);
        tag = next;
    }
    return tot;
}


int VU1_DmaGetStatusI(void)
{
    return VIF1_CHCR & 0x100;
}
