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

typedef union nuanimmask_u {
    unsigned char bytes[4];
    unsigned int bits;
} NuAnimMask;

typedef struct nuanimcurve_s {
    NuAnimMask mask;
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
    NuAnimKey *keys;
    NuAnimCurve *curves;
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

typedef struct nuanimkey2_s {
    float time;
    float dtime;
    float value;
    float slope;
} NuAnimKey2;

typedef struct nuanimstepkey_s {
    float value;
    int reserved;
} NuAnimStepKey;

typedef struct nuanimcurvedata2_s {
    NuAnimMask *mask;
    unsigned short *key_ixs;
    NuAnimKey2 *keys;
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

typedef struct nuanimdata2hdr_s {
    int version;
    int address_offset;
    NuAnimData2 *data;
} NuAnimData2Hdr;

typedef struct nuanimtime_s {
    float time;
    float time_offset;
    int chunk;
    unsigned int time_mask;
    unsigned int time_byte;
} NuAnimTime;

typedef struct nuvec_s {
    float x;
    float y;
    float z;
} NuVec;

typedef struct nuangvec_s {
    int x;
    int y;
    int z;
} NuAngVec;

typedef struct numtx_s {
    float _00, _01, _02, _03;
    float _10, _11, _12, _13;
    float _20, _21, _22, _23;
    float _30, _31, _32, _33;
} NuMtx;

typedef struct nujointdata_s {
    NuMtx orient;
    NuVec locator_offset;
    char *name;
    unsigned char parent_ix;
    unsigned char flags;
    char pad[10];
} NuJointData;

typedef struct nujointanim_s {
    float rx;
    float ry;
    float rz;
    float tx;
    float ty;
    float tz;
    float sx;
    float sy;
    float sz;
    short max_rx;
    short max_ry;
    short max_rz;
    short min_rx;
    short min_ry;
    short min_rz;
    unsigned char joint_id;
    signed char flags;
    char pad[2];
} NuJointAnim;

typedef struct nunode_s {
    char *name;
    int id;
    int type;
    int num_children;
    struct nunode_s *parent;
    struct nunode_s *child;
    struct nunode_s *next;
    void *data;
} NuNode;

typedef struct nujointnode_s {
    NuNode node;
    NuMtx orient;
    NuMtx mtx;
    NuMtx wmtx;
    char pad_e0[0x18];
    NuVec scale;
    NuVec pivot;
    unsigned char flags;
} NuJointNode;


typedef void (*NuErrorFunc)(const char *fmt, ...);

extern NuErrorFunc NuErrorProlog(const char *file, int line);
extern void *NuMemAllocFn(int size, char *file, int line);
extern void NuMemFreeFn(void *ptr, char *file, int line);
extern void *memset(void *dst, int value, unsigned int size);
extern int NuFileOpen(char *name, int mode);
extern int NuFileRead(int fh, void *buf, int size);
extern void NuFileClose(int fh);
extern int NuFileLoadBuffer(char *name, void *buf, int size);
extern int NuFileWriteInt(int fh, int value);
extern int NuFileWrite(int fh, void *data, int size);
extern int NuFileWriteChar(int fh, int value);
extern int strlen(const char *s);
extern int NuFileReadInt(int fh);
extern int NuFileWriteFloat(int fh, float value);
extern float NuFileReadFloat(int fh);
extern char NuFileReadChar(int fh);
extern NuAnimData *NuAnimDataCreate(int nchunks);
extern NuAnimDataChunk *NuAnimDataChunkCreate(int numnodes);
extern NuAnimCurveSet *NuAnimCurveSetCreate(int ncurves);
extern double floor(double x);
extern void NuAnimDataChunkDestroy(NuAnimDataChunk *chunk);
extern void NuMtxSetIdentity(NuMtx *m);
extern void NuMtxSetRotateXYZ(NuMtx *m, NuAngVec *a);
extern void NuMtxPreScale(NuMtx *m, NuVec *s);
extern void NuMtxTranslate(NuMtx *m, NuVec *t);
extern void NuMtxSetRotateXYZVU0(NuMtx *m, NuAngVec *a);
extern void NuMtxMulRVU0(NuMtx *dst, NuMtx *a, NuMtx *b);
extern void NuMtxMulR(NuMtx *dst, NuMtx *a, NuMtx *b);
extern void NuMtxScale(NuMtx *m, NuVec *s);
extern void NuMtxPreTranslate(NuMtx *m, NuVec *t);
extern void NuMtxPreScaleVU0(NuMtx *m, NuVec *s);
extern void NuMtxScaleVU0(NuMtx *m, NuVec *s);

extern unsigned char D_006ADC00[256];
extern int D_0062EE18;
extern char D_00614A60[];
extern char D_00614AD0[];
extern char D_00614B20[];
extern char D_00614B60[];
extern char D_00614BC8[];
extern char D_00614C10[];
extern char D_00614C50[];
extern char D_0062EE20[];
extern char D_00614CB8[];
extern char D_00614CD0[];
extern char D_00614CE8[];

#define bitcount_table D_006ADC00
#define bitcount_table_built D_0062EE18
#define nuanim_srcfile D_00614A60
#define nuanim_load_failed_msg D_00614AD0
#define nuanim_new_format_msg D_00614B20
#define nuanim_old_format_msg D_00614B60
#define nuanim_assert_msg D_0062EE20
#define nuanim_bad_index_msg D_00614CB8
#define nuanim_bad_curveset_msg D_00614CD0
#define nuanim_blend_bad_curveset_msg D_00614CE8
#define nuanim_keytype_none_msg D_00614BC8
#define nuanim_keytype_small_msg D_00614C10
#define nuanim_keytype_boolean_msg D_00614C50

#define NUANIMKEYTYPE_NONE 0
#define NUANIMKEYTYPE_HERMITE 1
#define NUANIMKEYTYPE_SMALL 2
#define NUANIMKEYTYPE_STEP 3
#define NUANIMKEYTYPE_BOOLEAN 4

#define NUANIM_SET_CURVES 9

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


void NuAnimDataWrite(int fh, NuAnimData *data) {
    NuAnimDataChunk *chunk;
    NuAnimCurveSet *curveset;
    NuAnimCurve *curve;
    int size;
    int nkeys;
    int ncurves;
    int i;
    int j;
    int k;

    if (data->node_names) {
        size = strlen(data->node_names) + 1;
        NuFileWriteInt(fh, size);
        NuFileWrite(fh, data->node_names, size);
    } else {
        NuFileWriteInt(fh, 0);
    }
    NuFileWriteFloat(fh, data->time);
    NuFileWriteInt(fh, data->nchunks);
    for (i = 0; i < data->nchunks; i++) {
        chunk = data->chunks[i];
        nkeys = 0;
        ncurves = 0;
        NuFileWriteInt(fh, chunk->numnodes);
        for (j = 0; j < chunk->numnodes; j++) {
            curveset = chunk->animcurvesets[j];
            if (curveset) {
                for (k = 0; k < NUANIM_SET_CURVES; k++) {
                    if (curveset->set[k]) {
                        ncurves++;
                        nkeys += curveset->set[k]->numkeys;
                    }
                }
            }
        }
        NuFileWriteInt(fh, nkeys);
        for (j = 0; j < chunk->numnodes; j++) {
            if (chunk->animcurvesets[j]) {
                for (k = 0; k < NUANIM_SET_CURVES; k++) {
                    curve = chunk->animcurvesets[j]->set[k];
                    if (curve) {
                        NuFileWrite(fh, curve->animkeys, curve->numkeys * sizeof(NuAnimKey));
                    }
                }
            }
        }
        NuFileWriteInt(fh, ncurves);
        for (j = 0; j < chunk->numnodes; j++) {
            for (k = 0; k < NUANIM_SET_CURVES; k++) {
                curveset = chunk->animcurvesets[j];
                if (curveset) {
                    curve = curveset->set[k];
                    if (curve) {
                        NuFileWrite(fh, curve, sizeof(NuAnimCurve));
                    }
                }
            }
        }
        for (j = 0; j < chunk->numnodes; j++) {
            if (chunk->animcurvesets[j]) {
                NuFileWriteChar(fh, 1);
                NuFileWriteInt(fh, chunk->animcurvesets[j]->flags);
                for (k = 0; k < NUANIM_SET_CURVES; k++) {
                    NuFileWriteFloat(fh, chunk->animcurvesets[j]->constants[k]);
                }
            } else {
                NuFileWriteChar(fh, 0);
            }
        }
    }
}


NuAnimData *NuAnimDataRead(int fh) {
    NuAnimData *data;
    NuAnimDataChunk *chunk;
    NuAnimCurve *curve;
    NuAnimKey *key;
    char *names;
    float time;
    int size;
    int numnodes;
    int ncurves;
    int count;
    int i;
    int j;
    int k;

    if (!bitcount_table_built) {
        buildBitCountTable();
    }
    size = NuFileReadInt(fh);
    if (size) {
        names = NuMemAllocFn(size, nuanim_srcfile, 0x18B);
        NuFileRead(fh, names, size);
    } else {
        names = 0;
    }
    time = NuFileReadFloat(fh);
    data = NuAnimDataCreate(NuFileReadInt(fh));
    data->time = time;
    data->node_names = names;
    for (i = 0; i < data->nchunks; i++) {
        numnodes = NuFileReadInt(fh);
        chunk = NuAnimDataChunkCreate(numnodes);
        data->chunks[i] = chunk;
        chunk->numnodes = numnodes;
        count = NuFileReadInt(fh);
        if (count) {
            chunk->keys = NuMemAllocFn(count * sizeof(NuAnimKey), nuanim_srcfile, 0x1A5);
            NuFileRead(fh, chunk->keys, count * sizeof(NuAnimKey));
        } else {
            chunk->keys = 0;
        }
        key = chunk->keys;
        count = NuFileReadInt(fh);
        if (count) {
            chunk->curves = NuMemAllocFn(count * sizeof(NuAnimCurve), nuanim_srcfile, 0x1B2);
            NuFileRead(fh, chunk->curves, count * sizeof(NuAnimCurve));
        } else {
            chunk->curves = 0;
        }
        for (j = 0; j < numnodes; j++) {
            ncurves = NuFileReadChar(fh);
            if (ncurves) {
                chunk->animcurvesets[j] = NuAnimCurveSetCreate(ncurves);
                chunk->animcurvesets[j]->flags = NuFileReadInt(fh);
                for (k = 0; k < chunk->animcurvesets[j]->ncurves; k++) {
                    chunk->animcurvesets[j]->constants[k] = NuFileReadFloat(fh);
                }
            }
        }
        curve = chunk->curves;
        for (j = 0; j < chunk->numnodes; j++) {
            if (chunk->animcurvesets[j]) {
                for (k = 0; k < chunk->animcurvesets[j]->ncurves; k++) {
                    if (chunk->animcurvesets[j]->constants[k] == 3.40282347e+38f) {
                        chunk->animcurvesets[j]->set[k] = curve;
                        chunk->num_valid_animcurvesets++;
                        curve->animkeys = key;
                        key += curve->numkeys;
                        curve++;
                    }
                }
            }
        }
    }
    return data;
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
        nkey = bitcount_table[curve->mask.bytes[0] & atime->time_mask];
        break;
    case 1:
        nkey = bitcount_table[curve->mask.bytes[0]];
        nkey += bitcount_table[curve->mask.bytes[1] & atime->time_mask];
        break;
    case 2:
        nkey = bitcount_table[curve->mask.bytes[0]];
        nkey += bitcount_table[curve->mask.bytes[1]];
        nkey += bitcount_table[curve->mask.bytes[2] & atime->time_mask];
        break;
    case 3:
        nkey = bitcount_table[curve->mask.bytes[0]];
        nkey += bitcount_table[curve->mask.bytes[1]];
        nkey += bitcount_table[curve->mask.bytes[2]];
        nkey += bitcount_table[curve->mask.bytes[3] & atime->time_mask];
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


float NuAnimCurve2CalcVal(NuAnimCurve2 *curve, NuAnimTime *atime, int keytype) {
    NuAnimCurveData2 *curvedata;
    NuAnimMask *mask;
    NuAnimStepKey *stepkeys;
    NuAnimKey2 *key;
    NuAnimKey2 *next;
    int nkey;
    int ix;
    int bit;
    float u;
    float dt;
    float a;
    float b;
    float c;

    if (keytype == NUANIMKEYTYPE_NONE) {
        return curve->constant;
    }
    mask = &curve->curvedata->mask[atime->chunk];
    if (keytype == NUANIMKEYTYPE_BOOLEAN) {
        bit = (int)floor(atime->time_offset) - 1;
        return mask->bits && (1 << bit);
    }
    switch (atime->time_byte) {
    case 0:
        nkey = bitcount_table[mask->bytes[0] & atime->time_mask];
        break;
    case 1:
        nkey = bitcount_table[mask->bytes[0]];
        nkey += bitcount_table[mask->bytes[1] & atime->time_mask];
        break;
    case 2:
        nkey = bitcount_table[mask->bytes[0]];
        nkey += bitcount_table[mask->bytes[1]];
        nkey += bitcount_table[mask->bytes[2] & atime->time_mask];
        break;
    case 3:
        nkey = bitcount_table[mask->bytes[0]];
        nkey += bitcount_table[mask->bytes[1]];
        nkey += bitcount_table[mask->bytes[2]];
        nkey += bitcount_table[mask->bytes[3] & atime->time_mask];
        break;
    }
    curvedata = curve->curvedata;
    ix = curvedata->key_ixs[atime->chunk];
    switch (keytype) {
    case NUANIMKEYTYPE_NONE:
        NuErrorProlog(nuanim_srcfile, 0x4ED)(nuanim_keytype_none_msg);
        break;
    case NUANIMKEYTYPE_HERMITE:
        key = &curvedata->keys[ix + nkey - 1];
        next = key + 1;
        dt = next->time - key->time;
        u = (atime->time - key->time) * key->dtime;
        b = key->slope * dt;
        c = next->slope * dt;
        a = key->value - next->value;
        return u * (u * (u * (a + a + b + c) + a * -3.0f - (b + b) - c) + b) + key->value;
    case NUANIMKEYTYPE_SMALL:
        NuErrorProlog(nuanim_srcfile, 0x50C)(nuanim_keytype_small_msg);
        break;
    case NUANIMKEYTYPE_STEP:
        stepkeys = (NuAnimStepKey *)curvedata->keys;
        return stepkeys[ix + nkey - 1].value;
    case NUANIMKEYTYPE_BOOLEAN:
        NuErrorProlog(nuanim_srcfile, 0x517)(nuanim_keytype_boolean_msg);
        break;
    }
    return 0.0f;
}


void NuAnimCurveSetApplyToJoint(NuAnimDataChunk *chunk, NuAnimTime *atime, int node, NuJointNode *joint) {
    NuVec t;
    NuVec r;
    NuVec p;
    NuAngVec ang;
    NuVec s;
    NuMtx *m;
    NuAnimCurveSet *curveset;
    NuJointNode *parent;

    m = &joint->mtx;
    if (node < 0 || node >= chunk->numnodes) {
        NuErrorProlog(nuanim_srcfile, 0x590)(nuanim_bad_index_msg);
    }
    curveset = chunk->animcurvesets[node];
    if (!curveset) {
        NuErrorProlog(nuanim_srcfile, 0x595)(nuanim_bad_curveset_msg);
    }
    if (curveset->flags & 1) {
        if (curveset->set[3]) {
            r.x = NuAnimCurveCalcVal2(curveset->set[3], atime);
        } else {
            r.x = curveset->constants[3];
        }
        if (curveset->set[4]) {
            r.y = NuAnimCurveCalcVal2(curveset->set[4], atime);
        } else {
            r.y = curveset->constants[4];
        }
        if (curveset->set[5]) {
            r.z = NuAnimCurveCalcVal2(curveset->set[5], atime);
        } else {
            r.z = curveset->constants[5];
        }
        ang.x = (int)(r.x * 10430.378f);
        ang.y = (int)(r.y * 10430.378f);
        ang.z = (int)(r.z * 10430.378f);
        NuMtxSetRotateXYZ(m, &ang);
    } else {
        NuMtxSetIdentity(m);
    }
    if (curveset->flags & 0x20) {
        NuMtxMulR(m, m, &joint->orient);
    }
    if (curveset->flags & 8) {
        if (curveset->set[6]) {
            joint->scale.x = NuAnimCurveCalcVal2(curveset->set[6], atime);
        } else {
            joint->scale.x = curveset->constants[6];
        }
        if (curveset->set[7]) {
            joint->scale.y = NuAnimCurveCalcVal2(curveset->set[7], atime);
        } else {
            joint->scale.y = curveset->constants[7];
        }
        if (curveset->set[8]) {
            joint->scale.z = NuAnimCurveCalcVal2(curveset->set[8], atime);
        } else {
            joint->scale.z = curveset->constants[8];
        }
        NuMtxPreScale(m, &joint->scale);
    }
    if (curveset->flags & 0x10) {
        parent = (NuJointNode *)joint->node.parent;
        s.x = 1.0f / parent->scale.x;
        s.y = 1.0f / parent->scale.y;
        s.z = 1.0f / parent->scale.z;
        NuMtxScale(m, &s);
    }
    if (curveset->set[0]) {
        t.x = NuAnimCurveCalcVal2(curveset->set[0], atime);
    } else {
        t.x = curveset->constants[0];
    }
    if (curveset->set[1]) {
        t.y = NuAnimCurveCalcVal2(curveset->set[1], atime);
    } else {
        t.y = curveset->constants[1];
    }
    if (curveset->set[2]) {
        t.z = NuAnimCurveCalcVal2(curveset->set[2], atime);
    } else {
        t.z = curveset->constants[2];
    }
    NuMtxTranslate(m, &t);
    if (joint->flags & 8) {
        NuMtxPreTranslate(m, &joint->pivot);
        p.x = -joint->pivot.x;
        p.y = -joint->pivot.y;
        p.z = -joint->pivot.z;
        NuMtxTranslate(m, &p);
    }
    m->_02 = -m->_02;
    m->_12 = -m->_12;
    m->_20 = -m->_20;
    m->_21 = -m->_21;
    m->_23 = -m->_23;
    m->_32 = -m->_32;
}


void NuAnimCurveSetApplyBlendToJoint(NuAnimDataChunk *chunk1, NuAnimTime *atime1, NuAnimDataChunk *chunk2,
                                     NuAnimTime *atime2, float blend, int node, NuJointNode *joint) {
    NuVec lo;
    NuAngVec rf;
    float r[3];
    float r1[3];
    float r2[3];
    float s[3];
    float t[3];
    NuVec invscale;
    float omblend;
    int i;
    NuMtx *m;
    NuAnimCurveSet *curveset1;
    NuAnimCurveSet *curveset2;
    NuJointNode *parent;

    m = &joint->mtx;
    omblend = 1.0f - blend;
    curveset1 = chunk1->animcurvesets[0];
    if (!curveset1) {
        NuErrorProlog(nuanim_srcfile, 0x619)(nuanim_blend_bad_curveset_msg);
    }
    curveset2 = chunk2->animcurvesets[node];
    if (!curveset2) {
        NuErrorProlog(nuanim_srcfile, 0x61B)(nuanim_blend_bad_curveset_msg);
    }
    if ((curveset1->flags & 1) || (curveset2->flags & 1)) {
        for (i = 3; i < 6; i++) {
            if (curveset1->set[i]) {
                r1[i - 3] = NuAnimCurveCalcVal2(curveset1->set[i], atime1);
            } else {
                r1[i - 3] = curveset1->constants[i];
            }
            if (curveset2->set[i]) {
                r2[i - 3] = NuAnimCurveCalcVal2(curveset2->set[i], atime2);
            } else {
                r2[i - 3] = curveset2->constants[i];
            }
        }
        r1[0] -= (r1[0] - r2[0] > 3.1415927f) ? 6.2831855f : 0.0f;
        r2[0] -= (r2[0] - r1[0] > 3.1415927f) ? 6.2831855f : 0.0f;
        r1[1] -= (r1[1] - r2[1] > 3.1415927f) ? 6.2831855f : 0.0f;
        r2[1] -= (r2[1] - r1[1] > 3.1415927f) ? 6.2831855f : 0.0f;
        r1[2] -= (r1[2] - r2[2] > 3.1415927f) ? 6.2831855f : 0.0f;
        r2[2] -= (r2[2] - r1[2] > 3.1415927f) ? 6.2831855f : 0.0f;
        r[0] = r1[0] * omblend + r2[0] * blend;
        r[1] = r1[1] * omblend + r2[1] * blend;
        r[2] = r1[2] * omblend + r2[2] * blend;
        rf.x = (int)(r[0] * 10430.378f);
        rf.y = (int)(r[1] * 10430.378f);
        rf.z = (int)(r[2] * 10430.378f);
        NuMtxSetRotateXYZVU0(m, &rf);
    } else {
        NuMtxSetIdentity(m);
    }
    if ((curveset1->flags & 0x20) || (curveset2->flags & 0x20)) {
        NuMtxMulRVU0(m, m, &joint->orient);
    }
    if ((curveset1->flags & 8) || (curveset2->flags & 8)) {
        for (i = 6; i < 9; i++) {
            if (curveset1->set[i]) {
                s[i - 6] = omblend * NuAnimCurveCalcVal2(curveset1->set[i], atime1);
            } else {
                s[i - 6] = omblend * curveset1->constants[i];
            }
            if (curveset2->set[i]) {
                s[i - 6] += blend * NuAnimCurveCalcVal2(curveset2->set[i], atime2);
            } else {
                s[i - 6] += blend * curveset2->constants[i];
            }
        }
        joint->scale.x = s[0];
        joint->scale.y = s[1];
        joint->scale.z = s[2];
        NuMtxPreScale(m, &joint->scale);
    }
    if ((curveset1->flags & 0x10) || (curveset2->flags & 0x10)) {
        parent = (NuJointNode *)joint->node.parent;
        invscale.x = 1.0f / parent->scale.x;
        invscale.y = 1.0f / parent->scale.y;
        invscale.z = 1.0f / parent->scale.z;
        NuMtxScaleVU0(m, &invscale);
    }
    for (i = 0; i < 3; i++) {
        if (curveset1->set[i]) {
            t[i] = omblend * NuAnimCurveCalcVal2(curveset1->set[i], atime1);
        } else {
            t[i] = omblend * curveset1->constants[i];
        }
        if (curveset2->set[i]) {
            t[i] += blend * NuAnimCurveCalcVal2(curveset2->set[i], atime2);
        } else {
            t[i] += blend * curveset2->constants[i];
        }
    }
    NuMtxTranslate(m, (NuVec *)t);
    if (joint->flags & 8) {
        NuMtxPreTranslate(m, &joint->pivot);
        lo.x = -joint->pivot.x;
        lo.y = -joint->pivot.y;
        lo.z = -joint->pivot.z;
        NuMtxTranslate(m, &lo);
    }
    m->_02 = -m->_02;
    m->_12 = -m->_12;
    m->_20 = -m->_20;
    m->_21 = -m->_21;
    m->_23 = -m->_23;
    m->_32 = -m->_32;
}


void NuAnimCurveSetApplyBlendToJoint2(NuAnimCurveSet *curveset1, NuAnimTime *atime1, NuAnimCurveSet *curveset2,
                                      NuAnimTime *atime2, float blend, NuJointData *joint, NuVec *scale,
                                      NuVec *parentscale, NuMtx *m, NuJointAnim *offset) {
    NuVec lo;
    NuAngVec rf;
    float r[3];
    float r1[3];
    float r2[3];
    float s1[3];
    float s2[3];
    float t[3];
    NuVec invscale;
    float omblend;
    int i;
    signed char procflags;

    omblend = 1.0f - blend;
    if (offset) {
        procflags = offset->flags;
    } else {
        procflags = 0;
    }
    if ((curveset1->flags & 1) || (curveset2->flags & 1) || (procflags & 1)) {
        for (i = 3; i < 6; i++) {
            if (curveset1->set[i]) {
                r1[i - 3] = NuAnimCurveCalcVal2(curveset1->set[i], atime1);
            } else {
                r1[i - 3] = curveset1->constants[i];
            }
            if (curveset2->set[i]) {
                r2[i - 3] = NuAnimCurveCalcVal2(curveset2->set[i], atime2);
            } else {
                r2[i - 3] = curveset2->constants[i];
            }
        }
        r1[0] -= (r1[0] - r2[0] > 3.1415927f) ? 6.2831855f : 0.0f;
        r2[0] -= (r2[0] - r1[0] > 3.1415927f) ? 6.2831855f : 0.0f;
        r1[1] -= (r1[1] - r2[1] > 3.1415927f) ? 6.2831855f : 0.0f;
        r2[1] -= (r2[1] - r1[1] > 3.1415927f) ? 6.2831855f : 0.0f;
        r1[2] -= (r1[2] - r2[2] > 3.1415927f) ? 6.2831855f : 0.0f;
        r2[2] -= (r2[2] - r1[2] > 3.1415927f) ? 6.2831855f : 0.0f;
        r[0] = r1[0] * omblend + r2[0] * blend;
        r[1] = r1[1] * omblend + r2[1] * blend;
        r[2] = r1[2] * omblend + r2[2] * blend;
        if (procflags & 1) {
            r[0] += offset->rx;
            r[1] += offset->ry;
            r[2] += offset->rz;
            rf.x = (int)(r[0] * 10430.378f);
            rf.y = (int)(r[1] * 10430.378f);
            rf.z = (int)(r[2] * 10430.378f);
            if (procflags & 8) {
                rf.x &= 0xFFFF;
                if (rf.x > 0x7FFF) {
                    rf.x -= 0x10000;
                }
                if (rf.x > offset->max_rx) {
                    rf.x = offset->max_rx;
                } else if (rf.x < offset->min_rx) {
                    rf.x = offset->min_rx;
                }
            }
            if (procflags & 0x10) {
                rf.y &= 0xFFFF;
                if (rf.y > 0x7FFF) {
                    rf.y -= 0x10000;
                }
                if (rf.y > offset->max_ry) {
                    rf.y = offset->max_ry;
                } else if (rf.y < offset->min_ry) {
                    rf.y = offset->min_ry;
                }
            }
            if (procflags & 0x20) {
                rf.z &= 0xFFFF;
                if (rf.z > 0x7FFF) {
                    rf.z -= 0x10000;
                }
                if (rf.z > offset->max_rz) {
                    rf.z = offset->max_rz;
                } else if (rf.z < offset->min_rz) {
                    rf.z = offset->min_rz;
                }
            }
        } else {
            rf.x = (int)(r[0] * 10430.378f);
            rf.y = (int)(r[1] * 10430.378f);
            rf.z = (int)(r[2] * 10430.378f);
        }
        NuMtxSetRotateXYZVU0(m, &rf);
    } else {
        NuMtxSetIdentity(m);
    }
    if ((curveset1->flags & 0x20) || (curveset2->flags & 0x20)) {
        NuMtxMulRVU0(m, m, &joint->orient);
    }
    if ((curveset1->flags & 8) || (curveset2->flags & 8) || (procflags & 4)) {
        for (i = 6; i < 9; i++) {
            if (curveset1->set[i]) {
                s1[i - 6] = NuAnimCurveCalcVal2(curveset1->set[i], atime1);
            } else {
                s1[i - 6] = curveset1->constants[i];
            }
            if (curveset2->set[i]) {
                s2[i - 6] = NuAnimCurveCalcVal2(curveset2->set[i], atime2);
            } else {
                s2[i - 6] = curveset2->constants[i];
            }
        }
        scale->x = s1[0] * blend + s2[0] * omblend;
        scale->y = s1[1] * blend + s2[1] * omblend;
        scale->z = s1[2] * blend + s2[2] * omblend;
        if (procflags & 4) {
            scale->x += offset->sx;
            scale->y += offset->sy;
            scale->z += offset->sz;
        }
        NuMtxPreScaleVU0(m, scale);
        scale->x *= parentscale->x;
        scale->y *= parentscale->y;
        scale->z *= parentscale->z;
    } else {
        *scale = *parentscale;
    }
    if ((curveset1->flags | curveset2->flags) & 0x10) {
        invscale.x = 1.0f / parentscale->x;
        invscale.y = 1.0f / parentscale->y;
        invscale.z = 1.0f / parentscale->z;
        NuMtxScaleVU0(m, &invscale);
        scale->x *= invscale.x;
        scale->y *= invscale.y;
        scale->z *= invscale.z;
    }
    for (i = 0; i < 3; i++) {
        if (curveset1->set[i]) {
            t[i] = omblend * NuAnimCurveCalcVal2(curveset1->set[i], atime1);
        } else {
            t[i] = omblend * curveset1->constants[i];
        }
        if (curveset2->set[i]) {
            t[i] += blend * NuAnimCurveCalcVal2(curveset2->set[i], atime2);
        } else {
            t[i] += blend * curveset2->constants[i];
        }
    }
    if (procflags & 2) {
        t[0] += offset->tx;
        t[1] += offset->ty;
        t[2] += offset->tz;
    }
    NuMtxTranslate(m, (NuVec *)t);
    if (joint->flags & 8) {
        NuMtxPreTranslate(m, &joint->locator_offset);
        lo.x = -joint->locator_offset.x;
        lo.y = -joint->locator_offset.y;
        lo.z = -joint->locator_offset.z;
        NuMtxTranslate(m, &lo);
    }
    m->_02 = -m->_02;
    m->_12 = -m->_12;
    m->_20 = -m->_20;
    m->_21 = -m->_21;
    m->_23 = -m->_23;
    m->_32 = -m->_32;
}


void NuAnimCurve2SetApplyBlendToJoint2(NuAnimCurve2 *curves1, signed char *curveflags1, signed char curvesetflags1,
                                       NuAnimTime *atime1, NuAnimCurve2 *curves2, signed char *curveflags2,
                                       signed char curvesetflags2, NuAnimTime *atime2, float blend, NuJointData *joint,
                                       NuVec *scale, NuVec *parentscale, NuMtx *m, NuJointAnim *offset) {
    NuVec lo;
    NuAngVec rf;
    float r[3];
    float r1[3];
    float r2[3];
    float s1[3];
    float s2[3];
    float t[3];
    NuVec invscale;
    float omblend;
    int i;
    signed char procflags;

    omblend = 1.0f - blend;
    if (offset) {
        procflags = offset->flags;
    } else {
        procflags = 0;
    }
    if ((curvesetflags1 & 1) || (curvesetflags2 & 1) || (procflags & 1)) {
        for (i = 3; i < 6; i++) {
            r1[i - 3] = NuAnimCurve2CalcVal(&curves1[i], atime1, curveflags1[i]);
            r2[i - 3] = NuAnimCurve2CalcVal(&curves2[i], atime2, curveflags2[i]);
        }
        r1[0] -= (r1[0] - r2[0] > 3.1415927f) ? 6.2831855f : 0.0f;
        r2[0] -= (r2[0] - r1[0] > 3.1415927f) ? 6.2831855f : 0.0f;
        r1[1] -= (r1[1] - r2[1] > 3.1415927f) ? 6.2831855f : 0.0f;
        r2[1] -= (r2[1] - r1[1] > 3.1415927f) ? 6.2831855f : 0.0f;
        r1[2] -= (r1[2] - r2[2] > 3.1415927f) ? 6.2831855f : 0.0f;
        r2[2] -= (r2[2] - r1[2] > 3.1415927f) ? 6.2831855f : 0.0f;
        r[0] = r1[0] * omblend + r2[0] * blend;
        r[1] = r1[1] * omblend + r2[1] * blend;
        r[2] = r1[2] * omblend + r2[2] * blend;
        if (procflags & 1) {
            r[0] += offset->rx;
            r[1] += offset->ry;
            r[2] += offset->rz;
            rf.x = (int)(r[0] * 10430.378f);
            rf.y = (int)(r[1] * 10430.378f);
            rf.z = (int)(r[2] * 10430.378f);
            if (procflags & 8) {
                rf.x &= 0xFFFF;
                if (rf.x > 0x7FFF) {
                    rf.x -= 0x10000;
                }
                if (rf.x > offset->max_rx) {
                    rf.x = offset->max_rx;
                } else if (rf.x < offset->min_rx) {
                    rf.x = offset->min_rx;
                }
            }
            if (procflags & 0x10) {
                rf.y &= 0xFFFF;
                if (rf.y > 0x7FFF) {
                    rf.y -= 0x10000;
                }
                if (rf.y > offset->max_ry) {
                    rf.y = offset->max_ry;
                } else if (rf.y < offset->min_ry) {
                    rf.y = offset->min_ry;
                }
            }
            if (procflags & 0x20) {
                rf.z &= 0xFFFF;
                if (rf.z > 0x7FFF) {
                    rf.z -= 0x10000;
                }
                if (rf.z > offset->max_rz) {
                    rf.z = offset->max_rz;
                } else if (rf.z < offset->min_rz) {
                    rf.z = offset->min_rz;
                }
            }
        } else {
            rf.x = (int)(r[0] * 10430.378f);
            rf.y = (int)(r[1] * 10430.378f);
            rf.z = (int)(r[2] * 10430.378f);
        }
        NuMtxSetRotateXYZVU0(m, &rf);
    } else {
        NuMtxSetIdentity(m);
    }
    if ((curvesetflags1 & 0x20) || (curvesetflags2 & 0x20)) {
        NuMtxMulRVU0(m, m, &joint->orient);
    }
    if ((curvesetflags1 & 8) || (curvesetflags2 & 8) || (procflags & 4)) {
        for (i = 6; i < 9; i++) {
            s1[i - 6] = NuAnimCurve2CalcVal(&curves1[i], atime1, curveflags1[i]);
            s2[i - 6] = NuAnimCurve2CalcVal(&curves2[i], atime2, curveflags2[i]);
        }
        scale->x = s1[0] * blend + s2[0] * omblend;
        scale->y = s1[1] * blend + s2[1] * omblend;
        scale->z = s1[2] * blend + s2[2] * omblend;
        if (procflags & 4) {
            scale->x += offset->sx;
            scale->y += offset->sy;
            scale->z += offset->sz;
        }
        NuMtxPreScaleVU0(m, scale);
        scale->x *= parentscale->x;
        scale->y *= parentscale->y;
        scale->z *= parentscale->z;
    } else {
        *scale = *parentscale;
    }
    if ((curvesetflags1 | curvesetflags2) & 0x10) {
        invscale.x = 1.0f / parentscale->x;
        invscale.y = 1.0f / parentscale->y;
        invscale.z = 1.0f / parentscale->z;
        NuMtxScaleVU0(m, &invscale);
        scale->x *= invscale.x;
        scale->y *= invscale.y;
        scale->z *= invscale.z;
    }
    for (i = 0; i < 3; i++) {
        t[i] = omblend * NuAnimCurve2CalcVal(&curves1[i], atime1, curveflags1[i]);
        t[i] += blend * NuAnimCurve2CalcVal(&curves2[i], atime2, curveflags2[i]);
    }
    if (procflags & 2) {
        t[0] += offset->tx;
        t[1] += offset->ty;
        t[2] += offset->tz;
    }
    NuMtxTranslate(m, (NuVec *)t);
    if (joint->flags & 8) {
        NuMtxPreTranslate(m, &joint->locator_offset);
        lo.x = -joint->locator_offset.x;
        lo.y = -joint->locator_offset.y;
        lo.z = -joint->locator_offset.z;
        NuMtxTranslate(m, &lo);
    }
    m->_02 = -m->_02;
    m->_12 = -m->_12;
    m->_20 = -m->_20;
    m->_21 = -m->_21;
    m->_23 = -m->_23;
    m->_32 = -m->_32;
}


void NuAnimCurveSetApplyToJoint2(NuAnimCurveSet *curveset, NuAnimTime *atime, NuJointData *joint, NuVec *scale,
                                 NuVec *parentscale, NuMtx *m, NuJointAnim *offset) {
    signed char procflags;
    NuVec t;
    NuVec r;
    NuVec lo;
    NuAngVec rf;
    NuVec invscale;

    if (offset) {
        procflags = offset->flags;
    } else {
        procflags = 0;
    }
    if ((curveset->flags & 1) || (procflags & 1)) {
        if (curveset->flags & 1) {
            if (curveset->set[3]) {
                r.x = NuAnimCurveCalcVal2(curveset->set[3], atime);
            } else {
                r.x = curveset->constants[3];
            }
            if (curveset->set[4]) {
                r.y = NuAnimCurveCalcVal2(curveset->set[4], atime);
            } else {
                r.y = curveset->constants[4];
            }
            if (curveset->set[5]) {
                r.z = NuAnimCurveCalcVal2(curveset->set[5], atime);
            } else {
                r.z = curveset->constants[5];
            }
        } else {
            r.x = r.y = r.z = 0.0f;
        }
        if (procflags & 1) {
            r.x += offset->rx;
            r.y += offset->ry;
            r.z += offset->rz;
            rf.x = (int)(r.x * 10430.378f);
            rf.y = (int)(r.y * 10430.378f);
            rf.z = (int)(r.z * 10430.378f);
            if (procflags & 8) {
                rf.x &= 0xFFFF;
                if (rf.x > 0x7FFF) {
                    rf.x -= 0x10000;
                }
                if (rf.x > offset->max_rx) {
                    rf.x = offset->max_rx;
                } else if (rf.x < offset->min_rx) {
                    rf.x = offset->min_rx;
                }
            }
            if (procflags & 0x10) {
                rf.y &= 0xFFFF;
                if (rf.y > 0x7FFF) {
                    rf.y -= 0x10000;
                }
                if (rf.y > offset->max_ry) {
                    rf.y = offset->max_ry;
                } else if (rf.y < offset->min_ry) {
                    rf.y = offset->min_ry;
                }
            }
            if (procflags & 0x20) {
                rf.z &= 0xFFFF;
                if (rf.z > 0x7FFF) {
                    rf.z -= 0x10000;
                }
                if (rf.z > offset->max_rz) {
                    rf.z = offset->max_rz;
                } else if (rf.z < offset->min_rz) {
                    rf.z = offset->min_rz;
                }
            }
        } else {
            rf.x = (int)(r.x * 10430.378f);
            rf.y = (int)(r.y * 10430.378f);
            rf.z = (int)(r.z * 10430.378f);
        }
        NuMtxSetRotateXYZVU0(m, &rf);
    } else {
        NuMtxSetIdentity(m);
    }
    if (curveset->flags & 0x20) {
        NuMtxMulRVU0(m, m, &joint->orient);
    }
    if ((curveset->flags & 8) || (procflags & 4)) {
        if (curveset->flags & 8) {
            if (curveset->set[6]) {
                scale->x = NuAnimCurveCalcVal2(curveset->set[6], atime);
            } else {
                scale->x = curveset->constants[6];
            }
            if (curveset->set[7]) {
                scale->y = NuAnimCurveCalcVal2(curveset->set[7], atime);
            } else {
                scale->y = curveset->constants[7];
            }
            if (curveset->set[8]) {
                scale->z = NuAnimCurveCalcVal2(curveset->set[8], atime);
            } else {
                scale->z = curveset->constants[8];
            }
        } else {
            scale->x = scale->y = scale->z = 0.0f;
        }
        if (procflags & 4) {
            scale->x += offset->sx;
            scale->y += offset->sy;
            scale->z += offset->sz;
        }
        NuMtxPreScaleVU0(m, scale);
        scale->x *= parentscale->x;
        scale->y *= parentscale->y;
        scale->z *= parentscale->z;
    } else {
        *scale = *parentscale;
    }
    if ((curveset->flags & 0x10) && parentscale) {
        invscale.x = 1.0f / parentscale->x;
        invscale.y = 1.0f / parentscale->y;
        invscale.z = 1.0f / parentscale->z;
        NuMtxScaleVU0(m, &invscale);
        scale->x *= invscale.x;
        scale->y *= invscale.y;
        scale->z *= invscale.z;
    }
    if (curveset->set[0]) {
        t.x = NuAnimCurveCalcVal2(curveset->set[0], atime);
    } else {
        t.x = curveset->constants[0];
    }
    if (curveset->set[1]) {
        t.y = NuAnimCurveCalcVal2(curveset->set[1], atime);
    } else {
        t.y = curveset->constants[1];
    }
    if (curveset->set[2]) {
        t.z = NuAnimCurveCalcVal2(curveset->set[2], atime);
    } else {
        t.z = curveset->constants[2];
    }
    if (procflags & 2) {
        t.x += offset->tx;
        t.y += offset->ty;
        t.z += offset->tz;
    }
    NuMtxTranslate(m, &t);
    if (joint->flags & 8) {
        NuMtxPreTranslate(m, &joint->locator_offset);
        lo.x = -joint->locator_offset.x;
        lo.y = -joint->locator_offset.y;
        lo.z = -joint->locator_offset.z;
        NuMtxTranslate(m, &lo);
    }
    m->_02 = -m->_02;
    m->_12 = -m->_12;
    m->_20 = -m->_20;
    m->_21 = -m->_21;
    m->_23 = -m->_23;
    m->_32 = -m->_32;
}


void NuAnimCurve2SetApplyToJoint(NuAnimCurve2 *curves, signed char *curveflags, signed char curvesetflags,
                                 NuAnimTime *atime, NuJointData *joint, NuVec *scale, NuVec *parentscale,
                                 NuMtx *m, NuJointAnim *offset) {
    signed char procflags;
    NuVec t;
    NuVec r;
    NuVec lo;
    NuAngVec rf;
    NuVec invscale;

    if (offset) {
        procflags = offset->flags;
    } else {
        procflags = 0;
    }
    if ((curvesetflags & 1) || (procflags & 1)) {
        if (curvesetflags & 1) {
            r.x = NuAnimCurve2CalcVal(&curves[3], atime, curveflags[3]);
            r.y = NuAnimCurve2CalcVal(&curves[4], atime, curveflags[4]);
            r.z = NuAnimCurve2CalcVal(&curves[5], atime, curveflags[5]);
        } else {
            r.x = r.y = r.z = 0.0f;
        }
        if (procflags & 1) {
            r.x += offset->rx;
            r.y += offset->ry;
            r.z += offset->rz;
            rf.x = (int)(r.x * 10430.378f);
            rf.y = (int)(r.y * 10430.378f);
            rf.z = (int)(r.z * 10430.378f);
            if (procflags & 8) {
                rf.x &= 0xFFFF;
                if (rf.x > 0x7FFF) {
                    rf.x -= 0x10000;
                }
                if (rf.x > offset->max_rx) {
                    rf.x = offset->max_rx;
                } else if (rf.x < offset->min_rx) {
                    rf.x = offset->min_rx;
                }
            }
            if (procflags & 0x10) {
                rf.y &= 0xFFFF;
                if (rf.y > 0x7FFF) {
                    rf.y -= 0x10000;
                }
                if (rf.y > offset->max_ry) {
                    rf.y = offset->max_ry;
                } else if (rf.y < offset->min_ry) {
                    rf.y = offset->min_ry;
                }
            }
            if (procflags & 0x20) {
                rf.z &= 0xFFFF;
                if (rf.z > 0x7FFF) {
                    rf.z -= 0x10000;
                }
                if (rf.z > offset->max_rz) {
                    rf.z = offset->max_rz;
                } else if (rf.z < offset->min_rz) {
                    rf.z = offset->min_rz;
                }
            }
        } else {
            rf.x = (int)(r.x * 10430.378f);
            rf.y = (int)(r.y * 10430.378f);
            rf.z = (int)(r.z * 10430.378f);
        }
        NuMtxSetRotateXYZVU0(m, &rf);
    } else {
        NuMtxSetIdentity(m);
    }
    if (curvesetflags & 0x20) {
        NuMtxMulRVU0(m, m, &joint->orient);
    }
    if ((curvesetflags & 8) || (procflags & 4)) {
        if (curvesetflags & 8) {
            scale->x = NuAnimCurve2CalcVal(&curves[6], atime, curveflags[6]);
            scale->y = NuAnimCurve2CalcVal(&curves[7], atime, curveflags[7]);
            scale->z = NuAnimCurve2CalcVal(&curves[8], atime, curveflags[8]);
        } else {
            scale->x = scale->y = scale->z = 0.0f;
        }
        if (procflags & 4) {
            scale->x += offset->sx;
            scale->y += offset->sy;
            scale->z += offset->sz;
        }
        NuMtxPreScaleVU0(m, scale);
        scale->x *= parentscale->x;
        scale->y *= parentscale->y;
        scale->z *= parentscale->z;
    } else {
        *scale = *parentscale;
    }
    if ((curvesetflags & 0x10) && parentscale) {
        invscale.x = 1.0f / parentscale->x;
        invscale.y = 1.0f / parentscale->y;
        invscale.z = 1.0f / parentscale->z;
        NuMtxScaleVU0(m, &invscale);
        scale->x *= invscale.x;
        scale->y *= invscale.y;
        scale->z *= invscale.z;
    }
    t.x = NuAnimCurve2CalcVal(&curves[0], atime, curveflags[0]);
    t.y = NuAnimCurve2CalcVal(&curves[1], atime, curveflags[1]);
    t.z = NuAnimCurve2CalcVal(&curves[2], atime, curveflags[2]);
    if (procflags & 2) {
        t.x += offset->tx;
        t.y += offset->ty;
        t.z += offset->tz;
    }
    NuMtxTranslate(m, &t);
    if (joint->flags & 8) {
        NuMtxPreTranslate(m, &joint->locator_offset);
        lo.x = -joint->locator_offset.x;
        lo.y = -joint->locator_offset.y;
        lo.z = -joint->locator_offset.z;
        NuMtxTranslate(m, &lo);
    }
    m->_02 = -m->_02;
    m->_12 = -m->_12;
    m->_20 = -m->_20;
    m->_21 = -m->_21;
    m->_23 = -m->_23;
    m->_32 = -m->_32;
}


void NuAnimCurveSetApplyToJointBasic(NuAnimCurveSet *curveset, NuAnimTime *atime, NuJointData *joint,
                                     NuVec *scale, NuVec *parentscale, NuMtx *m, NuJointAnim *offset) {
    signed char procflags;
    NuVec r;
    NuAngVec rf;

    if (offset) {
        procflags = offset->flags;
    } else {
        procflags = 0;
    }
    if (curveset->set[3]) {
        r.x = NuAnimCurveCalcVal2(curveset->set[3], atime);
    } else {
        r.x = curveset->constants[3];
    }
    if (curveset->set[4]) {
        r.y = NuAnimCurveCalcVal2(curveset->set[4], atime);
    } else {
        r.y = curveset->constants[4];
    }
    if (curveset->set[5]) {
        r.z = NuAnimCurveCalcVal2(curveset->set[5], atime);
    } else {
        r.z = curveset->constants[5];
    }
    if (procflags & 1) {
        r.x += offset->rx;
        r.y += offset->ry;
        r.z += offset->rz;
        rf.x = (int)(r.x * 10430.378f);
        rf.y = (int)(r.y * 10430.378f);
        rf.z = (int)(r.z * 10430.378f);
        if (procflags & 8) {
            rf.x &= 0xFFFF;
            if (rf.x > 0x7FFF) {
                rf.x -= 0x10000;
            }
            if (rf.x > offset->max_rx) {
                rf.x = offset->max_rx;
            } else if (rf.x < offset->min_rx) {
                rf.x = offset->min_rx;
            }
        }
        if (procflags & 0x10) {
            rf.y &= 0xFFFF;
            if (rf.y > 0x7FFF) {
                rf.y -= 0x10000;
            }
            if (rf.y > offset->max_ry) {
                rf.y = offset->max_ry;
            } else if (rf.y < offset->min_ry) {
                rf.y = offset->min_ry;
            }
        }
        if (procflags & 0x20) {
            rf.z &= 0xFFFF;
            if (rf.z > 0x7FFF) {
                rf.z -= 0x10000;
            }
            if (rf.z > offset->max_rz) {
                rf.z = offset->max_rz;
            } else if (rf.z < offset->min_rz) {
                rf.z = offset->min_rz;
            }
        }
    } else {
        rf.x = (int)(r.x * 10430.378f);
        rf.y = (int)(r.y * 10430.378f);
        rf.z = (int)(r.z * 10430.378f);
    }
    NuMtxSetRotateXYZVU0(m, &rf);
    if (curveset->flags & 0x20) {
        NuMtxMulRVU0(m, m, &joint->orient);
    }
    m->_30 = curveset->constants[0];
    m->_31 = curveset->constants[1];
    m->_32 = curveset->constants[2];
    if (procflags & 2) {
        m->_30 += offset->tx;
        m->_31 += offset->ty;
        m->_32 += offset->tz;
    }
    *scale = *parentscale;
    m->_02 = -m->_02;
    m->_12 = -m->_12;
    m->_20 = -m->_20;
    m->_21 = -m->_21;
    m->_23 = -m->_23;
    m->_32 = -m->_32;
}


void NuAnimCurve2SetApplyToJointBasic(NuAnimCurve2 *curves, signed char *curveflags, signed char curvesetflags,
                                      NuAnimTime *atime, NuJointData *joint, NuVec *scale, NuVec *parentscale,
                                      NuMtx *m, NuJointAnim *offset) {
    signed char procflags;
    NuVec r;
    NuAngVec rf;

    if (offset) {
        procflags = offset->flags;
    } else {
        procflags = 0;
    }
    r.x = NuAnimCurve2CalcVal(&curves[3], atime, curveflags[3]);
    r.y = NuAnimCurve2CalcVal(&curves[4], atime, curveflags[4]);
    r.z = NuAnimCurve2CalcVal(&curves[5], atime, curveflags[5]);
    if (procflags & 1) {
        r.x += offset->rx;
        r.y += offset->ry;
        r.z += offset->rz;
        rf.x = (int)(r.x * 10430.378f);
        rf.y = (int)(r.y * 10430.378f);
        rf.z = (int)(r.z * 10430.378f);
        if (procflags & 8) {
            rf.x &= 0xFFFF;
            if (rf.x > 0x7FFF) {
                rf.x -= 0x10000;
            }
            if (rf.x > offset->max_rx) {
                rf.x = offset->max_rx;
            } else if (rf.x < offset->min_rx) {
                rf.x = offset->min_rx;
            }
        }
        if (procflags & 0x10) {
            rf.y &= 0xFFFF;
            if (rf.y > 0x7FFF) {
                rf.y -= 0x10000;
            }
            if (rf.y > offset->max_ry) {
                rf.y = offset->max_ry;
            } else if (rf.y < offset->min_ry) {
                rf.y = offset->min_ry;
            }
        }
        if (procflags & 0x20) {
            rf.z &= 0xFFFF;
            if (rf.z > 0x7FFF) {
                rf.z -= 0x10000;
            }
            if (rf.z > offset->max_rz) {
                rf.z = offset->max_rz;
            } else if (rf.z < offset->min_rz) {
                rf.z = offset->min_rz;
            }
        }
    } else {
        rf.x = (int)(r.x * 10430.378f);
        rf.y = (int)(r.y * 10430.378f);
        rf.z = (int)(r.z * 10430.378f);
    }
    NuMtxSetRotateXYZVU0(m, &rf);
    if (curvesetflags & 0x20) {
        NuMtxMulRVU0(m, m, &joint->orient);
    }
    m->_30 = curves[0].constant;
    m->_31 = curves[1].constant;
    m->_32 = curves[2].constant;
    if (procflags & 2) {
        m->_30 += offset->tx;
        m->_31 += offset->ty;
        m->_32 += offset->tz;
    }
    *scale = *parentscale;
    m->_02 = -m->_02;
    m->_12 = -m->_12;
    m->_20 = -m->_20;
    m->_21 = -m->_21;
    m->_23 = -m->_23;
    m->_32 = -m->_32;
}


void NuAnimCurveSetApplyToMatrix(NuAnimCurveSet *curveset, NuAnimTime *atime, NuMtx *m) {
    NuVec t;
    NuVec r;
    NuAngVec ang;
    NuVec s;

    if (curveset->flags & 1) {
        if (curveset->set[3]) {
            r.x = NuAnimCurveCalcVal2(curveset->set[3], atime);
        } else {
            r.x = curveset->constants[3];
        }
        if (curveset->set[4]) {
            r.y = NuAnimCurveCalcVal2(curveset->set[4], atime);
        } else {
            r.y = curveset->constants[4];
        }
        if (curveset->set[5]) {
            r.z = NuAnimCurveCalcVal2(curveset->set[5], atime);
        } else {
            r.z = curveset->constants[5];
        }
        ang.x = (int)(r.x * 10430.378f);
        ang.y = (int)(r.y * 10430.378f);
        ang.z = (int)(r.z * 10430.378f);
        NuMtxSetRotateXYZ(m, &ang);
    } else {
        NuMtxSetIdentity(m);
    }
    if (curveset->flags & 8) {
        if (curveset->set[6]) {
            s.x = NuAnimCurveCalcVal2(curveset->set[6], atime);
        } else {
            s.x = curveset->constants[6];
        }
        if (curveset->set[7]) {
            s.y = NuAnimCurveCalcVal2(curveset->set[7], atime);
        } else {
            s.y = curveset->constants[7];
        }
        if (curveset->set[8]) {
            s.z = NuAnimCurveCalcVal2(curveset->set[8], atime);
        } else {
            s.z = curveset->constants[8];
        }
        NuMtxPreScale(m, &s);
    }
    if (curveset->set[0]) {
        t.x = NuAnimCurveCalcVal2(curveset->set[0], atime);
    } else {
        t.x = curveset->constants[0];
    }
    if (curveset->set[1]) {
        t.y = NuAnimCurveCalcVal2(curveset->set[1], atime);
    } else {
        t.y = curveset->constants[1];
    }
    if (curveset->set[2]) {
        t.z = NuAnimCurveCalcVal2(curveset->set[2], atime);
    } else {
        t.z = curveset->constants[2];
    }
    NuMtxTranslate(m, &t);
    m->_02 = -m->_02;
    m->_12 = -m->_12;
    m->_20 = -m->_20;
    m->_21 = -m->_21;
    m->_23 = -m->_23;
    m->_32 = -m->_32;
}


void NuAnimCurve2SetApplyToMatrix(NuAnimCurve2 *curves, signed char *curveflags, signed char curvesetflags,
                                  NuAnimTime *atime, NuMtx *m) {
    NuVec t;
    NuVec r;
    NuAngVec ang;
    NuVec s;

    if (curvesetflags & 1) {
        r.x = NuAnimCurve2CalcVal(&curves[3], atime, curveflags[3]);
        r.y = NuAnimCurve2CalcVal(&curves[4], atime, curveflags[4]);
        r.z = NuAnimCurve2CalcVal(&curves[5], atime, curveflags[5]);
        ang.x = (int)(r.x * 10430.378f);
        ang.y = (int)(r.y * 10430.378f);
        ang.z = (int)(r.z * 10430.378f);
        NuMtxSetRotateXYZ(m, &ang);
    } else {
        NuMtxSetIdentity(m);
    }
    if (curvesetflags & 8) {
        s.x = NuAnimCurve2CalcVal(&curves[6], atime, curveflags[6]);
        s.y = NuAnimCurve2CalcVal(&curves[7], atime, curveflags[7]);
        s.z = NuAnimCurve2CalcVal(&curves[8], atime, curveflags[8]);
        NuMtxPreScale(m, &s);
    }
    t.x = NuAnimCurve2CalcVal(&curves[0], atime, curveflags[0]);
    t.y = NuAnimCurve2CalcVal(&curves[1], atime, curveflags[1]);
    t.z = NuAnimCurve2CalcVal(&curves[2], atime, curveflags[2]);
    NuMtxTranslate(m, &t);
    m->_02 = -m->_02;
    m->_12 = -m->_12;
    m->_20 = -m->_20;
    m->_21 = -m->_21;
    m->_23 = -m->_23;
    m->_32 = -m->_32;
}


extern NuErrorFunc NuDebugMsgProlog(const char *file, int line);
extern void *NuStreamGetOutputBuffer(int stream);
extern int NuMemFileOpen(void *buf, int size, int mode);
extern char D_00614A80[];
extern char D_00614AA0[];
#define nuanim_load_deprecated_msg D_00614A80
#define nuanim_memfile_failed_msg D_00614AA0

NuAnimData *NuAnimDataLoad(char *file) {
    NuAnimData *data;
    void *buf;
    int size;
    int fh;

    NuErrorProlog(nuanim_srcfile, 0x2D4)(nuanim_load_deprecated_msg);
    data = 0;
    if (!bitcount_table_built) {
        buildBitCountTable();
    }
    buf = NuStreamGetOutputBuffer(0);
    size = NuFileLoadBuffer(file, buf, 0x7FFFFFFF);
    if (size > 0) {
        fh = NuMemFileOpen(buf, size, 0);
        if (fh) {
            data = NuAnimDataRead(fh);
            NuFileClose(fh);
        } else {
            NuDebugMsgProlog(nuanim_srcfile, 0x2EA)(nuanim_memfile_failed_msg, file);
        }
    }
    return data;
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


void NuAnimDataCalcTime(NuAnimData *data, float time, NuAnimTime *atime) {
    int key;

    if (time >= data->time) {
        if (data->time == 1.0f) {
            atime->time = 1.0f;
            atime->time_mask = 1;
            atime->time_byte = 0;
            atime->chunk = 0;
            return;
        }
        atime->time = data->time - 0.01f;
    } else if (time < 1.0f) {
        atime->time = 1.0f;
    } else {
        atime->time = time;
    }
    atime->chunk = ((int)atime->time - 1) >> 5;
    if (atime->chunk >= data->nchunks) {
        atime->chunk = data->nchunks - 1;
    }
    atime->time_offset = atime->time - (float)(atime->chunk << 5);
    key = (int)floor(atime->time_offset) - 1;
    atime->time_byte = (unsigned char)(key / 8);
    atime->time_mask = (unsigned char)((1 << (key % 8 + 1)) - 1);
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


inline void NuAnimCurveDestroy(NuAnimCurve *curve) {
    if (curve->animkeys) {
        NuMemFreeFn(curve->animkeys, nuanim_srcfile, 0x479);
    }
    NuMemFreeFn(curve, nuanim_srcfile, 0x47A);
}


inline void NuAnimKeyWrite(int fh, NuAnimKey *key) {
    if (key) {
        NuFileWriteFloat(fh, key->time);
        NuFileWriteFloat(fh, key->dtime);
        NuFileWriteFloat(fh, key->c);
        NuFileWriteFloat(fh, key->d);
    }
}


inline int NuAnimKeyRead(int fh, NuAnimKey *key) {
    key->time = NuFileReadFloat(fh);
    key->dtime = NuFileReadFloat(fh);
    key->c = NuFileReadFloat(fh);
    key->d = NuFileReadFloat(fh);
    return 1;
}


NuAnimCurveSet *NuAnimCurveSetCreate(int ncurves) {
    NuAnimCurveSet *curveset;
    int size;

    curveset = 0;
    if (ncurves) {
        size = ncurves * sizeof(NuAnimCurve *);
        curveset = NuMemAllocFn(sizeof(NuAnimCurveSet), nuanim_srcfile, 0x556);
        memset(curveset, 0, sizeof(NuAnimCurveSet));
        curveset->ncurves = ncurves;
        curveset->set = NuMemAllocFn(size, nuanim_srcfile, 0x55D);
        memset(curveset->set, 0, size);
        curveset->constants = NuMemAllocFn(size, nuanim_srcfile, 0x562);
        memset(curveset->constants, 0, size);
    }
    return curveset;
}


void NuAnimCurveSetDestroy(NuAnimCurveSet *curveset, int destroy_curves) {
    NuAnimCurve *curve;
    int i;

    if (curveset) {
        if (curveset->set) {
            if (destroy_curves) {
                for (i = 0; i < curveset->ncurves; i++) {
                    curve = curveset->set[i];
                    if (curve) {
                        NuAnimCurveDestroy(curve);
                    }
                }
            }
            NuMemFreeFn(curveset->set, nuanim_srcfile, 0x579);
        }
        if (curveset->constants) {
            NuMemFreeFn(curveset->constants, nuanim_srcfile, 0x57D);
        }
        NuMemFreeFn(curveset, nuanim_srcfile, 0x57F);
    }
}


NuAnimCurve *NuAnimCurveCreate(int numkeys) {
    NuAnimCurve *curve;
    int size;

    if (numkeys <= 0) {
        NuErrorProlog(nuanim_srcfile, 0x469)(nuanim_assert_msg);
    }
    curve = NuMemAllocFn(sizeof(NuAnimCurve), nuanim_srcfile, 0x46B);
    size = numkeys * sizeof(NuAnimKey);
    memset(curve, 0, sizeof(NuAnimCurve));
    curve->numkeys = numkeys;
    curve->animkeys = NuMemAllocFn(size, nuanim_srcfile, 0x46F);
    memset(curve->animkeys, 0, size);
    return curve;
}


inline void NuAnimCurveWrite(int fh, NuAnimCurve *curve) {
    unsigned int i;

    if (curve) {
        NuFileWriteInt(fh, 1);
        NuFileWriteInt(fh, curve->numkeys);
        NuFileWriteInt(fh, curve->mask.bits);
        for (i = 0; i < curve->numkeys; i++) {
            NuAnimKeyWrite(fh, &curve->animkeys[i]);
        }
        NuFileWriteInt(fh, curve->flags);
    } else {
        NuFileWriteInt(fh, 0);
    }
}


void NuAnimCurveSetWrite(int fh, NuAnimCurveSet *curveset) {
    int i;

    if (curveset) {
        NuFileWriteInt(fh, 1);
        NuFileWriteInt(fh, curveset->flags);
        for (i = 0; i < NUANIM_SET_CURVES; i++) {
            NuFileWriteFloat(fh, curveset->constants[i]);
        }
        for (i = 0; i < NUANIM_SET_CURVES; i++) {
            NuAnimCurveWrite(fh, curveset->set[i]);
        }
    } else {
        NuFileWriteInt(fh, 0);
    }
}


NuAnimCurve *NuAnimCurveRead(int fh) {
    NuAnimCurve *curve;
    unsigned int i;

    curve = 0;
    if (NuFileReadInt(fh)) {
        curve = NuAnimCurveCreate(NuFileReadInt(fh));
        curve->mask.bits = NuFileReadInt(fh);
        for (i = 0; i < curve->numkeys; i++) {
            NuAnimKeyRead(fh, &curve->animkeys[i]);
        }
        curve->flags = NuFileReadInt(fh);
    }
    return curve;
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


NuAnimData2 *NuAnimData2LoadBuff(char *file, void **buffer, void **buffend) {
    NuAnimData2Hdr *hdr;
    int size;

    *buffer = (void *)(((unsigned int)*buffer + 0xF) & 0xFFFFFFF0);
    size = NuFileLoadBuffer(file, *buffer, (char *)*buffend - (char *)*buffer);
    if (!size) {
        NuErrorProlog(nuanim_srcfile, 0x348)(nuanim_load_failed_msg, file);
    }
    hdr = *buffer;
    *buffer = (char *)hdr + size;
    if (hdr->version != 2) {
        NuErrorProlog(nuanim_srcfile, 0x34E)(nuanim_old_format_msg, file);
    }
    hdr->address_offset = (int)hdr - hdr->address_offset;
    hdr->data = NuAnimData2FixPtrs(hdr->data, hdr->address_offset);
    return hdr->data;
}


void NuAnimData2CalcTime(NuAnimData2 *data, float time, NuAnimTime *atime) {
    int key;

    if (time >= data->time) {
        if (data->time == 1.0f) {
            atime->time = 1.0f;
            atime->time_mask = 1;
            atime->time_byte = 0;
            atime->chunk = 0;
            return;
        }
        atime->time = data->time - 0.01f;
    } else if (time < 1.0f) {
        atime->time = 1.0f;
    } else {
        atime->time = time;
    }
    atime->chunk = ((int)atime->time - 1) >> 5;
    if (atime->chunk >= data->nchunks) {
        atime->chunk = data->nchunks - 1;
    }
    atime->time_offset = atime->time - (float)(atime->chunk << 5);
    key = (int)floor(atime->time_offset) - 1;
    atime->time_byte = (unsigned char)(key / 8);
    atime->time_mask = (unsigned char)((1 << (key % 8 + 1)) - 1);
}


NuAnimDataChunk *NuAnimDataChunkCreate(int numnodes) {
    NuAnimDataChunk *chunk;
    int size;

    if (numnodes <= 0) {
        NuErrorProlog(nuanim_srcfile, 0x27C)(nuanim_assert_msg);
    }
    chunk = NuMemAllocFn(sizeof(NuAnimDataChunk), nuanim_srcfile, 0x27D);
    size = numnodes * sizeof(NuAnimCurveSet *);
    memset(chunk, 0, sizeof(NuAnimDataChunk));
    chunk->numnodes = numnodes;
    chunk->animcurvesets = NuMemAllocFn(size, nuanim_srcfile, 0x281);
    memset(chunk->animcurvesets, 0, size);
    return chunk;
}


void NuAnimDataChunkDestroy(NuAnimDataChunk *chunk) {
    NuAnimCurveSet *curveset;
    int destroy_curves;
    int i;

    destroy_curves = chunk->curves == 0;
    for (i = 0; i < chunk->numnodes; i++) {
        curveset = chunk->animcurvesets[i];
        if (curveset) {
            NuAnimCurveSetDestroy(curveset, destroy_curves);
        }
    }
    if (chunk->keys) {
        NuMemFreeFn(chunk->keys, nuanim_srcfile, 0x2C2);
    }
    if (chunk->curves) {
        NuMemFreeFn(chunk->curves, nuanim_srcfile, 0x2C4);
    }
    if (chunk->animcurvesets) {
        NuMemFreeFn(chunk->animcurvesets, nuanim_srcfile, 0x2C7);
    }
    NuMemFreeFn(chunk, nuanim_srcfile, 0x2C9);
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
