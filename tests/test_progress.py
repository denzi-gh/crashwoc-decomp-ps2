"""Unit tests for promotion editing and progress regression detection.

flip_state, find_regressions and diff_fields are pure functions; the
committed progress/summary.json baseline is validated for shape. No
toolchain, no game files.
"""
import json
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from compare_progress import diff_fields, find_regressions
from promote import flip_state

MANIFEST = '''schema = 1
unit = "pal103:unit-0007"

[[function]]
id = "pal103:unit-0007:00105a50:NuListGetHead"
state = "asm"

[[function]]
id = "pal103:unit-0007:00105a58:NuListGetTail"
state = "asm"
'''


class TestFlipState(unittest.TestCase):
    ID = "pal103:unit-0007:00105a50:NuListGetHead"

    def test_flips_exactly_one_entry(self):
        out = flip_state(MANIFEST, self.ID, "matching")
        self.assertIn(f'id = "{self.ID}"\nstate = "matching"', out)
        self.assertEqual(out.count('state = "matching"'), 1)
        self.assertEqual(out.count('state = "asm"'), 1)

    def test_round_trip_is_identity(self):
        out = flip_state(flip_state(MANIFEST, self.ID, "matching"),
                         self.ID, "asm")
        self.assertEqual(out, MANIFEST)

    def test_preserves_crlf(self):
        crlf = MANIFEST.replace("\n", "\r\n")
        out = flip_state(crlf, self.ID, "matching")
        self.assertNotIn("\r\r", out)
        self.assertEqual(out.replace('state = "matching"', 'state = "asm"'),
                         crlf)

    def test_unknown_id_fails(self):
        with self.assertRaises(SystemExit):
            flip_state(MANIFEST, "pal103:unit-0007:deadbeef:Nope", "matching")


def summary(matching=2, matching_bytes=16, complete=0, exact=True,
            fingerprint="f1"):
    return {
        "profiles_fingerprint": fingerprint,
        "code": {"total_bytes": 100, "matching_bytes": matching_bytes},
        "functions": {"total": 10, "matching": matching, "equivalent": 0,
                      "asm": 10 - matching},
        "units": {"total": 5, "text_units": 5, "complete": complete},
        "image": {"loaded_exact": exact, "packaged_exact": False,
                  "pure_relink_exact": False},
    }


class TestRegressions(unittest.TestCase):
    def test_no_change_is_clean(self):
        self.assertEqual(find_regressions(summary(), summary(), True), [])

    def test_progress_is_clean(self):
        self.assertEqual(
            find_regressions(summary(), summary(matching=3,
                                               matching_bytes=40), True), [])

    def test_matching_decrease_fails(self):
        regs = find_regressions(summary(matching=3), summary(matching=2),
                                True)
        self.assertTrue(any("matching functions" in r for r in regs))

    def test_bytes_decrease_fails(self):
        regs = find_regressions(summary(matching_bytes=24),
                                summary(matching_bytes=16), True)
        self.assertTrue(any("matching bytes" in r for r in regs))

    def test_complete_decrease_fails(self):
        regs = find_regressions(summary(complete=1), summary(complete=0),
                                True)
        self.assertTrue(any("complete units" in r for r in regs))

    def test_image_break_fails(self):
        regs = find_regressions(summary(exact=True), summary(exact=False),
                                True)
        self.assertTrue(any("no longer byte-exact" in r for r in regs))

    def test_fingerprint_change_needs_green_verify(self):
        old, new = summary(fingerprint="f1"), summary(fingerprint="f2")
        self.assertTrue(any("fingerprint" in r
                            for r in find_regressions(old, new, False)))
        self.assertEqual(find_regressions(old, new, True), [])

    def test_diff_fields_ignores_volatile(self):
        old = dict(summary(), commit="a", generated="t1")
        new = dict(summary(matching=3), commit="b", generated="t2")
        changes = diff_fields(old, new)
        self.assertIn("functions.matching", changes)
        self.assertNotIn("commit", changes)
        self.assertNotIn("generated", changes)


class TestCommittedBaseline(unittest.TestCase):
    def test_shape_and_honesty(self):
        s = json.loads((ROOT / "progress" / "summary.json").read_text())
        self.assertEqual(s["schema"], 1)
        f = s["functions"]
        self.assertEqual(f["total"], f["matching"] + f["equivalent"]
                         + f["asm"])
        self.assertLessEqual(s["code"]["matching_bytes"],
                             s["code"]["total_bytes"])
        self.assertEqual(s["units"]["text_units"], 247)
        # Baselines never claim what the verifier didn't prove.
        self.assertIsInstance(s["image"]["loaded_exact"], bool)
        self.assertFalse(s["image"]["packaged_exact"])
        self.assertFalse(s["image"]["pure_relink_exact"])


if __name__ == "__main__":
    unittest.main()
