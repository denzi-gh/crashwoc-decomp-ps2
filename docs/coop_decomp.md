# Coop decomp backlog

List of function that (likely) contribute to Multiplayer


## 1. Shared enemies - generic `Character[1..8]`
Host-authoritative enemy sharing (identity = `creature->i_aitab`); the capstone.

| Function | Address | Unit | State | Notes |
| --- | --- | --- | --- | --- |
| `MoveCreature` | 0x00230D28 | game/ai | ⬚ | **The blocker**: per-type AI with ≥3 `.rodata` jump tables; architectural blocker, stays asm, hook by address. |
| `ProcessCreatures` | 0x001D1FF0 | game | ◐ 96.8% | Suppression hook site. |
| `ManageCreatures` | 0x001CC218 | game | ◐ 92.6% | Local-proximity streaming; drives `i_aitab` identity. |
| `AddCreature` | 0x001CDAE8 | game | ◐ 93.0% | |
| `RemoveCreature` | 0x001D5780 | game | ✅ | |
| `FindNearestCreature` | 0x002349C0 | game/ai | ✅ | |
| `FindAIType` | 0x00234AB8 | game/ai | ✅ | |
| `FindAILabel` | 0x00235068 | game/ai | ✅ | |
| `InitAI` | 0x00234920 | game/ai | ✅ | |
| `ResetAI` | 0x0022F2C8 | game/ai | ◐ | |
| `LoadAI` | 0x0022EF10 | game/ai | ◐ | |
| `SaveAI` | 0x0022F148 | game/ai | ◐ | |
| ~31 `ai.c` behaviours | - | game/ai | mixed | 9 matched; boss `*_Defeated`/debris twins near-match; large Crunch brains asm (see §2d). |

## 2. Shared bosses - one host-authoritative brain per boss
Each follows the Fire Boss shape: `Init*` (struct), `Process*` (brain, usually
jump-table asm), `Draw*` (puppet), `*Level` wrapper, hit test.

### 2a. Fire Boss - DONE (proof)
`game/jeep`, Level 0x16. Six support fns ✅. Mod M shipped.

| Function | Address | State | Notes |
| --- | --- | --- | --- |
| `InitFireBoss` | - | ◐ 66% | |
| `DrawFireBoss` | - | ◐ 86% | |
| `ProcessFireBoss` | 0x00228D70 | ⬚ | Two jump tables, asm. |

### 2b. Weather Boss
`game/vehicle`, all ⬚.

| Function | Address | Notes |
| --- | --- | --- |
| `InitWeatherBoss` / `_a` | 0x0021EA20 / 0x002107A0 | |
| `ProcessWeatherBoss` / `_a` | 0x0021EDE0 / 0x00210B28 | |
| `DrawWeatherBoss` / `_a` | 0x0021EE10 / 0x00210960 | |
| `ProcessWeatherBossLevel` | 0x00214AC8 | |
| `DrawWeatherBossLevelExtra` | 0x00214980 | |
| `WeatherBossReset` | 0x00214588 | |
| `WeatherBossNextAction` | 0x0021ED08 | |
| `InitWeatherBossTarget` | 0x0021CFD8 | Targeting |
| `ProcessWeatherBossTarget` | 0x0021D040 | Targeting |
| `DrawWeatherBossTarget` | 0x00202818 | Targeting |
| `ProcessWBBolts` | 0x00214400 | Attacks |
| `ProcessLighteningHail` | 0x00213ED0 | Attacks |
| `ProcessWBIntro` | 0x00214C68 | Attacks |
| `GetWeatherBossPos` | 0x0021C7B8 | Position |
| `GetWeatherBossSpline` | 0x0021C7C8 | Position |
| `SetWeatherStartPos` | 0x0021C278 | Position |
| `InitZoffa` | 0x00205268 | UFOs |
| `DrawZoffaUFOs` | 0x00205440 | UFOs |
| `MoveZoffaUFO` | 0x00205BE8 | UFOs |
| `ZoffaSmoke` | 0x002056D8 | UFOs |
| `DrawZoffa` | 0x0021D300 | UFOs |

### 2c. Earth Boss
`game/vehicle`, all ⬚.

| Function | Address | Notes |
| --- | --- | --- |
| `InitEarthBoss` | 0x002171A8 | |
| `ProcessEarthBossActions` | 0x00217AB8 | |
| `ProcessEarthBossVortex` | 0x002195A8 | |
| `DrawEarthBoss` | 0x00217680 | |
| `ProcessEarthBossLevel` | 0x002196E8 | |
| `DrawEarthBossLevelExtra` | 0x0021F380 | |
| `EarthBossReset` | 0x0021F3B0 | |
| `PrintEarthBossAction` | 0x00217890 | |

### 2d. Crunch fights
`game/ai`, all ⬚.

| Function | Address | Notes |
| --- | --- | --- |
| `WaterCrunchFunction_Spladoosh` | 0x0022F928 | |
| `WaterCrunchFunction_Attack` | 0x00234BA0 | |
| `WaterCrunchFunction_Punch1` | 0x00234BF0 | |
| `WaterCrunchFunction_Punch2` | 0x00234CC8 | |
| `WaterCrunchFunction_Defeated` | 0x00234DA0 | |
| `SpaceCrunchFunction_NewPoint` | 0x00230AE0 | |
| `SpaceCrunchFunction_NewCount` | 0x00234EB0 | |
| `SpaceCrunchFunction_AttackCortex` | 0x00234EF0 | |
| `SpaceCrunchFunction_CheckCortexBack` | 0x00234F20 | |
| `SpaceCrunchFunction_PunchCortex` | 0x00234F28 | |
| `SpaceCortexFunction_CheckPunch` | 0x00234F88 | |
| `SpaceCortexFunction_Defeated` | 0x00234F90 | |
| `SpaceCortexFunction_TakeHit` | 0x00234FD0 | |
| `UpdateCRUNCHTIME` | 0x0022FAC8 | BossBar 0x0021C428 |
| `DrawCRUNCHTIME` | 0x002308C0 | BossBar 0x0021C428 |
| `ResetCRUNCHTIME` | 0x00234DD8 | BossBar 0x0021C428 |
| `FindGongBongerAnim` | 0x00230C20 | |

## 3. Glider levels - PvP + puppet fidelity
Transform already mirrored (`DrawGlider` ◐ 89%); decompile projectiles to cross
bullets/bombs between players. All ⬚ unless noted.

| Function | Address | Notes |
| --- | --- | --- |
| `AddGliderBullet` | 0x00208A28 | |
| `ProcessGliderBullets` | 0x00209990 | |
| `DrawGliderBullets` | 0x00209750 | |
| `CollideGliderBullets` | 0x00208BF0 | |
| `GliderBulletsHitThings` | 0x002091B0 | |
| `GrabGliderBullet` | 0x0021D8B0 | |
| `FreeGliderBullet` | 0x0021D958 | |
| `AddGliderBomb` | 0x0021CCD0 | |
| `ProcessGliderBombs` | 0x0020D898 | |
| `DrawGliderBombs` | 0x0020D730 | |
| `CollideGliderBombs` | 0x0020DCC0 | |
| `GliderBombsHitThings` | 0x00209528 | |
| `GliderFire` | 0x00201D88 | |
| `PickGliderTarget` | 0x0021CC00 | |
| `DrawGliderTarget` | 0x00211D38 | |
| `DrawTorpedoTarget` | 0x00211EB8 | |
| `AddGliderHitPoints` | 0x0021C018 | |
| `GetGliderHealthPercentage` | 0x0021C2D8 | |
| `ExplodeGlider` | 0x0021D0F8 | |
| `DeadGliderCoco` | 0x002041C8 | |
| `ControlGlider` | 0x00202DC0 | |
| `ProcessGliderMovement` | 0x00203860 | |
| `MoveGlider` | 0x00204588 | |
| `GliderCam` | 0x0020A658 | |
| `CheckGliderCollisions` | 0x0020C7B8 | |

## 4. Atlasphere (ball) levels - puppet + ball-vs-ball PvP
Ball transform mirrored (`DrawAtlas` ✅, `ObjectToAtlas` ◐ 92%); decompile the sim
+ ball-vs-ball collision. All ⬚.

| Function | Address | Notes |
| --- | --- | --- |
| `ControlAtlas` | 0x002152F8 | |
| `ProcessAtlas` | 0x00216BA8 | |
| `MoveAtlas` | 0x00216F48 | |
| `ProcessMovementAtlas` | 0x002154E0 | |
| `AdjustAtlasRotations` | 0x00215750 | |
| `ProcessAtlasAtlasCollisions` | 0x0021F9A8 | `_a` 0x00219BD0, the PvP bump |
| `ProcessAtlasTrail` | 0x0021A498 | |
| `ResetAtlas` | 0x00216848 | |
| `CheckAtlasGround` | 0x00216598 | |
| `TerrainAtlas` | 0x00215DF0 | |

## 5. Shooter / space levels
Puppets + shared enemies on rails. All ⬚.

| Function | Address (single / `_s`) | Notes |
| --- | --- | --- |
| `MovePlane` | 0x00209C38 | |
| `ProcessGunBoat` / `ProcessGunBoats` | 0x0020E1D8 / 0x0021E3F0 | |
| `ProcessBigGun` / `ProcessBigGuns` | 0x0020F810 / 0x0021E6C8 | |
| `ProcessBattleShip` / `ProcessBattleShips` | 0x00210290 / 0x0021E8C0 | |
| `CollideWithBattleShip` / `CollideWithBattleShips` | 0x0020FE00 / 0x0021C718 | |
| `ProcessSatellite` / `ProcessSatellites` | 0x00207CA8 / 0x0021D610 | |
| `ProcessSpaceStation` / `ProcessSpaceStations` | 0x002084A0 / 0x0021D7D8 | |
| `ProcessAsteroid` / `ProcessAsteroids` | 0x0020B2A0 / 0x0021DE80 | |
| `ProcessHovaBlimp` / `ProcessHovaBlimps` | 0x00207488 / 0x00207858 | |

## 6. Player↔player interaction & shared hazards
Friendly fire, shared damage, and the client→host enemy-hit intercept.

| Function | Address | Unit | State | Notes |
| --- | --- | --- | --- | --- |
| `KillPlayer` | 0x00200E90 | game/game_obj | ✅ | Already used by the mod. |
| `PlayerCreatureCollisions` | 0x001F9BC0 | | ◐ 76.7% | Enemy hit/kill site (damage intercept). |
| `HitCreatures` | 0x001FD158 | | ⬚ | Apply |
| `KillGameObject` | 0x001FD4A8 | | ⬚ | Primitive |
| `PlayerObjectAnimCollision` | 0x001F0558 | | ⬚ | |
| `ObjectCylinderCollision` | 0x001F9558 | | ⬚ | |
| `CollidePlayerPoint` | 0x00213D10 | game/vehicle | ⬚ | |
| `HitItems` | 0x00201140 | | ✅ | |
| `CrateCollisions` | 0x001FAB20 | | ⬚ | |
| `WumpaCollisions` | 0x001FBEF0 | | ⬚ | |

## 7. Vehicle-level dispatch
`ProcessVehicleLevel` 0x00214DE8 (game/vehicle) ⬚ - master dispatch mapping each
level id to its vehicle/boss `Process*`; typing it yields the exact hook map for §§2–8.

## 8. Per-vehicle / mount movement
Faithful motion for a riding partner. `game/move`, all ⬚.

| Function | Address |
| --- | --- |
| `MoveCRASH` | 0x00250A98 |
| `MoveCOCO` | 0x00252478 |
| `MoveSWIMMING` | 0x00252248 |
| `MoveSCOOTER` | 0x00252FB0 |
| `MoveSNOWBOARD` | 0x00253278 |
| `MoveOFFROADER` | 0x00253720 |
| `MoveMECH` | 0x00253CD8 |
| `MoveFIREENGINE` | 0x002544D0 |
| `MoveGYRO` | 0x002548A8 |
| `MoveSUBMARINE` | 0x002554E0 |
| `MoveMINECART` | 0x002556C8 |
| `MoveTub` | 0x00255EC8 |
| `MoveMINETUB` | 0x00256A18 |

## 9. Chase levels
Mirror the chaser position so both players see the same pursuit. `game/chase`, all ⬚.

| Function | Address |
| --- | --- |
| `InitChase` | 0x002592F8 |
| `UpdateChase` | 0x00259798 |
| `DrawChases` | 0x0025A588 |
| `UpdateChases` | 0x0025AB98 |
| `ChaseActive` | 0x0025AB58 |
| `NearestChaserDistance` | 0x0025AC98 |
| `UpdateCrateBallsOfFireDoors` | 0x00258DB0 |

## 10. Cameras & cutscenes
Shared-camera / split behaviour + synced cutscene skip (lower priority, all ⬚).

| Function | Address | Notes |
| --- | --- | --- |
| `GameCam` | - | |
| `JeepCam` | 0x00226C98 | |
| `GliderCam` | 0x0020A658 | Plus the tween helpers |
| `NuGCutScene*` system | 0x001AAE88… | Plus per-level cutscene triggers |

## Priority (value ÷ effort)
1. Shared enemies - `MoveCreature` gate + `ManageCreatures`/`AddCreature` + the §6 hit intercept.
2. Glider PvP - `AddGliderBullet` + `CollideGliderBullets`/`GliderBulletsHitThings`.
3. Remaining bosses (Weather, Earth, Crunch) - repeat the Fire-Boss ladder.
4. `ProcessVehicleLevel` - cheap; unlocks the hook map for 1–3.
5. Atlasphere ball-vs-ball (`ProcessAtlasAtlasCollisions`).
6. Per-vehicle `Move*` - only for authoritative vehicle sync.
7. Chases / cameras / cutscenes - polish.

See [decomp_agent.md](decomp_agent.md) for the matching workflow.
