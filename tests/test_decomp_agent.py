"""Public tests for the decomp-agent domain layer and MCP glue.

These run with NO game assets, NO compiler install, NO generated disassembly and
NO proprietary data: a synthetic miniature project tree is built in a temp
directory. Toolchain-dependent behavior (compile_diff / verify / promote real
bytes) is intentionally NOT exercised here -- it belongs to the gated matching
CI. Everything below is pure Python.
"""
import json
import sys
import tempfile
import textwrap
import unittest
from pathlib import Path
from unittest import mock

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))

from decomp_agent import identifiers, schemas, classify
from decomp_agent.project import DecompProject, ProjectError
from decomp_agent.registry import Registry
from decomp_agent import candidates, context as ctx, evidence, sessions, blockers


UNITS = """\
[[unit]]
index = 0
name = '/usr/local/sce/ee/lib/crt0.s'

[[unit]]
index = 1
name = '..\\nu2crash.ps2\\nucore\\nulist.c'

[[unit]]
index = 2
name = '.\\main.c'
"""

FUNCTIONS = """\
[[function]]
index = 0
name = '_start'
address = 0x00100008
unit = 0
frame_size = 0
reg_mask = 0x00000000

[[function]]
index = 1
name = 'NuListGetHead'
address = 0x00100040
unit = 1
frame_size = 0
reg_mask = 0x00000000

[[function]]
index = 2
name = 'NuListDoWork'
address = 0x00100060
unit = 1
frame_size = 32
reg_mask = 0x80000000

[[function]]
index = 3
name = 'DupStatic'
address = 0x00100100
unit = 1
frame_size = 0
reg_mask = 0x00000000

[[function]]
index = 4
name = 'MainLoop'
address = 0x00100140
unit = 2
frame_size = 16
reg_mask = 0x80000000

[[function]]
index = 5
name = 'DupStatic'
address = 0x00100200
unit = 2
frame_size = 0
reg_mask = 0x00000000
"""

PROFILES = """\
schema = 1

[profile.default]
compiler = "ee-gcc-tt"
flags = ["-O2", "-G8", "-fomit-frame-pointer"]

[profile.sce]
compiler = "ee-gcc"
flags = ["-O2", "-G8", "-fomit-frame-pointer"]
"""

SECTIONS = json.dumps({
    "sections": [{"name": ".text", "addr": "0x00100000", "size": "0x00000400",
                  "type": 1, "offset": "0x1000"}],
    "program_headers": [{"offset": "0x1000", "vaddr": "0x00100000",
                         "filesz": "0x400", "memsz": "0x400"}],
})

SYMBOLS = "NuListGetHead = 0x00100040\nsomeGlobal = 0x00293700\n"

MANIFEST = """\
schema = 1
unit = "pal103:unit-0001"
source = "src/nucore/nulist.c"
profile = "default"
complete = false

[[function]]
id = "pal103:unit-0001:00100040:NuListGetHead"
state = "matching"

[[function]]
id = "pal103:unit-0001:00100060:NuListDoWork"
state = "asm"

[[function]]
id = "pal103:unit-0001:00100100:DupStatic"
state = "asm"
"""


def make_project(tmp: Path, version="pal103") -> DecompProject:
    (tmp / "tools").mkdir(parents=True)
    for marker in ("cc.py", "dispatch.py", "promote.py"):
        (tmp / "tools" / marker).write_text("# stub\n")
    cfg = tmp / "config" / version
    cfg.mkdir(parents=True)
    (cfg / "units.toml").write_text(UNITS)
    (cfg / "functions.toml").write_text(FUNCTIONS)
    (cfg / "profiles.toml").write_text(PROFILES)
    (cfg / "sections.json").write_text(SECTIONS)
    (cfg / "symbol_addrs.txt").write_text(SYMBOLS)
    status = cfg / "status" / "nucore"
    status.mkdir(parents=True)
    (status / "nulist.toml").write_text(MANIFEST)
    src = tmp / "src" / "nucore"
    src.mkdir(parents=True)
    (src / "nulist.c").write_text(
        "int NuListGetHead(void){return 0;}\n"
        "int NuListDoWork(int a){return a+1;}\n")
    (tmp / "config" / "decomp_agent.toml").write_text(
        'schema = 1\nproject = "crashwoc-decomp-ps2"\ndomain_api = 1\n'
        'default_version = "pal103"\n')
    return DecompProject(tmp, version=version)


class TestProjectValidation(unittest.TestCase):
    def test_valid_project(self):
        with tempfile.TemporaryDirectory() as d:
            p = make_project(Path(d))
            self.assertTrue(p.is_compatible())

    def test_unrelated_dir_rejected(self):
        with tempfile.TemporaryDirectory() as d:
            with self.assertRaises(ProjectError):
                DecompProject(d)

    def test_missing_registry_rejected(self):
        with tempfile.TemporaryDirectory() as d:
            p = make_project(Path(d))
            (p.config_dir / "functions.toml").unlink()
            self.assertFalse(DecompProject(p.root, validate=False).is_compatible())

    def test_path_traversal_rejected(self):
        with tempfile.TemporaryDirectory() as d:
            p = make_project(Path(d))
            with self.assertRaises(ProjectError):
                p.path("..", "etc", "passwd")
            with self.assertRaises(ProjectError):
                p.path("/etc/passwd")
            # legitimate nested path stays inside
            self.assertTrue(p.contains(p.path("config", "pal103", "units.toml")))

    def test_resolve_env(self):
        with tempfile.TemporaryDirectory() as d:
            make_project(Path(d))
            with mock.patch.dict("os.environ", {"CRASHWOC_PROJECT_DIR": d}):
                p = DecompProject.resolve()
                self.assertEqual(p.root, Path(d).resolve())


class TestIdentifiers(unittest.TestCase):
    def test_parse_function_id(self):
        fid = identifiers.parse_function_id("pal103:unit-0007:00105a60:NuListGetNext")
        self.assertEqual(fid.unit_index, 7)
        self.assertEqual(fid.address, 0x00105a60)
        self.assertEqual(fid.name, "NuListGetNext")
        self.assertEqual(fid.unit_id, "pal103:unit-0007")

    def test_invalid_ids(self):
        for bad in ("", "nope", "pal103:unit-7:00105a60:x",
                    "pal103:unit-0007:105a60:x", "pal103:unit-0007:00105a60:"):
            with self.assertRaises(identifiers.IdentifierError):
                identifiers.parse_function_id(bad)

    def test_unit_id(self):
        self.assertEqual(identifiers.parse_unit_id("pal103:unit-0007").unit_index, 7)
        with self.assertRaises(identifiers.IdentifierError):
            identifiers.parse_unit_id("pal103:unit-7")

    def test_address(self):
        self.assertEqual(identifiers.parse_address("0x105a60"), 0x105a60)
        self.assertEqual(identifiers.parse_address("105a60"), 0x105a60)
        with self.assertRaises(identifiers.IdentifierError):
            identifiers.parse_address("0x1_0000_0000")


class TestRegistry(unittest.TestCase):
    def setUp(self):
        self._tmp = tempfile.TemporaryDirectory()
        self.p = make_project(Path(self._tmp.name))
        self.reg = Registry(self.p)

    def tearDown(self):
        self._tmp.cleanup()

    def test_function_count_and_units(self):
        self.assertEqual(len(self.reg.functions), 6)
        self.assertEqual(self.reg.units[1].name, "nucore/nulist")
        self.assertEqual(self.reg.units[1].category, "engine")
        self.assertEqual(self.reg.units[2].category, "game")

    def test_extent_calculation(self):
        head = self.reg.by_id["pal103:unit-0001:00100040:NuListGetHead"]
        self.assertEqual(head.size, 0x20)          # gap to NuListDoWork
        last = self.reg.by_id["pal103:unit-0002:00100200:DupStatic"]
        self.assertEqual(last.end, 0x00100400)     # to end of .text

    def test_resolve_unique_name(self):
        r = self.reg.resolve("NuListGetHead")
        self.assertEqual(r.status, "resolved")
        self.assertEqual(r.record.address, 0x00100040)

    def test_resolve_duplicate_name_is_ambiguous(self):
        r = self.reg.resolve("DupStatic")
        self.assertEqual(r.status, "ambiguous")
        self.assertEqual(len(r.candidates), 2)

    def test_resolve_address(self):
        self.assertEqual(self.reg.resolve("0x00100040").status, "resolved")
        self.assertEqual(self.reg.resolve("0x00999999").status, "not_found")

    def test_profile_loading_and_resolution(self):
        self.assertIn("default", self.reg.profiles)
        # manifest wins (source-of-truth precedence over category default)
        name, src = self.reg.effective_profile(1)
        self.assertEqual((name, src), ("default", "manifest"))
        # no manifest -> category default (game -> default)
        name2, src2 = self.reg.effective_profile(2)
        self.assertEqual(src2, "category-default")

    def test_state_of(self):
        self.assertEqual(
            self.reg.state_of("pal103:unit-0001:00100040:NuListGetHead"), "matching")
        self.assertEqual(
            self.reg.state_of("pal103:unit-0002:00100140:MainLoop"), "asm")


class TestCandidates(unittest.TestCase):
    def setUp(self):
        self._tmp = tempfile.TemporaryDirectory()
        self.p = make_project(Path(self._tmp.name))

    def tearDown(self):
        self._tmp.cleanup()

    def test_matching_excluded_and_deterministic(self):
        a = candidates.list_candidates(self.p, limit=50)
        b = candidates.list_candidates(self.p, limit=50)
        self.assertEqual([c["id"] for c in a["candidates"]],
                         [c["id"] for c in b["candidates"]])
        ids = {c["id"] for c in a["candidates"]}
        # already-matching NuListGetHead must not appear
        self.assertNotIn("pal103:unit-0001:00100040:NuListGetHead", ids)
        # hand-written .s unit (crt0) function excluded
        self.assertNotIn("pal103:unit-0000:00100008:_start", ids)

    def test_leaf_ranks_above_framed(self):
        res = candidates.list_candidates(self.p, limit=50)
        scores = {c["name"]: c["score"] for c in res["candidates"]}
        self.assertGreater(scores.get("DupStatic", 0), scores.get("NuListDoWork", 0))

    def test_max_size_filter(self):
        res = candidates.list_candidates(self.p, max_size=16, limit=50)
        self.assertTrue(all(c["size"] <= 16 for c in res["candidates"]))


class TestContext(unittest.TestCase):
    def setUp(self):
        self._tmp = tempfile.TemporaryDirectory()
        self.p = make_project(Path(self._tmp.name))

    def tearDown(self):
        self._tmp.cleanup()

    def test_context_serializes(self):
        d = ctx.get_context(self.p, "pal103:unit-0001:00100060:NuListDoWork")
        self.assertTrue(d["ok"])
        json.dumps(d)  # must be JSON-serializable
        self.assertIn("facts", d)
        self.assertIn("unknowns", d)
        self.assertEqual(d["facts"]["compiler_profile"], "default")

    def test_context_ambiguous(self):
        d = ctx.get_context(self.p, "DupStatic")
        self.assertFalse(d["ok"])
        self.assertEqual(d["error"]["code"], "ambiguous")

    def test_ground_truth_block_always_present(self):
        d = ctx.get_context(self.p, "pal103:unit-0001:00100060:NuListDoWork")
        gt = d["ground_truth"]
        self.assertEqual(gt["authority"], "target_disassembly")
        self.assertIn("reference_decomp_policy", gt)
        self.assertIn("hint", gt["reference_decomp_policy"].lower())
        # No asm/ in the fixture: reports unavailable, never fabricates a body.
        self.assertFalse(gt["target_disassembly_available"])
        self.assertFalse(gt["attached_inline"])
        self.assertNotIn("target_asm", d)
        # The first unknown steers the caller to read ground truth.
        self.assertIn("target disassembly", d["unknowns"][0].lower())

    def test_get_disassembly_absent_is_honest(self):
        d = ctx.get_disassembly(self.p, "pal103:unit-0001:00100060:NuListDoWork")
        self.assertTrue(d["ok"])
        self.assertFalse(d["available"])
        self.assertIsNone(d["disassembly"])
        self.assertIn("configure", (d.get("reason") or "").lower())

    def test_ground_truth_inlines_small_listing(self):
        asm = self.p.root / "asm"
        asm.mkdir()
        (asm / "text.s").write_text(
            "glabel NuListDoWork\n"
            "    /* 0 00100060 03E00008 */  jr $ra\n"
            "    /* 4 00100064 24420001 */  addiu $v0, $v0, 1\n"
            "glabel Next\n")
        d = ctx.get_context(self.p, "pal103:unit-0001:00100060:NuListDoWork")
        self.assertTrue(d["ground_truth"]["target_disassembly_available"])
        self.assertTrue(d["ground_truth"]["attached_inline"])
        self.assertIn("target_asm", d)
        self.assertIn("jr $ra", d["target_asm"]["text"])
        dis = ctx.get_disassembly(self.p, "pal103:unit-0001:00100060:NuListDoWork")
        self.assertTrue(dis["available"])
        self.assertIn("addiu", dis["disassembly"])


class TestEvidence(unittest.TestCase):
    def setUp(self):
        self._tmp = tempfile.TemporaryDirectory()
        self.p = make_project(Path(self._tmp.name))

    def tearDown(self):
        self._tmp.cleanup()

    def test_search_finds_registry_and_source(self):
        r = evidence.search_evidence(self.p, "NuListGetHead")
        auths = {h["authority"] for h in r["hits"]}
        self.assertIn("registry", auths)
        self.assertTrue(r["hits"][0]["authority_rank"] <= 3)


class TestClassify(unittest.TestCase):
    def _fn(self, size=64, frame=0, reg_mask=0, freg_mask=0):
        from decomp_agent.registry import FunctionRecord
        return FunctionRecord("pal103", 0, "f", 0x1000, 1, size, frame, reg_mask,
                              freg_mask, 0, 0, 0, 0, False)

    def test_compile_error_is_proven(self):
        out = classify.classify(compile_ok=False, compile_error="boom",
                                target_fn=self._fn(), exact=False)
        self.assertEqual(out[0]["category"], "compile_error")
        self.assertEqual(out[0]["confidence"], "proven")

    def test_frame_mismatch_proven(self):
        out = classify.classify(compile_ok=True, target_fn=self._fn(frame=32),
                                candidate_frame=0, exact=False)
        cats = {c["category"] for c in out}
        self.assertIn("stack_frame", cats)

    def test_register_allocation_window(self):
        # same opcode (addiu 0x09), different registers
        tw = (0x09 << 26) | (1 << 21) | (2 << 16)
        cw = (0x09 << 26) | (3 << 21) | (4 << 16)
        out = classify.classify(compile_ok=True, target_fn=self._fn(),
                                windows=[(0, tw, cw)], exact=False)
        self.assertTrue(any(c["category"] == "register_allocation" for c in out))

    def test_exact_no_classifications(self):
        self.assertEqual(classify.classify(compile_ok=True, target_fn=self._fn(),
                                           exact=True), [])

    def test_heuristic_never_proven(self):
        tw, cw = (0x09 << 26), (0x0d << 26)
        out = classify.classify(compile_ok=True, target_fn=self._fn(),
                                windows=[(0, tw, cw)], exact=False)
        for c in out:
            if c["category"] not in ("compile_error", "unresolved_symbol",
                                     "stack_frame", "saved_registers",
                                     "size_difference"):
                self.assertNotEqual(c["confidence"], "proven")


class TestSessions(unittest.TestCase):
    def setUp(self):
        self._tmp = tempfile.TemporaryDirectory()
        self.p = make_project(Path(self._tmp.name))

    def tearDown(self):
        self._tmp.cleanup()

    def _start(self, client="claude-code"):
        return sessions.start_session(self.p, "NuListDoWork", client=client)["session_id"]

    def test_create_and_atomic_file(self):
        sid = self._start()
        ledger = self.p.sessions_dir / f"{sid}.json"
        self.assertTrue(ledger.is_file())
        json.loads(ledger.read_text())     # valid JSON, not partial
        self.assertFalse(list(self.p.sessions_dir.glob("*.tmp")))

    def test_repeated_asm_and_diff_signature(self):
        sid = self._start()
        sessions.checkpoint_session(self.p, sid, client="claude-code",
                                    candidate_asm_hash="h1", diff_signature="s1",
                                    fuzzy_score=50, compile_ok=True, exact=False)
        c2 = sessions.checkpoint_session(self.p, sid, client="claude-code",
                                         candidate_asm_hash="h1", diff_signature="s1",
                                         fuzzy_score=50, compile_ok=True, exact=False)
        self.assertTrue(c2["attempt"]["repeated_asm_hash"])
        self.assertTrue(c2["attempt"]["repeated_diff_signature"])

    def test_oscillation_detected(self):
        sid = self._start()
        for sig in ("s1", "s2", "s1"):
            c = sessions.checkpoint_session(self.p, sid, client="claude-code",
                                            diff_signature=sig, fuzzy_score=50,
                                            compile_ok=True, exact=False)
        self.assertTrue(c["attempt"]["oscillation"])

    def test_non_improvement_and_compile_failures(self):
        sid = self._start(client="c")
        sessions.checkpoint_session(self.p, sid, client="c", fuzzy_score=80,
                                    compile_ok=True, exact=False)
        for _ in range(2):
            c = sessions.checkpoint_session(self.p, sid, client="c", fuzzy_score=70,
                                            compile_ok=True, exact=False)
        self.assertGreaterEqual(c["counters"]["non_improvement_count"], 2)
        for _ in range(3):
            c = sessions.checkpoint_session(self.p, sid, client="c",
                                            compile_ok=False)
        self.assertEqual(c["counters"]["compile_failure_count"], 3)
        self.assertTrue(c["stop"]["should_stop"])

    def test_locking_prevents_second_writer(self):
        sid = self._start(client="claude-code")
        r = sessions.checkpoint_session(self.p, sid, client="codex",
                                        fuzzy_score=10, compile_ok=True)
        self.assertFalse(r["ok"])
        self.assertEqual(r["error"]["code"], "locked")

    def test_resume_after_client_change(self):
        sid = self._start(client="claude-code")
        sessions.end_session(self.p, sid, outcome=None, client="claude-code")
        r = sessions.resume_session(self.p, sid, client="codex")
        self.assertTrue(r["ok"])
        self.assertTrue(r["safe_to_resume"])

    def test_stale_revision_detection(self):
        sid = self._start()
        led = self.p.sessions_dir / f"{sid}.json"
        data = json.loads(led.read_text())
        data["repo_revision"] = "deadbeef"
        led.write_text(json.dumps(data))
        with mock.patch.object(sessions, "_git_revision", return_value="cafef00d"):
            r = sessions.resume_session(self.p, sid)
        self.assertFalse(r["ok"])
        self.assertIn("repo_revision_changed", r["conflicts"])

    def test_profile_fingerprint_mismatch(self):
        sid = self._start()
        led = self.p.sessions_dir / f"{sid}.json"
        data = json.loads(led.read_text())
        data["profile_fingerprint"] = "AAA"
        led.write_text(json.dumps(data))
        with mock.patch.object(sessions, "_fingerprint", return_value="BBB"):
            r = sessions.resume_session(self.p, sid)
        self.assertIn("profile_fingerprint_changed", r["conflicts"])

    def test_token_usage_accumulates_and_reports(self):
        sid = self._start(client="claude-code")
        sessions.checkpoint_session(self.p, sid, client="claude-code",
                                    model="opus-4.8", fuzzy_score=50,
                                    compile_ok=True, exact=False,
                                    input_tokens=1000, output_tokens=200)
        r = sessions.checkpoint_session(self.p, sid, client="claude-code",
                                        model="opus-4.8", fuzzy_score=60,
                                        compile_ok=True, exact=False,
                                        input_tokens=500, output_tokens=100)
        tu = r["token_usage"]
        self.assertEqual(tu["input"], 1500)
        self.assertEqual(tu["output"], 300)
        self.assertEqual(tu["total"], 1800)
        self.assertEqual(tu["reported_checkpoints"], 2)
        self.assertEqual(tu["by_model"]["opus-4.8"]["total"], 1800)
        # end_session surfaces the total; a checkpoint with no tokens is ignored
        e = sessions.end_session(self.p, sid, outcome="NO_PROGRESS",
                                 client="claude-code")
        self.assertEqual(e["token_usage"]["total"], 1800)

    def test_restore_conflict_when_source_changed(self):
        sid = self._start()
        # record a best snapshot
        sessions.checkpoint_session(self.p, sid, client="claude-code",
                                    fuzzy_score=90, compile_ok=True, exact=False,
                                    source_snapshot="int x;\n", source_hash="h")
        # user edits the file out from under us
        (self.p.root / "src" / "nucore" / "nulist.c").write_text("changed\n")
        r = sessions.restore_best_candidate(self.p, sid)
        self.assertFalse(r["ok"])
        self.assertEqual(r["error"]["code"], "source_conflict")


class TestBlockers(unittest.TestCase):
    def setUp(self):
        self._tmp = tempfile.TemporaryDirectory()
        self.p = make_project(Path(self._tmp.name))

    def tearDown(self):
        self._tmp.cleanup()

    def test_record_and_exclude(self):
        r = blockers.record_blocker(self.p, "NuListDoWork",
                                    kind="unsupported_lit8", confidence="proven")
        self.assertTrue(r["ok"])
        self.assertTrue(r["blocker"]["architectural"])
        by_fn = blockers.blockers_by_function(self.p)
        self.assertIn("pal103:unit-0001:00100060:NuListDoWork", by_fn)
        # excluded from candidates
        res = candidates.list_candidates(self.p, limit=50)
        self.assertNotIn("pal103:unit-0001:00100060:NuListDoWork",
                         {c["id"] for c in res["candidates"]})

    def test_bad_kind_rejected(self):
        r = blockers.record_blocker(self.p, "NuListDoWork", kind="nonsense")
        self.assertFalse(r["ok"])


class TestMcpGlue(unittest.TestCase):
    def test_resolve_project_validates(self):
        from decomp_mcp import server
        with tempfile.TemporaryDirectory() as d:
            make_project(Path(d))
            p = server.resolve_project(d)
            self.assertTrue(p.is_compatible())
        with tempfile.TemporaryDirectory() as d2:
            with self.assertRaises(ProjectError):
                server.resolve_project(d2)


class TestSchemas(unittest.TestCase):
    def test_envelopes(self):
        self.assertTrue(schemas.ok("x", a=1)["ok"])
        self.assertFalse(schemas.err("x", "bad")["ok"])

    def test_classification_clamps(self):
        c = schemas.classification("not_a_category", "not_a_conf", ["e"])
        self.assertEqual(c["category"], "unknown")
        self.assertEqual(c["confidence"], "low")


if __name__ == "__main__":
    unittest.main()
