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

    A procedure-less file descriptor right after a unit's own and below its
    first procedure is included code; the run starts there (nu2api.i in
    asmdma.s, 0x0016ECA4).
    """
    funcs = [(int(r["address"], 16), int(r["unit"]))
             for r in parse_toml_blocks(
                 ROOT / "config" / "pal103" / "functions.toml",
                 {"address": r"address = (0x[0-9A-Fa-f]+)", "unit": r"unit = (\d+)"})]
    runs = []
    for addr, unit in sorted(funcs):
        if not runs or runs[-1][0] != unit:
            runs.append((unit, addr))
    fdrs = {int(r["index"]): (int(r["address"], 16), int(r["procs"]))
            for r in parse_toml_blocks(
                ROOT / "config" / "pal103" / "units.toml",
                {"index": r"index = (\d+)", "address": r"address = (0x[0-9A-Fa-f]+)",
                 "procs": r"proc_count = (\d+)"})}
    for k, (unit, addr) in enumerate(runs):
        included = fdrs.get(unit + 1)
        if (included and included[1] == 0 and included[0] < addr
                and (k == 0 or runs[k - 1][1] < included[0])):
            runs[k] = (unit, included[0])
    return runs


def fold_auto_functions(lines):
    """Fold an unreferenced splat auto-function that follows a registered
    function in the same TU into it, so its symbol spans the registry extent
    (sceSifWriteBackDCache's trailing `jr $ra` at 0x0027B364)."""
    function_addrs = {int(r["address"], 16) for r in parse_toml_blocks(
        ROOT / "config" / "pal103" / "functions.toml",
        {"address": r"address = (0x[0-9A-Fa-f]+)"})}
    glabels = glabel_addresses(lines)
    starts = sorted(a for _u, a in load_tu_runs())
    text = "\n".join(lines)
    out = list(lines)
    owner = None
    for i in sorted(glabels):
        name = lines[i].split()[1]
        addr = glabels[i]
        if addr in function_addrs:
            owner = (name, addr)
            continue
        if (owner is None or not name.startswith("func_")
                or not asmtext.AUTO_NAME_RE.fullmatch(name)
                or any(owner[1] < s <= addr for s in starts)):
            owner = None
            continue
        prev = next(j for j in range(i - 1, -1, -1) if lines[j].strip()
                    and not lines[j].startswith("nonmatching"))
        if (out[prev] != f"endlabel {owner[0]}"
                or len(re.findall(rf"\b{name}\b", text)) > 3):
            owner = None
            continue
        out[prev] = None
        for j in range(prev + 1, i + 1):
            if lines[j].startswith(("nonmatching", "glabel")):
                out[j] = None
        end = next(j for j in range(i, len(lines))
                   if lines[j] == f"endlabel {name}")
        out[end] = f"endlabel {owner[0]}"
    return [l for l in out if l is not None]


def extent_end(addr, next_function, runs):
    """End of the function at `addr`: `next_function` or an earlier TU start."""
    later = [a for _u, a in runs if addr < a < next_function]
    return min(later, default=next_function)


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
