# Working notes

Loose ends, known artifacts, and decisions to revisit. Not a task tracker —
just things that came up while building and shouldn't be lost.

## Open — revisit later

### DrawCredits (game/credits, 0x00260E70) — WIP C in tree, fuzzy 42.9%

Session `s-20260709-101722-1e5028` (2026-07-09). Faithful C is in
`src/game/credits.c` (state stays `asm`; sibling **InitCredits** in the same
unit is byte-exact / `matching` / promoted). The reconstruction is
behaviourally complete and its **register allocation matches retail exactly**
(`f20/f21/f22/f23/f24 = xscale/size/y/-1.5f/100.0f`, `s0..s4` as retail;
frame 144, reg_mask/freg_mask exact).

- **The only divergence is one delay-slot `nop`.** At `0xA8` retail leaves the
  first `bc1f` (the `size > -1.5f` guard) delay slot empty and loads
  `D_0062E96C` (the `< 1.7f` cutoff) *after* it; ee-gcc's reorg instead fills
  that slot with the dead-when-taken `lwc1 D_0062E96C` (`0xc7809ffc`). That one
  extra instruction shifts the entire tail by a word, so the byte diff cascades
  to 42.9 % even though everything else is identical.
- The `mtc1 v0,f0 → nop → cvt.s.w` hazard `nop` (the `(float)short` convert) is
  **re-inserted by the decompals `as`** in the hybrid — raw `ee-gcc -S` output
  omits it, so ignore that when comparing raw compiler asm; only the `bc1f`
  slot is real.
- Retail was built with the *same* SN ee-gcc, so a source-level idiom must exist
  that makes reorg decline the hoist, but none of these found it: `&&` vs nested
  `if`s (both 42.9 %, identical bytes); a loop-top `float hi = D_0062E96C;` temp
  (80.6 % but hoists the load into `f1`, wrong reg, still fills the slot with a
  hoisted `D_0062E974`); an inner temp after the `-1.5f` check (gcc folds it
  away → 42.9 %); hoisting the `xscale*step` product to a `step` local
  (38.8 %, and it adds a 6th saved FP reg). gcc fills that slot with *any*
  anticipatable dead load; retail simply declined every candidate. Same class
  of delay-slot reorg artifact as the DrawCreatures near-match. Blocker
  `no_progress` recorded; candidate for `equivalent` review.

### MovePlayer (game/creature, 0x001CE3E0) — WIP C in tree, fuzzy 15.7%

Session `s-20260708-203123-c057fc` (2026-07-08, resumable). Full C
reconstruction from the PS2 disassembly is in `src/game/creature.c` (state
stays `asm`; all byte gates green with it). It compiles clean, the candidate
hybrid links, and the full-extent diff already has **frame size 368 == 368
and reg_mask 0xC0FF0000 exact**; 2408/15376 bytes match (fuzzy 15.66).

- **gen_hybrid multi-jump-table support landed (was the A.1 caveat):**
  `Lit4Mapper.map_jump_table` → `map_jump_tables(slice_text, counts, ctx)` —
  positional borrow, compiled appearance order onto slice appearance order
  (same argument as duplicate .lit4 slots); `_rewrite_local_data` orders
  labels by first use and allocates all tables in one call. Validated on
  MovePlayer's two switches: PlatformCrush dispatch (6 entries, cases 6–11)
  → `jtbl_0061D2C0`, surface_type effects (13 entries, cases 1..13) →
  `jtbl_0061D2E0`. Count mismatch (C switch shape vs retail) fails loudly.
  247 unit tests pass, `ninja check` image SHA exact after the change.
- **PAL deltas vs the GC reference** (all verified in disasm): frame dt
  `0.02f` not 1/60; `GameTimer.frame < 0x32` gate; `in_finish_range == 0x32`;
  submerged cap 0x32; `allow_jump = 0xA`; `tap = 0x19`; NewBuzz arg 0xA;
  hdg snap step `0x369` not 0x2D8; gyro intro `< 0x96`; loadsave frames
  0x32/0x33 and `NewMenu(&Cursor,0x13,3,-1)`; menu ids 0x1F/0x21/0x10/0xE
  (GC 0x22/0x24/0x12/0x10); sprint pad mask 0x28 (GC 0x88); friction base
  0.0072 (GC 0.005), submarine 0.00144/0.00072, slide 0.0216, spin-air
  0.0024, jump ½ / ¼ multipliers; swim sink −0.005; UpdateAnimPacket dt
  0.59999996f (0x3F199999, round-toward-zero 0.6 — write the exact decimal);
  tumble anim window uses `speed * 0.59999996f`; NewTerrainScaleY adjust
  0.0036; water ripple rand scale 1.5258789289873675e-06f. `gotlist[9]` =
  {8,7,0x10,0x20,0x40,0x80,0x100,0x200,0x400} — local initializer borrows
  retail `D_0061D290` via map_owned_data (worked first try).
- Deadzone/fabs comparisons go through soft-double (`fptodp/dpcmp/dpsub`):
  written as `double d = f; if (d < 0.0) d = 0.0 - d;` then compare against
  `42.5` (D_0061D2A8) / `0.33333334f` promoted (D_0061D2B0/B8). Bit patterns
  confirmed equal.
- `UpdateRumble`/`NewRumble`/`NewBuzz` + `TerrainFailsafe` are now GNU89
  `inline` defined before MovePlayer (retail inlines all of them; zero
  standalone `jal`s inside the unit). Their standalone retail addresses
  (0x1D5648, 0x1D5880, 0x1D58A8, 0x1D5900) interleave with non-inline tail
  functions — deferred-emission order needs checking when the unit tail is
  attacked.
- Struct facts added: `pad_s.paddata` 0x55C (held), `.buttons` 0x564
  (debounced), `l_alg_x/y` 0x57E/0x57F; `game_s.vibration` +0x9, `.mask`
  +0x3FE; `gamecam_s.yrot` +0xF8; mech/bazooka fire locator =
  `mtxLOCATOR[1][0]._30` (0x6A4), sight = `[1][1]._30` (0x6E4); the
  `objtab_s`/`nuspecial_s`/`tersurface_s` defs were deduped and moved above
  MovePlayer (single definitions now, DrawCreatures reuses them).
- **Next session:** iterate register allocation. The dominant lever is that
  retail keeps `c` in **$s4** (allocno priority 5) while the candidate keeps
  `c` in **$s3** (priority 4); this single difference is the `...83...`→
  `...63...` base-register swap that dominates the whole-function diff
  (register_allocation window [12, 15148]). Retail's callee-saved roles
  (2026-07-08): **$s1 = oldvc** (saved VEHICLECONTROL), **$s4 = c**,
  **$s5 = veh**, **$s7 = minfo**.
- **obj-pointer hypothesis DISPROVEN (attempt 5, 2026-07-08).** Tried the
  `struct obj_s *obj = &c->obj;` refactor (mechanically rewrote 519 `c->obj.`
  → `obj->` and 47 `&c->obj` scoped to lines 1562–3055). Result: it *did* move
  `c` into `$s4` (candidate prologue became `addiu $s3,$s4,4` = obj=$s3,
  c=$s4), **but the match collapsed to 208/15376 (fuzzy 1.35)** — retail
  accesses `c->obj.X` **directly as offsets off `c` ($s4)** (`lbu $3,0xca4($s4)`
  = `0x92830ca4`), it does **not** hold an intermediate obj pointer, and the
  extra `addiu` shifts the whole prologue. So the earlier "$s0 = &c->obj"
  reading was a transient live-range, not a persistent pseudo. Cleanly
  reverted; baseline restored exactly (asm_hash `fe3129bc…`, diff_sig
  `82d2c980…`, 2408/15376). **Refined model:** both builds allocate the *same*
  9 callee-saved regs (mask 0xC0FF0000) — this is **not** a missing-variable
  problem but a **priority tie-break**: `c` and one of retail's $s0–$s3
  pseudos are adjacent in gcc-2.95 global-alloc priority and swapped rank.
  Next lever must nudge that ordering (ref-count / live-range of `c` vs the
  competing pseudo) without adding an addressing indirection — e.g. identify
  which local occupies retail's $s3 and whether my C computes it with a
  different live range. Needs the target/candidate prologue register-move maps
  side by side; `deep_reasoning` escalation flagged by the ledger.
- Second, smaller diff: the inlined
  UpdateRumble+vibration block (0xB0–0xFC) — retail evaluates `buzz != 0` via
  `sltu a1,zero,v0` into the arg register in the delay slot of the `frame==0`
  branch (arg hoisted before the value branch), which the candidate schedules
  after the if/else. Resume with `resume_session s-20260708-203123-c057fc`;
  regenerate disasm via `cli disasm`.

### DrawCreatures (game/creature, 0x001D2F50) — 12 words from fitting

Attempted 2026-07-08 (session `s-20260708-191404-3740f3`). The high-fuzzy WIP
C is kept in `src/game/creature.c` (state stays `asm`, so the hybrid still
splices the retail slice and every byte gate passes; a copy also sits at
`build/pal103/agent_sessions/s-20260708-191404-3740f3-creature.{c,h}.bak`).
Established facts:

- PAL constants differ from the GC reference: `in_finish_range == 0x32` (not
  0x3C), spin rotation `spin_frame * 0x1999` (not 0x1555), glass pulse
  `GameTimer.frame % 0x1E` (not 0x24), skeletal flicker `% 0xA < 5` (not
  `% 0xC < 6`). `s.y` mirror is `s.y * -1.0` through soft-double
  (fptodp/dpmul/dptofp), not `-s.y`. Shadows go through
  `ScaleFlatShadow` + `ShadRndr(&mS, model->shaddata, 1.0f,
  CData[ch].shadow_scale)` (D_0055FC88 = `&CData[0].shadow_scale`), not the
  GC stencil path. `D_005F28D6` = `GyroMoveInfo.SPINFRAMES`. The unit .lit4
  pool has duplicate slots (0.025f x3, 0.05f x2) in first-use order.
- `StoreLocatorMatrices` must be GNU89 `inline` and defined before
  DrawCreatures: it is genuinely inlined twice (both spin blocks), and the
  deferred standalone emission explains its retail address 0x001D5918 at the
  very end of the unit. With that, frame == 17024 and all 77 calls match in
  order. `mtxLOCATOR`/`momLOCATOR` are model-major (`[2][16]`, +0x400/+0xC0
  per model) — the header's `[16][2]` needs flipping when this lands.
- The one remaining blocker: my build spills `c + 0xCE4` (next-creature) to
  sp+0x41A0 and keeps the `&mC` web in $s4, while retail keeps next-c in $fp
  and spills both matrix pointers (0x4198 = hoisted `&mC` temp, 0x41A0 = a
  per-path `pm = &mC` variable also passed to NuHGobjRndrMtx /
  DrawCharacterModel). Net +12 words -> `.org backwards`, hybrid unbuildable.
  Splitting the DrawPlayerJeep result into its own variable did not move the
  allocation. Next idea: stop gcc coalescing `pm` with the `&mC` CSE web
  (e.g. different expression forms per block) or find the retail variable set
  that gives vflag/yrot/bVar9/next-c the callee registers s6/s7/s5/$fp.

### Symbols / `.mdebug`

- **VU microprogram symbols are dropped.** The 17 `vu_load_*_mpg` / `patch_vu0_mpg`
  externals point into `.vutext` (executable), so they are excluded from
  `symbol_addrs.txt` (they are code entry points, not data). Fine while
  `.vutext` is raw `bin`; re-add them as code symbols when VU gets real
  treatment.
- **Line info only on SCE runtime.** 229 procedures (the SCE `.s` files) carry
  `line_low`/`line_high`; the 3522 game `.c` procedures were compiled without
  line numbers, so those fields are omitted. Expected, recorded for reference.

### Disassembly / splat

- **`.vutext` / `.vudata` are VU microcode, not MIPS.** Emitted as raw `bin`
  (`build/assets/vutext.bin`), not disassembled. `.vudata` is zero-length and
  skipped. A real VU disassembly is a separate, later problem.
- **`.reginfo` file-offset quirk.** `.reginfo` (`0x70000006`) has file offset
  `0x0053402C` — far after `.data`, overlapping the NOBITS region — with a
  nominal vram `0x00293680`. It is not loaded (flags 0) and is not modeled as a
  splat segment. Revisit only for the "pure relink exact" goal.
- **bss split is approximate in size, exact in placement.** `.sbss`
  (`0x00633000`) and `.bss` (`0x00633400`) both use explicit vram, so symbols
  land correctly, but the `0x2C` vram gap between them means splat treats
  `.sbss` as `0x400` rather than its true `0x3D4`. NOBITS, so no bytes are
  affected; may need exact boundaries when the linker script has to match
  byte-for-byte.
- **Rodata "may belong to text" warnings.** splat warns that `.rodata` / `.lit4`
  are referenced gp-relative from `.text` (e.g. `NuMtxAlignZ` → `D_0062C980`).
  Expected because `migrate_rodata_to_functions` is off for the bootstrap.
  Revisit when migrating rodata into per-function files.

### Binary baseline

- **Un-sectioned "orphan" MIPS bytes between `.vutext` and `.data`.** A 0x80-byte
  run at file `0x194680` (vram `0x00293680`) is loaded but covered by no section
  header, and it is MIPS code, not VU. Now formally homed in the data map as
  `section = 'orphan', owner = 'unassigned', evidence = 'orphan'` and linked
  from its own per-range object. WHICH unit produced it is still unknown
  (mdebug attributes nothing there); promote it out of `unassigned` only
  with evidence.
- **`--link` needs Linux.** The PS2 binutils are Linux x86-64 binaries, so the
  real `ee-ld` reconstruction only runs on Linux or in the container. The
  default (pure-Python) baseline check runs anywhere and is the portable gate.

### Environment

- **Local Python is 3.11 / 3.13; the plan pins 3.12.** stdlib-only tools
  (`extract_mdebug.py`, `verify_target.py`, `configure.py`) run on all three;
  splat runs fine on 3.11 here. Only matters if a tool starts depending on
  3.12-only behavior.

### Build graph

- **Header dependencies are coarse.** `ee-gcc -MM` exists on GCC 2.9, but
  cc.py compiles from a staged native scratch directory (EOVERFLOW
  workaround), so a depfile would list scratch paths ninja can't use.
  Instead gen_ninja lists every committed `include/*.h` as an implicit dep
  of every cc/hybrid edge. Coarse but correct; revisit only if the header
  tree grows enough to hurt (then remap depfile paths in cc.py).
- **The expected/ tree stays flat** (`expected/pal103/NNN_<name>.o`,
  split_text's stem convention). PR 6 decided the human-facing structure
  lives in the objdiff unit *names* (`nucore/nulist`, `game/main`,
  `sdk/sce/graphdev_127`), which objdiff renders as a tree; the object
  filenames underneath don't need to mirror it, and the index prefix keeps
  same-named units (16 graphdev.c!) unique for free.
- **build.ninja must be regenerated by hand after adding a source file,
  status manifest, or header** (`python tools/gen_ninja.py`; configure.py
  also regenerates it after every split). Ninja can't discover new edges
  itself; a stale graph simply doesn't build the new unit.

### Modding / equivalent build

- **Relocatable equivalent link is a named milestone (multiplayer needs
  it).** Today every link resolves externals to fixed retail addresses
  (absolute defsyms) and places each TU at its retail offset, so modified
  code in the equivalent build must not change any function's size. The
  patch vehicle until then: same-size edits, or trampolines into slack
  space. The milestone -- linking the equivalent image relocatably so code
  can grow -- becomes natural as coverage rises (C units link by symbol,
  not by pin) and should be designed alongside the data-map work (PR 8+).
  Driving use case: the PCSX2 multiplayer patch (PS2 sibling of the
  crashwoc-decomp-gc multiplayer branch, where dtk's full relink allowed
  growing code).
- **`build/pal103/image/equivalent.elf` as a PCSX2 boot candidate is
  untested.** ld resolves the entry from `_start` and lays out the loaded
  sections at retail addresses, but header/segment details may need work
  before PCSX2 accepts it ("packaged ELF" axis). Try booting it once the
  equivalent build first diverges from retail.

### Matching

- **`strlen` is asm on PS2, not C.** Unit 211 is `strlen.S`: the retail
  `strlen` (0x00271be0, 78 instrs) is the hand-written Newlib **r5900** port
  using 128-bit quadword ops (`lq`, `pcpyld`, `psubb`, `pnor`, `pcpyud`). The
  generic Newlib C (which the GameCube decomp matched, since GC had no r5900
  asm) compiles to a ~37-instruction scalar routine and cannot converge. Left
  as assembly. When picking "first C function" targets, prefer functions whose
  unit is a `.c` file, not `.s`/`.S`.
- **Early-return vs wrapped-body changes return-block cross-jumping.** For a
  guard at the top of a function, writing `if (cond) return X;` (early return)
  lets gcc cross-jump/merge that `return X` with a *later* identical `return X`
  into one inline block placed right after the loop body -- one extra branch
  (`bgtz ...; b epi`) that shifts the whole tail. Wrapping the body instead,
  `if (!cond) { ...; return Y; } return X;` (as the retail source did), keeps
  the guard's `return X` at the end and lets a mid-body early return use a
  single `blez reg, <shared epilogue>` (value in the delay slot). Matched
  `NewCharacterIdle` (game/creature) in one iteration by switching from the
  flattened `if (GameMode==1) return 0;` to the wrapped `if (GameMode!=1){...}
  return 0;` form. Mirror the reference's brace structure, not just its logic.
### game/creature (unit 91) session findings, 2026-07-07

- **RESOLVED: the TT compiler is identified and locked (2026-07-07).**
  Candidate sweep over the decomp.me compiler archives (scorecard =
  per-function byte compare of src/game/creature.c + src/nucore/nulist.c
  against retail, locked flags): every ee-gcc 2.9 build scores 6-7/19,
  **SN ProDG ee-gcc 2.95.2-EE (driver "2.9-ee-991111b/r4", decomp.me
  `ee-gcc2.95.2-273a`) scores 14/19** -- including every previously
  impossible sq-saving function -- with 2.95.2-274 and 2.95.3-107/-114
  output-identical on the corpus and 2.95.3-136 ruled out (different frame
  layout). Locked as component `ee-gcc-tt` (profile `default`); the Sony
  2.9-991111-01 stays as component `ee-gcc` (profile `sce`) for the
  runtime half. The Win32 driver runs under the locked `wibo` 1.1.0
  loader inside the container (proven .text-identical to a native
  Windows run). gen_hybrid learned the SN spelling `.align 3` as a
  segment lead. Full gates re-ran green under the new profiles
  fingerprint; promotions after the switch: ResetPlayerMoves,
  RemoveCreature, CloseCreatures, StoreLocatorMatrices, ChangeCharacter,
  NuListAppend, NuListRemove -> **14 matching total**. Still `equivalent`:
  ModelAnimDuration (retail carries an extra mtc1->c.le.s hazard nop this
  SN build does not emit -- possibly a different SN patch level),
  PlayerStartPos/AddCreature/ProcessCreatures/UpdateAnimPacket (ordinary
  source-shape iteration, first real chance now that the compiler is
  right). The paragraphs below record the original mismatch evidence.

- **The (previously) locked Sony compiler cannot reproduce TT game code's register saves.**
  Retail game/engine code saves callee-saved GPRs with `sq/lq` (10,484
  sq-saves in .text); `sd`-saves appear ONLY in SCE/newlib/libgcc units
  (1,431). EE GCC 2.9-ee-991111-01 with the locked flags emits `sd/ld`
  (16-byte slots, so the frame layout matches -- only the store width
  differs), and its cc1 has no switch that changes this (full
  TARGET_SWITCHES table extracted and probed). Additionally both this cc1
  and its `as` carry an *unconditional* R5900 short-loop erratum workaround
  that pads backward branches with nops; retail contains unpadded
  4-instruction loops (e.g. LoadCharacterModels at 0x001CCC78), so the
  locked toolchain provably did not build the game TUs. Travellers Tales
  most likely used SN Systems ProDG (their ee-gcc 2.95.x builds ship as
  Windows binaries). Consequence, per user decision (2026-07-07): keep the
  locked compiler; only leaf functions without saved registers can reach
  `matching`; everything else with byte-exact-except-save-width output is
  recorded as `equivalent`. ResetPlayerMoves and RemoveCreature verify
  byte-identical to retail EXCEPT sq/lq<->sd/ld (proven with a
  save-width-normalizing differ); CloseCreatures additionally differs in
  one store's base register choice (`sb $zero,0(s0)` vs `-4(s1)`).
- **Hybrids are now assembled with the decompals `as`, not Sony's.**
  Sony's `as` cannot assemble the retail slices at all: it predates
  `%gp_rel(sym)($gp)` (hard error), and its unconditional short-loop
  padding inserts bytes into spliced slices (unit 91 grew 0xC bytes).
  The decompals `as` assembles both the slices (it already builds the
  byte-verified expected objects from the same text) and ee-gcc's `-S`
  output: `.extern SYM, n` plus macro forms (`lw $2, SYM`) resolve to
  R_MIPS_GPREL16 under `-G8` (probed). Two pseudo-instruction encodings
  differ from Sony's and are text-normalized by gen_hybrid's `_sonyize`
  before assembly: `move` (Sony daddu, decompals or) and one-operand
  `break N` (code field position). The whole-unit byte gates remain the
  arbiter; verify-loaded still reproduces the retail SHA with unit 91's
  hybrid (6 matching functions from C) in the link. gen_hybrid also
  hoists byte-less trailing `.extern NAME, SIZE` directives (emitted by
  ee-gcc for sized extern arrays) into the prologue instead of failing.
- **.lit4 literal pools from C are SUPPORTED (2026-07-07).** ee-gcc never
  emits a .lit4 section in `-S` output -- float constants appear as
  `li.s $fN,<decimal>` and the assembler materializes them. gen_hybrid now
  rewrites pool-bound li.s lines (float32 image with a nonzero low half;
  zero-low-half constants stay inline lui+mtc1) into `lwc1 $fN, D_0062....`
  against the retail .lit4 slot, mapped BY VALUE from the gp references in
  the function's own retail slice (ambiguity or an unmappable constant
  fails loudly; li.d/.lit8 support added 2026-07-08, see below). Two
  supporting invariants landed
  with it: compiled segments are zero-filled to their registry extent with
  `.org` (retail extents include trailing pad nops; gas errors loudly if
  the code overruns), and the assembled hybrid is checked to own NO data
  sections. verify_promoted's per-function byte compare now reads the
  freshly built MATCHING HYBRID (the object the image actually links,
  where the slot mapping lives) instead of the plain compile; the plain
  compile still backs the declared-defined check, and match.py's
  .text-only link places the object's own .lit4 at the retail base
  (0x0062C980) so it links -- pool-using functions honestly read DIFF
  there and are verified through the hybrid gate. Proof: TerrainFailsafe
  (2000000.0f) is promoted `matching`, image SHA exact.
- **Initialized-local `.sdata` now maps onto the owned retail slot.**
  gen_hybrid (`_extract_local_data` + `Lit4Mapper.map_owned_data`) pulls a
  compiler-private initialized aggregate ($LCn in .sdata/.rodata) out of the
  compiled stream, matches its bytes to the unit-owned data symbol the
  function's retail slice references, and rewrites `%hi/%lo($LCn)` onto that
  symbol (`.extern`, no data emitted) -- the same trick `_rewrite_lis` plays
  for .lit4. `short layertab[2] = {0,1}` (retail D_006309C8) no longer blocks:
  **DrawCharacterModel is now `equivalent`** (matching C reconstructed from
  the retail disasm; short of byte-exact only on register allocation -- retail
  keeps `anim` in s2 and the locator `i` in s4, the locked compiler swaps them
  -- so it stays equivalent, not matching). Same unblock is now available to
  EvalModelAnim / DrawCreatures once their C is written. Fixed while here:
  `map_for_slice` returns ordered slot lists so two `.lit4` slots holding the
  same value in different branches (D_0062D178 / D_0062D17C, both 9.58738e-05f)
  map positionally instead of raising "ambiguous".
- **`li.d`/`.lit8` double pools + address-resolution borrow (2026-07-08).**
  Two gen_hybrid extensions:
  (A.2) `_rewrite_lis` now handles `li.d` the same way it handles `li.s`:
  a double whose float64 image has a zero low WORD (low 32 bits) stays inline
  (lui+mtc1 pair), anything else is rewritten `ldc1 $fN, D_xxxx` against the
  retail `.lit8` slot, mapped BY VALUE from the slice's `%gp_rel` references
  (`Lit4Mapper.map_for_slice8`, positional like `.lit4`). No more
  "li.d not supported" raise.
  (A.2b) `li.s`/`li.d` into a *GPR* (not `$fN`) is a DIFFERENT construct: not a
  pool load but ee-gcc materializing the raw float/double bit pattern inline in
  an integer register -- e.g. `li.d $5,1.0e1` to pass 10.0 as the `s64` argument
  of the software-double helper `dpmul` (float→double math on the EE, which has
  no hardware doubles). Retail expands it inline (`ori $a1,$zero,0x8048;
  dsll32 $a1,$a1,15` == 0x4024000000000000). `_rewrite_lis` now rewrites these
  to `dli $reg,0x<bits>` (BOTH sizes use `dli`, never `li`: a high-bit-set
  float32 image like -1.0 -> 0xBF800000 would `lui`-sign-extend into bits 32-63,
  so `dli` sets all 64 bits explicitly and zero-extends deterministically).
  Enabled UpdateAnimPacket (unit 91) to reach `equivalent` (2026-07-08).
  CAVEAT for a future MATCHING candidate: `dli` is a MACRO, so its expansion is
  assembler-dependent -- the same class of decompals-`as`-vs-SN-`as` divergence
  that `_sonyize` normalizes explicitly (move→daddu, break). It byte-matched
  retail for the 10.0 shape, but an arbitrary double is only best-effort: if the
  decompals `as` `dli` expansion differs from retail's SN sequence the whole-
  unit byte gate blocks promotion (loud, safe, never wrong bytes) and the
  function can't reach `matching` until the explicit sequence is emitted instead.
  Regex-covered by tests/test_fp_pool.py; the `$fN` negative lookahead keeps
  pool loads on the lwc1/ldc1 path.
  (A.4) `map_owned_data` no longer requires a NAMED retail twin: when the
  compiler-private aggregate's bytes match no slice-referenced symbol, it
  searches the function's OWN unit data ranges (data_map.toml) for the unique
  address holding those bytes and emits `D_<vram>` (auto-resolved by the image
  link's `AUTO_NAME_RE`), at the aggregate's natural alignment so zero padding
  can't spoof a match. Ambiguity (bytes at >1 address in the unit) still fails
  loudly. This keeps the hybrid `.text`-only and byte-exact (data still comes
  from the per-range data object at its retail address) -- the no-data-sections
  rule was NOT relaxed. The existing named-twin case (layertab -> D_006309C8)
  takes the fast path unchanged; `ninja check` image SHA still exact.
  (A.3, same day) `_assemble_data_bytes` now handles the string family
  (`.ascii` verbatim; `.asciiz`/`.asciz`/`.string` append the NUL) with a
  quote-aware decoder (`_decode_as_string`: octal `\NNN`, hex `\xHH`, letter
  escapes; a `#`/`,` inside quotes is literal), so a function-local string
  literal ($LCn in .rdata) captures its bytes and borrows the retail slot via
  the same named-twin/address path. `_borrow_addr_in_unit` is now two-tier
  (natural alignment, then byte-aligned) so an align-1 retail string is still
  located; any unique equal-bytes address is byte-correct since the bytes come
  from the per-range data object.
- **Switch jump tables now borrow the retail jtbl slot (A.1, 2026-07-08).**
  Ground truth from a throwaway `-O2` switch: ee-gcc emits the table mid-
  function in a `.rdata` block (`$Ln: .word $Lm ...`, relocations) addressed
  `%hi/%lo($Ln)` -- absolute, exactly like retail's auto-named `jtbl_<vram>`;
  and dispatches with `j $reg` where retail has `jr $reg`. `_extract_local_data`
  already captured that block (`.rdata..`.text`-delimited); the fix detects an
  all-`.word <label>` payload as a `_JumpTable(count)` marker (not bytes),
  `Lit4Mapper.map_jump_table` finds the single `jtbl_`/`jpt_` symbol the
  function's slice addresses (room for `count` entries), and `_rewrite_local_data`
  repoints `%hi/%lo($Ln)` onto it and drops the compiler's copy -- borrow-by-
  STRUCTURE, since the entries are relocations and can't be byte-matched. The
  retail table (byte-exact in the per-range data object) points at the matching
  function's own labels, so correctness falls out for a matching function.
  Added `_sonyize` rule `j $reg -> jr $reg` (register set enumerated so a
  `j $Llabel` is never touched). Validated: parser tested against the real
  ee-gcc output; `ninja check` SHA still exact. NOT yet byte-validated end to
  end -- that needs an actual switch function (ManageCreatures 0x9DC / MovePlayer)
  written in C; the tooling no longer raises, so they are now attemptable, and
  a pointer-array initializer (`.word GlobalSym`) that isn't a real jtbl fails
  loudly in map_jump_table (no jtbl_/jpt_ match) rather than corrupting.
  ModelAnimDuration's .lit4 ref maps fine
  but the function stays `equivalent`: the locked compiler allocates
  registers differently from retail there (an extra `daddu $a1,$v0,$zero`
  copy of action*4 in retail, different lb target reg from the start) --
  same compiler-mismatch class as the sq/sd saves, not a data problem.
  UpdateCharacterIdle (0.01666667f), ResetPlayer, LoadCharacterModels,
  NewCharacterIdle: .lit4 no longer blocks them; reconstruct and judge
  each against the compiler drift individually.
- **PS2 v1.03 creature layout facts** (full layout in include/creature.h):
  creature_s 0xCE4 (== alpha-NGC DWARF dump layout exactly: ai 0x18C/0x98,
  m 0x234, mtxLOCATOR 0x274, momLOCATOR 0xA74, lights 0xBF4/0xB0, rumble
  0xCA4); obj_s 0x188 at creature+0x4. `CModel` is **48** entries on PS2
  (GC: 49): CloseCreatures' loop bound is 0x1C980 = 48*0x988 and CLetter
  sits at CModel+48*0x988. CharacterModel is 0x988 (GC 0x7AC): PS2 adds
  shadow-skin fields sanmdata[118] @0x764, shadhdr @0x93C, shaddata @0x940
  (character @0x944, pLOCATOR[16] @0x948). NewRumble uses 0x32 (50) where
  GC uses 0x3C (60) -- PAL frame rate. ProcessCreatures has a PS2-only
  TimeTrial-restart block on level 0x11. ResetPlayer/ChangeCharacter carry
  ResetPlayerMoves' body manually inlined in retail (no call).

## Resolved

- **gen_hybrid mis-attributed a jump table to the wrong function via a
  substring match (2026-07-08).** `ninja report` (report_base build, which
  includes every WIP asm-state function's compiled C) failed with
  "DrawCreatures: 1 switch jump table(s) in the compiled code but 0
  jtbl_/jpt_ symbol(s) in the retail slice". Root cause was NOT DrawCreatures'
  C: its `switch(VEHICLE)` (sparse cases 0x36/0x53/0x63/0x81/0x8B) correctly
  compiles to a balanced comparison **tree**, exactly like retail (retail
  DrawCreatures has 0 jump tables, one `jr` = the return). The bug was in
  `_rewrite_local_data`: `used = {lbl for lbl in local_data if any(lbl in line
  ...)}` used a plain **substring** test, so ManageCreatures' 19-entry table
  `$L161` matched DrawCreatures' unrelated branch labels `$L1610/$L1612/$L1614`
  (`$L161` is a prefix). It then ran map_jump_tables for a table DrawCreatures
  doesn't own. Fixed by matching private labels on word boundaries with the
  same `(?<![\w$.])LABEL(?![\w$.])` regex the rewrite already used (both the
  `used` set and the `first_use` ordering), reusing the compiled patterns in
  the final `sub`. Regression test in tests/test_fp_pool.py
  (`TestLocalDataLabelBoundary`); report object builds; 248 unit tests pass.


- **Docs match the implementation again, and a smoke test guards the artifact
  (2026-07-07).** Corrected the stale "Sony's own `as`" claim in gen_hybrid.py's
  docstring and CLAUDE.md step 4 -- hybrids are normalized (`_sonyize`) and
  assembled with the decompals `as`, not Sony's. Rewrote README ## Toolchain
  (the two-compiler setup: `default`=ee-gcc-tt for game/engine, `sce`=ee-gcc for
  the Sony runtime; wibo; per-file fingerprints) and ## Matching C (clean C +
  status manifests; hybrid `matching`/`equivalent` sets; the current /
  report-current / matching / equivalent object sets; verify_promoted +
  link_image as the canonical byte gate; the objdiff data units and the
  decomp.dev treemap caveat below). New tools/smoke_report.py runs in
  matching.yml after the artifact is staged and before upload: it re-checks the
  exact staged bytes (valid JSON, strict Report-schema subset, no
  orig/absolute-path/blob leak via sanitize_report.violations, no hollow
  "100%-of-nothing" measure, every verified matching function at 100% via
  check_report_matches.check) so a stale or tampered staged file fails the run.
  ci.md gained the pipeline step, the "what can leave a run" gates, and a rollout
  checklist. IMPORTANT design fact to preserve: decomp.dev's treemap only renders
  units with total_code > 0, so the modelled data units are correctly present in
  the report and category metrics but do NOT show as clickable tiles -- this is
  expected, and we must never invent an artificial code value to force them to
  appear.

- **The linked data is now modelled in objdiff, and hollow measures are
  stripped both ways (2026-07-07).** Investigation (reproducible, `-p` probe
  project in build/): objdiff-cli 3.7.2 DOES score a data object that has no
  symbols -- it measures `total_data` from the allocated `.split.*` section
  bytes, aggregating every `.split.<sec>_<addr>` slice into one logical
  `.split` section. A target-only unit (no base) reports that `total_data`
  with matched_data absent (honest 0); a unit whose base equals its target
  reports matched_data_percent 100. So the preferred modelling is supported.
  gen_objdiff.data_units() now emits one target-only objdiff unit per linked
  data object from the committed data map (both per-owner `unit-NNNN` and every
  `unassigned/<sec>_<addr>` gap/orphan -- all bytes that actually link),
  named `data/<data-map-stem>` (prefix guarantees no collision with a .text
  unit) in a dedicated "data" progress category. Report-level `total_data` is
  now the real linked extent (0x39f... bytes) with 0 matched -- honest. The
  report edge gains the data-objects stamp as a dependency so the targets
  exist. Decisions: (a) unassigned ranges ARE modelled (they link, so they are
  honest data), (b) data gets its OWN category rather than folding into
  game/engine/sdk, keeping code categories pure. Consequence handled: data
  units carry zero code, and objdiff emits "100% of no code" (fuzzy_match,
  matched_code*, matched_functions*) for them -- the mirror of the data lie on
  code units. sanitize_report._strip_hollow_measures was therefore generalised
  from data-only to a per-total table (total_code -> matched_code*/complete_code*
  /fuzzy_match_percent; total_data -> matched_data*/complete_data*;
  total_functions -> matched_functions*): any derived measure whose total is
  absent or 0 is dropped, honest totals kept. fuzzy rides with total_code (a
  data-only unit's fuzzy is objdiff's no-code artifact) -- revisit if C data
  ever contributes to a unit's fuzzy. No byte gate touched; PR4's invariant
  still holds (14 matching functions at 100%). objdiff.json/build.ninja stay
  deterministic (both regenerate byte-identically across two runs).

- **The published report no longer claims completed data that does not exist
  (2026-07-07).** objdiff-cli emits the data-completeness measures
  (matched_data, matched_data_percent, complete_data, complete_data_percent)
  even when a measures block models zero data bytes -- which renders as "100%
  data complete" about nothing on decomp.dev. sanitize_report.py now runs
  `_strip_hollow_data` on every measures block (report, unit, category): when
  total_data is absent or 0, those derived measures are dropped and the honest
  total_data (0) is kept. A block that genuinely models data keeps them, so
  this survives future data-unit modelling untouched (that fuller modelling is
  the separate, investigation-driven follow-up). The sanitizer still only ever
  REMOVES fields; the output stays a strict subset of objdiff's Report schema.
  Nothing here touches a byte gate -- the public report is a measurement only.

- **matching.yml now triggers on push to ANY branch, so same-repo PRs get
  byte gates + a decomp.dev comment automatically (2026-07-07).** Goal: a
  collaborator's PR should get the decomp.dev progress-vs-main comment without
  a manual dispatch. The decomp.dev app posts that comment when it finds a
  `SLES_503.86_report` artifact for the PR's head commit, so the report has to
  be built on the PR branch. The naive fix -- adding a `pull_request` trigger
  gated by a job-level `if: head.repo == this repo` -- is NOT fork-safe: a
  `pull_request` run executes the workflow file *as modified by the PR branch*,
  so a fork simply deletes the `if`. Instead the `push` trigger was widened
  from `branches: [main]` to `branches: ["**"]`. `push` is fork-safe by
  construction: a push to a fork runs in the fork's own Actions, never in this
  repo, so untrusted code never reaches the private `/orig` image. A branch
  push here can only come from someone with write access (trusted, same as a
  push to main). concurrency now cancels superseded feature-branch runs but
  never a mid-flight `main` run (`cancel-in-progress: ref != refs/heads/main`).
  The baseline bot-commit stays gated to push on `refs/heads/main`. Supersedes
  the entry below; `pull_request` remains deliberately absent, and
  `pull_request_target` is still never used. Documented in docs/ci.md
  ("PR verification").

- **(Superseded by the entry above.) The `pull_request` trigger was removed
  from matching.yml to close the fork-exfiltration hole (2026-07-07).** The dtk
  private-image move (entry below) had accepted that a fork PR could rewrite the
  workflow and read /orig out of the private image. That risk is no longer
  accepted: matching.yml triggers on push, nightly, and workflow_dispatch only
  -- a fork PR can never start a job with /orig access. validate.yml still runs
  on every push and PR (public, no game files), so contributors keep full
  structural feedback. Byte verification of a fork PR is still a maintainer
  action: review the PR, take the reviewed commit onto an in-repo branch (its
  push now runs the gates), then merge. `pull_request_target` is deliberately
  NOT used (it would run untrusted PR code with the private image just the
  same). Documented in docs/ci.md ("PR verification").

- **CI moved from a self-hosted runner to the dtk-template private-image
  method (user decision, 2026-07-07).** The founding constraint is
  consciously relaxed from "game files never transit GitHub" to "game
  files never in the PUBLIC repo": orig/pal103 now lives in the private
  repo denzi-gh/crashwoc-decomp-ps2-build, baked into a private GHCR image
  (FROM the public toolchain image ghcr.io/denzi-gh/crashwoc-decomp, which
  publish-image.yml keeps fresh from the Containerfile). matching.yml runs
  inside that image on ubuntu-latest -- same gate commands as before,
  plus actions/cache for the toolchain and the ninja tree. It ALSO ran on
  pull requests (dtk-style, accepted fork-exfiltration risk) -- SUPERSEDED:
  the pull_request trigger was later removed (see the entry above); the
  bot-commit step is gated to main pushes. The
  self-hosted runner (registered a day earlier) was deregistered and
  docs/runner.md replaced by docs/ci.md. Why: runner ops (PC uptime,
  registration friction) outweighed the stricter storage stance; sly1,
  SOTC, and the whole dtk ecosystem run the equivalent trade.

- **verify_promoted must be serialized behind the builders of the files it
  rewrites.** The verifier re-derives (and thus REWRITES) the current
  objects, matching hybrids, and the matching image; running
  `ninja verify-loaded verify-promoted` let both edges write
  image/matching.bin concurrently and one run failed with a vanishing
  file. Fixed in gen_ninja: the verify_promoted edge now lists the current
  objects, matching objects, and the matching image as implicit inputs --
  purely for ordering. Exposed by the first real WIP-function demo
  (three consecutive combined runs green after the fix).

- **(Historical -- superseded by the dtk private-image move above. The
  self-hosted runner and docs/runner.md no longer exist; the described
  validate.yml no-pull_request guard was removed. The push-to-main / nightly
  / manual-only trigger policy, however, is again the current one.)** The
  protected workflow is the runner-side mirror of the local loop, and
  fork PRs can never reach it. `.github/workflows/matching.yml` runs on a
  self-hosted runner labeled `crashwoc` (docs/runner.md is the setup guide)
  on push-to-main, nightly, and manual dispatch ONLY -- validate.yml now
  mechanically fails if matching.yml ever gains a `pull_request` trigger.
  Steps are exactly the proven local commands via `python tools/dispatch.py`
  (dispatch is now container-aware on POSIX hosts too: direct execution
  only inside the container or with CRASHWOC_DIRECT=1, so the same workflow
  runs on a Windows or Linux runner). Checkout uses `clean: false` so
  gitignored runner state (orig/, compiler/, tools/download/, build/)
  survives; game files come from pre-placed orig/pal103/ or are copied
  from $CRASHWOC_ORIG_DIR. Gate order: fingerprints -> verify_target ->
  registry --checks -> configure -> ninja expected current matching ->
  verify-loaded + verify-promoted -> compare_progress (red before anything
  is published) -> report + sanitize -> upload artifact `SLES_503.86_report`
  (containing ONLY the sanitized report, staged as report.json) ->
  compare_progress --write + bot-commit progress/summary.json when counts
  changed (GITHUB_TOKEN pushes trigger no new runs; [skip ci] as belt).
  The sanitized report lost its invented `schema` key: decomp.dev parses
  the artifact as a protobuf-JSON Report, so the public file must stay a
  strict SUBSET of objdiff's schema (tested). Whole sequence dry-run green
  end-to-end on the future runner machine itself, including the seeded
  regression (exit 1 with exact deltas) and a compare_progress --write
  crash-fix for baselines outside the repo root.

- **Progress reporting is pinned, program-wide, and publishable only through
  a whitelist.** `ninja report` runs the pinned objdiff-cli 3.7.2 (locked in
  toolchain.lock.json as a single-binary artifact: real release URL, SHA-256
  `85b7bec0...` computed from the downloaded binary, installed by
  setup_toolchain.py's new `kind = "binary"` path, verified by
  `fingerprint_compiler.py --component objdiff`) over objdiff.json ->
  build/<v>/report.json covering all 247 units. Reporting never gates:
  a deliberately nonmatching edit to NuListGetHead produced fuzzy 99.5%,
  matched_functions 2 -> 1, and ninja still exited 0 (compile errors DO
  fail, by design). `ninja report-public` runs tools/sanitize_report.py:
  a field WHITELIST (known measures; unit name/measures/sections/functions/
  {complete, source_path, progress_categories}; category id/name/measures)
  -- any field a future objdiff adds is dropped until reviewed -- then a
  guard pass over the output that refuses to write if any string references
  orig/, is an absolute path, or looks like a hex/base64 blob. On the real
  report only the empty per-symbol `metadata` dicts get dropped
  (687 KB pretty -> 300 KB compact). Two report quirks, both harmless:
  objdiff-cli warns "Failed to parse MIPS mdebug line info: range exceeds
  .mdebug size" on expected objects (line info only; measures unaffected),
  and the report counts 3,761 functions vs 3,751 in functions.toml -- the
  10 extras are splat-invented `func_XXXXXXXX` labels for retail code with
  no mdebug PDR (crt0 1, nups2/ptl 4, nups2/asmdma 4, sifcmd 1): the report
  counts object symbols, the registry counts mdebug functions, both honest.

- **Promotion is mechanical: bytes in, state out, rollback on doubt.**
  `tools/promote.py <function>` is the only writer of `state = "matching"`:
  it flips the manifest entry (byte-preserving edit -- line endings and all
  other content untouched), runs `tools/verify_promoted.py`, and rolls the
  file back exactly if anything fails. The verifier re-derives every claim
  from scratch on each run: manifest consistency (status rules), declared
  functions defined in the plain compile (WIP C beyond declarations is a
  note, never fatal), every `matching` function byte-compared over its full
  registry extent (size = gap to next function, so a shrunken function
  can't match its prefix), `complete` units must not own data ranges yet
  (data-from-C unsupported -- fail loudly, not under-verify), and finally
  the freshly rebuilt matching image must reproduce the retail SHA. Results
  land in build/<v>/verify_results.json stamped with the profiles
  fingerprint. Acceptance runs: demote->promote round-trip restores the
  manifest exactly (GetHead + GetTail re-promoted through the tool);
  promoting `NuListCheck` (no C exists) flips, fails "declared but not
  defined", and rolls back to `asm`. `promote.py --init src/<unit>.c`
  scaffolds an all-asm manifest from the registries for new units.
  `tools/compare_progress.py` computes progress/summary.json (committed
  baseline: 2 matching, 16 verified bytes -- honest numbers only) and
  fails on regressions: fewer matching functions/bytes, fewer complete
  units, image no longer exact, or a fingerprint change without a fully
  green re-verify. Seeded-regression test: inflating the baseline to
  matching=3 makes it exit 1 with the exact delta reported. New ninja
  targets: `verify-promoted`, and `check` = current + verify-loaded +
  verify-promoted.

- **Data sections are attributed to units and linked per-range, image still
  exact.** `tools/extract_data_map.py` reads two `.mdebug` sources -- EXTR
  `ifd` (the unit that DEFINES each global) and per-unit stStatic SYMRs --
  and emits committed `config/pal103/data_map.toml`: an exact tiling of
  .data/.rodata/.lit4/.sdata/.sbss/.bss plus the orphan run into
  `unit-NNNN`/`unassigned` ranges (`--check`-gated like the other
  registries). Findings:
  - **EXTR layout confirmed** as `short reserved; short ifd;` (GNU ECOFF,
    little-endian): validated on every run by cross-checking all 3,055
    procedure externals' ifd against their PDR units (NuListGetHead -> 7
    among them). A wrong layout guess cannot survive this.
  - **No section interleaves.** All six data sections form clean
    non-interleaved per-unit symbol runs -- even .bss (commons were
    resolved per-unit by the original link). The planned degrade-to-
    unassigned path exists but never triggers on this binary.
  - 3,750 data symbols attribute 0x35BB8A of 0x47315C data bytes (~76%
    overall; .data itself is 92% attributed). .lit4 has zero symbols (compiler float
    literals) -- fully unassigned, as expected. A unit's range ends at its
    LAST symbol (sizes aren't recorded), so the tail bytes up to the next
    run are honest `gap` ranges; single-symbol runs prove no extent yet.
  - `tools/gen_data_objects.py` (ninja `data`) builds 323 objects (116
    per-unit + 207 unassigned incl. the orphan) over the 373 PROGBITS
    ranges; `link_image.py` now places every range at its exact address
    (`.vutext` remains one incbin). `ninja verify-loaded` reproduces
    `c92a5987…0fe438` over the per-range link -- byte-exact by
    construction, now proven.

- **The canonical image now contains decompiled C and is still byte-exact.**
  `tools/link_image.py` (ninja `verify-loaded`) links the full loaded image
  from a named object set: hybrid objects for units with a status manifest,
  expected objects for the other 246, incbin sections as in the baselines.
  With the unit-7 hybrid (2 C functions) in the link the image reproduces
  the retail loaded-image SHA (`c92a5987…0fe438`) exactly. Negative control:
  swapping one line of C (`return list->tail`) flips the gate to FAIL with
  the divergent SHA reported, and a failed gate deletes its output bin so a
  stale image can't pass for a verified one. `ninja image-equivalent` links
  the equivalent set (gate: links successfully; SHA reported, divergence
  allowed -- that's the modding build).

- **objdiff sees the whole program; C only ever improves the number.**
  `tools/gen_objdiff.py` (regenerated together with build.ninja by
  gen_ninja.py/configure.py) writes objdiff.json listing all 247 .text
  units. Every unit's diff target is its expected object, so every one of
  the 3751 functions is visible in objdiff under its real mdebug name from
  day one; only units whose `src/<name>.c` exists get a base object
  (`build/pal103/current/<name>.o`) — everything else honestly reads 0%.
  Builds go through `python tools/dispatch.py ninja <object>`
  (custom_make), so the GUI's rebuild works from the Windows host
  (verified end-to-end: touch nulist.c, objdiff-style base rebuild = one
  cc node). Unit names are derived from the mdebug source paths
  (nu2crash.ps2/... → engine, `.\*.c` → game/..., the rest → sdk/...);
  name collisions (16 graphdev.c, 10 libgcc2.c, 4 mallocr.c) get the unit
  index appended on every member. The unit name doubles as the canonical
  source location: unit X decompiles into src/X.c with a manifest at
  config/pal103/status/X.toml. Three progress categories: game, engine
  (Nu), sdk (SCE libs + kernel glue + newlib + gcc). match.py no longer
  writes objdiff.json.

- **`-mbranch-relocs` is a no-op for this pipeline — not adopted.**
  Experiment (PR 6, script kept in build/branch_relocs_experiment.py):
  assembled the branch-heavy 001_nufile expected slice (62 procedures,
  0x38B0 bytes) with decompals-as with and without the flag. Identical
  relocation tables (870 relocs, same kind histogram), identical raw
  .text, and both link byte-identical to retail at 0x001000C8. Reason:
  every branch target in the disassembly is a section-local label, which
  GAS resolves at assembly time regardless; the flag only matters for
  branches to external/global symbols, which the slices never contain.
  Default assembly flags stay as they are.

- **Incremental builds run through ninja in a long-lived dev container.**
  `tools/gen_ninja.py` (also called by configure.py after each split) emits
  a gitignored build.ninja over the proven tools: one splitter edge fans
  asm/text.s into 247 per-TU expected `.s` files (write-if-changed +
  `restat = 1`, so an unchanged TU never cascades into a reassembly), one
  `as` edge per expected object, one `cc.py` edge per src file (current),
  and one `gen_hybrid.py` edge per status manifest per link set
  (matching/equivalent). All paths repo-relative POSIX. From Windows,
  `python tools/dispatch.py ninja <target>` forwards into the detached
  `crashwoc-dev` container (`tools/dev_container.py`, repo mounted at
  /work), translating host paths to /work — this is the exact command
  objdiff will be configured with in PR 6. Verified: 247 expected objects
  byte-identical to split_text.py's (spot-checked 000/007), no-op build
  does nothing, touching one `.c` rebuilds exactly one `cc` node, and the
  ninja-built matching hybrid still passes verify_hybrid.py whole-unit
  byte identity.

- **Hybrid objects are manifest-driven splices — clean C sources, no
  INCLUDE_ASM annotations, no transplant tool.** Decided in two steps. The
  gate experiment first proved the file-scope `__asm__(".include ...")`
  route works on ee-gcc 2.9 (whole unit 7 byte-identical), so no
  asm-processor/mwccgap equivalent is needed. But annotations in every C
  file are noise, so the final mechanism moves the splice into the build:
  source files contain only decompiled functions; `tools/gen_hybrid.py`
  compiles the unit with `ee-gcc -S`, cuts the output into per-function
  segments, splices retail slices for every function the status manifest
  does not mark `matching` (address order — source order never matters), and
  assembles the result back through the ee-gcc driver (Sony's own `as`, same
  profile flags). Gate result: unit 7 hybrid (2 C functions + 8 spliced
  slices) is byte-identical to retail across the entire 0x240-byte range
  (0x00105850..0x00105A90). Negative controls: a function declared
  `matching` but absent from the C fails loudly before assembly; wrong C
  bytes marked `matching` fail the whole-unit compare at exactly that
  function. The `equivalent` link set works the same with
  matching+equivalent compiled from C; the current/report object is just the
  plain compile of the source, so fallback bytes can never appear in what
  objdiff scores — no preprocessor modes needed anywhere. Anything
  gen_hybrid cannot represent yet (data sections in compiler output,
  unexpected inter-function content) raises instead of guessing.
  Findings that shaped the mechanism:
  - The original assembler (GNU as 2.9-ee-991111) accepts only *numeric* GPR
    names (`jr $ra` = "illegal operands", `jr $31` assembles; `$sp/$fp/$gp`
    are special-cased). `tools/gen_slices.py` rewrites spimdisasm's symbolic
    registers numerically — a pure rename, encodings unchanged.
  - It also predates splat's `macro.inc` macro features, so slices are
    emitted with the block macros expanded (`glabel` → `.globl` + label;
    `nonmatching`/`endlabel` dropped — markers/metadata, no bytes).
  - Verified syntax support: `%hi/%lo`, branch-likely, `.L` local labels,
    FP registers, r5900 SIMD (`lq/sq/pnor/pcpyld`), 3-operand `mult`.
  - ee-gcc emits `.p2align 3` before every C function. Safe: every function
    in a `.c` unit is 8-aligned *relative to its TU base* (checked across all
    3751 registry functions; the only 31 exceptions are in hand-written `.s`
    units, which never become hybrid C units), so the directive never pads.
  - splat auto-functions (`func_XXXXXXXX`, absent from mdebug) ride along in
    the *preceding* registered function's slice — consistent with the
    "size = gap to next function" model. The two loader nops at 0x00100000
    (`func_00100000`, before `_start`) belong to no slice; the per-TU
    expected objects still carry them. If a function with a trailing auto
    block is ever decompiled, the whole-unit gate will flag the orphaned
    bytes and the auto block will need its own fallback slice.

- **Compiled objects were not deterministic run-to-run.** `build/src/*.o`
  hashes differed between identical `match.py` runs: the EOVERFLOW workaround
  compiled in a random `TemporaryDirectory`, and ee-gcc embeds that path in the
  object (debug/mdebug info); the `.text` bytes were unaffected. Fixed when
  compilation moved into `tools/cc.py`: it compiles in a *fixed* native scratch
  directory derived from the output object's repo-relative path, so the
  embedded path — and the whole object — is identical run after run (verified:
  back-to-back runs hash equal). Determinism is per output path: the same
  source compiled to a different output object embeds a different scratch path.

- **EE GCC + PS2 binutils need one container, and two quirks.** The matching
  compiler (EE GCC 2.9) is a 32-bit x86 binary needing i386 multilib; the
  decompals PS2 binutils need glibc 2.38 (trixie). Bookworm ran the compiler but
  not the binutils. The Containerfile now uses `python:3.12-slim-trixie` **plus**
  i386 libs so one image runs both. Second quirk: the 32-bit EE GCC faults
  (`Value too large for defined data type`, EOVERFLOW on `stat`) when reading a
  bind-mounted source with a 64-bit inode, so `tools/match.py` compiles from a
  native temp dir and copies the object out.

- **Same-name statics collide in a monolithic assembly.** Four static functions
  (`ReadNuIFFGeomSkin`, `ReadNuIFFGeomVtx`, `NuNodeRead`, `_fpadd_parts`) each
  appear in two TUs under the same name; `allow_duplicated:True` lets splat emit
  them, but a single assembly file cannot define one name twice.
  `tools/assemble_text.py` disambiguates byte-losslessly for the monolithic
  baseline: each definition becomes `NAME__<vram>` and each `jal`/`j` is
  repointed to the definition its own encoding targets (all `jal`, confirmed;
  the tool fails loudly on any other reference form), so no instruction byte
  moves. The PR 7 TU split reuses this same rename (rather than per-object local
  scope): the `nonmatching` macro also emits a *global* `NAME.NON_MATCHING`
  marker, so two TUs marking a same-named static would still collide at link
  time; unique `NAME__<vram>` names keep the markers unique too.
- **Per-TU objects and `.align` phase.** Splitting `.text` at the `.mdebug` unit
  boundaries assembles each TU separately, which exposed two toolchain quirks
  fixed in `tools/split_text.py`. (1) GNU `as` rounds a section's *size* up to
  its alignment, so a TU whose `.align 3` set 8-byte alignment but whose content
  isn't 8-aligned gained trailing pad. (2) 11 TUs start at a 4-aligned (not
  8-aligned) address, so their internal `.align 3` mis-phased when the object's
  section base is treated as offset 0. Both vanish because the disassembly
  already covers `.text` with one explicit instruction word per four bytes and
  zero address gaps: every `.align` is a no-op, so the tool drops them all and
  links the pure instruction streams with `SUBALIGN(1)` (plain concatenation).
  A guard fails loudly if a dropped `.align` ever coincides with an address gap.
- **`_stack_size` false address.** Was emitted as a data symbol at `0x00100000`
  (coincides with the `.text` base) and made splat label a spurious function at
  the entry. Fixed: `data_section_ranges` now excludes executable sections
  (`SHF_EXECINSTR`), so a "data" object in `.text`/`.vutext` can't slip in. This
  also correctly dropped the `ENTRYPOINT` alias, leaving `_start` alone at the
  entry. Symbol count 6320 → 6302.
- **Pinned splat missing from the container.** Added `requirements.txt`
  (`splat64[mips]==0.41.0` + pinned `spimdisasm`/`rabbitizer`); the
  `Containerfile` now installs it into the image, so `configure.py`'s version
  check passes in the container without a runtime fetch.
- **Wrong pip name in the lock.** PR 2 locked `splat0==0.41.0`; the PyPI package
  is `splat64` (`splat0` 404s). Fixed in `toolchain.lock.json`, with the
  `spimdisasm`/`rabbitizer` backends pinned alongside.

## Invariants (do not break)

- **Nothing game-derived is committed.** All splat output (asm, linker script,
  `build/assets/*.bin` raw dumps, generated macros) is gitignored. Only config
  is tracked. If a new tool emits game bytes, route them into `asm/` or
  `build/`.
- **`configure.py --check` is the determinism gate.** Splits twice and compares
  every generated file. Run it after any change to `splat.yaml`, the symbol
  registries, or the disassembler versions.
