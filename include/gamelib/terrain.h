#ifndef GAMELIB_TERRAIN_H
#define GAMELIB_TERRAIN_H

#include "nu_types.h"
#include "numath/numathtypes.h"

/* Terrain collision structures (offsets verified against retail asm). */
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
    struct numtx_s oldmtx;    /* 0x00 */
    struct numtx_s *curmtx;   /* 0x40 */
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
    s16 start;                /* 0x00 */
    s16 count;                /* 0x02 */
    f32 bounds[4];            /* 0x04 */
} TerrainGroupType; /* 0x14 */

typedef struct {
    f32 x;                                /* 0x000 */
    f32 y;                                /* 0x004 */
    f32 z;                                /* 0x008 */
    f32 x2;                               /* 0x00C */
    f32 y2;                               /* 0x010 */
    f32 z2;                               /* 0x014 */
    s16 timer;                            /* 0x018 */
    s16 pad;                              /* 0x01A */
    tertype *hitdata[512];                /* 0x01C */
} TerrainPollType; /* 0x81C */

typedef struct {
    TerrainSituType *terr;                /* 0x0000 */
    TerrainPlatformType platdata[128];    /* 0x0004 */
    void *wallinfo;                       /* 0x3204 */
    TerrainTrackInfoType TrackInfo[4];    /* 0x3208 */
    TerrainGroupType terrgroup[257];      /* 0x3238 */
    s32 miny;                             /* 0x464C */
    s32 terrgroupcnt;                     /* 0x4650 */
    s16 terrlist[4096];                   /* 0x4654 */
    TerrainPollType pollist[9];           /* 0x6654 */
} TerrainSetType;

typedef struct {
    s32 offset;
    struct nuvec_s translation;
    s16 type;
    s16 info;
    s16 rx;
    s16 ry;
    s16 rz;
    s16 pad;
    struct nuvec_s rotation;
    u32 flags;
    s16 prim;
    s16 id;
    s32 datapos;
} OffType;

typedef struct {
    s32 count;
    OffType offlist[512];
} OffFileType;

typedef struct {
    u16 count;
    u16 val;
    struct nuvec_s spl[4096];
} WallSplineType;

struct nugscn_s;

short *terraininit(s32 levelnum, short **store, short *endstore, s32 opt, char *name, struct nugscn_s *gscene, s32 gsid);
void terrainpickupinit(char *name, short **store);
void TerrainPlatformOldUpdate(void);
void TerrainPlatformNewUpdate(void);
void SphereDraw(struct nuvec_s *pos, f32 radius, f32 scale);
void RotateVec(struct nuvec_s *in, struct nuvec_s *out);
void DeRotatePoint(struct nuvec_s *pnt);
void TerrainSideClamp(struct nuvec_s *slide, struct nuvec_s *pos);
short InsideLineF(float _x, float _z, float _x0, float _z0, float _x1, float _z1);
short InsidePolLines(f32 x, f32 y, f32 z, f32 dx0, f32 dy0, f32 dz0, f32 dx1, f32 dy1, f32 dz1, struct nuvec_s *norm);
void NewScan(struct nuvec_s *ppos, s32 extramask, s32 platscan);
void NewScanRot(struct nuvec_s *ppos, s32 extramask);
float NewCast(struct nuvec_s *pos, float ytol);
s32 CheckCylinder(s32 p0, s32 p1, s32 *flags, s32 type);
s32 CheckSphere(s32 p);
s32 CheckSphereTer(struct nuvec_s *pnt, f32 radius);
s32 HitWallSpline(void);
s32 HitTerrain(void);
void TerrainImpactNorm(void);
void TerrainImpact(struct nuvec_s *vpos, struct nuvec_s *vvel, u8 *flags);
s32 TerrainImpactPlatform(u8 *flags);
s32 TerrainPlatformMoveCheck(struct nuvec_s *dest, struct nuvec_s *norm, s32 platid, s32 impact, s32 keepscan);
s32 TerrainPlatformEmbedded(struct nuvec_s *vvel);
void StorePlatImpact(void);
s32 PlatformChecks(s32 itterationcnt, struct nuvec_s *vvel);
s32 NewPlatInst(struct numtx_s *mtx, s32 instance);
s32 DeletePlatinst(s32 id);
void TerrShowCamTerr(void);
void TerrainSetCur(void *curterr);
f32 NewShadow(struct nuvec_s *ppos, f32 size);
f32 NewShadowMask(struct nuvec_s *ppos, f32 size, s32 extramask);
f32 NewShadowTol(struct nuvec_s *ppos, f32 size, f32 tol);
f32 NewShadowTolMask(struct nuvec_s *ppos, f32 size, f32 tol, s32 extramask);
f32 NewShadowPlat(struct nuvec_s *ppos, f32 size);
f32 NewShadowMaskPlat(struct nuvec_s *ppos, f32 size, s32 extramask);
f32 NewShadowMaskPlatRot(struct nuvec_s *ppos, f32 size, s32 extramask);
f32 NewShadowTolPlat(struct nuvec_s *ppos, f32 size, f32 tol);
f32 NewShadowTolMaskPlat(struct nuvec_s *ppos, f32 size, f32 tol, s32 extramask);
void AddCollisionSphere(struct nuvec_s *pos, f32 radius);
void AddPickupTerr(s32 type, struct nuvec_s *pos);
void *CrashDataPtr(void);
void NewShapeInit(s32 *shape);
s32 TerrainInfo(void);
s32 ShadowInfo(void);
s32 ShadowRoofInfo(void);
s32 EShadowRoofInfo(void);
void ShadowDir(struct nuvec_s *dir);
s32 EShadowInfo(void);
void noterraininit(void);
s32 FindPlatInst(s32 instance);
void UpdatePlatinst(s32 id, struct numtx_s *mtx);
void PlatInstRotate(s32 id, s32 rot);
void PlatInstBounce(s32 id, f32 plrgrav, f32 tension, f32 damp);
s32 PlatInstGetHit(s32 id);
s32 PlatImpactInfo(struct nuvec_s *norm, s32 *info0, s32 *info1);
void TerrainPlatGetMtx(s32 id, struct numtx_s **oldmtx, struct numtx_s **curmtx);
s32 TerrainPlatId(void);
s32 PlatformCrush(void);
void NewScanInit(void);
short *NewScanHandel(struct nuvec_s *pos, struct nuvec_s *vel, s32 platscan, short *handel, f32 size);
void NewRaySetDisablePalt(s32 disable);
void DrawPlatform(s32 id);
void DrawWallSpline(f32 y);
void FullReflect(struct nuvec_s *norm, struct nuvec_s *vel, struct nuvec_s *out);
void OffPlat(void);
void NewTerrain(struct nuvec_s *vpos, struct nuvec_s *vvel, u8 *flags, s32 terid, f32 stopflag, f32 size);
s32 TerrShapeSideStep(struct nuvec_s *vpos, struct nuvec_s *vvel, u8 *flags);
void CubeImpact(struct numtx_s *mat, struct numtx_s *nmat, struct nuvec_s *norm, f32 size, struct nuvec_s *impact);
void NewTerrainScaleY(struct nuvec_s *vpos, struct nuvec_s *vvel, u8 *flags, s32 terid, f32 stopflag, f32 size, f32 yscale);
void RayImpact(struct nuvec_s *vec);
s32 NewRayCast(struct nuvec_s *vpos, struct nuvec_s *vvel, f32 size);
s32 NewRayCastMask(struct nuvec_s *vpos, struct nuvec_s *vvel, f32 size, s32 extramask);
s32 NewRayCastSet(struct nuvec_s *vpos, struct nuvec_s *vvel, f32 size, f32 timeadj, f32 impactadj, s32 platscan);
s32 NewRayCastSetMask(struct nuvec_s *vpos, struct nuvec_s *vvel, f32 size, f32 timeadj, f32 impactadj, s32 platscan, s32 extramask);
short *NewScanHandelFull(struct nuvec_s *vpos, struct nuvec_s *vvel, s32 platscan, s32 extramask, f32 size);
short *NewScanHandelSubset(short *handel, struct nuvec_s *vpos, struct nuvec_s *vvel, s32 extramask, f32 size);
void ScanTerrain(s32 platscan, s32 extramask);
void ScanTerrainHandel(s32 extramask, short *Handel);
s32 NewRayCastSetHandel(struct nuvec_s *vpos, struct nuvec_s *vvel, f32 size, f32 timeadj, f32 impactadj, short *handel);
s32 TerrainTrackBack(struct nuvec_s *pos, struct nuvec_s *vel, struct nuvec_s *dest, f32 size, f32 timeadj);
void ScanTerrainPlatform(s32 msituid, s32 extramask);
s32 NewRayCastPlatForm(struct nuvec_s *vpos, struct nuvec_s *vvel, f32 size, f32 timeadj, s32 platid);

#endif
