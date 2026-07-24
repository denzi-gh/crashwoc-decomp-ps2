/*
 * Unit: nu3d/nucvtgeo
 *
 * Functions:
 *   0x001505b0 NuCvtDmaFaceOn
 *   0x00150a58 NuCvtDmaFaceY
 *   0x00150f28 NuCvtSizeDmaBezTriCFromIdx
 *   0x00151020 NuCvtDmaBezTriCFromIdx
 *   0x00151c40 NuCvtTriStreamCFromIdx
 *   0x00152090 NuCvtTriStripStreamCFromIdx
 *   0x00152528 NuCvtSizeDmaTriCFromIdx
 *   0x001526f0 NuCvtDmaTriCFromIdx
 *   0x00153320 NuCvtSizeDmaTriStripCFromIdx
 *   0x00153738 NuCvtDmaTriStripCFromIdx
 *   0x001547d8 NuPs2CreateFaceOn
 *   0x00154b40 NuCvtCalcFastPoint
 *   0x00154cd0 NuPs2CreateRenderStream
 *   0x00155140 NuPs2DestroyRenderStream
 *   0x00155178 NuCvtGeoSetColourRefOffset
 *   0x00155180 NuCvtGeoFixup
 *   0x001551c0 NuCvtGeoBeginColourRef
 *   0x001551d8 NuCvtGeoEndColourRef
 *   0x00155210 NuCvtSizeDmaFaceOn
 *   0x001552b8 NuCvtSizeDmaFaceY
 *   0x00155360 NuCvtSetMaxSpan
 *   0x00155390 NuCvtGeoAddColourRef
 *   0x001553d8 NuCvtDestroyPacketInfo
 */

struct nucolourref_s { // 0xc
	/* 0x0 */ unsigned int *addr;
	/* 0x4 */ int count;
	/* 0x8 */ unsigned int cols;
};


void NuCvtGeoFixup(struct nucolourref_s* ref, int offset) {
        while(ref->count != 0)
        {
            ref->addr = (int)((char*)offset + (int)ref->addr);
            ref = (char*)ref + ((ref->count * 8) + 8);
        }
}
