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

static struct nuviewport_s vpDevice;
static struct nuviewport_s vpCurrent;
static struct numtx_s smtx;
static int vport_inval;

void NuPs2GetViewport(struct nuviewport_s *viewport);


void NuVpUpdate(void)
{
    if (vport_inval) {
        vport_inval = 0;
        NuVpSetScalingMtx();
        if (vpCurrent.clip_left == 0.0f && vpCurrent.clip_right == 1.0f) {
            vpCurrent.clip_scale_x = 1.0f;
            vpCurrent.guard_scale_x = (float)vpCurrent.width / 57600.0f;
        } else {
            vpCurrent.clip_scale_x = vpCurrent.guard_scale_x =
                1.0f / (vpCurrent.clip_right - vpCurrent.clip_left);
        }
        if (vpCurrent.clip_top == 0.0f && vpCurrent.clip_bottom == 1.0f) {
            vpCurrent.clip_scale_y = 1.0f;
            vpCurrent.guard_scale_y = (float)vpCurrent.height / 57600.0f;
        } else {
            vpCurrent.clip_scale_y = vpCurrent.guard_scale_y =
                1.0f / (vpCurrent.clip_bottom - vpCurrent.clip_top);
        }
    }
}


void NuVpSetScalingMtx(void)
{
    struct nuviewport_s *viewport = &vpCurrent;
    struct numtx_s *matrix = &smtx;
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
    NuPs2GetViewport(&vpDevice);
    vpDevice.centre_x = 0.5f;
    vpDevice.centre_y = 0.5f;
    vpDevice.clip_left = 0.0f;
    vpDevice.clip_top = 0.0f;
    vpDevice.clip_right = 1.0f;
    vpDevice.clip_bottom = 1.0f;
    vpCurrent = vpDevice;
    vport_inval = 1;
    NuVpUpdate();
}


void NuVpRestore(void)
{
    vpCurrent = vpDevice;
    vport_inval = 1;
}


void NuVpSetPosition(float x, float y)
{
    vpCurrent.x = (int)x;
    vpCurrent.y = (int)y;
    vport_inval = 1;
}


void NuVpSetSize(float width, float height)
{
    vport_inval = 1;
    vpCurrent.width = (int)width;
    vpCurrent.height = (int)height;
}


void NuVpSetZRange(float zmin, float zmax)
{
    vport_inval = 1;
    vpCurrent.zmin = zmin;
    vpCurrent.zmax = zmax;
}


void NuVpSetCentre(float x, float y)
{
    vport_inval = 1;
    vpCurrent.centre_x = x;
    vpCurrent.centre_y = y;
}


void NuVpSetClipping(float left, float top, float right, float bottom)
{
    struct nuviewport_s *viewport = &vpCurrent;

    if (left < 0.0f) {
        viewport->clip_left = 0.0f;
    } else {
        viewport->clip_left = left;
    }
    if (top < 0.0f) {
        vpCurrent.clip_top = 0.0f;
    } else {
        vpCurrent.clip_top = top;
    }
    if (right > 1.0f) {
        vpCurrent.clip_right = 1.0f;
    } else {
        vpCurrent.clip_right = right;
    }
    if (bottom > 1.0f) {
        vpCurrent.clip_bottom = 1.0f;
    } else {
        vpCurrent.clip_bottom = bottom;
    }
    vport_inval = 1;
}


void NuVpGetScalingMtx(struct numtx_s *matrix)
{
    if (matrix) {
        *matrix = smtx;
    }
}


float NuVpVirtualWidth(float width)
{
    return width / (float)vpCurrent.width;
}


float NuVpVirtualHeight(float height)
{
    return height / (float)vpCurrent.height;
}


float NuVpPixelWidth(float width)
{
    return width * (float)vpCurrent.width + (float)vpCurrent.x;
}


float NuVpPixelHeight(float height)
{
    return height * (float)vpCurrent.height + (float)vpCurrent.y;
}


struct nuviewport_s *NuVpGetCurrentViewport(void)
{
    return &vpCurrent;
}
