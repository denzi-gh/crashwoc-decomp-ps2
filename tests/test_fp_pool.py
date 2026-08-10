"""Unit tests for gen_hybrid's float/double literal-pool handling (A.2).

These cover the pure, toolchain-free parts: the bit-pattern helpers and the
inline-passthrough branch of `_rewrite_lis`. A constant whose image has a zero
low half (li.s: low 16 bits; li.d: low 32 bits) is materialized inline by the
assembler as lui+mtc1 and must be left untouched -- that path never opens a
slice file or builds a Lit4Mapper, so it needs no game files. Where the GPR
path does consult the retail slice (a `li.d` may borrow a pooled slot) the
mapper is stubbed, so what is tested here is how the slots are spent, not how
they are derived. Deriving them -- and the mapping onto retail .lit4/.lit8
slots -- is exercised by the byte gates, which do need orig/.
"""
import struct
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
import gen_hybrid
from gen_hybrid import (HybridError, _JumpTable, _double_bits, _float_bits,
                        _rewrite_lis, _rewrite_local_data)


class TestLocalDataLabelBoundary(unittest.TestCase):
    """A private jump-table label must be matched on word boundaries. A plain
    substring test wrongly attributes function A's `$L161` table to function B
    that merely branches to `$L1610` (`$L161` is a prefix), which then fails
    map_jump_tables against B's table-free retail slice."""

    def test_prefix_label_is_not_a_false_use(self):
        # mapper_box sentinel: if the boundary logic is wrong, `used` is
        # non-empty and the code reaches for the mapper -> AttributeError,
        # which is a distinct, louder failure than a wrong pass-through.
        seg = ["\tbne\t$19,$2,$L1610", "$L1610:", "\tsw\t$0,0($3)"]
        out, externs = _rewrite_local_data(
            seg, {"$L161": _JumpTable(19)},
            version="pal103", unit_dir="unit-0091",
            name="DrawCreatures", addr=0x001d2f50, mapper_box=[object()])
        self.assertEqual(out, seg)
        self.assertEqual(externs, [])


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


class _FakeSlice:
    """Stand-in for the retail slice file _rewrite_lis reads."""

    def __init__(self, text=""):
        self._text = text

    def read_text(self):
        return self._text


class _StubMapper:
    """The one Lit4Mapper method the GPR path reaches for, pre-answered.

    Keeps these tests free of orig/ and of splat output: what a real
    Lit4Mapper derives from the ELF is covered by the byte gates, what is
    covered here is how _rewrite_lis spends the slots it is handed."""

    def __init__(self, by_bits=None):
        self.by_bits = by_bits or {}
        self.calls = 0

    def map_gpr_pool8(self, slice_text, context):
        self.calls += 1
        return self.by_bits


_ZERO_POINT_249 = 0x3FCFDF3B645A1CAC


class TestRewriteGprImmediate(unittest.TestCase):
    # `li.s`/`li.d` into a *GPR* is ee-gcc handing the raw float/double bit
    # pattern to the assembler (e.g. 10.0 as the s64 arg of the dpmul
    # software-double helper), NOT a .lit4/.lit8 pool load. Sony's `as` either
    # builds the image inline -- rewritten here to `dli` over the bit pattern
    # -- or, when that would cost more, pools it into unit-owned data that
    # retail addresses %hi/%lo, in which case the slot is borrowed.
    def _run(self, seg, mapper=None, slice_text=""):
        box = [mapper] if mapper is not None else [None]
        real_slice_path = gen_hybrid._slice_path
        gen_hybrid._slice_path = lambda *a, **k: _FakeSlice(slice_text)
        try:
            out, externs = _rewrite_lis(seg, "pal103", "unit-0001", "foo",
                                        0x1000, box)
        finally:
            gen_hybrid._slice_path = real_slice_path
        return out, externs, box

    def test_double_into_gpr_becomes_dli(self):
        # 10.0 -> 0x4024000000000000 (retail's dpmul s64 argument): retail
        # builds it inline (ori+dsll32), so no slot holds it and the constant
        # falls through to the `dli` the decompals `as` expands.
        mapper = _StubMapper()
        out, externs, _ = self._run(["\tli.d\t$5,1.0e1", "\tjr\t$31"], mapper)
        self.assertEqual(out, ["\tdli\t$5,0x4024000000000000", "\tjr\t$31"])
        self.assertEqual(externs, [])       # inline, no borrowed slot
        self.assertEqual(mapper.calls, 1)

    def test_double_into_gpr_borrows_pooled_slots_in_order(self):
        # game/crate FindLocalCrate: three source-level `0.249` literals, which
        # gcc 2.95 does not dedup, so retail holds three identical slots and
        # each load addresses its own -- spent positionally, in slice order.
        mapper = _StubMapper({_ZERO_POINT_249:
                              ["D_0061EE90", "D_0061EE98", "D_0061EEA0"]})
        out, externs, _ = self._run(
            ["\tli.d\t$5,2.48999999999999999112e-1"] * 3, mapper)
        self.assertEqual(out, [
            line
            for sym in ("D_0061EE90", "D_0061EE98", "D_0061EEA0")
            for line in ("\t.set\tnoat",
                         f"\tlui\t$at,%hi({sym})",
                         f"\tld\t$5,%lo({sym})($at)",
                         "\t.set\tat")])
        self.assertEqual(externs, ["\t.extern\tD_0061EE90, 8",
                                   "\t.extern\tD_0061EE98, 8",
                                   "\t.extern\tD_0061EEA0, 8"])

    def test_double_into_gpr_falls_back_when_slots_run_out(self):
        # More pooled loads than retail slots means the C shape does not line
        # up with retail; the extra one takes the inline path and the byte gate
        # then fails loudly rather than silently borrowing a slot twice.
        mapper = _StubMapper({_ZERO_POINT_249: ["D_0061EE90"]})
        out, externs, _ = self._run(
            ["\tli.d\t$5,2.48999999999999999112e-1"] * 2, mapper)
        self.assertEqual(out[-1], f"\tdli\t$5,0x{_ZERO_POINT_249:016X}")
        self.assertEqual(externs, ["\t.extern\tD_0061EE90, 8"])

    def test_float_into_gpr_becomes_dli_zero_extended(self):
        # 0.5 -> 0x3F000000, zero-extended into the 64-bit GPR. The 4-byte
        # form never borrows -- a float image is short enough to coincide with
        # unrelated data the slice happens to reference -- so no mapper is
        # built and the path stays pure.
        out, externs, box = self._run(["\tli.s\t$4,0.5"])
        self.assertEqual(out, ["\tdli\t$4,0x3F000000"])
        self.assertEqual(externs, [])
        self.assertIsNone(box[0])           # Lit4Mapper never constructed

    def test_high_bit_float_is_not_sign_extended(self):
        # -1.0 -> 0xBF800000 has bit 31 set; `dli` must keep bits 32-63 zero
        # rather than `lui`-sign-extending them (the whole point of using dli).
        out, _, _ = self._run(["\tli.s\t$6,-1.0"])
        self.assertEqual(out, ["\tdli\t$6,0xBF800000"])

    def test_named_gpr_register_is_accepted(self):
        # ee-gcc may spell the GPR by ABI name ($a1) rather than number ($5);
        # the $fN negative lookahead must still route it here, not to the pool.
        out, externs, _ = self._run(["\tli.d\t$a1,1.0e1"], _StubMapper())
        self.assertEqual(out, ["\tdli\t$a1,0x4024000000000000"])
        self.assertEqual(externs, [])

    def test_unparseable_gpr_constant_raises(self):
        with self.assertRaises(HybridError):
            self._run(["\tli.d\t$5,notanumber"])


if __name__ == "__main__":
    unittest.main()
