#!/usr/bin/env python3
"""Final gate on the staged decomp.dev artifact, run just before upload.

build/publish/report.json is the exact file matching.yml uploads as the
SLES_503.86_report artifact. tools/sanitize_report.py has already produced it;
this is a belt-and-braces check on the bytes that actually leave the runner, so
a stale, wrong, or hand-edited staged file cannot slip through. It never
rewrites anything -- it only passes or fails.

Checks (all reuse the sibling tools, so the gate can never drift from them):

  * valid JSON, and a strict SUBSET of objdiff's Report schema -- no stray
    fields at any level (decomp.dev parses it as a protobuf-JSON Report);
  * no orig/ reference, absolute path, or embedded byte blob
    (sanitize_report.violations);
  * no hollow measure survived -- a derived measure whose governing total is
    absent or 0 would claim "100% of nothing";
  * every verified `matching` function reads 100% in the report, agreeing with
    verify_results.json (check_report_matches.check).

The checks are pure functions over the two JSON documents plus the unit-index
map; main() wires the default paths. Stdlib and the sibling tools only -- no
toolchain, no game files.
"""
import argparse
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from sanitize_report import (violations, MEASURE_KEYS, UNIT_METADATA_KEYS,
                             HOLLOW_WHEN_TOTAL_ZERO, _measure_zero_or_absent)
from check_report_matches import check, verified_matching
from gen_objdiff import unit_table

TOP_KEYS = {"version", "measures", "units", "categories"}
UNIT_KEYS = {"name", "measures", "sections", "functions", "metadata"}
SECTION_KEYS = {"name", "size", "fuzzy_match_percent"}
FUNCTION_KEYS = {"name", "size", "address", "fuzzy_match_percent"}
CATEGORY_KEYS = {"id", "name", "measures"}


def _measure_problems(block, where):
    problems = []
    for key in block:
        if key not in MEASURE_KEYS:
            problems.append(f"{where}: unknown measure {key!r}")
    for total, derived in HOLLOW_WHEN_TOTAL_ZERO.items():
        if _measure_zero_or_absent(block, total):
            for key in derived:
                if key in block:
                    problems.append(
                        f"{where}: hollow {key!r} with {total} absent/0")
    return problems


def _subset(d, allowed, where):
    return [f"{where}: unexpected field {key!r}"
            for key in d if key not in allowed]


def schema_problems(report):
    """Every way the staged report strays from the published-subset schema.

    Mirrors what sanitize_report guarantees; if the staged file was produced by
    the sanitizer this returns []. A non-empty result means the staged bytes are
    not what the sanitizer would have written.
    """
    problems = _subset(report, TOP_KEYS, "report")
    problems += _measure_problems(report.get("measures", {}), "report.measures")
    for unit in report.get("units", []):
        where = f"unit {unit.get('name')!r}"
        problems += _subset(unit, UNIT_KEYS, where)
        problems += _measure_problems(unit.get("measures", {}),
                                      f"{where}.measures")
        problems += _subset(unit.get("metadata", {}), UNIT_METADATA_KEYS,
                            f"{where}.metadata")
        for sec in unit.get("sections", []):
            problems += _subset(sec, SECTION_KEYS, f"{where}.section")
        for fn in unit.get("functions", []):
            problems += _subset(fn, FUNCTION_KEYS, f"{where}.function")
    for cat in report.get("categories", []):
        where = f"category {cat.get('id')!r}"
        problems += _subset(cat, CATEGORY_KEYS, where)
        problems += _measure_problems(cat.get("measures", {}),
                                      f"{where}.measures")
    return problems


def smoke(report, verify, unit_name):
    """[problem strings] -- empty iff the staged artifact is publishable."""
    problems = list(violations(report))
    problems += schema_problems(report)
    _matched, match_problems = check(verify, report, unit_name)
    problems += match_problems
    return problems


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("report", nargs="?",
                        help="staged artifact (default: "
                             "build/publish/report.json)")
    parser.add_argument("--version", default="pal103")
    parser.add_argument("--verify",
                        help="default: build/<version>/verify_results.json")
    args = parser.parse_args()

    report_path = Path(args.report) if args.report else \
        ROOT / "build" / "publish" / "report.json"
    verify_path = Path(args.verify) if args.verify else \
        ROOT / "build" / args.version / "verify_results.json"
    for p in (report_path, verify_path):
        if not p.is_file():
            print(f"{p} missing; stage the artifact and run the build first.",
                  file=sys.stderr)
            return 2

    try:
        report = json.loads(report_path.read_text())
    except json.JSONDecodeError as exc:
        print(f"{report_path} is not valid JSON: {exc}", file=sys.stderr)
        return 1
    verify = json.loads(verify_path.read_text())
    unit_name = {idx: name for idx, _stem, name, _cat
                 in unit_table(args.version)}

    problems = smoke(report, verify, unit_name)
    if problems:
        print(f"REFUSING to publish {report_path}:", file=sys.stderr)
        for p in problems:
            print(f"  - {p}", file=sys.stderr)
        return 1

    n_units = len(report.get("units", []))
    n_matching = len(verified_matching(verify))
    print(f"smoke OK: {report_path} publishable "
          f"({n_units} units, {n_matching} verified matching at 100%)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
