"""Unit tests for the pre-upload artifact smoke test.

smoke() and schema_problems() are pure over the staged report dict, the verify
document, and a unit-index map -- no toolchain, no game files, no real artifact.
The fixtures are shaped like a sanitized public report (a strict schema subset).
"""
import copy
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from smoke_report import smoke, schema_problems

UNIT_NAME = {7: "nucore/nulist"}

VERIFY = {
    "functions": [
        {"id": "pal103:unit-0007:00105a50:NuListGetHead",
         "state": "matching", "verified": True},
    ]
}

# A clean, sanitized-shape staged report: one code unit with a verified
# matching function at 100%, and one target-only data unit (honest total_data).
REPORT = {
    "version": 2,
    "measures": {"total_code": "560", "matched_code": "116",
                 "total_data": "256", "total_units": 2},
    "units": [
        {"name": "nucore/nulist",
         "measures": {"total_code": "560", "matched_code": "116",
                      "total_functions": 2},
         "sections": [{"name": ".text", "size": "560"}],
         "functions": [{"name": "NuListGetHead", "size": "8",
                        "address": "512", "fuzzy_match_percent": 100.0}],
         "metadata": {"source_path": "src/nucore/nulist.c"}},
        {"name": "data/unit-0001",
         "measures": {"total_data": "256", "total_units": 1},
         "sections": [{"name": ".split", "size": "256"}],
         "functions": [], "metadata": {}},
    ],
    "categories": [{"id": "data", "name": "Data",
                    "measures": {"total_data": "256"}}],
}


def report():
    return copy.deepcopy(REPORT)


class TestCleanArtifactPasses(unittest.TestCase):
    def test_no_problems(self):
        self.assertEqual(smoke(report(), VERIFY, UNIT_NAME), [])

    def test_schema_ok(self):
        self.assertEqual(schema_problems(report()), [])


class TestGuardsFire(unittest.TestCase):
    def test_absolute_path_is_flagged(self):
        r = report()
        r["units"][0]["metadata"]["source_path"] = "/work/orig/SLES_503.86"
        self.assertTrue(smoke(r, VERIFY, UNIT_NAME))

    def test_orig_reference_is_flagged(self):
        r = report()
        r["units"][0]["metadata"]["source_path"] = "orig/pal103/SLES_503.86"
        self.assertTrue(any("orig/" in p
                            for p in smoke(r, VERIFY, UNIT_NAME)))

    def test_unknown_field_is_flagged(self):
        r = report()
        r["units"][0]["measures"]["secret_debug_dump"] = "ff"
        probs = schema_problems(r)
        self.assertTrue(any("unknown measure" in p for p in probs))

    def test_unexpected_top_level_field_is_flagged(self):
        r = report()
        r["leak"] = {"x": 1}
        self.assertTrue(any("unexpected field 'leak'" in p
                            for p in schema_problems(r)))

    def test_hollow_measure_is_flagged(self):
        # A data-only unit that still claims 100% code: total_code absent but a
        # derived code measure present -> "100% of nothing".
        r = report()
        r["units"][1]["measures"]["matched_code_percent"] = 100.0
        probs = schema_problems(r)
        self.assertTrue(any("hollow 'matched_code_percent'" in p
                            for p in probs))

    def test_hollow_data_measure_is_flagged(self):
        r = report()
        # nucore/nulist has no total_data, so matched_data_percent is hollow.
        r["units"][0]["measures"]["matched_data_percent"] = 100.0
        self.assertTrue(any("hollow 'matched_data_percent'" in p
                            for p in schema_problems(r)))

    def test_matching_function_below_100_is_flagged(self):
        r = report()
        r["units"][0]["functions"][0]["fuzzy_match_percent"] = 99.5
        probs = smoke(r, VERIFY, UNIT_NAME)
        self.assertTrue(any("NuListGetHead" in p for p in probs))

    def test_missing_matching_function_is_flagged(self):
        r = report()
        r["units"][0]["functions"] = []
        self.assertTrue(smoke(r, VERIFY, UNIT_NAME))


if __name__ == "__main__":
    unittest.main()
