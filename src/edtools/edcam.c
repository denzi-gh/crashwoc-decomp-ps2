/*
 * Unit: edtools/edcam
 *
 * Functions:
 *   0x001c1cf8 edcamMove
 *   0x001c2070 edcamSetSpeed
 *   0x001c2088 edcamSetAdjustFreedom
 *   0x001c2110 edcamSet
 *   0x001c2178 edcamSetPos
 *   0x001c2198 edcamSetAng
 *   0x001c21a8 edcamSetDist
 *   0x001c21b8 edcamGetDist
 *   0x001c21c8 edcamSetPosAng
 *   0x001c21f8 edcamGetPosAng
 *   0x001c2240 edcamGetPosAngSnap
 *   0x001c2288 edcamGetPosPointer
 */

#include "nu_libc.h"

#include "numath/numathtypes.h"
#include "numath/numtx.h"
#include "numath/nuvec.h"
#include "nups2/ps2ctl.h"

struct edcam_s {
    struct nuvec_s pos;
    int ax;
    int ay;
    float dist;
    struct nuvec_s snap_origin;
    struct nuvec_s snap_grid;
    char pad30[0x38 - 0x30];
    struct nuvec_s snap_pos;
    int snap_ax;
    int snap_ay;
    float rise_speed;
    float move_speed;
    float fall_speed;
    int pitch_speed;
    int yaw_speed;
    float zoom_speed;
    float zoom_min;
    int adjust_x : 1;
    int adjust_y : 1;
    int adjust_z : 1;
    int adjust_pitch : 1;
    int adjust_yaw : 1;
    int adjust_zoom : 1;
};

extern struct edcam_s *D_0062F99C;


extern void edmainSetCamera(struct numtx_s *m);

#define edcam D_0062F99C


void edcamMove(struct nupad_s *pad)
{
    struct nuvec_s rel;
    struct nuvec_s move;
    int ax;

    if (pad->valid) {
        if (edcam->adjust_zoom) {
            edcam->dist += pad->rdata.r1_alg * edcam->zoom_speed;
            edcam->dist -= pad->rdata.r2_alg * edcam->zoom_speed;
            edcam->dist = (edcam->dist > -edcam->zoom_min) ? -edcam->zoom_min : edcam->dist;
        }
        if (edcam->adjust_pitch) {
            ax = edcam->ax - NuPs2ApplyDeadZone(pad->rdata.r_alg_y, 0x20) * edcam->pitch_speed;
            edcam->ax = (ax > 0x4000) ? 0x4000 : ax;
        }
        if (edcam->adjust_yaw) {
            edcam->ax = (edcam->ax < -0x4000) ? -0x4000 : edcam->ax;
            edcam->ay -= NuPs2ApplyDeadZone(pad->rdata.r_alg_x, 0x20) * edcam->yaw_speed;
        }
        move.x = move.y = move.z = 0.0f;
        if (edcam->adjust_y) {
            move.y = pad->rdata.l1_alg * edcam->rise_speed;
            move.y -= pad->rdata.l2_alg * edcam->fall_speed;
        }
        if (edcam->adjust_z) {
            move.z = -(float)NuPs2ApplyDeadZone(pad->rdata.l_alg_y, 0x20) * edcam->move_speed;
        }
        if (edcam->adjust_x) {
            move.x = NuPs2ApplyDeadZone(pad->rdata.l_alg_x, 0x20) * edcam->move_speed;
        }
        NuVecRotateY(&move, &move, edcam->ay);
        NuVecAdd(&edcam->pos, &edcam->pos, &move);
        NuVecSub(&rel, &edcam->pos, &edcam->snap_origin);
        rel.x /= edcam->snap_grid.x;
        rel.y /= edcam->snap_grid.y;
        rel.z /= edcam->snap_grid.z;
        edcam->snap_pos.x = floor(rel.x) * edcam->snap_grid.x + edcam->snap_origin.x;
        edcam->snap_pos.y = floor(rel.y) * edcam->snap_grid.y + edcam->snap_origin.y;
        edcam->snap_pos.z = floor(rel.z) * edcam->snap_grid.z + edcam->snap_origin.z;
    }
}


void edcamSetSpeed(float rise, float move, float fall, float zoom)
{
    edcam->rise_speed = rise;
    edcam->move_speed = move;
    edcam->fall_speed = fall;
    edcam->zoom_speed = zoom;
}


void edcamSetAdjustFreedom(int x, int y, int z, int pitch, int yaw, int zoom)
{
    edcam->adjust_x = x;
    edcam->adjust_y = y;
    edcam->adjust_z = z;
    edcam->adjust_pitch = pitch;
    edcam->adjust_yaw = yaw;
    edcam->adjust_zoom = zoom;
}


void edcamSet(void)
{
    struct numtx_s mtx;
    struct nuvec_s offset;

    offset.x = 0.0f;
    offset.y = 0.0f;
    offset.z = edcam->dist;
    NuMtxSetTranslation(&mtx, &offset);
    NuMtxRotateX(&mtx, edcam->ax);
    NuMtxRotateY(&mtx, edcam->ay);
    NuMtxTranslate(&mtx, &edcam->pos);
    edmainSetCamera(&mtx);
}


void edcamSetPos(struct nuvec_s *pos)
{
    edcam->pos = *pos;
}


void edcamSetAng(int ax, int ay)
{
    edcam->ax = ax;
    edcam->ay = ay;
}


void edcamSetDist(float dist)
{
    edcam->dist = dist;
}


float edcamGetDist(void)
{
    return edcam->dist;
}


void edcamSetPosAng(struct nuvec_s *pos, int ax, int ay)
{
    edcam->pos = *pos;
    edcam->ax = ax;
    edcam->ay = ay;
}


void edcamGetPosAng(struct nuvec_s *pos, int *ax, int *ay)
{
    if (pos != 0) {
        *pos = edcam->pos;
    }
    if (ax != 0) {
        *ax = edcam->ax;
    }
    if (ay != 0) {
        *ay = edcam->ay;
    }
}


void edcamGetPosAngSnap(struct nuvec_s *pos, int *ax, int *ay)
{
    if (pos != 0) {
        *pos = edcam->snap_pos;
    }
    if (ax != 0) {
        *ax = edcam->snap_ax;
    }
    if (ay != 0) {
        *ay = edcam->snap_ay;
    }
}


struct nuvec_s *edcamGetPosPointer(void)
{
    return &edcam->pos;
}
