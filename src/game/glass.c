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

#include "creature.h"

struct numtl_s {
    u8 pad000[0x13C];
    void *fxcode;
    u8 pad140[0x1C];
    f32 *fxparam;
    u8 pad160[0x8];
    struct numtl_s *specmtl;
    u64 alphamode : 4;
    u64 filter : 2;
    u64 fx : 2;
    u64 utc : 2;
    u64 vtc : 2;
    u64 cull : 2;
    u64 zmode : 2;
    u64 lighting : 2;
    u64 colour : 1;
    u64 uvmode : 1;
    u64 atst : 3;
    u64 aref : 9;
    u32 attrib2;
    struct nucolour3_s ambient;
    struct nucolour3_s diffuse;
    u8 pad18C[0x14];
    f32 alpha;
    s32 tid;
    s32 alpha_sort;
    u64 pad1AC : 24;
    u64 fxlayer : 4;
    u64 fxmode : 4;
};

struct nutex_s {
    s32 type;
    s32 width;
    s32 height;
    s32 mmcnt;
    void *bits;
    void *pal;
    s32 decal;
    s32 linear;
};

struct camera_s {
    struct numtx_s mtx;
};

extern f32 glass_mix;
extern f32 glass_col_mix;
extern f32 glass_mix_speed;
extern f32 glass_strength;
extern s32 glass_enabled;
extern s32 glass_col_enabled;
extern f32 WATERBOSSGLASSMIX;
extern f32 ANGELGLASSMIX;
extern f32 MAXGLASSMIX;
extern struct numtl_s *glass_mtl;
extern struct numtl_s *glass_spec_mtl;
extern struct numtl_s *glass_copy_mtl;
extern struct nucolour3_s glass_col;
extern s32 glass_draw;
extern s32 glass_phase;
extern s32 GLASSPLAYER;
extern f32 plr_invisibility_time;
extern s32 Level;
extern s32 PS2_REZ_W;
extern s32 PS2_REZ_H;
extern struct camera_s global_camera;
extern struct numtl_s *nurndr_forced_mtl;

extern s32 D_00631E84;
#define spec_tid D_00631E84
extern s32 D_00631E88;
#define enviro_tid D_00631E88
extern s32 D_00633358;
#define glass_tid D_00633358
extern f32 D_0063335C;
#define glass_uscale D_0063335C
extern f32 D_00633360;
#define glass_vscale D_00633360
extern u8 D_00291A40[];
#define glass_fxcode D_00291A40

extern char D_00620728[];
#define spectxt_path D_00620728
extern char D_00620740[];
#define glass_file D_00620740
extern char D_00620750[];
#define envtxt_path D_00620750
extern char D_00620768[];
#define initglass1_msg D_00620768
extern char D_00620778[];
#define initglass2_msg D_00620778
extern char D_00620788[];
#define initglass3_msg D_00620788

extern struct numtl_s *NuMtlCreate(s32 count);
extern void NuMtlUpdate(struct numtl_s *mtl);
extern s32 NuMtlUpdateEx(struct numtl_s *mtl, s32 force);
extern void LoadScreen(char *name);
extern s32 NuTexCreate(struct nutex_s *tex);
extern struct nutex_s *NuTexReadBitmap(char *name);
extern s32 NuPower2(s32 value);
extern void NuMemFreeFn(void *ptr, char *file, s32 line);
extern s32 NuTexGetReqSize(s32 tid, s32 level);
extern s32 NuTexReserve(s32 size);
extern void NuTexAssignAddr(s32 tid, s32 addr);
extern void NuTexUnReserve(void);
extern s32 NuRndrBeginScene(s32 tid);
extern void NuRndrEndScene(void);
extern void NuRndrEndSceneEx(s32 flags);
extern void NuRndrRectUV2di(s32 x, s32 y, s32 w, s32 h, f32 u0, f32 v0, f32 u1, f32 v1,
                            s32 colour, struct numtl_s *mtl);
extern void NuMtxInvR(struct numtx_s *dst, struct numtx_s *src);
extern void NuMtxScale(struct numtx_s *m, struct nuvec_s *s);
extern s32 NuRndrSetFxMtx(struct numtx_s *m);


void InitGlass(void)
{
    struct nutex_s tex;

    glass_mix = 0.0f;
    glass_enabled = 0;
    glass_col_mix = 0.0f;
    glass_col_enabled = 0;

    glass_spec_mtl = NuMtlCreate(1);
    glass_spec_mtl->tid = spec_tid;
    glass_spec_mtl->diffuse.r = 1.0f;
    glass_spec_mtl->utc = 1;
    glass_spec_mtl->vtc = 1;
    glass_spec_mtl->lighting = 1;
    glass_spec_mtl->alphamode = 2;
    glass_spec_mtl->zmode = 1;
    glass_spec_mtl->fx = 0;
    glass_spec_mtl->alpha_sort = 0x7FFF;
    glass_spec_mtl->diffuse.g = 1.0f;
    glass_spec_mtl->diffuse.b = 1.0f;
    glass_spec_mtl->alpha = 1.0f;
    NuMtlUpdate(glass_spec_mtl);

    LoadScreen(initglass1_msg);
    tex.type = 0xF;
    tex.width = PS2_REZ_W;
    tex.height = PS2_REZ_H;
    glass_tid = NuTexCreate(&tex);
    glass_uscale = (PS2_REZ_W - 1.0f) / NuPower2(PS2_REZ_W);
    glass_vscale = (PS2_REZ_H - 1.0f) / NuPower2(PS2_REZ_H);

    glass_mtl = NuMtlCreate(1);
    glass_mtl->tid = glass_tid;
    glass_mtl->utc = 1;
    glass_mtl->vtc = 1;
    glass_mtl->alphamode = 0;
    glass_mtl->atst = 1;
    glass_mtl->fx = 1;
    glass_mtl->zmode = 0;
    glass_mtl->alpha_sort = 0x7FFE;
    glass_mtl->lighting = 0;
    glass_mtl->colour = 1;
    glass_mtl->diffuse.r = 1.0f;
    glass_mtl->diffuse.g = 1.0f;
    glass_mtl->diffuse.b = 1.0f;
    glass_mtl->alpha = 0.5f;
    NuMtlUpdate(glass_mtl);
    glass_mtl->fxcode = glass_fxcode;
    glass_mtl->fxparam[0] =
        ((PS2_REZ_W * 0.5f) * 16.0 - 32768.0) / (NuPower2(PS2_REZ_W) * 16.0);
    glass_mtl->fxparam[1] =
        ((PS2_REZ_H * 0.5f) * 16.0 - 32768.0) / (NuPower2(PS2_REZ_H) * 16.0);
    glass_mtl->fxparam[2] = glass_strength;
    glass_mtl->fxparam[3] = 0.0f;
    glass_mtl->fxparam[4] = 1.0 / (NuPower2(PS2_REZ_W) * 16.0);
    glass_mtl->fxparam[5] = 1.0 / (NuPower2(PS2_REZ_H) * 16.0);
    glass_mtl->fxparam[6] = (PS2_REZ_W - 1.0f) / NuPower2(PS2_REZ_W);
    glass_mtl->fxparam[7] = (PS2_REZ_H - 1.0f) / NuPower2(PS2_REZ_H);
    glass_mtl->specmtl = glass_spec_mtl;

    LoadScreen(initglass2_msg);
    glass_copy_mtl = NuMtlCreate(1);
    glass_copy_mtl->cull = 2;
    glass_copy_mtl->zmode = 3;
    glass_copy_mtl->filter = 0;
    glass_copy_mtl->lighting = 2;
    glass_copy_mtl->colour = 1;
    glass_copy_mtl->alphamode = 0;
    glass_copy_mtl->diffuse.r = 1.0f;
    glass_copy_mtl->diffuse.g = 1.0f;
    glass_copy_mtl->diffuse.b = 1.0f;
    glass_copy_mtl->alpha = 0.999f;
    glass_copy_mtl->tid = -1;
    NuMtlUpdate(glass_copy_mtl);
    LoadScreen(initglass3_msg);
}


void GlassGivenMaterial(struct numtl_s *mtl)
{
    mtl->tid = -1;
    mtl->alpha = 0.5f;
    mtl->utc = 1;
    mtl->vtc = 1;
    mtl->ambient = mtl->diffuse;
    mtl->alphamode = 0;
    mtl->atst = 1;
    mtl->fx = 1;
    mtl->zmode = 0;
    mtl->lighting = 0;
    mtl->colour = 0;
    mtl->fxmode = 0;
    mtl->fxlayer = 0;
    mtl->alpha_sort = 0x7FEF;
    NuMtlUpdate(mtl);
    mtl->fxcode = glass_fxcode;
    mtl->fxparam[0] =
        ((PS2_REZ_W * 0.5f) * 16.0 - 32768.0) / (NuPower2(PS2_REZ_W) * 16.0);
    mtl->fxparam[1] =
        ((PS2_REZ_H * 0.5f) * 16.0 - 32768.0) / (NuPower2(PS2_REZ_H) * 16.0);
    mtl->fxparam[2] = glass_strength;
    mtl->fxparam[3] = 0.0f;
    mtl->fxparam[4] = 1.0 / (NuPower2(PS2_REZ_W) * 16.0);
    mtl->fxparam[5] = 1.0 / (NuPower2(PS2_REZ_H) * 16.0);
    mtl->fxparam[6] = (PS2_REZ_W - 1.0f) / NuPower2(PS2_REZ_W);
    mtl->fxparam[7] = (PS2_REZ_H - 1.0f) / NuPower2(PS2_REZ_H);
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
        glass_mtl->alphamode = 1;
    } else {
        glass_mtl->alphamode = 0;
    }
    glass_spec_mtl->alpha = glass_mix;
    glass_mtl->diffuse = glass_col;
    NuMtlUpdateEx(glass_spec_mtl, 0);
    NuMtlUpdateEx(glass_mtl, 2);
    glass_mtl->fxcode = glass_fxcode;
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


void InitSpecular(void)
{
    struct nutex_s *tex;

    spec_tid = 0;
    tex = NuTexReadBitmap(spectxt_path);
    if (tex != 0) {
        spec_tid = NuTexCreate(tex);
        if (tex->bits != 0) {
            NuMemFreeFn(tex->bits, glass_file, 0x94);
        }
        NuMemFreeFn(tex, glass_file, 0x95);
    }
}


void InitEnviro(void)
{
    struct nutex_s *tex;

    enviro_tid = 0;
    tex = NuTexReadBitmap(envtxt_path);
    if (tex != 0) {
        enviro_tid = NuTexCreate(tex);
        if (tex->bits != 0) {
            NuMemFreeFn(tex->bits, glass_file, 0xA2);
        }
        NuMemFreeFn(tex, glass_file, 0xA3);
    }
}


void SpecularGivenMaterial(struct numtl_s *mtl)
{
    struct numtl_s *spec;

    spec = NuMtlCreate(1);
    spec->tid = spec_tid;
    spec->diffuse.r = 1.0f;
    spec->diffuse.g = 1.0f;
    spec->diffuse.b = 1.0f;
    spec->alpha = 1.0f;
    spec->alpha_sort = 0x7FFF;
    spec->utc = 1;
    spec->vtc = 1;
    spec->lighting = 1;
    spec->alphamode = 2;
    spec->zmode = 1;
    spec->fx = 0;
    NuMtlUpdate(spec);
    while (mtl->specmtl != 0) {
        mtl = mtl->specmtl;
    }
    mtl->specmtl = spec;
}


void EnviroGivenMaterial(struct numtl_s *mtl)
{
    struct numtl_s *env;

    env = NuMtlCreate(1);
    env->tid = enviro_tid;
    env->diffuse.r = 0.7f;
    env->diffuse.g = 0.7f;
    env->diffuse.b = 0.7f;
    env->alpha = 0.3f;
    env->alpha_sort = 0x7FF0;
    env->utc = 1;
    env->vtc = 1;
    env->lighting = 2;
    env->alphamode = 2;
    env->zmode = 1;
    env->fx = 1;
    NuMtlUpdate(env);
    while (mtl->specmtl != 0) {
        mtl = mtl->specmtl;
    }
    mtl->specmtl = env;
}


void InitFlickerPal(void)
{
}


void FlickerPalGivenMaterial(struct numtl_s *mtl)
{
}
