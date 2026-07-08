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
    """Run ``tools.decomp_agent.cli <cli_args> --json`` inside the dev container.

    This is invoked from a Windows host (the toolchain is Linux-only), from the
    MCP server's worker thread. It runs the domain CLI inside the long-lived dev
    container with a *single* hardened ``docker exec`` -- there is no nested
    ``python tools/dispatch.py`` process, and no subprocess on this path ever
    reads a pipe via ``communicate()``.

    Why that matters: the MCP stdio server runs on a Windows asyncio
    ProactorEventLoop, which keeps inheritable pipe handles alive. A child that
    inherits a duplicate of such a pipe's write end never lets the parent see
    EOF, so a piped ``subprocess.run`` (or ``capture_output=True``) can hang
    forever in ``communicate()``. Every command here instead writes to temp
    FILES and detaches stdin (``DEVNULL``), so we only ever wait on the process
    handle. The terminal CLI / ninja / objdiff path still uses
    ``tools/dispatch.py`` and is unaffected.
    """
    try:
        dispatch = _bridge.import_tool("dispatch")
        devc = _bridge.import_tool("dev_container")
    except Exception as exc:                                    # pragma: no cover
        return schemas.err(operation, f"cannot import container helpers: {exc}",
                           code="dispatch")

    try:
        eng = devc.engine()
    except devc.ContainerError as exc:
        return schemas.err(operation, str(exc), code="dispatch")

    ensure_err = _ensure_container(eng, devc)
    if ensure_err is not None:
        return schemas.err(operation, ensure_err, code="dispatch")

    # Build the in-container command. Global flags MUST precede the subcommand
    # for argparse; ``translate_arg`` maps the host repo root to ``/work``.
    inner = ["python", "-m", "tools.decomp_agent.cli", "--json",
             "--repo", str(project.root), "--version", project.version, *cli_args]
    try:
        translated = [dispatch.translate_arg(a) for a in inner]
    except dispatch.DispatchError as exc:
        return schemas.err(operation, str(exc), code="dispatch")
    cmd = [eng, "exec", "-w", "/work", devc.CONTAINER, *translated]

    try:
        rc, stdout, stderr = _hardened_run(cmd, cwd=str(project.root),
                                           timeout=timeout)
    except (OSError, subprocess.SubprocessError) as exc:
        return schemas.err(operation, f"container dispatch failed: {exc}",
                           code="dispatch")
    if rc == _TIMEOUT_RC:
        return schemas.err(operation,
                           f"container run timed out after {timeout}s",
                           code="dispatch", detail=(stderr or stdout)[-600:])
    payload = _last_json(stdout)
    if payload is None:
        tail = (stderr or stdout or "").strip()[-600:]
        return schemas.err(operation,
                           f"container run produced no JSON (exit {rc})",
                           code="dispatch", detail=tail)
    return payload


_TIMEOUT_RC = 124


def _hardened_run(cmd, *, cwd=None, timeout=1800):
    """Blocking subprocess that never reads a pipe: stdin ``DEVNULL``, stdout and
    stderr to temp FILES. Returns ``(returncode, stdout_text, stderr_text)`` with
    ``returncode == _TIMEOUT_RC`` on timeout. Deadlock-safe under the Windows
    Proactor loop (see :func:`dispatch_cli`)."""
    with tempfile.TemporaryFile(mode="w+", encoding="utf-8",
                                errors="replace") as out, \
            tempfile.TemporaryFile(mode="w+", encoding="utf-8",
                                   errors="replace") as err:
        try:
            proc = subprocess.run(cmd, cwd=cwd, stdin=subprocess.DEVNULL,
                                  stdout=out, stderr=err, timeout=timeout,
                                  close_fds=True)
            rc = proc.returncode
        except subprocess.TimeoutExpired:
            rc = _TIMEOUT_RC
        out.seek(0)
        err.seek(0)
        return rc, out.read(), err.read()


def _ensure_container(eng, devc):
    """Start the dev container if needed, using the deadlock-safe runner rather
    than ``dev_container.ensure()`` (which reads pipes via ``communicate()``).

    Returns ``None`` on success, or an error string. Container name, image and
    bind-mount root are taken from :mod:`dev_container` so this shares the exact
    same container the CLI / ninja path uses (it stays warm across both)."""
    try:
        rc, out, _ = _hardened_run(
            [eng, "container", "inspect", "--format", "{{.State.Running}}",
             devc.CONTAINER], timeout=120)
        if rc == 0:
            if out.strip() == "true":
                return None
            rc2, _, err2 = _hardened_run([eng, "start", devc.CONTAINER],
                                         timeout=120)
            return None if rc2 == 0 else \
                f"could not start container {devc.CONTAINER}: {err2.strip()}"
        rc3, _, err3 = _hardened_run(
            [eng, "run", "-d", "--name", devc.CONTAINER,
             "-v", f"{devc.ROOT}:/work", "-w", "/work",
             devc.IMAGE, "sleep", "infinity"], timeout=300)
        if rc3 != 0:
            return (f"could not start {devc.CONTAINER}: {err3.strip()}\n"
                    f"Is the image built?  "
                    f"{eng} build -f Containerfile -t {devc.IMAGE} .")
        return None
    except (OSError, subprocess.SubprocessError) as exc:
        return f"container ensure failed: {exc}"


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
