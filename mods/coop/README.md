# coop

In-game half of the coop mod. Each PCSX2 instance runs this mod; a PC
bridge (the `crashwoc-multiplayer` repo) copies state between the two
instances over PINE. Nobody is bound to the same room: each slot carries
the writer's level id, and the puppet only shows when both match.

Current scope (PR 1+2): publish the local player's state into the mailbox
every frame, consume the bridge-written remote slot into a snapshot, and
draw the remote player as a puppet. The puppet is a mod-owned creature
record fed to the retail renderer by a replace hook on `DrawCreatures`
(never registered with the engine, so it has no physics, AI, or
collisions) and it is shown only while both sides report the same level
and the remote keeps ticking (staleness ~0.6 s). The bridge lives in the
`crashwoc-multiplayer` repo: `python -m crashwoc_multiplayer coop-bridge`
(and `coop-peek` for a one-shot mailbox dump).

Known MVP limits: animations are hard-set (no blending, slight popping on
action changes) and vehicles are not represented (the puppet renders on
foot everywhere). The puppet does cast the flat ground shadow (from the
remote's transmitted shadow height) and the hub floor reflection.

## Build and boot

```
python tools/dispatch.py python tools/modsdk/build.py mods/coop/mod.toml
pcsx2-qt.exe -batch -elf <repo>\build\pal103\mods\mod.elf -- <game>.bin
```

coop and hello both hook `DoInput`, so build one or the other.

## Mailbox map

The contract is [mods/include/coop_mailbox.h](../include/coop_mailbox.h)
(seq-lock protocol documented there). Fixed addresses with the default
blob layout:

| address    | what                                            |
| ---------- | ----------------------------------------------- |
| `0x706A40` | SDK mailbox header (`CWMB`, version, frame)     |
| `0x706A4C` | diagnostics: last accepted remote seq           |
| `0x706A50` | diagnostics: staleness counter (frames)         |
| `0x706A60` | coop magic `CWCO` + version                     |
| `0x706A68` | ctl (PC writes): bit 0 = local-ghost self-test  |
| `0x706A6C` | diag: bit 0 = puppet shown, bits 8+ = re-inits  |
| `0x706A70` | local slot (game writes, bridge reads), 0x50 B  |
| `0x706AC0` | remote slot (bridge writes, game reads), 0x50 B |

## Ghost self-test (PR 2, no bridge needed)

Write `1` to `0x706A68` over PINE: the mod mirrors the local player into
the remote snapshot at `pos.x + 2.0`, so a second Crash shadows the
player through hubs, levels, deaths, and menus. Write `0` to turn it
off. `0x706A6C` bit 0 reports whether the puppet is being drawn.

## Verifying PR 1

Headless over PINE (enable it in PCSX2; TCP 127.0.0.1:28011): check coop
magic/version at `0x706A60`, confirm the local slot's `seq_open` ==
`seq_close` and both seq and frame advance, then write a seq-bracketed
remote slot (`seq_open` first, `seq_close` last, same generation) and
confirm the generation appears at `0x706A4C` and the staleness counter
at `0x706A50` resets. On the menus the local slot reports `level = -1`,
`flags = 0`; in a level it carries the level id, `COOP_F_PRESENT`, and a
position that tracks movement (that part needs someone at the pad).
