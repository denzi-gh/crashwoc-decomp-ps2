"""ELF32 parsing and a MIPS code-reference decoder (stdlib only).

`read_elf` parses any little-endian ELF32 file (relocatable objects and the
linked retail executable alike) into sections, symbols and relocations.

`code_refs` recovers the data addresses the retail `.text` builds from
`lui`/`%lo` pairs and `$gp`-relative accesses, straight from the instruction
words -- no disassembler, so the data map can stay a pure function of the ELF.
The pairing follows the code's control flow closely enough to find every pair
splat resolves (tests/test_xref_decoder.py checks it against asm/text.s):

  * a `lui rX` value lives until anything writes `rX`, until a call clobbers
    the caller-saved registers (after its delay slot), or until the next
    function starts;
  * the state after a branch's delay slot flows to the branch target and
    joins the state already there; a register survives a join only if every
    path that holds it agrees (backward branches feed a repeat pass);
  * `addu`/`daddu`/`or` with one tracked operand carries the value along
    (`lui; addu rX, rX, idx; lw %lo(tbl)(rX)` indexes a table at `tbl`), and a
    move (`rt == $zero`) copies it;
  * every `addiu`/load/store whose base holds a tracked value yields one
    reference at `hi + sext(imm)`; `$gp`-based ones are exact (`gp + imm`).

The access width comes from the opcode (`lw`/`lwc1` 4, `ld` 8, `lq`/`lqc2` 16,
`addiu` 0 = address only; the unaligned `lwl`/`lwr` family counts 1 byte at
its own effective address, so a pair covers the whole word).
"""
import struct
from dataclasses import dataclass

SHF_ALLOC = 0x2
SHT_SYMTAB = 2
SHT_RELA = 4
SHT_NOBITS = 8
SHT_REL = 9


def _cstr(blob, offset):
    end = blob.index(b"\0", offset)
    return blob[offset:end].decode("latin-1")


def read_elf(data):
    """Parse a little-endian ELF32 file.

    Returns (sections, symbols, relocations):
      sections:    [{index, name, type, flags, addr, offset, size, link, info,
                     entsize, data}]
      symbols:     [{name, value, size, type, bind, shndx}]
      relocations: {target section index: [(offset, type, symbol index, addend)]}
    """
    if data[:4] != b"\x7fELF" or data[4] != 1 or data[5] != 1:
        raise ValueError("not a little-endian ELF32 object")
    e_shoff, = struct.unpack_from("<I", data, 0x20)
    e_shentsize, e_shnum, e_shstrndx = struct.unpack_from("<HHH", data, 0x2E)
    raw = []
    for i in range(e_shnum):
        raw.append(struct.unpack_from("<IIIIIIIIII", data, e_shoff + i * e_shentsize))
    shstr_off, shstr_size = raw[e_shstrndx][4], raw[e_shstrndx][5]
    shstr = data[shstr_off:shstr_off + shstr_size]

    sections = []
    for i, (name, stype, flags, addr, off, size, link, info, _al, entsize) in enumerate(raw):
        sections.append({
            "index": i, "name": _cstr(shstr, name), "type": stype,
            "flags": flags, "addr": addr, "offset": off, "size": size,
            "link": link, "info": info, "entsize": entsize,
            "data": b"" if stype == SHT_NOBITS else data[off:off + size]})

    symbols = []
    for sec in sections:
        if sec["type"] != SHT_SYMTAB:
            continue
        strtab = sections[sec["link"]]["data"]
        for j in range(0, len(sec["data"]), 16):
            name, value, size, info, _other, shndx = struct.unpack_from(
                "<IIIBBH", sec["data"], j)
            symbols.append({"name": _cstr(strtab, name), "value": value,
                            "size": size, "type": info & 0xF,
                            "bind": info >> 4, "shndx": shndx})

    relocations = {}
    for sec in sections:
        if sec["type"] not in (SHT_REL, SHT_RELA):
            continue
        step = 8 if sec["type"] == SHT_REL else 12
        entries = relocations.setdefault(sec["info"], [])
        for j in range(0, len(sec["data"]), step):
            r_offset, r_info = struct.unpack_from("<II", sec["data"], j)
            addend = (struct.unpack_from("<i", sec["data"], j + 8)[0]
                      if step == 12 else None)
            entries.append((r_offset, r_info & 0xFF, r_info >> 8, addend))
    return sections, symbols, relocations


# --------------------------------------------------------------------------- #
# MIPS (R5900) code-reference decoder                                         #
# --------------------------------------------------------------------------- #

GP = 28
RA = 31

# Primary opcode -> (mnemonic, access width, writes GPR rt).
LOADS = {
    0x20: ("lb", 1, True), 0x24: ("lbu", 1, True),
    0x21: ("lh", 2, True), 0x25: ("lhu", 2, True),
    0x23: ("lw", 4, True), 0x27: ("lwu", 4, True),
    0x22: ("lwl", 1, True), 0x26: ("lwr", 1, True),
    0x37: ("ld", 8, True), 0x1A: ("ldl", 1, True), 0x1B: ("ldr", 1, True),
    0x1E: ("lq", 16, True),
    0x31: ("lwc1", 4, False), 0x35: ("ldc1", 8, False),
    0x36: ("lqc2", 16, False),
}
STORES = {
    0x28: ("sb", 1), 0x29: ("sh", 2), 0x2B: ("sw", 4),
    0x2A: ("swl", 1), 0x2E: ("swr", 1),
    0x3F: ("sd", 8), 0x2C: ("sdl", 1), 0x2D: ("sdr", 1),
    0x1F: ("sq", 16),
    0x39: ("swc1", 4), 0x3D: ("sdc1", 8), 0x3E: ("sqc2", 16),
}
ADDRESS_OPS = {0x08: "addi", 0x09: "addiu", 0x18: "daddi", 0x19: "daddiu"}
# Other I-type ops that write rt.
WRITES_RT = {0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F}
LUI = 0x0F
SPECIAL, REGIMM, JAL, COP0, COP1, COP2, MMI = 0x00, 0x01, 0x03, 0x10, 0x11, 0x12, 0x1C
F_JALR, F_OR, F_ADDU, F_DADDU = 0x09, 0x25, 0x21, 0x2D
# Registers a call clobbers: $at, $v0-$v1, $a0-$a3, $t0-$t9, $ra.
CALLER_SAVED = frozenset([1, 2, 3, *range(4, 16), 24, 25, RA])


@dataclass(frozen=True)
class Ref:
    """One data reference the code builds."""
    vram: int        # address of the instruction carrying the low half
    address: int     # referenced address
    width: int       # access width in bytes (0 = address only)
    mnemonic: str
    base: str        # "hi" (lui pair) or "gp"
    hi_vram: int = None   # the lui's address for a "hi" pair


def _sext16(value):
    return value - 0x10000 if value & 0x8000 else value


def _branch(word, vram):
    """(target or None, likely, unconditional) for a control transfer, else
    None. Calls (jal/jalr/bal) return and are not branches here."""
    op = word >> 26
    rs, rt = (word >> 21) & 31, (word >> 16) & 31
    rel = vram + 4 + (_sext16(word & 0xFFFF) << 2)
    if op == 0x02:                                    # j
        return (vram & 0xF0000000) | ((word & 0x03FFFFFF) << 2), False, True
    if op == SPECIAL and (word & 0x3F) == 0x08:       # jr
        return None, False, True
    if op in (0x04, 0x05, 0x06, 0x07):                # beq bne blez bgtz
        return rel, False, op == 0x04 and rs == rt
    if op in (0x14, 0x15, 0x16, 0x17):                # the likely forms
        return rel, True, False
    if op == REGIMM and rt in (0x00, 0x01, 0x02, 0x03):
        return rel, rt >= 2, False                    # bltz bgez (+likely)
    if op in (COP0, COP1, COP2) and rs == 0x08:       # bc0/bc1/bc2 (t/f/l)
        return rel, bool(rt & 2), False
    return None


def _merge(states):
    """A register survives a join only if every state holding it agrees."""
    out, conflict = {}, set()
    for state in states:
        for reg, value in state.items():
            if reg not in out:
                out[reg] = value
            elif out[reg][0] != value[0]:
                conflict.add(reg)
    return {reg: value for reg, value in out.items() if reg not in conflict}


def code_refs(code, base_vram, func_starts, gp, max_passes=4):
    """[Ref] for `code` (little-endian words loaded at base_vram).

    `func_starts` are the function entry addresses; register state resets at
    each one. Inside a function, the state after a branch's delay slot flows
    to its target, where it joins the fall-through state (`_merge`); after an
    unconditional transfer the branches into the next address replace the
    fall-through state (with none, the straight-line state carries on, as in
    spimdisasm). Backward branches feed the next pass; decoding repeats until
    their states stop changing.
    """
    starts = set(func_starts)
    back = {}
    for _ in range(max_passes):
        refs, seen = _decode(code, base_vram, starts, gp, back)
        if seen == back:
            break
        back = seen
    return refs


def _decode(code, base_vram, starts, gp, back):
    """One linear pass; returns (refs, {backward target: [states]})."""
    refs = []
    seen = {}
    regs = {}                 # reg -> (hi << 16, lui vram)
    pending = {}              # forward target -> [states]
    clobber = False           # a call's delay slot is being decoded
    in_flight = None          # (target, likely, uncond, state before slot)
    dead = False              # the previous slot ended an unconditional jump
    for off in range(0, len(code) - 3, 4):
        vram = base_vram + off
        if vram in starts:
            regs, pending, clobber, in_flight, dead = {}, {}, False, None, False
        else:
            incoming = pending.pop(vram, []) + back.get(vram, [])
            if incoming:
                regs = _merge(incoming if dead else [regs, *incoming])
            dead = False
        word, = struct.unpack_from("<I", code, off)
        op = word >> 26
        rs = (word >> 21) & 31
        rt = (word >> 16) & 31
        rd = (word >> 11) & 31
        imm = _sext16(word & 0xFFFF)
        written = None
        is_call = False

        if op == LUI:
            if rt:
                regs[rt] = ((word & 0xFFFF) << 16, vram)
        elif op in LOADS or op in STORES or op in ADDRESS_OPS:
            if op in LOADS:
                mnem, width, writes = LOADS[op]
            elif op in STORES:
                (mnem, width), writes = STORES[op], False
            else:
                mnem, width, writes = ADDRESS_OPS[op], 0, True
            if rs == GP:
                refs.append(Ref(vram, (gp + imm) & 0xFFFFFFFF, width, mnem, "gp"))
            elif rs in regs:
                hi, lui_vram = regs[rs]
                refs.append(Ref(vram, (hi + imm) & 0xFFFFFFFF, width, mnem,
                                "hi", lui_vram))
            if writes:
                written = rt
        elif op in WRITES_RT:
            written = rt
        elif op == SPECIAL:
            funct = word & 0x3F
            if funct in (F_ADDU, F_DADDU, F_OR):
                a, b = rs in regs, rt in regs
                if a != b and rd:
                    regs[rd] = regs[rs if a else rt]
                else:
                    written = rd
            else:
                written = rd
                if funct == F_JALR:
                    is_call = True
        elif op == REGIMM:
            if rt in (0x10, 0x11, 0x12, 0x13):   # bltzal/bgezal(l): link
                is_call = True
        elif op == JAL:
            is_call = True
        elif op in (COP0, COP1, COP2):
            if rs in (0, 1, 2):                   # mfc/dmfc/cfc -> GPR rt
                written = rt
        elif op == MMI:
            written = rd

        if written is not None:
            regs.pop(written, None)
        if clobber:
            for reg in CALLER_SAVED:
                regs.pop(reg, None)
        clobber = is_call             # clobber once the delay slot is done

        if in_flight is not None:     # this was a branch's delay slot
            target, likely, uncond, before = in_flight
            in_flight = None
            if target is not None and target > vram:
                pending.setdefault(target, []).append(dict(regs))
            elif target is not None:
                seen.setdefault(target, []).append(dict(regs))
            if likely:
                regs = before         # not taken: the slot is annulled
            dead = uncond
        else:
            br = _branch(word, vram)
            if br is not None:
                in_flight = (*br, dict(regs))
    return refs, seen
