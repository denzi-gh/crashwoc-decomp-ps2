"""The rooted project object every domain operation flows through.

``DecompProject`` binds an explicit repository root and a registry version, and
provides *safe* path resolution: any path the domain layer touches is validated
to stay inside the repository root, so no operation -- and no MCP tool built on
one -- can be tricked into reading or writing outside the selected repo.

Root resolution order (used by the CLI and MCP entry points):

  1. an explicit ``--repo`` / ``root=`` argument
  2. the ``CRASHWOC_PROJECT_DIR`` environment variable
  3. the current working directory

A directory only validates as a project when the load-bearing files exist
(``tools/cc.py``, ``tools/promote.py``, the three registries, etc.), so an
unrelated directory is rejected up front instead of failing deep inside an
operation. The markers are all code/registry files -- the domain layer depends
on no documentation file.
"""
from __future__ import annotations

import os
import tomllib
from dataclasses import dataclass
from pathlib import Path

# Files that must all be present for a directory to be a compatible project.
# All markers are code files: the domain layer depends on no documentation file.
REQUIRED_MARKERS = (
    "tools/cc.py",
    "tools/dispatch.py",
    "tools/promote.py",
)
# Version-scoped registries required for the domain layer to function.
REQUIRED_REGISTRIES = (
    "profiles.toml",
    "units.toml",
    "functions.toml",
    "symbol_addrs.txt",
)

ENV_PROJECT_DIR = "CRASHWOC_PROJECT_DIR"
DEFAULT_VERSION = "pal103"
AGENT_CONFIG = "config/decomp_agent.toml"


class ProjectError(Exception):
    """The selected directory is not a usable CrashWOC decompilation project."""


@dataclass
class ProjectCompatibility:
    compatible: bool
    missing_markers: list
    missing_registries: list
    reasons: list


class DecompProject:
    """A repository-rooted handle for the deterministic domain layer."""

    def __init__(self, root, version: str = DEFAULT_VERSION, *, validate: bool = True):
        self.root = Path(root).resolve()
        self.version = version
        if validate:
            self.require_compatible()

    # -- construction helpers ------------------------------------------------

    @classmethod
    def resolve(cls, root=None, version=None, *, validate: bool = True) -> "DecompProject":
        """Resolve a project root from an explicit arg, the env var, or cwd."""
        chosen = root or os.environ.get(ENV_PROJECT_DIR) or os.getcwd()
        proj = cls(chosen, version=version or DEFAULT_VERSION, validate=False)
        # Honour a version pinned in config/decomp_agent.toml unless overridden.
        if version is None:
            cfg = proj.agent_config()
            if cfg.get("default_version"):
                proj.version = cfg["default_version"]
        if validate:
            proj.require_compatible()
        return proj

    # -- compatibility -------------------------------------------------------

    def compatibility(self) -> ProjectCompatibility:
        missing_markers = [m for m in REQUIRED_MARKERS if not (self.root / m).is_file()]
        version_dir = self.root / "config" / self.version
        missing_registries = [r for r in REQUIRED_REGISTRIES
                              if not (version_dir / r).is_file()]
        reasons = []
        if not self.root.is_dir():
            reasons.append(f"{self.root} is not a directory")
        for m in missing_markers:
            reasons.append(f"missing project marker: {m}")
        for r in missing_registries:
            reasons.append(f"missing registry: config/{self.version}/{r}")
        return ProjectCompatibility(
            compatible=not reasons,
            missing_markers=missing_markers,
            missing_registries=missing_registries,
            reasons=reasons,
        )

    def is_compatible(self) -> bool:
        return self.compatibility().compatible

    def require_compatible(self) -> None:
        compat = self.compatibility()
        if not compat.compatible:
            raise ProjectError(
                f"{self.root} is not a compatible crashwoc-decomp-ps2 project: "
                + "; ".join(compat.reasons))

    # -- safe path resolution ------------------------------------------------

    def path(self, *parts) -> Path:
        """Resolve ``parts`` under the repository root, rejecting escapes.

        Accepts repo-relative parts only. An absolute component, or any path
        that would resolve outside the root (``..`` traversal, symlink escape),
        raises :class:`ProjectError`. This is the single choke point every
        file access in the domain layer goes through.
        """
        candidate = self.root
        for part in parts:
            p = Path(part)
            if p.is_absolute() or (len(p.parts) and p.parts[0] in ("", os.sep)):
                raise ProjectError(f"absolute path component not allowed: {part!r}")
            candidate = candidate / p
        resolved = candidate.resolve()
        try:
            resolved.relative_to(self.root)
        except ValueError:
            raise ProjectError(
                f"path escapes repository root: {os.path.join(*map(str, parts))!r}")
        return resolved

    def contains(self, path) -> bool:
        try:
            Path(path).resolve().relative_to(self.root)
            return True
        except ValueError:
            return False

    # -- common locations ----------------------------------------------------

    @property
    def config_dir(self) -> Path:
        return self.root / "config" / self.version

    @property
    def status_dir(self) -> Path:
        return self.config_dir / "status"

    @property
    def src_dir(self) -> Path:
        return self.root / "src"

    @property
    def sessions_dir(self) -> Path:
        return self.root / "build" / self.version / "agent_sessions"

    def agent_config(self) -> dict:
        """Load ``config/decomp_agent.toml`` if present (best-effort)."""
        path = self.root / AGENT_CONFIG
        if not path.is_file():
            return {}
        try:
            return tomllib.loads(path.read_text())
        except (tomllib.TOMLDecodeError, OSError):
            return {}

    def as_dict(self) -> dict:
        compat = self.compatibility()
        return {
            "root": str(self.root),
            "version": self.version,
            "compatible": compat.compatible,
            "missing_markers": compat.missing_markers,
            "missing_registries": compat.missing_registries,
            "reasons": compat.reasons,
        }
