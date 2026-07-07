#!/usr/bin/env python3
"""Build hybrid objects: clean contributor C + retail assembly, manifest-driven.

Source files under src/ contain only decompiled functions -- no fallback
annotations. This tool produces the object the canonical (byte-identical)
build links for a unit:

  1. `ee-gcc -S` compiles the unit's C file to the exact assembly the driver
     would feed Sony's assembler, and the output is cut into per-function
     segments.
  2. The unit's status manifest (config/<version>/status/...) decides each
     function: `matching` uses its C segment; everything else uses the
     function's retail slice (build/<version>/fallback/, tools/gen_slices.py).
  3. Segments and slices are spliced in retail ADDRESS order -- the order of
     functions in the C file never matters -- and the result is assembled
     back through the ee-gcc driver, so Sony's own `as` with the profile's
     flags produces the final object.

Two link sets exist: `matching` (only exact functions from C) and
`equivalent` (reviewed-equivalent C compiles in too; slices only for `asm`
functions). tools/verify_hybrid.py proves the matching set byte-identical to
retail over the whole unit.

Anything this tool cannot yet represent fails loudly instead of guessing:
a declared function missing from the C, data sections in the compiler
output, or unexpected content between functions.

Needs the EE GCC: run in the Containerfile image. All outputs are
game-derived and land in the gitignored build/ tree.
"""
import argparse
import re
import subprocess
import sys
import tomllib
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
import cc
from declib.toolchain import AS, tool_path

LINK_SETS = {"matching": ("matching",), "equivalent": ("matching", "equivalent")}

_ENT_RE = re.compile(r"^\s*\.ent\s+(\S+)")
_END_RE = re.compile(r"^\s*\.end\s+(\S+)")
# Directives that may lead a function block (walked back into its segment).
_LEAD_RE = re.compile(r"^\s*(\.text|\.p2align\s+\d+|\.globl\s+\S+)\s*$")
# Byte-less symbol metadata ee-gcc emits between/after functions for extern
# arrays it saw sizes for (`.extern CModel, 117120`): safe to hoist into the
# prologue -- it only informs the assembler's gp-relative addressing choice
# and emits no section content.
_EXTERN_RE = re.compile(r"^\s*\.extern\s+\S+\s*,\s*\d+\s*$")
# Compiler output this tool cannot splice yet (owned data): fail loudly.
_DATA_RE = re.compile(
    r"^\s*\.(section|data|sdata|rdata|rodata|lit4|lit8|sbss|bss|comm|lcomm)\b")


class HybridError(Exception):
    pass


def _is_noise(line):
    s = line.strip()
    return not s or s.startswith("#")


def parse_s(text):
    """(prologue_lines, [(name, segment_lines)...]) from `ee-gcc -S` output.

    A segment runs from the function's lead directives (.text/.p2align/.globl)
    through its `.end`. Anything between segments, after the last one, or in
    a data section is unsupported for now and raises instead of guessing.
    """
    lines = text.splitlines()
    ents = [(i, m.group(1)) for i, l in enumerate(lines)
            if (m := _ENT_RE.match(l))]
    ends = [(i, m.group(1)) for i, l in enumerate(lines)
            if (m := _END_RE.match(l))]
    if len(ents) != len(ends):
        raise HybridError(f"unbalanced .ent/.end ({len(ents)}/{len(ends)})")

    segments = []
    externs = []
    prologue_end = len(lines)
    prev_end = -1
    for (ei, name), (xi, xname) in zip(ents, ends):
        if xname != name or xi <= ei:
            raise HybridError(f".ent {name} paired with .end {xname}")
        start = ei
        while start - 1 > prev_end and (
                _LEAD_RE.match(lines[start - 1]) or not lines[start - 1].strip()):
            start -= 1
        if segments:
            gap = lines[prev_end + 1:start]
            bad = [g for g in gap
                   if not _is_noise(g) and not _EXTERN_RE.match(g)]
            if bad:
                raise HybridError(f"unhandled content before {name}: {bad[0]!r}")
            externs += [g for g in gap if _EXTERN_RE.match(g)]
        else:
            prologue_end = start
        segments.append((name, lines[start:xi + 1]))
        prev_end = xi

    tail = lines[prev_end + 1:]
    trailing = [l for l in tail
                if not _is_noise(l) and not _EXTERN_RE.match(l)]
    if trailing:
        raise HybridError(f"unhandled content after last function: "
                          f"{trailing[0]!r}")
    externs += [l for l in tail if _EXTERN_RE.match(l)]

    prologue = lines[:prologue_end] + externs
    for where, chunk in [("prologue", prologue)] + [
            (f"function {n}", seg) for n, seg in segments]:
        for line in chunk:
            if _DATA_RE.match(line):
                raise HybridError(f"data section in {where} not supported "
                                  f"yet: {line.strip()!r}")
    return prologue, segments


# The decompals `as` encodes two of ee-gcc's pseudo-instructions differently
# from Sony's `as`, whose choices are what retail contains. Rewriting the
# compiled segments to the explicit forms makes both assemblers -- and
# retail -- agree; the byte gates verify the result as always.
#   move $x,$y   Sony: daddu $x,$y,$0     decompals: or $x,$y,$0
#   break N      Sony: code in bits 6-15  decompals: code in bits 16-25
_MOVE_RE = re.compile(r"^(\s*)move(\s+)(\$\w+),(\$\w+)\s*$")
_BREAK_RE = re.compile(r"^(\s*)break(\s+)(\d+)\s*$")


def _sonyize(line):
    m = _MOVE_RE.match(line)
    if m:
        return f"{m.group(1)}daddu{m.group(2)}{m.group(3)},{m.group(4)},$0"
    m = _BREAK_RE.match(line)
    if m:
        return f"{m.group(1)}break{m.group(2)}0,{m.group(3)}"
    return line


def _manifest_functions(data):
    """[(addr, name, state)] in retail address order, from a status manifest."""
    out = []
    for entry in data.get("function", []):
        _v, _u, addr, name = entry["id"].split(":")
        out.append((int(addr, 16), name, entry["state"]))
    return sorted(out)


def _slice_lines(version, unit_dir, name, addr):
    path = (ROOT / "build" / version / "fallback" / unit_dir
            / f"{name}_{addr:08x}.s")
    if not path.is_file():
        raise HybridError(f"{path.relative_to(ROOT).as_posix()} missing "
                          f"(run `python tools/gen_slices.py`)")
    return [".text", ".set noat", ".set noreorder",
            path.read_text().rstrip("\n"),
            ".set reorder", ".set at"]


def build_hybrid(manifest_path, out_o, link_set="matching", version="pal103"):
    """Assemble one unit's hybrid object; returns the intermediate .s path."""
    data = tomllib.loads(Path(manifest_path).read_text())
    src = ROOT / data["source"]
    unit_dir = data["unit"].split(":")[1]
    functions = _manifest_functions(data)
    from_c = {name for _a, name, state in functions
              if state in LINK_SETS[link_set]}

    out_o = Path(out_o)
    hybrid_s = out_o.with_suffix(".s")
    cs = out_o.with_name(out_o.stem + "_cc.s")
    cc.compile_s(src, cs, profile=data.get("profile", "default"),
                 version=version)
    prologue, segments = parse_s(cs.read_text())
    seg_by_name = dict(segments)
    if len(seg_by_name) != len(segments):
        raise HybridError("duplicate function names in compiler output")
    missing = sorted(from_c - set(seg_by_name))
    if missing:
        raise HybridError(f"declared {'/'.join(LINK_SETS[link_set])} but not "
                          f"defined in {data['source']}: {', '.join(missing)}")

    out_lines = list(prologue)
    for addr, name, state in functions:
        if state in LINK_SETS[link_set]:
            out_lines += [_sonyize(l) for l in seg_by_name[name]]
        else:
            out_lines += _slice_lines(version, unit_dir, name, addr)
    hybrid_s.parent.mkdir(parents=True, exist_ok=True)
    hybrid_s.write_text("\n".join(out_lines) + "\n")
    _assemble_hybrid(hybrid_s, out_o)
    return hybrid_s


def _assemble_hybrid(hybrid_s, out_o):
    """Assemble the spliced .s with the decompals binutils `as`.

    Sony's own `as` (the original plan) cannot assemble the retail slices:
    it predates the `%gp_rel(sym)($gp)` relocation syntax, and it carries an
    unconditional R5900 short-loop erratum workaround that pads backward
    branches with nops -- retail game code contains unpadded 4-instruction
    loops, so the padded output can never be byte-identical. The decompals
    `as` assembles both the slices (it already produces the byte-verified
    expected objects from the same text) and ee-gcc's compiler output
    (`.extern SYM, n` + macro forms resolve to R_MIPS_GPREL16 under -G8,
    probed) without padding. The whole-unit byte gates remain the arbiter
    of every matching claim, so a divergent encoding fails loudly.
    """
    as_bin = tool_path(AS)
    if not as_bin:
        raise HybridError(f"{AS} not found (setup_toolchain.py installs it)")
    out_o = Path(out_o)
    out_o.parent.mkdir(parents=True, exist_ok=True)
    proc = subprocess.run(
        [as_bin, "-EL", "-march=r5900", "-G8",
         "-o", str(out_o), str(hybrid_s)],
        capture_output=True, text=True)
    if proc.returncode != 0:
        raise HybridError(f"assembling {hybrid_s} failed:\n{proc.stderr}")


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--version", default="pal103")
    parser.add_argument("--set", dest="link_set", default="matching",
                        choices=sorted(LINK_SETS),
                        help="which states compile from C (default: matching)")
    parser.add_argument("--manifest",
                        help="build only this status manifest (ninja edge "
                             "mode) instead of all of them")
    parser.add_argument("-o", "--output",
                        help="output object path (only with --manifest; "
                             "default: the standard build/ location)")
    args = parser.parse_args()

    if args.output and not args.manifest:
        parser.error("-o requires --manifest")
    if args.manifest:
        manifests = [Path(args.manifest)]
        if not manifests[0].is_file():
            print(f"manifest not found: {args.manifest}", file=sys.stderr)
            return 2
    else:
        status_dir = ROOT / "config" / args.version / "status"
        manifests = sorted(status_dir.rglob("*.toml")) if status_dir.is_dir() else []
        if not manifests:
            print("no status manifests; nothing to build.", file=sys.stderr)
            return 2

    failed = False
    for manifest in manifests:
        data = tomllib.loads(manifest.read_text())
        rel = Path(data["source"]).relative_to("src").with_suffix(".o")
        out_o = (Path(args.output).resolve() if args.output
                 else ROOT / "build" / args.version / args.link_set / rel)
        try:
            build_hybrid(manifest, out_o, args.link_set, args.version)
            try:
                shown = out_o.relative_to(ROOT).as_posix()
            except ValueError:
                shown = out_o.as_posix()
            print(f"{rel.as_posix():<40} -> {shown}")
        except HybridError as exc:
            print(f"{rel.as_posix():<40} FAILED: {exc}", file=sys.stderr)
            failed = True
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
