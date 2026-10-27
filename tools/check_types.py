#!/usr/bin/env python3
"""Census of the type problems that block shared headers and a 64-bit build.

Every unit carries its own copy of the structs, typedefs, externs and
prototypes it needs, so the same type is defined many times, sometimes with
different layouts or names. This tool counts what has to go before the C can
share headers and build for a 64-bit host, and ratchets the counts so they
can only go down:

  struct_redefinitions   struct/union tags defined in more than one file
                         (value = definitions beyond the first)
  typedef_redefinitions  typedef names defined in more than one file
  long_types             `long` that is not `long long`: 64-bit on the EE,
                         32-bit on Win64 and in o32 type tools
  varargs_hacks          files with a local `__builtin_args_info` va_start
  offset_casts           byte-offset pointer arithmetic, e.g.
                         `(char *)p + 0x10` or `(T *)((u32)p + 0x10)`
  sizeof_pads            struct members whose array size uses sizeof
                         (the layout breaks once the inner struct grows)
  libc_prototypes        local prototypes of C library functions
  extern_conflicts       globals declared extern with different types
  prototype_conflicts    functions declared with different signatures
  misplaced_headers      headers outside the include/ layout: the root holds
                         only the platform headers (PLATFORM_HEADERS);
                         include/<lib>/ holds <lib>types.h and one
                         <module>.h per src/<lib>/<module>.c

Scope: src/ and include/, except src/sdk/ (the Sony runtime keeps its own
headers). Stdlib only, no game files: runs in validate.yml.

The baseline (config/type_census.toml) holds only counts. --check fails when
a count goes up, and also when one goes down, so an improvement is recorded
by the same change that made it (python tools/check_types.py --update).

Usage:
  python tools/check_types.py                       # summary + top files
  python tools/check_types.py --metric long_types   # every site of one metric
  python tools/check_types.py --json
  python tools/check_types.py --check | --update
"""
import argparse
import json
import re
import sys
import tomllib
from collections import defaultdict
from pathlib import Path

REPO = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(Path(__file__).resolve().parent))
from check_prototypes import (KEYWORDS, SIGNATURE, SOFT_FLOAT,  # noqa: E402
                              strip_comments)

BASELINE = REPO / "config" / "type_census.toml"
EXCLUDE = ("src/sdk/",)
# Where the shared declarations will live; their own lines never count.
SHARED_STDARG = "include/nu_stdarg.h"
SHARED_LIBC = "include/nu_libc.h"
# The only headers allowed in the root of include/: the platform layer.
PLATFORM_HEADERS = frozenset((
    "include/nu_types.h", SHARED_STDARG, SHARED_LIBC,
))

METRICS = (
    "struct_redefinitions", "typedef_redefinitions", "long_types",
    "varargs_hacks", "offset_casts", "sizeof_pads", "libc_prototypes",
    "extern_conflicts", "prototype_conflicts", "misplaced_headers",
)

LIBC = frozenset("""
    abs atan atan2 atan2f atanf atof atoi atol calloc ceil ceilf cos cosf exit
    fabs fabsf fclose floor floorf fmod fmodf fopen fprintf fread free fseek
    fwrite isalpha isdigit isspace isupper islower labs malloc memchr memcmp
    memcpy memmove memset pow powf printf puts qsort rand realloc sin sinf
    snprintf sprintf sqrt sqrtf srand strcasecmp strcat strchr strcmp strcpy
    strlen strncasecmp strncat strncmp strncpy strrchr strstr strtod strtok
    strtol strtoul tan tanf tolower toupper vprintf vsnprintf vsprintf
""".split())

# Spellings that name the same C type, for comparing declarations.
TYPE_SPELLING = {
    "u8": "unsigned char", "s8": "signed char", "u16": "unsigned short",
    "s16": "short", "u32": "unsigned int", "s32": "int",
    "u64": "unsigned long long", "s64": "long long", "f32": "float",
    "f64": "double", "signed int": "int", "signed": "int",
    "unsigned": "unsigned int", "short int": "short",
    "signed short": "short", "unsigned short int": "unsigned short",
    "long int": "long", "unsigned long int": "unsigned long",
    "long long int": "long long", "uint": "unsigned int",
    "u_int": "unsigned int", "u_char": "unsigned char",
    "u_short": "unsigned short", "u_long": "unsigned long",
    "long unsigned int": "unsigned long", "long unsigned": "unsigned long",
    "unsigned long long int": "unsigned long long",
}
# Canonical multi-word spellings map to themselves, so the longest match wins.
for _canon in set(TYPE_SPELLING.values()):
    TYPE_SPELLING.setdefault(_canon, _canon)

LONG = re.compile(r"\blong\b")
LONG_LONG = re.compile(r"\blong\s+long\b")
OFFSET_CHAR = re.compile(
    r"\(\s*(?:const\s+)?(?:unsigned\s+char|signed\s+char|char|u8|s8)\s*\*\s*\)"
    r"\s*&?\s*[\w.\->\[\]]+\s*\)?\s*\+\s*(?:0x[0-9A-Fa-f]+|\d+)\b")
OFFSET_INT = re.compile(
    r"\(\s*[\w\s]+\*\s*\)\s*\(\s*\(\s*(?:u32|s32|int|unsigned(?:\s+int)?|"
    r"nuintptr|uintptr_t)\s*\)\s*[^();]+?\+\s*(?:0x[0-9A-Fa-f]+|\d+)\s*\)")
TAG_DEF = re.compile(r"\b(struct|union)\s+(\w+)\s*\{")
EXTERN_VAR = re.compile(r"^[ \t]*extern\s+([^;(){}]+);", re.M)
IF0 = re.compile(r"^[ \t]*#\s*if\s+0\b.*?^[ \t]*#\s*endif\b", re.M | re.S)


def clean(text):
    """Comments, `#if 0` blocks and string literals blanked, lines kept."""
    text = strip_comments(text)
    text = IF0.sub(lambda m: re.sub(r"[^\n]", " ", m.group(0)), text)
    return re.sub(r'"(?:\\.|[^"\\\n])*"', '""', text)


def line_of(text, pos):
    return text.count("\n", 0, pos) + 1


def normalize_type(text, aliases=None):
    """One spelling per C type (typedef aliases, qualifiers, spacing)."""
    text = re.sub(r"\b(const|volatile|register|extern|static|inline)\b", " ", text)
    text = re.sub(r"\*", " * ", text)
    words = text.split()
    out, i = [], 0
    while i < len(words):
        for size in (3, 2, 1):
            chunk = " ".join(words[i:i + size])
            if size <= len(words) - i and chunk in TYPE_SPELLING:
                out.append(TYPE_SPELLING[chunk])
                i += size
                break
        else:
            word = words[i]
            if aliases and word in aliases and not (
                    out and out[-1] in ("struct", "union", "enum")):
                word = aliases[word]
            out.append(word)
            i += 1
    return " ".join(" ".join(out).split())


def split_top(text, sep=","):
    """Split on `sep` outside parentheses and brackets."""
    parts, depth, cur = [], 0, []
    for ch in text:
        if ch in "([{":
            depth += 1
        elif ch in ")]}":
            depth -= 1
        if ch == sep and depth == 0:
            parts.append("".join(cur))
            cur = []
        else:
            cur.append(ch)
    parts.append("".join(cur))
    return [p.strip() for p in parts if p.strip()]


def param_type(param, aliases=None):
    """A parameter without its name, normalized."""
    fnptr = re.match(r"(.*?)\(\s*\*\s*\w*\s*\)\s*\((.*)\)\s*$", param, re.S)
    if fnptr:                                       # function pointer
        inner = split_top(fnptr.group(2))
        inner = [] if inner == ["void"] else [param_type(p, aliases) for p in inner]
        return f"{normalize_type(fnptr.group(1), aliases)} (*)({', '.join(inner)})"
    if "(" in param:
        return normalize_type(param, aliases)
    array = "[" in param
    param = re.sub(r"\[[^\]]*\]", " ", param)
    param = re.sub(r"\*", " * ", param)
    words = param.split()
    if len(words) > 1 and re.fullmatch(r"[A-Za-z_]\w*", words[-1]) \
            and normalize_type(words[-1]) == words[-1] \
            and words[-1] not in ("int", "char", "short", "long", "float",
                                  "double", "void", "signed", "unsigned"):
        words = words[:-1]
    if array:                                       # arrays decay
        words.append("*")
    return normalize_type(" ".join(words), aliases)


def struct_bodies(text):
    """Yield (kind, tag, body start, body end) for each tagged definition."""
    for m in TAG_DEF.finditer(text):
        depth, i = 1, m.end()
        while depth and i < len(text):
            depth += {"{": 1, "}": -1}.get(text[i], 0)
            i += 1
        yield m.group(1), m.group(2), m.start(), m.end(), i


def typedef_names(text):
    """Yield (name, position) for each typedef, braces and pointers handled."""
    for m in re.finditer(r"\btypedef\b", text):
        depth, i = 0, m.end()
        while i < len(text):
            ch = text[i]
            depth += {"{": 1, "}": -1, "(": 1, ")": -1}.get(ch, 0)
            if ch == ";" and depth == 0:
                break
            i += 1
        decl = text[m.end():i]
        fnptr = re.search(r"\(\s*\*\s*(\w+)\s*\)", decl)
        if fnptr:
            yield fnptr.group(1), m.start()
            continue
        decl = re.sub(r"\{.*\}", " ", decl, flags=re.S)
        for part in split_top(decl):
            name = re.findall(r"[A-Za-z_]\w*", re.sub(r"\[[^\]]*\]", "", part))
            if name:
                yield name[-1], m.start()


def typedef_targets(text):
    """Yield (name, target type) for typedefs of one plain declarator."""
    for m in re.finditer(r"\btypedef\b", text):
        depth, i = 0, m.end()
        while i < len(text):
            ch = text[i]
            depth += {"{": 1, "}": -1, "(": 1, ")": -1}.get(ch, 0)
            if ch == ";" and depth == 0:
                break
            i += 1
        decl = text[m.end():i]
        if "(" in decl or "[" in decl or "," in re.sub(r"\{.*\}", "", decl, flags=re.S):
            continue
        body = re.match(r"\s*(struct|union|enum)\s+(\w+)\s*\{.*\}(.*)$", decl, re.S)
        if body:
            decl = f"{body.group(1)} {body.group(2)} {body.group(3)}"
        elif "{" in decl:
            continue
        name = re.search(r"([A-Za-z_]\w*)\s*$", decl)
        if name:
            yield name.group(1), decl[:name.start()]


def typedef_aliases(texts):
    """{typedef name: normalized target} where every definition agrees."""
    seen = defaultdict(set)
    for text in texts.values():
        for name, target in typedef_targets(text):
            seen[name].add(normalize_type(target))
    aliases = {name: next(iter(t)) for name, t in seen.items() if len(t) == 1}
    # Resolve chains (typedef NuVec Vec3; typedef struct nuvec_s NuVec;).
    for _ in range(4):
        aliases = {name: normalize_type(t, aliases) for name, t in aliases.items()}
    return {name: t for name, t in aliases.items() if t != name}


def header_misplaced(header, sources):
    """Why a header breaks the include/ layout, or None if it fits.

    header: repo-relative POSIX path; sources: the census paths (src/*.c).
    """
    parts = header.split("/")
    if len(parts) == 2:
        return None if header in PLATFORM_HEADERS else "root holds platform headers only"
    if len(parts) != 3:
        return "nested deeper than include/<lib>/"
    lib, name = parts[1], parts[2][:-len(".h")]
    if name == f"{lib}types" or f"src/{lib}/{name}.c" in sources:
        return None
    return f"no src/{lib}/{name}.c and not {lib}types.h"


def load_sources(repo=REPO):
    """{repo-relative path: text} for the census scope."""
    paths = [*sorted((repo / "src").rglob("*.c")),
             *sorted((repo / "src").rglob("*.h")),
             *sorted((repo / "include").rglob("*.h"))]
    out = {}
    for path in paths:
        rel = path.relative_to(repo).as_posix()
        if not rel.startswith(EXCLUDE):
            out[rel] = path.read_text(errors="replace")
    return out


def census(sources):
    """{metric: {"value": int, "findings": [(file, line, detail)]}}."""
    texts = {path: clean(text) for path, text in sources.items()}
    aliases = typedef_aliases(texts)
    found = {name: [] for name in METRICS}
    value = dict.fromkeys(METRICS, 0)

    tags, typedefs = defaultdict(list), defaultdict(list)
    externs, sigs, statics = defaultdict(list), defaultdict(list), set()
    for path, text in texts.items():
        seen_tags = set()
        for _kind, tag, start, body_lo, body_hi in struct_bodies(text):
            if tag not in seen_tags:
                seen_tags.add(tag)
                tags[tag].append((path, line_of(text, start)))
            body = text[body_lo:body_hi]
            for pad in re.finditer(r"\[[^\]\n]*\bsizeof\b[^\]\n]*\]", body):
                found["sizeof_pads"].append(
                    (path, line_of(text, body_lo + pad.start()), f"in {tag}"))
        seen_types = set()
        for name, pos in typedef_names(text):
            if name not in seen_types:
                seen_types.add(name)
                typedefs[name].append((path, line_of(text, pos)))

        without_ll = LONG_LONG.sub(lambda m: "_" * len(m.group(0)), text)
        for m in LONG.finditer(without_ll):
            found["long_types"].append((path, line_of(text, m.start()),
                                        text.splitlines()[line_of(text, m.start()) - 1].strip()))

        if path != SHARED_STDARG and "__builtin_args_info" in text:
            found["varargs_hacks"].append(
                (path, line_of(text, text.index("__builtin_args_info")), ""))

        for pattern in (OFFSET_CHAR, OFFSET_INT):
            for m in pattern.finditer(text):
                found["offset_casts"].append(
                    (path, line_of(text, m.start()), " ".join(m.group(0).split())))

        for m in EXTERN_VAR.finditer(text):
            decl = m.group(1)
            base = re.match(r"\s*((?:(?:struct|union|enum)\s+\w+|\w+)(?:\s+\w+)*?)"
                            r"(?=\s*[*\w]+\s*(?:\[|,|$))", decl)
            if not base:
                continue
            base_type = base.group(1)
            for part in split_top(decl[base.end():]):
                name = re.search(r"([A-Za-z_]\w*)", part)
                if not name:
                    continue
                stars = part.count("*")
                dims = "[]" * part.count("[")
                ctype = normalize_type(base_type + " " + "*" * stars, aliases) + dims
                externs[name.group(1)].append((path, line_of(text, m.start()), ctype))

        for m in SIGNATURE.finditer(text):
            ret, name, params, end = m.groups()
            if ret.split() and ret.split()[0] in KEYWORDS:
                continue
            if re.match(r"\s*(?:\w+\s+)*static\b", m.group(0)):
                statics.add(name)
            line = line_of(text, m.start())
            if end == ";" and name in LIBC and path != SHARED_LIBC:
                found["libc_prototypes"].append(
                    (path, line, " ".join((ret + name).split())))
            if params.strip() == "":
                continue                            # unprototyped: no signature
            plist = split_top(params)
            ptypes = ([] if plist == ["void"] else
                      [param_type(p, aliases) for p in plist])
            sig = f"{normalize_type(ret, aliases)} ({', '.join(ptypes)})"
            sigs[name].append((path, line, sig))

    for name, defs in sorted(tags.items()):
        if len(defs) > 1:
            value["struct_redefinitions"] += len(defs) - 1
            found["struct_redefinitions"].append(
                (defs[0][0], defs[0][1],
                 f"{name} x{len(defs)}: " + ", ".join(p for p, _ in defs)))
    for name, defs in sorted(typedefs.items()):
        if len(defs) > 1:
            value["typedef_redefinitions"] += len(defs) - 1
            found["typedef_redefinitions"].append(
                (defs[0][0], defs[0][1],
                 f"{name} x{len(defs)}: " + ", ".join(p for p, _ in defs)))
    for name, decls in sorted(externs.items()):
        kinds = sorted({ctype for _p, _l, ctype in decls})
        if len(kinds) > 1:
            value["extern_conflicts"] += 1
            found["extern_conflicts"].append(
                (decls[0][0], decls[0][1], f"{name}: " + " | ".join(kinds)))
    for name, decls in sorted(sigs.items()):
        if name in statics or name in SOFT_FLOAT:
            continue
        kinds = sorted({sig for _p, _l, sig in decls})
        if len(kinds) > 1:
            value["prototype_conflicts"] += 1
            found["prototype_conflicts"].append(
                (decls[0][0], decls[0][1], f"{name}: " + " | ".join(kinds)))
    for path in sorted(sources):
        if path.startswith("include/") and path.endswith(".h"):
            why = header_misplaced(path, sources)
            if why:
                found["misplaced_headers"].append((path, 1, why))
    for name in ("long_types", "varargs_hacks", "offset_casts", "sizeof_pads",
                 "libc_prototypes", "misplaced_headers"):
        value[name] = len(found[name])
    return {name: {"value": value[name],
                   "findings": sorted(found[name])} for name in METRICS}


def load_baseline(path=BASELINE):
    return tomllib.loads(path.read_text())["metrics"] if path.is_file() else None


def write_baseline(result, path=BASELINE):
    lines = [
        "# Type census ratchet (tools/check_types.py). Counts only; regenerate",
        "# with `python tools/check_types.py --update` in the change that moves them.",
        "",
        "schema = 1",
        "",
        "[metrics]",
    ]
    lines += [f"{name} = {result[name]['value']}" for name in METRICS]
    path.write_text("\n".join(lines) + "\n")


def check(result, baseline):
    """(failures, improvements): lists of human-readable lines."""
    worse, better = [], []
    for name in METRICS:
        now, was = result[name]["value"], baseline.get(name)
        if was is None or now > was:
            worse.append(f"{name}: {was} -> {now}")
        elif now < was:
            better.append(f"{name}: {was} -> {now}")
    return worse, better


def top_files(findings, limit=5):
    counts = defaultdict(int)
    for path, _line, _detail in findings:
        counts[path] += 1
    ranked = sorted(counts.items(), key=lambda kv: (-kv[1], kv[0]))[:limit]
    return ", ".join(f"{p} {n}" for p, n in ranked)


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--metric", choices=METRICS,
                        help="list every finding of one metric")
    parser.add_argument("--json", action="store_true", help="full result as JSON")
    parser.add_argument("--check", action="store_true",
                        help="exit 1 unless the counts equal the baseline")
    parser.add_argument("--update", action="store_true",
                        help="write the current counts as the new baseline")
    args = parser.parse_args()

    result = census(load_sources())

    if args.update:
        write_baseline(result)
        print(f"wrote {BASELINE.relative_to(REPO).as_posix()}")
        return 0
    if args.json:
        print(json.dumps(result, indent=1))
        return 0
    if args.metric:
        for path, line, detail in result[args.metric]["findings"]:
            print(f"{path}:{line}: {detail}")
        print(f"{args.metric} = {result[args.metric]['value']}")
        return 0

    baseline = load_baseline() or {}
    for name in METRICS:
        now = result[name]["value"]
        was = baseline.get(name)
        mark = "" if was is None or was == now else f"  (baseline {was})"
        print(f"{name:22} {now:5}{mark}")
        if not args.check and result[name]["findings"]:
            print(f"{'':28}{top_files(result[name]['findings'])}")
    if not args.check:
        return 0

    if not baseline:
        print(f"no baseline at {BASELINE.relative_to(REPO).as_posix()}; "
              "run --update", file=sys.stderr)
        return 1
    worse, better = check(result, baseline)
    for line in worse:
        print(f"FAIL {line} (went up; see --metric for the sites)")
    for line in better:
        print(f"FAIL {line} (went down; run python tools/check_types.py "
              "--update and commit config/type_census.toml)")
    return 1 if worse or better else 0


if __name__ == "__main__":
    sys.exit(main())
