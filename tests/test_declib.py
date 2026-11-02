"""Unit tests for tools/declib on small synthetic inputs.

Everything here is hand-built fixture data -- no game-derived bytes, no
toolchain -- so the suite runs on any host and in public CI.
"""
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from declib.asmtext import AUTO_NAME_RE, disambiguate, resolve
from declib.toolchain import Reporter, h
from declib.tu import (glabel_addresses, parse_toml_blocks, prologue_end,
                       split_body)
from declib import tu


def instr(vram, word, text):
    """A splat-style instruction line: `/* fileoff vram word */ text`.

    The word hex in the comment is the four instruction bytes in file
    (little-endian) order, exactly as splat emits them.
    """
    fileoff = vram - 0x000FF000
    return (f"/* {fileoff:X} {vram:08X} {word.to_bytes(4, 'little').hex().upper()} */"
            f"  {text}")


def jal_word(target):
    return 0x0C000000 | (target >> 2)


class TestHelpers(unittest.TestCase):
    def test_h(self):
        self.assertEqual(h("0x10"), 16)
        self.assertEqual(h(16), 16)

    def test_auto_name_re(self):
        self.assertEqual(AUTO_NAME_RE.fullmatch("D_00633400").group(1), "00633400")
        self.assertEqual(AUTO_NAME_RE.fullmatch("func_001147A8").group(1), "001147A8")
        self.assertIsNone(AUTO_NAME_RE.fullmatch("NuListGetHead"))

    def test_resolve(self):
        defsyms, unresolved = resolve(
            ["D_00633400", "NuKnown", "NuUnknown"], {"NuKnown": 0x1000})
        self.assertEqual(defsyms, {"D_00633400": 0x633400, "NuKnown": 0x1000})
        self.assertEqual(unresolved, ["NuUnknown"])

    def test_parse_toml_blocks(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / "x.toml"
            path.write_text("header = 1\n\n[[unit]]\nindex = 7\n"
                            "name = 'a\\b.c'\n\n[[unit]]\nindex = 8\n")
            rows = list(parse_toml_blocks(
                path, {"index": r"index = (\d+)", "name": r"name = '([^']*)'"}))
        self.assertEqual(rows, [{"index": "7", "name": "a\\b.c"}, {"index": "8"}])


class TestDisambiguate(unittest.TestCase):
    def lines(self):
        return [
            ".include \"macro.inc\"",
            "",
            "glabel dup",
            instr(0x00100000, 0, "nop"),
            "endlabel dup",
            "glabel caller1",
            instr(0x00100008, jal_word(0x00100000), "jal   dup"),
            "endlabel caller1",
            "glabel dup",
            instr(0x00100010, 0, "nop"),
            "endlabel dup",
            "glabel caller2",
            instr(0x00100018, jal_word(0x00100010), "jal   dup"),
            "endlabel caller2",
        ]

    def test_renames_and_repoints(self):
        text, ok = disambiguate("\n".join(self.lines()) + "\n", Reporter())
        self.assertTrue(ok)
        out = text.splitlines()
        self.assertIn("glabel dup__00100000", out)
        self.assertIn("glabel dup__00100010", out)
        self.assertNotIn("glabel dup", out)
        self.assertTrue(any("jal   dup__00100000" in l for l in out))
        self.assertTrue(any("jal   dup__00100010" in l for l in out))
        # Byte-preserving: instruction comments (the encoded words) unchanged.
        self.assertEqual([l.split("*/")[0] for l in out if l.startswith("/*")],
                         [l.split("*/")[0] for l in self.lines()
                          if l.startswith("/*")])

    def test_rejects_non_jal_reference(self):
        bad = self.lines() + [".word dup"]
        reporter = Reporter()
        _text, ok = disambiguate("\n".join(bad) + "\n", reporter)
        self.assertFalse(ok)
        self.assertTrue(reporter.failed)

    def test_no_duplicates_is_identity(self):
        lines = ["glabel one", instr(0x00100000, 0, "nop"), "endlabel one"]
        text = "\n".join(lines) + "\n"
        out, ok = disambiguate(text, Reporter())
        self.assertTrue(ok)
        self.assertEqual(out, text)


class TestAlignmentAndSplit(unittest.TestCase):
    def test_drop_alignment_noop(self):
        lines = [
            instr(0x00100000, 0, "nop"),
            ".align 3",
            instr(0x00100004, 0, "nop"),
        ]
        kept = tu.drop_alignment(lines, Reporter())
        self.assertEqual(kept, [lines[0], lines[2]])

    def test_drop_alignment_detects_gap(self):
        lines = [
            instr(0x00100000, 0, "nop"),
            ".align 3",
            instr(0x00100010, 0, "nop"),   # 12-byte gap: .align was load-bearing
        ]
        reporter = Reporter()
        self.assertIsNone(tu.drop_alignment(lines, reporter))
        self.assertTrue(reporter.failed)

    def body(self):
        return [
            ".include \"macro.inc\"",     # prologue
            "nonmatching funcA, 0x8",
            "glabel funcA",
            instr(0x00100000, 0, "nop"),
            instr(0x00100004, 0, "nop"),
            "endlabel funcA",
            "nonmatching funcB, 0x4",     # lead lines belong to funcB's block
            "glabel funcB",
            instr(0x00100008, 0, "nop"),
            "endlabel funcB",
        ]

    def test_prologue_and_glabels(self):
        lines = self.body()
        self.assertEqual(prologue_end(lines), 1)
        self.assertEqual(glabel_addresses(lines), {2: 0x00100000, 7: 0x00100008})

    def test_split_body_sweeps_lead_lines(self):
        lines = self.body()
        body_start = prologue_end(lines)
        chunks = split_body(lines, body_start, [0x00100008])
        self.assertEqual(len(chunks), 2)
        # The cut lands above funcB's `nonmatching` lead, not at its glabel.
        self.assertEqual(chunks[0], lines[1:6])
        self.assertEqual(chunks[1], lines[6:])
        # Lossless partition.
        self.assertEqual(chunks[0] + chunks[1], lines[body_start:])


class TestTuRuns(unittest.TestCase):
    def test_included_code_starts_its_unit(self):
        # nu2api.i's stubs (0x0016ECA4) open asmdma.s, not ptl.c.
        runs = dict(tu.load_tu_runs())
        self.assertEqual(runs[65], 0x0016ECA4)
        self.assertEqual(runs[64], 0x0016E890)

    AUTO = ["glabel sceSifWriteBackDCache",
            "    /* 17C2B8 0027B2B8 C0FF193C */  lui        $t9, 0xFFFF",
            "endlabel sceSifWriteBackDCache", "",
            "nonmatching func_0027B364, 0x4", "",
            "glabel func_0027B364",
            "    /* 17C364 0027B364 0800E003 */  jr         $ra",
            "endlabel func_0027B364"]

    def test_unreferenced_auto_function_folds_into_its_owner(self):
        out = tu.fold_auto_functions(self.AUTO)
        self.assertEqual([l for l in out if l.strip()], [
            self.AUTO[0], self.AUTO[1], self.AUTO[7],
            "endlabel sceSifWriteBackDCache"])

    def test_referenced_auto_function_stays(self):
        lines = self.AUTO + ["    /* 0 0 0 */  lui $t1, %hi(func_0027B364)"]
        self.assertEqual(tu.fold_auto_functions(lines), lines)

    # asmdma's vpDmaTag_Close jump targets: a chain of auto-functions behind
    # nop padding and `/* Handwritten function */` leads, the first one
    # referenced from the same TU.
    CC = "    /* 6FE60 0016EE60 0800A8DC */  ld         $t0, 0x8($a1)"
    T1 = "    /* 6FE78 0016EE78 00008220 */  addi       $v0, $a0, 0x0"
    PAD = "    /* 6FE94 0016EE94 00000000 */  nop"
    T2 = "    /* 6FEB8 0016EEB8 000080AC */  sw         $zero, 0x0($a0)"
    CL = "    /* 6FF58 0016EF58 6300083C */  lui        $t0, %hi(vpdmatag_curr)"
    REF = "    /* 6FF60 0016EF60 1700093C */  lui        $t1, %hi({})"
    CHAIN = ["glabel vpDmaTag_ChangeCopy", CC,
             "endlabel vpDmaTag_ChangeCopy", "",
             "/* Handwritten function */",
             "nonmatching func_0016EE78, 0x1C", "",
             "glabel func_0016EE78", T1,
             "endlabel func_0016EE78", PAD, "",
             "/* Handwritten function */",
             "nonmatching func_0016EEB8, 0x24", "",
             "glabel func_0016EEB8", T2,
             "endlabel func_0016EEB8", "",
             "/* Handwritten function */",
             "nonmatching vpDmaTag_Close, 0x20", "",
             "glabel vpDmaTag_Close", CL, REF.format("func_0016EE78"),
             "endlabel vpDmaTag_Close"]

    def test_auto_function_chain_folds_behind_padding_and_leads(self):
        out = [l for l in tu.fold_auto_functions(self.CHAIN)
               if l.strip() and not l.startswith("/*")]
        self.assertEqual(out, [
            "glabel vpDmaTag_ChangeCopy", self.CC,
            ".Lfunc_0016EE78:", self.T1, self.PAD, self.T2,
            "endlabel vpDmaTag_ChangeCopy",
            "nonmatching vpDmaTag_Close, 0x20",
            "glabel vpDmaTag_Close", self.CL, self.REF.format(".Lfunc_0016EE78"),
            "endlabel vpDmaTag_Close"])

    def test_auto_function_referenced_from_another_tu_stays(self):
        # 0x0027B2B8 is sifcmd's: a `.L` label in asmdma cannot reach it.
        lines = self.CHAIN[:10] + [
            "    /* 17C2B8 0027B2B8 1700093C */  lui        $t1, %hi(func_0016EE78)"]
        self.assertEqual(tu.fold_auto_functions(lines), lines)

    def test_extent_end_stops_at_the_next_run(self):
        runs = [(64, 0x0016E890), (65, 0x0016ECA4)]
        self.assertEqual(tu.extent_end(0x0016EC48, 0x0016ECE4, runs), 0x0016ECA4)
        self.assertEqual(tu.extent_end(0x0016E890, 0x0016E8A0, runs), 0x0016E8A0)


if __name__ == "__main__":
    unittest.main()
