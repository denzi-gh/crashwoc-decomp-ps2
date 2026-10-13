"""Locked-assembler checks for internal alignment versus section tail padding."""
import os
import struct
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from declib.toolchain import AS, tool_path
from gen_hybrid import HybridError, _assemble_unit


def text_section(path):
    data = path.read_bytes()
    shoff = struct.unpack_from("<I", data, 0x20)[0]
    shentsize, shnum, shstrndx = struct.unpack_from("<HHH", data, 0x2E)
    names = struct.unpack_from("<I", data,
                               shoff + shentsize * shstrndx + 0x10)[0]
    for index in range(shnum):
        header = shoff + index * shentsize
        name = names + struct.unpack_from("<I", data, header)[0]
        if data[name:data.index(b"\x00", name)] == b".text":
            offset, size = struct.unpack_from("<II", data, header + 0x10)
            alignment = struct.unpack_from("<I", data, header + 0x20)[0]
            return data[offset:offset + size], alignment
    raise AssertionError("missing .text section")


@unittest.skipUnless(os.name == "posix" and tool_path(AS),
                     "requires the locked assembler in the dev container")
class TestHybridAlignment(unittest.TestCase):
    def test_internal_padding_is_preserved_without_rounding_section_end(self):
        with tempfile.TemporaryDirectory() as directory:
            obj = Path(directory) / "alignment.o"
            _assemble_unit(obj, [".text", ".align 3", ".word 0x24020001",
                                 ".p2align 3", ".word 0x24030002"])
            text, alignment = text_section(obj)
            # Four bytes of internal padding remain. The 12-byte body must
            # not acquire another four bytes at its end due to sh_addralign.
            self.assertEqual(text, struct.pack("<III", 0x24020001, 0,
                                               0x24030002))
            self.assertEqual(alignment, 8)

    def test_canonical_extent_overrun_is_still_rejected(self):
        with tempfile.TemporaryDirectory() as directory:
            obj = Path(directory) / "overrun.o"
            with self.assertRaisesRegex(HybridError, "org backwards"):
                _assemble_unit(obj, [".text", ".align 3", ".word 0",
                                     ".word 0", ".org 4, 0"])


if __name__ == "__main__":
    unittest.main()
