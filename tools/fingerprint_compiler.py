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


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--lock", type=Path, default=None)
    parser.add_argument("--root", type=Path, default=None)
    parser.add_argument("--component", default="ee-gcc",
                        help="component to fingerprint (default: ee-gcc)")
    parser.add_argument("--record", action="store_true",
                        help="write the manifest into the lock instead of "
                             "verifying against it")
    args = parser.parse_args()

    repo_root = args.root or Path(__file__).resolve().parent.parent
    lock_path = args.lock or (repo_root / "toolchain.lock.json")
    try:
        lock = json.loads(lock_path.read_text())
    except (OSError, json.JSONDecodeError) as exc:
        print(f"cannot load {lock_path}: {exc}", file=sys.stderr)
        return 1

    comp = lock["components"].get(args.component)
    if comp is None:
        print(f"unknown component: {args.component}", file=sys.stderr)
        return 1
    if "install_dir" not in comp:
        print(f"{args.component} has no install_dir; nothing to fingerprint",
              file=sys.stderr)
        return 1

    install_dir = repo_root / lock.get("install_root", "compiler") \
        / comp["install_dir"]
    if not install_dir.is_dir():
        print(f"{args.component}: not installed at {install_dir}; "
              f"run tools/setup_toolchain.py first", file=sys.stderr)
        return 1

    if args.record:
        if do_record(comp, install_dir) is None:
            return 1
        lock_path.write_text(json.dumps(lock, indent=4) + "\n")
        print(f"updated {lock_path.name}")
        return 0

    return 0 if do_verify(args.component, comp, install_dir) else 1


if __name__ == "__main__":
    sys.exit(main())
