# Modding SDK

Builds mods against PAL retail v1.03 by patching the retail ELF instead of
relinking it. Retail code and data never move: mod code is compiled
separately, placed in a second PT_LOAD segment above the retail image, and
wired in with jump trampolines over function prologues. Since nothing
relocates, no pointer baked into retail data can go stale. That pointer
problem is what killed the earlier shift/relink build on the
`modding-functionality` branch, which crashed at level load.

```
manifests (mod.toml)        tools/modsdk/build.py
  code (.c)          --->     compile (locked EE GCC, -O2 -G0)
  hooks                       link at fixed base (all 6.3k retail symbols)
  data patches                trampolines over retail prologues
  mailbox                     heap carve + blob PT_LOAD + verify
                            build/pal103/mods/mod.elf  (boots in PCSX2)
```

## Building

```bash
# hello-hook proof mod
python tools/dispatch.py python tools/modsdk/build.py mods/hello/mod.toml

# no mods -> output must be byte-identical to retail (the identity gate)
python tools/modsdk/build.py -o build/pal103/mods/identity.elf

# structure tests, no game files needed
python -m unittest tests.test_modsdk -v
```

To boot the result in PCSX2, either add `build/pal103/mods/mod.elf` as a
game-list ELF entry with Disc Path set to the retail image, or run it from
the command line. Pass the `.bin`, not the `.cue` (the CLI CDVD open
rejects cue sheets):

```
pcsx2-qt.exe -batch -elf <repo>\build\pal103\mods\mod.elf -- <game>.bin
```

No disc rebuild is needed for development.

The hello mod is the end-to-end check (verified 2026-07-10): the game
boots and plays, mailbox magic/version appear at `0x706A40`, the frame
counter advances at ~50 Hz, and the wumpa HUD counter ticks up once per
second. Readable live over PINE (enable it in PCSX2 settings; TCP
`127.0.0.1:28011`, `MsgRead32` is opcode 2).

## Memory layout

The loaded image ends at `0x0070698C` (PT_LOAD vaddr `0x00100000` + memsz
`0x0060698C`); user RAM runs to `0x02000000`. The game allocates through
`NuMemAlloc → malloc → sbrk` (newlib). `sbrk` keeps its break in a word at
`__ps2_klibinfo__ + 0x14` (= `0x00612BBC`), initialized to exactly the
image end, and caps it with the kernel `EndOfHeapMemory` syscall
(`EndOfHeap`, `0x00278C80`, is `li v1,0x3E; syscall`).

So carving RAM for the mod blob takes one word: the blob sits at
`0x00706A00` (image end aligned up) and the builder patches the initial
break to the first 16-aligned address past the blob. The game's heap
starts after the mod and nothing else changes. This held up in practice:
with the hello mod loaded, the game's sbrk break grew from the patched
value (observed around `0x01E04000`) and the blob stayed intact.
`build.py` re-reads the retail break value on every build and refuses to
patch if it isn't the image end.

Blob layout:

| where                    | what                                        |
| ------------------------ | ------------------------------------------- |
| `blob_base + 0x00`       | header: `"CWMS"`, version, base, end, mailbox addr, mod count |
| `blob_base + 0x40`       | mailbox (if any mod declares one)           |
| then, per mod (in order) | stub area, then the linked C image (text/rodata/data/bss) |

## Hooks

`[[hook]]` rewires a retail function `T` to a mod handler. Two modes:

- `pre`: `T` starts with `j stub; nop`. The stub saves `$ra` and the
  integer argument registers `a0–a3/t0–t3`, calls the handler, restores,
  replays the two displaced prologue instructions, and resumes at `T+8`.
  The hooked function then runs unchanged. Float argument registers are
  not preserved, so don't pre-hook functions that take float args.
- `replace`: `T` starts with `j handler; nop`. The handler runs instead
  of `T` and returns straight to the caller. The builder exports
  `orig_<T>` (a thunk of the displaced words plus `j T+8`) so the handler
  can still call the original.

The two displaced prologue words must be position-independent;
`mips.is_relocatable` rejects branches, jumps and syscalls, and the
builder refuses the hook. Two mods hooking the same function is a build
error (no chaining yet).

Mod C is compiled with the locked EE GCC at `-O2 -G0`. `-G0` because mod
code cannot share retail's `$gp` window, so every global access has to be
absolute. profiles.toml is untouched; its fingerprint gates
`compare_progress`, which is why the SDK carries its own flags in
`tools/modsdk/build.py`.

## Calling the engine

Every symbol in `config/pal103/symbol_addrs.txt` is available by name:
declare it in [mods/include/retail.h](../mods/include/retail.h) and call
it. The link script `PROVIDE`s all 6.3k addresses. Grow the header as
needed; the `src/` decomp is the ground truth for signatures.

## Mailbox

One mod per build may declare `[mailbox]`. The region lands at a fixed
address (`0x00706A40` with the default layout), recorded in the blob
header. [mods/include/mailbox.h](../mods/include/mailbox.h) is the layout
contract for the PC side: game mods write local state into the region,
the PINE bridge (the `crashwoc-multiplayer` repo) reads it and writes
remote state back. Bump the version field on any layout change.

## Pitfalls

Learned the hard way while bringing up the hello mod; the tools now
enforce or document all three.

1. The blob's program header must extend the phdr table in place at
   `e_phoff 0x34`, overwriting retail's unloaded debris bytes. Moving the
   table to the end of the file looks valid but PCSX2 silently ignores
   it, so the blob segment never loads and the hook jumps into zeroed
   RAM (the PC nop-slides to the top of memory, `TLB Miss pc=0x1fff000`,
   and the emulator UI hard-crashes). `elf_patch.py` only extends in
   place now.
2. Hook `DoInput` for an always-on per-frame tick. `UpdateLevel` sounds
   like the obvious choice but only runs inside gameplay levels; on the
   menus (`Level=0x25`) it never fires.
3. Check what a retail global actually means in the consuming assembly
   before writing to it. The mdebug name is not enough: `WUMPACOUNT`
   sounds like the HUD fruit counter but is the live count of `Wumpa[]`
   entities. Incrementing it every frame made the engine iterate far
   past the end of the array, which corrupted memory (wild byte stores
   in the PCSX2 log, then `vif0 force break`) and froze the game the
   moment Crash died. The HUD counter is `plr_wumpas` (u16), and its
   100-wumpa extra-life check is a while-loop, so it copes with any
   increment.

## Not implemented

- BIN/CUE disc patcher. Development boots via PCSX2 host-ELF with Disc
  Path, so there is no urgency. When it happens, the read-only `cue.py` /
  `iso9660.py` / `disc.py` from the old `crashwoc-coop` repo are a
  starting point; the image is raw 2352-byte sectors, so replacing the
  ELF means rewriting EDC/ECC per sector.
- `post` hooks, hook chaining, float-arg preservation. All refused with
  clear errors rather than half-supported.
- Asset and level tooling. Gated on decompiling the loaders
  (`visiLoadData`, `terraininit`, the `edobj`/`edanim`/`edgra` file
  loaders); each decompiled loader doubles as a file-format spec that a
  builder can invert.

## Files

| path                    | role                                        |
| ----------------------- | ------------------------------------------- |
| `tools/modsdk/manifest.py`    | mod.toml schema + validation          |
| `tools/modsdk/gen_symbols.py` | symbol registry → PROVIDE script      |
| `tools/modsdk/mips.py`        | r5900 words for stubs + relocatability check |
| `tools/modsdk/gen_hooks.py`   | trampoline planning/emission          |
| `tools/modsdk/elf_patch.py`   | retail ELF + patches + blob PT_LOAD; identity verify |
| `tools/modsdk/build.py`       | the driver (container for compile/link) |
| `mods/include/*.h`            | mod-facing API headers (committed)    |
| `mods/hello/`                 | end-to-end proof mod                  |
| `tests/test_modsdk.py`        | native structure tests                |
