#!/usr/bin/env python3
"""Model-independent MCP server for CrashWOC matching decompilation.

A thin, standards-compliant stdio MCP server that exposes the deterministic
domain layer (``tools.decomp_agent``) as typed tools and read-only resources.
It contains no provider- or model-specific logic: it validates inputs, calls the
domain layer, and serializes results. Model choice and reasoning effort belong
entirely to the client (Claude Code, Codex, ...).

Run:

    python -m tools.decomp_mcp.server --repo /path/to/crashwoc-decomp-ps2

Root resolution: ``--repo`` > ``CRASHWOC_PROJECT_DIR`` > cwd. The directory must
validate as a compatible project or the server refuses to start.

Protocol hygiene: stdout carries only MCP messages; all logging goes to stderr.
Nothing here ever ``print()``s to stdout.
"""
from __future__ import annotations

import argparse
import logging
import os
import sys
from pathlib import Path

# Make the sibling ``tools`` importable whether launched as a module or a script.
_TOOLS = Path(__file__).resolve().parent.parent
if str(_TOOLS) not in sys.path:
    sys.path.insert(0, str(_TOOLS))

from decomp_agent.project import DecompProject, ProjectError, ENV_PROJECT_DIR  # noqa: E402
from decomp_agent import (health, candidates, context as ctx, evidence, diff,   # noqa: E402
                          sessions, blockers, verification, registry as regmod)

log = logging.getLogger("decomp_mcp")


def resolve_project(repo=None, version=None) -> DecompProject:
    """Resolve and validate the repository root (raises ProjectError)."""
    return DecompProject.resolve(repo, version, validate=True)


def build_server(project: DecompProject):
    """Construct the FastMCP server (imports the SDK lazily)."""
    try:
        from mcp.server.fastmcp import FastMCP
    except Exception as exc:  # pragma: no cover - depends on optional dep
        raise SystemExit(
            "the MCP SDK is not installed. Install it with:\n"
            "    python -m pip install -r requirements-mcp.txt\n"
            f"(import error: {exc})")

    mcp = FastMCP("crashwoc-decomp",
                  instructions="Deterministic matching-decompilation tools for "
                               "Crash Bandicoot: The Wrath of Cortex (PS2). "
                               "Prefer canonical function ids "
                               "(version:unit-NNNN:vram:name). Model and "
                               "reasoning-effort choices are the client's.")

    P = lambda: project  # noqa: E731 -- single validated project handle

    # -- tools ---------------------------------------------------------------

    @mcp.tool()
    def project_health() -> dict:
        """Cheap repo diagnostics: registries, tools, freshness, repair commands."""
        return health.project_health(P())

    @mcp.tool()
    def list_candidates(scope: str = None, profile: str = None,
                        category: str = None, max_size: int = None,
                        require_c_unit: bool = False,
                        prefer_leaf_functions: bool = False,
                        limit: int = 20) -> dict:
        """Deterministically ranked functions worth attempting, with reasons."""
        return candidates.list_candidates(
            P(), scope=scope, profile=profile, category=category,
            max_size=max_size, require_c_unit=require_c_unit,
            prefer_leaf_functions=prefer_leaf_functions, limit=limit)

    @mcp.tool()
    def resolve_target(target: str) -> dict:
        """Resolve a canonical id / unit id / name / address; returns ambiguity."""
        return regmod.Registry(P()).resolve(target).as_dict()

    @mcp.tool()
    def get_context(target: str, detail: bool = False) -> dict:
        """Compact dossier: facts, ground_truth, classifications, hypotheses, unknowns.

        ``ground_truth`` points at (and, for matchable-size functions, inlines)
        the retail target disassembly -- the authoritative source for these
        bytes. Any cross-platform reference decompilation is a hint only.
        """
        return ctx.get_context(P(), target, detail=detail)

    @mcp.tool()
    def get_disassembly(target: str) -> dict:
        """Return one function's retail target disassembly -- the ground truth.

        Reconstruct C from this authoritative listing and re-diff. A reference
        decompilation of another platform is a hint to verify against it, never
        something to transcribe. Read this before recording a blocker or ending
        a session as blocked.
        """
        return ctx.get_disassembly(P(), target)

    @mcp.tool()
    def search_evidence(query: str, kinds: list = None, limit: int = 40) -> dict:
        """Deterministic search across registries, source, headers, history."""
        return evidence.search_evidence(P(), query, kinds=kinds, limit=limit)

    @mcp.tool()
    def compile_diff(target: str, session_id: str = None,
                     client: str = None) -> dict:
        """Compile the unit and byte-compare over the full extent (locked path)."""
        return diff.compile_diff(P(), target, session_id=session_id, client=client)

    @mcp.tool()
    def verify_candidate(target: str, level: str = "function") -> dict:
        """Byte verification: function (default) / unit / image (explicit)."""
        return verification.verify_candidate(P(), target, level=level)

    @mcp.tool()
    def promote_matching(target: str) -> dict:
        """Promote via tools/promote.py (the only writer of state=matching)."""
        return verification.promote_matching(P(), target)

    @mcp.tool()
    def start_session(target: str, client: str = None, model: str = None,
                      reasoning_effort: str = None) -> dict:
        """Create a shared, resumable experiment session for a target."""
        return sessions.start_session(P(), target, client=client, model=model,
                                      reasoning_effort=reasoning_effort)

    @mcp.tool()
    def resume_session(session_id: str, client: str = None) -> dict:
        """Return everything needed to continue a session; refuses unsafe resume."""
        return sessions.resume_session(P(), session_id, client=client)

    @mcp.tool()
    def checkpoint_session(session_id: str, hypothesis: str = None,
                           expected_effect: str = None, source_hash: str = None,
                           source_snapshot: str = None,
                           candidate_asm_hash: str = None, target_hash: str = None,
                           diff_signature: str = None, fuzzy_score: float = None,
                           matching_bytes: int = None, differing_bytes: int = None,
                           classifications: list = None, compile_ok: bool = None,
                           exact: bool = None, client: str = None,
                           model: str = None, reasoning_effort: str = None,
                           input_tokens: int = None,
                           output_tokens: int = None) -> dict:
        """Record one experiment; updates counters and repeat/oscillation flags.

        Optional input_tokens/output_tokens are opaque diagnostics accumulated
        per client/model; they never affect verification, ranking or promotion.
        """
        return sessions.checkpoint_session(
            P(), session_id, hypothesis=hypothesis, expected_effect=expected_effect,
            source_hash=source_hash, source_snapshot=source_snapshot,
            candidate_asm_hash=candidate_asm_hash, target_hash=target_hash,
            diff_signature=diff_signature, fuzzy_score=fuzzy_score,
            matching_bytes=matching_bytes, differing_bytes=differing_bytes,
            classifications=classifications, compile_ok=compile_ok, exact=exact,
            client=client, model=model, reasoning_effort=reasoning_effort,
            input_tokens=input_tokens, output_tokens=output_tokens)

    @mcp.tool()
    def end_session(session_id: str, outcome: str = None,
                    client: str = None) -> dict:
        """Finalize a session with an outcome and release its lock."""
        return sessions.end_session(P(), session_id, outcome=outcome, client=client)

    @mcp.tool()
    def restore_best_candidate(session_id: str, force: bool = False,
                               client: str = None) -> dict:
        """Restore the best attempt's source (guarded; returns conflict, never clobbers)."""
        return sessions.restore_best_candidate(P(), session_id, force=force,
                                               client=client)

    @mcp.tool()
    def recover_session_lock(session_id: str, client: str = None) -> dict:
        """Break a *stale* session lock and take ownership (refuses a fresh lock)."""
        return sessions.recover_lock(P(), session_id, client=client)

    @mcp.tool()
    def record_blocker(target: str, kind: str, confidence: str = "medium",
                       evidence: list = None, session_id: str = None,
                       recommended_human_action: str = None,
                       recommended_tooling_action: str = None,
                       client: str = None) -> dict:
        """Record a structured blocker (why this function cannot match yet)."""
        return blockers.record_blocker(
            P(), target, kind=kind, confidence=confidence, evidence=evidence,
            session_id=session_id, recommended_human_action=recommended_human_action,
            recommended_tooling_action=recommended_tooling_action, client=client)

    @mcp.tool()
    def list_blockers(target: str = None) -> dict:
        """List recorded blockers (optionally for one target)."""
        return blockers.list_blockers(P(), target=target)

    @mcp.tool()
    def compiler_probe(profile: str = "default") -> dict:
        """Compile a fixed tiny snippet through the locked profile (no caller flags)."""
        return verification.compiler_probe(P(), profile=profile)

    # -- resources -----------------------------------------------------------

    @mcp.resource("decomp://project/summary")
    def project_summary() -> dict:
        return P().as_dict()

    @mcp.resource("decomp://project/health")
    def project_health_res() -> dict:
        return health.project_health(P())

    @mcp.resource("decomp://function/{function_id}/context")
    def function_context(function_id: str) -> dict:
        return ctx.get_context(P(), function_id, detail=True)

    @mcp.resource("decomp://function/{function_id}/target-asm")
    def function_target_asm(function_id: str) -> dict:
        reg = regmod.Registry(P())
        r = reg.resolve(function_id)
        if r.record is None:
            return {"available": False, "reason": r.detail}
        return evidence.target_asm_for(P(), r.record)

    @mcp.resource("decomp://function/{function_id}/candidate-asm")
    def function_candidate_asm(function_id: str) -> dict:
        reg = regmod.Registry(P())
        r = reg.resolve(function_id)
        if r.record is None:
            return {"available": False, "reason": r.detail}
        return evidence.c_definition_for(P(), r.record)

    @mcp.resource("decomp://function/{function_id}/history")
    def function_history(function_id: str) -> dict:
        return {"session": sessions.summary_for_function(P(), function_id),
                "blockers": blockers.blockers_by_function(P(), False).get(function_id, [])}

    @mcp.resource("decomp://unit/{unit_id}/summary")
    def unit_summary(unit_id: str) -> dict:
        reg = regmod.Registry(P())
        r = reg.resolve(unit_id)
        if r.unit is None:
            return {"error": r.detail}
        fns = [f.as_dict() for f in reg.by_unit.get(r.unit.index, [])]
        return {"unit": r.unit.as_dict(), "functions": fns}

    @mcp.resource("decomp://session/{session_id}/ledger")
    def session_ledger(session_id: str) -> dict:
        return sessions.get_session(P(), session_id)

    @mcp.resource("decomp://compiler/{profile}/knowledge")
    def compiler_knowledge_res(profile: str) -> dict:
        from decomp_agent.compiler_knowledge import knowledge_for_profile
        return {"profile": profile, "quirks": knowledge_for_profile(P(), profile)}

    return mcp


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--repo", default=None,
                        help=f"repository root (default: ${ENV_PROJECT_DIR} or cwd)")
    parser.add_argument("--version", default=None, help="registry version")
    parser.add_argument("--log-level", default=os.environ.get("DECOMP_MCP_LOG", "INFO"))
    args = parser.parse_args(argv)

    logging.basicConfig(stream=sys.stderr, level=args.log_level,
                        format="%(asctime)s %(name)s %(levelname)s %(message)s")
    try:
        project = resolve_project(args.repo, args.version)
    except ProjectError as exc:
        print(f"decomp_mcp: {exc}", file=sys.stderr)
        return 2

    log.info("serving crashwoc-decomp MCP for %s (version %s)",
             project.root, project.version)
    server = build_server(project)
    server.run(transport="stdio")
    return 0


if __name__ == "__main__":
    sys.exit(main())
