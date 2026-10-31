"""gen_hybrid's SN-as normalization rules on synthetic snippets."""
import re
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from gen_hybrid import _fpr_reads, _gpr_reads, _sonyize, _sonyize_seg, parse_s


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


class TestMfc1AcrossAlignment(unittest.TestCase):

    def test_reader_after_aligned_loop_head_pads_after_the_alignment(self):
        # retail GliderSmoke 0x00204014: mfc1 $a1,$f1 / nop / loop head at
        # 0x0020401C -- aligned first, then the nop, then the label.
        got = _insns(["\tmfc1\t$5,$f1", "\t.p2align 3", "$L3029:",
                      "\taddu\t$3,$5,-16"])
        self.assertEqual(got, [".set\tnoreorder", "mfc1\t$5,$f1",
                               ".p2align 3", "nop", "$L3029:",
                               ".set\treorder", "addu\t$3,$5,-16"])

    def test_non_reader_after_aligned_loop_head_gets_no_nop(self):
        got = _insns(["\tmfc1\t$5,$f1", "\t.p2align 3", "$L1:",
                      "\taddu\t$3,$4,-16"])
        self.assertEqual(got, [".set\tnoreorder", "mfc1\t$5,$f1",
                               ".p2align 3", "$L1:", ".set\treorder",
                               "addu\t$3,$4,-16"])

    def test_without_alignment_the_assembler_pads_itself(self):
        seg = ["\tmfc1\t$5,$f1", "$L1:", "\taddu\t$3,$5,-16"]
        self.assertEqual(_insns(seg), [l.strip() for l in seg])

    def test_sce_profile_is_left_alone(self):
        seg = ["\tmfc1\t$5,$f1", "\t.p2align 3", "$L1:", "\taddu\t$3,$5,-16"]
        self.assertEqual(_insns(seg, "sce"), [l.strip() for l in seg])

    def test_store_reads_its_first_operand(self):
        self.assertEqual(_gpr_reads("\tsw\t$5,0($4)"), {"$5", "$4"})
        self.assertEqual(_gpr_reads("\tlw\t$5,0($4)"), {"$4"})
        self.assertEqual(_gpr_reads("\tbne\t$5,$0,$L3"), {"$5", "$0"})


_FILLED_JUMP = ["\t.set\tnoreorder", "\t.set\tnomacro", "\tjal\tcmd_sem_init",
                "\tsd\t$18,32($sp)", "\t.set\tmacro", "\t.set\treorder"]


def _labelled(lines, branch):
    """Is `branch` directly preceded by a slot-keeping label?"""
    i = lines.index(branch)
    return i > 0 and re.fullmatch(r"\$Lsony\d+:", lines[i - 1]) is not None


class TestSceDelaySlots(unittest.TestCase):

    def test_branch_after_a_filled_jump_slot_keeps_its_slot(self):
        # retail _sceCd_ncmd_prechk 0x00266260: lw $a0 / jal PollSema / nop
        got = _insns(_FILLED_JUMP + ["\tlw\t$4,%lo(sem)($16)",
                                     "\tjal\tPollSema"], "sce")
        self.assertTrue(_labelled(got, "jal\tPollSema"))

    def test_alignment_and_labels_do_not_reset_the_history(self):
        # retail _sceCd_cd_callback: `b; ld` block, .p2align, label, lw, jal
        got = _insns(_FILLED_JUMP + ["\t.p2align 2", "$L7:",
                                     "\tlw\t$4,%lo(sem)($2)",
                                     "\tjal\tiSignalSema"], "sce")
        self.assertTrue(_labelled(got, "jal\tiSignalSema"))

    def test_branch_after_two_reordered_insns_may_swap(self):
        # retail cdvd_exit: lui / lw / jal SignalSema swaps the lw in
        seg = _FILLED_JUMP + ["\tlui\t$16,%hi(sem)", "\tlw\t$4,%lo(sem)($16)",
                              "\tjal\tSignalSema"]
        self.assertEqual(_insns(seg, "sce"), [l.strip() for l in seg])

    def test_mflo_is_never_swapped_into_a_slot(self):
        # retail _calloc_r 0x00278620: mflo $a1 / jal _malloc_r / nop
        got = _insns(["\tmult\t$5,$6", "\tsd\t$31,16($sp)", "\tmflo\t$5",
                      "\tjal\t_malloc_r"], "sce")
        self.assertTrue(_labelled(got, "jal\t_malloc_r"))

    def test_branchless_noreorder_block_is_not_a_filled_slot(self):
        # _rewrite_lis wraps a li.d expansion in noreorder; that is not one
        # of ee-gcc's filled slots.
        seg = ["\t.set\tnoreorder", "\tdli\t$5,0x4024000000000000",
               "\t.set\treorder", "\taddu\t$4,$16,8", "\tjal\tdpmul"]
        self.assertEqual(_insns(seg, "sce"), [l.strip() for l in seg])

    def test_cfc2_and_its_reader_are_not_reordered(self):
        # retail sceDevVu0CheckBusy 0x00263528: cfc2 $v0,$vi29 / andi $v0
        got = _insns(["\tcfc2 $2, $vi29", "\t#NO_APP", "\tandi\t$2,$2,0xf"],
                     "sce")
        self.assertEqual(got, [".set\tnoreorder", "cfc2 $2, $vi29",
                               "andi\t$2,$2,0xf", ".set\treorder"])

    def test_mfc0_and_its_reader_are_not_reordered(self):
        # retail _sceSDC 0x00279464: mfc0 $v0,$28 / and $v0,$v0,$a3
        got = _insns(["\tmfc0 $2, $28", "\tand\t$2,$2,$7", "\taddu\t$2,$2,$6"],
                     "sce")
        self.assertEqual(got, [".set\tnoreorder", "mfc0 $2, $28",
                               "and\t$2,$2,$7", ".set\treorder",
                               "addu\t$2,$2,$6"])

    def test_cfc2_before_an_independent_insn_is_left_alone(self):
        seg = ["\tcfc2 $3, $vi29", "\taddu\t$4,$0,1"]
        self.assertEqual(_insns(seg, "sce"), [l.strip() for l in seg])

    def test_ctc2_before_a_cop2_op_is_not_reordered(self):
        # retail sceVu0MemWriteQ 0x00263990: ctc2 $a0,$vi1 / vsqi
        got = _insns(["\tctc2 $4, $vi1", "\tvsqi.xyzw $vf1, ($vi1++)",
                      "\taddu\t$4,$4,1"], "sce")
        self.assertEqual(got[:4], [".set\tnoreorder", "ctc2 $4, $vi1",
                                   "vsqi.xyzw $vf1, ($vi1++)", ".set\treorder"])

    def test_cop2_op_before_a_branch_fills_its_slot(self):
        # retail sceDevVu0Exec 0x002636CC: ctc2 / jr $ra / vcallmsr
        got = _insns(["\tctc2 $4, $vi27", "\tvcallmsr $vi27", "\tj\t$31"],
                     "sce")
        self.assertEqual(got, [".set\tnoreorder", "ctc2 $4, $vi27",
                               "jr\t$31", "vcallmsr $vi27", ".set\treorder"])

    def test_default_profile_still_pads_every_reorder_branch(self):
        got = _insns(_FILLED_JUMP + ["\tlw\t$4,%lo(sem)($16)",
                                     "\tjal\tPollSema"])
        self.assertEqual(got[-4:], [".set\tnoreorder", "jal\tPollSema", "nop",
                                    ".set\treorder"])


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


def _fake_listing(lines, _g_flag):
    """One word per instruction line, laid out back to back."""
    words, addr = {}, 0
    for index, line in enumerate(lines):
        if re.match(r"^\s+[a-z]", line):
            words[index] = [addr, 1]
            addr += 4
    return words


class TestSceShortLoopPads(unittest.TestCase):
    # Probed on the 2.9 `as` (profile sce); retail _strtod_r 0x00272934.

    A = "\taddu\t$2,$2,$3"
    BNE = ["\t.set\tnoreorder", "\tbne\t$2,$0,$L1", "\tnop", "\t.set\treorder"]

    def pads(self, lines):
        from unittest import mock
        import gen_hybrid
        with mock.patch.object(gen_hybrid, "_listing_words", _fake_listing):
            out = gen_hybrid._sce_short_loop_pads(lines, "-G0")
        branch = next(i for i, l in enumerate(out) if "bne" in l)
        n = 0
        while out[branch - 1 - n].strip() == "nop":
            n += 1
        return n, out

    def test_label_in_big_frag_counts_to_frag_end(self):
        # six words before the label, then label / addu / %hi lui: the
        # assembler sees a two-word loop however far the branch is.
        n, _out = self.pads([self.A] * 6 + ["$L1:", self.A,
                            "\tlui\t$3,%hi(sym)"] + [self.A] * 8 + self.BNE)
        self.assertEqual(n, 3)

    def test_label_in_small_frag_uses_true_distance(self):
        n, _out = self.pads([self.A] * 2 + ["$L1:", self.A, "\t.p2align\t2"]
                            + [self.A] * 8 + self.BNE)
        self.assertEqual(n, 0)

    def test_true_short_loop_in_one_frag(self):
        n, _out = self.pads(["\t.p2align\t3", "$L1:"] + [self.A] * 3
                            + self.BNE)
        self.assertEqual(n, 2)

    def test_jump_in_counted_words_blocks_padding(self):
        # retail sceCdSync: `jal sceCdDelayThread` inside a 3-word loop.
        n, _out = self.pads(["\t.p2align\t3", "$L1:", "\t.set\tnoreorder",
                             "\tjal\tg", "\tnop", "\t.set\treorder", self.A]
                            + self.BNE)
        self.assertEqual(n, 0)

    def test_jump_past_the_frag_end_does_not_block(self):
        n, _out = self.pads([self.A] * 6 + ["$L1:", self.A, "\t.p2align\t2",
                             "\t.set\tnoreorder", "\tjal\tg", "\tnop",
                             "\t.set\treorder", self.A] + self.BNE)
        self.assertEqual(n, 4)

    def test_retail_slice_label_is_left_alone(self):
        lines = [self.A] * 6 + [".L1:", self.A, "\t.p2align\t2", self.A,
                                "\tbnez\t$2,.L1", "\tnop"]
        from unittest import mock
        import gen_hybrid
        with mock.patch.object(gen_hybrid, "_listing_words", _fake_listing):
            self.assertEqual(gen_hybrid._sce_short_loop_pads(lines, "-G0"),
                             lines)

    def test_reorder_branch_pads_in_a_noreorder_block(self):
        _n, out = self.pads([self.A] * 6 + ["$L1:", self.A, "\t.p2align\t2",
                             self.A, "\tbne\t$2,$0,$L1"])
        branch = out.index("\tbne\t$2,$0,$L1")
        self.assertEqual(out[branch - 6:branch],
                         ["\t.set\tnoreorder"] + ["\tnop"] * 4
                         + ["\t.set\treorder"])


if __name__ == "__main__":
    unittest.main()
