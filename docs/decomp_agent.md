# Decomp-agent: shared matching-decompilation workflow (canonical)

This is the **cross-client** workflow for the matching MCP server. It applies
equally to Claude Code, OpenAI Codex (CLI or IDE), and any standards-compliant
MCP client. Client-specific launch/escalation notes live in
[CLAUDE.md](../CLAUDE.md) (Claude Code) and [AGENTS.md](../AGENTS.md) (Codex);
everything shared lives **here**, and this file wins on policy.

## What it is

Two repository-local layers:

- **`tools/decomp_agent/`** - a deterministic, model-independent domain layer.
  It works without MCP and without any LLM: it reads the committed registries,
  ranks candidates, builds context, drives the *existing* locked toolchain for
  compile/diff/verify, records shared sessions and blockers, and delegates
  promotion to `tools/promote.py`. Every operation is rooted through an explicit
  `DecompProject(root)` and returns JSON-compatible dicts.
- **`tools/decomp_mcp/server.py`** - a thin FastMCP stdio server that validates
  inputs and calls the domain layer. It contains **no** provider- or
  model-selection logic.

The domain CLI (`python -m tools.decomp_agent.cli`) exposes the same operations
for scripting and is also how toolchain-bound work is re-invoked inside the
container.

## Source-of-truth hierarchy

1. committed registries + status manifests (`config/pal103/…`)
2. **the retail target disassembly** (`get_disassembly` / the `target-asm`
   resource / `asm/text.s`) - the ground truth for a function's own bytes
3. fresh compiler / assembler / linker / byte-verification output
4. current hand-written C and committed headers
5. maintained docs (this file)
6. comments, unverified hypotheses, **and any cross-platform reference
   decompilation** (e.g. a GameCube/PC port)

A source comment never overrides `profiles.toml`, `status/`, `functions.toml`,
or `units.toml`. The effective compiler is resolved from the unit's status
manifest (if any) else the category default - never from a comment.

### Reference decompilations are hints, never authority

A decompilation of the same game on **another platform** is rank 6 - a hint for
names, call order, and rough structure only. A different compiler and a
different CPU produce different codegen: branch directions, dispatch constants,
immediate materialization, struct offsets, and even control-flow shape all
differ. **Reconstruct C from the target disassembly and verify every one of
those against it; never transcribe a port's control flow.** `get_context`
returns a `ground_truth` block on every call (and inlines the listing for
matchable-size functions) precisely so this cannot be skipped. In this repo,
trusting a GameCube reconstruction of `DrawPanel3DObject` produced a wrong
object-dispatch (`object >= 0` recursion) that stalled near 0%; reading the PS2
disassembly (`1/2/3 → 0x85/0x86/0x87`, fall-through render) matched it in one
iteration.

### Do not give up before reading ground truth

Never record a blocker or end a session `BLOCKED_*` without first retrieving the
target disassembly for the function (`get_disassembly`). "It didn't match from
the reference" is not a blocker. A blocker is a *proven* pipeline limit visible
in the disassembly or the compile output - e.g. an SN-`as` hazard nop the
decompals `as` does not reproduce (`assembler_normalization`), or C data
the pipeline cannot own yet (`bss_common`, `data_section_mismatch`). Jump
tables, `.lit4`/`.lit8` constants and `li.d` GPR pools are supported (gen_hybrid
borrows the retail slot; `config/pal103/compiler_knowledge.toml`) and are not
blockers. Record a blocker with the specific kind and evidence; keep iterating
on everything else.

## Canonical identifiers

Always prefer the canonical function id:

```
pal103:unit-0007:00105a60:NuListGetNext
```

A bare name that is duplicated across translation units (four disambiguated
statics) resolves to **all** candidates - the tools return ambiguity and never
choose. Retry with a canonical id.

## The loop

1. `project_health` - is the repo ready? (never builds; returns repair commands)
2. `list_candidates` - deterministic ranked targets (small, leaf, C-unit, unit
   momentum). No LLM, no embeddings; every ranking carries its reasons.
3. `resolve_target` / `get_context` - authoritative dossier, split into
   **facts / ground_truth / classifications / hypotheses / unknowns**. The
   `ground_truth` block points at (and, when small, inlines) the target
   disassembly; `get_disassembly` fetches it in full. Read it before writing C.
   Missing types and prototypes are reported as unknowns, never guessed.
4. `start_session` - open a shared, resumable session for the target.
5. Write C in `src/<unit>.c` (your client's normal editing tools - the MCP does
   **not** edit files, except the guarded best-candidate restore).
6. `compile_diff` - compile the unit through `tools/cc.py` and byte-compare over
   the **full canonical extent**. `exact = true` means byte equality over the
   whole extent, not a matching prefix. objdiff percentage is never proof.
7. `checkpoint_session` - record every experiment (hypothesis, hashes, diff
   signature, classifications). `classifications` takes `compile_diff`'s
   classification records as returned, or bare category names such as
   `"register_allocation"`. Repeats and oscillation are detected for you.
8. Iterate. Stop when a stop condition fires (below).
9. `verify_candidate --level function` - independent full-extent byte check.
10. `promote_matching` - delegates to `tools/promote.py` (the sole writer of
    `state = matching`; it verifies and rolls back on failure). The MCP never
    edits a manifest and never sets `equivalent` - equivalence is a human
    review decision.

If blocked, `record_blocker` with a structured kind so the obstacle is not
rediscovered. Only *architectural* kinds (compiler-owned data, `.lit8`, ...)
drop a function from candidate ranking; `architectural` overrides the kind's
default. An otherwise exact function that only the assembler lays out
differently (a hazard nop, an unfilled delay slot) is
`assembler_normalization` - temporary, since the fix belongs in the hybrid
normalization, not the C.

## Stop conditions (recommended)

- initial budget **8** attempts; hard maximum **20**
- extend past 8 only while the best result is still improving
- stop after **4** non-improving attempts
- stop after **3** consecutive compile failures
- stop when a candidate-assembly hash or diff signature repeats with no new
  hypothesis, or results oscillate between recorded states
- stop immediately on a proven architectural blocker
- stop if the compiler/profile fingerprint changes mid-session
- restore the best safe checkpoint before ending

`checkpoint_session` and `resume_session` return a `stop` recommendation and a
capability-based (never model-named) `escalation` hint. Outcomes
(`end_session` matches them case-insensitively):
`MATCHING`, `BLOCKED_TOOLING`, `BLOCKED_MISSING_CONTEXT`,
`LIKELY_EQUIVALENT_REVIEW_REQUIRED`, `NO_PROGRESS`, `BUDGET_EXHAUSTED`,
`STALE_SESSION`, `SOURCE_CONFLICT`.

## Shared cross-client sessions

Sessions live in `build/pal103/agent_sessions/` (gitignored) as atomic JSON
ledgers - independent of any conversation. **Claude Code can start a session
that Codex resumes, and vice-versa.** A single-writer lock (naming the owning
client) prevents concurrent writers; stale locks (older than 2h) are detectable
and recovered only through the explicit `recover_session_lock`. Client/model
metadata on a checkpoint is diagnostic only - it never affects byte
verification, ranking, or promotion.

Optionally, a client may pass `input_tokens` / `output_tokens` on
`checkpoint_session`. These are opaque diagnostics: the ledger accumulates them
(overall and per client / per model) and `end_session`, `resume_session` and the
session summary report a `token_usage` total, so you can see **how many tokens a
session cost** (and which client/model spent them). Token counts never affect
byte verification, ranking, or promotion.

Hand-off pattern: the current owner calls `end_session` (releasing the lock);
the next client calls `resume_session` and receives the best attempt, all prior
hypotheses, seen hashes/diff-signatures, encountered mismatch categories, failed
experiments, blockers, remaining budget, and the escalation recommendation.
`resume_session` **refuses** when the repo revision or profile fingerprint
changed, the target manifest changed, the source has unrelated edits, or a
foreign writer holds the lock.

## Model & reasoning independence

The MCP never chooses or switches models and never sets reasoning effort. It may
*return* a capability recommendation:

```json
{"recommended": true, "capability": "deep_reasoning",
 "reason": "remaining mismatch appears register-allocation/scheduling sensitive",
 "remaining_attempt_budget": 12}
```

and it *records* optional client-supplied diagnostics
(`{"client": "...", "model": "...", "reasoning_effort": "..."}`) as opaque
metadata. Escalation between a worker configuration and a stronger one is a
client-side decision (see CLAUDE.md / AGENTS.md).

## Running toolchain-bound operations

`compile_diff`, `verify_candidate`, `promote_matching`, and `compiler_probe`
need the locked toolchain. On a host without it (e.g. Windows) they are
transparently routed into the dev container via `tools/dispatch.py`; inside the
container they run in-process. This is automatic - callers just invoke the tool.

## Security boundary

The MCP exposes only typed, domain-specific tools. There is **no**
`run_command`, `read_file`, `write_file`, `set_compiler_flags`, or
`edit_manifest`. Compilation always goes through `tools/cc.py`; promotion always
through `tools/promote.py`. No arbitrary path, flag, or shell interface exists,
and no existing byte gate is weakened.

### Data and global-symbol policy

Retail data is owned per symbol. The data registry
(`config/pal103/data_symbols.toml`: every named data symbol with its unit,
section, `.symtab` size and, for the `sce` units, its STABS type) and the data
map (`config/pal103/data_map.toml`: each unit's owned byte ranges) say what
belongs to which unit; a unit's status manifest lists its symbols under
`[[data]]` (`asm` | `matching` | `retail`).

C defines a global only when all of this holds:

- the unit is fully matching (every `[[function]]` is `matching`);
- it is a registry symbol of that unit, defined under its retail name and
  storage: `static` where retail is static, `const` for `.rodata`, a function
  static inside the function that references it;
- its section is `.rodata`, `.data` or `.sdata` (under `-G8` every object of
  at most 8 bytes is small data, addressed gp-relative; a zero object needs an
  explicit `= 0`, or it becomes COMMON), or `.sbss`/`.bss` for an
  uninitialised object (no initialiser) of a unit that is `complete`;
- `promote_matching` (tools/promote.py) verified it: its bytes over
  `[addr, addr+size)` and its linked address equal retail.

Everywhere else - partially matched units, COMMON -
reference retail data with registered names or `extern D_...` symbols taken
from the PS2 disassembly, `target-asm`, `asm/text.s` or
`config/pal103/symbol_addrs.txt`, with readable aliases:

```c
extern struct nulsthdr_s *D_006309FC;
#define sceneinst_pool D_006309FC
```

A unit may also `extern` one of its own registry statics under its retail
name, declared with the registry size (`extern struct numtx_s vmtx;` in
nucamera); gen_hybrid renames it to the retail `D_<vram>`. GCSE hashes symbol
names, so the retail spelling can decide register ties.

Compiler-private data (`$LC` strings and aggregates, jump tables, `li.d`
pools) and the unit's `.lit4` pool are borrowed retail slots until the unit
is `complete`; then its hybrid runs in whole-unit mode and emits them from C
(below), and so are its `.sbss`/`.bss` objects. A raw binary asset (a font
bitmap) keeps its retail bytes with `state = "retail"` and a `reason`, set by
hand like `equivalent`. A small (`.sdata`/`.sbss`) symbol the C still reaches
through `extern` keeps its retail address and its gp-relative loads (the
hybrid declares it `.extern NAME, size`).

If compile_diff reaches 100% but the hybrid build fails with "data section in
compiler output not supported yet", a file-scope definition slipped into a
unit without a `[[data]]` table: replace it with the extern form above
(or, in a fully matching unit, do the data work below).

`complete = true` is written only by `promote.py --complete <unit id>`, after
the verifier proved: every function and data entry is done, and its
whole-unit hybrid supplies every range the unit owns, carries no retail bytes
in them beyond its `retail` entries, borrows nothing, and links every piece
with file bytes equal to retail and every `.sbss`/`.bss` object at its
address.

Whole-unit mode (every `complete` unit): gen_hybrid lays the unit's data out
the way the assembler did - each data section in retail function order (an
object travels with the function it is emitted for), one `.lit4` slot per
pool-bound `li.s`, one 8-byte `.rodata` literal per `li.d $gpr` that is not
built inline, nothing merged - and anchors each section where the code's and
the data pointers' retail references put it. Bytes it emits past the unit's
own ranges (a jump table or strings after its last code reference) lie in
data-map gaps; `promote.py --complete` records them as `[[claim]]`
(`section`, `start`, `end`), and the image link takes each claimed span from
its claimant's hybrid (several units may claim spans of one gap; the
unclaimed bytes stay retail). Re-run
`--complete` after a C change moves them. The unit's `.sbss`/`.bss` objects
come from C too (pieces without file bytes): ee-gcc writes an uninitialised
static as label, `.align`, `.space` - a function's statics before its code,
the file-scope ones at the end of the unit in declaration order - and an
uninitialised global as COMMON (retail has one, newlib's `errno`, which the
hybrid places like a static). The last static of a unit's run is unsized in
retail, so it lies in a data-map gap and becomes a claim. An object only
this layout can place (every `.sbss`/`.bss` object, an unsized symbol past
the unit's ranges such as bitconv `block_table32.83`) is promoted by
`promote.py --complete` together with the unit. Typical C work to get there: string
literals instead of `extern char D_...[]`, float literals instead of
`extern f32 D_...` `.lit4` slots, definitions placed before the function
retail emits them with (a function static goes out when its function is
parsed, an `inline` function's statics and literals where it is defined or
first inlined; a non-`static` `inline` function's own code goes to the end of
the unit, which is how retail ends up with functions whose data sits far
from their code), and the literals of dead statements kept when retail has
them (GCC 2.95 emits a string constant even for unreachable code).

### Per-unit data workflow

Preconditions: every `[[function]]` of the unit is `matching`; the unit is in
no open `decomp/*` PR or running batch.

0. Branch off an up-to-date `main`; `python tools/gen_ninja.py`; on the base,
   `python tools/check_codegen.py --build --snapshot` (wait for it).
1. `list_data_candidates` / `get_context <data id>`;
   `python tools/promote.py --init-data src/<unit>.c` adds the `[[data]]` table.
2. Types, in this order: STABS (the registry `type`) -> the shared headers and
   the extern types the matched C compiles against -> GC DWARF names (only when
   the size equals `.symtab`) -> otherwise stop (`missing_type_layout`).
3. Initialisers: `python tools/dump_data.py <data id> [--type '<C type>']`
   (`--probe` for a struct type, `--verify` to check float literals).
4. Replace the `extern D_` + `#define` alias with the definition under the
   retail name, in address order, `static` where retail is static, function
   statics inside the function the code references them from, `const` for
   `.rodata`; rename the alias's use sites inside the unit. Other units keep
   their `extern`. Raw binary assets get `state = "retail"` + `reason` by hand.
5. `compile_diff <data id>` until exact.
6. `python tools/promote.py <data ids>` (run it in the background; it is long).
7. If the unit qualifies, `python tools/promote.py --complete pal103:unit-NNNN`.
8. `python tools/check_codegen.py --build --compare`: only this unit's objects
   differ (data pieces; `.text` identical), 0 elsewhere; `ninja check`,
   `status.py --check`, `check_types.py --check` (update the census if the
   extern counts moved), unit tests, CRLF byte check.

Order by section: `.rodata`, `.data`, `.sdata`; `.sbss`/`.bss` objects get
their definitions with the rest and are promoted by `--complete`. Never in
this workflow: compiler-private data and the `.lit4` pool in partially
matched units (they stay borrowed until `--complete`).

### Inline assembly

Inline asm only where C cannot express the instruction: COP0 and privileged
(`mfc0`/`mtc0`, `di`/`ei`, `sync`, `cache`, `syscall`), COP2/VU0 macro mode
(`cfc2`/`ctc2`, `qmfc2`/`qmtc2`, `lqc2`/`sqc2`, `vcallms`, `v*`), `multu`
into `hi`/`lo` (libgcc2's `umul_ppmm`), literal delay `nop`s, and `.p2align`
where retail aligns a loop gcc does not (a goto loop in `DIntr`). That code
lives in the SDK runtime (`src/sdk/`) and the two engine units that drive the
VU directly (`src/game/vu.c`, `src/nups2/vu1man.c`). Write the rest of the
function in C and keep each asm statement to the instructions that need it;
copy upstream spellings (`longlong.h`) verbatim. A function that retail has
entirely hand-written (labels in delay slots, no compiler shape) is one asm
body. Never use inline asm or `register asm` variables to steer scheduling or
register allocation.

The 22 units retail assembled from `.s`/`.S` sources (the `.mdebug` file name
says so: `crt0.s`, `klib.s`, the r5900 newlib string routines, the engine's
`*vu0.s`, `asmmem.s`, `asmdma.s`, `asmgobj.s`, `asmstrm.s`, `numath.s`) are
`src/<unit>.c` files of file-scope asm, one block per function
(`.text`/`.globl`/`.ent`/`.end` inside, so gen_hybrid sees a function and no
compiler epilogue is added; `.text` because gcc leaves the section of the data
it emitted first). The engine units' `.data` (`vec_none` ...) is one asm block
too, `.data`/`.type`/`.size`/label/`.float` as the `.s` source wrote it: ee-gcc
aligns every C aggregate to 8, and two of these units place it 4-aligned. Write GPRs as numbers (the SN and Sony assemblers reject
`$ra`) and the VU0 operands `ACC`, `Q`, `I` bare (gen_hybrid adds the `$` the
decompals `as` needs). An instruction the assemblers cannot encode (`vclipw`,
`vsqrt`, the EE `sqrt.s`) and a short-loop branch SN's `as` would pad with a
nop retail lacks are `.word`s with the instruction in a comment.

### Types and shared headers

Every type shared by more than one unit is defined once in `include/`, with a
layout that also holds on a 64-bit host; the type census keeps duplicate
structs and typedefs at 0, so a copied definition fails `validate.yml`. If a
header in `include/` already defines a struct, include it instead of copying
the struct into the unit. A type that only one unit uses may stay in that unit.

Layout of `include/` (the engine's own module structure):

| Path | Holds |
|---|---|
| `include/nu_types.h`, `nu_stdarg.h`, `nu_libc.h` | platform layer only: fixed-width types (`u8`..`s64`, `u128`, `f32`, `nuintptr`), varargs, libc |
| `include/<lib>/<lib>types.h` | structs shared by several modules of one library (`game/gametypes.h`) |
| `include/<lib>/<module>.h` | one per `src/<lib>/<module>.c`: that module's structs, `extern` globals and prototypes |

Include them by path (`#include "nu3d/nugscn.h"`, `#include "nu_types.h"`);
tools/cc.py stages the tree and passes `-I.`. A module header is created when
something needs it, not as an empty skeleton. A module's `.c` includes its own
header, and callers include it instead of declaring the functions locally.
Types are spelled by their struct tag (`struct numtx_s`), not through typedef
aliases.

Four tools gate this work:

- `python tools/check_types.py` counts duplicate structs and typedefs, `long`,
  varargs hacks, byte-offset casts, `sizeof` pads, local libc prototypes and
  conflicting externs/prototypes (`--metric <name>` lists the sites).
  `validate.yml` runs `--check` against `config/type_census.toml`, which fails
  when a count changes in either direction; record the change with `--update`
  in the same commit.
- `python tools/check_codegen.py --build --snapshot` on the base, then
  `--build --compare` after the change, proves a refactor changed no code in
  any `current`, `matching`, `equivalent` or `report-current` object
  (`ninja check` only covers `matching` functions).
- `python tools/dispatch.py python tools/probe_layout.py [--check] [header...]`
  compiles `sizeof`/offset probes of every commented struct member with the
  matching compiler and reports each `/* 0x.. */` (or `// Offset: 0x..`,
  `}; /* size */`, `// Size: 0x..`) comment that disagrees, plus headers that
  do not compile on their own. Legacy root headers are reported but do not
  fail `--check`.
- `python tools/dispatch.py python tools/merge_views.py <tag> [alias...]`
  measures every local copy of a struct in its own file (bit-fields included,
  by bit position) and prints the sizes, the distinct layouts and the field
  names that differ at the same offset: the starting point for unifying a
  struct.

Rules for type refactors:

1. Moving or renaming types, externs or prototypes changes no code:
   `check_codegen.py --compare` ends at 0.
2. Offsets and sizes of a struct moving into a shared header are measured with
   the compiler (`probe_layout.py --check`), not taken from the comments. Every
   header compiles on its own: it includes what it uses. New headers write
   offsets as trailing `/* 0x.. */` and the size as `}; /* 0x.. */`.
3. A unit's local copy of a tag is removed in the same commit that includes the
   header defining it.
4. Merging two views of a struct is a type decision (`s16`/`u16`, pointer/`u32`,
   `s8`/`u8`, a bit-field's declared type change code); line them up by offset
   and let the codegen check decide. Names come from the GC DWARF dump first,
   then the module that owns the struct, then the majority. Where retail
   provably uses the same bytes two ways (access widths in the target
   disassembly), model it with a union rather than picking one view.
5. Where retail provably used an implicit declaration or a mismatched
   prototype, the PS2 build keeps it. `main` holds no host code: the NuWorld
   port's `NW_HOST` guards and `nu_port.h` seams are patches in its own
   repository (docs/pipeline.md, "NuWorld").
6. Headers hold types, `extern` declarations and prototypes only, and no
   alias `#define`s (`#define fseed D_0062EA68` would rename every `fseed` in
   every includer); the readable alias stays in the `.c` file.
7. `long` is 8 bytes under both compilers (pointers are 4): replace it with
   `s64`/`u64`, never `int`.
8. Keep prototypes of `inline` functions out of module headers: GCC 2.95 emits
   the out-of-line copies of `inline` functions in first-declaration order, so
   a header prototype reorders the unit's code. Their callers keep a local
   declaration.

## CLI quick reference

```bash
python -m tools.decomp_agent.cli health
python -m tools.decomp_agent.cli candidates --limit 10 --max-size 128
python -m tools.decomp_agent.cli context pal103:unit-0007:00105a60:NuListGetNext
python -m tools.decomp_agent.cli compile-diff pal103:unit-0007:00105a60:NuListGetNext --session <sid>
python -m tools.decomp_agent.cli session start pal103:unit-0007:00105a60:NuListGetNext
python -m tools.decomp_agent.cli session resume <sid> --client codex
python -m tools.decomp_agent.cli verify pal103:unit-0007:00105a60:NuListGetNext --level function
```

All accept `--repo`, `--version`, `--json`, before or after the subcommand.
