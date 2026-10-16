/*
 * Unit: nu3d/nufont
 *
 * Functions:
 *   0x001309a8 NuFntLoad
 *   0x00130be8 NuFntLoadMem
 *   0x00130e00 NuFntCreate
 *   0x00131480 NuFntPrintChar
 *   0x00131640 NuFntPrintV
 *   0x00131930 NuFntPrint
 *   0x00131990 NuFntPrintLen
 *   0x00131ab8 NuFntPrintEx
 *   0x00131ce8 NuFntInit
 *   0x00131d70 NuFntClose
 *   0x00131dd8 NuFntSet
 *   0x00131e18 NuFntDestroy
 *   0x00131e78 NuFntScale
 *   0x00131eb0 NuFntMoveAbs
 *   0x00131ef8 NuFntMoveRel
 *   0x00131f18 NuFntPos
 *   0x00131f60 NuFntSetPen
 *   0x00131f98 NuFntPrintLenV
 */

#include "nu_libc.h"
#include "nu_stdarg.h"
#include "nu_types.h"

#include "nucore/nuerror.h"
#include "numath/numathtypes.h"
#include "nu3d/numtl.h"
#include "nu3d/nustream.h"
#include "nu3d/nutex.h"

struct nufontchar_s {
    f32 u0;
    f32 v0;
    f32 u1;
    f32 v1;
    u8 width;
    u8 height;
};

struct nufontmap_s {
    u8 index[256];
};

struct nufont_s {
    char pad0[0xC];
    s32 height;
    char pad10[0x4];
    s32 base;
    struct numtl_s *mtl;
    s32 nchars;
    struct nufontmap_s map;
    struct nufontchar_s chars[1];
};

extern s32 NuFileSize(char *name);
extern s32 NuFileOpen(char *name, s32 mode);
extern s32 NuFileRead(s32 handle, void *buf, s32 size);
extern void NuFileClose(s32 handle);
extern void *NuMemAllocFn(s32 size, char *file, s32 line);
extern void NuMemFreeFn(void *ptr, char *file, s32 line);

extern void NuRndrStreamLink(struct nurndrstream_s *rs);
extern u8 *vpDmaTag_CntEx(u8 *cur, u32 qwc, u32 tag);
extern u8 *vpDmaTag_Close(u8 *cur);
extern u8 *vpDmaTag_Next(u8 *cur, s32 tag);
extern u8 *vtBeginDirect(u8 *cur, s32 flags);
extern u8 *vtCloseDirect(u8 *cur);
extern u8 *gsBeginGifPacket(u8 *cur, s32 eop, s32 nloop, s32 pre, s32 prim,
                            s32 flg, s32 nreg, ...);
extern u8 *gsCloseGifPacket(u8 *cur);

extern struct nurndrstream_s *rndrstream_2d;
extern u8 *rndrstream_free;
extern s32 PS2_REZ_W;
extern s32 PS2_REZ_H;
extern s32 PS2_VCNTR_X;
extern s32 PS2_VCNTR_Y;

extern struct nufont_s *NuFntCur;
extern struct nufont_s *system_font;
extern s32 no_default_font;
extern s32 NuFntBase;
extern s32 NuFntHeight;
extern s32 NuFntPrintLength;
extern s32 NuFntPrintHeight;
extern u8 ariel16_fnt[];
extern u8 ariel16_bmp[];

extern s32 D_0062ED34;
extern s32 D_0062ED38;
extern s32 D_0062ED3C;
extern s32 D_0062ED40;
extern s32 D_0062ED60;
extern s32 D_002E8570[4];
extern char D_0062ED50[];
extern char D_0062ED58[];
extern char D_00614858[];
extern char D_00614878[];
extern char D_00614898[];
extern char D_006148B8[];
extern char D_006148E8[];
extern char D_00614900[];
extern char D_00614918[];
extern char D_00614938[];
#define font_xscale D_0062ED34
#define font_yscale D_0062ED38
#define font_x D_0062ED3C
#define font_y D_0062ED40
#define font_initialised D_0062ED60
#define font_pen D_002E8570
#define fnt_name_fmt D_0062ED50
#define bmp_name_fmt D_0062ED58
#define nufont_srcfile D_00614858
#define str_font_load_failed D_00614878
#define str_memfont_load_failed D_00614898
#define str_font_not_8bit D_006148B8
#define str_font_off_map D_006148E8
#define str_font_alloc_failed D_00614900
#define str_font_tex_failed D_00614918
#define str_font_mtl_failed D_00614938


struct nufont_s *NuFntLoad(char *name)
{
    char path[256];
    struct nufont_s *font;
    struct nutex_s *bmp;
    struct numtl_s *mtl;
    s32 size;
    s32 handle;
    s32 tid;

    sprintf(path, fnt_name_fmt, name);
    size = NuFileSize(path);
    if (size != 0) {
        font = NuMemAllocFn(size, nufont_srcfile, 0x50);
        if (font != 0) {
            handle = NuFileOpen(path, 0);
            if (handle != 0) {
                NuFileRead(handle, font, size);
                NuFileClose(handle);
            }
            sprintf(path, bmp_name_fmt, name);
            bmp = NuTexReadBitmap(path);
            if (bmp != 0) {
                tid = NuTexCreate(bmp);
                if (tid != 0) {
                    font->mtl = mtl = NuMtlCreate(1);
                    if (mtl != 0) {
                        mtl->diffuse.r = 1.0f;
                        font->mtl->diffuse.g = 1.0f;
                        font->mtl->diffuse.b = 1.0f;
                        font->mtl->attrib.cull = 2;
                        font->mtl->attrib.zmode = 3;
                        font->mtl->alpha = 1.0f;
                        font->mtl->attrib.alpha = 1;
                        font->mtl->attrib.colour = 1;
                        font->mtl->tid = tid;
                        NuMtlUpdate(font->mtl);
                        NuMemFreeFn(bmp->bits, nufont_srcfile, 0x69);
                        NuMemFreeFn(bmp, nufont_srcfile, 0x6A);
                        return font;
                    }
                    NuTexDestroy(tid);
                }
                NuMemFreeFn(bmp->bits, nufont_srcfile, 0x70);
                NuMemFreeFn(bmp, nufont_srcfile, 0x71);
            }
        }
    }
    NuErrorProlog(nufont_srcfile, 0x75)(str_font_load_failed, name);
    return 0;
}


struct nufont_s *NuFntLoadMem(void *fntdata, s32 size, void *bmpdata)
{
    struct nufont_s *font;
    struct nutex_s *bmp;
    struct numtl_s *mtl;
    s32 tid;

    font = NuMemAllocFn(size, nufont_srcfile, 0x86);
    if (font != 0) {
        memcpy(font, fntdata, size);
        bmp = NuTexReadBitmapMem(bmpdata);
        if (bmp != 0) {
            tid = NuTexCreate(bmp);
            if (tid != 0) {
                font->mtl = mtl = NuMtlCreate(1);
                if (mtl != 0) {
                    mtl->diffuse.r = 1.0f;
                    font->mtl->diffuse.g = 1.0f;
                    font->mtl->diffuse.b = 1.0f;
                    font->mtl->attrib.filter = 0;
                    font->mtl->attrib.cull = 2;
                    font->mtl->attrib.zmode = 3;
                    font->mtl->alpha = 1.0f;
                    font->mtl->attrib.alpha = 1;
                    font->mtl->attrib.colour = 1;
                    font->mtl->tid = tid;
                    NuMtlUpdate(font->mtl);
                    NuMemFreeFn(bmp->bits, nufont_srcfile, 0x9C);
                    NuMemFreeFn(bmp, nufont_srcfile, 0x9D);
                    return font;
                }
                NuTexDestroy(tid);
            }
            NuMemFreeFn(bmp->bits, nufont_srcfile, 0xA3);
            NuMemFreeFn(bmp, nufont_srcfile, 0xA4);
        }
    }
    NuErrorProlog(nufont_srcfile, 0xA7)(str_memfont_load_failed);
    return 0;
}


static inline s32 NuFntColumnHasInk(struct nutex_s *bmp, s32 x, s32 y, s32 h,
                                    s32 key)
{
    u8 *pixel;
    s32 i;

    pixel = bmp->bits + bmp->width * y + x;
    for (i = 0; i < h; i++) {
        if (*pixel != key) {
            return 1;
        }
        pixel += bmp->width;
    }
    return 0;
}


static inline s32 NuFntFindGlyphStart(struct nutex_s *bmp, s32 *x, s32 *y,
                                      s32 h, s32 key)
{
    for (;;) {
        if (NuFntColumnHasInk(bmp, *x, *y, h, key)) {
            return 1;
        }
        (*x)++;
        if (*x >= bmp->width) {
            *x = 0;
            *y += h;
            if (*y + h > bmp->height) {
                return 0;
            }
        }
    }
}


static inline s32 NuFntFindGlyphEnd(struct nutex_s *bmp, s32 *x, s32 *y,
                                    s32 h, s32 key)
{
    for (;;) {
        if (!NuFntColumnHasInk(bmp, *x, *y, h, key)) {
            return 1;
        }
        (*x)++;
        if (*x >= bmp->width) {
            *x = 0;
            *y += h;
            if (*y + h > bmp->height) {
                return 0;
            }
        }
    }
}


struct nufont_s *NuFntCreate(char *name, s32 height, s32 base, s32 key,
                             char *charset)
{
    struct nufontchar_s chars[256];
    struct nufontmap_s map;
    s32 x;
    s32 y;
    struct nufont_s *font;
    struct nutex_s *bmp;
    char *c;
    s32 nchars;
    s32 left;
    s32 i;
    s32 tid;

    font = 0;
    memset(chars, 0, sizeof(chars));
    memset(&map, 0, sizeof(map));
    bmp = NuTexReadBitmap(name);
    if (bmp != 0) {
        if (bmp->type != 5) {
            NuErrorProlog(nufont_srcfile, 0x105)(str_font_not_8bit);
        }
        for (i = 0; i < 256; i++) {
            if (i == key) {
                bmp->pal[key] &= 0xFFFFFF;
            } else {
                bmp->pal[i] |= 0x7F000000;
            }
        }
        y = 0;
        x = 0;
        nchars = 0;
        for (c = charset; *c != 0; c++) {
            if (*c == ' ') {
                chars[nchars].u0 = 0.0f;
                chars[nchars].v1 = 0.0f;
                chars[nchars].u0 = 0.0f;
                chars[nchars].v1 = 0.0f;
                chars[nchars].width = height >> 2;
                chars[nchars].height = height;
            } else {
                if (!NuFntFindGlyphStart(bmp, &x, &y, height, key)) {
                    NuErrorProlog(nufont_srcfile, 0x11E)(str_font_off_map);
                }
                chars[nchars].u0 = (f32)x / (f32)bmp->width;
                chars[nchars].v0 = (f32)y / (f32)bmp->height;
                left = x;
                if (!NuFntFindGlyphEnd(bmp, &x, &y, height, key)) {
                    NuErrorProlog(nufont_srcfile, 0x124)(str_font_off_map);
                }
                chars[nchars].u1 = (f32)x / (f32)bmp->width;
                chars[nchars].v1 = ((f32)y + (f32)height - 1.0f)
                                 / (f32)bmp->height;
                chars[nchars].width = x - left;
                chars[nchars].height = height;
            }
            map.index[(u8)*c] = nchars;
            nchars++;
        }
        font = NuMemAllocFn(sizeof(struct nufont_s)
                            + nchars * sizeof(struct nufontchar_s),
                            nufont_srcfile, 0x12F);
        if (font == 0) {
            NuErrorProlog(nufont_srcfile, 0x131)(str_font_alloc_failed);
        }
        font->map = map;
        memcpy(font->chars, chars, nchars * sizeof(struct nufontchar_s));
        font->height = height;
        font->base = base;
        font->nchars = nchars;
        tid = NuTexCreate(bmp);
        if (tid == 0) {
            NuErrorProlog(nufont_srcfile, 0x13B)(str_font_tex_failed);
        }
        font->mtl = NuMtlCreate(1);
        if (font->mtl == 0) {
            NuErrorProlog(nufont_srcfile, 0x13E)(str_font_mtl_failed);
        }
        font->mtl->diffuse.r = 1.0f;
        font->mtl->diffuse.g = 1.0f;
        font->mtl->diffuse.b = 1.0f;
        font->mtl->attrib.cull = 2;
        font->mtl->attrib.zmode = 3;
        font->mtl->alpha = 1.0f;
        font->mtl->attrib.alpha = 1;
        font->mtl->attrib.colour = 1;
        font->mtl->tid = tid;
        NuMtlUpdate(font->mtl);
        NuMemFreeFn(bmp->bits, nufont_srcfile, 0x149);
        NuMemFreeFn(bmp, nufont_srcfile, 0x14A);
    }
    return font;
}


s32 NuFntPrintChar(char c)
{
    struct nufont_s *font;
    struct nufontchar_s *ch;
    s32 w;
    s32 h;

    font = NuFntCur;
    if (font != 0) {
        ch = &font->chars[font->map.index[(u8)c]];
        *rndrstream_2d->cur.f++ = ch->u0;
        *rndrstream_2d->cur.f++ = ch->v0;
        *rndrstream_2d->cur.f++ = 1.0f;
        rndrstream_2d->cur.i++;
        *rndrstream_2d->cur.i++ = font_x;
        *rndrstream_2d->cur.i++ = font_y;
        *rndrstream_2d->cur.i++ = 0;
        *rndrstream_2d->cur.i++ = 0;
        *rndrstream_2d->cur.f++ = ch->u1;
        *rndrstream_2d->cur.f++ = ch->v1;
        *rndrstream_2d->cur.f++ = 1.0f;
        rndrstream_2d->cur.i++;
        w = ((ch->width - 1) << 4) * font_xscale >> 4;
        h = ((ch->height - 1) << 3) * font_yscale >> 4;
        *rndrstream_2d->cur.i++ = font_x + w;
        *rndrstream_2d->cur.i++ = font_y + h;
        *rndrstream_2d->cur.i++ = 0;
        *rndrstream_2d->cur.i++ = 0;
        return w;
    }
    return 0;
}


s32 NuFntPrintV(char *fmt, va_list args)
{
    char buf[0x400];
    char *s;
    s32 len;
    s32 left;
    s32 w;

    vsprintf(buf, fmt, args);
    if (NuFntCur != 0) {
        len = 0;
        NuRndrStreamLink(rndrstream_2d);
        NuRndrStreamAddMtl(rndrstream_2d, NuFntCur->mtl);
        rndrstream_2d->cur.b = vpDmaTag_CntEx(rndrstream_2d->cur.b, 0x10000000,
                                              0x50000000);
        rndrstream_2d->cur.b[-0xE] = 0xE;
        rndrstream_2d->cur.b -= 4;
        rndrstream_2d->cur.b = vtBeginDirect(rndrstream_2d->cur.b, 0);
        rndrstream_2d->cur.b = gsBeginGifPacket(rndrstream_2d->cur.b,
                                                0, 1, 0, 0, 0, 1, 1);
        *rndrstream_2d->cur.i++ = font_pen[0];
        *rndrstream_2d->cur.i++ = font_pen[1];
        *rndrstream_2d->cur.i++ = font_pen[2];
        *rndrstream_2d->cur.i++ = font_pen[3];
        rndrstream_2d->cur.b = gsCloseGifPacket(rndrstream_2d->cur.b);
        rndrstream_2d->cur.b = gsBeginGifPacket(rndrstream_2d->cur.b,
                                                0, 1, 1, 6, 0, 2, 2, 5);
        vsprintf(buf, fmt, args);
        left = font_x;
        for (s = buf; *s != 0; s++) {
            switch (*s) {
            case '\n':
                NuFntPrintLength = (NuFntPrintLength < len) ? len
                                                            : NuFntPrintLength;
                font_y += NuFntHeight;
                NuFntPrintHeight += NuFntHeight;
                len = 0;
                font_x = left;
                break;
            case '\r':
                NuFntPrintLength = (NuFntPrintLength < len) ? len
                                                            : NuFntPrintLength;
                font_y += NuFntHeight;
                NuFntPrintHeight += NuFntHeight;
                len = 0;
                font_x = 0;
                break;
            case '\t':
                break;
            default:
                w = NuFntPrintChar(*s);
                len += w;
                font_x += w;
                NuFntPrintLength = (NuFntPrintLength < len) ? len
                                                            : NuFntPrintLength;
                break;
            }
        }
        rndrstream_2d->cur.b = gsCloseGifPacket(rndrstream_2d->cur.b);
        rndrstream_2d->cur.b = vtCloseDirect(rndrstream_2d->cur.b);
        rndrstream_2d->cur.b = vpDmaTag_Close(rndrstream_2d->cur.b);
        rndrstream_free = vpDmaTag_Next(rndrstream_2d->cur.b, 0);
    }
    return NuFntPrintLength;
}


s32 NuFntPrint(char *fmt, ...)
{
    va_list args;
    f32 len;

    va_start(args, fmt);
    len = NuFntPrintV(fmt, args);
    va_end(args);
    return len;
}


inline s32 NuFntPrintLenV(char *fmt, va_list args)
{
    char buf[0x400];
    char *s;
    s32 len;

    len = 0;
    NuFntPrintHeight = NuFntHeight;
    NuFntPrintLength = 0;
    if (NuFntCur != 0) {
        vsprintf(buf, fmt, args);
        for (s = buf; *s != 0; s++) {
            if (*s == '\n' || *s == '\r') {
                NuFntPrintLength = (NuFntPrintLength < len) ? len
                                                            : NuFntPrintLength;
                NuFntPrintHeight += NuFntHeight;
                len = 0;
            } else {
                len += (NuFntCur->chars[NuFntCur->map.index[(u8)*s]].width << 4)
                     * font_xscale;
            }
        }
        NuFntPrintLength = (NuFntPrintLength < len) ? len : NuFntPrintLength;
    }
    NuFntPrintLength >>= 4;
    return NuFntPrintLength;
}


s32 NuFntPrintLen(char *fmt, ...)
{
    va_list args;
    s32 len;

    va_start(args, fmt);
    len = NuFntPrintLenV(fmt, args);
    va_end(args);
    return len;
}


s32 NuFntPrintEx(s32 x, s32 y, s32 align, char *fmt, ...)
{
    va_list args;
    s32 len;

    va_start(args, fmt);
    len = NuFntPrintLenV(fmt, args);
    switch (align) {
    case 0:
        font_x = x + ((PS2_VCNTR_X - (PS2_REZ_W >> 1)) << 4);
        font_y = y - NuFntBase + ((PS2_VCNTR_Y - (PS2_REZ_H >> 1)) << 4);
        break;
    case 1:
        font_x = x - len + ((PS2_VCNTR_X - (PS2_REZ_W >> 1)) << 4);
        font_y = y - NuFntBase + ((PS2_VCNTR_Y - (PS2_REZ_H >> 1)) << 4);
        break;
    case 2:
        font_x = x - (len >> 1) + ((PS2_VCNTR_X - (PS2_REZ_W >> 1)) << 4);
        font_y = y - NuFntBase + ((PS2_VCNTR_Y - (PS2_REZ_H >> 1)) << 4);
        break;
    }
    return NuFntPrintV(fmt, args);
}


inline void NuFntSet(struct nufont_s *font)
{
    if (font == 0) {
        font = system_font;
    }
    NuFntCur = font;
    NuFntHeight = (font->height << 3) * font_yscale >> 4;
    NuFntBase = (font->base << 3) * font_yscale >> 4;
}


void NuFntInit(void)
{
    if (no_default_font == 0 && font_initialised == 0) {
        font_initialised = 1;
        system_font = NuFntLoadMem(ariel16_fnt, 0x8A0, ariel16_bmp);
        NuFntSet(system_font);
    }
}


inline void NuFntDestroy(struct nufont_s *font)
{
    if (font != 0) {
        if (font->mtl != 0) {
            if (font->mtl->tid != 0) {
                NuTexDestroy(font->mtl->tid);
            }
            NuMtlDestroy(font->mtl);
        }
        NuMemFreeFn(font, nufont_srcfile, 0x30);
    }
}


void NuFntClose(void)
{
    NuFntDestroy(system_font);
    system_font = 0;
    font_initialised = 0;
}


void NuFntScale(s32 xscale, s32 yscale)
{
    font_xscale = xscale;
    font_yscale = yscale;
    NuFntHeight = (NuFntCur->height << 3) * yscale >> 4;
    NuFntBase = (NuFntCur->base << 3) * yscale >> 4;
}


void NuFntMoveAbs(s32 x, s32 y)
{
    font_x = x + ((PS2_VCNTR_X - (PS2_REZ_W >> 1)) << 4);
    font_y = y - NuFntBase + ((PS2_VCNTR_Y - (PS2_REZ_H >> 1)) << 4);
}


void NuFntMoveRel(s32 x, s32 y)
{
    font_x += x;
    font_y += y;
}


void NuFntPos(s32 *x, s32 *y)
{
    *x = font_x - ((PS2_VCNTR_X - (PS2_REZ_W >> 1)) << 4);
    *y = font_y - ((PS2_VCNTR_Y - (PS2_REZ_H >> 1)) << 4);
}


void NuFntSetPen(s32 colour)
{
    font_pen[0] = colour & 0xFF;
    font_pen[1] = (colour >> 8) & 0xFF;
    font_pen[2] = (colour >> 16) & 0xFF;
    font_pen[3] = (colour >> 24) & 0xFF;
}

