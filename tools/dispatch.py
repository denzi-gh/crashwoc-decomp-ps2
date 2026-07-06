#!/usr/bin/env python3
"""Run a build command where the toolchain lives: in the dev container.

    python tools/dispatch.py ninja build/pal103/current/nucore/nulist.o
    python tools/dispatch.py ninja expected
    python tools/dispatch.py python tools/verify_hybrid.py

On a POSIX host (inside the container, or a Linux machine with the toolchain
installed) the command runs directly at the repo root. On Windows it is
forwarded into the long-lived `crashwoc-dev` container (started on demand --
tools/dev_container.py) via `docker exec`, with any host paths in the
arguments translated to their /work equivalents.

This is the command objdiff is configured with (PR 6): objdiff invokes
`python tools/dispatch.py ninja <object>` and ninja rebuilds exactly that
node, wherever objdiff itself is running.

Path translation is deliberately conservative:
  * an absolute path inside the repo  -> /work/<repo-relative, POSIX>
  * an absolute path outside the repo -> error (the container cannot see it)
  * a relative path with backslashes  -> forward slashes
  * everything else                   -> untouched

Exit status is the command's own.
"""
import os
import re
import subprocess
import sys
from pathlib import Path, PureWindowsPath

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
import dev_container

_WIN_ABS = re.compile(r"^[A-Za-z]:[\\/]")


class DispatchError(Exception):
    pass


def translate_arg(arg, root=ROOT):
    """One command argument, host form -> container (/work) form."""
    if _WIN_ABS.match(arg):
        # Case-insensitive prefix match: Windows paths compare caselessly and
        # the drive letter's case varies between producers.
        parts = PureWindowsPath(arg).parts
        root_parts = PureWindowsPath(str(root)).parts
        folded = [p.casefold() for p in parts]
        if folded[:len(root_parts)] != [p.casefold() for p in root_parts]:
            raise DispatchError(
                f"absolute path outside the repo cannot be dispatched: {arg}")
        rel = parts[len(root_parts):]
        return "/work/" + "/".join(rel) if rel else "/work"
    if "\\" in arg:
        return arg.replace("\\", "/")
    return arg


def main(argv):
    if not argv:
        print("usage: python tools/dispatch.py <command> [args...]",
              file=sys.stderr)
        return 2

    if os.name == "posix":
        return subprocess.run(argv, cwd=ROOT).returncode

    try:
        cmd = [translate_arg(a) for a in argv]
        eng = dev_container.ensure()
    except (DispatchError, dev_container.ContainerError) as exc:
        print(f"dispatch: {exc}", file=sys.stderr)
        return 1
    return subprocess.run([eng, "exec", "-w", "/work",
                           dev_container.CONTAINER, *cmd]).returncode


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
