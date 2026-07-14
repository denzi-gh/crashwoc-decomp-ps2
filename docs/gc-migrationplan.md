# GC → PS2 Decomp Migration Plan

## Context

Migrate portable game code from the GameCube decompilation (`denzi-gh/crashwoc-decomp`, local checkout at `C:\Users\denis\Documents\crashwoc-decomp`, HEAD `53f8ff2d982b9b1931d82a56805b3ddb775ef576`) into the PS2 matching decomp (`crashwoc-decomp-ps2`, PAL v1.03 SLES_503.86). Both builds used the SN ProDG gcc 2.95.2 family, so GC C is a strong structural draft — but it is **rank-6 hint data**: every branch direction, switch constant, call site, and struct offset must be re-derived from the PS2 retail disassembly. All work is local-only on branch `gc-migration`.

## Verified environment state (2026-07-14)

- Branch `gc-migration` exists and is **checked out**, working tree clean (HEAD `26f004f`).
- GC reference checkout exists with full source under `src/{gamecode,gamelib,nu3dx,nucore,numath,nuraster,nusound,nuxbox,system,runtime}`. Pilot files (`chase.c`, `vehterr.c`, `listman.c`, `game_deb.c`) all present in `src/gamecode/`.
- Dev container `crashwoc-dev-9886fd02` is running; `python tools/dispatch.py` round-trips OK. `orig/pal103/SLES_503.86` present; repo is configured (`build.ninja`, `objdiff.json`, `asm/text.s`).
- `python -m tools.decomp_agent.cli health`: green. State counts: **3340 asm / 8 equivalent / 139 matching** across 195 status manifests (all units already scaffolded with skeleton `src/**.c` and manifests — no `promote.py --init` needed). One pending repair: `python tools/gen_ninja.py` ("a status manifest is newer than build.ninja") — run in Phase 0.
- `include/` contains only `creature.h` → Phase 1 header layer is greenfield.
- Migration tooling does **not** exist yet: no `tools/xref_gc.py`, no `tools/port_draft.py`, no `config/pal103/gc_xref.toml`, no `docs/gc_migration.md`.
- Pilot units all at 0 matching: `game/chase` (unit-0118, 10 fns), `game/vehterr`, `game/listman`, `game/game_deb`.
- **Existing WIP near-matches live in tree as `state=asm`** (per docs/notes.md + memory): game/main (LoadLevel, SetLevel, InitWorld), game/game (InitLevel, HubSelect), game/game_obj (PlayerCreatureCollisions), game/move (MovePlayer), game/credits (DrawCredits), game/creature (DrawCreatures). **GC drafts must never overwrite existing non-skeleton function bodies.** The 8 `equivalent` functions are also untouchable.
- MCP server `crashwoc-decomp` is available (compile_diff, get_disassembly, sessions, blockers, promote_matching).

## Hard rules (supersede repo docs where stated)

1. All commits local on `gc-migration`. `git push`, `gh pr create`, any remote contact: **forbidden**. Frequent small commits (this session explicitly supersedes the repo's "never commit" rule). Never commit anything from `orig/`, `asm/`, `build/`, `expected/`, `reference/`, or game-derived bytes.
2. PS2 disassembly is ground truth; GC C is rank 6. Read PS2 asm first for every function. Verify branch directions, switch constants, every `jal` ↔ C call correspondence, all offsets. **Three-strike rule:** GC-derived draft not converging after 3 compile/diff iterations → delete, re-derive from asm alone.
3. Compile only via `tools/cc.py` / `compile_diff`. Promote only via `tools/promote.py` / `promote_matching`. Never hand-edit a manifest to `matching`; never set `equivalent`. Profile from the manifest (`default` for game/engine TUs). After any header change: `python tools/verify_promoted.py` (via dispatch) before committing.
4. GC layout facts (sizeofs, memset lengths, struct offsets) are HOSTILE DATA. Known divergences: `game_s` 0x40C (GC 0x414), Cursor memset 0x78 (GC 0x80), GC lacks `surround`. Every layout fact re-derived from PS2 load/store offsets, documented `/* field (+0xNN) verified in <FunctionName> */`.
5. Data-from-C is unsupported: no file-scope globals emitting `.data/.sdata/.rodata/.sbss/.bss`; use `extern D_...` + `#define` aliases.
6. Decomp-agent stop conditions apply: budget 8 attempts (extend to 20 only while improving), stop after 4 non-improving or 3 compile failures, record blockers only after reading disassembly, restore best checkpoint before ending a session.

## Phase 0 — Setup & tooling (commit each step)

0. Copy this plan to `docs/gc-migrationplan.md` → commit (`docs: add gc migration plan`). (Deferred from plan mode — user requested it explicitly.)
1. Run `python tools/gen_ninja.py` to clear the stale-manifest warning; commit only if it changes tracked files (it shouldn't — build.ninja is gitignored).
2. `docs/gc_migration.md`: rules digest, pinned GC SHA `53f8ff2d982b9b1931d82a56805b3ddb775ef576` (+ dirty-tree note: the GC checkout has untracked build artifacts), file-mapping table, running scoreboard. Commit.
3. GC access path: create an untracked directory junction `reference/gc` → `C:\Users\denis\Documents\crashwoc-decomp` (`reference/` is already gitignored; never enters build graph). Tools default to `reference/gc` with a `--gc-root` override so nothing depends on an absolute path.
4. `tools/xref_gc.py`: parse GC function definitions across `reference/gc/src/**` (regex-based C definition scanner, tolerant of K&R/ANSI styles), match names against `config/pal103/functions.toml`, emit `config/pal103/gc_xref.toml`: per PS2 fn → `ps2 unit / addr / state ↔ gc file / line range / confidence` (confidence: exact-name + unit-mapping match vs name-only). Include the known relocations (GC `nu3dx/nuglass.c`→PS2 `game/glass.c`, GC `nusound/sfx.c`→PS2 `game/sfx.c`, `gamecode`↔`game`, `nu3dx`↔`nu3d`). Deterministic output (sorted), `--check` mode like the other extractors. Unit test in `tests/`. Commit generator + generated table + test. **This produces the real overlap numbers** (claimed ~1,299 shared / ~1,161 still asm — verify).
5. `tools/port_draft.py`: given a canonical PS2 function id, look up gc_xref, extract the GC body, emit to scratch (`build/pal103/gc_drafts/<unit>/<fn>.c` — gitignored), with: identifier renames where known, 4-space K&R, and every sizeof/memset size/struct literal/hard offset replaced by `/* TODO(ps2-layout) */`. Never writes into `src/`. Unit test. Commit.

## Phase 1 — Header layer (incremental, gated)

Port GC header *structure*, re-derive every layout from PS2 evidence. Order: `types.h`/`macros.h` → numath (`nuvec.h`, `numtx.h`, `nuquat.h`) → nucore → game structs (`game.h`, `camera.h`, `inst.h`), following the existing `include/creature.h` pattern. Rules:
- A struct enters `include/` only once ≥1 function using it is `matching`.
- Every field carries `/* +0xNN verified in <Fn> */` where derived from asm.
- After every header commit: `python tools/dispatch.py python tools/verify_promoted.py` must pass (139+ matching preserved) before the commit is made.
- Existing `src/` near-matches use local struct defs; migrate them to headers opportunistically only when touching that unit, never speculatively.

Note: headers are pulled by need from Phase 2/3 work, not built exhaustively up front — Phase 1 continues interleaved with later phases.

## Phase 2 — Pilot (measure hit rate, then STOP)

Migrate end-to-end, smallest-function-first, recording per-function outcome (matched-as-is / matched-with-tweaks / rewritten-from-asm / blocked):
- `game/chase` (10 fns), `game/vehterr` (9), `game/listman` (9, 5 with reference), `game/game_deb` (7).

Per-function loop (used everywhere):
1. `get_context` / `get_disassembly` — read PS2 ground truth first.
2. `tools/port_draft.py` if gc_xref has a reference; else write from asm.
3. Reconcile draft vs asm: branch directions, dispatch constants, every `jal` ↔ call (inline/un-inline as asm dictates), fill every `TODO(ps2-layout)` from load/store offsets.
4. `start_session` → edit `src/<unit>.c` → `compile_diff` → `checkpoint_session`, iterate. Three strikes on a GC draft → discard, re-derive from asm. Respect budget/stop conditions.
5. Byte match → `verify_candidate --level function` → `promote_matching` → commit (`game/chase: match InitChase` or per-batch).
6. Blocked → `record_blocker` with evidence, move on.

Write measured hit rate into `docs/gc_migration.md`, commit, **stop and present results to the user** — hit rate decides bulk-wave pacing.

## Phase 3 — Bulk waves (after user go-ahead; smallest-first; commit per unit/batch)

- **Wave A** (`game/`): bug, cloudfx, font3d, sfx, lights, camera, deb3, cut, vehsupp, move, jeep, crate, game_obj → then game/main/panel/ai remainder → **vehicle last** (226 fns). Preserve all existing WIP bodies; extend, don't replace.
- **Wave B** (`gamelib/`): nubridge, nuwind, edfile, debris, gcutscn, terrain. GC builds terrain at `-O0` — if terrain stalls, record a per-TU profile anomaly blocker rather than grinding.
- **Wave C** (engine, safe subset): nutexanm, nuscene, nuanim, nucamera, nugobj, nufile, nufpar, numath remainder (nuvec/nuvec4/nuplane/numtx/nutrig already warm). **Skip:** nurndr, nutex, nusound (divergent platform internals).
- **Out of scope** (no GC reference): sdk, nups2, edtools, game/ed*.c, game/vu.c, nu3d/{nucvtgeo,nufpatch,nustream,nunode,nugscn}.c, gamelib/{saveload,sceneman,specterr,trigger}.c.

Definition of done per unit: every reachable fn `matching` or recorded blocker; `verify_promoted.py` clean; `tools/link_image.py` reproduces retail SHA; structs promoted to `include/` with verified offsets; no `TODO(ps2-layout)`; manifest reflects reality; committed locally.

## Verification

- Per function: `compile_diff` exact=true over full extent, then `verify_candidate --level function`.
- Per promotion: `tools/promote.py` self-verifies + rolls back.
- Per unit / per session end: `python tools/dispatch.py ninja check` (current + verify-loaded + verify-promoted) — full loaded image must reproduce retail SHA `c92a5987…0fe438`.
- Structure gates natively: `python -m unittest discover -s tests -v`, `python tools/status.py --check` (must stay green after xref/tooling commits).
- Scoreboard in `docs/gc_migration.md` updated per wave.

## Risk notes

- PAL v1.03 vs GC USA rev0: suspect language/level-table bounds and timing constants (50Hz float literals scale ×50/60 — see docs/notes.md PAL literal rule).
- Known SN-as hazard-nop wall (FP compare / mtc1→swc1) blocks some functions regardless of source quality — record as blockers, don't grind (SetLevel/HubSelect precedent).
- GC reconstruction is itself unverified (285/286 NonMatching) — algorithmic hints only.
