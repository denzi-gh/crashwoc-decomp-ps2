"""Mod manifest (mod.toml) schema and validation.

    [mod]           name, version, target = "pal103"
    [code]          sources = ["mod.c"]            (optional)
    [[hook]]        function, handler, mode = "pre" | "replace" | "supplant"
    [[data_patch]]  address (or symbol + offset), bytes = "hex"
    [mailbox]       size                           (one mod per build)

Structural validation only; symbol existence and hook-site safety are the
builder's job.
"""
import re
import tomllib
from dataclasses import dataclass, field
from pathlib import Path

NAME_RE = re.compile(r"^[a-z0-9_-]+$")
IDENT_RE = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")
HOOK_MODES = ("pre", "replace", "supplant")
TARGETS = ("pal103",)


class ManifestError(ValueError):
    pass


@dataclass
class Hook:
    function: str
    handler: str
    mode: str


@dataclass
class DataPatch:
    data: bytes
    address: int | None = None
    symbol: str | None = None
    offset: int = 0


@dataclass
class Manifest:
    name: str
    version: str
    target: str
    directory: Path
    sources: list[Path] = field(default_factory=list)
    hooks: list[Hook] = field(default_factory=list)
    data_patches: list[DataPatch] = field(default_factory=list)
    mailbox_size: int = 0


def _require(cond, msg):
    if not cond:
        raise ManifestError(msg)


def _int_value(raw, what):
    _require(isinstance(raw, int) and not isinstance(raw, bool),
             f"{what} must be an integer")
    return raw


def load_manifest(path):
    path = Path(path)
    try:
        raw = tomllib.loads(path.read_text())
    except (OSError, tomllib.TOMLDecodeError) as exc:
        raise ManifestError(f"{path}: {exc}") from exc
    directory = path.resolve().parent

    mod = raw.get("mod")
    _require(isinstance(mod, dict), f"{path}: missing [mod] table")
    name = mod.get("name", "")
    _require(isinstance(name, str) and NAME_RE.match(name),
             f"{path}: [mod] name must match {NAME_RE.pattern}")
    version = mod.get("version", "0")
    _require(isinstance(version, str) and version,
             f"{path}: [mod] version must be a non-empty string")
    target = mod.get("target", "")
    _require(target in TARGETS,
             f"{path}: [mod] target must be one of {TARGETS}")

    sources = []
    code = raw.get("code")
    if code is not None:
        _require(isinstance(code, dict), f"{path}: [code] must be a table")
        src_list = code.get("sources", [])
        _require(isinstance(src_list, list) and src_list,
                 f"{path}: [code] sources must be a non-empty list")
        for src in src_list:
            _require(isinstance(src, str) and src.endswith(".c"),
                     f"{path}: [code] sources entries must be .c paths")
            src_path = directory / src
            _require(not Path(src).is_absolute() and ".." not in Path(src).parts,
                     f"{path}: source {src} must stay inside the mod directory")
            _require(src_path.is_file(), f"{path}: source not found: {src}")
            sources.append(src_path)

    hooks = []
    for i, h in enumerate(raw.get("hook", [])):
        where = f"{path}: [[hook]] #{i + 1}"
        _require(isinstance(h, dict), f"{where} must be a table")
        function, handler = h.get("function", ""), h.get("handler", "")
        _require(isinstance(function, str) and IDENT_RE.match(function),
                 f"{where}: function must be a C identifier")
        _require(isinstance(handler, str) and IDENT_RE.match(handler),
                 f"{where}: handler must be a C identifier")
        mode = h.get("mode", "pre")
        _require(mode in HOOK_MODES, f"{where}: mode must be one of {HOOK_MODES}")
        _require(sources, f"{where}: hooks need [code] sources for the handler")
        hooks.append(Hook(function=function, handler=handler, mode=mode))
    seen = set()
    for h in hooks:
        _require(h.function not in seen,
                 f"{path}: duplicate hook on {h.function}")
        seen.add(h.function)

    data_patches = []
    for i, p in enumerate(raw.get("data_patch", [])):
        where = f"{path}: [[data_patch]] #{i + 1}"
        _require(isinstance(p, dict), f"{where} must be a table")
        address, symbol = p.get("address"), p.get("symbol")
        _require((address is None) != (symbol is None),
                 f"{where}: exactly one of address / symbol")
        if address is not None:
            address = _int_value(address, f"{where}: address")
        if symbol is not None:
            _require(isinstance(symbol, str) and IDENT_RE.match(symbol),
                     f"{where}: symbol must be a C identifier")
        offset = _int_value(p.get("offset", 0), f"{where}: offset")
        hexstr = p.get("bytes", "")
        _require(isinstance(hexstr, str) and hexstr
                 and len(hexstr) % 2 == 0
                 and re.fullmatch(r"[0-9a-fA-F]+", hexstr),
                 f"{where}: bytes must be a non-empty even-length hex string")
        data_patches.append(DataPatch(data=bytes.fromhex(hexstr),
                                      address=address, symbol=symbol,
                                      offset=offset))

    mailbox_size = 0
    mailbox = raw.get("mailbox")
    if mailbox is not None:
        _require(isinstance(mailbox, dict), f"{path}: [mailbox] must be a table")
        mailbox_size = _int_value(mailbox.get("size"), f"{path}: [mailbox] size")
        _require(0 < mailbox_size <= 0x10000,
                 f"{path}: [mailbox] size must be in (0, 0x10000]")

    known = {"mod", "code", "hook", "data_patch", "mailbox"}
    unknown = set(raw) - known
    _require(not unknown, f"{path}: unknown table(s): {', '.join(sorted(unknown))}")

    return Manifest(name=name, version=version, target=target,
                    directory=directory, sources=sources, hooks=hooks,
                    data_patches=data_patches, mailbox_size=mailbox_size)


def load_manifests(paths):
    manifests = [load_manifest(p) for p in paths]
    names = set()
    hooked = {}
    mailboxes = 0
    for m in manifests:
        _require(m.name not in names, f"duplicate mod name: {m.name}")
        names.add(m.name)
        for h in m.hooks:
            _require(h.function not in hooked,
                     f"conflicting hooks on {h.function}: "
                     f"{hooked.get(h.function)} and {m.name} "
                     f"(hook chaining is not supported yet)")
            hooked[h.function] = m.name
        mailboxes += 1 if m.mailbox_size else 0
    _require(mailboxes <= 1, "only one mod may declare [mailbox]")
    return manifests
