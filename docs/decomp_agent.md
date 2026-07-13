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
5. maintained docs (`docs/notes.md`, this file)
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
in the disassembly or the compile output - e.g. the function owns a `.rodata`
jump table (`compiler_owned_rodata`), or needs a `.lit8` double pool. Record
those with the specific kind and evidence; keep iterating on everything else.

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
   signature, classifications). Repeats and oscillation are detected for you.
8. Iterate. Stop when a stop condition fires (below).
9. `verify_candidate --level function` - independent full-extent byte check.
10. `promote_matching` - delegates to `tools/promote.py` (the sole writer of
    `state = matching`; it verifies and rolls back on failure). The MCP never
    edits a manifest and never sets `equivalent` - equivalence is a human
    review decision.

If blocked, `record_blocker` with a structured kind so the obstacle is not
rediscovered and candidate ranking can skip it.

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
capability-based (never model-named) `escalation` hint. Outcomes:
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

The current matching pipeline is function-first. It verifies C-generated `.text`
for functions, but general Data-from-C is not supported yet.

Until Data-from-C verification exists for a specific range:

- Do not define file-scope C globals in `src/**/*.c` if they emit `.data`,
  `.sdata`, `.rodata`, `.sbss`, or `.bss`.
- Reference retail-owned data with `extern D_...` symbols taken from the PS2
  disassembly, `target-asm`, `asm/text.s`, or `config/pal103/symbol_addrs.txt`.
- Give readable names with aliases, not C definitions:

```c
extern struct nulsthdr_s *D_006309FC;
#define sceneinst_pool D_006309FC
```
Do not replace that with a definition such as:

```c
static struct nulsthdr_s *sceneinst_pool;
```
If compile_diff reaches 100% but promote_matching fails with .data,
.sdata, .sbss, .bss, or unhandled content after last function, treat
that as unintended top-level C data. Remove the C data definition and use a
retail extern D_... reference instead.

Do not mark a unit complete = true while it owns data ranges unless
Data-from-C verification supports that unit/range.

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

All accept `--repo`, `--version`, `--json`.
