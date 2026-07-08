"""Route toolchain-bound operations through the locked-toolchain environment.

The compiler, assembler and linker only run where they are installed -- inside
the Containerfile image (or a native Linux install with ``CRASHWOC_DIRECT=1``).
The pure-registry operations run anywhere. This module lets a toolchain-bound
domain operation (``compile_diff``, ``verify_candidate``, ``promote_matching``)
be called from a Windows host: when the toolchain is not local, it re-invokes
the domain CLI inside the container via ``tools/dispatch.py`` and returns the
same JSON the in-container run would have produced.

Inside the container the check short-circuits, so there is no recursion: the
op runs in-process against the real toolchain.
"""
from __future__ import annotations

import json
import os
import subprocess
import tempfile

from . import _bridge, schemas


def toolchain_local(project) -> bool:
    """True when the compiler/binutils can be executed in this process."""
    if not _bridge.repo_matches(project):
        return False
    if os.environ.get("CRASHWOC_DIRECT"):
        return True
    try:
        dispatch = _bridge.import_tool("dispatch")
        return bool(dispatch.in_container())
    except Exception:
        return False


def dispatch_cli(project, cli_args, *, operation="dispatch", timeout=1800) -> dict:
    """Run ``tools.decomp_agent.cli <cli_args> --json`` inside the container."""
    tools = _bridge.tools_dir()
    # Global flags MUST precede the subcommand for argparse.
    cmd = ["python", str(tools / "dispatch.py"), "python", "-m",
           "tools.decomp_agent.cli", "--json", "--repo", str(project.root),
           "--version", project.version, *cli_args]
    # Redirect the child's stdout/stderr to temp FILES rather than pipes, and
    # detach its stdin. On Windows a running asyncio ProactorEventLoop (the MCP
    # stdio server) keeps inheritable handles; a piped ``subprocess.run`` then
    # hangs forever in ``communicate()`` waiting for a pipe EOF that never comes
    # because the child inherits a duplicate of the pipe's write end. Waiting on
    # the process handle with file-backed output sidesteps that deadlock, and
    # DEVNULL stdin keeps the child from ever touching the parent's JSON-RPC
    # stream. Behaviour is unchanged for the terminal CLI path.
    try:
        with tempfile.TemporaryFile(mode="w+", encoding="utf-8",
                                    errors="replace") as out, \
                tempfile.TemporaryFile(mode="w+", encoding="utf-8",
                                       errors="replace") as err:
            proc = subprocess.run(cmd, cwd=str(project.root), stdin=subprocess.DEVNULL,
                                  stdout=out, stderr=err, timeout=timeout)
            out.seek(0)
            err.seek(0)
            stdout, stderr = out.read(), err.read()
    except (OSError, subprocess.SubprocessError) as exc:
        return schemas.err(operation, f"container dispatch failed: {exc}",
                           code="dispatch")
    payload = _last_json(stdout)
    if payload is None:
        tail = (stderr or stdout or "").strip()[-600:]
        return schemas.err(operation,
                           f"container run produced no JSON (exit {proc.returncode})",
                           code="dispatch", detail=tail)
    return payload


def _last_json(text: str):
    """Parse the last complete JSON object printed on stdout."""
    depth, start, best = 0, None, None
    for i, ch in enumerate(text):
        if ch == "{":
            if depth == 0:
                start = i
            depth += 1
        elif ch == "}":
            depth -= 1
            if depth == 0 and start is not None:
                chunk = text[start:i + 1]
                try:
                    best = json.loads(chunk)
                except json.JSONDecodeError:
                    pass
    return best
