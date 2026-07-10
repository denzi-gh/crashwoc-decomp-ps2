"""Modded ELF writer: retail bytes + in-segment patches + blob PT_LOAD.

The blob's program header extends the table IN PLACE at e_phoff (retail's
bytes after the table are unloaded linker debris). Do not relocate the
table to end-of-file: PCSX2 silently skips segments from a moved table --
the hook then jumps into zeroed RAM and nop-slides to the top of memory
(observed 2026-07-10, TLB Miss pc=0x1fff000). Everything else keeps its
retail file offset.
"""
import struct

from declib.elf32 import Elf32Error, program_headers

PT_LOAD = 1
PF_RWX = 7
_E_PHOFF, _E_PHNUM = 0x1C, 0x2C


def _load_phdr(retail):
    phdrs = program_headers(retail)
    loads = [p for p in phdrs if p["type"] == PT_LOAD]
    if len(loads) != 1:
        raise Elf32Error(f"expected exactly one PT_LOAD, found {len(loads)}")
    return phdrs, loads[0]


def vaddr_to_offset(load, vaddr, size=1):
    lo, span = load["vaddr"], load["filesz"]
    if not (lo <= vaddr and vaddr + size <= lo + span):
        raise Elf32Error(
            f"0x{vaddr:x}+{size:#x} outside the file-backed segment "
            f"[0x{lo:x}, 0x{lo + span:x})")
    return load["offset"] + (vaddr - lo)


def build_patched_elf(retail, patches, blob_vaddr=None, blob=b""):
    """No patches and no blob -> output byte-identical to the input.

    The blob must have bss materialized as zeros (filesz == memsz).
    """
    phdrs, load = _load_phdr(retail)
    out = bytearray(retail)
    for vaddr in sorted(patches):
        data = patches[vaddr]
        off = vaddr_to_offset(load, vaddr, len(data))
        out[off:off + len(data)] = data
    if not blob:
        return bytes(out)
    if blob_vaddr is None or blob_vaddr % 16:
        raise Elf32Error(f"bad blob vaddr: {blob_vaddr!r}")

    # p_offset must be congruent to p_vaddr modulo p_align
    align = load["align"] or 0x1000
    blob_off = len(out)
    pad = (blob_vaddr - blob_off) % align
    out += b"\x00" * pad
    blob_off += pad
    out += blob

    e_phoff, = struct.unpack_from("<I", retail, _E_PHOFF)
    e_phentsize, e_phnum = struct.unpack_from("<HH", retail, 0x2A)
    new_end = e_phoff + e_phentsize * (e_phnum + 1)
    first_content = min([p["offset"] for p in phdrs if p["offset"]]
                        + [len(retail)])
    if new_end > first_content:
        raise Elf32Error(
            f"no room to extend the phdr table in place "
            f"(0x{new_end:x} > 0x{first_content:x})")
    struct.pack_into("<8I", out, e_phoff + e_phentsize * e_phnum,
                     PT_LOAD, blob_off, blob_vaddr, blob_vaddr,
                     len(blob), len(blob), PF_RWX, align)
    struct.pack_into("<H", out, _E_PHNUM, e_phnum + 1)
    return bytes(out)


def verify_output(output, retail, patches):
    """Retail segment must match retail outside the declared patch ranges."""
    problems = []
    _phdrs, load = _load_phdr(retail)
    lo, span = load["offset"], load["filesz"]
    expected = bytearray(retail[lo:lo + span])
    for vaddr, data in patches.items():
        off = vaddr_to_offset(load, vaddr, len(data)) - lo
        expected[off:off + len(data)] = data
    got = output[lo:lo + span]
    if len(output) < lo + span:
        return [f"output truncated: {len(output):#x} < {lo + span:#x}"]
    if got != bytes(expected):
        first = next(i for i in range(span) if got[i] != expected[i])
        problems.append(
            f"retail segment diverges at file offset {lo + first:#x} "
            f"(vaddr 0x{load['vaddr'] + first:x}) outside declared patches")
    return problems
