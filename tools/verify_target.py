#!/usr/bin/env python3
"""Verify the original target files against the committed target registry.

Checks, for the selected version (default: pal103):

  1. The target ELF exists and matches the registered size, SHA-1 and SHA-256.
  2. SYSTEM.CNF exists and matches the registered size, SHA-1 and SHA-256.
  3. SYSTEM.CNF parses to the exact expected BOOT2 / VER / VMODE values.
  4. The ELF header, program headers and full section header table match
     config/<version>/sections.json byte-for-byte in every field.

The original game files are never committed; this tool is how a checkout
proves it is working against the canonical target. Exits 0 only if every
check passes.
"""
import argparse
import hashlib
import json
import struct
import sys
from pathlib import Path

LABEL_WIDTH = 28


class Reporter:
    def __init__(self):
        self.failed = False
        self.details = []

    def result(self, label, ok, detail=None):
        status = "PASS" if ok else "FAIL"
        print(f"{label + ':':<{LABEL_WIDTH}} {status}")
        if not ok:
            self.failed = True
            if detail:
                self.details.append(f"{label}: {detail}")


def file_hashes(path):
    sha1 = hashlib.sha1()
    sha256 = hashlib.sha256()
    with open(path, "rb") as f:
        while chunk := f.read(1 << 20):
            sha1.update(chunk)
            sha256.update(chunk)
    return sha1.hexdigest(), sha256.hexdigest()


def check_file(reporter, label, root, spec):
    path = root / spec["path"]
    if not path.is_file():
        reporter.result(label, False, f"missing file: {spec['path']}")
        return None

    size = path.stat().st_size
    if size != spec["size"]:
        reporter.result(label, False,
                        f"size {size} != expected {spec['size']}")
        return None

    sha1, sha256 = file_hashes(path)
    problems = []
    if sha1 != spec["sha1"].lower():
        problems.append(f"sha1 {sha1} != {spec['sha1'].lower()}")
    if sha256 != spec["sha256"].lower():
        problems.append(f"sha256 {sha256} != {spec['sha256'].lower()}")
    reporter.result(label, not problems, "; ".join(problems) or None)
    return path if not problems else None


def check_system_cnf(reporter, path, spec):
    label = "SYSTEM.CNF metadata"
    if path is None:
        reporter.result(label, False, "file check failed; cannot parse")
        return

    raw = path.read_bytes()
    try:
        text = raw.decode("ascii")
    except UnicodeDecodeError as exc:
        reporter.result(label, False, f"not ASCII: {exc}")
        return

    problems = []
    line_ending = spec.get("line_ending", "\r\n")
    if line_ending not in text:
        problems.append(f"expected line ending {line_ending!r} not found")

    values = {}
    for line in text.split(line_ending):
        if not line:
            continue
        key, sep, value = line.partition(" = ")
        if not sep:
            problems.append(f"unparseable line: {line!r}")
            continue
        values[key] = value

    expected = spec["expected_values"]
    for key, want in expected.items():
        got = values.get(key)
        if got != want:
            problems.append(f"{key}: {got!r} != {want!r}")
    for key in values:
        if key not in expected:
            problems.append(f"unexpected key: {key!r}")

    reporter.result(label, not problems, "; ".join(problems) or None)


def parse_elf(data):
    if data[:4] != b"\x7fELF":
        raise ValueError("missing ELF magic")
    if data[4] != 1 or data[5] != 1:
        raise ValueError("not ELF32 little-endian")

    (e_type, e_machine, _e_version, e_entry, e_phoff, e_shoff, e_flags,
     e_ehsize, e_phentsize, e_phnum, e_shentsize, e_shnum,
     e_shstrndx) = struct.unpack_from("<HHIIIIIHHHHHH", data, 16)

    header = {
        "class": "ELF32",
        "endianness": "little",
        "type": e_type,
        "machine": e_machine,
        "entry": e_entry,
        "phoff": e_phoff,
        "shoff": e_shoff,
        "flags": e_flags,
        "ehsize": e_ehsize,
        "phentsize": e_phentsize,
        "phnum": e_phnum,
        "shentsize": e_shentsize,
        "shnum": e_shnum,
        "shstrndx": e_shstrndx,
    }

    phdrs = []
    for i in range(e_phnum):
        p_type, p_offset, p_vaddr, p_paddr, p_filesz, p_memsz, p_flags, \
            p_align = struct.unpack_from("<IIIIIIII", data,
                                         e_phoff + i * e_phentsize)
        phdrs.append({
            "index": i, "type": p_type, "offset": p_offset,
            "vaddr": p_vaddr, "paddr": p_paddr, "filesz": p_filesz,
            "memsz": p_memsz, "flags": p_flags, "align": p_align,
        })

    raw = [struct.unpack_from("<IIIIIIIIII", data, e_shoff + i * e_shentsize)
           for i in range(e_shnum)]
    shstr_off = raw[e_shstrndx][4]

    def name(off):
        end = data.index(b"\x00", shstr_off + off)
        return data[shstr_off + off:end].decode("ascii")

    sections = []
    for i, (sh_name, sh_type, sh_flags, sh_addr, sh_offset, sh_size,
            sh_link, sh_info, sh_addralign, sh_entsize) in enumerate(raw):
        sections.append({
            "index": i, "name": name(sh_name), "type": sh_type,
            "flags": sh_flags, "addr": sh_addr, "offset": sh_offset,
            "size": sh_size, "link": sh_link, "info": sh_info,
            "addralign": sh_addralign, "entsize": sh_entsize,
        })

    return header, phdrs, sections


def as_int(value):
    if isinstance(value, str):
        return int(value, 0)
    return value


def diff_fields(kind, expected, actual, problems):
    for key, want in expected.items():
        want_n = as_int(want) if key not in ("name", "class", "endianness") \
            else want
        got = actual.get(key)
        if got != want_n:
            problems.append(f"{kind} {key}: {got!r} != {want_n!r}")


def check_elf_structure(reporter, elf_path, sections_spec):
    if elf_path is None:
        for label in ("ELF header", "Program headers", "Section table"):
            reporter.result(label, False, "target ELF check failed")
        return

    try:
        header, phdrs, sections = parse_elf(elf_path.read_bytes())
    except (ValueError, struct.error, IndexError) as exc:
        for label in ("ELF header", "Program headers", "Section table"):
            reporter.result(label, False, f"cannot parse ELF: {exc}")
        return

    problems = []
    diff_fields("header", sections_spec["elf_header"], header, problems)
    reporter.result("ELF header", not problems, "; ".join(problems) or None)

    problems = []
    expected_phdrs = sections_spec["program_headers"]
    if len(expected_phdrs) != len(phdrs):
        problems.append(f"count {len(phdrs)} != {len(expected_phdrs)}")
    else:
        for exp, got in zip(expected_phdrs, phdrs):
            diff_fields(f"phdr[{exp['index']}]", exp, got, problems)
    reporter.result("Program headers", not problems,
                    "; ".join(problems) or None)

    problems = []
    expected_sections = sections_spec["sections"]
    if len(expected_sections) != len(sections):
        problems.append(f"count {len(sections)} != {len(expected_sections)}")
    else:
        for exp, got in zip(expected_sections, sections):
            diff_fields(f"section[{exp['index']}] ({exp['name']})",
                        exp, got, problems)
    reporter.result("Section table", not problems,
                    "; ".join(problems) or None)


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--version", default="pal103",
                        help="target version to verify (default: pal103)")
    parser.add_argument("--root", type=Path,
                        default=Path(__file__).resolve().parent.parent,
                        help="repository root (default: parent of tools/)")
    args = parser.parse_args()

    config_dir = args.root / "config" / args.version
    try:
        version = json.loads((config_dir / "version.json").read_text())
        sections_spec = json.loads((config_dir / "sections.json").read_text())
    except (OSError, json.JSONDecodeError) as exc:
        print(f"cannot load target registry for {args.version}: {exc}",
              file=sys.stderr)
        return 1

    print(f"Verifying target: {version['name']} ({version['release']})")
    print()

    reporter = Reporter()
    elf_path = check_file(reporter, "Target ELF hash", args.root,
                          version["files"]["elf"])
    cnf_path = check_file(reporter, "SYSTEM.CNF hash", args.root,
                          version["files"]["system_cnf"])
    check_system_cnf(reporter, cnf_path, version["files"]["system_cnf"])
    check_elf_structure(reporter, elf_path, sections_spec)

    print()
    if reporter.failed:
        print("Target verification FAILED:")
        for detail in reporter.details:
            print(f"  - {detail}")
        return 1
    print("Target verification passed.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
