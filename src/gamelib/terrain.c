/*
 * Unit: gamelib/terrain
 *
 * Functions:
 *   0x0016fde8 ReadTerrain
 *   0x00170190 terraininit
 *   0x00170ca8 ReadTerrainPickup
 *   0x00170ec0 terrainpickupinit
 *   0x00171220 TerrainPlatformOldUpdate
 *   0x00171310 TerrainPlatformNewUpdate
 *   0x00171630 TerrDraw
 *   0x001719e8 TerrDrawSitu
 *   0x00171d28 TerrDrawPlat
 *   0x00172298 SphereDraw
 *   0x001724f8 RotateVec
 *   0x00172608 RotateTerrain
 *   0x00172808 DeRotateTerrain
 *   0x00172bb8 DeRotatePoint
 *   0x00172cc0 InsidePolLines
 *   0x00173308 NewScan
 *   0x00173d80 NewScanRot
 *   0x00174e60 NewCast
 *   0x00175b10 CheckCylinder
 *   0x00176270 CheckSphere
 *   0x00176428 CheckSphereTer
 *   0x001765c8 HitPoly
 *   0x00176b48 HitWallSpline
 *   0x00177330 HitTerrain
 *   0x00177780 HitTerrPoly
 *   0x00177a38 ScanTerrain
 *   0x001796c0 TerrainImpactNorm
 *   0x00179900 TerrainImpact
 *   0x00179f50 TerrainImpactPlatform
 *   0x0017a180 TerrainPlatformMoveCheck
 *   0x0017a4e0 TerrainPlatformEmbedded
 *   0x0017b450 StorePlatImpact
 *   0x0017b6b0 PlatformChecks
 *   0x0017c2c8 NewTerrain
 *   0x0017c6e0 TerrainSideClamp
 *   0x0017c7d8 TerrShapeSideStep
 *   0x0017cbe8 CubeImpact
 *   0x0017cec8 NewTerrainScaleY
 *   0x0017d448 RayImpact
 *   0x0017d5e8 NewRayCast
 *   0x0017d7f0 NewRayCastMask
 *   0x0017da00 NewRayCastSet
 *   0x0017dc20 NewRayCastSetMask
 *   0x0017de50 NewPlatInst
 *   0x0017e0a8 DeletePlatinst
 *   0x0017e230 NewScanHandelFull
 *   0x0017f288 NewScanHandelSubset
 *   0x0017fbf8 ScanTerrainHandel
 *   0x00180630 NewRayCastSetHandel
 *   0x00180858 TerrainTrackBack
 *   0x00180af0 ScanTerrainPlatform
 *   0x00181888 NewRayCastPlatForm
 *   0x00181ab0 TerrShowCamTerr
 *   0x00181bb8 TerrainSetCur
 *   0x00181bc0 NewShadow
 *   0x00181c40 NewShadowMask
 *   0x00181cd0 NewShadowTol
 *   0x00181d58 NewShadowTolMask
 *   0x00181df0 NewShadowPlat
 *   0x00181e70 NewShadowMaskPlat
 *   0x00181f00 NewShadowMaskPlatRot
 *   0x00181f88 NewShadowTolPlat
 *   0x00182010 NewShadowTolMaskPlat
 *   0x001820a8 AddCollisionSphere
 *   0x001820f8 AddPickupTerr
 *   0x00182200 NewTerrAxisFreedom
 *   0x00182280 NewShapeInit
 *   0x00182290 CrashDataPtr
 *   0x00182298 noterraininit
 *   0x001822b8 TerrainInfo
 *   0x001822d8 ShadowInfo
 *   0x001822f8 ShadowRoofInfo
 *   0x00182318 EShadowRoofInfo
 *   0x00182338 ShadowDir
 *   0x00182370 EShadowInfo
 *   0x00182390 FindPlatInst
 *   0x00182410 UpdatePlatinst
 *   0x00182438 PlatInstRotate
 *   0x00182468 PlatInstBounce
 *   0x001824c8 PlatInstGetHit
 *   0x001824f8 PlatImpactInfo
 *   0x00182548 PlatformCrush
 *   0x00182550 TerrainPlatGetMtx
 *   0x00182588 TerrainPlatId
 *   0x00182590 NewScanInit
 *   0x001825a8 NewScanHandel
 *   0x00182610 NewRaySetDisablePalt
 *   0x00182618 DrawPlatform
 *   0x001826e8 TerrFlush
 *   0x001826f8 DrawWallSpline
 *   0x001827a8 DerotateMovementVector
 *   0x00182858 InsideLineF
 *   0x00182898 ScanTerrId
 *   0x001828d8 AllocTerrId
 *   0x00182918 FullDeflect
 *   0x00182990 FullDeflectTest
 *   0x00182a80 FullReflect
 *   0x00182b00 TerrainMoveImpactData
 *   0x00182c08 PlatformConnect
 *   0x00182ca0 OffPlat
 */

/* Retail called memset as a plain function here (terrainpickupinit). */
#define NU_LIBC_MEMSET_NOT_BUILTIN
#include "nu_libc.h"

#include "nucore/nuerror.h"
#include "numath/nutrig.h"
#include "numath/nuvec.h"
#include "nu3d/nucamera.h"
#include "nu3d/nugscn.h"
#include "gamelib/terrain.h"
#include "nups2/ps2ctl.h"

/* Terrain collision info structures (offsets verified against retail asm). */
typedef struct {
    struct nuvec_s offset;    /* 0x00 */
    float ang;                /* 0x0C */
    float size;               /* 0x10 */
} TerrShapeType;

typedef struct {
    struct nuvec_s origpos;   /* 0x00 */
    struct nuvec_s origvel;   /* 0x0C */
    struct nuvec_s curpos;    /* 0x18 */
    struct nuvec_s curvel;    /* 0x24 */
    short id;                 /* 0x30 */
    short scanmode;           /* 0x32 */
    float stopflag;           /* 0x34 */
    float vellen;             /* 0x38 */
    unsigned char *flags;     /* 0x3C */
    float ax;                 /* 0x40 */
    float ay;                 /* 0x44 */
    float len;                /* 0x48 */
    float size;               /* 0x4C */
    float sizesq;             /* 0x50 */
    float sizediv;            /* 0x54 */
    float yscale;             /* 0x58 */
    float yscalesq;           /* 0x5C */
    float inyscale;           /* 0x60 */
    float inyscalesq;         /* 0x64 */
    short hitcnt;             /* 0x68 */
    short hitterrno;          /* 0x6A */
    float csx;                /* 0x6C */
    float csy;                /* 0x70 */
    float csz;                /* 0x74 */
    float cex;                /* 0x78 */
    float cey;                /* 0x7C */
    float cez;                /* 0x80 */
    short hittype;            /* 0x84 */
    short plathit;            /* 0x86 */
    short *PlatScanStart;     /* 0x88 */
    tertype *hitter;          /* 0x8C */
    float hittime;            /* 0x90 */
    float timeadj;            /* 0x94 */
    float impactadj;          /* 0x98 */
    struct nuvec_s hitnorm;   /* 0x9C */
    struct nuvec_s uhitnorm;  /* 0xA8 */
    struct nuvec_s tempvec[2];/* 0xB4 */
    tertype rotter;           /* 0xCC */
    tertype *hitdata[512];    /* 0x130 */
} TerTempInfoType;

typedef struct {
    TerrainSituType *terr;                /* 0x0000 */
    void *data;                           /* 0x0004 */
    s16 typeterr[128];                    /* 0x0008 */
    TerrainPlatformType platdata[128];    /* 0x0108 */
    void *wallinfo;                       /* 0x3308 */
} PickupTerrType;

typedef struct {
    struct nuvec_s pos;                   /* 0x00 */
    f32 radius;                           /* 0x0C */
} SphereType;

/* A scan list holds a (count, terrain id) pair of shorts in a pointer slot, then count tertype pointers; a
 * handle list then holds wall-spline point pointers up to a null slot. SCAN_SLOT is a slot in shorts. */
#define SCAN_SLOT ((s32)(sizeof(tertype *) / sizeof(short)))

struct nuvec_s ShadNorm = { 0.0f, 0.0f, 0.0f };
struct nuvec_s EShadNorm = { 0.0f, 0.0f, 0.0f };
struct nuvec_s ShadRoofNorm = { 0.0f, 0.0f, 0.0f };
struct nuvec_s EShadRoofNorm = { 0.0f, 0.0f, 0.0f };
SphereType SphereData[16] = {0};
struct numtx_s tertempmat = {0};
struct nuvec4_s tertempvec4 = { 0.0f, 0.0f, 0.0f, 0.0f };
tertype TerrPolyInfo = {0};
tertype ScaleTerrainT1[512] = {0};
tertype ScaleTerrainT2[512] = {0};
struct nuvec_s WallSplList[128] = {0};
struct nuvec_s PlatImpactNorm = { 0.0f, 0.0f, 0.0f };
tertype PlatImpactTer = {0};
s32 TempScanStack[2048 * sizeof(tertype *) / sizeof(s32)] = {0};
s32 TerrPlatDis = -1;

extern f32 NuTrigTable[];
extern void *crashdata;
#ifdef NW_HOST /* retail's limits lie in debkeydata, past TempScanStack, so no scan stops at them: here they end it */
#define ScanHitLimit ((tertype **)TempScanStack + 0x800 - 100)
#define ScanHitLimitEnd ((tertype **)TempScanStack + 0x800)
#else
extern tertype *D_0034E7E0[];
#define ScanHitLimit D_0034E7E0
extern tertype *D_0034E970[];
#define ScanHitLimitEnd D_0034E970
#endif

extern f32 NuFsqrt(f32 x);
extern void *NuScratchAlloc32(s32 size);
extern void NuScratchRelease(void);
extern void NuVec4MtxTransformVU0(struct nuvec4_s *dst, struct nuvec4_s *src,
                                  struct numtx_s *m);
extern void NuVec4MtxInvTransformVU0(struct nuvec4_s *dst, struct nuvec4_s *src,
                                     struct numtx_s *m);
extern void NuRndrLine3dDbg(f32 x0, f32 y0, f32 z0, f32 x1, f32 y1, f32 z1, u32 colour);
extern struct nucamera_s global_camera;
extern s32 NuFileLoadBuffer(char *name, void *buf, s32 max);
extern s32 nufile_buffering_enabled;

#define DABS(v) ({ double d_ = (v); if (d_ < 0.0) { d_ = -d_; } d_; })

s32 ReadTerrain(char *name, s32 situ, short **store, TerrainSetType *terrset) {
    OffFileType *offfile;
    WallSplineType *wallspl;
    s32 lp;
    s32 count;
    s32 a;
    s32 b;
    f32 minx;
    f32 maxx;
    f32 minz;
    f32 maxz;
    char filename[100];

    NuDebugMsgProlog("..\\nu2crash.ps2\\gamelib\\terrain.c", 0x313)("store adv (%d)", store);
    strcpy(filename, name);
    strcat(filename, ".ter");
    if (NuFileLoadBuffer(filename, *store, 0x7FFFFFFF) == 0) {
        return -1;
    }
    offfile = (OffFileType *)(*store + *(s32 *)*store);
    *store += 2;
    count = 0;
    terrset->wallinfo = 0;
    crashdata = 0;
    for (lp = 0; lp < offfile->count; lp++) {
        switch (offfile->offlist[lp].type) {
            case -1:
                break;
            case 0:
            case 1:
                terrset->terr[situ].id = offfile->offlist[lp].id;
                terrset->terr[situ].type = offfile->offlist[lp].type;
                terrset->terr[situ].info = offfile->offlist[lp].info;
                terrset->terr[situ].Location.x = offfile->offlist[lp].translation.x;
                terrset->terr[situ].Location.y = offfile->offlist[lp].translation.y;
                terrset->terr[situ].Location.z = offfile->offlist[lp].translation.z;
                terrset->terr[situ].model = *store;
                terrset->terr[situ].flags = offfile->offlist[lp].flags;
                count++;
                situ++;
                break;
            case 2:
                if (*(s32 *)*store == 0x12345678) {
                    *(void **)*store = terrset->wallinfo;
                    terrset->wallinfo = *store + 2;
                    ((WallSplineType *)terrset->wallinfo)->count--;
                }
                break;
            case 3:
                crashdata = *store;
                break;
        }
        *store += offfile->offlist[lp].offset;
    }
    for (wallspl = terrset->wallinfo; wallspl != 0; wallspl = ((WallSplineType **)wallspl)[-1]) {
        for (a = 0; a < wallspl->count; a += 16) {
            if (wallspl->spl[a].y != 2147483647.0f) {
                if (a + 15 < wallspl->count) {
                    minx = maxx = wallspl->spl[a].x;
                    minz = maxz = wallspl->spl[a].z;
                    for (b = a + 1; b <= a + 16; b++) {
                        if (wallspl->spl[b].x < minx) {
                            minx = wallspl->spl[b].x;
                        }
                        if (wallspl->spl[b].x > maxx) {
                            maxx = wallspl->spl[b].x;
                        }
                        if (wallspl->spl[b].z < minz) {
                            minz = wallspl->spl[b].z;
                        }
                        if (wallspl->spl[b].z > maxz) {
                            maxz = wallspl->spl[b].z;
                        }
                    }
                    wallspl->spl[a].y = minx;
                    wallspl->spl[a + 1].y = maxx;
                    wallspl->spl[a + 2].y = minz;
                    wallspl->spl[a + 3].y = maxz;
                } else {
                    wallspl->spl[a].y = 2147483647.0f;
                }
            }
        }
    }
    return count;
}


f32 EShadY = 0;
s32 terrhitflags = 0;
f32 ShadRoofY = 0;
f32 EShadRoofY = 0;
TerrainSetType *CurTerr = 0;
PickupTerrType *PickupTerr = 0;
s32 curSphereter = 0;
s32 curPickInst = 0;
TerrainTrackInfoType *CurTrackInfo = 0;
TerTempInfoType *TerI = 0;
tertype *TerrPoly = 0;
s32 TerrPolyObj = 0;
tertype *ShadPoly = 0;
tertype *EShadPoly = 0;
tertype *ShadRoofPoly = 0;
tertype *EShadRoofPoly = 0;
s32 WallSplCount = 0;
tertype *ScaleTerrain = 0;
TerrShapeType *TerrShape = 0;
s32 TerrShapeAdjCnt = 0;
s32 PlatImpactId = 0;
s32 PlatCrush = 0;
s32 plathitid = 0;
short castnum = 0;
short ecastnum = 0;
short castroofnum = 0;
short ecastroofnum = 0;
short shadhit = 0;
short eshadhit = 0;
short shadroofhit = 0;
short eshadroofhit = 0;
s32 terraincnt = 0;
s32 situtotal = 0;
s32 platinrange = 0;
void *crashdata = 0;
s32 terlistptr = 0;
void *TempStackPtr = 0;
s32 testlock = 0;

short *terraininit(s32 levelnum, short **store, short *endstore, s32 opt, char *name,
                   struct nugscn_s *gscene, s32 gsid) {
    TerrainSituType *mbuf;
    tertype *ter;
    TerrainGroupType *group;
    s32 c;
    s32 b;
    s32 tx;
    s32 tz;
    f32 terminx;
    f32 termaxx;
    f32 terminy;
    f32 termaxy;
    f32 terminz;
    f32 termaxz;
    f32 maxradius;
    f32 radiussq;
    s32 xsteps;
    s32 zsteps;
    s32 lp;
    short id[2048];
    f32 minx[2048];
    f32 maxx[2048];
    f32 minz[2048];
    f32 maxz[2048];
    s32 platid;
    s32 index;
    s32 buffering;
    TerrainSetType *terrset;
    short *rv;

    rv = *store;
    terrset = (TerrainSetType *)*store;
    *store += sizeof(TerrainSetType) / sizeof(short);
    terrset->terr = (TerrainSituType *)*store;
    *store += 0x880 * sizeof(TerrainSituType) / sizeof(short);
    for (lp = 0; lp < 9; lp++) {
        terrset->pollist[lp].timer = 0;
        terrset->pollist[lp].hitdata[0] = 0;
    }
    memset(terrset->terr, 0, 0x800 * sizeof(TerrainSituType));
    c = 0;
    mbuf = terrset->terr;
    while (c < 0x800) {
        mbuf->type = -1;
        c++;
        mbuf++;
    }
    for (c = 0; c < 257; c++) {
        terrset->terrgroup[c].count = 0;
    }
    for (c = 0; c < 128; c++) {
        terrset->platdata[c].curmtx = 0;
        terrset->platdata[c].plrgrav = 0.0f;
        terrset->platdata[c].ypos = 0.0f;
        terrset->platdata[c].yvel = 0.0f;
        terrset->platdata[c].damp = 0.0f;
        terrset->platdata[c].tension = 0.0f;
    }
    for (c = 0; c < 4; c++) {
        terrset->TrackInfo[c].ptrid = 0;
    }
    buffering = nufile_buffering_enabled;
    nufile_buffering_enabled = 0;
    terraincnt = 0;
    curSphereter = 0;
    platinrange = 0;
    platid = 0;
    ShadPoly = 0;
    TerrPoly = 0;
    PlatImpactId = 0;
    situtotal = ReadTerrain(name, 0, store, terrset);
    if (situtotal < 0) {
        *store = rv;
        return 0;
    }
    terrset->miny = 0x7FFFFFFF;
    c = 0;
    mbuf = terrset->terr;
    while (c < 0x800) {
        if ((mbuf->type == 0) || (mbuf->type == 1)) {
            maxradius = 0.0f;
            terminx = 200000000.0f;
            terminy = 200000000.0f;
            terminz = 200000000.0f;
            termaxx = -200000000.0f;
            termaxy = -200000000.0f;
            termaxz = -200000000.0f;
            group = (TerrainGroupType *)mbuf->model;
            while (group->start >= 0) {
                b = group->count;
                ter = (tertype *)(group + 1);
                while (b > 0) {
                    if (ter->info[0] == 0xFF) {
                        ter->info[0] = ter->info[1];
                        ter->info[1] = 0;
                        ter->info[2] |= 0x80;
                    }
                    if (ter->minx < terminx) {
                        terminx = ter->minx;
                    }
                    if (ter->miny < terminy) {
                        terminy = ter->miny;
                    }
                    if (ter->minz < terminz) {
                        terminz = ter->minz;
                    }
                    if (ter->maxx > termaxx) {
                        termaxx = ter->maxx;
                    }
                    if (ter->maxy > termaxy) {
                        termaxy = ter->maxy;
                    }
                    if (ter->maxz > termaxz) {
                        termaxz = ter->maxz;
                    }
                    if (ter->pnts[0].y + mbuf->Location.y < (f32)terrset->miny) {
                        terrset->miny = ter->pnts[0].y + mbuf->Location.y;
                    }
                    if (ter->pnts[1].y + mbuf->Location.y < (f32)terrset->miny) {
                        terrset->miny = ter->pnts[1].y + mbuf->Location.y;
                    }
                    if (ter->pnts[2].y + mbuf->Location.y < (f32)terrset->miny) {
                        terrset->miny = ter->pnts[2].y + mbuf->Location.y;
                    }
                    if (ter->pnts[3].y + mbuf->Location.y < (f32)terrset->miny) {
                        terrset->miny = ter->pnts[3].y + mbuf->Location.y;
                    }
                    radiussq = ter->pnts[0].x * ter->pnts[0].x + ter->pnts[0].y * ter->pnts[0].y +
                         ter->pnts[0].z * ter->pnts[0].z;
                    if (maxradius < radiussq) {
                        maxradius = radiussq;
                    }
                    radiussq = ter->pnts[1].x * ter->pnts[1].x + ter->pnts[1].y * ter->pnts[1].y +
                         ter->pnts[1].z * ter->pnts[1].z;
                    if (maxradius < radiussq) {
                        maxradius = radiussq;
                    }
                    radiussq = ter->pnts[2].x * ter->pnts[2].x + ter->pnts[2].y * ter->pnts[2].y +
                         ter->pnts[2].z * ter->pnts[2].z;
                    if (maxradius < radiussq) {
                        maxradius = radiussq;
                    }
                    radiussq = ter->pnts[3].x * ter->pnts[3].x + ter->pnts[3].y * ter->pnts[3].y +
                         ter->pnts[3].z * ter->pnts[3].z;
                    if (maxradius < radiussq) {
                        maxradius = radiussq;
                    }
                    ter++;
                    b--;
                }
                group = (TerrainGroupType *)ter;
            }
            mbuf->radius = maxradius;
            if (mbuf->type == 1) {
                mbuf->min.x = terminx;
                mbuf->min.y = terminy;
                mbuf->min.z = terminz;
                mbuf->max.x = termaxx;
                mbuf->max.y = termaxy;
                mbuf->max.z = termaxz;
            } else {
                mbuf->min.x = terminx + mbuf->Location.x;
                mbuf->min.y = terminy + mbuf->Location.y;
                mbuf->min.z = terminz + mbuf->Location.z;
                mbuf->max.x = termaxx + mbuf->Location.x;
                mbuf->max.y = termaxy + mbuf->Location.y;
                mbuf->max.z = termaxz + mbuf->Location.z;
            }
            terraincnt++;
        }
        mbuf++;
        c++;
    }
    terminx = 200000000.0f;
    terminy = 200000000.0f;
    terminz = 200000000.0f;
    termaxx = -200000000.0f;
    termaxy = -200000000.0f;
    termaxz = -200000000.0f;
    c = 0;
    mbuf = &terrset->terr[c];
    while (c < 0x800) {
        if ((mbuf->type == 0) || (mbuf->type == 1)) {
            group = (TerrainGroupType *)mbuf->model;
            while (group->start >= 0) {
                b = group->count;
                if (group->bounds[0] + mbuf->Location.x < terminx) {
                    terminx = group->bounds[0] + mbuf->Location.x;
                }
                if (group->bounds[1] + mbuf->Location.x > termaxx) {
                    termaxx = group->bounds[1] + mbuf->Location.x;
                }
                if (group->bounds[2] + mbuf->Location.z < terminz) {
                    terminz = group->bounds[2] + mbuf->Location.z;
                }
                if (group->bounds[3] + mbuf->Location.z > termaxz) {
                    termaxz = group->bounds[3] + mbuf->Location.z;
                }
                ter = (tertype *)(group + 1);
                group = (TerrainGroupType *)&ter[b];
            }
        }
        mbuf++;
        c++;
    }
    terlistptr = 0;
    xsteps = 5;
    zsteps = 5;
    c = 0;
    mbuf = &terrset->terr[c];
    while (c < 0x800) {
        if (mbuf->type == 0) {
            group = (TerrainGroupType *)mbuf->model;
            minx[c] = 200000000.0f;
            minz[c] = 200000000.0f;
            maxx[c] = -200000000.0f;
            maxz[c] = -200000000.0f;
            while (group->start >= 0) {
                b = group->count;
                if (group->bounds[0] + mbuf->Location.x < minx[c]) {
                    minx[c] = group->bounds[0] + mbuf->Location.x;
                }
                if (group->bounds[1] + mbuf->Location.x > maxx[c]) {
                    maxx[c] = group->bounds[1] + mbuf->Location.x;
                }
                if (group->bounds[2] + mbuf->Location.z < minz[c]) {
                    minz[c] = group->bounds[2] + mbuf->Location.z;
                }
                if (group->bounds[3] + mbuf->Location.z > maxz[c]) {
                    maxz[c] = group->bounds[3] + mbuf->Location.z;
                }
                ter = (tertype *)(group + 1);
                group = (TerrainGroupType *)&ter[b];
            }
            tx = ((minx[c] + maxx[c]) / 2.0f) - terminx;
            tx = (tx * xsteps) / (termaxx - terminx);
            if (tx < 0) {
                tx = 0;
            }
            if (tx > xsteps - 1) {
                tx = xsteps - 1;
            }
            tz = ((minz[c] + maxz[c]) / 2.0f) - terminz;
            tz = (tz * zsteps) / (termaxz - terminz);
            if (tz < 0) {
                tz = 0;
            }
            if (tz > zsteps - 1) {
                tz = zsteps - 1;
            }
            id[c] = tx + tz * xsteps;
        }
        mbuf++;
        c++;
    }
    terrset->terrgroupcnt = 0;
    for (lp = 0; lp < xsteps * zsteps; lp++) {
        terminx = 200000000.0f;
        terminy = 200000000.0f;
        terminz = 200000000.0f;
        termaxx = -200000000.0f;
        termaxy = -200000000.0f;
        termaxz = -200000000.0f;
        terrset->terrgroup[terrset->terrgroupcnt].start = terlistptr;
        c = 0;
        mbuf = &terrset->terr[c];
        while (c < 0x800) {
            if ((id[c] == lp) && (mbuf->type == 0)) {
                if (minx[c] < terminx) {
                    terminx = minx[c];
                }
                if (minz[c] < terminz) {
                    terminz = minz[c];
                }
                if (maxx[c] > termaxx) {
                    termaxx = maxx[c];
                }
                if (maxz[c] > termaxz) {
                    termaxz = maxz[c];
                }
                terrset->terrgroup[terrset->terrgroupcnt].count++;
                terrset->terrlist[terlistptr++] = c;
            }
            mbuf++;
            c++;
        }
        if (terrset->terrgroup[terrset->terrgroupcnt].count != 0) {
            terrset->terrgroup[terrset->terrgroupcnt].bounds[0] = terminx;
            terrset->terrgroup[terrset->terrgroupcnt].bounds[1] = terminz;
            terrset->terrgroup[terrset->terrgroupcnt].bounds[2] = termaxx;
            terrset->terrgroup[terrset->terrgroupcnt].bounds[3] = termaxz;
            terrset->terrgroupcnt++;
        }
    }
    terrset->terrgroup[256].start = terlistptr;
    c = 0;
    mbuf = &terrset->terr[c];
    while (c < 0x800) {
        if (mbuf->type == 1) {
            terrset->terrlist[terlistptr++] = c;
            terrset->terrgroup[256].count++;
            if (platid < 128) {
                index = mbuf->info;
                mbuf->info = platid;
                if (index < gscene->numinstance) {
                    terrset->platdata[mbuf->info].status.bit.rotate = mbuf->flags & 1;
                    if (gscene->instances[index].anim != 0) {
                        terrset->platdata[mbuf->info].curmtx = &gscene->instances[index].anim->mtx;
                    } else {
                        terrset->platdata[mbuf->info].curmtx = &gscene->instances[index].mtx;
                    }
                    terrset->platdata[mbuf->info].instance = index;
                    terrset->platdata[mbuf->info].terrno = c;
                    if (gscene->instances[index].flags.visible == 0) {
                        mbuf->type = -1;
                    }
                    platid++;
                } else {
                    mbuf->type = -1;
                }
            } else {
                mbuf->type = -1;
            }
        }
        mbuf++;
        c++;
    }
    nufile_buffering_enabled = buffering;
    curSphereter = 0;
    TerrShapeAdjCnt = 0;
    curPickInst = 0;
    return rv;
}


s32 ReadTerrainPickup(char *name, short **store, PickupTerrType *pickup) {
    OffFileType *offfile;
    s32 lp;
    s32 count;
    char filename[100];

    strcpy(filename, name);
    strcat(filename, ".ter");
    NuFileLoadBuffer(filename, *store, 0x7FFFFFFF);
    offfile = (OffFileType *)(*store + *(s32 *)*store);
    *store += 2;
    count = 0;
    crashdata = 0;
    for (lp = 0; lp < offfile->count; lp++) {
        switch (offfile->offlist[lp].type) {
            case -1:
                break;
            case 0:
            case 1:
                pickup->terr[count].id = offfile->offlist[lp].id;
                if ((pickup->terr[count].id > 0) && (pickup->terr[count].id < 128)) {
                    pickup->typeterr[pickup->terr[count].id] = count;
                }
                pickup->terr[count].type = offfile->offlist[lp].type;
                pickup->terr[count].info = offfile->offlist[lp].info;
                pickup->terr[count].Location.x = offfile->offlist[lp].translation.x;
                pickup->terr[count].Location.y = offfile->offlist[lp].translation.y;
                pickup->terr[count].Location.z = offfile->offlist[lp].translation.z;
                pickup->terr[count].model = *store;
                pickup->terr[count].flags = offfile->offlist[lp].flags;
                count++;
                break;
            case 2:
                if (*(s32 *)*store == 0x12345678) {
                    *(void **)*store = pickup->wallinfo;
                    pickup->wallinfo = *store + 2;
                    ((WallSplineType *)pickup->wallinfo)->count--;
                }
                break;
            case 3:
                crashdata = *store;
                break;
        }
        *store += offfile->offlist[lp].offset;
    }
    return count;
}


void terrainpickupinit(char *name, short **store) {
    PickupTerrType *pickup;
    TerrainSituType *mbuf;
    tertype *ter;
    TerrainGroupType *group;
    s32 c;
    s32 b;
    s32 buffering;
    f32 terminx;
    f32 termaxx;
    f32 terminy;
    f32 termaxy;
    f32 terminz;
    f32 termaxz;
    f32 maxradius;
    f32 radiussq;

    pickup = (PickupTerrType *)*store;
    *store += 0x57A8;
    pickup->terr = (TerrainSituType *)*store;
    *store += 0xE00;
    pickup->data = *store;
    *store += 0xE00;
    memset(pickup->terr, 0, 0x1C00);
    c = 0;
    mbuf = pickup->terr;
    while (c < 128) {
        mbuf->type = -1;
        c++;
        mbuf++;
    }
    for (c = 0; c < 128; c++) {
        pickup->platdata[c].curmtx = 0;
        pickup->platdata[c].plrgrav = 0.0f;
        pickup->platdata[c].ypos = 0.0f;
        pickup->platdata[c].yvel = 0.0f;
        pickup->platdata[c].damp = 0.0f;
        pickup->platdata[c].tension = 0.0f;
    }
    for (c = 0; c < 128; c++) {
        pickup->typeterr[c] = -1;
    }
    buffering = nufile_buffering_enabled;
    curPickInst = 0;
    nufile_buffering_enabled = 0;
    terraincnt = 0;
    curSphereter = 0;
    platinrange = 0;
    ShadPoly = 0;
    TerrPoly = 0;
    PlatImpactId = 0;
    situtotal = ReadTerrainPickup(name, store, pickup);
    c = 0;
    mbuf = pickup->terr;
    while (c < 128) {
        if ((mbuf->type == 0) || (mbuf->type == 1)) {
            maxradius = 0.0f;
            terminx = 200000000.0f;
            terminy = 200000000.0f;
            terminz = 200000000.0f;
            termaxx = -200000000.0f;
            termaxy = -200000000.0f;
            termaxz = -200000000.0f;
            group = (TerrainGroupType *)mbuf->model;
            while (group->start >= 0) {
                b = group->count;
                ter = (tertype *)(group + 1);
                while (b > 0) {
                    if (ter->info[0] == 0xFF) {
                        ter->info[0] = ter->info[1];
                        ter->info[1] = 0;
                        ter->info[2] |= 0x80;
                    }
                    if (ter->minx < terminx) {
                        terminx = ter->minx;
                    }
                    if (ter->miny < terminy) {
                        terminy = ter->miny;
                    }
                    if (ter->minz < terminz) {
                        terminz = ter->minz;
                    }
                    if (ter->maxx > termaxx) {
                        termaxx = ter->maxx;
                    }
                    if (ter->maxy > termaxy) {
                        termaxy = ter->maxy;
                    }
                    if (ter->maxz > termaxz) {
                        termaxz = ter->maxz;
                    }
                    radiussq = ter->pnts[0].x * ter->pnts[0].x + ter->pnts[0].y * ter->pnts[0].y +
                         ter->pnts[0].z * ter->pnts[0].z;
                    if (maxradius < radiussq) {
                        maxradius = radiussq;
                    }
                    radiussq = ter->pnts[1].x * ter->pnts[1].x + ter->pnts[1].y * ter->pnts[1].y +
                         ter->pnts[1].z * ter->pnts[1].z;
                    if (maxradius < radiussq) {
                        maxradius = radiussq;
                    }
                    radiussq = ter->pnts[2].x * ter->pnts[2].x + ter->pnts[2].y * ter->pnts[2].y +
                         ter->pnts[2].z * ter->pnts[2].z;
                    if (maxradius < radiussq) {
                        maxradius = radiussq;
                    }
                    radiussq = ter->pnts[3].x * ter->pnts[3].x + ter->pnts[3].y * ter->pnts[3].y +
                         ter->pnts[3].z * ter->pnts[3].z;
                    if (maxradius < radiussq) {
                        maxradius = radiussq;
                    }
                    ter++;
                    b--;
                }
                group = (TerrainGroupType *)ter;
            }
            mbuf->max.z = termaxz;
            mbuf->radius = maxradius;
            mbuf->min.x = terminx;
            mbuf->min.y = terminy;
            mbuf->min.z = terminz;
            mbuf->max.x = termaxx;
            mbuf->max.y = termaxy;
            mbuf->type = 0;
            terraincnt++;
        }
        mbuf++;
        c++;
    }
    nufile_buffering_enabled = buffering;
    PickupTerr = pickup;
}


void TerrainPlatformOldUpdate(void) {
    s32 i;

    if (CurTerr != 0) {
        for (i = 0; i < 128; i++) {
            if (CurTerr->platdata[i].curmtx != 0) {
                CurTerr->platdata[i].oldmtx = *CurTerr->platdata[i].curmtx;
            }
        }
        for (i = 0; i < 4; i++) {
            if (CurTerr->TrackInfo[i].ptrid != 0) {
                CurTerr->TrackInfo[i].timer--;
                if (CurTerr->TrackInfo[i].timer <= 0) {
                    CurTerr->TrackInfo[i].ptrid = 0;
                }
            }
        }
    }
}


void TerrainPlatformNewUpdate(void) {
    TerrainSituType *situ;
    short *list;
    s32 i;

    if (CurTerr != 0) {
        list = CurTerr->terrlist + CurTerr->terrgroup[256].start;
        for (i = 0; i < CurTerr->terrgroup[256].count; i++, list++) {
            situ = &CurTerr->terr[*list];
            if (CurTerr->platdata[situ->info].curmtx != 0) {
                if (CurTerr->platdata[situ->info].hitcnt != 0) {
                    if (CurTerr->platdata[situ->info].hitcnt >= 0x7D) {
                        CurTerr->platdata[situ->info].yvel +=
                            CurTerr->platdata[situ->info].plrgrav -
                            CurTerr->platdata[situ->info].yvel * CurTerr->platdata[situ->info].damp;
                    } else {
                        CurTerr->platdata[situ->info].yvel +=
                            -CurTerr->platdata[situ->info].yvel * CurTerr->platdata[situ->info].damp;
                    }
                    CurTerr->platdata[situ->info].yvel = CurTerr->platdata[situ->info].yvel +
                        (DABS(CurTerr->platdata[situ->info].ypos) * -CurTerr->platdata[situ->info].ypos *
                             CurTerr->platdata[situ->info].tension * 0.5 -
                         CurTerr->platdata[situ->info].ypos * CurTerr->platdata[situ->info].tension);
                    CurTerr->platdata[situ->info].ypos += CurTerr->platdata[situ->info].yvel;
                    CurTerr->platdata[situ->info].hitcnt--;
                    CurTerr->platdata[situ->info].curmtx->_31 += CurTerr->platdata[situ->info].ypos;
                }
                situ->Location.x = CurTerr->platdata[situ->info].curmtx->_30;
                situ->Location.y = CurTerr->platdata[situ->info].curmtx->_31;
                situ->Location.z = CurTerr->platdata[situ->info].curmtx->_32;
            }
            if (CurTerr->platdata[situ->info].status.bit.hit) {
                CurTerr->platdata[situ->info].hitcnt = 0x80;
            }
            CurTerr->platdata[situ->info].status.bit.hit = 0;
        }
    }
}


void TerrDraw(tertype *ter, short terrno) {
    s32 i;
    signed char red;
    signed char green;

    red = terrno & 0xF0;
    green = terrno << 4;
    if (ter->norm[1].y > 65535.0f) {
        for (i = 0; i < 2; i++) {
            NuRndrLine3dDbg(CurTerr->terr[terrno].Location.x + ter->pnts[0].x,
                            CurTerr->terr[terrno].Location.y + ter->pnts[0].y,
                            CurTerr->terr[terrno].Location.z + ter->pnts[0].z,
                            CurTerr->terr[terrno].Location.x + ter->pnts[1].x + i * (ter->pnts[2].x - ter->pnts[1].x),
                            CurTerr->terr[terrno].Location.y + ter->pnts[1].y + i * (ter->pnts[2].y - ter->pnts[1].y),
                            CurTerr->terr[terrno].Location.z + ter->pnts[1].z + i * (ter->pnts[2].z - ter->pnts[1].z),
                            (green << 8) + red + 0x400000);
        }
        for (i = 0; i < 2; i++) {
            NuRndrLine3dDbg(CurTerr->terr[terrno].Location.x + ter->pnts[1].x,
                            CurTerr->terr[terrno].Location.y + ter->pnts[1].y,
                            CurTerr->terr[terrno].Location.z + ter->pnts[1].z,
                            CurTerr->terr[terrno].Location.x + ter->pnts[0].x + i * (ter->pnts[2].x - ter->pnts[0].x),
                            CurTerr->terr[terrno].Location.y + ter->pnts[0].y + i * (ter->pnts[2].y - ter->pnts[0].y),
                            CurTerr->terr[terrno].Location.z + ter->pnts[0].z + i * (ter->pnts[2].z - ter->pnts[0].z),
                            (green << 8) + red + 0x400000);
        }
    } else {
        for (i = 0; i < 2; i++) {
            NuRndrLine3dDbg(CurTerr->terr[terrno].Location.x + ter->pnts[0].x + i * (ter->pnts[1].x - ter->pnts[0].x),
                            CurTerr->terr[terrno].Location.y + ter->pnts[0].y + i * (ter->pnts[1].y - ter->pnts[0].y),
                            CurTerr->terr[terrno].Location.z + ter->pnts[0].z + i * (ter->pnts[1].z - ter->pnts[0].z),
                            CurTerr->terr[terrno].Location.x + ter->pnts[2].x + i * (ter->pnts[3].x - ter->pnts[2].x),
                            CurTerr->terr[terrno].Location.y + ter->pnts[2].y + i * (ter->pnts[3].y - ter->pnts[2].y),
                            CurTerr->terr[terrno].Location.z + ter->pnts[2].z + i * (ter->pnts[3].z - ter->pnts[2].z),
                            (green << 8) + red);
        }
        for (i = 0; i < 2; i++) {
            NuRndrLine3dDbg(CurTerr->terr[terrno].Location.x + ter->pnts[0].x + i * (ter->pnts[2].x - ter->pnts[0].x),
                            CurTerr->terr[terrno].Location.y + ter->pnts[0].y + i * (ter->pnts[2].y - ter->pnts[0].y),
                            CurTerr->terr[terrno].Location.z + ter->pnts[0].z + i * (ter->pnts[2].z - ter->pnts[0].z),
                            CurTerr->terr[terrno].Location.x + ter->pnts[1].x + i * (ter->pnts[3].x - ter->pnts[1].x),
                            CurTerr->terr[terrno].Location.y + ter->pnts[1].y + i * (ter->pnts[3].y - ter->pnts[1].y),
                            CurTerr->terr[terrno].Location.z + ter->pnts[1].z + i * (ter->pnts[3].z - ter->pnts[1].z),
                            (green << 8) + red);
        }
    }
}


void TerrDrawSitu(tertype *ter, struct nuvec_s *pos) {
    s32 i;
    signed char red;
    signed char green;

    red = 0x80;
    green = 0x80;
    if (ter->norm[1].y > 65535.0f) {
        for (i = 0; i < 2; i++) {
            NuRndrLine3dDbg(pos->x + ter->pnts[0].x,
                            pos->y + ter->pnts[0].y,
                            pos->z + ter->pnts[0].z,
                            pos->x + ter->pnts[1].x + i * (ter->pnts[2].x - ter->pnts[1].x),
                            pos->y + ter->pnts[1].y + i * (ter->pnts[2].y - ter->pnts[1].y),
                            pos->z + ter->pnts[1].z + i * (ter->pnts[2].z - ter->pnts[1].z),
                            (green << 8) + red + 0x400000);
        }
        for (i = 0; i < 2; i++) {
            NuRndrLine3dDbg(pos->x + ter->pnts[1].x,
                            pos->y + ter->pnts[1].y,
                            pos->z + ter->pnts[1].z,
                            pos->x + ter->pnts[0].x + i * (ter->pnts[2].x - ter->pnts[0].x),
                            pos->y + ter->pnts[0].y + i * (ter->pnts[2].y - ter->pnts[0].y),
                            pos->z + ter->pnts[0].z + i * (ter->pnts[2].z - ter->pnts[0].z),
                            (green << 8) + red + 0x400000);
        }
    } else {
        for (i = 0; i < 2; i++) {
            NuRndrLine3dDbg(pos->x + ter->pnts[0].x + i * (ter->pnts[1].x - ter->pnts[0].x),
                            pos->y + ter->pnts[0].y + i * (ter->pnts[1].y - ter->pnts[0].y),
                            pos->z + ter->pnts[0].z + i * (ter->pnts[1].z - ter->pnts[0].z),
                            pos->x + ter->pnts[2].x + i * (ter->pnts[3].x - ter->pnts[2].x),
                            pos->y + ter->pnts[2].y + i * (ter->pnts[3].y - ter->pnts[2].y),
                            pos->z + ter->pnts[2].z + i * (ter->pnts[3].z - ter->pnts[2].z),
                            (green << 8) + red);
        }
        for (i = 0; i < 2; i++) {
            NuRndrLine3dDbg(pos->x + ter->pnts[0].x + i * (ter->pnts[2].x - ter->pnts[0].x),
                            pos->y + ter->pnts[0].y + i * (ter->pnts[2].y - ter->pnts[0].y),
                            pos->z + ter->pnts[0].z + i * (ter->pnts[2].z - ter->pnts[0].z),
                            pos->x + ter->pnts[1].x + i * (ter->pnts[3].x - ter->pnts[1].x),
                            pos->y + ter->pnts[1].y + i * (ter->pnts[3].y - ter->pnts[1].y),
                            pos->z + ter->pnts[1].z + i * (ter->pnts[3].z - ter->pnts[1].z),
                            (green << 8) + red);
        }
    }
}


void TerrDrawPlat(tertype *ter, short terrno) {
    struct nuvec4_s pnts[5];
    s32 i;
    signed char red;
    signed char green;

    red = terrno & 0xF0;
    green = terrno << 4;
    pnts[0].x = ter->pnts[0].x;
    pnts[0].y = ter->pnts[0].y;
    pnts[0].z = ter->pnts[0].z;
    pnts[0].w = 0.0f;
    pnts[1].x = ter->pnts[1].x;
    pnts[1].y = ter->pnts[1].y;
    pnts[1].z = ter->pnts[1].z;
    pnts[1].w = 0.0f;
    pnts[2].x = ter->pnts[2].x;
    pnts[2].y = ter->pnts[2].y;
    pnts[2].z = ter->pnts[2].z;
    pnts[2].w = 0.0f;
    pnts[3].x = ter->pnts[3].x;
    pnts[3].y = ter->pnts[3].y;
    pnts[3].z = ter->pnts[3].z;
    pnts[3].w = 0.0f;
    pnts[4].x = ter->norm[0].x * 0.2f;
    pnts[4].y = ter->norm[0].y * 0.2f;
    pnts[4].z = ter->norm[0].z * 0.2f;
    pnts[4].w = 0.0f;
    if (CurTerr->platdata[CurTerr->terr[terrno].info].status.bit.rotate) {
        NuVec4MtxTransformVU0(&pnts[0], &pnts[0], CurTerr->platdata[CurTerr->terr[terrno].info].curmtx);
        NuVec4MtxTransformVU0(&pnts[1], &pnts[1], CurTerr->platdata[CurTerr->terr[terrno].info].curmtx);
        NuVec4MtxTransformVU0(&pnts[2], &pnts[2], CurTerr->platdata[CurTerr->terr[terrno].info].curmtx);
        NuVec4MtxTransformVU0(&pnts[3], &pnts[3], CurTerr->platdata[CurTerr->terr[terrno].info].curmtx);
        NuVec4MtxTransformVU0(&pnts[4], &pnts[4], CurTerr->platdata[CurTerr->terr[terrno].info].curmtx);
    }
    if (ter->norm[1].y > 65535.0f) {
        for (i = 0; i < 2; i++) {
            NuRndrLine3dDbg(CurTerr->terr[terrno].Location.x + pnts[0].x,
                            CurTerr->terr[terrno].Location.y + pnts[0].y,
                            CurTerr->terr[terrno].Location.z + pnts[0].z,
                            CurTerr->terr[terrno].Location.x + pnts[1].x + i * (pnts[2].x - pnts[1].x),
                            CurTerr->terr[terrno].Location.y + pnts[1].y + i * (pnts[2].y - pnts[1].y),
                            CurTerr->terr[terrno].Location.z + pnts[1].z + i * (pnts[2].z - pnts[1].z),
                            (green << 8) + red + 0x400000);
        }
        for (i = 0; i < 2; i++) {
            NuRndrLine3dDbg(CurTerr->terr[terrno].Location.x + pnts[1].x,
                            CurTerr->terr[terrno].Location.y + pnts[1].y,
                            CurTerr->terr[terrno].Location.z + pnts[1].z,
                            CurTerr->terr[terrno].Location.x + pnts[0].x + i * (pnts[2].x - pnts[0].x),
                            CurTerr->terr[terrno].Location.y + pnts[0].y + i * (pnts[2].y - pnts[0].y),
                            CurTerr->terr[terrno].Location.z + pnts[0].z + i * (pnts[2].z - pnts[0].z),
                            (green << 8) + red + 0x400000);
        }
    } else {
        for (i = 0; i < 2; i++) {
            NuRndrLine3dDbg(CurTerr->terr[terrno].Location.x + pnts[0].x + i * (pnts[1].x - pnts[0].x),
                            CurTerr->terr[terrno].Location.y + pnts[0].y + i * (pnts[1].y - pnts[0].y),
                            CurTerr->terr[terrno].Location.z + pnts[0].z + i * (pnts[1].z - pnts[0].z),
                            CurTerr->terr[terrno].Location.x + pnts[2].x + i * (pnts[3].x - pnts[2].x),
                            CurTerr->terr[terrno].Location.y + pnts[2].y + i * (pnts[3].y - pnts[2].y),
                            CurTerr->terr[terrno].Location.z + pnts[2].z + i * (pnts[3].z - pnts[2].z),
                            (green << 8) + red);
        }
        for (i = 0; i < 2; i++) {
            NuRndrLine3dDbg(CurTerr->terr[terrno].Location.x + pnts[0].x + i * (pnts[2].x - pnts[0].x),
                            CurTerr->terr[terrno].Location.y + pnts[0].y + i * (pnts[2].y - pnts[0].y),
                            CurTerr->terr[terrno].Location.z + pnts[0].z + i * (pnts[2].z - pnts[0].z),
                            CurTerr->terr[terrno].Location.x + pnts[1].x + i * (pnts[3].x - pnts[1].x),
                            CurTerr->terr[terrno].Location.y + pnts[1].y + i * (pnts[3].y - pnts[1].y),
                            CurTerr->terr[terrno].Location.z + pnts[1].z + i * (pnts[3].z - pnts[1].z),
                            (green << 8) + red);
        }
    }
    NuRndrLine3dDbg(CurTerr->terr[terrno].Location.x + pnts[0].x,
                    CurTerr->terr[terrno].Location.y + pnts[0].y,
                    CurTerr->terr[terrno].Location.z + pnts[0].z,
                    CurTerr->terr[terrno].Location.x + pnts[0].x + pnts[4].x,
                    CurTerr->terr[terrno].Location.y + pnts[0].y + pnts[4].y,
                    CurTerr->terr[terrno].Location.z + pnts[0].z + pnts[4].z,
                    (green << 8) + red);
}


void SphereDraw(struct nuvec_s *pos, f32 radius, f32 scale) {
    s32 i;
    s32 ang;
    f32 r1;
    f32 r2;
    f32 y1;
    f32 y2;

    for (i = 0; i < 8; i++) {
        if (i < 4) {
            r1 = NuTrigTable[(i << 12) & 0xffff] * radius;
        } else {
            r1 = NuTrigTable[((8 - i) << 12) & 0xffff] * radius;
        }
        if (i < 3) {
            r2 = NuTrigTable[((i + 1) << 12) & 0xffff] * radius;
        } else {
            r2 = NuTrigTable[((7 - i) << 12) & 0xffff] * radius;
        }
        y1 = NuFsqrt(radius * radius - r1 * r1) * scale;
        y2 = NuFsqrt(radius * radius - r2 * r2) * scale;
        if (i >= 5) {
            y1 = -y1;
        }
        if (i >= 4) {
            y2 = -y2;
        }
        y1 += pos->y + radius * scale;
        y2 += pos->y + radius * scale;
        for (ang = 0; ang < 0x10000; ang += 0x800) {
            NuRndrLine3dDbg(pos->x + NuTrigTable[(ang + 0x4000) & 0xffff] * r1, y1,
                            pos->z + NuTrigTable[ang & 0xffff] * r1,
                            pos->x + NuTrigTable[(ang + 0x4000) & 0xffff] * r2, y2,
                            pos->z + NuTrigTable[ang & 0xffff] * r2, 0x400000);
            NuRndrLine3dDbg(pos->x + NuTrigTable[(ang + 0x4000) & 0xffff] * r1, y1,
                            pos->z + NuTrigTable[ang & 0xffff] * r1,
                            pos->x + NuTrigTable[(ang + 0x4800) & 0xffff] * r1, y1,
                            pos->z + NuTrigTable[(ang + 0x800) & 0xffff] * r1, 0x400000);
        }
    }
}


//NGC MATCH
void RotateVec(struct nuvec_s *in, struct nuvec_s *out) {
    float tz;

    tz = in->y * NuTrigTable[(s32)TerI->ax & 0xffff] +
         in->z * NuTrigTable[(s32)(TerI->ax + 16384.0f) & 0xffff];
    out->y = in->y * NuTrigTable[(s32)(TerI->ax + 16384.0f) & 0xffff] -
             in->z * NuTrigTable[(s32)TerI->ax & 0xffff];
    out->z = tz * NuTrigTable[(s32)(TerI->ay + 16384.0f) & 0xffff] -
             in->x * NuTrigTable[(s32)TerI->ay & 0xffff];
    out->x = tz * NuTrigTable[(s32)TerI->ay & 0xffff] +
             in->x * NuTrigTable[(s32)(TerI->ay + 16384.0f) & 0xffff];
}

void RotateTerrain(tertype *ter) {
    float sinax;
    float cosax;
    float sinay;
    float cosay;
    float tz;

    sinax = NuTrigTable[(s32)TerI->ax & 0xffff];
    cosax = NuTrigTable[(s32)(TerI->ax + 16384.0f) & 0xffff];
    sinay = NuTrigTable[(s32)TerI->ay & 0xffff];
    cosay = NuTrigTable[(s32)(TerI->ay + 16384.0f) & 0xffff];
    TerI->rotter.pnts[0].y = ter->pnts[0].y * cosax - ter->pnts[0].z * sinax;
    tz = ter->pnts[0].y * sinax + ter->pnts[0].z * cosax;
    TerI->rotter.pnts[0].z = tz * cosay - ter->pnts[0].x * sinay;
    TerI->rotter.pnts[0].x = tz * sinay + ter->pnts[0].x * cosay;
    TerI->rotter.pnts[1].y = ter->pnts[1].y * cosax - ter->pnts[1].z * sinax;
    tz = ter->pnts[1].y * sinax + ter->pnts[1].z * cosax;
    TerI->rotter.pnts[1].z = tz * cosay - ter->pnts[1].x * sinay;
    TerI->rotter.pnts[1].x = tz * sinay + ter->pnts[1].x * cosay;
    TerI->rotter.pnts[2].y = ter->pnts[2].y * cosax - ter->pnts[2].z * sinax;
    tz = ter->pnts[2].y * sinax + ter->pnts[2].z * cosax;
    TerI->rotter.pnts[2].z = tz * cosay - ter->pnts[2].x * sinay;
    TerI->rotter.pnts[2].x = tz * sinay + ter->pnts[2].x * cosay;
    if (ter->norm[1].y < 65536.0f) {
        TerI->rotter.pnts[3].y = ter->pnts[3].y * cosax - ter->pnts[3].z * sinax;
        tz = ter->pnts[3].y * sinax + ter->pnts[3].z * cosax;
        TerI->rotter.pnts[3].z = tz * cosay - ter->pnts[3].x * sinay;
        TerI->rotter.pnts[3].x = tz * sinay + ter->pnts[3].x * cosay;
    }
}

//NGC MATCH
inline void DerotateMovementVector(void) {
    TerI->ay = (s32)NuAtan2D((TerI->curvel).x, (TerI->curvel).z);
    TerI->ax = (s32)NuAtan2D(-(TerI->curvel).y, NuFsqrt((TerI->curvel).x * (TerI->curvel).x + (TerI->curvel).z * (TerI->curvel).z));
    TerI->len = NuFsqrt((TerI->curvel).x * (TerI->curvel).x + (TerI->curvel).y * (TerI->curvel).y +
                    (TerI->curvel).z * (TerI->curvel).z);
}

//NGC MATCH
void DeRotateTerrain(tertype *ter) {
    float sinax;
    float cosax;
    float sinay;
    float cosay;
    float tz;
    float dist[8];

    sinax = NuTrigTable[(s32)-TerI->ax & 0xffff];
    cosax = NuTrigTable[(s32)(-TerI->ax + 16384.0f) & 0xffff];
    sinay = NuTrigTable[(s32)-TerI->ay & 0xffff];
    cosay = NuTrigTable[(s32)(-TerI->ay + 16384.0f) & 0xffff];
    dist[0] = (f32)(((ter->pnts[0].x - TerI->csx) * (ter->pnts[0].x - TerI->csx)) + ((ter->pnts[0].y - TerI->csy) * (ter->pnts[0].y - TerI->csy)) + ((ter->pnts[0].z - TerI->csz) * (ter->pnts[0].z - TerI->csz)));
    dist[1] = (f32)(((ter->pnts[1].x - TerI->csx) * (ter->pnts[1].x - TerI->csx)) + ((ter->pnts[1].y - TerI->csy) * (ter->pnts[1].y - TerI->csy)) + ((ter->pnts[1].z - TerI->csz) * (ter->pnts[1].z - TerI->csz)));
    dist[2] = (f32)(((ter->pnts[2].x - TerI->csx) * (ter->pnts[2].x - TerI->csx)) + ((ter->pnts[2].y - TerI->csy) * (ter->pnts[2].y - TerI->csy)) + ((ter->pnts[2].z - TerI->csz) * (ter->pnts[2].z - TerI->csz)));
    dist[3] = (f32)(((ter->pnts[3].x - TerI->csx) * (ter->pnts[3].x - TerI->csx)) + ((ter->pnts[3].y - TerI->csy) * (ter->pnts[3].y - TerI->csy)) + ((ter->pnts[3].z - TerI->csz) * (ter->pnts[3].z - TerI->csz)));
    tz = (ter->pnts[0].z - TerI->csz) * cosay - (ter->pnts[0].x - TerI->csx) * sinay;
    (TerI->rotter).pnts[0].x =
        (ter->pnts[0].z - TerI->csz) * sinay + (ter->pnts[0].x - TerI->csx) * cosay;
    (TerI->rotter).pnts[0].y = (ter->pnts[0].y - TerI->csy) * cosax - tz * sinax;
    (TerI->rotter).pnts[0].z = (ter->pnts[0].y - TerI->csy) * sinax + tz * cosax;
    tz = (ter->pnts[1].z - TerI->csz) * cosay - (ter->pnts[1].x - TerI->csx) * sinay;
    (TerI->rotter).pnts[1].x =
        (ter->pnts[1].z - TerI->csz) * sinay + (ter->pnts[1].x - TerI->csx) * cosay;
    (TerI->rotter).pnts[1].y = (ter->pnts[1].y - TerI->csy) * cosax - tz * sinax;
    (TerI->rotter).pnts[1].z = (ter->pnts[1].y - TerI->csy) * sinax + tz * cosax;
    tz = (ter->pnts[2].z - TerI->csz) * cosay - (ter->pnts[2].x - TerI->csx) * sinay;
    (TerI->rotter).pnts[2].x =
        (ter->pnts[2].z - TerI->csz) * sinay + (ter->pnts[2].x - TerI->csx) * cosay;
    (TerI->rotter).pnts[2].y = (ter->pnts[2].y - TerI->csy) * cosax - tz * sinax;
    (TerI->rotter).pnts[2].z = (ter->pnts[2].y - TerI->csy) * sinax + tz * cosax;
    if (ter->norm[1].y < 65536.0f) {
        tz = (ter->pnts[3].z - TerI->csz) * cosay - (ter->pnts[3].x - TerI->csx) * sinay;
        (TerI->rotter).pnts[3].x =
            (ter->pnts[3].z - TerI->csz) * sinay + (ter->pnts[3].x - TerI->csx) * cosay;
        (TerI->rotter).pnts[3].y = (ter->pnts[3].y - TerI->csy) * cosax - tz * sinax;
        (TerI->rotter).pnts[3].z = (ter->pnts[3].y - TerI->csy) * sinax + tz * cosax;
    }
    dist[4] = (f32)((TerI->rotter.pnts[0].x * TerI->rotter.pnts[0].x) + (TerI->rotter.pnts[0].y * TerI->rotter.pnts[0].y) + (TerI->rotter.pnts[0].z * TerI->rotter.pnts[0].z));
    dist[5] = (f32)((TerI->rotter.pnts[1].x * TerI->rotter.pnts[1].x) + (TerI->rotter.pnts[1].y * TerI->rotter.pnts[1].y) + (TerI->rotter.pnts[1].z * TerI->rotter.pnts[1].z));
    dist[6] = (f32)((TerI->rotter.pnts[2].x * TerI->rotter.pnts[2].x) + (TerI->rotter.pnts[2].y * TerI->rotter.pnts[2].y) + (TerI->rotter.pnts[2].z * TerI->rotter.pnts[2].z));
    dist[7] = (f32)((TerI->rotter.pnts[3].x * TerI->rotter.pnts[3].x) + (TerI->rotter.pnts[3].y * TerI->rotter.pnts[3].y) + (TerI->rotter.pnts[3].z * TerI->rotter.pnts[3].z));
}

//NGC MATCH
void DeRotatePoint(struct nuvec_s *pnt) {
    float sinax;
    float cosax;
    float sinay;
    float cosay;
    float tz;

    sinax = NuTrigTable[(s32)-TerI->ax & 0xffff];
    cosax = NuTrigTable[(s32)(-TerI->ax + 16384.0f) & 0xffff];
    sinay = NuTrigTable[(s32)-TerI->ay & 0xffff];
    cosay = NuTrigTable[(s32)(-TerI->ay + 16384.0f) & 0xffff];
    tz = (pnt->z - (TerI->curpos).z) * cosay - (pnt->x - (TerI->curpos).x) * sinay;
    pnt->x = (pnt->z - (TerI->curpos).z) * sinay + (pnt->x - (TerI->curpos).x) * cosay;
    pnt->z = ((pnt->y + TerI->size) - (TerI->curpos).y) * sinax + tz * cosax;
    pnt->y = ((pnt->y + TerI->size) - (TerI->curpos).y) * cosax - tz * sinax;
}

//NGC MATCH
void TerrainSideClamp(struct nuvec_s *slide, struct nuvec_s *pos) {
    float dotp;
    float dotq;

    dotp = (TerrShape->offset).x * slide->x + (TerrShape->offset).z * slide->z;
    dotq = (TerrShape->offset).x * slide->z - (TerrShape->offset).z * slide->x;
    if (dotp > TerrShape->size) {
        dotp = TerrShape->size;
    }
    if (dotp < -TerrShape->size) {
        dotp = -TerrShape->size;
    }
    if (dotq > TerrShape->size * 0.2f) {
        dotq = TerrShape->size * 0.2f;
    }
    if (dotq < -TerrShape->size * 0.2f) {
        dotq = -TerrShape->size * 0.2f;
    }
    pos->x = pos->x - ((dotp * slide->x + dotq * slide->z) - (TerrShape->offset).x);
    pos->z = pos->z - ((dotp * slide->z - dotq * slide->x) - (TerrShape->offset).z);
    (TerrShape->offset).x = dotp * slide->x + dotq * slide->z;
    (TerrShape->offset).z = dotp * slide->z - dotq * slide->x;
}

//NGC MATCH
short InsideLineF(float _x, float _z, float _x0, float _z0, float _x1, float _z1) {
    if ((_x - _x0) * (_z1 - _z0) + (_z - _z0) * (_x0 - _x1) < 0.0f) {
        return 0;
    } else {
        return 1;
    }
}

/* Retail NewCast has no calls, yet every inside-line test still materializes
 * InsideLineF's 0/1 return into a GPR before it is tested (and the last one
 * feeds a movn): the body was inlined by the compiler, not folded into a
 * branch. Expanding it through this inline twin reproduces that shape. */
static __inline__ short INSIDELINEF(float _x, float _z, float _x0, float _z0,
                                    float _x1, float _z1) {
    if ((_x - _x0) * (_z1 - _z0) + (_z - _z0) * (_x0 - _x1) < 0.0f) {
        return 0;
    } else {
        return 1;
    }
}

short InsidePolLines(f32 x, f32 y, f32 z, f32 dx0, f32 dy0, f32 dz0, f32 dx1, f32 dy1, f32 dz1,
                     struct nuvec_s *norm) {
    if ((DABS(norm->y) >= DABS(norm->x)) && (DABS(norm->y) >= DABS(norm->z))) {
        if (norm->y < 0.0f) {
            if (INSIDELINEF(x, z, 0.0f, 0.0f, dx0, dz0) && INSIDELINEF(x, z, dx1, dz1, 0.0f, 0.0f) &&
                INSIDELINEF(x, z, dx0, dz0, dx1, dz1)) {
                return 1;
            }
        } else {
            if (INSIDELINEF(x, z, dx0, dz0, 0.0f, 0.0f) && INSIDELINEF(x, z, 0.0f, 0.0f, dx1, dz1) &&
                INSIDELINEF(x, z, dx1, dz1, dx0, dz0)) {
                return 1;
            }
        }
    } else if ((DABS(norm->x) >= DABS(norm->y)) && (DABS(norm->x) >= DABS(norm->z))) {
        if (norm->x < 0.0f) {
            if (INSIDELINEF(y, z, dy0, dz0, 0.0f, 0.0f) && INSIDELINEF(y, z, 0.0f, 0.0f, dy1, dz1) &&
                INSIDELINEF(y, z, dy1, dz1, dy0, dz0)) {
                return 1;
            }
        } else {
            if (INSIDELINEF(y, z, 0.0f, 0.0f, dy0, dz0) && INSIDELINEF(y, z, dy1, dz1, 0.0f, 0.0f) &&
                INSIDELINEF(y, z, dy0, dz0, dy1, dz1)) {
                return 1;
            }
        }
    } else {
        if (norm->z < 0.0f) {
            if (INSIDELINEF(y, x, 0.0f, 0.0f, dy0, dx0) && INSIDELINEF(y, x, dy1, dx1, 0.0f, 0.0f) &&
                INSIDELINEF(y, x, dy0, dx0, dy1, dx1)) {
                return 1;
            }
        } else {
            if (INSIDELINEF(y, x, dy0, dx0, 0.0f, 0.0f) && INSIDELINEF(y, x, 0.0f, 0.0f, dy1, dx1) &&
                INSIDELINEF(y, x, dy1, dx1, dy0, dx0)) {
                return 1;
            }
        }
    }
    return 0;
}


void NewScan(struct nuvec_s *ppos, s32 extramask, s32 platscan) {
    struct nuvec_s pos;
    s32 a;
    s32 b;
    s32 c;
    s32 id;
    s32 hitcnt;
    s32 index;
    s32 lp;
    s32 curobj;
    short *curdata;
    tertype **curter;
    tertype *ter;
    tertype **hitdata;
    tertype **maxdata;
    short *lastwrite;
    short *list;
    f32 maxx;
    f32 maxz;
    f32 tmaxx;
    f32 tmaxz;
    f32 minx;
    f32 minz;
    f32 tminx;
    f32 tminz;
    TerrainSituType *situ;
    TerrainGroupType *group;

    pos = *ppos;
    index = -1;
    b = 0x7FFFFFFF;
    id = -1;
    for (lp = 0; lp < 8; lp++) {
        if ((CurTerr->pollist[lp].timer > 0) && ((pos.x + 1.0f) - CurTerr->pollist[lp].x < 2.0f) &&
            ((pos.z + 1.0f) - CurTerr->pollist[lp].z < 2.0f) &&
            ((pos.x + 1.0f) - CurTerr->pollist[lp].x > 0.0f) &&
            ((pos.z + 1.0f) - CurTerr->pollist[lp].z > 0.0f)) {
            id = lp;
            break;
        }
        if (CurTerr->pollist[lp].timer < b) {
            b = CurTerr->pollist[lp].timer;
            index = lp;
        }
    }
    if (id >= 0) {
        lastwrite = (short *)TerI->hitdata;
        index = id;
        CurTerr->pollist[index].timer = 8;
        maxx = pos.x + 0.1f;
        maxz = pos.z + 0.1f;
        minx = pos.x - 0.1f;
        minz = pos.z - 0.1f;
        hitdata = TerI->hitdata + 1;
        maxdata = hitdata + 0x1FC;
        hitcnt = 0;
        curdata = (short *)CurTerr->pollist[index].hitdata;
        while (*curdata > 0) {
            curobj = curdata[1];
            situ = &CurTerr->terr[curobj];
            curter = (tertype **)(curdata + SCAN_SLOT);
            if ((maxx >= situ->min.x) && (maxz >= situ->min.z) && (minx <= situ->max.x) &&
                (minz < situ->max.z) && (situ->type != -1)) {
                tmaxx = maxx - situ->Location.x;
                tmaxz = maxz - situ->Location.z;
                tminx = minx - situ->Location.x;
                tminz = minz - situ->Location.z;
                for (lp = *curdata; lp > 0; lp--, curter++) {
                    ter = *curter;
                    if ((tmaxx >= ter->minx) && (tminx < ter->maxx) && (tmaxz >= ter->minz) &&
                        (tminz < ter->maxz) && (hitdata < maxdata) &&
                        ((ter->info[1] == 0) || ((ter->info[1] & extramask) != 0))) {
                        *hitdata = ter;
                        hitdata++;
                        hitcnt++;
                    }
                }
                if (hitcnt != 0) {
                    lastwrite[0] = hitcnt;
                    lastwrite[1] = curobj;
                    lastwrite = (short *)hitdata;
                    hitdata++;
                    hitcnt = 0;
                }
                curdata = (short *)curter;
            } else {
                curdata = (short *)(curter + *curdata);
            }
        }
    } else {
        TerI->hitcnt = 0;
        lastwrite = (short *)CurTerr->pollist[index].hitdata;
        hitdata = CurTerr->pollist[index].hitdata + 1;
        maxdata = hitdata + 0x1FC;
        hitcnt = 0;
        maxx = pos.x + 1.0f;
        maxz = pos.z + 1.0f;
        minx = pos.x - 1.0f;
        minz = pos.z - 1.0f;
        for (a = 0; a < CurTerr->terrgroupcnt; a++) {
            if ((maxx >= CurTerr->terrgroup[a].bounds[0]) && (maxz >= CurTerr->terrgroup[a].bounds[1]) &&
                (minx <= CurTerr->terrgroup[a].bounds[2]) && (minz <= CurTerr->terrgroup[a].bounds[3])) {
                list = CurTerr->terrlist + CurTerr->terrgroup[a].start;
                for (b = 0; b < CurTerr->terrgroup[a].count; b++, list++) {
                    situ = &CurTerr->terr[*list];
                    if ((maxx >= situ->min.x) && (maxz >= situ->min.z) && (minx <= situ->max.x) &&
                        (minz < situ->max.z) && (situ->type != -1)) {
                        tmaxx = maxx - situ->Location.x;
                        tmaxz = maxz - situ->Location.z;
                        tminx = minx - situ->Location.x;
                        tminz = minz - situ->Location.z;
                        group = (TerrainGroupType *)situ->model;
                        while (group->start >= 0) {
                            c = group->count;
                            ter = (tertype *)(group + 1);
                            if ((tmaxx >= group->bounds[0]) && (tminx < group->bounds[1])) {
                                if ((tmaxz >= group->bounds[2]) && (tminz < group->bounds[3])) {
                                    for (; c > 0; c--) {
                                        if ((tmaxx >= ter->minx) && (tminx < ter->maxx) &&
                                            (tmaxz >= ter->minz) && (tminz < ter->maxz) &&
                                            (hitdata < maxdata)) {
                                            *hitdata = ter;
                                            hitdata++;
                                            hitcnt++;
                                        }
                                        ter++;
                                    }
                                    group = (TerrainGroupType *)ter;
                                } else {
                                    group = (TerrainGroupType *)(ter + c);
                                }
                            } else {
                                group = (TerrainGroupType *)(ter + c);
                            }
                        }
                        if (hitcnt != 0) {
                            lastwrite[0] = hitcnt;
                            lastwrite[1] = *list;
                            lastwrite = (short *)hitdata;
                            hitdata++;
                            hitcnt = 0;
                        }
                    }
                }
            }
        }
        CurTerr->pollist[index].timer = 8;
        CurTerr->pollist[index].x = ppos->x;
        CurTerr->pollist[index].z = ppos->z;
        lastwrite[0] = 0;
        lastwrite[1] = 0;
        lastwrite = (short *)TerI->hitdata;
        maxx = pos.x + 0.1f;
        maxz = pos.z + 0.1f;
        minx = pos.x - 0.1f;
        minz = pos.z - 0.1f;
        hitdata = TerI->hitdata + 1;
        maxdata = hitdata + 0x1FC;
        hitcnt = 0;
        curdata = (short *)CurTerr->pollist[index].hitdata;
        while (*curdata > 0) {
            curobj = curdata[1];
            situ = &CurTerr->terr[curobj];
            curter = (tertype **)(curdata + SCAN_SLOT);
            if ((maxx >= situ->min.x) && (maxz >= situ->min.z) && (minx <= situ->max.x) &&
                (minz < situ->max.z) && (situ->type != -1)) {
                tmaxx = maxx - situ->Location.x;
                tmaxz = maxz - situ->Location.z;
                tminx = minx - situ->Location.x;
                tminz = minz - situ->Location.z;
                for (lp = *curdata; lp > 0; lp--, curter++) {
                    ter = *curter;
                    if ((tmaxx >= ter->minx) && (tminx < ter->maxx) && (tmaxz >= ter->minz) &&
                        (tminz < ter->maxz) && (hitdata < maxdata) &&
                        ((ter->info[1] == 0) || ((ter->info[1] & extramask) != 0))) {
                        *hitdata = ter;
                        hitdata++;
                        hitcnt++;
                    }
                }
                if (hitcnt != 0) {
                    lastwrite[0] = hitcnt;
                    lastwrite[1] = curobj;
                    lastwrite = (short *)hitdata;
                    hitdata++;
                    hitcnt = 0;
                }
                curdata = (short *)curter;
            } else {
                curdata = (short *)(curter + *curdata);
            }
        }
    }
    a = 256;
    minx -= 0.05f;
    maxx += 0.05f;
    minz -= 0.05f;
    maxz += 0.05f;
    if (platscan != 0) {
        list = CurTerr->terrlist + CurTerr->terrgroup[a].start;
        for (b = 0; b < CurTerr->terrgroup[a].count; b++, list++) {
            situ = &CurTerr->terr[*list];
            if (CurTerr->platdata[situ->info].curmtx != 0) {
                if (CurTerr->platdata[situ->info].curmtx->_30 > CurTerr->platdata[situ->info].oldmtx._30) {
                    tminx = minx - situ->Location.x;
                    tmaxx = (maxx + (CurTerr->platdata[situ->info].curmtx->_30 -
                              CurTerr->platdata[situ->info].oldmtx._30) * 1.5f) - situ->Location.x;
                } else {
                    tmaxx = maxx - situ->Location.x;
                    tminx = (minx + (CurTerr->platdata[situ->info].curmtx->_30 -
                              CurTerr->platdata[situ->info].oldmtx._30) * 1.5f) - situ->Location.x;
                }
                if (CurTerr->platdata[situ->info].curmtx->_32 > CurTerr->platdata[situ->info].oldmtx._32) {
                    tminz = minz - situ->Location.z;
                    tmaxz = (maxz + (CurTerr->platdata[situ->info].curmtx->_32 -
                              CurTerr->platdata[situ->info].oldmtx._32) * 1.5f) - situ->Location.z;
                } else {
                    tmaxz = maxz - situ->Location.z;
                    tminz = (minz + (CurTerr->platdata[situ->info].curmtx->_32 -
                              CurTerr->platdata[situ->info].oldmtx._32) * 1.5f) - situ->Location.z;
                }
            } else {
                tmaxx = maxx - situ->Location.x;
                tminx = minx - situ->Location.x;
                tmaxz = maxz - situ->Location.z;
                tminz = minz - situ->Location.z;
            }
            if ((tmaxx >= situ->min.x) && (tmaxz >= situ->min.z) && (tminx <= situ->max.x) &&
                (tminz < situ->max.z) && (situ->type != -1)) {
                group = (TerrainGroupType *)situ->model;
                while (group->start >= 0) {
                    c = group->count;
                    ter = (tertype *)(group + 1);
                    if ((tmaxx >= group->bounds[0]) && (tminx < group->bounds[1])) {
                        if ((tmaxz >= group->bounds[2]) && (tminz < group->bounds[3])) {
                            for (; c > 0; c--) {
                                if ((tmaxx >= ter->minx) && (tminx < ter->maxx) && (tmaxz >= ter->minz) &&
                                    (tminz < ter->maxz) && (hitdata < maxdata) &&
                                    ((ter->info[1] == 0) || ((ter->info[1] & extramask) != 0))) {
                                    *hitdata = ter;
                                    hitdata++;
                                    hitcnt++;
                                }
                                ter++;
                            }
                            group = (TerrainGroupType *)ter;
                        } else {
                            group = (TerrainGroupType *)(ter + c);
                        }
                    } else {
                        group = (TerrainGroupType *)(ter + c);
                    }
                }
                if (hitcnt != 0) {
                    lastwrite[0] = hitcnt;
                    lastwrite[1] = *list;
                    lastwrite = (short *)hitdata;
                    hitdata++;
                    hitcnt = 0;
                }
            }
        }
    }
    lastwrite[0] = 0;
    lastwrite[1] = 0;
}


void NewScanRot(struct nuvec_s *ppos, s32 extramask) {
    struct nuvec_s pos;
    s32 a;
    s32 b;
    s32 c;
    s32 id;
    s32 hitcnt;
    s32 index;
    s32 lp;
    s32 curobj;
    short *curdata;
    tertype **curter;
    tertype *ter;
    tertype **hitdata;
    tertype **maxdata;
    short *lastwrite;
    short *list;
    f32 maxx;
    f32 maxz;
    f32 tmaxx;
    f32 tmaxz;
    f32 minx;
    f32 minz;
    f32 tminx;
    f32 tminz;
    TerrainSituType *situ;
    TerrainGroupType *group;
    struct nuvec4_s rotpnts[4];
    struct nuvec4_s rotnorm[2];
    f32 dx;
    f32 dz;
    f32 mag;
    s32 scalecnt;

    pos = *ppos;
    scalecnt = 0;
    index = -1;
    b = 0x7FFFFFFF;
    id = -1;
    for (lp = 0; lp < 8; lp++) {
        if ((CurTerr->pollist[lp].timer > 0) && ((pos.x + 1.0f) - CurTerr->pollist[lp].x < 2.0f) &&
            ((pos.z + 1.0f) - CurTerr->pollist[lp].z < 2.0f) &&
            ((pos.x + 1.0f) - CurTerr->pollist[lp].x > 0.0f) &&
            ((pos.z + 1.0f) - CurTerr->pollist[lp].z > 0.0f)) {
            id = lp;
            break;
        }
        if (CurTerr->pollist[lp].timer < b) {
            b = CurTerr->pollist[lp].timer;
            index = lp;
        }
    }
    if (id >= 0) {
        lastwrite = (short *)TerI->hitdata;
        index = id;
        CurTerr->pollist[index].timer = 8;
        maxx = pos.x + 0.1f;
        maxz = pos.z + 0.1f;
        minx = pos.x - 0.1f;
        minz = pos.z - 0.1f;
        hitdata = TerI->hitdata + 1;
        maxdata = hitdata + 0x1FC;
        hitcnt = 0;
        curdata = (short *)CurTerr->pollist[index].hitdata;
        while (*curdata > 0) {
            curobj = curdata[1];
            situ = &CurTerr->terr[curobj];
            curter = (tertype **)(curdata + SCAN_SLOT);
            if ((maxx >= situ->min.x) && (maxz >= situ->min.z) && (minx <= situ->max.x) &&
                (minz < situ->max.z) && (situ->type != -1)) {
                tmaxx = maxx - situ->Location.x;
                tmaxz = maxz - situ->Location.z;
                tminx = minx - situ->Location.x;
                tminz = minz - situ->Location.z;
                for (lp = *curdata; lp > 0; lp--, curter++) {
                    ter = *curter;
                    if ((tmaxx >= ter->minx) && (tminx < ter->maxx) && (tmaxz >= ter->minz) &&
                        (tminz < ter->maxz) && (hitdata < maxdata) &&
                        ((ter->info[1] == 0) || ((ter->info[1] & extramask) != 0))) {
                        *hitdata = ter;
                        hitdata++;
                        hitcnt++;
                    }
                }
                if (hitcnt != 0) {
                    lastwrite[0] = hitcnt;
                    lastwrite[1] = curobj;
                    lastwrite = (short *)hitdata;
                    hitdata++;
                    hitcnt = 0;
                }
                curdata = (short *)curter;
            } else {
                curdata = (short *)(curter + *curdata);
            }
        }
    } else {
        TerI->hitcnt = 0;
        lastwrite = (short *)CurTerr->pollist[index].hitdata;
        hitdata = CurTerr->pollist[index].hitdata + 1;
        maxdata = hitdata + 0x1FC;
        hitcnt = 0;
        maxx = pos.x + 1.0f;
        maxz = pos.z + 1.0f;
        minx = pos.x - 1.0f;
        minz = pos.z - 1.0f;
        for (a = 0; a < CurTerr->terrgroupcnt; a++) {
            if ((maxx >= CurTerr->terrgroup[a].bounds[0]) && (maxz >= CurTerr->terrgroup[a].bounds[1]) &&
                (minx <= CurTerr->terrgroup[a].bounds[2]) && (minz <= CurTerr->terrgroup[a].bounds[3])) {
                list = CurTerr->terrlist + CurTerr->terrgroup[a].start;
                for (b = 0; b < CurTerr->terrgroup[a].count; b++, list++) {
                    situ = &CurTerr->terr[*list];
                    if ((maxx >= situ->min.x) && (maxz >= situ->min.z) && (minx <= situ->max.x) &&
                        (minz < situ->max.z) && (situ->type != -1)) {
                        tmaxx = maxx - situ->Location.x;
                        tmaxz = maxz - situ->Location.z;
                        tminx = minx - situ->Location.x;
                        tminz = minz - situ->Location.z;
                        group = (TerrainGroupType *)situ->model;
                        while (group->start >= 0) {
                            c = group->count;
                            ter = (tertype *)(group + 1);
                            if ((tmaxx >= group->bounds[0]) && (tminx < group->bounds[1])) {
                                if ((tmaxz >= group->bounds[2]) && (tminz < group->bounds[3])) {
                                    for (; c > 0; c--) {
                                        if ((tmaxx >= ter->minx) && (tminx < ter->maxx) &&
                                            (tmaxz >= ter->minz) && (tminz < ter->maxz) &&
                                            (hitdata < maxdata)) {
                                            *hitdata = ter;
                                            hitdata++;
                                            hitcnt++;
                                        }
                                        ter++;
                                    }
                                    group = (TerrainGroupType *)ter;
                                } else {
                                    group = (TerrainGroupType *)(ter + c);
                                }
                            } else {
                                group = (TerrainGroupType *)(ter + c);
                            }
                        }
                        if (hitcnt != 0) {
                            lastwrite[0] = hitcnt;
                            lastwrite[1] = *list;
                            lastwrite = (short *)hitdata;
                            hitdata++;
                            hitcnt = 0;
                        }
                    }
                }
            }
        }
        CurTerr->pollist[index].timer = 8;
        CurTerr->pollist[index].x = ppos->x;
        CurTerr->pollist[index].z = ppos->z;
        lastwrite[0] = 0;
        lastwrite[1] = 0;
        lastwrite = (short *)TerI->hitdata;
        maxx = pos.x + 0.1f;
        maxz = pos.z + 0.1f;
        minx = pos.x - 0.1f;
        minz = pos.z - 0.1f;
        hitdata = TerI->hitdata + 1;
        maxdata = hitdata + 0x1FC;
        hitcnt = 0;
        curdata = (short *)CurTerr->pollist[index].hitdata;
        while (*curdata > 0) {
            curobj = curdata[1];
            situ = &CurTerr->terr[curobj];
            curter = (tertype **)(curdata + SCAN_SLOT);
            if ((maxx >= situ->min.x) && (maxz >= situ->min.z) && (minx <= situ->max.x) &&
                (minz < situ->max.z) && (situ->type != -1)) {
                tmaxx = maxx - situ->Location.x;
                tmaxz = maxz - situ->Location.z;
                tminx = minx - situ->Location.x;
                tminz = minz - situ->Location.z;
                for (lp = *curdata; lp > 0; lp--, curter++) {
                    ter = *curter;
                    if ((tmaxx >= ter->minx) && (tminx < ter->maxx) && (tmaxz >= ter->minz) &&
                        (tminz < ter->maxz) && (hitdata < maxdata) &&
                        ((ter->info[1] == 0) || ((ter->info[1] & extramask) != 0))) {
                        *hitdata = ter;
                        hitdata++;
                        hitcnt++;
                    }
                }
                if (hitcnt != 0) {
                    lastwrite[0] = hitcnt;
                    lastwrite[1] = curobj;
                    lastwrite = (short *)hitdata;
                    hitdata++;
                    hitcnt = 0;
                }
                curdata = (short *)curter;
            } else {
                curdata = (short *)(curter + *curdata);
            }
        }
    }
    a = 256;
    minx -= 0.05f;
    maxx += 0.05f;
    minz -= 0.05f;
    maxz += 0.05f;
    list = CurTerr->terrlist + CurTerr->terrgroup[a].start;
    for (b = 0; b < CurTerr->terrgroup[a].count; b++, list++) {
        situ = &CurTerr->terr[*list];
        if (CurTerr->platdata[situ->info].curmtx != 0) {
            if (CurTerr->platdata[situ->info].curmtx->_30 > CurTerr->platdata[situ->info].oldmtx._30) {
                tminx = minx - situ->Location.x;
                tmaxx = (maxx + (CurTerr->platdata[situ->info].curmtx->_30 -
                          CurTerr->platdata[situ->info].oldmtx._30) * 1.5f) - situ->Location.x;
            } else {
                tmaxx = maxx - situ->Location.x;
                tminx = (minx + (CurTerr->platdata[situ->info].curmtx->_30 -
                          CurTerr->platdata[situ->info].oldmtx._30) * 1.5f) - situ->Location.x;
            }
            if (CurTerr->platdata[situ->info].curmtx->_32 > CurTerr->platdata[situ->info].oldmtx._32) {
                tminz = minz - situ->Location.z;
                tmaxz = (maxz + (CurTerr->platdata[situ->info].curmtx->_32 -
                          CurTerr->platdata[situ->info].oldmtx._32) * 1.5f) - situ->Location.z;
            } else {
                tmaxz = maxz - situ->Location.z;
                tminz = (minz + (CurTerr->platdata[situ->info].curmtx->_32 -
                          CurTerr->platdata[situ->info].oldmtx._32) * 1.5f) - situ->Location.z;
            }
        } else {
            tmaxx = maxx - situ->Location.x;
            tminx = minx - situ->Location.x;
            tmaxz = maxz - situ->Location.z;
            tminz = minz - situ->Location.z;
        }
        if (CurTerr->platdata[situ->info].status.bit.rotate) {
            dx = (maxx + minx) * 0.5f - situ->Location.x;
            dz = (maxz + minz) * 0.5f - situ->Location.z;
            mag = dx * dx + dz * dz;
            if ((situ->type != -1) && (mag < situ->radius + 0.00001f)) {
                group = (TerrainGroupType *)situ->model;
                while (group->start >= 0) {
                    ter = (tertype *)(group + 1);
                    for (c = group->count; c > 0; c--, ter++) {
                        if ((hitdata < maxdata) && ((ter->info[1] == 0) || ((ter->info[1] & extramask) != 0))) {
                            rotpnts[0].x = ter->pnts[0].x;
                            rotpnts[0].y = ter->pnts[0].y;
                            rotpnts[0].z = ter->pnts[0].z;
                            rotpnts[0].w = 0.0f;
                            rotpnts[1].x = ter->pnts[1].x;
                            rotpnts[1].y = ter->pnts[1].y;
                            rotpnts[1].z = ter->pnts[1].z;
                            rotpnts[1].w = 0.0f;
                            rotpnts[2].x = ter->pnts[2].x;
                            rotpnts[2].y = ter->pnts[2].y;
                            rotpnts[2].z = ter->pnts[2].z;
                            rotpnts[2].w = 0.0f;
                            NuVec4MtxTransformVU0(&rotpnts[0], &rotpnts[0], CurTerr->platdata[situ->info].curmtx);
                            NuVec4MtxTransformVU0(&rotpnts[1], &rotpnts[1], CurTerr->platdata[situ->info].curmtx);
                            NuVec4MtxTransformVU0(&rotpnts[2], &rotpnts[2], CurTerr->platdata[situ->info].curmtx);
                            rotnorm[0].x = ter->norm[0].x;
                            rotnorm[0].y = ter->norm[0].y;
                            rotnorm[0].z = ter->norm[0].z;
                            rotnorm[0].w = 0.0f;
                            NuVec4MtxTransformVU0(&rotnorm[0], &rotnorm[0], CurTerr->platdata[situ->info].curmtx);
                            if (ter->norm[1].y < 65535.0f) {
                                rotpnts[3].x = ter->pnts[3].x;
                                rotpnts[3].y = ter->pnts[3].y;
                                rotpnts[3].z = ter->pnts[3].z;
                                rotpnts[3].w = 0.0f;
                                NuVec4MtxTransformVU0(&rotpnts[3], &rotpnts[3], CurTerr->platdata[situ->info].curmtx);
                                rotnorm[1].x = ter->norm[1].x;
                                rotnorm[1].y = ter->norm[1].y;
                                rotnorm[1].z = ter->norm[1].z;
                                rotnorm[1].w = 0.0f;
                                NuVec4MtxTransformVU0(&rotnorm[1], &rotnorm[1], CurTerr->platdata[situ->info].curmtx);
                            } else {
                                rotpnts[3] = rotpnts[2];
                            }
                            if (((rotpnts[0].x > tminx) || (rotpnts[1].x > tminx) || (rotpnts[2].x > tminx) ||
                                 (rotpnts[3].x > tminx)) &&
                                ((rotpnts[0].x < tmaxx) || (rotpnts[1].x < tmaxx) || (rotpnts[2].x < tmaxx) ||
                                 (rotpnts[3].x < tmaxx)) &&
                                ((rotpnts[0].z > tminz) || (rotpnts[1].z > tminz) || (rotpnts[2].z > tminz) ||
                                 (rotpnts[3].z > tminz)) &&
                                ((rotpnts[0].z < tmaxz) || (rotpnts[1].z < tmaxz) || (rotpnts[2].z < tmaxz) ||
                                 (rotpnts[3].z < tmaxz))) {
                                ScaleTerrain[scalecnt].info[0] = ter->info[0];
                                ScaleTerrain[scalecnt].info[1] = ter->info[1];
                                ScaleTerrain[scalecnt].info[2] = ter->info[2];
                                ScaleTerrain[scalecnt].info[3] = ter->info[3];
                                ScaleTerrain[scalecnt].pnts[0].x = rotpnts[0].x;
                                ScaleTerrain[scalecnt].pnts[0].y = rotpnts[0].y;
                                ScaleTerrain[scalecnt].pnts[0].z = rotpnts[0].z;
                                ScaleTerrain[scalecnt].pnts[1].x = rotpnts[1].x;
                                ScaleTerrain[scalecnt].pnts[1].y = rotpnts[1].y;
                                ScaleTerrain[scalecnt].pnts[1].z = rotpnts[1].z;
                                ScaleTerrain[scalecnt].pnts[2].x = rotpnts[2].x;
                                ScaleTerrain[scalecnt].pnts[2].y = rotpnts[2].y;
                                ScaleTerrain[scalecnt].pnts[2].z = rotpnts[2].z;
                                if (ter->norm[1].y < 65535.0f) {
                                    ScaleTerrain[scalecnt].pnts[3].x = rotpnts[3].x;
                                    ScaleTerrain[scalecnt].pnts[3].y = rotpnts[3].y;
                                    ScaleTerrain[scalecnt].pnts[3].z = rotpnts[3].z;
                                    mag = 1.0f / NuFsqrt(rotnorm[1].x * rotnorm[1].x + rotnorm[1].y * rotnorm[1].y +
                                                         rotnorm[1].z * rotnorm[1].z);
                                    ScaleTerrain[scalecnt].norm[1].x = rotnorm[1].x * mag;
                                    ScaleTerrain[scalecnt].norm[1].y = rotnorm[1].y * mag;
                                    ScaleTerrain[scalecnt].norm[1].z = rotnorm[1].z * mag;
                                } else {
                                    ScaleTerrain[scalecnt].norm[1].y = 65536.0f;
                                }
                                mag = 1.0f / NuFsqrt(rotnorm[0].x * rotnorm[0].x + rotnorm[0].y * rotnorm[0].y +
                                                     rotnorm[0].z * rotnorm[0].z);
                                ScaleTerrain[scalecnt].norm[0].x = rotnorm[0].x * mag;
                                ScaleTerrain[scalecnt].norm[0].y = rotnorm[0].y * mag;
                                ScaleTerrain[scalecnt].norm[0].z = rotnorm[0].z * mag;
                                *hitdata = &ScaleTerrain[scalecnt];
                                hitdata++;
                                hitcnt++;
                                scalecnt++;
                            }
                        }
                    }
                    group = (TerrainGroupType *)ter;
                }
            } else {
                continue;
            }
        } else if ((tmaxx >= situ->min.x) && (tmaxz >= situ->min.z) && (tminx <= situ->max.x) &&
                   (tminz < situ->max.z) && (situ->type != -1)) {
            group = (TerrainGroupType *)situ->model;
            while (group->start >= 0) {
                c = group->count;
                ter = (tertype *)(group + 1);
                if ((tmaxx >= group->bounds[0]) && (tminx < group->bounds[1])) {
                    if ((tmaxz >= group->bounds[2]) && (tminz < group->bounds[3])) {
                        for (; c > 0; c--) {
                            if ((tmaxx >= ter->minx) && (tminx < ter->maxx) && (tmaxz >= ter->minz) &&
                                (tminz < ter->maxz) && (hitdata < maxdata) &&
                                ((ter->info[1] == 0) || ((ter->info[1] & extramask) != 0))) {
                                *hitdata = ter;
                                hitdata++;
                                hitcnt++;
                            }
                            ter++;
                        }
                        group = (TerrainGroupType *)ter;
                    } else {
                        group = (TerrainGroupType *)(ter + c);
                    }
                } else {
                    group = (TerrainGroupType *)(ter + c);
                }
            }
        } else {
            continue;
        }
        if (hitcnt != 0) {
            lastwrite[0] = hitcnt;
            lastwrite[1] = *list;
            lastwrite = (short *)hitdata;
            hitdata++;
            hitcnt = 0;
        }
    }
    lastwrite[0] = 0;
    lastwrite[1] = 0;
}


//NGC MATCH
float NewCast(struct nuvec_s *pos, float ytol) {
    short *CurData;
    tertype **curter;
    tertype *ter;
    short castnum2;
    short ecastnum2;
    float ht;
    float ht2;
    struct nuvec_s norm;
    struct nuvec_s norm2;
    tertype *terhit1;
    tertype *terhit2;
    float eht;
    float eht2;
    struct nuvec_s enorm;
    struct nuvec_s enorm2;
    tertype *eterhit1;
    tertype *eterhit2;
    s32 objnum;
    s32 lp;
    s32 t;
    s32 line;
    float tx;
    float ty;
    float tz;

    ht = -2000000.0f;
    ht2 = 2000000.0f;
    terhit1 = 0;
    terhit2 = 0;
    eht = -2000000.0f;
    eht2 = 2000000.0f;
    eterhit1 = 0;
    eterhit2 = 0;
    tx = 0.0f;
    tz = 0.0f;
    castnum2 = -1;
    castnum = -1;
    ecastnum2 = -1;
    ecastnum = -1;
    CurData = (short *)TerI->hitdata;
    while (*(short *)CurData > 0) {
        tx = pos->x - CurTerr->terr[CurData[1]].Location.x;
        tz = pos->z - CurTerr->terr[CurData[1]].Location.z;
        objnum = CurData[1];
        curter = (tertype **)(CurData + SCAN_SLOT);
        lp = *(short *)CurData;
        for (; lp > 0; lp--, curter++) {
            ter = (tertype *)*curter;
            if (ter->norm[1].y > 65535.0f) {
                t = 0;
                if (ter->norm[0].y > 0.0f) {
                    if (((INSIDELINEF(tx, tz, ter->pnts[1].x, ter->pnts[1].z, ter->pnts[0].x, ter->pnts[0].z) != 0)
                         && (INSIDELINEF(tx, tz, ter->pnts[0].x, ter->pnts[0].z, ter->pnts[2].x, ter->pnts[2].z) != 0))
                        && (INSIDELINEF(tx, tz, ter->pnts[2].x, ter->pnts[2].z, ter->pnts[1].x, ter->pnts[1].z) != 0))
                    {
                        t = 1;
                    }
                } else {
                    if ((((ter->norm[0].y < 0.0f)
                          && (INSIDELINEF(tx, tz, ter->pnts[0].x, ter->pnts[0].z, ter->pnts[1].x, ter->pnts[1].z) != 0))
                         && (INSIDELINEF(tx, tz, ter->pnts[2].x, ter->pnts[2].z, ter->pnts[0].x, ter->pnts[0].z) != 0))
                        && (INSIDELINEF(tx, tz, ter->pnts[1].x, ter->pnts[1].z, ter->pnts[2].x, ter->pnts[2].z) != 0))
                    {
                        t = 2;
                    }
                }
                if (t != 0) {
                    ty = (ter->norm[0].x * (tx - ter->pnts[0].x) + ter->norm[0].z * (tz - ter->pnts[0].z))
                        / -ter->norm[0].y;
                    ty += ter->pnts[0].y + CurTerr->terr[objnum].Location.y;
                    if (ter->info[1] != 0) {
                        if (((ty <= pos->y) && (ty > eht)) && (t == 1)) {
                            eht = ty;
                            eterhit1 = ter;
                            enorm = ter->norm[0];
                            ecastnum = objnum;
                        } else {
                            if ((((ty > pos->y) && (ty <= eht2)) && (ty < pos->y + ytol))
                                && ((ty != eht2 || (ter->norm[0].y < enorm2.y))))
                            {
                                eterhit2 = ter;
                                enorm2 = ter->norm[0];
                                eht2 = ty;
                                ecastnum2 = objnum;
                            }
                        }
                    } else if (((ty <= pos->y) && (ty > ht)) && (t == 1)) {
                        ht = ty;
                        terhit1 = ter;
                        norm = ter->norm[0];
                        castnum = objnum;
                    } else {
                        if ((((ty > pos->y) && (ty <= ht2)) && (ty < pos->y + ytol))
                            && ((ty != ht2 || (ter->norm[0].y < norm2.y))))
                        {
                            terhit2 = ter;
                            norm2 = ter->norm[0];
                            ht2 = ty;
                            castnum2 = objnum;
                        }
                    }
                }
            } else {
                t = 0;
                if (ter->norm[0].y > 0.0f) {
                    if (((INSIDELINEF(tx, tz, ter->pnts[1].x, ter->pnts[1].z, ter->pnts[0].x, ter->pnts[0].z) != 0)
                         && (INSIDELINEF(tx, tz, ter->pnts[0].x, ter->pnts[0].z, ter->pnts[2].x, ter->pnts[2].z) != 0))
                        && ((INSIDELINEF(tx, tz, ter->pnts[3].x, ter->pnts[3].z, ter->pnts[1].x, ter->pnts[1].z) != 0
                             && (INSIDELINEF(tx, tz, ter->pnts[2].x, ter->pnts[2].z, ter->pnts[3].x, ter->pnts[3].z) != 0))))
                    {
                        t = 1;
                    }
                } else {
                    if ((((ter->norm[0].y < 0.0f)
                          && (INSIDELINEF(tx, tz, ter->pnts[0].x, ter->pnts[0].z, ter->pnts[1].x, ter->pnts[1].z) != 0))
                         && (INSIDELINEF(tx, tz, ter->pnts[2].x, ter->pnts[2].z, ter->pnts[0].x, ter->pnts[0].z) != 0))
                        && ((INSIDELINEF(tx, tz, ter->pnts[1].x, ter->pnts[1].z, ter->pnts[3].x, ter->pnts[3].z) != 0
                             && (INSIDELINEF(tx, tz, ter->pnts[3].x, ter->pnts[3].z, ter->pnts[2].x, ter->pnts[2].z) != 0))))
                    {
                        t = 2;
                    }
                }
                if (t != 0) {
                    if (t == 1) {
                        line = INSIDELINEF(tx, tz, ter->pnts[2].x, ter->pnts[2].z, ter->pnts[1].x, ter->pnts[1].z);
                    } else {
                        line = INSIDELINEF(tx, tz, ter->pnts[1].x, ter->pnts[1].z, ter->pnts[2].x, ter->pnts[2].z);
                    }
                    if ((line != 0) || (ter->norm[1].y == 0.0f)) {
                        ty = (ter->norm[0].x * (tx - ter->pnts[0].x) + ter->norm[0].z * (tz - ter->pnts[0].z))
                            / -ter->norm[0].y;
                        ty += ter->pnts[0].y + CurTerr->terr[objnum].Location.y;
                        if (ter->info[1] != 0) {
                            if (((ty <= pos->y) && (ty > eht)) && (t == 1)) {
                                eterhit1 = ter;
                                eht = ty;
                                enorm = ter->norm[0];
                                ecastnum = objnum;
                            } else {
                                if (((ty > pos->y) && (ty <= eht2))
                                    && ((ty < pos->y + ytol && ((ty != eht2 || (ter->norm[0].y < enorm2.y))))))
                                {
                                    eterhit2 = ter;
                                    enorm2 = ter->norm[0];
                                    eht2 = ty;
                                    ecastnum2 = objnum;
                                }
                            }
                        } else if (((ty <= pos->y) && (ty > ht)) && (t == 1)) {
                            terhit1 = ter;
                            ht = ty;
                            norm = ter->norm[0];
                            castnum = objnum;
                        } else {
                            if (((ty > pos->y) && (ty <= ht2))
                                && ((ty < pos->y + ytol && ((ty != ht2 || (ter->norm[0].y < norm2.y))))))
                            {
                                terhit2 = ter;
                                norm2 = ter->norm[0];
                                ht2 = ty;
                                castnum2 = objnum;
                            }
                        }
                    } else {
                        ty = (ter->norm[1].x * (tx - ter->pnts[3].x) + ter->norm[1].z * (tz - ter->pnts[3].z))
                            / -ter->norm[1].y;
                        ty += ter->pnts[3].y + CurTerr->terr[objnum].Location.y;
                        if (ter->info[1] != 0) {
                            if (((ty <= pos->y) && (ty > eht)) && (t == 1)) {
                                eterhit1 = ter;
                                eht = ty;
                                enorm = ter->norm[1];
                                ecastnum = objnum;
                            } else {
                                if (((ty > pos->y) && (ty <= eht2))
                                    && ((ty < pos->y + ytol && ((ty != eht2 || (ter->norm[1].y < enorm2.y))))))
                                {
                                    eterhit2 = ter;
                                    enorm2 = ter->norm[1];
                                    eht2 = ty;
                                    ecastnum2 = objnum;
                                }
                            }
                        } else if (((ty <= pos->y) && (ty > ht)) && (t == 1)) {
                            terhit1 = ter;
                            ht = ty;
                            norm = ter->norm[1];
                            castnum = objnum;
                        } else {
                            if (((ty > pos->y) && (ty <= ht2))
                                && ((ty < pos->y + ytol && ((ty != ht2 || (ter->norm[1].y < norm2.y))))))
                            {
                                terhit2 = ter;
                                norm2 = ter->norm[1];
                                ht2 = ty;
                                castnum2 = objnum;
                            }
                        }
                    }
                }
            }
        }
        CurData = (short *)curter;
    }
    if ((eht2 < 2000000.0f) && (enorm2.y < 0.0f)) {
        eshadroofhit = 1;
        EShadRoofY = eht2;
        EShadRoofNorm.x = enorm2.x;
        EShadRoofNorm.y = enorm2.y;
        EShadRoofNorm.z = enorm2.z;
        EShadRoofPoly = eterhit2;
        ecastroofnum = ecastnum2;
    } else {
        EShadRoofY = 2000000.0f;
        EShadRoofPoly = 0;
    }
    if ((eht2 < 2000000.0f) && (enorm2.y > 0.0f)) {
        eshadhit = 1;
        EShadY = eht2;
        EShadNorm.x = enorm2.x;
        EShadNorm.y = enorm2.y;
        EShadNorm.z = enorm2.z;
        EShadPoly = eterhit2;
        ecastnum = ecastnum2;
    } else {
        if (eht > -2000000.0f) {
            eshadhit = 2;
            EShadY = eht;
            EShadNorm.x = enorm.x;
            EShadNorm.y = enorm.y;
            EShadNorm.z = enorm.z;
            EShadPoly = eterhit1;
        } else {
            eshadhit = 3;
            EShadY = 2000000.0f;
            EShadNorm.y = 1.0f;
            EShadPoly = 0;
        }
    }
    if ((ht2 < 2000000.0f) && (norm2.y < 0.0f)) {
        shadroofhit = 1;
        ShadRoofY = ht2;
        ShadRoofNorm.x = norm2.x;
        ShadRoofNorm.y = norm2.y;
        ShadRoofNorm.z = norm2.z;
        ShadRoofPoly = terhit2;
        castroofnum = castnum2;
    } else {
        ShadRoofY = 2000000.0f;
        ShadRoofPoly = 0;
    }
    if ((ht2 < 2000000.0f) && (norm2.y > 0.0f)) {
        shadhit = 1;
        pos->y = ht2;
        ShadNorm.x = norm2.x;
        ShadNorm.y = norm2.y;
        ShadNorm.z = norm2.z;
        ShadPoly = terhit2;
        castnum = castnum2;
    } else {
        if (ht > -2000000.0f) {
            shadhit = 2;
            ShadNorm.x = norm.x;
            ShadNorm.y = norm.y;
            ShadNorm.z = norm.z;
            ShadPoly = terhit1;
            pos->y = ht;
        } else {
            shadhit = 3;
            pos->y = 2000000.0f;
            ShadNorm.y = 1.0f;
            ShadPoly = 0;
        }
    }
    return 0.0f;
}

s32 CheckCylinder(s32 p0, s32 p1, s32 *flags, s32 type) {
    struct nuvec_s perp;
    struct nuvec_s dir;
    struct nuvec_s nrm;
    struct nuvec_s v;
    f32 lensq;
    f32 c;
    f32 distsq;
    f32 dirlen;
    f32 a;
    f32 t;
    f32 b;

    if (((TerI->rotter.pnts[p0].x > TerI->size) && (TerI->rotter.pnts[p1].x > TerI->size)) ||
        ((TerI->rotter.pnts[p0].x < -TerI->size) && (TerI->rotter.pnts[p1].x < -TerI->size)) ||
        ((TerI->rotter.pnts[p0].y > TerI->size) && (TerI->rotter.pnts[p1].y > TerI->size)) ||
        ((TerI->rotter.pnts[p0].y < -TerI->size) && (TerI->rotter.pnts[p1].y < -TerI->size)) ||
        ((TerI->rotter.pnts[p0].z < -TerI->size) && (TerI->rotter.pnts[p1].z < -TerI->size)) ||
        ((TerI->rotter.pnts[p0].z > TerI->size + TerI->len) &&
         (TerI->rotter.pnts[p1].z > TerI->size + TerI->len))) {
        *flags &= type;
        return 0;
    }
    dir.x = TerI->rotter.pnts[p1].x - TerI->rotter.pnts[p0].x;
    dir.y = TerI->rotter.pnts[p1].y - TerI->rotter.pnts[p0].y;
    lensq = dir.x * dir.x + dir.y * dir.y;
    if (lensq < 1e-12f) {
        if (((TerI->rotter.pnts[p0].z <= 0.0f) && (TerI->rotter.pnts[p1].z >= 0.0f)) ||
            ((TerI->rotter.pnts[p0].z >= 0.0f) && (TerI->rotter.pnts[p1].z <= 0.0f))) {
            lensq = TerI->rotter.pnts[p1].x * TerI->rotter.pnts[p1].x +
                    TerI->rotter.pnts[p1].y * TerI->rotter.pnts[p1].y;
            if (lensq <= TerI->sizesq) {
                lensq = 1.0f / NuFsqrt(lensq);
                TerI->hittype = 0x12;
                TerI->hittime = 0.0f;
                TerI->hitnorm.x = -TerI->rotter.pnts[p1].x * lensq;
                TerI->hitnorm.y = -TerI->rotter.pnts[p1].y * lensq;
                TerI->hitnorm.z = 0.0f;
                return 1;
            }
        }
        return 0;
    }
    c = TerI->rotter.pnts[p0].x * dir.y - TerI->rotter.pnts[p0].y * dir.x;
    if (c * c > TerI->sizesq * lensq) {
        *flags &= type;
        return 0;
    }
    distsq = c * c / lensq;
    dir.z = TerI->rotter.pnts[p1].z - TerI->rotter.pnts[p0].z;
    dirlen = NuFsqrt(lensq + dir.z * dir.z);
    lensq = 1.0f / dirlen;
    dir.x *= lensq;
    dir.y *= lensq;
    dir.z *= lensq;
    perp.x = -dir.y;
    perp.y = dir.x;
    c = perp.x * perp.x + perp.y * perp.y;
    v.x = -TerI->rotter.pnts[p0].x;
    v.y = -TerI->rotter.pnts[p0].y;
    v.z = -TerI->rotter.pnts[p0].z;
    NuVecCross(&v, &v, &dir);
    a = -(v.x * perp.x + v.y * perp.y) / c;
    nrm.x = perp.y * dir.z;
    nrm.y = -(perp.x * dir.z);
    nrm.z = perp.x * dir.y - perp.y * dir.x;
    nrm.z = nrm.z / NuFsqrt(nrm.x * nrm.x + nrm.y * nrm.y + nrm.z * nrm.z);
    b = NuFsqrt(TerI->sizesq - distsq) / nrm.z;
    if (b < 0.0f) {
        a += b;
    } else {
        a -= b;
    }
    if ((a >= 0.0f) && (a <= TerI->len)) {
        c = dir.x * -TerI->rotter.pnts[p0].x + dir.y * -TerI->rotter.pnts[p0].y +
            dir.z * (a - TerI->rotter.pnts[p0].z);
        if ((c > 0.0f) && (c <= dirlen)) {
            t = a / TerI->len;
            if (t < TerI->hittime) {
                TerI->hittime = t;
                TerI->hittype = 2;
                TerI->hitnorm.x = -(TerI->rotter.pnts[p0].x + dir.x * c);
                TerI->hitnorm.y = -(TerI->rotter.pnts[p0].y + dir.y * c);
                TerI->hitnorm.z = a - (TerI->rotter.pnts[p0].z + dir.z * c);
                *flags &= type;
                return 1;
            }
            *flags &= type;
            return 0;
        }
    }
    if (((TerI->rotter.pnts[p0].z >= -TerI->size) || (TerI->rotter.pnts[p1].z >= -TerI->size)) &&
        ((TerI->rotter.pnts[p0].z <= TerI->size) || (TerI->rotter.pnts[p1].z <= TerI->size))) {
        lensq = -dir.x * TerI->rotter.pnts[p0].x - dir.y * TerI->rotter.pnts[p0].y -
            dir.z * TerI->rotter.pnts[p0].z;
        c = dir.x * (TerI->rotter.pnts[p1].x - TerI->rotter.pnts[p0].x) +
            dir.y * (TerI->rotter.pnts[p1].y - TerI->rotter.pnts[p0].y) +
            dir.z * (TerI->rotter.pnts[p1].z - TerI->rotter.pnts[p0].z);
        if (lensq < 0.0f) {
            return 0;
        }
        if (c < lensq) {
            return 0;
        }
        dir.x = dir.x * lensq + TerI->rotter.pnts[p0].x;
        dir.y = dir.y * lensq + TerI->rotter.pnts[p0].y;
        dir.z = dir.z * lensq + TerI->rotter.pnts[p0].z;
        lensq = dir.x * dir.x + dir.y * dir.y + dir.z * dir.z;
        if (lensq < TerI->sizesq) {
            lensq = 1.0f / NuFsqrt(lensq);
            TerI->hittype = 0x12;
            TerI->hittime = 0.0f;
            TerI->hitnorm.x = -dir.x * lensq;
            TerI->hitnorm.y = -dir.y * lensq;
            TerI->hitnorm.z = -dir.z * lensq;
            return 1;
        }
    }
    return 0;
}


// NGC MATCH (PS2 layout/control flow verified against 0x00176270)
s32 CheckSphere(s32 p) {
    float a;
    float b;

    if ((TerI->rotter.pnts[p].z < -TerI->size) ||
        (TerI->rotter.pnts[p].z > TerI->size + TerI->len)) {
        return 0;
    }

    b = TerI->rotter.pnts[p].x * TerI->rotter.pnts[p].x +
        TerI->rotter.pnts[p].y * TerI->rotter.pnts[p].y;
    if (b > TerI->sizesq) {
        return 0;
    }

    a = NuFsqrt(TerI->sizesq - b);
    if ((TerI->rotter.pnts[p].z - a < 0.0f) ||
        (TerI->rotter.pnts[p].z - a > TerI->len)) {
        b += TerI->rotter.pnts[p].z * TerI->rotter.pnts[p].z;
        if (b < TerI->sizesq) {
            a = 1.0f / NuFsqrt(b);
            TerI->hittype = 0x13;
            TerI->hittime = 0.0f;
            TerI->hitnorm.x = -TerI->rotter.pnts[p].x * a;
            TerI->hitnorm.y = -TerI->rotter.pnts[p].y * a;
            TerI->hitnorm.z = -TerI->rotter.pnts[p].z * a;
            return 1;
        }
        return 0;
    }

    b = (TerI->rotter.pnts[p].z - a) / TerI->len;
    if (b < TerI->hittime) {
        TerI->hittype = 3;
        TerI->hittime = b;
        TerI->hitnorm.x = -TerI->rotter.pnts[p].x;
        TerI->hitnorm.y = -TerI->rotter.pnts[p].y;
        TerI->hitnorm.z = -a;
        return 1;
    }
    return 0;
}

// NGC MATCH (PS2 layout/control flow verified against 0x00176428)
s32 CheckSphereTer(struct nuvec_s *pnt, f32 radius) {
    f32 a;
    f32 b;
    f32 rsq;

    if ((pnt->z < -TerI->size - radius) ||
        (pnt->z > TerI->size + radius + TerI->len)) {
        return 0;
    }

    b = pnt->x * pnt->x + pnt->y * pnt->y;
    rsq = (TerI->size + radius) * (TerI->size + radius);
    if (b > rsq) {
        return 0;
    }

    a = NuFsqrt(rsq - b);
    if ((pnt->z - a < 0.0f) || (pnt->z - a > TerI->len)) {
        b += pnt->z * pnt->z;
        if (b < rsq) {
            a = 1.0f / NuFsqrt(b);
            TerI->hittype = 0x14;
            TerI->hittime = 0.0f;
            TerI->hitnorm.x = -pnt->x * a;
            TerI->hitnorm.y = -pnt->y * a;
            TerI->hitnorm.z = -pnt->z * a;
            return 1;
        }
        return 0;
    }

    b = (pnt->z - a) / TerI->len;
    if (b < TerI->hittime) {
        TerI->hittype = 4;
        TerI->hittime = b;
        TerI->hitnorm.x = -pnt->x;
        TerI->hitnorm.y = -pnt->y;
        TerI->hitnorm.z = -a;
        return 1;
    }
    return 0;
}

/* PS2 layout and control flow reconstructed from 0x001765c8. */
s32 HitPoly(f32 ps, f32 pe, f32 ps2, f32 pe2, tertype *ter) {
    struct nuvec_s hitpos;
    f32 time;
    s32 hit;
    s32 check;
    s32 spherechecks;

    hit = 0;
    hitpos.x = (TerI->csx - TerI->size * ter->norm[0].x) - ter->pnts[0].x;
    hitpos.y = (TerI->csy - TerI->size * ter->norm[0].y) - ter->pnts[0].y;
    hitpos.z = (TerI->csz - TerI->size * ter->norm[0].z) - ter->pnts[0].z;
    {
        check = 0;
        if ((ps > 0.0f) && (pe < 0.0f)) {
            time = ps / (ps - pe);
            hitpos.x = hitpos.x + TerI->curvel.x * time;
            hitpos.y = hitpos.y + TerI->curvel.y * time;
            hitpos.z = hitpos.z + TerI->curvel.z * time;
            check = 1;
        } else {
            if ((ps < 0.0f) && (pe < 0.0f)) {
                time = 1e-9f;
                hitpos.x = hitpos.x + -ps * ter->norm[0].x;
                hitpos.y = hitpos.y + -ps * ter->norm[0].y;
                hitpos.z = hitpos.z + -ps * ter->norm[0].z;
                check = 1;
            }
        }
        if (check) {
            if ((InsidePolLines(hitpos.x, hitpos.y, hitpos.z,
                                ter->pnts[1].x - ter->pnts[0].x,
                                ter->pnts[1].y - ter->pnts[0].y,
                                ter->pnts[1].z - ter->pnts[0].z,
                                ter->pnts[2].x - ter->pnts[0].x,
                                ter->pnts[2].y - ter->pnts[0].y,
                                ter->pnts[2].z - ter->pnts[0].z,
                                &ter->norm[0]) != 0) &&
                (time <= TerI->hittime)) {
                if (ps > 0.0f) {
                    TerI->hittype = 1;
                } else {
                    TerI->hittype = 0x11;
                }
                TerI->hittime = time;
                TerI->hitnorm.x = ter->norm[0].x;
                TerI->hitnorm.y = ter->norm[0].y;
                TerI->hitnorm.z = ter->norm[0].z;
                hit = 1;
            }
        }
    }

    if (ter->norm[1].y < 65536.0f) {
        hitpos.x = (TerI->csx - TerI->size * ter->norm[1].x) - ter->pnts[3].x;
        hitpos.y = (TerI->csy - TerI->size * ter->norm[1].y) - ter->pnts[3].y;
        hitpos.z = (TerI->csz - TerI->size * ter->norm[1].z) - ter->pnts[3].z;
        check = 0;
        if ((ps2 > 0.0f) && (pe2 < 0.0f)) {
            time = ps2 / (ps2 - pe2);
            hitpos.x = hitpos.x + TerI->curvel.x * time;
            hitpos.y = hitpos.y + TerI->curvel.y * time;
            hitpos.z = hitpos.z + TerI->curvel.z * time;
            check = 1;
        } else {
            if ((ps2 < 0.0f) && (pe2 < 0.0f)) {
                time = 1e-9f;
                hitpos.x = hitpos.x + -ps2 * ter->norm[1].x;
                hitpos.y = hitpos.y + -ps2 * ter->norm[1].y;
                hitpos.z = hitpos.z + -ps2 * ter->norm[1].z;
                check = 1;
            }
        }
        if (check) {
            if ((InsidePolLines(hitpos.x, hitpos.y, hitpos.z,
                                ter->pnts[2].x - ter->pnts[3].x,
                                ter->pnts[2].y - ter->pnts[3].y,
                                ter->pnts[2].z - ter->pnts[3].z,
                                ter->pnts[1].x - ter->pnts[3].x,
                                ter->pnts[1].y - ter->pnts[3].y,
                                ter->pnts[1].z - ter->pnts[3].z,
                &ter->norm[1]) != 0) &&
                (time < TerI->hittime)) {
                if (ps2 > 0.0f) {
                    TerI->hittype = 1;
                    TerI->hittime = time;
                    TerI->hitnorm.x = ter->norm[1].x;
                    TerI->hitnorm.y = ter->norm[1].y;
                    TerI->hitnorm.z = ter->norm[1].z;
                } else {
                    TerI->hittype = 0x11;
                    TerI->hittime = time;
                    TerI->hitnorm.x = ter->norm[1].x;
                    TerI->hitnorm.y = ter->norm[1].y;
                    TerI->hitnorm.z = ter->norm[1].z;
                }
                hit = 1;
            }
        }
    }

    if (TerI->size != 0.0f) {
        spherechecks = 0xF;
        DeRotateTerrain(ter);
        hit = hit | CheckCylinder(0, 1, &spherechecks, 12);
        hit = hit | CheckCylinder(1, 2, &spherechecks, 9);
        hit = hit | CheckCylinder(2, 0, &spherechecks, 10);
        if (ter->norm[1].y < 65536.0f) {
            hit = hit | CheckCylinder(1, 3, &spherechecks, 5);
            hit = hit | CheckCylinder(3, 2, &spherechecks, 3);
            if ((spherechecks & 8) != 0) {
                hit = hit | CheckSphere(3);
            }
        }
        if ((spherechecks & 1) != 0) {
            hit = hit | CheckSphere(0);
        }
        if ((spherechecks & 2) != 0) {
            hit = hit | CheckSphere(1);
        }
        if ((spherechecks & 4) != 0) {
            hit = hit | CheckSphere(2);
        }
    }
    if (hit != 0) {
        TerI->hitter = ter;
    }
    return hit;
}

s32 HitWallSpline(void) {
    struct nuvec_s norm;
    struct nuvec_s dir;
    struct nuvec_s hitpos;
    s32 lp;
    f32 ps;
    f32 pe;
    f32 time;
    f32 t;
    f32 dist;
    f32 radsq;
    f32 hit;

    norm.y = 0.0f;
    if (WallSplCount == 0) {
        return 0;
    }
    hit = 0.0f;
    for (lp = 0; lp < WallSplCount; lp += 2) {
        if ((DABS(TerI->curpos.x - WallSplList[lp].x) < 64.0) &&
            (DABS(TerI->curpos.z - WallSplList[lp].z) < 64.0)) {
            norm.x = WallSplList[lp + 1].z - WallSplList[lp].z;
            norm.z = WallSplList[lp].x - WallSplList[lp + 1].x;
            NuVecNorm(&norm, &norm);
            ps = norm.x * (TerI->curpos.x - WallSplList[lp].x) +
                 norm.z * (TerI->curpos.z - WallSplList[lp].z) - TerI->size;
            pe = norm.x * (TerI->curpos.x + TerI->curvel.x - WallSplList[lp].x) +
                 norm.z * (TerI->curpos.z + TerI->curvel.z - WallSplList[lp].z) - TerI->size;
            if (((pe < 0.0f) || (ps < 0.0f)) && (ps > -TerI->size)) {
                dir.x = WallSplList[lp + 1].x - WallSplList[lp].x;
                dir.z = WallSplList[lp + 1].z - WallSplList[lp].z;
                if (ps < 0.0f) {
                    t = 1e-9f;
                    hitpos.x = TerI->curpos.x - norm.x * ps - TerI->size * norm.x;
                    hitpos.z = TerI->curpos.z - norm.z * ps - TerI->size * norm.z;
                } else {
                    t = ps / (ps - pe);
                    hitpos.x = TerI->curpos.x + TerI->curvel.x * t - TerI->size * norm.x;
                    hitpos.z = TerI->curpos.z + TerI->curvel.z * t - TerI->size * norm.z;
                }
                dist = dir.x * (hitpos.x - WallSplList[lp].x) + dir.z * (hitpos.z - WallSplList[lp].z);
                if ((dist >= -0.00005f) &&
                    (-dir.x * (hitpos.x - WallSplList[lp + 1].x) - dir.z * (hitpos.z - WallSplList[lp + 1].z) >=
                     -0.00005f) &&
                    (t < TerI->hittime)) {
                    if (ps > 0.0f) {
                        TerI->hittype = 1;
                    } else {
                        TerI->hittype = 0x11;
                    }
                    hit = 1.0f;
                    TerI->hittime = t;
                    TerI->hitnorm.x = norm.x;
                    TerI->hitnorm.y = norm.y;
                    TerI->hitnorm.z = norm.z;
                    TerI->hitterrno = -1;
                }
            }
            radsq = (TerI->size + 0.005f + TerI->vellen) * (TerI->size + 0.005f + TerI->vellen);
            if ((DABS(WallSplList[lp].x - TerI->curpos.x) < 64.0) &&
                (DABS(WallSplList[lp].z - TerI->curpos.z) < 64.0) &&
                ((WallSplList[lp].x - TerI->curpos.x) * (WallSplList[lp].x - TerI->curpos.x) +
                 (WallSplList[lp].z - TerI->curpos.z) * (WallSplList[lp].z - TerI->curpos.z) < radsq)) {
                dir.x = TerI->curvel.x;
                dir.y = 0.0f;
                dir.z = TerI->curvel.z;
                NuVecNorm(&dir, &dir);
                dist = dir.x * (WallSplList[lp].x - TerI->curpos.x) + dir.z * (WallSplList[lp].z - TerI->curpos.z);
                if (dist >= 0.0f) {
                    hitpos.x = TerI->curpos.x + dir.x * dist;
                    hitpos.z = TerI->curpos.z + dir.z * dist;
                    if ((WallSplList[lp].x - hitpos.x) * (WallSplList[lp].x - hitpos.x) +
                        (WallSplList[lp].z - hitpos.z) * (WallSplList[lp].z - hitpos.z) <=
                        (TerI->size + 0.0005f) * (TerI->size + 0.0005f)) {
                        dist -= NuFsqrt(TerI->size * TerI->size -
                                        (WallSplList[lp].x - hitpos.x) * (WallSplList[lp].x - hitpos.x) -
                                        (WallSplList[lp].z - hitpos.z) * (WallSplList[lp].z - hitpos.z));
                        if ((dist >= -0.0005f) &&
                            (dist * dist <= TerI->curvel.x * TerI->curvel.x + TerI->curvel.z * TerI->curvel.z)) {
                            time = dist / NuFsqrt(TerI->curvel.x * TerI->curvel.x + TerI->curvel.z * TerI->curvel.z);
                            if (time < 0.0f) {
                                time = 0.0f;
                            }
                            if (time < TerI->hittime) {
                                norm.x = TerI->curpos.x + dir.x * dist - WallSplList[lp].x;
                                norm.z = TerI->curpos.z + dir.z * dist - WallSplList[lp].z;
                                hit = 1.0f;
                                NuVecNorm(&norm, &norm);
                                TerI->hittype = 1;
                                TerI->hittime = time;
                                TerI->hitnorm.x = norm.x;
                                TerI->hitnorm.y = norm.y;
                                TerI->hitnorm.z = norm.z;
                                TerI->hitterrno = -1;
                            }
                        } else if ((WallSplList[lp].x - TerI->curpos.x) * (WallSplList[lp].x - TerI->curpos.x) +
                                   (WallSplList[lp].z - TerI->curpos.z) * (WallSplList[lp].z - TerI->curpos.z) <=
                                   (TerI->size + 0.0005f) * (TerI->size + 0.0005f)) {
                            norm.x = TerI->curpos.x - WallSplList[lp].x;
                            norm.z = TerI->curpos.z - WallSplList[lp].z;
                            hit = 1.0f;
                            NuVecNorm(&norm, &norm);
                            TerI->hittype = 0x11;
                            TerI->hittime = 0.0f;
                            TerI->hitnorm.x = norm.x;
                            TerI->hitnorm.y = norm.y;
                            TerI->hitnorm.z = norm.z;
                        }
                    } else if ((WallSplList[lp].x - TerI->curpos.x) * (WallSplList[lp].x - TerI->curpos.x) +
                               (WallSplList[lp].z - TerI->curpos.z) * (WallSplList[lp].z - TerI->curpos.z) <=
                               (TerI->size + 0.0005f) * (TerI->size + 0.0005f)) {
                        norm.x = TerI->curpos.x - WallSplList[lp].x;
                        norm.z = TerI->curpos.z - WallSplList[lp].z;
                        hit = 1.0f;
                        NuVecNorm(&norm, &norm);
                        TerI->hittype = 0x11;
                        TerI->hittime = 0.0f;
                        TerI->hitnorm.x = norm.x;
                        TerI->hitnorm.y = norm.y;
                        TerI->hitnorm.z = norm.z;
                    }
                } else if ((WallSplList[lp].x - TerI->curpos.x) * (WallSplList[lp].x - TerI->curpos.x) +
                           (WallSplList[lp].z - TerI->curpos.z) * (WallSplList[lp].z - TerI->curpos.z) <=
                           (TerI->size + 0.0005f) * (TerI->size + 0.0005f)) {
                    norm.x = TerI->curpos.x - WallSplList[lp].x;
                    norm.z = TerI->curpos.z - WallSplList[lp].z;
                    hit = 1.0f;
                    NuVecNorm(&norm, &norm);
                    TerI->hittype = 0x11;
                    TerI->hittime = 0.0f;
                    TerI->hitnorm.x = norm.x;
                    TerI->hitnorm.y = norm.y;
                    TerI->hitnorm.z = norm.z;
                }
            }
        }
    }
    return hit;
}


/* PS2 layout and control flow reconstructed from 0x00177330. */
s32 HitTerrain(void) {
    short *CurData;
    tertype **currter;
    tertype *ter;
    struct nuvec_s pos;
    f32 ps;
    f32 pe;
    f32 ps2;
    f32 pe2;
    f32 size;
    s32 lp;
    s32 check;
    s32 hit;

    CurData = (short *)TerI->hitdata;
    size = TerI->size;
    hit = 0;
    TerI->hittype = 0;
    TerI->hittime = 999.9f;
    HitWallSpline();
    TerI->vellen = NuFsqrt(TerI->curvel.x * TerI->curvel.x +
                           TerI->curvel.z * TerI->curvel.z);

loop1:
    while (*CurData > 0) {
        TerI->csx = TerI->curpos.x - CurTerr->terr[CurData[1]].Location.x;
        TerI->csy = TerI->curpos.y - CurTerr->terr[CurData[1]].Location.y;
        TerI->csz = TerI->curpos.z - CurTerr->terr[CurData[1]].Location.z;
        TerI->cex = TerI->curpos.x + TerI->curvel.x -
                     CurTerr->terr[CurData[1]].Location.x;
        TerI->cey = TerI->curpos.y + TerI->curvel.y -
                     CurTerr->terr[CurData[1]].Location.y;
        TerI->cez = TerI->curpos.z + TerI->curvel.z -
                     CurTerr->terr[CurData[1]].Location.z;

        currter = (tertype **)CurData + 1;
        for (lp = *CurData; lp > 0; lp--, currter++) {
            ter = *currter;
            check = 0;
            pe = ((ter->norm[0].x * (TerI->cex - ter->pnts[0].x) +
                   ter->norm[0].y * (TerI->cey - ter->pnts[0].y) +
                   ter->norm[0].z * (TerI->cez - ter->pnts[0].z)) - size) -
                 TerI->impactadj;
            if (pe < 0.0f) {
                ps = (ter->norm[0].x * (TerI->csx - ter->pnts[0].x) +
                      ter->norm[0].y * (TerI->csy - ter->pnts[0].y) +
                      ter->norm[0].z * (TerI->csz - ter->pnts[0].z)) - size;
                if (ps > -size) {
                    check = 1;
                }
            }

            if (ter->norm[1].y < 65536.0f) {
                pe2 = ((ter->norm[1].x * (TerI->cex - ter->pnts[3].x) +
                        ter->norm[1].y * (TerI->cey - ter->pnts[3].y) +
                        ter->norm[1].z * (TerI->cez - ter->pnts[3].z)) - size) -
                      TerI->impactadj;
                if (pe2 < 0.0f) {
                    ps2 = (ter->norm[1].x * (TerI->csx - ter->pnts[3].x) +
                           ter->norm[1].y * (TerI->csy - ter->pnts[3].y) +
                           ter->norm[1].z * (TerI->csz - ter->pnts[3].z)) - size;
                    if (ps2 > -size) {
                        check = 1;
                    }
                }
            }

            if ((check != 0) && (HitPoly(ps, pe, ps2, pe2, ter) != 0)) {
                TerI->hitterrno = CurData[1];
                hit = 1;
                if ((TerI->hitter->info[2] & 0x80) != 0) {
                    PlatCrush = TerI->hitter->info[0] + 1;
                }
            }
        }
        CurData = (short *)currter;
    }

    if (*CurData < 0) {
        while (*CurData < 0) {
            CurData += ((*CurData >= 0) ? *CurData : -*CurData) * SCAN_SLOT + SCAN_SLOT;
        }
        goto loop1;
    }

    for (lp = 0; lp < curSphereter; lp++) {
        pos = SphereData[lp].pos;
        DeRotatePoint(&pos);
        hit = hit | CheckSphereTer(&pos, SphereData[lp].radius);
    }

    if ((TerI->hittype != 0) && (TerI->hitterrno != -1) &&
        (CurTerr->terr[TerI->hitterrno].type == 1)) {
        plathitid = CurTerr->terr[TerI->hitterrno].info;
    }
    return hit;
}

/* PS2 layout and control flow reconstructed from 0x00177780. */
s32 HitTerrPoly(tertype *ter, s32 terrindex) {
    f32 ps;
    f32 pe;
    f32 ps2;
    f32 pe2;
    f32 size;
    s32 check;
    s32 hit;

    hit = 0;
    size = TerI->size;
    TerI->hittype = 0;
    TerI->hittime = 999.9f;
    TerI->vellen = NuFsqrt(TerI->curvel.x * TerI->curvel.x +
                           TerI->curvel.z * TerI->curvel.z);

    TerI->csx = TerI->curpos.x - CurTerr->terr[terrindex].Location.x;
    TerI->csy = TerI->curpos.y - CurTerr->terr[terrindex].Location.y;
    TerI->csz = TerI->curpos.z - CurTerr->terr[terrindex].Location.z;
    TerI->cex = TerI->curpos.x + TerI->curvel.x -
                 CurTerr->terr[terrindex].Location.x;
    TerI->cey = TerI->curpos.y + TerI->curvel.y -
                 CurTerr->terr[terrindex].Location.y;
    TerI->cez = TerI->curpos.z + TerI->curvel.z -
                 CurTerr->terr[terrindex].Location.z;

    check = 0;
    pe = ((ter->norm[0].x * (TerI->cex - ter->pnts[0].x) +
           ter->norm[0].y * (TerI->cey - ter->pnts[0].y) +
           ter->norm[0].z * (TerI->cez - ter->pnts[0].z)) - size) -
         TerI->impactadj;
    if (pe < 0.0f) {
        ps = (ter->norm[0].x * (TerI->csx - ter->pnts[0].x) +
              ter->norm[0].y * (TerI->csy - ter->pnts[0].y) +
              ter->norm[0].z * (TerI->csz - ter->pnts[0].z)) - size;
        if (ps > -size) {
            check = 1;
        }
    }

    if (ter->norm[1].y < 65536.0f) {
        pe2 = ((ter->norm[1].x * (TerI->cex - ter->pnts[3].x) +
                ter->norm[1].y * (TerI->cey - ter->pnts[3].y) +
                ter->norm[1].z * (TerI->cez - ter->pnts[3].z)) - size) -
              TerI->impactadj;
        if (pe2 < 0.0f) {
            ps2 = (ter->norm[1].x * (TerI->csx - ter->pnts[3].x) +
                   ter->norm[1].y * (TerI->csy - ter->pnts[3].y) +
                   ter->norm[1].z * (TerI->csz - ter->pnts[3].z)) - size;
            if (ps2 > -size) {
                check = 1;
            }
        }
    }

    if ((check != 0) && (HitPoly(ps, pe, ps2, pe2, ter) != 0)) {
        TerI->hitterrno = terrindex;
        hit = 1;
    }
    return hit;
}

/* PS2 switch structure reconstructed from 0x00182b00. */
inline void TerrainMoveImpactData(void) {
    switch (TerI->hittype) {
        case 0:
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x14:
            if (TerI->hitterrno != -1) {
                TerrPolyInfo = *TerI->hitter;
                TerrPoly = &TerrPolyInfo;
                TerrPolyObj = TerI->hitterrno;
            }
            return;
    }
}

inline void FullDeflect(struct nuvec_s *norm, struct nuvec_s *vel, struct nuvec_s *out) {
    f32 dot;

    dot = norm->x * -vel->x + norm->y * -vel->y + norm->z * -vel->z + 0.001f;
    out->x = norm->x * dot + vel->x;
    out->y = norm->y * dot + vel->y;
    out->z = norm->z * dot + vel->z;
}

inline s32 FullDeflectTest(struct nuvec_s *norm, struct nuvec_s *vel, struct nuvec_s *out) {
    f32 dot;

    dot = norm->x * -vel->x + norm->y * -vel->y + norm->z * -vel->z + 0.001f;
    NuFsqrt(vel->x * vel->x + vel->y * vel->y + vel->z * vel->z);
    out->x = norm->x * dot + vel->x;
    out->y = norm->y * dot + vel->y;
    out->z = norm->z * dot + vel->z;
    if (dot > 0.0f) {
        return 1;
    }
    return 0;
}

inline void PlatformConnect(void *id, struct nuvec_s *curvel, struct nuvec_s *vvel, s32 platid) {
    TerrainTrackInfoType *track;
    s32 c;

    if (CurTrackInfo != 0) {
        CurTrackInfo->timer = 4;
        CurTrackInfo->platid = platid;
        CurTrackInfo->platinf |= 1;
        return;
    }
    for (c = 0; c < 4; c++) {
        if (CurTerr->TrackInfo[c].ptrid == 0) {
            track = &CurTerr->TrackInfo[c];
            goto found;
        }
    }
    track = 0;
found:
    CurTrackInfo = track;
    if (track != 0) {
        track->timer = 4;
        track->ptrid = id;
        track->platid = platid;
        track->platinf |= 1;
        curvel->y = 0.0f;
        vvel->y = 0.0f;
    }
}

void TerrainImpactNorm(void) {
    f32 scale;

    TerrainMoveImpactData();
    switch (TerI->hittype) {
        case 4:
        case 0x14:
            terrhitflags |= 4;
        case 2:
        case 3:
        case 0x12:
        case 0x13:
            RotateVec(&TerI->hitnorm, &TerI->hitnorm);
            if ((TerI->hittype & 0x10) == 0) {
                TerI->hitnorm.x = TerI->hitnorm.x * TerI->sizediv;
                TerI->hitnorm.y = TerI->hitnorm.y * TerI->sizediv;
                TerI->hitnorm.z = TerI->hitnorm.z * TerI->sizediv;
            }
        case 1:
        case 0x11:
            if (TerI->yscale == 1.0f) {
                TerI->uhitnorm.x = TerI->hitnorm.x;
                TerI->uhitnorm.y = TerI->hitnorm.y;
                TerI->uhitnorm.z = TerI->hitnorm.z;
            } else {
                scale = 1.0f / NuFsqrt(TerI->hitnorm.x * TerI->hitnorm.x +
                                       TerI->hitnorm.y * TerI->hitnorm.y * TerI->inyscalesq +
                                       TerI->hitnorm.z * TerI->hitnorm.z);
                TerI->uhitnorm.x = TerI->hitnorm.x * scale;
                TerI->uhitnorm.y = TerI->hitnorm.y * TerI->inyscale * scale;
                TerI->uhitnorm.z = TerI->hitnorm.z * scale;
            }
            break;
    }
}


void TerrainImpact(struct nuvec_s *vpos, struct nuvec_s *vvel, u8 *flags) {
    f32 dx;
    f32 dy;
    f32 dz;

    TerrainMoveImpactData();
    switch (TerI->hittype) {
        case 0:
            flags[0] = 0;
            vpos->x = TerI->curpos.x + TerI->curvel.x;
            vpos->y = (TerI->curpos.y + TerI->curvel.y) * TerI->yscale - TerI->size * TerI->yscale;
            vpos->z = TerI->curpos.z + TerI->curvel.z;
            break;
        case 1:
        case 2:
        case 3:
        case 4:
            TerI->hittime -= TerI->timeadj;
            if (TerI->hittime < 0.0f) {
                TerI->hittime = 0.0f;
            }
            dx = TerI->curvel.x * TerI->hittime;
            dy = TerI->curvel.y * TerI->hittime;
            dz = TerI->curvel.z * TerI->hittime;
            TerI->curpos.x += dx;
            TerI->curpos.y += dy;
            TerI->curpos.z += dz;
            TerI->curvel.x -= dx;
            TerI->curvel.y -= dy;
            TerI->curvel.z -= dz;
            FullDeflect(&TerI->hitnorm, &TerI->curvel, &TerI->curvel);
            FullDeflect(&TerI->uhitnorm, vvel, vvel);
            if (TerI->uhitnorm.y < 0.707f) {
                flags[0] = 0;
                terrhitflags |= 1;
                TerI->curpos.x += TerI->hitnorm.x * 0.002f;
                TerI->curpos.z += TerI->hitnorm.z * 0.002f;
                TerI->curvel.x += TerI->hitnorm.x * 0.003f;
                TerI->curvel.z += TerI->hitnorm.z * 0.003f;
                vvel->x += TerI->hitnorm.x * 0.003f;
                vvel->z += TerI->hitnorm.z * 0.003f;
            } else {
                terrhitflags |= 2;
                flags[0] = 1;
                flags[1] = 1;
                TerI->curpos.y += TerI->hitnorm.y * 0.001f;
            }
            break;
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x14:
            FullDeflect(&TerI->hitnorm, &TerI->curvel, &TerI->curvel);
            FullDeflect(&TerI->uhitnorm, vvel, vvel);
            if (TerI->uhitnorm.y < 0.707f) {
                TerI->curpos.x += TerI->hitnorm.x * 0.005f;
                TerI->curpos.y += TerI->hitnorm.y * 0.002f;
                TerI->curpos.z += TerI->hitnorm.z * 0.005f;
                TerI->curvel.x += TerI->hitnorm.x * 0.005f;
                TerI->curvel.y += TerI->hitnorm.y * 0.002f;
                TerI->curvel.z += TerI->hitnorm.z * 0.005f;
                flags[0] = 1;
            } else {
                TerI->curpos.y += TerI->hitnorm.y * 0.005f;
                TerI->curvel.y += TerI->hitnorm.y * 0.01f;
            }
            break;
    }
    if ((TerI->hittype != 0) && (TerI->hittype < 0x10) && (TerI->hitterrno != -1) &&
        (CurTerr->terr[TerI->hitterrno].type == 1) && (TerI->uhitnorm.y > 0.707f)) {
        PlatformConnect(TerI->flags, &TerI->curvel, vvel, CurTerr->terr[TerI->hitterrno].info);
    }
}

/* PS2 switch structure reconstructed from 0x00179f50. */
s32 TerrainImpactPlatform(u8 *flags) {
    switch (TerI->hittype) {
        case 0:
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x14:
            if (TerI->hitterrno != -1) {
                TerrPolyInfo = *TerI->hitter;
                TerrPoly = &TerrPolyInfo;
                TerrPolyObj = TerI->hitterrno;
            }
            break;
    }

    switch (TerI->hittype) {
        case 0:
            flags[0] = 0;
            TerI->curpos.x += TerI->curvel.x;
            TerI->curpos.y += TerI->curvel.y;
            TerI->curpos.z += TerI->curvel.z;
            return 0;
        case 1:
        case 2:
        case 3:
        case 4:
            TerI->hittime -= TerI->timeadj;
            if (TerI->hittime < 0.0f) {
                TerI->hittime = 0.0f;
            }
            TerI->curpos.x += TerI->curvel.x * TerI->hittime;
            TerI->curpos.y += TerI->curvel.y * TerI->hittime;
            TerI->curpos.z += TerI->curvel.z * TerI->hittime;
            if (TerI->uhitnorm.y < 0.707f) {
                flags[0] = 0;
            } else {
                flags[0] = 1;
                flags[1] = 1;
                TerI->curpos.y += TerI->hitnorm.y * 0.002f;
            }
            return 0;
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x14:
            return 1;
    }
    return 1;
}

s32 TerrainPlatformMoveCheck(struct nuvec_s *dest, struct nuvec_s *norm, s32 platid, s32 impact, s32 keepscan) {
    short *curdata;
    s32 terrno;
    struct nuvec_s curvel;
    struct nuvec_s hitnorm;
    struct nuvec_s uhitnorm;
    short hitcnt;
    short hitterrno;
    short hittype;
    short plathit;
    f32 hittime;

    curdata = TerI->PlatScanStart;
    terrno = CurTerr->platdata[platid].terrno;
    while (*curdata > 0) {
        if (curdata[1] == terrno) {
            *curdata = -((*curdata < 0) ? -*curdata : *curdata);
        }
        curdata += ((*curdata < 0) ? -*curdata : *curdata) * SCAN_SLOT + SCAN_SLOT;
    }
    curvel = TerI->curvel;
    hitcnt = TerI->hitcnt;
    hitterrno = TerI->hitterrno;
    hittype = TerI->hittype;
    plathit = TerI->plathit;
    hittime = TerI->hittime;
    hitnorm = TerI->hitnorm;
    uhitnorm = TerI->uhitnorm;
    TerI->curvel.x = dest->x - TerI->curpos.x;
    TerI->curvel.y = dest->y - TerI->curpos.y;
    TerI->curvel.z = dest->z - TerI->curpos.z;
    TerI->ay = NuAtan2D(TerI->curvel.x, TerI->curvel.z);
    TerI->ax = NuAtan2D(-TerI->curvel.y,
                        NuFsqrt(TerI->curvel.x * TerI->curvel.x + TerI->curvel.z * TerI->curvel.z));
    TerI->len = NuFsqrt(TerI->curvel.x * TerI->curvel.x + TerI->curvel.y * TerI->curvel.y +
                        TerI->curvel.z * TerI->curvel.z);
    HitTerrain();
    if ((impact != 0) && (TerI->hittype != 0)) {
        TerrainImpactNorm();
        TerrainImpactPlatform(TerI->flags);
        if (norm != 0) {
            *norm = TerI->hitnorm;
        }
        *dest = TerI->curpos;
    }
    TerI->curvel = curvel;
    TerI->hitcnt = hitcnt;
    TerI->hitterrno = hitterrno;
    TerI->plathit = plathit;
    TerI->hittime = hittime;
    TerI->hitnorm = hitnorm;
    TerI->uhitnorm = uhitnorm;
    curdata = TerI->PlatScanStart;
    if ((TerI->hittype < 0x10) || (keepscan == 0)) {
        while (*curdata != 0) {
            if (curdata[1] == terrno) {
                *curdata = (*curdata < 0) ? -*curdata : *curdata;
            }
            curdata += ((*curdata < 0) ? -*curdata : *curdata) * SCAN_SLOT + SCAN_SLOT;
        }
    }
    if (TerI->hittype != 0) {
        TerI->hittype = hittype;
        return 0;
    }
    TerI->hittype = hittype;
    return 1;
}

s32 TerrainPlatformEmbedded(struct nuvec_s *vvel) {
    TerTempInfoType *TerITemp;
    TerTempInfoType *TerITemp2;
    s32 oldcurSphereter;
    s8 flags[2];
    s32 curscltemp;
    TerrainSituType *mbuf2;
    tertype **HitData;
    tertype *ter;
    short *LastWrite;
    TerrainGroupType *group;
    s32 platid;
    s32 c;
    s32 HitCnt;
    f32 len;
    f32 ty;
    struct nuvec4_s pnts[4];
    struct nuvec4_s norm[2];

    curscltemp = 0;
    platid = CurTerr->terr[TerI->hitterrno].info;
    if (CurTerr->platdata[platid].curmtx != 0) {
        if ((CurTerr->platdata[platid].oldmtx._30 == CurTerr->platdata[platid].curmtx->_30) &&
            (CurTerr->platdata[platid].oldmtx._31 == CurTerr->platdata[platid].curmtx->_31) &&
            (CurTerr->platdata[platid].oldmtx._32 == CurTerr->platdata[platid].curmtx->_32)) {
            if (CurTerr->platdata[platid].status.bit.rotate == 0) {
                return 1;
            }
            if ((CurTerr->platdata[platid].oldmtx._00 == CurTerr->platdata[platid].curmtx->_00) &&
                (CurTerr->platdata[platid].oldmtx._01 == CurTerr->platdata[platid].curmtx->_01) &&
                (CurTerr->platdata[platid].oldmtx._02 == CurTerr->platdata[platid].curmtx->_02) &&
                (CurTerr->platdata[platid].oldmtx._10 == CurTerr->platdata[platid].curmtx->_10) &&
                (CurTerr->platdata[platid].oldmtx._11 == CurTerr->platdata[platid].curmtx->_11) &&
                (CurTerr->platdata[platid].oldmtx._12 == CurTerr->platdata[platid].curmtx->_12) &&
                (CurTerr->platdata[platid].oldmtx._20 == CurTerr->platdata[platid].curmtx->_20) &&
                (CurTerr->platdata[platid].oldmtx._21 == CurTerr->platdata[platid].curmtx->_21) &&
                (CurTerr->platdata[platid].oldmtx._22 == CurTerr->platdata[platid].curmtx->_22)) {
                return 1;
            }
        }
        TerITemp = TerI;
        oldcurSphereter = curSphereter;
        curSphereter = 0;
        ScaleTerrain = ScaleTerrainT2;
        TerI = (TerTempInfoType *)NuScratchAlloc32(sizeof(TerTempInfoType));
        TerI->yscale = TerITemp->yscale;
        TerI->yscalesq = TerITemp->yscalesq;
        TerI->inyscale = TerITemp->inyscale;
        TerI->inyscalesq = TerITemp->inyscalesq;
        TerI->size = TerITemp->size;
        TerI->sizediv = TerITemp->sizediv;
        TerI->sizesq = TerITemp->sizesq;
        TerI->id = TerITemp->id;
        TerI->timeadj = TerITemp->timeadj;
        TerI->impactadj = TerITemp->impactadj;
        TerI->flags = TerITemp->flags;
        TerI->scanmode = TerITemp->scanmode;
        TerI->stopflag = TerITemp->stopflag;
        TerI->curvel.x = (CurTerr->platdata[platid].oldmtx._30 - CurTerr->platdata[platid].curmtx->_30) * 1.3f;
        TerI->curvel.y = (CurTerr->platdata[platid].oldmtx._31 - CurTerr->platdata[platid].curmtx->_31) * 1.3f *
                         TerI->inyscale;
        TerI->curvel.z = (CurTerr->platdata[platid].oldmtx._32 - CurTerr->platdata[platid].curmtx->_32) * 1.3f;
        if (CurTerr->platdata[platid].status.bit.rotate != 0) {
            tertempmat = CurTerr->platdata[platid].oldmtx;
            tertempvec4.x = TerITemp->curpos.x;
            tertempvec4.y = TerITemp->curpos.y * TerI->yscale;
            tertempvec4.z = TerITemp->curpos.z;
            tertempvec4.w = 1.0f;
            NuVec4MtxInvTransformVU0(&tertempvec4, &tertempvec4, &tertempmat);
            tertempmat = *CurTerr->platdata[platid].curmtx;
            NuVec4MtxTransformVU0(&tertempvec4, &tertempvec4, &tertempmat);
            tertempvec4.y = tertempvec4.y * TerI->inyscale;
            TerI->curvel.x = (TerITemp->curpos.x - tertempvec4.x) * 1.3f;
            TerI->curvel.y = (TerITemp->curpos.y - tertempvec4.y) * 1.3f;
            TerI->curvel.z = (TerITemp->curpos.z - tertempvec4.z) * 1.3f;
        }
        TerI->curpos.x = TerITemp->curpos.x - TerI->curvel.x * 0.91f;
        TerI->curpos.y = TerITemp->curpos.y - TerI->curvel.y * 0.91f;
        TerI->curpos.z = TerITemp->curpos.z - TerI->curvel.z * 0.91f;
        LastWrite = (short *)TerI->hitdata;
        HitData = TerI->hitdata + 1;
        HitCnt = 0;
        mbuf2 = &CurTerr->terr[TerITemp->hitterrno];
        group = (TerrainGroupType *)mbuf2->model;
        if (TerI->yscale == 1.0f) {
            while (group->start >= 0) {
                for (c = group->count, ter = (tertype *)(group + 1); c > 0; c--) {
                    *HitData++ = ter;
                    HitCnt++;
                    ter++;
                }
                group = (TerrainGroupType *)ter;
            }
        } else if (CurTerr->platdata[platid].status.bit.rotate != 0) {
            while (group->start >= 0) {
                for (c = group->count, ter = (tertype *)(group + 1); c > 0; c--) {
                    pnts[0].x = ter->pnts[0].x;
                    pnts[0].y = ter->pnts[0].y;
                    pnts[0].z = ter->pnts[0].z;
                    pnts[0].w = 0.0f;
                    pnts[1].x = ter->pnts[1].x;
                    pnts[1].y = ter->pnts[1].y;
                    pnts[1].z = ter->pnts[1].z;
                    pnts[1].w = 0.0f;
                    pnts[2].x = ter->pnts[2].x;
                    pnts[2].y = ter->pnts[2].y;
                    pnts[2].z = ter->pnts[2].z;
                    pnts[2].w = 0.0f;
                    NuVec4MtxTransformVU0(&pnts[0], &pnts[0], CurTerr->platdata[mbuf2->info].curmtx);
                    NuVec4MtxTransformVU0(&pnts[1], &pnts[1], CurTerr->platdata[mbuf2->info].curmtx);
                    NuVec4MtxTransformVU0(&pnts[2], &pnts[2], CurTerr->platdata[mbuf2->info].curmtx);
                    norm[0].x = ter->norm[0].x;
                    norm[0].y = ter->norm[0].y;
                    norm[0].z = ter->norm[0].z;
                    norm[0].w = 0.0f;
                    NuVec4MtxTransformVU0(&norm[0], &norm[0], CurTerr->platdata[mbuf2->info].curmtx);
                    if (ter->norm[1].y < 65535.0f) {
                        pnts[3].x = ter->pnts[3].x;
                        pnts[3].y = ter->pnts[3].y;
                        pnts[3].z = ter->pnts[3].z;
                        pnts[3].w = 0.0f;
                        NuVec4MtxTransformVU0(&pnts[3], &pnts[3], CurTerr->platdata[mbuf2->info].curmtx);
                        norm[1].x = ter->norm[1].x;
                        norm[1].y = ter->norm[1].y;
                        norm[1].z = ter->norm[1].z;
                        norm[1].w = 0.0f;
                        NuVec4MtxTransformVU0(&norm[1], &norm[1], CurTerr->platdata[mbuf2->info].curmtx);
                    } else {
                        pnts[3] = pnts[2];
                    }
                    ScaleTerrain[curscltemp].info[0] = ter->info[0];
                    ScaleTerrain[curscltemp].info[1] = ter->info[1];
                    ScaleTerrain[curscltemp].info[2] = ter->info[2];
                    ScaleTerrain[curscltemp].info[3] = ter->info[3];
                    ScaleTerrain[curscltemp].pnts[0].x = pnts[0].x;
                    ScaleTerrain[curscltemp].pnts[0].z = pnts[0].z;
                    ScaleTerrain[curscltemp].pnts[0].y =
                        (pnts[0].y + mbuf2->Location.y) * TerI->inyscale - mbuf2->Location.y;
                    ScaleTerrain[curscltemp].pnts[1].x = pnts[1].x;
                    ScaleTerrain[curscltemp].pnts[1].z = pnts[1].z;
                    ScaleTerrain[curscltemp].pnts[1].y =
                        (pnts[1].y + mbuf2->Location.y) * TerI->inyscale - mbuf2->Location.y;
                    ScaleTerrain[curscltemp].pnts[2].x = pnts[2].x;
                    ScaleTerrain[curscltemp].pnts[2].z = pnts[2].z;
                    ScaleTerrain[curscltemp].pnts[2].y =
                        (pnts[2].y + mbuf2->Location.y) * TerI->inyscale - mbuf2->Location.y;
                    if (norm[1].y < 65535.0f) {
                        ScaleTerrain[curscltemp].pnts[3].x = pnts[3].x;
                        ScaleTerrain[curscltemp].pnts[3].z = pnts[3].z;
                        ScaleTerrain[curscltemp].pnts[3].y =
                            (pnts[3].y + mbuf2->Location.y) * TerI->inyscale - mbuf2->Location.y;
                        len = 1.0f / NuFsqrt(norm[1].x * norm[1].x + norm[1].y * norm[1].y * TerI->yscalesq +
                                             norm[1].z * norm[1].z);
                        ScaleTerrain[curscltemp].norm[1].x = norm[1].x * len;
                        ScaleTerrain[curscltemp].norm[1].y = norm[1].y * TerI->yscale * len;
                        ScaleTerrain[curscltemp].norm[1].z = norm[1].z * len;
                    } else {
                        ScaleTerrain[curscltemp].norm[1].y = 65536.0f;
                    }
                    len = 1.0f / NuFsqrt(norm[0].x * norm[0].x + norm[0].y * norm[0].y * TerI->yscalesq +
                                         norm[0].z * norm[0].z);
                    ScaleTerrain[curscltemp].norm[0].x = norm[0].x * len;
                    ScaleTerrain[curscltemp].norm[0].y = norm[0].y * TerI->yscale * len;
                    ScaleTerrain[curscltemp].norm[0].z = norm[0].z * len;
                    *HitData++ = &ScaleTerrain[curscltemp];
                    HitCnt++;
                    curscltemp++;
                    ter++;
                }
                group = (TerrainGroupType *)ter;
            }
        } else {
            while (group->start >= 0) {
                for (c = group->count, ter = (tertype *)(group + 1); c > 0; c--) {
                    ScaleTerrain[curscltemp].info[0] = ter->info[0];
                    ScaleTerrain[curscltemp].info[1] = ter->info[1];
                    ScaleTerrain[curscltemp].info[2] = ter->info[2];
                    ScaleTerrain[curscltemp].info[3] = ter->info[3];
                    ScaleTerrain[curscltemp].pnts[0].x = ter->pnts[0].x;
                    ScaleTerrain[curscltemp].pnts[0].z = ter->pnts[0].z;
                    ScaleTerrain[curscltemp].pnts[0].y =
                        (ter->pnts[0].y + mbuf2->Location.y) * TerI->inyscale - mbuf2->Location.y;
                    ScaleTerrain[curscltemp].pnts[1].x = ter->pnts[1].x;
                    ScaleTerrain[curscltemp].pnts[1].z = ter->pnts[1].z;
                    ScaleTerrain[curscltemp].pnts[1].y =
                        (ter->pnts[1].y + mbuf2->Location.y) * TerI->inyscale - mbuf2->Location.y;
                    ScaleTerrain[curscltemp].pnts[2].x = ter->pnts[2].x;
                    ScaleTerrain[curscltemp].pnts[2].z = ter->pnts[2].z;
                    ScaleTerrain[curscltemp].pnts[2].y =
                        (ter->pnts[2].y + mbuf2->Location.y) * TerI->inyscale - mbuf2->Location.y;
                    if (ter->norm[1].y < 65535.0f) {
                        ScaleTerrain[curscltemp].pnts[3].x = ter->pnts[3].x;
                        ScaleTerrain[curscltemp].pnts[3].z = ter->pnts[3].z;
                        ScaleTerrain[curscltemp].pnts[3].y =
                            (ter->pnts[3].y + mbuf2->Location.y) * TerI->inyscale - mbuf2->Location.y;
                        len = 1.0f / NuFsqrt(ter->norm[1].x * ter->norm[1].x +
                                             ter->norm[1].y * ter->norm[1].y * TerI->yscalesq +
                                             ter->norm[1].z * ter->norm[1].z);
                        ScaleTerrain[curscltemp].norm[1].x = ter->norm[1].x * len;
                        ScaleTerrain[curscltemp].norm[1].y = ter->norm[1].y * TerI->yscale * len;
                        ScaleTerrain[curscltemp].norm[1].z = ter->norm[1].z * len;
                    } else {
                        ScaleTerrain[curscltemp].norm[1].y = 65536.0f;
                    }
                    len = 1.0f / NuFsqrt(ter->norm[0].x * ter->norm[0].x +
                                         ter->norm[0].y * ter->norm[0].y * TerI->yscalesq +
                                         ter->norm[0].z * ter->norm[0].z);
                    ScaleTerrain[curscltemp].norm[0].x = ter->norm[0].x * len;
                    ScaleTerrain[curscltemp].norm[0].y = ter->norm[0].y * TerI->yscale * len;
                    ScaleTerrain[curscltemp].norm[0].z = ter->norm[0].z * len;
                    *HitData++ = &ScaleTerrain[curscltemp];
                    HitCnt++;
                    curscltemp++;
                    ter++;
                }
                group = (TerrainGroupType *)ter;
            }
        }
        if (HitCnt != 0) {
            LastWrite[0] = HitCnt;
            LastWrite[1] = TerITemp->hitterrno;
            LastWrite = (short *)HitData;
            HitData++;
            HitCnt = 0;
        }
        LastWrite[0] = 0;
        LastWrite[1] = 0;
        flags[0] = 0;
        flags[1] = 0;
        DerotateMovementVector();
        HitTerrain();
        TerrainImpactNorm();
        if (TerrainImpactPlatform((u8 *)flags) != 0) {
            NuScratchRelease();
            ScaleTerrain = ScaleTerrainT1;
            curSphereter = oldcurSphereter;
            TerI = TerITemp;
            return 1;
        }
        TerITemp2 = TerI;
        TerI = TerITemp;
        TerITemp2->origpos = TerITemp2->curpos;
        NewCast(&TerITemp2->origpos, 5.0f);
        flags[0] = 0;
        if (TerITemp2->origpos.y >= 2000000.0f) {
            TerI->curpos.y = TerITemp2->curpos.y;
        } else {
            ty = ((TerITemp2->origpos.y + TerITemp2->size) - TerITemp2->curpos.y) + 0.0025f;
            if (ty < -0.008f) {
            } else if (ty < 0.009f) {
                TerITemp2->curpos.y = TerITemp2->curpos.y + ty;
                flags[0] = 1;
            } else if ((ty - 0.0025f) * (ty - 0.0025f) <
                       TerITemp2->curvel.x * TerITemp2->curvel.x + TerITemp2->curvel.z * TerITemp2->curvel.z) {
                TerITemp2->curpos.y = TerITemp2->curpos.y + ty;
                flags[0] = 1;
            }
        }
        if (TerrainPlatformMoveCheck(&TerITemp2->curpos, 0, platid, 1, 1) == 0) {
            flags[0] = 0;
        }
        TerI->curpos.x = TerITemp2->curpos.x;
        TerI->curpos.y = TerITemp2->curpos.y;
        TerI->curpos.z = TerITemp2->curpos.z;
        if ((flags[0] != 0) && (CurTerr->terr[castnum].type == 1)) {
            TerI->flags[1] = 1;
            PlatformConnect(TerITemp->flags, &TerITemp->curvel, vvel, CurTerr->terr[castnum].info);
        } else if (CurTrackInfo != 0) {
            CurTrackInfo->platinf &= 0xFFFE;
        }
        NuScratchRelease();
        ScaleTerrain = ScaleTerrainT1;
        curSphereter = oldcurSphereter;
        TerI = TerITemp;
        return 0;
    } else {
        return 1;
    }
}


void StorePlatImpact(void) {
    struct nuvec_s hitnorm;
    f32 scale;

    if ((TerI->hittype == 0) || (TerI->hitterrno == -1)) {
        return;
    }
    if (CurTerr->terr[TerI->hitterrno].type != 1) {
        return;
    }
    hitnorm = TerI->hitnorm;
    switch (TerI->hittype) {
        case 2:
        case 3:
        case 4:
        case 0x12:
        case 0x13:
        case 0x14:
            RotateVec(&hitnorm, &hitnorm);
            if ((TerI->hittype & 0x10) == 0) {
                hitnorm.x = hitnorm.x * TerI->sizediv;
                hitnorm.y = hitnorm.y * TerI->sizediv;
                hitnorm.z = hitnorm.z * TerI->sizediv;
            }
        case 1:
        case 0x11:
            if (TerI->yscale != 1.0f) {
                scale = 1.0f / NuFsqrt(hitnorm.x * hitnorm.x +
                                       hitnorm.y * hitnorm.y * TerI->inyscalesq +
                                       hitnorm.z * hitnorm.z);
                hitnorm.x = hitnorm.x * scale;
                hitnorm.y = hitnorm.y * (TerI->inyscale * scale);
                hitnorm.z = hitnorm.z * scale;
            }
            PlatImpactId = CurTerr->terr[TerI->hitterrno].info;
            PlatImpactNorm = hitnorm;
            PlatImpactTer = *TerI->hitter;
            break;
    }
}

s32 PlatformChecks(s32 itterationcnt, struct nuvec_s *vvel) {
    TerTempInfoType *TerITemp;
    TerTempInfoType *TerITemp2;
    short *CurData;
    short *WriteData;
    s32 lp;
    s32 platid;
    s32 result;
    struct nuvec4_s pnts[2];
    f32 dist;
    f32 dist2;
    u8 flags[2];

    if (TerI->PlatScanStart == 0) {
        return itterationcnt;
    }
    if (*TerI->PlatScanStart == 0) {
        return itterationcnt;
    }
    if (CurTrackInfo == 0) {
        return itterationcnt;
    }
    TerITemp = TerI;
    TerI = TerITemp2 = (TerTempInfoType *)NuScratchAlloc32(sizeof(TerTempInfoType));
    CurData = TerITemp->PlatScanStart;
    WriteData = (short *)&TerI->hitdata[0];
    while (*CurData > 0) {
        lp = *WriteData = *CurData;
        WriteData[1] = CurData[1];
        WriteData += SCAN_SLOT;
        CurData += SCAN_SLOT;
        while (lp > 0) {
            *WriteData++ = *CurData++;
            *WriteData++ = *CurData++;
#ifdef NW_HOST /* the upper half of a host pointer */
            *WriteData++ = *CurData++;
            *WriteData++ = *CurData++;
#endif
            lp--;
        }
    }
    WriteData[0] = 0;
    WriteData[1] = 0;
    TerI->size = TerITemp->size;
    TerI->yscale = TerITemp->yscale;
    TerI->inyscalesq = TerITemp->inyscalesq;
    TerI->sizediv = TerITemp->sizediv;
    TerI->timeadj = TerITemp->timeadj;
    TerI->inyscale = TerITemp->inyscale;
    TerI->impactadj = TerITemp->impactadj;
    TerI->tempvec[0] = TerITemp->curpos;
    if ((CurTrackInfo != 0) && ((CurTrackInfo->platinf & 1) != 0)) {
        if (CurTerr->platdata[CurTrackInfo->platid].status.bit.rotate != 0) {
            tertempmat = CurTerr->platdata[CurTrackInfo->platid].oldmtx;
            tertempvec4.x = TerITemp->curpos.x;
            tertempvec4.y = TerITemp->curpos.y * TerI->yscale;
            tertempvec4.z = TerITemp->curpos.z;
            tertempvec4.w = 1.0f;
            NuVec4MtxInvTransformVU0(&tertempvec4, &tertempvec4, &tertempmat);
            tertempmat = *CurTerr->platdata[CurTrackInfo->platid].curmtx;
            NuVec4MtxTransformVU0(&tertempvec4, &tertempvec4, &tertempmat);
            tertempvec4.y = tertempvec4.y * TerI->inyscale;
            TerI->tempvec[0].x += tertempvec4.x - TerITemp->curpos.x;
            TerI->tempvec[0].y += tertempvec4.y - TerITemp->curpos.y;
            TerI->tempvec[0].z += tertempvec4.z - TerITemp->curpos.z;
        } else {
            TerI->tempvec[0].x += CurTerr->platdata[CurTrackInfo->platid].curmtx->_30 -
                                  CurTerr->platdata[CurTrackInfo->platid].oldmtx._30;
            TerI->tempvec[0].y += (CurTerr->platdata[CurTrackInfo->platid].curmtx->_31 -
                                   CurTerr->platdata[CurTrackInfo->platid].oldmtx._31) *
                                  TerITemp->inyscale;
            TerI->tempvec[0].z += CurTerr->platdata[CurTrackInfo->platid].curmtx->_32 -
                                  CurTerr->platdata[CurTrackInfo->platid].oldmtx._32;
        }
    }
    TerI->curpos = TerI->tempvec[1] = TerI->tempvec[0];
    TerI->curvel.x = 0.0f;
    TerI->curvel.z = 0.0f;
    TerI->curpos.y += 0.2f;
    TerI->curvel.y = -0.4f;
    DerotateMovementVector();
    HitTerrain();
    StorePlatImpact();
    TerI->hittime = TerI->hittime + TerI->timeadj;
    TerrainImpactNorm();
    TerrainImpactPlatform(flags);
    TerI->tempvec[0].y = TerI->curpos.y - TerI->size;
    TerI = TerITemp;
    if (TerITemp2->hittype == 0) {
        NuScratchRelease();
        return itterationcnt;
    }
    if (DABS(TerITemp2->tempvec[1].y - TerITemp2->tempvec[0].y - TerI->size) > 0.05f) {
        NuScratchRelease();
        return itterationcnt;
    }
    if (TerITemp2->hitnorm.y < 0.707f) {
        NuScratchRelease();
        return itterationcnt;
    }
    platid = CurTerr->terr[TerITemp2->hitterrno].info;
    if ((vvel->y > 0.04f) &&
        (vvel->y > (CurTerr->platdata[platid].curmtx->_31 + 0.04f) - CurTerr->platdata[platid].oldmtx._31)) {
        NuScratchRelease();
        return itterationcnt;
    }
    if ((TerITemp2->hittype != 0) && (TerITemp2->hitnorm.y > 0.707f) && (TerITemp2->hitterrno >= 0) &&
        (CurTerr->terr[TerITemp2->hitterrno].type == 1)) {
        CurTerr->platdata[CurTerr->terr[TerITemp2->hitterrno].info].status.bit.hit = 1;
    }
    TerI->tempvec[0].x = TerITemp2->tempvec[0].x;
    if (CurTerr->platdata[CurTrackInfo->platid].status.bit.rotate != 0) {
        TerI->tempvec[0].y = TerITemp2->tempvec[0].y + TerI->size;
    } else {
        TerI->tempvec[0].y = TerITemp2->tempvec[0].y + TerI->size + 0.0025f;
    }
    TerI->tempvec[0].z = TerITemp2->tempvec[0].z;
    pnts[0].x = TerI->tempvec[0].x - TerI->curpos.x;
    pnts[0].y = TerI->tempvec[0].y - TerI->curpos.y;
    pnts[0].z = TerI->tempvec[0].z - TerI->curpos.z;
    pnts[0].w = NuFsqrt(pnts[0].x * pnts[0].x + pnts[0].y * pnts[0].y + pnts[0].z * pnts[0].z);
    pnts[0].x = pnts[0].x / pnts[0].w;
    pnts[0].y = pnts[0].y / pnts[0].w;
    pnts[0].z = pnts[0].z / pnts[0].w;
    pnts[1].x = TerI->curpos.x;
    pnts[1].y = TerI->curpos.y;
    pnts[1].z = TerI->curpos.z;
    do {
        result = TerrainPlatformMoveCheck(&TerI->tempvec[0], &TerI->tempvec[1], platid, 1, 0);
        if ((TerI->hittype != 0) && (TerI->hitterrno >= 0) &&
            (CurTerr->terr[TerI->hitterrno].type == 1)) {
            CurTrackInfo->platid = CurTerr->terr[TerI->hitterrno].info;
        }
        itterationcnt--;
        if (pnts[0].x * TerI->tempvec[1].x + pnts[0].y * TerI->tempvec[1].y +
            pnts[0].z * TerI->tempvec[1].z < -0.7f) {
            result = 1;
        }
        if ((result == 0) && (itterationcnt > 0)) {
            dist = pnts[0].x * (TerI->curpos.x - pnts[1].x) + pnts[0].y * (TerI->curpos.y - pnts[1].y) +
                   pnts[0].z * (TerI->curpos.z - pnts[1].z);
            if (dist < pnts[0].w) {
                if (FullDeflectTest(&TerI->tempvec[1], (struct nuvec_s *)&pnts[0], &TerI->tempvec[0]) != 0) {
                    dist2 = pnts[0].x * TerI->tempvec[0].x + pnts[0].y * TerI->tempvec[0].y +
                            pnts[0].z * TerI->tempvec[0].z;
                    if (dist2 > 0.0f) {
                        dist2 = (pnts[0].w - dist) / dist2;
                        if (dist2 < 0.8f) {
                            TerI->tempvec[0].x = TerI->tempvec[0].x * dist2 + TerI->curpos.x;
                            TerI->tempvec[0].y = TerI->tempvec[0].y * dist2 + TerI->curpos.y;
                            TerI->tempvec[0].z = TerI->tempvec[0].z * dist2 + TerI->curpos.z;
                            dist = 1.0f;
                        } else {
                            dist = 0.0f;
                        }
                    } else {
                        dist = 0.0f;
                    }
                } else {
                    dist = 0.0f;
                }
            } else {
                dist = 0.0f;
            }
        }
    } while ((result == 0) && (itterationcnt > 0) && (dist != 0.0f));
    if (result != 0) {
        TerrPoly = TerITemp2->hitter;
        TerI->curpos = TerI->tempvec[0];
        if (vvel->y < 0.0f) {
            FullDeflect(&ShadNorm, &TerI->curvel, &TerI->curvel);
            FullDeflect(&ShadNorm, vvel, vvel);
        }
        TerI->flags[1] = 1;
        PlatformConnect(TerITemp->flags, &TerITemp->curvel, vvel, CurTerr->terr[TerITemp2->hitterrno].info);
    }
    NuScratchRelease();
    TerI = TerITemp;
    return itterationcnt;
}


s32 NewPlatInst(struct numtx_s *mtx, s32 instance) {
    s32 orig;
    s32 i;
    s32 platid;

    if (terlistptr >= 0x1000) {
        return -1;
    }
    if (situtotal >= 0x800) {
        return -1;
    }
    if (mtx == 0) {
        return -1;
    }
    platid = -1;
    for (i = 0; i < 128; i++) {
        if (CurTerr->platdata[i].curmtx == 0) {
            platid = i;
            break;
        }
    }
    if (platid == -1) {
        return -1;
    }
    orig = -1;
    for (i = 0; i < 128; i++) {
        if ((CurTerr->platdata[i].curmtx != 0) && (CurTerr->platdata[i].instance == instance)) {
            orig = i;
            break;
        }
    }
    if (orig == -1) {
        return -1;
    }
    CurTerr->terr[situtotal] = CurTerr->terr[CurTerr->platdata[orig].terrno];
    CurTerr->terr[situtotal].info = platid;
    CurTerr->terr[situtotal].type = 1;
    CurTerr->platdata[platid].curmtx = mtx;
    CurTerr->platdata[platid].instance = instance;
    CurTerr->platdata[platid].terrno = situtotal;
    CurTerr->platdata[platid].status.bit.rotate = 0;
    CurTerr->terrlist[terlistptr++] = situtotal;
    CurTerr->platdata[platid].plrgrav = 0.0f;
    CurTerr->platdata[platid].ypos = 0.0f;
    CurTerr->platdata[platid].yvel = 0.0f;
    CurTerr->platdata[platid].damp = 0.0f;
    CurTerr->platdata[platid].tension = 0.0f;
    situtotal++;
    CurTerr->terrgroup[256].count++;
    return platid;
}


s32 DeletePlatinst(s32 id) {
    s32 i;
    s32 slot;

    if ((CurTerr->platdata[id].curmtx == 0) || (id < 0) || (id >= 128)) {
        return 0;
    }
    situtotal--;
    terlistptr--;
    CurTerr->platdata[CurTerr->terr[situtotal].info].terrno = CurTerr->platdata[id].terrno;
    CurTerr->terr[CurTerr->platdata[id].terrno] = CurTerr->terr[situtotal];
    CurTerr->terr[situtotal].type = -1;
    CurTerr->platdata[id].curmtx = 0;
    slot = CurTerr->terrgroup[256].start;
    for (i = 0; i < CurTerr->terrgroup[256].count; i++, slot++) {
        if (CurTerr->terrlist[slot] == situtotal) {
            CurTerr->terrlist[slot] = CurTerr->terrlist[terlistptr];
            break;
        }
    }
    CurTerr->terrgroup[256].count--;
    return 1;
}

void TerrShowCamTerr(void) {
    plathitid = -1;
    TerrPolyObj = -1;
    TerrPoly = 0;
    PlatCrush = 0;
    terrhitflags = 0;
    TerI = (TerTempInfoType *)NuScratchAlloc32(sizeof(TerTempInfoType));
    TerI->yscale = 1.0f;
    TerI->yscalesq = TerI->yscale * TerI->yscale;
    TerI->inyscale = 1.0f / TerI->yscale;
    TerI->inyscalesq = TerI->inyscale * TerI->inyscale;
    TerI->size = 0.1f;
    TerI->sizediv = 1.0f / TerI->size;
    TerI->sizesq = TerI->size * TerI->size;
    TerI->origpos.x = TerI->curpos.x = global_camera.mtx._30 - 2.0f;
    TerI->origpos.y = TerI->curpos.y = global_camera.mtx._31 - 2.0f;
    TerI->origpos.z = TerI->curpos.z = global_camera.mtx._32 - 2.0f;
    TerI->origvel.x = TerI->curvel.x = 4.0f;
    TerI->origvel.y = TerI->curvel.y = 4.0f;
    TerI->origvel.z = TerI->curvel.z = 4.0f;
    TerI->id = 0;
    TerI->stopflag = 0.0f;
    TerI->flags = 0;
    TerI->scanmode = 0;
    TerI->timeadj = 0.01f;
    TerI->impactadj = 0.00001f;
    ScanTerrain(1, 1);
    NuScratchRelease();
}


void TerrainSetCur(void *curterr) {
    CurTerr = curterr;
}

f32 NewShadow(struct nuvec_s *ppos, f32 size) {
    struct nuvec_s pos;

    if (CurTerr == 0) {
        return 2000000.0f;
    }
    TerI = NuScratchAlloc32(sizeof(TerTempInfoType));
    pos = *ppos;
    NewScan(&pos, 0, 0);
    NewCast(&pos, 5.0f);
    NuScratchRelease();
    return pos.y;
}

f32 NewShadowMask(struct nuvec_s *ppos, f32 size, s32 extramask) {
    struct nuvec_s pos;

    if (CurTerr == 0) {
        return 2000000.0f;
    }
    TerI = NuScratchAlloc32(sizeof(TerTempInfoType));
    pos = *ppos;
    NewScan(&pos, extramask, 0);
    NewCast(&pos, 5.0f);
    NuScratchRelease();
    return pos.y;
}

f32 NewShadowTol(struct nuvec_s *ppos, f32 size, f32 tol) {
    struct nuvec_s pos;

    if (CurTerr == 0) {
        return 2000000.0f;
    }
    TerI = NuScratchAlloc32(sizeof(TerTempInfoType));
    pos = *ppos;
    NewScan(&pos, 0, 0);
    NewCast(&pos, tol);
    NuScratchRelease();
    return pos.y;
}

f32 NewShadowTolMask(struct nuvec_s *ppos, f32 size, f32 tol, s32 extramask) {
    struct nuvec_s pos;

    if (CurTerr == 0) {
        return 2000000.0f;
    }
    TerI = NuScratchAlloc32(sizeof(TerTempInfoType));
    pos = *ppos;
    NewScan(&pos, extramask, 0);
    NewCast(&pos, tol);
    NuScratchRelease();
    return pos.y;
}

f32 NewShadowPlat(struct nuvec_s *ppos, f32 size) {
    struct nuvec_s pos;

    if (CurTerr == 0) {
        return 2000000.0f;
    }
    TerI = NuScratchAlloc32(sizeof(TerTempInfoType));
    pos = *ppos;
    NewScan(&pos, 0, 1);
    NewCast(&pos, 5.0f);
    NuScratchRelease();
    return pos.y;
}

f32 NewShadowMaskPlat(struct nuvec_s *ppos, f32 size, s32 extramask) {
    struct nuvec_s pos;

    if (CurTerr == 0) {
        return 2000000.0f;
    }
    TerI = NuScratchAlloc32(sizeof(TerTempInfoType));
    pos = *ppos;
    NewScan(&pos, extramask, 1);
    NewCast(&pos, 5.0f);
    NuScratchRelease();
    return pos.y;
}

f32 NewShadowMaskPlatRot(struct nuvec_s *ppos, f32 size, s32 extramask) {
    struct nuvec_s pos;

    if (CurTerr == 0) {
        return 2000000.0f;
    }
    TerI = NuScratchAlloc32(sizeof(TerTempInfoType));
    pos = *ppos;
    NewScanRot(&pos, extramask);
    NewCast(&pos, 5.0f);
    NuScratchRelease();
    return pos.y;
}

f32 NewShadowTolPlat(struct nuvec_s *ppos, f32 size, f32 tol) {
    struct nuvec_s pos;

    if (CurTerr == 0) {
        return 2000000.0f;
    }
    TerI = NuScratchAlloc32(sizeof(TerTempInfoType));
    pos = *ppos;
    NewScan(&pos, 0, 1);
    NewCast(&pos, tol);
    NuScratchRelease();
    return pos.y;
}

f32 NewShadowTolMaskPlat(struct nuvec_s *ppos, f32 size, f32 tol, s32 extramask) {
    struct nuvec_s pos;

    if (CurTerr == 0) {
        return 2000000.0f;
    }
    TerI = NuScratchAlloc32(sizeof(TerTempInfoType));
    pos = *ppos;
    NewScan(&pos, extramask, 1);
    NewCast(&pos, tol);
    NuScratchRelease();
    return pos.y;
}

void AddCollisionSphere(struct nuvec_s *pos, f32 radius) {
    if (curSphereter < 16) {
        SphereData[curSphereter].pos = *pos;
        SphereData[curSphereter].radius = radius;
        curSphereter++;
    }
}

void AddPickupTerr(s32 type, struct nuvec_s *pos) {
    s32 id;

    id = PickupTerr->typeterr[type];
    if ((id != -1) && (curPickInst < 128)) {
        CurTerr->terr[curPickInst + 2048] = PickupTerr->terr[id];
        CurTerr->terr[curPickInst + 2048].Location = *pos;
        curPickInst++;
    }
}

void NewTerrAxisFreedom(TerrShapeType *shape, struct nuvec_s *pos) {
    struct nuvec_s slide;

    TerrShapeAdjCnt = 2;
    TerrShape = shape;
    slide.x = NuTrigTable[(s32)(shape->ang + 16384.0f) & 0xffff];
    slide.y = 0.0f;
    slide.z = -NuTrigTable[(s32)shape->ang & 0xffff];
    TerrainSideClamp(&slide, pos);
}

void *CrashDataPtr(void) {
    return crashdata;
}

void NewShapeInit(s32 *shape) {
    shape[0] = 0;
    shape[1] = 0;
    shape[2] = 0;
}

s32 TerrainInfo(void) {
    if (TerrPoly != 0) {
        return TerrPoly->info[0];
    }
    return -1;
}

s32 ShadowInfo(void) {
    if (ShadPoly != 0) {
        return ShadPoly->info[0];
    }
    return -1;
}

s32 ShadowRoofInfo(void) {
    if (ShadRoofPoly != 0) {
        return ShadRoofPoly->info[0];
    }
    return -1;
}

s32 EShadowRoofInfo(void) {
    if (EShadRoofPoly != 0) {
        return EShadRoofPoly->info[1];
    }
    return -1;
}

void ShadowDir(struct nuvec_s *dir) {
    dir->x = ShadPoly->pnts[1].x - ShadPoly->pnts[0].x;
    dir->y = ShadPoly->pnts[1].y - ShadPoly->pnts[0].y;
    dir->z = ShadPoly->pnts[1].z - ShadPoly->pnts[0].z;
}

s32 EShadowInfo(void) {
    if (EShadPoly != 0) {
        return EShadPoly->info[1];
    }
    return -1;
}

void noterraininit(void) {
    terraincnt = 0;
    platinrange = 0;
    ShadPoly = 0;
    CurTerr = 0;
    curSphereter = 0;
    TerrShapeAdjCnt = 0;
    curPickInst = 0;
}

s32 FindPlatInst(s32 instance) {
    s32 i;
    s32 found;

    if (instance == -1) {
        return -1;
    }
    found = -1;
    for (i = 0; i < 128; i++) {
        if ((CurTerr->platdata[i].curmtx != 0) && (CurTerr->platdata[i].instance == instance)) {
            found = i;
            break;
        }
    }
    return found;
}

void UpdatePlatinst(s32 id, struct numtx_s *mtx) {
    if ((id >= 0) && (id < 128)) {
        CurTerr->platdata[id].curmtx = mtx;
    }
}

void PlatInstRotate(s32 id, s32 rot) {
    CurTerr->platdata[id].status.bit.rotate = rot;
}

void PlatInstBounce(s32 id, f32 plrgrav, f32 tension, f32 damp) {
    if ((id >= 0) && (id < 128)) {
        if (CurTerr->platdata[id].plrgrav == 0.0f) {
            CurTerr->platdata[id].ypos = 0.0f;
            CurTerr->platdata[id].yvel = 0.0f;
        }
        CurTerr->platdata[id].plrgrav = plrgrav;
        CurTerr->platdata[id].tension = tension;
        CurTerr->platdata[id].damp = damp;
    }
}

s32 PlatInstGetHit(s32 id) {
    if ((id >= 0) && (id < 128)) {
        return CurTerr->platdata[id].status.bit.hit;
    }
    return 0;
}

s32 PlatImpactInfo(struct nuvec_s *norm, s32 *info0, s32 *info1) {
    s32 id;

    if (PlatImpactId != 0) {
        *norm = PlatImpactNorm;
        *info0 = PlatImpactTer.info[0];
        *info1 = PlatImpactTer.info[1];
    }
    id = PlatImpactId;
    PlatImpactId = 0;
    return id;
}

void TerrainPlatGetMtx(s32 id, struct numtx_s **oldmtx, struct numtx_s **curmtx) {
    if (id >= 0) {
        *oldmtx = &CurTerr->platdata[id].oldmtx;
        *curmtx = CurTerr->platdata[id].curmtx;
    }
}

s32 TerrainPlatId(void) {
    return plathitid;
}

s32 PlatformCrush(void) {
    return PlatCrush;
}

void NewScanInit(void) {
    TempStackPtr = TempScanStack;
    TerrPlatDis = -1;
}

short *NewScanHandel(struct nuvec_s *pos, struct nuvec_s *vel, s32 platscan, short *handel,
                     f32 size) {
    if (CurTerr == 0) {
        return 0;
    }
    TerrPlatDis = -1;
    if (handel == 0) {
        return NewScanHandelFull(pos, vel, platscan, 0, size);
    } else {
        return NewScanHandelSubset(handel, pos, vel, 0, size);
    }
}

void NewRaySetDisablePalt(s32 disable) {
    TerrPlatDis = disable;
}


void DrawPlatform(s32 id) {
    TerrainSituType *terr;
    TerrainGroupType *group;
    s32 count;
    tertype *ter;

    if (id >= 0) {
        terr = &CurTerr->terr[CurTerr->platdata[id].terrno];
        if ((terr->type == 0) || (terr->type == 1)) {
            group = (TerrainGroupType *)terr->model;
            while (group->start >= 0) {
                ter = (tertype *)(group + 1);
                for (count = group->count; count > 0; count--) {
                    TerrDrawPlat(ter, CurTerr->platdata[id].terrno);
                    ter++;
                }
                group = (TerrainGroupType *)ter;
            }
        }
    }
}


inline void TerrFlush(void) {
    curSphereter = 0;
    TerrShapeAdjCnt = 0;
    curPickInst = 0;
}


void DrawWallSpline(f32 y) {
    struct nuvec_s end;
    struct nuvec_s start;
    s32 lp;

    if (WallSplCount != 0) {
        for (lp = 0; lp < WallSplCount; lp += 2) {
            start.x = WallSplList[lp].x;
            start.z = WallSplList[lp].z;
            if (lp == WallSplCount) {
                end.x = start.x;
                end.z = start.z;
            } else {
                end.x = WallSplList[lp + 1].x;
                end.z = WallSplList[lp + 1].z;
            }
            NuRndrLine3dDbg(start.x, y, start.z, end.x, y, end.z, 0x80808080);
        }
    }
}


inline TerrainTrackInfoType *ScanTerrId(void *id) {
    s32 c;

    for (c = 0; c < 4; c++) {
        if (CurTerr->TrackInfo[c].ptrid == id) {
            return &CurTerr->TrackInfo[c];
        }
    }
    return 0;
}

TerrainTrackInfoType *AllocTerrId(void) {
    s32 c;

    for (c = 0; c < 4; c++) {
        if (CurTerr->TrackInfo[c].ptrid == 0) {
            return &CurTerr->TrackInfo[c];
        }
    }
    return 0;
}


void FullReflect(struct nuvec_s *norm, struct nuvec_s *vel, struct nuvec_s *out) {
    f32 dot;

    dot = norm->x * -vel->x + norm->y * -vel->y + norm->z * -vel->z;
    out->x = norm->x * dot * 2.0f + vel->x;
    out->y = norm->y * dot * 2.0f + vel->y;
    out->z = norm->z * dot * 2.0f + vel->z;
}

void OffPlat(void) {
}

void NewTerrain(struct nuvec_s *vpos, struct nuvec_s *vvel, u8 *flags, s32 terid, f32 stopflag,
                f32 size) {
    s32 cnt;
    s32 normhit;

    if (CurTerr == 0) {
        return;
    }
    plathitid = -1;
    TerrPolyObj = -1;
    TerrPoly = 0;
    PlatCrush = 0;
    terrhitflags = 0;
    CurTrackInfo = ScanTerrId(flags);
    TerI = (TerTempInfoType *)NuScratchAlloc32(sizeof(TerTempInfoType));
    TerI->inyscalesq = TerI->inyscale = TerI->yscalesq = TerI->yscale = 1.0f;
    TerI->size = size;
    TerI->sizediv = 1.0f / size;
    TerI->sizesq = size * size;
    TerI->curpos.x = vpos->x;
    TerI->curpos.y = vpos->y + size;
    TerI->curpos.z = vpos->z;
    TerI->curvel = *vvel;
    TerI->id = terid;
    TerI->stopflag = stopflag;
    TerI->flags = flags;
    TerI->scanmode = 0;
    TerI->timeadj = 0.01f;
    TerI->impactadj = 0.00001f;
    ScanTerrain(1, 0);

    if ((flags[1] != 0) && (DABS(vvel->x) < stopflag) && (DABS(vvel->y) < stopflag) &&
        (DABS(vvel->z) < stopflag) && (platinrange == 0)) {
        NuScratchRelease();
        TerrFlush();
        return;
    }

    flags[0] = 0;
    flags[1] = 0;
    cnt = 4;
    do {
        cnt--;
        DerotateMovementVector();
        HitTerrain();
        StorePlatImpact();
        if ((TerI->hittype > 0x10) && (TerI->hitterrno != -1) &&
            (CurTerr->terr[TerI->hitterrno].type == 1)) {
            normhit = TerrainPlatformEmbedded(vvel);
        } else {
            normhit = 1;
        }
        if (normhit != 0) {
            TerrainImpactNorm();
            if (TerI->hittype != 0) {
                ShadNorm = TerI->uhitnorm;
            }
            TerrainImpact(vpos, vvel, flags);
        }
    } while ((TerI->hittype != 0) && (cnt > 0));

    if (TerI->hittype != 0) {
        vpos->x = TerI->curpos.x;
        vpos->y = TerI->curpos.y - TerI->size;
        vpos->z = TerI->curpos.z;
    }
    NuScratchRelease();
    TerrFlush();
}

// NGC MATCH (PS2 layout/control flow verified against 0x0017c7d8)
s32 TerrShapeSideStep(struct nuvec_s *vpos, struct nuvec_s *vvel, u8 *flags) {
    struct nuvec_s slide;
    struct nuvec_s temp;
    f32 dotp;
    s32 doslide;
    u8 tflags[2];

    switch (TerI->hittype) {
        case 0:
            return 1;
        default:
            if ((TerI->uhitnorm.y > 0.707f) || (TerI->uhitnorm.y < -0.8f)) {
                return 1;
            }
            doslide = 0;
            break;
        case 2:
        case 3:
            if (TerI->uhitnorm.y < 0.5f) {
                doslide = 0;
            } else {
                doslide = 1;
            }
            break;
    }

    slide.x = NuTrigTable[(s32)(TerrShape->ang + 16384.0f) & 0xffff];
    slide.y = 0.0f;
    slide.z = -NuTrigTable[(s32)TerrShape->ang & 0xffff];
    TerI->origpos.x = TerI->curpos.x;
    TerI->origpos.y = TerI->curpos.y;
    TerI->origpos.z = TerI->curpos.z;
    TerI->origvel.x = TerI->curvel.x;
    TerI->origvel.y = TerI->curvel.y;
    TerI->origvel.z = TerI->curvel.z;
    TerI->curvel.y = 0.0f;
    if (doslide == 1) {
        TerI->curvel.x = TerI->hitnorm.x * TerrShape->size * 0.15f;
        TerI->curvel.z = TerI->hitnorm.z * TerrShape->size * 0.15f;
        TerrShape->offset.x = TerrShape->offset.x + TerI->hitnorm.x * TerrShape->size * 0.05f;
        TerrShape->offset.z = TerrShape->offset.z + TerI->hitnorm.z * TerrShape->size * 0.05f;
    } else {
        dotp = slide.x * TerI->uhitnorm.x + slide.z * TerI->uhitnorm.z;
        if ((dotp == 0.0f) &&
            (dotp = (TerrShape->offset.x * slide.x + TerrShape->offset.z * slide.z) / TerrShape->size,
             dotp == 0.0f)) {
            return 1;
        }
        if (0.0f > dotp) {
            dotp = -NuFsqrt(-dotp);
        } else {
            dotp = NuFsqrt(dotp);
        }
        TerI->curvel.x = dotp * slide.x * TerrShape->size + TerrShape->offset.x;
        TerI->curvel.z = dotp * slide.z * TerrShape->size + TerrShape->offset.z;
    }

    do {
        TerI->ay = (s32)NuAtan2D(TerI->curvel.x, TerI->curvel.z);
        TerI->ax = (s32)NuAtan2D(-TerI->curvel.y,
                                  NuFsqrt(TerI->curvel.x * TerI->curvel.x +
                                          TerI->curvel.z * TerI->curvel.z));
        TerI->len = NuFsqrt(TerI->curvel.x * TerI->curvel.x +
                             TerI->curvel.y * TerI->curvel.y +
                             TerI->curvel.z * TerI->curvel.z);
        HitTerrain();
        TerrainImpactNorm();
        TerrainImpact(&temp, &temp, tflags);
        TerrShapeAdjCnt--;
    } while ((TerrShapeAdjCnt > 0) && (TerI->hittype != 0));

    if (TerI->hittype == 0) {
        TerI->curpos.x = TerI->curpos.x + TerI->curvel.x;
        TerI->curpos.y = TerI->curpos.y + TerI->curvel.y;
        TerI->curpos.z = TerI->curpos.z + TerI->curvel.z;
    }
    TerrShape->offset.x = TerrShape->offset.x + (TerI->origpos.x - TerI->curpos.x);
    TerrShape->offset.z = TerrShape->offset.z + (TerI->origpos.z - TerI->curpos.z);
    TerI->hittype = 1;
    TerI->curvel.x = TerI->origvel.x;
    TerI->curvel.y = TerI->origvel.y;
    TerI->curvel.z = TerI->origvel.z;
    TerrainSideClamp(&slide, &TerI->curpos);
    return 0;
}

void CubeImpact(struct numtx_s *mat, struct numtx_s *nmat, struct nuvec_s *norm, f32 size,
                struct nuvec_s *impact) {
    struct nuvec_s cube[8] = {
        {-1.0f, -1.0f, -1.0f},
        { 1.0f, -1.0f, -1.0f},
        { 1.0f,  1.0f, -1.0f},
        {-1.0f,  1.0f, -1.0f},
        {-1.0f, -1.0f,  1.0f},
        { 1.0f, -1.0f,  1.0f},
        { 1.0f,  1.0f,  1.0f},
        {-1.0f,  1.0f,  1.0f},
    };
    struct nuvec4_s pnts[10];
    f32 best;
    s32 bestnum;
    f32 dota;
    f32 dotb;
    s32 lp;

    best = 10000.0f;
    bestnum = 0;
    for (lp = 0; lp < 8; lp++) {
        pnts[lp].x = cube[lp].x;
        pnts[lp].y = cube[lp].y;
        pnts[lp].z = cube[lp].z;
        pnts[lp].w = 0.0f;
        NuVec4MtxTransformVU0(&pnts[8], &pnts[lp], nmat);
        NuVec4MtxTransformVU0(&pnts[lp], &pnts[lp], mat);
        dota = pnts[lp].x * norm->x + pnts[lp].y * norm->y + pnts[lp].z * norm->z;
        dotb = pnts[8].x * norm->x + pnts[8].y * norm->y + pnts[8].z * norm->z;
        if ((dota > dotb) && (dota < best)) {
            best = dota;
            bestnum = lp;
        }
    }
    impact->x = mat->_30 + pnts[bestnum].x * size;
    impact->y = mat->_31 + pnts[bestnum].y * size;
    impact->z = mat->_32 + pnts[bestnum].z * size;
}

// PS2 PAL: byte-exact over the full 0x580-byte extent at 0x0017cec8.
void NewTerrainScaleY(struct nuvec_s *vpos, struct nuvec_s *vvel, u8 *flags,
                      s32 terid, f32 stopflag, f32 size, f32 yscale) {
    s32 cnt;
    s32 normhit;

    if (CurTerr == 0) {
        return;
    }

    plathitid = -1;
    TerrPolyObj = -1;
    TerrPoly = 0;
    PlatCrush = 0;
    terrhitflags = 0;
    CurTrackInfo = ScanTerrId(flags);

    if (PadRecPtr() >= 0x23D) {
        plathitid = -1;
    }

    TerI = (TerTempInfoType *)NuScratchAlloc32(sizeof(TerTempInfoType));
    TerI->yscale = yscale;
    TerI->yscalesq = TerI->yscale * TerI->yscale;
    TerI->inyscale = 1.0f / yscale;
    TerI->inyscalesq = TerI->inyscale * TerI->inyscale;
    TerI->size = size;
    TerI->sizediv = 1.0f / TerI->size;
    TerI->sizesq = TerI->size * TerI->size;
    TerI->origpos.x = TerI->curpos.x = vpos->x;
    TerI->origpos.y = TerI->curpos.y = vpos->y + TerI->size * yscale;
    TerI->origpos.z = TerI->curpos.z = vpos->z;
    TerI->origvel.x = TerI->curvel.x = vvel->x;
    TerI->origvel.y = TerI->curvel.y = vvel->y;
    TerI->origvel.z = TerI->curvel.z = vvel->z;
    TerI->id = terid;
    TerI->stopflag = stopflag;
    TerI->flags = flags;
    TerI->scanmode = 0;
    TerI->timeadj = 0.01f;
    TerI->impactadj = 0.00001f;
    ScanTerrain(1, 0);

    if ((flags[1] != 0) && (DABS(vvel->x) < stopflag) && (DABS(vvel->y) < stopflag) &&
        (DABS(vvel->z) < stopflag) && (platinrange == 0)) {
        NuScratchRelease();
        TerrFlush();
        return;
    }

    TerI->curpos.y = TerI->curpos.y * TerI->inyscale;
    TerI->curvel.y = TerI->curvel.y * TerI->inyscale;
    flags[0] = 0;
    flags[1] = 0;
    cnt = PlatformChecks(4, vvel);

    do {
        DerotateMovementVector();
        HitTerrain();
        StorePlatImpact();
        if ((TerI->hittype > 0x10) && (TerI->hitterrno != -1) &&
            (CurTerr->terr[TerI->hitterrno].type == 1)) {
            cnt--;
            normhit = TerrainPlatformEmbedded(vvel);
        } else {
            normhit = 1;
        }
        if (normhit != 0) {
            cnt--;
            TerrainImpactNorm();
            if (TerI->hittype != 0) {
                ShadNorm = TerI->uhitnorm;
                if ((TerI->uhitnorm.y > 0.707f) && (TerI->hitterrno >= 0) &&
                    (CurTerr->terr[TerI->hitterrno].type == 1)) {
                    CurTerr->platdata[CurTerr->terr[TerI->hitterrno].info].status.bit.hit = 1;
                }
            }
            if (TerrShapeAdjCnt != 0) {
                if (TerrShapeSideStep(vpos, vvel, flags) != 0) {
                    TerrainImpact(vpos, vvel, flags);
                }
            } else {
                TerrainImpact(vpos, vvel, flags);
            }
        }
    } while ((TerI->hittype != 0) && (cnt > 0) &&
             (TerI->hitnorm.x * TerI->hitnorm.x +
              TerI->hitnorm.y * TerI->hitnorm.y +
              TerI->hitnorm.z * TerI->hitnorm.z <= 1.5f));

    if (TerI->hittype != 0) {
        vpos->x = TerI->curpos.x;
        vpos->y = TerI->curpos.y * TerI->yscale - TerI->size * TerI->yscale;
        vpos->z = TerI->curpos.z;
    }
    if (testlock != 0) {
        vpos->x = TerI->origpos.x;
        vpos->z = TerI->origpos.z;
    }

    NuScratchRelease();
    TerrFlush();
}

void RayImpact(struct nuvec_s *vec) {
    switch (TerI->hittype) {
        case 0:
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x14:
            if (TerI->hitterrno != -1) {
                TerrPolyInfo = *TerI->hitter;
                TerrPoly = &TerrPolyInfo;
                TerrPolyObj = TerI->hitterrno;
            }
            break;
    }

    switch (TerI->hittype) {
        case 0:
            break;
        case 1:
        case 2:
        case 3:
        case 4:
            TerI->hittime -= TerI->timeadj;
            if (TerI->hittime < 0.0f) {
                TerI->hittime = 0.0f;
            }
            vec->x = TerI->curvel.x * TerI->hittime;
            vec->y = TerI->curvel.y * TerI->hittime;
            vec->z = TerI->curvel.z * TerI->hittime;
            break;
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x14:
            vec->x = 0.0f;
            vec->y = 0.0f;
            vec->z = 0.0f;
            break;
    }
}

s32 NewRayCast(struct nuvec_s *vpos, struct nuvec_s *vvel, f32 size) {
    if (CurTerr == 0) {
        return 0;
    }
    plathitid = -1;
    TerrPolyObj = -1;
    TerrPoly = 0;
    TerI = (TerTempInfoType *)NuScratchAlloc32(sizeof(TerTempInfoType));
    TerI->inyscalesq = TerI->inyscale = TerI->yscalesq = TerI->yscale = 1.0f;
    TerI->size = size;
    TerI->sizediv = 1.0f / TerI->size;
    TerI->sizesq = TerI->size * TerI->size;
    TerI->origpos.x = TerI->curpos.x = vpos->x;
    TerI->origpos.y = TerI->curpos.y = vpos->y;
    TerI->origpos.z = TerI->curpos.z = vpos->z;
    TerI->origvel = TerI->curvel = *vvel;
    TerI->id = -1;
    TerI->flags = 0;
    TerI->scanmode = 1;
    TerI->timeadj = 0.01f;
    TerI->impactadj = 0.00001f;
    ScanTerrain(0, 0);
    DerotateMovementVector();
    HitTerrain();
    if (TerI->hittype != 0) {
        RayImpact(vvel);
        TerrainImpactNorm();
        ShadNorm = TerI->hitnorm;
    }
    NuScratchRelease();
    return TerI->hittype;
}

s32 NewRayCastMask(struct nuvec_s *vpos, struct nuvec_s *vvel, f32 size, s32 extramask) {
    if (CurTerr == 0) {
        return 0;
    }
    plathitid = -1;
    TerrPolyObj = -1;
    TerrPoly = 0;
    TerI = (TerTempInfoType *)NuScratchAlloc32(sizeof(TerTempInfoType));
    TerI->inyscalesq = TerI->inyscale = TerI->yscalesq = TerI->yscale = 1.0f;
    TerI->size = size;
    TerI->sizediv = 1.0f / TerI->size;
    TerI->sizesq = TerI->size * TerI->size;
    TerI->origpos.x = TerI->curpos.x = vpos->x;
    TerI->origpos.y = TerI->curpos.y = vpos->y;
    TerI->origpos.z = TerI->curpos.z = vpos->z;
    TerI->origvel = TerI->curvel = *vvel;
    TerI->id = -1;
    TerI->flags = 0;
    TerI->scanmode = 1;
    TerI->timeadj = 0.01f;
    TerI->impactadj = 0.00001f;
    ScanTerrain(0, extramask);
    DerotateMovementVector();
    HitTerrain();
    if (TerI->hittype != 0) {
        RayImpact(vvel);
        TerrainImpactNorm();
        ShadNorm = TerI->hitnorm;
    }
    NuScratchRelease();
    return TerI->hittype;
}

s32 NewRayCastSet(struct nuvec_s *vpos, struct nuvec_s *vvel, f32 size, f32 timeadj,
                  f32 impactadj, s32 platscan) {
    if (CurTerr == 0) {
        return 0;
    }
    plathitid = -1;
    TerrPolyObj = -1;
    TerrPoly = 0;
    TerI = (TerTempInfoType *)NuScratchAlloc32(sizeof(TerTempInfoType));
    TerI->inyscalesq = TerI->inyscale = TerI->yscalesq = TerI->yscale = 1.0f;
    TerI->size = size;
    TerI->sizediv = 1.0f / TerI->size;
    TerI->sizesq = TerI->size * TerI->size;
    TerI->origpos.x = TerI->curpos.x = vpos->x;
    TerI->origpos.y = TerI->curpos.y = vpos->y;
    TerI->origpos.z = TerI->curpos.z = vpos->z;
    TerI->origvel = TerI->curvel = *vvel;
    TerI->id = -1;
    TerI->flags = 0;
    TerI->scanmode = 1;
    TerI->timeadj = timeadj;
    TerI->impactadj = impactadj;
    ScanTerrain(platscan, 0);
    DerotateMovementVector();
    HitTerrain();
    if (TerI->hittype != 0) {
        RayImpact(vvel);
        TerrainImpactNorm();
        ShadNorm = TerI->hitnorm;
    }
    NuScratchRelease();
    return TerI->hittype;
}

s32 NewRayCastSetMask(struct nuvec_s *vpos, struct nuvec_s *vvel, f32 size, f32 timeadj,
                      f32 impactadj, s32 platscan, s32 extramask) {
    if (CurTerr == 0) {
        return 0;
    }
    plathitid = -1;
    TerrPolyObj = -1;
    TerrPoly = 0;
    TerI = (TerTempInfoType *)NuScratchAlloc32(sizeof(TerTempInfoType));
    TerI->inyscalesq = TerI->inyscale = TerI->yscalesq = TerI->yscale = 1.0f;
    TerI->size = size;
    TerI->sizediv = 1.0f / TerI->size;
    TerI->sizesq = TerI->size * TerI->size;
    TerI->origpos.x = TerI->curpos.x = vpos->x;
    TerI->origpos.y = TerI->curpos.y = vpos->y;
    TerI->origpos.z = TerI->curpos.z = vpos->z;
    TerI->origvel = TerI->curvel = *vvel;
    TerI->id = -1;
    TerI->flags = 0;
    TerI->scanmode = 1;
    TerI->timeadj = timeadj;
    TerI->impactadj = impactadj;
    ScanTerrain(platscan, extramask);
    DerotateMovementVector();
    HitTerrain();
    if (TerI->hittype != 0) {
        RayImpact(vvel);
        TerrainImpactNorm();
        ShadNorm = TerI->hitnorm;
    }
    NuScratchRelease();
    return TerI->hittype;
}

short *NewScanHandelFull(struct nuvec_s *vpos, struct nuvec_s *vvel, s32 platscan, s32 extramask,
                         f32 size) {
    s32 a;
    s32 b;
    s32 c;
    s32 HitCnt;
    tertype **HitData;
    tertype **MaxData;
    short *LastWrite;
    short *ttemp;
    short *scandata;
    WallSplineType *WallSpl;
    struct nuvec_s **walldata;
    f32 maxx;
    f32 maxy;
    f32 maxz;
    f32 minx;
    f32 miny;
    f32 minz;
    f32 tmaxx;
    f32 tmaxy;
    f32 tmaxz;
    f32 tminx;
    f32 tminy;
    f32 tminz;
    TerrainSituType *mbuf2;
    tertype *ter;
    TerrainGroupType *group;
    struct nuvec4_s pnts[4];
    struct nuvec4_s norm[2];
    f32 radmov;
    f32 tn;
    s32 curscltemp;

    curscltemp = 0;
    ScaleTerrain = ScaleTerrainT1;
    scandata = LastWrite = TempStackPtr;
    HitData = (tertype **)(LastWrite + SCAN_SLOT);
    MaxData = HitData + 0x1FC;
    if (HitData > ScanHitLimit) {
        return 0;
    }
    if (ScanHitLimitEnd < MaxData) {
        MaxData = ScanHitLimitEnd;
    }
    HitCnt = 0;
    platinrange = 0;
    if (vvel->x > 0.0f) {
        minx = vpos->x - 0.02f - size;
        maxx = vpos->x + vvel->x + 0.02f + size;
    } else {
        minx = vpos->x + vvel->x - 0.02f - size;
        maxx = vpos->x + 0.02f + size;
    }
    if (vvel->y > 0.0f) {
        miny = vpos->y - 0.02f - size;
        maxy = vpos->y + vvel->y + 0.02f + size;
    } else {
        miny = vpos->y + vvel->y - 0.02f - size;
        maxy = vpos->y + 0.02f + size;
    }
    if (vvel->z > 0.0f) {
        minz = vpos->z - 0.02f - size;
        maxz = vpos->z + vvel->z + 0.02f + size;
    } else {
        minz = vpos->z + vvel->z - 0.02f - size;
        maxz = vpos->z + 0.02f + size;
    }
    radmov = (minx - maxx) * (minx - maxx) + (miny - maxy) * (miny - maxy) + (minz - maxz) * (minz - maxz);
    radmov += 0.02f;
    for (a = 0; a < CurTerr->terrgroupcnt; a++) {
        if ((maxx >= CurTerr->terrgroup[a].bounds[0]) && (maxz >= CurTerr->terrgroup[a].bounds[1]) &&
            (minx <= CurTerr->terrgroup[a].bounds[2]) && (minz <= CurTerr->terrgroup[a].bounds[3])) {
            ttemp = &CurTerr->terrlist[CurTerr->terrgroup[a].start];
            for (b = 0; b < CurTerr->terrgroup[a].count; b++, ttemp++) {
                mbuf2 = &CurTerr->terr[*ttemp];
                if ((maxx >= mbuf2->min.x) && (maxy >= mbuf2->min.y) && (maxz >= mbuf2->min.z) &&
                    (minx <= mbuf2->max.x) && (miny < mbuf2->max.y) && (minz < mbuf2->max.z) &&
                    (mbuf2->type != -1)) {
                    tmaxx = maxx - mbuf2->Location.x;
                    tmaxy = maxy - mbuf2->Location.y;
                    tmaxz = maxz - mbuf2->Location.z;
                    tminx = minx - mbuf2->Location.x;
                    tminy = miny - mbuf2->Location.y;
                    tminz = minz - mbuf2->Location.z;
                    group = (TerrainGroupType *)mbuf2->model;
                    while (group->start >= 0) {
                        c = group->count;
                        ter = (tertype *)(group + 1);
                        if ((tmaxx >= group->bounds[0]) && (tminx < group->bounds[1]) &&
                            (tmaxz >= group->bounds[2]) && (tminz < group->bounds[3])) {
                            for (; c > 0; c--) {
                                if ((tmaxx >= ter->minx) && (tminx < ter->maxx) && (tmaxz >= ter->minz) &&
                                    (tminz < ter->maxz) && (tmaxy >= ter->miny) && (tminy < ter->maxy) &&
                                    (HitData < MaxData) &&
                                    ((ter->info[1] == 0) || ((ter->info[1] & extramask) != 0))) {
                                    *HitData++ = ter;
                                    HitCnt++;
                                }
                                ter++;
                            }
                            group = (TerrainGroupType *)ter;
                        } else {
                            group = (TerrainGroupType *)&ter[c];
                        }
                    }
                    if (HitCnt != 0) {
                        LastWrite[0] = HitCnt;
                        LastWrite[1] = *ttemp;
                        LastWrite = (short *)HitData;
                        HitData++;
                        HitCnt = 0;
                    }
                }
            }
        }
    }
    TerI->PlatScanStart = LastWrite;
    a = 0x100;
    minx = minx - 0.05f;
    maxx = maxx + 0.05f;
    miny = miny - 0.05f;
    maxy = maxy + 0.05f;
    minz = minz - 0.05f;
    maxz = maxz + 0.05f;
    if (platscan != 0) {
        ttemp = &CurTerr->terrlist[CurTerr->terrgroup[a].start];
        for (b = 0; b < CurTerr->terrgroup[a].count; b++, ttemp++) {
            mbuf2 = &CurTerr->terr[*ttemp];
            if (CurTerr->platdata[mbuf2->info].curmtx != 0) {
                if (CurTerr->platdata[mbuf2->info].curmtx->_30 > CurTerr->platdata[mbuf2->info].oldmtx._30) {
                    tminx = minx - mbuf2->Location.x;
                    tmaxx = (maxx + (CurTerr->platdata[mbuf2->info].curmtx->_30 -
                                     CurTerr->platdata[mbuf2->info].oldmtx._30) * 1.5f) -
                            mbuf2->Location.x;
                } else {
                    tmaxx = maxx - mbuf2->Location.x;
                    tminx = (minx + (CurTerr->platdata[mbuf2->info].curmtx->_30 -
                                     CurTerr->platdata[mbuf2->info].oldmtx._30) * 1.5f) -
                            mbuf2->Location.x;
                }
                if (CurTerr->platdata[mbuf2->info].curmtx->_31 > CurTerr->platdata[mbuf2->info].oldmtx._31) {
                    tminy = miny - mbuf2->Location.y;
                    tmaxy = (maxy + (CurTerr->platdata[mbuf2->info].curmtx->_31 -
                                     CurTerr->platdata[mbuf2->info].oldmtx._31) * 1.5f) -
                            mbuf2->Location.y;
                } else {
                    tmaxy = maxy - mbuf2->Location.y;
                    tminy = (miny + (CurTerr->platdata[mbuf2->info].curmtx->_31 -
                                     CurTerr->platdata[mbuf2->info].oldmtx._31) * 1.5f) -
                            mbuf2->Location.y;
                }
                if (CurTerr->platdata[mbuf2->info].curmtx->_32 > CurTerr->platdata[mbuf2->info].oldmtx._32) {
                    tminz = minz - mbuf2->Location.z;
                    tmaxz = (maxz + (CurTerr->platdata[mbuf2->info].curmtx->_32 -
                                     CurTerr->platdata[mbuf2->info].oldmtx._32) * 1.5f) -
                            mbuf2->Location.z;
                } else {
                    tmaxz = maxz - mbuf2->Location.z;
                    tminz = (minz + (CurTerr->platdata[mbuf2->info].curmtx->_32 -
                                     CurTerr->platdata[mbuf2->info].oldmtx._32) * 1.5f) -
                            mbuf2->Location.z;
                }
            } else {
                tmaxx = maxx - mbuf2->Location.x;
                tminx = minx - mbuf2->Location.x;
                tmaxy = maxy - mbuf2->Location.y;
                tminy = miny - mbuf2->Location.y;
                tmaxz = maxz - mbuf2->Location.z;
                tminz = minz - mbuf2->Location.z;
            }
            if (CurTerr->platdata[mbuf2->info].status.bit.rotate == 0) {
                if ((tmaxx >= mbuf2->min.x) && (tmaxy >= mbuf2->min.y) && (tmaxz >= mbuf2->min.z) &&
                    (tminx <= mbuf2->max.x) && (tminy < mbuf2->max.y) && (tminz < mbuf2->max.z) &&
                    (mbuf2->type != -1)) {
                    group = (TerrainGroupType *)mbuf2->model;
                    while (group->start >= 0) {
                        c = group->count;
                        ter = (tertype *)(group + 1);
                        if ((tmaxx >= group->bounds[0]) && (tminx < group->bounds[1]) &&
                            (tmaxz >= group->bounds[2]) && (tminz < group->bounds[3])) {
                            for (; c > 0; c--) {
                                if ((tmaxx >= ter->minx) && (tminx < ter->maxx) && (tmaxz >= ter->minz) &&
                                    (tminz < ter->maxz) && (tmaxy >= ter->miny) && (tminy < ter->maxy) &&
                                    (platinrange = 1, (HitData < MaxData) &&
                                                          ((ter->info[1] == 0) ||
                                                           ((ter->info[1] & extramask) != 0)))) {
                                    *HitData++ = ter;
                                    HitCnt++;
                                }
                                ter++;
                            }
                            group = (TerrainGroupType *)ter;
                        } else {
                            group = (TerrainGroupType *)&ter[c];
                        }
                    }
                }
            } else {
                tn = ((maxx + minx) * 0.5f - mbuf2->Location.x) * ((maxx + minx) * 0.5f - mbuf2->Location.x) +
                     (((maxy + miny) * 0.5f - mbuf2->Location.y) * TerI->inyscale) *
                         (((maxy + miny) * 0.5f - mbuf2->Location.y) * TerI->inyscale) +
                     ((maxz + minz) * 0.5f - mbuf2->Location.z) * ((maxz + minz) * 0.5f - mbuf2->Location.z);
                if ((mbuf2->type != -1) && (tn < mbuf2->radius + radmov)) {
                    group = (TerrainGroupType *)mbuf2->model;
                    while (group->start >= 0) {
                        c = group->count;
                        ter = (tertype *)(group + 1);
                        for (; c > 0; c--) {
                            platinrange = 1;
                            if ((HitData < MaxData) &&
                                ((ter->info[1] == 0) || ((ter->info[1] & extramask) != 0))) {
                                pnts[0].x = ter->pnts[0].x;
                                pnts[0].y = ter->pnts[0].y;
                                pnts[0].z = ter->pnts[0].z;
                                pnts[0].w = 0.0f;
                                pnts[1].x = ter->pnts[1].x;
                                pnts[1].y = ter->pnts[1].y;
                                pnts[1].z = ter->pnts[1].z;
                                pnts[1].w = 0.0f;
                                pnts[2].x = ter->pnts[2].x;
                                pnts[2].y = ter->pnts[2].y;
                                pnts[2].z = ter->pnts[2].z;
                                pnts[2].w = 0.0f;
                                NuVec4MtxTransformVU0(&pnts[0], &pnts[0], CurTerr->platdata[mbuf2->info].curmtx);
                                NuVec4MtxTransformVU0(&pnts[1], &pnts[1], CurTerr->platdata[mbuf2->info].curmtx);
                                NuVec4MtxTransformVU0(&pnts[2], &pnts[2], CurTerr->platdata[mbuf2->info].curmtx);
                                norm[0].x = ter->norm[0].x;
                                norm[0].y = ter->norm[0].y;
                                norm[0].z = ter->norm[0].z;
                                norm[0].w = 0.0f;
                                NuVec4MtxTransformVU0(&norm[0], &norm[0], CurTerr->platdata[mbuf2->info].curmtx);
                                if (ter->norm[1].y < 65535.0f) {
                                    pnts[3].x = ter->pnts[3].x;
                                    pnts[3].y = ter->pnts[3].y;
                                    pnts[3].z = ter->pnts[3].z;
                                    pnts[3].w = 0.0f;
                                    NuVec4MtxTransformVU0(&pnts[3], &pnts[3], CurTerr->platdata[mbuf2->info].curmtx);
                                    norm[1].x = ter->norm[1].x;
                                    norm[1].y = ter->norm[1].y;
                                    norm[1].z = ter->norm[1].z;
                                    norm[1].w = 0.0f;
                                    NuVec4MtxTransformVU0(&norm[1], &norm[1], CurTerr->platdata[mbuf2->info].curmtx);
                                } else {
                                    pnts[3] = pnts[2];
                                }
                                ScaleTerrain[curscltemp].info[0] = ter->info[0];
                                ScaleTerrain[curscltemp].info[1] = ter->info[1];
                                ScaleTerrain[curscltemp].info[2] = ter->info[2];
                                ScaleTerrain[curscltemp].info[3] = ter->info[3];
                                ScaleTerrain[curscltemp].pnts[0].x = pnts[0].x;
                                ScaleTerrain[curscltemp].pnts[0].z = pnts[0].z;
                                ScaleTerrain[curscltemp].pnts[0].y = pnts[0].y;
                                ScaleTerrain[curscltemp].pnts[1].x = pnts[1].x;
                                ScaleTerrain[curscltemp].pnts[1].z = pnts[1].z;
                                ScaleTerrain[curscltemp].pnts[1].y = pnts[1].y;
                                ScaleTerrain[curscltemp].pnts[2].x = pnts[2].x;
                                ScaleTerrain[curscltemp].pnts[2].z = pnts[2].z;
                                ScaleTerrain[curscltemp].pnts[2].y = pnts[2].y;
                                if (ter->norm[1].y < 65535.0f) {
                                    ScaleTerrain[curscltemp].pnts[3].x = pnts[3].x;
                                    ScaleTerrain[curscltemp].pnts[3].z = pnts[3].z;
                                    ScaleTerrain[curscltemp].pnts[3].y = pnts[3].y;
                                    tn = 1.0f / NuFsqrt(norm[1].x * norm[1].x + norm[1].y * norm[1].y +
                                                        norm[1].z * norm[1].z);
                                    ScaleTerrain[curscltemp].norm[1].x = norm[1].x * tn;
                                    ScaleTerrain[curscltemp].norm[1].y = norm[1].y * tn;
                                    ScaleTerrain[curscltemp].norm[1].z = norm[1].z * tn;
                                } else {
                                    ScaleTerrain[curscltemp].norm[1].y = 65536.0f;
                                }
                                tn = 1.0f / NuFsqrt(norm[0].x * norm[0].x + norm[0].y * norm[0].y +
                                                    norm[0].z * norm[0].z);
                                ScaleTerrain[curscltemp].norm[0].x = norm[0].x * tn;
                                ScaleTerrain[curscltemp].norm[0].y = norm[0].y * tn;
                                ScaleTerrain[curscltemp].norm[0].z = norm[0].z * tn;
                                *HitData++ = &ScaleTerrain[curscltemp];
                                HitCnt++;
                                curscltemp++;
                            }
                            ter++;
                        }
                        group = (TerrainGroupType *)ter;
                    }
                }
            }
            if (HitCnt != 0) {
                LastWrite[0] = HitCnt;
                LastWrite[1] = *ttemp;
                LastWrite = (short *)HitData;
                HitData++;
                HitCnt = 0;
            }
        }
    }
    LastWrite[0] = 0;
    LastWrite[1] = 0;
    walldata = (struct nuvec_s **)(LastWrite + SCAN_SLOT);
    minx = minx - 0.02;
    minz = minz - 0.02;
    maxx = maxx + 0.02;
    maxz = maxz + 0.02;
    for (WallSpl = CurTerr->wallinfo; WallSpl != 0; WallSpl = ((WallSplineType **)WallSpl)[-1]) {
        for (a = 0; a < WallSpl->count; a += 0x10) {
            if (WallSpl->spl[a].y != 2147483648.0f) {
                if ((WallSpl->spl[a + 1].y >= minx) && (WallSpl->spl[a].y <= maxx) &&
                    (WallSpl->spl[a + 3].y >= minz) && (WallSpl->spl[a + 2].y <= maxz)) {
                    c = a + 0x10;
                    if (c > WallSpl->count) {
                        c = WallSpl->count;
                    }
                } else {
                    c = 0;
                }
            } else {
                c = WallSpl->count;
            }
            for (b = a; b < c; b++) {
                if ((((WallSpl->spl[b].x >= minx) && (WallSpl->spl[b + 1].x <= maxx)) ||
                     ((WallSpl->spl[b + 1].x >= minx) && (WallSpl->spl[b].x <= maxx))) &&
                    (((WallSpl->spl[b].z >= minz) && (WallSpl->spl[b + 1].z <= maxz)) ||
                     ((WallSpl->spl[b + 1].z >= minz) && (WallSpl->spl[b].z <= maxz))) &&
                    (WallSplCount < 0x40)) {
                    *walldata++ = &WallSpl->spl[b];
                }
            }
        }
    }
    LastWrite = (short *)walldata;
#ifdef NW_HOST /* the wall list ends in a null pointer slot */
    *walldata = 0;
#endif
    LastWrite[0] = 0;
    LastWrite[1] = 0;
    TempStackPtr = LastWrite + SCAN_SLOT;
    return scandata;
}

short *NewScanHandelSubset(short *handel, struct nuvec_s *vpos, struct nuvec_s *vvel, s32 extramask,
                           f32 size) {
    s32 HitCnt;
    s32 lp;
    s32 terrid;
    tertype **HitData;
    tertype **MaxData;
    short *LastWrite;
    short *scandata;
    struct nuvec_s **walldata;
    struct nuvec_s **spl;
    short *CurData;
    tertype **curter;
    TerrainSituType *mbuf2;
    f32 maxx;
    f32 maxy;
    f32 maxz;
    f32 minx;
    f32 miny;
    f32 minz;
    f32 tmaxx;
    f32 tmaxy;
    f32 tmaxz;
    f32 tminx;
    f32 tminy;
    f32 tminz;
    f32 radmov;
    f32 tn;

    if (handel == 0) {
        return 0;
    }
    scandata = LastWrite = TempStackPtr;
    HitData = (tertype **)(LastWrite + SCAN_SLOT);
    MaxData = HitData + 0x1FC;
    if (HitData > ScanHitLimit) {
        return 0;
    }
    if (&ScanHitLimit[100] < MaxData) {
        MaxData = &ScanHitLimit[100];
    }
    if (vvel->x > 0.0f) {
        minx = vpos->x - 0.02f - size;
        maxx = vpos->x + vvel->x + 0.02f + size;
    } else {
        minx = vpos->x + vvel->x - 0.02f - size;
        maxx = vpos->x + 0.02f + size;
    }
    if (vvel->y > 0.0f) {
        miny = vpos->y - 0.02f - size;
        maxy = vpos->y + vvel->y + 0.02f + size;
    } else {
        miny = vpos->y + vvel->y - 0.02f - size;
        maxy = vpos->y + 0.02f + size;
    }
    if (vvel->z > 0.0f) {
        minz = vpos->z - 0.02f - size;
        maxz = vpos->z + vvel->z + 0.02f + size;
    } else {
        minz = vpos->z + vvel->z - 0.02f - size;
        maxz = vpos->z + 0.02f + size;
    }
    radmov = (minx - maxx) * (minx - maxx) + (miny - maxy) * (miny - maxy) + (minz - maxz) * (minz - maxz);
    radmov += 0.02f;
    HitCnt = 0;
    CurData = handel;
    while (*CurData > 0) {
        terrid = CurData[1];
        mbuf2 = &CurTerr->terr[CurData[1]];
        if ((mbuf2->type == 1) && (CurTerr->platdata[mbuf2->info].curmtx != 0)) {
            if (CurTerr->platdata[mbuf2->info].curmtx->_30 > CurTerr->platdata[mbuf2->info].oldmtx._30) {
                tminx = minx - mbuf2->Location.x;
                tmaxx = (maxx + (CurTerr->platdata[mbuf2->info].curmtx->_30 -
                                 CurTerr->platdata[mbuf2->info].oldmtx._30) * 1.5f) -
                        mbuf2->Location.x;
            } else {
                tmaxx = maxx - mbuf2->Location.x;
                tminx = (minx + (CurTerr->platdata[mbuf2->info].curmtx->_30 -
                                 CurTerr->platdata[mbuf2->info].oldmtx._30) * 1.5f) -
                        mbuf2->Location.x;
            }
            if (CurTerr->platdata[mbuf2->info].curmtx->_31 > CurTerr->platdata[mbuf2->info].oldmtx._31) {
                tminy = miny - mbuf2->Location.y;
                tmaxy = (maxy + (CurTerr->platdata[mbuf2->info].curmtx->_31 -
                                 CurTerr->platdata[mbuf2->info].oldmtx._31) * 1.5f) -
                        mbuf2->Location.y;
            } else {
                tmaxy = maxy - mbuf2->Location.y;
                tminy = (miny + (CurTerr->platdata[mbuf2->info].curmtx->_31 -
                                 CurTerr->platdata[mbuf2->info].oldmtx._31) * 1.5f) -
                        mbuf2->Location.y;
            }
            if (CurTerr->platdata[mbuf2->info].curmtx->_32 > CurTerr->platdata[mbuf2->info].oldmtx._32) {
                tminz = minz - mbuf2->Location.z;
                tmaxz = (maxz + (CurTerr->platdata[mbuf2->info].curmtx->_32 -
                                 CurTerr->platdata[mbuf2->info].oldmtx._32) * 1.5f) -
                        mbuf2->Location.z;
            } else {
                tmaxz = maxz - mbuf2->Location.z;
                tminz = (minz + (CurTerr->platdata[mbuf2->info].curmtx->_32 -
                                 CurTerr->platdata[mbuf2->info].oldmtx._32) * 1.5f) -
                        mbuf2->Location.z;
            }
            if (CurTerr->platdata[mbuf2->info].status.bit.rotate == 0) {
                if ((tmaxx >= mbuf2->min.x) && (tmaxy >= mbuf2->min.y) && (tmaxz >= mbuf2->min.z) &&
                    (tminx <= mbuf2->max.x) && (tminy < mbuf2->max.y) && (tminz < mbuf2->max.z)) {
                    curter = (tertype **)(CurData + SCAN_SLOT);
                    for (lp = *CurData; lp > 0; lp--, curter++) {
                        if ((tmaxx >= (*curter)->minx) && (tminx < (*curter)->maxx) &&
                            (tmaxz >= (*curter)->minz) && (tminz < (*curter)->maxz) &&
                            (tmaxy >= (*curter)->miny) && (tminy < (*curter)->maxy) &&
                            (HitData < MaxData) &&
                            (((*curter)->info[1] == 0) || (((*curter)->info[1] & extramask) != 0))) {
                            *HitData++ = *curter;
                            HitCnt++;
                        }
                    }
                } else {
                    curter = (tertype **)(CurData + SCAN_SLOT);
                    curter += *CurData;
                }
            } else {
                tn = ((maxx + minx) * 0.5f - mbuf2->Location.x) * ((maxx + minx) * 0.5f - mbuf2->Location.x) +
                     (((maxy + miny) * 0.5f - mbuf2->Location.y) * TerI->inyscale) *
                         (((maxy + miny) * 0.5f - mbuf2->Location.y) * TerI->inyscale) +
                     ((maxz + minz) * 0.5f - mbuf2->Location.z) * ((maxz + minz) * 0.5f - mbuf2->Location.z);
                if (tn < mbuf2->radius + radmov) {
                    curter = (tertype **)(CurData + SCAN_SLOT);
                    for (lp = *CurData; lp > 0; lp--, curter++) {
                        if ((HitData < MaxData) &&
                            (((*curter)->info[1] == 0) || (((*curter)->info[1] & extramask) != 0)) &&
                            (((*curter)->pnts[0].x > tminx) || ((*curter)->pnts[1].x > tminx) ||
                             ((*curter)->pnts[2].x > tminx) || ((*curter)->pnts[3].x > tminx)) &&
                            (((*curter)->pnts[0].x < tmaxx) || ((*curter)->pnts[1].x < tmaxx) ||
                             ((*curter)->pnts[2].x < tmaxx) || ((*curter)->pnts[3].x < tmaxx)) &&
                            (((*curter)->pnts[0].z > tminz) || ((*curter)->pnts[1].z > tminz) ||
                             ((*curter)->pnts[2].z > tminz) || ((*curter)->pnts[3].z > tminz)) &&
                            (((*curter)->pnts[0].z < tmaxz) || ((*curter)->pnts[1].z < tmaxz) ||
                             ((*curter)->pnts[2].z < tmaxz) || ((*curter)->pnts[3].z < tmaxz))) {
                            *HitData++ = *curter;
                            HitCnt++;
                        }
                    }
                } else {
                    curter = (tertype **)(CurData + SCAN_SLOT);
                    curter += *CurData;
                }
            }
        } else {
            tmaxx = maxx - mbuf2->Location.x;
            tminx = minx - mbuf2->Location.x;
            tmaxy = maxy - mbuf2->Location.y;
            tminy = miny - mbuf2->Location.y;
            tmaxz = maxz - mbuf2->Location.z;
            tminz = minz - mbuf2->Location.z;
            if ((maxx >= mbuf2->min.x) && (maxy >= mbuf2->min.y) && (maxz >= mbuf2->min.z) &&
                (minx <= mbuf2->max.x) && (miny < mbuf2->max.y) && (minz < mbuf2->max.z)) {
                curter = (tertype **)(CurData + SCAN_SLOT);
                for (lp = *CurData; lp > 0; lp--, curter++) {
                    if ((tmaxx >= (*curter)->minx) && (tminx < (*curter)->maxx) &&
                        (tmaxz >= (*curter)->minz) && (tminz < (*curter)->maxz) &&
                        (tmaxy >= (*curter)->miny) && (tminy < (*curter)->maxy) &&
                        (HitData < MaxData) &&
                        (((*curter)->info[1] == 0) || (((*curter)->info[1] & extramask) != 0))) {
                        *HitData++ = *curter;
                        HitCnt++;
                    }
                }
            } else {
                curter = (tertype **)(CurData + SCAN_SLOT);
                curter += *CurData;
            }
        }
        if (HitCnt != 0) {
            LastWrite[0] = HitCnt;
            LastWrite[1] = terrid;
            LastWrite = (short *)HitData;
            HitData++;
            HitCnt = 0;
        }
        CurData = (short *)curter;
    }
    LastWrite[0] = 0;
    LastWrite[1] = 0;
    walldata = (struct nuvec_s **)(LastWrite + SCAN_SLOT);
    CurData += SCAN_SLOT;
    minx = minx - 0.02;
    minz = minz - 0.02;
    maxx = maxx + 0.02;
    maxz = maxz + 0.02;
    while (*(nusintptr *)CurData > 0) {
        spl = (struct nuvec_s **)CurData;
        if (((((*spl)[0].x >= minx) && ((*spl)[1].x <= maxx)) ||
             (((*spl)[1].x >= minx) && ((*spl)[0].x <= maxx))) &&
            ((((*spl)[0].z >= minz) && ((*spl)[1].z <= maxz)) ||
             (((*spl)[1].z >= minz) && ((*spl)[0].z <= maxz)))) {
            *walldata++ = *spl;
        }
        CurData += SCAN_SLOT;
    }
    LastWrite = (short *)walldata;
#ifdef NW_HOST /* the wall list ends in a null pointer slot */
    *walldata = 0;
#endif
    LastWrite[0] = 0;
    LastWrite[1] = 0;
    TempStackPtr = LastWrite + SCAN_SLOT;
    return scandata;
}

void ScanTerrainHandel(s32 extramask, short *Handel) {
    s32 HitCnt;
    s32 lp;
    s32 terrid;
    tertype **HitData;
    tertype **MaxData;
    short *LastWrite;
    struct nuvec_s **spl;
    short *CurData;
    tertype **curter;
    TerrainSituType *mbuf2;
    f32 maxx;
    f32 maxy;
    f32 maxz;
    f32 minx;
    f32 miny;
    f32 minz;
    f32 tmaxx;
    f32 tmaxy;
    f32 tmaxz;
    f32 tminx;
    f32 tminy;
    f32 tminz;
    f32 radmov;
    f32 tn;

    if (Handel == 0) {
        return;
    }
    TerI->hitcnt = 0;
    LastWrite = (short *)TerI->hitdata;
    HitData = TerI->hitdata + 1;
    MaxData = HitData + 0x1FC;
    platinrange = 0;
    TerI->plathit = -1;
    switch (TerI->scanmode) {
        default:
            radmov = TerI->curvel.x * TerI->curvel.x + TerI->curvel.y * TerI->curvel.y +
                     TerI->curvel.z * TerI->curvel.z;
            radmov += TerI->sizesq + 0.02f;
            maxx = TerI->curpos.x + radmov;
            maxy = TerI->curpos.y + radmov * TerI->yscale;
            maxz = TerI->curpos.z + radmov;
            minx = TerI->curpos.x - radmov;
            miny = TerI->curpos.y - radmov * TerI->yscale;
            minz = TerI->curpos.z - radmov;
            break;
        case 1:
            if (TerI->curvel.x > 0.0f) {
                minx = TerI->curpos.x - 0.02f - TerI->size;
                maxx = TerI->curpos.x + TerI->curvel.x + 0.02f + TerI->size;
            } else {
                minx = TerI->curpos.x + TerI->curvel.x - 0.02f - TerI->size;
                maxx = TerI->curpos.x + 0.02f + TerI->size;
            }
            if (TerI->curvel.y > 0.0f) {
                miny = TerI->curpos.y - 0.02f - TerI->size;
                maxy = TerI->curpos.y + TerI->curvel.y + 0.02f + TerI->size;
            } else {
                miny = TerI->curpos.y + TerI->curvel.y - 0.02f - TerI->size;
                maxy = TerI->curpos.y + 0.02f + TerI->size;
            }
            if (TerI->curvel.z > 0.0f) {
                minz = TerI->curpos.z - 0.02f - TerI->size;
                maxz = TerI->curpos.z + TerI->curvel.z + 0.02f + TerI->size;
            } else {
                minz = TerI->curpos.z + TerI->curvel.z - 0.02f - TerI->size;
                maxz = TerI->curpos.z + 0.02f + TerI->size;
            }
            radmov = (minx - maxx) * (minx - maxx) + (miny - maxy) * (miny - maxy) +
                     (minz - maxz) * (minz - maxz);
            radmov += 0.02f;
            break;
    }
    HitCnt = 0;
    CurData = Handel;
    while (*CurData > 0) {
        terrid = CurData[1];
        mbuf2 = &CurTerr->terr[CurData[1]];
        curter = (tertype **)(CurData + SCAN_SLOT);
        if ((mbuf2->type == 1) && (CurTerr->platdata[mbuf2->info].curmtx != 0)) {
            if (CurTerr->platdata[mbuf2->info].curmtx->_30 > CurTerr->platdata[mbuf2->info].oldmtx._30) {
                tminx = minx - mbuf2->Location.x;
                tmaxx = (maxx + (CurTerr->platdata[mbuf2->info].curmtx->_30 -
                                 CurTerr->platdata[mbuf2->info].oldmtx._30) * 1.5f) -
                        mbuf2->Location.x;
            } else {
                tmaxx = maxx - mbuf2->Location.x;
                tminx = (minx + (CurTerr->platdata[mbuf2->info].curmtx->_30 -
                                 CurTerr->platdata[mbuf2->info].oldmtx._30) * 1.5f) -
                        mbuf2->Location.x;
            }
            if (CurTerr->platdata[mbuf2->info].curmtx->_31 > CurTerr->platdata[mbuf2->info].oldmtx._31) {
                tminy = miny - mbuf2->Location.y;
                tmaxy = (maxy + (CurTerr->platdata[mbuf2->info].curmtx->_31 -
                                 CurTerr->platdata[mbuf2->info].oldmtx._31) * 1.5f) -
                        mbuf2->Location.y;
            } else {
                tmaxy = maxy - mbuf2->Location.y;
                tminy = (miny + (CurTerr->platdata[mbuf2->info].curmtx->_31 -
                                 CurTerr->platdata[mbuf2->info].oldmtx._31) * 1.5f) -
                        mbuf2->Location.y;
            }
            if (CurTerr->platdata[mbuf2->info].curmtx->_32 > CurTerr->platdata[mbuf2->info].oldmtx._32) {
                tminz = minz - mbuf2->Location.z;
                tmaxz = (maxz + (CurTerr->platdata[mbuf2->info].curmtx->_32 -
                                 CurTerr->platdata[mbuf2->info].oldmtx._32) * 1.5f) -
                        mbuf2->Location.z;
            } else {
                tmaxz = maxz - mbuf2->Location.z;
                tminz = (minz + (CurTerr->platdata[mbuf2->info].curmtx->_32 -
                                 CurTerr->platdata[mbuf2->info].oldmtx._32) * 1.5f) -
                        mbuf2->Location.z;
            }
            if (mbuf2->info == TerrPlatDis) {
                curter = (tertype **)(CurData + SCAN_SLOT);
                curter += *CurData;
            } else if (CurTerr->platdata[mbuf2->info].status.bit.rotate == 0) {
                if ((tmaxx >= mbuf2->min.x) && (tmaxy >= mbuf2->min.y) && (tmaxz >= mbuf2->min.z) &&
                    (tminx <= mbuf2->max.x) && (tminy < mbuf2->max.y) && (tminz < mbuf2->max.z)) {
                    for (lp = *CurData; lp > 0; lp--, curter++) {
                        if ((tmaxx >= (*curter)->minx) && (tminx < (*curter)->maxx) &&
                            (tmaxz >= (*curter)->minz) && (tminz < (*curter)->maxz) &&
                            (tmaxy >= (*curter)->miny) && (tminy < (*curter)->maxy) &&
                            (HitData < MaxData) &&
                            (((*curter)->info[1] == 0) || (((*curter)->info[1] & extramask) != 0))) {
                            *HitData++ = *curter;
                            HitCnt++;
                        }
                    }
                } else {
                    curter = (tertype **)(CurData + SCAN_SLOT);
                    curter += *CurData;
                }
            } else {
                tn = ((maxx + minx) * 0.5f - mbuf2->Location.x) * ((maxx + minx) * 0.5f - mbuf2->Location.x) +
                     (((maxy + miny) * 0.5f - mbuf2->Location.y) * TerI->inyscale) *
                         (((maxy + miny) * 0.5f - mbuf2->Location.y) * TerI->inyscale) +
                     ((maxz + minz) * 0.5f - mbuf2->Location.z) * ((maxz + minz) * 0.5f - mbuf2->Location.z);
                if (tn < mbuf2->radius + radmov) {
                    for (lp = *CurData; lp > 0; lp--, curter++) {
                        if ((HitData < MaxData) &&
                            (((*curter)->info[1] == 0) || (((*curter)->info[1] & extramask) != 0)) &&
                            (((*curter)->pnts[0].x > tminx) || ((*curter)->pnts[1].x > tminx) ||
                             ((*curter)->pnts[2].x > tminx) || ((*curter)->pnts[3].x > tminx)) &&
                            (((*curter)->pnts[0].x < tmaxx) || ((*curter)->pnts[1].x < tmaxx) ||
                             ((*curter)->pnts[2].x < tmaxx) || ((*curter)->pnts[3].x < tmaxx)) &&
                            (((*curter)->pnts[0].z > tminz) || ((*curter)->pnts[1].z > tminz) ||
                             ((*curter)->pnts[2].z > tminz) || ((*curter)->pnts[3].z > tminz)) &&
                            (((*curter)->pnts[0].z < tmaxz) || ((*curter)->pnts[1].z < tmaxz) ||
                             ((*curter)->pnts[2].z < tmaxz) || ((*curter)->pnts[3].z < tmaxz))) {
                            *HitData++ = *curter;
                            HitCnt++;
                        }
                    }
                } else {
                    curter = (tertype **)(CurData + SCAN_SLOT);
                    curter += *CurData;
                }
            }
        } else if ((maxx >= mbuf2->min.x) && (maxy >= mbuf2->min.y) && (maxz >= mbuf2->min.z) &&
                   (minx <= mbuf2->max.x) && (miny < mbuf2->max.y) && (minz < mbuf2->max.z)) {
            tmaxx = maxx - mbuf2->Location.x;
            tminx = minx - mbuf2->Location.x;
            tmaxy = maxy - mbuf2->Location.y;
            tminy = miny - mbuf2->Location.y;
            tmaxz = maxz - mbuf2->Location.z;
            tminz = minz - mbuf2->Location.z;
            for (lp = *CurData; lp > 0; lp--, curter++) {
                if ((tmaxx >= (*curter)->minx) && (tminx < (*curter)->maxx) &&
                    (tmaxz >= (*curter)->minz) && (tminz < (*curter)->maxz) &&
                    (tmaxy >= (*curter)->miny) && (tminy < (*curter)->maxy) &&
                    (HitData < MaxData) &&
                    (((*curter)->info[1] == 0) || (((*curter)->info[1] & extramask) != 0))) {
                    *HitData++ = *curter;
                    HitCnt++;
                }
            }
        } else {
            curter = (tertype **)(CurData + SCAN_SLOT);
            curter += *CurData;
        }
        if (HitCnt != 0) {
            LastWrite[0] = HitCnt;
            LastWrite[1] = terrid;
            LastWrite = (short *)HitData;
            HitData++;
            HitCnt = 0;
        }
        CurData = (short *)curter;
    }
    LastWrite[0] = 0;
    LastWrite[1] = 0;
    CurData += SCAN_SLOT;
    minx = minx - 0.02;
    minz = minz - 0.02;
    maxx = maxx + 0.02;
    maxz = maxz + 0.02;
    WallSplCount = 0;
    while (*(nusintptr *)CurData != 0) {
        spl = (struct nuvec_s **)CurData;
        if (((((*spl)[0].x >= minx) && ((*spl)[1].x <= maxx)) ||
             (((*spl)[1].x >= minx) && ((*spl)[0].x <= maxx))) &&
            ((((*spl)[0].z >= minz) && ((*spl)[1].z <= maxz)) ||
             (((*spl)[1].z >= minz) && ((*spl)[0].z <= maxz))) &&
            (WallSplCount < 0x40)) {
            WallSplList[WallSplCount] = (*spl)[0];
            WallSplCount++;
            WallSplList[WallSplCount] = (*spl)[1];
            WallSplCount++;
        }
        CurData += SCAN_SLOT;
    }
    TerrPlatDis = -1;
}

s32 NewRayCastSetHandel(struct nuvec_s *vpos, struct nuvec_s *vvel, f32 size, f32 timeadj,
                        f32 impactadj, short *handel) {
    if (CurTerr == 0) {
        return 0;
    }
    if (handel == 0) {
        return 0;
    }
    plathitid = -1;
    TerrPolyObj = -1;
    TerrPoly = 0;
    TerI = (TerTempInfoType *)NuScratchAlloc32(sizeof(TerTempInfoType));
    TerI->inyscalesq = TerI->inyscale = TerI->yscalesq = TerI->yscale = 1.0f;
    TerI->size = size;
    TerI->sizediv = 1.0f / TerI->size;
    TerI->sizesq = TerI->size * TerI->size;
    TerI->origpos.x = TerI->curpos.x = vpos->x;
    TerI->origpos.y = TerI->curpos.y = vpos->y;
    TerI->origpos.z = TerI->curpos.z = vpos->z;
    TerI->origvel = TerI->curvel = *vvel;
    TerI->id = -1;
    TerI->flags = 0;
    TerI->scanmode = 1;
    TerI->timeadj = timeadj;
    TerI->impactadj = impactadj;
    ScanTerrainHandel(0, handel);
    DerotateMovementVector();
    HitTerrain();
    if (TerI->hittype != 0) {
        RayImpact(vvel);
        TerrainImpactNorm();
        ShadNorm = TerI->hitnorm;
    }
    NuScratchRelease();
    return TerI->hittype;
}


s32 TerrainTrackBack(struct nuvec_s *pos, struct nuvec_s *vel, struct nuvec_s *dest, f32 size,
                     f32 timeadj) {
    if ((CurTerr == 0) || (TerrPolyObj == -1)) {
        return 0;
    }
    TerI = (TerTempInfoType *)NuScratchAlloc32(sizeof(TerTempInfoType));
    TerI->inyscalesq = TerI->inyscale = TerI->yscalesq = TerI->yscale = 1.0f;
    TerI->size = size;
    TerI->sizediv = 1.0f / TerI->size;
    TerI->sizesq = TerI->size * TerI->size;
    TerI->curpos.x = pos->x + vel->x * (size + 0.5f);
    TerI->curpos.y = pos->y + vel->y * (size + 0.5f);
    TerI->curpos.z = pos->z + vel->z * (size + 0.5f);
    TerI->curvel.x = -vel->x * (size + 0.8f);
    TerI->curvel.y = -vel->y * (size + 0.8f);
    TerI->curvel.z = -vel->z * (size + 0.8f);
    TerI->id = -1;
    TerI->flags = 0;
    DerotateMovementVector();
    HitTerrPoly(TerrPoly, TerrPolyObj);
    switch (TerI->hittype) {
        case 1:
        case 2:
        case 3:
        case 4:
            TerI->hittime -= timeadj;
            if (TerI->hittime < 0.0f) {
                TerI->hittime = 0.0f;
            }
            dest->x = TerI->curpos.x + TerI->curvel.x * TerI->hittime;
            dest->y = TerI->curpos.y + TerI->curvel.y * TerI->hittime;
            dest->z = TerI->curpos.z + TerI->curvel.z * TerI->hittime;
            break;
        case 0:
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x14:
            dest->x = TerI->curpos.x;
            dest->y = TerI->curpos.y;
            dest->z = TerI->curpos.z;
            break;
    }
    NuScratchRelease();
    return TerI->hittype;
}


void ScanTerrainPlatform(s32 msituid, s32 extramask) {
    s32 hitcnt;
    s32 c;
    tertype **hitdata;
    tertype **maxdata;
    short *lastwrite;
    s32 scalecnt;
    TerrainSituType *situ;
    TerrainGroupType *group;
    tertype *ter;
    f32 maxx;
    f32 maxy;
    f32 maxz;
    f32 minx;
    f32 miny;
    f32 minz;
    f32 tmaxx;
    f32 tmaxy;
    f32 tmaxz;
    f32 tminx;
    f32 tminy;
    f32 tminz;
    f32 radmov;
    f32 mag;
    struct nuvec4_s rotpnts[4];
    struct nuvec4_s rotnorm[2];

    ScaleTerrain = ScaleTerrainT1;
    scalecnt = 0;
    TerI->hitcnt = 0;
    lastwrite = (short *)TerI->hitdata;
    hitdata = TerI->hitdata + 1;
    maxdata = hitdata + 0x1FC;
    platinrange = 0;
    TerI->plathit = -1;
    hitcnt = 0;
    switch (TerI->scanmode) {
        default:
            radmov = TerI->curvel.x * TerI->curvel.x + TerI->curvel.y * TerI->curvel.y +
                     TerI->curvel.z * TerI->curvel.z;
            radmov += TerI->sizesq + 0.02f;
            maxx = TerI->curpos.x + radmov;
            maxy = TerI->curpos.y + radmov * TerI->yscale;
            maxz = TerI->curpos.z + radmov;
            minx = TerI->curpos.x - radmov;
            miny = TerI->curpos.y - radmov * TerI->yscale;
            minz = TerI->curpos.z - radmov;
            break;
        case 1:
            if (TerI->curvel.x > 0.0f) {
                minx = TerI->curpos.x - 0.02f - TerI->size;
                maxx = TerI->curpos.x + TerI->curvel.x + 0.02f + TerI->size;
            } else {
                minx = TerI->curpos.x + TerI->curvel.x - 0.02f - TerI->size;
                maxx = TerI->curpos.x + 0.02f + TerI->size;
            }
            if (TerI->curvel.y > 0.0f) {
                miny = TerI->curpos.y - 0.02f - TerI->size;
                maxy = TerI->curpos.y + TerI->curvel.y + 0.02f + TerI->size;
            } else {
                miny = TerI->curpos.y + TerI->curvel.y - 0.02f - TerI->size;
                maxy = TerI->curpos.y + 0.02f + TerI->size;
            }
            if (TerI->curvel.z > 0.0f) {
                minz = TerI->curpos.z - 0.02f - TerI->size;
                maxz = TerI->curpos.z + TerI->curvel.z + 0.02f + TerI->size;
            } else {
                minz = TerI->curpos.z + TerI->curvel.z - 0.02f - TerI->size;
                maxz = TerI->curpos.z + 0.02f + TerI->size;
            }
            radmov = (minx - maxx) * (minx - maxx) + (miny - maxy) * (miny - maxy) +
                     (minz - maxz) * (minz - maxz);
            radmov += TerI->size + 0.02f;
            break;
    }
    TerI->PlatScanStart = lastwrite;
    minx -= 0.05f;
    maxx += 0.05f;
    miny -= 0.05f;
    maxy += 0.05f;
    minz -= 0.05f;
    maxz += 0.05f;
    situ = &CurTerr->terr[msituid];
    if (CurTerr->platdata[situ->info].curmtx != 0) {
        if (CurTerr->platdata[situ->info].curmtx->_30 > CurTerr->platdata[situ->info].oldmtx._30) {
            tminx = minx - situ->Location.x;
            tmaxx = (maxx + (CurTerr->platdata[situ->info].curmtx->_30 -
                             CurTerr->platdata[situ->info].oldmtx._30) * 1.5f) -
                    situ->Location.x;
        } else {
            tmaxx = maxx - situ->Location.x;
            tminx = (minx + (CurTerr->platdata[situ->info].curmtx->_30 -
                             CurTerr->platdata[situ->info].oldmtx._30) * 1.5f) -
                    situ->Location.x;
        }
        if (CurTerr->platdata[situ->info].curmtx->_31 > CurTerr->platdata[situ->info].oldmtx._31) {
            tminy = miny - situ->Location.y;
            tmaxy = (maxy + (CurTerr->platdata[situ->info].curmtx->_31 -
                             CurTerr->platdata[situ->info].oldmtx._31) * 1.5f) -
                    situ->Location.y;
        } else {
            tmaxy = maxy - situ->Location.y;
            tminy = (miny + (CurTerr->platdata[situ->info].curmtx->_31 -
                             CurTerr->platdata[situ->info].oldmtx._31) * 1.5f) -
                    situ->Location.y;
        }
        if (CurTerr->platdata[situ->info].curmtx->_32 > CurTerr->platdata[situ->info].oldmtx._32) {
            tminz = minz - situ->Location.z;
            tmaxz = (maxz + (CurTerr->platdata[situ->info].curmtx->_32 -
                             CurTerr->platdata[situ->info].oldmtx._32) * 1.5f) -
                    situ->Location.z;
        } else {
            tmaxz = maxz - situ->Location.z;
            tminz = (minz + (CurTerr->platdata[situ->info].curmtx->_32 -
                             CurTerr->platdata[situ->info].oldmtx._32) * 1.5f) -
                    situ->Location.z;
        }
    } else {
        tmaxx = maxx - situ->Location.x;
        tminx = minx - situ->Location.x;
        tmaxy = maxy - situ->Location.y;
        tminy = miny - situ->Location.y;
        tmaxz = maxz - situ->Location.z;
        tminz = minz - situ->Location.z;
    }
    if (CurTerr->platdata[situ->info].status.bit.rotate) {
        tminx = ((maxx + minx) * 0.5f - situ->Location.x) * ((maxx + minx) * 0.5f - situ->Location.x) +
             (((maxy + miny) * 0.5f - situ->Location.y) * TerI->inyscale) *
                 (((maxy + miny) * 0.5f - situ->Location.y) * TerI->inyscale) +
             ((maxz + minz) * 0.5f - situ->Location.z) * ((maxz + minz) * 0.5f - situ->Location.z);
        if ((situ->type != -1) && (tminx < situ->radius + radmov)) {
            group = (TerrainGroupType *)situ->model;
            while (group->start >= 0) {
                ter = (tertype *)(group + 1);
                for (c = group->count; c > 0; c--, ter++) {
                    platinrange = 1;
                    if ((hitdata < maxdata) && ((ter->info[1] == 0) || ((ter->info[1] & extramask) != 0))) {
                        rotpnts[0].x = ter->pnts[0].x;
                        rotpnts[0].y = ter->pnts[0].y;
                        rotpnts[0].z = ter->pnts[0].z;
                        rotpnts[0].w = 0.0f;
                        rotpnts[1].x = ter->pnts[1].x;
                        rotpnts[1].y = ter->pnts[1].y;
                        rotpnts[1].z = ter->pnts[1].z;
                        rotpnts[1].w = 0.0f;
                        rotpnts[2].x = ter->pnts[2].x;
                        rotpnts[2].y = ter->pnts[2].y;
                        rotpnts[2].z = ter->pnts[2].z;
                        rotpnts[2].w = 0.0f;
                        rotpnts[3].x = ter->pnts[3].x;
                        rotpnts[3].y = ter->pnts[3].y;
                        rotpnts[3].z = ter->pnts[3].z;
                        rotpnts[3].w = 0.0f;
                        NuVec4MtxTransformVU0(&rotpnts[0], &rotpnts[0], CurTerr->platdata[situ->info].curmtx);
                        NuVec4MtxTransformVU0(&rotpnts[1], &rotpnts[1], CurTerr->platdata[situ->info].curmtx);
                        NuVec4MtxTransformVU0(&rotpnts[2], &rotpnts[2], CurTerr->platdata[situ->info].curmtx);
                        NuVec4MtxTransformVU0(&rotpnts[3], &rotpnts[3], CurTerr->platdata[situ->info].curmtx);
                        rotnorm[0].x = ter->norm[0].x;
                        rotnorm[0].y = ter->norm[0].y;
                        rotnorm[0].z = ter->norm[0].z;
                        rotnorm[0].w = 0.0f;
                        NuVec4MtxTransformVU0(&rotnorm[0], &rotnorm[0], CurTerr->platdata[situ->info].curmtx);
                        if (ter->norm[1].y < 65535.0f) {
                            rotnorm[1].x = ter->norm[1].x;
                            rotnorm[1].y = ter->norm[1].y;
                            rotnorm[1].z = ter->norm[1].z;
                            rotnorm[1].w = 0.0f;
                            NuVec4MtxTransformVU0(&rotnorm[1], &rotnorm[1], CurTerr->platdata[situ->info].curmtx);
                        }
                        ScaleTerrain[scalecnt].info[0] = ter->info[0];
                        ScaleTerrain[scalecnt].info[1] = ter->info[1];
                        ScaleTerrain[scalecnt].info[2] = ter->info[2];
                        ScaleTerrain[scalecnt].info[3] = ter->info[3];
                        ScaleTerrain[scalecnt].pnts[0].x = rotpnts[0].x;
                        ScaleTerrain[scalecnt].pnts[0].z = rotpnts[0].z;
                        ScaleTerrain[scalecnt].pnts[0].y =
                            (rotpnts[0].y + situ->Location.y) * TerI->inyscale - situ->Location.y;
                        ScaleTerrain[scalecnt].pnts[1].x = rotpnts[1].x;
                        ScaleTerrain[scalecnt].pnts[1].z = rotpnts[1].z;
                        ScaleTerrain[scalecnt].pnts[1].y =
                            (rotpnts[1].y + situ->Location.y) * TerI->inyscale - situ->Location.y;
                        ScaleTerrain[scalecnt].pnts[2].x = rotpnts[2].x;
                        ScaleTerrain[scalecnt].pnts[2].z = rotpnts[2].z;
                        ScaleTerrain[scalecnt].pnts[2].y =
                            (rotpnts[2].y + situ->Location.y) * TerI->inyscale - situ->Location.y;
                        if (ter->norm[1].y < 65535.0f) {
                            ScaleTerrain[scalecnt].pnts[3].x = rotpnts[3].x;
                            ScaleTerrain[scalecnt].pnts[3].z = rotpnts[3].z;
                            ScaleTerrain[scalecnt].pnts[3].y =
                                (rotpnts[3].y + situ->Location.y) * TerI->inyscale - situ->Location.y;
                            mag = 1.0f / NuFsqrt(rotnorm[1].x * rotnorm[1].x +
                                                rotnorm[1].y * rotnorm[1].y * TerI->yscalesq +
                                                rotnorm[1].z * rotnorm[1].z);
                            ScaleTerrain[scalecnt].norm[1].x = rotnorm[1].x * mag;
                            ScaleTerrain[scalecnt].norm[1].y = rotnorm[1].y * TerI->yscale * mag;
                            ScaleTerrain[scalecnt].norm[1].z = rotnorm[1].z * mag;
                        } else {
                            ScaleTerrain[scalecnt].norm[1].y = 65536.0f;
                        }
                        hitcnt++;
                        mag = 1.0f / NuFsqrt(rotnorm[0].x * rotnorm[0].x +
                                            rotnorm[0].y * rotnorm[0].y * TerI->yscalesq +
                                            rotnorm[0].z * rotnorm[0].z);
                        ScaleTerrain[scalecnt].norm[0].x = rotnorm[0].x * mag;
                        ScaleTerrain[scalecnt].norm[0].y = rotnorm[0].y * TerI->yscale * mag;
                        ScaleTerrain[scalecnt].norm[0].z = rotnorm[0].z * mag;
                        *hitdata++ = &ScaleTerrain[scalecnt++];
                    }
                }
                group = (TerrainGroupType *)ter;
            }
            if (hitcnt != 0) {
                lastwrite[0] = hitcnt;
                lastwrite[1] = msituid;
                lastwrite = (short *)hitdata;
            }
        }
    } else if ((tmaxx >= situ->min.x) && (tmaxy >= situ->min.y) && (tmaxz >= situ->min.z) &&
               (tminx <= situ->max.x) && (tminy < situ->max.y) && (tminz < situ->max.z) &&
               (situ->type != -1)) {
        group = (TerrainGroupType *)situ->model;
        while (group->start >= 0) {
            ter = (tertype *)(group + 1);
            c = group->count;
            if ((tmaxx >= group->bounds[0]) && (tminx < group->bounds[1])) {
                if ((tmaxz >= group->bounds[2]) && (tminz < group->bounds[3])) {
                    for (; c > 0; c--, ter++) {
                        if ((tmaxx >= ter->minx) && (tminx < ter->maxx) && (tmaxz >= ter->minz) &&
                            (tminz < ter->maxz) && (tmaxy >= ter->miny) && (tminy < ter->maxy)) {
                            platinrange = 1;
                            if ((hitdata < maxdata) &&
                                ((ter->info[1] == 0) || ((ter->info[1] & extramask) != 0))) {
                                if (TerI->yscale == 1.0f) {
                                    *hitdata++ = ter;
                                    hitcnt++;
                                } else {
                                    ScaleTerrain[scalecnt].info[0] = ter->info[0];
                                    ScaleTerrain[scalecnt].info[1] = ter->info[1];
                                    ScaleTerrain[scalecnt].info[2] = ter->info[2];
                                    ScaleTerrain[scalecnt].info[3] = ter->info[3];
                                    ScaleTerrain[scalecnt].pnts[0].x = ter->pnts[0].x;
                                    ScaleTerrain[scalecnt].pnts[0].z = ter->pnts[0].z;
                                    ScaleTerrain[scalecnt].pnts[0].y =
                                        (ter->pnts[0].y + situ->Location.y) * TerI->inyscale - situ->Location.y;
                                    ScaleTerrain[scalecnt].pnts[1].x = ter->pnts[1].x;
                                    ScaleTerrain[scalecnt].pnts[1].z = ter->pnts[1].z;
                                    ScaleTerrain[scalecnt].pnts[1].y =
                                        (ter->pnts[1].y + situ->Location.y) * TerI->inyscale - situ->Location.y;
                                    ScaleTerrain[scalecnt].pnts[2].x = ter->pnts[2].x;
                                    ScaleTerrain[scalecnt].pnts[2].z = ter->pnts[2].z;
                                    ScaleTerrain[scalecnt].pnts[2].y =
                                        (ter->pnts[2].y + situ->Location.y) * TerI->inyscale - situ->Location.y;
                                    if (ter->norm[1].y < 65535.0f) {
                                        ScaleTerrain[scalecnt].pnts[3].x = ter->pnts[3].x;
                                        ScaleTerrain[scalecnt].pnts[3].z = ter->pnts[3].z;
                                        ScaleTerrain[scalecnt].pnts[3].y =
                                            (ter->pnts[3].y + situ->Location.y) * TerI->inyscale -
                                            situ->Location.y;
                                        mag = 1.0f / NuFsqrt(ter->norm[1].x * ter->norm[1].x +
                                                            ter->norm[1].y * ter->norm[1].y * TerI->yscalesq +
                                                            ter->norm[1].z * ter->norm[1].z);
                                        ScaleTerrain[scalecnt].norm[1].x = ter->norm[1].x * mag;
                                        ScaleTerrain[scalecnt].norm[1].y = ter->norm[1].y * TerI->yscale * mag;
                                        ScaleTerrain[scalecnt].norm[1].z = ter->norm[1].z * mag;
                                    } else {
                                        ScaleTerrain[scalecnt].norm[1].y = 65536.0f;
                                    }
                                    hitcnt++;
                                    mag = 1.0f / NuFsqrt(ter->norm[0].x * ter->norm[0].x +
                                                        ter->norm[0].y * ter->norm[0].y * TerI->yscalesq +
                                                        ter->norm[0].z * ter->norm[0].z);
                                    ScaleTerrain[scalecnt].norm[0].x = ter->norm[0].x * mag;
                                    ScaleTerrain[scalecnt].norm[0].y = ter->norm[0].y * TerI->yscale * mag;
                                    ScaleTerrain[scalecnt].norm[0].z = ter->norm[0].z * mag;
                                    *hitdata++ = &ScaleTerrain[scalecnt++];
                                }
                            }
                        }
                    }
                    group = (TerrainGroupType *)ter;
                } else {
                    group = (TerrainGroupType *)(ter + c);
                }
            } else {
                group = (TerrainGroupType *)(ter + c);
            }
        }
        if (hitcnt != 0) {
            lastwrite[0] = hitcnt;
            lastwrite[1] = msituid;
            lastwrite = (short *)hitdata;
        }
    }
    lastwrite[0] = 0;
    lastwrite[1] = 0;
}


/* PS2 PAL: byte-exact over the full 0x228-byte extent at 0x00181888. */
s32 NewRayCastPlatForm(struct nuvec_s *vpos, struct nuvec_s *vvel, f32 size, f32 timeadj,
                       s32 platid) {
    if (CurTerr == 0) {
        return 0;
    }
    plathitid = -1;
    TerrPolyObj = -1;
    TerrPoly = 0;
    TerI = (TerTempInfoType *)NuScratchAlloc32(sizeof(TerTempInfoType));
    TerI->inyscalesq = TerI->inyscale = TerI->yscalesq = TerI->yscale = 1.0f;
    TerI->size = size;
    TerI->sizediv = 1.0f / TerI->size;
    TerI->sizesq = TerI->size * TerI->size;
    TerI->origpos.x = TerI->curpos.x = vpos->x;
    TerI->origpos.y = TerI->curpos.y = vpos->y;
    TerI->origpos.z = TerI->curpos.z = vpos->z;
    TerI->origvel = TerI->curvel = *vvel;
    TerI->id = -1;
    TerI->flags = 0;
    TerI->scanmode = 1;
    TerI->timeadj = timeadj;
    TerI->impactadj = 0.0f;
    ScanTerrainPlatform(CurTerr->platdata[platid].terrno, 0);
    DerotateMovementVector();
    HitTerrain();
    if (TerI->hittype != 0) {
        RayImpact(vvel);
        TerrainImpactNorm();
        ShadNorm = TerI->hitnorm;
    }
    NuScratchRelease();
    return TerI->hittype;
}

#define TERRAIN_SCAN_SQR(x) ((x) * (x))

void ScanTerrain(s32 platscan, s32 extramask) {
    s32 a;
    s32 b;
    s32 c;
    s32 HitCnt;
    tertype** HitData;
    tertype** MaxData;
    short* LastWrite;
    short* ttemp;
    WallSplineType* WallSpl;
    float maxx;
    float maxy;
    float maxz;
    float tmaxx;
    float tmaxy;
    float tmaxz;
    float radmov;
    float minx;
    float miny;
    float minz;
    float tminx;
    float tminy;
    float tminz;
    float tn;
    TerrainSituType* mbuf2;
    tertype* ter;
    short* modp;
    s32 curscltemp;
    struct nuvec4_s pnts[4];
    struct nuvec4_s norm[2];

    curscltemp = 0;
    ScaleTerrain = ScaleTerrainT1;
    TerI->hitcnt = 0;
    LastWrite = (short*)TerI->hitdata;
    HitData = TerI->hitdata + 1;
    MaxData = HitData + 0x1fc;
    HitCnt = 0;
    platinrange = 0;
    TerI->plathit = -1;
    switch (TerI->scanmode) {
        default:
            radmov = NuFsqrt(TERRAIN_SCAN_SQR(TerI->curvel.x) + TERRAIN_SCAN_SQR(TerI->curvel.y) + TERRAIN_SCAN_SQR(TerI->curvel.z));
            radmov += TerI->size + 0.02f;
            maxx = TerI->curpos.x + radmov;
            maxy = TerI->curpos.y + (radmov * TerI->yscale);
            maxz = TerI->curpos.z + radmov;
            minx = TerI->curpos.x - radmov;
            miny = TerI->curpos.y - (radmov * TerI->yscale);
            minz = TerI->curpos.z - radmov;
            radmov *= radmov;
        break;
        case 1:
            if (TerI->curvel.x > 0.0f) {
                minx = (TerI->curpos.x - 0.02f) - TerI->size;
                maxx = TerI->curpos.x + TerI->curvel.x + 0.02f + TerI->size;
            }
            else {
                minx = (((TerI->curpos).x + (TerI->curvel).x) - 0.02f) - TerI->size;
                maxx = (TerI->curpos).x + 0.02f + TerI->size;
            }

            if ((TerI->curvel).y > 0.0f) {
                miny = ((TerI->curpos).y - 0.02f) - TerI->size;
                maxy = (TerI->curpos).y + (TerI->curvel).y + 0.02f + TerI->size;
            }
            else {
                miny = (((TerI->curpos).y + (TerI->curvel).y) - 0.02f) - TerI->size;
                maxy = (TerI->curpos).y + 0.02f + TerI->size;
            }

            if ((TerI->curvel).z > 0.0f) {
                minz = ((TerI->curpos).z - 0.02f) - TerI->size;
                maxz = (TerI->curpos).z + (TerI->curvel).z + 0.02f + TerI->size;
            }
            else {
                minz = (((TerI->curpos).z + (TerI->curvel).z) - 0.02f) - TerI->size;
                maxz = (TerI->curpos).z + 0.02f + TerI->size;
            }

            radmov = TERRAIN_SCAN_SQR(minx - maxx) + TERRAIN_SCAN_SQR(miny - maxy) + TERRAIN_SCAN_SQR(minz - maxz);
            radmov += 0.02f;
        break;
    }

    for (a = 0; a < CurTerr->terrgroupcnt; a++) {
        if (
            (maxx >= CurTerr->terrgroup[a].bounds[0])
            && (maxz >= CurTerr->terrgroup[a].bounds[1])
            && (minx <= CurTerr->terrgroup[a].bounds[2])
            && (minz <= CurTerr->terrgroup[a].bounds[3])
        ) {
            ttemp = CurTerr->terrlist + CurTerr->terrgroup[a].start;
            for (b = 0; b < CurTerr->terrgroup[a].count; b++, ttemp++) {
                mbuf2 = CurTerr->terr + *ttemp;
                if (
                    (maxx >= mbuf2->min.x)
                    && (maxy >= mbuf2->min.y)
                    && (maxz >= mbuf2->min.z)
                    && (minx <= mbuf2->max.x)
                    && (miny < mbuf2->max.y)
                    && (minz < mbuf2->max.z)
                    && (mbuf2->type != -1)
                ) {
                    tmaxx = maxx - mbuf2->Location.x;
                    tmaxy = maxy - mbuf2->Location.y;
                    tmaxz = maxz - mbuf2->Location.z;

                    tminx = minx - mbuf2->Location.x;
                    tminy = miny - mbuf2->Location.y;
                    tminz = minz - mbuf2->Location.z;

                    modp = (short*)mbuf2->model;
                    while (*modp >= 0) {
                        c = modp[1];
                        ter = (tertype *)(modp + 10);
                        if (
                            (tmaxx >= *(float*)&modp[2])
                            && (tminx < *(float*)&modp[4])
                            ){
                                if (
                                    (tmaxz >= *(float*)&modp[6])
                                    && (tminz < *(float*)&modp[8])
                                ) {
                                    for (; c > 0; c--) {
                                        if (
                                            (tmaxx >= ter->minx)
                                            && (tminx < ter->maxx)
                                            && (tmaxz >= ter->minz)
                                            && (tminz < ter->maxz)
                                            && (tmaxy >= ter->miny)
                                            && (tminy < ter->maxy)
                                            && (HitData < MaxData)
                                            && ((ter->info[1] == 0) || ((ter->info[1] & extramask) != 0))
                                        ) {
                                            if (TerI->yscale == 1.0f) {
                                                *HitData = ter;
                                                HitData++;
                                                HitCnt++;
                                            }
                                            else {
                                                ScaleTerrain[curscltemp].info[0] = ter->info[0];
                                                ScaleTerrain[curscltemp].info[1] = ter->info[1];
                                                ScaleTerrain[curscltemp].info[2] = ter->info[2];
                                                ScaleTerrain[curscltemp].info[3] = ter->info[3];
                                                ScaleTerrain[curscltemp].pnts[0].x = ter->pnts[0].x;
                                                ScaleTerrain[curscltemp].pnts[0].z = ter->pnts[0].z;
                                                ScaleTerrain[curscltemp].pnts[0].y =
                                                 (ter->pnts[0].y + mbuf2->Location.y) * TerI->inyscale - mbuf2->Location.y;
                                                ScaleTerrain[curscltemp].pnts[1].x = ter->pnts[1].x;
                                                ScaleTerrain[curscltemp].pnts[1].z = ter->pnts[1].z;
                                                ScaleTerrain[curscltemp].pnts[1].y =
                                                 (ter->pnts[1].y + mbuf2->Location.y) * TerI->inyscale - mbuf2->Location.y;
                                                ScaleTerrain[curscltemp].pnts[2].x = ter->pnts[2].x;
                                                ScaleTerrain[curscltemp].pnts[2].z = ter->pnts[2].z;
                                                ScaleTerrain[curscltemp].pnts[2].y =
                                                 (ter->pnts[2].y + mbuf2->Location.y) * TerI->inyscale - mbuf2->Location.y;
                                                if (65535.0f > ter->norm[1].y) {
                                                    ScaleTerrain[curscltemp].pnts[3].x = ter->pnts[3].x;
                                                    ScaleTerrain[curscltemp].pnts[3].z = ter->pnts[3].z;
                                                    ScaleTerrain[curscltemp].pnts[3].y =
                                                       (ter->pnts[3].y + mbuf2->Location.y) * TerI->inyscale - mbuf2->Location.y;
                                                    tn =  1.0f / NuFsqrt(ter->norm[1].x * ter->norm[1].x +
                                                                   ter->norm[1].y * ter->norm[1].y * TerI->yscalesq +
                                                                   ter->norm[1].z * ter->norm[1].z);
                                                    ScaleTerrain[curscltemp].norm[1].x = ter->norm[1].x * tn;
                                                    ScaleTerrain[curscltemp].norm[1].y = ter->norm[1].y * TerI->yscale * tn;
                                                    ScaleTerrain[curscltemp].norm[1].z = ter->norm[1].z * tn;
                                                }
                                                else {
                                                    ScaleTerrain[curscltemp].norm[1].y = 65536.0f;
                                                }
                                                tn = 1.0f / NuFsqrt(ter->norm[0].x * ter->norm[0].x +
                                                             ter->norm[0].y * ter->norm[0].y * TerI->yscalesq +
                                                             ter->norm[0].z * ter->norm[0].z);

                                                ScaleTerrain[curscltemp].norm[0].x = ter->norm[0].x * tn;
                                                ScaleTerrain[curscltemp].norm[0].y = ter->norm[0].y * TerI->yscale * tn;
                                                ScaleTerrain[curscltemp].norm[0].z = ter->norm[0].z * tn;
                                                *HitData = ScaleTerrain + curscltemp;
                                                HitData++;
                                                HitCnt++;
                                                curscltemp++;
                                            }
                                        }
                                        ter++;
                                    }
                                modp = (short*)ter;
                            } else {
                                modp = (short*)(ter + c);
                            }
                        } else {
                            modp = (short*)(ter + c);
                        }
                    }
                    if (HitCnt != 0) {
                        LastWrite[0] = HitCnt;
                        LastWrite[1] = *ttemp;
                        LastWrite = (short*)HitData;
                        HitData++;
                        HitCnt = 0;
                    }
                }
            }
        }
    }

    if (curPickInst != 0) {
        for (a = 0; a < curPickInst; a++) {
            mbuf2 = &CurTerr->terr[a+2048];

            tmaxx = maxx - mbuf2->Location.x;
            tmaxy = maxy - mbuf2->Location.y;
            tmaxz = maxz - mbuf2->Location.z;

            tminx = minx - mbuf2->Location.x;
            tminy = miny - mbuf2->Location.y;
            tminz = minz - mbuf2->Location.z;

            if (
                (tmaxx >= mbuf2->min.x)
                && (tmaxy >= mbuf2->min.y)
                && (tmaxz >= mbuf2->min.z)
                && (tminx <= mbuf2->max.x)
                && (tminy < mbuf2->max.y)
                && (tminz < mbuf2->max.z)
                && (mbuf2->type != -1)
            ) {
                modp = (short*)mbuf2->model;
                while (*modp >= 0) {
                    c = modp[1];
                    ter = (tertype *)(modp + 10);
                    if (
                        (tmaxx >= *(float*)&modp[2])
                        && (tminx < *(float*)&modp[4])
                    ) {
                        if (
                            (tmaxz >= *(float*)&modp[6])
                            && (tminz < *(float*)&modp[8])
                        ) {
                            for (; c > 0; c--) {
                                if (
                                    (tmaxx >= ter->minx)
                                    && (tminx < ter->maxx)
                                    && (tmaxz >= ter->minz)
                                    && (tminz < ter->maxz)
                                    && (tmaxy >= ter->miny)
                                    && (tminy < ter->maxy)
                                    && (HitData < MaxData)
                                    && ((ter->info[1] == 0) || ((ter->info[1] & extramask) != 0))
                                ) {
                                    if (TerI->yscale == 1.0f) {
                                        *HitData = ter;
                                        HitData++;
                                        HitCnt++;
                                    }
                                    else {
                                        ScaleTerrain[curscltemp].info[0] = ter->info[0];
                                        ScaleTerrain[curscltemp].info[1] = ter->info[1];
                                        ScaleTerrain[curscltemp].info[2] = ter->info[2];
                                        ScaleTerrain[curscltemp].info[3] = ter->info[3];
                                        ScaleTerrain[curscltemp].pnts[0].x = ter->pnts[0].x;
                                        ScaleTerrain[curscltemp].pnts[0].z = ter->pnts[0].z;
                                        ScaleTerrain[curscltemp].pnts[0].y =
                                         (ter->pnts[0].y + mbuf2->Location.y) * TerI->inyscale - mbuf2->Location.y;
                                        ScaleTerrain[curscltemp].pnts[1].x = ter->pnts[1].x;
                                        ScaleTerrain[curscltemp].pnts[1].z = ter->pnts[1].z;
                                        ScaleTerrain[curscltemp].pnts[1].y =
                                         (ter->pnts[1].y + mbuf2->Location.y) * TerI->inyscale - mbuf2->Location.y;
                                        ScaleTerrain[curscltemp].pnts[2].x = ter->pnts[2].x;
                                        ScaleTerrain[curscltemp].pnts[2].z = ter->pnts[2].z;
                                        ScaleTerrain[curscltemp].pnts[2].y =
                                         (ter->pnts[2].y + mbuf2->Location.y) * TerI->inyscale - mbuf2->Location.y;
                                        if (65535.0f > ter->norm[1].y) {
                                            ScaleTerrain[curscltemp].pnts[3].x = ter->pnts[3].x;
                                            ScaleTerrain[curscltemp].pnts[3].z = ter->pnts[3].z;
                                            ScaleTerrain[curscltemp].pnts[3].y =
                                               (ter->pnts[3].y + mbuf2->Location.y) * TerI->inyscale - mbuf2->Location.y;
                                            tn =  1.0f / NuFsqrt(TERRAIN_SCAN_SQR(ter->norm[1].x) + TERRAIN_SCAN_SQR(ter->norm[1].y) * TerI->yscalesq + TERRAIN_SCAN_SQR(ter->norm[1].z));
                                            ScaleTerrain[curscltemp].norm[1].x = ter->norm[1].x * tn;
                                            ScaleTerrain[curscltemp].norm[1].y = ter->norm[1].y * TerI->yscale * tn;
                                            ScaleTerrain[curscltemp].norm[1].z = ter->norm[1].z * tn;
                                        }
                                        else {
                                            ScaleTerrain[curscltemp].norm[1].y = 65536.0f;
                                        }
                                        tn = 1.0f / NuFsqrt(TERRAIN_SCAN_SQR(ter->norm[0].x) + TERRAIN_SCAN_SQR(ter->norm[0].y) * TerI->yscalesq + TERRAIN_SCAN_SQR(ter->norm[0].z));

                                        ScaleTerrain[curscltemp].norm[0].x = ter->norm[0].x * tn;
                                        ScaleTerrain[curscltemp].norm[0].y = ter->norm[0].y * TerI->yscale * tn;
                                        ScaleTerrain[curscltemp].norm[0].z = ter->norm[0].z * tn;
                                        *HitData = ScaleTerrain + curscltemp;
                                        HitData++;
                                        HitCnt++;
                                        curscltemp++;
                                    }
                                }
                                ter++;
                            }
                            modp = (short*)ter;
                        }
                        else {
                            modp = (short*)(ter + c);
                        }
                    }
                    else {
                        modp = (short*)(ter + c);
                    }
                }

                if (HitCnt != 0) {
                    LastWrite[0] = HitCnt;
                    LastWrite[1] = a + 0x800;
                    LastWrite = (short*)HitData;
                    HitData++;
                    HitCnt = 0;
                }
            }
        }
    }

    TerI->PlatScanStart = LastWrite;
    a = 0x100;
    minx -= 0.05f;
    maxx += 0.05f;
    miny -= 0.05f;
    maxy += 0.05f;
    minz -= 0.05f;
    maxz += 0.05f;
    if (platscan != 0) {
        ttemp = CurTerr->terrlist + CurTerr->terrgroup[a].start;
        for (b = 0; b < CurTerr->terrgroup[a].count; b++, ttemp++) {
            mbuf2 = CurTerr->terr + *ttemp;
            if (CurTerr->platdata[mbuf2->info].curmtx != 0) {
                if (CurTerr->platdata[mbuf2->info].curmtx->_30 > CurTerr->platdata[mbuf2->info].oldmtx._30) {
                    tminx = minx - mbuf2->Location.x;
                    tmaxx = (maxx + (CurTerr->platdata[mbuf2->info].curmtx->_30 -
                    CurTerr->platdata[mbuf2->info].oldmtx._30) * 1.5f) - mbuf2->Location.x;
                }
                else {
                    tmaxx = maxx - mbuf2->Location.x;
                    tminx = (minx + (CurTerr->platdata[mbuf2->info].curmtx->_30 -
                    CurTerr->platdata[mbuf2->info].oldmtx._30) * 1.5f) - mbuf2->Location.x;
                }
                if (CurTerr->platdata[mbuf2->info].curmtx->_31 > CurTerr->platdata[mbuf2->info].oldmtx._31) {
                    tminy = miny - mbuf2->Location.y;
                    tmaxy = (maxy + (CurTerr->platdata[mbuf2->info].curmtx->_31 -
                    CurTerr->platdata[mbuf2->info].oldmtx._31) * 1.5f) - mbuf2->Location.y;
                }
                else {
                    tmaxy = maxy - mbuf2->Location.y;
                    tminy = (miny + (CurTerr->platdata[mbuf2->info].curmtx->_31 -
                    CurTerr->platdata[mbuf2->info].oldmtx._31) * 1.5f) - mbuf2->Location.y;
                }
                if (CurTerr->platdata[mbuf2->info].curmtx->_32 > CurTerr->platdata[mbuf2->info].oldmtx._32) {
                    tminz = minz - mbuf2->Location.z;
                    tmaxz = (maxz + (CurTerr->platdata[mbuf2->info].curmtx->_32 -
                    CurTerr->platdata[mbuf2->info].oldmtx._32) * 1.5f) - mbuf2->Location.z;
                }
                else {
                    tmaxz = maxz - mbuf2->Location.z;
                    tminz = (minz + (CurTerr->platdata[mbuf2->info].curmtx->_32 -
                    CurTerr->platdata[mbuf2->info].oldmtx._32) * 1.5f) - mbuf2->Location.z;
                }
            }
            else {
                tmaxx = maxx - mbuf2->Location.x;
                tminx = minx - mbuf2->Location.x;
                tmaxy = maxy - mbuf2->Location.y;
                tminy = miny - mbuf2->Location.y;
                tmaxz = maxz - mbuf2->Location.z;
                tminz = minz - mbuf2->Location.z;
            }

            if ((s32)CurTerr->platdata[mbuf2->info].status.bit.rotate != 0) {
                tn = TERRAIN_SCAN_SQR((maxx + minx) * 0.5f - mbuf2->Location.x) +
                    TERRAIN_SCAN_SQR(((maxy + miny) * 0.5f - mbuf2->Location.y) * TerI->inyscale)  +
                    TERRAIN_SCAN_SQR((maxz + minz) * 0.5f - mbuf2->Location.z);
                if ((mbuf2->type != -1) && (tn < mbuf2->radius + radmov)) {
                    modp = (short *)mbuf2->model;
                    while (*modp >= 0) {
                        c = modp[1];
                        ter = (tertype *)(modp + 10);
                        for (; c > 0; c--) {
                            platinrange = 1;
                            if (
                                (HitData < MaxData)
                                && ((ter->info[1] == 0) || ((ter->info[1] & extramask) != 0))
                            ) {
                                pnts[0].x = ter->pnts[0].x;
                                pnts[0].y = ter->pnts[0].y;
                                pnts[0].z = ter->pnts[0].z;
                                pnts[0].w = 0.0f;
                                pnts[1].x = ter->pnts[1].x;
                                pnts[1].y = ter->pnts[1].y;
                                pnts[1].z = ter->pnts[1].z;
                                pnts[1].w = 0.0f;
                                pnts[2].x = ter->pnts[2].x;
                                pnts[2].y = ter->pnts[2].y;
                                pnts[2].z = ter->pnts[2].z;
                                pnts[2].w = 0.0f;
                                NuVec4MtxTransformVU0(&pnts[0], &pnts[0], CurTerr->platdata[mbuf2->info].curmtx);
                                NuVec4MtxTransformVU0(&pnts[1], &pnts[1], CurTerr->platdata[mbuf2->info].curmtx);
                                NuVec4MtxTransformVU0(&pnts[2], &pnts[2], CurTerr->platdata[mbuf2->info].curmtx);
                                norm[0].x = ter->norm[0].x;
                                norm[0].y = ter->norm[0].y;
                                norm[0].z = ter->norm[0].z;
                                norm[0].w = 0.0f;
                                NuVec4MtxTransformVU0(&norm[0], &norm[0], CurTerr->platdata[mbuf2->info].curmtx);

                                if (65535.0f > ter->norm[1].y) {
                                    pnts[3].x = ter->pnts[3].x;
                                    pnts[3].y = ter->pnts[3].y;
                                    pnts[3].z = ter->pnts[3].z;
                                    pnts[3].w = 0.0f;
                                    NuVec4MtxTransformVU0(&pnts[3], &pnts[3], CurTerr->platdata[mbuf2->info].curmtx);
                                    norm[1].x = ter->norm[1].x;
                                    norm[1].y = ter->norm[1].y;
                                    norm[1].z = ter->norm[1].z;
                                    norm[1].w = 0.0f;
                                    NuVec4MtxTransformVU0(&norm[1], &norm[1], CurTerr->platdata[mbuf2->info].curmtx);
                                }
                                else {
                                    pnts[3] = pnts[2];
                                }

                                if (
                                    (
                                        (pnts[0].x > tminx)
                                        || (pnts[1].x > tminx)
                                        || (pnts[2].x > tminx)
                                        || (pnts[3].x > tminx)
                                    )
                                    &&
                                    (
                                        (pnts[0].x < tmaxx)
                                        || (pnts[1].x < tmaxx)
                                        || (pnts[2].x < tmaxx)
                                        || (pnts[3].x < tmaxx)
                                    )
                                    &&
                                    (
                                        (pnts[0].z > tminz)
                                        || (pnts[1].z > tminz)
                                        || (pnts[2].z > tminz)
                                        || (pnts[3].z > tminz)
                                    )
                                    &&
                                    (
                                        (pnts[0].z < tmaxz)
                                        || (pnts[1].z < tmaxz)
                                        || (pnts[2].z < tmaxz)
                                        || (pnts[3].z < tmaxz)
                                    )
                                ) {
                                    ScaleTerrain[curscltemp].info[0] = ter->info[0];
                                    ScaleTerrain[curscltemp].info[1] = ter->info[1];
                                    ScaleTerrain[curscltemp].info[2] = ter->info[2];
                                    ScaleTerrain[curscltemp].info[3] = ter->info[3];
                                    ScaleTerrain[curscltemp].pnts[0].x = pnts[0].x;
                                    ScaleTerrain[curscltemp].pnts[0].z = pnts[0].z;
                                    ScaleTerrain[curscltemp].pnts[0].y =
                                     (pnts[0].y + mbuf2->Location.y) * TerI->inyscale - mbuf2->Location.y;
                                    ScaleTerrain[curscltemp].pnts[1].x = pnts[1].x;
                                    ScaleTerrain[curscltemp].pnts[1].z = pnts[1].z;
                                    ScaleTerrain[curscltemp].pnts[1].y =
                                     (pnts[1].y + mbuf2->Location.y) * TerI->inyscale - mbuf2->Location.y;
                                    ScaleTerrain[curscltemp].pnts[2].x = pnts[2].x;
                                    ScaleTerrain[curscltemp].pnts[2].z = pnts[2].z;
                                    ScaleTerrain[curscltemp].pnts[2].y =
                                     (pnts[2].y + mbuf2->Location.y) * TerI->inyscale - mbuf2->Location.y;
                                    if (65535.0f > ter->norm[1].y) {
                                        ScaleTerrain[curscltemp].pnts[3].x = pnts[3].x;
                                        ScaleTerrain[curscltemp].pnts[3].z = pnts[3].z;
                                        ScaleTerrain[curscltemp].pnts[3].y =
                                           (pnts[3].y + mbuf2->Location.y) * TerI->inyscale - mbuf2->Location.y;
                                        tn =  1.0f / NuFsqrt(TERRAIN_SCAN_SQR(norm[1].x) + TERRAIN_SCAN_SQR(norm[1].y) * TerI->yscalesq + TERRAIN_SCAN_SQR(norm[1].z));
                                        ScaleTerrain[curscltemp].norm[1].x = norm[1].x * tn;
                                        ScaleTerrain[curscltemp].norm[1].y = norm[1].y * TerI->yscale * tn;
                                        ScaleTerrain[curscltemp].norm[1].z = norm[1].z * tn;
                                    }
                                    else {
                                        ScaleTerrain[curscltemp].norm[1].y = 65536.0f;
                                    }

                                    tn =  1.0f / NuFsqrt(TERRAIN_SCAN_SQR(norm[0].x) + TERRAIN_SCAN_SQR(norm[0].y) * TerI->yscalesq + TERRAIN_SCAN_SQR(norm[0].z));

                                    ScaleTerrain[curscltemp].norm[0].x = norm[0].x * tn;
                                    ScaleTerrain[curscltemp].norm[0].y = norm[0].y * TerI->yscale * tn;
                                    ScaleTerrain[curscltemp].norm[0].z = norm[0].z * tn;
                                    *HitData = ScaleTerrain + curscltemp;
                                    HitData++;
                                    HitCnt++;
                                    curscltemp++;
                                }
                            }
                            ter++;
                        }
                        modp = (short *)ter;
                    }

                    if (HitCnt != 0) {
                        LastWrite[0] = HitCnt;
                        LastWrite[1] = *ttemp;
                        LastWrite = (short *)HitData;
                        HitData++;
                        HitCnt = 0;
                    }
                }
            }
            else if (
                (tmaxx >= mbuf2->min.x)
                && (tmaxy >= mbuf2->min.y)
                && (tmaxz >= mbuf2->min.z)
                && (tminx <= mbuf2->max.x)
                && (tminy < mbuf2->max.y)
                && (tminz < mbuf2->max.z)
                && (mbuf2->type != -1)
            ) {
                modp = (short *)mbuf2->model;
                while (*modp >= 0) {
                    c = modp[1];
                    ter = (tertype *)(modp + 10);
                    if (
                        (tmaxx >= *(float*)&modp[2])
                        && (tminx < *(float*)&modp[4])
                    ) {
                        if (
                            (tmaxz >= *(float*)&modp[6])
                            && (tminz < *(float*)&modp[8])
                        ) {
                            for (; c > 0; c--) {
                                if (
                                    (tmaxx >= ter->minx)
                                    && (tminx < ter->maxx)
                                    && (tmaxz >= ter->minz)
                                    && (tminz < ter->maxz)
                                    && (tmaxy >= ter->miny)
                                    && (tminy < ter->maxy)
                                    && (platinrange = 1,HitData < MaxData)
                                    && ((ter->info[1] == 0) || ((ter->info[1] & extramask) != 0))
                                ) {
                                    if (TerI->yscale == 1.0f) {
                                        *HitData = ter;
                                        HitData++;
                                        HitCnt++;
                                    }
                                    else {
                                        ScaleTerrain[curscltemp].info[0] = ter->info[0];
                                        ScaleTerrain[curscltemp].info[1] = ter->info[1];
                                        ScaleTerrain[curscltemp].info[2] = ter->info[2];
                                        ScaleTerrain[curscltemp].info[3] = ter->info[3];
                                        ScaleTerrain[curscltemp].pnts[0].x = ter->pnts[0].x;
                                        ScaleTerrain[curscltemp].pnts[0].z = ter->pnts[0].z;
                                        ScaleTerrain[curscltemp].pnts[0].y =
                                         (ter->pnts[0].y + mbuf2->Location.y) * TerI->inyscale - mbuf2->Location.y;
                                        ScaleTerrain[curscltemp].pnts[1].x = ter->pnts[1].x;
                                        ScaleTerrain[curscltemp].pnts[1].z = ter->pnts[1].z;
                                        ScaleTerrain[curscltemp].pnts[1].y =
                                         (ter->pnts[1].y + mbuf2->Location.y) * TerI->inyscale - mbuf2->Location.y;
                                        ScaleTerrain[curscltemp].pnts[2].x = ter->pnts[2].x;
                                        ScaleTerrain[curscltemp].pnts[2].z = ter->pnts[2].z;
                                        ScaleTerrain[curscltemp].pnts[2].y =
                                         (ter->pnts[2].y + mbuf2->Location.y) * TerI->inyscale - mbuf2->Location.y;
                                        if (65535.0f > ter->norm[1].y) {
                                            ScaleTerrain[curscltemp].pnts[3].x = ter->pnts[3].x;
                                            ScaleTerrain[curscltemp].pnts[3].z = ter->pnts[3].z;
                                            ScaleTerrain[curscltemp].pnts[3].y =
                                               (ter->pnts[3].y + mbuf2->Location.y) * TerI->inyscale - mbuf2->Location.y;
                                            tn =  1.0f / NuFsqrt(TERRAIN_SCAN_SQR(ter->norm[1].x) + TERRAIN_SCAN_SQR(ter->norm[1].y) * TerI->yscalesq + TERRAIN_SCAN_SQR(ter->norm[1].z));
                                            ScaleTerrain[curscltemp].norm[1].x = ter->norm[1].x * tn;
                                            ScaleTerrain[curscltemp].norm[1].y = ter->norm[1].y * TerI->yscale * tn;
                                            ScaleTerrain[curscltemp].norm[1].z = ter->norm[1].z * tn;
                                        }
                                        else {
                                            ScaleTerrain[curscltemp].norm[1].y = 65536.0f;
                                        }
                                        tn = 1.0f / NuFsqrt(TERRAIN_SCAN_SQR(ter->norm[0].x) + TERRAIN_SCAN_SQR(ter->norm[0].y) * TerI->yscalesq + TERRAIN_SCAN_SQR(ter->norm[0].z));

                                        ScaleTerrain[curscltemp].norm[0].x = ter->norm[0].x * tn;
                                        ScaleTerrain[curscltemp].norm[0].y = ter->norm[0].y * TerI->yscale * tn;
                                        ScaleTerrain[curscltemp].norm[0].z = ter->norm[0].z * tn;
                                        *HitData = ScaleTerrain + curscltemp;
                                        HitData++;
                                        HitCnt++;
                                        curscltemp++;
                                    }
                                }
                                ter++;
                            }
                            modp = (short *)ter;
                        }
                        else {
                            modp = (short *)(ter + c);
                        }
                    }
                    else {
                        modp = (short *)(ter + c);
                    }
                }

                if (HitCnt != 0) {
                    LastWrite[0] = HitCnt;
                    LastWrite[1] = *ttemp;
                    LastWrite = (short *)HitData;
                    HitData++;
                    HitCnt = 0;
                }
            }
        }
    }

    LastWrite[0] = 0;
    LastWrite[1] = 0;
    minx -= 0.02;
    minz -= 0.02;
    maxx += 0.02;
    maxz += 0.02;
    WallSplCount = 0;

    for (WallSpl = (WallSplineType *)CurTerr->wallinfo; WallSpl != 0; WallSpl = ((WallSplineType **)WallSpl)[-1]) {
        for (a = 0; a < WallSpl->count; a += 0x10) {
            if (WallSpl->spl[a].y != 2147483648.0f) {
                if (
                    (WallSpl->spl[a + 1].y >= minx)
                    && (WallSpl->spl[a].y <= maxx)
                    && (WallSpl->spl[a + 3].y >= minz)
                    && (WallSpl->spl[a + 2].y <= maxz)
                ) {
                    c = a + 0x10;
                    if (c > WallSpl->count) {
                        c = WallSpl->count;
                    }
                }
                else {
                    c = 0;
                }
            } else {
                c = WallSpl->count;
            }

            for (b = a; b < c; b++) {
                if (
                    (
                        ((WallSpl->spl[b].x >= minx) && (WallSpl->spl[b + 1].x <= maxx))
                        || ((WallSpl->spl[b + 1].x >= minx) && (WallSpl->spl[b].x <= maxx))
                    )
                    &&
                    (
                        ((WallSpl->spl[b].z >= minz) && (WallSpl->spl[b + 1].z <= maxz))
                        || ((WallSpl->spl[b + 1].z >= minz) && (WallSpl->spl[b].z <= maxz))
                    )
                    && (WallSplCount < 0x40)
                ) {
                    WallSplList[WallSplCount] = WallSpl->spl[b];
                    WallSplCount++;
                    WallSplList[WallSplCount] = WallSpl->spl[b + 1];
                    WallSplCount++;
                }
            }
        }
    }
}

#undef TERRAIN_SCAN_SQR
