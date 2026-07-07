"""Unit tests for the report-honors-matching CI gate.

verified_matching() and check() are pure over the two JSON documents plus a
unit-index -> objdiff-name map, so no toolchain, no game files, and no real
report are needed.
"""
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from check_report_matches import check, verified_matching

UNIT_NAME = {7: "nucore/nulist", 91: "game/creature"}

VERIFY = {
    "functions": [
        {"id": "pal103:unit-0007:00105a50:NuListGetHead",
         "state": "matching", "verified": True},
        {"id": "pal103:unit-0007:00105a80:NuListGetTail",
         "state": "matching", "verified": False},   # not verified yet
        {"id": "pal103:unit-0091:00300000:TerrainFailsafe",
         "state": "matching", "verified": True},
        {"id": "pal103:unit-0091:00300100:SomeEquivalent",
         "state": "equivalent", "verified": True},   # equivalent, not matching
    ]
}


def report_with(head=100.0, terrain=100.0, drop=None):
    units = [
        {"name": "nucore/nulist",
         "functions": [{"name": "NuListGetHead", "fuzzy_match_percent": head}]},
        {"name": "game/creature",
         "functions": [{"name": "TerrainFailsafe",
                        "fuzzy_match_percent": terrain}]},
    ]
    if drop == "unit":
        units = units[:1]                       # game/creature missing entirely
    elif drop == "function":
        units[1]["functions"] = []              # TerrainFailsafe missing
    return {"units": units}


class TestVerifiedMatching(unittest.TestCase):
    def test_only_matching_and_verified(self):
        self.assertEqual(verified_matching(VERIFY),
                         [(7, "NuListGetHead"), (91, "TerrainFailsafe")])


class TestCheck(unittest.TestCase):
    def test_all_100_passes(self):
        matched, problems = check(VERIFY, report_with(), UNIT_NAME)
        self.assertEqual(problems, [])
        self.assertEqual(matched, 2)

    def test_below_100_is_flagged(self):
        matched, problems = check(VERIFY, report_with(terrain=99.5), UNIT_NAME)
        self.assertEqual(matched, 1)
        self.assertEqual(len(problems), 1)
        self.assertIn("TerrainFailsafe", problems[0])
        self.assertIn("99.5", problems[0])

    def test_missing_function_is_flagged(self):
        _matched, problems = check(VERIFY, report_with(drop="function"),
                                   UNIT_NAME)
        self.assertTrue(any("not present" in p for p in problems))

    def test_missing_unit_is_flagged(self):
        _matched, problems = check(VERIFY, report_with(drop="unit"), UNIT_NAME)
        self.assertTrue(any("absent from the report" in p for p in problems))

    def test_equivalent_and_unverified_are_not_required(self):
        # Only the two matching+verified functions are checked; the report
        # need not even mention the equivalent or unverified ones.
        matched, problems = check(VERIFY, report_with(), UNIT_NAME)
        self.assertEqual(matched, 2)
        self.assertEqual(problems, [])


if __name__ == "__main__":
    unittest.main()
