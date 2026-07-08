"""Structured blocker records.

A blocker captures *why* a function cannot currently match, so the same finding
is not rediscovered every session and so candidate ranking can skip functions
with a proven architectural obstacle. Records are JSON under
``build/<version>/agent_blockers/`` (gitignored), written atomically.

A blocker is either ``temporary`` (tooling/context gap that may clear) or
``architectural`` (a fundamental obstacle -- compiler-owned data, .lit8, etc.).
Only architectural blockers exclude a function from candidate ranking.
"""
from __future__ import annotations

import json
import os
import re
import secrets
import tempfile
import time
from pathlib import Path

from . import schemas
from .registry import Registry

ARCHITECTURAL_KINDS = {
    "compiler_owned_rodata", "compiler_owned_sdata", "unsupported_lit8",
    "missing_type_layout", "assembly_origin", "unit_level_coupling",
    "compiler_patchlevel_mismatch",
}


def _dir(project) -> Path:
    d = project.root / "build" / project.version / "agent_blockers"
    d.mkdir(parents=True, exist_ok=True)
    return d


def _atomic_write(path: Path, data: dict) -> None:
    fd, tmp = tempfile.mkstemp(dir=str(path.parent), suffix=".tmp")
    try:
        with os.fdopen(fd, "w", encoding="utf-8") as f:
            json.dump(data, f, indent=2)
            f.write("\n")
        os.replace(tmp, path)
    finally:
        if os.path.exists(tmp):
            os.remove(tmp)


def record_blocker(project, target, *, kind, confidence="medium", evidence=None,
                   best_attempt=None, remaining_mismatch=None,
                   architectural=None, recommended_human_action=None,
                   recommended_tooling_action=None, session_id=None,
                   client=None) -> dict:
    reg = Registry(project)
    try:
        fn = reg.require_function(target)
    except ValueError as exc:
        return schemas.err("record_blocker", str(exc), code="unresolved")
    if kind not in schemas.BLOCKER_KINDS:
        return schemas.err("record_blocker", f"unknown blocker kind {kind!r}",
                           code="bad_kind", allowed=list(schemas.BLOCKER_KINDS))
    if confidence not in schemas.CONFIDENCE:
        confidence = "medium"
    if architectural is None:
        architectural = kind in ARCHITECTURAL_KINDS

    record = {
        "schema": 1,
        "function_id": fn.id_str(),
        "kind": kind,
        "confidence": confidence,
        "temporary": not architectural,
        "architectural": bool(architectural),
        "evidence": list(evidence or []),
        "best_attempt": best_attempt,
        "remaining_mismatch": remaining_mismatch,
        "recommended_human_action": recommended_human_action,
        "recommended_tooling_action": recommended_tooling_action,
        "session_id": session_id,
        "client": client,
        "timestamp": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
    }
    fid_slug = re.sub(r"[^A-Za-z0-9]", "_", fn.id_str())
    path = _dir(project) / f"{fid_slug}-{secrets.token_hex(3)}.json"
    _atomic_write(path, record)
    return schemas.ok("record_blocker", blocker=record,
                      path=path.relative_to(project.root).as_posix())


def list_blockers(project, target=None) -> dict:
    reg = Registry(project)
    fid = None
    if target is not None:
        try:
            fid = reg.require_function(target).id_str()
        except ValueError as exc:
            return schemas.err("list_blockers", str(exc), code="unresolved")
    out = []
    for rec in _iter_records(project):
        if fid is None or rec.get("function_id") == fid:
            out.append(rec)
    return schemas.ok("list_blockers", count=len(out), blockers=out)


def blockers_by_function(project, architectural_only=True) -> dict:
    out = {}
    for rec in _iter_records(project):
        if architectural_only and not rec.get("architectural"):
            continue
        out.setdefault(rec.get("function_id"), []).append(rec.get("kind"))
    return out


def search_blockers(project, query, limit, hit_builder):
    hits, ql = [], query.lower()
    for rec in _iter_records(project):
        blob = json.dumps(rec).lower()
        if ql in blob:
            hits.append(hit_builder(
                "build/%s/agent_blockers" % project.version,
                rec.get("kind"), "blocker",
                f"{rec.get('function_id')}: {rec.get('kind')} "
                f"({'architectural' if rec.get('architectural') else 'temporary'})",
                rec.get("function_id")))
            if len(hits) >= limit:
                break
    return hits


def _iter_records(project):
    d = project.root / "build" / project.version / "agent_blockers"
    if not d.is_dir():
        return
    for path in sorted(d.glob("*.json")):
        try:
            yield json.loads(path.read_text())
        except (OSError, json.JSONDecodeError):
            continue
