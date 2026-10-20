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
    int snap_grid_ax;
    int snap_grid_ay;
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

static struct edcam_s gp_cam = {
    { 0.0f, 0.0f, 0.0f }, 0, 0, -2.0f,          /* pos, ax, ay, dist */
    { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, /* snap_origin, snap_grid */
    1, 1,                                       /* snap_grid_ax, snap_grid_ay */
    { 0.0f, 0.0f, 0.0f }, 0, 0,                 /* snap_pos, snap_ax, snap_ay */
    0.0015625f, 0.0015625f, 0.0015625f,         /* rise, move, fall speed */
    2, 2, 0.0078125f, 0.5f,                     /* pitch, yaw, zoom speed, zoom_min */
    1, 1, 1, 1, 1, 1                            /* adjust x, y, z, pitch, yaw, zoom */
};

static struct edcam_s *ed_cam = &gp_cam;


extern void edmainSetCamera(struct numtx_s *m);


void edcamMove(struct nupad_s *pad)
{
    struct nuvec_s rel;
    struct nuvec_s move;
    int ax;

    if (pad->valid) {
        if (ed_cam->adjust_zoom) {
            ed_cam->dist += pad->rdata.r1_alg * ed_cam->zoom_speed;
            ed_cam->dist -= pad->rdata.r2_alg * ed_cam->zoom_speed;
            ed_cam->dist = (ed_cam->dist > -ed_cam->zoom_min) ? -ed_cam->zoom_min : ed_cam->dist;
        }
        if (ed_cam->adjust_pitch) {
            ax = ed_cam->ax - NuPs2ApplyDeadZone(pad->rdata.r_alg_y, 0x20) * ed_cam->pitch_speed;
            ed_cam->ax = (ax > 0x4000) ? 0x4000 : ax;
        }
        if (ed_cam->adjust_yaw) {
            ed_cam->ax = (ed_cam->ax < -0x4000) ? -0x4000 : ed_cam->ax;
            ed_cam->ay -= NuPs2ApplyDeadZone(pad->rdata.r_alg_x, 0x20) * ed_cam->yaw_speed;
        }
        move.x = move.y = move.z = 0.0f;
        if (ed_cam->adjust_y) {
            move.y = pad->rdata.l1_alg * ed_cam->rise_speed;
            move.y -= pad->rdata.l2_alg * ed_cam->fall_speed;
        }
        if (ed_cam->adjust_z) {
            move.z = -(float)NuPs2ApplyDeadZone(pad->rdata.l_alg_y, 0x20) * ed_cam->move_speed;
        }
        if (ed_cam->adjust_x) {
            move.x = NuPs2ApplyDeadZone(pad->rdata.l_alg_x, 0x20) * ed_cam->move_speed;
        }
        NuVecRotateY(&move, &move, ed_cam->ay);
        NuVecAdd(&ed_cam->pos, &ed_cam->pos, &move);
        NuVecSub(&rel, &ed_cam->pos, &ed_cam->snap_origin);
        rel.x /= ed_cam->snap_grid.x;
        rel.y /= ed_cam->snap_grid.y;
        rel.z /= ed_cam->snap_grid.z;
        ed_cam->snap_pos.x = floor(rel.x) * ed_cam->snap_grid.x + ed_cam->snap_origin.x;
        ed_cam->snap_pos.y = floor(rel.y) * ed_cam->snap_grid.y + ed_cam->snap_origin.y;
        ed_cam->snap_pos.z = floor(rel.z) * ed_cam->snap_grid.z + ed_cam->snap_origin.z;
    }
}


void edcamSetSpeed(float rise, float move, float fall, float zoom)
{
    ed_cam->rise_speed = rise;
    ed_cam->move_speed = move;
    ed_cam->fall_speed = fall;
    ed_cam->zoom_speed = zoom;
}


void edcamSetAdjustFreedom(int x, int y, int z, int pitch, int yaw, int zoom)
{
    ed_cam->adjust_x = x;
    ed_cam->adjust_y = y;
    ed_cam->adjust_z = z;
    ed_cam->adjust_pitch = pitch;
    ed_cam->adjust_yaw = yaw;
    ed_cam->adjust_zoom = zoom;
}


void edcamSet(void)
{
    struct numtx_s mtx;
    struct nuvec_s offset;

    offset.x = 0.0f;
    offset.y = 0.0f;
    offset.z = ed_cam->dist;
    NuMtxSetTranslation(&mtx, &offset);
    NuMtxRotateX(&mtx, ed_cam->ax);
    NuMtxRotateY(&mtx, ed_cam->ay);
    NuMtxTranslate(&mtx, &ed_cam->pos);
    edmainSetCamera(&mtx);
}


void edcamSetPos(struct nuvec_s *pos)
{
    ed_cam->pos = *pos;
}


void edcamSetAng(int ax, int ay)
{
    ed_cam->ax = ax;
    ed_cam->ay = ay;
}


void edcamSetDist(float dist)
{
    ed_cam->dist = dist;
}


float edcamGetDist(void)
{
    return ed_cam->dist;
}


void edcamSetPosAng(struct nuvec_s *pos, int ax, int ay)
{
    ed_cam->pos = *pos;
    ed_cam->ax = ax;
    ed_cam->ay = ay;
}


void edcamGetPosAng(struct nuvec_s *pos, int *ax, int *ay)
{
    if (pos != 0) {
        *pos = ed_cam->pos;
    }
    if (ax != 0) {
        *ax = ed_cam->ax;
    }
    if (ay != 0) {
        *ay = ed_cam->ay;
    }
}


void edcamGetPosAngSnap(struct nuvec_s *pos, int *ax, int *ay)
{
    if (pos != 0) {
        *pos = ed_cam->snap_pos;
    }
    if (ax != 0) {
        *ax = ed_cam->snap_ax;
    }
    if (ay != 0) {
        *ay = ed_cam->snap_ay;
    }
}


struct nuvec_s *edcamGetPosPointer(void)
{
    return &ed_cam->pos;
}
