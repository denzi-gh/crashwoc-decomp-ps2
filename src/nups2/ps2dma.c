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

extern struct DmaTag D_002EA550;
extern struct DmaTag D_002EA560;
extern struct DmaTag D_002EA570;
extern struct DmaTag D_002EA580;
extern u32 *D_0062F0E0;
extern struct DmaTag *vpdmatag_curr;
extern u32 *D_00633200;
extern struct GifTag *D_00633208;
extern int D_0062F0D8;
extern int D_0062F0DC;
extern char D_00616E00[];
extern char D_00617850[];
extern int gs_dma_status;
extern int vu1_dma_status;
extern struct DmaTag *vu1_last_write;
extern struct DmaTag *vu1_prev_write;
extern struct DmaTag *gs_last_write;
extern struct DmaTag *gs_prev_write;
extern struct DmaTag *gs_termaddr;
extern struct DmaTag *termaddr;
extern int cycle_steal;
extern int D_0062F0E8;
extern char D_006175F0[];
extern char D_006170D0[], D_006170E0[], D_006170F8[], D_00617110[], D_00617120[];
extern char D_00617130[], D_00617140[], D_00617158[], D_00617168[], D_00617180[];
extern char D_00617190[], D_006171A8[], D_006171C0[], D_006171D0[], D_006171E8[];
extern char D_00617200[], D_00617228[], D_00617250[], D_00617268[], D_00617280[];
extern char D_006172A0[], D_006172C8[], D_006172E0[], D_006172F8[], D_00617310[];
extern char D_00617330[], D_00617350[], D_00617368[], D_00617388[], D_006173A8[];
extern char D_006173C0[], D_006173E0[], D_00617400[], D_00617418[], D_00617430[];
extern char D_0062F100[], D_0062F108[];
extern char *D_002EA460[];
extern char *D_002EA480[];
extern int D_002EA4E8[];
extern int debug_dma_level;
extern char D_00617748[];
extern char D_00617760[];
extern char D_00617778[];
extern char D_00617788[];
extern char D_006177B8[];
extern char D_006177E0[];
extern char D_00617800[];
extern char D_00617820[];
extern char D_00617068[];
extern char D_00617078[];
extern char D_00617098[];
extern char D_006170B8[];
extern char D_0062F0F0[];
extern char D_0062F0F8[];
extern char D_00616E20[];
extern char D_00616E40[];
extern char D_00616A80[], D_00616A90[], D_00616AA0[], D_00616AB0[], D_00616AC0[];
extern char D_00616AD0[], D_00616AE0[], D_00616AF0[], D_00616B00[], D_00616B10[];
extern char D_00616B20[], D_00616B30[], D_00616B40[], D_00616B50[], D_00616B60[];
extern char D_00616B70[], D_00616B80[], D_00616B90[], D_00616BA0[], D_00616BB0[];
extern char D_00616BC0[], D_00616BD0[], D_00616BE0[], D_00616BF0[], D_00616C00[];
extern char D_00616C10[], D_00616C20[], D_00616C30[], D_00616C40[], D_00616C50[];
extern char D_00616C60[], D_00616C70[], D_00616C80[], D_00616C90[], D_00616CA0[];
extern char D_00616CB0[], D_00616CC0[], D_00616CD0[], D_00616CE0[], D_00616CF0[];
extern char D_00616D00[], D_00616D10[], D_00616D20[], D_00616D30[], D_00616D40[];
extern char D_00616D50[], D_00616D60[], D_00616D70[], D_00616D80[], D_00616D90[];
extern char D_00616DA0[], D_00616DB0[], D_00616DC0[], D_00616DD0[], D_00616DE0[];
extern char D_00616DF0[];

extern void NuStrCat(char *dst, char *src);
extern void TagWalk(struct DmaTag *tag, TagWalkFunc walker);
extern struct DmaTag *VifTagWalk(struct DmaTag *tag, void *user);
int VU0_DmaGetStatus(void);
int VU1_DmaGetStatus(void);

#define dmatag_cnt      D_002EA550
#define dmatag_ref      D_002EA560
#define dmatag_end      D_002EA570
#define dmatag_end_ex   D_002EA580
#define nuscratch_ptr   D_0062F0E0
#define giftag_curr     D_00633208
#define gs_dma_walk     D_0062F0D8
#define vu1_dma_walk    D_0062F0DC
#define ps2dma_file     D_00616E00
#define bad_status_msg  D_00617850
#define vif_walk_abort  D_0062F0E8
#define vif_walk_msg    D_006175F0
#define vt_nop_fmt          D_006170D0
#define vt_stcycl_fmt       D_006170E0
#define vt_offset_fmt       D_006170F8
#define vt_base_fmt         D_00617110
#define vt_itop_fmt         D_00617120
#define vt_stmod_fmt        D_00617130
#define vt_mskpath3_fmt     D_00617140
#define vt_mark_fmt         D_00617158
#define vt_flushe_fmt       D_00617168
#define vt_flush_fmt        D_00617180
#define vt_flusha_fmt       D_00617190
#define vt_mscal_fmt        D_006171A8
#define vt_mscnt_fmt        D_006171C0
#define vt_mscalf_fmt       D_006171D0
#define vt_stmask_fmt       D_006171E8
#define vt_strow_fmt        D_00617200
#define vt_stcol_fmt        D_00617228
#define vt_mpg_fmt          D_00617250
#define vt_direct_fmt       D_00617268
#define vt_direct_qw_fmt    D_00617280
#define vt_directhl_fmt     D_006172A0
#define vt_s32_fmt          D_006172C8
#define vt_s16_fmt          D_006172E0
#define vt_s8_fmt           D_006172F8
#define vt_v2_32_fmt        D_00617310
#define vt_v2_16_fmt        D_00617330
#define vt_v2_8_fmt         D_00617350
#define vt_v3_32_fmt        D_00617368
#define vt_v3_16_fmt        D_00617388
#define vt_v3_8_fmt         D_006173A8
#define vt_v4_32_fmt        D_006173C0
#define vt_v4_16_fmt        D_006173E0
#define vt_v4_8_fmt         D_00617400
#define vt_v4_5_fmt         D_00617418
#define vt_unknown_fmt      D_00617430
#define unpack_r_str        D_0062F100
#define unpack_u_str        D_0062F108
#define dmatag_id_names     D_002EA460
#define dmatag_mark_names   D_002EA480
#define dmatag_mark_quiet   D_002EA4E8
#define null_tag_msg        D_00617748
#define misaligned_tag_msg  D_00617760
#define suppressed_str      D_00617778
#define tag_fmt             D_00617788
#define misaligned_ref_msg  D_006177B8
#define overflow_msg        D_006177E0
#define underflow_msg       D_00617800
#define unknown_id_msg      D_00617820
#define prim_fmt        D_00617068
#define giftag_packed_fmt  D_00617078
#define giftag_reglist_fmt D_00617098
#define giftag_image_fmt   D_006170B8
#define eop_str         D_0062F0F0
#define empty_str       D_0062F0F8
#define gs_packed_fmt   D_00616E20
#define gs_reglist_fmt  D_00616E40


void GSRegDecode(char *indent, u32 reg, u32 *data, int packed)
{
    char *name;

    switch (reg) {
    case GS_PRIM:
        name = D_00616A80;
        break;
    case GS_RGBAQ:
        name = D_00616A90;
        break;
    case GS_ST:
        name = D_00616AA0;
        break;
    case GS_UV:
        name = D_00616AB0;
        break;
    case GS_XYZF2:
        name = D_00616AC0;
        break;
    case GS_XYZ2:
        name = D_00616AD0;
        break;
    case GS_FOG:
        name = D_00616AE0;
        break;
    case GS_XYZF3:
        name = D_00616AF0;
        break;
    case GS_XYZ3:
        name = D_00616B00;
        break;
    case GS_XYOFFSET_1:
        name = D_00616B10;
        break;
    case GS_XYOFFSET_2:
        name = D_00616B20;
        break;
    case GS_PRMODECONT:
        name = D_00616B30;
        break;
    case GS_PRMODE:
        name = D_00616B40;
        break;
    case GS_TEX0_1:
        name = D_00616B50;
        break;
    case GS_TEX0_2:
        name = D_00616B60;
        break;
    case GS_TEX1_1:
        name = D_00616B70;
        break;
    case GS_TEX1_2:
        name = D_00616B80;
        break;
    case GS_TEX2_1:
        name = D_00616B90;
        break;
    case GS_TEX2_2:
        name = D_00616BA0;
        break;
    case GS_TEXCLUT:
        name = D_00616BB0;
        break;
    case GS_SCANMSK:
        name = D_00616BC0;
        break;
    case GS_MIPTBP1_1:
        name = D_00616BD0;
        break;
    case GS_MIPTBP1_2:
        name = D_00616BE0;
        break;
    case GS_MIPTBP2_1:
        name = D_00616BF0;
        break;
    case GS_MIPTBP2_2:
        name = D_00616C00;
        break;
    case GS_CLAMP_1:
        name = D_00616C10;
        break;
    case GS_CLAMP_2:
        name = D_00616C20;
        break;
    case GS_TEXA:
        name = D_00616C30;
        break;
    case GS_FOGCOL:
        name = D_00616C40;
        break;
    case GS_TEXFLUSH:
        name = D_00616C50;
        break;
    case GS_SCISSOR_1:
        name = D_00616C60;
        break;
    case GS_SCISSOR_2:
        name = D_00616C70;
        break;
    case GS_ALPHA_1:
        name = D_00616C80;
        break;
    case GS_ALPHA_2:
        name = D_00616C90;
        break;
    case GS_DIMX:
        name = D_00616CA0;
        break;
    case GS_DTHE:
        name = D_00616CB0;
        break;
    case GS_COLCLAMP:
        name = D_00616CC0;
        break;
    case GS_TEST_1:
        name = D_00616CD0;
        break;
    case GS_TEST_2:
        name = D_00616CE0;
        break;
    case GS_PABE:
        name = D_00616CF0;
        break;
    case GS_FBA_1:
        name = D_00616D00;
        break;
    case GS_FBA_2:
        name = D_00616D10;
        break;
    case GS_FRAME_1:
        name = D_00616D20;
        break;
    case GS_FRAME_2:
        name = D_00616D30;
        break;
    case GS_ZBUF_1:
        name = D_00616D40;
        break;
    case GS_ZBUF_2:
        name = D_00616D50;
        break;
    case GS_BITBLTBUF:
        name = D_00616D60;
        break;
    case GS_TRXPOS:
        name = D_00616D70;
        break;
    case GS_TRXREG:
        name = D_00616D80;
        break;
    case GS_TRXDIR:
        name = D_00616D90;
        break;
    case GS_HWREG:
        name = D_00616DA0;
        break;
    case GS_SIGNAL:
        name = D_00616DB0;
        break;
    case GS_FINISH:
        name = D_00616DC0;
        break;
    case GS_LABEL:
        name = D_00616DD0;
        break;
    case GS_NOP:
        name = D_00616DE0;
        break;
    default:
        name = D_00616DF0;
        break;
    }
    if (packed) {
        NuDebugMsgProlog(ps2dma_file, 0x107)(gs_packed_fmt, indent, name,
                                             data[3], data[2], data[1], data[0]);
    } else {
        NuDebugMsgProlog(ps2dma_file, 0x10A)(gs_reglist_fmt, indent, name,
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
                sprintf(prim, prim_fmt, (int)gif->prim);
            } else {
                prim[0] = 0;
            }
            if (gif->eop) {
                eop = eop_str;
            } else {
                eop = empty_str;
            }
            NuDebugMsgProlog(ps2dma_file, 0x12E)(giftag_packed_fmt, indent, size - 1, prim, eop);
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
            NuDebugMsgProlog(ps2dma_file, 0x146)(giftag_reglist_fmt, prefix, count);
            while (count != 0) {
                GSRegDecode(indent, ((u32 *)packet)[2], (u32 *)packet, 0);
                count--;
                packet += 2;
            }
            break;
        default:
            size = gif->nloop + 1;
            NuDebugMsgProlog(ps2dma_file, 0x152)(giftag_image_fmt, indent, size - 1);
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
    u32 *qw;
    u32 vifcode;
    u32 qwc;
    u32 i;
    int num;

    start = code;
    vifcode = *code++;
    switch ((vifcode >> 24) & 0x7F) {
    case 0x00:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog(ps2dma_file, 0x16C)(vt_nop_fmt, indent, start);
        }
        break;
    case 0x01:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog(ps2dma_file, 0x170)(vt_stcycl_fmt, indent, start,
                (vifcode >> 8) & 0xFF, vifcode & 0xFF);
        }
        break;
    case 0x02:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog(ps2dma_file, 0x174)(vt_offset_fmt, indent, start);
        }
        break;
    case 0x03:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog(ps2dma_file, 0x178)(vt_base_fmt, indent, start);
        }
        break;
    case 0x04:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog(ps2dma_file, 0x17C)(vt_itop_fmt, indent, start);
        }
        break;
    case 0x05:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog(ps2dma_file, 0x180)(vt_stmod_fmt, indent, start);
        }
        break;
    case 0x06:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog(ps2dma_file, 0x184)(vt_mskpath3_fmt, indent, start);
        }
        break;
    case 0x07:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog(ps2dma_file, 0x188)(vt_mark_fmt, indent, start);
        }
        break;
    case 0x10:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog(ps2dma_file, 0x18C)(vt_flushe_fmt, indent, start);
        }
        break;
    case 0x11:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog(ps2dma_file, 0x190)(vt_flush_fmt, indent, start);
        }
        break;
    case 0x13:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog(ps2dma_file, 0x194)(vt_flusha_fmt, indent, start);
        }
        break;
    case 0x14:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog(ps2dma_file, 0x198)(vt_mscal_fmt, indent, start,
                vifcode & 0xFFFF);
        }
        break;
    case 0x17:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog(ps2dma_file, 0x19C)(vt_mscnt_fmt, indent, start);
        }
        break;
    case 0x15:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog(ps2dma_file, 0x1A0)(vt_mscalf_fmt, indent, start,
                vifcode & 0xFFFF);
        }
        break;
    case 0x20:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog(ps2dma_file, 0x1A4)(vt_stmask_fmt, indent, start, code[0]);
        }
        code += 1;
        break;
    case 0x30:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog(ps2dma_file, 0x1A9)(vt_strow_fmt, indent, start,
                code[0], code[1], code[2], code[3]);
        }
        code += 4;
        break;
    case 0x31:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog(ps2dma_file, 0x1AE)(vt_stcol_fmt, indent, start,
                code[0], code[1], code[2], code[3]);
        }
        code += 4;
        break;
    case 0x4A:
        if (debug_dma_level > 0) {
            NuDebugMsgProlog(ps2dma_file, 0x1B3)(vt_mpg_fmt, indent, start,
                vifcode & 0xFFFF, (vifcode >> 16) & 0xFF);
        }
        if ((vifcode & 0xFF0000) == 0) {
            code += 512;
        } else {
            code += ((vifcode & 0xFF0000) >> 16) * 2;
        }
        break;
    case 0x50:
        qwc = vifcode & 0xFFFF;
        if (debug_dma_level > 0) {
            NuDebugMsgProlog(ps2dma_file, 0x1BB)(vt_direct_fmt, indent, start, qwc);
        }
        if (debug_dma_level == 5) {
            qw = code;
            for (i = 0; i < (qwc > 10 ? 10 : qwc); i++) {
                NuDebugMsgProlog(ps2dma_file, 0x1BE)(vt_direct_qw_fmt, indent, i,
                    qw[3], qw[2], qw[1], qw[0]);
                qw += 4;
            }
        }
        if (debug_dma_level >= 7) {
            DirectDecode((u64 *)code, indent, qwc);
        }
        code += qwc * 4;
        break;
    case 0x51:
        qwc = vifcode & 0xFFFF;
        if (debug_dma_level > 0) {
            NuDebugMsgProlog(ps2dma_file, 0x1C8)(vt_directhl_fmt, indent, start, qwc);
        }
        code += qwc * 4;
        break;
    default:
        if ((vifcode & 0x60000000) == 0x60000000) {
            strcpy(name, "VT UNPACK");
            if (vifcode & 0x8000) {
                strcat(name, unpack_r_str);
            }
            if (vifcode & 0x4000) {
                strcat(name, unpack_u_str);
            }
            num = (vifcode >> 16) & 0xFF;
            if (num == 0) {
                num = 256;
            }
            switch ((vifcode >> 24) & 0xF) {
            case 0:
                if (debug_dma_level > 0) {
                    NuDebugMsgProlog(ps2dma_file, 0x1DD)(vt_s32_fmt, indent, start, name,
                        num, vifcode & 0x3FF);
                }
                code = (u32 *)((u8 *)code + num * 4);
                break;
            case 1:
                if (debug_dma_level > 0) {
                    NuDebugMsgProlog(ps2dma_file, 0x1E2)(vt_s16_fmt, indent, start, name,
                        num, vifcode & 0x3FF);
                }
                code = (u32 *)((u8 *)code + num * 2);
                break;
            case 2:
                if (debug_dma_level > 0) {
                    NuDebugMsgProlog(ps2dma_file, 0x1E7)(vt_s8_fmt, indent, start, name,
                        num, vifcode & 0x3FF);
                }
                code = (u32 *)((u8 *)code + num);
                break;
            case 4:
                if (debug_dma_level > 0) {
                    NuDebugMsgProlog(ps2dma_file, 0x1EC)(vt_v2_32_fmt, indent, start, name,
                        num, vifcode & 0x3FF);
                }
                code = (u32 *)((u8 *)code + num * 8);
                break;
            case 5:
                if (debug_dma_level > 0) {
                    NuDebugMsgProlog(ps2dma_file, 0x1F1)(vt_v2_16_fmt, indent, start, name,
                        num, vifcode & 0x3FF);
                }
                code = (u32 *)((u8 *)code + num * 4);
                break;
            case 6:
                if (debug_dma_level > 0) {
                    NuDebugMsgProlog(ps2dma_file, 0x1F6)(vt_v2_8_fmt, indent, start, name,
                        num, vifcode & 0x3FF);
                }
                code = (u32 *)((u8 *)code + num * 2);
                break;
            case 8:
                if (debug_dma_level > 0) {
                    NuDebugMsgProlog(ps2dma_file, 0x1FB)(vt_v3_32_fmt, indent, start, name,
                        num, vifcode & 0x3FF);
                }
                code = (u32 *)((u8 *)code + num * 12);
                break;
            case 9:
                if (debug_dma_level > 0) {
                    NuDebugMsgProlog(ps2dma_file, 0x200)(vt_v3_16_fmt, indent, start, name,
                        num, vifcode & 0x3FF);
                }
                code = (u32 *)((u8 *)code + num * 6);
                break;
            case 10:
                if (debug_dma_level > 0) {
                    NuDebugMsgProlog(ps2dma_file, 0x205)(vt_v3_8_fmt, indent, start, name,
                        num, vifcode & 0x3FF);
                }
                code = (u32 *)((u8 *)code + num * 3);
                break;
            case 12:
                if (debug_dma_level > 0) {
                    NuDebugMsgProlog(ps2dma_file, 0x20A)(vt_v4_32_fmt, indent, start, name,
                        num, vifcode & 0x3FF);
                }
                code = (u32 *)((u8 *)code + num * 16);
                break;
            case 13:
                if (debug_dma_level > 0) {
                    NuDebugMsgProlog(ps2dma_file, 0x20F)(vt_v4_16_fmt, indent, start, name,
                        num, vifcode & 0x3FF);
                }
                code = (u32 *)((u8 *)code + num * 8);
                break;
            case 14:
                if (debug_dma_level > 0) {
                    NuDebugMsgProlog(ps2dma_file, 0x214)(vt_v4_8_fmt, indent, start, name,
                        num, vifcode & 0x3FF);
                }
                code = (u32 *)((u8 *)code + num * 4);
                break;
            case 15:
                if (debug_dma_level > 0) {
                    NuDebugMsgProlog(ps2dma_file, 0x219)(vt_v4_5_fmt, indent, start, name,
                        num, vifcode & 0x3FF);
                }
                code = (u32 *)((u8 *)code + num * 8);
                break;
            }
            if ((u32)code & 3) {
                code = (u32 *)((u32)code + 4 - ((u32)code & 3));
            }
        } else {
            NuDebugMsgProlog(ps2dma_file, 0x222)(vt_unknown_fmt, indent, start, vifcode);
            for (;;) {
            }
        }
        break;
    }
    return code;
}


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
        NuDebugMsgProlog(ps2dma_file, 0x2B4)(null_tag_msg);
    }
    do {
        if (indent_len) {
            memset(indent, ' ', indent_len);
        }
        indent[indent_len] = 0;
        if ((u32)tag & 0xF) {
            NuDebugMsgProlog(ps2dma_file, 0x2BB)(misaligned_tag_msg);
            for (;;) {
            }
        }
        history[history_pos] = tag;
        history_pos = (history_pos + 1) & 0x1F;
        mark = tag->pad;
        if (mark >= 26) {
            mark = 0;
        }
        quiet = dmatag_mark_quiet[mark];
        if (depth) {
            quiet |= quiet_stack[depth - 1];
        }
        if (debug_dma_level > 0) {
            if (quiet) {
                quiet_str = suppressed_str;
            } else {
                quiet_str = empty_str;
            }
            NuDebugMsgProlog(ps2dma_file, 0x2D5)(tag_fmt, indent, tag,
                dmatag_id_names[tag->id >> 4], tag->qwc, tag->addr, tag->pad,
                dmatag_mark_names[mark], quiet_str);
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
                NuDebugMsgProlog(ps2dma_file, 0x2FA)(misaligned_ref_msg);
            }
            tag++;
            break;
        case 4:
            if (tag->addr & 0xF) {
                NuDebugMsgProlog(ps2dma_file, 0x303)(misaligned_ref_msg);
            }
            tag++;
            break;
        case 0:
            tag++;
            break;
        case 5:
            if (depth >= 2) {
                NuDebugMsgProlog(ps2dma_file, 0x30C)(overflow_msg);
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
                NuDebugMsgProlog(ps2dma_file, 0x31A)(underflow_msg);
                return;
            }
            depth--;
            indent_len -= 4;
            tag = ret_stack[depth];
            break;
        case 7:
            return;
        default:
            NuDebugMsgProlog(ps2dma_file, 0x32A)(unknown_id_msg);
            for (;;) {
            }
        }
    } while (!vif_walk_abort);
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
    giftag_curr = (struct GifTag *)tag;
    return packet + 4;
}


u32 *gsCloseGifPacket(u32 *packet)
{
    struct GifTag *tag = giftag_curr;

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
    nuscratch_ptr = (u32 *)SCRATCHPAD_BASE;
}


u32 *NuScratchAlloc32(u32 size)
{
    u32 *previous = nuscratch_ptr;
    u32 *block = (u32 *)(((u32)previous + 3) & ~3U);

    size = (size + 3) & ~3U;
    nuscratch_ptr = (u32 *)((u32)block + size);
    *nuscratch_ptr = (u32)previous;
    nuscratch_ptr++;
    return block;
}


u32 *NuScratchAlloc64(u32 size)
{
    u32 *previous = nuscratch_ptr;
    u32 *block = (u32 *)(((u32)previous + 7) & ~7U);

    size = (size + 3) & ~3U;
    nuscratch_ptr = (u32 *)((u32)block + size);
    *nuscratch_ptr = (u32)previous;
    nuscratch_ptr++;
    return block;
}


u32 *NuScratchAlloc128(u32 size)
{
    u32 *previous = nuscratch_ptr;
    u32 *block = (u32 *)(((u32)previous + 15) & ~15U);

    size = (size + 3) & ~3U;
    nuscratch_ptr = (u32 *)((u32)block + size);
    *nuscratch_ptr = (u32)previous;
    nuscratch_ptr++;
    return block;
}


void NuScratchRelease(void)
{
    nuscratch_ptr = (u32 *)nuscratch_ptr[-1];
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
    vpdmatag_curr = tag;
    *tag = dmatag_cnt;
    return tag + 1;
}


struct DmaTag *vpDmaTag_End(struct DmaTag *tag)
{
    vpdmatag_curr = tag;
    *tag = dmatag_end;
    return tag + 1;
}


struct DmaTag *vpDmaTag_EndEx(struct DmaTag *tag, u32 vifcode0, u32 vifcode1)
{
    vpdmatag_curr = tag;
    dmatag_end_ex.vifcode[0] = vifcode0;
    dmatag_end_ex.vifcode[1] = vifcode1;
    *tag = dmatag_end_ex;
    return tag + 1;
}


struct DmaTag *vpDmaTag_Ref(struct DmaTag *tag, u32 addr)
{
    vpdmatag_curr = tag;
    dmatag_ref.addr = addr;
    *tag = dmatag_ref;
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

    if (gs_dma_walk) {
        gs_dma_walk = 0;
    }
    status = gs_dma_status;
    if (status == 0 && (GIF_CHCR & DMA_STR)) {
        NuErrorProlog(ps2dma_file, 0x570)(bad_status_msg);
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

    if (vu1_dma_walk) {
        vu1_dma_walk = 0;
        TagWalk(vu1_last_write, VifTagWalk);
    }
    status = vu1_dma_status;
    if (status == 0 && (VIF1_CHCR & DMA_STR)) {
        NuErrorProlog(ps2dma_file, 0x580)(bad_status_msg);
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
    D_00633200 = packet;
    *packet = (irq << 31) | 0x50000000;
    return packet + 1;
}


u32 *vtCloseDirect(u32 *packet)
{
    u32 *direct = D_00633200;

    *direct |= (((u32)(packet - 1) - (u32)direct) >> 4) & 0xFFFF;
    return packet;
}


struct DmaTag *VifTagDummy(struct DmaTag *tag, void *user)
{
    return tag;
}


struct DmaTag *VifTagWalk(struct DmaTag *tag, void *user)
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
        if (vif_walk_abort) {
            return (struct DmaTag *)code;
        }
    }
    if (code != end) {
        NuDebugMsgProlog(ps2dma_file, 0x251)(vif_walk_msg);
        for (;;) {
        }
    }
    return (struct DmaTag *)code;
}


u64 CrcChain(struct DmaTag *tag, int uncached)
{
    struct DmaTag *stack[4];
    u64 *data;
    struct DmaTag *next;
    u64 crc;
    int depth;
    int qwc;
    int i;

    depth = 0;
    crc = 0;
    if (uncached) {
        tag = (struct DmaTag *)((u32)tag | 0x20000000);
    }
    while (tag) {
        crc += ((u64 *)tag)[0];
        switch (tag->id >> 4) {
        case 1:
            data = (u64 *)(tag + 1);
            next = tag + tag->qwc + 1;
            break;
        case 5:
            stack[depth++] = tag + 1;
            data = (u64 *)(tag + 1);
            next = (struct DmaTag *)tag->addr;
            break;
        case 6:
            data = (u64 *)(tag + 1);
            next = stack[--depth];
            break;
        case 2:
            next = (struct DmaTag *)tag->addr;
            data = (u64 *)(tag + 1);
            break;
        case 0:
        case 7:
            next = 0;
            data = (u64 *)(tag + 1);
            break;
        default:
            next = tag + 1;
            data = (u64 *)next;
            break;
        }
        crc += ((u64 *)tag)[1];
        qwc = tag->qwc;
        if (qwc) {
            for (i = 0; i < qwc; i++) {
                crc += *data++;
                crc += *data++;
            }
        }
        tag = next;
    }
    return crc;
}


int VU1_DmaGetStatusI(void)
{
    return VIF1_CHCR & 0x100;
}
