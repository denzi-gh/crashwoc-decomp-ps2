/*
 * Unit: numath/numath
 *
 * Hand-written assembly in retail (numath.s): each function is one
 * asm body (docs/decomp_agent.md, "Inline assembly").
 *
 * Functions:
 *   0x00110394 NuFsqrt
 *   0x001103a0 NuFrsqrt
 *   0x001103ac NuFadda
 *   0x001103b8 NuFsuba
 *   0x001103c4 NuFmula
 *   0x001103d0 NuFmadd
 *   0x001103dc NuFmadda
 *   0x001103e8 NuFmsuba
 */


__asm__(
    ".text\n"
    ".globl  NuFsqrt\n"
    ".ent    NuFsqrt\n"
    ".set    noreorder\n"
    ".set    noat\n"
    "NuFsqrt:\n"
    "    .word   0x460C0004              # sqrt.s $f0, $f12\n"
    "    jr      $31\n"
    "     nop\n"
    ".set    at\n"
    ".set    reorder\n"
    ".end    NuFsqrt\n");


__asm__(
    ".text\n"
    ".globl  NuFrsqrt\n"
    ".ent    NuFrsqrt\n"
    ".set    noreorder\n"
    ".set    noat\n"
    "NuFrsqrt:\n"
    "    rsqrt.s $f0, $f12, $f13\n"
    "    jr      $31\n"
    "     nop\n"
    ".set    at\n"
    ".set    reorder\n"
    ".end    NuFrsqrt\n");


__asm__(
    ".text\n"
    ".globl  NuFadda\n"
    ".ent    NuFadda\n"
    ".set    noreorder\n"
    ".set    noat\n"
    "NuFadda:\n"
    "    adda.s  $f12, $f13\n"
    "    jr      $31\n"
    "     nop\n"
    ".set    at\n"
    ".set    reorder\n"
    ".end    NuFadda\n");


__asm__(
    ".text\n"
    ".globl  NuFsuba\n"
    ".ent    NuFsuba\n"
    ".set    noreorder\n"
    ".set    noat\n"
    "NuFsuba:\n"
    "    suba.s  $f12, $f13\n"
    "    jr      $31\n"
    "     nop\n"
    ".set    at\n"
    ".set    reorder\n"
    ".end    NuFsuba\n");


__asm__(
    ".text\n"
    ".globl  NuFmula\n"
    ".ent    NuFmula\n"
    ".set    noreorder\n"
    ".set    noat\n"
    "NuFmula:\n"
    "    mula.s  $f12, $f13\n"
    "    jr      $31\n"
    "     nop\n"
    ".set    at\n"
    ".set    reorder\n"
    ".end    NuFmula\n");


__asm__(
    ".text\n"
    ".globl  NuFmadd\n"
    ".ent    NuFmadd\n"
    ".set    noreorder\n"
    ".set    noat\n"
    "NuFmadd:\n"
    "    madd.s  $f0, $f12, $f13\n"
    "    jr      $31\n"
    "     nop\n"
    ".set    at\n"
    ".set    reorder\n"
    ".end    NuFmadd\n");


__asm__(
    ".text\n"
    ".globl  NuFmadda\n"
    ".ent    NuFmadda\n"
    ".set    noreorder\n"
    ".set    noat\n"
    "NuFmadda:\n"
    "    madda.s $f12, $f13\n"
    "    jr      $31\n"
    "     nop\n"
    ".set    at\n"
    ".set    reorder\n"
    ".end    NuFmadda\n");


__asm__(
    ".text\n"
    ".globl  NuFmsuba\n"
    ".ent    NuFmsuba\n"
    ".set    noreorder\n"
    ".set    noat\n"
    "NuFmsuba:\n"
    "    msuba.s $f12, $f13\n"
    "    jr      $31\n"
    "     nop\n"
    ".set    at\n"
    ".set    reorder\n"
    ".end    NuFmsuba\n");
