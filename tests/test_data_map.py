"""Unit tests for the data-attribution algorithm and the committed data map.

build_ranges and xref_items are pure functions, tested on synthetic items.
The committed config/pal103/data_map.toml (addresses and owners only -- no
game bytes) is validated against the committed section spec: exact tiling,
well-formed owners, known evidence kinds, and the attributions the data plan
measured (unit-0054's code-referenced .rodata run, the 57 .lit4 pools).
"""
import json
import re
import sys
import tomllib
import unittest
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from extract_data_map import (ORPHAN_END, ORPHAN_START, build_ranges,
                              xref_items)
from gen_data_objects import NOBITS_SECTIONS, object_groups


class TestBuildRanges(unittest.TestCase):
    def test_two_runs_with_gaps(self):
        # unit 1 owns [10,20), then a gap, then unit 2 owns [30,40).
        syms = [(10, 1), (14, 1), (20, 1), (30, 2), (40, 2)]
        self.assertEqual(build_ranges(0, 50, syms), [
            (0, 10, "unassigned", "gap"),
            (10, 20, "unit-0001", "mdebug"),
            (20, 30, "unassigned", "gap"),
            (30, 40, "unit-0002", "mdebug"),
            (40, 50, "unassigned", "gap"),
        ])

    def test_single_symbol_run_proves_no_extent(self):
        ranges = build_ranges(0, 20, [(8, 3)])
        self.assertEqual(ranges, [(0, 8, "unassigned", "gap"),
                                  (8, 20, "unassigned", "gap")])

    def test_interleaved_units_degrade(self):
        self.assertIsNone(build_ranges(0, 50, [(10, 1), (20, 2), (30, 1)]))

    def test_conflicting_claim_degrades(self):
        self.assertIsNone(build_ranges(0, 50, [(10, 1), (10, 2)]))

    def test_empty_section_is_one_gap(self):
        self.assertEqual(build_ranges(0, 16, []),
                         [(0, 16, "unassigned", "gap")])

    def test_adjacent_runs_no_gap_between(self):
        syms = [(0, 1), (8, 1), (8, 1), (12, 2), (16, 2)]
        # unit 1's last symbol at 8 is also unit 2's run start? No -- 8 is
        # unit 1's; unit 2 starts at 12, so [8,12) is a gap.
        self.assertEqual(build_ranges(0, 16, syms), [
            (0, 8, "unit-0001", "mdebug"),
            (8, 12, "unassigned", "gap"),
            (12, 16, "unit-0002", "mdebug"),
        ])


def named(start, size, unit):
    return (start, start + size, unit, "mdebug")


def xref(start, width, unit):
    return (start, start + width, unit, "xref")


class TestSizedAndXrefRanges(unittest.TestCase):
    def test_sized_single_symbol_owns_its_bytes(self):
        self.assertEqual(build_ranges(0, 32, [named(8, 16, 1)]), [
            (0, 8, "unassigned", "gap"),
            (8, 24, "unit-0001", "mdebug"),
            (24, 32, "unassigned", "gap"),
        ])

    def test_run_ends_at_the_largest_end(self):
        ranges = build_ranges(0, 64, [named(0, 4, 1), named(8, 24, 1),
                                      named(16, 4, 1)])
        self.assertEqual(ranges[0], (0, 32, "unit-0001", "mdebug"))

    def test_xref_extends_a_named_run(self):
        ranges = build_ranges(0, 64, [named(0, 8, 1), xref(16, 4, 1)])
        self.assertEqual(ranges[0], (0, 20, "unit-0001", "mdebug"))

    def test_xref_only_run(self):
        self.assertEqual(build_ranges(0, 16, [xref(4, 4, 3), xref(8, 4, 3)]), [
            (0, 4, "unassigned", "gap"),
            (4, 12, "unit-0003", "xref"),
            (12, 16, "unassigned", "gap"),
        ])

    def test_address_only_xref_owns_nothing_alone(self):
        self.assertEqual(build_ranges(0, 16, [xref(4, 0, 3)]),
                         [(0, 4, "unassigned", "gap"),
                          (4, 16, "unassigned", "gap")])

    def test_xref_into_a_neighbour_is_dropped(self):
        # unit 2's code reaches past unit 1's named run: the xref goes, the
        # named runs stay.
        ranges = build_ranges(0, 64, [named(0, 8, 2), named(16, 8, 1),
                                      xref(32, 4, 2)])
        self.assertEqual([r[2] for r in ranges if r[2] != "unassigned"],
                         ["unit-0002", "unit-0001"])
        self.assertEqual(ranges[0], (0, 8, "unit-0002", "mdebug"))

    def test_interleaved_xref_only_units_are_dropped(self):
        ranges = build_ranges(0, 64, [xref(0, 4, 1), xref(8, 4, 2),
                                      xref(16, 4, 1)])
        self.assertEqual(ranges, [(0, 64, "unassigned", "gap")])

    def test_sized_overlap_raises(self):
        with self.assertRaises(ValueError):
            build_ranges(0, 64, [named(0, 16, 1), named(8, 4, 2)])


class TestXrefItems(unittest.TestCase):
    def test_single_unit_outside_sized_symbols(self):
        refs = [(0x10, 4, 1, None), (0x10, 8, 1, None),   # widest access wins
                (0x20, 4, 1, None), (0x20, 4, 2, None),   # two units: dropped
                (0x34, 4, 1, None),                       # inside a symbol
                (0x80, 4, 1, None)]                       # outside [lo, hi)
        items = xref_items(refs, [named(0x30, 8, 1)], 0, 0x40)
        self.assertEqual(items, [(0x10, 0x18, 1, "xref")])


class TestCommittedDataMap(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.ranges = tomllib.loads(
            (ROOT / "config/pal103/data_map.toml").read_text())["range"]
        spec = json.loads((ROOT / "config/pal103/sections.json").read_text())
        cls.sections = {s["name"]: (int(s["addr"], 0),
                                    int(s["addr"], 0) + int(s["size"], 0))
                        for s in spec["sections"]}

    def by_section(self, name):
        return sorted((r for r in self.ranges if r["section"] == name),
                      key=lambda r: r["start"])

    def test_every_section_tiles_exactly(self):
        for name in (".data", ".rodata", ".lit4", ".sdata", ".sbss", ".bss"):
            lo, hi = self.sections[name]
            cursor = lo
            for r in self.by_section(name):
                self.assertEqual(r["start"], cursor, name)
                self.assertGreater(r["end"], r["start"], name)
                cursor = r["end"]
            self.assertEqual(cursor, hi, name)

    def test_orphan_is_homed(self):
        (orphan,) = self.by_section("orphan")
        self.assertEqual((orphan["start"], orphan["end"]),
                         (ORPHAN_START, ORPHAN_END))
        self.assertEqual(orphan["evidence"], "orphan")
        self.assertEqual(orphan["owner"], "unassigned")

    def test_owners_and_evidence_well_formed(self):
        for r in self.ranges:
            self.assertTrue(re.fullmatch(r"unit-\d{4}|unassigned",
                                         r["owner"]), r["owner"])
            self.assertIn(r["evidence"], ("mdebug", "xref", "gap", "orphan"))
            if r["owner"] == "unassigned":
                self.assertIn(r["evidence"], ("gap", "orphan"))
            else:
                self.assertIn(r["evidence"], ("mdebug", "xref"))
            if r["evidence"] == "xref":
                self.assertIn(r["section"], (".rodata", ".lit4"))

    def test_one_owned_range_per_unit_and_section(self):
        owned = Counter((r["owner"], r["section"]) for r in self.ranges
                        if r["owner"] != "unassigned")
        self.assertEqual([k for k, n in owned.items() if n > 1], [])

    def test_test_shadow_cast_aggregate_is_owned(self):
        # testShadowCast (unit-0054) builds 0x615FA0 in the .rodata gap;
        # only its code references attribute it.
        hit = [r for r in self.by_section(".rodata")
               if r["start"] <= 0x00615FA0 < r["end"]]
        self.assertEqual([(r["start"], r["end"], r["owner"]) for r in hit],
                         [(0x00615BD8, 0x00615FC0, "unit-0054")])

    def test_lit4_is_57_unit_pools(self):
        lit4 = self.by_section(".lit4")
        self.assertEqual(len(lit4), 57)
        self.assertTrue(all(r["owner"] != "unassigned" and r["evidence"] == "xref"
                            for r in lit4))
        self.assertEqual(len({r["owner"] for r in lit4}), 57)

    def test_object_groups_cover_all_progbits_ranges(self):
        progbits = [r for r in self.ranges
                    if r["section"] not in NOBITS_SECTIONS]
        groups = object_groups(progbits)
        covered = sum(len(entries) for _stem, entries in groups)
        self.assertEqual(covered, len(progbits))
        stems = [stem for stem, _e in groups]
        self.assertEqual(len(stems), len(set(stems)))
        for stem in stems:
            self.assertTrue(re.fullmatch(
                r"unit-\d{4}|unassigned/[a-z0-9]+_[0-9a-f]{8}", stem), stem)


if __name__ == "__main__":
    unittest.main()
