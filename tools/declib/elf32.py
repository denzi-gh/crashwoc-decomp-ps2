"""Minimal little-endian ELF32 reader for linker outputs and objects.

tools/verify_target.py has its own parser, but it is retail-shaped: it
indexes the section-header string table unconditionally and so cannot read
an ELF without section headers, and it returns spec-comparison dicts rather
than byte offsets. This reader serves the reconstruction tools instead:
section sizes of assembled objects (gen_shift_data), loaded-image extraction
from a linked ELF (link_shift), and header fields for packaging
(package_elf). Read-only; never writes.
"""
import struct

SHT_PROGBITS = 1
SHT_SYMTAB = 2
SHT_NOBITS = 8
SHF_ALLOC = 0x2
SHN_UNDEF = 0
STB_LOCAL = 0


class Elf32Error(Exception):
    pass


def read_elf32(data):
    """Parse bytes -> {"ehdr": {...}, "sections": [{...}]}.

    ehdr carries type/machine/entry/phoff/shoff/flags/phnum/shnum. Sections
    carry name/type/flags/addr/offset/size/addralign. An ELF without section
    headers (shnum == 0) yields an empty section list.
    """
    if data[:4] != b"\x7fELF":
        raise Elf32Error("not an ELF")
    if data[4] != 1 or data[5] != 1:
        raise Elf32Error("not little-endian ELF32")
    (e_type, e_machine, _ver, e_entry, e_phoff, e_shoff, e_flags, _ehsize,
     _phentsize, e_phnum, e_shentsize, e_shnum, e_shstrndx) = struct.unpack_from(
        "<HHIIIIIHHHHHH", data, 0x10)
    sections = []
    if e_shnum:
        raw = []
        for i in range(e_shnum):
            (sh_name, sh_type, sh_flags, sh_addr, sh_offset, sh_size, _link,
             _info, sh_addralign, _entsize) = struct.unpack_from(
                "<10I", data, e_shoff + i * e_shentsize)
            raw.append((sh_name, sh_type, sh_flags, sh_addr, sh_offset,
                        sh_size, sh_addralign))
        str_off = raw[e_shstrndx][4]
        for (sh_name, sh_type, sh_flags, sh_addr, sh_offset, sh_size,
             sh_addralign) in raw:
            name_off = str_off + sh_name
            name = data[name_off:data.index(b"\x00", name_off)].decode()
            sections.append({"name": name, "type": sh_type, "flags": sh_flags,
                             "addr": sh_addr, "offset": sh_offset,
                             "size": sh_size, "addralign": sh_addralign})
    return {"ehdr": {"type": e_type, "machine": e_machine, "entry": e_entry,
                     "phoff": e_phoff, "shoff": e_shoff, "flags": e_flags,
                     "phnum": e_phnum, "shnum": e_shnum},
            "sections": sections}


def section_sizes(data):
    """{section name: size} for every section in an object or executable."""
    return {s["name"]: s["size"] for s in read_elf32(data)["sections"]}


def symbol_names(data):
    """(global_defined, undefined) symbol name sets from an object's symtab.

    Read from the symbol table directly, NOT via nm: nm hides `.L`-prefixed
    symbols as debugger-local even when they are real undefined externals
    (the jump-table label references in splat's rodata disassembly).
    Locally-bound definitions are excluded from `global_defined` -- a local
    can never satisfy another object's reference.
    """
    if data[:4] != b"\x7fELF":
        raise Elf32Error("not an ELF")
    e_shoff, = struct.unpack_from("<I", data, 0x20)
    e_shentsize, e_shnum, e_shstrndx = struct.unpack_from("<HHH", data, 0x2E)
    raw = [struct.unpack_from("<10I", data, e_shoff + i * e_shentsize)
           for i in range(e_shnum)]
    defined, undefined = set(), set()
    for sh in raw:
        sh_type, sh_offset, sh_size, sh_link, sh_entsize = \
            sh[1], sh[4], sh[5], sh[6], sh[9]
        if sh_type != SHT_SYMTAB:
            continue
        str_off = raw[sh_link][4]
        for i in range(sh_size // (sh_entsize or 16)):
            st_name, _val, _size, st_info, _other, st_shndx = \
                struct.unpack_from("<IIIBBH", data, sh_offset + i * 16)
            if not st_name:
                continue
            end = data.index(b"\x00", str_off + st_name)
            name = data[str_off + st_name:end].decode()
            if st_shndx == SHN_UNDEF:
                undefined.add(name)
            elif (st_info >> 4) != STB_LOCAL:
                defined.add(name)
    return defined, undefined


def loaded_image(data):
    """(base_vaddr, image_bytes, memsz) of a linked ELF's allocated sections.

    The image is the PROGBITS bytes placed at (addr - base), gaps zero-filled
    -- the same result as `objcopy -O binary` over the allocated sections.
    memsz extends the span over NOBITS (bss) sections, matching how the one
    PT_LOAD segment of the retail ELF describes memory past the file image.
    """
    parsed = read_elf32(data)
    progbits = [s for s in parsed["sections"]
                if s["type"] == SHT_PROGBITS and s["flags"] & SHF_ALLOC
                and s["size"]]
    nobits = [s for s in parsed["sections"]
              if s["type"] == SHT_NOBITS and s["flags"] & SHF_ALLOC
              and s["size"]]
    if not progbits:
        raise Elf32Error("no allocated PROGBITS sections")
    base = min(s["addr"] for s in progbits)
    end = max(s["addr"] + s["size"] for s in progbits)
    image = bytearray(end - base)
    for s in progbits:
        image[s["addr"] - base:s["addr"] - base + s["size"]] = \
            data[s["offset"]:s["offset"] + s["size"]]
    mem_end = max([end] + [s["addr"] + s["size"] for s in nobits])
    return base, bytes(image), mem_end - base
