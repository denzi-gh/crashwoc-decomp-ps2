#!/usr/bin/env python3
"""Map every `D_<vram>` and data alias in the C to what the ELF knows about it.

The C references retail data through `extern ... D_<vram>` declarations, often
behind `#define <alias> D_<vram>` (or `D_<vram>[i]`, `(*(T *)D_<vram>)`).
The per-unit data workflow (docs/decomp_agent.md) replaces those with
definitions under the retail name; this tool prepares that work. It writes
build/<version>/data_renames.json (game-derived evidence, never committed) and
prints a summary. Nothing in the tree is changed.

One record per distinct `D_` symbol and one per alias. A `D_` record is
classified by its address against the data registry
(config/<version>/data_symbols.toml, regenerated in memory):

  named             a registry symbol starts here: its retail name (keys are
                    unit-qualified; duplicate names, static/global clashes and
                    function statics -- stem + the functions referencing it --
                    are flagged)
  member            inside a sized symbol: `name.field` (field known),
                    `name[i]` (element size known) or `name+0xN`
  literal           a .lit4 word: exact bits + the shortest round-trip float
                    text; it stays borrowed
  string            an unnamed .rodata/.sdata string one unit's code uses
  compiler_private  other unnamed bytes one unit's code uses (jump_table,
                    pool, aggregate)
  unknown           nothing attributes it (after an unsized static, COMMON
                    candidates, unreferenced bytes)
  vu                .vutext

Each record carries the evidence: the SYMR/EXTR table index, the .symtab
size, the stab, the referencing functions and a byte preview. Named records
also carry the type study: `type_candidates` by level -- 1 STABS (exact),
2 the extern types the C compiles against (flagged when a matching function
of that unit uses the symbol, and when an include/ struct's size comment
equals the .symtab size), 3 GC DWARF (`--gc-dwarf PATH`; rejected when its
size disagrees with .symtab), 4 byte heuristics (hints, never a type) -- and
`chosen_type`, the highest level that names one (or null).

Alias records say how the alias relates to the retail name: same, stem (the
alias is a function static's stem) or different (with its use-site count).

Usage:
  python tools/data_renames.py [--gc-dwarf PATH] [--out FILE]

Stdlib only; needs the target ELF.
"""
import argparse
import bisect
import json
import math
import re
import struct
import sys
import tomllib
from collections import Counter, defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from check_types import EXTERN_VAR, clean, normalize_type, split_top  # noqa: E402
from extract_data_map import code_xref, data_sections, section_items  # noqa: E402
from extract_data_symbols import collect, load_elf, unit_stabs  # noqa: E402
from extract_mdebug import MDebug, ST_GLOBAL, SC_DATA_CLASSES, find_section  # noqa: E402
from probe_layout import parse_header  # noqa: E402

D_RE = re.compile(r"\bD_([0-9A-Fa-f]{8})\b")
ALIAS_RE = re.compile(r"^[ \t]*#[ \t]*define[ \t]+(\w+)[ \t]+(.*?D_[0-9A-Fa-f]{8}.*?)\s*$",
                      re.M)
GC_VAR_RE = re.compile(
    r"^(?:static\s+)?(?P<type>.+?)\s+(?P<name>\w+)(?P<dims>(?:\[\w*\])*)"
    r"(?:\s*:\s*0x[0-9A-Fa-f]+)?;\s*// size: (?P<size>0x[0-9A-Fa-f]+)\s*$")
PREVIEW = 16


# --------------------------------------------------------------------------- #
# Inputs                                                                      #
# --------------------------------------------------------------------------- #

def source_texts(root):
    """{repo-relative path: text} of every C source and header."""
    paths = [*sorted((root / "src").rglob("*.c")),
             *sorted((root / "src").rglob("*.h")),
             *sorted((root / "include").rglob("*.h"))]
    return {p.relative_to(root).as_posix(): p.read_text(errors="replace")
            for p in paths}


def extern_decls(text):
    """Yield (name, C type, line) for each `extern` variable declaration."""
    for m in EXTERN_VAR.finditer(text):
        decl = m.group(1)
        base = re.match(r"\s*((?:(?:struct|union|enum)\s+\w+|\w+)(?:\s+\w+)*?)"
                        r"(?=\s*[*\w]+\s*(?:\[|,|$))", decl)
        if not base:
            continue
        line = text.count("\n", 0, m.start()) + 1
        for part in split_top(decl[base.end():]):
            name = re.search(r"([A-Za-z_]\w*)", part)
            if not name:
                continue
            dims = "".join(f"[{d.strip()}]" for d in re.findall(r"\[([^\]]*)\]", part))
            ctype = normalize_type(base.group(1) + " " + "*" * part.count("*")) + dims
            yield name.group(1), ctype, line


def scan_sources(texts):
    """(D_ uses {addr: Counter(file)}, aliases [..], externs {name: [..]},
    token counts Counter(name))."""
    uses = defaultdict(Counter)
    aliases, externs, tokens = [], defaultdict(list), Counter()
    for path, raw in texts.items():
        text = clean(raw)
        for m in D_RE.finditer(text):
            uses[int(m.group(1), 16)][path] += 1
        tokens.update(re.findall(r"\b[A-Za-z_]\w*\b", text))
        for m in ALIAS_RE.finditer(text):
            targets = D_RE.findall(m.group(2))
            if len(set(targets)) != 1:
                continue
            aliases.append({"alias": m.group(1), "expr": m.group(2).split("/*")[0].strip(),
                            "address": int(targets[0], 16), "file": path,
                            "line": text.count("\n", 0, m.start()) + 1})
        for name, ctype, line in extern_decls(text):
            externs[name].append({"file": path, "line": line, "type": ctype})
    return uses, aliases, externs, tokens


def load_manifests(root, version):
    """({function vram: (id, state)}, {source path: unit index})."""
    funcs, sources = {}, {}
    for path in sorted((root / "config" / version / "status").rglob("*.toml")):
        data = tomllib.loads(path.read_text())
        unit = int(data["unit"].rsplit("-", 1)[1])
        sources[data["source"]] = unit
        for entry in data.get("function", []):
            vram = int(entry["id"].split(":")[2], 16)
            funcs[vram] = (entry["id"], entry["state"])
    return funcs, sources


def header_layouts(texts):
    """{type name: {size, fields {offset: name}, file}} from include/ size
    and offset comments (`struct tag` or typedef name)."""
    out = {}
    for path, text in texts.items():
        if not path.startswith("include/"):
            continue
        for st in parse_header(text):
            if st["type"] in out:
                continue
            out[st["type"]] = {
                "size": st["size"][0] if st["size"] else None,
                "fields": {off: name for name, off, _l in st["fields"]},
                "file": path}
    return out


def load_gc_dwarf(path):
    """{name: [{cu, type, size}]} -- variable lines of the GC DWARF dump."""
    out, cu = defaultdict(list), None
    for line in Path(path).read_text(encoding="utf-8", errors="replace").splitlines():
        if line.startswith("// Compile unit:"):
            cu = line.split(":", 1)[1].strip()
            continue
        m = GC_VAR_RE.match(line)
        if m:
            ctype = " ".join(m.group("type").replace("const const", "const")
                             .replace("volatile volatile", "volatile").split())
            out[m.group("name")].append({"cu": cu, "type": ctype + m.group("dims"),
                                         "size": int(m.group("size"), 16),
                                         "defined": " : 0x" in line})
    return out


def table_indices(md):
    """{(unit, name, address): 'EXTR n' | 'SYMR n'} of the data symbols."""
    out = {}
    for i, (name, value, st, sc, ifd) in enumerate(md.externals()):
        if st == ST_GLOBAL and sc in SC_DATA_CLASSES and name:
            out.setdefault((ifd, name, value), f"EXTR {i}")
    for u in md.units():
        for i in range(u["sym_count"]):
            iss, value, _st, _sc, index = md.local_symbol(u["sym_first"] + i)
            if iss == -1 or md.is_stab(index):
                continue
            name = md.cstr(md.h["cbSsOffset"] + u["ss_base"] + iss)
            out.setdefault((u["index"], name, value & 0xFFFFFFFF),
                           f"SYMR {u['sym_first'] + i}")
    return out


# --------------------------------------------------------------------------- #
# Classification                                                              #
# --------------------------------------------------------------------------- #

def float_text(bits):
    """Shortest decimal that round-trips the float32 bit pattern."""
    value = struct.unpack("<f", struct.pack("<I", bits))[0]
    if math.isnan(value) or math.isinf(value):
        return repr(value)
    for digits in range(1, 10):
        text = f"{value:.{digits}g}"
        if struct.unpack("<I", struct.pack("<f", float(text)))[0] == bits:
            return text
    return repr(value)


def c_string(blob):
    """The NUL-terminated printable string at the start of blob, else None."""
    end = blob.find(b"\0")
    if end <= 0:
        return None
    text = blob[:end]
    if all(32 <= b < 127 or b in (9, 10, 13) for b in text):
        return text.decode("ascii")
    return None


def byte_hints(blob, text_lo, text_hi, data_lo, data_hi):
    """Level-4 hints (never a type) from the first bytes."""
    hints = []
    if not blob:
        return hints
    if not any(blob):
        hints.append("all_zero")
    if c_string(blob) is not None:
        hints.append("ascii_string")
    words = [struct.unpack_from("<I", blob, i)[0] for i in range(0, len(blob) - 3, 4)]
    if words and all(text_lo <= w < text_hi for w in words):
        hints.append("code_pointers")
    elif words and all(data_lo <= w < data_hi or w == 0 for w in words) and any(words):
        hints.append("data_pointers")
    floats = [struct.unpack("<f", struct.pack("<I", w))[0] for w in words]
    if words and all(w == 0 or 1e-6 < abs(f) < 1e7 for w, f in zip(words, floats)) \
            and any(words):
        hints.append("float32_words")
    return hints


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--version", default="pal103")
    parser.add_argument("--root", type=Path, default=ROOT)
    parser.add_argument("--gc-dwarf", type=Path,
                        help="GC DWARF dump (type level 3); skipped when absent")
    parser.add_argument("--out", type=Path,
                        help="output JSON (default build/<version>/data_renames.json)")
    args = parser.parse_args()
    root = args.root

    elf, sections_spec, _rel = load_elf(root, args.version)
    md = MDebug(elf, find_section(elf, ".mdebug")[0])
    dsecs = data_sections(sections_spec)
    records = collect(md, elf, dsecs)
    refs = code_xref(md, elf, sections_spec)
    items = section_items(md, elf, sections_spec, records, refs)
    indices = table_indices(md)
    stabs = {u["index"]: unit_stabs(md, u)[0] for u in md.units()
             if any(True for _ in md.stabs(u))}

    alloc = []
    for s in sections_spec["sections"]:
        flags, size = int(s["flags"], 0), int(s["size"], 0)
        if flags & 0x2 and size and int(s["addr"], 0):
            alloc.append((s["name"], int(s["addr"], 0), int(s["addr"], 0) + size,
                          int(s["type"], 0), int(s["offset"], 0)))

    def section_at(addr):
        for name, lo, hi, _t, _o in alloc:
            if lo <= addr < hi:
                return name
        return None

    def file_bytes(addr, size):
        for _name, lo, hi, stype, off in alloc:
            if lo <= addr < hi:
                if stype == 8:
                    return None
                return elf[off + addr - lo:off + min(addr + size, hi) - lo]
        return None

    text = next(a for a in alloc if a[0] == ".text")
    data_lo = min(lo for n, lo, _h, _t, _o in alloc if n not in (".text", ".vutext"))
    data_hi = max(hi for n, _l, hi, _t, _o in alloc if n not in (".text", ".vutext"))

    # Functions referencing each address, and the referencing units.
    procs = sorted((f["address"], f["name"], f["unit"]) for u in md.units()
                   for f in md.procedures(u))
    starts = [a for a, _n, _u in procs]
    ref_funcs, ref_units, ref_width = defaultdict(set), defaultdict(set), defaultdict(int)
    for addr, width, unit, ref in refs:
        i = bisect.bisect_right(starts, ref.vram) - 1
        ref_funcs[addr].add(procs[i][1] if i >= 0 else f"{ref.vram:#x}")
        ref_units[addr].add(unit)
        ref_width[addr] = max(ref_width[addr], width)
    func_at = {a: n for a, n, _u in procs}

    texts = source_texts(root)
    uses, aliases, externs, tokens = scan_sources(texts)
    manifest_funcs, source_units = load_manifests(root, args.version)
    layouts = header_layouts(texts)
    gc = load_gc_dwarf(args.gc_dwarf) if args.gc_dwarf and args.gc_dwarf.is_file() else None

    by_addr = defaultdict(list)
    for rec in records:
        by_addr[rec["address"]].append(rec)
    name_units = defaultdict(set)
    for rec in records:
        name_units[rec["name"]].add(rec["unit"])
    global_names = {r["name"] for r in records if r["scope"] == "global"}
    static_names = {r["name"] for r in records if r["scope"] != "global"}
    sized = sorted((r["address"], r["address"] + r["size"], r) for r in records
                   if r.get("size"))
    sized_starts = [s for s, _e, _r in sized]
    named_by_section = defaultdict(list)
    for rec in records:
        named_by_section[rec["section"]].append(rec)
    for recs in named_by_section.values():
        recs.sort(key=lambda r: r["address"])
    attributed = {sec: sorted(it[0] for it in its) for sec, its in items.items()}

    matched_name = {fid.rsplit(":", 1)[1] for fid, state in manifest_funcs.values()
                    if state == "matching"}

    def evidence(rec):
        ev = {"table": indices.get((rec["unit"], rec["name"], rec["address"])),
              "symtab_size": rec.get("size") if rec.get("size_evidence") == "symtab" else None,
              "stab": rec.get("stab"),
              "functions": sorted(ref_funcs.get(rec["address"], ()))}
        blob = file_bytes(rec["address"], min(rec.get("size") or PREVIEW, PREVIEW))
        ev["preview"] = blob.hex() if blob is not None else None
        return ev

    def type_study(rec):
        cands = []
        if rec.get("type"):
            cands.append({"level": 1, "source": "stabs", "type": rec["type"]})
        names = {rec["name"], f"D_{rec['address']:08X}", f"D_{rec['address']:08x}"}
        funcs = ref_funcs.get(rec["address"], set())
        for name in sorted(names):
            for decl in externs.get(name, []):
                unit = source_units.get(decl["file"])
                cand = {"level": 2, "source": "extern", "type": decl["type"],
                        "file": decl["file"], "line": decl["line"],
                        "matched_use": bool(unit is not None and unit == rec["unit"]
                                            and funcs & matched_name)}
                base = re.sub(r"\[[^\]]*\]", "", decl["type"]).strip()
                layout = layouts.get(base)
                if layout and layout["size"] and rec.get("size"):
                    count = 1
                    for dim in re.findall(r"\[(\d+)\]", decl["type"]):
                        count *= int(dim)
                    cand["header_size"] = layout["size"] * count == rec["size"]
                cands.append(cand)
        if gc is not None:
            for entry in gc.get(rec["name"], []):
                if not entry["size"]:
                    continue
                cand = {"level": 3, "source": "gc_dwarf", "type": entry["type"],
                        "cu": entry["cu"], "size": entry["size"]}
                if rec.get("size") and entry["size"] != rec["size"]:
                    cand["rejected"] = (f"GC size {entry['size']:#x} != .symtab "
                                        f"{rec['size']:#x}")
                cands.append(cand)
        blob = file_bytes(rec["address"], min(rec.get("size") or PREVIEW, 64))
        hints = byte_hints(blob, text[1], text[2], data_lo, data_hi) if blob else []
        if hints:
            cands.append({"level": 4, "source": "bytes", "hints": hints})
        chosen = None
        level1 = [c for c in cands if c["level"] == 1]
        level2 = [c for c in cands if c["level"] == 2]
        strong = {c["type"] for c in level2 if c["matched_use"] or c.get("header_size")}
        level3 = {c["type"] for c in cands if c["level"] == 3 and "rejected" not in c}
        if level1:
            chosen = {"level": 1, "type": level1[0]["type"]}
        elif len(strong) == 1:
            chosen = {"level": 2, "type": next(iter(strong))}
        elif len({c["type"] for c in level2}) == 1:
            chosen = {"level": 2, "type": level2[0]["type"]}
        elif len(level3) == 1:
            chosen = {"level": 3, "type": next(iter(level3))}
        return cands, chosen

    def element_size(rec, chosen):
        """Element size of an array symbol, when the type says it."""
        if rec["unit"] in stabs and rec.get("stab"):
            table = stabs[rec["unit"]]
            _n, _d, ref = table.add(rec["stab"])
            typ = table.resolve(ref)
            if getattr(typ, "kind", None) == "array":
                return table.size(typ.element)
        if chosen and "[" in chosen["type"]:
            base = chosen["type"].split("[")[0].strip()
            scalar = {"char": 1, "s8": 1, "u8": 1, "short": 2, "s16": 2, "u16": 2,
                      "int": 4, "s32": 4, "u32": 4, "float": 4, "f32": 4,
                      "double": 8, "f64": 8, "long": 8, "s64": 8, "u64": 8}
            if base.endswith("*"):
                return 4
            if base in scalar:
                return scalar[base]
            layout = layouts.get(base)
            if layout and layout["size"]:
                return layout["size"]
        return None

    def field_name(rec, chosen, offset):
        if chosen:
            layout = layouts.get(chosen["type"].strip())
            if layout and offset in layout["fields"]:
                return layout["fields"][offset]
        return None

    study_cache = {}

    def study(rec):
        if rec["key"] not in study_cache:
            study_cache[rec["key"]] = type_study(rec)
        return study_cache[rec["key"]]

    out_syms = []
    for addr in sorted(uses):
        sec = section_at(addr)
        rec_out = {"symbol": f"D_{addr:08X}", "address": f"{addr:#010x}",
                   "section": sec, "files": dict(sorted(uses[addr].items())),
                   "extern_types": sorted({d["type"] for n in (f"D_{addr:08X}", f"D_{addr:08x}")
                                           for d in externs.get(n, [])}),
                   "functions": sorted(ref_funcs.get(addr, ())),
                   "units": sorted(ref_units.get(addr, ()))}
        named = by_addr.get(addr, [])
        if named:
            rec = named[0]
            flags = []
            if len(name_units[rec["name"]]) > 1:
                flags.append("duplicate_name")
            if rec["name"] in global_names and rec["name"] in static_names:
                flags.append("static_global_clash")
            cands, chosen = study(rec)
            rec_out.update({"kind": "named", "key": rec["key"], "name": rec["name"],
                            "scope": rec["scope"], "flags": flags,
                            "evidence": evidence(rec), "type_candidates": cands,
                            "chosen_type": chosen})
            if rec["scope"] == "function_static":
                rec_out["stem"] = rec["stem"]
            if len(named) > 1:
                rec_out["also_named"] = [r["key"] for r in named[1:]]
        else:
            i = bisect.bisect_right(sized_starts, addr) - 1
            inside = sized[i][2] if i >= 0 and sized[i][0] < addr < sized[i][1] else None
            if inside is not None:
                offset = addr - inside["address"]
                _c, chosen = study(inside)
                field = field_name(inside, chosen, offset)
                elem = element_size(inside, chosen)
                if field:
                    rename = f"{inside['name']}.{field}"
                elif elem and offset % elem == 0:
                    rename = f"{inside['name']}[{offset // elem}]"
                else:
                    rename = f"{inside['name']}+{offset:#x}"
                rec_out.update({"kind": "member", "key": inside["key"],
                                "offset": offset, "rename": rename,
                                "chosen_type": chosen})
            elif sec == ".lit4":
                bits = struct.unpack("<I", file_bytes(addr, 4))[0]
                rec_out.update({"kind": "literal", "bits": f"{bits:#010x}",
                                "text": float_text(bits)})
            elif sec == ".vutext":
                rec_out["kind"] = "vu"
            else:
                blob = file_bytes(addr, 256)
                string = c_string(blob) if blob else None
                prev = [r for r in named_by_section.get(sec, []) if r["address"] < addr]
                prev = prev[-1] if prev else None
                starts_sec = attributed.get(sec, [])
                j = bisect.bisect_left(starts_sec, addr) - 1
                prev_item = starts_sec[j] if j >= 0 else None
                single = len(ref_units.get(addr, ())) == 1
                if sec in (".rodata", ".sdata") and string is not None and single:
                    rec_out.update({"kind": "string", "text": string})
                elif prev is not None and "size" not in prev \
                        and prev_item == prev["address"]:
                    rec_out.update({"kind": "unknown",
                                    "note": f"after unsized {prev['key']}"})
                elif single:
                    word = struct.unpack_from("<I", blob)[0] if blob and len(blob) >= 4 else None
                    if word is not None and text[1] <= word < text[2]:
                        sub = "jump_table"
                    elif ref_width.get(addr) in (8, 16):
                        sub = "pool"
                    else:
                        sub = "aggregate"
                    rec_out.update({"kind": "compiler_private", "subkind": sub})
                else:
                    note = ("common candidate" if sec in (".bss", ".sbss")
                            else "referenced by several units" if ref_units.get(addr)
                            else "no code reference")
                    rec_out.update({"kind": "unknown", "note": note})
            if rec_out["kind"] in ("string", "compiler_private", "literal", "unknown", "vu"):
                blob = file_bytes(addr, PREVIEW)
                rec_out["preview"] = blob.hex() if blob is not None else None
        out_syms.append(rec_out)

    sym_by_addr = {int(r["address"], 16): r for r in out_syms}
    out_aliases = []
    for alias in aliases:
        target = sym_by_addr.get(alias["address"])
        rec = {"alias": alias["alias"], "expr": alias["expr"],
               "symbol": f"D_{alias['address']:08X}", "file": alias["file"],
               "line": alias["line"], "kind": target["kind"] if target else None,
               "uses": max(tokens[alias["alias"]] - 1, 0)}
        if target and target["kind"] == "named":
            rec["retail_name"] = target["name"]
            if alias["alias"] == target["name"]:
                rec["relation"] = "same"
            elif alias["alias"] == target.get("stem"):
                rec["relation"] = "stem"
            else:
                rec["relation"] = "different"
        elif target and target["kind"] == "member":
            rec["retail_name"] = target["rename"]
        out_aliases.append(rec)

    summary = {
        "d_symbols": len(out_syms),
        "kinds": dict(sorted(Counter(r["kind"] for r in out_syms).items())),
        "compiler_private": dict(sorted(Counter(r.get("subkind") for r in out_syms
                                                if r["kind"] == "compiler_private").items())),
        "aliases": len(out_aliases),
        "alias_relations": dict(sorted(Counter(r.get("relation") for r in out_aliases
                                               if r.get("relation")).items())),
        "chosen_type_levels": dict(sorted(Counter(
            str((r.get("chosen_type") or {}).get("level")) for r in out_syms
            if r["kind"] == "named").items())),
        "gc_dwarf": str(args.gc_dwarf) if gc is not None else None,
    }
    out = args.out or root / "build" / args.version / "data_renames.json"
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps({"schema": 1, "summary": summary, "symbols": out_syms,
                               "aliases": out_aliases}, indent=1) + "\n")
    print(json.dumps(summary, indent=1))
    try:
        shown = out.relative_to(root)
    except ValueError:
        shown = out
    print(f"wrote {shown}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
