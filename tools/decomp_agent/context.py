"""Compact target dossier: facts, classifications, hypotheses, unknowns.

``get_context`` assembles everything reliable about a single canonical function
into one JSON structure, deliberately separated by epistemic status:

  * ``facts``            -- committed-registry truth + mechanically derived data
                            (callees from the disassembly, referenced symbols)
  * ``classifications``  -- high-confidence structural reads (leaf, has-frame)
  * ``hypotheses``       -- weak signals worth a human/agent's attention
  * ``unknowns``         -- explicitly *not* known (prototype, type layout,
                            ownership) -- never guessed into a fact

The retail *target disassembly* is the ground truth for a function's own bytes.
It is summarised in every dossier (``ground_truth``) and, for matchable-size
functions, attached inline by default -- so an agent reconstructs C from the
authoritative instruction stream rather than guessing from an external reference
decompilation. Larger bodies are pointed to (``get_disassembly`` / the
``target-asm`` resource / ``detail=True``) to keep the response bounded.
"""
from __future__ import annotations

from . import schemas, evidence
from .project import DecompProject
from .registry import Registry
from .compiler_knowledge import knowledge_for_profile

RA_BIT = 0x80000000

# Attach the target disassembly inline by default up to this many listing lines.
# Above it, the dossier carries a pointer instead of the whole body (a caller
# fetches it explicitly with get_disassembly). Most matchable functions fit.
AUTO_ASM_MAX_LINES = 600

# Non-negotiable epistemics that every dossier repeats, so no client can start
# writing C without seeing them. The target disassembly outranks any external
# decompilation of another platform.
_REFERENCE_DECOMP_POLICY = (
    "A decompilation of another platform (e.g. a GameCube/PC port) is a HINT "
    "for names, call order and rough structure ONLY -- the lowest authority "
    "rank. Verify every branch, dispatch constant, immediate, struct offset and "
    "return path against the target disassembly below; never transcribe another "
    "platform's control flow. Different compiler + different CPU means different "
    "codegen."
)


def get_context(project: DecompProject, target, *, detail=False,
                include_asm=None, include_c=True) -> dict:
    reg = Registry(project)
    res = reg.resolve(target)
    if res.status == "ambiguous":
        return schemas.err("get_context", res.detail, code="ambiguous",
                           candidates=res.candidates)
    if res.status != "resolved" or res.record is None:
        return schemas.err("get_context", res.detail or "unresolved",
                           code="not_found")

    fn = res.record
    unit = reg.units.get(fn.unit_index)
    profile_name, profile_source = reg.effective_profile(fn.unit_index)
    profile = reg.profiles.get(profile_name, {})
    state = reg.state_of(fn.id_str())

    asm = evidence.target_asm_for(project, fn)
    callees = asm.get("callees", [])
    referenced = asm.get("referenced_symbols", [])

    # Ground truth: the retail disassembly. Attach inline by default when it is
    # present and small enough; otherwise the dossier carries only a pointer.
    asm_lines = asm.get("line_count") or 0
    auto_attach = asm.get("available", False) and asm_lines <= AUTO_ASM_MAX_LINES
    include_asm = (detail or auto_attach) if include_asm is None else include_asm

    # Callers: functions whose disassembly jal's this name (derivable only when
    # asm is present; otherwise explicitly unknown).
    callers = _derive_callers(project, fn) if detail else None

    nearby = _nearby(reg, fn)
    owned_data = reg.data_ranges_for_unit(fn.unit_index)

    facts = {
        "id": fn.id_str(),
        "name": fn.name,
        "address": fn.address,
        "address_hex": f"0x{fn.address:08x}",
        "extent": [fn.address, fn.end],
        "size": fn.size,
        "unit_index": fn.unit_index,
        "unit_name": unit.name if unit else None,
        "unit_path": unit.source_path if unit else None,
        "source_path": unit.source_path if unit else None,
        "manifest_path": reg.manifest_path_for(fn.unit_index),
        "state": state,
        "compiler_profile": profile_name,
        "compiler_profile_source": profile_source,
        "compiler_component": profile.get("compiler"),
        "compiler_flags": profile.get("flags", []),
        "frame_size": fn.frame_size,
        "reg_mask": f"0x{fn.reg_mask:08x}",
        "freg_mask": f"0x{fn.freg_mask:08x}",
        "saves_ra": bool(fn.reg_mask & RA_BIT),
        "line_range": [fn.line_low, fn.line_high],
        "direct_callees": callees,
        "referenced_symbols": referenced,
        "gp_relative_references": [s for s in referenced if s.startswith(("D_", "sym"))],
        "nearby_functions": nearby,
        "owns_data_ranges": len(owned_data),
        "name_is_duplicated": fn.name_is_duplicated,
    }

    classifications = []
    if not (fn.reg_mask & RA_BIT) and fn.frame_size == 0:
        classifications.append(schemas.classification(
            "control_flow", "high", ["reg_mask has no $ra, frame_size==0"]))
        classifications[-1]["label"] = "leaf function (no calls, no frame)"
    elif fn.reg_mask & RA_BIT:
        classifications.append(schemas.classification(
            "control_flow", "high", ["reg_mask saves $ra"]))
        classifications[-1]["label"] = "non-leaf function (makes calls)"

    hypotheses = []
    if owned_data:
        hypotheses.append({
            "label": "unit owns data ranges; data-from-C is unsupported",
            "confidence": "medium",
            "detail": f"unit owns {len(owned_data)} data range(s); a function "
                      f"touching them may hit initialized_local_sdata / rodata "
                      f"blockers"})
    if fn.name_is_duplicated:
        hypotheses.append({
            "label": "name duplicated across TUs (disambiguated as NAME__<vram>)",
            "confidence": "high",
            "detail": "always target this function by its canonical id"})

    # Ground-truth pointer + epistemic policy, present in every dossier so the
    # authoritative source can never be missed and a reference decompilation is
    # never silently promoted to authority.
    ground_truth = {
        "authority": "target_disassembly",
        "note": "The retail target disassembly is the ground truth for this "
                "function's bytes. Reconstruct C from it and re-diff; matching "
                "is decided by bytes, never by resemblance to any reference.",
        "reference_decomp_policy": _REFERENCE_DECOMP_POLICY,
        "target_disassembly_available": asm.get("available", False),
        "source": asm.get("source"),
        "line_count": asm.get("line_count"),
        "attached_inline": bool(include_asm and asm.get("available")),
        "how_to_read": "get_disassembly(<id>) or `cli disasm <id>`; also "
                       "get_context(detail=true) and the "
                       "decomp://function/<id>/target-asm resource.",
    }
    if not asm.get("available"):
        ground_truth["reason"] = asm.get("reason")

    unknowns = ["exact C prototype", "parameter/return types",
                "struct field offsets", "whether the retail body was "
                "hand-written asm (assembly_origin)"]
    # The first move is always to read ground truth, not to guess from a port.
    read_first = ("READ the target disassembly first (see ground_truth); it is "
                  "authoritative. Any reference decomp is a hint to verify "
                  "against it, not to transcribe.")
    unknowns.insert(0, read_first)

    fingerprint = _fingerprint(project)
    prev = _previous_session_summary(project, fn.id_str())

    dossier = schemas.ok(
        "get_context",
        function=fn.as_dict(),
        facts=facts,
        ground_truth=ground_truth,
        classifications=classifications,
        hypotheses=hypotheses,
        unknowns=unknowns,
        profile_fingerprint=fingerprint,
        compiler_knowledge=knowledge_for_profile(project, profile_name),
        previous_session=prev,
        known_blockers=_known_blockers(project, fn.id_str()),
    )

    if include_c:
        dossier["current_c"] = evidence.c_definition_for(project, fn)
    if include_asm:
        dossier["target_asm"] = asm
    if callers is not None:
        dossier["known_callers"] = callers
    return dossier


def get_disassembly(project: DecompProject, target) -> dict:
    """Return one function's retail target disassembly -- the ground truth.

    A cheap, first-class primitive so a client never has to hand-slice
    ``asm/text.s``: resolve the target, extract its listing, and return it in an
    envelope. When ``asm/`` is absent (fresh checkout) the envelope says so and
    points at ``configure.py``. This is derived, bounded to a single function
    extent, and textual (mnemonics/operands) -- not the raw ELF.
    """
    reg = Registry(project)
    res = reg.resolve(target)
    if res.status == "ambiguous":
        return schemas.err("get_disassembly", res.detail, code="ambiguous",
                           candidates=res.candidates)
    if res.status != "resolved" or res.record is None:
        return schemas.err("get_disassembly", res.detail or "unresolved",
                           code="not_found")
    fn = res.record
    asm = evidence.target_asm_for(project, fn)
    return schemas.ok(
        "get_disassembly",
        function_id=fn.id_str(),
        name=fn.name,
        address_hex=f"0x{fn.address:08x}",
        extent=[fn.address, fn.end],
        size=fn.size,
        available=asm.get("available", False),
        source=asm.get("source"),
        line_count=asm.get("line_count"),
        callees=asm.get("callees", []),
        referenced_symbols=asm.get("referenced_symbols", []),
        disassembly=asm.get("text"),
        reason=asm.get("reason"),
        note="Ground truth for these bytes. Reconstruct C from this; treat any "
             "cross-platform reference decomp as a hint to verify against it.",
    )


def _nearby(reg: Registry, fn, span=2):
    fns = reg.by_unit.get(fn.unit_index, [])
    fns = sorted(fns, key=lambda f: f.address)
    idx = next((i for i, f in enumerate(fns) if f.address == fn.address), None)
    if idx is None:
        return []
    out = []
    for f in fns[max(0, idx - span): idx + span + 1]:
        if f.address == fn.address:
            continue
        out.append({"id": f.id_str(), "name": f.name,
                    "state": reg.state_of(f.id_str()), "size": f.size})
    return out


def _derive_callers(project, fn):
    path = project.root / "asm" / "text.s"
    if not path.is_file():
        return None
    import re
    callers, cur = [], None
    pat = re.compile(r"\bjal\s+" + re.escape(fn.name) + r"\b")
    for line in path.read_text(errors="replace").splitlines():
        if line.startswith("glabel "):
            cur = line.split()[1]
        elif pat.search(line) and cur and cur not in callers and cur != fn.name:
            callers.append(cur)
    return callers


def _fingerprint(project):
    try:
        from . import _bridge
        if _bridge.repo_matches(project):
            cc = _bridge.import_tool("cc")
            return cc.profiles_fingerprint(project.version)
    except Exception:
        pass
    return None


def _previous_session_summary(project, function_id):
    try:
        from . import sessions
        return sessions.summary_for_function(project, function_id)
    except Exception:
        return None


def _known_blockers(project, function_id):
    try:
        from . import blockers
        return blockers.blockers_by_function(project).get(function_id, [])
    except Exception:
        return []
