"""gen_hybrid's mtc1 hazard nop on compares (toolchain-free)."""
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from gen_hybrid import _sonyize_seg


def _insns(seg, profile="default"):
    """Emitted lines minus comments/blanks, stripped -- what the assembler sees."""
    return [l.strip() for l in _sonyize_seg(seg, profile)
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

    def test_non_compare_reader_of_the_pair_gets_a_nop(self):
        # The rule is per register pair, not per instruction class: add.s
        # reading $f1 (the mate of $f0) is padded like a compare.
        got = _insns(["\tmtc1\t$1,$f0", "\tadd.s\t$f4,$f1,$f12"])
        self.assertEqual(got, ["mtc1\t$1,$f0", "nop", "add.s\t$f4,$f1,$f12"])

    def test_lis_before_aligned_loop_head_is_not_reordered(self):
        # retail DrawZoffaUFOs: li.s $f21 then `.p2align 3` + the loop label.
        # No hazard (the loop's first insn reads no FPR), but the alignment
        # hides that from the decompals `as`, so the li.s and the alignment
        # are assembled without reordering and no nop is written.
        got = _insns(["\tli.s\t$f21,1.8e2", "\t.p2align 3", "$L295:",
                      "\tlw\t$2,224($16)"])
        self.assertEqual(got, [".set\tnoreorder", "li.s\t$f21,1.8e2",
                               ".p2align 3", "$L295:", ".set\treorder",
                               "lw\t$2,224($16)"])

    def test_lis_before_aligned_loop_head_is_left_alone_for_sce(self):
        # The sce retail assembler behaves like GNU `as`, so the decompals
        # `as` padding is already right there.
        seg = ["\tli.s\t$f21,1.8e2", "\t.p2align 3", "$L295:",
               "\tlw\t$2,224($16)"]
        self.assertEqual(_insns(seg, "sce"), [l.strip() for l in seg])

    def test_lis_before_aligned_end_of_segment_is_left_alone(self):
        seg = ["\tli.s\t$f21,1.8e2", "\t.p2align 3", "$L295:"]
        self.assertEqual(_insns(seg), [l.strip() for l in seg])

    def test_lis_hazard_across_aligned_loop_head_pads_after_alignment(self):
        # retail NuLightInit: mtc1 $zero,$f0 / pad / nop / loop head swc1 $f0
        got = _insns(["\tli.s\t$f0,1.0", "\t.p2align 3", "$Lloop:",
                      "\tadd.s\t$f2,$f0,$f1"])
        self.assertEqual(got, [".set\tnoreorder", "li.s\t$f0,1.0",
                               ".p2align 3", "nop", "$Lloop:",
                               ".set\treorder", "add.s\t$f2,$f0,$f1"])

    def test_inline_lis_hazard_padding_precedes_join_label(self):
        # Incoming branches must skip the nop needed by the fall-through load.
        got = _insns(["\tli.s\t$f0,1.0", "$Ljoin:",
                      "\tc.lt.s\t$f0,$f15"])
        self.assertEqual(got, ["li.s\t$f0,1.0", "nop", "$Ljoin:",
                               "c.lt.s\t$f0,$f15"])

    def test_literal_mtc1_hazard_crosses_comments_and_multiple_labels(self):
        got = _insns(["\tmtc1\t$0,$f0", "\t#nop", "$Lfirst:",
                      "", "$Ljoin:", "\tc.lt.s\t$f13,$f0"])
        self.assertEqual(got, ["mtc1\t$0,$f0", "nop", "$Lfirst:",
                               "$Ljoin:", "c.lt.s\t$f13,$f0"])

    def test_explicit_padding_before_join_label_is_not_duplicated(self):
        seg = ["\tli.s\t$f0,1.0", "\tnop", "$Ljoin:",
               "\tc.lt.s\t$f0,$f15"]
        self.assertEqual(_insns(seg), [l.strip() for l in seg])
        self.assertEqual(_sonyize_seg(_sonyize_seg(seg)), _sonyize_seg(seg))

    def test_instruction_after_join_label_separates_the_hazard(self):
        seg = ["\tli.s\t$f0,1.0", "$Ljoin:", "\tlw\t$2,0($4)",
               "\tc.lt.s\t$f0,$f15"]
        self.assertEqual(_insns(seg), [l.strip() for l in seg])

    def test_non_compare_after_join_label_is_padded_before_it(self):
        # retail CrateOff 0x1F3600: lui/mtc1 $at,$f1 / nop / LABEL: add.s
        seg = ["\tli.s\t$f0,1.0", "$Ljoin:", "\tadd.s\t$f4,$f0,$f12"]
        self.assertEqual(_insns(seg), ["li.s\t$f0,1.0", "nop", "$Ljoin:",
                                       "add.s\t$f4,$f0,$f12"])

    def test_compare_after_unrelated_instruction_is_untouched(self):
        got = _insns(["\tlw\t$2,0($4)", "\tc.le.s\t$f1,$f12"])
        self.assertEqual(got, ["lw\t$2,0($4)", "c.le.s\t$f1,$f12"])

    def test_sonyize_rewrites_still_apply(self):
        # _sonyize_seg must remain a superset of _sonyize. (The trailing jump
        # also picks up its delay-slot nop -- see tests/test_delay_slot.py.)
        got = _insns(["\tmove\t$2,$3", "\tj\t$31"])
        self.assertEqual(got, ["daddu\t$2,$3,$0", ".set\tnoreorder",
                               "jr\t$31", "nop", ".set\treorder"])


if __name__ == "__main__":
    unittest.main()
