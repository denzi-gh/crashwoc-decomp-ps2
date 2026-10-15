/*
 * Unit: nu3d/nuvport
 *
 * Functions:
 *   0x00114bd0 NuVpUpdate
 *   0x00114cf0 NuVpSetScalingMtx
 *   0x00114da8 NuVpInit
 *   0x00114ea0 NuVpRestore
 *   0x00114f40 NuVpSetPosition
 *   0x00114f68 NuVpSetSize
 *   0x00114f90 NuVpSetZRange
 *   0x00114fb0 NuVpSetCentre
 *   0x00114fd0 NuVpSetClipping
 *   0x00115078 NuVpGetScalingMtx
 *   0x00115110 NuVpVirtualWidth
 *   0x00115128 NuVpVirtualHeight
 *   0x00115140 NuVpPixelWidth
 *   0x00115168 NuVpPixelHeight
 *   0x00115190 NuVpGetCurrentViewport
 */

#include "numath/numathtypes.h"
#include "nu3d/nuvport.h"

extern struct nuviewport_s D_0067D480;
extern struct nuviewport_s D_0067D4C0;
extern struct numtx_s D_0067D500;
extern int D_00633058;
#define default_viewport D_0067D480
#define current_viewport D_0067D4C0
#define viewport_scaling_matrix D_0067D500
#define viewport_dirty D_00633058

void NuPs2GetViewport(struct nuviewport_s *viewport);


void NuVpUpdate(void)
{
    if (viewport_dirty) {
        viewport_dirty = 0;
        NuVpSetScalingMtx();
        if (current_viewport.clip_left == 0.0f && current_viewport.clip_right == 1.0f) {
            current_viewport.clip_scale_x = 1.0f;
            current_viewport.guard_scale_x = (float)current_viewport.width / 57600.0f;
        } else {
            current_viewport.clip_scale_x = current_viewport.guard_scale_x =
                1.0f / (current_viewport.clip_right - current_viewport.clip_left);
        }
        if (current_viewport.clip_top == 0.0f && current_viewport.clip_bottom == 1.0f) {
            current_viewport.clip_scale_y = 1.0f;
            current_viewport.guard_scale_y = (float)current_viewport.height / 57600.0f;
        } else {
            current_viewport.clip_scale_y = current_viewport.guard_scale_y =
                1.0f / (current_viewport.clip_bottom - current_viewport.clip_top);
        }
    }
}


void NuVpSetScalingMtx(void)
{
    struct nuviewport_s *viewport = &current_viewport;
    struct numtx_s *matrix = &viewport_scaling_matrix;
    float width = (float)viewport->width;
    float height = (float)viewport->height;

    matrix->_00 = width * 0.5f;
    matrix->_01 = 0.0f;
    matrix->_02 = 0.0f;
    matrix->_03 = 0.0f;
    matrix->_10 = 0.0f;
    matrix->_11 = -height * 0.5f;
    matrix->_12 = 0.0f;
    matrix->_13 = 0.0f;
    matrix->_20 = 0.0f;
    matrix->_21 = 0.0f;
    matrix->_22 = viewport->zmax - viewport->zmin;
    matrix->_23 = 0.0f;
    matrix->_30 = (float)viewport->x + width * viewport->centre_x;
    matrix->_31 = (float)viewport->y + height * viewport->centre_y;
    matrix->_32 = viewport->zmin;
    matrix->_33 = 1.0f;
}


void NuVpInit(void)
{
    NuPs2GetViewport(&default_viewport);
    default_viewport.centre_x = 0.5f;
    default_viewport.centre_y = 0.5f;
    default_viewport.clip_left = 0.0f;
    default_viewport.clip_top = 0.0f;
    default_viewport.clip_right = 1.0f;
    default_viewport.clip_bottom = 1.0f;
    current_viewport = default_viewport;
    viewport_dirty = 1;
    NuVpUpdate();
}


void NuVpRestore(void)
{
    current_viewport = default_viewport;
    viewport_dirty = 1;
}


void NuVpSetPosition(float x, float y)
{
    current_viewport.x = (int)x;
    current_viewport.y = (int)y;
    viewport_dirty = 1;
}


void NuVpSetSize(float width, float height)
{
    viewport_dirty = 1;
    current_viewport.width = (int)width;
    current_viewport.height = (int)height;
}


void NuVpSetZRange(float zmin, float zmax)
{
    viewport_dirty = 1;
    current_viewport.zmin = zmin;
    current_viewport.zmax = zmax;
}


void NuVpSetCentre(float x, float y)
{
    viewport_dirty = 1;
    current_viewport.centre_x = x;
    current_viewport.centre_y = y;
}


void NuVpSetClipping(float left, float top, float right, float bottom)
{
    struct nuviewport_s *viewport = &current_viewport;

    if (left < 0.0f) {
        viewport->clip_left = 0.0f;
    } else {
        viewport->clip_left = left;
    }
    if (top < 0.0f) {
        current_viewport.clip_top = 0.0f;
    } else {
        current_viewport.clip_top = top;
    }
    if (right > 1.0f) {
        current_viewport.clip_right = 1.0f;
    } else {
        current_viewport.clip_right = right;
    }
    if (bottom > 1.0f) {
        current_viewport.clip_bottom = 1.0f;
    } else {
        current_viewport.clip_bottom = bottom;
    }
    viewport_dirty = 1;
}


void NuVpGetScalingMtx(struct numtx_s *matrix)
{
    if (matrix) {
        *matrix = viewport_scaling_matrix;
    }
}


float NuVpVirtualWidth(float width)
{
    return width / (float)current_viewport.width;
}


float NuVpVirtualHeight(float height)
{
    return height / (float)current_viewport.height;
}


float NuVpPixelWidth(float width)
{
    return width * (float)current_viewport.width + (float)current_viewport.x;
}


float NuVpPixelHeight(float height)
{
    return height * (float)current_viewport.height + (float)current_viewport.y;
}


struct nuviewport_s *NuVpGetCurrentViewport(void)
{
    return &current_viewport;
}
