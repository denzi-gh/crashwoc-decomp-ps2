"""Tests for the decomp-agent data path (tools/decomp_agent/data.py).

They read the committed registries only (data_symbols.toml, data_map.toml,
status manifests); byte previews need the ELF and are skipped without it.
"""
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from decomp_agent import classify, data as data_mod, schemas  # noqa: E402
from decomp_agent.project import DecompProject  # noqa: E402
from decomp_agent.registry import Registry  # noqa: E402

ELF = ROOT / "orig" / "pal103" / "SLES_503.86"


class TestDataResolution(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.project = DecompProject(root=ROOT, version="pal103")
        cls.reg = Registry(cls.project)

    def resolve(self, target):
        return data_mod.resolve_target(self.project, target, self.reg.resolve(target))

    def test_data_id_resolves(self):
        out = self.resolve("pal103:unit-0014:00613fb0:nuvec4_zero")
        self.assertEqual((out["status"], out["kind"], out["data"]["name"]),
                         ("resolved", "data", "nuvec4_zero"))

    def test_bare_name_resolves_when_no_function_has_it(self):
        out = self.resolve("nuvec4_zero")
        self.assertEqual(out["data"]["id"], "pal103:unit-0014:00613fb0:nuvec4_zero")

    def test_static_name_shared_by_units_is_ambiguous(self):
        out = self.resolve("one")              # fdlibm's `one` in many units
        self.assertEqual(out["status"], "ambiguous")
        self.assertGreater(len(out["candidates"]), 2)

    def test_function_targets_are_untouched(self):
        out = self.resolve("NuVec4Add")
        self.assertEqual(out["status"], "resolved")
        self.assertNotIn("kind", out)

    def test_is_data_target(self):
        did = "pal103:unit-0014:00613fb0:nuvec4_zero"
        self.assertTrue(data_mod.is_data_target(self.project, did, self.reg.resolve(did)))
        self.assertFalse(data_mod.is_data_target(
            self.project, "NuVec4Add", self.reg.resolve("NuVec4Add")))

    def test_candidates_shape(self):
        out = data_mod.list_data_candidates(self.project, limit=5)
        self.assertTrue(out["ok"])
        for row in out["candidates"]:
            self.assertIn(row["section"], (".rodata", ".data", ".sdata"))
        self.assertIsInstance(out["complete_only"], list)

    @unittest.skipUnless(ELF.is_file(), "needs the target ELF")
    def test_context(self):
        out = data_mod.data_context(self.project, "pal103:unit-0163:0062b530:bp")
        self.assertEqual(out["facts"]["section"], ".rodata")
        self.assertEqual(out["type_evidence"][0]["type"], "double[]")
        self.assertEqual(out["bytes"]["hex"], "000000000000f03f000000000000f83f")


class TestDataSchemas(unittest.TestCase):
    def test_new_blocker_kinds(self):
        for kind in ("compiler_private_partial_unit", "data_section_mismatch",
                     "bss_common"):
            self.assertIn(kind, schemas.BLOCKER_KINDS)

    def test_hybrid_messages_classify(self):
        cat = classify._hybrid_error_category
        self.assertEqual(cat("data_section_mismatch: x compiles into .sdata"),
                         "data_section_mismatch")
        self.assertEqual(cat("s: C defines uninitialised data (.bss); .sbss/.bss"
                             "/COMMON stay retail until data/bss"), "bss_common")
        self.assertEqual(cat("data section in compiler output not supported yet: "
                             "x (.data) -- a unit's C defines data only with a "
                             "[[data]] table"), "data_entry")
        for category in ("data_section_mismatch", "bss_common", "data_entry"):
            self.assertIn(category, schemas.MISMATCH_CATEGORIES)


if __name__ == "__main__":
    unittest.main()
