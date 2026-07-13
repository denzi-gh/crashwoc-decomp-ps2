# Crash WoC Coop — Feature Roadmap

Shared checklist tracking the path from "one visible puppet" to **true coop**.
This is the **mod-side** copy (repo `crashwoc-decomp-ps2`); a mirror lives at
`crashwoc-multiplayer/COOP_ROADMAP.md`. Keep both in sync.

**Rule:** after implementing or changing a coop feature, tick its box here and
update the one-line status, in the same change.

Legend: `[x]` done · `[~]` in progress · `[ ]` not started.
Full design/decomp notes: plan `zesty-exploring-lecun.md`.

---

## Foundation (done)

- [x] Modding SDK: fixed-layout injection, DoInput pre-hook + mailbox @0x706A40 — boot-verified
- [x] Mailbox contract `mods/include/coop_mailbox.h` — magic "CWCO" @0x706A60
- [x] Publish local player state → local slot (seq-lock)
- [x] Consume remote slot → remote snapshot (seq-lock, staleness)
- [x] Puppet render via REPLACE hook on `DrawCreatures` (visual-only creature_s)
- [x] Level-independence show rule (puppet only when both report same Level)
- [x] Same-PC bridge over PINE (`crashwoc-multiplayer`, ports 28011/28012)
- [x] Spin + bazooka + aim sync — mailbox **v2** (slot 0x60, remote @0x706AD0)
- [x] Launch script `start_coop_pcsx2.py` + `unlock_levels.py` (replays OpenGame)
- [x] Netcode latency step 1 — publish post-simulation from the draw hook

## Stage 2 — Internet play over UDP  *(PC-only, zero decomp — IN PROGRESS)*

Lives in the `crashwoc-multiplayer` repo (`coop/relay.py`, `coop-relay` CLI).
The mailbox contract is unchanged, so the current mod/ELF stays valid.

- [x] `coop/relay.py` — `CoopRelay` (one PINE instance + one UDP peer socket)
- [x] Net packet framing (magic + version + session + send-seq + 0x60 slot)
- [x] `coop-relay` CLI (`--pine-endpoint --peer --listen-port --rate --stats`)
- [x] `tests/coop/test_coop_relay.py` (framing, version gate, latest-wins, torn, UDP loopback)
- [x] README two-PC setup (UDP port, firewall/port-forward, same COOP_VERSION)
- [x] Live two-PC session verified — played over the internet with a peer (2026-07-11)

## Stage 1 — Presence polish: names, paused, vehicles, mask  *(mailbox v3)*

One version bump (v3, slot 0x60→0x70, remote slot →0x706AE0) carries every new
field so we grow the contract once. Text uses `Text3D` @0x238280 (2D screen)
positioned by `NuCameraTransformScreen` @0x114048 (world→screen, NULL matrix =
global screen matrix `D_0067A8C0`); pause detected via `Paused` @0x630af4.

**1a — Name tags + Paused presence (the immediate work):**
- [x] Mailbox **v3**: add `name[16]`, `paused`, `mask_active`, `vehicle`, `vehiclecontrol` (bump both versions)
- [x] Publish `paused`/`vehicle`/`mask_active`; consume + `name`; ghost
- [x] Pause heartbeat: keep publishing while `Paused != 0` so peer stays fresh
- [x] Paused puppet renders **gray + frozen** (flat gray lights, anim clock held) — confirmed in-game (puppet darkens and holds)
- [x] Name tag above the remote puppet (project + `Text3D`); own name hidden — renders in-game (must be UPPERCASE: lowercase a–x are icon glyphs)
- [x] "Paused" label above a paused remote puppet (name hides while paused so they don't overlap)
- [x] PC side supplies names (`--name` / `--p1-name`/`--p2-name`), injected into the peer slot
- [x] Names are **optional** — bridge/relay run fine with no name args; only the floating tag is lost, "PAUSED" and all puppet sync still work
- [x] Version banner "CRASH: TWOC: MULTIPLAYER Vn" on the **in-game pause menu** (`Paused != 0`) only — not on the title/front-end, not on the live HUD; lets both players eyeball-match layout versions by pausing in a level
- [~] Label projection: view-space depth (`NuCameraTransformView` @0x113D18) drives distance-scale + behind-camera cull; GS guard-band centre 32768 for screen→ndc — placement/scale calibrating in-game
- [ ] Final placement/scale sign-off (COOP_LABEL_UP, COOP_DIST_REF, front-cull sign) — needs the pad

**1b — Vehicles + Aku Aku mask rendering:** wired 2026-07-11 (code done, needs
in-game confirm). Puppet is fed `obj.vehicle = remote.vehicle` in `puppet_update`,
so the *same* `DrawCreatures` call renders every mode; the mask is drawn from the
`coop_draw_creatures` hook via a puppet-owned `mask_s`. Ground/rail/swim/jeep need
no decomp (by-address calls read only their creature arg). Glider + atlas *did*:
their transform lives in `NEWBUGGY` (`creature+0x224`), now typed by the
`game/vehicle` decomp and driven on a puppet-owned copy from **mailbox v4**
`vehicle_xf[7]`.
- [x] Puppet renders in ground/rail vehicles (`obj.vehicle` → `model[1]`) — code done
- [x] Puppet renders in swim mode (global `VEHICLECONTROL == 2` body-swap) — code done
- [x] Jeep — **confirmed in-game** (`DrawJeep` builds its matrix from pos/hdg)
- [x] Glider + Atlas — puppet owns a `NEWBUGGY` (typed from the decomp: unit `game/vehicle` `DrawGlider`/`DrawAtlas`/`ObjectToAtlas`, merged from `main`) driven from a synced **mailbox v4** `vehicle_xf[7]`: glider = pitch/roll/yaw + `Buggy.pos` + `enable`, atlas = `ball_pos` + `rotquat`. Replaces the borrow-the-local-Buggy hack; glider banks + ball spins like the *remote's*. Glider **confirmed in-game** (incl. weather boss). **Tornado Valley (level 0xD) fix:** `enable` is now *synced*, not forced to 1 — forcing 1 pinned the puppet into `DrawGlider`'s fixed-`D_006B75A0` branch instead of the positioned `Buggy.pos` branch, so player 2 was invisible.
- [x] **Per-remote vehicle mode** — `coop_draw_creatures` brackets the global `VEHICLECONTROL` (and forces `vtog_time == vtog_duration`) to the *remote's* `vehiclecontrol` for the puppet pass. `DrawCreatures` picks the puppet's vehicle from the global (= *local*) mount state, so before this the mech (`0x44`) rendered from our state: a peer in the mech was invisible when we were on foot, and a peer on foot wrongly showed a mech when we were mounted. Now each puppet renders its own mount state. Fixes all toggle-mount `model[1]` vehicles (mech, scooter, snowboard, gyro, sub, minecart, off-roader, fire-engine).
- [x] Aku Aku mask on puppet: init via `NewMask(&mask, &puppet.pos)`, `UpdateMask` + `DrawMask` each frame, shadow bracketed (`COOP_PUPPET_MASK` toggle).
- [x] *(was optional)* puppet-owned `NEWBUGGY` — done: typed by the `game/vehicle` decomp (merged from `main`), mirrored into `mods/include/creature.h`; borrow hack removed
- [ ] *(follow-up)* sync vehicle body `anim`/`vehicle_frame` so the mounted body animation (not just the shell) tracks the remote
- [ ] *(follow-up)* puppet renders the glider-level **Zoffa UFO teleport-in effect + debris** on the remote's side (visual completeness in glider/farm/space-arena levels). Enabler **landed 2026-07-11**: `game/vehicle` `TeleportManager` (`0x00205978`) decompiled + **matching** — respawns each active teleporting Zoffa at one of four camera-relative `TeleportPos` points (cursor `D_006332A4`, advanced on success), clamps above `Level_GliderFloor`, re-aims 90/135° off `PlayerGlider.AngleY`, and seeds `Velocity` by rotating `TeleportVel` through the camera matrix. Ties into Stage 4 (shared enemies); the teleport/`ZoffaSmoke` debris is per-instance sim today, so this is a render-the-remote's-Zoffa task.

**1c — Hub teleporter warp effect on the puppet** (`mailbox v5`): the puppet
should warp in/out of the hub with the real teleport effect instead of just
blinking out when the level swap hides it. Uses `AddWarpDebris(obj)`
(`0x00260CD8`, `game/game_deb`, already decompiled): a fire-and-forget finite
debris effect at the object's mid-body that self-animates in the debris pass.
- [x] **Warp-OUT** (hub → level): mailbox **v5** adds `int warp_level` (`0x84`,
  reuses the old `reserved`; bumps both versions to 5). `warp_level` (`0x630B9C`)
  goes != -1 the instant the remote steps on a hub teleporter, ~1 s *before* its
  level loads while it is still standing in the hub. The mod fires
  `AddWarpDebris(&puppet.obj)` once (latched, gated on `Level == 0x25`) so the
  local player sees the remote dissolve into the warp effect at the right moment,
  then the effect plays out after the level swap hides the puppet. **Code done,
  needs in-game confirm.**
- [x] **Teleporter ring on warp-OUT** — the actual spinning teleporter *prop*,
  not just the debris. **Phase-0 finding:** the teleporter is *not* a placed
  object — `JonProbe` (`0x1DB150`, `game/game`, asm) draws it every frame from
  the singleton probe globals (`probeon`/`probepos`/`proberot`/`probecol`/…, gated
  by `Hub != -1 && in_finish_range > 0`) via `Draw3DCharacter` (`0x1EE410`) of
  character **`0xB1`**'s model (`CModel[CRemap[0xB1]]`). The placed type-`0xB1`
  creature is only the node marker (`CheckFinish` locates it with
  `FindNearestCreature(..,0xB1,..)` → `in_finish_pos`; `creature.c:3413` gives it a
  `DrawProbeFX` glow). The ring model alone (`Draw3DCharacter`) is only the
  physical prop; the *glowing beam* is a separate procedural effect inside
  `JonProbe` (two `NuLgtArcLaser` filaments + rising debris + a `probecol`
  intensity ramp), so a bare model draw shows no glow. **Approach (`COOP_TP_BEAM`):**
  reuse the real `JonProbe` with an **isolated probe context** — save the local
  player's probe globals (`Hub`, `in_finish_range`, `in_finish_pos`, `probeon`,
  `probepos`/`probedpos`/`probepos2`/`probespk`, `probey`/`probecol`/`probetime`,
  `proberot`), load a puppet-owned set (pre-latched `probeon = 1` so the trigger's
  rumble/SFX + the +4.5 descend are skipped; positions pinned to the node), call
  `JonProbe` (it advances + draws ring **and** beam), save the puppet set back,
  restore the local set. The local player's own teleporter/warp is never
  disturbed. Runs once/frame (guarded) in the `coop_draw_creatures` puppet pass;
  armed in the sim hook (`coop_warp_effect`). Bare-ring fallback (`COOP_TP_BEAM 0`)
  kept in case the reuse misbehaves on hardware.
  - **Timing = mailbox v6.** The teleporter must appear the moment the remote
    *runs into* a hub node's teleport zone (like real Crash), not at the dissolve.
    `HubLevelSelect` (`0x1DD108`) ramps `in_finish_range` 1..0x32 the instant Crash
    enters the zone — `JonProbe`'s own gate — and only sets `warp_level` once it
    hits 0x32 (commit). So v6 adds `int in_finish_range` at `0x88` (reuses the old
    `reserved2`; slot stays `0x90`, addresses unchanged). The puppet's teleporter
    is armed on `remote.in_finish_range > 0` (zone entry); the dissolve
    (`AddWarpDebris` + the `warp_level` body-vanish bracket) still fires at commit.
  - **Position = mailbox v7.** The teleporter belongs at the NODE, not the puppet's
    entry point (latching the puppet's body pos misplaced the ring — the puppet
    crosses the zone edge, the pad is at the spline node). `HubLevelSelect` sets
    `in_finish_pos` to the exact node every frame in the zone, so v7 adds
    `float in_finish_pos[3]` at `0x8C` (slot grows `0x90`→`0x9C`, remote slot
    `0x706B00`→`0x706B0C`, mailbox `0x130`→`0x148`). The puppet's teleporter is
    placed at the synced node. Also fixed the entrance: pre-latching skipped the
    trigger's `probepos.y += 4.5`, so seed it at node+4.5 (`COOP_TP_UP`) — the ring
    now starts at the top and settles down instead of jerking up first.
    Iters: 1 bare ring (no glow, bad descend); 2 JonProbe reuse (glow) but triggered
    on `warp_level` (too late); 3 (v6) timing on zone entry; 4 (v7) node position +
    smooth descent. **CONFIRMED in-game.** (Decorative follow-up: the ring "head"
    glides toward the local player, not the puppet — deferred.)

**Feature 2 — in-level / to-hub warp-OUT (CUSTOM, code done, needs confirm).**
KEY: a level exit already has its OWN persistent placed teleporter pad (a type-`0xB1`
creature, always drawn by the creature loop) — unlike the hub, where the teleporter
is *summoned* by `JonProbe` from spline nodes with no placed pad. So we must NOT draw
our own teleporter at a level exit (that made a redundant second one). The teleporter
(`coop_probe_run`) is therefore **hub-only**. At a level exit we ONLY fire
`AddWarpDebris` **and hide the puppet's body** the moment the remote reaches the exit
zone (`in_finish_range > 0`), so it dissolves + despawns *into the existing pad* with
the effect instead of lingering visible until the level swap. The hub body-vanish
still uses retail's `warp_level` draw bracket (no extra hide). **No contract change**
(reuses v7 `in_finish_range`).

**Feature 3 — warp-IN / arrival (CUSTOM, code done, needs confirm).** No retail
reference — our design. On the puppet's rising show edge (the remote just entered
the local player's room, `!g_puppet_active`), `coop_warp_in_arm` plays a teleporter
at the arrival point (`g_puppet.obj.pos`) via the same `coop_probe_run` primitive,
and the body (+ mask) is held hidden for `COOP_TPIN_REVEAL` (40) frames — long enough
for the teleporter's beam/lighting to build up first — then revealed, so it
materialises out of the teleporter instead of popping in; teleporter runs
`COOP_TPIN_FRAMES` (105). Skipped if the remote arrives already in a warp-out zone.
The hub warp-OUT and warp-IN teleporters share one draw (mutually exclusive: leaving
vs arriving). Body-hide (`coop_body_hidden`) is shared by F2 (dissolve) and F3
(materialise). Gated `#define COOP_TPIN`. **No contract change.** Tunables:
`COOP_TPIN_FRAMES`/`COOP_TPIN_REVEAL`.
- Arrival also fires `AddWarpDebris` (materialise sparkle), matching the warp-out.
- **Departure debris (any non-teleport leave):** on the puppet's falling show edge,
  if the remote left to another room (`remote.level != Level`, present, not dead)
  and no warp-out already sparkled it (`!g_warp_latched`), fire a farewell
  `AddWarpDebris` at its last position. Covers **quit-to-hub from the pause menu**
  (and any forced/cutscene transition), where the remote never touches a teleport
  zone so F1/F2 wouldn't fire — it used to just blink out.
- [ ] *(follow-up)* boss-stage teleport-out variant (effect-only, no teleporter
  object — retail `finish_type` case); optional `finish_type` sync if needed.
- [ ] *(follow-up, decorative)* teleporter "head" tracks the puppet, not the
  local player (the `probepos → probedpos` glide currently chases the local pad).

## Stage 3 — Shared collectibles & progression  *(med–high decomp)*

Design locked 2026-07-12 (plan `read-the-coop-notes-harmonic-umbrella.md`):
**absolute state + OR-merge, no event channel** (the transport has no reliable
delivery; deltas would need acks/dedup, absolute bitmaps are idempotent for
free), and **symmetric execution** — both consoles run the real
`PickupItem`/`BreakCrate` with full rewards, behind the `g_coop_applying`
seam for a later asymmetric mode. Counters are never synced: `plr_crates` is
re-derived every frame by `UpdatePlayerStats` and crate wumpa lands via
deferred screen-wumpa entities, so state converges by replaying the same
functions, not by copying tallies.

- [x] *(supports 3a)* decomp `game_obj.c` pickups — **matching** 2026-07-12:
  `KillItem`, `PickupCrystal`, `PickupCrateGem`, `PickupBonusGem`,
  `PickupPower`, `PickupRelic`, `HitItems`, `PickupItem` (PR-D1/D2)
- [x] *(supports 3b)* decomp `crate.c` — **matching** 2026-07-12: `BreakCrate`,
  `SaveCrateTypeData`, `RestoreCrateTypeData` (+ `GetCrateType` faithful
  near-match); `crate_s`/`crategroup_s` typed (PR-D3). Found+fixed a
  decompals-as align-after-mtc1 hazard-nop bug along the way.
- [x] 3a: **mailbox v8** (slot `0x9C`→`0x118`, remote slot →`0x706B88`,
  mailbox `0x148`→`0x240`, `[mailbox] size = 0x400`): `bonus`, `items`
  (`plr_items`), `level_flags[35]`, `powers` (`Game.powerbits`),
  `hub_flags[6]`, `crate_bits[8]` (reserved for 3b), `item_bits[2]`.
  Bridge codec + offset/roundtrip tests updated in lock-step (v8).
- [x] 3a: committed progression sync — `level_flags`/`hub_flags`/`powers`
  OR-merged **always** (cross-level), then `CalculateGamePercentage(&Game)`
  on any new bit re-derives percent/crystals-per-hub/gems/relics, so a
  crystal the remote earns shows in the local hub UI + save immediately.
  Skipped while `TimeTrial`; merge gated on `local_valid` (never into an
  unloaded profile). **CONFIRMED in-game 2026-07-12.**
- [x] 3a: live item sync — **identity-based** (reworked 2026-07-12): each
  reward bit maps to exactly one object character (PickupItem's dispatch:
  crystal 0x75↔1, crate gem 0x77↔2, coloured gems 0x78–0x7D↔4…0x80, powers
  0xA2–0xA7↔powerbits; 0x76 = time-trial clock, never replayed). Any LIVE
  placed item whose bit is in `plr_items | remote.items` (or on the powers
  rising edge) gets the real `PickupItem` — grant + despawn + effects —
  ≤2/tick, self-healing; bits with no live object OR directly (HUD first).
  `item_bits` is reserved again (always 0), the `PickupItem` capture hook
  is gone. The original slot-index channel replayed the WRONG object:
  `pObj[]` fills as objects stream in, so slot order diverges between
  instances (in-game bug: a crystal pickup granted the peer the crate gem
  and left the crystal standing). **CONFIRMED in-game 2026-07-12.**
- [x] 3a: **hub award celebration on the puppet** — a newly merged level-flag
  bit means the remote is mid-celebration (retail only commits a bit when the
  hub award flight lands), so when both share the hub the mod replays the
  real show: `AddAward` (0x1DBDC0) spawns the crystal/gem award, the slot is
  rewritten to the flying stage with the **puppet** as source (its tumble
  anim is already action-synced), pop chime + `AddGameDebris` 0xA1 sparkle,
  then retail's `UpdateAwards` flies it to the level pad and **commits on
  landing** (celebrated bits are deferred via `g_award_pending`, so the pad
  marker/tallies appear when the crystal lands, not seconds early; lost
  flights self-heal to a direct merge). Fresh-finish gate = the remote's
  previous room must be the finished level (a link-freshness warmup does NOT
  work: the level→hub load always trips the staleness limit and reset it —
  v1 of this feature silently never fired). Also gated: hub only, puppet
  visible, level in the local `HData[Hub]`, local `new_lev_flags` overlap
  skipped, ≤3 awards/tick. v2 confirmed working in-game but ~2 s late (the
  committed bit only exists after the remote's flight LANDS) and spawned
  too high (puppet already in the post-tumble jump by then). **v3 = mailbox
  v9**: publish `pending_flags` + `pending_level` (`last_level`); a group
  FALLING out of the remote's pending_flags is the pop cue
  (`coop_award_pop_edge`; slot `0x118`→`0x11C`, remote →`0x706B8C`, mailbox
  `0x248`; merge-time path kept as late fallback for missed edges). v3
  shipped `pending_flags = new_lev_flags` and behaved EXACTLY like v2 in
  game — wrong premise: the tumble does NOT clear `new_lev_flags` at the
  pop (only the AddAward-failure path does, creature.c:1716); the pop frame
  ORs the group into **`temp_lev_flags`** (0x006310BE, zeroed by
  `HubStart`), and `new_lev_flags` is XOR-cleared by `UpdateAwards` only at
  pad ARRIVAL. **v4 fix (same v9 layout): publish `pending_flags =
  new_lev_flags & ~temp_lev_flags`** — that value falls at the exact pop
  frame. **Confirmed in-game 2026-07-12** (cosmetic tweaks may follow).
- [x] 3b: per-crate destroyed-state sync + shared checkpoints + shared death
  reset — capture: replace-hook on `CrateOff` (0x1F3178, the one point every
  crate dies through; stack/TNT/nitro-switch chains bypass `BreakCrate`)
  records the flat `Crate[]` slot into `crate_bits` on a nonzero return,
  bonus-round crates excluded, applied echoes captured on purpose (bitmap
  convergence). Apply: in `coop_merge`'s same-level tier, newly-published
  remote bits (`& ~applied & ~own`) replay through the real
  `BreakCrate(group, &Crate[i], GetCrateType(...), 0)` under
  `g_coop_applying`, ≤8/tick (chains amplify); already-destroyed slots
  (retail predicate: `on == 0 || (newtype == 0xF && metal_count)`) mark
  applied silently; TNT/nitro mid-countdown (`armed != -1`) retried later.
  **Checkpoints are SHARED** (user decision 2026-07-12, replaced the earlier
  own-checkpoints plan): the replayed `CrateOff` runs its own
  `ResetCheckpoint`, whose args all derive from the crate — both players get
  the identical respawn point and the same `CrateTypeData` log baseline; no
  hook needed. **Death reset is SHARED** (user decision same day): either
  player's death resets BOTH players' post-checkpoint crates. Own respawn:
  the `GotoCheckpoint` hook drops now-intact crates from the published
  bitmap IMMEDIATELY (retail resets crates before GotoCheckpoint) and
  pauses the apply loop ~1 s; the peer detects the falling edge in the
  published set (absolute-state cue — robust to stale gaps, unlike the dead
  flag), mirrors retail's death-block crate pair
  `RestoreCrateTypeData(); ResetCrates();` (log self-clears → simultaneous
  deaths are a no-op), drops its own now-intact bits and pauses its apply
  too. Ordering is load-bearing: v1 dropped the bits only at settle expiry,
  so the dead player's own apply loop re-broke every resurrected crate from
  the peer's still-stale echo the moment the settle ended and no reset ever
  propagated (the in-game failure of 2026-07-12). Applied-bits re-arm on
  the falling edge so a genuine re-break after a reset replays; a
  drop-intact sweep runs every tick so ANY resurrection path (menu restart)
  stops being claimed within a tick.
  No contract change (`crate_bits[8]` reserved since v8, still v9);
  `coop-peek` popcounts cover the slot-order divergence check. Playtest
  watch-item: a crate un-breaking under the living player's feet (retail
  never resets crates while a player stands in the level).
  **Confirmed in-game 2026-07-12** (shared checkpoints, death reset and
  crate sync behave as intended; 5 hooks after the item-sync rework).
- [ ] *(follow-up)* asymmetric reward mode (same level: only the breaker gets
  rewards). Hard part: crate rewards are deferred screen-wumpa entities that
  escape any counter bracket — needs an `AddScreenWumpa` suppress hook or
  catching the spawned wumpa entities.
- [ ] *(follow-up)* `PickupPower` explainer popup also opens on the applier
  under symmetric replay — revisit if playtests object.
- [ ] *(stretch)* shared lives pool; `Wumpa[]` world-fruit entity sync

## VS Mode — competitive pickup tracking + coloured crystals  *(interlude before Stage 4)*

Bridge `coop-bridge --vs [--vs-stats PATH]`: gameplay stays fully shared
(one crystal, the race is who touches it first); the bridge sets
`COOP_CTL_VS` on both instances + `COOP_CTL_P2` on the second endpoint
(ctl bits only — mailbox layout stays v9), attributes every first claim
(crystal / crate gem / coloured gems in-level via `items` rising edges,
relics via `level_flags` tier bits) and writes a JSON stats file live
(who got what, where, when + totals; summary printed on exit).
Attribution is bridge-reliable: the origin side's bit is always observed
before its echo can reach the peer (the bridge IS the transport).

- [x] contract: `COOP_CTL_VS`/`COOP_CTL_P2` (coop_mailbox.h + mailbox.py, no
  version bump), diag bits 1/2 = tint active / tint unavailable — 2026-07-12
- [x] bridge: `--vs`/`--vs-stats`, per-cycle ctl writes, `VsRecorder`
  (`coop/vs.py`, first-wins claims, echo-safe, baseline priming against
  pre-existing saves) + 15 new tests, 82 green + ruff — 2026-07-12
- [x] mod: PickupItem attribution hook (6 hooks now; first-hand vs
  `g_coop_applying` echo), `g_vs_mine`/`g_vs_peer` per level, owner query —
  2026-07-12
- [x] coloured crystals (glow + carving + facet reflections; CONFIRMED
  in-game 2026-07-13): P1 blue / P2 red. The crystal is TWO renders. The
  GLOW (ObjTab[0x84] gobj, also drawn as the pause-panel carving via a
  byte-identical chain — Draw3DObject 0x1F0B40 / DrawPanel3DCharacter
  0x23A130) is tinted by parsing its prebuilt DMA/VIF packets and
  rewriting the UNPACK V4-8 vertex-colour payloads (originals kept for
  restore; own colour pre-claim, winner colour after a claim). The BODY
  (creature model CModel[CRemap[0x75]], skinned, 26 joints) gets a light
  tint through c->lights in the DrawCreatures hook — this colours its lit
  alpha REFLECTION overlay only: the main surface is texture-only (DECAL;
  in-game probes eliminated lights, vertex colours and nearby material
  constants), so the body keeps its pink with coloured facets. Dead end
  disproven in-game: crossfade colour-ref descriptors are never built —
  retail ships pre-converted streams that skip the builder, making the
  `nugscn_generate_colourref` global dead tooling. Live tuning: poke
  'VTNT' + 3 floats at payload+0x248 to override the body light colour.
- [ ] *(follow-up)* fully coloured crystal BODY: patch the crystal
  texture's palette (CLUT) in EE RAM — needs the texture-upload path
  (NuTex) mapped first; GS-level analysis (PCSX2 GS dump) is the tool.
- [ ] *(follow-up)* relay (`coop-relay`) VS support (ctl write + recorder on
  one side or merged post-hoc)
- [ ] *(follow-up)* crate-gem / coloured-gem carving tints (same mechanism,
  ObjTab 0x88..0x8E) + relic display
- [ ] *(follow-up)* per-level scoreboard overlay in-game (Text3D)

## Stage 4 — Shared enemies / shared boss  *(high decomp — capstone)*

- [ ] Client `ProcessCreatures` hook suppresses local enemy AI
- [ ] Enemy/boss state array in the mailbox (+ `ai.hits`/`count`/`die_time`)
- [ ] Client→host hit events (around `PlayerCreatureCollisions`)
- [ ] First proof: one shared boss (Fire Boss, `jeep.c`) authoritative on host
- [ ] *(supports Stage 4)* decomp `ProcessFireBoss`/`DrawFireBoss` (`jeep.c`)
- [ ] *(stretch)* full enemy sharing across `Character[1..8]`
