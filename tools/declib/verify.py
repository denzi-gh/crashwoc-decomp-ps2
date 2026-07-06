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


def defined_functions(nm_bin, obj):
    out = subprocess.run([nm_bin, str(obj)], check=True,
                         capture_output=True, text=True).stdout
    names = []
    for line in out.splitlines():
        parts = line.split()
        if len(parts) == 3 and parts[1] in ("T", "t"):
            names.append(parts[2])
    return names


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
