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
- **`li.d` into a GPR: Sony's `as` sometimes pools it (fixed 2026-07-27,
  `Lit4Mapper.map_gpr_pool8`).** ee-gcc leaves the raw 64-bit image of a
  soft-double operand to the assembler (`li.d $5,0.249` for the `dpcmp` argument
  of any `float < double-literal` compare). Sony's `as` picks between two
  expansions and *both* occur in retail: a cheap image built inline (10.0 →
  `ori`+`dsll32`), or - when inline would cost more - the constant pooled into
  the unit's own `.rodata` and loaded `lui $at,%hi(SYM)` + `ld $reg,%lo(SYM)($at)`.
  The pooled form is %hi/%lo, not gp-relative, so `map_for_slice8` never saw it
  and the constant fell through to `dli`, whose longer expansion pushed the
  function past its extent (`.org backwards`). Now a 64-bit image equal to an
  8-aligned initialized-data slot the function's own retail slice addresses
  borrows that slot, positionally per value - gcc 2.95 does not dedup equal
  doubles, so retail's `FindLocalCrate` holds three identical `0.249` slots
  (D_0061EE90/98/A0) and each load addresses its own. Only the 64-bit form
  borrows: a 4-byte float image could coincide with unrelated referenced data.

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

## Matching lever: unsigned switch index removes gcc's low-bound range test

For a dense `switch` over small non-negative constants (e.g. a shape/type tag
with cases 0/1/2/3), the **signedness of the switch expression** decides the
dispatch shape, and it is worth several instructions:

- **`int` index**: gcc cannot prove `index >= 0`, so the tree's left subtree
  needs a bound test. Because the empty `case 0:` shares a label with the
  implicit default, gcc folds "`< 1` → default" and "`== 0` → default" into a
  single **`slt index,2` + `bnel`** range test (2 insns, and it steals the
  `beq`'s delay slot so case 1 cannot use `beql`).
- **`unsigned int` index**: `index >= 0` is known, the bound test disappears and
  gcc emits a flat equality chain — `beq 1` / **`beqz`** / `beq 2` / `beq 3` /
  `b default` — in *preorder of the balanced case tree* (root first: 1, 0, 2, 3).

Retail's `DebugRenderTriggers` (gamelib/trigger) shows the unsigned form, so the
sub-record tag `NuTriggerSub.type` is `unsigned int`. Writing it `int` costs +1
insn per switch and blocks the match. `case 0: break;`, `case 0: continue;` and
an explicit `default:` all compile identically — they do **not** un-fold the
range test; only the signedness does.

Diagnostic: an `slt <index>,<maxcase+1>` right after the first `beq` in a
dispatch chain means "make the index unsigned", not "reorder the cases".

Confirmed by probe (three switch spellings compiled through the locked profile);
see also the sibling dispatches in `CheckParentedTriggerWithPos` /
`CheckUnparentedTriggerWithPos`, which use the same 1/0/2/3 order and are the
next targets in this unit.

### Same function, supporting levers
- `NuRndrLine3d(line,0,0)` takes **two 0x24-stride vertices** (pos at `+0x00`,
  colour at `+0x18`); the pair is one `struct NuRndrVtx line[2]` local, and the
  two `col = -1` stores emit **reversed** (write `line[1]` first) — the adjacent-
  store permutation already noted for game/game_obj.
- Block-scoped temps get **overlapping** frame slots across disjoint `if`/`else`
  arms (parented arm: vec@0x50 + mtx@0x60; unparented arm: mtx@0x50), and every
  stack slot is 16-byte aligned. Declaration order inside each arm sets the
  offsets.
- Explicit pointer locals (`trig = &sys->triggers[i]`, `sub = &def->subs[k]`)
  give retail's stable base registers: `addu base,base,index`. Indexing inline
  instead yields `addu dst,index,base` and swaps the loop-head allocation.

- **Anchor caller searches.** `jal[ \t]+NuRndrLine3d` also matches
  `NuRndrLine3dDbg`, which is a `jr $ra; nop` **stub**. The whole
  `TerrDraw`/`SphereDraw`/`DrawWallSpline`/`DrawCube`/`edlightDraw*` family calls
  the stub and can never render. Use `/jal[ \t]+NAME$/`.
- **Check the data before the code.** `DebugRenderTriggers` is live, correct,
  armed by default, and calls the real `NuRndrLine3d` — and still draws nothing:
  **zero `.trg` files ship on the disc**, so `g_NuTriggerSysList` is 0 in every
  level. A renderer with no data is indistinguishable from a broken one until you
  look at the disc. Same trap twice more: `specterr`'s renderer is armed but
  nothing builds its input, and `LData[0x2A]` ("testzone") names assets that were
  never shipped.
- **A `.lit4` constant is not a variable.** It is a **shared compiler literal**
  for the whole TU (`TerrDraw` gates on `65535.0f` from `.lit4`). Patching one
  corrupts every other use of that value. Only `sdata`/`sbss` globals are real
  switches — worth knowing when reading any gate.

## game/crate, session 2026-07-27 (AddCrateExplosion / CrateBounceReaction / CrateOff)

`CrateBounceReaction` **matched and promoted** (31 matching / 25 asm). Its old
blocker called it "large FP physics"; it is nothing of the sort — a plain
if/else-if dispatch chain on `type`. Third time a GC-reference-derived blocker in
this unit was wrong. **Read the disassembly before trusting a recorded blocker.**

Levers that landed, all reusable:

- **A ternary of two *comparisons* is not a ternary of two values.**
  `c ? a == k : b == k` goes through `do_jump`, which expands both arms branchy
  and lets them share a tail — retail's shape. `(c ? a : b) == k` materialises a
  register first and does not match. Pick the arm order whose *false* branch is
  retail's fall-through; swapping the arms flips `beq`/`bnel` and moves which
  arm's first insn the delay-slot filler annuls.
- **`fold_truthop` merges adjacent field compares.** `crate->newtype == -1 &&
  crate->subtype == 9` (0x3E, 0x3F) becomes one `lw 0x3C` + `and 0xFFFF0000` +
  compare against `0x09FF0000`. Do not go looking for a 32-bit field there.
  The same fold needs help across `||`: write `(t == 0xD || t == 0xE)`
  parenthesised to get retail's `(unsigned)(t - 13) < 2` range test.
- **A lone `mov.s`/`daddu` copy after a load is GCSE PRE**, never a C local.
  Re-read `crate->timer` at both use sites instead of caching it in an `f32 t` —
  the local removes the copy and lets the delay-slot filler duplicate a global
  load instead.
- **Hoist a shared assignment out of the arms.** `bounce = 1` at the end of the
  `type == 6` arm (rather than in each branch) is what lets cross-jumping fold it
  into the common tail; each inline copy costs a word *and* changes which insn
  reorg puts in the call's delay slot.
- **`i_cratetypedata++` reloads when inlined.** The stores through
  `struct CRATETYPEDATA *` may alias the counter, so gcc re-reads it. An explicit
  `ictd = i_cratetypedata; ... i_cratetypedata = ictd + 1;` keeps it in a
  register (retail's form). `SaveCrateTypeData` as a standalone function does not
  need this.
- **A search flag assigned before its loop burns a callee-saved register.**
  Retail sets the flag only on the two loop exits (1 at the break, 0 after the
  loop). Writing `found = 0;` before the loop keeps it live across the calls
  inside it and costs a seventh saved register plus 16 bytes of frame; the
  `goto` past a post-loop `found = 0;` is what retail's codegen implies.
- **gcc emits `switch` case bodies in source order**, so the `case` order in C is
  the retail *source* order, not numeric. `CrateOff`'s is 2, 7, 3, 0x10, 9, 0xC,
  0xB, 0xA, 0x14, 6/8, default.
- **`.lit4` entries are compiler literals, not globals.** `D_0062D788/78C/790/794`
  (AddCrateExplosion) and `D_0062D728/72C/730` (CrateBounceReaction) are just
  `-0.12f`, `0.035f`, `-0.1f`, `0.02f` written in the source; that is why gcc
  hoists them out of a loop full of calls. SN's `as` does not dedup them, so
  equal values get one slot each. Write the literal, not an `extern`.

Two near-matches kept in the tree, both with the full analysis in their MCP
blockers:

- **AddCrateExplosion — 1064/1072 bytes**, everything exact but two words. LICM
  inserts hoisted invariants *after* the front-end's loop-counter init (proven
  with `-fno-schedule-insns -fno-schedule-insns2`), and the scheduler breaks ties
  on RTL order, so the compiler-generated `movn` division constant can never
  precede `j = 5` in the phase-2 preheader. 18 source spellings, all identical.
  Note the phase-1 outer loop has the *same* ordering problem and *is* steerable,
  because there the hoisted value has a source form: `a = i * 0x4000;` as its own
  statement lands ahead of the inner loop's init.
- **CrateOff — 507 vs 506 words.** Blocked on the `_sonyize` hazard-nop gap
  (retail's `mtc1 $at,$f1 / nop / add.s $f0,$f0,$f1` at 0x001F3600; ee-gcc emits
  no `#nop` there, so the nop is purely Sony's `as`) plus gcc CSE-ing the two
  `(const_double 0.0)` operands of `dpcmp`/`dpsub` into one hoisted pseudo where
  retail rematerialises `$zero` at both sites. Both must be fixed together.
