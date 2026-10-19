#!/usr/bin/env python3
"""Promote a function or data symbol to `matching` -- the only writer of it.

    python tools/promote.py NuListGetHead
    python tools/promote.py pal103:unit-0007:00105a50:NuListGetHead
    python tools/promote.py A B C            # one verifier run for all three
    python tools/promote.py --targets-file ready.txt
    python tools/promote.py --init src/nucore/nulist.c
    python tools/promote.py --init-data src/numath/nuvec4.c
    python tools/promote.py pal103:unit-0014:00613fb0:nuvec4_zero   # data id
    python tools/promote.py --complete pal103:unit-0014

A hand-edited `state = "matching"` is never trusted (and the container CI
re-verifies every claim); this tool makes the honest path the easy path:

  1. Find each function's manifest entry (by full id, or by name when the
     name is unique across manifests) and flip its state in the file. All
     targets are resolved before anything is written, so a typo cannot
     leave a batch half-flipped.
  2. Run the full promotion verifier (tools/verify_promoted.py): manifest
     consistency, each function's bytes against retail over its whole
     registry extent, every previously promoted function again, and the
     matching-image SHA gate.
  3. If ANYTHING fails, every manifest edit is rolled back exactly and the
     failure is reported -- the repo never holds an unverified promotion.

Because step 2 re-derives *every* `matching` claim in the repo, promoting N
functions together is exactly as strong a proof as promoting them one by one
-- and costs one verifier run instead of N (that run is several minutes and
grows with the matching set). The only difference is granularity of failure:
a batch rolls back whole, so re-run the targets individually, or in halves,
to find the one that does not hold.

`--init` creates the status manifest for a source file that doesn't have
one yet (every function `asm`), deriving the unit from the canonical
source-path mapping (unit `nucore/nulist` <-> src/nucore/nulist.c). Edit
nothing by hand afterwards except `equivalent` states, which are a
reviewed human judgement, and data `retail` states (a raw asset kept as
retail bytes, with a `reason`).

Data symbols (`[[data]]`, ids shaped like function ids) are promoted the same
way; `--init-data` adds the unit's table (every registry symbol with file
bytes, `asm`). `--complete <unit id>` sets `complete = true`, which switches
the unit's hybrid to whole-unit mode (its compiler-private data, jump tables
and pools from C, gen_hybrid), writes the `[[claim]]` table of the gap bytes
that layout emits, and runs the verifier, which then also proves the unit's
pieces carry no retail bytes beyond its `retail` entries and equal retail;
it rolls back on failure. On a unit that is already complete it refreshes
the claims. It also promotes the unit's `asm` entries that lie outside every
range the unit owns (an unsized trailing symbol in a data-map gap): only the
whole-unit layout can place those, so promoting such an id directly is
refused.

Needs the toolchain for the verify step: run via
`python tools/dispatch.py python tools/promote.py ...` from the host.
"""
import argparse
import re
import subprocess
import sys
import tomllib
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from gen_objdiff import unit_table
from declib.tu import parse_toml_blocks
from status import load_profile_names


def default_profile_for(category):
    """Safe default compiler profile for a translation-unit category.

    Game and engine code only matches under the SN ProDG compiler (profile
    `default`); the SCE / newlib / libgcc / runtime half -- all classified
    `sdk` by gen_objdiff.classify -- needs the Sony compiler (profile `sce`).
    An explicit --profile override still wins over this default.
    """
    return "sce" if category == "sdk" else "default"


def resolve_profile(profile, category, known):
    """The profile to write into a new manifest: an explicit override when
    given, otherwise the category default. Either way it must be a profile
    that exists in profiles.toml, so a typo fails loudly at --init time
    instead of only later in status validation."""
    chosen = profile if profile is not None else default_profile_for(category)
    if chosen not in known:
        raise SystemExit(
            f"promote --init: unknown profile {chosen!r} "
            f"(profiles.toml has: {', '.join(sorted(known))})")
    return chosen


def find_entry(version, needle):
    """(manifest_path, full_id, state) for a function or data id, or a name
    unique across both tables of every manifest."""
    status_dir = ROOT / "config" / version / "status"
    hits = []
    for manifest in sorted(status_dir.rglob("*.toml")) \
            if status_dir.is_dir() else []:
        data = tomllib.loads(manifest.read_text())
        for entry in data.get("function", []) + data.get("data", []):
            fid = entry["id"]
            if fid == needle or fid.rsplit(":", 1)[1] == needle:
                hits.append((manifest, fid, entry["state"]))
    if not hits:
        raise SystemExit(
            f"promote: no manifest entry found for {needle!r}. If the unit "
            f"has no manifest yet, create one with --init src/<unit>.c")
    if len(hits) > 1:
        ids = ", ".join(h[1] for h in hits)
        raise SystemExit(f"promote: {needle!r} is ambiguous ({ids}); "
                         f"use the full id")
    return hits[0]


def flip_state(text, fid, new_state):
    """Return manifest text with `fid`'s state replaced (exactly once)."""
    pattern = re.compile(
        r'(id = "' + re.escape(fid) + r'"\s*\r?\nstate = ")[a-z]+(")')
    out, n = pattern.subn(rf"\g<1>{new_state}\g<2>", text)
    if n != 1:
        raise SystemExit(f"promote: could not locate the state line for "
                         f"{fid} ({n} matches)")
    return out


def run_verifier(version):
    return subprocess.run(
        [sys.executable, str(ROOT / "tools" / "verify_promoted.py"),
         "--version", version]).returncode == 0


def unit_functions(version, unit_index):
    """[(addr_int, name)] for one unit, address-sorted, from functions.toml."""
    return sorted(
        (int(r["a"], 16), r["n"]) for r in parse_toml_blocks(
            ROOT / "config" / version / "functions.toml",
            {"n": r"name = '([^']*)'", "a": r"address = (0x[0-9A-Fa-f]+)",
             "u": r"unit = (\d+)"}) if int(r["u"]) == unit_index)


def manifest_text(version, unit_index, name, src_rel, profile, funcs):
    """The exact all-asm status manifest body for a unit (used by `--init`)."""
    lines = [
        f"# Function status for {version} unit {unit_index} ({name}.c).",
        "#",
        "# Generated by `python tools/promote.py --init`. States: asm (no",
        "# accepted C), equivalent (reviewed, behaviorally equivalent C),",
        "# matching (mechanically byte-exact C -- promoted only by",
        "# tools/promote.py, verified by tools/verify_promoted.py).",
        "",
        "schema = 1",
        f'unit = "{version}:unit-{unit_index:04d}"',
        f'source = "{src_rel}"',
        f'profile = "{profile}"',
        "complete = false",
        "",
    ]
    for addr, fname in funcs:
        lines += ["[[function]]",
                  f'id = "{version}:unit-{unit_index:04d}:{addr:08x}:{fname}"',
                  'state = "asm"', ""]
    return "\n".join(lines)


def init_manifest(version, source, profile=None):
    """Create a fresh all-asm manifest for src/<unit>.c."""
    src_rel = Path(source).as_posix()
    if not (ROOT / src_rel).is_file():
        raise SystemExit(f"promote --init: {src_rel} does not exist")
    name = src_rel.removeprefix("src/").removesuffix(".c")
    row = next((r for r in unit_table(version) if r[2] == name), None)
    if row is None:
        raise SystemExit(
            f"promote --init: no .text unit is named '{name}' "
            f"(see objdiff.json for the canonical unit names)")
    unit_index = row[0]
    category = row[3]
    profile = resolve_profile(profile, category,
                              load_profile_names(ROOT / "config" / version))

    out = ROOT / "config" / version / "status" / f"{name}.toml"
    if out.is_file():
        raise SystemExit(f"promote --init: {out.relative_to(ROOT).as_posix()} "
                         f"already exists")
    funcs = unit_functions(version, unit_index)
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(manifest_text(version, unit_index, name, src_rel, profile,
                                 funcs))
    print(f"wrote {out.relative_to(ROOT).as_posix()} "
          f"({len(funcs)} functions, all asm, profile {profile!r})")
    print("Regenerate the build config so the unit gets its hybrid edges:\n"
          "    python tools/gen_ninja.py")
    return 0


def manifest_for(version, needle):
    """The manifest path for a unit id (version:unit-NNNN) or src/<unit>.c."""
    status_dir = ROOT / "config" / version / "status"
    for manifest in sorted(status_dir.rglob("*.toml")):
        data = tomllib.loads(manifest.read_text())
        if needle in (data.get("unit"), data.get("source")):
            return manifest, data
    raise SystemExit(f"promote: no status manifest for {needle!r}")


def data_table_text(version, unit_index, symbols):
    """`[[data]]` blocks (all `asm`) for the unit's registry symbols."""
    lines = []
    for sym in symbols:
        lines += ["[[data]]",
                  f'id = "{version}:unit-{unit_index:04d}:{sym["address"]:08x}:'
                  f'{sym["name"]}"', 'state = "asm"', ""]
    return "\n".join(lines)


def init_data(version, source):
    """Append an all-asm `[[data]]` table to an existing manifest."""
    from status import DATA_PROGBITS, load_data_registry
    manifest, data = manifest_for(version, Path(source).as_posix())
    if data.get("data"):
        raise SystemExit(f"promote --init-data: {manifest.relative_to(ROOT).as_posix()} "
                         f"already has a [[data]] table")
    unit_index = int(data["unit"].split("unit-")[1])
    syms = sorted((s for s in load_data_registry(ROOT / "config" / version)
                   .get(unit_index, {}).values() if s["section"] in DATA_PROGBITS),
                  key=lambda s: s["address"])
    if not syms:
        raise SystemExit(f"promote --init-data: unit {unit_index} has no data "
                         f"symbols with file bytes")
    text = read_manifest(manifest)
    newline = "\r\n" if "\r\n" in text else "\n"
    body = data_table_text(version, unit_index, syms).replace("\n", newline)
    text = text.rstrip("\r\n") + newline + newline + body
    write_manifest(manifest, text)
    print(f"added {len(syms)} [[data]] entries (asm) to "
          f"{manifest.relative_to(ROOT).as_posix()}")
    return 0


def whole_unit_only(version, data):
    """The unit's `asm` [[data]] ids only its whole-unit layout can place.

    Each lies outside every range the unit owns (an unsized trailing symbol
    in a data-map gap: its size is the compiled one, and the layout claims
    those gap bytes), so `--complete` promotes it with the unit."""
    from gen_hybrid import unit_data_ranges
    ranges = unit_data_ranges(version, data["unit"].split(":")[1])
    return [e["id"] for e in data.get("data", []) if e["state"] == "asm"
            and not any(r["start"] <= int(e["id"].split(":")[2], 16) < r["end"]
                        for r in ranges)]


def set_complete(text):
    """Manifest text with `complete = true` (exactly once)."""
    if re.search(r"^complete = true\r?$", text, flags=re.M):
        return text
    out, n = re.subn(r"^complete = false(\r?)$", r"complete = true\1", text,
                     flags=re.M)
    if n != 1:
        raise SystemExit("promote --complete: no `complete = false` line")
    return out


_CLAIM_BLOCK_RE = re.compile(
    r"(?:\r?\n)*\[\[claim\]\]\r?\nsection = \"[^\"]*\"\r?\n"
    r"start = 0x[0-9a-f]+\r?\nend = 0x[0-9a-f]+(?:\r?\n)?")


def write_claims(text, claims):
    """Manifest text with its `[[claim]]` table replaced by `claims`
    ([(section, start, end)]), appended at the end."""
    from gen_hybrid import claim_text
    newline = "\r\n" if "\r\n" in text else "\n"
    body = _CLAIM_BLOCK_RE.sub("", text).rstrip("\r\n") + newline
    if "[[claim]]" in body:
        raise SystemExit("promote --complete: a [[claim]] block is not in "
                         "the form promote.py writes")
    if claims:
        body += newline + claim_text(claims).replace("\n", newline)
    return body


def read_manifest(path):
    """Manifest text, byte-preserving (see the newline="" note in promote)."""
    with open(path, encoding="utf-8", newline="") as f:
        return f.read()


def write_manifest(path, text):
    with open(path, "w", encoding="utf-8", newline="") as f:
        f.write(text)


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("target", nargs="*",
                        help="function id(s) or unique function name(s); "
                             "several are flipped together and proved by a "
                             "single verifier run")
    parser.add_argument("--targets-file", metavar="FILE",
                        help="read additional targets from FILE, one per line "
                             "(blank lines and #-comments ignored)")
    parser.add_argument("--to", default="matching", choices=("matching",),
                        help="target state (only `matching`; `equivalent` is "
                             "a reviewed hand edit)")
    parser.add_argument("--init", metavar="SRC",
                        help="create an all-asm manifest for src/<unit>.c "
                             "instead of promoting")
    parser.add_argument("--init-data", metavar="SRC",
                        help="add an all-asm [[data]] table to the manifest "
                             "of src/<unit>.c")
    parser.add_argument("--complete", metavar="UNIT", nargs="+",
                        help="set complete = true for unit ids "
                             "(version:unit-NNNN) and write their [[claim]] "
                             "tables, after one full verification")
    parser.add_argument("--profile", default=None,
                        help="compiler profile for the new --init manifest "
                             "(default: derived from the unit category -- "
                             "sce for SDK/SCE/newlib/libgcc, else default); "
                             "must exist in profiles.toml")
    parser.add_argument("--version", default="pal103")
    args = parser.parse_args()

    if args.profile and not args.init:
        parser.error("--profile only applies to --init")
    if args.init:
        return init_manifest(args.version, args.init, args.profile)
    if args.init_data:
        return init_data(args.version, args.init_data)
    if args.complete:
        from gen_hybrid import HybridError, plan_claims
        originals = {}
        for unit in args.complete:
            manifest, _data = manifest_for(args.version, unit)
            originals[manifest] = read_manifest(manifest)
        try:
            for manifest, text in originals.items():
                text = set_complete(text)
                for did in whole_unit_only(args.version, tomllib.loads(text)):
                    text = flip_state(text, did, "matching")
                    print(f"{did}: asm -> matching (whole-unit layout)")
                write_manifest(manifest, text)
                claims = plan_claims(manifest, args.version)
                write_manifest(manifest, write_claims(read_manifest(manifest),
                                                      claims))
                print(f"complete = true in {manifest.relative_to(ROOT).as_posix()}"
                      f" ({len(claims)} claim(s))")
        except HybridError as exc:
            for manifest, text in originals.items():
                write_manifest(manifest, text)
            print(f"\nwhole-unit layout FAILED, nothing changed: {exc}",
                  file=sys.stderr)
            return 1
        print(f"\nverifying {len(originals)} unit(s)...\n", flush=True)
        if run_verifier(args.version):
            print(f"\n{len(originals)} unit(s) complete.")
            return 0
        for manifest, text in originals.items():
            write_manifest(manifest, text)
        print(f"\nVerification FAILED; complete rolled back for all "
              f"{len(originals)} unit(s).", file=sys.stderr)
        return 1

    targets = list(args.target)
    if args.targets_file:
        for line in Path(args.targets_file).read_text().splitlines():
            line = line.split("#", 1)[0].strip()
            if line:
                targets.append(line)
    if not targets:
        parser.error("a function/data id or name is required (or --init SRC)")

    # Resolve every target first: a typo must not leave half the batch flipped.
    pending = []                       # [(manifest, fid, old_state)]
    for t in targets:
        manifest, fid, state = find_entry(args.version, t)
        rel = manifest.relative_to(ROOT).as_posix()
        if state == "matching":
            print(f"{fid} is already `matching` in {rel}; nothing to do.")
            continue
        if fid in whole_unit_only(args.version, tomllib.loads(manifest.read_text())):
            raise SystemExit(f"promote: {fid} lies outside the ranges its unit "
                             f"owns; only the whole-unit layout places it: "
                             f"promote.py --complete {fid.rsplit(':', 2)[0]}")
        pending.append((manifest, fid, state))
    if not pending:
        return 0

    # newline="" everywhere: the edit must be byte-preserving outside the
    # flipped words, whatever line endings the file uses and whatever
    # platform this runs on. One original per manifest, so several functions
    # in the same file compose instead of clobbering each other.
    originals = {}
    for manifest, fid, _ in pending:
        if manifest not in originals:
            originals[manifest] = read_manifest(manifest)
        write_manifest(manifest,
                       flip_state(read_manifest(manifest), fid, "matching"))

    for manifest, fid, state in pending:
        rel = manifest.relative_to(ROOT).as_posix()
        print(f"{fid}: {state} -> matching in {rel}")
    print(f"\nverifying {len(pending)} promotion(s) in "
          f"{len(originals)} manifest(s)...\n", flush=True)

    if run_verifier(args.version):
        for manifest, fid, _ in pending:
            print(f"Promoted {fid} to `matching`.")
        print(f"\nCommit: {', '.join(sorted(m.relative_to(ROOT).as_posix() for m in originals))}")
        return 0

    for manifest, text in originals.items():
        write_manifest(manifest, text)
    print(f"\nVerification FAILED; rolled back all {len(pending)} promotion(s) "
          f"in {len(originals)} manifest(s).", file=sys.stderr)
    if len(pending) > 1:
        print("Re-run the targets individually (or in halves) to find the "
              "one that does not hold.", file=sys.stderr)
    return 1


if __name__ == "__main__":
    sys.exit(main())
