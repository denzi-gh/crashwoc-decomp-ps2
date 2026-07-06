#!/usr/bin/env python3
"""Build the progress summary and fail on any regression vs the baseline.

Reads the verifier's artifact (build/<version>/verify_results.json -- run
`ninja verify-promoted` first) plus the committed registries and manifests,
computes the canonical summary, and compares it against the committed
baseline progress/summary.json:

  REGRESSIONS (exit 1):
    * fewer `matching` functions than the baseline
    * fewer byte-verified matching bytes
    * fewer `complete` units
    * the loaded image is no longer byte-exact
    * the profiles fingerprint changed while the verify run was not fully
      green (a flag/compiler change must re-prove every promoted function)

  NOT regressions: WIP percentages of any kind -- only verified state can
  gate.

`--write` updates progress/summary.json (the new baseline, committed on
main by the protected workflow when counts change) and emits
build/<version>/changes.json with the per-field delta.

Stdlib only; no toolchain and no game files needed (everything it reads is
committed or a build artifact).
"""
import argparse
import datetime
import json
import subprocess
import sys
import tomllib
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from status import check_version
from declib.tu import load_tu_runs, parse_toml_blocks

# Fields whose change is never meaningful for regression comparison.
VOLATILE = ("commit", "generated")


def fresh_summary(version, verify):
    """The canonical summary dict from committed state + verify results."""
    config = ROOT / "config" / version
    sections = json.loads((config / "sections.json").read_text())["sections"]
    text_size = next(int(s["size"], 0) for s in sections
                     if s["name"] == ".text")
    data_total = sum(int(s["size"], 0) for s in sections
                     if s["name"] in (".data", ".rodata", ".lit4", ".sdata",
                                      ".sbss", ".bss"))
    n_funcs = sum(1 for _ in parse_toml_blocks(
        config / "functions.toml", {"i": r"index = (\d+)"}))
    n_units = sum(1 for _ in parse_toml_blocks(
        config / "units.toml", {"i": r"index = (\d+)"}))

    problems, counts = check_version(version, ROOT)
    if any(problems.values()):
        raise SystemExit("compare_progress: status manifests are invalid; "
                         "fix them first (python tools/status.py)")
    complete = 0
    status_dir = config / "status"
    for m in sorted(status_dir.rglob("*.toml")) if status_dir.is_dir() else []:
        if tomllib.loads(m.read_text()).get("complete"):
            complete += 1

    attributed = sum(
        r["end"] - r["start"]
        for r in tomllib.loads((config / "data_map.toml").read_text())["range"]
        if r["owner"] != "unassigned")

    matching_bytes = sum(
        f.get("size", 0) for f in verify["functions"]
        if f["state"] == "matching" and f["verified"])

    def git_commit():
        try:
            return subprocess.run(
                ["git", "rev-parse", "HEAD"], cwd=ROOT, check=True,
                capture_output=True, text=True).stdout.strip()
        except (OSError, subprocess.CalledProcessError):
            return "unknown"

    return {
        "schema": 1,
        "version": version,
        "commit": git_commit(),
        "generated": datetime.datetime.now(datetime.timezone.utc)
        .strftime("%Y-%m-%dT%H:%M:%SZ"),
        "profiles_fingerprint": verify["profiles_fingerprint"],
        "code": {"total_bytes": text_size,
                 "matching_bytes": matching_bytes},
        "functions": {"total": n_funcs,
                      "matching": counts["matching"],
                      "equivalent": counts["equivalent"],
                      "asm": n_funcs - counts["matching"]
                      - counts["equivalent"]},
        "data": {"total_bytes": data_total,
                 "attributed_bytes": attributed,
                 "exact_source_bytes": 0},
        "units": {"total": n_units,
                  "text_units": len(load_tu_runs()),
                  "complete": complete},
        "vu": {"source_percent": 0.0},
        "dvp": {"source_percent": 0.0},
        "image": {"loaded_exact": bool(verify["image"]["exact"]),
                  "packaged_exact": False,
                  "pure_relink_exact": False},
    }


def find_regressions(old, new, verify_all_green):
    """List of regression strings comparing baseline -> fresh summary."""
    regressions = []

    def dec(path, label):
        o = old
        n = new
        for key in path:
            o, n = o.get(key, 0) if isinstance(o, dict) else 0, n[key]
        if n < o:
            regressions.append(f"{label} decreased: {o} -> {n}")

    dec(("functions", "matching"), "matching functions")
    dec(("code", "matching_bytes"), "verified matching bytes")
    dec(("units", "complete"), "complete units")
    if old.get("image", {}).get("loaded_exact") \
            and not new["image"]["loaded_exact"]:
        regressions.append("loaded image is no longer byte-exact")
    if old.get("profiles_fingerprint") \
            and old["profiles_fingerprint"] != new["profiles_fingerprint"] \
            and not verify_all_green:
        regressions.append(
            "profiles fingerprint changed without a fully green re-verify")
    return regressions


def diff_fields(old, new):
    """Flat {dotted.path: [old, new]} for every changed non-volatile field."""
    changes = {}

    def walk(o, n, prefix):
        for key in n:
            if key in VOLATILE:
                continue
            ov = o.get(key) if isinstance(o, dict) else None
            if isinstance(n[key], dict):
                walk(ov if isinstance(ov, dict) else {}, n[key],
                     f"{prefix}{key}.")
            elif ov != n[key]:
                changes[f"{prefix}{key}"] = [ov, n[key]]
    walk(old, new, "")
    return changes


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--version", default="pal103")
    parser.add_argument("--against", type=Path,
                        default=ROOT / "progress" / "summary.json",
                        help="baseline to compare with (default: the "
                             "committed progress/summary.json)")
    parser.add_argument("--write", action="store_true",
                        help="update the baseline and emit changes.json")
    args = parser.parse_args()

    results_path = ROOT / "build" / args.version / "verify_results.json"
    if not results_path.is_file():
        print("build/…/verify_results.json missing; run "
              "`ninja verify-promoted` first.", file=sys.stderr)
        return 2
    verify = json.loads(results_path.read_text())
    if verify.get("schema") != 1:
        print("unsupported verify_results schema", file=sys.stderr)
        return 2

    new = fresh_summary(args.version, verify)
    old = json.loads(args.against.read_text()) if args.against.is_file() else {}

    if old:
        regressions = find_regressions(old, new, verify.get("all_green"))
        changes = diff_fields(old, new)
    else:
        regressions, changes = [], diff_fields({}, new)
        print(f"no baseline at {args.against}; treating everything as new.")

    for path, (ov, nv) in sorted(changes.items()):
        print(f"  {path}: {ov} -> {nv}")
    if not changes:
        print("  no changes vs baseline.")

    if args.write:
        args.against.parent.mkdir(parents=True, exist_ok=True)
        args.against.write_text(json.dumps(new, indent=2) + "\n")
        changes_out = ROOT / "build" / args.version / "changes.json"
        changes_out.write_text(json.dumps(changes, indent=2) + "\n")
        print(f"wrote {args.against.relative_to(ROOT).as_posix()} and "
              f"{changes_out.relative_to(ROOT).as_posix()}")

    if regressions:
        print("\nPROGRESS REGRESSION:")
        for r in regressions:
            print(f"  - {r}")
        return 1
    print("\nNo regressions.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
