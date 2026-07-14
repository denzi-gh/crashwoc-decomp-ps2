"""Unit tests for tools/xref_gc.py.

Exercises the GC C definition scanner (ANSI, K&R, pointer returns, multi-line
signatures, declarations rejected) and the PS2<->GC matching / confidence logic
on a synthetic config + GC tree -- no game data, no real GC checkout.
"""
import sys
import tempfile
import textwrap
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
import xref_gc


class TestScanCFile(unittest.TestCase):
    def names(self, text):
        return [d[0] for d in xref_gc.scan_c_file(text)]

    def test_ansi_brace_on_signature_line(self):
        text = "void InitChase(CHASE *chase) {\n    chase->time = 0;\n}\n"
        defs = xref_gc.scan_c_file(text)
        self.assertEqual(defs, [("InitChase", 1, 3)])

    def test_pointer_return_type(self):
        text = "struct nugspline_s *NuSplineFind(struct nugscn_s *s) {\n}\n"
        self.assertEqual(self.names(text), ["NuSplineFind"])

    def test_declaration_is_not_a_definition(self):
        text = "s32 qrand(void);\nint sprintf(char *s, const char *f, ...);\n"
        self.assertEqual(self.names(text), [])

    def test_multiline_signature(self):
        text = (
            "void EvalModelAnim(struct CharacterModel *model,\n"
            "                   struct anim_s *anim,\n"
            "                   struct numtx_s *m) {\n"
            "    return;\n"
            "}\n"
        )
        self.assertEqual(self.names(text), ["EvalModelAnim"])

    def test_knr_definition(self):
        text = (
            "int foo(a, b)\n"
            "int a;\n"
            "char *b;\n"
            "{\n"
            "    return a;\n"
            "}\n"
        )
        defs = xref_gc.scan_c_file(text)
        self.assertEqual(defs, [("foo", 1, 6)])

    def test_control_keywords_skipped(self):
        # A bare `if (...) {` at column 0 is never a function def.
        text = "if (x) {\n}\nstruct s {\n    int a;\n};\n"
        self.assertEqual(self.names(text), [])

    def test_two_functions(self):
        text = (
            "void a(void) {\n"
            "}\n"
            "\n"
            "int b(int x) {\n"
            "    return x;\n"
            "}\n"
        )
        self.assertEqual(self.names(text), ["a", "b"])


class TestExpectedUnit(unittest.TestCase):
    def test_dir_map(self):
        self.assertEqual(xref_gc.expected_unit("gamecode", "chase"),
                         "game/chase")
        self.assertEqual(xref_gc.expected_unit("nu3dx", "nuanim"),
                         "nu3d/nuanim")

    def test_identity_dir(self):
        self.assertEqual(xref_gc.expected_unit("nucore", "nulist"),
                         "nucore/nulist")

    def test_file_relocation_overrides(self):
        self.assertEqual(xref_gc.expected_unit("nu3dx", "nuglass"),
                         "game/glass")
        self.assertEqual(xref_gc.expected_unit("nusound", "sfx"),
                         "game/sfx")


class TestBuildXref(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        root = Path(self.tmp.name)
        self.config = root / "config" / "testv"
        (self.config / "status" / "game").mkdir(parents=True)
        (self.config / "status" / "game" / "chase.toml").write_text(
            textwrap.dedent("""\
                schema = 1
                unit = "testv:unit-0118"
                source = "src/game/chase.c"
                profile = "default"
                complete = false

                [[function]]
                id = "testv:unit-0118:00259000:InitChase"
                state = "asm"

                [[function]]
                id = "testv:unit-0118:00259100:OnlyOnPs2"
                state = "matching"
                """))
        self.gc = root / "gc"
        (self.gc / "src" / "gamecode").mkdir(parents=True)
        (self.gc / "src" / "nucore").mkdir(parents=True)
        (self.gc / "src" / "gamecode" / "chase.c").write_text(
            "void InitChase(CHASE *c) {\n}\n")
        # Same name in an unrelated unit -> name-only when no unit match.
        (self.gc / "src" / "nucore" / "misc.c").write_text(
            "void OnlyOnGcElsewhere(void) {\n}\n")

    def tearDown(self):
        self.tmp.cleanup()

    def test_exact_match(self):
        funcs, rows = xref_gc.build_xref(self.config, self.gc)
        by_name = {r["id"].split(":")[-1]: r for r in rows}
        self.assertIn("InitChase", by_name)
        self.assertEqual(by_name["InitChase"]["confidence"], "exact")
        self.assertEqual(by_name["InitChase"]["gc_file"],
                         "src/gamecode/chase.c")
        self.assertEqual(by_name["InitChase"]["state"], "asm")

    def test_unreferenced_function_absent(self):
        _funcs, rows = xref_gc.build_xref(self.config, self.gc)
        ids = {r["id"] for r in rows}
        self.assertNotIn("testv:unit-0118:00259100:OnlyOnPs2", ids)

    def test_deterministic(self):
        _f1, r1 = xref_gc.build_xref(self.config, self.gc)
        _f2, r2 = xref_gc.build_xref(self.config, self.gc)
        self.assertEqual(r1, r2)


if __name__ == "__main__":
    unittest.main()
