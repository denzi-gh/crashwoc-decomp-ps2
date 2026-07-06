"""Unit tests for tools/status.py on a synthetic repository tree.

Builds a miniature config/<version>/ + src/ layout in a temp directory --
no game data -- and checks that a well-formed manifest passes and that every
seeded inconsistency is rejected.
"""
import sys
import tempfile
import textwrap
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
import status

VERSION = "testv"

UNITS_TOML = textwrap.dedent("""\
    [[unit]]
    index = 7
    name = '..\\eng\\nucore\\nulist.c'
    """)

FUNCTIONS_TOML = textwrap.dedent("""\
    [[function]]
    index = 0
    name = 'NuListGetHead'
    address = 0x00105a50
    unit = 7

    [[function]]
    index = 1
    name = 'NuListGetTail'
    address = 0x00105a58
    unit = 7
    """)

PROFILES_TOML = textwrap.dedent("""\
    schema = 1
    [profile.default]
    flags = ["-O2"]
    """)

GOOD_MANIFEST = textwrap.dedent("""\
    schema = 1
    unit = "testv:unit-0007"
    source = "src/nucore/nulist.c"
    profile = "default"
    complete = false

    [[function]]
    id = "testv:unit-0007:00105a50:NuListGetHead"
    state = "matching"

    [[function]]
    id = "testv:unit-0007:00105a58:NuListGetTail"
    state = "asm"
    """)


class StatusTree:
    """A synthetic repo tree with one unit, two functions, one manifest."""

    def __init__(self, root):
        self.root = Path(root)
        config = self.root / "config" / VERSION
        (config / "status" / "nucore").mkdir(parents=True)
        (config / "units.toml").write_text(UNITS_TOML)
        (config / "functions.toml").write_text(FUNCTIONS_TOML)
        (config / "profiles.toml").write_text(PROFILES_TOML)
        self.manifest = config / "status" / "nucore" / "nulist.toml"
        self.manifest.write_text(GOOD_MANIFEST)
        (self.root / "src" / "nucore").mkdir(parents=True)
        (self.root / "src" / "nucore" / "nulist.c").write_text("/* c */\n")

    def problems(self):
        return status.check_manifest(self.manifest, VERSION, self.root)


class TestStatusValidator(unittest.TestCase):
    def setUp(self):
        self._tmp = tempfile.TemporaryDirectory()
        self.tree = StatusTree(self._tmp.name)

    def tearDown(self):
        self._tmp.cleanup()

    def rewrite(self, old, new):
        self.tree.manifest.write_text(
            self.tree.manifest.read_text().replace(old, new))

    def assert_rejected(self, fragment):
        problems = self.tree.problems()
        self.assertTrue(problems, "expected a validation failure")
        self.assertTrue(any(fragment in p for p in problems),
                        f"no problem mentions {fragment!r}: {problems}")

    def test_good_manifest_passes(self):
        self.assertEqual(self.tree.problems(), [])

    def test_version_check_passes_and_counts(self):
        results, counts = status.check_version(VERSION, self.tree.root)
        self.assertEqual([p for probs in results.values() for p in probs], [])
        self.assertEqual(counts["matching"], 1)
        self.assertEqual(counts["asm"], 1)

    def test_rejects_wrong_function_name(self):
        self.rewrite("00105a50:NuListGetHead", "00105a50:NuListGetHeadX")
        self.assert_rejected("not 'NuListGetHeadX'")

    def test_rejects_unknown_address(self):
        self.rewrite("00105a50", "00105a54")
        self.assert_rejected("no function at 0x00105a54")

    def test_rejects_missing_function(self):
        text = self.tree.manifest.read_text()
        head = text.split("[[function]]")[0]
        self.tree.manifest.write_text(
            head + "[[function]]" + text.split("[[function]]")[1])
        self.assert_rejected("not listed")

    def test_rejects_duplicate_entry(self):
        self.rewrite('id = "testv:unit-0007:00105a58:NuListGetTail"',
                     'id = "testv:unit-0007:00105a50:NuListGetHead"')
        self.assert_rejected("duplicate entry")

    def test_rejects_bad_state(self):
        self.rewrite('state = "asm"', 'state = "nearly"')
        self.assert_rejected("bad state")

    def test_rejects_incomplete_complete(self):
        self.rewrite("complete = false", "complete = true")
        self.assert_rejected("complete = true but")

    def test_rejects_unknown_profile(self):
        self.rewrite('profile = "default"', 'profile = "O9"')
        self.assert_rejected("unknown profile")

    def test_rejects_wrong_source_path(self):
        self.rewrite('source = "src/nucore/nulist.c"',
                     'source = "src/nucore/other.c"')
        self.assert_rejected("does not match manifest path")

    def test_rejects_missing_source_file(self):
        (self.tree.root / "src" / "nucore" / "nulist.c").unlink()
        self.assert_rejected("does not exist")

    def test_rejects_unknown_unit(self):
        self.rewrite("unit-0007", "unit-0008")
        self.assert_rejected("unit 8 not in units.toml")

    def test_rejects_unit_source_mismatch(self):
        # units.toml says unit 7 is nulist.c; point source (and the manifest
        # path convention) at a different basename.
        config = self.tree.root / "config" / VERSION
        other = config / "status" / "nucore" / "other.toml"
        other.write_text(GOOD_MANIFEST.replace(
            "src/nucore/nulist.c", "src/nucore/other.c"))
        (self.tree.root / "src" / "nucore" / "other.c").write_text("/* c */\n")
        problems = status.check_manifest(other, VERSION, self.tree.root)
        self.assertTrue(any("'nulist.c' in units.toml but source is 'other.c'"
                            in p for p in problems), problems)

    def test_rejects_unknown_key(self):
        self.tree.manifest.write_text(
            self.tree.manifest.read_text() + "\nowner = \"me\"\n")
        self.assert_rejected("unknown keys")

    def test_coverage_rule_flags_unmanifested_source(self):
        (self.tree.root / "src" / "nucore" / "stray.c").write_text("/* c */\n")
        results, _counts = status.check_version(VERSION, self.tree.root)
        coverage = results.get(f"{VERSION}/<coverage>", [])
        self.assertTrue(any("src/nucore/stray.c" in p for p in coverage),
                        results)


if __name__ == "__main__":
    unittest.main()
