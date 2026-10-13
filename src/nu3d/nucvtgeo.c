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

typedef unsigned char u8;
typedef unsigned long long u64;

typedef struct nucolourref_s {
    unsigned int addr;
    int count;
} NuColourRef;

typedef struct NuGeom {
    struct NuGeom *next;
    void *mtl;
    void *dmastream;
} NuGeom;

typedef struct NuGobj {
    struct NuGobj *sysnext;
    struct NuGobj *syslast;
    int type;
    NuGeom *geom;
} NuGobj;

typedef struct NuVec {
    float x;
    float y;
    float z;
} NuVec;

typedef union NuFloatBits {
    float f;
    int i;
} NuFloatBits;

typedef struct NuMtl {
    u8 pad000[0x168];
    unsigned int attrib_lo;
    unsigned int attrib_mid : 16;
    unsigned int alpha : 2;
    unsigned int attrib_hi : 14;
    u8 pad170[0x1A8 - 0x170];
    u64 alpha_reg;
    u8 pad1B0[0x1C5 - 0x1B0];
    u8 fxid;
} NuMtl;


extern void NuMemFreeFn(void *ptr, char *file, int line);
extern u8 *vpDmaTag_Cnt(u8 *cur);

extern int NuCvtPntEna;
extern int NuCvtPntOn;
extern int NuCvtUvOn;
extern float NuCvtMaxSpan;
extern NuFloatBits NuCvtPntSpan[3];
extern NuVec NuCvtPntMid;
extern void *info_pktdata;

extern int D_0062EEDC;
extern int D_0062EEE0;
extern NuColourRef *D_00633118;
extern NuColourRef *D_0063311C;
extern char D_00615538[];

#define colourref_on D_0062EEDC
#define colourref_offset D_0062EEE0
#define colourref_list D_00633118
#define colourref_cur D_0063311C
#define nucvtgeo_srcfile D_00615538

#define NUMTL_ALPHA_BLEND 2
#define ALPHA_REG_TOP_MASK 0xFF00000000000000ULL
#define FLOAT_EXP_MASK 0x7F800000
#define SPAN_EXP_BIAS 0x04008000

#define NEXT_COLOURREF(ref) ((NuColourRef *)((char *)(ref) + ((ref)->count * 8 + 8)))

void NuCvtGeoAddColourRef(unsigned int addr);


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


void NuCvtCalcFastPoint(NuVec *a, NuVec *b, NuMtl *mtl) {
    int point;

    if (mtl) {
        if (mtl->alpha == NUMTL_ALPHA_BLEND && mtl->fxid == 0
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


void NuPs2DestroyRenderStream(NuGobj *gobj) {
    NuGeom *geom;

    geom = gobj->geom;
    if (geom && geom->dmastream) {
        NuMemFreeFn(geom->dmastream, nucvtgeo_srcfile, 0xA98);
    }
}


void NuCvtGeoSetColourRefOffset(int offset) {
    colourref_offset = offset;
}


void NuCvtGeoFixup(NuColourRef *ref, int offset) {
    while (ref->count != 0) {
        ref->addr = offset + ref->addr;
        ref = NEXT_COLOURREF(ref);
    }
}


void NuCvtGeoBeginColourRef(NuColourRef *list) {
    colourref_list = list;
    colourref_on = 1;
    colourref_cur = 0;
    colourref_offset = 0;
}


NuColourRef *NuCvtGeoEndColourRef(void) {
    NuCvtGeoAddColourRef(0);
    colourref_on = 0;
    return NEXT_COLOURREF(colourref_cur);
}


unsigned int NuCvtSizeDmaFaceOn(void *dma, int count, void *unused, int *qwc) {
    unsigned int size;
    int words;
    int batch;
    int i;

    words = 8;
    size = 0x10;
    while (count) {
        batch = count > 20 ? 20 : count;
        size += 0x28;
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


unsigned int NuCvtSizeDmaFaceY(void *dma, int count, void *unused, int *qwc) {
    unsigned int size;
    int words;
    int batch;
    int i;

    words = 8;
    size = 0x24;
    while (count) {
        batch = count > 10 ? 10 : count;
        size += 0x28;
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


void NuCvtSetMaxSpan(float span) {
    NuCvtMaxSpan = span;
    if (span <= 0.0f) {
        NuCvtPntEna = 0;
    } else {
        NuCvtPntEna = 1;
    }
}


void NuCvtGeoAddColourRef(unsigned int addr) {
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


void NuCvtDestroyPacketInfo(void) {
    if (info_pktdata) {
        NuMemFreeFn(info_pktdata, nucvtgeo_srcfile, 0xA9E);
    }
}
