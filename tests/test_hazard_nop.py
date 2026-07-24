"""Unit tests for gen_hybrid's mtc1 -> FP-compare hazard nop (`_sonyize_seg`).

Toolchain-free: these pin the *rule*, not the bytes -- the byte gates
(verify-promoted / verify-loaded, which need orig/) carry the proof that the
rule reproduces retail.

The rule, from two retail functions:
  fsign @0x221530        mtc1 $at,$f0 / nop / c.le.s $f1,$f12   (disjoint regs)
  ApplyFriction @0x2208f8 mtc1 $zero,$f2 / lwc1 $f1 / c.lt.s $f2,$f1  (no nop)
Sony's `as` nops whenever a compare is *adjacent* to an mtc1, regardless of any
register dependency; the decompals `as` only on a dependency. One instruction of
separation is enough for both, so only the adjacent case is materialized.
"""
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from gen_hybrid import _sonyize_seg


def _insns(seg):
    """Emitted lines minus comments/blanks, stripped -- what the assembler sees."""
    return [l.strip() for l in _sonyize_seg(seg)
            if l.strip() and not l.strip().startswith("#")]


class TestHazardNop(unittest.TestCase):

    def test_literal_mtc1_before_compare_gets_a_nop(self):
        got = _insns(["\tmtc1\t$1,$f0", "\tc.le.s\t$f1,$f12"])
        self.assertEqual(got, ["mtc1\t$1,$f0", "nop", "c.le.s\t$f1,$f12"])

    def test_inline_lis_before_compare_gets_a_nop(self):
        # `li.s $f0,1.0` is an assembler macro -> lui $at + mtc1 $at,$f0, so the
        # hazard is invisible in the .s. This is retail fsign.
        got = _insns(["\tli.s\t$f0,1.00000000000000000000e0",
                      "\tc.le.s\t$f1,$f12"])
        self.assertEqual(got, ["li.s\t$f0,1.00000000000000000000e0", "nop",
                               "c.le.s\t$f1,$f12"])

    def test_nop_is_added_even_when_registers_are_dependent(self):
        # The decompals `as` would insert this one itself, but an explicit nop is
        # idempotent: with it ahead, the compare sees no hazard and `as` adds
        # nothing, so the result is exactly one nop either way.
        got = _insns(["\tmtc1\t$1,$f2", "\tc.le.s\t$f2,$f12"])
        self.assertEqual(got, ["mtc1\t$1,$f2", "nop", "c.le.s\t$f2,$f12"])

    def test_separating_instruction_suppresses_the_nop(self):
        # retail ApplyFriction: the lwc1 already separates them.
        got = _insns(["\tmtc1\t$0,$f2", "\tl.s\t$f1,0($4)",
                      "\tc.lt.s\t$f2,$f1"])
        self.assertEqual(got, ["mtc1\t$0,$f2", "l.s\t$f1,0($4)",
                               "c.lt.s\t$f2,$f1"])

    def test_gcc_nop_marker_does_not_separate(self):
        # ee-gcc's `#nop` is a comment, not an instruction: it must neither be
        # materialized nor count as separation.
        got = _insns(["\tmtc1\t$1,$f0", "\t#nop", "\tc.le.s\t$f1,$f12"])
        self.assertEqual(got, ["mtc1\t$1,$f0", "nop", "c.le.s\t$f1,$f12"])

    def test_non_compare_fp_use_is_not_claimed(self):
        # Only the compare class is evidenced against retail; add.s/swc1 stay
        # untouched until a retail function proves them.
        got = _insns(["\tmtc1\t$1,$f0", "\tadd.s\t$f4,$f1,$f12"])
        self.assertEqual(got, ["mtc1\t$1,$f0", "add.s\t$f4,$f1,$f12"])

    def test_compare_after_unrelated_instruction_is_untouched(self):
        got = _insns(["\tlw\t$2,0($4)", "\tc.le.s\t$f1,$f12"])
        self.assertEqual(got, ["lw\t$2,0($4)", "c.le.s\t$f1,$f12"])

    def test_sonyize_rewrites_still_apply(self):
        # _sonyize_seg must remain a superset of _sonyize.
        got = _insns(["\tmove\t$2,$3", "\tj\t$31"])
        self.assertEqual(got, ["daddu\t$2,$3,$0", "jr\t$31"])


if __name__ == "__main__":
    unittest.main()
