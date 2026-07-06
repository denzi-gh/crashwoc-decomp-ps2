# crashwoc-decomp-ps2

A matching decompilation of **Crash Bandicoot: The Wrath of Cortex** for the
PlayStation 2.

## Canonical target

All matching work targets exactly one build:

```text
Crash Bandicoot: The Wrath of Cortex
PAL retail v1.03
SLES-50386 (SLES_503.86)
```

No game files are committed to this repository. You must supply your own
legally obtained copies and place them at:

```text
orig/pal103/SLES_503.86
orig/pal103/SYSTEM.CNF
```

They must match these values exactly:

| File          | Size (bytes) | SHA-256                                                            |
| ------------- | -----------: | ------------------------------------------------------------------ |
| `SLES_503.86` |    6,731,763 | `7fc6826d08c42a5a92fc018edae3e520783beb5a8330410251192bfe26b1eca1` |
| `SYSTEM.CNF`  |           56 | `86fe327e4576516f57004800b2663c75c8a65a62872f3fdf3aa0618daf23d9c1` |

SHA-1 values and the expected `SYSTEM.CNF` content
(`BOOT2 = cdrom0:\SLES_503.86;1`, `VER = 1.03`, `VMODE = PAL`) are recorded in
[config/pal103/version.json](config/pal103/version.json).

Other builds (US v1.00 `SLUS_202.38`, the European demo, early European
builds) may be placed under `reference/` for analysis, but they never affect
PAL matching progress.

### Target ELF properties

Recorded in full in [config/pal103/sections.json](config/pal103/sections.json),
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

Expected output:

```text
Target ELF hash:             PASS
SYSTEM.CNF hash:             PASS
SYSTEM.CNF metadata:         PASS
ELF header:                  PASS
Program headers:             PASS
Section table:               PASS
```

The verifier checks file sizes, SHA-1 and SHA-256 hashes, the parsed
`SYSTEM.CNF` values, and every field of the ELF header, program headers and
section header table against the committed registry.

## Symbol registries

The retail executable ships a complete MIPS ECOFF debug section (`.mdebug`,
magic `0x7009`). It describes every translation unit and procedure in the
program — their addresses, stack frames and saved-register masks.
[tools/extract_mdebug.py](tools/extract_mdebug.py) reads it straight out of the
binary, in native table order, into three deterministic registries:

| File                                                           | Entries | Contents                                        |
| -------------------------------------------------------------- | ------: | ----------------------------------------------- |
| [config/pal103/units.toml](config/pal103/units.toml)           |     267 | translation units (file descriptors)            |
| [config/pal103/functions.toml](config/pal103/functions.toml)   |   3,751 | procedures (address, frame, register masks)      |
| [config/pal103/symbol_addrs.txt](config/pal103/symbol_addrs.txt) |   6,320 | splat-style `name = address` (functions + data) |

```bash
python tools/extract_mdebug.py           # (re)generate the registries
python tools/extract_mdebug.py --check   # verify committed == fresh extraction
```

The extraction is a byte-exact function of the input ELF: every address is read
or computed from the tables (`fdr.adr + pdr.adr`), never guessed, and `--check`
proves regeneration reproduces the committed files. Global data addresses in
`symbol_addrs.txt` are cross-checked against the allocated section ranges in
[config/pal103/sections.json](config/pal103/sections.json), so an ECOFF `value`
that is a size rather than an address can never be emitted as one.

## Disassembly (splat bootstrap)

The target ELF is split into per-section assembly and a linker script by
[splat](https://github.com/ethteck/splat), driven by
[splat.yaml](splat.yaml). Run it with:

```bash
python -m pip install -r requirements.txt   # pinned splat + backends
python configure.py                          # disassemble into asm/ and build/
python configure.py --check                  # split twice; prove identical output
```

`configure.py` first checks that the installed disassembler matches the locked
versions (splat + its `spimdisasm`/`rabbitizer` backends, which determine the
generated text), confirms the target ELF is present, then runs `splat split`.
The pinned versions live in [requirements.txt](requirements.txt) and the
[Containerfile](Containerfile) bakes them into the image.

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

Before any code is decompiled, [tools/build_baseline.py](tools/build_baseline.py)
proves the target's loaded image can be rebuilt from per-section objects placed
at the exact addresses in [linker.ld](linker.ld) — the identity reconstruction
every later matching step builds on.

```bash
python tools/build_baseline.py          # reconstruct + verify (no toolchain needed)
python tools/build_baseline.py --link    # also link with the real PS2 ee-ld
```

It tiles the single `PT_LOAD` segment into the loaded `PROGBITS` sections
(`.text`, `.vutext`, `.data`, `.rodata`, `.lit4`, `.sdata`), carries each as a
raw `.incbin` object, and checks two things:

- **Loaded layout** — every object address and NOBITS size in `linker.ld` matches
  the target's section table, and the highest `.bss` address reaches the segment
  `memsz` end (`0x0070698C`).
- **Packaged hash** — reconstructing the loaded image (objects at their addresses
  over zero-filled gaps) reproduces the target's `PT_LOAD` file image exactly:
  SHA-256 `c92a59870d47441bbd3f741eca2be42ccbbab28b7d0670d96e09c2f2340fe438`.

One 0x80-byte run between `.vutext` and `.data` holds MIPS bytes described by no
section header; it is carried as an explicit `.orphan` object so no loaded byte
is ever invented. The three inter-section gaps (before `.rodata`, `.lit4`,
`.sdata`) are all-zero and recreated as linker fill.

The default run needs no toolchain and is deterministic. `--link` assembles and
links with the locked PS2 binutils (Linux x86-64; run it in the
[Containerfile](Containerfile) image on non-Linux hosts) and confirms the real
`ee-ld` output's loaded image has the same packaged SHA-256. As with the
disassembly, **nothing game-derived is committed**: the extracted `.bin` blobs,
generated `.s`/`.o` and the linked ELF all land in gitignored `build/baseline/`.
Only `linker.ld` (addresses only) and the tool are tracked.

## Monolithic assembly baseline

The next step replaces the baseline's `.text` blob with real assembled
instructions. [tools/assemble_text.py](tools/assemble_text.py) assembles splat's
whole-program `.text` disassembly (`asm/text.s`), links it at the target's
`.text` address, and proves the result is byte-identical to the retail `.text`.

```bash
python tools/assemble_text.py    # assemble asm/text.s, link, compare .text
```

It checks five things in order:

- **Disambiguate duplicate statics** — four static functions
  (`ReadNuIFFGeomSkin`, `ReadNuIFFGeomVtx`, `NuNodeRead`, `_fpadd_parts`) share a
  name across two translation units each. Legal as separate TUs, they collide in
  a single assembly file, so each definition is renamed to `NAME__<vram>` and
  each `jal` is repointed to the definition its own encoding already targets. No
  instruction byte moves.
- **Assemble** — the transformed text assembles with the locked PS2 `as`
  (`-march=r5900`).
- **Resolve external symbols** — every symbol `asm/text.s` references but does
  not define is resolved to its true address, from the committed mdebug registry,
  splat's auto lists, or (for address-named auto symbols like `D_00633400`) the
  name itself.
- **Link** — `.text` is placed at `0x00100000` with those symbols defined.
- **Assembled `.text` byte-identical** — the linked `.text` equals the target's
  `.text` (`elf[0x1000 : 0x1000+0x181530]`) byte-for-byte, SHA-256
  `25e07defbc3617ff1502295a050c8fea1ee0c5a6d0e52838e82d6b7b3abf63e5`.

Like `--link`, this needs the PS2 binutils (Linux x86-64; run it in the
[Containerfile](Containerfile) image on non-Linux hosts). **Nothing game-derived
is committed**: the transformed `.s`, the object, the link script and the linked
ELF all land in gitignored `build/baseline/`; only this script and the addresses
it reads are tracked.

## Translation-unit split

The final structural step splits that single `.text` into one assembly object per
translation unit, following the boundaries the retail `.mdebug` records (which
unit owns each procedure), and proves the *whole loaded image* still links
exactly. [tools/split_text.py](tools/split_text.py) drives it:

```bash
python tools/split_text.py    # split by TU, assemble each, link the full image
```

The `.mdebug` procedure table gives 247 translation units that own `.text` code
(of 267 total units). Each becomes its own `.s` object; the six non-`.text`
sections stay as `.incbin`, exactly as the binary baseline carries them. It
checks, in order:

- **Disambiguate duplicate statics** — reuses the monolithic baseline's
  byte-lossless rename so the four cross-TU static names never collide.
- **Drop no-op `.align` directives** — the disassembly covers `.text` with one
  explicit instruction word per four bytes and no address gaps, so every
  `.align` is a no-op. Dropping them makes each TU a pure instruction stream:
  no trailing section pad, and no dependence on the object's base phase — which
  the 11 TUs that start at a 4-aligned (not 8-aligned) address would otherwise
  assemble wrongly.
- **Split lossless** — the per-TU bodies are a strict, order-preserving
  partition of the monolithic body: same lines, nothing added or dropped.
- **Assemble / resolve / link** — every TU is assembled separately, all external
  symbols resolved to their true addresses, and the objects linked with the six
  `.incbin` sections via `SUBALIGN(1)` (pure concatenation) into the full
  `PT_LOAD` image.
- **Linked image matches packaged hash** — the reconstructed loaded image is
  byte-identical, SHA-256
  `c92a59870d47441bbd3f741eca2be42ccbbab28b7d0670d96e09c2f2340fe438` — the same
  packaged hash the binary baseline established.

Like the other reconstruction steps this needs the PS2 binutils (Linux x86-64;
run it in the [Containerfile](Containerfile) image); the split and lossless
checks are pure Python and run anywhere. **Nothing game-derived is committed**:
the 247 per-TU `.s`, the objects, the `.incbin` blobs, the link script and the
linked ELF all land in gitignored `build/split/`.

## Completion levels

Progress is tracked against three distinct goals:

1. **Loaded-image exact** — every runtime-relevant byte and address matches
   (all allocated sections, entry point, `_gp`, program headers). This is the
   primary technical goal.
2. **Packaged ELF exact** — the generated `SLES_503.86` has the exact retail
   SHA-256.
3. **Pure relink exact** — every ELF byte, including `.mdebug`, `.stab`,
   `.symtab` and all linker metadata, is regenerated. Final archival goal.

## Toolchain

Matching uses the original Sony/Cygnus **EE GCC 2.9-ee-991111-01** compiler
exclusively. Modern compilers are used only for analysis and tooling, never
for matching results.

Every tool is pinned in [toolchain.lock.json](toolchain.lock.json):

| Component      | Version           | Role                                  |
| -------------- | ----------------- | ------------------------------------- |
| `ee-gcc`       | 2.9-ee-991111-01  | matching compiler (byte-exact anchor) |
| `ps2-binutils` | 0.10              | reconstruction assembler + linker     |
| `python`       | 3.12              | tooling runtime (via uv)              |
| `splat`        | 0.41.0            | ELF disassembler / splitter           |
| `objdiff`      | 3.7.2             | object diffing / match scoring        |

The compiler is anchored by SHA-256; the archive itself is user-supplied and
never committed (`compiler/` and `tools/download/` are gitignored).

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
byte-identical to a recorded manifest — catching same-version installs that
would silently change codegen:

```bash
python tools/fingerprint_compiler.py           # verify against the lock
```

A reproducible environment is defined in [Containerfile](Containerfile)
(Python 3.12 plus the host packages the EE toolchain needs):

```bash
docker build -f Containerfile -t crashwoc-decomp .
docker run --rm -it -v "$PWD:/work" crashwoc-decomp
```

## Legal

This repository contains no game assets, no game code, and no copyrighted
binaries. It is a clean-room reconstruction workspace: all original files
must be supplied by the user from their own copy of the game.
