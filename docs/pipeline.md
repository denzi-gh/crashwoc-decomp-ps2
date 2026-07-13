# Pipeline

The full reconstruction pipeline, step by step. Each stage is byte-gated and
each later stage assumes the earlier gates hold. The [README](../README.md) is
the front page; this is the deep reference.

>This doc is mostly AI generated, but verified by me that it's correct!

## Target ELF properties

Recorded in full in [config/pal103/sections.json](../config/pal103/sections.json),
extracted directly from the retail executable:

```text
entry:     0x00100008
_gp:       0x00634970
ELF flags: 0x20924001  (noreorder, 5900, eabi64, mips3)

PT_LOAD (single RWE segment):
  offset   0x1000
  vaddr    0x00100000
  filesz   0x00532FBC
  memsz    0x0060698C
  align    0x1000

.text    0x00100000    .lit4    0x0062C980
.vutext  0x00281530    .sdata   0x0062E980
.data    0x00293700    .sbss    0x00633000
.rodata  0x00613480    .bss     0x00633400
```

## Verifying the target

```bash
python tools/verify_target.py
```

Checks file sizes, SHA-1 and SHA-256 hashes, the parsed `SYSTEM.CNF` values, and
every field of the ELF header, program headers and section header table against
the committed registry. Every line must read `PASS`.

## Symbol registries

The retail executable ships a complete MIPS ECOFF debug section (`.mdebug`,
magic `0x7009`). It describes every translation unit and procedure in the
program - their addresses, stack frames and saved-register masks.
[tools/extract_mdebug.py](../tools/extract_mdebug.py) reads it straight out of the
binary, in native table order, into three deterministic registries:

| File                                                              | Entries | Contents                                        |
| ----------------------------------------------------------------- | ------: | ----------------------------------------------- |
| [config/pal103/units.toml](../config/pal103/units.toml)           |     267 | translation units (file descriptors)            |
| [config/pal103/functions.toml](../config/pal103/functions.toml)   |   3,751 | procedures (address, frame, register masks)      |
| [config/pal103/symbol_addrs.txt](../config/pal103/symbol_addrs.txt) |   6,320 | splat-style `name = address` (functions + data) |

```bash
python tools/extract_mdebug.py           # (re)generate the registries
python tools/extract_mdebug.py --check   # verify committed == fresh extraction
```

The extraction is a byte-exact function of the input ELF: every address is read
or computed from the tables (`fdr.adr + pdr.adr`), never guessed, and `--check`
proves regeneration reproduces the committed files. Global data addresses in
`symbol_addrs.txt` are cross-checked against the allocated section ranges in
[config/pal103/sections.json](../config/pal103/sections.json), so an ECOFF `value`
that is a size rather than an address can never be emitted as one.

## Disassembly (splat bootstrap)

The target ELF is split into per-section assembly and a linker script by
[splat](https://github.com/ethteck/splat), driven by
[splat.yaml](../splat.yaml):

```bash
python -m pip install -r requirements.txt   # pinned splat + backends
python configure.py                          # disassemble into asm/ and build/
python configure.py --check                  # split twice; prove identical output
```

`configure.py` first checks that the installed disassembler matches the locked
versions (splat + its `spimdisasm`/`rabbitizer` backends, which determine the
generated text), confirms the target ELF is present, then runs `splat split`.
The pinned versions live in [requirements.txt](../requirements.txt) and the
[Containerfile](../Containerfile) bakes them into the image.

The whole loaded image is a single `PT_LOAD` segment: file offset `0x1000` maps
to vram `0x00100000` with a constant delta of `0xFF000`, so one splat code
segment covers every loaded section. `.bss`/`.sbss` are `NOBITS` and not
rom-contiguous, so those subsegments carry an explicit vram. splat consumes the
`symbol_addrs.txt` registry from the previous step for its function and data
names.

**Nothing splat produces is committed.** Every output is a deterministic
function of the user-supplied ELF and lands in gitignored directories: assembly
in `asm/`, and the linker script, raw section dumps (`build/assets/`, which are
game bytes), generated assembler macros and undefined-symbol lists in `build/`.
`configure.py --check` regenerates the whole split twice and verifies every
file is byte-for-byte identical, so the disassembly is reproducible from the
binary alone.

## Binary baseline

Before any code is decompiled, [tools/build_baseline.py](../tools/build_baseline.py)
proves the target's loaded image can be rebuilt from per-section objects placed
at the exact addresses in [linker.ld](../linker.ld) - the identity reconstruction
every later matching step builds on.

```bash
python tools/build_baseline.py          # reconstruct + verify (no toolchain needed)
python tools/build_baseline.py --link    # also link with the real PS2 ee-ld
```

It tiles the single `PT_LOAD` segment into the loaded `PROGBITS` sections
(`.text`, `.vutext`, `.data`, `.rodata`, `.lit4`, `.sdata`), carries each as a
raw `.incbin` object, and checks two things:

- **Loaded layout** - every object address and NOBITS size in `linker.ld` matches
  the target's section table, and the highest `.bss` address reaches the segment
  `memsz` end (`0x0070698C`).
- **Packaged hash** - reconstructing the loaded image (objects at their addresses
  over zero-filled gaps) reproduces the target's `PT_LOAD` file image exactly:
  SHA-256 `c92a59870d47441bbd3f741eca2be42ccbbab28b7d0670d96e09c2f2340fe438`.

One 0x80-byte run between `.vutext` and `.data` holds MIPS bytes described by no
section header; it is carried as an explicit `.orphan` object so no loaded byte
is ever invented. The three inter-section gaps (before `.rodata`, `.lit4`,
`.sdata`) are all-zero and recreated as linker fill.

The default run needs no toolchain and is deterministic. `--link` assembles and
links with the locked PS2 binutils (Linux x86-64; run it in the
[Containerfile](../Containerfile) image on non-Linux hosts) and confirms the real
`ee-ld` output's loaded image has the same packaged SHA-256. As with the
disassembly, **nothing game-derived is committed**: the extracted `.bin` blobs,
generated `.s`/`.o` and the linked ELF all land in gitignored `build/baseline/`.
Only `linker.ld` (addresses only) and the tool are tracked.

## Monolithic assembly baseline

The next step replaces the baseline's `.text` blob with real assembled
instructions. [tools/assemble_text.py](../tools/assemble_text.py) assembles splat's
whole-program `.text` disassembly (`asm/text.s`), links it at the target's
`.text` address, and proves the result is byte-identical to the retail `.text`.

```bash
python tools/assemble_text.py    # assemble asm/text.s, link, compare .text
```

It checks five things in order:

- **Disambiguate duplicate statics** - four static functions
  (`ReadNuIFFGeomSkin`, `ReadNuIFFGeomVtx`, `NuNodeRead`, `_fpadd_parts`) share a
  name across two translation units each. Legal as separate TUs, they collide in
  a single assembly file, so each definition is renamed to `NAME__<vram>` and
  each `jal` is repointed to the definition its own encoding already targets. No
  instruction byte moves.
- **Assemble** - the transformed text assembles with the locked PS2 `as`
  (`-march=r5900`).
- **Resolve external symbols** - every symbol `asm/text.s` references but does
  not define is resolved to its true address, from the committed mdebug registry,
  splat's auto lists, or (for address-named auto symbols like `D_00633400`) the
  name itself.
- **Link** - `.text` is placed at `0x00100000` with those symbols defined.
- **Assembled `.text` byte-identical** - the linked `.text` equals the target's
  `.text` (`elf[0x1000 : 0x1000+0x181530]`) byte-for-byte, SHA-256
  `25e07defbc3617ff1502295a050c8fea1ee0c5a6d0e52838e82d6b7b3abf63e5`.

Like `--link`, this needs the PS2 binutils (Linux x86-64; run it in the
[Containerfile](../Containerfile) image on non-Linux hosts). **Nothing game-derived
is committed**: the transformed `.s`, the object, the link script and the linked
ELF all land in gitignored `build/baseline/`; only this script and the addresses
it reads are tracked.

## Translation-unit split

The final structural step splits that single `.text` into one assembly object per
translation unit, following the boundaries the retail `.mdebug` records (which
unit owns each procedure), and proves the *whole loaded image* still links
exactly. [tools/split_text.py](../tools/split_text.py) drives it:

```bash
python tools/split_text.py    # split by TU, assemble each, link the full image
```

The `.mdebug` procedure table gives 247 translation units that own `.text` code
(of 267 total units). Each becomes its own `.s` object; the six non-`.text`
sections stay as `.incbin`, exactly as the binary baseline carries them. It
checks, in order:

- **Disambiguate duplicate statics** - reuses the monolithic baseline's
  byte-lossless rename so the four cross-TU static names never collide.
- **Drop no-op `.align` directives** - the disassembly covers `.text` with one
  explicit instruction word per four bytes and no address gaps, so every
  `.align` is a no-op. Dropping them makes each TU a pure instruction stream:
  no trailing section pad, and no dependence on the object's base phase - which
  the 11 TUs that start at a 4-aligned (not 8-aligned) address would otherwise
  assemble wrongly.
- **Split lossless** - the per-TU bodies are a strict, order-preserving
  partition of the monolithic body: same lines, nothing added or dropped.
- **Assemble / resolve / link** - every TU is assembled separately, all external
  symbols resolved to their true addresses, and the objects linked with the six
  `.incbin` sections via `SUBALIGN(1)` (pure concatenation) into the full
  `PT_LOAD` image.
- **Linked image matches packaged hash** - the reconstructed loaded image is
  byte-identical, SHA-256
  `c92a59870d47441bbd3f741eca2be42ccbbab28b7d0670d96e09c2f2340fe438` - the same
  packaged hash the binary baseline established.

Like the other reconstruction steps this needs the PS2 binutils (Linux x86-64;
run it in the [Containerfile](../Containerfile) image); the split and lossless
checks are pure Python and run anywhere. **Nothing game-derived is committed**:
the 247 per-TU `.s`, the objects, the `.incbin` blobs, the link script and the
linked ELF all land in gitignored `build/split/`.

## Matching C

With the loaded image reconstructible from per-TU objects, decompilation proper
begins: replacing a translation unit's assembly with hand-written C that
compiles to the exact retail bytes. Each unit `nucore/nulist` decompiles into
[src/nucore/nulist.c](../src/nucore/nulist.c) - clean C, no `INCLUDE_ASM`
annotations - with a status manifest at `config/pal103/status/nucore/nulist.toml`
marking every function `asm`, `equivalent`, or `matching`.

**Hybrid objects.** [tools/gen_hybrid.py](../tools/gen_hybrid.py) compiles a unit
with its profile's compiler (`ee-gcc -S`), then splices, in retail address
order, the C for each `matching`/`equivalent` function and the retail assembly
slice for every other function; it normalizes the two compilers' differing
pseudo-op encodings and assembles with the decompals `as`. Two link sets come
out of this: `matching` (only byte-exact C; retail slices fill the rest) which
must reproduce the retail bytes and feeds the image gate, and `equivalent`
(reviewed-equivalent C compiles in too) which only has to link - the modding
build.

**The build graph** (`build.ninja`, generated by
[tools/gen_ninja.py](../tools/gen_ninja.py)) builds these object sets per unit:

| Set              | What it is                                                  |
| ---------------- | ----------------------------------------------------------- |
| `current`        | plain compile of the C - the honest objdiff score           |
| `report-current` | the C with the hybrid's normalization, used as objdiff's base so a verified `matching` function reads 100% (the raw `current` object misses by `.lit4` pool placement) |
| `matching`       | byte-exact hybrid - links the full image for the byte gate  |
| `equivalent`     | modding hybrid - must link, may diverge                      |

**Canonical verification** never trusts a score: `tools/promote.py` is the only
writer of `state = matching`, and [tools/verify_promoted.py](../tools/verify_promoted.py)
re-derives every `matching` claim byte-for-byte over the function's full registry
extent, ending in [tools/link_image.py](../tools/link_image.py) relinking the whole
image and checking the retail SHA-256
`c92a59870d47441bbd3f741eca2be42ccbbab28b7d0670d96e09c2f2340fe438`. The public
objdiff report is a *measurement* layered on top; it can never move a byte gate.

The first matched unit is [src/nucore/nulist.c](../src/nucore/nulist.c):
`NuListGetHead` and `NuListGetTail` are byte-identical to retail. (Not every
runtime function is C - `strlen`, for one, is hand-written r5900 SIMD assembly
in the retail build and stays as asm.)

This step needs both toolchains at once - an EE GCC compiler and the PS2
binutils - so it runs in the [Containerfile](../Containerfile) image, which pairs
trixie's glibc (for the binutils) with i386 multilib (for the 32-bit compiler).
The C in `src/` is the committed work product; every generated artifact is
game-derived and gitignored: object sets in `build/`, expected objects in
`expected/`, and `objdiff.json`.

### The objdiff project and the published report

[tools/gen_objdiff.py](../tools/gen_objdiff.py) writes `objdiff.json` so the whole
program is visible in [objdiff](https://github.com/encounter/objdiff) from day
one: all 247 `.text` units (each targeting its expected object; units with a
manifest diff against `report-current`, the rest against raw `current`), plus one
target-only unit per linked data object in a dedicated **`data`** progress
category. Those data units report an honest `total_data` (the real linked byte
count) with zero matched until decompiled C data exists - objdiff measures their
bytes from the sections directly, so no artificial code value is invented for
them. The decomp.dev treemap only renders units with `total_code > 0`, so the
data units are correctly counted in the report and category metrics but do not
appear as clickable tiles - expected, not a bug.

`ninja report` scores the objects and [tools/sanitize_report.py](../tools/sanitize_report.py)
whitelist-filters the result into the only report that is ever published
(`report.public.json`): it drops every non-whitelisted field, aborts on any
`orig/` reference / absolute path / embedded byte blob, and strips derived
measures whose total is zero (so nothing claims "100% of nothing"). That
sanitized report is the `SLES_503.86_report` artifact consumed by decomp.dev.

## Completion levels

Progress is tracked against three distinct goals:

1. **Loaded-image exact** - every runtime-relevant byte and address matches
   (all allocated sections, entry point, `_gp`, program headers). This is the
   primary technical goal.
2. **Packaged ELF exact** - the generated `SLES_503.86` has the exact retail
   SHA-256.
3. **Pure relink exact** - every ELF byte, including `.mdebug`, `.stab`,
   `.symtab` and all linker metadata, is regenerated. Final archival goal.

## Toolchain

The retail ELF was built by **two** compilers, and matching uses both - each
for the half of the program it actually compiled. Which one a translation unit
uses is a named profile in
[config/pal103/profiles.toml](../config/pal103/profiles.toml):

| Profile   | Compiler    | Covers                                             |
| --------- | ----------- | -------------------------------------------------- |
| `default` | `ee-gcc-tt` | the game and Nu-engine TUs (the ones being decompiled) |
| `sce`     | `ee-gcc`    | the Sony runtime half (SCE libraries, newlib, libgcc, CRT) |

The game/engine TUs provably cannot match under the Sony 2.9 build (its `sq`/`lq`
callee saves and lack of R5900 short-loop padding), so `default` is the SN ProDG
**EE GCC 2.95.2-EE** compiler; the Sony **EE GCC 2.9-ee-991111-01** stays as `sce`
for the runtime. Every unit compiles with `default` unless its status manifest
names a profile, and both flag sets are `-O2 -G8 -fomit-frame-pointer`. Modern
compilers are used only for analysis and tooling, never for matching results.

Every tool is pinned in [toolchain.lock.json](../toolchain.lock.json):

| Component      | Version              | Role                                       |
| -------------- | -------------------- | ------------------------------------------ |
| `ee-gcc-tt`    | 2.95.2-EE (SN ProDG) | `default` matching compiler (game/engine)  |
| `ee-gcc`       | 2.9-ee-991111-01     | `sce` matching compiler (Sony runtime)     |
| `wibo`         | 1.1.0                | runs the Win32 `ee-gcc-tt` under Linux     |
| `ps2-binutils` | 0.10                 | reconstruction + hybrid assembler / linker |
| `python`       | 3.12                 | tooling runtime (via uv)                   |
| `splat`        | 0.41.0               | ELF disassembler / splitter                |
| `objdiff`      | 3.7.2                | object diffing / match scoring             |

Each compiler is anchored by SHA-256, and every installed file is additionally
verified byte-for-byte against a recorded fingerprint manifest (so a same-version
reinstall that would silently change codegen is caught); the archives themselves
are user-supplied and never committed (`compiler/` and `tools/download/` are
gitignored).

### Installing

```bash
# Place the compiler archive in tools/download/, or set its URL in the lock.
python tools/setup_toolchain.py --download     # downloads + verifies SHA-256
python tools/fingerprint_compiler.py --record  # record a per-file manifest
```

`setup_toolchain.py` verifies every downloaded archive against its locked
SHA-256 and refuses to install anything whose hash is not yet locked (use
`--record` to trust and record a hash from a local archive). Once a compiler is
installed, `fingerprint_compiler.py` verifies that every file of the install is
byte-identical to a recorded manifest - catching same-version installs that
would silently change codegen:

```bash
python tools/fingerprint_compiler.py           # verify one component (ee-gcc)
python tools/fingerprint_compiler.py --all     # verify every fingerprinted component (CI)
```

## CI (dtk-template private-build-image method)

Two workflows, both on GitHub-hosted runners:

- **validate.yml** - every push and PR. Structure only: manifests, registries,
  tests, and the guard that nothing game-derived is tracked in this repository.
  Needs no game files.
- **matching.yml** - pushes to any branch in this repo, nightly, and manual
  dispatch (never on `pull_request`; see the fork-safety note below). The full
  byte-gated pipeline. It needs the retail ELF, which this public repository
  must never contain - so the job runs *inside* a private container image that
  has the game files baked in, following
  [dtk-template's method](https://github.com/encounter/dtk-template/blob/main/docs/github_actions.md).

```
this repo (public, zero game bytes)
  Containerfile ──publish-image.yml──► ghcr.io/denzi-gh/crashwoc-decomp      [public package]
                                              ▲ FROM
crashwoc-decomp-ps2-build (PRIVATE repo)      │
  orig/pal103/{SLES_503.86,SYSTEM.CNF}        │
  Dockerfile (COPY orig /orig) ── its build.yml ──► ghcr.io/denzi-gh/crashwoc-decomp-ps2-build:main  [PRIVATE package]
                                                          ▲ matching.yml job container
matching.yml: cp -a /orig . → cached toolchain install + fingerprints → verify_target
  → registry --checks → configure → ninja gates → report → check_report_matches
  → sanitize → stage → smoke_report → upload artifact
```

### The three pieces

1. **Public base image** (`ghcr.io/denzi-gh/crashwoc-decomp`): built from
   [Containerfile](../Containerfile) by `publish-image.yml` whenever the
   Containerfile or requirements change. Contains only the build
   environment (Debian, Python, splat, ninja, i386 runtime libs) - no game
   bytes, no matching toolchain - so the package is safe to keep public.
2. **Private build repo** (`denzi-gh/crashwoc-decomp-ps2-build`): holds
   `orig/pal103/` and a two-line Dockerfile (`FROM` the base image,
   `COPY orig /orig`). Its workflow pushes the result to GHCR as a
   **private** package. To update game files or pick up a new base image,
   push there or re-run its workflow.
3. **matching.yml** here: runs in that private image on `ubuntu-latest`.
   Pull authorization is the package's Actions-access grant plus plain
   `GITHUB_TOKEN` - no PAT, no secret URL.

### One-time setup (already done; recorded for re-setup)

1. Run `publish-image.yml` once (workflow_dispatch), then make the
   `crashwoc-decomp` package **public**: package page → Package settings →
   Change visibility.
2. On the `crashwoc-decomp-ps2-build` package: Package settings →
   **Manage Actions access** → add repository `crashwoc-decomp-ps2` with
   the **Read** role.

### Container image pinning

The [Containerfile](../Containerfile) pins every upstream source by immutable
digest so the build environment is byte-reproducible:

- **Base image**: `python:3.12-slim-trixie@sha256:423ed6ab…199fbf`.
- **uv**: `ghcr.io/astral-sh/uv:0.11.27@sha256:4d01caf3…693419` (copied in for
  the `/uv` binary).

The human-readable tags are kept alongside the digests for readability only -
the `@sha256:` digest is what Docker resolves. Python packages are pinned
separately in [requirements.txt](../requirements.txt) (`splat64`, `spimdisasm`,
`rabbitizer`), which must match `toolchain.lock.json`.

**Updating a pinned digest:**

1. Resolve the new digest from the registry (do not hand-write it). For the
   base image:
   ```bash
   docker buildx imagetools inspect python:3.12-slim-trixie \
     --format '{{.Manifest.Digest}}'
   ```
   For uv, pick a concrete version tag (e.g. a new `0.x.y`) and inspect it:
   ```bash
   docker buildx imagetools inspect ghcr.io/astral-sh/uv:0.x.y \
     --format '{{.Manifest.Digest}}'
   ```
   (Without buildx, the registry HTTP API's `Docker-Content-Digest` response
   header on a `HEAD`/`GET` of the manifest gives the same value.)
2. Replace the `@sha256:` in the Containerfile (and the tag/version comment).
   Use the multi-arch index digest, not a single-platform one, so the image
   stays portable.
3. Rebuild (`docker build -f Containerfile -t crashwoc-decomp .`) and re-verify:
   inside the image `uv pip list` shows the locked `splat64`/`spimdisasm`/
   `rabbitizer` versions (`python configure.py --strict` passes), and the full
   `matching.yml` pipeline (via the private build image rebuilt `FROM` this
   base) is green end-to-end. Update this section's digests.

### Caching and speed

Two `actions/cache` entries keep runs fast: the locked toolchain
(`tools/download` + `compiler`, keyed on `toolchain.lock.json`) and the
ninja build tree (`build`, `expected`, `.ninja_*`, keyed per commit with a
prefix restore key). A cold run does image pull + toolchain download
(~5–10 min); warm runs are incremental.

### What can leave a run

Only one thing, ever: the `SLES_503.86_report` artifact (the
whitelist-sanitized report from `tools/sanitize_report.py` - this is what
[decomp.dev](https://decomp.dev) consumes). Everything else (orig copies,
objects, images) dies with the ephemeral runner. The pipeline never writes back
to the repo; progress history for a site comes from accumulating those report
artifacts, not a committed baseline.

Before the artifact uploads, two gates run against it. `tools/check_report_matches.py`
(right after `ninja report`) asserts every function `verify_promoted.py` proved
byte-matching reads 100% in the report. Then, after the artifact is staged to
`build/publish/report.json`, `tools/smoke_report.py` re-checks the exact bytes
that will leave the runner: valid JSON, a strict subset of objdiff's Report
schema (no stray fields), no `orig/` reference / absolute path / embedded byte
blob, no hollow "100% of nothing" measure, and again every verified matching
function at 100%. A stale or tampered staged file fails the run before upload.

### PR verification (no fork ever touches the private image)

`matching.yml` deliberately has **no `pull_request` trigger**. A
`pull_request` run would execute the workflow file *from the PR branch*
inside the private `/orig` image, so any fork could rewrite it to exfiltrate
the retail game files. Removing the trigger closes that hole entirely;
`pull_request_target` is **not** used, because it would run the same
untrusted PR code with the same private resources.

Instead, byte verification is driven by the **`push` trigger on any branch**
(`branches: ["**"]`). This is fork-safe by construction: a push to a fork
runs in the *fork's* Actions, never in this repository, so untrusted code can
never reach the private `/orig` image through it. Only someone with write
access can push a branch here, and their code is as trusted as a push to
`main`.

The practical payoff - same-repo PRs get their byte gates and a decomp.dev
progress comment automatically:

- A **collaborator branch** (same-repo PR): every push runs the full pipeline
  and uploads `SLES_503.86_report`. The decomp.dev app attaches that artifact
  to the open PR as a progress-vs-main comment. No manual dispatch needed.
- A **fork PR**: triggers only **validate.yml** (public, no game files -
  manifests, registries, unit tests, the "nothing game-derived is tracked"
  guard). That is the full contract a fork PR can rely on. To byte-verify it,
  a maintainer reviews the diff (especially `.github/`, `tools/`, the
  toolchain lock), cherry-picks / merges the vetted commit onto an **in-repo**
  branch, and lets that branch's push (or `workflow_dispatch`) run the gates.
