"""Unit tests for .sbss/.bss data from C (data/bss): uninitialised objects
in the hybrid, their NOBITS pieces in whole-unit mode, the manifest rules
and the image link's reserved space.

Synthetic assembly in the shape ee-gcc emits plus the committed registry;
nothing here needs game files or the toolchain.
"""
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
import gen_hybrid  # noqa: E402
from gen_hybrid import (_Placed, _emit_nobits, _plan_data,  # noqa: E402
                        _registry_map, _small_data_externs, _split_sections,
                        _whole_unit_pieces)
from link_image import build_script  # noqa: E402
from status import check_data, load_data_registry  # noqa: E402
from verify_promoted import complete_problems  # noqa: E402

# sbrkr: the COMMON global errno; nuvport: three .bss statics.
SBRKR = """\
\t.text
\t.ent\t_sbrk_r
_sbrk_r:
\tj\t$31
\t.end\t_sbrk_r
\t.comm\terrno,4,4
"""
NUVPORT = """\
\t.section\t.bss
vpDevice:
\t.align\t4
\t.space\t64
\t.previous
"""


class TestUninitialisedObjects(unittest.TestCase):
    def test_common_global_maps_onto_its_registry_symbol(self):
        _text, _private, items = _split_sections(SBRKR.splitlines())
        (item, sym), = _registry_map(items, "pal103", "unit-0205")
        self.assertEqual((sym["name"], item.section, item.align, item.size),
                         ("errno", ".sbss", 2, 4))
        self.assertTrue(item.globl)

    def test_bss_static_dropped_per_symbol(self):
        _text, _private, items = _split_sections(NUVPORT.splitlines())
        self.assertEqual((items[0].section, items[0].align, items[0].size),
                         (".bss", 4, 64))
        data = {"source": "src/nu3d/nuvport.c", "data": [
            {"id": "pal103:unit-0034:0067d480:vpDevice", "state": "asm"}]}
        rename, emit = _plan_data(items, data, "pal103", "unit-0034", False)
        self.assertEqual((rename, emit), ({"vpDevice": "D_0067D480"}, []))
        self.assertEqual(_small_data_externs(items, emit, rename),
                         ["\t.extern\tD_0067D480, 64"])

    def test_emit_nobits(self):
        out = _emit_nobits(0x100, 0x104, 0x110, {0x104: ["a:"], 0x10C: ["b:"]})
        self.assertEqual(out, ["\t.space\t4", "a:", "\t.space\t8", "b:",
                               "\t.space\t4"])


class TestNobitsPieces(unittest.TestCase):
    RANGES = [
        {"section": ".sbss", "start": 0x2000, "end": 0x2008, "owner": "unit-0001"},
        {"section": ".sbss", "start": 0x2008, "end": 0x2010, "owner": "unassigned"},
        {"section": ".sbss", "start": 0x2010, "end": 0x2020, "owner": "unit-0002"},
    ]

    def placed(self, label, offset, size):
        item = gen_hybrid.DataItem(label, ".sbss", 2)
        sym = {"name": label, "address": 0x2000 + offset}
        return _Placed(item, sym, offset, [(offset, bytes(size))], size)

    def test_trailing_static_is_a_claim_and_nothing_is_retail(self):
        layout = {".sbss": (0x2000, [self.placed("a", 0, 4),
                                     self.placed("b", 4, 4),
                                     self.placed("last", 8, 4)])}
        with tempfile.TemporaryDirectory() as tmp:
            lines, pieces, retail, claims = _whole_unit_pieces(
                layout, "unit-0001", "pal103", Path(tmp), self.RANGES)
        self.assertEqual([(p["name"], p["end"]) for p in pieces],
                         [(".sbss.split_00002000", 0x2008),
                          (".sbss.split_00002008", 0x200C)])
        self.assertIn('\t.section\t.sbss.split_00002000,"aw",@nobits', lines)
        self.assertEqual(claims, [(".sbss", 0x2008, 0x200C)])
        self.assertEqual(retail, [])
        self.assertFalse(any(".incbin" in l or ".byte" in l for l in lines))
        self.assertEqual(lines[-2:], ["last:", "\t.space\t4"])


class TestManifestRules(unittest.TestCase):
    REGISTRY = load_data_registry(ROOT / "config" / "pal103")

    def entries(self, state="asm"):
        syms = sorted(self.REGISTRY[34].values(), key=lambda s: s["address"])
        return [{"id": f"pal103:unit-0034:{s['address']:08x}:{s['name']}",
                 "state": state} for s in syms]

    def test_nobits_symbols_are_listed(self):
        entries = self.entries()
        self.assertEqual(check_data(entries, "pal103", 34, self.REGISTRY)[0], [])
        problems, _s = check_data(entries[1:], "pal103", 34, self.REGISTRY)
        self.assertTrue(any("not listed" in p for p in problems), problems)

    def test_nobits_cannot_be_retail(self):
        entries = self.entries()
        entries[0].update(state="retail", reason="no")
        problems, _s = check_data(entries, "pal103", 34, self.REGISTRY)
        self.assertTrue(any("no file bytes" in p for p in problems), problems)


class TestImageLink(unittest.TestCase):
    RANGES = [
        {"section": ".sbss", "start": 0x00633000, "end": 0x00633008,
         "owner": "unit-0001"},
        {"section": ".sbss", "start": 0x00633008, "end": 0x00633400,
         "owner": "unassigned"},
    ]

    def test_complete_unit_supplies_its_nobits_pieces(self):
        obj = Path("build/a.o")
        script = build_script(0x1000, [], 0x2000, [], {"sbss": 0x400, "bss": 0},
                              {}, nobits_ranges=self.RANGES,
                              nobits_providers={"unit-0001": obj})
        self.assertIn("build/a.o(.sbss.split_00633000)", script)
        self.assertIn(". += 0x3F8;", script)
        self.assertIn('ASSERT(ABSOLUTE(.) == 0x00633400, ".sbss ends misplaced");',
                      script)
        self.assertIn(".bss  (NOLOAD) : { . += 0x000000; }", script)

    def test_claimed_nobits_span_and_reserved_holes(self):
        obj = Path("build/a.o")
        claimed = {(".sbss", 0x00633008): [(0x00633008, 0x0063300C, obj)]}
        script = build_script(0x1000, [], 0x2000, [], {"sbss": 0x400, "bss": 0},
                              {}, claimed=claimed, nobits_ranges=self.RANGES)
        self.assertIn(". += 0x8;\n", script)          # unit-0001 reserved
        self.assertIn("build/a.o(.sbss.split_00633008)\n        . += 0x3F4;",
                      script)

    def test_complete_unit_must_provide_its_nobits_ranges(self):
        report = {"whole_unit": True, "pieces": [], "retail": [], "borrowed": []}
        why = complete_problems({}, 1, "pal103", report, self.RANGES)
        self.assertTrue(any("expected object" in w for w in why), why)


if __name__ == "__main__":
    unittest.main()
