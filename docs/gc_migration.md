# GC → PS2 decomp migration

Working notes for migrating portable game code from the **GameCube**
decompilation into this **PS2** matching decomp. Full plan:
[docs/gc-migrationplan.md](gc-migrationplan.md).

## Source of the hints

- GC repo: `denzi-gh/crashwoc-decomp`, pinned at
  `53f8ff2d982b9b1931d82a56805b3ddb775ef576`.
- Local access: directory junction `reference/gc` →
  `C:\Users\denis\Documents\crashwoc-decomp` (`reference/` is gitignored;
  never enters the build graph). Tools default to `reference/gc`, override
  with `--gc-root`.
- **Dirty-tree note:** the GC checkout carries untracked build artifacts
  (`dump_*`, `*_TBADDED_*` scratch headers). Only `reference/gc/src/**/*.c`
  is scanned; the SHA above pins the tracked source.
- GC reconstruction is itself unverified (285/286 TUs NonMatching). It is an
  **algorithmic hint only** — rank 6.

## Rules digest (full text in the plan)

1. All commits local on `gc-migration`. No `git push`, no PR, no remote.
   Never commit `orig/`, `asm/`, `build/`, `expected/`, `reference/`.
2. **PS2 disassembly is ground truth; GC C is rank 6.** Read the PS2 asm
   first for every function. Verify every branch direction, switch constant,
   `jal` ↔ call correspondence and struct offset. **Three-strike rule:** a
   GC-derived draft not converging after 3 compile/diff iterations → delete,
   re-derive from asm alone.
3. Compile only via `tools/cc.py` / `compile_diff`; promote only via
   `tools/promote.py` / `promote_matching`. Never hand-edit a manifest to
   `matching`; never set `equivalent`. After any header change:
   `python tools/dispatch.py python tools/verify_promoted.py` must pass.
4. **GC layout facts are HOSTILE DATA.** Known divergences: `game_s` is
   `0x40C` (GC `0x414`), Cursor memset `0x78` (GC `0x80`), GC lacks
   `surround`. Re-derive every sizeof / memset length / struct offset from
   PS2 loads/stores; document as `/* field (+0xNN) verified in <Fn> */`.
5. Data-from-C is unsupported: no file-scope globals emitting
   `.data/.sdata/.rodata/.sbss/.bss`; use `extern D_...` + `#define` aliases.
6. Decomp-agent stop conditions apply (budget 8, extend to 20 only while
   improving; stop after 4 non-improving or 3 compile failures; record a
   blocker only after reading the disassembly; restore the best checkpoint
   before ending a session).
7. **Never overwrite an existing non-skeleton function body.** The WIP
   near-matches (LoadLevel, SetLevel, InitWorld, InitLevel, HubSelect,
   PlayerCreatureCollisions, MovePlayer, DrawCredits, DrawCreatures) live in
   tree as `state=asm`; the 8 `equivalent` functions are also untouchable.

## Tooling

- `python tools/xref_gc.py [--check]` → `config/pal103/gc_xref.toml`: per PS2
  function, its GC twin (`gc_file` / line range / `confidence`). Deterministic,
  `--check`-gated. `exact` = GC file maps to the same PS2 unit; `name` =
  name-only.
- `python tools/port_draft.py <id|name>` → `build/pal103/gc_drafts/<unit>/<fn>.c`
  (gitignored): the GC body with hostile layout facts neutralised to
  `/* TODO(ps2-layout) */`. Reconcile against the PS2 asm; never copy into
  `src/` unmodified.

## GC → PS2 file mapping

Directory-level (GC dir → PS2 unit dir):

| GC dir      | PS2 dir  |
|-------------|----------|
| `gamecode`  | `game`   |
| `nu3dx`     | `nu3d`   |
| `nucore`    | `nucore` |
| `numath`    | `numath` |
| `nusound`   | `nusound`|
| `gamelib`   | `gamelib`|

File-level relocations (override the dir rule):

| GC file             | PS2 unit     |
|---------------------|--------------|
| `nu3dx/nuglass.c`   | `game/glass` |
| `nusound/sfx.c`     | `game/sfx`   |

Add new relocations to `FILE_MAP` in `tools/xref_gc.py` (and note here) as
they are discovered.

## Overlap (measured 2026-07-14 by `xref_gc.py`)

- **1366** of 3487 status-carried PS2 functions have a GC twin
  (**1309** exact, 57 name-only).
- **1289** of those are still `state=asm` — the migration pool.

## Scoreboard

Per-function outcomes: `as-is` (GC draft matched unchanged), `tweaked`
(matched with edits), `from-asm` (GC draft discarded, rewritten), `blocked`
(recorded blocker).

### Phase 2 — pilot (measure hit rate, then STOP)

| Unit           | fns | GC-ref | matched | as-is | tweaked | from-asm | blocked |
|----------------|-----|--------|---------|-------|---------|----------|---------|
| `game/chase`   | 10  | 10     | 0       | 0     | 0       | 0        | 0       |
| `game/vehterr` | 9   | 9      | 0       | 0     | 0       | 0        | 0       |
| `game/listman` | 9   | 5      | 8       | 3     | 1       | 4        | 1       |
| `game/game_deb`| 7   | 7      | 0       | 0     | 0       | 0        | 0       |

Notes:
- `game/listman` (done 2026-07-14): of the 5 GC-referenced fns, 4 matched
  (NuLstDestroy/NuLstAlloc/NuLstFree as-is once the GC `0x8000` in-use flag was
  corrected to `0x10000`; NuLstGetNext tweaked). NuLstCreate is a faithful
  near-match (state=asm, regalloc s0/s1 wall). The other 4 fns had no GC
  reference and were reconstructed from asm — all 4 matched. Total 8/9 matching.
  Reusable ee-gcc levers logged in the memory `gc-migration-listman-lessons`.

**Measured pilot hit rate:** _(pending — fill in after all 4 pilot units, then
stop and report to the user; the hit rate decides bulk-wave pacing.)_

### Phase 3 — bulk waves

_(populated after user go-ahead.)_
