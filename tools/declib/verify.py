"""Link-at-address and object symbol inspection for byte verification.

Extracted verbatim from match.py: link a candidate object's .text at its true
address (externals resolved by defsym, exactly as the reconstruction does) so
its bytes can be compared function-by-function against the retail image.
"""
import re
import subprocess
from pathlib import Path

from .toolchain import LD, NM, OBJCOPY, tool_path

_GP = 0x00634970
# Retail .lit4 base (config/pal103/sections.json). A compiled object's own
# literal pool is placed here so its R_MIPS_GPREL16 relocs resolve and the
# .text link succeeds. The pool's internal layout differs from retail's, so
# functions that load pool constants still (honestly) compare as DIFF here;
# gen_hybrid's per-function slot mapping is what makes them byte-exact in
# the verified matching build.
_LIT4 = 0x0062C980


def defined_functions(nm_bin, obj):
    out = subprocess.run([nm_bin, str(obj)], check=True,
                         capture_output=True, text=True).stdout
    names = []
    for line in out.splitlines():
        parts = line.split()
        if len(parts) == 3 and parts[1] in ("T", "t"):
            names.append(parts[2])
    return names


_DUP_STATIC_RE = re.compile(r"^(.+)__[0-9A-F]{8}$")


def defined_function_offsets(nm_bin, obj):
    """{name: .text offset} for an object's defined text symbols.

    In a relocatable object nm's symbol value is the offset within the
    symbol's section, so this locates each function inside the object's
    .text regardless of source order or address gaps between functions.

    Duplicated statics (NAME__<VRAM>) are also listed under NAME.
    """
    out = subprocess.run([nm_bin, str(obj)], check=True,
                         capture_output=True, text=True).stdout
    offsets = {}
    for line in out.splitlines():
        parts = line.split()
        if len(parts) == 3 and parts[1] in ("T", "t"):
            offsets[parts[2]] = int(parts[0], 16)
    for name, offset in list(offsets.items()):
        m = _DUP_STATIC_RE.match(name)
        if m:
            offsets.setdefault(m.group(1), offset)
    return offsets


def undefined_externals(nm_bin, obj):
    out = subprocess.run([nm_bin, str(obj)], check=True,
                         capture_output=True, text=True).stdout
    return {line.split()[-1] for line in out.splitlines()
            if re.match(r"\s+U ", line)}


class ToolError(subprocess.CalledProcessError):
    """CalledProcessError whose message carries the tool's stderr.

    The base class prints only the command and exit status, which turns every
    ld failure into an opaque "returned non-zero exit status 1". Subclassing
    keeps existing `except CalledProcessError` handlers working unchanged."""

    def __str__(self):
        err = (self.stderr or "").strip()
        return super().__str__() + (f"\n{err}" if err else "")


def _run_tool(cmd):
    proc = subprocess.run(cmd, capture_output=True, text=True)
    if proc.returncode != 0:
        raise ToolError(proc.returncode, cmd, proc.stdout, proc.stderr)
    return proc


def link_unit_at(obj, base_addr, pieces, defsyms, tmp):
    """Link a hybrid with data pieces at their true addresses.

    `.text` goes to base_addr and every piece ({name, start}) into its own
    output section at its address, so relocations from the code into C data
    resolve (link_text_at would discard the pieces). Returns (text bytes,
    {piece name: bytes}, {symbol: address} of the linked image).
    """
    ld_bin, objcopy_bin = tool_path(LD), tool_path(OBJCOPY)
    nm_bin = tool_path(NM)
    o = Path(obj).as_posix()
    lines = ["SECTIONS {", f"  . = 0x{base_addr:08X};",
             f"  .text : SUBALIGN(1) {{ {o}(.text) }}",
             f"  . = 0x{_LIT4:08X};", f"  .lit4 : {{ {o}(.lit4) }}"]
    for p in sorted(pieces, key=lambda p: p["start"]):
        lines += [f"  . = 0x{p['start']:08X};",
                  f"  {p['name']} : SUBALIGN(1) {{ {o}({p['name']}) }}"]
    lines += [f"  _gp = 0x{_GP:08X};",
              "  /DISCARD/ : { *(.pdr) *(.reginfo) *(.mdebug*) *(.comment) "
              "*(.gnu.attributes) *(.MIPS.abiflags) }", "}"]
    lines += [f"{n} = 0x{a:08X};" for n, a in defsyms.items()]
    ld = tmp / "link.ld"
    ld.write_text("\n".join(lines) + "\n")
    elf_out = tmp / "linked.elf"
    _run_tool([ld_bin, "-EL", "-T", str(ld), "-o", str(elf_out), str(obj)])
    out = {}
    for name in [".text"] + [p["name"] for p in pieces]:
        bin_out = tmp / f"{name.strip('.')}.bin"
        _run_tool([objcopy_bin, "-O", "binary", f"--only-section={name}",
                   str(elf_out), str(bin_out)])
        out[name] = bin_out.read_bytes()
    symbols = {}
    for line in _run_tool([nm_bin, str(elf_out)]).stdout.splitlines():
        parts = line.split()
        if len(parts) == 3:
            symbols[parts[2]] = int(parts[0], 16)
    return out.pop(".text"), out, symbols


def link_text_at(obj, base_addr, defsyms, tmp):
    """Link obj's .text at base_addr (externals defsym'd) and return raw bytes."""
    ld_bin, objcopy_bin = tool_path(LD), tool_path(OBJCOPY)
    script = (f"SECTIONS {{\n  . = 0x{base_addr:08X};\n"
              f"  .text : SUBALIGN(1) {{ {Path(obj).as_posix()}(.text) }}\n"
              f"  . = 0x{_LIT4:08X};\n"
              f"  .lit4 : {{ {Path(obj).as_posix()}(.lit4) }}\n"
              f"  _gp = 0x{_GP:08X};\n  /DISCARD/ : {{ *(*) }}\n}}\n"
              + "\n".join(f"{n} = 0x{a:08X};" for n, a in defsyms.items()) + "\n")
    ld = tmp / "link.ld"
    ld.write_text(script)
    elf_out, bin_out = tmp / "linked.elf", tmp / "linked.bin"
    _run_tool([ld_bin, "-EL", "-T", str(ld), "-o", str(elf_out), str(obj)])
    _run_tool([objcopy_bin, "-O", "binary", "--only-section=.text",
               str(elf_out), str(bin_out)])
    return bin_out.read_bytes()
