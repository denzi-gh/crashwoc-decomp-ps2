"""Load the committed registries and resolve canonical targets.

Everything here reads only *committed* configuration -- the mdebug-derived
registries (``functions.toml``, ``units.toml``, ``symbol_addrs.txt``,
``sections.json``, ``data_map.toml``), the status manifests, and
``profiles.toml``. No game bytes, no toolchain. It is the layer-1 source of
truth every other module keys off.

The function *extent* is computed exactly the way the byte gates do it
(``tools/verify_promoted.py``): sort every procedure by address and take each
one's size as the gap to the next; the final procedure runs to the end of
``.text``. Exact matching is always verified over this full extent, never a
prefix.

Resolution never guesses. A bare name that is duplicated across translation
units resolves to *all* candidate ids (the caller must retry with a canonical
id); an address or canonical id resolves to exactly one record or to nothing.
"""
from __future__ import annotations

import re
import tomllib
from dataclasses import dataclass, field
from functools import cached_property

from . import identifiers
from .identifiers import FunctionId
from .project import DecompProject

STATES = ("asm", "equivalent", "matching")

# --- unit classification (ported from tools/gen_objdiff.classify, rooted here
#     so the domain layer never depends on a __file__-derived ROOT) -----------


def _stem_of(path: str) -> str:
    return re.sub(r"\.[A-Za-z]+$", "", path.replace("\\", "/").rsplit("/", 1)[-1])


def classify_unit(mdebug_name: str):
    """(category, unit_name) from one mdebug source path. Mirrors gen_objdiff."""
    norm = mdebug_name.replace("\\", "/")
    stem = _stem_of(norm)
    if "nu2crash.ps2/" in norm:
        rel = norm.split("nu2crash.ps2/", 1)[1]
        return "engine", re.sub(r"\.[A-Za-z]+$", "", rel)
    if norm.startswith("./") or norm == "vu/vu.c" or norm.endswith(".i"):
        return "game", f"game/{stem}"
    if "/newlib/" in norm:
        rel = norm.split("/newlib/", 1)[1]
        return "sdk", "sdk/newlib/" + re.sub(r"\.[A-Za-z]+$", "", rel)
    if "/gcc/" in norm or stem in ("dp-bit", "fp-bit"):
        return "sdk", f"sdk/gcc/{stem}"
    return "sdk", f"sdk/sce/{stem}"


def default_profile_for(category: str) -> str:
    """Category default compiler profile (mirrors tools/promote.default_profile_for)."""
    return "sce" if category == "sdk" else "default"


# --- records ----------------------------------------------------------------


@dataclass(frozen=True)
class UnitRecord:
    index: int
    mdebug_name: str
    category: str
    name: str            # normalized unit name (e.g. "nucore/nulist")
    source_path: str     # "src/nucore/nulist.c"
    manifest_path: str   # "config/<v>/status/nucore/nulist.toml"
    owns_text: bool

    def as_dict(self) -> dict:
        return {
            "unit_index": self.index,
            "unit_name": self.name,
            "category": self.category,
            "mdebug_name": self.mdebug_name,
            "source_path": self.source_path,
            "manifest_path": self.manifest_path,
            "owns_text": self.owns_text,
        }


@dataclass(frozen=True)
class FunctionRecord:
    version: str
    index: int
    name: str
    address: int
    unit_index: int
    size: int
    frame_size: int
    reg_mask: int
    freg_mask: int
    reg_offset: int
    freg_offset: int
    line_low: int
    line_high: int
    name_is_duplicated: bool

    @property
    def end(self) -> int:
        return self.address + self.size

    def function_id(self) -> FunctionId:
        return FunctionId(self.version, self.unit_index, self.address, self.name)

    def id_str(self) -> str:
        return str(self.function_id())

    def as_dict(self) -> dict:
        return {
            "id": self.id_str(),
            "name": self.name,
            "address": self.address,
            "address_hex": f"0x{self.address:08x}",
            "extent": [self.address, self.end],
            "extent_hex": [f"0x{self.address:08x}", f"0x{self.end:08x}"],
            "size": self.size,
            "unit_index": self.unit_index,
            "frame_size": self.frame_size,
            "reg_mask": f"0x{self.reg_mask:08x}",
            "freg_mask": f"0x{self.freg_mask:08x}",
            "line_low": self.line_low,
            "line_high": self.line_high,
            "name_is_duplicated": self.name_is_duplicated,
        }


@dataclass
class Resolution:
    """Outcome of resolving a target string."""

    status: str                         # "resolved" | "ambiguous" | "not_found"
    record: FunctionRecord = None
    unit: UnitRecord = None
    candidates: list = field(default_factory=list)   # canonical ids on ambiguity
    query: str = ""
    detail: str = ""

    def as_dict(self) -> dict:
        out = {"status": self.status, "query": self.query}
        if self.record is not None:
            out["function"] = self.record.as_dict()
        if self.unit is not None:
            out["unit"] = self.unit.as_dict()
        if self.candidates:
            out["candidates"] = self.candidates
        if self.detail:
            out["detail"] = self.detail
        return out


# --- the registry ------------------------------------------------------------


class Registry:
    """Cached, deterministic view of one version's committed registries."""

    def __init__(self, project: DecompProject):
        self.project = project
        self.version = project.version

    # -- raw loaders ---------------------------------------------------------

    def _load_toml(self, name: str) -> dict:
        return tomllib.loads(self.project.path("config", self.version, name).read_text())

    @cached_property
    def sections(self) -> dict:
        return tomllib_json(self.project.path("config", self.version, "sections.json"))

    @cached_property
    def _text_extent(self):
        for s in self.sections["sections"]:
            if s["name"] == ".text":
                addr = _h(s["addr"])
                return addr, addr + _h(s["size"])
        raise KeyError(".text section not found in sections.json")

    @cached_property
    def units(self) -> dict:
        """{unit_index: UnitRecord} for every mdebug unit (247 own .text)."""
        raw = {int(u["index"]): u["name"]
               for u in self._load_toml("units.toml")["unit"]}
        text_units = set(self.unit_first_address)     # units that own .text code

        # Normalized names, with collision suffixing over .text units only
        # (mirrors gen_objdiff.unit_table).
        names, cats = {}, {}
        for idx, mdebug in raw.items():
            cat, name = classify_unit(mdebug)
            name = re.sub(r"[^A-Za-z0-9_.\-/]", "_", name)
            names[idx], cats[idx] = name, cat
        counts = {}
        for idx in text_units:
            counts[names[idx]] = counts.get(names[idx], 0) + 1

        out = {}
        for idx, mdebug in raw.items():
            base = names[idx]
            name = f"{base}_{idx:03d}" if counts.get(base, 0) > 1 else base
            out[idx] = UnitRecord(
                index=idx, mdebug_name=mdebug, category=cats[idx], name=name,
                source_path=f"src/{name}.c",
                manifest_path=f"config/{self.version}/status/{name}.toml",
                owns_text=idx in text_units,
            )
        return out

    @cached_property
    def _raw_functions(self) -> list:
        return self._load_toml("functions.toml")["function"]

    @cached_property
    def unit_first_address(self) -> dict:
        """{unit_index: lowest function address} -- also the set of .text units."""
        firsts = {}
        for f in self._raw_functions:
            u, a = int(f["unit"]), _h(f["address"])
            if u not in firsts or a < firsts[u]:
                firsts[u] = a
        return firsts

    @cached_property
    def functions(self) -> list:
        """Address-sorted FunctionRecords with extents computed to the next."""
        text_end = self._text_extent[1]
        rows = sorted(self._raw_functions, key=lambda r: _h(r["address"]))
        name_counts = {}
        for r in rows:
            name_counts[r["name"]] = name_counts.get(r["name"], 0) + 1
        out = []
        for i, r in enumerate(rows):
            addr = _h(r["address"])
            end = _h(rows[i + 1]["address"]) if i + 1 < len(rows) else text_end
            out.append(FunctionRecord(
                version=self.version,
                index=int(r["index"]),
                name=r["name"],
                address=addr,
                unit_index=int(r["unit"]),
                size=max(0, end - addr),
                frame_size=_h(r.get("frame_size", 0)),
                reg_mask=_h(r.get("reg_mask", 0)),
                freg_mask=_h(r.get("freg_mask", 0)),
                reg_offset=_h(r.get("reg_offset", 0)),
                freg_offset=_h(r.get("freg_offset", 0)),
                line_low=int(r.get("line_low", 0)),
                line_high=int(r.get("line_high", 0)),
                name_is_duplicated=name_counts[r["name"]] > 1,
            ))
        return out

    @cached_property
    def by_id(self) -> dict:
        return {f.id_str(): f for f in self.functions}

    @cached_property
    def by_address(self) -> dict:
        out = {}
        for f in self.functions:
            out.setdefault(f.address, []).append(f)
        return out

    @cached_property
    def by_name(self) -> dict:
        out = {}
        for f in self.functions:
            out.setdefault(f.name, []).append(f)
        return out

    @cached_property
    def by_unit(self) -> dict:
        out = {}
        for f in self.functions:
            out.setdefault(f.unit_index, []).append(f)
        return out

    # -- profiles ------------------------------------------------------------

    @cached_property
    def profiles(self) -> dict:
        data = self._load_toml("profiles.toml")
        return {name: {"flags": list(spec["flags"]), "compiler": spec["compiler"]}
                for name, spec in data["profile"].items()}

    def effective_profile(self, unit_index: int):
        """(profile_name, source) for a unit: manifest wins, else category default."""
        manifest = self.manifest_for_unit(unit_index)
        if manifest is not None and manifest.get("profile") in self.profiles:
            return manifest["profile"], "manifest"
        unit = self.units.get(unit_index)
        category = unit.category if unit else "engine"
        default = default_profile_for(category)
        if default not in self.profiles:
            default = "default" if "default" in self.profiles else next(iter(self.profiles))
        return default, "category-default"

    # -- status manifests ----------------------------------------------------

    @cached_property
    def manifests(self) -> dict:
        """{unit_index: parsed manifest data} for every committed status manifest."""
        out = {}
        status_dir = self.project.status_dir
        if not status_dir.is_dir():
            return out
        for path in sorted(status_dir.rglob("*.toml")):
            try:
                data = tomllib.loads(path.read_text())
            except tomllib.TOMLDecodeError:
                continue
            m = re.match(r"^[a-z0-9]+:unit-(\d+)$", str(data.get("unit", "")))
            if m:
                data["_path"] = path.relative_to(self.project.root).as_posix()
                out[int(m.group(1))] = data
        return out

    def manifest_for_unit(self, unit_index: int):
        return self.manifests.get(unit_index)

    @cached_property
    def _state_by_id(self) -> dict:
        out = {}
        for data in self.manifests.values():
            for entry in data.get("function", []):
                out[entry["id"]] = entry.get("state", "asm")
        return out

    def state_of(self, function_id: str) -> str:
        """Committed state for a function id (default ``asm`` if not listed)."""
        return self._state_by_id.get(function_id, "asm")

    def manifest_path_for(self, unit_index: int):
        data = self.manifests.get(unit_index)
        if data is not None:
            return data["_path"]
        unit = self.units.get(unit_index)
        return unit.manifest_path if unit else None

    # -- symbols & data ------------------------------------------------------

    @cached_property
    def symbols(self) -> dict:
        """name -> address from the committed symbol registry (+ splat auto lists)."""
        addrs = {}
        line_re = re.compile(r"(\S+)\s*=\s*(0x[0-9A-Fa-f]+)")
        for rel in ("config/%s/symbol_addrs.txt" % self.version,
                    "build/undefined_syms_auto.txt",
                    "build/undefined_funcs_auto.txt"):
            try:
                path = self.project.path(*rel.split("/"))
            except Exception:
                continue
            if not path.is_file():
                continue
            for line in path.read_text().splitlines():
                m = line_re.match(line.split("//")[0].strip().rstrip(";"))
                if m:
                    addrs.setdefault(m.group(1), int(m.group(2), 16))
        return addrs

    @cached_property
    def data_ranges(self) -> list:
        path = self.project.path("config", self.version, "data_map.toml")
        if not path.is_file():
            return []
        return tomllib.loads(path.read_text()).get("range", [])

    def data_ranges_for_unit(self, unit_index: int) -> list:
        owner = f"unit-{unit_index:04d}"
        return [r for r in self.data_ranges if r.get("owner") == owner]

    # -- resolution ----------------------------------------------------------

    def resolve(self, target) -> Resolution:
        """Resolve a canonical id / unit id / name / address to one record.

        Returns a :class:`Resolution`. Ambiguous bare names yield every
        candidate id; nothing is chosen on the caller's behalf.
        """
        query = str(target).strip()

        if identifiers.is_function_id(query):
            rec = self.by_id.get(query)
            if rec is not None:
                return Resolution("resolved", record=rec,
                                  unit=self.units.get(rec.unit_index), query=query)
            # Well-formed but not in the registry: try (unit, addr) match to
            # surface a helpful "renamed?" ambiguity, else not_found.
            fid = identifiers.parse_function_id(query)
            for rec in self.by_address.get(fid.address, []):
                if rec.unit_index == fid.unit_index:
                    return Resolution(
                        "not_found", query=query,
                        detail=f"no function named {fid.name!r} at "
                               f"0x{fid.address:08x}; registry has "
                               f"{rec.name!r} there")
            return Resolution("not_found", query=query,
                              detail="canonical id not present in functions.toml")

        if identifiers.is_unit_id(query):
            uid = identifiers.parse_unit_id(query)
            unit = self.units.get(uid.unit_index)
            if unit is None:
                return Resolution("not_found", query=query,
                                  detail=f"unit {uid.unit_index} not in units.toml")
            return Resolution("resolved", unit=unit, query=query,
                              detail="unit id (no single function selected)")

        if identifiers.looks_like_address(query):
            addr = identifiers.parse_address(query)
            hits = self.by_address.get(addr, [])
            if not hits:
                return Resolution("not_found", query=query,
                                  detail=f"no function at 0x{addr:08x}")
            if len(hits) == 1:
                rec = hits[0]
                return Resolution("resolved", record=rec,
                                  unit=self.units.get(rec.unit_index), query=query)
            return Resolution("ambiguous", query=query,
                              candidates=[h.id_str() for h in hits],
                              detail=f"{len(hits)} functions at 0x{addr:08x}")

        # Bare name.
        hits = self.by_name.get(query, [])
        if not hits:
            return Resolution("not_found", query=query,
                              detail=f"no function named {query!r}")
        if len(hits) == 1:
            rec = hits[0]
            return Resolution("resolved", record=rec,
                              unit=self.units.get(rec.unit_index), query=query)
        return Resolution("ambiguous", query=query,
                          candidates=[h.id_str() for h in hits],
                          detail=f"{len(hits)} functions named {query!r}; "
                                 f"retry with a canonical id")

    def require_function(self, target) -> FunctionRecord:
        """Resolve to exactly one function record or raise ValueError."""
        res = self.resolve(target)
        if res.status == "resolved" and res.record is not None:
            return res.record
        if res.status == "ambiguous":
            raise ValueError(f"ambiguous target {target!r}: {', '.join(res.candidates)}")
        raise ValueError(f"cannot resolve {target!r}: {res.detail}")


def _h(value):
    return int(value, 0) if isinstance(value, str) else int(value)


def tomllib_json(path):
    import json
    return json.loads(path.read_text())
