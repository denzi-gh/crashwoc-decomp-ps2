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

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef long long s64;
typedef unsigned long long u64;

typedef struct NuPs2Tex {
    u8 unk000[0x30];
    int reqsize[10];
    u8 unk058[0x10];
    u16 texaddr;
    u8 unk06A[0x28];
    u8 mipcount;
    u8 psm;
    u8 unk094[0x18];
    int gsaddr;
    u8 unk0B0[0x10];
} NuPs2Tex;

typedef struct NuTex {
    int type;
    int width;
    u8 unk008[0x8];
    void *bits;
    u8 unk014[0x4];
    union {
        u16 refcount;
        s64 flags;
    };
    NuPs2Tex ps2tex;
} NuTex;

typedef struct NuMtl {
    u8 unk000[0x168];
    u64 attrib;
    u8 unk170[0x34];
    int tid;
    int alpha;
    short mm_k;
    u8 mm_l;
    u8 unk1AF[0x15];
    s8 gs_context;
} NuMtl;

#define NUTEX_VALID 0x10000
#define NUTEX_NATIVE 0x20000
#define NUTEX_MAX 0x400

extern NuTex *D_0062EBEC;
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

extern int NuTexReadBitmapMMEx(char *name, int mipmaps, int *addr, int *addrend);
extern int NuTexCreateEx(NuTex *spec, void *extmem, int extsize);
extern int NuTexCreateNative(void *data);
extern int NuTexRecoverNativeDataEx(int tex, void *dest, int size, int header, int image, int palette);
extern void NuTexClose(void);
extern void NuPs2ChangeTexPal(NuPs2Tex *ps2tex);
extern void NuPs2ChangeTexPalStreamTemp(NuPs2Tex *ps2tex);
extern void NuPs2TxDestroy(NuPs2Tex *ps2tex);
extern u64 Ps2TxGetTex0(NuPs2Tex *ps2tex);
extern void NuRndrStreamSetZBState(int addr, int psm, int test);
extern void *NuRndrStreamAddZBState(void *stream, int enable);
extern void ps2videoGetFreeMem(int *bot, int *top);
extern int NuFileLoadBuffer(char *name, void *buf, int max);
extern void *NuMemAllocFn(int size, char *file, int line);
extern void NuMemFreeFn(void *ptr, char *file, int line);
extern void *memset(void *dst, int value, unsigned int size);


u64 *NuTexWriteMaterialSettings(u64 *dst, NuMtl *mtl) {
    int k;
    int l;
    int filter;
    int maxlevel;
    NuPs2Tex *ps2tex;

    if (numtl_force_mipmap) {
        k = sys_mm_k;
        l = sys_mm_l;
    } else {
        k = mtl->mm_k;
        l = mtl->mm_l;
    }
    filter = (int)(mtl->attrib >> 36) & 3;
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
    if (mtl->gs_context) {
        *dst++ = 0x15;
    } else {
        *dst++ = 0x14;
    }
    *dst++ = nutex_wrap_mode[(int)(mtl->attrib >> 40) & 3] | (u64)nutex_wrap_mode[(int)(mtl->attrib >> 42) & 3] << 2;
    if (mtl->gs_context) {
        *dst++ = 9;
    } else {
        *dst++ = 8;
    }
    return dst;
}


int NuTexGetSysBuffSize(int count) {
    return count * sizeof(NuTex);
}


inline void NuTexInitEx(char **buff, int count) {
    int size;
    NuTex *list;

    if (nutex_initialised) {
        NuTexClose();
    }
    ps2videoGetFreeMem(&gs_bot, 0);
    size = count * sizeof(NuTex);
    list = (NuTex *)(((int)*buff + 15) & 0xFFFFFFF0);
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
        nutex_sysbuff = NuMemAllocFn(NUTEX_MAX * sizeof(NuTex), nutex_srcfile, 0x1A2);
    }
    buff = nutex_sysbuff;
    NuTexInitEx(&buff, NUTEX_MAX);
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


int NuTexReadBitmap(char *name) {
    return NuTexReadBitmapMMEx(name, 0, 0, 0);
}


int NuTexReadBitmapMM(char *name, int mipmaps) {
    return NuTexReadBitmapMMEx(name, mipmaps, 0, 0);
}


int NuTexCreate(NuTex *spec) {
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


int NuTexHasAlpha(int tex) {
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


int NuTexRecoverNativeData(int tex, void *dest, int size) {
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


int NuTexGetPointers(int tex, NuTex **entry, NuPs2Tex **ps2tex) {
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
        NuPs2ChangeTexPal(&TexList[tex - 1].ps2tex);
    }
}


void NuTexPalChangeStreamTemp(int tex) {
    if (tex > 0) {
        NuPs2ChangeTexPalStreamTemp(&TexList[tex - 1].ps2tex);
    }
}


u64 NuTexGetTex0(int tex) {
    tex--;
    if (tex >= 0 && (TexList[tex].flags & NUTEX_VALID)) {
        return Ps2TxGetTex0(&TexList[tex].ps2tex);
    }
    return 0;
}


void *NuTexSetZBuffer(void *stream, int tex) {
    NuTex *entry;
    int addr;

    if (TexList[tex - 1].flags & NUTEX_VALID) {
        entry = &TexList[tex - 1];
        addr = entry->ps2tex.texaddr;
        if (!addr) {
            addr = gs_botfree;
        }
        NuRndrStreamSetZBState(addr, entry->ps2tex.psm, 0);
        stream = NuRndrStreamAddZBState(stream, 1);
    }
    return stream;
}


int NuTexGetTextAddr(int tex) {
    NuTex *entry = &TexList[tex - 1];

    if (entry->flags & NUTEX_VALID) {
        return entry->ps2tex.texaddr ? entry->ps2tex.texaddr : gs_botfree;
    }
    return 0;
}
