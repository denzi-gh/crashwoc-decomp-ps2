"""Unit tests for promotion manifest editing.

flip_state is a pure function; no toolchain, no game files.
"""
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from promote import data_table_text, flip_state, set_complete, whole_unit_only

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


class TestDataEntries(unittest.TestCase):
    def test_flip_state_on_a_data_entry(self):
        text = ('[[data]]\nid = "pal103:unit-0014:00613fb0:nuvec4_zero"\n'
                'state = "asm"\n')
        self.assertIn('state = "matching"',
                      flip_state(text, "pal103:unit-0014:00613fb0:nuvec4_zero",
                                 "matching"))

    def test_data_table_text(self):
        text = data_table_text("pal103", 14, [
            {"address": 0x00613FB0, "name": "nuvec4_zero"},
            {"address": 0x00613FC0, "name": "nuvec4_one"}])
        self.assertEqual(text.count("[[data]]"), 2)
        self.assertIn('id = "pal103:unit-0014:00613fc0:nuvec4_one"\nstate = "asm"',
                      text)

    def test_set_complete(self):
        self.assertIn("complete = true\r\n",
                      set_complete("schema = 1\r\ncomplete = false\r\n"))
        with self.assertRaises(SystemExit):
            set_complete("schema = 1\n")

    def test_whole_unit_only(self):
        # bitconv's block_table32.83 is unsized and sits in the data-map gap
        # after the unit's .data range (committed data_map.toml).
        data = {"unit": "pal103:unit-0057", "data": [
            {"id": "pal103:unit-0057:002e98b0:lut.69", "state": "asm"},
            {"id": "pal103:unit-0057:002ea030:block_table32.83", "state": "asm"}]}
        self.assertEqual(whole_unit_only("pal103", data),
                         ["pal103:unit-0057:002ea030:block_table32.83"])
        data["data"][1]["state"] = "matching"
        self.assertEqual(whole_unit_only("pal103", data), [])


if __name__ == "__main__":
    unittest.main()
