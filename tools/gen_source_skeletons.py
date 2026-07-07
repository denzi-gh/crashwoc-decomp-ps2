#!/usr/bin/env python3
"""Generate the browsable skeleton source tree: one comment-only C file and
one all-`asm` status manifest per .text translation unit.

Contributors should be able to open the repo, see every unit of the game
under its canonical path, and start writing C for one without first guessing
`src/<group>/<stem>.c` or hand-authoring a manifest. This tool materialises
that tree deterministically from the committed registries:

  * `src/<group>/<stem>.c` -- a COMMENT-ONLY skeleton listing the unit's
    functions (address + mdebug name). No guessed signatures or empty bodies:
    the registry has no trustworthy types, and a fake `void f(void){}` would
    invent misleading fuzzy progress. A comment-only file compiles to zero
    symbols, so objdiff honestly reports 0% until real C is written.
  * `config/<version>/status/<group>/<stem>.toml` -- the all-`asm` manifest,
    byte-identical to `promote.py --init` (shared `manifest_text`): full
    function list, every function `state = "asm"`, `complete = false`.

Existing files are never overwritten, so hand-written sources and worked-on
manifests (any function past `asm`) are preserved. Headers are NOT generated
-- real declarations and shared types stay a manual, understood act.

  python tools/gen_source_skeletons.py            # write missing files
  python tools/gen_source_skeletons.py --check     # verify committed tree

`--check` proves the committed tree is complete and that every untouched
(all-`asm`) manifest is exactly what this tool would emit -- i.e. matches the
registries. Manifests with any non-`asm` state are treated as worked-on and
exempted (their registry consistency is enforced by tools/status.py). C files
are checked for existence only: they are the work surface and legitimately
diverge from the skeleton the moment someone starts.

Pure Python; no toolchain or game files needed (runs in validate.yml).
"""
import argparse
import sys
import tomllib
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from gen_objdiff import unit_table
from promote import default_profile_for, manifest_text, unit_functions


def skeleton_c(name, funcs):
    """The comment-only C skeleton body for a unit (trailing newline)."""
    lines = ["/*", f" * Unit: {name}", " *", " * Functions:"]
    for addr, fname in funcs:
        lines.append(f" *   0x{addr:08x} {fname}")
    lines += [" */", ""]
    return "\n".join(lines)


def plan(version):
    """[(name, src_rel, manifest_rel, c_text, manifest_body)] for every unit."""
    out = []
    for unit_index, _stem, name, category in unit_table(version):
        src_rel = f"src/{name}.c"
        manifest_rel = f"config/{version}/status/{name}.toml"
        funcs = unit_functions(version, unit_index)
        profile = default_profile_for(category)
        out.append((
            name, src_rel, manifest_rel,
            skeleton_c(name, funcs),
            manifest_text(version, unit_index, name, src_rel, profile, funcs),
        ))
    return out


def _is_all_asm(manifest_path):
    data = tomllib.loads(manifest_path.read_text())
    return all(f.get("state", "asm") == "asm"
               for f in data.get("function", ()))


def generate(version):
    """Write every missing skeleton .c and manifest; never overwrite. Returns
    (written_c, written_manifests)."""
    wrote_c = wrote_m = 0
    for _name, src_rel, manifest_rel, c_text, manifest_body in plan(version):
        src = ROOT / src_rel
        manifest = ROOT / manifest_rel
        if not src.exists():
            src.parent.mkdir(parents=True, exist_ok=True)
            src.write_text(c_text)
            wrote_c += 1
        if not manifest.exists():
            manifest.parent.mkdir(parents=True, exist_ok=True)
            manifest.write_text(manifest_body)
            wrote_m += 1
    return wrote_c, wrote_m


def check(version):
    """Return a list of problem strings (empty == the committed tree is good)."""
    problems = []
    for name, src_rel, manifest_rel, _c_text, manifest_body in plan(version):
        src = ROOT / src_rel
        manifest = ROOT / manifest_rel
        if not src.exists():
            problems.append(f"missing source {src_rel} (run without --check)")
        if not manifest.exists():
            problems.append(f"missing manifest {manifest_rel} "
                            f"(run without --check)")
            continue
        # Only untouched (all-`asm`) manifests must reproduce exactly; a
        # worked-on manifest is exempt (status.py checks its registry consistency).
        if _is_all_asm(manifest) and \
                manifest.read_text() != manifest_body:
            problems.append(f"stale manifest {manifest_rel}: does not match a "
                            f"fresh generation (registry drift?)")
    return problems


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--version", default="pal103")
    parser.add_argument("--check", action="store_true",
                        help="verify the committed tree instead of writing")
    args = parser.parse_args()

    if args.check:
        problems = check(args.version)
        if problems:
            print(f"gen_source_skeletons --check: {len(problems)} problem(s):",
                  file=sys.stderr)
            for p in problems:
                print(f"  {p}", file=sys.stderr)
            return 1
        print("gen_source_skeletons --check: committed tree is up to date.")
        return 0

    wrote_c, wrote_m = generate(args.version)
    print(f"wrote {wrote_c} skeleton .c file(s) and {wrote_m} manifest(s) "
          f"(existing files left untouched).")
    if wrote_c or wrote_m:
        print("Regenerate the build config:\n    python tools/gen_ninja.py")
    return 0


if __name__ == "__main__":
    sys.exit(main())
