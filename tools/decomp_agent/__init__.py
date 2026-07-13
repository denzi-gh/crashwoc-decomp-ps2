"""Deterministic, model-independent domain layer for matching decompilation.

This package is the CrashWOC-specific "brain" an AI coding agent talks to
through the thin MCP server in ``tools/decomp_mcp``. Every operation:

  * works without MCP and without any LLM,
  * is rooted through an explicit :class:`~tools.decomp_agent.project.DecompProject`
    (no reliance on module-global ``__file__`` paths for repository data),
  * returns stable JSON-compatible results (plain dicts / lists / scalars),
  * wraps the repository's existing locked tooling instead of re-implementing
    build, diff, verification or promotion logic.

The authority order every module honours (highest first):

  1. committed machine-readable registries + status manifests
  2. the retail target disassembly (ground truth for a function's own bytes)
  3. fresh compiler / assembler / linker / byte-verification output
  4. current hand-written C and committed headers
  5. comments and unverified hypotheses

The full cross-client workflow is embedded in the MCP server's ``instructions``
(``tools/decomp_mcp/server.py``); the domain layer needs no documentation file.
"""

DOMAIN_API = 1

from .project import DecompProject, ProjectError  # noqa: E402,F401
from . import identifiers  # noqa: E402,F401

__all__ = ["DecompProject", "ProjectError", "identifiers", "DOMAIN_API"]
