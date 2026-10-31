/*
 * Unit: nu3d/asmgobj
 *
 * Hand-written assembly in retail (asmgobj.s): each function is one
 * asm body (docs/decomp_agent.md, "Inline assembly").
 *
 * Functions:
 *   0x00158fb8 NuGobjColourBlend
 *   0x00159010 NuGobjColourBlendGobj
 */


__asm__(
    ".text\n"
    ".globl  NuGobjColourBlend\n"
    ".ent    NuGobjColourBlend\n"
    ".set    noreorder\n"
    ".set    noat\n"
    "NuGobjColourBlend:\n"
    "    addiu   $10, $0, 0xFF\n"
    "    beqz    $6, .LNuGobjColourBlend_1\n"
    "     pextlb  $8, $0, $4\n"
    "    beq     $6, $10, .LNuGobjColourBlend_2\n"
    "     pextlb  $9, $0, $5\n"
    "    sub     $10, $10, $6\n"
    "    pextlh  $6, $6, $6\n"
    "    pextlh  $10, $10, $10\n"
    "    pextlh  $6, $6, $6\n"
    "    pextlh  $10, $10, $10\n"
    "    pmulth  $8, $8, $10\n"
    "    pmaddh  $9, $9, $6\n"
    "    pmfhl.lh $8\n"
    "    psrlh   $8, $8, 8\n"
    "    ppacb   $2, $0, $8\n"
    "    jr      $31\n"
    "     nop\n"
    ".LNuGobjColourBlend_1:\n"
    "    jr      $31\n"
    "     add     $2, $0, $4\n"
    ".LNuGobjColourBlend_2:\n"
    "    jr      $31\n"
    "     add     $2, $0, $5\n"
    ".set    at\n"
    ".set    reorder\n"
    ".end    NuGobjColourBlend\n");


__asm__(
    ".text\n"
    ".align  3\n"
    ".globl  NuGobjColourBlendGobj\n"
    ".ent    NuGobjColourBlendGobj\n"
    ".set    noreorder\n"
    ".set    noat\n"
    "NuGobjColourBlendGobj:\n"
    "    addiu   $12, $0, 0xFF\n"
    "    sub     $12, $12, $5\n"
    "    pextlh  $5, $5, $5\n"
    "    pextlh  $12, $12, $12\n"
    "    pextlh  $5, $5, $5\n"
    "    pextlh  $12, $12, $12\n"
    "    lw      $9, 0x4($4)\n"
    "    addi    $2, $0, 0x0\n"
    ".LNuGobjColourBlendGobj_1:\n"
    "    lw      $8, 0x0($4)\n"
    "    addiu   $4, $4, 0x8\n"
    "    beqz    $9, .LNuGobjColourBlendGobj_3\n"
    "     lw      $10, 0x0($4)\n"
    "    lw      $11, 0x4($4)\n"
    "    add     $2, $2, $9\n"
    ".LNuGobjColourBlendGobj_2:\n"
    "    addiu   $4, $4, 0x8\n"
    "    addi    $9, $9, -0x1\n"
    "    pextlb  $10, $0, $10\n"
    "    pextlb  $11, $0, $11\n"
    "    pmulth  $10, $10, $12\n"
    "    pmaddh  $11, $11, $5\n"
    "    pmfhl.lh $13\n"
    "    psrlh   $13, $13, 8\n"
    "    ppacb   $13, $0, $13\n"
    "    lw      $10, 0x0($4)\n"
    "    lw      $11, 0x4($4)\n"
    "    sw      $13, 0x0($8)\n"
    "    bnez    $9, .LNuGobjColourBlendGobj_2\n"
    "     addi    $8, $8, 0x4\n"
    "    b       .LNuGobjColourBlendGobj_1\n"
    "     lw      $9, 0x4($4)\n"
    ".LNuGobjColourBlendGobj_3:\n"
    "    jr      $31\n"
    "     nop\n"
    ".set    at\n"
    ".set    reorder\n"
    ".end    NuGobjColourBlendGobj\n");
