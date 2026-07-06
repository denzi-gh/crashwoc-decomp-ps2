"""Shared library for the reconstruction pipeline.

The proven pipeline logic lives here so every tool builds on the same code:

  toolchain -- locked-toolchain paths, tool resolution, Reporter, small helpers
  asmtext   -- whole-.text disassembly transforms (disambiguate, symbol resolve)
  tu        -- translation-unit splitting of the monolithic .text
  target    -- target ELF loading and loaded-image tiling
  verify    -- link-at-address and per-function byte comparison

The modules were extracted verbatim from the standalone CLIs
(assemble_text.py, split_text.py, build_baseline.py, match.py), which remain
as thin entry points. Behavior is unchanged; the byte-exact gates those CLIs
enforce prove it.
"""
