#!/usr/bin/env python3
"""Cut the monolithic .text disassembly into per-FUNCTION fallback assembly.

The TU split proved the monolith partitions losslessly at unit boundaries;
this tool goes one level deeper and cuts at every function boundary, emitting
one self-contained `.s` per function into

    build/<version>/fallback/unit-NNNN/<Name>_<vram>.s

These are the INCLUDE_ASM inputs for hybrid translation units: a C file
carries the retail assembly of its not-yet-decompiled functions by including
these slices in source order (see include/include_asm.h; tools/cc.py stages
the owning unit's slice directory next to the source at compile time).

The slices must assemble under the *original* Sony assembler (ee-gcc's
`ee/bin/as`), which predates the macro features splat's `macro.inc` relies
on. So instead of shipping macro invocations, the splat block macros are
expanded here into plain directives:

    glabel N   ->  .globl N + N:          (function label)
    alabel N   ->  .globl N + N:          (alternative entry)
    jlabel N / ehlabel N  ->  N:          (local jump/handler targets)
    nonmatching / matching / endlabel     ->  dropped (markers and metadata
                                              only; no bytes)

Instruction lines, local `.L` labels, and blank lines pass through verbatim,
so the encoded words -- and therefore the bytes -- are exactly the retail
slice's. Every slice is checked: exactly one function label, and its first
instruction's vram must equal the function's registered address.

Nothing here is committed: the output is game-derived and lands in the
gitignored build/ tree. Pure Python; no toolchain needed.

Exit status is 0 only if every function produced a verified slice.
"""
import argparse
import re
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from declib.asmtext import disambiguate, instr_at
from declib.toolchain import Reporter
from declib.tu import drop_alignment, parse_toml_blocks, prologue_end, split_body

# Block-macro expansion for the original assembler (see module docstring).
_GLOBAL_LABELS = ("glabel", "alabel")
_LOCAL_LABELS = ("jlabel", "ehlabel")
_DROPPED = ("nonmatching", "matching", "endlabel")

# The original assembler (GNU as 2.9-ee-991111) accepts only numeric GPR
# names: `jr $ra` is "illegal operands", `jr $31` assembles (probed; only
# $sp/$fp/$gp are special-cased because ee-gcc's own output uses them).
# spimdisasm emits symbolic names, so slices are rewritten numerically.
# Renaming a register never changes the encoding; the whole-unit byte gate
# (tools/verify_hybrid.py) proves it.
_GPR_NUMBER = {
    "zero": 0, "at": 1, "v0": 2, "v1": 3,
    "a0": 4, "a1": 5, "a2": 6, "a3": 7,
    "t0": 8, "t1": 9, "t2": 10, "t3": 11,
    "t4": 12, "t5": 13, "t6": 14, "t7": 15,
    "s0": 16, "s1": 17, "s2": 18, "s3": 19,
    "s4": 20, "s5": 21, "s6": 22, "s7": 23,
    "t8": 24, "t9": 25, "k0": 26, "k1": 27,
    "gp": 28, "sp": 29, "fp": 30, "s8": 30, "ra": 31,
}
_GPR_RE = re.compile(r"\$(" + "|".join(_GPR_NUMBER) + r")\b")


def numeric_registers(line):
    return _GPR_RE.sub(lambda m: f"${_GPR_NUMBER[m.group(1)]}", line)


def load_functions(version):
    """[(address, name, unit)] sorted by address, from functions.toml."""
    rows = [(int(r["address"], 16), r["name"], int(r["unit"]))
            for r in parse_toml_blocks(
                ROOT / "config" / version / "functions.toml",
                {"name": r"name = '([^']*)'",
                 "address": r"address = (0x[0-9A-Fa-f]+)",
                 "unit": r"unit = (\d+)"})]
    rows.sort()
    return rows


def expand_macros(chunk):
    """Replace splat block macros with plain directives; keep the rest."""
    out = []
    for line in chunk:
        stripped = line.strip()
        word = stripped.split()[0] if stripped else ""
        if word in _DROPPED:
            continue
        if word in _GLOBAL_LABELS or word in _LOCAL_LABELS:
            label = stripped.split()[1].rstrip(",")
            if word in _GLOBAL_LABELS:
                out.append(f".globl {label}")
            out.append(f"{label}:")
            continue
        out.append(numeric_registers(line))
    return out


def trim_to_function(slice_lines, addr, is_first_function):
    """Drop everything before the label whose block starts at `addr`.

    A chunk normally leads with its function's own `.globl`, but splat also
    emits auto-functions (`func_XXXXXXXX`) that the mdebug registry does not
    know. One precedes the very first registered function (`_start`): the
    eight loader words at the top of .text, which belong to no slice (the
    per-TU expected objects still carry them). Auto-function blocks *after* a
    registered function stay in that function's slice -- they live inside its
    [addr, next_addr) range, which is exactly what the slice must reproduce.
    Any other slice losing instruction words here is an error.
    """
    for i, line in enumerate(slice_lines):
        if not line.startswith(".globl "):
            continue
        first_vram = next((instr_at(slice_lines, j)[0]
                           for j in range(i, len(slice_lines))
                           if instr_at(slice_lines, j) is not None), None)
        if first_vram == addr:
            dropped_instrs = sum(
                1 for j in range(i) if instr_at(slice_lines, j) is not None)
            if dropped_instrs and not is_first_function:
                return None, dropped_instrs
            return slice_lines[i:], dropped_instrs
    return None, 0


def first_instr_vram(lines):
    for i in range(len(lines)):
        info = instr_at(lines, i)
        if info:
            return info[0]
    return None


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--version", default="pal103")
    args = parser.parse_args()

    text_s = ROOT / "asm" / "text.s"
    if not text_s.is_file():
        print("asm/text.s missing; run `python configure.py` first "
              "(gitignored, game-derived).", file=sys.stderr)
        return 2

    reporter = Reporter()
    mono, ok = disambiguate(text_s.read_text(), reporter)
    if not ok:
        return _finish(reporter)
    lines = drop_alignment(mono.splitlines(), reporter)
    if lines is None:
        return _finish(reporter)

    functions = load_functions(args.version)
    body_start = prologue_end(lines)
    chunks = split_body(lines, body_start, [a for a, _n, _u in functions[1:]])
    if len(chunks) != len(functions):
        reporter.result("Function-granularity split", False,
                        f"{len(chunks)} chunks for {len(functions)} functions")
        return _finish(reporter)
    reporter.result("Function-granularity split", True)

    out_root = ROOT / "build" / args.version / "fallback"
    if out_root.exists():
        shutil.rmtree(out_root)

    problems = []
    for fi, (chunk, (addr, name, unit)) in enumerate(zip(chunks, functions)):
        slice_lines, dropped = trim_to_function(expand_macros(chunk), addr,
                                                fi == 0)
        if slice_lines is None:
            problems.append(f"{name}: {dropped} instruction(s) before the "
                            f"function label" if dropped
                            else f"{name}: no global label in slice")
            continue
        if dropped:
            print(f"  note: {dropped} pre-function words at the top of .text "
                  f"are not in {name}'s slice (loader words before 0x{addr:08x})")
        got = first_instr_vram(slice_lines)
        if got != addr:
            problems.append(f"{name}: slice starts at "
                            f"{got and hex(got)}, expected 0x{addr:08x}")
            continue
        unit_dir = out_root / f"unit-{unit:04d}"
        unit_dir.mkdir(parents=True, exist_ok=True)
        out = unit_dir / f"{name}_{addr:08x}.s"
        out.write_text(
            "/* Generated by tools/gen_slices.py -- do not edit, do not "
            "commit (game-derived). */\n"
            + "\n".join(slice_lines) + "\n")

    reporter.result("Per-function slices verified", not problems,
                    "; ".join(problems[:4]) or None)
    if not problems:
        units = {u for _a, _n, u in functions}
        print(f"  {len(functions)} slices -> "
              f"{out_root.relative_to(ROOT).as_posix()}/ "
              f"({len(units)} unit directories, gitignored)")
    return _finish(reporter)


def _finish(reporter):
    print()
    if reporter.failed:
        print("Fallback slice generation FAILED:")
        for detail in reporter.details:
            print(f"  - {detail}")
        return 1
    print("Fallback slice generation OK.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
