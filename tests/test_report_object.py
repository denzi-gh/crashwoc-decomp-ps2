"""Unit tests for the objdiff report base object assembly.

_splice_unit is the pure line generator shared by the hybrid and the report
base. For the report base (report_base=True) with li.s-free segments it needs
no toolchain and no game files: `_rewrite_lis` returns untouched segments when
there is no float pool load, and un-decompiled functions are omitted entirely
rather than sliced from retail.
"""
import re
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from gen_hybrid import _splice_unit

PROLOGUE = [".text", ".globl foo"]
# foo: matching, bar: asm (no C), baz: equivalent. Only foo/baz have segments.
FUNCTIONS = [(0x1000, "foo", "matching"),
             (0x1080, "bar", "asm"),
             (0x1100, "baz", "equivalent")]
SEGMENTS = {
    "foo": [".ent foo", "\tmove\t$2,$3", "\tbreak\t7", ".end foo"],
    "baz": [".ent baz", "\tnop", ".end baz"],
}
UNIT_END = 0x1180


def splice_report():
    return _splice_unit(list(PROLOGUE), dict(SEGMENTS), list(FUNCTIONS),
                        "unit-0007", "equivalent", "pal103",
                        report_base=True, unit_end=UNIT_END)


class TestReportBaseSplice(unittest.TestCase):
    def setUp(self):
        self.text = "\n".join(splice_report())
        # Whitespace-insensitive view: the assembler output uses tabs.
        self.compact = re.sub(r"[ \t]+", " ", self.text)

    def test_c_functions_are_present(self):
        self.assertIn(".ent foo", self.text)
        self.assertIn(".ent baz", self.text)

    def test_sonyize_normalization_applied(self):
        # move/break are rewritten to the explicit forms the decompals `as`
        # and retail agree on.
        self.assertIn("daddu $2,$3,$0", self.compact)
        self.assertIn("break 0,7", self.compact)
        self.assertNotIn("move $2,$3", self.compact)

    def test_asm_function_is_omitted_entirely(self):
        # bar is un-decompiled: it must not appear as a symbol, must not pull
        # in a retail slice (no .set noreorder wrapper from _slice_lines), and
        # must not be zero-filled either -- it simply has no presence.
        self.assertNotIn("bar", self.text)
        self.assertNotIn(".set noreorder", self.text)

    def test_no_org_padding(self):
        # The report base diffs per symbol, so it never extent-pads; that also
        # avoids a backwards-.org failure when an equivalent function compiles
        # longer than its retail extent.
        self.assertNotIn(".org", self.text)

    def test_no_data_directives_emitted(self):
        for bad in (".lit4", ".lit8", ".data", ".rodata", ".sdata"):
            self.assertNotIn(bad, self.text)


if __name__ == "__main__":
    unittest.main()
