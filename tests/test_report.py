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

    def test_measure_whitelist_is_the_known_cli_schema(self):
        # Every measure the fixture uses must be a real whitelisted key.
        for key in self.public["measures"]:
            self.assertIn(key, MEASURE_KEYS)


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
