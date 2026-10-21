#ifndef NUMATH_NUTRIG_H
#define NUMATH_NUTRIG_H

#include "nu_types.h"

s32 NuAtani(s32 x, s32 y);
f32 NuAtanf(s32 x, s32 y);
s32 NuAtan2D(f32 x, f32 y);
void NuTrigInit(void);
f32 NuAtan2(f32 y, f32 x);
s32 NuAngAdd(s32 a, s32 b);
s32 NuAngSub(s32 a, s32 b);
s32 NuPower2(s32 value);

#endif /* NUMATH_NUTRIG_H */
