#!/usr/bin/env python3
"""Attribute the data sections to translation units from ELF evidence.

Produces config/<version>/data_map.toml: an exact tiling of every allocated
non-code section (.data, .rodata, .lit4, .sdata, .sbss, .bss) plus the 0x80
un-sectioned orphan run at 0x00293680, into ranges that are either

  owner = 'unit-NNNN', evidence = 'mdebug'   -- named symbols prove it (the
                                                unit's own code references
                                                may extend the run)
  owner = 'unit-NNNN', evidence = 'xref'     -- only the unit's code does
  owner = 'unassigned', evidence = 'gap'     -- bytes nothing attributes
  owner = 'unassigned', evidence = 'orphan'  -- the orphan run

Attribution sources, all read straight from the retail ELF:

  * named symbols (tools/extract_data_symbols.py): the EXTR `ifd` names the
    unit that DEFINES each global, a unit's SYMR run its statics; `.symtab`
    (or STABS) gives the size of most of them.
  * code cross-references (declib.elf.code_refs over .text): an unnamed
    .rodata/.lit4 address that exactly one unit's code builds, outside every
    sized named symbol, is that unit's (strings, jump tables, literal pools).
    Not used for .data/.sdata, where code also reaches into other units'
    globals by field offset.

Nothing is guessed. Within a section the items must form non-interleaved
per-unit runs (each unit's data is contiguous, as .text is). Two units naming
one address, or interleaved named runs, leave the whole section unassigned
(reported); a code reference that would interleave is dropped instead. A run
ends at max(address + size) over its items (a code reference counts its
access width: lw/lwc1 4, ld 8, lq 16, an address-only addiu 0), so an
unsized trailing symbol owns nothing past its address and a run of one
unsized symbol owns nothing at all: those bytes stay an explicit gap.

The EXTR layout guess (reserved:16 | ifd:16, little-endian) is validated on
every run: each procedure external's ifd must equal the unit its PDR
belongs to, across all 3.7k procedures, before any attribution is trusted.

Usage:
  python tools/extract_data_map.py            # regenerate the map
  python tools/extract_data_map.py --check    # verify committed == fresh

Stdlib only; needs the target ELF (user-supplied, never committed).
"""
import argparse
import bisect
import json
import sys
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from declib.elf import code_refs
from extract_data_symbols import collect
from extract_mdebug import (MDebug, SC_DATA_CLASSES, ST_GLOBAL, ST_PROC,
                            ST_STATIC_PROC, find_section)

ORPHAN_START = 0x00293680          # un-sectioned loaded MIPS bytes (0x80)
ORPHAN_END = 0x00293700
# Sections whose unnamed bytes code cross-references may attribute.
XREF_SECTIONS = (".rodata", ".lit4")


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


def code_units(md):
    """Sorted [(first vram, unit)] runs of .text from the procedure table."""
    procs = sorted((f["address"], f["unit"]) for u in md.units()
                   for f in md.procedures(u))
    runs = []
    for addr, unit in procs:
        if not runs or runs[-1][1] != unit:
            runs.append((addr, unit))
    return runs


def code_xref(md, elf, sections_spec):
    """[(address, width, unit, Ref)] for every data reference in .text."""
    text = next(s for s in sections_spec["sections"] if s["name"] == ".text")
    addr, off = int(text["addr"], 0), int(text["offset"], 0)
    size = int(text["size"], 0)
    starts = [f["address"] for u in md.units() for f in md.procedures(u)]
    refs = code_refs(elf[off:off + size], addr, starts,
                     int(sections_spec["gp_value"], 0))
    runs = code_units(md)
    firsts = [a for a, _u in runs]
    out = []
    for ref in refs:
        i = bisect.bisect_right(firsts, ref.vram) - 1
        if i >= 0:
            out.append((ref.address, ref.width, runs[i][1], ref))
    return out


def xref_items(refs, named, lo, hi):
    """[(start, end, unit, 'xref')] for unnamed bytes in [lo, hi) that
    exactly one unit's code references, outside every sized named item.

    `refs` is code_xref(); `named` the section's named items. The end is the
    widest access at that address (0 for an address-only reference)."""
    sized = sorted((s, e) for s, e, _u, _ev in named if e > s)
    sized_starts = [s for s, _e in sized]

    def inside_sized(addr):
        i = bisect.bisect_right(sized_starts, addr) - 1
        return i >= 0 and sized[i][0] <= addr < sized[i][1]

    units, width = defaultdict(set), defaultdict(int)
    for addr, w, unit, _ref in refs:
        if lo <= addr < hi:
            units[addr].add(unit)
            width[addr] = max(width[addr], w)
    out = []
    for addr in sorted(units):
        if len(units[addr]) != 1 or inside_sized(addr):
            continue
        (unit,) = units[addr]
        out.append((addr, min(addr + width[addr], hi), unit, "xref"))
    return out


def _span(item):
    """The bytes an item occupies for overlap tests (a point counts as 1)."""
    start, end = item[0], item[1]
    return start, max(end, start + 1)


def drop_conflicting_xref(named, xref):
    """The xref items that join their unit's run without crossing another
    unit's items. An item whose run would reach over another unit's named
    item is dropped; if two units' runs still overlap, both units' xref items
    are dropped. Named items are never dropped."""
    hulls = {}
    for item in named:
        lo, hi = _span(item)
        cur = hulls.get(item[2])
        hulls[item[2]] = (lo, hi) if cur is None else (min(cur[0], lo),
                                                       max(cur[1], hi))
    others = sorted(_span(item) + (item[2],) for item in named)
    kept = []
    for item in xref:
        unit = item[2]
        lo, hi = _span(item)
        if unit in hulls:
            lo, hi = min(lo, hulls[unit][0]), max(hi, hulls[unit][1])
        if any(u != unit and s < hi and lo < e for s, e, u in others):
            continue
        kept.append(item)
    while True:
        spans = defaultdict(list)
        for item in named + kept:
            spans[item[2]].append(_span(item))
        hull = {u: (min(s for s, _e in v), max(e for _s, e in v))
                for u, v in spans.items()}
        contested = set()
        order = sorted(hull, key=lambda u: hull[u])
        for a, b in zip(order, order[1:]):
            if hull[b][0] < hull[a][1]:
                contested.update((a, b))
        if not any(item[2] in contested for item in kept):
            return kept
        kept = [item for item in kept if item[2] not in contested]


def build_ranges(lo, hi, items):
    """Tile [lo, hi) into (start, end, owner, evidence) ranges.

    `items` are (start, end, unit, evidence) inside the section -- named
    symbols ('mdebug'; end = address + size, or the address when unsized)
    and code references ('xref') -- or bare (address, unit) pairs, which are
    unsized named symbols. Returns None when the named items interleave (or
    two units name one address); the caller degrades the section to
    whole-section unassigned. Conflicting xref items are dropped first.
    """
    items = [it if len(it) == 4 else (it[0], it[0], it[1], "mdebug")
             for it in items]
    named = [it for it in items if it[3] == "mdebug"]
    xref = [it for it in items if it[3] == "xref"]

    by_addr = {}
    for start, _end, unit, _ev in sorted(named):
        if by_addr.get(start, unit) != unit:
            return None                        # two units claim one address
        by_addr[start] = unit
    seen, prev = set(), None
    for start in sorted(by_addr):
        unit = by_addr[start]
        if unit != prev:
            if unit in seen:
                return None                    # unit reappears: interleaved
            seen.add(unit)
            prev = unit

    kept = drop_conflicting_xref(named, xref)
    runs = []                                  # [unit, start, end, evidence]
    for start, end, unit, evidence in sorted(named + kept,
                                             key=lambda it: (it[0], it[1])):
        if runs and runs[-1][0] == unit:
            run = runs[-1]
            run[2] = max(run[2], end)
            if evidence == "mdebug":
                run[3] = "mdebug"
        else:
            if any(r[0] == unit for r in runs):
                raise ValueError(f"unit-{unit:04d} interleaves at {start:#x}")
            runs.append([unit, start, end, evidence])

    ranges, cursor = [], lo
    for unit, first, last, evidence in runs:
        if first < cursor:
            raise ValueError(f"unit-{unit:04d} at {first:#x} overlaps the "
                             f"previous range ending at {cursor:#x}")
        if first > cursor:
            ranges.append((cursor, first, "unassigned", "gap"))
            cursor = first
        if last > first:                       # an unsized point owns nothing
            ranges.append((first, last, f"unit-{unit:04d}", evidence))
            cursor = last
    if cursor > hi:
        raise ValueError(f"a range overruns the section end {hi:#x}")
    if cursor < hi:
        ranges.append((cursor, hi, "unassigned", "gap"))
    return ranges


def section_items(md, elf, sections_spec, records, refs=None):
    """{section: [(start, end, unit, evidence)]} -- every named symbol
    (sized where known) plus, in XREF_SECTIONS, the unnamed code-referenced
    addresses. A section whose named items interleave keeps named items only
    (it degrades anyway)."""
    if refs is None:
        refs = code_xref(md, elf, sections_spec)
    out = {}
    for name, lo, hi in data_sections(sections_spec):
        named = [(r["address"], r["address"] + r.get("size", 0), r["unit"],
                  "mdebug") for r in records if r["section"] == name]
        items = list(named)
        if name in XREF_SECTIONS and build_ranges(lo, hi, named) is not None:
            items += drop_conflicting_xref(
                named, xref_items(refs, named, lo, hi))
        out[name] = items
    return out


def attributed_items(md, elf, sections_spec, records, refs=None):
    """[(section, start, end, unit, evidence)] over all data sections."""
    return [(name, *item) for name, items
            in section_items(md, elf, sections_spec, records, refs).items()
            for item in items]


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
    records = collect(md, data, data_sections(sections_spec))
    if {(r["address"], r["unit"]) for r in records} != syms:
        raise ValueError("data symbol registry and .mdebug attribution "
                         "disagree")
    items = section_items(md, data, sections_spec, records)

    lines = [
        "# Generated by tools/extract_data_map.py from the .mdebug, .symtab",
        f"# and .text sections of {elf_rel}. Do not edit by hand:",
        "# regenerate with `python tools/extract_data_map.py`.",
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
        in_section = items[name]
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
            "xref_ranges": sum(1 for r in ranges if r[3] == "xref"),
            "symbols": sum(1 for it in in_section if it[3] == "mdebug"),
            "xref_items": sum(1 for it in in_section if it[3] == "xref"),
            "owned_bytes": owned,
            "total_bytes": hi - lo,
            "interleaved": degraded,
        }

    total = sum(s["total_bytes"] for s in stats["sections"].values())
    owned = sum(s["owned_bytes"] for s in stats["sections"].values())
    n_ranges = sum(s["ranges"] for s in stats["sections"].values()) + 1
    lines[3] = (f"# {n_ranges} ranges; {owned:#x} of {total:#x} data bytes "
                f"attributed by symbol and code-reference evidence.")
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
        print(f"  {name:<9} {s['symbols']:>5} syms  {s['xref_items']:>5} xref  "
              f"{s['owned_ranges']:>4} owned ranges ({s['xref_ranges']} xref)  "
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
