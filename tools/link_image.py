#!/usr/bin/env python3
"""Link the full loaded image from a named object set; gate on the retail SHA.

This is the canonical-image step: where split_text.py proved the pure
reassembly reconstructs the retail image, this tool links the image the
project actually ships per build mode. For every .text translation unit it
picks one object, in retail address order:

  * a unit with a status manifest contributes its HYBRID object
    (build/<version>/<set>/<unit>.o -- decompiled C spliced with retail
    slices by tools/gen_hybrid.py),
  * every other unit contributes its expected object
    (expected/<version>/NNN_<name>.o -- the retail slice reassembled).

The six non-.text loaded sections ride along as `.incbin` objects exactly as
in the baselines, and the result is objcopy'd into the loaded image.

Two sets, two gates:

  * --set matching   must be byte-identical to retail: the SHA-256 of the
                     linked image must equal the retail PT_LOAD bytes.
                     This is `ninja verify-loaded` -- proof that the
                     decompiled C really is in the canonical image and the
                     image is still exact.
  * --set equivalent must link (reviewed-equivalent C included); the SHA is
                     reported but not required. This is the modding build:
                     the ELF PCSX2 boots for patches that diverge from
                     retail. NOTE while externals resolve to fixed retail
                     addresses (absolute defsyms), modified code must not
                     change function sizes; a relocatable link is a later
                     milestone.

Needs the PS2 binutils: run in the Containerfile image (from the host:
`python tools/dispatch.py ninja verify-loaded`). All outputs are
game-derived and land in gitignored build/<version>/image/.

Exit status is 0 only if the set's gate holds.
"""
import argparse
import hashlib
import subprocess
import sys
import tomllib
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from declib.toolchain import AS, LD, NM, OBJCOPY, Reporter, h, tool_path
from declib.asmtext import load_symbol_addrs, resolve
from declib.target import derive_tiling, load_target, parse_linker_script
from declib.tu import load_tu_runs, load_units
from gen_expected_s import unit_stem
from split_text import (build_link_script, nm_defined_undefined,
                        write_incbin_objects)

LINK_SETS = ("matching", "equivalent")


def manifest_units(version):
    """{unit_index: object path relative to build/<version>/<set>/}."""
    out = {}
    status_dir = ROOT / "config" / version / "status"
    if status_dir.is_dir():
        for m in sorted(status_dir.rglob("*.toml")):
            data = tomllib.loads(m.read_text())
            idx = int(data["unit"].split("unit-")[1])
            out[idx] = Path(data["source"]).relative_to("src").with_suffix(".o")
    return out


def select_text_objects(version, link_set):
    """[(unit_index, object_path)] in retail address order.

    Hybrid object where a status manifest exists, expected object otherwise.
    """
    hybrids = manifest_units(version)
    names = load_units()
    objs = []
    for unit, _addr in load_tu_runs():
        if unit in hybrids:
            objs.append((unit,
                         ROOT / "build" / version / link_set / hybrids[unit]))
        else:
            objs.append((unit, ROOT / "expected" / version
                         / f"{unit_stem(unit, names)}.o"))
    return objs


def link_image(reporter, version, link_set):
    """Link the image, compare its SHA to retail; returns (sha_ok, out_bin)."""
    as_bin, ld_bin = tool_path(AS), tool_path(LD)
    nm_bin, objcopy_bin = tool_path(NM), tool_path(OBJCOPY)
    if not all((as_bin, ld_bin, nm_bin, objcopy_bin)):
        reporter.result("PS2 binutils available", False,
                        "not found; run in the Containerfile image")
        return None, None

    elf, sections_spec, pt_load = load_target(version)
    text_addr = h(next(s for s in sections_spec["sections"]
                       if s["name"] == ".text")["addr"])
    incbin_objects, _gaps = derive_tiling(elf, sections_spec, pt_load)
    _addrs, nobits = parse_linker_script((ROOT / "linker.ld").read_text())

    text_objs = select_text_objects(version, link_set)
    hybrid_count = sum(1 for _u, p in text_objs
                       if p.is_relative_to(ROOT / "build"))
    missing = [p.relative_to(ROOT).as_posix()
               for _u, p in text_objs if not p.is_file()]
    reporter.result("All .text objects present", not missing,
                    None if not missing else
                    f"{len(missing)} missing (run `ninja expected {link_set}`)"
                    f", e.g. {missing[:3]}")
    if missing:
        return None, None
    print(f"  {len(text_objs)} .text objects "
          f"({hybrid_count} hybrid from C, {len(text_objs) - hybrid_count} "
          f"expected)")

    out_dir = ROOT / "build" / version / "image"
    out_dir.mkdir(parents=True, exist_ok=True)
    write_incbin_objects(incbin_objects, elf, out_dir)

    defined, undefined = set(), set()
    try:
        for obj in incbin_objects:
            if obj["name"] == "text":
                continue
            subprocess.run([as_bin, "-EL", "-march=r5900",
                            "-o", str(out_dir / f"{obj['name']}.o"),
                            str(out_dir / f"{obj['name']}.s")],
                           check=True, capture_output=True, text=True)
        for _unit, obj in text_objs:
            d, u = nm_defined_undefined(nm_bin, obj)
            defined |= d
            undefined |= u
    except subprocess.CalledProcessError as exc:
        reporter.result("Assemble incbin sections", False,
                        (exc.stderr or "as failed").strip().splitlines()[0])
        return None, None

    external = (undefined - defined) - {"_gp"}
    defsyms, unresolved = resolve(sorted(external), load_symbol_addrs(version))
    reporter.result("Resolve external symbols", not unresolved,
                    None if not unresolved
                    else f"{len(unresolved)} unresolved, e.g. {unresolved[:5]}")
    if unresolved:
        return None, None

    script = build_link_script(text_addr, [p for _u, p in text_objs],
                               incbin_objects, nobits, defsyms)
    ld_script = out_dir / f"{link_set}.ld"
    ld_script.write_text(script)
    elf_out = out_dir / f"{link_set}.elf"
    bin_out = out_dir / f"{link_set}.bin"
    incbin_o = [str(out_dir / f"{o['name']}.o")
                for o in incbin_objects if o["name"] != "text"]
    sections = ["--only-section=.text"] + [
        f"--only-section=.{o['name']}"
        for o in incbin_objects if o["name"] != "text"]
    try:
        subprocess.run([ld_bin, "-EL", "-T", str(ld_script), "-o",
                        str(elf_out), *[str(p) for _u, p in text_objs],
                        *incbin_o],
                       check=True, capture_output=True, text=True)
        subprocess.run([objcopy_bin, "-O", "binary", *sections,
                        str(elf_out), str(bin_out)],
                       check=True, capture_output=True, text=True)
    except subprocess.CalledProcessError as exc:
        reporter.result(f"Link {link_set} image", False,
                        (exc.stderr or "ld failed").strip().splitlines()[0])
        return None, None
    reporter.result(f"Link {link_set} image", True)

    load_off, filesz = h(pt_load["offset"]), h(pt_load["filesz"])
    want = hashlib.sha256(elf[load_off:load_off + filesz]).hexdigest()
    got = hashlib.sha256(bin_out.read_bytes()).hexdigest()
    print(f"  linked  sha256 {got}")
    print(f"  retail  sha256 {want}")
    return got == want, bin_out


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--version", default="pal103")
    parser.add_argument("--set", dest="link_set", default="matching",
                        choices=LINK_SETS)
    args = parser.parse_args()

    print(f"Image link: {args.version}, set={args.link_set}\n")
    reporter = Reporter(40)
    sha_ok, bin_out = link_image(reporter, args.version, args.link_set)

    if sha_ok is not None:
        if args.link_set == "matching":
            reporter.result("Loaded image byte-identical to retail", sha_ok,
                            None if sha_ok else "SHA mismatch -- a hybrid "
                            "object diverges from retail")
        else:
            print(f"  image {'matches' if sha_ok else 'DIVERGES from'} "
                  f"retail (allowed for set=equivalent)")

    print()
    if reporter.failed:
        print(f"Image link ({args.link_set}) FAILED:")
        for detail in reporter.details:
            print(f"  - {detail}")
        # A failed gate must not leave a fresh output behind, or a stale
        # image could be mistaken for a verified one.
        if bin_out and bin_out.is_file():
            bin_out.unlink()
        return 1
    print(f"Image link ({args.link_set}) OK -> "
          f"{bin_out.relative_to(ROOT).as_posix()}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
