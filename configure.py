#!/usr/bin/env python3
"""Bootstrap the build: disassemble the target ELF into asm with splat.

This is the entry point for turning the user-supplied retail ELF into the
per-section assembly and linker script the project builds from. Nothing it
produces is committed -- every output is a deterministic function of the input
binary and lands in gitignored directories (asm/, build/).

Steps:
  1. Confirm the disassembler matches the locked versions (splat + backends).
  2. Confirm the target ELF is present (splat validates its sha1).
  3. Run `splat split` on splat.yaml.
  4. Regenerate build.ninja (tools/gen_ninja.py), so the incremental build
     graph always reflects the fresh split. `python tools/gen_ninja.py` alone
     is enough after changes that don't need a re-split (new src file, new
     status manifest, new header).

With --check, the split is run twice and every generated file is hashed and
compared, proving the output is byte-for-byte reproducible.

Only the Python standard library is used; splat is invoked as a subprocess so
the toolchain stays a single, version-checked dependency.
"""
import argparse
import hashlib
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent
sys.path.insert(0, str(ROOT / "tools"))
from gen_ninja import emit_ninja
from gen_objdiff import emit_objdiff

SPLAT_YAML = ROOT / "splat.yaml"
LOCK = ROOT / "toolchain.lock.json"
VERSION_JSON = ROOT / "config" / "pal103" / "version.json"

# Directories splat writes into; everything here is regenerable and gitignored.
GENERATED_DIRS = ("asm", "build")


def locked_versions():
    """(splat, spimdisasm, rabbitizer) versions from the toolchain lock."""
    lock = json.loads(LOCK.read_text())
    splat = lock["components"]["splat"]
    pin = splat["pip"].split("==")[-1]
    backends = splat.get("backends", {})
    return pin, backends.get("spimdisasm"), backends.get("rabbitizer")


def installed_version(module):
    try:
        import importlib.metadata as md
        return md.version(module)
    except Exception:
        return None


def check_toolchain(strict):
    """Compare installed disassembler versions against the lock.

    Returns True if usable. A mismatch is fatal only under --strict; otherwise
    it is a warning, because splat's output is an analysis artifact, not a
    match anchor -- but reproducing THESE files requires THESE versions.
    """
    want_splat, want_spim, want_rabbit = locked_versions()
    checks = [
        ("splat64", want_splat, installed_version("splat64")),
        ("spimdisasm", want_spim, installed_version("spimdisasm")),
        ("rabbitizer", want_rabbit, installed_version("rabbitizer")),
    ]
    ok = True
    missing = False
    for name, want, have in checks:
        if have is None:
            print(f"  {name:<12} MISSING (locked {want})")
            missing = True
            ok = False
        elif want and have != want:
            print(f"  {name:<12} {have}  != locked {want}")
            ok = False
        else:
            print(f"  {name:<12} {have}  ok")
    if missing:
        print("\nInstall the locked disassembler with:")
        print('    python -m pip install "splat64==%s"' % want_splat)
        print("    python -m pip install spimdisasm==%s rabbitizer==%s"
              % (want_spim, want_rabbit))
        return False
    if not ok and strict:
        print("\nVersion mismatch and --strict given; refusing to run.")
        return False
    if not ok:
        print("\nWarning: versions differ from the lock; output may not match "
              "the committed registries. Pass --strict to make this fatal.")
    return True


def check_target():
    """Confirm the target ELF exists. splat checks its sha1 during the split."""
    meta = json.loads(VERSION_JSON.read_text())
    elf_rel = meta["files"]["elf"]["path"]
    elf = ROOT / elf_rel
    if not elf.is_file():
        print(f"\nTarget ELF not found at {elf_rel}.")
        print("Supply your own legally obtained copy (see README) and verify "
              "it with `python tools/verify_target.py`.")
        return False
    print(f"  target       {elf_rel}  present ({elf.stat().st_size} bytes)")
    return True


def run_split(extra_args=()):
    """Invoke `splat split splat.yaml`; return True on success."""
    cmd = [sys.executable, "-m", "splat", "split", str(SPLAT_YAML), *extra_args]
    result = subprocess.run(cmd, cwd=ROOT)
    return result.returncode == 0


def hash_generated():
    """Map every generated file (relative path -> sha256) for comparison."""
    digests = {}
    for d in GENERATED_DIRS:
        base = ROOT / d
        if not base.is_dir():
            continue
        for path in sorted(base.rglob("*")):
            if path.is_file():
                rel = path.relative_to(ROOT).as_posix()
                digests[rel] = hashlib.sha256(path.read_bytes()).hexdigest()
    return digests


def determinism_check():
    """Split twice and prove every generated file is byte-identical."""
    print("First split...")
    if not run_split():
        return False
    first = hash_generated()
    print(f"  {len(first)} files generated")

    print("Second split...")
    if not run_split():
        return False
    second = hash_generated()

    if first == second:
        print(f"\nDeterministic: {len(first)} files reproduced identically.")
        return True

    print("\nNON-DETERMINISTIC output detected:")
    for rel in sorted(set(first) | set(second)):
        a, b = first.get(rel), second.get(rel)
        if a != b:
            if a is None:
                print(f"  only in 2nd run: {rel}")
            elif b is None:
                print(f"  only in 1st run: {rel}")
            else:
                print(f"  differs: {rel}")
    return False


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--check", action="store_true",
                        help="split twice and verify byte-identical output "
                             "instead of a single split")
    parser.add_argument("--strict", action="store_true",
                        help="treat a disassembler version mismatch as fatal")
    args = parser.parse_args()

    print("Toolchain:")
    if not check_toolchain(args.strict):
        return 1
    print("Target:")
    if not check_target():
        return 1

    print()
    if args.check:
        if not determinism_check():
            return 1
        emit_ninja()
        emit_objdiff()
        return 0

    print("Splitting...")
    if not run_split():
        print("\nsplit failed.")
        return 1
    emit_ninja()
    emit_objdiff()
    print("\nSplit complete. Generated asm/, build/, build.ninja and "
          "objdiff.json (all gitignored).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
