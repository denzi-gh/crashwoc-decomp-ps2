"""tools/gen_source_skeletons.py: deterministic skeleton + manifest generation.

The registry-derived parts are checked against the real committed registries
(read-only). Writing / --check semantics are exercised on a synthetic plan in a
temp directory so the real source tree is never touched.
"""
import sys
import tempfile
import textwrap
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
import gen_source_skeletons as gss
import promote


class TestSkeletonContent(unittest.TestCase):
    def test_skeleton_c_is_comment_only(self):
        funcs = [(0x1cbce8, "ResetPlayer"), (0x1cc218, "ManageCreatures")]
        text = gss.skeleton_c("game/creature", funcs)
        self.assertEqual(text.splitlines()[0], "/*")
        self.assertIn(" * Unit: game/creature", text)
        self.assertIn(" *   0x001cbce8 ResetPlayer", text)
        # No C code: nothing that would emit a symbol or fake progress.
        for token in ("{", "}", ";", "void", "int "):
            self.assertNotIn(token, text)

    def test_plan_manifest_matches_promote_init(self):
        # The generated manifest for a real unit is byte-identical to what
        # promote.py --init would write (shared manifest_text).
        entry = next(e for e in gss.plan("pal103") if e[0] == "game/creature")
        _name, src_rel, _man_rel, _c, manifest_body = entry
        funcs = promote.unit_functions("pal103", 91)
        expected = promote.manifest_text("pal103", 91, "game/creature",
                                         src_rel, "default", funcs)
        self.assertEqual(manifest_body, expected)

    def test_plan_covers_every_text_unit(self):
        from declib.tu import load_tu_runs
        self.assertEqual(len(gss.plan("pal103")), len(load_tu_runs()))


ALL_ASM = textwrap.dedent("""\
    schema = 1
    unit = "pal103:unit-0001"
    source = "src/zz/a.c"
    profile = "default"
    complete = false

    [[function]]
    id = "pal103:unit-0001:00001000:Foo"
    state = "asm"
""")

WORKED_ON = ALL_ASM.replace('state = "asm"', 'state = "matching"')

SYNTH_PLAN = [
    ("zz/a", "src/zz/a.c", "config/pal103/status/zz/a.toml",
     "/*\n * Unit: zz/a\n */\n", ALL_ASM),
]


class TestGenerateAndCheck(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.root = Path(self.tmp.name)
        self._orig_root = gss.ROOT
        self._orig_plan = gss.plan
        gss.ROOT = self.root
        gss.plan = lambda version: [tuple(x) for x in SYNTH_PLAN]

    def tearDown(self):
        gss.ROOT = self._orig_root
        gss.plan = self._orig_plan
        self.tmp.cleanup()

    def test_generate_creates_then_check_passes(self):
        wrote_c, wrote_m = gss.generate("pal103")
        self.assertEqual((wrote_c, wrote_m), (1, 1))
        self.assertTrue((self.root / "src/zz/a.c").is_file())
        self.assertTrue(
            (self.root / "config/pal103/status/zz/a.toml").is_file())
        self.assertEqual(gss.check("pal103"), [])
        # Idempotent: a second generate writes nothing.
        self.assertEqual(gss.generate("pal103"), (0, 0))

    def test_check_flags_missing_files(self):
        problems = gss.check("pal103")
        self.assertTrue(any("missing source" in p for p in problems))
        self.assertTrue(any("missing manifest" in p for p in problems))

    def test_check_flags_stale_all_asm_manifest(self):
        gss.generate("pal103")
        man = self.root / "config/pal103/status/zz/a.toml"
        man.write_text(ALL_ASM.replace("00001000", "00009999"))
        problems = gss.check("pal103")
        self.assertTrue(any("stale manifest" in p for p in problems))

    def test_worked_on_manifest_is_exempt(self):
        gss.generate("pal103")
        # A manifest with a non-asm state diverges legitimately; not flagged.
        man = self.root / "config/pal103/status/zz/a.toml"
        man.write_text(WORKED_ON)
        self.assertEqual(gss.check("pal103"), [])

    def test_generate_never_overwrites(self):
        src = self.root / "src/zz/a.c"
        src.parent.mkdir(parents=True)
        src.write_text("// my real work\n")
        gss.generate("pal103")
        self.assertEqual(src.read_text(), "// my real work\n")


if __name__ == "__main__":
    unittest.main()
