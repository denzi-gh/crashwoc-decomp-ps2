#!/usr/bin/env python3
"""Install and verify the locked toolchain described by toolchain.lock.json.

For every component that ships an immutable ``artifact`` (the matching compiler
and the reconstruction binutils) this tool:

  1. Locates the archive: an already-downloaded copy in the download cache, or
     downloads it from the locked URL.
  2. Verifies the archive's SHA-256 against the lock. A mismatch is fatal and
     the bad file is removed.
  3. Extracts it into the install root.

A component whose locked ``sha256`` is null is *not* installed: there is no
trusted hash to check it against. Run with ``--record`` and a locally supplied
archive to compute and write that hash into the lock the first time, on an
archive you trust; every install afterwards is then verified against it.

Nothing here is a match anchor by itself -- this only reproduces the exact
tools. The archives are user-supplied and never committed (the install root and
download cache are gitignored). Stdlib only; runs on Python 3.11+.
"""
import argparse
import hashlib
import json
import shutil
import sys
import urllib.request
from pathlib import Path

CHUNK = 1 << 20


def sha256_of(path):
    h = hashlib.sha256()
    with open(path, "rb") as f:
        while chunk := f.read(CHUNK):
            h.update(chunk)
    return h.hexdigest()


def human(n):
    for unit in ("B", "KiB", "MiB", "GiB"):
        if n < 1024 or unit == "GiB":
            return f"{n:.0f} {unit}" if unit == "B" else f"{n:.1f} {unit}"
        n /= 1024


def download(url, dest):
    print(f"  downloading {url}")
    tmp = dest.with_suffix(dest.suffix + ".part")
    with urllib.request.urlopen(url) as resp, open(tmp, "wb") as out:
        total = 0
        while chunk := resp.read(CHUNK):
            out.write(chunk)
            total += len(chunk)
    tmp.replace(dest)
    print(f"  wrote {dest.name} ({human(total)})")


def locate_archive(comp_name, artifact, cache_dir, allow_download):
    """Return a path to the archive, downloading if needed, or None."""
    filename = artifact.get("filename")
    url = artifact.get("url")
    if not filename and url:
        filename = url.rsplit("/", 1)[-1]
    if not filename:
        print(f"  {comp_name}: no filename or URL locked; place the archive in "
              f"{cache_dir} and record its name in the lock", file=sys.stderr)
        return None

    dest = cache_dir / filename
    if dest.is_file():
        return dest
    if url and allow_download:
        cache_dir.mkdir(parents=True, exist_ok=True)
        try:
            download(url, dest)
        except OSError as exc:
            print(f"  {comp_name}: download failed: {exc}", file=sys.stderr)
            return None
        return dest
    if url:
        print(f"  {comp_name}: not cached and downloads disabled "
              f"(pass --download); expected {dest}", file=sys.stderr)
    else:
        print(f"  {comp_name}: no URL locked and no archive at {dest}; "
              f"supply it manually", file=sys.stderr)
    return None


def install_component(name, comp, root, cache_dir, args, lock_dirty):
    artifact = comp.get("artifact")
    if not artifact:
        print(f"{name}: version {comp['version']} "
              f"({comp.get('managed_by') or comp.get('pip') or comp.get('source')})"
              " -- not installed by this tool")
        return True

    install_dir = root / comp["install_dir"]
    if install_dir.exists() and any(install_dir.iterdir()) and not args.force:
        print(f"{name}: already present at {install_dir} (use --force to reinstall)")
        return True

    print(f"{name}: version {comp['version']}")
    archive = locate_archive(name, artifact, cache_dir, args.download)
    if archive is None:
        return False

    actual = sha256_of(archive)
    locked = artifact.get("sha256")

    if locked is None:
        if not args.record:
            print(f"  no sha256 locked for {name}; its hash is {actual}\n"
                  f"  re-run with --record to trust this archive and write it "
                  f"into the lock", file=sys.stderr)
            return False
        artifact["sha256"] = actual
        lock_dirty[0] = True
        print(f"  recorded sha256 {actual}")
    elif actual != locked.lower():
        print(f"  SHA-256 MISMATCH for {name}:\n    expected {locked.lower()}\n"
              f"    got      {actual}\n  removing {archive}", file=sys.stderr)
        try:
            archive.unlink()
        except OSError:
            pass
        return False
    else:
        print(f"  sha256 ok ({actual})")

    if install_dir.exists():
        shutil.rmtree(install_dir)
    install_dir.mkdir(parents=True, exist_ok=True)
    try:
        shutil.unpack_archive(str(archive), str(install_dir))
    except (shutil.ReadError, ValueError, OSError) as exc:
        print(f"  extraction failed: {exc}", file=sys.stderr)
        shutil.rmtree(install_dir, ignore_errors=True)
        return False
    print(f"  installed to {install_dir}")
    return True


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--lock", type=Path, default=None,
                        help="path to toolchain.lock.json "
                             "(default: repo root)")
    parser.add_argument("--root", type=Path, default=None,
                        help="repository root (default: parent of tools/)")
    parser.add_argument("--only", action="append", default=[],
                        metavar="NAME",
                        help="install only the named component(s)")
    parser.add_argument("--download", action="store_true",
                        help="allow downloading archives from locked URLs")
    parser.add_argument("--record", action="store_true",
                        help="trust supplied archives with a null locked hash "
                             "and write their SHA-256 back into the lock")
    parser.add_argument("--force", action="store_true",
                        help="reinstall even if the install dir already exists")
    args = parser.parse_args()

    repo_root = args.root or Path(__file__).resolve().parent.parent
    lock_path = args.lock or (repo_root / "toolchain.lock.json")
    try:
        lock = json.loads(lock_path.read_text())
    except (OSError, json.JSONDecodeError) as exc:
        print(f"cannot load {lock_path}: {exc}", file=sys.stderr)
        return 1

    install_root = repo_root / lock.get("install_root", "compiler")
    cache_dir = repo_root / lock.get("download_cache", "tools/download")
    install_root.mkdir(parents=True, exist_ok=True)

    components = lock["components"]
    names = args.only or list(components)
    unknown = [n for n in names if n not in components]
    if unknown:
        print(f"unknown component(s): {', '.join(unknown)}", file=sys.stderr)
        return 1

    lock_dirty = [False]
    ok = True
    for name in names:
        if not install_component(name, components[name], install_root,
                                 cache_dir, args, lock_dirty):
            ok = False
        print()

    if lock_dirty[0]:
        lock_path.write_text(json.dumps(lock, indent=4) + "\n")
        print(f"updated {lock_path.name} with newly recorded hashes")

    if not ok:
        print("toolchain setup incomplete (see messages above).",
              file=sys.stderr)
        return 1
    print("toolchain ready.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
