"""Unit tests for the data-attribution algorithm and the committed data map.

build_ranges is a pure function, tested on synthetic symbols. The committed
config/pal103/data_map.toml (addresses and owners only -- no game bytes) is
validated against the committed section spec: exact tiling, well-formed
owners, known evidence kinds.
"""
import json
import re
import sys
import tomllib
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from extract_data_map import ORPHAN_END, ORPHAN_START, build_ranges
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
            self.assertIn(r["evidence"], ("mdebug", "gap", "orphan"))
            if r["owner"] == "unassigned":
                self.assertIn(r["evidence"], ("gap", "orphan"))
            else:
                self.assertEqual(r["evidence"], "mdebug")

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
