/*
 * Unit: nu3d/nurndr
 *
 * Functions:
 *   0x0011f030 NuRndrSetZBuffer
 *   0x0011f1b8 NuRndrSetRTarget
 *   0x0011f3d0 NuRndrClear
 *   0x0011f870 NuRndrGradClear
 *   0x0011fe28 NuRndrGScnObj
 *   0x00120240 NuRndrGobj
 *   0x00120590 NuRndrFxBlueAndGreenData
 *   0x001207d0 NuRndrFxSpriteSet
 *   0x00120a78 NuRndrFxSprite2Set
 *   0x00120e60 NuRndrFxCode
 *   0x00121220 NuRndrGobjSkin
 *   0x00121498 NuRndrShadowInit
 *   0x00121628 NuRndrStreamQuad3dClip
 *   0x00121988 NuRndrOTInit
 *   0x00121a98 NuRndrOTInsertTriBatch
 *   0x00121ee0 NuRndrOTFlush
 *   0x00122068 NuRndrStreamTriStrip3dClip
 *   0x00122458 NuRndrStreamTri3dClip
 *   0x00122850 NuRndrCylCapMakeData
 *   0x00122b88 NuRndrCylMakeData
 *   0x00122da8 NuRndrShadowCylinderCap
 *   0x00123210 NuRndrShadowEllipse
 *   0x00123660 NuRndrShadowPols
 *   0x001241b8 NuRndrShadowCylinderCap2
 *   0x00124f40 NuRndrShadowStack
 *   0x00125180 NuHGobjRndrMtxDwa
 *   0x00125608 NuHGobjEval
 *   0x00125918 NuHGobjEvalAnim
 *   0x00125c10 NuHGobjEvalAnim2
 *   0x00125f30 NuHGobjRndrAnim
 *   0x00126558 NuHGobjEvalDwa
 *   0x00126758 NuHGobjEvalDwa2
 *   0x00126900 NuHGobjEvalDwaBlend
 *   0x00126c28 NuHGobjEvalDwaBlend2
 *   0x00126ee0 NuHGobjEvalAnimBlend
 *   0x001271e8 NuHGobjEvalAnimBlend2
 *   0x00127530 NuHGobjRndrAnimBlend
 *   0x001279b0 NuRndrLine2d
 *   0x00127c08 NuRndrLineStrip2d
 *   0x00127e70 ClipLinePoint
 *   0x00128138 NuRndrLine3d
 *   0x00128578 NuRndrTri2d
 *   0x001287d0 NuRndrTriStrip2d
 *   0x00128a38 NuRndrTriStrip3d
 *   0x00128c48 NuRndrRect2di
 *   0x00128f40 NuRndrGradRect2di
 *   0x001293c0 NuRndrRectUV2di
 *   0x001296e8 NuRndrLine2di
 *   0x00129940 NuRndrLineRect2di
 *   0x00129ca0 NuRndrTri2di
 *   0x00129f68 NuRndrTriStrip2di
 *   0x0012a218 NuRndrLineStrip2di
 *   0x0012a4c8 NuRndrQuad2d
 *   0x0012a730 NuRndrTriStripGif
 *   0x0012a9c0 NuRndrOldFaceOn
 *   0x0012ae38 NuRndrFaceOn
 *   0x0012b270 NuRndrFaceY
 *   0x0012b610 NuRndrRectUV3di
 *   0x0012b870 NuRndrRectUVStream
 *   0x0012bac0 NuRndrWater
 *   0x0012cbf8 NuRndrShadow
 *   0x0012d238 NuRndrShadowLightFan
 *   0x0012d5b0 NuRndrShadowStencil
 *   0x0012db68 NuRndrTrail
 *   0x0012e200 NuRndrAddFootPrint
 *   0x0012e560 NuRndrFootPrints
 *   0x0012e810 NuRndrStreamQuadShad
 *   0x0012ee58 NuRndrWaterRippleUpdate
 *   0x0012ef88 NuRndrStreamQuadWRip
 *   0x0012f640 NuHGobjRndr
 *   0x0012f6c0 NuHGobjRndrMtx
 *   0x0012f6e0 NuRndrInitEx
 *   0x0012f770 NuRndrInit
 *   0x0012f800 NuRndrClose
 *   0x0012f808 NuRndrBeginScene
 *   0x0012f8c8 NuRndrBeginSceneEx
 *   0x0012f990 NuRndrEndScene
 *   0x0012f9c0 NuRndrEndSceneEx
 *   0x0012fa10 NuRndrSwapScreen
 *   0x0012fa98 NuRndrGlobalMtl
 *   0x0012faa0 NuRndrFogColour
 *   0x0012faa8 NuRndrFogDistance
 *   0x0012fab0 NuRndrFogMode
 *   0x0012fab8 NuRndrParticleGroup
 *   0x0012fc68 NuRndrStarfield
 *   0x0012fc98 NuRndrRect
 *   0x0012fdf0 NuRndrLine3dDbg
 *   0x0012fdf8 NuRndrLine3dDbgFlush
 *   0x0012fe00 NuRndrTri3dClip
 *   0x0012fe28 NuRndrTriStrip3dClip
 *   0x0012fe50 NuRndrQuad3d
 *   0x0012fe58 NuRndrFlush
 *   0x0012fea8 NuRndrShadowOnOff
 *   0x0012feb0 NuRndrRectUV3diNS
 *   0x00130000 NuRndrFx
 *   0x00130078 NuRndrShadowDirCol
 *   0x001300b8 NuRndrWaterLevelTint
 *   0x001300c8 NuRndrAddShadow
 *   0x001301a0 NuRndrShadPolys
 *   0x00130230 NuRndrAddWaterRipple
 *   0x001302a8 NuRndrWaterRip
 *   0x001303a0 NuRndrInitWorld
 *   0x001303d0 NuRndrSplineSearch
 *   0x00130418 NuRndrCreateBlendShapeDeformerWeightsArray
 *   0x001304a0 NuRndrDeformerWeightsArrayInit
 *   0x001304e8 NuRndrFxBlueOrGreenData
 *   0x00130638 NuRndrOTInsertItem
 *   0x00130690 OTTriInit
 *   0x001306c0 OTTriInsert
 *   0x00130740 GlobalCam3DNClip
 *   0x001307c0 RotateCyl
 *   0x00130888 DeRotateVectorAngles
 *   0x00130920 NuRndrTest
 *   0x00130928 NuRndrAnglesZX
 */

typedef unsigned int u32;

extern struct numtl_s *D_00633078;
#define GlobalMtl D_00633078

extern int nurndr_shadowson;


void NuRndrClose(void) {
}


void NuRndrGlobalMtl(struct numtl_s *mtl) {
    GlobalMtl = mtl;
}


void NuRndrFogColour(u32 colour) {
}


void NuRndrFogDistance(float near, float far) {
}


void NuRndrFogMode(u32 mode) {
}


void NuRndrLine3dDbg(u32 colour, float x0, float y0, float z0, float x1, float y1, float z1) {
}


void NuRndrLine3dDbgFlush(void) {
}


int NuRndrQuad3d(void *verts, struct numtl_s *mtl) {
    return 1;
}


void NuRndrShadowOnOff(int on) {
    nurndr_shadowson = on;
}


void NuRndrTest(void) {
}
