/*
 * Unit: numath/nutrig
 *
 * Functions:
 *   0x0010aa30 NuAtani
 *   0x0010ac70 NuAtanf
 *   0x0010af40 NuAtan2D
 *   0x0010b1f0 NuTrigInit
 *   0x0010b278 NuAtan2
 *   0x0010b2c8 NuAngAdd
 *   0x0010b2f0 NuAngSub
 *   0x0010b318 NuPower2
 */

#include "nu_libc.h"

#include "numath/nutrig.h"

extern f32 NuTrigTable[];
extern u16 D_00293960[];
#define ang D_00293960

static inline u16 xy(s32 x, s32 y)
{
    u64 wide_x = x;
    u64 wide_y = y;

    if (wide_y < wide_x) {
        return 0x4000 - ang[(wide_y << 9) / wide_x];
    }
    return ang[(wide_x << 9) / wide_y];
}


s32 NuAtani(s32 x, s32 y)
{
    if (x == 0) {
        return y < 0 ? 0x8000 : 0;
    }
    if (y == 0) {
        return x < 0 ? 0xc000 : 0x4000;
    }

    if (x < 0) {
        if (y < 0) {
            return xy(-x, -y) + 0x8000;
        }
        return -xy(-x, y);
    }

    if (y < 0) {
        return 0x8000 - xy(x, -y);
    }

    return xy(x, y);
}


static inline f32 xyf(s32 x, s32 y)
{
    u64 wide_x = x;
    u64 wide_y = y;

    if (wide_y < wide_x) {
        return (0x4000 - ang[(wide_y << 9) / wide_x]) * 0.0000958738f;
    }
    return ang[(wide_x << 9) / wide_y] * 0.0000958738f;
}


f32 NuAtanf(s32 x, s32 y)
{
    if (x == 0) {
        return y < 0 ? 3.1415927f : 0.0f;
    }
    if (y == 0) {
        return x < 0 ? 4.712389f : 1.5707964f;
    }

    if (x < 0) {
        if (y < 0) {
            return xyf(-x, -y) + 3.1415927f;
        }
        return -xyf(-x, y);
    }

    if (y < 0) {
        return 3.1415927f - xyf(x, -y);
    }

    return xyf(x, y);
}


static inline u16 fxyd(f32 x, f32 y)
{
    if (y < x) {
        return 0x4000 - ang[(s32)((y * 512.0f) / x)];
    }
    return ang[(s32)((x * 512.0f) / y)];
}


s32 NuAtan2D(f32 x, f32 y)
{
    if (x == 0.0f) {
        return y < 0.0f ? 0x8000 : 0;
    }
    if (y == 0.0f) {
        return x < 0.0f ? 0xc000 : 0x4000;
    }

    if (x < 0.0f) {
        if (y < 0.0f) {
            return fxyd(-x, -y) + 0x8000;
        }
        return -fxyd(-x, y);
    }

    if (y < 0.0f) {
        return 0x8000 - fxyd(x, -y);
    }

    return fxyd(x, y);
}


void NuTrigInit(void)
{
    s32 i;

    for (i = 0; i < 0x10000; i++) {
        NuTrigTable[i] = sin(i * 0.0000958738f);
    }
}

f32 NuAtan2(f32 y, f32 x)
{
    return atan2(y, x);
}

s32 NuAngAdd(s32 a, s32 b)
{
    s32 result = (u16)(a + b);

    if (result > 0x7fff) {
        result += -0x10000;
    }

    return result;
}

s32 NuAngSub(s32 a, s32 b)
{
    s32 result = (u16)(a - b);

    if (result > 0x7fff) {
        result += -0x10000;
    }

    return result;
}

s32 NuPower2(s32 value)
{
    s32 result = 1;

    while (result < value) {
        result *= 2;
    }

    return result;
}
