# Crash Bandicoot: The Wrath of Cortex Decompilation (PS2)

A byte-exact matching decompilation of **Crash Bandicoot: The Wrath of Cortex**
for the PlayStation 2 - reconstructing the retail executable from hand-written C
that compiles to the exact original bytes.

> **Want to contribute?** Start with [CONTRIBUTING.md](CONTRIBUTING.md).

## Target

All matching work targets exactly one build:

```text
Crash Bandicoot: The Wrath of Cortex
PAL retail v1.03 - SLES-50386 (SLES_503.86)
```

You supply your own legally obtained copies at
`orig/pal103/`; they must match exactly:

| File          | Size (bytes) | SHA-256                                                            |
| ------------- | -----------: | ------------------------------------------------------------------ |
| `SLES_503.86` |    6,731,763 | `7fc6826d08c42a5a92fc018edae3e520783beb5a8330410251192bfe26b1eca1` |
| `SYSTEM.CNF`  |           56 | `86fe327e4576516f57004800b2663c75c8a65a62872f3fdf3aa0618daf23d9c1` |


## Diffing

After initial setup, objdiff.json will be generated and can be used with the objdiff-cli or the objdiff GUI.

## Documentation

| Doc | What it covers |
| --- | -------------- |
| [CONTRIBUTING.md](CONTRIBUTING.md)       | Setup (Docker or native), and how to validate the repo |
| [docs/pipeline.md](docs/pipeline.md)     | The full reconstruction pipeline, toolchain, and CI (fork-safe private-image method) |
| [docs/decomp_agent.md](docs/decomp_agent.md) | The matching MCP server and shared agent workflow |

## Comunity
The Wrath of Cortex Modding Discord Server:
https://discord.gg/QqQtEfKC2V
## Legal

This repository contains no game assets, no game code, and no copyrighted
binaries. It is a clean-room reconstruction workspace: all original files must be
supplied by the user from their own copy of the game.
