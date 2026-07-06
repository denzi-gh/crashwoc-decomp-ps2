#!/usr/bin/env python3
"""Translation-unit split: cut .text into per-TU assembly, keep the image exact.

The monolithic assembly baseline proved the whole `.text` assembles back to the
retail bytes. This step splits that single file into one assembly object per
translation unit, following the boundaries the retail `.mdebug` section records
(config/pal103/functions.toml -> which unit each procedure belongs to), and
proves the *final loaded image* still reconstructs exactly when every TU is
assembled separately and linked with the other sections.

Two levels of proof, mirroring the binary baseline:

  * Always (no toolchain): the per-TU bodies are a strict partition of the
    monolithic body -- same lines, same order, nothing added or dropped -- so
    their concatenation is byte-identical. Checked directly.

  * With the PS2 binutils (Linux x86-64; run in the Containerfile image): each
    TU is assembled on its own, then all TU objects are linked together with the
    six non-.text sections carried as `.incbin` (exactly as the binary baseline
    does) into the full PT_LOAD image, which must match the packaged SHA-256.

Splitting per TU is also what finally retires the duplicate-static problem: the
four names shared across TUs (ReadNuIFFGeomSkin, ...) now live in separate
objects. They are still emitted under unique `NAME__<vram>` labels (reusing the
monolithic baseline's byte-lossless disambiguation) so their `.NON_MATCHING`
markers never collide at link time either.

Nothing game-derived is committed: the per-TU `.s`, the objects, the `.incbin`
blobs, the link script and the linked ELF all land in gitignored build/split/.
This script and the addresses/units it reads are the only tracked inputs.

Exit status is 0 only if the split is lossless and (when the toolchain is
available) the linked image matches the packaged hash.
"""
import argparse
import hashlib
import re
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
import assemble_text as at        # disambiguate, resolve, symbol/tool helpers
import build_baseline as bb       # load_target, derive_tiling, parse_linker_script

LABEL_WIDTH = 34
_GP = 0x00634970                  # config/pal103/sections.json; also in linker.ld


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
            while j < len(lines) and at.instr_at(lines, j) is None:
                j += 1
            if j < len(lines):
                out[i] = at.instr_at(lines, j)[0]
    return out


# A line that introduces a function block (leading run swept up with the block).
_BLOCK_LEAD = ("nonmatching ", "matching ", ".align")


def is_lead(line):
    s = line.strip()
    if s == "" or line.startswith(_BLOCK_LEAD):
        return True
    # A standalone comment (e.g. "/* Handwritten function */") but not an
    # instruction line, which also starts with "/*".
    return s.startswith("/*") and at.INSTR_RE.search(line) is None


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
        info = at.INSTR_RE.search(line)
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


def write_tu_files(chunks, runs, unit_names, prologue, out_dir):
    """Write prologue+chunk to build/split/NNN_name.s; return the object stems."""
    stems = []
    for chunk, (unit, _addr) in zip(chunks, runs):
        stem = f"{unit:03d}_{unit_names.get(unit, 'unit')}"
        (out_dir / f"{stem}.s").write_text(
            "\n".join(prologue + [""] + chunk) + "\n")
        stems.append(stem)
    return stems


def write_incbin_objects(objects, elf, out_dir):
    """`.incbin` wrappers for the six non-.text loaded sections (game bytes)."""
    for obj in objects:
        if obj["name"] == "text":
            continue
        blob = elf[obj["offset"]:obj["offset"] + obj["size"]]
        (out_dir / f"{obj['name']}.bin").write_bytes(blob)
        (out_dir / f"{obj['name']}.s").write_text(
            "/* Generated by tools/split_text.py -- do not edit, do not commit "
            "(game-derived). */\n"
            f'\t.section .split.{obj["name"]}, "a", @progbits\n'
            f'\t.incbin "{(out_dir / (obj["name"] + ".bin")).as_posix()}"\n')


def check_lossless(reporter, chunks, body):
    """The chunks must be a strict, order-preserving partition of the body."""
    joined = [l for chunk in chunks for l in chunk]
    ok = joined == body
    detail = None
    if not ok:
        detail = f"{len(joined)} chunk lines vs {len(body)} body lines"
    reporter.result("Split lossless (covers .text)", ok, detail)


def build_link_script(text_addr, tu_objs, incbin_objects, nobits, defsyms):
    """Full-image link script: TU objects for .text, incbin for the rest."""
    # SUBALIGN(1): concatenate the TU objects with no inter-object realignment.
    # The objects carry no `.align` (all were no-ops -- see split_alignment), so
    # each is a pure instruction stream and simple concatenation is exact.
    L = ["SECTIONS", "{", f"    . = 0x{text_addr:08X};",
         "    .text : SUBALIGN(1)", "    {"]
    L += [f"        {o.as_posix()}(.text)" for o in tu_objs]
    L.append("    }")
    for o in incbin_objects:
        if o["name"] == "text":
            continue
        L.append(f"    . = 0x{o['vaddr']:08X};")
        L.append(f"    .{o['name']} : {{ *(.split.{o['name']}) }}")
    L.append("    . = 0x00633000;")
    L.append(f"    .sbss (NOLOAD) : {{ . += 0x{nobits['sbss']:06X}; }}")
    L.append("    . = 0x00633400;")
    L.append(f"    .bss  (NOLOAD) : {{ . += 0x{nobits['bss']:06X}; }}")
    L.append(f"    _gp = 0x{_GP:08X};")
    L.append("    /DISCARD/ : { *(.pdr) *(.reginfo) *(.mdebug*) *(.comment) "
             "*(.gnu.attributes) }")
    L.append("}")
    L += [f"{n} = 0x{a:08X};" for n, a in sorted(defsyms.items())]
    return "\n".join(L) + "\n"


def nm_defined_undefined(nm_bin, obj):
    """(defined_names, undefined_names) from one object's symbol table."""
    out = subprocess.run([nm_bin, str(obj)], check=True,
                         capture_output=True, text=True).stdout
    defined, undefined = set(), set()
    for line in out.splitlines():
        parts = line.split()
        if len(parts) == 2 and parts[0] in ("U", "w"):
            undefined.add(parts[1])
        elif len(parts) == 3:
            defined.add(parts[2])
    return defined, undefined


def link_full_image(reporter, tu_stems, incbin_objects, nobits, text_addr,
                    elf, pt_load, out_dir):
    """Assemble every TU, link the whole image, compare the packaged hash."""
    as_bin = at.tool_path(at.AS)
    ld_bin = at.tool_path(at.LD)
    nm_bin = at.tool_path(at.NM)
    objcopy_bin = at.tool_path(at.OBJCOPY)
    if not all((as_bin, ld_bin, nm_bin, objcopy_bin)):
        reporter.result("Linked image matches packaged hash", False,
                        "PS2 binutils not found (Linux x86-64; run in the "
                        "Containerfile image)")
        return
    macro_dir = ROOT / "build" / "include"

    tu_objs, defined, undefined = [], set(), set()
    try:
        for stem in tu_stems:
            obj = out_dir / f"{stem}.o"
            subprocess.run([as_bin, "-EL", "-march=r5900", "-I", str(macro_dir),
                            "-o", str(obj), str(out_dir / f"{stem}.s")],
                           check=True, capture_output=True, text=True)
            d, u = nm_defined_undefined(nm_bin, obj)
            defined |= d
            undefined |= u
            tu_objs.append(obj)
        for obj in incbin_objects:          # assemble the six incbin wrappers
            if obj["name"] == "text":
                continue
            # Same -march as the TU objects so the ELF ABI flags match and ld
            # does not reject "linking 32-bit code with 64-bit code".
            subprocess.run([as_bin, "-EL", "-march=r5900",
                            "-o", str(out_dir / f"{obj['name']}.o"),
                            str(out_dir / f"{obj['name']}.s")],
                           check=True, capture_output=True, text=True)
    except subprocess.CalledProcessError as exc:
        reporter.result("Assemble per-TU objects", False,
                        (exc.stderr or "as failed").strip().splitlines()[0])
        return
    except OSError as exc:
        reporter.result("Assemble per-TU objects", False,
                        f"cannot execute PS2 as here ({exc}); Linux x86-64 only")
        return
    reporter.result("Assemble per-TU objects", True)

    external = (undefined - defined) - {"_gp"}
    defsyms, unresolved = at.resolve(sorted(external),
                                     at.load_symbol_addrs("pal103"))
    reporter.result("Resolve external symbols", not unresolved,
                    None if not unresolved
                    else f"{len(unresolved)} unresolved, e.g. {unresolved[:5]}")
    if unresolved:
        return

    incbin_o = [str(out_dir / f"{o['name']}.o")
                for o in incbin_objects if o["name"] != "text"]
    script = build_link_script(text_addr, tu_objs, incbin_objects, nobits, defsyms)
    (out_dir / "split.ld").write_text(script)
    elf_out = out_dir / "split.elf"
    bin_out = out_dir / "split.bin"
    try:
        subprocess.run([ld_bin, "-EL", "-T", str(out_dir / "split.ld"),
                        "-o", str(elf_out), *[str(o) for o in tu_objs], *incbin_o],
                       check=True, capture_output=True, text=True)
        subprocess.run([objcopy_bin, "-O", "binary",
                        "--only-section=.text", "--only-section=.vutext",
                        "--only-section=.orphan", "--only-section=.data",
                        "--only-section=.rodata", "--only-section=.lit4",
                        "--only-section=.sdata", str(elf_out), str(bin_out)],
                       check=True, capture_output=True, text=True)
    except subprocess.CalledProcessError as exc:
        reporter.result("Link full loaded image", False,
                        (exc.stderr or "ld failed").strip().splitlines()[0])
        return
    reporter.result("Link full loaded image", True)

    load_off = bb.h(pt_load["offset"])
    filesz = bb.h(pt_load["filesz"])
    want = hashlib.sha256(elf[load_off:load_off + filesz]).hexdigest()
    got = hashlib.sha256(bin_out.read_bytes()).hexdigest()
    reporter.result("Linked image matches packaged hash", got == want,
                    None if got == want else f"linked sha256 {got} != {want}")


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--version", default="pal103")
    args = parser.parse_args()

    text_s = ROOT / "asm" / "text.s"
    if not text_s.is_file() or not (ROOT / "build" / "include" / "macro.inc").is_file():
        print("asm/text.s or build/include/macro.inc missing; run "
              "`python configure.py` first (gitignored, game-derived).",
              file=sys.stderr)
        return 2

    elf, sections_spec, pt_load = bb.load_target(args.version)
    text_addr = bb.h(next(s for s in sections_spec["sections"]
                          if s["name"] == ".text")["addr"])
    incbin_objects, _gaps = bb.derive_tiling(elf, sections_spec, pt_load)
    _addrs, nobits = bb.parse_linker_script((ROOT / "linker.ld").read_text())

    reporter = at.Reporter()
    reporter.__dict__.setdefault("failed", False)

    # Reuse the monolithic baseline's byte-lossless disambiguation, then split.
    mono, ok = at.disambiguate(text_s.read_text(), reporter)
    if not ok:
        return _finish(reporter)
    lines = drop_alignment(mono.splitlines(), reporter)
    if lines is None:
        return _finish(reporter)

    runs = load_tu_runs()
    cut_addrs = [addr for _unit, addr in runs[1:]]   # runs[0] heads the file
    body_start = prologue_end(lines)
    prologue = lines[:body_start]
    chunks = split_body(lines, body_start, cut_addrs)

    print(f"Translation-unit split: {args.version}")
    print(f"  {len(runs)} .text translation units "
          f"(of {len(load_units())} mdebug units)")
    print(f"  6 non-.text sections carried as incbin\n")

    if len(chunks) != len(runs):
        reporter.result("Split lossless (covers .text)", False,
                        f"{len(chunks)} chunks for {len(runs)} TUs")
        return _finish(reporter)
    check_lossless(reporter, chunks, lines[body_start:])

    out_dir = ROOT / "build" / "split"
    if out_dir.exists():
        shutil.rmtree(out_dir)
    out_dir.mkdir(parents=True)
    tu_stems = write_tu_files(chunks, runs, load_units(), prologue, out_dir)
    write_incbin_objects(incbin_objects, elf, out_dir)
    print(f"  wrote {len(tu_stems)} TU objects -> "
          f"{out_dir.relative_to(ROOT).as_posix()}/ (gitignored)\n")

    link_full_image(reporter, tu_stems, incbin_objects, nobits, text_addr,
                    elf, pt_load, out_dir)

    return _finish(reporter)


def _finish(reporter):
    print()
    if reporter.failed:
        print("Translation-unit split FAILED:")
        for detail in reporter.details:
            print(f"  - {detail}")
        return 1
    print("Translation-unit split OK.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
