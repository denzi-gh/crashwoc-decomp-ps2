#!/usr/bin/env python3
"""Write the per-TU expected assembly files the ninja build assembles.

Same split as tools/split_text.py (disambiguate duplicate statics, drop the
proven no-op `.align` directives, cut at the mdebug unit boundaries), but as a
single-purpose build step: it only writes the 247 per-TU `.s` files, into

    build/<version>/expected_s/NNN_<name>.s

so ninja can assemble each one independently into its expected object
(expected/<version>/NNN_<name>.o). split_text.py remains the full proof CLI
(lossless-partition check + whole-image relink); this tool is its fast
build-graph twin and must stay content-identical to it.

Files are only rewritten when their content changed, and the ninja edge that
runs this tool uses `restat = 1`: retouching asm/text.s re-runs the split, but
unchanged TUs do not cascade into 247 reassemblies.

Nothing here is committed: the output is game-derived and lands in the
gitignored build/ tree. Pure Python; no toolchain needed.

Exit status is 0 only if the split succeeded and covers .text losslessly.
"""
import argparse
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from declib.asmtext import disambiguate
from declib.toolchain import Reporter
from declib.tu import (drop_alignment, load_tu_runs, load_units, prologue_end,
                       split_body)


def unit_stem(unit, unit_names):
    """The stable file stem for one TU (split_text.py's convention)."""
    return f"{unit:03d}_{unit_names.get(unit, 'unit')}"


def expected_s_dir(version):
    return ROOT / "build" / version / "expected_s"


def write_if_changed(path, text):
    """Write only when content differs; returns True if the file was touched."""
    if path.is_file() and path.read_text() == text:
        return False
    path.write_text(text)
    return True


def generate(version, reporter):
    """Split the monolith and write the per-TU .s files; returns True on
    success."""
    text_s = ROOT / "asm" / "text.s"
    if not text_s.is_file():
        print("asm/text.s missing; run `python configure.py` first "
              "(gitignored, game-derived).", file=sys.stderr)
        return False

    mono, ok = disambiguate(text_s.read_text(), reporter)
    if not ok:
        return False
    lines = drop_alignment(mono.splitlines(), reporter)
    if lines is None:
        return False

    runs = load_tu_runs()
    body_start = prologue_end(lines)
    prologue = lines[:body_start]
    chunks = split_body(lines, body_start, [a for _u, a in runs[1:]])
    if len(chunks) != len(runs):
        reporter.result("Split covers .text", False,
                        f"{len(chunks)} chunks for {len(runs)} TUs")
        return False
    joined = [l for chunk in chunks for l in chunk]
    if joined != lines[body_start:]:
        reporter.result("Split covers .text", False, "partition not lossless")
        return False
    reporter.result("Split covers .text", True)

    out_dir = expected_s_dir(version)
    out_dir.mkdir(parents=True, exist_ok=True)
    unit_names = load_units()
    changed = 0
    for chunk, (unit, _addr) in zip(chunks, runs):
        path = out_dir / f"{unit_stem(unit, unit_names)}.s"
        if write_if_changed(path, "\n".join(prologue + [""] + chunk) + "\n"):
            changed += 1
    print(f"  {len(runs)} TU files in {out_dir.relative_to(ROOT).as_posix()}/ "
          f"({changed} rewritten, gitignored)")
    return True


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--version", default="pal103")
    args = parser.parse_args()

    reporter = Reporter()
    ok = generate(args.version, reporter)
    print()
    if not ok or reporter.failed:
        print("Expected-assembly generation FAILED:")
        for detail in reporter.details:
            print(f"  - {detail}")
        return 1
    print("Expected-assembly generation OK.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
