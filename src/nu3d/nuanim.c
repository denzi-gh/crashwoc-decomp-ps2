/*
 * Unit: nu3d/nuanim
 *
 * Functions:
 *   0x00140508 NuAnimDataWrite
 *   0x00140850 NuAnimDataRead
 *   0x00140c00 NuAnimDataLoad
 *   0x00140d40 NuAnimDataFixPtrs
 *   0x00140f38 NuAnimData2FixPtrs
 *   0x00141080 NuAnimCurveCalcVal2
 *   0x00141220 NuAnimCurve2CalcVal
 *   0x00141500 NuAnimCurveSetApplyToJoint
 *   0x00141898 NuAnimCurveSetApplyBlendToJoint
 *   0x00141e88 NuAnimCurveSetApplyBlendToJoint2
 *   0x001426f8 NuAnimCurve2SetApplyBlendToJoint2
 *   0x00142f08 NuAnimCurveSetApplyToJoint2
 *   0x00143510 NuAnimCurve2SetApplyToJoint
 *   0x00143a70 NuAnimCurveSetApplyToJointBasic
 *   0x00143dc8 NuAnimCurve2SetApplyToJointBasic
 *   0x001440e8 NuAnimCurveSetApplyToMatrix
 *   0x00144350 NuAnimCurve2SetApplyToMatrix
 *   0x00144518 NuAnimDataLoadBuff
 *   0x001445f8 NuAnimDataDestroy
 *   0x00144688 NuAnimDataCalcTime
 *   0x001447c8 NuAnimDataCreate
 *   0x00144838 NuAnimCurveSetCreate
 *   0x001448f8 NuAnimCurveSetDestroy
 *   0x001449e0 NuAnimCurveSetWrite
 *   0x00144b68 NuAnimCurveCreate
 *   0x00144c18 NuAnimCurveDestroy
 *   0x00144c68 NuAnimCurveWrite
 *   0x00144d50 NuAnimCurveRead
 *   0x00144e28 NuAnimKeyWrite
 *   0x00144e88 NuAnimKeyRead
 *   0x00144ee8 NuAnimDataFindVersion
 *   0x00144f50 NuAnimData2LoadBuff
 *   0x00145030 NuAnimData2CalcTime
 *   0x00145170 NuAnimDataChunkCreate
 *   0x00145220 NuAnimDataChunkDestroy
 *   0x001452f0 buildBitCountTable
 *   0x00145358 NuAnimCurveScale
 */

typedef struct nuanimkey_s {
    float time;
    float dtime;
    float c;
    float d;
} NuAnimKey;

typedef struct nuanimcurve_s {
    unsigned char mask[4];
    NuAnimKey *animkeys;
    unsigned int numkeys;
    unsigned int flags;
} NuAnimCurve;

typedef struct nuanimcurveset_s {
    int flags;
    float *constants;
    NuAnimCurve **set;
    signed char ncurves;
} NuAnimCurveSet;

typedef struct nuanimdatachunk_s {
    int numnodes;
    int num_valid_animcurvesets;
    NuAnimCurveSet **animcurvesets;
} NuAnimDataChunk;

typedef struct nuanimdata_s {
    float time;
    char *node_names;
    int nchunks;
    NuAnimDataChunk **chunks;
} NuAnimData;

typedef struct nuanimdatahdr_s {
    int version;
    int address_offset;
    NuAnimData *data;
} NuAnimDataHdr;

typedef struct nuanimcurvedata2_s {
    unsigned int *mask;
    unsigned short *key_ixs;
    NuAnimKey *keys;
} NuAnimCurveData2;

typedef union nuanimcurve2_u {
    float constant;
    NuAnimCurveData2 *curvedata;
} NuAnimCurve2;

typedef struct nuanimdata2_s {
    float time;
    short nnodes;
    short ncurves;
    short nchunks;
    NuAnimCurve2 *curves;
    signed char *curveflags;
    char *curvesetflags;
} NuAnimData2;

typedef struct nuanimtime_s {
    float time;
    float time_offset;
    int chunk;
    unsigned int time_mask;
    unsigned int time_byte;
} NuAnimTime;


typedef void (*NuErrorFunc)(const char *fmt, ...);

extern NuErrorFunc NuErrorProlog(const char *file, int line);
extern void *NuMemAllocFn(int size, char *file, int line);
extern void NuMemFreeFn(void *ptr, char *file, int line);
extern void *memset(void *dst, int value, unsigned int size);
extern int NuFileOpen(char *name, int mode);
extern int NuFileRead(int fh, void *buf, int size);
extern void NuFileClose(int fh);
extern int NuFileLoadBuffer(char *name, void *buf, int size);
extern void NuFileWriteFloat(int fh, float value);
extern float NuFileReadFloat(int fh);
extern void NuAnimDataChunkDestroy(NuAnimDataChunk *chunk);

extern unsigned char D_006ADC00[256];
extern int D_0062EE18;
extern char D_00614A60[];
extern char D_00614AD0[];
extern char D_00614B20[];

#define bitcount_table D_006ADC00
#define bitcount_table_built D_0062EE18
#define nuanim_srcfile D_00614A60
#define nuanim_load_failed_msg D_00614AD0
#define nuanim_new_format_msg D_00614B20

#define NUANIM_FIXPTR(ptr, base) ((ptr) ? (void *)((char *)(ptr) + (base)) : 0)


inline void buildBitCountTable(void) {
    int value;
    int bit;

    for (value = 0; value < 256; value++) {
        bitcount_table[value] = 0;
        for (bit = 0; bit < 8; bit++) {
            if ((value >> bit) & 1) {
                bitcount_table[value]++;
            }
        }
    }
    bitcount_table_built = 1;
}


NuAnimData *NuAnimDataFixPtrs(NuAnimData *data, int base) {
    NuAnimDataChunk *chunk;
    NuAnimCurveSet *curveset;
    NuAnimCurve *curve;
    int i;
    int j;
    int k;

    if (!bitcount_table_built) {
        buildBitCountTable();
    }
    data = NUANIM_FIXPTR(data, base);
    data->node_names = NUANIM_FIXPTR(data->node_names, base);
    data->chunks = NUANIM_FIXPTR(data->chunks, base);
    if (data->chunks) {
        for (i = 0; i < data->nchunks; i++) {
            data->chunks[i] = NUANIM_FIXPTR(data->chunks[i], base);
            chunk = data->chunks[i];
            if (!chunk) {
                continue;
            }
            chunk->animcurvesets = NUANIM_FIXPTR(chunk->animcurvesets, base);
            if (!chunk->animcurvesets) {
                continue;
            }
            for (j = 0; j < chunk->numnodes; j++) {
                chunk->animcurvesets[j] = NUANIM_FIXPTR(chunk->animcurvesets[j], base);
                curveset = chunk->animcurvesets[j];
                if (!curveset) {
                    continue;
                }
                curveset->constants = NUANIM_FIXPTR(curveset->constants, base);
                curveset->set = NUANIM_FIXPTR(curveset->set, base);
                if (!curveset->set) {
                    continue;
                }
                for (k = 0; k < curveset->ncurves; k++) {
                    curveset->set[k] = NUANIM_FIXPTR(curveset->set[k], base);
                    curve = curveset->set[k];
                    if (curve) {
                        curve->animkeys = NUANIM_FIXPTR(curve->animkeys, base);
                    }
                }
            }
        }
    }
    return data;
}


NuAnimData2 *NuAnimData2FixPtrs(NuAnimData2 *data, int base) {
    NuAnimCurve2 *curve;
    int i;
    int count;

    if (!bitcount_table_built) {
        buildBitCountTable();
    }
    data = NUANIM_FIXPTR(data, base);
    if (data) {
        count = data->nnodes * data->ncurves;
        data->curves = NUANIM_FIXPTR(data->curves, base);
        data->curveflags = NUANIM_FIXPTR(data->curveflags, base);
        data->curvesetflags = NUANIM_FIXPTR(data->curvesetflags, base);
        for (i = 0; i < count; i++) {
            if (data->curveflags[i]) {
                curve = &data->curves[i];
                curve->curvedata = NUANIM_FIXPTR(curve->curvedata, base);
                curve->curvedata->mask = NUANIM_FIXPTR(curve->curvedata->mask, base);
                curve->curvedata->key_ixs = NUANIM_FIXPTR(curve->curvedata->key_ixs, base);
                curve->curvedata->keys = NUANIM_FIXPTR(curve->curvedata->keys, base);
            }
        }
    }
    return data;
}


float NuAnimCurveCalcVal2(NuAnimCurve *curve, NuAnimTime *atime) {
    int nkey;
    NuAnimKey *key;
    NuAnimKey *next;
    float u;
    float dt;
    float a;
    float b;
    float c;

    switch (atime->time_byte) {
    case 0:
        nkey = bitcount_table[curve->mask[0] & atime->time_mask];
        break;
    case 1:
        nkey = bitcount_table[curve->mask[0]];
        nkey += bitcount_table[curve->mask[1] & atime->time_mask];
        break;
    case 2:
        nkey = bitcount_table[curve->mask[0]];
        nkey += bitcount_table[curve->mask[1]];
        nkey += bitcount_table[curve->mask[2] & atime->time_mask];
        break;
    case 3:
        nkey = bitcount_table[curve->mask[0]];
        nkey += bitcount_table[curve->mask[1]];
        nkey += bitcount_table[curve->mask[2]];
        nkey += bitcount_table[curve->mask[3] & atime->time_mask];
        break;
    }
    next = &curve->animkeys[nkey];
    key = next - 1;
    if (!(curve->flags & 1)) {
        dt = next->time - key->time;
        u = (atime->time - key->time) * key->dtime;
        b = key->c * dt;
        c = next->c * dt;
        a = key->d - next->d;
        return u * (u * (u * (a + a + b + c) + a * -3.0f - (b + b) - c) + b) + key->d;
    }
    return key->d;
}


NuAnimData *NuAnimDataLoadBuff(char *file, void **buffer, void **buffend) {
    NuAnimDataHdr *hdr;
    int size;

    *buffer = (void *)(((unsigned int)*buffer + 0xF) & 0xFFFFFFF0);
    size = NuFileLoadBuffer(file, *buffer, (char *)*buffend - (char *)*buffer);
    if (!size) {
        NuErrorProlog(nuanim_srcfile, 0x2F8)(nuanim_load_failed_msg, file);
    }
    hdr = *buffer;
    *buffer = (char *)hdr + size;
    if (hdr->version != 1) {
        NuErrorProlog(nuanim_srcfile, 0x2FE)(nuanim_new_format_msg, file);
    }
    hdr->address_offset = (int)hdr - hdr->address_offset;
    hdr->data = NuAnimDataFixPtrs(hdr->data, hdr->address_offset);
    return hdr->data;
}


void NuAnimDataDestroy(NuAnimData *data) {
    int i;

    for (i = 0; i < data->nchunks; i++) {
        NuAnimDataChunkDestroy(data->chunks[i]);
    }
    if (data->node_names) {
        NuMemFreeFn(data->node_names, nuanim_srcfile, 0x2A5);
    }
    NuMemFreeFn(data, nuanim_srcfile, 0x2A7);
}


NuAnimData *NuAnimDataCreate(int nchunks) {
    NuAnimData *data;
    int size;

    size = nchunks * sizeof(NuAnimDataChunk *) + sizeof(NuAnimData);
    data = NuMemAllocFn(size, nuanim_srcfile, 0x291);
    memset(data, 0, size);
    data->nchunks = nchunks;
    data->chunks = (NuAnimDataChunk **)(data + 1);
    return data;
}


void NuAnimCurveDestroy(NuAnimCurve *curve) {
    if (curve->animkeys) {
        NuMemFreeFn(curve->animkeys, nuanim_srcfile, 0x479);
    }
    NuMemFreeFn(curve, nuanim_srcfile, 0x47A);
}


void NuAnimKeyWrite(int fh, NuAnimKey *key) {
    if (key) {
        NuFileWriteFloat(fh, key->time);
        NuFileWriteFloat(fh, key->dtime);
        NuFileWriteFloat(fh, key->c);
        NuFileWriteFloat(fh, key->d);
    }
}


int NuAnimKeyRead(int fh, NuAnimKey *key) {
    key->time = NuFileReadFloat(fh);
    key->dtime = NuFileReadFloat(fh);
    key->c = NuFileReadFloat(fh);
    key->d = NuFileReadFloat(fh);
    return 1;
}


int NuAnimDataFindVersion(char *file) {
    NuAnimDataHdr hdr;
    int fh;

    memset(&hdr, 0, sizeof(hdr));
    fh = NuFileOpen(file, 0);
    if (fh) {
        NuFileRead(fh, &hdr, sizeof(hdr));
        NuFileClose(fh);
        return hdr.version;
    }
    return -1;
}


inline void NuAnimCurveScale(NuAnimCurve *curve, float scale, float offset) {
    unsigned int i;
    NuAnimKey *key;

    for (i = 0; i < curve->numkeys; i++) {
        key = &curve->animkeys[i];
        key->time = key->time * scale + offset;
        key->dtime = key->dtime / scale;
        key->c = key->c / scale;
    }
}
