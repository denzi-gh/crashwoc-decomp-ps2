#!/usr/bin/env python3
"""Check the offset and size comments in include/ against the PS2 compiler.

Struct members in the headers carry their offset as a comment, in one of the
styles the decomp has used:

    float x;                /* 0x4 */           trailing
    /* 0x4 */ float x;                          leading
    float x; // Offset: 0x4, DWARF: 0x1234      DWARF dump
    }; /* 0x10 */                               struct size, trailing
    // Size: 0x10                               struct size, line before
    
Needs the toolchain, so on a host run it through the container:
  python tools/dispatch.py python tools/probe_layout.py                  # all headers
  python tools/dispatch.py python tools/probe_layout.py include/game/creature.h
  python tools/dispatch.py python tools/probe_layout.py --check          # exit 1 on drift
"""
import argparse
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
import cc  # noqa: E402
from check_types import header_misplaced  # noqa: E402

PROBE_SYMBOL = "nu_layout_probe"
HEX = r"0x[0-9A-Fa-f]+"
# Comment text that states an offset: "0x4", "Offset: 0x4, ...". Bit-field
# positions ("0x0:3") are skipped, they have no byte offset.
OFFSET_COMMENT = re.compile(rf"^\s*(?:Offset:\s*)?({HEX})(?!\s*:\s*\d)\b")
SIZE_COMMENT = re.compile(rf"^\s*(?:Size:\s*)?({HEX})\b")
TAG_DEF = re.compile(r"\b(struct|union)\s+(\w+)\s*\{")
TYPEDEF_DEF = re.compile(r"\btypedef\s+(struct|union)\s*(\w*)\s*\{")


def comments_in(text):
    """Comment bodies in order, both styles."""
    return [m.group(1) if m.group(1) is not None else m.group(2)
            for m in re.finditer(r"/\*(.*?)\*/|//([^\n]*)", text, re.S)]


def strip_comments(text):
    return re.sub(r"/\*.*?\*/|//[^\n]*", " ", text, flags=re.S)


def matching_brace(text, open_pos):
    """Index just past the brace that closes text[open_pos] ('{')."""
    depth, i = 0, open_pos
    in_block = in_line = False
    while i < len(text):
        two = text[i:i + 2]
        if in_block:
            if two == "*/":
                in_block, i = False, i + 2
                continue
        elif in_line:
            if text[i] == "\n":
                in_line = False
        elif two == "/*":
            in_block, i = True, i + 2
            continue
        elif two == "//":
            in_line, i = True, i + 2
            continue
        elif text[i] == "{":
            depth += 1
        elif text[i] == "}":
            depth -= 1
            if depth == 0:
                return i + 1
        i += 1
    raise ValueError("unbalanced braces")


def drop_nested(text):
    """Replace every balanced {...} (comments inside included) with {}."""
    out, i = [], 0
    while True:
        j = text.find("{", i)
        if j < 0:
            out.append(text[i:])
            return "".join(out)
        out.append(text[i:j] + "{}")
        i = matching_brace(text, j)


def members(body):
    """Split a struct body into top-level member declarations.

    Yields (offset of the chunk in body, raw chunk). A comment that follows
    the `;` on the same line belongs to the member before it.
    """
    i, start, depth = 0, 0, 0
    first_nl = body.find("\n")
    if first_nl >= 0 and re.fullmatch(r"\s*(?://.*|/\*.*?\*/\s*)?", body[:first_nl]):
        i = start = first_nl                            # `{ // size` line
    while i < len(body):
        two = body[i:i + 2]
        if two == "/*":
            i = body.index("*/", i + 2) + 2
            continue
        if two == "//":
            nl = body.find("\n", i)
            i = len(body) if nl < 0 else nl
            continue
        ch = body[i]
        if ch in "{(":
            depth += 1
        elif ch in "})":
            depth -= 1
        elif ch == ";" and depth == 0:
            # A trailing comment (possibly wrapping over several lines)
            # belongs to this member.
            j = i + 1
            while j < len(body) and body[j] in " \t":
                j += 1
            if body.startswith("/*", j):
                j = body.index("*/", j + 2) + 2
            elif body.startswith("//", j):
                nl = body.find("\n", j)
                j = len(body) if nl < 0 else nl
            end = j if j > i + 1 and body[i + 1:j].strip() else i + 1
            yield start, body[start:end]
            start = i = end
            continue
        i += 1


def declarators(code):
    """Member names declared by one declaration (comments/bodies removed)."""
    code = re.sub(r"\{\}", " ", code).strip().rstrip(";").strip()
    fnptr = re.search(r"\(\s*\*\s*(\w+)\s*\)\s*\(", code)
    if fnptr:
        return [fnptr.group(1)]
    parts = [p.strip() for p in code.split(",")]
    names = []
    for k, part in enumerate(parts):
        if re.search(r":\s*\w+\s*$", part):            # bit-field
            return []
        part = re.sub(r"\[[^\]]*\]", "", part).replace("*", " ")
        words = re.findall(r"[A-Za-z_]\w*", part)
        if not words or (k == 0 and len(words) < 2):
            continue                                    # anonymous member
        names.append(words[-1])
    return names


def parse_header(text):
    """Commented layout facts of every struct/union defined in a header.

    Returns [{type, line, size: (value, line) or None,
              fields: [(name, offset, line)]}].
    """
    found = []
    defs = [(m.start(), m.end() - 1, f"{m.group(1)} {m.group(2)}")
            for m in TAG_DEF.finditer(text)]
    tagged = {pos for pos, _o, _t in defs}
    for m in TYPEDEF_DEF.finditer(text):
        if m.group(2):
            continue                                    # tagged: covered above
        close = matching_brace(text, m.end() - 1)
        name = re.match(r"\s*(\w+)\s*;", text[close:])
        if name and m.start() not in tagged:
            defs.append((m.start(), m.end() - 1, name.group(1)))

    commented = [m.span() for m in re.finditer(r"/\*.*?\*/|//[^\n]*", text, re.S)]
    for start, brace, ctype in sorted(defs):
        if any(lo <= start < hi for lo, hi in commented):
            continue                                    # inside a comment
        close = matching_brace(text, brace)
        body = text[brace + 1:close - 1]
        line0 = text.count("\n", 0, brace + 1) + 1

        size = None
        tail = text[close:text.find("\n", close) if text.find("\n", close) >= 0 else len(text)]
        tail_match = re.match(r"\s*\w*\s*;\s*(?:/\*(.*?)\*/|//(.*))", tail)
        head = text[brace + 1:text.find("\n", brace + 1)]
        head_match = re.match(r"\s*//(.*)", head)
        prev_lines = text[:start].rstrip().splitlines()
        prev = prev_lines[-1].strip() if prev_lines else ""
        def_line = text.count("\n", 0, start) + 1
        candidates = []
        if tail_match:
            candidates.append((tail_match.group(1) or tail_match.group(2),
                               text.count("\n", 0, close) + 1))
        if head_match:
            candidates.append((head_match.group(1), line0))
        if prev.startswith("//") and "Size:" in prev:
            candidates.append((prev[2:], def_line - 1))
        for comment, line in candidates:
            hit = SIZE_COMMENT.match(comment or "")
            if hit:
                size = (int(hit.group(1), 16), line)
                break

        fields = []
        for offset, chunk in members(body):
            flat = drop_nested(chunk)
            names = declarators(strip_comments(flat))
            if not names:
                continue
            lead = len(chunk) - len(chunk.lstrip())
            line = line0 + body.count("\n", 0, offset + lead)
            for comment in comments_in(flat):
                hit = OFFSET_COMMENT.match(comment)
                if hit:
                    fields.append((names[0], int(hit.group(1), 16), line))
                    break
        found.append({"type": ctype, "line": def_line,
                      "size": size, "fields": fields})
    return found


def probe_source(header_rel, structs):
    """C source of the probe and the list of (struct index, what) per word."""
    lines = [f'#include "{header_rel}"', "", f"int {PROBE_SYMBOL}[] = {{"]
    order = []
    for k, s in enumerate(structs):
        lines.append(f"    sizeof({s['type']}),")
        order.append((k, None))
        for name, _off, _line in s["fields"]:
            lines.append(f"    (int)&(({s['type']} *)0)->{name},")
            order.append((k, name))
    lines += ["    0", "};", ""]
    return "\n".join(lines), order


def read_words(asm):
    """The .word values that follow the probe label in ee-gcc output."""
    words, inside = [], False
    for raw in asm.splitlines():
        line = raw.strip()
        if line == f"{PROBE_SYMBOL}:":
            inside = True
            continue
        if not inside:
            continue
        m = re.match(r"\.word\s+(-?(?:0x[0-9A-Fa-f]+|\d+))$", line)
        if m:
            words.append(int(m.group(1), 0))
        elif line and not line.startswith("#"):
            break
    return words


def compare(header_rel, structs, order, words):
    """Mismatch lines; checked count."""
    if len(words) != len(order) + 1:
        return [f"{header_rel}: probe returned {len(words)} values, "
                f"expected {len(order) + 1}"], 0
    out, checked = [], 0
    for (k, name), value in zip(order, words):
        s = structs[k]
        if name is None:
            if s["size"]:
                checked += 1
                if s["size"][0] != value:
                    out.append(f"{header_rel}:{s['size'][1]}: sizeof({s['type']}) "
                               f"comment {s['size'][0]:#x}, compiler {value:#x}")
            continue
        _n, want, line = next(f for f in s["fields"] if f[0] == name)
        checked += 1
        if want != value:
            out.append(f"{header_rel}:{line}: {s['type']}.{name} "
                       f"comment {want:#x}, compiler {value:#x}")
    return out, checked


def probe_header(path, version, profile="default"):
    """(mismatch lines, checked count) for one header."""
    rel = path.resolve().relative_to(ROOT / "include").as_posix()
    structs = [s for s in parse_header(path.read_text(errors="replace"))
               if s["size"] or s["fields"]]
    source, order = probe_source(rel, structs)
    work = ROOT / "build" / version / "probe"
    work.mkdir(parents=True, exist_ok=True)
    stem = re.sub(r"[^A-Za-z0-9_]", "_", rel)
    src, out_s = work / f"{stem}.c", work / f"{stem}.s"
    src.write_text(source)
    try:
        cc.compile_s(src, out_s, profile=profile, version=version)
    except subprocess.CalledProcessError as exc:
        scratch = str(cc._workdir_for(out_s)) + "/"
        errors = [line.replace(scratch, "") for line in
                  (exc.stderr or "").strip().splitlines()
                  if not line.startswith("In file included from")][:2]
        return [f"include/{rel}: does not compile on its own: " + " | ".join(errors)], 0
    return compare(f"include/{rel}", structs, order, read_words(out_s.read_text()))


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("headers", nargs="*", help="headers (default: all of include/)")
    parser.add_argument("--version", default="pal103")
    parser.add_argument("--check", action="store_true", help="exit 1 on any mismatch")
    args = parser.parse_args()

    if not cc.compiler_available("default", args.version):
        print("matching compiler not found; run through the container: "
              "python tools/dispatch.py python tools/probe_layout.py",
              file=sys.stderr)
        return 2
    headers = [Path(h) for h in args.headers] or cc.header_files()
    sources = {p.relative_to(ROOT).as_posix() for p in (ROOT / "src").rglob("*.c")}
    total, bad, legacy = 0, [], []
    for header in headers:
        rel = header.resolve().relative_to(ROOT).as_posix()
        lines, checked = probe_header(header, args.version)
        total += checked
        # Headers outside the include/ layout (check_types: misplaced_headers)
        # are being dissolved; they are reported but never fail --check.
        (legacy if header_misplaced(rel, sources) else bad).extend(lines)
        print(f"{rel}: {checked} checked, {len(lines)} wrong")
    for line in bad:
        print(line)
    for line in legacy:
        print(f"{line}  [legacy header, not gating]")
    print(f"{total} layout comments checked, {len(bad) + len(legacy)} wrong"
          f" ({len(legacy)} in legacy headers)")
    return 1 if args.check and bad else 0


if __name__ == "__main__":
    sys.exit(main())
