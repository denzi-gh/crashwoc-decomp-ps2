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

#include "creature.h"

/* Terrain collision info structures (offsets verified against retail asm). */
typedef struct {
    float minx;
    float maxx;
    float miny;
    float maxy;
    float minz;
    float maxz;
    struct nuvec_s pnts[4];
    struct nuvec_s norm[2];
    unsigned char info[4];
} tertype; /* 0x64 */

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
    void *ptrid;              /* 0x00 */
    s16 platid;               /* 0x04 */
    s16 platinf;              /* 0x06 */
    s16 timer;                /* 0x08 */
    s16 pad;                  /* 0x0A */
} TerrainTrackInfoType;

typedef struct {
    struct nuvec_s Location;  /* 0x00 */
    s16 *model;               /* 0x0C */
    struct nuvec_s min;       /* 0x10 */
    struct nuvec_s max;       /* 0x1C */
    u32 flags;                /* 0x28 */
    s32 type;                 /* 0x2C */
    s16 info;                 /* 0x30 */
    s16 id;                   /* 0x32 */
    f32 radius;               /* 0x34 */
} TerrainSituType; /* 0x38 */

typedef union {
    u32 all;
    struct {
        u32 rotate : 1;
        u32 hit : 1;
    } bit;
} PlatStatusType;

typedef struct {
    u8 oldmtx[0x40];          /* 0x00 */
    void *curmtx;             /* 0x40 */
    s16 terrno;               /* 0x44 */
    s16 instance;             /* 0x46 */
    PlatStatusType status;    /* 0x48 */
    s16 hitcnt;               /* 0x4C */
    s16 pad;                  /* 0x4E */
    f32 plrgrav;              /* 0x50 */
    f32 ypos;                 /* 0x54 */
    f32 yvel;                 /* 0x58 */
    f32 tension;              /* 0x5C */
    f32 damp;                 /* 0x60 */
} TerrainPlatformType; /* 0x64 */

typedef struct {
    TerrainSituType *terr;                /* 0x0000 */
    TerrainPlatformType platdata[128];    /* 0x0004 */
    void *wallinfo;                       /* 0x3204 */
    TerrainTrackInfoType TrackInfo[4];    /* 0x3208 */
} TerrainSetType;

typedef struct {
    struct nuvec_s pos;                   /* 0x00 */
    f32 radius;                           /* 0x0C */
} SphereType;

extern TerTempInfoType *TerI;
extern TerrShapeType *TerrShape;
extern f32 NuTrigTable[];
extern f32 D_0062CDA4; /* 0.2f (gp-relative .sdata constant) */
extern f32 D_0062CDA8;
extern f32 D_0062CDAC;
extern f32 D_0062CDB0;
extern f32 D_0062CDB4;
extern f32 D_0062CDBC;
extern f32 D_0062CDC0;
extern f32 D_0062CDC4;
extern f32 D_0062CCC4;
extern f32 D_0062CCC8;
extern f32 D_0062CCE0;
extern f32 D_0062CCE4;
extern f32 D_0062CD4C;
extern f32 D_0062CD50;
extern s32 TerrShapeAdjCnt;
extern s32 terraincnt;
extern TerrainSetType *CurTerr;
extern TerrainTrackInfoType *CurTrackInfo;
extern tertype *TerrPoly;
extern tertype TerrPolyInfo;
extern s32 TerrPolyObj;
extern s32 plathitid;
extern s32 PlatCrush;
extern s32 terrhitflags;
extern s32 platinrange;
extern struct nuvec_s ShadNorm;
extern struct nuvec_s EShadNorm;
extern struct nuvec_s ShadRoofNorm;
extern struct nuvec_s EShadRoofNorm;
extern f32 EShadY;
extern f32 ShadRoofY;
extern f32 EShadRoofY;
extern tertype *EShadPoly;
extern tertype *ShadRoofPoly;
extern tertype *EShadRoofPoly;
extern short castnum;
extern short ecastnum;
extern short castroofnum;
extern short ecastroofnum;
extern short shadhit;
extern short eshadhit;
extern short shadroofhit;
extern short eshadroofhit;
extern s32 testlock;
extern s32 curSphereter;
extern s32 curPickInst;
extern SphereType SphereData[];
extern void *crashdata;
extern tertype *ShadPoly;
extern s32 TempScanStack[];
extern void *TempStackPtr;
extern s32 TerrPlatDis;

extern s32 NuAtan2D(f32 x, f32 z);
extern f32 NuFsqrt(f32 x);
extern s32 PadRecPtr(void);
extern void *NuScratchAlloc32(s32 size);
extern void NuScratchRelease(void);
extern void ScanTerrain(s32 a0, s32 a1);
extern u64 fptodp(f32 value);
extern s32 dpcmp(u64 a, u64 b);
extern u64 dpsub(u64 a, u64 b);
extern s32 PlatformChecks(s32 itterationcnt, struct nuvec_s *vvel);
extern s32 HitTerrain(void);
extern void StorePlatImpact(void);
extern s32 TerrainPlatformEmbedded(struct nuvec_s *vvel);
extern void TerrainImpactNorm(void);
extern void TerrainImpact(struct nuvec_s *vpos, struct nuvec_s *vvel, u8 *flags);
extern short InsidePolLines(f32 x, f32 y, f32 z,
                            f32 dx0, f32 dy0, f32 dz0,
                            f32 dx1, f32 dy1, f32 dz1,
                            struct nuvec_s *norm);
extern s32 CheckCylinder(s32 p0, s32 p1, s32 *flags, s32 type);
extern s32 HitWallSpline(void);

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

//NGC MATCH
void DerotateMovementVector(void) {
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
    if (dotq > TerrShape->size * D_0062CDA4) {
        dotq = TerrShape->size * D_0062CDA4;
    }
    if (dotq < -TerrShape->size * D_0062CDA4) {
        dotq = -TerrShape->size * D_0062CDA4;
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
        tx = pos->x - CurTerr->terr[*(short *)((s32)CurData + 2)].Location.x;
        tz = pos->z - CurTerr->terr[*(short *)((s32)CurData + 2)].Location.z;
        objnum = *(short *)((s32)CurData + 2);
        curter = (tertype **)(CurData + 2);
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
                time = D_0062CCC4;
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
                time = D_0062CCC8;
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
                TerI->hittime = time;
                if (ps2 > 0.0f) {
                    TerI->hittype = 1;
                } else {
                    TerI->hittype = 0x11;
                }
                TerI->hitnorm.x = ter->norm[1].x;
                TerI->hitnorm.y = ter->norm[1].y;
                TerI->hitnorm.z = ter->norm[1].z;
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
    TerI->hittime = D_0062CCE0;
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
        do {
            CurData += ((*CurData >= 0) ? *CurData : -(*CurData)) * 2 + 2;
        } while (*CurData < 0);
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
    TerI->hittime = D_0062CCE4;
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
            if (TerI->uhitnorm.y < D_0062CD4C) {
                flags[0] = 0;
            } else {
                flags[1] = 1;
                flags[0] = 1;
                TerI->curpos.y += TerI->hitnorm.y * D_0062CD50;
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

void TerrainSetCur(void *curterr) {
    CurTerr = curterr;
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

void UpdatePlatinst(s32 id, void *mtx) {
    if ((id >= 0) && (id < 128)) {
        CurTerr->platdata[id].curmtx = mtx;
    }
}

void PlatInstRotate(s32 id, s32 rot) {
    CurTerr->platdata[id].status.bit.rotate = rot;
}

s32 PlatInstGetHit(s32 id) {
    if ((id >= 0) && (id < 128)) {
        return CurTerr->platdata[id].status.bit.hit;
    }
    return 0;
}

void TerrainPlatGetMtx(s32 id, u8 **oldmtx, void **curmtx) {
    if (id >= 0) {
        *oldmtx = CurTerr->platdata[id].oldmtx;
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

void NewRaySetDisablePalt(s32 disable) {
    TerrPlatDis = disable;
}

void TerrFlush(void) {
    curSphereter = 0;
    TerrShapeAdjCnt = 0;
    curPickInst = 0;
}

TerrainTrackInfoType *ScanTerrId(void *id) {
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

void OffPlat(void) {
}

/* PS2 switch structure reconstructed from 0x00182b00. */
void TerrainMoveImpactData(void) {
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
            if ((TerI->uhitnorm.y > D_0062CDA8) || (TerI->uhitnorm.y < D_0062CDAC)) {
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
        TerI->curvel.x = TerI->hitnorm.x * TerrShape->size * D_0062CDB0;
        TerI->curvel.z = TerI->hitnorm.z * TerrShape->size * D_0062CDB0;
        TerrShape->offset.x = TerrShape->offset.x + TerI->hitnorm.x * TerrShape->size * D_0062CDB4;
        TerrShape->offset.z = TerrShape->offset.z + TerI->hitnorm.z * TerrShape->size * D_0062CDB4;
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

// NGC MATCH (PS2 control flow and layout verified against 0x0017cec8)
void NewTerrainScaleY(struct nuvec_s *vpos, struct nuvec_s *vvel, u8 *flags,
                      s32 terid, f32 stopflag, f32 size, f32 yscale) {
    s32 c;
    s32 cnt;
    s32 normhit;
    u64 d;
    u64 zero;

    if (CurTerr == 0) {
        return;
    }

    TerrPolyObj = -1;
    plathitid = -1;
    TerrPoly = 0;
    PlatCrush = 0;
    terrhitflags = 0;
    for (c = 0; c < 4; c++) {
        if (CurTerr->TrackInfo[c].ptrid == flags) {
            CurTrackInfo = &CurTerr->TrackInfo[c];
            goto track_info_done;
        }
    }
    CurTrackInfo = 0;

track_info_done:
    if (PadRecPtr() >= 0x23D) {
        plathitid = -1;
    }

    TerI = (TerTempInfoType *)NuScratchAlloc32(0x930);
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
    TerI->timeadj = D_0062CDBC;
    TerI->impactadj = D_0062CDC0;
    ScanTerrain(1, 0);

    if (flags[1] != 0) {
        zero = 0;
        d = fptodp(vvel->x);
        if (dpcmp(d, zero) < 0) {
            d = dpsub(zero, d);
        }
        if (dpcmp(d, fptodp(stopflag)) < 0) {
            d = fptodp(vvel->y);
            if (dpcmp(d, zero) < 0) {
                d = dpsub(zero, d);
            }
            if (dpcmp(d, fptodp(stopflag)) < 0) {
                d = fptodp(vvel->z);
                if (dpcmp(d, zero) < 0) {
                    d = dpsub(zero, d);
                }
                if ((dpcmp(d, fptodp(stopflag)) < 0) && (platinrange == 0)) {
                    goto cleanup;
                }
            }
        }
    }

    TerI->curpos.y = TerI->curpos.y * TerI->inyscale;
    TerI->curvel.y = TerI->curvel.y * TerI->inyscale;
    flags[0] = 0;
    flags[1] = 0;
    cnt = PlatformChecks(4, vvel);

    do {
        TerI->ay = (s32)NuAtan2D(TerI->curvel.x, TerI->curvel.z);
        TerI->ax = (s32)NuAtan2D(-TerI->curvel.y,
                                  NuFsqrt(TerI->curvel.x * TerI->curvel.x +
                                          TerI->curvel.z * TerI->curvel.z));
        TerI->len = NuFsqrt(TerI->curvel.x * TerI->curvel.x +
                             TerI->curvel.y * TerI->curvel.y +
                             TerI->curvel.z * TerI->curvel.z);
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
                if ((TerI->uhitnorm.y > D_0062CDC4) && (TerI->hitterrno >= 0) &&
                    (CurTerr->terr[TerI->hitterrno].type == 1)) {
                    CurTerr->platdata[TerrShapeAdjCnt * CurTerr->terr[TerI->hitterrno].info].status.all |= 2;
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

cleanup:
    NuScratchRelease();
    curSphereter = 0;
    TerrShapeAdjCnt = 0;
    curPickInst = 0;
}
