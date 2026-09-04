/*
 * Unit: game/vehicle
 *
 * Functions:
 *   0x00201a38 StillLockedOnTarget
 *   0x00201b70 DrawGlider
 *   0x00201d88 GliderFire
 *   0x00202818 DrawWeatherBossTarget
 *   0x00202980 ControlGliderWeatherBoss
 *   0x00202dc0 ControlGlider
 *   0x00203860 ProcessGliderMovement
 *   0x00203f00 GliderSmoke
 *   0x002041c8 DeadGliderCoco
 *   0x002043b8 ProcessGliderMovementWB
 *   0x00204588 MoveGlider
 *   0x00205268 InitZoffa
 *   0x00205440 DrawZoffaUFOs
 *   0x00205548 FindZoffaRestartPoint
 *   0x002056d8 ZoffaSmoke
 *   0x00205978 TeleportManager
 *   0x00205be8 MoveZoffaUFO
 *   0x00206f80 DrawZoffaDebugStuff
 *   0x002070d0 InitHova
 *   0x002072f0 InitialiseHovaBlimps
 *   0x00207488 ProcessHovaBlimp
 *   0x00207858 ProcessAllHovaBlimps
 *   0x00207998 DrawAllHovaBlimps
 *   0x00207ca8 ProcessSatellite
 *   0x00207e60 InitSpaceStation
 *   0x002081a0 InitSpaceStations
 *   0x002082b8 DrawSpaceStation
 *   0x002084a0 ProcessSpaceStation
 *   0x00208a28 AddGliderBullet
 *   0x00208bf0 CollideGliderBullets
 *   0x002091b0 GliderBulletsHitThings
 *   0x00209528 GliderBombsHitThings
 *   0x00209750 DrawGliderBullets
 *   0x00209990 ProcessGliderBullets
 *   0x00209c38 MovePlane
 *   0x00209dd0 FarmReset
 *   0x0020a118 ProcessFarmLevel
 *   0x0020a2f8 GliderCamWeatherBoss
 *   0x0020a658 GliderCam
 *   0x0020ae70 InitAsteroid
 *   0x0020b198 DrawAsteroid
 *   0x0020b2a0 ProcessAsteroid
 *   0x0020b8a0 CheckForPotentialMidAirCollisions
 *   0x0020c498 SafeFromCollisions
 *   0x0020c5b0 LinesIntersectEllipse
 *   0x0020c7b8 CheckGliderCollisions
 *   0x0020d730 DrawGliderBombs
 *   0x0020d898 ProcessGliderBombs
 *   0x0020dcc0 CollideGliderBombs
 *   0x0020ded8 InitGunBoats
 *   0x0020e1d8 ProcessGunBoat
 *   0x0020e770 InitBigGuns
 *   0x0020f640 GetBigGunBestTarget
 *   0x0020f810 ProcessBigGun
 *   0x0020fe00 CollideWithBattleShip
 *   0x0020ff90 InitBattleShip
 *   0x00210150 InitBattleShips
 *   0x00210290 ProcessBattleShip
 *   0x002107a0 InitWeatherBoss_a
 *   0x00210960 DrawWeatherBoss_a
 *   0x00210b28 ProcessWeatherBoss_a
 *   0x00211d38 DrawGliderTarget
 *   0x00211eb8 DrawTorpedoTarget
 *   0x00212618 FireFlyReset
 *   0x00212918 DrawFireFlyLevelExtra
 *   0x00212b08 ProcessFireFlyIntro
 *   0x00213110 ProcessFireFlyLevel
 *   0x00213308 WeatherResearchReset
 *   0x00213598 SpaceArenaReset
 *   0x00213880 DrawSpaceArenaLevelExtra
 *   0x00213a70 ProcessSpaceArenaLevel
 *   0x00213d10 CollidePlayerPoint
 *   0x00213ed0 ProcessLighteningHail
 *   0x00214030 UnleashLighteningHail
 *   0x00214220 DrawWBBolts
 *   0x00214400 ProcessWBBolts
 *   0x00214588 WeatherBossReset
 *   0x00214980 DrawWeatherBossLevelExtra
 *   0x00214ac8 ProcessWeatherBossLevel
 *   0x00214c68 ProcessWBIntro
 *   0x00214de8 ProcessVehicleLevel
 *   0x00214f40 DrawExtraCreatures
 *   0x00215140 GetCurrentLevelObjectives
 *   0x002152f8 ControlAtlas
 *   0x002154e0 ProcessMovementAtlas
 *   0x00215750 AdjustAtlasRotations
 *   0x002159f0 ProcessPlatformsAtlas
 *   0x00215df0 TerrainAtlas
 *   0x00216598 CheckAtlasGround
 *   0x00216848 ResetAtlas
 *   0x00216ba8 ProcessAtlas
 *   0x00216f48 MoveAtlas
 *   0x002171a8 InitEarthBoss
 *   0x00217420 JonnyParticles
 *   0x00217680 DrawEarthBoss
 *   0x00217890 PrintEarthBossAction
 *   0x00217ab8 ProcessEarthBossActions
 *   0x00218d00 ProcessRumblePanel
 *   0x002190e8 RumbleHeadUpDisplay
 *   0x002195a8 ProcessEarthBossVortex
 *   0x002196e8 ProcessEarthBossLevel
 *   0x00219bd0 ProcessAtlasAtlasCollisions_a
 *   0x00219f90 TrailPointInPoly
 *   0x0021a138 SetTrailPos
 *   0x0021a498 ProcessAtlasTrail
 *   0x0021a940 AddRockVel
 *   0x0021adc0 ShootRoksSkyward
 *   0x0021ae90 AllRoksSkyward
 *   0x0021af28 DrawJeepRocks
 *   0x0021b088 ProcessJeepRock
 *   0x0021b410 ProcessJeepRocks
 *   0x0021b598 ProcessRockRockCollisions
 *   0x0021b718 RumbleCam
 *   0x0021ba08 ProcessVehMasks
 *   0x0021bc88 DrawVehMasks
 *   0x0021be58 MoveVehicle
 *   0x0021bed8 GetRumbleCrunchRoks
 *   0x0021bf50 GetRumblePlayerRoks
 *   0x0021bfc8 GetRumbleTotalRoks
 *   0x0021c018 AddGliderHitPoints
 *   0x0021c048 ResetVehicleLevel
 *   0x0021c1b0 DrawVehicleTrail
 *   0x0021c1e8 DrawAtlas
 *   0x0021c270 VehicleSetup
 *   0x0021c278 SetWeatherStartPos
 *   0x0021c2c8 GetRumblePlayerHealthPercentage
 *   0x0021c2d8 GetGliderHealthPercentage
 *   0x0021c2f8 GetMaxLevelObjectives
 *   0x0021c370 LoadVehicleStuff
 *   0x0021c3b8 ObjectToAtlas
 *   0x0021c428 BossBar
 *   0x0021c558 InitVehMasks
 *   0x0021c580 InitVehMask
 *   0x0021c5f0 SetNewMaskStuff
 *   0x0021c718 CollideWithBattleShips
 *   0x0021c798 ProcessCrashteroidsIntro
 *   0x0021c7b8 GetWeatherBossPos
 *   0x0021c7c8 GetWeatherBossSpline
 *   0x0021c7d8 GetBattleShipBestTarget
 *   0x0021c938 GetGunBoatBestTarget
 *   0x0021caa0 GetZoffaBestTarget
 *   0x0021cc00 PickGliderTarget
 *   0x0021ccd0 AddGliderBomb
 *   0x0021ce80 GliderWeatherBossRailStuff
 *   0x0021ceb0 InitGlider
 *   0x0021cfd8 InitWeatherBossTarget
 *   0x0021d040 ProcessWeatherBossTarget
 *   0x0021d0f8 ExplodeGlider
 *   0x0021d1d8 DeadGliderWB
 *   0x0021d2c8 DrawGliderDebugStuff
 *   0x0021d300 DrawZoffa
 *   0x0021d3b8 FindFreeHova
 *   0x0021d3f0 InitSatellite
 *   0x0021d4e8 DrawSatellite
 *   0x0021d610 ProcessSatellites
 *   0x0021d668 DrawSatellites
 *   0x0021d7d8 ProcessSpaceStations
 *   0x0021d848 DrawSpaceStations
 *   0x0021d8b0 GrabGliderBullet
 *   0x0021d958 FreeGliderBullet
 *   0x0021da20 InitPlane
 *   0x0021db00 DrawPlane
 *   0x0021db68 DrawFarmLevelExtra
 *   0x0021dbf0 FindFreeAsteroid
 *   0x0021dc90 GetRandomGliderLevelEdgePoint
 *   0x0021dd48 CountAsteroids
 *   0x0021dda0 RespawnAsteroids
 *   0x0021de38 InitAsteroids
 *   0x0021de80 ProcessAsteroids
 *   0x0021df38 DrawAsteroids
 *   0x0021df90 PossIntersect
 *   0x0021e128 GetCurrentFarmObjectives
 *   0x0021e158 FindSurfaceType
 *   0x0021e210 InitGunBoat
 *   0x0021e330 DrawGunBoat
 *   0x0021e3f0 ProcessGunBoats
 *   0x0021e448 InitBigGun
 *   0x0021e618 DrawBigGun
 *   0x0021e6c8 ProcessBigGuns
 *   0x0021e720 DrawBigGuns
 *   0x0021e818 DrawBattleShip
 *   0x0021e8c0 ProcessBattleShips
 *   0x0021e918 PrintWeatherBossAction
 *   0x0021ea20 InitWeatherBoss
 *   0x0021ea40 ProcessBazookaToken
 *   0x0021eb08 DrawBazookaToken
 *   0x0021eb58 FireWBBolt
 *   0x0021ed08 WeatherBossNextAction
 *   0x0021ed80 GetCurrentWeatherBossObjectives
 *   0x0021ede0 ProcessWeatherBoss
 *   0x0021ee10 DrawWeatherBoss
 *   0x0021ee40 GetCurrentFireFlyObjectives
 *   0x0021ee88 DrawWeatherResearchLevelExtra
 *   0x0021ef90 ProcessWeatherResearchLevel
 *   0x0021eff8 GetCurrentWeatherResearchObjectives
 *   0x0021f040 GetCurrentSpaceArenaObjectives
 *   0x0021f078 InitLighteningHail
 *   0x0021f0d8 DrawLighteningHail
 *   0x0021f1b0 FindFreeWBBoltOfType
 *   0x0021f1f8 FindFreeWBBolt
 *   0x0021f230 InitWBBolts
 *   0x0021f2b0 InitWBIntro
 *   0x0021f380 DrawEarthBossLevelExtra
 *   0x0021f3b0 EarthBossReset
 *   0x0021f410 GetCurrentRumbleObjectives
 *   0x0021f420 InitJeepRocks
 *   0x0021f448 KillAtlasphere
 *   0x0021f4b0 InitTrail
 *   0x0021f4e8 UnembedRayCastAtlas
 *   0x0021f578 UnembedRayCastAtlasSimple
 *   0x0021f5c8 InitAtlas
 *   0x0021f7d8 DrawJonny
 *   0x0021f8d8 UpdateRumbleCamTween
 *   0x0021f9a0 InitRumblePanel
 *   0x0021f9a8 ProcessAtlasAtlasCollisions
 *   0x0021f9e0 CheckAtlasVortex
 *   0x0021fa98 FadeOutLastTrail
 *   0x0021fb30 PointLineSide
 *   0x0021fb68 PointsSame
 *   0x0021fbb8 FindTrailAng
 *   0x0021fbf0 FindJeepRock
 *   0x0021fc30 KeepHoldOnRock
 *   0x0021fcb8 AddRock
 *   0x0021fce0 DrawJeepRock
 *   0x0021fdc0 SmashRockIntoTwo
 *   0x0021feb8 CheckAgainstRocks
 */

#include "creature.h"

extern s32 Level;
extern struct numtx_s mTEMP;

/* gp-relative angle scale constants used to convert the glider's float roll/
 * pitch/yaw into NuMtx integer rotation units. */
extern float D_0062D91C;
extern float D_0062D920;
extern float D_0062D924;
extern struct nuvec_s D_006B75A0;   /* fixed glider translation (level 0xD) */

/* per-level glider model indices into CModel (-1 == none).  These live far
 * from $gp (retail uses %hi/%lo absolute loads); declare as incomplete arrays
 * so the compiler does not treat them as gp-relative small data. */
extern s8 D_0056236E[];
extern s8 D_005623B9[];
extern s8 D_005623C3[];

extern s32 AtlasFrame;
extern s32 tttt;
extern s8 D_0056238B[];   /* per-level atlasphere model index (far from $gp) */

extern s32 temp_xzmomset;
extern float D_00560D44[]; /* ball vertical lift offset (far from $gp) */
extern float D_0062DE9C;   /* ball momentum -> velocity divisor */
extern float D_0062DEA0;   /* ball 'boing' vertical momentum */

void NuMtxSetRotationX(struct numtx_s *m, s32 r);
void NuMtxSetRotationZ(struct numtx_s *m, s32 r);
void NuMtxRotateX(struct numtx_s *m, s32 r);
void NuMtxRotateY(struct numtx_s *m, s32 r);
void NuMtxTranslate(struct numtx_s *m, struct nuvec_s *v);
void NuQuatToMtx(struct nuquat_s *q, struct numtx_s *m);
void NuHGobjRndr(struct NUHGOBJ_s *hobj, struct numtx_s *m, s32 nlayers,
                 short *layer);

/* Faithful full-C near-match (state=asm): structure/frame/reg-mask exact, but
 * ee-gcc rematerializes %hi(mTEMP) in each of the Level==0x1A/else blocks and
 * fills the `bne` delay slot with a speculative scale load, where retail shares
 * one `lui $s1,%hi(mTEMP)` in that delay slot (+4 bytes). See docs/notes.md
 * ("DrawGlider") and the recorded no_progress blocker. */
void DrawGlider(struct creature_s *c)
{
    struct NEWBUGGY *b;
    float lift;
    float scale;
    s8 idx;

    b = c->Buggy;
    lift = ((Level != 0xD) && (Level != 0x1A)) ? 15.0f : 0.0f;

    if (b->enable && (Level == 0xD)) {
        NuMtxSetRotationX(&mTEMP, lift * D_0062D91C);
        NuMtxRotateY(&mTEMP, 0);
        NuMtxTranslate(&mTEMP, &D_006B75A0);
    } else if (Level == 0x1A) {
        scale = D_0062D920;
        NuMtxSetRotationZ(&mTEMP, b->roll * scale);
        NuMtxRotateX(&mTEMP, (b->pitch + b->pitch) * scale);
        NuMtxRotateY(&mTEMP, b->yaw * scale);
        NuMtxTranslate(&mTEMP, &b->pos);
    } else {
        scale = D_0062D924;
        NuMtxSetRotationZ(&mTEMP, b->roll * scale);
        NuMtxRotateX(&mTEMP, (b->pitch + lift) * scale);
        NuMtxRotateY(&mTEMP, b->yaw * scale);
        NuMtxTranslate(&mTEMP, &b->pos);
    }

    if (Level == 0x12) {
        idx = D_005623C3[0];
    } else if (Level == 0x1A) {
        idx = D_005623B9[0];
    } else {
        idx = D_0056236E[0];
    }
    if (idx != -1) {
        DrawCharacterModel(&CModel[idx], &c->obj.anim, &mTEMP, 0, 1, 0,
                           &c->mtxLOCATOR[1][0], &c->momLOCATOR[1][0], &c->obj);
    }
}

void DrawAtlas(struct creature_s *c)
{
    struct numtx_s m;
    struct NEWBUGGY *b;
    s8 idx;

    b = c->Buggy;
    if (AtlasFrame == 0x705) {
        tttt++;
    }
    NuQuatToMtx(&b->rotquat, &m);
    NuMtxTranslate(&m, &b->ball_pos);
    idx = D_0056238B[0];
    if (idx != -1) {
        NuHGobjRndr(CModel[idx].hobj, &m, 1, 0);
    }
}

/* Seeds the ball (atlasphere) NEWBUGGY transform from a character's object:
 * ball_pos <- obj->pos (+ vertical lift), ball_vel <- obj->mom / divisor.
 * Faithful near-match (state=asm): first half byte-exact; the mom/divisor FP
 * load order and the `boing` beql delay-slot differ (scheduling). */
void ObjectToAtlas(struct obj_s *obj, struct creature_s *c)
{
    struct NEWBUGGY *b = c->Buggy;

    b->ball_pos.x = obj->pos.x;
    b->ball_pos.y = obj->pos.y + D_00560D44[0];
    b->ball_pos.z = obj->pos.z;
    if (temp_xzmomset != 0) {
        b->ball_vel.x = obj->mom.x / D_0062DE9C;
        b->ball_vel.z = obj->mom.z / D_0062DE9C;
    }
    if (obj->boing != 0) {
        obj->mom.y = D_0062DEA0;
    }
    b->ball_vel.y = obj->mom.y;
}

/* ------------------------------------------------------------------ */
/* Small leaf functions (game/vehicle)                                 */
/* ------------------------------------------------------------------ */

extern s32 JeepInControl;
extern s32 NumRockPanel;
extern s32 D_005B7918[];   /* rumble player health % (far) */
extern s32 D_005B7CA0[];   /* current rumble objectives (far) */
extern struct nuvec_s D_005B44F8[]; /* weather boss position (far) */
extern struct spline_s D_005B4548;  /* weather boss spline (far) */
extern struct nuvec_s D_006B75B0;   /* weather start pos temp (far) */
extern struct NEWBUGGY PlayerGlider;
extern struct nuvec_s v000;

extern void ProcessFireFlyIntro(void);
extern void InitWeatherBoss_a(void);
extern void ProcessWeatherBoss_a(void *wb);
extern void DrawWeatherBoss_a(void *wb);
extern void AddRockVel(struct nuvec_s *pos, struct nuvec_s *vel, s32 owner);
extern void *memset(void *dst, s32 val, s32 len);
extern char *best_cRPos;

extern s32 WeatherBoss[];   /* weather boss state block (far) */
extern u8 VehicleMask[];    /* veh mask block, memset 0x2E8 (far) */

/* HovaBlimp array, stride 0x134, 6 entries. */
struct hovablimp_s {
    s32 active;                /* 0x00 */
    char pad_04[0x134 - 0x04];
};
extern struct hovablimp_s HovaBlimp[];

/* JeepRock array, stride 0x3D4, 6 entries. */
struct jeeprock_s {
    char pad_00[0x18];
    s32 exists;                /* 0x18 */
    char pad_1C[0x3D4 - 0x1C];
};
extern struct jeeprock_s JeepRock[];

extern s32 RumbleDisplayMode;
extern s32 RumbleStoreTotalRoks;

s32 GetRumbleTotalRoks(void)
{
    s32 count;
    s32 i;

    if (RumbleDisplayMode == -1) {
        return RumbleStoreTotalRoks;
    }
    count = 0;
    for (i = 0; i < 6; i++) {
        if (JeepRock[i].exists != 0) {
            count++;
        }
    }
    RumbleStoreTotalRoks = count;
    return count;
}

void AddGliderHitPoints(void)
{
    struct NEWBUGGY *b = player->Buggy;

    b->health += 25;
    if (b->health > 100) {
        b->health = 100;
    }
}

void VehicleSetup(void)
{
    JeepInControl = 0;
}

void SetWeatherStartPos(void *x)
{
    D_006B75B0 = *(struct nuvec_s *)((char *)x + 0x6C);
    PlayerGlider.pos = D_006B75B0;
}

s32 GetRumblePlayerHealthPercentage(void)
{
    return D_005B7918[0];
}

s32 GetGliderHealthPercentage(struct creature_s *c)
{
    struct NEWBUGGY *b = c->Buggy;

    if (b != 0) {
        return b->health;
    }
    return 100;
}

extern void LoadWesternArenaData(void);
extern s32 jonfirst;

void LoadVehicleStuff(void)
{
    switch (Level) {
    case 3:
        LoadWesternArenaData();
        break;
    case 0xD:
        jonfirst = 0;
        break;
    }
}

void InitVehMasks(void)
{
    memset(VehicleMask, 0, 0x2E8);
}

void ProcessCrashteroidsIntro(void)
{
    ProcessFireFlyIntro();
}

struct nuvec_s *GetWeatherBossPos(void)
{
    return D_005B44F8;
}

struct spline_s *GetWeatherBossSpline(void)
{
    return &D_005B4548;
}

void GliderWeatherBossRailStuff(struct creature_s *c)
{
    if (best_cRPos != 0) {
        *(struct nuvec_s *)((char *)c + 0x60) =
            *(struct nuvec_s *)(best_cRPos + 0x18);
    }
}

s32 GetCurrentFarmObjectives(void)
{
    s32 count = 0;
    s32 i;

    for (i = 0; i < 6; i++) {
        if (HovaBlimp[i].active != 0) {
            count++;
        }
    }
    return count;
}

void InitWeatherBoss(void)
{
    InitWeatherBoss_a();
}

struct MYDRAW;
extern struct MYDRAW IconMainDraw;
extern s32 BazookaIconOn;
extern struct nuvec_s BazookaTokenCurrentPos;
extern void NuMtxSetTranslation(struct numtx_s *m, struct nuvec_s *v);
extern s32 MyDrawModelNew(struct MYDRAW *Draw, struct numtx_s *mC,
                          struct numtx_s *loc_mtx);

void DrawBazookaToken(void)
{
    if (BazookaIconOn) {
        NuMtxSetTranslation(&mTEMP, &BazookaTokenCurrentPos);
        MyDrawModelNew(&IconMainDraw, &mTEMP, 0);
    }
}

void ProcessWeatherBoss(void)
{
    if (WeatherBoss[0] != 0) {
        ProcessWeatherBoss_a(WeatherBoss);
    }
}

void DrawWeatherBoss(void)
{
    if (WeatherBoss[0] != 0) {
        DrawWeatherBoss_a(WeatherBoss);
    }
}

s32 GetCurrentRumbleObjectives(void)
{
    return D_005B7CA0[0];
}

void InitJeepRocks(void)
{
    s32 i;

    for (i = 5; i >= 0; i--) {
        JeepRock[i].exists = 0;
    }
}

void InitRumblePanel(void)
{
    NumRockPanel = 0;
}

void AddRock(struct nuvec_s *pos, s32 owner)
{
    AddRockVel(pos, &v000, owner);
}

/* ------------------------------------------------------------------ */

/* BattleShipList: stride 0x554, 6 entries. */
struct battleship_s {
    s32 active;                /* 0x000 */
    char pad_004[0x510 - 0x004];
    s32 field510;              /* 0x510 */
    char pad_514[0x554 - 0x514];
};
extern struct battleship_s BattleShipList[];

/* BigGunList: stride 0x154, 12 entries. */
struct biggun_s {
    s32 active;                /* 0x000 */
    char pad_004[0x120 - 0x004];
    s32 field120;              /* 0x120 */
    char pad_124[0x154 - 0x124];
};
extern struct biggun_s BigGunList[];

/* SpaceStationList: stride 0xD0, 3 entries. */
struct spacestation_s {
    char pad_00[0x40];
    s32 field40;               /* 0x40 */
    char pad_44[0xD0 - 0x44];
};
extern struct spacestation_s SpaceStationList[];

/* trail array: stride 0x20, 128 entries. */
struct trail_s {
    f32 field0;                /* 0x00 */
    char pad_04[0x20 - 0x04];
};
extern struct trail_s trail[128];
extern f32 D_0062DF00;
extern s32 trailpt;
extern s32 trailair;

extern s32 EarthBoss[];    /* far; field 0x10 at [4] */
extern u8 PlayerAtlas[];   /* far */
extern void ProcessAtlasAtlasCollisions_a(void *atlas, void *boss);
extern void KillGameObject(struct obj_s *obj, s32 type);

s32 GetCurrentFireFlyObjectives(void)
{
    s32 count = 0;
    s32 i;

    for (i = 0; i < 6; i++) {
        struct battleship_s *bs = &BattleShipList[i];
        if (bs->active != 0) {
            count += bs->field510 > 0;
        }
    }
    return count;
}

s32 GetCurrentWeatherResearchObjectives(void)
{
    s32 count = 0;
    s32 i;

    for (i = 0; i < 12; i++) {
        if (BigGunList[i].active != 0) {
            if (BigGunList[i].field120 != 2) {
                count++;
            }
        }
    }
    return count;
}

s32 GetCurrentSpaceArenaObjectives(void)
{
    s32 count = 0;
    s32 i;

    for (i = 0; i < 3; i++) {
        if (SpaceStationList[i].field40 != 0) {
            count++;
        }
    }
    return count;
}

void KillAtlasphere(struct NEWBUGGY *b)
{
    struct creature_s *c = b->owner;
    struct creature_s *o;

    if (c != 0) {
        b->fieldC = 1;
        *((s8 *)c + 0x17E) = 0;
        b->ball_vel = v000;
        o = b->owner;
        if (*((s8 *)o + 0x149) == 0) {
            KillGameObject(&o->obj, 0xB);
        }
    }
}

void InitTrail(void)
{
    s32 i;

    for (i = 127; i >= 0; i--) {
        trail[i].field0 = D_0062DF00;
    }
    trailpt = 0;
    trailair = 0;
}

extern struct nuvec_s D_0061F750[]; /* fixed ray direction, far from $gp */
extern f32 D_0062DF04;
extern s32 TryUnembeddPointDirSimple(struct nuvec_s *pos, struct nuvec_s *dir,
                                     s16 *handle, s32 n, f32 x, f32 y);

void UnembedRayCastAtlasSimple(struct NEWBUGGY *b, s16 *handle)
{
    struct nuvec_s dir = D_0061F750[0];

    TryUnembeddPointDirSimple(&b->ball_pos, &dir, handle, 5, b->atlas_rotparam,
                              D_0062DF04);
}

void ProcessAtlasAtlasCollisions(void)
{
    if (EarthBoss[4] > 0) {
        ProcessAtlasAtlasCollisions_a(PlayerAtlas, EarthBoss);
    }
}

/* ------------------------------------------------------------------ */

/* SatelliteList: stride 0x124, 9 entries. */
struct satellite_s {
    s32 active;                /* 0x00 */
    char pad_04[0x124 - 0x04];
};
extern struct satellite_s SatelliteList[];

extern s32 D_005A74C8[];   /* far scratch, zeroed each ProcessSpaceStations */
extern void *app_fnt;

extern void NuRndrTrail(s32 pt, struct trail_s *trail, s32 count);
extern void DrawJeepTrails(void);
extern void NuFntSet(void *fnt);
extern void NuFntSetPen(s32 col);
extern void NuFntScale(s32 x, s32 y);
extern void ProcessSatellite(struct satellite_s *s);
extern void ProcessSpaceStation(struct spacestation_s *s);
extern void DrawSpaceStation(struct spacestation_s *s);

void DrawVehicleTrail(void)
{
    if (Level == 0) {
        NuRndrTrail(trailpt, trail, 0x80);
    }
    DrawJeepTrails();
}

void DrawGliderDebugStuff(void)
{
    NuFntSet(app_fnt);
    NuFntSetPen(0x7F7F7F7F);
    NuFntScale(0x10, 0x10);
}

struct hovablimp_s *FindFreeHova(void)
{
    s32 i;

    for (i = 0; i < 6; i++) {
        if (HovaBlimp[i].active == 0) {
            return &HovaBlimp[i];
        }
    }
    return 0;
}

void ProcessSatellites(void)
{
    s32 i;

    for (i = 0; i < 9; i++) {
        if (SatelliteList[i].active != 0) {
            ProcessSatellite(&SatelliteList[i]);
        }
    }
}

void ProcessSpaceStations(void)
{
    s32 i;

    D_005A74C8[0] = 0;
    for (i = 0; i < 3; i++) {
        if (SpaceStationList[i].field40 != 0) {
            ProcessSpaceStation(&SpaceStationList[i]);
        }
    }
}

void DrawSpaceStations(void)
{
    s32 i;

    for (i = 0; i < 3; i++) {
        if (SpaceStationList[i].field40 != 0) {
            DrawSpaceStation(&SpaceStationList[i]);
        }
    }
}

struct asteroid_s {
    s32 active;                /* 0x00 */
    char pad_04[0x2C - 0x04];
    f32 radius;                /* 0x2C */
    char pad_30[0x4C - 0x30];
};
extern struct asteroid_s AsteroidList[];   /* far; 100 entries, InitAsteroid's argument is always 0 */
extern void InitAsteroid(s32 x);

s32 CountAsteroids(void)
{
    f32 sum = 0.0f;
    s32 i;

    for (i = 0; i < 100; i++) {
        if (AsteroidList[i].active != 0) {
            sum += AsteroidList[i].radius * AsteroidList[i].radius * 0.25f;
        }
    }
    return (s32)sum;
}

void InitAsteroids(void)
{
    s32 i;

    memset(AsteroidList, 0, 0x1DB0);
    for (i = 0x1D; i >= 0; i--) {
        InitAsteroid(0);
    }
}

struct wbbolt_s {
    s32 active;                /* 0x00 */
    char pad_04[0x24 - 0x04];
    s32 type;                  /* 0x24 */
    char pad_28[0x30 - 0x28];
};
extern struct wbbolt_s BoltList[];

struct wbbolt_s *FindFreeWBBoltOfType(s32 type)
{
    struct wbbolt_s *b;
    s32 i;

    b = BoltList;
    for (i = 0; i < 0x78; i++) {
        if (b->active != 0) {
            if (b->type == type) {
                return b;
            }
        }
        b++;
    }
    return 0;
}

struct wbbolt_s *FindFreeWBBolt(void)
{
    struct wbbolt_s *b;
    s32 i;

    b = BoltList;
    for (i = 0; i < 0x78; i++) {
        if (b->active == 0) {
            return b;
        }
        b++;
    }
    return 0;
}

extern void DrawEarthBoss(void);
extern void DrawJeepRocks(void);
extern void DrawVehMasks(void);

void DrawEarthBossLevelExtra(void)
{
    DrawEarthBoss();
    DrawJeepRocks();
    DrawVehMasks();
}

f32 PointLineSide(struct nuvec_s *p, struct nuvec_s *a, struct nuvec_s *b)
{
    f32 px = p->x, pz = p->z;
    f32 ax = a->x, az = a->z;
    f32 bx = b->x, bz = b->z;

    return (bx - px) * (bz - az) - (bz - pz) * (bx - ax);
}

s32 PointsSame(struct nuvec_s *a, struct nuvec_s *b)
{
    s32 r = 0;

    if (a->x == b->x) {
        if (a->y == b->y) {
            if (a->z == b->z) {
                r = 1;
            }
        }
    }
    return r;
}

extern void NuVecSub(struct nuvec_s *d, struct nuvec_s *a, struct nuvec_s *b);
extern s32 NuAtan2D(f32 x, f32 z);

u16 FindTrailAng(struct nuvec_s *a, struct nuvec_s *b)
{
    struct nuvec_s d;

    NuVecSub(&d, b, a);
    return (u16)(NuAtan2D(d.x, d.z) - 0x2000);
}

struct jeeprock_s *FindJeepRock(void)
{
    s32 i;

    for (i = 0; i < 6; i++) {
        if (JeepRock[i].exists == 0) {
            return &JeepRock[i];
        }
    }
    return 0;
}
