#!/usr/bin/env python3
"""Fail if any verified `matching` function is under 100% in the objdiff report.

The canonical matching truth is verify_results.json: byte-identity of each
`matching` function against retail over its full registry extent. This gate
asserts the PUBLIC objdiff report agrees with it -- every function the manifest
calls `matching` and verify_promoted.py confirmed must score 100% in
build/<v>/report.json, and the report must count at least as many verified
matching functions as verify_results.json does. It runs before the report
artifact is staged, so a report base that silently dropped or mis-scored a
verified function fails CI.

Pure Python; reads only the two generated JSON files (no toolchain, no game
files). The report base objects are built by `ninja report-current`.
"""
import argparse
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from gen_objdiff import unit_table


def verified_matching(verify):
    """[(unit_index, function_name)] for every confirmed `matching` function."""
    out = []
    for f in verify.get("functions", []):
        if f.get("state") == "matching" and f.get("verified"):
            _v, unit, _addr, name = f["id"].split(":")
            out.append((int(unit.split("-")[1]), name))
    return out


def check(verify, report, unit_name):
    """(matched_count, problems) -- problems is empty iff every verified
    matching function reads 100% in the report.

    unit_name maps a unit index to its objdiff unit name (from unit_table);
    passing it in keeps this function pure and testable.
    """
    units_by_name = {u["name"]: u for u in report.get("units", [])}
    problems = []
    matched = 0
    for unit_index, fname in verified_matching(verify):
        uname = unit_name.get(unit_index)
        runit = units_by_name.get(uname)
        if runit is None:
            problems.append(f"{fname}: unit {uname!r} (index {unit_index}) "
                            f"absent from the report")
            continue
        fn = next((f for f in runit.get("functions", [])
                   if f.get("name") == fname), None)
        if fn is None:
            problems.append(f"{fname}: not present in report unit {uname!r}")
            continue
        pct = fn.get("fuzzy_match_percent")
        if pct is None or pct < 100.0:
            problems.append(f"{fname} ({uname}): report shows {pct}, not 100")
        else:
            matched += 1
    return matched, problems


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--version", default="pal103")
    parser.add_argument("--verify",
                        help="default: build/<version>/verify_results.json")
    parser.add_argument("--report",
                        help="default: build/<version>/report.json")
    args = parser.parse_args()

    verify_path = Path(args.verify) if args.verify else \
        ROOT / "build" / args.version / "verify_results.json"
    report_path = Path(args.report) if args.report else \
        ROOT / "build" / args.version / "report.json"
    for p in (verify_path, report_path):
        if not p.is_file():
            print(f"{p} missing; run the build first.", file=sys.stderr)
            return 2

    verify = json.loads(verify_path.read_text())
    report = json.loads(report_path.read_text())
    unit_name = {idx: name for idx, _stem, name, _cat
                 in unit_table(args.version)}

    expected = len(verified_matching(verify))
    matched, problems = check(verify, report, unit_name)
    if problems:
        print("report does not honor verified matching functions:",
              file=sys.stderr)
        for p in problems:
            print(f"  - {p}", file=sys.stderr)
        return 1
    if matched < expected:
        print(f"report counts {matched} verified matching functions at 100%, "
              f"expected {expected}", file=sys.stderr)
        return 1
    print(f"report OK: {matched} verified matching function(s) at 100%")
    return 0


if __name__ == "__main__":
    sys.exit(main())
