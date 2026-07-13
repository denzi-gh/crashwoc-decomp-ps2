"""Repository health diagnostics -- cheap, never builds.

``project_health`` answers "is this repo in a state where matching work can
proceed, and if not, what exact command fixes it?" without touching the
toolchain or the game bytes. It reports registry presence, generated-state
presence and apparent freshness, tool availability, the profiles fingerprint
(when derivable), status-manifest validation, and a dirty tracked-file summary,
and turns every gap into an explicit repair command.
"""
from __future__ import annotations

import subprocess

from . import _bridge, schemas
from .project import DecompProject, REQUIRED_REGISTRIES
from .registry import Registry


def _newest_mtime(paths):
    best = 0.0
    for p in paths:
        try:
            best = max(best, p.stat().st_mtime)
        except OSError:
            pass
    return best


def _profiles_fingerprint(project):
    if not _bridge.repo_matches(project):
        return None, "profiles fingerprint only derivable for the packaged repo root"
    try:
        cc = _bridge.import_tool("cc")
        return cc.profiles_fingerprint(project.version), None
    except Exception as exc:                                   # pragma: no cover
        return None, f"fingerprint unavailable: {exc}"


def _tool_availability(project):
    """Best-effort availability of the locked compiler / assembler / linker."""
    result = {"compiler": None, "assembler": None, "linker": None, "nm": None,
              "note": None}
    if not _bridge.repo_matches(project):
        result["note"] = "tool availability only probed for the packaged repo root"
        return result
    try:
        cc = _bridge.import_tool("cc")
        tc = _bridge.import_tool("declib.toolchain")
        result["compiler"] = bool(cc.compiler_available("default", project.version))
        result["assembler"] = tc.tool_path(tc.AS) is not None
        result["linker"] = tc.tool_path(tc.LD) is not None
        result["nm"] = tc.tool_path(tc.NM) is not None
    except Exception as exc:                                   # pragma: no cover
        result["note"] = f"tool probe failed: {exc}"
    return result


def _git_dirty_summary(project):
    try:
        out = subprocess.run(
            ["git", "status", "--porcelain", "--untracked-files=no"],
            cwd=str(project.root), capture_output=True, text=True, timeout=20)
    except (OSError, subprocess.SubprocessError):
        return {"available": False, "tracked_changed": 0, "files": []}
    if out.returncode != 0:
        return {"available": False, "tracked_changed": 0, "files": []}
    files = [line[3:] for line in out.stdout.splitlines() if line.strip()]
    return {"available": True, "tracked_changed": len(files), "files": files[:20]}


def project_health(project: DecompProject) -> dict:
    reg = Registry(project)
    compat = project.compatibility()
    version_dir = project.config_dir

    registries = {r: (version_dir / r).is_file() for r in REQUIRED_REGISTRIES}
    registries["sections.json"] = (version_dir / "sections.json").is_file()
    registries["data_map.toml"] = (version_dir / "data_map.toml").is_file()

    # ELF presence via the version registry (never read the bytes here).
    elf_present = False
    try:
        import json
        meta = json.loads((version_dir / "version.json").read_text())
        elf_rel = meta["files"]["elf"]["path"]
        elf_present = (project.root / elf_rel).is_file()
    except Exception:
        elf_rel = None

    asm_present = (project.root / "asm" / "text.s").is_file()
    ninja_present = (project.root / "build.ninja").is_file()
    objdiff_present = (project.root / "objdiff.json").is_file()

    # Freshness: a status manifest newer than the build graph means the graph
    # must be regenerated before its hybrid edges exist.
    manifests = sorted(project.status_dir.rglob("*.toml")) \
        if project.status_dir.is_dir() else []
    newest_manifest = _newest_mtime(manifests + [version_dir / "functions.toml"])
    graph_mtime = (project.root / "build.ninja").stat().st_mtime \
        if ninja_present else 0.0
    graph_stale = ninja_present and newest_manifest > graph_mtime

    fingerprint, fp_note = _profiles_fingerprint(project)
    tools = _tool_availability(project)

    # Status-manifest validation (pure Python, no assets).
    status_ok, status_problems = _status_validation(project)

    missing, warnings, repairs = [], [], []
    if not compat.compatible:
        missing += compat.reasons
    if not elf_present:
        missing.append("target ELF absent (orig/) -- byte gates & compile/diff unavailable")
        repairs.append("place your legally obtained retail ELF at orig/%s/" % project.version)
    if not asm_present:
        missing.append("generated disassembly absent (asm/text.s)")
        repairs.append("python configure.py")
    if not ninja_present:
        warnings.append("build.ninja absent")
        repairs.append("python tools/gen_ninja.py")
    if graph_stale:
        warnings.append("a status manifest is newer than build.ninja")
        repairs.append("python tools/gen_ninja.py")
    if not status_ok:
        warnings.append(f"{len(status_problems)} status-manifest problem(s)")
        repairs.append("python tools/status.py --check")
    if fp_note:
        warnings.append(fp_note)
    if tools.get("compiler") is False:
        warnings.append("locked compiler not installed")
        repairs.append("python tools/setup_toolchain.py --download")

    state_counts = _state_counts(reg)

    return schemas.ok(
        "project_health",
        repository={"root": str(project.root), "compatible": compat.compatible,
                    "version": project.version},
        registries=registries,
        target_elf_present=elf_present,
        disassembly_present=asm_present,
        build_ninja_present=ninja_present,
        objdiff_present=objdiff_present,
        generated_state_fresh=(not graph_stale) if ninja_present else None,
        tools=tools,
        profiles=reg.profiles,
        profiles_fingerprint=fingerprint,
        status_valid=status_ok,
        status_problem_count=len(status_problems),
        status_problems=status_problems[:10],
        state_counts=state_counts,
        dirty_tracked=_git_dirty_summary(project),
        missing_prerequisites=missing,
        warnings=warnings,
        repairs=sorted(set(repairs)),
    )


def _status_validation(project):
    """Run the repository's own status validator (pure Python)."""
    try:
        status = _bridge.import_tool("status")
        results, _counts = status.check_version(project.version, project.root)
        problems = [f"{label}: {probs[0]}" for label, probs in sorted(results.items())
                    if probs]
        return (not problems), problems
    except Exception as exc:
        return False, [f"status validation failed to run: {exc}"]


def _state_counts(reg: Registry):
    counts = {"asm": 0, "equivalent": 0, "matching": 0}
    for data in reg.manifests.values():
        for entry in data.get("function", []):
            st = entry.get("state", "asm")
            counts[st] = counts.get(st, 0) + 1
    return counts
