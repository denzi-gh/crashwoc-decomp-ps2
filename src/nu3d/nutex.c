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

typedef struct NuTex {
    int type;
    int width;
    u8 unk008[0x10];
    u16 refcount;
    u8 unk01A[0xB2];
    int gsaddr;
    u8 unk0D0[0x10];
} NuTex;

extern NuTex *D_0062EBEC;
#define TexList D_0062EBEC

extern int gs_botfree;
extern int D_00633068;
#define gs_tempfree D_00633068

extern int NuTexReadBitmapMMEx(char *name, int mipmaps, int *addr, int *addrend);
extern int NuTexCreateEx(NuTex *spec, void *extmem, int extsize);
extern int NuTexRecoverNativeDataEx(int tex, void *dest, int size, int header, int image, int palette);


int NuTexGetSysBuffSize(int count) {
    return count * sizeof(NuTex);
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
