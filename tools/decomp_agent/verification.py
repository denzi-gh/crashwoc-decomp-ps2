"""Verification and promotion wrappers -- delegate to the repository's gates.

These operations never define their own notion of "matching"; they re-use the
committed byte gates:

  * ``verify_candidate(level="function")`` -- rebuilds the target as a matching
    hybrid and proves byte equality over the *full canonical extent* (the same
    check ``tools/verify_promoted.py`` applies per function).
  * ``verify_candidate(level="unit")``    -- runs ``tools/verify_promoted.py``
    and reports the target unit's committed ``matching`` claims.
  * ``verify_candidate(level="image")``   -- runs the full image gate.
  * ``promote_matching``                  -- delegates to ``tools/promote.py``,
    the sole writer of ``state = "matching"`` (it verifies and rolls back on
    failure); this wrapper never edits a manifest.

Toolchain-bound: routed into the container by ``runtime`` when not local.
"""
from __future__ import annotations

import json
import subprocess
import sys

from . import schemas, runtime, _bridge
from .project import DecompProject
from .registry import Registry


def verify_candidate(project: DecompProject, target, *, level="function") -> dict:
    if level not in ("function", "unit", "image"):
        return schemas.err("verify_candidate", f"unknown level {level!r}",
                           code="bad_level")
    reg = Registry(project)
    res = reg.resolve(target)
    if level != "image":
        if res.status == "ambiguous":
            return schemas.err("verify_candidate", res.detail, code="ambiguous",
                               candidates=res.candidates)
        if res.status != "resolved":
            return schemas.err("verify_candidate", res.detail or "unresolved",
                               code="not_found")

    if not runtime.toolchain_local(project):
        args = ["verify", str(target), "--level", level]
        return runtime.dispatch_cli(project, args, operation="verify_candidate")

    if level == "function":
        return _verify_function(project, reg, res.record)
    return _verify_via_promoted(project, reg, res, level)


def _verify_function(project, reg, fn):
    from . import diff
    try:
        built = diff._build_and_compare(project, reg, fn,
                                        reg.units.get(fn.unit_index),
                                        reg.effective_profile(fn.unit_index)[0])
    except diff._DiffToolingError as exc:
        return schemas.err("verify_candidate", str(exc), code="tooling",
                           outcome_hint="BLOCKED_TOOLING")
    if built.get("hybrid_error"):
        return schemas.ok("verify_candidate", level="function",
                          function_id=fn.id_str(), exact=False,
                          reason=built["hybrid_error"])
    return schemas.ok("verify_candidate", level="function",
                      function_id=fn.id_str(), exact=bool(built.get("exact")),
                      full_extent=[fn.address, fn.end], size=fn.size,
                      matching_bytes=built.get("matching_bytes"),
                      unresolved_symbols=built.get("unresolved") or [])


def _verify_via_promoted(project, reg, res, level):
    """Run tools/verify_promoted.py and shape the result for unit/image."""
    args = [sys.executable, "tools/verify_promoted.py", "--version", project.version]
    if level == "unit":
        args.append("--skip-image")
    proc = subprocess.run(args, cwd=str(project.root), capture_output=True, text=True)
    results_path = project.root / "build" / project.version / "verify_results.json"
    data = {}
    if results_path.is_file():
        try:
            data = json.loads(results_path.read_text())
        except (OSError, json.JSONDecodeError):
            data = {}
    if level == "image":
        return schemas.ok("verify_candidate", level="image",
                          exact=bool(data.get("image", {}).get("exact")),
                          image=data.get("image"),
                          all_green=data.get("all_green"),
                          returncode=proc.returncode)
    # unit
    unit_index = res.unit.index if res.unit else (res.record.unit_index if res.record else None)
    fids = {f.id_str() for f in reg.by_unit.get(unit_index, [])}
    fns = [r for r in data.get("functions", []) if r.get("id") in fids]
    return schemas.ok("verify_candidate", level="unit", unit_index=unit_index,
                      returncode=proc.returncode,
                      functions=fns,
                      all_verified=all(r.get("verified") for r in fns) if fns else None)


def promote_matching(project: DecompProject, target, *, precheck=True) -> dict:
    reg = Registry(project)
    res = reg.resolve(target)
    if res.status == "ambiguous":
        return schemas.err("promote_matching", res.detail, code="ambiguous",
                           candidates=res.candidates)
    if res.status != "resolved" or res.record is None:
        return schemas.err("promote_matching", res.detail or "unresolved",
                           code="not_found")
    fn = res.record

    if not runtime.toolchain_local(project):
        args = ["promote", fn.id_str()]
        if not precheck:
            args.append("--no-precheck")
        return runtime.dispatch_cli(project, args, operation="promote_matching",
                                    timeout=3600)

    # Confirm an exact function-level result exists before delegating.
    if precheck:
        pre = _verify_function(project, reg, fn)
        if not pre.get("exact"):
            return schemas.err(
                "promote_matching",
                "function does not currently match byte-for-byte over its full "
                "extent; refusing to promote",
                code="not_exact", verification=pre)

    proc = subprocess.run(
        [sys.executable, "tools/promote.py", fn.id_str(),
         "--version", project.version],
        cwd=str(project.root), capture_output=True, text=True)
    ok = proc.returncode == 0
    rolled_back = "rolled back" in (proc.stdout + proc.stderr).lower()
    return schemas.ok("promote_matching", function_id=fn.id_str(),
                      promoted=ok, rolled_back=rolled_back,
                      returncode=proc.returncode,
                      output=(proc.stdout or proc.stderr).strip()[-2000:])


def compiler_probe(project: DecompProject, profile="default", snippet=None) -> dict:
    """Optional safe probe: compile a fixed tiny snippet through tools/cc.py.

    No caller flags, no arbitrary command; artifacts land in a gitignored
    scratch dir. Returns the generated assembly and the profile fingerprint.
    """
    if not runtime.toolchain_local(project):
        args = ["probe", "--profile", profile]
        return runtime.dispatch_cli(project, args, operation="compiler_probe")
    try:
        cc = _bridge.import_tool("cc")
    except Exception as exc:                                   # pragma: no cover
        return schemas.err("compiler_probe", str(exc), code="tooling")
    if profile not in reg_profiles(project):
        return schemas.err("compiler_probe", f"unknown profile {profile!r}",
                           code="bad_profile")
    code = snippet if _safe_snippet(snippet) else "int decomp_probe(int a){return a+1;}\n"
    scratch = project.root / "build" / project.version / "agent_probe"
    scratch.mkdir(parents=True, exist_ok=True)
    src = scratch / "probe.c"
    src.write_text(code)
    out_s = scratch / "probe.s"
    try:
        cc.compile_s(src, out_s, profile=profile, version=project.version)
    except subprocess.CalledProcessError as exc:
        return schemas.ok("compiler_probe", profile=profile, compiled=False,
                          diagnostics=(exc.stderr or "")[-2000:])
    return schemas.ok("compiler_probe", profile=profile, compiled=True,
                      assembly=out_s.read_text()[:8000],
                      profile_fingerprint=cc.profiles_fingerprint(project.version))


def reg_profiles(project):
    return Registry(project).profiles


def _safe_snippet(text):
    """A conservative allowlist: only a small, self-contained C snippet."""
    if not isinstance(text, str) or len(text) > 4000:
        return False
    banned = ("#include", "#import", "asm", "__asm", "system(", "popen")
    low = text.lower()
    return not any(b in low for b in banned)
