"""Unit tests for the MIPS code-reference decoder (tools/declib/elf.py).

The synthetic tests encode a few instruction words in-test (no game files).
The cross-check proves the decoder finds every lui/%lo and %gp_rel pair
splat resolved in asm/text.s; it needs the user's ELF and a configured asm/
tree and is skipped without them.
"""
import json
import re
import sys
import tomllib
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from declib.elf import code_refs  # noqa: E402

BASE = 0x00100000
GP = 0x00634970
ZERO, AT, V0, V1, A0, T0, T1, S0, S5, SP = 0, 1, 2, 3, 4, 8, 9, 16, 21, 29


def i_type(op, rs, rt, imm):
    return (op << 26) | (rs << 21) | (rt << 16) | (imm & 0xFFFF)


def lui(rt, imm):
    return i_type(0x0F, 0, rt, imm)


def addiu(rt, rs, imm):
    return i_type(0x09, rs, rt, imm)


def ori(rt, rs, imm):
    return i_type(0x0D, rs, rt, imm)


def lw(rt, imm, rs):
    return i_type(0x23, rs, rt, imm)


def ld(rt, imm, rs):
    return i_type(0x37, rs, rt, imm)


def lwc1(ft, imm, rs):
    return i_type(0x31, rs, ft, imm)


def sw(rt, imm, rs):
    return i_type(0x2B, rs, rt, imm)


def addu(rd, rs, rt):
    return (rs << 21) | (rt << 16) | (rd << 11) | 0x21


def beq(rs, rt, words):
    return i_type(0x04, rs, rt, words)


def bne(rs, rt, words):
    return i_type(0x05, rs, rt, words)


def bnel(rs, rt, words):
    return i_type(0x15, rs, rt, words)


def jal(target):
    return (0x03 << 26) | ((target >> 2) & 0x03FFFFFF)


NOP = 0


def refs(words, starts=(BASE,)):
    code = b"".join(w.to_bytes(4, "little") for w in words)
    return [(r.vram - BASE, r.address, r.width, r.base)
            for r in code_refs(code, BASE, starts, GP)]


class TestPairs(unittest.TestCase):
    def test_lui_load_pair(self):
        self.assertEqual(refs([lui(V0, 0x61), lw(A0, 0x1234, V0)]),
                         [(4, 0x611234, 4, "hi")])

    def test_negative_low_half(self):
        self.assertEqual(refs([lui(V0, 0x62), addiu(A0, V0, -0x6FE0)]),
                         [(4, 0x619020, 0, "hi")])

    def test_widths(self):
        got = refs([lui(V0, 0x61), ld(A0, 8, V0), lwc1(0, 0x10, V0),
                    sw(A0, 0x20, V0)])
        self.assertEqual([(a, w) for _o, a, w, _b in got],
                         [(0x610008, 8), (0x610010, 4), (0x610020, 4)])

    def test_gp_relative_is_exact(self):
        self.assertEqual(refs([lw(V1, -0x5F60, 28)]),
                         [(0, GP - 0x5F60, 4, "gp")])

    def test_one_lui_feeds_several_uses(self):
        got = refs([lui(V0, 0x61), lw(A0, 0, V0), lw(V1, 4, V0)])
        self.assertEqual([a for _o, a, _w, _b in got], [0x610000, 0x610004])

    def test_index_through_addu(self):
        got = refs([lui(V0, 0x61), addu(V0, V0, A0), lw(V1, 0x40, V0)])
        self.assertEqual(got, [(8, 0x610040, 4, "hi")])


class TestInvalidation(unittest.TestCase):
    def test_write_kills_the_value(self):
        self.assertEqual(refs([lui(V0, 0x61), ori(V0, V0, 1), lw(A0, 0, V0)]), [])

    def test_load_into_base_kills_it_after_use(self):
        got = refs([lui(V0, 0x61), lw(V0, 0, V0), lw(A0, 4, V0)])
        self.assertEqual([a for _o, a, _w, _b in got], [0x610000])

    def test_call_clobbers_after_its_delay_slot(self):
        got = refs([lui(V1, 0x61), lui(S0, 0x62), jal(0x00200000),
                    lw(A0, 0, V1),                 # delay slot: still valid
                    lw(A0, 4, V1),                 # caller-saved: gone
                    lw(A0, 8, S0)])                # callee-saved: survives
        self.assertEqual([a for _o, a, _w, _b in got], [0x610000, 0x620008])

    def test_function_start_resets(self):
        got = refs([lui(V0, 0x61), NOP, lw(A0, 0, V0)],
                   starts=(BASE, BASE + 8))
        self.assertEqual(got, [])


class TestControlFlow(unittest.TestCase):
    def test_delay_slot_lui_reaches_the_target(self):
        got = refs([beq(ZERO, ZERO, 2),            # -> word 3
                    lui(V0, 0x61),                 # delay slot
                    addiu(V0, ZERO, 3),            # only reachable by label
                    lw(A0, 0x10, V0)])             # target
        self.assertEqual(got, [(12, 0x610010, 4, "hi")])

    def test_disagreeing_paths_drop_the_register(self):
        got = refs([bne(T0, ZERO, 2),              # -> word 3
                    lui(V0, 0x61),                 # delay: taken path 0x61
                    lui(V0, 0x62),                 # fall-through 0x62
                    lw(A0, 0, V0)])                # join: conflict
        self.assertEqual(got, [])

    def test_likely_slot_is_annulled_on_fall_through(self):
        got = refs([lui(V0, 0x61),
                    bnel(T0, ZERO, 4),             # -> word 6
                    lui(V0, 0x62),                 # runs only when taken
                    lw(A0, 0, V0),                 # fall-through: 0x61
                    beq(ZERO, ZERO, 2), NOP,       # -> word 7
                    lw(A0, 4, V0),                 # target of bnel: 0x62
                    NOP])
        self.assertEqual([(o, a) for o, a, _w, _b in got],
                         [(12, 0x610000), (24, 0x620004)])

    def test_backward_branch_feeds_its_target(self):
        got = refs([beq(ZERO, ZERO, 2), NOP,       # -> word 3
                    lw(A0, 0x10, S5),              # reached only from below
                    lui(S5, 0x61),
                    bne(T1, ZERO, -3), NOP])       # -> word 2
        self.assertEqual(got, [(8, 0x610010, 4, "hi")])


ELF = ROOT / "orig" / "pal103" / "SLES_503.86"
ASM = ROOT / "asm" / "text.s"


@unittest.skipUnless(ELF.is_file() and ASM.is_file(),
                     "needs the target ELF and a configured asm/ tree")
class TestAgainstSplat(unittest.TestCase):
    """Every pair splat resolved is a decoder pair (the decoder may find more:
    splat leaves some pairs unresolved, e.g. testShadowCast's)."""

    @classmethod
    def setUpClass(cls):
        from declib.asmtext import AUTO_NAME_RE, INSTR_RE, load_symbol_addrs
        spec = json.loads((ROOT / "config/pal103/sections.json").read_text())
        text = next(s for s in spec["sections"] if s["name"] == ".text")
        lo, off = int(text["addr"], 0), int(text["offset"], 0)
        size = int(text["size"], 0)
        funcs = tomllib.loads((ROOT / "config/pal103/functions.toml")
                              .read_text())["function"]
        data = ELF.read_bytes()
        cls.mine = {(r.vram, r.address) for r in code_refs(
            data[off:off + size], lo, [f["address"] for f in funcs],
            int(spec["gp_value"], 0))}
        symbols = load_symbol_addrs("pal103")
        ref_re = re.compile(r"%(?:lo|gp_rel)\(\s*([A-Za-z_.$][\w.$]*)\s*"
                            r"(?:([+-])\s*(0x[0-9A-Fa-f]+|\d+))?\s*\)")
        cls.splat, cls.unresolved = set(), []
        for line in ASM.read_text().splitlines():
            m = ref_re.search(line)
            instr = INSTR_RE.search(line) if m else None
            if not instr:
                continue
            addr = symbols.get(m.group(1))
            if addr is None:
                auto = AUTO_NAME_RE.fullmatch(m.group(1))
                addr = int(auto.group(1), 16) if auto else None
            if addr is None:
                cls.unresolved.append(line)
                continue
            if m.group(2):
                addr += int(m.group(3), 0) * (1 if m.group(2) == "+" else -1)
            cls.splat.add((int(instr.group(1), 16), addr))

    def test_all_splat_pairs_found(self):
        self.assertEqual(self.unresolved, [])
        missing = sorted(self.splat - self.mine)
        self.assertEqual(missing, [], [f"{v:#x}->{a:#x}" for v, a in missing[:20]])

    def test_extras_include_test_shadow_cast(self):
        # testShadowCast builds 0x615FA0 with `lui 0x61` / `addiu 0x5FA0`.
        extras = self.mine - self.splat
        self.assertIn((0x00162B54, 0x00615FA0), extras)


if __name__ == "__main__":
    unittest.main()
