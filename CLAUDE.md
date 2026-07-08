# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

A byte-exact matching decompilation of **Crash Bandicoot: The Wrath of Cortex** (PS2), targeting exactly one build: PAL retail v1.03, `SLES_503.86`. The user supplies the game ELF at `orig/pal103/`; it is never committed. The retail ELF was built by TWO toolchains, both locked in `toolchain.lock.json` / `config/pal103/profiles.toml` with flags `-O2 -G8 -fomit-frame-pointer`: the game/engine TUs (profile `default`) match under **SN ProDG EE GCC 2.95.2-EE** (`ee-gcc-tt`, a Win32 binary run under wibo in the container), the Sony runtime half (profile `sce`) under **EE GCC 2.9-ee-991111-01**. Modern compilers are for tooling only, never matching.

## Hard rules

- **Never run `git commit`.** Leave changes staged or modified; the user commits.
- **Never commit game-derived content.** `orig/`, `reference/`, `asm/`, `expected/`, `build/`, `compiler/`, `tools/download/`, `objdiff.json`, and `build.ninja` are gitignored. Tracked work products are only: config registries, tools, `linker.ld` (addresses only), docs, and hand-written matching C in `src/`. (Exception: the separate PRIVATE repo `denzi-gh/crashwoc-decomp-ps2-build` holds `orig/` for CI — approved, must stay private.)
- **Never fabricate hashes or URLs.** Unknown values are explicit `null`; hashes measured from `orig/` are fine.
- Work proceeds PR-by-PR against the user's plan: do the requested PR, verify its pass condition, stop.

## Toolchain runs in a container

The EE GCC (32-bit x86) and PS2 binutils (glibc 2.38) are Linux-only. Anything that assembles/links/compiles must run in the dev container; pure-Python tools (registry extraction, verification, splits, status checks, unit tests) run natively on Windows.

The universal entry point handles this automatically — it forwards into a long-lived `crashwoc-dev-<hash>` container (repo mounted at `/work`), translating host paths:

```
python tools/dispatch.py ninja check          # any ninja target
python tools/dispatch.py python tools/verify_hybrid.py
```

Build the image once if needed: `docker build -f Containerfile -t crashwoc-decomp .`

Known quirk: the 32-bit EE GCC gets EOVERFLOW stat-ing bind-mounted sources, so `tools/cc.py` compiles from a fixed container-native scratch dir and copies the `.o` out (also makes objects deterministic — keep that behavior).

## Common commands

```bash
# Structure checks (run natively, no game files needed — this is what validate.yml runs)
python -m unittest discover -s tests -v            # all unit tests
python -m unittest tests.test_status -v            # one test module
python tools/status.py --check                     # validate status manifests

# Registry / target gates (native, need orig/)
python tools/verify_target.py                      # hash + ELF structure vs committed registry
python tools/extract_mdebug.py --check             # committed registries == fresh extraction
python tools/extract_data_map.py --check           # data map == fresh extraction
python configure.py                                # splat disassembly into asm/, then regenerates build.ninja + objdiff.json
python configure.py --check                        # split twice, prove deterministic

# Build & byte gates (via dispatch → container)
python tools/dispatch.py ninja expected            # 247 per-TU retail objects
python tools/dispatch.py ninja current             # plain compile of src/ (what objdiff scores)
python tools/dispatch.py ninja check               # current + verify-loaded + verify-promoted
python tools/dispatch.py ninja verify-loaded       # full loaded image must reproduce retail SHA
python tools/dispatch.py ninja verify-promoted     # re-verify every "matching" claim from scratch
python tools/dispatch.py ninja report              # objdiff-cli progress report
python tools/dispatch.py ninja report-public       # whitelist-sanitized report (decomp.dev)

# Promotion (the only writer of state = "matching")
python tools/promote.py <FunctionName>             # flip manifest, verify, roll back on failure
python tools/promote.py --init src/<unit>.c        # scaffold an all-asm manifest for a new unit
python tools/compare_progress.py                   # regression gate vs progress/summary.json
```

**After adding a source file, status manifest, or header, regenerate the build graph**: `python tools/gen_ninja.py` (ninja cannot discover new edges; configure.py also regenerates it).

## Architecture

The pipeline is a chain of byte-gated reconstruction steps; each later stage assumes the earlier gates hold:

1. **Registries** (`config/pal103/`) — extracted deterministically from the retail ELF's `.mdebug` debug section: `units.toml` (267 TUs), `functions.toml` (3,751 procedures with addresses/frames), `symbol_addrs.txt`, `data_map.toml` (exact tiling of data sections into per-unit ranges), `sections.json`, `version.json`. All are `--check`-gated: committed files must equal a fresh extraction.
2. **Disassembly** — splat (pinned 0.41.0, `splat.yaml`) splits the ELF into `asm/` using the registries. Nothing splat emits is committed.
3. **Reconstruction** — `build_baseline.py` (incbin sections at `linker.ld` addresses), `assemble_text.py` (monolithic `.text` reassembled), `split_text.py` (247 per-TU objects). Each proves the loaded `PT_LOAD` image reproduces the packaged SHA-256 `c92a5987…0fe438`.
4. **Matching C** — `src/<unit>.c` mirrors the objdiff unit name (e.g. `src/nucore/nulist.c` for unit `nucore/nulist`), with a status manifest at `config/pal103/status/<unit>.toml` marking each function `asm`/`equivalent`/`matching`. `tools/gen_hybrid.py` builds hybrid objects: compile the unit with `ee-gcc -S`, splice retail assembly slices for every non-`matching` function (address order), normalize (`_sonyize`) and assemble with the **decompals `as`** (not Sony's). Sources stay clean C — no INCLUDE_ASM annotations.
5. **Link sets** — the ninja graph builds three parallel object sets: `current` (plain compile, the honest objdiff score), `matching` (hybrids, must be byte-exact — feeds `verify-loaded`), `equivalent` (modding build; must link, may diverge).
6. **Verification** — `verify_promoted.py` re-derives every `matching` claim (full registry extent, size = gap to next function), `link_image.py` links the full image and compares the retail SHA, `compare_progress.py` fails on any regression before anything is published.

`configure.py` / `gen_ninja.py` regenerate `build.ninja` and `objdiff.json`; objdiff rebuilds via `python tools/dispatch.py ninja <object>` so the GUI works from the Windows host.

CI (docs/ci.md): `validate.yml` (structure, no game files, every push/PR) and `matching.yml` (full byte gates, runs inside a private GHCR image with game files baked in, dtk-template style).

## Matching gotchas

- Prefer functions whose TU is a `.c` file. Several runtime routines (`strlen`, `memcpy`, …) are hand-written r5900 assembly in retail and can never match from C.
- The original assembler accepts only numeric GPR names (`jr $31`, not `jr $ra`); `gen_slices.py` handles the rewrite.
- Four static functions are duplicated across TUs and disambiguated as `NAME__<vram>`.
- `docs/notes.md` is the running log of quirks, resolved decisions, and invariants — check it before re-deriving a decision, and record new loose ends there.

## Matching MCP / decomp-agent (Claude Code specifics)

The shared, model-independent matching workflow — candidate selection, context,
compile/diff over the full extent, shared cross-client sessions, blockers,
verification, promotion — lives in `tools/decomp_agent/` (domain layer) and is
exposed over MCP by `tools/decomp_mcp/server.py`. **The canonical workflow, stop
conditions, and session hand-off rules are in [docs/decomp_agent.md](docs/decomp_agent.md)
— read that; do not duplicate them here.** Codex-specific launch/escalation is
in `AGENTS.md`.

Launch: this repo ships `.mcp.json` (server `crashwoc-decomp`, stdio,
`python -m tools.decomp_mcp.server --repo .`). Install the SDK once with
`python -m pip install -r requirements-mcp.txt`; the domain layer and its CLI
need no extra packages. Toolchain-bound tools auto-route through
`tools/dispatch.py` into the container.

Escalation is a **client-side** decision — the MCP never selects a model or
reasoning level. Use an initial worker configuration for attempts 1–8 and a
stronger configuration (higher reasoning effort) for 9–20, escalating only while
the best result is still improving with no architectural blocker or
oscillation. The shared session ledger lets Claude Code do the initial attempts
and Codex the escalation, or the reverse.
