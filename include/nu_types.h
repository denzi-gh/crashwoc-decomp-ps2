#ifndef NU_TYPES_H
#define NU_TYPES_H

#ifdef NW_HOST

#include <stdint.h>

typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint64_t u64;
typedef int64_t s64;
typedef unsigned __int128 u128;
typedef uintptr_t nuintptr;
typedef intptr_t nusintptr;

#else

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef unsigned long long u64;
typedef signed long long s64;
typedef unsigned int u128 __attribute__((mode(TI)));
typedef unsigned int nuintptr;
typedef signed int nusintptr;

#endif

typedef float f32;
typedef double f64;

/* Compile-time size checks: a wrong width fails every build that includes
 * this header. They declare array types only and emit nothing. */
#define NU_STATIC_ASSERT(name, cond) typedef char nu_static_assert_##name[(cond) ? 1 : -1]

NU_STATIC_ASSERT(u8, sizeof(u8) == 1);
NU_STATIC_ASSERT(u16, sizeof(u16) == 2);
NU_STATIC_ASSERT(u32, sizeof(u32) == 4);
NU_STATIC_ASSERT(u64, sizeof(u64) == 8);
NU_STATIC_ASSERT(u128, sizeof(u128) == 16);
NU_STATIC_ASSERT(f32, sizeof(f32) == 4);
NU_STATIC_ASSERT(f64, sizeof(f64) == 8);
NU_STATIC_ASSERT(nuintptr, sizeof(nuintptr) == sizeof(void *));

#endif /* NU_TYPES_H */
