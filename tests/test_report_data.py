"""Unit tests for gen_report_data's pure parts (no toolchain, no game files)."""
import sys
import tomllib
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from gen_data_objects import (load_claims, load_map, object_groups,
                              range_section, split_claims)
from gen_report_data import (c_ranges, claim_pieces, claim_stem,
                             report_data_units)


def rng(section, start, end):
    return {"section": section, "start": start, "end": end,
            "owner": "unit-0002"}


def piece(section, start, end):
    return {"name": f"{section}.split_{start:08x}", "section": section,
            "start": start, "end": end}


class TestCRanges(unittest.TestCase):
    RANGES = [rng(".data", 0x100, 0x140), rng(".rodata", 0x200, 0x210)]
    PIECES = [piece(".data", 0x100, 0x140), piece(".rodata", 0x200, 0x210)]

    def test_all_from_c(self):
        got = c_ranges(self.RANGES, self.PIECES, [])
        self.assertEqual([r["start"] for r, _p in got], [0x100, 0x200])

    def test_retail_span_drops_its_range(self):
        got = c_ranges(self.RANGES, self.PIECES, [[0x120, 0x124]])
        self.assertEqual([r["start"] for r, _p in got], [0x200])

    def test_adjacent_retail_span_does_not_drop(self):
        got = c_ranges(self.RANGES, self.PIECES, [[0x140, 0x150]])
        self.assertEqual(len(got), 2)

    def test_range_without_piece_is_skipped(self):
        got = c_ranges(self.RANGES, self.PIECES[:1], [])
        self.assertEqual([r["start"] for r, _p in got], [0x100])

    def test_claim_piece_never_matches_an_owned_range(self):
        claim = piece(".rodata", 0x210, 0x21c)
        got = c_ranges(self.RANGES, [claim], [])
        self.assertEqual(got, [])

    def test_nobits_range_with_its_piece(self):
        bss = rng(".bss", 0x400, 0x480)
        got = c_ranges([bss], [piece(".bss", 0x400, 0x480)], [])
        self.assertEqual([r["section"] for r, _p in got], [".bss"])


class TestClaims(unittest.TestCase):
    GAP = {"section": ".rodata", "start": 0x100, "end": 0x140,
           "owner": "unassigned", "evidence": "gap"}
    OWNED = {"section": ".rodata", "start": 0x140, "end": 0x150,
             "owner": "unit-0002", "evidence": "mdebug"}

    def spans(self, claims):
        return [(r["start"], r["end"], r.get("claimant"))
                for r in split_claims([self.GAP, self.OWNED], claims)]

    def test_unclaimed_ranges_pass_through(self):
        self.assertEqual(self.spans([]), [(0x100, 0x140, None),
                                          (0x140, 0x150, None)])

    def test_gap_cut_at_claim_boundaries(self):
        got = self.spans([(".rodata", 0x108, 0x110, "unit-0003"),
                          (".rodata", 0x120, 0x140, "unit-0004")])
        self.assertEqual(got, [(0x100, 0x108, None),
                               (0x108, 0x110, "unit-0003"),
                               (0x110, 0x120, None),
                               (0x120, 0x140, "unit-0004"),
                               (0x140, 0x150, None)])

    def test_whole_gap_claimed(self):
        got = self.spans([(".rodata", 0x100, 0x140, "unit-0003")])
        self.assertEqual(got[0], (0x100, 0x140, "unit-0003"))

    def test_claim_outside_a_gap_fails(self):
        with self.assertRaises(ValueError):
            self.spans([(".rodata", 0x13c, 0x144, "unit-0003")])

    def test_overlapping_claims_fail(self):
        with self.assertRaises(ValueError):
            self.spans([(".rodata", 0x108, 0x110, "unit-0003"),
                        (".rodata", 0x10c, 0x114, "unit-0004")])

    def test_committed_claims_tile_their_gaps(self):
        ranges = load_map("pal103")
        split = split_claims(ranges, load_claims("pal103"))
        for section in {r["section"] for r in ranges}:
            want = sum(r["end"] - r["start"] for r in ranges
                       if r["section"] == section)
            got = [(r["start"], r["end"]) for r in split
                   if r["section"] == section]
            self.assertEqual(sum(e - s for s, e in got), want)
            got.sort()
            self.assertTrue(all(a[1] == b[0] for a, b in zip(got, got[1:])))
        stems = [stem for stem, _e in object_groups(split)]
        self.assertEqual(len(stems), len(set(stems)))
        claimed = {(r["section"], r["start"]) for r in split
                   if "claimant" in r}
        self.assertEqual(len(claimed), len(load_claims("pal103")))

    def test_claim_stem_names_the_split_piece(self):
        self.assertEqual(claim_stem(".sbss", 0x00633234),
                         "unassigned/sbss_00633234")

    def test_claim_pieces(self):
        pieces = [piece(".rodata", 0x108, 0x110)]
        got = claim_pieces([(".rodata", 0x108, 0x110)], pieces)
        self.assertEqual(got, [({"section": ".rodata", "start": 0x108,
                                 "end": 0x110}, pieces[0])])
        with self.assertRaises(SystemExit):
            claim_pieces([(".rodata", 0x108, 0x114)], pieces)


class TestRangeSection(unittest.TestCase):
    def test_section_and_sized_symbol(self):
        text = range_section("data_00293700", Path("x/data_00293700.bin"), 304)
        self.assertIn('.section .split.data_00293700, "a", @progbits', text)
        self.assertIn(".type\tdata_00293700, @object", text)
        self.assertIn(".size\tdata_00293700, 304", text)
        self.assertIn("data_00293700:\n", text)
        self.assertNotIn(".globl", text)

    def test_nobits_section(self):
        text = range_section("bss_00633400", None, 256)
        self.assertIn('.section .split.bss_00633400, "aw", @nobits', text)
        self.assertIn(".size\tbss_00633400, 256", text)
        self.assertIn("\t.space\t256\n", text)
        self.assertNotIn(".incbin", text)


class TestReportDataUnits(unittest.TestCase):
    def test_units_follow_manifests(self):
        units = report_data_units("pal103")
        self.assertEqual(len({s for s, _m, _c in units}), len(units))
        for stem, manifest, complete in units:
            data = tomllib.loads((ROOT / manifest).read_text())
            self.assertEqual(data["unit"], f"pal103:{stem}")
            self.assertEqual(complete, bool(data.get("complete")))
            self.assertTrue(complete or any(
                d.get("state") == "matching" for d in data.get("data", ())))

    def test_deterministic(self):
        self.assertEqual(report_data_units("pal103"),
                         report_data_units("pal103"))


if __name__ == "__main__":
    unittest.main()
