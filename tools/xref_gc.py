#!/usr/bin/env python3
"""Cross-reference PS2 retail functions against the GameCube decompilation.

The GameCube decomp (denzi-gh/crashwoc-decomp) shares the SN ProDG gcc 2.95.2
family and a large fraction of portable game code with this PS2 matching
decomp. GC C is a rank-6 structural hint only -- but knowing *which* GC file /
line range holds the twin of a given PS2 function lets tools/port_draft.py pull
a draft to reconcile against the PS2 disassembly.

This scanner:
  * loads every PS2 function from the committed status manifests
    (config/<version>/status/**/*.toml) -- id, address, canonical unit
    (from the manifest `source` path) and state;
  * regex/brace-scans every GC C source under <gc-root>/src/** for function
    *definitions* (ANSI or K&R), recording name -> (file, line range);
  * matches by name, assigning a confidence:
      exact -- the GC file maps to the same PS2 unit (via the dir/file
               relocation table below), so name + location agree;
      name  -- name-only match (GC twin lives in a different/unknown unit).

Known GC->PS2 relocations (dir-level and file-level) are baked in:
  GC gamecode/*        -> PS2 game/*
  GC nu3dx/*           -> PS2 nu3d/*
  GC nu3dx/nuglass.c   -> PS2 game/glass.c
  GC nusound/sfx.c     -> PS2 game/sfx.c

Output config/<version>/gc_xref.toml is deterministic (sorted by unit then
address); --check verifies the committed file equals a fresh scan, like the
other extractors. Stdlib only; reads the (gitignored) GC checkout, never the
game ELF.

Usage:
  python tools/xref_gc.py                       # regenerate the table
  python tools/xref_gc.py --check               # committed == fresh
  python tools/xref_gc.py --gc-root <path>      # override GC checkout
"""
import argparse
import re
import sys
import tomllib
from pathlib import Path

REPO = Path(__file__).resolve().parent.parent

# GC source dir -> PS2 canonical unit dir. Dirs not listed map to themselves.
DIR_MAP = {
    "gamecode": "game",
    "nu3dx": "nu3d",
}

# GC (dir, basename) -> explicit PS2 canonical unit, overriding DIR_MAP.
FILE_MAP = {
    ("nu3dx", "nuglass"): "game/glass",
    ("nusound", "sfx"): "game/sfx",
}

# Reserved words that can begin a col-0 line but never start a function
# definition. `struct`/`enum`/`union` are intentionally absent: they legally
# begin a struct-returning function's signature (e.g. `struct s *Foo(...)`),
# and bare aggregate definitions (`struct s {`) carry no '(' so are filtered
# by the paren check anyway.
NON_DEF_STARTS = {
    "if", "for", "while", "switch", "do", "else", "return", "case",
    "typedef", "static_assert", "goto", "sizeof",
}

IDENT_RE = re.compile(r"[A-Za-z_][A-Za-z0-9_]*")


def expected_unit(gc_dir, gc_base):
    """The PS2 canonical unit a GC file is expected to land in."""
    if (gc_dir, gc_base) in FILE_MAP:
        return FILE_MAP[(gc_dir, gc_base)]
    return f"{DIR_MAP.get(gc_dir, gc_dir)}/{gc_base}"


def strip_line_comment(s):
    """Remove a trailing // comment (naive; ignores // inside strings)."""
    i = s.find("//")
    return s[:i] if i >= 0 else s


def extract_name(signature):
    """Last identifier before the first '(' in a function signature."""
    head = signature.split("(", 1)[0]
    idents = IDENT_RE.findall(head)
    return idents[-1] if idents else None


def find_body_end(lines, open_idx):
    """Index of the col-0 '}' closing a function whose body opens at/near
    open_idx. GC bodies (K&R style) close with '}' at column 0."""
    for k in range(open_idx + 1, len(lines)):
        if lines[k][:1] == "}":
            return k
    return len(lines) - 1


def scan_c_file(text):
    """Yield (name, line_start, line_end) 1-based for each definition."""
    lines = text.split("\n")
    n = len(lines)
    defs = []
    i = 0
    while i < n:
        line = lines[i]
        first = line[:1]
        if not first or first.isspace() or first == "#" or first == "}" \
                or first == "/" or first == "*":
            i += 1
            continue
        word = IDENT_RE.match(line)
        if not word or word.group(0) in NON_DEF_STARTS or "(" not in line:
            i += 1
            continue

        # Accumulate the parameter list, tracking paren depth across lines.
        depth = 0
        started = False
        sig_parts = []
        j = i
        close_line = None
        while j < n and j < i + 30:
            seg = strip_line_comment(lines[j])
            sig_parts.append(seg)
            for ch in seg:
                if ch == "(":
                    depth += 1
                    started = True
                elif ch == ")":
                    depth -= 1
            if started and depth <= 0:
                close_line = j
                break
            j += 1
        if close_line is None:
            i += 1
            continue

        name = extract_name(" ".join(sig_parts))
        if not name:
            i += 1
            continue

        # After the param list closes, peek the next significant char to
        # classify: '{' = ANSI def, ';' = declaration, identifier = K&R
        # param declarations preceding the body.
        tail = strip_line_comment(lines[close_line])
        rp = tail.rfind(")")
        rest = tail[rp + 1:].strip() if rp >= 0 else ""
        kind = None
        open_idx = close_line
        scan = close_line
        buf = rest
        while True:
            buf = buf.strip()
            if buf.startswith("{"):
                kind = "def"
                open_idx = scan
                break
            if buf.startswith(";"):
                kind = "decl"
                break
            if buf:
                # K&R param declaration or attribute; consume to next line.
                if scan >= close_line + 12:
                    break
                scan += 1
                if scan >= n:
                    break
                buf = strip_line_comment(lines[scan])
                continue
            # blank tail: advance a line
            if scan >= close_line + 12:
                break
            scan += 1
            if scan >= n:
                break
            buf = strip_line_comment(lines[scan])

        if kind == "def":
            end = find_body_end(lines, open_idx)
            defs.append((name, i + 1, end + 1))
            i = end + 1
            continue
        i += 1
    return defs


def load_ps2_functions(config_dir):
    """List of dicts for every function carried by a status manifest."""
    status_dir = config_dir / "status"
    funcs = []
    for path in sorted(status_dir.rglob("*.toml")):
        data = tomllib.loads(path.read_text())
        source = data.get("source", "")
        # src/game/chase.c -> game/chase
        unit = source[len("src/"):-len(".c")] if source.startswith("src/") \
            and source.endswith(".c") else source
        for fn in data.get("function", []):
            fid = fn.get("id", "")
            parts = fid.split(":", 3)
            if len(parts) != 4:
                continue
            _, unit_tok, addr_hex, name = parts
            funcs.append({
                "id": fid,
                "unit": unit,
                "addr": int(addr_hex, 16),
                "name": name,
                "state": fn.get("state", "asm"),
            })
    return funcs


def scan_gc(gc_root):
    """name -> sorted list of (gc_rel_path, dir, base, start, end)."""
    src = gc_root / "src"
    index = {}
    for cfile in sorted(src.rglob("*.c")):
        rel = cfile.relative_to(gc_root).as_posix()
        parts = cfile.relative_to(src).parts
        gc_dir = parts[0] if len(parts) > 1 else ""
        base = cfile.stem
        try:
            text = cfile.read_text(encoding="utf-8", errors="replace")
        except OSError:
            continue
        for name, start, end in scan_c_file(text):
            index.setdefault(name, []).append((rel, gc_dir, base, start, end))
    for name in index:
        index[name].sort()
    return index


def base_name(name):
    """Strip a NAME__<vram> disambiguation suffix for GC matching."""
    m = re.match(r"^(.*)__[0-9a-fA-F]{6,8}$", name)
    return m.group(1) if m else name


def build_xref(config_dir, gc_root):
    funcs = load_ps2_functions(config_dir)
    gc_index = scan_gc(gc_root)
    rows = []
    for fn in funcs:
        for cand_name in (fn["name"], base_name(fn["name"])):
            cands = gc_index.get(cand_name)
            if cands:
                break
        else:
            continue
        exact = [c for c in cands if expected_unit(c[1], c[2]) == fn["unit"]]
        if exact:
            chosen = exact[0]
            confidence = "exact"
        elif len(cands) == 1:
            chosen = cands[0]
            confidence = "name"
        else:
            chosen = cands[0]
            confidence = "name"
        rel, _gc_dir, _base, start, end = chosen
        rows.append({
            "id": fn["id"],
            "unit": fn["unit"],
            "addr": fn["addr"],
            "state": fn["state"],
            "gc_file": rel,
            "gc_line_start": start,
            "gc_line_end": end,
            "confidence": confidence,
        })
    rows.sort(key=lambda r: (r["unit"], r["addr"]))
    return funcs, rows


def render(funcs, rows, gc_root):
    n_total = len(funcs)
    n_ref = len(rows)
    n_exact = sum(1 for r in rows if r["confidence"] == "exact")
    n_name = n_ref - n_exact
    n_asm_ref = sum(1 for r in rows if r["state"] == "asm")
    lines = [
        "# Generated by tools/xref_gc.py -- do not edit by hand.",
        "# Cross-reference of PS2 retail functions to their GameCube decomp",
        "# twins (rank-6 structural hints; re-derive every fact from PS2 asm).",
        f"# {n_ref} of {n_total} status-carried PS2 functions have a GC",
        f"# reference: {n_exact} exact (unit agrees), {n_name} name-only.",
        f"# {n_asm_ref} of the referenced functions are still state=asm.",
        "",
    ]
    for r in rows:
        lines.append("[[xref]]")
        lines.append(f"id = {r['id']!r}")
        lines.append(f"unit = {r['unit']!r}")
        lines.append(f"addr = {r['addr']:#010x}")
        lines.append(f"state = {r['state']!r}")
        lines.append(f"gc_file = {r['gc_file']!r}")
        lines.append(f"gc_line_start = {r['gc_line_start']}")
        lines.append(f"gc_line_end = {r['gc_line_end']}")
        lines.append(f"confidence = {r['confidence']!r}")
        lines.append("")
    text = "\n".join(lines)
    if not text.endswith("\n"):
        text += "\n"
    stats = dict(total=n_total, ref=n_ref, exact=n_exact, name=n_name,
                 asm_ref=n_asm_ref)
    return text, stats


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--gc-root", default=str(REPO / "reference" / "gc"),
                        help="GameCube decomp checkout (default reference/gc)")
    parser.add_argument("--config", default=str(REPO / "config" / "pal103"),
                        help="PS2 config dir (default config/pal103)")
    parser.add_argument("--check", action="store_true",
                        help="verify committed gc_xref.toml == fresh scan")
    args = parser.parse_args()

    gc_root = Path(args.gc_root)
    config_dir = Path(args.config)
    if not (gc_root / "src").is_dir():
        print(f"GC checkout not found at {gc_root} (create the reference/gc "
              f"junction or pass --gc-root)", file=sys.stderr)
        return 2

    funcs, rows = build_xref(config_dir, gc_root)
    text, stats = render(funcs, rows, gc_root)
    out = config_dir / "gc_xref.toml"
    rel = out.relative_to(REPO) if out.is_absolute() and REPO in out.parents \
        else out

    if args.check:
        if out.is_file() and out.read_text() == text:
            print(f"{rel}: PASS")
            return 0
        print(f"{rel}: FAIL -- differs from a fresh scan "
              f"(or does not exist); regenerate it.", file=sys.stderr)
        return 1

    out.write_text(text)
    print(f"wrote {rel}")
    print(f"  {stats['ref']} / {stats['total']} PS2 functions referenced "
          f"({stats['exact']} exact, {stats['name']} name-only); "
          f"{stats['asm_ref']} still asm.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
