/*
 * Unit: game/ai
 *
 * Functions:
 *   0x0022ef10 LoadAI
 *   0x0022f148 SaveAI
 *   0x0022f2c8 ResetAI
 *   0x0022f568 UpdateDRAINDAMAGE
 *   0x0022f858 DrawDRAINDAMAGE
 *   0x0022f928 WaterCrunchFunction_Spladoosh
 *   0x0022fac8 UpdateCRUNCHTIME
 *   0x002308c0 DrawCRUNCHTIME
 *   0x00230ae0 SpaceCrunchFunction_NewPoint
 *   0x00230c20 FindGongBongerAnim
 *   0x00230d28 MoveCreature
 *   0x00234920 InitAI
 *   0x002349c0 FindNearestCreature
 *   0x00234ab8 FindAIType
 *   0x00234b60 ResetDRAINDAMAGE
 *   0x00234ba0 WaterCrunchFunction_Attack
 *   0x00234bf0 WaterCrunchFunction_Punch1
 *   0x00234cc8 WaterCrunchFunction_Punch2
 *   0x00234da0 WaterCrunchFunction_Defeated
 *   0x00234dd8 ResetCRUNCHTIME
 *   0x00234eb0 SpaceCrunchFunction_NewCount
 *   0x00234ef0 SpaceCrunchFunction_AttackCortex
 *   0x00234f20 SpaceCrunchFunction_CheckCortexBack
 *   0x00234f28 SpaceCrunchFunction_PunchCortex
 *   0x00234f88 SpaceCortexFunction_CheckPunch
 *   0x00234f90 SpaceCortexFunction_Defeated
 *   0x00234fd0 SpaceCortexFunction_TakeHit
 *   0x00235000 RatioDifferenceAlongLine
 *   0x00235068 FindAILabel
 *   0x002350a8 PlayerLateralInRange
 *   0x00235198 PlayerLateralOutOfRange
 */

#include "nu_libc.h"

#include "nu3d/nugscn.h"
#include "nu3d/nulight.h"
#include "numath/nutrig.h"
#include "numath/nuvec.h"
#include "game/gametypes.h"
#include "gamelib/terrain.h"
#include "game/creature.h"
#include "gamelib/debris.h"

extern struct rail_s Rail[];           /* 0x00586388 */
extern RailPosition gempath_RPos;     /* 0x0058b518 */
extern s32 LEVELAITYPES;               /* 0x00631bc8 */
extern s32 LEVELAICOUNT;               /* 0x00631bcc */
extern s32 Level;                      /* 0x00630b90 */
extern s32 bonus_restart;              /* 0x00630d34 */
extern u8 temp_iRAIL;                  /* 0x00630ab4 */
extern s16 temp_iALONG;                /* 0x00630ab8 */
extern f32 temp_fALONG;                /* 0x00630abc */

void LoadScreen(char *name);           /* 0x001cabf0 */
f32 FindNearestCreature(struct nuvec_s *pos, s32 type, struct nuvec_s *out); /* 0x002349c0 */
void LoadAI(void);                     /* 0x0022ef10 */
void ResetAI(void);                    /* 0x0022f2c8 */
void GetALONG(struct nuvec_s *pos, RailPosition *rpos, s32 iRAIL, s32 iALONG, s32 dir); /* 0x001dac00 */
s32 AheadOfCheckpoint(s32 iRAIL, s32 iALONG, f32 fALONG); /* 0x001f0278 */
void RemoveGameObject(struct obj_s *obj); /* 0x00200c38 */

extern char tbuf[];                    /* 0x00592090 */
extern char LevelFileName[];           /* 0x0058b098 */
extern struct chase_s Chase[3];        /* 0x005f3ed0 (scratch load buffer) */

void *NuFileOpen(char *name, s32 mode);            /* 0x001002d8 */
s32 NuFileLoadBuffer(char *name, void *buf, s32 max); /* 0x00100a88 */
void *NuMemFileOpen(void *buf, s32 size, s32 mode);   /* 0x00103430 */
void NuFileClose(void *f);                         /* 0x00102c20 */
f32 NuFileReadFloat(void *f);                      /* 0x00102e20 */
s32 NuFileReadInt(void *f);                        /* 0x00102e48 */
s32 NuFileReadChar(void *f);                       /* 0x00102ec0 */
void NuFileWriteFloat(void *f, f32 v);             /* 0x00102f08 */
s32 NuFileWriteInt(void *f, s32 v);                /* 0x00102f30 */
void NuFileWriteChar(void *f, s32 v);              /* 0x00102fa8 */
s32 qrand(void);
f32 NuFsqrt(f32 x);
f32 RatioAlongLine(struct nuvec_s *pos, struct nuvec_s *a, struct nuvec_s *b); /* 0x001f0730 */

extern s32 DrainDamage_Intro;
extern s32 SpaceCortex_Defeated;
extern s32 boss_dead;
extern s32 Hub;

extern struct game_s Game;
extern s32 gamesfx_effect_volume;
extern struct nugscn_s *world_scene[32];

extern s32 jcrunch;
extern s32 CrunchTime_Intro;
void InitVehMasks(void);
void InitVehMask(s32 a, s32 b);
void SetNewMaskStuff(s32 index, struct nuvec_s *centre, struct nuvec_s *offset,
                     f32 rad, f32 anginc, f32 tweeninc, s32 fixed,
                     s32 killatend, f32 scale, f32 tiltx);

extern f32 NuTrigTable[];
extern struct nuvec_s D_0058656C;      /* DRAINDAMAGE aim target */
u16 SeekRot(u16 a, u16 target, s32 rate);

extern struct cammtx_s GameCam[];        /* 0x005864c8 */
extern struct nuvec_s v001;
void JudderGameCamera(struct cammtx_s *cam, f32 time, struct nuvec_s *pos);
void AddGameDebrisRot(s32 i, struct nuvec_s *pos, s32 n, u16 xrot, u16 yrot);
void Draw3DCharacter(struct nuvec_s *pos, s32 xrot, s32 yrot, s32 z,
                     struct CharacterModel *model, f32 scale, s32 action,
                     f32 anim_time, s32 last);

extern struct gdeb_s GDeb[170];        /* 0x0060dd00 */
void AddFiniteShotDebrisEffect(s32 *key, s32 effect, struct nuvec_s *pos, s32 n);
void DebrisEmitterOrientation(s32 key, s32 a, s32 hdg);

void GameSfx(s32 id, struct nuvec_s *pos);

s32 WaterCrunchFunction_Spladoosh(struct creature_s *c);
s32 WaterCrunchFunction_Attack(struct creature_s *c);
s32 WaterCrunchFunction_Punch1(struct creature_s *c);
s32 WaterCrunchFunction_Punch2(struct creature_s *c);
s32 WaterCrunchFunction_Defeated(void);
s32 SpaceCrunchFunction_NewCount(struct creature_s *c);
s32 SpaceCrunchFunction_NewPoint(struct creature_s *c, struct nuvec_s *pos);
s32 SpaceCrunchFunction_AttackCortex(struct creature_s *c, struct nuvec_s *v);
s32 SpaceCrunchFunction_PunchCortex(void);
s32 SpaceCrunchFunction_CheckCortexBack(void);
s32 SpaceCortexFunction_CheckPunch(void);
s32 SpaceCortexFunction_TakeHit(struct creature_s *c);
s32 SpaceCortexFunction_Defeated(void);

static s32 drain_wawa_ok;
static u16 drain_wawa_xrot;
static u16 drain_wawa_yrot;
static s16 drain_ang[3];
static struct nuvec_s drain_wawa_pos;
static AnimPacket drain_wawa_anim;
static struct CharacterModel *drain_wawa_model;
static u32 crunchtime_attack_phase;
static u32 earth_attack_wait;
static u32 water_attack_wait;
static u32 water_last_chute;
static u32 fire_attack_wait;
static u32 weather_attack_wait;
static u16 spaceuka_angle[3];
static struct nuvec_s space_ukapos;
static s32 SpaceCrunch_Punch;
static s32 SpaceCortex_Back;

static struct creatcmd_s CMD_FISHSWIM[23] = {
    { 0x00, 0, 0.0f },
    { 0x60, 16384, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x2A, 32768, 0.0f },
    { 0x47, 76, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x2D, 1310, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x10, 0, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x56, 514, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x44, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x55, 1, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x57, 1, 0.0f },
    { 0x3D, 0, 1.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_EEL[15] = {
    { 0x00, 0, 0.0f },
    { 0x60, 16384, 0.0f },
    { 0x4C, 0, 0.0f },
    { 0x5E, 0, 0.0f },
    { 0x5F, 1, 0.0f },
    { 0x47, 76, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x2D, 262, 0.0f },
    { 0x55, 1024, 0.0f },
    { 0x56, 512, 0.0f },
    { 0x57, 1, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_CLAM[11] = {
    { 0x00, 0, 0.0f },
    { 0x60, 20480, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x56, 4, 0.0f },
    { 0x2D, 327, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_DONKEY[27] = {
    { 0x00, 0, 0.0f },
    { 0x60, 32768, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x8B, 119, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x3B, 0, 0.0f },
    { 0x74, 12, 2.5f },
    { 0x78, 97, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x68, 47, 0.0f },
    { 0x48, 0, 0.0f },
    { 0x3E, 0, 0.0f },
    { 0x3B, 3, 0.1f },
    { 0x8A, 0, 0.0f },
    { 0x7E, 2, 2.5f },
    { 0x8D, 97, 0.0f },
    { 0x8C, 119, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_DROID1[21] = {
    { 0x00, 0, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x56, 4, 0.0f },
    { 0x33, 49152, 0.0f },
    { 0x2D, 327, 0.0f },
    { 0x8B, 105, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x74, 12, 2.0f },
    { 0x8A, 0, 0.0f },
    { 0x68, 128, 0.0f },
    { 0x48, 0, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x74, 13, 2.5f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 105, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_DROID2[23] = {
    { 0x00, 0, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x23, 0, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x56, 4, 0.0f },
    { 0x2D, 655, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x74, 10, 3.0f },
    { 0x8A, 0, 0.0f },
    { 0x68, 130, 0.0f },
    { 0x46, 5, 0.0f },
    { 0x48, 0, 0.0f },
    { 0x3E, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x6A, -1, 2.0f },
    { 0x68, 59, 0.0f },
    { 0x8F, 4, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_DROID3[32] = {
    { 0x00, 0, 0.0f },
    { 0x60, 32768, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x2A, 32768, 0.0f },
    { 0x47, 58, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 478, 0.0f },
    { 0x2D, 1310, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x48, 58, 0.0f },
    { 0x10, 0, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x56, 514, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x44, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x55, 17, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x57, 3, 0.0f },
    { 0x3D, 0, 2.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 0, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x3E, 0, 3.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_DROID4[24] = {
    { 0x00, 0, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x2A, 32768, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x2D, 1310, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x48, 0, 0.0f },
    { 0x10, 0, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x56, 514, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x3E, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x55, 1, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x57, 1, 0.0f },
    { 0x3D, 0, 1.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_SCORPION[27] = {
    { 0x00, 0, 0.0f },
    { 0x39, 16, 0.0f },
    { 0x3A, 16, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x2A, 32768, 0.0f },
    { 0x47, 116, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x2D, 1310, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x10, 0, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x56, 514, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x44, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x55, 1, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x57, 3, 0.0f },
    { 0x3D, 0, 1.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_ROOFSCORPION[26] = {
    { 0x00, 0, 0.0f },
    { 0x60, 65536, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x2A, 32768, 0.0f },
    { 0x47, 116, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x2D, 1310, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x10, 0, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x56, 514, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x44, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x55, 1, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x57, 9, 0.0f },
    { 0x3D, 0, 1.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_BIRD[33] = {
    { 0x00, 0, 0.0f },
    { 0x60, 20480, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x29, 0, 0.0f },
    { 0x56, 32, 0.0f },
    { 0x2D, 1310, 0.0f },
    { 0x4D, 0, 3.0f },
    { 0x8B, 105, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x74, 12, 1.5f },
    { 0x78, 114, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8B, 114, 0.0f },
    { 0x48, 116, 0.0f },
    { 0x3C, 0, 0.5f },
    { 0x55, 64, 0.0f },
    { 0x57, 3, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 116, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x55, 64, 0.0f },
    { 0x57, 3, 0.0f },
    { 0x74, 13, 1.5f },
    { 0x78, 105, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_BUTTERFLY[30] = {
    { 0x00, 0, 0.0f },
    { 0x60, 20480, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x56, 4, 0.0f },
    { 0x2D, 327, 0.0f },
    { 0x8B, 105, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x42, 0, 1.0f },
    { 0x8A, 0, 0.0f },
    { 0x7E, 2, 1.5f },
    { 0x8D, 102, 0.0f },
    { 0x8C, 105, 0.0f },
    { 0x8B, 102, 0.0f },
    { 0x09, 1, 0.0f },
    { 0x48, 29, 0.0f },
    { 0x42, 0, 2.0f },
    { 0x43, 0, 1.0f },
    { 0x55, 17, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x42, 0, 1.0f },
    { 0x43, 0, 1.0f },
    { 0x8A, 0, 0.0f },
    { 0x09, 0, 0.0f },
    { 0x42, 0, 3.0f },
    { 0x43, 0, 1.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 105, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_BEE[10] = {
    { 0x00, 0, 0.0f },
    { 0x60, 20480, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x47, 29, 0.0f },
    { 0x23, 0, 0.0f },
    { 0x56, 4, 0.0f },
    { 0x2D, 327, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_SHARK[22] = {
    { 0x00, 0, 0.0f },
    { 0x60, 24576, 0.0f },
    { 0x4C, 0, 0.0f },
    { 0x5E, 0, 0.0f },
    { 0x5F, 1, 0.0f },
    { 0x47, 76, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x2D, 218, 0.0f },
    { 0x55, 1024, 0.0f },
    { 0x56, 512, 0.0f },
    { 0x57, 1, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x48, 76, 0.0f },
    { 0x42, 0, 2.0f },
    { 0x43, 0, 1.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 0, 0.0f },
    { 0x3E, 0, 2.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_DEEPSEASHARK[15] = {
    { 0x00, 0, 0.0f },
    { 0x60, 16384, 0.0f },
    { 0x4C, 0, 0.0f },
    { 0x5E, 0, 0.0f },
    { 0x5F, 1, 0.0f },
    { 0x47, 76, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x2D, -163, 0.0f },
    { 0x55, 1024, 0.0f },
    { 0x56, 1024, 0.0f },
    { 0x57, 1, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_PENGUINP[51] = {
    { 0x00, 0, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x47, 116, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x48, 116, 0.0f },
    { 0x09, 1, 0.0f },
    { 0x3D, 0, 1.0f },
    { 0x55, 1, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x57, 3, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 87, 0.0f },
    { 0x09, 0, 0.0f },
    { 0x3C, 0, 1.0f },
    { 0x55, 0, 0.0f },
    { 0x56, 1026, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 70, 0.0f },
    { 0x3C, 0, 0.5f },
    { 0x56, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x68, 19, 0.0f },
    { 0x3D, 0, 2.0f },
    { 0x37, 2, 0.0f },
    { 0x38, 384, 0.0f },
    { 0x55, 1, 0.0f },
    { 0x56, 1025, 8.0f },
    { 0x57, 3, 0.0f },
    { 0x8A, 0, -8.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x48, 115, 0.0f },
    { 0x3C, 0, 1.5f },
    { 0x55, 0, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 60, 0.0f },
    { 0x3C, 0, 0.5f },
    { 0x8A, 0, 0.0f },
    { 0x48, 87, 0.0f },
    { 0x09, 1, 0.0f },
    { 0x3C, 0, 1.0f },
    { 0x56, 1026, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_BIGPENGUINP[50] = {
    { 0x00, 0, 0.0f },
    { 0x60, 128, 0.0f },
    { 0x73, 0, 1.5f },
    { 0x01, 0, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x47, 116, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x38, 1534, 0.0f },
    { 0x48, 116, 0.0f },
    { 0x09, 1, 0.0f },
    { 0x3D, 0, 1.5f },
    { 0x55, 1, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x57, 3, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 87, 0.0f },
    { 0x09, 0, 0.0f },
    { 0x3C, 0, 1.0f },
    { 0x55, 0, 0.0f },
    { 0x56, 1026, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 70, 0.0f },
    { 0x3C, 0, 0.5f },
    { 0x56, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x68, 19, 0.0f },
    { 0x3D, 0, 3.0f },
    { 0x38, 0, 0.0f },
    { 0x55, 1, 0.0f },
    { 0x56, 1025, 8.0f },
    { 0x57, 3, 0.0f },
    { 0x8A, 0, -8.0f },
    { 0x38, 1534, 0.0f },
    { 0x48, 115, 0.0f },
    { 0x3C, 0, 1.5f },
    { 0x55, 0, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 60, 0.0f },
    { 0x3C, 0, 0.5f },
    { 0x8A, 0, 0.0f },
    { 0x48, 87, 0.0f },
    { 0x09, 1, 0.0f },
    { 0x3C, 0, 1.0f },
    { 0x56, 1026, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_CRAB[42] = {
    { 0x00, 0, 0.0f },
    { 0x60, 32768, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x2A, 16384, 0.0f },
    { 0x5E, 0, 0.0f },
    { 0x5F, 1, 0.0f },
    { 0x05, 0, 0.5f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x47, 0, 0.0f },
    { 0x4D, 0, 1.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x48, 0, 0.0f },
    { 0x3E, 0, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x7E, 5, 0.25f },
    { 0x8D, 109, 0.25f },
    { 0x48, 34, 0.0f },
    { 0x42, 0, 2.0f },
    { 0x43, 0, 1.0f },
    { 0x74, 3, 0.25f },
    { 0x78, 109, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x8B, 109, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x55, 4, 0.0f },
    { 0x57, 3, 0.0f },
    { 0x74, 2, 0.05f },
    { 0x78, 97, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_KNIGHT[74] = {
    { 0x00, 0, 0.0f },
    { 0x60, 320, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x2A, 16384, 0.0f },
    { 0x5E, 0, 0.0f },
    { 0x5F, 1, 0.0f },
    { 0x05, 0, 0.5f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x4D, 0, 2.0f },
    { 0x38, 48, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x8B, 119, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x7E, 6, 0.0f },
    { 0x8D, 100, 0.0f },
    { 0x7E, 2, 5.0f },
    { 0x8D, 109, 0.0f },
    { 0x74, 12, 5.0f },
    { 0x78, 109, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8B, 109, 0.0f },
    { 0x7E, 6, 0.0f },
    { 0x8D, 100, 0.0f },
    { 0x7E, 4, 0.05f },
    { 0x8D, 97, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x74, 2, 0.05f },
    { 0x78, 97, 0.0f },
    { 0x75, 9, 0.0f },
    { 0x79, 100, 0.0f },
    { 0x55, 4, 0.0f },
    { 0x57, 3, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x7E, 6, 0.0f },
    { 0x8D, 100, 0.0f },
    { 0x48, 0, 0.0f },
    { 0x3E, 0, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8B, 105, 0.0f },
    { 0x7E, 6, 0.0f },
    { 0x8D, 100, 0.0f },
    { 0x7E, 5, 0.25f },
    { 0x8D, 119, 0.0f },
    { 0x7E, 3, 5.0f },
    { 0x8D, 119, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x42, 0, 2.0f },
    { 0x43, 0, 1.0f },
    { 0x74, 3, 0.25f },
    { 0x78, 119, 0.0f },
    { 0x75, 13, 5.0f },
    { 0x79, 119, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x8B, 100, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_SEAL[13] = {
    { 0x00, 0, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x47, 116, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x55, 1, 0.0f },
    { 0x57, 3, 0.0f },
    { 0x09, 1, 0.0f },
    { 0x3D, 0, 0.75f },
    { 0x8A, 0, 0.0f },
    { 0x8F, 7, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_PARROT[51] = {
    { 0x00, 0, 0.0f },
    { 0x4C, 0, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x47, 116, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x09, 0, 0.0f },
    { 0x51, 2, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x48, 116, 0.0f },
    { 0x2D, 131, 0.0f },
    { 0x53, 0, 0.5f },
    { 0x41, 0, 0.0f },
    { 0x74, 12, 5.0f },
    { 0x55, 2, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x57, 3, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x65, 49149, 0.0f },
    { 0x68, 125, 0.0f },
    { 0x48, 29, 0.0f },
    { 0x54, 0, 2.0f },
    { 0x2D, 1769, 0.0f },
    { 0x57, 33, 0.0f },
    { 0x3E, 0, 3.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 29, 0.0f },
    { 0x54, 0, 2.0f },
    { 0x2D, 1769, 0.0f },
    { 0x57, 17, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x74, 12, 2.5f },
    { 0x8A, 0, 0.0f },
    { 0x48, 0, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x3F, 0, 8.0f },
    { 0x8A, 0, 0.0f },
    { 0x0A, 0, 0.0f },
    { 0x0B, 0, 0.0f },
    { 0x55, 1, 0.0f },
    { 0x40, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x65, 32766, 0.0f },
    { 0x68, 124, 0.0f },
    { 0x6A, -1, 1.5f },
    { 0x45, 191, 0.0f },
    { 0x46, 5, 0.0f },
    { 0x8F, 6, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_NARWHAL[21] = {
    { 0x00, 0, 0.0f },
    { 0x60, 24576, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x34, 0, 0.0f },
    { 0x47, 44, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 384, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x60, 4096, 0.0f },
    { 0x5B, 0, 0.0f },
    { 0x42, 0, 2.0f },
    { 0x43, 0, 2.0f },
    { 0x8A, 0, 0.0f },
    { 0x61, 4096, 0.0f },
    { 0x5C, 0, 0.0f },
    { 0x24, 0, 0.0f },
    { 0x59, 0, 1.0f },
    { 0x3E, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_WIZARD[55] = {
    { 0x00, 0, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x23, 0, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x8B, 105, 0.0f },
    { 0x7E, 6, 0.0f },
    { 0x8D, 100, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x42, 0, 1.0f },
    { 0x43, 0, 2.0f },
    { 0x56, 4, 0.0f },
    { 0x2D, 327, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x8B, 119, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x74, 12, 7.5f },
    { 0x56, 4, 0.0f },
    { 0x2D, 327, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x7E, 6, 0.0f },
    { 0x8D, 100, 0.0f },
    { 0x7E, 2, 7.5f },
    { 0x8E, 119, 0.0f },
    { 0x48, 0, 0.0f },
    { 0x3F, 0, 21.0f },
    { 0x74, 9, 0.0f },
    { 0x78, 100, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x7E, 6, 0.0f },
    { 0x8D, 100, 0.0f },
    { 0x62, 2, 0.0f },
    { 0x6B, 1, 0.0f },
    { 0x40, 0, 0.0f },
    { 0x74, 9, 0.0f },
    { 0x78, 100, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 105, 0.0f },
    { 0x8B, 100, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_MONKEYBANANA[43] = {
    { 0x00, 0, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x47, 116, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x5B, 0, 0.0f },
    { 0x42, 0, 1.0f },
    { 0x8A, 0, 0.0f },
    { 0x5C, 0, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x68, 117, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x48, 116, 0.0f },
    { 0x09, 1, 0.0f },
    { 0x3D, 0, 1.0f },
    { 0x74, 12, 2.0f },
    { 0x78, 98, 1.0f },
    { 0x55, 1, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x57, 3, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x8B, 98, 0.0f },
    { 0x68, 118, 0.0f },
    { 0x48, 0, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x3F, 0, 41.0f },
    { 0x8A, 0, 0.0f },
    { 0x6A, -1, 1.5f },
    { 0x68, 81, 0.0f },
    { 0x40, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 116, 0.0f },
    { 0x3D, 0, 1.0f },
    { 0x55, 1, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x57, 3, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_KOICARP[24] = {
    { 0x00, 0, 0.0f },
    { 0x60, 24576, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x34, 0, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x29, 0, 0.0f },
    { 0x2D, 327, 0.0f },
    { 0x32, 32768, 0.0f },
    { 0x51, 0, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x56, 64, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x42, 0, 2.0f },
    { 0x43, 0, 1.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 0, 0.0f },
    { 0x3E, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_TURTLE[34] = {
    { 0x00, 0, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x47, 76, 0.0f },
    { 0x37, 0, 0.0f },
    { 0x38, 0, 0.0f },
    { 0x55, 1, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x57, 1, 0.0f },
    { 0x74, 8, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x09, 1, 0.0f },
    { 0x3D, 0, 2.0f },
    { 0x55, 1, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x09, 0, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x56, 514, 0.0f },
    { 0x3C, 0, 2.0f },
    { 0x8A, 0, 0.0f },
    { 0x3D, 0, 2.0f },
    { 0x55, 1, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x09, 1, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x56, 514, 0.0f },
    { 0x3C, 0, 2.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_NINJASTALK[36] = {
    { 0x00, 0, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x47, 29, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x59, 0, 1.0f },
    { 0x48, 29, 0.0f },
    { 0x74, 4, 3.0f },
    { 0x7A, 1, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x5B, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x68, 120, 0.0f },
    { 0x6D, 9, 0.0f },
    { 0x6D, 8, 0.0f },
    { 0x5C, 0, 0.0f },
    { 0x48, 43, 0.0f },
    { 0x3E, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x3E, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x68, 123, 0.0f },
    { 0x6D, 9, 0.0f },
    { 0x6D, 8, 0.0f },
    { 0x8F, 7, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_NINJASWORD[78] = {
    { 0x00, 0, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x8B, 115, 0.0f },
    { 0x5B, 0, 0.0f },
    { 0x74, 4, 1.0f },
    { 0x7A, 0, 0.0f },
    { 0x78, 97, 0.0f },
    { 0x75, 4, 1.0f },
    { 0x7B, 1, 0.0f },
    { 0x79, 98, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x5C, 0, 0.0f },
    { 0x01, 1, 0.0f },
    { 0x68, 120, 0.0f },
    { 0x6D, 9, 0.0f },
    { 0x6D, 8, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x22, 0, 0.0f },
    { 0x48, 43, 0.0f },
    { 0x3E, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 0, 0.0f },
    { 0x3F, 0, 25.0f },
    { 0x8A, 0, 0.0f },
    { 0x68, 121, 0.0f },
    { 0x62, 3, 0.0f },
    { 0x6B, 1, 0.0f },
    { 0x40, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x5B, 0, 0.0f },
    { 0x68, 123, 0.0f },
    { 0x6D, 9, 0.0f },
    { 0x6D, 8, 0.0f },
    { 0x74, 6, 1.0f },
    { 0x7A, 0, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 115, 0.0f },
    { 0x8B, 98, 0.0f },
    { 0x5C, 0, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x68, 120, 0.0f },
    { 0x6D, 9, 0.0f },
    { 0x6D, 8, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x48, 43, 0.0f },
    { 0x3E, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 0, 0.0f },
    { 0x3F, 0, 25.0f },
    { 0x8A, 0, 0.0f },
    { 0x68, 121, 0.0f },
    { 0x62, 3, 0.0f },
    { 0x6B, 1, 0.0f },
    { 0x40, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x5B, 0, 0.0f },
    { 0x68, 123, 0.0f },
    { 0x6D, 9, 0.0f },
    { 0x6D, 8, 0.0f },
    { 0x74, 6, 1.0f },
    { 0x7A, 1, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 115, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_BAT[17] = {
    { 0x00, 0, 0.0f },
    { 0x4C, 0, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x09, 0, 0.0f },
    { 0x47, 29, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x51, 2, 0.0f },
    { 0x2D, 436, 0.0f },
    { 0x53, 0, 0.5f },
    { 0x30, 4096, 0.15f },
    { 0x55, 770, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x57, 1, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_GONGBONGER[29] = {
    { 0x00, 0, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x3B, 0, 0.0f },
    { 0x42, 0, 1.0f },
    { 0x43, 0, 2.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 0, 0.0f },
    { 0x3B, 3, 0.1f },
    { 0x3F, 0, 41.0f },
    { 0x68, 203, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x6F, 0, 0.0f },
    { 0x70, 0, 0.0f },
    { 0x40, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_FLAMINGO[26] = {
    { 0x00, 0, 0.0f },
    { 0x60, 49152, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x3B, 0, 0.0f },
    { 0x42, 0, 2.0f },
    { 0x43, 0, 1.0f },
    { 0x8A, 0, 0.0f },
    { 0x3B, 0, 0.0f },
    { 0x74, 4, 3.0f },
    { 0x41, 0, 2.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 0, 0.0f },
    { 0x3B, 1, 0.1f },
    { 0x3E, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_NGIN[28] = {
    { 0x00, 0, 0.0f },
    { 0x60, 128, 0.0f },
    { 0x73, 0, 1.5f },
    { 0x01, 0, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x48, 43, 0.0f },
    { 0x3E, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x3E, 0, 2.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 0, 0.0f },
    { 0x3E, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 53, 0.0f },
    { 0x3E, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x5B, 0, 0.0f },
    { 0x3C, 0, 2.0f },
    { 0x8A, 0, 0.0f },
    { 0x5C, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_NTROPY[19] = {
    { 0x00, 0, 0.0f },
    { 0x60, 128, 0.0f },
    { 0x73, 0, 1.5f },
    { 0x01, 0, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x42, 0, 1.0f },
    { 0x43, 0, 1.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 0, 0.0f },
    { 0x3E, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_DINGODILEFLAME[21] = {
    { 0x00, 0, 0.0f },
    { 0x60, 128, 0.0f },
    { 0x73, 0, 1.5f },
    { 0x01, 0, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x42, 0, 1.0f },
    { 0x43, 0, 2.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 0, 0.0f },
    { 0x3E, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_DINGODILEFIRE[21] = {
    { 0x00, 0, 0.0f },
    { 0x60, 128, 0.0f },
    { 0x73, 0, 1.5f },
    { 0x01, 0, 0.0f },
    { 0x26, 1, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x42, 0, 1.0f },
    { 0x43, 0, 2.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 27, 0.0f },
    { 0x3E, 0, 1.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_GORILLA[21] = {
    { 0x00, 0, 0.0f },
    { 0x60, 32832, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x38, 44, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x74, 16, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x65, 32766, 0.0f },
    { 0x68, 118, 0.0f },
    { 0x48, 56, 0.0f },
    { 0x3E, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_MAMMOTH[19] = {
    { 0x00, 0, 0.0f },
    { 0x60, 32832, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x38, 300, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x74, 16, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 56, 0.0f },
    { 0x3E, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_TINY[40] = {
    { 0x00, 0, 0.0f },
    { 0x60, 32896, 0.0f },
    { 0x73, 0, 1.5f },
    { 0x01, 0, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x2A, 16384, 0.0f },
    { 0x5E, 0, 0.0f },
    { 0x5F, 1, 0.0f },
    { 0x05, 0, 0.5f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x47, 0, 0.0f },
    { 0x4D, 0, 2.0f },
    { 0x8B, 105, 0.0f },
    { 0x7E, 5, 0.5f },
    { 0x8D, 109, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x74, 3, 0.5f },
    { 0x78, 109, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8B, 109, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x55, 4, 0.0f },
    { 0x57, 3, 0.0f },
    { 0x74, 2, 0.05f },
    { 0x78, 105, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8B, 112, 0.0f },
    { 0x48, 56, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x3E, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 105, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_LIZARD[63] = {
    { 0x00, 0, 0.0f },
    { 0x60, 32768, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x47, 58, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x48, 58, 0.0f },
    { 0x0F, 1, 0.5f },
    { 0x3D, 0, 2.0f },
    { 0x55, 1, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x57, 3, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x3E, 0, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 58, 0.0f },
    { 0x09, 1, 0.0f },
    { 0x3D, 0, 2.0f },
    { 0x55, 1, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x57, 3, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 58, 0.0f },
    { 0x09, 0, 0.0f },
    { 0x3C, 0, 1.0f },
    { 0x55, 0, 0.0f },
    { 0x56, 514, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 58, 0.0f },
    { 0x0F, 256, 0.5f },
    { 0x3D, 0, 2.0f },
    { 0x55, 1, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x57, 3, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x3E, 0, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 58, 0.0f },
    { 0x09, 0, 0.0f },
    { 0x3D, 0, 2.0f },
    { 0x55, 1, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x57, 3, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x09, 1, 0.0f },
    { 0x3C, 0, 1.0f },
    { 0x55, 0, 0.0f },
    { 0x56, 514, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_LIZARDAB[72] = {
    { 0x00, 0, 0.0f },
    { 0x60, 32768, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x47, 58, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x48, 58, 0.0f },
    { 0x09, 1, 0.0f },
    { 0x3D, 0, 2.5f },
    { 0x55, 1, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x57, 3, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x3E, 0, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 58, 0.0f },
    { 0x09, 2, 0.0f },
    { 0x3C, 0, 0.5f },
    { 0x55, 0, 0.0f },
    { 0x56, 2, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x3D, 0, 2.5f },
    { 0x55, 1, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x57, 3, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x3E, 0, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 58, 0.0f },
    { 0x09, 3, 0.0f },
    { 0x3C, 0, 0.5f },
    { 0x55, 0, 0.0f },
    { 0x56, 2, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x3D, 0, 2.5f },
    { 0x55, 1, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x57, 3, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x3E, 0, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 58, 0.0f },
    { 0x09, 4, 0.0f },
    { 0x3C, 0, 0.5f },
    { 0x55, 0, 0.0f },
    { 0x56, 2, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x3D, 0, 2.5f },
    { 0x55, 1, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x57, 3, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8F, 7, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_RICKSHAW[24] = {
    { 0x00, 0, 0.0f },
    { 0x60, 32768, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x47, 58, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 128, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x5B, 0, 0.0f },
    { 0x42, 0, 2.0f },
    { 0x43, 0, 1.0f },
    { 0x55, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x5C, 0, 0.0f },
    { 0x10, 0, 0.0f },
    { 0x25, 0, 0.0f },
    { 0x3D, 0, 2.0f },
    { 0x55, 1, 0.0f },
    { 0x57, 3, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_TUNA[15] = {
    { 0x00, 0, 0.0f },
    { 0x60, 40960, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x29, 0, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x5A, 0, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x57, 3, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_WHEELA[28] = {
    { 0x00, 0, 0.0f },
    { 0x60, 32768, 0.0f },
    { 0x47, 58, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 128, 0.0f },
    { 0x2D, 655, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x5B, 0, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x42, 0, 2.0f },
    { 0x43, 0, 1.0f },
    { 0x8A, 0, 0.0f },
    { 0x5C, 0, 0.0f },
    { 0x55, 1, 0.0f },
    { 0x56, 32, 0.0f },
    { 0x57, 3, 0.0f },
    { 0x8B, 98, 0.0f },
    { 0x10, 0, 0.0f },
    { 0x7E, 8, 0.0f },
    { 0x8D, 97, 0.0f },
    { 0x3D, 0, 2.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 98, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_CACTUS[26] = {
    { 0x00, 0, 0.0f },
    { 0x39, 16, 0.0f },
    { 0x3A, 16, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x29, 0, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x3B, 0, 0.0f },
    { 0x3C, 0, 3.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 0, 0.0f },
    { 0x38, 0, 0.0f },
    { 0x3B, 1, 0.4f },
    { 0x3E, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_DIVER[18] = {
    { 0x00, 0, 0.0f },
    { 0x60, 147456, 0.0f },
    { 0x07, 1, 0.5f },
    { 0x35, 0, 0.0f },
    { 0x24, 0, 0.0f },
    { 0x5E, 0, 0.0f },
    { 0x5F, 1, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x4D, 0, 1.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x55, 8, 0.0f },
    { 0x56, 8, 0.0f },
    { 0x2D, 327, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_DYNO[20] = {
    { 0x00, 0, 0.0f },
    { 0x60, 20480, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x37, 0, 0.0f },
    { 0x38, 0, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x3C, 0, 3.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 0, 0.0f },
    { 0x3F, 0, 30.0f },
    { 0x8A, 0, 0.0f },
    { 0x6A, 1, 1.0f },
    { 0x68, 59, 1.0f },
    { 0x40, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_VULTURE[30] = {
    { 0x00, 0, 0.0f },
    { 0x60, 32768, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x04, 0, 1.5f },
    { 0x22, 2, 0.0f },
    { 0x47, 29, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 428, 0.0f },
    { 0x74, 4, 5.0f },
    { 0x7A, 0, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x65, 65532, 0.0f },
    { 0x68, 125, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x09, 1, 0.0f },
    { 0x55, 1, 0.0f },
    { 0x54, 0, 1.5f },
    { 0x57, 65, 0.0f },
    { 0x3C, 0, 0.5f },
    { 0x8A, 0, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x09, 2, 0.0f },
    { 0x57, 3, 0.0f },
    { 0x3D, 0, 3.0f },
    { 0x8A, 0, 0.0f },
    { 0x8F, 4, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_JELLYFISH[22] = {
    { 0x00, 0, 0.0f },
    { 0x39, 17, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x04, 0, 1.0f },
    { 0x2F, 0, 0.0f },
    { 0x47, 76, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x55, 1, 0.0f },
    { 0x23, 0, 0.0f },
    { 0x56, 4, 0.0f },
    { 0x2D, 655, 0.0f },
    { 0x57, 1, 0.0f },
    { 0x32, 32768, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x13, 0, 0.5f },
    { 0x3D, 0, 0.5f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_WETJELLYFISH[16] = {
    { 0x00, 0, 0.0f },
    { 0x6E, 142, 0.0f },
    { 0x60, 16384, 0.0f },
    { 0x39, 17, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x68, 146, 0.0f },
    { 0x47, 76, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x29, 0, 0.0f },
    { 0x55, 1, 0.0f },
    { 0x09, 1, 0.0f },
    { 0x3D, 0, 1.0f },
    { 0x8A, 0, 0.0f },
    { 0x8F, 21, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_PYGMY[19] = {
    { 0x00, 0, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x42, 0, 1.0f },
    { 0x43, 0, 1.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 82, 0.0f },
    { 0x3E, 0, 1.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_OCTOPUS[19] = {
    { 0x00, 0, 0.0f },
    { 0x60, 16384, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x42, 0, 1.0f },
    { 0x43, 0, 2.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 0, 0.0f },
    { 0x3F, 0, 15.0f },
    { 0x8A, 0, 0.0f },
    { 0x40, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_TENTACLE[19] = {
    { 0x00, 0, 0.0f },
    { 0x60, 24576, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x29, 0, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x3B, 0, 0.0f },
    { 0x3C, 0, 3.0f },
    { 0x8A, 0, 0.0f },
    { 0x68, 71, 0.0f },
    { 0x48, 0, 0.0f },
    { 0x3B, 2, 0.25f },
    { 0x3E, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_MOUSE[32] = {
    { 0x00, 0, 0.0f },
    { 0x60, 20480, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x47, 58, 0.0f },
    { 0x56, 32, 0.0f },
    { 0x2D, 1310, 0.0f },
    { 0x4D, 0, 3.0f },
    { 0x8B, 105, 0.0f },
    { 0x48, 58, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x74, 12, 1.5f },
    { 0x78, 114, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8B, 114, 0.0f },
    { 0x48, 58, 0.0f },
    { 0x3C, 0, 0.15f },
    { 0x55, 64, 0.0f },
    { 0x57, 3, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 58, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x55, 64, 0.0f },
    { 0x57, 3, 0.0f },
    { 0x74, 13, 1.5f },
    { 0x78, 105, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_ROBIN[10] = {
    { 0x00, 0, 0.0f },
    { 0x60, 20480, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x47, 29, 0.0f },
    { 0x23, 0, 0.0f },
    { 0x2D, 327, 0.0f },
    { 0x56, 4, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_STARFISH[11] = {
    { 0x00, 0, 0.0f },
    { 0x60, 20480, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x29, 0, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_DRAGONFLY[15] = {
    { 0x00, 0, 0.0f },
    { 0x60, 20480, 0.0f },
    { 0x47, 29, 0.0f },
    { 0x8B, 102, 0.0f },
    { 0x15, 0, 0.0f },
    { 0x55, 17, 0.0f },
    { 0x23, 0, 0.0f },
    { 0x2D, 327, 0.0f },
    { 0x3D, 0, 6.0f },
    { 0x8A, 0, 0.0f },
    { 0x42, 0, 2.0f },
    { 0x2E, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 102, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_SNOWHOG[11] = {
    { 0x00, 0, 0.0f },
    { 0x60, 20480, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x29, 0, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_POLARGUARD[11] = {
    { 0x00, 0, 0.0f },
    { 0x60, 4096, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x29, 0, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_SKIER[42] = {
    { 0x00, 0, 0.0f },
    { 0x60, 32768, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x2A, 32768, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x54, 0, 0.05f },
    { 0x8B, 97, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x09, 1, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x56, 514, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x42, 0, 0.666f },
    { 0x43, 0, 0.333f },
    { 0x8A, 0, 0.0f },
    { 0x69, 47, 32766.0f },
    { 0x48, 58, 0.0f },
    { 0x55, 1, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x57, 17, 0.0f },
    { 0x3D, 0, 3.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x09, 0, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x56, 514, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x42, 0, 0.666f },
    { 0x43, 0, 0.333f },
    { 0x8A, 0, 0.0f },
    { 0x69, 47, 32766.0f },
    { 0x48, 3, 0.0f },
    { 0x55, 1, 0.0f },
    { 0x56, 0, 0.0f },
    { 0x57, 17, 0.0f },
    { 0x3D, 0, 3.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_MINE[11] = {
    { 0x00, 0, 0.0f },
    { 0x60, 16384, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x24, 1, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 256, 0.0f },
    { 0x56, 8, 0.0f },
    { 0x2D, 1310, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_FALLINGMINE[22] = {
    { 0x00, 0, 0.0f },
    { 0x60, 16384, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x24, 1, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 256, 0.0f },
    { 0x56, 8, 0.0f },
    { 0x2D, 1310, 0.0f },
    { 0x74, 12, 3.0f },
    { 0x8A, 0, 0.0f },
    { 0x65, 49149, 0.0f },
    { 0x68, 130, 0.0f },
    { 0x46, 5, 0.0f },
    { 0x09, 1, 0.0f },
    { 0x55, 33, 0.0f },
    { 0x3C, 0, 3.0f },
    { 0x8A, 0, 0.0f },
    { 0x6A, -1, 1.0f },
    { 0x68, 59, 1.0f },
    { 0x8F, 21, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_HARPOONIST[26] = {
    { 0x00, 0, 0.0f },
    { 0x60, 180224, 0.0f },
    { 0x07, 1, 0.5f },
    { 0x22, 2, 0.0f },
    { 0x5E, 0, 0.0f },
    { 0x5F, 1, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x4D, 0, 1.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x42, 0, 1.0f },
    { 0x43, 0, 2.0f },
    { 0x55, 8, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x7E, 2, 3.0f },
    { 0x8E, 97, 0.0f },
    { 0x66, 1411, 0.0f },
    { 0x68, 40, 0.0f },
    { 0x48, 27, 0.0f },
    { 0x3E, 0, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_LABGUYSHIELD[71] = {
    { 0x00, 0, 0.0f },
    { 0x60, 294912, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x08, 0, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x47, 56, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x48, 56, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x74, 18, 0.0f },
    { 0x78, 107, 0.0f },
    { 0x75, 16, 0.0f },
    { 0x79, 116, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8B, 107, 0.0f },
    { 0x86, 16, 0.0f },
    { 0x7E, 14, 0.0f },
    { 0x8D, 98, 0.0f },
    { 0x8C, 115, 0.0f },
    { 0x8B, 116, 0.0f },
    { 0x86, 16, 0.0f },
    { 0x7E, 14, 0.0f },
    { 0x8D, 112, 0.0f },
    { 0x71, -1, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x8B, 112, 0.0f },
    { 0x72, 0, 0.16f },
    { 0x7E, 10, 0.0f },
    { 0x8D, 113, 0.0f },
    { 0x48, 44, 0.0f },
    { 0x38, 0, 0.0f },
    { 0x10, 0, 0.0f },
    { 0x55, 1, 0.0f },
    { 0x57, 3, 0.0f },
    { 0x3E, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x8B, 113, 0.0f },
    { 0x48, 56, 0.0f },
    { 0x3C, 0, 0.5f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x8B, 98, 0.0f },
    { 0x7E, 12, 0.0f },
    { 0x8D, 102, 0.0f },
    { 0x48, 56, 0.0f },
    { 0x38, 0, 0.0f },
    { 0x11, 0, 0.0f },
    { 0x55, 1, 0.0f },
    { 0x57, 3, 0.0f },
    { 0x3C, 0, 0.333f },
    { 0x8A, 0, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x8B, 102, 0.0f },
    { 0x60, 20480, 0.0f },
    { 0x48, 25, 0.0f },
    { 0x3E, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8F, 4, 0.0f },
    { 0x8C, 101, 0.0f },
    { 0x8B, 115, 0.0f },
    { 0x8F, 0, 0.0f },
    { 0x8B, 101, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_LABGUYPOTION[13] = {
    { 0x00, 0, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x47, 82, 0.0f },
    { 0x5A, 0, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_IDLELABGUY[13] = {
    { 0x00, 0, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x5A, 0, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x41, 0, 14.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_STAMPEDER[11] = {
    { 0x00, 0, 0.0f },
    { 0x60, 32768, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x29, 0, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x47, 58, 0.0f },
    { 0x41, 0, 2.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_FISHY[8] = {
    { 0x00, 0, 0.0f },
    { 0x60, 20480, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x21, 0, 0.0f },
    { 0x47, 76, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_CRYSTAL[16] = {
    { 0x00, 0, 0.0f },
    { 0x61, 4, 0.0f },
    { 0x60, 16, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x04, 0, 0.25f },
    { 0x2F, 0, 0.0f },
    { 0x30, 655, 0.1f },
    { 0x55, 512, 0.0f },
    { 0x56, 64, 0.0f },
    { 0x2D, 655, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_FLYINGCRYSTAL[14] = {
    { 0x00, 0, 0.0f },
    { 0x61, 4, 0.0f },
    { 0x60, 16400, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x2F, 0, 0.0f },
    { 0x30, 655, 0.1f },
    { 0x55, 512, 0.0f },
    { 0x56, 64, 0.0f },
    { 0x2D, 655, 0.0f },
    { 0x34, 0, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_CLOCK[16] = {
    { 0x00, 0, 0.0f },
    { 0x61, 4, 0.0f },
    { 0x60, 32784, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x04, 0, 0.25f },
    { 0x2F, 0, 0.0f },
    { 0x30, 1310, 0.05f },
    { 0x55, 512, 0.0f },
    { 0x56, 64, 0.0f },
    { 0x2D, 655, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_FLYINGCLOCK[14] = {
    { 0x00, 0, 0.0f },
    { 0x61, 4, 0.0f },
    { 0x60, 49168, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x2F, 0, 0.0f },
    { 0x30, 655, 0.1f },
    { 0x55, 512, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x56, 64, 0.0f },
    { 0x2D, 655, 0.0f },
    { 0x34, 0, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_CRATEGEM[27] = {
    { 0x00, 0, 0.0f },
    { 0x61, 4, 0.0f },
    { 0x60, 16, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x04, 0, 0.5f },
    { 0x2F, 0, 0.0f },
    { 0x56, 64, 0.0f },
    { 0x2D, 655, 0.0f },
    { 0x5B, 0, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x74, 10, 5.0f },
    { 0x8A, 0, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x74, 11, 5.0f },
    { 0x78, 97, 0.0f },
    { 0x75, 14, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x5C, 0, 0.0f },
    { 0x30, 655, 0.1f },
    { 0x55, 512, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_FLYINGCRATEGEM[25] = {
    { 0x00, 0, 0.0f },
    { 0x61, 4, 0.0f },
    { 0x60, 16400, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x2F, 0, 0.0f },
    { 0x56, 64, 0.0f },
    { 0x2D, 655, 0.0f },
    { 0x34, 0, 0.0f },
    { 0x5B, 0, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x74, 10, 5.0f },
    { 0x8A, 0, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x74, 11, 5.0f },
    { 0x78, 97, 0.0f },
    { 0x75, 14, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x5C, 0, 0.0f },
    { 0x30, 655, 0.1f },
    { 0x55, 512, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_GEM[16] = {
    { 0x00, 0, 0.0f },
    { 0x61, 4, 0.0f },
    { 0x60, 16, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x04, 0, 0.5f },
    { 0x2F, 0, 0.0f },
    { 0x30, 655, 0.1f },
    { 0x55, 512, 0.0f },
    { 0x56, 64, 0.0f },
    { 0x2D, 655, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_FLYINGGEM[14] = {
    { 0x00, 0, 0.0f },
    { 0x61, 4, 0.0f },
    { 0x60, 16400, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x2F, 0, 0.0f },
    { 0x30, 655, 0.1f },
    { 0x55, 512, 0.0f },
    { 0x56, 64, 0.0f },
    { 0x2D, 655, 0.0f },
    { 0x34, 0, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_ICON[16] = {
    { 0x00, 0, 0.0f },
    { 0x61, 4, 0.0f },
    { 0x60, 16, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x04, 0, 0.5f },
    { 0x2F, 0, 0.0f },
    { 0x30, 655, 0.1f },
    { 0x55, 512, 0.0f },
    { 0x56, 64, 0.0f },
    { 0x2D, 655, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_COCO[13] = {
    { 0x00, 0, 0.0f },
    { 0x60, 64, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x49, 34, 0.0f },
    { 0x4A, 1, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_PROBE[16] = {
    { 0x00, 0, 0.0f },
    { 0x61, 4, 0.0f },
    { 0x60, 4096, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x04, 0, 1.6f },
    { 0x2F, 0, 0.0f },
    { 0x30, 1310, 0.05f },
    { 0x55, 512, 0.0f },
    { 0x56, 64, 0.0f },
    { 0x2D, 163, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_FLYINGPROBE[14] = {
    { 0x00, 0, 0.0f },
    { 0x61, 4, 0.0f },
    { 0x60, 20480, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x2F, 0, 0.0f },
    { 0x30, 1310, 0.05f },
    { 0x55, 512, 0.0f },
    { 0x34, 0, 0.0f },
    { 0x56, 64, 0.0f },
    { 0x2D, 163, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_EARTHCRUNCH[27] = {
    { 0x00, 0, 0.0f },
    { 0x60, 4128, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x23, 0, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x56, 4, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x3C, 0, 3.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 43, 0.0f },
    { 0x3E, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 25, 0.0f },
    { 0x3E, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 53, 0.0f },
    { 0x3E, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_ATLASCRUNCH[21] = {
    { 0x00, 0, 0.0f },
    { 0x60, 4128, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x23, 0, 0.0f },
    { 0x47, 24, 0.0f },
    { 0x56, 4, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x37, 1, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x48, 24, 0.0f },
    { 0x3E, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 31, 0.0f },
    { 0x3E, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 97, 0.0f },
    { 0x90, 0, 0.0f },
};
struct nuvec_s punchpos1[4] = {
    { -10.0f, -0.7f, -14.25f },
    { -6.0f, -0.7f, -14.25f },
    { -2.0f, -0.7f, -14.25f },
    { 2.0f, -0.7f, -14.25f },
};
struct nuvec_s punchpos2[4] = {
    { 6.5f, -0.7f, -10.5f },
    { 6.5f, -0.7f, -6.5f },
    { 6.5f, -0.7f, -2.5f },
    { 6.5f, -0.7f, 2.5f },
};
static struct creatcmd_s CMD_WATERCRUNCH[214] = {
    { 0x00, 0, 0.0f },
    { 0x60, 299040, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x73, 0, 1.5f },
    { 0x38, 510, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x2A, 32768, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x3C, 0, 1.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 44, 0.0f },
    { 0x3F, 0, 11.0f },
    { 0x8A, 0, 0.0f },
    { 0x68, 47, 0.0f },
    { 0x09, 1, 0.0f },
    { 0x55, 2049, 3.0f },
    { 0x3F, 0, 27.0f },
    { 0x8A, 0, 0.0f },
    { 0x5D, (nusintptr)&DrainDamage_Intro, 0.0f },
    { 0x6C, 0, 0.3f },
    { 0x65, 32766, 0.0f },
    { 0x68, 17, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x40, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x61, 4096, 0.0f },
    { 0x8C, 115, 0.0f },
    { 0x8B, 49, 0.0f },
    { 0x59, 0, 1.0f },
    { 0x8B, 115, 0.0f },
    { 0x48, 22, 0.0f },
    { 0x3F, 0, 49.0f },
    { 0x74, 18, 0.0f },
    { 0x78, 97, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x85, (nusintptr)WaterCrunchFunction_Attack, 0.0f },
    { 0x62, 13, 0.0f },
    { 0x6B, 3, 0.0f },
    { 0x40, 0, 0.0f },
    { 0x74, 18, 0.0f },
    { 0x78, 97, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 49, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x68, 55, 0.0f },
    { 0x68, 206, 0.0f },
    { 0x60, 4096, 0.0f },
    { 0x48, 14, 0.0f },
    { 0x3F, 0, 15.0f },
    { 0x8A, 0, 0.0f },
    { 0x85, (nusintptr)WaterCrunchFunction_Spladoosh, 0.0f },
    { 0x61, 4096, 0.0f },
    { 0x01, 2, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x22, 1, 0.0f },
    { 0x8B, 50, 0.0f },
    { 0x48, 22, 0.0f },
    { 0x3F, 0, 49.0f },
    { 0x74, 18, 0.0f },
    { 0x78, 98, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x85, (nusintptr)WaterCrunchFunction_Attack, 0.0f },
    { 0x62, 13, 0.0f },
    { 0x6B, 3, 0.0f },
    { 0x40, 0, 0.0f },
    { 0x74, 18, 0.0f },
    { 0x78, 98, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 88, 0.0f },
    { 0x3F, 0, 45.0f },
    { 0x74, 18, 0.0f },
    { 0x78, 98, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x85, (nusintptr)WaterCrunchFunction_Attack, 0.0f },
    { 0x62, 12, 0.0f },
    { 0x6B, 2, 0.0f },
    { 0x40, 0, 0.0f },
    { 0x74, 18, 0.0f },
    { 0x78, 98, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 50, 0.0f },
    { 0x8B, 98, 0.0f },
    { 0x68, 55, 0.0f },
    { 0x68, 206, 0.0f },
    { 0x60, 4096, 0.0f },
    { 0x48, 14, 0.0f },
    { 0x3F, 0, 15.0f },
    { 0x8A, 0, 0.0f },
    { 0x85, (nusintptr)WaterCrunchFunction_Spladoosh, 0.0f },
    { 0x61, 4096, 0.0f },
    { 0x01, 4, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x22, 3, 0.0f },
    { 0x8B, 51, 0.0f },
    { 0x48, 22, 0.0f },
    { 0x3F, 0, 49.0f },
    { 0x74, 18, 0.0f },
    { 0x78, 99, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x85, (nusintptr)WaterCrunchFunction_Attack, 0.0f },
    { 0x62, 13, 0.0f },
    { 0x6B, 3, 0.0f },
    { 0x40, 0, 0.0f },
    { 0x74, 18, 0.0f },
    { 0x78, 99, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 88, 0.0f },
    { 0x3F, 0, 45.0f },
    { 0x74, 18, 0.0f },
    { 0x78, 99, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x85, (nusintptr)WaterCrunchFunction_Attack, 0.0f },
    { 0x62, 12, 0.0f },
    { 0x6B, 2, 0.0f },
    { 0x40, 0, 0.0f },
    { 0x74, 18, 0.0f },
    { 0x78, 99, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 55, 0.0f },
    { 0x3F, 0, 27.0f },
    { 0x74, 18, 0.0f },
    { 0x78, 99, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x85, (nusintptr)WaterCrunchFunction_Punch1, 0.0f },
    { 0x6C, 0, 0.5f },
    { 0x40, 0, 0.0f },
    { 0x74, 18, 0.0f },
    { 0x78, 99, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 51, 0.0f },
    { 0x8B, 99, 0.0f },
    { 0x68, 55, 0.0f },
    { 0x68, 206, 0.0f },
    { 0x60, 4096, 0.0f },
    { 0x48, 14, 0.0f },
    { 0x3F, 0, 15.0f },
    { 0x8A, 0, 0.0f },
    { 0x85, (nusintptr)WaterCrunchFunction_Spladoosh, 0.0f },
    { 0x61, 4096, 0.0f },
    { 0x01, 6, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x35, 0, 0.0f },
    { 0x36, 0, 0.0f },
    { 0x03, 0, 0.0f },
    { 0x22, 5, 0.0f },
    { 0x8B, 52, 0.0f },
    { 0x48, 22, 0.0f },
    { 0x3F, 0, 49.0f },
    { 0x74, 18, 0.0f },
    { 0x78, 100, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x85, (nusintptr)WaterCrunchFunction_Attack, 0.0f },
    { 0x62, 13, 0.0f },
    { 0x6B, 3, 0.0f },
    { 0x40, 0, 0.0f },
    { 0x74, 18, 0.0f },
    { 0x78, 100, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 55, 0.0f },
    { 0x3F, 0, 27.0f },
    { 0x74, 18, 0.0f },
    { 0x78, 100, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x85, (nusintptr)WaterCrunchFunction_Punch2, 0.0f },
    { 0x6C, 0, 0.5f },
    { 0x40, 0, 0.0f },
    { 0x74, 18, 0.0f },
    { 0x78, 100, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 88, 0.0f },
    { 0x3F, 0, 45.0f },
    { 0x74, 18, 0.0f },
    { 0x78, 100, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x85, (nusintptr)WaterCrunchFunction_Attack, 0.0f },
    { 0x62, 12, 0.0f },
    { 0x6B, 2, 0.0f },
    { 0x40, 0, 0.0f },
    { 0x74, 18, 0.0f },
    { 0x78, 100, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 55, 0.0f },
    { 0x3F, 0, 27.0f },
    { 0x74, 18, 0.0f },
    { 0x78, 100, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x85, (nusintptr)WaterCrunchFunction_Punch2, 0.0f },
    { 0x6C, 0, 0.5f },
    { 0x40, 0, 0.0f },
    { 0x74, 18, 0.0f },
    { 0x78, 100, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 52, 0.0f },
    { 0x8B, 100, 0.0f },
    { 0x68, 55, 0.0f },
    { 0x68, 206, 0.0f },
    { 0x60, 4096, 0.0f },
    { 0x48, 14, 0.0f },
    { 0x3F, 0, 15.0f },
    { 0x8A, 0, 0.0f },
    { 0x85, (nusintptr)WaterCrunchFunction_Spladoosh, 0.0f },
    { 0x85, (nusintptr)WaterCrunchFunction_Defeated, 1.0f },
    { 0x8F, 7, 1.0f },
    { 0x90, 0, 0.0f },
};
static struct nuvec_s crunchtime_arena_midpos = { 0.0f, 0.0f, 4.14f };
static struct nuvec_s water_attack_pos[4] = {
    { -3.75f, 2.25f, 3.23f },
    { 3.75f, 2.25f, 3.23f },
    { -2.3f, 1.68f, 1.73f },
    { 2.3f, 1.68f, 1.73f },
};
static struct nuvec_s fire_backpos[5] = {
    { -4.02f, 0.0f, 4.23f },
    { -2.27f, 0.0f, 5.44f },
    { 0.0f, 0.0f, 5.88f },
    { 2.27f, 0.0f, 5.44f },
    { 4.02f, 0.0f, 4.23f },
};
static struct nuvec_s fire_frontpos[5] = {
    { -1.46f, 0.0f, 1.6f },
    { -0.83f, 0.0f, 2.0f },
    { 0.0f, 0.0f, 2.18f },
    { 0.83f, 0.0f, 2.0f },
    { 1.46f, 0.0f, 1.6f },
};
static struct nuvec_s weather_srcpos[9] = {
    { -4.62f, 1.11f, 4.68f },
    { -3.67f, 1.11f, 5.47f },
    { -2.52f, 1.11f, 6.09f },
    { -1.28f, 1.11f, 6.47f },
    { 0.0f, 1.11f, 6.59f },
    { 1.28f, 1.11f, 6.47f },
    { 2.52f, 1.11f, 6.09f },
    { 3.67f, 1.11f, 5.47f },
    { 4.62f, 1.11f, 4.68f },
};
static struct nuvec_s weather_dstpos[9] = {
    { -1.45f, 0.25f, 1.48f },
    { -1.16f, 0.25f, 1.72f },
    { -0.79f, 0.25f, 1.91f },
    { -0.41f, 0.25f, 2.04f },
    { 0.0f, 0.25f, 2.07f },
    { 0.41f, 0.25f, 2.04f },
    { 0.79f, 0.25f, 1.91f },
    { 1.16f, 0.25f, 1.72f },
    { 1.45f, 0.25f, 1.48f },
};
static struct nuvec_s space_buttonpos[4] = {
    { -4.90326f, 1.17723f, 7.31819f },
    { -1.71111f, 1.17723f, 8.61123f },
    { 1.70436f, 1.17723f, 8.57616f },
    { 4.87776f, 1.17723f, 7.26821f },
};
static struct creatcmd_s CMD_SPACECRUNCH[112] = {
    { 0x00, 0, 0.0f },
    { 0x73, 0, 0.77f },
    { 0x60, 4096, 0.0f },
    { 0x5D, (nusintptr)&SpaceCrunch_Punch, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x01, 5, 0.0f },
    { 0x23, 0, 0.0f },
    { 0x3C, 0, 1.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 44, 0.0f },
    { 0x3F, 0, 11.0f },
    { 0x8A, 0, 0.0f },
    { 0x68, 47, 0.0f },
    { 0x09, 0, 0.0f },
    { 0x55, 2049, 3.0f },
    { 0x56, 4112, 0.0f },
    { 0x57, 1, 0.0f },
    { 0x3F, 0, 27.0f },
    { 0x8A, 0, 0.0f },
    { 0x5D, (nusintptr)&CrunchTime_Intro, 0.0f },
    { 0x6C, 0, 0.3f },
    { 0x68, 17, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x40, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x5E, 0, 0.0f },
    { 0x28, 0, 0.0f },
    { 0x56, 4112, 0.0f },
    { 0x3C, 0, 1.0f },
    { 0x8A, 0, 0.0f },
    { 0x8B, 99, 0.0f },
    { 0x85, (nusintptr)SpaceCrunchFunction_NewCount, 0.0f },
    { 0x8B, 100, 0.0f },
    { 0x85, (nusintptr)SpaceCrunchFunction_NewPoint, 0.0f },
    { 0x7F, 44, 0.0f },
    { 0x8D, 106, 0.0f },
    { 0x3F, 0, 12.0f },
    { 0x8A, 0, 0.0f },
    { 0x68, 47, 0.0f },
    { 0x55, 2049, 2.0f },
    { 0x57, 1, 0.0f },
    { 0x40, 0, 29.0f },
    { 0x8A, 0, 0.0f },
    { 0x6C, 0, 0.3f },
    { 0x68, 17, 0.0f },
    { 0x68, 130, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x40, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 119, 0.0f },
    { 0x8B, 97, 0.0f },
    { 0x48, 73, 0.0f },
    { 0x3E, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x85, (nusintptr)SpaceCrunchFunction_AttackCortex, 0.0f },
    { 0x8B, 106, 0.0f },
    { 0x3F, 0, 11.0f },
    { 0x8A, 0, 0.0f },
    { 0x68, 47, 0.0f },
    { 0x55, 2049, 3.0f },
    { 0x57, 1, 0.0f },
    { 0x3F, 0, 27.0f },
    { 0x8A, 0, 0.0f },
    { 0x6C, 0, 0.3f },
    { 0x68, 17, 0.0f },
    { 0x80, 0, 0.0f },
    { 0x8D, 120, 0.0f },
    { 0x68, 130, 0.0f },
    { 0x8B, 120, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x40, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x80, 0, 0.0f },
    { 0x8E, 119, 0.0f },
    { 0x85, (nusintptr)SpaceCrunchFunction_PunchCortex, 0.0f },
    { 0x48, 0, 0.0f },
    { 0x3E, 0, 1.0f },
    { 0x8A, 0, 0.0f },
    { 0x5D, (nusintptr)&SpaceCrunch_Punch, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x3C, 0, 3.0f },
    { 0x8A, 0, 0.0f },
    { 0x83, (nusintptr)&SpaceCortex_Defeated, 1.0f },
    { 0x8D, 102, 0.0f },
    { 0x76, (nusintptr)SpaceCrunchFunction_CheckCortexBack, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 99, 0.0f },
    { 0x8B, 119, 0.0f },
    { 0x83, (nusintptr)&SpaceCortex_Defeated, 1.0f },
    { 0x8D, 102, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x3C, 0, 3.0f },
    { 0x8A, 0, 0.0f },
    { 0x1A, 0, 0.0f },
    { 0x81, 0, 0.0f },
    { 0x8E, 100, 0.0f },
    { 0x48, 115, 0.0f },
    { 0x74, 19, 0.0f },
    { 0x78, 97, 0.0f },
    { 0x3E, 0, 3.0f },
    { 0x8A, 0, 0.0f },
    { 0x8C, 99, 0.0f },
    { 0x8B, 102, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_SPACECORTEX[79] = {
    { 0x00, 0, 0.0f },
    { 0x60, 266272, 0.0f },
    { 0x5D, (nusintptr)&SpaceCortex_Defeated, 0.0f },
    { 0x1B, 4, 0.0f },
    { 0x37, 0, 0.0f },
    { 0x38, 510, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x28, 0, 0.0f },
    { 0x56, 4112, 0.0f },
    { 0x3C, 0, 3.0f },
    { 0x8A, 0, 0.0f },
    { 0x8B, 116, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x76, (nusintptr)SpaceCortexFunction_CheckPunch, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x5D, (nusintptr)&SpaceCortex_Back, 0.0f },
    { 0x09, 1, 0.0f },
    { 0x3F, 0, 44.0f },
    { 0x8A, 0, 0.0f },
    { 0x55, 1, 0.0f },
    { 0x57, 1, 0.0f },
    { 0x40, 0, 79.0f },
    { 0x8A, 0, 0.0f },
    { 0x68, 17, 0.0f },
    { 0x6C, 0, 0.15f },
    { 0x40, 0, 99.0f },
    { 0x8A, 0, 0.0f },
    { 0x61, 4096, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x74, 18, 0.0f },
    { 0x78, 104, 0.0f },
    { 0x40, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x60, 4096, 0.0f },
    { 0x8C, 102, 0.0f },
    { 0x8B, 104, 0.0f },
    { 0x85, (nusintptr)SpaceCortexFunction_TakeHit, 0.0f },
    { 0x60, 4096, 0.0f },
    { 0x48, 70, 0.0f },
    { 0x3E, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x1F, 0, 0.0f },
    { 0x82, 0, 0.0f },
    { 0x8D, 100, 0.0f },
    { 0x8B, 102, 0.0f },
    { 0x09, 0, 0.0f },
    { 0x0C, 0, 0.666f },
    { 0x48, 28, 0.0f },
    { 0x55, 1, 0.0f },
    { 0x57, 1, 0.0f },
    { 0x3E, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x09, 0, 0.0f },
    { 0x48, 34, 0.0f },
    { 0x55, 1, 0.0f },
    { 0x3C, 0, 0.333f },
    { 0x8A, 0, 0.0f },
    { 0x5D, (nusintptr)&SpaceCortex_Back, 1.0f },
    { 0x68, 17, 0.0f },
    { 0x55, 0, 0.0f },
    { 0x57, 0, 0.0f },
    { 0x8C, 116, 0.0f },
    { 0x8B, 100, 0.0f },
    { 0x09, 0, 0.0f },
    { 0x0C, 0, 2.0f },
    { 0x48, 28, 0.0f },
    { 0x55, 1, 0.0f },
    { 0x57, 1, 0.0f },
    { 0x3E, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x85, (nusintptr)SpaceCortexFunction_Defeated, 1.0f },
    { 0x56, 64, 0.0f },
    { 0x2D, 1310, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_SPACEROKKO[10] = {
    { 0x00, 0, 0.0f },
    { 0x73, 0, 2.0f },
    { 0x60, 20480, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x28, 0, 0.0f },
    { 0x56, 4112, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_SPACEWAWA[10] = {
    { 0x00, 0, 0.0f },
    { 0x73, 0, 2.0f },
    { 0x60, 20480, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x28, 0, 0.0f },
    { 0x56, 4112, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_SPACEPYRO[10] = {
    { 0x00, 0, 0.0f },
    { 0x73, 0, 2.0f },
    { 0x60, 20480, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x28, 0, 0.0f },
    { 0x56, 4112, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
static struct creatcmd_s CMD_SPACELOLO[10] = {
    { 0x00, 0, 0.0f },
    { 0x73, 0, 2.0f },
    { 0x60, 20480, 0.0f },
    { 0x01, 0, 0.0f },
    { 0x28, 0, 0.0f },
    { 0x56, 4112, 0.0f },
    { 0x47, 34, 0.0f },
    { 0x41, 0, 0.0f },
    { 0x8A, 0, 0.0f },
    { 0x90, 0, 0.0f },
};
struct aitype_s AIType[107] = {
    { 12, 2, CMD_PENGUINP, "penguin pooof", 0.0f },
    { 24, 2, CMD_CRAB, "crab", 0.0f },
    { 55, 2, CMD_SEAL, "seal", 3.0f },
    { 15, 1, CMD_PARROT, "parrot", 0.0f },
    { 56, 1, CMD_NARWHAL, "narwhal", 0.0f },
    { 17, 2, CMD_MONKEYBANANA, "monkey banana", 0.0f },
    { 57, 2, CMD_KNIGHT, "knight", 0.0f },
    { 58, 1, CMD_WIZARD, "wizard", 0.0f },
    { 60, 1, CMD_KOICARP, "koi carp", 0.0f },
    { 22, 2, CMD_TURTLE, "turtle", 0.0f },
    { 61, 2, CMD_NINJASTALK, "ninja stalk", 0.0f },
    { 61, 2, CMD_NINJASWORD, "ninja sword", 0.0f },
    { 62, 1, CMD_BAT, "bat", 0.0f },
    { 63, 2, CMD_GONGBONGER, "gong bonger", 0.0f },
    { 64, 2, CMD_FLAMINGO, "flamingo", 0.0f },
    { 5, 2, CMD_NGIN, "ngin", 0.0f },
    { 6, 2, CMD_NTROPY, "ntropy", 0.0f },
    { 80, 2, CMD_DINGODILEFLAME, "dingodile flame", 0.0f },
    { 80, 4, CMD_DINGODILEFIRE, "dingodile fire", 0.0f },
    { 21, 2, CMD_GORILLA, "gorilla", 0.0f },
    { 70, 2, CMD_MAMMOTH, "mammoth", 0.0f },
    { 82, 2, CMD_TINY, "tiny", 0.0f },
    { 97, 2, CMD_LIZARD, "lizard", 0.0f },
    { 97, 5, CMD_LIZARDAB, "lizard a-b", 30.0f },
    { 106, 2, CMD_RICKSHAW, "rick shaw", 0.0f },
    { 108, 1, CMD_TUNA, "tuna", 0.0f },
    { 109, 4, CMD_WHEELA, "wheela", 0.0f },
    { 110, 1, CMD_CACTUS, "cactus", 0.0f },
    { 35, 2, CMD_SCORPION, "scorpion", 0.0f },
    { 35, 2, CMD_ROOFSCORPION, "roof scorpion 2", 0.0f },
    { 35, 4, CMD_ROOFSCORPION, "roof scorpion 4", 0.0f },
    { 19, 2, CMD_SHARK, "shark", 0.0f },
    { 19, 2, CMD_DEEPSEASHARK, "deep sea shark", 0.0f },
    { 111, 1, CMD_DROID1, "droid1", 0.0f },
    { 112, 1, CMD_DROID2, "droid2", 0.0f },
    { 113, 2, CMD_DROID3, "droid3", 0.0f },
    { 114, 2, CMD_DROID4, "droid4 2", 0.0f },
    { 114, 4, CMD_DROID4, "droid4 4", 0.0f },
    { 26, 2, CMD_DONKEY, "donkey", 0.0f },
    { 126, 2, CMD_DIVER, "diver", 0.0f },
    { 128, 2, CMD_DYNO, "dyno", 0.0f },
    { 27, 2, CMD_EEL, "eel", 0.0f },
    { 30, 2, CMD_FISHSWIM, "lionfish", 0.0f },
    { 34, 2, CMD_FISHSWIM, "anglerfish", 0.0f },
    { 33, 3, CMD_VULTURE, "vulture", 0.0f },
    { 136, 1, CMD_JELLYFISH, "jellyfish", 0.0f },
    { 150, 2, CMD_WETJELLYFISH, "wet jellyfish", 3.0f },
    { 138, 2, CMD_PYGMY, "pygmy", 0.0f },
    { 146, 2, CMD_OCTOPUS, "octopus", 0.0f },
    { 147, 1, CMD_TENTACLE, "tentacle", 0.0f },
    { 14, 1, CMD_POLARGUARD, "polarguard", 0.0f },
    { 152, 2, CMD_SKIER, "skier", 0.0f },
    { 154, 1, CMD_MINE, "mine", 0.0f },
    { 154, 2, CMD_FALLINGMINE, "falling mine", 0.0f },
    { 155, 3, CMD_HARPOONIST, "harpoonist", 0.0f },
    { 12, 2, CMD_BIGPENGUINP, "big penguin", 0.0f },
    { 169, 3, CMD_LABGUYSHIELD, "labguy W shield", 0.0f },
    { 169, 2, CMD_LABGUYPOTION, "labguy W potion", 0.0f },
    { 170, 3, CMD_LABGUYSHIELD, "labguy B shield", 0.0f },
    { 170, 2, CMD_LABGUYPOTION, "labguy B potion", 0.0f },
    { 171, 2, CMD_IDLELABGUY, "nuclear lab guy", 0.0f },
    { 172, 2, CMD_IDLELABGUY, "cylon lab guy", 0.0f },
    { 45, 1, CMD_STAMPEDER, "elephant", 0.0f },
    { 44, 1, CMD_STAMPEDER, "giraffe", 0.0f },
    { 43, 1, CMD_STAMPEDER, "baby rhino", 0.0f },
    { 53, 1, CMD_STAMPEDER, "daddy rhino", 0.0f },
    { 28, 1, CMD_FISHY, "fishy", 0.0f },
    { 148, 5, CMD_MOUSE, "mouse", 0.0f },
    { 149, 1, CMD_ROBIN, "robin", 0.0f },
    { 20, 1, CMD_STARFISH, "starfish", 0.0f },
    { 23, 6, CMD_DRAGONFLY, "dragonfly", 0.0f },
    { 13, 1, CMD_SNOWHOG, "snowhog", 0.0f },
    { 48, 5, CMD_BIRD, "bird", 0.0f },
    { 50, 2, CMD_BUTTERFLY, "butterfly", 0.0f },
    { 51, 1, CMD_BEE, "bee", 0.0f },
    { 25, 1, CMD_CLAM, "clam", 0.0f },
    { 117, 1, CMD_CRYSTAL, "crystal", 0.0f },
    { 117, 1, CMD_FLYINGCRYSTAL, "flying crystal", 0.0f },
    { 118, 1, CMD_CLOCK, "clock", 0.0f },
    { 118, 1, CMD_FLYINGCLOCK, "flying clock", 0.0f },
    { 119, 1, CMD_CRATEGEM, "crate gem", 0.0f },
    { 119, 1, CMD_FLYINGCRATEGEM, "flying crategem", 0.0f },
    { 120, 1, CMD_GEM, "bonus gem", 0.0f },
    { 120, 1, CMD_FLYINGGEM, "flying bonusgem", 0.0f },
    { 121, 1, CMD_GEM, "red gem", 0.0f },
    { 122, 1, CMD_GEM, "green gem", 0.0f },
    { 123, 1, CMD_GEM, "blue gem", 0.0f },
    { 124, 1, CMD_GEM, "yellow gem", 0.0f },
    { 125, 1, CMD_GEM, "purple gem", 0.0f },
    { 162, 1, CMD_ICON, "bazooka", 0.0f },
    { 163, 1, CMD_ICON, "super slam", 0.0f },
    { 164, 1, CMD_ICON, "sprint", 0.0f },
    { 165, 1, CMD_ICON, "double jump", 0.0f },
    { 166, 1, CMD_ICON, "super spin", 0.0f },
    { 167, 1, CMD_ICON, "tiptoe", 0.0f },
    { 1, 3, CMD_COCO, "coco", 0.0f },
    { 177, 1, CMD_PROBE, "probe", 0.0f },
    { 177, 1, CMD_FLYINGPROBE, "flying probe", 0.0f },
    { 127, 1, CMD_EARTHCRUNCH, "earth crunch", 0.0f },
    { 173, 1, CMD_ATLASCRUNCH, "atlas crunch", 0.0f },
    { 127, 7, CMD_WATERCRUNCH, "water crunch", 0.0f },
    { 127, 6, CMD_SPACECRUNCH, "space crunch", 0.0f },
    { 2, 2, CMD_SPACECORTEX, "space cortex", 0.0f },
    { 85, 1, CMD_SPACEROKKO, "space rok-ko", 0.0f },
    { 87, 1, CMD_SPACEWAWA, "space wa-wa", 0.0f },
    { 86, 1, CMD_SPACEPYRO, "space py-ro", 0.0f },
    { 88, 1, CMD_SPACELOLO, "space lo-lo", 0.0f },
};
u8 LevelAIType[107] = { 0 };
struct aitab_s AITab[96] = { 0 };
struct nuvec_s ai_lookpos = { 0.0f, 0.0f, 0.0f };
u32 weather_attack_on[9] = { 0 };

/* Public inline as in retail: its code goes to the end of the unit, its
 * load-screen string to the front of .rodata. */
inline void InitAI(void)
{
    s32 i;

    LEVELAITYPES = 0;
    for (i = 0; i < 107; i++) {
        if (CRemap[AIType[i].character] != -1) {
            LevelAIType[LEVELAITYPES++] = i;
        }
    }
    LEVELAICOUNT = 0;
    LoadScreen("LoadAI...");
    if (Level != 40) {
        LoadAI();
    }
    ResetAI();
}

inline s32 FindAIType(char *name, s32 type)
{
    s32 i;

    for (i = 0; i < 107; i++) {
        if (strcmp(name, AIType[i].name) == 0) {
            if (type == AIType[i].points) {
                if (type < 9) {
                    return i;
                }
            }
        }
    }
    return -1;
}

void ResetAI(void)
{
    s32 n;
    s32 flag;
    struct aitab_s *ai;
    struct rail_s *rail;
    s32 irail;
    s32 j;

    flag = 0;
    if (Rail[7].type == 3) {
        flag = AheadOfCheckpoint(gempath_RPos.iRAIL, gempath_RPos.iALONG,
                                 gempath_RPos.fALONG) != 0;
    }

    ai = AITab;
    for (n = 0; n < LEVELAICOUNT; n++, ai++) {
        GetALONG(&ai->pos[0], 0, -1, -1, 1);
        ai->iRAIL = temp_iRAIL;
        ai->iALONG = temp_iALONG;
        ai->fALONG = temp_fALONG;

        irail = (s8)temp_iRAIL;
        rail = Rail;
        if (irail != -1 && rail[irail].type == 3) {
            if (flag) {
                goto active;
            }
        } else if (bonus_restart == 0 && AheadOfCheckpoint(ai->iRAIL, ai->iALONG, ai->fALONG)) {
        active:
            ai->status = 1;
        }
        ai->time = 0;
        ai->delay = AIType[ai->ai_type].delay;
        ai->origin = ai->pos[0];

        if (AIType[ai->ai_type].points >= 2) {
            for (j = 1; j < AIType[ai->ai_type].points; j++) {
                NuVecAdd(&ai->origin, &ai->origin, &ai->pos[j]);
            }
            NuVecScale(&ai->origin, &ai->origin,
                       1.0f / (f32)AIType[ai->ai_type].points);
        }
    }

    for (n = 1; n < 9; n++) {
        if (Character[n].used != 0) {
            RemoveGameObject(&Character[n].obj);
            Character[n].used = 0;
            Character[n].on = 0;
        }
    }
}


void FindGongBongerAnim(struct nuvec_s *pos, struct nuhspecial_s *out)
{
    struct nuhspecial_s buf[3];
    s32 i;

    memset(buf, 0, sizeof(buf));
    if (world_scene[0] != 0) {
        NuSpecialFind(world_scene[0], &buf[0], "gongbong1");
        NuSpecialFind(world_scene[0], &buf[1], "gongbong2");
        NuSpecialFind(world_scene[0], &buf[2], "gongbong3");
        for (i = 0; i < 3; i++) {
            if (buf[i].special != 0) {
                struct nuvec_s *np = (struct nuvec_s *)&buf[i].special->instance->mtx._30;
                f32 dx = np->x - pos->x;
                f32 dz = np->z - pos->z;
                if (dx * dx + dz * dz < 25.0f) {
                    *out = buf[i];
                    return;
                }
            }
        }
    }
}

s32 WaterCrunchFunction_Punch1(struct creature_s *c)
{
    struct debkeydatatype_s *deb;
    s32 key;
    s32 i;

    GameSfx(0xCB, &c->obj.pos);
    for (i = 0; i < 4; i++) {
        key = -1;
        AddFiniteShotDebrisEffect(&key, GDeb[141].i, &punchpos1[i], 1);
        DebrisEmitterOrientation(key, 0, (s16)c->obj.hdg);
        deb = &debkeydata[key];
        deb->delay = 30 - i * 10;
    }
    return 1;
}


s32 WaterCrunchFunction_Punch2(struct creature_s *c)
{
    struct debkeydatatype_s *deb;
    s32 key;
    s32 i;

    GameSfx(0xCB, &c->obj.pos);
    for (i = 0; i < 4; i++) {
        key = -1;
        AddFiniteShotDebrisEffect(&key, GDeb[141].i, &punchpos2[i], 1);
        DebrisEmitterOrientation(key, 0, (s16)c->obj.hdg);
        deb = &debkeydata[key];
        deb->delay = 30 - i * 10;
    }
    return 1;
}


s32 WaterCrunchFunction_Attack(struct creature_s *c)
{
    gamesfx_effect_volume = 0x7FFE;
    GameSfx(0x88, &c->obj.pos);
    NewRumble(&player->rumble, 0xBF);
    NewBuzz(&player->rumble, 0xA);
    return 1;
}

s32 WaterCrunchFunction_Defeated(void)
{
    boss_dead = (Game.hub[Hub].flags & 4) ? 2 : 1;
    return 1;
}

f32 DRAINWAWADY = 1.0f;
static s32 crunch_vulnerable = 0;
static f32 SPACEUKAHACKY = 0.333f;
static f32 SPACEUKADIST = 0.111f;
static f32 SPACEUKAFRAMEHACK = 150.0f;
s8 crunchtime_phase1tab[1] = { 1 };
s8 crunchtime_phase2tab[2] = { 2, 1 };
s8 crunchtime_phase3tab[4] = { 3, 1, 2, 3 };
s8 crunchtime_phase4tab[8] = { 4, 1, 3, 4, 2, 1, 3, 4 };
s32 LEVELAITYPES = 0;
s32 LEVELAICOUNT = 0;
s32 temp_creature_i = 0;
s32 DrainDamage_Intro = 0;
u32 earth_attack_on = 0;
u32 water_attack_on = 0;
u32 fire_attack_on = 0;
s32 CrunchTime_Intro = 0;
s32 SpaceCortex_Defeated = 0;

void UpdateDRAINDAMAGE(void)
{
    struct creature_s *c;
    struct nuvec_s pos;
    struct nuvec_s d;
    s32 model;
    u16 xrot;
    u16 yrot;

    FindNearestCreature(&player->obj.pos, 0x7F, 0);
    if (temp_creature_i != -1) {
        c = &Character[temp_creature_i];
    } else {
        c = 0;
    }
    drain_wawa_model = 0;
    model = CRemap[0x57];
    if (model != -1) {
        drain_wawa_model = &CModel[model];
        if (drain_wawa_model->anmdata[12] != 0 && drain_wawa_model->anmdata[13] != 0) {
            drain_ang[0] += 0x816;
            drain_ang[1] += 0x57C;
            drain_ang[2] += 0xAB0;
            if (c != 0 || drain_wawa_ok != 0) {
                if (c != 0) {
                    pos.x = c->obj.pos.x - NuTrigTable[c->obj.hdg] * 3.0f;
                    pos.y = c->obj.pos.y + DRAINWAWADY;
                    pos.z = c->obj.pos.z
                          - NuTrigTable[(u16)(c->obj.hdg + 0x4000)] * 3.0f;
                }
                NuVecSub(&d, &D_0058656C, &drain_wawa_pos);
                xrot = NuAtan2D(d.y, NuFsqrt(d.x * d.x + d.z * d.z));
                yrot = NuAtan2D(d.x, d.z);
                if (drain_wawa_ok == 0) {
                    drain_wawa_ok = 1;
                    drain_wawa_xrot = xrot;
                    drain_wawa_yrot = yrot;
                    drain_wawa_pos = pos;
                    ResetAnimPacket(&drain_wawa_anim, 0xC);
                    GameSfx(0x1E, &drain_wawa_pos);
                } else {
                    drain_wawa_xrot = SeekRot(drain_wawa_xrot, xrot, 3);
                    drain_wawa_yrot = SeekRot(drain_wawa_yrot, yrot, 3);
                }
                drain_wawa_anim.oldaction = drain_wawa_anim.action;
                UpdateAnimPacket(&CModel[model], &drain_wawa_anim, 0.59999996f, 0.0f);
                if (drain_wawa_anim.flags & 1) {
                    if (drain_wawa_anim.action == 0xC) {
                        if (c == 0
                            || NuVecDistSqr(&drain_wawa_pos, &c->obj.pos, 0) > 25.0f) {
                            drain_wawa_anim.newaction = 0xD;
                            GameSfx(0x21, &drain_wawa_pos);
                        }
                    } else {
                        drain_wawa_ok = 0;
                    }
                }
            } else {
                drain_wawa_ok = 0;
            }
        } else {
            drain_wawa_model = 0;
        }
    }
}

void DrawDRAINDAMAGE(void)
{
    struct nuvec_s pos;

    if (drain_wawa_model != 0 && drain_wawa_ok != 0) {
        pos.x = drain_wawa_pos.x + NuTrigTable[(u16)drain_ang[0]] * 0.1f;
        pos.y = drain_wawa_pos.y + NuTrigTable[(u16)drain_ang[1]] * 0.1f;
        pos.z = drain_wawa_pos.z + NuTrigTable[(u16)drain_ang[2]] * 0.1f;
        Draw3DCharacter(&pos, drain_wawa_xrot, (drain_wawa_yrot - 0x8000) & 0xffff, 0,
                        drain_wawa_model, 3.0f, drain_wawa_anim.action,
                        drain_wawa_anim.anim_time, 0);
    }
}

s32 WaterCrunchFunction_Spladoosh(struct creature_s *c)
{
    struct nuvec_s v;
    s32 i;
    s32 r;

    GameSfx(0x47, &c->obj.pos);
    JudderGameCamera(&GameCam[0], 0.2f, 0);
    NewRumble(&player->rumble, 0x7F);
    NewBuzz(&player->rumble, 0xA);
    for (i = 0; i < 0x40; i++) {
        NuVecScale(&v, &v001, (f32)qrand() * 1.5259022e-05f);
        NuVecRotateY(&v, &v, qrand());
        r = qrand();
        v.y += c->obj.pos.y;
        if (r < 0x4000) {
            v.x += c->mtxLOCATOR[0][0]._30;
            v.z += c->mtxLOCATOR[0][0]._32;
        } else if (r < 0x8000) {
            v.x += c->mtxLOCATOR[0][1]._30;
            v.z += c->mtxLOCATOR[0][1]._32;
        } else {
            v.x += c->obj.pos.x;
            v.z += c->obj.pos.z;
        }
        v.x -= NuTrigTable[c->obj.hdg] * 2.0f;
        v.z -= NuTrigTable[(c->obj.hdg + 0x4000) & 0xFFFF] * 2.0f;
        AddGameDebrisRot(0x8E, &v, 1, 0, 0);
    }
    return 1;
}

extern struct objtab_s ObjTab[];
void AddProjectile(struct nuvec_s *pos, struct nuvec_s *target, struct nuvec_s *mom, s32 type,
                   u16 hdg, struct obj_s *owner);
struct deb3_s *AddDeb3(struct nuvec_s *pos, s32 db, s32 emit,
                       struct nuangvec_s *angle);
s32 KillPlayer(struct obj_s *obj, s32 anim);
void ProcessVehMasks(void);


/* Public inline ahead of UpdateCRUNCHTIME: its mask offset constant is
 * emitted before UpdateCRUNCHTIME's, as in retail. */
inline void ResetCRUNCHTIME(void)
{
    s32 i;
    struct nuvec_s v;

    jcrunch = 0;
    crunch_vulnerable = 0;
    crunchtime_attack_phase = 0;
    earth_attack_on = 0;
    earth_attack_wait = 0;
    water_attack_on = 0;
    water_attack_wait = 0;
    water_last_chute = -1;
    fire_attack_on = -1;
    fire_attack_wait = 0;
    for (i = 8; i >= 0; i--) {
        weather_attack_on[i] = 0;
    }
    weather_attack_wait = 0;
    CrunchTime_Intro = 1;

    InitVehMasks();
    InitVehMask(0, 3);
    v = (struct nuvec_s){0.0f, 5.0f, 0.0f};
    SetNewMaskStuff(0, &crunchtime_arena_midpos, &v, 0.0f, -540.0f, 1.0f,
                    0, 0, 1.0f, 0.0f);
}

s32 UpdateCRUNCHTIME(void)
{
    struct nuvec_s pos[2];
    struct nuvec_s v;
    struct nuvec_s plrmid;
    struct nuangvec_s ang2;
    struct nuangvec_s ang;
    struct creature_s *crunch;
    struct creature_s *cortex;
    struct nuvec_s *p0;
    struct nuvec_s *p1;
    struct nuvec_s *p2;
    struct nuvec_s *p3;
    struct nuvec_s *path;
    struct nuvec_s *path2;
    f32 f;
    s32 i;
    s32 j;
    s32 active;
    s32 kill;
    s32 extra_time;
    s32 old;
    u16 yrot;
    double dd;

    jcrunch = 0;
    FindNearestCreature(&player->obj.pos, 0x7F, 0);
    if (temp_creature_i != -1) {
        crunch = &Character[temp_creature_i];
    } else {
        crunch = 0;
    }
    FindNearestCreature(&player->obj.pos, 2, 0);
    if (temp_creature_i != -1) {
        cortex = &Character[temp_creature_i];
    } else {
        cortex = 0;
    }
    plrmid.x = player->obj.pos.x;
    plrmid.y = player->obj.pos.y
           + (player->obj.bot + player->obj.top) * player->obj.SCALE * 0.5f;
    plrmid.z = player->obj.pos.z;
    spaceuka_angle[0] += 0x57C;
    spaceuka_angle[1] += 0x804;
    spaceuka_angle[2] += 0xA20;
    crunchtime_attack_phase = (cortex != 0) ? 4 - cortex->ai.hits : 0;
    extra_time = 5;

    if (crunch != 0 && (u32)crunchtime_attack_phase < 4
        && crunch->ai.i0 > 0 && crunch->ai.i0 < 5
        && crunch->ai.terflags == 0 && crunch->ai.movflags == 0
        && (path = AITab[crunch->i_aitab].pos,
            NuVecDistSqr(&crunch->obj.pos, &path[crunch->ai.i0], 0) < 1.0f)
        && crunch->obj.anim.newaction != 0x73
        && crunch->obj.anim.newaction != 0x49
        && !(crunch->obj.anim.action == 0x2C && crunch->obj.anim.anim_time < 15.0f)
        && !(crunch->obj.anim.newaction == 0x2C && crunch->obj.anim.blend != 0)) {
        jcrunch = crunch->ai.i0;
    } else {
        jcrunch = 0;
    }

    j = -1;
    if (crunch != 0) {
        for (i = 0; i < 4; i++) {
            path2 = AITab[crunch->i_aitab].pos;
            if (NuVecDistSqr(&crunch->obj.pos, &path2[i + 1], 0) < 0.5f) {
                j = i;
                i = 4;
            }
        }
    }
    for (i = 0; i < 4; i++) {
        if (ObjTab[181 + i].obj.special != 0) {
            *(struct nuvec_s *)&ObjTab[181 + i].obj.special->instance->mtx._30 = space_buttonpos[i];
            if (i == j) {
                ObjTab[181 + i].obj.special->instance->mtx._31 -= 0.125f;
            }
        }
    }

    if (crunch != 0 && crunch->ai.i0 == 1 && crunch->obj.anim.newaction == 0x22
        && (u32)crunchtime_attack_phase < 4) {
        earth_attack_on = extra_time;
    }
    if (earth_attack_on != 0) {
        earth_attack_on--;
    }
    if (earth_attack_wait != 0) {
        earth_attack_wait--;
    }
    if (earth_attack_wait == 0 && earth_attack_on != 0) {
        AddProjectile(0, 0, 0, 0xE, qrand(), 0);
        earth_attack_wait = 0x19;
    }

    if (crunch != 0 && crunch->ai.i0 == 2 && crunch->obj.anim.newaction == 0x22
        && (u32)crunchtime_attack_phase < 4) {
        water_attack_on = extra_time;
    }
    if (water_attack_on != 0) {
        water_attack_on--;
    }
    if (water_attack_wait != 0) {
        water_attack_wait--;
    }
    if (water_attack_wait == 0 && water_attack_on != 0) {
        if ((u32)water_last_chute < 2) {
            water_last_chute = qrand() / 0x8000 + 2;
        } else {
            water_last_chute = qrand() / 0x8000;
        }
        p0 = &water_attack_pos[water_last_chute];
        ang.x = 0;
        ang.y = NuAtan2D(crunchtime_arena_midpos.x - p0->x,
                         crunchtime_arena_midpos.z - p0->z);
        ang.z = 0;
        ang2 = ang;
        AddDeb3(p0, 0x11, 5, &ang2);
        GameSfx(0x48, p0);
        water_attack_wait = 0x3E;
    }

    active = 0;
    if (crunch != 0 && crunch->ai.i0 == 3 && crunch->obj.anim.newaction == 0x22
        && (u32)crunch->ai.i0 >= (u32)crunchtime_attack_phase) {
        active = 1;
    }
    if (fire_attack_wait != 0) {
        fire_attack_wait--;
        if (fire_attack_wait == 0x32) {
            GameSfx(0x4D, 0);
        }
    }
    if (active && fire_attack_wait == 0) {
        do {
            j = qrand() / 0x4000;
        } while (j == fire_attack_on);
        fire_attack_on = j;
        fire_attack_wait = 100;
        GameSfx(0x85, 0);
    }
    if (fire_attack_wait != 0) {
        p0 = &fire_backpos[fire_attack_on];
        p1 = &fire_backpos[fire_attack_on + 1];
        p2 = &fire_frontpos[fire_attack_on + 1];
        p3 = &fire_frontpos[fire_attack_on];
        f = qrand() * 1.5259022e-05f;
        pos[0].x = p0->x + (p1->x - p0->x) * f;
        pos[0].y = p0->y + (p1->y - p0->y) * f;
        pos[0].z = p0->z + (p1->z - p0->z) * f;
        pos[1].x = p3->x + (p2->x - p3->x) * f;
        pos[1].y = p3->y + (p2->y - p3->y) * f;
        pos[1].z = p3->z + (p2->z - p3->z) * f;
        f = qrand() * 1.5259022e-05f;
        v.x = pos[1].x + (pos[0].x - pos[1].x) * f;
        v.y = pos[1].y + (pos[0].y - pos[1].y) * f;
        v.z = pos[1].z + (pos[0].z - pos[1].z) * f;
        if ((u32)fire_attack_wait >= 0x32) {
            for (i = 0; i < 2; i++) {
                AddGameDebrisRot(0x75, &v, 1, 0, 0);
            }
        } else if (fire_attack_wait != 0) {
            for (i = 0; i < 4; i++) {
                AddGameDebrisRot(0x76, &v, 1, 0, 0);
            }
            if (player->obj.pos.y + player->obj.bot * player->obj.SCALE
                < crunchtime_arena_midpos.y + 1.0f
                && (player->obj.pos.x - p0->x) * (p1->z - p0->z)
                   + (player->obj.pos.z - p0->z) * (p0->x - p1->x) >= 0.0f
                && (player->obj.pos.x - p1->x) * (p2->z - p1->z)
                   + (player->obj.pos.z - p1->z) * (p1->x - p2->x) >= 0.0f
                && (player->obj.pos.x - p2->x) * (p3->z - p2->z)
                   + (player->obj.pos.z - p2->z) * (p2->x - p3->x) >= 0.0f
                && (player->obj.pos.x - p3->x) * (p0->z - p3->z)
                   + (player->obj.pos.z - p3->z) * (p3->x - p0->x) >= 0.0f) {
                KillPlayer(&player->obj, 0xD);
            }
        }
    }

    if (crunch != 0 && crunch->ai.i0 == 4 && crunch->obj.anim.newaction == 0x22
        && (u32)crunchtime_attack_phase < 4 && weather_attack_wait == 0) {
        do {
            j = qrand() / 0x1C72;
        } while (weather_attack_on[j] != 0);
        weather_attack_on[j] = 100;
        weather_attack_wait = 0x21;
        GameSfx(0x32, &weather_dstpos[j]);
    }
    kill = 0;
    for (j = 0; j < 9; j++) {
        if (weather_attack_on[j] != 0) {
            weather_attack_on[j]--;
            if (weather_attack_on[j] == 0x32) {
                gamesfx_effect_volume = 0xBFFD;
                GameSfx(0x4B, &weather_dstpos[j]);
            }
            if (weather_attack_on[j] != 0) {
                AddGameDebrisRot(0x78, &weather_srcpos[j], 1, 0, 0);
                AddGameDebrisRot(0x78, &weather_dstpos[j], 1, 0, 0);
            }
        }
        if ((u32)(weather_attack_on[j] - 1) < 0x31) {
            if (NuVecDistSqr(&plrmid, &weather_srcpos[j], 0) < 0.110889f
                || NuVecDistSqr(&plrmid, &weather_dstpos[j], 0) < 0.110889f) {
                kill = 1;
            } else {
                yrot = NuAtan2D(weather_srcpos[j].x - weather_dstpos[j].x,
                                weather_srcpos[j].z - weather_dstpos[j].z);
                NuVecSub(&pos[1], &plrmid, &weather_dstpos[j]);
                NuVecRotateY(&pos[1], &pos[1], -yrot);
                dd = pos[1].x;
                if (dd < 0.0) {
                    dd = 0.0 - dd;
                }
                if (dd < 0.333f) {
                    kill = 1;
                }
            }
        }
    }
    if (weather_attack_wait != 0) {
        weather_attack_wait--;
    }
    if (kill) {
        KillPlayer(&player->obj, 0xD);
    }

    if (cortex != 0) {
        v.x = cortex->obj.pos.x - NuTrigTable[cortex->obj.hdg] * SPACEUKADIST;
        v.z = cortex->obj.pos.z
            - NuTrigTable[(cortex->obj.hdg + 0x4000) & 0xFFFF] * SPACEUKADIST;
        v.y = cortex->obj.pos.y + cortex->obj.top * cortex->obj.SCALE + SPACEUKAHACKY;
        if (CrunchTime_Intro != 0
            || (cortex->obj.anim.newaction != 0x1C
                && cortex->obj.anim.newaction != 0x46
                && !(cortex->obj.anim.action == 0x49
                     && SPACEUKAFRAMEHACK < cortex->obj.anim.anim_time))) {
            v.y += 2.0f;
        }
        f = 0.039960004f;
        v.x += NuTrigTable[spaceuka_angle[0]] * 0.1f;
        v.y += NuTrigTable[spaceuka_angle[1]] * 0.1f;
        v.z += NuTrigTable[spaceuka_angle[2]] * 0.1f;
        space_ukapos.x += (v.x - space_ukapos.x) * f;
        space_ukapos.y += (v.y - space_ukapos.y) * f;
        space_ukapos.z += (v.z - space_ukapos.z) * f;
    }

    old = crunch_vulnerable;
    crunch_vulnerable = (crunch != 0 && crunch->obj.anim.action == 0x73);
    if (crunch_vulnerable != old) {
        if (crunch_vulnerable != 0) {
            v = (struct nuvec_s){0.0f, 1.0f, 0.0f};
            SetNewMaskStuff(0, &crunch->obj.pos, &v, 1.0f, -540.0f, 1.0f,
                            0, 0, 1.0f, 0.0f);
        } else {
            v = (struct nuvec_s){0.0f, 5.0f, 0.0f};
            SetNewMaskStuff(0, &crunchtime_arena_midpos, &v, 0.0f, -540.0f, 1.0f,
                            0, 0, 1.0f, 0.0f);
        }
    }
    ProcessVehMasks();

    if (jcrunch == 1) {
        return 0xC8;
    }
    if (jcrunch == 2) {
        return 0xCB;
    }
    if (jcrunch != 3) {
        return (jcrunch != 4) ? 0xC7 : 0xC9;
    }
    return 0xCA;
}

extern s32 Paused;
void DrawVehMasks(void);

void DrawCRUNCHTIME(void)
{
    struct nuvec_s mid;
    s32 i;
    s32 a;
    u32 col;

    for (i = 0; i < 9; i++) {
        if ((u32)weather_attack_on[i] >= 0x32) {
            continue;
        }
        if (weather_attack_on[i] != 0 && Paused == 0) {
            a = weather_attack_on[i];
            if (a >= 0x23) {
                a = (0x32 - a) << 4;
            } else {
                a = a << 4;
            }
            if (a > 0x80) {
                a = 0x80;
            }
            col = a << 24;
            NuVecAdd(&mid, &weather_srcpos[i], &weather_dstpos[i]);
            NuVecScale(&mid, &mid, 0.5f);
            NuLgtArcLaser(0, &weather_srcpos[i], &weather_dstpos[i], &mid,
                          0.1f, 0.2f, 0.01f, 1.65f, col + 0xFF7F3F);
            NuLgtArcLaser(0, &weather_srcpos[i], &weather_dstpos[i], &mid,
                          0.1f, 0.2f, 0.01f, 1.65f, col + 0xFF7F3F);
            NuLgtArcLaser(0, &weather_srcpos[i], &weather_dstpos[i], &mid,
                          0.4f, 0.3f, 0.001f, 1.65f, col | 0x800040);
        }
    }
    i = CRemap[4];
    if (i != -1) {
        Draw3DCharacter(&space_ukapos, 0, 0, 0, &CModel[i],
                        1.0f, -1, 1.0f, 0);
    }
    DrawVehMasks();
}


s32 SpaceCrunchFunction_NewPoint(struct creature_s *c, struct nuvec_s *pos)
{
    s32 old;

    old = c->ai.i0;
    switch ((u32)crunchtime_attack_phase) {
    case 0:
        c->ai.i0 = crunchtime_phase1tab[1 - c->ai.count];
        break;
    case 1:
        c->ai.i0 = crunchtime_phase2tab[2 - c->ai.count];
        break;
    case 2:
        c->ai.i0 = crunchtime_phase3tab[4 - c->ai.count];
        break;
    case 3:
        c->ai.i0 = crunchtime_phase4tab[8 - c->ai.count];
        break;
    default:
        do {
            c->ai.i0 = qrand() / 0x4000 + 1;
        } while (c->ai.i0 == old);
        break;
    }
    c->ai.newpos = pos[c->ai.i0];
    if (old == 0 || c->ai.i0 == old) {
        c->obj.anim.newaction = 0x2C;
    } else if (old < c->ai.i0) {
        c->obj.anim.newaction = 0x32;
    } else {
        c->obj.anim.newaction = 0x39;
    }
    return 1;
}

s32 SpaceCrunchFunction_NewCount(struct creature_s *c)
{
    switch ((u32)crunchtime_attack_phase) {
    case 0:
    default:
        c->ai.count = 1;
        break;
    case 1:
        c->ai.count = 2;
        break;
    case 2:
        c->ai.count = 4;
        break;
    case 3:
        c->ai.count = 8;
        break;
    }
    return 1;
}


s32 SpaceCrunchFunction_AttackCortex(struct creature_s *c, struct nuvec_s *v)
{
    c->ai.i0 = 0;
    c->ai.newpos = *v;
    c->obj.anim.newaction = 0x2C;
    return 1;
}

s32 SpaceCortexFunction_TakeHit(struct creature_s *c)
{
    GameSfx(0x37, &c->obj.pos);
    player->obj.mom.x = 0.0f;
    player->obj.mom.z = 0.0f;
    return 1;
}

s32 SpaceCortexFunction_CheckPunch(void)
{
    return SpaceCrunch_Punch;
}

s32 SpaceCrunchFunction_CheckCortexBack(void)
{
    return SpaceCortex_Back;
}

s32 SpaceCortexFunction_Defeated(void)
{
    boss_dead = (Game.hub[Hub].flags & 4) ? 2 : 1;
    SpaceCortex_Defeated = 1;
    return 1;
}

s32 SpaceCrunchFunction_PunchCortex(void)
{
    FindNearestCreature(&player->obj.pos, 2, 0);
    if (temp_creature_i != -1) {
        Character[temp_creature_i].obj.anim.newaction = 0x49;
    }
    SpaceCrunch_Punch = 1;
    return 1;
}

void ResetDRAINDAMAGE(void)
{
    drain_wawa_ok = 0;
    DrainDamage_Intro = 1;
    drain_ang[0] = qrand();
    drain_ang[1] = qrand();
    drain_ang[2] = qrand();
}


f32 RatioDifferenceAlongLine(struct nuvec_s *a, struct nuvec_s *b, f32 p0, f32 p1)
{
    f32 r;
    f32 dx;
    f32 dz;
    f32 d;

    r = p1 - p0;
    dx = (b->x - a->x) * r;
    dz = (b->z - a->z) * r;
    d = NuFsqrt(dx * dx + dz * dz);
    if (r < 0.0f) {
        d = -d;
    }
    return d;
}

void LoadAI(void)
{
    void *file;
    s32 size;
    s32 count;
    s32 n;
    char name[16];
    s32 points;
    s32 type;
    s32 i;
    s32 slot;

    sprintf(tbuf, "%s.ai", LevelFileName);
    size = NuFileLoadBuffer(tbuf, Chase, 0x7FFFFFFF);
    file = NuMemFileOpen(Chase, size, 0);
    if (file == 0) {
        return;
    }

    count = NuFileReadInt(file);
    for (n = 0; n < count; n++) {
        for (i = 0; i < 16; i++) {
            name[i] = NuFileReadChar(file);
        }
        points = NuFileReadInt(file);
        type = FindAIType(name, points);
        for (i = 0; i < points; i++) {
            slot = (type != -1) ? i : 0;
            AITab[LEVELAICOUNT].pos[slot].x = NuFileReadFloat(file);
            AITab[LEVELAICOUNT].pos[slot].y = NuFileReadFloat(file);
            AITab[LEVELAICOUNT].pos[slot].z = NuFileReadFloat(file);
        }
        if (type != -1) {
            AITab[LEVELAICOUNT].ai_type = type;
            LEVELAICOUNT++;
        }
    }

    NuFileClose(file);
}


void SaveAI(void)
{
    void *file;
    s32 i;
    s32 j;
    u8 type;
    s32 count;

    sprintf(tbuf, "%s.ai", LevelFileName);
    file = NuFileOpen(tbuf, 1);
    if (file == 0) {
        return;
    }

    NuFileWriteInt(file, LEVELAICOUNT);
    for (i = 0; i < LEVELAICOUNT; i++) {
        type = AITab[i].ai_type;
        for (j = 0; j < 16; j++) {
            NuFileWriteChar(file, AIType[type].name[j]);
        }
        count = AIType[type].points;
        NuFileWriteInt(file, count);
        for (j = 0; j < count; j++) {
            NuFileWriteFloat(file, AITab[i].pos[j].x);
            NuFileWriteFloat(file, AITab[i].pos[j].y);
            NuFileWriteFloat(file, AITab[i].pos[j].z);
        }
    }

    NuFileClose(file);
}

/* Inline helpers: defined after every non-inlining caller (UpdateDRAINDAMAGE,
 * UpdateCRUNCHTIME, SpaceCrunchFunction_PunchCortex call FindNearestCreature
 * out of line in retail) and before MoveCreature, which inlines all four.
 * gcc 2.95 defers public inline functions to the end of the unit, which is
 * why their out-of-line copies sit after MoveCreature. */
inline f32 FindNearestCreature(struct nuvec_s *pos, s32 type, struct nuvec_s *out)
{
    struct creature_s *c;
    f32 d;
    f32 best;
    s32 i;
    s32 nearest;

    nearest = -1;
    best = 1000000.0f;
    c = Character;
    for (i = 0; i < 9; i++, c++) {
        if (c->on == 0) {
            continue;
        }
        if (c->used == 0) {
            continue;
        }
        if (c->obj.character != type) {
            continue;
        }
        if (pos != &c->obj.pos) {
            d = NuVecDistSqr(pos, &c->obj.pos, 0);
            if (d < best) {
                best = d;
                nearest = i;
                if (out != 0) {
                    *out = c->obj.pos;
                }
            }
        }
    }
    temp_creature_i = nearest;
    return best;
}

inline s32 FindAILabel(struct creatcmd_s *cmd, s32 label)
{
    s32 i;

    i = 0;
loop:
    if (cmd->cmd == 0x90) {
        return 0;
    }
    if (cmd->cmd == 0x8B) {
        if (cmd->i == label) {
            return i;
        }
    }
    cmd++;
    i++;
    goto loop;
}

inline s32 PlayerLateralInRange(f32 p0, struct nuvec_s *line, struct nuvec_s *a, struct nuvec_s *b, f32 p1)
{
    f32 ratio;
    f32 dx;
    f32 dz;
    f32 d;
    double dist;

    ratio = RatioAlongLine(line, a, b) - p0;
    dx = (b->x - a->x) * ratio;
    dz = (b->z - a->z) * ratio;
    d = NuFsqrt(dx * dx + dz * dz);
    if (ratio < 0.0f) {
        d = -d;
    }
    dist = d;
    if (dist < 0.0) {
        dist = -dist;
    }
    return dist < p1;
}


inline s32 PlayerLateralOutOfRange(f32 p0, struct nuvec_s *line, struct nuvec_s *a, struct nuvec_s *b, f32 p1)
{
    f32 ratio;
    f32 dx;
    f32 dz;
    f32 d;
    double dist;

    ratio = RatioAlongLine(line, a, b) - p0;
    dx = (b->x - a->x) * ratio;
    dz = (b->z - a->z) * ratio;
    d = NuFsqrt(dx * dx + dz * dz);
    if (ratio < 0.0f) {
        d = -d;
    }
    dist = d;
    if (dist < 0.0) {
        dist = -dist;
    }
    return dist > p1;
}

extern struct nuvec_s v000;
extern u16 temp_xrot;
extern u16 temp_zrot;
extern s32 gamesfx_pitch;
extern s32 DESTRUCTIBLECRATECOUNT;
extern struct panelcount_s plr_crates;
s32 HitCreatures(struct obj_s *obj, s32 a, s32 b);
s32 HitCrates(struct obj_s *obj, s32 destroy);
s32 HitItems(struct obj_s *obj);
s32 HitWumpa(struct obj_s *obj, s32 destroy);
void GetTopBot(struct creature_s *c);
void NewTopBot(struct obj_s *obj);
void OldTopBot(struct obj_s *obj);
void GetSurfaceInfo(struct obj_s *obj, s32 mode, f32 y);
void ObjectRotation(struct obj_s *obj, s32 mode, s32 vehicle);
s32 RotDiff(u16 a, u16 b);
u16 TurnRot(u16 a, u16 b, s32 rate);
void AddKaboom(s32 type, struct nuvec_s *pos, f32 radius);
void AddGameDebris(s32 type, struct nuvec_s *pos);
s32 GetDieAnim(struct obj_s *obj, s32 anim);
s32 KillGameObject(struct obj_s *obj, s32 anim);
void FlyGameObject(struct obj_s *obj, u16 yrot);
s32 ResetHGobjAnim(struct nuhspecial_s *obj);
s32 StartHGobjAnim(struct nuhspecial_s *obj);
void GameSfxLoop(s32 sfx, struct nuvec_s *pos);

typedef void (*aicmdfn_t)(struct creature_s *c, struct nuvec_s *pos);
typedef s32 (*aieventfn_t)(struct creature_s *c, struct nuvec_s *pos);

/* Per-type creature AI brain: steps the AITab command list (three jump
 * tables: jtbl_00620010 commands 0..0x8B, jtbl_00620240 the 0x7E test
 * conditions, jtbl_00620290 event kinds), then movement, rotation, terrain,
 * events and attack locators. FindNearestCreature, FindAILabel and the two
 * PlayerLateral tests are inlined.
 *
 * Faithful full-C near-match (state=asm): frame/reg_mask/freg_mask exact,
 * 3157/3838 instructions aligned (3759 by opcode). Blocked by nine SN-as
 * mtc1 hazard nops the decompals `as` drops (see the recorded blocker), then
 * by whole-function callee-saved numbering. Levers that mattered: the spill
 * slots 0x40..0x60 follow declaration order (oldhdg .. ai_type); `default`
 * is the first switch block; event cases are in source order 0,1,2,3,5,7,4,6;
 * the 0x8C label walk is a goto loop and 0x8D/0x8E a two-case switch; two
 * shared floats fa/fb land in $f20/$f21 (block-scoped temps push t to $f23);
 * one UpdateAnimPacket call with a ternary model. */
void MoveCreature(struct creature_s *c)
{
    struct nuvec_s v;
    struct nuvec_s v2;
    struct nuvec_s v3;
    struct nuvec_s mid;
    u16 oldhdg;
    u16 touchmask;
    s32 ptype;
    s32 ptarget;
    s32 evhit;
    s32 branch;
    s32 ok;
    s32 points;
    s32 ai_type;
    struct creature_s *plr;
    struct nuvec_s *pos;
    struct nuvec_s *p;
    struct nuvec_s *q;
    s32 hit;
    s32 i;
    s32 j;
    s32 n;
    u16 ang;
    f32 t;
    f32 fa;
    f32 fb;
    f32 dx;
    f32 dz;
    f32 dy;
    f32 fy;

    plr = player;
    GetTopBot(player);
    NewTopBot(&plr->obj);
    switch (c->obj.dead) {
    case 1:
        NuVecAdd(&c->obj.pos, &c->obj.pos, &c->obj.mom);
        if (HitCreatures(&c->obj, 1, 4) || HitCrates(&c->obj, 1) || HitItems(&c->obj)) {
            c->obj.die_time = c->obj.die_duration;
        } else {
            HitWumpa(&c->obj, 0);
        }
        break;
    case 0:
        mid.x = c->obj.pos.x;
        mid.y = c->obj.pos.y + (c->obj.bot + c->obj.top) * c->obj.SCALE * 0.5f;
        mid.z = c->obj.pos.z;
        c->obj.oldpos = c->obj.pos;
        c->obj.anim.oldaction = c->obj.anim.action;
        OldTopBot(&c->obj);
        pos = AITab[c->i_aitab].pos;
        ai_type = AITab[c->i_aitab].ai_type;
        points = AIType[ai_type].points;
        if (c->cmdcurr == 0) {
            c->cmdcurr = c->cmdtable;
        }
        BranchLoop: {
            ok = 1;
            switch (c->cmdcurr->cmd) {
            default:
                ok = 0;
                break;
            case 0x00:
                memset(&c->ai, 0, sizeof(struct AI_s));
                c->ai.scale = 1.0f;
                for (i = 0; i < 2; i++) {
                    c->ai.event[i] = -1;
                }
                if (c->obj.character == 0x3F) {
                    FindGongBongerAnim(pos, &c->ai.obj);
                }
                c->ai.locator_kill = -1;
                c->ai.kill = -1;
                c->ai.force_action = -1;
                c->ai.look_creature = -1;
                break;
            case 0x2B:
                GetALONG(&c->obj.pos, &c->obj.RPos, c->obj.RPos.iRAIL, c->obj.RPos.iALONG,
                         c->cmdcurr->i);
                break;
            case 0x85:
                ((aicmdfn_t)c->cmdcurr->i)(c, pos);
                break;
            case 0x01:
                c->ai.i0 = c->cmdcurr->i;
                c->obj.pos = pos[c->ai.i0];
                break;
            case 0x02:
                player->obj.pos = pos[c->cmdcurr->i];
                player->obj.oldpos = player->obj.pos;
                player->obj.mom = v000;
                c->obj.contact = 0;
                c->obj.kill_contact = 0;
                c->obj.touch = 0;
                break;
            case 0x05:
                c->ai.lateral = c->cmdcurr->f;
                c->obj.pos.x = pos[c->ai.i0].x + (pos[c->ai.i1].x - pos[c->ai.i0].x) * c->ai.lateral;
                c->obj.pos.y = pos[c->ai.i0].y + (pos[c->ai.i1].y - pos[c->ai.i0].y) * c->ai.lateral;
                c->obj.pos.z = pos[c->ai.i0].z + (pos[c->ai.i1].z - pos[c->ai.i0].z) * c->ai.lateral;
                break;
            case 0x06:
                c->ai.lateral = RatioAlongLine(&plr->obj.pos, &pos[c->ai.i0], &pos[c->ai.i1]);
                c->obj.pos.x = pos[c->ai.i0].x + (pos[c->ai.i1].x - pos[c->ai.i0].x) * c->ai.lateral;
                c->obj.pos.y = pos[c->ai.i0].y + (pos[c->ai.i1].y - pos[c->ai.i0].y) * c->ai.lateral;
                c->obj.pos.z = pos[c->ai.i0].z + (pos[c->ai.i1].z - pos[c->ai.i0].z) * c->ai.lateral;
                break;
            case 0x03:
                if (c->obj.flags & 0x10000) {
                    if (c->obj.roof_y != 2000000.0f) {
                        c->obj.pos.y = c->obj.roof_y - c->obj.top * c->obj.SCALE;
                    }
                } else if (c->obj.shadow != 2000000.0f) {
                    c->obj.pos.y = c->obj.shadow - c->obj.bot * c->obj.SCALE;
                }
                break;
            case 0x04:
                c->obj.pos.y += c->cmdcurr->f;
                break;
            case 0x07:
                i = c->cmdcurr->i >> 8;
                j = c->cmdcurr->i & 0xFF;
                c->obj.pos.x = pos[i].x + (pos[j].x - pos[i].x) * c->cmdcurr->f;
                c->obj.pos.y = pos[i].y + (pos[j].y - pos[i].y) * c->cmdcurr->f;
                c->obj.pos.z = pos[i].z + (pos[j].z - pos[i].z) * c->cmdcurr->f;
                break;
            case 0x08:
                c->ai.i0 = points - 1;
                c->obj.pos = pos[c->ai.i0];
                break;
            case 0x09:
                c->ai.newpos.x = pos[c->cmdcurr->i].x;
                c->ai.newpos.y = pos[c->cmdcurr->i].y;
                c->ai.newpos.z = pos[c->cmdcurr->i].z;
                break;
            case 0x0D:
                c->ai.lateral = c->cmdcurr->f;
                c->ai.newpos.x = pos[c->ai.i0].x + (pos[c->ai.i1].x - pos[c->ai.i0].x) * c->ai.lateral;
                c->ai.newpos.y = pos[c->ai.i0].y + (pos[c->ai.i1].y - pos[c->ai.i0].y) * c->ai.lateral;
                c->ai.newpos.z = pos[c->ai.i0].z + (pos[c->ai.i1].z - pos[c->ai.i0].z) * c->ai.lateral;
                break;
            case 0x0E:
                c->ai.lateral = RatioAlongLine(&plr->obj.pos, &pos[c->ai.i0], &pos[c->ai.i1]);
                c->ai.newpos.x = pos[c->ai.i0].x + (pos[c->ai.i1].x - pos[c->ai.i0].x) * c->ai.lateral;
                c->ai.newpos.y = pos[c->ai.i0].y + (pos[c->ai.i1].y - pos[c->ai.i0].y) * c->ai.lateral;
                c->ai.newpos.z = pos[c->ai.i0].z + (pos[c->ai.i1].z - pos[c->ai.i0].z) * c->ai.lateral;
                break;
            case 0x0A:
                c->ai.newpos = c->obj.pos;
                break;
            case 0x0B:
                if (c->obj.shadow != 2000000.0f) {
                    c->ai.newpos.y = c->obj.shadow;
                }
                break;
            case 0x0C:
                c->ai.newpos.y += c->cmdcurr->f;
                break;
            case 0x0F:
                i = c->cmdcurr->i >> 8;
                j = c->cmdcurr->i & 0xFF;
                c->ai.newpos.x = pos[i].x + (pos[j].x - pos[i].x) * c->cmdcurr->f;
                c->ai.newpos.y = pos[i].y + (pos[j].y - pos[i].y) * c->cmdcurr->f;
                c->ai.newpos.z = pos[i].z + (pos[j].z - pos[i].z) * c->cmdcurr->f;
                break;
            case 0x10:
                c->ai.i0++;
                if (c->ai.i0 == points) {
                    c->ai.i0 = 0;
                }
                c->ai.newpos = pos[c->ai.i0];
                break;
            case 0x11:
                c->ai.i0--;
                if (c->ai.i0 < 0) {
                    c->ai.i0 += points;
                }
                c->ai.newpos = pos[c->ai.i0];
                break;
            case 0x12:
                c->ai.i0 = points - 1;
                c->ai.newpos = pos[c->ai.i0];
                break;
            case 0x13:
            case 0x14:
                v.x = 0.0f;
                v.y = 0.0f;
                v.z = c->cmdcurr->f;
                if (c->cmdcurr->cmd == 0x13) {
                    v.z *= qrand() * 1.5259022e-05f;
                }
                NuVecRotateX(&v, &v, qrand());
                NuVecRotateY(&c->ai.newpos, &v, qrand());
                NuVecAdd(&c->ai.newpos, &c->ai.newpos, &c->ai.wobblepos);
                break;
            case 0x15:
                i = qrand() % points;
                c->ai.newpos.x = pos[i].x;
                c->ai.newpos.y = pos[i].y;
                c->ai.newpos.z = pos[i].z;
                break;
            case 0x21:
                c->obj.hdg = c->cmdcurr->i;
                break;
            case 0x22:
                c->obj.hdg = NuAtan2D(pos[c->cmdcurr->i].x - c->obj.pos.x,
                                      pos[c->cmdcurr->i].z - c->obj.pos.z);
                break;
            case 0x23:
            case 0x24:
            case 0x25:
            case 0x28:
                if (c->cmdcurr->cmd == 0x28) {
                    p = &ai_lookpos;
                } else if (c->cmdcurr->cmd == 0x25) {
                    p = &c->ai.newpos;
                } else if (c->cmdcurr->cmd == 0x24) {
                    p = &D_0058656C;
                } else {
                    p = &plr->obj.pos;
                }
                c->obj.hdg = NuAtan2D(p->x - c->obj.pos.x, p->z - c->obj.pos.z);
                break;
            case 0x27:
                if (FindNearestCreature(&c->obj.pos, c->ai.look_creature, &v) < 1000000.0f) {
                    c->obj.hdg = NuAtan2D(v.x - c->obj.pos.x, v.z - c->obj.pos.z);
                }
                break;
            case 0x26:
                fa = points;
                v = v000;
                for (i = 0; i < points; i++) {
                    NuVecAdd(&v, &v, &pos[i]);
                }
                NuVecScale(&v, &v, 1.0f / fa);
                c->obj.hdg = NuAtan2D(v.x - c->obj.pos.x, v.z - c->obj.pos.z);
                break;
            case 0x29:
                c->obj.hdg = qrand();
                break;
            case 0x2A:
                c->obj.hdg += c->cmdcurr->i;
                break;
            case 0x2D:
                c->ai.rotspeed = c->cmdcurr->i;
                break;
            case 0x2E:
                c->ai.rotspeed = (c->cmdcurr->i * qrand()) / 0x10000;
                break;
            case 0x32:
                if (qrand() < (u16)c->cmdcurr->i) {
                    c->ai.rotspeed = -c->ai.rotspeed;
                }
                break;
            case 0x2C:
                if (c->obj.RPos.iRAIL != -1) {
                    c->obj.hdg = c->obj.RPos.angle + c->cmdcurr->i;
                }
                break;
            case 0x2F:
                c->ai.wobblepos = c->obj.pos;
                break;
            case 0x30:
                c->ai.wobble_ang = qrand();
                c->ai.wobble_speed = c->cmdcurr->i;
                c->ai.wobble = c->cmdcurr->f;
                break;
            case 0x74:
            case 0x75:
            case 0x76:
            case 0x77:
                if (c->obj.dead != 0 || plr->obj.dead != 0) {
                    for (j = 0; j < 2; j++) {
                        c->ai.event[j] = -1;
                    }
                } else {
                    if (c->cmdcurr->cmd == 0x76 || c->cmdcurr->cmd == 0x77) {
                        i = c->cmdcurr->cmd - 0x76;
                        c->ai.event[i] = 0x14;
                        c->ai.event_function[i] = (void *)c->cmdcurr->i;
                    } else {
                        i = c->cmdcurr->cmd - 0x74;
                        c->ai.event[i] = c->cmdcurr->i;
                        switch (c->ai.event[i]) {
                        case 0 ... 7:
                        case 10 ... 13:
                            c->ai.event_distance[i] = c->cmdcurr->f;
                            break;
                        }
                    }
                    c->ai.event_branch[i] = -1;
                    c->ai.event_flags[i] = 0;
                    c->ai.event_occured[i] = 0;
                }
                break;
            case 0x78:
            case 0x79:
                i = c->cmdcurr->cmd - 0x78;
                c->ai.event_branch[i] = FindAILabel(c->cmdtable, c->cmdcurr->i);
                break;
            case 0x7A:
            case 0x7B:
            case 0x7C:
            case 0x7D:
                c->ai.event_index[c->cmdcurr->cmd - 0x7A] = c->cmdcurr->i;
                break;
            case 0x4B:
                c->ai.angle = c->ai.oldangle = c->cmdcurr->i;
                break;
            case 0x4C:
                c->ai.angle = c->ai.oldangle = qrand();
                break;
            case 0x53:
                c->ai.radius = c->cmdcurr->f;
                break;
            case 0x54:
                c->ai.height = c->cmdcurr->f;
                break;
            case 0x4D:
                c->ai.speed = c->cmdcurr->f;
                break;
            case 0x4E:
                c->ai.speed = qrand() * 1.5259022e-05f * c->cmdcurr->f;
                break;
            case 0x4F:
                c->ai.speed += c->cmdcurr->f;
                break;
            case 0x52:
                c->ai.distance = c->cmdcurr->f;
                break;
            case 0x50:
                c->ai.direction = c->cmdcurr->i;
                break;
            case 0x51:
                if (c->cmdcurr->i > 0) {
                    c->ai.direction = qrand() / (0xFFFF / c->cmdcurr->i + 1);
                } else {
                    c->ai.direction = 0;
                }
                break;
            case 0x5B:
                c->obj.invisible = 1;
                break;
            case 0x5C:
                c->obj.invisible = 0;
                break;
            case 0x5D:
                if (c->cmdcurr->i != 0) {
                    *(s32 *)c->cmdcurr->i = c->cmdcurr->f;
                }
                break;
            case 0x5E:
                c->ai.i0 = c->cmdcurr->i;
                break;
            case 0x5F:
                c->ai.i1 = c->cmdcurr->i;
                break;
            case 0x60:
                c->obj.flags |= c->cmdcurr->i;
                if (c->cmdcurr->i & 0x10000) {
                    GetTopBot(c);
                }
                break;
            case 0x61:
                c->obj.flags |= c->cmdcurr->i;
                c->obj.flags ^= c->cmdcurr->i;
                if (c->cmdcurr->i & 0x10000) {
                    GetTopBot(c);
                }
                break;
            case 0x86:
                touchmask = c->cmdcurr->i;
                break;
            case 0x31:
                c->ai.look_creature = c->cmdcurr->i;
                break;
            case 0x16:
                c->ai.count = c->cmdcurr->i;
                break;
            case 0x17:
                i = abs(c->cmdcurr->i);
                if (i > 0) {
                    c->ai.count = qrand() / (0xFFFF / (i + 1) + 1);
                } else {
                    c->ai.count = 0;
                }
                break;
            case 0x18:
                i = c->ai.count + c->cmdcurr->i;
                if (i < 0) {
                    i = 0;
                }
                c->ai.count = i;
                break;
            case 0x19:
                c->ai.count++;
                break;
            case 0x1A:
                if (c->ai.count != 0) {
                    c->ai.count--;
                }
                break;
            case 0x1B:
                c->ai.hits = c->cmdcurr->i;
                break;
            case 0x1C:
                i = abs(c->cmdcurr->i);
                if (i > 0) {
                    c->ai.hits = qrand() / (0xFFFF / (i + 1) + 1);
                } else {
                    c->ai.hits = 0;
                }
                break;
            case 0x1D:
                i = c->ai.hits + c->cmdcurr->i;
                if (i < 0) {
                    i = 0;
                }
                c->ai.hits = i;
                break;
            case 0x1E:
                c->ai.hits++;
                break;
            case 0x1F:
                if (c->ai.hits != 0) {
                    c->ai.hits--;
                }
                break;
            case 0x45:
                NewRumble(&player->rumble, c->cmdcurr->i);
                break;
            case 0x46:
                NewBuzz(&player->rumble, c->cmdcurr->i);
                break;
            case 0x55:
                c->ai.movflags = c->cmdcurr->i;
                if (c->cmdcurr->i & 0x800) {
                    c->ai.height = c->cmdcurr->f;
                }
                break;
            case 0x56:
                c->ai.rotflags = c->cmdcurr->i;
                if (c->ai.rotflags & 1) {
                    double dd = c->cmdcurr->f;
                    if (dd < 0.0) {
                        dd = 0.0 - dd;
                    }
                    c->ai.spins = dd;
                }
                break;
            case 0x33:
                c->ai.rotadjust = c->cmdcurr->i;
                break;
            case 0x57:
                c->ai.terflags = c->cmdcurr->i;
                break;
            case 0x34:
                c->obj.shadow = 2000000.0f;
                break;
            case 0x35:
                if (c->obj.flags & 0x10000) {
                    v.x = c->obj.pos.x;
                    v.y = c->obj.pos.y - 1.0f;
                    v.z = c->obj.pos.z;
                    c->obj.shadow = NewShadowMaskPlat(&v, 0.0f, -1);
                } else {
                    c->obj.shadow = NewShadowMaskPlat(&c->obj.pos, 0.0f, -1);
                }
                GetSurfaceInfo(&c->obj, 1, c->obj.shadow);
                ObjectRotation(&c->obj, 0, 0);
                c->obj.surface_xrot = temp_xrot;
                c->obj.surface_zrot = temp_zrot;
                break;
            case 0x36:
                if (c->obj.flags & 0x10000) {
                    c->obj.xrot = c->obj.roof_xrot;
                    c->obj.zrot = c->obj.roof_zrot;
                } else {
                    c->obj.xrot = c->obj.surface_xrot;
                    c->obj.zrot = c->obj.surface_zrot;
                }
                break;
            case 0x89:
                i = c->obj.character;
                ChangeCharacter(c, player->obj.character);
                ChangeCharacter(player, i);
                break;
            case 0x87:
                ChangeCharacter(c, c->cmdcurr->i);
                break;
            case 0x88:
                ChangeCharacter(player, c->cmdcurr->i);
                break;
            case 0x47:
                ResetAnimPacket(&c->obj.anim, c->cmdcurr->i);
                break;
            case 0x48:
                c->obj.anim.newaction = c->cmdcurr->i;
                break;
            case 0x59: {
                f32 f = c->cmdcurr->f;
                c->obj.anim.anim_time = f;
                if (f < 1.0f) {
                    c->obj.anim.anim_time = 1.0f;
                }
                break;
            }
            case 0x5A:
                if (c->obj.model->anmdata[c->obj.anim.newaction] != 0) {
                    c->obj.anim.anim_time = qrand() * 1.5259022e-05f
                        * (c->obj.model->anmdata[c->obj.anim.newaction]->time - 1.0f) + 1.0f;
                }
                break;
            case 0x49:
                c->ai.force_action = c->cmdcurr->i;
                break;
            case 0x4A:
                c->ai.idle_update = c->cmdcurr->i;
                break;
            case 0x3C:
                c->ai.duration = c->cmdcurr->f;
                break;
            case 0x3D:
                NuVecSub(&v, &c->ai.newpos, &c->ai.oldpos);
                c->ai.duration = NuVecMag(&v) / c->cmdcurr->f;
                if (c->ai.duration < 0.02f) {
                    c->ai.duration = 0.02f;
                }
                break;
            case 0x3E:
                if (c->obj.model->anmdata[c->obj.anim.newaction] != 0) {
                    c->ai.duration = (c->obj.model->anmdata[c->obj.anim.newaction]->time - 1.0f)
                        / (c->obj.model->animlist[c->obj.anim.newaction]->speed * 30.0f);
                } else {
                    c->ai.duration = 1.0f;
                }
                if (1.0f < c->cmdcurr->f) {
                    c->ai.duration *= c->cmdcurr->f;
                }
                break;
            case 0x40: {
                f32 end;
                if (c->obj.model->anmdata[c->obj.anim.newaction] == 0) {
                    c->ai.duration = 1.0f;
                    break;
                }
                end = c->obj.model->anmdata[c->obj.anim.newaction]->time;
                if (1.0f < c->cmdcurr->f && c->cmdcurr->f < end
                    && c->obj.anim.anim_time < c->cmdcurr->f) {
                    end = c->cmdcurr->f;
                }
                c->ai.duration = (end - c->obj.anim.anim_time)
                    / (c->obj.model->animlist[c->obj.anim.newaction]->speed * 30.0f);
                break;
            }
            case 0x3F:
                if (c->obj.model->anmdata[c->obj.anim.newaction] != 0) {
                    c->ai.duration = (c->cmdcurr->f - 1.0f)
                        / (c->obj.model->animlist[c->obj.anim.newaction]->speed * 30.0f);
                } else {
                    c->ai.duration = 1.0f;
                }
                break;
            case 0x41:
                c->ai.duration = 0.0f;
                break;
            case 0x42:
                c->ai.duration = qrand() * 1.5259022e-05f * c->cmdcurr->f;
                break;
            case 0x43:
                c->ai.duration += c->cmdcurr->f;
                break;
            case 0x44:
                if (c->ai.rotspeed != 0) {
                    i = NuAtan2D(c->ai.newpos.x - c->obj.pos.x, c->ai.newpos.z - c->obj.pos.z);
                    i = RotDiff(c->ai.old_yrot, i);
                    if ((c->ai.rotflags & 0x200) && i < 0) {
                        i += 0x10000;
                    } else if ((c->ai.rotflags & 0x400) && i > 0) {
                        i -= 0x10000;
                    }
                    c->ai.duration = (f32)abs(i) / (f32)(c->ai.rotspeed * 50);
                } else {
                    c->ai.duration = 1.0f;
                }
                break;
            case 0x37:
                c->obj.attack = c->cmdcurr->i;
                break;
            case 0x38:
                c->obj.vulnerable = c->cmdcurr->i;
                if ((c->obj.flags & 4) && ai_type >= 0 && ai_type < 0x3E
                    && c->obj.character != 0x16) {
                    c->obj.vulnerable |= 0x180;
                }
                break;
            case 0x39:
                c->ai.kill = c->cmdcurr->i;
                break;
            case 0x3A:
                c->ai.locator_kill = c->cmdcurr->i;
                break;
            case 0x3B:
                c->ai.attack_locator_bits = c->cmdcurr->i;
                c->ai.attack_locator_radius = c->cmdcurr->f;
                break;
            case 0x6A:
                if (c->cmdcurr->i != -1) {
                    v = pos[c->cmdcurr->i];
                } else {
                    v = c->obj.pos;
                }
                AddKaboom(0x10, &v, c->cmdcurr->f);
                if (c->obj.character == 0x80) {
                    AddDeb3(&v, 9, 1, 0);
                    AddDeb3(&v, 9, 1, 0);
                    AddDeb3(&v, 9, 1, 0);
                    AddGameDebris(0xD, &v);
                    AddGameDebris(0xE, &v);
                }
                break;
            case 0x6C:
                JudderGameCamera(&GameCam[0], 0.3f, 0);
                break;
            case 0x71:
                if (KillPlayer(&plr->obj, GetDieAnim(&plr->obj, c->cmdcurr->i)) == 0) {
                    KillGameObject(&c->obj, 4);
                }
                break;
            case 0x72:
                plr->obj.mom.x = NuTrigTable[c->obj.hdg] * c->cmdcurr->f;
                plr->obj.mom.z = NuTrigTable[(c->obj.hdg + 0x4000) & 0xFFFF] * c->cmdcurr->f;
                break;
            case 0x6D:
                AddGameDebris(c->cmdcurr->i, &c->obj.pos);
                break;
            case 0x6E:
                AddGameDebris(c->cmdcurr->i, &mid);
                break;
            case 0x7E:
            case 0x7F:
            case 0x80:
            case 0x81:
            case 0x82:
            case 0x83:
            case 0x84:
                hit = 0;
                if (c->cmdcurr->cmd == 0x7E) {
                    switch (c->cmdcurr->i) {
                    case 0:
                        if (NuVecDist(&plr->obj.pos, &c->obj.pos, 0) < c->cmdcurr->f) {
                            hit = 1;
                        }
                        break;
                    case 1:
                        if (NuVecDist(&plr->obj.pos, &c->obj.pos, 0) > c->cmdcurr->f) {
                            hit = 1;
                        }
                        break;
                    case 2:
                        if (NuVecXZDist(&plr->obj.pos, &c->obj.pos, 0) < c->cmdcurr->f) {
                            hit = 1;
                        }
                        break;
                    case 3:
                        if (NuVecXZDist(&plr->obj.pos, &c->obj.pos, 0) > c->cmdcurr->f) {
                            hit = 1;
                        }
                        break;
                    case 4:
                        hit = PlayerLateralInRange(c->ai.lateral, &plr->obj.pos, &pos[c->ai.i0], &pos[c->ai.i1], c->cmdcurr->f);
                        break;
                    case 5:
                        hit = PlayerLateralOutOfRange(c->ai.lateral, &plr->obj.pos, &pos[c->ai.i0], &pos[c->ai.i1], c->cmdcurr->f);
                        break;
                    case 6:
                        if (plr->obj.dead != 0) {
                            hit = 1;
                        }
                        break;
                    case 7:
                        if (plr_crates.count >= DESTRUCTIBLECRATECOUNT) {
                            hit = 1;
                        }
                        break;
                    case 8:
                        if (c->ai.i0 == (s32)c->cmdcurr->f) {
                            hit = 1;
                        }
                        break;
                    case 9:
                        if (c->ai.i1 == (s32)c->cmdcurr->f) {
                            hit = 1;
                        }
                        break;
                    case 10:
                        if (c->ai.i0 == points - 1) {
                            hit = 1;
                        }
                        break;
                    case 11:
                        if (c->ai.i1 == points - 1) {
                            hit = 1;
                        }
                        break;
                    case 12:
                    case 13:
                        if (c->ai.i0 == 0) {
                            hit = 1;
                        }
                        break;
                    case 14:
                        if ((c->obj.touch & touchmask) != 0) {
                            hit = 1;
                        }
                        break;
                    }
                } else if (c->cmdcurr->cmd == 0x7F) {
                    if (c->obj.anim.newaction == c->cmdcurr->i) {
                        hit = 1;
                    }
                } else if (c->cmdcurr->cmd == 0x80) {
                    if (c->ai.i0 == c->cmdcurr->i) {
                        hit = 1;
                    }
                } else if (c->cmdcurr->cmd == 0x81) {
                    if (c->ai.count == c->cmdcurr->i) {
                        hit = 1;
                    }
                } else if (c->cmdcurr->cmd == 0x82) {
                    if (c->ai.hits == c->cmdcurr->i) {
                        hit = 1;
                    }
                } else if (c->cmdcurr->cmd == 0x83) {
                    if (c->cmdcurr->i != 0) {
                        if (*(s32 *)c->cmdcurr->i == (s32)c->cmdcurr->f) {
                            hit = 1;
                        }
                    }
                } else if (c->cmdcurr->cmd == 0x84) {
                    if (player->obj.character == c->cmdcurr->i) {
                        hit = 1;
                    }
                }
                c->cmdcurr++;
                switch (c->cmdcurr->cmd) {
                case 0x8D:
                    if (hit) {
                        c->cmdcurr = &c->cmdtable[FindAILabel(c->cmdtable, c->cmdcurr->i)];
                    }
                    break;
                case 0x8E:
                    if (!hit) {
                        c->cmdcurr = &c->cmdtable[FindAILabel(c->cmdtable, c->cmdcurr->i)];
                    }
                    break;
                }
                break;
            case 0x62:
                ptarget = -1;
                ptype = c->cmdcurr->i;
                break;
            case 0x63:
                ptarget = c->cmdcurr->i;
                break;
            case 0x6B:
                v = v000;
                j = 0;
                for (i = 0; i < 16; i++) {
                    if ((c->cmdcurr->i >> i) & 1) {
                        NuVecAdd(&v, &v, (struct nuvec_s *)&c->mtxLOCATOR[0][i]._30);
                        j++;
                    }
                }
                if (j != 0) {
                    if (j > 1) {
                        NuVecScale(&v, &v, 1.0f / j);
                    }
                    AddProjectile(&v, (ptarget != -1) ? &pos[ptarget] : 0,
                                  &c->momLOCATOR[0][i], ptype, c->obj.hdg, &c->obj);
                }
                break;
            case 0x65:
                gamesfx_effect_volume = c->cmdcurr->i;
                break;
            case 0x66:
                gamesfx_pitch = c->cmdcurr->i;
                break;
            case 0x67:
                GameSfx(c->cmdcurr->i, 0);
                break;
            case 0x68:
                GameSfx(c->cmdcurr->i, &c->obj.pos);
                break;
            case 0x69:
                gamesfx_effect_volume = c->cmdcurr->f;
                GameSfx(c->cmdcurr->i, &c->obj.pos);
                break;
            case 0x6F:
                ResetHGobjAnim(&c->ai.obj);
                break;
            case 0x70:
                StartHGobjAnim(&c->ai.obj);
                break;
            case 0x73:
                c->ai.scale = c->cmdcurr->f;
                break;
            case 0x8B:
                break;
            }
            if (ok) {
                c->ai.oldpos = c->obj.pos;
                c->ai.time = c->obj.mom.x = c->obj.mom.y = c->obj.mom.z = 0.0f;
                c->cmdcurr++;
                c->ai.oldpos = c->obj.pos;
                c->ai.old_yrot = c->obj.hdg;
                c->ai.oldangle = c->ai.angle;
                goto BranchLoop;
            }
            i = 0;
label8c:
            if (c->cmdcurr->cmd == 0x8C) {
                c->cmdcurr = &c->cmdtable[FindAILabel(c->cmdtable, c->cmdcurr->i)];
                i++;
                goto label8c;
            }
            if (i != 0) {
                goto BranchLoop;
            }

            evhit = 0;
            branch = -2;
            c->ai.time += 0.02f;
            if (0.0f < c->ai.duration && c->ai.duration <= c->ai.time) {
                c->ai.time = c->ai.duration;
                branch = -1;
                for (i = 0; i < 2; i++) {
                    if (c->ai.event[i] != -1 && c->ai.event_occured[i] != 0) {
                        branch = c->ai.event_branch[i];
                    }
                    c->ai.event[i] = -1;
                }
                evhit = 1;
            }
            if (0.0f < c->ai.duration) {
                t = c->ai.time / c->ai.duration;
            } else {
                t = 0.0f;
            }

            c->ai.wobble_ang += c->ai.wobble_speed;
            if (c->ai.movflags & 0x40) {
                n = c->ai.i0 + 1;
                if (n == points) {
                    n = 0;
                }
                NuVecSub(&v, &pos[n], &pos[c->ai.i0]);
                v2.x = v.x;
                v.x = v.z + c->obj.pos.x;
                v.z = -v2.x + c->obj.pos.z;
                if ((player->obj.pos.x - c->obj.pos.x) * (v.z - c->obj.pos.z)
                    + (player->obj.pos.z - c->obj.pos.z) * (c->obj.pos.x - v.x) >= 0.0f) {
                    i = 0;
                    n = c->ai.i0 + 1;
                    if (n == points) {
                        n = 0;
                    }
                } else {
                    n = c->ai.i0;
                    i = 1;
                }
                q = &pos[n];
                {
                dx = q->x - c->obj.pos.x;
                dz = q->z - c->obj.pos.z;
                fy = dx * dx + dz * dz;
                ang = NuAtan2D(dx, dz);
                fb = c->ai.speed * 0.02f;
                if (fy <= fb * fb) {
                    c->obj.pos = *q;
                    if (!i) {
                        c->ai.i0 = n;
                    } else {
                        c->ai.i0--;
                        if (c->ai.i0 < 0) {
                            c->ai.i0 += points;
                        }
                    }
                } else {
                    c->obj.pos.x += NuTrigTable[ang] * fb;
                    c->obj.pos.z += NuTrigTable[(ang + 0x4000) & 0xFFFF] * fb;
                }
                }
            } else if (c->ai.movflags & 2) {
                if (c->ai.direction == 0) {
                    c->ai.angle += c->ai.rotspeed;
                } else {
                    c->ai.angle -= c->ai.rotspeed;
                }
                c->obj.pos.x = c->ai.newpos.x + NuTrigTable[c->ai.angle] * c->ai.radius;
                c->obj.pos.y = c->ai.newpos.y;
                c->obj.pos.z = c->ai.newpos.z
                    + NuTrigTable[(c->ai.angle + 0x4000) & 0xFFFF] * c->ai.radius;
            } else if (c->ai.movflags & 1) {
                if (c->ai.movflags & 0x30) {
                    if (c->ai.movflags & 0x10) {
                        fa = 1.0f - NuTrigTable[(s32)(t * 16384.0f) & 0xFFFF];
                    } else {
                        fa = NuTrigTable[(s32)(t * 16384.0f + 16384.0f) & 0xFFFF];
                    }
                    c->obj.pos.x = c->ai.newpos.x + (c->ai.oldpos.x - c->ai.newpos.x) * fa;
                    c->obj.pos.y = c->ai.newpos.y + (c->ai.oldpos.y - c->ai.newpos.y) * fa;
                    c->obj.pos.z = c->ai.newpos.z + (c->ai.oldpos.z - c->ai.newpos.z) * fa;
                } else {
                    c->obj.pos.x = c->ai.oldpos.x + (c->ai.newpos.x - c->ai.oldpos.x) * t;
                    c->obj.pos.y = c->ai.oldpos.y + (c->ai.newpos.y - c->ai.oldpos.y) * t;
                    c->obj.pos.z = c->ai.oldpos.z + (c->ai.newpos.z - c->ai.oldpos.z) * t;
                    if (c->ai.movflags & 0x800) {
                        c->obj.pos.y += NuTrigTable[(s32)(t * 32768.0f) & 0xFFFF] * c->ai.height;
                    }
                }
            } else if (c->ai.movflags & 4) {
                double dd;
                dx = pos[c->ai.i1].x - pos[c->ai.i0].x;
                dz = pos[c->ai.i1].z - pos[c->ai.i0].z;
                fb = (c->ai.speed * 0.02f) / NuFsqrt(dx * dx + dz * dz);
                fa = RatioAlongLine(&plr->obj.pos, &pos[c->ai.i0], &pos[c->ai.i1]);
                dd = c->ai.lateral - fa;
                if (dd < 0.0) {
                    dd = 0.0 - dd;
                }
                if (dd > fb) {
                    if (fa < c->ai.lateral) {
                        c->obj.anim.newaction = 0x39;
                        c->ai.lateral -= fb;
                    } else if (c->ai.lateral < fa) {
                        c->obj.anim.newaction = 0x32;
                        c->ai.lateral += fb;
                    }
                    c->obj.pos.x = pos[c->ai.i0].x + (pos[c->ai.i1].x - pos[c->ai.i0].x) * c->ai.lateral;
                    c->obj.pos.y = pos[c->ai.i0].y + (pos[c->ai.i1].y - pos[c->ai.i0].y) * c->ai.lateral;
                    c->obj.pos.z = pos[c->ai.i0].z + (pos[c->ai.i1].z - pos[c->ai.i0].z) * c->ai.lateral;
                }
            } else if (c->ai.movflags & 8) {
                f32 lo = pos[c->ai.i0].y;
                f32 hi = pos[c->ai.i1].y;
                f32 target;
                f32 own;
                double dd;
                if (hi < lo) {
                    fb = hi;
                    hi = lo;
                    lo = fb;
                }
                target = plr->obj.pos.y + (plr->obj.bot + plr->obj.top) * plr->obj.SCALE * 0.5f;
                own = c->obj.pos.y + (c->obj.bot + c->obj.top) * c->obj.SCALE * 0.5f;
                if (target < lo) {
                    target = lo;
                } else if (hi < target) {
                    target = hi;
                }
                dy = target - own;
                dd = dy;
                if (dd < 0.0) {
                    dd = 0.0 - dd;
                }
                if (dd > 0.2f) {
                    if (dy < 0.0f) {
                        c->obj.anim.newaction = 0x16;
                    } else {
                        c->obj.anim.newaction = 0x58;
                    }
                } else {
                    c->obj.anim.newaction = 0x22;
                }
                if (c->obj.anim.newaction != 0x22
                    && c->obj.model->anmdata[c->obj.anim.newaction] == 0) {
                    c->obj.anim.newaction = 0x22;
                }
                c->obj.pos.y += dy * 0.02f;
            } else if (c->ai.movflags & 0x400) {
                oldhdg = c->obj.hdg;
                c->ai.angle += c->ai.rotspeed;
                NuVecSub(&v, &pos[c->ai.i1], &pos[c->ai.i0]);
                c->obj.hdg = NuAtan2D(v.x, v.z);
                NuVecScale(&v, &v, NuTrigTable[c->ai.angle] * 0.5f + 0.5f);
                NuVecAdd(&c->obj.pos, &pos[c->ai.i0], &v);
                c->obj.hdg += c->ai.angle;
                if (c->ai.rotflags & 0x400) {
                    c->obj.hdg += 0x8000;
                }
                c->obj.dyrot = RotDiff(oldhdg, c->obj.hdg);
            }
            if (c->ai.movflags & 0x100) {
                c->ai.wobblepos = c->obj.pos;
            }
            if (c->ai.movflags & 0x200) {
                c->obj.pos.y = c->ai.wobblepos.y + NuTrigTable[c->ai.wobble_ang] * c->ai.wobble;
            }
            c->obj.mom.x = c->obj.pos.x - c->obj.oldpos.x;
            c->obj.mom.y = c->obj.pos.y - c->obj.oldpos.y;
            c->obj.mom.z = c->obj.pos.z - c->obj.oldpos.z;
            c->obj.xz_distance = NuFsqrt(c->obj.mom.x * c->obj.mom.x + c->obj.mom.z * c->obj.mom.z);
            c->obj.xyz_distance = NuFsqrt(c->obj.mom.x * c->obj.mom.x + c->obj.mom.y * c->obj.mom.y
                                          + c->obj.mom.z * c->obj.mom.z);

            if (!(c->ai.movflags & 0x400)) {
                oldhdg = c->obj.hdg;
            }
            if (c->ai.movflags & 2) {
                if (c->ai.direction == 0) {
                    c->obj.hdg = c->ai.angle + 0x4000;
                } else {
                    c->obj.hdg = c->ai.angle - 0x4000;
                }
            } else if (c->ai.rotflags & 1) {
                fb = c->ai.spins * 65536.0f;
                if (c->ai.rotflags & 0x80) {
                    fa = fb * (1.0f - NuTrigTable[(s32)(t * 16384.0f) & 0xFFFF]);
                    if (c->ai.rotflags & 0x400) {
                        c->obj.hdg = c->ai.old_yrot + (s32)(-fb + fa);
                    } else {
                        c->obj.hdg = c->ai.old_yrot + (s32)(fb - fa);
                    }
                } else if (c->ai.rotflags & 0x100) {
                    fa = fb * NuTrigTable[(s32)(t * 16384.0f + 16384.0f) & 0xFFFF];
                    if (c->ai.rotflags & 0x400) {
                        c->obj.hdg = c->ai.old_yrot + (s32)(-fb + fa);
                    } else {
                        c->obj.hdg = c->ai.old_yrot + (s32)(fb - fa);
                    }
                } else {
                    i = (s32)(fb * t);
                    if (c->ai.rotflags & 0x400) {
                        c->obj.hdg = c->ai.old_yrot - i;
                    } else {
                        c->obj.hdg = c->ai.old_yrot + i;
                    }
                }
            } else if (c->ai.rotflags & 2) {
                i = NuAtan2D(c->ai.newpos.x - c->ai.oldpos.x, c->ai.newpos.z - c->ai.oldpos.z);
                i = RotDiff(c->ai.old_yrot, i + c->ai.rotadjust);
                if ((c->ai.rotflags & 0x200) && i < 0) {
                    i += 0x10000;
                } else if ((c->ai.rotflags & 0x400) && i > 0) {
                    i -= 0x10000;
                }
                {
                fb = i;
                if (c->ai.rotflags & 0x80) {
                    fa = fb * (1.0f - NuTrigTable[(s32)(t * 16384.0f) & 0xFFFF]);
                    if (c->ai.rotflags & 0x400) {
                        c->obj.hdg = c->ai.old_yrot + (s32)(-fb + fa);
                    } else {
                        c->obj.hdg = c->ai.old_yrot + (s32)(fb - fa);
                    }
                } else if (c->ai.rotflags & 0x100) {
                    fa = fb * NuTrigTable[(s32)(t * 16384.0f + 16384.0f) & 0xFFFF];
                    if (c->ai.rotflags & 0x400) {
                        c->obj.hdg = c->ai.old_yrot + (s32)(-fb + fa);
                    } else {
                        c->obj.hdg = c->ai.old_yrot + (s32)(fb - fa);
                    }
                } else {
                    c->obj.hdg = c->ai.old_yrot + (s32)(fb * t);
                }
                }
            } else if (c->ai.rotflags & 0x180C) {
                if (c->ai.rotflags & 0x800) {
                    if (!(FindNearestCreature(&c->obj.pos, c->ai.look_creature, &v) < 1000000.0f)) {
                        goto rotdone;
                    }
                    p = &v;
                } else if (c->ai.rotflags & 0x1000) {
                    p = &ai_lookpos;
                } else if (c->ai.rotflags & 8) {
                    p = &D_0058656C;
                } else {
                    p = &plr->obj.pos;
                }
                ang = NuAtan2D(p->x - c->obj.pos.x, p->z - c->obj.pos.z) + c->ai.rotadjust;
                if (c->ai.rotflags & 0x10) {
                    c->obj.hdg = SeekRot(c->obj.hdg, ang, 3);
                } else {
                    c->obj.hdg = TurnRot(c->obj.hdg, ang, c->ai.rotspeed);
                }
            } else if (c->ai.rotflags & 0x20) {
                if (0.0f < c->obj.xz_distance) {
                    ang = NuAtan2D(c->obj.mom.x, c->obj.mom.z) + c->ai.rotadjust;
                    i = RotDiff(c->obj.hdg, ang);
                    j = c->ai.rotspeed;
                    if (abs(i) > j) {
                        if (i > 0) {
                            c->obj.hdg += j;
                        } else if (i < 0) {
                            c->obj.hdg -= j;
                        }
                    } else {
                        c->obj.hdg = ang;
                    }
                }
            } else if (c->ai.rotflags & 0x40) {
                c->obj.hdg += c->ai.rotspeed;
            }
rotdone:
            if (!(c->ai.movflags & 0x400)) {
                c->obj.dyrot = RotDiff(oldhdg, c->obj.hdg);
            }

            c->obj.ground = 0;
            if (c->ai.terflags & 1) {
                if (c->ai.terflags & 2) {
                    c->obj.shadow = NewShadowMaskPlat(&c->obj.pos, 0.0f, -1);
                    GetSurfaceInfo(&c->obj, 1, c->obj.shadow);
                    if (c->obj.shadow != 2000000.0f) {
                        c->obj.ground = 3;
                        c->obj.pos.y = c->obj.shadow - c->obj.bot * c->obj.SCALE;
                    }
                } else if (c->ai.terflags & 4) {
                    c->obj.shadow = NewShadowMaskPlat(&c->obj.pos, 0.0f, -1);
                    GetSurfaceInfo(&c->obj, 1, c->obj.shadow);
                    if (c->obj.shadow != 2000000.0f) {
                        f32 y = c->obj.shadow - c->obj.bot * c->obj.SCALE;
                        if (c->obj.pos.y <= y) {
                            c->obj.pos.y = y;
                            c->obj.ground = 3;
                        }
                    }
                } else if (c->ai.terflags & 8) {
                    v.x = c->obj.pos.x;
                    v.y = c->obj.pos.y - 1.0f;
                    v.z = c->obj.pos.z;
                    c->obj.shadow = NewShadowMaskPlat(&v, 0.0f, -1);
                    GetSurfaceInfo(&c->obj, 1, c->obj.shadow);
                    if (c->obj.roof_type != -1) {
                        c->obj.ground = 0;
                        c->obj.pos.y = c->obj.roof_y - c->obj.top * c->obj.SCALE;
                    }
                } else if (c->ai.terflags & 0x10) {
                    c->obj.shadow = NewShadowMaskPlat(&c->obj.pos, 0.0f, -1);
                    GetSurfaceInfo(&c->obj, 1, c->obj.shadow);
                    if (c->obj.shadow != 2000000.0f) {
                        c->obj.ground = 0;
                        c->obj.pos.y = (c->obj.shadow - c->obj.bot * c->obj.SCALE) + c->ai.height;
                    }
                } else if (c->ai.terflags & 0x60) {
                    c->obj.shadow = NewShadowMaskPlat(&c->obj.pos, 0.0f, -1);
                    GetSurfaceInfo(&c->obj, 1, c->obj.shadow);
                    if (c->obj.shadow != 2000000.0f) {
                        if (c->ai.terflags & 0x40) {
                            fb = 1.0f - t;
                        } else {
                            fb = t;
                        }
                        c->obj.ground = 0;
                        c->obj.pos.y = (c->obj.shadow - c->obj.bot * c->obj.SCALE) + c->ai.height * fb;
                    }
                } else {
                    c->obj.shadow = NewShadowMaskPlat(&c->obj.pos, 0.0f, -1);
                    GetSurfaceInfo(&c->obj, 1, c->obj.shadow);
                }
                ObjectRotation(&c->obj, 2, 0);
            }
            c->obj.mom.y = c->obj.pos.y - c->obj.oldpos.y;

            if (!evhit) {
                hit = -1;
                for (i = 0; i < 2; i++) {
                    if (c->ai.event[i] == -1) {
                        continue;
                    }
                    switch (c->ai.event[i]) {
                    case 0:
                        if (plr->obj.dead == 0) {
                            v2.x = c->obj.pos.x;
                            v2.y = (c->obj.objtop + c->obj.objbot) * 0.5f;
                            v2.z = c->obj.pos.z;
                            v3.x = plr->obj.pos.x;
                            v3.y = (plr->obj.objtop + plr->obj.objbot) * 0.5f;
                            v3.z = plr->obj.pos.z;
                            NuVecSub(&v, &v3, &v2);
                            if (v.x * v.x + v.y * v.y + v.z * v.z
                                < c->ai.event_distance[i] * c->ai.event_distance[i]) {
                                hit = i;
                            }
                        }
                        break;
                    case 1:
                        if (plr->obj.dead == 0) {
                            v2.x = c->obj.pos.x;
                            v2.y = (c->obj.shadow == 2000000.0f) ? c->obj.pos.y : c->obj.shadow;
                            v2.z = c->obj.pos.z;
                            v3.x = plr->obj.pos.x;
                            v3.y = plr->obj.objbot;
                            v3.z = plr->obj.pos.z;
                            NuVecSub(&v, &v3, &v2);
                            if (v.x * v.x + v.y * v.y + v.z * v.z
                                < c->ai.event_distance[i] * c->ai.event_distance[i]) {
                                hit = i;
                            }
                        }
                        break;
                    case 2:
                        if (plr->obj.dead == 0
                            && PlayerLateralInRange(c->ai.lateral, &plr->obj.pos, &pos[c->ai.i0], &pos[c->ai.i1], c->ai.event_distance[i])) {
                            hit = i;
                        }
                        break;
                    case 3:
                        if (plr->obj.dead == 0
                            && PlayerLateralOutOfRange(c->ai.lateral, &plr->obj.pos, &pos[c->ai.i0], &pos[c->ai.i1], c->ai.event_distance[i])) {
                            hit = i;
                        }
                        break;
                    case 5:
                        if (plr->obj.dead == 0) {
                            v2.x = plr->obj.pos.x;
                            v2.y = (plr->obj.objtop + plr->obj.objbot) * 0.5f;
                            v2.z = plr->obj.pos.z;
                            v3 = pos[c->ai.event_index[i]];
                            NuVecSub(&v, &v3, &v2);
                            if (v.x * v.x + v.y * v.y + v.z * v.z
                                < c->ai.event_distance[i] * c->ai.event_distance[i]) {
                                hit = i;
                            }
                        }
                        break;
                    case 7:
                        if (plr->obj.dead == 0) {
                            v2.x = plr->obj.pos.x;
                            v2.y = (plr->obj.objtop + plr->obj.objbot) * 0.5f;
                            v2.z = plr->obj.pos.z;
                            v3 = pos[c->ai.event_index[i]];
                            NuVecSub(&v, &v3, &v2);
                            if (v.x * v.x + v.y * v.y + v.z * v.z
                                > c->ai.event_distance[i] * c->ai.event_distance[i]) {
                                hit = i;
                            }
                        }
                        break;
                    case 4:
                        if (plr->obj.dead == 0) {
                            dx = plr->obj.pos.x - pos[c->ai.event_index[i]].x;
                            dz = plr->obj.pos.z - pos[c->ai.event_index[i]].z;
                            if (dx * dx + dz * dz < c->ai.event_distance[i] * c->ai.event_distance[i]) {
                                hit = i;
                            }
                        }
                        break;
                    case 6:
                        if (plr->obj.dead == 0) {
                            dx = plr->obj.pos.x - pos[c->ai.event_index[i]].x;
                            dz = plr->obj.pos.z - pos[c->ai.event_index[i]].z;
                            if (c->ai.event_distance[i] * c->ai.event_distance[i] < dx * dx + dz * dz) {
                                hit = i;
                            }
                        }
                        break;
                    case 8:
                        if (plr->obj.dead == 0 && c->obj.contact == &plr->obj
                            && c->obj.contact_type == 2) {
                            hit = i;
                        }
                        break;
                    case 9:
                        if (plr->obj.dead != 0) {
                            hit = i;
                        }
                        break;
                    case 10:
                        if (plr->obj.dead == 0
                            && NuVecDist(&plr->obj.pos, &c->obj.pos, 0) < c->ai.event_distance[i]) {
                            hit = i;
                        }
                        break;
                    case 11:
                        if (plr->obj.dead == 0
                            && NuVecDist(&plr->obj.pos, &c->obj.pos, 0) > c->ai.event_distance[i]) {
                            hit = i;
                        }
                        break;
                    case 12:
                        if (plr->obj.dead == 0
                            && NuVecXZDist(&plr->obj.pos, &c->obj.pos, 0) < c->ai.event_distance[i]) {
                            hit = i;
                        }
                        break;
                    case 13:
                        if (plr->obj.dead == 0
                            && NuVecXZDist(&plr->obj.pos, &c->obj.pos, 0) > c->ai.event_distance[i]) {
                            hit = i;
                        }
                        break;
                    case 14:
                        if (plr->obj.dead == 0 && plr_crates.count >= DESTRUCTIBLECRATECOUNT) {
                            hit = i;
                        }
                        break;
                    case 16:
                        if (plr->obj.dead == 0 && c->obj.contact == &plr->obj) {
                            hit = i;
                        }
                        break;
                    case 18:
                        if (plr->obj.dead == 0 && c->obj.kill_contact != 0) {
                            hit = i;
                        }
                        break;
                    case 19:
                        if (c->hit_type == 2) {
                            hit = i;
                        }
                        break;
                    case 20:
                        if (((aieventfn_t)c->ai.event_function[i])(c, pos)) {
                            hit = i;
                        }
                        break;
                    }
                    if (hit != -1) {
                        if (c->ai.event_flags[i] & 1) {
                            c->ai.event_occured[i] = 1;
                        } else {
                            branch = c->ai.event_branch[i];
                        }
                        for (j = 0; j < 2; j++) {
                            c->ai.event[j] = -1;
                        }
                        break;
                    }
                }
            }
            if (branch == -2) {
                goto Skip;
            }
            c->ai.time = 0.0f;
            c->ai.oldpos = c->obj.pos;
            c->ai.old_yrot = c->obj.hdg;
            if (branch == -1) {
                c->cmdcurr++;
            } else {
                c->cmdcurr = &c->cmdtable[branch];
            }
            goto BranchLoop;
        }
    Skip:

        if ((u32)(c->cmdcurr->cmd - 0x8F) < 2) {
            if (c->obj.dead != 0) {
                break;
            }
            if (c->cmdcurr->i == 0) {
                i = 1;
                if (c->obj.touch & 6) {
                    i = 4;
                }
            } else {
                i = c->cmdcurr->i;
            }
            if (i == 1) {
                FlyGameObject(&c->obj, NuAtan2D(c->obj.pos.x - plr->obj.pos.x,
                                                c->obj.pos.z - plr->obj.pos.z));
            }
            KillGameObject(&c->obj, i);
        }
        if (c->obj.dead != 0) {
            break;
        }
        if (plr->obj.dead == 0 && c->ai.attack_locator_bits != 0 && c->obj.draw_frame != 0) {
            for (i = 0; i < 16; i++) {
                if (!((c->ai.attack_locator_bits >> i) & 1)) {
                    continue;
                }
                if (c->obj.model->pLOCATOR[i] == 0) {
                    continue;
                }
                {
                f32 d = c->ai.attack_locator_radius * c->obj.SCALE;
                f32 hi;
                v3.y = c->mtxLOCATOR[0][i]._31;
                if (v3.y + d < plr->obj.objbot) {
                    continue;
                }
                if (plr->obj.objtop < v3.y - d) {
                    continue;
                }
                v3.x = c->mtxLOCATOR[0][i]._30;
                v3.z = c->mtxLOCATOR[0][i]._32;
                dx = v3.x - plr->obj.pos.x;
                dz = v3.z - plr->obj.pos.z;
                hi = plr->obj.RADIUS + d;
                fb = dx * dx + dz * dz;
                if (hi * hi < fb) {
                    continue;
                }
                v2.y = v3.y - c->momLOCATOR[0][i].y;
                if (c->obj.attack != 0) {
                    if (plr->spin != 0
                        && plr->spin_frame < plr->spin_frames - plr->minfo->SPINRESETFRAMES) {
                        if (v2.y + d - 0.012f <= plr->obj.oldobjbot) {
                            plr->obj.mom.y = 0.0f;
                            plr->obj.boing |= 1;
                            NewRumble(&plr->rumble, 0x7F);
                            NewBuzz(&plr->rumble, 0xA);
                        } else if (plr->obj.oldobjtop <= v2.y - d + 0.012f) {
                            plr->obj.mom.y = 0.0f;
                        } else {
                            ang = NuAtan2D(dx, dz);
                            plr->obj.mom.x = -NuTrigTable[ang] * 0.12f;
                            plr->obj.mom.z = -NuTrigTable[(ang + 0x4000) & 0xFFFF] * 0.12f;
                        }
                    } else {
                        if (KillPlayer(&plr->obj, (c->ai.locator_kill != -1)
                                                      ? c->ai.locator_kill
                                                      : GetDieAnim(&plr->obj, -1)) == 0) {
                            KillGameObject(&c->obj, 4);
                        }
                    }
                }
                break;
                }
            }
        }
        break;
    }

    if (c->obj.dead != 0) {
        if (c->obj.die_action != -1) {
            c->obj.anim.newaction = c->obj.die_action;
            fb = 0.59999996f;
        } else {
            fb = 0.0f;
        }
    } else {
        if (c->ai.force_action != -1) {
            c->obj.anim.newaction = c->ai.force_action;
        }
        fb = 0.59999996f;
    }
    if (c->ai.idle_update != 0) {
        UpdateCharacterIdle(c, c->obj.character);
    }
    UpdateAnimPacket((c->obj.dead != 0) ? &CModel[c->obj.die_model[0]] : c->obj.model,
                     &c->obj.anim, fb, c->obj.xz_distance);
    c->obj.frame++;
    switch (c->obj.character) {
    case 0xB1:
        gamesfx_pitch = 0x75A;
        gamesfx_effect_volume = 0x3FFF;
        GameSfxLoop(0x61, &c->obj.pos);
        break;
    case 0x9A:
        if (ai_type == 0x35 && 0.00666f < c->obj.xyz_distance) {
            switch (Level) {
            case 6:
                i = 0xCA;
                break;
            case 0x22:
                i = 0xC9;
                break;
            case 2:
                i = 0xCB;
                break;
            default:
                i = -1;
                break;
            }
            if (i != -1) {
                GameSfxLoop(i, &c->obj.pos);
            }
        }
        break;
    }
}
