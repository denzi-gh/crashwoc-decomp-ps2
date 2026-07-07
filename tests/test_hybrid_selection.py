"""The per-unit hybrid-object selection in gen_ninja / gen_objdiff.

An all-`asm` unit (e.g. a fresh skeleton) must NOT get matching / equivalent /
report-current hybrid edges -- its bytes come from the expected retail object
and objdiff scores its plain `current` object. A unit only earns each hybrid
when a function actually reaches the corresponding state. These tests drive the
decision by substituting a synthetic source list, so no game files, toolchain,
or new committed manifests are needed.
"""
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
import gen_ninja
import gen_objdiff

# (src_rel, manifest_rel, profile, states)
SYNTH_SOURCES = [
    ("src/game/creature.c", "config/pal103/status/game/creature.toml",
     "default", frozenset({"asm", "matching"})),
    ("src/zz/allasm.c", "config/pal103/status/zz/allasm.toml",
     "default", frozenset({"asm"})),
    ("src/zz/equiv.c", "config/pal103/status/zz/equiv.toml",
     "default", frozenset({"asm", "equivalent"})),
]


class TestHybridEdgeSelection(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls._orig = gen_ninja._sources
        gen_ninja._sources = lambda version: list(SYNTH_SOURCES)
        cls.tmp = tempfile.TemporaryDirectory()
        path = Path(cls.tmp.name) / "build.ninja"
        gen_ninja.emit_ninja("pal103", path)
        cls.text = path.read_text()

    @classmethod
    def tearDownClass(cls):
        gen_ninja._sources = cls._orig
        cls.tmp.cleanup()

    def has_edge(self, out):
        return f"build {out}:" in self.text

    def test_matching_hybrid_only_when_a_function_matches(self):
        self.assertTrue(self.has_edge("build/pal103/matching/game/creature.o"))
        self.assertFalse(self.has_edge("build/pal103/matching/zz/allasm.o"))
        self.assertFalse(self.has_edge("build/pal103/matching/zz/equiv.o"))

    def test_equivalent_hybrid_when_matching_or_equivalent(self):
        self.assertTrue(
            self.has_edge("build/pal103/equivalent/game/creature.o"))
        self.assertTrue(self.has_edge("build/pal103/equivalent/zz/equiv.o"))
        self.assertFalse(self.has_edge("build/pal103/equivalent/zz/allasm.o"))

    def test_report_current_only_when_a_function_matches(self):
        self.assertTrue(
            self.has_edge("build/pal103/report-current/game/creature.o"))
        self.assertFalse(
            self.has_edge("build/pal103/report-current/zz/allasm.o"))
        self.assertFalse(
            self.has_edge("build/pal103/report-current/zz/equiv.o"))

    def test_every_unit_still_gets_a_current_object(self):
        for rel in ("game/creature", "zz/allasm", "zz/equiv"):
            self.assertTrue(self.has_edge(f"build/pal103/current/{rel}.o"))

    def test_report_edge_uses_current_base_for_non_matching_units(self):
        (report,) = [l for l in self.text.splitlines()
                     if l.startswith("build build/pal103/report.json:")]
        # No matching function -> objdiff base is the plain current object.
        self.assertIn("build/pal103/current/zz/allasm.o", report)
        self.assertIn("build/pal103/current/zz/equiv.o", report)
        # A matching unit -> the normalized report-current object.
        self.assertIn("build/pal103/report-current/game/creature.o", report)
        self.assertNotIn("build/pal103/report-current/zz/allasm.o", report)


class TestObjdiffBaseSelection(unittest.TestCase):
    """gen_objdiff must point an all-`asm` unit at its `current` object."""

    def _emit_with(self, manifest_info):
        orig = gen_objdiff._manifest_info
        gen_objdiff._manifest_info = lambda version: manifest_info
        try:
            tmp = Path(tempfile.mktemp(suffix=".json"))
            gen_objdiff.emit_objdiff("pal103", tmp)
            import json
            cfg = json.loads(tmp.read_text())
        finally:
            gen_objdiff._manifest_info = orig
        return {u["name"]: u for u in cfg["units"]}

    def test_all_asm_unit_scores_current_object(self):
        # creature.c exists in the tree; flag it as having no matching function.
        units = self._emit_with({"src/game/creature.c": (False, False)})
        u = units["game/creature"]
        self.assertEqual(u["base_path"],
                         "build/pal103/current/game/creature.o")
        self.assertEqual(u["metadata"]["complete"], False)

    def test_matching_unit_scores_report_current_object(self):
        units = self._emit_with({"src/game/creature.c": (False, True)})
        u = units["game/creature"]
        self.assertEqual(u["base_path"],
                         "build/pal103/report-current/game/creature.o")


import link_image


def _manifest(unit_idx, source, states):
    body = ["schema = 1", f'unit = "pal103:unit-{unit_idx:04d}"',
            f'source = "{source}"', 'profile = "default"',
            "complete = false", ""]
    for i, st in enumerate(states):
        body += ["[[function]]",
                 f'id = "pal103:unit-{unit_idx:04d}:{0x1000 + i:08x}:f{i}"',
                 f'state = "{st}"', ""]
    return "\n".join(body)


class TestLinkImageSelection(unittest.TestCase):
    """link_image must not reference a hybrid the build no longer emits."""

    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        root = Path(self.tmp.name)
        status = root / "config" / "pal103" / "status"
        (status / "a").mkdir(parents=True)
        (status / "a" / "matches.toml").write_text(
            _manifest(1, "src/a/matches.c", ["asm", "matching"]))
        (status / "a" / "equiv.toml").write_text(
            _manifest(2, "src/a/equiv.c", ["asm", "equivalent"]))
        (status / "a" / "allasm.toml").write_text(
            _manifest(3, "src/a/allasm.c", ["asm", "asm"]))
        self._orig = link_image.ROOT
        link_image.ROOT = root

    def tearDown(self):
        link_image.ROOT = self._orig
        self.tmp.cleanup()

    def test_matching_set_excludes_equiv_and_allasm(self):
        self.assertEqual(set(link_image.manifest_units("pal103", "matching")),
                         {1})

    def test_equivalent_set_includes_matching_and_equiv_not_allasm(self):
        self.assertEqual(
            set(link_image.manifest_units("pal103", "equivalent")), {1, 2})


if __name__ == "__main__":
    unittest.main()
