"""Unit tests for gen_hybrid's unfilled-branch delay-slot nop (`_sonyize_seg`).

Toolchain-free: these pin the *rule*, not the bytes -- the byte gates
(verify-promoted / verify-loaded, which need orig/) carry the proof that the
rule reproduces retail.

The rule, from retail nups2/ps2video:
  NuPs2GetRenderWidth @0x16A8E8   lwc1 / cvt.s.w / jr $31 / nop
  timeUserRead        @0x16B2C0   lui / lw / jr $31 / nop
ee-gcc emits both tails as `<insn>` + a bare `j $31` with no `.set noreorder`
around it, leaving the delay slot for the assembler. Sony's `as` always fills
it with a nop; the decompals `as` in `.set reorder` mode swaps the preceding
instruction into it instead, which loses a word and shifts every later
function. Branches gcc itself filled sit inside its own `.set noreorder`
block and must be passed through untouched.
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


class TestDelaySlotNop(unittest.TestCase):

    def test_bare_epilogue_jump_gets_a_nop(self):
        # retail NuPs2GetRenderWidth: the cvt must NOT sink into the slot.
        got = _insns(["\tcvt.s.w\t$f0,$f0", "\tj\t$31"])
        self.assertEqual(got, ["cvt.s.w\t$f0,$f0", ".set\tnoreorder",
                               "jr\t$31", "nop", ".set\treorder"])

    def test_gcc_filled_slot_is_left_alone(self):
        # gcc wraps every slot it fills itself; that block is retail's own
        # choice and must pass through with no extra nop.
        seg = ["\t.set\tnoreorder", "\t.set\tnomacro", "\tbeq\t$2,$0,$L4",
               "\tlui\t$2,%hi(D_002EA300)", "\t.set\tmacro", "\t.set\treorder"]
        self.assertEqual(_insns(seg), [l.strip() for l in seg])

    def test_reorder_resumes_after_a_gcc_block(self):
        seg = ["\t.set\tnoreorder", "\t.set\tnomacro", "\tbeq\t$2,$0,$L4",
               "\tnop", "\t.set\tmacro", "\t.set\treorder",
               "\tsw\t$0,0($3)", "\tj\t$31"]
        self.assertEqual(_insns(seg), [
            ".set\tnoreorder", ".set\tnomacro", "beq\t$2,$0,$L4", "nop",
            ".set\tmacro", ".set\treorder", "sw\t$0,0($3)",
            ".set\tnoreorder", "jr\t$31", "nop", ".set\treorder"])

    def test_conditional_branch_in_reorder_mode_gets_a_nop(self):
        got = _insns(["\tlw\t$2,0($4)", "\tbeq\t$2,$0,$L4"])
        self.assertEqual(got, ["lw\t$2,0($4)", ".set\tnoreorder",
                               "beq\t$2,$0,$L4", "nop", ".set\treorder"])

    def test_non_branch_mnemonics_are_not_claimed(self):
        # `jalr`-shaped names only: nothing that merely starts with a branch
        # mnemonic may be wrapped.
        for line in ("\tbreak\t7", "\tbeqz.x\t$2,$L1", "\tjr.hb\t$31"):
            self.assertNotIn(".set\tnoreorder",
                             [l.strip() for l in _sonyize_seg([line])])

    def test_label_lines_are_untouched(self):
        got = _insns(["$L4:", "\tj\t$31"])
        self.assertEqual(got, ["$L4:", ".set\tnoreorder", "jr\t$31", "nop",
                               ".set\treorder"])

    def test_mtc1_hazard_and_delay_slot_compose(self):
        got = _insns(["\tmtc1\t$1,$f0", "\tc.le.s\t$f1,$f12", "\tj\t$31"])
        self.assertEqual(got, ["mtc1\t$1,$f0", "nop", "c.le.s\t$f1,$f12",
                               ".set\tnoreorder", "jr\t$31", "nop",
                               ".set\treorder"])


if __name__ == "__main__":
    unittest.main()
