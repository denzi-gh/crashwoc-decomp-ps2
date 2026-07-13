# Contributing

Day-to-day work goes through one script, `decomp.py`. You should not need to call
the internal tools directly.

## Prerequisites

- **Python 3.12+** on your host.
- The **matching toolchain**, which is Linux-only. Run it either in a container
  (default, works on any OS) or natively on Linux/WSL2 - see *Setup* below.
- Your own copy of the game at `orig/pal103/` (see the
  [README](README.md) for the exact build and hashes). Game files are never
  committed.

## Setup

### Option A - Docker / Podman (any OS)

The Linux-only toolchain runs in a container; three commands cover everything.

```bash
python decomp.py toolchain   # build the image, start the dev container, install + verify the locked toolchain
python decomp.py setup       # verify orig/, split it, build objdiff's objects, run verification, generate the report
python decomp.py <unit>.c    # rebuild one unit and print its per-function match table
```

All three are safe to re-run; `setup` only rebuilds what changed. When it
finishes, **open the repository in the objdiff GUI** for live per-function match
feedback.

### Option B - Native, no Docker (Linux / WSL2)

The toolchain binaries are Linux x86-64 (plus one 32-bit compiler run under
`wibo`), so a Linux host - or Windows via **WSL2** - can run them directly and
skip the container:

```bash
python -m pip install -r requirements.txt -r requirements-mcp.txt   # splat + MCP SDK
sudo dpkg --add-architecture i386                                    # 32-bit compiler needs i386 libs
sudo apt-get update && sudo apt-get install -y libc6:i386 libstdc++6:i386 zlib1g:i386
python tools/setup_toolchain.py --download                           # fetch + verify the locked toolchain
export CRASHWOC_DIRECT=1                                             # run the toolchain natively, no container
python decomp.py setup
python decomp.py <unit>.c
```

`CRASHWOC_DIRECT=1` tells `tools/dispatch.py` to run toolchain commands in place
instead of forwarding them into the container. macOS and plain Windows (no WSL)
should use Option A, the binaries are Linux ELFs. Either way, you can also skip
local byte-work entirely and let CI run the byte gates on your pushed branch.

## Working on a unit

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

- **Match** - the objdiff fuzzy percentage (what CI and decomp.dev use): a guide,
  not proof.
- **State** - the committed manifest state: `asm`, `equivalent`, `matching`.
- **Verified** - canonical truth: the bytes were re-derived and proven identical
  to retail.

Any function that reaches **100%** while still `asm` is **automatically promoted**:
the wrapper runs the real byte + image verification and keeps `matching` only if
it passes, rolling back otherwise. Pass `--no-promote` to inspect without
promoting. If a unit has no manifest yet, create one first:

```bash
python tools/promote.py --init src/<group>/<unit>.c
```

## Validate the repo is healthy

These gates are pure Python - no toolchain, no game files, any OS. Run them
before opening a PR; CI (`validate.yml`) runs the same set.

```bash
python -m unittest discover -s tests -v      # all unit tests
python tools/status.py --check               # status manifests are well-formed
python tools/verify_target.py                # orig/ matches the committed registry (needs orig/)
python tools/extract_mdebug.py --check       # registries == fresh extraction (needs orig/)
python tools/extract_data_map.py --check     # data map == fresh extraction (needs orig/)
python configure.py --check                  # disassembly is deterministic (needs orig/ + splat)
```

The full byte matrix (compile, `verify-loaded`, `verify-promoted`) needs the
toolchain and runs in the container or the native Option-B environment; the
pipeline and CI are documented in [docs/pipeline.md](docs/pipeline.md).

**Never hand-edit a manifest to `state = "matching"`** - only `tools/promote.py`
writes it, after byte verification. `equivalent` is a human review decision.

## Internals

The wrapper is a thin front end over the real tooling (`tools/dispatch.py`,
`tools/promote.py`, `tools/verify_promoted.py`, `configure.py`, the ninja
targets, the report scripts). Reach for those only for maintenance the commands
above don't cover; they're documented in [docs/pipeline.md](docs/pipeline.md).
