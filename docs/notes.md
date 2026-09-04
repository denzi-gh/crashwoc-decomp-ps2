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
- **Unfilled branch delay-slot nop (fixed 2026-09-01, `_sonyize_seg`).** A
  branch gcc emits in `.set reorder` mode has an empty delay slot: Sony's `as`
  fills it with a `nop`, the decompals `as` swaps the preceding instruction in.
  The rewrite wraps such a branch in `.set noreorder` and writes the nop out;
  branches gcc filled itself sit inside its own `.set noreorder` block and pass
  through untouched. See "The assembler delay-slot wall" below.
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

## nups2/ps2dma + gamelib/terrain + gamelib/gcutscn, session 2026-09-01 (31 matched)

Three units in parallel, 31 functions byte-exact, no blockers. Agents matched
only; promotion was run sequentially afterwards because `promote.py` re-verifies
the whole matching set per promotion (~2m48s at 508 functions) and concurrent
promotions roll each other back.

### Matching lever: bitfield store keeps the displacement folded

`PlatInstRotate` is `lw 0x4C($v1)` / `and` / `or` / `sw 0x4C($v1)` with a
*single* `addu $v1,$v1,$a0`. The obvious read-modify-write
`p->status = (p->status & ~1) | (rot & 1)` makes gcc CSE the whole address
(`addu $3,$3,76` plus `addu $3,$3,$4`, displacement 0) -- one instruction too
many. A **bitfield store** (`status.bit.rotate = rot`) goes through
`store_bit_field` on a single `MEM`, so the `+4` array base stays folded into the
0x4C displacement and only one address `addu` is emitted.

General rule: **when retail shows the same non-zero displacement on both the `lw`
and the `sw` of a read-modify-write, the source was a bitfield assignment, not an
explicit mask/or.**

Corollaries, both useful as evidence when reading a listing:

- Signedness decides how `~1` is materialised. With `u32`, `& ~1` becomes
  `li $6,-65536; ori $6,$6,0xfffe` (0xFFFFFFFE stays positive in the 64-bit
  HOST_WIDE_INT). In signed/bitfield context it is a single `addiu $r,$zero,-2`.
  **A lone `addiu rX,$zero,-2` before an `and` means a signed or bitfield
  operand.**
- An unsigned 1-bit extract (`p->status.bit.hit`) gives `lw; srl 1; andi 1`;
  a signed `(x >> 1) & 1` gives `sra` -- gcc 2.95 does not narrow
  `ashiftrt`+`and` to `lshiftrt`.

### Matching lever: `!(x & bit)` lowers to sra/xori/andi

`return !(p->flags & 2);` compiles to `lw; sra $v0,$v0,1; xori $v0,$v0,1;
andi $v0,$v0,1` -- the tested bit is shifted to bit 0, inverted, then masked.
Do **not** read a retail `sra k / xori 1 / andi 1` triple as a bitfield or as
`~(x>>k)&1`; the plain readable `!(x & (1<<k))` is the source. `(x & 2) == 0`
and `!((x>>1)&1)` produce the same code; `~(x>>1)&1` does not (it emits `nor`).

### Matching lever: memory-mapped registers are `li`, not %hi/%lo

Retail's DMAC status readers materialise the register address as
`lui $v1,0x1000 / ori $v1,$v1,0x8000 / lw $v0,0($v1)`. That is exactly what
`*(volatile int *)0x10008000` produces: a CONST_INT address is not a legitimate
MIPS address, so gcc `force_reg`s it into one `li`, which the assembler expands
to `lui`+`ori`. Do not reach for an `extern` symbol or a shared struct base
pointer -- retail rebuilds the full address per register (`VU0_DmaWrite`
materialises 0x10008020, 0x10008030, 0x1000E010 and 0x10008000 separately), so
one `#define REG (*(volatile int *)ADDR)` per register is the right source shape.
A getter's `sra` (rather than `srl`) additionally proves the register is declared
`volatile int`: `(chcr >> 8) & 1` only matches signed; `u32` yields `srl`.

### Matching lever: `lq`/`sq` template copy needs `aligned(16)`

`vpDmaTag_Cnt`/`_End` copy a 16-byte DMAtag template out of `.data` with a single
`lq`/`sq` pair plus a separate `addiu $t0` for the address. Plain
`*tag = template;` reproduces that exactly -- including the extra `addiu` and the
register allocation -- **only** if the struct typedef carries
`__attribute__((aligned(16)))`. Without it the type is 4-byte aligned and gcc
emits four `lw`/`sw` pairs. The same recipe should unblock `vpDmaTag_Ref`
(D_002EA560) and `vpDmaTag_EndEx` (D_002EA580); note D_002EA580 sits exactly at
the exclusive end of unit-62's `.data` range 0x2ea460-0x2ea580 in `data_map.toml`,
so that one needs a data-ownership check first.

### Smaller facts

- **`.extern sym, N` drives `%gp_rel`.** Declaring `extern <real 4-byte type>
  sym;` makes gcc emit `.extern sym, 4`, and gas under `-G8` assembles a bare
  `sw $4,sym` into `sw $a0, %gp_rel(sym)($gp)`. A function-pointer typedef is
  enough -- no `D_` alias or definition is needed for a symbol already in
  `symbol_addrs.txt`. This is the same size-driven rule as the nu3d/nucamera
  note above, seen from the emission side.
- **Two-sided bound folds to one `sltiu`.** `if ((id >= 0) && (id < 128))`
  compiles to a single `sltiu $v0,$a0,0x80`; the parameter does not need to be
  made unsigned.
- **Trailing pad nops are free.** gcc's per-function `.align 3` produces retail's
  nop padding, so a 12- or 20-byte body matches a 16- or 24-byte canonical extent
  with no special handling.
- **Probe before spending an attempt.** All three agents pre-validated candidate
  codegen shapes with throwaway `tools/cc.py -S` compiles in a scratch dir, then
  spent one `compile_diff` each. 29 of 31 functions matched on attempt 1.

### Structure facts

- **`TerrainSetType`**: `terr` @0x00, `platdata[128]` (stride 100, a real `mult`)
  @0x04, `wallinfo` @0x3204, `TrackInfo[4]` (stride 0xC) @0x3208. So
  `platdata[i].curmtx` = `CurTerr + i*100 + 0x44`, `.status` = `+0x4C`, whose
  bit 0 = rotate and bit 1 = hit (the `status |= 2` in `NewTerrainScaleY` is the
  same bit). `ScanTerrId`/`AllocTerrId`'s two parallel 0xC-stride pointers are
  just gcc's givs for `CurTerr->TrackInfo[c].ptrid` and `&CurTerr->TrackInfo[c]`;
  a plain `for (c = 0; c < 4; c++)` reproduces both.
- **`DmaTag`** (16 B, 16-aligned): `u16 qwc @0; u8 pad @2; u8 id @3`
  (`vpDmaTag_Set` writes `id << 4` into byte 3 = DMAtag ID bits 30:28);
  `u32 addr @4`; `u32 vifcode[2] @8/@0xC`. Retail templates: `D_002EA550` CNT,
  `D_002EA560` REF, `D_002EA570` END, `D_002EA580` ENDEX. `D_0062F0E0`
  (`.sdata`, gp-relative) is the scratchpad bump pointer -- `NuScratchReset` sets
  it to 0x70000000, the allocators store the previous value as a link word and
  advance past it, `NuScratchRelease` pops via `ptr[-1]`. Channel CHCRs used
  here: 0x10008000 VIF0, 0x10009000 VIF1, 0x1000A000 GIF, 0x1000D000 fromSPR,
  0x1000D400 toSPR. `VifTagDummy` is a dead identity stub -- no `jal` and no data
  reference anywhere in `asm/text.s`.
- **`NuGCutScene`** (0x94 B): `0x00 next`, `0x04 prev`, `0x08 name[16]`,
  `0x18 mtx` (NuMtx, 0x40), `0x58 def`, `0x5C centre` (vec4, transformed in
  place), `0x6C flags`, `0x70 time` (1.0f at Start), `0x74 speed` (1.0f at
  Create), `0x78 camsys`, `0x7C rigidsys`, `0x80 charsys`, `0x84 locatorsys`,
  `0x88 triggersys`, `0x8C chain`, `0x90 endcallback`. Flags: 0x002
  running/started, 0x010 matrix set, 0x100 disabled, 0x200 updated-this-frame.
  `D_0063322C` is the list head. Handler globals with signatures read off their
  `jalr` sites: `NuCutSceneCharacterRender(scene, def, character, desc, float)`
  @0x62F7E0, `NuCutSceneCharacterRelease(character)` @0x62F7E4,
  `NuCutSceneFindCharacters(void)` @0x62F7E8,
  `NuCutSceneCharacterCreateData(desc, character, heap)` @0x62F7EC,
  `NuCutSceneRigidCollisionCheck(rigid, NuMtx*)` @0x62F7F0, and `locatorfns`
  @0x62F7F4 -- a pointer to a name-terminated array of 8-byte
  `{const char *name; void (*func)();}` entries indexed by `sll 3`.

## 100-function parallel session, 2026-09-01 (10 units, 96 matched, 4 blockers)

Ten waves of ten single-function agents across ten disjoint units (nucore/nufile,
nu3d/nustream, nu3d/nucamera, nu3d/nutex, nu3d/nurndr, nu3d/nulight,
nups2/ps2video, nups2/clipping, gamelib/debris, gamelib/edptl, gamelib/edobj).
Agents matched only; promotion ran afterwards in a single serial chain.

### Reading the disassembly: shape rules

- **Guard polarity: ee-gcc emits the INVERTED branch and places the `if`-body
  block LAST.** Retail's `bgez $a0,L / <inline return 0> / L: <body>` comes from
  the *positive* guard `if (i >= 0) { return body; } return 0;`. The intuitive
  `if (i < 0) return 0; return body;` produces the mirror image and stalls around
  30%. Whichever exit retail places inline is the source's trailing `return`.
- **Three handle templates, not interchangeable.** Value accessor:
  `tex--; if (tex >= 0) { return TexList[tex].f; } return 0;`. Void mutator:
  guards on the *original* handle with the decrement inside the body --
  `if (tex > 0) { tex--; TexList[tex].f = v; }` giving `blez` with the `addiu -1`
  in the delay slot. Forwarder: see below.
- **Never write `arr[i - 1].f` when retail shows an explicit `addiu -1`.** gcc
  folds `(i-1)*STRIDE` into `i*STRIDE - STRIDE` and absorbs it into the store
  displacement, costing an instruction. A separate `i--;` statement is a real
  assignment and is never folded. Same trap as `&arr[expr - CONST]`, which folds
  into `%hi/%lo(sym-N)`.
- **`sll rX,16 / sra rX,16` in a tiny setter proves a `short` PARAMETER**, not a
  mask: SN ee-gcc sign-extends an incoming `s16` argument in the prologue even
  when its only use is an `sh` that would truncate anyway. Declaring it `s32`
  deletes both instructions.
- **A 3-instruction leaf whose only real instruction is
  `addiu $vN,$gp,%gp_rel(SYM)` returns the ADDRESS**, not the value.
- **`sltiu $v0,$v0,1` on a call result is plain `!call(...)`** -- no cast, no
  `== 0` spelling.
- **A prolog / `jal` / `nop` / epilog body with NO argument setup is a pure
  forwarding wrapper** whose parameter list equals the callee's; the `nop` is
  expected, not a reorg artefact. The argument-zeroing variant emits one
  `daddu $aN,$zero,$zero` (or `addiu $aN,$zero,<default>`) per defaulted
  argument, the last landing in the `jal` delay slot with no `nop`. EE EABI
  passes arguments in `$a0-$a3, $t0, $t1`, so a wrapper touching `$t0`/`$t1` is
  passing arguments, not scratch.
- **`beqz $a0,L; lw $v0,A / jr $ra; lw $v0,B / L: jr $ra; nop` is plain
  `if (p) return B; return A;`** -- the taken-path load is hoisted into the
  branch delay slot and the target block collapses to a duplicated
  `jr $ra; nop`. No ternary, no `movz`.

### Loop direction has THREE cases

gcc-2.95's `check_dbra_loop` may reverse a whole loop, induction variable
included:

- **descending retail pointer** = a reversed *ascending-from-0* source loop;
- **ascending pointer + down-counter** = an ascending loop whose reversal was
  *blocked* -- by a `jal` in the body **or** by a non-zero start index;
- the order of the two constant setups disambiguates: **`li -1` before
  `li <n-1>` means the counter was synthesized by reversal** (`move_movables`
  inserts the hoisted `li -1` before `loop_start`, `check_dbra_loop` inserts its
  counter init afterwards); the reverse order means the source already counted
  down.

### Addressing: which register the `addu` writes into

For a `%hi/%lo`-based array element with a non-power-of-2 stride, the inline
subscript and the pointer local pick different `addu` operand orders. Mechanism:
MIPS `LEGITIMIZE_ADDRESS` swaps `PLUS(symbol, reg)` so the register comes first,
which only the pointer form reaches.

**Probe both spellings rather than trusting a remembered direction** -- this
session found the mapping stated one way in the existing gamelib/terrain note and
the other way in gamelib/edptl, and both were correct for their own function.
`(arr + i)->f` and `(*(arr + i)).f` behave as the pointer local. The rule does
*not* apply when the base is a pointer LOADED from memory (e.g.
`extern NuTex *TexList`) -- there the plain subscript already gives the right
form.

### Types steer register ties

**`int` parameters plus explicit `(u64)` casts are not cosmetic.** Packing
`x0 | x1<<16 | y0<<32 | y1<<48` with `u64` parameters ties the whole `or` chain
to `$a0`; with `int` parameters and explicit casts it ties each `or` to the
*shifted* operand, which is what retail has. gcc folds the SI-to-DI cast away
(the EE ABI keeps `int` sign-extended) but the extra RTL moves local-alloc's tie.
`unsigned int` emits real zero-extension pairs, `u16` an `andi` per operand, no
cast at all gives 32-bit `sll`. Eight spellings were probed; only one matches.
Field order inside the expression is free -- write the readable ascending form.

### Signature recovery for stubs: three evidence classes

An empty body emits the same two words for ANY signature, so name the parameters
from the CALLERS and say which class you are in:

1. **`jal` sites with argument setup** -- signature inferable (14 sites gave
   `NuRndrLine3dDbg` a 7-parameter signature; one site gave
   `NuRndrShadowOnOff(int)`; three sites gave `NuLightFogClear(int)`).
2. **call sites with no argument setup and no `$v0` consumer** -- positive
   evidence for `void f(void)`.
3. **no callers at all** -- the signature is an explicit *style choice*; say so
   plainly rather than dressing it up as inferred (`NuRndrFogMode`,
   `NuRndrQuad3d`, `NuRndrTest`, `NuLightClose`).

Do not infer the flavour from the name: the PS2 fog trio looked like setters and
were empty stubs, while `NuLightFogClear` was reported as a stub by one agent and
is in fact a one-store setter. Read the second instruction.

### The assembler delay-slot wall (fixed 2026-09-01, `_sonyize_seg`)

Where **gcc leaves a branch delay slot unfilled** (a bare `j $31` or branch in
`.set reorder` mode, with no `.set noreorder` wrapper of its own) SN's `as`
always fills it with the `nop` retail contains, while the decompals `as`
*swaps the preceding instruction in* -- one word short, shifting every function
after it. Four faithful near-matches sat on this:

| function | instruction the decompals `as` swapped in |
|---|---|
| `NuPs2GetRenderWidth` 0x16a8e8 | `cvt.s.w` |
| `timeUserRead` 0x16b2c0 | volatile `lw` |
| `timeUserReset` 0x16b2a0 | volatile `sw` |
| `NuCameraSetAxes` 0x113b18 | plain `sw` |

`_sonyize_seg` now tracks gcc's own `.set noreorder` / `.set reorder` state and,
for a branch emitted in reorder mode, wraps it in `.set noreorder` and writes
the `nop` out. All four are promoted; `ninja check` (verify-loaded +
verify-promoted over all 54 hybrid units, full image SHA) passes unchanged.

**The earlier "do not apply a blanket fix" warning here was wrong**, and is
recorded so it is not re-derived. It rested on counting retail return delay
slots (13 `add.s`, 8 `div.s`, 3 `mul.s`, 133 `swc1`, 4 `lwc1`) and concluding
SN's `as` had moved them there. It had not: **gcc's own delay-slot scheduler**
filled those, and gcc brackets every slot it fills with `.set noreorder` /
`.set nomacro`, so the rewrite passes them through untouched. Retail
`cbPtlChangeSScale` @0x197130 settles it from the other side -- `lwc1` /
`cvt.w.s` / `swc1 $f1,%gp_rel(D_0062F314)($gp)` / `jr $ra` / `nop`: an FP store
gcc did *not* sink, and retail keeps the nop. Related but distinct: a trailing
`sdr` (part of a length-2 unaligned-store pattern) is never swapped by either
assembler, so `NuMtxMul`/`NuMtxSetIdentity` matched even before this.

Left unused: gcc 2.95 brackets every volatile access with `#.set volatile` /
`#.set novolatile`, emitted *commented out* because this build targets gas while
SN's gcc was configured for the MIPS assembler and emitted them live. The
delay-slot rewrite already covers both volatile blockers, so those markers stay
ignored -- reach for them only if a volatile-ordering difference turns up
somewhere the delay-slot rule does not explain.

### Traps and tooling

- **`.extern` of an INVENTED name breaks the whole unit.** Unlike `.comm` it
  emits no bytes, but gas still puts the name in the symtab and `compile_diff`
  reports it under `unresolved_symbols` and refuses to link -- even when no
  from-C function references it. Every file-scope name must be a registered
  symbol, a splat `dlabel` from `asm/data/*.s`, or a `D_<vram>` alias.
- **`.comm` is a promotion landmine that hides until the first promotion.** A
  unit with zero `matching` functions never builds a hybrid, so pre-existing
  file-scope tentative definitions sit undetected; `nucore/nufile` had nine and
  no function in it could ever have been promoted. Sweep `src/**` for bare
  `T name[N];` at file scope before targeting a fresh unit.
- **`verify_candidate --level unit` shells out to the repo-wide
  `verify_promoted.py`** (only `--skip-image` differs) and rewrites the shared
  `build/pal103/verify_results.json`. It races other agents and returns results
  reflecting *other* units' state. Only `--level function` is safe while agents
  run in parallel. A lone `exact: false` with a null byte count should be re-run
  before it is believed.
- **`get_context.facts.gp_relative_references` is unreliable for objects over the
  `-G8` threshold** -- it mislabelled 64-byte `NuMtx` externs as gp-relative six
  times in one unit. The disassembly (rank 2) is authoritative.
- **`tools/cc.py -o foo.s` writes an OBJECT, not assembly.** Use `cc.compile_s`.
  Probe files must live inside the repo (e.g. `build/probe/`) and dispatch takes
  a repo-relative path -- an absolute `/work/...` path gets mangled by Git Bash.
  Putting many candidate spellings in ONE probe TU costs one container
  round-trip; agents that probed first matched on attempt 1 almost without
  exception.
- **`promote.py` re-verifies the entire matching set on every promotion** (~3 min
  at ~550 functions) and **two promotion loops must never run at once** -- this
  session lost about an hour to a duplicated drainer whose concurrent promotions
  rolled each other back while each reported all-PASS. Serial promotion after
  all agents finish is the only reliable order.
- **Line endings are mixed across `src/`** (nurndr/nufile/nucamera/edobj are
  CRLF; ps2video/nutex/debris/edptl/clipping are LF). Check bytes before a
  scripted edit; a blanket normalization rewrites the whole file.

### Retail source bug: `NuTexHeight` returns the width

`NuTexHeight` (0x11e910) and `NuTexWidth` (0x11e8e8) are byte-for-byte identical
apart from labels -- both end `lw $v0, 0x4($v1)`. It is a copy-paste bug in the
original source, matched faithfully. The no-comment rule means `src/nu3d/nutex.c`
carries no hint of this, so **do not "fix" it** -- that breaks the match.

### Structure facts

- **`NuTex`** (0xE0, 1-based handles, gp-relative *pointer* `D_0062EBEC`):
  `type` @0x00, `width` @0x04, u16 refcount / 64-bit flag word @0x18
  (0x10000 in-use, 0x20000 external memory), GS VRAM address @0xCC.
  `D_0062EBF0` allocation cursor, `D_0063305C` slot count, `gs_botfree`
  0x0062ec40, `gs_top` 0x0062ec3c, temp pointer `D_00633068`.
- **`nu3d/nucamera` matrix block is NOT a contiguous 0x40 run**: view A700,
  projection A740, scaling A780, VPC A840, PC A880, VPCS A8C0, PCS A900 -- A7C0
  and A800 belong to something else. Read each function's own `%hi/%lo` pair.
  `D_002D3EB0` = camera axes (`NuVec3`, 12 B), `D_002D3EF0` = translation row
  (`global_camera + 0x30`), `D_00614170` = the TU's `__FILE__` string (pinning
  `NuCameraDestroy` to source line 74).
- **`nu3d/nustream`**: `D_00633020`/`D_00633024` output-buffer bases,
  `D_00633028` write cursor, `D_00633038` packed SCISSOR_1 (SCAX0/SCAX1/SCAY0/
  SCAY1 at bits 0/16/32/48; argument order is `(x0,y0,x1,y1)`, so `a1`/`a2` are
  swapped relative to field order), `D_00633040` packed XY-offset,
  `rndrstream_scissor_id` 0x62EA8C / `rndrstream_xyoffset_id` 0x62EA8E (15-bit
  dirty counters), `D_0062EA74` buffer index, `D_0062EA90` buffer size,
  `D_0062EAC0` ZB-state valid flag, `rndrstream_free` 0x0062EB00 bump cursor.
- **`nucore/nufile`**: `fmode` D_00293700, `forig` D_00293710, `filesys_root`
  D_00293720, `working_dir[256]` D_00293730, `curr_dat` D_0062E998,
  `nufile_deviceid` D_0062E99C, `file_time_count` D_0062E9A0, `blk_level`
  D_0062E980, `blk_stack` D_00633548 (`{int id; int size; int start;}`, 0xC),
  `memfiles[20]` 0x14 D_00639548, `datfiles[20]` 0x1C D_006396D8 (`len` @0x08),
  `file_info[16]` 0x20 D_00639908, `file_buff[4]` **0x10008** D_00639B08.
  Handles: fd-range (<0x400) = OS fd + 1, memfiles = `fh - 0x400`, datfiles =
  `fh - 0x800`. `NuDatOpenEx(char*, char**, int*, short)`. **Still open**: the
  `struct filebuff_s` typedef says `char data[4096]` but the stride proves
  0x10000.
- **`gamelib/debris`**: `debkeydata` @0x00320eb0 stride **0x56C** -- `s16 on`
  @0x484, `s16 limit` @0x486, `s16 index` @0x48A, `s32 trigger` @0x548,
  `float bounceoffset` @0x558, `s16 groupid` @0x568. `struct debpart_s` (32 B):
  `pos` @0x00, `time` @0x0C, `mom` @0x10, `rate` @0x1C. `gencodetab` @0x00320E70
  = momentum-adjust callbacks `void (*)(deb_s*, debinfo_s*, debpart_s*)`,
  `[1]=FromPos [2]=FromPosRev [3]=FromSplash [4]=FromAshRock [5]=FromPosRevTree
  [6]=FromPosAll`; `gensorttab` @0x00320E48 = the `GenDebIndex*` generators.
  `DebrisOff`/`DebrisOn` take `s32 *key`; `DebrisSetTrigger`/`SetGroupID` take
  the key directly. **`src/game/ai.c`, `src/gamelib/debris.c` and
  `src/gamelib/edptl.c` each declare a partial view of this same record -- worth
  unifying.**
- **`gamelib/edptl` / `gamelib/edobj` share the editor menu-item struct**:
  `int value` @0x0C, `unsigned char toggle` @0x10, `float fvalue` @0x4C, with the
  callback ABI `(void *menu, struct <unit>item_s *item)`. `int_global =
  item->fvalue;` gives `lwc1`/`cvt.w.s`/`swc1` with **no** `mfc1` (that appears
  only when the destination is narrower than 32 bits). Not every callback has the
  `edobj_nearest != -1` guard -- adding one emits a branch that is not there, and
  `frame_size == 0` / `reg_mask == 0` proves there is no trailing call.
  `ObjectPath` @0x0048cb00 stride 0x3EC: `+0x04` anim speed (float), `+0x10` anim
  pause (float), `+0x24` anim start offset (int), `+0x12C` switch type,
  `+0x130` switch id (int), `+0x134` switch var (float), `+0x138` switch delay
  (float), `+0x320 int sound_id[8]`, `+0x340 int sound_type[8]`,
  `+0x360 float sound_timing[8]`, `+0x380` float[3] positions stride 0xC,
  emitter count `+0x20`. `edpp_ptls` @0x00484f18 256 x 0x4C: position @0x00,
  debris handle @0x10 (-1 = free), `float bounceoffset` @0x40. Editor state block
  `.sdata` 0x0062F624..0x0062F650 (nine ints, `-1` = nothing selected).
- **`nups2/clipping`**: `clipflags` 0x0062F1EC; outcodes RIGHT 0x01, LEFT 0x02,
  TOP 0x04, BOTTOM 0x08, ZFAR 0x10, ZNEAR 0x20; `ClipPolygon` does
  `clipflags <<= 6` per vertex, so `TestPrev*` reads the same bits shifted by 6.
  All ten `TestCurr*`/`TestPrev*` are now matched.
- **`nups2/ps2video`**: EE timers T0_MODE 0x10000010 (`0x82` = clock/256 +
  start), T0_COUNT 0x10000000, T1_COUNT 0x10000800 (swap timing). GS frame
  packets are GIF A+D pairs: FRAME_1 at `D_002EA190`/`D_002EA300`, ZBUF_1 at
  +0x10 (`D_002EA1A0`/`D_002EA310`); parity polarity differs per accessor and
  must be read, not assumed. `ifnVif1` is the VIF1 INTC handler and contains the
  **first inline asm in `src/`** -- SCE's `ExitHandler()` as
  `__asm__ volatile ("sync\n\tei")`, which needs no special hybrid handling.

### Parallel session 2026-09-04: edptl / edobj / nutex / debris (52 matched, 1 blocker)

4 agents x 2 waves, one unit each; batch-promoted 52 (edptl +15, edobj +14,
debris +12, nutex +11), full image re-verified byte-identical. All four files are
**CRLF** now -- the LF note at line ~546 is stale for edptl/nutex/debris.

- **Cancel-stub store order (edptl):** a `eduiMenuDestroy(menu); menu = 0;` cancel
  stub that nulls TWO gp-relative globals keeps them in **source order** (`a=0; b=0`
  -> `sw a; sw b`); the "pairs reverse" rule did not apply. The null is not
  guaranteed -- `edppClose` is two `eduiMenuDestroy` calls and nulls nothing (read
  the second-to-last instruction); back-to-back calls with no arg setup reproduce
  retail's delay-slot arg scheduling from plain source-order calls.
- **Secondary-array pointer local flips the biv (edobj):** in a two-parallel-array
  loop with a $v0/$v1 induction-pointer tie, keep the loop-controlling array a direct
  global subscript and give the SECONDARY array a pointer local (`oi = ObjectInstance;
  oi[i].id`). Same family as "explicit temp flips regalloc".
- **`edobject_s` (stride 0x3EC) additions:** `+0x00` int instance id (-1=unused),
  `+0x08` int oscillate (from item->toggle via lbu+sw), `+0x1DC int particle_rate[8]`,
  `+0x1FC int particle_switch[8]` (both indexed by edobj_nearest_particle),
  `+0x3E0/+0x3E4/+0x3E8` float bouncy player_grav/tension/damping. `edobjitem_s`
  gains `unsigned char toggle` @0x10. Guard is `if (edobj_nearest != -1) { ... }`;
  the field store schedules into the `jal edobjConvertPathToAnim` delay slot on its
  own. gp globals: edobj_nearest, edobj_nearest_particle 0x62f62c,
  edobj_next_instance 0x62f620, menu ptrs 0x62f554/55c/564/570.
- **Alignment-mask materialization (nutex, broadly reusable):** `x & ~31` makes gcc
  load `-32` with a single `addiu`; retail uses `lui/ori`. Spelling the mask as the
  unsigned literal **`& 0xFFFFFFE0`** (or `~31u`) forces `lui 0xFFFF; ori 0xFFE0` and
  can cascade the whole regalloc into a match (NuTexAssignTempAddr).
- **Linear compare-chain dispatch (nutex):** a `switch` over contiguous small cases
  balances into an `slt` range split; an if-chain if-converts the tail into
  `xori/movn`. To reproduce retail's linear `beq` chain with separate return blocks
  (default LAST, reached by an explicit `b`), use explicit `goto` labels with
  `return 0` placed last (NuTexAlphaBits).
- **`NuTex` additions:** union `{ u16 refcount; s64 flags; }` @0x18 (0x10000 in-use,
  0x20000 external), `int reqsize[]` @0x50 (value <<8), `u16 texaddr` @0x88
  (0 -> gs_botfree), `int gsaddr` @0xCC, palette region @0x20. `long` is 64-bit
  here (emits `ld`); anonymous unions compile in SN ee-gcc 2.95.
- **TexList[tex-1] element-offset fold:** a SINGLE access of `TexList[tex-1].f` folds
  `(tex-1)*0xE0 + off` into one displacement (gcc keeps undecremented `tex` in the
  multiply). A DOUBLE access of the same element CSEs `&TexList[tex-1]` and emits an
  extra `addu` no C spelling avoids. **BLOCKER:** `NuTexGetTextAddr` (0x11eff8) reads
  @0x18 and @0x88 off one element, +1 word past the 56B extent, left state=asm.
- **Pointer-to-pointer list walk (debris, verified twice):** FIND reuses the
  clobbered walker for the guard read (`list=&arr[i]; if(*list){do{cur=*list;
  list=&cur->next;}while(*list);}`) so gcc recomputes `base+i*4` while a separate
  strength-reduced pointer serves the return `&arr[i]`. REMOVE = peeled head check +
  `do{cur=*list; list=&cur->next; if(cur->next==0)break;
  if(cur->next==node){cur->next=node->next;break;}}while(1); node->next=0;` with the
  `list=&cur->next` advance BEFORE the two ifs (fills the beqz/bnel delay slots);
  retargeting the same source to `chunkctrl_s` (next@0x10) matched with no changes.
  `deb_s`: emitpos@0x490, next@0x4B0, reflect_a/b s16 @0x554/0x556, reflect_x/y f32
  @0x558/0x55C.
- **Address-of loop var (debris):** a loop passing `&local` to a callee keeps a
  register counter and a SEPARATE stack `key`, assigning `key = i; DebFree(&key);`
  only at the call site; plain `bne` (advance in delay) is right, an explicit `p++`
  gives a wrong `bnel`.

### Parallel session 2026-09-04 (b): edptl / edobj / nurndr / gcutscn (51 matched, 1 divergence)

4 agents x 2 waves, one unit each, small-first. All 52 attempts matched at
`--level function` on attempt 1 (every agent probed spellings in one TU first),
but the batch `promote.py` (full gap-to-next extent) rolled back on ONE:
`edobjPlayerObjectDistance` (0x19f338) diverges over its full extent though the
function-level diff was exact -- the verify gap the template warns about. Re-run
of the other 51 promoted clean; full image re-verified byte-identical. Net
+51: edptl +13, edobj +12 (13 attempted, 1 held back), nurndr +13, gcutscn +13.
`edobjPlayerObjectDistance` left state=asm as a faithful near-match; the trailing
region past the function body is what disagrees -- investigate the gap-to-next
extent before re-attempting (it is an FP distance leaf: `if (edmainQueryLocVec()
== 0) return 0.0f; return NuVecDist(&ObjectInstance[obj].pos,
edmainQueryLocVec(), 0);`, `return 0.0f` = `mtc1 $zero,$f0`).

**Cancel/menu stubs (edptl):** the wave-1 "multi-null keeps SOURCE order" finding
extends to 3 and 4 nulled globals (`cbPtlCancelColMenu` nulls 3, `cbPtlCancelRotMenu`
nulls 4, all source order after `eduiMenuDestroy` -- the adjacent-store
reverse/rotate rule does NOT fire for a post-destroy null run). `edptlcbEmptyClipboard`
is a save/restore around a global-input callee: `saved = g; g = clip; cb(); g = saved;
clip = -1;` -- `saved` naturally lands in `$s0` (live across the call), the
`g = clip` store fills the `jal` delay slot. `cbChangeFileName` proved
`edptlitem_s` has a `char text[8]` @0x44 (between toggle@0x10 and fvalue@0x4C).

**edobj struct + levers:** `edobject_s` (stride 0x3EC) per-waypoint layout is
position `waypoints[8]`@0x2C, `waypoint_speed[8]`@0x8C, `waypoint_rot[8]`(nuvec)@0xAC;
`switch_type`(int)@0x12C just before switch_id@0x130; `edobjinstance_s` (stride 0x50)
`pos`(nuvec)@0x30, id@0x40. A gp-global leaf called twice is NOT CSE'd -- write the
call twice (`edmainQueryLocVec()` for the null-check and again for the arg).
if/else vs ternary store: `type=item->value; if(type==0)type=-1; g=type;` if-converts
to `movz`; the branchy `g=...; if(sentinel)g=-1; else g=item->value;` keeps `bne`+`b`
and TAIL-MERGES the two stores into one `sw` ONLY when the compared reg is also the
else-value (SetParticleType, sentinel 0); when the sentinel is a distinct constant
occupying the compare reg (SetSoundType, 99999) the two stores stay separate.
Byte-field-into-int-temp (`on = item->toggle; g = on; if(on)...`) reads the byte once
and fills the `beqz` delay slot with the `sw` (SnapY/SnapXZ); reading `item->toggle`
twice re-reads because the global store aliases it.

**nurndr:** a 12-byte `NuVec3 {float x,y,z}` value copy from a param pointer emits
`ldl/ldr`(0-7)+`lw`(8-11) (and the `sdl/sdr`+`sw` mirror) with NO aligned attribute --
retail's shape (`NuRndrShadowDirCol` copies `*dir` into `curlight`@0x2e4e48). To
reproduce a global RE-READ after a call, snapshot before and `+=` after:
`n = g; g += f(...,n,...);` (`NuRndrSplineSearch`: `NuRndrShadMaskCount += NuSplineFindAllSub(spline, D_0062ECA8, &NuRndrShadMasks[n], 32 - n)`). The
`_sonyize_seg` reorder-branch fix also covers a `.set reorder` conditional branch
(`beqz/nop`) sitting between two if-gated calls, not just bare `jal`s
(`NuRndrEndSceneEx`/`NuRndrFlush`, which are byte-identical twins `void f(int flags)`).
Running-pointer DMA-tag fill `p = vpDmaTag_RetEx(...); *p++ = 0x60010063; *p = 0;
vpDmaTag_Close(p+1);` matches; a low-half-zero `li` collapses to a bare `lui`.
`NuRndrStarfield` is a NuError "not implemented" stub (same call-through-returned-fnptr
as `nulist.c::NuListCheck`). New gp/data: ot_tri 0x2dcdd8 (memset 0x4000),
ot_highest gp@0x62ed00, curlight NuVec3 0x2e4e48, curlightcol gp@0x62ed18,
NRFanData gp@0x62ecdc, NuRndrShadMasks 0x2d6798 (int, stride 4); `numtl_s` embeds a
`NuRndrStream` @0x124, `numtl_defaultmtl3d` gp-ptr@0x62ebe4.

**gcutscn:** the matrix-op scene wrappers share one shape --
`scene->flags |= 0x10; NuMtx<op>(&scene->mtx, arg); instNuGCutSceneCalculateCentre(scene, &scene->mtx);` -- and a trailing `float` arg (angle) rides `$f12` forwarded to the
matrix op with no extra move (`RotateY` = `SetPos` with the op swapped). Two adjacent
`flags &= ~A; flags &= ~B;` do NOT fold: gcc keeps two `and`s with the masks
materialized separately (`addiu -N`) and elides the intermediate store, so
`lw; and; and; sw` with two distinct `addiu -N` = two source statements (a single
`&= ~(A|B)` folds to one `and`). `instNuGCutSceneFind` is a `for(s=list; s;
s=s->next) if(strcasecmp(name,s->name)==0) return s;` -- head read once via `%gp_rel`
into a saved reg, `bnel` with `s=s->next` in the delay slot. Post-increment
array-append (`instNuGCutSceneAddCamTgt`): `if(sys && sys->count < sys->max){
E *e = &sys->targets[sys->count++]; ...; return 1;} return 0;` -- the `u8 count` is
read once into a temp, `andi 0xFF` gives the zero-extended value feeding BOTH the
`sltu` bound and the `sll` index while the RAW byte feeds the `+1` store; caching
count in a local instead removes the `andi` and relocates the pointer arg, so use the
field directly with `count++`. Struct: `NuGCutSceneDef` rigiddef@0x14 / triggerdef@0x24
(deref-then-`[0]`=count); `NuGCutCamSys` targets(NuGCutCamTgt[])@0x0, cams(4B[])@0x4,
bytes@0x8/9/A reset at start, max(u8)@0xB, count(u8)@0xC; `NuGCutCamTgt` (16B)
ptr@0/f32@4/f32@8/char@0xC; scene flag 0x200=updated-this-frame, 0x001 a low state bit
cleared with RUNNING(0x2) on reset.

## Parallel session 2026-09-04 (c): edptl / vehicle / nustream / main / nufile / jeep (71 matched, 6 blockers)

6 agents x 2 waves, one warm unit each (same agent re-owned its unit across
waves so wave 2 inherited the warmed file), small-first. 77 attempted, **71
byte-exact**, batch-promoted in one serial `promote.py --targets-file` run, full
loaded image re-verified byte-identical (`ninja check`). Per unit: edptl +13/14,
vehicle +13/14, nufile +12/12, main +12/13, jeep +11/11, nustream +10/13. Nearly
every match landed on attempt 1 after probing candidate spellings in one scratch
TU first.

### New reusable levers

- **Dead local *aggregate* copy from a global survives GCC 2.95 dead-store
  elimination and compiles to a real block-move; dead scalar locals don't.**
  `minigamedata_s save = D_0061B4B0;` (3xu32, **align 4**) reproduces retail's
  `ldl/ldr+lw` / `sdl/sdr+sw` split; align-1 gives the wrong `lwl/lwr` tail.
  Corollary: a 16-byte extent that is real code (not padding) can need a dead
  `char buf[16];` local purely to emit the `addiu sp,-0x10 / jr ra / addiu sp,0x10`
  frame (`MiniGameRenderScore`); a truly empty body gives the 8-byte `jr/nop`.
- **Bare scalar `extern s32 D_X;` is `-G8`-eligible and emits
  `%gp_rel`/`R_MIPS_GPREL16`; for a symbol far from `$gp` the *linker* fails with
  `relocation truncated to fit` -- NOT a compile error, so `compile_diff`'s
  `compiler_ok` passes and only the link step (or `--level function`) reveals it.**
  Retail addresses these with plain `lui/lw`. Fix: declare `extern s32 NAME[];`
  and reference `NAME[0]`. Generalizes the existing "`extern T NAME[]` for far
  floats/doubles" note to *any* scalar type, with a distinct symptom (ld
  relocation-truncated) from the compile-time GPREL16-overflow case.
- **A bare global reused across an intervening call needs a local to keep its
  `$sN` save/frame (required, not stylistic).** Passing `D_0061B4E0` directly into
  `NuStrCpy(load_txt, D_0061B4E0)` after `NuDisableVBlank()` lets gcc recompute the
  address post-call, dropping the `$s0` save and shrinking the frame 32->16;
  `char *msg = D_0061B4E0;` before the call restores retail's save
  (`BGLoadLevel`/`LoadScreen`/`BGLoadSfxFn`).
- **A struct must be declared >8 bytes to force `lui`/`%hi` over `%gp_rel`, even
  if only two fields are read.** `MGCheckRng`'s `mg_player_actpos` reads 2 floats
  but retail uses `lui/addiu`; model it as a 12-byte vec3 (unread tail) so it
  crosses the `-G8` threshold.
- **Retail `slt` (not `sltu`) on a loop bound means an index-typed source loop,
  even when the compiled shape looks pointer-based.** gcc always lowers pointer
  `LT_EXPR` to unsigned; a signed `for (i=0;i<N;i++)` keeps the same
  strength-reduced pointer-vs-limit `bnel` shape but stays signed
  (`InitCreatureModels`).
- **A pointer-bounded `for (p=arr; p<&arr[N]; p++)` over a constant `N` can emit an
  unwanted upfront `sltu`+`beq` guard** (gcc can't prove the range is non-empty),
  overrunning the extent (`.org backwards`). The index-subscript form
  `for (i=0;i<N;i++)` lets gcc prove `N>0` and drops the guard, matching retail's
  unconditional-first-iteration do-while shape (`CountAsteroids`).
- **`beql`/`bnel` (branch-likely) vs a plain branch tracks the loop-*test*
  instruction class, not trip count.** Accumulate loops (always complete) use
  `beql`/`bnel` when the continuation test is a pointer-vs-end `slt`/`sltu`, but a
  plain `bgez` when it is a downcounter; search loops with an early `return` inside
  the body use plain `bne`/`beqz` regardless of trip count.
- **A multi-field struct-fill with duplicate-valued fields does not emit in C
  declaration order** -- gcc treats the fungible same-value stores as reorderable
  and resolves them by whole-unit register pressure. Treat a probe's field order as
  a hypothesis only; the real fix loop is `compile_diff` + manually decoding the
  mismatched words (rs/rt/rd/imm) to see which fields swapped, not further isolated
  probing (`NuMemFileOpen`, 5-field init, 3 iterations).
- **Register-*count* lever:** a cheap independent computation (`&global_string`)
  gets a callee-saved register only if its live range is forced -- assigning it to a
  local *before* the `if` (not inline in the true branch) gave `NuFileSetCurrentDirectory`
  retail's 2-saved (`s0`+`s1`) prologue instead of 1. Same family as "explicit temp
  flips regalloc", but for register count.
- **Early `v0 = 1` return materialization is easy to miss.** All four rndrstream
  setters (`SetAmbientLight`, `SetDirectionalLights`, `SetDirectionalLights4`,
  `SetFxMtx`) compute `addiu v0,zero,1` as their *second* instruction and
  `return 1` unconditionally; missing it stole `$v0` from the destination-pointer
  role and cascaded a whole-body tie mismatch (`SetFxMtx` scored 2.6%). Check for
  it before assuming a leaf "setter" is `void`.
- **Two textually-identical sibling shapes can need OPPOSITE regalloc idioms.**
  `edptlcbApplyBounceFactor` matched with `deb = &debkeydata[..]; deb->field = x;`
  but `edptlcbSetSwitchVar` needed the *direct* double-index
  `debkeydata[..].field = x;` (no intermediate pointer) to flip a v0/v1 tie -- never
  assume a sibling's working shape transfers.
- **Single-bit test canonicalization:** `(x & POW2) != 0` with a compile-time single
  bit gets canonicalized to `sra N; andi 1`; retail sometimes has `andi MASK;
  sltu $0,v`. Assigning the mask to a local first (`unsigned int mask = 0x800;
  return (x & mask) != 0;`) defeats the canonicalization.
- **Negated-ternary polarity:** for a shared-tail 3-way dispatch where two arms share
  code, `(cond != X) ? A : B` (negated, operands swapped) reproduces retail's
  non-inverted `beq`-to-target shape where `switch`/if-else-if and `(cond == X) ? B :
  A` both invert (`NuRndrStreamFxMPG`; standing recipe).
- **A zero-init accumulator compiles to `add.s`, not `mov.s`.** `fade = 0.0f;
  step = 1.0f/(count+1); fade += step;` gives `mtc1 $zero` early / `div.s` late /
  `add.s` in the branch delay slot; a plain `fade = step;` gives `mov.s` and swaps
  which register holds `step` vs `fade` (`NewFadeOutLastTrail`).
- **3-element trailing-field write:** for `col[0..2].a`, ascending C order does not
  match; write `col[2]` (from a distinct constant) first, then `col[1]`, `col[0]` --
  the "pair reverses" rule applied to the two order-free zero-writes, composing across
  3 elements.
- **Padding-promoted-to-field:** when a second function over the same struct proves a
  byte range you modeled as padding is a real, separately-managed field (here
  `nurndrstream_s.unk_0x14`, zeroed by the *array* reset but not the *single*),
  convert the pad to a named field -- evidence composes across functions in a unit.
- **`arr[idx].f` (index local) and `p = &arr[i]; p->f` (pointer local) pick opposite
  `addu` operand orders** -- third data point (`trail[idx].x` -> `addu v1,v0,a0`
  matches; pointer-local -> `addu v1,a0,v0` wrong). Reindexing via the subscript form
  still CSE'd one computed address.
- **Cancel/menu stubs keep source order for null-runs; the adjacent-store
  reverse/rotate rule fires ONLY for stores at adjacent addresses.** `cbPtlSelType`
  writes three *non-adjacent* globals in plain source order; `cbPtlCancelTextureMenu`
  mixes a conditional single-null with a following unconditional single-null.
- **Guard-/tail-call polarity is per-function -- probe every one.** `nucore/nufile`
  showed four distinct shapes in one unit (flattened early-return, wrapped body,
  single `||`, and a nested `if(a){if(b){...}}`); the "guard polarity" heuristic does
  not always predict which. Faithful reconstructions of retail quirks: `NuFileAppendPath`'s
  `if(*p++!=sep){*p++=sep;}` (post-increment always runs; retail compares against the
  NUL so the store always fires), and `NuFileWriteStringV`'s `sceWrite(fh-1, buf,
  strlen(fmt))` -- `strlen` of the *format* string, not the formatted buffer, with
  `len = strlen(fmt);` as its own statement to schedule `fh-1` late.

### New / confirmed blocker classes (proven, left state=asm)

- **`jal` delay-slot nop-vs-fill (the INVERSE of the `_sonyize` wall).** gcc fills
  the `jal NuGScnRead` delay slot with an independent store where retail keeps an
  explicit `nop` (`LoadFont3D`, 7 orderings). `_sonyize` cannot help -- it adds a nop
  where gcc left the slot empty, not the reverse. The same statements match when
  *inlined* into a larger function (`BGLoadSfxFn`), so it is isolation-dependent.
- **Tail-only global vs loop-setup global competing for a branch delay slot.** A
  `blez`-gated loop needs one extern array's address immediately while an unconditional
  tail store needs a second array's address; retail fills the loop-guard's slot with
  the loop one, ee-gcc prefers the tail one (or computes it two instructions early).
  12 variants, no phrasing controls it (`NuRndrStreamResetStreams`, 59.4%).
- **Pure integer load-vs-store scheduling for back-to-back calls.** An independent
  load competing with a preceding global store as delay-slot fillers for two opaque
  calls: the decompals build hoists the load earliest, retail leaves it as the second
  call's own delay slot (`NuRndrStreamPrependMPG`, 92.3%). Distinct from the
  fp-hazard-nop / delay-slot-reorg-fill classes -- no float, no branch.
- **Commutative `addu` operand order is not source-controllable.** For a
  `count += (a > b)` accumulate, gcc canonicalizes the `addu` operand order the same
  way for every spelling (7 tried); when retail's order differs it is structural
  (`GetCurrentFireFlyObjectives`, 94.4%).
- **Register $s0/$s1 tie flip.** ee-gcc ties a signed-char param to `$s0` and a struct
  pointer to `$s1`; retail ties them opposite, costing one `move` that shifts the tail
  (`NuRndrStreamAddTex`, 9 spellings).
- **List-scheduler prologue-adjust + post-call delay-slot fill** differ from retail
  with no source control (`edppProc`, 4 orderings).

### Struct & symbol facts

- **gamelib/edptl:** `edptlitem_s` {value@0x0C, toggle@0x10, char text[8]@0x44,
  fvalue@0x4C}; `edptlconfirmmenu_s` {callback(void(*)(void*,int))@0x24, detacharg@0x30}
  (passed as the callback's sole param, not the `(void*menu, item*)` pair);
  `edppptl_s` {debris@0x10 (-1=free), switchvar(f)@0x38, bounceoffset(f)@0x40,
  bouncefactor(f)@0x44}; `debkey_s` {debtype(short)@0x482, switchvar(f)@0x550,
  bounceoffset(f)@0x558, bouncefactor(f)@0x55C}; `debtypedef_s` = `debtab[]` pointee
  {drawcutoff(f)@0x30, emitvel(f)@0x34}; `pad_s` {buttons(u32)@0x564}. `DebFreeInstantly`
  takes `s32 *key`. `edppRegisterLevel` is a two-field setter (NuStrCpy/else-clear +
  id store), NOT an ObjectPath array-append (inherited hint was wrong).
- **game/vehicle:** `wbbolt_s` {active@0x00, type(s32)@0x24, stride 0x30};
  `asteroid_s` {active@0x00, radius(f)@0x2C, stride 0x4C, `AsteroidList` = 0x1DB0
  bytes}; `NEWBUGGY` {ball_pos@0x20C, atlas_rotparam@0x26C}; `struct MYDRAW` is
  TU-local to vehsupp.c -- a bare opaque `struct MYDRAW;` forward decl suffices for a
  pointer-only param. `FindTrailAng` == jeep.c `NewFindTrailAng`
  (`NuVecSub(&d,b,a); return (u16)(NuAtan2D(d.x,d.z)-0x2000);`).
- **nu3d/nustream:** `nurndrstream_s` stride 0x38 {cur@0x00, tex@0x04, unk_0x14 (real
  field, zeroed by ResetStreams only), zbuf@0x10, mpgtag@0x18, short -2 sentinels
  @0x24/0x26/0x28, unk_0x30 (int bitmask, `&4` tested)}. `rndrstream_rt_light_buffer`:
  SoA `dirx/diry/dirz[3]`@0x10/0x20/0x30, `col[3]` nucolour4_s@0x40/0x50/0x60, ambient
  rgba@0x70. `D_00679B40[]` = stream pool (count `D_00633028`); `D_0067A590`+0x10 = NuMtx.
  `NuRndrSwapStreamBuffers` toggles buffer-index `D_0062EA74` (signed %2) + forwards to
  `NuRndrInitStreams` (does not swap the bases directly). All four `*light*`/`*fx*`
  setters `return 1`.
- **nucore/nufile:** `NuMemFileOpen(char*start,int size,int mode)` fills a free
  `memfiles[]` slot (stride 0x14): end=start+size-1, curr=start, used=1, returns i+0x400;
  fails (0) on size<=0 or mode!=0. New symbols `D_0062E9B8` (char[] suffix via NuStrCat),
  `D_0062E9C0/C8/CC/D0`, `NuMemFreeFn(void*,char*,int)`.
- **game/jeep:** `fireboss_s` {HitPoints(s32)@0x408, Pos(nuvec_s)@0x418} (proven 3 ways).
  `AddVariableShotDebrisEffect(s32, nuvec_s*, s32, u16, u16)` (real def fwd-declared
  creature.c:1485). `FireBossActionName` = switch over `D_005C162C[0]`, 6 dense cases +
  default (jtbl_0061FC70); `char *(void)` signature is a style choice (no C callers yet).
