"""Mismatch classification.

Turns the structured facts a ``compile_diff`` gathered (compile status,
unresolved symbols, frame/saved-register comparison against the mdebug registry,
and per-word mismatch windows) into a list of categorised classifications with
honest confidence. Proven facts (compile failed, symbol unresolved, frame size
differs from the mdebug record) are marked ``proven``; anything read from a
single differing instruction word is a heuristic and never exceeds ``medium``.
"""
from __future__ import annotations

from . import schemas

RA_BIT = 0x80000000

# MIPS primary opcodes we recognise for coarse window classification.
_BRANCH_OPS = {0x04, 0x05, 0x06, 0x07, 0x01}     # beq bne blez bgtz regimm
_LOAD_OPS = {0x20, 0x21, 0x23, 0x24, 0x25, 0x37, 0x1F}
_STORE_OPS = {0x28, 0x29, 0x2B, 0x3F}
_IMM_OPS = {0x08, 0x09, 0x0C, 0x0D, 0x0E, 0x0A, 0x0B, 0x0F, 0x24, 0x25}


def _fields(word):
    return {
        "op": (word >> 26) & 0x3F,
        "rs": (word >> 21) & 0x1F,
        "rt": (word >> 16) & 0x1F,
        "rd": (word >> 11) & 0x1F,
        "funct": word & 0x3F,
        "imm": word & 0xFFFF,
    }


def _window_category(tw, cw):
    """Coarse category for one differing (target, candidate) instruction word."""
    if tw == 0 or cw == 0:
        return "delay_slot", "low"
    t, c = _fields(tw), _fields(cw)
    if t["op"] != c["op"]:
        if t["op"] in _BRANCH_OPS or c["op"] in _BRANCH_OPS:
            return "control_flow", "medium"
        if (t["op"] in _LOAD_OPS) != (c["op"] in _LOAD_OPS) or \
                (t["op"] in _STORE_OPS) != (c["op"] in _STORE_OPS):
            return "load_store_width", "low"
        return "instruction_order", "low"
    # Same opcode.
    if t["op"] in _BRANCH_OPS and t["imm"] != c["imm"]:
        return "branch_direction", "medium"
    if t["op"] in _IMM_OPS and t["imm"] != c["imm"] and t["rs"] == c["rs"]:
        return "immediate_materialization", "medium"
    if (t["rs"], t["rt"], t["rd"]) != (c["rs"], c["rt"], c["rd"]):
        return "register_allocation", "medium"
    return "instruction_scheduling", "low"


def classify(*, compile_ok, compile_error=None, unresolved=None,
             target_fn, candidate_frame=None, candidate_reg_mask=None,
             candidate_freg_mask=None, candidate_size=None,
             windows=None, hybrid_error=None, exact=False):
    """Return a list of classification records. ``windows`` is a list of
    (offset, target_word, candidate_word) triples for differing 4-byte slots."""
    out = []
    if exact:
        return out

    if compile_ok is False:
        out.append(_c("compile_error", "proven",
                      [compile_error or "compiler returned non-zero"]))
        return out

    if unresolved:
        out.append(_c("unresolved_symbol", "proven",
                      [f"unresolved: {', '.join(unresolved[:6])}"]))

    if hybrid_error:
        cat = _hybrid_error_category(hybrid_error)
        out.append(_c(cat, "high", [hybrid_error]))
        return out

    # Frame / saved-register comparison against the authoritative mdebug record.
    if candidate_frame is not None and candidate_frame != target_fn.frame_size:
        out.append(_c("stack_frame", "proven",
                      [f"frame {candidate_frame} != mdebug {target_fn.frame_size}"]))
    if candidate_reg_mask is not None and candidate_reg_mask != target_fn.reg_mask:
        out.append(_c("saved_registers", "proven",
                      [f"reg_mask 0x{candidate_reg_mask:08x} != "
                       f"mdebug 0x{target_fn.reg_mask:08x}"]))
    if candidate_freg_mask is not None and candidate_freg_mask != target_fn.freg_mask:
        out.append(_c("saved_registers", "high",
                      [f"freg_mask 0x{candidate_freg_mask:08x} != "
                       f"mdebug 0x{target_fn.freg_mask:08x}"]))

    if candidate_size is not None and candidate_size != target_fn.size:
        conf = "proven"
        cat = "size_difference"
        if candidate_size > target_fn.size:
            cat = "inlining" if candidate_size > target_fn.size + 32 else "size_difference"
            conf = "medium" if cat == "inlining" else "proven"
        out.append(_c(cat, conf,
                      [f"candidate {candidate_size} bytes vs extent "
                       f"{target_fn.size}"]))

    # Per-window instruction reads (heuristic).
    seen = {}
    for off, tw, cw in (windows or []):
        cat, conf = _window_category(tw, cw)
        key = (cat, conf)
        rec = seen.get(key)
        if rec is None:
            rec = _c(cat, conf, [], window=[off, off + 4])
            seen[key] = rec
            out.append(rec)
        rec["evidence"].append(
            f"@0x{off:x}: target=0x{tw:08x} candidate=0x{cw:08x}")
        rec["window"] = [min(rec["window"][0], off), max(rec["window"][1], off + 4)]

    for rec in out:
        rec["evidence"] = rec["evidence"][:6]

    if not out:
        out.append(_c("unknown", "low", ["bytes differ but no signal matched"]))
    return out


def _hybrid_error_category(msg: str) -> str:
    m = msg.lower()
    if "li.d" in m or ".lit8" in m:
        return "unsupported_lit8"
    # Read-only data (`.rdata`/`.rodata`) is most often a switch jump table in
    # this codebase; check it before the generic .sdata/.data case.
    if ".rdata" in m or ".rodata" in m or "rodata" in m or "jump table" in m:
        return "jump_table_rodata"
    if "data section" in m or ".sdata" in m or ".data" in m:
        return "initialized_local_sdata"
    if ".org backwards" in m or "moving .org" in m:
        return "size_difference"
    if "unresolved" in m:
        return "unresolved_symbol"
    if ".lit4" in m or "gp reference" in m:
        return "literal_pool"
    return "likely_compiler_drift"


def _c(category, confidence, evidence, window=None):
    return schemas.classification(category, confidence, evidence, window=window)
