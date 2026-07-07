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

The data sections are linked from the PER-RANGE expected data objects that
tools/gen_data_objects.py builds from config/<version>/data_map.toml (one
object per owning unit, one per unassigned gap, the 0x80 orphan included);
the link script places every range back at its exact address, so the data
half of the image is retail-byte-exact by construction. Only `.vutext`
still rides as a whole-section incbin (VU microcode, no attribution yet).

Two sets, two gates:

  * --set matching   must be byte-identical to retail: the SHA-256 of the
                     linked image must equal the retail PT_LOAD bytes.
                     This is `ninja verify-loaded`.
  * --set equivalent must link (reviewed-equivalent C included); the SHA is
                     reported but not required. This is the modding build.
                     NOTE while externals resolve to fixed retail addresses
                     (absolute defsyms), modified code must not change
                     function sizes; a relocatable link is a later
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
from gen_data_objects import (NOBITS_SECTIONS, load_map, object_groups,
                              section_label)
from gen_expected_s import unit_stem
from split_text import nm_defined_undefined

LINK_SETS = ("matching", "equivalent")
_GP = 0x00634970                  # config/pal103/sections.json; linker.ld

# The loaded-image pieces this script knows how to place. If the tiling ever
# discovers anything else (a new orphan run), fail loudly instead of
# silently dropping bytes.
_TILING_NAMES = {"text", "vutext", "orphan", "data", "rodata", "lit4",
                 "sdata"}


# The function states from which each link set actually compiles C (and so
# builds a hybrid object). Mirrors gen_ninja / gen_hybrid: an all-`asm` unit
# builds no hybrid in either set, so its bytes come from the expected object.
_HYBRID_STATES = {"matching": {"matching"},
                  "equivalent": {"matching", "equivalent"}}


def manifest_units(version, link_set):
    """{unit_index: object rel path} for units that build a hybrid in this set.

    Only units with a function in a state this set links from C get a hybrid;
    the rest (e.g. all-`asm` skeletons) fall through to the expected object.
    """
    wanted = _HYBRID_STATES[link_set]
    out = {}
    status_dir = ROOT / "config" / version / "status"
    if status_dir.is_dir():
        for m in sorted(status_dir.rglob("*.toml")):
            data = tomllib.loads(m.read_text())
            states = {f.get("state", "asm")
                      for f in data.get("function", ())}
            if not states & wanted:
                continue
            idx = int(data["unit"].split("unit-")[1])
            out[idx] = Path(data["source"]).relative_to("src").with_suffix(".o")
    return out


def select_text_objects(version, link_set):
    """[(unit_index, object_path)] in retail address order.

    Hybrid object where the unit builds one for this link set, expected object
    otherwise.
    """
    hybrids = manifest_units(version, link_set)
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


def build_script(text_addr, text_objs, vutext_vaddr, map_ranges, nobits,
                 defsyms):
    """Link script: .text from TU objects, data sections from map ranges."""
    L = ["SECTIONS", "{", f"    . = 0x{text_addr:08X};",
         "    .text : SUBALIGN(1)", "    {"]
    L += [f"        {p.as_posix()}(.text)" for p in text_objs]
    L.append("    }")
    L.append(f"    . = 0x{vutext_vaddr:08X};")
    L.append("    .vutext : { *(.split.vutext) }")

    by_sec = {}
    for r in map_ranges:
        by_sec.setdefault(r["section"], []).append(r)
    for sec, rs in sorted(by_sec.items(), key=lambda kv: kv[1][0]["start"]):
        rs.sort(key=lambda r: r["start"])
        L.append(f"    . = 0x{rs[0]['start']:08X};")
        L.append(f"    .{sec.lstrip('.')} : SUBALIGN(1)")
        L.append("    {")
        L += [f"        *(.split.{section_label(r)})" for r in rs]
        L.append("    }")

    L.append("    . = 0x00633000;")
    L.append(f"    .sbss (NOLOAD) : {{ . += 0x{nobits['sbss']:06X}; }}")
    L.append("    . = 0x00633400;")
    L.append(f"    .bss  (NOLOAD) : {{ . += 0x{nobits['bss']:06X}; }}")
    L.append(f"    _gp = 0x{_GP:08X};")
    L.append("    /DISCARD/ : { *(.pdr) *(.reginfo) *(.mdebug*) *(.comment) "
             "*(.gnu.attributes) }")
    L.append("}")
    L += [f"{n} = 0x{a:08X};" for n, a in sorted(defsyms.items())]
    return "\n".join(L) + "\n"


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
    tiling, _gaps = derive_tiling(elf, sections_spec, pt_load)
    names = {o["name"] for o in tiling}
    if names != _TILING_NAMES:
        reporter.result("Tiling matches the modeled sections", False,
                        f"unexpected pieces: {sorted(names ^ _TILING_NAMES)}")
        return None, None
    vutext = next(o for o in tiling if o["name"] == "vutext")
    _addrs, nobits = parse_linker_script((ROOT / "linker.ld").read_text())

    text_objs = select_text_objects(version, link_set)
    hybrid_count = sum(1 for _u, p in text_objs
                       if p.is_relative_to(ROOT / "build"))
    map_ranges = [r for r in load_map(version)
                  if r["section"] not in NOBITS_SECTIONS]
    data_dir = ROOT / "expected" / version / "data"
    data_objs = [data_dir / f"{stem}.o"
                 for stem, _e in object_groups(map_ranges)]

    missing = [p.relative_to(ROOT).as_posix()
               for p in [p for _u, p in text_objs] + data_objs
               if not p.is_file()]
    reporter.result("All linked objects present", not missing,
                    None if not missing else
                    f"{len(missing)} missing (run `ninja expected data "
                    f"{link_set}`), e.g. {missing[:3]}")
    if missing:
        return None, None
    print(f"  {len(text_objs)} .text objects ({hybrid_count} hybrid from C, "
          f"{len(text_objs) - hybrid_count} expected)")
    print(f"  {len(data_objs)} data objects over {len(map_ranges)} map ranges")

    out_dir = ROOT / "build" / version / "image"
    out_dir.mkdir(parents=True, exist_ok=True)

    # .vutext still rides as one incbin (VU microcode, unattributed).
    vutext_bin = out_dir / "vutext.bin"
    vutext_bin.write_bytes(
        elf[vutext["offset"]:vutext["offset"] + vutext["size"]])
    (out_dir / "vutext.s").write_text(
        "/* Generated by tools/link_image.py -- do not edit, do not commit "
        "(game-derived). */\n"
        '\t.section .split.vutext, "a", @progbits\n'
        f'\t.incbin "{vutext_bin.as_posix()}"\n')

    defined, undefined = set(), set()
    try:
        subprocess.run([as_bin, "-EL", "-march=r5900",
                        "-o", str(out_dir / "vutext.o"),
                        str(out_dir / "vutext.s")],
                       check=True, capture_output=True, text=True)
        for _unit, obj in text_objs:
            d, u = nm_defined_undefined(nm_bin, obj)
            defined |= d
            undefined |= u
    except subprocess.CalledProcessError as exc:
        reporter.result("Assemble vutext incbin", False,
                        (exc.stderr or "as failed").strip().splitlines()[0])
        return None, None

    external = (undefined - defined) - {"_gp"}
    defsyms, unresolved = resolve(sorted(external), load_symbol_addrs(version))
    reporter.result("Resolve external symbols", not unresolved,
                    None if not unresolved
                    else f"{len(unresolved)} unresolved, e.g. {unresolved[:5]}")
    if unresolved:
        return None, None

    script = build_script(text_addr, [p for _u, p in text_objs],
                          vutext["vaddr"], map_ranges, nobits, defsyms)
    ld_script = out_dir / f"{link_set}.ld"
    ld_script.write_text(script)
    elf_out = out_dir / f"{link_set}.elf"
    bin_out = out_dir / f"{link_set}.bin"
    sections = ["--only-section=.text", "--only-section=.vutext"] + [
        f"--only-section=.{s.lstrip('.')}"
        for s in dict.fromkeys(r["section"] for r in map_ranges)]
    try:
        subprocess.run([ld_bin, "-EL", "-T", str(ld_script), "-o",
                        str(elf_out), *[str(p) for _u, p in text_objs],
                        str(out_dir / "vutext.o"),
                        *[str(p) for p in data_objs]],
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
