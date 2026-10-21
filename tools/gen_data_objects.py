#!/usr/bin/env python3
"""Build the expected data objects from the committed data map.

For every range in config/<version>/data_map.toml this emits an `.incbin`
slice of the retail bytes (a `.space` NOBITS section for .sbss/.bss) and
assembles it:

  expected/<version>/data/unit-NNNN.o           all ranges a unit owns, one
                                                object per unit (its objdiff
                                                data target)
  expected/<version>/data/unassigned/<sec>_<addr>.o
                                                each gap / orphan range; a
                                                gap with manifest `[[claim]]`s
                                                is cut at the claim
                                                boundaries, one object per
                                                claimed span and per hole

Every range becomes its own input section, `.split.<sec>_<vram8hex>`, so the
image link script can place the pieces back at their exact addresses in
order -- the linked data sections are retail-byte-exact by construction
(tools/link_image.py consumes these; `ninja verify-loaded` proves it). The
NOBITS sections and the pieces of claimed gaps are objdiff targets only; the
link never takes them.
Each section carries a local symbol `<sec>_<vram8hex>` sized to the range:
objdiff only diffs data bytes covered by a symbol.

Needs the PS2 binutils: run in the Containerfile image (ninja target
`data`). Everything emitted is game-derived: objects land in gitignored
expected/, intermediates in build/.

Exit status is 0 only if every range assembled.
"""
import argparse
import os
import subprocess
import sys
import tomllib
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from declib.toolchain import AS, h, tool_path
from declib.target import load_target

NOBITS_SECTIONS = {".sbss", ".bss"}


def load_map(version):
    """[{section,start,end,owner,evidence}] from the committed data map."""
    path = ROOT / "config" / version / "data_map.toml"
    if not path.is_file():
        raise SystemExit(f"{path.relative_to(ROOT)} missing; run "
                         "`python tools/extract_data_map.py` first.")
    return tomllib.loads(path.read_text())["range"]


def load_claims(version):
    """[(section, start, end, unit stem)] of every manifest `[[claim]]`."""
    out = []
    status_dir = ROOT / "config" / version / "status"
    if status_dir.is_dir():
        for m in sorted(status_dir.rglob("*.toml")):
            data = tomllib.loads(m.read_text())
            stem = data["unit"].split(":")[1]
            out += [(c["section"], c["start"], c["end"], stem)
                    for c in data.get("claim", [])]
    return sorted(out)


def split_claims(ranges, claims):
    """`ranges` with every claimed gap cut at its claim boundaries: one range
    per claimed span (`claimant` = its unit) and one per hole. Pure."""
    spans = {}
    for section, start, end, unit in claims:
        gap = next((r for r in ranges if r["section"] == section
                    and r["owner"] == "unassigned"
                    and r["start"] <= start and end <= r["end"]), None)
        if gap is None:
            raise ValueError(f"{unit}: claim {section} 0x{start:08x}-"
                             f"0x{end:08x} is not inside one data-map gap")
        spans.setdefault((section, gap["start"]), []).append(
            (start, end, unit))
    out = []
    for r in ranges:
        claimed = sorted(spans.get((r["section"], r["start"]), []))
        if not claimed:
            out.append(r)
            continue
        cursor = r["start"]
        for start, end, unit in claimed + [(r["end"], r["end"], None)]:
            if start < cursor:
                raise ValueError(f"{r['section']} claims overlap at "
                                 f"0x{start:08x}")
            if start > cursor:
                out.append({**r, "start": cursor, "end": start})
            if unit is not None:
                out.append({**r, "start": start, "end": end,
                            "claimant": unit})
            cursor = end
    return out


def range_bytes(elf, sections_spec, pt_load, entry):
    """The retail file bytes of one map range."""
    if entry["section"] == "orphan":
        # Un-sectioned loaded bytes: locate via the PT_LOAD mapping.
        off = h(pt_load["offset"]) + (entry["start"] - h(pt_load["vaddr"]))
        return elf[off: off + entry["end"] - entry["start"]]
    sec = next(s for s in sections_spec["sections"]
               if s["name"] == entry["section"])
    off = h(sec["offset"]) + (entry["start"] - h(sec["addr"]))
    return elf[off: off + entry["end"] - entry["start"]]


def section_label(entry):
    sec = entry["section"].lstrip(".")
    return f"{sec}_{entry['start']:08x}"


def range_section(label, bin_path, size):
    """`.split.<label>` section with symbol `<label>` over `bin_path`, or
    `size` NOBITS bytes when `bin_path` is None.

    Shared with gen_report_data so objdiff pairs target and base."""
    if bin_path is None:
        kind, body = '"aw", @nobits', f'\t.space\t{size}\n'
    else:
        kind, body = '"a", @progbits', f'\t.incbin "{bin_path.as_posix()}"\n'
    return (f'\t.section .split.{label}, {kind}\n'
            f'\t.type\t{label}, @object\n'
            f'\t.size\t{label}, {size}\n'
            f'{label}:\n' + body)


def object_groups(ranges):
    """[(stem, [entries])] -- one object per owning unit, one per gap (per
    gap piece after split_claims).

    Stems are relative to expected/<version>/data/ (no extension); the same
    grouping drives both object generation here and the image link
    (tools/link_image.py), so the two can never disagree on paths.
    """
    grouped = {}
    for entry in ranges:
        stem = (f"unassigned/{section_label(entry)}"
                if entry["owner"] == "unassigned" else entry["owner"])
        grouped.setdefault(stem, []).append(entry)
    return sorted(grouped.items())


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--version", default="pal103")
    args = parser.parse_args()

    as_bin = tool_path(AS)
    if not as_bin:
        print("PS2 binutils not found; run in the Containerfile image.",
              file=sys.stderr)
        return 2

    elf, sections_spec, pt_load = load_target(args.version)
    ranges = split_claims(load_map(args.version), load_claims(args.version))

    work = ROOT / "build" / args.version / "data_s"
    out_root = ROOT / "expected" / args.version / "data"
    (out_root / "unassigned").mkdir(parents=True, exist_ok=True)
    work.mkdir(parents=True, exist_ok=True)

    header = ("/* Generated by tools/gen_data_objects.py -- do not edit, do "
              "not commit (game-derived). */\n")
    jobs = []
    groups = object_groups(ranges)
    for stem, entries in groups:
        s_path = work / f"{stem.replace('/', '_')}.s"
        pieces = [header]
        for entry in sorted(entries, key=lambda e: e["start"]):
            label = section_label(entry)
            bin_path = None
            if entry["section"] not in NOBITS_SECTIONS:
                bin_path = work / f"{label}.bin"
                bin_path.write_bytes(range_bytes(elf, sections_spec, pt_load,
                                                 entry))
            pieces.append(range_section(label, bin_path,
                                        entry["end"] - entry["start"]))
        s_path.write_text("".join(pieces))
        jobs.append((stem, [as_bin, "-EL", "-march=r5900",
                            "-o", str(out_root / f"{stem}.o"), str(s_path)]))

    def assemble(job):
        stem, cmd = job
        run = subprocess.run(cmd, capture_output=True, text=True)
        return None if run.returncode == 0 else (
            f"{stem}: as failed: {run.stderr.strip().splitlines()[:2]}")

    with ThreadPoolExecutor(max_workers=os.cpu_count() or 4) as pool:
        failed = [e for e in pool.map(assemble, jobs) if e]
    for err in failed:
        print(err, file=sys.stderr)
    if failed:
        return 1

    n_gap = sum(1 for stem, _e in groups if stem.startswith("unassigned/"))
    n_claimed = sum(1 for r in ranges if "claimant" in r)
    print(f"{len(jobs)} data objects -> "
          f"{out_root.relative_to(ROOT).as_posix()}/ "
          f"({len(jobs) - n_gap} per-unit, {n_gap} unassigned; "
          f"{len(ranges)} ranges, {n_claimed} claimed, gitignored)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
