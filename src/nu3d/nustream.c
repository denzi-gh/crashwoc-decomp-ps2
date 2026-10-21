/*
 * Unit: nu3d/nustream
 *
 * Functions:
 *   0x001103f8 NuRndrInitStreams
 *   0x00110580 NuRndrStreamInit
 *   0x001106e8 NuRndrStreamAllocStream
 *   0x00110838 NuRndrStreamFlush
 *   0x00110a50 NuRndrStreamSetMisc
 *   0x00110e28 NuRndrStreamSetLocalMtx
 *   0x00110f08 NuRndrSetViewMtx
 *   0x00111208 NuRndrSetSpecularLight
 *   0x00111420 NuStreamGetOutputBuffer
 *   0x00111438 NuRndrSwapStreamBuffers
 *   0x00111470 FaceYDirStream
 *   0x00111538 NuRndrSetLocalMtx
 *   0x00111650 NuRndrSetXYOffset
 *   0x00111670 NuRndrSetScissor
 *   0x001116a0 NuRndrSetZFix
 *   0x00111738 NuRndrSetDirectionalLights
 *   0x001117f0 NuRndrSetDirectionalLights4
 *   0x001118b0 NuRndrSetAmbientLight
 *   0x001118e8 NuRndrSetGlobalAlpha
 *   0x001118f0 NuRndrSetFxMtx
 *   0x00111988 NuRndrStreamGetScissor
 *   0x00111998 NuRndrStreamGetOffset
 *   0x001119a8 NuRndrStreamResetStream
 *   0x001119e8 NuRndrStreamResetStreams
 *   0x00111a68 NuRndrVu1Draw
 *   0x00111ad8 NuRndrStreamFxMPG
 *   0x00111b38 NuRndrStreamAlloc
 *   0x00111b50 NuRndrStreamAddMtl
 *   0x00111c18 NuRndrStreamAddTex
 *   0x00111c78 NuRndrStreamSetDirectionalLights
 *   0x00111c80 NuRndrStreamSetAmbientLight
 *   0x00111c88 NuRndrStreamSetViewMtx
 *   0x00111d28 NuRndrStreamSetViewMtxScissorClip
 *   0x00111d98 NuRndrStreamSetLocalMtxEx
 *   0x00111eb0 NuRndrStreamPrependMPG
 *   0x00111f18 NuRndrStreamClearZBState
 *   0x00111f20 NuRndrStreamSetZBState
 *   0x00112000 NuRndrStreamAddZBState
 *   0x00112060 CheckRndrStream
 */


#include "nu_libc.h"
#include "nu_types.h"

#include "nu3d/numtl.h"
#include "nu3d/nustream.h"
#include "nu3d/nutex.h"
#include "nucore/nuerror.h"
#include "numath/numathtypes.h"
#include "numath/numtx.h"
#include "numath/nuvec.h"
#include "nu3d/nucamera.h"
#include "nu3d/nuvport.h"

struct nucolour4_s {
    struct nucolour3_s rgb;
    float a;
};

struct _sceDmaTag {
    u16 qwc;
    u8 mark;
    u8 id;
    struct _sceDmaTag *next;
    u32 p[2];
}; /* 0x10 */

typedef union {
    u128 qw;
    struct _sceDmaTag tag;
} NuDmaTagQw;

struct nurtlight_s {
    NuDmaTagQw cnt;
    float dirx[3];
    char pad_0x1C[4];
    float diry[3];
    char pad_0x2C[4];
    float dirz[3];
    char pad_0x3C[4];
    struct nucolour4_s col[3];
    float r;
    float g;
    float b;
    float a;
    NuDmaTagQw ref;
};

struct nuspecpkt_s {
    NuDmaTagQw cnt;
    struct numtx_s mtx;
    struct nucolour4_s col;
    NuDmaTagQw ref;
};

struct nuzbstate_s {
    char pad[0x24];
    int zmask;
};

struct nulocalmtxpkt_s {
    char tag[0x10];
    struct numtx_s mtx;
    int unpack;
    short mode;
    char pad_0x56[6];
    unsigned int mscal;
    unsigned int flush;
};

union vifword_u {
    unsigned int u;
    float f;
};

struct nurndrfxmtx_s {
    NuDmaTagQw cnt;
    struct numtx_s mtx;
    NuDmaTagQw ref;
};


extern unsigned char *D_00633020;
#define stream_outbuf0 D_00633020

extern unsigned char *D_00633024;
#define stream_outbuf1 D_00633024

extern u64 D_00633038[1];
#define rndrstream_scissor D_00633038[0]

extern u64 D_00633040[1];
#define rndrstream_offset D_00633040[0]

extern int D_0062EAC0;
#define zbstate_set D_0062EAC0

extern short rndrstream_scissor_id;

extern short rndrstream_xyoffset_id;

extern char *rndrstream_free[1];

extern int D_0062EA74;
#define rndrstream_buffer_index D_0062EA74

extern struct nurtlight_s rndrstream_rt_light_buffer;

extern void *rndrstream_rt_light_addr[1];

extern void *gs_top;

extern struct rndrstream_s *rndrstream_fx;

extern void *rndrstream_fx_microcode;

extern char D_0028CFF0[];

extern char D_00289020[];
#define fx_microcode_default D_00289020

extern void *vpDmaTag_Call(void *cur, void *microcode);

extern void *vpDmaTag_Next(void *cur, void *mode);

extern struct nuzbstate_s D_0067A600;
#define zbstate_pkt D_0067A600

extern int D_002D3E00[];
#define zmask_table D_002D3E00

extern void NuMemCopy128(void *dst, void *src, int count);

extern void *rndrstream_fx_mtx_addr[1];

extern struct nurndrfxmtx_s D_0067A590;
#define rndrstream_fx_mtx D_0067A590

extern void *txtrstream_start[1];

extern void *rndrstream_start;

extern void FlushCache(int mode);

extern void GS_DmaWrite(void *p, int mode);

extern void VU1_DmaWrite(void *p, int mode);

extern void tbslotReset(int a, int b);

extern void tbslotBeginFn(int a, int b);

extern int D_0062EA84;
#define view_mtx_pkt_size D_0062EA84

extern char D_0067A460[];
#define clip_view_mtx_pkt D_0067A460

extern char D_0067A3C0[];
#define view_mtx_pkt D_0067A3C0

extern int D_00633028;
#define rndrstream_count D_00633028

extern struct rndrstream_s D_00679B40[];
#define rndrstream_pool D_00679B40

extern int rndrstream_local_mtx_id;

extern void *rndrstream_last_mtx[1];

extern int D_0062EA80;
#define local_mtx_pkt_size D_0062EA80

extern struct nulocalmtxpkt_s D_0067A340;
#define local_mtx_pkt D_0067A340

extern float NuTrigTable[];

extern char D_0067A500[];
#define faceon_dir_pkt D_0067A500

extern void *vpDmaTag_Cnt(void *packet);

extern void *vpDmaTag_Close(void *packet);

extern float D_0062EA78;
#define rndrstream_zfix D_0062EA78

extern int D_0062EAB8;
#define view_mtx_id_next D_0062EAB8

extern short rndrstream_view_mtx_id;

extern struct numtx_s D_0067A2C0;
#define zfix_mtx D_0067A2C0

extern struct numtx_s D_0067A300;
#define view_mtx D_0067A300

extern struct numtx_s D_0067A3D0;
#define vu1_view_mtx D_0067A3D0

extern struct numtx_s D_0067A470;
#define vu1_clip_view_mtx D_0067A470

extern u8 *vpDmaTag_CntEx(void *packet, unsigned int vif0, unsigned int vif1);

extern void *vtBeginDirect(void *packet, unsigned int irq);

extern void *vtCloseDirect(void *packet);

extern void *gsBeginGifPacket(void *packet, int nloop, int eop, int pre,
                              int prim, int flg, int nreg, ...);

extern void *gsCloseGifPacket(void *packet);

extern int D_0062EA70;
#define stream_outbuf_size D_0062EA70

extern char D_006140D0[];
#define nustream_file D_006140D0

extern char D_006140F0[];
#define trashed_stream_a_msg D_006140F0

extern char D_00614108[];
#define trashed_stream_b_msg D_00614108

extern int D_0062EA90;
#define stream_outbuf_offset D_0062EA90

extern int D_0062EA94;
#define stream_outbuf_cycle D_0062EA94

extern int D_0062EA98;
#define stream_outbuf_cycle_delay D_0062EA98

extern int D_0062EA9C;
#define stream_outbuf_cycle_len D_0062EA9C

extern struct _sceDmaTag *rndrstream_end[1];

extern struct _sceDmaTag *txtrstream_end[1];

extern u8 *frame_base;

extern struct rndrstream_s *rndrstream_system;

extern void NuRndrStreamLink(struct rndrstream_s *stream);

extern struct rndrstream_s *D_0067A240[];
#define rndrstream_sorted D_0067A240

extern int sortstreams_enabled;

extern void *NuMemAllocFn(int size, char *file, int line);

extern struct rndrstream_s *rndrstream_mtl_sort;

extern struct rndrstream_s *rndrstream_faceon;

extern struct rndrstream_s *rndrstream_3d_u;

extern struct rndrstream_s *rndrstream_3d_l;

extern struct rndrstream_s *rndrstream_2d;

extern struct rndrstream_s *rndrstream_ptl;

extern struct rndrstream_s *rndrstream_ot_sort;

extern char D_002834E0[];
#define rndr3d_l_microcode D_002834E0

extern char D_00284620[];
#define rndr3d_u_microcode D_00284620

extern char D_002885B0[];
#define faceon_microcode D_002885B0

extern char D_002888E0[];
#define ptl_microcode D_002888E0

extern int testfclip;

extern int D_0062EAA4;
#define rt_light_dirty D_0062EAA4

extern int D_0062EAA8;
#define spec_dirty D_0062EAA8

extern int D_0062EAAC;
#define fx_mtx_dirty D_0062EAAC

extern struct nuspecpkt_s D_0067A520;
#define rndrstream_spec D_0067A520

extern NuDmaTagQw D_002D3EA0;
#define rt_light_reftag D_002D3EA0

extern NuDmaTagQw D_0067A580;
#define spec_reftag D_0067A580

extern NuDmaTagQw D_0067A5E0;
#define fx_mtx_reftag D_0067A5E0

extern char D_00614148[];
#define bad_attrib_msg D_00614148

extern u8 *vpDmaTag_RefEx(void *packet, void *addr, int qwc, unsigned int vif1);

extern void *D_00633030[1];
#define rndrstream_spec_addr D_00633030[0]

extern char D_00614120[];
#define stream_overflow_msg D_00614120

extern int D_0062EABC;
#define view_mtx_pkt_dirty D_0062EABC

extern struct numtx_s D_0067A420;
#define vu1_proj_mtx D_0067A420

extern struct numtx_s D_0067A4C0;
#define vu1_clip_proj_mtx D_0067A4C0

extern struct nuvec_s D_002D3DE0;
#define spec_light_dir D_002D3DE0

extern struct nucolour4_s D_002D3DF0;
#define spec_light_col D_002D3DF0

extern struct nuvec_s D_002D3E08;
#define spec_view_dir D_002D3E08

extern struct nucamera_s global_camera;

extern struct numtx_s numtx_identity;

extern int D_0062EAB4;
#define local_mtx_pkt_dirty D_0062EAB4

extern int D_0062EAB0;
#define local_mtx_id_next D_0062EAB0

extern int D_0063302C;
#define local_mtx_clip D_0063302C


void NuRndrInitStreams(void)
{
    int wrap;
    struct rndrstream_s *sys;

    if (stream_outbuf_cycle) {
        if (stream_outbuf_cycle_delay == 0) {
            stream_outbuf_cycle_delay = 10;
            wrap = stream_outbuf_offset / stream_outbuf_cycle_len;
            stream_outbuf_offset++;
        }
        stream_outbuf_cycle_delay--;
    }
    switch (rndrstream_buffer_index) {
    case 0:
        rndrstream_free[0] = stream_outbuf0 + stream_outbuf_offset;
        break;
    case 1:
        rndrstream_free[0] = stream_outbuf1 + stream_outbuf_offset;
        break;
    case 2:
        break;
    }
    if ((unsigned int)rndrstream_free[0] & 0xF) {
        rndrstream_free[0] = (char *)(((unsigned int)rndrstream_free[0] + 0xF) & ~0xF);
    }
    rndrstream_start = 0;
    frame_base = rndrstream_free[0];
    rndrstream_end[0] = 0;
    txtrstream_start[0] = 0;
    txtrstream_end[0] = 0;
    NuRndrStreamResetStreams(1);
    sys = rndrstream_system;
    NuRndrStreamLink(sys);
    sys->ptr.u8 = vpDmaTag_CntEx(sys->ptr.u8, 0x13000000, 0x50000000) - 4;
    sys->ptr.u8 = vtBeginDirect(sys->ptr.u8, 0);
    sys->ptr.u8 = gsBeginGifPacket(sys->ptr.u8, 0, 1, 0, 0, 0, 1, 0xE);
    *((u64 *)sys->ptr.u8)++ = 0;
    *((u64 *)sys->ptr.u8)++ = 0x1A;
    sys->ptr.u8 = gsCloseGifPacket(sys->ptr.u8);
    sys->ptr.u8 = vtCloseDirect(sys->ptr.u8);
    vpDmaTag_Close(sys->ptr.u8);
    rndrstream_free[0] = vpDmaTag_Next(sys->ptr.u8, 0);
    NuMtxSetIdentity(&zfix_mtx);
}


void NuRndrStreamInit(int size)
{
    rndrstream_count = 0;
    stream_outbuf0 = NuMemAllocFn(size, nustream_file, 0x207);
    stream_outbuf1 = NuMemAllocFn(size, nustream_file, 0x208);
    stream_outbuf_size = size;
    rndrstream_system = NuRndrStreamAllocStream(0x7FFFFFFF, 0, 0);
    rndrstream_mtl_sort = NuRndrStreamAllocStream(0x1000000, 0, 0x3D);
    rndrstream_faceon = NuRndrStreamAllocStream(0x1000000, faceon_microcode, 0x3C);
    rndrstream_3d_u = NuRndrStreamAllocStream(0x10000000, rndr3d_u_microcode, 0x3C);
    rndrstream_3d_l = NuRndrStreamAllocStream(0x10000000, rndr3d_l_microcode, 0x3D);
    rndrstream_2d = NuRndrStreamAllocStream(0x100000, 0, 0x10);
    rndrstream_ptl = NuRndrStreamAllocStream(0xFFFFFF, ptl_microcode, 0x3C);
    rndrstream_fx = NuRndrStreamAllocStream(0xFFFFFF, fx_microcode_default, 0x3C);
    rndrstream_ot_sort = NuRndrStreamAllocStream(0xFFFFFF, 0, 0x3D);
    rndrstream_buffer_index = 0;
    memset(stream_outbuf0, 0, size);
    memset(stream_outbuf1, 0, size);
    rndrstream_fx_microcode = fx_microcode_default;
    NuRndrInitStreams();
}


struct rndrstream_s *NuRndrStreamAllocStream(unsigned int sortkey, void *mpgtag, unsigned int flags)
{
    struct rndrstream_s *stream;
    int i;
    int j;

    if (rndrstream_count < 32) {
        stream = &rndrstream_pool[rndrstream_count];
        memset(stream, 0, sizeof(struct rndrstream_s));
        stream->mpg = mpgtag;
        stream->attr = flags;
        stream->mtl = 0;
        stream->drawpri = sortkey;
        stream->base.u8 = 0;
        stream->ptr.u8 = 0;
        stream->t_base.u32 = 0;
        stream->t_ptr.u32 = 0;
        stream->local_mtx_id = -2;
        stream->view_mtx_id = -2;
        stream->scissor_id = -2;
        stream->misc_addr.voidptr = 0;
        stream->tid = 0;
        stream->xyoffset_id = -2;
        stream->gs_ptr = gs_top;
        rndrstream_sorted[rndrstream_count] = stream;
        if (sortstreams_enabled) {
            for (i = 0; i < rndrstream_count; i++) {
                if (sortkey >= rndrstream_sorted[i]->drawpri) {
                    for (j = rndrstream_count + 1; j > i; j--) {
                        rndrstream_sorted[j] = rndrstream_sorted[j - 1];
                    }
                    rndrstream_sorted[i] = stream;
                    break;
                }
            }
        }
        rndrstream_count++;
        return stream;
    }
    return 0;
}


void NuRndrStreamFlush(void)
{
    struct rndrstream_s *stream;
    struct rndrstream_s *s;
    void *mpg;
    int i;
    int n;

    mpg = 0;
    if ((u8 *)rndrstream_free[0] > frame_base + stream_outbuf_size) {
        NuErrorProlog(nustream_file, 0x324)(stream_overflow_msg);
    }
    for (i = 0; i < rndrstream_count; i++) {
        stream = rndrstream_sorted[i];
        if (stream->base.u8) {
            if (i == 0 && rndrstream_start == 0) {
                mpg = NuRndrStreamPrependMPG(stream, mpg);
                rndrstream_start = stream->base.u8;
            }
            if (rndrstream_end[0]) {
                mpg = NuRndrStreamPrependMPG(stream, mpg);
                rndrstream_end[0]->next = stream->base.dmatag;
                rndrstream_end[0]->id = 0x20;
            }
            rndrstream_end[0] = (struct _sceDmaTag *)stream->ptr.u8;
        }
        if (stream->t_base.u32) {
            if (txtrstream_start[0] == 0) {
                txtrstream_start[0] = stream->t_base.u32;
            }
            if (txtrstream_end[0]) {
                txtrstream_end[0]->next = stream->t_base.dmatag;
                txtrstream_end[0]->id = (txtrstream_end[0]->id & 0x80) | 0x20;
            }
            txtrstream_end[0] = stream->t_ptr.dmatag;
        }
    }
    if (rndrstream_end[0]) {
        rndrstream_end[0]->id = 0x70;
        rndrstream_end[0]->mark = 0xD;
        rndrstream_end[0]->p[0] = 0x13000000;
        rndrstream_end[0]->p[1] = 0;
    }
    if (txtrstream_end[0]) {
        txtrstream_end[0]->id = (txtrstream_end[0]->id & 0x80) | 0x70;
    }
    if (rndrstream_count > 0) {
        s = rndrstream_pool;
        n = rndrstream_count;
        do {
            s->mtl = 0;
            s->base.u8 = 0;
            s->ptr.u8 = 0;
            s->t_base.u32 = 0;
            s->t_ptr.u32 = 0;
            s->local_mtx_id = -2;
            s->view_mtx_id = -2;
            s->scissor_id = -2;
            s->xyoffset_id = -2;
            s->misc_addr.voidptr = 0;
            s->tid = 0;
            s->gs_ptr = gs_top;
            s++;
            n--;
        } while (n != 0);
    }
    rndrstream_rt_light_addr[0] = 0;
    rndrstream_spec_addr = 0;
    rndrstream_fx_microcode = fx_microcode_default;
    rndrstream_fx_mtx_addr[0] = 0;
}


void NuRndrStreamSetMisc(struct rndrstream_s *stream)
{
    u8 *pkt;

    if (stream->scissor_id != rndrstream_scissor_id || stream->xyoffset_id != rndrstream_xyoffset_id) {
        stream->ptr.u8 = vpDmaTag_CntEx(stream->ptr.u8, 0x13000000, 0x50000000) - 4;
        stream->ptr.u8 = vtBeginDirect(stream->ptr.u8, 0);
        stream->ptr.u8 = gsBeginGifPacket(stream->ptr.u8, 0, 1, 0, 0, 0, 1, 0xE);
        *((u64 *)stream->ptr.u8)++ = rndrstream_offset;
        *((u64 *)stream->ptr.u8)++ = 0x18;
        *((u64 *)stream->ptr.u8)++ = rndrstream_scissor;
        *((u64 *)stream->ptr.u8)++ = 0x40;
        stream->ptr.u8 = gsCloseGifPacket(stream->ptr.u8);
        stream->ptr.u8 = vtCloseDirect(stream->ptr.u8);
        stream->ptr.u8 = vpDmaTag_Close(stream->ptr.u8);
        stream->scissor_id = rndrstream_scissor_id;
        stream->xyoffset_id = rndrstream_xyoffset_id;
    }
    switch (stream->attr & 0x43) {
    case 0:
        break;
    case 1:
        if (rt_light_dirty) {
            rt_light_dirty = 0;
            pkt = vpDmaTag_CntEx(&rndrstream_rt_light_buffer, 0, 0x6C07000C);
            ((struct _sceDmaTag *)pkt)[-1].mark = 0x15;
            ((struct _sceDmaTag *)pkt)[-1].qwc = 7;
            pkt = vpDmaTag_RefEx(&rt_light_reftag, 0, 0, 0x6C07000C);
            ((struct _sceDmaTag *)pkt)[-1].mark = 0x15;
            ((struct _sceDmaTag *)pkt)[-1].qwc = 7;
        }
        if (rndrstream_rt_light_addr[0] == 0) {
            rndrstream_rt_light_addr[0] = stream->ptr.u8 + 0x10;
            NuMemCopy128(stream->ptr.u8, &rndrstream_rt_light_buffer, 8);
            stream->ptr.u8 += 0x80;
            rndrstream_rt_light_buffer.ref.tag.next = rndrstream_rt_light_addr[0];
        } else if (stream->misc_addr.voidptr != rndrstream_rt_light_addr[0]) {
            *((NuDmaTagQw *)stream->ptr.u8)++ = rndrstream_rt_light_buffer.ref;
        }
        stream->misc_addr.voidptr = rndrstream_rt_light_addr[0];
        break;
    case 2:
        if (spec_dirty) {
            spec_dirty = 0;
            pkt = vpDmaTag_CntEx(&rndrstream_spec, 0, 0x6C05000C);
            ((struct _sceDmaTag *)pkt)[-1].mark = 0x17;
            ((struct _sceDmaTag *)pkt)[-1].qwc = 5;
            pkt = vpDmaTag_RefEx(&spec_reftag, 0, 0, 0x6C05000C);
            ((struct _sceDmaTag *)pkt)[-1].mark = 0x17;
            ((struct _sceDmaTag *)pkt)[-1].qwc = 5;
        }
        if (rndrstream_spec_addr == 0) {
            rndrstream_spec_addr = stream->ptr.u8 + 0x10;
            NuMemCopy128(stream->ptr.u8, &rndrstream_spec, 6);
            stream->ptr.u8 += 0x60;
            rndrstream_spec.ref.tag.next = rndrstream_spec_addr;
        } else if (stream->misc_addr.voidptr != rndrstream_spec_addr) {
            *((NuDmaTagQw *)stream->ptr.u8)++ = rndrstream_spec.ref;
        }
        stream->misc_addr.voidptr = rndrstream_spec_addr;
        break;
    case 0x40:
        if (fx_mtx_dirty) {
            fx_mtx_dirty = 0;
            pkt = vpDmaTag_CntEx(&rndrstream_fx_mtx, 0, 0x6C04000C);
            ((struct _sceDmaTag *)pkt)[-1].mark = 0x17;
            ((struct _sceDmaTag *)pkt)[-1].qwc = 4;
            pkt = vpDmaTag_RefEx(&fx_mtx_reftag, 0, 0, 0x6C04000C);
            ((struct _sceDmaTag *)pkt)[-1].mark = 0x17;
            ((struct _sceDmaTag *)pkt)[-1].qwc = 4;
        }
        if (rndrstream_fx_mtx_addr[0] == 0) {
            rndrstream_fx_mtx_addr[0] = stream->ptr.u8 + 0x10;
            NuMemCopy128(stream->ptr.u8, &rndrstream_fx_mtx, 5);
            stream->ptr.u8 += 0x50;
            rndrstream_fx_mtx.ref.tag.next = rndrstream_fx_mtx_addr[0];
        } else if (stream->misc_addr.voidptr != rndrstream_fx_mtx_addr[0]) {
            *((NuDmaTagQw *)stream->ptr.u8)++ = rndrstream_fx_mtx.ref;
        }
        stream->misc_addr.voidptr = rndrstream_fx_mtx_addr[0];
        break;
    default:
        NuErrorProlog(nustream_file, 0x45C)(bad_attrib_msg);
        break;
    }
}


int NuRndrStreamSetLocalMtx(struct rndrstream_s *stream)
{
    int done;

    done = 0;
    if (stream->local_mtx_id != rndrstream_local_mtx_id && (stream->attr & 8)) {
        switch (stream->attr & 0x43) {
        case 1:
            local_mtx_pkt.mode = 1;
            break;
        case 2:
        case 0x40:
            local_mtx_pkt.mode = 4;
            break;
        default:
            local_mtx_pkt.mode = 0;
            break;
        }
        rndrstream_last_mtx[0] = stream->ptr.u8;
        NuMemCopy128(stream->ptr.u8, &local_mtx_pkt, local_mtx_pkt_size >> 4);
        stream->ptr.u8 += local_mtx_pkt_size;
        stream->local_mtx_id = rndrstream_local_mtx_id;
        done = 1;
    }
    return done;
}


int NuRndrSetViewMtx(struct numtx_s *vm, struct numtx_s *pm, struct numtx_s *clip_pm)
{
    u8 *pkt;

    view_mtx = *vm;
    if (view_mtx_pkt_dirty) {
        view_mtx_pkt_dirty = 0;
        pkt = vpDmaTag_CntEx(view_mtx_pkt, 0, 0x6C040004);
        if (rndrstream_zfix != 0.0f) {
            NuMtxMulH(((struct numtx_s *)pkt)++, &view_mtx, &zfix_mtx);
        } else {
            NuMemCopy128(((struct numtx_s *)pkt)++, &view_mtx, 4);
        }
        *((u64 *)pkt)++ = 0;
        *((unsigned int *)pkt)++ = 0;
        *((unsigned int *)pkt)++ = 0x6C040008;
        NuMemCopy128(((struct numtx_s *)pkt)++, pm, 4);
        view_mtx_pkt_size = (char *)vpDmaTag_Close(pkt) - view_mtx_pkt;
        pkt = vpDmaTag_CntEx(clip_view_mtx_pkt, 0, 0x6C040004);
        if (rndrstream_zfix != 0.0f) {
            NuMtxMulH(((struct numtx_s *)pkt)++, &view_mtx, &zfix_mtx);
        } else {
            NuMemCopy128(((struct numtx_s *)pkt)++, &view_mtx, 4);
        }
        *((u64 *)pkt)++ = 0;
        *((unsigned int *)pkt)++ = 0;
        *((unsigned int *)pkt)++ = 0x6C040008;
        NuMemCopy128(((struct numtx_s *)pkt)++, clip_pm, 4);
        vpDmaTag_Close(pkt);
    } else {
        if (rndrstream_zfix != 0.0f) {
            NuMtxMulH(&vu1_view_mtx, &view_mtx, &zfix_mtx);
            NuMtxMulH(&vu1_clip_view_mtx, &view_mtx, &zfix_mtx);
        } else {
            NuMemCopy128(&vu1_view_mtx, &view_mtx, 4);
            NuMemCopy128(&vu1_clip_view_mtx, &view_mtx, 4);
        }
        NuMemCopy128(&vu1_proj_mtx, pm, 4);
        NuMemCopy128(&vu1_clip_proj_mtx, clip_pm, 4);
    }
    rndrstream_view_mtx_id = view_mtx_id_next++;
    view_mtx_id_next &= 0x7FFF;
    return 1;
}


int NuRndrSetSpecularLight(struct nuvec_s *dir, struct nucolour4_s *col)
{
    struct nuvec_s v;
    float half;

    if (dir) {
        spec_light_dir = *dir;
    }
    if (col) {
        spec_light_col = *col;
    }
    half = 0.5f;
    NuVecInvMtxRotate(&v, &spec_light_dir, &global_camera.mtx);
    rndrstream_spec.col = spec_light_col;
    if (v.z >= half) {
        rndrstream_spec.col.a *= (1.0f - v.z) * 2.0f;
    }
    NuVecLerp(&v, &v, &spec_view_dir, half);
    NuVecMtxRotate(&v, &v, &global_camera.mtx);
    NuVecNorm(&v, &v);
    rndrstream_spec.mtx = numtx_identity;
    NuMtxAlignZ(&rndrstream_spec.mtx, &v);
    rndrstream_spec.mtx._20 = -rndrstream_spec.mtx._20;
    rndrstream_spec.mtx._21 = -rndrstream_spec.mtx._21;
    rndrstream_spec.mtx._22 = -rndrstream_spec.mtx._22;
    rndrstream_spec.mtx._32 = rndrstream_spec.mtx._03 = rndrstream_spec.mtx._13 = rndrstream_spec.mtx._23 = 0.0f;
    rndrstream_spec.mtx._30 = half;
    rndrstream_spec.mtx._31 = half;
    rndrstream_spec.mtx._33 = 1.0f;
    rndrstream_spec_addr = 0;
    return 1;
}


void *NuStreamGetOutputBuffer(int index)
{
    if (index) {
        return stream_outbuf1;
    }
    return stream_outbuf0;
}


void NuRndrSwapStreamBuffers(void)
{
    rndrstream_buffer_index = (rndrstream_buffer_index + 1) % 2;
    NuRndrInitStreams();
}


void FaceYDirStream(int angle)
{
    struct nuviewport_s *vp;
    union vifword_u *pkt;

    vp = NuVpGetCurrentViewport();
    pkt = vpDmaTag_Cnt(faceon_dir_pkt);
    pkt--;
    pkt->u = 0x6C010016;
    pkt++;
    pkt->f = NuTrigTable[(u16)(angle + 0x4000)];
    pkt++;
    pkt->f = vp->width * 9.765625146e-05f;
    pkt++;
    pkt->f = -NuTrigTable[(u16)angle];
    pkt++;
    pkt->f = vp->height * 0.0002790178696f;
    pkt++;
    vpDmaTag_Close(pkt);
}


int NuRndrSetLocalMtx(struct numtx_s *mtx, int clip)
{
    u8 *pkt;
    unsigned int *code;

    if (local_mtx_pkt_dirty) {
        local_mtx_pkt_dirty = 0;
        pkt = vpDmaTag_CntEx(local_mtx_pkt.tag, 0, 0x6C040000);
        ((struct _sceDmaTag *)pkt)[-1].mark = 0x14;
        NuMemCopy128(pkt, mtx, 4);
        pkt += 0x40;
        *((unsigned int *)pkt)++ = 0x6D010017;
        code = (unsigned int *)(pkt + 8);
        *((short *)pkt)++ = 0;
        code[0] = (clip != 1) ? 0x14000002 : 0x14000000;
        code[1] = 0x17000000;
        local_mtx_pkt_size = (char *)vpDmaTag_Close(code + 2) - local_mtx_pkt.tag;
    } else {
        NuMemCopy128(&local_mtx_pkt.mtx, mtx, 4);
        local_mtx_pkt.mscal = (clip != 1) ? 0x14000002 : 0x14000000;
    }
    local_mtx_clip = clip;
    rndrstream_local_mtx_id = local_mtx_id_next++;
    local_mtx_id_next &= 0x7FFF;
    return 1;
}


void NuRndrSetXYOffset(int x, int y)
{
    rndrstream_offset = (u64)x | ((u64)y << 32);
    rndrstream_xyoffset_id = (rndrstream_xyoffset_id + 1) & 0x7FFF;
}


void NuRndrSetScissor(int x0, int y0, int x1, int y1)
{
    rndrstream_scissor = (u64)x0 | ((u64)x1 << 16) | ((u64)y0 << 32) | ((u64)y1 << 48);
    rndrstream_scissor_id = (rndrstream_scissor_id + 1) & 0x7FFF;
}


int NuRndrSetZFix(int zfix)
{
    float z;

    z = zfix;
    if (z != rndrstream_zfix) {
        rndrstream_zfix = z;
        zfix_mtx._32 = z;
        NuMtxMulH(&vu1_view_mtx, &view_mtx, &zfix_mtx);
        NuMtxMulH(&vu1_clip_view_mtx, &view_mtx, &zfix_mtx);
        rndrstream_view_mtx_id = view_mtx_id_next++;
        view_mtx_id_next &= 0x7FFF;
    }
    return 1;
}


int NuRndrSetDirectionalLights(struct nuvec_s *d0, struct nucolour3_s *c0,
                               struct nuvec_s *d1, struct nucolour3_s *c1,
                               struct nuvec_s *d2, struct nucolour3_s *c2)
{
    rndrstream_rt_light_buffer.dirx[0] = d0->x;
    rndrstream_rt_light_buffer.dirx[1] = d1->x;
    rndrstream_rt_light_buffer.dirx[2] = d2->x;
    rndrstream_rt_light_buffer.diry[0] = d0->y;
    rndrstream_rt_light_buffer.diry[1] = d1->y;
    rndrstream_rt_light_buffer.diry[2] = d2->y;
    rndrstream_rt_light_buffer.dirz[0] = d0->z;
    rndrstream_rt_light_buffer.dirz[1] = d1->z;
    rndrstream_rt_light_buffer.dirz[2] = d2->z;
    rndrstream_rt_light_buffer.col[0].rgb = *c0;
    rndrstream_rt_light_buffer.col[1].rgb = *c1;
    rndrstream_rt_light_buffer.col[2].rgb = *c2;
    rndrstream_rt_light_buffer.col[2].a = 1.0f;
    rndrstream_rt_light_buffer.col[1].a = 0.0f;
    rndrstream_rt_light_buffer.col[0].a = 0.0f;
    rndrstream_rt_light_addr[0] = 0;
    return 1;
}


int NuRndrSetDirectionalLights4(struct nuvec_s *d0, struct nucolour4_s *c0,
                                struct nuvec_s *d1, struct nucolour4_s *c1,
                                struct nuvec_s *d2, struct nucolour4_s *c2)
{
    rndrstream_rt_light_buffer.dirx[0] = d0->x;
    rndrstream_rt_light_buffer.dirx[1] = d1->x;
    rndrstream_rt_light_buffer.dirx[2] = d2->x;
    rndrstream_rt_light_buffer.diry[0] = d0->y;
    rndrstream_rt_light_buffer.diry[1] = d1->y;
    rndrstream_rt_light_buffer.diry[2] = d2->y;
    rndrstream_rt_light_buffer.dirz[0] = d0->z;
    rndrstream_rt_light_buffer.dirz[1] = d1->z;
    rndrstream_rt_light_buffer.dirz[2] = d2->z;
    rndrstream_rt_light_buffer.col[0] = *c0;
    rndrstream_rt_light_buffer.col[1] = *c1;
    rndrstream_rt_light_buffer.col[2] = *c2;
    rndrstream_rt_light_addr[0] = 0;
    return 1;
}


int NuRndrSetAmbientLight(struct nucolour3_s *colour)
{
    rndrstream_rt_light_buffer.r = colour->r;
    rndrstream_rt_light_buffer.g = colour->g;
    rndrstream_rt_light_buffer.b = colour->b;
    rndrstream_rt_light_buffer.a = 1.0f;
    rndrstream_rt_light_addr[0] = 0;
    return 1;
}


int NuRndrSetGlobalAlpha(float alpha)
{
    return 1;
}


int NuRndrSetFxMtx(struct numtx_s *mtx)
{
    rndrstream_fx_mtx.mtx = *mtx;
    rndrstream_fx_mtx_addr[0] = 0;
    return 1;
}


u64 *NuRndrStreamGetScissor(void)
{
    return &rndrstream_scissor;
}


u64 *NuRndrStreamGetOffset(void)
{
    return &rndrstream_offset;
}


void NuRndrStreamResetStream(struct rndrstream_s *stream, int flag)
{
    stream->base.u8 = 0;
    stream->ptr.u8 = 0;
    stream->t_base.u32 = 0;
    stream->t_ptr.u32 = 0;
    if (flag) {
        stream->gs_ptr = gs_top;
        stream->local_mtx_id = -2;
        stream->view_mtx_id = -2;
        stream->scissor_id = -2;
        stream->misc_addr.voidptr = 0;
        stream->tid = 0;
        stream->xyoffset_id = -2;
    }
}


void NuRndrStreamResetStreams(int flag)
{
    struct rndrstream_s *s;
    int i;

    for (i = 0; i < rndrstream_count; i++) {
        s = &rndrstream_pool[i];
        s->mtl = 0;
        s->base.u8 = 0;
        s->ptr.u8 = 0;
        s->t_base.u32 = 0;
        s->t_ptr.u32 = 0;
        if (flag) {
            s->local_mtx_id = -2;
            s->view_mtx_id = -2;
            s->scissor_id = -2;
            s->xyoffset_id = -2;
            s->misc_addr.voidptr = 0;
            s->tid = 0;
            s->gs_ptr = gs_top;
        }
    }
    rndrstream_rt_light_addr[0] = 0;
    rndrstream_spec_addr = 0;
    rndrstream_fx_microcode = fx_microcode_default;
    rndrstream_fx_mtx_addr[0] = 0;
}


void NuRndrVu1Draw(void)
{
    __asm__ volatile ("sync");
    FlushCache(0);
    if (txtrstream_start[0] != 0) {
        GS_DmaWrite(txtrstream_start[0], 1);
    }
    VU1_DmaWrite(rndrstream_start, 1);
    tbslotReset(0, 1);
    tbslotReset(0, 0);
    tbslotBeginFn(0, 1);
    tbslotBeginFn(0, 0);
}


void NuRndrStreamFxMPG(int mode)
{
    void *microcode;

    if (mode == 0) {
        microcode = D_00289020;
    } else {
        microcode = (mode != 1) ? D_00289020 : D_0028CFF0;
    }
    if (rndrstream_fx_microcode != microcode) {
        rndrstream_fx->ptr.u8 = vpDmaTag_Call(rndrstream_fx->ptr.u8, microcode);
        rndrstream_fx_microcode = microcode;
    }
}


void *NuRndrStreamAlloc(int qwords)
{
    char *mem;

    mem = rndrstream_free[0];
    rndrstream_free[0] = mem + qwords * 16;
    return mem;
}


int NuRndrStreamAddMtl(struct rndrstream_s *stream, struct numtl_s *mtl)
{
    if (!NuMtlValid(mtl)) {
        return 0;
    }
    if (mtl != 0 && mtl != stream->mtl && (stream->attr & 0x10)) {
        if (stream->tid != mtl->tid) {
            stream->tid = mtl->tid;
            NuTexAccomodateRS(stream, mtl->tid);
            stream->ptr.u8 = NuTexSet(stream->ptr.u8, mtl->tid, mtl->context);
        }
        stream->ptr.u8 = vpDmaTag_Call(stream->ptr.u8, NuMtlGetStateBlockDma(mtl));
        stream->ptr.u8 = NuRndrStreamAddZBState(stream->ptr.u8, NuMtlGetZupd(mtl));
    }
    stream->mtl = mtl;
    return 1;
}


u64 NuRndrStreamAddTex(struct rndrstream_s *stream, int tid, signed char mode)
{
    NuTexAccomodateRS(stream, tid);
    stream->ptr.u8 = NuTexSet(stream->ptr.u8, tid, mode);
    return NuTexGetTex0(tid);
}


int NuRndrStreamSetDirectionalLights(struct rndrstream_s *stream,
                                     struct nuvec_s *d0, struct nucolour3_s *c0,
                                     struct nuvec_s *d1, struct nucolour3_s *c1,
                                     struct nuvec_s *d2, struct nucolour3_s *c2)
{
    return 0;
}


int NuRndrStreamSetAmbientLight(struct rndrstream_s *stream,
                                struct nucolour3_s *colour)
{
    return 0;
}


int NuRndrStreamSetViewMtx(struct rndrstream_s *stream)
{
    if (stream->view_mtx_id == rndrstream_view_mtx_id) {
        return 0;
    }
    if ((stream->attr & 4) && view_mtx_pkt_size != 1) {
        NuMemCopy128(stream->ptr.u8, view_mtx_pkt, view_mtx_pkt_size >> 4);
        stream->ptr.u8 += view_mtx_pkt_size;
        NuMemCopy128(stream->ptr.u8, faceon_dir_pkt, 2);
        stream->ptr.u8 += 0x20;
        stream->view_mtx_id = rndrstream_view_mtx_id;
        return 1;
    }
    return 0;
}


int NuRndrStreamSetViewMtxScissorClip(struct rndrstream_s *stream)
{
    if (!(stream->attr & 4)) {
        return 0;
    }
    if (view_mtx_pkt_size == 1) {
        return 0;
    }
    NuMemCopy128(stream->ptr.u8, clip_view_mtx_pkt, view_mtx_pkt_size >> 4);
    stream->view_mtx_id = -2;
    stream->ptr.u8 += view_mtx_pkt_size;
    return 1;
}


int NuRndrStreamSetLocalMtxEx(struct rndrstream_s *stream, int clip)
{
    if (stream->local_mtx_id != rndrstream_local_mtx_id && (stream->attr & 8)) {
        switch (stream->attr & 0x43) {
        case 1:
            local_mtx_pkt.mode = 1;
            break;
        case 2:
        case 0x40:
            local_mtx_pkt.mode = 4;
            break;
        default:
            local_mtx_pkt.mode = 0;
            break;
        }
        rndrstream_last_mtx[0] = stream->ptr.u8;
        NuMemCopy128(stream->ptr.u8, &local_mtx_pkt, local_mtx_pkt_size >> 4);
        stream->ptr.u8 += local_mtx_pkt_size;
        stream->local_mtx_id = rndrstream_local_mtx_id;
        if (clip) {
            if (testfclip) {
                stream->ptr.u8 = vpDmaTag_CntEx(stream->ptr.u8, 0x1400000E, 0x17000000);
            } else {
                stream->ptr.u8 = vpDmaTag_CntEx(stream->ptr.u8, 0x1400000A, 0x17000000);
            }
        }
        return 1;
    }
    return 0;
}


void *NuRndrStreamPrependMPG(struct rndrstream_s *stream, void *tag)
{
    void *cur;
    void *old_free;

    cur = stream->mpg;
    if (cur == 0 || cur == tag) {
        return tag;
    }
    old_free = rndrstream_free[0];
    rndrstream_free[0] = vpDmaTag_Call(rndrstream_free[0], cur);
    rndrstream_free[0] = vpDmaTag_Next(rndrstream_free[0], stream->base.u8);
    stream->base.u8 = old_free;
    return stream->mpg;
}


void NuRndrStreamClearZBState(void)
{
    zbstate_set = 0;
}


void NuRndrStreamSetZBState(unsigned int zbuf, unsigned int psm, int zmask)
{
    u8 *pkt;
    u64 zbp;
    u64 format;
    u64 mask;

    pkt = vpDmaTag_CntEx(&zbstate_pkt, 0x11000000, 0x50000000);
    pkt[-14] = 0xC;
    pkt = vtBeginDirect(pkt - 4, 0);
    pkt = gsBeginGifPacket(pkt, 0, 1, 0, 0, 0, 1, 0xE);
    zbp = zbuf >> 5;
    format = psm;
    mask = zmask;
    *(u64 *)pkt = zbp | format << 24 | mask << 32;
    pkt += 8;
    *(u64 *)pkt = 0x4E;
    pkt += 8;
    pkt = gsCloseGifPacket(pkt);
    pkt = vtCloseDirect(pkt);
    vpDmaTag_Close(pkt);
    zbstate_set = 1;
}


void *NuRndrStreamAddZBState(void *stream, int index)
{
    if (zbstate_set) {
        zbstate_pkt.zmask = zmask_table[index];
        NuMemCopy128(stream, &zbstate_pkt, 3);
        stream = (u128 *)stream + 3;
    }
    return stream;
}


void CheckRndrStream(void)
{
    int i;

    for (i = 0; i < stream_outbuf_size; i++) {
        if (stream_outbuf0[i]) {
            NuDebugMsgProlog(nustream_file, 0x238)(trashed_stream_a_msg);
        }
        if (stream_outbuf1[i]) {
            NuDebugMsgProlog(nustream_file, 0x23B)(trashed_stream_b_msg);
        }
    }
}
