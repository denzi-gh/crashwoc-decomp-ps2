# CI architecture (dtk-template private-build-image method)

Two workflows, both on GitHub-hosted runners:

- **validate.yml** — every push and PR. Structure only: manifests,
  registries, tests, and the guard that nothing game-derived is tracked in
  this repository. Needs no game files.
- **matching.yml** — pushes to main, nightly, and manual dispatch only
  (never on `pull_request`). The full byte-gated pipeline. It needs the
  retail ELF, which this public repository must never contain — so the job
  runs *inside* a private container image that has the game files baked in,
  following
  [dtk-template's method](https://github.com/encounter/dtk-template/blob/main/docs/github_actions.md).

```
this repo (public, zero game bytes)
  Containerfile ──publish-image.yml──► ghcr.io/denzi-gh/crashwoc-decomp      [public package]
                                              ▲ FROM
crashwoc-decomp-ps2-build (PRIVATE repo)      │
  orig/pal103/{SLES_503.86,SYSTEM.CNF}        │
  Dockerfile (COPY orig /orig) ── its build.yml ──► ghcr.io/denzi-gh/crashwoc-decomp-ps2-build:main  [PRIVATE package]
                                                          ▲ matching.yml job container
matching.yml: cp -a /orig . → cached toolchain install + fingerprints → verify_target
  → registry --checks → configure → ninja gates → report → check_report_matches
  → sanitize → stage → smoke_report → upload artifact
  → progress baseline bot-commit (main pushes only)
```

## The three pieces

1. **Public base image** (`ghcr.io/denzi-gh/crashwoc-decomp`): built from
   [Containerfile](../Containerfile) by `publish-image.yml` whenever the
   Containerfile or requirements change. Contains only the build
   environment (Debian, Python, splat, ninja, i386 runtime libs) — no game
   bytes, no matching toolchain — so the package is safe to keep public.
2. **Private build repo** (`denzi-gh/crashwoc-decomp-ps2-build`): holds
   `orig/pal103/` and a two-line Dockerfile (`FROM` the base image,
   `COPY orig /orig`). Its workflow pushes the result to GHCR as a
   **private** package. To update game files or pick up a new base image,
   push there or re-run its workflow.
3. **matching.yml** here: runs in that private image on `ubuntu-latest`.
   Pull authorization is the package's Actions-access grant plus plain
   `GITHUB_TOKEN` — no PAT, no secret URL.

## One-time setup (already done; recorded for re-setup)

1. Run `publish-image.yml` once (workflow_dispatch), then make the
   `crashwoc-decomp` package **public**: package page → Package settings →
   Change visibility.
2. On the `crashwoc-decomp-ps2-build` package: Package settings →
   **Manage Actions access** → add repository `crashwoc-decomp-ps2` with
   the **Read** role.

## Container image pinning

The [Containerfile](../Containerfile) pins every upstream source by immutable
digest so the build environment is byte-reproducible:

- **Base image**: `python:3.12-slim-trixie@sha256:423ed6ab…199fbf`.
- **uv**: `ghcr.io/astral-sh/uv:0.11.27@sha256:4d01caf3…693419` (copied in for
  the `/uv` binary).

The human-readable tags are kept alongside the digests for readability only —
the `@sha256:` digest is what Docker resolves. Python packages are pinned
separately in [requirements.txt](../requirements.txt) (`splat64`, `spimdisasm`,
`rabbitizer`), which must match `toolchain.lock.json`.

### Updating a pinned digest

1. Resolve the new digest from the registry (do not hand-write it). For the
   base image:
   ```bash
   docker buildx imagetools inspect python:3.12-slim-trixie \
     --format '{{.Manifest.Digest}}'
   ```
   For uv, pick a concrete version tag (e.g. a new `0.x.y`) and inspect it:
   ```bash
   docker buildx imagetools inspect ghcr.io/astral-sh/uv:0.x.y \
     --format '{{.Manifest.Digest}}'
   ```
   (Without buildx, the registry HTTP API's `Docker-Content-Digest` response
   header on a `HEAD`/`GET` of the manifest gives the same value.)
2. Replace the `@sha256:` in the Containerfile (and the tag/version comment).
   Use the multi-arch index digest, not a single-platform one, so the image
   stays portable.
3. Rebuild and re-verify (see below); update this section's digests.

### Tests after a pin update

- `docker build -f Containerfile -t crashwoc-decomp .` succeeds.
- Inside the image, `uv pip list` shows the locked `splat64`/`spimdisasm`/
  `rabbitizer` versions, i.e. `python configure.py --strict` passes.
- The full `matching.yml` pipeline (via the private build image rebuilt
  `FROM` this base) is green end-to-end.

## Caching and speed

Two `actions/cache` entries keep runs fast: the locked toolchain
(`tools/download` + `compiler`, keyed on `toolchain.lock.json`) and the
ninja build tree (`build`, `expected`, `.ninja_*`, keyed per commit with a
prefix restore key). A cold run does image pull + toolchain download
(~5–10 min); warm runs are incremental.

## What can leave a run

Only two things, ever: the `SLES_503.86_report` artifact (the
whitelist-sanitized report from `tools/sanitize_report.py` — this is what
[decomp.dev](https://decomp.dev) consumes) and, on main-branch pushes with
changed counts, a bot commit of `progress/summary.json`. Everything else
(orig copies, objects, images) dies with the ephemeral runner.

Before the artifact uploads, two gates run against it. `tools/check_report_matches.py`
(right after `ninja report`) asserts every function `verify_promoted.py` proved
byte-matching reads 100% in the report. Then, after the artifact is staged to
`build/publish/report.json`, `tools/smoke_report.py` re-checks the exact bytes
that will leave the runner: valid JSON, a strict subset of objdiff's Report
schema (no stray fields), no `orig/` reference / absolute path / embedded byte
blob, no hollow "100% of nothing" measure, and again every verified matching
function at 100%. A stale or tampered staged file fails the run before upload.

The report also models the program's data: `tools/gen_objdiff.py` lists one
target-only unit per linked data object in a dedicated `data` category, so
`total_data` is honest (real linked bytes, zero matched until C data exists) and
no artificial code value is invented. Note decomp.dev's treemap only renders
units with `total_code > 0`; the data units are correctly counted in the report
and category metrics but do **not** appear as clickable tiles — expected, not a
regression.

## PR verification (no fork ever touches the private image)

`matching.yml` deliberately has **no `pull_request` trigger**. A
`pull_request` run would execute the workflow file *from the PR branch*
inside the private `/orig` image, so any fork could rewrite it to exfiltrate
the retail game files. Removing the trigger closes that hole entirely;
`pull_request_target` is **not** used, because it would run the same
untrusted PR code with the same private resources.

What every PR still gets automatically: **validate.yml** (public,
GitHub-hosted, no game files) runs on every push and pull request —
manifests, registries, unit tests, and the "nothing game-derived is tracked"
guard. That is the full contract a fork PR can rely on.

Byte verification of a contributed change is a maintainer action:

1. Let validate.yml run on the PR (automatic, public).
2. Review the diff — especially any change to `.github/`, `tools/`, or the
   toolchain lock.
3. Cherry-pick / merge the reviewed commit onto an **in-repo** branch (not a
   fork). Only maintainers can push in-repo branches.
4. Run `matching.yml` on that branch via **workflow_dispatch** (or let the
   nightly/`main` run cover it). This is the only path that reaches `/orig`,
   and it only ever runs code a maintainer has vetted.
5. Merge once the byte gates are green.

Non-PR runs never upload the baseline either: the bot-commit step is gated
to `push` events on `refs/heads/main`.

## Progress publication

`compare_progress.py` gates every run (regressions are red before anything
is published) and `--write` only rewrites `progress/summary.json` when a
non-volatile count changed, so nightly runs don't produce noise commits.
The summary moves only on *verified* progress (promoted functions, complete
units); WIP fuzzy percentages live in the report artifact alone.

## Rollout checklist

Before opening this branch to `main` and going public:

1. **Local (host, no toolchain):** `python -m unittest discover -s tests -v`,
   `python tools/status.py --check`, `python tools/gen_ninja.py` and
   `python tools/gen_objdiff.py` — the graph and objdiff config regenerate
   deterministically.
2. **Container, full byte matrix** (`python tools/dispatch.py ...`):
   `fingerprint_compiler.py --all` → `configure.py --strict` (and
   `--strict --check` for the nightly path) → `ninja expected current matching`
   → `ninja verify-loaded verify-promoted` → `compare_progress.py` →
   `ninja report report-public` → `check_report_matches.py` → stage → `smoke_report.py`.
3. **Confirm invariants:** loaded matching image is byte-identical to retail
   (`c92a5987…0fe438`); every `matching` function verified over its full extent;
   `verify_results.json` ↔ `progress/summary.json` ↔ public report agree; verified
   matching functions read 100%; data metrics honest (no "100% of nothing"); the
   `validate.yml` "nothing game-derived is tracked" guard stays green.
4. **Fork-PR dry run:** a fork PR triggers only `validate.yml`, never `matching.yml`.
5. **Independent review** of the diff, especially `.github/`, `tools/`, and the
   toolchain lock. Rebase on `main`, submit, merge only on green byte gates.
6. **Post-merge:** confirm the `main` run is green, download and eyeball the
   `SLES_503.86_report` artifact, then make the repo public. The private build
   image / GHCR package stays private; add the project to decomp.dev last.
