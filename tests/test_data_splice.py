"""Unit tests for Data-from-C in gen_hybrid and the `complete` rule.

The assembly below is the shape both EE compilers emit for data (from
tools/probe_data_emission.py); the `complete` rule is in test_whole_unit.py. The registry tests use the committed
config/pal103/data_symbols.toml (numath/nuvec4, e_pow); nothing here needs
game files or the toolchain.
"""
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from gen_hybrid import (DataItem, HybridError, _JumpTable,  # noqa: E402
                        _directive_size, _is_align_pad, _plan_data,
                        _relabel_data, _relabel_items, _small_data_externs,
                        _split_sections, has_c_data, parse_s, piece_name)

PROBE = """\
\t.file\t1 "probe.c"
gcc2_compiled.:
\t.globl\tg_const4
\t.sdata
\t.align\t2
\t.type\t g_const4,@object
\t.size\t g_const4,4
g_const4:
\t.word\t286331153
\t.globl\tg_data4
\t.align\t2
\t.type\t g_data4,@object
\t.size\t g_data4,4
g_data4:
\t.word\t572662306
\t.rdata
\t.align\t3
\t.type\t s_const16,@object
\t.size\t s_const16,16
s_const16:
\t.word\t9
\t.word\t10
\t.word\t11
\t.word\t12
\t.globl\tg_strtab
\t.data
\t.align\t3
\t.size\t g_strtab,8
g_strtab:
\t.word\t$LC1
\t.word\t$LC0
\t.rdata
\t.align\t3
$LC1:
\t.ascii\t"a longer string literal\\000"
\t.sdata
\t.align\t3
$LC0:
\t.ascii\t"a\\000"
\t.text
\t.align\t3
\t.globl\tuse_statics
\t.text
\t.ent\tuse_statics
use_statics:
\tlui\t$2,%hi(s_const16) # high
\tj\t$31
\t.end\tuse_statics
\t.data
\t.align\t3
\t.type\t lut.6,@object
\t.size\t lut.6,16
lut.6:
\t.word\t1
\t.word\t2
\t.word\t3
\t.word\t4
\t.text
\t.align\t3
\t.globl\tlut_a
\t.text
\t.ent\tlut_a
lut_a:
\tlui\t$2,%hi(lut.6) # high
\tj\t$31
\t.end\tlut_a

\t.comm\tg_common,4,4
\t.section\t.sbss
s_bss:
\t.align\t2
\t.space\t4
\t.previous
"""


class TestSplitSections(unittest.TestCase):
    def setUp(self):
        self.text, self.private, self.items = _split_sections(PROBE.splitlines())

    def test_named_objects_sections_and_attributes(self):
        got = [(i.name, i.section, i.align, i.size, i.globl) for i in self.items]
        self.assertEqual(got, [
            ("g_const4", ".sdata", 2, 4, True),
            ("g_data4", ".sdata", 2, 4, True),
            ("s_const16", ".rodata", 3, 16, False),
            ("g_strtab", ".data", 3, 8, True),
            ("lut.6", ".data", 3, 16, False),
            ("g_common", "COMMON", None, 4, False),
            ("s_bss", ".sbss", None, None, False),
        ])
        self.assertEqual(self.items[2].lines, ["\t.word\t9", "\t.word\t10",
                                               "\t.word\t11", "\t.word\t12"])
        # An .align after the label is interior to the object.
        self.assertEqual(self.items[6].lines, ["\t.align\t2", "\t.space\t4"])

    def test_private_blocks(self):
        self.assertEqual(self.private, {"$LC1": b"a longer string literal\0",
                                        "$LC0": b"a\0"})

    def test_code_stream_keeps_functions_only(self):
        text = "\n".join(self.text)
        for gone in ("g_const4", "g_strtab", ".sdata", ".rdata", ".comm",
                     "lut.6:", ".word", ".previous"):
            self.assertNotIn(gone, text)
        self.assertIn("\t.globl\tuse_statics", self.text)
        self.assertIn("\tlui\t$2,%hi(lut.6) # high", self.text)

    def test_parse_s_rejects_data_unless_asked(self):
        with self.assertRaises(HybridError):
            parse_s(PROBE)
        items = []
        _prologue, segments, _local = parse_s(PROBE, items)
        self.assertEqual([n for n, _s in segments], ["use_statics", "lut_a"])
        self.assertEqual(len(items), 7)

    def test_jump_table_stays_a_marker(self):
        _t, private, _i = _split_sections(
            ["\tj\t$4", "\t.rdata", "\t.align\t4", "$L14:", "\t.word\t$L7",
             "\t.word\t$L8", "\t.text", "$L7:", "\tnop"])
        self.assertEqual(private, {"$L14": _JumpTable(2)})

    def test_function_globl_in_a_data_section_stays_in_the_code(self):
        text, _p, items = _split_sections(
            ["\t.sdata", "$LC0:", "\t.word\t1", "\t.globl\tfoo", "\t.text",
             "\t.ent\tfoo", "foo:", "\t.end\tfoo"])
        self.assertIn("\t.globl\tfoo", text)
        self.assertEqual(items, [])


def items_for(names, section=".rodata", size=16):
    out = []
    for name in names:
        item = DataItem(name, section, 3)
        item.size, item.globl = size, True
        item.lines = ["\t.word\t0"] * (size // 4)
        out.append(item)
    return out


NUVEC4 = ["nuvec4_zero", "nuvec4_one", "nuvec4_x", "nuvec4_y", "nuvec4_z",
          "nuvec4_w"]
NUVEC4_ADDR = [0x00613FB0 + 16 * i for i in range(6)]


def manifest(states):
    return {"source": "src/numath/nuvec4.c",
            "data": [{"id": f"pal103:unit-0014:{a:08x}:{n}", "state": s}
                     for (n, a), s in zip(zip(NUVEC4, NUVEC4_ADDR), states)]}


class TestPlanData(unittest.TestCase):
    def test_matching_objects_are_emitted(self):
        rename, emit = _plan_data(items_for(NUVEC4), manifest(["matching"] * 6),
                                  "pal103", "unit-0014", report=False)
        self.assertEqual(rename, {})
        self.assertEqual([s["address"] for _i, s in emit], NUVEC4_ADDR)

    def test_asm_objects_are_dropped(self):
        rename, emit = _plan_data(items_for(NUVEC4),
                                  manifest(["matching"] + ["asm"] * 5),
                                  "pal103", "unit-0014", report=False)
        self.assertEqual([s["name"] for _i, s in emit], ["nuvec4_zero"])
        self.assertEqual(rename, {})          # globals keep their names

    def test_report_object_emits_nothing(self):
        _rename, emit = _plan_data(items_for(NUVEC4), manifest(["matching"] * 6),
                                   "pal103", "unit-0014", report=True)
        self.assertEqual(emit, [])

    def test_needs_a_data_table(self):
        with self.assertRaisesRegex(HybridError, "init-data"):
            _plan_data(items_for(NUVEC4), {"source": "x.c"}, "pal103",
                       "unit-0014", report=False)

    def test_section_mismatch(self):
        with self.assertRaisesRegex(HybridError, "data_section_mismatch"):
            _plan_data(items_for(NUVEC4, section=".sdata"),
                       manifest(["matching"] * 6), "pal103", "unit-0014", False)

    def test_size_mismatch(self):
        with self.assertRaisesRegex(HybridError, ".size 8"):
            _plan_data(items_for(NUVEC4, size=8), manifest(["matching"] * 6),
                       "pal103", "unit-0014", False)

    def test_matching_entry_must_be_defined(self):
        with self.assertRaisesRegex(HybridError, "not defined"):
            _plan_data(items_for(NUVEC4[:5]), manifest(["matching"] * 6),
                       "pal103", "unit-0014", False)

    def test_unknown_object(self):
        with self.assertRaisesRegex(HybridError, "not a data symbol"):
            _plan_data(items_for(["stray"]), manifest(["asm"] * 6), "pal103",
                       "unit-0014", False)

    def test_dropped_static_resolves_to_its_address(self):
        # e_pow's `one` is static: dropped, it is referenced as D_<VRAM>.
        item = items_for(["one"], size=8)
        data = {"source": "e_pow.c", "data": [
            {"id": "pal103:unit-0163:0062b568:one", "state": "asm"}]}
        rename, emit = _plan_data(item, data, "pal103", "unit-0163", False)
        self.assertEqual((rename, emit), ({"one": "D_0062B568"}, []))

    def test_has_c_data(self):
        self.assertTrue(has_c_data(manifest(["asm", "matching"])))
        self.assertFalse(has_c_data(manifest(["asm", "retail"])))

    def test_sdata_from_c(self):
        # nurand: two 8-byte .sdata statics; an emitted one keeps its name,
        # a dropped one is addressed as its retail address.
        items = items_for(["global_rand", "fseed"], section=".sdata", size=8)
        data = {"source": "src/numath/nurand.c", "data": [
            {"id": "pal103:unit-0026:0062ea60:global_rand", "state": "matching"},
            {"id": "pal103:unit-0026:0062ea68:fseed", "state": "asm"}]}
        rename, emit = _plan_data(items, data, "pal103", "unit-0026", False)
        self.assertEqual([s["name"] for _i, s in emit], ["global_rand"])
        self.assertEqual(rename, {"fseed": "D_0062EA68"})
        # Only the size keeps the dropped object's loads gp-relative.
        self.assertEqual(_small_data_externs(items, emit, rename),
                         ["\t.extern\tD_0062EA68, 8"])


class TestHelpers(unittest.TestCase):
    def test_relabel_data_whole_names(self):
        lines = ["\tlui\t$2,%hi(lut.6)", "\tlw\t$3,%lo(lut.60)($2)", "\tla\t$4,lut"]
        self.assertEqual(_relabel_data(lines, {"lut.6": "lut.69", "lut": "D_1"}),
                         ["\tlui\t$2,%hi(lut.69)", "\tlw\t$3,%lo(lut.60)($2)",
                          "\tla\t$4,D_1"])

    def test_relabel_items_leaves_string_text(self):
        # nu3d/nuscene: NuNodeRead is a duplicated static (NuNodeRead__<vram>)
        lines = ['\t.ascii\t"NuNodeRead: Unknown node type.\\000"',
                 "\t.word\tNuNodeRead"]
        self.assertEqual(_relabel_items(lines, {}, {"NuNodeRead":
                                                    "NuNodeRead__00118900"}),
                         [lines[0], "\t.word\tNuNodeRead__00118900"])

    def test_directive_sizes(self):
        self.assertEqual(_directive_size("\t.word\t1,2,3"), 12)
        self.assertEqual(_directive_size("\t.word\tsym"), 4)
        self.assertEqual(_directive_size('\t.ascii\t"ab\\000"'), 3)
        self.assertEqual(_directive_size("\t.half\t1"), 2)

    def test_piece_name(self):
        self.assertEqual(piece_name(".rodata", 0x00613FB0), ".rodata.split_00613fb0")

    def test_align_pad(self):
        # nuvec_zero (12 bytes at ...850) then `.align 3` nuvec_one at ...860.
        self.assertTrue(_is_align_pad(bytes(4), 0x29385C, 0x293860, 3))
        self.assertFalse(_is_align_pad(b"\0\0\0\1", 0x29385C, 0x293860, 3))
        self.assertFalse(_is_align_pad(bytes(4), 0x29385C, 0x293860, 2))
        self.assertFalse(_is_align_pad(bytes(12), 0x293854, 0x293860, 3))
        self.assertFalse(_is_align_pad(bytes(4), 0x29385C, 0x293860, 0))


if __name__ == "__main__":
    unittest.main()
