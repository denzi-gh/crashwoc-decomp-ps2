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


if __name__ == "__main__":
    unittest.main()
