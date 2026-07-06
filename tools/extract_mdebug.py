#!/usr/bin/env python3
"""Extract the MIPS ECOFF symbolic debug (.mdebug) tables from the target ELF.

The retail SLES_503.86 executable ships a full `.mdebug` section (SHT_MIPS_DEBUG,
magic 0x7009). It is the single richest description of the program's structure:
every translation unit, every procedure, their addresses, stack frames and saved
register masks. This tool parses it and emits three deterministic registries that
later tooling (splat, the TU split, objdiff) builds on:

  config/<version>/units.toml        one entry per file descriptor  (267)
  config/<version>/functions.toml    one entry per procedure descr. (3751)
  config/<version>/symbol_addrs.txt  splat-style name = address list

Nothing here is heuristic: the tables are read straight out of the binary in
their native order, so the output is a byte-exact function of the input ELF.

Stdlib only; runs on Python 3.11+.

Usage:
  python tools/extract_mdebug.py            # regenerate the registries
  python tools/extract_mdebug.py --check    # verify committed == regenerated
  python tools/extract_mdebug.py --version pal103
"""
import argparse
import json
import struct
import sys
from pathlib import Path

# ECOFF symbolic header magic and the structure sizes this build uses.
HDR_MAGIC = 0x7009
FDR_SIZE = 72
PDR_SIZE = 52
SYMR_SIZE = 12
EXTR_SIZE = 16

# HDRR (symbolic header) field order: 2 shorts then 23 ints.
HDRR_FMT = "<hhiiiiiiiiiiiiiiiiiiiiiii"
HDRR_FIELDS = (
    "magic", "vstamp", "ilineMax", "cbLine", "cbLineOffset", "idnMax",
    "cbDnOffset", "ipdMax", "cbPdOffset", "isymMax", "cbSymOffset", "ioptMax",
    "cbOptOffset", "iauxMax", "cbAuxOffset", "issMax", "cbSsOffset",
    "issExtMax", "cbSsExtOffset", "ifdMax", "cbFdOffset", "crfd",
    "cbRfdOffset", "iextMax", "cbExtOffset",
)

FDR_FMT = "<IiiiiiiiiiHHiiiiIii"   # adr..cbLine, 72 bytes
PDR_FMT = "<Iiiiiiiiihhiii"        # adr..cbLineOffset, 52 bytes

# Symbol type (st) and storage class (sc) codes we care about.
ST_PROC = 6
ST_STATIC_PROC = 14
ST_GLOBAL = 1

# MIPS $sp / $ra, used to render the frame/return registers legibly.
REG_NAMES = {29: "sp", 30: "fp", 31: "ra", 0: "zero"}


# --------------------------------------------------------------------------- #
# ELF section lookup                                                          #
# --------------------------------------------------------------------------- #

def find_section(data, name):
    """Return (offset, size) of a named section from the ELF section table."""
    if data[:4] != b"\x7fELF":
        raise ValueError("not an ELF file")
    (e_shoff,) = struct.unpack_from("<I", data, 32)
    (e_shentsize, e_shnum, e_shstrndx) = struct.unpack_from("<HHH", data, 46)
    shstr_off = struct.unpack_from(
        "<I", data, e_shoff + e_shstrndx * e_shentsize + 16)[0]

    def sname(name_off):
        end = data.index(b"\x00", shstr_off + name_off)
        return data[shstr_off + name_off:end].decode("ascii")

    for i in range(e_shnum):
        base = e_shoff + i * e_shentsize
        sh_name, _sh_type, _flags, _addr, sh_offset, sh_size = \
            struct.unpack_from("<IIIIII", data, base)
        if sname(sh_name) == name:
            return sh_offset, sh_size
    raise ValueError(f"section {name!r} not found")


# --------------------------------------------------------------------------- #
# .mdebug parsing                                                             #
# --------------------------------------------------------------------------- #

class MDebug:
    """Parsed view over a .mdebug section.

    Every cb*Offset in the header is an absolute file offset, so the tables are
    read straight from `data` at those offsets.
    """

    def __init__(self, data, md_offset):
        self.data = data
        fields = struct.unpack_from(HDRR_FMT, data, md_offset)
        self.h = dict(zip(HDRR_FIELDS, fields))
        if self.h["magic"] != HDR_MAGIC:
            raise ValueError(
                f".mdebug magic {self.h['magic']:#06x} != {HDR_MAGIC:#06x}")

    def cstr(self, base):
        end = self.data.index(b"\x00", base)
        return self.data[base:end].decode("latin-1")

    def local_symbol(self, index):
        """(iss, value, st, sc) for local symbol `index`."""
        iss, value, bits = struct.unpack_from(
            "<iiI", self.data, self.h["cbSymOffset"] + index * SYMR_SIZE)
        return iss, value, bits & 0x3f, (bits >> 6) & 0x1f

    def units(self):
        """Yield one dict per file descriptor, in native table order."""
        for i in range(self.h["ifdMax"]):
            off = self.h["cbFdOffset"] + i * FDR_SIZE
            (adr, rss, issBase, cbSs, isymBase, csym, ilineBase, cline,
             ioptBase, copt, ipdFirst, cpd, iauxBase, caux, rfdBase, crfd,
             bits, cbLineOffset, cbLine) = struct.unpack_from(FDR_FMT, self.data, off)
            name = self.cstr(self.h["cbSsOffset"] + issBase + rss) \
                if rss != -1 else ""
            yield {
                "index": i,
                "name": name,
                "address": adr,
                "lang": bits & 0x1f,
                "sym_first": isymBase,
                "sym_count": csym,
                "proc_first": ipdFirst,
                "proc_count": cpd,
                "ss_base": issBase,
            }

    def procedures(self, unit):
        """Yield one dict per procedure of `unit`, in native table order."""
        for p in range(unit["proc_count"]):
            pidx = unit["proc_first"] + p
            off = self.h["cbPdOffset"] + pidx * PDR_SIZE
            (adr, isym, iline, regmask, regoffset, iopt, fregmask, fregoffset,
             frameoffset, framereg, pcreg, lnLow, lnHigh,
             cbLineOffset) = struct.unpack_from(PDR_FMT, self.data, off)
            name = ""
            if isym != -1:
                siss, _v, _st, _sc = self.local_symbol(
                    unit["sym_first"] + isym)
                name = self.cstr(self.h["cbSsOffset"] + unit["ss_base"] + siss)
            yield {
                "index": pidx,
                "name": name,
                "address": (unit["address"] + adr) & 0xFFFFFFFF,
                "unit": unit["index"],
                "frame_size": frameoffset,
                "frame_reg": framereg,
                "return_reg": pcreg,
                "reg_mask": regmask & 0xFFFFFFFF,
                "reg_offset": regoffset,
                "freg_mask": fregmask & 0xFFFFFFFF,
                "freg_offset": fregoffset,
                "line_low": lnLow,
                "line_high": lnHigh,
            }

    def externals(self):
        """Yield (name, value, st, sc) per external symbol, in table order."""
        for i in range(self.h["iextMax"]):
            _flags_ifd, iss, value, bits = struct.unpack_from(
                "<IiiI", self.data, self.h["cbExtOffset"] + i * EXTR_SIZE)
            name = self.cstr(self.h["cbSsExtOffset"] + iss) if iss != -1 else ""
            yield name, value & 0xFFFFFFFF, bits & 0x3f, (bits >> 6) & 0x1f


# --------------------------------------------------------------------------- #
# Minimal TOML emitter (stdlib has no writer)                                 #
# --------------------------------------------------------------------------- #

def toml_str(value):
    """Render a string as TOML, preferring a literal single-quoted string.

    Source paths carry backslashes, which a basic (double-quoted) TOML string
    would treat as escapes; a literal string keeps them verbatim. Fall back to
    an escaped basic string only if the value can't be a literal.
    """
    if "'" not in value and "\n" not in value and "\r" not in value \
            and all(ord(c) >= 0x20 for c in value):
        return f"'{value}'"
    escaped = (value.replace("\\", "\\\\").replace('"', '\\"')
               .replace("\n", "\\n").replace("\r", "\\r").replace("\t", "\\t"))
    return f'"{escaped}"'


def reg_name(num):
    return REG_NAMES.get(num, f"${num}")


# --------------------------------------------------------------------------- #
# Registry rendering                                                          #
# --------------------------------------------------------------------------- #

def render_units(md, provenance):
    lines = [provenance, ""]
    count = 0
    for u in md.units():
        count += 1
        lines.append("[[unit]]")
        lines.append(f"index = {u['index']}")
        lines.append(f"name = {toml_str(u['name'])}")
        lines.append(f"address = {u['address']:#010x}")
        lines.append(f"lang = {u['lang']}")
        lines.append(f"proc_first = {u['proc_first']}")
        lines.append(f"proc_count = {u['proc_count']}")
        lines.append(f"sym_first = {u['sym_first']}")
        lines.append(f"sym_count = {u['sym_count']}")
        lines.append("")
    header = f"# {count} translation units (file descriptors)."
    lines[0] = lines[0].replace("__COUNT__", header)
    return "\n".join(lines).rstrip() + "\n", count


def render_functions(md, provenance):
    lines = [provenance, ""]
    count = 0
    for u in md.units():
        for f in md.procedures(u):
            count += 1
            lines.append("[[function]]")
            lines.append(f"index = {f['index']}")
            lines.append(f"name = {toml_str(f['name'])}")
            lines.append(f"address = {f['address']:#010x}")
            lines.append(f"unit = {f['unit']}")
            lines.append(f"frame_size = {f['frame_size']}")
            lines.append(f"frame_reg = {toml_str(reg_name(f['frame_reg']))}")
            lines.append(f"return_reg = {toml_str(reg_name(f['return_reg']))}")
            lines.append(f"reg_mask = {f['reg_mask']:#010x}")
            lines.append(f"reg_offset = {f['reg_offset']}")
            lines.append(f"freg_mask = {f['freg_mask']:#010x}")
            lines.append(f"freg_offset = {f['freg_offset']}")
            # Line bounds are only present for units built with debug lines
            # (here, the SCE runtime .s files); -1 means "no line info".
            if f["line_low"] != -1 or f["line_high"] != -1:
                lines.append(f"line_low = {f['line_low']}")
                lines.append(f"line_high = {f['line_high']}")
            lines.append("")
    header = f"# {count} procedures (procedure descriptors)."
    lines[0] = lines[0].replace("__COUNT__", header)
    return "\n".join(lines).rstrip() + "\n", count


def section_ranges(sections_spec):
    """Address ranges of allocated sections, for validating data symbols."""
    ranges = []
    for s in sections_spec["sections"]:
        flags = int(s["flags"], 0)
        addr = int(s["addr"], 0)
        size = int(s["size"], 0)
        if flags & 0x2 and addr and size:      # SHF_ALLOC, real address
            ranges.append((addr, addr + size, s["name"]))
    return ranges


def in_allocated(addr, ranges):
    return any(lo <= addr < hi for lo, hi, _ in ranges)


def render_symbol_addrs(md, sections_spec, provenance):
    """Splat-style `name = 0xADDR; // type:func` list, sorted by address.

    Functions come from the procedure table (authoritative, covers statics).
    Global data comes from external symbols that land in an allocated data
    section, so a symbol whose ECOFF `value` is a size rather than an address
    (unlinked commons) can never slip in.
    """
    ranges = section_ranges(sections_spec)
    # (address, name) -> type. Dedupe identical entries deterministically.
    symbols = {}

    for u in md.units():
        for f in md.procedures(u):
            if f["name"]:
                symbols[(f["address"], f["name"])] = "func"

    for name, value, st, sc in md.externals():
        if not name or st in (ST_PROC, ST_STATIC_PROC):
            continue                            # procs already covered above
        if st != ST_GLOBAL:
            continue
        if not in_allocated(value, ranges):
            continue                            # skip abs consts / sizes
        symbols.setdefault((value, name), None)

    lines = [provenance.replace("__COUNT__",
             f"# {len(symbols)} symbols (functions + global data)."),
             ""]
    for (addr, name) in sorted(symbols, key=lambda k: (k[0], k[1])):
        kind = symbols[(addr, name)]
        suffix = f" // type:{kind}" if kind else ""
        lines.append(f"{name} = {addr:#010x};{suffix}")
    return "\n".join(lines) + "\n", len(symbols)


# --------------------------------------------------------------------------- #
# Driver                                                                      #
# --------------------------------------------------------------------------- #

def build(root, version):
    config_dir = root / "config" / version
    version_meta = json.loads((config_dir / "version.json").read_text())
    sections_spec = json.loads((config_dir / "sections.json").read_text())

    elf_rel = version_meta["files"]["elf"]["path"]
    elf_path = root / elf_rel
    if not elf_path.is_file():
        raise FileNotFoundError(
            f"target ELF not found at {elf_rel}; supply it and re-run "
            f"(see README). tools/verify_target.py checks it.")

    data = elf_path.read_bytes()
    md_off, md_size = find_section(data, ".mdebug")

    md = MDebug(data, md_off)
    provenance = (
        "# Generated by tools/extract_mdebug.py from the .mdebug section of\n"
        f"# {elf_rel} (offset {md_off:#x}, size {md_size:#x}). Do not edit by\n"
        "# hand: regenerate with `python tools/extract_mdebug.py`.\n"
        "__COUNT__")

    units_text, n_units = render_units(md, provenance)
    funcs_text, n_funcs = render_functions(md, provenance)
    syms_text, n_syms = render_symbol_addrs(md, sections_spec, provenance)

    return {
        config_dir / "units.toml": (units_text, n_units, "units"),
        config_dir / "functions.toml": (funcs_text, n_funcs, "procedures"),
        config_dir / "symbol_addrs.txt": (syms_text, n_syms, "symbols"),
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--version", default="pal103")
    parser.add_argument("--root", type=Path,
                        default=Path(__file__).resolve().parent.parent)
    parser.add_argument("--check", action="store_true",
                        help="verify committed files match a fresh extraction "
                             "instead of writing them")
    args = parser.parse_args()

    try:
        outputs = build(args.root, args.version)
    except (OSError, ValueError, KeyError, json.JSONDecodeError) as exc:
        print(f"extraction failed: {exc}", file=sys.stderr)
        return 1

    failed = False
    for path, (text, count, label) in outputs.items():
        rel = path.relative_to(args.root)
        if args.check:
            current = path.read_text() if path.is_file() else None
            ok = current == text
            status = "PASS" if ok else "FAIL"
            print(f"{str(rel) + ':':<34} {status}  ({count} {label})")
            if not ok:
                failed = True
                if current is None:
                    print(f"  - {rel} does not exist; run without --check")
                else:
                    print(f"  - {rel} differs from a fresh extraction")
        else:
            path.write_text(text)
            print(f"wrote {rel}  ({count} {label})")

    if args.check and failed:
        print("\n.mdebug registries are stale; regenerate them.")
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
