#!/usr/bin/env python3
"""Compile the hand-written C, verify each function against the retail bytes,
emit objdiff.json, and report matching progress.

This is the first tool in the matching loop proper. For every `.c` under src/ it:

  1. Compiles it with the locked EE GCC (`-O2 -G8 -fomit-frame-pointer`) into a
     "base" object -- the candidate.
  2. Builds the matching "expected" object by assembling that translation unit's
     retail disassembly (the same per-TU slice the PR 7 split produces), so the
     two objects can be diffed symbol-for-symbol.
  3. Links the base object at the functions' true addresses (resolving every
     external symbol exactly as the reconstruction does) and compares each
     function to the retail bytes. A function matches when its bytes are
     identical.
  4. Writes objdiff.json so the same target/base object pairs can be opened in
     objdiff, and prints a progress report.

Needs the EE GCC and the PS2 binutils together, so it runs in the Containerfile
image (trixie glibc for the binutils, i386 multilib for the 32-bit compiler).
The hand-written C in src/ is the committed work product; everything this tool
generates is game-derived and gitignored: base objects in build/src/, expected
objects in expected/, and objdiff.json.

The pipeline logic lives in tools/declib/ (verify, tu, asmtext, target,
toolchain); this file is the CLI.

Exit status is 0 only if every function present in src/ matches.
"""
import argparse
import json
import re
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from cc import compile_c
from declib.toolchain import AS, LD, NM, EEGCC, Reporter, h, tool_path
from declib.asmtext import disambiguate, load_symbol_addrs, resolve
from declib.target import load_target
from declib.tu import (drop_alignment, load_tu_runs, parse_toml_blocks,
                       prologue_end, split_body)
from declib.verify import (defined_function_offsets, link_text_at,
                           undefined_externals)

LABEL_WIDTH = 30


def load_program(version):
    """Return program facts: funcs {name:(addr,size,unit)}, units, elf, delta."""
    elf, sections_spec, pt_load = load_target(version)
    delta = h(pt_load["vaddr"]) - h(pt_load["offset"])
    text = next(s for s in sections_spec["sections"] if s["name"] == ".text")
    text_addr, text_size = h(text["addr"]), h(text["size"])

    rows = [(r["n"], int(r["a"], 16), int(r["u"]))
            for r in parse_toml_blocks(
                ROOT / "config" / version / "functions.toml",
                {"n": r"name = '([^']*)'", "a": r"address = (0x[0-9A-Fa-f]+)",
                 "u": r"unit = (\d+)"})]
    rows.sort(key=lambda r: r[1])
    funcs = {}
    for i, (name, addr, unit) in enumerate(rows):
        end = rows[i + 1][1] if i + 1 < len(rows) else text_addr + text_size
        funcs[name] = (addr, end - addr, unit)

    units = {int(r["i"]): r["n"] for r in parse_toml_blocks(
        ROOT / "config" / version / "units.toml",
        {"i": r"index = (\d+)", "n": r"name = '([^']*)'"})}
    # len(rows), not len(funcs): a few statics share a name across units.
    return funcs, units, elf, delta, text_addr, text_size, len(rows)


def unit_slice(reporter, version):
    """Return (prologue, {unit_index: [asm lines]}) from the retail .text split.

    Reuses the PR 7 pipeline (disambiguate duplicate statics, drop the no-op
    `.align` directives, partition at the mdebug unit boundaries) so an expected
    object can be assembled for any single unit without rebuilding all of them.
    """
    mono, ok = disambiguate((ROOT / "asm" / "text.s").read_text(), reporter)
    if not ok:
        return None, None
    lines = drop_alignment(mono.splitlines(), reporter)
    if lines is None:
        return None, None
    runs = load_tu_runs()
    body_start = prologue_end(lines)
    chunks = split_body(lines, body_start, [a for _u, a in runs[1:]])
    return lines[:body_start], {runs[i][0]: chunks[i] for i in range(len(runs))}


def verify_unit(reporter, src, funcs, elf, delta, prologue, chunk, unit_name,
                nm_bin, as_bin):
    """Compile src, build its expected object, compare each function. Return a
    dict of results and the (target_path, base_path) for objdiff.json."""
    rel = src.relative_to(ROOT / "src").with_suffix("")
    base_o = ROOT / "build" / "src" / rel.with_suffix(".o")
    expected_o = ROOT / "expected" / rel.with_suffix(".o")
    compile_c(src, base_o)

    # Expected object: assemble this unit's retail disassembly slice.
    expected_o.parent.mkdir(parents=True, exist_ok=True)
    exp_s = expected_o.with_suffix(".s")
    exp_s.write_text("\n".join(prologue + [""] + chunk) + "\n")
    subprocess.run([as_bin, "-EL", "-march=r5900",
                    "-I", str(ROOT / "build" / "include"),
                    "-o", str(expected_o), str(exp_s)],
                   check=True, capture_output=True, text=True)

    offsets = defined_function_offsets(nm_bin, base_o)
    names = [n for n in offsets if n in funcs]
    names.sort(key=lambda n: funcs[n][0])
    if not names:
        reporter.result(f"{rel.as_posix()}", False, "no known target functions")
        return {"funcs": [], "matched": 0}, (expected_o, base_o)

    base_addr = min(funcs[n][0] for n in names)
    undef = undefined_externals(nm_bin, base_o)
    defsyms, unresolved = resolve(sorted(undef - {"_gp"}),
                                  load_symbol_addrs("pal103"))
    results = []
    with tempfile.TemporaryDirectory() as tmp:
        tmp = Path(tmp)
        if unresolved:
            reporter.result(f"{rel.as_posix()}", False,
                            f"unresolved externals: {unresolved[:4]}")
            return {"funcs": [], "matched": 0}, (expected_o, base_o)
        linked = link_text_at(base_o, base_addr, defsyms, tmp)
        for n in names:
            addr, size, _u = funcs[n]
            # Locate the function by its own symbol offset in the object, so
            # source order and address gaps between C functions never matter.
            got = linked[offsets[n]: offsets[n] + size]
            want = elf[addr - delta: addr - delta + size]
            ok = got == want
            results.append({"name": n, "addr": addr, "size": size, "match": ok})

    matched = sum(r["match"] for r in results)
    for r in results:
        mark = "OK  " if r["match"] else "DIFF"
        print(f"    {mark} {r['name']:<28} 0x{r['addr']:08X}  {r['size']:>4} bytes")
    reporter.result(f"{rel.as_posix()}", matched == len(results),
                    None if matched == len(results)
                    else f"{len(results) - matched} of {len(results)} differ")
    return {"funcs": results, "matched": matched}, (expected_o, base_o)


def write_objdiff(units):
    """Emit objdiff.json pairing each unit's expected (target) and base object."""
    data = {
        "min_version": "2.0.0",
        "units": [
            {"name": name,
             "target_path": str(t.relative_to(ROOT).as_posix()),
             "base_path": str(b.relative_to(ROOT).as_posix()),
             "metadata": {"source_path": f"src/{name}.c"}}
            for name, t, b in units],
    }
    (ROOT / "objdiff.json").write_text(json.dumps(data, indent=2) + "\n")


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--version", default="pal103")
    args = parser.parse_args()

    if not EEGCC.is_file() or not tool_path(LD):
        print("EE GCC or PS2 binutils not found; run in the Containerfile image "
              "(setup_toolchain.py installs them).", file=sys.stderr)
        return 2
    src_files = sorted((ROOT / "src").rglob("*.c"))
    if not src_files:
        print("no src/**/*.c yet.", file=sys.stderr)
        return 2

    funcs, units, elf, delta, _ta, text_size, total_funcs = load_program(args.version)
    reporter = Reporter()
    prologue, chunks = unit_slice(reporter, args.version)
    if chunks is None:
        return 1

    nm_bin, as_bin = tool_path(NM), tool_path(AS)
    print(f"\nMatching progress: {args.version}\n")

    objdiff_units, total_matched, total_bytes = [], 0, 0
    prog_funcs, prog_matched_funcs = 0, 0
    for src in src_files:
        rel = src.relative_to(ROOT / "src").with_suffix("").as_posix()
        print(f"  {rel}")
        # Every function in a src file is expected to belong to one unit.
        unit_idx = _unit_of(src, funcs, units)
        chunk = chunks.get(unit_idx, [])
        res, (tpath, bpath) = verify_unit(
            reporter, src, funcs, elf, delta, prologue, chunk, rel, nm_bin, as_bin)
        objdiff_units.append((rel, tpath, bpath))
        for r in res["funcs"]:
            prog_funcs += 1
            if r["match"]:
                prog_matched_funcs += 1
                total_bytes += r["size"]
        total_matched += res["matched"]

    write_objdiff(objdiff_units)

    print(f"\n{'Functions matched:':<{LABEL_WIDTH}} {prog_matched_funcs}/{prog_funcs}")
    print(f"{'Bytes matched:':<{LABEL_WIDTH}} 0x{total_bytes:X} of 0x{text_size:X} "
          f".text ({100.0 * total_bytes / text_size:.4f}%)")
    print(f"{'Total functions in program:':<{LABEL_WIDTH}} {total_funcs}")
    print("objdiff.json written.")

    if reporter.failed:
        print("\nSome functions do not match:")
        for d in reporter.details:
            print(f"  - {d}")
        return 1
    return 0


def _unit_of(src, funcs, units):
    """The single mdebug unit a src file's functions belong to (best-effort)."""
    stem = src.stem
    for idx, name in units.items():
        if re.split(r"[\\/]", name)[-1] == f"{stem}.c":
            return idx
    return -1


if __name__ == "__main__":
    sys.exit(main())
