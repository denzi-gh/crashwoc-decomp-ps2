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

## Overlap (measured by `xref_gc.py`; refreshed 2026-07-15)

- **1366** of 3487 status-carried PS2 functions have a GC twin
  (**1309** exact, 57 name-only).
- **1090** of those are still `state=asm` — the migration pool (270 twins now
  `matching`, measured 2026-07-19 against the status manifests).

**`gc_xref.toml` embeds each function's `state`, so it goes stale on every
promotion** and `xref_gc.py --check` fails until it is regenerated. It had drifted
by 143 states (1289 → 1147) before being refreshed on 2026-07-15. Re-run
`python tools/xref_gc.py` whenever you promote, or treat the state column as
advisory and read the status manifests instead — they are the source of truth.
Branch-wide state totals (2026-07-19): **364 matching / 7 equivalent / 3116 asm**
(3487 status-carried functions).

## Scoreboard

Per-function outcomes: `as-is` (GC draft matched unchanged), `tweaked`
(matched with edits), `from-asm` (GC draft discarded, rewritten), `blocked`
(recorded blocker).

### Phase 2 — pilot (measure hit rate, then STOP)

Outcome legend: as-is = GC draft compiled byte-exact unchanged (a hostile
constant may still have been corrected against asm); tweaked = GC draft matched
after a structural edit; from-asm = no GC reference, rebuilt from disassembly;
near = faithful near-match kept in tree as `state=asm` (blocker recorded).

| Unit           | fns | GC-ref | attempted | matched | as-is | tweaked | from-asm | near |
|----------------|-----|--------|-----------|---------|-------|---------|----------|------|
| `game/listman` | 9   | 5      | 9         | 8       | 3     | 1       | 4        | 1    |
| `game/game_deb`| 7   | 7      | 6         | 4       | 4     | 0       | 0        | 2    |
| `game/chase`   | 10  | 10     | 1         | 1       | 1     | 0       | 0        | 0    |
| `game/vehterr` | 9   | 9      | 1         | 1       | 0     | 1       | 0        | 0    |
| **total**      | 35  | 31     | **17**    | **14**  | 8     | 2       | 4        | 3    |

Per-unit notes:
- `game/listman` (complete, 8/9): 4 of 5 GC-referenced matched
  (Destroy/Alloc/Free as-is after correcting the GC `0x8000` in-use flag to the
  retail `0x10000`; GetNext tweaked). The 4 unreferenced fns
  (GetByIdx/GetPrev/AllocBefore/AllocAfter) all matched from asm. NuLstCreate is
  a near-match (s0/s1 regalloc wall).
- `game/game_deb` (tractable set done, 4/6 attempted): Init/Add/AddRot/AddMtx
  matched as-is. AddWarpDebris near (single f3/f4 FP-allocator swap). Retail
  **inlines AddGameDebris** into AddMechanicalDebris — reproduced with a gnu89
  plain-`inline` (emits standalone AND inlines), fixing the frame; residual is
  a jeepbits-base regalloc, kept near. AddAnimDebris (4KB, GC-acknowledged
  regalloc-blocked) not attempted.
- `game/chase` (sampled): ChaseActive matched as-is (chase_s status@0x7540,
  stride 0x7548). Nine larger fns (splines/models) not attempted.
- `game/vehterr` (sampled): FindSurfaceRotXZFromNormal matched (tweaked: u16
  angle, int callee types, temp for post-call store scheduling). Eight larger
  FP-terrain fns not attempted.

**Measured pilot hit rate (byte-exact / attempted): 14 / 17 = 82%.**
GC-referenced only: 10 / 13 = 77%. The three non-matches are all faithful
near-matches (structure/offsets/extent exact) blocked on compiler
register-allocation / scheduling, not on GC-draft correctness — every hostile
GC constant and layout fact was catchable against the PS2 asm. Reusable ee-gcc
levers are logged in the memory `gc-migration-listman-lessons` and in the
recorded blockers. Smallest-first, GC drafts are a strong structural base:
recommend proceeding to Phase 3 bulk waves, expecting a high as-is/tweaked rate
on small leaf functions and near-matches concentrated in FP-heavy and
regalloc-sensitive code.

### Assembler fix — mtc1 hazard nop (2026-07-15)

Not a migration wave: a `_sonyize` fix in `tools/gen_hybrid.py`. The decompals
`as` inserts the `mtc1`→`c.cond.s` hazard nop only on a register dependency;
Sony's `as` inserts it whenever the compare is adjacent. Full rule, retail proof
pair, and the falsified `#nop`-marker theory are in docs/notes.md; unit tests in
`tests/test_hazard_nop.py`.

**+4 matching (298 → 302), no regressions** (`verify-promoted` re-derived all
298 prior claims, `verify-loaded` reproduced the retail SHA):
- `fsign` (game/vehsupp) — was 33.3%, the C was always correct.
- `JudderGameCamera` (game/camera) — was a recorded near-match.
- `ModelAnimDuration` (game/creature) — **was `state=equivalent`**, now
  byte-verified `matching` (equivalent count 8 → 7).
- `NuLineToPointDistSqr` (numath/nuplane) — 56.9% → 100%. Was *not* in the
  documented backlog; found only by auditing every site where the rule fires.

Audited across all 35 non-sdk units, the rule fires in **12 `matching`
functions** (ProcessTimer, NuPlnDist2, AnimateGYRO/GLIDER/ATLASPHERE/DIVE,
RatioDifferenceAlongLine, AddBugLight, DrawPanel3DObject, + the fixed ones) —
all still byte-exact, so `verify-promoted` is real evidence of idempotency, not
a vacuous gate. It fires in 11 `asm` functions; measured before/after, **nothing
regressed**: NuLineToPointDistSqr went exact, MovePlayer improved slightly
(12968→12960 differing bytes), the other 9 are byte-identical (dependent cases
where the assembler already inserted the nop, so the explicit one changes
nothing).

The documented backlog predicted ~25 beneficiaries; the rest are **not** this
wall (regalloc/scheduling, `.rodata` doubles).

**The second claimed assembler bug does not exist on this branch.** The
decompals `as` does hoist into a bare `j $31` slot (probed), but *not* for `jal`
(it leaves the nop retail has). And gcc never emits a hoistable bare return
here: across all 35 non-sdk units there are 41 bare `j $31` in reorder mode — 29
preceded by a label, 11 by a directive, 1 by `li.s` (a *macro*: lui+mtc1 = two
instructions, which cannot fit a one-slot delay, so gas will not hoist it, and
that function — ModelAnimDuration — matches). **0 hoistable.** Beware the
false positive: `InitCrateExplosions`/`CalculateGamePercentage` look hoistable
until you notice their predecessor sits in a bgez delay slot inside gcc's own
noreorder block; both are `matching`. The only claimed cases,
NuVpSetSize/NuVpInit, live on the unmerged `nu3d/nuvport` branch — re-evaluate
there, not here.

### Assembler fix — mtc1 hazard nop, WIDENING EVIDENCE (2026-07-16)

**Not yet implemented — this records the retail evidence the rule needs.**
docs/notes.md states verbatim: *"Only the compare class is claimed - widen to
add.s/swc1 only against retail evidence."* Auditing unit-0094 produced that
evidence: the hazard is **not** specific to `c.cond.s`. Three distinct consumer
classes, four independent retail witnesses, all in `game/game`:

| consumer   | witness | site |
|------------|---------|------|
| `c.cond.s` | `UpdateTimer` | 0x001F024C — `mtc1 $at,$f3 / nop / add.s / c.le.s $f3,$f1` |
| `mul.s`    | `DrawParallax` | 0x001EE7E4 — `mtc1 $at,$f1 / nop / mul.s / mul.s $f0,$f0,$f1` |
| `swc1`     | `AddMaskFeathers` | 0x001EF8F4 — `mtc1 $at,$f0 / nop / swc1 $f0,0x84($a1)` |
| `swc1`     | `LoseMask` | 0x001E35E0 — same construct, independent second witness |

**Unified rule:** any FP instruction that READS a register written by `mtc1`
must be ≥3 instructions later (≥2 slots). Sony's `as` enforces this; the
decompals `as` only does so for the compare class today.

**Negative controls (must stay as-is after any widening):**
- `ApplyFriction` @0x2208f8 — disjoint registers at distance 1 → **no** nop.
- `fsign` @0x221530 — disjoint but adjacent → exactly **1** nop.

Expected beneficiaries: DrawParallax (a pure one-word shift — **zero** real
instruction diffs), UpdateTimer, AddMaskFeathers, LoseMask — all with **no
source edits**. Re-measure UpdateAwards too. Note this is the same wall behind
`game/sfx` GameSfx (`mtc1`→`add.s`) and `game/vehsupp` fsign/ASin/ACos
(`mtc1`→`c.le.s`), so the widening reaches beyond unit-0094.

A possible **third** class is unconfirmed: `AddKaboom` @0x001DF51C has a nop
between `c.lt.s` and `bc1tl` (compare→branch, not mtc1→consumer). Check it
against the widened rule before assuming it is covered.

### Wave A cont. — game/vehsupp (2026-07-15)

+3 matching, each byte-exact on the **first** attempt (24 matching in the unit):
- `SetNuVecPntrA` (32B) — `SetNuVecPntr` twin on `D_006B75D0`.
- `ACos360f` (40B) — `90.0f - ASin360f(x)`; needs `extern f32 ASin360f(f32)`
  since ASin360f is earlier in the same TU and still asm.
- `MyAnimateModelNew` (72B) — `oldaction = action`, then
  `UpdateAnimPacket(model, &Anim, dt, 0.0f)` (EABI: dt passes through $f12,
  the 0.0f literal lands in $f13) and `GetLights(field2C, &lights, 1)`.
  `field2C` is really a `nuvec_s *`; cast at the call rather than retype the
  struct, which would disturb the matching `MyInitModelNew`.

### Wave A cont. — game/{bug,cloudfx,sfx,font3d} (2026-07-19)

Four units revisited in parallel (one subagent each), smallest-remaining-first.
**+5 matching (359 → 364), no regressions** (each promotion ran the full byte
gate; the loaded image still reproduces retail SHA `c92a5987…0fe438`):

- `game/cloudfx` 5→7: cloudInit, DoClouds (both from-asm — GC render twins are
  useless; PS2 rewrote the unit for the console DMA/VU1 path).
- `game/font3d` 3→5: InitFont3D (tweaked from twin), RemapAccentedCharacter
  (as-is, jtbl borrowed).
- `game/sfx` 5→6: TestLocalSfx (from-asm, no twin).
- `game/bug` 5→5: UpdateBugLight attempted, faithful near-match, blocked.

Cross-cutting findings this wave:
1. **GC render/DMA twins are hostile for PS2**: cloudfx (cloudInit/DoClouds/
   cloudRender/TimeTunnel*) and font3d Text3D all had to be reconstructed from asm
   — the "exact" GC hints did not apply after the PS2 console rewrite.
2. **A "data-from-C" blocker class was mis-classified**: game/sfx
   InitGlobalSfx/InitLevelSfxTables level tables and SFX strings are
   extern-referenceable retail data, not data-from-C. Re-audit other
   `data-from-C` blockers the same way before trusting them — the real wall there
   is regalloc/scheduling. Only genuine compiler-emitted `.rodata` (jump tables,
   `switch(language)` selectors as in InitLocalSfx) is a true data-from-C wall.
3. The **mtc1-hazard-nop widening** tooling fix remains the single highest-leverage
   next step: it would unblock GameSfx (+ likely GameAudioUpdate) and reaches into
   UpdateBugLight's 7-site wall — a one-`_sonyize`-rule change vs per-function
   hand-work. See the 2026-07-16 "WIDENING EVIDENCE" subsection above.

### Phase 3 — bulk waves (in progress from 2026-07-14)

**Wave A** (`game/`), smallest-first. Same outcome legend as Phase 2.

| Unit           | attempted | matched | as-is | tweaked | from-asm | near/deferred |
|----------------|-----------|---------|-------|---------|----------|---------------|
| `game/bug`     | 6         | 5       | 0     | 5       | 0        | UpdateBugLight now a faithful near-match (11.9%, 2224B PAL rewrite from asm) — 3 simultaneous walls: delay-slot-fill + mtc1-hazard (7 sites) + callee-save $f25/$f26 |
| `game/cloudfx` | 9         | 7       | 0     | 1       | 6        | +2 (2026-07-19): cloudInit (448B), DoClouds (1096B) — both from-asm (GC render twins useless). Near: cloudRender (VU1/GIF/DMA packet, callee-save regalloc), TimeTunnelInit (24.7%, 3-way $s1/$s2/$s3). TimeTunnelRender (1920B, DoClouds-like driver) analyzed, deferred |
| `game/font3d`  | 7         | 5       | 1     | 4       | 0        | +2 (2026-07-19): InitFont3D (440B, tweaked), RemapAccentedCharacter (368B, as-is, jtbl borrowed). Near: Update3DFontObjects 92.6% (whole-fn FP $f2/$f3 tie, 4 spellings→same hash), Text3D from-asm (GC twin hostile for PAL; all divergences decoded) |
| `game/sfx`     | 12        | 6       | 0     | 5       | 1        | +1 (2026-07-19): TestLocalSfx (112B, from-asm, no twin). **Data-from-C reclassified**: Init{Level,Global}Sfx tables/strings are extern-referenceable retail data — real wall is regalloc/scheduling; both now faithful near-matches. Only InitLocalSfx has genuine compiler-owned jtbl rodata. PauseGameAudio (8B coin-flip) + GameSfx (mtc1→add.s) + GameAudioUpdate (soft-double+mtc1) near/blocked |
| `game/lights`  | 9         | 8       | 4     | 4       | 1        | SetLights near (parallel-move); 11 larger FP-proportion/editor/data fns pending |
| `game/camera`  | 7         | 6       | 3     | 3       | 0        | JudderGameCamera near (mtc1->c.lt.s wall); shared rail helpers GetALONG/Further* matched; 12 larger fns pending |
| `game/deb3`    | 8         | 8       | 6     | 2       | 0        | 8 debris helpers all byte-exact; 2 PAL 0x3c->0x32 divergences (JonMaskFPS, PlayRandSFX); 11 larger RBody/AddDeb3/Proc/Launch fns pending |
| `game/cut`     | 9         | 9       | 8     | 1       | 0        | 9 byte-exact (helpers/cutscene fns); confirmed unregistered D_ data symbols link via extern; NewCut PS2 drops music_volume assign; 17 larger fns (Load/Play/Update/SetLights) pending |
| `game/jeep`    | 27        | 23      | 5     | 12      | 6        | +16 this pass: WesternRaceManager, FindTerrainType, FireBossReset, ProcessFireBossLevel, ProcessGenericTrail, NewFindTrailAng, EmptyTrail, NewInitTrail(inline), WesternArenaReset, InitJeepBalloons, FindJeepBalloon, AddBalloon(inline), DrawEnemyJeeps, ProcessEnemyJeeps, AnimateForLightsEnemyJeep(s)(inline). Levers: far-.data D_ via `extern T D_xxx[]` (absolute, else gp-rel truncates link); inline helper via gnu89 `inline` earlier in TU; PAL anim const 0.5999999642 (0x3F199999, not 0.6f); PAL WesternTime 1/50. Near: SteeringUpdate (const-hoist regalloc), TiltSeek (mfc1->cvt.w.s hazard nop), NewFadeOutLastTrail (fade-init regalloc), BlendNUVECs (jal delay fill). Deferred: ProcessWesternArenaLevel (inlines 2 EnemyJeep loops), FireBossActionName (switch-jtbl+string data), CurrentWesternPosition (movz/movn); big Jeep/FireBoss/Trail state machines pending |
| `game/move`    | 10        | 9       | 6     | 3       | 0        | +2 recent (parallel): AnimateOFFROADER (216B; RotDiff precompute, dead-check `>=2` fall-through, angle chain 0x67 fall-through), AnimateGYRO (248B, 2-param `(creature_s*, pad_s*)`; pad_angle sign `>=0`/direction `==0` ternaries steer bltz/bnez; mtc1-zero;c.lt.s hazard nop reproduced OK). 9 Animate* vehicle-state fns (DROPSHIP/MINECART/GLIDER/ATLASPHERE/JEEP/MOSQUITO/DIVE/OFFROADER/GYRO). Near: AnimateSWIMMING (264B, structurally exact; soft-double `0` materialized into a callee-saved reg vs gcc's `$zero` direct — whole-fn callee-save regalloc wall, same class as PlayerLateral*; stays asm). obj@0x4 in creature_s. PAL divergence: ATLASPHERE idle 1/240->1/200 (0.005f). UpdateArrow deferred: isolated compile_diff link chokes on neighbouring jtbl_00628700 (tooling, not C). Big Move* state machines + Reset*/Check*/spline fns pending |
| `game/vehsupp` | 20        | 20      | 14    | 4       | 2        | +3 recent (parallel): SetNuVecPntr (stores 3 floats into D_006B75C0 via `extern struct nuvec_s D_006B75C0[]` absolute form), MyInitModelNew (136B, 6 int args a0-a3/t0/t1; &CModel[CRemap[i]] stride 0x988, ResetAnimPacket+ResetLights), MyDrawModelNew (152B, DrawCharacterModel forward, ChrisJoint* gp-rel globals in source order, capture ret before ChrisJointOveride=0 clear); MYDRAW extended to full layout (model@0x1C,index@0x20,numjoints@0x24,jointlist@0x28,lights@0x30). math/trig/seek core + debug-draws + anim wrappers. PAL 1/50 (ProcessTimer); rate = double 1-1/pow(2,dt/hl); X=X+delta idiom; Sin/Cos = NuTrigTable[(u16)(s32)(a*182.0444489f)]; Draw* reconstructed from asm (absent in GC). Near: CrossProduct/SeekHalfLifeNUVEC/SeekAng* (FP regalloc), fsign/ASin/ACos (mtc1->c.le.s wall), Rationalise360f/LimitAng360f (180/360 .rodata double + hazard). MYDRAW/model wrappers + FindSpline* pending |
| `game/crate`   | 32        | 25      | 5     | 6       | 14       | **Full-unit pass 2026-07-16** (all 32 GC-twinned asm touched; 19→25). +6 this pass: InitCrates (NuSpecialFind loop; non-void-return lever pushes loop-cmp off `$v0`; CRATEGROUPCOUNT/CRATECOUNT pair swap), OpenPreviousCheckpoints (`crate->iRAIL` GCSE-reused into FurtherBEHIND), DestroyAllNitroCrates (nested CrateOff + kaboom), CrateBottomAbove (flip `pos.y>crate.y`→`crate.y<pos.y` for load order), CrateRayCast (min/max ternaries + RayIntersectCuboid), HeightSortCrateData (struct-swap bubble sort). Also corrected CrateOff to return s32 (DestroyAllNitroCrates re-verified). Near (kept asm, 77-94%, structurally exact): ReadCrateData 92.5 (drops NuFileExists; version-check slti/5-vs-slt/4 cascade), InCrate 92.4 (FP clip regalloc +1 shift), CrateTopBelow 94.4 (D_0062D7A4 in f21; prologue/LICM order), NewCrateAnimation 90.6 (qrand FP load-order), ReadInCrateData 77.8 (goto trigger-link tail), AddExtraLife (call-structure exact; u64 0 wants callee-saved reg; also fixed GPREL16: `extern u64 D_[]` for far data). **Assembler wall (5): HopCratesAbove/CrateOnTop/CrateAbove/CrateBelow/CrateSafety** — `.p2align 3` inner-loop padding (decompals-as emits 2 nop words before the `+0.5f` hot loop that retail SN-as did not; isolated-assembly-proven; one `_sonyize`/gen_hybrid fix unblocks all 5). GetCrateType 58 (movz/movn+beql tie). Reconstructions <12% (logic faithful, codegen-divergent, escalation candidates): GotoCheckpoint, UpdateCrateExplosions, DrawCrateExplosions, ConvertCrateData (pointer-idiom loop), RayIntersectCuboid 56, AttackCrate/BreakCrate (PS2 **inlines** StartExclamation+DestroyAllNitroCrates). Deferred (dedicated reconstruction, 1-4KB): AddQuad3DrotXYZ (GC render callee `NuRndrTri3d` not a PS2 symbol), AddCrateExplosion (PAL .lit4 consts), CrateBounceReaction, CrateOff, DrawCrates, ResetCrates, UpdateCrates. Structs: BoxExpType colbox[2]@0x4/BoxPol[]@0x1C, BoxPol ang[3]@0/angmom[3]@0xC int + pos@0x18/mom@0x24 float + rndfade@0x30; crateeditor_s (nuhspecial obj@0,name@0xC,character@0x10). |
| `game/main`    | 8         | 7       | —     | —       | —        | +4 recent (parallel): MiniGame, MiniGameRender (empty `jr $ra` stubs); CreateFadeMtl (256B — NuMtlCreate(1)→D_00633260, seven GS-reg bitfields in a `union{u64; struct fadebits;}` written max-value in emission order so gcc folds to plain OR and reproduces the register tie-break; else the mask form mis-allocates $t2 vs $v1 — see notes.md); UpdateFade (152B, fadeval clamp [0,255]+fadehack, packs D_00633264=v|v<<8|v<<16|0x80000000, first attempt). Near: BGLoadSfxFn (68.4%, 104/152 — bytes 0x0-0x4c exact via entry-anchored `msg`/`done` locals fixing frame=48/reg_mask=0x80030000; blocked by NuGScnRead call-delay-slot fill, `delay-slot-reorg-fill`, source-invariant; stays asm). Prior matches (DoInput etc.) predate the Wave A table |
| `game/game`    | 68        | 61      | —     | —       | —        | **Full-unit pass 2026-07-16** (68 of 108 GC-twinned asm touched; 14→61 matching). +47 this pass. Levers proven here: **gnu89 plain `inline`** (emits standalone AND inlines — 13 fns incl. AddMaskFeathers→LoseMask, qrand, HubFromLevel, ResetTimer); **signed-vs-unsigned field type steers constant materialization** (`s32 playing &= ~1` → `addiu -2` 1 word, u32 → `lui+ori` 2 words, ResetHGobjAnim; `u32 isec` → `sltiu` not `slti`, UpdateTimer); retail-const-struct → `extern struct T D_xxxxxxxx;` + assign (initialised locals blow the frame; took DrawTempCharacter 18.6%→100%); explicit loop-invariant FP local → callee-saved `$f20` (DrawMaskFeathers frame+freg_mask); `x ^ 1` not `1 - x` when x is provably 0/1 (sltu folds the bitfield mask); far pointer scalar → `extern T *world_scene[]` + `[0]` (else GPREL16 overflow, InitSplineTable). **Blocked (21):** 7 `toolchain_failure` — AddAward (`.p2align 3`, groups with the 5 game/crate cases → one fix unblocks 6); AddMaskFeathers/LoseMask/DrawParallax/UpdateTimer (mtc1-hazard **widening** evidence, see the 2026-07-16 subsection above); NewMask; HubSelect. 13 `no_progress` (regalloc/scheduling ties), 1 unknown (InitLevel). **Open near-matches kept in tree (state=asm, NOT blocked — no wall found):** ResetVehicleControl 69.85% (one word: loop1 wants `done=1` in the `bnez $s3` delay slot @0xd4), AddKaboom 54.43% (`goto judder` shared-tail confirmed correct; case-4 arm insn-identical to retail modulo the `%hi` temp landing in `$2` not `$a0`). **40 untouched** — deliberately NOT blockered: no bytes read, so no wall claim is honest. Full per-function detail, attempt logs, dead levers and banked decodes: [gc_game_handoff.md](gc_game_handoff.md). GC drafts confirmed HOSTILE 5×: UpdateTempCharacter (GC `oldpaddata&0x840` vs PS2 `paddata(+0x55C)&0x800`), Draw3DObject (GC dropped the 1/2/3→0x85/0x86/0x87 dispatch AND mis-nested the render inside the `Level!=0x25` guard), BonusTiming/MakeMaskMatrix/UpdateTimer (60Hz consts). PAL divergences: UpdateTimer itime+=6 (GC 5), WumpaHitTerrain 50.0f (GC 60.0f), TransporterGo NewBuzz(…,5) (GC 6), BonusTiming delay 5 / 0x4B=75 (GC 6 / 90). Prior matches (qrand etc.) predate the Wave A table |
| `game/game_obj`| 17        | 16      | 8     | 8       | 0        | +5 recent (parallel): PickupPower (216B, `switch(type)` 0xA2..0xA7→jtbl, `Game[0x406]|=1<<new_power`, NewMenu/GameSfx, trailing mom.x/mom.z/slide triple-store rotated-right → source order `mom.x,mom.z,slide` cancels to retail `slide,mom.x,mom.z`), AddGameObject (256B, RemoveGameObject counterpart: memset 0x188+7 inits, slot-find loop, inline CountGameObjects; FP $f0/$f1 order fixed by assigning reflect_y last, found-block+1.0f-pair store swaps), ResetProjectiles (184B, 16-entry loop w/ explicit `projectile_s *p` induction stride 0x1B0 — array index overran; inline RemoveGameObject body), PickupCrystal, PickupCrateGem (pickup handlers: `plr_lives_s.count=1`/`plr_items|=bit`; AddPanelDebris EABI split f12/f13/a0/f14/a1; reorderable counter-vs-D_-byte store pair emits reversed). Near: PickupBonusGem (87.5%, `s32 mask` param — jal GameSfx delay-slot fill not source-steerable, stays asm). 10 helpers on the pre-existing scaffold (KillPlayer already matched; PlayerCreatureCollisions kept as near-match, untouched). Clear/Count/Remove GameObjects (pObj[64], GAMEOBJECTCOUNT; RemoveGameObject inlines CountGameObjects via gnu89 `inline`), New/OldTopBot (objbot@0x110/objtop@0x114/oldobj*@0x120/124; pos.y-first add.s), KillItem (parent@0x0->on/off_wait), CylinderCylinderOverlapXZ (p0,r0,p1,r1), ResetGameObject (memset 0x188 + reflect_y=2e6/scale/SCALE=1). PAL divergences: GameObjectRadius r*3.0f (GC 2.5f), FlyGameObject mom.z=0.1999999881 (0x3E4CCCCC, GC 0.1666667×60/50). Deferred: GetDieAnim (nested qrand cond), big collision/projectile state machines |
| `game/ai`      | 7         | 5       | 4     | 1       | 0        | Shared-enemies set. +1 recent (parallel): DrawDRAINDAMAGE (208B, guard D_00633310&&D_00633300, stack nuvec base+NuTrigTable[idx]*3.0f per axis, Draw3DCharacter sig from bug.c; reused D_00633308[3] qrand indices via `(u16)` casts; D_006B7620 anim_s). Near: ResetCRUNCHTIME (81.5%, retail CSEs two -1 stores as lui+ori in $v1 kept live across the loop-counter setup — signed -1 rematerializes as addiu, unsigned reorders the stores; scheduling wall, stays asm), WaterCrunchFunction_Punch1 (53.7%, loop tail/epilogue byte-exact after `<` bound fix; residual is whole-fn callee-save regalloc permutation of 8 tied loop-invariants; stays asm). Prior: InitAI/FindNearestCreature/FindAIType/FindAILabel matched (predate Wave A) |

Notes:
- `game/bug` (5/6): InitBugAreas, InBugArea, ResetBug, AddBugLight, DrawBug all
  byte-exact. Heavy extern-D_ data unit; callee signatures (Draw3DCharacter,
  NuLightAddSpotXSpanFade) re-derived from PS2 call sites (EABI split int/float
  regs). InBugArea needed the `(Rail+i)->type` offset-first regalloc lever.
  UpdateBugLight (2224B, FP-compare heavy) was attempted 2026-07-19 and is now a
  faithful near-match kept at `state=asm`: a substantial PAL rewrite reconstructed
  from asm (GC inline consts are gp-rel named globals D_0062E914…/D_00632CD8…;
  extra mech/tail wobble-lerp; PAL 50Hz immediates verified). Structurally exact
  (264/2224), blocked by three proven walls at once — delay-slot-reorg-fill,
  mtc1-hazard-nop (7 sites: 0x25f200/2ac/450/54c/584/89c/8b8), and a whole-fn
  callee-save regalloc tie ($f25 vs $f26).
- `game/cloudfx` (7/10, updated 2026-07-19): the 4 small tail fns (CloseClouds
  empty-stub, cloudProcess, TimeTunnelClose, CloudFxInit) rebuilt from asm;
  InitClouds matched with two PS2 divergences from GC (no `srand`, loop count 10
  not 20). **+cloudInit (448B) and +DoClouds (1096B)** matched from asm — the GC
  render twins are structurally useless (PS2 rewrote cloudfx for the console HW).
  cloudInit: RNG loop into s16 `clouds[]`, 64-bit material-attrib bitfield edit
  (`ld/sd` at +0x168, `dsll` masks), FP scale is 16.0f not 8.0, `clouds[i]`
  subscript (not pre-loop ptr) so the IV base lands in the preheader. DoClouds:
  trig-drift/wrap loop, drops NuCameraEnableClipping, `D_00632904` disable flag,
  calls cloudRender with 7 int + 2 float EABI args (reorder the float params
  earlier in the signature to match arg-load scheduling — int/float arg registers
  count independently). Near-matches kept asm: cloudRender (1264B hand-written
  VU1/GIF/DMA packet builder, whole-fn callee-save regalloc wall) and
  TimeTunnelInit (24.7%, 3-way $s1/$s2/$s3 permutation). TimeTunnelRender (1920B,
  DoClouds-like driver) analyzed and deferred as the best next-session target.

- `game/font3d` (5/7, updated 2026-07-19): CombinationCharacterBC/BD (string-pair
  scan over extern char tables), Reset3DFontObjects. Reset3DFontObjects diverges
  from GC -- PS2 computes a randomized
  `anim_time = qrand()*rate*(anmdata[action]->time - 1) + 1` instead of `1.0f`;
  matched by dropping `volatile` (CSE the action load) and forcing both
  loop-invariant FP constants (1.0f, D_0062E350) into ordered explicit locals so
  the save-mask and preheader order match. **+InitFont3D (440B)** matched (ported
  from the GC twin, every offset re-derived: Font3DTab ascii@0/obj{scene@4,
  special@8}/name@0xC stride 0x10; Font3DAccentTab obj{scene@0,special@4}/name@8
  stride 0xC; nugscn_s mtls@0x8/nummtl@0xC; numtl_s special_id@0x1C6;
  Font3DMtlTab[5][2]; &Tab[i].obj / &MtlTab[i][1] reproduce as base+addend).
  **+RemapAccentedCharacter (368B)** matched (GC twin verbatim; 110-entry
  jtbl_00620510 borrowed by structure, switch on `(s8)((u8)*c - 0x80)`, min case
  −128). Near-matches kept asm: Update3DFontObjects 92.6% (whole-fn FP tie —
  retail `rate` in $f3 / `1.0f` in $f2, candidate swaps; 4 spellings → identical
  hash 402231ec) and Text3D (2248B; GC twin badly misleading for PAL v1.03, every
  divergence decoded from asm: swapped alignment masks vert=`align&5`/
  horiz=`align&0xA`, extra `'p'` cases, general scale-1.0 set `{x,t,o,s}`, no
  safe-area/pulsate/rotate, magic floats gp-rel D_0062E358/35C/360, Comb*BD/BC
  inlined) — from-asm base at 0.7%, needs a dedicated multi-iteration budget.

New reusable levers recorded in memory `gc-migration-listman-lessons`:
empty-stub / dropped-call / PAL-loop-count divergences, `(arr+i)->field`
offset-first regalloc fix, mul.s operand order, EABI split arg registers,
ordered-explicit-locals FP-constant hoist, drop-volatile CSE.

- `game/sfx` (6/12, updated 2026-07-19): ResumeGameAudio/ResetGameSfx/
  UpdateGameSfx/GameMusic/GameSfxLoop byte-exact. PS2 divergences from GC:
  ResumeGameAudio calls NuSoundSetChannelPitch(4,0x75A,0) not NuSoundResumeSfx;
  PauseGameAudio fully rewritten (2 channel loops); global SFX count 0xB1->0xC5
  and local base -0xB1->-0xC5 (PAL); GameMusic/GameSfxLoop dropped the
  strcpy(sfxpath,...) and GameSfxLoop dropped the NuSoundPlayLoop else.
  New lever: **pointer do-while + `(s32)` cast** for a signed-`slt`, no-entry-
  guard, no-reversal, single-pointer-IV loop (UpdateGameSfx global loop). Store-
  permutation lever reused: pre-rotate the trailing `gamesfx_*=-1` cleanup source
  left so gcc's right-rotation lands on retail order.
  **+TestLocalSfx (112B)** matched from asm (no twin): infinite loop of
  `InitLocalSfx(table, count)` + `NuSoundKillAllAudio()` over three externed level
  tables (D_005E99C0/0xD0, D_005E9FC0/0xD2, D_005E9120/0xCE), `soundtestcount++`.
  **Data-from-C reclassified 2026-07-19:** the level tables and SFX strings are
  retail-owned data referenced as `extern D_...` (the ai.c pattern) — **not**
  data-from-C blocked. InitGlobalSfx and InitLevelSfxTables are now faithful
  near-matches (do-while + separate `base` ptr → correct frame 112 / reg_mask
  0x803f0000; LData mapping fully decoded, ldata_s pSFX@0x1C/nSFX@0x20 stride
  0x54); residual is pure gcc store scheduling / $s0↔$s1 numbering, not
  source-steerable. Only **InitLocalSfx** is genuinely blocked (two
  `switch(language)` selectors emit jtbl_006286A0/C0 into .rodata = real
  data-from-C). Near-matches: **PauseGameAudio** (8B, 2nd-loop compare-reg/
  branch-form regalloc coin-flip); **GameSfx** (structurally exact, blocked by the
  documented decompals-`as` mtc1->add.s hazard-nop omission from `dist2+2.0f<dist1`
  @0xcc). **GameAudioUpdate** deferred (1040B, soft-float dp abs, gp-rel PAL fade
  consts D_0062E788/78C, same mtc1-nop wall class).
