# Self-hosted runner for the protected `matching` workflow

The public `validate.yml` workflow runs on GitHub's own runners and never
touches game files. Everything byte-gated — expected objects, the matching
image SHA, promoted-function verification, the progress report — needs the
retail ELF and the Linux-only toolchain, so `matching.yml` runs on a
self-hosted runner: a machine you control that holds the game files.
The game bytes never leave that machine; the only uploads are the
whitelist-sanitized report and the small committed progress baseline.

## Requirements

- Windows or Linux, with:
  - **Docker** (Docker Desktop on Windows) — or podman with the
    `CRASHWOC_ENGINE=podman` environment variable set for the runner.
  - **Python 3.11+** on `PATH` as `python`.
  - **git** on `PATH`.
- ~10 GiB free disk (container image, toolchain, build tree).
- A legally obtained copy of the PAL v1.03 game ELF (`SLES_503.86`).

The workflow's build commands all go through `python tools/dispatch.py`,
which starts and execs into a long-lived dev container — the same mechanism
used for local development, on both Windows and Linux hosts. The container
name embeds a fingerprint of the checkout path, so the runner's workspace
and a developer working copy on the same machine never collide.
(A Linux machine with the toolchain installed natively can set
`CRASHWOC_DIRECT=1` to skip the container.)

## Registering the runner

1. Repository → **Settings → Actions → Runners → New self-hosted runner**.
2. Follow GitHub's platform instructions to install and configure the
   runner agent.
3. When asked for labels, add **`crashwoc`** (the workflow targets
   `[self-hosted, crashwoc]`).
4. Run the agent as a service so nightly runs work unattended
   (`./svc.sh install` on Linux, "Run as service" option on Windows).

## Providing the game files

Two options — the workflow checks them in this order:

1. **Pre-place them in the workspace** (one-time): after the first checkout
   the runner's work folder contains the repo; copy the ELF to
   `orig/pal103/SLES_503.86` there. The workflow checks out with
   `clean: false`, so gitignored directories (`orig/`, `compiler/`,
   `tools/download/`, `build/`) survive between runs.
2. **`CRASHWOC_ORIG_DIR`**: set this environment variable for the runner
   process to a directory containing the game files; the workflow copies
   its contents into `orig/pal103/` whenever the ELF is missing (e.g. after
   a fresh workspace). For a service runner, put it in the runner's `.env`
   file (`CRASHWOC_ORIG_DIR=D:\games\crashwoc\pal103`) and restart the
   service.

`verify_target.py` hash-checks the ELF against the committed registry on
every run, so a wrong or corrupted file fails immediately.

## Security invariants

- **`matching.yml` must never gain a `pull_request` trigger.** It runs only
  on pushes to `main`, the nightly schedule, and manual dispatch. Fork PRs
  get `validate.yml` on GitHub-hosted runners only. Review any change to
  the workflow's `on:` block with this in mind.
- In **Settings → Actions → General**, keep "Require approval for all
  external contributors" (or stricter) enabled — defense in depth for the
  runner even though no PR-triggered workflow targets it.
- The runner account needs no repository secrets beyond the default
  `GITHUB_TOKEN` (used to upload the report artifact and bot-commit
  `progress/summary.json`).
- Nothing under `orig/`, `asm/`, `expected/`, `build/` (other than the
  sanitized `build/publish/report.json`) is ever uploaded; the public
  `validate.yml` separately enforces that no game-derived file is tracked
  in git.

## First run and maintenance

- Trigger the first run manually: **Actions → matching → Run workflow**.
  It builds the container image, downloads and fingerprint-verifies the
  locked toolchain into `compiler/`, then runs the full gate sequence.
  Later runs reuse all of that and are incremental (ninja).
- A red run means a real gate failed: target hash, registry `--check`,
  image SHA, a promoted function's bytes, or a progress regression.
  The failing step's log names the exact gate; nothing is uploaded and the
  baseline is not moved on a red run.
- To reset the runner state, delete `build/` (safe, regenerated) —
  or the whole work folder; with `CRASHWOC_ORIG_DIR` set, even a fresh
  workspace re-provisions itself.
