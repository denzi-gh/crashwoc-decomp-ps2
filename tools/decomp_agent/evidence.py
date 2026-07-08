"""Deterministic evidence search across the committed sources of truth.

``search_evidence`` is a plain, reproducible text/registry search -- no
embeddings, no ranking model. It walks the registries, status manifests, hand
C, committed headers, the compiler-knowledge registry, and recorded
session/blocker history, and returns hits tagged with an *authority level* that
mirrors the source-of-truth hierarchy, so a caller can tell a registry fact from
a source comment.
"""
from __future__ import annotations

import re

from . import schemas
from .project import DecompProject
from .registry import Registry

# authority -> hierarchy rank (1 = highest). Mirrors the source-of-truth order.
AUTHORITY_RANK = {
    "registry": 1,
    "status-manifest": 1,
    "build-output": 2,
    "source": 3,
    "header": 3,
    "doc": 4,
    "compiler-knowledge": 4,
    "session": 4,
    "blocker": 4,
    "comment": 5,
}


def _hit(source_path, location, authority, excerpt, matched=None):
    return {
        "source_path": source_path,
        "location": location,
        "authority": authority,
        "authority_rank": AUTHORITY_RANK.get(authority, 5),
        "excerpt": excerpt[:240],
        "matched_entity": matched,
    }


def _grep_files(project, rel_dir, glob, needle, authority, limit, matched=None):
    hits = []
    base = project.root / rel_dir
    if not base.is_dir():
        return hits
    low = needle.lower()
    for path in sorted(base.rglob(glob)):
        try:
            text = path.read_text(errors="replace")
        except OSError:
            continue
        rel = path.relative_to(project.root).as_posix()
        for i, line in enumerate(text.splitlines(), 1):
            if low in line.lower():
                hits.append(_hit(rel, f"line {i}", authority, line.strip(), matched))
                if len(hits) >= limit:
                    return hits
    return hits


def search_evidence(project: DecompProject, query: str, *, kinds=None,
                    limit=40) -> dict:
    reg = Registry(project)
    q = str(query).strip()
    ql = q.lower()
    kinds = set(kinds) if kinds else None

    def want(kind):
        return kinds is None or kind in kinds

    hits = []

    # 1. function registry (name / id / address substring).
    if want("functions"):
        for fn in reg.functions:
            if (ql in fn.name.lower() or ql in fn.id_str().lower()
                    or ql in f"{fn.address:08x}"):
                hits.append(_hit(f"config/{project.version}/functions.toml",
                                 f"0x{fn.address:08x}", "registry",
                                 f"{fn.name} @ 0x{fn.address:08x} "
                                 f"(unit {fn.unit_index}, {fn.size} bytes)",
                                 fn.id_str()))
                if len(hits) >= limit:
                    break

    # 2. unit registry.
    if want("units") and len(hits) < limit:
        for unit in reg.units.values():
            if ql in unit.name.lower() or ql in unit.mdebug_name.lower():
                hits.append(_hit(f"config/{project.version}/units.toml",
                                 f"unit-{unit.index:04d}", "registry",
                                 f"{unit.name} <- {unit.mdebug_name} "
                                 f"[{unit.category}]", str(unit.index)))
                if len(hits) >= limit:
                    break

    # 3. symbol registry.
    if want("symbols") and len(hits) < limit:
        for name, addr in reg.symbols.items():
            if ql in name.lower() or ql in f"{addr:08x}":
                hits.append(_hit(f"config/{project.version}/symbol_addrs.txt",
                                 f"0x{addr:08x}", "registry",
                                 f"{name} = 0x{addr:08x}", name))
                if len(hits) >= limit:
                    break

    # 4. status manifests.
    if want("status") and len(hits) < limit:
        hits += _grep_files(project, f"config/{project.version}/status", "*.toml",
                            q, "status-manifest", limit - len(hits))

    # 5. source files.
    if want("source") and len(hits) < limit:
        hits += _grep_files(project, "src", "*.c", q, "source", limit - len(hits))

    # 6. headers.
    if want("headers") and len(hits) < limit:
        for rel in ("include", "build/include"):
            if len(hits) >= limit:
                break
            hits += _grep_files(project, rel, "*.h", q, "header", limit - len(hits))

    # 7. compiler knowledge.
    if want("compiler_knowledge") and len(hits) < limit:
        hits += _grep_files(project, f"config/{project.version}", "compiler_knowledge.toml",
                            q, "compiler-knowledge", limit - len(hits))

    # 8. session histories & 9. blocker records.
    if want("sessions") and len(hits) < limit:
        try:
            from . import sessions
            hits += sessions.search_sessions(project, q, limit - len(hits), _hit)
        except Exception:
            pass
    if want("blockers") and len(hits) < limit:
        try:
            from . import blockers
            hits += blockers.search_blockers(project, q, limit - len(hits), _hit)
        except Exception:
            pass

    hits.sort(key=lambda h: h["authority_rank"])
    return schemas.ok("search_evidence", query=q, count=len(hits), hits=hits)


# --- target-assembly / C extraction helpers (shared with context.py) --------


def target_asm_for(project, fn) -> dict:
    """Extract one function's disassembly listing from asm/text.s (if present).

    Returns {"available": bool, "source": rel, "text": str|None, "callees": [...],
    "referenced_symbols": [...]}. The listing is derived (splat output); callers
    gate it behind explicit detail/resource requests.
    """
    path = project.root / "asm" / "text.s"
    if not path.is_file():
        return {"available": False, "reason": "asm/text.s absent (run configure.py)",
                "text": None, "callees": [], "referenced_symbols": []}
    lines = path.read_text(errors="replace").splitlines()
    start = None
    for i, line in enumerate(lines):
        if line.startswith("glabel ") and line.split()[1] == fn.name:
            start = i
            break
    if start is None:
        return {"available": False, "reason": f"{fn.name} not found in asm/text.s",
                "text": None, "callees": [], "referenced_symbols": []}
    end = start + 1
    while end < len(lines) and not lines[end].startswith(("glabel ", "endlabel ")):
        end += 1
    body = lines[start:end]
    callees, syms = [], []
    for line in body:
        m = re.search(r"\bjal\s+([A-Za-z_.$][\w.$]*)", line)
        if m and m.group(1) not in callees:
            callees.append(m.group(1))
        for s in re.findall(r"%(?:gp_rel|hi|lo|call16)\(([A-Za-z_.$][\w.$]*)\)", line):
            if s not in syms:
                syms.append(s)
    return {"available": True, "source": "asm/text.s",
            "text": "\n".join(body), "line_count": len(body),
            "callees": callees, "referenced_symbols": syms}


def c_definition_for(project, fn) -> dict:
    """Best-effort extraction of a function's C definition from its unit source."""
    unit_reg = Registry(project)
    unit = unit_reg.units.get(fn.unit_index)
    if unit is None:
        return {"available": False, "reason": "unit unknown", "text": None}
    path = project.root / unit.source_path
    if not path.is_file():
        return {"available": False, "reason": f"{unit.source_path} absent",
                "text": None, "source": unit.source_path}
    text = path.read_text(errors="replace")
    lines = text.splitlines()
    # Heuristic: a line that starts at column 0 (or after a type), contains the
    # name followed by '(', and is not a call/prototype (no trailing ';').
    pat = re.compile(r"\b" + re.escape(fn.name) + r"\s*\(")
    for i, line in enumerate(lines):
        if pat.search(line) and not line.lstrip().startswith(("//", "*")) \
                and line[:1] not in (" ", "\t"):
            # Walk forward to the matching closing brace.
            depth, started, body = 0, False, []
            for j in range(i, min(len(lines), i + 400)):
                body.append(lines[j])
                depth += lines[j].count("{") - lines[j].count("}")
                if "{" in lines[j]:
                    started = True
                if started and depth <= 0:
                    return {"available": True, "source": unit.source_path,
                            "line": i + 1, "text": "\n".join(body)}
            if not started:      # prototype only
                break
    return {"available": False, "reason": f"{fn.name} not defined in {unit.source_path}",
            "text": None, "source": unit.source_path}
