"""Link-at-address and object symbol inspection for byte verification.

Extracted verbatim from match.py: link a candidate object's .text at its true
address (externals resolved by defsym, exactly as the reconstruction does) so
its bytes can be compared function-by-function against the retail image.
"""
import re
import subprocess
from pathlib import Path

from .toolchain import LD, OBJCOPY, tool_path

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


def defined_function_offsets(nm_bin, obj):
    """{name: .text offset} for an object's defined text symbols.

    In a relocatable object nm's symbol value is the offset within the
    symbol's section, so this locates each function inside the object's
    .text regardless of source order or address gaps between functions.
    """
    out = subprocess.run([nm_bin, str(obj)], check=True,
                         capture_output=True, text=True).stdout
    offsets = {}
    for line in out.splitlines():
        parts = line.split()
        if len(parts) == 3 and parts[1] in ("T", "t"):
            offsets[parts[2]] = int(parts[0], 16)
    return offsets


def undefined_externals(nm_bin, obj):
    out = subprocess.run([nm_bin, str(obj)], check=True,
                         capture_output=True, text=True).stdout
    return {line.split()[-1] for line in out.splitlines()
            if re.match(r"\s+U ", line)}


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
    subprocess.run([ld_bin, "-EL", "-T", str(ld), "-o", str(elf_out), str(obj)],
                   check=True, capture_output=True, text=True)
    subprocess.run([objcopy_bin, "-O", "binary", "--only-section=.text",
                    str(elf_out), str(bin_out)], check=True, capture_output=True)
    return bin_out.read_bytes()
