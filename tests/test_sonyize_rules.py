"""gen_hybrid's SN-as normalization rules on synthetic snippets."""
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from gen_hybrid import _fpr_reads, _sonyize, _sonyize_seg, parse_s


def _insns(seg, profile="default"):
    """Emitted lines minus comments/blanks, stripped -- what the assembler sees."""
    return [l.strip() for l in _sonyize_seg(seg, profile)
            if l.strip() and not l.strip().startswith("#")]


class TestMtc1PairRule(unittest.TestCase):

    def test_pair_mate_store_gets_a_nop(self):
        # retail NuMtxSetTranslation 0x10949C: mtc1 $zero,$f1 / nop / swc1 $f0
        got = _insns(["\tmtc1\t$0,$f1", "\ts.s\t$f0,48($4)"])
        self.assertEqual(got, ["mtc1\t$0,$f1", "nop", "s.s\t$f0,48($4)"])

    def test_pair_mate_arith_after_inline_lis_gets_a_nop(self):
        # retail GetTopBot 0x1FAA64: lui/mtc1 $at,$f1 / nop / add.s $f0,$f0,$f2
        got = _insns(["\tli.s\t$f1,5.0e-1", "\tadd.s\t$f0,$f0,$f2"])
        self.assertEqual(got, ["li.s\t$f1,5.0e-1", "nop",
                               "add.s\t$f0,$f0,$f2"])

    def test_other_pair_reader_gets_no_nop(self):
        # retail GetTopBot 0x1FAB00: lui/mtc1 $at,$f2 / add.s $f0,$f0,$f1
        seg = ["\tli.s\t$f2,5.0e-1", "\tadd.s\t$f0,$f0,$f1"]
        self.assertEqual(_insns(seg), [l.strip() for l in seg])

    def test_independent_compare_gets_no_nop(self):
        # retail NuBridgeUpdate 0x1B31B0: mtc1 $at,$f0 / c.eq.s $f20,$f23 --
        # disproves the older "any adjacent compare" rule.
        seg = ["\tli.s\t$f0,1.0", "\tc.eq.s\t$f20,$f23"]
        self.assertEqual(_insns(seg), [l.strip() for l in seg])

    def test_pair_write_only_gets_no_nop(self):
        # A follower that only writes the pair is not a hazard (0 / 165).
        for follower in ("\tl.s\t$f0,0($4)", "\tmtc1\t$3,$f0",
                         "\tadd.s\t$f0,$f2,$f3"):
            seg = ["\tmtc1\t$2,$f1", follower]
            self.assertEqual(_insns(seg), [l.strip() for l in seg], follower)

    def test_exact_dependency_gets_exactly_one_nop(self):
        got = _insns(["\tmtc1\t$2,$f0", "\tcvt.s.w\t$f0,$f0"])
        self.assertEqual(got, ["mtc1\t$2,$f0", "nop", "cvt.s.w\t$f0,$f0"])

    def test_integer_follower_gets_no_nop(self):
        seg = ["\tmtc1\t$2,$f0", "\taddu\t$3,$4,$5"]
        self.assertEqual(_insns(seg), [l.strip() for l in seg])

    def test_nop_goes_before_label_and_comments(self):
        got = _insns(["\tmtc1\t$2,$f3", "\t#nop", "$L5:",
                      "\tmul.s\t$f0,$f14,$f2"])
        self.assertEqual(got, ["mtc1\t$2,$f3", "nop", "$L5:",
                               "mul.s\t$f0,$f14,$f2"])

    def test_gcc_noreorder_block_is_not_looked_into(self):
        # retail ScaleFlatShadow 0x2012E4: mtc1 $at,$f0 / b / mul.s ..,$f0 --
        # the reader sits in gcc's own delay-slot block, no nop.
        seg = ["\tli.s\t$f0,5.0e-1", "\t.set\tnoreorder", "\t.set\tnomacro",
               "\tb\t$L435", "\tmul.s\t$f0,$f14,$f0", "\t.set\tmacro",
               "\t.set\treorder"]
        self.assertEqual(_insns(seg), [l.strip() for l in seg])

    def test_rule_is_idempotent(self):
        seg = ["\tmtc1\t$0,$f1", "$L1:", "\ts.s\t$f0,0($4)"]
        once = _sonyize_seg(seg)
        self.assertEqual(_sonyize_seg(once), once)

    def test_fpr_read_sets(self):
        self.assertEqual(_fpr_reads("\ts.s\t$f3,4($4)"), {3})
        self.assertEqual(_fpr_reads("\tl.s\t$f3,4($4)"), set())
        self.assertEqual(_fpr_reads("\tc.lt.s\t$f1,$f2"), {1, 2})
        self.assertEqual(_fpr_reads("\tmula.s\t$f1,$f2"), {1, 2})
        self.assertEqual(_fpr_reads("\tmadd.s\t$f0,$f1,$f2"), {1, 2})
        self.assertEqual(_fpr_reads("\tmov.s\t$f2,$f0"), {0})
        self.assertEqual(_fpr_reads("\tmfc1\t$2,$f5"), {5})
        self.assertEqual(_fpr_reads("\tmtc1\t$2,$f5"), set())
        self.assertEqual(_fpr_reads("\taddu\t$2,$3,$4"), set())
        self.assertEqual(_fpr_reads("$L3:"), set())


class TestTruncWS(unittest.TestCase):

    def test_cvt_w_s_becomes_trunc_w_s(self):
        self.assertEqual(_sonyize("\tcvt.w.s\t$f6,$f1"), "\ttrunc.w.s\t$f6,$f1")

    def test_no_raw_word_after_mfc1(self):
        # retail GenericDebinfoDmaTypeUpdate: mfc1 $8,$f1 / cvt.w.s $f1,$f7,
        # no nop; a `.word` here would make the decompals `as` pad one.
        got = _insns(["\tmfc1\t$8,$f1", "\tcvt.w.s\t$f1,$f7"])
        self.assertEqual(got, ["mfc1\t$8,$f1", "trunc.w.s\t$f1,$f7"])
        self.assertFalse(any(l.startswith(".word") for l in got))

    def test_other_cvt_forms_are_untouched(self):
        self.assertEqual(_sonyize("\tcvt.s.w\t$f0,$f0"), "\tcvt.s.w\t$f0,$f0")


class TestSceDelaySlot(unittest.TestCase):

    def test_default_profile_pads_unfilled_slot(self):
        got = _insns(["\tsw\t$2,0($3)", "\tj\t$31"])
        self.assertEqual(got, ["sw\t$2,0($3)", ".set\tnoreorder", "jr\t$31",
                               "nop", ".set\treorder"])

    def test_sce_profile_leaves_slot_to_the_assembler(self):
        # retail sceDevVif0Reset: addiu / jr $ra / sw -- swapped.
        got = _insns(["\tsw\t$2,0($3)", "\tj\t$31"], profile="sce")
        self.assertEqual(got, ["sw\t$2,0($3)", "jr\t$31"])

    def test_sce_profile_keeps_the_hazard_rule(self):
        got = _insns(["\tmtc1\t$2,$f1", "\ts.s\t$f0,0($4)"], profile="sce")
        self.assertEqual(got, ["mtc1\t$2,$f1", "nop", "s.s\t$f0,0($4)"])


class TestParseFile(unittest.TestCase):

    def test_skeleton_unit_without_functions_parses(self):
        text = ('\t.file\t1 "/tmp/x/glass.c"\ngcc2_compiled.:\n'
                '__gnu_compiled_c:\n')
        prologue, segments, _local = parse_s(text)
        self.assertEqual(segments, [])
        self.assertIn("gcc2_compiled.:", prologue)

    def test_file_marker_between_and_after_functions(self):
        text = "\n".join([
            '\t.file\t1 "a.c"', "gcc2_compiled.:", "\t.text",
            "\t.align\t2", "\t.globl\tf", "\t.ent\tf", "f:", "\tj\t$31",
            "\t.end\tf",
            '\t.file\t2 "inl.h"', "\t.align\t2", "\t.globl\tg", "\t.ent\tg",
            "g:", "\tj\t$31", "\t.end\tg",
            '\t.file\t1 "a.c"', ""])
        _prologue, segments, _local = parse_s(text)
        self.assertEqual([n for n, _s in segments], ["f", "g"])
        self.assertTrue(any("inl.h" in l for l in segments[1][1]))


if __name__ == "__main__":
    unittest.main()
