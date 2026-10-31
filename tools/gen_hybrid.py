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
     functions in the C file never matters. The spliced assembly is
     normalized (`_sonyize_seg`: the pseudo-ops ee-gcc and the decompals
     binutils encode differently, plus the hazard and delay-slot nops the two
     assemblers disagree on) and assembled with the decompals `as`
     (declib.toolchain.AS), NOT the ee-gcc driver -- the retail slices are
     already-assembled text that only that assembler round-trips byte-exactly.

Two link sets exist: `matching` (only exact functions from C) and
`equivalent` (reviewed-equivalent C compiles in too; slices only for `asm`
functions). tools/verify_hybrid.py proves the matching set byte-identical to
retail over the whole unit. build_report_object() reuses the same compile and
normalization to emit objdiff's base object (C functions only, no slices, no
extent padding) -- a measurement, never linked.

Data-from-C: a unit whose manifest has a `matching` [[data]] entry also
gets data pieces -- one input section per owned data range
(`<sec>.split_<vram8>`) holding its `matching` objects compiled from C at
their registry addresses and every other byte as retail `.incbin`; the image
link then takes that unit's data from the hybrid instead of the expected
data object. C data of `asm`/`retail` entries is dropped and its references
renamed onto the retail symbol; so is an `extern` of one of the unit's own
registry statics under its retail name. A sidecar `<object>.data.json`
records the pieces, the retail byte ranges in them and the compiler-private
retail slots the code borrows (verify_promoted's `complete` rule reads it).

Whole-unit mode (data/private): a `complete` unit's hybrid borrows nothing.
Its compiler-private data -- `$LC` strings and aggregates, jump tables, the
`li.d $gpr` pool and the .lit4 pool, which gen_hybrid allocates itself the
way the retail assemblers did -- is laid out per section in retail function
order, anchored where the retail references put it, and emitted into the
pieces together with the named objects. Bytes that land in a data-map gap
are the unit's `[[claim]]`s (promote.py --complete writes them; the image
link then takes each claimed span from this hybrid). Its uninitialised
objects (.sbss/.bss statics, a COMMON global) become NOBITS pieces the same
way; outside whole-unit mode they are always dropped.

Anything this tool cannot yet represent fails loudly instead of guessing:
a declared function missing from the C, C data without a [[data]] table, in
the wrong section or of the wrong size, or unexpected content between
functions.

Needs the EE GCC: run in the Containerfile image. All outputs are
game-derived and land in the gitignored build/ tree.
"""
import argparse
import bisect
import itertools
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
# `.file` markers emit no bytes; allowed anywhere noise is.
_FILE_RE = re.compile(r'^\s*\.file\s+(?:\d+\s+)?".*"\s*$')
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


def parse_s(text, data_items=None, stream=None):
    """(prologue_lines, [(name, segment_lines)...]) from `ee-gcc -S` output.

    Local initialized aggregates come back separately as {label: bytes};
    named data objects are appended to `data_items` (a list) when given and
    rejected otherwise; anything unexpected raises. `stream` collects every
    data object in assembly order (_split_sections).
    """
    lines, local_data, items = _split_sections(text.splitlines(), stream)
    if items:
        if data_items is None:
            raise HybridError(f"data section in compiler output not supported "
                              f"yet: {items[0].name} ({items[0].section})")
        data_items.extend(items)
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
                _LEAD_RE.match(lines[start - 1]) or _FILE_RE.match(lines[start - 1])
                or not lines[start - 1].strip()):
            start -= 1
        if segments:
            gap = lines[prev_end + 1:start]
            bad = [g for g in gap if not _is_noise(g)
                   and not _EXTERN_RE.match(g) and not _FILE_RE.match(g)]
            if bad:
                raise HybridError(f"unhandled content before {name}: {bad[0]!r}")
            externs += [g for g in gap if _EXTERN_RE.match(g)]
        else:
            prologue_end = start
        segments.append((name, lines[start:xi + 1]))
        prev_end = xi

    # A skeleton unit is all prologue.
    tail = lines[prev_end + 1:] if segments else []
    trailing = [l for l in tail if not _is_noise(l)
                and not _EXTERN_RE.match(l) and not _FILE_RE.match(l)]
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
    return prologue, segments, local_data


# The decompals `as` encodes two of ee-gcc's pseudo-instructions differently
# from Sony's `as`, whose choices are what retail contains. Rewriting the
# compiled segments to the explicit forms makes both assemblers -- and
# retail -- agree; the byte gates verify the result as always.
#   move $x,$y   Sony: daddu $x,$y,$0     decompals: or $x,$y,$0
#   break N      Sony: code in bits 6-15  decompals: code in bits 16-25
# Also `j $reg` (ee-gcc's switch-dispatch spelling) -> `jr $reg` (retail); the
# register set is enumerated so a jump to a `$L...` label is never touched.
_MOVE_RE = re.compile(r"^(\s*)move(\s+)(\$\w+),(\$\w+)\s*$")
_BREAK_RE = re.compile(r"^(\s*)break(\s+)(\d+)\s*$")
_JREG_RE = re.compile(
    r"^(\s*)j(\s+)(\$(?:\d+|zero|at|v[01]|a[0-3]|t\d|s[0-8]|k[01]|gp|sp|fp|ra))"
    r"\s*$")

# The decompals `as` rejects `cvt.w.s`; `trunc.w.s` is the same encoding. A raw
# `.word` would get a hazard nop after mfc1/mtc1 that retail does not have.
_CVTWS_RE = re.compile(r"^(\s*)cvt\.w\.s(?:\s+)\$f(\d+)\s*,\s*\$f(\d+)\s*$")

# VU0 macro operands ACC, Q and I: SN's and Sony's `as` take them bare, the
# decompals `as` only `$`-prefixed.
_VU_OP_RE = re.compile(r"^(\s*v[a-z]\w*(?:\.[xyzw]+)?\s+)(.*)$")
_VU_SPECIAL_RE = re.compile(r"(?<![\w$])(ACC|Q|I)\b")

# R5900 mtc1 hazard: SN's `as` pads a nop after `mtc1 rX,$fN` (or an inline
# `li.s`, also across a label) iff the next instruction reads $fN or its
# even/odd pair mate; the decompals `as` pads only exact dependencies.
_MTC1_RE = re.compile(r"^\s*mtc1\s+\$\w+\s*,\s*\$f\d+\s*$")
_LIS_INLINE_RE = re.compile(r"^\s*li\.s\s+\$f\d+\s*,")
_MTC1_DEST_RE = re.compile(r"^\s*(?:mtc1\s+\$\w+\s*,|li\.s\s+)\s*\$f(\d+)")
_FPR_RE = re.compile(r"\$f(\d+)\b")
# READ_ALL ops read every FPR operand, NO_READ ops none; any other FP op
# writes its first operand and reads the rest.
_FPR_READ_ALL = frozenset(("s.s", "swc1", "mfc1", "adda.s", "suba.s",
                           "mula.s", "madda.s", "msuba.s"))
_FPR_NO_READ = frozenset(("l.s", "lwc1", "mtc1", "li.s"))
# mfc1 interlock: both assemblers pad a nop when the next instruction reads
# the mfc1 destination. Across a `.p2align` SN's `as` aligns first, then pads,
# then binds the label (GliderSmoke 0x00204014, NuLightInit mtc1 0x001351B0);
# the decompals `as` pads before aligning.
_MFC1_RE = re.compile(r"^\s*mfc1\s+(\$\w+)\s*,\s*\$f\d+\s*$")
_GPR_RE = re.compile(
    r"\$(?:\d+|zero|at|v[01]|a[0-3]|t\d|s[0-8]|k[01]|gp|sp|fp|ra)\b")
# Coprocessor moves: the decompals `as` pads a nop after cfc2/mfc0 before a
# reader of the destination and after ctc2 before any COP2 op; retail has
# neither (sceDevVu0CheckBusy 0x00263528, _sceSDC 0x00279464,
# sceVu0MemWriteQ 0x00263990).
_COP2_MOVE_RE = re.compile(r"^\s*(cfc2|ctc2|mfc0)(?:\.n?i)?\s+(\$\w+)\s*,")
_COP2_OP_RE = re.compile(r"^\s+(?:v[a-z][\w.]*|qm[ft]c2|[cl]fc2|ctc2|[ls]qc2)"
                         r"(?:\.n?i)?\s")
# Ops that read their first GPR operand.
_GPR_READ_ALL = frozenset((
    "sb", "sh", "sw", "sd", "sq", "swl", "swr", "sdl", "sdr", "swc1", "sdc1",
    "sqc2", "mtc1", "dmtc1", "ctc1", "mthi", "mtlo", "mult", "multu", "div",
    "divu", "dmult", "dmultu", "ddiv", "ddivu", "mult1", "multu1", "div1",
    "divu1", "madd", "maddu", "jr", "jalr"))

# Unfilled branch delay slots. ee-gcc fills a delay slot itself only inside an
# explicit `.set noreorder` / `.set nomacro` block; every branch it emits
# outside one -- in practice the `j $31` ending a leaf function whose last
# instruction the scheduler could not sink into the slot -- is left empty for
# the assembler to fill, and the two assemblers fill it differently. Sony's
# `as` always drops in a `nop`; the decompals `as` in `.set reorder` mode
# instead *swaps* the preceding instruction into the slot. So retail reads
#
#     cvt.s.w $f0,$f0 / jr $31 / nop        (NuPs2GetRenderWidth @0x16A8E8)
#
# where the decompals `as` emits `jr $31 / cvt.s.w $f0,$f0` -- one word short,
# shifting every function after it. Reproduce Sony's choice explicitly: wrap a
# reorder-mode branch in `.set noreorder` and write the `nop` out. Where the
# decompals `as` could not have swapped anyway it already emitted exactly this
# nop, so the rewrite is a no-op there; the byte gates carry the proof.
# Not for profile `sce`: its retail assembler swapped too (sceDevVif0Reset
# ends `jr $ra / sw`).
_SET_REORDER_RE = re.compile(r"^\s*\.set\s+(no)?reorder\s*$")
_MNEMONIC_RE = re.compile(r"^\s+([a-z][a-z0-9._]*)\b")
_BRANCH_OPS = frozenset((
    "b", "bal", "j", "jr", "jal", "jalr",
    "beq", "bne", "beqz", "bnez", "blez", "bgtz", "bltz", "bgez",
    "bltzal", "bgezal", "bgt", "bge", "blt", "ble",
    "bgtu", "bgeu", "bltu", "bleu", "bc1t", "bc1f",
))


def _is_branch(line):
    m = _MNEMONIC_RE.match(line)
    return bool(m) and m.group(1) in _BRANCH_OPS


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
_LID_RE = re.compile(r"^\s*li\.d\s+(\$f\d+),(\S+)\s*$")
# `li.s`/`li.d` into a *GPR* (not `$fN`): ee-gcc materializes the raw float/
# double bit pattern in an integer register -- e.g. `li.d $5,1.0e1` to pass
# 10.0 as the `s64` argument of the software double helpers (dpmul et al).
# This is not a .lit4/.lit8 pool load, and the decompals `as` rejects the
# float-syntax GPR macro, so it must be rewritten. Sony's `as` picks one of two
# expansions, and which one is visible in retail: a cheap image built inline
# (10.0 -> `ori $a1,$zero,0x8048; dsll32 $a1,$a1,15`), or -- when inline would
# cost more -- the constant pooled into the unit's own initialized data and
# loaded `lui $at,%hi(SYM)` + `ld $reg,%lo(SYM)($at)` (every `f < 0.249`
# soft-double compare in game/crate). Pooled bytes are data the hybrid must not
# own, but retail already holds them and the function's own slice names the
# slot, so a 64-bit image equal to a slot the slice addresses borrows it
# (Lit4Mapper.map_gpr_pool8); everything else becomes the integer `li`/`dli`
# the assembler does expand, over the raw bit pattern.
_LI_GPR_RE = re.compile(r"^\s*(li\.[sd])\s+(\$(?!f\d)\S+),(\S+)\s*$")
_GPREL_SYM_RE = re.compile(r"%gp_rel\(([A-Za-z_$][\w$]*)\)")

# An initialized function-local aggregate (e.g. `short layertab[2] = {0,1}`)
# is emitted by ee-gcc as a private label ($LCn) in a data section, referenced
# %hi/%lo (or %gp_rel). The hybrid may only contribute .text, but retail
# already holds the identical bytes in a unit-owned data slot that the
# function's retail slice names (e.g. D_006309C8). So the block is captured,
# dropped, and the label rewritten onto that retail symbol -- the same trick
# _rewrite_lis plays for .lit4 pool constants.
_LOCAL_LABEL_RE = re.compile(r"^(\$[A-Za-z_.$][\w.$]*):\s*$")
# Alignment directives ee-gcc puts before loop heads.
_ALIGN_RE = re.compile(r"^\s*\.(?:p2align|align)\s+\d+\s*$")
_ENTER_DATA_RE = re.compile(
    r"^\s*\.(sdata|data|rdata|rodata|sdata2)\b"
    r"|^\s*\.section\s+\.(sdata|data|rdata|rodata)\b")
_LEAVE_DATA_RE = re.compile(
    r"^\s*\.(text|ent|globl)\b|^\s*\.section\s+\.text\b")
# Symbol names in %hi/%lo/%gp_rel operands (candidate owned-data slots).
_SYM_REF_RE = re.compile(r"%(?:hi|lo|gp_rel)\(([A-Za-z_$][\w$]*)\)")
# Symbols a retail slice loads a doubleword from (`ld $reg, %lo(SYM)($at)`).
_LD_LO_SYM_RE = re.compile(r"\bld\s+\$\w+,\s*%lo\(([A-Za-z_$][\w$]*)\)")
# Initialized-data sections whose bytes exist in the ELF image (a private
# aggregate can only borrow a slot that actually holds matching bytes).
_OWNED_INIT_DATA = {".sdata", ".sdata2", ".data", ".rdata", ".rodata",
                    ".lit4", ".lit8"}

# A switch jump table: ee-gcc emits it as a private label ($Ln) in a .rdata
# block whose entries are `.word <local label>` -- relocations, not constant
# bytes, so it can't be assembled or byte-matched. The identical table lives in
# retail's .rodata as an auto-named `jtbl_<vram>`/`jpt_<vram>` slot the
# function's own retail slice addresses %hi/%lo; the compiler's copy is dropped
# and its label rewritten onto that slot (borrow-by-structure, not by value).
_WORD_LABEL_RE = re.compile(r"^\s*\.word\s+(\$?[A-Za-z_.$][\w.$]*)\s*$")
_JTBL_NAME_RE = re.compile(r"^(?:jtbl|jpt)_[0-9A-Fa-f]+$")


class _JumpTable:
    """Marker for a captured switch jump table: N `.word <label>` entries, no
    fixed bytes. Borrowed onto a retail jtbl_/jpt_ slot rather than emitted."""
    __slots__ = ("count",)

    def __init__(self, count):
        self.count = count

    def __eq__(self, other):
        return isinstance(other, _JumpTable) and other.count == self.count

    def __repr__(self):
        return f"_JumpTable({self.count})"


_STR_ESCAPES = {"a": 7, "b": 8, "f": 12, "n": 10, "r": 13, "t": 9, "v": 11,
                '"': 34, "\\": 92, "'": 39}


def _decode_as_string(body):
    """Bytes of the quoted arguments of a .ascii/.asciz-family directive.

    Concatenates every double-quoted run (comma-separated) and decodes the GNU
    as escapes ee-gcc emits: octal `\\NNN` (up to 3 digits, how gcc writes the
    trailing NUL and non-printables), hex `\\xHH`, and the letter escapes. A `#`
    outside quotes ends the arguments (trailing comment). Fails loudly on
    anything unexpected rather than guessing."""
    out = bytearray()
    i, n = 0, len(body)
    while i < n:
        c = body[i]
        if c == "#":
            break                       # trailing comment
        if c in ", \t":
            i += 1
            continue
        if c != '"':
            raise HybridError(f"unexpected char {c!r} in string directive")
        i += 1                          # opening quote
        while i < n and body[i] != '"':
            if body[i] != "\\":
                out.append(ord(body[i]) & 0xFF)
                i += 1
                continue
            i += 1                      # backslash
            if i >= n:
                raise HybridError("dangling escape in string literal")
            e = body[i]
            if e == "x":
                j = i + 1
                while j < n and j < i + 3 and body[j] in "0123456789abcdefABCDEF":
                    j += 1
                if j == i + 1:
                    raise HybridError("empty \\x escape in string literal")
                out.append(int(body[i + 1:j], 16) & 0xFF)
                i = j
            elif e in "01234567":
                j = i
                while j < n and j < i + 3 and body[j] in "01234567":
                    j += 1
                out.append(int(body[i:j], 8) & 0xFF)
                i = j
            elif e in _STR_ESCAPES:
                out.append(_STR_ESCAPES[e])
                i += 1
            else:
                raise HybridError(f"unsupported string escape \\{e}")
        if i >= n:
            raise HybridError("unterminated string literal")
        i += 1                          # closing quote
    return bytes(out)


def _assemble_data_bytes(block):
    """Little-endian bytes of a captured local data block (one label).

    Handles the directives ee-gcc emits for small const aggregates (numeric
    directives plus the .ascii/.asciz string family); anything else fails
    loudly rather than guessing the layout."""
    buf = bytearray()

    def emit(vals, size, signed=False):
        for a in vals:
            v = int(a, 0)
            buf.extend((v & ((1 << (8 * size)) - 1)).to_bytes(size, "little"))

    for line in block:
        raw = line.strip()
        if not raw or raw.startswith("#") or _LOCAL_LABEL_RE.match(line):
            continue
        if not raw.startswith("."):
            raise HybridError(f"unexpected line in local data block: {line!r}")
        parts = raw.split(None, 1)
        d = parts[0]
        body = parts[1] if len(parts) > 1 else ""
        # String directives are parsed from the raw body (a `#` or `,` inside a
        # quoted literal is not a comment or separator); numeric ones split on
        # commas after dropping any trailing `#` comment.
        if d in (".ascii", ".asciiz", ".asciz", ".string"):
            buf.extend(_decode_as_string(body))
            if d != ".ascii":
                buf.append(0)           # .asciz/.string/.asciiz add the NUL
            continue
        args = [a.strip() for a in body.split("#", 1)[0].split(",")
                if a.strip()]
        if d == ".align":
            step = 1 << int(args[0], 0)
            while len(buf) % step:
                buf.append(0)
        elif d in (".byte",):
            emit(args, 1)
        elif d in (".half", ".hword", ".short"):
            emit(args, 2)
        elif d in (".word", ".long", ".gpword"):
            emit(args, 4)
        elif d in (".dword", ".quad"):
            emit(args, 8)
        elif d == ".float":
            for a in args:
                buf.extend(struct.pack("<f", float(a)))
        elif d == ".double":
            for a in args:
                buf.extend(struct.pack("<d", float(a)))
        elif d in (".space", ".skip"):
            buf.extend(b"\x00" * int(args[0], 0))
        else:
            raise HybridError(f"unhandled local-data directive {d!r}")
    return bytes(buf)


# --------------------------------------------------------------------------- #
# Section-tracking splitter (compiled data)                                   #
# --------------------------------------------------------------------------- #

# Section switches ee-gcc emits; `.section` names map onto the same set.
_SECTION_SWITCH_RE = re.compile(
    r"^\s*(?:\.(text|data|rdata|rodata|sdata|sdata2|sbss|bss)\s*$"
    r"|\.section\s+\.?([A-Za-z0-9_]+)\b.*$|(\.previous)\s*$)")
_SECTION_NAMES = {"text": ".text", "data": ".data", "rdata": ".rodata",
                  "rodata": ".rodata", "sdata": ".sdata", "sdata2": ".sdata",
                  "sbss": ".sbss", "bss": ".bss"}
_GLOBL_RE = re.compile(r"^\s*\.globl\s+(\S+)\s*$")
_TYPE_OBJ_RE = re.compile(r"^\s*\.type\s+(\S+?)\s*,\s*@object\s*$")
_SIZE_DIR_RE = re.compile(r"^\s*\.size\s+(\S+?)\s*,\s*(\d+)\s*$")
_COMM_RE = re.compile(r"^\s*\.(comm|lcomm)\s+([^,\s]+)\s*,\s*(\d+)(?:\s*,\s*(\d+))?")
# Uninitialised data: no file bytes, only addresses.
NOBITS_SECTIONS = (".sbss", ".bss")
_DATA_LABEL_RE = re.compile(r"^([A-Za-z_.$][\w.$]*):\s*$")
_ALIGN_ARG_RE = re.compile(r"^\s*\.(?:p2align|align)\s+(\d+)\s*$")
_WORD_PRIVATE_RE = re.compile(r"^\s*\.word\s+(\$L\w*)\s*$")
FUNCTION_STATIC_RE = re.compile(r"^(.+)\.(\d+)$")


class DataItem:
    """One labelled object the compiler emitted in a data section."""

    def __init__(self, name, section, align, line=None):
        self.name, self.section, self.align = name, section, align
        self.lines = []          # its data directives (interior .align kept)
        self.size = None         # from `.size NAME,N`
        self.globl = False
        self.line = line         # index of its label in the compiled .s

    def __repr__(self):
        return f"DataItem({self.name!r}, {self.section!r})"


def _section_switch(line):
    """The section a switch line selects ('previous' for `.previous`)."""
    m = _SECTION_SWITCH_RE.match(line)
    if not m:
        return None
    if m.group(3):
        return "previous"
    name = m.group(1) or m.group(2)
    return _SECTION_NAMES.get(name, f".{name}")


def _split_sections(lines, stream=None):
    """Split compiled assembly by section.

    Returns (text_lines, private, items): the code stream with every data
    section's content removed, the compiler-private blocks ($LCn strings and
    aggregates as bytes, $Ln switch tables as _JumpTable markers), and the
    named data objects as DataItem (`.comm`/`.lcomm` as section 'COMMON').
    `.globl`, `.align`, `.type` and `.size` attach to the label that follows
    them; an `.align` followed by more data stays interior to its object.
    `stream` (a list) receives every labelled object, private and named, in
    assembly order with its line index (the whole-unit layout)."""
    labels, section, previous = set(), ".text", ".text"
    for line in lines:
        new = _section_switch(line)
        if new is not None:
            section, previous = (previous, section) if new == "previous" \
                else (new, section)
            continue
        m = _DATA_LABEL_RE.match(line)
        if m and section != ".text":
            labels.add(m.group(1))

    text, private_items, items = [], [], []
    section, previous = ".text", ".text"
    current, pending_align, globls, sizes = None, None, set(), {}
    for index, line in enumerate(lines):
        new = _section_switch(line)
        if new is not None:
            old = section
            section, previous = (previous, section) if new == "previous" \
                else (new, section)
            if old == ".text" and section == ".text":
                text.append(line)           # a plain `.text` lead stays
            current, pending_align = None, None
            continue
        if section == ".text":
            g = _GLOBL_RE.match(line)
            if g and g.group(1) in labels:
                globls.add(g.group(1))
                continue
            c = _COMM_RE.match(line)
            if c:
                align = int(c.group(4)) if c.group(4) else 0
                item = DataItem(c.group(2), "COMMON",
                                align.bit_length() - 1 if align else None, index)
                item.size = int(c.group(3))
                item.lines = [f"\t.space\t{item.size}"]
                items.append(item)
                if stream is not None:
                    stream.append(item)
                continue
            text.append(line)
            continue
        s = line.strip()
        if not s or s.startswith("#"):
            continue
        a = _ALIGN_ARG_RE.match(line)
        if a:
            pending_align = int(a.group(1))
            continue
        if _TYPE_OBJ_RE.match(line):
            continue
        z = _SIZE_DIR_RE.match(line)
        if z:
            sizes[z.group(1)] = int(z.group(2))
            continue
        g = _GLOBL_RE.match(line)
        if g:
            if g.group(1) in labels:
                globls.add(g.group(1))
            else:
                text.append(line)           # a function's lead, not data
            continue
        if _ENT_RE.match(line):
            raise HybridError(f"function inside a data section: {s!r}")
        m = _DATA_LABEL_RE.match(line)
        if m:
            current = DataItem(m.group(1), section, pending_align, index)
            pending_align = None
            (private_items if current.name.startswith("$") else items).append(current)
            if stream is not None:
                stream.append(current)
            continue
        if current is None:
            raise HybridError(
                "local data block with no private label unsupported")
        if pending_align is not None:
            if section in NOBITS_SECTIONS and not current.lines \
                    and current.align is None:
                # ee-gcc writes an uninitialised static as label, .align,
                # .space; the assembler moves the label to the boundary.
                current.align = pending_align
            else:
                current.lines.append(f"\t.align\t{pending_align}")
            pending_align = None
        current.lines.append(line)

    for item in items:
        item.globl = item.name in globls
        if item.size is None:
            item.size = sizes.get(item.name)
        if item.size is None and item.section in NOBITS_SECTIONS:
            item.size = len(_assemble_data_bytes(item.lines))
    private = {}
    for item in private_items:
        payload = [l for l in item.lines if not _ALIGN_ARG_RE.match(l)]
        if payload and all(_WORD_LABEL_RE.match(p) for p in payload):
            private[item.name] = _JumpTable(len(payload))
        else:
            private[item.name] = _assemble_data_bytes(item.lines)
    return text, private, items


def _extract_local_data(lines):
    """Pull ee-gcc's private data blocks ($LCn/$Ln in .sdata/.rodata/...) out
    of the compiled stream so the rest parses as pure text.

    A thin view over _split_sections for callers that expect no named data:
    named objects are left out of both results (parse_s reports them).

    Returns (clean_lines, {label: value}) where value is the block's bytes for
    a constant aggregate, or a `_JumpTable(count)` marker for a switch jump
    table (`.word <label>` entries, which are relocations, not bytes). Each
    block must define exactly one private label; the whole-unit byte gates
    verify every remap downstream."""
    clean, local, _items = _split_sections(lines)
    return clean, local


def _float_bits(text):
    """float32 bit pattern of a gcc-printed decimal constant."""
    return struct.unpack("<I", struct.pack("<f", float(text)))[0]


def _double_bits(text):
    """float64 bit pattern of a gcc-printed decimal constant."""
    return struct.unpack("<Q", struct.pack("<d", float(text)))[0]


def borrow_scan_ranges(ranges):
    """{owner: [(start, end)]} -- the data_map ranges a compiler-private
    aggregate may be found in by bytes (Lit4Mapper._borrow_addr_in_unit).

    A unit's .lit4 run is its float pool, never an aggregate's home: leaving
    it out keeps a pool word from making a borrow ambiguous.
    """
    out = {}
    for r in ranges:
        if r["section"] == ".lit4":
            continue
        out.setdefault(r["owner"], []).append((int(r["start"]), int(r["end"])))
    return out


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
        lit8 = next((s for s in sections_spec["sections"]
                     if s["name"] == ".lit8"), None)
        self._lo8 = h(lit8["addr"]) if lit8 else 0
        self._hi8 = self._lo8 + (h(lit8["size"]) if lit8 else 0)
        self._symbols = load_symbol_addrs(version)
        self._auto_re = AUTO_NAME_RE
        self._version, self._objects = version, None
        # Initialized-data sections a hybrid may borrow a slot from by symbol
        # (never .sbss/.bss: those hold no file bytes to match against).
        self._data_ranges = [
            (h(s["addr"]), h(s["addr"]) + h(s["size"]))
            for s in sections_spec["sections"]
            if s["name"] in _OWNED_INIT_DATA and h(s["size"]) > 0]
        # Per-unit data ranges: data_map.toml exactly tiles every data byte, so
        # a compiler-private aggregate with no named twin can borrow the unique
        # address in its own unit's range that holds the same bytes.
        dm = tomllib.loads(
            (ROOT / "config" / version / "data_map.toml").read_text())
        self._unit_ranges = borrow_scan_ranges(dm.get("range", []))

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

    def _value8_at(self, addr):
        off = addr - self._delta
        return struct.unpack_from("<Q", self._elf, off)[0]

    def _bytes_at(self, addr, size):
        off = addr - self._delta
        return bytes(self._elf[off:off + size])

    def _in_init_data(self, addr, size):
        return any(lo <= addr and addr + size <= hi
                   for lo, hi in self._data_ranges)

    def _in_named_object(self, addr):
        """True when a sized data registry symbol covers addr."""
        if self._objects is None:
            from status import load_data_registry
            self._objects = sorted(
                (s["address"], s["address"] + s["size"])
                for syms in load_data_registry(ROOT / "config" / self._version).values()
                for s in syms.values() if s.get("size"))
        i = bisect.bisect_right(self._objects, (addr, float("inf"))) - 1
        return i >= 0 and self._objects[i][0] <= addr < self._objects[i][1]

    def _scan_addr(self, want, unit_dir, align):
        """Addresses in unit_dir's data ranges, at `align`, whose bytes ==
        want (each confirmed to lie in file-backed initialized data)."""
        hits = []
        for start, end in self._unit_ranges.get(unit_dir, []):
            a = start + (-start % align)
            while a + len(want) <= end:
                if self._in_init_data(a, len(want)) \
                        and self._bytes_at(a, len(want)) == want:
                    hits.append(a)
                a += align
        return hits

    def _borrow_addr_in_unit(self, want, unit_dir):
        """Addresses in unit_dir's data ranges whose bytes == want.

        Searched at the aggregate's natural alignment first (largest power of
        two <= its size, capped at 16) so a run of zero padding cannot spoof an
        aligned aggregate; only if that finds nothing does it retry byte-aligned
        so an unaligned string (align 1 in retail) is still located. Any unique
        equal-bytes address is byte-correct regardless -- the data itself comes
        from the per-range object, this only picks the symbol to resolve."""
        nat = 1
        while nat * 2 <= len(want) and nat < 16:
            nat *= 2
        for align in (nat, 1):
            hits = self._scan_addr(want, unit_dir, align)
            if hits:
                return hits
            if align == 1:
                break
        return []

    def map_owned_data(self, slice_text, want, context, unit_dir=None):
        """Retail owned-data symbol a compiler-private initialized aggregate
        ($LCn) can borrow instead of owning data.

        Prefer a named symbol the function's retail slice already references
        (strongest evidence: retail names and uses it). Failing that, fall back
        to the unique address in the function's own unit data range holding the
        same bytes, named `D_<vram>` so the image link auto-resolves it. Either
        way the hybrid stays .text-only and the bytes come from the per-range
        data object at their exact retail address."""
        named = set()
        for sym in set(_SYM_REF_RE.findall(slice_text)):
            addr = self._address_of(sym)
            if addr is None or not self._in_init_data(addr, len(want)):
                continue
            if self._bytes_at(addr, len(want)) == want:
                named.add(sym)
        if len(named) > 1:
            # A short aggregate (`""`, a zero word) also starts named objects
            # (or fields of them) the slice touches; the compiler-private
            # slot is the one address no registry object covers.
            private = {s for s in named if not self._in_named_object(
                self._address_of(s))}
            if len(private) == 1:
                named = private
        if len(named) == 1:
            return next(iter(named))
        if len(named) > 1:
            raise HybridError(
                f"{context}: local aggregate maps ambiguously to "
                f"{sorted(named)}")
        if unit_dir is not None:
            hits = self._borrow_addr_in_unit(want, unit_dir)
            if len(hits) == 1:
                return f"D_{hits[0]:08x}"
            if len(hits) > 1:
                raise HybridError(
                    f"{context}: local aggregate (0x{want.hex()}) occurs at "
                    f"multiple addresses in {unit_dir} "
                    f"({', '.join(f'0x{a:08x}' for a in hits)})")
        raise HybridError(
            f"{context}: local initialized aggregate (0x{want.hex()}) matches "
            f"no owned-data symbol the retail slice references and no unique "
            f"address in {unit_dir or 'the unit'}")

    def map_jump_tables(self, slice_text, counts, context):
        """Ordered retail `jtbl_`/`jpt_` slots the compiler's switch jump
        tables borrow (borrow-by-structure: a table holds relocations, not
        bytes, so it cannot be byte-matched). `counts` lists the entry count
        of every compiled table in code appearance order; the retail slots
        are taken in slice appearance order -- the compiler emits switch
        tables in source order, which is the order the retail code addresses
        them: the same positional argument map_for_slice makes for duplicate
        .lit4 slots. Each slot must have room for its table's 4-byte entries;
        the compiler's copies are dropped and the whole-unit byte gate
        verifies the assignment (the retail tables, byte-exact in the
        per-range data objects, point at the matching function's labels)."""
        order = []
        seen = set()
        for sym in _SYM_REF_RE.findall(slice_text):   # appearance order
            if sym in seen or not _JTBL_NAME_RE.match(sym):
                continue
            seen.add(sym)
            order.append(sym)
        if len(order) != len(counts):
            raise HybridError(
                f"{context}: {len(counts)} switch jump table(s) in the "
                f"compiled code but {len(order)} jtbl_/jpt_ symbol(s) in the "
                f"retail slice ({order}); the C switch shapes do not line up "
                f"with retail")
        for sym, count in zip(order, counts):
            addr = self._address_of(sym)
            if addr is None or not self._in_init_data(addr, count * 4):
                raise HybridError(
                    f"{context}: retail slot {sym} lacks room for the "
                    f"{count}-entry compiled jump table it would borrow")
        return order

    def map_for_slice(self, slice_text, context):
        """Ordered {float32 bits: [retail .lit4 symbols]} for one slice.

        Distinct slots that hold the same value (two `li.s 9.58738e-05` in
        different branches, each with its own slot) are kept in slice
        appearance order, so the k-th pool-bound `li.s` of a value borrows the
        k-th slot -- the compiler emits them in the same order retail does, and
        the whole-unit byte gates verify the assignment."""
        by_bits = {}
        seen = set()
        for sym in _GPREL_SYM_RE.findall(slice_text):   # appearance order
            if sym in seen:
                continue
            addr = self._address_of(sym)
            if addr is None or not (self._lo <= addr < self._hi):
                continue
            seen.add(sym)
            by_bits.setdefault(self._value_at(addr), []).append(sym)
        return by_bits

    def map_for_slice8(self, slice_text, context):
        """Ordered {float64 bits: [retail .lit8 symbols]} for one slice --
        the .lit8/`li.d` analogue of map_for_slice."""
        by_bits = {}
        seen = set()
        for sym in _GPREL_SYM_RE.findall(slice_text):   # appearance order
            if sym in seen:
                continue
            addr = self._address_of(sym)
            if addr is None or not (self._lo8 <= addr < self._hi8):
                continue
            seen.add(sym)
            by_bits.setdefault(self._value8_at(addr), []).append(sym)
        return by_bits

    def map_gpr_pool8(self, slice_text, context):
        """Ordered {float64 bits: [retail symbols]} for the 64-bit constants
        one slice loads out of pooled initialized data.

        Sony's `as` pools a `li.d $gpr,<decimal>` whose image would cost more
        to build inline than to load, putting it in the unit's own .rodata and
        addressing it `lui $at,%hi(SYM)` + `ld $reg,%lo(SYM)($at)` -- unlike
        .lit4/.lit8 this is %hi/%lo, not gp-relative, so map_for_slice8 never
        sees it. The retail slice names the slots, so the hybrid borrows them
        rather than owning the bytes. Slots stay in slice appearance order and
        are consumed positionally, exactly like map_for_slice: gcc 2.95 does
        not dedup equal doubles, so three source-level `0.249` literals get
        three identical slots and each load addresses its own (FindLocalCrate:
        D_0061EE90/98/A0).

        A candidate must be 8-aligned (`ld`), lie in file-backed initialized
        data, and hold exactly the wanted image. Only the 64-bit form borrows:
        a 4-byte float image could plausibly coincide with unrelated data the
        slice happens to reference, 8 bytes cannot. Only symbols the slice
        actually `ld`s count: a table whose base the slice merely takes
        (`lui`/`addiu`) can hold the wanted image in its first element without
        retail ever loading it from there (__ieee754_pow: bp[0] = 1.0 at
        D_0062B530, while retail builds its 1.0 inline). Constants with no
        such slot fall through to the inline `dli` expansion, and the
        whole-unit byte gate is the arbiter either way."""
        by_bits = {}
        seen = set()
        for sym in _LD_LO_SYM_RE.findall(slice_text):   # appearance order
            if sym in seen:
                continue
            seen.add(sym)
            addr = self._address_of(sym)
            if addr is None or addr % 8 or not self._in_init_data(addr, 8):
                continue
            by_bits.setdefault(self._value8_at(addr), []).append(sym)
        return by_bits


def _sonyize(line):
    m = _MOVE_RE.match(line)
    if m:
        return f"{m.group(1)}daddu{m.group(2)}{m.group(3)},{m.group(4)},$0"
    m = _BREAK_RE.match(line)
    if m:
        return f"{m.group(1)}break{m.group(2)}0,{m.group(3)}"
    m = _JREG_RE.match(line)
    if m:
        return f"{m.group(1)}jr{m.group(2)}{m.group(3)}"
    m = _CVTWS_RE.match(line)
    if m:
        return f"{m.group(1)}trunc.w.s\t$f{m.group(2)},$f{m.group(3)}"
    m = _VU_OP_RE.match(line)
    if m:
        return m.group(1) + _VU_SPECIAL_RE.sub(r"$\1", m.group(2))
    return line


def _writes_fpr_via_mtc1(line):
    """Does this line assemble to an mtc1 as its last instruction? (_MTC1_RE)"""
    return bool(_MTC1_RE.match(line) or _LIS_INLINE_RE.match(line))


def _fpr_reads(line):
    """FPR numbers one compiled instruction line reads (see _FPR_READ_ALL)."""
    m = _MNEMONIC_RE.match(line)
    if not m:
        return set()
    mnemonic = m.group(1)
    operands = line[m.end():].split("#", 1)[0]
    regs = [int(r) for r in _FPR_RE.findall(operands)]
    if not regs or mnemonic in _FPR_NO_READ:
        return set()
    if mnemonic in _FPR_READ_ALL or mnemonic.startswith("c."):
        return set(regs)
    return set(regs[1:])


def _mtc1_hazard(line, following):
    """True if SN's `as` pads a nop between `line` and `following`."""
    m = _MTC1_DEST_RE.match(line)
    if not m:
        return False
    pair = int(m.group(1)) // 2
    return any(r // 2 == pair for r in _fpr_reads(following))


def _gpr_reads(line):
    """GPRs one instruction line reads."""
    m = _MNEMONIC_RE.match(line)
    if not m:
        return set()
    mnemonic = m.group(1)
    operands = line[m.end():].split("#", 1)[0]
    regs = _GPR_RE.findall(operands)
    if mnemonic in _GPR_READ_ALL or _is_branch(line):
        return set(regs)
    return set(regs[1:])


def _mfc1_hazard(line, following):
    m = _MFC1_RE.match(line)
    return bool(m) and m.group(1) in _gpr_reads(following)


_SLOT_LABELS = itertools.count()


def _filled_slot_lines(seg):
    """Line indices inside ee-gcc's `.set noreorder` branch blocks (not the
    branchless ones _rewrite_lis adds)."""
    lines, block, noreorder = set(), [], False
    for index, line in enumerate(seg):
        m = _SET_REORDER_RE.match(line)
        if m:
            if noreorder and m.group(1) is None and any(
                    _is_branch(seg[i]) for i in block):
                lines.update(block)
            noreorder, block = m.group(1) is not None, []
        elif noreorder:
            block.append(index)
    return lines


def _sce_keeps_slot(history):
    """Profile `sce`: retail leaves a reorder-mode branch slot empty where the
    decompals `as` swaps, if the instruction two back was in a noreorder
    block (old GAS prev_prev_insn_unreordered; _sceCd_ncmd_prechk 0x00266260,
    sceSifResetIop 0x002808BC) or the previous one is mflo/mfhi (_calloc_r
    0x00278620). `history`: (mnemonic, in noreorder block), newest last."""
    if len(history) >= 2 and history[-2][1]:
        return True
    return bool(history) and history[-1][0] in ("mflo", "mfhi")


def _sonyize_seg(seg, profile="default"):
    """`_sonyize` over one function segment, plus the nops SN's `as` writes and
    the decompals `as` does not: the mtc1 pair hazard (after any alignment,
    before any label), the mfc1 interlock across an alignment, and unfilled
    branch delay slots (for profile `sce` only per _sce_keeps_slot)."""
    out = []
    reorder = True
    sce = profile == "sce"
    pad_slots = not sce
    reorder_before = None
    nop_after = None
    history = []
    filled = _filled_slot_lines(seg) if sce else set()
    skip = set()
    for index, line in enumerate(seg):
        if index in skip:
            continue
        if index == reorder_before:
            out.append("\t.set\treorder")
            reorder_before = None
        m = _SET_REORDER_RE.match(line)
        if m:
            reorder = m.group(1) is None
            out.append(line)
            continue
        mnemonic = _MNEMONIC_RE.match(line)
        if reorder and pad_slots and _is_branch(line):
            out.append("\t.set\tnoreorder")
            out.append(_sonyize(line))
            out.append("\tnop")
            out.append("\t.set\treorder")
            continue
        if reorder and sce and _is_branch(line) and _sce_keeps_slot(history):
            # A labelled branch is never swapped into; unlike a noreorder nop
            # this leaves the next branch free to swap.
            out.append(f"$Lsony{next(_SLOT_LABELS)}:")
            history.append((mnemonic.group(1), False))
            out.append(_sonyize(line))
            continue
        if mnemonic:
            history.append((mnemonic.group(1), index in filled))
        cop2 = _COP2_MOVE_RE.match(line)
        if cop2 and reorder:
            next_index = index + 1
            while next_index < len(seg) and _is_noise(seg[next_index]):
                next_index += 1
            following = seg[next_index] if next_index < len(seg) else ""
            if following and not _is_branch(following) and (
                    cop2.group(2) in _gpr_reads(following)
                    if cop2.group(1) != "ctc2"
                    else _COP2_OP_RE.match(following)):
                branch = next_index + 1
                while branch < len(seg) and _is_noise(seg[branch]):
                    branch += 1
                if (sce and branch < len(seg) and _is_branch(seg[branch])
                        and not _MNEMONIC_RE.match(following).group(1)
                        in ("qmfc2", "cfc2")):
                    # The retail `sce` assembler swaps the COP2 op into the
                    # branch slot (sceDevVu0Exec 0x002636D4).
                    out += ["\t.set\tnoreorder", _sonyize(line),
                            _sonyize(seg[branch]), _sonyize(following),
                            "\t.set\treorder"]
                    skip.update((next_index, branch))
                    continue
                out.append("\t.set\tnoreorder")
                reorder_before = next_index + 1
        out.append(_sonyize(line))
        if index == nop_after:
            out.append("\tnop")
            nop_after = None
        mfc1 = _MFC1_RE.match(line)
        if _writes_fpr_via_mtc1(line) or (mfc1 and pad_slots):
            next_index = index + 1
            last_align = None
            while next_index < len(seg) and (
                    _is_noise(seg[next_index])
                    or _LOCAL_LABEL_RE.match(seg[next_index])
                    or _ALIGN_RE.match(seg[next_index])):
                if _ALIGN_RE.match(seg[next_index]):
                    last_align = next_index
                next_index += 1
            if next_index >= len(seg):
                continue
            following = seg[next_index]
            hazard = (_mfc1_hazard(line, following) if mfc1
                      else _mtc1_hazard(line, following))
            if last_align is not None and reorder and pad_slots:
                # The decompals `as` pads before the alignment whether or not
                # the next instruction reads the register; SN's `as` pads
                # after it, and only for a real hazard (none at
                # DrawZoffaUFOs). Not for profile `sce`.
                out[-1:] = ["\t.set\tnoreorder", out[-1]]
                reorder_before = next_index
                if hazard:
                    nop_after = last_align
            elif hazard and not mfc1:
                out.append("\tnop")
    if reorder_before is not None:
        out.append("\t.set\treorder")
    return out


# Profile `sce`: the retail 2.9 `as` pads a backward PC-relative branch (not
# `j`) with nops until five words precede it from the target label (the R5900
# short-loop erratum), unless a branch or jump lies in the counted words.
# ee-gcc pads true short loops itself, but the assembler misjudges the length
# when the label sits in an earlier frag of six or more words: it then counts
# only the words from the label to that frag's end (_strtod_r 0x00272934,
# 0x00272D80; probed). A frag ends at an alignment, at `.org` and after every
# `%hi` lui, including the one a symbolic load or `la` macro starts with.
_PCREL_BRANCH_OPS = (_BRANCH_OPS - {"j", "jr", "jal", "jalr"}) | frozenset(
    op + "l" for op in ("beq", "bne", "beqz", "bnez", "blez", "bgtz", "bltz",
                        "bgez", "bltzal", "bgezal", "bc1t", "bc1f"))
_SHORT_LOOP_WORDS = 5
_LISTING_RE = re.compile(r"^\s*(\d+) ([0-9a-f]{4,}) [0-9A-F]+")
_LISTING_MORE_RE = re.compile(r"^\s*(\d+)\s+[0-9A-F]+\s*$")
_SYMBOL_OPERAND_RE = re.compile(r"(?<![\w$%.])[A-Za-z_][\w.]*")
_ORG_RE = re.compile(r"^\s*\.org\b")


def _backward_target(line, labels):
    """The already defined compiler label a PC-relative branch line targets."""
    m = _MNEMONIC_RE.match(line)
    if not m or m.group(1) not in _PCREL_BRANCH_OPS:
        return None
    target = line[m.end():].split("#", 1)[0].split(",")[-1].strip()
    if target.startswith("$L") and target in labels:
        return target
    return None


def _hi_breaks_frag(line, words):
    """Does this instruction line (`words` long) start with a `%hi` lui?"""
    m = _MNEMONIC_RE.match(line)
    if not m or _is_branch(line):
        return False
    operands = line[m.end():].split("#", 1)[0]
    return "%hi(" in operands or (
        words > 1 and bool(_SYMBOL_OPERAND_RE.search(operands)))


def _listing_words(lines, g_flag):
    """{line index: [address, words]} from a decompals `as` listing."""
    import tempfile
    rows = {}
    with tempfile.TemporaryDirectory() as tmp:
        src, lst = Path(tmp) / "unit.s", Path(tmp) / "unit.lst"
        src.write_text("\n".join(lines) + "\n")
        proc = subprocess.run(
            [tool_path(AS), "-EL", "-march=r5900", g_flag, "-no-pad-sections",
             f"-al={lst}", "-o", str(Path(tmp) / "unit.o"), str(src)],
            capture_output=True, text=True)
        if proc.returncode != 0:
            raise HybridError(f"assembling the short-loop listing failed:\n"
                              f"{proc.stderr}")
        for row in lst.read_text().splitlines():
            m = _LISTING_RE.match(row)
            if m:
                rows[int(m.group(1)) - 1] = [int(m.group(2), 16), 1]
                continue
            m = _LISTING_MORE_RE.match(row)
            if m and int(m.group(1)) - 1 in rows:
                rows[int(m.group(1)) - 1][1] += 1
    return rows


def _sce_short_loop_pads(lines, g_flag):
    """Profile `sce`: write out the short-loop nops the retail `as` adds (see
    _PCREL_BRANCH_OPS), measured on a decompals listing. Retail slices branch
    to `.L` labels and already carry their nops; only compiler `$L` branches
    are considered."""
    lines = [l for line in lines for l in line.split("\n")]
    seen = set()
    for line in lines:
        label = _LOCAL_LABEL_RE.match(line)
        if label:
            seen.add(label.group(1))
        if _backward_target(line, seen):
            break
    else:
        return lines
    words = _listing_words(lines, g_flag)
    labels = {}                 # label -> (address, frag)
    starts, ends = {}, {}       # frag -> its first / past-last address
    frag, started, pending = 0, False, []
    cur = shift = 0
    in_data, reorder = False, True
    pads, branches = {}, []     # branches: word addresses, ascending
    for index, line in enumerate(lines):
        if _ENTER_DATA_RE.match(line):
            in_data = True
        elif _LEAVE_DATA_RE.match(line):
            in_data = False
        m = _SET_REORDER_RE.match(line)
        if in_data or m:
            reorder = m.group(1) is None if m else reorder
            continue
        label = _LOCAL_LABEL_RE.match(line)
        if label:
            if started:
                labels[label.group(1)] = (cur, frag)
            else:
                pending.append(label.group(1))
            continue
        if _ALIGN_RE.match(line) or _ORG_RE.match(line):
            if _ORG_RE.match(line):
                shift = 0       # absolute: earlier pads ate into the fill
            if started:
                ends[frag], frag, started = cur, frag + 1, False
            continue
        if index not in words or not _MNEMONIC_RE.match(line):
            continue
        addr, count = words[index]
        addr += shift
        if not started:
            starts[frag], started = addr, True
        for name in pending:
            labels[name] = (addr, frag)
        pending = []
        target = _backward_target(line, labels)
        if target:
            at, home = labels[target]
            end = ends[home] if home != frag and \
                ends[home] - starts[home] >= 4 * 6 else addr
            loop = (end - at) // 4
            if loop < _SHORT_LOOP_WORDS and \
                    bisect.bisect_left(branches, at) == \
                    bisect.bisect_left(branches, end):
                pads[index] = (_SHORT_LOOP_WORDS - loop, reorder)
                shift += 4 * (_SHORT_LOOP_WORDS - loop)
                addr += 4 * (_SHORT_LOOP_WORDS - loop)
        if _is_branch(line) or _MNEMONIC_RE.match(line).group(1) in \
                _PCREL_BRANCH_OPS:
            branches.append(addr)
        cur = addr + 4 * count
        if _hi_breaks_frag(line, count):
            ends[frag], frag = addr + 4, frag + 1
            starts[frag] = addr + 4
    if not pads:
        return lines
    out = []
    for index, line in enumerate(lines):
        if index in pads:
            n, in_reorder = pads[index]
            nops = ["\tnop"] * n
            out += ["\t.set\tnoreorder", *nops, "\t.set\treorder"] \
                if in_reorder else nops
        out.append(line)
    return out


def _unit_end(unit_dir, version="pal103"):
    """End address of a unit: the next unit's start, or the .text end."""
    from declib.asmtext import load_text_target
    from declib.tu import load_tu_runs
    runs = load_tu_runs()
    index = int(unit_dir.split("-")[1])
    starts = [a for u, a in runs if u == index]
    if not starts:
        raise HybridError(f"unit {unit_dir} not in the TU runs")
    later = [a for _u, a in runs if a > starts[0]]
    if later:
        return min(later)
    _elf, text_addr, _off, text_size = load_text_target(version)
    return text_addr + text_size


def _unit_start(unit_dir, version="pal103"):
    """Where a unit's object starts: its TU run (asmdma's run begins at the
    included nu2api.i code), or .text itself for the first unit (crt0 carries
    the loader words ahead of _start)."""
    from declib.asmtext import load_text_target
    from declib.tu import load_tu_runs
    runs = load_tu_runs()
    index = int(unit_dir.split("-")[1])
    start = next(a for u, a in runs if u == index)
    if start == min(a for _u, a in runs):
        _elf, text_addr, _off, _size = load_text_target(version)
        return text_addr
    return start


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


def pooled_gpr8(bits):
    """Does the assembler pool a `li.d $gpr` image instead of building it?

    Inline only when each 32-bit half has a zero 16-bit half (lui/ori plus
    shifts, as GNU as decides); everything else is an 8-byte `.rodata`
    literal (kprintf: 0.1 and 1e6 pooled, 1.0 and 10.0 inline)."""
    def simple(word):
        return (word >> 16) == 0 or (word & 0xFFFF) == 0
    return not (simple(bits >> 32) and simple(bits & 0xFFFFFFFF))


class _Pool:
    """The literal pools a whole-unit hybrid emits itself, in assembly order:
    one `.lit4` slot per pool-bound li.s and one 8-byte `.rodata` literal per
    pooled `li.d $gpr`, never merged (retail: every .lit4 word is used once).

    `entries` keeps, per function, the kind of every pool decision in line
    order ("lit4", "gpr8", or "dli" for an inline li.d) with its label, so it
    can be paired with the retail slots the normal borrow pass finds."""

    def __init__(self):
        self.function = None
        self.entries = {}
        self.count = 0

    def add(self, kind):
        label = f"$Lpool{self.count}"
        self.count += 1
        self.entries.setdefault(self.function, []).append((kind, label))
        return label

    def inline(self):
        self.entries.setdefault(self.function, []).append(("dli", None))


def _pool_block(section, align, label, value):
    """Data lines for one pool literal, spliced in right after its load."""
    return [f"\t{section}", f"\t.align\t{align}", f"{label}:",
            *(f"\t.word\t0x{w:08X}" for w in
              struct.unpack(f"<{len(value) // 4}I", value)),
            "\t.previous"]


def _rewrite_lis(seg, version, unit_dir, name, addr, mapper_box, pool=None,
                 trace=None):
    """Rewrite pool-bound `li.s`/`li.d` lines in one compiled segment.

    ee-gcc defers float/double materialization to the assembler: a constant
    whose image has a zero low half (li.s: low 16 bits; li.d: low 32 bits)
    becomes an inline lui+mtc1 (no data -- pass it through), and anything else
    is a literal-pool entry the hybrid must not own. Retail holds every such
    constant in .lit4/.lit8 addressed gp-relative and the function's own retail
    slice names the exact slot, so pool-bound loads are rewritten to address
    the retail slot by symbol (`lwc1`/`ldc1` + `.extern` -> gp_rel).

    Returns (lines, extern_directives). Distinct slots holding the same value
    are consumed positionally in slice-appearance order; the whole-unit byte
    gates verify the assignment, and any constant that cannot be mapped
    unambiguously fails loudly.

    With a `pool` (whole-unit mode, over the whole compiled file) nothing is
    borrowed: each pool-bound constant gets its own `_Pool` label and a data
    block right after the load, so the assembler order of the pools is the
    file's. `trace` (a list) records the retail slot every borrowed load got,
    as (kind, symbol) in line order -- the anchors a whole-unit layout needs.
    """
    if not any("li.s" in l or "li.d" in l for l in seg):
        return seg, []
    maps = {}                      # kind (4/8/"gpr8") -> {bits: [syms]}
    used = {}                      # (kind, bits) -> slots of that value spent
    slice_cache = [None]
    out, externs = [], []

    def slots_for(kind, bits):
        if kind not in maps:
            if mapper_box[0] is None:
                mapper_box[0] = Lit4Mapper(version)
            if slice_cache[0] is None:
                slice_cache[0] = _slice_path(
                    version, unit_dir, name, addr).read_text()
            method = {4: "map_for_slice", 8: "map_for_slice8",
                      "gpr8": "map_gpr_pool8"}[kind]
            maps[kind] = getattr(mapper_box[0], method)(slice_cache[0], name)
        return maps[kind].get(bits, ())

    reorder = True

    def emit_unswappable(lines):
        # Sony's `as` never moves part of a `li.d $gpr` expansion into a
        # following branch delay slot -- it leaves a nop there (__kernel_cos
        # `b` at 0x0026B4B8, __ieee754_rem_pio2 `jal` at 0x0026AB9C). The
        # decompals `as` would swap the last instruction in; one it emitted
        # under noreorder is never a swap candidate, so it fills the slot
        # with a nop instead.
        if reorder:
            out.append("\t.set\tnoreorder")
        out.extend(lines)
        if reorder:
            out.append("\t.set\treorder")

    for line in seg:
        mr = _SET_REORDER_RE.match(line)
        if mr:
            reorder = mr.group(1) is None
        me = _ENT_RE.match(line)
        if me and pool is not None:
            pool.function = me.group(1)
        ms, md = _LIS_RE.match(line), _LID_RE.match(line)
        if ms:
            reg, const, size, load, low = ms.group(1), ms.group(2), 4, "lwc1", \
                0xFFFF
            bits_fn = _float_bits
        elif md:
            reg, const, size, load, low = md.group(1), md.group(2), 8, "ldc1", \
                0xFFFFFFFF
            bits_fn = _double_bits
        else:
            s = line.strip()
            if s.startswith("li.s") or s.startswith("li.d"):
                mg = _LI_GPR_RE.match(line)   # inline GPR immediate, byte-material
                if mg:
                    op, reg, const = mg.groups()
                    try:
                        bits = (_double_bits(const) if op == "li.d"
                                else _float_bits(const))
                    except ValueError:
                        raise HybridError(
                            f"{name}: unparseable {op} constant {const!r}")
                    if pool is not None and op == "li.d":
                        if pooled_gpr8(bits):
                            label = pool.add("gpr8")
                            emit_unswappable([
                                "\t.set\tnoat",
                                f"\tlui\t$at,%hi({label})",
                                f"\tld\t{reg},%lo({label})($at)",
                                "\t.set\tat",
                            ])
                            out.extend(_pool_block(".rdata", 3, label,
                                                   struct.pack("<Q", bits)))
                            continue
                        pool.inline()
                        emit_unswappable([f"\tdli\t{reg},0x{bits:016X}"])
                        continue
                    # Costly images are pooled by Sony's `as` into unit-owned
                    # data instead of built inline; borrow retail's slot when
                    # the function's own slice addresses one holding exactly
                    # these bytes (map_gpr_pool8), positionally per value.
                    slots = slots_for("gpr8", bits) if op == "li.d" else ()
                    k = used.get(("gpr8", bits), 0)
                    if k < len(slots):
                        sym = slots[k]
                        used[("gpr8", bits)] = k + 1
                        if trace is not None:
                            trace.append(("gpr8", sym))
                        # Explicit %hi/%lo, not the `ld $reg,SYM` macro: under
                        # -G8 an 8-byte `.extern` is a small symbol and the
                        # macro would collapse to a single gp-relative load.
                        emit_unswappable([
                            "\t.set\tnoat",
                            f"\tlui\t$at,%hi({sym})",
                            f"\tld\t{reg},%lo({sym})($at)",
                            "\t.set\tat",
                        ])
                        externs.append(f"\t.extern\t{sym}, 8")
                        continue
                    # Emit `dli` (64-bit explicit) for BOTH sizes: `li` on a
                    # high-bit-set float32 image (e.g. -1.0 -> 0xBF800000) would
                    # `lui`-sign-extend into bits 32-63; `dli` sets all 64 bits
                    # deterministically so the register holds exactly the raw
                    # bit pattern, zero-extended. The byte gate is the backstop
                    # if decompals `as` ever expands `dli` unlike retail's SN as.
                    if op == "li.d":
                        if trace is not None:
                            trace.append(("dli", None))
                        emit_unswappable([f"\tdli\t{reg},0x{bits:016X}"])
                    else:
                        out.append(f"\tdli\t{reg},0x{bits:08X}")
                    continue
                raise HybridError(f"{name}: unparseable fp-pool load {s!r}")
            out.append(line)
            continue
        try:
            bits = bits_fn(const)
        except ValueError:
            raise HybridError(
                f"{name}: unparseable li.{'s' if size == 4 else 'd'} "
                f"constant {const!r}")
        if bits & low == 0:            # low half zero -> inline lui+mtc1
            out.append(line)
            continue
        if pool is not None:
            if size != 4:
                raise HybridError(f"{name}: li.d into an FPR (.lit8) is not "
                                  f"supported in whole-unit mode")
            label = pool.add("lit4")
            out.append(f"\tlwc1\t{reg},%gp_rel({label})($28)")
            out.extend(_pool_block(".section\t.lit4", 2, label,
                                   struct.pack("<I", bits)))
            continue
        slots = slots_for(size, bits)
        k = used.get((size, bits), 0)
        if k >= len(slots):
            raise HybridError(
                f"{name}: li.{'s' if size == 4 else 'd'} constant {const} "
                f"(0x{bits:0{size * 2}X}) has no .lit{size} slot among the "
                f"retail function's gp references")
        sym = slots[k]
        used[(size, bits)] = k + 1
        if trace is not None:
            trace.append(("lit4" if size == 4 else "lit8", sym))
        out.append(f"\t{load}\t{reg},{sym}")
        externs.append(f"\t.extern\t{sym}, {size}")
    return out, externs


def _rewrite_local_data(seg, local_data, version, unit_dir, name, addr,
                        mapper_box, trace=None):
    """Rewrite references to a compiler-private initialized aggregate ($LCn)
    or switch jump table ($Ln) onto the retail slot it borrows.

    A constant aggregate borrows the owned-data slot holding the same bytes; a
    `_JumpTable` borrows the retail jtbl_/jpt_ slot the slice addresses. Returns
    (lines, extern_directives); segments that touch no captured local label
    pass through untouched, so this is free for the common case. `trace` (a
    dict) collects every {label: retail symbol} decision; a label two
    functions map onto different slots fails loudly."""
    if not local_data:
        return seg, []
    # Match a private label only on word boundaries, exactly as the rewrite
    # below does -- a plain substring test wrongly attributes another
    # function's `$L161` jump table to a function that merely owns a `$L1610`
    # branch label (`$L161` is a prefix of `$L1610`), which then fails
    # map_jump_tables against a slice that has no such table.
    label_re = {lbl: re.compile(r"(?<![\w$.])" + re.escape(lbl) + r"(?![\w$.])")
                for lbl in local_data}
    used = {lbl for lbl in local_data
            if any(label_re[lbl].search(line) for line in seg)}
    if not used:
        return seg, []
    if mapper_box[0] is None:
        mapper_box[0] = Lit4Mapper(version)
    slice_text = _slice_path(version, unit_dir, name, addr).read_text()
    # Order labels by first use in the compiled code so jump tables borrow
    # the retail slots positionally (compiled order == slice order).
    first_use = {}
    for idx, line in enumerate(seg):
        for lbl in used:
            if lbl not in first_use and label_re[lbl].search(line):
                first_use[lbl] = idx
    ordered = sorted(used, key=lambda lbl: first_use[lbl])
    jt_labels = [lbl for lbl in ordered
                 if isinstance(local_data[lbl], _JumpTable)]
    jt_syms = {}
    if jt_labels:
        syms = mapper_box[0].map_jump_tables(
            slice_text, [local_data[lbl].count for lbl in jt_labels], name)
        jt_syms = dict(zip(jt_labels, syms))
    externs = []
    rename = {}
    for lbl in ordered:
        val = local_data[lbl]
        if isinstance(val, _JumpTable):
            sym = jt_syms[lbl]
            externs.append(f"\t.extern\t{sym}, {val.count * 4}")
        else:
            sym = mapper_box[0].map_owned_data(slice_text, val, name, unit_dir)
            externs.append(f"\t.extern\t{sym}, {len(val)}")
        rename[lbl] = sym
        # Same address under two spellings (D_0061F460 / D_0061f460) is one borrow.
        if (trace is not None and trace.setdefault(lbl, sym) != sym
                and _symbol_address(trace[lbl], version)
                != _symbol_address(sym, version)):
            raise HybridError(f"{name}: {lbl} borrows {sym}, another function "
                              f"borrows {trace[lbl]}")
    out = []
    for line in seg:
        for lbl, sym in rename.items():
            line = label_re[lbl].sub(sym, line)
        out.append(line)
    return out, externs


# --------------------------------------------------------------------------- #
# Data-from-C: per-symbol data pieces                                         #
# --------------------------------------------------------------------------- #

# Data sections whose owned ranges a hybrid with C data provides as pieces
# (everything with file bytes; .sbss/.bss stay with the NOLOAD placeholders).
PIECE_SECTIONS = (".data", ".rodata", ".lit4", ".sdata")
# Sections C may own symbols in for now (.sbss/.bss: data/bss).
C_DATA_SECTIONS = (".data", ".rodata", ".sdata")
_RETAIL_CACHE = {}


def piece_name(section, start):
    """Input-section name of the hybrid piece for one owned data range."""
    return f"{section}.split_{start:08x}"


def _retail(version):
    if version not in _RETAIL_CACHE:
        from declib.target import load_target
        _RETAIL_CACHE[version] = load_target(version)
    return _RETAIL_CACHE[version]


def _retail_bytes(version, section, lo, hi):
    elf, spec, _pt = _retail(version)
    sec = next(s for s in spec["sections"] if s["name"] == section)
    off = h(sec["offset"]) + lo - h(sec["addr"])
    return elf[off:off + hi - lo]


def unit_data_ranges(version, unit_dir):
    """The unit's owned data_map ranges that carry file bytes, by address."""
    dm = tomllib.loads((ROOT / "config" / version / "data_map.toml").read_text())
    return sorted((r for r in dm["range"] if r["owner"] == unit_dir
                   and r["section"] in PIECE_SECTIONS), key=lambda r: r["start"])


def data_states(data):
    """{address: state} of a manifest's [[data]] table."""
    return {int(e["id"].split(":")[2], 16): e["state"]
            for e in data.get("data", [])}


def has_c_data(data):
    """True when a unit's hybrid carries data pieces (a `matching` entry)."""
    return "matching" in data_states(data).values()


def _registry_map(items, version, unit_dir):
    """[(item, registry symbol)] for the compiled named data objects.

    The k-th compiled `stem.N` is the k-th registry function static with
    that stem in address order; every object must be a registry symbol of the
    unit, in the registry's section."""
    from status import load_data_registry
    unit_index = int(unit_dir.split("-")[1])
    registry = load_data_registry(ROOT / "config" / version).get(unit_index, {})
    by_name = {s["name"]: s for s in registry.values()
               if s["scope"] != "function_static"}
    stems = {}
    for sym in sorted(registry.values(), key=lambda s: s["address"]):
        if sym["scope"] == "function_static":
            stems.setdefault(sym["stem"], []).append(sym)
    seen, out = {}, []
    for item in items:
        m = FUNCTION_STATIC_RE.match(item.name)
        if m:
            k = seen.get(m.group(1), 0)
            seen[m.group(1)] = k + 1
            candidates = stems.get(m.group(1), [])
            if k >= len(candidates):
                raise HybridError(f"{item.name}: unit {unit_dir} has no function "
                                  f"static #{k + 1} named {m.group(1)}.N")
            sym = candidates[k]
        else:
            sym = by_name.get(item.name)
            if sym is None:
                raise HybridError(f"{item.name}: not a data symbol of {unit_dir} "
                                  f"(config/{version}/data_symbols.toml)")
        if item.section == "COMMON" and sym["section"] in NOBITS_SECTIONS \
                and sym["scope"] == "global":
            # An uninitialised global: the linker allocated it, and the
            # hybrid places it like the unit's other uninitialised data.
            item.section, item.globl = sym["section"], True
        if item.section != sym["section"]:
            raise HybridError(
                f"data_section_mismatch: {item.name} compiles into {item.section}, "
                f"retail {sym['name']} is in {sym['section']} (check const and "
                f"the profile's -G)")
        out.append((item, sym))
    return out


def _plan_data(items, data, version, unit_dir, report):
    """Map compiled data objects onto the unit's registry symbols.

    Returns (rename, emit): label renames for every compiled data object
    (a function static's compiler number becomes the retail one; a dropped
    static's references go to `D_<VRAM>`, the retail address, so they
    resolve like any retail datum) and the [(DataItem, symbol)] the hybrid
    emits -- the `matching` entries; everything else is dropped and stays
    retail bytes. The k-th compiled `stem.N` is the k-th registry function
    static with that stem in address order."""
    if not items:
        if not report:
            missing = [a for a, s in data_states(data).items() if s == "matching"]
            if missing:
                raise HybridError(f"matching data at {', '.join(f'0x{a:08x}' for a in missing)} "
                                  f"is not defined in {data['source']}")
        return {}, []
    if not data.get("data"):
        raise HybridError(f"data section in compiler output not supported yet: "
                          f"{items[0].name} ({items[0].section}) -- a unit's C "
                          f"defines data only with a [[data]] table "
                          f"(promote.py --init-data)")
    states = data_states(data)
    rename, emit = {}, []
    for item, sym in _registry_map(items, version, unit_dir):
        if not report and states.get(sym["address"]) == "matching":
            if sym["section"] not in C_DATA_SECTIONS:
                raise HybridError(f"{sym['name']}: {sym['section']} data from C "
                                  f"is placed only by the whole-unit layout "
                                  f"(promote.py --complete)")
            if item.size is not None and sym.get("size") is not None \
                    and item.size != sym["size"]:
                raise HybridError(f"{sym['name']}: .size {item.size} != .symtab "
                                  f"size {sym['size']}")
            if item.name != sym["name"]:
                rename[item.name] = sym["name"]
            emit.append((item, sym))
        else:
            new = sym["name"] if sym["scope"] == "global" else f"D_{sym['address']:08X}"
            if new != item.name:
                rename[item.name] = new
    if not report:
        defined = {sym["address"] for _i, sym in emit}
        missing = [a for a, s in states.items() if s == "matching" and a not in defined]
        if missing:
            raise HybridError(f"matching data at {', '.join(f'0x{a:08x}' for a in missing)} "
                              f"is not defined in {data['source']}")
    return rename, emit


def _small_data_externs(items, emit, rename):
    """`.extern NAME, size` for each compiled .sdata/.sbss/.bss object the
    hybrid drops.

    Its references then name an undefined symbol, and only the size keeps the
    assembler addressing it gp-relative (or not), as it did the defined
    object."""
    kept = {id(item) for item, _sym in emit}
    out = []
    for item in items:
        if item.section not in (".sdata",) + NOBITS_SECTIONS or id(item) in kept:
            continue
        if item.size is None:
            raise HybridError(f"{item.name}: small-data object without .size")
        out.append(f"\t.extern\t{rename.get(item.name, item.name)}, {item.size}")
    return out


_EXTERN_DECL_RE = re.compile(r"^\s*\.extern\s+(\S+)\s*,\s*(\d+)\s*$")
_SMALL_SECTIONS = (".sdata", ".sbss")


def _own_static_renames(lines, version, unit_dir, g_flag):
    """{NAME: D_<VRAM>} for each `.extern NAME, size` that names a file-scope
    static of this unit in the data registry.

    Its C keeps the retail name (GCSE hashes symbol names, so the spelling
    steers codegen); the hybrid addresses the retail object. Only the unit's
    own statics qualify; any other undeclared name stays unresolved. The
    declared size must address it as retail does (gp-relative iff small
    data) and, where .symtab sized it, equal that size."""
    from status import load_data_registry
    unit_index = int(unit_dir.split("-")[1])
    registry = load_data_registry(ROOT / "config" / version).get(unit_index, {})
    statics = {s["name"]: s for s in registry.values() if s["scope"] == "static"}
    threshold = int(g_flag[2:])
    rename = {}
    for line in lines:
        m = _EXTERN_DECL_RE.match(line)
        if not m or m.group(1) not in statics:
            continue
        name, size = m.group(1), int(m.group(2))
        sym = statics[name]
        if (0 < size <= threshold) != (sym["section"] in _SMALL_SECTIONS):
            raise HybridError(f"{name}: declared {size} bytes under {g_flag}, "
                              f"retail {name} is {sym['section']}")
        limit = sym.get("size", sym.get("extent"))
        if sym.get("size") is not None and size != sym["size"] \
                or limit is not None and size > limit:
            raise HybridError(f"{name}: declared {size} bytes, retail {name} "
                              f"is {limit}")
        rename[name] = f"D_{sym['address']:08X}"
    return rename


def _directive_size(line):
    """Bytes one data directive emits (a symbolic .word is 4)."""
    word = line.split()[0]
    if word in (".word", ".gpword") and not re.match(
            r"^\s*\.word\s+-?(?:0x[0-9A-Fa-f]+|\d+)(?:\s*,\s*-?(?:0x[0-9A-Fa-f]+|\d+))*\s*(?:#.*)?$",
            line):
        return 4 * len(line.split(None, 1)[1].split("#")[0].split(","))
    return len(_assemble_data_bytes([line]))


def _item_body(item, sym, version):
    """An emitted object's directives. A `.word $LCn` (a pointer to a
    compiler-private string) takes the retail pointer at that offset, as the
    `D_<VRAM>` the image link resolves; a mid-object `.align` is refused."""
    out, offset = [], 0
    for line in item.lines:
        if _ALIGN_ARG_RE.match(line):
            raise HybridError(f"{sym['name']}: .align inside the object is not "
                              f"supported")
        m = _WORD_PRIVATE_RE.match(line)
        if m:
            addr = sym["address"] + offset
            word = struct.unpack("<I", _retail_bytes(version, sym["section"],
                                                     addr, addr + 4))[0]
            out.append(f"\t.word\tD_{word:08X}")
            offset += 4
            continue
        out.append(line)
        offset += _directive_size(line)
    return out


def _is_align_pad(gap, cursor, addr, align):
    """True when the bytes between two C objects are exactly the zero padding
    the compiler's `.align <align>` before the second one emits: the second
    object sits on the first such boundary at or after the end of the first."""
    if not align or any(gap):
        return False
    unit = 1 << align
    return addr == (cursor + unit - 1) // unit * unit


def _data_pieces(emit, unit_dir, version, bin_dir):
    """(lines, pieces, retail) for a hybrid with C data.

    One piece per owned range with file bytes (`<sec>.split_<vram8>`): the
    emitted objects at their registry addresses (`.org`), everything else as
    `.incbin` of the retail bytes, which gen_hybrid cuts from the ELF itself.
    `.align 0` keeps the assembler from padding or moving labels. Returns the
    piece list [{name, section, start, end, objects}] and the retail byte
    ranges."""
    lines, pieces, retail = [], [], []
    bin_dir.mkdir(parents=True, exist_ok=True)
    by_range = {}
    ranges = unit_data_ranges(version, unit_dir)
    for item, sym in emit:
        rng = next((r for r in ranges if r["start"] <= sym["address"] < r["end"]
                    and r["section"] == sym["section"]), None)
        size = sym.get("size") or item.size
        if rng is None or size is None or sym["address"] + size > rng["end"]:
            raise HybridError(f"{sym['name']}: not inside one owned range of "
                              f"{unit_dir} (data_map.toml)")
        by_range.setdefault(rng["start"], []).append((sym["address"], size, item, sym))
    for rng in ranges:
        start, end, section = rng["start"], rng["end"], rng["section"]
        name = piece_name(section, start)
        blob = bin_dir / f"{name}.bin"
        raw = _retail_bytes(version, section, start, end)
        blob.write_bytes(raw)
        flags = "a" if section == ".rodata" else "aw"
        lines += [f'\t.section\t{name},"{flags}",@progbits', "\t.align\t0"]
        cursor, objects = start, []
        for addr, size, item, sym in sorted(by_range.get(start, []),
                                            key=lambda t: t[0]):
            if item.align and addr % (1 << item.align):
                raise HybridError(f"{sym['name']}: .align {item.align} does not "
                                  f"hold at 0x{addr:08x}")
            if addr < cursor:
                raise HybridError(f"{sym['name']}: overlaps the object before it")
            if addr > cursor and _is_align_pad(raw[cursor - start:addr - start],
                                                 cursor, addr, item.align):
                # The compiler's own `.align` padding before this object.
                lines.append(f"\t.space\t{addr - cursor}")
            elif addr > cursor:
                lines.append(f'\t.incbin\t"{blob.as_posix()}",{cursor - start},'
                             f'{addr - cursor}')
                retail.append((cursor, addr))
            lines.append(f"\t.org\t0x{addr - start:X}")
            if item.globl:
                lines.append(f"\t.globl\t{sym['name']}")
            lines += [f"\t.type\t{sym['name']},@object",
                      f"\t.size\t{sym['name']},{size}", f"{sym['name']}:"]
            lines += _item_body(item, sym, version)
            objects.append({"name": sym["name"], "address": addr, "size": size,
                            "sized_by": "registry" if sym.get("size") else "compiler"})
            cursor = addr + size
            lines.append(f"\t.org\t0x{cursor - start:X}")
        if cursor < end:
            lines.append(f'\t.incbin\t"{blob.as_posix()}",{cursor - start},'
                         f'{end - cursor}')
            retail.append((cursor, end))
        pieces.append({"name": name, "section": section, "start": start,
                       "end": end, "objects": objects})
    return lines, pieces, retail


_STRING_DIRECTIVE_RE = re.compile(r"^\s*\.(?:ascii|asciz|asciiz|string)\b")


def _relabel_items(lines, rename, dups):
    """Both relabels over a data object's directives; the text of a string
    directive is never a symbol ("NuNodeRead: ..." keeps its name)."""
    out = _relabel(_relabel_data(lines, rename), dups)
    return [old if _STRING_DIRECTIVE_RE.match(old) else new
            for old, new in zip(lines, out)]


def _relabel_data(lines, rename):
    """Rewrite whole data-symbol names (`lut.6` never matches inside
    `lut.60`, `lut` never inside `lut.6`)."""
    if not rename:
        return lines
    pattern = re.compile(r"(?<![\w.$])(" + "|".join(
        map(re.escape, sorted(rename, key=len, reverse=True))) + r")(?![\w$.])")
    return [pattern.sub(lambda m: rename[m.group(1)], l) for l in lines]


# --------------------------------------------------------------------------- #
# Whole-unit mode (data/private)                                              #
# --------------------------------------------------------------------------- #

def whole_unit(data):
    """True when the hybrid supplies every data byte of the unit from C.

    That is every `complete` unit. Its compiler-private data ($LC strings and
    aggregates, jump tables, the `li.d` GPR pool and the .lit4 pool) is laid
    out the way the assembler did and emitted into the unit's pieces instead
    of borrowing retail slots; bytes it emits outside its owned ranges sit in
    adjacent data-map gaps and are its `[[claim]]`s."""
    return bool(data.get("complete"))


def provides_data(data):
    """True when the unit's hybrid is its data provider in the image link."""
    return has_c_data(data) or whole_unit(data)


def manifest_claims(data):
    """[(section, start, end)] of a manifest's `[[claim]]` table."""
    return sorted((c["section"], c["start"], c["end"])
                  for c in data.get("claim", []))


def claim_text(claims):
    """`[[claim]]` blocks for [(section, start, end)] (promote.py writes them)."""
    lines = []
    for section, start, end in claims:
        lines += ["[[claim]]", f'section = "{section}"',
                  f"start = 0x{start:08x}", f"end = 0x{end:08x}", ""]
    return "\n".join(lines)


_NUMBER_RE = re.compile(r"^-?(?:0x[0-9A-Fa-f]+|\d+)$")
# The MIPS assembler aligns these to their size unless `.align 0` is in
# effect, dragging a label defined right before them along.
_NATURAL_ALIGN = {".half": 1, ".hword": 1, ".short": 1, ".word": 2,
                  ".gpword": 2, ".long": 2, ".float": 2, ".dword": 3,
                  ".quad": 3, ".double": 3}


def _first_directive(item):
    for line in item.lines:
        s = line.strip()
        if s and not _ALIGN_ARG_RE.match(line):
            return s.split()[0]
    return None


def _item_atoms(item, offset):
    """(atoms, size) of a compiled data object placed at section offset
    `offset`. An atom is (offset, bytes) or (offset, ("word", expression)) --
    a symbolic `.word` stays a relocation. Interior `.align` and the natural
    alignment of `.half`/`.word`/`.dword` pad relative to the section, as the
    assembler does."""
    atoms, cur = [], offset

    def pad_to(log):
        nonlocal cur
        pad = -cur % (1 << log)
        if pad:
            atoms.append((cur, bytes(pad)))
            cur += pad

    for line in item.lines:
        a = _ALIGN_ARG_RE.match(line)
        if a:
            pad_to(int(a.group(1)))
            continue
        raw = line.strip()
        if not raw or raw.startswith("#"):
            continue
        word = raw.split()[0]
        if word in _NATURAL_ALIGN:
            pad_to(_NATURAL_ALIGN[word])
        if word in (".word", ".gpword", ".long"):
            args = [x.strip() for x in raw.split(None, 1)[1].split("#", 1)[0]
                    .split(",") if x.strip()]
            for arg in args:
                if _NUMBER_RE.match(arg):
                    atoms.append((cur, (int(arg, 0) & 0xFFFFFFFF)
                                  .to_bytes(4, "little")))
                else:
                    atoms.append((cur, ("word", arg)))
                cur += 4
            continue
        try:
            blob = _assemble_data_bytes([line])
        except ValueError:
            raise HybridError(f"{item.name}: unsupported data directive "
                              f"{raw!r} (only .word may name a symbol)")
        if blob:
            atoms.append((cur, blob))
            cur += len(blob)
    return atoms, cur - offset


def _symbol_address(sym, version):
    """Retail address of a symbol name, as the image link resolves it."""
    from declib.asmtext import AUTO_NAME_RE, load_symbol_addrs
    addr = load_symbol_addrs(version).get(sym)
    if addr is None:
        m = AUTO_NAME_RE.fullmatch(sym)
        addr = int(m.group(1), 16) if m else None
    return addr


def _retail_anchors(compiled, functions, unit_dir, version):
    """Where retail keeps the compiler-private objects the code references.

    Runs the normal borrow pass over every function -- the retail slot each
    pool load and each private label borrows from the function's own slice --
    without emitting anything. Returns ({label: address}, {function:
    [(kind, address)]}) with the pool decisions in line order."""
    _prologue, segments, local_data = parse_s(compiled, [])
    seg_by_name = dict(segments)
    box, labels, pools = [None], {}, {}
    for addr, name, _state in functions:
        seg = seg_by_name.get(name)
        if seg is None:
            continue
        trace = []
        _rewrite_lis(seg, version, unit_dir, name, addr, box, trace=trace)
        _rewrite_local_data(seg, local_data, version, unit_dir, name, addr,
                            box, trace=labels)
        pools[name] = [(kind, sym and _symbol_address(sym, version))
                       for kind, sym in trace]
    return ({lbl: _symbol_address(sym, version) for lbl, sym in labels.items()},
            pools)


class _Placed:
    """One data object of a whole-unit layout at its section offset."""

    def __init__(self, item, sym, offset, atoms, size):
        self.item, self.sym, self.offset = item, sym, offset
        self.atoms, self.size = atoms, size

    @property
    def label(self):
        return self.sym["name"] if self.sym else self.item.name


def _attach_ranks(lines, stream, functions):
    """{id(item): rank}: the retail position of each data object.

    An object travels with the function it is emitted for (the next `.end`
    after it, so a function's strings, jump tables and pools move with it
    when the C defines functions out of address order); a named object above
    the first function stays first, and so do the strings of initialisers
    emitted before such an object; anything after the last function goes
    last."""
    rank = {name: i for i, (_a, name, _s) in enumerate(functions)}
    ends, first_ent = [], None
    for i, line in enumerate(lines):
        m = _ENT_RE.match(line)
        if m and first_ent is None:
            first_ent = i
        m = _END_RE.match(line)
        if m:
            if m.group(1) not in rank:
                raise HybridError(f"{m.group(1)}: compiled but not a function "
                                  f"of the unit (whole-unit layout)")
            ends.append((i, rank[m.group(1)]))
    head = [item for item in stream
            if first_ent is not None and item.line < first_ent]
    last_named = max((item.line for item in head
                      if not item.name.startswith("$")), default=-1)
    out = {}
    for item in head:
        if not item.name.startswith("$") or item.line < last_named:
            out[id(item)] = -1
    for item in stream:
        if id(item) in out:
            continue
        out[id(item)] = next((r for i, r in ends if i > item.line),
                             len(functions))
    return out


def _whole_unit_layout(lines, stream, functions, item_syms, anchors, unit_dir):
    """{section: (base, [_Placed])}: the unit's data objects in retail order,
    aligned as the assembler aligns them, each section anchored at the one
    base address every known object agrees on."""
    ranks = _attach_ranks(lines, stream, functions)
    ordered = sorted(stream, key=lambda it: (ranks[id(it)], it.line))
    by_sec, cursors = {}, {}
    for item in ordered:
        sym = item_syms.get(id(item))
        align = item.align
        if align is None:
            align = _NATURAL_ALIGN.get(_first_directive(item), 0)
        cur = cursors.get(item.section, 0)
        off = cur + (-cur % (1 << align))
        atoms, size = _item_atoms(item, off)
        if off > cur:
            atoms.insert(0, (cur, bytes(off - cur)))
        if sym is not None and sym.get("size") is not None and size != sym["size"]:
            raise HybridError(f"{sym['name']}: compiles to {size} bytes, .symtab "
                              f"size {sym['size']}")
        by_sec.setdefault(item.section, []).append(
            _Placed(item, sym, off, atoms, size))
        cursors[item.section] = off + size

    out = {}
    for sec, placed in by_sec.items():
        bases = {}
        for p in placed:
            addr = p.sym["address"] if p.sym else anchors.get(p.item.name)
            if addr is not None:
                bases.setdefault(addr - p.offset, []).append((p, addr))
        if not bases:
            raise HybridError(f"{unit_dir}: nothing anchors its {sec} data "
                              f"({placed[0].label}...): no registry symbol and "
                              f"no retail reference to it")
        base = max(bases, key=lambda b: len(bases[b]))
        if len(bases) > 1:
            bad_p, bad_addr = next(v[0] for b, v in bases.items() if b != base)
            raise HybridError(
                f"{unit_dir}: {sec} layout disagrees with retail: "
                f"{bad_p.label} lands at 0x{base + bad_p.offset:08x}, retail has "
                f"it at 0x{bad_addr:08x} (C order or definitions differ)")
        out[sec] = (base, placed)
    return out


def _data_pointer_anchors(stream, item_syms, version):
    """{private label: address} from retail pointer words: a `.word $LCn` in
    a registry object points where the retail word at that offset points."""
    out = {}
    for item in stream:
        sym = item_syms.get(id(item))
        if sym is None:
            continue
        atoms, _size = _item_atoms(item, 0)
        for off, atom in atoms:
            if isinstance(atom, tuple) and atom[1].startswith("$"):
                addr = sym["address"] + off
                word = struct.unpack("<I", _retail_bytes(
                    version, sym["section"], addr, addr + 4))[0]
                if out.setdefault(atom[1], word) != word:
                    raise HybridError(f"{atom[1]}: retail pointers to it "
                                      f"disagree (0x{out[atom[1]]:08x}, "
                                      f"0x{word:08x})")
    return out


def _emit_atoms(atoms, lo, hi, labels):
    """Assembly for the atoms of [lo, hi) (absolute addresses), with the
    label lines due at each address emitted first. The atoms must tile the
    window without a hole."""
    out, expect = [], lo
    for addr, atom in atoms:
        n = 4 if isinstance(atom, tuple) else len(atom)
        a, b = max(addr, lo), min(addr + n, hi)
        if a >= b:
            continue
        if a != expect:
            raise HybridError(f"whole-unit layout leaves 0x{expect:08x}-"
                              f"0x{a:08x} empty")
        expect = b
        for line in labels.pop(a, []):
            out.append(line)
        if isinstance(atom, tuple):
            if (a, b) != (addr, addr + 4):
                raise HybridError(f"relocated word {atom[1]} at 0x{addr:08x} "
                                  f"straddles a data-map range boundary")
            out.append(f"\t.word\t{atom[1]}")
            continue
        chunk = atom[a - addr:b - addr]
        # Labels inside a byte run (an object starting mid-atom) split it.
        cuts = sorted(x for x in labels if a < x < b)
        start = a
        for x in cuts + [b]:
            part = chunk[start - a:x - a]
            for i in range(0, len(part), 16):
                out.append("\t.byte\t" + ",".join(f"0x{c:02x}" for c in part[i:i + 16]))
            if x < b:
                out.extend(labels.pop(x))
            start = x
    if expect != hi or labels:
        raise HybridError(f"whole-unit layout ends at 0x{expect:08x}, not "
                          f"0x{hi:08x}" if expect != hi else
                          f"labels {sorted(l for v in labels.values() for l in v)[:3]} "
                          f"fall on no data")
    return out


def _emit_nobits(start, lo, hi, labels):
    """Assembly for [start, hi) of a NOBITS piece whose objects lie in
    [lo, hi): zero fill (`.space`) with the label lines due at each address."""
    out, cursor = [], start
    for addr in sorted(labels):
        if not lo <= addr < hi:
            raise HybridError(f"label at 0x{addr:08x} outside 0x{lo:08x}-"
                              f"0x{hi:08x}")
        if addr > cursor:
            out.append(f"\t.space\t{addr - cursor}")
        out.extend(labels[addr])
        cursor = addr
    if hi > cursor:
        out.append(f"\t.space\t{hi - cursor}")
    return out


def _whole_unit_pieces(layout, unit_dir, version, bin_dir, ranges=None):
    """(lines, pieces, retail, claims) for a whole-unit hybrid.

    One piece per data-map range the unit owns: the C bytes where its
    sections lie, retail `.incbin` elsewhere (`retail`: those bytes keep the
    unit from being complete). Bytes its layout emits in a gap range are a
    claim, with a piece of exactly that span (the image link fills the rest
    of the gap). Reaching into another unit's range fails."""
    if ranges is None:
        ranges = tomllib.loads((ROOT / "config" / version / "data_map.toml")
                               .read_text())["range"]
    spans = {sec: (base, base + max((p.offset + p.size for p in placed),
                                    default=0))
             for sec, (base, placed) in layout.items()}
    lines, pieces, retail, claims = [], [], [], []
    bin_dir.mkdir(parents=True, exist_ok=True)
    for r in sorted(ranges, key=lambda r: r["start"]):
        sec = r["section"]
        nobits = sec in NOBITS_SECTIONS
        if sec not in PIECE_SECTIONS and not nobits:
            continue
        start, end = r["start"], r["end"]
        span = spans.get(sec)
        lo, hi = (max(span[0], start), min(span[1], end)) if span else (0, 0)
        if r["owner"] != unit_dir:
            if lo >= hi:
                continue
            if r["owner"] != "unassigned":
                raise HybridError(f"{unit_dir}: its {sec} data reaches into "
                                  f"{r['owner']}'s range 0x{start:08x}-"
                                  f"0x{end:08x}")
            claims.append((sec, lo, hi))
            start, end = lo, hi
        name = piece_name(sec, start)
        if nobits:
            # No file bytes: labels at their addresses, zero fill around them.
            lines += [f'\t.section\t{name},"aw",@nobits', "\t.align\t0"]
            filler = []
        else:
            blob = bin_dir / f"{name}.bin"
            blob.write_bytes(_retail_bytes(version, sec, start, end))
            flags = "a" if sec == ".rodata" else "aw"
            lines += [f'\t.section\t{name},"{flags}",@progbits', "\t.align\t0"]
            filler = retail
        objects = []
        if lo < hi:
            base, placed = layout[sec]
            atoms, labels = [], {}
            for p in placed:
                atoms += [(base + off, atom) for off, atom in p.atoms]
                addr = base + p.offset
                if not lo <= addr < hi:
                    continue
                if p.sym:
                    head = ([f"\t.globl\t{p.label}"] if p.item.globl else []) + [
                        f"\t.type\t{p.label},@object", f"\t.size\t{p.label},{p.size}"]
                    objects.append({"name": p.label, "address": addr,
                                    "size": p.size, "sized_by": "registry"
                                    if p.sym.get("size") else "compiler"})
                else:
                    head = []
                labels.setdefault(addr, []).extend(head + [f"{p.label}:"])
            if nobits:
                lines += _emit_nobits(start, lo, hi, labels)
            else:
                if lo > start:
                    lines.append(f'\t.incbin\t"{blob.as_posix()}",0,{lo - start}')
                    filler.append((start, lo))
                lines.append(f"\t.org\t0x{lo - start:X}")
                lines += _emit_atoms(atoms, lo, hi, labels)
                lines.append(f"\t.org\t0x{hi - start:X}")
        else:
            hi = start
        if hi < end:
            if nobits:
                lines.append(f"\t.space\t{end - hi}")
            else:
                lines.append(f'\t.incbin\t"{blob.as_posix()}",{hi - start},'
                             f'{end - hi}')
                filler.append((hi, end))
        pieces.append({"name": name, "section": sec, "start": start,
                       "end": end, "objects": objects})
    for sec, (base, placed) in layout.items():
        if not any(p["section"] == sec and p["start"] <= base < p["end"]
                   for p in pieces):
            raise HybridError(f"{unit_dir}: its {sec} data at 0x{base:08x} is "
                              f"in no data-map range")
    return lines, pieces, retail, sorted(claims)


def _build_whole_unit(data, out_o, link_set, version, plan=False):
    """Hybrid object of a whole-unit (complete) unit; with `plan`, only the
    claims its layout needs."""
    unit_dir = data["unit"].split(":")[1]
    profile = data.get("profile", "default")
    functions = _manifest_functions(data)
    out_o = Path(out_o)
    out_o.parent.mkdir(parents=True, exist_ok=True)
    cs = out_o.with_name(out_o.stem + "_cc.s")
    cc.compile_s(ROOT / data["source"], cs, profile=profile, version=version)
    compiled = cs.read_text()
    labels, retail_pools = _retail_anchors(compiled, functions, unit_dir,
                                           version)
    pool = _Pool()
    lines, _externs = _rewrite_lis(compiled.splitlines(), version, unit_dir,
                                   data["source"], None, [None], pool=pool)
    items, stream = [], []
    prologue, segments, _private = parse_s("\n".join(lines), items, stream)
    for _addr, name, _state in functions:
        mine = [kind for kind, _l in pool.entries.get(name, [])]
        theirs = [kind for kind, _a in retail_pools.get(name, [])]
        if mine != theirs:
            raise HybridError(f"{name}: assembler pool decisions {mine} differ "
                              f"from retail's {theirs}")
        for (_k, label), (_k2, addr) in zip(pool.entries.get(name, []),
                                           retail_pools.get(name, [])):
            if label:
                labels[label] = addr

    states = data_states(data)
    item_syms, rename = {}, {}
    for item, sym in _registry_map(items, version, unit_dir):
        if sym["section"] not in C_DATA_SECTIONS + NOBITS_SECTIONS:
            raise HybridError(f"{sym['name']}: {sym['section']} data from C "
                              f"is not supported")
        if states.get(sym["address"]) not in ("matching", "retail"):
            raise HybridError(f"{sym['name']}: [[data]] entry is not done")
        item_syms[id(item)] = sym
        if item.name != sym["name"]:
            rename[item.name] = sym["name"]
    dups = _duplicate_static_labels(functions, version)
    for item in stream:
        item.lines = _relabel_items(item.lines, rename, dups)
    for lbl, addr in _data_pointer_anchors(stream, item_syms, version).items():
        if labels.setdefault(lbl, addr) != addr:
            raise HybridError(f"{lbl}: the code borrows 0x{labels[lbl]:08x}, "
                              f"retail data points at 0x{addr:08x}")
    layout = _whole_unit_layout(lines, stream, functions, item_syms, labels,
                                unit_dir)
    for base, placed in layout.values():
        for p in placed:
            if p.sym and states.get(p.sym["address"]) == "retail":
                # A raw asset keeps its retail bytes (the alignment pad
                # before it stays the compiler's).
                blob = _retail_bytes(version, p.sym["section"], base + p.offset,
                                     base + p.offset + p.size)
                p.atoms = [a for a in p.atoms if a[0] < p.offset] \
                    + [(p.offset, blob)]
    piece_lines, pieces, retail, claims = _whole_unit_pieces(
        layout, unit_dir, version, out_o.parent / f"{out_o.stem}_data")
    if plan:
        return claims
    if claims != manifest_claims(data):
        shown = ", ".join(f"{s} 0x{a:08x}-0x{b:08x}" for s, a, b in claims)
        raise HybridError(f"{unit_dir}: its layout claims [{shown or 'nothing'}] "
                          f"but the manifest's [[claim]] table differs (run "
                          f"promote.py --complete {data['unit']})")

    prologue = _relabel(_relabel_data(prologue, rename), dups)
    seg_by_name = {name: _relabel(_relabel_data(seg, rename), dups)
                   for name, seg in segments}
    missing = sorted(n for _a, n, _s in functions if n not in seg_by_name)
    if missing:
        raise HybridError(f"declared matching but not defined in "
                          f"{data['source']}: {', '.join(missing)}")
    borrowed = []
    out_lines = _splice_unit(prologue, seg_by_name, functions, unit_dir,
                             link_set, version, report_base=False,
                             unit_end=_unit_end(unit_dir), local_data={},
                             profile=profile, borrowed=borrowed)
    if borrowed:
        raise HybridError(f"{unit_dir}: whole-unit code still borrows "
                          f"{borrowed[:3]}")
    unit_s = _assemble_unit(out_o, out_lines + piece_lines,
                            profile_g_flag(profile, version),
                            allowed=[p["name"] for p in pieces])
    _write_data_report(out_o, pieces, retail, [], version, claims=claims)
    return unit_s


def plan_claims(manifest_path, version="pal103"):
    """The [(section, start, end)] gap bytes a unit's whole-unit layout emits
    (promote.py --complete writes them as the manifest's [[claim]] table)."""
    data = tomllib.loads(Path(manifest_path).read_text())
    rel = Path(data["source"]).relative_to("src").with_suffix(".o")
    return _build_whole_unit(data, ROOT / "build" / version / "matching" / rel,
                             "matching", version, plan=True)


def _compile_and_split(data, out_o, link_set, version, report=False):
    """Compile the unit's C and split it into (prologue, seg_by_name,
    functions, unit_dir, local_data, emit). Shared by the hybrid link and the
    report base object -- both start from the same compiler output and the
    same from-C function set. `emit` lists the C data objects the hybrid
    owns (none for the report object, whose data references are renamed to
    the retail names instead)."""
    src = ROOT / data["source"]
    unit_dir = data["unit"].split(":")[1]
    functions = _manifest_functions(data)
    from_c = {name for _a, name, state in functions
              if state in LINK_SETS[link_set]}
    cs = Path(out_o).with_name(Path(out_o).stem + "_cc.s")
    cc.compile_s(src, cs, profile=data.get("profile", "default"),
                 version=version)
    items = []
    prologue, segments, local_data = parse_s(cs.read_text(), items)
    data_rename, emit = _plan_data(items, data, version, unit_dir, report)
    prologue = _relabel_data(prologue, data_rename) \
        + _small_data_externs(items, emit, data_rename)
    segments = [(name, _relabel_data(seg, data_rename)) for name, seg in segments]
    for item, _sym in emit:
        item.lines = _relabel_items(item.lines, data_rename, {})
    rename = _duplicate_static_labels(functions, version)
    rename.update(_own_static_renames(
        prologue, version, unit_dir,
        profile_g_flag(data.get("profile", "default"), version)))
    prologue = _relabel(prologue, rename)
    segments = [(name, _relabel(seg, rename)) for name, seg in segments]
    seg_by_name = dict(segments)
    if len(seg_by_name) != len(segments):
        raise HybridError("duplicate function names in compiler output")
    missing = sorted(from_c - set(seg_by_name))
    if missing:
        raise HybridError(f"declared {'/'.join(LINK_SETS[link_set])} but not "
                          f"defined in {data['source']}: {', '.join(missing)}")
    return prologue, seg_by_name, functions, unit_dir, local_data, emit


def _splice_unit(prologue, seg_by_name, functions, unit_dir, link_set,
                 version, report_base, unit_end, local_data=None,
                 profile="default", borrowed=None):
    """Ordered .s lines for one unit.

    Hybrid: link_set functions from C, padded to their extent; the rest from
    retail slices. Report base: every function that has C, unpadded; nothing
    else.
    """
    out_lines = list(prologue)
    body_lines = []
    mapper_box = [None]
    base = functions[0][0]
    first_from_c = functions[0][1] in seg_by_name if report_base \
        else functions[0][2] in LINK_SETS[link_set]
    if first_from_c and not report_base:
        # C for the first function also carries any code ahead of it.
        base = _unit_start(unit_dir, version)
    ends = [a for a, _n, _s in functions[1:]] + [unit_end]
    for (addr, name, state), end in zip(functions, ends):
        # The report base keys off "did the compiler produce C for this
        # function", not the manifest state, so a WIP function written in C
        # but still marked asm keeps its fuzzy match in the report. The hybrid
        # keys off link_set, so an asm/equivalent function stays a retail
        # slice in the matching image.
        from_c = name in seg_by_name if report_base \
            else state in LINK_SETS[link_set]
        if from_c:
            seg, externs = _rewrite_lis(seg_by_name[name], version, unit_dir,
                                        name, addr, mapper_box)
            out_lines += externs   # symbol metadata; hoisted, emits no bytes
            if borrowed is not None:
                borrowed += [e.split()[1].rstrip(",") for e in externs]
            seg, externs = _rewrite_local_data(seg, local_data or {}, version,
                                               unit_dir, name, addr, mapper_box)
            out_lines += externs   # .extern for the borrowed retail data slot
            if borrowed is not None:
                borrowed += [e.split()[1].rstrip(",") for e in externs]
            body_lines += _sonyize_seg(seg, profile)
            if end is not None and not report_base:
                # A function's registry extent runs to the next function and
                # includes retail's trailing pad nops; the compiler does not
                # emit those. Zero-fill to the extent end (zero == nop) --
                # and gas fails loudly ("moving .org backwards") if the
                # compiled code overruns its extent.
                body_lines.append(f".org 0x{end - base:X}, 0")
        elif report_base:
            continue                 # not implemented in C: no symbol, no bytes
        else:
            body_lines += _slice_lines(version, unit_dir, name, addr)
    out_lines += body_lines
    if profile == "sce":
        out_lines = _sce_short_loop_pads(out_lines,
                                         profile_g_flag(profile, version))
    return out_lines


def profile_g_flag(profile, version="pal103"):
    """The `-G<n>` of a compiler profile: the assembler must use the same
    small-data threshold the compiler did, or a `.extern SYM, n` / macro load
    is addressed differently from the compiler's own assumption."""
    flags = [f for f in cc.profile_spec(profile, version)["flags"]
             if re.fullmatch(r"-G\d+", f)]
    if len(flags) != 1:
        raise HybridError(f"profile {profile!r} must name exactly one -G flag")
    return flags[0]


def _assemble_unit(out_o, out_lines, g_flag="-G8", allowed=()):
    """Write the spliced .s next to out_o, assemble it, and reject data
    outside the `allowed` piece sections."""
    out_o = Path(out_o)
    unit_s = out_o.with_suffix(".s")
    unit_s.parent.mkdir(parents=True, exist_ok=True)
    unit_s.write_text("\n".join(out_lines) + "\n")
    _assemble_hybrid(unit_s, out_o, g_flag)
    _check_no_data_sections(out_o, allowed)
    return unit_s


def build_hybrid(manifest_path, out_o, link_set="matching", version="pal103"):
    """Assemble one unit's hybrid object; returns the intermediate .s path."""
    data = tomllib.loads(Path(manifest_path).read_text())
    if whole_unit(data):
        return _build_whole_unit(data, out_o, link_set, version)
    prologue, seg_by_name, functions, unit_dir, local_data, emit = \
        _compile_and_split(data, out_o, link_set, version)
    borrowed = []
    out_lines = _splice_unit(prologue, seg_by_name, functions, unit_dir,
                             link_set, version, report_base=False,
                             unit_end=_unit_end(unit_dir), local_data=local_data,
                             profile=data.get("profile", "default"),
                             borrowed=borrowed)
    pieces, retail = [], []
    if emit:
        out_o = Path(out_o)
        lines, pieces, retail = _data_pieces(
            emit, unit_dir, version, out_o.parent / f"{out_o.stem}_data")
        out_lines += lines
    unit_s = _assemble_unit(out_o, out_lines,
                            profile_g_flag(data.get("profile", "default"), version),
                            allowed=[p["name"] for p in pieces])
    _write_data_report(out_o, pieces, retail, borrowed, version)
    return unit_s


def data_report_path(out_o):
    """The sidecar a hybrid build writes next to its object."""
    out_o = Path(out_o)
    return out_o.with_name(out_o.stem + ".data.json")


def _write_data_report(out_o, pieces, retail, borrowed, version, claims=None):
    """The sidecar: pieces, retail byte ranges in owned ranges, borrowed
    compiler-private slots; a whole-unit hybrid adds its claims."""
    slots = [{"symbol": sym, "address": _symbol_address(sym, version)}
             for sym in dict.fromkeys(borrowed)]
    report = {"schema": 1, "pieces": pieces,
              "retail": [[lo, hi] for lo, hi in retail],
              "borrowed": slots}
    if claims is not None:
        report.update(whole_unit=True, claims=[list(c) for c in claims])
    import json
    data_report_path(out_o).write_text(json.dumps(report, indent=1) + "\n")


def build_report_object(manifest_path, out_o, version="pal103"):
    """Assemble one unit's objdiff report base object; returns the .s path.

    Same C compile and normalization as the hybrid, but the object carries a
    symbol for EVERY function the compiler produced C for, whatever its
    manifest state, and nothing else -- un-decompiled functions are omitted
    (no symbol), never spliced from retail. A `matching` function is
    byte-identical to its hybrid (hence to retail), so objdiff scores it 100%;
    an `equivalent` or a WIP-but-still-`asm` function shows its honest fuzzy
    match; a function with no C shows 0%. No retail slice bytes, and
    `_check_no_data_sections` guarantees it introduces no data section of its
    own -- this is a report-only measurement object, never a link input.
    """
    data = tomllib.loads(Path(manifest_path).read_text())
    # link_set only gates the compile-time "matching/equivalent must be
    # defined in C" check in _compile_and_split; the report includes functions
    # by whether the compiler emitted them, not by link_set.
    link_set = "equivalent"
    prologue, seg_by_name, functions, unit_dir, local_data, _emit = \
        _compile_and_split(data, out_o, link_set, version, report=True)
    out_lines = _splice_unit(prologue, seg_by_name, functions, unit_dir,
                             link_set, version, report_base=True,
                             unit_end=_unit_end(unit_dir), local_data=local_data,
                             profile=data.get("profile", "default"))
    return _assemble_unit(out_o, out_lines,
                          profile_g_flag(data.get("profile", "default"), version))


def _duplicate_static_labels(functions, version):
    """{NAME: NAME__<VRAM>} for this unit's duplicated statics, the label the
    retail slices use (declib.asmtext.disambiguate)."""
    text = (ROOT / "config" / version / "functions.toml").read_text()
    names = re.findall(r"^name = '([^']*)'", text, re.M)
    dups = {n for n in set(names) if names.count(n) > 1}
    return {name: f"{name}__{addr:08X}" for addr, name, _s in functions
            if name in dups}


def _relabel(lines, rename):
    """Rewrite every whole-symbol occurrence of a key of `rename`."""
    if not rename:
        return lines
    pattern = re.compile(r"(?<![\w.$])(" + "|".join(map(re.escape, rename))
                         + r")(?![\w$])")
    return [pattern.sub(lambda m: rename[m.group(1)], l) for l in lines]


_DATA_SECTIONS = {".lit4", ".lit8", ".data", ".sdata", ".rdata", ".rodata",
                  ".sbss", ".bss"}


def _check_no_data_sections(out_o, allowed=()):
    """Fail loudly if the assembled hybrid carries data outside its pieces.

    A hybrid contributes .text and, with C data, only its `allowed` piece
    sections: every other data byte of the image comes from the per-range
    data objects (tools/gen_data_objects.py). Any other non-empty data section
    means a compiled segment slipped data past the rewrites (e.g. an unhandled
    literal pool) and the link would either fail or, worse, silently place
    bytes this tool never verified; a COMMON symbol would be invisible to the
    image SHA, so it is refused too.
    """
    from declib.elf import read_elf
    try:
        sections, symbols, _rel = read_elf(Path(out_o).read_bytes())
    except ValueError:
        raise HybridError(f"{out_o}: not an ELF object")
    allowed = set(allowed)
    offenders = [f"{s['name']} ({s['size']} bytes)" for s in sections
                 if s["size"] > 0 and s["name"] not in allowed
                 and any(s["name"] == d or s["name"].startswith(d + ".")
                         for d in _DATA_SECTIONS)]
    common = [s["name"] for s in symbols if s["shndx"] in (0xFFF2, 0xFF03)]
    if offenders or common:
        raise HybridError(
            f"{out_o}: hybrid object owns data outside its pieces: "
            f"{', '.join(offenders + [f'COMMON {c}' for c in common])} -- "
            f"compiled segments may only contribute .text")


def _assemble_hybrid(hybrid_s, out_o, g_flag="-G8"):
    """Assemble the spliced .s with the decompals binutils `as`.

    Sony's own `as` (the original plan) cannot assemble the retail slices:
    it predates the `%gp_rel(sym)($gp)` relocation syntax, and it carries an
    unconditional R5900 short-loop erratum workaround that pads backward
    branches with nops -- retail game code contains unpadded 4-instruction
    loops, so the padded output can never be byte-identical. The decompals
    `as` assembles both the slices (it already produces the byte-verified
    expected objects from the same text) and ee-gcc's compiler output
    (`.extern SYM, n` + macro forms resolve to R_MIPS_GPREL16 under -G8,
    probed) without padding. `g_flag` is the unit profile's own `-G`
    (profile_g_flag). Internal alignment directives still emit their
    required padding, but section-end rounding must be disabled: a retail TU
    can end on a four-byte boundary even when C uses eight-byte alignment
    internally (nucore/nustring). The canonical extents already own all tail
    padding; adding more shifts subsequent TUs in the loaded image.
    The whole-unit byte gates remain the arbiter
    of every matching claim, so a divergent encoding fails loudly.
    """
    as_bin = tool_path(AS)
    if not as_bin:
        raise HybridError(f"{AS} not found (setup_toolchain.py installs it)")
    out_o = Path(out_o)
    out_o.parent.mkdir(parents=True, exist_ok=True)
    proc = subprocess.run(
        [as_bin, "-EL", "-march=r5900", g_flag, "-no-pad-sections",
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
