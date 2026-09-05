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


typedef unsigned short u16;
typedef unsigned long long u64;

struct nuvec_s {
    float x;
    float y;
    float z;
};

struct nucolour3_s {
    float r;
    float g;
    float b;
};

struct nucolour4_s {
    struct nucolour3_s rgb;
    float a;
};

struct nurndrstream_s {
    void *cur;
    void *tex;
    int unk_0x08;
    int unk_0x0C;
    void *zbuf;
    int unk_0x14;
    void *mpgtag;
    char pad_0x1C[4];
    int unk_0x20;
    short unk_0x24;
    short unk_0x26;
    short unk_0x28;
    char pad_0x2A[2];
    int unk_0x2C;
    int unk_0x30;
    int unk_0x34;
};

struct nurtlight_s {
    char pad_0x00[0x10];
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
};

struct nurndrfx_s {
    char pad[4];
    void *cur;
};

struct nuzbstate_s {
    char pad[0x24];
    int unk_0x24;
};

typedef struct {
    float m[16];
} NuMtx;

struct nurndrfxmtx_s {
    char pad[0x10];
    NuMtx mtx;
};


extern void *D_00633020;
#define stream_outbuf0 D_00633020

extern void *D_00633024;
#define stream_outbuf1 D_00633024

extern u64 D_00633038;
#define rndrstream_scissor D_00633038

extern u64 D_00633040;
#define rndrstream_offset D_00633040

extern int D_0062EAC0;
#define zbstate_set D_0062EAC0

extern u16 rndrstream_scissor_id;

extern u16 rndrstream_xyoffset_id;

extern char *rndrstream_free;

extern int D_0062EA74;
#define rndrstream_buffer_index D_0062EA74

extern void NuRndrInitStreams(void);

extern struct nurtlight_s rndrstream_rt_light_buffer;

extern void *rndrstream_rt_light_addr;

extern void *gs_top;

extern struct nurndrfx_s *rndrstream_fx;

extern void *rndrstream_fx_microcode;

extern char D_0028CFF0[];

extern char D_00289020[];

extern void *vpDmaTag_Call(void *cur, void *microcode);

extern void *vpDmaTag_Next(void *cur, void *mode);

extern void NuTexAccomodateRS(struct nurndrstream_s *stream, void *ptr, signed char tex);

extern void *NuTexSet(void *tex0, void *ptr, signed char tex);

extern void *NuTexGetTex0(void *ptr);

extern struct nuzbstate_s D_0067A600;

extern int D_002D3E00[];

extern void NuMemCopy128(void *dst, void *src, int count);

extern void *rndrstream_fx_mtx_addr;

extern struct nurndrfxmtx_s D_0067A590;

extern void *txtrstream_start;

extern void *rndrstream_start;

extern void FlushCache(int mode);

extern void GS_DmaWrite(void *p, int mode);

extern void VU1_DmaWrite(void *p, int mode);

extern void tbslotReset(int a, int b);

extern void tbslotBeginFn(int a, int b);

extern int D_0062EA84;

extern char D_0067A460[];

extern int D_00633028;

extern struct nurndrstream_s D_00679B40[];

extern int D_00633030;


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
    rndrstream_rt_light_addr = 0;
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
    rndrstream_rt_light_addr = 0;
    return 1;
}


int NuRndrSetAmbientLight(struct nucolour3_s *colour)
{
    rndrstream_rt_light_buffer.r = colour->r;
    rndrstream_rt_light_buffer.g = colour->g;
    rndrstream_rt_light_buffer.b = colour->b;
    rndrstream_rt_light_buffer.a = 1.0f;
    rndrstream_rt_light_addr = 0;
    return 1;
}


int NuRndrSetGlobalAlpha(float alpha)
{
    return 1;
}


int NuRndrSetFxMtx(NuMtx *mtx)
{
    D_0067A590.mtx = *mtx;
    rndrstream_fx_mtx_addr = 0;
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


void NuRndrStreamResetStream(struct nurndrstream_s *stream, int flag)
{
    stream->cur = 0;
    stream->tex = 0;
    stream->unk_0x08 = 0;
    stream->unk_0x0C = 0;
    if (flag) {
        stream->zbuf = gs_top;
        stream->unk_0x20 = -2;
        stream->unk_0x24 = -2;
        stream->unk_0x26 = -2;
        stream->unk_0x2C = 0;
        stream->unk_0x34 = 0;
        stream->unk_0x28 = -2;
    }
}


void NuRndrStreamResetStreams(int flag)
{
    struct nurndrstream_s *s;
    int n;
    int i;

    n = D_00633028;
    if (n > 0) {
        s = D_00679B40;
        i = n;
        do {
            s->unk_0x14 = 0;
            s->cur = 0;
            s->tex = 0;
            s->unk_0x08 = 0;
            if (flag) {
                s->unk_0x0C = 0;
                s->unk_0x20 = -2;
                s->unk_0x24 = -2;
                s->unk_0x26 = -2;
                s->unk_0x28 = -2;
                s->unk_0x2C = 0;
                s->unk_0x34 = 0;
                s->zbuf = gs_top;
            }
            s++;
            i--;
        } while (i != 0);
    }
    rndrstream_rt_light_addr = 0;
    D_00633030 = 0;
    rndrstream_fx_microcode = D_00289020;
    rndrstream_fx_mtx_addr = 0;
}


void NuRndrVu1Draw(void)
{
    __asm__ volatile ("sync");
    FlushCache(0);
    if (txtrstream_start != 0) {
        GS_DmaWrite(txtrstream_start, 1);
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
        rndrstream_fx->cur = vpDmaTag_Call(rndrstream_fx->cur, microcode);
        rndrstream_fx_microcode = microcode;
    }
}


void *NuRndrStreamAlloc(int qwords)
{
    char *mem;

    mem = rndrstream_free;
    rndrstream_free = mem + qwords * 16;
    return mem;
}


void *NuRndrStreamAddTex(struct nurndrstream_s *stream, void *ptr, signed char tex)
{
    NuTexAccomodateRS(stream, ptr, tex);
    stream->tex = NuTexSet(stream->tex, ptr, tex);
    return NuTexGetTex0(ptr);
}


int NuRndrStreamSetDirectionalLights(struct nurndrstream_s *stream,
                                     struct nuvec_s *d0, struct nucolour3_s *c0,
                                     struct nuvec_s *d1, struct nucolour3_s *c1,
                                     struct nuvec_s *d2, struct nucolour3_s *c2)
{
    return 0;
}


int NuRndrStreamSetAmbientLight(struct nurndrstream_s *stream,
                                struct nucolour3_s *colour)
{
    return 0;
}


int NuRndrStreamSetViewMtxScissorClip(struct nurndrstream_s *stream)
{
    if (!(stream->unk_0x30 & 4)) {
        return 0;
    }
    if (D_0062EA84 == 1) {
        return 0;
    }
    NuMemCopy128(stream->tex, D_0067A460, D_0062EA84 >> 4);
    stream->unk_0x24 = -2;
    stream->tex = (char *)stream->tex + D_0062EA84;
    return 1;
}


void *NuRndrStreamPrependMPG(struct nurndrstream_s *stream, void *tag)
{
    void *cur;
    void *old_free;

    cur = stream->mpgtag;
    if (cur == 0 || cur == tag) {
        return tag;
    }
    old_free = rndrstream_free;
    rndrstream_free = vpDmaTag_Call(rndrstream_free, cur);
    rndrstream_free = vpDmaTag_Next(rndrstream_free, stream->cur);
    stream->cur = old_free;
    return stream->mpgtag;
}


void NuRndrStreamClearZBState(void)
{
    zbstate_set = 0;
}


void *NuRndrStreamAddZBState(void *stream, int index)
{
    if (zbstate_set) {
        D_0067A600.unk_0x24 = D_002D3E00[index];
        NuMemCopy128(stream, &D_0067A600, 3);
        stream = (char *)stream + 0x30;
    }
    return stream;
}
