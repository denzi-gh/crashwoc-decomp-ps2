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

struct nuvec_s {
    float x;
    float y;
    float z;
};

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

struct numtx_s {
    float _00, _01, _02, _03;
    float _10, _11, _12, _13;
    float _20, _21, _22, _23;
    float _30, _31, _32, _33;
};

struct nupad_s {
    unsigned char pad000[0x568];
    unsigned long long flags2;
    unsigned char pad570[0x57C - 0x570];
    unsigned char r_dx;
    unsigned char r_dy;
    unsigned char l_dx;
    unsigned char l_dy;
    unsigned char pad580[0x588 - 0x580];
    unsigned char l1;
    unsigned char r1;
    unsigned char l2;
    unsigned char r2;
};

extern struct edcam_s *D_0062F99C;

extern int NuPs2ApplyDeadZone(int value, int deadzone);
extern void NuVecRotateY(struct nuvec_s *dst, struct nuvec_s *src, int angle);
extern void NuVecAdd(struct nuvec_s *dst, struct nuvec_s *a, struct nuvec_s *b);
extern void NuVecSub(struct nuvec_s *dst, struct nuvec_s *a, struct nuvec_s *b);
extern double floor(double x);

extern void NuMtxSetTranslation(struct numtx_s *m, struct nuvec_s *v);
extern void NuMtxRotateX(struct numtx_s *m, int angle);
extern void NuMtxRotateY(struct numtx_s *m, int angle);
extern void NuMtxTranslate(struct numtx_s *m, struct nuvec_s *v);
extern void edmainSetCamera(struct numtx_s *m);

#define edcam D_0062F99C


void edcamMove(struct nupad_s *pad)
{
    struct nuvec_s rel;
    struct nuvec_s move;
    int ax;

    if (pad->flags2 & 4) {
        if (edcam->adjust_zoom) {
            edcam->dist += pad->r1 * edcam->zoom_speed;
            edcam->dist -= pad->r2 * edcam->zoom_speed;
            edcam->dist = (edcam->dist > -edcam->zoom_min) ? -edcam->zoom_min : edcam->dist;
        }
        if (edcam->adjust_pitch) {
            ax = edcam->ax - NuPs2ApplyDeadZone(pad->r_dy, 0x20) * edcam->pitch_speed;
            edcam->ax = (ax > 0x4000) ? 0x4000 : ax;
        }
        if (edcam->adjust_yaw) {
            edcam->ax = (edcam->ax < -0x4000) ? -0x4000 : edcam->ax;
            edcam->ay -= NuPs2ApplyDeadZone(pad->r_dx, 0x20) * edcam->yaw_speed;
        }
        move.x = move.y = move.z = 0.0f;
        if (edcam->adjust_y) {
            move.y = pad->l1 * edcam->rise_speed;
            move.y -= pad->l2 * edcam->fall_speed;
        }
        if (edcam->adjust_z) {
            move.z = -(float)NuPs2ApplyDeadZone(pad->l_dy, 0x20) * edcam->move_speed;
        }
        if (edcam->adjust_x) {
            move.x = NuPs2ApplyDeadZone(pad->l_dx, 0x20) * edcam->move_speed;
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
