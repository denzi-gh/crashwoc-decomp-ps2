"""Deterministic candidate selection and ranking.

``list_candidates`` returns a ranked list of functions worth attempting, using
*only* facts already in the committed registries plus recorded session/blocker
history -- no LLM, no embeddings, no vector database. Ranking is a pure function
of those inputs, so two clients (Claude Code, Codex) asking the same question
get the same order.

Cheap, reliable signals available without disassembly:

  * ``size``            -- gap to the next procedure (small is easier)
  * leaf-ness          -- the mdebug ``reg_mask`` tells us whether ``$ra`` is
                          saved; a function that saves no ``$ra`` and has a zero
                          frame is a leaf (no calls, no spills) -- ideal
  * unit momentum      -- a unit that already has ``matching`` functions has a
                          known-good profile and prototypes nearby
  * profile known      -- the effective compiler profile resolves
  * C translation unit -- the owning TU is a ``.c`` file, not hand-written asm

Anything requiring disassembly (jump tables, ``.lit8``, initialized-local
``.sdata``) is *not* inferred here; those are surfaced as recorded blockers once
an experiment proves them, which is the honest source.
"""
from __future__ import annotations

from . import schemas
from .project import DecompProject
from .registry import Registry, FunctionRecord

RA_BIT = 0x80000000


def _is_c_unit(unit) -> bool:
    return unit is not None and unit.mdebug_name.replace("\\", "/").lower().endswith(".c")


def _is_leaf(fn: FunctionRecord) -> bool:
    return not (fn.reg_mask & RA_BIT) and fn.frame_size == 0


def _risk(score_reasons, excluded) -> str:
    if excluded:
        return "excluded"
    tags = set(score_reasons)
    if "unknown-profile" in tags or "sdk-unit" in tags:
        return "high"
    if "leaf" in tags and "small" in tags and "c-unit" in tags:
        return "low"
    return "medium"


def _score(fn, unit, profile_known, unit_matching_count, state,
           has_blocker, exhausted):
    """(score:int, reasons:list, excluded:bool, exclude_reason). Deterministic."""
    reasons, score = [], 0
    excluded, exclude_reason = False, None

    if state == "matching":
        return 0, ["already-matching"], True, "already matching"
    if unit is None or not unit.owns_text:
        return 0, ["no-text-unit"], True, "unit owns no .text"
    mdebug = unit.mdebug_name.replace("\\", "/").lower()
    if mdebug.endswith((".s", ".asm")):
        return 0, ["asm-unit"], True, "translation unit is hand-written assembly"
    if not profile_known:
        return 0, ["unknown-profile"], True, "no known compiler profile"
    if has_blocker:
        return 0, ["architectural-blocker"], True, "recorded architectural blocker"
    if exhausted:
        return 0, ["exhausted-session"], True, "a prior session ended without a match"

    # size: smaller is better (cap the bonus so a 4-byte stub does not dominate).
    if fn.size <= 32:
        score += 40; reasons.append("small")
    elif fn.size <= 64:
        score += 30; reasons.append("small")
    elif fn.size <= 128:
        score += 20; reasons.append("modest-size")
    elif fn.size <= 256:
        score += 8
    else:
        score -= 10; reasons.append("large")

    if _is_leaf(fn):
        score += 35; reasons.append("leaf")
    elif not (fn.reg_mask & RA_BIT):
        score += 12; reasons.append("no-saved-ra")

    if fn.frame_size == 0:
        score += 8; reasons.append("zero-frame")
    elif fn.frame_size <= 64:
        score += 3

    if _is_c_unit(unit):
        score += 15; reasons.append("c-unit")
    if unit.category == "engine":
        score += 6; reasons.append("engine")
    elif unit.category == "game":
        score += 4; reasons.append("game")
    else:
        reasons.append("sdk-unit")

    if unit_matching_count > 0:
        score += 12; reasons.append("unit-momentum")

    if state == "equivalent":
        score += 5; reasons.append("equivalent-wip")

    return score, reasons, excluded, exclude_reason


def list_candidates(project: DecompProject, *, scope=None, profile=None,
                    category=None, max_size=None, require_c_unit=False,
                    exclude_known_blockers=True, prefer_leaf_functions=False,
                    include_matching=False, limit=20) -> dict:
    reg = Registry(project)

    # Recorded history (lazy; tolerate absence).
    blocker_ids, exhausted_ids = _history(project)

    # Per-unit matching counts for the momentum signal.
    unit_matching = {}
    for data in reg.manifests.values():
        idx = _unit_index(data)
        if idx is None:
            continue
        unit_matching[idx] = sum(1 for e in data.get("function", [])
                                 if e.get("state") == "matching")

    scope_unit = None
    if scope:
        res = reg.resolve(scope)
        if res.unit is not None:
            scope_unit = res.unit.index

    rows = []
    for fn in reg.functions:
        unit = reg.units.get(fn.unit_index)
        if scope_unit is not None and fn.unit_index != scope_unit:
            continue
        if category and (unit is None or unit.category != category):
            continue
        if max_size is not None and fn.size > max_size:
            continue
        if require_c_unit and not _is_c_unit(unit):
            continue

        prof_name, _prof_src = reg.effective_profile(fn.unit_index)
        profile_known = prof_name in reg.profiles
        if profile and prof_name != profile:
            continue

        state = reg.state_of(fn.id_str())
        has_blocker = exclude_known_blockers and fn.id_str() in blocker_ids
        exhausted = fn.id_str() in exhausted_ids

        score, reasons, excluded, exclude_reason = _score(
            fn, unit, profile_known, unit_matching.get(fn.unit_index, 0),
            state, has_blocker, exhausted)

        if excluded and not (include_matching and exclude_reason == "already matching"):
            continue
        if prefer_leaf_functions and "leaf" not in reasons:
            score -= 20

        rows.append({
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
            "profile": prof_name,
            "state": state,
            "risk": _risk(reasons, excluded),
            "score": score,
            "reasons": reasons,
            "known_blockers": sorted(blocker_ids.get(fn.id_str(), []))
            if isinstance(blocker_ids, dict) else [],
        })

    rows.sort(key=lambda r: (-r["score"], r["address"]))
    total = len(rows)
    if limit and limit > 0:
        rows = rows[:limit]

    return schemas.ok("list_candidates", version=project.version,
                      total_matched=total, returned=len(rows),
                      filters={"scope": scope, "profile": profile,
                               "category": category, "max_size": max_size,
                               "require_c_unit": require_c_unit,
                               "prefer_leaf_functions": prefer_leaf_functions},
                      candidates=rows)


def _unit_index(data):
    import re
    m = re.match(r"^[a-z0-9]+:unit-(\d+)$", str(data.get("unit", "")))
    return int(m.group(1)) if m else None


def _history(project):
    """(blockers_by_fid, exhausted_fids) from recorded state; empty on absence."""
    blockers_by_fid, exhausted = {}, set()
    try:
        from . import blockers
        blockers_by_fid = blockers.blockers_by_function(project)
    except Exception:
        blockers_by_fid = {}
    try:
        from . import sessions
        exhausted = sessions.exhausted_function_ids(project)
    except Exception:
        exhausted = set()
    return blockers_by_fid, exhausted
