#!/usr/bin/env python3
"""Monolithic assembly baseline: assemble .text and prove it is byte-identical.

The binary baseline carried `.text` as a raw `.incbin` blob. This step
replaces that blob with real assembled instructions: it takes splat's whole-
program `.text` disassembly (asm/text.s), assembles it with the locked PS2 `as`,
links it at the target's `.text` address, and checks that the resulting bytes
match the target's `.text` exactly. It is the first move from "raw bytes" toward
"source", and the anchor every later per-function match builds on.

The one wrinkle a single monolithic assembly must solve is duplicate labels:
four static functions (ReadNuIFFGeomSkin, ReadNuIFFGeomVtx, NuNodeRead,
_fpadd_parts) each appear in two translation units under the same name. They are
legal as separate TUs but collide in one assembly file. This tool disambiguates
them without touching a single instruction byte: every definition is renamed to
`NAME__<vram>`, and every `jal`/`j` to one of them is repointed to the
definition its own encoding already targets. The resolved addresses -- and so
the assembled bytes -- are unchanged; the final byte-for-byte compare proves it.

Every other symbol asm/text.s references (data, bss, other functions) is left
undefined by the assembler and resolved at link time to its true address, taken
from the committed mdebug registry (config/pal103/symbol_addrs.txt), splat's
auto symbol lists, or -- for splat's address-named auto symbols (`D_00633400`,
`func_001147A8`, ...) -- the address embedded in the name itself.

Needs the locked PS2 binutils, which are Linux x86-64 binaries: run this on
Linux or in the Containerfile image. Nothing game-derived is committed -- the
transformed `.s`, the object, the link script and the linked ELF all land in
gitignored build/baseline/. This script and the addresses it reads are the only
tracked inputs.

The pipeline logic lives in tools/declib/ (asmtext, toolchain); this file is
the CLI. The moved names are re-exported here so other tools' historical
`import assemble_text as at` call sites keep working.

Exit status is 0 only if the linked `.text` equals the target's `.text`.
"""
import argparse
import hashlib
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from declib.toolchain import (BINUTILS_DIR, AS, LD, NM, OBJCOPY,  # noqa: F401
                              Reporter, h, tool_path)
from declib.asmtext import (INSTR_RE, DEF_PREFIXES, AUTO_NAME_RE,  # noqa: F401
                            disambiguate, instr_at, load_symbol_addrs,
                            load_text_target, resolve, undefined_symbols)

LABEL_WIDTH = 32


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--version", default="pal103",
                        help="target version (default: pal103)")
    args = parser.parse_args()

    text_s = ROOT / "asm" / "text.s"
    macro = ROOT / "build" / "include" / "macro.inc"
    if not text_s.is_file() or not macro.is_file():
        print("asm/text.s or build/include/macro.inc missing; run "
              "`python configure.py` first (it is gitignored, game-derived).",
              file=sys.stderr)
        return 2

    as_bin, ld_bin = tool_path(AS), tool_path(LD)
    nm_bin, objcopy_bin = tool_path(NM), tool_path(OBJCOPY)
    if not all((as_bin, ld_bin, nm_bin, objcopy_bin)):
        print(f"PS2 binutils not found under {BINUTILS_DIR.relative_to(ROOT)}/ "
              "(install via tools/setup_toolchain.py; Linux x86-64 -- run this "
              "on Linux or in the Containerfile image).", file=sys.stderr)
        return 2

    elf, text_addr, text_off, text_size = load_text_target(args.version)
    out_dir = ROOT / "build" / "baseline"
    out_dir.mkdir(parents=True, exist_ok=True)

    print(f"Monolithic assembly baseline: {args.version} .text "
          f"@ 0x{text_addr:08X}, size 0x{text_size:X}\n")

    reporter = Reporter(LABEL_WIDTH)
    mono, ok = disambiguate(text_s.read_text(), reporter)
    if not ok:
        return _fail(reporter)
    mono_s = out_dir / "text_mono.s"
    mono_s.write_text(mono)

    obj = out_dir / "text_mono.o"
    try:
        subprocess.run([as_bin, "-EL", "-march=r5900", "-I", str(macro.parent),
                        "-o", str(obj), str(mono_s)],
                       check=True, capture_output=True, text=True)
    except subprocess.CalledProcessError as exc:
        reporter.result("Assemble asm/text.s", False,
                        exc.stderr.strip().splitlines()[0] if exc.stderr else "as failed")
        return _fail(reporter)
    except OSError as exc:
        reporter.result("Assemble asm/text.s", False,
                        f"cannot execute PS2 as here ({exc}); Linux x86-64 only")
        return _fail(reporter)
    reporter.result("Assemble asm/text.s", True)

    undef = undefined_symbols(nm_bin, obj)
    defsyms, unresolved = resolve(undef, load_symbol_addrs(args.version))
    reporter.result("Resolve external symbols", not unresolved,
                    None if not unresolved
                    else f"{len(unresolved)} unresolved, e.g. {unresolved[:5]}")
    if unresolved:
        return _fail(reporter)
    print(f"  {len(undef)} external symbols resolved")

    # Link .text at its target address; discard everything else. text_mono.o is
    # referenced by absolute path so the script needs no search path.
    script = (f"SECTIONS {{\n  . = 0x{text_addr:08X};\n"
              f"  .text : {{ {obj.as_posix()}(.text) }}\n"
              f"  /DISCARD/ : {{ *(*) }}\n}}\n"
              + "\n".join(f"{n} = 0x{a:08X};" for n, a in defsyms.items()) + "\n")
    link_ld = out_dir / "text_mono.ld"
    link_ld.write_text(script)
    elf_out = out_dir / "text_mono.elf"
    bin_out = out_dir / "text_mono.bin"
    try:
        subprocess.run([ld_bin, "-EL", "-T", str(link_ld), "-o", str(elf_out)],
                       check=True, capture_output=True, text=True)
        subprocess.run([objcopy_bin, "-O", "binary", "--only-section=.text",
                        str(elf_out), str(bin_out)],
                       check=True, capture_output=True, text=True)
    except subprocess.CalledProcessError as exc:
        reporter.result("Link .text", False,
                        exc.stderr.strip().splitlines()[0] if exc.stderr else "ld failed")
        return _fail(reporter)
    reporter.result("Link .text", True)

    target = elf[text_off:text_off + text_size]
    linked = bin_out.read_bytes()
    ok = linked == target
    detail = None
    if not ok:
        first = next((i for i in range(min(len(linked), len(target)))
                      if linked[i] != target[i]), min(len(linked), len(target)))
        detail = (f"len {len(linked):#x} vs {len(target):#x}; "
                  f"first diff at .text offset 0x{first:x} "
                  f"(vram 0x{text_addr + first:08X})")
    reporter.result("Assembled .text byte-identical", ok, detail)

    if reporter.failed:
        return _fail(reporter)
    print(f"\nMonolithic assembly baseline OK. .text sha256:\n  "
          f"{hashlib.sha256(target).hexdigest()}")
    return 0


def _fail(reporter):
    print("\nMonolithic assembly baseline FAILED:")
    for detail in reporter.details:
        print(f"  - {detail}")
    return 1


if __name__ == "__main__":
    sys.exit(main())
