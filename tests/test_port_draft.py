"""Unit tests for tools/port_draft.py.

Exercises the hostile-layout neutralisation (sizeof, memset-family size arg),
identifier renames, target resolution (id vs bare name vs ambiguous) and the
end-to-end draft emission on a synthetic config + GC tree -- no game data.
"""
import sys
import tempfile
import textwrap
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
import port_draft


class TestReplaceSizeof(unittest.TestCase):
    def test_simple(self):
        self.assertEqual(
            port_draft.replace_sizeof("x = sizeof(Foo);"),
            "x = /* TODO(ps2-layout) */;")

    def test_nested_parens(self):
        self.assertEqual(
            port_draft.replace_sizeof("n = sizeof(a[b(c)]) / 2;"),
            "n = /* TODO(ps2-layout) */ / 2;")

    def test_multiple(self):
        out = port_draft.replace_sizeof("sizeof(A) + sizeof(B)")
        self.assertEqual(out, "/* TODO(ps2-layout) */ + /* TODO(ps2-layout) */")

    def test_word_boundary(self):
        # `sizeofx` is not sizeof.
        self.assertEqual(port_draft.replace_sizeof("sizeofx(y)"), "sizeofx(y)")


class TestReplaceMemsetSize(unittest.TestCase):
    def test_memset(self):
        self.assertEqual(
            port_draft.replace_memset_size("memset(&norm, 0, 0xc);"),
            "memset(&norm, 0, /* TODO(ps2-layout) */);")

    def test_bzero(self):
        self.assertEqual(
            port_draft.replace_memset_size("bzero(p, 128);"),
            "bzero(p, /* TODO(ps2-layout) */);")

    def test_nested_call_in_size(self):
        out = port_draft.replace_memset_size("memcpy(a, b, count(x, y));")
        self.assertEqual(out, "memcpy(a, b, /* TODO(ps2-layout) */);")

    def test_non_memset_untouched(self):
        self.assertEqual(
            port_draft.replace_memset_size("foo(a, b, 3);"),
            "foo(a, b, 3);")


class TestApplyRenames(unittest.TestCase):
    def test_word_boundary_rename(self):
        out = port_draft.apply_renames("GcName + GcNameX", {"GcName": "Ps2"})
        self.assertEqual(out, "Ps2 + GcNameX")


class TestResolveAndDraft(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        root = Path(self.tmp.name)
        self.config = root / "config" / "pal103"
        self.config.mkdir(parents=True)
        (self.config / "gc_xref.toml").write_text(textwrap.dedent("""\
            [[xref]]
            id = 'pal103:unit-0118:00259000:InitChase'
            unit = 'game/chase'
            addr = 0x00259000
            state = 'asm'
            gc_file = 'src/gamecode/chase.c'
            gc_line_start = 1
            gc_line_end = 3
            confidence = 'exact'

            [[xref]]
            id = 'pal103:unit-0200:00300000:Dup'
            unit = 'game/a'
            addr = 0x00300000
            state = 'asm'
            gc_file = 'src/gamecode/a.c'
            gc_line_start = 1
            gc_line_end = 2
            confidence = 'name'

            [[xref]]
            id = 'pal103:unit-0201:00300100:Dup'
            unit = 'game/b'
            addr = 0x00300100
            state = 'asm'
            gc_file = 'src/gamecode/b.c'
            gc_line_start = 1
            gc_line_end = 2
            confidence = 'name'
            """))
        self.gc = root / "gc"
        (self.gc / "src" / "gamecode").mkdir(parents=True)
        (self.gc / "src" / "gamecode" / "chase.c").write_text(
            "void InitChase(CHASE *c) {\n    memset(c, 0, sizeof(CHASE));\n}\n")

    def tearDown(self):
        self.tmp.cleanup()

    def test_resolve_by_id(self):
        xref = port_draft.load_xref(self.config)
        rows = port_draft.resolve(xref, "pal103:unit-0118:00259000:InitChase")
        self.assertEqual(len(rows), 1)

    def test_resolve_by_name(self):
        xref = port_draft.load_xref(self.config)
        rows = port_draft.resolve(xref, "InitChase")
        self.assertEqual(len(rows), 1)

    def test_resolve_ambiguous(self):
        xref = port_draft.load_xref(self.config)
        rows = port_draft.resolve(xref, "Dup")
        self.assertEqual(len(rows), 2)

    def test_make_draft_neutralises(self):
        xref = port_draft.load_xref(self.config)
        row = port_draft.resolve(xref, "InitChase")[0]
        fn, draft = port_draft.make_draft(row, self.gc)
        self.assertEqual(fn, "InitChase")
        # both the sizeof and the memset size were neutralised
        self.assertIn("/* TODO(ps2-layout) */", draft)
        self.assertNotIn("sizeof(CHASE)", draft)
        # header cites the source location
        self.assertIn("src/gamecode/chase.c:1-3", draft)


if __name__ == "__main__":
    unittest.main()
