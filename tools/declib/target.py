"""Target ELF loading and loaded-image tiling.

Extracted verbatim from build_baseline.py: the version registry loader, the
PT_LOAD-to-incbin tiling (with explicit orphan runs so no loaded byte is ever
invented or dropped), and the linker.ld layout parser.
"""
import json

from .toolchain import ROOT, h


def load_target(version):
    """Return (elf_bytes, sections_spec, pt_load) for the version."""
    config_dir = ROOT / "config" / version
    sections_spec = json.loads((config_dir / "sections.json").read_text())
    version_meta = json.loads((config_dir / "version.json").read_text())
    elf_path = ROOT / version_meta["files"]["elf"]["path"]
    elf = elf_path.read_bytes()
    pt_load = sections_spec["program_headers"][0]
    return elf, sections_spec, pt_load


def derive_tiling(elf, sections_spec, pt_load):
    """Tile the loaded file image into ordered incbin objects.

    Every loaded PROGBITS section that carries file bytes becomes an object.
    Any run of bytes between them that no section covers is checked: an all-zero
    run is left to the linker (zero fill between explicit addresses); a run with
    any non-zero byte becomes an explicit `orphan` object so no loaded byte is
    ever invented or dropped. Returns (objects, gaps) where each object is
    {name, vaddr, offset, size} and gaps is a list of (offset, size) zero runs.
    """
    load_off = h(pt_load["offset"])
    load_vaddr = h(pt_load["vaddr"])
    filesz = h(pt_load["filesz"])
    load_end = load_off + filesz

    # Loaded PROGBITS sections with file content, inside the PT_LOAD file range.
    loaded = []
    for s in sections_spec["sections"]:
        if h(s["type"]) != 1 or h(s["size"]) == 0:
            continue
        off = h(s["offset"])
        if load_off <= off < load_end:
            loaded.append(s)
    loaded.sort(key=lambda s: h(s["offset"]))

    objects = []
    gaps = []
    orphan_index = 0
    cursor = load_off
    for s in loaded:
        off = h(s["offset"])
        if off > cursor:  # bytes between the previous object and this section
            run = elf[cursor:off]
            if any(run):
                name = "orphan" if orphan_index == 0 else f"orphan{orphan_index}"
                orphan_index += 1
                objects.append({"name": name, "vaddr": load_vaddr + (cursor - load_off),
                                "offset": cursor, "size": off - cursor})
            else:
                gaps.append((cursor, off - cursor))
        objects.append({"name": s["name"].lstrip("."), "vaddr": h(s["addr"]),
                        "offset": off, "size": h(s["size"])})
        cursor = off + h(s["size"])

    if cursor < load_end:  # trailing bytes before filesz end
        run = elf[cursor:load_end]
        if any(run):
            objects.append({"name": f"orphan{orphan_index}",
                            "vaddr": load_vaddr + (cursor - load_off),
                            "offset": cursor, "size": load_end - cursor})
        else:
            gaps.append((cursor, load_end - cursor))
    return objects, gaps


def parse_linker_script(text):
    """Extract {section_name: vaddr} and NOLOAD sizes from linker.ld.

    Understands only the constructs this project's script uses: `. = ADDR;`
    setting the location counter immediately before a `.name : { ... }` output
    section, and `.name (NOLOAD) : { . += SIZE; }` for bss. Enough to validate
    that the committed script agrees with the tiling derived from the target.
    """
    import re
    addrs, nobits = {}, {}
    pending = None
    set_re = re.compile(r"\.\s*=\s*(0x[0-9A-Fa-f]+)\s*;")
    sec_re = re.compile(r"(\.[A-Za-z0-9_.]+)\s*(\(NOLOAD\))?\s*:")
    inc_re = re.compile(r"\.\s*\+=\s*(0x[0-9A-Fa-f]+)\s*;")
    for line in text.splitlines():
        line = line.split("/*")[0]
        m = set_re.search(line)
        if m:
            pending = int(m.group(1), 0)
        m = sec_re.search(line)
        if m:
            name = m.group(1).lstrip(".")
            if pending is not None:
                addrs[name] = pending
            if m.group(2):  # NOLOAD
                size_m = inc_re.search(line)
                if size_m:
                    nobits[name] = int(size_m.group(1), 0)
            pending = None
    return addrs, nobits
