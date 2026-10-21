"""GCC 2.9 STABS type parser -- only what data symbols need.

The `sce` half of the retail ELF (newlib, libm, libgcc) carries STABS inside
`.mdebug` (see extract_mdebug.MDebug.stabs). A unit's stab strings define
numbered types (`int:t1=r1;-2147483648;2147483647;`, `foo:T22=s8a:1,0,32;;`)
that later strings reference by number (`one:S13`). This module parses those
definitions into a per-unit table and answers the two questions the data
registry asks of a symbol's stab: its C type, and its size when complete.

Supported descriptors: ranges (`r`, incl. the float form `r1;8;0;` and octal
bounds), pointers (`*`), arrays (`a`), structs and unions (`s`/`u`), enums
(`e`), functions (`f`), forward references (`x`), const/volatile (`k`/`B`)
and plain aliases (`NN=MM`). Anything else raises StabsError, so an
unsupported string is visible instead of guessed.
"""
import re

SIZES = (1, 2, 4, 8, 16)


class StabsError(ValueError):
    pass


class Type:
    kind = "?"


class Basic(Type):
    kind = "basic"

    def __init__(self, size, is_float=False, is_void=False, signed=False):
        self.size, self.is_float, self.is_void = size, is_float, is_void
        self.signed = signed


class Pointer(Type):
    kind = "pointer"

    def __init__(self, target):
        self.target = target


class Array(Type):
    kind = "array"

    def __init__(self, lo, hi, element):
        self.lo, self.hi, self.element = lo, hi, element

    @property
    def count(self):
        return self.hi - self.lo + 1 if self.hi >= self.lo else None


class Struct(Type):
    def __init__(self, kind, size, fields):
        self.kind, self.size, self.fields = kind, size, fields  # struct/union


class Enum(Type):
    kind = "enum"

    def __init__(self, values):
        self.values = values


class Function(Type):
    kind = "function"

    def __init__(self, result):
        self.result = result


class XRef(Type):
    kind = "xref"

    def __init__(self, tag_kind, tag):
        self.tag_kind, self.tag = tag_kind, tag


class Qualified(Type):
    kind = "qualified"

    def __init__(self, qualifier, target):
        self.qualifier, self.target = qualifier, target


class Alias(Type):
    kind = "alias"

    def __init__(self, target):
        self.target = target


class Ref:
    """A reference to type number `num` in `table`, resolved lazily so a
    struct can point at itself before its definition is complete. An
    anonymous definition (an array's `r1;0;-1;` index) has num None."""

    def __init__(self, table, num, anonymous=None):
        self.table, self.num, self.anonymous = table, num, anonymous

    def get(self):
        if self.num is None:
            return self.anonymous
        try:
            return self.table.types[self.num]
        except KeyError:
            raise StabsError(f"type {self.num} is referenced but never defined")


class TypeTable:
    """The numbered types of one unit, built from its stab strings in order."""

    def __init__(self):
        self.types = {}       # num -> Type
        self.typedefs = {}    # num -> typedef name (`name:t`)
        self.tags = {}        # num -> tag name (`name:T`)

    # -- parsing -----------------------------------------------------------

    def add(self, string):
        """Parse one stab string; return (name, descriptor, Ref or None)."""
        name, sep, rest = string.partition(":")
        if not sep or not rest:
            return name, "", None
        desc = ""
        if rest[0].isalpha() and not rest[0].isdigit():
            desc, rest = rest[0], rest[1:]
            if desc == "T" and rest[:1] == "t":   # `name:Tt` tag + typedef
                desc, rest = "Tt", rest[1:]
        if not rest or not (rest[0].isdigit() or rest[0] == "-"):
            return name, desc, None
        ref, end = self._type(rest, 0)
        if end != len(rest):
            raise StabsError(f"trailing text in {string!r} at {end}")
        if desc in ("t", "Tt") and name.strip():
            self.typedefs.setdefault(ref.num, name)
        if desc in ("T", "Tt") and name.strip():
            self.tags.setdefault(ref.num, name)
        return name, desc, ref

    def _number(self, s, i):
        m = re.compile(r"-?\d+").match(s, i)
        if not m:
            raise StabsError(f"number expected in {s!r} at {i}")
        text = m.group(0)
        neg = text.startswith("-")
        digits = text[1:] if neg else text
        # Bounds of 64/128-bit ranges are printed in octal with a leading 0.
        value = int(digits, 8) if len(digits) > 1 and digits[0] == "0" else int(digits)
        return (-value if neg else value), m.end()

    def _expect(self, s, i, ch):
        if i >= len(s) or s[i] != ch:
            raise StabsError(f"{ch!r} expected in {s!r} at {i}")
        return i + 1

    def _type(self, s, i):
        """Parse `NN`, `NN=def` or an anonymous def at s[i]; (Ref, end)."""
        if i < len(s) and not (s[i].isdigit() or s[i] == "-"):
            typ, i = self._definition(s, i, None)
            return Ref(self, None, typ), i
        num, i = self._number(s, i)
        ref = Ref(self, num)
        if i < len(s) and s[i] == "=":
            typ, i = self._definition(s, i + 1, num)
            self.types[num] = typ
        return ref, i

    def _definition(self, s, i, num):
        if i >= len(s):
            raise StabsError(f"definition expected in {s!r}")
        ch = s[i]
        if ch.isdigit() or ch == "-":
            target_num, j = self._number(s, i)
            if target_num == num and (j >= len(s) or s[j] != "="):
                return Basic(0, is_void=True), j          # `void:t19=19`
            ref, j = self._type(s, i)
            return Alias(ref), j
        if ch == "r":
            base, j = self._type(s, i + 1)
            j = self._expect(s, j, ";")
            lo, j = self._number(s, j)
            j = self._expect(s, j, ";")
            hi, j = self._number(s, j)
            j = self._expect(s, j, ";")
            return self._range(base, num, lo, hi), j
        if ch == "*":
            target, j = self._type(s, i + 1)
            return Pointer(target), j
        if ch in "kB":
            target, j = self._type(s, i + 1)
            return Qualified("const" if ch == "k" else "volatile", target), j
        if ch == "f":
            result, j = self._type(s, i + 1)
            return Function(result), j
        if ch == "a":
            index, j = self._type(s, i + 1)
            index_type = index.get()
            if not isinstance(index_type, _Range):
                raise StabsError(f"array index is not a range in {s!r}")
            element, j = self._type(s, j)
            return Array(index_type.lo, index_type.hi, element), j
        if ch in "su":
            size, j = self._number(s, i + 1)
            fields = []
            while j < len(s) and s[j] != ";":
                colon = s.index(":", j)
                fname = s[j:colon]
                ftype, k = self._type(s, colon + 1)
                k = self._expect(s, k, ",")
                bitpos, k = self._number(s, k)
                k = self._expect(s, k, ",")
                bitsize, k = self._number(s, k)
                j = self._expect(s, k, ";")
                fields.append((fname, ftype, bitpos, bitsize))
            j = self._expect(s, j, ";")
            return Struct("struct" if ch == "s" else "union", size, fields), j
        if ch == "e":
            values, j = [], i + 1
            while j < len(s) and s[j] != ";":
                colon = s.index(":", j)
                value, k = self._number(s, colon + 1)
                values.append((s[j:colon], value))
                j = self._expect(s, k, ",")
            return Enum(values), self._expect(s, j, ";")
        if ch == "x":
            kind = {"s": "struct", "u": "union", "e": "enum"}.get(s[i + 1:i + 2])
            if kind is None:
                raise StabsError(f"unknown cross-reference in {s!r}")
            colon = s.index(":", i + 2)
            return XRef(kind, s[i + 2:colon]), colon + 1
        raise StabsError(f"unsupported type descriptor {ch!r} in {s!r}")

    def _range(self, base, num, lo, hi):
        """A range is an integer, a float (`r1;N;0;`) or an array index."""
        if hi == 0 and lo > 0:
            return _Range(lo, hi, Basic(lo, is_float=True))
        if lo == 0 and hi == -1:
            # `r1;0;-1;`: unsigned of the base type's size, or an open index.
            base_size = None
            if base.num is None or base.num != num:
                try:
                    base_size = self.size(base)
                except StabsError:
                    base_size = None
            return _Range(lo, hi, Basic(base_size) if base_size else None)
        if lo > hi >= 0:
            # SN gcc 2.95 writes signed bounds as full-width octal two's
            # complement: int is r1;020000000000;017777777777;.
            size = next((n for n in SIZES if n * 8 >= lo.bit_length()), None)
            return _Range(lo, hi, Basic(size, signed=True) if size else None)
        bits = (max(hi, -lo - 1).bit_length() + (1 if lo < 0 else 0))
        size = next((n for n in SIZES if n * 8 >= bits), None)
        # `char` is printed as r2;0;127; but is signed on the EE.
        signed = lo < 0 or (lo == 0 and hi == 127)
        return _Range(lo, hi, Basic(size, signed=signed) if size else None)

    # -- queries -----------------------------------------------------------

    def resolve(self, ref):
        """Follow aliases and qualifiers to the underlying Type."""
        seen = set()
        while True:
            if ref.num is not None and ref.num in seen:
                raise StabsError(f"alias cycle at type {ref.num}")
            seen.add(ref.num)
            typ = ref.get()
            if isinstance(typ, _Range):
                return typ.as_basic() or typ
            if isinstance(typ, (Alias, Qualified)):
                ref = typ.target
                continue
            if isinstance(typ, XRef):
                full = self._tagged(typ)
                if full is None:
                    return typ
                ref = full
                continue
            return typ

    def _tagged(self, xref):
        for num, tag in self.tags.items():
            if tag == xref.tag:
                typ = self.types.get(num)
                kind = getattr(typ, "kind", None)
                if kind == xref.tag_kind or (xref.tag_kind == "enum"
                                             and isinstance(typ, Enum)):
                    return Ref(self, num)
        return None

    def size(self, ref):
        """Byte size of the type, or None when it is incomplete."""
        typ = self.resolve(ref)
        if isinstance(typ, Basic):
            return None if typ.is_void else typ.size
        if isinstance(typ, Pointer):
            return 4
        if isinstance(typ, Array):
            elem = self.size(typ.element)
            return None if typ.count is None or elem is None else typ.count * elem
        if isinstance(typ, Struct):
            return typ.size
        if isinstance(typ, Enum):
            return 4
        return None

    def c_type(self, ref, inner=""):
        """The C spelling of the type, declarator `inner` (may be empty)."""
        def join(base, decl):
            if not decl:
                return base
            return f"{base}{decl}" if decl.startswith("[") else f"{base} {decl}"

        name = None if ref.num is None else self.typedefs.get(ref.num)
        if name is not None:
            return join(name, inner)
        typ = ref.get()
        if isinstance(typ, _Range):
            raise StabsError(f"unnamed range type {ref.num}")
        if isinstance(typ, Basic) and typ.is_void:
            return join("void", inner)
        if ref.num in self.tags and isinstance(typ, (Struct, Enum)):
            kind = typ.kind if isinstance(typ, Struct) else "enum"
            return join(f"{kind} {self.tags[ref.num]}", inner)
        if isinstance(typ, Alias):
            return self.c_type(typ.target, inner)
        if isinstance(typ, Qualified):
            return self.c_type(typ.target, f"{typ.qualifier} {inner}".strip())
        if isinstance(typ, XRef):
            return join(f"{typ.tag_kind} {typ.tag}", inner)
        if isinstance(typ, Pointer):
            return self.c_type(typ.target, f"*{inner}")
        if isinstance(typ, Array):
            decl = f"({inner})" if inner.startswith("*") else inner
            count = "" if typ.count is None else str(typ.count)
            return self.c_type(typ.element, f"{decl}[{count}]")
        if isinstance(typ, Function):
            decl = f"({inner})" if inner.startswith("*") else inner
            return self.c_type(typ.result, f"{decl}()")
        if isinstance(typ, Struct):
            return join(f"{typ.kind} {{...}}", inner)
        if isinstance(typ, Enum):
            return join("enum {...}", inner)
        raise StabsError(f"cannot spell type {ref.num}")


class _Range(Type):
    """A parsed range; integer/float ranges act as Basic types."""
    kind = "range"

    def __init__(self, lo, hi, basic):
        self.lo, self.hi, self.basic = lo, hi, basic

    def as_basic(self):
        return self.basic


def join_continuations(strings):
    held = ""
    for string in strings:
        if string.endswith("\\"):
            held += string[:-1]
            continue
        yield held + string
        held = ""
    if held:
        raise StabsError(f"continued stab string never ends: {held[:60]!r}")


def symbol_type(table, string):
    """(descriptor, Ref) of a data symbol's stab string, e.g. `one:S13`."""
    _name, desc, ref = table.add(string)
    return desc, ref
