#!/usr/bin/env python3
"""Print a C initialiser for one retail data symbol. Writes nothing.

    python tools/dump_data.py nuvec4_zero
    python tools/dump_data.py pal103:unit-0163:0062b530:bp
    python tools/dump_data.py 0x002e98b0 --type 'u32[256]'
    python tools/dump_data.py LData --type 'struct ldata_s[44]' --probe
    python tools/dump_data.py one --verify            # container: check literals

The symbol comes from the data registry (config/<version>/data_symbols.toml),
its bytes from the user's ELF. The type is, in order: `--type`; the symbol's
STABS type (the `sce` units, read natively from the ELF). A struct, union or
typedef'd type in `--type` needs `--probe`: the type is declared once in a
copy of the owning unit's source (or `--unit SRC`) compiled with `ee-gcc -g`,
and its STABS give the exact layout -- the compiler's, not a guess.
Scalars, pointers and arrays of them need no probe.

Rendering: integers in decimal (hex above 0xFFFF), floats and doubles as the
shortest literal that round-trips their bit pattern (marked unverified until
`--verify` compiles them and compares bits, adding digits when needed;
NaN/inf/denormals are flagged), char arrays as string literals (an embedded
NUL keeps the brace form), an all-zero object as {0}. A pointer prints as
`&sym`, `&sym[i]`, `(char *)&sym + 0xN`, a function name (duplicated statics
as NAME__<vram>, flagged), a string literal for an unnamed .rodata string,
or `D_<vram> /* unresolved */`.

Needs the target ELF; --probe and --verify need the toolchain (they run
themselves through tools/dispatch.py when called on the host).
"""
import argparse
import math
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
from declib.stabs import (Array, Basic, Enum, Pointer, Struct,  # noqa: E402
                          TypeTable, join_continuations)
from declib.target import load_target  # noqa: E402
from declib.toolchain import h  # noqa: E402

SCALARS = {
    "char": (1, True, False), "signed char": (1, True, False),
    "unsigned char": (1, False, False), "s8": (1, True, False), "u8": (1, False, False),
    "short": (2, True, False), "unsigned short": (2, False, False),
    "s16": (2, True, False), "u16": (2, False, False),
    "int": (4, True, False), "signed": (4, True, False), "unsigned": (4, False, False),
    "unsigned int": (4, False, False), "s32": (4, True, False), "u32": (4, False, False),
    "long": (8, True, False), "unsigned long": (8, False, False),
    "long long": (8, True, False), "unsigned long long": (8, False, False),
    "s64": (8, True, False), "u64": (8, False, False),
    "float": (4, True, True), "f32": (4, True, True),
    "double": (8, True, True), "f64": (8, True, True),
}
PROBE_NAME = "__dump_data_probe"
STABS_RE = re.compile(r'^\s*\.stabs\s+"((?:[^"\\]|\\.)*)",\s*(\d+)')


class DumpError(Exception):
    pass


# --------------------------------------------------------------------------- #
# Inputs                                                                      #
# --------------------------------------------------------------------------- #

class Image:
    """The retail ELF, the data registry and the function table."""

    def __init__(self, version):
        self.version = version
        self.elf, spec, _pt = load_target(version)
        self.sections = [(s["name"], h(s["addr"]), h(s["addr"]) + h(s["size"]),
                          h(s["offset"]), h(s["type"]))
                         for s in spec["sections"] if h(s["addr"]) and h(s["size"])]
        cfg = ROOT / "config" / version
        self.symbols = tomllib.loads((cfg / "data_symbols.toml").read_text())["symbol"]
        funcs = tomllib.loads((cfg / "functions.toml").read_text())["function"]
        names = [f["name"] for f in funcs]
        dup = {n for n in names if names.count(n) > 1}
        self.functions = {f["address"]: (f"{f['name']}__{f['address']:08X}"
                                         if f["name"] in dup else f["name"],
                                         f["name"] in dup) for f in funcs}
        self.by_address = {}
        for sym in self.symbols:
            self.by_address.setdefault(sym["address"], sym)
        self.sized = sorted((s["address"], s["address"] + s["size"], s)
                            for s in self.symbols if s.get("size"))

    def section_of(self, addr):
        for name, lo, hi, _off, stype in self.sections:
            if lo <= addr < hi:
                return name, lo, hi, _off, stype
        return None

    def bytes(self, addr, size):
        sec = self.section_of(addr)
        if sec is None:
            raise DumpError(f"{addr:#010x} is in no section")
        name, lo, hi, off, stype = sec
        if addr + size > hi:
            raise DumpError(f"[{addr:#x}, {addr + size:#x}) leaves {name}")
        if stype == 8:
            return bytes(size)                      # NOBITS: zero at load
        return self.elf[off + addr - lo:off + addr - lo + size]

    def resolve(self, target):
        """The registry record for an id, a name, a key or an address."""
        target = target.strip()
        m = re.fullmatch(r"[a-z0-9]+:unit-(\d{4}):([0-9a-f]{8}):(\S+)", target)
        if m:
            hits = [s for s in self.symbols if s["unit"] == int(m.group(1))
                    and s["address"] == int(m.group(2), 16) and s["name"] == m.group(3)]
        elif re.fullmatch(r"(0x)?[0-9A-Fa-f]{6,8}", target):
            addr = int(target, 16)
            hits = [s for s in self.symbols if s["address"] == addr]
            if not hits:
                return {"key": f"D_{addr:08X}", "name": f"D_{addr:08X}",
                        "address": addr, "section": (self.section_of(addr) or ["?"])[0]}
        else:
            hits = [s for s in self.symbols if target in (s["key"], s["name"])]
        if not hits:
            raise DumpError(f"no data symbol {target!r} in the registry")
        if len(hits) > 1:
            raise DumpError(f"{target!r} is ambiguous: "
                            + ", ".join(s["key"] for s in hits))
        return hits[0]

    def unit_source(self, unit):
        for path in sorted((ROOT / "config" / self.version / "status").rglob("*.toml")):
            data = tomllib.loads(path.read_text())
            if data["unit"].endswith(f"unit-{unit:04d}"):
                return ROOT / data["source"], data.get("profile", "default")
        return None, None


# --------------------------------------------------------------------------- #
# Types                                                                       #
# --------------------------------------------------------------------------- #

def parse_simple_type(text):
    """(base, pointer depth, [dims]) for `base [*...] [N]...`, else None."""
    m = re.fullmatch(r"\s*((?:const\s+|volatile\s+)*)([A-Za-z_][\w ]*?)\s*(\**)\s*"
                     r"((?:\[\s*\d*\s*\])*)\s*", text)
    if not m:
        return None
    base = " ".join(m.group(2).split())
    dims = [int(d) if d.strip() else None
            for d in re.findall(r"\[\s*(\d*)\s*\]", m.group(4))]
    return base, len(m.group(3)), dims


def simple_table(text):
    """(TypeTable, Ref) for a scalar/pointer/array type, else None."""
    parsed = parse_simple_type(text)
    if not parsed:
        return None
    base, stars, dims = parsed
    if base not in SCALARS and not (stars and base in ("void", "struct", "char")):
        if not stars:
            return None
    table = TypeTable()
    num = [100]

    def new(typ, name=None):
        num[0] += 1
        table.types[num[0]] = typ
        if name:
            table.typedefs[num[0]] = name
        from declib.stabs import Ref
        return Ref(table, num[0])

    if base in SCALARS:
        size, signed, is_float = SCALARS[base]
        ref = new(Basic(size, is_float=is_float, signed=signed), base)
    else:
        ref = new(Basic(0, is_void=True), "void")       # opaque pointee
    for _ in range(stars):
        ref = new(Pointer(ref))
    for dim in reversed(dims):
        ref = new(Array(0, (dim - 1) if dim else -1, ref))
    return table, ref


def declarator(text, name):
    """`T name` for a type string `base [*...] [N]...`."""
    parsed = parse_simple_type(text)
    if not parsed:
        raise DumpError(f"--probe needs a type of the form `base [*] [N]`: {text!r}")
    base, stars, dims = parsed
    return f"{base} {'*' * stars}{name}" + "".join(f"[{d if d else ''}]" for d in dims)


def probe_table(type_text, source, version, profile="default"):
    """(TypeTable, Ref) from the STABS of `type_text` declared in `source`.

    Compiles a copy of the unit source plus one declaration with the unit
    profile's compiler at -g -O0 (both EE compilers emit STABS)."""
    import cc
    argv, _driver = cc.compiler_command(cc.profile_spec(profile, version)["compiler"])
    work = Path(tempfile.gettempdir()) / "crashwoc-dump-data"
    if work.exists():
        shutil.rmtree(work)
    work.mkdir(parents=True)
    try:
        cc._stage_headers(work)
        text = source.read_text() if source else ""
        (work / "probe.c").write_text(text + f"\n{declarator(type_text, PROBE_NAME)};\n")
        proc = subprocess.run([*argv, "-O0", "-g", *cc.INCLUDE_ARGS, "-S",
                               "-o", "probe.s", "probe.c"], capture_output=True,
                              text=True, cwd=work)
        if proc.returncode:
            raise DumpError("probe compile failed: " + proc.stderr.strip()[:400])
        asm = (work / "probe.s").read_text()
    finally:
        shutil.rmtree(work, ignore_errors=True)
    table, ref = TypeTable(), None
    parts = [m.group(1).encode().decode("unicode_escape")
             for line in asm.splitlines() if (m := STABS_RE.match(line))]
    for string in join_continuations(parts):
        name, _desc, r = table.add(string)
        if name == PROBE_NAME:
            ref = r
    if ref is None:
        raise DumpError("the probe emitted no STABS for the declaration")
    return table, ref


def stabs_table(image, record):
    """(TypeTable, Ref) from the owning unit's own STABS, else None."""
    if not record.get("stab"):
        return None
    from extract_data_symbols import unit_stabs
    from extract_mdebug import MDebug, find_section
    md = MDebug(image.elf, find_section(image.elf, ".mdebug")[0])
    unit = next(u for u in md.units() if u["index"] == record["unit"])
    table = unit_stabs(md, unit)[0]
    _name, _desc, ref = table.add(record["stab"])
    return table, ref


# --------------------------------------------------------------------------- #
# Rendering                                                                   #
# --------------------------------------------------------------------------- #

def float_text(bits, size, extra=0):
    """(literal, flag) -- the shortest round-trip text, +extra digits."""
    fmt, ifmt = ("<f", "<I") if size == 4 else ("<d", "<Q")
    value = struct.unpack(fmt, struct.pack(ifmt, bits))[0]
    if math.isnan(value):
        return "0.0 /* NaN */", "nan"
    if math.isinf(value):
        return ("-" if value < 0 else "") + "1e999 /* inf */", "inf"
    flag = None
    exp_mask = 0x7F800000 if size == 4 else 0x7FF0000000000000
    if bits & exp_mask == 0 and value != 0:
        flag = "denormal"
    for digits in range(1, 18):
        text = f"{value:.{digits + extra}g}"
        if struct.unpack(ifmt, struct.pack(fmt, float(text)))[0] == bits:
            break
    if "e" in text and value == int(value) and abs(value) < 1e17:
        text = str(int(value))                  # 1e+01 -> 10.0
    if "e" not in text and "." not in text:
        text += ".0"
    return text + ("f" if size == 4 else ""), flag


def c_escape(blob):
    out = []
    for b in blob:
        ch = chr(b)
        if ch in '"\\':
            out.append("\\" + ch)
        elif ch == "\n":
            out.append("\\n")
        elif ch == "\t":
            out.append("\\t")
        elif ch == "\r":
            out.append("\\r")
        elif 32 <= b < 127:
            out.append(ch)
        else:
            out.append(f"\\{b:03o}")
    return '"' + "".join(out) + '"'


class Renderer:
    def __init__(self, image, table, extra_digits=0):
        self.image, self.table = image, table
        self.extra = extra_digits
        self.floats = []          # (literal, size, bits) for --verify
        self.notes = []

    def pointer(self, value):
        if value == 0:
            return "0"
        img = self.image
        if value in img.functions:
            name, dup = img.functions[value]
            if dup:
                self.notes.append(f"{name}: a duplicated static, declared per unit")
            return name
        sym = img.by_address.get(value)
        if sym is not None:
            return f"&{sym['name']}"
        import bisect
        starts = [s for s, _e, _r in img.sized]
        i = bisect.bisect_right(starts, value) - 1
        if i >= 0 and img.sized[i][0] < value < img.sized[i][1]:
            base = img.sized[i][2]
            return f"(char *)&{base['name']} + {value - base['address']:#x}"
        sec = img.section_of(value)
        if sec and sec[0] == ".rodata":
            blob = img.bytes(value, min(256, sec[2] - value))
            end = blob.find(b"\0")
            if end >= 0:
                return c_escape(blob[:end])
        if sec and sec[0] == ".sdata":
            # ee-gcc puts strings of at most 8 bytes in .sdata.
            blob = img.bytes(value, min(8, sec[2] - value))
            end = blob.find(b"\0")
            if end > 0 and all(32 <= b < 127 for b in blob[:end]):
                return c_escape(blob[:end])
        return f"D_{value:08X} /* unresolved */"

    def render(self, ref, blob, depth=0):
        table = self.table
        typ = table.resolve(ref)
        pad = "    " * (depth + 1)
        if not any(blob):
            if isinstance(typ, (Struct, Array)):
                return "{0}"
        if isinstance(typ, Basic):
            if typ.is_void or not typ.size:
                raise DumpError("cannot render a void/sizeless object")
            if typ.is_float:
                bits = int.from_bytes(blob[:typ.size], "little")
                text, flag = float_text(bits, typ.size, self.extra)
                self.floats.append((text, typ.size, bits))
                if flag:
                    self.notes.append(f"{text}: {flag}")
                return text
            value = int.from_bytes(blob[:typ.size], "little", signed=typ.signed)
            return str(value) if -0xFFFF <= value <= 0xFFFF else (
                f"{value:#x}" if value >= 0 else f"-{-value:#x}")
        if isinstance(typ, Enum):
            value = int.from_bytes(blob[:4], "little", signed=True)
            names = [n for n, v in typ.values if v == value]
            return names[0] if names else str(value)
        if isinstance(typ, Pointer):
            return self.pointer(int.from_bytes(blob[:4], "little"))
        if isinstance(typ, Array):
            elem = table.size(typ.element)
            if not elem:
                raise DumpError("array of a sizeless element")
            count = typ.count or len(blob) // elem
            etyp = table.resolve(typ.element)
            if isinstance(etyp, Basic) and etyp.size == 1 and not etyp.is_float:
                text = blob[:count]
                end = text.find(b"\0")
                if end >= 0 and not any(text[end:]) and \
                        all(32 <= b < 127 or b in (9, 10, 13) for b in text[:end]):
                    return c_escape(text[:end])
            items = [self.render(typ.element, blob[i * elem:(i + 1) * elem], depth + 1)
                     for i in range(count)]
            return self.braces(items, pad, depth)
        if isinstance(typ, Struct):
            items = []
            fields = typ.fields if typ.kind == "struct" else typ.fields[:1]
            for name, fref, bitpos, bitsize in fields:
                fsize = table.size(fref)
                if bitpos % 8 or fsize is None or bitsize != fsize * 8:
                    raise DumpError(f"bit-field or sizeless member {name!r}: "
                                    "write this one by hand")
                off = bitpos // 8
                items.append(f"{self.render(fref, blob[off:off + fsize], depth + 1)}"
                             f" /* {name} */")
            return self.braces(items, pad, depth)
        raise DumpError(f"cannot render a {type(typ).__name__}")

    def braces(self, items, pad, depth):
        flat = "{ " + ", ".join(items) + " }"
        if len(flat) <= 76 - 4 * depth and "/*" not in flat:
            return flat
        close = "    " * depth
        if all("\n" not in i and "/*" not in i for i in items):
            rows, row = [], ""
            for item in items:                  # several short values a line
                cell = item + ","
                if row and len(pad) + len(row) + 1 + len(cell) > 79:
                    rows.append(row)
                    row = cell
                else:
                    row = f"{row} {cell}" if row else cell
            rows.append(row[:-1])
            return "{\n" + "\n".join(pad + r for r in rows) + "\n" + close + "}"
        return "{\n" + ",\n".join(pad + i for i in items) + "\n" + close + "}"


def verify_floats(renderer, profile, version):
    """True when every float/double literal compiles to its bit pattern."""
    import cc
    for size, ctype in ((4, "float"), (8, "double")):
        lits = [(t, b) for t, s, b in renderer.floats if s == size]
        if not lits:
            continue
        src = (f"const {ctype} {PROBE_NAME}[] = {{ "
               + ", ".join(t.split(" ")[0] for t, _b in lits) + " };\n")
        work = Path(tempfile.gettempdir()) / "crashwoc-dump-verify"
        work.mkdir(parents=True, exist_ok=True)
        (work / "v.c").write_text(src)
        cc.compile_s(work / "v.c", work / "v.s", profile=profile, version=version)
        words = [int(w, 0) & 0xFFFFFFFF for w in re.findall(
            r"^\s*\.word\s+(-?(?:0x[0-9A-Fa-f]+|\d+))", (work / "v.s").read_text(), re.M)]
        shutil.rmtree(work, ignore_errors=True)
        if size == 4:
            got = words
        else:
            got = [words[i] | (words[i + 1] << 32) for i in range(0, len(words) - 1, 2)]
        if got != [b for _t, b in lits]:
            return False
    return True


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("target", help="data id, registry key, name or address")
    parser.add_argument("--type", dest="ctype", help="C type of the object")
    parser.add_argument("--unit", help="source to declare a --probe type in "
                        "(default: the owning unit's source)")
    parser.add_argument("--probe", action="store_true",
                        help="take the layout from an ee-gcc -g STABS probe")
    parser.add_argument("--verify", action="store_true",
                        help="compile the float/double literals and compare bits")
    parser.add_argument("--size", type=lambda v: int(v, 0),
                        help="bytes to dump for an unsized symbol")
    parser.add_argument("--version", default="pal103")
    args = parser.parse_args()

    if (args.probe or args.verify) and not Path("/work").is_dir():
        import os
        if not os.environ.get("CRASHWOC_IN_CONTAINER"):
            return subprocess.call([sys.executable, str(ROOT / "tools" / "dispatch.py"),
                                    "python", "tools/dump_data.py", *sys.argv[1:]])
    try:
        image = Image(args.version)
        record = image.resolve(args.target)
        source, profile = image.unit_source(record["unit"]) if "unit" in record \
            else (None, "default")
        if args.unit:
            source = ROOT / args.unit
        if args.ctype and args.probe:
            table, ref = probe_table(args.ctype, source, args.version, profile)
        elif args.ctype:
            got = simple_table(args.ctype)
            if got is None:
                raise DumpError(f"{args.ctype!r} needs --probe (struct/typedef)")
            table, ref = got
        else:
            got = stabs_table(image, record)
            if got is None:
                raise DumpError(f"{record['key']} has no STABS type: pass --type")
            table, ref = got
        size = record.get("size") or args.size or table.size(ref)
        if not size:
            raise DumpError(f"{record['key']} is unsized: pass --size or a "
                            "complete --type")
        tsize = table.size(ref)
        if tsize is not None and tsize != size:
            raise DumpError(f"type size {tsize:#x} != symbol size {size:#x}")
        if tsize is None:                       # open array: complete it
            typ = table.resolve(ref)
            if isinstance(typ, Array):
                elem = table.size(typ.element)
                if not elem or size % elem:
                    raise DumpError("symbol size is not a whole number of elements")
                typ.hi = size // elem - 1
        blob = image.bytes(record["address"], size)
        for extra in range(0, 4):
            renderer = Renderer(image, table, extra)
            text = renderer.render(ref, blob)
            if not args.verify or verify_floats(renderer, profile, args.version):
                break
        else:
            raise DumpError("float literals still do not round-trip at +3 digits")
    except DumpError as exc:
        print(f"dump_data: {exc}", file=sys.stderr)
        return 1

    decl = table.c_type(ref, record["name"].split(".")[0])
    storage = "static " if record.get("scope", "global") != "global" else ""
    const = "const " if record.get("section") == ".rodata" else ""
    print(f"/* {record['key']} @ {record['address']:#010x} "
          f"({record.get('section')}, {size:#x} bytes) */")
    if renderer.floats and not args.verify:
        print("/* float literals unverified: rerun with --verify */")
    for note in dict.fromkeys(renderer.notes):
        print(f"/* {note} */")
    print(f"{storage}{const}{decl} = {text};")
    return 0


if __name__ == "__main__":
    sys.exit(main())
