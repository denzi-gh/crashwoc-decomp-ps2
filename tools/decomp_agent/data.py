"""Data-from-C for the agent layer: data ids, candidates, context, byte diff.

A data id has the function id's shape (``pal103:unit-0014:00613fb0:nuvec4_zero``)
and names a symbol of the data registry (``config/<v>/data_symbols.toml``)
that a unit's status manifest lists under ``[[data]]``. The operations mirror
the function ones and reuse the same gates:

  * ``resolve_data`` -- id / registry key / name / address -> records;
  * ``list_data_candidates`` -- data entries of fully matching units, ranked
    (.rodata before .data, STABS-typed, pointer-free, small first), plus the
    units that only need ``--init-data`` or ``--complete``;
  * ``data_context`` -- bytes, size/extent/section/storage/scope, type
    evidence, referencing functions, the dump command, the current C;
  * ``data_diff`` -- builds the unit's hybrid with this entry `matching` (a
    temporary manifest), links its pieces at their addresses and compares
    [addr, addr+size) with retail (toolchain-bound);
  * promotion goes through ``tools/promote.py`` like functions.
"""
from __future__ import annotations

import json
import re
import struct
import tempfile
import tomllib
from functools import lru_cache
from pathlib import Path

from . import _bridge, schemas
from .identifiers import is_function_id, parse_function_id

PROGBITS = (".data", ".rodata", ".sdata")


@lru_cache(maxsize=4)
def _symbols(path: str, _mtime: int) -> tuple:
    return tuple(tomllib.loads(Path(path).read_text()).get("symbol", []))


def data_symbols(project) -> tuple:
    path = project.path("config", project.version, "data_symbols.toml")
    if not path.is_file():
        return ()
    return _symbols(str(path), path.stat().st_mtime_ns)


def data_id(project, sym) -> str:
    return f"{project.version}:unit-{sym['unit']:04d}:{sym['address']:08x}:{sym['name']}"


def resolve_data(project, target) -> list:
    """Registry records a target names: a data id, key, name or address."""
    query = str(target).strip()
    syms = data_symbols(project)
    if is_function_id(query):
        fid = parse_function_id(query)
        return [s for s in syms if s["unit"] == fid.unit_index
                and s["address"] == fid.address and s["name"] == fid.name]
    m = re.fullmatch(r"(?:0x)?([0-9A-Fa-f]{6,8})", query)
    if m and not re.fullmatch(r"[A-Za-z_]\w*", query):
        addr = int(m.group(1), 16)
        return [s for s in syms if s["address"] == addr]
    return [s for s in syms if query in (s["key"], s["name"])]


def is_data_target(project, target, function_resolution=None) -> bool:
    """True when a target names data and no function: a data id always; a
    bare name or address only if the function registry does not know it."""
    hits = resolve_data(project, target)
    if not hits:
        return False
    if is_function_id(str(target).strip()):
        return True
    return function_resolution is None or function_resolution.status == "not_found"


def resolve_target(project, target, function_resolution) -> dict:
    """resolve_target's answer: the function resolution, or the data symbol
    when the target names data (a name both a function and a datum carry is
    ambiguous: every candidate id is returned, nothing is chosen)."""
    hits = resolve_data(project, target)
    out = function_resolution.as_dict()
    if not hits:
        return out
    ids = [data_id(project, s) for s in hits]
    if function_resolution.status != "not_found" and \
            not is_function_id(str(target).strip()):
        out["status"] = "ambiguous"
        out["candidates"] = list(out.get("candidates") or
                                 ([function_resolution.record.id_str()]
                                  if function_resolution.record else [])) + ids
        out["detail"] = f"{target!r} names a function and data"
        return out
    if len(hits) > 1:
        return {"status": "ambiguous", "query": str(target), "candidates": ids,
                "detail": f"{target!r} names several data symbols"}
    sym = dict(hits[0])
    sym["id"] = ids[0]
    return {"status": "resolved", "query": str(target), "kind": "data", "data": sym}


def _manifest(project, unit_index):
    status = project.path("config", project.version, "status")
    for path in sorted(status.rglob("*.toml")):
        data = tomllib.loads(path.read_text())
        if data.get("unit") == f"{project.version}:unit-{unit_index:04d}":
            return path, data
    return None, None


def _entry_state(data, did):
    for entry in (data or {}).get("data", []):
        if entry["id"] == did:
            return entry["state"]
    return None


# --- candidates ---------------------------------------------------------------

def list_data_candidates(project, *, limit=20) -> dict:
    """Data work worth doing next, deterministically ordered, with reasons."""
    syms = data_symbols(project)
    by_unit = {}
    for s in syms:
        by_unit.setdefault(s["unit"], []).append(s)
    dm = tomllib.loads(project.path("config", project.version,
                                    "data_map.toml").read_text())["range"]
    owned = {}
    for r in dm:
        owned.setdefault(r["owner"], []).append(r)
    rows, complete_only, init_needed = [], [], []
    status = project.path("config", project.version, "status")
    for path in sorted(status.rglob("*.toml")):
        data = tomllib.loads(path.read_text())
        fns = data.get("function", [])
        if not fns or any(f["state"] != "matching" for f in fns):
            continue
        unit = int(data["unit"].split("unit-")[1])
        unit_syms = [s for s in by_unit.get(unit, []) if s["section"] in PROGBITS]
        if not unit_syms:
            if not data.get("complete") and not owned.get(f"unit-{unit:04d}"):
                complete_only.append(data["unit"])
            continue
        if not data.get("data"):
            init_needed.append({"unit": data["unit"], "source": data["source"],
                                "symbols": len(unit_syms),
                                "command": f"python tools/promote.py --init-data "
                                           f"{data['source']}"})
            continue
        for s in unit_syms:
            did = data_id(project, s)
            if _entry_state(data, did) != "asm":
                continue
            reasons, excluded = [], None
            if s["section"] == ".sdata":
                excluded = ".sdata data from C needs data/sdata"
            typ = s.get("type") or ""
            if typ:
                reasons.append(f"STABS type {typ}")
            if "*" in typ:
                reasons.append("holds pointers")
            rows.append({
                "id": did, "unit": data["unit"], "source": data["source"],
                "section": s["section"], "size": s.get("size"), "type": typ or None,
                "reasons": reasons, "excluded": excluded,
                "_key": (excluded is not None, s["section"] != ".rodata",
                         not typ, "*" in typ, s.get("size") or 1 << 30,
                         s["address"])})
    rows.sort(key=lambda r: r["_key"])
    for r in rows:
        del r["_key"]
    return schemas.ok("list_data_candidates", count=len(rows),
                      candidates=rows[:limit], needs_init_data=init_needed[:limit],
                      complete_only=complete_only[:limit])


# --- context --------------------------------------------------------------------

def _retail_bytes(project, sym, size):
    spec = json.loads(project.path("config", project.version,
                                   "sections.json").read_text())
    meta = json.loads(project.path("config", project.version,
                                   "version.json").read_text())
    elf_path = project.root / meta["files"]["elf"]["path"]
    if not elf_path.is_file():
        return None
    for s in spec["sections"]:
        lo, sz = int(s["addr"], 0), int(s["size"], 0)
        if lo and lo <= sym["address"] < lo + sz:
            if int(s["type"], 0) == 8:
                return bytes(size)
            with open(elf_path, "rb") as f:
                f.seek(int(s["offset"], 0) + sym["address"] - lo)
                return f.read(size)
    return None


def _current_c(project, source, sym):
    path = project.root / source
    if not path.is_file():
        return None
    text = path.read_text(errors="replace")
    name = sym.get("stem") or sym["name"]
    pat = re.compile(r"^[^\n;{}]*\b" + re.escape(name) + r"\b\s*(?:\[[^\]]*\]\s*)*=[^;]*;",
                     re.M | re.S)
    m = pat.search(text)
    return m.group(0).strip()[:600] if m else None


def _externs(project, source, sym):
    path = project.root / source
    if not path.is_file():
        return []
    check_types = _bridge.import_tool("check_types")
    text = check_types.clean(path.read_text(errors="replace"))
    names = {sym["name"], f"D_{sym['address']:08X}", f"D_{sym['address']:08x}"}
    out = []
    for m in check_types.EXTERN_VAR.finditer(text):
        decl = " ".join(m.group(1).split())
        if any(re.search(rf"\b{re.escape(n)}\b", decl) for n in names):
            out.append(decl)
    return out


def data_context(project, target) -> dict:
    hits = resolve_data(project, target)
    if not hits:
        return schemas.err("get_context", f"no data symbol {target!r}", code="not_found")
    if len(hits) > 1:
        return schemas.err("get_context", f"{target!r} names several data symbols",
                           code="ambiguous",
                           candidates=[data_id(project, s) for s in hits])
    sym = hits[0]
    did = data_id(project, sym)
    path, data = _manifest(project, sym["unit"])
    size = sym.get("size") or sym["extent"]
    blob = _retail_bytes(project, sym, min(size, 256))
    preview = None
    if blob is not None:
        words = [struct.unpack_from("<I", blob, i)[0] for i in range(0, len(blob) - 3, 4)]
        preview = {"hex": blob[:64].hex(),
                   "words": [f"0x{w:08x}" for w in words[:16]],
                   "floats": [struct.unpack("<f", struct.pack("<I", w))[0]
                              for w in words[:16]],
                   "all_zero": not any(blob)}
    fns = data.get("function", []) if data else []
    facts = {
        "id": did, "key": sym["key"], "name": sym["name"], "scope": sym["scope"],
        "unit": f"{project.version}:unit-{sym['unit']:04d}",
        "source": data.get("source") if data else None,
        "manifest": path.relative_to(project.root).as_posix() if path else None,
        "state": _entry_state(data, did),
        "address": f"0x{sym['address']:08x}", "section": sym["section"],
        "storage": sym["storage"], "size": sym.get("size"),
        "size_evidence": sym.get("size_evidence"), "extent": sym["extent"],
        "pad_zero": sym.get("pad_zero"),
        "unit_fully_matching": bool(fns) and all(f["state"] == "matching" for f in fns),
    }
    types = []
    if sym.get("type"):
        types.append({"level": 1, "source": "stabs", "type": sym["type"],
                      "stab": sym.get("stab")})
    for decl in _externs(project, facts["source"] or "", sym):
        types.append({"level": 2, "source": "extern", "decl": decl})
    unknowns, hints = [], []
    if facts["state"] is None:
        unknowns.append("the manifest has no [[data]] entry for it "
                        "(python tools/promote.py --init-data <source>)")
    if not facts["unit_fully_matching"]:
        unknowns.append("the unit is not fully matching: its data stays retail "
                        "(PLAN_DATA Part 3 preconditions)")
    if sym["section"] not in (".data", ".rodata"):
        unknowns.append(f"{sym['section']} data from C is a later PR")
    if sym.get("size") is None:
        hints.append("unsized: the compiled .size is accepted only when at most "
                     "15 zero bytes remain to the next item")
    dump = f"python tools/dump_data.py {did}" + ("" if sym.get("type") else " --type '<C type>'")
    return schemas.ok("get_context", kind="data", facts=facts, bytes=preview,
                      type_evidence=types,
                      dump_command=dump,
                      current_c=_current_c(project, facts["source"], sym)
                      if facts["source"] else None,
                      rules=[
                          "define it under its retail name and storage (static "
                          "where retail is static, const for .rodata)",
                          "function statics live inside the function that "
                          "references them",
                          "only promote_matching writes state = \"matching\"",
                      ],
                      unknowns=unknowns, hints=hints)


# --- diff / verify ----------------------------------------------------------------

def data_diff(project, target) -> dict:
    """Byte-compare one data entry built from C (toolchain-bound)."""
    hits = resolve_data(project, target)
    if len(hits) != 1:
        return schemas.err("compile_diff", f"{target!r} does not name one data symbol",
                           code="not_found" if not hits else "ambiguous")
    sym = hits[0]
    did = data_id(project, sym)
    path, data = _manifest(project, sym["unit"])
    if data is None or _entry_state(data, did) is None:
        return schemas.err("compile_diff", f"{did} has no [[data]] entry",
                           code="no_entry",
                           next_step=f"python tools/promote.py --init-data "
                                     f"{data['source'] if data else '<source>'}")
    gen_hybrid = _bridge.import_tool("gen_hybrid")
    verify = _bridge.import_tool("declib.verify")
    asmtext = _bridge.import_tool("declib.asmtext")
    toolchain = _bridge.import_tool("declib.toolchain")
    text = path.read_text()
    flipped = re.sub(r'(id = "' + re.escape(did) + r'"\s*\r?\nstate = ")[a-z]+(")',
                     r"\g<1>matching\g<2>", text)
    work = project.root / "build" / project.version / "agent" / "data"
    work.mkdir(parents=True, exist_ok=True)
    temp_manifest = work / f"unit-{sym['unit']:04d}.toml"
    temp_manifest.write_text(flipped)
    out_o = work / f"unit-{sym['unit']:04d}.o"
    try:
        gen_hybrid.build_hybrid(temp_manifest, out_o, link_set="matching",
                                version=project.version)
    except gen_hybrid.HybridError as exc:
        message = str(exc)
        category = ("data_section_mismatch" if "data_section_mismatch" in message
                    else "bss_common" if ".bss" in message or "COMMON" in message
                    else "compile_error")
        return schemas.ok("compile_diff", kind="data", data_id=did, exact=False,
                          hybrid_error=message, category=category)
    report = json.loads(gen_hybrid.data_report_path(out_o).read_text())
    nm_bin = toolchain.tool_path(toolchain.NM)
    undef = verify.undefined_externals(nm_bin, out_o)
    defsyms, unresolved = asmtext.resolve(sorted(undef - {"_gp"}),
                                          asmtext.load_symbol_addrs(project.version))
    if unresolved:
        return schemas.ok("compile_diff", kind="data", data_id=did, exact=False,
                          unresolved=unresolved[:8], category="unresolved_symbol")
    funcs = tomllib.loads(project.path("config", project.version,
                                       "functions.toml").read_text())["function"]
    base = min(f["address"] for f in funcs if f["unit"] == sym["unit"])
    with tempfile.TemporaryDirectory() as tmp:
        _text, pieces, symbols = verify.link_unit_at(out_o, base, report["pieces"],
                                                     defsyms, Path(tmp))
    obj = next((o for p in report["pieces"] for o in p.get("objects", [])
                if o["address"] == sym["address"]), None)
    size = sym.get("size") or (obj or {}).get("size")
    piece = next((p for p in report["pieces"]
                  if p["start"] <= sym["address"] < p["end"]), None)
    got = pieces[piece["name"]][sym["address"] - piece["start"]:
                                sym["address"] - piece["start"] + size] if piece else b""
    want = _retail_bytes(project, sym, size) or b""
    diffs = [i for i in range(min(len(got), len(want))) if got[i] != want[i]]
    exact = got == want and symbols.get(sym["name"]) == sym["address"]
    return schemas.ok("compile_diff", kind="data", data_id=did, exact=exact,
                      size=size, linked_at=f"0x{symbols.get(sym['name'], 0):08x}",
                      first_diffs=[{"offset": i, "want": want[i], "got": got[i]}
                                   for i in diffs[:16]],
                      differing_bytes=len(diffs),
                      retail_bytes_in_pieces=report["retail"],
                      next_step="promote_matching" if exact else
                      "fix the initialiser (python tools/dump_data.py "
                      f"{did} --verify)")
