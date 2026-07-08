"""compile_diff: the main iteration operation.

Given a canonical function id, this compiles the affected C unit through the
*existing locked path* and compares the result to the retail bytes over the
function's full canonical extent. It never invents a compiler pipeline: it
builds the same kind of matching hybrid ``tools/promote.py`` would, so a result
of ``exact=true`` means exactly what a promotion means -- byte equality over the
whole extent, not a matching prefix.

To diff a function still marked ``asm`` (the normal case during iteration), a
*temporary* manifest is synthesized in a gitignored scratch location with the
target forced to ``matching`` and every other function left as a retail slice.
The committed manifest is never touched, no state is changed, and no gate is
weakened -- the hybrid is byte-identical to what promotion would build.

Toolchain-bound: runs only where the compiler + PS2 binutils exist (the
Containerfile image / a native Linux install). On a host without them it returns
a ``BLOCKED_TOOLING`` result with the exact repair command, never a guess.
"""
from __future__ import annotations

import hashlib
import re
import tempfile
from pathlib import Path

from . import schemas, classify, evidence, _bridge
from .project import DecompProject
from .registry import Registry


def compile_diff(project: DecompProject, target, *, session_id=None,
                 client=None) -> dict:
    reg = Registry(project)
    res = reg.resolve(target)
    if res.status == "ambiguous":
        return schemas.err("compile_diff", res.detail, code="ambiguous",
                           candidates=res.candidates)
    if res.status != "resolved" or res.record is None:
        return schemas.err("compile_diff", res.detail or "unresolved",
                           code="not_found")
    fn = res.record
    unit = reg.units.get(fn.unit_index)
    profile, _ = reg.effective_profile(fn.unit_index)

    if not _bridge.repo_matches(project):
        return schemas.err(
            "compile_diff",
            "compile/diff is only available for the repository this package "
            "ships in (toolchain scripts resolve their own root)",
            code="root_mismatch")

    # Route into the container when the toolchain is not local (e.g. a Windows
    # host); inside the container this short-circuits and runs in-process.
    from . import runtime
    if not runtime.toolchain_local(project):
        args = ["compile-diff", fn.id_str()]
        if session_id:
            args += ["--session", session_id]
        if client:
            args += ["--client", client]
        return runtime.dispatch_cli(project, args, operation="compile_diff")

    # Toolchain + assets preconditions (cheap; never guess past them).
    guard = _preconditions(project, profile)
    if guard is not None:
        return guard

    cdef = evidence.c_definition_for(project, fn)
    if not cdef.get("available"):
        return schemas.err(
            "compile_diff",
            f"{fn.name} is not defined in {unit.source_path}; write the C first",
            code="missing_c", outcome_hint="BLOCKED_MISSING_CONTEXT",
            source_path=unit.source_path)

    try:
        built = _build_and_compare(project, reg, fn, unit, profile)
    except _DiffToolingError as exc:
        return schemas.err("compile_diff", str(exc), code="tooling",
                           outcome_hint="BLOCKED_TOOLING")

    if built.get("hybrid_error"):
        classifications = classify.classify(
            compile_ok=built["compile_ok"], target_fn=fn,
            hybrid_error=built["hybrid_error"], exact=False)
        return _finish(project, fn, unit, profile, built, classifications,
                       session_id, client)

    classifications = classify.classify(
        compile_ok=True, unresolved=built.get("unresolved"),
        target_fn=fn, candidate_frame=built.get("candidate_frame"),
        candidate_reg_mask=built.get("candidate_reg_mask"),
        candidate_freg_mask=built.get("candidate_freg_mask"),
        windows=built.get("windows"), exact=built["exact"])
    return _finish(project, fn, unit, profile, built, classifications,
                   session_id, client)


# --- build + compare --------------------------------------------------------


class _DiffToolingError(Exception):
    pass


def _preconditions(project, profile):
    try:
        cc = _bridge.import_tool("cc")
        tc = _bridge.import_tool("declib.toolchain")
    except Exception as exc:                                   # pragma: no cover
        return schemas.err("compile_diff", f"cannot import toolchain modules: {exc}",
                           code="tooling")
    missing = []
    if not cc.compiler_available(profile, project.version):
        missing.append("compiler")
    if not tc.tool_path(tc.LD):
        missing.append("linker")
    if not tc.tool_path(tc.NM):
        missing.append("nm")
    if not tc.tool_path(tc.AS):
        missing.append("assembler")
    if missing:
        return schemas.err(
            "compile_diff",
            f"locked toolchain unavailable: {', '.join(missing)}",
            code="tooling", outcome_hint="BLOCKED_TOOLING",
            repair="run via `python tools/dispatch.py` (Containerfile image) "
                   "or install with tools/setup_toolchain.py")
    # Target ELF present?
    try:
        asmtext = _bridge.import_tool("declib.asmtext")
        asmtext.load_text_target(project.version)
    except Exception:
        return schemas.err("compile_diff", "target ELF absent (orig/)",
                           code="tooling", outcome_hint="BLOCKED_TOOLING",
                           repair="place the retail ELF under orig/ (README.md)")
    return None


def _build_and_compare(project, reg, fn, unit, profile):
    cc = _bridge.import_tool("cc")           # noqa: F841 (ensures tools on path)
    gen_hybrid = _bridge.import_tool("gen_hybrid")
    asmtext = _bridge.import_tool("declib.asmtext")
    verify = _bridge.import_tool("declib.verify")
    tc = _bridge.import_tool("declib.toolchain")

    scratch = project.root / "build" / project.version / "agent_diff" / unit.name
    scratch.mkdir(parents=True, exist_ok=True)
    manifest_path = scratch / "manifest.toml"
    manifest_path.write_text(_temp_manifest(reg, fn, unit, profile))
    out_o = scratch / "candidate.o"

    try:
        spliced_s = gen_hybrid.build_hybrid(manifest_path, out_o,
                                            link_set="matching",
                                            version=project.version)
    except gen_hybrid.HybridError as exc:
        return {"compile_ok": True, "hybrid_error": str(exc), "exact": False}
    except FileNotFoundError as exc:
        raise _DiffToolingError(f"missing artifact: {exc}")
    except Exception as exc:  # subprocess/compile failures
        msg = str(exc)
        if "returned non-zero" in msg or "CalledProcess" in type(exc).__name__:
            return {"compile_ok": False, "compile_error": msg, "exact": False}
        raise _DiffToolingError(msg)

    nm_bin = tc.tool_path(tc.NM)
    offsets = verify.defined_function_offsets(nm_bin, out_o)
    if fn.name not in offsets:
        return {"compile_ok": True,
                "hybrid_error": f"{fn.name} not defined in built object",
                "exact": False}

    undef = verify.undefined_externals(nm_bin, out_o)
    defsyms, unresolved = asmtext.resolve(sorted(undef - {"_gp"}),
                                          asmtext.load_symbol_addrs(project.version))

    elf, text_addr, text_off, _size = asmtext.load_text_target(project.version)
    want = elf[text_off + (fn.address - text_addr):
               text_off + (fn.address - text_addr) + fn.size]

    linked_ok, got = False, b""
    if not unresolved:
        base_addr = min(o[1] for o in _unit_offsets(reg, fn, offsets))
        with tempfile.TemporaryDirectory() as tmp:
            linked = verify.link_text_at(out_o, base_addr, defsyms, Path(tmp))
        off = offsets[fn.name]
        got = linked[off: off + fn.size]
        linked_ok = True

    exact = linked_ok and got == want and len(got) == fn.size
    windows, matching_bytes = _diff_words(want, got) if linked_ok else ([], 0)

    frame, rmask, fmask = _parse_frame(Path(spliced_s), fn.name)
    cand_seg = _extract_segment(Path(spliced_s), fn.name)

    return {
        "compile_ok": True,
        "exact": exact,
        "linked": linked_ok,
        "unresolved": unresolved,
        "want": want,
        "got": got,
        "windows": windows,
        "matching_bytes": matching_bytes,
        "candidate_frame": frame,
        "candidate_reg_mask": rmask,
        "candidate_freg_mask": fmask,
        "candidate_asm_hash": hashlib.sha256(cand_seg.encode()).hexdigest()
        if cand_seg else None,
        "candidate_size": None,
    }


def _finish(project, fn, unit, profile, built, classifications, session_id, client):
    want = built.get("want", b"")
    got = built.get("got", b"")
    size = fn.size
    matching_bytes = built.get("matching_bytes", 0)
    differing = size - matching_bytes if built.get("linked") else None
    fuzzy = round(100.0 * matching_bytes / size, 3) if size and built.get("linked") else None
    target_hash = hashlib.sha256(want).hexdigest() if want else None
    diff_sig = _diff_signature(built.get("windows"), size, built.get("exact"))

    improved = None
    if session_id:
        improved = _compare_to_best(project, session_id, built.get("exact"),
                                    fuzzy, matching_bytes)

    windows_out = [{"offset": off, "offset_hex": f"0x{off:x}",
                    "target_word": f"0x{tw:08x}", "candidate_word": f"0x{cw:08x}"}
                   for off, tw, cw in (built.get("windows") or [])[:24]]

    return schemas.ok(
        "compile_diff",
        function_id=fn.id_str(),
        exact=bool(built.get("exact")),
        address_hex=f"0x{fn.address:08x}",
        extent=[fn.address, fn.end],
        target_size=size,
        candidate_linked=bool(built.get("linked")),
        matching_bytes=matching_bytes if built.get("linked") else None,
        differing_bytes=differing,
        fuzzy_score=fuzzy,
        stack_frame={"target": fn.frame_size, "candidate": built.get("candidate_frame")},
        saved_registers={"target_reg_mask": f"0x{fn.reg_mask:08x}",
                         "candidate_reg_mask": (f"0x{built['candidate_reg_mask']:08x}"
                                                if built.get("candidate_reg_mask") is not None
                                                else None)},
        unresolved_symbols=built.get("unresolved") or [],
        hybrid_error=built.get("hybrid_error"),
        compiler_ok=built.get("compile_ok"),
        mismatch_classifications=classifications,
        mismatch_windows=windows_out,
        candidate_asm_hash=built.get("candidate_asm_hash"),
        target_extent_hash=target_hash,
        diff_signature=diff_sig,
        improved_over_best=improved,
        recommended_next_evidence=_recommend(classifications),
        unknowns=["exact prototype/types not verified"] if not built.get("exact") else [],
        outcome_hint="MATCHING" if built.get("exact") else None,
    )


# --- helpers ----------------------------------------------------------------


def _temp_manifest(reg, fn, unit, profile):
    lines = ["schema = 1",
             f'unit = "{reg.version}:unit-{fn.unit_index:04d}"',
             f'source = "{unit.source_path}"',
             f'profile = "{profile}"', "complete = false", ""]
    for f in sorted(reg.by_unit.get(fn.unit_index, []), key=lambda r: r.address):
        state = "matching" if f.address == fn.address else "asm"
        lines += ["[[function]]", f'id = "{f.id_str()}"', f'state = "{state}"', ""]
    return "\n".join(lines)


def _unit_offsets(reg, fn, offsets):
    out = []
    by_name = {f.name: f for f in reg.by_unit.get(fn.unit_index, [])}
    for name, off in offsets.items():
        rec = by_name.get(name)
        if rec is not None:
            out.append((rec.address, rec.address))     # base = the smallest addr
    if not out:
        out.append((fn.address, fn.address))
    return out


def _diff_words(want, got):
    windows, matching = [], 0
    n = max(len(want), len(got))
    for off in range(0, n, 4):
        tw = int.from_bytes(want[off:off + 4].ljust(4, b"\0"), "little")
        cw = int.from_bytes(got[off:off + 4].ljust(4, b"\0"), "little")
        if tw == cw and off + 4 <= len(want) and off + 4 <= len(got):
            matching += 4
        else:
            windows.append((off, tw, cw))
    return windows, matching


def _diff_signature(windows, size, exact):
    if exact:
        return hashlib.sha256(b"EXACT").hexdigest()
    payload = f"{size}|" + ";".join(f"{o}:{tw:08x}:{cw:08x}"
                                    for o, tw, cw in (windows or []))
    return hashlib.sha256(payload.encode()).hexdigest()


_FRAME_RE = re.compile(r"^\s*\.frame\s+\$\w+,\s*(\d+)")
_MASK_RE = re.compile(r"^\s*\.mask\s+(0x[0-9A-Fa-f]+)")
_FMASK_RE = re.compile(r"^\s*\.fmask\s+(0x[0-9A-Fa-f]+)")
_ENT_RE = re.compile(r"^\s*\.ent\s+(\S+)")
_END_RE = re.compile(r"^\s*\.end\s+(\S+)")


def _parse_frame(spliced_s, name):
    if not spliced_s.is_file():
        return None, None, None
    frame = rmask = fmask = None
    in_seg = False
    for line in spliced_s.read_text().splitlines():
        m = _ENT_RE.match(line)
        if m:
            in_seg = m.group(1) == name
            continue
        if not in_seg:
            continue
        if _END_RE.match(line):
            break
        fm = _FRAME_RE.match(line)
        if fm:
            frame = int(fm.group(1))
        mm = _MASK_RE.match(line)
        if mm:
            rmask = int(mm.group(1), 16)
        xm = _FMASK_RE.match(line)
        if xm:
            fmask = int(xm.group(1), 16)
    return frame, rmask, fmask


def _extract_segment(spliced_s, name):
    if not spliced_s.is_file():
        return ""
    out, in_seg = [], False
    for line in spliced_s.read_text().splitlines():
        m = _ENT_RE.match(line)
        if m and m.group(1) == name:
            in_seg = True
        if in_seg:
            out.append(line)
        if in_seg and _END_RE.match(line) and _END_RE.match(line).group(1) == name:
            break
    return "\n".join(out)


def _compare_to_best(project, session_id, exact, fuzzy, matching_bytes):
    try:
        from . import sessions
        ledger = sessions._read_ledger(project, session_id)
    except Exception:
        return None
    best = ledger.get("_best_score")
    score = float("inf") if exact else (fuzzy if fuzzy is not None else matching_bytes or 0)
    if best is None:
        return True
    return score > best


def _recommend(classifications):
    cats = {c["category"] for c in classifications}
    if "stack_frame" in cats or "saved_registers" in cats:
        return "check the C prototype and local variable types (frame/mask driven)"
    if "register_allocation" in cats or "instruction_scheduling" in cats:
        return "allocation/scheduling sensitive -- consider deeper reasoning or reordering locals"
    if "immediate_materialization" in cats:
        return "check constant/immediate values and their types"
    if "control_flow" in cats or "branch_direction" in cats:
        return "check loop/branch structure vs target disassembly"
    if "literal_pool" in cats or "unsupported_lit8" in cats:
        return "float/double literal handling -- may be an architectural blocker"
    return "inspect the mismatch windows against the target disassembly"
