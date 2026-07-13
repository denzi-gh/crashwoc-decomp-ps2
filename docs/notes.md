# Working notes

Durable quirks and invariants. 
things that shouldn't be re-derived. Resolved history lives in git.

## coop Stage 3 Phase 1 — mailbox v8 + 3a item/flag/power sync (2026-07-12)

Mod-side (branch modding-sdk) + bridge (crashwoc-multiplayer, `main`) landed
together; **two-instance CONFIRMED in-game 2026-07-12**.

**Hub award celebration (follow-up, same day, no contract change):** retail's
end-of-level show is the hub tumble award — `AddAward` (0x1DBDC0) spawns
`Award[i_award]` (stride 0x30: progress f32 +0, heading +4, bits u16 +6,
level s8 +8, stage s8 +9 [1 = held above player, 0 = flying], src vec +0xC,
fx vec +0x18, dest pad spline +0x24), and `UpdateAwards` (0x1DBF68) rides it
above the tumbling player, pops it (GameSfx 0x26 + AddGameDebris 0xA1),
flies it to the pad and **commits `Game.level[].flags |= bits` + XOR-clears
`new_lev_flags` + CalculateGamePercentage on arrival**. Note `CheckFinish`
REVERTS its flag writes (game.c:3200) into `new_lev_flags` — the durable
commit happens per-award at flight arrival (or creature.c:1717 direct when
`AddAward` returns 0). Consequence: a merged bit ⇒ the remote is
mid-celebration NOW. The mod replays it on the puppet (`coop_award_celebrate`):
real `AddAward(Hub, level, group_bits)` per gotlist group ({8, 7, 0x10…0x400},
creature.c:1564), then rewrites the slot to stage 0 with src = puppet
mid-body + the pop chime/sparkle; retail flies it and its arrival re-commit
is idempotent (our merge already OR'd the bits). Gates: Level 0x25, puppet
active, level ∈ HData[Hub].level[] (AddAward samples the LOCAL hub spline —
a different-hub peer is skipped), ≤3 awards/tick (Award has 3 cycling
slots). The stage-1-skip matters: stage 1 reads the GLOBAL player + tumble
globals — never let a mod-spawned award run it.

**v2 of the gate + deferred commit (same day, after the first cut failed
in-game):** (a) a link-freshness warmup CANNOT gate the celebration — the
remote's level→hub load stalls its publishes past STALE_LIMIT and resets
any warmup right before the tumble commit arrives, so it silently skipped
every time. Fresh-finish detection is now the remote's PREVIOUS distinct
room (`g_remote_prev_room`, maintained next to `g_last_remote_room`): only
bits of the level the remote just came from celebrate; profile catch-up
bursts (peer was already in the hub / just connected) merge silently.
(b) celebrated bits are NOT direct-committed — they ride `g_award_pending[35]`
and retail's UpdateAwards commits them (+ CalculateGamePercentage + pad
sparkle) when the flight LANDS, so the pad marker no longer "plops up"
seconds early; safety: pending is dropped on level change (next merge tick
direct-commits) and force-flushed after 750 ticks if a flight is lost while
staying in the hub; bits already in the local `new_lev_flags` (0x6310D2 —
both players finished the same level simultaneously) are skipped, the local
player's own award commits them.

**v3 = mailbox v9 (same day; v2 was in-game confirmed working but ~2 s late
and spawned off a jumping puppet):** the committed `level_flags` bit only
exists AFTER the remote's flight lands, so triggering off it is inherently
one flight-duration late — and by then the puppet is doing the post-tumble
jump, which is why the crystal popped out too high. v9 publishes
`pending_flags` (= the writer's `new_lev_flags`) + `pending_level`
(= `last_level` 0x630B98) at slot 0x114/0x116; `seq_close` → 0x118, slot
0x11C, remote abs 0x706B8C, mailbox 0x248. A falling edge in the remote's
`pending_flags` (coop_award_pop_edge, latched every tick) is the
frame-accurate pop cue — the local flight launches the same frame as the
remote's, off a puppet still in the hold-up pose (src = pos + 0.9, retail's
mid-body). Edge bits are filtered by local Game flags + `g_award_pending`
(a staleness resync that drops bits without a pop can't double-spawn); the
merge-time fallback (prev-room gate) stays for missed edges (stale gap over
the pop, or entering the hub mid-flight). Bridge codec v9 in lock-step
("…2IHhI"), peek prints `pending X@level`.

**v4 (same day; v3 behaved in-game EXACTLY like v2):** v3's premise was
wrong. The tumble does NOT clear the gotlist group from `new_lev_flags` at
the pop — creature.c:1716 (`new_lev_flags = bits ^ (bits | new_lev_flags)`)
is inside the `AddAward(...) == 0` FAILURE branch (award table full →
direct commit, no flight). On the normal path the pop frame only does
`temp_lev_flags |= bits` (0x006310BE, u16; its only other writer is
`HubStart`, which zeroes it — so it's a clean monotonic per-hub-visit
"already popped" mask), and `new_lev_flags` keeps the bit until
`UpdateAwards` XOR-clears it at pad ARRIVAL. Publishing raw `new_lev_flags`
therefore fell at landing = the same instant as the v8 committed-bit
trigger. Fix (mod-side only, v9 layout unchanged): publish `pending_flags =
new_lev_flags & ~temp_lev_flags`, which genuinely falls at the AddAward pop
frame; reader untouched. Lesson: `new_lev_flags`'s two clear sites (tumble
failure path vs UpdateAwards arrival) look interchangeable in a summary —
always check which branch the store sits in.

Contract: `coop_mailbox.h` v7→**v8**, progression block at slot 0x98..0x114
(`bonus`, `items`, `level_flags[35]`, `powers`, `hub_flags[6]`,
`crate_bits[8]` reserved for 3b, `item_bits[2]`), slot 0x9C→**0x118**, remote
slot abs **0x706B88**, CoopMailbox 0x148→**0x240**. `mod.toml [mailbox]`
0x200→**0x400** and `mailbox.h payload` 0x1E0→0x3E0 (mailbox base 0x706A40 and
payload 0x706A60 unchanged — only the mod image after the region shifts;
build verified). Bridge `mailbox.py` codec/addresses/tests + `coop-peek`
popcount lines updated in lock-step, `COOP_VERSION = 8`.

Mod mechanics (mods/coop/mod.c): `coop_pickup_item` replace-hook on
`PickupItem` records the `pObj[]` slot into `g_item_bits` (echo captures kept
— harmless under OR, makes bitmaps converge). `coop_merge` (from `coop_tick`,
after `consume_remote`): tier 1 OR-merges `level_flags`/`hub_flags`/`powers`
into `Game` ALWAYS (gated `local_valid` so we never merge into an unloaded
profile, skipped under `TimeTrial`) and calls `CalculateGamePercentage(&Game)`
when any flag bit is new (that recompute IS the cross-level crystal
visibility); tier 2 (same level, `!bonus` either side, `!Paused`, `!dead`)
ORs `remote.items` into `plr_items` and replays newly-set `item_bits` via
`g_coop_applying = 1; PickupItem(pObj[i]); g_coop_applying = 0` with
`obj->dead` as the idempotence gate. Bitmaps reset on `Level` change (before
merge, so a stale bitmap never replays into a slot-reusing new level).
Invalid-state publishes zero the whole progression block (zeros are the
OR-merge no-op). Ghost mode mirrors the local progression → doubles as the
idempotency self-test (mirror must cause zero re-application).
`g_coop_applying` is the seam for 3b's ResetCheckpoint suppression and the
future asymmetric-rewards mode.

## coop Stage 3 Phase 2 — 3b per-crate sync + checkpoint reconcile (2026-07-12)

No contract change (v9's `crate_bits[8]` was reserved since v8; bridge/relay
untouched). Three new replace hooks (7 total):

- **Capture `CrateOff`** (0x1F3178), NOT BreakCrate: stack chains,
  nitro-switch chains and TNT explosions kill crates via direct `CrateOff`
  calls. Bit set only on a nonzero return (crate really went off), identity
  = flat `Crate[]` slot (stride 0x90, ≤256, deterministic fill order),
  `Bonus == 0` gate; applied echoes captured on purpose (bitmap converges
  from either side).
- **Apply** in `coop_merge` tier 2 (same level, !bonus, !Paused, !dead):
  `remote.crate_bits & ~applied & ~own` replay through the real
  `BreakCrate(group, &Crate[i], GetCrateType(&Crate[i], 0), 0)` under
  `g_coop_applying`, budget 8/tick (chains amplify). "Already destroyed"
  uses retail's own UpdatePlayerStats predicate
  `on == 0 || (newtype == 0xF && metal_count != 0)` (exploded slots keep
  `on != 0` — plain `on` is NOT the destroyed test) → mark applied
  silently. TNT/nitro mid-countdown (`armed != -1`) are skipped and
  retried: their own explosion flips them destroyed. Orphan slots (no
  CrateGroup covers the index) and `idx >= CRATECOUNT` mark applied and are
  never touched (slot-divergence guard; `coop-peek` popcounts spot it).
- **Checkpoints are SHARED** (user decision 2026-07-12, replaced the
  planned own-checkpoints suppress hook): CrateOff's checkpoint branch
  calls `ResetCheckpoint(crate+0x31, crate+0x32, crate+0x34f, &crate->pos)`
  — every arg derives from the CRATE, so the symmetric replay produces the
  identical respawn point and the same CrateTypeData log baseline on both
  sides. Feature = simply not intercepting ResetCheckpoint.
- **Death reset is SHARED** (user decision same day, replaced the union
  rule): either player's death resets BOTH players' post-checkpoint crates,
  exactly like retail as a team. Own respawn: the death block runs
  `RestoreCrateTypeData` + `ResetCrates` BEFORE `GotoCheckpoint`
  (E5C4C/E5C54 → E5D2C), so in the `GotoCheckpoint` hook (event-driven —
  death block + debug menu, never polled) the crates are already final:
  drop the now-intact crates from the published bitmap IMMEDIATELY, then
  pause the apply loop for a 50-tick (~1 s) settle. The peer triggers on
  bits FALLING out of the remote's published set (absolute state, robust to
  stale gaps — a dead-flag edge can be swallowed by staleness, a missing
  bit is still missing on recovery) and mirrors retail's death-block crate
  pair `RestoreCrateTypeData(); ResetCrates();` (the REST of that block,
  ResetWumpa/Chases/AI/etc., is personal world state and is NOT mirrored),
  arming its own 50-tick apply-pause. The drop-intact sweep additionally
  runs EVERY processed tick, so any resurrection path (menu restart, the
  mirror itself) stops being claimed within a tick.
  **Ordering is load-bearing** (v1 of this feature failed in-game): the
  drop must happen at respawn and the settle must pause APPLY — v1 dropped
  at settle EXPIRY, so the dead player's own apply loop re-broke every
  resurrected crate from the peer's still-stale echo bitmap the moment the
  settle ended (its applied mask never covers crates it broke first-hand),
  and the peer never saw a stable falling edge. Symptom: crates popped
  back broken on the dead side, no reset on the living side.
  `RestoreCrateTypeData` self-clears `i_cratetypedata` (crate.c:293), so
  simultaneous deaths double-reset as a no-op. Applied bits re-arm on the
  falling edge (`applied &= remote` each processed tick), so a genuine
  re-break after a reset replays; `g_crate_remote_prev` is zeroed on level
  mismatch (re-entry must not fake a falling edge) but KEPT across
  pause/own-death so a fall is processed late, not lost.
  `ResetCheckpoint(-1,-1,0,NULL)` (death block prelude) only invalidates
  `cp_iRAIL/cp_iALONG` — nothing crate-related, so the two-call mirror is
  complete. CrateTypeData log cap is 0x20 = 32 entries (SaveCrateTypeData,
  crate.c:270) — a retail limitation, identical on both sides.

**Item sync reworked to identity-based (2026-07-12, in-game bug):** the
pObj-slot `item_bits` channel replayed the WRONG object — `pObj[]` fills as
objects stream in, so slot order diverges between instances whose players
explored differently (observed: P1's crystal pickup granted P2 the CRATE
GEM, P2's crystal stayed standing, tallies crossed at level end). The
"deterministic per level load" assumption from the plan is false. Fix: no
capture at all — each reward bit maps to exactly ONE object character in
PickupItem's dispatch (crystal 0x75↔1, crate gem 0x77↔2, coloured gems
0x78→4 0x79→8 0x7A→0x20 0x7B→0x10 0x7C→0x40 0x7D→0x80; powers
0xA7→bit0 0xA5→1 0xA6→2 0xA2→3 0xA4→4 0xA3→5; 0x76 = time-trial clock,
NEVER replay). Merge rule: any LIVE pObj object whose bit ∈
`plr_items | remote.items` (or ∈ the powers rising edge `npw`, computed
before tier-1's OR) → real `PickupItem` (grant + despawn + effects),
≤2/tick, retried while the object is alive (self-healing for objects that
stream in later); remaining remote items bits OR directly (HUD first).
Powers replay is EDGE-only: powerbits persist across levels, an
alive-object rule would auto-collect an owned power on level re-entry.
`item_bits[2]` stays in the v9 layout as reserved/always-0 (no bump; both
mods rebuilt together anyway); the PickupItem hook is gone (5 hooks).

`plr_crates`/box counter needs nothing (re-derived every frame). Ghost mode
mirrors `g_crate_bits` → the apply loop must be a total no-op (idempotency
self-test). Build: 6 hooks, blob 0x706a00..0x70b53c, mailbox 0x706a40.
Needs two-instance confirm (wumpa crate, nitro switch, TNT chain, shared
checkpoint respawn point, death → both players' crates reset + box counters
equal). Playtest watch-item: a crate un-breaking under the living player
(retail never resets crates while a player is standing in the level).

## game/crate — first C bodies (2026-07-12, coop Stage 3 PR-D3)

`BreakCrate` (848B), `SaveCrateTypeData`, `RestoreCrateTypeData` all
**matching** — the first matches in unit 95. `src/game/crate.c` now types
`crate_s` (stride 0x90: pos@0x10, hop_mom@0x24, on@0x30, type bytes
0x3A..0x3F all u8 read through (s8) casts with -1 = none, metal_count@0x41,
grid@0x44/0x48, armed@0x76 s16 with -1 = idle), `crategroup_s` (stride 0x30,
first@0x10/count@0x12) and `cratesave_s` (crate* + 4 saved bytes).

Semantics locked for the coop mod: `BreakCrate(group, crate, type, flags)` —
type 0x13 TNT arm (skipped when `flags & 0x200`), 0xE nitro
(kaboom + newtype=0xF + metal_count=1), 0x11 nitro-switch (chains every
`GetCrateType(pc,0) == 0x10` crate via direct `CrateOff(pg, pc, 0, 0)`),
default → `CrateOff(group, crate, 0, (flags >> 9) & 1)` (NOT `flags & 1` —
bit 9 = the TNT-suppress bit, evaluated in a branch-likely delay slot) plus
the stack-hop rescan (goto-restart loop, `pc->pos.y == cur->pos.y + 0.5f`,
`hop_mom = CRATEHOPSPEED` hoisted to a local). TNT/nitro/nitro-switch are
all gated on `armed == -1`, so re-application is idempotent (coop 3b).

`GetCrateType` stays a faithful **near-match** (state=asm, full C in tree):
everything byte-identical except ONE `daddu` copy in the `type == 0` movz
block — reload materializes retail's ITE else-operand copy where our build
coalesces it; every same-BB C form (ternary, if/else, temp local, destructive
rebase, assignment-in-cond) produces the 3-insn coalesced form. Resume there
if a new regalloc technique appears.

**Toolchain bug found + fixed (tools/gen_hybrid.py):** the decompals `as`
emits TWO spurious COP1-hazard nops when an alignment directive follows an
FP materialization (`li.s`/`li.d`/raw `mtc1`) in reorder mode — even at an
already-aligned position (BreakCrate's hop-loop `.p2align 3` after the 0.5f
lui+mtc1 gained 8 bytes → `.org backwards`). Entering noreorder *after* the
mtc1 flushes the same nops, so `_fix_fpmacro_align` brackets the pair in
`.set noreorder … .set reorder`; `_sonyize` also normalizes `.p2align N` →
`.align N`. Covered by tests/test_hybrid_align.py; all pre-existing matches
re-verified byte-identical under the change (full image SHA gate).

## game/game_obj — PickupItem + HitItems (2026-07-12, coop Stage 3 PR-D2)

Both **matching** (720B + 192B). `PickupItem` is the by-character dispatch
(jtbl base 0x75, 0x33 entries): 0x75 crystal, 0x76 time-trial clock
(`StartTimeTrial(&obj->pos, 0)`), 0x77 crate gem, 0x78–0x7D coloured gems
(bit from an if-chain: 0x79→8, 0x7A→0x20, 0x7B→0x10, 0x7C→0x40, 0x7D→0x80,
else/0x78→4), 0xA2–0xA7 powers (sets `boss_dead = 2` when
`LBIT & 0x3E00000`). **The power mapping differs from `PickupPower`**:
here 0xA2→3, 0xA3→5, 0xA4→4, 0xA5→1, 0xA6→2, 0xA7→0. Each pickup case ends
with a second `GameSfx(0x26, 0)` (cross-jumped tail). Function tail = the
`KillItem` body written out (dead=1, parent off_wait=2, on=0).
`HitItems(obj)` scans `pObj[0..0x40)`, skipping null/dead/invisible, and on
`flags & 0x10` + `GameObjectOverlap(obj, po, 0)` calls `PickupItem(po)` and
returns 1.

**Matching techniques found:**
- **GCSE/PRE re-read copy**: retail reads `obj->character` through a u16
  view (`(s16)*(u16 *)&obj->character`) in BOTH the switch cond and the
  gem-case compares. GCSE unifies the two loads into one `lhu` plus a
  **register copy** (`daddu a1, v0`), the copy blocks the lh-fold, and the
  load dies at the switch subtract (2-address `addiu v0, v0, -0x75`). No
  local-variable formulation reproduces this — plain copies always get
  copy-propagated away; the *re-read of the same expression in another
  basic block* is what makes the pattern. (This exact 4-insn shape is
  unique to PickupItem in the whole image.)
- **Switch index type**: switch on a short-typed expression → HImode dance
  (`addiu; sll; sra; sltiu`); switch on an int local/expression → plain
  SImode (`lh; addiu; sltiu`). PickupItem's outer switch is short-indexed,
  its inner power switch reloads through a fresh `s32` local (plain `lh`).
- **Case-body order = source order**: the inner power switch's bodies sit
  in VALUE order (0,1,2,3,4,5), so retail listed the cases in that order
  (0xA7, 0xA5, 0xA6, 0xA2, 0xA4, 0xA3); getting this wrong costs the
  shared-store crossjump (+4 insns).

## game/game_obj — pickup handlers (2026-07-12, coop Stage 3 PR-D1)

All six tiny pickup functions matched + promoted first/second try: `KillItem`,
`PickupCrystal`, `PickupCrateGem`, `PickupBonusGem` (gem bit in $a0),
`PickupPower` (0xA2..0xA7 → `new_power` via jtbl, `Game.powerbits |= 1<<n`),
`PickupRelic` (tier byte; relics reuse the crystal panel slot). The `plr_*`
panel counters are 8-byte `panelcount_s` (count +0, draw +2, frame +4 = sparkle
timer, byte +7 = variant: relic tier in `plr_crystal`, gem bit in
`plr_bonusgem`), all $gp-relative.

**Matching technique — adjacent independent global stores are
scheduler-permuted.** When the ONLY mismatch is the order of neighbouring
stores to distinct globals, permute the source statements; the compiler applies
a deterministic reorder, so write the source in the order that lands on retail:
a 2-store pair emits **reversed** (source `count; frame` → retail
`frame; count`), the 3-store tail in `PickupPower` emits **rotated right**
(source `mom.x; mom.z; slide` → retail `slide; mom.x; mom.z`). Same family as
the "store before call = source order" note from DoInput.

## Coop VS mode — per-instance model tinting (2026-07-12)

VS mode (`coop-bridge --vs`) needed the crystal rendered in a player colour.
Findings that made it possible (all asm-verified):

- **No colour parameter exists in the item draw chain** and items are NOT
  dynamically lit: `DrawCreatures` maps char 0x75 → `ObjTab[0x84]` and
  `Draw3DObject` (0x1F0980) ends in `NuRndrGScnObj(gobj, mtx)` — no
  `GetLights`, so the puppet's grey-light trick cannot tint items. The six
  coloured gems are six separately-baked models (ObjTab 0x89–0x8E).
- **Gobjs anchor crossfade colour-ref descriptors at gobj+0x5C and +0x64**
  (`NuGobjApplyCrossFade` 0x1160F0 reads exactly those and calls
  `NuGobjColourBlendGobj` 0x159010). Wire format (from the blend walk):
  repeated blocks `{u32 *dst; u32 count; {u32 src0,src1}[count]}`, count==0
  terminates. `dst` points at the LIVE vertex colours inside the prebuilt
  VIF packets; `src0`/`src1` are the two baked colour sets and are never
  written. So a mod tint = walk the descriptor, write
  `luminance(src0) * playerRGB` into `dst`; restore = write `src0` back
  verbatim. Idempotent, reversible, zero copies. (GS byte order assumed
  R-in-low-byte; a swap would just trade red/blue.)
- **BUT retail never builds them** (probed in-game 2026-07-12: crystal gobj
  chain resolved perfectly, +0x5C/+0x64 both NULL). The builders are gated
  on the Nu option global `nugscn_generate_colourref` (0x0062EE28), read at
  scene load by `NuPs2CreateRenderStream`/`NuPs2CreateFaceOn` — and raising
  it does NOT help either: `ReadNuIFFGobj` (0x118530) skips the runtime
  stream builder entirely when the loaded file already carries prebuilt
  streams (first geom's +0x30 nonzero), which every shipped WoC scene does.
  Dead tooling both ways.
- **Working glow tint (2026-07-13, in-game confirmed)**: parse the prebuilt
  packets directly. Loaded (pre-converted) geom nodes hang off gobj+0xC as
  `{next, material, packet}`; each packet is a DMA tag + VIF stream, and
  the per-vertex colours are the **UNPACK V4-8 payloads** (R low byte — the
  crystal's retail purple is `72 00 6D`). The mod walks the VIF stream
  (UNPACK sizes from vn/vl; STMASK/STROW/MPG/DIRECT skipped; cnt-chains
  followed), saves every V4-8 word and rewrites it as
  `maxcomponent(orig) * playerRGB` (restore = originals back).
- **The crystal BODY: colour = material diffuse as VU1 constants** (found
  2026-07-13 after eliminating everything else by in-game pokes). The body
  is the creature model `CModel[CRemap[0x75]]` (skinned, 26 joints), drawn
  by the regular DrawCreatures model path. NOT the source of its pink:
  per-creature lights (c->lights, SetCreatureLights 0x2472A0 — they tint
  only the lit alpha REFLECTION overlay), the body's V4-8 vertex bytes
  (`00 00 00 44/7F` = weights/unused), and NO texture: tinting every CLUT
  and raw image in the global NuTex list turned the scene green while the
  crystal stayed pink. The actual source: each **material state packet**
  of the model bakes the material diffuse as **VU1 float constants on a
  0..255 scale** — signature `STCYCL 0x01000101` + `UNPACK V4-32
  0x6C030013` (3 quads to VU addr 0x13), RGBA floats immediately after;
  retail crystal = `(97.3, 0.0, 92.7)` with alpha 64 (opaque main
  surface) / 68.8 (alpha reflection overlay). The packet array hangs off
  **hobj+0x08, NULL-terminated** (crystal has exactly these two). Poking
  the two RGB float triples recoloured the world crystal instantly. The
  mod (coop_body_scan/coop_body_write) rewrites the RGB floats (alpha
  kept), replacing the old lights-based reflection-only tint; diag bit 4 =
  body tint active. Dev calibration: 'VTNT' magic + 3 floats (0..255) at
  coop payload+0x248 override the diffuse live over PINE (rewritten every
  tick while set).
- **NuTex texture-system map** (derived for the hunt, kept for future
  texture work — e.g. gem tints or texture swaps): texture list base =
  `*(u32 *)0x0062EBEC` (`D_0062EBEC`), stride 0xE0, tids are 1-BASED;
  entry+0x18 (u64) bit 0x10000 = valid; entry+0x04 = size hint
  (NuTexWidth/Height both read it). The PS2 part is entry+0x20 (ps2tex):
  +0x14 = mip-0 raw image data pointer (EE RAM), +0x93 = GS PSM byte
  (0x00 CT32 / 0x01 CT24 / 0x13 T8 / 0x14 T4 / 0x1B T8H), +0x98 = palette
  upload packet (+0x9C = stream-temp override; CLUT data at packet+0x60 —
  16 RGBA32 entries for T4, 256 CSM1-swizzled for T8: blocks 1/2 of each
  32-colour group swap, see NuPs2ChangeTexPal 0x168618). `NuTexPalChange`
  (0x11ED98) takes `(tid, newPal)` and does the swizzled copy-in.
  Uploads are rebuilt per frame from EE RAM inside the render-list build
  (NuTexAccomodateRS path-3 chain, markers 0xACEB00B5/0xBABEF00D), so
  poking pixel/palette bytes in EE RAM shows up immediately — no GS-cache
  invalidation needed. Materials (`NuMtl`) link textures at mtl+0x1A4
  (tid, `NuMtlBuildRenderList` 0x1AFF0 feeds it to NuTexSet); mtl render
  chain at +0x160. CAUTION: the two PSMT8H entries are 16×16 utility
  ramps whose "palette packet" is a different, much smaller layout —
  writing 1KB at +0x60 there corrupts the heap (crashed the game once
  during the hunt).
  Shipped VS look: strong player-colour glow + carving + fully coloured
  body on both surfaces.
- **HUB level-stone HUD crystal = a THIRD render source** (RAM bisection
  2026-07-13): with glow AND body tinted, the stone HUD stayed pink. It is
  a PANEL-scene material (at 0x100xxxx, a separately loaded scene) — found
  by scanning the heap for the diffuse signature with purple floats and
  bisecting pokes. Mod-usable anchor: the **global NuMtl chain** at
  `*(u32 *)0x0062EBA4` with next at **+0x160** (688 materials spanning all
  loaded scenes; verified identical layout on both instances, hub and
  level, and persistent across level changes — the +0x168 walk that
  NuMtlDisplayMtl uses only yields 1 entry, +0x160 is the real chain).
  The stone-HUD material is identified by its bit-exact retail crystal
  diffuse `0x42C2A5A4 / 0x0 / 0x42B966BA`; the only other chain matches
  are the two ObjTab[0x84] glow material diffuses, which are invisible
  (poke-proven), so tinting all matches is safe. The mod keeps a
  session per-level crystal-owner table and colours the stone HUD by
  `hubleveltext_level`'s winner while `hubleveltext_open` (globals
  0x631090/0x631098, written by HubSelect, read by DrawPanel). Writes
  re-verify the UNPACK signature word first (stale-heap guard).
  Gotchas found in the first two-instance hub test: (1) peer claims made
  while the players were in DIFFERENT levels never reached the other
  instance's owner table — the merge's level-mismatch early-return sits
  before the g_vs_peer attribution; fixed by attributing the remote
  slot's crystal bit against `remote_snap.level` (first-wins guard keeps
  own claims; echoes can't appear cross-level because item replays only
  run level-matched). (2) ALL hub stone crystals render from the ONE
  shared material, so while a stone HUD is open every visible stone
  crystal mirrors that stone's colour — per-stone simultaneous colours
  need per-stone gobj/material clones (roadmapped follow-up: clone gobj
  header + geom nodes + the 224-byte material state packet per claimed
  stone, SHARE the vertex packets, repoint the stone's instance record
  gobj index, tint clones once; prototype the scene-table append vs
  pointer-patch question over PINE first).
- **The pause-panel carving draws the SAME gobj as the world item**:
  `DrawPanel3DCharacter` (0x239EC0) remaps item ids (0x75→0x84, 0x77→0x88,
  0x78..0x7D→0x89..0x8E) and resolves
  `gobj = (*(scene+0x14))[ *( *(special+0x40) + 0x40) ]` — byte-identical to
  Draw3DObject's chain (0x1F0B40 vs 0x23A130). One tint covers both: own
  colour pre-claim, winner colour after the claim (the world crystal is
  gone by then, so the retint only shows in the carving).
- **Ownership needs no wire data**: a first-hand pickup marks `g_vs_mine`
  synchronously in the PickupItem hook, while the peer's claim can only
  arrive later through the bridge (`remote.items` bit not already ours);
  both sides derive the same owner. Player identity (P1 blue / P2 red) comes
  from the ctl word: the bridge writes `COOP_CTL_VS` (+`COOP_CTL_P2` on the
  second endpoint) every cycle — ctl BITS are not a layout change, v9 stays.
- Bridge-side attribution (`coop/vs.py` `VsRecorder`) is authoritative
  because the bridge IS the transport: the origin's rising edge is always
  observed before the echo can be delivered. The first slot per side only
  primes baselines, so a pre-existing save never generates claims.

## nucore/nuerror — whole unit matched (2026-07-12)

All 7 functions **matching** on the first compile each (unit 3 fully C for
`.text`; `complete` stays false — the unit still owns 2 data ranges):
`NuErrorFunction`/`NuWarningFunction`/`NuDebugMsgFunction` (variadic
printers), `Nu{Error,Warning,DebugMsg}Prolog` (`__FILE__`/`__LINE__`
stashers returning the printer's address), `NuAssertMsg` (printf wrapper).

**Varargs recipe for this toolchain (first variadic match):** the SN ee-gcc
install ships NO libc headers — `#include <stdarg.h>` fails with "No include
path". Define locally:

```c
typedef char *va_list;
#define va_start(ap, last) \
    ((ap) = ((va_list)__builtin_next_arg(last) \
        - (__builtin_args_info(2) < 8 ? (8 - __builtin_args_info(2)) * 8 : 0)))
#define va_end(ap)
```

This is gcc 2.95 `va-mips.h` (EABI): the variadic prologue saves a1..t3 +
f12..f18 at the frame top, and `va_start` must point at the FIRST anonymous
GPR save slot. Bare `__builtin_next_arg` points past all 8 GPR slots (was
+0x38 for 1 named arg — the lone diff word before the fix);
`__builtin_args_info(2)` = named GPR args consumed, so the subtraction lands
on the saved-a1 slot. `va_arg` wasn't needed (bodies hand the list straight
to `vsprintf`).

Other facts locked here: sbss error state cluster `D_0063300C` (full path,
`_fbss+0xC`, unnamed in the registry), `D_00633010` (basename),
`D_00633014` (line); `strrchr(file, '\\')` with the unconditional
store-then-reassign global pattern matched directly. Log bookkeeping:
`D_0062E9E0` = log created, `D_0062E9F4` = NuDebugMsg re-entrancy guard,
`D_0062E9F8` = message counter (`++ctr` inline in the sprintf arg),
`errmsg_to_file`/`D_0062E9DC` route to file vs console. NuDebugMsg brackets
its body in `NuDisableVBlankE`/`NuEnableVBlankE` and appends via
`NuFileOpen(name, 2)` + `NuFileSeek(f, 0, 2)`, falling back to mode 1
create. NuErrorFunction ends in `for (;;) {}`. Local-buffer layout: the
vsprintf body buffer declared first sits at sp+0, the header buffer above it
(buf/hdr = 0x1000/0x100 error, 0x400/0x100 warning, 0x400/0x400 debug).

## Fire Boss sync surface (PR-S4-D2 → gates PR-S4-C)

`ProcessFireBoss` (0x00228D70, 6.2KB) stays `state=asm`: two dense
`switch(action)` dispatches compile to `.rodata` jump tables
(`jtbl_0061FB00` transition, `jtbl_0061FB20` per-frame) that the hybrid
pipeline cannot own — architectural `compiler_owned_rodata` blocker recorded.
D2's deliverable is the typing + this sync surface, not a byte match.

State machine: `action` (`fb+0x61C`, 0..5 via the `fireboss_action` enum) with
`prev_action` (`fb+0x620`) as the transition edge; `mech_phase` (`fb+0x40C`,
4→0) selects the in/out mech position via `InMechPos`/`OutMechPos`/`*Z` tables
(`(4-mech_phase)*0xC`). `health<=0` (`fb+0x408`) forces `action=5`.

**Host→client (absolute state, publish each frame):**
- `action` u8 (`+0x61C`) — drives anim + wall-of-fire; `mech_phase` u8 (`+0x40C`)
- `health` s16 (`+0x408`, mirrors `FireBossHealth`)
- `pos[3]` f32 (`+0x418`; Y raycast-clamped; mirrors `FireBossPosition`)
- `heading` f32/u16 (`+0x414`); `state_timer` f32 (`+0x618`)
- rock throws: monotonic `rock_count` (bumped when `AddRock` returns non-null →
  `fb+0x684`), with `throw_pos[3]` (`+0x62C`) + type∈{0,2}. Replay client-side
  via `AddRock(v000, type, angle)` (`angle` = `D_0062E17C`/`D_0062E180`).
- wall-of-fire globals the brain writes / `DrawFireBoss` reads →
  pack as `flags` bits + `wallfire_yaw`: `WallOfFireOn`, `WallOfFireAttatched`,
  `WallOfFireAngleY` f32, `WallOfFireHurtTimer`; `WallOfFirePosition` derives
  from `pos`. `water_hit` (`+0x67C`) → flags bit.
- terminal: `FireBossWon`/`FireBossFinished` set from `FireBossHealth<=0` in
  `ProcessFireBossLevel` (already matched) — fires client-side off mirrored health.

**Never sync (instance-local pointers, rebuilt locally):** `model.cmodel`
(`+0x440`), `rock` (`+0x684` AddRock handle), `spline` (`+0x5E4`). Client runs
`InitFireBoss` (models+spline) once, then drives the puppet with
`MyChangeAnim(&fb->model, anim_id)` on `action` change + `MyAnimateModelNew(
&fb->model, dt)` and `(&fb->model_hurt, dt)` every frame so native `DrawFireBoss`
renders live — same doctrine as the player puppet.

**Client→host:** balloon damage as monotonic `balloon_hits` (client
`ProcessJeepBalloon` decrements `FireBoss.health` locally; host applies the
delta). Camera is cosmetic only (`JudderGameCamera(GameCam)` screen-shake +
`JeepCamTween` global float) — not structural, not synced.

## Invariants (do not break)

- **Nothing game-derived is committed.** All splat output (asm, linker script,
  `build/assets/*.bin` raw dumps, generated macros) is gitignored; only config is
  tracked. A new tool that emits game bytes must route them into `asm/` or
  `build/`.
- **`configure.py --check` is the determinism gate.** It splits twice and
  compares every generated file. Run it after any change to `splat.yaml`, the
  symbol registries, or the disassembler versions.

## Toolchain quirks

- **Two compilers, and the game half is SN ProDG.** The game/Nu-engine TUs
  (profile `default`) match under **SN ProDG ee-gcc 2.95.2-EE** run via `wibo`;
  the Sony 2.9 build (`sce`) provably did not build them - it emits `sd/ld` where
  retail saves callee GPRs with `sq/lq`, and its cc1/`as` unconditionally pad
  R5900 short loops with nops that retail lacks.
- **Hybrids assemble with the decompals `as`, not Sony's.** Sony's `as` predates
  `%gp_rel(sym)($gp)` and pads loops, corrupting spliced slices. `gen_hybrid`'s
  `_sonyize` normalizes the two pseudo-op encodings that differ (`move`→`daddu`,
  one-operand `break N`).
- **`mtc1→{swc1,c.le.s}` hazard nop.** Retail's SN `as` inserts it; the decompals
  `as` inserts only the `mtc1→cvt.s.w` one. This blocks a few otherwise-exact
  functions (SetLevel, ModelAnimDuration) - a tooling fix, not a source problem.
- **Data/pool codegen is supported** in `gen_hybrid` (see
  `config/pal103/compiler_knowledge.toml`): `.lit4`/`.lit8` pools mapped by value,
  initialized-local `.sdata`/`.rodata` borrowed onto the owned retail slot, switch
  jump tables borrowed by structure, local strings. Caveat: `dli` is a macro, so
  its expansion is assembler-dependent and can block a *matching* candidate even
  when the bytes happen to agree.

## Matching gotchas

- **Prefer `.c` units.** Some runtime routines are hand-written r5900 assembly in
  retail (e.g. `strlen`, unit 211 - 128-bit `lq`/`pcpyld`/`psubb`) and can never
  match from C.
- **Wrap the body vs early-return.** A top guard written `if (!cond) { …; return
  Y; } return X;` keeps the guard's `return X` at the end and lets a mid-body
  early return share a single `blez → epilogue`; a flattened `if (cond) return
  X;` cross-jumps that return with a later identical one and shifts the tail.
  Mirror retail's brace structure, not just its logic.
- **EE FPU is always round-toward-zero.** `(int)(float)` lowers to `cvt.w.s` (no
  `trunc.w.s`); write exact decimal float literals (e.g. `0.59999996f` for 0.6).
  Floats/doubles far from `$gp` must be declared `extern T NAME[]` and read `[0]`
  so gcc uses `%hi/%lo` rather than a truncating `%gp_rel` (GPREL16 overflow).

## Structural quirks

- **VU microcode is raw `bin`.** `.vutext`/`.vudata` are VU, not MIPS -
  emitted as `build/assets/*.bin`, not disassembled; their 17 microprogram
  symbols are dropped until VU gets real treatment. A 0x80-byte orphan MIPS run
  between `.vutext` and `.data` (vram 0x293680) is loaded but owned by no section
  header; homed as `orphan`/`unassigned` until evidence attributes it.
- **`.mdebug` line info exists only on SCE runtime units** (229 `.s` procedures);
  the 3522 game `.c` procedures were compiled without it.
- **bss split is exact in placement, approximate in size.** `.sbss`/`.bss` use
  explicit vram so symbols land correctly; the vram gap makes splat over-report
  `.sbss` size. NOBITS, so no bytes are affected. Revisit for byte-exact relink.
- **`.reginfo` is not modeled** (unloaded, overlaps NOBITS) - relink-exact goal
  only.

## Build & modding

- **Regenerate `build.ninja` by hand** (`python tools/gen_ninja.py`) after adding
  a source file, status manifest, or header - ninja can't discover new edges.
  `configure.py` also regenerates it after every split.
- **Header deps are coarse:** `gen_ninja` lists every `include/*.h` as an implicit
  dep of every compile edge (cc.py compiles from a scratch dir, so a real depfile
  would carry unusable paths). Correct, just conservative.
- **`expected/` is flat** (`NNN_<name>.o`); the human-facing tree lives in the
  objdiff unit *names*, which objdiff renders as a tree.
- **Relocatable equivalent link is a future milestone** (the PCSX2 multiplayer
  patch needs it). Today links pin externals to fixed retail addresses, so
  equivalent-build edits must stay same-size or trampoline into slack. Booting
  `build/pal103/image/equivalent.elf` in PCSX2 is untested.
