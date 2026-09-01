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

struct nurndrstream_s;
struct nuvec_s;
struct nucolour3_s;


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


void *NuStreamGetOutputBuffer(int index)
{
    if (index) {
        return stream_outbuf1;
    }
    return stream_outbuf0;
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


int NuRndrSetGlobalAlpha(float alpha)
{
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


void *NuRndrStreamAlloc(int qwords)
{
    char *mem;

    mem = rndrstream_free;
    rndrstream_free = mem + qwords * 16;
    return mem;
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


void NuRndrStreamClearZBState(void)
{
    zbstate_set = 0;
}
