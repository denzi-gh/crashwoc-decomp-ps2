"""Modding-SDK structure tests: pure Python, no game files, no container.
The byte gates that need orig/ live in tools/modsdk/build.py itself."""
import struct
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))

from declib import elf32  # noqa: E402
from modsdk import elf_patch, gen_hooks, mips  # noqa: E402
from modsdk.manifest import (  # noqa: E402
    ManifestError, load_manifest, load_manifests)

GOOD = """\
[mod]
name = "demo"
version = "0.1"
target = "pal103"

[code]
sources = ["mod.c"]

[[hook]]
function = "UpdateLevel"
handler = "demo_tick"
mode = "pre"

[[data_patch]]
address = 0x00612bbc
bytes = "00000000"

[mailbox]
size = 0x200
"""


def write_mod(tmp, toml=GOOD, with_source=True):
    d = Path(tmp)
    (d / "mod.toml").write_text(toml)
    if with_source:
        (d / "mod.c").write_text("void demo_tick(void) {}\n")
    return d / "mod.toml"


class TestManifest(unittest.TestCase):
    def test_good_manifest(self):
        with tempfile.TemporaryDirectory() as tmp:
            m = load_manifest(write_mod(tmp))
        self.assertEqual(m.name, "demo")
        self.assertEqual([h.mode for h in m.hooks], ["pre"])
        self.assertEqual(m.data_patches[0].address, 0x612BBC)
        self.assertEqual(m.data_patches[0].data, b"\x00" * 4)
        self.assertEqual(m.mailbox_size, 0x200)

    def test_missing_source_rejected(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = write_mod(tmp, with_source=False)
            with self.assertRaisesRegex(ManifestError, "source not found"):
                load_manifest(path)

    def test_bad_hook_mode_rejected(self):
        bad = GOOD.replace('mode = "pre"', 'mode = "post"')
        with tempfile.TemporaryDirectory() as tmp:
            with self.assertRaisesRegex(ManifestError, "mode"):
                load_manifest(write_mod(tmp, bad))

    def test_source_escape_rejected(self):
        bad = GOOD.replace('"mod.c"', '"../mod.c"')
        with tempfile.TemporaryDirectory() as tmp:
            with self.assertRaisesRegex(ManifestError, "inside the mod"):
                load_manifest(write_mod(tmp, bad))

    def test_address_and_symbol_mutually_exclusive(self):
        bad = GOOD.replace("address = 0x00612bbc",
                           'address = 0x00612bbc\nsymbol = "X"')
        with tempfile.TemporaryDirectory() as tmp:
            with self.assertRaisesRegex(ManifestError, "exactly one"):
                load_manifest(write_mod(tmp, bad))

    def test_odd_hex_rejected(self):
        bad = GOOD.replace('bytes = "00000000"', 'bytes = "000"')
        with tempfile.TemporaryDirectory() as tmp:
            with self.assertRaisesRegex(ManifestError, "hex"):
                load_manifest(write_mod(tmp, bad))

    def test_unknown_table_rejected(self):
        with tempfile.TemporaryDirectory() as tmp:
            with self.assertRaisesRegex(ManifestError, "unknown table"):
                load_manifest(write_mod(tmp, GOOD + "\n[nonsense]\nx = 1\n"))

    def test_conflicting_hooks_across_mods_rejected(self):
        with tempfile.TemporaryDirectory() as t1, \
                tempfile.TemporaryDirectory() as t2:
            p1 = write_mod(t1)
            other = GOOD.replace('name = "demo"', 'name = "other"') \
                        .replace("[mailbox]\nsize = 0x200\n", "")
            p2 = write_mod(t2, other)
            with self.assertRaisesRegex(ManifestError, "conflicting hooks"):
                load_manifests([p1, p2])

    def test_two_mailboxes_rejected(self):
        with tempfile.TemporaryDirectory() as t1, \
                tempfile.TemporaryDirectory() as t2:
            p1 = write_mod(t1)
            other = GOOD.replace('name = "demo"', 'name = "other"') \
                        .replace('function = "UpdateLevel"',
                                 'function = "DoInput"')
            p2 = write_mod(t2, other)
            with self.assertRaisesRegex(ManifestError, "one mod"):
                load_manifests([p1, p2])


class TestMips(unittest.TestCase):
    def test_encoders_match_known_words(self):
        # Ground truth from the retail disassembly.
        self.assertEqual(mips.addiu(mips.SP, mips.SP, -0x30), 0x27BDFFD0)
        self.assertEqual(mips.jr_ra(), 0x03E00008)
        self.assertEqual(mips.j(0x1C6908) >> 26, 0x02)
        self.assertEqual((mips.j(0x1C6908) & 0x3FFFFFF) << 2, 0x1C6908)
        self.assertEqual(mips.jal(0x278C80), 0x0C09E320)

    def test_relocatable_classification(self):
        self.assertTrue(mips.is_relocatable(0x27BDFFD0))   # addiu sp
        self.assertTrue(mips.is_relocatable(0x7FB10010))   # sq s1
        self.assertTrue(mips.is_relocatable(0x00000000))   # nop
        self.assertTrue(mips.is_relocatable(0x3C020070))   # lui
        self.assertFalse(mips.is_relocatable(0x0C09E320))  # jal
        self.assertFalse(mips.is_relocatable(0x08000000))  # j
        self.assertFalse(mips.is_relocatable(0x03E00008))  # jr ra
        self.assertFalse(mips.is_relocatable(0x10400003))  # beqz
        self.assertFalse(mips.is_relocatable(0x0000000C))  # syscall
        self.assertFalse(mips.is_relocatable(0x45010004))  # bc1t


class DummyHook:
    def __init__(self, function, handler, mode):
        self.function, self.handler, self.mode = function, handler, mode


class TestGenHooks(unittest.TestCase):
    DISPLACED = [0x27BDFFD0, 0x7FB10010]  # addiu sp,-0x30; sq s1,0x10(sp)

    def plan_one(self, mode):
        hooks = [DummyHook("UpdateLevel", "demo_tick", mode)]
        plans, size, provides = gen_hooks.plan_stubs(hooks, 0x00707000)
        return plans[0], size, provides

    def test_pre_stub_roundtrip(self):
        plan, size, provides = self.plan_one("pre")
        self.assertEqual(size, gen_hooks.PRE_STUB_SIZE)
        self.assertEqual(provides, {})
        stub = gen_hooks.emit_stub(plan, 0x001EF658, self.DISPLACED,
                                   0x00707800)
        self.assertEqual(len(stub), gen_hooks.PRE_STUB_SIZE)
        words = struct.unpack(f"<{len(stub) // 4}I", stub)
        self.assertIn(mips.jal(0x00707800), words)       # calls the handler
        self.assertIn(mips.j(0x001EF658 + 8), words)     # resumes after
        self.assertIn(self.DISPLACED[0], words)          # replays prologue
        self.assertIn(self.DISPLACED[1], words)
        # The delay slot of the resume jump holds the second displaced word.
        idx = words.index(mips.j(0x001EF658 + 8))
        self.assertEqual(words[idx - 1], self.DISPLACED[0])
        self.assertEqual(words[idx + 1], self.DISPLACED[1])
        patch = gen_hooks.emit_patch(plan, 0x001EF658, self.DISPLACED,
                                     0x00707800)
        self.assertEqual(patch[:4], mips.j(plan.stub_addr).to_bytes(4, "little"))
        self.assertEqual(patch[4:], b"\x00\x00\x00\x00")

    def test_replace_exports_orig_thunk(self):
        plan, size, provides = self.plan_one("replace")
        self.assertEqual(size, gen_hooks.REPLACE_THUNK_SIZE)
        self.assertEqual(provides, {"orig_UpdateLevel": plan.stub_addr})
        stub = gen_hooks.emit_stub(plan, 0x001EF658, self.DISPLACED,
                                   0x00707800)
        words = struct.unpack("<4I", stub)
        self.assertEqual(words[0], self.DISPLACED[0])
        self.assertEqual(words[1], mips.j(0x001EF658 + 8))
        self.assertEqual(words[2], self.DISPLACED[1])
        patch = gen_hooks.emit_patch(plan, 0x001EF658, self.DISPLACED,
                                     0x00707800)
        self.assertEqual(patch[:4], mips.j(0x00707800).to_bytes(4, "little"))

    def test_branchy_prologue_refused(self):
        plan, _size, _p = self.plan_one("pre")
        with self.assertRaises(mips.HookSiteError):
            gen_hooks.emit_stub(plan, 0x001EF658,
                                [0x0C09E320, 0x00000000], 0x00707800)

    def test_supplant_no_thunk_no_orig(self):
        plan, size, provides = self.plan_one("supplant")
        self.assertEqual(size, 0)            # occupies no stub space
        self.assertEqual(provides, {})       # and exports no orig_
        stub = gen_hooks.emit_stub(plan, 0x001EF658, self.DISPLACED,
                                   0x00707800)
        self.assertEqual(stub, b"")
        patch = gen_hooks.emit_patch(plan, 0x001EF658, self.DISPLACED,
                                     0x00707800)
        self.assertEqual(patch[:4], mips.j(0x00707800).to_bytes(4, "little"))
        self.assertEqual(patch[4:], b"\x00\x00\x00\x00")

    def test_supplant_accepts_a_jump_prologue(self):
        """The reason supplant exists.

        NuRndrLine3dDbg's entire body is `jr $ra; nop`. `replace` refuses it
        because it cannot relocate a jump into an orig_ thunk -- but supplant
        never replays the displaced words, so the site is fine.
        """
        jr_ra_nop = [0x03E00008, 0x00000000]
        pre, _s, _p = self.plan_one("pre")
        with self.assertRaises(mips.HookSiteError):
            gen_hooks.emit_stub(pre, 0x0012FDF0, jr_ra_nop, 0x00707800)

        plan, _size, _provides = self.plan_one("supplant")
        self.assertEqual(
            gen_hooks.emit_stub(plan, 0x0012FDF0, jr_ra_nop, 0x00707800), b"")
        patch = gen_hooks.emit_patch(plan, 0x0012FDF0, jr_ra_nop, 0x00707800)
        self.assertEqual(patch[:4], mips.j(0x00707800).to_bytes(4, "little"))

    def test_supplant_still_rejects_unaligned_target(self):
        plan, _size, _p = self.plan_one("supplant")
        with self.assertRaises(mips.HookSiteError):
            gen_hooks.emit_stub(plan, 0x0012FDF2, self.DISPLACED, 0x00707800)


def make_mini_elf():
    """A synthetic single-PT_LOAD ELF shaped like the retail one."""
    vaddr, off, filesz, memsz = 0x100000, 0x1000, 0x200, 0x400
    ehdr = bytearray(0x34)
    ehdr[:4] = b"\x7fELF"
    ehdr[4:6] = b"\x01\x01"
    struct.pack_into("<HHIIIIIHHHHHH", ehdr, 0x10,
                     2, 8, 1, 0x100008, 0x34, 0, 0x20924001,
                     0x34, 0x20, 1, 0x28, 0, 0)
    phdr = struct.pack("<8I", 1, off, vaddr, vaddr, filesz, memsz, 7, 0x1000)
    pad = b"\x00" * (off - 0x34 - len(phdr))
    body = bytes(range(256)) * 2
    return bytes(ehdr) + phdr + pad + body


class TestElfPatch(unittest.TestCase):
    def test_no_patches_is_identity(self):
        elf = make_mini_elf()
        self.assertEqual(elf_patch.build_patched_elf(elf, {}), elf)
        self.assertEqual(elf_patch.verify_output(elf, elf, {}), [])

    def test_patch_and_blob(self):
        elf = make_mini_elf()
        patches = {0x100010: b"\xAA\xBB\xCC\xDD"}
        blob = b"\x11" * 0x30
        out = elf_patch.build_patched_elf(elf, patches, 0x706A00, blob)
        self.assertEqual(out[0x1010:0x1014], b"\xAA\xBB\xCC\xDD")
        self.assertEqual(elf_patch.verify_output(out, elf, patches), [])
        phdrs = elf32.program_headers(out)
        self.assertEqual(len(phdrs), 2)
        self.assertEqual(phdrs[0]["offset"], 0x1000)  # retail phdr intact
        new = phdrs[1]
        self.assertEqual((new["vaddr"], new["filesz"], new["memsz"]),
                         (0x706A00, len(blob), len(blob)))
        self.assertEqual(new["offset"] % new["align"],
                         new["vaddr"] % new["align"])
        self.assertEqual(out[new["offset"]:new["offset"] + len(blob)], blob)

    def test_undeclared_divergence_detected(self):
        elf = make_mini_elf()
        out = bytearray(elf_patch.build_patched_elf(elf, {}))
        out[0x1020] ^= 0xFF
        problems = elf_patch.verify_output(bytes(out), elf, {})
        self.assertTrue(problems and "diverges" in problems[0])

    def test_patch_outside_segment_rejected(self):
        elf = make_mini_elf()
        with self.assertRaises(elf32.Elf32Error):
            elf_patch.build_patched_elf(elf, {0x100000 + 0x200: b"\x00"})


if __name__ == "__main__":
    unittest.main()
