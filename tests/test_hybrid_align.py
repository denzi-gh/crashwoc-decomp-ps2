"""gen_hybrid alignment normalization for the decompals assembler.

The decompals `as` emits two spurious COP1-hazard nops when an alignment
directive follows an FP materialization (`li.s`/`li.d` macro or raw `mtc1`)
in reorder mode -- even at an already-aligned position -- which desyncs a
compiled function from its retail extent (first seen on game/crate
BreakCrate's hop-loop `.p2align 3` after the 0.5f lui+mtc1). gen_hybrid
counters this two ways: `_sonyize` normalizes `.p2align N` to `.align N`,
and `_fix_fpmacro_align` brackets an FP-materialization + align pair in
`.set noreorder` so the assembler inserts nothing. Pure text transforms --
no game files or toolchain needed.
"""
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
import gen_hybrid


class TestSonyizeP2Align(unittest.TestCase):
    def test_p2align_rewritten_to_align(self):
        self.assertEqual(gen_hybrid._sonyize("\t.p2align 3"), "\t.align 3")
        self.assertEqual(gen_hybrid._sonyize("\t.p2align 2"), "\t.align 2")

    def test_other_lines_untouched(self):
        for line in ("\t.align 3", "\tlb\t$2,48($4)", "$L54:"):
            self.assertEqual(gen_hybrid._sonyize(line), line)


class TestFixFpMacroAlign(unittest.TestCase):
    def test_lis_then_align_gets_bracketed(self):
        lines = [
            "\tli.s\t$f2,5.00000000000000000000e-1",
            "\t.align 3",
            "$L54:",
            "\tlb\t$2,48($4)",
        ]
        fixed = gen_hybrid._fix_fpmacro_align(lines)
        self.assertEqual(fixed, [
            "\t.set\tnoreorder",
            "\tli.s\t$f2,5.00000000000000000000e-1",
            "\t.align 3",
            "\t.set\treorder",
            "$L54:",
            "\tlb\t$2,48($4)",
        ])

    def test_labels_and_comments_between_are_skipped(self):
        lines = [
            "\tmtc1\t$1,$f2",
            "$Ltmp:",
            "\t#nop",
            "\t.align 3",
            "$L54:",
        ]
        fixed = gen_hybrid._fix_fpmacro_align(lines)
        self.assertEqual(fixed[0], "\t.set\tnoreorder")
        self.assertEqual(fixed[4], "\t.align 3")
        self.assertEqual(fixed[5], "\t.set\treorder")

    def test_align_after_plain_insn_untouched(self):
        lines = [
            "\tlb\t$2,48($4)",
            "\t.align 3",
            "$L54:",
        ]
        self.assertEqual(gen_hybrid._fix_fpmacro_align(lines), lines)

    def test_align_at_segment_start_untouched(self):
        lines = ["\t.align 3", "$L1:", "\tnop"]
        self.assertEqual(gen_hybrid._fix_fpmacro_align(lines), lines)

    def test_two_sites_both_bracketed(self):
        lines = [
            "\tli.s\t$f2,5.00000000000000000000e-1",
            "\t.align 3",
            "$L1:",
            "\tlb\t$2,48($4)",
            "\t.align 3",
            "$L2:",
            "\tli.s\t$f4,2.50000000000000000000e-1",
            "\t.align 2",
            "$L3:",
        ]
        fixed = gen_hybrid._fix_fpmacro_align(lines)
        self.assertEqual(fixed.count("\t.set\tnoreorder"), 2)
        self.assertEqual(fixed.count("\t.set\treorder"), 2)
        # the plain-insn site stays unbracketed
        i = fixed.index("\tlb\t$2,48($4)")
        self.assertEqual(fixed[i + 1], "\t.align 3")


if __name__ == "__main__":
    unittest.main()
