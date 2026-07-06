/* Binary-baseline linker script for the PAL retail v1.03 target (SLES_503.86).
 *
 * This is the identity reconstruction: before any code is decompiled, prove the
 * raw loaded image can be rebuilt from per-section objects placed at their exact
 * virtual addresses. tools/build_baseline.py extracts each loaded section into a
 * gitignored `.bin` blob, wraps it in a `.incbin` object (`build/baseline/*.s`),
 * and either links it with this script (real ee-ld) or reconstructs the loaded
 * image in pure Python from these same addresses. Either way the result must
 * equal the target's PT_LOAD image byte-for-byte.
 *
 * Nothing here is game-derived: only virtual addresses and sizes, which are the
 * same facts recorded in config/pal103/sections.json. The bytes live in the
 * gitignored objects this script consumes.
 *
 * Layout of the single PT_LOAD segment (file 0x1000 -> vram 0x00100000, memsz
 * 0x0060698C). The loaded PROGBITS sections tile the file image contiguously
 * except for three all-zero alignment gaps (before .rodata, .lit4, .sdata) that
 * the linker recreates by advancing the location counter. One 0x80 run between
 * .vutext and .data holds MIPS bytes described by no section header; it is
 * carried as an explicit `.orphan` object so no loaded byte is invented.
 */

OUTPUT_ARCH(mips)

/* No ENTRY: the baseline reconstructs the loaded image only, and objcopy
 * extracts it by section, so the entry point (0x00100008) is immaterial here. */

SECTIONS
{
    . = 0x00100000;
    .text    : { *(.baseline.text)   }   /* 0x00100000  size 0x181530 */

    . = 0x00281530;
    .vutext  : { *(.baseline.vutext) }   /* 0x00281530  size 0x012150  VU microcode */

    . = 0x00293680;
    .orphan  : { *(.baseline.orphan) }   /* 0x00293680  size 0x000080  un-sectioned MIPS bytes */

    . = 0x00293700;
    .data    : { *(.baseline.data)   }   /* 0x00293700  size 0x37FD28 */

    . = 0x00613480;                      /* 0x58 zero gap after .data */
    .rodata  : { *(.baseline.rodata) }   /* 0x00613480  size 0x0194A0 */

    . = 0x0062C980;                      /* 0x60 zero gap after .rodata */
    .lit4    : { *(.baseline.lit4)   }   /* 0x0062C980  size 0x001FF8 */

    . = 0x0062E980;                      /* 0x08 zero gap after .lit4 */
    .sdata   : { *(.baseline.sdata)  }   /* 0x0062E980  size 0x00463C, ends at filesz 0x00533FBC */

    /* NOBITS: no file bytes, but they define the loaded memory size (memsz). */
    . = 0x00633000;
    .sbss (NOLOAD) : { . += 0x0003D4; }  /* 0x00633000  size 0x0003D4 */
    . = 0x00633400;
    .bss  (NOLOAD) : { . += 0x0D358C; }  /* 0x00633400  size 0x0D358C, ends at memsz 0x0070698C */

    _gp = 0x00634970;

    /* The baseline reconstructs the loaded image only; drop everything else
     * (comments, symbol tables, debug) rather than inventing its placement. */
    /DISCARD/ : { *(.pdr) *(.reginfo) *(.mdebug*) *(.comment) *(.gnu.attributes) }
}
