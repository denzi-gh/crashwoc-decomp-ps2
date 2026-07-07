#!/usr/bin/env python3
"""Fingerprint and verify an installed matching compiler.

Codegen identity is what matters for matching: two "EE GCC 2.9-ee-991111-01"
installs can differ in ways that silently change output. This tool records a
manifest of every file in the compiler's install directory (path -> SHA-256)
into toolchain.lock.json, and later verifies an install against that manifest.

Usage:
  python tools/fingerprint_compiler.py                 # verify against the lock
  python tools/fingerprint_compiler.py --record        # write the manifest
  python tools/fingerprint_compiler.py --component ps2-binutils [--record]
  python tools/fingerprint_compiler.py --all           # verify every component

`--all` verifies every lock component that carries an install_dir (the matching
compilers ee-gcc-tt and ee-gcc, the PE runner wibo, ps2-binutils and objdiff),
so a component the CI would otherwise forget cannot go unchecked. Components
without an install_dir (python, splat) are analysis dependencies whose versions
are gated by `python configure.py --strict`, not by file fingerprints.

Verify mode exits 0 only if every recorded file is present and unchanged and no
unexpected files were added. With an empty manifest, verify reports that no
fingerprints are recorded yet and exits non-zero, so a fresh checkout cannot
mistake "unverified" for "verified". Stdlib only; runs on Python 3.11+.
"""
import argparse
import hashlib
import json
import sys
from pathlib import Path

CHUNK = 1 << 20
LABEL_WIDTH = 40


def sha256_of(path):
    h = hashlib.sha256()
    with open(path, "rb") as f:
        while chunk := f.read(CHUNK):
            h.update(chunk)
    return h.hexdigest()


def manifest_of(install_dir):
    """Map POSIX-style relative path -> sha256 for every regular file."""
    result = {}
    for path in sorted(install_dir.rglob("*")):
        if path.is_file() and not path.is_symlink():
            rel = path.relative_to(install_dir).as_posix()
            result[rel] = sha256_of(path)
    return result


def do_record(comp, install_dir):
    manifest = manifest_of(install_dir)
    if not manifest:
        print(f"no files found under {install_dir}", file=sys.stderr)
        return None
    comp["fingerprints"] = manifest
    print(f"recorded {len(manifest)} file fingerprint(s) from {install_dir}")
    return manifest


def do_verify(name, comp, install_dir):
    recorded = comp.get("fingerprints") or {}
    if not recorded:
        print(f"{name}: no fingerprints recorded in the lock; "
              f"run with --record on a trusted install first", file=sys.stderr)
        return False

    present = manifest_of(install_dir)
    failures = []

    for rel, want in recorded.items():
        got = present.get(rel)
        if got is None:
            failures.append(f"missing: {rel}")
        elif got != want:
            failures.append(f"changed: {rel}")

    for rel in present:
        if rel not in recorded:
            failures.append(f"unexpected: {rel}")

    checked = len(recorded)
    label = f"{name} ({checked} files)"
    status = "PASS" if not failures else "FAIL"
    print(f"{label + ':':<{LABEL_WIDTH}} {status}")
    for f in failures:
        print(f"  - {f}")
    return not failures


def verifiable_components(lock):
    """Lock component names that fingerprint verification applies to.

    Every component with an install_dir is a shipped binary whose codegen
    identity must be pinned file-by-file; components without one (python,
    splat) are version-gated elsewhere. `--all` and the CI iterate exactly
    this set, so adding a fingerprinted component to the lock enrolls it in
    verification automatically -- nothing has to be listed twice.
    """
    return [name for name, comp in lock["components"].items()
            if comp.get("install_dir")]


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--lock", type=Path, default=None)
    parser.add_argument("--root", type=Path, default=None)
    parser.add_argument("--component", default=None,
                        help="component to fingerprint (default: ee-gcc)")
    parser.add_argument("--all", action="store_true",
                        help="verify every component with an install_dir "
                             "(cannot be combined with --component/--record)")
    parser.add_argument("--record", action="store_true",
                        help="write the manifest into the lock instead of "
                             "verifying against it")
    args = parser.parse_args()

    if args.all and (args.component or args.record):
        print("--all cannot be combined with --component or --record",
              file=sys.stderr)
        return 2

    repo_root = args.root or Path(__file__).resolve().parent.parent
    lock_path = args.lock or (repo_root / "toolchain.lock.json")
    try:
        lock = json.loads(lock_path.read_text())
    except (OSError, json.JSONDecodeError) as exc:
        print(f"cannot load {lock_path}: {exc}", file=sys.stderr)
        return 1

    install_root = repo_root / lock.get("install_root", "compiler")

    def verify_one(name):
        comp = lock["components"].get(name)
        if comp is None:
            print(f"unknown component: {name}", file=sys.stderr)
            return None
        if "install_dir" not in comp:
            print(f"{name} has no install_dir; nothing to fingerprint",
                  file=sys.stderr)
            return None
        install_dir = install_root / comp["install_dir"]
        if not install_dir.is_dir():
            print(f"{name}: not installed at {install_dir}; "
                  f"run tools/setup_toolchain.py first", file=sys.stderr)
            return None
        return do_verify(name, comp, install_dir)

    if args.all:
        names = verifiable_components(lock)
        if not names:
            print("no components with an install_dir to verify",
                  file=sys.stderr)
            return 1
        ok = True
        for name in names:
            result = verify_one(name)
            ok = bool(result) and ok
        return 0 if ok else 1

    name = args.component or "ee-gcc"
    comp = lock["components"].get(name)
    if comp is None:
        print(f"unknown component: {name}", file=sys.stderr)
        return 1
    if "install_dir" not in comp:
        print(f"{name} has no install_dir; nothing to fingerprint",
              file=sys.stderr)
        return 1

    install_dir = install_root / comp["install_dir"]
    if not install_dir.is_dir():
        print(f"{name}: not installed at {install_dir}; "
              f"run tools/setup_toolchain.py first", file=sys.stderr)
        return 1

    if args.record:
        if do_record(comp, install_dir) is None:
            return 1
        lock_path.write_text(json.dumps(lock, indent=4) + "\n")
        print(f"updated {lock_path.name}")
        return 0

    return 0 if do_verify(name, comp, install_dir) else 1


if __name__ == "__main__":
    sys.exit(main())
