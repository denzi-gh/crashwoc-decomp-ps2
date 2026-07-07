#!/usr/bin/env python3
"""Contributor front door: the only commands a contributor needs.

    python decomp.py toolchain          # make the dev toolchain ready (once)
    python decomp.py setup              # prepare the workspace for objdiff
    python decomp.py src/game/creature.c   # inspect a unit (auto-promotes 100%)

Every command is a thin, reliable wrapper over the existing tools -- it never
reimplements their logic. The build graph, objdiff report, matching verifier,
sanitizer and CI gates remain the source of truth:

  * `toolchain` ensures the container image, starts the dev container, installs
    the locked toolchain (tools/setup_toolchain.py) and verifies every locked
    component's fingerprint (tools/fingerprint_compiler.py --all). Idempotent.
  * `setup` runs the honest local pipeline once: target verify, strict split +
    graph, the object sets objdiff scores, canonical matching verification, and
    the objdiff report -- then tells you objdiff is ready to open.
  * `<unit>.c` rebuilds just that unit through the real objdiff-cli path,
    auto-promotes any function that reaches 100% via tools/promote.py (which
    byte-verifies and rolls back on failure -- an objdiff % alone is never
    treated as matching truth), and prints a per-function table merging the
    objdiff report, the status manifest, and the canonical verify results.

Container commands go through tools/dispatch.py, so this works from the Windows
host exactly as it does inside the container.
"""
import argparse
import subprocess
import sys
import tomllib
from pathlib import Path

ROOT = Path(__file__).resolve().parent
sys.path.insert(0, str(ROOT / "tools"))
from gen_objdiff import unit_table

PY = sys.executable
PUBLIC_IMAGE = "ghcr.io/denzi-gh/crashwoc-decomp"


class DecompError(Exception):
    """A user-facing error with a clear, actionable message."""


# --------------------------------------------------------------------------
# Command construction (pure -- unit tested without executing anything).
# --------------------------------------------------------------------------
def dispatch_cmd(*args):
    """A command routed through tools/dispatch.py into the dev container."""
    return [PY, "tools/dispatch.py", *args]


def setup_commands():
    """The ordered pipeline `setup` runs. All container-routed so a contributor
    needs only the toolchain image, not a native splat/toolchain install."""
    return [
        dispatch_cmd("python", "tools/verify_target.py"),
        dispatch_cmd("python", "configure.py", "--strict"),
        dispatch_cmd("ninja", "expected", "current", "report-current", "data"),
        dispatch_cmd("ninja", "verify-promoted"),
        dispatch_cmd("ninja", "report"),
        dispatch_cmd("python", "tools/check_report_matches.py"),
    ]


# --------------------------------------------------------------------------
# Unit-path resolution.
# --------------------------------------------------------------------------
def resolve_unit(arg, version):
    """(name, src_rel, manifest_rel) for a `src/<group>/<unit>.c` argument.

    Raises DecompError with an actionable message for anything that is not a
    known, manifested source unit.
    """
    p = Path(arg)
    try:
        rel = (p if p.is_absolute() else ROOT / p).resolve().relative_to(ROOT)
    except ValueError:
        raise DecompError(f"{arg}: not inside the repository")
    rel_posix = rel.as_posix()
    if not rel_posix.startswith("src/") or not rel_posix.endswith(".c"):
        raise DecompError(
            f"{arg}: expected a path like src/<group>/<unit>.c")
    name = rel_posix[len("src/"):-len(".c")]
    if name not in {row[2] for row in unit_table(version)}:
        raise DecompError(
            f"{arg}: '{name}' is not a known .text unit "
            f"(see objdiff.json for the canonical unit names)")
    if not (ROOT / rel_posix).is_file():
        raise DecompError(f"{arg}: file does not exist")
    manifest_rel = f"config/{version}/status/{name}.toml"
    if not (ROOT / manifest_rel).is_file():
        raise DecompError(
            f"{arg}: no status manifest at {manifest_rel}. Create one with\n"
            f"    python tools/promote.py --init {rel_posix}")
    return name, rel_posix, manifest_rel


# --------------------------------------------------------------------------
# Report / manifest / verify merge (pure).
# --------------------------------------------------------------------------
def _as_float(value):
    """objdiff encodes some numbers as strings; coerce, default 0.0."""
    try:
        return float(value)
    except (TypeError, ValueError):
        return 0.0


def report_unit(report, name):
    """({func_name: fuzzy_pct}, unit_measures) for a unit, or ({}, {})."""
    for unit in report.get("units", []):
        if unit.get("name") == name:
            pcts = {f["name"]: _as_float(f.get("fuzzy_match_percent"))
                    for f in unit.get("functions", [])}
            return pcts, unit.get("measures", {})
    return {}, {}


def manifest_functions(manifest_path):
    """[(full_id, bare_name, state)] in manifest (address) order, plus profile.

    Returns (rows, profile).
    """
    data = tomllib.loads(Path(manifest_path).read_text())
    rows = []
    for f in data.get("function", []):
        fid = f["id"]
        rows.append((fid, fid.rsplit(":", 1)[1], f.get("state", "asm")))
    return rows, data.get("profile", "default")


def verified_ids(verify):
    """Full ids of functions canonically verified as matching."""
    return {f["id"] for f in verify.get("functions", [])
            if f.get("state") == "matching" and f.get("verified")}


def build_rows(manifest_rows, pcts, verified):
    """[(bare_name, pct, state, is_verified)] for display, manifest order."""
    return [(name, pcts.get(name, 0.0), state, fid in verified)
            for fid, name, state in manifest_rows]


def promote_candidates(manifest_rows, pcts):
    """Full ids of functions objdiff scores at 100% but still marked `asm`."""
    return [fid for fid, name, state in manifest_rows
            if state == "asm" and pcts.get(name, 0.0) >= 100.0]


def format_table(name, profile, rows, unit_measures):
    """The per-unit report table (matches the documented shape)."""
    nw = max([len("Function")] + [len(r[0]) for r in rows]) + 2
    out = [f"Unit: {name}", f"Compiler: {profile}", ""]
    header = f"{'Function'.ljust(nw)}{'Match'.ljust(11)}" \
             f"{'State'.ljust(14)}Verified"
    out.append(header)
    out.append("-" * max(len(header), 64))
    for bare, pct, state, is_verified in rows:
        out.append(f"{bare.ljust(nw)}{(f'{pct:.2f}%').ljust(11)}"
                   f"{state.ljust(14)}{'yes' if is_verified else 'no'}")
    verified_count = sum(1 for r in rows if r[3])
    fuzzy = _as_float(unit_measures.get("fuzzy_match_percent"))
    out += ["",
            f"Unit fuzzy progress:  {fuzzy:8.2f}%",
            f"Verified matching:    {verified_count:5d} / {len(rows)}"]
    return "\n".join(out)


# --------------------------------------------------------------------------
# Execution helpers (thin subprocess wrappers; overridable for tests).
# --------------------------------------------------------------------------
def run(cmd):
    """Run a command, streaming its output; return the exit code."""
    print("+ " + " ".join(str(c) for c in cmd), flush=True)
    return subprocess.run(cmd, cwd=ROOT).returncode


def probe(cmd):
    """Run a command silently; return the exit code."""
    return subprocess.run(cmd, cwd=ROOT,
                          capture_output=True, text=True).returncode


def _load_json(rel):
    import json
    path = ROOT / rel
    if not path.is_file():
        return {}
    return json.loads(path.read_text())


def _regen_graph(version):
    """Regenerate build.ninja + objdiff.json natively (pure Python)."""
    from gen_ninja import emit_ninja
    from gen_objdiff import emit_objdiff
    emit_ninja(version)
    emit_objdiff(version)


def _graph_stale(version, src_rel, manifest_rel):
    ninja = ROOT / "build.ninja"
    if not ninja.is_file() or not (ROOT / "objdiff.json").is_file():
        return True
    cutoff = ninja.stat().st_mtime
    for rel in (src_rel, manifest_rel):
        path = ROOT / rel
        if path.is_file() and path.stat().st_mtime > cutoff:
            return True
    return False


# --------------------------------------------------------------------------
# Commands.
# --------------------------------------------------------------------------
def _ensure_image(eng, image):
    """Make the local dev image available: pull the public one and tag it, or
    fall back to a local Containerfile build. Skips when already present."""
    if probe([eng, "image", "inspect", image]) == 0:
        print(f"image {image} already present.")
        return True
    print(f"pulling public dev image {PUBLIC_IMAGE} ...")
    if run([eng, "pull", PUBLIC_IMAGE]) == 0:
        return run([eng, "tag", PUBLIC_IMAGE, image]) == 0
    print("public image unavailable; building locally from Containerfile ...")
    return run([eng, "build", "-f", "Containerfile", "-t", image, "."]) == 0


def cmd_toolchain(version):
    import dev_container
    try:
        eng = dev_container.engine()
    except dev_container.ContainerError as exc:
        print(f"decomp toolchain: {exc}", file=sys.stderr)
        return 1
    if not _ensure_image(eng, dev_container.IMAGE):
        print("decomp toolchain: could not obtain the dev image.",
              file=sys.stderr)
        return 1
    try:
        dev_container.ensure(eng)
    except dev_container.ContainerError as exc:
        print(f"decomp toolchain: {exc}", file=sys.stderr)
        return 1
    if run(dispatch_cmd("python", "tools/setup_toolchain.py", "--download")):
        print("decomp toolchain: toolchain install FAILED (see above). If a "
              "locked hash is null, the maintainer must record it first.",
              file=sys.stderr)
        return 1
    if run(dispatch_cmd("python", "tools/fingerprint_compiler.py", "--all")):
        print("decomp toolchain: fingerprint verification FAILED.",
              file=sys.stderr)
        return 1
    print("\ntoolchain ready. Next: python decomp.py setup")
    return 0


def cmd_setup(version):
    for cmd in setup_commands():
        if run(cmd):
            print(f"decomp setup: step failed: {' '.join(cmd)}",
                  file=sys.stderr)
            return 1
    print("\nworkspace ready -- open the repo in objdiff (GUI) to start, or\n"
          "    python decomp.py src/<group>/<unit>.c")
    return 0


def cmd_unit(arg, version, *, promote=True):
    try:
        name, src_rel, manifest_rel = resolve_unit(arg, version)
    except DecompError as exc:
        print(f"decomp: {exc}", file=sys.stderr)
        return 1

    if _graph_stale(version, src_rel, manifest_rel):
        _regen_graph(version)
    if run(dispatch_cmd("ninja", "report")):
        print("decomp: building the report failed (see above).",
              file=sys.stderr)
        return 1

    manifest_rows, profile = manifest_functions(ROOT / manifest_rel)
    pcts, measures = report_unit(_load_json(f"build/{version}/report.json"),
                                 name)

    if promote:
        promoted = False
        for fid in promote_candidates(manifest_rows, pcts):
            print(f"\n{fid} reads 100% but is `asm`; verifying + promoting ...")
            if run(dispatch_cmd("python", "tools/promote.py", fid)) == 0:
                promoted = True
        if promoted:
            # A state change moves the unit's base object; rebuild the report.
            _regen_graph(version)
            run(dispatch_cmd("ninja", "report"))
            manifest_rows, profile = manifest_functions(ROOT / manifest_rel)
            pcts, measures = report_unit(
                _load_json(f"build/{version}/report.json"), name)

    verified = verified_ids(_load_json(f"build/{version}/verify_results.json"))
    rows = build_rows(manifest_rows, pcts, verified)
    print("\n" + format_table(name, profile, rows, measures))
    return 0


def main(argv=None):
    parser = argparse.ArgumentParser(
        description=__doc__.splitlines()[0],
        formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("command",
                        help="`toolchain`, `setup`, or a src/<unit>.c path")
    parser.add_argument("--version", default="pal103")
    parser.add_argument("--no-promote", action="store_true",
                        help="inspect only; do not auto-promote 100% functions")
    args = parser.parse_args(argv)

    if args.command == "toolchain":
        return cmd_toolchain(args.version)
    if args.command == "setup":
        return cmd_setup(args.version)
    if args.command.endswith(".c"):
        return cmd_unit(args.command, args.version,
                        promote=not args.no_promote)
    parser.error(f"unknown command {args.command!r} "
                 f"(expected `toolchain`, `setup`, or a src/<unit>.c path)")


if __name__ == "__main__":
    sys.exit(main())
