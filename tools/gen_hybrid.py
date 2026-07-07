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
import struct
import subprocess
import sys
import tomllib
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
import cc
from declib.toolchain import AS, h, tool_path

LINK_SETS = {"matching": ("matching",), "equivalent": ("matching", "equivalent")}

_ENT_RE = re.compile(r"^\s*\.ent\s+(\S+)")
_END_RE = re.compile(r"^\s*\.end\s+(\S+)")
# Directives that may lead a function block (walked back into its segment).
# (`.p2align N` is the Sony 2.9 spelling, `.align N` the SN 2.95 one; both
# mean 2^N here.)
_LEAD_RE = re.compile(r"^\s*(\.text|\.p2align\s+\d+|\.align\s+\d+|\.globl\s+\S+)\s*$")
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

# Float-constant loads. ee-gcc emits `li.s $fN,<decimal>` and leaves the
# materialization to the assembler: a constant whose float32 image has a
# zero low half becomes an inline lui+mtc1 (no data, byte-exact, leave it
# alone); anything else becomes a .lit4 literal-pool entry -- data this
# tool must not own. Retail already contains every such constant in its
# .lit4 section, addressed gp-relative, and the function's own retail slice
# names exactly which slot (`%gp_rel(D_0062....)`), so pool-bound li.s
# lines are rewritten to load the retail slot by symbol instead
# (`lwc1 $fN, D_0062....` + `.extern` -> R_MIPS_GPREL16, resolved to the
# retail address by the image link). The whole-unit byte gates verify the
# mapping; any constant that cannot be mapped unambiguously fails loudly.
_LIS_RE = re.compile(r"^\s*li\.s\s+(\$f\d+),(\S+)\s*$")
_LID_RE = re.compile(r"^\s*li\.d\s")
_GPREL_SYM_RE = re.compile(r"%gp_rel\(([A-Za-z_$][\w$]*)\)")


def _float_bits(text):
    """float32 bit pattern of a gcc-printed decimal constant."""
    return struct.unpack("<I", struct.pack("<f", float(text)))[0]


class Lit4Mapper:
    """Map float32 bit patterns to the retail .lit4 slots a function uses."""

    def __init__(self, version):
        from declib.asmtext import AUTO_NAME_RE, load_symbol_addrs
        from declib.target import load_target
        elf, sections_spec, pt_load = load_target(version)
        self._elf = elf
        self._delta = h(pt_load["vaddr"]) - h(pt_load["offset"])
        lit4 = next(s for s in sections_spec["sections"]
                    if s["name"] == ".lit4")
        self._lo = h(lit4["addr"])
        self._hi = self._lo + h(lit4["size"])
        self._symbols = load_symbol_addrs(version)
        self._auto_re = AUTO_NAME_RE

    def _address_of(self, sym):
        """Same resolution rule as the image link (declib.asmtext.resolve):
        the registry first, then splat's auto names (D_<vram>)."""
        addr = self._symbols.get(sym)
        if addr is not None:
            return addr
        m = self._auto_re.fullmatch(sym)
        return int(m.group(1), 16) if m else None

    def _value_at(self, addr):
        off = addr - self._delta
        return struct.unpack_from("<I", self._elf, off)[0]

    def map_for_slice(self, slice_text, context):
        """{float32 bits: retail symbol} for one function's retail slice."""
        mapping = {}
        for sym in set(_GPREL_SYM_RE.findall(slice_text)):
            addr = self._address_of(sym)
            if addr is None or not (self._lo <= addr < self._hi):
                continue
            bits = self._value_at(addr)
            other = mapping.get(bits)
            if other is not None and other != sym:
                raise HybridError(
                    f"{context}: retail slice references two .lit4 slots "
                    f"({other}, {sym}) holding the same value 0x{bits:08X}; "
                    f"cannot map li.s unambiguously")
            mapping[bits] = sym
        return mapping


def _sonyize(line):
    m = _MOVE_RE.match(line)
    if m:
        return f"{m.group(1)}daddu{m.group(2)}{m.group(3)},{m.group(4)},$0"
    m = _BREAK_RE.match(line)
    if m:
        return f"{m.group(1)}break{m.group(2)}0,{m.group(3)}"
    return line


def _unit_end(unit_dir):
    """End address of a unit (= next unit's start), or None for the last."""
    from declib.tu import load_tu_runs
    runs = load_tu_runs()
    index = int(unit_dir.split("-")[1])
    starts = [a for u, a in runs if u == index]
    if not starts:
        raise HybridError(f"unit {unit_dir} not in the TU runs")
    later = [a for _u, a in runs if a > starts[0]]
    return min(later) if later else None


def _manifest_functions(data):
    """[(addr, name, state)] in retail address order, from a status manifest."""
    out = []
    for entry in data.get("function", []):
        _v, _u, addr, name = entry["id"].split(":")
        out.append((int(addr, 16), name, entry["state"]))
    return sorted(out)


def _slice_path(version, unit_dir, name, addr):
    path = (ROOT / "build" / version / "fallback" / unit_dir
            / f"{name}_{addr:08x}.s")
    if not path.is_file():
        raise HybridError(f"{path.relative_to(ROOT).as_posix()} missing "
                          f"(run `python tools/gen_slices.py`)")
    return path


def _slice_lines(version, unit_dir, name, addr):
    path = _slice_path(version, unit_dir, name, addr)
    return [".text", ".set noat", ".set noreorder",
            path.read_text().rstrip("\n"),
            ".set reorder", ".set at"]


def _rewrite_lis(seg, version, unit_dir, name, addr, mapper_box):
    """Rewrite pool-bound `li.s` lines in one compiled segment.

    Returns (lines, extern_directives). Inline-representable constants
    (zero low half -- the assembler materializes them as lui+mtc1, no
    data) pass through untouched.
    """
    if not any("li.s" in l or "li.d" in l for l in seg):
        return seg, []
    mapping = None
    out, externs = [], []
    for line in seg:
        if _LID_RE.match(line):
            raise HybridError(f"{name}: li.d (.lit8 pool) not supported yet")
        m = _LIS_RE.match(line)
        if not m:
            out.append(line)
            continue
        reg, const = m.group(1), m.group(2)
        try:
            bits = _float_bits(const)
        except ValueError:
            raise HybridError(f"{name}: unparseable li.s constant {const!r}")
        if bits & 0xFFFF == 0:
            out.append(line)
            continue
        if mapping is None:
            if mapper_box[0] is None:
                mapper_box[0] = Lit4Mapper(version)
            slice_text = _slice_path(version, unit_dir, name, addr).read_text()
            mapping = mapper_box[0].map_for_slice(slice_text, name)
        sym = mapping.get(bits)
        if sym is None:
            raise HybridError(
                f"{name}: li.s constant {const} (0x{bits:08X}) has no "
                f".lit4 slot among the retail function's gp references")
        out.append(f"\tlwc1\t{reg},{sym}")
        externs.append(f"\t.extern\t{sym}, 4")
    return out, externs


def _compile_and_split(data, out_o, link_set, version):
    """Compile the unit's C and split it into (prologue, seg_by_name,
    functions, unit_dir). Shared by the hybrid link and the report base
    object -- both start from the same compiler output and the same
    from-C function set."""
    src = ROOT / data["source"]
    unit_dir = data["unit"].split(":")[1]
    functions = _manifest_functions(data)
    from_c = {name for _a, name, state in functions
              if state in LINK_SETS[link_set]}
    cs = Path(out_o).with_name(Path(out_o).stem + "_cc.s")
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
    return prologue, seg_by_name, functions, unit_dir


def _splice_unit(prologue, seg_by_name, functions, unit_dir, link_set,
                 version, report_base, unit_end):
    """Ordered .s lines for one unit.

    Every function with accepted C (per link_set) is compiled and normalized
    (`_rewrite_lis` maps pool-bound li.s onto the retail .lit4 slots,
    `_sonyize` fixes the two decompals pseudo-ops). The two modes differ in
    what they do around those functions:

      report_base=False (hybrid): un-decompiled functions are spliced from
        their retail slice, and each C function is zero-padded to its registry
        extent so the linked image is byte-exact (gas fails loudly if compiled
        code overruns its extent).
      report_base=True  (objdiff report base): un-decompiled functions are
        omitted entirely (no symbol -> objdiff pairs the retail target with
        nothing -> 0%), and C functions are NOT extent-padded. objdiff diffs
        per symbol, so the .lit4/pseudo-op normalization alone makes a matching
        function byte-identical to its retail symbol (100%); an equivalent
        function -- which may compile longer than its retail extent -- shows
        its honest partial match without a backwards-.org failure.
    """
    out_lines = list(prologue)
    body_lines = []
    mapper_box = [None]
    base = functions[0][0]
    ends = [a for a, _n, _s in functions[1:]] + [unit_end]
    for (addr, name, state), end in zip(functions, ends):
        if state in LINK_SETS[link_set]:
            seg, externs = _rewrite_lis(seg_by_name[name], version, unit_dir,
                                        name, addr, mapper_box)
            out_lines += externs   # symbol metadata; hoisted, emits no bytes
            body_lines += [_sonyize(l) for l in seg]
            if end is not None and not report_base:
                # A function's registry extent runs to the next function and
                # includes retail's trailing pad nops; the compiler does not
                # emit those. Zero-fill to the extent end (zero == nop) --
                # and gas fails loudly ("moving .org backwards") if the
                # compiled code overruns its extent.
                body_lines.append(f".org 0x{end - base:X}, 0")
        elif report_base:
            continue                 # un-decompiled: no symbol, no bytes
        else:
            body_lines += _slice_lines(version, unit_dir, name, addr)
    out_lines += body_lines
    return out_lines


def _assemble_unit(out_o, out_lines):
    """Write the spliced .s next to out_o, assemble it, and reject data."""
    out_o = Path(out_o)
    unit_s = out_o.with_suffix(".s")
    unit_s.parent.mkdir(parents=True, exist_ok=True)
    unit_s.write_text("\n".join(out_lines) + "\n")
    _assemble_hybrid(unit_s, out_o)
    _check_no_data_sections(out_o)
    return unit_s


def build_hybrid(manifest_path, out_o, link_set="matching", version="pal103"):
    """Assemble one unit's hybrid object; returns the intermediate .s path."""
    data = tomllib.loads(Path(manifest_path).read_text())
    prologue, seg_by_name, functions, unit_dir = _compile_and_split(
        data, out_o, link_set, version)
    out_lines = _splice_unit(prologue, seg_by_name, functions, unit_dir,
                             link_set, version, report_base=False,
                             unit_end=_unit_end(unit_dir))
    return _assemble_unit(out_o, out_lines)


def build_report_object(manifest_path, out_o, version="pal103"):
    """Assemble one unit's objdiff report base object; returns the .s path.

    Same C compile and normalization as the matching+equivalent hybrid, but
    un-decompiled functions are zero-filled instead of spliced from retail:
    the object carries ONLY C-generated function bytes. A `matching` function
    is byte-identical to its hybrid (hence to retail), so objdiff scores it
    100%; an `equivalent` function shows its honest partial match; an `asm`
    function has no symbol here and shows 0%. No retail slice bytes, and
    `_check_no_data_sections` guarantees it introduces no data section of its
    own -- this is a report-only measurement object, never a link input.
    """
    data = tomllib.loads(Path(manifest_path).read_text())
    # "equivalent" is the widest from-C set (matching + equivalent), so both
    # appear in the report; the byte gates never trust this object.
    link_set = "equivalent"
    prologue, seg_by_name, functions, unit_dir = _compile_and_split(
        data, out_o, link_set, version)
    out_lines = _splice_unit(prologue, seg_by_name, functions, unit_dir,
                             link_set, version, report_base=True,
                             unit_end=_unit_end(unit_dir))
    return _assemble_unit(out_o, out_lines)


_DATA_SECTIONS = {".lit4", ".lit8", ".data", ".sdata", ".rdata", ".rodata",
                  ".sbss", ".bss"}


def _check_no_data_sections(out_o):
    """Fail loudly if the assembled hybrid carries any data section bytes.

    A hybrid may only contribute .text: the image's data bytes come from the
    per-range data objects (tools/gen_data_objects.py). A non-empty data
    section here means a compiled segment slipped data past the rewrites
    (e.g. an unhandled literal pool) and the link would either fail or,
    worse, silently place bytes this tool never verified.
    """
    f = Path(out_o).read_bytes()
    if f[:4] != b"\x7fELF":
        raise HybridError(f"{out_o}: not an ELF object")
    shoff = struct.unpack_from("<I", f, 0x20)[0]
    shentsize = struct.unpack_from("<H", f, 0x2E)[0]
    shnum = struct.unpack_from("<H", f, 0x30)[0]
    shstrndx = struct.unpack_from("<H", f, 0x32)[0]
    def field(i, off):
        return struct.unpack_from("<I", f, shoff + i * shentsize + off)[0]
    str_off = field(shstrndx, 0x10)
    offenders = []
    for i in range(shnum):
        name_off = str_off + field(i, 0x0)
        name = f[name_off:f.index(b"\x00", name_off)].decode()
        size = field(i, 0x14)
        if name in _DATA_SECTIONS and size > 0:
            offenders.append(f"{name} ({size} bytes)")
    if offenders:
        raise HybridError(
            f"{out_o}: hybrid object owns data sections: "
            f"{', '.join(offenders)} -- compiled segments may only "
            f"contribute .text")


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
                        choices=sorted(LINK_SETS) + ["report"],
                        help="which states compile from C (default: matching; "
                             "`report` builds the objdiff report base object)")
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
        subdir = "report-current" if args.link_set == "report" \
            else args.link_set
        out_o = (Path(args.output).resolve() if args.output
                 else ROOT / "build" / args.version / subdir / rel)
        try:
            if args.link_set == "report":
                build_report_object(manifest, out_o, args.version)
            else:
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
