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
