"""Unit tests for the ninja build-graph generator and the host dispatcher.

gen_ninja reads only committed state (the mdebug registries, src/, status
manifests) so it can be exercised against the real repo config on any host --
no game-derived files, no toolchain. dispatch path translation is a pure
function.
"""
import re
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
import dispatch
from gen_ninja import _esc, emit_ninja
from declib.tu import load_tu_runs


class TestEmitNinja(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.tmp = tempfile.TemporaryDirectory()
        cls.path = Path(cls.tmp.name) / "build.ninja"
        emit_ninja("pal103", cls.path)
        cls.text = cls.path.read_text()
        cls.lines = cls.text.splitlines()

    @classmethod
    def tearDownClass(cls):
        cls.tmp.cleanup()

    def edges(self, rule):
        return [l for l in self.lines if re.match(rf"build .*: {rule} ", l)
                or re.match(rf"build .*: {rule}$", l)]

    def test_one_expected_object_per_text_tu(self):
        n_units = len(load_tu_runs())
        as_edges = self.edges("as_expected")
        self.assertEqual(len(as_edges), n_units)
        for edge in as_edges:
            self.assertTrue(edge.startswith("build expected/pal103/"))

    def test_splitter_edge_lists_every_tu_output(self):
        (edge,) = self.edges("expected_s")
        outs = edge.split(": expected_s")[0].removeprefix("build ").split()
        self.assertEqual(len(outs), len(load_tu_runs()))
        self.assertTrue(all(o.startswith("build/pal103/expected_s/")
                            for o in outs))

    def test_one_current_object_per_source(self):
        srcs = sorted((ROOT / "src").rglob("*.c"))
        cc_edges = self.edges("cc")
        self.assertEqual(len(cc_edges), len(srcs))

    def test_hybrid_edges_per_manifest_and_set(self):
        manifests = sorted(
            (ROOT / "config" / "pal103" / "status").rglob("*.toml"))
        hybrid_edges = self.edges("hybrid")
        self.assertEqual(len(hybrid_edges), 2 * len(manifests))
        sets = {re.search(r"build build/pal103/(\w+)/", e).group(1)
                for e in hybrid_edges}
        self.assertEqual(sets, {"matching", "equivalent"})

    def test_phony_targets_and_default(self):
        for name in ("expected", "current", "matching", "fallback"):
            self.assertIn(f"build {name}: phony", self.text)
        self.assertIn("default expected current matching", self.lines)

    def test_paths_are_relative_posix(self):
        self.assertNotIn("\\", self.text)
        for line in self.lines:
            self.assertIsNone(re.search(r"\b[A-Za-z]\$?:[/\\]", line),
                              f"absolute path in: {line}")

    def test_escaping(self):
        self.assertEqual(_esc("a b:c$d"), "a$ b$:c$$d")


class TestDispatchTranslation(unittest.TestCase):
    ROOT = Path(r"C:\repo\crashwoc")

    def tr(self, arg):
        return dispatch.translate_arg(arg, root=self.ROOT)

    def test_absolute_inside_repo(self):
        self.assertEqual(self.tr(r"C:\repo\crashwoc\src\nucore\nulist.c"),
                         "/work/src/nucore/nulist.c")

    def test_absolute_forward_slashes(self):
        self.assertEqual(self.tr("C:/repo/crashwoc/build.ninja"),
                         "/work/build.ninja")

    def test_absolute_outside_repo_rejected(self):
        with self.assertRaises(dispatch.DispatchError):
            self.tr(r"C:\elsewhere\thing.o")

    def test_relative_backslashes(self):
        self.assertEqual(self.tr(r"build\pal103\current\nucore\nulist.o"),
                         "build/pal103/current/nucore/nulist.o")

    def test_plain_args_untouched(self):
        for arg in ("ninja", "expected", "-j8", "--version",
                    "build/pal103/current/nucore/nulist.o"):
            self.assertEqual(self.tr(arg), arg)


if __name__ == "__main__":
    unittest.main()
