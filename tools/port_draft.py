#!/usr/bin/env python3
"""Pull a GameCube-decomp draft for a PS2 function into a scratch file.

Given a PS2 function (canonical id or bare name), look it up in
config/<version>/gc_xref.toml, slice the twin definition out of the GC
checkout, and emit a *draft* to build/<version>/gc_drafts/<unit>/<fn>.c
(gitignored). The draft is a rank-6 hint, never a finished match: it is
deliberately neutralised where GC layout facts are HOSTILE DATA.

Transforms applied (deterministic):
  * every `sizeof(...)` expression   -> /* TODO(ps2-layout) */
  * the size argument of memset / memcpy / memmove / memclr / bzero
                                     -> /* TODO(ps2-layout) */
  * known identifier renames (GC -> PS2), word-boundary
  * tabs -> 4 spaces
The original GC lines stay one click away (the header cites gc_file:lines);
struct literals and hard offsets cannot be auto-detected -- the header warns
that they must be re-derived from the PS2 disassembly by hand.

This tool NEVER writes into src/. Stdlib only; reads gc_xref.toml + the
(gitignored) GC checkout.

Usage:
  python tools/port_draft.py pal103:unit-0118:002592f8:InitChase
  python tools/port_draft.py InitChase          # resolve by name
  python tools/port_draft.py InitChase --gc-root <path> --out <dir>
"""
import argparse
import re
import sys
import tomllib
from pathlib import Path

REPO = Path(__file__).resolve().parent.parent

# GC identifier -> PS2 identifier. Word-boundary rename applied to every draft.
# Seed with verified renames only; the mechanism is here, the table grows as
# renames are proven against the PS2 disassembly.
RENAMES = {}

MEMSET_FUNCS = {"memset", "memcpy", "memmove", "memclr", "bzero"}
TODO = "/* TODO(ps2-layout) */"


def load_xref(config_dir):
    path = config_dir / "gc_xref.toml"
    if not path.is_file():
        raise FileNotFoundError(f"{path} not found; run tools/xref_gc.py first")
    return tomllib.loads(path.read_text()).get("xref", [])


def resolve(xref, target):
    """Return the single xref row for `target` (id or bare name)."""
    if ":" in target:
        rows = [r for r in xref if r["id"] == target]
    else:
        rows = [r for r in xref if r["id"].split(":")[-1] == target]
    return rows


def _find_call_span(text, start):
    """Given index of '(' at `start`, return index just past the matching
    ')'. Returns -1 if unbalanced."""
    depth = 0
    i = start
    while i < len(text):
        c = text[i]
        if c == "(":
            depth += 1
        elif c == ")":
            depth -= 1
            if depth == 0:
                return i + 1
        i += 1
    return -1


def replace_sizeof(text):
    """Replace every `sizeof(...)` (balanced) with the TODO marker."""
    out = []
    i = 0
    while True:
        m = re.compile(r"\bsizeof\s*\(").search(text, i)
        if not m:
            out.append(text[i:])
            break
        out.append(text[i:m.start()])
        end = _find_call_span(text, m.end() - 1)
        if end < 0:
            out.append(text[m.start():])
            break
        out.append(TODO)
        i = end
    return "".join(out)


def _split_top_args(arglist):
    """Split a call's argument string on top-level commas."""
    args = []
    depth = 0
    cur = []
    for c in arglist:
        if c in "([{":
            depth += 1
        elif c in ")]}":
            depth -= 1
        if c == "," and depth == 0:
            args.append("".join(cur))
            cur = []
        else:
            cur.append(c)
    args.append("".join(cur))
    return args


def replace_memset_size(text):
    """Neutralise the size (last) argument of memset-family calls."""
    out = []
    i = 0
    call_re = re.compile(r"\b(" + "|".join(sorted(MEMSET_FUNCS)) + r")\s*\(")
    while True:
        m = call_re.search(text, i)
        if not m:
            out.append(text[i:])
            break
        open_paren = m.end() - 1
        end = _find_call_span(text, open_paren)
        if end < 0:
            out.append(text[i:m.end()])
            i = m.end()
            continue
        inner = text[open_paren + 1:end - 1]
        args = _split_top_args(inner)
        if len(args) >= 2:
            args[-1] = " " + TODO
        out.append(text[i:m.start()])
        out.append(m.group(1) + "(" + ",".join(args) + ")")
        i = end
    return "".join(out)


def apply_renames(text, mapping):
    for gc_name, ps2_name in sorted(mapping.items()):
        text = re.sub(r"\b" + re.escape(gc_name) + r"\b", ps2_name, text)
    return text


def make_draft(row, gc_root):
    fn = row["id"].split(":")[-1]
    gc_path = gc_root / row["gc_file"]
    lines = gc_path.read_text(encoding="utf-8", errors="replace").split("\n")
    start, end = row["gc_line_start"], row["gc_line_end"]
    body = "\n".join(lines[start - 1:end])
    body = body.replace("\t", "    ")
    body = replace_sizeof(body)
    body = replace_memset_size(body)
    body = apply_renames(body, RENAMES)

    header = [
        f"/* GC draft for {row['id']}",
        f" * PS2 unit {row['unit']}, addr {row['addr']:#010x}, "
        f"state={row['state']}.",
        f" * source: {row['gc_file']}:{start}-{end} "
        f"(confidence={row['confidence']}).",
        " *",
        " * RANK-6 HINT ONLY. Re-derive from the PS2 disassembly:",
        " *   - every branch direction, switch constant, jal <-> call",
        " *   - every struct offset / sizeof / memset length (marked with a",
        " *     TODO(ps2-layout) comment) and every struct literal / hard",
        " *     offset (NOT auto-detected -- verify each by hand).",
        " * Never copy this into src/ unmodified. */",
        "",
    ]
    return fn, "\n".join(header) + body + "\n"


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("target", help="PS2 function id or bare name")
    parser.add_argument("--gc-root", default=str(REPO / "reference" / "gc"))
    parser.add_argument("--config", default=str(REPO / "config" / "pal103"))
    parser.add_argument("--out", default=None,
                        help="draft dir (default build/<version>/gc_drafts)")
    args = parser.parse_args()

    config_dir = Path(args.config)
    gc_root = Path(args.gc_root)
    version = config_dir.name
    out_root = Path(args.out) if args.out else \
        REPO / "build" / version / "gc_drafts"

    try:
        xref = load_xref(config_dir)
    except FileNotFoundError as exc:
        print(exc, file=sys.stderr)
        return 2

    rows = resolve(xref, args.target)
    if not rows:
        print(f"no GC reference for '{args.target}' in gc_xref.toml",
              file=sys.stderr)
        return 1
    if len(rows) > 1:
        print(f"'{args.target}' is ambiguous -- pass a canonical id:",
              file=sys.stderr)
        for r in rows:
            print(f"  {r['id']}  ({r['gc_file']})", file=sys.stderr)
        return 1

    row = rows[0]
    if not (gc_root / row["gc_file"]).is_file():
        print(f"GC source {row['gc_file']} not found under {gc_root}",
              file=sys.stderr)
        return 2

    fn, draft = make_draft(row, gc_root)
    out_dir = out_root / row["unit"]
    out_dir.mkdir(parents=True, exist_ok=True)
    out_path = out_dir / f"{fn}.c"
    out_path.write_text(draft)
    rel = out_path.relative_to(REPO) if REPO in out_path.parents else out_path
    print(f"wrote {rel} (from {row['gc_file']}:"
          f"{row['gc_line_start']}-{row['gc_line_end']}, "
          f"confidence={row['confidence']})")
    return 0


if __name__ == "__main__":
    sys.exit(main())
