#ifndef GAMELIB_TERRAIN_H
#define GAMELIB_TERRAIN_H

#include "nu_types.h"
#include "numath/numathtypes.h"

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
