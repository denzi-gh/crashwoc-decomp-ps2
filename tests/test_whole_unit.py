"""Unit tests for whole-unit mode (data/private): gen_hybrid's own literal
pools and data layout, the [[claim]] table (status.py, promote.py), the
`complete` rule and the claimed gap ranges of the image link.

Synthetic assembly in the shape ee-gcc emits; nothing here needs game files
or the toolchain.
"""
import struct
import sys
import tempfile
import tomllib
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
import gen_hybrid  # noqa: E402
from gen_hybrid import (HybridError, _Placed, _Pool, _attach_ranks,  # noqa: E402
                        _emit_atoms, _item_atoms, _rewrite_lis,
                        _split_sections, _whole_unit_layout,
                        _whole_unit_pieces, claim_text, manifest_claims,
                        pooled_gpr8, provides_data, whole_unit)
from link_image import build_script, claimed_ranges, gap_holes  # noqa: E402
from promote import set_complete, write_claims  # noqa: E402
from status import check_claims  # noqa: E402
from verify_promoted import complete_problems  # noqa: E402

UNIT = """\
\t.rdata
\t.align\t3
$LC0:
\t.ascii\t"first\\000"
\t.text
\t.align\t3
\t.globl\tlate
\t.ent\tlate
late:
\tli.s\t$f0,1.10000002384185791016e0
\tj\t$31
\t.end\tlate
\t.rdata
\t.align\t3
$LC1:
\t.ascii\t"second\\000"
\t.text
\t.align\t3
\t.globl\tearly
\t.ent\tearly
early:
\tli.d\t$5,1.00000000000000005551e-1
\tli.s\t$f1,1.0e0
\tli.d\t$5,1.0e1
\tj\t$31
\t.end\tearly
"""
# Retail order: `early` (0x100) before `late` (0x200).
FUNCTIONS = [(0x100, "early", "matching"), (0x200, "late", "matching")]


class TestPoolRewrite(unittest.TestCase):
    def setUp(self):
        self.pool = _Pool()
        self.lines, self.externs = _rewrite_lis(
            UNIT.splitlines(), "pal103", "unit-0001", "x", None, [None],
            pool=self.pool)

    def test_pooled_gpr8_rule(self):
        bits = lambda v: struct.unpack("<Q", struct.pack("<d", v))[0]
        self.assertTrue(pooled_gpr8(bits(0.1)))
        self.assertTrue(pooled_gpr8(bits(1e6)))
        self.assertTrue(pooled_gpr8(bits(1e-4)))
        self.assertFalse(pooled_gpr8(bits(1.0)))
        self.assertFalse(pooled_gpr8(bits(10.0)))

    def test_loads_address_own_labels(self):
        text = "\n".join(self.lines)
        self.assertIn("lwc1\t$f0,%gp_rel($Lpool0)($28)", text)
        self.assertIn("lui\t$at,%hi($Lpool1)", text)
        self.assertIn("ld\t$5,%lo($Lpool1)($at)", text)
        self.assertIn("dli\t$5,0x4024000000000000", text)
        self.assertIn("li.s\t$f1,1.0e0", text)        # inline: untouched
        self.assertEqual(self.externs, [])            # nothing borrowed

    def test_decisions_per_function_in_line_order(self):
        self.assertEqual(self.pool.entries, {
            "late": [("lit4", "$Lpool0")],
            "early": [("gpr8", "$Lpool1"), ("dli", None)]})

    def test_pool_blocks_land_in_the_data_stream(self):
        stream = []
        text, _private, _items = _split_sections(self.lines, stream)
        self.assertEqual([(i.name, i.section) for i in stream], [
            ("$LC0", ".rodata"), ("$Lpool0", ".lit4"), ("$LC1", ".rodata"),
            ("$Lpool1", ".rodata")])
        self.assertFalse(any("$Lpool0:" in l for l in text))

    def test_retail_order_follows_function_addresses(self):
        stream = []
        _split_sections(self.lines, stream)
        ranks = _attach_ranks(self.lines, stream, FUNCTIONS)
        self.assertEqual({i.name: ranks[id(i)] for i in stream},
                         {"$LC0": 1, "$Lpool0": 1, "$LC1": 0, "$Lpool1": 0})

    def test_initialiser_strings_stay_with_the_objects_above_the_code(self):
        # game/credits: the Credit table's strings come before credit_speed,
        # all above the first function; only what follows the last named
        # object travels with that function.
        lines = ["\t.data", "Credit:", "\t.word\t$LC0", "\t.sdata", "$LC0:",
                 '\t.ascii\t"a\\000"', "speed:", "\t.word\t1", "$LC1:",
                 '\t.ascii\t"b\\000"', "\t.text", "\t.ent\tlate", "late:",
                 "\t.end\tlate", "\t.ent\tearly", "early:", "\t.end\tearly"]
        stream = []
        _split_sections(lines, stream)
        ranks = _attach_ranks(lines, stream, FUNCTIONS)
        self.assertEqual({i.name: ranks[id(i)] for i in stream},
                         {"Credit": -1, "$LC0": -1, "speed": -1, "$LC1": 1})


class TestLayout(unittest.TestCase):
    def setUp(self):
        pool = _Pool()
        self.lines, _e = _rewrite_lis(UNIT.splitlines(), "pal103", "unit-0001",
                                      "x", None, [None], pool=pool)
        self.stream = []
        _split_sections(self.lines, self.stream)

    def layout(self, anchors):
        return _whole_unit_layout(self.lines, self.stream, FUNCTIONS, {},
                                  anchors, "unit-0001")

    def test_sections_in_retail_order_with_alignment(self):
        out = self.layout({"$LC1": 0x1000, "$Lpool0": 0x2000})
        base, placed = out[".rodata"]
        self.assertEqual(base, 0x1000)
        self.assertEqual([(p.label, p.offset, p.size) for p in placed],
                         [("$LC1", 0, 7), ("$Lpool1", 8, 8), ("$LC0", 16, 6)])
        self.assertEqual(out[".lit4"][0], 0x2000)

    def test_disagreeing_anchor_fails(self):
        with self.assertRaisesRegex(HybridError, "disagrees with retail"):
            self.layout({"$LC1": 0x1000, "$LC0": 0x1018, "$Lpool0": 0x2000})

    def test_unanchored_section_fails(self):
        with self.assertRaisesRegex(HybridError, "nothing anchors"):
            self.layout({"$LC1": 0x1000})


class TestAtoms(unittest.TestCase):
    def item(self, *lines):
        item = gen_hybrid.DataItem("$L9", ".rodata", 2)
        item.lines = list(lines)
        return item

    def test_symbolic_words_stay_relocations(self):
        atoms, size = _item_atoms(self.item("\t.word\t$L5,3"), 0)
        self.assertEqual(atoms, [(0, ("word", "$L5")), (4, b"\3\0\0\0")])
        self.assertEqual(size, 8)

    def test_alignment_is_relative_to_the_section(self):
        atoms, size = _item_atoms(self.item("\t.byte\t1", "\t.word\t2"), 4)
        self.assertEqual(atoms, [(4, b"\1"), (5, b"\0\0\0"),
                                 (8, b"\2\0\0\0")])
        self.assertEqual(size, 8)

    def test_emit_splits_at_window_and_labels(self):
        atoms = [(0x10, b"abcdefgh"), (0x18, ("word", "$L5"))]
        out = _emit_atoms(atoms, 0x12, 0x1C, {0x12: ["$LC3:"], 0x14: ["x:"]})
        self.assertEqual(out, ["$LC3:", "\t.byte\t0x63,0x64", "x:",
                               "\t.byte\t0x65,0x66,0x67,0x68",
                               "\t.word\t$L5"])

    def test_emit_refuses_a_hole_and_a_split_relocation(self):
        with self.assertRaisesRegex(HybridError, "empty"):
            _emit_atoms([(0x10, b"ab"), (0x14, b"cd")], 0x10, 0x16, {})
        with self.assertRaisesRegex(HybridError, "straddles"):
            _emit_atoms([(0x10, ("word", "$L5"))], 0x10, 0x12, {})


class TestPieces(unittest.TestCase):
    RANGES = [
        {"section": ".rodata", "start": 0x1000, "end": 0x1008,
         "owner": "unit-0001"},
        {"section": ".rodata", "start": 0x1008, "end": 0x1020,
         "owner": "unassigned"},
        {"section": ".rodata", "start": 0x1020, "end": 0x1040,
         "owner": "unit-0002"},
    ]

    def setUp(self):
        self._orig = gen_hybrid._retail_bytes
        gen_hybrid._retail_bytes = lambda v, s, lo, hi: bytes(hi - lo)
        self.tmp = tempfile.TemporaryDirectory()

    def tearDown(self):
        gen_hybrid._retail_bytes = self._orig
        self.tmp.cleanup()

    def placed(self, label, offset, blob):
        item = gen_hybrid.DataItem(label, ".rodata", 3)
        return _Placed(item, None, offset, [(offset, blob)], len(blob))

    def test_gap_bytes_become_claims(self):
        layout = {".rodata": (0x1000, [self.placed("$LC0", 0, b"x" * 8),
                                       self.placed("$L7", 8, b"y" * 12)])}
        lines, pieces, retail, claims = _whole_unit_pieces(
            layout, "unit-0001", "pal103", Path(self.tmp.name), self.RANGES)
        # The gap piece is exactly the claimed span; the image link fills
        # the rest of the gap.
        self.assertEqual([(p["start"], p["end"]) for p in pieces],
                         [(0x1000, 0x1008), (0x1008, 0x1014)])
        self.assertEqual(claims, [(".rodata", 0x1008, 0x1014)])
        self.assertEqual(retail, [])
        self.assertIn("$L7:", lines)

    def test_owned_bytes_the_c_lacks_are_retail(self):
        layout = {".rodata": (0x1000, [self.placed("$LC0", 0, b"x" * 4)])}
        _l, _p, retail, claims = _whole_unit_pieces(
            layout, "unit-0001", "pal103", Path(self.tmp.name), self.RANGES)
        self.assertEqual((retail, claims), ([(0x1004, 0x1008)], []))

    def test_reaching_into_another_unit_fails(self):
        layout = {".rodata": (0x1000, [self.placed("$LC0", 0, b"x" * 0x28)])}
        with self.assertRaisesRegex(HybridError, "unit-0002"):
            _whole_unit_pieces(layout, "unit-0001", "pal103",
                               Path(self.tmp.name), self.RANGES)


class TestClaims(unittest.TestCase):
    GAPS = [(".rodata", 0x1008, 0x1020), (".sdata", 0x2000, 0x2010)]

    def test_claim_text_round_trips(self):
        claims = [(".rodata", 0x1008, 0x1014), (".sdata", 0x2004, 0x2008)]
        data = tomllib.loads("complete = true\n" + claim_text(claims))
        self.assertEqual(manifest_claims(data), claims)
        self.assertTrue(whole_unit(data) and provides_data(data))

    def test_write_claims_replaces_the_table(self):
        text = "schema = 1\r\ncomplete = true\r\n"
        once = write_claims(text, [(".rodata", 0x1008, 0x1014)])
        self.assertIn('[[claim]]\r\nsection = ".rodata"\r\nstart = 0x00001008'
                      '\r\nend = 0x00001014\r\n', once)
        twice = write_claims(once, [(".sdata", 0x2004, 0x2008)])
        self.assertEqual(twice.count("[[claim]]"), 1)
        self.assertIn("0x00002004", twice)
        self.assertEqual(write_claims(twice, []), text)

    def test_set_complete_is_idempotent(self):
        self.assertEqual(set_complete("complete = true\n"), "complete = true\n")

    def test_check_claims(self):
        ok = [{"section": ".rodata", "start": 0x1008, "end": 0x1014}]
        self.assertEqual(check_claims(ok, True, self.GAPS),
                         ([], {(".rodata", 0x1008)}))
        problems, _u = check_claims(ok, False, self.GAPS)
        self.assertTrue(any("complete" in p for p in problems), problems)
        problems, _u = check_claims(
            [{"section": ".rodata", "start": 0x1000, "end": 0x1010}], True,
            self.GAPS)
        self.assertTrue(any("not inside one data-map gap" in p
                            for p in problems), problems)
        problems, _u = check_claims(ok + [{"section": ".rodata", "start": 0x1010,
                                           "end": 0x1018}], True, self.GAPS)
        self.assertTrue(any("overlaps" in p for p in problems), problems)


class TestCompleteRule(unittest.TestCase):
    RANGES = [
        {"section": ".rodata", "start": 0x100, "end": 0x160, "owner": "unit-0014"},
        {"section": ".lit4", "start": 0x200, "end": 0x210, "owner": "unit-0014"},
    ]

    def report(self, retail=(), borrowed=(), lit4=True):
        pieces = [{"name": ".rodata.split_00000100", "start": 0x100, "end": 0x160}]
        if lit4:
            pieces.append({"name": ".lit4.split_00000200", "start": 0x200,
                           "end": 0x210})
        return {"whole_unit": True, "pieces": pieces,
                "retail": [list(r) for r in retail], "claims": [],
                "borrowed": [{"symbol": f"D_{a:08X}", "address": a}
                             for a in borrowed]}

    def test_clean_unit(self):
        self.assertEqual(complete_problems({}, 14, "pal103", self.report(),
                                           self.RANGES), [])

    def test_lit4_must_come_from_c_too(self):
        why = complete_problems({}, 14, "pal103", self.report(retail=[(0x200, 0x210)]),
                                self.RANGES)
        self.assertTrue(any("retail bytes" in w for w in why), why)
        why = complete_problems({}, 14, "pal103", self.report(lit4=False),
                                self.RANGES)
        self.assertTrue(any("expected object" in w for w in why), why)

    def test_needs_whole_unit_mode(self):
        why = complete_problems({}, 14, "pal103", None, self.RANGES)
        self.assertTrue(any("whole-unit" in w for w in why), why)

    def test_borrowing_is_refused(self):
        why = complete_problems({}, 14, "pal103", self.report(borrowed=[0x150]),
                                self.RANGES)
        self.assertTrue(any("borrows" in w for w in why), why)


class TestClaimedRanges(unittest.TestCase):
    RANGES = [
        {"section": ".rodata", "start": 0x100, "end": 0x108, "owner": "unit-0001"},
        {"section": ".rodata", "start": 0x108, "end": 0x120, "owner": "unassigned"},
    ]

    def test_claimants_supply_their_spans(self):
        # game/credits and game/game_deb share one .rodata gap.
        objs = {1: Path("build/a.o"), 2: Path("build/b.o")}
        claimed = claimed_ranges(self.RANGES, {1: [(".rodata", 0x110, 0x118)],
                                               2: [(".rodata", 0x108, 0x10c)]},
                                 objs)
        self.assertEqual(claimed, {(".rodata", 0x108): [
            (0x108, 0x10c, objs[2]), (0x110, 0x118, objs[1])]})
        self.assertEqual(gap_holes(self.RANGES, claimed),
                         [(".rodata", 0x10c, 0x110), (".rodata", 0x118, 0x120)])
        script = build_script(0x1000, [], 0x2000, self.RANGES,
                              {"sbss": 0, "bss": 0}, {},
                              {"unit-0001": objs[1]}, claimed)
        self.assertIn("build/b.o(.rodata.split_00000108)\n"
                      "        *(.split.gapfill_0000010c)\n"
                      "        build/a.o(.rodata.split_00000110)\n"
                      "        *(.split.gapfill_00000118)", script)
        with self.assertRaisesRegex(ValueError, "overlap"):
            claimed_ranges(self.RANGES, {1: [(".rodata", 0x108, 0x110)],
                                         2: [(".rodata", 0x10c, 0x118)]}, objs)


if __name__ == "__main__":
    unittest.main()
