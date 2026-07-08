"""Canonical identifier parsing and formatting.

The canonical function id is the primary key everything else keys off:

    pal103:unit-0007:00105a60:NuListGetNext
    ^^^^^^ ^^^^^^^^^ ^^^^^^^^ ^^^^^^^^^^^^^
    version unit     vram     function name

  * version   -- lower-case alnum registry version (``pal103``)
  * unit      -- ``unit-NNNN`` zero-padded to four digits
  * vram      -- eight lower-case hex digits (the function's first byte)
  * name      -- the mdebug procedure name (may contain ``__<vram>`` for the
                 four disambiguated cross-TU statics; may contain ``$`` etc.)

A canonical unit id is the first two fields: ``pal103:unit-0007``.

Parsing is strict: a malformed id raises :class:`IdentifierError` rather than
being coerced. Nothing here guesses -- resolving a bare name or address against
the registry (and returning *ambiguity* instead of choosing) lives in
``registry.py``; this module only deals with the syntax.
"""
from __future__ import annotations

import re
from dataclasses import dataclass

_FUNCTION_RE = re.compile(r"^([a-z0-9]+):unit-(\d{4}):([0-9a-f]{8}):(\S+)$")
_UNIT_RE = re.compile(r"^([a-z0-9]+):unit-(\d{4})$")
_ADDRESS_RE = re.compile(r"^(?:0x)?([0-9a-fA-F]{1,8})$")
_NAME_RE = re.compile(r"^[A-Za-z_$][\w$.]*(?:__[0-9A-Fa-f]{8})?$")


class IdentifierError(ValueError):
    """A string could not be parsed as the canonical form it was expected to be."""


@dataclass(frozen=True)
class FunctionId:
    """A parsed canonical function id."""

    version: str
    unit_index: int
    address: int
    name: str

    @property
    def unit_id(self) -> str:
        return f"{self.version}:unit-{self.unit_index:04d}"

    def __str__(self) -> str:
        return (f"{self.version}:unit-{self.unit_index:04d}:"
                f"{self.address:08x}:{self.name}")

    def as_dict(self) -> dict:
        return {
            "id": str(self),
            "version": self.version,
            "unit_id": self.unit_id,
            "unit_index": self.unit_index,
            "address": self.address,
            "address_hex": f"0x{self.address:08x}",
            "name": self.name,
        }


@dataclass(frozen=True)
class UnitId:
    """A parsed canonical unit id."""

    version: str
    unit_index: int

    def __str__(self) -> str:
        return f"{self.version}:unit-{self.unit_index:04d}"

    def as_dict(self) -> dict:
        return {"id": str(self), "version": self.version,
                "unit_index": self.unit_index}


def parse_function_id(value: str) -> FunctionId:
    """Parse a canonical function id, or raise :class:`IdentifierError`."""
    if not isinstance(value, str):
        raise IdentifierError(f"function id must be a string, got {type(value).__name__}")
    m = _FUNCTION_RE.match(value.strip())
    if not m:
        raise IdentifierError(
            f"not a canonical function id: {value!r} "
            f"(expected 'version:unit-NNNN:vvvvvvvv:name')")
    return FunctionId(m.group(1), int(m.group(2)), int(m.group(3), 16), m.group(4))


def parse_unit_id(value: str) -> UnitId:
    """Parse a canonical unit id, or raise :class:`IdentifierError`."""
    if not isinstance(value, str):
        raise IdentifierError(f"unit id must be a string, got {type(value).__name__}")
    m = _UNIT_RE.match(value.strip())
    if not m:
        raise IdentifierError(
            f"not a canonical unit id: {value!r} (expected 'version:unit-NNNN')")
    return UnitId(m.group(1), int(m.group(2)))


def is_function_id(value: str) -> bool:
    return isinstance(value, str) and bool(_FUNCTION_RE.match(value.strip()))


def is_unit_id(value: str) -> bool:
    return isinstance(value, str) and bool(_UNIT_RE.match(value.strip()))


def parse_address(value) -> int:
    """Parse an address given as int, ``0x…`` or bare hex. Raise on garbage."""
    if isinstance(value, int):
        addr = value
    else:
        m = _ADDRESS_RE.match(str(value).strip())
        if not m:
            raise IdentifierError(f"not an address: {value!r}")
        addr = int(m.group(1), 16)
    if not 0 <= addr <= 0xFFFFFFFF:
        raise IdentifierError(f"address out of 32-bit range: {value!r}")
    return addr


def looks_like_address(value: str) -> bool:
    return isinstance(value, str) and bool(_ADDRESS_RE.match(value.strip()))


def is_plausible_name(value: str) -> bool:
    """A conservative check that ``value`` could be a function/symbol name."""
    return isinstance(value, str) and bool(_NAME_RE.match(value.strip()))


def make_function_id(version: str, unit_index: int, address: int, name: str) -> str:
    return f"{version}:unit-{unit_index:04d}:{address:08x}:{name}"


def make_unit_id(version: str, unit_index: int) -> str:
    return f"{version}:unit-{unit_index:04d}"
