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
extern void NuRndrStreamLink(struct nurndrstream_s *stream);
extern u32 *vpDmaTag_Cnt(u32 *cur);
extern u32 *vpDmaTag_Close(u32 *cur);
extern u32 *vpDmaTag_Next(u32 *cur, int mode);

extern struct nucamera_s global_camera;
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

extern struct numtl_s *D_006331B0;
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

extern struct numtl_s *D_006B1EB0[8];
#define watermtl D_006B1EB0

extern int D_006B1ED0[8];
#define watermtl_table D_006B1ED0

extern struct nuinstance_s *D_006B1EF0[256];
#define waterinst D_006B1EF0

extern WATERTABLEENTRY D_006B22F0[8];
#define watertable D_006B22F0


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
            scene->instances[i].flagword &= ~1;
        }
    }
}


static inline u32 getWaterTable(float fx1, float fx4) {
    u32 table;
    WATERTABLEENTRY *wt;
    WATERTABLEENTRY *tables;

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
    struct numtl_s *mtl;
    int width;
    int height;
    u32 table;

    for (mtl = NuMtlIterate(1); mtl; mtl = NuMtlIterate(0)) {
        switch (mtl->fxid) {
        case 0x85:
        case 0x86:
            if (mtl->fxid == 0x86) {
                mtl->rs.mpgtag = water_alt_microcode;
            } else {
                mtl->rs.mpgtag = water_microcode;
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
            mtl->rs.mpgtag = water_microcode;
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
    struct numtl_s *mtl;
    struct numtl_s *layer;
    struct nucolour3_s diffuse;
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
        /* atst is 3 bits, so % 8 is a no-op; it changes the RTL that sets the
           $s7/$fp allocation tie between watermtl and watertable. */
        layer->attrib.atst = mtl->attrib.atst % 8;
        layer->attrib.afail = mtl->attrib.afail;
        layer->attrib.fx = 1;
        layer->attrib.zmode = 0;
        layer->attrib.lighting = 0;
        layer->attrib.colour = 1;
        layer->sort_pri = 0x7FFF;
        NuMtlUpdate(layer);
        layer->rs.mpgtag = water_microcode;
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
        mtl->sort_pri = 0x7FFE;
        NuMtlUpdate(mtl);
        mtl->rs.mpgtag = water_microcode;
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
        watermtl_table[nwatermtl] = table;
        mtl->next = layer;
    }
    nwatermtl *= 2;
    fixNewMaterials();
}


void NuWaterInit(struct nugscn_s *scene) {
    struct nutex_s spec;
    struct numtl_s *mtl;

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
    struct nuvec_s wave;
    struct nuvec_s pos;
    struct numtx_s mtx;
    struct nuvec_s mtxscale = {0.5f, 0.5f, 0.5f};
    struct numtl_s *mtl;
    struct nurndrstream_s *stream;
    float *table;
    float amp;
    float freq;
    int i;
    int j;

    NuMtxInvR(&mtx, &global_camera.mtx);
    NuMtxScale(&mtx, &mtxscale);
    mtx._03 = mtx._13 = mtx._23 = 0.0f;
    for (i = 0; i < nwatertable; i++) {
        freq = time * watermtl[i]->fx1.f32;
        amp = watermtl[i]->fx4.f32 * scale;
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
        table = &watertable[watermtl_table[i]].wave[0].x;
        mtl = watermtl[i];
        stream = &mtl->rs;
        NuRndrStreamLink(stream);
        stream->cur.w = vpDmaTag_Cnt(stream->cur.w);
        *stream->cur.w++ = 0x10000000;
        *stream->cur.w++ = 0x6C2001B2;
        for (j = 0; j < 32; j++) {
            *((struct nuvec4_s *)stream->cur.w)++ = *(struct nuvec4_s *)table;
            table += 4;
        }
        stream->cur.w = vpDmaTag_Close(stream->cur.w);
        rndrstream_free = vpDmaTag_Next(stream->cur.w, 0);
    }
}


void animateWater(float scale, float time) {
    struct nuvec_s wave;
    struct nuvec_s pos;
    struct numtx_s mtx;
    struct nuvec_s mtxscale = {0.5f, 0.5f, 0.5f};
    struct numtl_s *mtl;
    struct nurndrstream_s *stream;
    float amp;
    float freq;
    int i;
    int j;

    NuMtxInvR(&mtx, &global_camera.mtx);
    NuMtxScale(&mtx, &mtxscale);
    mtx._03 = mtx._13 = mtx._23 = 0.0f;
    for (i = 0; i < nwatermtl; i++) {
        mtl = watermtl[i];
        freq = time * mtl->fx1.f32;
        amp = mtl->fx4.f32 * scale;
        stream = &mtl->rs;
        NuRndrStreamLink(stream);
        stream->cur.w = vpDmaTag_Cnt(stream->cur.w);
        *stream->cur.w++ = 0;
        *stream->cur.w++ = 0;
        *stream->cur.w++ = 0x10000000;
        *stream->cur.w++ = 0x6C2001B2;
        NuRandSeed(0x11);
        for (j = 0; j < 32; j++) {
            wave.x = NuTrigTable[(u16)((freq * (NuRandFloat() * 0.4f + 0.8f) + NuRandFloat() * 6.284f) * 10430.378f)] * amp;
            wave.y = 0.0f;
            wave.z = NuTrigTable[(u16)((freq * (NuRandFloat() * 0.4f + 0.7f) + NuRandFloat() * 6.284f) * 10430.378f)] * amp;
            NuVecMtxTransformH(&pos, &wave, &mtx);
            *((float *)stream->cur.w)++ = pos.x;
            *((float *)stream->cur.w)++ = pos.y;
            *((float *)stream->cur.w)++ = 1.0f;
            *((float *)stream->cur.w)++ = wave.x;
        }
        stream->cur.w = vpDmaTag_Close(stream->cur.w);
        rndrstream_free = vpDmaTag_Next(stream->cur.w, 0);
    }
}


void NuWaterRender(struct nugscn_s *scene) {
    int i;
    struct nuinstance_s *inst;

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
            if (inst->flagword & 4) {
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
    struct numtl_s *mtl;
    float unset;

    unset = 4294967296.0f;
    for (i = 0; i < nwatermtl; i++) {
        mtl = watermtl[i];
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
    water_speed = speed * 0.1f;
}


void NuWaterLoad(struct nugscn_s *scene) {
    nwaterinst = 0;
    if (scene) {
        initWater(scene);
        fixMaterials();
    }
}
