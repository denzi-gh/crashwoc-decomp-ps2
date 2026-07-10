# coop

In-game half of the coop mod. Each PCSX2 instance runs this mod; a PC
bridge (the `crashwoc-multiplayer` repo) copies state between the two
instances over PINE. Nobody is bound to the same room: each slot carries
the writer's level id, and the puppet only shows when both match.

Current scope (PR 1): publish the local player's state into the mailbox
every frame and consume the bridge-written remote slot into a snapshot.
Puppet rendering is PR 2, the bridge is PR 3.

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
| `0x706A70` | local slot (game writes, bridge reads), 0x50 B  |
| `0x706AC0` | remote slot (bridge writes, game reads), 0x50 B |

## Verifying PR 1

Headless over PINE (enable it in PCSX2; TCP 127.0.0.1:28011): check coop
magic/version at `0x706A60`, confirm the local slot's `seq_open` ==
`seq_close` and both seq and frame advance, then write a seq-bracketed
remote slot (`seq_open` first, `seq_close` last, same generation) and
confirm the generation appears at `0x706A4C` and the staleness counter
at `0x706A50` resets. On the menus the local slot reports `level = -1`,
`flags = 0`; in a level it carries the level id, `COOP_F_PRESENT`, and a
position that tracks movement (that part needs someone at the pad).
