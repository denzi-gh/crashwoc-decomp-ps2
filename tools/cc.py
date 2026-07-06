#!/usr/bin/env python3
"""Compile hand-written C with the locked EE GCC. The only way C gets compiled.

Every tool funnels compilation through here so two things hold everywhere:

  * Profile discipline: flags come from a named profile in
    config/pal103/profiles.toml (default: the locked matching flags), never
    from ad-hoc command lines. The profiles file plus the compiler-binary
    fingerprints from toolchain.lock.json fold into a single profiles
    fingerprint (--fingerprint) so a flag or compiler change can never
    silently alter what an existing "matching" result meant.

  * Deterministic objects: the 32-bit compiler faults (EOVERFLOW) stat'ing
    large-inode bind-mounted files, so compilation runs in a native scratch
    directory -- but at a *fixed* path derived from the output, not a random
    tempdir, because ee-gcc embeds the build path in the object's debug info.
    Same input, same object bytes, run after run.

Sources contain only hand-written C -- no fallback annotations. The hybrid
objects the canonical matching build links are produced separately by
tools/gen_hybrid.py, which uses this module's `compile_s` (C -> assembly) and
`assemble_s` (assembly -> object, through the same ee-gcc driver so Sony's
own `as` and flags are used).

Needs the EE GCC (32-bit Linux binary): run in the Containerfile image.

Exit status: 0 on success; 1 with the compiler's stderr on a failed compile.
"""
import argparse
import hashlib
import json
import re
import shutil
import subprocess
import sys
import tempfile
import tomllib
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from declib.toolchain import EEGCC


def load_profiles(version="pal103"):
    """{profile_name: [flags]} from the committed profile registry."""
    path = ROOT / "config" / version / "profiles.toml"
    data = tomllib.loads(path.read_text())
    return {name: spec["flags"] for name, spec in data["profile"].items()}


def profile_flags(profile, version="pal103"):
    profiles = load_profiles(version)
    if profile not in profiles:
        raise SystemExit(f"cc.py: unknown profile '{profile}' "
                         f"(config/{version}/profiles.toml has: "
                         f"{', '.join(sorted(profiles))})")
    return profiles[profile]


def profiles_fingerprint(version="pal103"):
    """SHA-256 over the parsed profile registry and the locked compiler binaries.

    Hashes canonicalized content (not raw file bytes) so line endings cannot
    change the fingerprint; any flag change or compiler-binary change does.
    """
    profiles = load_profiles(version)
    lock = json.loads((ROOT / "toolchain.lock.json").read_text())
    compiler = lock["components"]["ee-gcc"]["fingerprints"]
    payload = (json.dumps(profiles, sort_keys=True) + "\n"
               + json.dumps(compiler, sort_keys=True) + "\n")
    return hashlib.sha256(payload.encode()).hexdigest()


def _workdir_for(out_path):
    """Fixed native scratch directory for one output file.

    Derived from the output's repo-relative path so it is stable across runs
    (deterministic embedded paths) and unique per output (parallel-safe).
    """
    try:
        rel = Path(out_path).resolve().relative_to(ROOT).as_posix()
    except ValueError:
        rel = Path(out_path).name
    token = re.sub(r"[^A-Za-z0-9_.-]", "_", rel)
    return Path(tempfile.gettempdir()) / "crashwoc-cc" / token


def _stage_headers(work):
    """Committed headers the source may #include, staged next to it."""
    include_dir = ROOT / "include"
    if include_dir.is_dir():
        for header in include_dir.glob("*.h"):
            shutil.copy(header, work / header.name)


def _run_driver(src, out_name, extra_args, out_path, profile, version):
    """Stage src into the scratch dir, run the ee-gcc driver, copy the output."""
    src = Path(src)
    out_path = Path(out_path)
    flags = profile_flags(profile, version)
    work = _workdir_for(out_path)
    if work.exists():
        shutil.rmtree(work)
    work.mkdir(parents=True)
    try:
        shutil.copy(src, work / src.name)
        _stage_headers(work)
        produced = work / out_name
        subprocess.run([str(EEGCC), *flags, *extra_args,
                        "-o", str(produced), str(work / src.name)],
                       check=True, capture_output=True, text=True, cwd=work)
        out_path.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy(produced, out_path)
    finally:
        shutil.rmtree(work, ignore_errors=True)


def compile_c(src, out_o, profile="default", version="pal103"):
    """Compile a C source to an object."""
    _run_driver(src, "out.o", ["-c"], out_o, profile, version)


def compile_s(src, out_s, profile="default", version="pal103"):
    """Compile a C source to the assembly ee-gcc would feed its assembler."""
    _run_driver(src, "out.s", ["-S"], out_s, profile, version)


def assemble_s(src_s, out_o, profile="default", version="pal103"):
    """Assemble a .s through the ee-gcc driver -- Sony's own as, same flags."""
    _run_driver(src_s, "out.o", ["-c"], out_o, profile, version)


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("src", nargs="?", help="C (or .s) source file")
    parser.add_argument("-o", "--output", help="output object path")
    parser.add_argument("--profile", default="default",
                        help="compiler profile name (default: default)")
    parser.add_argument("--version", default="pal103")
    parser.add_argument("--fingerprint", action="store_true",
                        help="print the profiles fingerprint and exit")
    args = parser.parse_args()

    if args.fingerprint:
        print(profiles_fingerprint(args.version))
        return 0

    if not args.src or not args.output:
        parser.error("src and -o are required unless --fingerprint")
    if not EEGCC.is_file():
        print("EE GCC not found; run in the Containerfile image "
              "(setup_toolchain.py installs it).", file=sys.stderr)
        return 2

    try:
        compile_c(args.src, args.output, args.profile, args.version)
    except subprocess.CalledProcessError as exc:
        sys.stderr.write(exc.stderr or "cc.py: ee-gcc failed\n")
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
