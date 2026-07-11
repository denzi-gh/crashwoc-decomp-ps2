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
    smooth descent. **Glow + zone-entry timing confirmed in-game; v7 position/
    descent needs confirm.**
- [ ] *(PR2, separate)* **Warp-IN / return-to-hub**: when the remote *returns*
  to the hub (or a boss stage ends), the puppet should appear/leave with the warp
  effect rather than pop in/out. Tip (user): all boss stages use a similar
  teleport-out (Crash warps out with the effect, no teleporter object visible) —
  find that path and drive the same `AddWarpDebris` on level-entry/exit edges.

## Stage 3 — Shared collectibles & progression  *(med–high decomp)*

- [ ] 3a: sync `plr_*.count`, `plr_items`, `Game.level[].flags` / `Game` counters
- [ ] 3a: pickup-event channel in the mailbox (deltas, de-dup by seq)
- [ ] *(supports 3a)* decomp `game_obj.c` pickups (`HitItems`, `Pickup*`, collisions)
- [ ] 3b: per-crate destroyed-state sync + checkpoint reconciliation
- [ ] *(supports 3b)* decomp `crate.c` (`BreakCrate`, crate save/restore)

## Stage 4 — Shared enemies / shared boss  *(high decomp — capstone)*

- [ ] Client `ProcessCreatures` hook suppresses local enemy AI
- [ ] Enemy/boss state array in the mailbox (+ `ai.hits`/`count`/`die_time`)
- [ ] Client→host hit events (around `PlayerCreatureCollisions`)
- [ ] First proof: one shared boss (Fire Boss, `jeep.c`) authoritative on host
- [ ] *(supports Stage 4)* decomp `ProcessFireBoss`/`DrawFireBoss` (`jeep.c`)
- [ ] *(stretch)* full enemy sharing across `Character[1..8]`
