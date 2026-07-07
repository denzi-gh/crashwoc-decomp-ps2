"""Unit tests for promote.py --init profile selection.

The profile a new status manifest gets must be right by default: game and
engine TUs compile under the SN ProDG compiler (`default`), while the SCE /
newlib / libgcc / runtime half (all classified `sdk`) needs the Sony compiler
(`sce`). resolve_profile is a pure function over (override, category, known
profiles) -- no toolchain, no game files, no manifest I/O needed.
"""
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from promote import default_profile_for, resolve_profile

KNOWN = {"default", "sce"}


class TestDefaultProfileForCategory(unittest.TestCase):
    def test_game_uses_default(self):
        self.assertEqual(default_profile_for("game"), "default")

    def test_engine_uses_default(self):
        self.assertEqual(default_profile_for("engine"), "default")

    def test_sdk_uses_sce(self):
        # sdk covers SCE, newlib, libgcc and the EE runtime glue.
        self.assertEqual(default_profile_for("sdk"), "sce")


class TestResolveProfile(unittest.TestCase):
    def test_default_per_category(self):
        self.assertEqual(resolve_profile(None, "game", KNOWN), "default")
        self.assertEqual(resolve_profile(None, "engine", KNOWN), "default")
        self.assertEqual(resolve_profile(None, "sdk", KNOWN), "sce")

    def test_valid_override_wins(self):
        # An explicit, existing profile overrides the category default in
        # both directions (a game TU forced onto sce, an sdk TU onto default).
        self.assertEqual(resolve_profile("sce", "game", KNOWN), "sce")
        self.assertEqual(resolve_profile("default", "sdk", KNOWN), "default")

    def test_unknown_override_is_rejected(self):
        with self.assertRaises(SystemExit) as cm:
            resolve_profile("O9", "game", KNOWN)
        self.assertIn("unknown profile", str(cm.exception))

    def test_default_not_in_profiles_is_rejected(self):
        # If profiles.toml somehow lacked the derived default, --init must
        # fail loudly rather than write an invalid manifest.
        with self.assertRaises(SystemExit):
            resolve_profile(None, "sdk", {"default"})


class TestRealProfilesAreKnown(unittest.TestCase):
    def test_derived_defaults_exist_in_committed_profiles(self):
        # Guard against the committed profiles.toml drifting away from the
        # categories --init can derive.
        from status import load_profile_names
        known = load_profile_names(ROOT / "config" / "pal103")
        for category in ("game", "engine", "sdk"):
            self.assertIn(default_profile_for(category), known)


if __name__ == "__main__":
    unittest.main()
