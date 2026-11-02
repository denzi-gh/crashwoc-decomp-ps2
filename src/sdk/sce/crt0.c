/*
 * Unit: sdk/sce/crt0
 *
 * Hand-written assembly in retail (crt0.s): each function is one
 * asm body (docs/decomp_agent.md, "Inline assembly").
 *
 * Functions:
 *   0x00100008 _start
 *   0x001000b8 _exit
 *   0x001000c0 _root
 */

/* argc, argv and the argument text handed to main. */
static struct {
    int argc;
    char *argv[16];
    char text[256];
} _args;


__asm__(
    ".text\n"
    ".globl  _start\n"
    ".ent    _start\n"
    ".set    noreorder\n"
    ".set    noat\n"
    ".L_start_1:\n"
    "    nop\n"
    "    nop\n"
    "_start:\n"
    "    lui     $2, %hi(_fbss)\n"
    "    lui     $3, %hi(D_0070698C)     # _end\n"
    "    addiu   $2, $2, %lo(_fbss)\n"
    "    addiu   $3, $3, %lo(D_0070698C) # _end\n"
    ".L_start_2:\n"
    "    sq      $0, 0x0($2)\n"
    "    nop\n"
    "    sltu    $1, $2, $3\n"
    "    nop\n"
    "    nop\n"
    "    bnez    $1, .L_start_2\n"
    "     addiu   $2, $2, 0x10\n"
    "    lui     $4, %hi(_gp)\n"
    "    lui     $5, 0x1F0\n"
    "    lui     $6, %hi(.L_start_1)\n"
    "    lui     $7, %hi(_args)\n"
    "    lui     $8, %hi(_root)\n"
    "    addiu   $4, $4, %lo(_gp)\n"
    "    addiu   $5, $5, (0x1F00000 & 0xFFFF)\n"
    "    addiu   $6, $6, %lo(.L_start_1)\n"
    "    addiu   $7, $7, %lo(_args)\n"
    "    addiu   $8, $8, %lo(_root)\n"
    "    daddu   $28, $4, $0\n"
    "    addiu   $3, $0, 0x3C\n"
    "    syscall 0\n"
    "    daddu   $29, $2, $0\n"
    "    lui     $4, %hi(D_0070698C)     # _end\n"
    "    lui     $5, (0x0 >> 16)\n"
    "    addiu   $4, $4, %lo(D_0070698C) # _end\n"
    "    addiu   $5, $5, -0x1\n"
    "    addiu   $3, $0, 0x3D\n"
    "    syscall 0\n"
    "    jal     _InitSys\n"
    "     nop\n"
    "    jal     FlushCache\n"
    "     daddu   $4, $0, $0\n"
    "    ei\n"
    "    lui     $2, %hi(_args)\n"
    "    addiu   $2, $2, %lo(_args)\n"
    "    lw      $4, 0x0($2)\n"
    "    jal     main\n"
    "     addiu   $5, $2, 0x4\n"
    "    j       Exit\n"
    "     daddu   $4, $2, $0\n"
    ".set    at\n"
    ".set    reorder\n"
    ".end    _start\n");


__asm__(
    ".text\n"
    ".align  3\n"
    ".globl  _exit\n"
    ".ent    _exit\n"
    ".set    noreorder\n"
    ".set    noat\n"
    "_exit:\n"
    "    j       Exit\n"
    "     daddu   $4, $0, $0\n"
    ".set    at\n"
    ".set    reorder\n"
    ".end    _exit\n");


__asm__(
    ".text\n"
    ".globl  _root\n"
    ".ent    _root\n"
    ".set    noreorder\n"
    ".set    noat\n"
    "_root:\n"
    "    addiu   $3, $0, 0x23\n"
    "    syscall 0\n"
    ".set    at\n"
    ".set    reorder\n"
    ".end    _root\n");
