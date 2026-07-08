"""Unit tests for gen_hybrid's compiler-private data-block handling.

`_extract_local_data` and `_assemble_data_bytes` are pure text/byte helpers
(no toolchain, no game files): ee-gcc emits an initialized function-local
aggregate such as `short layertab[2] = {0,1}` as a `$LCn` label in a data
section, and these helpers pull that block out of the compiled stream and
recover its bytes so the label can be remapped onto the unit-owned retail slot.
`parse_s` must then see a pure-text stream and report the captured block.
"""
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from gen_hybrid import (HybridError, _JumpTable, _assemble_data_bytes,
                        _decode_as_string, _extract_local_data, _sonyize,
                        parse_s)

# A switch jump table exactly as ee-gcc -O2 emits it, mid-function, in a
# .rdata block whose entries are relocations to local code labels.
JTBL_BLOCK = [
    "\tlui\t$2,%hi($L11) # high",
    "\tsll\t$3,$4,2",
    "\taddiu\t$2,$2,%lo($L11) # low",
    "\taddu\t$3,$3,$2",
    "\tlw\t$4,0($3)",
    "\tj\t$4",
    "\t.p2align 2",
    "\t.rdata",
    "\t.align\t4",
    "$L11:",
    "\t.word\t$L4",
    "\t.word\t$L5",
    "\t.word\t$L6",
    "\t.text",
    "\t.p2align 2",
    "$L4:",
    "\tjal\tf0",
]

# The exact shape ee-gcc emits before DrawCharacterModel for layertab={0,1}.
SDATA_BLOCK = [
    "\t.text",
    "\t.globl\tProcessCreatures",
    "\t.ent\tProcessCreatures",
    "ProcessCreatures:",
    "\tnop",
    "\t.end\tProcessCreatures",
    "\t.sdata",
    "\t.align\t3",
    "$LC0:",
    "\t.half\t0",
    "\t.half\t1",
    "\t.text",
    "\t.align\t3",
    "\t.globl\tDrawCharacterModel",
    "\t.text",
    "\t.ent\tDrawCharacterModel",
    "DrawCharacterModel:",
    "\tlui\t$2,%hi($LC0)",
    "\t.end\tDrawCharacterModel",
]


class TestExtractLocalData(unittest.TestCase):
    def setUp(self):
        self.clean, self.local = _extract_local_data(list(SDATA_BLOCK))

    def test_block_captured_with_bytes(self):
        # {0, 1} as two little-endian shorts.
        self.assertEqual(self.local, {"$LC0": b"\x00\x00\x01\x00"})

    def test_data_directives_removed_from_stream(self):
        text = "\n".join(self.clean)
        self.assertNotIn(".sdata", text)
        self.assertNotIn(".half", text)
        self.assertNotIn("$LC0:", text)      # the label definition is gone

    def test_function_and_reference_survive(self):
        # The function body (and its %hi($LC0) reference, remapped later) stay.
        self.assertIn("DrawCharacterModel:", self.clean)
        self.assertIn("\tlui\t$2,%hi($LC0)", self.clean)

    def test_no_block_is_a_noop(self):
        lines = ["\t.text", "\t.ent\tfoo", "\tnop", "\t.end\tfoo"]
        clean, local = _extract_local_data(list(lines))
        self.assertEqual(clean, lines)
        self.assertEqual(local, {})

    def test_multi_label_block_splits_per_label(self):
        # ee-gcc packs several private slots between one .sdata/.text switch
        # (e.g. the two format strings of a function that calls NuDebugMsgProlog
        # twice); each label is captured as its own aggregate.
        _clean, local = _extract_local_data(
            ["\t.sdata", "$LC0:", "\t.word\t1",
             "$LC1:", "\t.word\t2", "\t.text"])
        self.assertEqual(local, {"$LC0": b"\x01\x00\x00\x00",
                                 "$LC1": b"\x02\x00\x00\x00"})

    def test_multi_label_strings_split_without_interior_align(self):
        # The .align between two strings only aligns the second slot, so it is
        # not part of the first string's bytes.
        _clean, local = _extract_local_data(
            ["\t.rdata", "$LC0:", '\t.ascii\t"ab\\000"',
             "\t.align\t2", "$LC1:", '\t.ascii\t"cd\\000"', "\t.text"])
        self.assertEqual(local, {"$LC0": b"ab\x00", "$LC1": b"cd\x00"})

    def test_labelless_block_raises(self):
        with self.assertRaises(HybridError):
            _extract_local_data(["\t.sdata", "\t.word\t1", "\t.text"])


class TestAssembleDataBytes(unittest.TestCase):
    def test_directives_and_alignment(self):
        block = ["\t.align\t3", "$LC0:", "\t.byte\t1", "\t.align\t2",
                 "\t.half\t0x0102", "\t.word\t0x03040506"]
        # byte 01, pad to 4-byte boundary (000000), half 0102, word 03040506.
        self.assertEqual(
            _assemble_data_bytes(block),
            b"\x01\x00\x00\x00" + b"\x02\x01" + b"\x06\x05\x04\x03")

    def test_unknown_directive_raises(self):
        with self.assertRaises(HybridError):
            _assemble_data_bytes(["\t.reloc\tfoo"])


class TestStringDirectives(unittest.TestCase):
    def test_ascii_keeps_bytes_verbatim(self):
        # .ascii does NOT append a NUL; gcc writes the terminator explicitly.
        self.assertEqual(_decode_as_string('"hello\\000"'), b"hello\x00")
        self.assertEqual(_decode_as_string('"abc"'), b"abc")

    def test_asciz_family_appends_nul(self):
        for d in (".asciiz", ".asciz", ".string"):
            self.assertEqual(_assemble_data_bytes([f'{d}\t"hi"']), b"hi\x00")
        # .ascii does not.
        self.assertEqual(_assemble_data_bytes(['.ascii\t"hi"']), b"hi")

    def test_comma_and_hash_inside_quotes_are_literal(self):
        # A '#' or ',' inside the quotes must not be read as comment/separator.
        self.assertEqual(_decode_as_string('"a,b#c"'), b"a,b#c")

    def test_concatenated_runs(self):
        self.assertEqual(_decode_as_string('"ab", "cd"'), b"abcd")

    def test_octal_and_hex_escapes(self):
        self.assertEqual(_decode_as_string('"\\015\\012"'), b"\r\n")
        self.assertEqual(_decode_as_string('"\\x41\\x42"'), b"AB")

    def test_trailing_comment_after_string(self):
        self.assertEqual(_decode_as_string('"ok"  # a note'), b"ok")

    def test_string_block_assembles_with_alignment(self):
        block = ["\t.align\t2", "$LC0:", '\t.ascii\t"%d\\000"']
        self.assertEqual(_assemble_data_bytes(block), b"%d\x00")

    def test_bad_escape_raises(self):
        with self.assertRaises(HybridError):
            _decode_as_string('"\\q"')

    def test_unterminated_raises(self):
        with self.assertRaises(HybridError):
            _decode_as_string('"oops')


class TestJumpTable(unittest.TestCase):
    def setUp(self):
        self.clean, self.local = _extract_local_data(list(JTBL_BLOCK))

    def test_table_captured_as_marker(self):
        # Relocation entries -> a JumpTable marker (count), never bytes.
        self.assertEqual(self.local, {"$L11": _JumpTable(3)})

    def test_table_data_removed_but_refs_and_targets_survive(self):
        text = "\n".join(self.clean)
        self.assertNotIn(".word", text)      # the table itself is gone
        self.assertNotIn(".rdata", text)
        self.assertIn("%hi($L11)", text)     # the dispatch load stays (remapped)
        self.assertIn("$L4:", self.clean)    # jump targets stay in the code
        self.assertNotIn("$L11:", self.clean)  # the data label definition is gone

    def test_all_word_symbol_block_is_a_table_not_bytes(self):
        _clean, local = _extract_local_data(
            ["\t.rdata", "$L1:", "\t.word\t$L2", "\t.word\t$L3", "\t.text"])
        self.assertEqual(local, {"$L1": _JumpTable(2)})

    def test_numeric_word_block_stays_bytes(self):
        # A .word with a numeric value is a constant aggregate, not a table.
        _clean, local = _extract_local_data(
            ["\t.data", "$LC0:", "\t.word\t1", "\t.word\t2", "\t.text"])
        self.assertEqual(local, {"$LC0": b"\x01\x00\x00\x00\x02\x00\x00\x00"})


class TestSonyizeJumpReg(unittest.TestCase):
    def test_j_register_becomes_jr(self):
        self.assertEqual(_sonyize("\tj\t$4"), "\tjr\t$4")
        self.assertEqual(_sonyize("\tj\t$ra"), "\tjr\t$ra")

    def test_j_label_untouched(self):
        # A jump to a code label must NOT be rewritten to jr.
        self.assertEqual(_sonyize("\tj\t$L13"), "\tj\t$L13")
        self.assertEqual(_sonyize("\tj\t.L001CC7A0"), "\tj\t.L001CC7A0")


class TestParseSReturnsLocalData(unittest.TestCase):
    def test_parse_s_surfaces_captured_block(self):
        prologue, segments, local = parse_s("\n".join(SDATA_BLOCK))
        self.assertEqual(local, {"$LC0": b"\x00\x00\x01\x00"})
        names = [n for n, _seg in segments]
        self.assertIn("DrawCharacterModel", names)
        self.assertIn("ProcessCreatures", names)


if __name__ == "__main__":
    unittest.main()
