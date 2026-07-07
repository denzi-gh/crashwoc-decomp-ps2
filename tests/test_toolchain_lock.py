"""Unit tests for toolchain-lock fingerprint coverage.

These guard the invariant behind `fingerprint_compiler.py --all` and the CI:
every compiler and executable tool the build actually uses must be pinned
file-by-file in toolchain.lock.json, so a restored cache is never trusted
unverified. Pure reads of the committed lock and profiles -- no toolchain,
no game files.
"""
import json
import sys
import tomllib
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from fingerprint_compiler import verifiable_components

LOCK = json.loads((ROOT / "toolchain.lock.json").read_text())
PROFILES = tomllib.loads(
    (ROOT / "config" / "pal103" / "profiles.toml").read_text())

# Executable tools the matching build runs that are not a profile's compiler
# but must still be verified: the PE runner, the report differ, the binutils.
NON_COMPILER_TOOLS = ("wibo", "objdiff", "ps2-binutils")


class TestVerifiableComponents(unittest.TestCase):
    def test_only_components_with_install_dir(self):
        names = set(verifiable_components(LOCK))
        for name, comp in LOCK["components"].items():
            self.assertEqual(name in names, "install_dir" in comp,
                             f"{name}: verifiable set must match install_dir")

    def test_covers_every_shipped_binary(self):
        names = set(verifiable_components(LOCK))
        self.assertLessEqual(
            {"ee-gcc-tt", "wibo", "ee-gcc", "ps2-binutils", "objdiff"}, names)

    def test_analysis_deps_are_not_fingerprinted(self):
        # splat and python are version-gated by `configure.py --strict`, not
        # by file fingerprints, so they must stay out of the verify set.
        names = set(verifiable_components(LOCK))
        self.assertNotIn("splat", names)
        self.assertNotIn("python", names)


class TestProfileCompilersAreVerified(unittest.TestCase):
    def _required_components(self):
        compilers = {spec["compiler"]
                     for spec in PROFILES.get("profile", {}).values()}
        return compilers | set(NON_COMPILER_TOOLS)

    def test_every_required_component_is_in_the_lock(self):
        for name in self._required_components():
            self.assertIn(name, LOCK["components"],
                          f"{name} referenced but absent from the lock")

    def test_every_required_component_is_verifiable(self):
        verifiable = set(verifiable_components(LOCK))
        for name in self._required_components():
            self.assertIn(
                name, verifiable,
                f"{name} is used by the build but has no install_dir to "
                f"fingerprint; `--all` would silently skip it")

    def test_every_required_component_has_recorded_fingerprints(self):
        for name in self._required_components():
            comp = LOCK["components"][name]
            self.assertTrue(
                comp.get("fingerprints"),
                f"{name} has an empty fingerprint manifest; an unverified "
                f"install could pass. Run fingerprint_compiler.py --record.")


if __name__ == "__main__":
    unittest.main()
