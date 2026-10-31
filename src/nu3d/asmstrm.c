/*
 * Unit: nu3d/asmstrm
 *
 * Hand-written assembly in retail (asmstrm.s): each function is one
 * asm body (docs/decomp_agent.md, "Inline assembly").
 *
 * Functions:
 *   0x0014c7d4 NuRndrStreamLink
 */

/* The .s source's data, as it wrote it: no C aggregate can be 4-aligned
 * under ee-gcc, and two of these units place it at 4-aligned addresses. */
__asm__(
    ".data\n"
    ".align  2\n"
    ".type   vec_none, @object\n"
    ".size   vec_none, 16\n"
    "vec_none:\n"
    "    .float  0.0, 0.0, 0.0, 0.0\n"
    ".type   vec_one, @object\n"
    ".size   vec_one, 16\n"
    "vec_one:\n"
    "    .float  1.0, 1.0, 1.0, 1.0\n"
    ".type   vec_255, @object\n"
    ".size   vec_255, 16\n"
    "vec_255:\n"
    "    .float  255.0, 255.0, 255.0, 255.0\n"
    ".type   curr_vtx, @object\n"
    ".size   curr_vtx, 4\n"
    "curr_vtx:\n"
    "    .word   0\n"
    ".text\n");


__asm__(
    ".text\n"
    ".globl  NuRndrStreamLink\n"
    ".ent    NuRndrStreamLink\n"
    ".set    noreorder\n"
    ".set    noat\n"
    "NuRndrStreamLink:\n"
    "    lw      $8, 0x0($4)\n"
    "    lui     $9, %hi(rndrstream_free)\n"
    "    beqz    $8, .LNuRndrStreamLink_1\n"
    "     lw      $9, %lo(rndrstream_free)($9)\n"
    "    lw      $8, 0x4($4)\n"
    "    addi    $10, $9, -0x10\n"
    "    sw      $9, 0x4($8)\n"
    "    beq     $10, $8, .LNuRndrStreamLink_2\n"
    "     nop\n"
    "    jr      $31\n"
    "     sw      $9, 0x4($4)\n"
    "    nop\n"
    ".LNuRndrStreamLink_1:\n"
    "    sw      $9, 0x0($4)\n"
    "    lw      $8, 0x4($4)\n"
    "    addi    $10, $9, -0x10\n"
    "    beq     $10, $8, .LNuRndrStreamLink_2\n"
    "     nop\n"
    "    sw      $9, 0x4($4)\n"
    ".LNuRndrStreamLink_2:\n"
    "    jr      $31\n"
    "     nop\n"
    ".set    at\n"
    ".set    reorder\n"
    ".end    NuRndrStreamLink\n");
