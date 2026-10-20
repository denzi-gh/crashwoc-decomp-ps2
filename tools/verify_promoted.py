#!/usr/bin/env python3
"""Re-derive every promotion claim; a manifest is never trusted, only bytes.

For each status manifest this verifier proves, from scratch:

  1. Manifest consistency (tools/status.py rules).
  2. Every function declared `matching` or `equivalent` is defined in the
     plain compile of the unit's C (WIP C beyond the declarations is fine
     and reported, never fatal).
  3. Every `matching` function is BYTE-IDENTICAL to retail over its full
     registry extent [addr, addr+size), where size is the distance to the
     next function -- a shrunken function can never "match" its prefix.
     The bytes are taken from the freshly rebuilt matching hybrid (the
     object the canonical image links), which carries gen_hybrid's
     per-function .lit4 slot mapping; gen_hybrid guarantees a `matching`
     function's hybrid bytes always come from the C compile.
  4. Every `matching` data entry ([[data]]) is BYTE-IDENTICAL to retail over
     [addr, addr+size) -- size from .symtab/STABS, never the extent; an
     unsized object uses its compiled .size only if at most 15 zero bytes,
     or exactly the zero pad aligning it (up to 128), remain to the next
     item -- and its linked symbol sits at the registry
     address. Padding and compiler-private bytes are left to the image gate.
  5. A `complete` unit: every function `matching`, every data entry
     `matching`/`retail`, no owned .sbss/.bss range (data/bss), and its
     whole-unit hybrid (gen_hybrid: compiler-private data, jump tables,
     pools and the .lit4 pool from C) carries no retail bytes in its owned
     ranges except its `retail` entries, borrows no retail slot, leaves no
     owned range to the expected data object, and every piece it links --
     owned ranges and the gap ranges it `[[claim]]`s -- equals retail.
  6. The image gate: link the full matching image from the hybrids step 3
     rebuilt (rebuilding any it did not) -- it must reproduce the retail
     loaded-image SHA.

Results land in build/<version>/verify_results.json (the artifact the
progress tooling and CI consume), stamped with the profiles fingerprint so
a flag or compiler change can never silently keep old "matching" results.

Needs the EE GCC and PS2 binutils: run in the Containerfile image
(`ninja verify-promoted` or `python tools/dispatch.py ninja verify-promoted`).

Exit status is 0 only if every claim verified and the image is exact.
"""
import argparse
import hashlib
import json
import sys
import tempfile
import tomllib
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from cc import compile_c, profiles_fingerprint
from gen_hybrid import (HybridError, build_hybrid, data_report_path,
                        has_c_data, provides_data)
from link_image import link_image
from status import check_version
from declib.asmtext import load_symbol_addrs, load_text_target, resolve
from declib.toolchain import LD, NM, Reporter, tool_path
from declib.tu import parse_toml_blocks
from declib.verify import (defined_function_offsets, link_text_at,
                           link_unit_at, undefined_externals)

LABEL_WIDTH = 44


def function_sizes(version):
    """{unit: {name: (addr, size)}} from the registry; size = gap to next."""
    _elf, text_addr, _off, text_size = load_text_target(version)
    rows = [(int(r["a"], 16), r["n"], int(r["u"]))
            for r in parse_toml_blocks(
                ROOT / "config" / version / "functions.toml",
                {"n": r"name = '([^']*)'", "a": r"address = (0x[0-9A-Fa-f]+)",
                 "u": r"unit = (\d+)"})]
    rows.sort()
    out = {}
    for i, (addr, name, unit) in enumerate(rows):
        end = rows[i + 1][0] if i + 1 < len(rows) else text_addr + text_size
        out.setdefault(unit, {})[name] = (addr, end - addr)
    return out


def _registry(version):
    """{unit: {address: symbol}} from the data registry."""
    from status import load_data_registry
    return load_data_registry(ROOT / "config" / version)


def _retail_data(elf, version, addr, size):
    """Retail file bytes of [addr, addr+size) (zeros for NOBITS)."""
    spec = json.loads((ROOT / "config" / version / "sections.json").read_text())
    for s in spec["sections"]:
        lo, sz = int(s["addr"], 0), int(s["size"], 0)
        if lo <= addr and addr + size <= lo + sz and lo:
            if int(s["type"], 0) == 8:
                return bytes(size)
            off = int(s["offset"], 0) + addr - lo
            return elf[off:off + size]
    return None


def verify_data(reporter, label, data, unit_index, version, elf, pieces,
                linked_pieces, symbols, results):
    """Byte-verify every `matching` data entry of one unit (step 4)."""
    registry = _registry(version).get(unit_index, {})
    objects = {o["address"]: o for p in pieces for o in p.get("objects", [])}
    bad = []
    for entry in data.get("data", []):
        if entry["state"] != "matching":
            continue
        addr = int(entry["id"].split(":")[2], 16)
        sym = registry.get(addr, {})
        obj = objects.get(addr)
        size = sym.get("size")
        reason = None
        if obj is None:
            reason = "not emitted by the hybrid"
        elif size is None:
            # Unsized (an .lcomm-like static): the compiled size, when the
            # rest of the registry extent is zero pad -- at most 15 bytes,
            # or exactly what aligns the next item (the next object file's
            # section alignment, at most 128).
            size = obj["size"]
            rest = sym.get("extent", size) - size
            pad = _retail_data(elf, version, addr + size, rest) if rest > 0 else b""
            nxt = addr + sym.get("extent", size)
            align = min(nxt & -nxt, 128)
            if rest < 0 or (rest > 15 and rest >= align) or (pad and any(pad)):
                reason = (f"unsized: compiled size {size:#x} leaves {rest:#x} "
                          f"non-pad bytes of the extent")
        if reason is None:
            piece = next(p for p in pieces if p["start"] <= addr < p["end"])
            blob = linked_pieces[piece["name"]]
            got = blob[addr - piece["start"]:addr - piece["start"] + size]
            want = _retail_data(elf, version, addr, size)
            if got != want:
                reason = "bytes differ from retail"
            elif symbols.get(sym.get("name")) != addr:
                reason = (f"linked at {symbols.get(sym.get('name'), 0):#010x}, "
                          f"registry {addr:#010x}")
        ok = reason is None
        if not ok:
            bad.append(sym.get("name", entry["id"]))
        results.append({"id": entry["id"], "kind": "data", "state": "matching",
                        "verified": ok, "size": size,
                        **({"reason": reason} if reason else {})})
    count = sum(1 for e in data.get("data", []) if e["state"] == "matching")
    reporter.result(f"{label} (matching data, {count} sym)", not bad,
                    None if not bad else f"diverge: {bad}")


def complete_problems(data, unit_index, version, report, ranges):
    """Why a unit may not be `complete` (step 5); [] when it may."""
    out = []
    owner = f"unit-{unit_index:04d}"
    owned = [r for r in ranges if r["owner"] == owner]
    nobits = [r for r in owned if r["section"] in (".sbss", ".bss")]
    if nobits:
        out.append(f"owns {len(nobits)} .sbss/.bss range(s) (data/bss)")
    if not (report or {}).get("whole_unit"):
        out.append("its hybrid was not built in whole-unit mode")
        return out
    registry = _registry(version).get(unit_index, {})
    retail_ok = []
    for entry in data.get("data", []):
        if entry["state"] == "retail":
            addr = int(entry["id"].split(":")[2], 16)
            sym = registry.get(addr, {})
            retail_ok.append((addr, addr + (sym.get("size") or sym.get("extent", 0))))
    progbits = [r for r in owned if r["section"] not in (".sbss", ".bss")]
    pieces = {p["start"] for p in report.get("pieces", [])}
    left = [r for r in progbits if r["start"] not in pieces]
    if left:
        out.append(f"{len(left)} owned data range(s) still come from the "
                   f"expected object (no C data pieces)")
    for lo, hi in report.get("retail", []):
        cursor = lo
        for a, b in sorted(retail_ok):
            if a <= cursor < b:
                cursor = b
        if cursor < hi:
            out.append(f"retail bytes at 0x{cursor:08x} in its pieces (the C "
                       f"does not emit them)")
            break
    borrowed = [s["symbol"] for s in report.get("borrowed", [])]
    if borrowed:
        out.append(f"borrows retail data ({', '.join(borrowed[:3])})")
    return out


def verify_manifest(reporter, manifest, version, elf, text_addr, text_off,
                    sizes, nm_bin, results, built=None):
    """Verify one manifest's claims; add it to `built` once rebuilt."""
    data = tomllib.loads(manifest.read_text())
    src = ROOT / data["source"]
    rel = Path(data["source"]).relative_to("src").with_suffix("")
    unit_index = int(data["unit"].split("unit-")[1])
    label = rel.as_posix()
    unit_sizes = sizes.get(unit_index, {})

    declared = {}
    for entry in data.get("function", []):
        name = entry["id"].rsplit(":", 1)[1]
        declared[name] = entry
    nonasm = {n: e for n, e in declared.items()
              if e["state"] in ("matching", "equivalent")}
    c_data = provides_data(data)
    if not nonasm and not c_data:
        if data.get("complete"):
            reporter.result(f"{label} (complete)", False,
                            "complete needs every function `matching`")
        return

    current_o = ROOT / "build" / version / "current" / rel.with_suffix(".o")
    compile_c(src, current_o, profile=data.get("profile", "default"),
              version=version)
    offsets = defined_function_offsets(nm_bin, current_o)

    missing = sorted(set(nonasm) - set(offsets))
    reporter.result(f"{label} (declared functions defined)", not missing,
                    None if not missing else f"missing from C: {missing}")
    if missing:
        for name, entry in nonasm.items():
            results.append({"id": entry["id"], "state": entry["state"],
                            "verified": False,
                            "reason": "declared but not defined in C"
                            if name in missing else "unit failed"})
        return

    matching = {n: e for n, e in nonasm.items() if e["state"] == "matching"}
    if matching or c_data:
        # Byte source is the freshly built MATCHING HYBRID, not the plain
        # compile: the hybrid object is what the canonical image links, and
        # it is where gen_hybrid's per-function .lit4 slot mapping lives (a
        # plain compile lays its literal pool out its own way, so a function
        # that loads pool constants could never byte-match through it).
        # gen_hybrid itself guarantees every `matching` function's bytes in
        # the hybrid come from the C compile, never from a slice.
        hybrid_o = ROOT / "build" / version / "matching" / rel.with_suffix(".o")
        try:
            build_hybrid(manifest, hybrid_o, link_set="matching",
                         version=version)
        except HybridError as exc:
            reporter.result(f"{label} (matching bytes)", False, str(exc))
            for entry in nonasm.values():
                results.append({"id": entry["id"], "state": entry["state"],
                                "verified": False,
                                "reason": f"hybrid build failed: {exc}"})
            return
        if built is not None:
            built.add(manifest)
        hybrid_offsets = defined_function_offsets(nm_bin, hybrid_o)
        base_addr = min(unit_sizes[n][0]
                        for n in hybrid_offsets if n in unit_sizes)
        report = json.loads(data_report_path(hybrid_o).read_text())
        undef = undefined_externals(nm_bin, hybrid_o)
        defsyms, unresolved = resolve(sorted(undef - {"_gp"}),
                                      load_symbol_addrs(version))
        if unresolved:
            reporter.result(f"{label} (matching bytes)", False,
                            f"unresolved externals: {unresolved[:4]}")
            for entry in nonasm.values():
                results.append({"id": entry["id"], "state": entry["state"],
                                "verified": False,
                                "reason": "unresolved externals"})
            return
        with tempfile.TemporaryDirectory() as tmp:
            if report["pieces"]:
                linked, linked_pieces, symbols = link_unit_at(
                    hybrid_o, base_addr, report["pieces"], defsyms, Path(tmp))
            else:
                linked = link_text_at(hybrid_o, base_addr, defsyms, Path(tmp))
        bad = []
        for name, entry in sorted(matching.items(),
                                  key=lambda kv: unit_sizes[kv[0]][0]):
            addr, size = unit_sizes[name]
            got = linked[hybrid_offsets[name]: hybrid_offsets[name] + size]
            want = elf[text_off + (addr - text_addr):
                       text_off + (addr - text_addr) + size]
            ok = got == want and len(got) == size
            if not ok:
                bad.append(name)
            results.append({"id": entry["id"], "state": "matching",
                            "verified": ok, "size": size})
        if matching:
            reporter.result(f"{label} (matching bytes, {len(matching)} fn)",
                            not bad, None if not bad else f"diverge: {bad}")
        if has_c_data(data):
            verify_data(reporter, label, data, unit_index, version, elf,
                        report["pieces"], linked_pieces, symbols, results)
        if report.get("whole_unit") and report["pieces"]:
            # Every byte a whole-unit hybrid links, its gap claims included.
            off = [p["name"] for p in report["pieces"]
                   if linked_pieces[p["name"]] != _retail_data(
                       elf, version, p["start"], p["end"] - p["start"])]
            reporter.result(f"{label} (unit data, {len(report['pieces'])} "
                            f"pieces)", not off,
                            None if not off else f"diverge: {off[:4]}")

    for name, entry in nonasm.items():
        if entry["state"] == "equivalent":
            results.append({"id": entry["id"], "state": "equivalent",
                            "verified": True, "note": "defined; bytes not "
                            "required for equivalent"})

    if data.get("complete"):
        ranges = tomllib.loads(
            (ROOT / "config" / version / "data_map.toml").read_text())["range"]
        report = None
        hybrid_o = ROOT / "build" / version / "matching" / rel.with_suffix(".o")
        if data_report_path(hybrid_o).is_file():
            report = json.loads(data_report_path(hybrid_o).read_text())
        why = complete_problems(data, unit_index, version, report, ranges)
        if any(e["state"] != "matching" for e in declared.values()):
            why.insert(0, "not every function is `matching`")
        reporter.result(f"{label} (complete)", not why, "; ".join(why) or None)


def _is_data(result):
    """A results row belongs to `data` when its id names a data entry."""
    return result.get("kind") == "data"


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--version", default="pal103")
    parser.add_argument("--skip-image", action="store_true",
                        help="skip the full matching-image SHA gate (used by "
                             "callers that run it themselves)")
    args = parser.parse_args()

    nm_bin = tool_path(NM)
    if not nm_bin or not tool_path(LD):
        print("PS2 binutils not found; run in the Containerfile image.",
              file=sys.stderr)
        return 2

    status_dir = ROOT / "config" / args.version / "status"
    manifests = sorted(status_dir.rglob("*.toml")) if status_dir.is_dir() else []
    if not manifests:
        print("no status manifests; nothing to verify.", file=sys.stderr)
        return 2

    print(f"Promotion verification: {args.version}, "
          f"{len(manifests)} manifest(s)\n")
    reporter = Reporter(LABEL_WIDTH)

    # 1. Manifest consistency (same rules the public CI runs).
    problems, _counts = check_version(args.version, ROOT)
    bad = {k: v for k, v in problems.items() if v}
    reporter.result("Manifest consistency (status rules)", not bad,
                    "; ".join(f"{k}: {v[0]}" for k, v in
                              sorted(bad.items())[:3]) or None)

    results = []
    built = set()                 # manifests verify_manifest rebuilt a hybrid for
    if not bad:
        elf, text_addr, text_off, _size = load_text_target(args.version)
        sizes = function_sizes(args.version)
        for manifest in manifests:
            verify_manifest(reporter, manifest, args.version, elf, text_addr,
                            text_off, sizes, nm_bin, results, built)

    # 5. Image gate over freshly rebuilt hybrids.
    image = {"matching_sha256": None, "exact": False}
    if not reporter.failed and not args.skip_image:
        try:
            for manifest in manifests:
                data = tomllib.loads(manifest.read_text())
                # Only units with a `matching` function contribute a hybrid to
                # the image (same rule as gen_ninja); an all-`asm` skeleton has
                # no function bodies, so its compiled `.s` is just the file
                # prologue and build_hybrid would (correctly) refuse to parse it.
                states = {f["state"] for f in data.get("function", [])}
                if "matching" not in states and not provides_data(data):
                    continue
                if manifest in built:     # already rebuilt by verify_manifest
                    continue
                rel = Path(data["source"]).relative_to("src").with_suffix(".o")
                build_hybrid(manifest,
                             ROOT / "build" / args.version / "matching" / rel,
                             link_set="matching", version=args.version)
        except HybridError as exc:
            reporter.result("Rebuild matching hybrids", False, str(exc))
        else:
            sha_ok, bin_out = link_image(reporter, args.version, "matching")
            image["exact"] = bool(sha_ok)
            if bin_out and bin_out.is_file():
                image["matching_sha256"] = hashlib.sha256(
                    bin_out.read_bytes()).hexdigest()
            reporter.result("Loaded image byte-identical to retail",
                            bool(sha_ok),
                            None if sha_ok else "SHA mismatch")

    out = ROOT / "build" / args.version / "verify_results.json"
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps({
        "schema": 1,
        "version": args.version,
        "profiles_fingerprint": profiles_fingerprint(args.version),
        "all_green": not reporter.failed,
        "functions": [r for r in results if not _is_data(r)],
        "data": [r for r in results if _is_data(r)],
        "image": image,
    }, indent=2) + "\n")
    print(f"\nwrote {out.relative_to(ROOT).as_posix()}")

    if reporter.failed:
        print("\nPromotion verification FAILED:")
        for detail in reporter.details:
            print(f"  - {detail}")
        return 1
    print("Promotion verification OK.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
