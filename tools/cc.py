#!/usr/bin/env python3
"""Compile hand-written C with the locked EE GCC. The only way C gets compiled.

Every tool and build mode funnels C compilation through here so three things
hold everywhere at once:

  * Profile discipline: flags come from a named profile in
    config/pal103/profiles.toml (default: the locked matching flags), never
    from ad-hoc command lines. The profiles file plus the compiler-binary
    fingerprints from toolchain.lock.json fold into a single profiles
    fingerprint (--fingerprint) so a flag or compiler change can never
    silently alter what an existing "matching" result meant.

  * Build modes: one source file serves all three build modes through
    preprocessor defines only --
      matching    (no defines)               C for matched functions,
                                             INCLUDE_ASM fallbacks active
      equivalent  (-DNON_MATCHING)           reviewed-equivalent C compiled in,
                                             fallbacks only for unfinished
      current     (-DSKIP_ASM -DNON_MATCHING) pure contributor C, no fallback
                                             bytes -- the honest objdiff/report
                                             object

  * Deterministic objects: the 32-bit compiler faults (EOVERFLOW) stat'ing
    large-inode bind-mounted files, so compilation runs in a native scratch
    directory -- but at a *fixed* path derived from the output object, not a
    random tempdir, because ee-gcc embeds the build path in the object's debug
    info. Same input, same object bytes, run after run.

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

# Preprocessor defines that select what each build mode compiles. See the
# module docstring; INCLUDE_ASM/NON_MATCHING structure arrives with the hybrid
# object work -- pure-C sources compile identically under every mode.
MODE_DEFINES = {
    "matching": [],
    "equivalent": ["-DNON_MATCHING"],
    "current": ["-DSKIP_ASM", "-DNON_MATCHING"],
}


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


def _workdir_for(out_o):
    """Fixed native scratch directory for one output object.

    Derived from the object's repo-relative path so it is stable across runs
    (deterministic embedded paths) and unique per object (parallel-safe).
    """
    try:
        rel = Path(out_o).resolve().relative_to(ROOT).as_posix()
    except ValueError:
        rel = Path(out_o).name
    token = re.sub(r"[^A-Za-z0-9_.-]", "_", rel)
    return Path(tempfile.gettempdir()) / "crashwoc-cc" / token


def compile_c(src, out_o, profile="default", mode="matching", version="pal103"):
    """Compile src to out_o with the named profile and build-mode defines."""
    src = Path(src)
    out_o = Path(out_o)
    flags = [*profile_flags(profile, version), *MODE_DEFINES[mode]]
    work = _workdir_for(out_o)
    if work.exists():
        shutil.rmtree(work)
    work.mkdir(parents=True)
    try:
        shutil.copy(src, work / src.name)
        obj = work / "out.o"
        subprocess.run([str(EEGCC), *flags, "-c",
                        "-o", str(obj), str(work / src.name)],
                       check=True, capture_output=True, text=True, cwd=work)
        out_o.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy(obj, out_o)
    finally:
        shutil.rmtree(work, ignore_errors=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("src", nargs="?", help="C source file to compile")
    parser.add_argument("-o", "--output", help="output object path")
    parser.add_argument("--profile", default="default",
                        help="compiler profile name (default: default)")
    parser.add_argument("--mode", default="matching",
                        choices=sorted(MODE_DEFINES),
                        help="build mode selecting preprocessor defines "
                             "(default: matching)")
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
        compile_c(args.src, args.output, args.profile, args.mode, args.version)
    except subprocess.CalledProcessError as exc:
        sys.stderr.write(exc.stderr or "cc.py: ee-gcc failed\n")
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
