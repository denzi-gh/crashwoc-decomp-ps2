#!/usr/bin/env python3
"""Validate the committed function-status manifests against the mdebug registries.

Progress state lives in one hand-edited TOML per translation unit
(config/<version>/status/<group>/<unit>.toml); the mdebug-derived registries
(units.toml, functions.toml) stay generated and untouchable. This tool is the
public gate that keeps the two consistent -- it needs no game files and no
toolchain, so it runs on every pull request:

  * every manifest parses, uses schema 1, and declares a known profile
  * the manifest's path, `source` file, and mdebug unit all name the same TU
  * every function id (version:unit-NNNN:vram:name) exists in functions.toml
    with exactly that unit, address, and name
  * the manifest lists the unit's full function set -- no missing entries,
    no duplicates, no strays -- each with a valid state
    (asm | equivalent | matching)
  * the optional `[[data]]` table (Data-from-C) names data registry symbols
    (config/<version>/data_symbols.toml) of the unit by id
    (version:unit-NNNN:vram:name), each with a state asm | matching | retail
    (`retail` = raw asset kept as retail bytes, set by hand with a `reason`);
    when present it lists every symbol of the unit in a section with file
    bytes (.data, .rodata, .sdata) exactly once -- never a .sbss/.bss one
  * `complete = true` requires every function to be `matching` and every
    data entry `matching` or `retail` (the byte rules are verify_promoted's)
  * the optional `[[claim]]` table (section/start/end, written by
    promote.py --complete) only on a complete unit: each claim inside one
    unassigned data-map gap, no overlaps, no gap claimed by two units
  * every src/**/*.c has a manifest, so no source file can escape accounting

A `matching` state here is a *claim*; the container-side verifier re-derives
the bytes before it is trusted. This tool only guarantees the claim is
well-formed and structurally consistent.

Exit status is 0 only if every manifest (and the src coverage rule) passes.
"""
import argparse
import functools
import re
import sys
import tomllib
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from declib.tu import parse_toml_blocks

STATES = ("asm", "equivalent", "matching")
MANIFEST_KEYS = {"schema", "unit", "source", "profile", "complete", "function",
                 "data", "claim"}
FUNCTION_KEYS = {"id", "state"}
DATA_STATES = ("asm", "matching", "retail")
DATA_KEYS = {"id", "state", "reason"}
CLAIM_KEYS = {"section", "start", "end"}
# Data sections with file bytes; .sbss/.bss symbols never get [[data]] entries.
DATA_PROGBITS = (".data", ".rodata", ".sdata")
ID_RE = re.compile(r"^([a-z0-9]+):unit-(\d{4}):([0-9a-f]{8}):(\S+)$")
UNIT_RE = re.compile(r"^([a-z0-9]+):unit-(\d{4})$")


def load_registry(config_dir):
    """(units, functions) from the generated mdebug registries.

    units: {index: original source path}; functions: {unit_index: {vram: name}}.
    """
    units = {int(r["index"]): r["name"] for r in parse_toml_blocks(
        config_dir / "units.toml",
        {"index": r"index = (\d+)", "name": r"name = '([^']*)'"})}
    functions = {}
    for r in parse_toml_blocks(
            config_dir / "functions.toml",
            {"name": r"name = '([^']*)'", "address": r"address = (0x[0-9A-Fa-f]+)",
             "unit": r"unit = (\d+)"}):
        functions.setdefault(int(r["unit"]), {})[int(r["address"], 16)] = r["name"]
    return units, functions


def load_data_registry(config_dir):
    """{unit_index: {address: symbol record}} from data_symbols.toml ({} when
    the registry is absent)."""
    path = Path(config_dir) / "data_symbols.toml"
    if not path.is_file():
        return {}
    stat = path.stat()
    return _data_registry(str(path), stat.st_mtime_ns, stat.st_size)


@functools.lru_cache(maxsize=4)
def _data_registry(path, _mtime, _size):
    out = {}
    for sym in tomllib.loads(Path(path).read_text()).get("symbol", []):
        out.setdefault(sym["unit"], {})[sym["address"]] = sym
    return out


def check_data(entries, version, unit_index, registry):
    """(problems, {address: state}) for a manifest's `[[data]]` table."""
    problems, states = [], {}
    unit_syms = registry.get(unit_index, {}) if unit_index is not None else {}
    for i, entry in enumerate(entries):
        unknown = set(entry) - DATA_KEYS
        if unknown:
            problems.append(f"data #{i}: unknown keys: {', '.join(sorted(unknown))}")
        did = entry.get("id", "")
        m = ID_RE.match(did)
        if not m:
            problems.append(f"data #{i}: bad id {did!r}")
            continue
        dversion, dunit, daddr, dname = (m.group(1), int(m.group(2)),
                                         int(m.group(3), 16), m.group(4))
        if dversion != version or dunit != unit_index:
            problems.append(f"{did}: id names a different version/unit than "
                            f"the manifest")
            continue
        sym = unit_syms.get(daddr)
        if sym is None:
            problems.append(f"{did}: no data symbol at 0x{daddr:08x} in unit "
                            f"{unit_index} (data_symbols.toml)")
        elif sym["name"] != dname:
            problems.append(f"{did}: data_symbols.toml names 0x{daddr:08x} "
                            f"'{sym['name']}', not '{dname}'")
        elif sym["section"] not in DATA_PROGBITS:
            problems.append(f"{did}: {sym['section']} symbols carry no file "
                            f"bytes and get no [[data]] entry")
        if daddr in states:
            problems.append(f"{did}: duplicate data entry")
        state = entry.get("state")
        if state not in DATA_STATES:
            problems.append(f"{did}: bad data state {state!r} "
                            f"(must be one of {', '.join(DATA_STATES)})")
        if state == "retail" and not str(entry.get("reason", "")).strip():
            problems.append(f"{did}: state = \"retail\" needs a `reason`")
        if state != "retail" and "reason" in entry:
            problems.append(f"{did}: `reason` is only for state = \"retail\"")
        states[daddr] = state
    if entries:
        expected = {a for a, s in unit_syms.items() if s["section"] in DATA_PROGBITS}
        for addr in sorted(expected - set(states)):
            problems.append(f"data symbol 0x{addr:08x} ({unit_syms[addr]['name']}) "
                            f"not listed")
    return problems, states


def load_data_gaps(config_dir):
    """[(section, start, end)] of the data map's unassigned gap ranges ([]
    when the map is absent)."""
    path = Path(config_dir) / "data_map.toml"
    if not path.is_file():
        return []
    stat = path.stat()
    return _data_gaps(str(path), stat.st_mtime_ns, stat.st_size)


@functools.lru_cache(maxsize=4)
def _data_gaps(path, _mtime, _size):
    return [(r["section"], r["start"], r["end"])
            for r in tomllib.loads(Path(path).read_text()).get("range", [])
            if r["owner"] == "unassigned"]


def check_claims(claims, complete, gaps):
    """(problems, {(section, gap start)}) for a manifest's `[[claim]]` table:
    the gap bytes a complete unit's whole-unit hybrid emits (written by
    promote.py --complete). Each claim lies inside one data-map gap; claims
    never overlap."""
    problems, used, seen = [], set(), []
    if claims and not complete:
        problems.append("[[claim]] needs complete = true")
    for i, claim in enumerate(claims):
        unknown = set(claim) - CLAIM_KEYS
        if unknown:
            problems.append(f"claim #{i}: unknown keys: {', '.join(sorted(unknown))}")
        section, start, end = claim.get("section"), claim.get("start"), claim.get("end")
        if section not in DATA_PROGBITS:
            problems.append(f"claim #{i}: section must be one of "
                            f"{', '.join(DATA_PROGBITS)}, got {section!r}")
            continue
        if not isinstance(start, int) or not isinstance(end, int) or start >= end:
            problems.append(f"claim #{i}: start/end must be integers with "
                            f"start < end")
            continue
        gap = next((g for g in gaps if g[0] == section
                    and g[1] <= start and end <= g[2]), None)
        if gap is None:
            problems.append(f"claim #{i}: {section} 0x{start:08x}-0x{end:08x} "
                            f"is not inside one data-map gap")
            continue
        if any(s == section and start < e and b < end for s, b, e in seen):
            problems.append(f"claim #{i}: overlaps another claim")
        seen.append((section, start, end))
        used.add((section, gap[1]))
    return problems, used


def load_profile_names(config_dir):
    data = tomllib.loads((config_dir / "profiles.toml").read_text())
    return set(data.get("profile", {}))


def check_manifest(path, version, root):
    """Return a list of problem strings for one status manifest."""
    config_dir = root / "config" / version
    units, functions = load_registry(config_dir)
    profiles = load_profile_names(config_dir)
    data_registry = load_data_registry(config_dir)
    rel = path.relative_to(root / "config" / version / "status").as_posix()
    problems = []

    try:
        data = tomllib.loads(path.read_text())
    except tomllib.TOMLDecodeError as exc:
        return [f"TOML parse error: {exc}"]

    unknown = set(data) - MANIFEST_KEYS
    if unknown:
        problems.append(f"unknown keys: {', '.join(sorted(unknown))}")
    if data.get("schema") != 1:
        problems.append(f"schema must be 1, got {data.get('schema')!r}")

    # The manifest path, `source`, and mdebug unit must agree on the TU.
    source = data.get("source")
    if not isinstance(source, str):
        problems.append("missing `source`")
        source = None
    else:
        expected_source = f"src/{rel[:-len('.toml')]}.c"
        if source != expected_source:
            problems.append(f"source '{source}' does not match manifest path "
                            f"(expected '{expected_source}')")
        if not (root / source).is_file():
            problems.append(f"source file '{source}' does not exist")

    unit_index = None
    m = UNIT_RE.match(data.get("unit", "") or "")
    if not m:
        problems.append(f"bad unit id {data.get('unit')!r} "
                        f"(expected '{version}:unit-NNNN')")
    elif m.group(1) != version:
        problems.append(f"unit id version '{m.group(1)}' != '{version}'")
    else:
        unit_index = int(m.group(2))
        if unit_index not in units:
            problems.append(f"unit {unit_index} not in units.toml")
        elif source:
            retail_base = re.split(r"[\\/]", units[unit_index])[-1]
            # Units sharing a retail name (16 graphdev.c, ...) carry their
            # index like their objdiff unit name: graphdev_127.c.
            stem, dot, ext = retail_base.rpartition(".")
            indexed = f"{stem}_{unit_index:03d}{dot}{ext}"
            if Path(source).name not in (retail_base, indexed):
                problems.append(
                    f"unit {unit_index} is '{retail_base}' in units.toml but "
                    f"source is '{Path(source).name}'")

    profile = data.get("profile")
    if profile not in profiles:
        problems.append(f"unknown profile {profile!r} "
                        f"(profiles.toml has: {', '.join(sorted(profiles))})")

    # Function entries: full set, no duplicates, valid ids and states.
    entries = data.get("function", [])
    states = {}
    for i, entry in enumerate(entries):
        unknown = set(entry) - FUNCTION_KEYS
        if unknown:
            problems.append(f"function #{i}: unknown keys: "
                            f"{', '.join(sorted(unknown))}")
        fid = entry.get("id", "")
        fm = ID_RE.match(fid)
        if not fm:
            problems.append(f"function #{i}: bad id {fid!r}")
            continue
        fversion, funit, faddr, fname = (fm.group(1), int(fm.group(2)),
                                         int(fm.group(3), 16), fm.group(4))
        if fversion != version or funit != unit_index:
            problems.append(f"{fid}: id names a different version/unit "
                            f"than the manifest")
            continue
        registry = functions.get(unit_index, {})
        if faddr not in registry:
            problems.append(f"{fid}: no function at 0x{faddr:08x} in unit "
                            f"{unit_index} (functions.toml)")
        elif registry[faddr] != fname:
            problems.append(f"{fid}: functions.toml names 0x{faddr:08x} "
                            f"'{registry[faddr]}', not '{fname}'")
        if faddr in states:
            problems.append(f"{fid}: duplicate entry")
        state = entry.get("state")
        if state not in STATES:
            problems.append(f"{fid}: bad state {state!r} "
                            f"(must be one of {', '.join(STATES)})")
        states[faddr] = state

    if unit_index is not None:
        expected = set(functions.get(unit_index, {}))
        missing = expected - set(states)
        extra = set(states) - expected
        for addr in sorted(missing):
            problems.append(f"function 0x{addr:08x} "
                            f"({functions[unit_index][addr]}) not listed")
        for addr in sorted(extra):
            problems.append(f"listed function 0x{addr:08x} does not belong "
                            f"to unit {unit_index}")

    data_entries = data.get("data", [])
    data_problems, data_states = check_data(data_entries, version, unit_index,
                                            data_registry)
    problems += data_problems

    if data.get("complete") not in (True, False, None):
        problems.append(f"complete must be a boolean")
    if data.get("complete") and any(s != "matching" for s in states.values()):
        problems.append("complete = true but not every function is `matching`")
    if data.get("complete") and any(s not in ("matching", "retail")
                                    for s in data_states.values()):
        problems.append("complete = true but a data entry is still `asm`")
    if data.get("complete") and not data_entries and unit_index is not None \
            and any(s["section"] in DATA_PROGBITS
                    for s in data_registry.get(unit_index, {}).values()):
        problems.append("complete = true but the unit's data symbols have no "
                        "[[data]] entries (promote.py --init-data)")
    claim_problems, _gaps = check_claims(data.get("claim", []),
                                         data.get("complete"),
                                         load_data_gaps(config_dir))
    problems += claim_problems

    return problems


def check_version(version, root):
    """Validate all manifests of one version plus the src coverage rule.

    Returns (problems_by_label, state_counts).
    """
    status_dir = root / "config" / version / "status"
    results = {}
    counts = {s: 0 for s in STATES}
    counts.update({f"data_{s}": 0 for s in DATA_STATES})

    manifests = sorted(status_dir.rglob("*.toml")) if status_dir.is_dir() else []
    gaps = load_data_gaps(root / "config" / version)
    claimed = {}
    for path in manifests:
        rel = path.relative_to(status_dir).as_posix()
        problems = check_manifest(path, version, root)
        results[f"{version}/{rel}"] = problems
        if not problems:
            data = tomllib.loads(path.read_text())
            for entry in data.get("function", []):
                counts[entry["state"]] += 1
            for entry in data.get("data", []):
                counts[f"data_{entry['state']}"] += 1
            # A claimed gap range is supplied whole by its claimant's hybrid.
            _p, used = check_claims(data.get("claim", []), True, gaps)
            for key in used:
                other = claimed.setdefault(key, rel)
                if other != rel:
                    problems.append(f"{key[0]} gap 0x{key[1]:08x} is also "
                                    f"claimed by {other}")

    # Every C source must be accounted for by a manifest.
    src_dir = root / "src"
    covered = {tomllib.loads(p.read_text()).get("source")
               for p in manifests if not results.get(
                   f"{version}/{p.relative_to(status_dir).as_posix()}")}
    for src in sorted(src_dir.rglob("*.c")) if src_dir.is_dir() else []:
        rel_src = src.relative_to(root).as_posix()
        if rel_src not in covered:
            results.setdefault(f"{version}/<coverage>", []).append(
                f"{rel_src} has no status manifest "
                f"(expected config/{version}/status/"
                f"{src.relative_to(src_dir).with_suffix('.toml').as_posix()})")

    return results, counts


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--check", action="store_true",
                        help="validate and exit nonzero on any problem "
                             "(the default behavior; flag kept for symmetry "
                             "with the other tools)")
    args = parser.parse_args()

    versions = sorted(p.name for p in (ROOT / "config").iterdir()
                      if (p / "units.toml").is_file())
    failed = False
    total = {s: 0 for s in STATES}
    total.update({f"data_{s}": 0 for s in DATA_STATES})
    for version in versions:
        results, counts = check_version(version, ROOT)
        for s in total:
            total[s] += counts[s]
        for label, problems in sorted(results.items()):
            ok = not problems
            print(f"{label + ':':<44} {'PASS' if ok else 'FAIL'}")
            for p in problems:
                print(f"  - {p}")
            failed |= not ok

    print(f"\nFunctions with C accepted: "
          f"{total['matching']} matching, {total['equivalent']} equivalent "
          f"(everything else is asm)")
    if any(total[f"data_{s}"] for s in DATA_STATES):
        print(f"Data entries: {total['data_matching']} matching, "
              f"{total['data_retail']} retail, {total['data_asm']} asm")
    if failed:
        print("Status manifests FAILED validation.")
        return 1
    print("Status manifests OK.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
