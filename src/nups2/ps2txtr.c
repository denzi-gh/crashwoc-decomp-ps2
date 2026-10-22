/*
 * Unit: nups2/ps2txtr
 *
 * Functions:
 *   0x00165ff0 QuantizeTexture
 *   0x00166330 GenTextureLoad
 *   0x001670a8 GenPaletteLoad
 *   0x00167398 Ps2TxSetStateData
 *   0x001675a8 Ps2TxSetSurfaceStateData
 *   0x00167798 NuPs2CreateRenderTarget
 *   0x00167b10 NuPs2ChangeTexPalStreamTemp
 *   0x00167c98 NuPs2TintTexPalStreamTemp
 *   0x00167df8 NuPs2TxCreate
 *   0x00168120 NuPs2TxGenUpload
 *   0x00168430 Ps2TxValToPowerOfTwo
 *   0x00168470 NuPs2TxDestroy
 *   0x00168500 NuPs2TxUploadPal
 *   0x00168618 NuPs2ChangeTexPal
 *   0x00168718 Ps2TxGetTex0
 *   0x00168788 GetTexturePreLoad
 */

#include "nu_libc.h"
#include "nu_types.h"

#include "nu3d/nustream.h"
#include "nu3d/nutex.h"
#include "nucore/nuerror.h"
#include "nups2/nups2types.h"
#include "nups2/ps2txtr.h"

typedef union QuantColour {
    u32 value;
    struct {
        u32 b : 8;
        u32 g : 8;
        u32 r : 8;
        u32 a : 8;
    } c;
} QuantColour;

typedef union GifPtr {
    u8 *b;
    u32 *w;
    u64 *d;
    struct DmaTag *tag;
} GifPtr;

typedef struct PalHeader {
    u128 qw[6];
} PalHeader;

typedef struct Clut16 {
    u32 col[16];
} Clut16;

#define PAL_HEADER_SIZE 0x60
#define GS_PSMT4 0x14

extern int gs_botfree;
extern int PS2_REZ_W;
extern int PS2_REZ_H;
extern int nutex_force_quantisation;
extern int numtl_force_mipmap;
extern int nutex_force_overbrighten;

extern u8 *D_0062F018;
#define quant_indices D_0062F018
extern int D_0062F01C;
#define quant_count D_0062F01C
extern u32 D_006B3330[256];
#define quant_palette D_006B3330
extern s16 D_002EA0B0[16];
#define psmt4_col96off D_002EA0B0

extern char D_00616150[];
#define ps2txtr_srcfile D_00616150
extern char D_00616170[];
#define quant_alpha_msg D_00616170
extern char D_00616190[];
#define quant_banner_msg D_00616190
extern char D_006161C0[];
#define quant_overflow_msg D_006161C0
extern char D_006161F0[];
#define ps2txtr_preload_msg D_006161F0
extern char D_00616300[];
#define ps2txtr_target_msg D_00616300
extern char D_00616380[];
#define ps2txtr_format_msg D_00616380
extern char D_006163A0[];
#define ps2txtr_quantised_msg D_006163A0
extern char D_006163B8[];
#define ps2txtr_width_msg D_006163B8
extern char D_00616448[];
#define ps2txtr_height_msg D_00616448
extern char D_0062F020[];
#define ps2txtr_assert_msg D_0062F020

extern void *NuMemAllocFn(int size, char *file, int line);
extern void NuMemFreeFn(void *ptr, char *file, int line);
extern void NuPs2GetCurrentBackBuffer(u64 *frame);
extern void NuPs2GetCurrentFrontBuffer(u64 *frame);
extern void NuPs2GetCurrentZBuffer(u64 *zbuf);
extern u8 *vpDmaTag_CntEx(u8 *cur, u32 qwc, u32 tag);
extern u8 *vpDmaTag_RetEx(u8 *cur, u32 qwc, u32 tag);
extern u8 *vpDmaTag_Close(u8 *cur);
extern u8 *vpDmaTag_Call(u8 *cur, void *addr);
extern u8 *vtBeginDirect(u8 *cur, int flags);
extern u8 *vtCloseDirect(u8 *cur);
extern u8 *gsBeginGifPacket(u8 *cur, int eop, int nloop, int pre, int prim,
                            int flg, int nreg, ...);
extern u8 *gsCloseGifPacket(u8 *cur);


static inline int QuantAddColour(u32 colour) {
    int i;

    for (i = 0; i < quant_count; i++) {
        if (colour == quant_palette[i]) {
            return i;
        }
    }
    if (i < 256) {
        quant_palette[i] = colour;
        quant_count++;
        return i;
    }
    return -1;
}


void QuantizeTexture(struct nutex_s *spec) {
    QuantColour col;
    int index;
    int alpha;
    int pix;
    int x;
    int y;
    u8 *src;

    index = -1;
    if (!quant_indices) {
        quant_indices = NuMemAllocFn(0x40000, ps2txtr_srcfile, 0x180);
    }
    quant_count = 0;
    switch (spec->type) {
    case 2:
    case 3:
        break;
    default:
        NuDebugMsgProlog(ps2txtr_srcfile, 0x18B)(quant_alpha_msg);
        return;
    }
    pix = 0;
    src = spec->bits;
    for (y = 0; y < spec->height; y++) {
        for (x = 0; x < spec->width; x++) {
            switch (spec->type) {
            case 2:
                col.c.a = 0x80;
                col.c.r = *src++ & 0xF0;
                col.c.g = *src++ & 0xF8;
                col.c.b = *src++ & 0xF8;
                index = QuantAddColour(col.value);
                break;
            case 3:
                alpha = (*src++ & 0xF0) * 255 / 240;
                col.c.a = alpha > 0x80 ? 0x80 : alpha;
                col.c.r = *src++ & 0xF0;
                col.c.g = *src++ & 0xF8;
                col.c.b = *src++ & 0xF0;
                index = QuantAddColour(col.value);
                break;
            }
            if (index == -1) {
                quant_count = 0;
                NuDebugMsgProlog(ps2txtr_srcfile, 0x1A9)(quant_banner_msg);
                NuDebugMsgProlog(ps2txtr_srcfile, 0x1AA)(quant_overflow_msg, x, y);
                NuDebugMsgProlog(ps2txtr_srcfile, 0x1AB)(quant_banner_msg);
                return;
            }
            quant_indices[pix++] = index;
        }
    }
}


int GenTextureLoad(struct nutex_s *spec, struct nups2tex_s *tex) {
    int skip;
    int w;
    int h;
    int offset;
    int step;
    int i;
    u8 *bits;
    u8 *src;
    int nibbles;
    int psm;
    int bpp;
    int tcc;
    int alphamode;
    int walign;
    int halign;
    int size;
    int allocsize;
    int tbw;
    int first;
    int x;
    int y;
    int toggle;
    int texel;
    u8 *pkt;
    GifPtr p;

    skip = 0;
    w = spec->width;
    h = spec->height;
    while (w > 0x200 || h > 0x200) {
        w >>= 1;
        skip++;
        h >>= 1;
    }
    if (!w || !h) {
        return 0;
    }
    offset = 0;
    w >>= tex->mipcount;
    h >>= tex->mipcount;
    bits = spec->bits;
    if (spec->mmcnt) {
        step = 1;
        for (i = 0; i < tex->mipcount; i++) {
            offset += (spec->width >> i) * (spec->height >> i);
        }
    } else {
        step = 1 << (tex->mipcount + skip);
    }
    switch (spec->type) {
    case 4:
        size = (w * h / 2 + 15) & 0xFFFFFFF0;
        bits += offset >> 1;
        psm = 0x14;
        bpp = 4;
        nibbles = 1;
        tcc = 1;
        alphamode = 0;
        walign = 0x80;
        halign = 0x80;
        break;
    case 5:
        size = (w * h + 15) & 0xFFFFFFF0;
        bits += offset;
        psm = 0x13;
        bpp = 8;
        nibbles = 2;
        tcc = 1;
        alphamode = 0;
        walign = 0x80;
        halign = 0x40;
        break;
    case 0:
    case 1:
    default:
        size = (w * h * 2 + 15) & 0xFFFFFFF0;
        bits += offset * 2;
        psm = 2;
        bpp = 16;
        nibbles = 4;
        tcc = 1;
        alphamode = 1;
        walign = 0x40;
        halign = 0x40;
        break;
    case 2:
        size = (w * h * 3 + 15) & 0xFFFFFFF0;
        bits += offset * 3;
        psm = 1;
        bpp = 32;
        nibbles = 6;
        tcc = 0;
        alphamode = 1;
        walign = 0x40;
        halign = 0x20;
        break;
    case 3:
    case 6:
    case 7:
        size = (w * h * 4 + 15) & 0xFFFFFFF0;
        bits += offset * 4;
        psm = 0;
        bpp = 32;
        nibbles = 8;
        tcc = 1;
        alphamode = 0;
        walign = 0x40;
        halign = 0x20;
        break;
    }
    if (w < 8 || h < 8) {
        return 0;
    }
    if (tex->mipcount < 7) {
        tbw = ((w + (walign - 1)) & -walign) / 64;
        if (h >= halign) {
            halign = h;
        }
        tex->upsize[tex->mipcount] = ((tbw * (halign << 6) * bpp >> 3) + 0x1FFF) & 0xFFFFE000;
        allocsize = size + 0x90;
        allocsize += (allocsize + 0x7CFFF) / 0x7D000 * 32;
        tex->mipdata[tex->mipcount] = NuMemAllocFn(allocsize, ps2txtr_srcfile, 0x250);
        tex->reqsize[tex->mipcount] = allocsize;
        if (tex->mipdata[tex->mipcount]) {
            tex->tbw[tex->mipcount] = tbw;
            tex->tcc = tcc;
            pkt = 0;
            first = 1;
            tex->alphamode = alphamode;
            tex->psm = psm;
            p.b = tex->mipdata[tex->mipcount];
            if (spec->mmcnt) {
                for (y = 0; y < h; y++) {
                    if (y == 0 || ((u32)(p.b - pkt) > 0x7CFFF && !((nuintptr)p.b & 0xF))) {
                        if (pkt) {
                            p.b = gsCloseGifPacket(p.b);
                            p.b = vtCloseDirect(p.b);
                            p.b = vpDmaTag_Close(p.b);
                        }
                        pkt = p.b;
                        p.b = vpDmaTag_CntEx(pkt, 0, 0x50000000);
                        p.b = vtBeginDirect(p.b - 4, 0);
                        if (first) {
                            p.b = gsBeginGifPacket(p.b, 0, 1, 0, 0, 0, 1, 0xE);
                            first = 0;
                            *p.d++ = 0;
                            *p.d++ = 0x51;
                            *p.d++ = w | (u64)h << 32;
                            *p.d++ = 0x52;
                            *p.d++ = 0;
                            *p.d = 0x53;
                            p.b = gsCloseGifPacket(p.b + 8);
                        }
                        p.b = gsBeginGifPacket(p.b, 0, 1, 0, 0, 2, 0);
                    }
                    src = bits + nibbles * y * w / 2;
                    for (x = 0; x < w; x += 2) {
                        switch (spec->type) {
                        case 4:
                            *p.b++ = *src >> 4 | *src << 4;
                            src++;
                            break;
                        case 6:
                            *p.b++ = *src++;
                            *p.b++ = *src++;
                            *p.b++ = *src++;
                            *p.b++ = *src++;
                            *p.b++ = *src++;
                            *p.b++ = *src++;
                            *p.b++ = *src++;
                            *p.b++ = *src++;
                            break;
                        case 5:
                            *p.b++ = *src++;
                            *p.b++ = *src++;
                            break;
                        case 7:
                            *p.b++ = *src++;
                            *p.b++ = *src++;
                            *p.b++ = *src++;
                            *p.b++ = *src++;
                            *p.b++ = *src++;
                            *p.b++ = *src++;
                            *p.b++ = *src++;
                            *p.b++ = *src++;
                            break;
                        case 0:
                        case 1:
                        default:
                            *p.b++ = *src++;
                            *p.b++ = *src++;
                            *p.b++ = *src++;
                            *p.b++ = *src++;
                            break;
                        case 2:
                            *p.b++ = src[2];
                            *p.b++ = src[1];
                            *p.b++ = src[0];
                            *p.b++ = src[5];
                            *p.b++ = src[4];
                            *p.b++ = src[3];
                            src += 6;
                            break;
                        case 3:
                            *p.b++ = src[3];
                            *p.b++ = src[2];
                            *p.b++ = src[1];
                            if (nutex_force_overbrighten) {
                                *p.b++ = src[0];
                            } else {
                                *p.b++ = src[0] >> 1;
                            }
                            *p.b++ = src[7];
                            *p.b++ = src[6];
                            *p.b++ = src[5];
                            if (nutex_force_overbrighten) {
                                *p.b++ = src[4];
                            } else {
                                *p.b++ = src[4] >> 1;
                            }
                            src += 8;
                            break;
                        }
                    }
                }
            } else {
                for (y = 0; y < spec->height; y += step) {
                    if (y == 0 || ((u32)(p.b - pkt) > 0x7CFFF && !((nuintptr)p.b & 0xF))) {
                        if (pkt) {
                            p.b = gsCloseGifPacket(p.b);
                            p.b = vtCloseDirect(p.b);
                            p.b = vpDmaTag_Close(p.b);
                        }
                        pkt = p.b;
                        p.b = vpDmaTag_CntEx(pkt, 0, 0x50000000);
                        p.b = vtBeginDirect(p.b - 4, 0);
                        if (first) {
                            p.b = gsBeginGifPacket(p.b, 0, 1, 0, 0, 0, 1, 0xE);
                            first = 0;
                            *p.d++ = 0;
                            *p.d++ = 0x51;
                            *p.d++ = w | (u64)h << 32;
                            *p.d++ = 0x52;
                            *p.d++ = 0;
                            *p.d = 0x53;
                            p.b = gsCloseGifPacket(p.b + 8);
                        }
                        p.b = gsBeginGifPacket(p.b, 0, 1, 0, 0, 2, 0);
                    }
                    toggle = 0;
                    src = bits + nibbles * y * spec->width / 2;
                    for (x = 0; x < spec->width; x += step) {
                        switch (spec->type) {
                        case 4:
                            src = bits + nibbles * y * spec->width / 2 + x / 2;
                            texel = *src >> (4 - (x & 1) * 4) & 0xF;
                            if (!toggle) {
                                *p.b = texel;
                            } else {
                                *p.b++ |= texel << 4;
                            }
                            toggle = 1 - toggle;
                            break;
                        case 6:
                            *p.b++ = *src++;
                            *p.b++ = *src++;
                            *p.b++ = *src++;
                            *p.b++ = *src++;
                            break;
                        case 5:
                            *p.b++ = *src;
                            src += step;
                            break;
                        case 7:
                            *p.b++ = *src++;
                            *p.b++ = *src++;
                            *p.b++ = *src++;
                            *p.b++ = *src++;
                            break;
                        case 0:
                        case 1:
                        default:
                            *p.b++ = src[0];
                            *p.b++ = src[1];
                            src += step * 2;
                            break;
                        case 2:
                            *p.b++ = src[2];
                            *p.b++ = src[1];
                            *p.b++ = src[0];
                            src += step + step * 2;
                            break;
                        case 3:
                            *p.b++ = src[3];
                            *p.b++ = src[2];
                            *p.b++ = src[1];
                            if (nutex_force_overbrighten) {
                                *p.b++ = src[0];
                            } else {
                                *p.b++ = src[0] >> 1;
                            }
                            src += step * 4;
                            break;
                        }
                    }
                }
            }
            p.b = gsCloseGifPacket(p.b);
            p.b = vtCloseDirect(p.b);
            vpDmaTag_Close(p.b);
            ((struct DmaTag *)pkt)->id = 0x60;
            if (!tex->mipcount) {
                tex->gsused += (u32)tex->upsize[tex->mipcount] >> 8;
            } else {
                switch (psm) {
                case 0x14:
                    switch (w) {
                    case 32:
                        tex->upsize[tex->mipcount] = 8;
                        break;
                    case 16:
                        tex->upsize[tex->mipcount] = 10;
                        break;
                    case 8:
                        tex->upsize[tex->mipcount] = psmt4_col96off[h * 2 >> 4 > 0 ? h * 2 >> 4 : 1];
                        break;
                    default:
                        tex->gsused += (u32)tex->upsize[tex->mipcount] >> 8;
                        break;
                    }
                    break;
                case 0x13:
                    switch (w) {
                    case 32:
                        tex->upsize[tex->mipcount] = 0x10;
                        break;
                    case 16:
                        tex->upsize[tex->mipcount] = 0x14;
                        break;
                    case 8:
                        tex->upsize[tex->mipcount] = 0x15;
                        break;
                    default:
                        tex->gsused += (u32)tex->upsize[tex->mipcount] >> 8;
                        break;
                    }
                    break;
                case 0:
                case 1:
                    switch (w) {
                    default:
                        tex->gsused += (u32)tex->upsize[tex->mipcount] >> 8;
                        break;
                    case 16:
                        tex->upsize[tex->mipcount] = w;
                        break;
                    case 8:
                        tex->upsize[tex->mipcount] = 0x14;
                        break;
                    }
                    break;
                case 2:
                    break;
                }
            }
            tex->uppsm = psm;
            tex->dbw[tex->mipcount] = tex->tbw[tex->mipcount];
            switch (spec->type) {
            case 6:
                w = spec->width * 2;
                tbw = ((w + 0x7F) & ~0x7F) >> 6;
                tex->tbw[tex->mipcount] = tbw;
                tex->psm = 0x14;
                tex->tw += 1;
                tex->th += 2;
                break;
            case 7:
                w = spec->width * 2;
                tbw = ((w + 0x7F) & ~0x7F) >> 6;
                tex->tbw[tex->mipcount] = tbw;
                tex->psm = 0x13;
                tex->tw += 1;
                tex->th += 1;
                break;
            }
            tex->mipcount++;
            return 1;
        }
    }
    return 0;
}


int GenPaletteLoad(struct nutex_s *spec, struct nups2tex_s *tex) {
    int cpsm;
    int size;
    int width;
    int height;
    int bufsize;
    u8 *buf;
    GifPtr p;
    u32 *src;
    int block;
    int i;

    switch (spec->type) {
    case 4:
    case 6:
        cpsm = 0;
        size = 0x40;
        width = 8;
        height = 2;
        break;
    case 5:
    case 7:
    case 8:
    case 9:
    case 10:
        cpsm = 0;
        size = 0x400;
        width = 16;
        height = 16;
        break;
    default:
        return 0;
    }
    bufsize = size + 0x80;
    tex->palsize = 0x2000;
    buf = NuMemAllocFn(bufsize, ps2txtr_srcfile, 0x3ED);
    tex->paldata = buf;
    if (buf) {
        tex->palstreamsize = bufsize;
        tex->cpsm = cpsm;
        tex->cbw = (width + 63) / 64;
        p.b = vpDmaTag_RetEx(buf, 0, 0x50000000);
        p.b = vtBeginDirect(p.b - 4, 0);
        p.b = gsBeginGifPacket(p.b, 0, 1, 0, 0, 0, 1, 0xE);
        *p.d++ = 0;
        *p.d++ = 0x51;
        *p.d++ = width | (u64)height << 32;
        *p.d++ = 0x52;
        *p.d++ = 0;
        *p.d = 0x53;
        p.b = gsCloseGifPacket(p.b + 8);
        p.b = gsBeginGifPacket(p.b, 0, 1, 0, 0, 2, 0);
        src = spec->pal;
        switch (spec->type) {
        case 4:
        case 6:
            *(Clut16 *)p.w = *(Clut16 *)src;
            p.w += 16;
            break;
        case 5:
        case 7:
        case 8:
        case 9:
        case 10:
            for (block = 0; block < 8; block++) {
                for (i = 0; i < 8; i++) {
                    p.w[i] = src[i];
                    p.w[i + 8] = src[i + 16];
                    p.w[i + 16] = src[i + 8];
                    p.w[i + 24] = src[i + 24];
                }
                src += 32;
                p.w += 32;
            }
            break;
        }
        p.b = gsCloseGifPacket(p.b + size);
        p.b = vtCloseDirect(p.b);
        vpDmaTag_Close(p.b);
        tex->gsused += tex->palsize >> 8;
        return 1;
    }
    return 0;
}


u64 *Ps2TxSetStateData(u64 *dst, struct nups2tex_s *tex, struct nups2tex_s *paltex, char context) {
    struct nups2tex_s *pal;
    int cld;
    int tbp;

    pal = tex;
    if (paltex) {
        pal = paltex;
    }
    cld = 0;
    if (pal->cbw) {
        *dst++ = pal->cbw;
        *dst++ = 0x1C;
        cld = 2;
    }
    *dst++ = 1;
    *dst++ = 0x3F;
    tbp = tex->tbp[0];
    if (!tbp) {
        tbp = tex->gsaddr;
    }
    tbp = tbp ? tbp : gs_botfree;
    *dst++ = tbp | (u64)tex->tbw[0] << 14 | (u64)tex->psm << 20
           | (u64)tex->tw << 26 | (u64)tex->th << 30 | (u64)tex->tcc << 34
           | (u64)pal->cbp << 37 | (u64)pal->cpsm << 51 | (u64)cld << 61;
    if (context) {
        *dst++ = 7;
    } else {
        *dst++ = 6;
    }
    switch (tex->alphamode) {
    case 0:
        break;
    case 1:
        *dst++ = 0x80;
        *dst++ = 0x3B;
        break;
    case 2:
        *dst++ = (u64)0x80 << 32;
        *dst++ = 0x3B;
        break;
    }
    if (tex->mipcount > 1) {
        *dst++ = tex->tbp[1] | (u64)tex->tbw[1] << 14 | (u64)tex->tbp[2] << 20
               | (u64)tex->tbw[2] << 34 | (u64)tex->tbp[3] << 40 | (u64)tex->tbw[3] << 54;
        if (context) {
            *dst++ = 0x35;
        } else {
            *dst++ = 0x34;
        }
        if (tex->mipcount > 4) {
            *dst++ = tex->tbp[4] | (u64)tex->tbw[4] << 14 | (u64)tex->tbp[5] << 20
                   | (u64)tex->tbw[5] << 34 | (u64)tex->tbp[6] << 40 | (u64)tex->tbw[6] << 54;
            if (context) {
                *dst++ = 0x37;
            } else {
                *dst++ = 0x36;
            }
        }
    }
    return dst;
}


inline int Ps2TxValToPowerOfTwo(int value) {
    int shift;
    int orig;

    shift = 0;
    orig = value;
    while (value >= 2) {
        value >>= 1;
        shift++;
    }
    if (orig & ((1 << shift) - 1)) {
        shift++;
    }
    return shift;
}


u64 *Ps2TxSetSurfaceStateData(u64 *dst, int surface) {
    u64 frame;
    u64 zbuf;
    int tbp;
    int psm;
    int tcc;
    int tw;
    int th;

    switch (surface) {
    case -1:
        NuPs2GetCurrentBackBuffer(&frame);
        tbp = (int)(frame & 0x1FF) << 5;
        psm = (int)(frame >> 24 & 0x3F);
        break;
    case -3:
        NuPs2GetCurrentFrontBuffer(&frame);
        tbp = (int)(frame & 0x1FF) << 5;
        psm = (int)(frame >> 24 & 0x3F);
        break;
    case -2:
        NuPs2GetCurrentZBuffer(&zbuf);
        tbp = (int)(zbuf & 0x1FF) << 5;
        psm = (int)(zbuf >> 24 & 0xF) | 0x30;
        break;
    default:
        return dst;
    }
    switch (psm) {
    case 0:
        tcc = 1;
        break;
    case 1:
        tcc = 0;
        break;
    case 2:
    default:
        tcc = 0;
        break;
    }
    tw = Ps2TxValToPowerOfTwo(PS2_REZ_W);
    th = Ps2TxValToPowerOfTwo(PS2_REZ_H);
    *dst++ = 1;
    *dst++ = 0x3F;
    *dst++ = tbp | (u64)(PS2_REZ_W / 64) << 14 | (u64)psm << 20 | (u64)tw << 26
           | (u64)th << 30 | (u64)tcc << 34;
    *dst++ = 6;
    *dst++ = 0x80;
    *dst++ = 0x3B;
    return dst;
}


int NuPs2CreateRenderTarget(struct nutex_s *spec, struct nups2tex_s *tex) {
    int clut[256];
    int size;
    int align;
    int i;

    memset(tex, 0, sizeof(struct nups2tex_s));
    size = 0;
    align = 0;
    tex->tw = Ps2TxValToPowerOfTwo(spec->width);
    tex->th = Ps2TxValToPowerOfTwo(spec->height);
    switch (spec->type) {
    case 8:
    case 9:
        tex->psm = 0x13;
        tex->tcc = 1;
        tex->cpsm = 0;
        align = 0x80;
        tex->alphamode = 0;
        spec->pal = clut;
        size = (spec->width * spec->height * 2 + 15) & 0xFFFFFFF0;
        for (i = 0; i < 256; i++) {
            clut[i] = i << 8;
        }
        GenPaletteLoad(spec, tex);
        break;
    case 10:
        tex->psm = 0x1B;
        tex->tcc = 1;
        tex->cpsm = 0;
        align = 0x40;
        tex->alphamode = 0;
        spec->pal = clut;
        size = (spec->width * spec->height * 2 + 15) & 0xFFFFFFF0;
        for (i = 0; i < 256; i++) {
            clut[i] = i << 8;
        }
        GenPaletteLoad(spec, tex);
        break;
    case 13:
    case 16:
    case 19:
        tex->tcc = 1;
        tex->psm = 2;
        tex->alphamode = 2;
        align = 0x40;
        size = (spec->width * spec->height * 2 + 15) & 0xFFFFFFF0;
        break;
    case 12:
    case 15:
    case 18:
        tex->tcc = 0;
        tex->psm = 1;
        tex->alphamode = 1;
        align = 0x40;
        size = (spec->width * spec->height * 4 + 15) & 0xFFFFFFF0;
        break;
    case 11:
    case 14:
    case 17:
        tex->psm = 0;
        tex->tcc = 1;
        tex->alphamode = 0;
        align = 0x40;
        size = (spec->width * spec->height * 4 + 15) & 0xFFFFFFF0;
        break;
    default:
        NuErrorProlog(ps2txtr_srcfile, 0x578)(ps2txtr_target_msg);
        break;
    }
    switch (spec->type) {
    case 0x13:
        tex->psm = 0x32;
        break;
    case 0x11:
        tex->psm = 0x30;
        break;
    case 0x12:
        tex->psm = 0x31;
        break;
    }
    switch (spec->type) {
    case 11:
    case 12:
    case 13:
        tex->tbp[0] = NuTexReserve(size);
        tex->tbw[0] = ((spec->width + (align - 1)) & -align) / 64;
        break;
    case 8:
    case 9:
    case 10:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
        tex->tbp[0] = 0;
        tex->tbw[0] = ((spec->width + (align - 1)) & -align) / 64;
        break;
    default:
        NuErrorProlog(ps2txtr_srcfile, 0x5A5)(ps2txtr_target_msg);
        break;
    }
    size += 255;
    size /= 256;
    tex->reqsize[0] = size;
    return 1;
}


void NuPs2ChangeTexPalStreamTemp(struct nups2tex_s *tex, u32 *pal) {
    u32 *dst;
    int block;
    int i;

    if (tex->paldata) {
        tex->palstream = NuRndrStreamAlloc(tex->palstreamsize >> 4);
        *(PalHeader *)tex->palstream = *(PalHeader *)tex->paldata;
        dst = (u32 *)tex->palstream;
        dst += PAL_HEADER_SIZE / 4;
        if (tex->psm == GS_PSMT4) {
            *(Clut16 *)dst = *(Clut16 *)pal;
        } else {
            memcpy(tex->palstream + 0x418, tex->paldata + 0x418, tex->palstreamsize - 0x460);
            for (block = 0; block < 8; block++) {
                for (i = 0; i < 8; i++) {
                    dst[i] = pal[i];
                    dst[i + 8] = pal[i + 16];
                    dst[i + 16] = pal[i + 8];
                    dst[i + 24] = pal[i + 24];
                }
                pal += 32;
                dst += 32;
            }
        }
    }
}


void NuPs2TintTexPalStreamTemp(struct nups2tex_s *tex, int tint) {
    u8 *src;
    u8 *dst;
    int r;
    int g;
    int b;
    int a;
    int c;
    int i;

    r = tint & 0xFF;
    g = tint >> 8 & 0xFF;
    b = tint >> 16 & 0xFF;
    a = tint >> 24 & 0xFF;
    if (tex->paldata) {
        tex->palstream = NuRndrStreamAlloc(tex->palstreamsize >> 4);
        *(PalHeader *)tex->palstream = *(PalHeader *)tex->paldata;
        memcpy(tex->palstream + 0x418, tex->paldata + 0x418, tex->palstreamsize - 0x460);
        dst = tex->palstream;
        dst += PAL_HEADER_SIZE;
        src = tex->paldata;
        src += PAL_HEADER_SIZE;
        for (i = 0; i < 0x400; i += 4) {
            c = src[i] * r >> 7;
            if (c > 0xFF) {
                dst[i] = 0xFF;
            } else {
                dst[i] = c;
            }
            c = src[i + 1] * g >> 7;
            if (c > 0xFF) {
                dst[i + 1] = 0xFF;
            } else {
                dst[i + 1] = c;
            }
            c = src[i + 2] * b >> 7;
            if (c > 0xFF) {
                dst[i + 2] = 0xFF;
            } else {
                dst[i + 2] = c;
            }
            c = src[i + 3] * a >> 7;
            if (c > 0xFF) {
                dst[i + 3] = 0xFF;
            } else {
                dst[i + 3] = c;
            }
        }
    }
}


int NuPs2TxCreate(struct nutex_s *spec, struct nups2tex_s *tex) {
    struct nutex_s quantised;
    int level;

    if (spec->type >= 8) {
        if (spec->type < 0x14) {
            return NuPs2CreateRenderTarget(spec, tex);
        }
        NuErrorProlog(ps2txtr_srcfile, 0x640)(ps2txtr_format_msg);
        return 0;
    }
    memset(tex, 0, sizeof(struct nups2tex_s));
    quant_count = 0;
    if (nutex_force_quantisation) {
        QuantizeTexture(spec);
    }
    if (quant_count) {
        spec->type = 5;
        quantised = *spec;
        quantised.type = 5;
        quantised.bits = quant_indices;
        quantised.pal = (int *)quant_palette;
        spec = &quantised;
        NuDebugMsgProlog(ps2txtr_srcfile, 0x653)(ps2txtr_quantised_msg, quant_count);
    }
    tex->tw = Ps2TxValToPowerOfTwo(spec->width);
    tex->th = Ps2TxValToPowerOfTwo(spec->height);
    if (spec->width && 1 << tex->tw != spec->width) {
        NuDebugMsgProlog(ps2txtr_srcfile, 0x65C)(ps2txtr_width_msg, spec->width);
    }
    if (spec->height && 1 << tex->th != spec->height) {
        NuDebugMsgProlog(ps2txtr_srcfile, 0x65F)(ps2txtr_height_msg, spec->height);
    }
    tex->gsused = 0;
    if (spec->bits) {
        GenTextureLoad(spec, tex);
        if (spec->mmcnt) {
            for (level = 1; level < spec->mmcnt; level++) {
                GenTextureLoad(spec, tex);
            }
        } else if (numtl_force_mipmap) {
            while (GenTextureLoad(spec, tex)) {
            }
        }
    }
    GenPaletteLoad(spec, tex);
    switch (spec->type) {
    case 6:
        spec->width *= 2;
        spec->height *= 4;
        break;
    case 7:
        spec->width *= 2;
        spec->height *= 2;
        break;
    }
    while (tex->tw > 0x200 || tex->th > 0x200) {
        tex->tw >>= 1;
        tex->th >>= 1;
    }
    return 1;
}


u8 *NuPs2TxGenUpload(u8 *stream, struct nups2tex_s *tex, int addr) {
    GifPtr p;
    int first;
    int wshift;
    int i;

    p.b = stream;
    tex->gsbase = addr;
    if (tex->paldata) {
        addr -= tex->palsize >> 8;
        tex->cbp = addr;
        p.b = vpDmaTag_CntEx(p.b, 0, 0x50000000);
        p.tag[-1].pad = 3;
        p.b = vtBeginDirect(p.b - 4, 0);
        p.b = gsBeginGifPacket(p.b, 0, 1, 0, 0, 0, 1, 0xE);
        *p.d++ = 1;
        *p.d++ = 0x3F;
        *p.d++ = (u64)tex->cbp << 32 | (u64)tex->cbw << 48 | (u64)tex->cpsm << 56;
        *p.d = 0x50;
        p.b = gsCloseGifPacket(p.b + 8);
        p.b = vtCloseDirect(p.b);
        p.b = vpDmaTag_Close(p.b);
        p.b = vpDmaTag_Call(p.b, tex->paldata);
    }
    wshift = tex->tw;
    first = 1;
    if (tex->uppsm >= 3) {
        wshift--;
    }
    for (i = 0; i < tex->mipcount; i++) {
        if (tex->mipdata[i]) {
            if (wshift >= 5 || first) {
                if (!((u32)tex->upsize[i] >> 8)) {
                    NuErrorProlog(ps2txtr_srcfile, 0x6CD)(ps2txtr_assert_msg);
                }
                first = 0;
                addr -= (u32)tex->upsize[i] >> 8;
                tex->tbp[i] = addr;
            } else {
                tex->tbp[i] = addr + tex->upsize[i];
            }
            p.b = vpDmaTag_CntEx(p.b, 0, 0x50000000);
            p.b = vtBeginDirect(p.b - 4, 0);
            p.b = gsBeginGifPacket(p.b, 0, 1, 0, 0, 0, 1, 0xE);
            *p.d++ = 1;
            *p.d++ = 0x3F;
            *p.d++ = (u64)tex->dbw[i] << 16 | (u64)tex->uppsm << 24 | (u64)tex->tbp[i] << 32
                   | (u64)tex->dbw[i] << 48 | (u64)tex->uppsm << 56;
            *p.d = 0x50;
            p.b = gsCloseGifPacket(p.b + 8);
            p.b = vtCloseDirect(p.b);
            p.b = vpDmaTag_Close(p.b);
            p.b = vpDmaTag_Call(p.b, tex->mipdata[i]);
        }
        wshift--;
    }
    tex->palend = addr;
    if (tex->gsbase - tex->palend < tex->gsused) {
        NuErrorProlog(ps2txtr_srcfile, 0x6EF)(ps2txtr_assert_msg);
    }
    return p.b;
}


int NuPs2TxDestroy(struct nups2tex_s *tex) {
    int i;

    for (i = 0; i < tex->mipcount; i++) {
        NuMemFreeFn(tex->mipdata[i], ps2txtr_srcfile, 0x136);
    }
    if (tex->paldata) {
        NuMemFreeFn(tex->paldata, ps2txtr_srcfile, 0x139);
    }
    return 0;
}


u8 *NuPs2TxUploadPal(u8 *stream, struct nups2tex_s *tex, int addr) {
    u8 *pal;
    GifPtr p;

    p.b = stream;
    pal = tex->paldata;
    if (tex->palstream) {
        pal = tex->palstream;
    }
    tex->gsbase = addr;
    if (pal) {
        addr -= tex->palsize >> 8;
        tex->cbp = addr;
        p.b = vpDmaTag_CntEx(p.b, 0, 0x50000000);
        p.b = vtBeginDirect(p.b - 4, 0);
        p.b = gsBeginGifPacket(p.b, 0, 1, 0, 0, 0, 1, 0xE);
        *p.d++ = 1;
        *p.d++ = 0x3F;
        *p.d++ = (u64)tex->cbp << 32 | (u64)tex->cbw << 48 | (u64)tex->cpsm << 56;
        *p.d = 0x50;
        p.b = gsCloseGifPacket(p.b + 8);
        p.b = vtCloseDirect(p.b);
        p.b = vpDmaTag_Close(p.b);
        p.b = vpDmaTag_Call(p.b, pal);
    }
    tex->palend = addr;
    return p.b;
}


void NuPs2ChangeTexPal(struct nups2tex_s *tex, u32 *pal) {
    u32 *dst;
    int block;
    int i;

    if (tex->paldata) {
        dst = (u32 *)tex->paldata;
        dst += PAL_HEADER_SIZE / 4;
        if (tex->psm == GS_PSMT4) {
            *(Clut16 *)dst = *(Clut16 *)pal;
        } else {
            for (block = 0; block < 8; block++) {
                for (i = 0; i < 8; i++) {
                    dst[i] = pal[i];
                    dst[i + 8] = pal[i + 16];
                    dst[i + 16] = pal[i + 8];
                    dst[i + 24] = pal[i + 24];
                }
                pal += 32;
                dst += 32;
            }
        }
    }
}


u64 Ps2TxGetTex0(struct nups2tex_s *tex) {
    int cld;

    cld = tex->paldata ? 2 : 0;
    return tex->tbp[0] | (u64)tex->tbw[0] << 14 | (u64)tex->psm << 20
         | (u64)tex->tw << 26 | (u64)tex->th << 30 | (u64)tex->tcc << 34
         | (u64)tex->cbp << 37 | (u64)tex->cpsm << 51 | (u64)cld << 61;
}


int GetTexturePreLoad(void) {
    NuErrorProlog(ps2txtr_srcfile, 0x1CC)(ps2txtr_preload_msg);
    return 0;
}
