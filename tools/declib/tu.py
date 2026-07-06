"""Translation-unit splitting of the monolithic .text disassembly.

Extracted verbatim from split_text.py: the mdebug-driven TU runs, the
function-block cutter, and the proven no-op `.align` removal. These are the
pieces every per-unit consumer (splitter, matcher, later slicers) shares.
"""
import re

from . import asmtext
from .toolchain import ROOT


def parse_toml_blocks(path, keys):
    """Yield one dict per `[[...]]` block, pulling `key = value` scalars."""
    text = path.read_text()
    for block in re.split(r"\[\[[a-z]+\]\]", text)[1:]:
        row = {}
        for key, pat in keys.items():
            m = re.search(pat, block)
            if m:
                row[key] = m.group(1)
        yield row


def load_units():
    """unit index -> a filesystem-safe base name derived from the source path."""
    names = {}
    for row in parse_toml_blocks(ROOT / "config" / "pal103" / "units.toml",
                                 {"index": r"index = (\d+)", "name": r"name = '([^']*)'"}):
        base = re.split(r"[\\/]", row["name"])[-1]
        names[int(row["index"])] = re.sub(r"[^A-Za-z0-9_.-]", "_", base)
    return names


def load_tu_runs():
    """Ordered [(unit_index, first_vram), ...], one per TU that owns .text code.

    Built from the procedure table: sort procedures by address and start a new
    run whenever the owning unit changes. The mdebug lays each TU's code out
    contiguously, so these runs are exactly the .text translation units.
    """
    funcs = [(int(r["address"], 16), int(r["unit"]))
             for r in parse_toml_blocks(
                 ROOT / "config" / "pal103" / "functions.toml",
                 {"address": r"address = (0x[0-9A-Fa-f]+)", "unit": r"unit = (\d+)"})]
    runs = []
    for addr, unit in sorted(funcs):
        if not runs or runs[-1][0] != unit:
            runs.append((unit, addr))
    return runs


def glabel_addresses(lines):
    """line index of each `glabel` -> the function's first-instruction vram."""
    out = {}
    for i, line in enumerate(lines):
        if line.startswith("glabel "):
            j = i + 1
            while j < len(lines) and asmtext.instr_at(lines, j) is None:
                j += 1
            if j < len(lines):
                out[i] = asmtext.instr_at(lines, j)[0]
    return out


# A line that introduces a function block (leading run swept up with the block).
_BLOCK_LEAD = ("nonmatching ", "matching ", ".align")


def is_lead(line):
    s = line.strip()
    if s == "" or line.startswith(_BLOCK_LEAD):
        return True
    # A standalone comment (e.g. "/* Handwritten function */") but not an
    # instruction line, which also starts with "/*".
    return s.startswith("/*") and asmtext.INSTR_RE.search(line) is None


def split_body(lines, body_start, cut_addrs):
    """Partition lines[body_start:] into chunks that start at each cut address.

    A cut is placed at the head of the function block whose first byte is a TU's
    first-function address -- i.e. just above its leading `.align`/`nonmatching`
    run -- so each chunk holds whole function blocks and nothing is duplicated.
    Returns the list of chunk line-lists, in file (address) order.
    """
    addr_to_glabel = {a: i for i, a in glabel_addresses(lines).items()}
    cut_lines = []
    for a in cut_addrs:
        gi = addr_to_glabel[a]
        start = gi
        while start - 1 >= body_start and is_lead(lines[start - 1]):
            start -= 1
        cut_lines.append(start)
    cut_lines = sorted(set(cut_lines))

    chunks, prev = [], body_start
    for cut in cut_lines:
        chunks.append(lines[prev:cut])
        prev = cut
    chunks.append(lines[prev:])
    return chunks


def drop_alignment(lines, reporter):
    """Remove every `.align` directive; they are all no-ops for this .text.

    The disassembly covers .text with one explicit instruction word per four
    bytes and no address gaps, so no `.align` ever emits a padding byte. Dropping
    them makes each TU object a pure instruction stream: its section needs no
    alignment, so it carries no trailing pad and its internal layout no longer
    depends on the object's base phase (mod 8) -- which a TU that starts at a
    4-aligned address would otherwise get wrong. Returns the filtered lines, or
    None if an `.align` unexpectedly sits next to an address discontinuity.
    """
    kept, prev_end, dropped, align_pending = [], None, 0, False
    for line in lines:
        info = asmtext.INSTR_RE.search(line)
        if info:
            addr = int(info.group(1), 16)
            if prev_end is not None and addr != prev_end and align_pending:
                reporter.result("Drop no-op .align directives", False,
                                f"gap 0x{prev_end:x}->0x{addr:x} filled by .align")
                return None
            prev_end = addr + 4
            align_pending = False
        if line.strip().startswith(".align"):
            dropped += 1
            align_pending = True
            continue
        kept.append(line)
    reporter.result("Drop no-op .align directives", True)
    print(f"  {dropped} .align directives removed (all no-ops)")
    return kept


def prologue_end(lines):
    """First line that begins content (a function block); end of the header."""
    for i, line in enumerate(lines):
        if line.startswith("glabel ") or line.startswith(_BLOCK_LEAD):
            return i
    return 0
