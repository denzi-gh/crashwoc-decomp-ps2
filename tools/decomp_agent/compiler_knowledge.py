"""Loader for the committed compiler-knowledge registry.

Reads ``config/<version>/compiler_knowledge.toml`` and exposes it filtered by
profile. Purely declarative -- the file is the source of truth, this module only
selects and shapes it into JSON-compatible dicts. Absence yields an empty list
rather than an error, so the domain layer works on a repo that has not added the
registry yet.
"""
from __future__ import annotations

import tomllib

_CONFIDENCE = ("proven", "likely", "hypothesis")


def load_knowledge(project) -> list:
    path = project.config_dir / "compiler_knowledge.toml"
    if not path.is_file():
        return []
    try:
        data = tomllib.loads(path.read_text())
    except tomllib.TOMLDecodeError:
        return []
    out = []
    for q in data.get("quirk", []):
        conf = q.get("confidence", "hypothesis")
        out.append({
            "id": q.get("id"),
            "profiles": list(q.get("profiles", [])),
            "category": q.get("category"),
            "signature": q.get("signature"),
            "effect": q.get("effect"),
            "handling": q.get("handling"),
            "confidence": conf if conf in _CONFIDENCE else "hypothesis",
            "evidence": list(q.get("evidence", [])),
        })
    return out


def knowledge_for_profile(project, profile_name: str) -> list:
    return [q for q in load_knowledge(project)
            if not q["profiles"] or profile_name in q["profiles"]]
