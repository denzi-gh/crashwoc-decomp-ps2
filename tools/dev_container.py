#!/usr/bin/env python3
"""Manage the long-lived dev container that runs the Linux-only toolchain.

The EE GCC and PS2 binutils only run on Linux, so every build command
executes inside the Containerfile image. Starting a fresh container per
command is slow; instead one detached container (`crashwoc-dev`) idles with
the repo bind-mounted at /work, and tools/dispatch.py `exec`s into it. That
is what makes single-object rebuilds from the host (ninja, objdiff) fast.

Commands:
    python tools/dev_container.py start     # create/start the container
    python tools/dev_container.py stop      # remove it
    python tools/dev_container.py status    # absent | stopped | running

`ensure()` is the library entry point dispatch.py uses: it starts the
container if needed and returns the engine name.

Engine: docker by default, podman if docker is absent; override with the
CRASHWOC_ENGINE environment variable. The image must exist first:

    docker build -f Containerfile -t crashwoc-decomp .

If the repository is moved, `stop` then `start` so the bind mount points at
the new location.
"""
import argparse
import hashlib
import os
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
# The name carries a fingerprint of THIS checkout's location: two checkouts
# on one machine (e.g. the dev tree and the CI runner's workspace) must
# never exec into each other's container, because the bind mount points at
# whichever repo created it first. Override with CRASHWOC_CONTAINER.
CONTAINER = os.environ.get("CRASHWOC_CONTAINER") or (
    "crashwoc-dev-"
    + hashlib.sha256(str(ROOT).casefold().encode()).hexdigest()[:8])
IMAGE = "crashwoc-decomp"


class ContainerError(Exception):
    pass


def engine():
    """The container engine binary name."""
    override = os.environ.get("CRASHWOC_ENGINE")
    if override:
        return override
    for name in ("docker", "podman"):
        if shutil.which(name):
            return name
    raise ContainerError("neither docker nor podman found on PATH "
                         "(set CRASHWOC_ENGINE to override)")


def _run(eng, *args, check=True):
    return subprocess.run([eng, *args], check=check,
                          capture_output=True, text=True)


def status(eng=None):
    """'absent' | 'stopped' | 'running'."""
    eng = eng or engine()
    probe = _run(eng, "container", "inspect", "--format",
                 "{{.State.Running}}", CONTAINER, check=False)
    if probe.returncode != 0:
        return "absent"
    return "running" if probe.stdout.strip() == "true" else "stopped"


def ensure(eng=None):
    """Start the dev container if it is not running; return the engine name."""
    eng = eng or engine()
    state = status(eng)
    if state == "running":
        return eng
    if state == "stopped":
        _run(eng, "start", CONTAINER)
        return eng
    result = _run(eng, "run", "-d", "--name", CONTAINER,
                  "-v", f"{ROOT}:/work", "-w", "/work",
                  IMAGE, "sleep", "infinity", check=False)
    if result.returncode != 0:
        raise ContainerError(
            f"could not start {CONTAINER}: {result.stderr.strip()}\n"
            f"Is the image built?  {eng} build -f Containerfile -t {IMAGE} .")
    return eng


def stop(eng=None):
    eng = eng or engine()
    _run(eng, "rm", "-f", CONTAINER, check=False)


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("action", choices=("start", "stop", "status"))
    args = parser.parse_args()

    try:
        if args.action == "start":
            eng = ensure()
            print(f"{CONTAINER} running ({eng}, {ROOT} -> /work)")
        elif args.action == "stop":
            stop()
            print(f"{CONTAINER} removed")
        else:
            print(status())
    except ContainerError as exc:
        print(f"dev_container: {exc}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
