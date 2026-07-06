# CI architecture (dtk-template private-build-image method)

Two workflows, both on GitHub-hosted runners:

- **validate.yml** — every push and PR. Structure only: manifests,
  registries, tests, and the guard that nothing game-derived is tracked in
  this repository. Needs no game files.
- **matching.yml** — pushes to main, pull requests, nightly, manual. The
  full byte-gated pipeline. It needs the retail ELF, which this public
  repository must never contain — so the job runs *inside* a private
  container image that has the game files baked in, following
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
  → registry --checks → configure → ninja gates → report → sanitized artifact
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

## PR builds and the accepted risk

`matching.yml` runs on pull requests so contributors get byte verification
before merge. Two consequences, both accepted deliberately:

- `pull_request` runs use the workflow file **from the PR branch**, so a
  malicious PR could alter the workflow to exfiltrate `/orig` from the
  private image. This is the same trade-off every dtk-template project
  makes; the "secret" is a game file contributors must own anyway.
- Fork PRs may fail to pull the private image (their `GITHUB_TOKEN` may
  lack access). validate.yml still checks them; a failed matching job on a
  fork PR is acceptable noise, and maintainers can re-run the job after
  merging or pushing the branch in-repo.

PR runs never upload the baseline: the bot-commit step is gated to
`push` events on `refs/heads/main`.

## Progress publication

`compare_progress.py` gates every run (regressions are red before anything
is published) and `--write` only rewrites `progress/summary.json` when a
non-volatile count changed, so nightly runs don't produce noise commits.
The summary moves only on *verified* progress (promoted functions, complete
units); WIP fuzzy percentages live in the report artifact alone.

