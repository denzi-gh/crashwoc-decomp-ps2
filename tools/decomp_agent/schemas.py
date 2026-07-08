"""Shared vocabularies and JSON envelope helpers.

Domain results are plain JSON-compatible dicts. This module fixes the closed
vocabularies (mismatch categories, confidence levels, session outcomes, blocker
kinds) so the domain layer, the CLI and the MCP server all speak the same
words, and provides :func:`ok` / :func:`err` envelope builders every operation
returns.
"""
from __future__ import annotations

DOMAIN_API = 1

# Confidence for any heuristic classification. A heuristic must never claim
# "proven"; "proven" is reserved for byte-verified or mechanically-derived facts.
CONFIDENCE = ("proven", "high", "medium", "low")

# compile_diff mismatch categories (superset of the spec's minimum list).
MISMATCH_CATEGORIES = (
    "compile_error",
    "unresolved_symbol",
    "size_difference",
    "stack_frame",
    "saved_registers",
    "register_allocation",
    "instruction_order",
    "control_flow",
    "branch_direction",
    "delay_slot",
    "instruction_scheduling",
    "immediate_materialization",
    "signedness",
    "load_store_width",
    "likely_type_or_offset",
    "inlining",
    "loop_shape",
    "literal_pool",
    "jump_table_rodata",
    "initialized_local_sdata",
    "unsupported_lit8",
    "assembler_normalization",
    "likely_compiler_drift",
    "unknown",
)

# Session final outcomes.
OUTCOMES = (
    "MATCHING",
    "BLOCKED_TOOLING",
    "BLOCKED_MISSING_CONTEXT",
    "LIKELY_EQUIVALENT_REVIEW_REQUIRED",
    "NO_PROGRESS",
    "BUDGET_EXHAUSTED",
    "STALE_SESSION",
    "SOURCE_CONFLICT",
)

# Structured blocker kinds.
BLOCKER_KINDS = (
    "compiler_owned_rodata",
    "compiler_owned_sdata",
    "unsupported_lit8",
    "compiler_patchlevel_mismatch",
    "missing_type_layout",
    "unknown_prototype",
    "assembly_origin",
    "unresolved_symbol",
    "unit_level_coupling",
    "toolchain_failure",
    "no_progress",
    "budget_exhausted",
    "stale_session",
    "source_conflict",
    "unknown",
)

# Recommended attempt budgets (documented stop policy; not enforced by MCP).
DEFAULT_INITIAL_BUDGET = 8
DEFAULT_HARD_BUDGET = 20
STOP_NON_IMPROVING = 4
STOP_COMPILE_FAILURES = 3

SESSION_SCHEMA = 1


def ok(operation: str, **payload) -> dict:
    """Standard success envelope."""
    out = {"ok": True, "operation": operation, "domain_api": DOMAIN_API}
    out.update(payload)
    return out


def err(operation: str, message: str, *, code: str = "error", **payload) -> dict:
    """Standard failure envelope. ``message`` is safe, non-secret text."""
    out = {"ok": False, "operation": operation, "domain_api": DOMAIN_API,
           "error": {"code": code, "message": message}}
    out.update(payload)
    return out


def classification(category: str, confidence: str, evidence, *, window=None) -> dict:
    """One mismatch classification record."""
    if category not in MISMATCH_CATEGORIES:
        category = "unknown"
    if confidence not in CONFIDENCE:
        confidence = "low"
    rec = {"category": category, "confidence": confidence,
           "evidence": list(evidence) if not isinstance(evidence, str) else [evidence]}
    if window is not None:
        rec["window"] = window
    return rec
