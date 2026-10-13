"""Which hybrids verify_hybrid and verify_promoted (re)build. Toolchain mocked."""
import io
import sys
import tempfile
import unittest
from contextlib import redirect_stdout
from pathlib import Path
from unittest import mock

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
import verify_hybrid
import verify_promoted
from declib.toolchain import Reporter


def _manifest(states):
    funcs = "".join(
        f'\n[[function]]\nid = "pal103:unit-0099:{0x100000 + 0x10 * i:08x}:'
        f'Fn{i}"\nstate = "{s}"\n' for i, s in enumerate(states))
    return ('schema = 1\nunit = "pal103:unit-0099"\n'
            'source = "src/zz/unit.c"\nprofile = "default"\n'
            'complete = false\n' + funcs)


class TestVerifyHybridSelection(unittest.TestCase):
    def setUp(self):
        self._tmp = tempfile.TemporaryDirectory()
        self.dir = Path(self._tmp.name)

    def tearDown(self):
        self._tmp.cleanup()

    def _check(self, states):
        manifest = self.dir / "unit.toml"
        manifest.write_text(_manifest(states))
        reporter = Reporter()
        with mock.patch.object(verify_hybrid, "build_hybrid",
                               side_effect=verify_hybrid.HybridError("built")) \
                as build, \
                mock.patch.object(verify_hybrid, "unit_range",
                                  return_value=(0x100000, 0x100040)), \
                redirect_stdout(io.StringIO()):
            verify_hybrid.check_unit(reporter, manifest, "pal103", b"\0" * 0x40,
                                     0x100000, 0, 0x40, "nm")
        return build, reporter

    def test_all_asm_unit_is_skipped(self):
        build, reporter = self._check(["asm", "asm"])
        build.assert_not_called()
        self.assertFalse(reporter.failed)

    def test_equivalent_only_unit_is_skipped(self):
        build, _reporter = self._check(["asm", "equivalent"])
        build.assert_not_called()

    def test_unit_with_matching_function_is_built(self):
        build, reporter = self._check(["matching", "asm"])
        build.assert_called_once()
        self.assertTrue(reporter.failed)       # the mocked build "failed"


class TestVerifyPromotedImageGate(unittest.TestCase):
    def setUp(self):
        self._tmp = tempfile.TemporaryDirectory()
        self.root = Path(self._tmp.name)
        status = self.root / "config" / "pal103" / "status" / "zz"
        status.mkdir(parents=True)
        self.manifest = status / "unit.toml"
        self.manifest.write_text(_manifest(["matching", "asm"]))

    def tearDown(self):
        self._tmp.cleanup()

    def _run(self, rebuilt_by_manifest_pass):
        def fake_verify(*args):
            manifest, built = args[1], args[-1]
            if rebuilt_by_manifest_pass:
                built.add(manifest)

        patches = [
            mock.patch.object(verify_promoted, "ROOT", self.root),
            mock.patch.object(verify_promoted, "tool_path", return_value="nm"),
            mock.patch.object(verify_promoted, "check_version",
                              return_value=({}, {})),
            mock.patch.object(verify_promoted, "load_text_target",
                              return_value=(b"", 0, 0, 0)),
            mock.patch.object(verify_promoted, "function_sizes", return_value={}),
            mock.patch.object(verify_promoted, "verify_manifest",
                              side_effect=fake_verify),
            mock.patch.object(verify_promoted, "link_image",
                              return_value=(True, None)),
            mock.patch.object(verify_promoted, "profiles_fingerprint",
                              return_value="fp"),
            mock.patch.object(sys, "argv", ["verify_promoted.py"]),
        ]
        with mock.patch.object(verify_promoted, "build_hybrid") as build:
            for p in patches:
                p.start()
            try:
                with redirect_stdout(io.StringIO()):
                    rc = verify_promoted.main()
            finally:
                for p in reversed(patches):
                    p.stop()
        return rc, build

    def test_image_gate_reuses_rebuilt_hybrids(self):
        rc, build = self._run(rebuilt_by_manifest_pass=True)
        self.assertEqual(rc, 0)
        build.assert_not_called()

    def test_image_gate_builds_what_was_not_rebuilt(self):
        rc, build = self._run(rebuilt_by_manifest_pass=False)
        self.assertEqual(rc, 0)
        build.assert_called_once()
        self.assertEqual(build.call_args.args[0], self.manifest)


if __name__ == "__main__":
    unittest.main()
