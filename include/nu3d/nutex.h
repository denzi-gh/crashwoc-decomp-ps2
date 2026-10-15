#ifndef NU3D_NUTEX_H
#define NU3D_NUTEX_H

#include "nu_types.h"
#include "nups2/ps2txtr.h"

/* Texture description (the GC version adds decal and linear; the PS2 one
 * is 0x18, see the allocations in nutex.c). */
struct nutex_s {
    unsigned int type;        /* 0x00 NUTEXTYPE */
    int width;                /* 0x04 */
    int height;               /* 0x08 */
    int mmcnt;                /* 0x0C */
    void *bits;               /* 0x10 */
    int *pal;                 /* 0x14 */
}; /* 0x18 */

/* One entry of the texture table (TexList). */
struct nutexslot_s {
    union {
        struct nutex_s spec;          /* 0x00 */
        struct {
            int type;
            int width;
            int height;
            int mmcnt;
            void *bits;
            int *pal;
        };
    };
    union {
        u16 refcount;                 /* 0x18 */
        s64 flags;                    /* 0x18 */
        struct {
            u64 refbits : 16;
            u64 valid : 1;
            u64 native : 1;
        };
    };
    struct nups2tex_s ps2tex;         /* 0x20 */
    u8 pad0D0[0x10];                  /* 0xD0 */
}; /* 0xE0 */

struct numtl_s;
struct nurndrstream_s;
union variptr_u;

int NuTexFindFreeSlot(void);
int NuTexHasAlpha(int tex);
struct nutex_s *NuTexReadBitmapMMEx(char *name, int mipmaps, union variptr_u *buff, union variptr_u *buffend);
struct nutex_s *NuTexReadBitmapMem(u8 *mem);
u8 *NuTexSetRenderTarget(u8 *stream, int tex);
int NuTexCreateEx(struct nutex_s *spec, void *extmem, int extsize);
int NuTexCreateStream(struct nutex_s *spec);
void NuTexAccomodateRS(struct nurndrstream_s *rs, int tex);
u8 *NuTexAccomodate(u8 *stream, int tex);
u8 *NuTexSet(u8 *stream, int tex, char context);
void NuTexDisplayTexturePage(int page, float depth);
int NuTexRecoverNativeDataEx(int tex, u8 *dest, int size, int header, int palette, int image);
struct nutexslot_s *NuTexMarkNative(struct nutexslot_s *entry);
int NuTexCreateNative(void *data);
u64 *NuTexWriteMaterialSettings(u64 *dst, struct numtl_s *mtl);
int NuTexGetSysBuffSize(int count);
void NuTexInitEx(union variptr_u *buff, int count);
void NuTexInit(void);
void NuTexClose(void);
int NuTexReserve(int size);
void NuTexUnReserve(void);
struct nutex_s *NuTexReadBitmap(char *name);
struct nutex_s *NuTexReadBitmapMM(char *name, int mipmaps);
int NuTexCreate(struct nutex_s *spec);
void NuTexDestroy(int tex);
short NuTexRef(int tex);
short NuTexUnRef(int tex);
int NuTexType(int tex);
int NuTexWidth(int tex);
int NuTexHeight(int tex);
int NuTexAlphaBits(int type);
int NuTexPixelSize(int type);
int NuTexImgSize(int type, int width, int height);
int NuTexImgSizeMM(int type, int width, int height, int levels);
int NuTexPalSize(int type);
int NuTexRecoverNativeData(int tex, u8 *dest, int size);
int NuTexReadNative(char *name, char *buff, int size, int *loaded);
int NuTexGetPointers(int tex, struct nutexslot_s **entry, struct nups2tex_s **ps2tex);
void NuTexFlushTemp(void);
void NuTexAssignAddr(int tex, int addr);
void NuTexAssignTempAddr(int tex);
int NuTexGetReqSize(int tex, int level);
void NuTexCloneTempAddr(int dst, int src);
void NuTexPalChange(int tex);
void NuTexPalChangeStreamTemp(int tex);
u64 NuTexGetTex0(int tex);
u8 *NuTexSetPal(u8 *stream, int tex, int paltex, char context);
void *NuTexSetZBuffer(void *stream, int tex);
int NuTexGetTextAddr(int tex);

#endif /* NU3D_NUTEX_H */
