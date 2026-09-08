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
typedef unsigned short u16;
typedef long long s64;

typedef struct NuTex {
    int type;
    int width;
    u8 unk008[0x10];
    union {
        u16 refcount;
        s64 flags;
    };
    u8 unk020[0x30];
    int reqsize[10];
    u8 unk078[0x10];
    u16 texaddr;
    u8 unk08A[0x42];
    int gsaddr;
    u8 unk0D0[0x10];
} NuTex;

struct ps2tex_s { // 0xb0
	/* 0x00 */ struct ps2tex_s *succ;
	/* 0x04 */ struct ps2tex_s *prev;
	/* 0x08 */ short unsigned int lowaddr;
	/* 0x0a */ short unsigned int highaddr;
	/* 0x0c */ short unsigned int req;
	/* 0x0e:0 */ short unsigned int housed : 1;
	/* 0x10 */ short unsigned int p2w;
	/* 0x12 */ short unsigned int p2h;
	/* 0x14 */ struct _sceDmaTag *dt_load[7];
	/* 0x30 */ unsigned int loadsize[7];
	/* 0x4c */ unsigned int offset[7];
	/* 0x68 */ short unsigned int loadaddr[7];
	/* 0x76 */ short unsigned int loadwidth[7];
	/* 0x84 */ short unsigned int bltwidth[7];
	/* 0x92 */ unsigned char nummm;
	/* 0x93 */ unsigned char psm;
	/* 0x94 */ unsigned char bltpsm;
	/* 0x95 */ unsigned char tcc;
	/* 0x96 */ unsigned char texa;
	/* 0x97 */ unsigned char tfx;
	/* 0x98 */ struct sceDmaTag *clut_load;
	/* 0x9c */ struct sceDmaTag *clut_alt;
	/* 0xa0 */ short unsigned int clut_addr;
	/* 0xa2 */ short unsigned int clut_width;
	/* 0xa4 */ short unsigned int clut_psm;
	/* 0xa6 */ short unsigned int clut_offset;
	/* 0xa8 */ short unsigned int clut_loadsize;
	/* 0xac */ unsigned int tmpaddr;
};

enum nutextype_e
{
    NUTEX_NUMTEXTURETYPES = 26,
    NUTEX_RZT16 = 25,
    NUTEX_RZT24 = 24,
    NUTEX_RZT32 = 23,
    NUTEX_RTT16 = 22,
    NUTEX_RTT24 = 21,
    NUTEX_RTT32 = 20,
    NUTEX_RT16 = 19,
    NUTEX_RT24 = 18,
    NUTEX_RT32 = 17,
    NUTEX_RT8H = 16,
    NUTEX_RTT8 = 15,
    NUTEX_RT8 = 14,
    NUTEX_BB = 13,
    NUTEX_DXT5 = 12,
    NUTEX_DXT4 = 11,
    NUTEX_DXT3 = 10,
    NUTEX_DXT2 = 9,
    NUTEX_DXT1 = 8,
    NUTEX_PAL8_S = 7,
    NUTEX_PAL4_S = 6,
    NUTEX_PAL8 = 5,
    NUTEX_PAL4 = 4,
    NUTEX_RGBA32 = 3,
    NUTEX_RGB24 = 2,
    NUTEX_RGBA16 = 1,
    NUTEX_RGB16 = 0
};

struct nutex_s { // 0x1c
	/* 0x00 */ enum nutextype_e type;
	/* 0x04 */ int width;
	/* 0x08 */ int height;
	/* 0x0c */ int mmcnt;
	/* 0x10 */ void *bits;
	/* 0x14 */ int *pal;
};

struct nusystex_s { // 0xe0
	/* 0x00 */ struct nutex_s tex;
	/* 0x18 */ long ref;
	/* 0x20 */ struct ps2tex_s ps2tex;
	/* 0x1a:0 */ short int used;
	/* 0x1a:1 */ short int xdef;
    int pad[3];
};

extern NuTex *D_0062EBEC;
#define TexList D_0062EBEC

extern int gs_botfree;
extern int D_00633068;
#define gs_tempfree D_00633068

extern int D_0062EC04;
#define gs_reserve_sp D_0062EC04
extern int D_0067D640[256];
#define gs_reserve_stack D_0067D640

extern int D_0062ebf4;
#define nutex_alloc_cnt D_0062ebf4
extern struct nusystex_s * D_0062ebec;
#define tinfo D_0062ebec

extern int NuTexReadBitmapMMEx(char *name, int mipmaps, int *addr, int *addrend);
extern int NuTexCreateEx(NuTex *spec, void *extmem, int extsize);
extern int NuTexRecoverNativeDataEx(int tex, void *dest, int size, int header, int image, int palette);
extern void NuPs2ChangeTexPal(void *pal);
extern void NuPs2ChangeTexPalStreamTemp(void *pal);


int NuTexGetSysBuffSize(int count) {
    return count * sizeof(NuTex);
}

int NuTexReserve(int size)
{
    int rounded;
    int aligned;

    if (gs_reserve_sp >= 0x100)
        return 0;

    rounded = size + 0xff;
    if (rounded < 0)
        rounded += 0xff;

    gs_reserve_stack[gs_reserve_sp] = gs_botfree;
    gs_reserve_sp++;

    aligned = (gs_botfree + 0x1f) & 0xffffffe0;
    gs_botfree = aligned + (rounded >> 8);

    return aligned;
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

int NuPs2TxDestroy(struct ps2tex_s * tex);
void NuMemFreeFn(void * addr, char * file, int line);

void NuTexDestroy(int tid) {
  tid--;
  if (tid >= 0) {
    if ((tinfo[tid].ref & 0x20000U) == 0) {
      if (tinfo[tid].tex.bits != 0) {
        NuMemFreeFn(tinfo[tid].tex.bits,"..\\nu2.ps2\\nu3d\\nutex.c",0x3f6);
        tinfo[tid].tex.bits = 0;
      }
      NuPs2TxDestroy(&tinfo[tid].ps2tex);
    }
    tinfo[tid].ref = tinfo[tid].ref & 0xfffffffffffeffff;
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


void NuTexFlushTemp(void) {
    gs_tempfree = gs_botfree;
}


void NuTexAssignAddr(int tex, int addr) {
    if (tex > 0) {
        tex--;
        TexList[tex].gsaddr = addr;
    }
}


void NuTexAssignTempAddr(int tex) {
    if (tex > 0) {
        tex--;
        gs_tempfree = (gs_tempfree + 31) & 0xFFFFFFE0;
        TexList[tex].gsaddr = gs_tempfree;
        gs_tempfree += TexList[tex].reqsize[0];
    }
}


int NuTexGetReqSize(int tex, int level) {
    if (tex > 0) {
        tex--;
        return TexList[tex].reqsize[level] << 8;
    }
    return 0;
}


void NuTexCloneTempAddr(int dst, int src) {
    if (dst > 0 && src > 0) {
        dst--;
        src--;
        TexList[dst].gsaddr = TexList[src].gsaddr;
    }
}


void NuTexPalChange(int tex) {
    if (tex > 0) {
        NuPs2ChangeTexPal(TexList[tex - 1].unk020);
    }
}


void NuTexPalChangeStreamTemp(int tex) {
    if (tex > 0) {
        NuPs2ChangeTexPalStreamTemp(TexList[tex - 1].unk020);
    }
}

int Ps2TxGetTex0(struct ps2tex_s* ptex);

int NuTexGetTex0(int tid, int palid) {
    int id;

    id = tid - 1;
    if ((id >= 0) && ((tinfo[id].ref & 0x10000U) != 0)) {
        return Ps2TxGetTex0(&tinfo[id].ps2tex);
    }
    return 0;
}

int NuTexGetTextAddr(int tex) {
    if (TexList[tex - 1].flags & 0x10000) {
        return TexList[tex - 1].texaddr ? TexList[tex - 1].texaddr : gs_botfree;
    }
    return 0;
}
