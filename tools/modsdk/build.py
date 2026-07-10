#!/usr/bin/env python3
"""Modding SDK driver: manifests -> bootable modded ELF.

    python tools/dispatch.py python tools/modsdk/build.py mods/hello/mod.toml

Layout: blob at the image end (0x0070698C -> 0x00706A00): header, optional
mailbox, then per mod a stub area followed by its linked C image. The sbrk
break word (__ps2_klibinfo__+0x14) is patched past the blob so the game
heap never claims it. Zero manifests -> output byte-identical to retail.

Compile/link need the container; run via tools/dispatch.py. Outputs land
in build/pal103/mods/ (gitignored).
"""
import argparse
import hashlib
import shutil
import struct
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(ROOT / "tools"))

import cc  # noqa: E402
from declib import elf32  # noqa: E402
from declib.toolchain import LD, tool_path  # noqa: E402
from modsdk import elf_patch, gen_hooks, gen_symbols  # noqa: E402
from modsdk.manifest import ManifestError, load_manifests  # noqa: E402
from modsdk.mips import HookSiteError  # noqa: E402

RETAIL = ROOT / "orig" / "pal103" / "SLES_503.86"
OUT_DIR = ROOT / "build" / "pal103" / "mods"

# The SDK's own flags: -G0 because mod code cannot share retail's $gp
# window. Not a profiles.toml profile -- its fingerprint gates
# compare_progress.
MOD_CFLAGS = ["-O2", "-G0", "-fomit-frame-pointer", "-c"]
HEADER_SIZE = 0x40
MAGIC = b"CWMS"
SDK_VERSION = 1

KLIBINFO = "__ps2_klibinfo__"
BREAK_OFFSET = 0x14  # sbrk break word, retail-initialized to the image end


class BuildError(SystemExit):
    def __init__(self, msg):
        super().__init__(f"modsdk: {msg}")


def align(value, to):
    return (value + to - 1) & ~(to - 1)


def compile_mod_c(src, out_o):
    """cc.py's staged compile (EOVERFLOW workaround) with the SDK's flags."""
    spec = cc.profile_spec("default")
    argv, driver = cc.compiler_command(spec["compiler"])
    if not Path(driver).is_file():
        raise BuildError("EE GCC not found; run via tools/dispatch.py")
    work = cc._workdir_for(out_o)
    if work.exists():
        shutil.rmtree(work)
    work.mkdir(parents=True)
    try:
        shutil.copy(src, work / src.name)
        cc._stage_headers(work)
        for header in (ROOT / "mods" / "include").glob("*.h"):
            shutil.copy(header, work / header.name)
        produced = work / "out.o"
        proc = subprocess.run([*argv, *MOD_CFLAGS, "-o", str(produced),
                               str(work / src.name)],
                              capture_output=True, text=True, cwd=work)
        if proc.returncode:
            raise BuildError(f"compile failed for {src}:\n{proc.stderr}")
        out_o.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy(produced, out_o)
    finally:
        shutil.rmtree(work, ignore_errors=True)


def link_mod(objects, base, provides, out_elf, script_path):
    ld = tool_path(LD)
    if not ld:
        raise BuildError("PS2 binutils not found; run via tools/dispatch.py")
    lines = ["OUTPUT_ARCH(mips)", "SECTIONS", "{", f"  . = 0x{base:08X};",
             "  .text : { *(.text) *(.text.*) }",
             "  .rodata : { *(.rodata) *(.rodata.*) *(.rdata) }",
             "  .data : { *(.data) *(.data.*) }",
             "  .sdata : { *(.sdata) *(.sdata.*) }",
             "  .sbss : { *(.sbss) *(.sbss.*) *(.scommon) }",
             "  .bss : { *(.bss) *(.bss.*) *(COMMON) }",
             "  /DISCARD/ : { *(.reginfo) *(.mdebug) *(.comment) *(.pdr) }",
             "}", ""]
    lines += [f"PROVIDE({name} = 0x{addr:08X});"
              for name, addr in sorted(provides.items())]
    lines.append(gen_symbols.provide_script())
    script_path.write_text("\n".join(lines))
    proc = subprocess.run([ld, "-EL", "-e", f"0x{base:x}",
                           "-T", str(script_path),
                           "-o", str(out_elf), *map(str, objects)],
                          capture_output=True, text=True)
    if proc.returncode:
        raise BuildError(f"link failed for {out_elf.name}:\n{proc.stderr}")


def build(manifest_paths, out_path):
    try:
        manifests = load_manifests(manifest_paths)
    except ManifestError as exc:
        raise BuildError(str(exc))
    if not RETAIL.is_file():
        raise BuildError(f"retail ELF not found at {RETAIL}")
    retail = RETAIL.read_bytes()
    load = [p for p in elf32.program_headers(retail) if p["type"] == 1][0]
    image_end = load["vaddr"] + load["memsz"]

    symbols = gen_symbols.symbol_table()
    if KLIBINFO not in symbols:
        raise BuildError(f"{KLIBINFO} missing from symbol_addrs.txt")
    break_vaddr = symbols[KLIBINFO][0] + BREAK_OFFSET
    break_off = elf_patch.vaddr_to_offset(load, break_vaddr, 4)
    initial_break, = struct.unpack_from("<I", retail, break_off)
    if initial_break != image_end:
        raise BuildError(
            f"sbrk break sanity check failed: retail word at "
            f"0x{break_vaddr:x} is 0x{initial_break:x}, expected image end "
            f"0x{image_end:x} -- re-verify the RAM carve before patching")

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    if not manifests:
        out_path.write_bytes(retail)
        print(f"identity build: {out_path} is byte-identical to retail")
        return

    def resolve(name, want_func, what):
        if name not in symbols:
            raise BuildError(f"{what}: unknown retail symbol {name}")
        addr, is_func = symbols[name]
        if want_func and not is_func:
            raise BuildError(f"{what}: {name} is data, not a function")
        return addr

    blob_base = align(image_end, 0x80)
    blob = bytearray()
    cursor = blob_base + HEADER_SIZE
    mailbox_addr = 0
    for m in manifests:
        if m.mailbox_size:
            mailbox_addr = cursor
            cursor += align(m.mailbox_size, 0x10)
    blob += b"\x00" * (cursor - blob_base)

    patches = {}

    def add_patch(vaddr, data, what):
        for other, odata in patches.items():
            if vaddr < other + len(odata) and other < vaddr + len(data):
                raise BuildError(f"{what}: patch at 0x{vaddr:x} overlaps "
                                 f"patch at 0x{other:x}")
        patches[vaddr] = data

    for m in manifests:
        stub_base = align(cursor, 0x10)
        plans, stub_size, provides = gen_hooks.plan_stubs(m.hooks, stub_base)
        code_base = align(stub_base + stub_size, 0x10)
        handlers = {}
        code_image, code_memsz = b"", 0
        if m.sources:
            objs = []
            for src in m.sources:
                obj = OUT_DIR / m.name / (src.stem + ".o")
                compile_mod_c(src, obj)
                objs.append(obj)
            if mailbox_addr:
                provides = {**provides, "modsdk_mailbox": mailbox_addr}
            mod_elf = OUT_DIR / m.name / f"{m.name}.elf"
            link_mod(objs, code_base, provides, mod_elf,
                     OUT_DIR / m.name / f"{m.name}.ld")
            linked = mod_elf.read_bytes()
            lbase, code_image, code_memsz = elf32.loaded_image(linked)
            if lbase != code_base:
                raise BuildError(f"{m.name}: linked at 0x{lbase:x}, "
                                 f"expected 0x{code_base:x}")
            handlers = elf32.symbol_values(linked)

        stub_bytes = bytearray()
        for hook, plan in zip(m.hooks, plans):
            target = resolve(hook.function, True, f"{m.name} hook")
            if hook.handler not in handlers:
                raise BuildError(f"{m.name}: handler {hook.handler} not "
                                 f"defined (must be a global function)")
            handler_addr = handlers[hook.handler]
            toff = elf_patch.vaddr_to_offset(load, target, 8)
            displaced = list(struct.unpack_from("<2I", retail, toff))
            try:
                stub_bytes += gen_hooks.emit_stub(plan, target, displaced,
                                                  handler_addr)
                patch = gen_hooks.emit_patch(plan, target, displaced,
                                             handler_addr)
            except HookSiteError as exc:
                raise BuildError(str(exc))
            add_patch(target, patch, f"{m.name} hook {hook.function}")

        for i, p in enumerate(m.data_patches):
            base_addr = (p.address if p.address is not None
                         else resolve(p.symbol, False,
                                      f"{m.name} data_patch"))
            add_patch(base_addr + p.offset, p.data,
                      f"{m.name} data_patch #{i + 1}")

        blob += b"\x00" * (stub_base - (blob_base + len(blob)))
        blob += stub_bytes
        blob += b"\x00" * (code_base - (blob_base + len(blob)))
        blob += code_image
        blob += b"\x00" * (code_memsz - len(code_image))  # bss as zeros
        cursor = code_base + code_memsz
        print(f"  {m.name}: {len(m.hooks)} hook(s), stubs at 0x{stub_base:x},"
              f" code at 0x{code_base:x}+0x{code_memsz:x}")

    blob_end = blob_base + len(blob)
    new_break = align(blob_end, 0x10)
    add_patch(break_vaddr, struct.pack("<I", new_break), "sdk heap carve")
    struct.pack_into("<4s5I", blob, 0, MAGIC, SDK_VERSION, blob_base,
                     blob_end, mailbox_addr, len(manifests))

    out = elf_patch.build_patched_elf(retail, patches, blob_base, bytes(blob))
    problems = elf_patch.verify_output(out, retail, patches)
    if problems:
        raise BuildError("; ".join(problems))
    out_path.write_bytes(out)
    print(f"wrote {out_path} ({len(out)} bytes)")
    print(f"  blob 0x{blob_base:x}..0x{blob_end:x} "
          f"({len(blob)} bytes), heap now starts at 0x{new_break:x}")
    if mailbox_addr:
        print(f"  mailbox at 0x{mailbox_addr:x}")
    print(f"  retail segment sha256 "
          f"{hashlib.sha256(out[load['offset']:load['offset'] + load['filesz']]).hexdigest()[:16]}... "
          f"(differs from retail only at {len(patches)} declared patch(es))")


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("manifests", nargs="*",
                        help="mod.toml paths, build order (none = identity)")
    parser.add_argument("-o", "--output",
                        help=f"output ELF (default {OUT_DIR}/mod.elf)")
    args = parser.parse_args()
    out = Path(args.output) if args.output else OUT_DIR / "mod.elf"
    out.parent.mkdir(parents=True, exist_ok=True)
    build(args.manifests, out)


if __name__ == "__main__":
    main()
