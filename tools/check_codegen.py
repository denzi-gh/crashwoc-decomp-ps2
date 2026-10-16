#!/usr/bin/env python3
"""Snapshot the compiled objects and prove a refactor left their code unchanged.

`ninja check` only proves the functions marked `matching`. A header or type
refactor also recompiles every near-match and every `asm`-state C body that
objdiff scores, and a regression there fails nothing. This tool closes that
gap: snapshot the objects before the change, rebuild, compare.

Per object it records what can change the code, independent of line numbers
and file names:

  * every allocated section (SHF_ALLOC): name, type, size and, unless NOBITS,
    a hash of its bytes;
  * every relocation against an allocated section: offset, type and target
    symbol name (section symbols by section name);
  * the defined and undefined symbols (name, section, value, size, type,
    binding), order-independent;
  * per function symbol, a hash of its bytes plus the relocations inside it,
    so a comparison names the functions that moved.

Non-allocated sections (.mdebug, .comment, .pdr, ...) are ignored: they carry
line and file information that a header edit shifts without touching code.

The object list is read from the phony targets in build.ninja (`current`,
`matching`, `equivalent`, `report-current`), so stale files in build/ never
count. Stdlib only.

Usage:
  python tools/check_codegen.py --build --snapshot      # on the base commit
  python tools/check_codegen.py --build --compare       # after the change
"""
import argparse
import hashlib
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from declib.elf import SHF_ALLOC, SHT_NOBITS, read_elf  # noqa: E402

SETS = ("current", "matching", "equivalent", "report-current")

STT_SECTION = 3
STT_FILE = 4
STT_FUNC = 2
SHN_UNDEF = 0
SHN_ABS = 0xFFF1
SHN_COMMON = 0xFFF2


def _sha(*parts):
    h = hashlib.sha256()
    for part in parts:
        h.update(part if isinstance(part, bytes) else str(part).encode())
        h.update(b"\0")
    return h.hexdigest()[:16]


def fingerprint(data):
    """Code fingerprint of one object (see module docstring)."""
    sections, symbols, relocations = read_elf(data)
    alloc = {s["index"]: s for s in sections if s["flags"] & SHF_ALLOC}

    def shname(shndx):
        if shndx == SHN_UNDEF:
            return "UND"
        if shndx == SHN_ABS:
            return "ABS"
        if shndx == SHN_COMMON:
            return "COM"
        return sections[shndx]["name"] if shndx < len(sections) else f"#{shndx}"

    def target(sym_index):
        sym = symbols[sym_index]
        if sym["type"] == STT_SECTION:
            return "section:" + shname(sym["shndx"])
        return sym["name"]

    relocs = {idx: sorted((off, rtype, target(si), add)
                          for off, rtype, si, add in entries)
              for idx, entries in relocations.items() if idx in alloc}

    out_sections = {}
    for idx, sec in sorted(alloc.items()):
        out_sections[sec["name"]] = {
            "type": sec["type"], "size": sec["size"],
            "hash": None if sec["type"] == SHT_NOBITS else _sha(sec["data"]),
            "relocs": _sha(json.dumps(relocs.get(idx, []))),
        }

    out_symbols = sorted(
        f"{s['name']}|{shname(s['shndx'])}|{s['value']:#x}|{s['size']:#x}"
        f"|{s['type']}|{s['bind']}"
        for s in symbols
        if s["name"] and s["type"] not in (STT_SECTION, STT_FILE))

    functions = {}
    for s in symbols:
        if s["type"] != STT_FUNC or s["shndx"] not in alloc or not s["name"]:
            continue
        sec = alloc[s["shndx"]]
        lo, hi = s["value"], s["value"] + s["size"]
        inside = [(off - lo, rtype, tgt, add)
                  for off, rtype, tgt, add in relocs.get(s["shndx"], [])
                  if lo <= off < hi]
        functions[s["name"]] = _sha(sec["data"][lo:hi], json.dumps(inside))

    return {"sections": out_sections, "symbols": out_symbols,
            "functions": functions}


def ninja_objects(ninja_path, sets=SETS):
    """{set name: [object paths]} from the phony lines of build.ninja."""
    found = {}
    for line in Path(ninja_path).read_text().splitlines():
        if not line.startswith("build "):
            continue
        head, _, rest = line[len("build "):].partition(":")
        parts = rest.split()
        if head.strip() in sets and parts and parts[0] == "phony":
            found[head.strip()] = parts[1:]
    return found


def snapshot(objects):
    """{object path: fingerprint or {"missing": true}}."""
    out = {}
    for paths in objects.values():
        for rel in paths:
            path = ROOT / rel
            out[rel] = (fingerprint(path.read_bytes()) if path.is_file()
                        else {"missing": True})
    return out


def compare(before, after):
    """Human-readable difference lines; empty when nothing changed."""
    lines = []
    for rel in sorted(set(before) | set(after)):
        a, b = before.get(rel), after.get(rel)
        if a == b:
            continue
        if a is None or b is None:
            lines.append(f"{rel}: {'added' if a is None else 'removed'}")
            continue
        if a.get("missing") or b.get("missing"):
            lines.append(f"{rel}: {'now built' if a.get('missing') else 'missing'}")
            continue
        details = []
        for name in sorted(set(a["sections"]) | set(b["sections"])):
            sa, sb = a["sections"].get(name), b["sections"].get(name)
            if sa == sb:
                continue
            if sa is None or sb is None:
                details.append(f"section {name} {'added' if sa is None else 'removed'}")
            elif sa["size"] != sb["size"]:
                details.append(f"section {name} size {sa['size']:#x} -> {sb['size']:#x}")
            else:
                what = [k for k in ("hash", "relocs", "type") if sa[k] != sb[k]]
                details.append(f"section {name} {'/'.join(what)} differ")
        fa, fb = a["functions"], b["functions"]
        changed = sorted(n for n in set(fa) & set(fb) if fa[n] != fb[n])
        if changed:
            details.append("functions changed: " + ", ".join(changed))
        gone, new = sorted(set(fa) - set(fb)), sorted(set(fb) - set(fa))
        if gone:
            details.append("functions removed: " + ", ".join(gone))
        if new:
            details.append("functions added: " + ", ".join(new))
        sym_a, sym_b = set(a["symbols"]), set(b["symbols"])
        if sym_a != sym_b:
            details.append(f"symbols differ (-{len(sym_a - sym_b)} +{len(sym_b - sym_a)})")
        lines.append(f"{rel}: " + ("; ".join(details) or "changed"))
    return lines


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--version", default="pal103")
    mode = parser.add_mutually_exclusive_group(required=True)
    mode.add_argument("--snapshot", action="store_true",
                      help="write the fingerprint of every object")
    mode.add_argument("--compare", action="store_true",
                      help="compare the objects against the snapshot")
    parser.add_argument("--file", help="snapshot path (default: "
                        "build/<version>/codegen_snapshot.json)")
    parser.add_argument("--build", action="store_true",
                        help="build the object sets first (via tools/dispatch.py)")
    args = parser.parse_args()

    snap_path = Path(args.file) if args.file else (
        ROOT / "build" / args.version / "codegen_snapshot.json")
    if args.build:
        subprocess.run([sys.executable, str(ROOT / "tools" / "dispatch.py"),
                        "ninja", *SETS], cwd=ROOT, check=True)
    objects = ninja_objects(ROOT / "build.ninja")
    missing_sets = [s for s in SETS if s not in objects]
    if missing_sets:
        print(f"build.ninja has no phony target(s): {', '.join(missing_sets)}; "
              "run python tools/gen_ninja.py", file=sys.stderr)
        return 2
    current = snapshot(objects)

    if args.snapshot:
        snap_path.parent.mkdir(parents=True, exist_ok=True)
        snap_path.write_text(json.dumps(current, indent=1, sort_keys=True) + "\n")
        print(f"snapshot: {len(current)} objects -> {snap_path}")
        return 0

    if not snap_path.is_file():
        print(f"no snapshot at {snap_path}; run --snapshot on the base first",
              file=sys.stderr)
        return 2
    lines = compare(json.loads(snap_path.read_text()), current)
    for line in lines:
        print(line)
    print(f"{len(lines)} of {len(current)} objects differ from the snapshot")
    return 1 if lines else 0


if __name__ == "__main__":
    sys.exit(main())
