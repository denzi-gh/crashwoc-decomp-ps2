/*
 * Unit: game/glass
 *
 * Functions:
 *   0x00238bf8 InitGlass
 *   0x00239190 GlassGivenMaterial
 *   0x00239540 ProcessGlass
 *   0x002396b8 DoGlass
 *   0x002398f0 InitSpecular
 *   0x00239960 InitEnviro
 *   0x002399d0 SpecularGivenMaterial
 *   0x00239b38 EnviroGivenMaterial
 *   0x00239ca8 InitFlickerPal
 *   0x00239cb0 FlickerPalGivenMaterial
 *   0x00239cb8 DrawGlassCreatures
 */

#include "nu3d/numtl.h"
#include "nu3d/nustream.h"
#include "nu3d/nutex.h"
#include "numath/numtx.h"
#include "numath/nutrig.h"
#include "nu3d/nucamera.h"
#include "game/creature.h"

f32 glass_mix = 0.0f;
f32 glass_col_mix = 0.0f;
f32 glass_col_mix_speed = 0.02f;
f32 glass_mix_speed = 0.02f;
f32 glass_strength = 0.0625f;
s32 glass_enabled = 0;
s32 glass_col_enabled = 0;
static s32 spectid = 0;
static s32 envtid = 0;
f32 WATERBOSSGLASSMIX = 0.6f;
f32 ANGELGLASSMIX = 0.5f;
f32 MAXGLASSMIX = 1.0f;
struct numtl_s *glass_mtl = 0;
struct numtl_s *glass_spec_mtl = 0;
struct numtl_s *glass_copy_mtl = 0;
extern s32 glass_draw;
extern s32 glass_phase;
extern s32 GLASSPLAYER;
extern f32 plr_invisibility_time;
extern s32 Level;
extern s32 PS2_REZ_W;
extern s32 PS2_REZ_H;
extern struct nucamera_s global_camera;
extern struct numtl_s *nurndr_forced_mtl;

extern s32 D_00633358;
#define glass_tid D_00633358
extern f32 D_0063335C;
#define glass_uscale D_0063335C
extern f32 D_00633360;
#define glass_vscale D_00633360
extern u8 D_00291A40[];
#define glass_fxcode D_00291A40

struct nucolour3_s glass_col[2] = {
    { 1.0f, 1.0f, 1.0f },
    { 0.5f, 0.5f, 0.5f },
};

extern void LoadScreen(char *name);
extern void NuMemFreeFn(void *ptr, char *file, s32 line);
extern s32 NuRndrBeginScene(s32 tid);
extern void NuRndrEndScene(void);
extern void NuRndrEndSceneEx(s32 flags);
extern void NuRndrRectUV2di(s32 x, s32 y, s32 w, s32 h, f32 u0, f32 v0, f32 u1, f32 v1,
                            s32 colour, struct numtl_s *mtl);


inline void InitSpecular(void)
{
    struct nutex_s *tex;

    spectid = 0;
    tex = NuTexReadBitmap("stuff\\spectxtf.bmp");
    if (tex != 0) {
        spectid = NuTexCreate(tex);
        if (tex->bits != 0) {
            NuMemFreeFn(tex->bits, ".\\glass.c", 0x94);
        }
        NuMemFreeFn(tex, ".\\glass.c", 0x95);
    }
}


inline void InitEnviro(void)
{
    struct nutex_s *tex;

    envtid = 0;
    tex = NuTexReadBitmap("stuff\\envtxt.bmp");
    if (tex != 0) {
        envtid = NuTexCreate(tex);
        if (tex->bits != 0) {
            NuMemFreeFn(tex->bits, ".\\glass.c", 0xA2);
        }
        NuMemFreeFn(tex, ".\\glass.c", 0xA3);
    }
}


inline void SpecularGivenMaterial(struct numtl_s *mtl)
{
    struct numtl_s *spec;

    spec = NuMtlCreate(1);
    spec->tid = spectid;
    spec->diffuse.r = 1.0f;
    spec->diffuse.g = 1.0f;
    spec->diffuse.b = 1.0f;
    spec->alpha = 1.0f;
    spec->alpha_sort = 0x7FFF;
    spec->attrib.utc = 1;
    spec->attrib.vtc = 1;
    spec->attrib.lighting = 1;
    spec->attrib.alpha = 2;
    spec->attrib.zmode = 1;
    spec->attrib.fx = 0;
    NuMtlUpdate(spec);
    while (mtl->specmtl != 0) {
        mtl = mtl->specmtl;
    }
    mtl->specmtl = spec;
}


inline void EnviroGivenMaterial(struct numtl_s *mtl)
{
    struct numtl_s *env;

    env = NuMtlCreate(1);
    env->tid = envtid;
    env->diffuse.r = 0.7f;
    env->diffuse.g = 0.7f;
    env->diffuse.b = 0.7f;
    env->alpha = 0.3f;
    env->alpha_sort = 0x7FF0;
    env->attrib.utc = 1;
    env->attrib.vtc = 1;
    env->attrib.lighting = 2;
    env->attrib.alpha = 2;
    env->attrib.zmode = 1;
    env->attrib.fx = 1;
    NuMtlUpdate(env);
    while (mtl->specmtl != 0) {
        mtl = mtl->specmtl;
    }
    mtl->specmtl = env;
}


inline void InitFlickerPal(void)
{
}


inline void FlickerPalGivenMaterial(struct numtl_s *mtl)
{
}


void InitGlass(void)
{
    struct nutex_s tex;

    glass_mix = 0.0f;
    glass_enabled = 0;
    glass_col_mix = 0.0f;
    glass_col_enabled = 0;

    glass_spec_mtl = NuMtlCreate(1);
    glass_spec_mtl->tid = spectid;
    glass_spec_mtl->diffuse.r = 1.0f;
    glass_spec_mtl->attrib.utc = 1;
    glass_spec_mtl->attrib.vtc = 1;
    glass_spec_mtl->attrib.lighting = 1;
    glass_spec_mtl->attrib.alpha = 2;
    glass_spec_mtl->attrib.zmode = 1;
    glass_spec_mtl->attrib.fx = 0;
    glass_spec_mtl->alpha_sort = 0x7FFF;
    glass_spec_mtl->diffuse.g = 1.0f;
    glass_spec_mtl->diffuse.b = 1.0f;
    glass_spec_mtl->alpha = 1.0f;
    NuMtlUpdate(glass_spec_mtl);

    LoadScreen("Initglass 1");
    tex.type = 0xF;
    tex.width = PS2_REZ_W;
    tex.height = PS2_REZ_H;
    glass_tid = NuTexCreate(&tex);
    glass_uscale = (PS2_REZ_W - 1.0f) / NuPower2(PS2_REZ_W);
    glass_vscale = (PS2_REZ_H - 1.0f) / NuPower2(PS2_REZ_H);

    glass_mtl = NuMtlCreate(1);
    glass_mtl->tid = glass_tid;
    glass_mtl->attrib.utc = 1;
    glass_mtl->attrib.vtc = 1;
    glass_mtl->attrib.alpha = 0;
    glass_mtl->attrib.atst = 1;
    glass_mtl->attrib.fx = 1;
    glass_mtl->attrib.zmode = 0;
    glass_mtl->alpha_sort = 0x7FFE;
    glass_mtl->attrib.lighting = 0;
    glass_mtl->attrib.colour = 1;
    glass_mtl->diffuse.r = 1.0f;
    glass_mtl->diffuse.g = 1.0f;
    glass_mtl->diffuse.b = 1.0f;
    glass_mtl->alpha = 0.5f;
    NuMtlUpdate(glass_mtl);
    glass_mtl->stream.mpgtag = glass_fxcode;
    glass_mtl->effect.param[0] =
        ((PS2_REZ_W * 0.5f) * 16.0 - 32768.0) / (NuPower2(PS2_REZ_W) * 16.0);
    glass_mtl->effect.param[1] =
        ((PS2_REZ_H * 0.5f) * 16.0 - 32768.0) / (NuPower2(PS2_REZ_H) * 16.0);
    glass_mtl->effect.param[2] = glass_strength;
    glass_mtl->effect.param[3] = 0.0f;
    glass_mtl->effect.param[4] = 1.0 / (NuPower2(PS2_REZ_W) * 16.0);
    glass_mtl->effect.param[5] = 1.0 / (NuPower2(PS2_REZ_H) * 16.0);
    glass_mtl->effect.param[6] = (PS2_REZ_W - 1.0f) / NuPower2(PS2_REZ_W);
    glass_mtl->effect.param[7] = (PS2_REZ_H - 1.0f) / NuPower2(PS2_REZ_H);
    glass_mtl->specmtl = glass_spec_mtl;

    LoadScreen("Initglass 2");
    glass_copy_mtl = NuMtlCreate(1);
    glass_copy_mtl->attrib.cull = 2;
    glass_copy_mtl->attrib.zmode = 3;
    glass_copy_mtl->attrib.filter = 0;
    glass_copy_mtl->attrib.lighting = 2;
    glass_copy_mtl->attrib.colour = 1;
    glass_copy_mtl->attrib.alpha = 0;
    glass_copy_mtl->diffuse.r = 1.0f;
    glass_copy_mtl->diffuse.g = 1.0f;
    glass_copy_mtl->diffuse.b = 1.0f;
    glass_copy_mtl->alpha = 0.999f;
    glass_copy_mtl->tid = -1;
    NuMtlUpdate(glass_copy_mtl);
    LoadScreen("Initglass 3");
}


void GlassGivenMaterial(struct numtl_s *mtl)
{
    mtl->tid = -1;
    mtl->alpha = 0.5f;
    mtl->attrib.utc = 1;
    mtl->attrib.vtc = 1;
    mtl->ambient = mtl->diffuse;
    mtl->attrib.alpha = 0;
    mtl->attrib.atst = 1;
    mtl->attrib.fx = 1;
    mtl->attrib.zmode = 0;
    mtl->attrib.lighting = 0;
    mtl->attrib.colour = 0;
    mtl->vanmode = 0;
    mtl->uanmode = 0;
    mtl->alpha_sort = 0x7FEF;
    NuMtlUpdate(mtl);
    mtl->stream.mpgtag = glass_fxcode;
    mtl->effect.param[0] =
        ((PS2_REZ_W * 0.5f) * 16.0 - 32768.0) / (NuPower2(PS2_REZ_W) * 16.0);
    mtl->effect.param[1] =
        ((PS2_REZ_H * 0.5f) * 16.0 - 32768.0) / (NuPower2(PS2_REZ_H) * 16.0);
    mtl->effect.param[2] = glass_strength;
    mtl->effect.param[3] = 0.0f;
    mtl->effect.param[4] = 1.0 / (NuPower2(PS2_REZ_W) * 16.0);
    mtl->effect.param[5] = 1.0 / (NuPower2(PS2_REZ_H) * 16.0);
    mtl->effect.param[6] = (PS2_REZ_W - 1.0f) / NuPower2(PS2_REZ_W);
    mtl->effect.param[7] = (PS2_REZ_H - 1.0f) / NuPower2(PS2_REZ_H);
}


void ProcessGlass(s32 paused)
{
    f32 target;

    if (!paused) {
        if (Level == 0x17) {
            target = WATERBOSSGLASSMIX;
        } else if (player->used && player->obj.dead == 3) {
            target = ANGELGLASSMIX;
        } else {
            target = 0.0f;
            if (glass_enabled) {
                target = MAXGLASSMIX;
            }
        }
        if (glass_mix > target) {
            glass_mix -= glass_mix_speed;
            if (glass_mix < target) {
                glass_mix = target;
            }
        } else if (glass_mix < target) {
            glass_mix += glass_mix_speed;
            if (glass_mix > target) {
                glass_mix = target;
            }
        }
    }
    if (glass_mix != 1.0f) {
        glass_mtl->alpha = glass_mix;
        glass_mtl->attrib.alpha = 1;
    } else {
        glass_mtl->attrib.alpha = 0;
    }
    glass_spec_mtl->alpha = glass_mix;
    glass_mtl->diffuse = glass_col[0];
    NuMtlUpdateEx(glass_spec_mtl, 0);
    NuMtlUpdateEx(glass_mtl, 2);
    glass_mtl->stream.mpgtag = glass_fxcode;
}


inline void DrawGlassCreatures(s32 draw)
{
    if (Level != 0x17) {
        if (GLASSPLAYER && plr_invisibility_time < 5.0f) {
            glass_draw = draw;
            glass_phase = 1;
            DrawCreatures(Character, 1, 1, 0);
        }
    } else {
        glass_draw = draw;
        glass_phase = 1;
        DrawCreatures(&Character[1], 8, 1, 1);
    }
}


void DoGlass(s32 editor)
{
    struct numtx_s m;
    struct nuvec_s scale = {0.5f, 0.5f, 0.5f};

    NuTexAssignAddr(glass_tid, NuTexReserve(NuTexGetReqSize(glass_tid, 0)));
    if (NuRndrBeginScene(glass_tid)) {
        NuRndrRectUV2di(0, 0, PS2_REZ_W << 4, PS2_REZ_H << 4, 0.0f, 0.0f,
                        glass_uscale, glass_vscale, 0x80808080, glass_copy_mtl);
        NuRndrEndSceneEx(0);
    }
    NuMtxInvR(&m, &global_camera.mtx);
    NuMtxScale(&m, &scale);
    m._32 = 0.0f;
    m._30 = 0.5f;
    m._31 = 0.5f;
    m._33 = 1.0f;
    m._23 = m._32;
    m._13 = m._32;
    m._03 = m._32;
    NuRndrSetFxMtx(&m);
    if (NuRndrBeginScene(-1)) {
        if (glass_mix != 1.0f) {
            DrawGlassCreatures(1);
        }
        nurndr_forced_mtl = glass_mtl;
        DrawGlassCreatures(0);
        nurndr_forced_mtl = 0;
        NuRndrEndScene();
    }
    NuTexUnReserve();
}
