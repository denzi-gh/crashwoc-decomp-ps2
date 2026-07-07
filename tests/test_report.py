"""Unit tests for the report sanitizer (publication whitelist + guards).

sanitize() and violations() are pure functions over a report dict; the
fixture is synthetic -- no toolchain, no game files, no real report needed.
"""
import copy
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from sanitize_report import MEASURE_KEYS, sanitize, violations

# Shaped like a real objdiff-cli 3.7.2 report, with hostile extras planted
# in every non-whitelisted spot the schema offers.
REPORT = {
    "version": 2,
    "measures": {
        "fuzzy_match_percent": 2.9,
        "total_code": "560",
        "matched_code": "16",
        "total_units": 1,
        "secret_debug_dump": "ff" * 64,          # not a known measure
    },
    "units": [{
        "name": "nucore/nulist",
        "measures": {"fuzzy_match_percent": 2.9, "total_functions": 2},
        "sections": [{
            "name": ".text", "size": "576", "fuzzy_match_percent": 2.9,
            "metadata": {"leak": "orig/pal103/SLES_503.86"},
        }],
        "functions": [
            {"name": "NuListGetHead", "size": "8", "address": "512",
             "fuzzy_match_percent": 100.0,
             "metadata": {"raw_bytes": "9c" * 40}},
            {"name": "NuListGetTail", "size": "8", "address": "520",
             "fuzzy_match_percent": 100.0, "metadata": {}},
        ],
        "metadata": {
            "complete": False,
            "source_path": "src/nucore/nulist.c",
            "progress_categories": ["engine"],
            "module_name": "C:\\Users\\someone\\Desktop\\repo",
        },
        "extra_blob": "QUJD" * 32,
    }],
    "categories": [{
        "id": "engine", "name": "Nu engine",
        "measures": {"total_units": 1}, "runner_path": "/work/orig",
    }],
    "unknown_top_level": {"anything": True},
}


class TestSanitize(unittest.TestCase):
    def setUp(self):
        self.public, self.dropped = sanitize(copy.deepcopy(REPORT))

    def test_whitelisted_fields_survive(self):
        self.assertEqual(self.public["measures"]["total_code"], "560")
        (unit,) = self.public["units"]
        self.assertEqual(unit["name"], "nucore/nulist")
        self.assertEqual(unit["metadata"]["source_path"],
                         "src/nucore/nulist.c")
        self.assertEqual(unit["functions"][0]["fuzzy_match_percent"], 100.0)
        self.assertEqual(unit["sections"][0]["size"], "576")
        (cat,) = self.public["categories"]
        self.assertEqual(cat["measures"], {"total_units": 1})

    def test_everything_hostile_is_dropped(self):
        self.assertIn("measures.secret_debug_dump", self.dropped)
        self.assertIn("unit.section.metadata", self.dropped)
        self.assertIn("unit.function.metadata", self.dropped)
        self.assertIn("unit.metadata.module_name", self.dropped)
        self.assertIn("unit.extra_blob", self.dropped)
        self.assertIn("category.runner_path", self.dropped)
        self.assertIn("unknown_top_level", self.dropped)

    def test_sanitized_output_passes_the_guard(self):
        self.assertEqual(violations(self.public), [])

    def test_output_is_a_strict_schema_subset(self):
        # decomp.dev parses the published artifact as a protobuf-JSON
        # Report; the sanitizer may only remove fields, never add them.
        self.assertLessEqual(set(self.public),
                             {"version", "measures", "units", "categories"})
        for unit in self.public["units"]:
            self.assertLessEqual(set(unit), {"name", "measures", "sections",
                                             "functions", "metadata"})

    def test_measure_whitelist_is_the_known_cli_schema(self):
        # Every measure the fixture uses must be a real whitelisted key.
        for key in self.public["measures"]:
            self.assertIn(key, MEASURE_KEYS)


class TestHollowData(unittest.TestCase):
    """The data-completeness measures must not claim 100% of nothing."""

    def _report(self, total_data):
        # A measures block that claims fully-complete data, planted at the
        # report, unit and category levels.
        measures = {
            "fuzzy_match_percent": 2.9,
            "total_code": "560", "matched_code": "16",
            "total_data": total_data,
            "matched_data": "8", "matched_data_percent": 100.0,
            "complete_data": "8", "complete_data_percent": 100.0,
        }
        return {
            "version": 2,
            "measures": copy.deepcopy(measures),
            "units": [{"name": "nucore/nulist",
                       "measures": copy.deepcopy(measures),
                       "sections": [], "functions": [], "metadata": {}}],
            "categories": [{"id": "engine", "name": "Nu engine",
                            "measures": copy.deepcopy(measures)}],
        }

    def _blocks(self, public):
        yield public["measures"]
        for u in public["units"]:
            yield u["measures"]
        for c in public["categories"]:
            yield c["measures"]

    def test_zero_data_strips_derived_measures_everywhere(self):
        public, _ = sanitize(self._report("0"))
        for m in self._blocks(public):
            self.assertNotIn("matched_data", m)
            self.assertNotIn("matched_data_percent", m)
            self.assertNotIn("complete_data", m)
            self.assertNotIn("complete_data_percent", m)
            self.assertEqual(m["total_data"], "0")   # honest 0 stays
            self.assertEqual(m["total_code"], "560")  # code measures untouched

    def test_absent_total_data_also_strips(self):
        report = self._report("0")
        for level in (report["measures"], report["units"][0]["measures"],
                      report["categories"][0]["measures"]):
            del level["total_data"]
        public, _ = sanitize(report)
        for m in self._blocks(public):
            self.assertNotIn("matched_data_percent", m)
            self.assertNotIn("complete_data_percent", m)

    def test_real_data_measures_are_preserved(self):
        public, _ = sanitize(self._report("128"))
        for m in self._blocks(public):
            self.assertEqual(m["total_data"], "128")
            self.assertEqual(m["matched_data_percent"], 100.0)
            self.assertEqual(m["complete_data_percent"], 100.0)

    def test_output_stays_a_schema_subset(self):
        # Stripping never introduces a field; the result is still a subset.
        public, _ = sanitize(self._report("0"))
        for m in self._blocks(public):
            self.assertLessEqual(set(m), MEASURE_KEYS)


class TestHollowCode(unittest.TestCase):
    """The mirror of TestHollowData: a data-only unit must not claim 100%
    code/functions/fuzzy against zero code (objdiff emits those for the data
    units introduced by the data-map modelling)."""

    def _data_only_report(self):
        # Shaped like objdiff's output for a target-only data unit: real data
        # bytes, but "100% of nothing" code/function/fuzzy measures.
        hollow = {
            "fuzzy_match_percent": 100.0,
            "matched_code_percent": 100.0, "complete_code_percent": 100.0,
            "matched_functions_percent": 100.0,
            "total_data": "256",
        }
        return {
            "version": 2,
            "measures": dict(hollow),
            "units": [{"name": "data/unit-0001", "measures": dict(hollow),
                       "sections": [], "functions": [], "metadata": {}}],
            "categories": [{"id": "data", "name": "Data",
                            "measures": dict(hollow)}],
        }

    def _blocks(self, public):
        yield public["measures"]
        yield public["units"][0]["measures"]
        yield public["categories"][0]["measures"]

    def test_hollow_code_and_fuzzy_are_stripped(self):
        public, _ = sanitize(self._data_only_report())
        for m in self._blocks(public):
            self.assertNotIn("fuzzy_match_percent", m)
            self.assertNotIn("matched_code_percent", m)
            self.assertNotIn("complete_code_percent", m)
            self.assertNotIn("matched_functions_percent", m)
            self.assertEqual(m["total_data"], "256")   # honest data stays

    def test_real_code_keeps_fuzzy(self):
        # A block with actual code keeps its code/fuzzy measures.
        rep = self._data_only_report()
        rep["measures"]["total_code"] = "560"
        rep["measures"]["matched_code"] = "16"
        public, _ = sanitize(rep)
        self.assertEqual(public["measures"]["fuzzy_match_percent"], 100.0)
        self.assertEqual(public["measures"]["matched_code_percent"], 100.0)


class TestViolations(unittest.TestCase):
    def test_orig_reference_is_blocked(self):
        found = violations({"name": "orig/pal103/SLES_503.86"})
        self.assertTrue(any("orig/" in v for v in found))

    def test_orig_needs_path_context(self):
        # "origin"-like words are not the game directory.
        self.assertEqual(violations({"name": "game/original_level"}), [])

    def test_absolute_paths_are_blocked(self):
        for path in ("C:\\Users\\x\\repo", "/work/expected/pal103/a.o"):
            self.assertTrue(violations({"p": path}), path)

    def test_hex_blob_is_blocked(self):
        found = violations({"name": "9c" * 32})
        self.assertTrue(any("hex blob" in v for v in found))

    def test_base64_blob_is_blocked(self):
        found = violations({"name": "QUJD" * 16 + "=="})
        self.assertTrue(any("base64 blob" in v for v in found))

    def test_ordinary_symbols_pass(self):
        clean = {"units": [{"name": "nucore/nulist", "functions": [
            {"name": "NuListGetHead", "address": "512", "size": "8"}]}]}
        self.assertEqual(violations(clean), [])


if __name__ == "__main__":
    unittest.main()
