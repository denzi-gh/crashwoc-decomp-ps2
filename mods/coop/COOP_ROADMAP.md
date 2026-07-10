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
- [ ] Live two-PC session verified *(needs the user + a peer)*

## Stage 1 — Finish the puppet: vehicles + Aku Aku mask  *(low decomp → mailbox v3)*

- [ ] Mailbox **v3**: add `vehicle`, `mask_active`, `vtog` (+ bump both versions)
- [ ] Publish/consume/ghost the new fields
- [ ] Puppet renders in ground/rail vehicles (`obj.vehicle` → `model[1]`)
- [ ] Puppet renders in swim mode (body model swap)
- [ ] Puppet renders in glider/plane/atlas/jeep (bracket own `vehicle.c` draw)
- [ ] Aku Aku mask on puppet via `DrawMask`/`DrawMaskFeathers` bracket hook
- [ ] *(optional)* decomp `DrawGlider`/`DrawAtlas`/`DrawPlayerJeep` if bracket misbehaves
- [ ] *(optional)* decomp `DrawMask`/`UpdateMask`/`MakeMaskMatrix` for a parameterized mask

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
