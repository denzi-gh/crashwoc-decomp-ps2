"""Shared, cross-client experiment sessions.

A session is a durable ledger of every attempt at matching one function, stored
as JSON under ``build/<version>/agent_sessions/`` (gitignored). It is
independent of any conversation: Claude Code can start a session and Codex can
resume it (or the reverse), because all state lives in the file, not in a chat
history.

Guarantees:

  * **atomic writes** -- every ledger update writes a temp file and
    ``os.replace``s it, so an interrupted write never leaves a partial JSON.
  * **single writer** -- an ``O_EXCL`` lock file names the owning client; a
    second writer is refused rather than silently stealing the lock. Stale
    locks (older than a TTL) are detectable and recoverable only through an
    explicit call.
  * **repeat / oscillation detection** -- candidate-assembly hashes and
    diff-signature hashes seen before are recognised, and non-improvement /
    consecutive-compile-failure counters drive the documented stop policy.

Client/model metadata on a checkpoint is diagnostic only; it never affects
verification, ranking or promotion.
"""
from __future__ import annotations

import json
import os
import secrets
import subprocess
import tempfile
import time
from pathlib import Path

from . import schemas
from .registry import Registry

LOCK_TTL_SECONDS = 2 * 60 * 60      # 2h: a lock older than this is "stale"

# Outcomes that mark a function's session as exhausted for candidate ranking.
_EXHAUSTED_OUTCOMES = {
    "NO_PROGRESS", "BUDGET_EXHAUSTED", "BLOCKED_TOOLING",
    "BLOCKED_MISSING_CONTEXT",
}


class SessionError(Exception):
    pass


class LockError(SessionError):
    pass


# --- low-level storage ------------------------------------------------------


def _sessions_dir(project) -> Path:
    d = project.sessions_dir
    d.mkdir(parents=True, exist_ok=True)
    return d


def _ledger_path(project, session_id) -> Path:
    if not _SAFE_ID(session_id):
        raise SessionError(f"invalid session id: {session_id!r}")
    return _sessions_dir(project) / f"{session_id}.json"


def _lock_path(project, session_id) -> Path:
    return _sessions_dir(project) / f"{session_id}.lock"


def _SAFE_ID(session_id) -> bool:
    import re
    return isinstance(session_id, str) and bool(re.fullmatch(r"[A-Za-z0-9_-]{6,64}", session_id))


def _atomic_write(path: Path, data: dict) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    fd, tmp = tempfile.mkstemp(dir=str(path.parent), suffix=".tmp")
    try:
        with os.fdopen(fd, "w", encoding="utf-8") as f:
            json.dump(data, f, indent=2, sort_keys=False)
            f.write("\n")
            f.flush()
            os.fsync(f.fileno())
        os.replace(tmp, path)
    finally:
        if os.path.exists(tmp):
            os.remove(tmp)


def _read_ledger(project, session_id) -> dict:
    path = _ledger_path(project, session_id)
    if not path.is_file():
        raise SessionError(f"no such session: {session_id}")
    return json.loads(path.read_text())


# --- locking ----------------------------------------------------------------


def _acquire_lock(project, session_id, client) -> dict:
    lock = _lock_path(project, session_id)
    info = {"client": client or "unknown", "pid": os.getpid(),
            "acquired": time.time(), "host": _host()}
    try:
        fd = os.open(str(lock), os.O_CREAT | os.O_EXCL | os.O_WRONLY)
    except FileExistsError:
        existing = _read_lock(lock)
        age = time.time() - existing.get("acquired", 0)
        raise LockError(
            f"session {session_id} is locked by "
            f"{existing.get('client', '?')} (age {age:.0f}s); "
            + ("stale -- recover with recover_lock" if age > LOCK_TTL_SECONDS
               else "still active"))
    with os.fdopen(fd, "w", encoding="utf-8") as f:
        json.dump(info, f)
    return info


def _read_lock(lock: Path) -> dict:
    try:
        return json.loads(lock.read_text())
    except (OSError, json.JSONDecodeError):
        return {}


def _release_lock(project, session_id) -> None:
    lock = _lock_path(project, session_id)
    try:
        lock.unlink()
    except OSError:
        pass


def _require_lock(project, session_id, client):
    """Ensure ``client`` may write: it must hold the lock (or take a free one)."""
    lock = _lock_path(project, session_id)
    if not lock.exists():
        return _acquire_lock(project, session_id, client)
    existing = _read_lock(lock)
    age = time.time() - existing.get("acquired", 0)
    if age > LOCK_TTL_SECONDS:
        raise LockError(f"session {session_id} lock is stale "
                        f"({age:.0f}s); call recover_lock to take it over")
    # Same client identity + host may continue.
    if existing.get("client") == (client or "unknown") and existing.get("host") == _host():
        return existing
    raise LockError(f"session {session_id} is owned by "
                    f"{existing.get('client', '?')}@{existing.get('host', '?')}; "
                    f"{client!r} may not write")


def recover_lock(project, session_id, client=None) -> dict:
    """Explicitly break a *stale* lock and take ownership. Refuses a fresh lock."""
    lock = _lock_path(project, session_id)
    if lock.exists():
        existing = _read_lock(lock)
        age = time.time() - existing.get("acquired", 0)
        if age <= LOCK_TTL_SECONDS:
            return schemas.err("recover_lock",
                               f"lock is not stale (age {age:.0f}s <= "
                               f"{LOCK_TTL_SECONDS}s); refusing to steal",
                               code="lock_active")
        lock.unlink()
    info = _acquire_lock(project, session_id, client)
    _release_lock  # noqa: keep import usage explicit
    return schemas.ok("recover_lock", session_id=session_id, lock=info)


# --- helpers ----------------------------------------------------------------


def _host() -> str:
    import socket
    try:
        return socket.gethostname()
    except OSError:
        return "unknown"


def _git_revision(project) -> str:
    try:
        out = subprocess.run(["git", "rev-parse", "HEAD"], cwd=str(project.root),
                             capture_output=True, text=True, timeout=15)
        if out.returncode == 0:
            return out.stdout.strip()
    except (OSError, subprocess.SubprocessError):
        pass
    return None


def _source_hash(project, unit_source_path) -> str:
    import hashlib
    path = project.root / unit_source_path
    if not path.is_file():
        return None
    return hashlib.sha256(path.read_bytes()).hexdigest()


def _fingerprint(project):
    try:
        from . import _bridge
        if _bridge.repo_matches(project):
            return _bridge.import_tool("cc").profiles_fingerprint(project.version)
    except Exception:
        pass
    return None


# --- public API -------------------------------------------------------------


def start_session(project, target, *, client=None, model=None,
                  reasoning_effort=None, initial_budget=None,
                  hard_budget=None) -> dict:
    reg = Registry(project)
    try:
        fn = reg.require_function(target)
    except ValueError as exc:
        return schemas.err("start_session", str(exc), code="unresolved")

    unit = reg.units.get(fn.unit_index)
    profile, _src = reg.effective_profile(fn.unit_index)
    session_id = "s-" + time.strftime("%Y%m%d-%H%M%S") + "-" + secrets.token_hex(3)
    src_hash = _source_hash(project, unit.source_path) if unit else None

    ledger = {
        "schema": schemas.SESSION_SCHEMA,
        "session_id": session_id,
        "target_function_id": fn.id_str(),
        "target_name": fn.name,
        "unit_index": fn.unit_index,
        "unit_source": unit.source_path if unit else None,
        "manifest_path": reg.manifest_path_for(fn.unit_index),
        "repo_root": str(project.root),
        "repo_revision": _git_revision(project),
        "created": _now(),
        "starting_source_hash": src_hash,
        "current_source_hash": src_hash,
        "target_size": fn.size,
        "compiler_profile": profile,
        "profile_fingerprint": _fingerprint(project),
        "initial_budget": initial_budget or schemas.DEFAULT_INITIAL_BUDGET,
        "hard_budget": hard_budget or schemas.DEFAULT_HARD_BUDGET,
        "attempt_count": 0,
        "best_attempt": None,
        "best_source_snapshot": None,
        "non_improvement_count": 0,
        "compile_failure_count": 0,
        "blocker_status": None,
        "outcome": None,
        "active_client": {"client": client, "model": model,
                          "reasoning_effort": reasoning_effort},
        "attempts": [],
        "seen_asm_hashes": [],
        "seen_diff_signatures": [],
        "hypotheses": [],
        "mismatch_categories": [],
        "token_usage": _empty_tokens(),
    }
    _acquire_lock(project, session_id, client)
    _atomic_write(_ledger_path(project, session_id), ledger)
    return schemas.ok("start_session", session=_summary(ledger),
                      session_id=session_id)


def checkpoint_session(project, session_id, *, kind="experiment",
                       hypothesis=None, expected_effect=None,
                       source_hash=None, source_snapshot=None,
                       candidate_asm_hash=None, target_hash=None,
                       diff_signature=None, fuzzy_score=None,
                       matching_bytes=None, differing_bytes=None,
                       classifications=None, compile_ok=None, exact=None,
                       parent=None, client=None, model=None,
                       reasoning_effort=None, input_tokens=None,
                       output_tokens=None) -> dict:
    """Record one experiment; update counters and repeat/oscillation flags.

    ``input_tokens`` / ``output_tokens`` are optional client-reported
    diagnostics; they are accumulated per client and per model but never affect
    verification, ranking or promotion.
    """
    try:
        _require_lock(project, session_id, client)
    except LockError as exc:
        return schemas.err("checkpoint_session", str(exc), code="locked")

    ledger = _read_ledger(project, session_id)
    n = ledger["attempt_count"] + 1

    repeated_asm = bool(candidate_asm_hash) and candidate_asm_hash in ledger["seen_asm_hashes"]
    seen_sigs = ledger["seen_diff_signatures"]
    repeated_sig = bool(diff_signature) and diff_signature in seen_sigs
    # Oscillation: a signature reappears that was not the immediately previous.
    oscillation = repeated_sig and (not seen_sigs or seen_sigs[-1] != diff_signature)

    # Best/improvement tracking.
    improved, became_best = False, False
    if compile_ok is False:
        ledger["compile_failure_count"] += 1
    else:
        ledger["compile_failure_count"] = 0
        score = _score_value(exact, fuzzy_score, matching_bytes)
        best_score = ledger.get("_best_score")
        if best_score is None or score > best_score:
            improved = became_best = True
            ledger["_best_score"] = score
            ledger["best_attempt"] = n
            if source_snapshot is not None:
                ledger["best_source_snapshot"] = source_snapshot
            ledger["best_source_hash"] = source_hash

    if improved:
        ledger["non_improvement_count"] = 0
    elif compile_ok is not False:
        ledger["non_improvement_count"] += 1

    record = {
        "attempt": n, "parent": parent, "kind": kind, "timestamp": _now(),
        "client": client, "model": model, "reasoning_effort": reasoning_effort,
        "hypothesis": hypothesis, "expected_effect": expected_effect,
        "source_hash": source_hash, "candidate_asm_hash": candidate_asm_hash,
        "target_hash": target_hash, "diff_signature": diff_signature,
        "fuzzy_score": fuzzy_score, "matching_bytes": matching_bytes,
        "differing_bytes": differing_bytes,
        "classifications": classifications or [],
        "compile_ok": compile_ok, "exact": exact,
        "improved": improved, "became_best": became_best,
        "repeated_asm_hash": repeated_asm, "repeated_diff_signature": repeated_sig,
        "oscillation": oscillation,
        "input_tokens": input_tokens, "output_tokens": output_tokens,
    }
    _accumulate_tokens(ledger, input_tokens, output_tokens, client, model)
    ledger["attempts"].append(record)
    ledger["attempt_count"] = n
    if candidate_asm_hash and not repeated_asm:
        ledger["seen_asm_hashes"].append(candidate_asm_hash)
    if diff_signature:
        seen_sigs.append(diff_signature)
    if source_hash:
        ledger["current_source_hash"] = source_hash
    if hypothesis:
        ledger["hypotheses"].append({"attempt": n, "text": hypothesis})
    for c in (classifications or []):
        cat = c.get("category")
        if cat and cat not in ledger["mismatch_categories"]:
            ledger["mismatch_categories"].append(cat)
    if exact:
        ledger["outcome"] = "MATCHING"

    _atomic_write(_ledger_path(project, session_id), ledger)
    stop = evaluate_stop(ledger)
    return schemas.ok("checkpoint_session", attempt=record,
                      counters=_counters(ledger), stop=stop,
                      token_usage=ledger["token_usage"])


def resume_session(project, session_id, *, client=None) -> dict:
    """Return everything a different client/model needs to continue safely."""
    try:
        ledger = _read_ledger(project, session_id)
    except SessionError as exc:
        return schemas.err("resume_session", str(exc), code="not_found")

    reg = Registry(project)
    conflicts = []
    # Safety checks.
    if ledger.get("repo_revision") and _git_revision(project) != ledger["repo_revision"]:
        conflicts.append("repo_revision_changed")
    if ledger.get("profile_fingerprint") and _fingerprint(project) not in \
            (None, ledger["profile_fingerprint"]):
        conflicts.append("profile_fingerprint_changed")
    # Target still present & manifest unchanged shape.
    if reg.by_id.get(ledger["target_function_id"]) is None:
        conflicts.append("target_missing")
    cur_hash = _source_hash(project, ledger.get("unit_source")) if ledger.get("unit_source") else None
    unrelated_edit = (cur_hash is not None
                      and cur_hash != ledger.get("current_source_hash")
                      and cur_hash != ledger.get("starting_source_hash"))
    if unrelated_edit:
        conflicts.append("source_changed_since_last_checkpoint")

    lock = _read_lock(_lock_path(project, session_id))
    lock_age = time.time() - lock.get("acquired", 0) if lock else None
    lock_active = bool(lock) and lock_age is not None and lock_age <= LOCK_TTL_SECONDS
    lock_foreign = lock_active and (lock.get("client") != (client or "unknown")
                                    or lock.get("host") != _host())

    safe = not conflicts and not lock_foreign
    payload = {
        "session_id": session_id,
        "safe_to_resume": safe,
        "conflicts": conflicts,
        "lock": {"present": bool(lock), "active": lock_active,
                 "foreign": lock_foreign, "owner": lock.get("client"),
                 "age_seconds": lock_age},
        "target_summary": {
            "id": ledger["target_function_id"], "name": ledger.get("target_name"),
            "size": ledger.get("target_size"), "profile": ledger.get("compiler_profile"),
            "unit_source": ledger.get("unit_source")},
        "current_source_hash": cur_hash,
        "starting_source_hash": ledger.get("starting_source_hash"),
        "best_attempt": ledger.get("best_attempt"),
        "best_source_snapshot": ledger.get("best_source_snapshot"),
        "hypotheses": ledger.get("hypotheses", []),
        "seen_asm_hashes": ledger.get("seen_asm_hashes", []),
        "seen_diff_signatures": ledger.get("seen_diff_signatures", []),
        "mismatch_categories": ledger.get("mismatch_categories", []),
        "failed_experiments": [a for a in ledger.get("attempts", [])
                               if a.get("compile_ok") is False],
        "blocker_status": ledger.get("blocker_status"),
        "remaining_budget": _remaining_budget(ledger),
        "repo_revision": ledger.get("repo_revision"),
        "profile_fingerprint": ledger.get("profile_fingerprint"),
        "token_usage": ledger.get("token_usage", _empty_tokens()),
        "escalation": _escalation(ledger),
        "counters": _counters(ledger),
        "stop": evaluate_stop(ledger),
    }
    if not safe:
        return schemas.err("resume_session",
                           "unsafe to resume: " + ", ".join(conflicts + (
                               ["lock_foreign"] if lock_foreign else [])),
                           code="unsafe_resume", **payload)
    return schemas.ok("resume_session", **payload)


def end_session(project, session_id, *, outcome=None, client=None,
                blocker_status=None) -> dict:
    try:
        _require_lock(project, session_id, client)
    except LockError as exc:
        return schemas.err("end_session", str(exc), code="locked")
    ledger = _read_ledger(project, session_id)
    if outcome and outcome not in schemas.OUTCOMES:
        return schemas.err("end_session", f"unknown outcome {outcome!r}",
                           code="bad_outcome")
    ledger["outcome"] = outcome or ledger.get("outcome") or "NO_PROGRESS"
    if blocker_status:
        ledger["blocker_status"] = blocker_status
    ledger["ended"] = _now()
    _atomic_write(_ledger_path(project, session_id), ledger)
    _release_lock(project, session_id)
    return schemas.ok("end_session", session=_summary(ledger),
                      token_usage=ledger.get("token_usage", _empty_tokens()))


def get_session(project, session_id) -> dict:
    try:
        ledger = _read_ledger(project, session_id)
    except SessionError as exc:
        return schemas.err("get_session", str(exc), code="not_found")
    return schemas.ok("get_session", session=ledger, stop=evaluate_stop(ledger))


def list_sessions(project) -> dict:
    out = []
    d = project.sessions_dir
    if d.is_dir():
        for path in sorted(d.glob("*.json")):
            try:
                out.append(_summary(json.loads(path.read_text())))
            except (OSError, json.JSONDecodeError):
                continue
    return schemas.ok("list_sessions", count=len(out), sessions=out)


def restore_best_candidate(project, session_id, *, expected_source_hash=None,
                           client=None, force=False) -> dict:
    """Write the best attempt's source snapshot back to the unit file, guarded.

    Refuses to overwrite when the current file differs from the session's last
    known hash unless ``force`` -- returns a conflict instead of clobbering an
    unrelated edit. Touches only the target unit's source file.
    """
    ledger = _read_ledger(project, session_id)
    snapshot = ledger.get("best_source_snapshot")
    unit_source = ledger.get("unit_source")
    if snapshot is None:
        return schemas.err("restore_best_candidate",
                           "no best-source snapshot recorded", code="no_snapshot")
    if not unit_source:
        return schemas.err("restore_best_candidate", "session has no unit source",
                           code="no_source")
    try:
        target = project.path(*unit_source.split("/"))
    except Exception as exc:
        return schemas.err("restore_best_candidate", str(exc), code="bad_path")

    cur = _source_hash(project, unit_source)
    guard = expected_source_hash or ledger.get("current_source_hash")
    if not force and cur is not None and guard is not None and cur != guard:
        return schemas.err(
            "restore_best_candidate",
            "current source differs from the session's last known state; "
            "refusing to overwrite (pass force after reconciling)",
            code="source_conflict",
            current_source_hash=cur, expected_source_hash=guard)

    target.write_text(snapshot, encoding="utf-8")
    new_hash = _source_hash(project, unit_source)
    ledger["current_source_hash"] = new_hash
    _atomic_write(_ledger_path(project, session_id), ledger)
    return schemas.ok("restore_best_candidate", restored_to=unit_source,
                      best_attempt=ledger.get("best_attempt"),
                      source_hash=new_hash)


# --- stop policy ------------------------------------------------------------


def evaluate_stop(ledger) -> dict:
    """Recommend whether to stop, per the documented policy. Advisory only."""
    n = ledger["attempt_count"]
    reasons = []
    outcome = None
    if ledger.get("outcome") == "MATCHING":
        return {"should_stop": True, "reasons": ["matched"], "outcome": "MATCHING"}
    if ledger["compile_failure_count"] >= schemas.STOP_COMPILE_FAILURES:
        reasons.append("3 consecutive compile failures")
        outcome = "BLOCKED_TOOLING"
    if ledger["non_improvement_count"] >= schemas.STOP_NON_IMPROVING:
        reasons.append("4 non-improving attempts")
        outcome = outcome or "NO_PROGRESS"
    last = ledger["attempts"][-1] if ledger["attempts"] else {}
    if last.get("oscillation"):
        reasons.append("diff signature oscillation")
        outcome = outcome or "NO_PROGRESS"
    if last.get("repeated_asm_hash"):
        reasons.append("candidate assembly hash repeated")
        outcome = outcome or "NO_PROGRESS"
    if n >= ledger["hard_budget"]:
        reasons.append(f"hard budget ({ledger['hard_budget']}) reached")
        outcome = outcome or "BUDGET_EXHAUSTED"
    elif n >= ledger["initial_budget"] and ledger["non_improvement_count"] > 0:
        reasons.append(f"initial budget ({ledger['initial_budget']}) reached "
                       f"without recent improvement")
        outcome = outcome or "BUDGET_EXHAUSTED"
    if ledger.get("profile_fingerprint") is None:
        pass
    return {"should_stop": bool(reasons), "reasons": reasons, "outcome": outcome,
            "escalation": _escalation(ledger)}


def _escalation(ledger) -> dict:
    """Capability-based (never model-named) escalation recommendation."""
    n = ledger["attempt_count"]
    cats = set(ledger.get("mismatch_categories", []))
    hard = cats & {"register_allocation", "instruction_scheduling",
                   "instruction_order", "delay_slot"}
    recommend = (n >= ledger["initial_budget"] and ledger["non_improvement_count"] < 4
                 and ledger["compile_failure_count"] < 3 and bool(hard))
    return {
        "recommended": recommend,
        "capability": "deep_reasoning" if hard else "standard",
        "reason": ("remaining mismatch appears allocation/scheduling sensitive"
                   if hard else "no escalation signal"),
        "remaining_attempt_budget": _remaining_budget(ledger),
    }


# --- cross-module queries ---------------------------------------------------


def exhausted_function_ids(project) -> set:
    out = set()
    d = project.sessions_dir
    if not d.is_dir():
        return out
    for path in d.glob("*.json"):
        try:
            data = json.loads(path.read_text())
        except (OSError, json.JSONDecodeError):
            continue
        if data.get("outcome") in _EXHAUSTED_OUTCOMES:
            out.add(data.get("target_function_id"))
    out.discard(None)
    return out


def summary_for_function(project, function_id):
    d = project.sessions_dir
    if not d.is_dir():
        return None
    latest = None
    for path in d.glob("*.json"):
        try:
            data = json.loads(path.read_text())
        except (OSError, json.JSONDecodeError):
            continue
        if data.get("target_function_id") != function_id:
            continue
        if latest is None or data.get("created", "") > latest.get("created", ""):
            latest = data
    return _summary(latest) if latest else None


def search_sessions(project, query, limit, hit_builder):
    hits, ql = [], query.lower()
    d = project.sessions_dir
    if not d.is_dir():
        return hits
    for path in sorted(d.glob("*.json")):
        try:
            data = json.loads(path.read_text())
        except (OSError, json.JSONDecodeError):
            continue
        blob = json.dumps({k: data.get(k) for k in
                           ("target_function_id", "target_name", "outcome",
                            "hypotheses", "mismatch_categories")})
        if ql in blob.lower():
            hits.append(hit_builder(
                path.relative_to(project.root).as_posix(),
                f"session {data.get('session_id')}", "session",
                f"{data.get('target_name')} outcome={data.get('outcome')} "
                f"attempts={data.get('attempt_count')}",
                data.get("target_function_id")))
            if len(hits) >= limit:
                break
    return hits


# --- helpers ----------------------------------------------------------------


def _now():
    return time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime())


def _empty_tokens():
    return {"input": 0, "output": 0, "total": 0, "reported_checkpoints": 0,
            "by_client": {}, "by_model": {}}


def _accumulate_tokens(ledger, input_tokens, output_tokens, client, model):
    """Fold optional client-reported token counts into the running totals."""
    if not input_tokens and not output_tokens:
        return
    tu = ledger.setdefault("token_usage", _empty_tokens())
    i, o = int(input_tokens or 0), int(output_tokens or 0)
    tu["input"] += i
    tu["output"] += o
    tu["total"] += i + o
    tu["reported_checkpoints"] += 1
    for bucket, key in (("by_client", client or "unknown"),
                        ("by_model", model or "unknown")):
        row = tu[bucket].setdefault(key, {"input": 0, "output": 0, "total": 0})
        row["input"] += i
        row["output"] += o
        row["total"] += i + o


def _remaining_budget(ledger):
    return max(0, ledger["hard_budget"] - ledger["attempt_count"])


def _counters(ledger):
    return {
        "attempt_count": ledger["attempt_count"],
        "best_attempt": ledger.get("best_attempt"),
        "non_improvement_count": ledger["non_improvement_count"],
        "compile_failure_count": ledger["compile_failure_count"],
        "remaining_budget": _remaining_budget(ledger),
        "initial_budget": ledger["initial_budget"],
        "hard_budget": ledger["hard_budget"],
    }


def _score_value(exact, fuzzy_score, matching_bytes):
    if exact:
        return float("inf")
    if fuzzy_score is not None:
        return float(fuzzy_score)
    if matching_bytes is not None:
        return float(matching_bytes)
    return 0.0


def _summary(ledger):
    if not ledger:
        return None
    return {
        "session_id": ledger.get("session_id"),
        "target_function_id": ledger.get("target_function_id"),
        "target_name": ledger.get("target_name"),
        "created": ledger.get("created"),
        "outcome": ledger.get("outcome"),
        "attempt_count": ledger.get("attempt_count", 0),
        "best_attempt": ledger.get("best_attempt"),
        "compiler_profile": ledger.get("compiler_profile"),
        "counters": _counters(ledger),
        "token_usage": ledger.get("token_usage", _empty_tokens()),
    }
