/*
 * Unit: nu3d/nutex
 *
 * Functions:
 *   0x0011c590 NuTexReadBitmapMMEx
 *   0x0011cb90 NuTexReadBitmapMem
 *   0x0011cfb8 NuTexSetRenderTarget
 *   0x0011d108 NuTexCreateEx
 *   0x0011d300 NuTexCreateStream
 *   0x0011d450 NuTexAccomodateRS
 *   0x0011d580 NuTexAccomodate
 *   0x0011d6e0 NuTexSet
 *   0x0011d848 NuTexDisplayTexturePage
 *   0x0011dbb8 NuTexRecoverNativeDataEx
 *   0x0011de90 NuTexCreateNative
 *   0x0011e2d0 NuTexWriteMaterialSettings
 *   0x0011e438 NuTexGetSysBuffSize
 *   0x0011e448 NuTexInitEx
 *   0x0011e510 NuTexInit
 *   0x0011e5d0 NuTexClose
 *   0x0011e690 NuTexReserve
 *   0x0011e700 NuTexUnReserve
 *   0x0011e730 NuTexReadBitmap
 *   0x0011e758 NuTexReadBitmapMM
 *   0x0011e778 NuTexCreate
 *   0x0011e798 NuTexDestroy
 *   0x0011e850 NuTexRef
 *   0x0011e888 NuTexUnRef
 *   0x0011e8c0 NuTexType
 *   0x0011e8e8 NuTexWidth
 *   0x0011e910 NuTexHeight
 *   0x0011e938 NuTexHasAlpha
 *   0x0011e978 NuTexAlphaBits
 *   0x0011e9c8 NuTexPixelSize
 *   0x0011ea20 NuTexImgSize
 *   0x0011ea88 NuTexImgSizeMM
 *   0x0011eb70 NuTexPalSize
 *   0x0011eba0 NuTexRecoverNativeData
 *   0x0011ebc8 NuTexReadNative
 *   0x0011ec40 NuTexGetPointers
 *   0x0011ec98 NuTexFlushTemp
 *   0x0011eca8 NuTexAssignAddr
 *   0x0011ecd0 NuTexAssignTempAddr
 *   0x0011ed20 NuTexGetReqSize
 *   0x0011ed58 NuTexCloneTempAddr
 *   0x0011ed98 NuTexPalChange
 *   0x0011edd0 NuTexPalChangeStreamTemp
 *   0x0011ee08 NuTexGetTex0
 *   0x0011ee60 NuTexSetPal
 *   0x0011ef80 NuTexSetZBuffer
 *   0x0011eff8 NuTexGetTextAddr
 */

#include "nu_libc.h"
#include "nu_types.h"

#include "nu3d/numtl.h"
#include "nu3d/nustream.h"
#include "nu3d/nutex.h"
#include "nu3d/nuvport.h"
#include "nucore/nuerror.h"
#include "numath/numathtypes.h"
#include "nups2/ps2txtr.h"

typedef union NuTexNativeHeader {
    struct {
        int width;
        int height;
        int gsused;
        u8 type;
        u8 psm;
        u8 mipcount;
        u8 tcc;
    } old;
    struct {
        u16 width;
        u16 version;
        u16 height;
        u16 pad;
        int gsused;
        u32 type : 8;
        u32 psm : 8;
        u32 uppsm : 8;
        u32 mipcount : 7;
        u32 tcc : 1;
    } cur;
} NuTexNativeHeader;

typedef struct NuTexNativePal {
    int size;
    int palsize;
    int cbw;
    int cpsm;
} NuTexNativePal;

typedef struct NuTexNativeMipOld {
    int size;
    int upsize;
    int bw;
    int pad;
} NuTexNativeMipOld;

typedef struct NuTexNativeMip {
    int size;
    int upsize;
    int tbw;
    int dbw;
    int pad;
} NuTexNativeMip;

typedef struct BmpFileHeader {
    u16 pad;
    u16 type;
    int size;
    u16 reserved1;
    u16 reserved2;
    int offbits;
} BmpFileHeader;

typedef struct BmpInfoHeader {
    int size;
    u32 width;
    u32 height;
    u16 planes;
    u16 bitcount;
    int compression;
    int imagesize;
    int xppm;
    int yppm;
    int clrused;
    int clrimportant;
} BmpInfoHeader;

typedef union BmpColour {
    u32 value;
    struct {
        u32 blue : 8;
        u32 green : 8;
        u32 red : 8;
        u32 alpha : 8;
    } c;
} BmpColour;

#define NUTEX_VALID 0x10000
#define NUTEX_NATIVE 0x20000
#define NUTEX_MAX 0x400
#define DMA_TAG_SIZE (sizeof(u64) * 2)

extern struct nutexslot_s *D_0062EBEC;
#define TexList D_0062EBEC

extern int gs_botfree;
extern int gs_top;
extern int D_00633068;
#define gs_tempfree D_00633068
extern int D_00633060;
#define gs_topfree D_00633060
extern int D_00633064;
#define gs_bot D_00633064

extern int D_0062EC04;
#define gs_reserve_sp D_0062EC04
extern int D_0067D640[256];
#define gs_reserve_stack D_0067D640

extern int D_0062EBE8;
#define nutex_initialised D_0062EBE8
extern int D_0062EBF0;
#define nutex_cnt D_0062EBF0
extern int D_0063305C;
#define nutex_max D_0063305C
extern char *D_0062EC10;
#define nutex_sysbuff D_0062EC10
extern int D_0062EBF8;
#define gs_wrap_frame D_0062EBF8
extern int D_0062EBFC;
#define gs_frame D_0062EBFC
extern int D_0062EC00;
#define gs_flush_cnt D_0062EC00
extern int nutex_alloc_cnt;

extern int numtl_force_mipmap;
extern int sys_mm_k;
extern int sys_mm_l;

extern int D_002D3F20[4];
#define nutex_wrap_mode D_002D3F20
extern int D_002D3F30[4];
#define nutex_mag_filter D_002D3F30
extern int D_002D3F40[4];
#define nutex_min_filter D_002D3F40

extern char D_00614590[];
#define nutex_srcfile D_00614590

extern void ps2videoGetFreeMem(int *bot, int *top);
extern int NuFileLoadBuffer(char *name, void *buf, int max);
extern void *NuMemAllocFn(int size, char *file, int line);
extern void NuMemFreeFn(void *ptr, char *file, int line);
extern void FlushCache(int mode);
extern int NuFileOpen(char *name, int mode);
extern void NuFileClose(int fh);
extern int NuFileRead(int fh, void *buf, int size);
extern int NuFilePos(int fh);
extern int NuFileSeek(int fh, int offset, int origin);

extern char D_0062EC18[];
#define nutex_err_assert D_0062EC18
extern char D_0062EC20[];
#define nutex_err_doh D_0062EC20
extern char D_006145B0[];
#define nutex_err_overflow D_006145B0
extern char D_006145C0[];
#define nutex_err_bitcount D_006145C0
extern char D_006146B8[];
#define nutex_err_recover D_006146B8
extern char D_00614600[];
#define nutex_err_membitcount D_00614600

extern char D_00614640[];
#define nutex_err_alloc D_00614640
extern char D_00614660[];
#define nutex_err_accomodate D_00614660
extern char D_00614690[];
#define nutex_err_set D_00614690

extern int D_0062EC30;
#define nutex_page_initialised D_0062EC30
extern struct numtl_s *D_0067D540[64];
#define nutex_page_mtl D_0067D540
extern unsigned short nutex_display_lod;

extern int nutex_use_path3;
extern int txupload_on;
extern int nurndr_pixel_width;
extern int nurndr_pixel_height;
extern int nurndr_pixel_p2w;
extern int nurndr_pixel_p2h;
extern u64 cur_rt;

extern void NuDisableVBlank(void);
extern void NuEnableVBlank(void);
extern void NuMemSetExternal(void *addr, int size);
extern int NuPs2TxCreate(struct nutex_s *spec, struct nups2tex_s *ps2tex);
extern void *NuPs2TxGenUpload(void *stream, struct nups2tex_s *ps2tex, int addr);
extern void *vpDmaTag_CntEx(void *cur, unsigned int qwc, unsigned int tag);
extern void *vpDmaTag_Next(void *cur, int mode);
extern void *vpDmaTag_Close(void *cur);
extern void *vtBeginDirect(void *cur, int irq);
extern void *vtCloseDirect(void *cur);
extern void *gsBeginGifPacket(void *cur, int eop, int nloop, int pre, int prim,
                              int flg, int nreg, ...);
extern void *gsCloseGifPacket(void *cur);
extern void NuRndrRect(struct numtlrect_s *rect);


static inline int NuTexFindFreeSlot(void) {
    int start;

    start = nutex_cnt;
    while (TexList[nutex_cnt].flags & NUTEX_VALID) {
        nutex_cnt++;
        if (start == nutex_cnt) {
            return -1;
        }
        if (nutex_cnt >= nutex_max) {
            nutex_cnt = 0;
        }
    }
    nutex_alloc_cnt++;
    return nutex_cnt;
}


inline int NuTexHasAlpha(int tex) {
    int type;

    tex--;
    if (tex >= 0) {
        type = TexList[tex].type;
        if (type == 1 || type == 3) {
            return 1;
        }
    }
    return 0;
}


struct nutex_s *NuTexReadBitmapMMEx(char *name, int mipmaps, char **buff, char **buffend) {
    BmpFileHeader file;
    BmpInfoHeader info;
    BmpColour pal[256];
    BmpColour colour;
    BmpColour *entry;
    struct nutex_s *spec;
    int fh;
    int pos;
    int palsize;
    int levels;
    int rowbytes;
    int total;
    int width;
    int height;
    int scale;
    int offset;
    int i;
    int x;
    int y;
    u32 swap;
    u8 *row;
    u8 *top;
    u8 *bottom;
    u8 *src;
    u8 *dst;

    spec = 0;
    if (!name) {
        NuErrorProlog(nutex_srcfile, 0x234)(nutex_err_assert);
    }
    fh = NuFileOpen(name, 0);
    if (fh) {
        if (buff) {
            spec = (struct nutex_s *)(((int)*buff + 15) & 0xFFFFFFF0);
            *buff = (char *)spec + sizeof(struct nutex_s);
            if (*buffend < *buff) {
                NuErrorProlog(nutex_srcfile, 0x23B)(nutex_err_overflow);
            }
        } else {
            spec = NuMemAllocFn(sizeof(struct nutex_s), nutex_srcfile, 0x23E);
        }
        if (spec) {
            memset(spec, 0, sizeof(struct nutex_s));
            pos = NuFilePos(fh);
            NuFileRead(fh, &file.type, 14);
            NuFileRead(fh, &info, sizeof(BmpInfoHeader));
            switch (info.bitcount) {
            case 32:
                spec->type = 3;
                palsize = 0;
                break;
            case 24:
                spec->type = 2;
                palsize = 0;
                break;
            case 4:
                spec->type = 4;
                palsize = 0x40;
                NuFileRead(fh, pal, 0x40);
                entry = pal;
                for (i = 0; i < 16; i++) {
                    colour = *entry;
                    swap = colour.c.blue;
                    colour.c.blue = colour.c.red;
                    colour.c.red = swap;
                    *entry++ = colour;
                }
                break;
            case 8:
                spec->type = 5;
                NuFileRead(fh, pal, 0x400);
                palsize = 0x400;
                entry = pal;
                for (i = 0; i < 256; i++) {
                    colour = *entry;
                    swap = colour.c.blue;
                    colour.c.blue = colour.c.red;
                    colour.c.red = swap;
                    *entry++ = colour;
                }
                break;
            default:
                NuErrorProlog(nutex_srcfile, 0x26E)(nutex_err_bitcount, info.bitcount, name);
                break;
            }
            levels = mipmaps + 1;
            spec->width = info.width;
            spec->height = info.height;
            spec->mmcnt = levels;
            total = 0;
            rowbytes = (spec->width * NuTexPixelSize(spec->type) + 7) / 8;
            width = spec->width;
            height = spec->height;
            i = levels;
            do {
                i--;
                total += NuTexImgSize(spec->type, width, height);
                width >>= 1;
                height >>= 1;
            } while (i);
            if (buff) {
                *buff = (char *)(((int)*buff + 15) & 0xFFFFFFF0);
                spec->bits = *buff;
                *buff += total + palsize;
                if (*buffend < *buff) {
                    NuErrorProlog(nutex_srcfile, 0x283)(nutex_err_overflow);
                }
            } else {
                spec->bits = NuMemAllocFn(total + palsize, nutex_srcfile, 0x286);
            }
            FlushCache(0);
            NuFileSeek(fh, file.offbits, pos);
            NuFileRead(fh, spec->bits, NuTexImgSize(spec->type, spec->width, spec->height));
            if (buff) {
                row = (u8 *)(((int)*buff + 15) & 0xFFFFFFF0);
                *buff = (char *)(row + rowbytes);
                if (*buffend < *buff) {
                    NuErrorProlog(nutex_srcfile, 0x28F)(nutex_err_overflow);
                }
            } else {
                row = NuMemAllocFn(rowbytes, nutex_srcfile, 0x292);
            }
            top = spec->bits;
            bottom = top + NuTexImgSize(spec->type, spec->width, spec->height) - rowbytes;
            while (top < bottom) {
                memcpy(row, top, rowbytes);
                memcpy(top, bottom, rowbytes);
                top += rowbytes;
                memcpy(bottom, row, rowbytes);
                bottom -= rowbytes;
            }
            if (!buff) {
                NuMemFreeFn(row, nutex_srcfile, 0x2A1);
            }
            scale = 2;
            dst = spec->bits;
            dst += NuTexImgSize(spec->type, spec->width, spec->height);
            while (mipmaps) {
                mipmaps--;
                src = spec->bits;
                if (spec->type == 2) {
                    for (y = 0; y < spec->height; y += scale) {
                        offset = (y * spec->width) * 3;
                        for (x = 0; x < spec->width; x += scale) {
                            *dst++ = src[offset + x * 3];
                            *dst++ = src[offset + x * 3 + 1];
                            *dst++ = src[offset + x * 3 + 2];
                        }
                    }
                } else {
                    NuErrorProlog(nutex_srcfile, 0x2B6)(nutex_err_doh);
                }
                scale *= 2;
            }
            if (palsize) {
                spec->pal = (u8 *)spec->bits + total;
                memcpy(spec->pal, pal, palsize);
            }
        }
        NuFileClose(fh);
    }
    return spec;
}


struct nutex_s *NuTexReadBitmapMem(u8 *mem) {
    BmpFileHeader file;
    BmpInfoHeader info;
    u8 pal[0x400];
    struct nutex_s *spec;
    u8 *src;
    int palsize;
    int maxdim;
    int shift;
    int rowbytes;
    int size;
    u8 *row;
    u8 *top;
    u8 *bottom;

    spec = NuMemAllocFn(sizeof(struct nutex_s), nutex_srcfile, 0x2E9);
    if (spec) {
        memset(spec, 0, sizeof(struct nutex_s));
        src = mem;
        memcpy(&file.type, src, 14);
        src += 14;
        memcpy(&info, src, sizeof(BmpInfoHeader));
        src += sizeof(BmpInfoHeader);
        switch (info.bitcount) {
        case 32:
            spec->type = 3;
            palsize = 0;
            break;
        case 24:
            spec->type = 2;
            palsize = 0;
            break;
        case 4:
            spec->type = 4;
            memcpy(pal, src, 0x40);
            palsize = 0x40;
            break;
        case 8:
            spec->type = 5;
            palsize = 0x400;
            memcpy(pal, src, 0x400);
            break;
        default:
            NuErrorProlog(nutex_srcfile, 0x310)(nutex_err_membitcount, info.bitcount);
            break;
        }
        maxdim = info.height < info.width ? info.width : info.height;
        for (shift = 31; !((maxdim >> shift) & 1); shift--) {
        }
        spec->width = info.width;
        spec->height = info.height;
        spec->mmcnt = 0;
        rowbytes = (spec->width * NuTexPixelSize(spec->type) + 7) / 8;
        size = NuTexImgSize(spec->type, spec->width, spec->height);
        spec->bits = NuMemAllocFn(size + palsize, nutex_srcfile, 0x31F);
        FlushCache(0);
        src = mem + file.offbits;
        memcpy(spec->bits, src, size);
        row = NuMemAllocFn(rowbytes, nutex_srcfile, 0x325);
        top = spec->bits;
        bottom = top + NuTexImgSize(spec->type, spec->width, spec->height) - rowbytes;
        while (top < bottom) {
            memcpy(row, top, rowbytes);
            memcpy(top, bottom, rowbytes);
            top += rowbytes;
            memcpy(bottom, row, rowbytes);
            bottom -= rowbytes;
        }
        NuMemFreeFn(row, nutex_srcfile, 0x32F);
        if (palsize) {
            spec->pal = (u8 *)spec->bits + NuTexImgSize(spec->type, spec->width, spec->height);
            memcpy(spec->pal, pal, palsize);
        }
    }
    return spec;
}


u8 *NuTexSetRenderTarget(u8 *stream, int tex) {
    struct nutexslot_s *entry;
    int addr;
    u64 *gif;

    if (TexList[tex - 1].flags & NUTEX_VALID) {
        entry = &TexList[tex - 1];
        stream = vpDmaTag_CntEx(stream, 0x13000000, 0x50000000);
        stream[-0xE] = 10;
        gif = gsBeginGifPacket(vtBeginDirect(stream - 4, 0), 0, 1, 0, 0, 0, 1, 0xE);
        addr = entry->ps2tex.tbp[0];
        if (!addr) {
            addr = entry->ps2tex.gsaddr;
            if (!addr) {
                addr = gs_botfree;
            }
        }
        *gif++ = 0;
        *gif++ = 0x3F;
        *gif++ = (addr >> 5) | (u64)entry->ps2tex.tbw[0] << 16 | (u64)entry->ps2tex.psm << 24;
        *gif++ = 0x4C;
        stream = vpDmaTag_Close(vtCloseDirect(gsCloseGifPacket(gif)));
        nurndr_pixel_width = entry->width;
        nurndr_pixel_height = entry->height;
        nurndr_pixel_p2w = entry->ps2tex.tw;
        nurndr_pixel_p2h = entry->ps2tex.th;
    }
    cur_rt = (addr >> 5) | (u64)entry->ps2tex.tbw[0] << 16 | (u64)entry->ps2tex.psm << 24;
    return stream;
}


int NuTexCreateEx(struct nutex_s *spec, void *extmem, int extsize) {
    int slot;

    NuDisableVBlank();
    slot = NuTexFindFreeSlot();
    if (slot >= 0) {
        memset(&TexList[slot], 0, sizeof(struct nutexslot_s));
        if (spec) {
            TexList[slot].spec = *spec;
        }
        if (extmem) {
            NuMemSetExternal(extmem, extsize);
        }
        if (NuPs2TxCreate(&TexList[slot].spec, &TexList[slot].ps2tex)) {
            TexList[slot].bits = 0;
            TexList[slot].valid = 1;
            if (extmem) {
                TexList[slot].native = 1;
                NuMemSetExternal(0, 0);
            } else {
                TexList[slot].native = (spec == 0);
            }
            NuEnableVBlank();
            return slot + 1;
        }
        if (extmem) {
            NuMemSetExternal(0, 0);
        }
    }
    NuEnableVBlank();
    NuErrorProlog(nutex_srcfile, 0x3CF)(nutex_err_alloc);
    return 0;
}


int NuTexCreateStream(struct nutex_s *spec) {
    int slot;

    slot = NuTexFindFreeSlot();
    if (slot >= 0) {
        TexList[slot].spec = *spec;
        if (NuPs2TxCreate(spec, &TexList[slot].ps2tex)) {
            TexList[slot].bits = 0;
            TexList[slot].valid = 1;
            TexList[slot].native = (spec == 0);
            return slot + 1;
        }
    }
    return 0;
}


void NuTexAccomodateRS(struct nurndrstream_s *rs, int tex) {
    u32 *prev;

    gs_topfree = rs->zbuf;
    if (nutex_use_path3) {
        prev = rs->cur.w;
        rs->cur.b = vpDmaTag_Next(prev, 0);
        rs->cur.b[3 - DMA_TAG_SIZE] |= 0x80;
        rs->cur.b[-0xE] = 0x18;
        rs->cur.w[0] = 0;
        rs->cur.w[1] = 0xACEB00B5;
        rs->cur.b += 16;
        if (rs->txtr_end) {
            rs->txtr_end[1] = (u32)rs->cur.b;
        } else {
            rs->txtr_start = rs->cur.w;
        }
        rs->cur.b = NuTexAccomodate(rs->cur.b, tex);
        rs->txtr_end = rs->cur.w;
        rs->cur.b = vpDmaTag_Next(rs->cur.b, 0);
        rs->cur.b[3 - DMA_TAG_SIZE] |= 0x80;
        rs->cur.b[-0xE] = 0x19;
        rs->cur.w[0] = (u32)prev;
        rs->cur.w[1] = 0xBABEF00D;
        rs->cur.b += 16;
        prev[1] = (u32)rs->cur.b;
    } else {
        rs->cur.b = NuTexAccomodate(rs->cur.b, tex);
    }
    rs->zbuf = gs_topfree;
}


u8 *NuTexAccomodate(u8 *stream, int tex) {
    struct nutexslot_s *entry;
    struct nups2tex_s *ps2tex;

    if (tex > 0) {
        if (txupload_on) {
            entry = &TexList[tex - 1];
            if (entry->flags & NUTEX_VALID) {
                if (entry->type < 8U) {
                    ps2tex = &entry->ps2tex;
                    if (gs_topfree - ps2tex->gsused < gs_botfree) {
                        gs_topfree = gs_top;
                        gs_wrap_frame = gs_frame;
                    }
                    stream = NuPs2TxGenUpload(stream, ps2tex, gs_topfree);
                    ps2tex->uploaded = 1;
                    gs_topfree -= ps2tex->gsused;
                }
            } else {
                NuErrorProlog(nutex_srcfile, 0x530)(nutex_err_accomodate);
                return stream;
            }
        } else {
            entry = &TexList[tex - 1];
            if (entry->flags & NUTEX_VALID) {
                if (entry->type >= 11U) {
                    return stream;
                }
                ps2tex = &entry->ps2tex;
                gs_wrap_frame = gs_frame;
            }
            gs_topfree = gs_top;
            stream = NuPs2TxUploadPal(stream, ps2tex, gs_topfree);
            ps2tex->uploaded = 1;
        }
    }
    return stream;
}


u8 *NuTexSet(u8 *stream, int tex, char context) {
    struct nups2tex_s *ps2tex;
    u64 *gif;

    if (tex > 0) {
        if (TexList[tex - 1].flags & NUTEX_VALID) {
            ps2tex = &TexList[tex - 1].ps2tex;
            stream = vpDmaTag_CntEx(stream, 0x13000000, 0x50000000);
            stream[-0xE] = 2;
            gif = gsBeginGifPacket(vtBeginDirect(stream - 4, 0), 0, 1, 0, 0, 0, 1, 0xE);
            gif = Ps2TxSetStateData(gif, ps2tex, 0, context);
            stream = vpDmaTag_Close(vtCloseDirect(gsCloseGifPacket(gif)));
        } else {
            NuErrorProlog(nutex_srcfile, 0x57A)(nutex_err_set);
        }
    } else if (tex < 0) {
        stream = vpDmaTag_CntEx(stream, 0x11000000, 0x50000000);
        gif = gsBeginGifPacket(vtBeginDirect(stream - 4, 0), 0, 1, 0, 0, 0, 1, 0xE);
        gif = Ps2TxSetSurfaceStateData(gif, tex);
        stream = vpDmaTag_Close(vtCloseDirect(gsCloseGifPacket(gif)));
    }
    return stream;
}


void NuTexDisplayTexturePage(int page, float depth) {
    struct numtlrect_s rect;
    struct nutexslot_s *entry;
    float x;
    float y;
    float w;
    float h;
    float rowh;
    int count;
    int i;

    if (!nutex_page_initialised) {
        for (count = 0; count < 64; count++) {
            nutex_page_mtl[count] = 0;
        }
        nutex_page_initialised = 1;
    }
    x = 0.1f;
    y = x;
    count = 0;
    i = 0;
    rowh = 0.0f;
    for (; page >= 0 && i < nutex_max; i++) {
        entry = &TexList[i];
        if (entry->flags & NUTEX_VALID) {
            w = NuVpVirtualWidth((float)(entry->width << 4));
            h = NuVpVirtualHeight((float)(entry->height << 3));
            rowh = rowh < h ? h : rowh;
            if (x + w >= 0.9f) {
                y += rowh + 0.01f;
                x = 0.1f;
                rowh = 0.0f;
            }
            if (y + h >= 0.9f || count >= 64) {
                y = 0.1f;
                count = 0;
                page--;
            }
            rowh = rowh < h ? h : rowh;
            if (page == 0) {
                if (!nutex_page_mtl[count]) {
                    nutex_page_mtl[count] = NuMtlCreate(1);
                }
                nutex_page_mtl[count]->attrib.alpha = NuTexHasAlpha(i + 1);
                nutex_page_mtl[count]->attrib.filter = 2;
                nutex_page_mtl[count]->attrib.zmode = 3;
                nutex_page_mtl[count]->attrib.colour = 1;
                nutex_page_mtl[count]->ambient.r = nutex_page_mtl[count]->ambient.g =
                    nutex_page_mtl[count]->ambient.b = 1.0f;
                nutex_page_mtl[count]->diffuse.r = nutex_page_mtl[count]->diffuse.g =
                    nutex_page_mtl[count]->diffuse.b = 1.0f;
                nutex_page_mtl[count]->tid = i + 1;
                nutex_page_mtl[count]->L = 0;
                nutex_page_mtl[count]->K = nutex_display_lod << 4;
                NuMtlUpdate(nutex_page_mtl[count]);
                rect.x = x;
                rect.y = y;
                rect.w = w;
                rect.h = h;
                rect.z = depth;
                rect.u0 = rect.v0 = 0.0f;
                rect.u1 = rect.v1 = 1.0f;
                rect.mtl = nutex_page_mtl[count++];
                NuRndrRect(&rect);
            }
            x += w + 0.01f;
        }
    }
}


int NuTexRecoverNativeDataEx(int tex, u8 *dest, int size, int header, int palette, int image) {
    struct nutexslot_s *entry;
    struct nups2tex_s *ps2tex;
    NuTexNativeHeader *hdr;
    NuTexNativePal *pal;
    NuTexNativeMip *mip;
    u8 *end;
    u8 *p;
    int level;

    if (TexList[tex - 1].flags & NUTEX_VALID) {
        end = dest + size;
        entry = &TexList[tex - 1];
        ps2tex = &entry->ps2tex;
        if (header) {
            hdr = (NuTexNativeHeader *)dest;
            hdr->cur.version = 1;
            hdr->cur.pad = 0;
            hdr->cur.width = entry->width;
            hdr->cur.height = entry->height;
            hdr->cur.type = entry->type;
            hdr->cur.psm = ps2tex->psm;
            hdr->cur.uppsm = ps2tex->uppsm;
            hdr->cur.gsused = ps2tex->gsused;
            hdr->cur.mipcount = ps2tex->mipcount;
            hdr->cur.tcc = ps2tex->tcc;
            p = dest + sizeof(NuTexNativeHeader);
        }
        if (palette && entry->type - 4 < 4U) {
            pal = (NuTexNativePal *)p;
            p += sizeof(NuTexNativePal);
            pal->size = (ps2tex->palstreamsize + 15) & 0xFFFFFFF0;
            pal->palsize = ps2tex->palsize;
            pal->cbw = ps2tex->cbw;
            pal->cpsm = ps2tex->cpsm;
            memcpy(p, ps2tex->paldata, pal->size);
            p += pal->size;
            pal->size += sizeof(NuTexNativePal);
            if (p >= end) {
                NuErrorProlog(nutex_srcfile, 0x67D)(nutex_err_recover);
            }
        }
        if (image) {
            for (level = 0; level < ps2tex->mipcount; level++) {
                mip = (NuTexNativeMip *)p;
                p += sizeof(NuTexNativeMip);
                mip->size = (ps2tex->reqsize[level] + 15) & 0xFFFFFFF0;
                mip->upsize = ps2tex->upsize[level];
                mip->tbw = ps2tex->tbw[level];
                mip->dbw = ps2tex->dbw[level];
                mip->pad = (((int)p + 0x7F) & 0xFFFFFF80) - (int)p;
                if (nutex_use_path3) {
                    if (!mip->pad) {
                        mip->pad = 0x80;
                    }
                    mip->pad -= 16;
                }
                if (mip->pad) {
                    memset(p, 0, mip->pad);
                    p += mip->pad;
                }
                memcpy(p, ps2tex->mipdata[level], mip->size);
                p += mip->size;
                mip->size += sizeof(NuTexNativeMip);
                if (p >= end) {
                    NuErrorProlog(nutex_srcfile, 0x69E)(nutex_err_recover);
                }
            }
        }
        return p - dest;
    }
    return 0;
}


static inline struct nutexslot_s *NuTexMarkNative(struct nutexslot_s *entry) {
    entry->valid = 1;
    entry->native = 1;
    return entry;
}


int NuTexCreateNative(void *data) {
    NuTexNativeHeader *hdr;
    NuTexNativePal *pal;
    NuTexNativeMipOld *oldmip;
    NuTexNativeMip *mip;
    struct nutexslot_s *entry;
    struct nups2tex_s *ps2tex;
    u8 *chunk;
    int slot;
    int level;
    int size;
    int shift;

    hdr = data;
    slot = NuTexFindFreeSlot();
    if (slot >= 0) {
        if (hdr->cur.version == 0) {
            entry = NuTexMarkNative(&TexList[slot]);
            ps2tex = &entry->ps2tex;
            chunk = (u8 *)hdr + sizeof(NuTexNativeHeader);
            entry->type = hdr->old.type;
            entry->width = hdr->old.width;
            entry->height = hdr->old.height;
            entry->mmcnt = hdr->old.mipcount - 1;
            entry->bits = 0;
            entry->pal = 0;
            memset(ps2tex, 0, sizeof(struct nups2tex_s));
            ps2tex->mipcount = hdr->old.mipcount;
            ps2tex->psm = hdr->old.psm;
            ps2tex->uppsm = hdr->old.psm;
            ps2tex->tcc = hdr->old.tcc;
            ps2tex->gsused = hdr->old.gsused;
            if (entry->type - 4 < 4U) {
                pal = (NuTexNativePal *)chunk;
                ps2tex->paldata = (u8 *)pal + sizeof(NuTexNativePal);
                ps2tex->palsize = pal->palsize;
                ps2tex->cbw = pal->cbw;
                ps2tex->cpsm = pal->cpsm;
                ps2tex->palstreamsize = pal->size - sizeof(NuTexNativePal);
                chunk += pal->size;
            }
            for (level = 0; level < hdr->old.mipcount; level++) {
                oldmip = (NuTexNativeMipOld *)chunk;
                ps2tex->mipdata[level] = chunk + (oldmip->pad + sizeof(NuTexNativeMipOld));
                ps2tex->upsize[level] = oldmip->upsize;
                ps2tex->tbw[level] = oldmip->bw;
                ps2tex->dbw[level] = oldmip->bw;
                ps2tex->reqsize[level] = oldmip->size - sizeof(NuTexNativeMipOld);
                chunk += oldmip->size + oldmip->pad;
            }
            shift = 0;
            for (size = entry->width; size >= 2; size >>= 1) {
                shift++;
            }
            ps2tex->tw = shift;
            shift = 0;
            for (size = entry->height; size >= 2; size >>= 1) {
                shift++;
            }
            ps2tex->th = shift;
            return slot + 1;
        }
        if (hdr->cur.version == 1) {
            TexList[slot].valid = 1;
            TexList[slot].native = 1;
            entry = &TexList[slot];
            ps2tex = &entry->ps2tex;
            chunk = (u8 *)hdr + sizeof(NuTexNativeHeader);
            entry->type = hdr->cur.type;
            entry->width = hdr->cur.width;
            entry->height = hdr->cur.height;
            entry->mmcnt = hdr->cur.mipcount - 1;
            entry->bits = 0;
            entry->pal = 0;
            memset(ps2tex, 0, sizeof(struct nups2tex_s));
            ps2tex->mipcount = hdr->cur.mipcount;
            ps2tex->psm = hdr->cur.psm;
            ps2tex->uppsm = hdr->cur.uppsm;
            ps2tex->tcc = hdr->cur.tcc;
            ps2tex->gsused = hdr->cur.gsused;
            if (entry->type - 4 < 4U) {
                pal = (NuTexNativePal *)chunk;
                ps2tex->paldata = (u8 *)pal + sizeof(NuTexNativePal);
                ps2tex->palsize = pal->palsize;
                ps2tex->cbw = pal->cbw;
                ps2tex->cpsm = pal->cpsm;
                ps2tex->palstreamsize = pal->size - sizeof(NuTexNativePal);
                chunk += pal->size;
            }
            for (level = 0; level < hdr->cur.mipcount; level++) {
                mip = (NuTexNativeMip *)chunk;
                ps2tex->mipdata[level] = chunk + (mip->pad + sizeof(NuTexNativeMip));
                ps2tex->upsize[level] = mip->upsize;
                ps2tex->tbw[level] = mip->tbw;
                ps2tex->dbw[level] = mip->dbw;
                ps2tex->reqsize[level] = mip->size - sizeof(NuTexNativeMip);
                chunk += mip->size + mip->pad;
            }
            shift = 0;
            for (size = entry->width; size >= 2; size >>= 1) {
                shift++;
            }
            ps2tex->tw = shift;
            shift = 0;
            for (size = entry->height; size >= 2; size >>= 1) {
                shift++;
            }
            ps2tex->th = shift;
            return slot + 1;
        }
    }
    return 0;
}


u64 *NuTexWriteMaterialSettings(u64 *dst, struct numtl_s *mtl) {
    int k;
    int l;
    int filter;
    int maxlevel;
    struct nups2tex_s *ps2tex;

    if (numtl_force_mipmap) {
        k = sys_mm_k;
        l = sys_mm_l;
    } else {
        k = mtl->K;
        l = mtl->L;
    }
    filter = (int)(mtl->attrib_reg >> 36) & 3;
    if (mtl->tid > 0) {
        ps2tex = &TexList[mtl->tid - 1].ps2tex;
        if (filter == 2) {
            maxlevel = ps2tex->mipcount - 1;
        } else {
            maxlevel = 0;
        }
    } else {
        maxlevel = 0;
    }
    *dst++ = 1;
    *dst++ = 0x3F;
    *dst++ = (u64)(maxlevel >= 0 ? maxlevel : 0) << 2 | (u64)nutex_mag_filter[filter] << 5
           | (u64)nutex_min_filter[filter] << 6 | (u64)l << 19 | (u64)k << 32;
    if (mtl->ctxt) {
        *dst++ = 0x15;
    } else {
        *dst++ = 0x14;
    }
    *dst++ = nutex_wrap_mode[(int)(mtl->attrib_reg >> 40) & 3] | (u64)nutex_wrap_mode[(int)(mtl->attrib_reg >> 42) & 3] << 2;
    if (mtl->ctxt) {
        *dst++ = 9;
    } else {
        *dst++ = 8;
    }
    return dst;
}


int NuTexGetSysBuffSize(int count) {
    return count * sizeof(struct nutexslot_s);
}


inline void NuTexInitEx(char **buff, int count) {
    int size;
    struct nutexslot_s *list;

    if (nutex_initialised) {
        NuTexClose();
    }
    ps2videoGetFreeMem(&gs_bot, 0);
    size = count * sizeof(struct nutexslot_s);
    list = (struct nutexslot_s *)(((int)*buff + 15) & 0xFFFFFFF0);
    gs_botfree = gs_bot;
    gs_top = 0x4000;
    gs_topfree = 0x4000;
    gs_flush_cnt = 0;
    gs_frame = 0;
    gs_wrap_frame = 0;
    *buff = (char *)list;
    TexList = list;
    memset(list, 0, size);
    *buff += size;
    nutex_max = count;
    nutex_initialised = 1;
    nutex_cnt = 0;
    nutex_alloc_cnt = 0;
}


void NuTexInit(void) {
    char *buff;

    if (!nutex_sysbuff) {
        nutex_sysbuff = NuMemAllocFn(NUTEX_MAX * sizeof(struct nutexslot_s), nutex_srcfile, 0x1A2);
    }
    buff = nutex_sysbuff;
    NuTexInitEx(&buff, NUTEX_MAX);
}


void NuTexClose(void) {
    int i;

    for (i = 0; i < nutex_max; i++) {
        if (TexList[i].bits) {
            NuMemFreeFn(TexList[i].bits, nutex_srcfile, 0x1B0);
            TexList[i].bits = 0;
        }
        if (TexList[i].flags & NUTEX_VALID) {
            NuTexDestroy(i + 1);
        }
    }
    nutex_cnt = 0;
    nutex_alloc_cnt = 0;
    nutex_initialised = 0;
}


int NuTexReserve(int size) {
    int addr;

    if (gs_reserve_sp >= 256) {
        return 0;
    }
    size += 255;
    gs_reserve_stack[gs_reserve_sp++] = gs_botfree;
    addr = (gs_botfree + 31) & 0xFFFFFFE0;
    gs_botfree = addr + size / 256;
    return addr;
}


void NuTexUnReserve(void) {
    if (gs_reserve_sp) {
        gs_reserve_sp--;
        gs_botfree = gs_reserve_stack[gs_reserve_sp];
    }
}


struct nutex_s *NuTexReadBitmap(char *name) {
    return NuTexReadBitmapMMEx(name, 0, 0, 0);
}


struct nutex_s *NuTexReadBitmapMM(char *name, int mipmaps) {
    return NuTexReadBitmapMMEx(name, mipmaps, 0, 0);
}


int NuTexCreate(struct nutex_s *spec) {
    return NuTexCreateEx(spec, 0, 0);
}


void NuTexDestroy(int tex) {
    tex--;
    if (tex >= 0) {
        if (!(TexList[tex].flags & NUTEX_NATIVE)) {
            if (TexList[tex].bits) {
                NuMemFreeFn(TexList[tex].bits, nutex_srcfile, 0x3F9);
                TexList[tex].bits = 0;
            }
            NuPs2TxDestroy(&TexList[tex].ps2tex);
        }
        TexList[tex].flags &= ~NUTEX_VALID;
        nutex_alloc_cnt--;
    }
}


short NuTexRef(int tex) {
    tex--;
    if (tex >= 0) {
        return ++TexList[tex].refcount;
    }
    return 0;
}


short NuTexUnRef(int tex) {
    tex--;
    if (tex >= 0) {
        return --TexList[tex].refcount;
    }
    return 0;
}


int NuTexType(int tex) {
    tex--;
    if (tex >= 0) {
        return TexList[tex].type;
    }
    return 0;
}


int NuTexWidth(int tex) {
    tex--;
    if (tex >= 0) {
        return TexList[tex].width;
    }
    return 0;
}


int NuTexHeight(int tex) {
    tex--;
    if (tex >= 0) {
        return TexList[tex].width;
    }
    return 0;
}


int NuTexAlphaBits(int type) {
    if (type == 1) goto b4;
    if (type == 0) goto b0;
    if (type == 2) goto b0;
    if (type == 3) goto b8;
    goto def;
b0:
    return 0;
b4:
    return 4;
b8:
    return 8;
def:
    return 0;
}


inline int NuTexPixelSize(int type) {
    switch (type) {
    case 4:
        return 4;
    case 5:
        return 8;
    case 0:
    case 1:
        return 16;
    case 2:
        return 24;
    case 3:
        return 32;
    }
    return 0;
}


inline int NuTexImgSize(int type, int width, int height) {
    return (width * height * NuTexPixelSize(type)) >> 3;
}


int NuTexImgSizeMM(int type, int width, int height, int levels) {
    int size;
    int level;

    size = NuTexImgSize(type, width, height);
    level = 0;
    while (level < levels) {
        level++;
        size += ((width >> level) * (height >> level) * NuTexPixelSize(type)) >> 3;
    }
    return size;
}


int NuTexPalSize(int type) {
    switch (type) {
    case 5:
        return 0x400;
    case 4:
        return 0x40;
    }
    return 0;
}


int NuTexRecoverNativeData(int tex, u8 *dest, int size) {
    return NuTexRecoverNativeDataEx(tex, dest, size, 1, 1, 1);
}


int NuTexReadNative(char *name, char *buff, int size, int *loaded) {
    int pad;
    int len;

    pad = -((int)buff & 15) & 15;
    buff += pad;
    len = pad + NuFileLoadBuffer(name, buff, size - pad);
    if (len) {
        if (loaded) {
            *loaded = len;
        }
        return NuTexCreateNative(buff);
    }
    return 0;
}


int NuTexGetPointers(int tex, struct nutexslot_s **entry, struct nups2tex_s **ps2tex) {
    tex--;
    if (tex >= 0 && (TexList[tex].flags & NUTEX_VALID)) {
        if (entry) {
            *entry = &TexList[tex];
        }
        if (ps2tex) {
            *ps2tex = &TexList[tex].ps2tex;
        }
        return 1;
    }
    return 0;
}


void NuTexFlushTemp(void) {
    gs_tempfree = gs_botfree;
}


void NuTexAssignAddr(int tex, int addr) {
    if (tex > 0) {
        tex--;
        TexList[tex].ps2tex.gsaddr = addr;
    }
}


void NuTexAssignTempAddr(int tex) {
    if (tex > 0) {
        tex--;
        gs_tempfree = (gs_tempfree + 31) & 0xFFFFFFE0;
        TexList[tex].ps2tex.gsaddr = gs_tempfree;
        gs_tempfree += TexList[tex].ps2tex.reqsize[0];
    }
}


int NuTexGetReqSize(int tex, int level) {
    if (tex > 0) {
        tex--;
        return TexList[tex].ps2tex.reqsize[level] << 8;
    }
    return 0;
}


void NuTexCloneTempAddr(int dst, int src) {
    if (dst > 0 && src > 0) {
        dst--;
        src--;
        TexList[dst].ps2tex.gsaddr = TexList[src].ps2tex.gsaddr;
    }
}


void NuTexPalChange(int tex) {
    if (tex > 0) {
        /* Retail bug: called without the palette argument, which the
         * callee reads from $a1. */
        ((void (*)(struct nups2tex_s *))NuPs2ChangeTexPal)(&TexList[tex - 1].ps2tex);
    }
}


void NuTexPalChangeStreamTemp(int tex) {
    if (tex > 0) {
        /* Retail bug: called without the palette argument, which the
         * callee reads from $a1. */
        ((void (*)(struct nups2tex_s *))NuPs2ChangeTexPalStreamTemp)(&TexList[tex - 1].ps2tex);
    }
}


u64 NuTexGetTex0(int tex) {
    tex--;
    if (tex >= 0 && (TexList[tex].flags & NUTEX_VALID)) {
        return Ps2TxGetTex0(&TexList[tex].ps2tex);
    }
    return 0;
}


u8 *NuTexSetPal(u8 *stream, int tex, int paltex, char context) {
    struct nups2tex_s *ps2tex;
    struct nups2tex_s *palette;
    u64 *gif;

    if (tex > 0 && paltex > 0) {
        if (TexList[tex - 1].flags & NUTEX_VALID) {
            ps2tex = &TexList[tex - 1].ps2tex;
            palette = &TexList[paltex - 1].ps2tex;
            stream = vpDmaTag_CntEx(stream, 0x13000000, 0x50000000);
            stream[-0xE] = 2;
            gif = gsBeginGifPacket(vtBeginDirect(stream - 4, 0), 0, 1, 0, 0, 0, 1, 0xE);
            gif = Ps2TxSetStateData(gif, ps2tex, palette, context);
            stream = vpDmaTag_Close(vtCloseDirect(gsCloseGifPacket(gif)));
        } else {
            NuErrorProlog(nutex_srcfile, 0x5A4)(nutex_err_set);
        }
    }
    return stream;
}


void *NuTexSetZBuffer(void *stream, int tex) {
    struct nutexslot_s *entry;
    int addr;

    if (TexList[tex - 1].flags & NUTEX_VALID) {
        entry = &TexList[tex - 1];
        addr = entry->ps2tex.tbp[0];
        if (!addr) {
            addr = gs_botfree;
        }
        NuRndrStreamSetZBState(addr, entry->ps2tex.psm, 0);
        stream = NuRndrStreamAddZBState(stream, 1);
    }
    return stream;
}


int NuTexGetTextAddr(int tex) {
    struct nutexslot_s *entry;
    int addr;

    if (TexList[tex - 1].flags & NUTEX_VALID) {
        entry = &TexList[tex - 1];
        addr = entry->ps2tex.tbp[0];
        if (!addr) {
            addr = gs_botfree;
        }
        return addr;
    }
    return 0;
}
