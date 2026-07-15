# Working notes

Durable quirks and invariants. 
things that shouldn't be re-derived. Resolved history lives in git.

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
- **`mtc1`→FP-compare hazard nop (fixed 2026-07-15, `_sonyize`).** The two
  assemblers agreed on the latency all along and disagreed on the *dependency*:
  Sony's `as` inserts the nop whenever a `c.cond.s` is adjacent to an `mtc1`, the
  decompals `as` only when the compare reads the register the `mtc1` just wrote.
  Retail `fsign` @0x221530 is the proof - `mtc1 $at,$f0` / nop / `c.le.s
  $f1,$f12`, disjoint registers - while retail `ApplyFriction` @0x2208f8 has *no*
  nop where one instruction already separates them. Usually neither instruction
  is visible in the `.s`: `li.s $f0,1.0` is a macro expanding to `lui $at` +
  `mtc1 $at,$f0`. `_sonyize` now materializes the nop; an explicit one is
  idempotent (the compare no longer sees a hazard, so `as` adds nothing, and the
  dependent case yields exactly one either way). Only the compare class is
  claimed - widen to add.s/swc1 only against retail evidence.
  **ee-gcc's `#nop` comment lines are NOT this marker** and must not be
  materialized: retail ApplyFriction has no nop at one of them. They are
  annotations, not instructions - an earlier note claimed otherwise and was wrong.
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

## Matching lever: many-field 64-bit GS/render register via bitfield union

When retail configures a wide (u64) hardware register by writing many small
adjacent bitfields — e.g. `CreateFadeMtl` (game/main) setting seven GS-register
fields inside the material's `0x168` word — the explicit `(v & ~mask) | val`
C form is functionally identical but loses gcc's register tie-break (the folded
flags value lands in the wrong GPR, e.g. `$t2` instead of retail's `$v1`).

Reproduce retail's allocation with the real source idiom instead: a
`union { u64 raw; struct { ... bitfields ... } bits; }`, with `pad` bitfields to
land each real field at its exact bit position, assignments written in retail's
**emission order**, and each full-width field written as its **max value** so gcc
folds the whole thing to a single OR chain. This matched `CreateFadeMtl`
byte-exact where the mask form could not. Same class of fix as other
whole-function regalloc tie-breaks — the difference is purely which GPR holds
the intermediate, and only the union spelling steers it.
