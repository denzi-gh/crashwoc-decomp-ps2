"""Locked-toolchain locations, tool resolution, and shared reporting.

Extracted from assemble_text.py / build_baseline.py / match.py. The PS2
binutils and EE GCC are Linux binaries installed under compiler/ by
tools/setup_toolchain.py; anything that invokes them runs in the Containerfile
image.
"""
import shutil
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent

BINUTILS_DIR = ROOT / "compiler" / "ps2-binutils-0.10"
AS = "mips-ps2-decompals-as"
LD = "mips-ps2-decompals-ld"
NM = "mips-ps2-decompals-nm"
OBJCOPY = "mips-ps2-decompals-objcopy"

EEGCC = ROOT / "compiler" / "ee-gcc-2.9-ee-991111-01" / "bin" / "ee-gcc"

# Pinned report generator (toolchain.lock.json `objdiff`): scores progress,
# never defines a match -- byte gates stay with the assembler/linker above.
OBJDIFF_CLI = ROOT / "compiler" / "objdiff-cli-3.7.2" / "objdiff-cli"


def tool_path(name, search_path=False):
    """Resolve a binutils tool under compiler/, optionally falling back to PATH.

    The strict default (no PATH fallback) is what the assembly/matching tools
    use so only the locked, fingerprinted binaries can produce baseline bytes;
    build_baseline.py opts into the PATH fallback it always had.
    """
    local = BINUTILS_DIR / name
    if local.is_file():
        return str(local)
    return shutil.which(name) if search_path else None


def h(value):
    """Section-spec hex string (or int) -> int."""
    return int(value, 0) if isinstance(value, str) else value


class Reporter:
    def __init__(self, label_width=32):
        self.label_width = label_width
        self.failed = False
        self.details = []

    def result(self, label, ok, detail=None):
        print(f"{label + ':':<{self.label_width}} {'PASS' if ok else 'FAIL'}")
        if not ok:
            self.failed = True
            if detail:
                self.details.append(f"{label}: {detail}")
