/*
 * Unit: game/game_deb
 *
 * Functions:
 *   0x0025fb08 AddAnimDebris
 *   0x00260ae8 InitGameDebris
 *   0x00260b60 AddGameDebris
 *   0x00260bb8 AddGameDebrisRot
 *   0x00260c20 AddGameDebrisMtx
 *   0x00260cd8 AddWarpDebris
 *   0x00260d50 AddMechanicalDebris
 */

#include "numath/numtx.h"
#include "numath/nuvec.h"
#include "game/gametypes.h"
#include "gamelib/terrain.h"
#include "game/creature.h"

struct adeb_s {
    short character;                    /* 0x00 */
    short action;                       /* 0x02 */
    float time;                         /* 0x04 */
    float time2;                        /* 0x08 */
    u32 flags;                          /* 0x0C */
    u16 locators;                       /* 0x10 */
    u8 gdeb;                            /* 0x12 */
    signed char count;                  /* 0x13 */
    u8 random;                          /* 0x14 */
    u8 xrot;                            /* 0x15 */
    signed char type;                   /* 0x16 */
    signed char target;                 /* 0x17 */
    short sfx;                          /* 0x18 */
    u8 rumble;                          /* 0x1A */
    u8 buzz;                            /* 0x1B */
    u64 levbits;                        /* 0x20 */
    float min;                          /* 0x28 */
    float max;                          /* 0x2C */
}; /* 0x30 */

struct gdeb_s GDeb[170] = {
    { -1, "DEBUG", -1 },
    { -1, "SNOW", -1 },
    { -1, "ICE", -1 },
    { -1, "DUST", -1 },
    { -1, "SAND", -1 },
    { -1, "CRATE_R", -1 },
    { -1, "CRATE_G", -1 },
    { -1, "CREAT_01", -1 },
    { -1, "NINJA_EXPLODE", -1 },
    { -1, "NINJA_SMOKE", -1 },
    { -1, "CRASH_SLIDE", -1 },
    { -1, "CRASH_SPIN", -1 },
    { -1, "CRASH_SPINDUST", -1 },
    { -1, "CRASH_SLAM", -1 },
    { -1, "CRASH_SLAMBITS", -1 },
    { -1, "CRASH_FOOTFALL", -1 },
    { -1, "CRASH_JUMP", -1 },
    { -1, "CRASH_JUMPBITS", -1 },
    { -1, "CRASH_BREATH", -1 },
    { -1, "CRASH_HVYBREATH", -1 },
    { -1, "CrashBubble", -1 },
    { -1, "Bubble1", -1 },
    { -1, "Bubble2", -1 },
    { -1, "EXPLOSION", -1 },
    { -1, "EXPLOSION2", -1 },
    { -1, "HIT", -1 },
    { -1, "SPLAT", -1 },
    { -1, "SPLAT2", -1 },
    { -1, "SMLEXPLOSION", -1 },
    { -1, "FLAME", -1 },
    { -1, "SNOW_LEV", -1 },
    { -1, "ANGELCRASH", -1 },
    { -1, "RockFlame", -1 },
    { -1, "RockImpact", -1 },
    { -1, "CoalSmoke", -1 },
    { -1, "CoalImpact", -1 },
    { -1, "CoalBreak", -1 },
    { -1, "MagicSpell", -1 },
    { -1, "RockSmoke", -1 },
    { -1, "FireDrop", -1 },
    { -1, "FireImpact", -1 },
    { -1, "FireSteam", -1 },
    { -1, "TreeBurn", -1 },
    { -1, "HutExplode", -1 },
    { -1, "Trail1", -1 },
    { -1, "Tornado1", -1 },
    { -1, "Leaves1", -1 },
    { -1, "Flame1", -1 },
    { -1, "FISHSPLASH", -1 },
    { -1, "FISHDRIP", -1 },
    { -1, "COCO_01", -1 },
    { -1, "COCO_02", -1 },
    { -1, "UFOFIRE", -1 },
    { -1, "WATER", -1 },
    { -1, "BIGGUN0", -1 },
    { -1, "BIGGUN1", -1 },
    { -1, "BIGGUN2", -1 },
    { -1, "BIGGUN3", -1 },
    { -1, "BIGGUN4", -1 },
    { -1, "BIGGUN5", -1 },
    { -1, "MUZZLE", -1 },
    { -1, "ROCK_SMOKE", -1 },
    { -1, "ROCK_SMOKE_B", -1 },
    { -1, "SMOKE", -1 },
    { -1, "ROCK_SMOKE5", -1 },
    { -1, "SPLASH", -1 },
    { -1, "EXPLOSION", -1 },
    { -1, "EXPLOSION2", -1 },
    { -1, "DYNAMITE", -1 },
    { -1, "POOP", -1 },
    { -1, "GREEN_SMOKE", -1 },
    { -1, "STATION", -1 },
    { -1, "GREEN_HIT", -1 },
    { -1, "RED_HIT", -1 },
    { -1, "GLOOP", -1 },
    { -1, "Potion", -1 },
    { -1, "ProbeGlow", -1 },
    { -1, "ProbeSpark", -1 },
    { -1, "ProbeSpark2", -1 },
    { -1, "DRAGON_BREATH", 0x20000 },
    { -1, "DRAGON_SMOKE", 0x20000 },
    { -1, "BAZOOKA_FIRE", -1 },
    { -1, "BAZOOKA_BITS", -1 },
    { -1, "SNWBRD_FRONT", 0x220 },
    { -1, "SNWBRD_BACK", 0x220 },
    { -1, "SNWBRD_SIDE", 0x220 },
    { -1, "LIGHTS_SPOT", 0x40 },
    { -1, "SPARKS", 0x10 },
    { -1, "CRASH_BAT", 0x20000 },
    { -1, "PENGUIN_SPIN", -1 },
    { -1, "JEEP_DUST", -1 },
    { -1, "SMOKE", -1 },
    { -1, "TubDust", -1 },
    { -1, "TubSpark", -1 },
    { -1, "LowFog", 0x800 },
    { -1, "Rain", -1 },
    { -1, "RainSplash", -1 },
    { -1, "RainDust", -1 },
    { -1, "FireFly", -1 },
    { -1, "GreenGas", -1 },
    { -1, "Flood1", 0x4000 },
    { -1, "Flood2", 0x4000 },
    { -1, "Flood3", 0x4000 },
    { -1, "Flood4", 0x4000 },
    { -1, "HeliSmoke", -1 },
    { -1, "DynaSmoke", 0x80000 },
    { -1, "Torn1", 0x2000 },
    { -1, "Torn2", 0x2000 },
    { -1, "MechWater", 0x400000 },
    { -1, "DroidSpark", 0x100000 },
    { -1, "Debris1", 0x2000 },
    { -1, "Debris2", 0x2000 },
    { -1, "Debris3", 0x2000 },
    { -1, "Stars1", 0x4000000 },
    { -1, "Blast1", -1 },
    { -1, "StarDust", -1 },
    { -1, "CSmoke1", 0x2000000 },
    { -1, "CSteam1", 0x2000000 },
    { -1, "CFire1", 0x2000000 },
    { -1, "CIce1", 0x2000000 },
    { -1, "CLaser1", 0x2000000 },
    { -1, "CFire2", 0x2000000 },
    { -1, "CDust", 0x2000000 },
    { -1, "CSnow", 0x2000000 },
    { -1, "CSpark", 0x2000000 },
    { -1, "CFire", 0x2000000 },
    { -1, "CMask1", -1 },
    { -1, "CMask2", -1 },
    { -1, "CMask3", -1 },
    { -1, "CMask4", -1 },
    { -1, "GetWumpa", -1 },
    { -1, "BIG_DUST", 0x100 },
    { -1, "ShipSmoke", 0x40000 },
    { -1, "ShipBubble", 0x40000 },
    { -1, "Hornet", 0x40000 },
    { -1, "FlameBall", -1 },
    { -1, "FlameBang", -1 },
    { -1, "FlameFire", -1 },
    { -1, "CAttack1", -1 },
    { -1, "CAttack2", -1 },
    { -1, "CAttack3", -1 },
    { -1, "CAttack4", -1 },
    { -1, "HitBubble", -1 },
    { -1, "Sparky", -1 },
    { -1, "NTropyZap", -1 },
    { -1, "WCrunch1", 0x1000000 },
    { -1, "WCrunch2", 0x1000000 },
    { -1, "WCrunch3", 0x1000000 },
    { -1, "WCrunch4", 0x1000000 },
    { -1, "FCrunch1", 0x400000 },
    { -1, "GreenBang", -1 },
    { -1, "SeaBits", -1 },
    { -1, "NarSplash", -1 },
    { -1, "LAND", 0x600000 },
    { -1, "DEATH", 0x200000 },
    { -1, "SuperMask", -1 },
    { -1, "GoldMask", -1 },
    { -1, "JeepDirt", -1 },
    { -1, "JeepBoost", -1 },
    { -1, "CocoFlame", 0x4000000 },
    { -1, "ZoffaTrail", 0x4000000 },
    { -1, "SPANG", 0x2000000000 },
    { -1, "FCrunch2", 0x400000 },
    { -1, "CMask5", -1 },
    { -1, "Hub1", 0x2000000000 },
    { -1, "Hub2", 0x2000000000 },
    { -1, "Hub3", 0x2000000000 },
    { -1, "Hub4", 0x2000000000 },
    { -1, "Hub5", 0x2000000000 },
    { -1, "Hub6", 0x2000000000 },
};

struct adeb_s ADeb[112] = {
    { 97, 58, 1.0f, 1.0f, 0x202004, 0x0, 255, 1, 1, 0, -1, -1, 133, 0, 0,
      -1, 0.0f, 1882.0f },
    { 97, 58, 1.0f, 1.0f, 0x302000, 0x0, 255, 1, 0, 0, -1, -1, 31, 0, 0,
      -1, 16383.0f, 256.0f },
    { 97, 58, 7.0f, 1.0f, 0x302000, 0x0, 255, 1, 0, 0, -1, -1, 32, 0, 0,
      -1, 16383.0f, 256.0f },
    { 0, 58, 10.0f, 1.0f, 0x2020, 0x2, 15, 1, 0, 0, -1, -1, 1024, 0, 0,
      -1, 0.0f, 0.07f },
    { 0, 58, 22.0f, 1.0f, 0x2040, 0x1, 15, 1, 0, 0, -1, -1, 1025, 0, 0,
      -1, 0.0f, 0.07f },
    { 0, 116, 10.0f, 1.0f, 0x2000, 0x2, 255, 1, 0, 0, -1, -1, 1024, 0, 0,
      -1, 0.0f, 0.0f },
    { 0, 116, 25.0f, 1.0f, 0x2000, 0x1, 255, 1, 0, 0, -1, -1, 1025, 0, 0,
      -1, 0.0f, 0.0f },
    { 0, 72, 16.0f, 1.0f, 0x2020, 0x2, 15, 1, 0, 0, -1, -1, 1024, 0, 0,
      -1, 0.0f, 0.0f },
    { 0, 72, 8.0f, 1.0f, 0x2040, 0x1, 15, 1, 0, 0, -1, -1, 1025, 0, 0,
      -1, 0.0f, 0.0f },
    { 0, 83, 30.0f, 1.0f, 0x2000, 0x2, 255, 1, 0, 0, -1, -1, 141, 0, 0,
      -1, 0.0f, 0.0f },
    { 0, 83, 6.0f, 1.0f, 0x2000, 0x1, 255, 1, 0, 0, -1, -1, 142, 0, 0,
      -1, 0.0f, 0.0f },
    { 0, 77, 1.0f, 1.0f, 0x2000, 0x0, 255, 1, 0, 0, -1, -1, 27, 0, 0,
      -1, 0.0f, 0.0f },
    { 0, 77, 16.0f, 1.0f, 0x2000, 0x1, 255, 1, 0, 0, -1, -1, 28, 0, 0,
      -1, 0.0f, 0.0f },
    { 0, 63, 13.0f, 1.0f, 0x2000, 0x2, 255, 1, 0, 0, -1, -1, 10, 0, 0,
      -1, 0.0f, 0.0f },
    { 0, 63, 30.0f, 1.0f, 0x2000, 0x1, 255, 1, 0, 0, -1, -1, 11, 0, 0,
      -1, 0.0f, 0.0f },
    { 0, 64, 1.0f, 1.0f, 0x2000, 0x2, 255, 1, 0, 0, -1, -1, 12, 0, 0,
      -1, 0.0f, 0.0f },
    { 0, 64, 9.0f, 1.0f, 0x2000, 0x1, 255, 1, 0, 0, -1, -1, 13, 0, 0,
      -1, 0.0f, 0.0f },
    { 0, 34, 13.0f, 1.0f, 0x83, 0x4, 18, 9, 0, 64, -1, -1, -1, 0, 0,
      0x200000A1, 0.0f, 0.0f },
    { 0, 2, 22.0f, 1.0f, 0x83, 0x4, 18, 9, 0, 64, -1, -1, -1, 0, 0,
      0x200000A1, 0.0f, 0.0f },
    { 0, 4, 1.0f, 1.0f, 0x83, 0x4, 18, 9, 0, 64, -1, -1, -1, 0, 0,
      0x200000A1, 0.0f, 0.0f },
    { 0, 41, 15.0f, 28.0f, 0x8B, 0x4, 19, 1, 0, 81, -1, -1, -1, 0, 0,
      0x200000A1, 0.0f, 0.0f },
    { 0, 67, 5.0f, 1.0f, 0xB, 0x1, 10, -3, 0, 32, -1, -1, -1, 0, 0,
      -1, 0.0f, 0.0f },
    { 0, 29, 1.0f, 1.0f, 0x5, 0x40, 31, 1, 0, 0, -1, -1, -1, 0, 0,
      -1, 0.0f, 0.0f },
    { 1, 29, 1.0f, 1.0f, 0x5, 0x0, 31, 1, 0, 0, -1, -1, -1, 0, 0,
      -1, 0.0f, 0.0f },
    { 0, 114, 43.0f, 1.0f, 0x2000, 0x0, 255, 0, 0, 0, -1, -1, 62, 0, 0,
      0x200000A1, 0.0f, 0.0f },
    { 0, 114, 22.0f, 1.0f, 0x2000, 0x0, 255, 0, 0, 0, -1, -1, 63, 0, 0,
      0x200000A1, 0.0f, 0.0f },
    { 115, 76, 10.0f, 1.0f, 0x2000, 0x0, 255, 1, 0, 0, -1, -1, 41, 0, 0,
      -1, 0.0f, 0.0f },
    { 115, 76, 22.0f, 1.0f, 0x2000, 0x0, 255, 1, 0, 0, -1, -1, 41, 0, 0,
      -1, 0.0f, 0.0f },
    { 115, 76, 1.0f, 1.0f, 0x5, 0x1, 20, -10, 128, 0, -1, -1, -1, 0, 0,
      -1, 0.0f, 0.0f },
    { 115, 34, 1.0f, 1.0f, 0x5, 0x1, 20, -10, 128, 0, -1, -1, -1, 0, 0,
      -1, 0.0f, 0.0f },
    { 1, 72, 9.0f, 1.0f, 0x2020, 0x1, 15, 1, 0, 0, -1, -1, 1024, 0, 0,
      -1, 0.0f, 0.0f },
    { 1, 72, 21.0f, 1.0f, 0x2040, 0x2, 15, 1, 0, 0, -1, -1, 1025, 0, 0,
      -1, 0.0f, 0.0f },
    { 1, 58, 9.0f, 1.0f, 0x2020, 0x1, 15, 1, 0, 0, -1, -1, 1024, 0, 0,
      -1, 0.0f, 0.0f },
    { 1, 58, 21.0f, 1.0f, 0x2040, 0x2, 15, 1, 0, 0, -1, -1, 1025, 0, 0,
      -1, 0.0f, 0.0f },
    { 1, 116, 10.0f, 1.0f, 0x2020, 0x1, 15, 1, 0, 0, -1, -1, 1024, 0, 0,
      -1, 0.0f, 0.0f },
    { 1, 116, 26.0f, 1.0f, 0x2040, 0x2, 15, 1, 0, 0, -1, -1, 1025, 0, 0,
      -1, 0.0f, 0.0f },
    { 1, 64, 5.0f, 1.0f, 0x2020, 0x1, 15, 1, 0, 0, -1, -1, 12, 0, 0,
      -1, 0.0f, 0.0f },
    { 1, 64, 14.0f, 1.0f, 0x2040, 0x2, 15, 1, 0, 0, -1, -1, 13, 0, 0,
      -1, 0.0f, 0.0f },
    { 62, 29, 1.0f, 1.0f, 0x102000, 0x1, 255, 1, 0, 0, -1, -1, 83, 0, 0,
      -1, 16383.0f, 0.0f },
    { 62, 29, 5.0f, 1.0f, 0x502000, 0x1, 255, 1, 0, 0, -1, -1, 82, 0, 0,
      -1, 131064.0f, 0.06125f },
    { 57, 50, 7.0f, 1.0f, 0x102000, 0x1, 255, 1, 0, 0, -1, -1, 141, 0, 0,
      -1, 16383.0f, 0.0f },
    { 57, 50, 1.0f, 1.0f, 0x102000, 0x1, 255, 1, 0, 0, -1, -1, 142, 0, 0,
      -1, 16383.0f, 0.0f },
    { 57, 57, 7.0f, 1.0f, 0x102000, 0x1, 255, 1, 0, 0, -1, -1, 141, 0, 0,
      -1, 16383.0f, 0.0f },
    { 57, 57, 1.0f, 1.0f, 0x102000, 0x1, 255, 1, 0, 0, -1, -1, 142, 0, 0,
      -1, 16383.0f, 0.0f },
    { 55, 116, 1.0f, 1.0f, 0x102000, 0x1, 255, 1, 0, 0, -1, 1, 129, 0, 0,
      -1, 32766.0f, 0.0f },
    { 70, 56, 1.0f, 1.0f, 0x102000, 0x0, 255, 1, 0, 0, -1, -1, 111, 0, 0,
      -1, 32766.0f, 0.0f },
    { 12, 87, 1.0f, 1.0f, 0x102000, 0x0, 255, 1, 0, 0, -1, -1, 31, 0, 0,
      -1, 16383.0f, 0.0f },
    { 12, 87, 25.0f, 1.0f, 0x102000, 0x0, 255, 1, 0, 0, -1, -1, 32, 0, 0,
      -1, 16383.0f, 0.0f },
    { 12, 116, 1.0f, 1.0f, 0x102000, 0x0, 255, 1, 0, 0, -1, -1, 31, 0, 0,
      -1, 16383.0f, 0.0f },
    { 12, 116, 9.0f, 1.0f, 0x102000, 0x0, 255, 1, 0, 0, -1, -1, 32, 0, 0,
      -1, 16383.0f, 0.0f },
    { 66, 58, 13.0f, 1.0f, 0x12000, 0x2, 16, 1, 0, 0, -1, -1, 204, 0, 0,
      -1, 0.0f, 0.15f },
    { 66, 58, 24.0f, 1.0f, 0x12000, 0xC, 16, 1, 0, 0, -1, -1, 204, 0, 0,
      -1, 0.0f, 0.15f },
    { 66, 58, 35.0f, 1.0f, 0x12000, 0x3, 16, 1, 0, 0, -1, -1, 204, 0, 0,
      -1, 0.0f, 0.15f },
    { 66, 58, 1.0f, 1.0f, 0x12000, 0xC, 16, 1, 0, 0, -1, -1, 204, 0, 0,
      -1, 0.0f, 0.15f },
    { 66, 117, 2.0f, 1.0f, 0x2000, 0x1, 255, 1, 0, 0, -1, -1, 203, 0, 0,
      -1, 0.0f, 0.0f },
    { 66, 51, 30.0f, 1.0f, 0x2000, 0x1, 255, 1, 0, 0, -1, -1, 205, 0, 0,
      -1, 0.0f, 0.0f },
    { 66, 64, 4.0f, 1.0f, 0x2000, 0x0, 255, 1, 0, 0, -1, -1, 18, 0, 0,
      -1, 0.0f, 0.0f },
    { 66, 64, 16.0f, 1.0f, 0x2000, 0x0, 255, 1, 0, 0, -1, -1, 18, 0, 0,
      -1, 0.0f, 0.0f },
    { 64, 0, 17.0f, 1.0f, 0x302000, 0x1, 255, 1, 0, 0, -1, -1, 28, 0, 0,
      -1, 49149.0f, 2823.0f },
    { 60, 0, 9.0f, 1.0f, 0x2000, 0x1, 48, 1, 0, 0, -1, -1, 143, 0, 0,
      -1, 0.0f, 0.0f },
    { 60, 0, 39.0f, 1.0f, 0x2000, 0x1, 48, 1, 0, 0, -1, -1, 143, 0, 0,
      -1, 0.0f, 0.0f },
    { 60, 0, 1.0f, 1.0f, 0x5, 0x1, 49, 1, 48, 0, -1, -1, -1, 0, 0,
      -1, 0.0f, 0.0f },
    { 24, 0, 9.0f, 1.0f, 0x302000, 0x2, 255, 1, 0, 0, -1, -1, 28, 0, 0,
      -1, 49149.0f, 2823.0f },
    { 24, 0, 29.0f, 1.0f, 0x302000, 0x2, 255, 1, 0, 0, -1, -1, 28, 0, 0,
      -1, 49149.0f, 2823.0f },
    { 24, 0, 37.0f, 1.0f, 0x302000, 0x1, 255, 1, 0, 0, -1, -1, 28, 0, 0,
      -1, 49149.0f, 2823.0f },
    { 80, 0, 1.0f, 1.0f, 0x100804, 0x1, 255, 1, 64, 0, 16, -1, 75, 0, 0,
      -1, 32766.0f, 0.0f },
    { 80, 27, 17.0f, 1.0f, 0x100800, 0x1, 255, 1, 0, 0, 6, 1, 75, 0, 0,
      -1, 32766.0f, 0.0f },
    { 80, 27, 32.0f, 1.0f, 0x100800, 0x1, 255, 1, 0, 0, 6, 2, 75, 0, 0,
      -1, 32766.0f, 0.0f },
    { 80, 27, 47.0f, 1.0f, 0x100800, 0x1, 255, 1, 0, 0, 6, 3, 75, 0, 0,
      -1, 32766.0f, 0.0f },
    { 155, 27, 1.0f, 1.0f, 0x800, 0x1, 255, 1, 0, 0, 5, -1, -1, 0, 0,
      -1, 0.0f, 0.0f },
    { 155, 34, 1.0f, 1.0f, 0x5, 0x2, 20, -5, 128, 0, -1, -1, -1, 0, 0,
      -1, 0.0f, 0.0f },
    { 126, 34, 1.0f, 1.0f, 0x5, 0x2, 20, -5, 128, 0, -1, -1, -1, 0, 0,
      -1, 0.0f, 0.0f },
    { 169, 82, 14.0f, 1.0f, 0x102800, 0x1, 255, 1, 0, 0, 8, 1, 131, 0, 0,
      -1, 8191.0f, 0.0f },
    { 170, 82, 14.0f, 1.0f, 0x102800, 0x1, 255, 1, 0, 0, 8, 1, 131, 0, 0,
      -1, 8191.0f, 0.0f },
    { 171, 34, 1.0f, 1.0f, 0x102804, 0x1, 255, -3, 64, 0, 17, -1, 75, 0, 0,
      -1, 32766.0f, 0.0f },
    { 172, 34, 72.0f, 1.0f, 0x102800, 0x1, 255, 1, 0, 0, 11, 1, 75, 0, 0,
      -1, 65532.0f, 0.0f },
    { 127, 88, 1.0f, 45.0f, 0x4009, 0x3, 140, 1, 0, 0, -1, -1, -1, 0, 0,
      0x800000, 0.0f, 0.0f },
    { 127, 115, 20.0f, 0.0f, 0x0, 0x0, 255, 0, 0, 0, -1, -1, 34, 0, 0,
      0x2000000, 0.0f, 0.0f },
    { 127, 55, 27.0f, 1.0f, 0x0, 0x3, 13, 1, 0, 0, -1, -1, 59, 0, 0,
      0x800000, 0.0f, 0.0f },
    { 127, 0, 35.0f, 1.0f, 0x2000, 0x2, 130, 1, 0, 0, -1, -1, 64, 0, 0,
      0x2000000, 0.0f, 0.0f },
    { 66, 117, 19.0f, 70.0f, 0xB, 0x40, 79, 1, 0, 64, -1, -1, -1, 0, 0,
      -1, 0.0f, 0.0f },
    { 12, 70, 15.0f, 1.0f, 0x8009, 0x0, 89, 1, 0, 0, -1, -1, -1, 0, 0,
      -1, 0.0f, 0.0f },
    { 0, 36, 30.0f, 1.0f, 0x2000, 0x0, 255, 1, 0, 0, -1, -1, 15, 0, 0,
      -1, 0.0f, 0.0f },
    { 0, 35, 25.0f, 1.0f, 0x2000, 0x0, 255, 1, 0, 0, -1, -1, 15, 0, 0,
      -1, 0.0f, 0.0f },
    { 138, 82, 17.0f, 1.0f, 0x2800, 0x1, 255, 1, 0, 0, 9, 1, 131, 0, 0,
      -1, 8191.0f, 0.0f },
    { 6, 0, 10.0f, 1.0f, 0x102800, 0x1, 255, 1, 0, 0, 15, 1, 75, 0, 0,
      -1, 32766.0f, 0.0f },
    { 108, 34, 1.0f, 1.0f, 0x2000, 0x0, 255, 1, 0, 0, -1, 1, 143, 0, 0,
      -1, 0.0f, 0.0f },
    { 0, 37, 143.0f, 1.0f, 0x2000, 0x0, 255, 1, 0, 0, -1, 1, 64, 0, 0,
      -1, 0.0f, 0.0f },
    { 68, 113, 29.0f, 1.0f, 0x2000, 0x0, 255, 1, 0, 0, -1, 1, 116, 0, 3,
      -1, 0.0f, 0.0f },
    { 68, 113, 13.0f, 1.0f, 0x2000, 0x0, 255, 1, 0, 0, -1, 1, 116, 0, 3,
      -1, 0.0f, 0.0f },
    { 68, 104, 17.0f, 1.0f, 0x2000, 0x0, 255, 1, 0, 0, -1, 1, 116, 0, 5,
      -1, 0.0f, 0.0f },
    { 68, 104, 36.0f, 1.0f, 0x2000, 0x0, 255, 1, 0, 0, -1, 1, 116, 0, 5,
      -1, 0.0f, 0.0f },
    { 178, 104, 17.0f, 1.0f, 0x2000, 0x0, 255, 1, 0, 0, -1, 1, 116, 0, 5,
      -1, 0.0f, 0.0f },
    { 178, 104, 36.0f, 1.0f, 0x2000, 0x0, 255, 1, 0, 0, -1, 1, 116, 0, 5,
      -1, 0.0f, 0.0f },
    { 15, 29, 1.0f, 1.0f, 0x2000, 0x0, 255, 1, 0, 0, -1, 1, 126, 0, 0,
      -1, 0.0f, 0.0f },
    { 56, 44, 23.0f, 1.0f, 0x102000, 0x1, 152, 1, 0, 0, -1, 1, 143, 0, 0,
      -1, 196596.0f, 0.0f },
    { 56, 44, 1.0f, 1.0f, 0x102000, 0x1, 152, 1, 0, 0, -1, 1, 143, 0, 0,
      -1, 65532.0f, 0.0f },
    { 152, 58, 1.0f, 1.0f, 0x102000, 0x1, 152, 1, 0, 0, -1, 1, 47, 0, 0,
      -1, 16383.0f, 0.0f },
    { 154, 34, 1.0f, 1.0f, 0x40005, 0x0, 20, 1, 64, 0, -1, 1, -1, 0, 0,
      -1, 0.01f, 0.0f },
    { 61, 34, 49.0f, 1.0f, 0x2000, 0x0, 255, 1, 0, 0, -1, -1, 122, 0, 0,
      -1, 0.0f, 0.0f },
    { 61, 34, 52.0f, 1.0f, 0x2000, 0x0, 255, 1, 0, 0, -1, -1, 122, 0, 0,
      -1, 0.0f, 0.0f },
    { 61, 0, 49.0f, 1.0f, 0x2000, 0x0, 255, 1, 0, 0, -1, -1, 122, 0, 0,
      -1, 0.0f, 0.0f },
    { 61, 0, 52.0f, 1.0f, 0x2000, 0x0, 255, 1, 0, 0, -1, -1, 122, 0, 0,
      -1, 0.0f, 0.0f },
    { 106, 58, 1.0f, 1.0f, 0x102000, 0x0, 255, 1, 0, 0, -1, -1, 203, 0, 0,
      -1, 32766.0f, 0.0f },
    { 109, 58, 1.0f, 1.0f, 0x102000, 0x0, 255, 1, 187, 0, -1, -1, 200, 0, 0,
      -1, 32766.0f, 0.0f },
    { 109, 58, 1.0f, 1.0f, 0x102000, 0x0, 255, 1, 0, 0, -1, -1, 201, 0, 0,
      -1, 65532.0f, 0.0f },
    { 113, 0, 1.0f, 1.0f, 0x2000, 0x0, 255, 1, 0, 0, -1, -1, 155, 0, 0,
      -1, 0.0f, 0.0f },
    { 113, 58, 1.0f, 1.0f, 0x2000, 0x0, 255, 1, 0, 0, -1, -1, 154, 0, 0,
      -1, 0.0f, 0.0f },
    { 114, 0, 1.0f, 1.0f, 0x2000, 0x0, 255, 1, 0, 0, -1, -1, 128, 0, 0,
      -1, 0.0f, 0.0f },
    { 114, 34, 1.0f, 1.0f, 0x2000, 0x0, 255, 1, 0, 0, -1, -1, 154, 0, 0,
      -1, 0.0f, 0.0f },
    { 110, 0, 5.0f, 1.0f, 0x102000, 0x0, 255, 1, 0, 0, -1, -1, 156, 0, 0,
      -1, 32766.0f, 0.0f },
    { -1, 0, 0.0f, 0.0f, 0x0, 0x0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0x0, 0.0f, 0.0f },
};

extern s32 NODEBRIS;
extern s32 Level;
extern s32 Paused;
extern s32 GemPath;
extern s32 Death;
extern s32 Bonus;
extern s32 jonframe1;
extern s32 SFXCOUNT_ALL;
extern s32 gamesfx_effect_volume;
extern s32 gamesfx_pitch;
extern f32 EShadY;
extern struct nuvec_s ShadNorm;
extern struct nuvec_s v000;
extern struct cammtx_s GameCam[];
extern struct aitab_s AITab[];

extern s32 LookupDebrisEffect(char *name);
extern void AddFiniteShotDebrisEffect(s32 *key, s32 effect, struct nuvec_s *pos,
                                      s32 n);
extern void AddVariableShotDebrisEffect(s32 effect, struct nuvec_s *pos, s32 n,
                                        short xrot, short yrot);
extern void AddVariableShotDebrisEffectMtx2(s32 effect, struct nuvec_s *pos,
                                            s32 n, struct numtx_s *m1,
                                            struct numtx_s *m2);

extern void AddDeb3(struct nuvec_s *pos, s32 effect, s32 n, void *arg);
extern s32 qrand(void);
extern f32 frand(void);
extern void JudderGameCamera(struct cammtx_s *cam, f32 time, struct nuvec_s *pos);
extern void GameSfx(s32 id, struct nuvec_s *pos);
extern void AddKaboom(s32 type, struct nuvec_s *pos, f32 radius);
extern void AddProjectile(struct nuvec_s *pos, struct nuvec_s *target,
                          struct nuvec_s *mom, s32 type, u16 hdg,
                          struct obj_s *owner);
extern void NuRndrAddFootPrint(u16 hdg, f32 sizex, f32 sizez, s32 depth,
                               struct nuvec_s *pos, struct nuvec_s *norm,
                               s32 type, s32 colour);

extern struct numtx_s mTEMP;
extern struct numtx_s numtx_identity;

s32 mechbits[7] = { 21, 22, 23, 21, 22, 23, -1 };
s32 atlasbits[7] = { 18, 19, 20, 18, 19, 20, -1 };
s32 helibits[7] = { 24, 25, 26, 24, 25, 26, -1 };
s32 jeepbits[8] = { 11, 11, 12, 13, 11, 12, 13, -1 };
s32 ttty = 0;


inline void AddGameDebris(s32 i, struct nuvec_s *pos) {
    s32 key;

    if (NODEBRIS == 0 && (u32)i < 0xaa && GDeb[i].i != -1) {
        key = -1;
        AddFiniteShotDebrisEffect(&key, GDeb[i].i, pos, 1);
    }
}

inline void AddGameDebrisRot(s32 i, struct nuvec_s *pos, s32 n, u16 xrot,
                             u16 yrot) {
    if (NODEBRIS == 0 && (u32)i < 0xaa && GDeb[i].i != -1) {
        AddVariableShotDebrisEffect(GDeb[i].i, pos, n, xrot, yrot + 0x4000);
    }
}

inline void AddGameDebrisMtx(s32 i, struct nuvec_s *pos, s32 n,
                             struct numtx_s *m) {
    if (NODEBRIS == 0 && (u32)i < 0xaa && GDeb[i].i != -1) {
        NuMtxSetRotationX(&mTEMP, 0x4000);
        NuMtxMulR(&mTEMP, &mTEMP, m);
        AddVariableShotDebrisEffectMtx2(GDeb[i].i, pos, n, &mTEMP,
                                        &numtx_identity);
    }
}

void AddAnimDebris(struct CharacterModel *model, struct numtx_s *mtxLOCATOR,
                   s32 action, float time, struct obj_s *obj) {
    struct adeb_s *adeb;
    struct creature_s *c;
    struct nuvec_s *p;
    u16 xrot;
    u16 yrot;
    float y;
    float sizex;
    float sizez;
    s32 n;
    s32 i;
    s32 j;
    s32 count;
    s32 sfx;
    s32 inf;
    s32 gindex;
    s32 depth;
    s32 sound;
    s32 list[16];
    struct numtx_s *mtx[16];
    struct nuvec_s pos;
    struct nuvec_s v[16];

    if (Paused != 0) {
        return;
    }
    for (adeb = ADeb; adeb->character != -1; adeb++) {
        if (model->character != adeb->character || action != adeb->action
            || ((adeb->levbits >> Level) & 1) == 0) {
            continue;
        }
        if (obj != 0) {
            if ((adeb->flags & 0x10) != 0 && (obj->ground & 2) == 0) {
                continue;
            }
            if ((adeb->flags & 0x80) != 0 && obj->submerged != 0) {
                continue;
            }
        }
        if (!((adeb->flags & 4) != 0
              || ((adeb->flags & 8) != 0 && time >= adeb->time
                  && (adeb->time2 <= 1.0f
                      || (adeb->time2 > adeb->time && time <= adeb->time2)))
              || (time > adeb->time
                  && time - model->animlist[action]->speed * 0.59999996f
                         <= adeb->time))) {
            continue;
        }
        if (adeb->character == 0x9A) {
            pos = v000;
        }
        pos = v000;
        count = 0;
        for (j = 0; j < 16; j++) {
            if (((adeb->locators >> j) & 1) != 0 && model->pLOCATOR[j] != 0) {
                list[count] = j;
                v[count] = *(struct nuvec_s *)&mtxLOCATOR[j]._30;
                mtx[count] = &mtxLOCATOR[j];
                if (count == 0 || (adeb->flags & 0x4000) != 0) {
                    NuVecAdd(&pos, &pos, (struct nuvec_s *)&mtxLOCATOR[j]._30);
                }
                count++;
            }
        }
        if (count > 0) {
            NuVecScale(&pos, &pos, 1.0f / (float)count);
        }
        if ((adeb->flags & 0x8000) != 0 && obj != 0) {
            count = 1;
            pos.x = obj->pos.x;
            pos.y = obj->pos.y + obj->bot * obj->SCALE;
            pos.z = obj->pos.z;
        }
        if (count == 0 && obj != 0) {
            v[count] = obj->pos;
            pos = v[count];
            count = 1;
        }
        if ((adeb->flags & 0x10000) != 0) {
            JudderGameCamera(GameCam, adeb->max, 0);
        }
        if (count != 0 && (adeb->flags & 0x80) != 0) {
            if (obj == 0) {
                goto debris;
            }
            if (obj->layer_type != -1 && pos.y < obj->layer_shadow) {
                continue;
            }
        }
        if (obj == 0
            || !(((obj->flags & 4) != 0 && obj->dead == 1)
                 || ((adeb->flags & 0x100) != 0 && obj->xz_distance < adeb->min)
                 || ((adeb->flags & 0x200) != 0 && obj->xz_distance > adeb->max)
                 || ((adeb->flags & 0x40000) != 0
                     && obj->xyz_distance < adeb->min)
                 || ((adeb->flags & 0x80000) != 0
                     && obj->xyz_distance > adeb->max)
                 || ((obj->flags & 1) != 0
                     && (GemPath == 1 || GemPath == 3 || Death == 1
                         || Death == 3 || Bonus == 1 || Bonus == 3)))) {
debris:
            if (count != 0 && adeb->gdeb < 0xAA) {
                if ((adeb->flags & 1) != 0) {
                    n = adeb->count;
                    if (n < 0) {
                        n = (jonframe1 % -n) == 0;
                    }
                    if (n != 0
                        && (adeb->random == 0
                            || qrand() <= adeb->random << 8)) {
                        if ((adeb->flags & 0x8000) != 0) {
                            AddGameDebrisRot(adeb->gdeb, &pos, n, 0, 0);
                        } else if ((adeb->flags & 0x400) != 0) {
                            if ((adeb->flags & 0x4000) != 0) {
                                AddGameDebrisMtx(adeb->gdeb, &pos, n, mtx[0]);
                            } else {
                                for (i = 0; i < count; i++) {
                                    AddGameDebrisMtx(adeb->gdeb, &v[i], n,
                                                     mtx[i]);
                                }
                            }
                        } else {
                            if ((adeb->flags & 2) != 0) {
                                xrot = adeb->xrot << 8;
                                if (obj == 0) {
                                    yrot = 0;
                                } else {
                                    yrot = obj->hdg;
                                }
                            } else {
                                xrot = 0;
                                yrot = 0;
                            }
                            if ((adeb->flags & 0x4000) != 0) {
                                AddGameDebrisRot(adeb->gdeb, &pos, n, xrot,
                                                 yrot);
                            } else {
                                for (i = 0; i < count; i++) {
                                    AddGameDebrisRot(adeb->gdeb, &v[i], n,
                                                     xrot, yrot);
                                }
                            }
                        }
                    }
                } else if ((adeb->flags & 0x8000) != 0) {
                    AddGameDebris(adeb->gdeb, &pos);
                } else if ((adeb->flags & 0x4000) != 0) {
                    AddGameDebris(adeb->gdeb, &pos);
                } else {
                    for (i = 0; i < count; i++) {
                        AddGameDebris(adeb->gdeb, &v[i]);
                    }
                }
            }
            if (adeb->rumble != 0) {
                NewRumble(&player->rumble, adeb->rumble);
            }
            if (adeb->buzz != 0) {
                NewBuzz(&player->rumble, adeb->buzz);
            }
        }
        if (((adeb->sfx >= 0 && adeb->sfx < SFXCOUNT_ALL)
             || adeb->sfx == 0x400 || adeb->sfx == 0x401)
            && (adeb->random == 0 || qrand() <= adeb->random << 8)) {
            sfx = adeb->sfx;
            if (sfx == 0x400) {
                sfx = 0x8D;
                if (obj != 0) {
                    if (obj->layer_type == 1) {
                        sfx = 0x2D;
                    } else {
                        switch (obj->surface_type) {
                        case 1:
                        case 5:
                        case 6:
                        case 13:
                        case 14:
                        case 15:
                            sfx = 0x1F;
                            break;
                        }
                    }
                }
            } else if (sfx == 0x401) {
                sfx = 0x8E;
                if (obj != 0) {
                    if (obj->layer_type == 1) {
                        sfx = 0x2E;
                    } else {
                        switch (obj->surface_type) {
                        case 1:
                        case 5:
                        case 6:
                        case 13:
                        case 14:
                        case 15:
                            sfx = 0x20;
                            break;
                        }
                    }
                }
            }
            sound = 1;
            if ((adeb->flags & 0x400000) != 0 && frand() > adeb->max) {
                sound = 0;
            }
            if (sound) {
                if ((adeb->flags & 0x100000) != 0) {
                    gamesfx_effect_volume = (s32)adeb->min;
                }
                if ((adeb->flags & 0x200000) != 0) {
                    gamesfx_pitch = (s32)adeb->max;
                }
                GameSfx(sfx, (count == 0 || (adeb->flags & 0x2000) == 0)
                                 ? 0 : &pos);
            }
        }
        if (count != 0 && (adeb->flags & 0x800) != 0 && adeb->type != -1
            && (obj == 0 || obj->dead == 0)
            && (adeb->random == 0 || qrand() <= adeb->random << 8)) {
            n = adeb->count;
            if (n < 0) {
                n = (jonframe1 % -n) == 0;
            }
            if (n != 0) {
                c = 0;
                if (obj != 0) {
                    c = obj->parent;
                }
                AddProjectile(&pos,
                              (adeb->target != -1 && c != 0
                               && c->i_aitab != -1)
                                  ? &AITab[c->i_aitab].pos[adeb->target] : 0,
                              0, adeb->type,
                              (obj != 0) ? obj->hdg : (u16)qrand(), obj);
            }
        }
        if ((adeb->flags & 0x1000) != 0 && (obj == 0 || obj->dead == 0)) {
            p = 0;
            if (adeb->target != -1) {
                if (obj != 0) {
                    p = &AITab[((struct creature_s *)obj->parent)->i_aitab]
                             .pos[adeb->target];
                }
            } else if (count != 0) {
                p = &pos;
            } else if (obj != 0) {
                p = &obj->pos;
            }
            if (p != 0) {
                AddKaboom(0x10, p, !(adeb->max > 0.0f) ? 1.0f : adeb->max);
            }
        }
        if (obj == 0 || (adeb->flags & 0x60) == 0 || (obj->ground & 2) == 0) {
            continue;
        }
        y = NewShadowMask(&pos, 0.0f, -1);
        if (y == 2000000.0f) {
            continue;
        }
        inf = ShadowInfo();
        gindex = -1;
        if (EShadY != 2000000.0f && EShadowInfo() == 4) {
            y = EShadY;
            depth = 0x78;
            obj->ddsnow = depth;
            gindex = 4;
        }
        switch (inf) {
        case 2:
            if (gindex == -1) {
                depth = 0x78;
                obj->ddr = 0x60;
                obj->ddg = 0x70;
                obj->ddb = -0x40;
                gindex = 4;
                obj->ddsnow = depth;
            }
            break;
        case 8:
            depth = 0x78;
            obj->ddr = -0x80;
            obj->ddg = 0x50;
            obj->ddb = 0x20;
            gindex = 4;
            obj->ddsand = depth;
            break;
        default:
            if (obj->ddsnow > 0 && gindex == -1) {
                obj->ddr = 0x60;
                depth = obj->ddsnow;
                obj->ddg = 0x70;
                obj->ddb = -0x40;
                gindex = 6;
                obj->ddsnow -= 2;
            }
            if (obj->ddsand > 0 && gindex == -1) {
                obj->ddr = -0x80;
                depth = obj->ddsand;
                obj->ddg = 0x50;
                gindex = 0;
                obj->ddb = 0x20;
                obj->ddsand -= 2;
            }
            if (obj->ddwater > 0) {
                if (gindex != -1) {
                    break;
                }
                if (obj->ddwater < 0x6E) {
                    depth = obj->ddwater;
                    obj->ddr = 0x30;
                    obj->ddg = 0x30;
                    obj->ddb = 0x30;
                    gindex = 2;
                }
                obj->ddwater -= 2;
            }
            break;
        }
        if (gindex == -1) {
            continue;
        }
        if (model->character == 1) {
            sizex = 0.07f;
            sizez = 0.12f;
        } else {
            sizex = 0.1f;
            sizez = 0.2f;
        }
        pos.y = y + 0.02f;
        if ((adeb->flags & 0x20) != 0) {
            if ((short)obj->hdg >= 0) {
                NuRndrAddFootPrint(obj->hdg, sizex, sizez, depth, &pos,
                                   &ShadNorm, gindex + 1,
                                   obj->ddb * 0x10000 + obj->ddg * 0x100
                                       + obj->ddr);
            } else {
                NuRndrAddFootPrint(obj->hdg, -sizex, sizez, depth, &pos,
                                   &ShadNorm, gindex,
                                   obj->ddb * 0x10000 + obj->ddg * 0x100
                                       + obj->ddr);
            }
        } else {
            if ((short)obj->hdg >= 0) {
                NuRndrAddFootPrint(obj->hdg, sizex, sizez, 0x30, &pos,
                                   &ShadNorm, gindex,
                                   obj->ddb * 0x10000 + obj->ddg * 0x100
                                       + obj->ddr);
            } else {
                NuRndrAddFootPrint(obj->hdg, -sizex, sizez, 0x30, &pos,
                                   &ShadNorm, gindex + 1,
                                   obj->ddb * 0x10000 + obj->ddg * 0x100
                                       + obj->ddr);
            }
        }
    }
}


void InitGameDebris(void) {
    s32 i;

    for (i = 0; i < 170; i++) {
        GDeb[i].i = -1;
        if (((GDeb[i].levbits >> Level) & 1) != 0) {
            GDeb[i].i = LookupDebrisEffect(GDeb[i].name);
        }
    }
}

void AddWarpDebris(struct obj_s *obj, struct nuvec_s *pos) {
    struct nuvec_s v;
    s32 key;

    key = -1;
    v.x = obj->pos.x;
    v.y = obj->pos.y + (obj->bot + obj->top) * obj->SCALE * 0.5f;
    v.z = obj->pos.z;
    AddFiniteShotDebrisEffect(&key, GDeb[77].i, &v, 1);
}


void AddMechanicalDebris(struct nuvec_s *pos, s32 vehicle) {
    s32 i;
    s32 *pt;

    pt = jeepbits;
    i = 1;
    switch (vehicle) {
    case 0x44:
    case 0xB2:
        pt = mechbits;
        i = 1;
        break;
    case 0x53:
        pt = atlasbits;
        break;
    case 0x3B:
        pt = helibits;
        break;
    case 0x20:
        AddGameDebris(0x8E, pos);
        pt = jeepbits;
        i = 6;
        break;
    }
    while (*pt != -1) {
        AddDeb3(pos, *pt, i, 0);
        pt++;
    }
}
