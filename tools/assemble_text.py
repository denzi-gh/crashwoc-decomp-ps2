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

Exit status is 0 only if the linked `.text` equals the target's `.text`.
"""
import argparse
import hashlib
import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
LABEL_WIDTH = 32

BINUTILS_DIR = ROOT / "compiler" / "ps2-binutils-0.10"
AS = "mips-ps2-decompals-as"
LD = "mips-ps2-decompals-ld"
NM = "mips-ps2-decompals-nm"
OBJCOPY = "mips-ps2-decompals-objcopy"

# `/* fileoff vram word */` comment splat emits before every instruction. The
# word is the four instruction bytes in file (little-endian) order.
INSTR_RE = re.compile(r"/\*\s*[0-9A-Fa-f]+\s+([0-9A-Fa-f]{8})\s+([0-9A-Fa-f]{8})\s*\*/")
# Lines that define/annotate a symbol rather than reference one; excluded from
# the reference rewrite so a definition is never mistaken for a call.
DEF_PREFIXES = ("glabel ", "endlabel ", "nonmatching ", "matching ",
                "alabel ", "dlabel ", "enddlabel ", "jlabel ", "ehlabel ")
# splat's address-named auto symbols: the address is the hex suffix of the name.
AUTO_NAME_RE = re.compile(r"(?:D_|func_|jtbl_|jpt_|L)([0-9A-Fa-f]+)$")


class Reporter:
    def __init__(self):
        self.failed = False
        self.details = []

    def result(self, label, ok, detail=None):
        print(f"{label + ':':<{LABEL_WIDTH}} {'PASS' if ok else 'FAIL'}")
        if not ok:
            self.failed = True
            if detail:
                self.details.append(f"{label}: {detail}")


def h(value):
    return int(value, 0) if isinstance(value, str) else value


def load_text_target(version):
    """Return (elf_bytes, text_addr, text_offset, text_size) for the version."""
    config_dir = ROOT / "config" / version
    sections = json.loads((config_dir / "sections.json").read_text())
    version_meta = json.loads((config_dir / "version.json").read_text())
    elf = (ROOT / version_meta["files"]["elf"]["path"]).read_bytes()
    text = next(s for s in sections["sections"] if s["name"] == ".text")
    return elf, h(text["addr"]), h(text["offset"]), h(text["size"])


def tool_path(name):
    local = BINUTILS_DIR / name
    return str(local) if local.is_file() else None


def instr_at(lines, i):
    """(vram, word) decoded from the instruction comment on line i, else None."""
    m = INSTR_RE.search(lines[i])
    if not m:
        return None
    return int(m.group(1), 16), int.from_bytes(bytes.fromhex(m.group(2)), "little")


def disambiguate(text, reporter):
    """Rename duplicate static defs and repoint their calls; return new text.

    Returns (transformed_text, ok). The transform is byte-preserving: only
    symbol *names* change, and every call is repointed to the same address it
    already encoded, so no instruction's bytes move.
    """
    lines = text.splitlines()
    glabels = [l.split()[1] for l in lines if l.startswith("glabel ")]
    dups = {n for n in glabels if glabels.count(n) > 1}
    if not dups:
        return text, True

    out = list(lines)
    uniq = lambda name, addr: f"{name}__{addr:08X}"

    # Rename each duplicate definition (its glabel plus the paired nonmatching
    # above and endlabel below) to NAME__<vram of the function's first byte>.
    for i, line in enumerate(lines):
        if not (line.startswith("glabel ") and line.split()[1] in dups):
            continue
        name = line.split()[1]
        j = i + 1
        while j < len(lines) and instr_at(lines, j) is None:
            j += 1
        if j >= len(lines):
            reporter.result("Disambiguate duplicate statics", False,
                            f"no instruction found after 'glabel {name}'")
            return text, False
        u = uniq(name, instr_at(lines, j)[0])
        out[i] = line.replace(name, u, 1)
        for b in range(i - 1, max(-1, i - 6), -1):
            if lines[b].startswith("nonmatching ") and \
                    lines[b].split()[1].rstrip(",") == name:
                out[b] = lines[b].replace(name, u, 1)
                break
        for f in range(i + 1, len(lines)):
            if lines[f].startswith("endlabel ") and lines[f].split()[1] == name:
                out[f] = lines[f].replace(name, u, 1)
                break

    # Repoint every call to a duplicate name to the definition its own encoding
    # targets (top address nibble is 0 across the loaded image).
    problems = []
    for i, line in enumerate(lines):
        if line.startswith(DEF_PREFIXES):
            continue
        toks = line.split()
        if not toks or toks[-1] not in dups:
            continue
        info = instr_at(lines, i)
        mnem = line.split("*/", 1)[1].split()[0] if "*/" in line else None
        if info and mnem in ("jal", "j"):
            target = (info[1] & 0x03FFFFFF) << 2
            out[i] = line.replace(toks[-1], uniq(toks[-1], target))
        else:
            problems.append(f"line {i + 1}: {mnem or '?'} references "
                            f"duplicate '{toks[-1]}' (only jal/j handled)")

    ok = not problems
    detail = None if ok else "; ".join(problems)
    reporter.result("Disambiguate duplicate statics", ok,
                    detail or f"{len(dups)} names, {len(dups) * 2} defs renamed")
    if ok:
        # A visible note; not a failure.
        print(f"  {', '.join(sorted(dups))}")
    return ("\n".join(out) + "\n"), ok


def load_symbol_addrs(version):
    """name -> address from the committed registry and splat's auto lists."""
    addrs = {}
    sources = [
        ROOT / "config" / version / "symbol_addrs.txt",
        ROOT / "build" / "undefined_syms_auto.txt",
        ROOT / "build" / "undefined_funcs_auto.txt",
    ]
    line_re = re.compile(r"(\S+)\s*=\s*(0x[0-9A-Fa-f]+)")
    for path in sources:
        if not path.is_file():
            continue
        for line in path.read_text().splitlines():
            m = line_re.match(line.split("//")[0].strip().rstrip(";"))
            if m:
                addrs.setdefault(m.group(1), int(m.group(2), 16))
    return addrs


def undefined_symbols(nm_bin, obj):
    out = subprocess.run([nm_bin, str(obj)], check=True,
                         capture_output=True, text=True).stdout
    return sorted({line.split()[-1] for line in out.splitlines()
                   if re.match(r"\s+U ", line)})


def resolve(undef, addrs):
    """(defsyms, unresolved) -- map each undefined symbol to its address."""
    defsyms, unresolved = {}, []
    for name in undef:
        if name in addrs:
            defsyms[name] = addrs[name]
            continue
        m = AUTO_NAME_RE.fullmatch(name)
        if m:
            defsyms[name] = int(m.group(1), 16)
        else:
            unresolved.append(name)
    return defsyms, unresolved


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

    reporter = Reporter()
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
