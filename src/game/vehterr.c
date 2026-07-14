/*
 * Unit: game/vehterr
 *
 * Functions:
 *   0x00221b60 TryUnembeddPointDir
 *   0x00221de0 BestGuessActualsJeep
 *   0x00222198 MyCast
 *   0x00222918 FindSurfaceNormalAndUnembedd
 *   0x00222f80 TerrainVehicleSoft
 *   0x00223608 TryUnembeddPointDirSimple
 *   0x002237a8 TryUnembeddPointSafe
 *   0x00223948 FindVehicleNormalGiven4Points
 *   0x00223a70 FindSurfaceRotXZFromNormal
 */

#include "creature.h"

extern int NuAtan2D(float a, float b);
extern void NuVecRotateX(struct nuvec_s *dst, struct nuvec_s *src, int angle);


void FindSurfaceRotXZFromNormal(struct nuvec_s *normal, s16 *rotX, s16 *rotZ) {
    struct nuvec_s temp;
    u16 rx;
    int rz;

    rx = NuAtan2D(normal->z, normal->y);
    NuVecRotateX(&temp, normal, -rx);
    rz = NuAtan2D(temp.x, temp.y);
    *rotX = rx;
    *rotZ = -rz;
}
