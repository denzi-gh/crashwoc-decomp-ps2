"""Unit tests for gen_report_data's pure parts (no toolchain, no game files)."""
import sys
import tomllib
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from gen_data_objects import range_section
from gen_report_data import c_ranges, report_data_units


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


class TestRangeSection(unittest.TestCase):
    def test_section_and_sized_symbol(self):
        text = range_section("data_00293700", Path("x/data_00293700.bin"), 304)
        self.assertIn('.section .split.data_00293700, "a", @progbits', text)
        self.assertIn(".type\tdata_00293700, @object", text)
        self.assertIn(".size\tdata_00293700, 304", text)
        self.assertIn("data_00293700:\n", text)
        self.assertNotIn(".globl", text)


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
