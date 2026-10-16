#!/usr/bin/env python3
"""Probe how both EE compilers emit data, and compare with retail bytes.

Data-from-C (PLAN_DATA.md, `data/core`) needs facts, not guesses, about where
and in which order ee-gcc puts initialised data: which section an item lands
in at `-G8` / `-G0`, whether a static is emitted at its definition or deferred
to the end of the file, whether an unreferenced one is dropped, the `.align`
in front of it, how function statics are numbered (`name.N`), and where `$LC`
strings, jump tables and float pools go. This tool compiles a fixed probe
source under a fixed matrix and records exactly that; its JSON is the evidence
`config/<version>/profiles.toml` requires before a profile change.

Matrix: {ee-gcc-tt (profile default), ee-gcc (profile sce)} x {-G8, -G0}
(+ `-G0 -membedded-data` for ee-gcc), always `-O2 -fomit-frame-pointer -S`,
assembled with the decompals `as` at the same `-G`, objects parsed with
declib.elf.read_elf.

Retail comparisons (bytes from the user's ELF):
  nuvec4   the six 16-byte const vec4 globals of numath/nuvec4, defined in
           address order (default profile) vs [0x613FB0, 0x614010)
  e_pow    sdk/newlib/libm/math/e_pow.c with its `#define` scalars and the
           extern bp/dp_h/dp_l tables replaced by fdlibm's `static const
           double` definitions (values from retail), at sce -G8 and -G0:
           .text must equal the current object's, data vs [0x62B530, 0x62B700)
  lit4     one fully matching default unit compiled raw (no gen_hybrid
           .lit4 borrowing) vs its retail .lit4 run

Writes build/<version>/probe_data_emission.json and prints a summary. Needs
the toolchain: run it in the container,
  python tools/dispatch.py python tools/probe_data_emission.py
"""
import argparse
import json
import re
import shutil
import struct
import subprocess
import sys
import tempfile
import tomllib
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
import cc  # noqa: E402
from declib.elf import read_elf  # noqa: E402
from gen_hybrid import _sonyize  # noqa: E402
from declib.target import load_target  # noqa: E402
from declib.toolchain import AS, h, tool_path  # noqa: E402

BASE_FLAGS = ["-O2", "-fomit-frame-pointer"]
MATRIX = [
    ("default", "ee-gcc-tt", ["-G8"]),
    ("default", "ee-gcc-tt", ["-G0"]),
    ("sce", "ee-gcc", ["-G8"]),
    ("sce", "ee-gcc", ["-G0"]),
    ("sce", "ee-gcc", ["-G0", "-membedded-data"]),
]

PROBE_C = r"""
/* globals and statics before the functions */
const int g_const4 = 0x11111111;
int g_data4 = 0x22222222;
const double g_const8 = 1.5;
int g_data8[2] = { 0x33333333, 0x44444444 };
const int g_const16[4] = { 1, 2, 3, 4 };
int g_data16[4] = { 5, 6, 7, 8 };
int g_common;
int g_zero = 0;
static int s_bss;
static int s_big[8];
static const int s_const4 = 0x55555555;
static const int s_const4_unref = 0x66666666;
static int s_data4 = 0x77777777;
static int s_data4_unref = 0x78787878;
static const int s_const16[4] = { 9, 10, 11, 12 };
static int s_data16[4] = { 13, 14, 15, 16 };
const char *g_strtab[3] = { "a", "a longer string literal", "abcdefg" };

int use_statics(int i)
{
    return s_const4 + s_data4 + s_const16[i] + s_data16[i] + s_bss + s_big[i];
}

int lut_a(int i)
{
    static int lut[4] = { 1, 2, 3, 4 };
    return lut[i];
}

int lut_b(int i)
{
    static int lut[4] = { 5, 6, 7, 8 };
    return lut[i];
}

int sw(int i)
{
    switch (i) {
    case 0: return 10;
    case 1: return 31;
    case 2: return 52;
    case 3: return 73;
    case 4: return 94;
    case 5: return 15;
    default: return 0;
    }
}

const char *short_str(void) { return "short"; }
const char *long_str(void) { return "a string longer than eight"; }
float lis(float x) { return x * 0.3f; }
double lid(double x) { return x * 0.3; }

/* globals and statics after the functions */
const int g_const_after = 0x12345678;
int g_data_after = 0x23456789;
static int s_data_after = 0x34567890;

int use_after(void) { return s_data_after; }
"""

LID_GPR_RE = re.compile(r"^(\s*)li\.d\s+\$(\d+),\s*(\S+)\s*$")
SECTION_DIRECTIVES = {".text": ".text", ".data": ".data", ".rdata": ".rodata",
                      ".sdata": ".sdata", ".sbss": ".sbss", ".bss": ".bss"}
LABEL_RE = re.compile(r"^([$\w.]+):")


def compile_and_assemble(component, flags, source, name, work_root):
    """(assembly text, object bytes) for `source` under the given compiler."""
    argv, _driver = cc.compiler_command(component)
    work = work_root / name
    if work.exists():
        shutil.rmtree(work)
    work.mkdir(parents=True)
    cc._stage_headers(work)
    (work / "probe.c").write_text(source)
    subprocess.run([*argv, *BASE_FLAGS, *flags, *cc.INCLUDE_ARGS, "-S",
                    "-o", "out.s", "probe.c"], check=True, capture_output=True,
                   text=True, cwd=work)
    asm = (work / "out.s").read_text()
    # The decompals `as` rejects `li.d $gpr` on the r5900 (gen_hybrid borrows
    # a retail slot instead); an exact `dli` keeps the data layout intact.
    lines = []
    for line in asm.splitlines():
        m = LID_GPR_RE.match(line)
        if m:
            bits = struct.unpack("<Q", struct.pack("<d", float(m.group(3))))[0]
            line = f"{m.group(1)}dli\t${m.group(2)},{bits:#x}"
        lines.append(_sonyize(line))
    (work / "out.s").write_text("\n".join(lines) + "\n")
    g = next(f for f in flags if f.startswith("-G"))
    proc = subprocess.run([tool_path(AS), "-EL", "-march=r5900", g,
                           "-no-pad-sections", "-o", "out.o", "out.s"],
                          capture_output=True, text=True, cwd=work)
    if proc.returncode:
        raise SystemExit(f"{name}: as failed: {proc.stderr}")
    return asm, (work / "out.o").read_bytes()


def emission(asm):
    """[{label, section, line, align}] in .s order, plus .comm/.lcomm."""
    out, section, align = [], ".text", None
    for i, raw in enumerate(asm.splitlines()):
        line = raw.split("#")[0].strip()
        if not line:
            continue
        word = line.split()[0]
        if word in SECTION_DIRECTIVES:
            section = SECTION_DIRECTIVES[word]
            continue
        if word == ".section":
            section = line.split()[1].rstrip(",")
            continue
        if word == ".align":
            align = int(line.split()[1])
            continue
        if word in (".comm", ".lcomm"):
            name, size = [p.strip() for p in line.split(None, 1)[1].split(",")][:2]
            out.append({"label": name, "section": word, "line": i,
                        "size": int(size), "align": None})
            continue
        m = LABEL_RE.match(line)
        if m:
            out.append({"label": m.group(1), "section": section, "line": i,
                        "align": align})
            align = None
        elif not word.startswith("."):
            align = None
    return out


def object_facts(obj):
    """{sections: {name: size}, symbols: {name: {section, value, size, bind}}}."""
    sections, symbols, _rel = read_elf(obj)
    names = {s["index"]: s["name"] for s in sections}
    special = {0: "UND", 0xFFF1: "ABS", 0xFFF2: "COMMON"}
    return {
        "sections": {s["name"]: s["size"] for s in sections
                     if s["flags"] & 0x2 and s["size"]},
        "symbols": {s["name"]: {"section": special.get(s["shndx"], names.get(s["shndx"])),
                                "value": s["value"], "size": s["size"],
                                "bind": "global" if s["bind"] == 1 else "local"}
                    for s in symbols if s["name"] and s["type"] in (0, 1, 2)},
        "_sections": sections,
    }


def section_bytes(facts, name):
    return next((s["data"] for s in facts["_sections"] if s["name"] == name), b"")


def summarize(asm, facts):
    labels = emission(asm)
    funcs = [e["line"] for e in labels if e["section"] == ".text"
             and not e["label"].startswith("$")]
    last_func = max(funcs) if funcs else -1
    items = []
    for e in labels:
        if e["section"] == ".text" and not e["label"].startswith("$"):
            continue
        if e["label"].startswith("$L") and not e["label"].startswith("$LC") \
                and e["section"] == ".text":
            continue
        sym = facts["symbols"].get(e["label"])
        items.append({
            "label": e["label"], "s_section": e["section"], "align": e["align"],
            "after_last_function": e["line"] > last_func,
            "object_section": sym["section"] if sym else None,
            "value": sym["value"] if sym else None,
            "size": sym["size"] if sym else e.get("size"),
            "bind": sym["bind"] if sym else None})
    emitted = {i["label"] for i in items}
    dropped = sorted(n for n in re.findall(r"\b([gs]_\w+)\s*(?:\[[^\]]*\])?\s*=", PROBE_C)
                     if n not in emitted and not any(i["label"].startswith(n + ".")
                                                     for i in items))
    return {"items": items, "dropped": dropped,
            "sections": {k: v for k, v in facts["sections"].items()}}


# --------------------------------------------------------------------------- #
# Retail comparisons                                                          #
# --------------------------------------------------------------------------- #

def float_literal(bits):
    """A C float literal for a float32 bit pattern (shortest round trip)."""
    value = struct.unpack("<f", struct.pack("<I", bits))[0]
    for digits in range(1, 10):
        text = f"{value:.{digits}g}"
        if struct.unpack("<I", struct.pack("<f", float(text)))[0] == bits:
            break
    if "e" not in text and "." not in text:
        text += ".0"
    return text + "f"


def double_literal(value):
    text = repr(value)
    if "e" not in text and "." not in text and "inf" not in text:
        text += ".0"
    return text


class Retail:
    def __init__(self, version):
        self.elf, spec, _pt = load_target(version)
        self.secs = [(h(s["addr"]), h(s["addr"]) + h(s["size"]), h(s["offset"]))
                     for s in spec["sections"] if h(s["type"]) == 1 and h(s["addr"])]

    def bytes(self, lo, hi):
        for a, b, off in self.secs:
            if a <= lo and hi <= b:
                return self.elf[off + lo - a:off + hi - a]
        raise ValueError(f"[{lo:#x}, {hi:#x}) is not in one PROGBITS section")


def registry(version):
    return tomllib.loads((ROOT / "config" / version / "data_symbols.toml")
                         .read_text())["symbol"]


def probe_nuvec4(version, retail, work):
    syms = [s for s in registry(version) if s["unit"] == 14]
    lines = ["struct nuvec4_s { float x, y, z, w; };"]
    for s in syms:
        words = struct.unpack("<4I", retail.bytes(s["address"], s["address"] + 16))
        lines.append(f"const struct nuvec4_s {s['name']} = "
                     f"{{ {', '.join(float_literal(w) for w in words)} }};")
    lo, hi = syms[0]["address"], syms[-1]["address"] + syms[-1]["size"]
    asm, obj = compile_and_assemble("ee-gcc-tt", ["-G8"], "\n".join(lines) + "\n",
                                    "nuvec4", work)
    facts = object_facts(obj)
    got = section_bytes(facts, ".rodata")
    return {"range": [f"{lo:#010x}", f"{hi:#010x}"], "source": lines,
            "sections": facts["sections"],
            "order": [n for n, _ in sorted(((n, v["value"]) for n, v in facts["symbols"].items()
                                            if v["section"] == ".rodata"), key=lambda t: t[1])],
            "rodata_equals_retail": got == retail.bytes(lo, hi)}


E_POW_STATICS = ("bp", "dp_h", "dp_l", "zero", "one", "two", "two53", "huge",
                 "tiny", "L1", "L2", "L3", "L4", "L5", "L6", "P1", "P2", "P3",
                 "P4", "P5", "lg2", "lg2_h", "lg2_l", "ovt", "cp", "cp_h",
                 "cp_l", "ivln2", "ivln2_h", "ivln2_l")
E_POW_RANGE = (0x0062B530, 0x0062B700)
# The statics, then e_pow's own `$LC0..2` (inf, 1e300, 1e-300); the `li.d`
# pool after them is the assembler's (SN-as), not the compiler's.
E_POW_COMPILER_RANGE = (0x0062B530, 0x0062B650)
E_POW_TABLES = {"D_0062B530": "bp", "D_0062B540": "dp_h", "D_0062B550": "dp_l"}


def e_pow_source(version, retail):
    src = (ROOT / "src/sdk/newlib/libm/math/e_pow.c").read_text()
    by_name = {s["name"]: s for s in registry(version) if s["unit"] == 163}
    parts = []
    for name in E_POW_STATICS:
        s = by_name[name]
        blob = retail.bytes(s["address"], s["address"] + s["size"])
        values = [double_literal(v) for v in struct.unpack(f"<{len(blob) // 8}d", blob)]
        parts.append(f"{name}[] = {{ {', '.join(values)} }}" if name in ("bp", "dp_h", "dp_l")
                     else f"{name} = {values[0]}")
    decl = "static const double\n" + ",\n".join(parts) + ";\n"
    out = []
    for line in src.splitlines():
        if re.match(r"\s*extern const double D_0062B5[345]0\[\];", line):
            continue
        m = re.match(r"#define (\w+) ", line)
        if m and m.group(1) in E_POW_STATICS:
            continue
        if line.startswith("double __ieee754_pow"):
            out.append(decl)
        out.append(line)
    return "\n".join(out) + "\n"


def function_bodies(asm):
    """The lines between each `.ent` and `.end` (the code, without data)."""
    out, inside = [], False
    for line in asm.splitlines():
        word = line.split()[0] if line.split() else ""
        if word == ".ent":
            inside = True
        if inside:
            out.append(line)
        if word == ".end":
            inside = False
    return out


def probe_e_pow(version, retail, work):
    source = e_pow_source(version, retail)
    original = (ROOT / "src/sdk/newlib/libm/math/e_pow.c").read_text()
    out = {}
    want = retail.bytes(*E_POW_RANGE)
    for g in ("-G8", "-G0"):
        base_asm, _o = compile_and_assemble("ee-gcc", [g], original, f"e_pow_orig{g}", work)
        for old, new in E_POW_TABLES.items():
            base_asm = re.sub(rf"\b{old}\b", new, base_asm)
        asm, obj = compile_and_assemble("ee-gcc", [g], source, f"e_pow{g}", work)
        facts = object_facts(obj)
        data = {name: section_bytes(facts, name) for name in (".rodata", ".sdata", ".lit8", ".data")
                if section_bytes(facts, name)}
        statics = sorted(((n, v) for n, v in facts["symbols"].items()
                          if n in E_POW_STATICS), key=lambda t: (t[1]["section"], t[1]["value"]))
        rodata = data.get(".rodata", b"")
        out[g] = {
            "sections": facts["sections"],
            "code_equals_original": function_bodies(asm) == function_bodies(base_asm),
            "statics": [{"name": n, **{k: v[k] for k in ("section", "value", "size")}}
                        for n, v in statics],
            "statics_dropped": sorted(set(E_POW_STATICS) - {n for n, _ in statics}),
            "rodata_prefix_equals_retail_statics":
                rodata[:0x108] == want[:0x108] if len(rodata) >= 0x108 else False,
            "rodata_equals_retail_range": rodata == want,
            "rodata_equals_retail_statics_and_lc":
                rodata == retail.bytes(*E_POW_COMPILER_RANGE),
            "rodata_size": len(rodata), "retail_range_size": len(want),
        }
    return out


def probe_lit4(version, retail, work):
    """The smallest fully matching default unit with a .lit4 run, raw."""
    dm = tomllib.loads((ROOT / "config" / version / "data_map.toml").read_text())["range"]
    runs = {r["owner"]: (r["start"], r["end"]) for r in dm if r["section"] == ".lit4"}
    best = None
    for path in sorted((ROOT / "config" / version / "status").rglob("*.toml")):
        man = tomllib.loads(path.read_text())
        owner = "unit-" + man["unit"].split("-")[1]
        fns = man.get("function", [])
        if man.get("profile") != "default" or owner not in runs or not fns \
                or any(f["state"] != "matching" for f in fns):
            continue
        size = runs[owner][1] - runs[owner][0]
        if size >= 8 and (best is None or size < best[0]):
            best = (size, owner, man["source"])
    if best is None:
        return {"unit": None}
    _size, owner, source = best
    lo, hi = runs[owner]
    asm, obj = compile_and_assemble("ee-gcc-tt", ["-G8"], (ROOT / source).read_text(),
                                    "lit4", work)
    got = section_bytes(object_facts(obj), ".lit4")
    want = retail.bytes(lo, hi)
    return {"unit": owner, "source": source, "range": [f"{lo:#010x}", f"{hi:#010x}"],
            "li_s_count": len(re.findall(r"\bli\.s\b", asm)),
            "object_lit4_size": len(got), "retail_lit4_size": len(want),
            "lit4_equals_retail": got == want,
            "object_words": [f"{w:#010x}" for w in struct.unpack(f"<{len(got) // 4}I", got)],
            "retail_words": [f"{w:#010x}" for w in struct.unpack(f"<{len(want) // 4}I", want)]}


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--version", default="pal103")
    parser.add_argument("--out", type=Path)
    args = parser.parse_args()
    if not tool_path(AS) or not cc.compiler_available("default", args.version):
        print("toolchain not found: run through tools/dispatch.py", file=sys.stderr)
        return 2

    work = Path(tempfile.gettempdir()) / "crashwoc-probe-data"
    work.mkdir(parents=True, exist_ok=True)
    result = {"schema": 1, "base_flags": BASE_FLAGS, "probe_source": PROBE_C,
              "matrix": []}
    for profile, component, flags in MATRIX:
        asm, obj = compile_and_assemble(component, flags, PROBE_C,
                                        f"{component}{''.join(flags)}", work)
        entry = {"profile": profile, "compiler": component, "flags": flags,
                 **summarize(asm, object_facts(obj)), "assembly": asm}
        result["matrix"].append(entry)

    retail = Retail(args.version)
    result["nuvec4"] = probe_nuvec4(args.version, retail, work)
    result["e_pow"] = probe_e_pow(args.version, retail, work)
    result["lit4"] = probe_lit4(args.version, retail, work)
    shutil.rmtree(work, ignore_errors=True)

    out = args.out or ROOT / "build" / args.version / "probe_data_emission.json"
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps(result, indent=1) + "\n")

    for entry in result["matrix"]:
        print(f"== {entry['compiler']} {' '.join(entry['flags'])}")
        for item in entry["items"]:
            print(f"   {item['label']:<22} .s {item['s_section']:<8} obj "
                  f"{str(item['object_section']):<8} align {item['align']} "
                  f"{'end-of-file' if item['after_last_function'] else ''}")
        print(f"   dropped: {', '.join(entry['dropped']) or '-'}")
    nv = result["nuvec4"]
    print(f"nuvec4: .rodata == retail {nv['range']}: {nv['rodata_equals_retail']}")
    for g in ("-G8", "-G0"):
        e = result["e_pow"][g]
        print(f"e_pow {g}: code==original {e['code_equals_original']}, sections "
              f"{e['sections']}, statics dropped {len(e['statics_dropped'])}, "
              f".rodata == retail statics+$LC {e['rodata_equals_retail_statics_and_lc']} "
              f"(statics prefix {e['rodata_prefix_equals_retail_statics']})")
    lt = result["lit4"]
    print(f"lit4 {lt.get('unit')} ({lt.get('source')}): equals retail "
          f"{lt.get('lit4_equals_retail')} ({lt.get('object_lit4_size')} vs "
          f"{lt.get('retail_lit4_size')} bytes)")
    print(f"wrote {out.relative_to(ROOT)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
