/*
 * Unit: nu3d/nuwater
 *
 * Functions:
 *   0x00163910 initWater
 *   0x00163af8 fixNewMaterials
 *   0x00163f28 fixMaterials
 *   0x00164668 NuWaterInit
 *   0x001647f8 animateWaterTables
 *   0x00164b68 animateWater
 *   0x00164e40 NuWaterRender
 *   0x00164fb0 NuWaterOverride
 *   0x00165068 NuWaterSpeed
 *   0x00165078 NuWaterLoad
 */

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef struct NuColour3 {
    float r;
    float g;
    float b;
} NuColour3;

typedef struct NuMtlAttrib {
    u32 : 32;
    u32 alpha : 4;
    u32 filter : 2;
    u32 fx : 2;
    u32 utc : 2;
    u32 vtc : 2;
    u32 cull : 2;
    u32 zmode : 2;
    u32 lighting : 2;
    u32 colour : 1;
    u32 fill : 1;
    u32 atst : 3;
    u32 : 9;
} NuMtlAttrib;

typedef struct NuMtlAttrib2 {
    u32 fx : 2;
    u32 : 30;
    u32 : 32;
} NuMtlAttrib2;

typedef union NuMtlFx {
    float f;
    int i;
} NuMtlFx;

typedef struct NuWaterFx {
    u8 pad00[0x8];
    float wave;
    int phase;
    float uscale;
    float vscale;
    float width;
    float height;
} NuWaterFx;

typedef struct NuVec {
    float x;
    float y;
    float z;
} NuVec;

typedef struct NuVec4 {
    float x;
    float y;
    float z;
    float w;
} NuVec4;

typedef struct NuMtx {
    float _00, _01, _02, _03;
    float _10, _11, _12, _13;
    float _20, _21, _22, _23;
    float _30, _31, _32, _33;
} NuMtx;

typedef struct NuCamera {
    NuMtx mtx;
} NuCamera;

typedef struct NuRndrStream {
    u32 *start;
    u32 *cur;
    u8 pad08[0x10];
    void *fxcode;
    u8 pad1C[0x1C];
} NuRndrStream;

typedef struct NuMtl {
    u64 pad000[0x24];
    u8 pad120[0x4];
    NuRndrStream stream;
    NuWaterFx *fxdata;
    u8 pad160[0x8];
    union {
        NuMtlAttrib attrib;
        struct NuMtl *layer;
    };
    NuMtlAttrib2 attrib2;
    u8 pad178[0x8];
    NuColour3 diffuse;
    NuMtlFx fx1;
    NuMtlFx fx2;
    NuMtlFx fx3;
    NuMtlFx fx4;
    float power;
    float alpha;
    int tid;
    int alpha_sort;
    u8 pad1AC[0x19];
    u8 fxid;
} NuMtl;

typedef struct NuGeom {
    struct NuGeom *next;
    NuMtl *mtl;
} NuGeom;

typedef struct NuGobj {
    u8 pad00[0xC];
    NuGeom *geom;
} NuGobj;

typedef struct NuInstance {
    NuMtx mtx;
    int objid;
    int flags;
    u8 pad48[0x8];
} NuInstance;

typedef struct NuGScn {
    u8 pad00[0x14];
    NuGobj **gobjs;
    int numinstance;
    NuInstance *instances;
} NuGScn;

typedef struct NuTexSpec {
    int type;
    int width;
    int height;
    int mmcnt;
    void *bits;
    int *pal;
    int decal;
    int linear;
} NuTexSpec;

typedef struct WaterTable {
    NuVec4 wave[32];
    float fx1;
    float fx4;
} WaterTable;

typedef void (*NuErrorFunc)(const char *fmt, ...);

extern NuErrorFunc NuErrorProlog(const char *file, int line);
extern NuMtl *NuMtlIterate(int first);
extern NuMtl *NuMtlCreate(int count);
extern void NuMtlUpdate(NuMtl *mtl);
extern int NuTexCreate(NuTexSpec *spec);
extern int NuTexGetReqSize(int tid, int level);
extern int NuTexReserve(int size);
extern void NuTexAssignAddr(int tid, int addr);
extern void NuTexUnReserve(void);
extern int NuPower2(int value);
extern int NuRndrBeginScene(int target);
extern void NuRndrEndScene(void);
extern void NuRndrRectUV2di(int x, int y, int w, int h, float u0, float v0, float u1, float v1,
                            u32 colour, NuMtl *mtl);
extern void NuRndrGobj(NuGobj *gobj, NuInstance *inst);
extern void NuMtxInvR(NuMtx *dst, NuMtx *src);
extern void NuMtxScale(NuMtx *m, NuVec *scale);
extern void NuVecMtxTransformH(NuVec *dst, NuVec *src, NuMtx *m);
extern void NuRandSeed(int seed);
extern float NuRandFloat(void);
extern void NuRndrStreamLink(NuRndrStream *stream);
extern u32 *vpDmaTag_Cnt(u32 *cur);
extern u32 *vpDmaTag_Close(u32 *cur);
extern u32 *vpDmaTag_Next(u32 *cur, int mode);

extern NuCamera global_camera;
extern float NuTrigTable[];
extern u32 *rndrstream_free;
extern int PS2_REZ_W;
extern int PS2_REZ_H;
extern int skip_grab;

extern char D_00616100[];
#define nuwater_file D_00616100

extern char D_0062F000[];
#define msg_assert D_0062F000

extern char D_0028F470[];
#define water_microcode D_0028F470

extern char D_00290840[];
#define water_alt_microcode D_00290840

extern int D_006331AC;
#define nwaterinst D_006331AC

extern NuMtl *D_006331B0;
#define water_grab_mtl D_006331B0

extern int D_006331B4;
#define water_grab_tid D_006331B4

extern float D_006331B8;
#define water_grab_u D_006331B8

extern float D_006331BC;
#define water_grab_v D_006331BC

extern int D_0062EFEC;
#define nwatermtl D_0062EFEC

extern float D_0062EFF0;
#define water_speed D_0062EFF0

extern int D_0062EFF4;
#define water_grab D_0062EFF4

extern int D_0062EFF8;
#define water_use_tables D_0062EFF8

extern int D_0062EFFC;
#define nwatertable D_0062EFFC

extern float D_0062F008;
#define water_time D_0062F008

extern NuMtl *D_006B1EB0[8];
#define watermtl D_006B1EB0

extern int D_006B1ED0[8];
#define watermtl_table D_006B1ED0

extern NuInstance *D_006B1EF0[256];
#define waterinst D_006B1EF0

extern WaterTable D_006B22F0[8];
#define watertable D_006B22F0


static inline int isWaterGobj(NuGobj *gobj) {
    NuGeom *geom;

    for (geom = gobj->geom; geom; geom = geom->next) {
        switch (geom->mtl->fxid) {
        case 4:
        case 0x84:
        case 0x85:
        case 0x86:
            return 1;
        }
    }
    return 0;
}


void initWater(NuGScn *scene) {
    int i;
    int count;

    count = 0;
    for (i = 0, nwaterinst = 0; i < scene->numinstance; i++) {
        if (isWaterGobj(scene->gobjs[scene->instances[i].objid])) {
            nwaterinst++;
        }
    }
    if (nwaterinst >= 256) {
        NuErrorProlog(nuwater_file, 0x98)(msg_assert);
    }
    for (i = 0; i < scene->numinstance; i++) {
        if (isWaterGobj(scene->gobjs[scene->instances[i].objid])) {
            waterinst[count++] = &scene->instances[i];
            scene->instances[i].flags &= ~1;
        }
    }
}


static inline u32 getWaterTable(float fx1, float fx4) {
    u32 table;
    WaterTable *wt;
    WaterTable *tables;

    for (table = 0; table < nwatertable; table++) {
        wt = &watertable[table];
        if (wt->fx1 == fx1 && wt->fx4 == fx4) {
            return table;
        }
    }
    tables = watertable;
    table = nwatertable++;
    tables[table].fx1 = fx1;
    tables[table].fx4 = fx4;
    return table;
}


void fixNewMaterials(void) {
    NuMtl *mtl;
    int width;
    int height;
    u32 table;

    for (mtl = NuMtlIterate(1); mtl; mtl = NuMtlIterate(0)) {
        switch (mtl->fxid) {
        case 0x85:
        case 0x86:
            if (mtl->fxid == 0x86) {
                mtl->stream.fxcode = water_alt_microcode;
            } else {
                mtl->stream.fxcode = water_microcode;
            }
            mtl->fxdata->wave = mtl->fx3.f / mtl->fx4.f;
            mtl->fxdata->phase = 0;
            width = 1;
            while (width < PS2_REZ_W) {
                width <<= 1;
            }
            mtl->fxdata->uscale = 1.0 / (width * 16.0);
            for (height = 1; height < PS2_REZ_H; height <<= 1) {
            }
            mtl->fxdata->vscale = 1.0 / (height * 16.0);
            mtl->fxdata->width = PS2_REZ_W;
            mtl->fxdata->height = PS2_REZ_H;
            table = getWaterTable(mtl->fx1.f, mtl->fx4.f);
            watermtl_table[nwatermtl] = table;
            if (nwatermtl >= 8) {
                NuErrorProlog(nuwater_file, 0xE0)(msg_assert);
            }
            watermtl[nwatermtl++] = mtl;
            break;
        case 0x84:
            water_grab = 1;
            mtl->tid = water_grab_tid;
            NuMtlUpdate(mtl);
            mtl->stream.fxcode = water_microcode;
            mtl->fxdata->wave = mtl->fx2.f / 48.0f / mtl->fx4.f;
            mtl->fxdata->phase = 0;
            width = 1;
            while (width < PS2_REZ_W) {
                width <<= 1;
            }
            mtl->fxdata->uscale = 1.0 / (width * 16.0);
            for (height = 1; height < PS2_REZ_H; height <<= 1) {
            }
            mtl->fxdata->vscale = 1.0 / (height * 16.0);
            mtl->fxdata->width = PS2_REZ_W;
            mtl->fxdata->height = PS2_REZ_H;
            table = getWaterTable(mtl->fx1.f, mtl->fx4.f);
            watermtl_table[nwatermtl] = table;
            if (nwatermtl >= 8) {
                NuErrorProlog(nuwater_file, 0xF6)(msg_assert);
            }
            watermtl[nwatermtl++] = mtl;
            break;
        }
    }
}


void fixMaterials(void) {
    NuMtl *mtl;
    NuMtl *layer;
    NuColour3 diffuse;
    int fxid;
    int i;
    int width;
    int height;
    u32 table;

    nwatermtl = 0;
    for (mtl = NuMtlIterate(1); mtl; mtl = NuMtlIterate(0)) {
        if (mtl->fxid == 4) {
            if (nwatermtl >= 8) {
                NuErrorProlog(nuwater_file, 0x110)(msg_assert);
            }
            watermtl[nwatermtl++] = mtl;
        }
    }
    for (i = 0; i < nwatermtl; i++) {
        mtl = watermtl[i];
        fxid = mtl->fxid;
        if (fxid != 4) {
            continue;
        }
        water_grab = 1;
        diffuse = mtl->diffuse;
        layer = NuMtlCreate(1);
        layer->fxid = fxid;
        layer->tid = mtl->tid;
        layer->attrib.utc = 1;
        layer->attrib.vtc = 1;
        layer->diffuse.r = 1.0f;
        layer->diffuse.g = 1.0f;
        layer->diffuse.b = 1.0f;
        layer->fx1 = mtl->fx1;
        layer->fx2 = mtl->fx2;
        layer->fx3 = mtl->fx3;
        layer->fx4 = mtl->fx4;
        layer->alpha = mtl->alpha;
        layer->attrib.alpha = mtl->attrib.alpha;
        layer->attrib.atst = mtl->attrib.atst;
        layer->attrib2.fx = mtl->attrib2.fx;
        layer->attrib.fx = 1;
        layer->attrib.zmode = 0;
        layer->attrib.lighting = 0;
        layer->attrib.colour = 1;
        layer->alpha_sort = 0x7FFF;
        NuMtlUpdate(layer);
        layer->stream.fxcode = water_microcode;
        layer->fxdata->wave = mtl->fx3.f / mtl->fx4.f;
        layer->fxdata->phase = 0;
        width = 1;
        while (width < PS2_REZ_W) {
            width <<= 1;
        }
        layer->fxdata->uscale = 1.0 / (width * 16.0);
        for (height = 1; height < PS2_REZ_H; height <<= 1) {
        }
        layer->fxdata->vscale = 1.0 / (height * 16.0);
        layer->fxdata->width = PS2_REZ_W;
        layer->fxdata->height = PS2_REZ_H;
        layer->layer = 0;
        table = getWaterTable(layer->fx1.f, layer->fx4.f);
        watermtl_table[nwatermtl + i] = table;
        watermtl[nwatermtl + i] = layer;
        mtl->diffuse = diffuse;
        mtl->attrib.alpha = 0;
        mtl->attrib.utc = 1;
        mtl->attrib.vtc = 1;
        mtl->attrib.lighting = 0;
        mtl->attrib.zmode = 0;
        mtl->attrib.fx = 1;
        mtl->attrib.colour = 1;
        mtl->alpha = 1.0f;
        mtl->tid = water_grab_tid;
        mtl->alpha_sort = 0x7FFE;
        NuMtlUpdate(mtl);
        mtl->stream.fxcode = water_microcode;
        mtl->fxdata->wave = mtl->fx2.f / 48.0f / mtl->fx4.f;
        mtl->fxdata->phase = 0;
        width = 1;
        while (width < PS2_REZ_W) {
            width <<= 1;
        }
        mtl->fxdata->uscale = 1.0 / (width * 16.0);
        for (height = 1; height < PS2_REZ_H; height <<= 1) {
        }
        mtl->fxdata->vscale = 1.0 / (height * 16.0);
        mtl->fxdata->width = PS2_REZ_W;
        mtl->fxdata->height = PS2_REZ_H;
        table = getWaterTable(mtl->fx1.f, mtl->fx4.f);
        watermtl_table[nwatermtl] = table;
        mtl->layer = layer;
    }
    nwatermtl *= 2;
    fixNewMaterials();
}


void NuWaterInit(NuGScn *scene) {
    NuTexSpec spec;
    NuMtl *mtl;

    water_grab = 0;
    spec.type = 0xF;
    spec.width = PS2_REZ_W;
    spec.height = PS2_REZ_H;
    water_grab_tid = NuTexCreate(&spec);
    water_grab_u = ((float)PS2_REZ_W - 1.0f) / (float)NuPower2(PS2_REZ_W);
    water_grab_v = ((float)PS2_REZ_H - 1.0f) / (float)NuPower2(PS2_REZ_H);
    mtl = NuMtlCreate(1);
    water_grab_mtl = mtl;
    mtl->attrib.cull = 2;
    mtl->attrib.zmode = 3;
    mtl->attrib.filter = 0;
    mtl->attrib.lighting = 2;
    mtl->attrib.colour = 1;
    mtl->diffuse.r = 1.0f;
    mtl->diffuse.g = 1.0f;
    mtl->diffuse.b = 1.0f;
    mtl->alpha = 0.999f;
    mtl->attrib.alpha = 0;
    mtl->tid = -1;
    NuMtlUpdate(mtl);
    nwaterinst = 0;
    if (scene) {
        initWater(scene);
        fixMaterials();
    }
}


void animateWaterTables(float scale, float time) {
    NuVec wave;
    NuVec pos;
    NuMtx mtx;
    NuVec mtxscale = {0.5f, 0.5f, 0.5f};
    NuMtl *mtl;
    NuRndrStream *stream;
    float *table;
    float amp;
    float freq;
    int i;
    int j;

    NuMtxInvR(&mtx, &global_camera.mtx);
    NuMtxScale(&mtx, &mtxscale);
    mtx._03 = mtx._13 = mtx._23 = 0.0f;
    for (i = 0; i < nwatertable; i++) {
        freq = time * watermtl[i]->fx1.f;
        amp = watermtl[i]->fx4.f * scale;
        table = &watertable[i].wave[0].x;
        NuRandSeed(0x11);
        for (j = 0; j < 32; j++) {
            wave.x = NuTrigTable[(u16)((freq * (NuRandFloat() * 0.4f + 0.8f) + NuRandFloat() * 6.284f) * 10430.378f)] * amp;
            wave.y = 0.0f;
            wave.z = NuTrigTable[(u16)((freq * (NuRandFloat() * 0.4f + 0.7f) + NuRandFloat() * 6.284f) * 10430.378f)] * amp;
            NuVecMtxTransformH(&pos, &wave, &mtx);
            *table++ = pos.x;
            *table++ = pos.y;
            *table++ = 1.0f;
            *table++ = wave.x;
        }
    }
    for (i = 0; i < nwatermtl; i++) {
        mtl = watermtl[i];
        table = &watertable[watermtl_table[i]].wave[0].x;
        stream = &mtl->stream;
        NuRndrStreamLink(stream);
        stream->cur = vpDmaTag_Cnt(stream->cur);
        *stream->cur++ = 0x10000000;
        *stream->cur++ = 0x6C2001B2;
        for (j = 0; j < 32; j++) {
            *((NuVec4 *)stream->cur)++ = *(NuVec4 *)table;
            table += 4;
        }
        stream->cur = vpDmaTag_Close(stream->cur);
        rndrstream_free = vpDmaTag_Next(stream->cur, 0);
    }
}


void animateWater(float scale, float time) {
    NuVec wave;
    NuVec pos;
    NuMtx mtx;
    NuVec mtxscale = {0.5f, 0.5f, 0.5f};
    NuMtl *mtl;
    NuRndrStream *stream;
    float amp;
    float freq;
    int i;
    int j;

    NuMtxInvR(&mtx, &global_camera.mtx);
    NuMtxScale(&mtx, &mtxscale);
    mtx._03 = mtx._13 = mtx._23 = 0.0f;
    for (i = 0; i < nwatermtl; i++) {
        mtl = watermtl[i];
        freq = time * mtl->fx1.f;
        amp = mtl->fx4.f * scale;
        stream = &mtl->stream;
        NuRndrStreamLink(stream);
        stream->cur = vpDmaTag_Cnt(stream->cur);
        *stream->cur++ = 0;
        *stream->cur++ = 0;
        *stream->cur++ = 0x10000000;
        *stream->cur++ = 0x6C2001B2;
        NuRandSeed(0x11);
        for (j = 0; j < 32; j++) {
            wave.x = NuTrigTable[(u16)((freq * (NuRandFloat() * 0.4f + 0.8f) + NuRandFloat() * 6.284f) * 10430.378f)] * amp;
            wave.y = 0.0f;
            wave.z = NuTrigTable[(u16)((freq * (NuRandFloat() * 0.4f + 0.7f) + NuRandFloat() * 6.284f) * 10430.378f)] * amp;
            NuVecMtxTransformH(&pos, &wave, &mtx);
            *((float *)stream->cur)++ = pos.x;
            *((float *)stream->cur)++ = pos.y;
            *((float *)stream->cur)++ = 1.0f;
            *((float *)stream->cur)++ = wave.x;
        }
        stream->cur = vpDmaTag_Close(stream->cur);
        rndrstream_free = vpDmaTag_Next(stream->cur, 0);
    }
}


void NuWaterRender(NuGScn *scene) {
    int i;
    NuInstance *inst;

    if (scene == 0 || nwaterinst == 0) {
        return;
    }
    if (water_grab) {
        NuTexAssignAddr(water_grab_tid, NuTexReserve(NuTexGetReqSize(water_grab_tid, 0)));
        if (!skip_grab && NuRndrBeginScene(water_grab_tid)) {
            NuRndrRectUV2di(0, 0, PS2_REZ_W << 4, PS2_REZ_H << 4, 0.0f, 0.0f, water_grab_u,
                            water_grab_v, 0x80808080, water_grab_mtl);
            NuRndrEndScene();
        }
    }
    if (NuRndrBeginScene(-1)) {
        if (water_use_tables) {
            animateWaterTables(0.1f, water_time);
        } else {
            animateWater(0.1f, water_time);
        }
        for (i = 0; i < nwaterinst; i++) {
            inst = waterinst[i];
            if (inst->flags & 4) {
                NuRndrGobj(scene->gobjs[inst->objid], inst);
            }
        }
        water_time += water_speed;
        NuRndrEndScene();
    }
    if (water_grab) {
        NuTexUnReserve();
    }
}


void NuWaterOverride(float fx1, float fx2, float fx3, float fx4) {
    int i;
    NuMtl *mtl;
    float unset;

    unset = 4294967296.0f;
    for (i = 0; i < nwatermtl; i++) {
        mtl = watermtl[i];
        if (fx1 != unset) {
            mtl->fx1.f = fx1;
        }
        if (fx2 != unset) {
            mtl->fx2.f = fx2;
        }
        if (fx3 != unset) {
            mtl->fx3.f = fx3;
        }
        if (fx4 != unset) {
            mtl->fx4.f = fx4;
        }
        if (mtl->alpha_sort == 0x7FFE) {
            mtl->fxdata->wave = mtl->fx2.f / 48.0f / mtl->fx4.f;
        } else {
            mtl->fxdata->wave = mtl->fx3.f / mtl->fx4.f;
        }
    }
}


void NuWaterSpeed(float speed) {
    water_speed = speed * 0.1f;
}


void NuWaterLoad(NuGScn *scene) {
    nwaterinst = 0;
    if (scene) {
        initWater(scene);
        fixMaterials();
    }
}
