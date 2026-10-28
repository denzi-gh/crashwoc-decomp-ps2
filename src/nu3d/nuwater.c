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

#include "nu_port.h"
#include "nu_types.h"

#include "nucore/nuerror.h"
#include "numath/numathtypes.h"
#include "numath/numtx.h"
#include "numath/nurand.h"
#include "numath/nutrig.h"
#include "numath/nuvec.h"
#include "nu3d/numtl.h"
#include "nu3d/nustream.h"
#include "nu3d/nutex.h"
#include "nu3d/nugobj.h"
#include "nu3d/nucamera.h"
#include "nu3d/nugscn.h"

typedef struct waterTableEntry_s {
    struct nuvec4_s wave[32];
    float fx1;
    float fx4;
} WATERTABLEENTRY;

extern int NuRndrBeginScene(int target);
extern void NuRndrEndScene(void);
extern void NuRndrRectUV2di(int x, int y, int w, int h, float u0, float v0, float u1, float v1,
                            u32 colour, struct numtl_s *mtl);
extern void NuRndrGobj(struct nugobj_s *gobj, struct nuinstance_s *inst);
extern void NuRndrStreamLink(struct rndrstream_s *stream);
extern u32 *vpDmaTag_Cnt(u32 *cur);
extern u32 *vpDmaTag_Close(u32 *cur);
extern u32 *vpDmaTag_Next(u32 *cur, int mode);

extern struct nucamera_s global_camera;
extern float NuTrigTable[];
extern u32 *rndrstream_free;
extern int PS2_REZ_W;
extern int PS2_REZ_H;
int skip_grab = 0;
NW_STATIC int nwater_mtl = 0;
static float theta_step = 0.1f;
static int get_screen = 0;
static int water_tables_active = 0;
NW_STATIC int nwater_tables = 0;
static int nwinst;
static struct numtl_s *water_copy_mtl;
static int water_rt;
static float wateru1;
static float waterv1;
NW_STATIC struct numtl_s *water_mtls[8];
NW_STATIC int water_mtl_tables[8];
static struct nuinstance_s *winst[256];
NW_STATIC WATERTABLEENTRY water_table[8];

#define nuwater_file "..\\nu2crash.ps2\\nu3d\\nuwater.c"

extern char vu_load_water_mpg[];

extern char vu_load_wateruv_mpg[];


static inline int isWaterGobj(struct nugobj_s *gobj) {
    struct nugeom_s *geom;

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


void initWater(struct nugscn_s *scene) {
    int i;
    int count;

    count = 0;
    for (i = 0, nwinst = 0; i < scene->numinstance; i++) {
        if (isWaterGobj(scene->gobjs[scene->instances[i].objid])) {
            nwinst++;
        }
    }
    if (nwinst >= 256) {
        NuErrorProlog(nuwater_file, 0x98)("assert");
    }
    for (i = 0; i < scene->numinstance; i++) {
        if (isWaterGobj(scene->gobjs[scene->instances[i].objid])) {
            winst[count++] = &scene->instances[i];
            scene->instances[i].flagword &= ~1;
        }
    }
}


static inline u32 getWaterTable(float fx1, float fx4) {
    u32 table;
    WATERTABLEENTRY *wt;
    WATERTABLEENTRY *tables;

    for (table = 0; table < nwater_tables; table++) {
        wt = &water_table[table];
        if (wt->fx1 == fx1 && wt->fx4 == fx4) {
            return table;
        }
    }
    tables = water_table;
    table = nwater_tables++;
    tables[table].fx1 = fx1;
    tables[table].fx4 = fx4;
    return table;
}


void fixNewMaterials(void) {
    struct numtl_s *mtl;
    int width;
    int height;
    u32 table;

    for (mtl = NuMtlIterate(1); mtl; mtl = NuMtlIterate(0)) {
        switch (mtl->fxid) {
        case 0x85:
        case 0x86:
            if (mtl->fxid == 0x86) {
                mtl->rs.mpg = vu_load_wateruv_mpg;
            } else {
                mtl->rs.mpg = vu_load_water_mpg;
            }
            mtl->vuattr.vec3->z = mtl->fx3.f32 / mtl->fx4.f32;
            mtl->vuattr.vec4->w = 0;
            width = 1;
            while (width < PS2_REZ_W) {
                width <<= 1;
            }
            mtl->vuattr.mtx44->_10 = 1.0 / (width * 16.0);
            for (height = 1; height < PS2_REZ_H; height <<= 1) {
            }
            mtl->vuattr.mtx44->_11 = 1.0 / (height * 16.0);
            mtl->vuattr.mtx44->_12 = PS2_REZ_W;
            mtl->vuattr.mtx44->_13 = PS2_REZ_H;
            table = getWaterTable(mtl->fx1.f32, mtl->fx4.f32);
            water_mtl_tables[nwater_mtl] = table;
            if (nwater_mtl >= 8) {
                NuErrorProlog(nuwater_file, 0xE0)("assert");
            }
            water_mtls[nwater_mtl++] = mtl;
            break;
        case 0x84:
            get_screen = 1;
            mtl->tid = water_rt;
            NuMtlUpdate(mtl);
            mtl->rs.mpg = vu_load_water_mpg;
            mtl->vuattr.vec3->z = mtl->fx2.f32 / 48.0f / mtl->fx4.f32;
            mtl->vuattr.vec4->w = 0;
            width = 1;
            while (width < PS2_REZ_W) {
                width <<= 1;
            }
            mtl->vuattr.mtx44->_10 = 1.0 / (width * 16.0);
            for (height = 1; height < PS2_REZ_H; height <<= 1) {
            }
            mtl->vuattr.mtx44->_11 = 1.0 / (height * 16.0);
            mtl->vuattr.mtx44->_12 = PS2_REZ_W;
            mtl->vuattr.mtx44->_13 = PS2_REZ_H;
            table = getWaterTable(mtl->fx1.f32, mtl->fx4.f32);
            water_mtl_tables[nwater_mtl] = table;
            if (nwater_mtl >= 8) {
                NuErrorProlog(nuwater_file, 0xF6)("assert");
            }
            water_mtls[nwater_mtl++] = mtl;
            break;
        }
    }
}


void fixMaterials(void) {
    struct numtl_s *mtl;
    struct numtl_s *layer;
    struct nucolour3_s diffuse;
    int fxid;
    int i;
    int width;
    int height;
    u32 table;

    nwater_mtl = 0;
    for (mtl = NuMtlIterate(1); mtl; mtl = NuMtlIterate(0)) {
        if (mtl->fxid == 4) {
            if (nwater_mtl >= 8) {
                NuErrorProlog(nuwater_file, 0x110)("assert");
            }
            water_mtls[nwater_mtl++] = mtl;
        }
    }
    for (i = 0; i < nwater_mtl; i++) {
        mtl = water_mtls[i];
        fxid = mtl->fxid;
        if (fxid != 4) {
            continue;
        }
        get_screen = 1;
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
        /* atst is 3 bits, so % 8 is a no-op; it changes the RTL that sets the
           $s7/$fp allocation tie between water_mtls and water_table. */
        layer->attrib.atst = mtl->attrib.atst % 8;
        layer->attrib.afail = mtl->attrib.afail;
        layer->attrib.fx = 1;
        layer->attrib.zmode = 0;
        layer->attrib.lighting = 0;
        layer->attrib.colour = 1;
        layer->sort_pri = 0x7FFF;
        NuMtlUpdate(layer);
        layer->rs.mpg = vu_load_water_mpg;
        layer->vuattr.vec3->z = mtl->fx3.f32 / mtl->fx4.f32;
        layer->vuattr.vec4->w = 0;
        width = 1;
        while (width < PS2_REZ_W) {
            width <<= 1;
        }
        layer->vuattr.mtx44->_10 = 1.0 / (width * 16.0);
        for (height = 1; height < PS2_REZ_H; height <<= 1) {
        }
        layer->vuattr.mtx44->_11 = 1.0 / (height * 16.0);
        layer->vuattr.mtx44->_12 = PS2_REZ_W;
        layer->vuattr.mtx44->_13 = PS2_REZ_H;
        layer->next = 0;
        table = getWaterTable(layer->fx1.f32, layer->fx4.f32);
        water_mtl_tables[nwater_mtl + i] = table;
        water_mtls[nwater_mtl + i] = layer;
        mtl->diffuse = diffuse;
        mtl->attrib.alpha = 0;
        mtl->attrib.utc = 1;
        mtl->attrib.vtc = 1;
        mtl->attrib.lighting = 0;
        mtl->attrib.zmode = 0;
        mtl->attrib.fx = 1;
        mtl->attrib.colour = 1;
        mtl->alpha = 1.0f;
        mtl->tid = water_rt;
        mtl->sort_pri = 0x7FFE;
        NuMtlUpdate(mtl);
        mtl->rs.mpg = vu_load_water_mpg;
        mtl->vuattr.vec3->z = mtl->fx2.f32 / 48.0f / mtl->fx4.f32;
        mtl->vuattr.vec4->w = 0;
        width = 1;
        while (width < PS2_REZ_W) {
            width <<= 1;
        }
        mtl->vuattr.mtx44->_10 = 1.0 / (width * 16.0);
        for (height = 1; height < PS2_REZ_H; height <<= 1) {
        }
        mtl->vuattr.mtx44->_11 = 1.0 / (height * 16.0);
        mtl->vuattr.mtx44->_12 = PS2_REZ_W;
        mtl->vuattr.mtx44->_13 = PS2_REZ_H;
        table = getWaterTable(mtl->fx1.f32, mtl->fx4.f32);
        water_mtl_tables[nwater_mtl] = table;
        mtl->next = layer;
    }
    nwater_mtl *= 2;
    fixNewMaterials();
}


void NuWaterInit(struct nugscn_s *scene) {
    struct nutex_s spec;
    struct numtl_s *mtl;

    get_screen = 0;
    spec.type = 0xF;
    spec.width = PS2_REZ_W;
    spec.height = PS2_REZ_H;
    water_rt = NuTexCreate(&spec);
    wateru1 = ((float)PS2_REZ_W - 1.0f) / (float)NuPower2(PS2_REZ_W);
    waterv1 = ((float)PS2_REZ_H - 1.0f) / (float)NuPower2(PS2_REZ_H);
    mtl = NuMtlCreate(1);
    water_copy_mtl = mtl;
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
    nwinst = 0;
    if (scene) {
        initWater(scene);
        fixMaterials();
    }
}


void animateWaterTables(float scale, float time) {
    struct nuvec_s wave;
    struct nuvec_s pos;
    struct numtx_s mtx;
    struct nuvec_s mtxscale = {0.5f, 0.5f, 0.5f};
    struct numtl_s *mtl;
    struct rndrstream_s *stream;
    float *table;
    float amp;
    float freq;
    int i;
    int j;

    NuMtxInvR(&mtx, &global_camera.mtx);
    NuMtxScale(&mtx, &mtxscale);
    mtx._03 = mtx._13 = mtx._23 = 0.0f;
    for (i = 0; i < nwater_tables; i++) {
        freq = time * water_mtls[i]->fx1.f32;
        amp = water_mtls[i]->fx4.f32 * scale;
        table = &water_table[i].wave[0].x;
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
    for (i = 0; i < nwater_mtl; i++) {
        table = &water_table[water_mtl_tables[i]].wave[0].x;
        mtl = water_mtls[i];
        stream = &mtl->rs;
        NuRndrStreamLink(stream);
        stream->ptr.u32 = vpDmaTag_Cnt(stream->ptr.u32);
        *stream->ptr.u32++ = 0x10000000;
        *stream->ptr.u32++ = 0x6C2001B2;
        for (j = 0; j < 32; j++) {
            NW_PUT(struct nuvec4_s, stream->ptr.u32, *(struct nuvec4_s *)table);
            table += 4;
        }
        stream->ptr.u32 = vpDmaTag_Close(stream->ptr.u32);
        rndrstream_free = vpDmaTag_Next(stream->ptr.u32, 0);
    }
}


void animateWater(float scale, float time) {
    struct nuvec_s wave;
    struct nuvec_s pos;
    struct numtx_s mtx;
    struct nuvec_s mtxscale = {0.5f, 0.5f, 0.5f};
    struct numtl_s *mtl;
    struct rndrstream_s *stream;
    float amp;
    float freq;
    int i;
    int j;

    NuMtxInvR(&mtx, &global_camera.mtx);
    NuMtxScale(&mtx, &mtxscale);
    mtx._03 = mtx._13 = mtx._23 = 0.0f;
    for (i = 0; i < nwater_mtl; i++) {
        mtl = water_mtls[i];
        freq = time * mtl->fx1.f32;
        amp = mtl->fx4.f32 * scale;
        stream = &mtl->rs;
        NuRndrStreamLink(stream);
        stream->ptr.u32 = vpDmaTag_Cnt(stream->ptr.u32);
        *stream->ptr.u32++ = 0;
        *stream->ptr.u32++ = 0;
        *stream->ptr.u32++ = 0x10000000;
        *stream->ptr.u32++ = 0x6C2001B2;
        NuRandSeed(0x11);
        for (j = 0; j < 32; j++) {
            wave.x = NuTrigTable[(u16)((freq * (NuRandFloat() * 0.4f + 0.8f) + NuRandFloat() * 6.284f) * 10430.378f)] * amp;
            wave.y = 0.0f;
            wave.z = NuTrigTable[(u16)((freq * (NuRandFloat() * 0.4f + 0.7f) + NuRandFloat() * 6.284f) * 10430.378f)] * amp;
            NuVecMtxTransformH(&pos, &wave, &mtx);
            NW_PUT(float, stream->ptr.u32, pos.x);
            NW_PUT(float, stream->ptr.u32, pos.y);
            NW_PUT(float, stream->ptr.u32, 1.0f);
            NW_PUT(float, stream->ptr.u32, wave.x);
        }
        stream->ptr.u32 = vpDmaTag_Close(stream->ptr.u32);
        rndrstream_free = vpDmaTag_Next(stream->ptr.u32, 0);
    }
}


void NuWaterRender(struct nugscn_s *scene) {
    static float theta = 7.0f;
    int i;
    struct nuinstance_s *inst;

    if (scene == 0 || nwinst == 0) {
        return;
    }
    if (get_screen) {
        NuTexAssignAddr(water_rt, NuTexReserve(NuTexGetReqSize(water_rt, 0)));
        if (!skip_grab && NuRndrBeginScene(water_rt)) {
            NuRndrRectUV2di(0, 0, PS2_REZ_W << 4, PS2_REZ_H << 4, 0.0f, 0.0f, wateru1,
                            waterv1, 0x80808080, water_copy_mtl);
            NuRndrEndScene();
        }
    }
    if (NuRndrBeginScene(-1)) {
        if (water_tables_active) {
            animateWaterTables(0.1f, theta);
        } else {
            animateWater(0.1f, theta);
        }
        for (i = 0; i < nwinst; i++) {
            inst = winst[i];
            if (inst->flagword & 4) {
                NuRndrGobj(scene->gobjs[inst->objid], inst);
            }
        }
        theta += theta_step;
        NuRndrEndScene();
    }
    if (get_screen) {
        NuTexUnReserve();
    }
}


void NuWaterOverride(float fx1, float fx2, float fx3, float fx4) {
    int i;
    struct numtl_s *mtl;
    float unset;

    unset = 4294967296.0f;
    for (i = 0; i < nwater_mtl; i++) {
        mtl = water_mtls[i];
        if (fx1 != unset) {
            mtl->fx1.f32 = fx1;
        }
        if (fx2 != unset) {
            mtl->fx2.f32 = fx2;
        }
        if (fx3 != unset) {
            mtl->fx3.f32 = fx3;
        }
        if (fx4 != unset) {
            mtl->fx4.f32 = fx4;
        }
        if (mtl->sort_pri == 0x7FFE) {
            mtl->vuattr.vec3->z = mtl->fx2.f32 / 48.0f / mtl->fx4.f32;
        } else {
            mtl->vuattr.vec3->z = mtl->fx3.f32 / mtl->fx4.f32;
        }
    }
}


void NuWaterSpeed(float speed) {
    theta_step = speed * 0.1f;
}


void NuWaterLoad(struct nugscn_s *scene) {
    nwinst = 0;
    if (scene) {
        initWater(scene);
        fixMaterials();
    }
}
