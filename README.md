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
