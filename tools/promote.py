#!/usr/bin/env python3
"""Promote a function to `matching` -- the only writer of that state.

    python tools/promote.py NuListGetHead
    python tools/promote.py pal103:unit-0007:00105a50:NuListGetHead
    python tools/promote.py --init src/nucore/nulist.c

A hand-edited `state = "matching"` is never trusted (and the container CI
re-verifies every claim); this tool makes the honest path the easy path:

  1. Find the function's manifest entry (by full id, or by name when the
     name is unique across manifests) and flip its state in the file.
  2. Run the full promotion verifier (tools/verify_promoted.py): manifest
     consistency, the function's bytes against retail over its whole
     registry extent, every previously promoted function again, and the
     matching-image SHA gate.
  3. If ANYTHING fails, the manifest edit is rolled back exactly and the
     failure is reported -- the repo never holds an unverified promotion.

`--init` creates the status manifest for a source file that doesn't have
one yet (every function `asm`), deriving the unit from the canonical
source-path mapping (unit `nucore/nulist` <-> src/nucore/nulist.c). Edit
nothing by hand afterwards except `equivalent` states, which are a
reviewed human judgement.

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
    """(manifest_path, full_id) for a function id or unique name."""
    status_dir = ROOT / "config" / version / "status"
    hits = []
    for manifest in sorted(status_dir.rglob("*.toml")) \
            if status_dir.is_dir() else []:
        for entry in tomllib.loads(manifest.read_text()).get("function", []):
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
    """The exact all-asm status manifest body for a unit.

    Shared by `--init` and tools/gen_source_skeletons.py so both emit
    byte-identical manifests; keep any change here in lockstep with the
    committed skeletons (regenerate + `gen_source_skeletons.py --check`).
    """
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


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("target", nargs="?",
                        help="function id or unique function name")
    parser.add_argument("--to", default="matching", choices=("matching",),
                        help="target state (only `matching`; `equivalent` is "
                             "a reviewed hand edit)")
    parser.add_argument("--init", metavar="SRC",
                        help="create an all-asm manifest for src/<unit>.c "
                             "instead of promoting")
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
    if not args.target:
        parser.error("a function id/name is required (or --init SRC)")

    manifest, fid, state = find_entry(args.version, args.target)
    rel = manifest.relative_to(ROOT).as_posix()
    if state == "matching":
        print(f"{fid} is already `matching` in {rel}; nothing to do.")
        return 0

    # newline="" everywhere: the edit must be byte-preserving outside the
    # one flipped word, whatever line endings the file uses and whatever
    # platform this runs on.
    with open(manifest, encoding="utf-8", newline="") as f:
        original = f.read()
    with open(manifest, "w", encoding="utf-8", newline="") as f:
        f.write(flip_state(original, fid, "matching"))
    print(f"{fid}: {state} -> matching in {rel}; verifying...\n", flush=True)

    if run_verifier(args.version):
        print(f"\nPromoted {fid} to `matching`. Commit {rel}.")
        return 0

    with open(manifest, "w", encoding="utf-8", newline="") as f:
        f.write(original)
    print(f"\nVerification FAILED; rolled back {rel} (still `{state}`).",
          file=sys.stderr)
    return 1


if __name__ == "__main__":
    sys.exit(main())
