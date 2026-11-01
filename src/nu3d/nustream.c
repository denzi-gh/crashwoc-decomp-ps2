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
    struct _sceDmaTag tag;
    u128 qw;
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
    char pad_0x28[8];
};

struct nuviewmtxpkt_s {
    NuDmaTagQw cnt;
    struct numtx_s view;
    NuDmaTagQw unpack;
    struct numtx_s proj;
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


static int nurndr_maxstreamsize = 0;
static int rndrstream_buffid = 0;
static float rndrstream_oldzfix = 0.0f;
short rndrstream_view_mtx_id = -1;
static int rndrstream_local_mtx_buffer_size = 1;
static int rndrstream_view_mtx_buffer_size = 1;
int rndrstream_local_mtx_id = -1;
short rndrstream_scissor_id = -1;
short rndrstream_xyoffset_id = -1;
static int stream_start_offset = 0;
static int stream_start_automove = 0;
static int stream_start_automove_cnt = 0;
static int stream_start_automove_range = 0x400;
int sortstreams_enabled = 1;

static struct nuvec_s rndrstream_specular_dir = { 0.0f, 0.0f, 1.0f };
static struct nucolour4_s rndrstream_specular_intensity = { { 1.0f, 1.0f, 1.0f }, 0.0f };

extern struct _sceDmaTag *rndrstream_end[1];
extern void *rndrstream_start[1];
extern struct rndrstream_s *rndrstream_system;
extern struct rndrstream_s *rndrstream_mtl_sort;
extern struct rndrstream_s *rndrstream_faceon;
extern struct rndrstream_s *rndrstream_2d;
extern struct rndrstream_s *rndrstream_3d_u;
extern struct rndrstream_s *rndrstream_3d_l;
extern struct rndrstream_s *rndrstream_ptl;
extern struct rndrstream_s *rndrstream_fx;
extern struct rndrstream_s *rndrstream_ot_sort;
extern char *rndrstream_free[1];
extern struct _sceDmaTag *txtrstream_end[1];
extern void *txtrstream_start[1];
extern u8 *frame_base[1];
extern void *rndrstream_fx_microcode;
extern void *rndrstream_rt_light_addr[1];
extern void *rndrstream_fx_mtx_addr[1];
extern void *rndrstream_last_mtx[1];
extern int testfclip;
extern struct nurtlight_s rndrstream_rt_light_buffer;

static unsigned char *rndrstream_a;
static unsigned char *rndrstream_b;
static int numstreams;
static int rndrstream_clip_flag;
static void *rndrstream_specular_addr[1];
static u64 rndrstream_scissor[1];
static u64 rndrstream_xyoffset[1];

static struct rndrstream_s rndrstreams[32];
static struct rndrstream_s *sortstreams[32];
static struct numtx_s rndrstream_zfix_mtx;
static struct numtx_s rndrstream_view_nozfix_mtx;
static struct nulocalmtxpkt_s rndrstream_local_mtx_buffer;
static struct nuviewmtxpkt_s rndrstream_view_mtx_buffer_vpc __attribute__((aligned(64)));
static struct nuviewmtxpkt_s rndrstream_view_mtx_buffer_scc;
static u128 rndrstream_facey_buffer[2];
static struct nuspecpkt_s rndrstream_specular_buffer;
static struct nurndrfxmtx_s rndrstream_fx_mtx_buffer;
static struct nuzbstate_s zbstatebuff __attribute__((aligned(64)));

extern int gs_top;
extern char vu_load_fx2_mpg[];
extern char vu_load_fx_mpg[];
extern char vu_load_tric_mpg[];
extern char vu_load_tricu_mpg[];
extern char vu_load_faceon_mpg[];
extern char vu_load_ptl_mpg[];
extern float NuTrigTable[];
extern struct nucamera_s global_camera;
extern struct numtx_s numtx_identity;

extern void *vpDmaTag_Call(void *cur, void *microcode);
extern void *vpDmaTag_Next(void *cur, void *mode);
extern void *vpDmaTag_Cnt(void *packet);
extern void *vpDmaTag_Close(void *packet);
extern u8 *vpDmaTag_CntEx(void *packet, unsigned int vif0, unsigned int vif1);
extern u8 *vpDmaTag_RefEx(void *packet, void *addr, int qwc, unsigned int vif1);
extern void *vtBeginDirect(void *packet, unsigned int irq);
extern void *vtCloseDirect(void *packet);
extern void *gsBeginGifPacket(void *packet, int nloop, int eop, int pre,
                              int prim, int flg, int nreg, ...);
extern void *gsCloseGifPacket(void *packet);
extern void NuMemCopy128(void *dst, void *src, int count);
extern void *NuMemAllocFn(int size, char *file, int line);
extern void FlushCache(int mode);
extern void GS_DmaWrite(void *p, int mode);
extern void VU1_DmaWrite(void *p, int mode);
extern void tbslotReset(int a, int b);
extern void tbslotBeginFn(int a, int b);
extern void NuRndrStreamLink(struct rndrstream_s *stream);

#define nustream_file "..\\nu2crash.ps2\\nu3d\\nustream.c"


void NuRndrInitStreams(void)
{
    int wrap;
    struct rndrstream_s *sys;

    if (stream_start_automove) {
        if (stream_start_automove_cnt == 0) {
            stream_start_automove_cnt = 10;
            wrap = stream_start_offset / stream_start_automove_range;
            stream_start_offset++;
        }
        stream_start_automove_cnt--;
    }
    switch (rndrstream_buffid) {
    case 0:
        rndrstream_free[0] = rndrstream_a + stream_start_offset;
        break;
    case 1:
        rndrstream_free[0] = rndrstream_b + stream_start_offset;
        break;
    case 2:
        break;
    }
    if ((unsigned int)rndrstream_free[0] & 0xF) {
        rndrstream_free[0] = (char *)(((unsigned int)rndrstream_free[0] + 0xF) & ~0xF);
    }
    rndrstream_start[0] = 0;
    frame_base[0] = rndrstream_free[0];
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
    NuMtxSetIdentity(&rndrstream_zfix_mtx);
}


void NuRndrStreamInit(int size)
{
    numstreams = 0;
    rndrstream_a = NuMemAllocFn(size, nustream_file, 0x207);
    rndrstream_b = NuMemAllocFn(size, nustream_file, 0x208);
    nurndr_maxstreamsize = size;
    rndrstream_system = NuRndrStreamAllocStream(0x7FFFFFFF, 0, 0);
    rndrstream_mtl_sort = NuRndrStreamAllocStream(0x1000000, 0, 0x3D);
    rndrstream_faceon = NuRndrStreamAllocStream(0x1000000, vu_load_faceon_mpg, 0x3C);
    rndrstream_3d_u = NuRndrStreamAllocStream(0x10000000, vu_load_tricu_mpg, 0x3C);
    rndrstream_3d_l = NuRndrStreamAllocStream(0x10000000, vu_load_tric_mpg, 0x3D);
    rndrstream_2d = NuRndrStreamAllocStream(0x100000, 0, 0x10);
    rndrstream_ptl = NuRndrStreamAllocStream(0xFFFFFF, vu_load_ptl_mpg, 0x3C);
    rndrstream_fx = NuRndrStreamAllocStream(0xFFFFFF, vu_load_fx_mpg, 0x3C);
    rndrstream_ot_sort = NuRndrStreamAllocStream(0xFFFFFF, 0, 0x3D);
    rndrstream_buffid = 0;
    memset(rndrstream_a, 0, size);
    memset(rndrstream_b, 0, size);
    rndrstream_fx_microcode = vu_load_fx_mpg;
    NuRndrInitStreams();
}


/* Non-static inline functions: the code goes to the end of the unit, their
   strings and statics stay where they are defined (retail data order). */
inline void CheckRndrStream(void)
{
    int i;

    for (i = 0; i < nurndr_maxstreamsize; i++) {
        if (rndrstream_a[i]) {
            NuDebugMsgProlog(nustream_file, 0x238)("trashed stream a");
        }
        if (rndrstream_b[i]) {
            NuDebugMsgProlog(nustream_file, 0x23B)("trashed stream b");
        }
    }
}


struct rndrstream_s *NuRndrStreamAllocStream(unsigned int sortkey, void *mpgtag, unsigned int flags)
{
    struct rndrstream_s *stream;
    int i;
    int j;

    if (numstreams < 32) {
        stream = &rndrstreams[numstreams];
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
        sortstreams[numstreams] = stream;
        if (sortstreams_enabled) {
            for (i = 0; i < numstreams; i++) {
                if (sortkey >= sortstreams[i]->drawpri) {
                    for (j = numstreams + 1; j > i; j--) {
                        sortstreams[j] = sortstreams[j - 1];
                    }
                    sortstreams[i] = stream;
                    break;
                }
            }
        }
        numstreams++;
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
    if ((u8 *)rndrstream_free[0] > frame_base[0] + nurndr_maxstreamsize) {
        NuErrorProlog(nustream_file, 0x324)("Stream buffer overflow - crash soon");
    }
    for (i = 0; i < numstreams; i++) {
        stream = sortstreams[i];
        if (stream->base.u8) {
            if (i == 0 && rndrstream_start[0] == 0) {
                mpg = NuRndrStreamPrependMPG(stream, mpg);
                rndrstream_start[0] = stream->base.u8;
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
    if (numstreams > 0) {
        s = rndrstreams;
        n = numstreams;
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
    rndrstream_specular_addr[0] = 0;
    rndrstream_fx_microcode = vu_load_fx_mpg;
    rndrstream_fx_mtx_addr[0] = 0;
}


void NuRndrStreamSetMisc(struct rndrstream_s *stream)
{
    static int light_first_time = 1;
    static int spec_first_time = 1;
    static int fx_first_time = 1;
    u8 *pkt;

    if (stream->scissor_id != rndrstream_scissor_id || stream->xyoffset_id != rndrstream_xyoffset_id) {
        stream->ptr.u8 = vpDmaTag_CntEx(stream->ptr.u8, 0x13000000, 0x50000000) - 4;
        stream->ptr.u8 = vtBeginDirect(stream->ptr.u8, 0);
        stream->ptr.u8 = gsBeginGifPacket(stream->ptr.u8, 0, 1, 0, 0, 0, 1, 0xE);
        *((u64 *)stream->ptr.u8)++ = rndrstream_xyoffset[0];
        *((u64 *)stream->ptr.u8)++ = 0x18;
        *((u64 *)stream->ptr.u8)++ = rndrstream_scissor[0];
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
        if (light_first_time) {
            light_first_time = 0;
            pkt = (u8 *)&rndrstream_rt_light_buffer;
            pkt = vpDmaTag_CntEx(pkt, 0, 0x6C07000C);
            ((struct _sceDmaTag *)pkt)[-1].mark = 0x15;
            ((struct _sceDmaTag *)pkt)[-1].qwc = 7;
            pkt = vpDmaTag_RefEx(&rndrstream_rt_light_buffer.ref, 0, 0, 0x6C07000C);
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
        if (spec_first_time) {
            spec_first_time = 0;
            pkt = (u8 *)&rndrstream_specular_buffer;
            pkt = vpDmaTag_CntEx(pkt, 0, 0x6C05000C);
            ((struct _sceDmaTag *)pkt)[-1].mark = 0x17;
            ((struct _sceDmaTag *)pkt)[-1].qwc = 5;
            pkt = vpDmaTag_RefEx(&rndrstream_specular_buffer.ref, 0, 0, 0x6C05000C);
            ((struct _sceDmaTag *)pkt)[-1].mark = 0x17;
            ((struct _sceDmaTag *)pkt)[-1].qwc = 5;
        }
        if (rndrstream_specular_addr[0] == 0) {
            rndrstream_specular_addr[0] = stream->ptr.u8 + 0x10;
            NuMemCopy128(stream->ptr.u8, &rndrstream_specular_buffer, 6);
            stream->ptr.u8 += 0x60;
            rndrstream_specular_buffer.ref.tag.next = rndrstream_specular_addr[0];
        } else if (stream->misc_addr.voidptr != rndrstream_specular_addr[0]) {
            *((NuDmaTagQw *)stream->ptr.u8)++ = rndrstream_specular_buffer.ref;
        }
        stream->misc_addr.voidptr = rndrstream_specular_addr[0];
        break;
    case 0x40:
        if (fx_first_time) {
            fx_first_time = 0;
            pkt = (u8 *)&rndrstream_fx_mtx_buffer;
            pkt = vpDmaTag_CntEx(pkt, 0, 0x6C04000C);
            ((struct _sceDmaTag *)pkt)[-1].mark = 0x17;
            ((struct _sceDmaTag *)pkt)[-1].qwc = 4;
            pkt = vpDmaTag_RefEx(&rndrstream_fx_mtx_buffer.ref, 0, 0, 0x6C04000C);
            ((struct _sceDmaTag *)pkt)[-1].mark = 0x17;
            ((struct _sceDmaTag *)pkt)[-1].qwc = 4;
        }
        if (rndrstream_fx_mtx_addr[0] == 0) {
            rndrstream_fx_mtx_addr[0] = stream->ptr.u8 + 0x10;
            NuMemCopy128(stream->ptr.u8, &rndrstream_fx_mtx_buffer, 5);
            stream->ptr.u8 += 0x50;
            rndrstream_fx_mtx_buffer.ref.tag.next = rndrstream_fx_mtx_addr[0];
        } else if (stream->misc_addr.voidptr != rndrstream_fx_mtx_addr[0]) {
            *((NuDmaTagQw *)stream->ptr.u8)++ = rndrstream_fx_mtx_buffer.ref;
        }
        stream->misc_addr.voidptr = rndrstream_fx_mtx_addr[0];
        break;
    default:
        NuErrorProlog(nustream_file, 0x45C)("Invalid attribute combination in stream");
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
            rndrstream_local_mtx_buffer.mode = 1;
            break;
        case 2:
        case 0x40:
            rndrstream_local_mtx_buffer.mode = 4;
            break;
        default:
            rndrstream_local_mtx_buffer.mode = 0;
            break;
        }
        rndrstream_last_mtx[0] = stream->ptr.u8;
        NuMemCopy128(stream->ptr.u8, &rndrstream_local_mtx_buffer, rndrstream_local_mtx_buffer_size >> 4);
        stream->ptr.u8 += rndrstream_local_mtx_buffer_size;
        stream->local_mtx_id = rndrstream_local_mtx_id;
        done = 1;
    }
    return done;
}


void *NuStreamGetOutputBuffer(int index)
{
    if (index) {
        return rndrstream_b;
    }
    return rndrstream_a;
}


void NuRndrSwapStreamBuffers(void)
{
    rndrstream_buffid = (rndrstream_buffid + 1) % 2;
    NuRndrInitStreams();
}


void FaceYDirStream(int angle)
{
    struct nuviewport_s *vp;
    union vifword_u *pkt;

    vp = NuVpGetCurrentViewport();
    pkt = vpDmaTag_Cnt(rndrstream_facey_buffer);
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
    static int localmtx_id = 0;
    static int firsttime = 1;
    u8 *pkt;
    unsigned int *code;

    if (firsttime) {
        firsttime = 0;
        pkt = (u8 *)&rndrstream_local_mtx_buffer;
        pkt = vpDmaTag_CntEx(pkt, 0, 0x6C040000);
        ((struct _sceDmaTag *)pkt)[-1].mark = 0x14;
        NuMemCopy128(pkt, mtx, 4);
        pkt += 0x40;
        *((unsigned int *)pkt)++ = 0x6D010017;
        *(short *)pkt = 0;
        code = (unsigned int *)(pkt + 8);
        pkt += 8;
        code[0] = (clip != 1) ? 0x14000002 : 0x14000000;
        code[1] = 0x17000000;
        rndrstream_local_mtx_buffer_size = (char *)vpDmaTag_Close(code + 2) - (char *)&rndrstream_local_mtx_buffer;
    } else {
        NuMemCopy128(&rndrstream_local_mtx_buffer.mtx, mtx, 4);
        rndrstream_local_mtx_buffer.mscal = (clip != 1) ? 0x14000002 : 0x14000000;
    }
    rndrstream_clip_flag = clip;
    rndrstream_local_mtx_id = localmtx_id++;
    localmtx_id &= 0x7FFF;
    return 1;
}


static int viewmtx_id = 0;

inline int NuRndrSetViewMtx(struct numtx_s *vm, struct numtx_s *pm, struct numtx_s *clip_pm)
{
    static int firsttime = 1;
    u8 *pkt;

    rndrstream_view_nozfix_mtx = *vm;
    if (firsttime) {
        firsttime = 0;
        pkt = vpDmaTag_CntEx(&rndrstream_view_mtx_buffer_vpc, 0, 0x6C040004);
        if (rndrstream_oldzfix != 0.0f) {
            NuMtxMulH(((struct numtx_s *)pkt)++, &rndrstream_view_nozfix_mtx, &rndrstream_zfix_mtx);
        } else {
            NuMemCopy128(((struct numtx_s *)pkt)++, &rndrstream_view_nozfix_mtx, 4);
        }
        *((u64 *)pkt)++ = 0;
        *((unsigned int *)pkt)++ = 0;
        *((unsigned int *)pkt)++ = 0x6C040008;
        NuMemCopy128(((struct numtx_s *)pkt)++, pm, 4);
        rndrstream_view_mtx_buffer_size = (char *)vpDmaTag_Close(pkt) - (char *)&rndrstream_view_mtx_buffer_vpc;
        pkt = vpDmaTag_CntEx(&rndrstream_view_mtx_buffer_scc, 0, 0x6C040004);
        if (rndrstream_oldzfix != 0.0f) {
            NuMtxMulH(((struct numtx_s *)pkt)++, &rndrstream_view_nozfix_mtx, &rndrstream_zfix_mtx);
        } else {
            NuMemCopy128(((struct numtx_s *)pkt)++, &rndrstream_view_nozfix_mtx, 4);
        }
        *((u64 *)pkt)++ = 0;
        *((unsigned int *)pkt)++ = 0;
        *((unsigned int *)pkt)++ = 0x6C040008;
        NuMemCopy128(((struct numtx_s *)pkt)++, clip_pm, 4);
        vpDmaTag_Close(pkt);
    } else {
        if (rndrstream_oldzfix != 0.0f) {
            NuMtxMulH(&rndrstream_view_mtx_buffer_vpc.view, &rndrstream_view_nozfix_mtx, &rndrstream_zfix_mtx);
            NuMtxMulH(&rndrstream_view_mtx_buffer_scc.view, &rndrstream_view_nozfix_mtx, &rndrstream_zfix_mtx);
        } else {
            NuMemCopy128(&rndrstream_view_mtx_buffer_vpc.view, &rndrstream_view_nozfix_mtx, 4);
            NuMemCopy128(&rndrstream_view_mtx_buffer_scc.view, &rndrstream_view_nozfix_mtx, 4);
        }
        NuMemCopy128(&rndrstream_view_mtx_buffer_vpc.proj, pm, 4);
        NuMemCopy128(&rndrstream_view_mtx_buffer_scc.proj, clip_pm, 4);
    }
    rndrstream_view_mtx_id = viewmtx_id++;
    viewmtx_id &= 0x7FFF;
    return 1;
}

void NuRndrSetXYOffset(int x, int y)
{
    rndrstream_xyoffset[0] = (u64)x | ((u64)y << 32);
    rndrstream_xyoffset_id = (rndrstream_xyoffset_id + 1) & 0x7FFF;
}


void NuRndrSetScissor(int x0, int y0, int x1, int y1)
{
    rndrstream_scissor[0] = (u64)x0 | ((u64)x1 << 16) | ((u64)y0 << 32) | ((u64)y1 << 48);
    rndrstream_scissor_id = (rndrstream_scissor_id + 1) & 0x7FFF;
}


int NuRndrSetZFix(int zfix)
{
    float z;

    z = zfix;
    if (z != rndrstream_oldzfix) {
        rndrstream_oldzfix = z;
        rndrstream_zfix_mtx._32 = z;
        NuMtxMulH(&rndrstream_view_mtx_buffer_vpc.view, &rndrstream_view_nozfix_mtx, &rndrstream_zfix_mtx);
        NuMtxMulH(&rndrstream_view_mtx_buffer_scc.view, &rndrstream_view_nozfix_mtx, &rndrstream_zfix_mtx);
        rndrstream_view_mtx_id = viewmtx_id++;
        viewmtx_id &= 0x7FFF;
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
    rndrstream_fx_mtx_buffer.mtx = *mtx;
    rndrstream_fx_mtx_addr[0] = 0;
    return 1;
}


u64 *NuRndrStreamGetScissor(void)
{
    return rndrstream_scissor;
}


u64 *NuRndrStreamGetOffset(void)
{
    return rndrstream_xyoffset;
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

    for (i = 0; i < numstreams; i++) {
        s = &rndrstreams[i];
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
    rndrstream_specular_addr[0] = 0;
    rndrstream_fx_microcode = vu_load_fx_mpg;
    rndrstream_fx_mtx_addr[0] = 0;
}


void NuRndrVu1Draw(void)
{
    __asm__ volatile ("sync");
    FlushCache(0);
    if (txtrstream_start[0] != 0) {
        GS_DmaWrite(txtrstream_start[0], 1);
    }
    VU1_DmaWrite(rndrstream_start[0], 1);
    tbslotReset(0, 1);
    tbslotReset(0, 0);
    tbslotBeginFn(0, 1);
    tbslotBeginFn(0, 0);
}


void NuRndrStreamFxMPG(int mode)
{
    void *microcode;

    if (mode == 0) {
        microcode = vu_load_fx_mpg;
    } else {
        microcode = (mode != 1) ? vu_load_fx_mpg : vu_load_fx2_mpg;
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
    if ((stream->attr & 4) && rndrstream_view_mtx_buffer_size != 1) {
        NuMemCopy128(stream->ptr.u8, &rndrstream_view_mtx_buffer_vpc, rndrstream_view_mtx_buffer_size >> 4);
        stream->ptr.u8 += rndrstream_view_mtx_buffer_size;
        NuMemCopy128(stream->ptr.u8, rndrstream_facey_buffer, 2);
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
    if (rndrstream_view_mtx_buffer_size == 1) {
        return 0;
    }
    NuMemCopy128(stream->ptr.u8, &rndrstream_view_mtx_buffer_scc, rndrstream_view_mtx_buffer_size >> 4);
    stream->view_mtx_id = -2;
    stream->ptr.u8 += rndrstream_view_mtx_buffer_size;
    return 1;
}


int NuRndrStreamSetLocalMtxEx(struct rndrstream_s *stream, int clip)
{
    if (stream->local_mtx_id != rndrstream_local_mtx_id && (stream->attr & 8)) {
        switch (stream->attr & 0x43) {
        case 1:
            rndrstream_local_mtx_buffer.mode = 1;
            break;
        case 2:
        case 0x40:
            rndrstream_local_mtx_buffer.mode = 4;
            break;
        default:
            rndrstream_local_mtx_buffer.mode = 0;
            break;
        }
        rndrstream_last_mtx[0] = stream->ptr.u8;
        NuMemCopy128(stream->ptr.u8, &rndrstream_local_mtx_buffer, rndrstream_local_mtx_buffer_size >> 4);
        stream->ptr.u8 += rndrstream_local_mtx_buffer_size;
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


static int zbstate_set = 0;

struct _sceDmaTag *rndrstream_end[1] = { 0 };
void *rndrstream_start[1] = { 0 };
struct rndrstream_s *rndrstream_3d = 0;
struct rndrstream_s *rndrstream_system = 0;
struct rndrstream_s *rndrstream_mtl_sort = 0;
struct rndrstream_s *rndrstream_faceon = 0;
struct rndrstream_s *rndrstream_2d = 0;
struct rndrstream_s *rndrstream_3d_u = 0;
struct rndrstream_s *rndrstream_3d_l = 0;
struct rndrstream_s *rndrstream_ptl = 0;
struct rndrstream_s *rndrstream_fx = 0;
struct rndrstream_s *rndrstream_ot_sort = 0;
char *rndrstream_free[1] = { 0 };
struct _sceDmaTag *txtrstream_end[1] = { 0 };
void *txtrstream_start[1] = { 0 };
u8 *frame_base[1] = { 0 };
void *rndrstream_fx_microcode = 0;
void *rndrstream_rt_light_addr[1] = { 0 };
void *rndrstream_fx_mtx_addr[1] = { 0 };
void *rndrstream_last_mtx[1] = { 0 };
int testfclip = 0;


inline void *NuRndrStreamAddZBState(void *stream, int index)
{
    static int mask[] = { -1, 0 };
    if (zbstate_set) {
        zbstatebuff.zmask = mask[index];
        NuMemCopy128(stream, &zbstatebuff, 3);
        stream = (u128 *)stream + 3;
    }
    return stream;
}


inline int NuRndrSetSpecularLight(struct nuvec_s *dir, struct nucolour4_s *col)
{
    static struct nuvec_s camvec = { 0.0f, 0.0f, -1.0f };
    struct nuvec_s v;
    float half;

    if (dir) {
        rndrstream_specular_dir = *dir;
    }
    if (col) {
        rndrstream_specular_intensity = *col;
    }
    half = 0.5f;
    NuVecInvMtxRotate(&v, &rndrstream_specular_dir, &global_camera.mtx);
    rndrstream_specular_buffer.col = rndrstream_specular_intensity;
    if (v.z >= half) {
        rndrstream_specular_buffer.col.a *= (1.0f - v.z) * 2.0f;
    }
    NuVecLerp(&v, &v, &camvec, half);
    NuVecMtxRotate(&v, &v, &global_camera.mtx);
    NuVecNorm(&v, &v);
    rndrstream_specular_buffer.mtx = numtx_identity;
    NuMtxAlignZ(&rndrstream_specular_buffer.mtx, &v);
    rndrstream_specular_buffer.mtx._20 = -rndrstream_specular_buffer.mtx._20;
    rndrstream_specular_buffer.mtx._21 = -rndrstream_specular_buffer.mtx._21;
    rndrstream_specular_buffer.mtx._22 = -rndrstream_specular_buffer.mtx._22;
    rndrstream_specular_buffer.mtx._32 = rndrstream_specular_buffer.mtx._03 = rndrstream_specular_buffer.mtx._13 = rndrstream_specular_buffer.mtx._23 = 0.0f;
    rndrstream_specular_buffer.mtx._30 = half;
    rndrstream_specular_buffer.mtx._31 = half;
    rndrstream_specular_buffer.mtx._33 = 1.0f;
    rndrstream_specular_addr[0] = 0;
    return 1;
}


struct nurtlight_s rndrstream_rt_light_buffer = { { 0 } };


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

    pkt = vpDmaTag_CntEx(&zbstatebuff, 0x11000000, 0x50000000);
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


