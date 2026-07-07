# Contributing

Everything you need for day-to-day work goes through one script, `decomp.py`.
You should not need to call the internal tools directly.

## Prerequisites

- **Docker or Podman** (the Linux-only matching toolchain runs in a container).
- **Python 3.12+** on your host.
- Your own **legally obtained** copy of the game placed under `orig/` (see the
  README for the exact build and file layout). Game files are never committed.

## Three commands

### 1. Prepare the toolchain (once)

```bash
python decomp.py toolchain
```

Ensures the dev image, starts the persistent dev container, installs the locked
toolchain, and verifies every locked component's fingerprint. Safe to re-run.

### 2. Initialise the workspace

```bash
python decomp.py setup
```

Verifies your `orig/` target, splits it, builds the objects objdiff scores,
runs the canonical matching verification, and generates the objdiff report.
Safe to re-run; it only rebuilds what changed. When it finishes, **open the
repository in the objdiff GUI** to get live per-function match feedback.

### 3. Work on a unit

```bash
python decomp.py src/game/creature.c
```

Rebuilds that unit, refreshes the report, and prints a per-function table:

```text
Unit: game/creature
Compiler: default

Function                  Match      State         Verified
----------------------------------------------------------------
CreatureInit              100.00%    matching      yes
CreatureUpdate             96.42%    asm           no
CreatureDraw               73.18%    asm           no

Unit fuzzy progress:       89.31%
Verified matching:          1 / 3
```

- **Match** is the objdiff fuzzy percentage (the same report CI and decomp.dev
  use) — a guide, not proof.
- **State** is the committed manifest state: `asm`, `equivalent`, `matching`.
- **Verified** is canonical truth: the function's bytes were re-derived and
  proven byte-identical to retail.

Any function that reaches **100%** while still `asm` is **automatically
promoted**: the wrapper runs the real byte + image verification and keeps the
`matching` state only if it passes, rolling back otherwise. Pass `--no-promote`
to inspect without promoting.

If a unit has no manifest yet, create one first:

```bash
python tools/promote.py --init src/<group>/<unit>.c
```

## Maintenance / internals

The wrapper is a thin front end over the existing tooling. The individual tools
(`tools/dispatch.py`, `tools/promote.py`, `tools/verify_promoted.py`,
`configure.py`, the ninja targets, the report/sanitize scripts, …) remain the
source of truth and are documented in the README under *Common commands* and in
`docs/`. Reach for them only for maintenance tasks the three commands above do
not cover.
