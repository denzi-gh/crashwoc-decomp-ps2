#!/usr/bin/env python3
"""Attribute the data sections to translation units from .mdebug evidence.

Produces config/<version>/data_map.toml: an exact tiling of every allocated
non-code section (.data, .rodata, .lit4, .sdata, .sbss, .bss) plus the 0x80
un-sectioned orphan run at 0x00293680, into ranges that are either

  owner = 'unit-NNNN', evidence = 'mdebug'   -- proven ownership
  owner = 'unassigned', evidence = 'gap'     -- bytes nothing attributes
  owner = 'unassigned', evidence = 'orphan'  -- the orphan run

Attribution sources, both read straight from the retail `.mdebug`:

  * external symbols (EXTR): `ifd` names the unit that DEFINES each global;
    data globals are those whose address lands in a data section and whose
    storage class is a data class.
  * per-unit local symbols (SYMR): stStatic entries with a data storage
    class attribute file-local statics.

Nothing is guessed. Within a section the attributed symbols must form
non-interleaved per-unit runs (each unit's data is contiguous, as .text is);
a section that violates this stays whole-section unassigned and is reported.
A unit's owned range ends at its LAST symbol's address -- symbol sizes are
not recorded in .mdebug, so the bytes from the last symbol to the next run
are an explicit gap, promotable later only by evidence (e.g. the unit's C
emits data whose bytes match exactly). Consequently a single-symbol run
proves ownership of no measurable extent yet and stays a gap.

The EXTR layout guess (reserved:16 | ifd:16, little-endian) is validated on
every run: each procedure external's ifd must equal the unit its PDR
belongs to, across all 3.7k procedures, before any attribution is trusted.

Usage:
  python tools/extract_data_map.py            # regenerate the map
  python tools/extract_data_map.py --check    # verify committed == fresh

Stdlib only; needs the target ELF (user-supplied, never committed).
"""
import argparse
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from extract_mdebug import (MDebug, SC_DATA_CLASSES, ST_GLOBAL, ST_PROC,
                            ST_STATIC_PROC, find_section)

ORPHAN_START = 0x00293680          # un-sectioned loaded MIPS bytes (0x80)
ORPHAN_END = 0x00293700


def data_sections(sections_spec):
    """[(name, start, end)] for allocated, non-exec sections with a size."""
    out = []
    for s in sections_spec["sections"]:
        flags, addr = int(s["flags"], 0), int(s["addr"], 0)
        size, stype = int(s["size"], 0), int(s["type"], 0)
        if stype not in (1, 8):                # PROGBITS / NOBITS
            continue
        if flags & 0x2 and not flags & 0x4 and addr and size:
            out.append((s["name"], addr, addr + size))
    return sorted(out, key=lambda r: r[1])


def validate_extr_ifd(md):
    """Prove the EXTR ifd field is where we think it is, or die.

    Every procedure has both a PDR (with its owning unit) and usually an
    EXTR; the two unit attributions must agree for every single procedure
    external. Returns the number of externals checked.
    """
    proc_unit = {}
    for u in md.units():
        for f in md.procedures(u):
            if f["name"]:
                proc_unit.setdefault((f["address"], f["name"]), f["unit"])

    checked = 0
    for name, value, st, _sc, ifd in md.externals():
        if st not in (ST_PROC, ST_STATIC_PROC) or not name:
            continue
        unit = proc_unit.get((value, name))
        if unit is None:
            continue
        if ifd != unit:
            raise ValueError(
                f"EXTR layout validation failed: {name} @ {value:#010x} has "
                f"ifd {ifd} but its procedure belongs to unit {unit}")
        checked += 1
    if checked == 0:
        raise ValueError("EXTR layout validation checked nothing")
    return checked


def attributed_symbols(md, ifd_max):
    """{(address, unit)} for every data symbol .mdebug attributes to a unit."""
    out = set()
    for name, value, st, sc, ifd in md.externals():
        if (st == ST_GLOBAL and sc in SC_DATA_CLASSES and name
                and 0 <= ifd < ifd_max):
            out.add((value, ifd))
    for u in md.units():
        for _name, addr, _sc in md.local_data_symbols(u):
            out.add((addr, u["index"]))
    return out


def build_ranges(lo, hi, syms):
    """Tile [lo, hi) into (start, end, owner, evidence) ranges.

    `syms` is [(addr, unit)] inside the section. Returns None when the
    section's unit sequence interleaves (or two units claim one address) --
    the caller degrades it to whole-section unassigned.
    """
    by_addr = {}
    for addr, unit in sorted(syms):
        if by_addr.get(addr, unit) != unit:
            return None                        # two units claim one address
        by_addr[addr] = unit
    ordered = sorted(by_addr.items())

    runs, seen = [], set()
    for addr, unit in ordered:
        if runs and runs[-1][0] == unit:
            runs[-1][2] = addr
        else:
            if unit in seen:
                return None                    # unit reappears: interleaved
            seen.add(unit)
            runs.append([unit, addr, addr])

    ranges, cursor = [], lo
    for unit, first, last in runs:
        if first > cursor:
            ranges.append((cursor, first, "unassigned", "gap"))
            cursor = first
        if last > first:                       # single-symbol runs prove no extent
            ranges.append((first, last, f"unit-{unit:04d}", "mdebug"))
            cursor = last
    if cursor < hi:
        ranges.append((cursor, hi, "unassigned", "gap"))
    return ranges


def build_map(root, version):
    """Return (map_text, stats) for the version."""
    config_dir = root / "config" / version
    version_meta = json.loads((config_dir / "version.json").read_text())
    sections_spec = json.loads((config_dir / "sections.json").read_text())
    elf_rel = version_meta["files"]["elf"]["path"]
    elf_path = root / elf_rel
    if not elf_path.is_file():
        raise FileNotFoundError(f"target ELF not found at {elf_rel}")
    data = elf_path.read_bytes()
    md_off, _md_size = find_section(data, ".mdebug")
    md = MDebug(data, md_off)

    checked = validate_extr_ifd(md)
    syms = attributed_symbols(md, md.h["ifdMax"])

    lines = [
        "# Generated by tools/extract_data_map.py from the .mdebug section of",
        f"# {elf_rel}. Do not edit by hand: regenerate with",
        "# `python tools/extract_data_map.py`.",
        "__SUMMARY__",
        "",
        "schema = 1",
        "",
    ]
    stats = {"extr_checked": checked, "symbols": len(syms), "sections": {}}

    def emit(section, ranges):
        for start, end, owner, evidence in ranges:
            lines.append("[[range]]")
            lines.append(f"section = '{section}'")
            lines.append(f"start = {start:#010x}")
            lines.append(f"end = {end:#010x}")
            lines.append(f"owner = '{owner}'")
            lines.append(f"evidence = '{evidence}'")
            lines.append("")

    emit("orphan", [(ORPHAN_START, ORPHAN_END, "unassigned", "orphan")])

    for name, lo, hi in data_sections(sections_spec):
        in_section = [(a, u) for a, u in syms if lo <= a < hi]
        ranges = build_ranges(lo, hi, in_section)
        degraded = ranges is None
        if degraded:
            ranges = [(lo, hi, "unassigned", "gap")]
        # Tiling invariant: contiguous, exact cover.
        cursor = lo
        for start, end, _o, _e in ranges:
            if start != cursor or end <= start:
                raise ValueError(f"{name}: tiling broken at {start:#x}")
            cursor = end
        if cursor != hi:
            raise ValueError(f"{name}: tiling ends at {cursor:#x} != {hi:#x}")
        emit(name, ranges)
        owned = sum(e - s for s, e, o, _ev in ranges if o != "unassigned")
        stats["sections"][name] = {
            "ranges": len(ranges),
            "owned_ranges": sum(1 for r in ranges if r[2] != "unassigned"),
            "symbols": len(in_section),
            "owned_bytes": owned,
            "total_bytes": hi - lo,
            "interleaved": degraded,
        }

    total = sum(s["total_bytes"] for s in stats["sections"].values())
    owned = sum(s["owned_bytes"] for s in stats["sections"].values())
    n_ranges = sum(s["ranges"] for s in stats["sections"].values()) + 1
    lines[3] = (f"# {n_ranges} ranges; {owned:#x} of {total:#x} data bytes "
                f"attributed by mdebug evidence.")
    return "\n".join(lines).rstrip() + "\n", stats


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--version", default="pal103")
    parser.add_argument("--root", type=Path,
                        default=Path(__file__).resolve().parent.parent)
    parser.add_argument("--check", action="store_true",
                        help="verify the committed map matches a fresh "
                             "extraction instead of writing it")
    args = parser.parse_args()

    try:
        text, stats = build_map(args.root, args.version)
    except (OSError, ValueError, KeyError, json.JSONDecodeError) as exc:
        print(f"data map extraction failed: {exc}", file=sys.stderr)
        return 1

    print(f"EXTR ifd validated against {stats['extr_checked']} procedure "
          f"externals; {stats['symbols']} data symbols attributed.")
    for name, s in stats["sections"].items():
        note = "  INTERLEAVED -> whole section unassigned" \
            if s["interleaved"] else ""
        print(f"  {name:<9} {s['symbols']:>5} syms  "
              f"{s['owned_ranges']:>4} owned ranges  "
              f"{s['owned_bytes']:#9x} / {s['total_bytes']:#9x} bytes{note}")

    out = args.root / "config" / args.version / "data_map.toml"
    rel = out.relative_to(args.root)
    if args.check:
        current = out.read_text() if out.is_file() else None
        if current == text:
            print(f"{rel}: PASS")
            return 0
        print(f"{rel}: FAIL -- differs from a fresh extraction "
              f"(or does not exist); regenerate it.")
        return 1
    out.write_text(text)
    print(f"wrote {rel}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
