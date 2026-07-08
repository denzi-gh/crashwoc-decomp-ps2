#!/usr/bin/env python3
"""Command-line front end for the deterministic domain layer.

Independent of MCP: everything the MCP server exposes is reachable here, so the
same operations can be scripted, tested, and (crucially) re-invoked inside the
container by ``tools/decomp_agent/runtime.py`` for toolchain-bound work.

    python -m tools.decomp_agent.cli health
    python -m tools.decomp_agent.cli candidates --limit 10 --max-size 128
    python -m tools.decomp_agent.cli context pal103:unit-0007:00105a60:NuListGetNext
    python -m tools.decomp_agent.cli compile-diff <id> --session <sid>
    python -m tools.decomp_agent.cli session start <id>
    python -m tools.decomp_agent.cli session resume <sid>
    python -m tools.decomp_agent.cli verify <id> --level function

Global flags: --repo, --version, --json.
"""
from __future__ import annotations

import argparse
import json
import sys

from .project import DecompProject, ProjectError
from . import (health as health_mod, candidates as cand_mod, context as ctx_mod,
               evidence as ev_mod, diff as diff_mod, sessions, blockers,
               verification, registry as reg_mod)


def _project(args):
    return DecompProject.resolve(args.repo, args.version)


def cmd_health(args):
    return health_mod.project_health(_project(args))


def cmd_candidates(args):
    return cand_mod.list_candidates(
        _project(args), scope=args.scope, profile=args.profile,
        category=args.category, max_size=args.max_size,
        require_c_unit=args.require_c_unit,
        prefer_leaf_functions=args.prefer_leaf, limit=args.limit)


def cmd_resolve(args):
    return reg_mod.Registry(_project(args)).resolve(args.target).as_dict()


def cmd_context(args):
    return ctx_mod.get_context(_project(args), args.target, detail=args.detail)


def cmd_disasm(args):
    return ctx_mod.get_disassembly(_project(args), args.target)


def cmd_evidence(args):
    return ev_mod.search_evidence(_project(args), args.query,
                                  kinds=args.kind or None, limit=args.limit)


def cmd_compile_diff(args):
    return diff_mod.compile_diff(_project(args), args.target,
                                 session_id=args.session, client=args.client)


def cmd_verify(args):
    return verification.verify_candidate(_project(args), args.target,
                                         level=args.level)


def cmd_promote(args):
    return verification.promote_matching(_project(args), args.target,
                                         precheck=not args.no_precheck)


def cmd_probe(args):
    return verification.compiler_probe(_project(args), profile=args.profile)


def cmd_blocker_record(args):
    return blockers.record_blocker(
        _project(args), args.target, kind=args.kind, confidence=args.confidence,
        evidence=args.evidence, session_id=args.session, client=args.client,
        recommended_human_action=args.human, recommended_tooling_action=args.tooling)


def cmd_blocker_list(args):
    return blockers.list_blockers(_project(args), target=args.target)


def cmd_session(args):
    proj = _project(args)
    if args.session_cmd == "start":
        return sessions.start_session(proj, args.target, client=args.client,
                                      model=args.model,
                                      reasoning_effort=args.reasoning)
    if args.session_cmd == "resume":
        return sessions.resume_session(proj, args.id, client=args.client)
    if args.session_cmd == "list":
        return sessions.list_sessions(proj)
    if args.session_cmd == "get":
        return sessions.get_session(proj, args.id)
    if args.session_cmd == "checkpoint":
        return sessions.checkpoint_session(
            proj, args.id, hypothesis=args.hypothesis, client=args.client,
            model=args.model, reasoning_effort=args.reasoning,
            source_hash=args.source_hash, candidate_asm_hash=args.asm_hash,
            diff_signature=args.diff_sig, fuzzy_score=args.fuzzy,
            matching_bytes=args.matching_bytes, compile_ok=_tri(args.compile_ok),
            exact=_tri(args.exact), input_tokens=args.input_tokens,
            output_tokens=args.output_tokens)
    if args.session_cmd == "end":
        return sessions.end_session(proj, args.id, outcome=args.outcome,
                                    client=args.client)
    if args.session_cmd == "restore":
        return sessions.restore_best_candidate(proj, args.id,
                                               force=args.force, client=args.client)
    if args.session_cmd == "recover-lock":
        return sessions.recover_lock(proj, args.id, client=args.client)
    raise SystemExit(f"unknown session subcommand: {args.session_cmd}")


def _tri(value):
    if value is None:
        return None
    if isinstance(value, bool):
        return value
    return str(value).strip().lower() in ("1", "true", "yes", "y", "on")


def build_parser():
    p = argparse.ArgumentParser(prog="decomp_agent", description=__doc__.splitlines()[0])
    p.add_argument("--repo", default=None, help="repository root (default: env/cwd)")
    p.add_argument("--version", default=None, help="registry version (default: pal103)")
    p.add_argument("--json", action="store_true", help="emit full JSON")
    sub = p.add_subparsers(dest="cmd", required=True)

    sub.add_parser("health").set_defaults(func=cmd_health)

    c = sub.add_parser("candidates")
    c.add_argument("--scope"); c.add_argument("--profile"); c.add_argument("--category")
    c.add_argument("--max-size", type=int); c.add_argument("--limit", type=int, default=20)
    c.add_argument("--require-c-unit", action="store_true")
    c.add_argument("--prefer-leaf", action="store_true")
    c.set_defaults(func=cmd_candidates)

    r = sub.add_parser("resolve"); r.add_argument("target"); r.set_defaults(func=cmd_resolve)

    ctx = sub.add_parser("context"); ctx.add_argument("target")
    ctx.add_argument("--detail", action="store_true"); ctx.set_defaults(func=cmd_context)

    da = sub.add_parser("disasm"); da.add_argument("target")
    da.set_defaults(func=cmd_disasm)

    ev = sub.add_parser("evidence"); ev.add_argument("query")
    ev.add_argument("--kind", action="append"); ev.add_argument("--limit", type=int, default=40)
    ev.set_defaults(func=cmd_evidence)

    cd = sub.add_parser("compile-diff"); cd.add_argument("target")
    cd.add_argument("--session"); cd.add_argument("--client")
    cd.set_defaults(func=cmd_compile_diff)

    v = sub.add_parser("verify"); v.add_argument("target")
    v.add_argument("--level", choices=("function", "unit", "image"), default="function")
    v.set_defaults(func=cmd_verify)

    pr = sub.add_parser("promote"); pr.add_argument("target")
    pr.add_argument("--no-precheck", action="store_true"); pr.set_defaults(func=cmd_promote)

    pb = sub.add_parser("probe"); pb.add_argument("--profile", default="default")
    pb.set_defaults(func=cmd_probe)

    br = sub.add_parser("blocker")
    brs = br.add_subparsers(dest="blocker_cmd", required=True)
    brr = brs.add_parser("record"); brr.add_argument("target")
    brr.add_argument("--kind", required=True); brr.add_argument("--confidence", default="medium")
    brr.add_argument("--evidence", action="append"); brr.add_argument("--session")
    brr.add_argument("--client"); brr.add_argument("--human"); brr.add_argument("--tooling")
    brr.set_defaults(func=cmd_blocker_record)
    brl = brs.add_parser("list"); brl.add_argument("target", nargs="?")
    brl.set_defaults(func=cmd_blocker_list)

    s = sub.add_parser("session")
    ss = s.add_subparsers(dest="session_cmd", required=True)
    st = ss.add_parser("start"); st.add_argument("target")
    for a in ("--client", "--model", "--reasoning"):
        st.add_argument(a)
    rs = ss.add_parser("resume"); rs.add_argument("id"); rs.add_argument("--client")
    ss.add_parser("list")
    g = ss.add_parser("get"); g.add_argument("id")
    cp = ss.add_parser("checkpoint"); cp.add_argument("id")
    cp.add_argument("--hypothesis"); cp.add_argument("--client"); cp.add_argument("--model")
    cp.add_argument("--reasoning"); cp.add_argument("--source-hash")
    cp.add_argument("--asm-hash"); cp.add_argument("--diff-sig")
    cp.add_argument("--fuzzy", type=float); cp.add_argument("--matching-bytes", type=int)
    cp.add_argument("--compile-ok"); cp.add_argument("--exact")
    cp.add_argument("--input-tokens", type=int); cp.add_argument("--output-tokens", type=int)
    en = ss.add_parser("end"); en.add_argument("id"); en.add_argument("--outcome")
    en.add_argument("--client")
    re_ = ss.add_parser("restore"); re_.add_argument("id")
    re_.add_argument("--force", action="store_true"); re_.add_argument("--client")
    rl = ss.add_parser("recover-lock"); rl.add_argument("id"); rl.add_argument("--client")
    s.set_defaults(func=cmd_session)

    return p


def _print(result, as_json):
    if as_json:
        print(json.dumps(result, indent=2, default=str))
        return
    # Compact human view: headline + a few salient keys.
    if isinstance(result, dict):
        op = result.get("operation", "")
        head = "OK" if result.get("ok", True) else "ERROR"
        print(f"[{head}] {op}")
        if not result.get("ok", True):
            print("  " + result.get("error", {}).get("message", ""))
        for k, v in result.items():
            if k in ("ok", "operation", "domain_api", "error"):
                continue
            s = json.dumps(v, default=str)
            print(f"  {k}: {s[:300]}{'…' if len(s) > 300 else ''}")
    else:
        print(json.dumps(result, indent=2, default=str))


def main(argv=None):
    args = build_parser().parse_args(argv)
    try:
        result = args.func(args)
    except ProjectError as exc:
        result = {"ok": False, "operation": args.cmd,
                  "error": {"code": "project", "message": str(exc)}}
    except Exception as exc:  # noqa: BLE001 -- CLI boundary: never leak a traceback
        result = {"ok": False, "operation": getattr(args, "cmd", "?"),
                  "error": {"code": "exception", "message": str(exc)}}
    _print(result, args.json)
    return 0 if (isinstance(result, dict) and result.get("ok", True)) else 1


if __name__ == "__main__":
    sys.exit(main())
