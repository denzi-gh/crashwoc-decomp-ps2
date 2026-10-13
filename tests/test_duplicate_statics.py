"""Duplicated statics: NAME__<VRAM> relabelling and lookup by name."""
import subprocess
import sys
import unittest
from pathlib import Path
from unittest import mock

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
import gen_hybrid
from declib import verify

FP_BIT = [(0x0026F748, "__pack_f", "matching"),
          (0x0026F8E8, "_fpadd_parts", "matching"),
          (0x0026FB20, "fpadd", "asm")]


class TestDuplicateStaticLabels(unittest.TestCase):
    def test_only_registry_duplicates_are_relabelled(self):
        self.assertEqual(gen_hybrid._duplicate_static_labels(FP_BIT, "pal103"),
                         {"_fpadd_parts": "_fpadd_parts__0026F8E8"})

    def test_relabel_rewrites_whole_symbols_only(self):
        rename = {"_fpadd_parts": "_fpadd_parts__0026F8E8"}
        lines = ["\t.ent\t_fpadd_parts", "_fpadd_parts:",
                 "\tjal\t_fpadd_parts", "\tjal\t_fpadd_parts_x",
                 "$_fpadd_parts:", "\t.end\t_fpadd_parts"]
        self.assertEqual(gen_hybrid._relabel(lines, rename), [
            "\t.ent\t_fpadd_parts__0026F8E8", "_fpadd_parts__0026F8E8:",
            "\tjal\t_fpadd_parts__0026F8E8", "\tjal\t_fpadd_parts_x",
            "$_fpadd_parts:", "\t.end\t_fpadd_parts__0026F8E8"])

    def test_relabel_without_duplicates_is_identity(self):
        lines = ["\tjal\tfpadd"]
        self.assertIs(gen_hybrid._relabel(lines, {}), lines)


class TestDefinedFunctionOffsets(unittest.TestCase):
    def test_duplicate_static_is_also_found_by_registry_name(self):
        nm_out = ("000001a0 t _fpadd_parts__0026F8E8\n"
                  "000003d8 T fpadd\n")
        done = subprocess.CompletedProcess([], 0, stdout=nm_out)
        with mock.patch.object(verify.subprocess, "run", return_value=done):
            offsets = verify.defined_function_offsets("nm", "x.o")
        self.assertEqual(offsets["_fpadd_parts"], 0x1A0)
        self.assertEqual(offsets["_fpadd_parts__0026F8E8"], 0x1A0)
        self.assertEqual(offsets["fpadd"], 0x3D8)


if __name__ == "__main__":
    unittest.main()
