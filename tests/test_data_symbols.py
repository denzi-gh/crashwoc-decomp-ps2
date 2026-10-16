"""Tests for the per-symbol data registry (config/pal103/data_symbols.toml).

The committed registry holds names, addresses and sizes only (no game bytes)
and is checked against its own invariants and the data map. The extraction
tests rebuild it from the user's ELF and are skipped without it.
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
from extract_data_symbols import with_extents  # noqa: E402

REGISTRY = ROOT / "config" / "pal103" / "data_symbols.toml"
ELF = ROOT / "orig" / "pal103" / "SLES_503.86"
SECTIONS = (".data", ".rodata", ".sdata", ".sbss", ".bss")


class TestCommittedRegistry(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        data = tomllib.loads(REGISTRY.read_text())
        cls.schema = data["schema"]
        cls.symbols = data["symbol"]
        spec = json.loads((ROOT / "config/pal103/sections.json").read_text())
        cls.sections = {s["name"]: (int(s["addr"], 0),
                                    int(s["addr"], 0) + int(s["size"], 0))
                        for s in spec["sections"]}

    def test_schema_and_census(self):
        self.assertEqual(self.schema, 1)
        self.assertEqual(len(self.symbols), 3750)
        scopes = Counter(s["scope"] for s in self.symbols)
        self.assertEqual(scopes["global"], 2549)
        self.assertEqual(scopes["static"] + scopes["function_static"], 1201)
        self.assertEqual(scopes["function_static"], 141)
        globals_ = [s for s in self.symbols if s["scope"] == "global"]
        statics = [s for s in self.symbols if s["scope"] != "global"]
        self.assertEqual(sum("size" in s for s in globals_), 2548)
        self.assertEqual(sum("size" in s for s in statics), 778)
        self.assertEqual(sum("type" in s for s in self.symbols), 130)

    def test_keys_unique_and_qualified(self):
        keys = [s["key"] for s in self.symbols]
        self.assertEqual(len(keys), len(set(keys)))
        for s in self.symbols:
            self.assertEqual(s["key"], f"unit-{s['unit']:04d}:{s['name']}")

    def test_sorted_by_address(self):
        order = [(s["address"], s["unit"], s["name"]) for s in self.symbols]
        self.assertEqual(order, sorted(order))

    def test_section_holds_the_address(self):
        for s in self.symbols:
            self.assertIn(s["section"], SECTIONS, s["key"])
            lo, hi = self.sections[s["section"]]
            self.assertTrue(lo <= s["address"] < hi, s["key"])
            self.assertLessEqual(s["address"] + s["extent"], hi, s["key"])

    def test_scope_rules(self):
        for s in self.symbols:
            m = re.fullmatch(r"(.+)\.(\d+)", s["name"])
            if s["scope"] == "function_static":
                self.assertTrue(m, s["key"])
                self.assertEqual(s["stem"], m.group(1))
            else:
                self.assertNotIn("stem", s)
            if s["scope"] == "static":
                self.assertIsNone(m, s["key"])

    def test_size_fields_consistent(self):
        for s in self.symbols:
            if "size" in s:
                self.assertGreater(s["size"], 0)
                self.assertIn(s["size_evidence"], ("symtab", "stabs"))
                self.assertLessEqual(s["size"], s["extent"], s["key"])
                self.assertIn("pad_zero", s)
            else:
                self.assertNotIn("size_evidence", s)
                self.assertNotIn("pad_zero", s)
            if "type" in s:
                self.assertIn("stab", s)

    def test_known_symbols(self):
        by_key = {s["key"]: s for s in self.symbols}
        one = by_key["unit-0162:one"]                 # e_fmod's `one:S13`
        self.assertEqual((one["section"], one["type"], one["size"]),
                         (".rodata", "double", 8))
        blanks = next(s for s in self.symbols
                      if s["scope"] == "function_static"
                      and s["stem"] == "blanks")      # vfprintf, `blanks:V`
        self.assertEqual((blanks["type"], blanks["size"]), ("char[16]", 16))
        errno = next(s for s in self.symbols if s["name"] == "errno")
        self.assertEqual((errno["storage"], errno["section"]), ("scBss", ".sbss"))

    def test_owned_named_ranges_cover_sized_symbols(self):
        ranges = tomllib.loads((ROOT / "config/pal103/data_map.toml")
                               .read_text())["range"]
        owned = [(r["start"], r["end"], r["owner"]) for r in ranges
                 if r["owner"] != "unassigned"]
        for s in self.symbols:
            if "size" not in s:
                continue
            owner = f"unit-{s['unit']:04d}"
            self.assertTrue(any(lo <= s["address"] and s["address"] + s["size"] <= hi
                                and o == owner for lo, hi, o in owned), s["key"])


class TestExtents(unittest.TestCase):
    def test_extent_and_padding(self):
        recs = [{"key": "a", "address": 0x10, "section": ".x", "size": 4},
                {"key": "b", "address": 0x18, "section": ".x"}]
        blob = {(0x14, 0x18): b"\0\0\0\0"}
        with_extents(recs, [0x10, 0x18], [(".x", 0x0, 0x20)],
                     lambda _s, lo, hi: blob.get((lo, hi), b"\1"))
        self.assertEqual((recs[0]["extent"], recs[0]["pad_zero"]), (8, True))
        self.assertEqual(recs[1]["extent"], 8)          # to the section end
        self.assertNotIn("pad_zero", recs[1])

    def test_overrun_raises(self):
        recs = [{"key": "a", "address": 0x10, "section": ".x", "size": 0x10}]
        with self.assertRaises(ValueError):
            with_extents(recs, [0x10, 0x18], [(".x", 0x0, 0x40)],
                         lambda _s, lo, hi: b"")


@unittest.skipUnless(ELF.is_file(), "needs the target ELF")
class TestExtraction(unittest.TestCase):
    def test_committed_registry_is_fresh(self):
        from extract_data_symbols import build
        text, _records = build(ROOT, "pal103")
        self.assertEqual(REGISTRY.read_text(), text)

    def test_stab_copies_are_not_statics(self):
        from extract_mdebug import MDebug, find_section
        data = ELF.read_bytes()
        md = MDebug(data, find_section(data, ".mdebug")[0])
        statics, stab_units = [], 0
        for u in md.units():
            statics += [(u["index"], a) for _n, a, _sc in md.local_data_symbols(u)]
            stab_units += any(True for _ in md.stabs(u))
        self.assertEqual(len(statics), 1201)
        self.assertEqual(len(set(statics)), 1201)
        self.assertEqual(stab_units, 85)


if __name__ == "__main__":
    unittest.main()
