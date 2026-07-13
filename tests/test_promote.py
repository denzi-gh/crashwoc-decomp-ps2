"""Unit tests for promotion manifest editing.

flip_state is a pure function; no toolchain, no game files.
"""
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
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


if __name__ == "__main__":
    unittest.main()
