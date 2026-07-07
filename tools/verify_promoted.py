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
  4. A `complete` unit must not own data ranges yet (data-from-C is not
     supported until the hybrid can splice data sections); fail loudly
     rather than silently under-verify.
  5. The image gate: rebuild every matching hybrid and link the full
     matching image -- it must reproduce the retail loaded-image SHA.

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
from gen_hybrid import HybridError, build_hybrid
from link_image import link_image
from status import check_version
from declib.asmtext import load_symbol_addrs, load_text_target, resolve
from declib.toolchain import LD, NM, Reporter, tool_path
from declib.tu import parse_toml_blocks
from declib.verify import (defined_function_offsets, link_text_at,
                           undefined_externals)

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


def verify_manifest(reporter, manifest, version, elf, text_addr, text_off,
                    sizes, nm_bin, results):
    """Verify one manifest's function claims; append per-function results."""
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
    if not nonasm:
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
    if matching:
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
        hybrid_offsets = defined_function_offsets(nm_bin, hybrid_o)
        base_addr = min(unit_sizes[n][0]
                        for n in hybrid_offsets if n in unit_sizes)
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
        reporter.result(f"{label} (matching bytes, {len(matching)} fn)",
                        not bad, None if not bad else f"diverge: {bad}")

    for name, entry in nonasm.items():
        if entry["state"] == "equivalent":
            results.append({"id": entry["id"], "state": "equivalent",
                            "verified": True, "note": "defined; bytes not "
                            "required for equivalent"})

    if data.get("complete"):
        owned = [r for r in tomllib.loads(
            (ROOT / "config" / version / "data_map.toml").read_text())["range"]
            if r["owner"] == f"unit-{unit_index:04d}"]
        reporter.result(f"{label} (complete: no owned data yet)", not owned,
                        None if not owned else
                        f"unit owns {len(owned)} data ranges; data-from-C "
                        "verification is not supported yet")


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
    if not bad:
        elf, text_addr, text_off, _size = load_text_target(args.version)
        sizes = function_sizes(args.version)
        for manifest in manifests:
            verify_manifest(reporter, manifest, args.version, elf, text_addr,
                            text_off, sizes, nm_bin, results)

    # 5. Image gate over freshly rebuilt hybrids.
    image = {"matching_sha256": None, "exact": False}
    if not reporter.failed and not args.skip_image:
        try:
            for manifest in manifests:
                data = tomllib.loads(manifest.read_text())
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
        "functions": results,
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
