"""Unit tests for tools/dump_data.py's rendering (no game files).

The image is a stub: a few sections, symbols and functions, so pointer
resolution, string detection and float literals are tested on known bytes.
"""
import struct
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from declib.stabs import TypeTable  # noqa: E402
from dump_data import (DumpError, Renderer, c_escape, declarator,  # noqa: E402
                       float_text, parse_simple_type, simple_table)


class StubImage:
    """Just enough of dump_data.Image for Renderer."""

    def __init__(self):
        self.rodata = 0x00613480
        self.blob = b"hello\0" + bytes(10)
        self.functions = {0x00100100: ("Main", False),
                          0x00100200: ("Dup__00100200", True)}
        self.by_address = {0x00293700: {"name": "table", "address": 0x00293700}}
        self.sized = [(0x00293700, 0x00293710,
                       {"name": "table", "address": 0x00293700})]

    def section_of(self, addr):
        if self.rodata <= addr < self.rodata + len(self.blob):
            return (".rodata", self.rodata, self.rodata + len(self.blob), 0, 1)
        return None

    def bytes(self, addr, size):
        off = addr - self.rodata
        return self.blob[off:off + size]


def render(type_text, blob):
    table, ref = simple_table(type_text)
    r = Renderer(StubImage(), table)
    return r.render(ref, blob), r


class TestFloatText(unittest.TestCase):
    def test_shortest_round_trip(self):
        bits = struct.unpack("<I", struct.pack("<f", 0.3))[0]
        self.assertEqual(float_text(bits, 4), ("0.3f", None))
        bits = struct.unpack("<Q", struct.pack("<d", 1.5))[0]
        self.assertEqual(float_text(bits, 8), ("1.5", None))

    def test_integral_values_avoid_exponents(self):
        bits = struct.unpack("<Q", struct.pack("<d", 1e10))[0]
        self.assertEqual(float_text(bits, 8)[0], "10000000000.0")

    def test_special_values_are_flagged(self):
        self.assertEqual(float_text(0x7FC00000, 4)[1], "nan")
        self.assertEqual(float_text(0x7F800000, 4)[1], "inf")
        self.assertEqual(float_text(0x00000001, 4)[1], "denormal")


class TestTypes(unittest.TestCase):
    def test_parse_simple_type(self):
        self.assertEqual(parse_simple_type("const u32 [4][2]"), ("u32", 0, [4, 2]))
        self.assertEqual(parse_simple_type("char *[3]"), ("char", 1, [3]))
        self.assertEqual(parse_simple_type("double[]"), ("double", 0, [None]))

    def test_struct_needs_a_probe(self):
        self.assertIsNone(simple_table("struct nuvec_s"))

    def test_declarator(self):
        self.assertEqual(declarator("struct x_s[4]", "p"), "struct x_s p[4]")
        self.assertEqual(declarator("char *", "p"), "char *p")


class TestRender(unittest.TestCase):
    def test_scalars(self):
        self.assertEqual(render("int[3]", struct.pack("<3i", 1, -2, 0x12345))[0],
                         "{ 1, -2, 0x12345 }")
        self.assertEqual(render("u8[2]", b"\xff\x01")[0], "{ 255, 1 }")

    def test_all_zero_aggregate(self):
        self.assertEqual(render("float[4]", bytes(16))[0], "{0}")

    def test_char_array_as_string(self):
        self.assertEqual(render("char[8]", b"a\"b\n\0\0\0\0")[0], '"a\\"b\\n"')
        # An embedded NUL keeps the brace form.
        self.assertEqual(render("char[4]", b"a\0b\0")[0], "{ 97, 0, 98, 0 }")

    def test_pointers(self):
        words = struct.pack("<6I", 0, 0x00100100, 0x00293700, 0x00293708,
                            0x00613480, 0x00700000)
        text, r = render("void *[6]", words)
        self.assertEqual(text, '{\n    0,\n    Main,\n    &table,\n'
                               '    (char *)&table + 0x8,\n    "hello",\n'
                               '    D_00700000 /* unresolved */\n}')
        self.assertEqual(render("void *", struct.pack("<I", 0x00100200))[1].notes,
                         ["Dup__00100200: a duplicated static, declared per unit"])

    def test_struct_from_stabs(self):
        t = TypeTable()
        t.add("float:t12=r1;4;0;")
        t.add("int:t1=r1;-2147483648;2147483647;")
        _n, _d, ref = t.add("v:T20=s8x:12,0,32;n:1,32,32;;")
        r = Renderer(StubImage(), t)
        self.assertEqual(r.render(ref, struct.pack("<fi", 2.0, 7)),
                         "{\n    2.0f /* x */,\n    7 /* n */\n}")

    def test_bit_fields_are_refused(self):
        t = TypeTable()
        t.add("int:t1=r1;-2147483648;2147483647;")
        _n, _d, ref = t.add("b:T21=s4a:1,0,3;b:1,3,29;;")
        with self.assertRaises(DumpError):
            Renderer(StubImage(), t).render(ref, b"\1\0\0\0")

    def test_escape(self):
        self.assertEqual(c_escape(b"\x01\\"), '"\\001\\\\"')


if __name__ == "__main__":
    unittest.main()
