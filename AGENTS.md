# AGENTS.md — Codex-specific guidance

Shared rules, the workflow, canonical ids, source-of-truth order, stop
conditions and session hand-off are in **[docs/decomp_agent.md](docs/decomp_agent.md)**
(canonical) — read that first. This file only adds what is specific to OpenAI
Codex (CLI and IDE).

## Repository hard rules (apply to every agent)

- Never `git commit`, `git push`, or open a PR. Leave changes staged/modified.
- Never weaken a byte gate; never expose game-derived content (`orig/`,
  `reference/`, `asm/`, `expected/`, `build/`, `compiler/`, `tools/download/`).
- Compilation goes through `tools/cc.py`; promotion through `tools/promote.py`.
  Never hand-edit a status entry to `state = "matching"`, and never mark a
  function `equivalent` (human review only).

## Launching the MCP server

A project-scoped example lives at [.codex/config.toml](.codex/config.toml):

```toml
[mcp_servers.crashwoc_decomp]
command = "python"
args = ["-m", "tools.decomp_mcp.server", "--repo", "."]
enabled = true
startup_timeout_sec = 20
tool_timeout_sec = 180
```

Install the SDK first: `python -m pip install -r requirements-mcp.txt` (or set
`command = "uv"`, `args = ["run", "python", "-m", "tools.decomp_mcp.server", "--repo", "."]`
if you manage the env with uv). `--repo .` resolves against the directory Codex
launches the server from — no machine-specific absolute paths. Verify the exact
key names against your installed Codex version; the block above is the current
documented form.

Toolchain-bound tools (`compile_diff`, `verify_candidate`, `promote_matching`)
route through the dev container automatically, so keep `tool_timeout_sec`
generous.

## Client-side escalation (Codex)

Model choice is **yours**, not the MCP's. A workable policy:

```
Attempts 1–8:   GPT-5.5, reasoning effort: medium
Attempts 9–20:  GPT-5.5, reasoning effort: xhigh
```

Escalate to `xhigh` only when the session shows all of:

- not yet exact, **and** measurable progress was made,
- no proven architectural blocker,
- no repeated-hash / oscillation condition,
- budget remains.

`checkpoint_session` / `resume_session` return a capability-based `escalation`
hint (`deep_reasoning` vs `standard`) and remaining budget — use it as the
trigger, but the model/effort mapping stays here on the client side.

## Hand-off with Claude Code

Sessions are shared. To pick up a session Claude Code started: `resume_session`
with `client: "codex"` (and your model/effort as opaque metadata). It returns
the best attempt, prior hypotheses, seen hashes, mismatch categories, blockers
and remaining budget. If it refuses (`unsafe_resume`), reconcile the reported
conflict (repo revision, profile fingerprint, source edits, or a foreign lock)
before continuing — do not force. When you finish or pause, `end_session` to
release the lock so Claude Code (or another Codex run) can resume cleanly.
