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
     normalized (`_sonyize`: the two pseudo-ops ee-gcc and the decompals
     binutils encode differently) and assembled with the decompals `as`
     (declib.toolchain.AS), NOT the ee-gcc driver -- the retail slices are
     already-assembled text that only that assembler round-trips byte-exactly.

Two link sets exist: `matching` (only exact functions from C) and
`equivalent` (reviewed-equivalent C compiles in too; slices only for `asm`
functions). tools/verify_hybrid.py proves the matching set byte-identical to
retail over the whole unit. build_report_object() reuses the same compile and
normalization to emit objdiff's base object (C functions only, no slices, no
extent padding) -- a measurement, never linked.

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

    ee-gcc's private initialized aggregates ($LCn in a data section) are pulled
    out first (see _extract_local_data) and returned as {label: bytes} so the
    remaining stream is pure text; the caller remaps each onto the retail
    owned-data slot the function's slice names.
    """
    lines, local_data = _extract_local_data(text.splitlines())
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

# ee-gcc emits `cvt.w.s $fd,$fs` for the float->int truncation, which Sony's
# assembler accepts but the decompals `as` rejects on r5900 ("opcode not
# supported"). The retail disassembly already carries it as a raw `.word`
# (0x46000064 == `cvt.w.s $f1,$f0`), so encode it the same way here.
# cop1.S cvt.w = 0x46000024 | (fs << 11) | (fd << 6).
_CVTWS_RE = re.compile(r"^(\s*)cvt\.w\.s(?:\s+)\$f(\d+)\s*,\s*\$f(\d+)\s*$")

# R5900 mtc1 hazard nop. An `mtc1` immediately followed by an FP compare needs
# a nop between them, and the two assemblers disagree on when: Sony's `as`
# (whose choices retail contains) inserts it whenever the compare is adjacent,
# the decompals `as` only when the compare also *reads the register mtc1 just
# wrote*. Where gcc left an independent compare adjacent, retail has the nop and
# the decompals `as` drops it, so the function assembles one word short and the
# whole tail shifts -- the wall behind a documented backlog of near-matches.
#
# Usually neither instruction is even visible here: `li.s $f0,1.0` is an
# assembler macro that expands to `lui $at,0x3f80` + `mtc1 $at,$f0` (retail
# fsign @0x221530 is exactly this -- mtc1 $at,$f0 / nop / c.le.s $f1,$f12, on
# disjoint registers). So the trigger is "the previous instruction writes an FP
# register through mtc1", which is either a literal `mtc1` or an inline `li.s`
# (the pool-bound ones are already lwc1 by the time _sonyize runs).
#
# Inserting the nop explicitly is safe in both cases and needs no dependency
# analysis of our own: with a real `nop` ahead of it the compare no longer sees
# a hazard, so the decompals `as` adds nothing, and the dependent case that it
# *would* have handled comes out with exactly one nop either way (probed both).
# Only the compare class is claimed here -- the byte gates carry the proof, so
# widen it (add.s, swc1, ...) only against retail evidence, one class at a time.
_MTC1_RE = re.compile(r"^\s*mtc1\s+\$\w+\s*,\s*\$f\d+\s*$")
_LIS_INLINE_RE = re.compile(r"^\s*li\.s\s+\$f\d+\s*,")
_CCOND_RE = re.compile(r"^\s*c\.(?:eq|lt|le|ueq|ult|ule|f|un|olt|ole)\.s\s+\$f")

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
# double bit pattern inline in an integer register -- e.g. `li.d $5,1.0e1` to
# pass 10.0 as the `s64` argument of the software double helpers (dpmul et al).
# This is not a .lit4/.lit8 pool load, and the decompals `as` rejects the
# float-syntax GPR macro; retail expands it inline as an integer immediate
# (`ori $a1,$zero,0x8048; dsll32 $a1,$a1,15` for 10.0). We rewrite it to the
# integer `li`/`dli` the assembler does expand, over the raw bit pattern.
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
_ENTER_DATA_RE = re.compile(
    r"^\s*\.(sdata|data|rdata|rodata|sdata2)\b"
    r"|^\s*\.section\s+\.(sdata|data|rdata|rodata)\b")
_LEAVE_DATA_RE = re.compile(
    r"^\s*\.(text|ent|globl)\b|^\s*\.section\s+\.text\b")
# Symbol names in %hi/%lo/%gp_rel operands (candidate owned-data slots).
_SYM_REF_RE = re.compile(r"%(?:hi|lo|gp_rel)\(([A-Za-z_$][\w$]*)\)")
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


def _extract_local_data(lines):
    """Pull ee-gcc's private data blocks ($LCn/$Ln in .sdata/.rodata/...) out
    of the compiled stream so the rest parses as pure text.

    Returns (clean_lines, {label: value}) where value is the block's bytes for
    a constant aggregate, or a `_JumpTable(count)` marker for a switch jump
    table (`.word <label>` entries, which are relocations, not bytes). Each
    block must define exactly one private label; the whole-unit byte gates
    verify every remap downstream."""
    clean, local = [], {}
    i, n = 0, len(lines)
    while i < n:
        if not _ENTER_DATA_RE.match(lines[i]):
            clean.append(lines[i])
            i += 1
            continue
        j = i + 1
        block = []
        while j < n and not _LEAVE_DATA_RE.match(lines[j]):
            block.append(lines[j])
            j += 1
        # Consume the redundant .text that closes the block, if present.
        if j < n and re.match(r"^\s*\.text\b", lines[j]):
            j += 1
        labels = [_LOCAL_LABEL_RE.match(b).group(1)
                  for b in block if _LOCAL_LABEL_RE.match(b)]
        if not labels:
            raise HybridError(
                "local data block with no private label unsupported")
        # ee-gcc packs one or more private slots ($LCn/$Ln) between a single
        # .rdata/.text switch -- e.g. the two format strings of a function that
        # calls NuDebugMsgProlog twice. Each label names an independent
        # aggregate that borrows its own retail slot, so partition the block at
        # every label. A one-label block is the common case and keeps its whole
        # body (interior .align in a const aggregate is meaningful); when a
        # block holds several labels the directives between them only align the
        # next slot, so they are not part of any aggregate's bytes.
        if len(labels) == 1:
            segments = [(labels[0], block)]
        else:
            segments = []
            for b in block:
                m = _LOCAL_LABEL_RE.match(b)
                if m:
                    segments.append((m.group(1), []))
                elif segments and b.strip() and \
                        b.strip().split(None, 1)[0] not in (".align", ".p2align"):
                    segments[-1][1].append(b)
        # A slot whose every payload entry is `.word <symbol>` is a jump table
        # (relocations); anything else is a constant aggregate we can assemble.
        for label, seg in segments:
            payload = [b for b in seg if b.strip()
                       and not _LOCAL_LABEL_RE.match(b)
                       and b.strip().split(None, 1)[0] not in (".align", ".p2align")]
            if payload and all(_WORD_LABEL_RE.match(p) for p in payload):
                local[label] = _JumpTable(len(payload))
            else:
                local[label] = _assemble_data_bytes(seg)
        i = j
    return clean, local


def _float_bits(text):
    """float32 bit pattern of a gcc-printed decimal constant."""
    return struct.unpack("<I", struct.pack("<f", float(text)))[0]


def _double_bits(text):
    """float64 bit pattern of a gcc-printed decimal constant."""
    return struct.unpack("<Q", struct.pack("<d", float(text)))[0]


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
        self._unit_ranges = {}
        for r in dm.get("range", []):
            self._unit_ranges.setdefault(r["owner"], []).append(
                (int(r["start"]), int(r["end"])))

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
        fd = int(m.group(2))
        fs = int(m.group(3))
        word = 0x46000024 | (fs << 11) | (fd << 6)
        return f"{m.group(1)}.word 0x{word:08X}"
    return line


def _writes_fpr_via_mtc1(line):
    """Does this line assemble to an mtc1 as its last instruction? (_MTC1_RE)"""
    return bool(_MTC1_RE.match(line) or _LIS_INLINE_RE.match(line))


def _sonyize_seg(seg):
    """`_sonyize` over one function segment, plus the mtc1 hazard nops.

    Line-by-line except that a nop is materialized between an mtc1-writing
    instruction and an adjacent FP compare (see _MTC1_RE): a hazard Sony's `as`
    covers unconditionally and the decompals `as` only on a register
    dependency. Comment/blank lines do not separate the two -- gcc emits its own
    `#nop` markers between them, and those are only annotations (retail
    ApplyFriction has no nop at one of them), never a nop to materialize.
    """
    out = []
    prev_insn = None
    for line in seg:
        if _CCOND_RE.match(line) and prev_insn is not None \
                and _writes_fpr_via_mtc1(prev_insn):
            out.append("\tnop")
        out.append(_sonyize(line))
        if not _is_noise(line):
            prev_insn = line
    return out


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
    """
    if not any("li.s" in l or "li.d" in l for l in seg):
        return seg, []
    maps = {}                      # size (4/8) -> {bits: [syms]}
    used = {}                      # (size, bits) -> slots of that value spent
    slice_cache = [None]
    out, externs = [], []

    def slots_for(size, bits):
        if size not in maps:
            if mapper_box[0] is None:
                mapper_box[0] = Lit4Mapper(version)
            if slice_cache[0] is None:
                slice_cache[0] = _slice_path(
                    version, unit_dir, name, addr).read_text()
            fn = (mapper_box[0].map_for_slice if size == 4
                  else mapper_box[0].map_for_slice8)
            maps[size] = fn(slice_cache[0], name)
        return maps[size].get(bits, ())

    for line in seg:
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
                    # Emit `dli` (64-bit explicit) for BOTH sizes: `li` on a
                    # high-bit-set float32 image (e.g. -1.0 -> 0xBF800000) would
                    # `lui`-sign-extend into bits 32-63; `dli` sets all 64 bits
                    # deterministically so the register holds exactly the raw
                    # bit pattern, zero-extended. The byte gate is the backstop
                    # if decompals `as` ever expands `dli` unlike retail's SN as.
                    width = 16 if op == "li.d" else 8
                    out.append(f"\tdli\t{reg},0x{bits:0{width}X}")
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
        slots = slots_for(size, bits)
        k = used.get((size, bits), 0)
        if k >= len(slots):
            raise HybridError(
                f"{name}: li.{'s' if size == 4 else 'd'} constant {const} "
                f"(0x{bits:0{size * 2}X}) has no .lit{size} slot among the "
                f"retail function's gp references")
        sym = slots[k]
        used[(size, bits)] = k + 1
        out.append(f"\t{load}\t{reg},{sym}")
        externs.append(f"\t.extern\t{sym}, {size}")
    return out, externs


def _rewrite_local_data(seg, local_data, version, unit_dir, name, addr,
                        mapper_box):
    """Rewrite references to a compiler-private initialized aggregate ($LCn)
    or switch jump table ($Ln) onto the retail slot it borrows.

    A constant aggregate borrows the owned-data slot holding the same bytes; a
    `_JumpTable` borrows the retail jtbl_/jpt_ slot the slice addresses. Returns
    (lines, extern_directives); segments that touch no captured local label
    pass through untouched, so this is free for the common case."""
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
    out = []
    for line in seg:
        for lbl, sym in rename.items():
            line = label_re[lbl].sub(sym, line)
        out.append(line)
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
    prologue, segments, local_data = parse_s(cs.read_text())
    seg_by_name = dict(segments)
    if len(seg_by_name) != len(segments):
        raise HybridError("duplicate function names in compiler output")
    missing = sorted(from_c - set(seg_by_name))
    if missing:
        raise HybridError(f"declared {'/'.join(LINK_SETS[link_set])} but not "
                          f"defined in {data['source']}: {', '.join(missing)}")
    return prologue, seg_by_name, functions, unit_dir, local_data


def _splice_unit(prologue, seg_by_name, functions, unit_dir, link_set,
                 version, report_base, unit_end, local_data=None):
    """Ordered .s lines for one unit.

    Each included function is normalized identically (`_rewrite_lis` maps
    pool-bound li.s onto the retail .lit4 slots, `_sonyize` fixes the two
    decompals pseudo-ops). The two modes differ in which functions they
    include and in how they treat the rest:

      report_base=False (hybrid): functions whose manifest state is in
        link_set compile from C and are zero-padded to their registry extent
        (so the linked image is byte-exact; gas fails loudly if compiled code
        overruns its extent); every other function is spliced from its retail
        slice.
      report_base=True  (objdiff report base): EVERY function the compiler
        produced C for gets a symbol -- matching, equivalent, or a WIP
        function still marked asm -- regardless of manifest state, so its
        fuzzy match shows honestly (a matching function reads 100%, a WIP
        function its partial match). Functions with no C are omitted: no
        symbol, so objdiff pairs the retail target with nothing -> 0%. No
        extent padding, so an equivalent/WIP function that compiles longer
        than its retail extent does not trigger a backwards-.org failure.
    """
    out_lines = list(prologue)
    body_lines = []
    mapper_box = [None]
    base = functions[0][0]
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
            seg, externs = _rewrite_local_data(seg, local_data or {}, version,
                                               unit_dir, name, addr, mapper_box)
            out_lines += externs   # .extern for the borrowed retail data slot
            body_lines += _sonyize_seg(seg)
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
    prologue, seg_by_name, functions, unit_dir, local_data = _compile_and_split(
        data, out_o, link_set, version)
    out_lines = _splice_unit(prologue, seg_by_name, functions, unit_dir,
                             link_set, version, report_base=False,
                             unit_end=_unit_end(unit_dir), local_data=local_data)
    return _assemble_unit(out_o, out_lines)


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
    prologue, seg_by_name, functions, unit_dir, local_data = _compile_and_split(
        data, out_o, link_set, version)
    out_lines = _splice_unit(prologue, seg_by_name, functions, unit_dir,
                             link_set, version, report_base=True,
                             unit_end=_unit_end(unit_dir), local_data=local_data)
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
