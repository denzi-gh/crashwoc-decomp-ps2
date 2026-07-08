"""Unit tests for gen_hybrid's float/double literal-pool handling (A.2).

These cover the pure, toolchain-free parts: the bit-pattern helpers and the
inline-passthrough branch of `_rewrite_lis`. A constant whose image has a zero
low half (li.s: low 16 bits; li.d: low 32 bits) is materialized inline by the
assembler as lui+mtc1 and must be left untouched -- that path never opens a
slice file or builds a Lit4Mapper, so it needs no game files. The pool-bound
mapping onto retail .lit4/.lit8 slots is exercised by the byte gates (which do
need orig/), not here.
"""
import struct
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from gen_hybrid import HybridError, _double_bits, _float_bits, _rewrite_lis


class TestBitHelpers(unittest.TestCase):
    def test_float_bits(self):
        self.assertEqual(_float_bits("1.0"), 0x3F800000)
        self.assertEqual(_float_bits("1.0"),
                         struct.unpack("<I", struct.pack("<f", 1.0))[0])

    def test_double_bits(self):
        self.assertEqual(_double_bits("1.0"), 0x3FF0000000000000)
        self.assertEqual(_double_bits("2.0"), 0x4000000000000000)

    def test_double_low_word_nonzero(self):
        # 0.1 is not exactly representable: its low 32 bits are set, so it is
        # a genuine .lit8 pool constant, not an inline load.
        self.assertNotEqual(_double_bits("0.1") & 0xFFFFFFFF, 0)


class TestRewriteInlinePassthrough(unittest.TestCase):
    # No slice files and no ELF are touched: the mapper box stays None because
    # every constant here is inline-representable.
    def _run(self, seg):
        box = [None]
        out, externs = _rewrite_lis(seg, "pal103", "unit-0001", "foo",
                                    0x1000, box)
        return out, externs, box

    def test_inline_float_and_double_pass_through(self):
        seg = ["\tli.s\t$f0,1.0",       # 0x3F800000, low 16 zero -> inline
               "\tli.d\t$f2,2.0",       # 0x4000...0000, low 32 zero -> inline
               "\tjr\t$31"]
        out, externs, box = self._run(seg)
        self.assertEqual(out, seg)          # untouched
        self.assertEqual(externs, [])       # no borrowed slot
        self.assertIsNone(box[0])           # Lit4Mapper never constructed

    def test_no_fp_load_is_a_noop(self):
        seg = ["\taddiu\t$sp,$sp,-16", "\tjr\t$31"]
        out, externs, box = self._run(seg)
        self.assertEqual(out, seg)
        self.assertEqual(externs, [])
        self.assertIsNone(box[0])

    def test_malformed_fp_load_raises(self):
        # Looks like an fp-pool load but does not parse: fail loudly rather
        # than silently emitting an unhandled literal pool.
        with self.assertRaises(HybridError):
            self._run(["\tli.s\t$f0"])


class TestRewriteGprImmediate(unittest.TestCase):
    # `li.s`/`li.d` into a *GPR* is ee-gcc materializing the raw float/double
    # bit pattern inline (e.g. 10.0 as the s64 arg of the dpmul software-double
    # helper), NOT a .lit4/.lit8 pool load. It is rewritten to `dli` over the
    # bit pattern -- no slice, no ELF, no mapper. This path is pure.
    def _run(self, seg):
        box = [None]
        out, externs = _rewrite_lis(seg, "pal103", "unit-0001", "foo",
                                    0x1000, box)
        return out, externs, box

    def test_double_into_gpr_becomes_dli(self):
        # 10.0 -> 0x4024000000000000 (retail's dpmul s64 argument).
        out, externs, box = self._run(["\tli.d\t$5,1.0e1", "\tjr\t$31"])
        self.assertEqual(out, ["\tdli\t$5,0x4024000000000000", "\tjr\t$31"])
        self.assertEqual(externs, [])       # inline, no borrowed slot
        self.assertIsNone(box[0])           # Lit4Mapper never constructed

    def test_float_into_gpr_becomes_dli_zero_extended(self):
        # 0.5 -> 0x3F000000, zero-extended into the 64-bit GPR.
        out, _, _ = self._run(["\tli.s\t$4,0.5"])
        self.assertEqual(out, ["\tdli\t$4,0x3F000000"])

    def test_high_bit_float_is_not_sign_extended(self):
        # -1.0 -> 0xBF800000 has bit 31 set; `dli` must keep bits 32-63 zero
        # rather than `lui`-sign-extending them (the whole point of using dli).
        out, _, _ = self._run(["\tli.s\t$6,-1.0"])
        self.assertEqual(out, ["\tdli\t$6,0xBF800000"])

    def test_named_gpr_register_is_accepted(self):
        # ee-gcc may spell the GPR by ABI name ($a1) rather than number ($5);
        # the $fN negative lookahead must still route it here, not to the pool.
        out, externs, box = self._run(["\tli.d\t$a1,1.0e1"])
        self.assertEqual(out, ["\tdli\t$a1,0x4024000000000000"])
        self.assertIsNone(box[0])

    def test_unparseable_gpr_constant_raises(self):
        with self.assertRaises(HybridError):
            self._run(["\tli.d\t$5,notanumber"])


if __name__ == "__main__":
    unittest.main()
