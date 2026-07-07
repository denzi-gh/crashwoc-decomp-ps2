"""Unit tests for the contributor wrapper decomp.py.

Pure parts (path resolution, report/manifest/verify merge, command
construction) are tested directly; orchestration is tested by recording the
commands issued, with subprocess replaced -- nothing is executed and no
container is needed.
"""
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT))
import decomp

CREATURE = "src/game/creature.c"


class TestCommandConstruction(unittest.TestCase):
    def test_dispatch_cmd(self):
        self.assertEqual(
            decomp.dispatch_cmd("ninja", "report"),
            [decomp.PY, "tools/dispatch.py", "ninja", "report"])

    def test_setup_pipeline_order_and_routing(self):
        cmds = decomp.setup_commands()
        # Every step routes through the dispatcher.
        for c in cmds:
            self.assertEqual(c[:2], [decomp.PY, "tools/dispatch.py"])
        tails = [c[2:] for c in cmds]
        self.assertEqual(tails, [
            ["python", "tools/verify_target.py"],
            ["python", "configure.py", "--strict"],
            ["ninja", "expected", "current", "report-current", "data"],
            ["ninja", "verify-promoted"],
            ["ninja", "report"],
            ["python", "tools/check_report_matches.py"],
        ])

    def test_setup_pipeline_is_deterministic(self):
        self.assertEqual(decomp.setup_commands(), decomp.setup_commands())


class TestResolveUnit(unittest.TestCase):
    def test_valid_unit(self):
        name, src, manifest = decomp.resolve_unit(CREATURE, "pal103")
        self.assertEqual(name, "game/creature")
        self.assertEqual(src, "src/game/creature.c")
        self.assertEqual(manifest, "config/pal103/status/game/creature.toml")

    def test_rejects_non_src_path(self):
        with self.assertRaises(decomp.DecompError):
            decomp.resolve_unit("tools/promote.py", "pal103")

    def test_rejects_non_c_path(self):
        with self.assertRaises(decomp.DecompError):
            decomp.resolve_unit("src/game/creature.h", "pal103")

    def test_rejects_unknown_unit(self):
        with self.assertRaises(decomp.DecompError):
            decomp.resolve_unit("src/game/nosuchunit.c", "pal103")

    def test_rejects_missing_source_file(self):
        # A real unit name whose src file does not exist in the tree.
        from gen_objdiff import unit_table
        name = next(r[2] for r in unit_table("pal103")
                    if not (ROOT / f"src/{r[2]}.c").is_file())
        with self.assertRaises(decomp.DecompError) as ctx:
            decomp.resolve_unit(f"src/{name}.c", "pal103")
        self.assertIn("does not exist", str(ctx.exception))

    def test_rejects_unit_without_manifest(self):
        # A known unit whose src exists but has no manifest: guide to --init.
        # Construct that state under a temp ROOT (it cannot occur in the real
        # tree, where the coverage rule pairs every src with a manifest).
        import tempfile
        from gen_objdiff import unit_table
        name = next(r[2] for r in unit_table("pal103"))
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            src = root / "src" / f"{name}.c"
            src.parent.mkdir(parents=True, exist_ok=True)
            src.write_text("// wip\n")
            orig = decomp.ROOT
            decomp.ROOT = root
            try:
                with self.assertRaises(decomp.DecompError) as ctx:
                    decomp.resolve_unit(f"src/{name}.c", "pal103")
            finally:
                decomp.ROOT = orig
        self.assertIn("promote.py --init", str(ctx.exception))


class TestReportMerge(unittest.TestCase):
    REPORT = {"units": [{
        "name": "game/creature",
        "measures": {"fuzzy_match_percent": "89.31"},
        "functions": [
            {"name": "ChangeCharacter", "fuzzy_match_percent": 100.0},
            {"name": "ResetPlayer", "fuzzy_match_percent": 73.18},
            {"name": "ManageCreatures"},   # 0% omitted by objdiff
        ],
    }]}

    def test_report_unit_parsing(self):
        pcts, measures = decomp.report_unit(self.REPORT, "game/creature")
        self.assertEqual(pcts["ChangeCharacter"], 100.0)
        self.assertEqual(pcts["ResetPlayer"], 73.18)
        self.assertEqual(pcts["ManageCreatures"], 0.0)  # missing -> 0.0
        self.assertEqual(decomp._as_float(measures["fuzzy_match_percent"]),
                         89.31)

    def test_missing_unit_returns_empty(self):
        self.assertEqual(decomp.report_unit(self.REPORT, "no/unit"), ({}, {}))

    def test_verified_ids_only_verified_matching(self):
        verify = {"functions": [
            {"id": "u:001:aa:A", "state": "matching", "verified": True},
            {"id": "u:001:bb:B", "state": "matching", "verified": False},
            {"id": "u:001:cc:C", "state": "equivalent", "verified": True},
        ]}
        self.assertEqual(decomp.verified_ids(verify), {"u:001:aa:A"})

    def test_build_rows_merges_state_and_verified(self):
        manifest_rows = [
            ("pal103:unit-0091:001cd7c8:ChangeCharacter", "ChangeCharacter",
             "matching"),
            ("pal103:unit-0091:001cbce8:ResetPlayer", "ResetPlayer", "asm"),
        ]
        pcts = {"ChangeCharacter": 100.0, "ResetPlayer": 73.18}
        verified = {"pal103:unit-0091:001cd7c8:ChangeCharacter"}
        rows = decomp.build_rows(manifest_rows, pcts, verified)
        self.assertEqual(rows, [
            ("ChangeCharacter", 100.0, "matching", True),
            ("ResetPlayer", 73.18, "asm", False),
        ])

    def test_promote_candidates_only_hundred_percent_asm(self):
        manifest_rows = [
            ("id:A", "A", "asm"),        # 100% asm -> candidate
            ("id:B", "B", "asm"),        # 73% asm  -> no
            ("id:C", "C", "matching"),   # 100% matching -> already done
        ]
        pcts = {"A": 100.0, "B": 73.18, "C": 100.0}
        self.assertEqual(decomp.promote_candidates(manifest_rows, pcts),
                         ["id:A"])

    def test_format_table_shape(self):
        rows = [("ChangeCharacter", 100.0, "matching", True),
                ("ResetPlayer", 73.18, "asm", False)]
        out = decomp.format_table("game/creature", "default", rows,
                                  {"fuzzy_match_percent": "89.31"})
        self.assertIn("Unit: game/creature", out)
        self.assertIn("Compiler: default", out)
        self.assertRegex(out, r"ChangeCharacter\s+100\.00%\s+matching\s+yes")
        self.assertRegex(out, r"ResetPlayer\s+73\.18%\s+asm\s+no")
        self.assertIn("Verified matching:", out)
        self.assertRegex(out, r"Verified matching:\s+1 / 2")


class _Recorder:
    """Replaces decomp.run: records commands, returns a scripted exit code."""
    def __init__(self, rc=0):
        self.calls = []
        self.rc = rc

    def __call__(self, cmd):
        self.calls.append(list(cmd))
        return self.rc


class TestOrchestration(unittest.TestCase):
    # ResetPlayer is `asm` in the committed creature manifest; report it 100%.
    FAKE_REPORT = {"units": [{
        "name": "game/creature", "measures": {},
        "functions": [{"name": "ResetPlayer",
                       "fuzzy_match_percent": 100.0}]}]}

    def setUp(self):
        self._orig = {k: getattr(decomp, k)
                      for k in ("run", "_regen_graph", "_graph_stale",
                                "_load_json")}
        decomp._regen_graph = lambda version: None
        decomp._graph_stale = lambda *a, **k: False
        decomp._load_json = lambda rel: (
            self.FAKE_REPORT if rel.endswith("report.json") else {})

    def tearDown(self):
        for k, v in self._orig.items():
            setattr(decomp, k, v)

    def test_setup_runs_full_pipeline(self):
        decomp.run = rec = _Recorder(rc=0)
        self.assertEqual(decomp.cmd_setup("pal103"), 0)
        self.assertEqual(rec.calls, decomp.setup_commands())

    def test_setup_stops_on_first_failure(self):
        decomp.run = rec = _Recorder(rc=1)
        self.assertEqual(decomp.cmd_setup("pal103"), 1)
        self.assertEqual(len(rec.calls), 1)   # aborted after the first step

    def test_unit_auto_promotes_hundred_percent_asm(self):
        decomp.run = rec = _Recorder(rc=0)
        self.assertEqual(decomp.cmd_unit(CREATURE, "pal103", promote=True), 0)
        promote_calls = [c for c in rec.calls if "tools/promote.py" in c]
        self.assertEqual(len(promote_calls), 1)
        self.assertIn("pal103:unit-0091:001cbce8:ResetPlayer",
                      promote_calls[0])

    def test_unit_no_promote_flag_skips_promotion(self):
        decomp.run = rec = _Recorder(rc=0)
        self.assertEqual(decomp.cmd_unit(CREATURE, "pal103", promote=False), 0)
        self.assertFalse(any("tools/promote.py" in c for c in rec.calls))


class TestMainRouting(unittest.TestCase):
    def setUp(self):
        self._orig = (decomp.cmd_toolchain, decomp.cmd_setup, decomp.cmd_unit)
        self.seen = []
        decomp.cmd_toolchain = lambda v: self.seen.append(("toolchain", v)) or 0
        decomp.cmd_setup = lambda v: self.seen.append(("setup", v)) or 0
        decomp.cmd_unit = lambda a, v, promote=True: \
            self.seen.append(("unit", a, promote)) or 0

    def tearDown(self):
        (decomp.cmd_toolchain, decomp.cmd_setup,
         decomp.cmd_unit) = self._orig

    def test_routes_toolchain(self):
        decomp.main(["toolchain"])
        self.assertEqual(self.seen, [("toolchain", "pal103")])

    def test_routes_setup(self):
        decomp.main(["setup"])
        self.assertEqual(self.seen, [("setup", "pal103")])

    def test_routes_unit_path(self):
        decomp.main([CREATURE])
        self.assertEqual(self.seen, [("unit", CREATURE, True)])

    def test_no_promote_flag(self):
        decomp.main([CREATURE, "--no-promote"])
        self.assertEqual(self.seen, [("unit", CREATURE, False)])

    def test_unknown_command_errors(self):
        with self.assertRaises(SystemExit):
            decomp.main(["frobnicate"])


if __name__ == "__main__":
    unittest.main()
