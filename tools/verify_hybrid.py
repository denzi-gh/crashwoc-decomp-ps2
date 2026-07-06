#!/usr/bin/env python3
"""Verify hybrid translation units: whole-unit bytes and current-object purity.

For every unit with a status manifest this tool proves the two properties the
hybrid object model rests on:

  * The matching hybrid is byte-exact for the WHOLE unit. tools/gen_hybrid.py
    splices the clean C (functions the manifest marks `matching`) with the
    retail slices for everything else, in address order; the object's `.text`
    linked at the unit's retail address must equal the retail bytes across
    the entire unit range -- every function, every C/asm seam. This single
    comparison proves selection, order, alignment, and the splice at once;
    wrong bytes from a mismarked function fail it precisely.

  * The current object carries no fallback bytes. The plain compile of the
    source -- which is what objdiff scores -- must define exactly the
    registry functions the manifest declares `matching` or `equivalent`;
    spliced retail assembly can never appear there because the source never
    contains any.

Needs the EE GCC and PS2 binutils: run in the Containerfile image. Outputs
land in gitignored build/<version>/{matching,current}/.

Exit status is 0 only if every manifested unit passes both checks.
"""
import argparse
import sys
import tempfile
import tomllib
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from cc import compile_c
from gen_hybrid import HybridError, build_hybrid
from declib.asmtext import (load_symbol_addrs, load_text_target, resolve)
from declib.toolchain import LD, NM, Reporter, tool_path
from declib.tu import load_tu_runs, parse_toml_blocks
from declib.verify import defined_functions, link_text_at, undefined_externals

LABEL_WIDTH = 40


def registry_names(version):
    """Every function name the mdebug registry knows, program-wide."""
    return {r["name"] for r in parse_toml_blocks(
        ROOT / "config" / version / "functions.toml",
        {"name": r"name = '([^']*)'"})}


def unit_range(unit_index, text_addr, text_size):
    """[start, end) of the unit's .text from the mdebug run table."""
    runs = load_tu_runs()
    for i, (unit, addr) in enumerate(runs):
        if unit == unit_index:
            end = runs[i + 1][1] if i + 1 < len(runs) else text_addr + text_size
            return addr, end
    return None, None


def check_unit(reporter, manifest, version, elf, text_addr, text_off, text_size,
               nm_bin):
    data = tomllib.loads(manifest.read_text())
    src = ROOT / data["source"]
    rel = Path(data["source"]).relative_to("src").with_suffix("")
    unit_index = int(data["unit"].split("unit-")[1])
    label = rel.as_posix()

    start, end = unit_range(unit_index, text_addr, text_size)
    if start is None:
        reporter.result(f"{label} (unit range)", False,
                        f"unit {unit_index} owns no .text")
        return
    size = end - start
    want = elf[text_off + (start - text_addr): text_off + (end - text_addr)]

    # Whole-unit byte identity of the matching hybrid.
    hybrid_o = ROOT / "build" / version / "matching" / rel.with_suffix(".o")
    try:
        build_hybrid(manifest, hybrid_o, link_set="matching", version=version)
    except HybridError as exc:
        reporter.result(f"{label} (hybrid bytes)", False, str(exc))
        return
    undef = undefined_externals(nm_bin, hybrid_o)
    defsyms, unresolved = resolve(sorted(undef - {"_gp"}),
                                  load_symbol_addrs(version))
    if unresolved:
        reporter.result(f"{label} (hybrid bytes)", False,
                        f"unresolved externals: {unresolved[:4]}")
        return
    with tempfile.TemporaryDirectory() as tmp:
        linked = link_text_at(hybrid_o, start, defsyms, Path(tmp))
    ok = linked == want
    detail = None
    if not ok:
        limit = min(len(linked), len(want))
        first = next((i for i in range(limit) if linked[i] != want[i]), limit)
        detail = (f"unit 0x{start:08X}..0x{end:08X}: linked 0x{len(linked):X} "
                  f"bytes vs 0x{len(want):X}; first diff at +0x{first:X} "
                  f"(vram 0x{start + first:08X})")
    reporter.result(f"{label} (hybrid bytes, 0x{size:X})", ok, detail)

    # Current-object consistency: every declared matching/equivalent function
    # must exist in the plain compile (the object objdiff scores). C defined
    # beyond the declarations is fine -- that is work in progress, visible in
    # objdiff as a fuzzy score while the manifest still says `asm` and the
    # matching build still splices the retail slice. Compiler markers
    # (gcc2_compiled. etc.) and contributor-local helpers are not registry
    # names and are ignored.
    declared = {e["id"].rsplit(":", 1)[1] for e in data.get("function", [])
                if e["state"] in ("matching", "equivalent")}
    current_o = ROOT / "build" / version / "current" / rel.with_suffix(".o")
    compile_c(src, current_o, profile=data.get("profile", "default"),
              version=version)
    defined = set(defined_functions(nm_bin, current_o)) & registry_names(version)
    missing = declared - defined
    wip = sorted(defined - declared)
    reporter.result(f"{label} (current consistency)", not missing,
                    None if not missing
                    else f"declared but not defined in C: {sorted(missing)}")
    if wip:
        print(f"    note: work in progress (defined in C, still `asm` in the "
              f"manifest): {', '.join(wip)}")


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--version", default="pal103")
    args = parser.parse_args()

    nm_bin = tool_path(NM)
    if not nm_bin or not tool_path(LD):
        print("PS2 binutils not found; run in the Containerfile image.",
              file=sys.stderr)
        return 2

    status_dir = ROOT / "config" / args.version / "status"
    manifests = sorted(status_dir.rglob("*.toml")) if status_dir.is_dir() else []
    if not manifests:
        print("no status manifests; nothing to verify.", file=sys.stderr)
        return 2

    elf, text_addr, text_off, text_size = load_text_target(args.version)
    print(f"Hybrid verification: {args.version}, {len(manifests)} unit(s)\n")
    reporter = Reporter(LABEL_WIDTH)
    for manifest in manifests:
        check_unit(reporter, manifest, args.version, elf, text_addr, text_off,
                   text_size, nm_bin)

    print()
    if reporter.failed:
        print("Hybrid verification FAILED:")
        for detail in reporter.details:
            print(f"  - {detail}")
        return 1
    print("Hybrid verification OK.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
