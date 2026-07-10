/* Retail symbols mod code uses; each is bound to its fixed address at
 * link time (gen_symbols.py). Grow as needed -- src/ is the ground truth
 * for signatures. Plain extern only: mod code is compiled -G0.
 *
 * Verify a global's SEMANTICS from the consuming asm before writing it,
 * not from its name: WUMPACOUNT is the live count of Wumpa[] entities
 * (blindly incrementing it made the engine walk past the array end and
 * corrupt memory at Crash's death); the HUD collected counter is
 * plr_wumpas, whose >=100 handling is a tolerant while-loop. */
#ifndef MODSDK_RETAIL_H
#define MODSDK_RETAIL_H

extern int Level;
extern int PLAYERCOUNT;             /* 0/1 flag: player exists this level */
extern unsigned short plr_wumpas;   /* HUD wumpa counter, player 0 */

void UpdateLevel(void);   /* per-frame, gameplay levels only */
void DoInput(void);       /* per-frame, menus included */

#endif
