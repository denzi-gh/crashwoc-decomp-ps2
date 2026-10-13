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

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef unsigned int u128 __attribute__((mode(TI)));

typedef void (*NuErrorFunc)(const char *fmt, ...);

typedef struct NuTex {
    u32 type;
    int width;
    int height;
    int mmcount;
    void *bits;
    u32 *pal;
} NuTex;

typedef struct NuPs2Tex {
    u8 unk000[0x8];
    u16 palend;
    u16 gsbase;
    u16 gsused;
    u8 unk00E[0x2];
    u16 tw;
    u16 th;
    void *mipdata[7];
    int reqsize[7];
    int upsize[7];
    u16 tbp[7];
    u16 tbw[7];
    u16 dbw[7];
    u8 mipcount;
    u8 psm;
    u8 uppsm;
    u8 tcc;
    u8 alphamode;
    u8 unk097;
    u8 *paldata;
    u8 *palstream;
    u16 cbp;
    u16 cbw;
    u16 cpsm;
    u16 palsize;
    u16 palstreamsize;
    u8 unk0AA[0x2];
    int gsaddr;
} NuPs2Tex;

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

extern u8 *D_0062F018;
#define quant_indices D_0062F018
extern int D_0062F01C;
#define quant_count D_0062F01C
extern u32 D_006B3330[256];
#define quant_palette D_006B3330

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

extern NuErrorFunc NuErrorProlog(const char *file, int line);
extern NuErrorFunc NuDebugMsgProlog(const char *file, int line);
extern void *NuMemAllocFn(int size, char *file, int line);
extern void NuMemFreeFn(void *ptr, char *file, int line);
extern void *memcpy(void *dst, const void *src, unsigned int size);
extern void *NuRndrStreamAlloc(int qwc);
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


void QuantizeTexture(NuTex *spec) {
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


int GenPaletteLoad(NuTex *spec, NuPs2Tex *tex) {
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


u64 *Ps2TxSetStateData(u64 *dst, NuPs2Tex *tex, NuPs2Tex *paltex, char context) {
    NuPs2Tex *pal;
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


void NuPs2ChangeTexPalStreamTemp(NuPs2Tex *tex, u32 *pal) {
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


int NuPs2TxDestroy(NuPs2Tex *tex) {
    int i;

    for (i = 0; i < tex->mipcount; i++) {
        NuMemFreeFn(tex->mipdata[i], ps2txtr_srcfile, 0x136);
    }
    if (tex->paldata) {
        NuMemFreeFn(tex->paldata, ps2txtr_srcfile, 0x139);
    }
    return 0;
}


u8 *NuPs2TxUploadPal(u8 *stream, NuPs2Tex *tex, int addr) {
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


void NuPs2ChangeTexPal(NuPs2Tex *tex, u32 *pal) {
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


u64 Ps2TxGetTex0(NuPs2Tex *tex) {
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
