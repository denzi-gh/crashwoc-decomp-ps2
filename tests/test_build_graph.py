"""Unit tests for the ninja build-graph generator and the host dispatcher.

gen_ninja reads only committed state (the mdebug registries, src/, status
manifests) so it can be exercised against the real repo config on any host --
no game-derived files, no toolchain. dispatch path translation is a pure
function.
"""
import json
import re
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
import dispatch
from gen_ninja import _esc, emit_ninja
from gen_objdiff import CATEGORIES, classify, emit_objdiff
from link_image import select_text_objects
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
        for name in ("expected", "current", "matching", "fallback",
                     "verify-loaded", "image-equivalent"):
            self.assertIn(f"build {name}: phony", self.text)
        self.assertIn("default expected current matching", self.lines)

    def test_image_edges(self):
        image_edges = self.edges("link_image")
        self.assertEqual(
            {e.split()[1].rstrip(":") for e in image_edges},
            {"build/pal103/image/matching.bin",
             "build/pal103/image/equivalent.bin"})
        # The matching image must depend on every .text object it links.
        (matching,) = [e for e in image_edges if "/matching.bin" in e]
        n_units = len(load_tu_runs())
        self.assertGreaterEqual(matching.count(".o"), n_units)

    def test_paths_are_relative_posix(self):
        self.assertNotIn("\\", self.text)
        for line in self.lines:
            self.assertIsNone(re.search(r"\b[A-Za-z]\$?:[/\\]", line),
                              f"absolute path in: {line}")

    def test_escaping(self):
        self.assertEqual(_esc("a b:c$d"), "a$ b$:c$$d")


class TestClassify(unittest.TestCase):
    CASES = [
        (r"..\nu2crash.ps2\nucore\nulist.c", "engine", "nucore/nulist"),
        ("../nu2crash.ps2/nu3d/nugsys.c", "engine", "nu3d/nugsys"),
        (r".\main.c", "game", "game/main"),
        ("vu/vu.c", "game", "game/vu"),
        (r"C:\DOCUME~1\andy\LOCALS~1\Temp\ccAlaaaa.i", "game",
         "game/ccAlaaaa"),
        ("/usr/local/sce/ee/lib/crt0.s", "sdk", "sdk/sce/crt0"),
        ("../../../../../src/newlib/libc/string/strcat.c", "sdk",
         "sdk/newlib/libc/string/strcat"),
        ("../../src/gcc/libgcc2.c", "sdk", "sdk/gcc/libgcc2"),
        ("dp-bit.c", "sdk", "sdk/gcc/dp-bit"),
        ("graphdev.c", "sdk", "sdk/sce/graphdev"),
        ("../include/syscall.h", "sdk", "sdk/sce/syscall"),
    ]

    def test_representative_paths(self):
        for path, category, name in self.CASES:
            self.assertEqual(classify(path), (category, name), path)


class TestEmitObjdiff(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.tmp = tempfile.TemporaryDirectory()
        path = Path(cls.tmp.name) / "objdiff.json"
        emit_objdiff("pal103", path)
        cls.cfg = json.loads(path.read_text())

    @classmethod
    def tearDownClass(cls):
        cls.tmp.cleanup()

    def test_every_text_unit_listed_once(self):
        names = [u["name"] for u in self.cfg["units"]]
        self.assertEqual(len(names), len(load_tu_runs()))
        self.assertEqual(len(names), len(set(names)))

    def test_target_always_base_only_with_source(self):
        for u in self.cfg["units"]:
            self.assertTrue(u["target_path"].startswith("expected/pal103/"))
            src = ROOT / u["metadata"]["source_path"]
            if src.is_file():
                self.assertEqual(u["base_path"],
                                 f"build/pal103/current/{u['name']}.o")
            else:
                self.assertNotIn("base_path", u)

    def test_units_use_known_categories(self):
        ids = {c["id"] for c in CATEGORIES}
        self.assertEqual(ids, {c["id"]
                               for c in self.cfg["progress_categories"]})
        for u in self.cfg["units"]:
            cats = u["metadata"]["progress_categories"]
            self.assertEqual(len(cats), 1)
            self.assertIn(cats[0], ids)

    def test_build_command_is_the_dispatcher(self):
        self.assertEqual(self.cfg["custom_make"], "python")
        self.assertEqual(self.cfg["custom_args"],
                         ["tools/dispatch.py", "ninja"])
        self.assertTrue(self.cfg["build_target"])
        self.assertIn("src/**/*.c", self.cfg["watch_patterns"])

    def test_paths_are_relative_posix(self):
        text = json.dumps(self.cfg)
        self.assertNotIn("\\", text)


class TestSelectTextObjects(unittest.TestCase):
    def test_hybrid_substitution_in_address_order(self):
        objs = select_text_objects("pal103", "matching")
        self.assertEqual(len(objs), len(load_tu_runs()))
        self.assertEqual([u for u, _p in objs],
                         [u for u, _a in load_tu_runs()])
        by_unit = dict(objs)
        # Unit 7 has a status manifest: its hybrid object replaces the
        # expected object; its neighbours stay expected.
        self.assertTrue(str(by_unit[7]).replace("\\", "/").endswith(
            "build/pal103/matching/nucore/nulist.o"))
        self.assertTrue(str(by_unit[0]).replace("\\", "/").endswith(
            "expected/pal103/000_crt0.s.o"))
        self.assertTrue(str(by_unit[6]).replace("\\", "/").endswith(".o"))
        hybrids = [p for _u, p in objs if "expected" not in str(p)]
        self.assertEqual(len(hybrids), 1)


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
