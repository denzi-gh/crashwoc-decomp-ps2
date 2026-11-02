/*
 * Unit: sdk/newlib/libc/machine/r5900/memcmp
 *
 * Hand-written assembly in retail (memcmp.S): each function is one
 * asm body (docs/decomp_agent.md, "Inline assembly").
 *
 * Functions:
 *   0x00270de8 memcmp
 */


__asm__(
    ".text\n"
    ".globl  memcmp\n"
    ".ent    memcmp\n"
    ".set    noreorder\n"
    ".set    noat\n"
    "memcmp:\n"
    "    sltiu   $2, $6, 0x10\n"
    "    bnez    $2, .Lmemcmp_2\n"
    "     or      $2, $4, $5\n"
    "    andi    $2, $2, 0xF\n"
    "    bnez    $2, .Lmemcmp_2\n"
    "     nop\n"
    ".Lmemcmp_1:\n"
    "    lq      $3, 0x0($4)\n"
    "    sltiu   $7, $6, 0x20\n"
    "    lq      $2, 0x0($5)\n"
    "    addiu   $4, $4, 0x10\n"
    "    pxor    $8, $2, $3\n"
    "    addiu   $2, $5, 0x10\n"
    "    pcpyud  $10, $8, $7\n"
    "    or      $9, $10, $8\n"
    "    movz    $5, $2, $9\n"
    "    bnel    $9, $0, .Lmemcmp_2\n"
    "     addiu   $4, $4, -0x10\n"
    "    beqz    $7, .Lmemcmp_1\n"
    "     addiu   $6, $6, -0x10\n"
    ".Lmemcmp_2:\n"
    "    lui     $2, (0xFFFFFFFF >> 16)\n"
    "    addiu   $6, $6, -0x1\n"
    "    ori     $2, $2, (0xFFFFFFFF & 0xFFFF)\n"
    "    beq     $6, $2, .Lmemcmp_5\n"
    "     nop\n"
    "    lui     $7, (0xFFFFFFFF >> 16)\n"
    "    ori     $7, $7, (0xFFFFFFFF & 0xFFFF)\n"
    ".Lmemcmp_3:\n"
    "    lbu     $3, 0x0($4)\n"
    "    lbu     $2, 0x0($5)\n"
    "    beq     $3, $2, .Lmemcmp_4\n"
    "     addiu   $4, $4, 0x1\n"
    "    jr      $31\n"
    "     subu    $2, $3, $2\n"
    ".Lmemcmp_4:\n"
    "    addiu   $6, $6, -0x1\n"
    "    bne     $6, $7, .Lmemcmp_3\n"
    "     addiu   $5, $5, 0x1\n"
    ".Lmemcmp_5:\n"
    "    jr      $31\n"
    "     daddu   $2, $0, $0\n"
    ".set    at\n"
    ".set    reorder\n"
    ".end    memcmp\n");
