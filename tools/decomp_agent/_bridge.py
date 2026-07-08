"""Import bridge to the repository's existing ``tools/`` scripts.

The domain layer prefers *importing* proven repository functions
(``tools/cc.py``, ``tools/declib``, ``tools/verify_promoted.py`` ...) over
re-implementing or shelling out to them. Those modules live under
``<repo>/tools`` and resolve their own paths from ``__file__``; this helper puts
that directory on ``sys.path`` so they import cleanly.

Note: because those scripts derive their own ``ROOT`` from ``__file__``, the
toolchain-invoking operations (compile, verify, promote) act on the repository
that physically contains this package. The MCP server validates the requested
root, and in normal use the requested root *is* that repository, so the two
coincide. Pure-registry operations do not use this bridge -- they read strictly
through the rooted :class:`DecompProject`.
"""
from __future__ import annotations

import importlib
import sys
from pathlib import Path

_PACKAGE_REPO_ROOT = Path(__file__).resolve().parent.parent.parent


def tools_dir() -> Path:
    return _PACKAGE_REPO_ROOT / "tools"


def ensure_on_path() -> Path:
    d = tools_dir()
    s = str(d)
    if s not in sys.path:
        sys.path.insert(0, s)
    return d


def import_tool(name: str):
    """Import a module from ``tools/`` (e.g. ``cc``, ``declib.toolchain``)."""
    ensure_on_path()
    return importlib.import_module(name)


def repo_matches(project) -> bool:
    """True when the project root is the repository this package ships in."""
    try:
        return Path(project.root).resolve() == _PACKAGE_REPO_ROOT
    except Exception:
        return False
