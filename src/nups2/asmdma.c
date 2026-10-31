/*
 * Unit: nups2/asmdma
 *
 * Hand-written assembly in retail (asmdma.s): each function is one
 * asm body (docs/decomp_agent.md, "Inline assembly").
 *
 * Functions:
 *   0x0016ece4 vpVifAlign128
 *   0x0016ed00 vpDmaTag_CntEx
 *   0x0016ed28 vpDmaTag_CallEx
 *   0x0016ed50 vpDmaTag_Call
 *   0x0016ed74 vpDmaTag_RefEx
 *   0x0016ed9c vpDmaTag_Ret
 *   0x0016edc0 vpDmaTag_RetEx
 *   0x0016ede8 vpDmaTag_Next
 *   0x0016ee0c vpDmaTag_NextEx
 *   0x0016ee34 vpDmaTag_Copy
 *   0x0016ee44 vpDmaTag_RepointCopy
 *   0x0016ee60 vpDmaTag_ChangeCopy
 *   0x0016ef58 vpDmaTag_Close
 *   0x0016ef78 vpVifAlign64
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
    ".globl  vpVifAlign128\n"
    ".ent    vpVifAlign128\n"
    ".set    noreorder\n"
    ".set    noat\n"
    ".LvpVifAlign128_1:\n"
    "    jr      $31\n"
    "     addi    $2, $4, 0x0\n"
    "    nop\n"
    "    nop\n"
    ".LvpVifAlign128_2:\n"
    "    sw      $0, 0x0($4)\n"
    "    sd      $0, 0x4($4)\n"
    "    jr      $31\n"
    "     addi    $2, $4, 0xC\n"
    ".LvpVifAlign128_3:\n"
    "    sd      $0, 0x0($4)\n"
    "    jr      $31\n"
    "     addi    $2, $4, 0x8\n"
    "    nop\n"
    ".LvpVifAlign128_4:\n"
    "    sw      $0, 0x0($4)\n"
    "    jr      $31\n"
    "     addi    $2, $4, 0x4\n"
    "    nop\n"
    "vpVifAlign128:\n"
    "    lui     $9, %hi(.LvpVifAlign128_1)\n"
    "    andi    $8, $4, 0xF\n"
    "    addi    $9, $9, %lo(.LvpVifAlign128_1)\n"
    "    sll     $8, $8, 2\n"
    "    add     $9, $9, $8\n"
    "    jr      $9\n"
    "     nop\n"
    ".set    at\n"
    ".set    reorder\n"
    ".end    vpVifAlign128\n");


__asm__(
    ".text\n"
    ".globl  vpDmaTag_CntEx\n"
    ".ent    vpDmaTag_CntEx\n"
    ".set    noreorder\n"
    ".set    noat\n"
    "vpDmaTag_CntEx:\n"
    "    lui     $8, %hi(vpdmatag_curr)\n"
    "    sw      $4, %lo(vpdmatag_curr)($8)\n"
    "    addiu   $9, $0, 0x1\n"
    "    sll     $9, $9, 28\n"
    "    pextlw  $9, $5, $9\n"
    "    pextlw  $10, $6, $0\n"
    "    pextlw  $9, $10, $9\n"
    "    sq      $9, 0x0($4)\n"
    "    jr      $31\n"
    "     addiu   $2, $4, 0x10\n"
    ".set    at\n"
    ".set    reorder\n"
    ".end    vpDmaTag_CntEx\n");


__asm__(
    ".text\n"
    ".globl  vpDmaTag_CallEx\n"
    ".ent    vpDmaTag_CallEx\n"
    ".set    noreorder\n"
    ".set    noat\n"
    "vpDmaTag_CallEx:\n"
    "    lui     $8, %hi(vpdmatag_curr)\n"
    "    sw      $4, %lo(vpdmatag_curr)($8)\n"
    "    addiu   $9, $0, 0x5\n"
    "    sll     $9, $9, 28\n"
    "    pextlw  $9, $6, $9\n"
    "    pextlw  $10, $7, $5\n"
    "    pextlw  $9, $10, $9\n"
    "    sq      $9, 0x0($4)\n"
    "    jr      $31\n"
    "     addiu   $2, $4, 0x10\n"
    ".set    at\n"
    ".set    reorder\n"
    ".end    vpDmaTag_CallEx\n");


__asm__(
    ".text\n"
    ".globl  vpDmaTag_Call\n"
    ".ent    vpDmaTag_Call\n"
    ".set    noreorder\n"
    ".set    noat\n"
    "vpDmaTag_Call:\n"
    "    lui     $8, %hi(vpdmatag_curr)\n"
    "    sw      $4, %lo(vpdmatag_curr)($8)\n"
    "    addiu   $9, $0, 0x5\n"
    "    sll     $9, $9, 28\n"
    "    pextlw  $9, $5, $9\n"
    "    pcpyld  $9, $0, $9\n"
    "    sq      $9, 0x0($4)\n"
    "    jr      $31\n"
    "     addiu   $2, $4, 0x10\n"
    ".set    at\n"
    ".set    reorder\n"
    ".end    vpDmaTag_Call\n");


__asm__(
    ".text\n"
    ".globl  vpDmaTag_RefEx\n"
    ".ent    vpDmaTag_RefEx\n"
    ".set    noreorder\n"
    ".set    noat\n"
    "vpDmaTag_RefEx:\n"
    "    lui     $8, %hi(vpdmatag_curr)\n"
    "    sw      $4, %lo(vpdmatag_curr)($8)\n"
    "    addiu   $9, $0, 0x3\n"
    "    sll     $9, $9, 28\n"
    "    pextlw  $9, $6, $9\n"
    "    pextlw  $10, $7, $5\n"
    "    pextlw  $9, $10, $9\n"
    "    sq      $9, 0x0($4)\n"
    "    jr      $31\n"
    "     addiu   $2, $4, 0x10\n"
    ".set    at\n"
    ".set    reorder\n"
    ".end    vpDmaTag_RefEx\n");


__asm__(
    ".text\n"
    ".globl  vpDmaTag_Ret\n"
    ".ent    vpDmaTag_Ret\n"
    ".set    noreorder\n"
    ".set    noat\n"
    "vpDmaTag_Ret:\n"
    "    lui     $8, %hi(vpdmatag_curr)\n"
    "    sw      $4, %lo(vpdmatag_curr)($8)\n"
    "    addiu   $9, $0, 0x6\n"
    "    sll     $9, $9, 28\n"
    "    pextlw  $9, $0, $9\n"
    "    pcpyld  $9, $0, $9\n"
    "    sq      $9, 0x0($4)\n"
    "    jr      $31\n"
    "     addiu   $2, $4, 0x10\n"
    ".set    at\n"
    ".set    reorder\n"
    ".end    vpDmaTag_Ret\n");


__asm__(
    ".text\n"
    ".globl  vpDmaTag_RetEx\n"
    ".ent    vpDmaTag_RetEx\n"
    ".set    noreorder\n"
    ".set    noat\n"
    "vpDmaTag_RetEx:\n"
    "    lui     $8, %hi(vpdmatag_curr)\n"
    "    sw      $4, %lo(vpdmatag_curr)($8)\n"
    "    addiu   $9, $0, 0x6\n"
    "    sll     $9, $9, 28\n"
    "    pextlw  $9, $5, $9\n"
    "    pextlw  $10, $6, $0\n"
    "    pextlw  $9, $10, $9\n"
    "    sq      $9, 0x0($4)\n"
    "    jr      $31\n"
    "     addiu   $2, $4, 0x10\n"
    ".set    at\n"
    ".set    reorder\n"
    ".end    vpDmaTag_RetEx\n");


__asm__(
    ".text\n"
    ".globl  vpDmaTag_Next\n"
    ".ent    vpDmaTag_Next\n"
    ".set    noreorder\n"
    ".set    noat\n"
    "vpDmaTag_Next:\n"
    "    lui     $8, %hi(vpdmatag_curr)\n"
    "    sw      $4, %lo(vpdmatag_curr)($8)\n"
    "    addiu   $9, $0, 0x2\n"
    "    sll     $9, $9, 28\n"
    "    pextlw  $9, $5, $9\n"
    "    pcpyld  $9, $0, $9\n"
    "    sq      $9, 0x0($4)\n"
    "    jr      $31\n"
    "     addiu   $2, $4, 0x10\n"
    ".set    at\n"
    ".set    reorder\n"
    ".end    vpDmaTag_Next\n");


__asm__(
    ".text\n"
    ".globl  vpDmaTag_NextEx\n"
    ".ent    vpDmaTag_NextEx\n"
    ".set    noreorder\n"
    ".set    noat\n"
    "vpDmaTag_NextEx:\n"
    "    lui     $8, %hi(vpdmatag_curr)\n"
    "    sw      $4, %lo(vpdmatag_curr)($8)\n"
    "    addiu   $9, $0, 0x2\n"
    "    sll     $9, $9, 28\n"
    "    pextlw  $9, $6, $9\n"
    "    pextlw  $10, $7, $5\n"
    "    pextlw  $9, $10, $9\n"
    "    sq      $9, 0x0($4)\n"
    "    jr      $31\n"
    "     addiu   $2, $4, 0x10\n"
    ".set    at\n"
    ".set    reorder\n"
    ".end    vpDmaTag_NextEx\n");


__asm__(
    ".text\n"
    ".globl  vpDmaTag_Copy\n"
    ".ent    vpDmaTag_Copy\n"
    ".set    noreorder\n"
    ".set    noat\n"
    "vpDmaTag_Copy:\n"
    "    lq      $8, 0x0($5)\n"
    "    sq      $8, 0x0($4)\n"
    "    jr      $31\n"
    "     addiu   $2, $4, 0x10\n"
    ".set    at\n"
    ".set    reorder\n"
    ".end    vpDmaTag_Copy\n");


__asm__(
    ".text\n"
    ".globl  vpDmaTag_RepointCopy\n"
    ".ent    vpDmaTag_RepointCopy\n"
    ".set    noreorder\n"
    ".set    noat\n"
    "vpDmaTag_RepointCopy:\n"
    "    lq      $8, 0x0($5)\n"
    "    pextlw  $9, $6, $8\n"
    "    pcpyud  $8, $8, $8\n"
    "    pcpyld  $8, $8, $9\n"
    "    sq      $8, 0x0($4)\n"
    "    jr      $31\n"
    "     addiu   $2, $4, 0x10\n"
    ".set    at\n"
    ".set    reorder\n"
    ".end    vpDmaTag_RepointCopy\n");


__asm__(
    ".text\n"
    ".globl  vpDmaTag_ChangeCopy\n"
    ".ent    vpDmaTag_ChangeCopy\n"
    ".set    noreorder\n"
    ".set    noat\n"
    "vpDmaTag_ChangeCopy:\n"
    "    ld      $8, 0x8($5)\n"
    "    pextlw  $9, $7, $6\n"
    "    pcpyld  $8, $8, $9\n"
    "    sq      $8, 0x0($4)\n"
    "    jr      $31\n"
    "     addiu   $2, $4, 0x10\n"
    ".LvpDmaTag_ChangeCopy_1:\n"
    "    addi    $2, $4, 0x0\n"
    "    subu    $9, $2, $8\n"
    "    srl     $9, $9, 4\n"
    "    addi    $9, $9, -0x1\n"
    "    sh      $9, 0x0($8)\n"
    "    jr      $31\n"
    "     nop\n"
    "    nop\n"
    "    nop\n"
    "    nop\n"
    "    nop\n"
    "    nop\n"
    "    nop\n"
    "    nop\n"
    "    nop\n"
    "    nop\n"
    ".LvpDmaTag_ChangeCopy_2:\n"
    "    sw      $0, 0x0($4)\n"
    "    sd      $0, 0x4($4)\n"
    "    addi    $2, $4, 0xC\n"
    "    subu    $9, $2, $8\n"
    "    srl     $9, $9, 4\n"
    "    addi    $9, $9, -0x1\n"
    "    sh      $9, 0x0($8)\n"
    "    jr      $31\n"
    "     nop\n"
    "    nop\n"
    "    nop\n"
    "    nop\n"
    "    nop\n"
    "    nop\n"
    "    nop\n"
    "    nop\n"
    ".LvpDmaTag_ChangeCopy_3:\n"
    "    sd      $0, 0x0($4)\n"
    "    addi    $2, $4, 0x8\n"
    "    subu    $9, $2, $8\n"
    "    srl     $9, $9, 4\n"
    "    addi    $9, $9, -0x1\n"
    "    sh      $9, 0x0($8)\n"
    "    jr      $31\n"
    "     nop\n"
    "    nop\n"
    "    nop\n"
    "    nop\n"
    "    nop\n"
    "    nop\n"
    "    nop\n"
    "    nop\n"
    "    nop\n"
    ".LvpDmaTag_ChangeCopy_4:\n"
    "    sw      $0, 0x0($4)\n"
    "    addi    $2, $4, 0x4\n"
    "    subu    $9, $2, $8\n"
    "    srl     $9, $9, 4\n"
    "    addi    $9, $9, -0x1\n"
    "    sh      $9, 0x0($8)\n"
    "    jr      $31\n"
    "     nop\n"
    ".set    at\n"
    ".set    reorder\n"
    ".end    vpDmaTag_ChangeCopy\n");


__asm__(
    ".text\n"
    ".globl  vpDmaTag_Close\n"
    ".ent    vpDmaTag_Close\n"
    ".set    noreorder\n"
    ".set    noat\n"
    "vpDmaTag_Close:\n"
    "    lui     $8, %hi(vpdmatag_curr)\n"
    "    lwu     $8, %lo(vpdmatag_curr)($8)\n"
    "    lui     $9, %hi(func_0016EE78)\n"
    "    andi    $10, $4, 0xF\n"
    "    addi    $9, $9, %lo(func_0016EE78)\n"
    "    sll     $10, $10, 4\n"
    "    add     $9, $9, $10\n"
    "    jr      $9\n"
    ".set    at\n"
    ".set    reorder\n"
    ".end    vpDmaTag_Close\n");


__asm__(
    ".text\n"
    ".globl  vpVifAlign64\n"
    ".ent    vpVifAlign64\n"
    ".set    noreorder\n"
    ".set    noat\n"
    "vpVifAlign64:\n"
    "    add     $2, $4, $0\n"
    "    andi    $8, $2, 0x7\n"
    "    nop\n"
    "    beq     $0, $8, .LvpVifAlign64_2\n"
    "     nop\n"
    ".LvpVifAlign64_1:\n"
    "    sw      $0, 0x0($2)\n"
    "    addiu   $2, $2, 0x4\n"
    "    andi    $8, $2, 0x7\n"
    "    nop\n"
    "    .word   0x1408FFFB              # bne        $0, $8, .LvpVifAlign64_1\n"
    "     nop\n"
    ".LvpVifAlign64_2:\n"
    "    jr      $31\n"
    "     nop\n"
    ".set    at\n"
    ".set    reorder\n"
    ".end    vpVifAlign64\n");
