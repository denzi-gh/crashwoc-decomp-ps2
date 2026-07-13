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

#include "creature.h"

double atan2(double y, double x);
double sin(double x);

extern f32 NuTrigTable[];
extern u16 D_00293960[];

static inline u16 xy(s32 x, s32 y)
{
    u16 result;

    if ((u32)y < (u32)x) {
        result = D_00293960[(u64)(y * 0x200) / x];
        result = 0x4000 - result;
    } else {
        result = D_00293960[(u64)(x * 0x200) / y];
    }

    return result;
}

s32 NuAtani(s32 x, s32 y)
{
    s32 result = x;

    if (result == 0) {
        result = y < 0 ? 0x8000 : 0;
    } else if (y == 0) {
        result = result < 0 ? 0xc000 : 0x4000;
    } else if (result < 0) {
        if (y < 0) {
            result = xy(-result, -y) + 0x8000;
        } else {
            result = -xy(-result, y);
        }
    } else if (y < 0) {
        result = 0x8000 - xy(result, -y);
    } else {
        result = xy(result, y);
    }

    return result;
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
            s32 ax = -x;
            s32 ay = -y;
            u16 angle;

            if ((u32)ay < (u32)ax) {
                angle = 0x4000 - D_00293960[(u64)(ay * 0x200) / ax];
            } else {
                angle = D_00293960[(u64)(ax * 0x200) / ay];
            }
            return angle * 0.0000958738f + 3.1415927f;
        } else {
            s32 ax = -x;
            u16 angle;

            if ((u32)y < (u32)ax) {
                angle = 0x4000 - D_00293960[(u64)(y * 0x200) / ax];
            } else {
                angle = D_00293960[(u64)(ax * 0x200) / y];
            }
            return -(angle * 0.0000958738f);
        }
    }

    if (y < 0) {
        s32 ay = -y;
        u16 angle;

        if ((u32)ay < (u32)x) {
            angle = 0x4000 - D_00293960[(u64)(ay * 0x200) / x];
        } else {
            angle = D_00293960[(u64)(x * 0x200) / ay];
        }
        return 3.1415927f - angle * 0.0000958738f;
    } else {
        u16 angle;

        if ((u32)y < (u32)x) {
            angle = 0x4000 - D_00293960[(u64)(y * 0x200) / x];
        } else {
            angle = D_00293960[(u64)(x * 0x200) / y];
        }
        return angle * 0.0000958738f;
    }
}

static inline u16 fxyd(f32 x, f32 y)
{
    u16 result;

    if (x > y) {
        result = D_00293960[(s32)((y * 512.0f) / x)];
        result = 0x4000 - result;
    } else {
        result = D_00293960[(s32)((x * 512.0f) / y)];
    }

    return result;
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
