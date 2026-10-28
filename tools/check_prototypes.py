#!/usr/bin/env python3
"""Report prototypes in src/ that disagree with the function's own definition.

Units declare the functions they call locally, so the same function can be
declared differently in different files. Some mismatches change the caller's
code without any compile error, and the result looks like a register-allocation
near-match:

  * return type `void` vs a value: with `void` gcc treats $v0 as free after the
    call, so the caller's register allocation shifts;
  * float vs integer return: the result is read from $f0 instead of $v0;
  * a parameter that is float in one and integer in the other: under the EE
    EABI floats travel in $f12.. and integers in $a0.., so the argument lands
    in the wrong register class. (Reordering *between* the two classes is
    harmless, so only the per-class sequence is compared.) Doubles are
    soft-float and travel in 64-bit GPRs, so `double` vs `float` differs but
    `double` vs a 64-bit integer does not, and 32- vs 64-bit integers differ.

A definition is trusted as ground truth only when its own function is
`matching` in the status manifests (the definition then provably reflects
retail). Declarations are only reported when an `asm`-state function in the
same file calls them, i.e. where fixing them can change a match result.

Soft-float helpers (fptodp, dpcmp, ...) are deliberately modelled with integer
types (compiler_knowledge.toml: default.soft-double-abi) and are skipped.

Every report is a lead to check against the caller's disassembly, not a
verdict: the original source sometimes used a different prototype itself
(e.g. eduiMenuProcess is defined with a `f32 dt` but edobjProc/edppProc/
edwmpProc call it with two arguments and never load $f12), and then the
"wrong" local declaration is the faithful one.

Stdlib only; reads src/ and config/<version>/status/. Report-only by default;
--check exits 1 when anything is reported.

Usage:
  python tools/check_prototypes.py
  python tools/check_prototypes.py --all     # also unverified definitions
  python tools/check_prototypes.py --check
"""
import argparse
import re
import sys
import tomllib
from collections import defaultdict
from pathlib import Path

REPO = Path(__file__).resolve().parent.parent

SOFT_FLOAT = frozenset((
    "fptodp", "dptofp", "dpcmp", "dpadd", "dpsub", "dpmul", "dpdiv", "dpneg",
    "dptoli", "litodp", "fpcmp", "__make_fp", "__make_dp",
))
KEYWORDS = frozenset(("if", "while", "for", "switch", "return", "sizeof",
                      "do", "else", "case", "goto", "asm", "__asm", "__asm__"))
FLOAT_TYPES = re.compile(r"\b(f32|float)\b")
# Doubles are soft-float here (compiler_knowledge.toml: default.soft-double-abi):
# they travel in a 64-bit GPR, like the 64-bit integers.
WIDE_TYPES = re.compile(r"\b(f64|double|s64|u64|long\s+long)\b")
# Disambiguation suffix of duplicated statics in manifest ids (NAME__<vram>).
DUP_SUFFIX = re.compile(r"__[0-9a-fA-F]{8}$")

# A declaration or definition head: [storage] type name(params) followed by ';'
# or '{'. One level of nested parentheses covers function-pointer parameters.
# NW_HOST_* attribute words (nu_port.h) are not part of the type.
SIGNATURE = re.compile(
    r"^[ \t]*(?:extern\s+|static\s+|inline\s+|NW_HOST_[A-Z_]+\s+)*"
    r"([A-Za-z_][\w\s\*]*?[\s\*])"          # return type
    r"([A-Za-z_]\w*)\s*"                   # name
    r"\(([^;{}()]*(?:\([^()]*\)[^;{}()]*)*)\)\s*([;{])",
    re.M)


def strip_comments(text):
    """Blank out comments, keeping newlines so line numbers stay correct."""
    text = re.sub(r"/\*.*?\*/",
                  lambda m: re.sub(r"[^\n]", " ", m.group(0)), text, flags=re.S)
    return re.sub(r"//[^\n]*", "", text)


def type_class(text):
    """'void', 'float' (an FPR), 'wide' (a 64-bit GPR: doubles and 64-bit
    integers) or 'int' (a 32-bit GPR: other integers and pointers)."""
    text = " ".join(text.split())
    if text == "void":
        return "void"
    if "*" in text:
        return "int"
    if WIDE_TYPES.search(text):
        return "wide"
    return "float" if FLOAT_TYPES.search(text) else "int"


def param_classes(params):
    """(GPR param classes, float params) as tuples, or None if variadic."""
    params = params.strip()
    if params in ("", "void"):
        return ((), ())
    gprs, floats = [], []
    for param in re.split(r",(?![^()]*\))", params):
        param = param.strip()
        if param == "...":
            return None
        if "(" in param or "*" in param or "[" in param:
            gprs.append("int")
            continue
        tokens = param.split()
        if len(tokens) > 1:
            tokens = tokens[:-1]            # drop the parameter name
        cls = type_class(" ".join(tokens))
        if cls == "float":
            floats.append("float")
        else:
            gprs.append(cls)
    return (tuple(gprs), tuple(floats))


def scan_file(path, text=None):
    """Yield (name, kind, line, ret_class, params, body_span) per signature."""
    text = strip_comments(text if text is not None else path.read_text(errors="replace"))
    for match in SIGNATURE.finditer(text):
        ret, name, params, end = match.groups()
        ret_words = ret.split()
        if name in KEYWORDS or (ret_words and ret_words[0] in KEYWORDS):
            continue
        line = text.count("\n", 0, match.start()) + 1
        body = None
        if end == "{":
            depth, i = 1, match.end()
            while depth and i < len(text):
                depth += {"{": 1, "}": -1}.get(text[i], 0)
                i += 1
            body = text[match.end():i]
        yield (name, "def" if end == "{" else "decl", line,
               type_class(ret), param_classes(params), body,
               " ".join((ret + name).split()))


def function_name(function_id):
    """The C name behind a manifest id (version:unit:vram:NAME[__<vram>])."""
    return DUP_SUFFIX.sub("", function_id.split(":")[-1])


def load_states(version):
    """{source path: {function name: state}} from the status manifests."""
    states = {}
    for manifest in (REPO / "config" / version / "status").rglob("*.toml"):
        data = tomllib.loads(manifest.read_text())
        states[data["source"]] = {
            function_name(e["id"]): e["state"]
            for e in data.get("function", [])}
    return states


def find_mismatches(sources, states, trust_all=False):
    """sources: {relative path: text}. Returns a sorted list of dicts."""
    defs = defaultdict(list)
    decls = defaultdict(list)
    bodies = defaultdict(dict)
    for path, text in sources.items():
        for name, kind, line, ret, params, body, head in scan_file(None, text):
            if kind == "def":
                defs[name].append((path, line, ret, params, head))
                bodies[path][name] = body
            else:
                decls[name].append((path, line, ret, params, head))

    found = []
    for name, declarations in decls.items():
        if name in SOFT_FLOAT or len(defs.get(name, ())) != 1:
            continue
        def_path, def_line, def_ret, def_params, def_head = defs[name][0]
        if not trust_all and states.get(def_path, {}).get(name) != "matching":
            continue
        for path, line, ret, params, head in declarations:
            if path == def_path:
                continue
            kinds = []
            if ret != def_ret:
                kinds.append("return")
            if params is not None and def_params is not None and params != def_params:
                kinds.append("params")
            if not kinds:
                continue
            call = re.compile(r"\b" + re.escape(name) + r"\s*\(")
            callers = sorted(
                fn for fn, body in bodies[path].items()
                if body and call.search(body)
                and states.get(path, {}).get(fn) == "asm")
            if not callers:
                continue
            found.append({"file": path, "line": line, "name": name,
                          "kinds": kinds, "declared": head,
                          "defined": def_head, "definition": f"{def_path}:{def_line}",
                          "asm_callers": callers})
    return sorted(found, key=lambda m: (m["file"], m["line"]))


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--version", default="pal103")
    parser.add_argument("--all", action="store_true",
                        help="also trust definitions that are not yet matching")
    parser.add_argument("--check", action="store_true",
                        help="exit 1 if any mismatch is reported")
    args = parser.parse_args()

    sources = {str(p.relative_to(REPO)).replace("\\", "/"): p.read_text(errors="replace")
               for p in sorted((REPO / "src").rglob("*.c"))}
    mismatches = find_mismatches(sources, load_states(args.version), args.all)
    for m in mismatches:
        print(f"{m['file']}:{m['line']}: {m['name']} ({', '.join(m['kinds'])})")
        print(f"    declared: {m['declared']}")
        print(f"    defined:  {m['defined']}  [{m['definition']}]")
        print(f"    asm callers here: {', '.join(m['asm_callers'])}")
    print(f"{len(mismatches)} prototype mismatch(es) affecting asm functions")
    return 1 if args.check and mismatches else 0


if __name__ == "__main__":
    sys.exit(main())
