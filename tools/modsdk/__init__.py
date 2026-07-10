"""Fixed-layout modding SDK for the PAL v1.03 retail ELF.

Retail code/data stay byte-identical at retail addresses (no relocation,
no stale pointers). Mod C links at a fixed base above the image end and is
wired in by j-trampoline hooks; the game heap is patched past the blob.
See docs/modding-sdk.md.
"""
