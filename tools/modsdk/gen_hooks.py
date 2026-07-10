"""Hook trampolines: prologue patches + stub code.

  pre      T+0: j stub; nop   stub saves ra,a0-a3,t0-t3, calls the handler,
                              restores, replays the two displaced words,
                              resumes at T+8. Float arg regs are NOT saved.
  replace  T+0: j H; nop      plus an orig_<T> thunk so the handler can
                              still call the original.

Stubs sit at the start of each mod's blob slice, so orig_<name> addresses
exist before the mod is linked. Both displaced words must pass
mips.is_relocatable or the hook is refused.
"""
from dataclasses import dataclass

from . import mips
from .mips import A0, A1, A2, A3, RA, SP, T0, T1, T2, T3

PRE_STUB_SIZE = 0x70
REPLACE_THUNK_SIZE = 0x10

_SAVED = (A0, A1, A2, A3, T0, T1, T2, T3)
_FRAME = 0x50
_RA_SLOT = 0x40


@dataclass
class StubPlan:
    function: str
    handler: str
    mode: str
    stub_addr: int
    size: int


def plan_stubs(hooks, stub_base):
    """Returns (plans, total_size, {orig_<fn>: thunk addr})."""
    plans, provides = [], {}
    addr = stub_base
    for h in hooks:
        size = PRE_STUB_SIZE if h.mode == "pre" else REPLACE_THUNK_SIZE
        plans.append(StubPlan(function=h.function, handler=h.handler,
                              mode=h.mode, stub_addr=addr, size=size))
        if h.mode == "replace":
            provides[f"orig_{h.function}"] = addr
        addr += size
    return plans, addr - stub_base, provides


def _check_displaced(function, target, words):
    if target % 4:
        raise mips.HookSiteError(f"{function}: address 0x{target:x} unaligned")
    for i, w in enumerate(words):
        if not mips.is_relocatable(w):
            raise mips.HookSiteError(
                f"{function}: instruction {i} (0x{w:08x} at "
                f"0x{target + 4 * i:x}) is a branch/jump or otherwise "
                f"position-dependent; hook a different function")


def _check_reach(src, dst):
    if (src & 0xF0000000) != (dst & 0xF0000000):
        raise mips.HookSiteError(
            f"j from 0x{src:x} cannot reach 0x{dst:x} (256 MB region)")


def emit_stub(plan, target, displaced, handler_addr):
    _check_displaced(plan.function, target, displaced)
    resume = target + 8
    if plan.mode == "pre":
        words = [mips.addiu(SP, SP, -_FRAME),
                 mips.sd(RA, _RA_SLOT, SP)]
        words += [mips.sd(r, 8 * i, SP) for i, r in enumerate(_SAVED)]
        _check_reach(plan.stub_addr, handler_addr)
        words += [mips.jal(handler_addr), mips.NOP]
        words += [mips.ld(RA, _RA_SLOT, SP)]
        words += [mips.ld(r, 8 * i, SP) for i, r in enumerate(_SAVED)]
        words += [mips.addiu(SP, SP, _FRAME)]
        _check_reach(plan.stub_addr, resume)
        # displaced[1] rides the resume jump's delay slot
        words += [displaced[0], mips.j(resume), displaced[1]]
    else:
        _check_reach(plan.stub_addr, resume)
        words = [displaced[0], mips.j(resume), displaced[1], mips.NOP]
    assert 4 * len(words) <= plan.size, plan
    words += [mips.NOP] * (plan.size // 4 - len(words))
    return b"".join(w.to_bytes(4, "little") for w in words)


def emit_patch(plan, target, displaced, handler_addr):
    """The two words written over the hooked function's prologue."""
    _check_displaced(plan.function, target, displaced)
    if plan.mode == "pre":
        _check_reach(target, plan.stub_addr)
        entry = plan.stub_addr
    else:
        _check_reach(target, handler_addr)
        entry = handler_addr
    return (mips.j(entry).to_bytes(4, "little")
            + mips.NOP.to_bytes(4, "little"))
