#!/usr/bin/env python3
"""Extract the per-symbol data registry from the retail ELF.

Produces config/<version>/data_symbols.toml: one `[[symbol]]` per named data
symbol `.mdebug` attributes to a unit -- every data global the external table
(EXTR) defines and every file-local static of a unit's SYMR run, i.e. exactly
the set tools/extract_data_map.py attributes. Linker symbols (`_gp`, `_fbss`)
and COMMON globals (named nowhere in the ELF) are not in it.

Per symbol, every field is read from the ELF, nothing is guessed:

  key, name, unit   `unit-NNNN:<name>` (unique), the retail name (function
                    statics keep their `.N`), the owning unit (EXTR ifd /
                    SYMR file descriptor)
  scope, stem       global | static | function_static (`<stem>.<N>`)
  address, section  the section comes from the address (sections.json), never
                    from the storage class (`errno` is scBss in .sbss)
  storage           the ECOFF storage class name
  size              `.symtab` st_size when > 0 (size_evidence = 'symtab'),
                    else the size of a complete STABS type ('stabs')
  extent            bytes to the next attributed item (any unit, named or
                    code-referenced -- see extract_data_map.py) or the
                    section end: an upper bound that includes padding
  pad_zero          the bytes [address + size, address + extent) are all zero
  type, stab        the C type rebuilt from the unit's STABS and the raw stab
                    string (only the `sce` units carry STABS)

TOML has no null: an absent key means unknown. The registry is a pure
function of the ELF (`--check` regenerates it and compares). `--stats` prints
the census the data plan quotes.

Usage:
  python tools/extract_data_symbols.py            # regenerate the registry
  python tools/extract_data_symbols.py --check    # verify committed == fresh
  python tools/extract_data_symbols.py --stats    # census only

Stdlib only; needs the target ELF (user-supplied, never committed).
"""
import argparse
import json
import re
import sys
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from declib.elf import read_elf
from declib.stabs import StabsError, TypeTable
from extract_mdebug import (MDebug, N_GSYM, N_LCSYM, N_STSYM, SC_DATA_CLASSES,
                            SC_NAMES, ST_GLOBAL, find_section, toml_str)

SCHEMA = 1
FUNCTION_STATIC_RE = re.compile(r"^(.+)\.(\d+)$")
STB_LOCAL, STB_GLOBAL = 0, 1
# The stab descriptor each scope must carry where a stab exists.
STAB_DESCRIPTOR = {"global": "G", "static": "S", "function_static": "V"}


def section_of(address, sections):
    """Name of the allocated section holding `address`, else None."""
    for name, lo, hi in sections:
        if lo <= address < hi:
            return name
    return None


def load_elf(root, version):
    """(elf bytes, sections_spec, elf_rel) for the version."""
    config_dir = root / "config" / version
    version_meta = json.loads((config_dir / "version.json").read_text())
    sections_spec = json.loads((config_dir / "sections.json").read_text())
    elf_rel = version_meta["files"]["elf"]["path"]
    elf_path = root / elf_rel
    if not elf_path.is_file():
        raise FileNotFoundError(f"target ELF not found at {elf_rel}")
    return elf_path.read_bytes(), sections_spec, elf_rel


def unit_stabs(md, unit):
    """(TypeTable, {name: (desc, ref, string)} globals,
    {address: [(name, desc, ref, string)]} statics) of one unit."""
    table = TypeTable()
    globals_, statics = {}, {}
    for stab_type, string, value, _sc in md.stabs(unit):
        name, desc, ref = table.add(string)
        if stab_type == N_GSYM and ref is not None:
            globals_.setdefault(name, (desc, ref, string))
        elif stab_type in (N_STSYM, N_LCSYM) and ref is not None:
            statics.setdefault(value, []).append((name, desc, ref, string))
    return table, globals_, statics


def collect(md, elf, sections):
    """[record] for every named data symbol, sorted by (address, unit, name).

    `sections` is extract_data_map.data_sections(); records carry no extent
    yet (it needs the code cross-references, see with_extents()).
    """
    _secs, elf_symbols, _rel = read_elf(elf)
    symtab = {}
    for sym in elf_symbols:
        if sym["name"]:
            symtab.setdefault((sym["name"], sym["value"]), []).append(sym)

    ifd_max = md.h["ifdMax"]
    raw = []                       # (name, address, unit, scope, sc)
    for name, value, st, sc, ifd in md.externals():
        if st == ST_GLOBAL and sc in SC_DATA_CLASSES and name \
                and 0 <= ifd < ifd_max:
            raw.append((name, value, ifd, "global", sc))
    units = list(md.units())
    for u in units:
        for name, addr, sc in md.local_data_symbols(u):
            scope = "function_static" if FUNCTION_STATIC_RE.match(name) \
                else "static"
            raw.append((name, addr, u["index"], scope, sc))

    stabs = {u["index"]: unit_stabs(md, u) for u in units
             if any(True for _ in md.stabs(u))}

    records, keys = [], set()
    for name, address, unit, scope, sc in raw:
        key = f"unit-{unit:04d}:{name}"
        if key in keys:
            raise ValueError(f"duplicate registry key {key}")
        keys.add(key)
        section = section_of(address, sections)
        if section is None:
            raise ValueError(f"{key} @ {address:#010x} is in no data section")
        matches = symtab.get((name, address), [])
        if len(matches) != 1:
            raise ValueError(f"{key}: {len(matches)} .symtab entries at "
                             f"{address:#010x} (expected exactly 1)")
        elf_sym = matches[0]
        want_bind = STB_GLOBAL if scope == "global" else STB_LOCAL
        if elf_sym["bind"] != want_bind:
            raise ValueError(f"{key}: .symtab binding {elf_sym['bind']} "
                             f"disagrees with scope {scope}")
        rec = {"key": key, "name": name, "unit": unit, "scope": scope,
               "address": address, "section": section,
               "storage": SC_NAMES.get(sc, f"sc{sc}")}
        m = FUNCTION_STATIC_RE.match(name)
        if scope == "function_static":
            rec["stem"] = m.group(1)

        stab = None
        if unit in stabs:
            table, gstabs, sstabs = stabs[unit]
            if scope == "global":
                stab = gstabs.get(name)
                stab = stab and (stab[0], stab[1], stab[2])
            else:
                want = rec.get("stem", name)
                hits = [(d, r, s) for n, d, r, s in sstabs.get(address, [])
                        if n == want]
                stab = hits[0] if hits else None
        stab_size = None
        if stab is not None:
            desc, ref, string = stab
            if desc != STAB_DESCRIPTOR[scope]:
                raise ValueError(f"{key}: stab {string!r} has descriptor "
                                 f"{desc!r}, scope {scope} expects "
                                 f"{STAB_DESCRIPTOR[scope]!r}")
            try:
                rec["type"] = table.c_type(ref)
                stab_size = table.size(ref)
            except StabsError as exc:
                raise ValueError(f"{key}: {exc}") from exc
            rec["stab"] = string

        if elf_sym["size"] > 0:
            rec["size"], rec["size_evidence"] = elf_sym["size"], "symtab"
            if stab_size is not None and stab_size != elf_sym["size"]:
                raise ValueError(f"{key}: STABS size {stab_size} != .symtab "
                                 f"size {elf_sym['size']}")
        elif stab_size:
            rec["size"], rec["size_evidence"] = stab_size, "stabs"
        records.append(rec)
    records.sort(key=lambda r: (r["address"], r["unit"], r["name"]))
    return records


def with_extents(records, boundaries, sections, file_bytes):
    """Add `extent` and `pad_zero` in place.

    `boundaries` is the sorted list of every attributed item start (named
    symbols and code-referenced addresses, all units); `file_bytes(section,
    lo, hi)` returns the retail bytes, or None for a NOBITS section.
    """
    import bisect
    ends = {name: hi for name, _lo, hi in sections}
    for rec in records:
        addr = rec["address"]
        i = bisect.bisect_right(boundaries, addr)
        nxt = boundaries[i] if i < len(boundaries) else None
        end = ends[rec["section"]]
        if nxt is not None and nxt < end:
            end = nxt
        rec["extent"] = end - addr
        size = rec.get("size")
        if size is None:
            continue
        if size > rec["extent"]:
            raise ValueError(f"{rec['key']}: size {size:#x} overruns the next "
                             f"item at {addr + rec['extent']:#010x}")
        pad = file_bytes(rec["section"], addr + size, end)
        rec["pad_zero"] = True if pad is None else not any(pad)


def render(records, elf_rel):
    n_sized = sum(1 for r in records if "size" in r)
    n_typed = sum(1 for r in records if "type" in r)
    lines = [
        "# Generated by tools/extract_data_symbols.py from the .mdebug, .symtab",
        f"# and .text sections of {elf_rel}. Do not edit by hand:",
        "# regenerate with `python tools/extract_data_symbols.py`.",
        f"# {len(records)} data symbols; {n_sized} sized, {n_typed} typed "
        "(STABS). An absent key is unknown.",
        "",
        f"schema = {SCHEMA}",
        "",
    ]
    order = ("key", "name", "unit", "scope", "stem", "address", "section",
             "storage", "size", "size_evidence", "extent", "pad_zero", "type",
             "stab")
    for rec in records:
        lines.append("[[symbol]]")
        for field in order:
            if field not in rec:
                continue
            value = rec[field]
            if field == "address":
                text = f"{value:#010x}"
            elif isinstance(value, bool):
                text = "true" if value else "false"
            elif isinstance(value, int):
                text = str(value)
            else:
                text = toml_str(value)
            lines.append(f"{field} = {text}")
        lines.append("")
    return "\n".join(lines).rstrip() + "\n"


def stats(records):
    """The census lines of the data plan's facts table."""
    by_scope = Counter(r["scope"] for r in records)
    globals_ = [r for r in records if r["scope"] == "global"]
    statics = [r for r in records if r["scope"] != "global"]
    names = Counter(r["name"] for r in statics)
    multi = {n for n, c in names.items() if c > 1}
    clashes = {r["name"] for r in globals_} & set(names)
    unsized = Counter(r["section"] for r in statics if "size" not in r)
    lines = [
        f"symbols: {len(records)} ({by_scope['global']} global, "
        f"{by_scope['static']} static, {by_scope['function_static']} "
        "function_static)",
        f"sized: {sum('size' in r for r in globals_)}/{len(globals_)} globals, "
        f"{sum('size' in r for r in statics)}/{len(statics)} statics "
        f"(evidence: {dict(Counter(r.get('size_evidence') for r in records if 'size' in r))})",
        f"unsized statics by section: {dict(sorted(unsized.items()))}",
        f"STABS-typed: {sum('type' in r for r in records)}",
        f"static names: {len(names)} distinct, {len(multi)} in more than one "
        f"unit ({sum(names[n] for n in multi)} entries); static/global "
        f"clashes: {sorted(clashes)}",
        f"pad_zero false: {sum(1 for r in records if r.get('pad_zero') is False)}",
    ]
    return lines


def build(root, version):
    """(registry text, records)."""
    from extract_data_map import attributed_items, data_sections
    elf, sections_spec, elf_rel = load_elf(root, version)
    md_off, _ = find_section(elf, ".mdebug")
    md = MDebug(elf, md_off)
    sections = data_sections(sections_spec)
    records = collect(md, elf, sections)
    items = attributed_items(md, elf, sections_spec, records)
    boundaries = sorted({start for _sec, start, _end, _unit, _ev in items})
    by_name = {s["name"]: s for s in sections_spec["sections"]}

    def file_bytes(section, lo, hi):
        sec = by_name[section]
        if int(sec["type"], 0) == 8:          # NOBITS: no file bytes
            return None
        off = int(sec["offset"], 0) + lo - int(sec["addr"], 0)
        return elf[off:off + hi - lo]

    with_extents(records, boundaries, sections, file_bytes)
    return render(records, elf_rel), records


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--version", default="pal103")
    parser.add_argument("--root", type=Path, default=ROOT)
    mode = parser.add_mutually_exclusive_group()
    mode.add_argument("--check", action="store_true",
                      help="verify the committed registry matches a fresh "
                           "extraction instead of writing it")
    mode.add_argument("--stats", action="store_true",
                      help="print the census and write nothing")
    args = parser.parse_args()

    try:
        text, records = build(args.root, args.version)
    except (OSError, ValueError, KeyError, json.JSONDecodeError) as exc:
        print(f"data symbol extraction failed: {exc}", file=sys.stderr)
        return 1

    if args.stats:
        print("\n".join(stats(records)))
        return 0
    out = args.root / "config" / args.version / "data_symbols.toml"
    rel = out.relative_to(args.root)
    if args.check:
        current = out.read_text() if out.is_file() else None
        if current == text:
            print(f"{rel}: PASS  ({len(records)} symbols)")
            return 0
        print(f"{rel}: FAIL -- differs from a fresh extraction "
              f"(or does not exist); regenerate it.")
        return 1
    out.write_text(text)
    print(f"wrote {rel}  ({len(records)} symbols)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
