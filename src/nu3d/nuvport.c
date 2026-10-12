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

/* Fields accessed by the viewport setters and coordinate helpers. */
typedef struct NuViewport {
    int x;
    int y;
    int width;
    int height;
    float zmin;
    float zmax;
    float centre_x;
    float centre_y;
    float clip_left;
    float clip_top;
    float clip_right;
    float clip_bottom;
    float clip_scale_x;
    float clip_scale_y;
    float guard_scale_x;
    float guard_scale_y;
} NuViewport;

typedef struct NuViewportMatrix {
    float elements[16];
} NuViewportMatrix;

extern NuViewport D_0067D480;
extern NuViewport D_0067D4C0;
extern NuViewportMatrix D_0067D500;
extern int D_00633058;
#define default_viewport D_0067D480
#define current_viewport D_0067D4C0
#define viewport_scaling_matrix D_0067D500
#define viewport_dirty D_00633058

void NuPs2GetViewport(NuViewport *viewport);
void NuVpSetScalingMtx(void);


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
    NuViewport *viewport = &current_viewport;
    NuViewportMatrix *matrix = &viewport_scaling_matrix;
    float width = (float)viewport->width;
    float height = (float)viewport->height;

    matrix->elements[0] = width * 0.5f;
    matrix->elements[1] = 0.0f;
    matrix->elements[2] = 0.0f;
    matrix->elements[3] = 0.0f;
    matrix->elements[4] = 0.0f;
    matrix->elements[5] = -height * 0.5f;
    matrix->elements[6] = 0.0f;
    matrix->elements[7] = 0.0f;
    matrix->elements[8] = 0.0f;
    matrix->elements[9] = 0.0f;
    matrix->elements[10] = viewport->zmax - viewport->zmin;
    matrix->elements[11] = 0.0f;
    matrix->elements[12] = (float)viewport->x + width * viewport->centre_x;
    matrix->elements[13] = (float)viewport->y + height * viewport->centre_y;
    matrix->elements[14] = viewport->zmin;
    matrix->elements[15] = 1.0f;
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
    NuViewport *viewport = &current_viewport;

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


void NuVpGetScalingMtx(NuViewportMatrix *matrix)
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


NuViewport *NuVpGetCurrentViewport(void)
{
    return &current_viewport;
}
