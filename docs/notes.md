# Working notes

Loose ends, known artifacts, and decisions to revisit. Not a task tracker —
just things that came up while building and shouldn't be lost.

## Open — revisit later

### HubSelect (game/game, 0x001DC7C8) — faithful full-C in tree, fuzzy 9.5%

Session `s-20260711-204712-b3fed2` (2026-07-11). Full-C reconstruction of the
2368-byte hub-warp/level-select routine is in `src/game/game.c` (with local
minimal `struct creature_s`/`nugspline_s`/`spltab_s`/`gdeb_s`/`moveinfo_s`
views; offsets verified against `include/creature.h` — obj at creature+0x4).
Structure is faithful to the retail disasm (GC `game.c` used only as a naming/
shape hint): two hub-SFX loops, the `hublevelopen` open/lock grid with the
`k`-flag debris spawn, and the spline-crossing enter/leave tests with the
Hub-launch (mom via `NuTrigTable`, `AddGameDebris(0x82)`) and leave-cut switch.
Compiles + links clean, no unresolved symbols; state stays **asm** (not promoted).

- **Blocked by the SN-assembler FP-hazard nop class** (same as SetLevel /
  ModelAnimDuration below). `ee-gcc -S` emits commented `#nop` before the
  `c.lt.s`/`c.le.s` compares and before the `bc1f`/`bc1t` branches; `_sonyize`
  keeps them as `#nop` and decompals `as` drops them, while retail's SN ProDG
  `as` materialized real nops. HubSelect has ~12+ float compares (two `d<6`/`d<3`
  volume gates, ~8 `c.le.s` crossing tests, the spin `c.lt.s`), so each dropped
  nop shifts the whole tail — hence the low fuzzy despite correct logic.
  Candidate frame is +16 (288 vs 272), also downstream of the shift.
- PS2-specific constants that differ from the GC port: first hub-SFX loop uses
  `GameSfxLoop(0x10F, closest)`; the `i==3` random pitch is
  `gamesfx_pitch = 0x3AD - (qrand()*0x139)/0x10000` + `GameSfx(0x114,..)`;
  `NewBuzz(&rumble, 0xA)`; volume slope consts are the gp floats
  `D_0062D2A4`/`D_0062D2A8`, debris y-lift is `+0.25f` (i<4) else `D_0062D2AC`.
- Same tooling fix as SetLevel would unblock it (blocker recorded:
  `build/pal103/agent_blockers/…HubSelect-075fde.json`).

### SetLevel (game/main, 0x001CB600) — faithful C in tree, fuzzy 73.3%

Session `s-20260709-232345-bd8dbe` (2026-07-09). Full-C reconstruction is in
`src/game/main.c` (state stays `asm`; byte gates unaffected — the hybrid splices
the retail slice for it). Behaviourally complete and **structurally byte-exact**:
88/120 bytes match. Prototype `void SetLevel(void)`:

```c
LDATA = &LData[Level];          /* LData elem 0x54; LDATA gp_rel ptr */
LBIT  = (u64)1 << Level;        /* sd / dsllv */
if (pNuCam != 0) {
    if ((u32)LDATA->farclip > 9) /* farclip u16 @0x2A -> sltiu, reloaded twice */
        pNuCam->farclip = (float)LDATA->farclip;   /* @0x4C */
    else
        pNuCam->farclip = 1000.0f;
}
AIVISRANGE = 25.0f;
```

Names/types from the GC reference (`farclip` both fields). `ldata_s` extended to
size 0x54 with `farclip` @0x2A; `nucam_s` extended with `farclip` @0x4C. The
`(u32)` cast is what yields `sltiu` (u16 would promote to signed `int`); the two
direct field reads (no local) reproduce retail's two `lhu` loads.

- **The only divergence is one hazard `nop`.** Retail's 1000.0f (else) branch is
  `lui at,0x447A0000; mtc1 at,f0; nop; swc1 f0,0x4C($a1)` (nop @0x660). Our build
  omits that nop, shifting the tail and the `beqz`(0x628)/`b`(0x650) offsets each
  by one. `ee-gcc -S` emits a commented `#nop` and defers to the assembler
  (`.set reorder`); decompals `as` inserts the `mtc1->cvt.s.w` hazard nop (so the
  cast branch at 0x648 matches) but **not** the `mtc1->swc1` one. Retail's SN
  ProDG `as` inserts the mtc1->store hazard nop; our SN ee-gcc + decompals `as`
  do not. The branch layout (1000.0f at the trailing fallthrough label, cast in
  the `b` delay slot) is fixed by retail, so the store can't be tucked into a
  delay slot — it needs the standalone nop.
- **Same class as ModelAnimDuration** (mtc1->c.le.s hazard nop, line ~278) — a
  `compiler_patchlevel_mismatch` between retail's SN `as` and our locked build.
  Blocker recorded. Fix is tooling-side: teach decompals `as`/gen_hybrid to emit
  the `mtc1->{swc1,c.le.s}` hazard nop as it already does for `mtc1->cvt.s.w`;
  that would make both SetLevel and ModelAnimDuration byte-exact. Candidate for
  `equivalent` review.

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

- **RESOLVED 2026-07-10: modding goes through the fixed-layout injection
  SDK (`tools/modsdk/`, branch `modding-sdk`), not a relocatable link.**
  The shift/relink approach (branch `modding-functionality`) was set
  aside: relocating the image goes stale on every unlabeled non-string
  data pointer, and mods don't need anything to move. The SDK keeps
  retail bytes at retail addresses, injects compiled mod code as a second
  PT_LOAD above the image end, hooks in via `j` trampolines, and carves
  the heap past the blob with a one-word sbrk-break patch. Design, memory
  layout, hook contract, and the boot pitfalls (in-place phdr extension,
  DoInput vs UpdateLevel, WUMPACOUNT vs plr_wumpas) are in
  docs/modding-sdk.md. Boot-verified the same day in PCSX2 over PINE.
  Repo split decided alongside: the old multiplayer repo is now
  `crashwoc-coop` (kept as reference); the new empty
  `crashwoc-multiplayer` gets the ground-up PC-side network layer, coded
  against mods/include/mailbox.h.
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

## PlayerCreatureCollisions (game/game_obj, 0x1f9bc0) — faithful near-match WIP

(2026-07-10) Full-C reconstruction of `PlayerCreatureCollisions` (3544 bytes,
frame 224, ~886 insns) built from the ground-truth disassembly and kept in
`src/game/game_obj.c` at `state=asm`. It compiles and links over the full
extent; the structure is faithful (verified block-by-block against `asm/text.s`):
per-object loop over `pObj[]`, cpos from locator matrix vs `po->pos`,
cuboid/sphere overlap split, the attack (`!overlap`) branch with the second
`reach` overlap test + debris, and the physical-overlap branch (top/bottom
`dir==1`, side-up `.L001FA18C` ctype=2, side-down `.L001FA458` ctype=4, dir==0
`.L001FA5F8` ctype=1), the shared kill tail (`.L001FA724/72C/730`), and the
post-loop mask-lose / obj-death / `ObjectToAtlas` epilogue.

Blocked on **register allocation only** (fuzzy 1.467, 52/3544 bytes; ~98% differ
purely from register renumbering, not logic):
- target puts `obj` in `$s2` (`daddu $s2,$a0` @0x10), reserving `$s0/$s1` for the
  short-lived hot loop temps (`overlap`, `is_cuboid`); gcc-2.95 gives me `obj` in
  `$s1`, shifting every register.
- one extra callee-saved float: `distsq` lands in `f22` (freg_mask 0x00700000)
  instead of coalescing into `f20`, which the target reuses for the platform-ride
  `dx/dz`; this also inflates the frame to 240 vs 224.

Confirmed exact facts to preserve on resume: `maskflag<3` is held once in `$fp`
(hoisted to `mask_weak`); spin `reach` = `radius*SCALE` then `*2` for
character!=1 or slide==0, else `*3.0` (NOT `(x+x)*3`); `reach` add consts
0x3F000000/0x3E800000; bounce consts `D_0062D7C0..D8` are gp-rel floats;
`po->parent->0x21C` (cast `(s8*)`) drives the obj-death `GetDieAnim` fallback;
`v010` copied into `obj->vSN` (0x98). Session `s-20260710-071439-be7b01`.
Next: coax allocation order (deep-reasoning escalation) so overlap+is_cuboid take
`$s0/$s1`, obj takes `$s2`, and distsq coalesces into `f20`.

### InitWorld (game/main, 0x1c57c0, 2712 bytes) — faithful full-C near-match WIP

Full C reconstruction kept in `src/game/main.c` at state=asm (2026-07-10). This
was the last un-decompiled function in the coop/multiplayer plan
(`crashwoc-decomp-docs/plan.md`, True-coop world-ownership root). Structure is
verified block-by-block against ground truth: 175/175 `jal` match, control flow
exact. It's the big per-level world/asset loader — long runs of
`NuDisableVBlank()/NuStrCpy(load_txt, <caption>)/NuEnableVBlank()` wrapping the
real work call, interleaved with `NuGScnRead` (world_scene[0..4]) and
`sprintf(tbuf,fmt,LevelFileName)+NuFileSize>0` guarded effect/object/anim/clump
loads. Gated on `LDATA->flags` bits (0x4 world scenes, 0x8 terrain, 0x10 edpp
effects, 0x40 crates, 0x1 pause) and `Level` values (0xE/0x1F/0x27/0x29 pick the
scene set; 0x25 savegame icon; 0x23 pause; 0x27/0x29 cut movies). Ends with a
0x800-byte `qrand()>>8` stack buffer → `NuRndrShadowInit`, then
`edcrt_used=edwmp_used=edai_used=0`.

Confirmed-exact facts: `LDATA` fields exposed at 0x00 (`char *name`, NuStrCat
onto LevelFileName) and 0x1C (`void *sfx`, arg 2 of edbitsRegisterSfx) — struct
edit is offset-preserving (size stays 0x54; DoInput re-verified byte-exact).
`world_scene[i]=0` clear loop 31→0. The 64-bit LBIT mask `0x5A031894DULL`
materializes via `li 0x5; dsll16; ori 0xA031; dsll16; ori 0x894D` — decompals
`as` expands the `dli` to the SAME 5 instrs as retail SN as (verified), so that
is NOT a blocker. `terraininit` takes 7 args (a0..a3 + t0/t1/t2). superbuffer
bumps: `(ptr + n + 0xF) & 0xFFFFFFF0`.

Blocker: gcc-2.95 register allocation (same class as [[setlevel-wip-state]] et
al). Target holds the four hot `%hi(symbol)` bases in callee-saved regs
(s4=LevelFileName, s6=world_scene, s7=load_txt, fp=tbuf) → 10 saved regs
(reg_mask 0xc0ff0000, frame 2224). Direct-global C gets 8 (0x807f0000) and is
+4 bytes / one delay-slot nop (the `beqz(flags&4)` slot the target fills with
`lui %hi(D_0061AED0)`) → overflows the .org-positioned hybrid diff harness.
Explicit `char *lt/tb/lfn` base locals (CURRENT tree) make it fit + link + score
(fuzzy 13.569, 368/2712) but drop to 7 callee regs (0x803f0000, frame 2176).
Neither reproduces the target's %hi-in-callee-reg allocation. Session
`s-20260710-082142-d199f2`. Resume: coax gcc to promote %hi(load_txt)/%hi(tbuf)
into s7/fp (deep-reasoning escalation), or accept as near-match — not required
for the mod (matching build splices the retail slice regardless).

## game/vehicle — NEWBUGGY special-vehicle transforms (2026-07-11)

Typed `struct NEWBUGGY` (creature_s.Buggy, creature+0x224) from the glider /
atlasphere transform code. Fields below are PS2-verified from the retail
disassembly (offset → type → meaning); the coop-mod puppet syncs these.

Glider (read `DrawGlider`, write `ProcessGliderMovement`):
- 0x28  float   `timer`  (ProcessTimer countdown)
- 0x30  nuvec   `pos`    glider position → NuMtxTranslate
- 0x48  nuvec   `vel`    velocity; `pos += vel*dt` each frame
- 0x70  float   `pitch`  X rotation (NuMtxRotateX)
- 0x74  float   `roll`   Z rotation (NuMtxSetRotationZ)
- 0x80  float   `yaw`    Y rotation (NuMtxRotateY)
- 0x98  s32     `mode`   0 normal / weather-boss branch selector
- 0xA0  nuvec   `avel`   angular/seek vector (SetNuVec)
- 0xB4  s32     `enable` transform-enable flag (DrawGlider gates level-0xD branch)

Atlasphere / ball (read `DrawAtlas`, seed `ObjectToAtlas`, write `MoveAtlas`/
`AdjustAtlasRotations`):
- 0x20C nuvec   `ball_pos`   position → NuMtxTranslate; MoveAtlas copies it into
                the character `obj.pos` (± D_00560D44 y-lift), so the character
                *follows* the ball.
- 0x23C nuvec   `ball_vel`   velocity; MoveAtlas copies into `obj.mom`.
                ObjectToAtlas seeds it from `obj->mom / D_0062DE9C` (x,z gated by
                `temp_xzmomset`) and `obj->mom.y` (0x240).
- 0x26C float   `atlas_rotparam`  rotation tuning float (AdjustAtlasRotations arg)
- 0x284 nuquat  `rotquat`        render orientation → NuQuatToMtx (DrawAtlas)
- 0x294 nuquat  `rotquat_delta`  accumulator; AdjustAtlasRotations NuQuatMuls a
                frame delta into it.

Angle→NuMtx int units: glider angle * gp const (level 0xD `D_0062D91C`,
0x1A `D_0062D920`, else `D_0062D924`); `(int)(float)` casts lower to `cvt.w.s`
(EE FPU is always round-toward-zero, so no `trunc.w.s`). Per-level model-index
bytes (`D_0056236E/B9/C3` glider, `D_0056238B` ball) live far from `$gp`; declare
as `extern s8 NAME[]` and read `[0]` so gcc uses %hi/%lo, not a truncating
`%gp_rel` (R_MIPS_GPREL16 overflow otherwise). Same for the far float lift const
`D_00560D44[]`.

Match status:
- `DrawAtlas` — **matching** (byte-exact, promoted; full image re-verifies).
- `DrawGlider` — faithful full-C near-match kept `state=asm`. Frame(112)/
  reg_mask(0x800f0000)/freg_mask(0x00300000) all exact; inverted-ternary
  `lift = (Level!=0xD && Level!=0x1A)?15:0` removes the merge `b`. Blocked +4B on
  the %hi(mTEMP) rematerialization / `bne` delay-slot scheduling (retail shares
  one `lui $s1` in the delay slot across the 0x1A/else split; gcc rematerializes
  per block + speculatively loads the else scale). Same class as
  [[setlevel-wip-state]] / [[initworld-wip-state]]. Outer-else pointer hoist
  fits (536B) but over-shares (single addiu) and reshuffles s0–s3 → 47%.
- `ObjectToAtlas` — faithful near-match `state=asm`, 50% (ball_pos half exact).
  Diverges on the mom/divisor FP load order (retail dividend-first, gcc hoists
  the shared divisor) and a `beql` vs `beq` delay-slot fill in the `boing` check.
- `MoveGlider` — architectural blocker: `.rodata` jump table `jtbl_0061F3B0`
  + soft-float doubles. Semantics extracted only.
- `ProcessGliderMovement` (1696B), `MoveAtlas` (608B), `AdjustAtlasRotations`
  (672B, soft-float doubles) — large physics fns; field semantics extracted,
  full byte-match deferred (out of scope this pass).

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

## Invariants (do not break)

- **Nothing game-derived is committed.** All splat output (asm, linker script,
  `build/assets/*.bin` raw dumps, generated macros) is gitignored. Only config
  is tracked. If a new tool emits game bytes, route them into `asm/` or
  `build/`.
- **`configure.py --check` is the determinism gate.** Splits twice and compares
  every generated file. Run it after any change to `splat.yaml`, the symbol
  registries, or the disassembler versions.
