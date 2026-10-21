/*
 * Unit: nu3d/nucvtgeo
 *
 * Functions:
 *   0x001505b0 NuCvtDmaFaceOn
 *   0x00150a58 NuCvtDmaFaceY
 *   0x00150f28 NuCvtSizeDmaBezTriCFromIdx
 *   0x00151020 NuCvtDmaBezTriCFromIdx
 *   0x00151c40 NuCvtTriStreamCFromIdx
 *   0x00152090 NuCvtTriStripStreamCFromIdx
 *   0x00152528 NuCvtSizeDmaTriCFromIdx
 *   0x001526f0 NuCvtDmaTriCFromIdx
 *   0x00153320 NuCvtSizeDmaTriStripCFromIdx
 *   0x00153738 NuCvtDmaTriStripCFromIdx
 *   0x001547d8 NuPs2CreateFaceOn
 *   0x00154b40 NuCvtCalcFastPoint
 *   0x00154cd0 NuPs2CreateRenderStream
 *   0x00155140 NuPs2DestroyRenderStream
 *   0x00155178 NuCvtGeoSetColourRefOffset
 *   0x00155180 NuCvtGeoFixup
 *   0x001551c0 NuCvtGeoBeginColourRef
 *   0x001551d8 NuCvtGeoEndColourRef
 *   0x00155210 NuCvtSizeDmaFaceOn
 *   0x001552b8 NuCvtSizeDmaFaceY
 *   0x00155360 NuCvtSetMaxSpan
 *   0x00155390 NuCvtGeoAddColourRef
 *   0x001553d8 NuCvtDestroyPacketInfo
 */

#include "nu_libc.h"
#include "nu_types.h"

#include "numath/numathtypes.h"
#include "numath/nufloat.h"
#include "nu3d/numtl.h"
#include "nu3d/nugobj.h"
#include "nups2/nups2types.h"
#include "nucore/nuerror.h"

struct nucolourpair_s {
    u32 colour;
    u32 lit;
};

typedef struct nucolourblend_s {
    unsigned int addr;
    int count;
    struct nucolourpair_s pair[0];
} NuColourRef;

struct nudmatag_s;

union nucvtgiftag_u {
    struct GifTag tag;
    u64 dw[2];
    u32 w[4];
};

struct nucvtrgba_s {
    u8 b;
    u8 g;
    u8 r;
    u8 a;
};

union nucvtcolour_u {
    u32 w;
    struct nucvtrgba_s c;
};

struct nucvtstreamvtx_s {
    float x;
    float y;
    float z;
    union {
        float nx;
        u16 adc;
        u8 kick;
    };
    int u;
    int v;
    int ny;
    int nz;
    int r;
    int g;
    int b;
    int a;
};

struct nucvtstreamtri_s {
    struct DmaTag tag;
    struct nucvtstreamvtx_s vtx[3];
};

struct nucvtpktinfo_s {
    int offset;
    int count;
};

typedef union NuFloatBits {
    float f;
    int i;
} NuFloatBits;


extern void NuMemFreeFn(void *ptr, char *file, int line);
extern u8 *vpDmaTag_Cnt(u8 *cur);
extern u8 *vpDmaTag_CntEx(u8 *cur, s32 qwc, u32 tag);
extern u8 *vpDmaTag_Close(u8 *cur);
extern u8 *vpDmaTag_Ret(u8 *cur);
extern u8 *vpDmaTag_RefEx(u8 *cur, u32 addr, s32 qwc, u32 tag);
extern void *NuMemAllocFn(int size, char *file, int line);

extern int NuCvtPntEna;
extern int NuCvtPntOn;
extern int NuCvtUvOn;
extern float NuCvtMaxSpan;
extern NuFloatBits NuCvtPntSpan[3];
extern struct nuvec_s NuCvtPntMid;
extern struct nucvtpktinfo_s *info_pktdata;
extern u8 *info_pktbuff;
extern int info_pktcnt;
extern int info_pktsizecnt;
extern int nugscn_generate_packetinfo;
extern int info_stripcount;
extern int info_tristripcount;
extern struct nudmatag_s *nucvtgeo_lasttag;
extern int nugscn_generate_colourref;
extern int info_tricount;
extern int show_strip;

extern int D_0062EEDC;
extern int D_0062EEE0;
extern NuColourRef *D_00633118;
extern NuColourRef *D_0063311C;
extern char D_00615538[];
extern char D_00615558[];
extern char D_00615568[];
extern char D_00615668[];
extern char D_00615678[];
extern char D_00615590[];
extern char D_006155A8[];
extern char D_006155D8[];
extern char D_00615608[];
extern char D_00615638[];
extern char D_00615690[];

#define colourref_on D_0062EEDC
#define colourref_offset D_0062EEE0
#define colourref_list D_00633118
#define colourref_cur D_0063311C
#define nucvtgeo_srcfile D_00615538
#define stream_align_msg D_00615558
#define dma_size_msg D_00615568
#define faceon_alpha_msg D_00615668
#define faceon_type_msg D_00615678
#define pkt_count_msg D_00615590
#define u_high_msg D_006155A8
#define v_high_msg D_006155D8
#define u_low_msg D_00615608
#define v_low_msg D_00615638
#define prim_type_msg D_00615690

#define NUMTL_ALPHA_BLEND 2
#define NUPRIM_TRILIST 5
#define NUPRIM_TRISTRIP 6
#define ALPHA_REG_TOP_MASK 0xFF00000000000000ULL
#define FLOAT_EXP_MASK 0x7F800000
#define SPAN_EXP_BIAS 0x04008000

#define HALVE_ALPHA_RED_GREEN(col, shift) \
    ((u8)((col).c.a >> 1) << 24 | (u8)((((col).w >> 16) & 0xFF) >> (shift)) << 16 \
     | (u8)((((col).w >> 8) & 0xFF) >> (shift)) << 8)

#define HALVE_BLUE(col, shift) ((u8)(((col).w & 0xFF) >> (shift)))

#define HALVE_COLOUR(col, shift) (HALVE_ALPHA_RED_GREEN(col, shift) | HALVE_BLUE(col, shift))

#define NEXT_COLOURREF(ref) ((NuColourRef *)((char *)(ref) + ((ref)->count * 8 + 8)))

void NuCvtGeoSetColourRefOffset(int offset);
void NuCvtGeoFixup(NuColourRef *ref, int offset);
void NuCvtGeoBeginColourRef(NuColourRef *list);
NuColourRef *NuCvtGeoEndColourRef(void);
unsigned int NuCvtSizeDmaFaceOn(void *dma, int count, void *unused, int *qwc);
unsigned int NuCvtSizeDmaFaceY(void *dma, int count, void *unused, int *qwc);
void NuCvtSetMaxSpan(float span);
void NuCvtGeoAddColourRef(unsigned int addr);
void NuCvtDestroyPacketInfo(void);


inline void NuCvtGeoSetColourRefOffset(int offset) {
    colourref_offset = offset;
}


inline void NuCvtGeoFixup(NuColourRef *ref, int offset) {
    while (ref->count != 0) {
        ref->addr = offset + ref->addr;
        ref = NEXT_COLOURREF(ref);
    }
}


inline void NuCvtGeoBeginColourRef(NuColourRef *list) {
    colourref_list = list;
    colourref_on = 1;
    colourref_cur = 0;
    colourref_offset = 0;
}


inline NuColourRef *NuCvtGeoEndColourRef(void) {
    NuColourRef *end;

    NuCvtGeoAddColourRef(0);
    end = NEXT_COLOURREF(colourref_cur);
    colourref_on = 0;
    return end;
}


inline unsigned int NuCvtSizeDmaFaceOn(void *dma, int count, void *unused, int *qwc) {
    unsigned int size;
    int words;
    int batch;
    int i;

    words = 8;
    size = 0x10;
    while (count) {
        batch = count > 20 ? 20 : count;
        size += 4;
        size += 16;
        size += 4;
        size += 8;
        size += 4;
        size += 4;
        for (i = 0; i < batch; i++) {
            size += 4;
            size += 4;
            size += 4;
        }
        words += 8;
        size += 4;
        for (i = 0; i < batch; i++) {
            size += 8;
        }
        size += 4;
        words += batch * 8;
        for (i = 0; i < batch; i++) {
            size += 4;
        }
        size += 4;
        size += 4;
        count -= batch;
    }
    size = (size + 15) & 0xFFFFFFF0;
    if (qwc) {
        *qwc = words;
    }
    return size;
}


inline unsigned int NuCvtSizeDmaFaceY(void *dma, int count, void *unused, int *qwc) {
    unsigned int size;
    int words;
    int batch;
    int i;

    words = 8;
    size = 0x24;
    while (count) {
        batch = count > 10 ? 10 : count;
        size += 4;
        size += 16;
        size += 4;
        size += 8;
        size += 4;
        size += 4;
        for (i = 0; i < batch; i++) {
            size += 12;
        }
        words += 8;
        size += 4;
        for (i = 0; i < batch; i++) {
            size += 8;
        }
        size += 4;
        words += batch * 8;
        for (i = 0; i < batch; i++) {
            size += 4;
        }
        size += 8;
        count -= batch;
    }
    size = (size + 15) & 0xFFFFFFF0;
    if (qwc) {
        *qwc = words;
    }
    return size;
}


inline void NuCvtSetMaxSpan(float span) {
    NuCvtMaxSpan = span;
    if (span <= 0.0f) {
        NuCvtPntEna = 0;
    } else {
        NuCvtPntEna = 1;
    }
}


inline void NuCvtGeoAddColourRef(unsigned int addr) {
    if (colourref_on) {
        if (!colourref_cur) {
            colourref_cur = colourref_list;
        } else {
            colourref_cur = NEXT_COLOURREF(colourref_cur);
        }
        colourref_cur->addr = addr + colourref_offset;
        colourref_cur->count = 0;
    }
}


inline void NuCvtDestroyPacketInfo(void) {
    if (info_pktdata) {
        NuMemFreeFn(info_pktdata, nucvtgeo_srcfile, 0xA9E);
    }
}


u8 *NuCvtDmaFaceOn(u8 **dma, struct nufaceon_s *faceon, int count, struct numtl_s *mtl) {
    union nucvtgiftag_u sprite;
    union nucvtgiftag_u tri;
    union nucvtcolour_u lit;
    u8 *start;
    u8 *p;
    int shift;
    int batch;
    int left;
    union nucvtcolour_u colour;
    int i;

    shift = mtl->tid != 0;
    p = *dma;
    sprite.dw[0] = 0x5003400000008000LL;
    sprite.dw[1] = 0x52521;
    tri.dw[0] = 0x3001C00000008003LL;
    tri.dw[1] = 0x512;
    if (p == 0) {
        return 0;
    }
    start = p;
    p = vpDmaTag_CntEx(start, 0, 0);
    while (count) {
        batch = count > 20 ? 20 : count;
        sprite.tag.nloop = batch;
        *(u32 *)p = 0x6C018000;
        p += 4;
        *(u32 *)p = sprite.w[0];
        p += 4;
        *(u32 *)p = sprite.w[1];
        p += 4;
        *(u32 *)p = sprite.w[2];
        p += 4;
        *(u32 *)p = sprite.w[3];
        p += 4;
        *(u32 *)p = 0x6D018001;
        p += 4;
        *(u16 *)p = batch * 3;
        p += 2;
        *(u16 *)p = 0;
        p += 2;
        *(u16 *)p = 0;
        p += 2;
        *(u16 *)p = 0;
        p += 2;
        *(u32 *)p = 0x01000103;
        p += 4;
        *(u32 *)p = (batch << 16) | 0x68008004;
        p += 4;
        for (i = 0; i < batch; i++) {
            *(float *)p = faceon[i].point.x;
            p += 4;
            *(float *)p = faceon[i].point.y;
            p += 4;
            *(float *)p = faceon[i].point.z;
            p += 4;
        }
        *(u32 *)p = (batch << 16) | 0x64008005;
        p += 4;
        left = count - batch;
        for (i = 0; i < batch; i++) {
            *(float *)p = -faceon[i].width * 1960.0f * 640.0f / 256.0f;
            p += 4;
            *(float *)p = -faceon[i].height * 2300.0f;
            p += 4;
        }
        *(u32 *)p = (batch << 16) | 0x6E00C006;
        p += 4;
        NuCvtGeoAddColourRef((unsigned int)p);
        for (i = 0; i < batch; i++) {
            *p++ = ((faceon[i].colour >> 16) & 0xFF) >> shift;
            *p++ = ((faceon[i].colour >> 8) & 0xFF) >> shift;
            *p++ = (faceon[i].colour & 0xFF) >> shift;
            *p++ = ((faceon[i].colour >> 24) & 0xFF) >> 1;
            colour.w = faceon[i].colour;
            lit.c.b = colour.c.b * 0.3f;
            lit.c.g = colour.c.g * 0.3f;
            lit.c.r = colour.c.r * 1.0f;
            lit.c.a = colour.c.a;
            if (colourref_on) {
                u32 blue = HALVE_BLUE(colour, shift);

                colourref_cur->pair[colourref_cur->count].colour = HALVE_ALPHA_RED_GREEN(colour, shift) | blue;
                colourref_cur->pair[colourref_cur->count].lit = HALVE_COLOUR(lit, shift);
                colourref_cur->count++;
            }
        }
        *(u32 *)p = 0x01000101;
        p += 4;
        *(u32 *)p = 0x14000304;
        p += 4;
        faceon += batch;
        count = left;
    }
    *dma = vpDmaTag_Close(p);
    return start;
}


u8 *NuCvtDmaFaceY(u8 **dma, struct nufaceon_s *faceon, int count, struct numtl_s *mtl) {
    union nucvtgiftag_u strip;
    union nucvtgiftag_u fan;
    union nucvtcolour_u lit;
    u8 *start;
    u8 *p;
    int shift;
    int batch;
    int left;
    union nucvtcolour_u colour;
    int i;

    shift = mtl->tid != 0;
    p = *dma;
    strip.dw[0] = 0xC002400000008000LL;
    strip.w[2] = 0x12512512;
    strip.w[3] = 0x5125;
    fan.dw[0] = 0x3002C00000008000LL;
    fan.dw[1] = 0x512;
    if (p == 0) {
        return 0;
    }
    start = p;
    nucvtgeo_lasttag = (struct nudmatag_s *)start;
    p = vpDmaTag_Cnt(start);
    *(u32 *)p = 0x6C010056;
    p[-14] = 5;
    p += 4;
    *(u32 *)p = fan.w[0];
    p += 4;
    *(u32 *)p = fan.w[1];
    p += 4;
    *(u32 *)p = fan.w[2];
    p += 4;
    *(u32 *)p = fan.w[3];
    p += 4;
    while (count) {
        batch = count > 10 ? 10 : count;
        strip.tag.nloop = batch;
        *(u32 *)p = 0x6C018000;
        p += 4;
        *(u32 *)p = strip.w[0];
        p += 4;
        *(u32 *)p = strip.w[1];
        p += 4;
        *(u32 *)p = strip.w[2];
        p += 4;
        *(u32 *)p = strip.w[3];
        p += 4;
        *(u32 *)p = 0x6D018001;
        p += 4;
        *(u16 *)p = batch * 3;
        p += 2;
        *(u16 *)p = 0;
        p += 2;
        *(u16 *)p = 0;
        p += 2;
        *(u16 *)p = 0;
        p += 2;
        *(u32 *)p = 0x01000103;
        p += 4;
        *(u32 *)p = (batch << 16) | 0x68008004;
        p += 4;
        for (i = 0; i < batch; i++) {
            *(float *)p = faceon[i].point.x;
            p += 4;
            *(float *)p = faceon[i].point.y;
            p += 4;
            *(float *)p = faceon[i].point.z;
            p += 4;
        }
        *(u32 *)p = (batch << 16) | 0x64008005;
        p += 4;
        left = count - batch;
        for (i = 0; i < batch; i++) {
            *(float *)p = faceon[i].width * 0.5f;
            p += 4;
            *(float *)p = faceon[i].height * 0.5f;
            p += 4;
        }
        *(u32 *)p = (batch << 16) | 0x6E00C006;
        p += 4;
        NuCvtGeoAddColourRef((unsigned int)p);
        for (i = 0; i < batch; i++) {
            *p++ = ((faceon[i].colour >> 16) & 0xFF) >> shift;
            *p++ = ((faceon[i].colour >> 8) & 0xFF) >> shift;
            *p++ = (faceon[i].colour & 0xFF) >> shift;
            *p++ = ((faceon[i].colour >> 24) & 0xFF) >> 1;
            colour.w = faceon[i].colour;
            lit.c.b = colour.c.b * 0.3f;
            lit.c.g = colour.c.g * 0.3f;
            lit.c.r = colour.c.r * 1.0f;
            lit.c.a = colour.c.a;
            if (colourref_on) {
                u32 blue = HALVE_BLUE(colour, shift);

                colourref_cur->pair[colourref_cur->count].colour = HALVE_ALPHA_RED_GREEN(colour, shift) | blue;
                colourref_cur->pair[colourref_cur->count].lit = HALVE_COLOUR(lit, shift);
                colourref_cur->count++;
            }
        }
        *(u32 *)p = 0x01000101;
        p += 4;
        *(u32 *)p = 0x14000306;
        p += 4;
        faceon += batch;
        count = left;
    }
    *dma = vpDmaTag_Close(p);
    return start;
}


unsigned int NuCvtSizeDmaBezTriCFromIdx(void *dma, int count) {
    u8 *p;
    int batch;
    int i;

    p = vpDmaTag_Cnt(0);
    p[-14] = 5;
    while (count) {
        batch = count > 9 ? 9 : count;
        p += 0x3C;
        for (i = 0; i < batch * 2; i += 6) {
            p += 0x30;
        }
        p += 4;
        for (i = 0; i < batch * 2; i += 6) {
            p += 0x18;
        }
        p += 4;
        for (i = 0; i < batch * 2; i += 6) {
            p += 0xC;
        }
        p += 0x1C;
        for (i = 0; i < batch * 2; i += 6) {
            p += 0x24;
        }
        p += 4;
        count -= batch;
    }
    return ((unsigned int)p + 15) & 0xFFFFFFF0;
}


#define NUCVT_MIN(a, b) ((a) < (b) ? (a) : (b))
#define NUCVT_MAX(a, b) ((a) > (b) ? (a) : (b))

#define PUT_BEZ_POS(v) \
    *(float *)p = (v).pnt.x; \
    p += 4; \
    *(float *)p = (v).pnt.y; \
    p += 4; \
    *(float *)p = (v).pnt.z; \
    p += 4

#define PUT_BEZ_UV(v) \
    *(short *)p = ((v).tc[0] - minu) * 4096.0f; \
    p += 2; \
    *(short *)p = ((v).tc[1] - minv) * 4096.0f; \
    p += 2; \
    *(short *)p = (v).nrm.y * 4096.0f; \
    p += 2; \
    *(short *)p = (v).nrm.z * 4096.0f; \
    p += 2

#define PUT_BEZ_RGBA(v) \
    r = ((v).diffuse & 0xFF) * gain; \
    if (r > 255.0f) { \
        r = 255.0f; \
    } \
    g = (((v).diffuse >> 8) & 0xFF) * gain; \
    if (g > 255.0f) { \
        g = 255.0f; \
    } \
    b = (((v).diffuse >> 16) & 0xFF) * gain; \
    if (b > 255.0f) { \
        b = 255.0f; \
    } \
    a = (((v).diffuse >> 24) & 0xFF) * 128.0f; \
    if (a > 255.0f) { \
        a = 255.0f; \
    } \
    *p++ = (int)r; \
    *p++ = (int)g; \
    *p++ = (int)b; \
    *p++ = (int)a

u8 *NuCvtDmaBezTriCFromIdx(u8 **dma, struct nuvtx_tc1_s *vtx, short *idx, int count) {
    union nucvtgiftag_u strip;
    union nucvtgiftag_u tri;
    u8 *start;
    u8 *p;
    float minu;
    float minv;
    float r;
    float g;
    float b;
    float a;
    float gain;
    int batch;
    int i;

    gain = 16.0f;
    strip.dw[0] = 0x3001C00000008000LL;
    strip.dw[1] = 0x512;
    tri.dw[0] = 0x3001C00000008003LL;
    tri.dw[1] = 0x512;
    start = *dma;
    nucvtgeo_lasttag = (struct nudmatag_s *)start;
    p = vpDmaTag_Cnt(start);
    p[-14] = 5;
    while (count) {
        batch = count > 9 ? 9 : count;
        strip.tag.nloop = batch;
        *(u32 *)p = 0x6C0180D2;
        p += 4;
        *(u32 *)p = strip.w[0];
        p += 4;
        *(u32 *)p = strip.w[1];
        p += 4;
        *(u32 *)p = strip.w[2];
        p += 4;
        *(u32 *)p = strip.w[3];
        p += 4;
        *(u32 *)p = 0x6C018001;
        p += 4;
        *(u32 *)p = tri.w[0];
        p += 4;
        *(u32 *)p = tri.w[1];
        p += 4;
        *(u32 *)p = tri.w[2];
        p += 4;
        *(u32 *)p = tri.w[3];
        p += 4;
        *(u32 *)p = 0x6D018002;
        p += 4;
        *(u16 *)p = batch * 3;
        p += 2;
        *(u16 *)p = 0;
        p += 2;
        *(u16 *)p = 0;
        p += 2;
        *(u16 *)p = 0;
        p += 2;
        *(u32 *)p = 0x01000103;
        p += 4;
        *(u32 *)p = (batch << 16) | 0x6C008003;
        p += 4;
        for (i = 0; i < batch * 2; i += 6) {
            PUT_BEZ_POS(vtx[idx[i]]);
            *(float *)p = vtx[idx[i]].nrm.x;
            p += 4;
            PUT_BEZ_POS(vtx[idx[i + 3]]);
            *(float *)p = vtx[idx[i + 3]].nrm.x;
            p += 4;
            PUT_BEZ_POS(vtx[idx[i + 5]]);
            *(float *)p = vtx[idx[i + 5]].nrm.x;
            p += 4;
        }
        *(u32 *)p = (batch << 16) | 0x6D008004;
        p += 4;
        for (i = 0; i < batch * 2; i += 6) {
            minu = NUCVT_MIN(vtx[idx[i]].tc[0], NUCVT_MIN(vtx[idx[i + 3]].tc[0], vtx[idx[i + 5]].tc[0]));
            minv = NUCVT_MIN(vtx[idx[i]].tc[1], NUCVT_MIN(vtx[idx[i + 3]].tc[1], vtx[idx[i + 5]].tc[1]));
            minu = floor(minu);
            minv = floor(minv);
            PUT_BEZ_UV(vtx[idx[i]]);
            PUT_BEZ_UV(vtx[idx[i + 3]]);
            PUT_BEZ_UV(vtx[idx[i + 5]]);
        }
        *(u32 *)p = (batch << 16) | 0x6E00C005;
        p += 4;
        for (i = 0; i < batch * 2; i += 6) {
            PUT_BEZ_RGBA(vtx[idx[i]]);
            PUT_BEZ_RGBA(vtx[idx[i + 3]]);
            PUT_BEZ_RGBA(vtx[idx[i + 5]]);
        }
        *(u32 *)p = 0x01000101;
        p += 4;
        strip.tag.nloop = batch * 4;
        *(u32 *)p = (batch * 3 + 3) | 0x6C018000;
        p += 4;
        *(u32 *)p = strip.w[0];
        p += 4;
        *(u32 *)p = strip.w[1];
        p += 4;
        *(u32 *)p = strip.w[2];
        p += 4;
        *(u32 *)p = strip.w[3];
        p += 4;
        *(u32 *)p = (batch * 3 + 4 | 0x8000 | batch << 16) | 0x68000000;
        p += 4;
        for (i = 0; i < batch * 2; i += 6) {
            PUT_BEZ_POS(vtx[idx[i + 1]]);
            PUT_BEZ_POS(vtx[idx[i + 2]]);
            PUT_BEZ_POS(vtx[idx[i + 4]]);
        }
        *(u32 *)p = 0x14000308;
        p += 4;
        count -= batch;
        idx += batch * 2;
    }
    *dma = vpDmaTag_Close(p);
    return start;
}


int NuCvtTriStreamCFromIdx(u8 **dma, struct nuvtx_tc1_s *vtx, short *idx, int nidx, struct numtl_s *mtl) {
    struct nucvtstreamtri_s *tri;
    int count;
    int shift;
    int j;

    if (mtl->tid != -1) {
        shift = 1;
    } else {
        shift = 0;
    }
    tri = (struct nucvtstreamtri_s *)*dma;
    count = 0;
    while (nidx) {
        vpDmaTag_RefEx((u8 *)tri, (u32)tri->vtx, 0, 0x6C098003);
        count++;
        tri->tag.qwc = 9;
        for (j = 0; j < 3; j++) {
            tri->vtx[j].x = vtx[*idx].pnt.x;
            tri->vtx[j].y = vtx[*idx].pnt.y;
            tri->vtx[j].z = vtx[*idx].pnt.z;
            tri->vtx[j].nx = vtx[*idx].nrm.x;
            if (j < 2) {
                if (mtl->attrib.lighting == NUMTL_ALPHA_BLEND) {
                    tri->vtx[j].adc = 0x8001;
                } else {
                    tri->vtx[j].kick = 1;
                }
            } else {
                if (mtl->attrib.lighting == NUMTL_ALPHA_BLEND) {
                    tri->vtx[j].adc = 0;
                } else {
                    tri->vtx[j].kick = 0;
                }
            }
            tri->vtx[j].u = vtx[*idx].tc[0] * 4096.0f;
            tri->vtx[j].v = vtx[*idx].tc[1] * 4096.0f;
            tri->vtx[j].ny = vtx[*idx].nrm.y * 4096.0f;
            tri->vtx[j].nz = vtx[*idx].nrm.z * 4096.0f;
            switch (mtl->attrib.colour) {
            case 0:
                if (mtl->attrib.lighting == NUMTL_ALPHA_BLEND) {
                    tri->vtx[j].r = (u8)(mtl->diffuse.r * 255.0f) >> shift;
                    tri->vtx[j].g = (u8)(mtl->diffuse.g * 255.0f) >> shift;
                    tri->vtx[j].b = (u8)(mtl->diffuse.b * 255.0f) >> shift;
                    tri->vtx[j].a = (u8)(mtl->alpha * 127.0f);
                } else {
                    tri->vtx[j].r = 0;
                    tri->vtx[j].g = 0;
                    tri->vtx[j].b = 0;
                    tri->vtx[j].a = (u8)(mtl->alpha * 127.0f);
                }
                break;
            case 1:
                tri->vtx[j].r = ((u32)vtx[*idx].diffuse & 0xFF) >> shift;
                tri->vtx[j].g = (((u32)vtx[*idx].diffuse >> 8) & 0xFF) >> shift;
                tri->vtx[j].b = (((u32)vtx[*idx].diffuse >> 16) & 0xFF) >> shift;
                tri->vtx[j].a = vtx[*idx].rgba.a >> 1;
                break;
            }
            idx++;
        }
        tri++;
        nidx -= 3;
    }
    if ((u32)tri & 0xF) {
        NuErrorProlog(nucvtgeo_srcfile, 0x3FB)(stream_align_msg);
    }
    *dma = (u8 *)tri;
    return count;
}


int NuCvtTriStripStreamCFromIdx(u8 **dma, struct nuvtx_tc1_s *vtx, short *idx, int nidx, struct numtl_s *mtl) {
    struct nucvtstreamtri_s *tri;
    int vid[3];
    int left;
    int count;
    int shift;
    int j;

    if (mtl->tid != -1) {
        shift = 1;
    } else {
        shift = 0;
    }
    tri = (struct nucvtstreamtri_s *)*dma;
    count = 0;
    left = 0;
    while (nidx) {
        if (left == 0) {
            left = *idx++;
            nidx -= 3;
            vid[1] = *idx++;
            left -= 2;
            vid[2] = *idx++;
        }
        vid[0] = vid[1];
        vid[1] = vid[2];
        vid[2] = *idx;
        vpDmaTag_RefEx((u8 *)tri, (u32)tri->vtx, 0, 0x6C098003);
        idx++;
        count++;
        tri->tag.qwc = 9;
        for (j = 0; j < 3; j++) {
            tri->vtx[j].x = vtx[vid[j]].pnt.x;
            tri->vtx[j].y = vtx[vid[j]].pnt.y;
            tri->vtx[j].z = vtx[vid[j]].pnt.z;
            tri->vtx[j].nx = vtx[vid[j]].nrm.x;
            if (j < 2) {
                if (mtl->attrib.lighting == NUMTL_ALPHA_BLEND) {
                    tri->vtx[j].adc = 0x8001;
                } else {
                    tri->vtx[j].kick = 1;
                }
            } else {
                if (mtl->attrib.lighting == NUMTL_ALPHA_BLEND) {
                    tri->vtx[j].adc = 0;
                } else {
                    tri->vtx[j].kick = 0;
                }
            }
            tri->vtx[j].u = vtx[vid[j]].tc[0] * 4096.0f;
            tri->vtx[j].v = vtx[vid[j]].tc[1] * 4096.0f;
            tri->vtx[j].ny = vtx[vid[j]].nrm.y * 4096.0f;
            tri->vtx[j].nz = vtx[vid[j]].nrm.z * 4096.0f;
            switch (mtl->attrib.colour) {
            case 0:
                if (mtl->attrib.lighting == NUMTL_ALPHA_BLEND) {
                    tri->vtx[j].r = (u8)(mtl->diffuse.r * 255.0f) >> shift;
                    tri->vtx[j].g = (u8)(mtl->diffuse.g * 255.0f) >> shift;
                    tri->vtx[j].b = (u8)(mtl->diffuse.b * 255.0f) >> shift;
                    tri->vtx[j].a = (u8)(mtl->alpha * 127.0f);
                } else {
                    tri->vtx[j].r = 0;
                    tri->vtx[j].g = 0;
                    tri->vtx[j].b = 0;
                    tri->vtx[j].a = (u8)(mtl->alpha * 127.0f);
                }
                break;
            case 1:
                tri->vtx[j].r = ((u32)vtx[vid[j]].diffuse & 0xFF) >> shift;
                tri->vtx[j].g = (((u32)vtx[vid[j]].diffuse >> 8) & 0xFF) >> shift;
                tri->vtx[j].b = (((u32)vtx[vid[j]].diffuse >> 16) & 0xFF) >> shift;
                tri->vtx[j].a = vtx[vid[j]].rgba.a >> 1;
                break;
            }
        }
        tri++;
        nidx--;
        left--;
    }
    if ((u32)tri & 0xF) {
        NuErrorProlog(nucvtgeo_srcfile, 0x462)(stream_align_msg);
    }
    *dma = (u8 *)tri;
    return count;
}


unsigned int NuCvtSizeDmaTriCFromIdx(void *dma, int count, struct numtl_s *mtl, int *qwc) {
    unsigned int size;
    int words;
    int fast;
    int batch;
    int i;

    fast = 0;
    if (NuCvtPntEna && mtl->attrib.lighting == NUMTL_ALPHA_BLEND) {
        fast = mtl->fxid == 0;
    }
    words = 8;
    size = 0x24;
    if (NuCvtPntOn) {
        size += 4;
        size += 0x10;
    }
    while (count) {
        batch = count > 42 ? 42 : count;
        size += 0x18;
        if (NuCvtPntOn) {
            size += 0x10;
        } else {
            size += 8;
        }
        size += 4;
        count -= batch;
        if (NuCvtPntOn) {
            size += 4;
        }
        size += 4;
        for (i = 0; i < batch; i++) {
            if (!NuCvtPntOn) {
                size += 0x10;
            } else {
                size += 8;
            }
        }
        if (NuCvtPntOn) {
            size += 4;
        }
        if (NuCvtUvOn) {
            size += 4;
        }
        size += 4;
        for (i = 0; i < batch; i++) {
            size += 4;
            if (!fast) {
                size += 2;
                size += 2;
            }
        }
        if (NuCvtUvOn) {
            size += 4;
        }
        size += 4;
        switch (mtl->attrib.colour) {
        case 0:
            if (mtl->attrib.lighting == NUMTL_ALPHA_BLEND) {
                for (i = 0; i < batch; i++) {
                    size += 4;
                }
            } else {
                for (i = 0; i < batch; i++) {
                    size += 4;
                }
            }
            break;
        case 1:
            words += 8 + batch * 8;
            for (i = 0; i < batch; i++) {
                size += 4;
            }
            break;
        }
        size += 8;
    }
    size = (size + 15) & 0xFFFFFFF0;
    if (qwc) {
        *qwc = words;
    }
    return size;
}


u8 *NuCvtDmaTriCFromIdx(u8 **dma, struct nuvtx_tc1_s *vtx, short *idx, int nidx, struct numtl_s *mtl) {
    union nucvtgiftag_u strip;
    union nucvtgiftag_u fan;
    union nucvtcolour_u lit;
    union nucvtcolour_u colour;
    NuFloatBits packed[3];
    u8 *start;
    u8 *p;
    float minu;
    float minv;
    int fast;
    int shift;
    int size;
    int batch;
    int left;
    int i;

    fast = 0;
    if (NuCvtPntEna && mtl->attrib.lighting == NUMTL_ALPHA_BLEND) {
        fast = mtl->fxid == 0;
    }
    size = (((nidx + 41) / 42) * 21 + nidx * 9 + 4) * 4;
    shift = mtl->tid != -1;
    strip.dw[0] = 0x3002400000008000LL;
    strip.dw[1] = 0x512;
    fan.dw[0] = 0x3002C00000008000LL;
    fan.dw[1] = 0x512;
    if (size > 0xFFFFF) {
        NuErrorProlog(nucvtgeo_srcfile, 0x556)(dma_size_msg, size, nidx);
    }
    start = *dma;
    nucvtgeo_lasttag = (struct nudmatag_s *)start;
    p = vpDmaTag_Cnt(start);
    p[-14] = 5;
    if (NuCvtPntOn) {
        *(u32 *)p = 0x30000000;
        p += 4;
        *(u32 *)p = NuCvtPntSpan[0].i;
        p += 4;
        *(u32 *)p = NuCvtPntSpan[1].i;
        p += 4;
        *(u32 *)p = NuCvtPntSpan[2].i;
        p += 4;
        *(u32 *)p = 0;
        p += 4;
    }
    *(u32 *)p = 0x6C010056;
    p += 4;
    *(u32 *)p = fan.w[0];
    p += 4;
    *(u32 *)p = fan.w[1];
    p += 4;
    *(u32 *)p = fan.w[2];
    p += 4;
    *(u32 *)p = fan.w[3];
    p += 4;
    while (nidx) {
        batch = nidx > 42 ? 42 : nidx;
        strip.tag.nloop = batch;
        *(u32 *)p = 0x6C0180D2;
        p += 4;
        *(u32 *)p = strip.w[0];
        p += 4;
        *(u32 *)p = strip.w[1];
        p += 4;
        *(u32 *)p = strip.w[2];
        p += 4;
        *(u32 *)p = strip.w[3];
        p += 4;
        if (NuCvtPntOn) {
            *(u32 *)p = 0x6C018002;
            p += 4;
            *(u32 *)p = batch * 3;
            p += 4;
            *(float *)p = NuCvtPntSpan[0].f - NuCvtPntMid.x;
            p += 4;
            *(float *)p = NuCvtPntSpan[1].f - NuCvtPntMid.y;
            p += 4;
            *(float *)p = NuCvtPntSpan[2].f - NuCvtPntMid.z;
            p += 4;
        } else {
            *(u32 *)p = 0x6D018002;
            p += 4;
            *(u16 *)p = batch * 3;
            p += 2;
            *(u16 *)p = 0;
            p += 2;
            *(u16 *)p = 0;
            p += 2;
            *(u16 *)p = 0;
            p += 2;
        }
        *(u32 *)p = 0x01000103;
        p += 4;
        if (NuCvtPntOn) {
            *(u32 *)p = 0x05000001;
            p += 4;
            *(u32 *)p = (batch << 16) | 0x6D008003;
        } else {
            *(u32 *)p = (batch << 16) | 0x6C008003;
        }
        p += 4;
        left = nidx - batch;
        for (i = 0; i < batch; i++) {
            if (!NuCvtPntOn) {
                *(float *)p = vtx[idx[i]].pnt.x;
                p += 4;
                *(float *)p = vtx[idx[i]].pnt.y;
                p += 4;
                *(float *)p = vtx[idx[i]].pnt.z;
                p += 4;
                *(float *)p = vtx[idx[i]].nrm.x;
            } else {
                packed[0].f = NuCvtPntSpan[0].f + (vtx[idx[i]].pnt.x - NuCvtPntMid.x);
                packed[1].f = NuCvtPntSpan[1].f + (vtx[idx[i]].pnt.y - NuCvtPntMid.y);
                packed[2].f = NuCvtPntSpan[2].f + (vtx[idx[i]].pnt.z - NuCvtPntMid.z);
                *(u16 *)p = packed[0].i - NuCvtPntSpan[0].i;
                p += 2;
                *(u16 *)p = packed[1].i - NuCvtPntSpan[1].i;
                p += 2;
                *(u16 *)p = packed[2].i - NuCvtPntSpan[2].i;
                p += 2;
                *(u16 *)p = vtx[idx[i]].nrm.x;
            }
            if (i % 3 < 2) {
                *(u16 *)p = 0x8001;
            } else {
                *(u16 *)p = 0;
            }
            if (NuCvtPntOn) {
                p += 2;
            } else {
                p += 4;
            }
        }
        if (NuCvtPntOn) {
            *(u32 *)p = 0x05000000;
            p += 4;
        }
        if (NuCvtUvOn) {
            *(u32 *)p = 0x05000001;
            p += 4;
        }
        if (fast) {
            *(u32 *)p = (batch << 16) | 0x65008004;
        } else {
            *(u32 *)p = (batch << 16) | 0x6D008004;
        }
        p += 4;
        for (i = 0; i < batch; i++) {
            if (i % 3 == 0) {
                info_tricount++;
                minu = NUCVT_MIN(vtx[idx[i]].tc[0], NUCVT_MIN(vtx[idx[i + 1]].tc[0], vtx[idx[i + 2]].tc[0]));
                minv = NUCVT_MIN(vtx[idx[i]].tc[1], NUCVT_MIN(vtx[idx[i + 1]].tc[1], vtx[idx[i + 2]].tc[1]));
                minu = floor(minu);
                minv = floor(minv);
            }
            *(short *)p = (vtx[idx[i]].tc[0] - minu) * 4096.0f;
            p += 2;
            *(short *)p = (vtx[idx[i]].tc[1] - minv) * 4096.0f;
            p += 2;
            if (!fast) {
                *(short *)p = vtx[idx[i]].nrm.y * 4096.0f;
                p += 2;
                *(short *)p = vtx[idx[i]].nrm.z * 4096.0f;
                p += 2;
            }
        }
        if (NuCvtUvOn) {
            *(u32 *)p = 0x05000000;
            p += 4;
        }
        *(u32 *)p = (batch << 16) | 0x6E00C005;
        p += 4;
        if (show_strip) {
            for (i = 0; i < batch; i++) {
                *(u32 *)p = 0x80FF0000;
                p += 4;
            }
        } else {
            switch (mtl->attrib.colour) {
            case 0:
                if (mtl->attrib.lighting == NUMTL_ALPHA_BLEND) {
                    for (i = 0; i < batch; i++) {
                        *p++ = ((int)(mtl->diffuse.r * 255.0f) & 0xFF) >> shift;
                        *p++ = ((int)(mtl->diffuse.g * 255.0f) & 0xFF) >> shift;
                        *p++ = ((int)(mtl->diffuse.b * 255.0f) & 0xFF) >> shift;
                        *p++ = mtl->alpha * 127.0f;
                    }
                } else {
                    for (i = 0; i < batch; i++) {
                        *p++ = 0;
                        *p++ = 0;
                        *p++ = 0;
                        *p++ = mtl->alpha * 127.0f;
                    }
                }
                break;
            case 1:
                NuCvtGeoAddColourRef((unsigned int)p);
                for (i = 0; i < batch; i++) {
                    *p++ = vtx[idx[i]].rgba.r >> shift;
                    *p++ = vtx[idx[i]].rgba.g >> shift;
                    *p++ = vtx[idx[i]].rgba.b >> shift;
                    *p++ = ((vtx[idx[i]].diffuse >> 24) & 0xFF) >> 1;
                    colour.w = vtx[idx[i]].diffuse;
                    lit.c.b = colour.c.b * 0.3f;
                    lit.c.g = colour.c.g * 0.3f;
                    lit.c.r = colour.c.r * 1.0f;
                    lit.c.a = colour.c.a;
                    if (colourref_on) {
                        u32 blue = HALVE_BLUE(colour, shift);

                        colourref_cur->pair[colourref_cur->count].colour = HALVE_ALPHA_RED_GREEN(colour, shift) | blue;
                        colourref_cur->pair[colourref_cur->count].lit = HALVE_COLOUR(lit, shift);
                        colourref_cur->count++;
                    }
                }
                break;
            }
        }
        *(u32 *)p = 0x01000101;
        p += 4;
        *(u32 *)p = 0x14000300;
        p += 4;
        idx += batch;
        nidx = left;
    }
    *dma = vpDmaTag_Close(p);
    return start;
}


unsigned int NuCvtSizeDmaTriStripCFromIdx(short *idx, int nidx, struct numtl_s *mtl, int *qwc) {
    unsigned int size;
    int dmasize;
    int words;
    int fast;
    int left;
    int cont;
    int chunk;
    int batch;
    int used;
    int run;
    int n;
    int k;
    int i;

    fast = 0;
    if (NuCvtPntEna && mtl->attrib.lighting == NUMTL_ALPHA_BLEND) {
        fast = mtl->fxid == 0;
    }
    dmasize = nidx * 9 + 4;
    dmasize = (((nidx + 41) / 42) * 21 + dmasize) * 4;
    words = 8;
    if (dmasize > 0xFFFFF) {
        NuErrorProlog(nucvtgeo_srcfile, 0x64C)(dma_size_msg, dmasize, nidx);
    }
    size = 0x24;
    if (NuCvtPntOn) {
        size += 4;
        size += 0x10;
    }
    left = 0;
    cont = 0;
    while (nidx) {
        if (left == 0) {
            run = *idx;
            if (run > 42) {
                nidx--;
                idx++;
                left = run;
                cont = 0;
            } else {
                batch = 0;
                n = nidx;
                used = 0;
                if (n > 0) {
                    do {
                        batch += run;
                        used += run + 1;
                        n -= run + 1;
                        if (n <= 0) {
                            break;
                        }
                        run = idx[used];
                    } while (batch + run < 43);
                }
            }
        }
        if (left) {
            batch = left > 40 ? 40 : left;
            chunk = batch;
            if (cont) {
                batch += 2;
                nidx += 2;
                idx -= 2;
            }
            used = batch;
        }
        if (batch < 2) {
            NuErrorProlog(nucvtgeo_srcfile, 0x693)(pkt_count_msg);
        }
        if (batch > 42) {
            NuErrorProlog(nucvtgeo_srcfile, 0x695)(pkt_count_msg);
        }
        size += 4;
        size += 4;
        size += 4;
        size += 4;
        size += 4;
        if (NuCvtPntOn) {
            size += 4;
        }
        size += 4;
        if (!NuCvtPntOn) {
            size += 4;
            size += 4;
            size += 4;
            size += 4;
        } else {
            size += 4;
            size += 4;
        }
        info_pktsizecnt++;
        size += 8;
        run = 0;
        k = 0;
        for (i = 0; i < batch; i++) {
            if (run == 0 && left == 0) {
                run = idx[k];
                k++;
            }
            if (!NuCvtPntOn) {
                size += 0x10;
            } else {
                size += 8;
            }
            k++;
            run--;
        }
        if (NuCvtPntOn) {
            size += 4;
        }
        if (left) {
            cont = 1;
        }
        if (NuCvtUvOn) {
            size += 4;
        }
        size += 4;
        run = 0;
        k = 0;
        for (i = 0; i < batch; i++) {
            if (run == 0 && left == 0) {
                run = idx[k];
                k++;
            }
            size += 4;
            if (!fast) {
                size += 2;
                size += 2;
            }
            k++;
            run--;
        }
        if (NuCvtUvOn) {
            size += 4;
        }
        size += 4;
        k = 0;
        run = 0;
        switch (mtl->attrib.colour) {
        case 0:
            if (mtl->attrib.lighting == NUMTL_ALPHA_BLEND) {
                for (i = 0; i < batch; i++) {
                    size += 4;
                }
            } else {
                for (i = 0; i < batch; i++) {
                    size += 4;
                }
            }
            break;
        case 1:
            words += 8 + batch * 8;
            for (i = 0; i < batch; i++) {
                if (run == 0 && left == 0) {
                    run = idx[k];
                    k++;
                }
                size += 4;
                k++;
                run--;
            }
            break;
        }
        size += 8;
        nidx -= used;
        idx += used;
        if (left) {
            left -= chunk;
        }
    }
    size = (size + 15) & 0xFFFFFFF0;
    if (qwc) {
        *qwc = words;
    }
    return size;
}


u8 *NuCvtDmaTriStripCFromIdx(u8 **dma, struct nuvtx_tc1_s *vtx, short *idx, int nidx, struct numtl_s *mtl) {
    union nucvtgiftag_u strip;
    union nucvtgiftag_u fan;
    union nucvtcolour_u lit;
    union nucvtcolour_u colour;
    NuFloatBits packed[3];
    u8 *start;
    u8 *p;
    float minu;
    float minv;
    float maxu;
    float maxv;
    float du;
    float dv;
    int used;
    int k;
    int fast;
    int left;
    int cont;
    int chunk;
    int shift;
    int batch;
    int run;
    int adc;
    int n;
    int i;
    int j;
    float tu;
    float tv;

    fast = 0;
    if (NuCvtPntEna && mtl->attrib.lighting == NUMTL_ALPHA_BLEND) {
        fast = mtl->fxid == 0;
    }
    shift = mtl->tid != -1;
    strip.dw[0] = 0x3002400000008000LL;
    strip.dw[1] = 0x512;
    fan.dw[0] = 0x3002C00000008000LL;
    fan.dw[1] = 0x512;
    start = *dma;
    nucvtgeo_lasttag = (struct nudmatag_s *)start;
    p = vpDmaTag_Cnt(start);
    p[-14] = 5;
    if (NuCvtPntOn) {
        *(u32 *)p = 0x30000000;
        p += 4;
        *(u32 *)p = NuCvtPntSpan[0].i;
        p += 4;
        *(u32 *)p = NuCvtPntSpan[1].i;
        p += 4;
        *(u32 *)p = NuCvtPntSpan[2].i;
        p += 4;
        *(u32 *)p = 0;
        p += 4;
    }
    left = 0;
    cont = 0;
    *(u32 *)p = 0x6C010056;
    p += 4;
    *(u32 *)p = fan.w[0];
    p += 4;
    *(u32 *)p = fan.w[1];
    p += 4;
    *(u32 *)p = fan.w[2];
    p += 4;
    *(u32 *)p = fan.w[3];
    p += 4;
    while (nidx) {
        if (left == 0) {
            run = *idx;
            if (run > 42) {
                nidx--;
                idx++;
                left = run;
                cont = 0;
            } else {
                batch = 0;
                n = nidx;
                used = 0;
                if (n > 0) {
                    do {
                        batch += run;
                        used += run + 1;
                        n -= run + 1;
                        if (n <= 0) {
                            break;
                        }
                        run = idx[used];
                    } while (batch + run < 43);
                }
            }
        }
        if (left) {
            batch = left > 40 ? 40 : left;
            chunk = batch;
            if (cont) {
                batch += 2;
                nidx += 2;
                idx -= 2;
            }
            used = batch;
        }
        if (batch < 2) {
            NuErrorProlog(nucvtgeo_srcfile, 0x7C1)(pkt_count_msg);
        }
        if (batch > 42) {
            NuErrorProlog(nucvtgeo_srcfile, 0x7C3)(pkt_count_msg);
        }
        strip.dw[0] = 0x3002400000000000LL | (batch | 0x8000);
        strip.dw[1] = 0x512;
        *(u32 *)p = 0x6C0180D2;
        p += 4;
        *(u32 *)p = strip.w[0];
        p += 4;
        *(u32 *)p = strip.w[1];
        p += 4;
        *(u32 *)p = strip.w[2];
        p += 4;
        *(u32 *)p = strip.w[3];
        p += 4;
        if (NuCvtPntOn) {
            *(u32 *)p = 0x6C018002;
            p += 4;
            *(u32 *)p = batch * 3;
            p += 4;
            *(float *)p = NuCvtPntSpan[0].f - NuCvtPntMid.x;
            p += 4;
            *(float *)p = NuCvtPntSpan[1].f - NuCvtPntMid.y;
            p += 4;
            *(float *)p = NuCvtPntSpan[2].f - NuCvtPntMid.z;
            p += 4;
        } else {
            *(u32 *)p = 0x6D018002;
            p += 4;
            *(u16 *)p = batch * 3;
            p += 2;
            *(u16 *)p = 0;
            p += 2;
            *(u16 *)p = 0;
            p += 2;
            *(u16 *)p = 0;
            p += 2;
        }
        *(u32 *)p = 0x01000103;
        p += 4;
        if (NuCvtPntOn) {
            *(u32 *)p = 0x05000001;
            p += 4;
            *(u32 *)p = (batch << 16) | 0x6D008003;
            p += 4;
        } else {
            *(u32 *)p = (batch << 16) | 0x6C008003;
            p += 4;
        }
        run = 0;
        adc = 2;
        if (nugscn_generate_packetinfo) {
            info_pktdata[info_pktcnt].offset = p - info_pktbuff;
            info_pktdata[info_pktcnt].count = batch;
        }
        info_pktcnt++;
        k = 0;
        for (i = 0; i < batch; i++) {
            if (run == 0 && left == 0) {
                run = idx[k];
                adc = 2;
                k++;
            }
            if (!NuCvtPntOn) {
                *(float *)p = vtx[idx[k]].pnt.x;
                p += 4;
                *(float *)p = vtx[idx[k]].pnt.y;
                p += 4;
                *(float *)p = vtx[idx[k]].pnt.z;
                p += 4;
                *(float *)p = vtx[idx[k]].nrm.x;
            } else {
                packed[0].f = NuCvtPntSpan[0].f + (vtx[idx[k]].pnt.x - NuCvtPntMid.x);
                packed[1].f = NuCvtPntSpan[1].f + (vtx[idx[k]].pnt.y - NuCvtPntMid.y);
                packed[2].f = NuCvtPntSpan[2].f + (vtx[idx[k]].pnt.z - NuCvtPntMid.z);
                *(u16 *)p = packed[0].i - NuCvtPntSpan[0].i;
                p += 2;
                *(u16 *)p = packed[1].i - NuCvtPntSpan[1].i;
                p += 2;
                *(u16 *)p = packed[2].i - NuCvtPntSpan[2].i;
                p += 2;
                *(u16 *)p = vtx[idx[k]].nrm.x;
            }
            if (adc) {
                *(u16 *)p = 0x8001;
            } else {
                *(u16 *)p = 0;
            }
            if (adc) {
                adc--;
                if (adc == 0) {
                    info_stripcount++;
                }
            } else {
                info_tristripcount++;
            }
            if (!NuCvtPntOn) {
                p += 4;
            } else {
                p += 2;
            }
            k++;
            run--;
        }
        if (NuCvtPntOn) {
            *(u32 *)p = 0x05000000;
            p += 4;
        }
        if (left) {
            cont = 1;
        }
        if (NuCvtUvOn) {
            *(u32 *)p = 0x05000001;
            p += 4;
        }
        if (fast) {
            *(u32 *)p = (batch << 16) | 0x65008004;
            p += 4;
        } else {
            *(u32 *)p = (batch << 16) | 0x6D008004;
            p += 4;
        }
        minu = minv = 0.0f;
        run = 0;
        k = 0;
        if (left) {
            minu = vtx[idx[0]].tc[0];
            minv = vtx[idx[0]].tc[1];
            maxu = minu;
            maxv = minv;
            for (j = 1; j < batch; j++) {
                tu = vtx[idx[j]].tc[0];
                minu = NUCVT_MIN(tu, minu);
                tv = vtx[idx[j]].tc[1];
                minv = NUCVT_MIN(tv, minv);
                maxu = NUCVT_MAX(tu, maxu);
                maxv = NUCVT_MAX(tv, maxv);
            }
            minu = floor(minu);
            minv = floor(minv);
            if (NuFabs(minu - maxu) > 8.0f || minu < -8.0f) {
                minu += 8.0f;
            }
            if (NuFabs(minv - maxv) > 8.0f || minv < -8.0f) {
                minv += 8.0f;
            }
        }
        for (i = 0; i < batch; i++) {
            if (run == 0 && left == 0) {
                run = idx[k];
                k++;
                minu = vtx[idx[k]].tc[0];
                minv = vtx[idx[k]].tc[1];
                maxu = minu;
                maxv = minv;
                for (j = 1; j < run; j++) {
                    tu = vtx[idx[k + j]].tc[0];
                    minu = NUCVT_MIN(tu, minu);
                    tv = vtx[idx[k + j]].tc[1];
                    minv = NUCVT_MIN(tv, minv);
                    maxu = NUCVT_MAX(tu, maxu);
                    maxv = NUCVT_MAX(tv, maxv);
                }
                minu = floor(minu);
                minv = floor(minv);
                if (NuFabs(minu - maxu) > 8.0f || minu < -8.0f) {
                    minu += 8.0f;
                }
                if (NuFabs(minv - maxv) > 8.0f || minv < -8.0f) {
                    minv += 8.0f;
                }
            }
            du = vtx[idx[k]].tc[0] - minu;
            dv = vtx[idx[k]].tc[1] - minv;
            if (du > 15.0f) {
                NuDebugMsgProlog(nucvtgeo_srcfile, 0x88F)(u_high_msg, du, left, minu);
            }
            if (dv > 15.0f) {
                NuDebugMsgProlog(nucvtgeo_srcfile, 0x891)(v_high_msg, dv, left, minv);
            }
            if (du < -16.0f) {
                NuDebugMsgProlog(nucvtgeo_srcfile, 0x893)(u_low_msg, du, left, minu);
            }
            if (dv < -16.0f) {
                NuDebugMsgProlog(nucvtgeo_srcfile, 0x895)(v_low_msg, dv, left, minv);
            }
            *(short *)p = du * 4096.0f;
            p += 2;
            *(short *)p = dv * 4096.0f;
            p += 2;
            if (!fast) {
                *(short *)p = vtx[idx[k]].nrm.y * 4096.0f;
                p += 2;
                *(short *)p = vtx[idx[k]].nrm.z * 4096.0f;
                p += 2;
            }
            k++;
            run--;
        }
        if (NuCvtUvOn) {
            *(u32 *)p = 0x05000000;
            p += 4;
        }
        *(u32 *)p = (batch << 16) | 0x6E00C005;
        p += 4;
        k = 0;
        run = 0;
        if (show_strip) {
            for (i = 0; i < batch; i++) {
                *(u32 *)p = 0x800000FF;
                p += 4;
            }
        } else {
            switch (mtl->attrib.colour) {
            case 0:
                if (mtl->attrib.lighting == NUMTL_ALPHA_BLEND) {
                    for (i = 0; i < batch; i++) {
                        *p++ = ((int)(mtl->diffuse.r * 255.0f) & 0xFF) >> shift;
                        *p++ = ((int)(mtl->diffuse.g * 255.0f) & 0xFF) >> shift;
                        *p++ = ((int)(mtl->diffuse.b * 255.0f) & 0xFF) >> shift;
                        *p++ = mtl->alpha * 127.0f;
                    }
                } else {
                    for (i = 0; i < batch; i++) {
                        *p++ = 0;
                        *p++ = 0;
                        *p++ = 0;
                        *p++ = mtl->alpha * 127.0f;
                    }
                }
                break;
            case 1:
                NuCvtGeoAddColourRef((unsigned int)p);
                for (i = 0; i < batch; i++) {
                    if (run == 0 && left == 0) {
                        run = idx[k];
                        k++;
                    }
                    *p++ = vtx[idx[k]].rgba.r >> shift;
                    *p++ = vtx[idx[k]].rgba.g >> shift;
                    *p++ = vtx[idx[k]].rgba.b >> shift;
                    *p++ = ((vtx[idx[k]].diffuse >> 24) & 0xFF) >> 1;
                    colour.w = vtx[idx[k]].diffuse;
                    lit.c.b = colour.c.b * 0.3f;
                    lit.c.g = colour.c.g * 0.3f;
                    lit.c.r = colour.c.r * 1.0f;
                    lit.c.a = colour.c.a;
                    if (colourref_on) {
                        u32 blue = (colour.w & 0xFF) >> shift;

                        colourref_cur->pair[colourref_cur->count].colour = (colour.c.a >> 1) << 24 | (((colour.w >> 16) & 0xFF) >> shift) << 16
                            | (((colour.w >> 8) & 0xFF) >> shift) << 8 | blue;
                        colourref_cur->pair[colourref_cur->count].lit = (lit.c.a >> 1) << 24 | (((lit.w >> 16) & 0xFF) >> shift) << 16
                            | (((lit.w >> 8) & 0xFF) >> shift) << 8 | ((lit.w & 0xFF) >> shift);
                        colourref_cur->count++;
                    }
                    k++;
                    run--;
                }
                break;
            }
        }
        *(u32 *)p = 0x01000101;
        p += 4;
        *(u32 *)p = 0x14000300;
        p += 4;
        idx += used;
        nidx -= used;
        if (left) {
            left -= chunk;
        }
    }
    *dma = vpDmaTag_Close(p);
    return start;
}


void NuPs2CreateFaceOn(struct nugobj_s *gobj) {
    struct nufaceongeom_s *geom;
    NuColourRef *colourref;
    u8 *buffer;
    u8 *dma;
    int words;
    int total;
    unsigned int size;
    int left;
    int n;

    total = 0;
    size = 0;
    geom = gobj->faceon_geom;
    if (geom) {
        for (; geom; geom = geom->next) {
            for (left = geom->nfaceons; left > 0;) {
                n = left > 6000 ? 6000 : left;
                left -= 6000;
                switch (geom->faceon_type) {
                case 0:
                    size += NuCvtSizeDmaFaceOn(geom->faceons, n, geom->mtl, &words);
                    total += words;
                    break;
                case 1:
                    size += NuCvtSizeDmaFaceY(geom->faceons, n, geom->mtl, &words);
                    total += words;
                    break;
                }
            }
            size += 0x10;
        }
        buffer = NuMemAllocFn(size + total, nucvtgeo_srcfile, 0x937);
        dma = buffer;
        if (nugscn_generate_colourref) {
            colourref = (NuColourRef *)(buffer + size);
            gobj->blend = colourref;
            NuCvtGeoBeginColourRef(colourref);
        }
        for (geom = gobj->faceon_geom; geom; geom = geom->next) {
            NuDebugMsgProlog(nucvtgeo_srcfile, 0x942)(faceon_alpha_msg, geom->mtl->attrib.alpha);
            geom->dmastream = dma;
            for (left = geom->nfaceons; left > 0;) {
                n = left > 6000 ? 6000 : left;
                left -= 6000;
                switch (geom->faceon_type) {
                case 0:
                    NuCvtDmaFaceOn(&dma, geom->faceons, n, geom->mtl);
                    break;
                case 1:
                    NuCvtDmaFaceY(&dma, geom->faceons, n, geom->mtl);
                    break;
                default:
                    NuDebugMsgProlog(nucvtgeo_srcfile, 0x955)(faceon_type_msg, geom->faceon_type);
                    break;
                }
            }
            dma = vpDmaTag_Ret(dma);
        }
    }
    if (nugscn_generate_colourref) {
        *(int *)gobj->pad68 = (u8 *)NuCvtGeoEndColourRef() - (u8 *)colourref;
    }
}


void NuCvtCalcFastPoint(struct nuvec_s *a, struct nuvec_s *b, struct numtl_s *mtl) {
    int point;

    if (mtl) {
        if (mtl->attrib.lighting == NUMTL_ALPHA_BLEND && mtl->fxid == 0
            && (mtl->alpha_reg & ALPHA_REG_TOP_MASK) == 0) {
            point = 1;
        } else {
            point = 0;
        }
        if ((mtl->alpha_reg & ALPHA_REG_TOP_MASK) != 0 || !NuCvtPntEna) {
            NuCvtUvOn = 1;
        } else {
            NuCvtUvOn = 0;
        }
        if (NuCvtPntEna) {
            NuCvtPntOn = point;
        } else {
            NuCvtPntOn = 0;
        }
    } else {
        NuCvtPntOn = 0;
        NuCvtUvOn = 1;
    }
    NuCvtPntSpan[0].f = b->x - a->x;
    NuCvtPntSpan[1].f = b->y - a->y;
    NuCvtPntSpan[2].f = b->z - a->z;
    if (NuCvtPntSpan[0].f >= NuCvtMaxSpan || NuCvtPntSpan[1].f >= NuCvtMaxSpan
        || NuCvtPntSpan[2].f >= NuCvtMaxSpan) {
        NuCvtPntOn = 0;
    }
    NuCvtPntSpan[0].i = (NuCvtPntSpan[0].i & FLOAT_EXP_MASK) + SPAN_EXP_BIAS;
    NuCvtPntSpan[1].i = (NuCvtPntSpan[1].i & FLOAT_EXP_MASK) + SPAN_EXP_BIAS;
    NuCvtPntSpan[2].i = (NuCvtPntSpan[2].i & FLOAT_EXP_MASK) + SPAN_EXP_BIAS;
    NuCvtPntMid.x = (b->x + a->x) * 0.5f;
    NuCvtPntMid.y = (b->y + a->y) * 0.5f;
    NuCvtPntMid.z = (b->z + a->z) * 0.5f;
}


void NuPs2CreateRenderStream(struct nugobj_s *gobj) {
    struct nugeom_s *geom;
    struct nuprim_s *prim;
    NuColourRef *colourref;
    u8 *buffer;
    u8 *dma;
    short *idx;
    short *strip;
    unsigned int size;
    int words;
    int totalwords;
    int build;
    int left;
    int n;
    int next;
    int dmasize;
    int len;

    build = 0;
    totalwords = 0;
    size = 0;
    geom = gobj->geom;
    info_pktsizecnt = 0;
    info_pktcnt = 0;
    NuCvtCalcFastPoint(&gobj->min, &gobj->max, 0);
    for (; geom; geom = geom->next) {
        if (!geom->skins && !geom->vtxskininfo) {
            build = 1;
        }
        if (NuCvtPntEna) {
            NuCvtCalcFastPoint(&gobj->min, &gobj->max, geom->mtl);
        }
        prim = geom->prim;
        if (prim) {
            switch (prim->type) {
            case NUPRIM_TRILIST:
                idx = prim->vid;
                for (left = prim->cnt; left > 0;) {
                    n = left > 6000 ? 6000 : left;
                    left -= 6000;
                    size += NuCvtSizeDmaTriCFromIdx(idx, n, geom->mtl, &words);
                    idx += n;
                    totalwords += words;
                }
                break;
            case NUPRIM_TRISTRIP:
                idx = prim->vid;
                for (left = prim->cnt; left > 0;) {
                    int limit = 6000;

                    len = 0;
                    strip = idx;
                    while (len < left) {
                        next = len + (*strip + 1);
                        strip += *strip + 1;
                        if (next > limit) {
                            break;
                        }
                        len = next;
                    }
                    n = len;
                    left -= n;
                    size += NuCvtSizeDmaTriStripCFromIdx(idx, n, geom->mtl, &words);
                    idx += n;
                    totalwords += words;
                }
                break;
            default:
                NuErrorProlog(nucvtgeo_srcfile, 0xA11)(prim_type_msg);
                break;
            }
        }
        size += 0x10;
    }
    if (build) {
        buffer = NuMemAllocFn(size + totalwords, nucvtgeo_srcfile, 0xA22);
        dma = buffer;
        if (nugscn_generate_packetinfo) {
            info_pktdata = NuMemAllocFn(info_pktsizecnt * 8, nucvtgeo_srcfile, 0xA26);
            info_pktbuff = buffer;
        }
        info_pktsizecnt = 0;
        if (nugscn_generate_colourref) {
            colourref = (NuColourRef *)(buffer + size);
            gobj->blend = colourref;
            NuCvtGeoBeginColourRef(colourref);
        }
        for (geom = gobj->geom; geom; geom = geom->next) {
            if (geom->skins || geom->vtxskininfo) {
                continue;
            }
            geom->dmastream = dma;
            info_pktbuff = dma;
            prim = geom->prim;
            if (prim) {
                switch (prim->type) {
                case NUPRIM_TRILIST:
                    idx = prim->vid;
                    for (left = prim->cnt; left > 0;) {
                        n = left > 6000 ? 6000 : left;
                        left -= 6000;
                        dmasize = NuCvtSizeDmaTriCFromIdx(idx, n, geom->mtl, 0);
                        memset(dma, 0, dmasize);
                        NuCvtDmaTriCFromIdx(&dma, geom->vtxptr, idx, n, geom->mtl);
                        idx += n;
                    }
                    break;
                case NUPRIM_TRISTRIP:
                    idx = prim->vid;
                    for (left = prim->cnt; left > 0;) {
                        int limit = 6000;

                        len = 0;
                        strip = idx;
                        while (len < left) {
                            next = len + (*strip + 1);
                            strip += *strip + 1;
                            if (next > limit) {
                                break;
                            }
                            len = next;
                        }
                        n = len;
                        left -= n;
                        dmasize = NuCvtSizeDmaTriStripCFromIdx(idx, n, geom->mtl, 0);
                        memset(dma, 0, dmasize);
                        NuCvtDmaTriStripCFromIdx(&dma, geom->vtxptr, idx, n, geom->mtl);
                        idx += n;
                    }
                    break;
                default:
                    NuErrorProlog(nucvtgeo_srcfile, 0xA67)(prim_type_msg);
                    break;
                }
            }
            dma = vpDmaTag_Ret(dma);
            geom->dmasize = dma - (u8 *)geom->dmastream;
        }
        if (nugscn_generate_colourref) {
            gobj->tricount = (u8 *)NuCvtGeoEndColourRef() - (u8 *)colourref;
        }
        NuCvtPntOn = 0;
    }
}


void NuPs2DestroyRenderStream(struct nugobj_s *gobj) {
    struct nugeom_s *geom;

    geom = gobj->geom;
    if (geom && geom->dmastream) {
        NuMemFreeFn(geom->dmastream, nucvtgeo_srcfile, 0xA98);
    }
}
