"""Whole-.text disassembly transforms and symbol resolution.

Extracted verbatim from assemble_text.py: the byte-preserving duplicate-static
disambiguation, the instruction-comment parser, and the "resolve every
external to its true address" defsym machinery the monolithic baseline proved.
"""
import json
import re
import subprocess

from .toolchain import ROOT

# `/* fileoff vram word */` comment splat emits before every instruction. The
# word is the four instruction bytes in file (little-endian) order.
INSTR_RE = re.compile(r"/\*\s*[0-9A-Fa-f]+\s+([0-9A-Fa-f]{8})\s+([0-9A-Fa-f]{8})\s*\*/")
# Lines that define/annotate a symbol rather than reference one; excluded from
# the reference rewrite so a definition is never mistaken for a call.
DEF_PREFIXES = ("glabel ", "endlabel ", "nonmatching ", "matching ",
                "alabel ", "dlabel ", "enddlabel ", "jlabel ", "ehlabel ")
# splat's address-named auto symbols: the address is the hex suffix of the name.
AUTO_NAME_RE = re.compile(r"(?:D_|func_|jtbl_|jpt_|L)([0-9A-Fa-f]+)$")


def load_text_target(version):
    """Return (elf_bytes, text_addr, text_offset, text_size) for the version."""
    config_dir = ROOT / "config" / version
    sections = json.loads((config_dir / "sections.json").read_text())
    version_meta = json.loads((config_dir / "version.json").read_text())
    elf = (ROOT / version_meta["files"]["elf"]["path"]).read_bytes()
    text = next(s for s in sections["sections"] if s["name"] == ".text")

    def h(value):
        return int(value, 0) if isinstance(value, str) else value

    return elf, h(text["addr"]), h(text["offset"]), h(text["size"])


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
