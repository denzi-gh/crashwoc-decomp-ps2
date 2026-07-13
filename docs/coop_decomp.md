# Coop decomp backlog — functions to decompile for a flawless / custom coop

A working list of the retail functions that a **complete, custom two-player coop**
(the `mods/coop/` mod, ladder in [COOP_ROADMAP.md](../mods/coop/COOP_ROADMAP.md))
would want decompiled and typed. It is a *decomp backlog*, not a promise: most of
these can be **hooked by address without a byte-match** once their struct/args are
typed — the value of decompiling is the **types and the exact semantics** the mod
needs to mirror, suppress, replay, or intercept them.

A function earns a place here when the mod must do one of:
- **Mirror** its state onto the remote puppet (draw/pose fidelity),
- **Suppress + apply** it authoritatively (host runs it, client mirrors — shared
  enemies/bosses),
- **Intercept** a hit/kill/spawn (client→host damage, PvP, shared hazards),
- **Type a struct** so a mailbox field can carry it.

**State legend:** ✅ matching · ◐ equivalent / faithful near-match in tree · ⬚ asm
(not started). **Fuzzy** = the objdiff `current` score (plain compile of `src/`)
from a fresh `ninja report` (regenerated 2026-07-13). It is how close the honest
compile already is: `0%` means no C written yet; a promoted `matching` function
reads `100%`, a near-match reads its partial score. States are authoritative from
`config/pal103/status/**/*.toml`; check the manifest before starting one. Addresses
are PAL v1.03 (`SLES_503.86`); every symbol below is in the SDK symbol table, so
the mod can already reference it by name.

> **The Fire Boss is the executed proof** (Stage 4, ladder D1–M). It shows the
> whole pattern end-to-end: type the state (D1/D2), add the `CoopBossState`
> mailbox field (C, v11), designate a host (B, `--host`), and replace the brain
> with a host-publish / client-puppet hook (M). Every boss/enemy below is a
> repeat of that shape.

---

## 1. Shared enemies — generic `Character[1..8]` (the big payoff)

Today each instance streams and simulates its own enemies from `AITab[]` by local
proximity, so peers never see each other's fights. Sharing them host-authoritatively
(identity = `creature->i_aitab`, never slot index) is the capstone. Blocked chiefly
on `MoveCreature`.

| Function | Addr | Unit | State | Fuzzy | Why coop needs it |
|---|---|---|---|---|---|
| `MoveCreature` | 0x00230D28 | game/ai | ⬚ | 0% | **The blocker.** Per-type AI with unknown side effects (projectiles/sfx/effects) that pos+action mirroring alone won't reproduce. Must be typed to know what to replay vs suppress. **Architectural blocker recorded**: 15,352 B with ≥3 `.rodata` jump tables — the hybrid pipeline cannot own jump-table data, so it stays `asm` until that lands. Hook by address. |
| `ProcessCreatures` | 0x001D1FF0 | game/creature | ◐ equiv | 96.8% | The suppression hook site: slot 0 (`MovePlayer`) always native; enemy slots with a fresh remote record skip `MoveCreature` and apply pos/hdg/action/dead. Full C exists. |
| `ManageCreatures` | 0x001CC218 | game/creature | ◐ | 92.6% | Streams creatures from `AITab[]` by *local* proximity — the reason slot indices diverge and identity must be `i_aitab`. Type it to reconcile spawn/despawn. |
| `AddCreature` | 0x001CDAE8 | game/creature | ◐ | 93.0% | Client spawns a missing shared enemy: `AddCreature(character, slot, i_aitab)`. Need its preconditions. |
| `RemoveCreature` | 0x001D5780 | game/creature | ✅ | 100% | Orphaned-local despawn path. Already matched. |
| `FindNearestCreature` / `FindAIType` / `FindAILabel` | 0x002349C0 / 0x00234AB8 / 0x00235068 | game/ai | ✅ | 100% | Identity + lookup helpers for reconciling records to live creatures. **All three matched** (`FindAILabel` needed a `goto` loop). |
| `InitAI` / `ResetAI` / `LoadAI` / `SaveAI` | 0x00234920 / 0x0022F2C8 / 0x0022EF10 / 0x0022F148 | game/ai | ✅ / ◐ / ◐ / ◐ | 100% / 37% / n | AITab lifecycle. **`InitAI` matched.** `ResetAI`/`LoadAI`/`SaveAI` are faithful full-C near-matches (structs `aitab_s`/`aitype_s`/`rail_s`/`aipath_s` typed, semantics exact) blocked on whole-function callee-saved allocation + loop-invariant hoisting; stay `asm`. |
| the ~31 `ai.c` behaviours (`PlayerLateralInRange`, `RatioDifferenceAlongLine`, …) | 0x0022E…–0x0023A… | game/ai | ✅ ×9 / ◐ | mixed | **9 matched** (`RatioDifferenceAlongLine`, `ResetDRAINDAMAGE`, `FindGongBongerAnim`, `WaterCrunchFunction_Attack`, `SpaceCrunchFunction_{PunchCortex,AttackCortex,CheckCortexBack}`, `SpaceCortexFunction_{CheckPunch,TakeHit}`). `PlayerLateralInRange`/`OutOfRange` + the boss `*_Defeated`/`NewCount` + the debris `Punch1`/`Punch2` twins are faithful near-matches (semantics + `fptodp`/`dpcmp`/`dpsub` u64-double typing captured) blocked on FP/register scheduling; stay `asm`. Remaining `asm`: `WaterCrunchFunction_Spladoosh`, `SpaceCrunchFunction_NewPoint`, `Update/Draw/Reset CRUNCHTIME`, `Update/Draw DRAINDAMAGE` (large §2d Crunch-boss brains). |

**Hit intercept** (client→host enemy damage): see §6.

---

## 2. Shared bosses — one host-authoritative brain per boss

Each boss follows the Fire Boss shape: `Init*` (Rosetta-stone struct), `Process*`
(the brain — usually `.rodata` jump tables, stays asm), `Draw*` (what the puppet
must keep valid), the `*Level` wrapper (plumbing), and a hit test. Type the struct,
add a per-boss mailbox block (or generalise `CoopBossState`), replace the brain.

### 2a. Fire Boss (Py-Ro / Ashes chase) — **DONE (proof)**
`game/jeep`, Level 0x16. Six support fns matched ✅ 100% (`FireBossReset`,
`DrawFireBossLevelExtra`, `GetTotalFireBossObjectives`,
`GetCurrentFireBossObjectives`, `InitJeepBalloons`, `FindJeepBalloon`);
`InitFireBoss` ◐ 66% (near-match, kept asm); `DrawFireBoss` ◐ 86%;
`ProcessFireBoss` 0x00228D70 ⬚ 0% (two jump tables, stays asm — sync surface in
[notes.md](notes.md#fire-boss-sync-surface)). Mod M shipped.

### 2b. Weather Boss (wind / Zoffa UFOs, glider) — `game/vehicle`, all ⬚ 0%
| Function | Addr | Fuzzy | Note |
|---|---|---|---|
| `InitWeatherBoss` / `_a` | 0x0021EA20 / 0x002107A0 | 0% | struct Rosetta stone |
| `ProcessWeatherBoss` / `_a` | 0x0021EDE0 / 0x00210B28 | 0% | brain |
| `DrawWeatherBoss` / `_a` | 0x0021EE10 / 0x00210960 | 0% | puppet render |
| `ProcessWeatherBossLevel` / `DrawWeatherBossLevelExtra` | 0x00214AC8 / 0x00214980 | 0% | level plumbing |
| `WeatherBossReset` / `WeatherBossNextAction` | 0x00214588 / 0x0021ED08 | 0% | reset + state machine |
| `InitWeatherBossTarget` / `ProcessWeatherBossTarget` / `DrawWeatherBossTarget` | 0x0021CFD8 / 0x0021D040 / 0x00202818 | 0% | targeting reticule |
| `ProcessWBBolts` / `ProcessLighteningHail` / `ProcessWBIntro` | 0x00214400 / 0x00213ED0 / 0x00214C68 | 0% | attacks/hazards |
| `GetWeatherBossPos` / `GetWeatherBossSpline` / `SetWeatherStartPos` | 0x0021C7B8 / 0x0021C7C8 / 0x0021C278 | 0% | pos/spline for the puppet |
| `InitZoffa` / `DrawZoffaUFOs` / `MoveZoffaUFO` / `ZoffaSmoke` / `DrawZoffa` | 0x00205268 / 0x00205440 / 0x00205BE8 / 0x002056D8 / 0x0021D300 | 0% | the UFO fleet the boss flies |

### 2c. Earth Boss (Rok-Ko / rock) — `game/vehicle`, all ⬚ 0%
`InitEarthBoss` 0x002171A8 · `ProcessEarthBossActions` 0x00217AB8 ·
`ProcessEarthBossVortex` 0x002195A8 · `DrawEarthBoss` 0x00217680 ·
`ProcessEarthBossLevel` 0x002196E8 · `DrawEarthBossLevelExtra` 0x0021F380 ·
`EarthBossReset` 0x0021F3B0 · `PrintEarthBossAction` 0x00217890 (the action enum).

### 2d. Crunch fights (Water / Space Crunch + Cortex) — `game/ai`, all ⬚ 0%
`WaterCrunchFunction_{Spladoosh,Attack,Punch1,Punch2,Defeated}`
(0x0022F928 / 0x00234BA0 / 0x00234BF0 / 0x00234CC8 / 0x00234DA0) ·
`SpaceCrunchFunction_{NewPoint,NewCount,AttackCortex,CheckCortexBack,PunchCortex}`
(0x00230AE0 / 0x00234EB0 / 0x00234EF0 / 0x00234F20 / 0x00234F28) ·
`SpaceCortexFunction_{CheckPunch,Defeated,TakeHit}` (0x00234F88 / 0x00234F90 / 0x00234FD0) ·
`UpdateCRUNCHTIME` / `DrawCRUNCHTIME` / `ResetCRUNCHTIME`
(0x0022FAC8 / 0x002308C0 / 0x00234DD8) — the shared boss-health bar
(`BossBar` 0x0021C428). `FindGongBongerAnim` 0x00230C20.

---

## 3. Glider levels — PvP (shoot your partner) + puppet fidelity

The glider transform is already mirrored onto the puppet (`vehicle_xf`,
[DrawGlider](../src/game/vehicle.c) ◐ 89%). To make bullets/bombs cross between
players (friendly fire or co-op assist), decompile the projectile system and
intercept the collision like the Fire Boss balloon hit.

| Function | Addr | State | Fuzzy | Coop use |
|---|---|---|---|---|
| `AddGliderBullet` | 0x00208A28 | ⬚ | 0% | Publish a spawn (like a rock throw) so the peer replays your shots. |
| `ProcessGliderBullets` / `DrawGliderBullets` | 0x00209990 / 0x00209750 | ⬚ | 0% | Bullet sim/render for the mirrored shots. |
| `CollideGliderBullets` / `GliderBulletsHitThings` | 0x00208BF0 / 0x002091B0 | ⬚ | 0% | **PvP intercept:** let a bullet hit the partner puppet + report the hit. |
| `GrabGliderBullet` / `FreeGliderBullet` | 0x0021D8B0 / 0x0021D958 | ⬚ | 0% | Bullet pool alloc/free (needed to replay spawns). |
| `AddGliderBomb` / `ProcessGliderBombs` / `DrawGliderBombs` | 0x0021CCD0 / 0x0020D898 / 0x0020D730 | ⬚ | 0% | Bomb equivalents. |
| `CollideGliderBombs` / `GliderBombsHitThings` | 0x0020DCC0 / 0x00209528 | ⬚ | 0% | PvP bomb intercept. |
| `GliderFire` / `PickGliderTarget` / `DrawGliderTarget` / `DrawTorpedoTarget` | 0x00201D88 / 0x0021CC00 / 0x00211D38 / 0x00211EB8 | ⬚ | 0% | Firing + lock-on (retarget onto the partner puppet). |
| `AddGliderHitPoints` / `GetGliderHealthPercentage` / `ExplodeGlider` / `DeadGliderCoco` | 0x0021C018 / 0x0021C2D8 / 0x0021D0F8 / 0x002041C8 | ⬚ | 0% | Health + death, for a shared/PvP damage model. |
| `ControlGlider` / `ProcessGliderMovement` / `MoveGlider` / `GliderCam` / `CheckGliderCollisions` | 0x00202DC0 / 0x00203860 / 0x00204588 / 0x0020A658 / 0x0020C7B8 | ⬚ | 0% | Movement/camera if you want authoritative glider sync (vs the current cosmetic puppet). |

---

## 4. Atlasphere (ball) levels — puppet + ball-vs-ball PvP

Ball transform already mirrored (`DrawAtlas` ✅ 100%, `ObjectToAtlas` ◐ 92%, rotquat
in `vehicle_xf`). For **shared/PvP ball play**, decompile the ball sim + the
ball-vs-ball collision. All ⬚ 0%:

`ControlAtlas` 0x002152F8 · `ProcessAtlas` 0x00216BA8 · `MoveAtlas` 0x00216F48 ·
`ProcessMovementAtlas` 0x002154E0 · `AdjustAtlasRotations` 0x00215750 ·
`ProcessAtlasAtlasCollisions` **0x0021F9A8** (and `_a` 0x00219BD0) — *ball-vs-ball,
the PvP bump* · `ProcessAtlasTrail` 0x0021A498 · `ResetAtlas` 0x00216848 ·
`CheckAtlasGround` 0x00216598 · `TerrainAtlas` 0x00215DF0.

---

## 5. Shooter / space levels (planes, gunboats, battleships, asteroids)

For puppets + shared enemies in the on-rails shooter levels. All ⬚ 0%:
`MovePlane` 0x00209C38 · `ProcessGunBoat(s)` 0x0020E1D8 / 0x0021E3F0 ·
`ProcessBigGun(s)` 0x0020F810 / 0x0021E6C8 ·
`ProcessBattleShip(s)` 0x00210290 / 0x0021E8C0 + `CollideWithBattleShip(s)`
0x0020FE00 / 0x0021C718 · `ProcessSatellite(s)` 0x00207CA8 / 0x0021D610 ·
`ProcessSpaceStation(s)` 0x002084A0 / 0x0021D7D8 ·
`ProcessAsteroid(s)` 0x0020B2A0 / 0x0021DE80 · `ProcessHovaBlimp(s)` 0x00207488 /
0x00207858.

---

## 6. Player↔player interaction & shared hazards (custom coop)

The functions that make one player affect the other: friendly fire, shared
damage, spin/slam hitting the partner, and the client→host enemy-hit intercept.

| Function | Addr | Unit | State | Fuzzy | Coop use |
|---|---|---|---|---|---|
| `KillPlayer` | 0x00200E90 | game/game_obj | ✅ | 100% | Already called by the mod (`KillPlayer(obj, how)`); matched + typed. Lets a player kill/hurt the partner (friendly fire) or gate deaths. |
| `PlayerCreatureCollisions` | 0x001F9BC0 | game/game_obj | ◐ | 76.7% | Enemy hit/kill site (`po->dead=1/4; KillGameObject`) — the client→host damage intercept for shared enemies. |
| `HitCreatures` | 0x001FD158 | game/game_obj | ⬚ | 0% | Host applies a client-reported enemy hit. |
| `KillGameObject` | 0x001FD4A8 | game/game_obj | ⬚ | 0% | The object-death primitive behind creature/boss kills. |
| `PlayerObjectAnimCollision` / `ObjectCylinderCollision` | 0x001F0558 / 0x001F9558 | game/game_obj | ⬚ | 0% | Player-vs-object collisions — a spin/slam hitting the partner puppet. |
| `CollidePlayerPoint` | 0x00213D10 | game/vehicle | ⬚ | 0% | Point-vs-player test used by vehicle hazards. |
| `HitItems` / `CrateCollisions` / `WumpaCollisions` | 0x00201140 / 0x001FAB20 / 0x001FBEF0 | game/game_obj | ✅/⬚/⬚ | 100% / 0% / 0% | Pickup/crate hit paths (`HitItems` matched). Stage 3 handled sync by identity; decompile the rest for physical/PvP crate play. |

---

## 7. Vehicle-level dispatch — the hook map for all vehicle coop

`ProcessVehicleLevel` **0x00214DE8** (game/vehicle, ⬚ 0%) is the master dispatch that
routes each level id to its vehicle/boss `Process*`. Typing it produces the exact
**level → sub-process** table — i.e. which handler to replace for every vehicle and
boss coop feature above (the Fire Boss path `→ ProcessFireBossLevel` is one row).

---

## 8. Per-vehicle / mount movement — puppet fidelity when the partner rides one

`game/move` holds the movement brain for every mount. The puppet currently borrows
`DrawCreatures` for rendering, but faithful motion/state for a *riding* partner (and
any authoritative vehicle sync) needs these. All ⬚ 0%:

`MoveCRASH` 0x00250A98 · `MoveCOCO` 0x00252478 · `MoveSWIMMING` 0x00252248 ·
`MoveSCOOTER` 0x00252FB0 · `MoveSNOWBOARD` 0x00253278 · `MoveOFFROADER` 0x00253720 ·
`MoveMECH` 0x00253CD8 · `MoveFIREENGINE` 0x002544D0 · `MoveGYRO` 0x002548A8 ·
`MoveSUBMARINE` 0x002554E0 · `MoveMINECART` 0x002556C8 · `MoveTub` 0x00255EC8 ·
`MoveMINETUB` 0x00256A18.

---

## 9. Chase levels (boulder / ball-of-fire pursuit)

Shared "run-toward-camera" chase sequences. `game/chase`, all ⬚ 0%:
`InitChase` 0x002592F8 · `UpdateChase` 0x00259798 · `DrawChases` 0x0025A588 ·
`UpdateChases` 0x0025AB98 · `ChaseActive` 0x0025AB58 ·
`NearestChaserDistance` 0x0025AC98 · `UpdateCrateBallsOfFireDoors` 0x00258DB0.
Mirror the chaser position so both players see the same boulder gaining on the host.

---

## 10. Cameras & cutscenes (custom coop presentation)

Lower priority, but decompiling these enables shared-camera / split behaviour and
synchronised cutscene skip. All ⬚ 0%:
- Cameras: `GameCam`, `JeepCam` (0x00226C98), `GliderCam` (0x0020A658), the camera
  tween helpers — for a custom coop camera (shared, or picture-in-picture).
- Cutscenes: the `NuGCutScene*` system (0x001AAE88…) + per-level cutscene triggers
  — to keep both players' cutscenes in lock-step or skip together.

---

## Suggested priority (value ÷ effort)

1. **Generic shared enemies** — `MoveCreature` + `ManageCreatures`/`AddCreature` +
   the `PlayerCreatureCollisions` hit intercept. Highest payoff (every non-boss
   level gains a visible partner-vs-enemy fight); `MoveCreature` (0%) is the gate,
   but the surrounding plumbing is already 77–100% (`ProcessCreatures` 96.8%,
   `RemoveCreature` ✅, `PlayerCreatureCollisions` 76.7%).
2. **Glider PvP** — `AddGliderBullet` + `CollideGliderBullets`/`GliderBulletsHitThings`.
   Self-contained, high "fun" value ("shoot your partner"), reuses the Fire-Boss
   spawn/hit-counter pattern directly. All 0% today.
3. **The remaining bosses** (Weather, Earth, Crunch) — repeat the Fire-Boss ladder
   per boss; each is a bounded, well-scoped proof. All 0% today.
4. **`ProcessVehicleLevel`** — cheap and unlocks the exact hook map for 1–3.
5. **Atlasphere ball-vs-ball** (`ProcessAtlasAtlasCollisions`) — a natural PvP mode
   (`DrawAtlas` ✅ / `ObjectToAtlas` 92% already give the puppet).
6. **Per-vehicle `Move*`** — only when authoritative vehicle sync (beyond the
   cosmetic puppet) is wanted.
7. **Chases / cameras / cutscenes** — polish.

See [COOP_ROADMAP.md](../mods/coop/COOP_ROADMAP.md) for the executed ladder and
[decomp_agent.md](decomp_agent.md) for the matching workflow (MCP sessions →
`compile_diff` → `promote_matching`).
