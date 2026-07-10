"""r5900 instruction words for hook stubs: a few encoders and a
conservative check for whether an instruction may be moved out of a
function prologue (rejects branches, jumps, and unknown opcodes)."""

ZERO, AT, V0, V1 = 0, 1, 2, 3
A0, A1, A2, A3, T0, T1, T2, T3 = 4, 5, 6, 7, 8, 9, 10, 11
SP, RA = 29, 31

NOP = 0x00000000


class HookSiteError(ValueError):
    pass


def _u16(imm):
    if not -0x8000 <= imm <= 0xFFFF:
        raise ValueError(f"immediate out of range: {imm:#x}")
    return imm & 0xFFFF


def j(target):
    return 0x08000000 | ((target >> 2) & 0x03FFFFFF)


def jal(target):
    return 0x0C000000 | ((target >> 2) & 0x03FFFFFF)


def jr_ra():
    return 0x03E00008


def addiu(rt, rs, imm):
    return (0x09 << 26) | (rs << 21) | (rt << 16) | _u16(imm)


def sd(rt, offset, base):
    return (0x3F << 26) | (base << 21) | (rt << 16) | _u16(offset)


def ld(rt, offset, base):
    return (0x37 << 26) | (base << 21) | (rt << 16) | _u16(offset)


def is_relocatable(word):
    """True if the instruction behaves identically at a different PC."""
    if word == NOP:
        return True
    op = word >> 26
    if op == 0:                                   # SPECIAL
        fn = word & 0x3F
        return fn not in (0x08, 0x09, 0x0C, 0x0D)  # jr, jalr, syscall, break
    if op == 1:                                   # REGIMM
        rt = (word >> 16) & 0x1F
        return 0x08 <= rt <= 0x0E                  # traps ok, branches not
    if op in (0x02, 0x03):                        # j, jal
        return False
    if op in (0x04, 0x05, 0x06, 0x07,             # beq..bgtz
              0x14, 0x15, 0x16, 0x17):            # branch-likely
        return False
    if op in (0x10, 0x11, 0x12):                  # COP0/1/2
        rs = (word >> 21) & 0x1F
        if rs == 0x08:                            # BCzT/BCzF
            return False
        if op == 0x10 and rs >= 0x10:             # eret/tlb*/ei/di
            return False
        return True
    if op in (0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
              0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1E, 0x1F,
              0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
              0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
              0x30, 0x31, 0x33, 0x36, 0x37, 0x39, 0x3A, 0x3E, 0x3F):
        return True                                # ALU imm, loads/stores, MMI
    return False
