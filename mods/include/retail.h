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

struct nuvec_s; /* defined in creature.h; only used by pointer below */

extern int Level;
extern int PLAYERCOUNT;             /* 0/1 flag: player exists this level */
extern unsigned short plr_wumpas;   /* HUD wumpa counter, player 0 */
extern int Paused;                  /* 0 = running; ramps 1..0x19 while in-game paused */
extern int VEHICLECONTROL;          /* 0 on foot / 1 riding vehicle / 2 swimming */
extern float vtog_time;             /* vehicle mount-transition timer (== vtog_duration when complete) */
extern float vtog_duration;         /* mount-transition length */

void UpdateLevel(void);   /* per-frame, gameplay levels only */
void DoInput(void);       /* per-frame, menus included */

/* 2D text in normalized screen space (game HUD/menu font); align 0=left,
 * 8=centre, 2=right. x,y ~ -1..1, z = 1.0, sx/sy/sz = scale. 0x00238280 */
void Text3D(char *text, int align, int colour, float x, float y, float z,
            float sx, float sy, float sz);
/* Project count world points (nuvec, stride 12) to screen space through the
 * global screen matrix when mtx is NULL. 0x00114048 */
void NuCameraTransformScreen(struct nuvec_s *dest, struct nuvec_s *src,
                             int count, void *mtx);
/* Transform count world points (nuvec, stride 12) into CAMERA/view space
 * through the global view matrix when mtx is NULL (affine, no perspective
 * divide). dest.z is the signed depth along the camera axis: its sign is the
 * front/back test and its magnitude is the true perspective distance. 0x00113D18 */
void NuCameraTransformView(struct nuvec_s *dest, struct nuvec_s *src,
                           int count, void *mtx);

#endif
